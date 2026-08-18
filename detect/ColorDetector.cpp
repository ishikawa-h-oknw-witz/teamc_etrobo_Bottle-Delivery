#include "ColorDetector.h"
#include "Logger.h"

static constexpr ColorHSVRange mColorHSVRanges[] =
{
    { Color::Red,      0,29, 21,100,11,100 },
    { Color::Red,    280,360,21,100,11,100 },
    { Color::Blue,   160,279,21,100,11,100 },
    { Color::Yellow,  30, 69,21,100,11,100 },
    { Color::Green,   70,159,21,100,11,100 },
    { Color::Gray,     0,360, 0, 20,11, 89 },
    { Color::Black,    0,360, 0,100, 0, 20 },
    { Color::White,    0,360, 0, 20,90,100 }
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
            return range.color;
        }
    }

    return Color::Unknown;   // または適切なデフォルト
}