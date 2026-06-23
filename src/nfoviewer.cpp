#include "nfoviewer.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <memory>

#include <QAction>
#include <QApplication>
#include <QByteArray>
#include <QCloseEvent>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFont>
#include <QFontDatabase>
#include <QFontMetricsF>
#include <QIcon>
#include <QKeySequence>
#include <QMessageBox>
#include <QScreen>
#include <QScrollBar>
#include <QStringList>
#include <QTextBlock>
#include <QTextBlockFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextEdit>
#include <QTextOption>
#include <QTimer>
#include <QToolBar>

#include "cp437.h"
#include "settingsdialog.h"

namespace {
// The bundled unscii font is an 8x16 cell font, so render it at its native
// pixel height for crisp, gap-free character-cell art.
constexpr int kFontPixelSize = 16;
constexpr int kFontPixelSizeStep = 2;
constexpr int kMinFontPixelSize = 8;
constexpr int kMaxFontPixelSize = 72;

// Automatic width fitting only applies to documents that read as a bounded
// canvas (NFO art) rather than free-flowing prose. The discriminator is simply
// the widest line: art stays within a normal column count, prose does not.
constexpr int kMaxFitColumns = 256;
// Leave a few characters of breathing room on the right so the fit is not
// pinned to the very last glyph on the longest line (also absorbs window-manager
// rounding at fractional display scales).
constexpr int kFitSlackColumns = 6;
// The classic NFO canvas width, used for the launch window size when auto-fit
// is enabled and no file has been loaded yet.
constexpr int kDefaultNfoColumns = 80;

// Decide whether the widest line is narrow enough for automatic width fitting.
bool hasFittableLineWidths(const QString &text) {
    int maxLength = 0;
    bool hasContent = false;
    const QStringList lines = text.split(u'\n');
    for (const QString &line : lines) {
        const int length = static_cast<int>(line.length());
        if (length > 0) {
            hasContent = true;
            maxLength = std::max(maxLength, length);
        }
    }
    return hasContent && maxLength <= kMaxFitColumns;
}
} // namespace

MainWindow::MainWindow() {
    text_ = new QTextEdit(this);
    text_->setReadOnly(true);
    text_->setAcceptRichText(false);
    setCentralWidget(text_);

    // Register the bundled CP437-complete font so it is available as a choice
    // alongside the system fonts. It is the default but never forced: the user
    // can pick any monospaced family in the settings dialog.
    const int fontId = QFontDatabase::addApplicationFont(QStringLiteral(":/unscii-16.ttf"));
    if (fontId != -1) {
        const QStringList families = QFontDatabase::applicationFontFamilies(fontId);
        if (!families.isEmpty()) {
            embeddedFontFamily_ = families.constFirst();
        }
    }

    createActions();
    createToolBar();

    wrapEnabled_ = settings_.value(QStringLiteral("view/wrapEnabled"), false).toBool();
    autoFitEnabled_ = settings_.value(QStringLiteral("view/autoFit"), true).toBool();
    applyViewSettings();
    readSettings();

    // When auto-fitting, the width is derived rather than remembered, so open at
    // a comfortable average-NFO width instead of the restored width.
    if (autoFitEnabled_) {
        applyDefaultWidth();
    }

    setCurrentFile(QString());
}

