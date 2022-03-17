#include "ScrollableGraphicsView.h"

#include <QWheelEvent>
#include <QTimeLine>

ScrollableGraphicsView::ScrollableGraphicsView(QWidget* parent) :
	QGraphicsView(parent),
	m_scheduledScalings(0),
	m_currentScaleFactor(1.0)
{
	setTransformationAnchor(AnchorUnderMouse);
	setDragMode(QGraphicsView::RubberBandDrag);
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

void ScrollableGraphicsView::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::MiddleButton)
	{
		// If there's a rubber band already started it doesn't autmatically get cleared when we switch to
		// scroll hand mode. We should probably keep track of things properly but it seems to work if you just do this.
		// I'm not sure why buttons has to be 0 here - if you just clear the left button it doesn't work.
		QMouseEvent releaseEvent(QEvent::MouseButtonRelease, event->localPos(), event->screenPos(), event->windowPos(),
			Qt::LeftButton, 0, event->modifiers());
		QGraphicsView::mouseReleaseEvent(&releaseEvent);

		setDragMode(QGraphicsView::ScrollHandDrag);
		// We need to pretend it is actually the left button that was pressed!
		QMouseEvent fakeEvent(event->type(), event->localPos(), event->screenPos(), event->windowPos(),
			Qt::LeftButton, event->buttons() | Qt::LeftButton, event->modifiers());
		QGraphicsView::mousePressEvent(&fakeEvent);
	}
	else
	{
		QGraphicsView::mousePressEvent(event);
	}
}

void ScrollableGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
	QGraphicsView::mouseMoveEvent(event);
}

void ScrollableGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::MiddleButton)
	{
		QMouseEvent fakeEvent(event->type(), event->localPos(), event->screenPos(), event->windowPos(),
			Qt::LeftButton, event->buttons() & ~Qt::LeftButton, event->modifiers());
		QGraphicsView::mouseReleaseEvent(&fakeEvent);
		setDragMode(QGraphicsView::RubberBandDrag);
	}
	else
	{
		QGraphicsView::mouseReleaseEvent(event);
	}
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
