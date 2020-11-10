import tango
from tango.server import Device, attribute


def read_double_scalar():
    dev = tango.DeviceProxy("sys/tg_test/1")
    dev.subscribe_event("double_scalar", tango.EventType.ATTR_CONF_EVENT, cb)
    return dev.read_attribute("double_scalar").value


def cb(*args):
    pass


class Device1(Device):

    attr1 = attribute()

    def read_attr1(self):
        return read_double_scalar()