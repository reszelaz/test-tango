import time
import threading

import tango
from tango.server import Device, attribute, command


def create_device_proxy():
    tango.DeviceProxy("sys/tg_test/1")
    return time.time()


class Device2(Device):

    attr2 = attribute()

    def init_device(self):
        self.set_state(tango.DevState.ON)

    def read_attr2(self):
        return create_device_proxy()

    def is_cmd2_allowed(self):
        return self.get_state() == tango.DevState.ON

    @command()
    def cmd2(self):
        self.set_state(tango.DevState.MOVING)

        def job(device):
            create_device_proxy()
            device.set_state(tango.DevState.ON)

        threading.Thread(target=job, args=(self, )).start()
