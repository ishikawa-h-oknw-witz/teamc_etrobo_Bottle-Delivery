#include "TargetColorDetector.h"
#include "Logger.h"

TargetColorDetector::TargetColorDetector(
    ColorDetector& colorDetector)
    : mColorDetector(colorDetector),
      mTargetColorNum(0)
{
}

void TargetColorDetector::setTargetColors(
    const Color targetColors[], int num)
{
    mTargetColorNum = num;

    for (int i = 0; i < num; i++)
    {
        mTargetColors[i] = targetColors[i];
    }
}

bool TargetColorDetector::judge()
{
    Color detected = mColorDetector.detect();

    for (int i = 0; i < mTargetColorNum; i++)
    {
        if (detected == mTargetColors[i])
        {
            return true;
        }
    }

    return false;
}