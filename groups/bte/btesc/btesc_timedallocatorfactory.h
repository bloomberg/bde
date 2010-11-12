// btesc_timedallocatorfactory.h       -*-C++-*-
#ifndef INCLUDED_BTESC_TIMEDALLOCATORFACTORY
#define INCLUDED_BTESC_TIMEDALLOCATORFACTORY

//@PURPOSE: Provide protocol for stream-channel-allocator factory with timeout.
//
//@CLASSES:
// btesc_TimedAllocatorFactory: blocking allocator factory with timeout
//
//@SEE_ALSO: btesc_timedcballocatorfactory btemc_timedallocatorfactory
//
//@AUTHOR: Tom Marshall (tmarshal)
//
//@DESCRIPTION: This component defines an abstract interface for a blocking
// mechanism with timeout capability that allocates an appropriate
// 'btesc_(Timed)ChannelAllocator' corresponding to a specified name (string).
// 'btesc_TimedAllocatorFactory' is typically used as an abstract naming
// service that supplies abstract connectors to servers supporting the named
// service.  This additional "stage" in the "lateral architecture" of a
// communications infrastructure decouples the service-name mapping convention
// from the individual connectors that are programmed to connect to a
// homogeneous service.  Note that "factory" as used here is synonymous with
// the term "allocator" used ubiquitously to imply a mechanism that provides
// temporary exclusive ownership of a resource, without making any statement
// about the lifecycle of that resource (here, a
// 'btesc_(Timed)ChannelAllocator').
//
///Protocol Hierarchy
///------------------
// The interface hierarchy (defined by direct inheritance) of the
// 'btesc_TimedAllocatorFactory' protocol is as follows:
//..
//                      ,---------------------------.
//                     ( btesc_TimedAllocatorFactory )
//                      `---------------------------'
//                                     |
//                                     V
//                         ,----------------------.
//                        ( btesc_AllocatorFactory )
//                         `----------------------'
//..
// This protocol adds the ability to allocate timed channel allocators as well
// as a "timeout" capability for the factory allocation methods themselves.
// Note that 'deallocate' cannot block, fail, or throw, and thus does not need
// a timeout facility.
//
///Channel-Allocator Allocation
///----------------------------
// This interface establishes methods for allocating blocking
// stream-based-channel allocators.  Each allocation method blocks until (1)
// "success" -- the requested channel allocator is allocated and its address
// returned to the user (in which case the 'status' argument is not modified),
// (2) "interrupted" -- the operation was interrupted (e.g., via a timeout):
// the channel-allocator address is null and 'status' is set to a non-negative
// value (see below), (3) "name unknown" -- the 'name' argument was not
// recognized: the channel-allocator address is null and 'status' is -2, or (4)
// "error" -- an implementation-dependent error occurred: the channel-allocator
// address is null and 'status' is less than -2.  The user may retry
// interrupted operations with a reasonable expectation of success.  An "error"
// status implies that the allocation is unlikely to succeed if retried, but
// does not *necessarily* invalidate the factory.  The 'isInvalid' method may
// be used to confirm the occurrence of a permanent error.  An allocation can
// *never* succeed once a factory is invalid.
//
// The meanings of the 'status' value for an unsuccessful allocation (i.e., a
// null channel-allocator address) are summarized as follows:
//..
//  "status"    meaning (only when returned channel-allocator address is null)
//  --------    -------------------------------------------------------------
//  positive    Interruption by an "asynchronous event"
//
//  zero        Interruption by a user-requested timeout
//
//  -2          Specified 'name' not recognized
//
//  < -2        Allocation operation unable to succeed at this time
//..
// Note that unless asynchronous events are explicitly enabled (see below),
// they are ignored, and "status" will never be positive.  Note also that, for
// consistency with non-blocking transport, a status of -1 is reserved for a
// "cancelled" operation, which is not possible in this blocking protocol.
//
///Asynchronous Events
///-------------------
// Allocation methods in this protocol anticipate the possible occurrence of an
// "asynchronous event" (AE) during execution.  A common example of an AE is a
// Unix signal, but note that a specific Unix signal *may* not result in an AE,
// and an AE is certainly not limited to signals, even on Unix platforms.
//
// This interface cannot fully specify either the nature of or the behavior
// resulting from an AE, but certain restrictions can be (and are) imposed.  By
// default, AEs are either ignored or, if that is not possible, cause an error.
// At the user's option, however, a concrete implementation can be authorized
// to return, if possible, with an "interrupted" status (leaving the factory
// unaffected) upon the occurrence of an AE.  Such authorizations are made
// explicitly by incorporating into the optional (trailing) integer 'flags'
// argument to a method call the 'btesc_Flag::ASYNC_INTERRUPT' value.
//
///Timeouts
///--------
// A timeout is registered by the caller, when a method is invoked, as a
// 'bdet_TimeInterval' value indicating the absolute *system* time after which
// the operation should be interrupted.  Information regarding the nature of an
// interruption is provided in the 'status' value (see above).
//
// Concrete allocator factories will make a "best effort" to honor the timeout
// request promptly, but no guarantee can be made as to the maximum duration of
// any particular allocation attempt.  Any implementation of this interface
// will support a timeout granularity of ten milliseconds (0.01s) or less.  The
// timeout is guaranteed *not* to occur before the specified time has passed.
// If a timeout is specified with a time that has already passed, the
// allocation will be attempted, but will not block.
//
///USAGE EXAMPLE
///=============
// The purpose of the 'btesc_TimedAllocatorFactory' protocol is to isolate the
// convention of naming a service (or client request protocol) from a connector
// (or acceptor) programmed to accommodate a homogeneous service (query).
//
// TBD: Need an awesome example of this from the application-level perspective.
//
// Please remember that these example code snippets are intended to illustrate
// the use of 'btesc_TimedAllocatorFactory' and do not represent
// production-quality software.

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

                        // =================================
                        // class btesc_TimedAllocatorFactory
                        // =================================