void MainWindow::createActions() {
    openAct_ = new QAction(QIcon::fromTheme(QStringLiteral("document-open")), tr("&Open..."), this);
    openAct_->setShortcuts(QKeySequence::Open);
    connect(openAct_, &QAction::triggered, this, &MainWindow::openFile);

    copyAct_ = new QAction(QIcon::fromTheme(QStringLiteral("edit-copy")), tr("&Copy"), this);
    copyAct_->setShortcuts(QKeySequence::Copy);
    copyAct_->setEnabled(false);
    connect(copyAct_, &QAction::triggered, text_, &QTextEdit::copy);
    connect(text_, &QTextEdit::copyAvailable, copyAct_, &QAction::setEnabled);

    increaseTextSizeAct_ =
        new QAction(QIcon::fromTheme(QStringLiteral("zoom-in")), tr("&Increase text size"), this);
    increaseTextSizeAct_->setShortcuts(QKeySequence::ZoomIn);
    connect(increaseTextSizeAct_, &QAction::triggered, this, &MainWindow::increaseTextSize);

    decreaseTextSizeAct_ =
        new QAction(QIcon::fromTheme(QStringLiteral("zoom-out")), tr("&Decrease text size"), this);
    decreaseTextSizeAct_->setShortcuts(QKeySequence::ZoomOut);
    connect(decreaseTextSizeAct_, &QAction::triggered, this, &MainWindow::decreaseTextSize);

    resetTextSizeAct_ = new QAction(
        QIcon::fromTheme(QStringLiteral("zoom-original")), tr("&Reset text size"), this);
    resetTextSizeAct_->setShortcut(QKeySequence(QStringLiteral("Ctrl+0")));
    connect(resetTextSizeAct_, &QAction::triggered, this, &MainWindow::resetTextSize);

    wrapAct_ = new QAction(QIcon::fromTheme(QStringLiteral("text-wrap")), tr("&Wrap text"), this);
    wrapAct_->setCheckable(true);
    connect(wrapAct_, &QAction::triggered, this, &MainWindow::onWrapToggled);

    settingsAct_ =
        new QAction(QIcon::fromTheme(QStringLiteral("configure"),
                                     QIcon::fromTheme(QStringLiteral("preferences-system"))),
                    tr("&Settings..."),
                    this);
    settingsAct_->setShortcuts(QKeySequence::Preferences);
    connect(settingsAct_, &QAction::triggered, this, &MainWindow::openSettings);

    aboutAct_ = new QAction(QIcon::fromTheme(QStringLiteral("help-about")), tr("&About"), this);
    connect(aboutAct_, &QAction::triggered, this, &MainWindow::about);
}

void MainWindow::createToolBar() {
    toolBar_ = addToolBar(tr("Main"));
    // Lock the toolbar in place: not draggable, not floatable, and confined to
    // the top area.
    toolBar_->setMovable(false);
    toolBar_->setFloatable(false);
    toolBar_->setAllowedAreas(Qt::TopToolBarArea);
    toolBar_->addAction(openAct_);
    toolBar_->addAction(copyAct_);
    toolBar_->addAction(increaseTextSizeAct_);
    toolBar_->addAction(decreaseTextSizeAct_);
    toolBar_->addAction(resetTextSizeAct_);
    toolBar_->addAction(wrapAct_);
    toolBar_->addAction(settingsAct_);
    toolBar_->addAction(aboutAct_);
}

void MainWindow::openFile() {
    const QString fileName = QFileDialog::getOpenFileName(
        this, tr("Open"), QString(), tr("NFO files (*.nfo);;All files (*)"));
    if (!fileName.isEmpty()) {
        loadFile(fileName);
    }
}

void MainWindow::openInitialFile(const QString &fileName) {
    if (fileName.isEmpty()) {
        openFile();
    } else {
        loadFile(fileName);
    }
}

void MainWindow::loadFile(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        warn(tr("Cannot read %1.\n%2").arg(fileName, file.errorString()));
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    const QByteArray bytes = file.readAll();
    const std::unique_ptr<char, decltype(&std::free)> decoded(
        cp437_to_utf8(reinterpret_cast<const unsigned char *>(bytes.constData()),
                      static_cast<std::size_t>(bytes.size())),
        std::free);
    if (!decoded) {
        QApplication::restoreOverrideCursor();
        warn(tr("Ran out of memory while decoding %1.").arg(fileName));
        return;
    }

    // NFO files use CR/LF line endings; drop the carriage returns so QTextEdit
    // does not render stray control characters.
    QString contents = QString::fromUtf8(decoded.get());
    contents.remove(u'\r');
    text_->setPlainText(contents);
    contentFittable_ = hasFittableLineWidths(contents);
    applyLineSpacing();

    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);

    // Size the window to the art so no horizontal scrollbar is needed (when the
    // line widths justify it and the user has not disabled auto-fitting).
    // Deferred to the next event-loop turn so it runs after any restored
    // geometry and the file dialog's geometry events have settled; otherwise a
    // synchronous resize can be overridden by them.
    if (autoFitEnabled_) {
        QTimer::singleShot(0, this, &MainWindow::fitWidthToContents);
    }
}

