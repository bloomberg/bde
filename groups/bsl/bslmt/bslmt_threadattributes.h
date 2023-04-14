// bslmt_threadattributes.h                                           -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BSLMT_THREADATTRIBUTES
#define INCLUDED_BSLMT_THREADATTRIBUTES

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a description of the attributes of a thread.
//
//@CLASSES:
//  bslmt::ThreadAttributes: description of the attributes of a thread
//
//@SEE_ALSO: bslmt_threadutil, bslmt_configuration
//
//@DESCRIPTION: This component provides a simply constrained (value-semantic)
// attribute class, 'bslmt::ThreadAttributes', for describing attributes of a
// thread in a platform-independent way.
//
// The default values and constraints for the attributes provided by
// 'bslmt::ThreadAttributes' are listed in the following two tables:
//..
//  Name                Type                   Default
//  ------------------  ---------------------  ----------------------
//  detachedState       enum DetachedState     e_CREATE_JOINABLE
//  stackSize           int                    e_UNSET_STACK_SIZE
//  guardSize           int                    e_UNSET_GUARD_SIZE
//  inheritSchedule     bool                   'true'
//  schedulingPolicy    enum SchedulingPolicy  e_SCHED_DEFAULT
//  schedulingPriority  int                    e_UNSET_PRIORITY
//  threadName          bsl::string            ""
//
//  Name          Constraint
//  ---------     ---------------------------------------------------
//  stackSize     'e_UNSET_STACK_SIZE == stackSize || 0 <= stackSize'
//  guardSize     'e_UNSET_GUARD_SIZE == guardSize || 0 <= guardSize'
//..
//
///'detachedState' Attribute
///- - - - - - - - - - - - -
// The 'detachedState' attribute indicates whether an associated thread should
// be created in a joinable or detached state, through the enum values
// 'e_CREATE_JOINABLE' and 'e_CREATE_DETACHED', respectively.  A thread in the
// joinable state will have its exit status maintained after thread
// termination.  Any thread can join with a joinable thread (see
// 'bslmt_threadutil'), in which case the joining thread will block, waiting
// for the joined thread's execution to complete, after which the joined
// thread's termination status will be reported back to the joining thread, and
// its resources reclaimed.  A thread in a detached state will have its
// resources claimed at thread termination, and cannot be joined.  Note that a
// joinable thread can be made detached after it is created, but not vice
// versa.
//
///'stackSize' Attribute
///- - - - - - - - - - -
// The 'stackSize' attribute indicates the size, in bytes, of the stack that
// should be provided to a newly created thread.  If the stack size is
// 'e_UNSET_STACK_SIZE' then a created thread will be provided a default stack
// size (see 'bslmt_configuration').  The 'stackSize' attribute should be
// interpreted to mean that a created thread can safely define an automatic
// variable of the configured 'stackSize' bytes in its thread-entry function.
// Note that, on some platforms, an adjusted value derived from the 'stackSize'
// attribute may be supplied to the underlying representation by the thread
// creation function.
//
///'guardSize' Attribute
///- - - - - - - - - - -
// The 'guardSize' attribute indicates the size of the memory region to provide
// past the end of a created thread's stack to protect against stack overflows.
// If a thread's stack pointer overflows into a guard area, the task will
// receive an error (e.g., a signal).  If 'guardSize' is 'e_UNSET_GUARD_SIZE',
// then a created thread will be provided with a default native guard size (see
// 'bslmt_configuration').  Note that the interpretation of 'guardSize' may
// vary among platforms, and the value may be adjusted up (e.g., by rounding up
// to a multiple of page size) or ignored entirely (e.g., the Windows platform
// does not support this attribute).
//
///'inheritSchedule' Attribute
///- - - - - - - - - - - - - -
// The 'inheritSchedule' attribute, if 'true', indicates that a created
// thread's 'schedulingPolicy' and 'schedulingPriority' attributes should be
// taken from its parent thread and the configured values of those thread
// attributes should be ignored.  If 'inheritSchedule' is 'false', then the
// 'schedulingPolicy' and 'schedulingPriority' attribute values should be used
// to configure a thread.  See 'bslmt_threadutil' for information about support
// for this attribute.
//
///'schedulingPolicy' Attribute
/// - - - - - - - - - - - - - -
// The 'schedulingPolicy' attribute indicates the policy that should be used to
// schedule the created thread for execution.  Typically clients should use the
// default platform supplied scheduling policy, which is indicated by the
// 'e_SCHED_DEFAULT' value.  The alternative scheduling policies,
// 'e_THREAD_FIFO' and 'e_SCHED_RR', are "real-time" scheduling policies, and
// may not be available unless the task is run with the appropriate privileges.
// 'e_SCHED_FIFO' indicates a thread should run until it either yields or is
// interrupted by a thread of higher priority.  'e_SCHED_RR' is the same as
// 'e_SCHED_FIFO', except that the created thread may be interrupted by a ready
// thread of equal priority after a finite time-slice.  This attribute is
// ignored unless 'inheritSchedule' is 'false'.  See 'bslmt_threadutil' for
// information about support for this attribute.
//
///'schedulingPriority' Attribute
/// - - - - - - - - - - - - - - -
// The 'schedulingPriority' attribute is a platform specific value whose valid
// values range from the minimum to the maximum value for the associated
// 'schedulingPolicy', with higher numbers indicating a more urgent priority.
// Functions to obtain the minimum and maximum values are in this component and
// 'bslmt_threadutil'.  This attribute is ignored unless 'inheritSchedule' is
// 'false'.  See 'bslmt_threadutil' for information about support for this
// attribute.
//
///'threadName' Attribute
/// - - - - - - - - - - -
// The 'threadName' attribute indicates the name the thread is to have.  Thread
// names show up in debuggers on some platforms, and are unsupported on others.
// Thread names have unlimited lengths in a thread attributes object, but the
// thread names actually supported by specific platforms may have limited
// length, depending upon the platform, so thread names may be truncated when
// assigned to the actual thread.  At this time, only Linux, Solaris, Darwin,
// and Windows support thread names.  Unix platforms have a maximum thread name
// length of 15, while on Windows, the limit is 32767, or '(1 << 15) - 1'
// characters.
//
///Fluent Interface
///------------------
// 'bslmt::ThreadAttributes' provides manipulators that return a non-'const'
// reference to the object so that setting individual attributes can be
// "chained" into a single expression statement, or that attributes can be
// "built" in place as a function argument.  For example:
//..
//  bslmt::ThreadUtil::Handle handle;
//
//  int status = bslmt::ThreadUtil::create(
//                          &handle,
//                          bslmt::ThreadAttributes().setThreadName("myName")
//                                                   .setInheritSchedule(true),
//                          myThreadFunction,
//                          &myThreadArgument);
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating and Modifying Thread Attributes Objects
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we will demonstrate creating and configuring a
// 'bslmt::ThreadAttributes' object, then using it with a hypothetical
// thread-creation function.  Finally we show how a thread creation function
// might interpret those attributes for the underlying operating system.
//
// First we forward declare a routine that we will use to create a thread:
//..
//  void myThreadCreate(int                             *threadHandle,
//                      const bslmt::ThreadAttributes&   attributes,
//                      void                           (*function)());
//      // Spawn a thread having properties described by the specified
//      // 'attributes' and that runs the specified 'function', and assign a
//      // handle referring to the spawned thread to the specified
//      // '*threadHandle'.
//..
// Then, we declare two routines that will return the minimum and maximum
// thread priority given a scheduling policy.  Note that similar methods exist
// in 'bslmt_threadutil'.
//..
//  int myMinPriority(bslmt::ThreadAttributes::SchedulingPolicy policy);
//  int myMaxPriority(bslmt::ThreadAttributes::SchedulingPolicy policy);
//..
// Next we define a function that we will use as our thread entry point.  This
// function declares a single variable on the stack of predetermined size.
//..
//  enum { k_BUFFER_SIZE = 128 * 1024 };
//
//  void myThreadFunction()
//  {
//      int bufferLocal[k_BUFFER_SIZE];
//
//      // Perform some calculation that involves no subroutine calls or
//      // additional automatic variables.
//  }
//..
// Then, we define our main function, in which we demonstrate configuring a
// 'bslmt::ThreadAttributes' object describing the properties a thread we will
// create.
//..
//  void testMain()
//  {
//..
// Next, we create a thread attributes object, 'attributes', and set its
// 'stackSize' attribute to a value large enough to accommodate the
// 'BUFFER_SIZE' buffer used by 'myThreadFunction'.  Note that we use
// 'BUFFER_SIZE' as an illustration; in practice, it is difficult or impossible
// to gauge the exact amount of stack size required for a typical thread, and
// the value supplied should be a reasonable *upper* bound on the anticipated
// requirement.
//..
//      bslmt::ThreadAttributes attributes;
//      attributes.setStackSize(k_BUFFER_SIZE);
//..
// Then, we set the 'detachedState' property to 'e_CREATE_DETACHED', indicating
// that the thread will not be joinable, and its resources will be reclaimed
// upon termination.
//..
//      attributes.setDetachedState(
//                             bslmt::ThreadAttributes::e_CREATE_DETACHED);
//..
// Now, we create a thread, using the attributes configured above:
//..
//      int handle;
//      myThreadCreate(&handle, attributes, &myThreadFunction);
//  }
//..
// Finally, we define the thread creation function, and show how a thread
// attributes object might be interpreted by it:
//..
//  void myThreadCreate(int                             *threadHandle,
//                      const bslmt::ThreadAttributes&   attributes,
//                      void                           (*function)())
//      // Spawn a thread with properties described by the specified
//      // 'attributes', running the specified 'function', and assign a handle
//      // referring to the spawned thread to the specified '*threadHandle'.
//  {
//      int stackSize = attributes.stackSize();
//      if (bslmt::ThreadAttributes::e_UNSET_STACK_SIZE == stackSize) {
//          stackSize = bslmt::Configuration::defaultThreadStackSize();
//      }
//
//      // Add a "fudge factor" to 'stackSize' to ensure that the client can
//      // declare an object of 'stackSize' bytes on the stack safely.
//
//      stackSize += 8192;
//
//      int guardSize = attributes.guardSize();
//      if (bslmt::ThreadAttributes::e_UNSET_GUARD_SIZE == guardSize) {
//          guardSize = bslmt::Configuration::nativeDefaultThreadGuardSize();
//      }
//
//      int policy = attributes.schedulingPolicy();
//      int priority = attributes.schedulingPriority();
//
//      // the following is pseudo-code for actually creating the thread
//      /*
//      if (bslmt::ThreadAttributes::e_UNSET_PRIORITY == priority) {
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
//      */
//  }
//..
// Notice that a new value derived from the 'stackSize' attribute is used so
// that the meaning of the attribute is platform neutral.

