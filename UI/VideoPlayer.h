#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include "../common.h"
#include <QWidget>
#include <QGraphicsPixmapItem>
#include "../Data/Document.h"
#include <QTimer>

namespace Ui {
	class VideoPlayer;
}

class VideoPlayer : public QWidget
{
	Q_OBJECT

public:
	explicit VideoPlayer(Data::Document& document, QWidget* parent = nullptr);
	~VideoPlayer() override;
	Q_DISABLE_COPY_MOVE(VideoPlayer);

	void Render(int currentFrame);

	void resizeEvent(QResizeEvent* event) override;

signals:
	void ImageClicked(const QPointF& imagePos);

private:
	void DrawMotionTrails(QPixmap& pixmap, int currentFrame);

	void PlayBtnClicked();
	void Play();
	void Pause();
	void TimerTick();
	void GoToNextFrame();
	void GoToPreviousFrame();
	void GoToFirstFrame();
	void GoToLastFrame();

	void OnVideoLoaded();
	void CenterVideo();
	void OnGraphicsViewClicked(const QPointF& scenePosition);

	_NODISCARD QPointF ScenePosToImagePos(const QPointF& scenePosition) const;

	Ui::VideoPlayer* ui;
	Data::Document& m_document;
	Data::Video& m_video;
	QGraphicsPixmapItem m_pixmapDisplayer;
	QTimer m_timer;
};

#endif // VIDEOPLAYER_H
