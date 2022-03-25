#include "TrackedPoint.h"
#include <QDataStream>
#include <QDebug>

namespace Data
{
#if 0
	TrackedPoint::TrackedPoint() :
		m_name(),
		m_keyframes(),
		m_color(Qt::red),
		m_index(-1),
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
		qDebug() << "Point copied";
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

#endif

	TrackedPoint::TrackedPoint(QString name, const int pointIndex) :
		m_name(std::move(name)),
		m_keyframes(),
		m_color(static_cast<Qt::GlobalColor>(static_cast<int>(Qt::black) + ((pointIndex + 5) % 13))),
		m_index(pointIndex),
		m_showInViewport(true)
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
		emit KeyframesChanged(m_keyframes.values());
		qDebug() << "Added keyframe at frame " << keyframe.frameIndex << " at position " << keyframe.position;
	}

	const Keyframe& TrackedPoint::GetKeyframe(const int index)
	{
		const auto it = m_keyframes.find(index);
		if (it != m_keyframes.end())
			return it.value();
		throw NoKeyframeFoundException(m_name, index);
	}

	const Keyframe& TrackedPoint::GetLastKeyframe(const int index)
	{
		int currentIndex = index;
		while(currentIndex >= 0)
		{
			const auto it = m_keyframes.find(currentIndex);
			if (it != m_keyframes.end())
				return it.value();
			currentIndex--;
		}
		throw NoKeyframeFoundException(m_name, index);
	}

	const QMap<int, Keyframe>& TrackedPoint::GetKeyframes() const
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

	void TrackedPoint::SetPointIndex(const int index)
	{
		m_index = index;
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

	std::unique_ptr<TrackedPoint> TrackedPoint::GetCopy() const
	{
		std::unique_ptr<TrackedPoint> point = std::make_unique<TrackedPoint>(m_name, m_index);
		point->m_color = m_color;
		point->m_keyframes = m_keyframes; // note: QMap is copy on write
		point->m_showInViewport = m_showInViewport;
		return point;
	}
}
