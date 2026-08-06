//攻略パッケージ
#include "app.h"
//シーンパッケージ
#include "SceneManager.h"
//制御パッケージ            
#include "LineTraceRunner.h"
#include "GyroTraceRunner.h"
#include "ArmController.h"
//演算パッケージ
#include "PIDCalculator.h"
#include "TrapezoidCalculator.h"
#include "DistanceCalculator.h"
//デバイスパッケージ
#include "Motor.h"
#include "ForceSensor.h" 
#include "ColorSensor.h"
//ログ用
#include "Logger.h"
//タスク系
#include "kernel.h"   
#include "kernel_cfg.h"

using namespace spikeapi;

/* インスタンス生成 */
Motor leftWheel(EPort::PORT_B,Motor::EDirection::COUNTERCLOCKWISE,true);
Motor rightWheel(EPort::PORT_A,Motor::EDirection::CLOCKWISE,true);
Motor ArmMotor(EPort::PORT_C,Motor::EDirection::COUNTERCLOCKWISE,true);
ForceSensor forceSensor(EPort::PORT_D);
ColorSensor colorSensor(EPort::PORT_E);

PIDCalculator pidCalculator;
DistanceCalculator distanceCalculator(leftWheel, rightWheel);
TrapezoidCalculator trapezoidCalculator(distanceCalculator);

LineTraceRunner lineTraceRunner(leftWheel, rightWheel, colorSensor, pidCalculator);
GyroTraceRunner gyroTraceRunner(leftWheel, rightWheel, distanceCalculator, pidCalculator, trapezoidCalculator);
ArmController armController(ArmMotor);

ColorDetector colorDetector(colorSensor);
TargetDistanceDetector targetDistanceDetector(distanceCalculator);
TargetColorDetector targetColorDetector(colorDetector);
SceneManager sceneManager(lineTraceRunner, gyroTraceRunner, pidCalculator, trapezoidCalculator, distanceCalculator, targetDistanceDetector, targetColorDetector);

Logger logger(colorSensor, leftWheel, rightWheel);
/* インスタンス生成ここまで */

struct SceneOrder
{
    int sceneNum;
    int sceneId;
    ActionType actionType;
};

SceneOrder LAP[] =
{
    { 0,  0,  ActionType::LineTrace}, // Lap直線1
    { 1,  1,  ActionType::LineTrace}, // Lapカーブ1-1
    { 2,  2,  ActionType::LineTrace}, // Lapカーブ1-2
    { 3,  3,  ActionType::LineTrace}, // Lapカーブ1-3
    { 4,  4,  ActionType::LineTrace}, // Lap直線2
    { 5,  5,  ActionType::LineTrace}, // Lapカーブ2-1
    { 6,  6,  ActionType::LineTrace}, // Lapカーブ2-2
    { 7,  7,  ActionType::LineTrace}, // Lapカーブ2-3
    { 8,  8,  ActionType::LineTrace}, // Lap直線3
    { 9,  9,  ActionType::LineTrace}, // Lapカーブ3
    {10, 10,  ActionType::LineTrace}, // Lap蛇行1
    {11, 11,  ActionType::LineTrace}, // Lap蛇行2
    {12, 12,  ActionType::LineTrace}  // Lap直線4
};

SceneOrder DetectBottleColor[] =
{
    {0, 0, ActionType::BottoleDetect}, //黄ボトル検知
    {1, 1, ActionType::BottoleDetect}, //青ボトル検知
    {2, 2, ActionType::BottoleDetect}  //赤ボトル検知

};

SceneOrder EnterZone[] =
{
    {0, 13, ActionType::LineTrace}, // Dlvカーブ1
    {1, 14, ActionType::LineTrace}, // Dlvカーブ2
    {2, 15, ActionType::LineTrace}, // Dlv行き青スルー
    {3, 16, ActionType::LineTrace}, // Dlv直線1
    {4, 17, ActionType::LineTrace}, // Dlvカーブ3
    {5, 18, ActionType::LineTrace}  // Dlv青まで
};

SceneOrder MoveZone[] =
{
    {0, 15, ActionType::LineTrace}, // Dlv行き青スルー
    {1, 18, ActionType::LineTrace}  // Dlv行き青まで
};

SceneOrder CarryZone[] =
{
    {0,  0, ActionType::Turn},     //右に90°回転
    {1,  0, ActionType::Move},     // Dlvエリアまで
    {2,  1, ActionType::Move},     // Dlv線まで帰還
    {3,  0, ActionType::Turn},     //右に90°回転
    {4, 20, ActionType::LineTrace} // Dlv帰り青スルー
};

