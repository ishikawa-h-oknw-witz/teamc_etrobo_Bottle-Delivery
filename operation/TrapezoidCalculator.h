#ifndef TRAPEZOID_CALCULATOR_H
#define TRAPEZOID_CALCULATOR_H

#include "DistanceCalculator.h"

struct TrapezoidParameter
{
    float startSpeed;
    float maxSpeed;
    float endSpeed;
    float totalDistance;
};

class TrapezoidCalculator
{
public:
    TrapezoidCalculator(
            DistanceCalculator& distanceCalculator);
    TrapezoidCalculator();

    void setParameter(TrapezoidParameter parameter);

    int getSpeed();

private:
    DistanceCalculator& mDistanceCalculator;

    int mStartSpeed;
    int mMaxSpeed;
    int mEndSpeed;

    float mAccelDistance;
    float mDecelDistance;
    float mTotalDistance;
};

#endif