#include <cstdio>
#include <windows.h>
#include "pythonDefs.h"

using namespace std;

PyAPI_FUNC(int) (* _PyRun_SimpleStringFlags)(const char *, PyCompilerFlags *);
PyAPI_FUNC(PyObject *) (*_PyImport_ImportModule)(const char *name);
PyAPI_FUNC(PyObject *) (*_PyDict_GetItemString)(PyObject *dp, const char *key);
PyAPI_FUNC(PyObject *) (*_PyModule_GetDict)(PyObject *);
PyAPI_FUNC(void) (*_Py_Initialize)(void);
PyAPI_FUNC(void) (*_Py_Finalize)(void);
PyAPI_FUNC(int) (*_Py_IsInitialized)(void);
PyAPI_FUNC(PyObject *) (*_PyTuple_New)(Py_ssize_t size);
PyAPI_FUNC(Py_ssize_t) (*_PyTuple_Size)(PyObject *);
PyAPI_FUNC(PyObject *) (*_PyTuple_GetItem)(PyObject *, Py_ssize_t);
PyAPI_FUNC(int) (*_PyTuple_SetItem)(PyObject *, Py_ssize_t, PyObject *);
PyAPI_FUNC(Py_ssize_t) (*_PyList_Size)(PyObject *);
PyAPI_FUNC(PyObject *) (*_PyList_GetItem)(PyObject *, Py_ssize_t);
PyAPI_FUNC(PyObject *) (*_Py_BuildValue)(const char *, ...);
PyAPI_FUNC(PyObject *) (*_PyEval_CallObjectWithKeywords)(PyObject *func, PyObject *args, PyObject *kwargs);
PyAPI_FUNC(double) (*_PyFloat_AsDouble)(PyObject *);
PyAPI_FUNC(int) (*_PyObject_IsTrue)(PyObject *);


PyAPI_FUNC(PyObject *) (*_PyObject_CallFunction)(PyObject *callable_object,const char *format, ...);

struct AddressResolveStruct
{
	void *varAddress;
	const char *varName;
};

HMODULE python3DLLModule = nullptr;
HMODULE python36DLLModule = nullptr;


bool pythonInitialize()
{
	const AddressResolveStruct funcsToResolve[] = {
		{ &_PyRun_SimpleStringFlags,"PyRun_SimpleStringFlags" },
		{ &_PyImport_ImportModule,"PyImport_ImportModule" },
		{ &_PyDict_GetItemString,"PyDict_GetItemString" },
		{ &_PyModule_GetDict,"PyModule_GetDict" },
		{ &_Py_Initialize,"Py_Initialize" },
		{ &_Py_Finalize,"Py_Finalize" },
		{ &_Py_IsInitialized,"Py_IsInitialized" },
		{ &_PyTuple_New,"PyTuple_New" },
		{ &_PyTuple_Size,"PyTuple_Size" },
		{ &_PyTuple_GetItem,"PyTuple_GetItem" },
		{ &_PyTuple_SetItem,"PyTuple_SetItem" },
		{ &_PyList_Size,"PyList_Size" },
		{ &_PyList_GetItem,"PyList_GetItem" },
		{ &_Py_BuildValue,"Py_BuildValue"},
		{ &_PyEval_CallObjectWithKeywords,"PyEval_CallObjectWithKeywords" },
		{ &_PyObject_CallFunction,"PyObject_CallFunction" },
		{ &_PyFloat_AsDouble,"PyFloat_AsDouble" },
		{ &_PyObject_IsTrue,"PyObject_IsTrue" },

	};


	python3DLLModule = LoadLibrary(L"pythonDll/python3.dll");
	python36DLLModule = LoadLibrary(L"pythonDll/python36.dll");

	if (python3DLLModule&&python36DLLModule)
	{
		printf("python dll load success!\n");

		for (auto toResolve : funcsToResolve)
		{
			*(void **)(toResolve.varAddress) = nullptr;
			auto pFunc = GetProcAddress(python36DLLModule, toResolve.varName);
			if (!pFunc)
			{
				auto pFunc = GetProcAddress(python3DLLModule, toResolve.varName);
			}
			if (!pFunc)
			{
				printf("function %s don't exist!\n", toResolve.varName);
				return false;
			}
			*(decltype(&pFunc))(toResolve.varAddress) = pFunc;
		}
	}
	return true;
}

void pythonFinalize()
{
	if (python3DLLModule&&python36DLLModule)
	{
		FreeLibrary(python36DLLModule);
		FreeLibrary(python3DLLModule);
		python3DLLModule = nullptr;
		python36DLLModule = nullptr;
	}

}
