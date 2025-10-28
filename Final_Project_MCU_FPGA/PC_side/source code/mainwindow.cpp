#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),serialPort(new SerialManager)
{
    ui->setupUi(this);
    //on_refreshButton_clicked();
    QStringList list=serialPort->availablePorts();
    ui->commBox->addItems(list);
    LDR1samples.resize(10);
    LDR2samples.resize(10);


    connect(serialPort, &SerialManager::fullMessageReceived, this, &MainWindow::handleRadarMessage);
    connect(serialPort, &SerialManager::errorOccurred, this, &MainWindow::onError);

    radar = new RadarWidget(ui->radarPlaceholder);
    QVBoxLayout *layout = new QVBoxLayout(ui->radarPlaceholder);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(radar);

    //for simulation
    scanTimer = new QTimer(this);
    connect(scanTimer, &QTimer::timeout, this, &MainWindow::onSimulateScan);
    //scanTimer->start(200);  // 5Hz

    detector = LightSourceDetector();

}

// helper function for the protocol
static inline bool buildRadarQByteArray(uint8_t cmd, const QByteArray& payload, QByteArray& out)
{
    if (payload.size() > RADAR_MAX_PAYLOAD_LEN) return false;

    radar_message_t m{};
    if (radar_build_message(&m, cmd,
                            reinterpret_cast<const uint8_t*>(payload.constData()),
                            static_cast<uint8_t>(payload.size())) != 0) {
        return false;
    }

    // נשרשר לבאפר בייטים לפי הפונקציה מהפרוטוקול
    // נשתמש בבאפר זמני בגודל מקסימלי: 1(magic)+1(cmd)+1(len)+16(payload)+1(crc)+1(end) = 21
    uint8_t buf[1 + 1 + 1 + RADAR_MAX_PAYLOAD_LEN + 1 + 1] = {0};
    int n = radar_message_to_bytes(buf, &m);
    if (n < 0) return false;

    out = QByteArray(reinterpret_cast<const char*>(buf), n);
    return true;
}


