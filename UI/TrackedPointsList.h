#pragma once

#include "../common.h"
#include "../Data/Document.h"
#include "../Tracking/TrackingManager.h"
#include <QListWidget>
#include <QVBoxLayout>
#include <QUndoStack>

class TrackedPointsList : public QWidget
{
	Q_OBJECT

public:
	explicit TrackedPointsList(Data::Document& document, QUndoStack& undoStack, Tracking::ManualTrackingManager& trackingManager, QWidget* parent = nullptr);
	~TrackedPointsList() override = default;
	Q_DISABLE_COPY_MOVE(TrackedPointsList);

	/**
	 * \brief Setup the widget managing this tracked point in the list
	 * (at the right of the control).
	 * \param point Data point being added.
	 */
	void AddTrackedPoint(Data::TrackedPoint& point);

	void RemoveTrackedPoint(int pointIndex);

public slots:


private:
	void SetupLayout();

	Data::Document& m_document;
	Tracking::ManualTrackingManager& m_trackingManager;
	QUndoStack& m_undoStack;
	QVBoxLayout* m_pointsListLayout;
	QSpacerItem* m_listSpacer;
	QVector<QWidget*> m_pointDisplayers;
};
