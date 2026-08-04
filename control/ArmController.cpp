#include "ArmController.h"

//コンストラクタ
ArmController::ArmController(
    Motor& armMotor)
    : mArmMotor(armMotor)
{
}

void ArmController::setMaxAngle(int maxAngle)
{
    if(maxAngle > 90)
    {
        mMaxAngle = 90;
    }
    else
    {
        mMaxAngle = maxAngle;
    }
}

void ArmController::resetAngle()
{
    mArmMotor.resetCount();
}

void ArmController::moveArm(int angle)
{
    // 上限チェック
    if(angle > mMaxAngle)
    {
        angle = mMaxAngle;
    }

    int currentAngle = mArmMotor.getCount();

    while(currentAngle < angle)
    {
        mArmMotor.setPower(30);
        currentAngle = mArmMotor.getCount();
    }

    while(currentAngle > angle)
    {
        mArmMotor.setPower(-30);
        currentAngle = mArmMotor.getCount();
    }

    mArmMotor.stop();
}

void ArmController::moveArmDown()
{
    moveArm(0);
}

void ArmController::moveArmUp()
{
    moveArm(mMaxAngle);
}