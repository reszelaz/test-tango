import time
import tango

dev = tango.DeviceProxy("test/device2/1")

while True:
    dev.read_attribute("attr2")
    time.sleep(0.1)