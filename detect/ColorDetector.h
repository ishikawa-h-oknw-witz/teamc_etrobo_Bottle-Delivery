#pragma once

#include "ColorSensor.h"

enum class Color
{
    None,
    Black,
    White,
    Gray,
    Red,
    Yellow,
    Green,
    Blue
};

struct ColorHSVRange
{
    Color color;

    int hMin;
    int hMax;

    int sMin;
    int sMax;

    int vMin;
    int vMax;
};

class ColorDetector
{
public:
    ColorDetector(ColorSensor& sensor);

    Color detect();

private:
    ColorSensor& mColorSensor;
};