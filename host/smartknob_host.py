# coding=utf-8
# Change your COM port to match that of your COM port!

import serial
import time

from ctypes import cast, POINTER
from comtypes import CLSCTX_ALL
from pycaw.pycaw import AudioUtilities, IAudioEndpointVolume
devices = AudioUtilities.GetSpeakers()
interface = devices.Activate(
    IAudioEndpointVolume._iid_, CLSCTX_ALL, None)
volume = cast(interface, POINTER(IAudioEndpointVolume))

#使用前请修改串口
connection = serial.Serial('COM4')

updateTime = 0.1  # number of seconds between each update


def sendData():
    volumeCurrent = round(volume.GetMasterVolumeLevelScalar()*100 + 1)
    try:
        recv = connection.readline().decode("utf-8")  # 读出串口数据，数据采用utf编码
        if recv.find("VOLUME:") != -1:
            volumeReceive = ''.join([x for x in recv if x.isdigit()])
            volumeReceive = int(volumeReceive)
            if volumeReceive != volumeCurrent:
                scalarVolume = int(volumeReceive) / 100
                volume.SetMasterVolumeLevelScalar(scalarVolume, None)
    except Exception as e:
        print(e)


while (1):
    connection.baudrate = 921600
    connection.setDTR(False)
    connection.setRTS(False)
    connection.flushInput()
    connection.open
    sendData()
    time.sleep(updateTime)
