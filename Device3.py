import time
import threading

import tango
from tango.server import Device, attribute, command

def cb(*args):
    print("Received event")


class Device3(Device):

    def init_device(self):
        self.continue_job = False
        self.job_thread = None
        self.set_state(tango.DevState.ON)

    def is_Start_allowed(self):
        return self.get_state() == tango.DevState.ON

    @command()
    def Start(self):
        self.set_state(tango.DevState.MOVING)
        self.continue_job = True

        def job(device):
            self.dev = tango.DeviceProxy("test/deviceeventgenerator/1")
            self.dev.subscribe_event("attr_with_events", tango.EventType.CHANGE_EVENT, cb)
            while self.continue_job:
                device.info_stream("Running job")
                time.sleep(0.01)

        self.job_thread = threading.Thread(target=job, args=(self, ))
        self.job_thread.start()

    def Stop(self):
        self.continue_job = False
        self.set_state(tango.DevState.ON)