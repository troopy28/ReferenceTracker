#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "TypeSafeSettings.h"
#include "../Data/Document.h"
#include "VideoPlayer.h"
#include "TrackedPointsList.h"
#include "GraphView.h"

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
	void OpenProjectMenuItemClicked();
	void OpenVideoMenuItemClicked();
	void SaveMenuItemClicked();
	void SaveAsMenuItemClicked();

	void GenerateRecentProjectsMenu();
	void GenerateRecentVideosMenu();

	void OpenProject(const QString& path);
	void OpenVideo(const QString& path);


	// Window management.
	void ComputeWindowTitle();

	Data::Document m_document;
	TypeSafeSettings m_typeSafeSettings;
	/**
	 * \brief Undo stack managed by Qt to allow for undo/redo.
	 */
	QUndoStack m_undoStack;
	Ui::MainWindow* ui;
	VideoPlayer* m_videoPlayer;
	TrackedPointsList* m_trackedPointsList;
	GraphView* m_graphView;

};

#endif // MAINWINDOW_H
