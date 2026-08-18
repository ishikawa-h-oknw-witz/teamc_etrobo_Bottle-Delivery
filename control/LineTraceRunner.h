#ifndef LINE_TRACE_RUNNNER_H
#define LINE_TRACE_RUNNNER_H

#include "ColorSensor.h"
#include "PIDCalculator.h"
#include "Motor.h"
#include "kernel.h"

using namespace spikeapi;

enum RunnerEdge
{
    LeftEdge = 1,
    RightEdge = -1
};

class LineTraceRunner
{
public:
    LineTraceRunner(
        Motor& leftMotor,
        Motor& rightMotor,
        ColorSensor& colorSensor,
        PIDCalculator& pidController);

    //Refllection用キャリブレーション
    void calibrateTargetReflection(int index);

    //Value用キャリブレーション
    void calibrateTargetValue(int index);

    //キャリブレーション値取得用のゲッター
    int getTargetSensorValue(int index) const;

    //基準速度設定用のセッター
    void setBaseSpeed(int speed);

    //走行エッジ設定用のセッター
    void setEdge(RunnerEdge edge);

    //目標輝度設定用のセッター
    void setTargetSensorValue(int targetSensorValue);

    //Reflectionを使った走行
    void run();

    //Valueを使った走行
    void vrun();

    //停止
    void stop();

private:
    Motor& mLeftMotor;

    Motor& mRightMotor;

    ColorSensor& mColorSensor;

    PIDCalculator& mPIDCalculator;

    int mTargetSensorValue;

    int mBaseSpeed;

    RunnerEdge mEdge = RunnerEdge::RightEdge;

    static const int CALIBRATION_NUM = 2;
    int mTargetSensorValues[CALIBRATION_NUM];
};

#endif