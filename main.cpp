#include <QApplication>

#include "nfoviewer.h"

int main(int argc, char *argv[]) {
  QCoreApplication::setApplicationName("Tatsh NFO Viewr");
  QCoreApplication::setApplicationVersion("0.1");
  QCoreApplication::setOrganizationName("Tatsh");
  QCoreApplication::setOrganizationDomain("tatsh.net");

  QApplication app(argc, argv);
  MainWindow window;
  window.show();
  return app.exec();
}
