import time
import threading

import tango
from tango.server import Device, attribute, command


def read_double_scalar():
    dev = tango.DeviceProxy("sys/tg_test/1")
    dev.subscribe_event("double_scalar", tango.EventType.ATTR_CONF_EVENT, cb)
    return dev.read_attribute("double_scalar").value


def cb(*args):
    pass


class DeviceEventGenerator(Device):

    attr_with_events = attribute()

    def init_device(self):
        self.set_change_event("attr_with_events", True, False)
        self.set_state(tango.DevState.ON)
        self.emit_events = False
        self.job_thread = None
        self.info_stream("DeviceEventGenerator.init_device")

    def read_attr_with_events(self):
        return 0.0

    def is_Start_allowed(self):
        return self.get_state() == tango.DevState.ON

    @command()
    def Start(self):
        self.emit_events = True
        self.set_state(tango.DevState.MOVING)

        def job(device):
            while device.emit_events:
                self.info_stream("Running job")
                device.push_change_event("attr_with_events", 0.0)
                time.sleep(0.01)

        self.job_thread = threading.Thread(target=job, args=(self, ))
        self.job_thread.start()

    @command()
    def Stop(self):
        self.emit_events = False
        self.job_thread.join()
        self.set_state(tango.DevState.ON)