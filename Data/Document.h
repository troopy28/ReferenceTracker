#pragma once

#include "../common.h"
#include <optional>
#include "TrackedPoint.h"
#include "Video.h"


namespace Data
{
	class WrongFormatException final : public std::exception
	{
	public:
		WrongFormatException() :
			std::exception("Wrong format. Make sure to select a Tracking Project File (*.tpj).")
		{
		}
	};

	class WrongTestIntegerException final : public std::exception
	{
	public:
		WrongTestIntegerException() :
			std::exception("There was an error reading the file (test integers do not match).")
		{
		}
	};

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
		~Document() override = default;
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
		TrackedPoint& CreateTrackedPoint(QString name, int index);
		void RemoveTrackedPoint(int index);
		TrackedPoint& InsertTrackedPoint(std::unique_ptr<TrackedPoint> point);
		_NODISCARD std::vector<std::unique_ptr<TrackedPoint>>& GetTrackedPoints();
		_NODISCARD TrackedPoint& GetTrackedPoint(int index) const;
		_NODISCARD const TrailLength& GetTrailLength() const;
		_NODISCARD const QVector<int>& GetActivePointIndices() const;

		void SetActive(const TrackedPoint& point, bool active = true);
		_NODISCARD bool IsActive(int index) const;

		Video& GetVideo();

		_NODISCARD bool IsDirty() const;
		void MarkDirty();

		_NODISCARD std::optional<QString> GetFilePath() const;

	signals:
		void TrackedPointAdded(TrackedPoint& addedPoint);
		void TrackedPointRemoved(int pointIndex);
		void TrackedPointsListChanged(const std::vector<TrackedPoint>& pointsList);
		void TrailLengthChanged(const TrailLength& newLength);
		void DocumentDirtinessChanged();
		void TrackPointActivationStateChanged(const TrackedPoint& addedPoint, bool isActive);

	private:
		/**
		 * \brief When this function is called, it is expected that
		 * m_filePath has a value. Otherwise a runtime error is
		 * thrown.
		 */
		void SaveImpl() const;
		/**
		 * \brief When this function is called, it is expected that
		 * m_filePath has a value. Otherwise a runtime error is
		 * thrown.
		 */
		void LoadImpl(const QString& path);

		void ClearDocument();

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
		 * Cannot use a QVector here, since they require their elements
		 * to be copiable (which std::unique_ptr is not).
		 */
		std::vector<std::unique_ptr<TrackedPoint>> m_trackedPoints;
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
