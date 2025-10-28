#ifndef LIGHTBEAMDETECTOR_H
#define LIGHTBEAMDETECTOR_H

// File: LightBeamDetector.cpp
// Description: Detect vertical light beams from LDR sensor arrays (left/right)

#include <vector>
#include <utility> // for std::pair
#include <algorithm>
#include <numeric>
#include <cmath>
#include <optional>

// Optional: you can use Qt types if preferred
// #include <QVector>
// using FloatVec = QVector<float>;

using FloatVec = std::vector<float>;
using ResultVec = std::vector<std::pair<float, int>>;

class LightBeamDetector {
public:
    LightBeamDetector(const std::optional<FloatVec>& leftTable = std::nullopt,
                      const std::optional<FloatVec>& rightTable = std::nullopt)
        : lutLeft(leftTable), lutRight(rightTable) {}

    ResultVec detect(const FloatVec& left, const FloatVec& right, float angleStart, float angleEnd);

private:
    std::optional<FloatVec> lutLeft;
    std::optional<FloatVec> lutRight;

    FloatVec smooth(const FloatVec& data, int k);
    int findCenter(const FloatVec& left, const FloatVec& right);
    std::vector<int> detectCandidates(const FloatVec& primary, const FloatVec& secondary, int start, int end, float degPerBin, float centerAngle, bool isLeft);
    int valleyWidth(const FloatVec& dips, int idx, float frac);
    int valueToDistance(float val, const FloatVec& table);
};


#endif // LIGHTBEAMDETECTOR_H
