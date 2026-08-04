#ifndef LOGGER_H
#define LOGGER_H

#include "Motor.h"
#include "IMU.h"
#include "ColorSensor.h"

using namespace spikeapi;

class Logger
{
public:
    Logger(ColorSensor& colorSensor,
           Motor& leftMotor,
           Motor& rightMotor);

    void output();

    static void init();
    static void printf(const char* fmt, ...);

private:
    ColorSensor& mColorSensor;
    Motor& mLeftMotor;
    Motor& mRightMotor;
    IMU mImu;
};

#endif