void MainWindow::applyLineSpacing() {
    // NFO art assumes glyph cells tile with no extra leading between rows.
    // Pin every line to the font's exact cell height so vertical box-drawing
    // characters and block elements connect across line boundaries.
    const QFontMetricsF metrics(text_->font());
    QTextBlockFormat blockFormat;
    blockFormat.setLineHeight(metrics.height(), QTextBlockFormat::FixedHeight);
    QTextCursor cursor(text_->document());
    cursor.select(QTextCursor::Document);
    cursor.mergeBlockFormat(blockFormat);
}

int MainWindow::contentWindowWidth(qreal textWidthPixels) const {
    // Reserve room for the document margins, a few characters of slack, the
    // frame, and the vertical scrollbar (always, since it may appear once the
    // content is taller than the window) so no horizontal scrollbar is needed.
    const QFontMetricsF metrics(text_->font());
    const qreal slack = kFitSlackColumns * metrics.averageCharWidth();
    const int viewportWidth = static_cast<int>(
        std::ceil(textWidthPixels + 2.0 * text_->document()->documentMargin() + slack));
    const int frame = 2 * text_->frameWidth();
    const int scrollBarWidth = text_->verticalScrollBar()->sizeHint().width();
    // The central QTextEdit spans the window's content width, so its required
    // width is the window width to request. Deliberately independent of the
    // current geometry so the result is stable even before the window is shown.
    return viewportWidth + frame + scrollBarWidth;
}

void MainWindow::fitWidthToContents() {
    if (!contentFittable_ || text_->lineWrapMode() != QTextEdit::NoWrap ||
        text_->document()->isEmpty()) {
        return;
    }

    // Measure the widest line directly with font metrics so the result does not
    // depend on the document's layout state or the current widget geometry.
    // horizontalAdvance handles proportional families (such as Iosevka) too.
    const QFontMetricsF metrics(text_->font());
    qreal widestLine = 0.0;
    const QTextDocument *doc = text_->document();
    for (QTextBlock block = doc->begin(); block.isValid(); block = block.next()) {
        widestLine = std::max(widestLine, metrics.horizontalAdvance(block.text()));
    }

    const int targetWidth = contentWindowWidth(widestLine);

    // If the art is wider than the screen, fitting cannot remove the horizontal
    // scrollbar, so leave the window alone and let the user enable wrapping.
    if (const QScreen *currentScreen = screen()) {
        if (targetWidth > currentScreen->availableGeometry().width()) {
            return;
        }
    }
    if (targetWidth != width()) {
        resize(targetWidth, height());
    }
}

void MainWindow::applyDefaultWidth() {
    // Metrics-based so the size tracks the font and the display's scaling (for
    // example roughly 940 px for 80 columns on a 4K screen at 150%).
    const QFontMetricsF metrics(text_->font());
    const int targetWidth = contentWindowWidth(kDefaultNfoColumns * metrics.averageCharWidth());
    if (targetWidth != width()) {
        resize(targetWidth, height());
    }
}

int MainWindow::baseFontSize() const {
    return settings_.value(QStringLiteral("view/fontSize"), kFontPixelSize).toInt();
}

void MainWindow::applyViewSettings() {
    const QString defaultFamily =
        embeddedFontFamily_.isEmpty() ? QStringLiteral("Monospace") : embeddedFontFamily_;
    const QString family =
        settings_.value(QStringLiteral("view/fontFamily"), defaultFamily).toString();

    QFont font(family);
    // The persisted base size plus the transient zoom offset gives the size on
    // screen; the offset is never written back to settings.
    font.setPixelSize(
        std::clamp(baseFontSize() + fontSizeOffset_, kMinFontPixelSize, kMaxFontPixelSize));
    font.setStyleHint(QFont::Monospace);
    text_->setFont(font);

    applyWrapMode();
    applyLineSpacing();
}

void MainWindow::applyWrapMode() {
    if (wrapEnabled_) {
        text_->setLineWrapMode(QTextEdit::WidgetWidth);
        text_->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    } else {
        text_->setLineWrapMode(QTextEdit::NoWrap);
    }
    if (wrapAct_ != nullptr) {
        wrapAct_->setChecked(wrapEnabled_);
    }
}

