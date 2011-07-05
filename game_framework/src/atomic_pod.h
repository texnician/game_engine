#ifndef _ATOMIC_POD_H_
#define _ATOMIC_POD_H_

// Try implement atomic plain-old-data lock-free operaton.
// Support following pod data
// atomic_bool     bool
// atomic_char     char
// atomic_schar    signed char
// atomic_uchar    unsigned char
// atomic_short    short
// atomic_ushort   unsigned short
// atomic_int      int
// atomic_uint     unsigned int
// atomic_long     long
// atomic_ulong    unsigned long
// atomic_llong    long long
// atomic_ullong   unsigned long long
// atomic_char16_t char16_t
// atomic_char32_t char32_t
// atomic_wchar_t  wchar_t

#include "define.h"

#if defined(_CXX0X_THREAD_)
// If has c++0x support, use std::atomic
#include <cstdatomic>

template<typename T>
struct atomic_pod
{
private:
    typedef T integral_type;
    std::atomic<integral_type> impl_;
    
public:
    typedef integral_type value_type;
    
    atomic_pod() = default;
    ~atomic_pod() = default;
    atomic_pod(integral_type v)
        : impl_(v) 
        {}
    atomic_pod(const atomic_pod&) = delete;
    atomic_pod<T>& operator=(const atomic_pod&) = delete;
    
    operator integral_type () const volatile
        {
            return impl_.load();
        }
    
    integral_type operator = (integral_type v)
        {
            impl_.store(v);
            return v;
        }
    
    bool is_lock_free() const volatile
        {
            return impl_.is_lock_free();
        }
    
    void store(integral_type v) volatile
        {
            impl_.store(v);
        }
    
    integral_type load() const volatile
        {
            return impl_.load();
        }

    integral_type
    operator++(int) volatile
        { return fetch_add(1); }

    integral_type
    operator--(int) volatile
        { return fetch_sub(1); }

    integral_type
    operator++() volatile
        { return ++impl_;}

    integral_type
    operator--() volatile
        { return --impl_; }

    integral_type
    operator+=(integral_type i) volatile
        { return impl_ += i; }

    integral_type
    operator-=(integral_type i) volatile
        { return impl_ -= i; }

    integral_type
    operator&=(integral_type i) volatile
        { return impl_ &= i; }

    integral_type
    operator|=(integral_type i) volatile
        { return impl_ |= i; }

    integral_type
    operator^=(integral_type i) volatile
        { return impl_ ^= i; }

    integral_type exchange(integral_type v) volatile
        {
            return impl_.exchange(v);
        }
    bool compare_exchange(integral_type& v1, integral_type v2) volatile
        {
            return impl_.compare_exchange_strong(v1, v2);
        }
    integral_type fetch_add(integral_type v) volatile
        {
            return impl_.fetch_add(v);
        }
    integral_type fetch_sub(integral_type v) volatile
        {
            return impl_.fetch_sub(v);
        }
    integral_type fetch_and(integral_type v) volatile
        {
            return impl_.fetch_and(v);
        }
    integral_type fetch_or(integral_type v) volatile
        {
            return impl_.fetch_or(v);
        }
    integral_type fetch_xor(integral_type v) volatile
        {
            return impl_.fetch_xor(v);
        }
};

template<>
struct atomic_pod<bool>
{
private:
    std::atomic<bool> impl_;

public:
    typedef bool value_type;
    
    atomic_pod() = default;
    ~atomic_pod() = default;
    atomic_pod(const atomic_pod&) = delete;
    atomic_pod& operator=(const atomic_pod&) = delete;

    atomic_pod(bool v)
        : impl_(v)
        {}

    bool operator = (bool v)
        {
            return impl_.operator=(v);
        }

    operator bool() const volatile
        {
            return impl_.load();
        }

    bool is_lock_free() const volatile
        {
            return impl_.is_lock_free();
        }

    void store(bool v) volatile
        {
            impl_.store(v);
        }
    
