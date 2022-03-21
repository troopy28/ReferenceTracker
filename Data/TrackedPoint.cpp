#include "TrackedPoint.h"
#include <QDataStream>

namespace Data
{
	TrackedPoint::TrackedPoint(QString name, const int pointIndex) :
		m_name(std::move(name)),
		m_keyframes(),
		m_color(static_cast<Qt::GlobalColor>(static_cast<int>(Qt::black) + ((pointIndex + 5) % 13))),
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

	const QHash<int, Keyframe>& TrackedPoint::GetKeyframes() const
	{
		return m_keyframes;
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

	void TrackedPoint::Save(QDataStream& out) const
	{
		out << m_name;
		out << static_cast<int32_t>(m_index); // Cast for clarity, but "useless".
		out << m_color;
		out << m_showInViewport;

		out << m_keyframes.size();
		for (const int key : m_keyframes.keys())
		{
			out << static_cast<int32_t>(key); // Cast for clarity, but "useless".
			const auto& [position, frameIndex] = m_keyframes[key];
			out << static_cast<int32_t>(frameIndex); // Again.
			out << position;
		}
	}

	void TrackedPoint::Load(QDataStream& in)
	{
		// The name and index are loaded by the document.

		in >> m_color;
		emit ColorChanged(m_color);
		in >> m_showInViewport;
		VisibilityChanged(m_showInViewport);

		int32_t keyframesCount;
		in >> keyframesCount;
		m_keyframes.reserve(keyframesCount);
		for(int i = 0; i < keyframesCount; i++)
		{
			int32_t key;
			in >> key;

			Keyframe keyframe;
			in >> keyframe.frameIndex;
			in >> keyframe.position;
			m_keyframes[key] = keyframe;
		}

	}
}