void MainWindow::onSimulateScan() {
    radar->setCurrentAngle(currentSimAngle);

    currentSimAngle += 1;
    if (currentSimAngle > 180)
        currentSimAngle = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_refreshButton_clicked()
{
    ui->commBox->clear();
    QStringList list=serialPort->availablePorts();
    ui->commBox->addItems(list);
}


void MainWindow::on_connectButton_clicked()
{

    QString portName = ui->commBox->currentText();
    if (!serialPort->isOpen()) {
        if (serialPort->openPort(portName)) {
            ui->connectButton->setText("disconnect");
        }
    } else {
        serialPort->closePort();
        ui->connectButton->setText("connect");
    }


}

void MainWindow::onDataReceived(const QByteArray& bytes)
{
    qDebug() << "Received:" << bytes;
    radar_message_t msg{};
    for (unsigned char b : bytes)
    {
        int rc = radar_parse_char(&msg, b);
        if (rc == 1) {
            // הודעה שלמה: אפשר לפענח לפי command_id
            handleRadarMessage(msg);
        }
    }
    //parseMessage(data);


}

void MainWindow::sendCommandWithRetry(const QByteArray& msg, uint8_t cmdId)
{
    messageToResend = msg;
    ackReceived = false;
    commandWaitingAck = cmdId;

    serialPort->sendBytes(msg);

    if (!retryTimer) {
        retryTimer = new QTimer(this);
        connect(retryTimer, &QTimer::timeout, this, [this]() {
            if (!ackReceived) {
                qDebug() << "Resending message (waiting for ACK)...";
                serialPort->sendBytes(messageToResend);
            } else {
                retryTimer->stop();
            }
        });
    }

    retryTimer->start(10000); // כל שנייה
}

void MainWindow::handleRadarMessage(const radar_message_t& msg)
{
    switch (msg.command_id) {
    case RADAR_MSG_SAMPLES_VALUES: {
        if (msg.payload_len < 1) {
            qWarning() << "Invalid SAMPLES_VALUES payload";
            return;
        }

        int angle = msg.payload[0];
        int sampleCount = (msg.payload_len - 1) / 2;

        if ((msg.payload_len - 1) % 2 != 0) {
            qWarning() << "Malformed SAMPLES_VALUES – uneven sample bytes";
            return;
        }

        tempSamples.clear();

        for (int i = 0; i < sampleCount; ++i) {
            int index = 1 + i * 2;
            uint16_t distance = (msg.payload[index] << 8) | msg.payload[index + 1];
            tempSamples.append(distance);
        }

        uint16_t sample = findTheDist(tempSamples);
        qDebug() << "SAMPLES at angle:" << angle << "samples:" << tempSamples;
        qDebug() << "max =" << sample;

        ui->dist_labal->setText(QString::number(sample));
        ui->angle_lable->setText(QString::number(angle));
        radar->setCurrentAngle(angle);

        break;
    }

    case RADAR_MSG_LDR_DONE_SAMPLE_CALIB: {
        if (dlg && dlg->isVisible()) {
             uint16_t LDR_sample1 = (msg.payload[1] << 8) | msg.payload[2];
             uint16_t LDR_sample2 = (msg.payload[3] << 8) | msg.payload[4];
             qDebug()<<"LDR1 = "<<LDR_sample1;
             qDebug()<<"LDR2 = "<<LDR_sample2;
             int avgSample = (LDR_sample1+LDR_sample2 )/2;
            dlg->onNewSampleReceived(avgSample);  // או נתון מתאים, אם יש
        }
        break;
    }
    case RADAR_MSG_LDR_CALIB_RESULT:
    {
        uint16_t LDR_sample1 = (msg.payload[1] << 8) | msg.payload[2];
        uint16_t LDR_sample2 = (msg.payload[3] << 8) | msg.payload[4];
        uint8_t index =msg.payload[0];
        qDebug()<<"LDR1 = "<<LDR_sample1;
        qDebug()<<"LDR2 = "<<LDR_sample2;
        //int avgSample = (LDR_sample1+LDR_sample2 )/2;

        LDR1samples[index]=LDR_sample1;
        LDR2samples[index]=LDR_sample2;

        if(index==9)
        {
            qDebug()<<"printing result";
            for (int i = 0; i < LDR1samples.size(); ++i) {
                qDebug() << "Index" << i << ":LDR1 " << LDR1samples[i]<<"LDR2 "<<LDR2samples[i];
            }
        }
        break;
    }

    case RADAR_MSG_LDR_DONE_SAMPLE:
    {
        uint16_t LDR_sample1 = (msg.payload[1] << 8) | msg.payload[2];
        uint16_t LDR_sample2 = (msg.payload[3] << 8) | msg.payload[4];
        uint8_t index =msg.payload[0];
        qDebug()<<"LDR1 = "<<LDR_sample1;
        qDebug()<<"LDR2 = "<<LDR_sample2;
        int avgSample = (LDR_sample1+LDR_sample2 )/2;
        double estimatedDist =estimateDistanceFromSamples(LDR_sample1,LDR_sample2,LDR1samples,LDR2samples);
        ui->ldr_lable->setText(QString::number(LDR_sample1));
        ui->ldr_lable_2->setText(QString::number(LDR_sample2));
        ui->ldr_est_lable->setText(QString::number(estimatedDist));
    break;
    }

    case RADAR_MSG_SCAN_SMPLES_VALUES:
    {
        if (msg.payload_len < 1) {
            qWarning() << "Invalid SAMPLES_VALUES payload";
            return;
        }

        int angle = msg.payload[0];
        int sampleCount = (msg.payload_len - 1) / 2;

        if ((msg.payload_len - 1) % 2 != 0) {
            qWarning() << "Malformed SAMPLES_VALUES – uneven sample bytes";
            return;
        }

        tempSamples.clear();

        for (int i = 0; i < sampleCount; ++i) {
            int index = 1 + i * 2;
            uint16_t distance = (msg.payload[index] << 8) | msg.payload[index + 1];
            tempSamples.append(distance);
        }

        uint16_t sample = findTheDist(tempSamples);
        qDebug() << "SAMPLES at angle:" << angle << "samples:" << tempSamples;
        qDebug() << "max =" << sample;

        if (!scanData.contains(angle))
            scanData[angle] = PointInScan{};

        scanData[angle].degree = angle;
        scanData[angle].distance_cm = sample;
        ui->dist_labal->setText(QString::number(sample));
        ui->angle_lable->setText(QString::number(angle));
        radar->setCurrentAngle(angle);

        break;
    }
    case RADAR_MSG_SCAN_LDR_VALUES: {
        if (msg.payload_len >= 3) {
            uint8_t degree = msg.payload[0];
            uint16_t ldr1 = (msg.payload[1] << 8) | msg.payload[2];
            uint16_t ldr2 = (msg.payload[3] << 8) | msg.payload[4];

            //qDebug()<<"LDR1 ="<<ldr1;
            //qDebug()<<"LDR2 ="<<ldr2;

            if (!scanData.contains(degree))
                scanData[degree] = PointInScan{};

            scanData[degree].degree = degree;
            scanData[degree].ldr1Sample = ldr1;
            scanData[degree].ldr2Sample = ldr2;
            ui->angle_lable->setText(QString::number(degree));
            ui->ldr_lable->setText(QString::number(ldr1));
            ui->ldr_lable_2->setText(QString::number(ldr2));
            ui->ldr_est_lable->setText("-");
            radar->setCurrentAngle(degree);


        }
        break;
    }

    case RADAR_MSG_DONE_SCANING:
    {
        uint8_t action= msg.payload[0];
        switch (action) {
        case DONE_FULL_SCAN_ULTRASONIC:
        {
            UltraSonicAlgoCalculation();
            break;
        }
        case DONE_FULL_SCAN_LDR :
        {

            LdrAlgoCalculation();
            break;
        }
        case DONE_FULL_SCAN_MIX :
        {
            qDebug()<<"start algorighm for MIX";
            UltraSonicAlgoCalculation();
            LdrAlgoCalculation();
            break;
        }
        default:
            qDebug()<<"error!!!!";
            break;
        }
        qDebug()<<"after algo";
        scanData.clear();
        break;
    }

    case RADAR_MSG_ACK:
    {
        if (msg.payload_len >= 1) {
            uint8_t ackedCommand = msg.payload[0];
            if (ackedCommand == commandWaitingAck) {
                ackReceived = true;
                qDebug() << "ACK received for command" << ackedCommand;

            }

            if (ackedCommand==RADAR_CMD_START_READ_FILE)
            {
                qDebug() <<"start chancks";
                // start sending chancks
                if (msg.payload[1]==0x06) // ack is ok
                {

                    if(theFileInProsses->InProsses)
                    {
                        QByteArray data= txData;
                        theFileInProsses->i=0;
                        theFileInProsses->seq=0;
                        theFileInProsses->n = (21 < theFileInProsses->sizeOfData) ? 21 : theFileInProsses->sizeOfData;
                        QByteArray pay ;
                        pay.append(static_cast<char>(theFileInProsses->seq & 0x0F));  // ראשון: 4 ביטים מה-seq
                        pay.append(static_cast<char>(theFileInProsses->n));          // שני: אורך הצ'אנק
                        pay.append(data.mid(theFileInProsses->i, theFileInProsses->n));
                        //theFileInProsses->i+=n;
                        //theFileInProsses->seq+=1;
                        QByteArray frame;
                        if (!buildRadarQByteArray(RADAR_CMD_CHUNK_READ_FILE, pay, frame))
                        {
                            QMessageBox::warning(this, "Error", "Failed to build message (payload too long?)");
                            return;
                        }

                        sendCommandWithRetry(frame, RADAR_CMD_CHUNK_READ_FILE);
                        qDebug()<<"sending first chank";
                    }


                }
            }
            else if(ackedCommand==RADAR_CMD_CHUNK_READ_FILE)
            {
                if (msg.payload[1]==0x06)
                {
                    qDebug()<<"Ack of chanck is ok";
                    theFileInProsses->i+=theFileInProsses->n;
                    if(theFileInProsses->i < theFileInProsses->sizeOfData)
                    {
                        theFileInProsses->seq+=1;
                        QByteArray data= txData;
                        theFileInProsses->n = (21 < theFileInProsses->sizeOfData-theFileInProsses->i) ? 21 : theFileInProsses->sizeOfData-theFileInProsses->i;

                        QByteArray pay ;
                        pay.append(static_cast<char>(theFileInProsses->seq & 0x0F));  // ראשון: 4 ביטים מה-seq
                        pay.append(static_cast<char>(theFileInProsses->n));          // שני: אורך הצ'אנק
                        pay.append(data.mid(theFileInProsses->i, theFileInProsses->n));

                        QByteArray frame;
                        if (!buildRadarQByteArray(RADAR_CMD_CHUNK_READ_FILE, pay, frame))
                        {
                            QMessageBox::warning(this, "Error", "Failed to build message (payload too long?)");
                            return;
                        }

                        sendCommandWithRetry(frame, RADAR_CMD_CHUNK_READ_FILE);

                        qDebug()<<"sending chank seq "<< (theFileInProsses->seq) ;
                    }

                    else
                    {
                        QByteArray payload ;
                        payload.append(static_cast<char>(0x00));
                        QByteArray frame;
                        if (!buildRadarQByteArray(RADAR_CMD_END_READ_FILE, payload, frame))
                        {
                            QMessageBox::warning(this, "Error", "Failed to build message (payload too long?)");
                            return;
                        }

                        sendCommandWithRetry(frame, RADAR_CMD_END_READ_FILE);
                        qDebug()<<" End sending chank";
                        theFileInProsses->InProsses=false;
                        theFileInProsses->isDownload=true;
                    }
                }
                else //(msg.payload[1]==0x0B) // bad Seq
                {
                    ackReceived=false;
                    qDebug()<<"Error with the chanck";
                }

            }
        }
        break;

    }

    case RADAR_MSG_ERROR: {
        qDebug() << "Error received from MCU";
        break;
    }

    default:
        qWarning() << "Unknown message type:" << msg.command_id;
    }
}




void MainWindow::parseMessage(const QByteArray &msg) {
    if (msg.size() < 3 || msg[0] != '@' || msg.back() != '#') {
        qWarning() << "Invalid message format";
        return;
    }

    uint8_t type = (uint8_t)msg[1];
    const uint8_t *data = reinterpret_cast<const uint8_t *>(msg.constData());

    switch (type) {
    case RADAR_MSG_SAMPLES_VALUES: {
        //
        qDebug() << "Type S received:";
            // צורת ההודעה: @ S angle [samples...] #
            int angle = data[2];  // Byte 2

            int payloadStart = 3;
            int payloadLength = msg.size() - 4; // בלי @, S, angle, #

            if (payloadLength % 2 != 0) {
                qWarning() << "Malformed S message – uneven sample bytes";
                return;
            }

            int sampleCount = payloadLength / 2;

            tempSamples.clear();
            //scanBaseAngle = angle;

            for (int i = 0; i < sampleCount; ++i) {
                int index = payloadStart + i * 2;
                uint16_t distance = (data[index] << 8) | data[index + 1];
                tempSamples.append(distance);
            }
            //radar->setAngleAndDistance(angle,tempSamples[0]);
            uint16_t sample= findTheDist(tempSamples); //*std::max_element(tempSamples.begin(), tempSamples.end());
            qDebug()<< tempSamples;
            qDebug()<<"max = "<< sample;
            ui->dist_labal->setText(QString::number(sample));
            ui->angle_lable->setText(QString::number(angle));
            radar->setCurrentAngle(angle);

        break;
    }
    case RADAR_MSG_LDR_DONE_SAMPLE: {
        // the MCU done taking one sample from the LDR
            if (dlg->isVisible()) {
            dlg->onNewSampleReceived(0);
            }

            break;
        }
    case RADAR_MSG_ACK:{
            break;
    }


    case RADAR_MSG_ERROR: {
        // שגיאה כלשהי
        qDebug() << "Error from MCU:" ;
        break;
    }

    default:
        qWarning() << "Unknown message type:" << type;
    }
}

void MainWindow::onError(QString error) {
    QMessageBox::warning(this, "Serial Error", error);
}


uint16_t MainWindow::findTheDist(QVector<uint16_t> &samples)
{
    if (samples.isEmpty())
        return 0;

    QMap<uint16_t, int> frequencyMap;

    // סופרים כל ערך
    for (uint16_t val : samples) {
        frequencyMap[val]++;
    }

    // מחפשים את הערך עם ההופעה הגבוהה ביותר
    int maxCount = 0;
    uint16_t mostFrequent = 0;

    for (auto it = frequencyMap.constBegin(); it != frequencyMap.constEnd(); ++it) {
        if (it.value() > maxCount) {
            maxCount = it.value();
            mostFrequent = it.key();
        }
    }

    return mostFrequent;
}


void MainWindow::on_uploadButton_clicked()
{
    if (loadedFiles.size()==10)
    {
        QMessageBox::information(this, "Info", "10 Files already loaded.");
        return;

    }
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Select file to upload",
        QDir::homePath(), // אפשר לשנות לנתיב אחר
        "All Files (*.*);;Text Files (*.txt);;Script Files (*.script)"
        );

    if (filePath.isEmpty()) {
        qDebug() << "No file selected.";
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Error", "Failed to open the selected file.");
        return;
    }


    QString rawText = file.readAll();  // קריאה כטקסט
    file.close();
    qDebug()<<rawText;
    // encoded the files
    QByteArray encoded = rawText.toLatin1();//encodeScriptText(rawText,50);
    QString fileName = QFileInfo(filePath).fileName();


    bool IsScript=fileName.contains("script");

    if (IsScript)
    {
        encoded=encodeScriptText(rawText,50);
    }


    for (const FileEntry &entry : loadedFiles) {
        if (entry.fileName == fileName) {
            QMessageBox::information(this, "Info", "File already loaded.");
            return;
        }
    }

    FileEntry tempFile={};
    tempFile.fileName=fileName;
    tempFile.fileData=encoded;
    tempFile.isDownload=false;
    tempFile.isScript=IsScript;
    tempFile.id= id_files;
    id_files++;
    //
    loadedFiles.append(tempFile);

    // עדכון ComboBox
    ui->fileNameComboBox->addItem(fileName);

    qDebug() << "Loaded" << fileName << "size:" << encoded.size();



}