    bool load() const volatile
        {
            return impl_.load();
        }

    bool exchange(bool v) volatile
        {
            return impl_.exchange(v);
        }

    bool compare_exchange(bool& v1, bool v2) volatile
        {
            return impl_.compare_exchange_strong(v1, v2);
        }
};

#else  // no c++0x support
#if defined(OS_LINUX) || defined(OS_CYGWIN)
// If has gcc builtin atomic operation
#if defined(_GLIBCXX_ATOMIC_BUILTINS_1) && defined(_GLIBCXX_ATOMIC_BUILTINS_2) \
    && defined(_GLIBCXX_ATOMIC_BUILTINS_4) && defined(_GLIBCXX_ATOMIC_BUILTINS_8)
#define WITH_GCC_ATOMIC_BUILTIN_SUPPORT 1
#else
#undef WITH_GCC_ATOMIC_BUILTIN_SUPPORT
#endif  // GLIBCXX_ATOMIC_BUILTINS_*

#if defined(WITH_GCC_ATOMIC_BUILTIN_SUPPORT)

template<typename T>
struct atomic_pod
{
private:
    typedef T integral_type;
    integral_type pod_;

    atomic_pod(const atomic_pod&);
    atomic_pod& operator=(const atomic_pod&);
    
public:
    typedef integral_type value_type;
    
    atomic_pod()
        {}
    ~atomic_pod()
        {}
    atomic_pod(integral_type v)
        {
            pod_ = v;
        }

    operator integral_type () const volatile
        {
            return load();
        }
    integral_type operator = (integral_type v)
        {
            store(v);
            return v;
        }
    
    bool is_lock_free() const volatile
        {
            return true;
        }
    void store(integral_type v) volatile
        {
            pod_ = v;
            __sync_synchronize();
        }
    
    integral_type load() const volatile
        {
            __sync_synchronize();
            integral_type ret = pod_;
            __sync_synchronize();
            return ret;
        }

    integral_type
    operator++(int) volatile
        { return fetch_add(1); }

    integral_type
    operator--(int) volatile
        { return fetch_sub(1); }

    integral_type
    operator++() volatile
        { return __sync_add_and_fetch(&pod_, 1); }

    integral_type
    operator--() volatile
        { return __sync_sub_and_fetch(&pod_, 1); }

    integral_type
    operator+=(integral_type i) volatile
        { return __sync_add_and_fetch(&pod_, i); }

    integral_type
    operator-=(integral_type i) volatile
        { return __sync_sub_and_fetch(&pod_, i); }

    integral_type
    operator&=(integral_type i) volatile
        { return __sync_and_and_fetch(&pod_, i); }

    integral_type
    operator|=(integral_type i) volatile
        { return __sync_or_and_fetch(&pod_, i); }

    integral_type
    operator^=(integral_type i) volatile
        { return __sync_xor_and_fetch(&pod_, i); }

    integral_type exchange(integral_type v) volatile
        {
            return __sync_lock_test_and_set(&pod_, v);
        }
    
    bool compare_exchange(integral_type& v1, integral_type v2) volatile
        {
            integral_type tmp = v1;
            integral_type ret = __sync_val_compare_and_swap(&pod_, tmp, v2);
            // Assume extra stores (of same value) allowed in true case.
            v1 = ret;
            return tmp == ret;
        }
    
    integral_type fetch_add(integral_type v) volatile
        {
            return __sync_fetch_and_add(&pod_, v);
        }
    
    integral_type fetch_sub(integral_type v) volatile
        {
            return __sync_fetch_and_sub(&pod_, v);
        }
    
    integral_type fetch_and(integral_type v) volatile
        {
            return __sync_fetch_and_and(&pod_, v);
        }
    
    integral_type fetch_or(integral_type v) volatile
        {
            return __sync_fetch_and_or(&pod_, v);
        }
    
