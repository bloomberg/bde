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
//  bcemt_ThreadAttributes: a description of the attributes of a thread
//
//@SEE_ALSO: bcemt_threadutil, bcemt_default
//
//@DESCRIPTION: This component provides a simply constrained (value-semantic)
// attribute class, 'bcemt_ThreadAttributes', for describing attributes of a
// thread in a platform-independent way.
//
// The default values and constraints for the various thread attributes
// supported by 'bcemt_ThreadAttributes' are listed in the following two
// tables:
//..
//  Name                Type                   Default
//  ------------------  ---------------------  -------------------------------
//  detachedState       enum DetachedState     BCEMT_CREATE_JOINABLE
//  stackSize           int                    BCEMT_UNSET_STACK_SIZE
//  guardSize           int                    BCEMT_UNSET_GUARD_SIZE
//  inheritSchedule     bool                   'true'
//  schedulingPolicy    enum SchedulingPolicy  BCEMT_SCHED_DEFAULT
//  schedulingPriority  int                    BCEMT_UNSET_PRIORITY
//
//  Name                          Constraint
//  ------------------            -------------------------------------
//  stackSize                     unset or >= 0
//  guardSize                     unset or >= 0
//..
///'detachedState' Attribute
///- - - - - - - - - - - - -
// The 'detachedState' attribute indicates whether an associated thread should
// be created in a joinable or detached state, through the enum values
// 'BCEMT_CREATE_JOINABLE' or 'BCEMT_CREATE_DETACHED', respectively.  A thread
// in the joinable state will have its exit status maintained after thread
// termination.  Any thread can join with a joinable thread (see
// 'bcemt_threadutil'), in which case the joining thread will block, waiting
// for the joined thread's execution to complete, then the joined thread's
// termination status will be reported back to the joining thread, and its
// resources reclaimed.  A thread in a detached state will have its resources
// claimed at thread termination, and cannot be joined.  Note that a joinable
// thread can be detached after it is created.
//
///'stackSize' Attribute
///- - - - - - - - - - -
// The 'stackSize' attribute indicates the size, in bytes, of the stack that
// should be provided to a newly created thread.  If the stack size is
// 'BCEMT_UNSET_STACK_SIZE' then a created thread will be provided a default
// stack size (see 'bcemt_default').  The 'stackSize' attribute should be
// interpreted to mean that a created thread can safely define an automatic
// variable of the configured 'stackSize' bytes in its thread-entry function.
// Note that this means the provided value may be adjusted on some platforms
// before being passed to the underlying representation -- for example, on
// Itanium (HPUX), the value must be scaled up to account for the fact that the
// architecture will split the stack in two.
//
///'guardSize' Attribute
///- - - - - - - - - - -
// The 'guardSize' attribute indicates the size of the memory region to provide
// past the end of a created thread's stack to protect against stack overflows.
// If a thread's stack pointer overflows into a guard area, the task will
// receive an error (e.g. a signal).  If 'guardSize' is
// 'BCEMT_UNSET_GUARD_SIZE', then a created thread will be provided with a
// default native guard size (see 'bcemt_default').  Note that the
// interpretation of 'guardSize' may vary between platforms, the value may be
// adjusted up (e.g. by rounding up to a multiple of page size), or ignored
// entirely (e.g. the Windows platform does not support this attribute).
//
///'inheritSchedule' Attribute
///- - - - - - - - - - - - - -
// The 'inheritSchedule' attribute, if 'true', indicates that a created
// thread's 'schedulingPolicy' and 'schedulingPriority' attributes should be
// taken from the calling thread and the configured values of those thread
// attributes should be ignored.  If 'inheritSchedule' is 'false', then the
// 'schedulingPolicy' and 'schedulingPriority' attribute values should be used
// to configure a thread.  Note that this attribute is currently ignored on
// Windows (as are 'schedulingPolicy' and 'schedulingPriority'), and on HPUX,
// for non-superusers, thread creation fails unless the 'inheritSchedule()' is
// 'true'.
//
///'schedulingPolicy' Attribute
/// - - - - - - - - - - - - - -
// The 'schedulingPolicy' attribute indicates the policy that should be used to
// schedule the created thread for execution.  Typically clients should use the
// default platform supplied scheduling policy, which is indicated by the
// 'BCEMT_SCHED_DEFAULT' value.  The alternative scheduling policies,
// 'BCEMT_THREAD_FIFO' and 'BCEMT_SCHED_RR', are "real-time" schduling policies
// and may not be available unless the task is run with the appropriate
// privileges.  'BCEMT_SCHED_FIFO' indicates a thread should run until it
// yields or is interrupted by a thread of higher priority.  'BCEMT_SCHED_RR'
// is the same, except that the created thread will be interrupted by a ready
// thread of equal priority after a finite time-slice.  Note that this
// attribute is currently ignored on Windows.  Also note that on AIX and Linux,
// for a non-superuser, thread creation fails if the policy is
// 'BCEMT_SCHED_FIFO' or 'BCEMT_SCHED_RR'.
//
///'schedulingPriority' Attribute
/// - - - - - - - - - - - - - - -
// The 'schedulingPriority' attribute is a platform specific value which can
// range from the minimum to the maximum values for the configured
// 'schedulingPolicy' with higher numbers indicating a more urgent priority.
// Functions to obtain these minimum and maximum values are in
// 'bcemt_threadutil'.  These attributes are both ignored unless
// 'inheritSchedule' is 'false'.  Note that these attributes are both currently
// ignored on Windows.
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Creating and modifying thread attributes objects
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we will demonstrate creating and configuring a
// 'bcemt_ThreadAttributes' object, then using it with a hypothetical thread
// creation function, and finally show how a thread creation function might
// interpret those attributes for the underlying operating system.
//
// First we forward declare a routine that we will use to create a thread:
//..
//  void myThreadCreate(int                           *threadHandle,
//                      const bcemt_ThreadAttributes&  attributes,
//                      void (*)()                     function);
//      // Spawn a thread with properties described by the specified
//      // 'attributes', running the specified 'function', and assign a handle
//      // referring to the spawned thread to the specified '*threadHandle'.
//..
// and routines that will return the minimum and maximum thread priority given
// a scheduling policy.  Note that similar methods exist in 'bcemt_threadutil'.
//..
//  int myMinPriority(bcemt_ThreadAttributes::SchedulingPolicy policy);
//  int myMaxPriority(bcemt_ThreadAttributes::SchedulingPolicy policy);
//..
// Then we define a function that we will use as our thread entry point.  This
// function declares a single variable on the stack of predetermined size.
//..
//  enum { BUFFER_SIZE = 128 * 1024 };
//
//  void myThreadFunction()
//  {
//      int bufferLocal[BUFFER_SIZE];
//
//      // Perform some calculation that involves no subroutine calls or
//      // additional automatic variables.
//  }
//..
// Next, we demonstrate configuring a 'bcemt_ThreadAttributes' object
// describing the properties a thread we will create.  We create a thread
// attributes object, 'attributes', then set its 'stackSize' attribute to a
// value large enough to accomodate the 'BUFFER_SIZE' buffer used by
// 'myThreadFunction'.  Note that we use 'BUFFER_SIZE' as illustration, in
// practice it is difficult or impossible to gauge the exact amount of stack
// size required for a typical thread, and the value supplied should be a
// reasonable *upper* bound on the anticipated requirement.
//..
//  bcemt_ThreadAttributes attributes;
//  attributes.setStackSize(BUFFER_SIZE);
//..
// Then, we set the 'detachedState' property to 'BCEMT_CREATE_DETACHED',
// indicating that the thread will not be joinable, and its resources will be
// reclaimed upon termination.
//..
//  attributes.setDetachedState(bcemt_ThreadAttributes::BCEMT_CREATE_DETACHED);
//..
// Now, we create a thread, using the attributes configured above:
//..
//  int handle;
//  myThreadCreate(&handle, attributes, &myThreadFunction);
//..
// Finally, we define the thread creation function, and show how a thread
// attributes object might be interpreted:
//..
//  void myThreadCreate(int                           *threadHandle,
//                      const bcemt_ThreadAttributes&  attributes,
//                      void (*)()                     function)
//      // Spawn a thread with properties described by the specified
//      // 'attributes', running the specified 'function', and assign a handle
//      // referring to the spawned thread to the specified '*threadHandle'.
//  {
//      int stackSize = attributes.stackSize();
//      if (BCEMT_UNSET_STACK_SIZE == stackSize) {
//          stackSize = bcemt_Default::defaultThreadStackSize();
//      }
//      // Add a "fudge factor" to 'stackSize' to ensure that the client
//      // can declare an object of 'stackSize' bytes on the stack safely.
//
//      stackSize += 8192;
//
//  #ifdef BSLS_PLATFORM__OS_HPUX
//      // The Itanium divides the stack into two sections: a variable stack
//      // and a control stack.  To make 'stackSize' have the same meaning
//      // across platforms, we must double it on this platform.
//
//      stackSize *= 2;
//  #endif
//
//      int guardSize = attributes.guardSize();
//      if (BCEMT_UNSET_GUARD_SIZE == guardSize) {
//          guardSize = bcemt_Default::nativeDefaultThreadGuardSize();
//      }
//
//      int policy = attributes.schedulingPolicy();
//      int priority = attributes.schedulingPriority();
//      if (BCEMT_UNSET_PRIORITY == priority) {
//          priority = operatingSystemDefaultPriority(policy);
//      }
//
//      operatingSystemThreadCreate(threadHandle,
//                                  stackSize,
//                                  guardSize,
//                                  attributes.inheritSchedule(),
//                                  policy,
//                                  priority,
//                                  attributes.detachedState()
//                                  function);
//  }
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_C_LIMITS
#include <bsl_c_limits.h>
#endif

