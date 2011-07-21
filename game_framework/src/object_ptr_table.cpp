#include "log.h"
#include "object_ptr_table.h"
#include "object_handler.h"

ObjectPtrTable* ObjectPtrTable::instance_ = NULL;
size_t ObjectPtrTable::max_size_ = 0;
size_t ObjectPtrTable::free_size_ = 0;
ObjectSlotIdx ObjectPtrTable::head_ = -1;
ObjectPtrTable::Slot* ObjectPtrTable::table_ = 0;

ObjectId GetUniqObjectId()
{
    static ObjectId id = 0;
    return id++;
}

ObjectPtrTable::ObjectPtrTable()
{}

ObjectPtrTable* ObjectPtrTable::Instance()
{
    if (NULL == instance_) {
        instance_ = new ObjectPtrTable;
    }
    return instance_;
}

bool ObjectPtrTable::Init(size_t max_size)
{
    if (NULL == table_) {
        table_ = new Slot[max_size];
        for (size_t i = 0; i < max_size; ++i) {
            table_[i].empty = true;
            table_[i].ptr = i+1;
        }
        max_size_ = max_size;
        free_size_ = max_size;
        head_ = 0;
        return true;
    }
    else {
        LOG(L_ERROR, "ObjectPtrTable already initialized!");
        return false;
    }
}

bool ObjectPtrTable::Release()
{
    head_ = -1;
    free_size_ = 0;
    max_size_ = 0;
    delete [] table_;
    return true;
}

size_t ObjectPtrTable::Capacity() const
{
    return max_size_;
}

size_t ObjectPtrTable::Size() const
{
    return max_size_ - free_size_;
}

size_t ObjectPtrTable::FreeSize() const
{
    return free_size_;
}

HandlableObject* ObjectPtrTable::Find(ObjectSlotIdx idx) const
{
    if (idx < max_size_ && idx >= 0) {
        assert(idx < max_size_ && "idx > max_size");
        Slot& slot = table_[idx];
        return slot.empty ? NULL : (HandlableObject*)(slot.ptr);
    }
    else {
        LOG(L_WARNING, "idx = %d, max_size_ = %d", idx, max_size_);
        return NULL;
    }
}

ObjectSlotIdx ObjectPtrTable::GetSlotIndex(HandlableObject& obj)
{
    assert(free_size_ > 0 && "ObjectPtrtable is full!");
    // Get first free slot
    Slot& free_head = table_[head_];
    assert(free_head.empty && "free_head is not empty!");
    ObjectSlotIdx result = head_;

    // set head to next slot
    head_ = free_head.ptr;

    // Set free slot
    free_head.empty = false;
    free_head.ptr = (uintptr_t)&obj;

    // Update free size
    --free_size_;
    
    // return assigned slot index
    return result;
}

size_t ObjectPtrTable::FreeSlotIndex(HandlableObject& obj)
{
    assert(obj.slot_idx_ < max_size_ && "Invalid object idx");
    Slot& obj_slot = table_[obj.slot_idx_];
    assert(!obj_slot.empty && obj_slot.ptr == (uintptr_t)(&obj));

    // (setf free_list (cons obj_slot free_list))
    obj_slot.empty = true;
    obj_slot.ptr = head_;
    head_ = obj.slot_idx_;

    // Update free size
    return ++free_size_;
}
