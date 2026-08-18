#pragma once

struct PID
{
    float kp;
    float ki;
    float kd;
};

class PIDCalculator
{
public:
    PIDCalculator();
    
    void setGain(float kp, float ki, float kd);
    
    float calculate(float error);

    void reset();
 
private:
    float mIntegral;
 
    float mPreviousError;

    PID mPID;
};