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

%include "protocol.h"  

%template(GameObjectHandler) ObjectHandler<HandlableObject>;
%rename(HandlerEq) ::operator==(GameObjectHandler, GameObjectHandler);
%rename(HandlerNe) ::operator==(GameObjectHandler, GameObjectHandler);
