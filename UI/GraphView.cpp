#include "GraphView.h"

#include <QColorDialog>
#include <qlayout.h>
#include <QPushButton>
#include <QSplitter>
#include <QTextEdit>
#include <QStackedLayout>
#include <QDebug>
#include <QLineEdit>
#include <QColorDialog>
#include <QTimer>

#include "ClickableLabel.h"
#include "ScrollableGraphicsView.h"

GraphView::GraphView(Data::Document& document, QWidget* parent) :
	QWidget(parent),
	m_document(document),
	m_pointsListLayout(nullptr),
	m_listSpacer(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding))
{
	// Setup the splitter.
	QSplitter* splitter = new QSplitter(this);
	splitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	// Setup the left part (points list).
	SetupPointsListWidget(splitter);
	QTextEdit* textedit = new QTextEdit(this);
	textedit->setPlainText("abc");

	// Setup the right part (curves viewer / timeline).
	splitter->addWidget(textedit); // todo: change this
	splitter->setStretchFactor(1, 3);

	// Global layout of the control. Contains everything.
	QStackedLayout* layout = new QStackedLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(splitter);
	setLayout(layout);
}

GraphView::~GraphView()
{
	qDebug() << "GraphView::~GraphView";
}

void GraphView::AddTrackedPoint(Data::TrackedPoint& point)
{
	constexpr static int colorSquareSize = 16;
	// 1. Create the widget.
	QWidget* pointListItemWidget = new QWidget(this); // todo: warning possible double deletion
	pointListItemWidget->setContentsMargins(1, 1, 1, 1);
	pointListItemWidget->setStyleSheet(QString("QWidget { border-top: 0 solid rgb(69, 69, 69); border-bottom: 1 solid rgb(69, 69, 69);}"));

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

	// Note: important to use the overload allowing to specify the "this" as the context, so that when 
	// "this" gets deleted, the lambda gets disconnected and deleted too (handled in QObject's destructor).
	connect(&point, &Data::TrackedPoint::ColorChanged, this, [colorDisplayer](const QColor& col)
		{
			QPixmap newPixmap(colorSquareSize, colorSquareSize);
			newPixmap.fill(col);
			colorDisplayer->setPixmap(newPixmap);
		});
	connect(colorDisplayer, &ClickableLabel::clicked, this, [&point]
		{
			// When the color square is clicked: change the color.
			const QColor currentColor = point.GetColor();
			const QColor chosenColor = QColorDialog::getColor(currentColor, nullptr, point.GetName());
			point.SetColor(chosenColor);
		});

	// 4. Create the spacer that goes between the name field and the other fields.
	// The UI should look like:
	// [color picker][name field] ------ spacer ------ [visible "checkbox"]
	QSpacerItem* spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

	// 5. Create the show/hide "checkbox"".
	ClickableLabel* visibilityDisplayer = new ClickableLabel(pointListItemWidget);
	visibilityDisplayer->setStyleSheet(QString("QWidget { border-width: 0;} QWidget:hover { border-width: 0; color: rgb(170, 170, 170);}"));
	visibilityDisplayer->setMinimumWidth(colorSquareSize);
	visibilityDisplayer->setMinimumHeight(colorSquareSize);
	const QPixmap visibilityPixmap(":/Resources/show.png");
	visibilityDisplayer->setPixmap(visibilityPixmap.scaled(colorSquareSize, colorSquareSize));
	connect(&point, &Data::TrackedPoint::VisibilityChanged, this, [visibilityDisplayer](const bool visible)
		{
			const QPixmap newPixmap(visible ? QString(":/Resources/show.png") : QString(":/Resources/hide.png"));
			visibilityDisplayer->setPixmap(newPixmap.scaled(colorSquareSize, colorSquareSize));
		});
	connect(visibilityDisplayer, &ClickableLabel::clicked, this, [&point]
		{
			point.SetVisibleInViewport(!point.IsVisibleInViewport());
		});

	// 5. Set everything up.
	QHBoxLayout* layout = new QHBoxLayout(pointListItemWidget);
	pointListItemWidget->setLayout(layout);
	layout->setContentsMargins(1, 5, 1, 5);
	layout->addWidget(colorDisplayer);
	layout->addWidget(pointNameDisplayer);
	layout->addItem(spacer);
	layout->addWidget(visibilityDisplayer);

	// 6. Add the layout! Phew!
	m_pointsListLayout->removeItem(m_listSpacer);
	m_pointsListLayout->addWidget(pointListItemWidget);
	m_pointsListLayout->addItem(m_listSpacer);
}

void GraphView::SetupPointsListWidget(QSplitter* splitter)
{
	// 1. Setup the list in itself.
	QWidget* globalListWidget = new QWidget(this);
	globalListWidget->setMinimumWidth(250);

	QScrollArea* scrollArea = new QScrollArea(globalListWidget);
	scrollArea->setWidgetResizable(true);
	scrollArea->setContentsMargins(0, 0, 0, 0);

	QWidget* scrollAreaWidget = new QWidget(globalListWidget);
	scrollAreaWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
	scrollAreaWidget->setContentsMargins(0, 0, 0, 0);
	m_pointsListLayout = new QVBoxLayout(scrollAreaWidget);
	m_pointsListLayout->setContentsMargins(0, 0, 0, 0);
	m_pointsListLayout->setSpacing(0);
	scrollAreaWidget->setLayout(m_pointsListLayout);
	scrollArea->setWidget(scrollAreaWidget);
	scrollArea->setStyleSheet(QString("QWidget { background-color: rgb(35, 35, 35); border-style: outset; border-width: 0;}"));

	/* Test: TODO remove. */
	QTimer* timer = new QTimer(this);
	static int i = 0;
	connect(timer, &QTimer::timeout, this, [this]
		{
			if (i < 10)
			{
				Data::TrackedPoint* testPoint = new Data::TrackedPoint("abc " + QString::number(i), i);
				AddTrackedPoint(*testPoint);
				i++;
			}
		});
	timer->start(1000);

	// Spacer to force items to live at the TOP only of the list (and not spread out vertically).
	scrollAreaWidget->layout()->addItem(m_listSpacer);

	// 2. Setup the button to add a point.
	QWidget* bottomBar = new QWidget(globalListWidget);
	QHBoxLayout* bottomBarLayout = new QHBoxLayout(bottomBar);
	bottomBarLayout->setContentsMargins(0, 0, 0, 0);
	bottomBar->setLayout(bottomBarLayout);
	QPushButton* addPointBtn = new QPushButton("Add Point", globalListWidget);
	bottomBarLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
	bottomBarLayout->addWidget(addPointBtn);

	QVBoxLayout* globalListWidgetLayout = new QVBoxLayout();
	globalListWidget->setLayout(globalListWidgetLayout);
	globalListWidgetLayout->addWidget(scrollArea);
	globalListWidgetLayout->addWidget(bottomBar);

	// Finally: add the widget to the splitter.
	splitter->addWidget(globalListWidget);
}