import time

import tango
from tango.server import Device, attribute


def create_device_proxy():
    tango.DeviceProxy("sys/tg_test/1")
    return time.time()


class Device2(Device):

    attr2 = attribute()

    def read_attr2(self):
        return create_device_proxy()
