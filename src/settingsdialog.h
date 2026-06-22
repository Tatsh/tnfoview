/** @file */
#pragma once

#include <QDialog>
#include <QString>

class QCheckBox;
class QComboBox;
class QSpinBox;

/** @brief Modal dialog for editing the viewer's font and text-wrapping preferences. */
class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Construct the dialog and populate the font list.
     * @param parent Parent widget, or nullptr.
     */
    explicit SettingsDialog(QWidget *parent = nullptr);

    /**
     * @brief Select the given font family in the combo box.
     * @param family Font family name; inserted if not already listed.
     */
    void setFontFamily(const QString &family);
    /**
     * @brief Set the font size shown in the spin box.
     * @param pixelSize Font size in pixels.
     */
    void setFontSize(int pixelSize);
    /**
     * @brief Set the wrap checkbox state.
     * @param enabled Whether wrapping is enabled.
     */
    void setWrapEnabled(bool enabled);
    /**
     * @brief Set the automatic-width-fitting checkbox state.
     * @param enabled Whether automatic fitting is enabled.
     */
    void setAutoFitWidth(bool enabled);

    /**
     * @brief Selected font family.
     * @return The selected font family name.
     */
    QString fontFamily() const;
    /**
     * @brief Chosen font size.
     * @return The font size in pixels.
     */
    int fontSize() const;
    /**
     * @brief Whether wrapping is enabled.
     * @return True when the wrap checkbox is checked.
     */
    bool wrapEnabled() const;
    /**
     * @brief Whether automatic window-width fitting is enabled.
     * @return True when the auto-fit checkbox is checked.
     */
    bool autoFitWidth() const;

private:
    QComboBox *fontComboBox_;
    QSpinBox *fontSizeSpinBox_;
    QCheckBox *wrapCheckBox_;
    QCheckBox *autoFitCheckBox_;
};
