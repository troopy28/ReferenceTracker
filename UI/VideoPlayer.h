#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QWidget>
#include <QGraphicsPixmapItem>


namespace Ui {
	class VideoPlayer;
}

class VideoPlayer : public QWidget
{
	Q_OBJECT

public:
	explicit VideoPlayer(QWidget* parent = nullptr);
	~VideoPlayer() override;
	Q_DISABLE_COPY_MOVE(VideoPlayer);

private:
	Ui::VideoPlayer* ui;
	QGraphicsPixmapItem m_pixmap;
};

#endif // VIDEOPLAYER_H
