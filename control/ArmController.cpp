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
    mArmMotor.setPower(100);
    tslp_tsk(2000*1000);

    mArmMotor.stop();
}

void ArmController::moveArmDown()
{
     mArmMotor.setPower(-50);
    tslp_tsk(600*1000);

    mArmMotor.stop();
}

void ArmController::moveArmUp()
{
     mArmMotor.setPower(50);
    tslp_tsk(450*1000);

    mArmMotor.stop();
}