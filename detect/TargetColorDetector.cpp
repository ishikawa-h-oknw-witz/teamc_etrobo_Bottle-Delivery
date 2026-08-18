#include "TargetColorDetector.h"
#include "Logger.h"
#include "kernel.h"

TargetColorDetector::TargetColorDetector(
    ColorDetector& colorDetector)
    : mColorDetector(colorDetector),
      mTargetColorNum(0)
{
}

void TargetColorDetector::setTargetColors(
    const Color targetColors[])
{
    mTargetColorNum = 7;

    for (int i = 0; i < 7; i++)
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

bool TargetColorDetector::judgeMultiple(
    int sampleCount,
    int requiredMatchCount,
    int sampleIntervalMs)
{
    if (sampleCount <= 0 ||
        requiredMatchCount <= 0 ||
        requiredMatchCount > sampleCount ||
        sampleIntervalMs < 0)
    {
        return false;
    }

    int matchCount = 0;

    for (int sample = 0; sample < sampleCount; sample++)
    {
        if (judge())
        {
            matchCount++;
        }

        if (sample + 1 < sampleCount && sampleIntervalMs > 0)
        {
            tslp_tsk(sampleIntervalMs * 1000);
        }
    }

    Logger::printf(
        "[TargetColorDetector]色検知回数=%d/%d 成功必要検知回数=%d\r\n",
        matchCount,
        sampleCount,
        requiredMatchCount);

    return matchCount >= requiredMatchCount;
}
