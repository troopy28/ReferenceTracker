#include "Document.h"

#include <QFile>
#include <QDebug>

namespace Data
{
	Document::Document() :
		m_filePath(),
		m_dirty(false),
		m_trackedPoints(),
		m_video(),
		m_trailLength(),
		m_activePointIndices()
	{
	}

	Document::Document(Document&& other) noexcept :
		m_filePath(std::move(other.m_filePath)),
		m_dirty(other.m_dirty),
		m_trackedPoints(std::move(other.m_trackedPoints)),
		m_video(std::move(other.m_video)),
		m_trailLength(other.m_trailLength),
		m_activePointIndices(std::move(other.m_activePointIndices))
	{
	}

	Document& Document::operator=(Document&& other) noexcept
	{
		m_filePath = std::move(other.m_filePath);
		m_dirty = other.m_dirty;
		m_trackedPoints = std::move(other.m_trackedPoints);
		m_video = std::move(other.m_video);
		m_trailLength = other.m_trailLength;
		m_activePointIndices = std::move(other.m_activePointIndices);
		return *this;
	}

	void Document::Save(const SaveAsCallback& saveAsCallback)
	{
		// 1. If no path is currently set, try getting one. If this fails, do not save
		// the document.
		if(!m_filePath.has_value())
		{
			m_filePath = saveAsCallback();
			if (!m_filePath.has_value())
			{
				return;
			}
		}

		// 2. Actual saving logic.
		qWarning() << "todo"; // TODO
	}

	void Document::LoadFromFile(const QString& filePath)
	{
		// 1. Before changing anything, ensure the file at the specified path exists.
		if (!QFile::exists(filePath))
		{
			qWarning() << "Document at" << filePath << "does not exist.";
			return;
		}

		// 2. Load the file.
		qWarning() << "todo"; // TODO

		// 3. Set the current file path, if loading succeeded.
		m_filePath = filePath;
	}

	TrackedPoint& Document::CreateTrackedPoint()
	{
		qDebug() << "Document::CreateTrackedPoint()";
		const int pointIndex = m_trackedPoints.size() + 1;
		const QString pointName = "Point " + QString::number(pointIndex + 1);
		m_trackedPoints.push_back(TrackedPoint(pointName, pointIndex));
		emit TrackedPointAdded(m_trackedPoints.last());
		return m_trackedPoints.last();
	}

	void Document::RemoveTrackedPoint(const int index)
	{
		qDebug() << "Document::RemoveTrackedPoint()";
		m_trackedPoints.removeAt(index);
		emit TrackedPointRemoved(index);
	}

	const QVector<TrackedPoint>& Document::GetTrackedPoints() const
	{
		return m_trackedPoints;
	}

	const TrailLength& Document::GetTrailLength() const
	{
		return m_trailLength;
	}

	const QVector<int>& Document::GetActivePointIndices() const
	{
		return m_activePointIndices;
	}

	void Document::SetActive(const TrackedPoint& point, const bool active)
	{
		if (active)
			m_activePointIndices.push_back(point.GetPointIndex());
		else
			m_activePointIndices.removeOne(point.GetPointIndex());
	}

	Video& Document::GetVideo()
	{
		return m_video;
	}
}
