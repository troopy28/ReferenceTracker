#pragma once

#include <QGraphicsView>

class ScrollableGraphicsView : public QGraphicsView
{
	Q_OBJECT

public:
	ScrollableGraphicsView(QWidget* parent = nullptr);

public slots:
	void ScalingTime(double x);
	void AnimFinished();

protected:
	void wheelEvent(QWheelEvent* evt) override;

private:
	int m_scheduledScalings;
	double m_currentScaleFactor;
};
