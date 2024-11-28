#ifndef DATASTORE_H
#define DATASTORE_H

#include <iostream>
#include <cstdlib>
#include <string>
#include <Python.h>

using namespace std;

string PATH="data/";

void Run_python(string s){
	PyRun_SimpleString(s.c_str());
	return;
}

struct DataStore{
	string name,conn,cursor;
	
	PyObject *pModule;
	PyObject *pDict;
	
	DataStore(string _name="data"){
		name=_name+".db";
		conn=_name+"_conn";
		cursor=_name+"_cursor";
		
		Py_Initialize();
		
		pModule=PyImport_ImportModule("__main__");
		pDict=PyModule_GetDict(pModule);
		
		Run_python("import sqlite3");
		Run_python(conn+"=sqlite3.connect('"+PATH+name+"',check_same_thread=False)");
		Run_python(cursor+"="+conn+".cursor()");
		Run_python(cursor+".execute('''CREATE TABLE IF NOT EXISTS dict (key TEXT PRIMARY KEY, value REAL)''')");
		
		Py_DECREF(pModule);
	}
	
	double Query(string key){
		pModule=PyImport_ImportModule("__main__");
		pDict=PyModule_GetDict(pModule);
		
		PyObject_SetAttrString(pModule,"value",PyFloat_FromDouble(0));
		
		Run_python(cursor+".execute(\"SELECT value FROM dict WHERE key=?\",('"+key+"',))");
		Run_python("result="+cursor+".fetchone()");
		Run_python("if result:value=result[0]");
		Run_python("if not result:"+cursor+".execute(\"INSERT INTO dict (key,value) VALUES (?,?)\",('"+key+"',0));"+conn+".commit()");
		
		PyObject *pValue=PyDict_GetItemString(pDict,"value");
		
		if(pValue!=nullptr){
	        if(PyFloat_Check(pValue)){
	        	double value=PyFloat_AsDouble(pValue);
	        	Py_DECREF(pModule);
	        	return value;
			}
	        else cerr<<"value 不是一个浮点数类型!"<<endl;
	    }
		else cerr<<"无法获取 value!"<<endl;
		
		Py_DECREF(pModule);
		return 0;
	}
	
	void Update(string key,double value){
		pModule=PyImport_ImportModule("__main__");
		pDict=PyModule_GetDict(pModule);
		
		Run_python(cursor+".execute(\"SELECT value FROM dict WHERE key=?\",('"+key+"',))");
		Run_python("result="+cursor+".fetchone()");
        Run_python("if result:"+cursor+".execute(\"UPDATE dict SET value=? WHERE key=?\",("+to_string(value)+",'"+key+"'))");
		Run_python("if not result:"+cursor+".execute(\"INSERT INTO dict (key,value) VALUES (?,?)\",('"+key+"',"+to_string(value)+"))");
		Run_python(conn+".commit()");
		
		Py_DECREF(pModule);
		return;
	}
};

#endif // DATASTORE_H
