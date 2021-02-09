import time
import tango

eg = tango.DeviceProxy("test/deviceeventgenerator/1")
dev2 = tango.DeviceProxy("test/device2/2")
dev3 = tango.DeviceProxy("test/device3/1")

eg.Start()
dev3.Start()
while True:
    dev2.read_attribute("attr2")
    time.sleep(0.01)
