// bslstl_stopstate.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLSTL_STOPSTATE
#define INCLUDED_BSLSTL_STOPSTATE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a stop state for 'std'-compliant stop tokens.
//
//@CLASSES:
//  bslstl::StopState: mechanism for requesting stops and invoking callbacks
//  bslstl::StopStateCallbackNode: base class for stop callbacks
//
//@CANONICAL_HEADER: bsl_stop_token.h
//
//@SEE_ALSO: bslstl_stopsource, bslstl_stoptoken, bslstl_stopcallback
//
//@DESCRIPTION: This component provides the 'bslstl::StopState' and
// 'bslstl::StopStateCallbackNode' classes, which are for internal use only
// and are used to implement 'bsl::stop_source', 'bsl::stop_token', and
// 'bsl::stop_callback'.  Please include '<bsl_stop_token.h>' instead.
//
///Usage
///-----
// This component is for internal usage only.  Use 'bsl::stop_source',
// 'bsl::stop_token', and 'bsl::stop_callback' instead.

#include <bsla_nodiscard.h>

#include <bsls_atomic.h>
#include <bsls_bsllock.h>
#include <bsls_exceptionutil.h>
#include <bsls_keyword.h>

namespace BloombergLP {
namespace bslstl {

                          // ========================
                          // class StopState_ListNode
                          // ========================

class StopState_ListNode {
    // This component-private class represents a node in a doubly-linked list.
  private:
    // DATA
    StopState_ListNode *d_prev_p;
    StopState_ListNode *d_next_p;  // set to null by 'unlink'

    // FRIENDS
    friend class StopState;
};

                        // ===========================
                        // class StopStateCallbackNode
                        // ===========================

class StopStateCallbackNode : private StopState_ListNode
{
  private:
    // DATA
    bsls::AtomicBool d_finished;
        // flag indicating whether the callback has completed

    // FRIENDS
    friend class StopState;

  public:
    // MANIPULATORS
    virtual void invoke() BSLS_NOTHROW_SPEC = 0;
        // Invoke the callback stored in the derived class.
};

                              // ===============
                              // class StopState
                              // ===============

class StopState {
  private:
    // DATA
    bsls::BslLock          d_stateMutex;
        // mutex guarding access to this object's data members

    StopState_ListNode     d_head;
        // head node for linked list of callbacks

    unsigned long long     d_stoppingThread;
        // thread ID of the thread that successfully called 'requestStop'

    StopStateCallbackNode *d_currentCallback_p;
        // pointer to the node containing the callback currently being invoked;
        // will be set to null if that callback deregisters its own node

    bsls::AtomicBool       d_stopRequested;
        // flag indicating whether a stop has been requested on this object

    // PRIVATE MANIPULATORS
    void unlink(StopState_ListNode *node);
        // Remove the specified 'node' from the list of nodes registered to
        // this state and set its 'd_next_p' pointer to null.  The behavior is
        // undefined unless 'd_stateMutex' is held by the calling thread and
        // 'node' is registered to this state.

  private:
    // NOT IMPLEMENTED
    StopState(const StopState&) BSLS_KEYWORD_DELETED;
    StopState& operator=(const StopState&) BSLS_KEYWORD_DELETED;

  public:
    // CREATORS
    StopState();
        // Create a 'StopState' object that initially has no registered
        // callbacks and on which a stop has not been requested.

    // MANIPULATORS
    bool enregister(StopStateCallbackNode *node);
        // If a stop has already been requested, then invoke the callback of
        // the specified 'node' and return 'false'.  Otherwise, add 'node' to
        // the list of nodes registered to this state and return 'true'.  The
        // behavior is undefined if this method is called more than once for a
        // given 'node'.

    void deregister(StopStateCallbackNode *node);
        // If the callback of the specified 'node' is currently executing, wait
        // for it to complete.  Otherwise, remove 'node' from the list of nodes
        // registered to this state.  The behavior is undefined if this method
        // is called more than once for a given 'node' or if 'node' was not
        // previously registered to this state by a call to 'enregister' that
        // returned 'true'.

    bool requestStop();
        // If this 'StopState' object has already had a stop requested, return
        // 'false'.  Otherwise, atomically mark this 'StopState' object as
        // having had a stop requested, then execute all registered callbacks
        // in an unspecified order, and finally return 'true'.

    // ACCESSORS
    BSLA_NODISCARD bool stopRequested() const;
        // Return 'true' if a stop has been requested on this 'StopState'
        // object, and 'false' otherwise.
};

}  // close package namespace
}  // close enterprise namespace
#endif  // INCLUDED_BSLSTL_STOPSTATE

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
