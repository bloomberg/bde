// bcemt_conditionimpl_win32.h                                        -*-C++-*-
#ifndef INCLUDED_BCEMT_CONDITIONIMPL_WIN32
#define INCLUDED_BCEMT_CONDITIONIMPL_WIN32

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a win32 implementation of 'bcemt_Condition'.
//
//@CLASSES:
//  bcemt_ConditionImpl<Win32Threads>: win32 specialization
//
//@SEE_ALSO: bcemt_condition
//
//@AUTHOR: Vlad Kliatchko (vkliatch), David Schumann (dschumann1)
//
//@DESCRIPTION: This component provides an implementation of 'bcemt_Condition'
// for Windows (win32) via the template specialization:
//..
//  bcemt_ConditionImpl<bces_Platform::Win32Threads>
//..
// This template class should not be used (directly) by client code.  Clients
// should instead use 'bcemt_Condition'.
//
///Usage
///-----
// This component is an implementation detail of 'bcemt' and is *not* intended
// for direct client use.  It is subject to change without notice.  As such, a
// usage example is not provided.

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCES_PLATFORM
#include <bces_platform.h>
#endif

#ifdef BCES_PLATFORM_WIN32_THREADS

// Platform-specific implementation starts here.

#ifndef INCLUDED_BCEMT_SLUICE
#include <bcemt_sluice.h>
#endif

#ifndef INCLUDED_BSLMA_MALLOCFREEALLOCATOR
#include <bslma_mallocfreeallocator.h>
#endif

namespace BloombergLP {

template <typename THREAD_POLICY>
class bcemt_ConditionImpl;

class bcemt_Mutex;

             // ======================================================
             // class bcemt_ConditionImpl<bces_Platform::Win32Threads>
             // ======================================================

template <>
class bcemt_ConditionImpl<bces_Platform::Win32Threads> {
    // This class provides a full specialization of 'bcemt_ConditionImpl'
    // for win32.  The implementation provided here defines an efficient
    // POSIX like condition variable.

    // DATA
    bcemt_Sluice d_waitSluice;  // TBD doc

    // NOT IMPLEMENTED
    bcemt_ConditionImpl(const bcemt_ConditionImpl&);
    bcemt_ConditionImpl& operator=(const bcemt_ConditionImpl&);

  public:
    // CREATORS
    bcemt_ConditionImpl();
        // Create a condition variable.

    ~bcemt_ConditionImpl();
        // Destroy condition variable this object.

    // MANIPULATORS
    void broadcast();
        // Signal this condition object, by waking up *all* threads that are
        // currently waiting on this condition.

    void signal();
        // Signal this condition object, by waking up a single thread that is
        // currently waiting on this condition.

    int timedWait(bcemt_Mutex *mutex, const bdet_TimeInterval& timeout);
        // Atomically unlock the specified 'mutex' and suspend execution of the
        // current thread until this condition object is "signaled" (i.e., one
        // of the 'signal' or 'broadcast' methods is invoked on this object) or
        // until the specified 'timeout' (expressed as the !ABSOLUTE! time from
        // 00:00:00 UTC, January 1, 1970), then re-acquire a lock on the
        // 'mutex'.  Return 0 on success, -1 on timeout, and a non-zero value
        // different from -1 if an error occurs.  The behavior is undefined
        // unless 'mutex' is locked by the calling thread prior to calling this
        // method.  Note that 'mutex' remains locked by the calling thread upon
        // returning from this function with success or timeout, but is
        // *not* guaranteed to remain locked otherwise.  Also note that
        // spurious wakeups are rare but possible, i.e., this method may
        // succeed (return 0) and return control to the thread without the
        // condition object being signaled.

    int wait(bcemt_Mutex *mutex);
        // Atomically unlock the specified 'mutex' and suspend execution of the
        // current thread until this condition object is "signaled" (i.e., one
        // of the 'signal' or 'broadcast' methods is invoked on this object),
        // then re-acquire the lock on the 'mutex'.  Return 0 upon success and
        // a non-zero value otherwise.  The behavior is undefined unless
        // 'mutex' is locked by the calling thread prior to calling this
        // method.  Note that 'mutex' remains locked by the calling thread upon
        // successfully returning from this function, but is *not* guaranteed
        // to remain locked otherwise.  Also note that spurious wakeups are
        // rare but possible, i.e., this method may succeed and return control
        // to the thread without the condition object being signaled.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

             // ------------------------------------------------------
             // class bcemt_ConditionImpl<bces_Platform::Win32Threads>
             // ------------------------------------------------------

// CREATORS
inline
bcemt_ConditionImpl<bces_Platform::Win32Threads>::bcemt_ConditionImpl()
: d_waitSluice(&bslma_MallocFreeAllocator::singleton())
{
    // We use the malloc/free allocator singleton so as not to produce "noise"
    // in any tests involving the global allocator or global new/delete.
}

inline
bcemt_ConditionImpl<bces_Platform::Win32Threads>::~bcemt_ConditionImpl()
{
}

// MANIPULATORS
inline
void bcemt_ConditionImpl<bces_Platform::Win32Threads>::broadcast()
{
    d_waitSluice.signalAll();
}

inline
void bcemt_ConditionImpl<bces_Platform::Win32Threads>::signal()
{
    d_waitSluice.signalOne();
}

}  // close namespace BloombergLP

#endif  // BCES_PLATFORM_WIN32_THREADS

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
