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

const int GatePosition[3] = {15, 38, 4};

struct SceneOrder
{
    int sceneNum;
    int sceneId;
    ActionType actionType;
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
    {0, 0, ActionType::BottoleDetect}, //黄ボトル検知
    {1, 1, ActionType::BottoleDetect}, //青ボトル検知
    {2, 2, ActionType::BottoleDetect}  //赤ボトル検知
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
    {1, 23, ActionType::LineTrace},  // Dlv青から行きゲート前まで
    {2, 24, ActionType::LineTrace},  // Dlv黄から行きゲート前まで
};

const SceneOrder EnterRally[] =
{
    {0, 25, ActionType::LineTrace}, // Dlv帰還カーブ1
};

const SceneOrder EnterBule[] =
{
    {0, 26, ActionType::LineTrace}, // 青まで
    {1,  4, ActionType::Turn}, // 後ろを向く
};

const SceneOrder MoveLine[] =
{
    {0, 27, ActionType::LineTrace}, // Rly1~5
    {1, 28, ActionType::LineTrace}, // Rly6~10
    {2, 29, ActionType::LineTrace}, // Rly11~15
    {3, 30, ActionType::LineTrace}, // Rly16~20
};

const SceneOrder Trun[] =
{
    {0, 2, ActionType::Turn}, //左に90°
    {1, 5, ActionType::Turn}, //左に100°
    {2, 0, ActionType::Turn}, //右に90°
    {3, 6, ActionType::Turn}, //右に100°
};

