#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "TypeSafeSettings.h"
#include "VideoPlayer.h"

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
	void ManualUiSetup();
	void ApplyUiSettings();

	TypeSafeSettings m_typeSafeSettings;
	Ui::MainWindow* ui;
	VideoPlayer m_videoPlayer;
};

#endif // MAINWINDOW_H
