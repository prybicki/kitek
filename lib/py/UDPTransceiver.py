#!/usr/bin/env python3

import socket
import select


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


if __name__ == '__main__':
	udp = UDPTransceiver(2048)
	while True:
		print(udp.get_next_msg())
