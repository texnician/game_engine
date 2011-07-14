#ifndef _OBJECT_HANDLER_
#define _OBJECT_HANDLER_

#include "define.h"
#include "log.h"
#include "object_ptr_table.h"

ObjectId GetUniqObjectId()
{
    static ObjectId id = 0;
    return id++;
}

class DLL_API HandlableObject
{
public:
    template<typename T>
    friend class ObjectHandler;
    friend class ObjectPtrTable;
    
    HandlableObject()
        {
            object_id_ = GetUniqObjectId();
            slot_idx_ = ObjectPtrTable::Instance()->GetSlotIndex(*this);
        }

    virtual ~HandlableObject()
        {
            ObjectPtrTable::Instance()->FreeSlotIndex(*this);
        }
    
private:
    ObjectId object_id_;
    ObjectSlotIdx slot_idx_;
};

template<typename T>
class ObjectHandler
{
public:
    explicit ObjectHandler(T& obj)
        {
            obj_id_ = obj.object_id_;
            slot_idx_ = obj.slot_idx_;
        }

    ObjectHandler(const ObjectHandler<T>& rhs)
        {
            obj_id_ = rhs.obj_id_;
            slot_idx_ = rhs.slot_idx_;
        }

    ObjectHandler& operator= (const ObjectHandler<T>& rhs)
        {
            obj_id_ = rhs.obj_id_;
            slot_idx_ = rhs.slot_idx_;
            return *this;
        }

    T* ToObject() const
        {
            T* p_obj = static_cast<T*>(ObjectPtrTable::Instance()->Find(slot_idx_));
            if (p_obj && p_obj->object_id_ == obj_id_) {
                return p_obj;
            }
            else {
                LOG(L_WARNING, "Invalid Object Handler %d:%d", obj_id_, slot_idx_);
                return NULL;
            }
        }
    
    friend bool operator== (const ObjectHandler<T>& lhs, const ObjectHandler<T>& rhs);
    
private:
    ObjectId obj_id_;
    ObjectSlotIdx slot_idx_;
};

template<typename T>
inline bool operator== (const ObjectHandler<T>& lhs, const ObjectHandler<T>& rhs)
{
    return lhs.obj_id_ == rhs.obj_id_ && lhs.slot_idx_ == rhs.slot_idx_;
}

#endif
