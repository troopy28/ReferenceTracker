#pragma once

#include <QGraphicsView>

class ScrollableGraphicsView : public QGraphicsView
{
	Q_OBJECT

public:
	explicit ScrollableGraphicsView(QWidget* parent = nullptr);
	~ScrollableGraphicsView() override = default;
	Q_DISABLE_COPY_MOVE(ScrollableGraphicsView);

signals:
	void LeftClicked(const QPointF& position);

public slots:
	void ScalingTime(double x);
	void AnimFinished();

protected:
	void wheelEvent(QWheelEvent* evt) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;

private:
	void HandleSingleClick(const QMouseEvent* evt);

	int m_scheduledScalings;
	double m_currentScaleFactor;
};
