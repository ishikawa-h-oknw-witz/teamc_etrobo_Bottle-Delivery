#ifndef ARM_CONTROLLER_H
#define ARM_CONTROLLER_H

#include "Motor.h"
#include "kernel.h"

using namespace spikeapi;

class ArmController
{
public:
    ArmController(
        Motor& ArmMotor);

    void setMaxAngle(int maxangle);

    void resetAngle();

    void moveArm(int angle);
    
    void moveArmDown();

    void moveArmUp();

private:
    Motor& mArmMotor;

    int mCurrentAngle;

    int mMaxAngle;
};

#endif