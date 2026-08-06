#ifndef TARGET_ANGLE_DETECTOR_H
#define TARGET_ANGLE_DETECTOR_H

#include "IMU.h"
#include "EventDetector.h"

class TargetAngleDetector : public IEventDetector
{
public:
    TargetAngleDetector(IMU& imu);

    void setTargetAngle(int angle);
    void setAngleTolerance(int tolerance);

    bool judge() override;

private:
    IMU& mIMU;
    int mTargetAngle;
    int mAngleTolerance;
};

#endif