double MainWindow::computeWidthInCM(double rho, double deltaPhiDeg)
{
    double deltaPhiRad = qDegreesToRadians(deltaPhiDeg);
    return 1.1 * rho * qSin(deltaPhiRad / 2.0);
}
DetectedObject MainWindow::analyzeObject(const QList<PointInScan>& points)
{
    DetectedObject obj;

    uint8_t startDeg = points.first().degree;
    uint8_t endDeg   = points.last().degree;
    double deltaPhi  = endDeg - startDeg;

    // degree from the center
    obj.phi = startDeg + deltaPhi / 2.0;

    // avg distance
    double sum = 0.0;
    for (const auto& p : points)
        sum += p.distance_cm;
    obj.rho = sum / points.size();

    // width
    obj.l = computeWidthInCM(obj.rho, deltaPhi);

    return obj;
}

QVector<DetectedObject> MainWindow::extractObjectsFromScan(const QMap<uint8_t, PointInScan>& scanData)
{
    QVector<DetectedObject> results;
    QList<PointInScan> currentObject;

    for (auto it = scanData.begin(); it != scanData.end(); ++it) {
        const PointInScan& p = it.value();

        bool isValid = p.distance_cm >= MIN_OBJECT_DISTANCE_CM && p.distance_cm <= MAX_OBJECT_DISTANCE_CM;

        if (isValid) {
            if (!currentObject.isEmpty()) {
                const PointInScan& last = currentObject.last();
                if (qAbs(p.distance_cm - last.distance_cm) < MAX_DISTANCE_JUMP_CM) {
                    currentObject.append(p);
                } else {
                    if (currentObject.size() >= MIN_OBJECT_WIDTH_DEG)
                        results.append(analyzeObject(currentObject));
                    currentObject.clear();
                    currentObject.append(p);
                }
            } else {
                currentObject.append(p);
            }
        } else {
            if (!currentObject.isEmpty()) {
                if (currentObject.size() >= 2)
                    results.append(analyzeObject(currentObject));
                currentObject.clear();
            }
        }
    }

    if (!currentObject.isEmpty() && currentObject.size() >= 2)
        results.append(analyzeObject(currentObject));

    return results;
}



