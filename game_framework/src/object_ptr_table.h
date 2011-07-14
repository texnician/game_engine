#ifndef _OBJECT_PTR_TABLE_
#define _OBJECT_PTR_TABLE_

#include "define.h"

typedef unsigned int ObjectId;
typedef size_t ObjectSlotIdx;
class HandlableObject;

class DLL_API ObjectPtrTable
{
    ObjectPtrTable();
    ObjectPtrTable(const ObjectPtrTable&);
    ~ObjectPtrTable();
    
public:
    bool Init(size_t max_size);

    bool Release();

    size_t Capacity() const;

    size_t Size() const;

    size_t FreeSize() const;
    
    HandlableObject* Find(ObjectSlotIdx idx) const;

    ObjectSlotIdx GetSlotIndex(HandlableObject& obj);

    size_t FreeSlotIndex(HandlableObject& obj);
    
    static ObjectPtrTable* Instance();

private:
    struct Slot
    {
        Slot()
            : empty(true), ptr(0)
            {}
        bool empty;
        uintptr_t ptr;
    };

    static size_t max_size_;
    static size_t free_size_;
    static ObjectSlotIdx head_;
    static ObjectPtrTable* instance_;
    static Slot* table_;
};

#endif
