// bcemt_threadattributes.h                                           -*-C++-*-
#ifndef INCLUDED_BCEMT_THREADATTRIBUTES
#define INCLUDED_BCEMT_THREADATTRIBUTES

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a value-semantic container of thread attributes.
//
//@CLASSES:
//  bcemt_ThreadAttributes: container of thread attributes
//
//@SEE_ALSO: bcemt_threadutil
//
//@DESCRIPTION: This component provides a value-semantic attribute class,
// 'bcemt_ThreadAttributes', for storing attributes of a thread in a
// platform-independent way.
//
// The default values for the various thread attributes supported by
// 'bcemt_ThreadAttributes' are listed in the following table:
//..
//  +--------------------+------------------------+---------------------------+
//  | Attribute          | Default Value          | Description               |
//  +====================+========================+===========================+
//  | DetachedState      | BCEMT_CREATE_JOINABLE  | see 'DetachedState'       |
//  |                    |                        | enumeration               |
//  +--------------------+------------------------+---------------------------+
//  | SchedulingPolicy   | BCEMT_SCHED_OTHER      | see 'SchedulingPolicy'    |
//  |                    |                        | enumeration -             |
//  |                    |                        | ignored unless            |
//  |                    |                        | '0 == inheritSchedule'    |
//  +--------------------+------------------------+---------------------------+
//  | SchedulingPriority | default value for      | thread priority - ignored |
//  |                    | platform               | unless                    |
//  |                    |                        | '0 == inheritSchedule'    |
//  +--------------------+------------------------+---------------------------+
//  | InheritSchedule    | 1                      | whether to inherit        |
//  |                    |                        | scheduling policy from    |
//  |                    |                        | parent thread -           |
//  |                    |                        | thread creation fails     |
//  |                    |                        | under many circumstances  |
//  |                    |                        | unless this is '1' (see   |
//  |                    |                        | 'setInheritSchedule')     |
//  +--------------------+------------------------+---------------------------+
//  | StackSize          | BCEMT_UNSET_STACK_SIZE | size (in bytes) of        |
//  |                    |                        | thread stack              |
//  +--------------------+------------------------+---------------------------+
//  | GuardSize          | BCEMT_UNSET_GUARD_SIZE | size (in bytes) of        |
//  |                    |                        | the guard area in         |
//  |                    |                        | thread's stack            |
//  +--------------------+------------------------+---------------------------+
//..
// Note that, with the exception of 'DetachedState' and 'StackSize', specifying
// the values of the thread attributes only provides a hint to the operating
// system and those values may be ignored.  Also note that all attributes other
// than 'DetachedState' and 'StackSize' are ignored on Windows.
//
///DetachedState
///-------------
// A thread can be created in either a "joinable" or "detached" state, by
// setting the 'detachedState' attribute to
// 'bcemt_ThreadAttributes::BCEMT_CREATE_JOINABLE' or
// 'bcemt_ThreadAttributes::BCEMT_CREATE_DETACHED', respectively.  Any thread
// can join with a "joinable" thread (i.e., the joining thread can block,
// waiting for the joinable thread's execution to complete) by invoking the
// 'bcemt_ThreadUtil::join' function on the joinable thread's 'Handle':
//..
//  int join(Handle handle, void **returnArg);
//  int detach(Handle handle);
//..
// Threads cannot join with a thread that was created in a detached state.  A
// joinable thread can also be put into the detached state via a call to the
// 'bcemt_ThreadUtil::detach' method.
//
// Note that a thread must be joined or detached before it completes.  That is,
// if a joinable thread is not joined before it completes, resources allocated
// by the kernel and possibly by the 'bcemt' library will be leaked.  A
// detached thread will be deallocated properly when it completes.
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Creating and modifying thread attributes objects
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// First we create a default-constructed 'bcemt_ThreadAttributes' object and
// assert that its detached state does indeed have the default value (i.e.,
// 'bcemt_ThreadAttributes::BCEMT_CREATE_JOINABLE'):
//..
//  bcemt_ThreadAttributes attributes;
//  assert(bcemt_ThreadAttributes::BCEMT_CREATE_JOINABLE ==
//                                                 attributes.detachedState());
//..
// Next we modify the detached state of 'attributes' to have the non-default
// value 'bcemt_ThreadAttributes::BCEMT_CREATE_DETACHED':
//..
//  attributes.setDetachedState(bcemt_ThreadAttributes::BCEMT_CREATE_DETACHED);
//  assert(bcemt_ThreadAttributes::BCEMT_CREATE_DETACHED ==
//                                                 attributes.detachedState());
//..
// Finally, we make a copy of 'attributes':
//..
//  bcemt_ThreadAttributes copy(attributes);
//  assert(bcemt_ThreadAttributes::BCEMT_CREATE_DETACHED ==
//                                                       copy.detachedState());
//  assert(attributes == copy);
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_CSTRING
#include <bsl_cstring.h>
#endif

namespace BloombergLP {

