// bces_atomicutilimpl_sparcv9.h   -*-C++-*-
#ifndef INCLUDED_BCES_ATOMICUTILIMPL_SPARCV9
#define INCLUDED_BCES_ATOMICUTILIMPL_SPARCV9

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide atomic operations on the 64-bit sparc(v9) architectures.
//
//@CLASSES:
//   bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>: namespace for atomic
//                                     operations on the SPARC_V9 architecture
//
//@SEE_ALSO: bces_atomicutil
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@DESCRIPTION: This component provides a full specialization of
// 'bces_AtomicUtilImpl' for 64-bit sparc architectures.  The component is
// not intended to be used directly.  See the 'bces_AtomicUtil' for more on
// using atomic operations.

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENTUTIL
#include <bsls_alignmentutil.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

#ifdef BSLS_PLATFORM__CPU_SPARC_V9

namespace BloombergLP {

extern "C" {

    void bces_AtomicUtilImpl_SparcV9SetInt(volatile int *aInt, int val);

    int bces_AtomicUtilImpl_SparcV9SwapInt(volatile int *aInt, int val);

    int bces_AtomicUtilImpl_SparcV9GetInt(const volatile int *);

    int bces_AtomicUtilImpl_SparcV9AddInt(volatile int *aInt, int val);

    int bces_AtomicUtilImpl_SparcV9AddIntRelaxed(volatile int *aInt, int val);

    int bces_AtomicUtilImpl_SparcV9TestAndSwap(volatile int *aInt,
                                               int           cmpVal,
                                               int           swapVal);
    void  bces_AtomicUtilImpl_SparcV9SetInt64(
                                       volatile bsls_PlatformUtil::Int64 *aInt,
                                       bsls_PlatformUtil::Int64           val);

    bsls_PlatformUtil::Int64  bces_AtomicUtilImpl_SparcV9SwapInt64(
                                       volatile bsls_PlatformUtil::Int64 *aInt,
                                       bsls_PlatformUtil::Int64           val);

    bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_SparcV9GetInt64(
                                    const volatile bsls_PlatformUtil::Int64 *);

    bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_SparcV9AddInt64(
                                       volatile bsls_PlatformUtil::Int64 *aInt,
                                       bsls_PlatformUtil::Int64           val);

    bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_SparcV9AddInt64Relaxed(
                                       volatile bsls_PlatformUtil::Int64 *aInt,
                                       bsls_PlatformUtil::Int64           val);

    bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_SparcV9TestAndSwap64(
                                   volatile bsls_PlatformUtil::Int64 *aInt,
                                   bsls_PlatformUtil::Int64           cmpVal,
                                   bsls_PlatformUtil::Int64           swapVal);

    void bces_AtomicUtilImpl_SparcV9SpinLock(volatile void *aSpin);
    int  bces_AtomicUtilImpl_SparcV9SpinTryLock(volatile void *aSpin,
                                                int            retries);
    void bces_AtomicUtilImpl_SparcV9SpinUnlock(volatile void *aSpin);
}

template <class PLATFORM> struct bces_AtomicUtilImpl;

template <>
struct bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9> {
    // Namespace for atomic operations on the SPARCv9 architecture.

    // TYPES
    struct Int {
#ifdef BSLS_PLATFORM__CMP_SUN
        #pragma align 4 (d_value)
#endif
        volatile int d_value;
    };

    struct Int64 {
#ifdef BSLS_PLATFORM__CMP_SUN
        #pragma align 8 (d_value)
#endif
        volatile bsls_PlatformUtil::Int64 d_value;
    };

    struct Pointer {
#ifdef BSLS_PLATFORM__CMP_SUN
        #pragma align 8 (d_value)
#endif
        const void *volatile d_value;
    };

    struct SpinLock {
        volatile char d_lock;
    };

    static void initInt(
           bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int *aInt,
           int                                                   initialValue);
        // Initialize the specified 'aInt' and set its value to the specified
        // 'initialValue'.  Note that this method must be called before any
        // other operations on 'aInt'.

    static void initInt64(
                 bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int64 *aInt,
                 bsls_PlatformUtil::Int64                                val);
        // Initialize the specified 'aInt' and set its value to the specified
        // 'initialValue'.  Note that this method must be called before any
        // other operations on 'aInt'.

