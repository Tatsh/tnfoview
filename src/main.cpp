#include <QApplication>
#include <QIcon>
#include <QSize>
#include <QStringList>
#include <QTimer>

#include "nfoviewer.h"

int main(int argc, char *argv[]) {
    QApplication::setApplicationName(QStringLiteral("Tatsh NFO Viewer"));
    QApplication::setApplicationVersion(QStringLiteral("0.1.0"));
    QApplication::setOrganizationName(QStringLiteral("Tatsh"));
    QApplication::setOrganizationDomain(QStringLiteral("tatsh.net"));
    // Match the installed desktop file so Wayland associates the window with it
    // and shows its icon.
    QApplication::setDesktopFileName(QStringLiteral("sh.tat.tnfoview"));

    QApplication app(argc, argv);

    // Prefer the installed themed icon, falling back to the icons bundled in the
    // binary, so every platform shows an application icon even when running
    // uninstalled.
    QIcon windowIcon;
    for (const auto size : {16, 32, 48, 64, 128, 256}) {
        windowIcon.addFile(QStringLiteral(":/icons/icon_%1.png").arg(size), QSize(size, size));
    }
    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("sh.tat.tnfoview"), windowIcon));

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
