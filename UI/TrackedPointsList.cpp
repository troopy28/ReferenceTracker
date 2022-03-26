#include "TrackedPointsList.h"

#include <QLayout>
#include <QPushButton>
#include <QSplitter>
#include <QTextEdit>
#include <QDebug>
#include <QLineEdit>
#include <QColorDialog>
#include <QMenu>

#include "ClickableLabel.h"
#include "ScrollableGraphicsView.h"
#include "../Actions/TrackedPointCommands.h"

TrackedPointsList::TrackedPointsList(Data::Document& document, QUndoStack& undoStack, Tracking::ManualTrackingManager& trackingManager, QWidget* parent) :
	QWidget(parent),
	m_document(document),
	m_trackingManager(trackingManager),
	m_undoStack(undoStack),
	m_pointsListLayout(nullptr),
	m_listSpacer(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding)),
	m_pointDisplayers()
{
	SetupLayout();
	connect(&m_document, &Data::Document::TrackedPointAdded, this, &TrackedPointsList::AddTrackedPoint);
	connect(&m_document, &Data::Document::TrackedPointRemoved, this, &TrackedPointsList::RemoveTrackedPoint);
}

void TrackedPointsList::AddTrackedPoint(Data::TrackedPoint& point)
{
	constexpr static int colorSquareSize = 16;
	// 1. Create the widget.
	QWidget* pointListItemWidget = new QWidget(this);
	pointListItemWidget->setContentsMargins(1, 1, 1, 1);
	pointListItemWidget->setStyleSheet(QString("QWidget { border-top: 0 solid rgb(69, 69, 69); border-bottom: 1 solid rgb(69, 69, 69);}"));
	pointListItemWidget->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(pointListItemWidget, &QWidget::customContextMenuRequested, this, [pointListItemWidget, this, &point](const QPoint& pos)
		{
			QMenu* menu = new QMenu();

			// Remove.
			QAction* removeAction = new QAction("Remove");
			connect(removeAction, &QAction::triggered, pointListItemWidget, [this, &point]
				{
					const int pointIndex = point.GetPointIndex();
					m_undoStack.push(new Actions::RemoveTrackedPointCommand(m_document, pointIndex));
				});
			menu->addAction(removeAction);

			// Track manually.
			QAction* trackAction = new QAction("Track Manually");
			connect(trackAction, &QAction::triggered, pointListItemWidget, [this, &point]
				{
					const int pointIndex = point.GetPointIndex();
					m_trackingManager.StartManualTracking(pointIndex);
				});
			menu->addAction(trackAction);

			// Spawn the menu.
			menu->popup(pointListItemWidget->mapToGlobal(pos));

		});

	// 2. Create the text field to edit its name.
	QLineEdit* pointNameDisplayer = new QLineEdit(point.GetName(), pointListItemWidget);
	pointNameDisplayer->setStyleSheet(QString("QLineEdit { background-color: rgb(35, 35, 35); border-width: 0; } QLineEdit:hover { background-color: rgb(42, 42, 42); border-width: 0; }"));
	connect(pointNameDisplayer, &QLineEdit::textChanged, &point, &Data::TrackedPoint::SetName);

	// 3. Create the button to pick the color.
	ClickableLabel* colorDisplayer = new ClickableLabel(pointListItemWidget);
	colorDisplayer->setStyleSheet(QString("QWidget { border: 1 solid rgb(50, 50, 50);}"));
	colorDisplayer->setMinimumWidth(colorSquareSize);
	colorDisplayer->setMinimumHeight(colorSquareSize);
	QPixmap colorPixmap(colorSquareSize, colorSquareSize);
	colorPixmap.fill(point.GetColor());
	colorDisplayer->setPixmap(colorPixmap);

	// Note: important to use the overload allowing to specify the "pointListItemWidget" as the context, so that when 
	//it (the widget) gets deleted, the lambda gets disconnected and deleted too (handled in QObject's destructor).
	connect(&point, &Data::TrackedPoint::ColorChanged, pointListItemWidget, [colorDisplayer](const QColor& col)
		{
			QPixmap newPixmap(colorSquareSize, colorSquareSize);
			newPixmap.fill(col);
			colorDisplayer->setPixmap(newPixmap);
		});
	connect(colorDisplayer, &ClickableLabel::clicked, pointListItemWidget, [&point]
		{
			// When the color square is clicked: change the color.
			const QColor currentColor = point.GetColor();
			const QColor chosenColor = QColorDialog::getColor(currentColor, nullptr, point.GetName());
			point.SetColor(chosenColor);
		});

	// 4. Create the spacer that goes between the name field and the other fields.
	// The UI should look like:
	// [color picker][name field] ------ spacer ------ [enable tracking "checkbox"][visible "checkbox"]
	QSpacerItem* spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

	// 5. Create the enable/disable tracking "checkbox".
	ClickableLabel* trackingStateDisplayer = new ClickableLabel(pointListItemWidget);
	trackingStateDisplayer->setStyleSheet(QString("QWidget { border-width: 0;} QWidget:hover { border-width: 0; color: rgb(170, 170, 170);}"));
	trackingStateDisplayer->setMinimumWidth(colorSquareSize);
	trackingStateDisplayer->setMinimumHeight(colorSquareSize);
	trackingStateDisplayer->setToolTip("Use in Automatic Tracking");
	const QPixmap trackingStatePixmap(":/Resources/tracking_off.png");
	trackingStateDisplayer->setPixmap(trackingStatePixmap.scaled(colorSquareSize, colorSquareSize));
	connect(&m_document, &Data::Document::TrackPointActivationStateChanged, pointListItemWidget, [trackingStateDisplayer, this, &point](const Data::TrackedPoint& tp, const bool visible)
		{
			if(point.GetPointIndex() == tp.GetPointIndex())
			{
				const QPixmap newPixmap(visible ? QString(":/Resources/tracking_on.png") : QString(":/Resources/tracking_off.png"));
				trackingStateDisplayer->setPixmap(newPixmap.scaled(colorSquareSize, colorSquareSize));
			}
		});
	connect(trackingStateDisplayer, &ClickableLabel::clicked, pointListItemWidget, [&point, this]
		{
			m_document.SetActive(point, !m_document.IsActive(point.GetPointIndex()));
		});

	// 6. Create the show/hide "checkbox".
	ClickableLabel* visibilityDisplayer = new ClickableLabel(pointListItemWidget);
	visibilityDisplayer->setStyleSheet(QString("QWidget { border-width: 0;} QWidget:hover { border-width: 0; color: rgb(170, 170, 170);}"));
	visibilityDisplayer->setMinimumWidth(colorSquareSize);
	visibilityDisplayer->setMinimumHeight(colorSquareSize);
	visibilityDisplayer->setToolTip("Show / Hide this point's data");
	const QPixmap visibilityPixmap(":/Resources/show.png");
	visibilityDisplayer->setPixmap(visibilityPixmap.scaled(colorSquareSize, colorSquareSize));
	connect(&point, &Data::TrackedPoint::VisibilityChanged, pointListItemWidget, [visibilityDisplayer](const bool visible)
		{
			const QPixmap newPixmap(visible ? QString(":/Resources/show.png") : QString(":/Resources/hide.png"));
			visibilityDisplayer->setPixmap(newPixmap.scaled(colorSquareSize, colorSquareSize));
		});
	connect(visibilityDisplayer, &ClickableLabel::clicked, pointListItemWidget, [&point]
		{
			point.SetVisibleInViewport(!point.IsVisibleInViewport());
		});


	// 7. Set everything up.
	QHBoxLayout* layout = new QHBoxLayout(pointListItemWidget);
	pointListItemWidget->setLayout(layout);
	layout->setContentsMargins(1, 5, 1, 5);
	layout->addWidget(colorDisplayer);
	layout->addWidget(pointNameDisplayer);
	layout->addItem(spacer);
	layout->addWidget(trackingStateDisplayer);
	layout->addWidget(visibilityDisplayer);

	// 8. Remove the spacer and clear the UI list.
	m_pointsListLayout->removeItem(m_listSpacer);
	for (auto* wgt : m_pointDisplayers)
		m_pointsListLayout->removeWidget(wgt);

	// 9. Add the widget in the displayers list.
	m_pointDisplayers.insert(point.GetPointIndex(), pointListItemWidget);

	// 10. Rebuild the list and add the final spacer.
	for (auto* wgt : m_pointDisplayers)
		m_pointsListLayout->addWidget(wgt);
	m_pointsListLayout->addItem(m_listSpacer);
}

