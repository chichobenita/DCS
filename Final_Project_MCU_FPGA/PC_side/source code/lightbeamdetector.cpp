#include "lightbeamdetector.h"

// Smooth data with moving average
FloatVec LightBeamDetector::smooth(const FloatVec& data, int k) {
    if (k <= 1 || data.size() < 3) return data;
    k = std::max(1, k | 1); // ensure odd
    FloatVec out(data.size());
    int half = k / 2;
    for (size_t i = 0; i < data.size(); ++i) {
        float sum = 0.0f;
        int count = 0;
        for (int j = -half; j <= half; ++j) {
            int idx = static_cast<int>(i) + j;
            if (idx >= 0 && idx < static_cast<int>(data.size())) {
                sum += data[idx];
                ++count;
            }
        }
        out[i] = sum / count;
    }
    return out;
}

// Find crossing or center minimum
int LightBeamDetector::findCenter(const FloatVec& left, const FloatVec& right) {
    int n = left.size();
    for (int i = 1; i < n - 1; ++i) {
        if ((right[i] - left[i]) * (right[i - 1] - left[i - 1]) < 0)
            return i;
    }
    float minDiff = 1e9;
    int minIdx = 0;
    for (int i = 0; i < n; ++i) {
        float diff = std::abs(right[i] - left[i]);
        if (diff < minDiff) {
            minDiff = diff;
            minIdx = i;
        }
    }
    return minIdx;
}

// Estimate width of dip
int LightBeamDetector::valleyWidth(const FloatVec& dips, int idx, float frac) {
    float h = dips[idx] * frac;
    int L = idx, R = idx;
    while (L > 0 && dips[L] >= h) --L;
    while (R < dips.size() - 1 && dips[R] >= h) ++R;
    return std::max(1, R - L);
}

// Find candidate dips
std::vector<int> LightBeamDetector::detectCandidates(const FloatVec& primary, const FloatVec& secondary, int start, int end, float degPerBin, float centerAngle, bool isLeft) {
    const int smoothK = 3;
    FloatVec smoothed = smooth(FloatVec(primary.begin() + start, primary.begin() + end), smoothK);
    FloatVec base = smoothed;
    float basePct = 92.0f;
    std::nth_element(base.begin(), base.begin() + base.size() * basePct / 100, base.end());
    float bval = base[base.size() * basePct / 100];

    FloatVec dips(smoothed.size());
    for (size_t i = 0; i < dips.size(); ++i)
        dips[i] = bval - smoothed[i];

    std::vector<int> candidates;
    const float dipThreshold = 600.0f;  // תתאם לפי הנתונים שלך
    for (size_t i = 1; i < dips.size() - 1; ++i) {
        if (dips[i] >= dipThreshold &&
            dips[i] >= dips[i - 1] &&
            dips[i] >= dips[i + 1]) {
            candidates.push_back(start + i);
        }
    }
    return candidates;
}

// Convert LDR value to estimated distance using LUT
int LightBeamDetector::valueToDistance(float val, const FloatVec& table) {
    float bestDiff = 1e9;
    int bestIdx = 0;
    for (int i = 0; i < table.size(); ++i) {
        float diff = std::abs(table[i] - val);
        if (diff < bestDiff) {
            bestDiff = diff;
            bestIdx = i;
        }
    }
    return bestIdx + 1;
}

// Main detection function
ResultVec LightBeamDetector::detect(const FloatVec& left, const FloatVec& right, float angleStart, float angleEnd) {
    int n = left.size();
    ResultVec result;
    if (n == 0 || right.size() != n) return result;

    FloatVec angles(n);
    float step = (angleEnd - angleStart) / (n - 1);
    for (int i = 0; i < n; ++i)
        angles[i] = angleStart + i * step;

    float degPerBin = step;
    int m = findCenter(left, right);

    auto picksLeft = detectCandidates(left, right, 0, m, degPerBin, angles[m], true);
    auto picksRight = detectCandidates(right, left, m, n, degPerBin, angles[m], false);

    for (int i : picksLeft) {
        float angle = angles[i];
        int dist = lutLeft ? valueToDistance(left[i], *lutLeft) : 0;
        result.emplace_back(angle, dist);
    }
    for (int i : picksRight) {
        float angle = angles[i];
        int dist = lutRight ? valueToDistance(right[i], *lutRight) : 0;
        result.emplace_back(angle, dist);
    }

    return result;
}
