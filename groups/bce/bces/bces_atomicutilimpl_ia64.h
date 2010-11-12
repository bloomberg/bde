// bces_atomicutilimpl_ia64.h                                         -*-C++-*-
#ifndef INCLUDED_BCES_ATOMICUTILIMPL_IA64
#define INCLUDED_BCES_ATOMICUTILIMPL_IA64

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide atomic operations on Intel Itanium (IA64) architecture.
//
//@CLASSES:
//   bces_AtomicUtilImpl<bsls_Platform::CpuIa64>: namespace for
//   atomic operations on the Intel Itanium architecture
//
//@AUTHOR: Andrei Basov (abasov)
//
//@DESCRIPTION: This component provides a full specialization of
// 'bces_AtomicUtilImpl' for the Intel Itanium (IA64) architecture.  This
// component is not intended to be used directly.  See the 'bces_AtomicUtil'
// for more on using atomic operations.

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENTUTIL
#include <bsls_alignmentutil.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifdef BSLS_PLATFORM__CPU_IA64

namespace BloombergLP {

#if defined(BSLS_PLATFORM__OS_WINDOWS)

#error "not implemented"

#elif defined(BSLS_PLATFORM__OS_HPUX)

int bces_AtomicUtilImpl_IA64SetInt(volatile int *aInt, int val);
int bces_AtomicUtilImpl_IA64GetInt(volatile const int*);

int bces_AtomicUtilImpl_IA64AddInt(volatile int *aInt, int val);

int bces_AtomicUtilImpl_IA64TestAndSwap(volatile int *aInt,
                                        int           cmpVal,
                                        int           swapVal);

bsls_PlatformUtil::Int64
bces_AtomicUtilImpl_IA64SetInt64(volatile bsls_PlatformUtil::Int64 *aInt,
                                 bsls_PlatformUtil::Int64           val);

bsls_PlatformUtil::Int64
bces_AtomicUtilImpl_IA64GetInt64(volatile const bsls_PlatformUtil::Int64*);

bsls_PlatformUtil::Int64
bces_AtomicUtilImpl_IA64AddInt64(volatile bsls_PlatformUtil::Int64 *aInt,
                                 bsls_PlatformUtil::Int64           val);

bsls_PlatformUtil::Int64
bces_AtomicUtilImpl_IA64TestAndSwap64(
                              volatile bsls_PlatformUtil::Int64 *aInt,
                              bsls_PlatformUtil::Int64           cmpVal,
                              bsls_PlatformUtil::Int64           swapVal);

void bces_AtomicUtilImpl_IA64SpinLock(volatile int *aSpin);

int  bces_AtomicUtilImpl_IA64SpinTryLock(volatile int *aSpin, int retries);

void bces_AtomicUtilImpl_IA64SpinUnlock(volatile int *aSpin);

#endif

template <class PLATFORM> struct bces_AtomicUtilImpl;

template <>
struct bces_AtomicUtilImpl<bsls_Platform::CpuIa64> {
    // Namespace for atomic operations on the Intel Itanium architecture.

    // TYPES
    struct SpinLock {
        volatile int d_spin;
    };

    struct Int {
        volatile int d_value;
    };

    struct Int64 {
        volatile bsls_PlatformUtil::Int64 d_value;
    };

    struct Pointer {
        volatile const void *d_value;
    };

    static void initInt(
          bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int *aInt,
          int                                               initialValue);
        // Initialize the specified atomic integer 'aInt', and set its value to
        // the specified 'initialValue'.  Note that this method must be called
        // before any other operations on 'aInt'.

    static void initInt64(
        bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int64 *aInt,
        bsls_PlatformUtil::Int64                            initialValue);
        // Initialize the specified atomic integer 'aInt', and set its value to
        // the specified 'initialValue'.  Note that this method must be called
        // before any other operations on 'aInt'.

    static void initPointer(
        bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Pointer *aPtr,
        const void                                           *initialValue);
        // Initialize the specified atomic pointer 'aPointer', and set its
        // value to the specified 'initialValue'.  Note that this function must
        // be called before any other operations on 'aPointer'.

    static void initSpinLock(
      bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::SpinLock *aSpin);
        // Initialize the specified spin lock 'aSpin' and set it to an an
        // unlocked state.  Note that method must be called before any other
        // operations on 'aSpin'.

