#include "LightSourceDetector.h"

// Smooths the input data using a symmetric moving average window
QVector<SmoothedPoint> LightSourceDetector::smoothValues(const QVector<Row>& input, int window) const {
    QVector<double> raw;
    for (const Row& row : input)
        raw.append(row.value);

    QVector<double> smooth = movingAvg(raw, window);

    QVector<SmoothedPoint> out;
    for (int i = 0; i < input.size(); ++i) {
        SmoothedPoint pt;
        pt.angle = input[i].angle;
        pt.rawValue = input[i].value;
        pt.smoothedValue = smooth[i];
        out.append(pt);
    }
    return out;
}

// Computes the moving average over a 1D vector with edge handling
QVector<double> LightSourceDetector::movingAvg(const QVector<double>& a, int k) {
    QVector<double> result;
    const int n = a.size();
    for (int i = 0; i < n; ++i) {
        int from = std::max(0, i - k);
        int to   = std::min(n - 1, i + k);
        double sum = 0;
        for (int j = from; j <= to; ++j)
            sum += a[j];
        result.append(sum / (to - from + 1));
    }
    return result;
}

// Main detection function using local derivative checks on smoothed signal
QVector<DetectedSource> LightSourceDetector::detectLocalMinima(
    const QMap<uint8_t, PointInScan>& scanData,
    double minSeparationDeg) const
{
    // Step 1: convert and sort the scan data by angle
    QVector<Row> rawValues;
    for (auto it = scanData.begin(); it != scanData.end(); ++it) {
        const auto& p = it.value();
        uint16_t minVal = std::min(p.ldr1Sample, p.ldr2Sample);
        rawValues.append({ double(p.degree), double(minVal) });
    }
    std::sort(rawValues.begin(), rawValues.end(), [](const Row& a, const Row& b) {
        return a.angle < b.angle;
    });

    // Step 2: smooth the raw signal values
    QVector<SmoothedPoint> smoothed = smoothValues(rawValues, 2);

    for (const SmoothedPoint& pt : smoothed) {
        qDebug() << "angle:" << pt.angle
                 << "raw :" << pt.rawValue
                 << "smoothed :" << pt.smoothedValue;
    }
    // Step 3: detect local minima based on changes in the derivative
    QVector<DetectedSource> out;
    double lastAcceptedAngle = -999.0;

    for (int i = 1; i < smoothed.size() - 1; ++i) {
        double prevDiff = smoothed[i].smoothedValue - smoothed[i - 1].smoothedValue;
        double nextDiff = smoothed[i + 1].smoothedValue - smoothed[i].smoothedValue;

        // Local minimum: decrease before, increase after
        if (prevDiff < 0 && nextDiff > 0) {
            double angle = smoothed[i].angle;

            // Enforce minimum separation between detected sources
            if (!out.isEmpty() && angle - lastAcceptedAngle < minSeparationDeg)
                continue;

            // Store detected source
            DetectedSource src;
            src.angleDeg = angle;
            //src.score = ((smoothed[i - 1].smoothedValue + smoothed[i + 1].smoothedValue) / 2.0) - smoothed[i].smoothedValue;
            src.score = smoothed[i].smoothedValue;
            src.merged = 1;
            out.append(src);

            lastAcceptedAngle = angle;
        }
    }

    return out;
}