    integral_type fetch_xor(integral_type v) volatile
        {
            return __sync_fetch_and_xor(&pod_, v);
        }
};

template<>
struct atomic_pod<bool>
{
private:
    bool pod_;
    atomic_pod(const atomic_pod&);
    atomic_pod& operator=(const atomic_pod<bool>&);
    
public:
    typedef bool value_type;
    
    atomic_pod()
        {}
    ~atomic_pod() {}
    

    atomic_pod(bool v)
        {
            pod_ = v;
        }

    bool operator = (bool v)
        {
            store(v);
            return v;
        }

    operator bool() const volatile
        {
            return load();
        }

    bool is_lock_free() const volatile
        {
            return true;
        }

    void store(bool v) volatile
        {
            pod_ = v;
            __sync_synchronize();
        }
    
    bool load() const volatile
        {
            __sync_synchronize();
            bool ret = pod_;
            __sync_synchronize();
            return ret;
        }

    bool exchange(bool v) volatile
        {
            return __sync_lock_test_and_set(&pod_, v);
        }
    
    bool compare_exchange(bool& v1, bool v2) volatile
        {
            bool tmp = v1;
            bool ret = __sync_val_compare_and_swap(&pod_, tmp, v2);
            // Assume extra stores (of same value) allowed in true case.
            v1 = ret;
            return tmp == ret;
        }
};

#else
// TODO: use third-party library
// http://www.hpl.hp.com/research/linux/atomic_ops
#include <atomic_ops.h>
#endif // WITH_GCC_ATOMIC_BUILTIN_SUPPORT

#elif defined(OS_WINDOWS)
// TODO: implement windows version

// InterlockedIncrement
// InterlockedDecrement
// InterlockedExchange

#include "windows.h"

template<typename T, size_t size>
struct AtomicOpPolicyBase;

// bool op
template<>
struct AtomicOpPolicyBase<bool, 1>
{
    // Compare exchange
    static bool InterlockedCompareExchange(bool volatile *Destination,
                                           bool Exchange,
                                           bool Comparand)
        {
            LONG ret = ::InterlockedCompareExchange((LONG*)Destination,
                                                    (LONG)Exchange,
                                                    (LONG)Comparand);
            return ret == 0 ? false : true;
        }

    // The function returns the initial value of the Target parameter.
    static bool InterlockedExchange(bool volatile* Target, bool Value)
        {
            LONG ret = ::InterlockedExchange((LONG*)Target, (LONG)Value);
            return ret == 0 ? false : true;
        }
};

// 8-bit pod op
template<typename T>
struct AtomicOpPolicyBase<T, 1>
{
    // Compare exchange
    static T InterlockedCompareExchange(T volatile *Destination,
                                        T Exchange,
                                        T Comparand)
        {
            LONG ret = ::InterlockedCompareExchange((LONG*)Destination,
                                                    (LONG)Exchange,
                                                    (LONG)Comparand);
            return static_cast<T>(ret);
        }

    // The function returns the initial value of the Target parameter.
    static T InterlockedExchange(T volatile* Target, T Value)
        {
            LONG ret = ::InterlockedExchange((LONG*)Target, (LONG)Value);
            return static_cast<T>(ret);
        }
    
    // fetch and add
    // The function returns the initial value of the Target parameter.
    static T InterlockedExchangeAdd(T volatile* Addend, T value)
        {
            LONG ret = ::InterlockedExchangeAdd((LONG*)Addend, (LONG)value);
            return (T)ret;
        }

    // ++n
    static T InterlockedIncrement(T volatile *Addend)
        {
            LONG ret = ::InterlockedIncrement((LONG*)Addend);
            return (T)ret;
        }

    // --n
    static T InterlockedDecrement(T volatile *Addend)
        {
            LONG ret = ::InterlockedDecrement((LONG*)Addend);
            return (T)ret;
        }
    