    static void initPointer(
       bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Pointer *aPtr,
       const void                                               *initialValue);
        // Initialize the specified 'aPointer' and set its value to the
        // specified 'initialValue'.  Note that this function must be called
        // before any other operations on 'aPointer'.

    static void initSpinLock(
            bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::SpinLock *aSpin);
        // Initialize the specified 'aSpin' and set it to an an unlocked state.
        // Note that method must be called before any other operations on
        // 'aSpin'.

    static void addInt(
                    bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int *aInt,
                    int                                                   val);
        // Atomically add to the specified 'aInt' the specified 'value'.  Note
        // that the behavior is undefined if 'aInt' is 0.

    static void incrementInt(
                   bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int *aInt);
        // Atomically increment the value of the specified 'aInt' by 1.  Note
        // that the behavior is undefined if 'aInt' is 0.

    static void decrementInt(
                   bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int *aInt);
        // Atomically decrement the value of the specified 'aInt' by 1.  Note
        // that the behavior is undefined if 'aInt' is 0.

    static int swapInt(
                    bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int *aInt,
                    int                                                   val);
        // Atomically set the value of the specified 'aInt' to the specified
        // 'value' and return its previous value.  Note that the behavior is
        // undefined if 'aInt' is 0.

    static int testAndSwapInt(
                bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int *aInt,
                int                                                   cmpVal,
                int                                                   swapVal);
        // Conditionally set the value of the specified 'aInt' to the specified
        // 'swapVal' and return its previous value.  The value of 'aInt' is
        // compared to the specified 'cmpVal'; if they are equal, then
        // 'swapVal' will be assigned to 'aInt', otherwise the value of 'aInt'
        // is left unchanged.  The whole operation is performed atomically.
        // Note that the behavior is undefined if 'aInt' is 0.

    static void setInt(
                    bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int *aInt,
                    int                                                   val);
        // Atomically set the value of the specified 'aInt' to the specified
        // 'value'.  The behavior is undefined if 'aInt' is 0.

    static int getInt(
             const bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int& aInt);
        // Atomically retrieve the value of the specified 'aInt'.

    static void setIntRelaxed(
                    bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int *aInt,
                    int                                                   val);
        // Atomically set the value of the specified 'aInt' to the specified
        // 'value' without a memory barrier.  The behavior is undefined if
        // 'aInt' is 0.

    static int getIntRelaxed(
             const bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int& aInt);
        // Atomically retrieve the value of the specified 'aInt' without a
        // memory barrier.

    static int addIntNv(
                    bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int *aInt,
                    int                                                   val);
        // Atomically add to the specified 'aInt' the specified 'value' and
        // return the resulting value.  The behavior is undefined if 'aInt' is
        // 0.

    static int addIntNvRelaxed(
                    bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int *aInt,
                    int                                                   val);
        // Atomically add to the specified 'aInt' the specified 'value' and
        // return the resulting value, without a memory barrier.  The behavior
        // is undefined if 'aInt' is 0.

    static int incrementIntNv(
                   bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int *aInt);
        // Atomically increment the specified 'aInt' by 1 and return the
        // resulting value.  Note that the behavior is undefined if 'aInt' is
        // 0.

    static int decrementIntNv(
                   bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int *aInt);
        // Atomically decrement the specified 'aInt' by 1 and return the
        // resulting value.  Note that the behavior is undefined if 'aInt' is
        // 0.

    static void addInt64(
                  bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int64 *aInt,
                  bsls_PlatformUtil::Int64                                val);
        // Atomically add to the specified 'aInt' the specified 'value'.  Note
        // that the behavior is undefined if 'aInt' is 0.

    static void incrementInt64(
                 bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int64 *aInt);
        // Atomically increment the value of the specified 'aInt' by 1.  Note
        // that the behavior is undefined if 'aInt' is 0.

    static void decrementInt64(
                 bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int64 *aInt);
        // Atomically decrement the value of the specified 'aInt' by 1.  Note
        // that the behavior is undefined if 'aInt' is 0.

    static bsls_PlatformUtil::Int64 swapInt64(
                  bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int64 *aInt,
                  bsls_PlatformUtil::Int64                                val);
        // Atomically set the value of the specified 'aInt' to the specified
        // 'value' and return its previous value.  Note that the behavior is
        // undefined if 'aInt' is 0.