SceneOrder ReturnZone[] =
{
    {0, 20, ActionType::LineTrace}, // Dlv帰り青スルー
    {1, 23, ActionType::LineTrace}  // Dlv帰還青まで
};

SceneOrder EnterRally[] =
{
    {0, 21, ActionType::LineTrace}, // Dlv帰還直線1
    {1, 22, ActionType::LineTrace}, // Dlv帰還カーブ1
    {2, 23, ActionType::LineTrace}  // Dlv帰還青まで
};

/* ログタスク */
void logger_task(intptr_t exinf)
{
    logger.output();
    ext_tsk();
}

/* メインタスク */
void main_task(intptr_t exinf)
{
    /* Bluetooth初期化＆接続待ち＆ログタスク起動100msec周期 */
    logger.init();
    sta_cyc(LOGGER_TASK_CYC);

    //フォースセンサボタン押下待ち
    while (!forceSensor.isTouched());

    int SceneNum = 0;
    int skipCount = 0;
    armController.moveArmDown();

    //メインループ10msec周期

    //LAP
    while(true)
    {
        const SceneOrder& lap = LAP[SceneNum];

        sceneManager.setActionType(lap.actionType);
        sceneManager.setSceneID(lap.sceneId);
        Logger::printf("SceneID=%d", lap.sceneId);
        if(sceneManager.SceneExecute())
        {
            SceneNum++;
        }

        if (SceneNum > 12)
        {
            break;
        }
    }

    leftWheel.stop();
    rightWheel.stop();

    //ボトル色検知
    armController.moveArmUp();

    const char* colorName[] = {"黄", "青", "赤"};

    for (SceneNum = 0; SceneNum < 3; SceneNum++)
    {
        const SceneOrder& detectbottlecolor = DetectBottleColor[SceneNum];

        sceneManager.setActionType(detectbottlecolor.actionType);
        sceneManager.setSceneID(detectbottlecolor.sceneId);
        Logger::printf("SceneID=%d", detectbottlecolor.sceneId);
        if(sceneManager.SceneExecute())
        {
            skipCount = SceneNum;
            Logger::printf("%s", colorName[SceneNum]);
            break;
        }
    }

    armController.moveArmDown();

    //ゾーン手前青まで
    SceneNum = 0;

    while (true)
    {
        const SceneOrder& enterzone = EnterZone[SceneNum];

        sceneManager.setActionType(enterzone.actionType);
        sceneManager.setSceneID(enterzone.sceneId);
        Logger::printf("SceneID=%d", enterzone.sceneId);
        if(sceneManager.SceneExecute())
        {
            SceneNum++;
        }

        if (SceneNum > 5)
        {
            break;
        }
    }

    //青スキップ
    SceneNum = 0;

    for (int skip = 0; skip < skipCount; skip++)
    {
        while (true)
        {
            const SceneOrder& movezone = MoveZone[SceneNum];

            sceneManager.setActionType(movezone.actionType);
            sceneManager.setSceneID(movezone.sceneId);
            Logger::printf("SceneID=%d", movezone.sceneId);
            if(sceneManager.SceneExecute())
            {
                SceneNum++;
            }

            if (SceneNum > 1)
            {
                break;
            }
        }
    }

    //ボトル設置
    SceneNum = 0;

    while (true)
    {
        const SceneOrder& carryzone = CarryZone[SceneNum];

        sceneManager.setActionType(carryzone.actionType);
        sceneManager.setSceneID(carryzone.sceneId);
        Logger::printf("SceneID=%d", carryzone.sceneId);
        if(sceneManager.SceneExecute())
        {
            SceneNum++;
        }

        if (SceneNum > 4)
        {
            break;
        }
    }

    //帰還時青スキップ
    SceneNum = 0;

    for (int skip = 0; skip < skipCount; skip++)
    {
        while (true)
        {
            const SceneOrder& returnzone = ReturnZone[SceneNum];

            sceneManager.setActionType(returnzone.actionType);
            sceneManager.setSceneID(returnzone.sceneId);
            Logger::printf("SceneID=%d", returnzone.sceneId);
            if(sceneManager.SceneExecute())
            {
                SceneNum++;
            }

            if (SceneNum > 1)
            {
                break;
            }
        }
    }

    //ラリーへ
    SceneNum = 0;

    while (true)
    {
        const SceneOrder& enterrally = CarryZone[SceneNum];

        sceneManager.setActionType(enterrally.actionType);
        sceneManager.setSceneID(enterrally.sceneId);
        Logger::printf("SceneID=%d", enterrally.sceneId);
        if(sceneManager.SceneExecute())
        {
            SceneNum++;
        }

        if (SceneNum > 2)
        {
            break;
        }
    }
    Logger::printf("終了");

    ext_tsk(); 
}


