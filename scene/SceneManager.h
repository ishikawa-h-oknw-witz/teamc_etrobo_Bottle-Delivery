#pragma once
        
#include "LineTraceRunner.h"
#include "GyroTraceRunner.h"
#include "PIDCalculator.h"
#include "TrapezoidCalculator.h"
#include "IEventDetector.h"
#include "TargetDistanceDetector.h"
#include "DistanceCalculator.h"

enum class ActionType
{
    LineTrace,
    Move,
    Turn
};

enum class Color
{
    None,
    Red,
    Blue,
    Yellow,
    Green,
    Gray,
    Black,
    White
};

struct LineTraceScene
{
    int sceneId;
    int targetDistance;
    int speed;
    RunnerEdge edge;
    Color finishColor;
    int targetReflection;
    PID pid;
};

class SceneManager
{
public:
    SceneManager(
        LineTraceRunner& lineTraceRunner,
        GyroTraceRunner& gyroTraceRunner,
        PIDCalculator& pidCalculator,
        TrapezoidCalculator& trapezoidCalculator,
        TargetDistanceDetector& targetDistanceDetector,
        DistanceCalculator& distanceCalculator);

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
    TargetDistanceDetector& mTargetDistanceDetector;
    DistanceCalculator& mDistanceCalculator;

    int mSceneId;
    ActionType mActionType;
    IEventDetector* mEventDetector;
};