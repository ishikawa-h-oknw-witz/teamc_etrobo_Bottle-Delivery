#include "LineTraceRunner.h"
#include "Logger.h"
#include "SceneManager.h"

// コンストラクタ
LineTraceRunner::LineTraceRunner(
    Motor& leftMotor,
    Motor& rightMotor,
    ColorSensor& colorSensor,
    PIDCalculator& pidCalculate)
    : mLeftMotor(leftMotor),
      mRightMotor(rightMotor),
      mColorSensor(colorSensor),
      mPIDCalculator(pidCalculate),
      mTargetReflection(50),
      mBaseSpeed(60)
{
}

void LineTraceRunner::setBaseSpeed(int speed)
{
    mBaseSpeed = speed;
}

void LineTraceRunner::setEdge(RunnerEdge edge)
{
    mEdge = edge;
}

void LineTraceRunner::run()
{
    int turn = 0;

    // 反射光取得
    int reflection =
        mColorSensor.getReflection();

    // 偏差計算
    int error =
        mTargetReflection - reflection;

    // PID制御依頼
    turn = mPIDCalculator.calculate(error);

    // モータ出力
    mLeftMotor.setPower(
        mBaseSpeed - mEdge * turn);
 
    mRightMotor.setPower(
        mBaseSpeed + mEdge * turn);

    tslp_tsk(10*1000);   // 約10ms周期
}

void LineTraceRunner::stop()
{
    mLeftMotor.stop();
    mRightMotor.stop();
}