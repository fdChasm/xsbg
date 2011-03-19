from structure import manager_base, extension_base
import sbclient

class Player:
	def __init__(self, cn):
		self.delete = False
		self.cn = cn
   
	def name(self):
		return sbclient.clientname(self.cn)

	def team(self):
		return sbclient.clientteam(self.cn)

	def weapon(self):
		return sbclient.getweapon(self.cn)

	def frags(self):
		return sbclient.getfrags(self.cn)

	def teamkills(self):
		return sbclient.getteamkills(self.cn)

	def flagsscored(self):
		return sbclient.getflagsscored(self.cn)

	def flagsdropped(self):
		return sbclient.getflagsdropped(self.cn)
		
	def flagsstolen(self):
		return sbclient.getflagsstolen(self.cn)

	def ping(self):
		return sbclient.getping(self.cn)

	def pj(self):
		return sbclient.getpj(self.cn)
		
	def ismaster(self):
		return sbclient.ismaster(self.cn)
		
	def isadmin(self):
		return sbclient.isadmin(self.cn)
		
	def isai(self):
		return sbclient.isai(self.cn)
		
	def isspectator(self):
		return sbclient.isspectator(self.cn)

class player_manager(manager_base):
	"""Class which handles the creation and extending of player objects"""
	def __init__(self):
		def getkey(p):
			return p.cn
		manager_base.__init__(self, Player, getkey)
	
	def getplayer(self, cn):
		try:
			return self.storage[cn]
		except:
			return None

PlayerManager = player_manager()

def player(cn):
	return PlayerManager.getplayer(cn)

class player_extension(extension_base):
	"""The "extension" to the player object which provides it's base functionality"""
	def __init__(self):
		self.events = {
			"connect": self.on_connect,
			"disconnect": self.on_disconnect
		}
	
	def on_connect(self, cn):
		p = Player(cn)
		#print p, "initialized for", cn
		return p
		
	def on_disconnect(self, cn):
		p = Player(cn)
		p.delete = True
		return p
		
	def load(self, players):
		return True
		
	def unload(self, players):
		return False
		
def extendPlayers(extension_name, extension):
	PlayerManager.register_extension(extension_name, extension)
	
playerbase = player_extension()

extendPlayers("base", playerbase)

def AllCns():
	clients = sbclient.listclients(True)
	if clients == '':
		return []
	clients = clients.split(' ')
	return map(int, clients)
	
def AllPlayers():
	cns = AllCns()
	return map(sbclient.clientname, cns)
	
class demoinfo:
	def __init__(self, textline):
		self.number = int(textline.split('. ')[0])
		textline = textline.split('. ')[1]
		self.date = textline.split(': ')[0]
		textline = textline.split(': ')[1]
		modemapsize = textline.split(', ')
		self.mode = modemapsize[0]
		self.map = modemapsize[1]
		self.size = modemapsize[2]

