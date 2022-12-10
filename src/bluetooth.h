#pragma once

#include <BleKeyboard.h>
#include "lvgl_gui.h"

void getProps(int32_t current);
void getConfigs(int32_t configs);
void sendpress();
void sendrelease();
void BuletoothInit();
void BluetoothLoop();
int32_t clickAct(void);