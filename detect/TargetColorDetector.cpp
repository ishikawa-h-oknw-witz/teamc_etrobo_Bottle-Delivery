#include "TargetColorDetector.h"
#include "Logger.h"
#include "kernel.h"

namespace
{
// 同じ色区間を連続して別の基準点として検知しないためのロック。
// 一度色を検知した後、色以外の場所へ抜けると次の検知を許可する。
bool waitingForColorRelease = false;
}

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
    bool isTargetColor = false;

    for (int i = 0; i < mTargetColorNum; i++)
    {
        if (detected == mTargetColors[i])
        {
            isTargetColor = true;
            break;
        }
    }

    if (waitingForColorRelease)
    {
        if (!isTargetColor)
        {
            waitingForColorRelease = false;
        }

        return false;
    }

    if (isTargetColor)
    {
        waitingForColorRelease = true;
        return true;
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
        const Color detected = mColorDetector.detect();

        for (int targetIndex = 0; targetIndex < mTargetColorNum; targetIndex++)
        {
            if (detected == mTargetColors[targetIndex])
            {
                matchCount++;
                break;
            }
        }

        if (sample + 1 < sampleCount && sampleIntervalMs > 0)
        {
            tslp_tsk(sampleIntervalMs * 1000);
        }
    }

    Logger::printf(
        "[TargetColorDetector]濶ｲ讀懃衍蝗樊焚=%d/%d 謌仙粥蠢�隕∵､懃衍蝗樊焚=%d\r\n",
        matchCount,
        sampleCount,
        requiredMatchCount);

    return matchCount >= requiredMatchCount;
}
