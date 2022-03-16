#include "VideoPlayer.h"
#include "ui_VideoPlayer.h"
#include <QPixmap>

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

	// Connect the UI buttons.
	connect(ui->playBtn, &QPushButton::clicked, this, &VideoPlayer::Play);
	connect(ui->pauseBtn, &QPushButton::clicked, this, &VideoPlayer::Pause);
	connect(ui->nextFrameBtn, &QPushButton::clicked, this, &VideoPlayer::GoToNextFrame);
	connect(ui->goToPreviousFrameBtn, &QPushButton::clicked, this, &VideoPlayer::GoToPreviousFrame);
	connect(ui->firstFrameBtn, &QPushButton::clicked, this, &VideoPlayer::GoToFirstFrame);
	connect(ui->lastFrameBtn, &QPushButton::clicked, this, &VideoPlayer::GoToLastFrame);

	// Connect the video display event.
	connect(&m_video, &Data::Video::FrameChanged, this, &VideoPlayer::Render);

	// Connect the timer's timeout event to the ReadNextFrame function of the video object.
	connect(&m_timer, &QTimer::timeout, this, &VideoPlayer::TimerTick);
}

VideoPlayer::~VideoPlayer()
{
	delete ui;
}

void VideoPlayer::Play()
{
	const int interval = static_cast<int>(1000.0f / static_cast<float>(m_video.GetFrameRate()));
	m_timer.start(interval);
}

void VideoPlayer::TimerTick()
{
	m_video.ReadNextFrame();

	// If we reach the end of the video, stop playing.
	if(m_video.GetCurrentFrameIndex() >= m_video.GetFrameCount())
	{
		m_timer.stop();
	}
	QApplication::processEvents();
}

void VideoPlayer::Pause()
{
	m_timer.stop();
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
