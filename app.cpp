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

    //HSV構造体定義
    ColorSensor::HSV hsv;

    int SeanID = 0;
    armController.moveArmDown();

    //メインループ10msec周期
    while(true)
    {
        //HSV取得
        colorSensor.getHSV(hsv);

        sceneManager.setActionType(ActionType::LineTrace);
        sceneManager.setSceneID(SeanID);
        Logger::printf("SeanID=%d", SeanID);
        if(sceneManager.SceneExecute())
        {
            SeanID++;
        }

        if (SeanID > 12){
            break;
        }
    }

    leftWheel.stop();
    rightWheel.stop();
    armController.moveArmUp();

    sceneManager.setActionType(ActionType::BottoleDetect);

    const char* colorName[] = {"黄", "青", "赤"};

    for (int sceneid = 0; sceneid < 3; sceneid++)
    {
        sceneManager.setSceneID(sceneid);

        if (sceneManager.SceneExecute())
        {
            Logger::printf("%s", colorName[sceneid]);
            break;
        }
    }

    armController.moveArmDown();

    while (true)
    {
        sceneManager.setActionType(ActionType::LineTrace);
        sceneManager.setSceneID(SeanID);
        Logger::printf("SeanID=%d", SeanID);
        if(sceneManager.SceneExecute())
        {
            SeanID++;
        }

        if (SeanID > 17){
            break;
        }
    }

    Logger::printf("終了");

    ext_tsk(); 
}


