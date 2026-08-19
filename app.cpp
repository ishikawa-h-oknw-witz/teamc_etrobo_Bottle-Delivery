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
//バッテリー
#include "Battery.h"

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

Battery battery;
/* インスタンス生成ここまで */

namespace
{
constexpr int RIGHT_EDGE_INDEX = 0;
constexpr int LEFT_EDGE_INDEX = 1;
constexpr int TURN_AROUND_INDEX = 2;
constexpr int MAX_POINT_SEARCH_COUNT = 20;

int getPointOrder(Color color)
{
    switch (color)
    {
    case Color::Green:
        return 0;
    case Color::Yellow:
        return 1;
    case Color::Red:
        return 2;
    case Color::Blue:
        return 3;
    default:
        return -1;
    }
}

bool getNextEdgeIndex(Color currentColor, Color nextColor, int& edgeIndex)
{
    const int currentOrder = getPointOrder(currentColor);
    const int nextOrder = getPointOrder(nextColor);

    if (currentOrder < 0 || nextOrder < 0 || currentOrder == nextOrder)
    {
        return false;
    }

    // 基準点の並びは下から緑・黄・赤・青。
    // 上方向へ進む場合は右エッジ、下方向へ進む場合は左エッジを使用する。
    edgeIndex = nextOrder > currentOrder
        ? RIGHT_EDGE_INDEX
        : LEFT_EDGE_INDEX;
    return true;
}
}

struct GatePosition
{
    Color pointColor;
    int gatePositionNum;
};

struct SceneOrder
{
    int sceneNum;
    int sceneId;
    ActionType actionType;
};

const GatePosition gatePositions[] =
{
    {Color::Yellow,  3}, // 黄色地点にある赤ゲート
    {Color::Red,     9}, // 赤色地点にある青ゲート
    {Color::Green,  13}, // 緑色地点にある黄ゲート
};

const SceneOrder LAP[] =
{
    { 0,  0, ActionType::LineTrace}, // Lap直線1
    { 1,  1, ActionType::LineTrace}, // Lapカーブ1-1
    { 2,  2, ActionType::LineTrace}, // Lapカーブ1-2
    { 3,  3, ActionType::LineTrace}, // Lapカーブ1-3
    { 4,  4, ActionType::LineTrace}, // Lap直線2
    { 5,  5, ActionType::LineTrace}, // Lapカーブ2-1
    { 6,  6, ActionType::LineTrace}, // Lapカーブ2-2
    { 7,  7, ActionType::LineTrace}, // Lapカーブ2-3
    { 8,  8, ActionType::LineTrace}, // Lap直線3
    { 9,  9, ActionType::LineTrace}, // Lapカーブ3
    {10, 10, ActionType::LineTrace}, // Lap蛇行1
    {11, 11, ActionType::LineTrace}, // Lap蛇行2
    {12, 12, ActionType::LineTrace}, // Lap直線4
    {13, 13, ActionType::LineTrace}, // Lap減速
    {14,  0, ActionType::Stop},      // 停止
};

const SceneOrder EnterBottle[] =
{
    {0, 2, ActionType::Move}, //ボトル前まで移動
    {1, 0, ActionType::Stop}  //回数確認用
};

const SceneOrder DetectBottleColor[] =
{
    {0, 0, ActionType::ColorDetect}, //黄ボトル検知
    {1, 1, ActionType::ColorDetect}, //青ボトル検知
    {2, 2, ActionType::ColorDetect}  //赤ボトル検知
};

const SceneOrder EnterZone[] =
{
    {0,  1, ActionType::Turn}, // 角度調整
    {1, 14, ActionType::LineTrace}, // Dlvカーブ1
    {2, 15, ActionType::LineTrace}, // Dlvカーブ2
    {3, 16, ActionType::LineTrace}, // Dlv行き青スルー
    {4, 17, ActionType::LineTrace}, // Dlv直線1
    {5, 18, ActionType::LineTrace}, // Dlvカーブ3
};

const SceneOrder MoveZone[] =
{
    {0, 19, ActionType::LineTrace},  // 黄エリア前まで
    {0, 20, ActionType::LineTrace},  // 青エリア前まで
    {0, 21, ActionType::LineTrace},  // 赤エリア前まで
};

const SceneOrder CarryZone[] =
{
    {0,  3, ActionType::Turn},     // 右に90°回転
    {1,  3, ActionType::Move},
    {2,  3, ActionType::Turn},     // 右に90°回転
    {3,  3, ActionType::Move},
    {4,  3, ActionType::Turn},     // 右に90°回転
    {5,  0, ActionType::Move},     // Dlvエリアまで
    {6,  1, ActionType::Move},     // Dlv線まで帰還
    {7,  0, ActionType::Turn},     // 右に90°回転
};

