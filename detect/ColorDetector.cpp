#include "ColorDetector.h"
#include "Logger.h"

static constexpr ColorHSVRange mColorHSVRanges[] =
{
    { Color::Red,      0,29, 51,100,11,100 },
    { Color::Red,    280,360,51,100,11,100 },
    { Color::Blue,   160,279,51,100,11,100 },
    { Color::Yellow,  30, 69,51,100,11,100 },
    { Color::Green,   70,159,51,100,11,100 },
    { Color::Gray,     0,360, 0, 50,11, 89 },
    { Color::Black,    0,360, 0,100, 0, 10 },
    { Color::White,    0,360, 0, 50,90,100 }
};

ColorDetector::ColorDetector(ColorSensor& sensor)
    : mColorSensor(sensor)
{
}

Color ColorDetector::detect()
{
    ColorSensor::HSV hsv;
    mColorSensor.getHSV(hsv);

    for (const auto& range : mColorHSVRanges)
    {
        if (hsv.h >= range.hMin && hsv.h <= range.hMax &&
            hsv.s >= range.sMin && hsv.s <= range.sMax &&
            hsv.v >= range.vMin && hsv.v <= range.vMax)
        {
            Logger::printf("[ColorDetector]指定色を検知しました\n");
            Logger::printf("H:%d, S:%d, V:%d\n", hsv.h, hsv.s, hsv.v);
            return range.color;
        }
    }

    return Color::Unknown;   // または適切なデフォルト
}