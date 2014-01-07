// bsls_atomicoperations_CXX11_all_gcc.h                              -*-C++-*-
#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_CXX11_ALL_GCC
#define INCLUDED_BSLS_ATOMICOPERATIONS_CXX11_ALL_GCC

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide implementations of C++11 atomic operations for gcc, clang
//
//@CLASSES:
//  bsls::AtomicOperations_CXX11_ALL_GCC: C++11 atomics for gcc and clang
//
//@DESCRIPTION: This component provides classes necessary to implement
// C++11 atomics in 32bit/64bit mode with the GCC and clang compilers.
// The classes are for private use only.  See 'bsls_atomicoperations'
// and 'bsls_atomic' for the public interface to atomics.

#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_DEFAULT
#include <bsls_atomicoperations_default.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)

namespace BloombergLP {

namespace bsls {

struct AtomicOperations_CXX11_ALL_GCC;
typedef AtomicOperations_CXX11_ALL_GCC  AtomicOperations_Imp;

         // ========================================================
         // struct Atomic_TypeTraits<AtomicOperations_CXX11_ALL_GCC>
         // ========================================================

template <>
struct Atomic_TypeTraits<AtomicOperations_CXX11_ALL_GCC>
{
    struct __attribute__((__aligned__(sizeof(int)))) Int
    {
          int d_value;
    };

    struct __attribute__((__aligned__(sizeof(Types::Int64)))) Int64
    {
          Types::Int64 d_value;
    };

    struct __attribute__((__aligned__(sizeof(void *)))) Pointer
    {
          void * d_value;
    };
};