    static void addInt(
        bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int *aInt, int val);
        // Atomically add to the specified 'aInt' the specified 'value'.  Note
        // that the behavior is undefined if 'aInt' is 0.

    static void incrementInt(
         bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int *aInt);
        // Atomically increment the value of the specified 'aInt' by 1.  Note
        // that the behavior is undefined if 'aInt' is 0.

    static void decrementInt(
         bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int *aInt);
        // Atomically decrement the value of the specified 'aInt' by 1.  Note
        // that the behavior is undefined if 'aInt' is 0.

    static int swapInt(
         bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int *aInt, int val);
        // Atomically set the value of the specified 'aInt' to the specified
        // 'value' and return its previous value.  Note that the behavior is
        // undefined if 'aInt' is 0.

    static int testAndSwapInt(
          bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int *aInt,
          int                                               cmpVal,
          int                                               swapVal);
        // Conditionally set the value of the specified 'aInt' to the specified
        // 'swapVal' and return its previous value.  The value of 'aInt' is
        // compared to the specified 'cmpVal'; if they are equal, then
        // 'swapVal' will be assigned to 'aInt', otherwise the value of 'aInt'
        // is left unchanged.  The whole operation is performed atomically.
        // Note that the behavior is undefined if 'aInt' is 0.

    static void setInt(
          bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int *aInt, int val);
        // Atomically set the value of the specified 'aInt' to the specified
        // 'value'.  The behavior is undefined if 'aInt' is 0.

    static void setIntRelaxed(
          bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int *aInt, int val);
        // Atomically set the value of the specified 'aInt' to the specified
        // 'value' without additional ordering constraints.  The behavior is
        // undefined if 'aInt' is 0.

    static int getInt(
       const bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int& aInt);
        // Atomically retrieve the value of the specified 'aInt'.

    static int getIntRelaxed(
       const bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int& aInt);
        // Atomically retrieve the value of the specified 'aInt', without
        // additional ordering constraints.

    static int addIntNv(
             bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int *aInt, int val);
        // Atomically add to the specified 'aInt' the specified 'value' and
        // return the resulting value.  The behavior is undefined if 'aInt' is
        // 0.

    static int addIntNvRelaxed(
             bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int *aInt, int val);
        // Atomically add to the specified 'aInt' the specified 'value' and
        // return the resulting value.  The behavior is undefined if 'aInt' is
        // 0.  TBD: NOTE that this method is identical to 'addIntNv' for now.

    static int incrementIntNv(
             bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int *aInt);
        // Atomically increment the specified 'aInt' by 1 and return the
        // resulting value.  Note that the behavior is undefined if 'aInt' is
        // 0.

    static int decrementIntNv(
             bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int *aInt);
        // Atomically decrement the specified 'aInt' by 1 and return the
        // resulting value.  Note that the behavior is undefined if 'aInt' is
        // 0.

    static void addInt64(
         bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int64 *aInt,
         bsls_PlatformUtil::Int64                            val);
        // Atomically add to the specified 'aInt' the specified 'value'.  Note
        // that the behavior is undefined if 'aInt' is 0.

    static void incrementInt64(
         bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int64 *aInt);
        // Atomically increment the value of the specified 'aInt' by 1.  Note
        // that the behavior is undefined if 'aInt' is 0.

    static void decrementInt64(
         bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int64 *aInt);
        // Atomically decrement the value of the specified 'aInt' by 1.  Note
        // that the behavior is undefined if 'aInt' is 0.

    static bsls_PlatformUtil::Int64 swapInt64(
         bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int64 *aInt,
         bsls_PlatformUtil::Int64                            val);
        // Atomically set the value of the specified 'aInt' to the specified
        // 'value' and return its previous value.  Note that the behavior is
        // undefined if 'aInt' is 0.

    static bsls_PlatformUtil::Int64 testAndSwapInt64(
         bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int64 *aInt,
         bsls_PlatformUtil::Int64                            cmpVal,
         bsls_PlatformUtil::Int64                            swapVal);
        // Conditionally set the value of the specified 'aInt' to the specified
        // 'swapVal' and return its previous value.  The value of 'aInt' is
        // compared to the specified 'cmpVal'; if they are equal, then
        // 'swapVal' will be assigned to 'aInt' otherwise, the value of 'aInt'
        // is left unchanged.  The whole operation is performed atomically.
        // Note that the behavior is undefined if 'aInt' is 0.

