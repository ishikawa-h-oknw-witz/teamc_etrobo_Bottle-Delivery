#ifndef GYRO_TRACE_RUNNER_H
#define GYRO_TRACE_RUNNER_H

#include "Motor.h"
#include "DistanceCalculator.h"
#include "PIDCalculator.h"
#include "TrapezoidCalculator.h"
#include "IMU.h"
#include "Clock.h"

using namespace spikeapi;

enum class Direction
{
    front,
    back
};

class GyroTraceRunner
{
public:
    GyroTraceRunner(
            Motor& leftMotor,
            Motor& rightMotor,
            DistanceCalculator& distanceCalculator,
            PIDCalculator& pidCalculate,
            TrapezoidCalculator& trapezoidCalculate);
    
    void setDirection(Direction direction);

    void setBasespeed(int speed);

    void setTargetAngle(float targetAngle);

    //前後退走行
    void move();

    //旋回
    void turn();

    void stop();

private:
    Motor& mLeftMotor;
    Motor& mRightMotor;
    DistanceCalculator& mDistanceCalculator;
    PIDCalculator& mPIDCalculator;
    TrapezoidCalculator& mTrapezoidCalculator;
    IMU mImu;
    Clock mClock;
    int mBaseSpeed;
    float mTargetAngle;
    Direction mDirection;
};
#endif