namespace BloombergLP {

class btesc_TimedAllocatorFactory
// : public btesc_AllocatorFactory                // TBD: later
{
    // This class is a protocol (pure abstract interface) for a (blocking)
    // mechanism that allocates instances of a 'btesc_(Timed)ChannelAllocator'
    // in response to a (null-terminated) character string.  A successful
    // allocation returns the address of a channel allocator and leaves
    // 'status' unmodified.  If the channel-allocator address is null, then a
    // non-negative status indicates an interruption, while a negative status
    // indicates an inability to succeed at the present time, but does not
    // guarantee that the factory is invalid.  An invalid factory cannot
    // successfully allocate a channel allocator.

  private:
    btesc_TimedAllocatorFactory&
        operator=(const btesc_TimedAllocatorFactory&); // not implemented

  protected:
    // CREATORS
    virtual ~btesc_TimedAllocatorFactory();
        // Destroy this channel (required for syntactic consistency only).

  public:
    // MANIPULATORS
    virtual btesc_ChannelAllocator *allocate(int        *status,
                                             const char *name,
                                             int         flags = 0) = 0;
        // Allocate a stream-based-channel allocator appropriate for the
        // specified 'name'.  If the optionally specified 'flags' incorporates
        // 'btesc_Flag::ASYNC_INTERRUPT', "asynchronous events" are permitted
        // to interrupt this operation; by default, such events are ignored.
        // Return the address of a channel allocator on success, and null
        // otherwise.  On an unsuccessful allocation, load the specified
        // 'status' with a positive value if an asynchronous event interrupted
        // the allocation, with -2 if 'name' was not recognized, and with a
        // more negative value (indicating an error) otherwise; 'status' is not
        // modified on success.  A non-null channel-allocator address will
        // remain valid until deallocated explicitly (see 'deallocate').  An
        // allocation that fails with a positive status is likely to succeed if
        // retried.  An allocation error does not *necessarily* invalidate this
        // factory, and so subsequent allocations *may* succeed.  Use the
        // 'isInvalid' method for more information on the state of this
        // factory.  Note that status values of 0 and -1 are not possible for
        // this method.

    virtual btesc_ChannelAllocator *timedAllocate(
                                      int                      *status,
                                      const char               *name,
                                      const bdet_TimeInterval&  timeout,
                                      int                       flags = 0) = 0;
        // Allocate a stream-based-channel allocator appropriate for the
        // specified 'name' or interrupt after the specified absolute 'timeout'
        // time is reached.  If the optionally specified 'flags' incorporates
        // 'btesc_Flag::ASYNC_INTERRUPT', "asynchronous events" are permitted
        // to interrupt this operation; by default, such events are ignored.
        // Return the address of a channel allocator on success, and null
        // otherwise.  On an unsuccessful allocation, load the specified
        // 'status' with 0 if 'timeout' interrupted this operation, with a
        // positive value if the interruption was due to an asynchronous event,
        // with -2 if 'name' was not recognized, and with a more negative value
        // (indicating an error) otherwise; 'status' is not modified on
        // success.  A non-null channel-allocator address will remain valid
        // until deallocated explicitly (see 'deallocate').  An allocation that
        // fails with a non-negative status is likely to succeed if retried.
        // An allocation error does not *necessarily* invalidate this factory,
        // and so subsequent allocations *may* succeed.  Use the 'isInvalid'
        // method for more information on the state of this factory.  Note that
        // if the specified 'timeout' value has already passed, the allocation
        // will still be attempted, but the attempt will not block.  Note also
        // that a status value of -1 is not possible for this method.

    virtual
    btesc_TimedChannelAllocator *allocateTimed(int        *status,
                                               const char *name,
                                               int        flags = 0)       = 0;
        // Allocate a timed stream-based-channel allocator appropriate for the
        // specified 'name'.  If the optionally specified 'flags' incorporates
        // 'btesc_Flag::ASYNC_INTERRUPT', "asynchronous events" are permitted
        // to interrupt this operation; by default, such events are ignored.
        // Return the address of a timed channel allocator on success, and null
        // otherwise.  On an unsuccessful allocation, load the specified
        // 'status' with a positive value if an asynchronous event interrupted
        // the allocation, with -2 if 'name' was not recognized, and with a
        // more negative value (indicating an error) otherwise; 'status' is not
        // modified on success.  A non-null channel-allocator address will
        // remain valid until deallocated explicitly (see 'deallocate').  An
        // allocation that fails with a positive status is likely to succeed if
        // retried.  An allocation error does not *necessarily* invalidate this
        // factory, and so subsequent allocations *may* succeed.  Use the
        // 'isInvalid' method for more information on the state of this
        // factory.  Note that status values of 0 and -1 are not possible for
        // this method.

    virtual
    btesc_TimedChannelAllocator *timedAllocateTimed(
                                      int                      *status,
                                      const char               *name,
                                      const bdet_TimeInterval&  timeout,
                                      int                       flags = 0) = 0;
        // Allocate a timed stream-based-channel allocator appropriate for the
        // specified 'name' or interrupt after the specified absolute 'timeout'
        // time is reached.  If the optionally specified 'flags' incorporates
        // 'btesc_Flag::ASYNC_INTERRUPT', "asynchronous events" are permitted
        // to interrupt this operation; by default, such events are ignored.
        // Return the address of a timed channel allocator on success, and null
        // otherwise.  On an unsuccessful allocation, load the specified
        // 'status' with 0 if 'timeout' interrupted this operation, with a
        // positive value if the interruption was due to an asynchronous event,
        // with -2 if 'name' was not recognized, and with a more negative value
        // (indicating an error) otherwise; 'status' is not modified on
        // success.  A non-null channel-allocator address will remain valid
        // until deallocated explicitly (see 'deallocate').  An allocation that
        // fails with a non-negative status is likely to succeed if retried.
        // An allocation error does not *necessarily* invalidate this factory,
        // and so subsequent allocations *may* succeed.  Use the 'isInvalid'
        // method for more information on the state of this factory.  Note that
        // if the specified 'timeout' value has already passed, the allocation
        // will still be attempted, but the attempt will not block.  Note also
        // that a status value of -1 is not possible for this method.

    virtual void deallocate(const btesc_TimedChannelAllocator
                                                        *channelAllocator) = 0;
        // Reclaim all resources associated with the specified
        // 'channelAllocator'.  The behavior is undefined unless
        // 'channelAllocator' is currently allocated from this factory, (i.e.,
        // was previously obtained from this instance and has not subsequently
        // been deallocated).  Note that being "invalid" does not affect this
        // factory's ability to deallocate outstanding resources.  Note also
        // that this method does not block.

    virtual void invalidate() = 0;
        // Place this factory object in a permanently invalid state.  No
        // subsequent allocation operations will succeed; 'deallocate' is
        // unaffected.

    // ACCESSORS
    virtual int isInvalid() const = 0;
        // Return 1 if this allocator factory is permanently invalid and 0
        // otherwise.  An invalid factory can never again allocate a channel
        // allocator; 'deallocate' isunaffected.  Note that the significance of
        // a 0 return *cannot* be relied upon.
};

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
