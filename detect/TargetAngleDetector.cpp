#include "TargetAngleDetector.h"
#include <cmath>

namespace
{
float normalizeAngle(float angle)
{
    while (angle > 180.0f)
    {
        angle -= 360.0f;
    }

    while (angle < -180.0f)
    {
        angle += 360.0f;
    }

    return angle;
}
}

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
