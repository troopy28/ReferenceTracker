#include "TrackingManager.h"
#include <QDebug>

#ifdef ENABLE_LEGACY_TRACKERS
#include <opencv2/tracking/tracking_legacy.hpp>
#endif

namespace Tracking
{
	cv::Ptr<cv::Tracker> InitializeTracker(const QString& trackerType)
	{
		if (trackerType == "MIL")
			return cv::TrackerMIL::create();
		if (trackerType == "KCF")
			return cv::TrackerKCF::create();
		if (trackerType == "GOTURN")
			return cv::TrackerGOTURN::create();
		if (trackerType == "CSRT")
			return cv::TrackerCSRT::create();

#ifdef ENABLE_LEGACY_TRACKERS
		// Issue with those: they don't inherit cv::Tracker but cv::legacy::Tracker.
		if (trackerType == "MOSSE")
			return cv::legacy::TrackerMOSSE::create();
		if (trackerType == "TLD")
			return cv::legacy::TrackerTLD::create();
		if (trackerType == "MEDIANFLOW")
			return cv::legacy::TrackerMedianFlow::create();
		if (trackerType == "BOOSTING")
			return cv::legacy::TrackerBoosting::create();
#endif

		throw UnknownTrackerTypeException(trackerType);
	}

	PointTracker::PointTracker(Data::TrackedPoint& trackedPoint, const QString& trackerType) :
		m_cvTracker(InitializeTracker(trackerType)),
		m_boudingBox(),
		m_trackedPoint(trackedPoint)
	{
	}

	void PointTracker::Tick(const cv::Mat& image, const int frameIndex)
	{
		if (!m_cvTracker->update(image, m_boudingBox))
		{
			throw TrackingException(m_trackedPoint.GetName(), frameIndex);
		}

		const int xCenter = m_boudingBox.x + m_boudingBox.width / 2;
		const int yCenter = m_boudingBox.y + m_boudingBox.height / 2;
		m_trackedPoint.AddKeyframe(Data::Keyframe{ QPoint(xCenter, yCenter), frameIndex });
		qDebug() << "Tracking - Position of point" << m_trackedPoint.GetName() << "at frame" << frameIndex << "is (" << xCenter << "," << yCenter << ").";
	}

	void PointTracker::Initialize(const cv::Mat& image, const int frameIndex, const int roiSize)
	{
		const Data::Keyframe currentKeyframe = m_trackedPoint.GetLastKeyframe(frameIndex);
		const QPoint pos = currentKeyframe.position;

		m_boudingBox = cv::Rect(pos.x() - roiSize / 2, pos.y() - roiSize / 2, roiSize, roiSize);
		m_cvTracker->init(image, m_boudingBox);
	}

	AutomaticTrackingManager::AutomaticTrackingManager(const TrackerParams& params) :
		m_params(params),
		m_trackers()
	{
		// Initialize the trackers: one for each target point.
		const auto& trackedPointsIndices = m_params.document.GetActivePointIndices();
		std::for_each(trackedPointsIndices.begin(), trackedPointsIndices.end(), [&params, this](const int pointIndex)
			{
				m_trackers.emplace_back(PointTracker(params.document.GetTrackedPoint(pointIndex), params.trackerType));
			});
	}

	void AutomaticTrackingManager::InitializeTrackers()
	{
		std::for_each(m_trackers.begin(), m_trackers.end(), [this](PointTracker& tracker)
			{
				tracker.Initialize(m_params.document.GetVideo().GetCurrentImage(), m_params.document.GetVideo().GetCurrentFrameIndex(), m_params.roiSize);
			});
	}

	void AutomaticTrackingManager::TickTrackers()
	{
		std::for_each(m_trackers.begin(), m_trackers.end(), [this](PointTracker& tracker)
			{
				tracker.Tick(m_params.document.GetVideo().GetCurrentImage(), m_params.document.GetVideo().GetCurrentFrameIndex());
			});
		
	}

	ManualTrackingManager::ManualTrackingManager(Data::Document& document) :
		m_document(document),
		m_manuallyTrackedIndex(std::nullopt)
	{
	}

	void ManualTrackingManager::StartManualTracking(int trackedPointId)
	{
		m_manuallyTrackedIndex = trackedPointId;
		emit ManualTrackingStarted(m_document.GetTrackedPoint(trackedPointId).GetName());
		qDebug() << "Manual tracking started for tracked point " << trackedPointId;
	}

	void ManualTrackingManager::OnImageClicked(const QPointF& position)
	{
		if (!m_manuallyTrackedIndex.has_value())
			return;

		const int frameIndex = m_document.GetVideo().GetCurrentFrameIndex();
		Data::TrackedPoint& point = m_document.GetTrackedPoint(m_manuallyTrackedIndex.value());
		point.AddKeyframe(Data::Keyframe{ position.toPoint(), frameIndex });
		emit KeyframeChanged();
		m_document.GetVideo().ReadNextFrame(true);
	}
}
