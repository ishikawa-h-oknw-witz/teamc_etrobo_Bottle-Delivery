#include "TrapezoidCalculator.h"

TrapezoidCalculator::TrapezoidCalculator(
            DistanceCalculator& distanceCalculator)
    : mDistanceCalculator(distanceCalculator),
      mAccelDistance(100.0f),
      mDecelDistance(200.0f)
{
}

void TrapezoidCalculator::setParameter(TrapezoidParameter parameter)
{
    mStartSpeed = parameter.startSpeed;
    mMaxSpeed = parameter.maxSpeed;
    mEndSpeed = parameter.endSpeed;
    mTotalDistance = parameter.totalDistance;
}

int TrapezoidCalculator::getSpeed()
{
    int currentDistance = mDistanceCalculator.getDistance();
    int remainingDistance = mTotalDistance - currentDistance;

    // 加速区間
    if (currentDistance < mAccelDistance)
    {
        return mStartSpeed +
            (mMaxSpeed - mStartSpeed)
            * currentDistance / mAccelDistance;
    }
    
    // 減速区間
    if (remainingDistance < mDecelDistance)
    {
        return mEndSpeed +
            (mMaxSpeed - mEndSpeed)
            * remainingDistance / mDecelDistance;
    }

    // 定速区間
    return mMaxSpeed;

}