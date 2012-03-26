#ifndef NFOVIEWER_H
#define NFOVIEWER_H

#include <QtGui>
#include <QCloseEvent>
#include <QFontMetrics>
#include <QMainWindow>
#include <QPainter>
#include <QSettings>
#include <QTextDocument>
#include <QTextEdit>
#include <QTextLayout>
#include <QTextLine>

class MainWindow : public QMainWindow {
  Q_OBJECT

  public:
    MainWindow();
    void QWarning(const QString &msg);

  protected:
    void closeEvent(QCloseEvent *event);
    void paintEvent(QPaintEvent *event);

  private slots: // In order from DAMN NFO Viewer
    void open_file();
    void text_wrap(int mode);
    void about();
    // void visit_website();

  private:
    void create_actions();
    void create_toolbar();
    void create_statusbar();
    void read_settings();
    void write_settings();
    void load_file(const QString &filename);
    void set_current_file(const QString &filename);
    QString stripped_name(const QString &full_filename);

    QTextDocument *doc;
    QTextEdit *text;
    QTextLayout *layout;

    QString current_file;

    QToolBar *toolbar; // In order from DAMN NFO Viewer
    QAction *open_act;
    QAction *settings_act;
    QAction *copy_act;
    QAction *zoomin_act;
    QAction *zoomout_act;
    QAction *zoom100_act;
    QAction *text_wrap_none_act;
    QAction *text_wrap_char_act;
    QAction *text_wrap_word_act;
    QAction *about_act;
    QAction *about_qt_act;
    QAction *exit_act;

    QSettings settings;
};

#endif // NFOVIEWER_H
