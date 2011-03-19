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

#include "sbpy.h"
//#include "server.h"

#include <string>
#include <iostream>

#ifdef _WIN32
#include <direct.h> // for _chdir()
#define chdir _chdir
#endif

extern int totalmillis;

namespace SbPy
{

static char *pyscripts_path;
static char *pn;

PyMODINIT_FUNC initModule(const char *);

void loadPyscriptsPath()
{
	char *path = getenv("SB_PYSCRIPTS_PATH");
	if(!path)
		pyscripts_path = path;
}

void initEnv(char *homedir)
{
	std::string newpath;
	const char *pp;
	if(!pyscripts_path)
		return;
       	pp = getenv("PYTHONPATH");
	if(!pp)
	{
		newpath = pyscripts_path;
	}
	else
	{
		newpath = pp;
		newpath.append(":");
		newpath.append(pyscripts_path);
	}
	newpath.append(":");
	newpath.append(homedir);
#ifndef _WIN32
	setenv("PYTHONPATH", newpath.c_str(), 1);
#endif
}

#define SBPY_ERR(x) \
	if(!x) \
	{ \
		if(PyErr_Occurred()) \
			PyErr_Print(); \
		return false;\
	}

static PyObject *eventsModule, *triggerEventFunc, *triggerPolicyEventFunc, *updateFunc;

PyThreadState * tstate = NULL;


bool initPy()
{
	PyObject *pFunc = 0, *pArgs = 0, *pValue = 0, *pluginsModule = 0, *autoexecModule = 0;
	
	eventsModule = PyImport_ImportModule("xsbg.events");
	SBPY_ERR(eventsModule)
	triggerEventFunc = PyObject_GetAttrString(eventsModule, "triggerClientEvent");
	SBPY_ERR(triggerEventFunc);
	if(!PyCallable_Check(triggerEventFunc))
	{
		fprintf(stderr, "Error: triggerEvent function could not be loaded.\n");
		return false;
	}
	triggerPolicyEventFunc = PyObject_GetAttrString(eventsModule, "triggerPolicyEvent");
	SBPY_ERR(triggerPolicyEventFunc);
	if(!PyCallable_Check(triggerPolicyEventFunc))
	{
		fprintf(stderr, "Error: triggerPolicyEvent function could not be loaded.\n");
		return false;
	}
	
	updateFunc = PyObject_GetAttrString(eventsModule, "update");
	SBPY_ERR(updateFunc);
	if(!PyCallable_Check(updateFunc))
	{
		fprintf(stderr, "Error: update function could not be loaded.\n");
		return false;
	}
	pluginsModule = PyImport_ImportModule("xsbg.plugins");
	SBPY_ERR(pluginsModule)
	pFunc = PyObject_GetAttrString(pluginsModule, "loadPlugins");
	SBPY_ERR(pFunc)
	if(!PyCallable_Check(pFunc))
	{
		fprintf(stderr, "Error: loadPlugins function could not be loaded.\n");
		return false;
	}
	
	pArgs = PyTuple_New(0);
	pValue = PyObject_CallObject(pFunc, pArgs);
	Py_DECREF(pArgs);
	Py_DECREF(pFunc);
	
	if(!pValue)
	{
		PyErr_Print();
		return false;
	}
	Py_DECREF(pValue);
	Py_DECREF(pluginsModule);
	
	//call the users autoexec.py file
	autoexecModule = PyImport_ImportModule("autoexec");
	SBPY_ERR(autoexecModule)
	
	return true;
}

void deinitPy()
{
	Py_XDECREF(triggerEventFunc);
	Py_XDECREF(triggerPolicyEventFunc);
	Py_XDECREF(updateFunc);
	Py_XDECREF(eventsModule);
	Py_Finalize();
}

bool init(const char *prog_name, const char *arg_pyscripts_path, char *homedir, const char *module_name)
{

	// Setup env vars and chdir
	if(!pn)
	{
		pn = new char[strlen(prog_name)+1];
		strcpy(pn, prog_name);
	}
	if(arg_pyscripts_path[0] || pyscripts_path)
	{
		if(!pyscripts_path)
		{
			pyscripts_path = new char[strlen(arg_pyscripts_path)+1];
			strcpy(pyscripts_path, arg_pyscripts_path);
		}
	}
	else loadPyscriptsPath();
	
	if(!pyscripts_path)
	{
		fprintf(stderr, "Fatal Error: Could not locate a pyscripts directory.\n");
		return false;
	}
	
	initEnv(homedir);
	
	if(-1 == chdir(pyscripts_path))
	{
		perror("Could not chdir into pyscripts path.\n");
		return false;
	}

	// Set program name
	Py_SetProgramName(pn);

	// Initialize
	Py_Initialize();
	
	// Initialize threads
	PyEval_InitThreads();
	
	initModule(module_name);
	
	tstate = PyEval_SaveThread();
	
	PyEval_RestoreThread(tstate);
	
	if(!initPy())
	{
		fprintf(stderr, "Error initializing python modules.\n");
		return false;
	}
	
	tstate = PyEval_SaveThread();
	
	
	
	//PyRun_SimpleString("print 'init: Python interpreter initialized'\n");
	return true;
}

PyObject *callPyFunc(PyObject *func, PyObject *args)
{
	PyObject *val;
	
	PyEval_RestoreThread(tstate);
	val = PyObject_CallObject(func, args);
	tstate = PyEval_SaveThread();
	
	Py_DECREF(args);
	if(!val)
		PyErr_Print();
	return val;
}

bool triggerFuncEvent(const char *name, std::vector<PyObject*> *args, PyObject *func)
{
	PyObject *pArgs, *pArgsArgs, *pName, *pValue;
	std::vector<PyObject*>::const_iterator itr;
	int i = 0;
	
	if(!func)
	{
		fprintf(stderr, "Python Error: Invalid handler to triggerEvent function.\n");
		return false;
	}
	pArgs = PyTuple_New(2);
	pName = PyString_FromString(name);
	SBPY_ERR(pName)
	PyTuple_SetItem(pArgs, 0, pName);
	if(args)
	{
		pArgsArgs = PyTuple_New(args->size());
		for(itr = args->begin(); itr != args->end(); itr++)
		{
			PyTuple_SetItem(pArgsArgs, i, *itr);
			i++;
		}
	}
	else
		pArgsArgs = PyTuple_New(0);
	PyTuple_SetItem(pArgs, 1, pArgsArgs);
	pValue = callPyFunc(func, pArgs);
	if(!pValue)
	{
		fprintf(stderr, "Error triggering event.\n");
		return false;
	}
	if(PyBool_Check(pValue))
	{
		bool ret = (pValue == Py_True);
		Py_DECREF(pValue);
		return ret;
	}
	Py_DECREF(pValue);
	return true;
}

#undef SBPY_ERR

bool triggerFuncEventInt(const char *name, int cn, PyObject *func)
{
	std::vector<PyObject*> args;
	PyObject *pCn = PyInt_FromLong(cn);
	args.push_back(pCn);
	return triggerFuncEvent(name, &args, func);
}

bool triggerFuncEventPyTuple(const char *name, PyObject *pTuple, PyObject *func)
{
	std::vector<PyObject*> args;
	args.push_back(pTuple);
	return triggerFuncEvent(name, &args, func);
}

bool triggerFuncEventString(const char *name, const char *str, PyObject *func)
{
	std::vector<PyObject*> args;
	PyObject *pCn = PyString_FromString(str);
	args.push_back(pCn);
	return triggerFuncEvent(name, &args, func);
}

bool triggerFuncEventIntBool(const char *name, int cn, bool b, PyObject *func)
{
	std::vector<PyObject*> args;
	PyObject *pCn = PyInt_FromLong(cn);
	PyObject *pB;
	if(b)
		pB = Py_True;
	else
		pB = Py_False;
	args.push_back(pCn);
	args.push_back(pB);
	
	return triggerFuncEvent(name, &args, func);
}

bool triggerFuncEventIntString(const char *name, int cn, const char *text, PyObject *func)
{
	std::vector<PyObject*> args;
	PyObject *pText = PyString_FromString(text);
	PyObject *pCn = PyInt_FromLong(cn);
	args.push_back(pCn);
	args.push_back(pText);
	return triggerFuncEvent(name, &args, func);
}

bool triggerFuncEventIntStringInt(const char *name, int cn, const char *text, int cn2, PyObject *func)
{
	std::vector<PyObject*> args;
	PyObject *pText = PyString_FromString(text);
	PyObject *pCn = PyInt_FromLong(cn);
	PyObject *pCn2 = PyInt_FromLong(cn2);
	args.push_back(pCn);
	args.push_back(pText);
	args.push_back(pCn2);
	return triggerFuncEvent(name, &args, func);
}

bool triggerFuncEventIntStringString(const char *name, int cn, const char *text, const char *text2, PyObject *func)
{
	std::vector<PyObject*> args;
	PyObject *pText2 = PyString_FromString(text2);
	PyObject *pText = PyString_FromString(text);
	PyObject *pCn = PyInt_FromLong(cn);
	args.push_back(pCn);
	args.push_back(pText);
	args.push_back(pText2);
	return triggerFuncEvent(name, &args, func);
}

bool triggerEvent(const char*name, std::vector<PyObject*>* args)
{
	return triggerFuncEvent(name, args, triggerEventFunc);
}

bool triggerEventPyTuple(const char *event_name, PyObject *pTuple)
{
	return triggerFuncEventPyTuple(event_name, pTuple, triggerEventFunc);
}

bool triggerEventInt(const char *name, int cn)
{
	return triggerFuncEventInt(name, cn, triggerEventFunc);
}

bool triggerEventIntBool(const char *name, int cn, bool b)
{
	return triggerFuncEventIntBool(name, cn, b, triggerEventFunc);
}

bool triggerEventStr(const char *name, const char *str)
{
	return triggerFuncEventString(name, str, triggerEventFunc);
}

bool triggerEventIntString(const char *name, int cn, const char *text)
{
	return triggerFuncEventIntString(name, cn, text, triggerEventFunc);
}

bool triggerEventIntStringInt(const char *name, int n, const char *str, int n2)
{
	return triggerFuncEventIntStringInt(name, n, str, n2, triggerEventFunc);
}

bool triggerEventIntStringString(const char *name, int cn, const char *text, const char *text2)
{
	return triggerFuncEventIntStringString(name, cn, text, text2, triggerEventFunc);
}

bool triggerEventIntInt(const char *name, int cn1, int cn2)
{
	std::vector<PyObject*> args;
	PyObject *pCn1 = PyInt_FromLong(cn1);
	PyObject *pCn2 = PyInt_FromLong(cn2);
	args.push_back(pCn1);
	args.push_back(pCn2);
	return triggerFuncEvent(name, &args, triggerEventFunc);
}

bool triggerEventIntIntString(const char *name, int cn1, int cn2, const char *text)
{
	std::vector<PyObject*> args;
	PyObject *pCn1 = PyInt_FromLong(cn1);
	PyObject *pCn2 = PyInt_FromLong(cn2);
	PyObject *pTxt = PyString_FromString(text);
	args.push_back(pCn1);
	args.push_back(pCn2);
	args.push_back(pTxt);
	return triggerFuncEvent(name, &args, triggerEventFunc);
}

bool triggerEventStrInt(const char *name, const char *str, int n)
{
	std::vector<PyObject*> args;
	PyObject *pstr, *pn;
	pstr = PyString_FromString(str);
	pn = PyInt_FromLong(n);
	args.push_back(pstr);
	args.push_back(pn);
	return triggerFuncEvent(name, &args, triggerEventFunc);
}

bool triggerPolicyEventInt(const char *name, int cn)
{
	return triggerFuncEventInt(name, cn, triggerPolicyEventFunc);
}

bool triggerPolicyEventIntString(const char *name, int cn, const char *text)
{
	return triggerFuncEventIntString(name, cn, text, triggerPolicyEventFunc);
}

void update()
{
	PyObject *pargs, *pvalue;
	pargs = PyTuple_New(0);
	pvalue = callPyFunc(updateFunc, pargs);
	if(pvalue)
		Py_DECREF(pvalue);
}

}
