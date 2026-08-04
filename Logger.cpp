#include "Logger.h"

#include <cstdarg>
#include <cstdio>

#include <syssvc/serial.h>
#include <spike/hub/bluetooth.h>

Logger::Logger(
    ColorSensor& colorSensor,
    Motor& leftMotor,
    Motor& rightMotor)
    : mColorSensor(colorSensor),
      mLeftMotor(leftMotor),
      mRightMotor(rightMotor)
{
}

void Logger::init()
{
    bool connected = false;

    serial_opn_por(2);

    while (!connected)
    {
        hub_bluetooth_is_connected(&connected);
        tslp_tsk(100 * 1000);
    }
}

void Logger::printf(const char* fmt, ...)
{
    char buffer[256];

    va_list args;
    va_start(args, fmt);

    int len = vsnprintf(
        buffer,
        sizeof(buffer),
        fmt,
        args);

    va_end(args);

    serial_wri_dat(2, buffer, len);
}

void Logger::output()
{
    ColorSensor::HSV hsv;
    mColorSensor.getHSV(hsv);

    printf(
        "Heading=%.2f "
        "Ref=%d "
        "H=%d S=%d V=%d "
        "L=%d R=%d "
        "LEnc=%d REnc=%d\r\n",

        mImu.getHeading(),
        mColorSensor.getReflection(),

        hsv.h,
        hsv.s,
        hsv.v,

        mLeftMotor.getPower(),
        mRightMotor.getPower(),

        mLeftMotor.getCount(),
        mRightMotor.getCount()
    );
}