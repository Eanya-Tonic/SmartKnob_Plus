#include <Arduino.h>


#include "lvgl_gui.h"
#include "bluetooth.h"
#include "interface_task.h"
#include "motor_task.h"

static MotorTask motor_task(1);
InterfaceTask interface_task(0, motor_task);

void setup()
{
  //启动串口
  Serial.begin(921600);
  //为 motor_task 设置一个日志记录器
  motor_task.setLogger(&interface_task);
  //启动 motor_task
  motor_task.begin();
  //启动 interface_task
  interface_task.begin();
  //初始化蓝牙
  BuletoothInit();
  //初始化 GUI
  gui_init();
}

void loop()
{
  //lvgl 的任务处理
  lv_task_handler(); 
  delay(5);
  //蓝牙的任务处理
  BluetoothLoop();
  //lvgl 的 GUI 事件处理
  gui_loop();
}