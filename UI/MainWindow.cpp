#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QSplitter>

#include <QResizeEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QDebug>
#include <QStackedLayout>

MainWindow::MainWindow(QWidget* parent) :
	QMainWindow(parent),
	m_document(),
	m_typeSafeSettings(),
	m_undoStack(),
	ui(new Ui::MainWindow),
	m_videoPlayer(new VideoPlayer(m_document.GetVideo(), this)),
	m_trackedPointsList(new TrackedPointsList(m_document, m_undoStack, this)),
	m_graphView(new GraphView(m_document, this))
{
	ui->setupUi(this);
	ManualUiSetup();
	ApplyUiSettings();

	connect(ui->actionOpen_Video, &QAction::triggered, this, &MainWindow::OpenVideoMenuItemClicked);
	connect(ui->actionClose, &QAction::triggered, this, &MainWindow::close);

	m_videoPlayer->Render(0);
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
	QMainWindow::resizeEvent(event);
	m_typeSafeSettings.SetMinimizedWidth(event->size().width());
	m_typeSafeSettings.SetMinimizeHeight(event->size().height());
}

void MainWindow::changeEvent(QEvent* e)
{
	QMainWindow::changeEvent(e);

	if (e->type() == QEvent::WindowStateChange)
	{
		m_typeSafeSettings.SetMaximized(this->windowState() & Qt::WindowMaximized);
	}
}

void MainWindow::ManualUiSetup()
{
	// Global vertical splitter. On top is the video displayer and the tracker settings.
	// At the bottom is the curves visualizer and the points list.
	QSplitter* globalVerticalSplitter = new QSplitter(Qt::Vertical, this);
	globalVerticalSplitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	globalVerticalSplitter->addWidget(m_videoPlayer);

	QSplitter* bottomSplitter = new QSplitter(this);
	bottomSplitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	// Setup the left part (points list).
	bottomSplitter->addWidget(m_trackedPointsList);

	// Setup the right part (curves viewer / timeline).
	bottomSplitter->addWidget(m_graphView);
	bottomSplitter->setStretchFactor(1, 3);

	// Global layout of the bottom control
	QStackedLayout* layout = new QStackedLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(bottomSplitter);
	setLayout(layout);

	globalVerticalSplitter->addWidget(bottomSplitter);
	setCentralWidget(globalVerticalSplitter);

	// Dynamic menus setup.
	GenerateRecentVideosMenu();
}

void MainWindow::ApplyUiSettings()
{
	resize(m_typeSafeSettings.GetMinimizedWidth(), m_typeSafeSettings.GetMinimizedHeight());
	if (m_typeSafeSettings.IsMaximized())
		setWindowState(Qt::WindowMaximized);
}

#pragma region Menu bar callbacks

void MainWindow::OpenVideoMenuItemClicked()
{
	const QString fileName = QFileDialog::getOpenFileName(
		this, "Save As", "", tr("Video file (*.mp4 *.avi)"));
	OpenVideo(fileName);
}

void MainWindow::GenerateRecentVideosMenu()
{
	ui->openRecentVideoMenu->clear();

	const QStringList recentVideoPaths = m_typeSafeSettings.GetRecentVideos();
	for (const QString& path : recentVideoPaths)
	{
		const QFileInfo fileInfo(path);
		ui->openRecentVideoMenu->addAction(fileInfo.fileName(), [this, path] {OpenVideo(path); });
	}
}

void MainWindow::OpenVideo(const QString& path)
{
	if (!path.isEmpty())
	{
		if (m_document.GetVideo().LoadFromFile(path))
		{
			m_typeSafeSettings.AddRecentVideo(path);
			GenerateRecentVideosMenu();
		}
		else
		{
			QMessageBox::warning(this, "Could not open the video.", path);
		}
	}
}


#pragma endregion

MainWindow::~MainWindow()
{
	qDebug() << "MainWindow::~MainWindow()",
	delete ui;
}
