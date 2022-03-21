#include "VideoPlayer.h"
#include "ui_VideoPlayer.h"
#include <QPixmap>
#include <QGraphicsRectItem>
#include <QPen>
#include <QDebug>

VideoPlayer::VideoPlayer(Data::Video& video, QWidget* parent) :
	QWidget(parent),
	ui(new Ui::VideoPlayer),
	m_video(video),
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
	const int interval = static_cast<int>(1000.0f / static_cast<float>(m_video.GetFrameRate()));
	m_timer.start(interval);
	ui->playBtn->setIcon(QIcon(QPixmap(":/Resources/pause.png")));
}

void VideoPlayer::TimerTick()
{
	m_video.ReadNextFrame();

	// If we reach the end of the video, stop playing.
	if (m_video.GetCurrentFrameIndex() >= m_video.GetFrameCount())
	{
		m_timer.stop();
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
	const int height = m_video.GetHeigth();
	const int controlWidth = ui->graphicsView->width();
	const int controlHeight = ui->graphicsView->height();

	const int xCenter = controlWidth / 2 - width / 2;
	const int yCenter = controlHeight / 2 - height / 2;
	m_pixmapDisplayer.setPos(xCenter, yCenter);

	ui->graphicsView->setSceneRect(0, 0, controlWidth, controlHeight);
	// ui->graphicsView->scene()->addRect(0, 0, controlWidth, controlHeight, QPen(Qt::red)); todo remove this comment
}


void VideoPlayer::Render(const int)
{
	// 1. Get the image from the video, and perform some additional drawing on top of it (keyframes etc.).
	const cv::Mat& currentVideoImage = m_video.GetCurrentImage();
	// todo: draw the keyframes on top of the video here.


	// 2. Put the image in the pixmap displayer.
	const QImage image(currentVideoImage.data, currentVideoImage.cols, currentVideoImage.rows, static_cast<int>(currentVideoImage.step), QImage::Format_BGR888);
	QPixmap pixmap;
	pixmap.convertFromImage(image);
	m_pixmapDisplayer.setPixmap(pixmap);
}

void VideoPlayer::resizeEvent(QResizeEvent* event)
{
	QWidget::resizeEvent(event);

	CenterVideo();
}

