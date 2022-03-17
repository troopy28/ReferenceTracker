#include "ScrollableGraphicsView.h"

#include <QWheelEvent>
#include <QTimeLine>

ScrollableGraphicsView::ScrollableGraphicsView(QWidget* parent) :
	QGraphicsView(parent),
	m_scheduledScalings(0),
	m_currentScaleFactor(1.0)
{
}

void ScrollableGraphicsView::wheelEvent(QWheelEvent* evt)
{
	const int numDegrees = evt->angleDelta().y() / 8;
	const int numSteps = numDegrees / 10; // /15

	m_scheduledScalings += numSteps;
	if (m_scheduledScalings * numSteps < 0) // if user moved the wheel in another direction, we reset previously scheduled scalings
		m_scheduledScalings = numSteps;

	QTimeLine* anim = new QTimeLine(200, this);
	anim->setUpdateInterval(20);

	connect(anim, &QTimeLine::valueChanged, this, &ScrollableGraphicsView::ScalingTime);
	connect(anim, &QTimeLine::finished, this, &ScrollableGraphicsView::AnimFinished);
	anim->start();

	evt->accept();
}

void ScrollableGraphicsView::ScalingTime(double x)
{
	constexpr double minimumSizeFactor = 0.1;

	if((m_currentScaleFactor > minimumSizeFactor && m_scheduledScalings < 0)
		|| m_scheduledScalings > 0)
	{
		const double factor = 1.0 + static_cast<double>(m_scheduledScalings) / 300.0;
		scale(factor, factor);
		m_currentScaleFactor *= factor;
	}
}

void ScrollableGraphicsView::AnimFinished()
{
	if (m_scheduledScalings > 0)
		m_scheduledScalings--;
	else
		m_scheduledScalings++;
	sender()->~QObject();
}
