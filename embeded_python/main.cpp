#include <Python.h>
#include "py_interface.h"

int main(int argc, char **argv)
{
    Py_Initialize();
    PyImport_AppendInittab("spam", &PyInit_spam);

    PyRun_SimpleString("from time import time,ctime\n"
                       "print('Today is', ctime(time()))\n");
    PyRun_SimpleString("import spam\n"
                       "xx = spam.SpamCall('abcd')\n"
                       "print(xx)\n");
    PyRun_SimpleString("exec(open('stackless.py').read())");
    // FILE * fp = fopen("stackless.py", "r");
    // if (fp == NULL)
    //     return 1;
    // PyRun_SimpleFile(fp, "stackless.py");
    Py_Finalize();
    return 0;
}