namespace BloombergLP {

                        // ============================
                        // class bcemt_ThreadAttributes
                        // ============================

class bcemt_ThreadAttributes {
    // This simply constrained (value-semantic) attribute class characterizes a
    // collection of thread attribute values.  See the Attributes section under
    // @DESCRIPTION in the component-level documentation.
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
        // different thread scheduling policies.

        BCEMT_SCHED_OTHER   = 0,  // default OS scheduling policy
        BCEMT_SCHED_FIFO    = 1,  // first-in-first-out scheduling policy
        BCEMT_SCHED_RR      = 2,  // round-robin scheduling policy
        BCEMT_SCHED_DEFAULT = 3   // whatever policy is the default for the
                                  // native thread implementation
    };

    enum {
        // The 'stackSize' and 'guardSize' attributes are initialized to these
        // values when a thread attributes object is default constructed.

        BCEMT_UNSET_STACK_SIZE          = -1,
        BCEMT_UNSET_GUARD_SIZE          = -1,
        BCEMT_UNSET_PRIORITY            = INT_MIN
    };

  private:
    // DATA
    DetachedState    d_detachedState;       // whether the threaad is detached
                                            // or joinable

    int              d_guardSize;           // size of guard area on stack

    bool             d_inheritSchedule;     // whether the thread inherits its
                                            // scheduling policy & priority
                                            // from its parent thread

