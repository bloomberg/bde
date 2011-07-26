// bcemt_threadattributes.h                                           -*-C++-*-
#ifndef INCLUDED_BCEMT_THREADATTRIBUTES
#define INCLUDED_BCEMT_THREADATTRIBUTES

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a description of the attributes of a thread
//
//@CLASSES:
//  bcemt_ThreadAttributes: description of the attributes of a thread
//
//@SEE_ALSO: bcemt_threadutil
//
//@DESCRIPTION: This component provides a simply constrained (value-semantic)
// attribute class, 'bcemt_ThreadAttributes', for storing attributes of a
// thread in a platform-independent way.
//
// The default values for the various thread attributes supported by
// 'bcemt_ThreadAttributes' are listed in the following table:
//..
//  Name                 Type                   Default
//  -------------------  ---------------------  ---------------------------
//  detachedState        enum DetachedState     BCEMT_CREATE_JOINABLE
//  stackSize            int                    BCEMT_UNSET_STACK_SIZE
//  guardSize            int                    BCEMT_UNSET_GUARD_SIZE
//  inheritSchedule      bool                   'true'
//  schedulingPolicy     enum SchedulingPolicy  BCEMT_SCHED_OTHER
//  schedulingPriority   int                    platform & policy dependent
//
//  Name                 Constraint
//  -------------------  ---------------------------
//  stackSize            >= 0
//  guardSize            >= 0
//  schedulingPriority   platform & policy dependent
//..
///'detachedState' Attribute
///- - - - - - - - - - - - -
// The 'detachedState' property indicates whether an associated thread should
// be created in a "joinable" or "detached" state, by setting the
// 'detachedState' attribute to 'bcemt_ThreadAttributes::BCEMT_CREATE_JOINABLE'
// or 'bcemt_ThreadAttributes::BCEMT_CREATE_DETACHED', respectively.  Any
// thread can join with a "joinable" thread (i.e., the joining thread will
// block, waiting for the joinable thread's execution to complete).
//
// A joinable thread can be detached after it is created.
//
// When a joinable thread finishes, its resources are not recovered until it is
// joined or detached.  The resources of a detached thread are recovered
// automatically when it finishes.
//
///'stackSize' Attribute
///- - - - - - - - - - -
// The stack size for the created thread.  The stack size defaults to
// 'BCEMT_UNSET_STACK_SIZE', which is negative.  If the stack size is negative
// at thread creation, 'bcemt_Default::defaultThreadStackSize' will be called
// to determine the actual stack size used.
//
///'guardSize' Attribute
///- - - - - - - - - - -
// On some architectures, there is an extra area of the allocated stack for
// handling stack overflows.  The guard size defaults to
// 'BCEMT_UNSET_GUARD_SIZE', which is negative.  If the guard size is negative
// at thread creation, 'bcemt_Default::nativeThreadGuardSize' will be called to
// determine the guard size of the created thread.  This value will vary
// depending upon the platform.  Note that this attribute is currently ignored
// on Windows.
//
///'inheritSchedule' Attribute
///- - - - - - - - - - - - - -
// If 'true', the default, the attribute's 'schedulingPolicy' and
// 'schedulingPriority' attributes are ignored upon thread creation, their
// values being taken from the calling thread.  Note that this attribute is
// currently ignored on Windows, and on HPUX, for non-superusers, thread
// creation fails unless 'true == inheritSchedule()'.
//
///'schedulingPolicy' Attribute
/// - - - - - - - - - - - - - -
// Determines the scheduling policy to be used when multiple threads are
// waiting to run, which of the threads is to be run first.  A thread with the
// 'BCEMT_SCHED_FIFO' policy is to be deemed uninterruptable, while one with a
// 'BCEMT_SCHED_RR' policy is to get finite time slices before letting other
// threads run.  'BCEMT_SCHED_OTHER' (the default) is platform-dependent and
// unspecified.  This attribute is ignored unless 'false == inheritSchedule()'.
// Note that this attribute is currently ignored on Windows, while on Linux and
// AIX for non-superusers, thread creation will fail unless
// 'schedulingPolicy == BCEMT_SCHED_OTHER'.
//
///'schedulingPriority' Attribute
/// - - - - - - - - - - - - - - -
// Determines the scheduling priority of the thread, with higher numbers
// indicating more urgent threads.  The supplied value must be in the range
// '[ getMinSchedPrioirity(policy), getMaxSchedPrioirity(policy) ]' where
// 'policy' is the 'schedulingPolicy' attribute and 'get{Min,Max}SchedPriority'
// are static public methods of this class.  This attribute is ignored unless
// 'false == inheritSchedule()'.  Note that this attribute is currently ignored
// on Windows, and on other platforms, most of the time, adjusting thread
// priorities seems to have little effect.
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Creating and modifying thread attributes objects
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// First we suppose we have routines that will create and join threads:
//..
//  void myThreadCreate(int                           *threadHandle,
//                      const bcemt_ThreadAttributes&  attr,
//                      void (*)()                     function);
//      // Spawn a thread with attributes described by the specified 'attr',
//      // running the specified 'function', and assign a handle referring to
//      // the spawned thread to the specified '*threadHandle'.
//
//  void myThreadJoin(int threadHandle);
//      // Block until the thread referred to by the specified 'threadHandle'
//      // has finished, and recover its resources.  The behavior is undefined
//      // unless the thread is joinable.
//..
// Then we define a function that we want to run in the thread
//..
//  enum { BUFFER_SIZE = 128 * 1024 };
//
//  void myThreadFunction()
//  {
//      char buffer[BUFFER_SIZE];
//
//      ... do i/o to/from 'buffer', process data ..
//  }
//..
// Next, we create our thread attributes object and make sure its stack size is
// big enough to accomodate 'buffer' (it is important that the stack size be a
// 'fudge factor' larger than the buffer size, since things other than that
// buffer will be on the stack):
//..
//  bcemt_ThreadAttributes attr;
//  attr.setStackSize(BUFFER_SIZE + 8192);
//  attr.setDetachedState(bcemt_ThreadAttributes::BCEMT_CREATE_DETACHED);
//..
// Then, we create the thread:
//..
//  int handle;
//  myThreadCreate(&handle,
//                 attr,
//                 &myThreadFunction);
//..
// Next, suppose we also want another copy of the thread to run, but joinable.
//..
//  attr.setDetachedState(bcemt_ThreadAttributes::BCEMT_CREATE_JOINABLE);
//
//  int handle2;
//  myThreadCreate(&handle2,
//                 attr,
//                 &myThreadFunction);
//..
// The thread associated with 'handle' does not need to be joined while the
// thread associated with 'handle2' will.  Let's suppose we know that the first
// thread will finish before the second will.  Finally, we want to wait for the
// second thread to finish with a join.
//..
//  myThreadJoin(handle2);
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
    // This simply constrained (value-semantic) attribute class characterizes a
    // collection of thread attribute values.  See the Attributes section under
    // @DESCRIPTION in the component-level documentation.  There are no
    // invariants checked at destruction, but certain possible values or
    // combinations of values can result in thread creation to fail.
    //
    // This class:
    //: o supports a complete set of *value* *semantic* operations
    //:   o except for 'bdex' serialization
    //: o is *exception-neutral*
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see 'bsldoc_glossary'.

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
    DetachedState    d_detachedState;       // is threaad detached or joinable?

    int              d_guardSize;           // size of guard area on stack

    bool             d_inheritSchedule;     // does the thread inherit
                                            // scheduling policy & priority
                                            // from the thread that created it?
                                
    SchedulingPolicy d_schedulingPolicy;    // nature of queue for chosing
                                            // which of waiting threads will
                                            // execute first

    int              d_schedulingPriority;  // thread priority -- higher
                                            // numbers more urgent

    int              d_stackSize;           // size of thread stack

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

    void setInheritSchedule(bool inheritSchedule);
        // Set the value of the inherit schedule attribute of this thread
        // attributes object to the specified 'inheritSchedule'.  A value of
        // 'false' for the inherit schedule attribute indicates that a thread
        // should *not* inherit the scheduling policy and priority of the
        // thread that created it, and a value of 'true' indicates that the
        // thread *should* inherit these attributes.  This attribute is ignored
        // on Windows.  Note that on HPUX, for non superusers, thread creation
        // fails unless 'inheritSchedule() == true'.

    void setSchedulingPolicy(SchedulingPolicy schedulingPolicy);
        // Set the value of the scheduling policy attribute of this thread
        // attributes object to the specified 'schedulingPolicy'.  Ignored on
        // Windows.  Unless '!inheritSchedule()' this attribute is ignored.  If
        // necessary this method will adjust 'schedulingPriority' to ensure
        // that it is in the range
        // '[ getMinSchedPriority(policy), getMaxSchedPriority(policy) ]'.
        // Note that on Linux and AIX, for non-superusers, thread creation
        // fails unless 'schedulingPriority == BCEMT_SCHED_OTHER'.

    void setSchedulingPriority(int schedulingPriority);
        // Set the value of the scheduling priority attribute of this thread
        // attributes object to the specified 'schedulingPriority'.  This
        // attribute is ignored on Windows.  Unless '!inheritSchedule()' this
        // attribute is ignored.  On Solaris, AIX, and Linux, higher numbers
        // indicate more urgent priorities.  Priorities must be in the range
        // 'min <= priority <= max' where 'min == getMinSchedPriority(policy)'
        // and 'max == getMaxSchedPriority(policy)'.  Note that this attribute
        // has little or no effect except when multiple threads are blocked on
        // a mutex, in which case it influences which thread is to acquire the
        // mutex first.

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

    bool inheritSchedule() const;
        // Return 1 if threads created using this thread attributes object
        // inherit the scheduling policy and priority from the "parent" thread,
        // and 0 otherwise.  Note that under many circumstances, thread
        // creation fails unless 'inheritSchedule() == true'.

    SchedulingPolicy schedulingPolicy() const;
        // Return the value of the scheduling policy attribute of this thread
        // attributes object.  Note that unless '!inheritSchedule()', this
        // attribute is ignored.

    int schedulingPriority() const;
        // Return the value of the scheduling priority attribute of this thread
        // attributes object.  A value of -1 indicates that the scheduling
        // priority is set to the platform's default value.  Note that unless
        // '!inheritSchedule()', this attribute is ignored.

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
void bcemt_ThreadAttributes::setInheritSchedule(bool inheritSchedule)
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
bool bcemt_ThreadAttributes::inheritSchedule() const
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
