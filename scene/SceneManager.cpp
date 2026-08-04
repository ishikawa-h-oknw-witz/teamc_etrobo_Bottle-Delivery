#include "SceneManager.h"
#include "Logger.h"

// {シーンID, 目標距離, 速度, 走行エッジ, 終了色, 目標輝度, {Kp, Ki, Kd}}
LineTraceScene lineTraceScenes[] =
{
    { 0,  500, 100, RunnerEdge::RightEdge, Color::None, 50, {0.2f, 0.0f, 0.2f} }, // Lap直線1
    { 1,  150,  80, RunnerEdge::RightEdge, Color::None, 50, {0.6f, 0.0f, 0.4f} }, // Lapカーブ1-1
    { 2,  100,  70, RunnerEdge::RightEdge, Color::None, 50, {0.5f, 0.0f, 0.4f} }, // Lapカーブ1-2
    { 3,  150,  80, RunnerEdge::RightEdge, Color::None, 50, {0.6f, 0.0f, 0.4f} }, // Lapカーブ1-3
    { 4,  400, 100, RunnerEdge::RightEdge, Color::None, 50, {0.3f, 0.0f, 0.4f} }, // Lap直線2
    { 5,  150,  80, RunnerEdge::RightEdge, Color::None, 50, {0.6f, 0.0f, 0.4f} }, // Lapカーブ2-1
    { 6,  100,  70, RunnerEdge::RightEdge, Color::None, 50, {0.5f, 0.0f, 0.4f} }, // Lapカーブ2-2
    { 7,  100,  80, RunnerEdge::RightEdge, Color::None, 50, {0.6f, 0.0f, 0.4f} }, // Lapカーブ2-3
    { 8,  300, 100, RunnerEdge::RightEdge, Color::None, 50, {0.3f, 0.0f, 0.4f} }, // Lap直線3
    { 9,  400,  60, RunnerEdge::RightEdge, Color::None, 50, {0.5f, 0.0f, 0.4f} }, // Lapカーブ3
    {10,  900, 100, RunnerEdge::RightEdge, Color::None, 50, {0.6f, 0.0f, 0.4f} }, // Lap蛇行1
    {11,  900,  80, RunnerEdge::RightEdge, Color::None, 50, {0.5f, 0.0f, 0.4f} }, // Lap蛇行2
    {12, 1500, 100, RunnerEdge::RightEdge, Color::None, 50, {0.4f, 0.0f, 0.4f} }, // Lap直線4
    {13,    0,  70, RunnerEdge::RightEdge, Color::None, 20, {0.4f, 0.0f, 0.4f} }, // Dlvボトルまで
    {14,  600,  70, RunnerEdge::LeftEdge,  Color::None, 50, {0.6f, 0.0f, 0.4f} }, // Dlvカーブ1
    {15,    0,  60, RunnerEdge::LeftEdge,  Color::Blue, 50, {0.6f, 0.0f, 0.4f} }, // Dlvカーブ2
    {16, 1200, 100, RunnerEdge::LeftEdge,  Color::None, 50, {0.3f, 0.0f, 0.4f} }, // Dlv直線1
    {17,  300,  60, RunnerEdge::LeftEdge,  Color::None, 50, {0.6f, 0.0f, 0.4f} }, // Dlvカーブ3
    {18,    0, 100, RunnerEdge::LeftEdge,  Color::Blue, 50, {0.3f, 0.0f, 0.4f} }, // Dlv青まで
    {19,  400, 100, RunnerEdge::RightEdge, Color::None, 50, {0.3f, 0.0f, 0.4f} }, // Dlv帰還直線1
    {20,  300,  60, RunnerEdge::RightEdge, Color::None, 50, {0.6f, 0.0f, 0.4f} }, // Dlv帰還カーブ1
    {21,    0, 100, RunnerEdge::RightEdge, Color::Blue, 50, {0.3f, 0.0f, 0.4f} }  // Dlv帰還青まで
};

MoveScene moveScenes[] =
{
    {0, 100, Direction::front,  70, Color::None, {1.0f, 0.0f, 0.0f}}, // Dlv青スルー
    {1, 300, Direction::front, 100, Color::None, {1.0f, 0.0f, 0.0f}}, // Dlvエリアまで
    {1, 300, Direction::back,  100, Color::None, {1.0f, 0.0f, 0.0f}}  // Dlv線まで帰還
};

TrunScene trunScenes[] =
{
    {0, 90, {1.0f, 0.0f, 0.0f}} //右に90°回転
};

BottleDetectScene bottleDetectScenes[] =
{
    {0, Color::Yellow}, //黄ボトル検知
    {1, Color::Blue  }, //青ボトル検知
    {2, Color::Red   }  //赤ボトル検知
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