    // fetch and and
    // The function returns the original value of the Destination parameter.
    static T InterlockedAnd(T volatile *Destination, T Value)
        {
            char ret = ::InterlockedAnd8((char*)Destination, (char)Value);
            return (T)ret;
        }
    
    // fetch and or
    // The function returns the original value of the Destination parameter.
    static T InterlockedOr(T volatile *Destination, T Value)
        {
            char ret = ::InterlockedOr8((char*)Destination, (char)Value);
            return (T)ret;
        }

    // fetch and xor
    // The function returns the original value of the Destination parameter.
    static T InterlockedXor(T volatile *Destination, LONG Value)
        {
            char ret = ::InterlockedXor8((char*)Destination, (char)Value);
            return (T)ret;
        }
};

template<typename T>
struct AtomicOpPolicyBase<T, 2>
{
    // Compare exchange
    static T InterlockedCompareExchange(T volatile *Destination,
                                        T Exchange,
                                        T Comparand)
        {
            LONG ret = ::InterlockedCompareExchange((LONG*)Destination,
                                                    (LONG)Exchange,
                                                    (LONG)Comparand);
            return (T)ret;
        }

    // The function returns the initial value of the Target parameter.
    static T InterlockedExchange(T volatile* Target, T Value)
        {
            LONG ret = ::InterlockedExchange((LONG*)Target, (LONG)Value);
            return (T)ret;
        }
    
    // fetch and add
    // The function returns the initial value of the Target parameter.
    static T InterlockedExchangeAdd(T volatile* Addend, T value)
        {
            LONG ret = ::InterlockedExchangeAdd((LONG*)Addend, (LONG)value);
            return (T)ret;
        }

    // ++n
    static T InterlockedIncrement(T volatile *Addend)
        {
            LONG ret = ::InterlockedIncrement((LONG*)Addend);
            return (T)ret;
        }

    // --n
    static T InterlockedDecrement(T volatile *Addend)
        {
            LONG ret = ::InterlockedDecrement((LONG*)Addend);
            return (T)ret;
        }

    // fetch and and
    // The function returns the original value of the Destination parameter.
    static T InterlockedAnd(T volatile *Destination, T Value)
        {
            SHORT ret = ::InterlockedAnd16((SHORT*)Destination, (SHORT)Value);
            return (T)ret;
        }
    
    // fetch and or
    // The function returns the original value of the Destination parameter.
    static T InterlockedOr(T volatile *Destination, T Value)
        {
            SHORT ret = ::InterlockedOr16((SHORT*)Destination, (SHORT)Value);
            return (T)ret;
        }

    // fetch and xor
    // The function returns the original value of the Destination parameter.
    static T InterlockedXor(T volatile *Destination, LONG Value)
        {
            SHORT ret = ::InterlockedXor16((SHORT*)Destination, (SHORT)Value);
            return (T)ret;
        }
};

// 32-bit pod op
template<typename T>
struct AtomicOpPolicyBase<T, 4>
{
    // Compare exchange
    static T InterlockedCompareExchange(T volatile *Destination,
                                        T Exchange,
                                        T Comparand)
        {
            LONG ret = ::InterlockedCompareExchange((LONG*)Destination,
                                                    (LONG)Exchange,
                                                    (LONG)Comparand);
            return (T)ret;
        }

    // The function returns the initial value of the Target parameter.
    static T InterlockedExchange(T volatile* Target, T Value)
        {
            LONG ret = ::InterlockedExchange((LONG*)Target, (LONG)Value);
            return (T)ret;
        }
    
    // fetch and add
    // The function returns the initial value of the Target parameter.
    static T InterlockedExchangeAdd(T volatile* Addend, T value)
        {
            LONG ret = ::InterlockedExchangeAdd((LONG*)Addend, (LONG)value);
            return (T)ret;
        }

    // ++n
    static T InterlockedIncrement(T volatile *Addend)
        {
            LONG ret = ::InterlockedIncrement((LONG*)Addend);
            return (T)ret;
        }

