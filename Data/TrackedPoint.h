#pragma once

#include <QVector2D>
#include <QColor>
#include <QHash>
#include <QObject>

namespace Data
{

	struct Keyframe
	{
		QVector2D position{ 0.0f, 0.0f };
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
		QString m_name;
		QHash<int, Keyframe> m_keyframes;
		QColor m_color;
		int m_index;
		bool m_showInViewport;
	};
}
