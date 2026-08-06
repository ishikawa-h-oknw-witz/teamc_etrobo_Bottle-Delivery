#include "TargetAngleDetector.h"

TargetAngleDetector::TargetAngleDetector(
    IMU& imu)
    : mIMU(imu),
      mTargetAngle(0),
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
    int currentAngle = mIMU.getAngle();

    return abs(currentAngle - mTargetAngle) <= mAngleTolerance;
}