    SchedulingPolicy d_schedulingPolicy;    // policy for scheduling thread
                                            // execution

    int              d_schedulingPriority;  // thread priority (higher numbers
                                            // more urgent)

    int              d_stackSize;           // size of thread's stack

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
        //
        // DEPRECATED: use 'bcemt_ThreadUtil' instead.

    static int getMinSchedPriority(int policy = -1);
        // Return the non-negative minimum available priority for the
        // optionally-specified 'policy' on success.  If no policy is
        // specified, the minimum priority for the process's policy is
        // returned.  Return 'INT_MIN' on error.  Note that for some
        // platform / policy cominations,
        // 'getMinSchedPriority(policy) == getMaxSchedPriority(policy)'.
        //
        // DEPRECATED: use 'bcemt_ThreadUtil' instead.

    // CREATORS
    bcemt_ThreadAttributes();
        // Create a 'bcemt_ThreadAttributes' object having the (default)
        // attribute values:
        //: o 'detachedState()      == BCEMT_CREATE_JOINABLE'
        //: o 'guardSize()          == BCEMT_UNSET_GUARD_SIZE'
        //: o 'inheritSchedule()    == true'
        //: o 'schedulingPolicy()   == BCEMT_SCHED_DEFAULT'
        //: o 'schedulingPriority() == BCEMT_UNSET_PRIORITY'
        //: o 'stackSize()          == BCEMT_UNSET_STACK_SIZE'

    bcemt_ThreadAttributes(const bcemt_ThreadAttributes& original);
        // Create a 'bcemt_ThreadAttributes' object having the same value
        // as the specified 'original' object.

    // ~bcemt_ThreadAttributes(); -- compiler generated

