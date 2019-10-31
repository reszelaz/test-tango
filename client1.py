import time
import tango

dev = tango.DeviceProxy("test/device1/1")

while True:
    dev.read_attribute("attr1")
    time.sleep(0.1)