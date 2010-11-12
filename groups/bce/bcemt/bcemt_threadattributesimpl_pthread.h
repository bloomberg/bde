// bcemt_threadattributesimpl_pthread.h                               -*-C++-*-
#ifndef INCLUDED_BCEMT_THREADATTRIBUTESIMPL_PTHREAD
#define INCLUDED_BCEMT_THREADATTRIBUTESIMPL_PTHREAD

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a POSIX implementation of 'bcemt_ThreadAttributes'.
//
//@CLASSES:
//  bcemt_ThreadAttributesImpl<PosixThreads>: POSIX specialization
//
//@SEE_ALSO: bcemt_threadattributes
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@DESCRIPTION: This component provides an implementation of
// 'bcemt_ThreadAttributes' for POSIX threads ("pthreads") via the template
// specialization:
//..
//  bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads>
//..
// This template class should not be used (directly) by client code.  Clients
// should instead use 'bcemt_ThreadAttributes'.
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

#ifdef BCES_PLATFORM__POSIX_THREADS

// Platform-specific implementation starts here.

#ifndef INCLUDED_PTHREAD
#include <pthread.h>
#define INCLUDED_PTHREAD
#endif

namespace BloombergLP {

template <typename THREAD_POLICY>
class bcemt_ThreadAttributesImpl;

        // =============================================================
        // class bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads>
        // =============================================================

template <>
class bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads> {
    // This class provides a full specialization of
    // 'bcemt_ThreadAttributesImpl' for pthreads.

    // DATA
    pthread_attr_t d_imp;  // thread attribute structure

  public:
    // TYPES
    typedef pthread_attr_t NativeAttribute;

    enum DetachedState {
        // This enumeration provides two values used to distinguish between
        // joinable and non-joinable threads.

        BCEMT_CREATE_JOINABLE = 0,  // create a joinable thread
        BCEMT_CREATE_DETACHED = 1   // create a non-joinable thread
    };

    enum SchedulingPolicy {
        // This enumeration provides values used to distinguish between
        // different thread scheduling policies.

        BCEMT_SCHED_OTHER = 0,  // default OS scheduling policy
        BCEMT_SCHED_FIFO  = 1,  // first-In-First-Out scheduling policy
        BCEMT_SCHED_RR    = 2   // round-Robin scheduling policy
    };

    // CLASS METHODS
    static int getMaxSchedPriority(int policy = -1);
        // Return the non-negative maximum available priority for the
        // optionally-specified 'policy' on success.  If no policy is
        // specified, the maximum priority for the current policy is returned.
        // Return a negative value on error.

    static int getMinSchedPriority(int policy = -1);
        // Return the non-negative minimum available priority for the
        // optionally-specified 'policy' on success.  If no policy is
        // specified, the minimum priority for the current policy is returned.
        // Return a negative value on error.

    // CREATORS
    bcemt_ThreadAttributesImpl();
        // Create a thread attribute object with the following default values:
        //..
        //      Attribute             Default Value
        //      -------------------+---------------------------------------
        //      DetachedState      |  BCEMT_CREATE_JOINABLE
        //      SchedulingPolicy   |  *BDE's* *default* *value* (see above)
        //      SchedulingPriority |  *platform's* *default* *value*
        //      InheritSchedule    |  TRUE
        //      StackSize          |  *platform's* *default* *value*
        //      GuardSize          |  *platform's* *default* *value*
        //..

    bcemt_ThreadAttributesImpl(const NativeAttribute& attribute);
        // Create a thread attribute object initialized with the values of the
        // specified native 'attribute' object.

    bcemt_ThreadAttributesImpl(const bcemt_ThreadAttributesImpl& original);
        // Create a thread attribute object initialized to the value of the
        // specified 'original' attribute object.

    ~bcemt_ThreadAttributesImpl();
        // Destroy this 'bcemt_ThreadAttributesImpl' object.

    bcemt_ThreadAttributesImpl& operator=(
                                        const bcemt_ThreadAttributesImpl& rhs);
        // Assign to this attribute object the value of the specified 'rhs'
        // attribute object, and return a reference to this modifiable
        // attribute object.

