import time

import tango
from tango.server import Device, attribute


class Device2(Device):

    attr2 = attribute()

    def read_attr2(self):
        tango.DeviceProxy("sys/tg_test/1")
        return time.time()