#include <Python.h>
#include "py_interface.h"
#include "object_handler.h"

class TestObj : public HandlableObject
{
public:
    int x;
    int y;
};

int main(int argc, char **argv)
{
    Py_Initialize();
    PyImport_AppendInittab("spam", &PyInit_spam);

    PyRun_SimpleString("from time import time,ctime\n"
                       "print('Today is', ctime(time()))\n");
    PyRun_SimpleString("import spam\n"
                       "xx = spam.SpamCall('abcd')\n"
                       "print(xx)\n");
    PyRun_SimpleString("with open('stackless.py') as ss: exec(ss.read())");
    Py_Finalize();

    ObjectPtrTable::Instance()->Init(10);
    
    TestObj stack_obj;
    TestObj *p_obj = new TestObj;
    ObjectHandler<TestObj> hda(stack_obj);
    ObjectHandler<TestObj> hdb(*p_obj);
    TestObj *stack_obj_ptr = hda.ToObject();
    TestObj *heap_obj_ptr = hdb.ToObject();
    delete p_obj;
    p_obj = new TestObj;
    ObjectHandler<TestObj> hdc(*p_obj);
    heap_obj_ptr = hdb.ToObject();
    if (hdc == hdb) {
        return false;
    }
    else {
        TestObj *heap_obj_ptr_c = hdc.ToObject();
    }
    return 0;
}
