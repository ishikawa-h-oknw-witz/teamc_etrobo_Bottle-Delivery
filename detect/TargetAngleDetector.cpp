#include "TargetAngleDetector.h"
#include <stdlib.h>

TargetAngleDetector::TargetAngleDetector()
    : mTargetAngle(0),
      mAngleTolerance(0.5)
{
}

void TargetAngleDetector::setTargetAngle(int angle)
{
    mTargetAngle = angle;
}

void TargetAngleDetector::setAngleTolerance(int tolerance)
{
    mAngleTolerance = tolerance;
}

bool TargetAngleDetector::judge()
{
    int currentAngle = mIMU.getHeading();

    return abs(currentAngle - mTargetAngle) <= mAngleTolerance;
}