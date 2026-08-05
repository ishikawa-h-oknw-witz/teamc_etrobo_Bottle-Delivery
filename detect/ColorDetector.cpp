#include "ColorDetector.h"

//コンストラクタ　ポート情報を持ったカラーセンサクラスのインスタンス化を受け取るために参照渡し
ColorDetector::ColorDetector(
    ColorSensor& sensor)
    : mColorSensor(sensor)
{
}

//目標色判定
bool ColorDetector::judgeColor(const std::vector<Color>& targetColors)
{
    Color detected = detectColor();

    for (Color color : targetColors)
    {
        if (detected == color)
        {
            return true;
        }
    }
    
    return false;
}

//色判定
Color ColorDetector::detectColor()
{
    ColorSensor::HSV hsv;
    mColorSensor.getColor(hsv);

    if (hsv.v <= 20)
    {
        return Color::Black;
    }

    if (hsv.s <= 20)
    {
        if (hsv.v >= 90)
        {
            return Color::White;
        }

        return Color::Gray;
    }

    if (hsv.h >= 30 &&
        hsv.h < 70)
    {
        return Color::Yellow;
    }

    if (hsv.h >= 70 &&
        hsv.h < 160)
    {
        return Color::Green;
    }

    if (hsv.h >= 160 && 
        hsv.h > 280)
    {
        return Color::Blue;
    }

    return Color::Red;
}