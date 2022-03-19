#pragma once

#include "../Data/Document.h"
#include <QListWidget>
#include <QSpacerItem>
#include <QSplitter>
#include <QVBoxLayout>

class TrackedPointsList : public QWidget
{
	Q_OBJECT

public:
	explicit TrackedPointsList(Data::Document& document, QWidget* parent = nullptr);
	~TrackedPointsList() override;

	/**
	 * \brief Setup the widget managing this tracked point in the list
	 * (at the right of the control).
	 * \param point Data point being added.
	 */
	void AddTrackedPoint(Data::TrackedPoint& point);

public slots:


private:
	void SetupLayout();

	Data::Document& m_document;
	QVBoxLayout* m_pointsListLayout;
	QSpacerItem* m_listSpacer;
};