    // --n
    static T InterlockedDecrement(T volatile *Addend)
        {
            LONG ret = ::InterlockedDecrement((LONG*)Addend);
            return (T)ret;
        }
    
    // fetch and and
    // The function returns the original value of the Destination parameter.
    static T InterlockedAnd(T volatile *Destination, T Value)
        {
            LONG ret = ::InterlockedAnd((LONG*)Destination, (LONG)Value);
            return (T)ret;
        }
    
    // fetch and or
    // The function returns the original value of the Destination parameter.
    static T InterlockedOr(T volatile *Destination, T Value)
        {
            LONG ret = ::InterlockedOr((LONG*)Destination, (LONG)Value);
            return (T)ret;
        }

    // fetch and xor
    // The function returns the original value of the Destination parameter.
    static T InterlockedXor(T volatile *Destination, LONG Value)
        {
            LONG ret = ::InterlockedXor((LONG*)Destination, (LONG)Value);
            return (T)ret;
        }
};

// 64-bit pod op
template<typename T>
struct AtomicOpPolicyBase<T, 8>
{
    // Compare exchange
    static T InterlockedCompareExchange(T volatile *Destination,
                                        T Exchange,
                                        T Comparand)
        {
            LONGLONG ret = ::InterlockedCompareExchange64((LONGLONG*)Destination,
                                                          (LONGLONG)Exchange,
                                                          (LONGLONG)Comparand);
            return (T)ret;
        }

    // The function returns the initial value of the Target parameter.
    static T InterlockedExchange(T volatile* Target, T Value)
        {
            LONGLONG ret = ::InterlockedExchange64((LONGLONG*)Target, (LONGLONG)Value);
            return (T)ret;
        }
    
    // fetch and add
    // The function returns the initial value of the Target parameter.
    static T InterlockedExchangeAdd(T volatile* Addend, T value)
        {
            LONGLONG ret = ::InterlockedExchangeAdd64((LONGLONG*)Addend, (LONGLONG)value);
            return (T)ret;
        }

    // ++n
    static T InterlockedIncrement(T volatile *Addend)
        {
            LONGLONG ret = ::InterlockedIncrement64((LONGLONG*)Addend);
            return (T)ret;
        }

    // --n
    static T InterlockedDecrement(T volatile *Addend)
        {
            LONGLONG ret = ::InterlockedDecrement64((LONGLONG*)Addend);
            return (T)ret;
        }
    
    // fetch and and
    // The function returns the original value of the Destination parameter.
    static T InterlockedAnd(T volatile *Destination, T Value)
        {
            LONGLONG ret = ::InterlockedAnd64((LONGLONG*)Destination, (LONGLONG)Value);
            return (T)ret;
        }
    
    // fetch and or
    // The function returns the original value of the Destination parameter.
    static T InterlockedOr(T volatile *Destination, T Value)
        {
            LONGLONG ret = ::InterlockedOr64((LONGLONG*)Destination, (LONGLONG)Value);
            return (T)ret;
        }

    // fetch and xor
    // The function returns the original value of the Destination parameter.
    static T InterlockedXor(T volatile *Destination, LONG Value)
        {
            LONGLONG ret = ::InterlockedXor((LONGLONG*)Destination, (LONGLONG)Value);
            return (T)ret;
        }
};

template<typename T>
struct AtomicOpPolicy : public AtomicOpPolicyBase<T, sizeof(T)>
{};

template<typename T>
struct atomic_pod
{
private:
    typedef T integral_type;
    mutable integral_type pod_;
    typedef AtomicOpPolicy<integral_type> Op;
    
    atomic_pod(const atomic_pod&);
    atomic_pod& operator=(const atomic_pod&);
    
public:
    typedef integral_type value_type;
    
    atomic_pod()
        {}
    ~atomic_pod()
        {}
    atomic_pod(integral_type v)
        {
            pod_ = v;
        }

    operator integral_type () const volatile
        {
            return load();
        }
    
