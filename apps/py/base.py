#!/usr/bin/env python3

import sys
import signal
import pyqtgraph as pg
from PyQt5.QtCore import QTimer
from PyQt5.QtWidgets import QApplication
from collections import deque

from UDPTransceiver import UDPTransceiver
from Controller import Controller
from Protocol_pb2 import *

CONTROLLER_FILE_PATH = '/dev/input/by-id/usb-NVIDIA_Corporation_NVIDIA_Controller_v01.04-event-joystick'
HISTORY_LENGTH = 1024  # TODO time instead of ticks
LOOP_HZ = 30

LOOP_PERIOD_MS = 1000.0 / LOOP_HZ


class Tasks:
    def __init__(self, period_ms):
        self.period_ms = period_ms
        self.tasks = []

    def add(self, func):
        timer = QTimer()
        timer.timeout.connect(func)
        timer.start(self.period_ms)
        self.tasks.append(timer)


class Base:
    def __init__(self):
        self.win = pg.GraphicsWindow()
        self.win.resize(1600, 900)
        self.tasks = Tasks(LOOP_PERIOD_MS)
        self.speed = {
            Side.LEFT: deque(maxlen=HISTORY_LENGTH),
            Side.RIGHT: deque(maxlen=HISTORY_LENGTH)
        }

        pen_lt = pg.mkPen(color='b', width=2)
        pen_rt = pg.mkPen(color='r', width=1)
        self.plot_input = self.win.addPlot(title="Joystick input")
        self.plot_speed = self.win.addPlot(title='Wheel speed')

        self.plot_input.setYRange(0, 1)
        self.plot_input_curve_l = self.plot_input.plot(pen=pen_lt)
        self.plot_input_curve_r = self.plot_input.plot(pen=pen_rt)

        self.plot_speed_curve_l = self.plot_speed.plot(pen=pen_lt)
        self.plot_speed_curve_r = self.plot_speed.plot(pen=pen_rt)

        self.ctl = Controller(CONTROLLER_FILE_PATH, HISTORY_LENGTH)
        self.udp = UDPTransceiver(2048)
        self.udp.set_target('192.168.0.172', 1024)

        self.tasks.add(self.process_controller)
        self.tasks.add(self.process_feedback)

    def process_feedback(self):
        fb = Feedback()
        msgs = self.udp.get_msgs()
        for msg in msgs:
            fb.ParseFromString(msg)
            self.speed[fb.wheelState.side].append(fb.wheelState.speed)
        self.plot_speed_curve_l.setData(self.speed[Side.LEFT])
        self.plot_speed_curve_r.setData(self.speed[Side.RIGHT])

    def process_controller(self):
        self.plot_input_curve_l.setData(self.ctl.left)
        self.plot_input_curve_r.setData(self.ctl.right)
        msgs = self.ctl.get_proto_messages()
        for msg in msgs:
            self.udp.send_msg(msg)


if __name__ == '__main__':
    pg.setConfigOptions(antialias=True)
    app = QApplication(sys.argv)
    signal.signal(signal.SIGINT, lambda *_: app.quit())
    base = Base()
    sys.exit(app.exec())
