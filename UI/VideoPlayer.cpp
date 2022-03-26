#include "VideoPlayer.h"
#include "ui_VideoPlayer.h"
#include <QPixmap>
#include <QGraphicsRectItem>
#include <QDebug>

VideoPlayer::VideoPlayer(Data::Document& document, QWidget* parent) :
	QWidget(parent),
	ui(new Ui::VideoPlayer),
	m_document(document),
	m_video(m_document.GetVideo()),
	m_pixmapDisplayer(),
	m_timer(this)
{
	ui->setupUi(this);
	ui->graphicsView->setScene(new QGraphicsScene(this));
	ui->graphicsView->scene()->addItem(&m_pixmapDisplayer);

	// Set the icons.

	//ui->firstFrameBtn->setFixedSize(100, 100);
	ui->firstFrameBtn->setIcon(QIcon(QPixmap(":/Resources/first_frame.png")));
	ui->previousFrameBtn->setIcon(QIcon(QPixmap(":/Resources/previous_frame.png")));
	ui->playBtn->setIcon(QIcon(QPixmap(":/Resources/play.png")));
	ui->nextFrameBtn->setIcon(QIcon(QPixmap(":/Resources/next_frame.png")));
	ui->lastFrameBtn->setIcon(QIcon(QPixmap(":/Resources/last_frame.png")));
	// ui->firstFrameBtn->setIconSize(QSize(16, 16));


	// Connect the UI buttons.
	connect(ui->playBtn, &QToolButton::clicked, this, &VideoPlayer::PlayBtnClicked);
	connect(ui->nextFrameBtn, &QToolButton::clicked, this, &VideoPlayer::GoToNextFrame);
	connect(ui->previousFrameBtn, &QToolButton::clicked, this, &VideoPlayer::GoToPreviousFrame);
	connect(ui->firstFrameBtn, &QToolButton::clicked, this, &VideoPlayer::GoToFirstFrame);
	connect(ui->lastFrameBtn, &QToolButton::clicked, this, &VideoPlayer::GoToLastFrame);

	// Connect the video-related events.
	connect(&m_video, &Data::Video::FrameChanged, this, &VideoPlayer::Render);
	connect(&m_video, &Data::Video::VideoLoaded, this, &VideoPlayer::OnVideoLoaded);

	// Connect the timer's timeout event to the ReadNextFrame function of the video object.
	connect(&m_timer, &QTimer::timeout, this, &VideoPlayer::TimerTick);

	// Forward the mouse click event. It is only used by the current tracking manager.
	connect(ui->graphicsView, &ScrollableGraphicsView::LeftClicked, this, &VideoPlayer::OnGraphicsViewClicked);

	CenterVideo();
}

VideoPlayer::~VideoPlayer()
{
	qDebug() << "VideoPlayer::~VideoPlayer()";
	delete ui;
}


void VideoPlayer::PlayBtnClicked()
{
	if(m_timer.isActive())
		Pause();
	else
		Play();
}

void VideoPlayer::Play()
{
	// Special case: we want to start playing from the last frame.
	// A handy behaviour for this case is to play from the start (instead
	// of doing nothing on the click).
	if (m_video.GetCurrentFrameIndex() == m_video.GetFrameCount() - 1)
		m_video.ReadFrameAtIndex(0);

	const int interval = static_cast<int>(1000.0f / static_cast<float>(m_video.GetFrameRate()));
	m_timer.start(interval);
	ui->playBtn->setIcon(QIcon(QPixmap(":/Resources/pause.png")));
}

void VideoPlayer::TimerTick()
{
	m_video.ReadNextFrame();

	// If we reach the end of the video, stop playing.
	if (m_video.GetCurrentFrameIndex() >= m_video.GetFrameCount() - 1)
	{
		Pause();
	}
	QApplication::processEvents();
}

void VideoPlayer::Pause()
{
	m_timer.stop();
	ui->playBtn->setIcon(QIcon(QPixmap(":/Resources/play.png")));
}

void VideoPlayer::GoToNextFrame()
{
	Pause();
	m_video.ReadNextFrame();
}

void VideoPlayer::GoToPreviousFrame()
{
	Pause();
	m_video.ReadFrameAtIndex(m_video.GetCurrentFrameIndex() - 1);
}

