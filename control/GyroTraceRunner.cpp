#include "GyroTraceRunner.h"
#include "SceneManager.h"
#include "kernel.h"
#include <cmath>

namespace
{
float normalizeAngle(float angle)
{
    while (angle > 180.0f)
    {
        angle -= 360.0f;
    }

    while (angle < -180.0f)
    {
        angle += 360.0f;
    }

    return angle;
}
}

// コンストラクタ
GyroTraceRunner::GyroTraceRunner(
    Motor& leftMotor,
    Motor& rightMotor,
    DistanceCalculator& distanceCalculator,
    PIDCalculator& pidCalculate,
    TrapezoidCalculator& trapezoidCalculate)
    : mLeftMotor(leftMotor),
      mRightMotor(rightMotor),
      mDistanceCalculator(distanceCalculator),
      mPIDCalculator(pidCalculate),
      mTrapezoidCalculator(trapezoidCalculate),
      mBaseSpeed(60),
      mHeadingAverage(0.0f)
{
}

void GyroTraceRunner::setDirection(Direction direction)
{
    mDirection = direction;
}

void GyroTraceRunner::setBasespeed(int speed)
{
    mBaseSpeed = speed;
}

void GyroTraceRunner::setTargetAngle(float targetAngle)
{
    mTargetAngle = targetAngle;
}

void GyroTraceRunner::setHeadingAverage(int headingAverage)
{
    mHeadingAverage = headingAverage;
}

void GyroTraceRunner::move()
{
    // 毎周期、台形加減速で算出した正の速度から設定し直す
    // （前回の後退時に負へ変換した値を次周期へ持ち越さない）
    mBaseSpeed = mTrapezoidCalculator.getSpeed();

    // 後退の場合は速度を負にする
    if(mDirection==Direction::back)
    {
        mBaseSpeed = -mBaseSpeed;
    }

    float heading = mImu.getHeading();
    
    // 目標角度は0°
    float error = (mTargetAngle + mHeadingAverage) - heading;
    float correction = mPIDCalculator.calculate(error);

    int leftPower = 0;
    int rightPower = 0;

    leftPower  = mBaseSpeed + correction;
    rightPower = mBaseSpeed - correction;

    mLeftMotor.setPower(leftPower);
    mRightMotor.setPower(rightPower);

    tslp_tsk(10*1000);   // 約10ms周期
}

void GyroTraceRunner::turn()
{
    float currentHeading = mImu.getHeading();

    float error = normalizeAngle((mTargetAngle + mHeadingAverage) - currentHeading);

    /*
    // パルス制御
    // 誤差が5°以下になったら最低出力で5msだけ旋回し、一旦停止して再度誤差を確認する。
    // これを繰り返すことで、目標角度付近でのオーバーシュートを抑える。
    if (abs(error) <= 5)
    {
        if (error > 0)
        {
            mLeftMotor.setPower(40);
            mRightMotor.setPower(-40);
        }
        else
        {
            mLeftMotor.setPower(-40);
            mRightMotor.setPower(40);
        }

        tslp_tsk(5 * 1000);

        mLeftMotor.stop();
        mRightMotor.stop();
        tslp_tsk(10 * 1000);
    }
    */

    // PID制御
    // 誤差が5°を超える間はPID制御により旋回出力を計算する。
    int turnPower = static_cast<int>(
        std::fabs(mPIDCalculator.calculate(error)));

    //PID計算結果が40以上なら40に制限し、30以下なら30に引き上げる
    //上限を決めるのは安定させるため、下限を決めるのは走行体のスタックを防ぐため
    if (turnPower > 40)
    {
        turnPower = 40;
    }

    if (turnPower < 35) //最低動作出力
    {
        turnPower = 35;
    }

    if (error > 0) {
        mLeftMotor.setPower(turnPower);
        mRightMotor.setPower(-turnPower);
    }
    else {
        mLeftMotor.setPower(-turnPower);
        mRightMotor.setPower(turnPower);
    }
    tslp_tsk(10*1000);   // 約10ms周期
}

void GyroTraceRunner::stop()
{
    mLeftMotor.stop();
    mRightMotor.stop();
}
