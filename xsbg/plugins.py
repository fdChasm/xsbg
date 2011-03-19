from ConfigParser import ConfigParser, NoOptionError
import os, sys, __builtin__
import glob, imp
from os.path import join, basename, splitext
import sbclient

# Initialize these before loading plugins
import xsbg.events
import xsbg.interface
import xsbg.players
#import xsbg.log

plugins = {}

def plugin(name):
	return plugins[name]

def loadPlugins():
	plugins = importPluginModulesIn('plugins')
 
def importPluginModulesIn(directory):
    modules = {}
    print "Loading plugins:"
    for path in glob.glob(join(directory,'[!_]*.py')): # list .py files not starting with '_'
        name, ext = splitext(basename(path))
        modules[name] = imp.load_source(name, path)
    return modules

