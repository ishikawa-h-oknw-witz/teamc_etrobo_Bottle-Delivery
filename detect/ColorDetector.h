#ifndef COLOR_DETECTOR_H
#define COLOR_DETECTOR_H

#pragma once

#include <initializer_list>
#include "ColorSensor.h"

using namespace spikeapi;

enum class Color
{
    None,
    Red,
    Blue,
    Yellow,
    Green,
    Gray,
    Black,
    White
};

class ColorDetector
{
public:
    ColorDetector(ColorSensor& sensor);

    bool judgeColor(std::initializer_list<Color> targetColors);
private:
    ColorSensor& mColorSensor;
    Color detectColor();
};

#endif