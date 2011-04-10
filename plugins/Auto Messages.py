from sbclient import *
from xsbg.events import registerClientEventHandler
from xsbg.players import extendPlayers, player

import time

weapons = {
0: "Chainsaw", 
1: "Shotgun", 
2: "Machine gun", 
3: "Rocket launcher", 
4: "Rifle", 
5: "Grenade launcher", 
6: "Pistol"}

sorryphrases = ["Sorry", "sry", "sory", "My appologies", "srry", "My bad", "My mistake", "rsy", "rys", "srory"]

class autoRespondManager:
	def __init__(self):
		#key is the cn of the player we're awaiting an apology from
		self.waitinglist = {}
		
	def on_disconnect(self, cn):
		if cn in self.waitinglist.keys():
			del self.waitinglist[cn]
		
	def on_message(self, event, cn, message):
		if cn in self.waitinglist.keys():
			self.waitinglist[cn].on_message(event, cn, message)
			
	def timeout(self, cn):
		if cn in self.waitinglist.keys():
			del self.waitinglist[cn]
			
	def register(self, cn):
		self.waitinglist[cn] = autorespond(cn)
		
aRM = autoRespondManager()
	
class autorespond:
	def __init__(self, watchcn):
		self.watchcn = watchcn
		self.starttime = time.time()
		
	def on_message(self, event, cn, message):
		if (time.time() - self.starttime) > 15:
			aRM.timeout(cn)
		else:
			if cn == self.watchcn:
				for phrase in sorryphrases:
					phrase = phrase.lower()
					message = message.lower()
					if message.find(phrase) >= 0:
						if event == "chat":
							autosay("No problem " + clientname(self.watchcn) + ". Thanks for apologizing.")
						elif event == "teamchat":
							autosayteam("No problem " + clientname(self.watchcn) + ". Thanks for apologizing.")
						aRM.timeout(cn)
						break

def teamkill_callback(event, killercn, killedcn):
	if cn() == killercn:
		killed = clientname(killedcn)
		autosay("Sorry " + killed + "!")
	elif cn() == killedcn:
		aRM.register(killercn)

def on_intermission(args):
	autosay("Good Game.")

registerClientEventHandler("teamkill", teamkill_callback)

registerClientEventHandler("chat", aRM.on_message)
registerClientEventHandler("teamchat", aRM.on_message)

registerClientEventHandler("Intermission", on_intermission)

print "\tAuto messages plugin now active!"
