import tango
from tango.server import Device, attribute


def cb(*args):
    pass


class Device1(Device):

    attr1 = attribute()

    def read_attr1(self):
        dev = tango.DeviceProxy("sys/tg_test/1")
        dev.subscribe_event("double_scalar",
                            tango.EventType.ATTR_CONF_EVENT,
                            cb)
        return dev.read_attribute("double_scalar").value
