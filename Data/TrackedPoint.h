#pragma once

#include "../common.h"
#include <memory>
#include <QVector2D>
#include <QColor>
#include <QMap>
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
		QPoint position{ 0, 0 };
		/**
		 * \brief Index at which the point is at this position.
		 */
		int frameIndex{ 0 };
	};

	class NoKeyframeFoundException final : public std::exception
	{
	public:
		NoKeyframeFoundException(const QString& trackedPointName, const int frameIndex) :
			std::exception(),
			m_customMessage((QString("Could not find any keyframe for the point ") + trackedPointName + " at frame " + QString::number(frameIndex)).toStdString() + " or before.")
		{
		}

		_NODISCARD char const* what() const noexcept override
		{
			return m_customMessage.c_str();
		}

	private:
		std::string m_customMessage;
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
		// TrackedPoint(); // Necessary for QVector.....
		TrackedPoint(QString name, int pointIndex);
		Q_DISABLE_COPY(TrackedPoint);
		// TrackedPoint(const TrackedPoint& other);
		TrackedPoint(TrackedPoint&& other) noexcept;
		~TrackedPoint() override = default;

		// TrackedPoint& operator=(const TrackedPoint& other);
		TrackedPoint& operator=(TrackedPoint&& other) noexcept;

		void AddKeyframe(const Keyframe& keyframe);
		/**
		 * \brief Tries returning a keyframe at the given index.
		 * \param index Index of the keyframe.
		 * \param keyframe Return param for the keyframe.
		 * \return Whether there is a keyframe.
		 */
		_NODISCARD bool GetKeyframe(int index, Keyframe& keyframe);
		/**
		 * \brief Used to return the first keyframe that can be found starting from the given
		 * index, and going backwards. This is used for instance by trackers to know which
		 * position to use to start tracking.
		 * If no keyframe is found, there will be a 
		 * \param index Index of the first frame to try.
		 * \return Return the first found keyframe.
		 */
		const Keyframe& GetLastKeyframe(int index);
		_NODISCARD const QMap<int, Keyframe>& GetKeyframes() const;

		void ClearKeyframes();

		_NODISCARD const QColor& GetColor() const;
		void SetColor(const QColor& color);

		void SetName(const QString& name);
		_NODISCARD const QString& GetName() const;

		void SetPointIndex(int index);
		_NODISCARD int GetPointIndex() const;

		_NODISCARD bool IsVisibleInViewport() const;
		void SetVisibleInViewport(bool visible);

		void Save(QDataStream& out) const;
		void Load(QDataStream& in);

		/**
		 * \brief Explicit copy method. Way way WAY less error-prone than copy constructors and all.
		 * \return Copy of this tracked point.
		 */
		_NODISCARD std::unique_ptr<TrackedPoint> GetCopy() const;

	signals:
		void ColorChanged(const QColor& color);
		void VisibilityChanged(const bool& visible);
		void KeyframesChanged(QList<Keyframe> keyframes);

	private:
		/**
		 * \brief Name of this tracked point, as displayed in the UI. There
		 * is no requirement that a tracked point's name must be unique.
		 */
		QString m_name;
		/**
		 * \brief Map of the different keyframes of this tracked point.
		 * Key: frame index. Value: the keyframe.
		 * Use a map and not a hash because we're often interested in getting the
		 * SORTED keys.
		 * This structure allows to have a sparse storage of the keyframes.
		 * This saves memory for large videos.
		 */
		QMap<int, Keyframe> m_keyframes;
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
