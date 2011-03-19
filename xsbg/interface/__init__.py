from sbclient import *
from colors import colorstring
import sys

terminal_out = sys.stdout

def fprint(msgtype, text):
	text = str(text)
	while len(text) > 255:
		temp = text[0:255]
		text = text[255:]
		echo(msgtype, temp)
	if len(text) != 0:
		echo(msgtype, text)

class std_err_class:
	def __init__(self):
		self.buffer = ""

	def write(self, text):
		text = str(text)
		if text.find('\n') >= 0:
			lines = text.split('\n')
			for line in lines:
				self.buffer += line
				self.flush()
		else:
			self.buffer += text
				
	def flush(self):
		if self.buffer != "":
			while len(self.buffer) > 255:
				temp = self.buffer[0:255]
				self.buffer = self.buffer[255:]
				echo(32, colorstring('red', temp))
			if len(self.buffer) != 0:
				echo(32, colorstring('red', self.buffer))
			self.buffer = ""
		
class std_out_class:
	def __init__(self):
		self.buffer = ""

	def write(self, text):
	
		text = str(text)
		if text.find('\n') >= 0:
			lines = text.split('\n')
			for line in lines:
				self.buffer += line
				self.flush()
		else:
			self.buffer += text
				
	def flush(self):
		if self.buffer != "":
			while len(self.buffer) > 255:
				temp = self.buffer[0:255]
				self.buffer = self.buffer[255:]
				echo(32, colorstring('green', temp))
			if len(self.buffer) != 0:
				echo(32, colorstring('green', self.buffer))
			self.buffer = ""

sys.stderr = std_err_class()
sys.stdout = std_out_class()
