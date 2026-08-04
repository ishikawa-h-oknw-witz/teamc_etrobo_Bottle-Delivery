#include "TargetDistanceDetector.h"

TargetDistanceDetector::TargetDistanceDetector(
    DistanceCalculator& distanceCalculator)
    : mDistanceCalculator(distanceCalculator),
      mTargetDistance(0)
{
}

void TargetDistanceDetector::setTargetDistance(int distance)
{
    mTargetDistance = distance;
}

bool TargetDistanceDetector::judge()
{
    return mDistanceCalculator.getDistance() >= mTargetDistance;
}