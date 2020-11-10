import threading

import tango
from tango.server import Device, attribute, command


def read_double_scalar():
    dev = tango.DeviceProxy("sys/tg_test/1")
    dev.subscribe_event("double_scalar", tango.EventType.ATTR_CONF_EVENT, cb)
    return dev.read_attribute("double_scalar").value


def cb(*args):
    pass


class Device1(Device):

    attr1 = attribute()

    def init_device(self):
        self.set_state(tango.DevState.ON)

    def read_attr1(self):
        return read_double_scalar()

    def is_cmd1_allowed(self):
        return self.get_state() == tango.DevState.ON

    @command()
    def cmd1(self):
        self.set_state(tango.DevState.MOVING)

        def job(device):
            read_double_scalar()
            device.set_state(tango.DevState.ON)

        threading.Thread(target=job, args=(self, )).start()