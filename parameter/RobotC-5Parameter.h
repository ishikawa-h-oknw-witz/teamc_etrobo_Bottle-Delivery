#pragma once

#include "SceneManager.h"

// ============================================================
// Robot C-5 Parameter
// ============================================================

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
    {26,    0,  70, RunnerEdge::RightEdge, {Color::Blue}, CalibrationData::LineCenter,       {0.3f, 0.0f, 0.4f} }, // Dlv帰還青まで
    {27,  200,  70, RunnerEdge::LeftEdge,  {Color::None}, CalibrationData::BlackWhiteCenter, {0.3f, 0.0f, 0.4f} }, // Rly1~5
    {28,  450,  70, RunnerEdge::LeftEdge,  {Color::None}, CalibrationData::BlackWhiteCenter, {0.3f, 0.0f, 0.4f} }, // Rly6~10
    {29,  700,  70, RunnerEdge::LeftEdge,  {Color::None}, CalibrationData::BlackWhiteCenter, {0.3f, 0.0f, 0.4f} }, // Rly11~15
    {30,  950,  70, RunnerEdge::LeftEdge,  {Color::None}, CalibrationData::BlackWhiteCenter, {0.3f, 0.0f, 0.4f} }, // Rly16~20
};


// Move
const MoveScene moveScenes[] =
{
    { 0, Direction::front, {50.0f, 100.0f,  50.0f, 100.0f}, 100, {Color::None}, {1.0f, 0.0f, 0.0f}}, // Dlvエリアまで
    { 1, Direction::back,  {50.0f, 100.0f,  50.0f, 200.0f}, 200, {Color::None}, {1.0f, 0.0f, 0.0f}}, // Dlv線まで帰還
    { 2, Direction::front, {50.0f,  70.0f,  50.0f,  10.0f},  10, {Color::None}, {1.0f, 0.0f, 0.0f}}, // Dlvエリアまで
    { 3, Direction::front, {50.0f,  70.0f,  50.0f,  50.0f},  50, {Color::None}, {1.0f, 0.0f, 0.0f}},
    { 4, Direction::front, {50.0f,  70.0f,  50.0f, 400.0f}, 400, {Color::None}, {1.0f, 0.0f, 0.0f}}, //Rly通過5n+1
    { 5, Direction::front, {50.0f,  70.0f,  50.0f, 650.0f}, 650, {Color::None}, {1.0f, 0.0f, 0.0f}}, //Rly通過5n+2
    { 6, Direction::front, {50.0f,  70.0f,  50.0f, 900.0f}, 900, {Color::None}, {1.0f, 0.0f, 0.0f}}, //Rly通過5n+3
    { 7, Direction::front, {50.0f,  70.0f,  50.0f,1150.0f},1150, {Color::None}, {1.0f, 0.0f, 0.0f}}, //Rly通過5n+4
    { 8, Direction::front, {50.0f,  70.0f,  50.0f,1400.0f},1400, {Color::None}, {1.0f, 0.0f, 0.0f}}, //Rly通過5n
    { 9, Direction::back,  {50.0f,  70.0f,  50.0f, 350.0f}, 350, {Color::None}, {1.0f, 0.0f, 0.0f}}, //Rly帰還5n+1
    {10, Direction::back,  {50.0f,  70.0f,  50.0f, 600.0f}, 600, {Color::None}, {1.0f, 0.0f, 0.0f}}, //Rly帰還5n+2
    {11, Direction::back,  {50.0f,  70.0f,  50.0f, 850.0f}, 850, {Color::None}, {1.0f, 0.0f, 0.0f}}, //Rly帰還5n+3
    {12, Direction::back,  {50.0f,  70.0f,  50.0f,1100.0f},1100, {Color::None}, {1.0f, 0.0f, 0.0f}}, //Rly帰還5n+4
    {13, Direction::back,  {50.0f,  70.0f,  50.0f,1350.0f},1350, {Color::None}, {1.0f, 0.0f, 0.0f}}, //Rly帰還5n
    {14, Direction::front, {50.0f,  70.0f,  50.0f,1350.0f},1350, {Color::None}, {1.0f, 0.0f, 0.0f}},
};


// Turn
const TurnScene turnScenes[] =
{
    {0,  90, {1.0f, 0.0f, 0.0f}}, // 右に90°
    {1, -15, {1.0f, 0.0f, 0.0f}}, // 左に15°
    {2, -90, {1.0f, 0.0f, 0.0f}}, // 左に90°
    {3,  30, {1.0f, 0.0f, 0.0f}}, // 右に30°
    {4, 180, {1.0f, 0.0f, 0.0f}}, // 後ろ
    {5,-100, {1.0f, 0.0f, 0.0f}},
    {6, 100, {1.0f, 0.0f, 0.0f}},
};


// Bottle Detect
const BottleDetectScene bottleDetectScenes[] =
{
    {0, {Color::Yellow}}, // 黄ボトル検知
    {1, {Color::Blue}},   // 青ボトル検知
    {2, {Color::Red}}     // 赤ボトル検知
};