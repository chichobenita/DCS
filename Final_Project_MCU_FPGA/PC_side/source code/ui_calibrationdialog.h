/********************************************************************************
** Form generated from reading UI file 'calibrationdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.18
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CALIBRATIONDIALOG_H
#define UI_CALIBRATIONDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_CalibrationDialog
{
public:
    QLabel *statusLabel;
    QLabel *progressLabel;
    QPushButton *measureButton;
    QPushButton *cancelButton;

    void setupUi(QDialog *CalibrationDialog)
    {
        if (CalibrationDialog->objectName().isEmpty())
            CalibrationDialog->setObjectName(QString::fromUtf8("CalibrationDialog"));
        CalibrationDialog->resize(611, 243);
        statusLabel = new QLabel(CalibrationDialog);
        statusLabel->setObjectName(QString::fromUtf8("statusLabel"));
        statusLabel->setGeometry(QRect(50, 30, 541, 61));
        QFont font;
        font.setPointSize(12);
        font.setBold(true);
        font.setItalic(true);
        statusLabel->setFont(font);
        progressLabel = new QLabel(CalibrationDialog);
        progressLabel->setObjectName(QString::fromUtf8("progressLabel"));
        progressLabel->setGeometry(QRect(50, 80, 511, 51));
        progressLabel->setFont(font);
        measureButton = new QPushButton(CalibrationDialog);
        measureButton->setObjectName(QString::fromUtf8("measureButton"));
        measureButton->setGeometry(QRect(90, 160, 75, 24));
        cancelButton = new QPushButton(CalibrationDialog);
        cancelButton->setObjectName(QString::fromUtf8("cancelButton"));
        cancelButton->setGeometry(QRect(290, 160, 75, 24));

        retranslateUi(CalibrationDialog);

        QMetaObject::connectSlotsByName(CalibrationDialog);
    } // setupUi

    void retranslateUi(QDialog *CalibrationDialog)
    {
        CalibrationDialog->setWindowTitle(QCoreApplication::translate("CalibrationDialog", "Dialog", nullptr));
        statusLabel->setText(QCoreApplication::translate("CalibrationDialog", "TextLabel", nullptr));
        progressLabel->setText(QCoreApplication::translate("CalibrationDialog", "TextLabel", nullptr));
        measureButton->setText(QCoreApplication::translate("CalibrationDialog", "measure", nullptr));
        cancelButton->setText(QCoreApplication::translate("CalibrationDialog", "cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CalibrationDialog: public Ui_CalibrationDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CALIBRATIONDIALOG_H
