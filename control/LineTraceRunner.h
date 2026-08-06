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

    void calibrateTargetReflection(int index);

    void calibrateTargetValue(int index);

    int getTargetSensorValue(int index) const;

    void setBaseSpeed(int speed);

    void setEdge(RunnerEdge edge);

    void setTargetSensorValue(int targetSensorValue);

    void run();

    void vrun();

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