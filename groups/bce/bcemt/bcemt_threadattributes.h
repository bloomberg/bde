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
//  +--------------------+-----------------------+----------------------------+
//  | Attribute          | Default Value         | Description                |
//  +====================+=======================+============================+
//  | DetachedState      | BCEMT_CREATE_JOINABLE | see 'DetachedState'        |
//  |                    |                       | enumeration                |
//  +--------------------+-----------------------+----------------------------+
//  | SchedulingPolicy   | BCEMT_SCHED_OTHER     | see 'SchedulingPolicy'     |
//  |                    |                       | enumeration -              |
//  |                    |                       | ignored unless             |
//  |                    |                       | 'inheritSchedule' is set   |
//  |                    |                       | to 0                       |
//  +--------------------+-----------------------+----------------------------+
//  | SchedulingPriority | default value for     | ignored unless             |
//  |                    | platform              | 'inheritSchedule' is set   |
//  |                    |                       | to 0                       |
//  +--------------------+-----------------------+----------------------------+
//  | InheritSchedule    | 1                     | Whether to inherit         |
//  |                    |                       | scheduling policy from     |
//  |                    |                       | parent thread.             |
//  |                    |                       | Thread creation fails      |
//  |                    |                       | under most circumstances   |
//  |                    |                       | unless this is '1'.        |
//  +--------------------+-----------------------+----------------------------+
//  | StackSize          | BCEMT_INVALID_STACK_SIZE | size (in bytes) of      |
//  |                    |                          | thread stack            |
//  +--------------------+-----------------------+----------------------------+
//  | GuardSize          | default value for     | size (in bytes) of         |
//  |                    | platform              | the guard area in          |
//  |                    |                       | thread's stack             |
//  +--------------------+-----------------------+----------------------------+
//..
// Note that, with the exception of 'bcemt_ThreadAttributes::DetachedState',
// specifying the values of the thread attributes only provides a hint to the
// operating system and those values may be ignored.  Also note that all
// attributes other than 'DetachedState' and 'StackSize' are completely ignored
// on Windows.
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
///SchedulingPolicy and SchedulingPriority
///---------------------------------------
//
// Experimentation has shown that It is very hard to construe a situation where
// thread scheduling policy or priority make any detectable difference in
// behavior, and when they do that difference is very slight.
//
///Usage
///-----
// The following snippets of code illustrate basic use of this component.
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
        // unless 'inheritPolicy' is set to 'false' (the non-default).

        BCEMT_SCHED_OTHER = 0,  // default OS scheduling policy
        BCEMT_SCHED_FIFO  = 1,  // first-in-first-out scheduling policy
        BCEMT_SCHED_RR    = 2   // round-robin scheduling policy
    };

    enum {
        BCEMT_INVALID_STACK_SIZE = -1
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
        // specified, the maximum priority for the current policy is returned.
        // Return a negative value on error.

    static int getMinSchedPriority(int policy = -1);
        // Return the non-negative minimum available priority for the
        // optionally-specified 'policy' on success.  If no policy is
        // specified, the minimum priority for the current policy is returned.
        // Return a negative value on error.

    // CREATORS
    bcemt_ThreadAttributes();
        // Create a thread attributes object having default values for its
        // attributes.  (See the component-level documentation for the default
        // values to which to attributes supported by 'bcemt_ThreadAttributes'
        // are initialized.)

    // compiler generated:
    // bcemt_ThreadAttributes(const bcemt_ThreadAttributes& original);
    // ~bcemt_ThreadAttributes();

    // MANIPULATORS
    // bcemt_ThreadAttributes& operator=(const bcemt_ThreadAttributes& rhs);

    // MANIPULATORS
    void setDetachedState(DetachedState detachedState);
        // Set the value of the detached state attribute of this thread
        // attributes object to the specified 'detachedState'.

    void setGuardSize(int guardSize);
        // Set the value of the guard size attribute of this thread attributes
        // object to the specified 'guardSize' (in bytes).  If 'guardSize < 0',
        // the default value as defined by the platform is used (which is
        // typically the size of a page).

    void setInheritSchedule(int inheritSchedule);
        // Set the value of the inherit schedule attribute of this thread
        // attributes object to 0 if the specified 'inheritSchedule' is 0, and
        // set the attribute to 1 otherwise.  A value of 0 for the inherit
        // schedule attribute indicates that a thread should *not* inherit the
        // scheduling policy and priority of the thread that created it, and a
        // value of 1 indicates that the thread *should* inherit these
        // attributes.  Note that if the attributes are inherited and this
        // object is supplied to 'bcemt_ThreadUtil::create' to spawn a new
        // thread, the values of the scheduling policy and priority attributes
        // in this thread attributes object are ignored.  Also note that under
        // most circumstances, thread creation fails if
        // '0 == inheritSchedule()'.

    void setSchedulingPolicy(SchedulingPolicy schedulingPolicy);
        // Set the value of the scheduling policy attribute of this thread
        // attributes object to the specified 'schedulingPolicy'.  Note that
        // unless '0 == inheritSchedule()' this attribute is ignored.  Also
        // note that this may adjust 'schedulingPriority' to make sure it is in
        // the range 'min <= priority <= max', with 'min' and 'max' being
        // determined by the static methods 'get{Min,Max}SchedPriority' above.

    void setSchedulingPriority(int schedulingPriority);
        // Set the value of the scheduling priority attribute of this thread
        // attributes object to the specified 'schedulingPriority'.  Note that
        // unless '0 == inheritSchedule()' this attribute is ignored.

    void setStackSize(int stackSize);
        // Set the value of the stack size attribute of this thread attributes
        // object to the specified 'stackSize' (in bytes).  If 'stackSize' is
        // negative, 'bcemt_ThreadUtil::create' will use the current default
        // thread stack size (see 'bcemt_default').

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

// MANIPULATORS
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