#include <bslscm_version.h>

#include <bslma_allocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_assert.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bsl_c_limits.h>
#include <bsl_iosfwd.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace bslmt {

                          // ======================
                          // class ThreadAttributes
                          // ======================

class ThreadAttributes {
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
        // This enumeration provides two values used to distinguish among a
        // joinable thread and a non-joinable (detached) thread.

        e_CREATE_JOINABLE = 0,  // create a joinable thread
        e_CREATE_DETACHED = 1   // create a non-joinable thread

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , BCEMT_CREATE_JOINABLE = e_CREATE_JOINABLE
      , BCEMT_CREATE_DETACHED = e_CREATE_DETACHED
      , CREATE_JOINABLE = e_CREATE_JOINABLE
      , CREATE_DETACHED = e_CREATE_DETACHED
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };

    enum SchedulingPolicy {
        // This enumeration provides values used to distinguish between
        // different thread scheduling policies.

        e_SCHED_OTHER   = 0,  // unspecified, OS-dependent scheduling
                                  // policy

        e_SCHED_FIFO    = 1,  // first-in-first-out scheduling policy

        e_SCHED_RR      = 2,  // round-robin scheduling policy

        e_SCHED_DEFAULT = 3   // default OS scheduling policy, usually
                                  // equivalent to 'e_SCHED_OTHER'
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , BCEMT_SCHED_OTHER = e_SCHED_OTHER
      , BCEMT_SCHED_FIFO = e_SCHED_FIFO
      , BCEMT_SCHED_RR = e_SCHED_RR
      , BCEMT_SCHED_DEFAULT = e_SCHED_DEFAULT
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
    };

    enum {
        // The following constants indicate that the 'stackSize', 'guardSize',
        // and 'schedulingPriority' attributes, respectively, are unspecified
        // and the thread creation routine is use platform-specific defaults.
        // These attributes are initialized to these values when a thread
        // attributes object is default constructed.

        e_UNSET_STACK_SIZE = -1,
        e_UNSET_GUARD_SIZE = -1,
        e_UNSET_PRIORITY   = INT_MIN,

        e_SCHED_MIN        = e_SCHED_OTHER,
        e_SCHED_MAX        = e_SCHED_DEFAULT
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , BCEMT_UNSET_STACK_SIZE = e_UNSET_STACK_SIZE
      , BCEMT_UNSET_GUARD_SIZE = e_UNSET_GUARD_SIZE
      , BCEMT_UNSET_PRIORITY = e_UNSET_PRIORITY
      , BCEMT_SCHED_MIN = e_SCHED_MIN
      , BCEMT_SCHED_MAX = e_SCHED_MAX
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
    };

  private:
    // DATA
    DetachedState    d_detachedState;       // whether the thread is detached
                                            // or joinable

    int              d_guardSize;           // size of guard area provided
                                            // beyond the end of the configured
                                            // thread's stack

    bool             d_inheritScheduleFlag; // whether the thread inherits its
                                            // scheduling policy & priority
                                            // from its parent thread

    SchedulingPolicy d_schedulingPolicy;    // policy for scheduling thread
                                            // execution

    int              d_schedulingPriority;  // thread priority (higher numbers
                                            // indicate more urgency)

    int              d_stackSize;           // size of the thread's stack

    bsl::string      d_threadName;          // name of the thread

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(ThreadAttributes,
                                   bslma::UsesBslmaAllocator);

    // CREATORS
    ThreadAttributes();
    explicit ThreadAttributes(bslma::Allocator *basicAllocator);
        // Create a 'ThreadAttributes' object having the (default) attribute
        // values:
        //: o 'detachedState()      == e_CREATE_JOINABLE'
        //: o 'guardSize()          == e_UNSET_GUARD_SIZE'
        //: o 'inheritSchedule()    == true'
        //: o 'schedulingPolicy()   == e_SCHED_DEFAULT'
        //: o 'schedulingPriority() == e_UNSET_PRIORITY'
        //: o 'stackSize()          == e_UNSET_STACK_SIZE'
        //: o 'threadName()         == ""'
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    ThreadAttributes(const ThreadAttributes&  original,
                     bslma::Allocator        *basicAllocator = 0);
        // Create a 'ThreadAttributes' object having the same value as the
        // specified 'original' object.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    // MANIPULATORS
    ThreadAttributes& operator=(const ThreadAttributes& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    // MANIPULATORS
    ThreadAttributes& setDetachedState(DetachedState value);
        // Set the 'detachedState' attribute of this object to the specified
        // 'value'.  Return a non-'const' reference to this object (see also
        // {Fluent Interface}).  A value of 'e_CREATE_JOINABLE' (the default)
        // indicates that a thread must be joined to clean up its resources
        // after it terminates; a value of 'e_CREATE_DETACHED' (the only other
        // legal value) indicates that the resources will be cleaned up
        // automatically upon thread termination, and that the thread must not
        // be joined.

    ThreadAttributes& setGuardSize(int value);
        // Set the 'guardSize' attribute of this object to the specified
        // 'value' (in bytes).  Return a non-'const' reference to this object
        // (see also {Fluent Interface}).  'e_UNSET_GUARD_SIZE == guardSize' is
        // intended to indicate that the default value as defined by the
        // platform is to be used.  This default value is typically the size of
        // one or two pages (see 'bslmt_configuration').  The behavior is
        // undefined unless 'e_UNSET_GUARD_SIZE == guardSize' or
        // 'guardSize >= 0'.

    ThreadAttributes& setInheritSchedule(bool value);
        // Set the 'inheritSchedule' attribute of this object to the specified
        // 'value'.  Return a non-'const' reference to this object (see also
        // {Fluent Interface}).  A value of 'false' for the inherit schedule
        // attribute indicates that a thread should *not* inherit the
        // scheduling policy and priority of the thread that created it and
        // instead should use the respective values supplied by this object;
        // whereas a value of 'true' indicates that the thread *should* inherit
        // these attributes and ignore the respective values in this object.
        // See 'bslmt_threadutil' for information about support for this
        // attribute.

    ThreadAttributes& setSchedulingPolicy(SchedulingPolicy value);
        // Set the value of the 'schedulingPolicy' attribute of this object to
        // the specified 'value'.  Return a non-'const' reference to this
        // object (see also {Fluent Interface}).  This attribute is ignored
        // unless 'inheritSchedule' is 'false'.  See 'bslmt_threadutil' for
        // information about this attribute.

    ThreadAttributes& setSchedulingPriority(int value);
        // Set the 'schedulingPriority' attribute of this object to the
        // specified 'value'.  Return a non-'const' reference to this object
        // (see also {Fluent Interface}).  This attribute is ignored unless
        // 'inheritSchedule()' is 'false'.  Higher values of 'value' signify
        // more urgent priorities.  Note that the valid range of priorities
        // depends upon the platform and 'schedulingPolicy' attribute, and the
        // minimum and maximum priority values are determined by methods in
        // 'bslmt_threadutil'.  See 'bslmt_threadutil' for information about
        // this attribute.

    ThreadAttributes& setStackSize(int value);
        // Set the 'stackSize' attribute of this object to the specified
        // 'value'.  Return a non-'const' reference to this object (see also
        // {Fluent Interface}).  If 'stackSize' is 'e_UNSET_STACK_SIZE', thread
        // creation should use the default stack size value provided by
        // 'bslmt_configuration'.  The behavior is undefined unless
        // 'e_UNSET_STACK_SIZE == stackSize' or '0 <= stackSize'.

    ThreadAttributes& setThreadName(const bslstl::StringRef& value);
        // Set the 'threadName' attribute of this object to the specified
        // 'value'.  Return a non-'const' reference to this object (see also
        // {Fluent Interface}).

    // ACCESSORS
    DetachedState detachedState() const;
        // Return the value of the 'detachedState' attribute of this object.  A
        // value of 'e_CREATE_JOINABLE' indicates that a thread must be joined
        // after it terminates to clean up its resources; a value of
        // 'e_CREATE_DETACHED' (the only other legal value) indicates that the
        // resources will be cleaned up automatically upon thread termination,
        // and that the thread must not be joined.

    int guardSize() const;
        // Return the value of the 'guardSize' attribute of this object.  The
        // value 'e_UNSET_GUARD_SIZE == guardSize' is intended to indicate that
        // the default value as defined by the platform (which is typically the
        // size of one or two pages) should be obtained from
        // 'bslmt_configuration' and used.

    bool inheritSchedule() const;
        // Return the value of the 'inheritSchedule' attribute of this object.
        // A value of 'false' for the inherit schedule attribute indicates that
        // a thread should *not* inherit the scheduling policy and priority of
        // the thread that created it and instead should use the respective
        // values supplied by this object; whereas a value of 'true' indicates
        // that the thread *should* inherit these attributes and ignore the
        // respective values in this object.  See 'bslmt_threadutil' for
        // information about support for this attribute.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the (absolute
        // value of) the optionally specified indentation 'level' and return a
        // reference to 'stream'.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.

    SchedulingPolicy schedulingPolicy() const;
        // Return the value of the 'schedulingPolicy' attribute of this object.
        // This attribute is ignored unless 'inheritSchedule' is 'false'.  See
        // 'bslmt_threadutil' for information about this attribute.

    int schedulingPriority() const;
        // Return the value of the 'schedulingPriority' attribute of this
        // object.  This attribute is ignored unless 'inheritSchedule()' is
        // 'false'.  Higher values of 'value' signify more urgent priorities.
        // Note that the valid range of priorities depends upon the platform
        // and 'schedulingPolicy' attribute, and the minimum and maximum
        // priority values are determined by methods in 'bslmt_threadutil'.
        // See 'bslmt_threadutil' for information about this attribute.

    int stackSize() const;
        // Return the value of the 'stackSize' attribute of this object.  If
        // 'stackSize' is 'e_UNSET_STACK_SIZE', thread creation should use the
        // default stack size value provided by 'bslmt_configuration'.

    bslstl::StringRef threadName() const;
        // Return the 'threadName' attribute of this object.  Note that the
        // returned string reference will be invalidated if 'setThreadName' is
        // subsequently called on this object.

                                  // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.
};