const SceneOrder ReturnZone[] =
{
    {0, 22, ActionType::LineTrace},  // Dlv黄から行きゲート前まで
    {0, 23, ActionType::LineTrace},  // Dlv青から行きゲート前まで
    {0, 24, ActionType::LineTrace},  // Dlv黄から行きゲート前まで
};

const SceneOrder EnterRally[] =
{
    {0, 25, ActionType::LineTrace}, // Dlv帰還カーブ1
    {1, 26, ActionType::LineTrace}, // Dlv帰還青まで
    {2, 27, ActionType::LineTrace}, // Dlv青半分まで
    {3,  0, ActionType::Turn},      // Dlv右に90°回転
    {4,  0, ActionType::Move},      // Dlv基準線まで
    {5,  0, ActionType::Stop}
};

/* ログタスク */
/*
void logger_task(intptr_t exinf)
{
    logger.output();
    ext_tsk();
}
*/

//シーン実行&遷移
bool change_scene(const SceneOrder sceneOrder[], int maxSceneNum)
{
    int sceneNum = 0;

    while (true)
    {
        const SceneOrder& sceneOrderItem = sceneOrder[sceneNum];

        sceneManager.setActionType(sceneorder.actionType);
        sceneManager.setSceneID(sceneorder.sceneId);
        Logger::printf("[app]SceneID=%d\n", sceneorder.sceneId);
        if(sceneManager.SceneExecute())
        {
            Logger::printf("Scene execution failed. ID=%d\r\n", sceneOrderItem.sceneId);
            gyroTraceRunner.stop();
            return false;
        }

        sceneNum++;

        if (sceneNum > maxSceneNum)
        {
            break;
        }
    }

    return true;
}

/* メインタスク */
void main_task(intptr_t exinf)
{
    /* Bluetooth初期化＆接続待ち＆ログタスク起動100msec周期 */
    logger.init();
    Logger::printf("[app]接続完了\n");
    Logger::printf("[app]出力電圧:%d\n",battery.getVoltage());
    Logger::printf("[app]出力電流:%d\n",battery.getCurrent());
    //sta_cyc(LOGGER_TASK_CYC);

    armController.moveArmDown();

    // 1回目の押下
    /*キャリブレーション用
    while (!forceSensor.isTouched());
    tslp_tsk(20 * 1000);
    while (forceSensor.isTouched());

    lineTraceRunner.calibrateTargetReflection(0);
    Logger::printf("キャリブレーション１完了\n");

    // 2回目の押下
    while (!forceSensor.isTouched());
    tslp_tsk(20 * 1000);
    while (forceSensor.isTouched());

    lineTraceRunner.calibrateTargetReflection(1);
    Logger::printf("キャリブレーション２完了\n");
    */

    // 3回目の押下（スタート）
    while (!forceSensor.isTouched());
    tslp_tsk(20 * 1000);
    while (forceSensor.isTouched());
    Logger::printf("[app]スタート\n");

    //メインループ10msec周期

    Logger::printf("[app]ラップ開始\n");
    //LAP
    change_scene(LAP, 14);

    tslp_tsk(100*1000);
    
    armController.moveArmUp();

    // アーム上昇直後の振動が収まるまで待つ
    tslp_tsk(200*1000);

    //ボトルまで動く
    change_scene(EnterBottle, 1);

    //ボトル色検知
    const char* colorName[] = {"黄", "青", "赤"};

    for (int SceneNum = 0; SceneNum < 3; SceneNum++)
    {
        const SceneOrder& detectbottlecolor = DetectBottleColor[SceneNum];

        sceneManager.setActionType(detectbottlecolor.actionType);
        sceneManager.setSceneID(detectbottlecolor.sceneId);
        Logger::printf("[app]SceneID=%d\n", detectbottlecolor.sceneId);
        if(sceneManager.SceneExecute())
        {
            skipCount = SceneNum;
            Logger::printf("[app]色検知:%s\n", colorName[SceneNum]);
            break;
        }
    }

    if (skipCount < 0)
    {
        tslp_tsk(200*1000);
        Logger::printf("[app]ボトル検知失敗\n");
        change_scene(EnterBottle, 1);
    }

    armController.moveArmDown();

    //Dlvカープ3まで
    change_scene(EnterZone, 5);

    //任意のエリア前まで移動
    change_scene(&MoveZone[skipCount], 0);

    //ボトル設置
    change_scene(CarryZone, 7);

    //カーブに向かう
    change_scene(&ReturnZone[skipCount], 0);

    //ラリーへ向かう
    change_scene(EnterRally, 5);

    int nowEdgeIndex = RIGHT_EDGE_INDEX;
    const int gatePositionCount =
        static_cast<int>(sizeof(gatePositions) / sizeof(gatePositions[0]));

    Logger::printf("[app]終了\n");

    ext_tsk(); 
}