    static bsls_PlatformUtil::Int64 testAndSwapInt64(
              bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int64 *aInt,
              bsls_PlatformUtil::Int64                                cmpVal,
              bsls_PlatformUtil::Int64                                swapVal);
        // Conditionally set the value of the specified 'aInt' to the specified
        // 'swapVal' and return its previous value.  The value of 'aInt' is
        // compared to the specified 'cmpVal'; if they are equal, then
        // 'swapVal' will be assigned to 'aInt' otherwise, the value of 'aInt'
        // is left unchanged.  The whole operation is performed atomically.
        // Note that the behavior is undefined if 'aInt' is 0.

    static bsls_PlatformUtil::Int64 addInt64Nv(
                  bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int64 *aInt,
                  bsls_PlatformUtil::Int64                                val);
        // Atomically add to the specified 'aInt' the specified 'value' and
        // return the resulting value.  The behavior is undefined if 'aInt' is
        // 0.

    static bsls_PlatformUtil::Int64 addInt64NvRelaxed(
                  bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int64 *aInt,
                  bsls_PlatformUtil::Int64                                val);
        // Atomically add to the specified 'aInt' the specified 'value' and
        // return the resulting value, without a memory barrier.  The behavior
        // is undefined if 'aInt' is 0.

    static bsls_PlatformUtil::Int64 incrementInt64Nv(
                 bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int64 *aInt);
        // Atomically increment the specified 'aInt' by 1 and return the
        // resulting value.  The behavior is undefined if 'aInt' is 0.

    static bsls_PlatformUtil::Int64 decrementInt64Nv(
                 bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int64 *aInt);
        // Atomically decrement the specified 'aInt' by 1 and return the
        // resulting value.  Note that the behavior is undefined if 'aInt' is
        // 0.

    static void setInt64(
                  bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int64 *aInt,
                  bsls_PlatformUtil::Int64                                val);
        // Atomically set the value of the specified 'aInt' to the specified
        // 'value'.  The behavior is undefined if 'aInt' is 0.

    static void setInt64Relaxed(
                  bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int64 *aInt,
                  bsls_PlatformUtil::Int64                                val);
        // Atomically set the value of the specified 'aInt' to the specified
        // 'value' without a memory barrier.  The behavior is undefined if
        // 'aInt' is 0.

    static bsls_PlatformUtil::Int64 getInt64(
           const bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int64& aInt);
        // Atomically retrieve the value of the specified 'aInt'.

    static bsls_PlatformUtil::Int64 getInt64Relaxed(
           const bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int64& aInt);
        // Atomically retrieve the value of the specified 'aInt' without a
        // memory barrier.

    static void *getPtr(
     const bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Pointer& aPointer);
        // Atomically retrieve the value of the specified 'aPointer'.

    static void *getPtrRelaxed(
     const bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Pointer& aPointer);
        // Atomically retrieve the value of the specified 'aPointer' without
        // a memory barrier.

    static void setPtr(
            bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Pointer *aPointer,
            const          void                                      *val);
        // Atomically set the value of the specified 'aPointer' to the
        // specified 'value'.  The behavior is undefined if 'aPointer' is 0.

    static void setPtrRelaxed(
            bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Pointer *aPointer,
            const void                                               *val);
        // Atomically set the value of the specified 'aPointer' to the
        // specified 'value' without a memory barrier.  The behavior is
        // undefined if 'aPointer' is 0.

    static void *swapPtr(
            bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Pointer *aPointer,
            const void volatile                                      *val);
        // Atomically set the value of the specified 'aPointer' to the
        // specified 'value' and return its previous value.  Note that the
        // behavior is undefined if 'aPointer' is 0.

    static void *testAndSwapPtr(
            bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Pointer *aPointer,
            const volatile void                                      *cmpVal,
            const volatile void                                      *swapVal);
        // Conditionally set the value of the specified 'aPointer' to the
        // specified 'swapVal' and return its previous value.  The value of
        // 'aPointer' is compared to the specified 'cmpVal'; if they are equal,
        // then 'swapVal' will be assigned to 'aPointer' otherwise, the value
        // of 'aPointer' is left unchanged.  The whole operation is performed
        // atomically.  Note that the behavior is undefined if 'aPointer' is 0.