    static bsls_PlatformUtil::Int64 addInt64Nv(
         bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int64 *aInt,
         bsls_PlatformUtil::Int64                            val);
        // Atomically add to the specified 'aInt' the specified 'value' and
        // return the resulting value.  The behavior is undefined if 'aInt' is
        // 0.

    static bsls_PlatformUtil::Int64 addInt64NvRelaxed(
         bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int64 *aInt,
         bsls_PlatformUtil::Int64                            val);
        // Atomically add to the specified 'aInt' the specified 'value' and
        // return the resulting value.  The behavior is undefined if 'aInt' is
        // 0.  TBD: NOTE that this method is identical to 'addInt64Nv' for now.

    static bsls_PlatformUtil::Int64 incrementInt64Nv(
         bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int64 *aInt);
        // Atomically increment the specified 'aInt' by 1 and return the
        // resulting value.  Note that the behavior is undefined if 'aInt' is
        // 0.

    static bsls_PlatformUtil::Int64 decrementInt64Nv(
         bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int64 *aInt);
        // Atomically decrement the specified 'aInt' by 1 and return the
        // resulting value.  Note that the behavior is undefined if 'aInt' is
        // 0.

    static void setInt64(
         bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int64 *aInt,
         bsls_PlatformUtil::Int64                            val);
        // Atomically set the value of the specified 'aInt' to the specified
        // 'value'.  The behavior is undefined if 'aInt' is 0.

    static void setInt64Relaxed(
         bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int64 *aInt,
         bsls_PlatformUtil::Int64                            val);
        // Atomically set the value of the specified 'aInt' to the specified
        // 'value', without additional ordering constraints.  The behavior is
        // undefined if 'aInt' is 0.

    static bsls_PlatformUtil::Int64 getInt64(
     const bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int64& aInt);
        // Atomically retrieve the value of the specified 'aInt'.

    static bsls_PlatformUtil::Int64 getInt64Relaxed(
     const bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int64& aInt);
        // Atomically retrieve the value of the specified 'aInt', without
        // additional ordering constraints.

    static void *getPtr(
     const bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Pointer& aPointer);
        // Atomically retrieve the value of the specified 'aPointer'.

    static void *getPtrRelaxed(
     const bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Pointer& aPointer);
        // Atomically retrieve the value of the specified 'aPointer', without
        // a memory barrier.

    static void setPtr(
     bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Pointer *aPointer,
     const void                                           *val);
        // Atomically set the value of the specified 'aPointer' to the
        // specified 'value'.  The behavior is undefined if 'aPointer' is 0.

    static void setPtrRelaxed(
     bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Pointer *aPointer,
     const void                                           *val);
        // Atomically set the value of the specified 'aPointer' to the
        // specified 'value', without a memory barrier.  The behavior is
        // undefined if 'aPointer' is 0.

    static void *swapPtr(
     bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Pointer *aPointer,
     const void                                           *val);
        // Atomically set the value of the specified 'aPointer' to the
        // specified 'value' and return its previous value.  Note that the
        // behavior is undefined if 'aPointer' is 0.

    static void *testAndSwapPtr(
     bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Pointer *aPointer,
     const void                                           *cmpVal,
     const void                                           *swapVal);
        // Conditionally set the value of the specified 'aPointer' to the
        // specified 'swapVal' and return its previous value.  The value of
        // 'aPointer' is compared to the specified 'cmpVal'; if they are equal,
        // then 'swapVal' will be assigned to 'aPointer' otherwise, the value
        // of 'aPointer' is left unchanged.  The whole operation is performed
        // atomically.  Note that the behavior is undefined if 'aPointer' is 0.

    static void spinLock(
    bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::SpinLock *aSpin);
        // Lock the specified 'aSpin'.  Note that the behavior is undefined if
        // 'aSpin' is 0.

    static int spinTryLock(
    bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::SpinLock *aSpin,
    int                                                    retries);
        // Attempt to lock the specified 'aSpin' up to 'retries' times.  Return
        // 0 on success, non-zero otherwise.  Note that the behavior is
        // undefined if 'aSpin' is 0 or 0 <= retries.