void MainWindow::UltraSonicAlgoCalculation()
{
    qDebug()<<"start algorighm for ULTASOIC";
    QVector<DetectedObject> objects = extractObjectsFromScan(scanData);

    for (const auto& obj : objects) {
        qDebug() << "Object at degree=" << obj.phi << "°, distance=" << obj.rho << "cm, width=" << obj.l << "cm";
        radar->setAngleAndDistance(obj.phi,obj.rho,obj.l);
    }



}

void MainWindow::LdrAlgoCalculation()
{
    qDebug()<<"start algorighm for LDR";

    qDebug()<<"All The Scan Data:";
    for (const PointInScan& point : scanData)
    {
        qDebug() << "angle:" << point.degree
                 << "LDR1 :" << point.ldr1Sample
                 << "LDR2 :" << point.ldr2Sample;

    }
    qDebug()<<"Done printing Scan Data:";
    QVector<DetectedSource> sources = detector.detectLocalMinima(scanData,20);

    for (const DetectedSource& s : sources)
    {
        double dist= estimateDistanceFromSamples(s.score,s.score,LDR1samples,LDR2samples);
        qDebug() << "Local minimum @ angle:" << s.angleDeg <<"value:"<<s.score<<"dist: "<<dist;
        radar->addLightSource(s.angleDeg,dist);
    }
}