    // MANIPULATORS
    bcemt_ThreadAttributes& operator=(const bcemt_ThreadAttributes& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    // MANIPULATORS
    void setDetachedState(DetachedState value);
        // Set the 'detachedState' attribute of this object to the specified
        // 'value'.

    void setGuardSize(int value);
        // Set the 'guardSize' attribute of this object to the specified
        // 'value' (in bytes).  If 'guardSize < 0', the default value as
        // defined by the platform is used (which is typically the size of a
        // page).  If guard size has not been set, a created thread will have
        // the native guard size (see 'bcemt_default').

    void setInheritSchedule(bool value);
        // Set the 'inheritSchedule' attribute of this object to the specified
        // 'value'.  A value of 'false' for the inherit schedule attribute
        // indicates that a thread should *not* inherit the scheduling policy
        // and priority of the thread that created it, and a value of 'true'
        // indicates that the thread *should* inherit these attributes.  Note
        // that this attribute is ignored on Windows, and that on HPUX, for non
        // superusers, thread creation fails unless 'inheritSchedule()' is
        // 'true'.

    void setSchedulingPolicy(SchedulingPolicy value);
        // Set the value of the 'schedulingPolicy' attribute of this object to
        // the specified 'value'.  This attribute is ignored unless
        // 'inheritSchedule() = false'.  Note that this attribute is currently
        // ignored on Windows, and that on Linux and AIX, for non-superusers,
        // thread creation fails if 'schedulingPolicy' is 'BCEMT_SCHED_FIFO' or
        // 'BCEMT_SCHED_RR'.

    void setSchedulingPriority(int value);
        // Set the 'schedulingPriority' attribute of this object to the
        // specified 'value'.  This attribute is ignored unless
        // 'inheritSchedule()' is 'false'.  Priorities must be in the range
        // 'min <= priority <= max' where 'min == getMinSchedPriority(policy)'
        // and 'max == getMaxSchedPriority(policy)'.  Higher values of 'value'
        // signify more urgent priorities.  Note that this attribute is
        // currently ignored on Windows.

    void setStackSize(int value);
        // Set the 'stackSize' attribute of this object to the specified
        // 'value.  If 'stackSize' is 'BCEMT_UNSET_STACK_SIZE', thread creation
        // will use the default value provided by 'bcemt_default'.  See the
        // component doc for a detailed description of the meaning of this
        // attribute.

    // ACCESSORS
    DetachedState detachedState() const;
        // Return the value of the 'detachedState' attribute of this object.

    int guardSize() const;
        // Return the value of the 'guardSize' attribute of this object.

    bool inheritSchedule() const;
        // Return the value of the 'inheritSchedule' attribute of this object.

    SchedulingPolicy schedulingPolicy() const;
        // Return the value of the 'schedulingPolicy' attribute of this object.

    int schedulingPriority() const;
        // Return the value of the 'schedulingPriority' attribute of this
        // object.

    int stackSize() const;
        // Return the value of the 'stackSize' attribute of this object.
};

// FREE OPERATORS
bool operator==(const bcemt_ThreadAttributes& lhs,
                const bcemt_ThreadAttributes& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'bcemt_ThreadAttributes' objects have
    // the same value if the corresponding values of their 'detachedState',
    // 'guardSize', 'inheritSchedule', 'schedulingPolicy',
    // 'schedulingPriority', and 'stackSize' attributes are the same.

bool operator!=(const bcemt_ThreadAttributes& lhs,
                const bcemt_ThreadAttributes& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'baetzo_LocalTimeDescriptor'
    // objects do not have the same value if the corresponding values of their
    // 'detachedState', 'guardSize', 'inheritSchedule', 'schedulingPolicy',
    // 'schedulingPriority', and 'stackSize' attributes are not the same.

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
: d_detachedState(original.d_detachedState)
, d_guardSize(original.d_guardSize)
, d_inheritSchedule(original.d_inheritSchedule)
, d_schedulingPolicy(original.d_schedulingPolicy)
, d_schedulingPriority(original.d_schedulingPriority)
, d_stackSize(original.d_stackSize)
{
}

// MANIPULATORS
inline
bcemt_ThreadAttributes& bcemt_ThreadAttributes::operator=(
                                             const bcemt_ThreadAttributes& rhs)
{
    d_detachedState                = rhs.d_detachedState;
    d_guardSize                    = rhs.d_guardSize;
    d_inheritSchedule              = rhs.d_inheritSchedule;
    d_schedulingPolicy             = rhs.d_schedulingPolicy;
    d_schedulingPriority           = rhs.d_schedulingPriority;
    d_stackSize                    = rhs.d_stackSize;

    return *this;
}

inline
void bcemt_ThreadAttributes::setDetachedState(
                                   bcemt_ThreadAttributes::DetachedState value)
{
    d_detachedState = value;
}

inline
void bcemt_ThreadAttributes::setGuardSize(int value)
{
    d_guardSize = value;
}

inline
void bcemt_ThreadAttributes::setInheritSchedule(bool value)
{
    d_inheritSchedule = value;
}

inline
void bcemt_ThreadAttributes::setSchedulingPolicy(
                                bcemt_ThreadAttributes::SchedulingPolicy value)
{
    d_schedulingPolicy = value;
}

inline
void bcemt_ThreadAttributes::setSchedulingPriority(int value)
{
    d_schedulingPriority = value;
}

inline
void bcemt_ThreadAttributes::setStackSize(int value)
{
    d_stackSize = value;
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
