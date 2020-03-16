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

LOOP_HZ = 30
HISTORY_LENGTH = 128  # TODO time instead of ticks

LOOP_PERIOD_MS = 1000.0 / LOOP_HZ

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

class Base:
    def __init__(self):
        self.win = pg.GraphicsWindow()
        self.win.resize(1600, 900)
        self.tasks = Tasks(LOOP_PERIOD_MS)

        self.speed = [
            deque(maxlen=HISTORY_LENGTH),
            deque(maxlen=HISTORY_LENGTH)
        ]
        self.pwm = [
            deque(maxlen=HISTORY_LENGTH),
            deque(maxlen=HISTORY_LENGTH)
        ]

        pen_lt = pg.mkPen(color='b', width=2)
        pen_rt = pg.mkPen(color='r', width=1)

        self.plot_pwm = self.win.addPlot(title="Engine PWM")
        self.win.nextRow()
        self.plot_pwm.setYRange(0, 1)
        self.plot_pwm_curve = [ 
            self.plot_pwm.plot(pen=pen_lt),
            self.plot_pwm.plot(pen=pen_rt)
        ]

        self.plot_speed = self.win.addPlot(title='Wheel speed')
        self.win.nextRow()
        # TODO: this is hardcoded, but max over time is needed in fact
        self.plot_speed.setYRange(-0.5, 0.5)
        self.plot_speed_curve = [
            self.plot_speed.plot(pen=pen_lt),
            self.plot_speed.plot(pen=pen_rt)
        ]
        self.plot_speed_target = [
            self.plot_speed.addLine(y=0, pen=pen_lt),
            self.plot_speed.addLine(y=0, pen=pen_rt),
        ]

        self.plot_pid = self.win.addPlot(title='PID', colspan=1)
        self.win.nextRow()

        self.udp = UDPTransceiver(4096)
        self.tasks.add(self.process_input)

    def process_input(self):
        msg = KitekMsg()
        # targetSpeed = [0, 0]
        for byteMsg in self.udp.get_msgs():
            msg.ParseFromString(byteMsg)
            self.pwm[msg.wheelState.side].append(msg.wheelState.pwm)
            self.speed[msg.wheelState.side].append(msg.wheelState.speed)
            self.plot_speed_target[msg.wheelState.side].setValue(msg.wheelState.targetSpeed)
        
        for side in [Side.LEFT, Side.RIGHT]:
            self.plot_pwm_curve[side].setData(self.pwm[side])
            self.plot_speed_curve[side].setData(self.speed[side])

        # print(type(self.plot_speed))
        

if __name__ == '__main__':
    pg.setConfigOptions(antialias=False)
    app = QApplication(sys.argv)
    signal.signal(signal.SIGINT, lambda *_: app.quit())
    base = Base()
    sys.exit(app.exec())
