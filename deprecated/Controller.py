import os
import fcntl
import libevdev
from collections import deque
from Protocol_pb2 import *

l_trigger = libevdev.InputEvent(libevdev.EV_ABS.ABS_BRAKE)
r_trigger = libevdev.InputEvent(libevdev.EV_ABS.ABS_GAS)


def is_axis(event):
    return event.matches(libevdev.EV_ABS)


def is_button(event):
    return event.matches(libevdev.EV_KEY)


class Controller:
    def __init__(self, path, history_length):
        fd = open(path, 'rb')
        fcntl.fcntl(fd, fcntl.F_SETFL, os.O_NONBLOCK)
        self.device = libevdev.Device(fd)
        self.left = deque(iterable=[0.0], maxlen=history_length)
        self.right = deque(iterable=[0.0], maxlen=history_length)

    def _handle_events(self):
        self.left.append(self.left[-1])
        self.right.append(self.right[-1])
        for event in self.device.events():
            if is_axis(event):
                value = event.value / self.device.absinfo[event.code].maximum
                if event == l_trigger:
                    self.left[-1] = value
                if event == r_trigger:
                    self.right[-1] = value

    def get_proto_messages(self):
        self._handle_events()
        cmds = []

        cmd = Command()
        cmd.setPWM.side = Side.LEFT
        cmd.setPWM.value = self.left[-1]
        cmds.append(cmd)

        cmd = Command()
        cmd.setPWM.side = Side.RIGHT
        cmd.setPWM.value = self.right[-1]
        cmds.append(cmd)

        return map(lambda cmd: cmd.SerializeToString(), cmds)
