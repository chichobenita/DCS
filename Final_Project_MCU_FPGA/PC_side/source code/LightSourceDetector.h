#ifndef LIGHTSOURCEDETECTOR_H
#define LIGHTSOURCEDETECTOR_H
#pragma once

#include <QtCore/QMap>
#include <QtCore/QVector>
#include <QtCore/QDebug>
#include <algorithm>
#include <limits>
#include <cstdint>
#include <cmath>

// --- Input point structure from the scan ---
struct PointInScan {
    uint8_t  distance_cm;
    uint16_t ldr1Sample;
    uint16_t ldr2Sample;
    uint16_t degree;
};

// --- Output structure for detected light sources ---
struct DetectedSource {
    double angleDeg;  // Detected light source angle
    double score;     // Prominence score
    int merged;       // Always 1 for now
};

// --- Internal structure for sorting and processing ---
struct Row {
    double angle;
    double value;
};

// --- Debugging structure: raw vs smoothed values ---
struct SmoothedPoint {
    double angle;
    double rawValue;
    double smoothedValue;
};

// --- Main detector class ---
class LightSourceDetector {
public:
    // Detect all local minima after smoothing the data
    QVector<DetectedSource> detectLocalMinima(
        const QMap<uint8_t, PointInScan>& scanData,
        double minSeparationDeg) const;

private:
    // Helper to smooth signal using moving average
    QVector<SmoothedPoint> smoothValues(const QVector<Row>& input, int window) const;

    // Compute simple moving average
    static QVector<double> movingAvg(const QVector<double>& a, int k);
};

#endif // LIGHTSOURCEDETECTOR_H
