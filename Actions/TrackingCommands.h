#pragma once

#include <QUndoCommand>
#include "../Data/Document.h"
#include "../Tracking/TrackingManager.h"
#include <QTimer>

namespace Actions
{
	class PerformAutomaticTrackingCommand final : public QUndoCommand
	{
	public:
		explicit PerformAutomaticTrackingCommand(Data::Document& document, const QString& trackerType, int roiSize);
		void redo() override;
		void undo() override;

	private:
		Data::Document& m_document;
		std::vector<std::unique_ptr<Data::TrackedPoint>> m_points;
		Tracking::TrackerParams m_params;
		Tracking::AutomaticTrackingManager m_trackingManager;
		QTimer m_timer;
	};

}