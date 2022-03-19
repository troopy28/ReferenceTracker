#pragma once

#include "../Data/Document.h"
#include <QListWidget>
#include <QVBoxLayout>
#include <QUndoStack>

class TrackedPointsList : public QWidget
{
	Q_OBJECT

public:
	explicit TrackedPointsList(Data::Document& document, QUndoStack& undoStack, QWidget* parent = nullptr);

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
	QUndoStack& m_undoStack;
	QVBoxLayout* m_pointsListLayout;
	QSpacerItem* m_listSpacer;
	QVector<QWidget*> m_pointDisplayers;
};
