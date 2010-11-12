// bcemt_threadattributesimpl_win32.h                                 -*-C++-*-
#ifndef INCLUDED_BCEMT_THREADATTRIBUTESIMPL_WIN32
#define INCLUDED_BCEMT_THREADATTRIBUTESIMPL_WIN32

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a win32 implementation of 'bcemt_ThreadAttributes'.
//
//@CLASSES:
//  bcemt_ThreadAttributesImpl<Win32Threads>: win32 specialization
//
//@SEE_ALSO: bcemt_threadattributes
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@DESCRIPTION: This component provides an implementation of
// 'bcemt_ThreadAttributes' for Windows (win32) via the template
// specialization:
//..
//  bcemt_ThreadAttributesImpl<bces_Platform::Win32Threads>
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

namespace BloombergLP {

template <typename THREAD_POLICY>
class bcemt_ThreadAttributesImpl;

        // =============================================================
        // class bcemt_ThreadAttributesImpl<bces_Platform::Win32Threads>
        // =============================================================

template <>
class bcemt_ThreadAttributesImpl<bces_Platform::Win32Threads> {
    // This class provides a full specialization of
    // 'bcemt_ThreadAttributesImpl' for the win32 platform.  This class
    // contains attributes used to create threads.  Note that many attributes
    // specified here have no effect on the win32 platform.

  public:
    // TYPES
    typedef bcemt_ThreadAttributesImpl<bces_Platform::Win32Threads>
                                                               NativeAttribute;

    // TYPES
    enum DetachedState {
        // This enumeration provides two values used to distinguish between a
        // joinable to a non-joinable thread.

        BCEMT_CREATE_JOINABLE = 0,  // create a joinable thread
        BCEMT_CREATE_DETACHED = 1   // create a non-joinable thread
    };

    enum SchedulingPolicy {
        // This enumeration provides values used to distinguish between
        // different thread scheduling policies.

        BCEMT_SCHED_OTHER = 0,  // default OS scheduling policy
        BCEMT_SCHED_FIFO  = 1,  // First-In-First-Out scheduling policy
        BCEMT_SCHED_RR    = 2   // Round-Robin scheduling policy
    };

  private:
    // DATA
    int              d_stackSize;         // stack size attribute

    int              d_priority;          // scheduling priority

    int              d_guardSize;         // guard size (not used by win32)

    SchedulingPolicy d_schedulingPolicy;  // scheduling policy (not used by
                                          // win32)

    DetachedState    d_detachedState;     // detached attribute

    int              d_inheritSchedule;   // inherit schedule attribute (not
                                          // used in win32)

  public:
    // CLASS METHODS
    static int getMaxSchedPriority(int policy = -1);
        // Return the non-negative maximum available priority for the
        // optionally-specified 'policy' on success.  If no policy is
        // specified, the maximum priority for the current policy is returned.
        // Return a negative value on error.  Note that on Win32, this method
        // always returns -1.

    static int getMinSchedPriority(int policy = -1);
        // Return the non-negative minimum available priority for the
        // optionally-specified 'policy' on success.  If no policy is
        // specified, the minimum priority for the current policy is returned.
        // Return a negative value on error.  Note that on Win32, this method
        // always returns -1.

    // CREATORS
    bcemt_ThreadAttributesImpl();
        // Create a thread attribute object with the following default values:
        //..
        //      Attribute             Default Value
        //      -------------------+----------------------------
        //      DetachedState      |  BCEMT_CREATE_JOINABLE
        //      SchedulingPolicy   |  BCEMT_SCHED_OTHER
        //      SchedulingPriority |  *platform's default value*
        //      InheritSchedule    |  TRUE
        //      StackSize          |  *platform's default value*
        //      GuardSize          |  *platform's default value*
        //..

    bcemt_ThreadAttributesImpl(const bcemt_ThreadAttributesImpl& original);
        // Create a thread attribute object initialized with the values in the
        // specified 'original' attribute object.

    ~bcemt_ThreadAttributesImpl();
        // Destroy this thread attribute object.

    // MANIPULATORS
    bcemt_ThreadAttributesImpl& operator=(
                                        const bcemt_ThreadAttributesImpl& rhs);
        // Assign to this attribute object the value of the specified 'rhs'
        // attribute object and return a reference to this modifiable
        // attribute object.

    void setDetachedState(DetachedState detachedState);
        // Set the value of the detached state attribute in this object to the
        // specified 'detachedState'.

    void setGuardSize(int guardSize);
        // Set the value of the guard size attribute in this object
        // to the specified 'guardSize' (in bytes).  Note that if
        // 'guardSize' < 0, the default value as defined by the platform is
        // used (which is typically the size of a page).

    void setInheritSchedule(int inheritSchedule);
        // Set the value of the inherit schedule attribute to 0 if
        // 'inheritSchedule' is 0 and set it to 1 otherwise.  A value of 0 for
        // this attribute will indicate that a thread should *not* inherit the
        // scheduling policy and priority of the thread that created it, and a
        // value of 1 indicates that the thread should inherit these
        // attributes.  Note that if the attributes are inherited, the values
        // of the scheduling policy and priority attributes in the
        // 'bcemt_ThreadAttributesImpl' object (supplied to
        // 'bcemt_ThreadUtilImpl::create' to spawn the thread) are ignored.

    void setSchedulingPolicy(SchedulingPolicy schedulingPolicy);
        // Set the value of the scheduling policy attribute in this object to
        // the specified 'schedulingPolicy'.

