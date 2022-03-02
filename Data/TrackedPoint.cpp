#include "TrackedPoint.h"

namespace Data
{
	TrackedPoint::TrackedPoint(QString name, const int pointIndex) :
		m_name(std::move(name)),
		m_keyframes(),
		m_color(),
		m_index(pointIndex),
		m_showInViewport()
	{
	}

	void TrackedPoint::AddKeyframe(const Keyframe& keyframe)
	{
		m_keyframes[keyframe.frameIndex] = keyframe;
	}

	Keyframe& TrackedPoint::GetKeyframe(const int index)
	{
		return m_keyframes[index];
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
	}
}
