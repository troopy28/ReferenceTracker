#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QSplitter>
#include <QListView>
#include <QTextEdit>
#include <QResizeEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QDebug>

MainWindow::MainWindow(QWidget* parent) :
	QMainWindow(parent),
	m_document(),
	m_typeSafeSettings(),
	ui(new Ui::MainWindow),
	m_videoPlayer(m_document.GetVideo(), this)
{
	ui->setupUi(this);
	ManualUiSetup();
	ApplyUiSettings();

	connect(ui->actionOpen_Video, &QAction::triggered, this, &MainWindow::OpenVideoMenuItemClicked);

	m_videoPlayer.Render(0);
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
	delete ui;
}
