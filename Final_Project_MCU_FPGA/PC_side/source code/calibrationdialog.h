#ifndef CALIBRATIONDIALOG_H
#define CALIBRATIONDIALOG_H

#pragma once

#include <QDialog>
#include <QVector>

namespace Ui {
class CalibrationDialog;
}

class CalibrationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CalibrationDialog(QWidget *parent = nullptr);
    ~CalibrationDialog();

    QVector<uint16_t> getCalibrationData() const;

signals:
    void requestLDRSample(int step); // שליחת בקשת דגימה לבקר

public slots:
    void onNewSampleReceived(uint16_t value); // מופעל כשנכנסת דגימה בפועל

private slots:
    void onMeasureButtonClicked();
    void onCancelClicked();

private:
    Ui::CalibrationDialog *ui;
    QVector<uint16_t> samples;
    int currentStep;
};

#endif // CALIBRATIONDIALOG_H