                           // ============================
                           // class bcemt_ThreadAttributes
                           // ============================

class bcemt_ThreadAttributes {
    // This 'class' implements a container of thread attributes for specifying
    // thread creation characteristics.

  public:
    // PUBLIC TYPES
    enum DetachedState {
        // This enumeration provides two values used to distinguish between a
        // joinable thread and a non-joinable (detached) thread.

        BCEMT_CREATE_JOINABLE = 0,  // create a joinable thread
        BCEMT_CREATE_DETACHED = 1   // create a non-joinable thread

#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , CREATE_JOINABLE = BCEMT_CREATE_JOINABLE
      , CREATE_DETACHED = BCEMT_CREATE_DETACHED
#endif
    };

    enum SchedulingPolicy {
        // This enumeration provides values used to distinguish between
        // different thread scheduling policies.  Ignored on Windows.  Ignored
        // unless 'inheritSchedule' is set to 'false' (the non-default).

        BCEMT_SCHED_OTHER = 0,  // default OS scheduling policy
        BCEMT_SCHED_FIFO  = 1,  // first-in-first-out scheduling policy
        BCEMT_SCHED_RR    = 2   // round-robin scheduling policy
    };

    enum {
        // The 'stackSize' and 'guardSize' attributes are initialized to these
        // values when a thread attributes object is default constructed.

        BCEMT_UNSET_STACK_SIZE = -1,
        BCEMT_UNSET_GUARD_SIZE = -1
    };

  private:
    // DATA
    DetachedState    d_detachedState;
    int              d_guardSize;
    int              d_inheritSchedule;
    SchedulingPolicy d_schedulingPolicy;
    int              d_schedulingPriority;
    int              d_stackSize;

    // FRIENDS
    friend bool operator==(const bcemt_ThreadAttributes&,
                           const bcemt_ThreadAttributes&);
    friend bool operator!=(const bcemt_ThreadAttributes&,
                           const bcemt_ThreadAttributes&);

  public:
    // CLASS METHODS
    static int getMaxSchedPriority(int policy = -1);
        // Return the non-negative maximum available priority for the
        // optionally-specified 'policy' on success.  If no policy is
        // specified, the maximum priority for the process's policy is
        // returned.  Return 'INT_MIN' on error.  Note that for some
        // platform / policy cominations,
        // 'getMinSchedPriority(policy) == getMaxSchedPriority(policy)'.

    static int getMinSchedPriority(int policy = -1);
        // Return the non-negative minimum available priority for the
        // optionally-specified 'policy' on success.  If no policy is
        // specified, the minimum priority for the process's policy is
        // returned.  Return 'INT_MIN' on error.  Note that for some
        // platform / policy cominations,
        // 'getMinSchedPriority(policy) == getMaxSchedPriority(policy)'.

    // CREATORS
    bcemt_ThreadAttributes();
        // Create a thread attributes object having default values for its
        // attributes.  (See the component-level documentation for the default
        // values to which to attributes supported by 'bcemt_ThreadAttributes'
        // are initialized.)

    bcemt_ThreadAttributes(const bcemt_ThreadAttributes& original);
        // Create a thread attributes object with value identical to that of
        // the specified 'original'.

    ~bcemt_ThreadAttributes();
        // Destroy this thread attributes object.

    // MANIPULATORS
    bcemt_ThreadAttributes& operator=(const bcemt_ThreadAttributes& rhs);
        // Set all attributes of this thread attributes object to the values of
        // the specified 'rhs'.

    // MANIPULATORS
    void setDetachedState(DetachedState detachedState);
        // Set the value of the detached state attribute of this thread
        // attributes object to the specified 'detachedState'.

    void setGuardSize(int guardSize);
        // Set the value of the guard size attribute of this thread attributes
        // object to the specified 'guardSize' (in bytes).  If 'guardSize < 0',
        // the default value as defined by the platform is used (which is
        // typically the size of a page).  This attribute is ignored on
        // Windows.  If guard size has not been set, 'bcemt_ThreadUtil::create'
        // will use the value 'bcemt_Default::nativeThreadGuardSize()'.

    void setInheritSchedule(int inheritSchedule);
        // Set the value of the inherit schedule attribute of this thread
        // attributes object to 0 if the specified 'inheritSchedule' is 0, and
        // set the attribute to 1 otherwise.  A value of 0 for the inherit
        // schedule attribute indicates that a thread should *not* inherit the
        // scheduling policy and priority of the thread that created it, and a
        // value of 1 indicates that the thread *should* inherit these
        // attributes.  This attribute is ignored on Windows.  Note that on all
        // Unix platforms other than Solaris, thread creation fails unless
        // '1 == inheritSchedule()', at least unless the process has superuser
        // priviledges.

    void setSchedulingPolicy(SchedulingPolicy schedulingPolicy);
        // Set the value of the scheduling policy attribute of this thread
        // attributes object to the specified 'schedulingPolicy'.  This
        // attribute is ignored on Windows.  Unless '0 == inheritSchedule()'
        // this attribute is ignored.  If necessary this method will adjust
        // 'schedulingPriority' to ensure that it is in the range
        // '[ getMinSchedPriority(policy), getMaxSchedPriority(policy) ]'.

