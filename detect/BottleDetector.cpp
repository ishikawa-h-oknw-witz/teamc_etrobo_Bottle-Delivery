#include "BottleDetector.h"

//コンストラクタ　ポート情報を持ったカラーセンサクラスのインスタンス化を受け取るために参照渡し
BottleDetector::BottleDetector(
    ColorSensor& sensor)
    : mColorSensor(sensor)
{
}

//黄色ボトル検知
bool BottleDetector::judgeYellow(Color targetColor)
{
    ColorSensor::HSV hsv;   
    mColorSensor.getColor(hsv);

    if (hsv.v > 30 &&
        hsv.s > 30 &&
        (hsv.h >= 30 && hsv.h < 70))
    {
        return true;
    }

    return false;
}

//青ボトル検知
bool BottleDetector::judgeBlue(Color targetColor)
{
    ColorSensor::HSV hsv;   
    mColorSensor.getColor(hsv);

    if (hsv.v > 30 &&
        hsv.s > 30 &&
        (hsv.h >= 170 && hsv.h < 265))
    {
        return true;
    }

    return false;
}

//赤ボトル検知
bool BottleDetector::judgeRed(Color targetColor)
{
    ColorSensor::HSV hsv;   
    mColorSensor.getColor(hsv);

    if (hsv.v > 30 &&
        hsv.s > 30 &&
        (hsv.h >= 0 && hsv.h < 30) ||
        (hsv.h > 320 && hsv.h <= 359))
    {
        return true;
    }

    return false;
}