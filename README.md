<p align="center"> <img src="https://github.com/Eanya-Tonic/SmartKnob_Plus/blob/main/src/images/logo.png" style="width:200px;" /> </p>  <h1 align="center">SmartKnob_Plus</h1>  <p align="center"> 一种开源输入设备 </p>

基于原始SmartKnob固件开发的增强版固件，采用LVGL显示框架、启用蓝牙HID支持、兼容Surface Dial模式。

*人机交互课程作业*

**目录**

&emsp;<a href="#1">实现功能如下</a>  
&emsp;&emsp;<a href="#2">操作说明</a>  
&emsp;&emsp;&emsp;<a href="#3">0x00 启动界面</a>  
&emsp;&emsp;&emsp;<a href="#4">0x01 音量控制界面</a>  
&emsp;&emsp;&emsp;<a href="#5">0x02 功能菜单</a>  
&emsp;&emsp;&emsp;<a href="#6">0x03 移动控制</a>  
&emsp;&emsp;&emsp;<a href="#7">0x04 媒体控制</a>  
&emsp;&emsp;&emsp;<a href="#8">0x05 兼容模式</a>  
&emsp;&emsp;<a href="#9">参考</a> 

## <a name="1">实现功能如下</a>
1.切换至LVGL显示框架显示UI，并定制UI（[src/lvgl_gui.h](https://github.com/Eanya-Tonic/SmartKnob_Plus/blob/main/src/lvgl_gui.h)）

2.启用蓝牙HID模式，支持控制音量、方向键滚动、切换歌曲（[src/bluetooth.h](https://github.com/Eanya-Tonic/SmartKnob_Plus/blob/main/src/bluetooth.h)）

3.兼容Surface Dial模式，可以模拟Surface Dial操作电脑（[lib/ESP32 BLE Keyboard/BleKeyboard.h](https://github.com/Eanya-Tonic/SmartKnob_Plus/blob/main/lib/ESP32%20BLE%20Keyboard/BleKeyboard.h)）


### <a name="2">操作说明</a>

  克隆项目文件到本地，采用VsCode + Platform IO插件，在Platform IO后打开项目文件后，编译烧录至SmartKnob即可。如果出现旋钮异常震动现象，可以采用和原版SmartKnob相同的方法校准旋钮并修改代码重新编译。

#### <a name="3">0x00 启动界面</a>

  一个读条启动界面，用于加载系统

<p align="center"><img src="https://user-images.githubusercontent.com/74545593/206860833-0b1d22f2-425c-4876-a621-7372364110f3.jpg" style="width:500px; margin:25px;"></p>


#### <a name="4">0x01 音量控制界面</a>

  音量控制界面用于控制系统音量，左右滚动增加或减小系统音量，短按静音/解除静音。由于蓝牙协议发送限制，代码无法实现在快速旋转时精准控制音量，易导致显示音量和实际音量不符，为解决此问题，请在PC上运行/host/smartknob_host.py并将SmartKnob通过USB连接到电脑，根据串口号修改python脚本中的COM号。

<p align="center"><img src="https://user-images.githubusercontent.com/74545593/206860896-8482ea61-2fff-43e8-b16d-b6fe40e52a0f.jpg" style="width: 500px; margin: 25px">
    <img src="https://user-images.githubusercontent.com/74545593/206860899-c3e0f620-b0b8-4701-be54-ffc1a5996451.jpg" style="width: 500px; margin: 25px"></p>

#### <a name="5">0x02 功能菜单</a>

  长按即可调取功能菜单，功能菜单用于选取需要的功能，目前有音量控制、移动控制、媒体控制、兼容模式四个可选择的功能。通过左右滚动旋钮即可切换选项，短按旋钮可以确认选项。

<p align="center"><img src="https://user-images.githubusercontent.com/74545593/206861828-59ede72c-e30e-404b-ba1a-ecbb64df808a.jpg" style="width: 500px; margin: 25px">
    <img src="https://user-images.githubusercontent.com/74545593/206861832-bc7aaafb-8286-4185-b36e-001e2a1d6bb0.jpg" style="width: 500px; margin: 25px"></p>

#### <a name="6">0x03 移动控制</a>

  移动控制控制方向键上下左右，左右滚动旋钮默认激活上下方向键，短按旋钮切换旋钮控制上下方向键/左右方向键。

<p align="center"><img src="https://user-images.githubusercontent.com/74545593/206861949-8ac3fa3a-7767-46d2-b857-4e473ed8ebb6.jpg" style="width: 500px; margin: 25px">
</p>

#### <a name="7">0x04 媒体控制</a>

  控制媒体的暂停/播放、切换上一首/下一首，通过左右滚动旋钮切换上一首/下一首歌曲，短按控制暂停/播放。

<p align="center"><img src="https://user-images.githubusercontent.com/74545593/206862031-a673b7d2-7be5-4bd5-9bf3-afaff643a325.jpg" style="width: 500px; margin: 25px">
</p>

#### <a name="8">0x05 兼容模式</a>

  兼容模式即进入Surface Dial模拟模式，由于Surface Dial会占用长按功能，因此进入Surface Dial模式后只能通过重启SmartKnob回到常规模式。左右滚动旋钮可以实现确认按钮/取消按钮，在确认模式下短按即可进入兼容模式配置界面。

<p align="center"><img src="https://user-images.githubusercontent.com/74545593/206862112-7e9938bb-7a57-48f7-a181-e22c9f377170.jpg" style="width: 500px; margin: 25px">
</p>

  在进入兼容模式前，允许用户自定义想要使用的阻尼模式，包括无阻尼、弱阻尼、强阻尼三个模式，用户可以在配置界面通过左右滚动选定选项，选定后短按旋钮可以正式启动兼容模式。

 <p align="center"><img src="https://user-images.githubusercontent.com/74545593/206862231-ba8e050d-5fec-4acd-b2e0-6eb165802865.jpg" style="width: 500px; margin: 25px">
</p>

  进入兼容模式后，控制模式选择操作转换至电脑上显示，具体操作方式与Surface Dial没有区别，旋钮自带的LCD屏幕转换为显示开发团队LOGO

 <p align="center"><img src="https://user-images.githubusercontent.com/74545593/206862324-fa6bd278-08ba-4d77-8d50-4dbb5c3f9ced.jpg" style="width: 500px; margin: 25px">
</p>

### <a name="9">参考</a>

[scottbez1 / smartknob](https://github.com/scottbez1/smartknob)

[AndreMiras / pycaw](https://github.com/AndreMiras/pycaw)

[T-vK / ESP32-BLE-Keyboard](https://github.com/T-vK/ESP32-BLE-Keyboard)

[lvgl / lvgl](https://github.com/lvgl/lvgl)

[起个名字都要抢占 / ESP32-SurfaceDial](https://gitee.com/adamhxx/esp32-surface-dial)