    static void spinLock(
            bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::SpinLock *aSpin);
        // Lock the specified 'aSpin'.  Note that the behavior is undefined if
        // 'aSpin' is 0.

    static int spinTryLock(
           bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::SpinLock *aSpin,
           int                                                        retries);
        // Attempt to lock the specified 'aSpin' up to 'retries' times.  Return
        // 0 on success, non-zero otherwise.  Note that the behavior is
        // undefined if 'aSpin' is 0 or 0 <= retries.

    static void spinUnlock(
             bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::SpinLock *aSpin);
        // Unlock the specified 'aSpin' which was previously locked by a
        // successful call to 'spinLock' or 'spinTryLock'.  Note that the
        // behavior is undefined if 'aSpin' is 0.
};

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::initInt(
            bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int *aInt,
            int                                                   initialValue)
{
    BSLS_ASSERT_OPT(bsls_AlignmentUtil::is4ByteAligned(aInt));
    aInt->d_value = initialValue;
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::initInt64(
          bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int64 *aInt,
          bsls_PlatformUtil::Int64                                initialValue)
{
    BSLS_ASSERT_OPT(bsls_AlignmentUtil::is8ByteAligned(aInt));
    aInt->d_value = initialValue;
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::initPointer(
        bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Pointer *aPtr,
        const void                                               *initialValue)
{
    BSLS_ASSERT_OPT(bsls_AlignmentUtil::is8ByteAligned(aPtr));
    aPtr->d_value = initialValue;
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::initSpinLock(
              bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::SpinLock *aSpin)
{
    aSpin->d_lock = 0;
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::addInt(
           bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int *aInt, int val)
{
    bces_AtomicUtilImpl_SparcV9AddInt(&aInt->d_value, val);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::incrementInt(
                    bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int *aInt)
{
    bces_AtomicUtilImpl_SparcV9AddInt(&aInt->d_value, 1);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::decrementInt(
                    bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int *aInt)
{
    bces_AtomicUtilImpl_SparcV9AddInt(&aInt->d_value, -1);
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::swapInt(
                    bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int *aInt,
                    int                                                   val)
{
    return bces_AtomicUtilImpl_SparcV9SwapInt(&aInt->d_value, val);
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::testAndSwapInt(
                 bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int *aInt,
                 int                                                   cmpVal,
                 int                                                   swapVal)
{
    return bces_AtomicUtilImpl_SparcV9TestAndSwap(&aInt->d_value,
                                                  cmpVal,
                                                  swapVal);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::setInt(
                    bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int *aInt,
                    int                                                   val)
{
    bces_AtomicUtilImpl_SparcV9SetInt(&aInt->d_value, val);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::setIntRelaxed(
                    bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int *aInt,
                    int                                                   val)
{
    aInt->d_value = val;
}

inline
int  bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::getInt(
              const bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int& aInt)
{
    return bces_AtomicUtilImpl_SparcV9GetInt(&aInt.d_value);
}

inline
int  bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::getIntRelaxed(
              const bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int& aInt)
{
    return aInt.d_value;
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::addIntNv(
           bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int *aInt, int val)
{
    return bces_AtomicUtilImpl_SparcV9AddInt(&aInt->d_value, val);
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::addIntNvRelaxed(
           bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int *aInt, int val)
{
    return bces_AtomicUtilImpl_SparcV9AddIntRelaxed(&aInt->d_value, val);
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::incrementIntNv(
                    bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int *aInt)
{
    return bces_AtomicUtilImpl_SparcV9AddInt(&aInt->d_value, 1);
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::decrementIntNv(
                    bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int *aInt)
{
    return bces_AtomicUtilImpl_SparcV9AddInt(&aInt->d_value, -1);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::addInt64(
                  bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int64 *aInt,
                  bsls_PlatformUtil::Int64                                val)
{
    bces_AtomicUtilImpl_SparcV9AddInt64(&aInt->d_value, val);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::incrementInt64(
                  bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int64 *aInt)
{
    bces_AtomicUtilImpl_SparcV9AddInt64(&aInt->d_value, 1);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::decrementInt64(
                  bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int64 *aInt)
{
    bces_AtomicUtilImpl_SparcV9AddInt64(&aInt->d_value, -1);
}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::swapInt64(
                  bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int64 *aInt,
                  bsls_PlatformUtil::Int64                                val)
{
    return bces_AtomicUtilImpl_SparcV9SwapInt64(&aInt->d_value, val);
}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::testAndSwapInt64(
                                             Int64                    *aInt,
                                             bsls_PlatformUtil::Int64  cmpVal,
                                             bsls_PlatformUtil::Int64  swapVal)
{
    return bces_AtomicUtilImpl_SparcV9TestAndSwap64(&aInt->d_value,
                                                    cmpVal,
                                                    swapVal);
}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::addInt64Nv(
                  bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int64 *aInt,
                  bsls_PlatformUtil::Int64                                val)
{
    return bces_AtomicUtilImpl_SparcV9AddInt64(&aInt->d_value, val);
}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::addInt64NvRelaxed(
                  bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int64 *aInt,
                  bsls_PlatformUtil::Int64                                val)
{
    return bces_AtomicUtilImpl_SparcV9AddInt64Relaxed(&aInt->d_value, val);
}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::incrementInt64Nv(
                  bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int64 *aInt)
{
    return bces_AtomicUtilImpl_SparcV9AddInt64(&aInt->d_value, 1);
}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::decrementInt64Nv(
                  bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int64 *aInt)
{
    return bces_AtomicUtilImpl_SparcV9AddInt64(&aInt->d_value, -1);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::setInt64Relaxed(
                  bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int64 *aInt,
                  bsls_PlatformUtil::Int64                                val)
{
    aInt->d_value = val;
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::setInt64(
                  bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int64 *aInt,
                  bsls_PlatformUtil::Int64                                val)
{
    bces_AtomicUtilImpl_SparcV9SetInt64(&aInt->d_value, val);
}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::getInt64(
            const bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int64& aInt)
{
    return bces_AtomicUtilImpl_SparcV9GetInt64(&aInt.d_value);
}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::getInt64Relaxed(
            const bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Int64& aInt)
{
    return aInt.d_value;
}

inline
void *bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::getPtr(
      const bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Pointer& aPointer)
{
    return (void *)bces_AtomicUtilImpl_SparcV9GetInt64(
                       (volatile bsls_PlatformUtil::Int64 *)&aPointer.d_value);
}

inline
void *bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::getPtrRelaxed(
      const bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Pointer& aPointer)
{
    return const_cast<void *>(aPointer.d_value);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::setPtr(
            bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Pointer *aPointer,
            const void                                               *val)
{
    bces_AtomicUtilImpl_SparcV9SetInt64(
                       (volatile bsls_PlatformUtil::Int64 *)&aPointer->d_value,
                       (bsls_PlatformUtil::Int64)val);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::setPtrRelaxed(
            bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Pointer *aPointer,
            const void                                               *val)
{
    aPointer->d_value = val;
}

inline
void *bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::swapPtr(
            bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Pointer *aPointer,
            const volatile void                                      *val)
{
    return (void *)bces_AtomicUtilImpl_SparcV9SwapInt64(
                       (volatile bsls_PlatformUtil::Int64 *)&aPointer->d_value,
                       (bsls_PlatformUtil::Int64)val);
}

inline
void *bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::testAndSwapPtr(
            bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::Pointer *aPointer,
            const volatile void                                      *cmpVal,
            const volatile void                                      *swapVal)
{
    return (void *)bces_AtomicUtilImpl_SparcV9TestAndSwap64(
                       (volatile bsls_PlatformUtil::Int64 *)&aPointer->d_value,
                       (bsls_PlatformUtil::Int64)cmpVal,
                       (bsls_PlatformUtil::Int64)swapVal);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::spinLock(
              bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::SpinLock *aSpin)
{
    bces_AtomicUtilImpl_SparcV9SpinLock(aSpin);
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::spinTryLock(
            bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::SpinLock *aSpin,
            int                                                        retries)
{
    return bces_AtomicUtilImpl_SparcV9SpinTryLock(aSpin, retries);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::spinUnlock(
              bces_AtomicUtilImpl<bsls_Platform::CpuSparc_V9>::SpinLock *aSpin)
{
     bces_AtomicUtilImpl_SparcV9SpinUnlock(aSpin);
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