// FREE OPERATORS
bool operator==(const ThreadAttributes& lhs, const ThreadAttributes& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'ThreadAttributes' objects have the
    // same value if the corresponding values of their 'detachedState',
    // 'guardSize', 'inheritSchedule', 'schedulingPolicy',
    // 'schedulingPriority', and 'stackSize' attributes are the same.

bool operator!=(const ThreadAttributes& lhs, const ThreadAttributes& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'baltzo::LocalTimeDescriptor'
    // objects do not have the same value if the corresponding values of their
    // 'detachedState', 'guardSize', 'inheritSchedule', 'schedulingPolicy',
    // 'schedulingPriority', and 'stackSize' attributes are not the same.

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&           stream,
                         const ThreadAttributes& object);
    // Write the value of the specified 'object' object to the specified output
    // 'stream' in a single-line format, and return a reference to 'stream'.
    // If 'stream' is not valid on entry, this operation has no effect.  Note
    // that this human-readable format is not fully specified, can change
    // without notice, and is logically equivalent to:
    //..
    //  print(stream, 0, -1);
    //..

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                          // ----------------------
                          // class ThreadAttributes
                          // ----------------------

// MANIPULATORS
inline
ThreadAttributes& ThreadAttributes::setDetachedState(
                                         ThreadAttributes::DetachedState value)
{
    BSLS_ASSERT_SAFE(e_CREATE_DETACHED == value ||
                     e_CREATE_JOINABLE == value);

    d_detachedState = value;

    return *this;
}

inline
ThreadAttributes& ThreadAttributes::setGuardSize(int value)
{
    BSLMF_ASSERT(-1 == e_UNSET_GUARD_SIZE);

    BSLS_ASSERT_SAFE(-1 <= value);

    d_guardSize = value;

    return *this;
}

inline
ThreadAttributes& ThreadAttributes::setInheritSchedule(bool value)
{
    d_inheritScheduleFlag = value;

    return *this;
}

inline
ThreadAttributes& ThreadAttributes::setSchedulingPolicy(
                                      ThreadAttributes::SchedulingPolicy value)
{
    BSLS_ASSERT_SAFE(e_SCHED_MIN <= (int) value);
    BSLS_ASSERT_SAFE(                   (int) value <= e_SCHED_MAX);

    d_schedulingPolicy = value;

    return *this;
}

inline
ThreadAttributes& ThreadAttributes::setSchedulingPriority(int value)
{
    d_schedulingPriority = value;

    return *this;
}

inline
ThreadAttributes& ThreadAttributes::setStackSize(int value)
{
    BSLMF_ASSERT(-1 == e_UNSET_STACK_SIZE);

    BSLS_ASSERT_SAFE(-1 <= value);

    d_stackSize = value;

    return *this;
}

inline
ThreadAttributes& ThreadAttributes::setThreadName(
                                                const bslstl::StringRef& value)
{
    d_threadName.assign(value);

    return *this;
}

// ACCESSORS
inline
ThreadAttributes::DetachedState ThreadAttributes::detachedState() const
{
    return d_detachedState;
}

inline
int ThreadAttributes::guardSize() const
{
    return d_guardSize;
}

inline
bool ThreadAttributes::inheritSchedule() const
{
    return d_inheritScheduleFlag;
}

inline
ThreadAttributes::SchedulingPolicy ThreadAttributes::schedulingPolicy() const
{
    return d_schedulingPolicy;
}

inline
int ThreadAttributes::schedulingPriority() const
{
    return d_schedulingPriority;
}

inline
int ThreadAttributes::stackSize() const
{
    return d_stackSize;
}

inline
bslstl::StringRef ThreadAttributes::threadName() const
{
    return d_threadName;
}

                                  // Aspects

inline
bslma::Allocator *ThreadAttributes::allocator() const
{
    return d_threadName.get_allocator().mechanism();
}

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream&           stream,
                         const ThreadAttributes& object)
{
    return object.print(stream, 0, -1);
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
