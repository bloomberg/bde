// bslmt_barrier.h                                                    -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BSLMT_BARRIER
#define INCLUDED_BSLMT_BARRIER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a thread barrier component.
//
//@CLASSES:
//   bslmt::Barrier: thread barrier class
//
//@SEE_ALSO: bslmt_mutex, bslmt_condition
//
//@DESCRIPTION: This component defines a thread barrier named 'bslmt::Barrier'.
// Barriers provide a simple mechanism for synchronizing a series of threads at
// a given point in a program.  A barrier is constructed with a number
// 'numThreads' which is the number of threads required to reach the
// synchronization point for the barrier to be unblocked.  As each thread
// reaches the synchronization point, it calls the 'wait' method and blocks.
// An invariant is that the number of threads blocking on a barrier is always
// less than 'numThreads'.  Once the required 'numThreads' threads have called
// 'wait', the invariant is restored by unblocking all the threads and
// resetting the barrier to its initial state.  In particular, the barrier can
// be reused several times in succession.
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
///---------------------
// The component 'bsls::SystemClockType' supplies the enumeration indicating
// the system clock on which timeouts supplied to other methods should be
// based.  If the clock type indicated at construction is
// 'bsls::SystemClockType::e_REALTIME', the timeout should be expressed as an
// absolute offset since 00:00:00 UTC, January 1, 1970 (which matches the epoch
// used in 'bsls::SystemTime::now(bsls::SystemClockType::e_REALTIME)'.  If the
// clock type indicated at construction is
// 'bsls::SystemClockType::e_MONOTONIC', the timeout should be expressed as an
// absolute offset since the epoch of this clock (which matches the epoch used
// in 'bsls::SystemTime::now(bsls::SystemClockType::e_MONOTONIC)'.
//
///Usage
///-----
// The following example demonstrates the use of a 'bslmt::Barrier' to create
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
// The 'bslmt::Barrier' is used repeatedly at each processing stage to wait for
// all trades to complete the given stage before continuing to the next stage.
//
// To begin, we define the fundamental structures 'Trade' and 'BasketTrade'.
//..
//  enum {
//      k_MAX_BASKET_TRADES = 10
//  };
//
//  struct Trade {
//      // Trade stuff...
//  };
//
//  struct BasketTrade {
//      bsl::vector<Trade> d_trades;  // array of trade that comprise the
//                                    // basket
//  };
//..
// Functions 'validateTrade', 'insertToDatabase', and 'submitToExchange' define
// functionality for the three stages of trade processing.  Again, the
// 'bslmt::Barrier' will be used so that no individual trade moves forward to
// the next stage before all trades have completed the given stage.  So, for
// instance, no individual trade can call the 'insertToDatabase' function until
// all trades have successfully completed the 'validateTrade' function.
//
// Functions 'deleteFromDatabase' and 'cancelAtExchange' are used for rolling
// back all trades in the event that any one trade fails to move forward.
//
// The implementation of these functions is left incomplete for our example.
//..
//  int validateTrade(Trade &trade)
//  {
//      (void)trade;
//      int result = 0;
//      // Do some checking here...
//
//      return result;
//  }
//
//  int insertToDatabase(Trade &trade)
//  {
//      (void)trade;
//      int result = 0;
//      // Insert the record here...
//
//      return result;
//  }
//
//  int submitToExchange(Trade &trade)
//  {
//      (void)trade;
//      int result = 0;
//      // Do submission here...
//
//      return result;
//  }
//
//  int deleteFromDatabase(Trade &trade)
//  {
//      (void)trade;
//      int result = 0;
//      // Delete record here...
//
//      return result;
//  }
//
//  int cancelAtExchange(Trade &trade)
//  {
//      (void)trade;
//      int result = 0;
//      // Cancel trade here...
//
//      return result;
//  }
//..
// The 'processTrade' function handles a single trade within a Trade Basket.
// Because this function is called within a 'bslmt::Thread' callback (see the
// 'tradeProcessingThread' function, below), its arguments are passed in a
// single structure.  The 'processTrade' function validates a trade, stores the
// trade into a database, and registers that trade with an exchange.  At each
// step, the 'processTrade' function synchronizes with other trades in the
// Trade Basket.
//..
//  struct TradeThreadArgument {
//      bsl::vector<Trade> *d_trades_p;
//      bslmt::Barrier     *d_barrier_p;
//      volatile bool      *d_errorFlag_p;
//      int                 d_tradeNum;
//  };
//
//  TradeThreadArgument *processTrade(TradeThreadArgument *arguments)
//  {
//      int retval;
//      Trade &trade = (*arguments->d_trades_p)[arguments->d_tradeNum];
//
//      retval = validateTrade(trade);
//..
// If this trade failed validation, then indicate that an error has occurred.
// Note that, even when an error occurs, we must still block on the barrier
// object; otherwise, other threads which did not fail would remain blocked
// indefinitely.
//..
//      if (retval) *arguments->d_errorFlag_p = true;
//      arguments->d_barrier_p->wait();
//..
// Once all threads have completed the validation phase, check to see if any
// errors occurred; if so, exit.  Otherwise continue to the next step.
//..
//      if (*arguments->d_errorFlag_p) return arguments;              // RETURN
//
//      retval = insertToDatabase(trade);
//      if (retval) *arguments->d_errorFlag_p = true;
//      arguments->d_barrier_p->wait();
//..
// As before, if an error occurs on this thread, we must still block on the
// barrier object.  This time, if an error has occurred, we need to check to
// see whether this trade had an error.  If not, then the trade has been
// inserted into the database, so we need to remove it before we exit.
//..
//      if (*arguments->d_errorFlag_p) {
//          if (!retval) deleteFromDatabase(trade);
//          return arguments;                                         // RETURN
//      }
//..
// The final synchronization point is at the exchange.  As before, if there is
// an error in the basket, we may need to cancel the individual trade.
//..
//      retval = submitToExchange(trade);
//      if (retval) *arguments->d_errorFlag_p = true;
//      arguments->d_barrier_p->wait();
//      if (*arguments->d_errorFlag_p) {
//          if (!retval) cancelAtExchange(trade);
//          deleteFromDatabase(trade);
//          return arguments;                                         // RETURN
//      }
//..
// All synchronized steps have completed for all trades in this basket.  The
// basket trade is placed.
//..
//      return arguments;
//  }
//..
// Function 'tradeProcessingThread' is a callback for 'bslmt::ThreadUtil',
// which requires 'void' pointers for argument and return type and 'extern "C"'
// linkage.  'bslmt::ThreadUtil::create()' expects a pointer to this function,
// and provides that function pointer to the newly created thread.  The new
// thread then executes this function.
//
// The 'tradeProcessingThread' function receives the 'void' pointer, casts it
// to our required type ('TradeThreadArgument *'), and then calls the
// type-specific function, 'processTrade'.  On return, the specific type is
// cast back to 'void*'.
//..
//  extern "C" void *tradeProcessingThread(void *argumentsIn)
//  {
//      return (void *) processTrade ((TradeThreadArgument *)argumentsIn);
//  }
//..
// Function 'processBasketTrade' drives the example.  Given a 'BasketTrade',
// the function spawns a separate thread for each individual trade in the
// basket, supplying the function 'tradeProcessingThread' to be executed on
// each thread.
//..
//  bool processBasketTrade(BasketTrade& trade)
//      // Return 'true' if the specified basket 'trade' was processed
//      // successfully, and 'false' otherwise.  The 'trade' is processed
//      // atomically, i.e., all the trades succeed, or none of the trades are
//      // executed.
//  {
//      TradeThreadArgument arguments[k_MAX_BASKET_TRADES];
//      bslmt::ThreadAttributes attributes;
//      bslmt::ThreadUtil::Handle threadHandles[k_MAX_BASKET_TRADES];
//
//      int numTrades = trade.d_trades.size();
//      assert(0 < numTrades && k_MAX_BASKET_TRADES >= numTrades);
//..
// Construct the barrier that will be used by the processing threads.  Since a
// thread will be created for each trade in the basket, use the number of
// trades as the barrier count.  When 'bslmt::Barrier::wait()' is called, the
// barrier will require 'numTrades' objects to wait before all are released.
//..
//      bslmt::Barrier barrier(numTrades);
//      bool errorFlag = false;
//..
// Create a thread to process each trade.
//..
//      for (int i = 0; i < numTrades; ++i) {
//          arguments[i].d_trades_p    = &trade.d_trades;
//          arguments[i].d_barrier_p   = &barrier;
//          arguments[i].d_errorFlag_p = &errorFlag;
//          arguments[i].d_tradeNum    = i;
//          bslmt::ThreadUtil::create(&threadHandles[i],
//                                    attributes,
//                                    tradeProcessingThread,
//                                    &arguments[i]);
//      }
//..
// Wait for all threads to complete.
//..
//      for (int i = 0; i < numTrades; ++i) {
//          bslmt::ThreadUtil::join(threadHandles[i]);
//      }
//..
// Check if any error occurred.
//..
//      return false == errorFlag;
//  }
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMT_CONDITION
#include <bslmt_condition.h>
#endif

#ifndef INCLUDED_BSLMT_MUTEX
#include <bslmt_mutex.h>
#endif

#ifndef INCLUDED_BSLS_TIMEINTERVAL
#include <bsls_timeinterval.h>
#endif

#ifndef INCLUDED_BSLS_SYSTEMCLOCKTYPE
#include <bsls_systemclocktype.h>
#endif

namespace BloombergLP {
namespace bslmt {

                              // =============
                              // class Barrier
                              // =============

class Barrier {
    // This class defines a thread barrier.

    Mutex           d_mutex;      // mutex used to control access to this
                                  // barrier.
    Condition       d_cond;       // condition variable used for signaling
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
    explicit Barrier(
    int                         numThreads,
    bsls::SystemClockType::Enum clockType = bsls::SystemClockType::e_REALTIME);
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
        // interval from some epoch, which is determined by the clock indicated
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
}  // close package namespace

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

// CREATORS
inline bslmt::Barrier::Barrier(int                         numThreads,
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
int bslmt::Barrier::numThreads() const
{
    return d_numThreads;
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
