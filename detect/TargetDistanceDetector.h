#pragma once

#include "IEventDetector.h"
#include "DistanceCalculator.h"

class TargetDistanceDetector : public IEventDetector
{
public:
    TargetDistanceDetector(DistanceCalculator& distanceCalculator);

    void setTargetDistance(int distance);

    bool judge() override;

private:
    DistanceCalculator& mDistanceCalculator;
    int mTargetDistance;
};