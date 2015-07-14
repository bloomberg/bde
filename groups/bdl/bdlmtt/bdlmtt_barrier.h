// bdlmtt_barrier.h                                                    -*-C++-*-
#ifndef INCLUDED_BDLMTT_BARRIER
#define INCLUDED_BDLMTT_BARRIER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")


//@PURPOSE: Provide a thread barrier component.
//
//@CLASSES:
//   bdlmtt::Barrier: thread barrier class
//
//@SEE_ALSO: bdlmtt_xxxthread
//
//@AUTHOR: Ilougino Rocha(irocha)
//
//@DESCRIPTION: This component defines a thread "barrier".  Barriers provide a
// simple mechanism for synchronizing a series of threads at a given point in a
// program.  A barrier is constructed with a number 'numThreads' which is the
// number of threads required to reach the synchronization point for the
// barrier to be unblocked.  As each thread reaches the synchronization point,
// it calls the 'wait' method and blocks.  An invariant is that the number of
// threads blocking on a barrier is always less than 'numThreads'.  Once the
// required 'numThreads' threads have called 'wait', the invariant is restored
// by unblocking all the threads and resetting the barrier to its initial
// state.  In particular, the barrier can be reused several times in
// succession.
//
// Note that the number of threads sharing the use of the barrier should be
// exactly 'numThreads', as only exactly 'numThreads' threads calling 'wait'
// will be unblocked.  In particular, extra threads calling 'wait' will block,
// perhaps unwittingly participating in the next round of reuse of the barrier
// together with the unblocked 'numThreads' threads (leading to potential race
// conditions).
//
// Note also that the behavior is undefined if a barrier is destroyed while one
// or more threads are waiting on it.
//
///Supported Clock-Types
///-------------------------
// The component 'bsls::SystemClockType' supplies the enumeration indicating
// the system clock on which timeouts supplied to other methods should be
// based.  If the clock type indicated at construction is
// 'bsls::SystemClockType::e_REALTIME', the timeout should be expressed as an
// absolute offset since 00:00:00 UTC, January 1, 1970 (which matches the epoch
// used in 'bdlt::CurrentTime::now(bsls::SystemClockType::e_REALTIME)'.  If the
// clock type indicated at construction is
// 'bsls::SystemClockType::e_MONOTONIC', the timeout should be expressed as an
// absolute offset since the epoch of this clock (which matches the epoch used
// in 'bdlt::CurrentTime::now(bsls::SystemClockType::e_MONOTONIC)'.
//
///Usage
///-----
// The following example demonstrates the use of a 'bdlmtt::Barrier' to create
// "checkpoints" in a threaded "basket trade" processing logic.  In this
// example, a "basket" is a series of trades submitted as one logical trade.
// If any given trade fails to process for any reason, then all the trades must
// be cancelled.
//
// The example is driven through function 'processBasketTrade', which takes as
// its argument a reference to a 'BasketTrade' structure.  The 'BasketTrade'
// structure contains a collection of 'Trade' objects; the 'processBasketTrade'
// function creates a separate thread to manage each 'Trade' object.
//
// The 'Trade' threads proceed independently, except that they synchronize with
// one another at various stages of the trade processing: each thread waits for
// all trades to complete a given step before any individual trade thread
// proceeds to the next step.
//
// The 'bdlmtt::Barrier' is used repeatedly at each processing stage to wait for
// all trades to complete the given stage before continuing to the next stage.
//
// To begin, we define the fundamental structures 'Trade' and 'BasketTrade'.
//..
// enum {
//     MAX_BASKET_TRADES = 10
// };
//
// struct Trade {
//     // Trade stuff...
// };
//
// struct BasketTrade {
//     bsl::vector<Trade> d_trades; // array of trades that comprise the basket
// };
//..
// Functions 'validateTrade', 'insertToDatabase', and 'submitToExchange' define
// functionality for the three stages of trade processing.  Again, the
// 'bdlmtt::Barrier' will be used so that no individual trade moves forward to
// the next stage before all trades have completed the given stage.  So, for
// instance, no individual trade can call the 'insertToDatabase' function until
// all trades have successfully completed the 'validateTrade' function.
//
// Functions 'deleteFromDatabase' and 'cancelAtExchange' are used for rolling
// back all trades in the event that any one trade fails to move forward.
//
// The implementation of these functions is left incomplete for our example.
//..
// int validateTrade(Trade &trade)
// {
//     int result = 0;
//     // Do some checking here...
//
//     return result;
// }
//
// int insertToDatabase(Trade &trade)
// {
//     int result = 0;
//     // Insert the record here...
//
//     return result;
// }
//
// int submitToExchange(Trade &trade)
// {
//     int result = 0;
//     // Do submission here...
//
//     return result;
// }
//
// int deleteFromDatabase(Trade &trade)
// {
//     int result = 0;
//     // Delete record here...
//
//     return result;
// }
//
// int cancelAtExchange(Trade &trade)
// {
//     int result = 0;
//     // Cancel trade here...
//
//     return result;
// }
//..
// The 'processTrade' function handles a single trade within a Trade Basket.
// Because this function is called within a 'bcemt::Thread' callback (see the
// 'tradeProcessingThread' function, below), its arguments are passed in a
// single structure.  The 'processTrade' function validates a trade, stores the
// trade into a database, and registers that trade with an exchange.  At each
// step, the 'processTrade' function synchronizes with other trades in the
// Trade Basket.
//..
// struct TradeThreadArgument {
//     bsl::vector<Trade> *d_trades_p;
//     bdlmtt::Barrier      *d_barrier_p;
//     volatile bool      *d_errorFlag_p;
//     int                 d_tradeNum;
// };
//
// TradeThreadArgument *processTrade(TradeThreadArgument *args)
// {
//     int retval;
//     Trade &trade = (*args->d_trades_p)[args->d_tradeNum];
//
//     retval = validateTrade(trade);
//..
// If this trade failed validation, then indicate that an error has occurred.
// Note that, even when an error occurs,  we must still block on the barrier
// object; otherwise, other threads which did not fail would remain blocked
// indefinitely.
//..
//     if (retval) *args->d_errorFlag_p = true;
//     args->d_barrier_p->wait();
//..
// Once all threads have completed the validation phase, check to see if any
// errors occurred; if so, exit.  Otherwise continue to the next step.
//..
//     if (*args->d_errorFlag_p) return args;
//
//     retval = insertToDatabase(trade);
//     if (retval) *args->d_errorFlag_p = true;
//     args->d_barrier_p->wait();
//..
// As before, if an error occurs on this thread, we must still block on the
// barrier object.  This time, if an error has occurred, we need to check to
// see whether this trade had an error.  If not, then the trade has been
// inserted into the database, so we need to remove it before we exit.
//..
//     if (*args->d_errorFlag_p) {
//        if (!retval) deleteFromDatabase(trade);
//        return args;
//     }
//
//..
// The final synchronization point is at the exchange.  As before, if there is
// an error in the basket, we may need to cancel the individual trade.
//..
//     retval = submitToExchange(trade);
//     if (retval) *args->d_errorFlag_p = true;
//     args->d_barrier_p->wait();
//     if (*args->d_errorFlag_p) {
//        if (!retval) cancelAtExchange(trade);
//        deleteFromDatabase(trade);
//        return args;
//     }
//..
// All synchronized steps have completed for all trades in this basket.  The
// basket trade is placed.
//..
//     return args;
// }
//..
// Function 'tradeProcessingThread' is a callback for 'bdlmtt::ThreadUtil', which
// requires 'void' pointers for argument and return type and extern "C"
// linkage.  'bdlmtt::ThreadUtil::create()' expects a pointer to this function,
// and provides that function pointer to the newly created thread.  The new
// thread then executes this function.
//
// The 'tradeProcessingThread' function receives the 'void' pointer, casts it
// to our required type ('TradeThreadArgument *'), and then calls the
// type-specific function, 'processTrade'.  On return, the specific type is
// cast back to 'void*'.
//..
// extern "C" void* tradeProcessingThread(void *argsIn)
// {
//     return (void *) processTrade ((TradeThreadArgument *)argsIn);
// }
//..
// Function 'processBasketTrade' drives the example.  Given a 'BasketTrade',
// the function spawns a separate thread for each individual trade in the
// basket, supplying the function 'tradeProcessingThread' to be executed on
// each thread.
//..
// void processBasketTrade(BasketTrade &trade)
//     // Return 'true' if the basket trade was processed successfully,
//     // 'false' otherwise.  The trade is processed atomically, i.e.,
//     // all the trades succeed, or none of the trades are executed.
// {
//     TradeThreadArgument arguments[MAX_BASKET_TRADES];
//     bcemt_Attribute attributes;
//     bdlmtt::ThreadUtil::Handle threadHandles[MAX_BASKET_TRADES];
//
//     int numTrades = trade.d_trades.length();
//     assert(0 < numTrades && MAX_BASKET_TRADES >= numTrades);
//
//..
// Construct the barrier that will be used by the processing threads.  Since a
// thread will be created for each trade in the basket, use the number of
// trades as the barrier count.  When 'bdlmtt::Barrier::wait()' is called, the
// barrier will require 'numTrades' objects to wait before all are released.
//..
//     bdlmtt::Barrier barrier(numTrades);
//     bool errorFlag = 0;
//
//..
// Create a thread to process each trade.
//..
//     for (int i = 0; i < numTrades; ++i) {
//         arguments[i].d_trades_p    = &trade.d_trades;
//         arguments[i].d_barrier_p   = &barrier;
//         arguments[i].d_errorFlag_p = &errorFlag;
//         arguments[i].d_tradeNum    = i;
//         bdlmtt::ThreadUtil::create(&threadHandles[i], attributes,
//                                  tradeProcessingThread, &arguments[i]);
//     }
//..
// Wait for all threads to complete.
//..
//     for (int i = 0; i < numTrades; ++i) {
//         bdlmtt::ThreadUtil::join(threadHandles[i]);
//     }
//..
// Check if any error occurred.
//..
//     return (false == errorFlag);
// }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLMTT_CONDITION
#include <bdlmtt_condition.h>
#endif

