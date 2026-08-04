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

    void setBaseSpeed(int speed);

    void setEdge(RunnerEdge edge);

    void run();

    void stop();

private:
    Motor& mLeftMotor;

    Motor& mRightMotor;

    ColorSensor& mColorSensor;

    PIDCalculator& mPIDCalculator;

    int mTargetReflection;

    int mBaseSpeed;

    RunnerEdge mEdge = RunnerEdge::RightEdge;
};

#endif