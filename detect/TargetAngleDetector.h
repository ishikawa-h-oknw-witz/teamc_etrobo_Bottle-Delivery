#pragma once

#include "IMU.h"
#include "IEventDetector.h"

using namespace spikeapi;

class TargetAngleDetector : public IEventDetector
{
public:
    TargetAngleDetector();

    void setTargetAngle(float angle);
    void setAngleTolerance(float tolerance);

    bool judge() override;

private:
    IMU mIMU;
    float mTargetAngle;
    float mAngleTolerance;
};
