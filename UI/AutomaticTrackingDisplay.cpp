#include "AutomaticTrackingDisplay.h"

#include <QLabel>
#include <QSpacerItem>
#include <QVBoxLayout>

#include "../Actions/TrackingCommands.h"

AutomaticTrackingDisplay::AutomaticTrackingDisplay(Data::Document& document, QUndoStack& undoStack, Tracking::ManualTrackingManager& trackingManager, QWidget* parent) :
	QWidget(parent),
	m_document(document),
	m_undoStack(undoStack),
	m_roiSizeField(new QSpinBox(this)),
	m_trackerTypeField(new QComboBox(this)),
	m_startTrackingBtn(new QPushButton("Start Tracking", this))
{
	m_roiSizeField->setValue(20);

	// Populate the tracker type combo box.
	const QVector<QString> trackerTypes = Tracking::GetTrackerTypes();
	std::for_each(trackerTypes.begin(), trackerTypes.end(), [this](const QString& type)
		{
			m_trackerTypeField->addItem(type);
		});
	m_trackerTypeField->setCurrentIndex(trackerTypes.size() - 1);

	QVBoxLayout* layout = new QVBoxLayout(this);
	setLayout(layout);
	layout->addWidget(new QLabel("Tracker Type"));
	layout->addWidget(m_trackerTypeField);
	layout->addWidget(new QLabel("ROI Size"));
	layout->addWidget(m_roiSizeField);
	layout->addWidget(m_startTrackingBtn);
	layout->addItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));

	connect(m_startTrackingBtn, &QPushButton::clicked, this, &AutomaticTrackingDisplay::StartTracking);
}

void AutomaticTrackingDisplay::StartTracking() const
{
	m_undoStack.push(new Actions::PerformAutomaticTrackingCommand(m_document, m_trackerTypeField->currentText(), m_roiSizeField->value()));
}