    static void spinUnlock(
    bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::SpinLock *aSpin);
        // Unlock the specified 'aSpin' which was previously locked by a
        // successful call to 'spinLock' or 'spinTryLock'.  Note that the
        // behavior is undefined if 'aSpin' is 0.
};

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::initInt(
              bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int *aInt,
              int                                               initialValue)
{
#ifdef BSLS_PLATFORM__CPU_64_BIT
    BSLS_ASSERT_OPT(bsls_AlignmentUtil::is2ByteAligned(aInt));
#else
    BSLS_ASSERT_OPT(bsls_AlignmentUtil::is2ByteAligned(aInt));
#endif
    aInt->d_value = initialValue;
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::initInt64(
            bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int64 *aInt,
            bsls_PlatformUtil::Int64                            initialValue)
{
    BSLS_ASSERT_OPT(bsls_AlignmentUtil::is8ByteAligned(aInt));
    aInt->d_value = initialValue;
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::initPointer(
          bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Pointer *aPtr,
          const void                                           *initialValue)
{
#ifdef BSLS_PLATFORM__CPU_64_BIT
    BSLS_ASSERT_OPT(bsls_AlignmentUtil::is8ByteAligned(aPtr));
#else
    BSLS_ASSERT_OPT(bsls_AlignmentUtil::is4ByteAligned(aPtr));
#endif
    aPtr->d_value = initialValue;
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::initSpinLock(
       bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::SpinLock *aSpin)
{
    BSLS_ASSERT_OPT(bsls_AlignmentUtil::is2ByteAligned(aSpin));
    aSpin->d_spin = 0;
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::addInt(
    bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int *aInt, int val)
{
    bces_AtomicUtilImpl_IA64AddInt(&aInt->d_value, val);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::incrementInt(
              bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int *aInt)
{
    bces_AtomicUtilImpl_IA64AddInt(&aInt->d_value, 1);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::decrementInt(
              bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int *aInt)
{
    bces_AtomicUtilImpl_IA64AddInt(&aInt->d_value, -1);
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::swapInt(
              bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int *aInt, int val)
{
    return bces_AtomicUtilImpl_IA64SetInt(&aInt->d_value, val);
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::testAndSwapInt(
              bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int *aInt,
              int                                               cmpVal,
              int                                               swapVal)
{
    return bces_AtomicUtilImpl_IA64TestAndSwap(&aInt->d_value,cmpVal, swapVal);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::setIntRelaxed(
             bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int *aInt, int val)
{
    aInt->d_value = val;
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::setInt(
             bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int *aInt, int val)
{
    bces_AtomicUtilImpl_IA64SetInt(&aInt->d_value, val);
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::getInt(
        const bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int& aInt)
{
    return bces_AtomicUtilImpl_IA64GetInt(&aInt.d_value);
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::getIntRelaxed(
        const bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int& aInt)
{
    return aInt.d_value;
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::addIntNv(
    bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int *aInt, int val)
{
    return bces_AtomicUtilImpl_IA64AddInt(&aInt->d_value, val);
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::addIntNvRelaxed(
    bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int *aInt, int val)
{
    return bces_AtomicUtilImpl_IA64AddInt(&aInt->d_value, val);
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::incrementIntNv(
             bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int *aInt)
{
    return bces_AtomicUtilImpl_IA64AddInt(&aInt->d_value, 1);
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::decrementIntNv(
              bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int *aInt)
{
    return bces_AtomicUtilImpl_IA64AddInt(&aInt->d_value, -1);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::addInt64(
            bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int64 *aInt,
            bsls_PlatformUtil::Int64                            val)
{
    bces_AtomicUtilImpl_IA64AddInt64(&aInt->d_value, val);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::incrementInt64(
            bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int64 *aInt)
{
    bces_AtomicUtilImpl_IA64AddInt64(&aInt->d_value, 1);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::decrementInt64(
            bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int64 *aInt)
{
    bces_AtomicUtilImpl_IA64AddInt64(&aInt->d_value, -1);
}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::swapInt64(
            bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int64 *aInt,
            bsls_PlatformUtil::Int64                            val)
{
    return bces_AtomicUtilImpl_IA64SetInt64(&aInt->d_value, val);
}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::testAndSwapInt64(
                                             Int64                    *aInt,
                                             bsls_PlatformUtil::Int64  cmpVal,
                                             bsls_PlatformUtil::Int64  swapVal)
{
    return bces_AtomicUtilImpl_IA64TestAndSwap64(&aInt->d_value,cmpVal,
                                                 swapVal);
}

inline
bsls_PlatformUtil::Int64
  bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::addInt64Nv(
         bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int64 *aInt,
         bsls_PlatformUtil::Int64                            val)
{
    return bces_AtomicUtilImpl_IA64AddInt64(&aInt->d_value, val);
}

inline
bsls_PlatformUtil::Int64
  bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::addInt64NvRelaxed(
         bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int64 *aInt,
         bsls_PlatformUtil::Int64                            val)
{
    return bces_AtomicUtilImpl_IA64AddInt64(&aInt->d_value, val);
}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::incrementInt64Nv(
            bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int64 *aInt)
{
    return bces_AtomicUtilImpl_IA64AddInt64(&aInt->d_value, 1);
}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::decrementInt64Nv(
            bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int64 *aInt)
{
    return bces_AtomicUtilImpl_IA64AddInt64(&aInt->d_value, -1);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::setInt64(
            bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int64 *aInt,
            bsls_PlatformUtil::Int64                            val)
{
    bces_AtomicUtilImpl_IA64SetInt64(&aInt->d_value, val);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::setInt64Relaxed(
            bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int64 *aInt,
            bsls_PlatformUtil::Int64                            val)
{
    aInt->d_value = val;
}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::getInt64(
      const bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int64& aInt)
{
    return bces_AtomicUtilImpl_IA64GetInt64(&aInt.d_value);
}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::getInt64Relaxed(
      const bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Int64& aInt)
{
    return aInt.d_value;
}

inline
void *bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::getPtr(
const bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Pointer& aPointer)
{
#ifdef BSLS_PLATFORM__CPU_64_BIT
    return (void*)bces_AtomicUtilImpl_IA64GetInt64(
                        (volatile bsls_PlatformUtil::Int64*)&aPointer.d_value);
#else
    return (void*)bces_AtomicUtilImpl_IA64GetInt(
                                        (volatile int*)&aPointer.d_value);
#endif
}

inline
void *bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::getPtrRelaxed(
const bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Pointer& aPointer)
{
    return (void*)aPointer.d_value;
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::setPtr(
    bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Pointer *aPointer,
    const void                                           *val)
{
#ifdef BSLS_PLATFORM__CPU_64_BIT
    bces_AtomicUtilImpl_IA64SetInt64(
                                (volatile bsls_PlatformUtil::Int64*)aPointer,
                                (bsls_PlatformUtil::Int64)val);
#else
    bces_AtomicUtilImpl_IA64SetInt((volatile int*)&aPointer->d_value,
                                   (int)val);
#endif
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::setPtrRelaxed(
    bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Pointer *aPointer,
    const void                                           *val)
{
    aPointer->d_value = val;
}

inline
void *bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::swapPtr(
  bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Pointer *aPointer,
  const void                                           *val)
{
#ifdef BSLS_PLATFORM__CPU_64_BIT
    return (void*)bces_AtomicUtilImpl_IA64SetInt64(
                        (volatile bsls_PlatformUtil::Int64*)&aPointer->d_value,
                        (bsls_PlatformUtil::Int64)val);
#else
    return (void*)bces_AtomicUtilImpl_IA64SetInt(
                                (volatile int*)&aPointer->d_value,
                                (int)val);
#endif
}

inline
void *bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::testAndSwapPtr(
   bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::Pointer *aPointer,
   const void                                           *CmpVal,
   const void                                           *SwapVal)
{
#ifdef BSLS_PLATFORM__CPU_64_BIT
    return (void*)bces_AtomicUtilImpl_IA64TestAndSwap64(
                       (volatile bsls_PlatformUtil::Int64 *)&aPointer->d_value,
                       (bsls_PlatformUtil::Int64)CmpVal,
                       (bsls_PlatformUtil::Int64)SwapVal);
#else
    return (void*)bces_AtomicUtilImpl_IA64TestAndSwap(
                                (volatile int*)&aPointer->d_value,
                                (int)CmpVal,
                                (int)SwapVal);
#endif
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::spinLock(
        bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::SpinLock *aSpin)
{
    bces_AtomicUtilImpl_IA64SpinLock(&aSpin->d_spin);
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::spinTryLock(
      bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::SpinLock *aSpin,
      int                                                    retries)
{
    return bces_AtomicUtilImpl_IA64SpinTryLock(&aSpin->d_spin,retries);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::spinUnlock(
        bces_AtomicUtilImpl<bsls_Platform::CpuIa64>::SpinLock *aSpin)
{
     bces_AtomicUtilImpl_IA64SpinUnlock(&aSpin->d_spin);
}

}  // close namespace BloombergLP

#endif

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
