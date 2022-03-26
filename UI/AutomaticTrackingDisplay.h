#pragma once

#include <QUndoStack>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include "../Tracking/TrackingManager.h"

class AutomaticTrackingDisplay : public QWidget
{
	Q_OBJECT

public:
	explicit AutomaticTrackingDisplay(Data::Document& document, QUndoStack& undoStack, Tracking::ManualTrackingManager& trackingManager, QWidget* parent = nullptr);
	~AutomaticTrackingDisplay() override = default;
	Q_DISABLE_COPY_MOVE(AutomaticTrackingDisplay);

private:
	void StartTracking() const;

	Data::Document& m_document;
	QUndoStack& m_undoStack;
	QSpinBox* m_roiSizeField;
	QComboBox* m_trackerTypeField;
	QPushButton* m_startTrackingBtn;

};