double MainWindow::interpolateSingle(uint16_t value, const QVector<uint16_t>& calibration) {
    const int step = 5; // 5 cm per calibration point
    const int N = calibration.size();

    for (int i = 0; i < N - 1; ++i) {
        uint16_t y1 = calibration[i];
        uint16_t y2 = calibration[i + 1];

        if ((y1 >= value && y2 <= value) || (y1 <= value && y2 >= value)) {
            double x1 = (i + 0) * step + step; // 5*(i+1)
            double x2 = (i + 1) * step + step;

            double ratio = (double)(value - y1) / (y2 - y1);
            return x1 + ratio * (x2 - x1);
        }
    }

    // Out-of-range handling
    if (value >= calibration.first()) return 5.0;
    if (value <= calibration.last()) return 50.0;

    return -1.0;
}

// Final function: estimates distance based on two sensors and their calibrations
double MainWindow::estimateDistanceFromSamples(uint16_t sample1,
                                   uint16_t sample2,
                                   const QVector<uint16_t>& calibrationLDR1,
                                   const QVector<uint16_t>& calibrationLDR2)
{
    double d1 = interpolateSingle(sample1, calibrationLDR1);
    double d2 = interpolateSingle(sample2, calibrationLDR2);

    if (d1 < 0 && d2 < 0) return -1.0;           // both failed
    if (d1 < 0) return d2;                       // only LDR2 is valid
    if (d2 < 0) return d1;                       // only LDR1 is valid
    return (d1 + d2) / 2.0;                      // average of both
}

