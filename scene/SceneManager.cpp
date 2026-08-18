#include "SceneManager.h"
#include "Logger.h"

namespace
{
    constexpr int BOTTLE_COLOR_SAMPLE_COUNT = 10;
    constexpr int BOTTLE_COLOR_REQUIRED_MATCH_COUNT = 6;
    constexpr int BOTTLE_COLOR_SAMPLE_INTERVAL_MS = 10;
    constexpr int MAX_SCENE_CONTROL_CYCLES = 3000;
}

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
    {12,  900, 100, RunnerEdge::RightEdge, {Color::None}, CalibrationData::BlackWhiteCenter, {0.4f, 0.0f, 0.4f} }, // Lap直線4
    {13,  180,  70, RunnerEdge::RightEdge, {Color::None}, CalibrationData::BlackWhiteCenter, {0.4f, 0.0f, 0.4f} }, // Lap減速
    {14,  800,  60, RunnerEdge::LeftEdge,  {Color::None}, CalibrationData::BlackWhiteCenter, {0.6f, 0.0f, 0.4f} }, // Dlvカーブ1
    {15,  200,  30, RunnerEdge::LeftEdge,  {Color::None}, CalibrationData::BlackWhiteCenter, {0.6f, 0.0f, 0.4f} }, // Dlvカーブ2
    {16,  100,  30, RunnerEdge::LeftEdge,  {Color::None}, CalibrationData::BlackWhiteCenter, {0.3f, 0.0f, 0.4f} }, // Dlv最初の青スルー
    {17, 1100,  70, RunnerEdge::LeftEdge,  {Color::None}, CalibrationData::BlackWhiteCenter, {0.3f, 0.0f, 0.4f} }, // Dlv直線1
    {18,  200,  30, RunnerEdge::LeftEdge,  {Color::None}, CalibrationData::BlackWhiteCenter, {0.6f, 0.0f, 0.4f} }, // Dlvカーブ3
    {19,  250,  70, RunnerEdge::LeftEdge,  {Color::None}, CalibrationData::BlackWhiteCenter, {0.3f, 0.0f, 0.4f} }, // 黄ボトル位置まで
    {20,  500,  70, RunnerEdge::LeftEdge,  {Color::None}, CalibrationData::BlackWhiteCenter, {0.3f, 0.0f, 0.4f} }, // 青ボトル位置まで
    {21,  750,  70, RunnerEdge::LeftEdge,  {Color::None}, CalibrationData::BlackWhiteCenter, {0.3f, 0.0f, 0.4f} }, // 赤ボトル位置まで
    {22,  300,  70, RunnerEdge::RightEdge, {Color::None}, CalibrationData::BlackWhiteCenter, {0.3f, 0.0f, 0.4f} }, // Dlv帰還直線 黄
    {23,  550,  70, RunnerEdge::RightEdge, {Color::None}, CalibrationData::BlackWhiteCenter, {0.3f, 0.0f, 0.4f} }, // Dlv帰還直線 青
    {24,  800,  70, RunnerEdge::RightEdge, {Color::None}, CalibrationData::BlackWhiteCenter, {0.3f, 0.0f, 0.4f} }, // Dlv帰還直線 赤
    {25,  200,  30, RunnerEdge::RightEdge, {Color::None}, CalibrationData::BlackWhiteCenter, {0.6f, 0.0f, 0.4f} }, // Dlv帰還カーブ1
    {26, 1000, 100, RunnerEdge::RightEdge, {Color::None}, CalibrationData::BlackWhiteCenter, {0.3f, 0.0f, 0.4f} }, // Dlv帰還青まで
    {27,  200,  30, RunnerEdge::RightEdge, {Color::None}, CalibrationData::BlackWhiteCenter, {0.3f, 0.0f, 0.4f} }, // Dlv青半分まで
    {28,    0,  70, RunnerEdge::RightEdge, {Color::Green, Color::Yellow, Color::Red, Color::Blue},
                                                          CalibrationData::BlackWhiteCenter, {0.3f, 0.0f, 0.4f} }, // Rly右エッジ基準点まで
    {29,    0,  70, RunnerEdge::LeftEdge,  {Color::Green, Color::Yellow, Color::Red, Color::Blue},
                                                          CalibrationData::BlackWhiteCenter, {0.3f, 0.0f, 0.4f} }, // Rly左エッジ基準点まで
};

