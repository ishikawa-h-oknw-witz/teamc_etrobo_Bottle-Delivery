#include "ColorDetector.h"
#include "Logger.h"

static constexpr ColorHSVRange mColorHSVRanges[] =
{
    { Color::Red,      0,29, 41,100,41,100 },
    { Color::Red,    280,360,41,100,41,100 },
    { Color::Blue,   160,279,41,100,41,100 },
    { Color::Yellow,  30, 69,41,100,41,100 },
    { Color::Green,   70,159,41,100,41,100 },
    { Color::Gray,     0,360, 0, 40,41, 89 },
    { Color::Black,    0,360, 0,100, 0, 40 },
    { Color::White,    0,360, 0, 40,90,100 }
};

ColorDetector::ColorDetector(ColorSensor& sensor)
    : mColorSensor(sensor),
      mLastDetectedColor(Color::Unknown)
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

    mLastDetectedColor = Color::Unknown;
    return mLastDetectedColor;
}

Color ColorDetector::getLastDetectedColor() const
{
    return mLastDetectedColor;
}
