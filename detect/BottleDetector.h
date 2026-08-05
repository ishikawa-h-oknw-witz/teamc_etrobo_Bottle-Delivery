#ifndef BOTTLE_DETECTOR_H
#define BOTTLE_DETECTOR_H

#include "ColorSensor.h"
#include "SceneManager.h"

using namespace spikeapi;

class BottleDetector
{
public:
    BottleDetector(ColorSensor& sensor);

    bool judgeYellow(Color targetColor);
    bool judgeBlue(Color targetColor);
    bool judgeRed(Color targetColor);
private:
    ColorSensor& mColorSensor;
};

#endif