    void setSchedulingPriority(int schedulingPriority);
        // Set the value of the scheduling priority attribute in this object
        // to the specified 'schedulingPriority'.

    void setStackSize(int stackSize);
        // Set the value of the stack size attribute in this object
        // to the specified 'stackSize' (in bytes).  Note that if
        // 'stackSize' < 0, the default value as defined by the platform is
        // used.

    // ACCESSORS
    DetachedState detachedState() const;
        // Return the value of the detached state attribute in this object.

    int guardSize() const;
        // Return the value of the guard size attribute in this object.

    int inheritSchedule() const;
        // Return 1 if threads (created with this object) will inherit the
        // "parent" threads scheduling policy and priority, and 0 if they do
        // not.

    const NativeAttribute& nativeAttribute() const;
        // Return a reference to this object.

    SchedulingPolicy schedulingPolicy() const;
        // Return the value of the scheduling policy attribute in this object.

    int schedulingPriority() const;
        // Return the value of the scheduling priority attribute in this
        // object.  A value of -1 indicates that the scheduling priority is
        // set the other platform's default value.

    int stackSize() const;
        // Return the value of the stack size attribute in this object.
};

// FREE OPERATORS
bool
operator==(const bcemt_ThreadAttributesImpl<bces_Platform::Win32Threads>& lhs,
           const bcemt_ThreadAttributesImpl<bces_Platform::Win32Threads>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.

bool
operator!=(const bcemt_ThreadAttributesImpl<bces_Platform::Win32Threads>& lhs,
           const bcemt_ThreadAttributesImpl<bces_Platform::Win32Threads>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

        // -------------------------------------------------------------
        // class bcemt_ThreadAttributesImpl<bces_Platform::Win32Threads>
        // -------------------------------------------------------------

// CLASS METHODS
inline
int bcemt_ThreadAttributesImpl<bces_Platform::Win32Threads>::
                                                       getMaxSchedPriority(int)
{
    return -1;
}

inline
int bcemt_ThreadAttributesImpl<bces_Platform::Win32Threads>::
                                                       getMinSchedPriority(int)
{
    return -1;
}

// CREATORS
inline
bcemt_ThreadAttributesImpl<bces_Platform::Win32Threads>::
                                                  ~bcemt_ThreadAttributesImpl()
{
}

// MANIPULATORS
inline
void bcemt_ThreadAttributesImpl<bces_Platform::Win32Threads>::setDetachedState(
        bcemt_ThreadAttributesImpl<bces_Platform::Win32Threads>::DetachedState
                                                                 detachedState)
{
    d_detachedState = detachedState;
}

inline
void bcemt_ThreadAttributesImpl<bces_Platform::Win32Threads>::setGuardSize(
                                                                 int guardSize)
{
    d_guardSize = guardSize;

}

inline
void
bcemt_ThreadAttributesImpl<bces_Platform::Win32Threads>::setInheritSchedule(
                                                           int inheritSchedule)
{
    d_inheritSchedule = inheritSchedule ? 1 : 0;
}

inline
void
bcemt_ThreadAttributesImpl<bces_Platform::Win32Threads>::setSchedulingPolicy(
    bcemt_ThreadAttributesImpl<bces_Platform::Win32Threads>::SchedulingPolicy
                                                              schedulingPolicy)
{
    d_schedulingPolicy = schedulingPolicy;
}

inline
void bcemt_ThreadAttributesImpl<bces_Platform::Win32Threads>::
                                  setSchedulingPriority(int schedulingPriority)
{
    d_priority = schedulingPriority;
}

inline
void bcemt_ThreadAttributesImpl<bces_Platform::Win32Threads>::setStackSize(
                                                                 int stackSize)
{
    d_stackSize = stackSize;
}

// ACCESSORS
inline
bcemt_ThreadAttributesImpl<bces_Platform::Win32Threads>::DetachedState
bcemt_ThreadAttributesImpl<bces_Platform::Win32Threads>::detachedState() const
{
    return d_detachedState;
}

inline
int bcemt_ThreadAttributesImpl<bces_Platform::Win32Threads>::guardSize() const
{
    return d_guardSize;
}

inline
int bcemt_ThreadAttributesImpl<bces_Platform::Win32Threads>::
                                                        inheritSchedule() const
{
    return d_inheritSchedule;
}

inline
const bcemt_ThreadAttributesImpl<bces_Platform::Win32Threads>::NativeAttribute&
bcemt_ThreadAttributesImpl<bces_Platform::Win32Threads>::nativeAttribute()
                                                                          const
{
    return *this;
}

inline
bcemt_ThreadAttributesImpl<bces_Platform::Win32Threads>::SchedulingPolicy
bcemt_ThreadAttributesImpl<bces_Platform::Win32Threads>::
                                                       schedulingPolicy() const
{
    return d_schedulingPolicy;
}

inline
int
bcemt_ThreadAttributesImpl<bces_Platform::Win32Threads>::
                                                     schedulingPriority() const
{
    return d_priority;
}

inline
int bcemt_ThreadAttributesImpl<bces_Platform::Win32Threads>::stackSize() const
{
    return d_stackSize;
}

// FREE OPERATORS
inline
bool
operator!=(const bcemt_ThreadAttributesImpl<bces_Platform::Win32Threads>& lhs,
           const bcemt_ThreadAttributesImpl<bces_Platform::Win32Threads>& rhs)
{
    return !(lhs == rhs);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
