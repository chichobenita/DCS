#include "calibrationdialog.h"
#include "ui_calibrationdialog.h"

CalibrationDialog::CalibrationDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CalibrationDialog)
    , currentStep(0)
{
    ui->setupUi(this);

    samples.resize(10); // 10 דגימות מ-0 עד 45 ס"מ בקפיצות של 5

    // כפתורים
    connect(ui->measureButton,
            &QPushButton::clicked,
            this,
            &CalibrationDialog::onMeasureButtonClicked);

    connect(ui->cancelButton, &QPushButton::clicked, this, &CalibrationDialog::onCancelClicked);

    // עדכון תצוגה ראשוני
    ui->statusLabel->setText("Place the flashlight at a distance of 5 cm and press 'Measure'");
    ui->progressLabel->setText(QString("Measurement %1 of 10").arg(currentStep+1));

}

CalibrationDialog::~CalibrationDialog()
{
    delete ui;
}

void CalibrationDialog::onMeasureButtonClicked()
{
    if (currentStep >= 10)
        return;

    // שלח סיגנל כדי לבקש דגימה מהבקר
    emit requestLDRSample(currentStep);

    // כפתור נחסם זמנית עד שתגיע דגימה
    ui->measureButton->setEnabled(false);
    ui->statusLabel->setText("Sampling...");
}

void CalibrationDialog::onNewSampleReceived(uint16_t value)
{
    if (currentStep >= 10)
        return;

    // שומרים במיקום התואם (0..9)
    samples[currentStep] = value;
    currentStep++;

    if (currentStep < 10) {
        // מכינים את ההנחיה למדידה הבאה (0,5,10,...,45)
        int next_cm = currentStep * 5;
        ui->statusLabel->setText(
            QString("Place the flashlight at a distance of %1 cm and press 'Measure'").arg(next_cm));
        ui->progressLabel->setText(
            QString("Measurement %1 of 10").arg(currentStep + 1));
        ui->measureButton->setEnabled(true);
    } else {
        // הושלם
        ui->statusLabel->setText("Calibration completed!");
        ui->progressLabel->setText("10 of 10");
        ui->measureButton->setEnabled(false);
        ui->cancelButton->setText("Close");
    }
}

void CalibrationDialog::onCancelClicked()
{

    //reject(); // סוגר את הדיאלוג
    this->hide();
}


QVector<uint16_t> CalibrationDialog::getCalibrationData() const
{
    return samples;
}
