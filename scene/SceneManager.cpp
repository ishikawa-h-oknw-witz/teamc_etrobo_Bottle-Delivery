#include "SceneManager.h"
#include "Logger.h"

// {シーンID, 目標距離, 速度, 走行エッジ, 終了色, 目標輝度, {Kp, Ki, Kd}}
const LineTraceScene lineTraceScenes[] =
{
    { 0,  500, 100, RunnerEdge::RightEdge, {Color::None}, CalibrationData::BlackWhiteCenter, {0.2f, 0.0f, 0.2f} }, // Lap直線1
    { 1,  150,  80, RunnerEdge::RightEdge, {Color::None}, CalibrationData::BlackWhiteCenter, {0.6f, 0.0f, 0.4f} }, // Lapカーブ1-1
    { 2,  100,  70, RunnerEdge::RightEdge, {Color::None}, CalibrationData::BlackWhiteCenter, {0.5f, 0.0f, 0.4f} }, // Lapカーブ1-2
    { 3,  150,  80, RunnerEdge::RightEdge, {Color::None}, CalibrationData::BlackWhiteCenter, {0.6f, 0.0f, 0.4f} }, // Lapカーブ1-3
    { 4,  400, 100, RunnerEdge::RightEdge, {Color::None}, CalibrationData::BlackWhiteCenter, {0.3f, 0.0f, 0.4f} }, // Lap直線2
    { 5,  150,  80, RunnerEdge::RightEdge, {Color::None}, CalibrationData::BlackWhiteCenter, {0.6f, 0.0f, 0.4f} }, // Lapカーブ2-1
    { 6,  100,  70, RunnerEdge::RightEdge, {Color::None}, CalibrationData::BlackWhiteCenter, {0.5f, 0.0f, 0.4f} }, // Lapカーブ2-2
    { 7,  100,  80, RunnerEdge::RightEdge, {Color::None}, CalibrationData::BlackWhiteCenter, {0.6f, 0.0f, 0.4f} }, // Lapカーブ2-3
    { 8,  300, 100, RunnerEdge::RightEdge, {Color::None}, CalibrationData::BlackWhiteCenter, {0.3f, 0.0f, 0.4f} }, // Lap直線3
    { 9,  400,  60, RunnerEdge::RightEdge, {Color::None}, CalibrationData::BlackWhiteCenter, {0.5f, 0.0f, 0.4f} }, // Lapカーブ3
    {10,  900, 100, RunnerEdge::RightEdge, {Color::None}, CalibrationData::BlackWhiteCenter, {0.6f, 0.0f, 0.4f} }, // Lap蛇行1
    {11,  900,  80, RunnerEdge::RightEdge, {Color::None}, CalibrationData::BlackWhiteCenter, {0.5f, 0.0f, 0.4f} }, // Lap蛇行2
    {12,  920, 100, RunnerEdge::RightEdge, {Color::None}, CalibrationData::BlackWhiteCenter, {0.4f, 0.0f, 0.4f} }, // Lap直線4
    {13,  200,  70, RunnerEdge::RightEdge, {Color::None}, CalibrationData::BlackWhiteCenter, {0.4f, 0.0f, 0.4f} }, // Lap減速
    {14,  800,  60, RunnerEdge::LeftEdge,  {Color::None}, CalibrationData::BlackWhiteCenter, {0.6f, 0.0f, 0.4f} }, // Dlvカーブ1
    {15,  130,  30, RunnerEdge::LeftEdge,  {Color::None}, CalibrationData::BlackWhiteCenter, {0.6f, 0.0f, 0.4f} }, // Dlvカーブ2
    {16,  100,  30, RunnerEdge::LeftEdge,  {Color::None}, CalibrationData::BlackWhiteCenter, {0.3f, 0.0f, 0.4f} }, // Dlv最初の青スルー
    {17, 1100,  70, RunnerEdge::LeftEdge,  {Color::None}, CalibrationData::BlackWhiteCenter, {0.3f, 0.0f, 0.4f} }, // Dlv直線1
    {18,  200,  30, RunnerEdge::LeftEdge,  {Color::None}, CalibrationData::BlackWhiteCenter, {0.6f, 0.0f, 0.4f} }, // Dlvカーブ3
    {19,  100,  70, RunnerEdge::LeftEdge,  {Color::None}, CalibrationData::BlackWhiteCenter, {0.3f, 0.0f, 0.4f} }, // Dlvゲート前まで
    {20,  100,  70, RunnerEdge::RightEdge, {Color::None}, CalibrationData::BlackWhiteCenter, {0.3f, 0.0f, 0.4f} }, // Dlv帰還青通過まで
    {21,  400, 100, RunnerEdge::RightEdge, {Color::None}, CalibrationData::BlackWhiteCenter, {0.3f, 0.0f, 0.4f} }, // Dlv帰還直線1
    {22,  200,  30, RunnerEdge::RightEdge, {Color::None}, CalibrationData::BlackWhiteCenter, {0.6f, 0.0f, 0.4f} }, // Dlv帰還カーブ1
    {23, 1000, 100, RunnerEdge::RightEdge, {Color::None}, CalibrationData::BlackWhiteCenter, {0.3f, 0.0f, 0.4f} }, // Dlv帰還青まで
    {24,   50,  30, RunnerEdge::RightEdge, {Color::None}, CalibrationData::BlackWhiteCenter, {0.3f, 0.0f, 0.4f} }  // Dlv青半分まで
};