QByteArray MainWindow::encodeScriptText(const QString &scriptText, uint8_t currentDelay = 50)
{
    QByteArray result;
    QStringList lines = scriptText.split('\n', Qt::SkipEmptyParts);

    for (const QString &line : lines) {
        QString trimmed = line.trimmed();
        if (trimmed.isEmpty()) continue;

        QStringList tokens = trimmed.split(QRegularExpression("\\s+|,"), Qt::SkipEmptyParts);
        QString cmd = tokens[0];
        QByteArray encoded;

        if (cmd == "inc_lcd" && tokens.size() >= 2) {
            encoded.append(RADAR_CMD_INC_LCD);
            encoded.append(tokens[1].toUShort());       // x
            encoded.append(currentDelay);               // delay

        } else if (cmd == "dec_lcd" && tokens.size() >= 2) {
            encoded.append(RADAR_CMD_DEC_LCD);
            encoded.append(tokens[1].toUShort());
            encoded.append(currentDelay);

        } else if (cmd == "rra_lcd" && tokens.size() >= 2) {
            encoded.append(RADAR_CMD_RRA_LCD);
            encoded.append(tokens[1].toLatin1().at(0)); // ASCII char
            encoded.append(currentDelay);

        } else if (cmd == "set_delay" && tokens.size() >= 2) {
            currentDelay = tokens[1].toUShort();
            encoded.append(RADAR_CMD_SET_DELAY);
            encoded.append(currentDelay);

        } else if (cmd == "clear_lcd") {
            encoded.append(RADAR_CMD_CLEAR_LCD);

        } else if (cmd == "servo_deg" && tokens.size() >= 2) {
            encoded.append(RADAR_CMD_SERVO_DEGREE);
            encoded.append(tokens[1].toUShort());

        } else if (cmd == "servo_scan" && tokens.size() >= 3) {
            encoded.append(RADAR_CMD_SERVO_SCAN);
            encoded.append(tokens[1].toUShort());
            encoded.append(tokens[2].toUShort());

        } else if (cmd == "sleep") {
            encoded.append(RADAR_CMD_SLEEP);
        } else {
            qWarning() << "Unknown or invalid command:" << line;
            continue;
        }

        // הוספת קידוד לפלט + פסיק
        result.append(encoded);
        result.append(',');  // מפריד בין הפקודות
    }

    // הסרת פסיק אחרון אם צריך
    if (!result.isEmpty() && result.endsWith(','))
        result.chop(1);

    return result;
}

void MainWindow::on_removeFileButton_clicked()
{
    QString fileName= ui->fileNameComboBox->currentText();
    int fileIndex=ui->fileNameComboBox->currentIndex();

    for (int i=0; i<loadedFiles.size(); i++)
    {


        if (loadedFiles[i].fileName == fileName) {

            loadedFiles.erase(loadedFiles.begin() + i);
            break;
        }
    }

    ui->fileNameComboBox->removeItem(fileIndex);

}

FileEntry* MainWindow::getFile(QString &fileName)
{
    for (FileEntry &entry : loadedFiles) {
        if (entry.fileName == fileName)
            return &entry;
    }
    return nullptr;
}

uint16_t crc16_ibm(const QByteArray& data, uint16_t seed = 0xFFFF) {
    uint16_t c = seed;
    for (uint8_t b : data) {
        c ^= b;
        for (int i = 0; i < 8; i++) {
            if (c & 1) {
                c = (c >> 1) ^ 0xA001;
            } else {
                c >>= 1;
            }
        }
    }
    return c & 0xFFFF;
}

static QByteArray buildFixedNameHeader16(const QString& name)
{
    QByteArray h = name.left(16).toLatin1();     // חותך ל-16 תווים מקסימום
    if (h.size() < 16)
        h.append(QByteArray(16 - h.size(), ' '));   // ריפוד ב-0x00 (אפשר ' ' אם אתה מעדיף)
    return h;   // תמיד 16 בייט בדיוק
}