    // MANIPULATORS
    void setDetachedState(DetachedState detachedState);
        // Set the value of the detached state attribute to the specified
        // 'detachedState'.

    void setGuardSize(int guardSize);
        // Set the value of the guard size attribute in this object
        // to the specified 'guardSize' (in bytes).  Note that if
        // 'guardSize < 0', the default value as defined by the platform is
        // used (which is typically the size of a page).

    void setInheritSchedule(int inheritSchedule);
        // Set the value of the inherit schedule attribute to 0 if the
        // specified 'inheritSchedule' is 0 and set this attribute to 1
        // otherwise.  A value of 0 for this attribute will indicate that a
        // thread should *not* inherit the scheduling policy and priority of
        // the thread that created it, and a value of 1 indicates that the
        // thread should inherit these attributes.  Note that if the attributes
        // are inherited, the values of the scheduling policy and priority
        // attributes in the 'bcemt_ThreadAttributesImpl' object (supplied to
        // 'bcemt_ThreadUtilImpl::create' to spawn the thread) are ignored.

    void setSchedulingPolicy(SchedulingPolicy schedulingPolicy);
        // Set the value of the scheduling policy attribute to the specified
        // 'schedulingPolicy'.

    void setSchedulingPriority(int schedulingPriority);
        // Set the value of the scheduling priority attribute to the specified
        // 'schedulingPriority'.

    void setStackSize(int stackSize);
        // Set the value of the stack size attribute in this object
        // to the specified 'stackSize' (in bytes).  Note that if
        // 'stackSize < 0', the default value as defined by the platform is
        // used.

    // ACCESSORS
    DetachedState detachedState() const;
        // Return the value of the detached state attribute.

    int guardSize() const;
        // Return the value of the guard size attribute.

    int inheritSchedule() const;
        // Return 1 if threads (created with this object) will inherit the
        // "parent" threads scheduling policy and priority, and 0 if they do
        // not.

    const NativeAttribute& nativeAttribute() const;
        // Return a const reference to the native 'pthread_attr_t' object used
        // by this attribute object.

    SchedulingPolicy schedulingPolicy() const;
        // Return the value of the scheduling policy attribute.

    int schedulingPriority() const;
        // Return the value of the scheduling priority attribute.  A value of
        // -1 indicates that the scheduling priority is set the other
        // platform's default value.

    int stackSize() const;
        // Return the value of the stack size attribute.
};

// FREE OPERATORS
bool
operator==(const bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads>& lhs,
           const bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.

bool
operator!=(const bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads>& lhs,
           const bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

        // -------------------------------------------------------------
        // class bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads>
        // -------------------------------------------------------------

// CREATORS
inline
bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads>::
                                                  ~bcemt_ThreadAttributesImpl()
{
    pthread_attr_destroy(&d_imp);
}

// MANIPULATORS
inline
void bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads>::setDetachedState(
        bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads>::DetachedState
                                                                 detachedState)
{
    pthread_attr_setdetachstate(&d_imp,
                                BCEMT_CREATE_DETACHED == detachedState
                                ? PTHREAD_CREATE_DETACHED
                                : PTHREAD_CREATE_JOINABLE);
}

inline
void bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads>::setGuardSize(
                                                                 int guardSize)
{
#if !defined(BSLS_PLATFORM__OS_CYGWIN)
    pthread_attr_setguardsize(&d_imp, guardSize);
#endif
}

inline
void
bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads>::setInheritSchedule(
                                                           int inheritSchedule)
{
    pthread_attr_setinheritsched(&d_imp,
                                 inheritSchedule
                                 ? PTHREAD_INHERIT_SCHED
                                 : PTHREAD_EXPLICIT_SCHED);

}

// ACCESSORS
inline
const bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads>::NativeAttribute&
bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads>::nativeAttribute()
                                                                          const
{
    return d_imp;
}

// FREE OPERATORS
inline
bool
operator!=(const bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads>& lhs,
           const bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads>& rhs)
{
    return !(lhs == rhs);
}

}  // close namespace BloombergLP

#endif  // BCES_PLATFORM__POSIX_THREADS

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