const MoveScene moveScenes[] =
{
    {0, Direction::front, {50.0f, 100.0f,  50.0f, 200.0f}, 200, {Color::None}, {1.0f, 0.0f, 0.0f}}, // Dlvエリアまで
    {1, Direction::back,  {50.0f, 100.0f,  50.0f, 200.0f}, 200, {Color::None}, {1.0f, 0.0f, 0.0f}}  // Dlv線まで帰還
};

const TurnScene turnScenes[] =
{
    {0,  90, {1.0f, 0.0f, 0.0f}}, //右に90°回転
    {1, -30, {1.0f, 0.0f, 0.0f}},  //左に30°回転
    {2, -90, {1.0f, 0.0f, 0.0f}}, //左に90°回転
};

const BottleDetectScene bottleDetectScenes[] =
{
    {0, {Color::Yellow}}, //黄ボトル検知
    {1, {Color::Blue}  }, //青ボトル検知
    {2, {Color::Red}   }  //赤ボトル検知
};

//コンストラクタ
SceneManager::SceneManager(
    LineTraceRunner& lineTraceRunner,
    GyroTraceRunner& gyroTraceRunner,
    PIDCalculator& pidCalculator,
    TrapezoidCalculator& trapezoidCalculator,
    DistanceCalculator& distanceCalculator,
    TargetDistanceDetector& targetDistanceDetector,
    TargetAngleDetector& targetAngleDetector,
    TargetColorDetector& targetColorDetector
    )
    : mLineTraceRunner(lineTraceRunner),
      mGyroTraceRunner(gyroTraceRunner),
      mPIDCalculator(pidCalculator),
      mTrapezoidCalculator(trapezoidCalculator),
      mDistanceCalculator(distanceCalculator),
      mTargetDistanceDetector(targetDistanceDetector),
      mTargetAngleDetector(targetAngleDetector),
      mTargetColorDetector(targetColorDetector),
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
    mImu.resetHeading();
    setParameter(mSceneId);

    mDistanceCalculator.reset();

    if (mActionType == ActionType::BottoleDetect)
    {
        return mEventDetector->judge();
    }

    if (mActionType == ActionType::Stop)
    {
        mGyroTraceRunner.stop();
        return true;
    }
    
    while(!mEventDetector->judge())
    {
        // 走行実行
        switch (mActionType)
        {
        case ActionType::LineTrace:
            mLineTraceRunner.run();
            break;

        case ActionType::Move:
            mGyroTraceRunner.move();
            break;

        case ActionType::Turn:
            mGyroTraceRunner.turn();
            break;

        default:
            break;
        }
        tslp_tsk(10*1000);
    }

    // シーン終了
    return true;
}

void SceneManager::setParameter(int sceneId)
{

    switch(mActionType)
    {
    case ActionType::LineTrace:
    {
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

        // 目標輝度
        if (linetracescene.targetSensorValue == CalibrationData::BlackWhiteCenter)
        {
            mLineTraceRunner.setTargetSensorValue(
                mLineTraceRunner.getTargetSensorValue(0));
        }
        else
        {
            mLineTraceRunner.setTargetSensorValue(
                mLineTraceRunner.getTargetSensorValue(1));
        }

        // 走行距離
        if (linetracescene.targetDistance != 0)
        {
            mTargetDistanceDetector.setTargetDistance(linetracescene.targetDistance);
            mEventDetector = &mTargetDistanceDetector;
        }

        //判定色
        if (linetracescene.finishColor[0] != Color::None)
        {
            mTargetColorDetector.setTargetColors(linetracescene.finishColor);
            mEventDetector = &mTargetColorDetector;
        }

        break;
    }
    case ActionType::Move:
    {
        const MoveScene& movescene = moveScenes[mSceneId];

        //向き
        mGyroTraceRunner.setDirection(movescene.direction);

        //台形計算
        mTrapezoidCalculator.setParameter(movescene.trapezoidParameter);

        //PID
        mPIDCalculator.setGain(
            movescene.pid.kp,
            movescene.pid.ki,
            movescene.pid.kd);

        //走行距離
        if (movescene.targetDistance != 0)
        {
            mTargetDistanceDetector.setTargetDistance(movescene.targetDistance);
            mEventDetector = &mTargetDistanceDetector;
        }

        //判定色
        if (movescene.finishColor[0] != Color::None)
        {
            mTargetColorDetector.setTargetColors(movescene.finishColor);
            mEventDetector = &mTargetColorDetector;
        }

        break;
    }
    case ActionType::Turn:
    {
        const TurnScene& turnscene = turnScenes[mSceneId];

        //PID
        mPIDCalculator.setGain(
            turnscene.pid.kp,
            turnscene.pid.ki,
            turnscene.pid.kd);
        
        if (turnscene.targetAngle != 0)
        {
            mGyroTraceRunner.setTargetAngle(turnscene.targetAngle);
            mTargetAngleDetector.setTargetAngle(turnscene.targetAngle);
            mEventDetector = &mTargetAngleDetector;
        }

        break;
    }
    case ActionType::BottoleDetect:
    {
        const BottleDetectScene& bottledetectscene = bottleDetectScenes[mSceneId];
    
        mTargetColorDetector.setTargetColors(bottledetectscene.detectColor);
        mEventDetector = &mTargetColorDetector;

        break;
    }
    default:
        break;
    }
}
