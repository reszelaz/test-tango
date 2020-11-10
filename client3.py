import time
import tango

dev = tango.DeviceProxy("test/device1/1")

while True:
    try:
        dev.cmd1()
    except Exception as e:
        print(e)
    time.sleep(0.1)
