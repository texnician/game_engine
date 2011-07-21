%module gf

#define DLL_API __declspec(dllexport)

%{
#include "object_ptr_table.h"    
#include "object_handler.h"
%}
%include <windows.i>
%include "object_handler.h"
%include "object_ptr_table.h"

%template(GameObjectHandler) ObjectHandler<HandlableObject>;
%rename(HandlerEq) ::operator==(GameObjectHandler, GameObjectHandler);
%rename(HandlerNe) ::operator==(GameObjectHandler, GameObjectHandler);
