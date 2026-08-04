#ifndef BASE_POINT_DETECTOR_H
#define BASE_POINT_DETECTOR_H

#include "ColorSensor.h"

using namespace spikeapi;

class BasePointDetector
{
public:
    BasePointDetector(ColorSensor& sensor);

    bool detect(BasePointColor targetColor);
private:
    ColorSensor& mColorSensor;
    BasePointColor judgeColor(hsv);
};

#endif