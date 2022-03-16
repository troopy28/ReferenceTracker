#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "TypeSafeSettings.h"
#include "VideoPlayer.h"
#include "../Data/Document.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget* parent = nullptr);
	~MainWindow() override;
	Q_DISABLE_COPY_MOVE(MainWindow);

	void resizeEvent(QResizeEvent* event) override;


private:
	// UI initialization.
	void ManualUiSetup();
	void ApplyUiSettings();

	// Menu bar callbacks.
	void OpenVideoMenuItemClicked();
	
	// Recent projects / videos menu management.
	void GenerateRecentVideosMenu();

	// MISC.
	void OpenVideo(const QString& path);

	Data::Document m_document;
	TypeSafeSettings m_typeSafeSettings;
	Ui::MainWindow* ui;
	VideoPlayer m_videoPlayer;
};

#endif // MAINWINDOW_H
