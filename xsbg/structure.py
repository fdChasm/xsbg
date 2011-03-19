from events import registerClientEventHandler, isClientEventRegistered

class manager_base:
	"""Base class for building extensible object managers"""
	def __init__(self, managed_class, getkey):
		#define the class which is being managed
		self.managed_class = managed_class
		
		#since this has to work for any class we have to define how to get the key by which the objects will be indexed
		if callable(getkey):
			self.getkey = getkey
		else:
			self.success = False
	
		#This is the dictionary in which we store all objects, indexed by the key_type which can be retrieved by the
		#stored object using the getkey function
		self.storage = {}
	
		#The dictionary of extensions to the player object management
		self.extensions = {}
		
		#a two dimensional dictionary where each key is the event name to be handled
			#with the values being a dictionary where each key is the extension name and the value is the function
		self.events = {}
		
		self.success = True
	
	def register_extension(self, extension_name, extension):
		if extension.load(self.storage):
			if not (extension_name in self.extensions.keys()):
				self.extensions[extension_name] = extension
			
				for event in extension.events.keys():
					if not (event in self.events.keys()):
						self.events[event] = {}
					self.events[event][extension_name] = extension.events[event]
				self.update_event_registration()
				return True
			else:
				return False
			
	def unregister_extension(self, extension_name):
		if extension_name in self.extension.keys():
			if self.extensions[extension_name].unload(self.storage):
				del self.extensions[extension_name]
				
	def update_event_registration(self):
		for event in self.events.keys():
			if not isClientEventRegistered(event, self.on_event):
				registerClientEventHandler(event, self.on_event)
				
	def on_event(self, event_name, *args):
		if event_name in self.events.keys():
			for extension_name in self.events[event_name].keys():
				#print extension_name, event_name
				return_value = self.events[event_name][extension_name](*args)
				if return_value.__class__ == self.managed_class:
					key = self.getkey(return_value)
					if not (key in self.storage.keys()):
						self.storage[key] = return_value
						
					if return_value.delete:
						try:
							del self.storage[key]
						except:
							pass
							
class extension_base:
	"""Base class for an extension to the object manager"""
	#A dictionary of (event name: handler)
	events = {}
	
	#def on_<event name>(self, <event args>):
		#do something with the event (like add a property to the player)
		
	def load(self, players={}):
		pass
		#called on registering the extension
		#code to update existing player objects (if necessary)
		
		#Return whether loading of the extension is permitted
			#don't bother checking whether the extension 
			#is already loaded because they can only be loaded once anyway
			
			#make sure you're going to allow it to be loaded before updating the player objects
		
	def unload(self, players={}):
		pass
		#called on unregistering the extension
		#code to clean up player objects (if necessary)
		
		#Return whether unloading the extension is permitted
			#don't clean up the player objects if you're not going to allow the extension to be unloaded
			
"""
class extension_manager:
	""Template for a extension to the player class""
	#A dictionary of (event name: handler)
	self.events = 
	{
		<event name>: on_<event name>,
		...
	}
	
	def on_<event name>(self, <event args>):
		#do something with the event (like add a property to the player)
		p = player(cn)
		p.new_property = some_property
		
	def load(self, players={}):
		#called on registering the extension
		#code to update existing player objects (if necessary)
		
		#Return whether loading of the extension is permitted
			#don't bother checking whether the extension 
			#is already loaded because they can only be loaded once anyway
			
			#make sure you're going to allow it to be loaded before updating the player objects
		
	def unload(self, players={}):
		#called on unregistering the extension
		#code to clean up player objects (if necessary)
		
		#Return whether unloading the extension is permitted
			#don't clean up the player objects if you're not going to allow the extension to be unloaded
		
		
#For an example of the usage of player extensions see the bottom of this file where
#the basis for the player object class is loaded as an extension

#Note any registered event can create or delete a player object
	#To create a player object return it (just don't have a property called delete which evaluates to true)
	#To delete a player object return the player object with the property delete which evaluates to true

"""
