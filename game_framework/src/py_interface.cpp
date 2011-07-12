#include "py_interface.h"

static PyObject* SpamCall(PyObject *self, PyObject *args)
{
    const char *input;
    int sts;
    if (!PyArg_ParseTuple(args, "s", &input)) {
        return NULL;
    }
    std::string s(input);
    std::reverse(s.begin(), s.end());
    return PyUnicode_FromFormat("%s", s.c_str());
}

static PyMethodDef SpamMethods[] = {
    {"SpamCall",  SpamCall, METH_VARARGS,
     "Reverse input string."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

const char *spam_doc = "A spam module.";

static struct PyModuleDef spammodule = {
    PyModuleDef_HEAD_INIT,
    "spam",   /* name of module */
    spam_doc, /* module documentation, may be NULL */
    -1,       /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
    SpamMethods
};

PyMODINIT_FUNC
PyInit_spam(void)
{
    return PyModule_Create(&spammodule);
}
