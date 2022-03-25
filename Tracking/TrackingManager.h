#pragma once

#include <opencv2/tracking.hpp>
#include "../Data/Document.h"

namespace Tracking
{
	inline QVector<QString> GetTrackerTypes()
	{
#ifdef ENABLE_LEGACY_TRACKERS
		return { "MIL", "KCF", "GOTURN", "CSRT", "TLD","MEDIANFLOW",  "MOSSE", "BOOSTING" };
#else
		return { "MIL", "KCF", "GOTURN", "CSRT" };
#endif

	}

	struct TrackerParams
	{
		int startFrame;
		int endFrame;
		std::vector<int> trackedPointsIndices;
		QString trackerType;
		Data::Document& document;

		explicit TrackerParams(Data::Document& doc) :
			startFrame(0),
			endFrame(0),
			trackedPointsIndices(),
			trackerType(),
			document(doc)
		{
		}
	};

	_NODISCARD cv::Ptr<cv::Tracker> InitializeTracker(const QString& trackerType);

	class TrackingException final : public std::exception
	{
	public:
		TrackingException(const QString& trackedPointName, const int frameIndex) :
			std::exception(),
			m_customMessage((QString("Could not track the point ") + trackedPointName + " at frame " + QString::number(frameIndex)).toStdString())
		{
		}

		_NODISCARD char const* what() const override
		{
			return m_customMessage.c_str();
		}

	private:
		std::string m_customMessage;
	};

	class UnknownTrackerTypeException final : public std::exception
	{
	public:
		explicit UnknownTrackerTypeException(const QString& trackerType) :
			std::exception(),
			m_customMessage((QString("Unknown tracker type:  ") + trackerType).toStdString())
		{
		}

		_NODISCARD char const* what() const override
		{
			return m_customMessage.c_str();
		}

	private:
		std::string m_customMessage;
	};

	class PointTracker
	{
	public:
		explicit PointTracker(Data::TrackedPoint& trackedPoint, const QString& trackerType);

		void Tick(const cv::Mat& image, int frameIndex);
		void Initialize(const cv::Mat& image, int frameIndex, int roiSize);

	private:
		cv::Ptr<cv::Tracker> m_cvTracker;
		cv::Rect m_boudingBox;
		Data::TrackedPoint& m_trackedPoint;
	};

	class AutomaticTrackingManager
	{
	public:
		explicit AutomaticTrackingManager(const TrackerParams& params);

		void TickTrackers(const cv::Mat& image);

	private:
		TrackerParams m_params;
		std::vector<PointTracker> m_trackers;
	};


	class TrackingManager : public QObject
	{
		Q_OBJECT

	public:
		explicit TrackingManager(Data::Document& document);
		void StartManualTracking(int trackedPointId);

	public slots:
		void OnImageClicked(const QPointF& position);

	signals:
		void KeyframeChanged();
		void ManualTrackingStarted(const QString& poi);
		void ManualTrackingEnded();

	private:
		Data::Document& m_document;
		/**
		 * \brief Index of the tracked point being manually tracked by the user.
		 */
		std::optional<int> m_manuallyTrackedIndex;
	};

}