void VideoPlayer::GoToFirstFrame()
{
	Pause();
	m_video.ReadFrameAtIndex(0);
}

void VideoPlayer::GoToLastFrame()
{
	Pause();
	m_video.ReadFrameAtIndex(m_video.GetFrameCount() - 1);
}

void VideoPlayer::OnVideoLoaded()
{
	// 1. Enable the player's UI.
	ui->firstFrameBtn->setEnabled(true);
	ui->previousFrameBtn->setEnabled(true);
	ui->playBtn->setEnabled(true);
	ui->nextFrameBtn->setEnabled(true);
	ui->lastFrameBtn->setEnabled(true);

	// 2. Center the video.
	CenterVideo();
}

void VideoPlayer::CenterVideo()
{
	// 2. Center the video.
	const int width = m_video.GetWidth();
	const int height = m_video.GetHeight();
	const int controlWidth = ui->graphicsView->width();
	const int controlHeight = ui->graphicsView->height();

	const int xOrigin = controlWidth / 2 - width / 2;
	const int yOrigin = controlHeight / 2 - height / 2;
	m_pixmapDisplayer.setPos(xOrigin, yOrigin);

	ui->graphicsView->setSceneRect(0, 0, controlWidth, controlHeight);
	// ui->graphicsView->scene()->addRect(0, 0, controlWidth, controlHeight, QPen(Qt::red)); todo remove this comment
}

void VideoPlayer::OnGraphicsViewClicked(const QPointF& scenePosition)
{
	const QPoint imagePosition = ScenePosToImagePos(scenePosition).toPoint();
	if(imagePosition.x() >= 0 && imagePosition.x() < m_video.GetWidth()
		&& imagePosition.y() >= 0 && imagePosition.y() < m_video.GetHeight())
	{
		emit ImageClicked(imagePosition);
	}
}

QPointF VideoPlayer::ScenePosToImagePos(const QPointF& scenePosition) const
{
	const int width = m_video.GetWidth();
	const int height = m_video.GetHeight();
	const int controlWidth = ui->graphicsView->width();
	const int controlHeight = ui->graphicsView->height();
	const int xOrigin = controlWidth / 2 - width / 2;
	const int yOrigin = controlHeight / 2 - height / 2;
	return {scenePosition.x() - xOrigin, scenePosition.y() - yOrigin};
}

void VideoPlayer::Render(const int currentFrame)
{
	// 1. Get the image from the video, and perform some additional drawing on top of it (keyframes etc.).
	const cv::Mat& currentVideoImage = m_video.GetCurrentImage();

	// 2. Convert the image to Qt.
	const QImage image(currentVideoImage.data, currentVideoImage.cols, currentVideoImage.rows, static_cast<int>(currentVideoImage.step), QImage::Format_BGR888);
	QPixmap pixmap;
	pixmap.convertFromImage(image);

	// 3. Perform some additional drawing on the Qt image (draw the motion trails).
	DrawMotionTrails(pixmap, currentFrame);

	// 4. Put the image
	m_pixmapDisplayer.setPixmap(pixmap);
}


void VideoPlayer::DrawMotionTrails(QPixmap& pixmap, const int currentFrame)
{
	QPainter painter(&pixmap);
	painter.setRenderHint(QPainter::Antialiasing);

	const std::vector<std::unique_ptr<Data::TrackedPoint>>& trackedPoints = m_document.GetTrackedPoints();

	for(const std::unique_ptr<Data::TrackedPoint>& trackedPoint : trackedPoints)
	{
		if (!trackedPoint->IsVisibleInViewport())
			continue;

		Data::Keyframe keyframe;
		if(trackedPoint->GetKeyframe(currentFrame, keyframe))
		{
			painter.setPen(QPen(trackedPoint->GetColor()));
			painter.drawEllipse(keyframe.position, 5, 5);
		}
	}


	//for(int i = -m_document.GetTrailLength().left; i < m_document.GetTrailLength().right; i++)
	//{
	//	const int clampedIndex = std::clamp(i, 0, m_video.GetFrameCount() - 1);
	//}
}

void VideoPlayer::resizeEvent(QResizeEvent* event)
{
	QWidget::resizeEvent(event);
	
	CenterVideo();
}

