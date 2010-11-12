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
//@DESCRIPTION: This component provides a container, 'bcemt_ThreadAttributes',
// for storing attributes of a thread in a platform-independent way.
//
// The default values for the various thread attributes supported by
// 'bcemt_ThreadAttributes' are listed in the following table:
//..
//  +--------------------+----------------------------+---------------------+
//  | Attribute          | Default Value              | Description         |
//  +====================+============================+=====================+
//  | DetachedState      | BCEMT_CREATE_JOINABLE      | see 'DetachedState' |
//  |                    |                            | enumeration         |
//  +--------------------+----------------------------+---------------------+
//  | SchedulingPolicy   | BCEMT_SCHED_OTHER          | see                 |
//  |                    |                            | 'SchedulingPolicy'  |
//  |                    |                            | enumeration         |
//  +--------------------+----------------------------+---------------------+
//  | SchedulingPriority | default value for platform | scheduling priority |
//  |                    |                            |
//  +--------------------+----------------------------+---------------------+
//  | InheritSchedule    | true                       | whether to inherit  |
//  |                    |                            | scheduling policy   |
//  |                    |                            | from parent thread  |
//  +--------------------+----------------------------+---------------------+
//  | StackSize          | default value for platform | size (in bytes) of  |
//  |                    |                            | thread stack        |
//  +--------------------+----------------------------+---------------------+
//  | GuardSize          | default value for platform | size (in bytes) of  |
//  |                    |                            | the guard area in   |
//  |                    |                            | thread's stack      |
//  +--------------------+----------------------------+---------------------+
//..
// Note that, with the exception of 'bcemt_ThreadAttributes::DetachedState',
// specifying the values of the thread attributes only provides a hint to the
// operating system and those values may be ignored.
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

#ifndef INCLUDED_BCEMT_THREADATTRIBUTESIMPL_PTHREAD
#include <bcemt_threadattributesimpl_pthread.h>
#endif

#ifndef INCLUDED_BCEMT_THREADATTRIBUTESIMPL_WIN32
#include <bcemt_threadattributesimpl_win32.h>
#endif

#ifndef INCLUDED_BCES_PLATFORM
#include <bces_platform.h>
#endif

namespace BloombergLP {

template <typename THREAD_POLICY>
class bcemt_ThreadAttributesImpl;

                           // ============================
                           // class bcemt_ThreadAttributes
                           // ============================

class bcemt_ThreadAttributes {
    // This 'class' implements a container of thread attributes for specifying
    // thread creation characteristics.

  public:
    // TYPES
    typedef bcemt_ThreadAttributesImpl<bces_Platform::ThreadPolicy> Imp;

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
        // different thread scheduling policies.

        BCEMT_SCHED_OTHER = 0,  // default OS scheduling policy
        BCEMT_SCHED_FIFO  = 1,  // first-in-first-out scheduling policy
        BCEMT_SCHED_RR    = 2   // round-robin scheduling policy
    };

  private:
    // DATA
    Imp d_imp;  // platform-specific implementation

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

    bcemt_ThreadAttributes(const bcemt_ThreadAttributes& original);
        // Create a thread attributes object having the value of the specified
        // 'original' thread attributes object.

    ~bcemt_ThreadAttributes();
        // Destroy this thread attributes object.

    // MANIPULATORS
    bcemt_ThreadAttributes& operator=(const bcemt_ThreadAttributes& rhs);
        // Assign to this thread attributes object the value of the specified
        // 'rhs' thread attributes object, and return a reference to this
        // modifiable thread attributes object.

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
        // in this thread attributes object are ignored.

    void setSchedulingPolicy(SchedulingPolicy schedulingPolicy);
        // Set the value of the scheduling policy attribute of this thread
        // attributes object to the specified 'schedulingPolicy'.

    void setSchedulingPriority(int schedulingPriority);
        // Set the value of the scheduling priority attribute of this thread
        // attributes object to the specified 'schedulingPriority'.

    void setStackSize(int stackSize);
        // Set the value of the stack size attribute of this thread attributes
        // object to the specified 'stackSize' (in bytes).  If 'stackSize < 0',
        // the default value as defined by the platform is used.

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
        // and 0 otherwise.

