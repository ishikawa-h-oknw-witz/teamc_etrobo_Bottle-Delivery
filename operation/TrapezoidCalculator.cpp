#include "TrapezoidCalculator.h"

TrapezoidCalculator::TrapezoidCalculator(
            DistanceCalculator& distanceCalculator)
    : mDistanceCalculator(distanceCalculator)
{
}

void TrapezoidCalculator::setParameter(TrapezoidParameter parameter)
{
    mStartSpeed = parameter.startSpeed;
    mMaxSpeed = parameter.maxSpeed;
    mEndSpeed = parameter.endSpeed;
    mTotalDistance = parameter.totalDistance;

    // 全体距離に対する割合で加速・減速距離を決定
    mAccelDistance = mTotalDistance * 0.2f;
    mDecelDistance = mTotalDistance * 0.3f;
}

int TrapezoidCalculator::getSpeed()
{
    int currentDistance = mDistanceCalculator.getDistance();
    int remainingDistance = mTotalDistance - currentDistance;

    // 加速区間：最初の20%
    if (currentDistance < mAccelDistance)
    {
        return mStartSpeed +
            (mMaxSpeed - mStartSpeed)
            * currentDistance / mAccelDistance;
    }

    // 減速区間：最後の30%
    if (remainingDistance < mDecelDistance)
    {
        return mEndSpeed +
            (mMaxSpeed - mEndSpeed)
            * remainingDistance / mDecelDistance;
    }

    // 定速区間：中間の50%
    return mMaxSpeed;
}