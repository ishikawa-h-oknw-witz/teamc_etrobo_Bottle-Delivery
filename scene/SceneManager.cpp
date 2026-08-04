#include "SceneManager.h"
#include "Logger.h"

// {シーンID, 目標距離, 速度, 走行エッジ, 終了色, 目標輝度, {Kp, Ki, Kd}}
LineTraceScene lineTraceScenes[] =
{
    { 0,  500, 100, RunnerEdge::RightEdge, Color::None, 50, {0.2f, 0.0f, 0.2f} }, // 直線1
    { 1,  150,  80, RunnerEdge::RightEdge, Color::None, 50, {0.6f, 0.0f, 0.4f} }, // カーブ1-1
    { 2,  100,  70, RunnerEdge::RightEdge, Color::None, 50, {0.5f, 0.0f, 0.4f} }, // カーブ1-2
    { 3,  150,  80, RunnerEdge::RightEdge, Color::None, 50, {0.6f, 0.0f, 0.4f} }, // カーブ1-3
    { 4,  400, 100, RunnerEdge::RightEdge, Color::None, 50, {0.3f, 0.0f, 0.4f} }, // 直線2
    { 5,  150,  80, RunnerEdge::RightEdge, Color::None, 50, {0.6f, 0.0f, 0.4f} }, // カーブ2-1
    { 6,  100,  70, RunnerEdge::RightEdge, Color::None, 50, {0.5f, 0.0f, 0.4f} }, // カーブ2-2
    { 7,  100,  80, RunnerEdge::RightEdge, Color::None, 50, {0.6f, 0.0f, 0.4f} }, // カーブ2-3
    { 8,  300, 100, RunnerEdge::RightEdge, Color::None, 50, {0.3f, 0.0f, 0.4f} }, // 直線3
    { 9,  400,  60, RunnerEdge::RightEdge, Color::None, 50, {0.5f, 0.0f, 0.4f} }, // カーブ3
    {10,  900, 100, RunnerEdge::RightEdge, Color::None, 50, {0.6f, 0.0f, 0.4f} }, // 蛇行1
    {11,  900,  80, RunnerEdge::RightEdge, Color::None, 50, {0.5f, 0.0f, 0.4f} }, // 蛇行2
    {12, 1500, 100, RunnerEdge::RightEdge, Color::None, 50, {0.4f, 0.0f, 0.4f} }  // 直線4
};

//コンストラクタ
SceneManager::SceneManager(
    LineTraceRunner& lineTraceRunner,
    GyroTraceRunner& gyroTraceRunner,
    PIDCalculator& pidCalculator,
    TrapezoidCalculator& trapezoidCalculator,
    TargetDistanceDetector& targetDistanceDetector,
    DistanceCalculator& distanceCalculator)
    : mLineTraceRunner(lineTraceRunner),
      mGyroTraceRunner(gyroTraceRunner),
      mPIDCalculator(pidCalculator),
      mTrapezoidCalculator(trapezoidCalculator),
      mTargetDistanceDetector(targetDistanceDetector),
      mDistanceCalculator(distanceCalculator),
      mSceneId(0),
      mEventDetector(nullptr)
{
}

int SceneManager::getSceneID()
{
    return mSceneId;
}

void SceneManager::setSceneID(int sceneId)
{
    mSceneId = sceneId;
}

void SceneManager::setActionType(ActionType actiontype)
{
    mActionType = actiontype;
}

bool SceneManager::SceneExecute()
{
    setParameter(mSceneId);

    mDistanceCalculator.reset();
    
    while(!mEventDetector->judge())
    {
        // 走行実行
        switch (mActionType)
        {
        case ActionType::LineTrace:
            mLineTraceRunner.run();
            break;

        /*
        case ActionType::Move:
            mGyroTraceRunner.move();
            break;

        case ActionType::Turn:
            mGyroTraceRunner.turn();
            break;
        */
        default:
            break;
        }
        //tslp_tsk(10 * 1000);
    }

    // シーン終了
    return true;
}

void SceneManager::setParameter(int sceneId)
{

    switch(mActionType)
    {
    case ActionType::LineTrace:
        const LineTraceScene& linetracescene = lineTraceScenes[mSceneId];

        // ライントレース
        mLineTraceRunner.setBaseSpeed(linetracescene.speed);

        // PID
        mPIDCalculator.setGain(
            linetracescene.pid.kp,
            linetracescene.pid.ki,
            linetracescene.pid.kd);

        // エッジ
        mLineTraceRunner.setEdge(linetracescene.edge);

        // 走行距離
        if (linetracescene.targetDistance != 0)
        {
            mTargetDistanceDetector.setTargetDistance(linetracescene.targetDistance);
            mEventDetector = &mTargetDistanceDetector;
        }
        break;

        /*
        case ActionType::Move:
            mGyroTraceRunner.move();
            break;

        case ActionType::Turn:
            mGyroTraceRunner.turn();
            break;*/
    }
}