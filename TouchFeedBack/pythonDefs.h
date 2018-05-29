#pragma once

#define PyAPI_FUNC(RTYPE) RTYPE

using PyCompilerFlags = void;
using PyObject = void;
using Py_ssize_t = long long;


extern PyAPI_FUNC(int) (*_PyRun_SimpleStringFlags)(const char *, PyCompilerFlags *);
extern PyAPI_FUNC(PyObject *) (*_PyImport_ImportModule)(const char *name);
extern PyAPI_FUNC(PyObject *) (*_PyDict_GetItemString)(PyObject *dp, const char *key);
extern PyAPI_FUNC(PyObject *) (*_PyModule_GetDict)(PyObject *);
extern PyAPI_FUNC(void) (*_Py_Initialize)(void);
extern PyAPI_FUNC(void) (*_Py_Finalize)(void);
extern PyAPI_FUNC(int) (*_Py_IsInitialized)(void);
extern PyAPI_FUNC(PyObject *) (*_PyTuple_New)(Py_ssize_t size);
extern PyAPI_FUNC(Py_ssize_t) (*_PyTuple_Size)(PyObject *);
extern PyAPI_FUNC(PyObject *) (*_PyTuple_GetItem)(PyObject *, Py_ssize_t);
extern PyAPI_FUNC(int) (*_PyTuple_SetItem)(PyObject *, Py_ssize_t, PyObject *);
extern PyAPI_FUNC(Py_ssize_t) (*_PyList_Size)(PyObject *);
extern PyAPI_FUNC(PyObject *) (*_PyList_GetItem)(PyObject *, Py_ssize_t);
extern PyAPI_FUNC(PyObject *) (*_Py_BuildValue)(const char *, ...);
extern PyAPI_FUNC(PyObject *) (*_PyEval_CallObjectWithKeywords)(
	PyObject *func, PyObject *args, PyObject *kwargs);
extern PyAPI_FUNC(PyObject *) (*_PyObject_CallFunction)(PyObject *callable_object,
	const char *format, ...);
extern PyAPI_FUNC(double) (*_PyFloat_AsDouble)(PyObject *);
extern PyAPI_FUNC(int) (*_PyObject_IsTrue)(PyObject *);



#define PyRun_SimpleStringFlags(/*const char **/cmdStr, /*PyCompilerFlags **/ flag) _PyRun_SimpleStringFlags(cmdStr,flag)
#define PyImport_ImportModule(/*const char **/name) _PyImport_ImportModule(name)
#define PyDict_GetItemString(/*PyObject **/dp, /*const char **/key) _PyDict_GetItemString(dp,key)
#define PyModule_GetDict(/*PyObject **/dict) _PyModule_GetDict(dict)
#define Py_Initialize() _Py_Initialize()
#define Py_Finalize() _Py_Initialize()
#define Py_IsInitialized() _Py_IsInitialized() 
#define PyTuple_New(/*Py_ssize_t*/ size) _PyTuple_New(size);
#define PyTuple_Size(/*PyObject **/tuple) _PyTuple_Size(tuple)
#define PyTuple_GetItem(/*PyObject **/tuple, /*Py_ssize_t*/ index) _PyTuple_GetItem(tuple,index)
#define PyTuple_SetItem(/*PyObject **/tuple, /*Py_ssize_t*/ index,/* PyObject **/ obj) _PyTuple_SetItem(tuple,index,obj)
#define PyList_Size(/*PyObject **/ list) _PyList_Size(list)
#define PyList_GetItem(/*PyObject **/list, /*Py_ssize_t*/index) _PyList_GetItem(list,index)
#define Py_BuildValue(fmt, ...) _Py_BuildValue(fmt,__VA_ARGS__)
#define PyObject_CallFunction(/*PyObject **/callable_object,/*const char **/format, ...) _PyObject_CallFunction(callable_object, format, __VA_ARGS__)
#define PyFloat_AsDouble(/*PyObject **/floatObj) _PyFloat_AsDouble(floatObj)
#define PyObject_IsTrue(/*PyObject **/obj) _PyObject_IsTrue(obj)

#define PyRun_SimpleString(s) PyRun_SimpleStringFlags(s, NULL)
#define PyEval_CallObject(func,arg) _PyEval_CallObjectWithKeywords(func, arg, (PyObject *)NULL)


bool pythonInitialize();
void pythonFinalize();