void TrackedPointsList::RemoveTrackedPoint(const int pointIndex)
{
	m_pointsListLayout->removeWidget(m_pointDisplayers[pointIndex]);
	delete m_pointDisplayers[pointIndex];
	m_pointDisplayers.removeAt(pointIndex);
}

void TrackedPointsList::SetupLayout()
{
	// 1. Setup the list in itself.
	setMinimumWidth(250);

	QScrollArea* scrollArea = new QScrollArea(this);
	scrollArea->setWidgetResizable(true);
	scrollArea->setContentsMargins(0, 0, 0, 0);

	QWidget* scrollAreaWidget = new QWidget(this);
	scrollAreaWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
	scrollAreaWidget->setContentsMargins(0, 0, 0, 0);
	m_pointsListLayout = new QVBoxLayout(scrollAreaWidget);
	m_pointsListLayout->setContentsMargins(0, 0, 0, 0);
	m_pointsListLayout->setSpacing(0);
	scrollAreaWidget->setLayout(m_pointsListLayout);
	scrollArea->setWidget(scrollAreaWidget);
	scrollArea->setStyleSheet(QString("QWidget { background-color: rgb(35, 35, 35); border-style: outset; border-width: 0;}"));

	// Spacer to force items to live at the TOP only of the list (and not spread out vertically).
	scrollAreaWidget->layout()->addItem(m_listSpacer);

	// 2. Setup the button to add a point.
	QWidget* bottomBar = new QWidget(this);
	QHBoxLayout* bottomBarLayout = new QHBoxLayout(bottomBar);
	bottomBarLayout->setContentsMargins(0, 0, 0, 0);
	bottomBar->setLayout(bottomBarLayout);
	QPushButton* addPointBtn = new QPushButton("Add Point", this);
	connect(addPointBtn, &QPushButton::clicked, this, [this]
		{
			m_undoStack.push(new Actions::CreateTrackedPointCommand(m_document));
		});
	bottomBarLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
	bottomBarLayout->addWidget(addPointBtn);

	QVBoxLayout* globalListWidgetLayout = new QVBoxLayout();
	setLayout(globalListWidgetLayout);
	globalListWidgetLayout->addWidget(scrollArea);
	globalListWidgetLayout->addWidget(bottomBar);
}
