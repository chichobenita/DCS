#pragma once

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsLineItem>
#include <QVector>
#include <QMap>

class RadarWidget : public QGraphicsView {
    Q_OBJECT

public:
    explicit RadarWidget(QWidget *parent = nullptr);
    void setAngleAndDistance(int angleDeg, uint16_t distanceCm,double widthCm);  // עדכון נקודה
    void setCurrentAngle(int angleDeg);                          // עדכון קו סריקה
    void clearPoints();
    void addLightSource(int angleDeg, double distanceCm);
protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void drawRadarGrid();
    void plotPoint(int angleDeg, double distanceCm,double widthCm);
    void updateSweepLine(int angleDeg);

    QGraphicsScene *scene;
    QList<QGraphicsItem*> plottedItems;
    QGraphicsLineItem *sweepLine;
    int radarRadiusPx;
    double cmToPx; // יחס ס"מ לפיקסלים
};
