#include "Document.h"

#include <QFile>
#include <QDebug>
#include <QDataStream>

namespace
{
	constexpr int32_t DataVersion = 100; // 1.0.0
	constexpr int32_t MagicNumber = 0x12ab8fa1; // 1.0.0
}

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

	void Document::Save(const SaveAsCallback& saveAsCallback, const bool saveAs)
	{
		// 1. If no path is currently set, try getting one. If this fails, do not save
		// the document.
		if (!m_filePath.has_value() || saveAs)
		{
			m_filePath = saveAsCallback();
			if (!m_filePath.has_value())
			{
				return;
			}
		}

		// 2. Actual saving logic.
		SaveImpl();

		// Once everything is saved, mark the document as non dirty.
		m_dirty = false;
		DocumentDirtinessChanged();
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
		LoadImpl(filePath);

		// Note: The current file path is set by LoadImpl, if loading succeeded.
	}

	TrackedPoint& Document::CreateTrackedPoint()
	{
		const int pointIndex = m_trackedPoints.size();
		const QString pointName = "Point " + QString::number(pointIndex + 1);
		return CreateTrackedPoint(pointName, pointIndex);
	}

	TrackedPoint& Document::CreateTrackedPoint(QString name, const int index)
	{
		m_trackedPoints.push_back(TrackedPoint(std::move(name), index));
		MarkDirty();
		emit TrackedPointAdded(m_trackedPoints.last());
		return m_trackedPoints.last();
	}

	void Document::RemoveTrackedPoint(const int index)
	{
		m_trackedPoints.removeAt(index);
		MarkDirty();
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
		MarkDirty();
	}

	Video& Document::GetVideo()
	{
		return m_video;
	}

	bool Document::IsDirty() const
	{
		return m_dirty;
	}

	void Document::MarkDirty()
	{
		m_dirty = true;
		emit DocumentDirtinessChanged();
	}

	std::optional<QString> Document::GetFilePath() const
	{
		return m_filePath;
	}

	void Document::SaveImpl() const
	{
		if (!m_filePath.has_value())
			throw std::runtime_error("Document::SaveImpl called but m_filePath has no value.");

		/* FILE STRUCTURE.
		 *
		 * [int] Data Version.
		 * [int] Test integer.
		 * [int] Trail length (left).
		 * [int] Trail length (right).
		 * [int] Number of tracked points.
		 * [lst] List of the tracked points, serialized.
		 * [int] Number of active point indices.
		 * [lst] List of the indices of the active points.
		 */

		QFile file(m_filePath.value());
		file.open(QIODevice::WriteOnly);
		QDataStream out(&file);

		constexpr int testInteger = 31;

		// This function uses a lot of "useless" casts that are used only to
		// clearly show the size/type of data being written at each line.
		// It would also enable users to change the type of the field being
		// written without breaking compatibility with old saves.

		// Save the header.
		out << static_cast<int32_t>(DataVersion);
		out << static_cast<int32_t>(MagicNumber);
		out << static_cast<int32_t>(testInteger); // The test integer is also saved at the end. When reading a save, both integers must match, or we know we have an issue.

		// Save the trail lengths.
		out << m_trailLength.left;
		out << m_trailLength.right;

		// Save the tracked points.
		out << static_cast<int32_t>(m_trackedPoints.size());
		for (const TrackedPoint& trackedPoint : m_trackedPoints)
		{
			trackedPoint.Save(out);
		}

		// Save the active point indices.
		out << m_activePointIndices.size();
		for (const int& index : m_activePointIndices)
		{
			out << static_cast<int32_t>(index);
		}

		// Save the video file path.
		out << m_video.GetFilePath();

		// Save again the test integer.
		out << static_cast<int32_t>(testInteger);
	}

	void Document::LoadImpl(const QString& path)
	{
		QFile file(path);
		file.open(QIODevice::ReadOnly);
		QDataStream in(&file);

		m_filePath = std::nullopt;
		m_activePointIndices.clear();
		m_trackedPoints.clear();

		// Load the header.
		int32_t dataVersion;
		in >> dataVersion;
		if (dataVersion != DataVersion)
			qWarning() << "The file was saved with a different version of the software.";

		int32_t magicNumber;
		in >> magicNumber;
		if (magicNumber != MagicNumber)
			throw WrongFormatException();

		int32_t testInteger1;
		in >> testInteger1;

		// Load the trail lenghts.
		in >> m_trailLength.left;
		in >> m_trailLength.right;

		// Load the tracked points.
		int32_t trackedPointsCount;
		in >> trackedPointsCount;
		m_trackedPoints.reserve(trackedPointsCount);
		for (int i = 0; i < trackedPointsCount; i++)
		{
			QString tpName;
			in >> tpName;
			int32_t tpIndex;
			in >> tpIndex;
			TrackedPoint& trackedPoint = CreateTrackedPoint(tpName, tpIndex);
			trackedPoint.Load(in);
		}

		// Load the active indices.
		int32_t activeIndicesCount;
		in >> activeIndicesCount;
		for (int i = 0; i < activeIndicesCount; i++)
		{
			int32_t index;
			in >> index;
			m_activePointIndices.push_back(index);
		}

		// Load the video.
		QString videoFilePath;
		in >> videoFilePath;

		// Re-read the test integer.
		int32_t testInteger2;
		in >> testInteger2;
		if (testInteger1 != testInteger2)
			throw WrongTestIntegerException();

		// Load the video.
		m_video.LoadFromFile(videoFilePath);
		m_filePath = path;

		m_dirty = false;
		emit DocumentDirtinessChanged();
	}

	void Document::ClearDocument()
	{
		QMutableVectorIterator<TrackedPoint> trptIt(m_trackedPoints);
		while(trptIt.hasNext())
		{
			trptIt.remove();
		}


	}
}
