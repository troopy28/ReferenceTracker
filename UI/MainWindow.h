#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "TypeSafeSettings.h"
#include "../Data/Document.h"
#include "VideoPlayer.h"
#include "TrackedPointsList.h"

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

protected:
	void changeEvent(QEvent* e) override;

private:
	// UI initialization.
	void ManualUiSetup();
	void ApplyUiSettings();

	// Menu bar management.
	void OpenVideoMenuItemClicked();
	void GenerateRecentVideosMenu();
	void OpenVideo(const QString& path);


	Data::Document m_document;
	TypeSafeSettings m_typeSafeSettings;
	Ui::MainWindow* ui;
	VideoPlayer* m_videoPlayer;
	TrackedPointsList* m_graphView;
};

#endif // MAINWINDOW_H
