/*
 *   Copyright (C) 2009 Gregory Haynes <greg@greghaynes.net>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <Python.h> //we will need to figure out how to compile with the python directory automatically figured out in the future

#include "clientmodule.h"
#include "game.h"

#include <iostream>

namespace SbPy
{

    PyObject *sb_name(PyObject *self, PyObject *args)
    {
    	char *pname;
	    if(PyArg_ParseTuple(args, "s", &pname))
	    {
		    filtertext(game::player1->name, pname, false, MAXNAMELEN);
		    if(!game::player1->name[0]) copystring(game::player1->name, "unnamed");
		    game::addmsg(N_SWITCHNAME, "rs", game::player1->name);
		    return Py_True;
	    }
	    else
	    {
		    return Py_BuildValue("s", game::player1->name);
	    }
    }

    PyObject *sb_team(PyObject *self, PyObject *args)
    {
    	char *pteam;
	if(PyArg_ParseTuple(args, "s", &pteam))
	{
		if(game::player1->clientnum < 0) filtertext(game::player1->team, pteam, false, MAXTEAMLEN);
		else game::addmsg(N_SWITCHTEAM, "rs", pteam);
		return Py_True;
	}
	else
	{
		return Py_BuildValue("s", game::player1->team);
	}
    }
    
    PyObject *sb_getclientname(PyObject *self, PyObject *args)
    {
	    int cn;
	    string name;
	    if(!PyArg_ParseTuple(args, "i", &cn))
		    return Py_None;
            fpsent *d = game::getclient(cn);
            if (!d)
            	return Py_None;
	    copystring(name, d->name);
	    return Py_BuildValue("s", name);
    }
    
    PyObject *sb_getclientteam(PyObject *self, PyObject *args)
    {
	    int *cn;
	    if(!PyArg_ParseTuple(args, "i", &cn))
		    return Py_False;
	    return Py_BuildValue("s", game::getclientteam(*cn));
    }
    
    PyObject *sb_ismaster(PyObject *self, PyObject *args)
    {
	    int cn;
	    if(!PyArg_ParseTuple(args, "i", &cn))
		    return Py_None;
	    return game::ismaster(cn) ? Py_True : Py_False;
    }
    
    PyObject *sb_isadmin(PyObject *self, PyObject *args)
    {
	    int cn;
	    if(!PyArg_ParseTuple(args, "i", &cn))
		    return Py_None;
	    return game::isadmin(cn) ? Py_True : Py_False;
    }
    
    PyObject *sb_getmastermode(PyObject *self, PyObject *args)
    {
	    return Py_BuildValue("i", game::mastermode);
    }
    
    PyObject *sb_getmastermodename(PyObject *self, PyObject *args)
    {
	    int mm;
	    if(!PyArg_ParseTuple(args, "i", &mm))
		    return Py_None;
		mm++;
		if (mm < MM_AUTH || mm > MM_PASSWORD) return Py_None;
	    return Py_BuildValue("s", mastermodenames[mm]);
    }
    
    PyObject *sb_isspectator(PyObject *self, PyObject *args)
    {
	    int cn;
	    if(!PyArg_ParseTuple(args, "i", &cn))
		    return Py_None;
	    return game::isspectator(cn) ? Py_True : Py_False;
    }
    
    PyObject *sb_isai(PyObject *self, PyObject *args)
    {
	    int *cn;
	    int *type;
	    if(!PyArg_ParseTuple(args, "ii", &cn, &type))
		    return Py_None;
	    return game::isai(*cn, *type) ? Py_True : Py_False;
    }
    
    PyObject *sb_listclients(PyObject *self, PyObject *args)
    {
	    bool *local;
	    PyArg_ParseTuple(args, "b", &local);
	
        vector<char> buf;
        string cn;
        int numclients = 0;
        if(local)
        {
            formatstring(cn)("%d", game::player1->clientnum);
            buf.put(cn, strlen(cn));
            numclients++;
        }
        loopv(game::clients) if(game::clients[i])
        {
            formatstring(cn)("%d", game::clients[i]->clientnum);
            if(numclients++) buf.add(' ');
            buf.put(cn, strlen(cn));
        }
        buf.add('\0');

        return Py_BuildValue("s", buf.getbuf());
    }
	
    PyObject *sb_getmap(PyObject *self, PyObject *args)
    {
	    game::getmap();
	    return Py_None;
    }
	
    PyObject *sb_stopdemo(PyObject *self, PyObject *args)
    {
	    game::stopdemo();
	    return Py_None;
    }
	
    PyObject *sb_recorddemo(PyObject *self, PyObject *args)
    {
	    bool val;
	    if(!PyArg_ParseTuple(args, "b", &val))
		    return Py_False;
	    game::recorddemo(val);
	    return Py_True;
    }
    
    PyObject *sb_cleardemos(PyObject *self, PyObject *args)
    {
	    bool val;
	    if(!PyArg_ParseTuple(args, "b", &val))
		    return Py_False;
	    game::cleardemos(val);
	    return Py_True;
    }

    PyObject *sb_getdemo(PyObject *self, PyObject *args)
    {
	    int num;
	    if(!PyArg_ParseTuple(args, "i", &num))
		    return Py_False;
	    game::getdemo(num);
	    return Py_True;
    }

    PyObject *sb_listdemos(PyObject *self, PyObject *args)
    {
	    game::addmsg(N_LISTDEMOS, "r");
	    return Py_None;
    }

    PyObject *sb_sendmap(PyObject *self, PyObject *args)
    {
	    game::sendmap();
	    return Py_None;
    }
    
    PyObject *sb_gotoplayer(PyObject *self, PyObject *args)
    {
	    char *cn;
	    if(!PyArg_ParseTuple(args, "s", &cn))
		    return Py_False;
	    game::gotoplayer(cn);
	    return Py_True;
    }
    
    PyObject *sb_gotosel(PyObject *self, PyObject *args)
    {
	    game::gotosel();
	    return Py_None;
    }


    PyObject *sb_echo(PyObject *self, PyObject *args)
    {
    	int type;
    	char *msg;
	    if(!PyArg_ParseTuple(args, "is", &type, &msg))
		    return Py_False;
	    conline(type, msg);
	    return Py_None;
    }

    PyObject *sb_cs_exec(PyObject *self, PyObject *args)
    {
	    PyObject *pFile = PyTuple_GetItem(args, 0);
	    if(pFile)
	    {
		    char *file = PyString_AsString(pFile);
		    if(file)
			    execfile(file);
	    }
	    else
		    fprintf(stderr, "Error with filename");
	    Py_INCREF(Py_None);
	    return Py_None;
    }
    
    PyObject *sb_attack(PyObject *self, PyObject *args)
    {
	    bool val;
	    if(!PyArg_ParseTuple(args, "b", &val))
		    return Py_False;
	    game::doattack(val);
	    return Py_True;
    }
    
    PyObject *sb_jump(PyObject *self, PyObject *args)
    {
	    if(game::canjump()) game::player1->jumping = 1;
	    Py_INCREF(Py_None);
	    return Py_True;
    }

    bool execpyfile(const char *cfgfile, bool msg)
    {
        string s;
        copystring(s, cfgfile);
        char *buf = loadfile(path(s), NULL);
        if(!buf)
        {
            if(msg) conoutf(CON_ERROR, "could not read \"%s\"", cfgfile);
            return false;
        }
        PyRun_SimpleString(buf);
        delete[] buf;
        return true;
    }
    
    PyObject *sb_bind(PyObject *self, PyObject *args)
    {
	    PyObject *pkey = PyTuple_GetItem(args, 0);
	    PyObject *paction = PyTuple_GetItem(args, 1);
	    if(pkey && paction)
	    {
		    char *key = PyString_AsString(pkey);
		    char *action = PyString_AsString(paction);
		    if(key && action)
			    bindkey(key, action, keym::ACTION_DEFAULT, "bind");
	    }
	    else
		    fprintf(stderr, "Error Binding key: invalid arguments.");
	    Py_INCREF(Py_None);
	    return Py_None;
    }

    
    PyObject *sb_say(PyObject *self, PyObject *args)
    {
		char *text;
	    if(!PyArg_ParseTuple(args, "s", &text))
		    return Py_False;

    	conoutf(CON_CHAT, "%s:\f0 %s", game::colorname(game::player1), text);
    	game::addmsg(N_TEXT, "rcs", game::player1, text);
    	return Py_True;
    }
    
    PyObject *sb_sayteam(PyObject *self, PyObject *args)
    {
		char *text;
	    if(!PyArg_ParseTuple(args, "s", &text))
		    return Py_False;

    	conoutf(CON_TEAMCHAT, "%s:\f1 %s", game::colorname(game::player1), text); 
    	game::addmsg(N_SAYTEAM, "rcs", game::player1, text); 
    	return Py_True;
    }
    
    PyObject *sb_auto_say(PyObject *self, PyObject *args)
    {
		char *text;
	    if(!PyArg_ParseTuple(args, "s", &text))
		    return Py_False;

    	conoutf(CON_CHAT, "\fs\f3Auto say:\fr %s:\f0 %s", game::colorname(game::player1), text);
    	game::addmsg(N_TEXT, "rcs", game::player1, text);
    	return Py_True;
    }
    
    PyObject *sb_auto_sayteam(PyObject *self, PyObject *args)
    {
		char *text;
	    if(!PyArg_ParseTuple(args, "s", &text))
		    return Py_False;

    	conoutf(CON_TEAMCHAT, "\fs\f3Auto sayteam:\fr %s:\f1 %s", game::colorname(game::player1), text); 
    	game::addmsg(N_SAYTEAM, "rcs", game::player1, text); 
    	return Py_True;
    }
    

    PyObject *sb_pybind(PyObject *self, PyObject *args)
    {
	    PyObject *pkey = PyTuple_GetItem(args, 0);
	    PyObject *paction = PyTuple_GetItem(args, 1);
	    if(pkey && paction)
	    {
		    char *key = PyString_AsString(pkey);
		    char *action = PyString_AsString(paction);
		    if(key && action)
			    bindkey(key, action, keym::ACTION_DEFAULT, "bind");
	    }
	    else
		    fprintf(stderr, "Error Binding key: invalid arguments.");
	    Py_INCREF(Py_None);
	    return Py_None;
    }
    
    PyObject *sb_mycn(PyObject *self, PyObject *args)
    {
	    return Py_BuildValue("i", game::player1->clientnum);
    }

    PyObject *sb_getweapon(PyObject *self, PyObject *args)
    {
	    int cn;
	    if(!PyArg_ParseTuple(args, "i", &cn))
		    return Py_False;
            fpsent *d = game::getclient(cn);
            if (!d)
            	return Py_None;
	    return Py_BuildValue("i", d->gunselect);
    }

    PyObject *sb_nextweapon(PyObject *self, PyObject *args)
    {
    	int dir;
    	bool force = true;
	    if(!PyArg_ParseTuple(args, "ib", &dir, &force))
		    return Py_False;
		    
        if(game::player1->state!=CS_ALIVE) return Py_False;
        dir = (dir < 0 ? NUMGUNS-1 : 1);
        int gun = game::player1->gunselect;
        loopi(NUMGUNS)
        {
            gun = (gun + dir)%NUMGUNS;
            if(force || game::player1->ammo[gun]) break;
        }
        if(gun != game::player1->gunselect) 
        {
        	game::gunselect(gun, game::player1);
        	return Py_True;
        }
        else
        {
        	playsound(S_NOAMMO);
        	return Py_False;
        }
    }

    PyObject *sb_setweapon(PyObject *self, PyObject *args)
    {
    	int gun;
    	bool force = true;
	    if(!PyArg_ParseTuple(args, "ib", &gun, &force))
		    return Py_False;
		if(game::player1->state!=CS_ALIVE || gun<GUN_FIST || gun>GUN_PISTOL) return Py_False;
		if(force || game::player1->ammo[gun])
		{
			game::gunselect(gun, game::player1);
			return Py_True;
		}
		else 
		{
			playsound(S_NOAMMO);
			return Py_False;
		}
    }
    
    PyObject *sb_getammolevels(PyObject *self, PyObject *args)
    {        
		int ammo;
		PyObject *pTuple = PyTuple_New(NUMGUNS);
		PyObject *pInt;
		loopi(NUMGUNS)
		{
			ammo = game::player1->ammo[i];
			pInt = PyInt_FromLong(ammo);
			PyTuple_SetItem(pTuple, i, pInt);
		}
		return pTuple;
        
    }
    
    PyObject *sb_setmaster(PyObject *self, PyObject *args)
    {
        char *text;
        if(!PyArg_ParseTuple(args, "s", &text))
            return Py_False;
    	game::setmaster(text);
    	return Py_True;
    }
    
    PyObject *sb_setmark(PyObject *self, PyObject *args)
    {
	    int cn;
	    bool value;
	    if(!PyArg_ParseTuple(args, "ib", &cn, &value))
		    return Py_False;
            fpsent *d = game::getclient(cn);
            if (!d)
            	return Py_None;
	    d->marked = value;
	    return Py_True;
    }
    
    PyObject *sb_markcolor(PyObject *self, PyObject *args)
    {
    	int cn;
	    int color;
	    if(!PyArg_ParseTuple(args, "ii", &cn, &color))
		    return Py_False;
            fpsent *d = game::getclient(cn);
            if (!d)
            	return Py_False;
	    d->markcolor = color;
	    return Py_True;
    }

//    static PyObject *configdir(PyObject *self, PyObject *args)
//    {
//	    return Py_BuildValue("s", server::pyconfigpath);
//    }

    static PyMethodDef ModuleMethods[] = {
        //Methods for game engine
            {"echo", sb_echo, METH_VARARGS, "Echo text from python onto the sauerbraten console."},
            {"cs_exec", sb_cs_exec, METH_VARARGS, "Run a cubescript file."},
            {"bind", sb_bind, METH_VARARGS, "bind a key"},
            {"pybind", sb_pybind, METH_VARARGS, "bind a python function to a key"},

        //Methods for local player
            {"jump", sb_jump, 0, "Jump."},
            {"attack", sb_attack, 1, "Attack."},
            {"name", sb_name, METH_VARARGS, "get/set player name"},
            {"team", sb_team, METH_VARARGS, "get/set player team"},
            {"nextweapon", sb_nextweapon, METH_VARARGS, "Cycle to next weapon"},
            {"setweapon", sb_setweapon, METH_VARARGS, "Set the players weapon"},
            {"getammo", sb_getammolevels, METH_VARARGS, "Get ammo levels"},
            {"cn", sb_mycn, METH_VARARGS, "Get the local players cn"},
            {"say", sb_say, METH_VARARGS, "say a message"},
            {"sayteam", sb_sayteam, METH_VARARGS, "say a message to your teammates"},
            {"autosay", sb_auto_say, METH_VARARGS, "say a message"},
            {"autosayteam", sb_auto_sayteam, METH_VARARGS, "say a message to your teammates"},
            {"setmaster", sb_setmaster, METH_VARARGS, "setmaster command"},
        
        //Methods for server
            {"getmastermode", sb_getmastermode, METH_VARARGS, "get the mastermode"},
            {"getmastermodename", sb_getmastermodename, METH_VARARGS, "get the mastermode name"},
            {"getmap", sb_getmap, METH_VARARGS, ""},
            {"stopdemo", sb_stopdemo, METH_VARARGS, ""},
            {"recorddemo", sb_recorddemo, METH_VARARGS, ""},
            {"cleardemos", sb_cleardemos, METH_VARARGS, ""},
            {"getdemo", sb_getdemo, METH_VARARGS, ""},
            {"listdemos", sb_listdemos, METH_VARARGS, ""},
            {"sendmap", sb_sendmap, METH_VARARGS, ""},
        
        //Methods for all players
            {"clientname", sb_getclientname, METH_VARARGS, "get a clients name by cn"},
            {"clientteam", sb_getclientteam, METH_VARARGS, "get a clients team by cn"},
            {"ismaster", sb_ismaster, METH_VARARGS, "get whether a player is master"},
            {"isadmin", sb_isadmin, METH_VARARGS, "get whether a player is admin"},
            {"isspectator", sb_isspectator, METH_VARARGS, "get whether a player is a spectator"},
            {"isai", sb_isai, METH_VARARGS, "get whether a player is an ai"},
            {"listclients", sb_listclients, METH_VARARGS, "get a list of client cn's"},
            {"gotoplayer", sb_gotoplayer, METH_VARARGS, ""},
            {"gotoselection", sb_gotosel, METH_VARARGS, ""},
            {"getweapon", sb_getweapon, METH_VARARGS, "Get the players weapon"},
            {"setmark", sb_setmark, METH_VARARGS, "Change a players mark"},
            {"markcolor", sb_markcolor, METH_VARARGS, "Add a player marker"},

	    {NULL, NULL, 0, NULL}
    };

    PyMODINIT_FUNC
    initModule(const char *module_name)
    {
	    (void) Py_InitModule(module_name, ModuleMethods);
	    return;
    }


}
