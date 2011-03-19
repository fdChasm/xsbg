from xsbg.events import registerClientEventHandler
from sbclient import *
import os.path

#helper function to remove spaces from Both beginning and end of the strings
def bstrip(string):
	string = str(string)
	string = string.lstrip()
	string = string.rstrip()
	return string

class autologin_manager:
	def __init__(self, credential_filename):
		self.credentials = {}
		credvars = {}
		credential_filename = os.path.expanduser(credential_filename)
		f = open(credential_filename, 'r')
		credlines = f.readlines()
		#please ignore this bit of ugly code, it parses the login details file
		for line in credlines:
			if line[0] == '$':
				line = line[1:]
				line = line.split('=')
				line = map(bstrip, line)
				if len(line) == 2:
					credvars[line[0]] = line[1]
			elif line[0] == '#':
				pass
			else:
				for varname in credvars.keys():
					line = line.replace('$' + varname, credvars[varname])
				line = line.split("\t\t")
				if len(line) > 2:
					if line[2][-1:] == "\n":
						line[2] = line[2][:-1]
					line = map(bstrip, line)
					self.credentials[line[0]] = [line[1], line[2]]
		f.close()
			
	def on_local_connect_post(self, eventname, cn, serverip, serverport):
		if serverip in self.credentials.keys():
			if self.credentials[serverip][1] == "setmaster":
				setmaster(self.credentials[serverip][0])
			elif self.credentials[serverip][1] == "#login":
				say("#login " + self.credentials[serverip][0])
	
am = autologin_manager("~/.sauerbraten/credentials.conf")

registerClientEventHandler("local_connect_post", am.on_local_connect_post)

print "\tAuto login plugin now active!"
