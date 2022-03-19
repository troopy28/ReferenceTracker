#include "TrackedPoint.h"

namespace Data
{
	TrackedPoint::TrackedPoint(QString name, const int pointIndex) :
		m_name(std::move(name)),
		m_keyframes(),
		m_color(static_cast<Qt::GlobalColor>(static_cast<int>(Qt::black) + (pointIndex + 5 % 13))),
		m_index(pointIndex),
		m_showInViewport(true)
	{
	}

	TrackedPoint::TrackedPoint(const TrackedPoint& other) :
		m_name(other.m_name),
		m_keyframes(other.m_keyframes),
		m_color(other.m_color),
		m_index(other.m_index),
		m_showInViewport(other.m_showInViewport)
	{
	}

	TrackedPoint::TrackedPoint(TrackedPoint&& other) noexcept :
		m_name(std::move(other.m_name)),
		m_keyframes(std::move(other.m_keyframes)),
		m_color(std::move(other.m_color)),
		m_index(other.m_index),
		m_showInViewport(other.m_showInViewport)
	{
	}

	TrackedPoint& TrackedPoint::operator=(const TrackedPoint& other)
	{
		m_name = other.m_name;
		m_keyframes = other.m_keyframes;
		m_color = other.m_color;
		m_index = other.m_index;
		m_showInViewport = other.m_showInViewport;
		return *this;
	}

	TrackedPoint& TrackedPoint::operator=(TrackedPoint&& other) noexcept
	{
		m_name = std::move(other.m_name);
		m_keyframes = std::move(other.m_keyframes);
		m_color = std::move(other.m_color);
		m_index = other.m_index;
		m_showInViewport = other.m_showInViewport;
		return *this;
	}

	void TrackedPoint::AddKeyframe(const Keyframe& keyframe)
	{
		m_keyframes[keyframe.frameIndex] = keyframe;
	}

	Keyframe& TrackedPoint::GetKeyframe(const int index)
	{
		return m_keyframes[index];
	}

	const QColor& TrackedPoint::GetColor() const
	{
		return m_color;
	}

	void TrackedPoint::SetColor(const QColor& color)
	{
		m_color = color;
		emit ColorChanged(m_color);
	}

	void TrackedPoint::SetName(const QString& name)
	{
		m_name = name;
	}

	const QString& TrackedPoint::GetName() const
	{
		return m_name;
	}

	int TrackedPoint::GetPointIndex() const
	{
		return m_index;
	}

	bool TrackedPoint::IsVisibleInViewport() const
	{
		return m_showInViewport;
	}

	void TrackedPoint::SetVisibleInViewport(const bool visible)
	{
		m_showInViewport = visible;
		emit VisibilityChanged(m_showInViewport);
	}
}
