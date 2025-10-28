#include "radarwidget.h"
#include <QtMath>
#include <QResizeEvent>
#include <QGraphicsEllipseItem>
#include <QGraphicsPathItem>
#include <QPainterPath>

RadarWidget::RadarWidget(QWidget *parent) : QGraphicsView(parent),
    scene(new QGraphicsScene(this)),
    sweepLine(nullptr),
    radarRadiusPx(120*1.5),
    cmToPx(1.5) // כל ס"מ = 0.5 פיקסל
{
    setScene(scene);
    setRenderHint(QPainter::Antialiasing);
    scene->setBackgroundBrush(Qt::black);
    drawRadarGrid();
}

void RadarWidget::resizeEvent(QResizeEvent *event) {
    QGraphicsView::resizeEvent(event);
    fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}

void RadarWidget::drawRadarGrid() {
    scene->clear();

    const int radarRangeCm = 120;
    const int stepCm = 20;
    const int steps = radarRangeCm / stepCm;

    QFont font("Arial", 8);
    font.setBold(true);

    for (int i = 1; i <= steps; ++i) {
        int r = static_cast<int>((stepCm * i) * cmToPx);  // המרה לסקייל פיקסלים
        QPainterPath arc;
        QRectF rect(-r, -r, 2*r, 2*r);
        arc.arcMoveTo(rect, 0);
        arc.arcTo(rect, 0, 180);
        scene->addPath(arc, QPen(Qt::green));

        //  תווית כל 50 ס"מ
        QString label = QString::number(i * stepCm);

        QGraphicsTextItem *textItem = scene->addText(label, font);
        textItem->setDefaultTextColor(Qt::green);
        textItem->setPos(-r -10, 1);  // ממוקם שמאלה מהקשת
    }

    // קווי זווית
    for (int angle = 0; angle <= 180; angle += 30) {
        double rad = qDegreesToRadians(static_cast<double>(angle));
        double x = radarRadiusPx * qCos(rad);
        double y = -radarRadiusPx * qSin(rad);
        scene->addLine(0, 0, x, y, QPen(Qt::green));

        // תווית זווית
        double tx = (radarRadiusPx + 15) * qCos(rad);
        double ty = -(radarRadiusPx + 15) * qSin(rad);
        QGraphicsTextItem *label = scene->addText(QString::number(angle) + "°", font);
        label->setDefaultTextColor(Qt::green);
        label->setPos(tx - 10, ty - 10);
    }

    scene->setSceneRect(
        -radarRadiusPx - 40,
        -radarRadiusPx - 40,
        2 * (radarRadiusPx + 40),
        radarRadiusPx + 60
        );
}

void RadarWidget::addLightSource(int angleDeg, double distanceCm) {
    double r = distanceCm * cmToPx;
    double rad = qDegreesToRadians(static_cast<double>(angleDeg));
    double x = r * qCos(rad);
    double y = -r * qSin(rad);

    QPen pen(Qt::blue);
    pen.setWidth(2);
    QGraphicsEllipseItem* point = scene->addEllipse(x - 2, y - 2, 4, 4, pen, QBrush(Qt::blue));
    plottedItems.append(point);  // נוסיף לרשימת הניקוי
}

void RadarWidget::plotPoint(int angleDeg, double distanceCm, double widthCm) {
    double r = distanceCm * cmToPx;
    double rad = qDegreesToRadians(static_cast<double>(angleDeg));
    double x = r * qCos(rad);
    double y = -r * qSin(rad);

    QPen pen(Qt::red);
    pen.setWidth(2);
    QGraphicsEllipseItem* point = scene->addEllipse(x - 2, y - 2, 4, 4, pen, QBrush(Qt::red));
    plottedItems.append(point);

    QString label = QString::number(widthCm, 'f', 1) + " cm";
    QGraphicsTextItem* textItem = scene->addText(label);
    textItem->setDefaultTextColor(Qt::white);
    textItem->setPos(x + 5, y - 5);
    plottedItems.append(textItem);
}

void RadarWidget::setAngleAndDistance(int angleDeg, uint16_t distanceCm,double widthCm ) {
    plotPoint(angleDeg, distanceCm,widthCm);
    updateSweepLine(angleDeg);
}

void RadarWidget::setCurrentAngle(int angleDeg) {
    updateSweepLine(angleDeg);
}

void RadarWidget::updateSweepLine(int angleDeg) {
    if (sweepLine) {
        scene->removeItem(sweepLine);
        delete sweepLine;
        sweepLine = nullptr;
    }

    double r = radarRadiusPx;
    double rad = qDegreesToRadians(static_cast<double>(angleDeg));
    double x = r * qCos(rad);
    double y = -r * qSin(rad);

    sweepLine = scene->addLine(0, 0, x, y, QPen(Qt::green, 2));
}

void RadarWidget::clearPoints() {
    for (QGraphicsItem* item : plottedItems) {
        scene->removeItem(item);
        delete item;
    }
    plottedItems.clear();
}
