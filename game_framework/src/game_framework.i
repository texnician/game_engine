%module gf

#define DLL_API __declspec(dllexport)

%{
#include "object_ptr_table.h"    
#include "object_handler.h"
#include "protocol.h"  
%}
%include <windows.i>
%include "object_handler.h"
%include "object_ptr_table.h"
%include <pybuffer.i>
%pybuffer_mutable_string(char *out_buf);
%pybuffer_string(char *in_buf);

//%typemap(in) unsigned char [ANY] = char [ANY];
//%typemap(out) unsigned char [ANY] = char [ANY];
//%apply int [ANY] { unsigned char [ANY] };
//%apply const int [ANY] { const unsigned char [ANY] };
// %include carrays.i
// %pybuffer_mutable_string(char *outMemBuf);
// %pybuffer_string(const char *inMemBuf);

// %array_functions(unsigned char, uchar_array)
// //%apply unsigned char *OUTPUT { unsigned char *outArrayBuf };

// %inline %{ 
//   size_t uchar_array_itemsize() { return sizeof(unsigned char); }
  
//   PyObject* uchar_array_from_buffer(unsigned char *outArrayBuf, int outArrayItemNumber,
//                                     const char *inMemBuf, int inMemBufSize)
//   {
//     if (outArrayItemNumber * sizeof(unsigned char) >= inMemBufSize) {
//         memcpy(outArrayBuf, inMemBuf, inMemBufSize);
//         return PyInt_FromLong(inMemBufSize);
//     }
//     else {
//         return Py_None;
//     }
//   }
  
//   PyObject* uchar_array_to_buffer(char *outMemBuf, int outMemBufSize,
//                                   unsigned char *inArrayBuf, int inArrayItemNumber)
//   {
//       size_t in_array_size = sizeof(unsigned char) * inArrayItemNumber;
//       if (outMemBufSize >= in_array_size) {
//           memcpy(outMemBuf, inArrayBuf, in_array_size);
//           return PyInt_FromLong(in_array_size);
//       }
//       else {
//           return Py_None;
//       }
//   }
// %}

%include "protocol.h"  

%template(GameObjectHandler) ObjectHandler<HandlableObject>;
%rename(HandlerEq) ::operator==(GameObjectHandler, GameObjectHandler);
%rename(HandlerNe) ::operator==(GameObjectHandler, GameObjectHandler);
