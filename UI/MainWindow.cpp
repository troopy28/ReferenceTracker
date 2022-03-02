#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QSplitter>
#include <QListView>
#include <QTextEdit>
#include <QResizeEvent>

MainWindow::MainWindow(QWidget* parent) :
	QMainWindow(parent),
	m_typeSafeSettings(),
	ui(new Ui::MainWindow),
	m_videoPlayer(this)
{
	ui->setupUi(this);
	ManualUiSetup();
	ApplyUiSettings();
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
	QMainWindow::resizeEvent(event);
	m_typeSafeSettings.SetMinimizedWidth(event->size().width());
	m_typeSafeSettings.SetMinimizeHeight(event->size().height());
}

void MainWindow::ManualUiSetup()
{
	// Global vertical splitter. On top is the video displayer and the tracker settings.
	// At the bottom is the curves visualizer and the points list.
	QSplitter* globalVerticalSplitter = new QSplitter(Qt::Vertical, this);
	globalVerticalSplitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);


	QListView* listview = new QListView(this);
	QTextEdit* textedit = new QTextEdit(this);
	textedit->setPlainText("abc");
	QSplitter* bottomHorizontalSplitter = new QSplitter(this);
	bottomHorizontalSplitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	bottomHorizontalSplitter->addWidget(listview);
	bottomHorizontalSplitter->addWidget(textedit);

	globalVerticalSplitter->addWidget(&m_videoPlayer);
	globalVerticalSplitter->addWidget(bottomHorizontalSplitter);


	setCentralWidget(globalVerticalSplitter);
}

void MainWindow::ApplyUiSettings()
{
	resize(m_typeSafeSettings.GetMinimizedWidth(), m_typeSafeSettings.GetMinimizedHeight());
}

MainWindow::~MainWindow()
{
	delete ui;
}
