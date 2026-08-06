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
      mTargetSensorValue(50),
      mBaseSpeed(60)
{
}

void LineTraceRunner::calibrateTargetReflection()
{
    int sum = 0;
    int reflection = 0;

    for (int i = 0; i < 10; i++)
    {
        reflection = mColorSensor.getReflection();
        sum += reflection;
        tslp_tsk(10*1000);   // 10ms待機
    }

    mTargetSensorValue = sum / 10;
}

void LineTraceRunner::calibrateTargetValue()
{
    int sum = 0;
    ColorSensor::HSV hsv;

    for (int i = 0; i < 10; i++)
    {
        mColorSensor.getHSV(hsv);
        sum += hsv.v;
        tslp_tsk(10*1000);   // 10ms待機
    }

    mTargetSensorValue = sum / 10;
}

int LineTraceRunner::getTargetSensorValue() const
{
    return mTargetSensorValue;
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
        mTargetSensorValue - reflection;

    // PID制御依頼
    turn = mPIDCalculator.calculate(error);

    // モータ出力
    mLeftMotor.setPower(
        mBaseSpeed - mEdge * turn);
 
    mRightMotor.setPower(
        mBaseSpeed + mEdge * turn);

    tslp_tsk(10*1000);   // 約10ms周期
}

void LineTraceRunner::vrun()
{
    ColorSensor::HSV hsv;
    int turn = 0;

    // 反射光取得
    mColorSensor.getHSV(hsv);

    // 偏差計算
    int error =
        mTargetSensorValue - hsv.v;

    // PID制御依頼
    turn = mPIDCalculator.calculate(error);

    // モータ出力
    mLeftMotor.setPower(
        mBaseSpeed + turn);
 
    mRightMotor.setPower(
        mBaseSpeed - turn);

    tslp_tsk(10*1000);   // 約10ms周期
}

void LineTraceRunner::stop()
{
    mLeftMotor.stop();
    mRightMotor.stop();
}