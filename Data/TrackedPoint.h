#pragma once

#include <QVector2D>
#include <QColor>
#include <QHash>

namespace Data
{

	struct Keyframe
	{
		QVector2D position{0.0f, 0.0f};
		int frameIndex{0};
	};

	/**
	 * \brief Represents a point that can be tracked on the image.
	 * It stores the name of the point, as well as the different positions
	 * it takes in the different frames of the video.
	 */
	class TrackedPoint
	{
	public:
		TrackedPoint(QString name, int pointIndex);

		void AddKeyframe(const Keyframe& keyframe);
		Keyframe& GetKeyframe(int index);

		_NODISCARD int GetPointIndex() const;
		_NODISCARD bool IsVisibleInViewport() const;
		void SetVisibleInViewport(bool visible);

	private:
		QString m_name;
		QHash<int, Keyframe> m_keyframes;
		QColor m_color;
		int m_index;
		bool m_showInViewport;
	};
}