static void dbgDumpHex(const char* title, const QByteArray& ba, int maxBytes = -1)
{
    QByteArray slice = (maxBytes > 0 && ba.size() > maxBytes) ? ba.left(maxBytes) : ba;
    qDebug().noquote() << title << "(" << ba.size() << "B) :" << slice.toHex(' ');
    if (maxBytes > 0 && ba.size() > maxBytes)
        qDebug() << "… (trimmed, showing first" << maxBytes << "bytes)";
}

void MainWindow::on_downloadButton_clicked()
{
    QString fileName=ui->fileNameComboBox->currentText();
    QByteArray dataName;
    FileEntry* file= getFile(fileName);
    qDebug()<<"start download file "<<file->fileName<<" to the MCU";

    //QString name = theFileInProsses->fileName;  // כבר קיים
    QByteArray nameBytes = fileName.toLatin1();//.rightJustified(10, ' ', true);

    //QByteArray data = nameBytes+file->fileData;

    // ← כותרת 16B קבועה לשם הקובץ
    const QByteArray header16 = buildFixedNameHeader16(fileName);


    txData = header16 + file->fileData;

    dbgDumpHex("SCRIPT txData (16B name + payload)", txData, 128); // מציג עד 128B

    //file->fileData=data;

    uint16_t sizeOfData = txData.size();
    uint16_t crc= crc16_ibm(txData);
    uint8_t  type_of_file;
    if(file->isScript)
    {
        type_of_file=0x02;
    }
    else{
        type_of_file = 0x01; // text
    }
    uint8_t  name_id =file->id;
    uint8_t  flags = 0x00;
    uint8_t   meta = ((flags & 0x0F) << 4) | (name_id & 0x0F);

    file->InProsses=true;

    theFileInProsses=file;

    theFileInProsses->sizeOfData=sizeOfData;
    theFileInProsses->action=OP_UPLOAD;
    theFileInProsses->crc=crc;
    theFileInProsses->id=name_id;
    theFileInProsses->meta=meta;


    //start
    QByteArray start_payload;
    start_payload.append(static_cast<char>(OP_UPLOAD));
    start_payload.append(static_cast<char>(0x00));
    start_payload.append(static_cast<char>((sizeOfData >> 8) & 0xFF));
    start_payload.append(static_cast<char>(sizeOfData & 0xFF));
    start_payload.append(static_cast<char>(crc & 0xFF));
    start_payload.append(static_cast<char>((crc >> 8) & 0xFF));
    start_payload.append(static_cast<char>(type_of_file));
    start_payload.append(meta);
    //fileNameToByte+=start_payload;
    //start_payload+=fileNameToByte;
    QByteArray frame;
    if (!buildRadarQByteArray(RADAR_CMD_START_READ_FILE, start_payload, frame))
    {
        QMessageBox::warning(this, "Error", "Failed to build message (payload too long?)");
        return;
    }

    sendCommandWithRetry(frame, RADAR_CMD_START_READ_FILE);
}




void MainWindow::on_startScriptButton_clicked()
{
    QByteArray payload;
    uint8_t command =RADAR_CMD_START_SCRIPT ;
    QString fileName=ui->fileNameComboBox->currentText();
    QByteArray dataName;
    FileEntry* file= getFile(fileName);
    qDebug()<<"start script of file "<<file->fileName<<" to the MCU";

    payload.append(file->id & 0x0F);

    // שליחה בפועל (הנתיב האחיד)
    QByteArray frame;
    if (!buildRadarQByteArray(command, payload, frame)) {
        QMessageBox::warning(this, "Error", "Failed to build message (payload too long?)");
        return;
    }

    sendCommandWithRetry(frame, command);
    qDebug() << "Sent cmd=0x" << QString::number(command,16) << frame.toHex(' ');

}


