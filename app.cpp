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
TargetAngleDetector targetAngleDetector;
TargetColorDetector targetColorDetector(colorDetector);
SceneManager sceneManager(lineTraceRunner, gyroTraceRunner, pidCalculator, trapezoidCalculator, distanceCalculator, targetDistanceDetector, targetAngleDetector, targetColorDetector);

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
    {12, 12,  ActionType::LineTrace}, // Lap直線4
    {13,  0,  ActionType::Stop},      // 停止
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
    {0,  0, ActionType::Turn},     // 右に90°回転
    {1,  0, ActionType::Move},     // Dlvエリアまで
    {2,  1, ActionType::Move},     // Dlv線まで帰還
    {3,  0, ActionType::Turn},     // 右に90°回転
    {4, 19, ActionType::LineTrace} // Dlv帰り青スルー
};

SceneOrder ReturnZone[] =
{
    {0, 21, ActionType::LineTrace}, // Dlv帰り青スルー
    {1, 22, ActionType::LineTrace}  // Dlv帰還青まで
};

SceneOrder EnterRally[] =
{
    {0, 20, ActionType::LineTrace}, // Dlv帰還直線1
    {1, 21, ActionType::LineTrace}, // Dlv帰還カーブ1
    {2, 22, ActionType::LineTrace}, // Dlv帰還青まで
    {3, 23, ActionType::LineTrace}, // Dlv青半分まで
    {4,  0, ActionType::Turn},      // Dlv右に90°回転
    {5,  0, ActionType::Move}       // Dlv基準線まで
};

/* ログタスク */
void logger_task(intptr_t exinf)
{
    logger.output();
    ext_tsk();
}

//シーン実行&遷移
void change_scene(SceneOrder sceneOrder[], int MaxSceneNum)
{
    int SceneNum = 0;

    while (true)
    {
        const SceneOrder& sceneorder = sceneOrder[SceneNum];

        sceneManager.setActionType(sceneorder.actionType);
        sceneManager.setSceneID(sceneorder.sceneId);
        Logger::printf("SceneID=%d", sceneorder.sceneId);
        if(sceneManager.SceneExecute())
        {
            SceneNum++;
        }

        if (SceneNum > MaxSceneNum)
        {
            break;
        }
    }
}

/* メインタスク */
void main_task(intptr_t exinf)
{
    /* Bluetooth初期化＆接続待ち＆ログタスク起動100msec周期 */
    logger.init();
    sta_cyc(LOGGER_TASK_CYC);

    armController.moveArmDown();

    // 1回目の押下
    while (!forceSensor.isTouched());
    tslp_tsk(20 * 1000);
    while (forceSensor.isTouched());

    lineTraceRunner.calibrateTargetReflection(0);

    // 2回目の押下
    while (!forceSensor.isTouched());
    tslp_tsk(20 * 1000);
    while (forceSensor.isTouched());

    lineTraceRunner.calibrateTargetReflection(1);

    // 3回目の押下（スタート）
    while (!forceSensor.isTouched());
    tslp_tsk(20 * 1000);
    while (forceSensor.isTouched());

    int skipCount = 0;

    //メインループ10msec周期

    //LAP
    change_scene(LAP, 13);

    tslp_tsk(100*1000);
    
    armController.moveArmUp();

    //ボトル色検知

    const char* colorName[] = {"黄", "青", "赤"};

    for (int SceneNum = 0; SceneNum < 3; SceneNum++)
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
    change_scene(EnterZone, 5);

    //青スキップ
    for (int skip = 0; skip < skipCount; skip++)
    {
        change_scene(MoveZone, 1);
    }

    //ボトル設置
    change_scene(CarryZone, 4);

    //帰還時青スキップ
    for (int skip = 0; skip < skipCount; skip++)
    {
        change_scene(ReturnZone, 1);
    }

    //ラリーへ
    change_scene(EnterRally, 5);

    Logger::printf("終了");

    ext_tsk(); 
}