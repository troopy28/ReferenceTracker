#pragma once

#include <optional>
#include "TrackedPoint.h"
#include "Video.h"

namespace Data
{
	struct TrailLength
	{
		int left{ 0 };
		int right{ 0 };
	};

	/**
	 * \brief Main class that stores all the data related to a project.
	 * It also includes the file IO system.
	 */
	class Document : public QObject
	{
		using SaveAsCallback = std::function<std::optional<QString>()>;
		Q_OBJECT

	public:
		Document();
		~Document() = default;
		Q_DISABLE_COPY(Document);  // NOLINT(clang-diagnostic-extra-semi) (because without this extra semicolon, visual studio is dumb and indents the whole file one tab too much on the right).
		Document(Document&& other) noexcept;
		Document& operator=(Document&& other) noexcept;

		/**
		 * \brief Saves the document to the currently defined file path (m_filePath). If
		 * there is no current file path, the saveAsCallback function is called.
		 * A callback is used to decouple the UI system from the data (the document): it
		 * makes sure the Document does not do any Qt-specific call.
		 *
		 * \param saveAsCallback Function that optionally provides a path to a file. If
		 * no path is set, the document is not saved. It is up to the caller to choose
		 * how to provide this path (here it's will be a Qt dialog).
		 *
		 * \param saveAs Force the document to ask for a new path using the saveAsCallback,
		 * even if a path is currently saved in the document.
		 */
		void Save(const SaveAsCallback& saveAsCallback, bool saveAs = false);
		void LoadFromFile(const QString& filePath);
		
		TrackedPoint& CreateTrackedPoint();
		void RemoveTrackedPoint(int index);
		_NODISCARD const QVector<TrackedPoint>& GetTrackedPoints() const;
		_NODISCARD const TrailLength& GetTrailLength() const;
		_NODISCARD const QVector<int>& GetActivePointIndices() const;
		void SetActive(const TrackedPoint& point, bool active = true);

		Video& GetVideo();

		_NODISCARD bool IsDirty() const;
		void MarkDirty();

		std::optional<QString> GetFilePath() const;

	signals:
		void TrackedPointAdded(TrackedPoint& addedPoint);
		void TrackedPointRemoved(int pointIndex);
		void TrackedPointsListChanged(const QVector<TrackedPoint>& pointsList);
		void TrailLengthChanged(const TrailLength& newLength);
		void DocumentDirtinessChanged();

	private:
		/**
		 * \brief Stores the path to the file used to save the current
		 * document. There can be no path (for a new document for
		 * instance.)
		 */
		std::optional<QString> m_filePath;
		/**
		 * \brief True when some data is unsaved.
		 */
		bool m_dirty;
		/**
		 * \brief List of points of interest on the image.
		 */
		QVector<TrackedPoint> m_trackedPoints;
		/**
		 * \brief Object managing the currently loaded video.
		 */
		Video m_video;
		/**
		 * \brief Trail length for the tracked points, on the left
		 * and on the right on the timeline.
		 */
		TrailLength m_trailLength;
		/**
		 * \brief Active points are the points that need to be tracked by automatic trackers,
		 * or manually pointed by the user.
		 */
		QVector<int> m_activePointIndices;
	};
}