#ifndef INCLUDED_BDLMTT_MUTEX
#include <bdlmtt_mutex.h>
#endif

#ifndef INCLUDED_BDLT_TIMEINTERVAL
#include <bsls_timeinterval.h>
#endif

#ifndef INCLUDED_BSLS_SYSTEMCLOCKTYPE
#include <bsls_systemclocktype.h>
#endif

namespace BloombergLP {

namespace bdlmtt {
                             // ===================
                             // class Barrier
                             // ===================

class Barrier {
    // This class defines a thread barrier.

    Mutex     d_mutex;      // mutex used to control access to this
                                  // barrier.
    Condition d_cond;       // condition variable used for signaling
                                  // blocked threads.
    const int       d_numThreads; // number of threads required to be waiting
                                  // before this barrier can be signaled.
    int             d_numWaiting; // number of threads currently waiting for
                                  // this barrier to be signaled.
    int             d_sigCount;   // counted of number of times this barrier
                                  // has been signaled.
    int             d_numPending; // Number of threads that have been signaled
                                  // but have not yet awakened.

    // NOT IMPLEMENTED
    Barrier(const Barrier&);
    Barrier& operator=(const Barrier&);

  public:
    // CREATORS
    explicit
    Barrier(
             int                         numThreads,
             bsls::SystemClockType::Enum clockType
                                          = bsls::SystemClockType::e_REALTIME);
        // Create a barrier that requires the specified 'numThreads' to
        // unblock.  Optionally specify a 'clockType' indicating the type of
        // the system clock against which the 'bsls::TimeInterval' timeouts
        // passed to the 'timedWait' method are to be interpreted.  If
        // 'clockType' is not specified then the realtime system clock is used.
        // The behavior is undefined unless '0 < numThreads'.