                   // =====================================
                   // struct AtomicOperations_CXX11_ALL_GCC
                   // =====================================

struct AtomicOperations_CXX11_ALL_GCC
  #ifdef BSLS_PLATFORM_CPU_64_BIT
    : AtomicOperations_Default64<AtomicOperations_CXX11_ALL_GCC>
  #else
    : AtomicOperations_Default32<AtomicOperations_CXX11_ALL_GCC>
  #endif
{
    typedef Atomic_TypeTraits<AtomicOperations_CXX11_ALL_GCC> AtomicTypes;

    typedef char AtomicInt_SizeCheck[sizeof(int) == 4 ? 1 : -1];
        // compile-time assert

        // *** atomic functions for int ***

    static void initInt(AtomicTypes::Int *atomicInt, int value);

    static int getInt(const AtomicTypes::Int *atomicInt);

    static int getIntAcquire(const AtomicTypes::Int *atomicInt);

    static int getIntRelaxed(const AtomicTypes::Int *atomicInt);

    static void setInt(AtomicTypes::Int *atomicInt, int value);

    static void setIntRelease(AtomicTypes::Int *atomicInt, int value);

    static void setIntRelaxed(AtomicTypes::Int *atomicInt, int value);

    static int swapInt(AtomicTypes::Int *atomicInt, int swapValue);

    static int swapIntAcqRel(AtomicTypes::Int *atomicInt, int swapValue);

    static int testAndSwapInt(AtomicTypes::Int *atomicInt,
                              int compareValue,
                              int swapValue);

    static int testAndSwapIntAcqRel(AtomicTypes::Int *atomicInt,
                                    int compareValue,
                                    int swapValue);

    static int addIntNv(AtomicTypes::Int *atomicInt, int value);

    static int addIntNvAcqRel(AtomicTypes::Int *atomicInt, int value);

    static int addIntNvRelaxed(AtomicTypes::Int *atomicInt, int value);

        // *** atomic functions for Int64 ***

    static void initInt64(AtomicTypes::Int64 *atomicInt, Types::Int64 value);

    static Types::Int64 getInt64(const AtomicTypes::Int64 *atomicInt);

    static Types::Int64 getInt64Acquire(const AtomicTypes::Int64 *atomicInt);

    static Types::Int64 getInt64Relaxed(const AtomicTypes::Int64 *atomicInt);

    static void setInt64(AtomicTypes::Int64 *atomicInt, Types::Int64 value);

    static void setInt64Release(AtomicTypes::Int64 *atomicInt,
                                Types::Int64 value);

    static void setInt64Relaxed(AtomicTypes::Int64 *atomicInt,
                                Types::Int64 value);

    static Types::Int64 swapInt64(AtomicTypes::Int64  *atomicInt,
                                  Types::Int64 swapValue);

    static Types::Int64 swapInt64AcqRel(AtomicTypes::Int64  *atomicInt,
                                        Types::Int64 swapValue);

    static Types::Int64 testAndSwapInt64(AtomicTypes::Int64 *atomicInt,
                                         Types::Int64 compareValue,
                                         Types::Int64 swapValue);

    static Types::Int64 testAndSwapInt64AcqRel(AtomicTypes::Int64 *atomicInt,
                                               Types::Int64 compareValue,
                                               Types::Int64 swapValue);

    static Types::Int64 addInt64Nv(AtomicTypes::Int64 *atomicInt,
                                   Types::Int64 value);

    static Types::Int64 addInt64NvAcqRel(AtomicTypes::Int64 *atomicInt,
                                         Types::Int64 value);

    static Types::Int64 addInt64NvRelaxed(AtomicTypes::Int64 *atomicInt,
                                          Types::Int64 value);
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                     // -------------------------------------
                     // struct AtomicOperations_CXX11_ALL_GCC
                     // -------------------------------------

inline
void AtomicOperations_CXX11_ALL_GCC::
    initInt(AtomicTypes::Int *atomicInt, int value)
{
    __atomic_store_n(&atomicInt->d_value, value, __ATOMIC_RELAXED);
}

inline
int AtomicOperations_CXX11_ALL_GCC::
    getInt(const AtomicTypes::Int *atomicInt)
{
    return __atomic_load_n(&atomicInt->d_value, __ATOMIC_SEQ_CST);
}

inline
int AtomicOperations_CXX11_ALL_GCC::
    getIntAcquire(const AtomicTypes::Int *atomicInt)
{
    return __atomic_load_n(&atomicInt->d_value, __ATOMIC_ACQUIRE);
}

inline
int AtomicOperations_CXX11_ALL_GCC::
    getIntRelaxed(const AtomicTypes::Int *atomicInt)
{
    return __atomic_load_n(&atomicInt->d_value, __ATOMIC_RELAXED);
}

inline
void AtomicOperations_CXX11_ALL_GCC::
    setInt(AtomicTypes::Int *atomicInt, int value)
{
    __atomic_store_n(&atomicInt->d_value, value, __ATOMIC_SEQ_CST);
}

inline
void AtomicOperations_CXX11_ALL_GCC::
    setIntRelease(AtomicTypes::Int *atomicInt, int value)
{
    __atomic_store_n(&atomicInt->d_value, value, __ATOMIC_RELEASE);
}

inline
void AtomicOperations_CXX11_ALL_GCC::
    setIntRelaxed(AtomicTypes::Int *atomicInt, int value)
{
    __atomic_store_n(&atomicInt->d_value, value, __ATOMIC_RELAXED);
}

inline
int AtomicOperations_CXX11_ALL_GCC::
    swapInt(AtomicTypes::Int *atomicInt, int swapValue)
{
    return
      __atomic_exchange_n(&atomicInt->d_value, swapValue, __ATOMIC_SEQ_CST);
}

inline
int AtomicOperations_CXX11_ALL_GCC::
    swapIntAcqRel(AtomicTypes::Int *atomicInt, int swapValue)
{
    return
      __atomic_exchange_n(&atomicInt->d_value, swapValue, __ATOMIC_ACQ_REL);
}

inline
int AtomicOperations_CXX11_ALL_GCC::
    testAndSwapInt(AtomicTypes::Int *atomicInt,
                   int compareValue, int swapValue)
{
    __atomic_compare_exchange_n(&atomicInt->d_value, &compareValue, swapValue,
                                0, __ATOMIC_SEQ_CST, __ATOMIC_RELAXED);
    return compareValue;
}

inline
int AtomicOperations_CXX11_ALL_GCC::
    testAndSwapIntAcqRel(AtomicTypes::Int *atomicInt,
                         int compareValue, int swapValue)
{
    __atomic_compare_exchange_n(&atomicInt->d_value, &compareValue, swapValue,
                                0, __ATOMIC_ACQ_REL, __ATOMIC_RELAXED);
    return compareValue;
}

inline
int AtomicOperations_CXX11_ALL_GCC::
    addIntNv(AtomicTypes::Int *atomicInt, int value)
{
    return __atomic_add_fetch(&atomicInt->d_value, value, __ATOMIC_SEQ_CST);
}

inline
int AtomicOperations_CXX11_ALL_GCC::
    addIntNvAcqRel(AtomicTypes::Int *atomicInt, int value)
{
    return __atomic_add_fetch(&atomicInt->d_value, value, __ATOMIC_ACQ_REL);
}

inline
int AtomicOperations_CXX11_ALL_GCC::
    addIntNvRelaxed(AtomicTypes::Int *atomicInt, int value)
{
    return __atomic_add_fetch(&atomicInt->d_value, value, __ATOMIC_RELAXED);
}

inline
void AtomicOperations_CXX11_ALL_GCC::
    initInt64(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
    __atomic_store_n(&atomicInt->d_value, value, __ATOMIC_RELAXED);
}

inline
Types::Int64 AtomicOperations_CXX11_ALL_GCC::
    getInt64(const AtomicTypes::Int64 *atomicInt)
{
    return __atomic_load_n(&atomicInt->d_value, __ATOMIC_SEQ_CST);
}

inline
Types::Int64 AtomicOperations_CXX11_ALL_GCC::
    getInt64Acquire(const AtomicTypes::Int64 *atomicInt)
{
    return __atomic_load_n(&atomicInt->d_value, __ATOMIC_ACQUIRE);
}

inline
Types::Int64 AtomicOperations_CXX11_ALL_GCC::
    getInt64Relaxed(const AtomicTypes::Int64 *atomicInt)
{
    return __atomic_load_n(&atomicInt->d_value, __ATOMIC_RELAXED);
}

inline
void AtomicOperations_CXX11_ALL_GCC::
    setInt64(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
    __atomic_store_n(&atomicInt->d_value, value, __ATOMIC_SEQ_CST);
}

inline
void AtomicOperations_CXX11_ALL_GCC::
    setInt64Release(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
    __atomic_store_n(&atomicInt->d_value, value, __ATOMIC_RELEASE);
}

inline
void AtomicOperations_CXX11_ALL_GCC::
    setInt64Relaxed(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
    __atomic_store_n(&atomicInt->d_value, value, __ATOMIC_RELAXED);
}

inline
Types::Int64 AtomicOperations_CXX11_ALL_GCC::
    swapInt64(AtomicTypes::Int64 *atomicInt, Types::Int64 swapValue)
{
    return
      __atomic_exchange_n(&atomicInt->d_value, swapValue, __ATOMIC_SEQ_CST);
}

inline
Types::Int64 AtomicOperations_CXX11_ALL_GCC::
    swapInt64AcqRel(AtomicTypes::Int64 *atomicInt, Types::Int64 swapValue)
{
    return
      __atomic_exchange_n(&atomicInt->d_value, swapValue, __ATOMIC_ACQ_REL);
}

inline
Types::Int64 AtomicOperations_CXX11_ALL_GCC::
    testAndSwapInt64(AtomicTypes::Int64 *atomicInt,
                     Types::Int64 compareValue, Types::Int64 swapValue)
{
    __atomic_compare_exchange_n(&atomicInt->d_value, &compareValue, swapValue,
                                0, __ATOMIC_SEQ_CST, __ATOMIC_RELAXED);
    return compareValue;
}

inline
Types::Int64 AtomicOperations_CXX11_ALL_GCC::
    testAndSwapInt64AcqRel(AtomicTypes::Int64 *atomicInt,
                           Types::Int64 compareValue, Types::Int64 swapValue)
{
    __atomic_compare_exchange_n(&atomicInt->d_value, &compareValue, swapValue,
                                0, __ATOMIC_ACQ_REL, __ATOMIC_RELAXED);
    return compareValue;
}

inline
Types::Int64 AtomicOperations_CXX11_ALL_GCC::
    addInt64Nv(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
    return __atomic_add_fetch(&atomicInt->d_value, value, __ATOMIC_SEQ_CST);
}

inline
Types::Int64 AtomicOperations_CXX11_ALL_GCC::
    addInt64NvAcqRel(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
    return __atomic_add_fetch(&atomicInt->d_value, value, __ATOMIC_ACQ_REL);
}

inline
Types::Int64 AtomicOperations_CXX11_ALL_GCC::
    addInt64NvRelaxed(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
    return __atomic_add_fetch(&atomicInt->d_value, value, __ATOMIC_RELAXED);
}

}  // close package namespace

}  // close enterprise namespace

#endif  // BSLS_PLATFORM_CPU_CXX11 && BSLS_PLATFORM_CMP_GNU

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