    void setSchedulingPriority(int schedulingPriority);
        // Set the value of the scheduling priority attribute of this thread
        // attributes object to the specified 'schedulingPriority'.  This
        // attribute is ignored on Windows.  Unless '0 == inheritSchedule()'
        // this attribute is ignored.  On Solaris, higher numbers indicate more
        // urgent priorities.  Note that this attribute has little or no effect
        // except when multiple threads are blocked on a mutex, in which case
        // it influences which thread is to acquire the mutex first.

    void setStackSize(int stackSize);
        // Set the value of the stack size attribute of this thread attributes
        // object to the specified 'stackSize' (in bytes).  If 'stackSize' has
        // not been set, 'bcemt_ThreadUtil::create' will use the value
        // 'bcemt_Default::defaultThreadStackSize()'.

    // ACCESSORS
    DetachedState detachedState() const;
        // Return the value of the detached state attribute of thread
        // attributes object.

    int guardSize() const;
        // Return the value of the guard size attribute of this thread
        // attributes object.

    int inheritSchedule() const;
        // Return 1 if threads created using this thread attributes object
        // inherit the scheduling policy and priority from the "parent" thread,
        // and 0 otherwise.  Note that under most circumstances, thread
        // creation fails unless '1 == inheritSchedule()'.

    SchedulingPolicy schedulingPolicy() const;
        // Return the value of the scheduling policy attribute of this thread
        // attributes object.  Note that unless '0 == inheritSchedule()', this
        // attribute is ignored.

    int schedulingPriority() const;
        // Return the value of the scheduling priority attribute of this thread
        // attributes object.  A value of -1 indicates that the scheduling
        // priority is set to the platform's default value.  Note that unless
        // '0 == inheritSchedule()', this attribute is ignored.

    int stackSize() const;
        // Return the value of the stack size attribute of this thread
        // attributes object.
};

// FREE OPERATORS
bool operator==(const bcemt_ThreadAttributes& lhs,
                const bcemt_ThreadAttributes& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' thread attributes objects
    // have the same value, and 'false' otherwise.  Two thread attributes
    // objects have the same value if they have the same values for all of
    // their respective attributes.

bool operator!=(const bcemt_ThreadAttributes& lhs,
                const bcemt_ThreadAttributes& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' thread attributes objects
    // do not have the same value, and 'false' otherwise.  Two thread
    // attributes objects do not have the same value if any of their respective
    // attributes differ in value.

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                         // ----------------------------
                         // class bcemt_ThreadAttributes
                         // ----------------------------

// CREATORS
inline
bcemt_ThreadAttributes::bcemt_ThreadAttributes(
                                        const bcemt_ThreadAttributes& original)
{
    bsl::memcpy(this, &original, sizeof(*this));
}

inline
bcemt_ThreadAttributes::~bcemt_ThreadAttributes()
{}

// MANIPULATORS
inline
bcemt_ThreadAttributes& bcemt_ThreadAttributes::operator=(
                                             const bcemt_ThreadAttributes& rhs)
{
    bsl::memcpy(this, &rhs, sizeof(*this));

    return *this;
}

inline
void bcemt_ThreadAttributes::setDetachedState(
                           bcemt_ThreadAttributes::DetachedState detachedState)
{
    d_detachedState = detachedState;
}

inline
void bcemt_ThreadAttributes::setGuardSize(int guardSize)
{
    d_guardSize = guardSize;
}

inline
void bcemt_ThreadAttributes::setInheritSchedule(int inheritSchedule)
{
    d_inheritSchedule = inheritSchedule;
}

inline
void bcemt_ThreadAttributes::setSchedulingPriority(int schPriority)
{
    d_schedulingPriority = schPriority;
}

inline
void bcemt_ThreadAttributes::setStackSize(int stackSize)
{
    d_stackSize = stackSize;
}

// ACCESSORS
inline
bcemt_ThreadAttributes::DetachedState
bcemt_ThreadAttributes::detachedState() const
{
    return d_detachedState;
}

inline
int bcemt_ThreadAttributes::guardSize() const
{
    return d_guardSize;
}

inline
int bcemt_ThreadAttributes::inheritSchedule() const
{
    return d_inheritSchedule;
}

inline
bcemt_ThreadAttributes::SchedulingPolicy
bcemt_ThreadAttributes::schedulingPolicy() const
{
    return d_schedulingPolicy;
}

inline
int bcemt_ThreadAttributes::schedulingPriority() const
{
    return d_schedulingPriority;
}

inline
int bcemt_ThreadAttributes::stackSize() const
{
    return d_stackSize;
}

// FREE OPERATORS
inline
bool operator!=(const bcemt_ThreadAttributes& lhs,
                const bcemt_ThreadAttributes& rhs)
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
