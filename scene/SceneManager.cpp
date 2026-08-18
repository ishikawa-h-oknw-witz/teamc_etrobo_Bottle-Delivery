#include "SceneManager.h"
#include "RobotParameter.h"
#include "Logger.h"

namespace
{
    constexpr int BOTTLE_COLOR_SAMPLE_COUNT = 10;
    constexpr int BOTTLE_COLOR_REQUIRED_MATCH_COUNT = 6;
    constexpr int BOTTLE_COLOR_SAMPLE_INTERVAL_MS = 10;
}

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
    setParameter();

    mDistanceCalculator.reset();

    if (mActionType == ActionType::BottoleDetect)
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
        /*
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
        */

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
