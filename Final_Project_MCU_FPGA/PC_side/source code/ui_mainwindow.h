/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.18
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGroupBox *groupBox;
    QComboBox *commBox;
    QLabel *label;
    QPushButton *connectButton;
    QPushButton *refreshButton;
    QGroupBox *groupBox_2;
    QPushButton *uploadButton;
    QPushButton *downloadButton;
    QPushButton *startScriptButton;
    QComboBox *fileNameComboBox;
    QPushButton *removeFileButton;
    QGroupBox *groupBox_3;
    QLineEdit *delayEdit;
    QLabel *label_2;
    QLineEdit *xInputEdit;
    QLabel *label_3;
    QLabel *label_4;
    QLineEdit *degreeEdit;
    QLineEdit *rightAngleEdit;
    QLineEdit *leftAngleEdit;
    QLabel *label_5;
    QLabel *label_6;
    QComboBox *CommandcomboBox;
    QLabel *label_7;
    QPushButton *sendCommendButton;
    QWidget *radarPlaceholder;
    QLabel *label_8;
    QLabel *dist_labal;
    QLabel *label_9;
    QLabel *angle_lable;
    QLabel *label_10;
    QLabel *ldr_lable;
    QLabel *label_11;
    QLabel *ldr_lable_2;
    QLabel *label_12;
    QLabel *ldr_est_lable;
    QPushButton *clearButton;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1278, 775);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 10, 201, 191));
        QFont font;
        font.setBold(true);
        groupBox->setFont(font);
        commBox = new QComboBox(groupBox);
        commBox->setObjectName(QString::fromUtf8("commBox"));
        commBox->setGeometry(QRect(10, 40, 81, 21));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 20, 49, 16));
        connectButton = new QPushButton(groupBox);
        connectButton->setObjectName(QString::fromUtf8("connectButton"));
        connectButton->setGeometry(QRect(10, 70, 75, 24));
        refreshButton = new QPushButton(groupBox);
        refreshButton->setObjectName(QString::fromUtf8("refreshButton"));
        refreshButton->setGeometry(QRect(90, 70, 75, 24));
        groupBox_2 = new QGroupBox(centralwidget);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(210, 10, 251, 191));
        uploadButton = new QPushButton(groupBox_2);
        uploadButton->setObjectName(QString::fromUtf8("uploadButton"));
        uploadButton->setGeometry(QRect(20, 30, 75, 24));
        downloadButton = new QPushButton(groupBox_2);
        downloadButton->setObjectName(QString::fromUtf8("downloadButton"));
        downloadButton->setGeometry(QRect(20, 70, 75, 24));
        startScriptButton = new QPushButton(groupBox_2);
        startScriptButton->setObjectName(QString::fromUtf8("startScriptButton"));
        startScriptButton->setGeometry(QRect(20, 110, 75, 24));
        fileNameComboBox = new QComboBox(groupBox_2);
        fileNameComboBox->addItem(QString());
        fileNameComboBox->setObjectName(QString::fromUtf8("fileNameComboBox"));
        fileNameComboBox->setGeometry(QRect(130, 30, 101, 22));
        removeFileButton = new QPushButton(groupBox_2);
        removeFileButton->setObjectName(QString::fromUtf8("removeFileButton"));
        removeFileButton->setGeometry(QRect(20, 150, 75, 24));
        groupBox_3 = new QGroupBox(centralwidget);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setGeometry(QRect(460, 10, 531, 191));
        delayEdit = new QLineEdit(groupBox_3);
        delayEdit->setObjectName(QString::fromUtf8("delayEdit"));
        delayEdit->setGeometry(QRect(110, 20, 81, 22));
        label_2 = new QLabel(groupBox_3);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(70, 20, 49, 21));
        xInputEdit = new QLineEdit(groupBox_3);
        xInputEdit->setObjectName(QString::fromUtf8("xInputEdit"));
        xInputEdit->setGeometry(QRect(110, 60, 81, 22));
        label_3 = new QLabel(groupBox_3);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(90, 60, 51, 21));
        label_4 = new QLabel(groupBox_3);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(10, 100, 111, 21));
        degreeEdit = new QLineEdit(groupBox_3);
        degreeEdit->setObjectName(QString::fromUtf8("degreeEdit"));
        degreeEdit->setGeometry(QRect(130, 100, 61, 22));
        rightAngleEdit = new QLineEdit(groupBox_3);
        rightAngleEdit->setObjectName(QString::fromUtf8("rightAngleEdit"));
        rightAngleEdit->setGeometry(QRect(220, 140, 61, 22));
        leftAngleEdit = new QLineEdit(groupBox_3);
        leftAngleEdit->setObjectName(QString::fromUtf8("leftAngleEdit"));
        leftAngleEdit->setGeometry(QRect(80, 140, 61, 22));
        label_5 = new QLabel(groupBox_3);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(150, 140, 71, 21));
        label_6 = new QLabel(groupBox_3);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(18, 140, 51, 21));
        CommandcomboBox = new QComboBox(groupBox_3);
        CommandcomboBox->addItem(QString());
        CommandcomboBox->addItem(QString());
        CommandcomboBox->addItem(QString());
        CommandcomboBox->addItem(QString());
        CommandcomboBox->addItem(QString());
        CommandcomboBox->addItem(QString());
        CommandcomboBox->addItem(QString());
        CommandcomboBox->addItem(QString());
        CommandcomboBox->addItem(QString());
        CommandcomboBox->addItem(QString());
        CommandcomboBox->addItem(QString());
        CommandcomboBox->addItem(QString());
        CommandcomboBox->addItem(QString());
        CommandcomboBox->setObjectName(QString::fromUtf8("CommandcomboBox"));
        CommandcomboBox->setGeometry(QRect(390, 90, 121, 22));
        label_7 = new QLabel(groupBox_3);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(390, 60, 121, 21));
        sendCommendButton = new QPushButton(groupBox_3);
        sendCommendButton->setObjectName(QString::fromUtf8("sendCommendButton"));
        sendCommendButton->setGeometry(QRect(410, 120, 75, 24));
        radarPlaceholder = new QWidget(centralwidget);
        radarPlaceholder->setObjectName(QString::fromUtf8("radarPlaceholder"));
        radarPlaceholder->setGeometry(QRect(10, 210, 951, 511));
        label_8 = new QLabel(centralwidget);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(980, 300, 81, 16));
        QFont font1;
        font1.setPointSize(14);
        font1.setBold(true);
        label_8->setFont(font1);
        dist_labal = new QLabel(centralwidget);
        dist_labal->setObjectName(QString::fromUtf8("dist_labal"));
        dist_labal->setGeometry(QRect(1070, 300, 81, 16));
        dist_labal->setFont(font1);
        label_9 = new QLabel(centralwidget);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(980, 250, 81, 31));
        label_9->setFont(font1);
        angle_lable = new QLabel(centralwidget);
        angle_lable->setObjectName(QString::fromUtf8("angle_lable"));
        angle_lable->setGeometry(QRect(1070, 250, 81, 31));
        angle_lable->setFont(font1);
        label_10 = new QLabel(centralwidget);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(980, 330, 81, 31));
        label_10->setFont(font1);
        ldr_lable = new QLabel(centralwidget);
        ldr_lable->setObjectName(QString::fromUtf8("ldr_lable"));
        ldr_lable->setGeometry(QRect(1070, 330, 81, 31));
        ldr_lable->setFont(font1);
        label_11 = new QLabel(centralwidget);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setGeometry(QRect(980, 360, 81, 31));
        label_11->setFont(font1);
        ldr_lable_2 = new QLabel(centralwidget);
        ldr_lable_2->setObjectName(QString::fromUtf8("ldr_lable_2"));
        ldr_lable_2->setGeometry(QRect(1070, 360, 81, 31));
        ldr_lable_2->setFont(font1);
        label_12 = new QLabel(centralwidget);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setGeometry(QRect(980, 390, 81, 31));
        label_12->setFont(font1);
        ldr_est_lable = new QLabel(centralwidget);
        ldr_est_lable->setObjectName(QString::fromUtf8("ldr_est_lable"));
        ldr_est_lable->setGeometry(QRect(1070, 390, 81, 31));
        ldr_est_lable->setFont(font1);
        clearButton = new QPushButton(centralwidget);
        clearButton->setObjectName(QString::fromUtf8("clearButton"));
        clearButton->setGeometry(QRect(970, 210, 75, 24));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1278, 22));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "communication", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "serial ", nullptr));
        connectButton->setText(QCoreApplication::translate("MainWindow", "connect", nullptr));
        refreshButton->setText(QCoreApplication::translate("MainWindow", "refresh", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("MainWindow", "file system", nullptr));
        uploadButton->setText(QCoreApplication::translate("MainWindow", "upload", nullptr));
        downloadButton->setText(QCoreApplication::translate("MainWindow", "download", nullptr));
        startScriptButton->setText(QCoreApplication::translate("MainWindow", "start script", nullptr));
        fileNameComboBox->setItemText(0, QCoreApplication::translate("MainWindow", "files names", nullptr));

        removeFileButton->setText(QCoreApplication::translate("MainWindow", "remove", nullptr));
        groupBox_3->setTitle(QCoreApplication::translate("MainWindow", "Manual control", nullptr));
        delayEdit->setText(QCoreApplication::translate("MainWindow", "200", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "delay ", nullptr));
        xInputEdit->setText(QCoreApplication::translate("MainWindow", "20", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "X:", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "Degree of the motor", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "right angle", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "left angle", nullptr));
        CommandcomboBox->setItemText(0, QCoreApplication::translate("MainWindow", "inc_lcd", nullptr));
        CommandcomboBox->setItemText(1, QCoreApplication::translate("MainWindow", "dec_lcd", nullptr));
        CommandcomboBox->setItemText(2, QCoreApplication::translate("MainWindow", "rra_lcd", nullptr));
        CommandcomboBox->setItemText(3, QCoreApplication::translate("MainWindow", "set_delay", nullptr));
        CommandcomboBox->setItemText(4, QCoreApplication::translate("MainWindow", "clear_lcd", nullptr));
        CommandcomboBox->setItemText(5, QCoreApplication::translate("MainWindow", "servo_deg", nullptr));
        CommandcomboBox->setItemText(6, QCoreApplication::translate("MainWindow", "servo_scan", nullptr));
        CommandcomboBox->setItemText(7, QCoreApplication::translate("MainWindow", "LDR configutre", nullptr));
        CommandcomboBox->setItemText(8, QCoreApplication::translate("MainWindow", "get_LDR_calibration", nullptr));
        CommandcomboBox->setItemText(9, QCoreApplication::translate("MainWindow", "Full Scan UltaSonic", nullptr));
        CommandcomboBox->setItemText(10, QCoreApplication::translate("MainWindow", "Full Scan LDR", nullptr));
        CommandcomboBox->setItemText(11, QCoreApplication::translate("MainWindow", "Full Scan MIX", nullptr));
        CommandcomboBox->setItemText(12, QCoreApplication::translate("MainWindow", "sleep", nullptr));

        label_7->setText(QCoreApplication::translate("MainWindow", "Selecting a command", nullptr));
        sendCommendButton->setText(QCoreApplication::translate("MainWindow", "send", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "Distanst", nullptr));
        dist_labal->setText(QString());
        label_9->setText(QCoreApplication::translate("MainWindow", "Angle:", nullptr));
        angle_lable->setText(QString());
        label_10->setText(QCoreApplication::translate("MainWindow", "LDR1: ", nullptr));
        ldr_lable->setText(QString());
        label_11->setText(QCoreApplication::translate("MainWindow", "LDR2: ", nullptr));
        ldr_lable_2->setText(QString());
        label_12->setText(QCoreApplication::translate("MainWindow", "LDR Est: ", nullptr));
        ldr_est_lable->setText(QString());
        clearButton->setText(QCoreApplication::translate("MainWindow", "clear screen", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
