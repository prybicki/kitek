#!/usr/bin/env python3

import sys
import select
import signal
import socket
import pyqtgraph as pg
from PyQt5.QtCore import QTimer
from PyQt5.QtWidgets import QApplication
from collections import deque
from Protocol_pb2 import *

SIDES = [Side.LEFT, Side.RIGHT]

LOOP_HZ = 30
HISTORY_LENGTH = 128  # TODO time instead of ticks
LOOP_PERIOD_MS = 1000.0 / LOOP_HZ
TIME_WINDOW_SECS = 8


class UDPTransceiver:
	def __init__(self, port):
		self.target = None
		self.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		self.socket.setblocking(False)
		self.socket.bind(('0.0.0.0', port))
		self.get_msgs()  # Discard pending messages
	
	def set_target(self, ip, port):
		self.target = (ip, port)
	
	def get_msgs(self):
		msgs = []
		while True:
			to_read, _, _ = select.select([self.socket], [], [], 0)
			if self.socket not in to_read:
				break
			data, addr = self.socket.recvfrom(32)
			msgs.append(data)
			if self.target is None or self.target != addr:
				self.target = addr
		return msgs

	def send_msg(self, msg):
		if self.target is None:
			return False
		self.socket.sendto(msg, self.target)
		return True

	def __del__(self):
		self.socket.close()


class Tasks:
    def __init__(self, period_ms):
        self.period_ms = period_ms
        self.tasks = []

    def add(self, func):
        timer = QTimer()
        timer.timeout.connect(func)
        timer.start(self.period_ms)
        self.tasks.append(timer)


class TimeSeriesPlot(pg.PlotItem):
    def __init__(self, keys, time_window, *args, **kwargs):
        super().__init__(*args, **kwargs)
        # self.addLegend()
        self.curve = {key: super(TimeSeriesPlot, self).plot() for key in keys}
        self.x = {key: deque() for key in keys}
        self.y = {key: deque() for key in keys}
        self.time_window = time_window

    def append(self, key, timestamp, value):
        timestamp /= 1e6
        self.x[key].append(timestamp)  # Convert usec -> sec
        self.y[key].append(value)
        print('{} - {} = {} ~ {}'.format(timestamp, self.x[key][0], timestamp - self.x[key][0], self.time_window))
        while len(self.x[key]) > 0 and timestamp - self.x[key][0] > self.time_window:
            self.x[key].popleft()
            self.y[key].popleft()
        # print('{}: ({}, {})'.format(key, self.x[key][0]))
        self.curve[key].setData(self.x[key], self.y[key])


class Base:
    def __init__(self):
        self.win = pg.GraphicsWindow()
        self.win.resize(1600, 900)
        self.tasks = Tasks(LOOP_PERIOD_MS)

        self.pwm_plot = TimeSeriesPlot(keys=SIDES, time_window=TIME_WINDOW_SECS, title='Engine PWM')
        self.pwm_plot.setYRange(0, 1)
        self.win.addItem(self.pwm_plot)
        self.win.nextRow()

        self.speed_plot = TimeSeriesPlot(keys=SIDES, time_window=TIME_WINDOW_SECS, title='Wheel Speed')
        self.speed_line = {side: self.speed_plot.addLine() for side in SIDES}
        self.win.addItem(self.speed_plot)
        self.win.nextRow()

        pid_keys = ['pOut', 'iOut', 'dOut', 'error', 'integral']
        self.pid_plot = TimeSeriesPlot(keys=pid_keys, time_window=TIME_WINDOW_SECS, title='PID')
        self.win.addItem(self.pid_plot)
        self.win.nextRow()

        self.udp = UDPTransceiver(4096)
        self.tasks.add(self.process_input)

    def process_input(self):
        msg = KitekMsg()
        for byteMsg in self.udp.get_msgs():
            msg.ParseFromString(byteMsg)
            if msg.HasField('wheelState'):
                w = msg.wheelState
                self.pwm_plot.append(w.side, w.timestamp, w.pwm)
                self.speed_plot.append(w.side, w.timestamp, w.speed)
                self.speed_line[w.side].setValue(w.targetSpeed)
            # if msg.pidState is not None:
            #     p = msg.pidState
            #     self.pid_plot.append('pOut', p.timestamp, p.pOut)
            #     self.pid_plot.append('iOut', p.timestamp, p.iOut)
            #     self.pid_plot.append('dOut', p.timestamp, p.dOut)
            #     self.pid_plot.append('error', p.timestamp, p.error)
            #     self.pid_plot.append('integral', p.timestamp, p.integral)
            
            # ADD horizontal line
            # self.speed_target[msg.wheelState.side].setValue(msg.wheelState.targetSpeed)

if __name__ == '__main__':
    pg.setConfigOptions(antialias=False)
    app = QApplication(sys.argv)
    signal.signal(signal.SIGINT, lambda *_: app.quit())
    base = Base()
    sys.exit(app.exec())
