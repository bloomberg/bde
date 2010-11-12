// btesc_timedcballocatorfactory.h   -*-C++-*-
#ifndef INCLUDED_BTESC_TIMEDCBALLOCATORFACTORY
#define INCLUDED_BTESC_TIMEDCBALLOCATORFACTORY

//@PURPOSE: Provide protocol for stream-channel-allocator factory with timeout.
//
//@CLASSES:
// btesc_TimedCbAllocatorFactory: non-blocking allocator factory with timeout
//
//@SEE_ALSO: btesc_timedallocatorfactory btemc_timedcballocatorfactory
//
//@AUTHOR: Tom Marshall (tmarshal)
//
//@DESCRIPTION: This component defines an abstract interface for a non-blocking
// mechanism with timeout capability that allocates an appropriate
// 'btesc_(Timed)CbChannelAllocator' corresponding to a specified name
// (string).  'btesc_TimedCbAllocatorFactory' is typically used as an abstract
// naming service that supplies abstract connectors to servers supporting the
// named service.  This additional "stage" in the "lateral architecture" of a
// communications infrastructure decouples the service-name mapping convention
// from the individual connectors that are programmed to connect to a
// homogeneous service.  Note that "factory" as used here is synonymous with
// the term "allocator" used ubiquitously to imply a mechanism that provides
// temporary exclusive ownership of a resource, without making any statement
// about the lifecycle of that resource (here, a
// 'btesc_(Timed)CbChannelAllocator').
//
///Protocol Hierarchy
///------------------
// The interface hierarchy (defined by direct inheritance) of the
// 'btesc_TimedCbAllocatorFactory' protocol is as follows:
//..
//                      ,-----------------------------.
//                     ( btesc_TimedCbAllocatorFactory )
//                      `-----------------------------'
//                                     |
//                                     V
//                        ,------------------------.
//                       ( btesc_CbAllocatorFactory )
//                        `------------------------'
//..
// This protocol adds the ability to allocate timed channel allocators as well
// as a "timeout" capability for the factory allocation methods themselves.
// Note that 'deallocate' cannot block, fail, or throw, and thus does not need
// a timeout facility.
//
///Non-Blocking Channel-Allocator Allocation
///-----------------------------------------
// This interface establishes methods for allocating non-blocking
// stream-based-channel allocators in a non-blocking manner; each method
// registers a user-supplied callback function object (functor) and returns
// immediately.  A successful return status implies that the registered
// callback will be invoked (and, conversely, an unsuccessful status implies
// otherwise).  The callback, in turn, will communicate the results of the
// registered allocation attempt.
//
// Enqueued allocation operations proceed asynchronously to one of four
// possible results: (1) "success" -- the requested channel allocator is
// allocated and its (non-null) address is passes to the callback function; in
// this case the callback "status" argument has no meaning, (2) "interrupted"
// -- the operation was asynchronously interrupted (e.g., via a timeout): the
// channel-allocator address is null and the (non-negative) status conveys the
// nature of the interruption, (3) "canceled" -- the operation was aborted
// (synchronously) by an explicit call to 'cancelAll': the channel-allocator
// address in null and the status" is -1, (4) "name unknown" -- the specified
// 'name' was not recognized: the channel-allocator address is null and status
// is -2, or (5) "error" -- an implementation-dependent error occurred: the
// channel-allocator address is null and status is less than -2.  The user may
// retry interrupted and canceled operations with a reasonable expectation of
// success.  An "error" status implies that allocation is unlikely to succeed
// if retried, but does not necessarily invalidate the factory.  The
// 'isInvalid' method may be used to confirm the occurrence of a permanent
// error.  If the factory is valid, an allocation operation will be enqueued
// and may succeed.  Otherwise, the allocation request itself will not succeed.
//
// The meanings of the callback function status value for an unsuccessful
// allocation (i.e., a null channel-allocator address) are summarized as
// follows:
//..
//  "status"      meaning (only when returned channel address is null)
//  --------      -------------------------------------------------------------
//  positive      Interruption by an "asynchronous event"
//
//  zero          Interruption by a user-requested timeout
//
//  -1            Operation explicitly canceled (synchronously) by the user
//
//  -2            Specified 'name' not recognized
//
//  < -2          Allocation operation unable to succeed at this time
//..
// Note that unless asynchronous events are explicitly enabled (see below),
// they are ignored, and "status" will never be positive.  Note also that
// whether the callback is invoked before or after the registering method
// returns is not specified; in either case, if the registration was
// successful, then the return value will reflect success.
//
///Callback Functor Registration
///- - - - - - - - - - - - - - -
// Once an operation is successfully initiated, a (reference-counted) copy of
// the ('bcef') callback functor is retained by the concrete factory until the
// callback is executed.  Therefore, the user need not be concerned with
// preserving any resources used by the callback.
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
// interruption is provided via the callback as the 'status' value (see above).
//
// Concrete allocator factories will make a "best effort" to honor the timeout
// request promptly, but no guarantee can be made as to the maximum duration of
// any particular allocation attempt.  Any implementation of this interface
// will support a timeout granularity of ten milliseconds (0.01s) or less.  The
// timeout is guaranteed *not* to occur before the specified time has passed.
// If a timeout is specified with a time that has already passed, the
// allocation will be attempted, but will not block.  Note that a long-running
// operation may affect subsequent allocation requests.
//
///USAGE EXAMPLE
///=============
// The purpose of the 'btesc_TimedCbAllocatorFactory' protocol is to isolate
// the convention of naming a service (or client request protocol) from a
// connector (or acceptor) programmed to accommodate a homogeneous service
// (query).
//
// TBD: Need an awesome example of this from the application-level perspective.
//
// Please remember that these example code snippets are intended to illustrate
// the use of 'btesc_TimedCbAllocatorFactory' and do not represent
// production-quality software.

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

                        // ===================================
                        // class btesc_TimedCbAllocatorFactory
                        // ===================================

