#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QResizeEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QDebug>
#include "DynamicSplitter.h"


MainWindow::MainWindow(QWidget* parent) :
	QMainWindow(parent),
	m_document(),
	m_typeSafeSettings(),
	m_trackingManager(m_document),
	m_undoStack(),
	ui(new Ui::MainWindow),
	m_videoPlayer(new VideoPlayer(m_document.GetVideo(), this)),
	m_trackedPointsList(new TrackedPointsList(m_document, m_undoStack, m_trackingManager, this)),
	m_graphView(new GraphView(m_document, m_trackingManager, this))
{
	ui->setupUi(this);
	
	ManualUiSetup();
	ApplyUiSettings();

	// File menu.
	connect(ui->actionOpenProject, &QAction::triggered, this, &MainWindow::OpenProjectMenuItemClicked);
	connect(ui->actionOpen_Video, &QAction::triggered, this, &MainWindow::OpenVideoMenuItemClicked);
	connect(ui->actionSave, &QAction::triggered, this, &MainWindow::SaveMenuItemClicked);
	connect(ui->actionSave_As, &QAction::triggered, this, &MainWindow::SaveAsMenuItemClicked);
	connect(ui->actionClose, &QAction::triggered, this, &MainWindow::close);

	// Edit menu.
	connect(ui->actionUndo, &QAction::triggered, &m_undoStack, &QUndoStack::undo);
	connect(ui->actionRedo, &QAction::triggered, &m_undoStack, &QUndoStack::redo);

	// Document.
	connect(&m_document, &Data::Document::DocumentDirtinessChanged, this, &MainWindow::ComputeWindowTitle);

	// Tracking manager.
	connect(m_videoPlayer, &VideoPlayer::ImageClicked, &m_trackingManager, &Tracking::TrackingManager::OnImageClicked);
	connect(&m_trackingManager, &Tracking::TrackingManager::ManualTrackingStarted, this, [this](const QString& pointName) {m_statusLabel->setText(QString("Manual tracking started for ") + pointName + "."); });
	connect(&m_trackingManager, &Tracking::TrackingManager::ManualTrackingEnded, this, [this] {m_statusLabel->setText(QString("Manual tracking stopped.")); });

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
	DynamicSplitter* globalVerticalSplitter = new DynamicSplitter(Qt::Vertical, this);
	globalVerticalSplitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	globalVerticalSplitter->AddWidget(m_videoPlayer);

	DynamicSplitter* bottomSplitter = new DynamicSplitter(Qt::Horizontal, this);
	bottomSplitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	// Setup the left part (points list).
	bottomSplitter->AddWidget(m_trackedPointsList);

	// Setup the right part (curves viewer / timeline).
	bottomSplitter->AddWidget(m_graphView);
	bottomSplitter->setStretchFactor(1, 3);


	globalVerticalSplitter->AddWidget(bottomSplitter);
	setCentralWidget(globalVerticalSplitter);

	ComputeWindowTitle();

	// Dynamic menus setup.
	GenerateRecentVideosMenu();
	GenerateRecentProjectsMenu();

	// Status bar.
	m_statusLabel = new QLabel("", this);
	ui->statusbar->addWidget(m_statusLabel);
}

void MainWindow::ApplyUiSettings()
{
	resize(m_typeSafeSettings.GetMinimizedWidth(), m_typeSafeSettings.GetMinimizedHeight());
	if (m_typeSafeSettings.IsMaximized())
		setWindowState(Qt::WindowMaximized);
}

void MainWindow::OpenProjectMenuItemClicked()
{
	const QString fileName = QFileDialog::getOpenFileName(
		this, "Open Project", "", "Tracking Project File(*.tpj)");
	OpenProject(fileName);
}

#pragma region Menu bar callbacks

void MainWindow::OpenVideoMenuItemClicked()
{
	const QString fileName = QFileDialog::getOpenFileName(
		this, "Open Video", "", "Video file (*.mp4 *.avi)");
	OpenVideo(fileName);
}

std::optional<QString> SaveAsCallback()
{
	const QString fileName = QFileDialog::getSaveFileName(
		nullptr, "Save As", "", "Tracking Project File(*.tpj)");
	if (fileName.isEmpty())
		return std::nullopt;
	else
		return std::make_optional<QString>(fileName);
}

void MainWindow::SaveMenuItemClicked()
{
	m_document.Save(&SaveAsCallback);
	m_typeSafeSettings.AddRecentProject(m_document.GetFilePath().value());
	GenerateRecentVideosMenu();
}

void MainWindow::SaveAsMenuItemClicked()
{
	m_document.Save(&SaveAsCallback, true);
	m_typeSafeSettings.AddRecentProject(m_document.GetFilePath().value());
	GenerateRecentVideosMenu();
}

void MainWindow::GenerateRecentProjectsMenu()
{
	ui->openRecentProjectMenu->clear();

	const QStringList recentProjectPaths = m_typeSafeSettings.GetRecentProjects();
	for (const QString& path : recentProjectPaths)
	{
		const QFileInfo fileInfo(path);
		ui->openRecentProjectMenu->addAction(fileInfo.fileName(), [this, path] {OpenProject(path); });
	}
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

void MainWindow::OpenProject(const QString& path)
{
	if (!path.isEmpty())
	{
		try
		{
			m_document.LoadFromFile(path);
			m_typeSafeSettings.AddRecentProject(path);
			GenerateRecentProjectsMenu();
		}
		catch (std::exception& ex)
		{
			QMessageBox::warning(this, QString("Could not open the project:\n ") + ex.what(), path);
		}
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
			m_document.MarkDirty();
		}
		else
		{
			QMessageBox::warning(this, "Could not open the video.", path);
		}
	}
}

void MainWindow::ComputeWindowTitle()
{
	QString title = "Reference Tracker - ";

	const std::optional<QString> documentPath = m_document.GetFilePath();
	if (documentPath.has_value())
	{
		title += documentPath.value() + " ";
	}
	else
	{
		title += "Untitled Project ";
	}
	if (m_document.IsDirty())
	{
		title += "- Unsaved Changes";
	}
	setWindowTitle(title);
}


#pragma endregion

MainWindow::~MainWindow()
{
	qDebug() << "MainWindow::~MainWindow()";
	delete ui;
}
