#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QWidget>
#include <QGraphicsPixmapItem>
#include "../Data/Video.h"
#include <QTimer>

namespace Ui {
	class VideoPlayer;
}

class VideoPlayer : public QWidget
{
	Q_OBJECT

public:
	explicit VideoPlayer(Data::Video& video, QWidget* parent = nullptr);
	~VideoPlayer() override;
	Q_DISABLE_COPY_MOVE(VideoPlayer);


private:
	void Play();
	void TimerTick();
	void Pause();
	void GoToNextFrame();
	void GoToPreviousFrame();
	void GoToFirstFrame();
	void GoToLastFrame();

	void Render(int currentFrame);


	Ui::VideoPlayer* ui;
	Data::Video& m_video;
	QGraphicsPixmapItem m_pixmapDisplayer;
	QTimer m_timer;
};

#endif // VIDEOPLAYER_H
