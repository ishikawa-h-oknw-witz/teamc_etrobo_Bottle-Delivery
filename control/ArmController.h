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

    //指定角度にアームを移動
    void moveArm(int angle);
    
    //最低角度にアームを移動
    void moveArmDown();

    //最大角度にアームを移動
    void moveArmUp();

private:
    Motor& mArmMotor;

    int mCurrentAngle;

    int mMaxAngle;
};

#endif