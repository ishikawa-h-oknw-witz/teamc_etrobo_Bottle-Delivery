#include "BasePointDetector.h"

//コンストラクタ　ポート情報を持ったカラーセンサクラスのインスタンス化を受け取るために参照渡し
BasePointDetector::BasePointDetector(
    ColorSensor& sensor)
    : mColorSensor(sensor)
{
}

//基準点検知
bool BasePointDetector::detect(BasePointColor targetColor)
{
    // カラーセンサ取得

    // 色判定

    //targetcolorと一致するか

    return ;
}

//色判定
BasePointColor BasePointDetector::judgeColor(
    HSV hsv)
{
    if(/* 赤のHSV範囲 */)
    {
        return BasePointColor::RED;
    }

    if(/* 青のHSV範囲 */)
    {
        return BasePointColor::BLUE;
    }

    if(/* 黄のHSV範囲 */)
    {
        return BasePointColor::YELLOW;
    }

    if(/* 緑のHSV範囲 */)
    {
        return BasePointColor::GREEN;
    }
}