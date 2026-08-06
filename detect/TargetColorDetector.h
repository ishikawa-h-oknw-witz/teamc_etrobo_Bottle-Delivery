#pragma once

#include "IEventDetector.h"
#include "ColorDetector.h"

class TargetColorDetector : public IEventDetector
{
public:
    TargetColorDetector(
        ColorDetector& colorDetector);

    bool judge() override;

    void setTargetColors(const Color targetColors[], int num);

private:
    ColorDetector& mColorDetector;
    
    static const int MAX_TARGET_COLORS = 3;

    Color mTargetColors[MAX_TARGET_COLORS];

    int mTargetColorNum;
};