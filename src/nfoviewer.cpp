#include "nfoviewer.h"
#include "cp437.h"

/**
 * @return MainWindow
 */
MainWindow::MainWindow() {
  text = new QTextEdit;
  text->setFontFamily("Monospace");
  text->setFontPointSize(8);
  text->setReadOnly(true);
  text->setAcceptRichText(false);

  doc = new QTextDocument;
  layout = new QTextLayout;

  setCentralWidget(text);

  create_actions();
  create_toolbar();
  create_statusbar();

  read_settings();

  set_current_file("");
}

void MainWindow::open_file() {
  QString filename = QFileDialog::getOpenFileName(this);
  if (!filename.isEmpty()) {
    load_file(filename);
  }
}

void MainWindow::about() {
  QMessageBox::about(this, tr("About"), tr("Tatsh NFO Viewer 0.1"));
}

void MainWindow::create_actions() {
  open_act = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
  open_act->setShortcuts(QKeySequence::Open);
  open_act->setStatusTip(tr("Open an existing document"));
  connect(open_act, SIGNAL(triggered()), this, SLOT(open_file()));

  settings_act = new QAction(tr("&Settings"), this);
  settings_act->setStatusTip(tr("Open the settings dialog"));
  connect(settings_act, SIGNAL(triggered()), this, SLOT(settings()));

  copy_act = new QAction(QIcon(":/images/copy.png"), tr("&Copy"), this);
  copy_act->setShortcuts(QKeySequence::Copy);
  copy_act->setStatusTip(tr("Copy the currently selected text to the clipboard"));
  connect(copy_act, SIGNAL(triggered()), text, SLOT(copy()));

  text_wrap_none_act = new QAction(QIcon(":/images/nowrap.png"), tr("&Disable text wrapping"), this);
  text_wrap_none_act->setStatusTip(tr("Disable text wrapping"));
  connect(text_wrap_none_act, SIGNAL(triggered()), this, SLOT(text_wrap(int)));

  text_wrap_char_act = new QAction(tr("Character wrap"), this);
  text_wrap_char_act->setStatusTip(tr("Set wrapping mode to character wrap"));
  connect(text_wrap_char_act, SIGNAL(triggered()), this, SLOT(text_wrap(int)));

  text_wrap_word_act = new QAction(tr("Word wrap"), this);
  text_wrap_word_act->setStatusTip(tr("Set wrapping mode to word wrap"));
  connect(text_wrap_word_act, SIGNAL(triggered()), this, SLOT(text_wrap(int)));

  about_act = new QAction(tr("&About"), this);
  about_act->setStatusTip(tr("About Tatsh NFO Viewer"));
  connect(about_act, SIGNAL(triggered()), this, SLOT(about()));

  copy_act->setEnabled(false);
  connect(text, SIGNAL(copyAvailable(bool)), copy_act, SLOT(setEnabled(bool)));
}

void MainWindow::create_toolbar() {
  toolbar = addToolBar(tr("Main"));
  toolbar->addAction(open_act);
  toolbar->addAction(copy_act);
  toolbar->addAction(text_wrap_none_act);
  toolbar->addAction(text_wrap_char_act);
  toolbar->addAction(text_wrap_word_act);
  toolbar->addAction(about_act);
}

void MainWindow::create_statusbar() {
  statusBar()->showMessage(tr("Ready"));
}

void MainWindow::load_file(const QString &filename) {
  QFile file(filename);
  if (!file.open(QFile::ReadOnly | QFile::Text)) {
    QWarning(tr("Cannot read %1\n%2").arg(filename).arg(file.errorString()));
    return;
  }

  QTextStream stream(&file);

  QCodePage437Codec *codec = new QCodePage437Codec;
  stream.setCodec(codec);

  QApplication::setOverrideCursor(Qt::WaitCursor);

  doc->setPlainText(stream.readAll());

//   QFont monospace = QFont("Monospace", 8);
//   QFontMetrics font_metrics(monospace);
//   int leading = font_metrics.leading();
//   qreal height = 0;
//   qreal width_used = 0;
//   layout->setText(doc->toPlainText());
//   layout->beginLayout();
//   while (1) {
//     QTextLine line = layout->createLine();
//     if (!line.isValid()) {
//       break;
//     }
//
//     line.setLeadingIncluded(true);
//     height += leading;
//     line.setPosition(QPointF(0, height));
//     height += line.height();
//     width_used = qMax(width_used, line.naturalTextWidth());
//   }
//   layout->endLayout();

  text->setPlainText(doc->toPlainText());
  QApplication::restoreOverrideCursor();

  set_current_file(filename);
  statusBar()->showMessage(tr("Successfully loaded %1").arg(filename), 2000);
}

/**
  * Because QTextEdit lacks a line-height property and does not understand line-height from CSS, we use a paint device.
  * @param QPaintEvent event
  * @return void
  */
void MainWindow::paintEvent(QPaintEvent *event) {
  (void)event;
  QPainter painter(this);
  layout->draw(&painter, QPoint(0, 0));
}

/**
 * @param QString filename
 * @return void
 */
void MainWindow::set_current_file(const QString &filename) {
  QString shown_name = "";

  current_file = filename;
  text->document()->setModified(false);
  setWindowModified(false);

  if (current_file.isEmpty()) {
    setWindowTitle(tr("Tatsh NFO Viewer"));
  }
  else {
    shown_name = stripped_name(current_file);
    setWindowTitle(tr("%1 - %2").arg(shown_name).arg("Tatsh NFO Viewer"));
  }
}

/**
 * @param int mode
 * @return void
 */
void MainWindow::text_wrap(int mode) {
  switch(mode) {
    default:
      text->setWordWrapMode(QTextOption::WrapAnywhere);
      break;
  }
  return;
}

/**
 * @param QString full_filename
 * @return QString
 */
QString MainWindow::stripped_name(const QString &full_filename) {
  return QFileInfo(full_filename).fileName();
}

/**
 * @param QCloseEvent event
 * @return void
 */
void MainWindow::closeEvent(QCloseEvent *event) {
  settings.beginGroup("mainwindow");
  settings.setValue("geometry", saveGeometry());
  settings.setValue("window_state", saveState());
  settings.endGroup();
  QMainWindow::closeEvent(event);
}

void MainWindow::read_settings() {
  // Restore window geometry from last session
  restoreGeometry(settings.value("mainwindow/geometry").toByteArray());
  restoreState(settings.value("mainwindow/window_state").toByteArray());
}

/**
  * Wrapper for QMessageBox::warning().
  * @param msg Message to use, must be placed in tr(). Can include extra arguments
  *   (arg("%1"), etc).
  */
void MainWindow::QWarning(const QString &msg) {
  QMessageBox::warning(this, tr("Warning"), msg);
  return;
}