    const Imp::NativeAttribute& nativeAttribute() const;
        // Return a reference to the non-modifiable platform-specific object
        // used to store data in this thread attributes object.

    SchedulingPolicy schedulingPolicy() const;
        // Return the value of the scheduling policy attribute of this thread
        // attributes object.

    int schedulingPriority() const;
        // Return the value of the scheduling priority attribute of this thread
        // attributes object.  A value of -1 indicates that the scheduling
        // priority is set to the platform's default value.

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

// CLASS METHODS
inline
int bcemt_ThreadAttributes::getMaxSchedPriority(int policy)
{
    return Imp::getMaxSchedPriority(policy);
}

inline
int bcemt_ThreadAttributes::getMinSchedPriority(int policy)
{
    return Imp::getMinSchedPriority(policy);
}

// CREATORS
inline
bcemt_ThreadAttributes::bcemt_ThreadAttributes()
{
}

inline
bcemt_ThreadAttributes::bcemt_ThreadAttributes(
                                        const bcemt_ThreadAttributes& original)
: d_imp(original.d_imp)
{
}

inline
bcemt_ThreadAttributes::~bcemt_ThreadAttributes()
{
}

// MANIPULATORS
inline
bcemt_ThreadAttributes&
bcemt_ThreadAttributes::operator=(const bcemt_ThreadAttributes& rhs)
{
    d_imp = rhs.d_imp;
    return *this;
}

inline
void bcemt_ThreadAttributes::setDetachedState(
                           bcemt_ThreadAttributes::DetachedState detachedState)
{
    d_imp.setDetachedState((Imp::DetachedState)detachedState);
}

inline
void bcemt_ThreadAttributes::setGuardSize(int guardSize)
{
    d_imp.setGuardSize(guardSize);
}

inline
void bcemt_ThreadAttributes::setInheritSchedule(int inheritSchedule)
{
    d_imp.setInheritSchedule(inheritSchedule);
}

inline
void bcemt_ThreadAttributes::setSchedulingPolicy(
                     bcemt_ThreadAttributes::SchedulingPolicy schedulingPolicy)
{
    d_imp.setSchedulingPolicy((Imp::SchedulingPolicy)schedulingPolicy);
}

inline
void bcemt_ThreadAttributes::setSchedulingPriority(int schPriority)
{
    d_imp.setSchedulingPriority(schPriority);
}

inline
void bcemt_ThreadAttributes::setStackSize(int stackSize)
{
    d_imp.setStackSize(stackSize);
}

// ACCESSORS
inline
bcemt_ThreadAttributes::DetachedState
bcemt_ThreadAttributes::detachedState() const
{
    return (bcemt_ThreadAttributes::DetachedState)d_imp.detachedState();
}

inline
int bcemt_ThreadAttributes::guardSize() const
{
    return d_imp.guardSize();
}

inline
int bcemt_ThreadAttributes::inheritSchedule() const
{
    return d_imp.inheritSchedule();
}

inline
const bcemt_ThreadAttributes::Imp::NativeAttribute&
bcemt_ThreadAttributes::nativeAttribute() const
{
    return d_imp.nativeAttribute();
}

inline
bcemt_ThreadAttributes::SchedulingPolicy
bcemt_ThreadAttributes::schedulingPolicy() const
{
    return (bcemt_ThreadAttributes::SchedulingPolicy)d_imp.schedulingPolicy();
}

inline
int bcemt_ThreadAttributes::schedulingPriority() const
{
    return d_imp.schedulingPriority();
}

inline
int bcemt_ThreadAttributes::stackSize() const
{
    return d_imp.stackSize();
}

// FREE OPERATORS
inline
bool operator==(const bcemt_ThreadAttributes& lhs,
                const bcemt_ThreadAttributes& rhs)
{
    return lhs.d_imp == rhs.d_imp;
}

inline
bool operator!=(const bcemt_ThreadAttributes& lhs,
                const bcemt_ThreadAttributes& rhs)
{
    return lhs.d_imp != rhs.d_imp;
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
