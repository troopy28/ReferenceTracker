#include <QApplication>
#include <QFile>

#include "UI/MainWindow.h"


/**
 * \brief Entry point of the program.
 * \param argc Number of arguments.
 * \param argv Value of the arguments.
 * \return Exit code.
 */
int main(int argc, char** argv)
{
    // Needed on some platforms, apparently.
    Q_INIT_RESOURCE(resources);

    QApplication a(argc, argv);

    // Apply the stylesheet.
    QFile styleFile(":/Resources/style.css");
    styleFile.open(QFile::ReadOnly);
    const QString style(styleFile.readAll());
    a.setStyleSheet(style);

    // Load the main window.
    MainWindow mainWindow;
    mainWindow.show();

    // Start the event loop.
    return QApplication::exec();
}