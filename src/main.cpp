#include <QApplication>
#include <QStringList>
#include <QTimer>

#include "nfoviewer.h"

int main(int argc, char *argv[]) {
    QApplication::setApplicationName(QStringLiteral("Tatsh NFO Viewer"));
    QApplication::setApplicationVersion(QStringLiteral("0.1.0"));
    QApplication::setOrganizationName(QStringLiteral("Tatsh"));
    QApplication::setOrganizationDomain(QStringLiteral("tatsh.net"));

    QApplication app(argc, argv);
    MainWindow window;
    window.show();

    // The optional first argument is a file to open; otherwise prompt for one.
    // Deferred to the first event-loop turn so the window is fully mapped before
    // the file is loaded and the window is fitted to it; this avoids a resize
    // race with the window manager during the initial map.
    const QStringList arguments = QApplication::arguments();
    const QString initialFile = arguments.size() > 1 ? arguments.at(1) : QString();
    QTimer::singleShot(
        0, &window, [&window, initialFile]() { window.openInitialFile(initialFile); });

    return app.exec();
}
