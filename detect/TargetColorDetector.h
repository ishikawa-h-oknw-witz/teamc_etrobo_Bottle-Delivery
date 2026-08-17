#pragma once

#include "IEventDetector.h"
#include "ColorDetector.h"

class TargetColorDetector : public IEventDetector
{
public:
    TargetColorDetector(
        ColorDetector& colorDetector);

    bool judge() override;

    /**
     * •¡”‰ñ‘ª’è‚µA•K—v‰ñ”ˆÈãˆê’v‚µ‚½ê‡‚Étrue‚ğ•Ô‚·
     */
    bool judgeMultiple(
        int sampleCount,
        int requiredMatchCount,
        int sampleIntervalMs);

    void setTargetColors(const Color targetColors[]);

private:
    ColorDetector& mColorDetector;
    
    static const int MAX_TARGET_COLORS = 7;

    Color mTargetColors[MAX_TARGET_COLORS];

    int mTargetColorNum;
};