    ~Barrier();
        // Wait for all *signaled* threads to unblock and destroy this barrier.
        // (See 'wait' and 'timedWait' below for the meaning of *signaled*.)
        // Note that the behavior is undefined if a barrier is destroyed while
        // one or more threads are waiting on it.

    // MANIPULATORS
    int timedWait(const bsls::TimeInterval &timeout);
        // Block until the required number of threads have called 'wait' or
        // 'timedWait' on this barrier, or until the specified 'timeout'
        // expires.  In the former case, *signal* all the threads that are
        // currently waiting on this barrier to unblock, reset the state of
        // this barrier to its initial state, and return 0.  If this method
        // times out before the required number of threads are waiting, the
        // thread is released to proceed and ceases to contribute to the number
        // of threads waiting.  Return a non-zero value if a timeout or error
        // occurs.  The 'timeout' is an absolute time represented as an
        // interval from some epoch, which is detemined by the clock indicated
        // at construction (see {'Supported Clock-Types'} in the component
        // documentation).  Note that 'timedWait' and 'wait' should not
        // generally be used together; if one or more threads called 'wait'
        // while others called 'timedWait', then if the thread(s) that called
        // 'timedWait' were to time out and not retry, the threads that called
        // 'wait' would never unblock.

    void wait();
        // Block until the required number of threads have called either 'wait'
        // or 'timedWait' on this barrier.  Then *signal* all the threads that
        // are currently waiting on this barrier to unblock and reset the state
        // of this barrier to its initial state.  Note that generally 'wait'
        // and 'timedWait' should not be used together, for reasons explained
        // in the documentation of 'timedWait'.

    // ACCESSORS
    int numThreads() const;
        // Return the number of threads that are required to call 'wait' before
        // all waiting threads will unblock.
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

// CREATORS
inline
Barrier::Barrier(int                         numThreads,
                             bsls::SystemClockType::Enum clockType)
: d_mutex()
, d_cond(clockType)
, d_numThreads(numThreads)
, d_numWaiting(0)
, d_sigCount(0)
, d_numPending(0)
{
}

// ACCESSORS
inline
int Barrier::numThreads() const
{
    return d_numThreads;
}
}  // close package namespace

}  // close namespace BloombergLP

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
