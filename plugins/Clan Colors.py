from xsbg.players import extendPlayers, player
from sbclient import *
import random, os.path

def bstrip(string):
	"""helper function to remove spaces from Both beginning and end of the strings"""
	string = str(string)
	string = string.lstrip()
	string = string.rstrip()
	return string

class clan:
	def __init__(self, name, tag, color):
		self.name = name
		self.tag = tag
		self.color = color

class player_clan:
	"""Extend the player object with the clan object"""
	def __init__(self, clan_colors_filename):
	
		self.clans = []

		clan_colors_filename = os.path.expanduser(clan_colors_filename)
	
		f = open(clan_colors_filename, 'r')
		lines = f.readlines()
		#please ignore this bit of ugly code, it parses the login details file
		for line in lines:
			if line[0] == '#':
				pass
			else:
				line = line.split("\t\t")
				if len(line) > 1:
					if line[1][-1:] == "\n":
						line[1] = line[1][:-1]
					line = map(bstrip, line)
					line[1] = int(line[1], 16)
					self.clans.append(clan(line[0], line[0], line[1]))
		f.close()
	
		self.events = {
			"connect": self.on_connect,
			"rename": self.on_rename
		}
	
	def on_connect(self, cn):
		p = player(cn)
		if p:
				for clanob in self.clans:
					if p.name().find(clanob.tag) >= 0:
						p.clan = clanob
						setmark(p.cn, True)
						markcolor(p.cn, p.clan.color)
						break

	def on_rename(self, cn, oldname, newname):
		self.on_connect(cn)
		
	def load(self, players):
		return True
		
	def unload(self, players):
		return True
		
player_clan_extension = player_clan("~/.sauerbraten/ClanColors.conf")
		
extendPlayers("clan_colors", player_clan_extension)

print "\tClan colors plugin Now Active!"