void MainWindow::on_sendCommendButton_clicked()
{
    if (!serialPort || !serialPort->isOpen()) {
        QMessageBox::warning(this, "Error", "Serial port not open.");
        return;
    }

    const QString cmd = ui->CommandcomboBox->currentText();
    QByteArray payload;
    uint8_t command = 0;

    auto readU8 = [](const QString& s, bool& ok)->uint8_t {
        uint32_t v = s.toUInt(&ok);
        return ok ? static_cast<uint8_t>(v) : 0;
    };

    if (cmd == "inc_lcd") {
        bool ok1=false, ok2=false;
        uint8_t x = ui->xInputEdit->text().toInt(&ok1);
        uint8_t d = ui->delayEdit->text().toInt(&ok2);
        if (!ok1 || !ok2) { QMessageBox::warning(this,"Error","Invalid X or delay"); return; }
        command = RADAR_CMD_INC_LCD;
        payload.append(x);
        payload.append(d);

    } else if (cmd == "dec_lcd") {
        bool ok1=false, ok2=false;
        uint8_t x = ui->xInputEdit->text().toInt(&ok1);
        uint8_t d = ui->delayEdit->text().toInt(&ok2);
        if (!ok1 || !ok2) { QMessageBox::warning(this,"Error","Invalid X or delay"); return; }
        command = RADAR_CMD_DEC_LCD;
        payload.append(x);
        payload.append(d);

    } else if (cmd == "rra_lcd") {
        if (ui->xInputEdit->text().length() != 1) { QMessageBox::warning(this,"Error","X must be a single char"); return; }
        bool ok2=false;
        char x = ui->xInputEdit->text().at(0).toLatin1();
        uint8_t d = ui->delayEdit->text().toInt(&ok2);
        if (!ok2) { QMessageBox::warning(this,"Error","Invalid delay"); return; }
        command = RADAR_CMD_RRA_LCD;
        payload.append(x);
        payload.append(char(d));

    } else if (cmd == "set_delay") {
        bool ok=false;
        uint8_t d = ui->delayEdit->text().toInt(&ok);
        if (!ok) { QMessageBox::warning(this,"Error","Invalid delay"); return; }
        command = RADAR_CMD_SET_DELAY;
        payload.append(d);

    } else if (cmd == "clear_lcd") {
        command = RADAR_CMD_CLEAR_LCD;

    } else if (cmd == "servo_deg") {
        bool ok=false;
        uint8_t p = ui->degreeEdit->text().toInt(&ok);
        if (!ok || p > 180) { QMessageBox::warning(this,"Error","Invalid degree"); return; }
        command = RADAR_CMD_SERVO_DEGREE;
        payload.append(p);

    } else if (cmd == "servo_scan") {
        bool ok1=false, ok2=false;
        uint8_t l = ui->leftAngleEdit->text().toInt(&ok1);
        uint8_t r = ui->rightAngleEdit->text().toInt(&ok2);
        if (!ok1 || !ok2 || l > 180 || r > 180) { QMessageBox::warning(this,"Error","Invalid angles"); return; }
        command = RADAR_CMD_SERVO_SCAN;
        payload.append(l);
        payload.append(r);

    } else if (cmd == "sleep") {
        command = RADAR_CMD_SLEEP;

    }
    else if (cmd == "get_LDR_calibration")
    {
        command = RADAR_CMD_LDR_GET_CALIB;

    }
    else if (cmd == "Full Scan MIX")
    {
        command = RADAR_CMD_FULL_Scan_MIX;

    }
    else if (cmd== "Full Scan LDR")
    {
        command = RADAR_CMD_FULL_Scan_LDR;
    }
    else if (cmd== "Full Scan UltaSonic")
    {
        command = RADAR_CMD_FULL_Scan_ULTRASONIC;
    }

    else if (cmd == "LDR configutre") {
        // במקום לבנות ידנית בתוך lambda, נשתמש תמיד באותו בילדר
        command = RADAR_CMD_LDR_CONFIGURE;

        dlg = new CalibrationDialog(this);

        connect(dlg, &CalibrationDialog::requestLDRSample, this,
                [this](int step){
                    QByteArray pl;
                    //pl.append('R');                 // בקשת Sample לקריאה
                    pl.append(step);
                    QByteArray frame;
                    if (!buildRadarQByteArray(RADAR_CMD_LDR_CONFIGURE, pl, frame)) {
                        qWarning() << "Failed to build LDR_CONFIGURE frame";
                        return;
                    }
                    serialPort->sendBytes(frame);
                    qDebug() << "Sent LDR_CONFIGURE step=" << step << frame.toHex(' ');
                });

        dlg->exec();
        return; // כבר שיגרנו מתוך הדיאלוג, לא לשגר כאן שוב

    } else {
        QMessageBox::warning(this, "Error", "Unknown command.");
        return;
    }

    // שליחה בפועל (הנתיב האחיד)
    QByteArray frame;
    if (!buildRadarQByteArray(command, payload, frame)) {
        QMessageBox::warning(this, "Error", "Failed to build message (payload too long?)");
        return;
    }

    sendCommandWithRetry(frame, command);
    qDebug() << "Sent cmd=0x" << QString::number(command,16) << frame.toHex(' ');
}


void MainWindow::on_clearButton_clicked()
{
    radar->clearPoints();
}