namespace BloombergLP {

class btesc_TimedCbAllocatorFactory
// : public btesc_CbAllocatorFactory                // TBD: later
{
    // This class is a protocol (pure abstract interface) for a non-blocking
    // mechanism with timeout capability that allocates instances of a
    // 'btesc_(Timed)CbChannelAllocator' in response to a (null-terminated)
    // character string.  A 'bcef' callback functor communicates the results of
    // the asynchronous allocation.  A successful allocation passes to the
    // callback the address of a channel allocator, in which case the
    // callback's "status" argument has no significance.  Otherwise, a
    // non-negative callback status indicates an asynchronous interruption (0
    // for a caller-requested timeout), a status of -1 implies that the
    // operation was canceled (synchronously) by the caller (see 'cancelAll'),
    // and a negative status value other than -1 implies an error (i.e., an
    // inability of this object to succeed at the present time).  A failure to
    // register an allocation operation (or an explicit call to 'invalidate')
    // implies that the channel-allocator factory is permanently invalid (see
    // 'isInvalid').  An invalid factory cannot successfully register new
    // allocation operations, but pending allocation operations will not
    // necessarily fail.  Note that an invalid factory is still capable of
    // deallocation.

  private:
    btesc_TimedCbAllocatorFactory&
        operator=(const btesc_TimedCbAllocatorFactory&); // not implemented

  protected:
    // CREATORS
    virtual ~btesc_TimedCbAllocatorFactory();
        // Destroy this channel (required for syntactic consistency only).

  public:
    // TYPES
    typedef bcef_Vfunc2<btesc_CbChannelAllocator *, int> Callback;
        // Invoked as a result of an 'allocate' or 'timedAllocate' request,
        // 'Callback' is an alias for a callback function object (functor) that
        // returns 'void' and takes as arguments the (possibly null) address of
        // a callback "channelAllocator" and an integer "status" indicating
        // either an interruption (non-negative, 0 if and only if timeout), a
        // canceled operation (-1), or an error (less than -1).  Note that
        // "status" is meaningful only if "channelAllocator" is 0.

    typedef bcef_Vfunc2<btesc_TimedCbChannelAllocator *, int> TimedCallback;
        // Invoked as a result of an 'allocateTimed' or 'timedAllocateTimed'
        // request, 'TimedCallback' is an alias for a callback function object
        // (functor) that returns 'void' and takes as arguments the (possibly
        // null) address of a timed callback "channelAllocator" and an integer
        // "status" indicating either an interruption (non-negative, 0 if and
        // only if timeout), a canceled operation (-1), or an error (less than
        // -1).  Note that "status" is meaningful only if "channelAllocator" is
        // 0.

    // MANIPULATORS
    virtual int allocate(const char      *name,
                         const Callback&  callback,
                         int              flags = 0)                       = 0;
        // Initiate a non-blocking operation to allocate an instance of a
        // 'btesc_CbChannelAllocator' appropriate for the specified 'name';
        // execute the specified 'callback' functor after the allocation
        // operation terminates.  If the optionally specified 'flags'
        // incorporates 'btesc_Flag::ASYNC_INTERRUPT', "asynchronous events" are
        // permitted to interrupt the allocation; by default, such events are
        // ignored.  Return 0 on successful initiation, and a non-zero value
        // otherwise (in which case 'callback' will not be invoked).
        //
        // When invoked, 'callback' is passed the (possibly null) address of a
        // callback "channelAllocator" and an integer "status".  If that
        // address is not 0, the allocation succeeded and the status has no
        // meaning; a non-null channel-allocator address will remain valid
        // until deallocated explicitly (see 'deallocate').  If the address is
        // 0, a positive status indicates an interruption due to an
        // asynchronous event; subsequent allocation attempts may succeed.  A
        // status of -1 indicates that the allocation operation was "canceled"
        // (synchronously) by the caller (see 'cancelAll') and, often, may be
        // retried successfully.  A status of -2 indicates that 'name' was not
        // recognized.  A status less than -2 indicates a more persistent
        // error, but not necessarily a permanent one; the factory itself may
        // still be valid (see 'isInvalid').

    virtual int timedAllocate(const char               *name,
                              const bdet_TimeInterval&  timeout,
                              const Callback&           callback,
                              int                       flags = 0)         = 0;
        // Initiate a non-blocking operation to allocate an instance of a
        // 'btesc_CbChannelAllocator' appropriate for the specified 'name' or
        // interrupt after the specified absolute 'timeout' time is reached;
        // execute the specified 'allocateCallback' functor after the
        // allocation operation terminates.  If the optionally specified
        // 'flags' incorporates 'btesc_Flag::ASYNC_INTERRUPT', "asynchronous
        // events" are permitted to interrupt this operation; by default, such
        // events are ignored.  Return 0 on successful initiation, and a
        // non-zero value otherwise (in which case 'callback' will not be
        // invoked).
        //
        // When invoked, 'callback' is passed the (possibly null) address of a
        // callback "channelAllocator" and an integer "status".  If that
        // address is not 0, the allocation succeeded and status has no
        // meaning; a non-null channel-allocator address will remain valid
        // until deallocated explicitly (see 'deallocate').  If the address is
        // 0, a 0 status indicates that the operation has timed out, while a
        // positive status indicates an interruption due to an asynchronous
        // event.  In either case, subsequent allocation attempts may succeed.
        // A status of -1 implies that the allocation operation was "canceled"
        // (synchronously) by the caller (see 'cancelAll') and, often, may be
        // retried successfully.  A status of -2 indicates that 'name' was not
        // recognized.  A status less than -2 indicates a more persistent
        // error, but not necessarily a permanent one; the factory itself may
        // still be valid (see 'isInvalid').  Note that if the specified
        // 'timeout' value has already passed, the allocation will still be
        // attempted, but the attempt will not block.

    virtual int allocateTimed(const char           *name,
                              const TimedCallback&  timedCallback,
                              int                   flags = 0)             = 0;
        // Initiate a non-blocking operation to allocate an instance of a
        // 'btesc_TimedCbChannelAllocator' appropriate for the specified
        // 'name'; execute the specified 'timedCallback' functor after the
        // allocation operation terminates.  If the optionally specified
        // 'flags' incorporates 'btesc_Flag::ASYNC_INTERRUPT', "asynchronous
        // events" are permitted to interrupt the allocation; by default, such
        // events are ignored.  Return 0 on successful initiation, and a
        // non-zero value otherwise (in which case 'timedCallback' will not be
        // invoked).
        //
        // When invoked, 'callback' is passed the (possibly null) address of a
        // timed callback "channelAllocator" and an integer "status".  If that
        // address is not 0, the allocation succeeded and status has no
        // meaning; a non-null channel-allocator address will remain valid
        // until deallocated explicitly (see 'deallocate').  If the address is
        // 0, a positive status indicates an interruption due to an
        // asynchronous event; subsequent allocation attempts may succeed.  A
        // status of -1 implies that the allocation operation was "canceled"
        // (synchronously) by the caller (see 'cancelAll') and, often, may be
        // retried successfully.  A status of -2 indicates that 'name' was not
        // recognized.  A status less than -2 indicates a more persistent
        // error, but not necessarily a permanent one; the factory itself may
        // still be valid (see 'isInvalid').

    virtual int timedAllocateTimed(const char               *name,
                                   const bdet_TimeInterval&  timeout,
                                   const TimedCallback&      timedCallback,
                                   int                       flags = 0)    = 0;
        // Initiate a non-blocking operation to allocate an instance of a
        // 'btesc_TimedCbChannelAllocator' appropriate for the specified 'name'
        // or interrupt after the specified absolute 'timeout' time is reached;
        // execute the specified 'timedCallback' functor after the allocation
        // operation terminates.  If the optionally specified 'flags'
        // incorporates 'btesc_Flag::ASYNC_INTERRUPT', "asynchronous events" are
        // permitted to interrupt this operation; by default, such events are
        // ignored.  Return 0 on successful initiation, and a non-zero value
        // otherwise (in which case 'timedCallback' will not be invoked).
        //
        // When invoked, 'callback' is passed the (possibly null) address of a
        // timed callback "channelAllocator" and an integer "status".  If that
        // address is not 0, the allocation succeeded and status has no
        // meaning; a non-null channel-allocator address will remain valid
        // until deallocated explicitly (see 'deallocate').  If the address is
        // 0, a 0 status indicates that the operation has timed out, while a
        // positive status indicates an interruption due to an asynchronous
        // event.  In either case, subsequent allocation attempts may succeed.
        // A status of -1 implies that the allocation operation was "canceled"
        // (synchronously) by the caller (see 'cancelAll') and, often, may be
        // retried successfully.  A status of -2 indicates that 'name' was not
        // recognized.  A status less than -2 indicates a more persistent
        // error, but not necessarily a permanent one; the factory itself may
        // still be valid (see 'isInvalid').  Note that if the specified
        // 'timeout' value has already passed, the allocation will still be
        // attempted, but the attempt will not block.

    virtual void cancelAll() = 0;
        // Immediately cancel all pending operations on this factory, invoking
        // each registered allocation callback in turn.  Each callback will be
        // invoked with a null allocator and a status of -1.  This method may
        // be invoked successfully on an invalid factory; however, calling the
        // method does not invalidate the factory.  Note that calling
        // 'cancelAll' from a callback that has itself been canceled simply
        // extends the set of canceled operations to include any new ones
        // initiated since the previous 'cancelAll' was invoked.

    virtual void deallocate(const btesc_TimedCbChannelAllocator
                                                        *channelAllocator) = 0;
        // Reclaim all resources associated with the specified
        // 'channelAllocator'.  First, invalidate 'channelAllocator' (via its
        // 'invalidate' method).  Second, invoke the 'cancelAll' method on
        // 'channelAllocator'.  Finally, reclaim all resources afforded
        // 'channelAllocator'.  The behavior is undefined unless
        // 'channelAllocator' is currently allocated from this factory, (i.e.,
        // was previously obtained from this instance and has not subsequently
        // been deallocated).  Note that being "invalid" does not affect this
        // factory's ability to deallocate outstanding resources.  Note also
        // that this method can never block.

    virtual void invalidate() = 0;
        // Place this factory object in a permanently invalid state.  No new
        // allocation operations can be initiated; deallocation and previously
        // initiated allocation requests are not affected.

    // ACCESSORS
    virtual int isInvalid() const = 0;
        // Return 1 if this allocator factory is permanently invalid and 0
        // otherwise.  An invalid factory can never again register a request to
        // allocate a channel allocator, but *may* succeed in completing
        // existing enqueued requests; deallocation and cancel operations are
        // unaffected.  Note that the significance of a 0 return cannot be
        // relied upon beyond the return of this method.
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
