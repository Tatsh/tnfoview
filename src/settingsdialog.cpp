#include "settingsdialog.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFontDatabase>
#include <QFormLayout>
#include <QSpinBox>
#include <QStringList>
#include <QVBoxLayout>

namespace {
constexpr int kMinFontSize = 6;
constexpr int kMaxFontSize = 72;

// Proportional families that should be offered despite not being reported as
// fixed-pitch by the font database.
const QString kExtraFamilies[] = {QStringLiteral("Iosevka")};

QStringList monospacedFamilies() {
    const QStringList all = QFontDatabase::families();
    QStringList families;
    for (const QString &family : all) {
        if (QFontDatabase::isFixedPitch(family)) {
            families.append(family);
        }
    }
    for (const QString &family : kExtraFamilies) {
        if (all.contains(family) && !families.contains(family)) {
            families.append(family);
        }
    }
    families.sort(Qt::CaseInsensitive);
    return families;
}
} // namespace

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle(tr("Settings"));

    fontComboBox_ = new QComboBox(this);
    fontComboBox_->addItems(monospacedFamilies());

    fontSizeSpinBox_ = new QSpinBox(this);
    fontSizeSpinBox_->setRange(kMinFontSize, kMaxFontSize);
    fontSizeSpinBox_->setSuffix(tr(" px"));

    wrapCheckBox_ = new QCheckBox(tr("Wrap content"), this);
    autoFitCheckBox_ = new QCheckBox(tr("Auto-resize window for content"), this);

    auto *formLayout = new QFormLayout;
    formLayout->addRow(tr("Font:"), fontComboBox_);
    formLayout->addRow(tr("Font size:"), fontSizeSpinBox_);
    formLayout->addRow(wrapCheckBox_);
    formLayout->addRow(autoFitCheckBox_);

    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto *layout = new QVBoxLayout(this);
    layout->addLayout(formLayout);
    layout->addWidget(buttonBox);
}

void SettingsDialog::setFontFamily(const QString &family) {
    const int index = fontComboBox_->findText(family);
    if (index >= 0) {
        fontComboBox_->setCurrentIndex(index);
    } else if (!family.isEmpty()) {
        // Preserve a configured family even if it is no longer installed.
        fontComboBox_->insertItem(0, family);
        fontComboBox_->setCurrentIndex(0);
    }
}

void SettingsDialog::setFontSize(int pixelSize) {
    fontSizeSpinBox_->setValue(pixelSize);
}

void SettingsDialog::setWrapEnabled(bool enabled) {
    wrapCheckBox_->setChecked(enabled);
}

void SettingsDialog::setAutoFitWidth(bool enabled) {
    autoFitCheckBox_->setChecked(enabled);
}

QString SettingsDialog::fontFamily() const {
    return fontComboBox_->currentText();
}

int SettingsDialog::fontSize() const {
    return fontSizeSpinBox_->value();
}

bool SettingsDialog::wrapEnabled() const {
    return wrapCheckBox_->isChecked();
}

bool SettingsDialog::autoFitWidth() const {
    return autoFitCheckBox_->isChecked();
}
