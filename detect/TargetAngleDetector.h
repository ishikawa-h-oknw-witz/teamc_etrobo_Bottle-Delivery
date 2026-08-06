#ifndef TARGET_ANGLE_DETECTOR_H
#define TARGET_ANGLE_DETECTOR_H

#include "IMU.h"
#include "IEventDetector.h"

using namespace spikeapi;

class TargetAngleDetector : public IEventDetector
{
public:
    TargetAngleDetector();

    void setTargetAngle(int angle);
    void setAngleTolerance(int tolerance);

    bool judge() override;

private:
    IMU mIMU;
    int mTargetAngle;
    int mAngleTolerance;
};

#endif