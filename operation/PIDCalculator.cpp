#include "PIDCalculator.h"
 
// コンストラクタ
PIDCalculator::PIDCalculator()
    : mIntegral(0),
      mPreviousError(0)
{
}

void PIDCalculator::setGain(float kp, float ki, float kd)
{
    mPID.kp = kp;
    mPID.ki = ki;
    mPID.kd = kd;
}

void PIDCalculator::reset()
{
    mIntegral = 0.0f;
    mPreviousError = 0.0f;
}

float PIDCalculator::calculate(float error)
{
    //float dt = 0.01f;
    
    // 積分項更新
    mIntegral += error;
 
    // 微分項計算
    float diff = error - mPreviousError;
    //float diff = (error - mPreviousError) / dt;
    
 
    // PID計算
    float turn =
        mPID.kp * error +
        mPID.ki * mIntegral +
        mPID.kd * diff;
 
    mPreviousError = error;

    return turn;
}