const MoveScene moveScenes[] =
{
    { 0, Direction::front, {50.0f, 100.0f,  50.0f, 100.0f}, 100, {Color::None}, {1.0f, 0.0f, 0.0f}}, // Dlvエリアまで
    { 1, Direction::back,  {50.0f, 100.0f,  50.0f, 200.0f}, 200, {Color::None}, {1.0f, 0.0f, 0.0f}}, // Dlv線まで帰還
    { 2, Direction::front, {50.0f,  70.0f,  50.0f,  10.0f},  10, {Color::None}, {1.0f, 0.0f, 0.0f}}, // Dlvエリアまで
    { 3, Direction::front, {50.0f,  70.0f,  50.0f,  30.0f},  30, {Color::None}, {1.0f, 0.0f, 0.0f}},
    { 4, Direction::front, {50.0f, 100.0f,  50.0f, 250.0f}, 250, {Color::None}, {2.0f, 0.0f, 0.0f}}, // Rlyゲート前1
    { 5, Direction::front, {50.0f, 100.0f,  50.0f, 500.0f}, 500, {Color::None}, {2.0f, 0.0f, 0.0f}}, // Rlyゲート前2
    { 6, Direction::front, {50.0f, 100.0f,  50.0f, 750.0f}, 750, {Color::None}, {2.0f, 0.0f, 0.0f}}, // Rlyゲート前3
    { 7, Direction::front, {50.0f, 100.0f,  50.0f,1000.0f},1000, {Color::None}, {2.0f, 0.0f, 0.0f}}, // Rlyゲート前4
    { 8, Direction::front, {50.0f, 100.0f,  50.0f,1250.0f},1250, {Color::None}, {2.0f, 0.0f, 0.0f}}, // Rlyゲート前5
    { 9, Direction::front, {50.0f, 100.0f,  50.0f, 200.0f}, 200, {Color::None}, {1.0f, 0.0f, 0.0f}}, // Rlyゲート通過
    {10, Direction::front, {50.0f,  50.0f,  50.0f,   0.0f},   0, {Color::Green, Color::Yellow, Color::Red, Color::Blue},
                                                                                {2.0f, 0.0f, 0.0f}}, //基準点帰還
};

const TurnScene turnScenes[] =
{
    {0,  90, {1.0f, 0.0f, 0.0f}}, //右に90°回転
    {1, -15, {1.0f, 0.0f, 0.0f}},  //左に30°回転
    {2, -90, {1.0f, 0.0f, 0.0f}}, //左に90°回転
    {3,  30, {1.0f, 0.0f, 0.0f}},  //右に30°回転
    {4, 180, {1.0f, 0.0f, 0.0f}},  //180°回転
};

const ColorDetectScene colorDetectScenes[] =
{
    {0, {Color::Yellow}}, //黄検知
    {1, {Color::Blue}  }, //青検知
    {2, {Color::Red}   }, //赤検知
    {3, {Color::Green} }, //緑検知
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
    mEventDetector = nullptr;
    setParameter();

    mDistanceCalculator.reset();
    mPIDCalculator.reset();

    if (mActionType == ActionType::ColorDetect)
    {
        return mTargetColorDetector.judgeMultiple(
            BOTTLE_COLOR_SAMPLE_COUNT,
            BOTTLE_COLOR_REQUIRED_MATCH_COUNT,
            BOTTLE_COLOR_SAMPLE_INTERVAL_MS);
    }

    if (mActionType == ActionType::Stop)
    {
        mGyroTraceRunner.stop();
        return true;
    }

    if (mEventDetector == nullptr)
    {
        Logger::printf("Event detector is not configured. SceneID=%d\r\n", mSceneId);
        mGyroTraceRunner.stop();
        return false;
    }
    
    int controlCycleCount = 0;
    while(!mEventDetector->judge())
    {
        if (controlCycleCount >= MAX_SCENE_CONTROL_CYCLES)
        {
            Logger::printf("Scene timeout. SceneID=%d\r\n", mSceneId);
            mGyroTraceRunner.stop();
            return false;
        }

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
        controlCycleCount++;
    }

    // シーン終了
    return true;
}

void SceneManager::setParameter()
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
    case ActionType::ColorDetect:
    {
        const ColorDetectScene& colorDetectScene = colorDetectScenes[mSceneId];
    
        mTargetColorDetector.setTargetColors(colorDetectScene.detectColor);
        mEventDetector = &mTargetColorDetector;

        break;
    }
    default:
        break;
    }
}