const SceneOrder Rally[] =
{
    {0,  4, ActionType::Move}, // ラインから区画
    {1,  5, ActionType::Move}, // 1区画前
    {2,  6, ActionType::Move}, // 2区画前
    {3,  7, ActionType::Move}, // 3区画前
    {4,  8, ActionType::Move}, // 4区画前
    {5,  9, ActionType::Move}, // 1区画後
    {6, 10, ActionType::Move}, // 2区画後
    {7, 11, ActionType::Move}, // 3区画後
    {8, 12, ActionType::Move}, // 4区画後
    {9, 13, ActionType::Move}, // ラインへ復帰
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
void change_scene(const SceneOrder sceneOrder[], int MaxSceneNum)
{
    int SceneNum = 0;

    while (true)
    {
        const SceneOrder& sceneorder = sceneOrder[SceneNum];

        sceneManager.setActionType(sceneorder.actionType);
        sceneManager.setSceneID(sceneorder.sceneId);
        Logger::printf("[app]SceneID=%d\n", sceneorder.sceneId);
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

    /*
    int skipCount = -1;

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
    change_scene(EnterRally, 0);

    Logger::printf("[app]終了\n");
    */

    Logger::printf("[app]ETrally Start!\n");

    change_scene(EnterBule, 1);

    //周回カウント
    for (int LoopIndex = 0; LoopIndex < 3; LoopIndex++)
    {
        int NowPosition = 0;
        bool shouldHandleException = false;

        //例外処理が必要か確認
        if (((GatePosition[0] - 1) % 5 ) - ((GatePosition[2] - 1) % 5) == 1)
        {
            shouldHandleException = true;
        }

        //赤ゲートのある列まで向かう
        change_scene(&MoveLine[(GatePosition[0] - 1) / 5], 0);
        //ゲートの方を向く
        change_scene(&Trun[0], 0);
        //区画に入る
        change_scene(&Rally[0], 0);

        //赤ゲート通過
        if (GatePosition[0] % 5 != 1)
        {
            change_scene(&Rally[(GatePosition[0] - 1) % 5], 0);
        }

        //例外処理
        if (shouldHandleException == true)
        {
            //青ゲートの列に移動
            //青ゲートが左
            if ((GatePosition[1] - 21) / 4 <= (GatePosition[0] - 1) / 5)
            {
                change_scene(&Trun[0], 0);
                change_scene(&Rally[((GatePosition[0] - 1) / 5) - ((GatePosition[1] - 21) / 4) + 1], 0);
                change_scene(&Trun[2], 0);
            }
            //青ゲートが右
            else
            {
                change_scene(&Trun[2], 0);
                change_scene(&Rally[((GatePosition[1] - 21) / 4) - ((GatePosition[0] - 1) / 5)], 0);
                change_scene(&Trun[0], 0);
            }
        }

        //青ゲートの行まで移動
        //青ゲートが上
        if ((GatePosition[1] - 21) % 4 > (GatePosition[0] - 1) % 5)
        {
            change_scene(&Rally[((GatePosition[1] - 21) % 4) - ((GatePosition[0] - 1) % 5)], 0);
        }
        //青ゲートが下
        else if ((GatePosition[1] - 21) % 4 < (GatePosition[0] - 1) % 5)
        {
            change_scene(&Rally[((GatePosition[0] - 1) % 5) - ((GatePosition[1] - 21) % 4) + 4], 0);
        }

        //通常処理
        if (shouldHandleException == false)
        {
            //青ゲート通過
            //青ゲートが左
            if ((GatePosition[1] - 21) / 4 <= (GatePosition[0] - 1) / 5 &&
                shouldHandleException == false)
            {
                change_scene(&Trun[0], 0);
                change_scene(&Rally[((GatePosition[0] - 1) / 5) - ((GatePosition[1] - 21) / 4) + 1], 0);
                NowPosition = ((GatePosition[1] - 21) / 4) - 1;
                change_scene(&Trun[2], 0);
            }
            //青ゲートが右
            else if (shouldHandleException == false)
            {
                change_scene(&Trun[2], 0);
                change_scene(&Rally[((GatePosition[1] - 21) / 4) - ((GatePosition[0] - 1) / 5)], 0);
                NowPosition = (GatePosition[1] - 21) / 4;
                change_scene(&Trun[0], 0);
            }
        }

        //例外処理
        if (shouldHandleException == true)
        {
            //青ゲートを右に通過
            if ((GatePosition[1] - 21) / 4 <= (GatePosition[0] - 1) / 5)
            {
                change_scene(&Trun[2], 0);
                change_scene(&Rally[1], 0);
                NowPosition = (GatePosition[1] - 21) / 4;
                change_scene(&Trun[0], 0);
            }
            //青ゲートを左に通過
            else
            {
                change_scene(&Trun[0], 0);
                change_scene(&Rally[1], 0);
                NowPosition = ((GatePosition[1] - 21) / 4) - 1;
                change_scene(&Trun[2], 0);
            }
        }

        //黄ゲートの行に移動
        //黄ゲートが上
        if ((GatePosition[1] - 21) % 4 < (GatePosition[2] - 1) % 5)
        {
            change_scene(&Rally[((GatePosition[2] - 1) % 5) - ((GatePosition[1] - 21) % 4)], 0);
        }
        //黄ゲートが下
        else if ((GatePosition[1] - 21) % 4 > (GatePosition[2] - 1) % 5)
        {
            change_scene(&Rally[((GatePosition[1] - 21) % 4) - ((GatePosition[2] - 1) % 5) + 4], 0);
        }

        //黄ゲートの列に移動
        //黄ゲートが左
        if ((GatePosition[2] - 1) / 5 < NowPosition)
        {
            change_scene(&Trun[0], 0);
            change_scene(&Rally[NowPosition - ((GatePosition[2] - 1) / 5)], 0);
            change_scene(&Trun[2], 0);
        }
        //黄ゲートが右
        else if ((GatePosition[2] - 1) / 5 < NowPosition)
        {
            change_scene(&Trun[2], 0);
            change_scene(&Rally[((GatePosition[2] - 1) / 5) - NowPosition], 0);
            change_scene(&Trun[0], 0);
        }

        //一番下の区間まで移動
        change_scene(&Rally[((GatePosition[2] - 1) % 5) + 4], 0);
        //ラインに帰還
        change_scene(&Rally[9], 0);
        //青マークの方を向く
        change_scene(&Trun[0], 0);

        //青を検知して反対を向く
        change_scene(EnterBule, 1);
    }

    ext_tsk(); 
}