#pragma once
        
#include "LineTraceRunner.h"
#include "GyroTraceRunner.h"
#include "PIDCalculator.h"
#include "TrapezoidCalculator.h"
#include "IEventDetector.h"
#include "DistanceCalculator.h"
#include "TargetDistanceDetector.h"
#include "TargetAngleDetector.h"
#include "TargetColorDetector.h"
#include "ColorDetector.h"

enum class ActionType
{
    LineTrace,
    Move,
    Turn,
    BottoleDetect,
    Stop
};

enum class CalibrationData
{
    BlackWhiteCenter,
    LineCenter
};

struct LineTraceScene
{
    int sceneId;
    int targetDistance;
    int speed;
    RunnerEdge edge;
    Color finishColor[7];
    CalibrationData targetSensorValue;
    PID pid;
};

struct MoveScene
{
    int sceneId;
    Direction direction;
    TrapezoidParameter trapezoidParameter;
    int targetDistance;
    Color finishColor[7];
    PID pid;
};

struct TurnScene
{
    int sceneId;
    float targetAngle;
    PID pid;
};

struct BottleDetectScene
{
    int sceneId;
    Color detectColor[7];
};


class SceneManager
{
public:
    SceneManager(
        LineTraceRunner& lineTraceRunner,
        GyroTraceRunner& gyroTraceRunner,
        PIDCalculator& pidCalculator,
        TrapezoidCalculator& trapezoidCalculator,
        DistanceCalculator& distanceCalculator,
        TargetDistanceDetector& targetDistanceDetector,
        TargetAngleDetector& targetAngleDetector,
        TargetColorDetector& targetColorDetector);

    int getSceneID();
    void setSceneID(int sceneid);
    void setActionType(ActionType actiontype);
    bool SceneExecute();
    void setParameter(int sceneId);

private:
    LineTraceRunner& mLineTraceRunner;
    GyroTraceRunner& mGyroTraceRunner;
    PIDCalculator& mPIDCalculator;
    TrapezoidCalculator& mTrapezoidCalculator;
    DistanceCalculator& mDistanceCalculator;
    TargetDistanceDetector& mTargetDistanceDetector;
    TargetAngleDetector& mTargetAngleDetector;
    TargetColorDetector& mTargetColorDetector;
    IMU mImu;
    int mSceneId;
    ActionType mActionType;
    IEventDetector* mEventDetector;
};