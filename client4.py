import time
import tango

dev = tango.DeviceProxy("test/device2/1")

while True:
    try:
        dev.cmd2()
    except Exception as e:
        print(e)
    time.sleep(0.1)