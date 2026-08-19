#include "TargetAngleDetector.h"
#include <stdlib.h>

TargetAngleDetector::TargetAngleDetector()
    : mTargetAngle(0.0f),
      mAngleTolerance(0.5f)
{
}

void TargetAngleDetector::setTargetAngle(float angle)
{
    mTargetAngle = angle;
}

void TargetAngleDetector::setAngleTolerance(float tolerance)
{
    mAngleTolerance = tolerance;
}

bool TargetAngleDetector::judge()
{
    float currentAngle = mIMU.getHeading();

    return abs(currentAngle - mTargetAngle) <= mAngleTolerance;
}