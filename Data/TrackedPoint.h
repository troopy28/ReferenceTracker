#pragma once

#include <QVector2D>
#include <QColor>
#include <QHash>
#include <QObject>

namespace Data
{

	/**
	 * \brief A keyframe stores the position of a point on the screen
	 *  at a given point in time.
	 */
	struct Keyframe
	{
		/**
		 * \brief Position of the point on the screen.
		 */
		QVector2D position{ 0.0f, 0.0f };
		/**
		 * \brief Index at which the point is at this position.
		 */
		int frameIndex{ 0 };
	};

	/**
	 * \brief Represents a point that can be tracked on the image.
	 * It stores the name of the point, as well as the different positions
	 * it takes in the different frames of the video.
	 */
	class TrackedPoint : public QObject
	{
		Q_OBJECT

	public:
		TrackedPoint(QString name, int pointIndex);
		TrackedPoint(const TrackedPoint& other);
		TrackedPoint(TrackedPoint&& other) noexcept;
		~TrackedPoint() override = default;

		TrackedPoint& operator=(const TrackedPoint& other);
		TrackedPoint& operator=(TrackedPoint&& other) noexcept;

		void AddKeyframe(const Keyframe& keyframe);
		Keyframe& GetKeyframe(int index);
		_NODISCARD const QHash<int, Keyframe>& GetKeyframes() const;

		_NODISCARD const QColor& GetColor() const;
		void SetColor(const QColor& color);

		void SetName(const QString& name);
		_NODISCARD const QString& GetName() const;

		_NODISCARD int GetPointIndex() const;
		_NODISCARD bool IsVisibleInViewport() const;
		void SetVisibleInViewport(bool visible);

		void Save(QDataStream& out) const;
		void Load(QDataStream& in);

	signals:
		void ColorChanged(const QColor& color);
		void VisibilityChanged(const bool& visible);

	private:
		/**
		 * \brief Name of this tracked point, as displayed in the UI. There
		 * is no requirement that a tracked point's name must be unique.
		 */
		QString m_name;
		/**
		 * \brief Map of the different keyframes of this tracked point.
		 * Key: frame index. Value: the keyframe.
		 * This structure allows to have a sparse storage of the keyframes.
		 * This saves memory for large videos.
		 */
		QHash<int, Keyframe> m_keyframes;
		/**
		 * \brief Color of the tracked point in the UI.
		 */
		QColor m_color;
		/**
		 * \brief Index of the tracked point in the tracked points list of
		 * the document.
		 */
		int m_index;
		/**
		 * \brief Whether to show this point in the UI (curve displayer and
		 * video player).
		 */
		bool m_showInViewport;
	};
}
