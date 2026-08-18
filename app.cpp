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

namespace
{
constexpr int RIGHT_EDGE_INDEX = 0;
constexpr int LEFT_EDGE_INDEX = 1;
constexpr int TURN_AROUND_INDEX = 2;
constexpr int MAX_POINT_SEARCH_COUNT = 20;
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
    {Color::Red,     7}, // 赤色地点にある青ゲート
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

// 次の色地点までライントレースする。
const SceneOrder EnterPoint[] =
{
    {0, 28, ActionType::LineTrace}, // 右エッジで次の色地点まで
    {0, 29, ActionType::LineTrace}, // 左エッジで次の色地点まで
};

const SceneOrder MovePointCenter[] =
{
    {0, 3, ActionType::Move}, // 目標の基準点中央まで50mm進む
};

const SceneOrder PassPoint[] =
{
    {0, 11, ActionType::Move}, // 目標外の基準点を100mmで通り抜ける
};

const SceneOrder GateTurn[] =
{
    {0, 0, ActionType::Turn}, // 右90°
    {1, 2, ActionType::Turn}, // 左90°
    {2, 4, ActionType::Turn}, // 180°
};

const SceneOrder RejoinTurn[] =
{
    {0, 5, ActionType::Turn}, // 次の基準点方向へ右45°
    {1, 6, ActionType::Turn}, // 次の基準点方向へ左45°
};

const SceneOrder EnterGate[] =
{
    {0, 4, ActionType::Move}, //ゲート前1
    {1, 5, ActionType::Move}, //ゲート前2
    {2, 6, ActionType::Move}, //ゲート前3
    {3, 7, ActionType::Move}, //ゲート前4
    {4, 8, ActionType::Move}, //ゲート前5
};

const SceneOrder GateCrossing[] =
{
    {0,  9, ActionType::Move}, // ゲートを500mm通過
    {1, 12, ActionType::Move}, // ゲートから500mm後退して帰還
};

const SceneOrder ReturnPoint[] =
{
    {0, 10, ActionType::Move}, //基準点帰還
};

const SceneOrder RejoinLine[] =
{
    {0, 13, ActionType::Move}, // 次の基準点方向へ50mm進んでラインへ戻る
};

/* ログタスク */
void logger_task(intptr_t exinf)
{
    logger.output();
    ext_tsk();
}

//シーン実行&遷移
bool change_scene(const SceneOrder sceneOrder[], int maxSceneNum)
{
    int sceneNum = 0;

    while (true)
    {
        const SceneOrder& sceneOrderItem = sceneOrder[sceneNum];

        sceneManager.setActionType(sceneOrderItem.actionType);
        sceneManager.setSceneID(sceneOrderItem.sceneId);
        Logger::printf("SceneID=%d\r\n", sceneOrderItem.sceneId);
        if (!sceneManager.SceneExecute())
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

    //メインループ10msec周期
/*
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
        Logger::printf("SceneID=%d\n", detectbottlecolor.sceneId);
        if(sceneManager.SceneExecute())
        {
            skipCount = SceneNum;
            Logger::printf("%s!!!!!!!!!!!!!!!!!!!!!!!!!!\n", colorName[SceneNum]);
            break;
        }
    }

    if (skipCount < 0)
    {
        tslp_tsk(200*1000);
        Logger::printf("Bottle color detection failed.\r\n");
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
*/
    int nowEdgeIndex = RIGHT_EDGE_INDEX;
    const int gatePositionCount =
        static_cast<int>(sizeof(gatePositions) / sizeof(gatePositions[0]));

    for (int gateIndex = 0; gateIndex < gatePositionCount; gateIndex++)
    {
        Color detectedPointColor = Color::Unknown;
        int pointSearchCount = 0;

        while (detectedPointColor != gatePositions[gateIndex].pointColor)
        {
            if (pointSearchCount >= MAX_POINT_SEARCH_COUNT)
            {
                Logger::printf("Target point was not found. GateIndex=%d\r\n", gateIndex);
                gyroTraceRunner.stop();
                ext_tsk();
                return;
            }

            if (!change_scene(&EnterPoint[nowEdgeIndex], 0))
            {
                ext_tsk();
                return;
            }

            // ライントレースの終了判定で検知済みの色を取得する。
            detectedPointColor = colorDetector.getLastDetectedColor();
            Logger::printf(
                "PointColor=%d TargetColor=%d\r\n",
                static_cast<int>(detectedPointColor),
                static_cast<int>(gatePositions[gateIndex].pointColor));

            const bool isTargetPoint =
                detectedPointColor == gatePositions[gateIndex].pointColor;

            // 目標色なら中央まで50mm、目標外なら色区間を抜けるため100mm進む。
            const SceneOrder* pointMove =
                isTargetPoint ? MovePointCenter : PassPoint;

            if (!change_scene(pointMove, 0))
            {
                ext_tsk();
                return;
            }

            pointSearchCount++;
        }

        Logger::printf("Target point detected. GateIndex=%d\r\n", gateIndex);

        if (!change_scene(&GateTurn[nowEdgeIndex], 0))
        {
            ext_tsk();
            return;
        }

        const int gateNum = gatePositions[gateIndex].gatePositionNum;

        if (gateNum <= 4 || gateNum >= 10)
        {
            int lookSideIndex = RIGHT_EDGE_INDEX;
            int enterGateIndex = 0;

            if (gateNum <= 4)
            {
                enterGateIndex = gateNum - 1;
                lookSideIndex = LEFT_EDGE_INDEX;
            }
            else
            {
                enterGateIndex = gateNum - 10;
                lookSideIndex = RIGHT_EDGE_INDEX;
            }

            if (enterGateIndex < 0 || enterGateIndex >= 5)
            {
                Logger::printf("Invalid gate position: %d\r\n", gateNum);
                gyroTraceRunner.stop();
                ext_tsk();
                return;
            }

            if (!change_scene(&EnterGate[enterGateIndex], 0) ||
                !change_scene(&GateTurn[lookSideIndex], 0) ||
                !change_scene(GateCrossing, 1) ||
                !change_scene(&GateTurn[lookSideIndex], 0))
            {
                ext_tsk();
                return;
            }
        }
        else
        {
            const int enterGateIndex = gateNum - 5;

            if (enterGateIndex < 0 || enterGateIndex >= 5)
            {
                Logger::printf("Invalid gate position: %d\r\n", gateNum);
                gyroTraceRunner.stop();
                ext_tsk();
                return;
            }

            if (!change_scene(&EnterGate[enterGateIndex], 0) ||
                !change_scene(&GateTurn[TURN_AROUND_INDEX], 0))
            {
                ext_tsk();
                return;
            }
        }

        if (!change_scene(ReturnPoint, 0))
        {
            ext_tsk();
            return;
        }

        const bool hasNextPoint = gateIndex + 1 < gatePositionCount;
        if (hasNextPoint)
        {
            // 基準点の並びは下から緑・黄・赤・青。
            // 黄から赤へは上方向、赤から緑へは下方向へ向き直す。
            if (!change_scene(&RejoinTurn[nowEdgeIndex], 0) ||
                !change_scene(RejoinLine, 0))
            {
                ext_tsk();
                return;
            }

            // 進行方向が反転するため、次の探索では反対側のエッジを使用する。
            nowEdgeIndex = (nowEdgeIndex == RIGHT_EDGE_INDEX)
                ? LEFT_EDGE_INDEX
                : RIGHT_EDGE_INDEX;
        }
    }

    gyroTraceRunner.stop();
    Logger::printf("終了\r\n");

    ext_tsk(); 
}
