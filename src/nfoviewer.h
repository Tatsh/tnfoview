/** @file */
#pragma once

#include <QMainWindow>
#include <QSettings>
#include <QString>

class QAction;
class QCloseEvent;
class QTextEdit;
class QToolBar;

/** @brief Main application window that displays an NFO file rendered from CP437. */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow();

    /**
     * @brief Load the given file on startup, or prompt for one when the path is empty.
     * @param fileName Path to the NFO file, or an empty string to prompt.
     */
    void openInitialFile(const QString &fileName);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    /** @brief Create the actions used by the toolbar. */
    void createActions();
    /** @brief Populate the toolbar with the actions. */
    void createToolBar();
    /** @brief Restore the window geometry and state saved on the previous quit. */
    void readSettings();
    /** @brief Prompt for a file and load it when one is chosen. */
    void openFile();
    /**
     * @brief Decode the given file from CP437 and display it.
     * @param fileName Path to the NFO file to load.
     */
    void loadFile(const QString &fileName);
    /** @brief Remove inter-line leading so box-drawing characters connect vertically. */
    void applyLineSpacing();
    /**
     * @brief Window width needed to show the given text pixel width without a horizontal scrollbar.
     *
     * Includes the document margins, slack, frame, and the vertical scrollbar.
     * @param textWidthPixels Width of the widest line in pixels.
     * @return The window width in pixels.
     */
    int contentWindowWidth(qreal textWidthPixels) const;
    /**
     * @brief Resize the window width to fit the widest line without a horizontal scrollbar.
     *
     * Clamped to the screen, and a no-op while wrapping or when the document is empty.
     */
    void fitWidthToContents();
    /** @brief Set the window width to comfortably hold an average-width NFO on launch. */
    void applyDefaultWidth();
    /** @brief Apply the stored font and the live wrapping state to the text view. */
    void applyViewSettings();
    /** @brief Apply the current wrap state to the text view and sync the toggle action. */
    void applyWrapMode();
    /**
     * @brief Handle the temporary wrap toggle without altering settings.
     * @param enabled Whether wrapping should be enabled.
     */
    void onWrapToggled(bool enabled);
    /** @brief Open the settings dialog and persist any accepted changes. */
    void openSettings();
    /**
     * @brief Configured base text size in pixels, before any temporary zoom.
     * @return The base font size in pixels.
     */
    int baseFontSize() const;
    /**
     * @brief Temporarily change the on-screen text size without altering settings.
     * @param deltaPixels Signed pixel amount to add to the current text size.
     */
    void changeTextSize(int deltaPixels);
    /** @brief Increase the on-screen text size by one step. */
    void increaseTextSize();
    /** @brief Decrease the on-screen text size by one step. */
    void decreaseTextSize();
    /** @brief Reset the temporary text-size change and fit the window width. */
    void resetTextSize();
    /**
     * @brief Update the window title and modified state for the given file.
     * @param fileName Path of the current file, or an empty string for none.
     */
    void setCurrentFile(const QString &fileName);
    /** @brief Show the about dialog. */
    void about();
    /**
     * @brief Show a warning dialog with the given message.
     * @param message Text to display.
     */
    void warn(const QString &message);

    QTextEdit *text_;
    QString currentFile_;
    /** Family name of the bundled font once registered, otherwise empty. */
    QString embeddedFontFamily_;
    /**
     * Transient text-size delta in pixels applied on top of the configured size
     * by the increase and decrease actions. Never persisted.
     */
    int fontSizeOffset_ = 0;
    /**
     * Whether the loaded document's lines are narrow enough (a bounded canvas
     * rather than free-flowing prose) to drive automatic window-width fitting.
     * Recomputed on each load.
     */
    bool contentFittable_ = false;
    /**
     * Live word-wrapping state. Seeded from settings, toggled transiently by the
     * wrap action without being written back.
     */
    bool wrapEnabled_ = false;
    /**
     * Whether the window width is fitted to the text automatically on load and
     * when changing the text size. The reset action fits regardless.
     */
    bool autoFitEnabled_ = true;
    QToolBar *toolBar_;
    QAction *openAct_;
    QAction *copyAct_;
    QAction *increaseTextSizeAct_;
    QAction *decreaseTextSizeAct_;
    QAction *resetTextSizeAct_;
    QAction *wrapAct_;
    QAction *settingsAct_;
    QAction *aboutAct_;
    QSettings settings_;
};
