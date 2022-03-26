#include "TrackingCommands.h"
#include <algorithm>
#include <QMessageBox>

namespace Actions
{
	PerformAutomaticTrackingCommand::PerformAutomaticTrackingCommand(Data::Document& document, const QString& trackerType, const int roiSize) :
		m_document(document),
		m_points(),
		m_params{ roiSize,  trackerType, document },
		m_trackingManager(m_params),
		m_timer()
	{
		m_params.roiSize = roiSize;
		m_params.trackerType = trackerType;
		//m_params.startFrame = startFrame;
		//m_params.endFrame = endFrame;

		const QVector<int>& activePointsIndices = m_document.GetActivePointIndices();
		std::for_each(activePointsIndices.cbegin(), activePointsIndices.cend(), [this](const int index)
			{
				const Data::TrackedPoint& trackedPoint = m_document.GetTrackedPoint(index);
				m_points.emplace_back(trackedPoint.GetCopy());
			});
	}

	void PerformAutomaticTrackingCommand::redo()
	{
		m_trackingManager.InitializeTrackers();

		m_timer.setInterval(16); // 60FPS max.
		QTimer::connect(&m_timer, &QTimer::timeout, [this]
			{
				Data::Video& video = m_document.GetVideo();

				if (video.GetCurrentFrameIndex() + 1 < video.GetFrameCount())
				{
					try
					{
						m_trackingManager.TickTrackers();
						video.ReadNextFrame();
					}
					catch (Tracking::TrackingException& ex)
					{
						QMessageBox messageBox;
						messageBox.setWindowTitle("Tracking Error");
						messageBox.setText(QString("An error occurred while tracking the points. ") + ex.what());
						m_timer.stop();
					}
				}
				else
				{
					m_timer.stop();
				}
			});
		m_timer.start();
	}

	void PerformAutomaticTrackingCommand::undo()
	{
		for (const std::unique_ptr<Data::TrackedPoint>& backupPoint : m_points)
		{
			Data::TrackedPoint& realPoint = m_document.GetTrackedPoint(backupPoint->GetPointIndex());
			realPoint.ClearKeyframes();
			for (const Data::Keyframe& keyframe : backupPoint->GetKeyframes())
			{
				realPoint.AddKeyframe(keyframe);
			}
		}
		m_points.clear();
	}
}