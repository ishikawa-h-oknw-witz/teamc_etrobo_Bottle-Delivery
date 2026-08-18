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

const int RightEdge = 0;
const int LeftEdge = 1;

struct GatePosition
{
    Color PointColor;
    int GatePositionNum;
};

struct SceneOrder
{
    int sceneNum;
    int sceneId;
    ActionType actionType;
};

const GatePosition Gateposition[] =
{
    {Color::Yellow, 3}, //赤ゲート
    {Color::Red,    4}, //青ゲート
    {Color::Green, 13}, //黄ゲート
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

const SceneOrder EnterPoint[] =
{
    {0, 28, ActionType::LineTrace}, // 右エッジ基準点まで
    {1, 29, ActionType::LineTrace}, // 左エッジ基準点まで
};

const SceneOrder DetectPointColor[] =
{
    {0, 3, ActionType::ColorDetect}, // 緑検知
    {1, 0, ActionType::ColorDetect}, // 黄検知
    {2, 2, ActionType::ColorDetect}, // 赤検知
    {3, 1, ActionType::ColorDetect}, // 青検知
};

const SceneOrder MovePointCenter[] =
{
    {0, 3, ActionType::Move}, //基準点中央まで
};

const SceneOrder GateTurn[] =
{
    {0, 0, ActionType::Turn}, //右90
    {1, 2, ActionType::Turn}, //左90
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
    {0, 9, ActionType::Move}, //ゲート通過
    {1, 1, ActionType::Move}, //ゲート帰還
};

const SceneOrder ReturnPoint[] =
{
    {0, 10, ActionType::Move}, //基準点帰還
}

/* ログタスク */
void logger_task(intptr_t exinf)
{
    logger.output();
    ext_tsk();
}

//シーン実行&遷移
void change_scene(const SceneOrder sceneOrder[], int MaxSceneNum)
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

    int skipCount = -1;

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
    int NowEdge = RightEdge;

    for (int i = 0, i < 3, i++)
    {
        for (int j = 0, j < 3, j++)
        {
            Color NowPointColor = Color::None;
            while (NowPonitColor != Gateposition[j].PointColor)
            {
                change_scene(&EnterPoint[NowEdge], 0);

                const Color* PointColors[] = {Color::Green, Color::Yellow, Collor::Red, Color::Blue};

                for (int SceneNum = 0; SceneNum < 4; SceneNum++)
                {
                    const SceneOrder& detectpointcolor = DetectPointColor[SceneNum];

                    sceneManager.setActionType(detectpointcolor.actionType);
                    sceneManager.setSceneID(detectpointcolor.sceneId);
                    Logger::printf("SceneID=%d\n", detectpointcolor.sceneId);
                    if (sceneManager.SceneExecute())
                    {
                        NowPointColor = PointColor[SceneNum];
                        Logger::printf("%s!!!!!!!!!!!!!!!!!!!!!!!!!!\n", PointColors[SceneNum]);
                        break;
                    }
                }
            }

            change_scene(MovePointCenter, 0);

            change_scene(&GateTurn[NowEdge], 0);

            int GateNum = Gateposition[j].GatePositionNum;

            if (GateNum <= 4 || GateNum >= 10)
            {
                int lookside = 0;
                if (GateNum <= 4)
                {
                    change_scene(EnterGate[Gateposition[j].GatePositionNum - 1], 0);
                    lookside = 1;
                }
                else 
                {
                    change_scene(EnterGate[Gateposition[j].GatePositionNum - 10], 0);
                    lookside = 0;
                }

                change_scene(&GateTurn[lookside], 0);
                change_scene(GateCrossing, 1);
                change_scene(&GateTurn[lookside], 0);
            }
            else
            {
                change_scene(EnterGate[Gateposition[j].GatePositionNum - 5], 0);
                change_scene(&GateTurn[3], 0);
            }
            change_scene(ReturnPoint, 0);

            j = 3;
        }

        i = 3;
    }

    Logger::printf("終了");

    ext_tsk(); 
}