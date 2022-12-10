#include "bluetooth.h"

// 设置蓝牙连接信息
BleKeyboard bleKeyboard("SmartKnob", "RichFamilyTeam");

/*
previous_position: 上一次的位置
current_position: 当前的位置
current_config: 当前的配置
*/
int32_t previous_position = 0;
int32_t current_position = 0;
int32_t current_config = 0;

// 是否是第一次启动
bool FirtTime = true;
// 是否反转控制
bool invertControl = false;

// 获取当前的位置
void getProps(int32_t current)
{
    current_position = current;
}

// 获取当前的配置
void getConfigs(int32_t configs)
{
    current_config = configs;
    /*
    config 0: 音量控制
    config 1: 移动控制
    config 2: 音乐控制
    config 3: 兼容模式确认页
    config 4: 兼容模式设置页
    config 5: 功能菜单
    config 6: 兼容模式无阻尼
    config 7: 兼容模式弱阻尼
    config 8: 兼容模式强阻尼
    */
}

// 发送Dial按下事件
void sendpress()
{
    bleKeyboard.sendDialReport(DIAL_PRESS);
}

// 发送Dial释放事件
void sendrelease()
{
    bleKeyboard.sendDialReport(DIAL_RELEASE);
}

// 初始化蓝牙
void BuletoothInit()
{
    bleKeyboard.begin();
}

// 蓝牙的任务处理
void BluetoothLoop()
{
    if (bleKeyboard.isConnected())
    {
        // 初始化设置音量为0
        if (FirtTime && current_position == 0)
        {
            for (size_t i = 0; i < 100; i++)
            {
                bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN);
            }
            FirtTime = false;
        }
        // 向右旋转
        if (previous_position - current_position < 0)
        {
            if (current_config == 0)
            {
                bleKeyboard.write(KEY_MEDIA_VOLUME_UP);
            }
            if (current_config == 1)
            {
                if (!invertControl)
                {
                    bleKeyboard.write(KEY_DOWN_ARROW);
                }
                else
                {
                    bleKeyboard.write(KEY_RIGHT_ARROW);
                }
            }
            if (current_config == 2)
            {
                bleKeyboard.write(KEY_MEDIA_NEXT_TRACK);
            }
            if (current_config >= 6)
            {
                bleKeyboard.sendDialReport(DIAL_R);
            }
        }
        // 向左旋转
        else if (previous_position - current_position > 0)
        {
            if (current_config == 0)
            {
                bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN);
            }
            if (current_config == 1)
            {
                if (!invertControl)
                {
                    bleKeyboard.write(KEY_UP_ARROW);
                }
                else
                {
                    bleKeyboard.write(KEY_LEFT_ARROW);
                }
            }
            if (current_config == 2)
            {
                bleKeyboard.write(KEY_MEDIA_PREVIOUS_TRACK);
            }
            if (current_config >= 6)
            {
                bleKeyboard.sendDialReport(DIAL_L);
            }
        }
        previous_position = current_position;
    }
}

//点击事件处理函数
int32_t clickAct(void)
{
    switch (current_config)
    {
    case 0:
        bleKeyboard.write(KEY_MEDIA_MUTE);
        muteAction();
        break;
    case 1:
        invertControl = !invertControl;
        break;
    case 2:
        bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);
        break;
    case 3:
        if (current_position == 1)
        {
            nextSetup();
            return 1;
        }
        break;
    case 4:
        //切换兼容模式
        startcompatibleMode();
        return 2;
        break;
    default:
        break;
    }
    return 0;
}