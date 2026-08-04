#ifndef DISTANCE_CALCULATOR_H
#define DISTANCE_CALCULATOR_H

#include "Motor.h"

using namespace spikeapi;

class DistanceCalculator
{
public:

    DistanceCalculator(
        Motor& leftMotor,
        Motor& rightMotor);

    /**
     * 走行距離計測開始
     */
    void reset();

    /**
     * 現在の走行距離取得(mm)
     */
    float getDistance();

private:

    Motor& mLeftMotor;

    Motor& mRightMotor;

    static constexpr float TIRE_DIAMETER = 56.0f;
};

#endif