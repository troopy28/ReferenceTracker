#pragma once

#include <QUndoStack>
#include <QVBoxLayout>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include "../Tracking/TrackingManager.h"

class AutomaticTrackingDisplay : public QWidget
{
	Q_OBJECT

public:
	explicit AutomaticTrackingDisplay(Data::Document& document, QUndoStack& undoStack, Tracking::TrackingManager& trackingManager, QWidget* parent = nullptr);

private:
	Data::Document& m_document;
	QSpinBox* m_roiSizeField;
	QComboBox* m_trackerTypeField;
	QPushButton* m_startTrackingBtn;

};