    integral_type operator = (integral_type v)
        {
            store(v);
            return v;
        }
    
    bool is_lock_free() const volatile
        {
            return true;
        }
    
    void store(integral_type v) volatile
        {
            Op::InterlockedExchange(&pod_, v);
        }
    
    integral_type load() const volatile
        {
            integral_type ret = Op::InterlockedCompareExchange(&pod_, pod_, pod_);
            return ret;
        }

    integral_type
    operator++(int) volatile
        { return fetch_add(1); }

    integral_type
    operator--(int) volatile
        { return fetch_sub(1); }

    integral_type
    operator++() volatile
        { return Op::InterlockedIncrement(&pod_); }

    integral_type
    operator--() volatile
        { return Op::InterlockedDecrement(&pod_); }

    integral_type
    operator+=(integral_type i) volatile
        {
            integral_type ret = fetch_add(i);
            return ret + i;
        }

    integral_type
    operator-=(integral_type i) volatile
        {
            integral_type ret = fetch_sub(i);
            return ret - i;
        }

    integral_type
    operator&=(integral_type i) volatile
        {
            integral_type ret = fetch_and(i);
            return ret &= i;
        }

    integral_type
    operator|=(integral_type i) volatile
        {
            integral_type ret = fetch_or(i);
            return ret != i;
        }

    integral_type
    operator^=(integral_type i) volatile
        {
            integral_type ret = fetch_xor(i);
            return ret ^= i;
        }

    integral_type exchange(integral_type v) volatile
        {
            return Op::InterlockedExchange(&pod_, v);
        }
    
    bool compare_exchange(integral_type& v1, integral_type v2) volatile
        {
            integral_type oldval = v1;
            integral_type ret = Op::InterlockedCompareExchange(&pod_, v2, oldval);
            v1 = ret;
            return oldval == ret;
        }
    
    integral_type fetch_add(integral_type v) volatile
        {
            return Op::InterlockedExchangeAdd(&pod_, v);
        }
    
    integral_type fetch_sub(integral_type v) volatile
        {
            integral_type tmp = ~v + 1;
            return Op::InterlockedExchangeAdd(&pod_, tmp);
        }
    
    integral_type fetch_and(integral_type v) volatile
        {
            return Op::InterlockedAnd(&pod_, v);
        }
    
    integral_type fetch_or(integral_type v) volatile
        {
            return Op::InterlockedOr(&pod_, v);
        }
    
    integral_type fetch_xor(integral_type v) volatile
        {
            return Op::InterlockedXor(&pod_, v);
        }
};

template<>
struct atomic_pod<bool>
{
private:
    mutable bool pod_;
    atomic_pod(const atomic_pod&);
    atomic_pod& operator=(const atomic_pod<bool>&);
    
public:
    typedef bool value_type;
    typedef AtomicOpPolicy<value_type> Op;
    
    atomic_pod()
        {}
    ~atomic_pod() {}
    

    atomic_pod(bool v)
        {
            pod_ = v;
        }

    bool operator = (bool v)
        {
            store(v);
            return v;
        }

    operator bool() const volatile
        {
            return load();
        }

    bool is_lock_free() const volatile
        {
            return true;
        }

    void store(bool v) volatile
        {
            Op::InterlockedExchange(&pod_, v);
        }
    
    bool load() const volatile
        {
            bool ret = Op::InterlockedCompareExchange(&pod_, pod_, pod_);
            return ret;
        }

    bool exchange(bool v) volatile
        {
            return Op::InterlockedExchange(&pod_, v);
        }
    
    bool compare_exchange(bool& v1, bool v2) volatile
        {
            bool oldval = v1;
            bool ret = Op::InterlockedCompareExchange(&pod_, v2, oldval);
            v1 = ret;
            return oldval == ret;
        }
};
#endif  // OS_WINDOWS
#endif  // _CXX0X_THREAD_

#endif // _ATOMIC_POD_H_
