#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "serialmanager.h"
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QVector>
#include <QtMath>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include "radarwidget.h"
#include <QVBoxLayout>
#include <algorithm>
#include <QMap>
#include "LightSourceDetector.h"
extern "C" {
#include "radar_protocol.h"
}
#include "calibrationdialog.h"
#include <cstdint>
#include "qnamespace.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/*struct PointInScan {
    uint8_t distance_cm;
    uint16_t ldr1Sample;
    uint16_t ldr2Sample;
    uint16_t  degree;
};*/

struct FileEntry {
    QString fileName;
    QByteArray fileData;
    bool isScript;
    bool isDownload;
    bool InProsses;
    uint8_t action;
    uint16_t sizeOfData;
    uint16_t crc;
    uint8_t type;
    uint8_t meta;
    uint8_t id;
    uint16_t i;
    uint8_t seq;
    uint8_t n;

};


struct DetectedObject {
    double rho;  // avg distance
    double phi;  // mid degeree  (°)
    double l;    // width cm
};

const int MAX_OBJECT_DISTANCE_CM = 60;
const int MIN_OBJECT_DISTANCE_CM = 5;
const int MAX_DISTANCE_JUMP_CM = 25;     // jump between two sample that seperate objects
const int MIN_OBJECT_WIDTH_DEG = 2;      // the min degeree that its can be object

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void parseMessage(const QByteArray &msg);


    ~MainWindow();

private slots:
    void handleRadarMessage(const radar_message_t& msg);
    void on_refreshButton_clicked();
    void on_connectButton_clicked();
    void onDataReceived(const QByteArray& bytes);
    void onError(QString error);

    void on_uploadButton_clicked();


    void onSimulateScan();


    void on_removeFileButton_clicked();

    void on_downloadButton_clicked();

    void on_startScriptButton_clicked();

    void on_sendCommendButton_clicked();
    void sendCommandWithRetry(const QByteArray& msg, uint8_t cmdId);

    void on_clearButton_clicked();

private:

    FileEntry* getFile(QString &fileName);
    uint16_t findTheDist(QVector<uint16_t>& samples);
    QByteArray encodeScriptText(const QString &scriptText, uint8_t currentDelay );
    double computeWidthInCM(double rho, double deltaPhiDeg);
    DetectedObject analyzeObject(const QList<PointInScan>& points);
    QVector<DetectedObject> extractObjectsFromScan(const QMap<uint8_t, PointInScan>& scanData);

    double estimateDistanceFromSamples(uint16_t sample1,
                                       uint16_t sample2,
                                       const QVector<uint16_t>& calibrationLDR1,
                                       const QVector<uint16_t>& calibrationLDR2);
    double interpolateSingle(uint16_t value, const QVector<uint16_t>& calibration);

    void UltraSonicAlgoCalculation();
    void LdrAlgoCalculation();

    QMap<uint8_t, PointInScan> scanData;

    Ui::MainWindow *ui;
    SerialManager* serialPort;
    QVector<FileEntry> loadedFiles;
    RadarWidget *radar;
    QVector<uint16_t> scanSamples;
    QVector<uint8_t> scanSamplesAngle;
    QVector<uint16_t> tempSamples;
    QVector<uint16_t> LDR1samples;
    QVector<uint16_t> LDR2samples;

    FileEntry* theFileInProsses;

    //for simulation
    QTimer *scanTimer;
    int currentSimAngle = 0;
    CalibrationDialog *dlg;

    QTimer* retryTimer = nullptr;
    QByteArray messageToResend;
    bool ackReceived = false;
    uint8_t commandWaitingAck = 0x00;

    uint8_t id_files=0;

    QByteArray txData;


    LightSourceDetector detector;
};
#endif // MAINWINDOW_H