void MainWindow::onWrapToggled(bool enabled) {
    wrapEnabled_ = enabled;
    applyWrapMode();
    // Turning wrapping off may leave the window too narrow or too wide.
    fitWidthToContents();
}

void MainWindow::openSettings() {
    const QString defaultFamily =
        embeddedFontFamily_.isEmpty() ? QStringLiteral("Monospace") : embeddedFontFamily_;
    SettingsDialog dialog(this);
    dialog.setFontFamily(
        settings_.value(QStringLiteral("view/fontFamily"), defaultFamily).toString());
    dialog.setFontSize(settings_.value(QStringLiteral("view/fontSize"), kFontPixelSize).toInt());
    dialog.setWrapEnabled(settings_.value(QStringLiteral("view/wrapEnabled"), false).toBool());
    dialog.setAutoFitWidth(settings_.value(QStringLiteral("view/autoFit"), true).toBool());
    if (dialog.exec() == QDialog::Accepted) {
        settings_.setValue(QStringLiteral("view/fontFamily"), dialog.fontFamily());
        settings_.setValue(QStringLiteral("view/fontSize"), dialog.fontSize());
        settings_.setValue(QStringLiteral("view/wrapEnabled"), dialog.wrapEnabled());
        settings_.setValue(QStringLiteral("view/autoFit"), dialog.autoFitWidth());
        // The dialog sets the size and wrapping explicitly, so drop the
        // transient zoom offset and adopt the configured wrap state.
        fontSizeOffset_ = 0;
        wrapEnabled_ = dialog.wrapEnabled();
        autoFitEnabled_ = dialog.autoFitWidth();
        applyViewSettings();
    }
}

void MainWindow::changeTextSize(int deltaPixels) {
    const int base = baseFontSize();
    const int currentSize =
        std::clamp(base + fontSizeOffset_, kMinFontPixelSize, kMaxFontPixelSize);
    const int newSize = std::clamp(currentSize + deltaPixels, kMinFontPixelSize, kMaxFontPixelSize);
    if (newSize == currentSize) {
        return;
    }
    // Adjust only the transient offset; the configured size is left untouched.
    fontSizeOffset_ = newSize - base;
    applyViewSettings();
    if (autoFitEnabled_) {
        fitWidthToContents();
    }
}

void MainWindow::increaseTextSize() {
    changeTextSize(kFontPixelSizeStep);
}

void MainWindow::decreaseTextSize() {
    changeTextSize(-kFontPixelSizeStep);
}

void MainWindow::resetTextSize() {
    // Reset any temporary zoom, then refit the window width to the text (a no-op
    // when wrapping is enabled or the content is not fittable).
    if (fontSizeOffset_ != 0) {
        fontSizeOffset_ = 0;
        applyViewSettings();
    }
    fitWidthToContents();
}

void MainWindow::setCurrentFile(const QString &fileName) {
    currentFile_ = fileName;
    text_->document()->setModified(false);
    setWindowModified(false);

    if (currentFile_.isEmpty()) {
        setWindowTitle(tr("Tatsh NFO Viewer"));
    } else {
        setWindowTitle(
            tr("%1 - %2").arg(QFileInfo(currentFile_).fileName(), tr("Tatsh NFO Viewer")));
    }
}

void MainWindow::about() {
    QMessageBox::about(
        this, tr("About"), tr("Tatsh NFO Viewer %1").arg(QApplication::applicationVersion()));
}

void MainWindow::warn(const QString &message) {
    QMessageBox::warning(this, tr("Warning"), message);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    settings_.beginGroup(QStringLiteral("mainwindow"));
    settings_.setValue(QStringLiteral("geometry"), saveGeometry());
    settings_.setValue(QStringLiteral("window_state"), saveState());
    settings_.endGroup();
    QMainWindow::closeEvent(event);
}

void MainWindow::readSettings() {
    restoreGeometry(settings_.value(QStringLiteral("mainwindow/geometry")).toByteArray());
    restoreState(settings_.value(QStringLiteral("mainwindow/window_state")).toByteArray());
}
