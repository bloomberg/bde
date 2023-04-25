// bdlf_noop.h                                                        -*-C++-*-
#ifndef INCLUDED_BDLF_NOOP
#define INCLUDED_BDLF_NOOP

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a functor class that does nothing.
//
//@CLASSES:
//  bdlf::NoOp: Functor that accepts any parameters and does nothing.
//
//@DESCRIPTION: This component provides a class, 'bdlf::NoOp', that is a
// functor whose function-call operator accepts any number of arguments, does
// nothing (leaving all arguments unmodified), and returns 'void'. On platforms
// where 'inline' 'constexpr' variables are supported, this component also
// provides 'bdlf::noOp', which is a 'constexpr' variable of type 'bdlf::NoOp'.
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: An Unwanted 'bsl::function' Callback
///- - - - - - - - - - - - - - - - - - - - - - - -
// Asynchronous systems often provide callback-based interfaces that invoke
// supplied callbacks at later points in time, often when an event has
// occurred, like the receipt of a data packet on an internet socket.
// Occasionally, we may need to supply these interfaces with a callback even
// though there's nothing we need the callback to do. In these cases, it's
// often possible to supply a "no-op" callback that does nothing.
//
// Consider, for example, the following hypothetical interface to an
// asynchronous system:
//..
//  struct MyAsyncSystemUtil {
//      // TYPES
//      typedef bsl::function<void(int)> StatusReceivedCallback;
//          // 'StatusReceivedCallback' is an alias for a callback functor that
//          // takes as an argument an integer status indicating success (0),
//          // or failures (any non-zero value).
//
//      static void sendPing(const StatusReceivedCallback& callback);
//          // Send a message to the server that this client is still active.
//          // Invoke the specified 'callback' with the value '0' on the worker
//          // thread when this client receives, within the configured time
//          // limit, an acknowledgement from the server that it received the
//          // message.  Otherwise, invoke the 'callback' with a non-zero value
//          // on the worker thread after the configured time limit has expired
//          // or the server or network responds with an error, whichever comes
//          // first.
//  };
//..
// Suppose that we are writing a client for this system that does not need to
// be concerned about the status of 'ping' messages sent to the server, then we
// can use 'bdlf::NoOp' to ignore the acknowledgements:
//..
//  void example1()
//  {
//      MyAsyncSystemUtil::sendPing(bdlf::NoOp());
//  }
//..
//
///Example 2: An Unwanted Template Type Callback
///- - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we are working with an asynchronous system whose interface
// requires a template for a callback instead of a 'bsl::function'.
//
// Consider, for example, the following hypothetical interface to an
// asynchronous system:
//..
//  template <class t_CALLBACK>
//  class AsyncSystem
//  {
//      // This class implements an asynchronous system which does important
//      // example work.  While the important example work is being done, a
//      // callback mechanism of type 't_CALLBACK' is used to provide estimates
//      // about the percentage of the work that is complete.  The type
//      // 't_CALLBACK' must be callable with a single 'int' parameter [0,100].
//
//      // DATA
//      t_CALLBACK d_callback;  // callable that gives feedback when 'run' runs
//
//      public:
//      // CREATORS
//      explicit AsyncSystem(const t_CALLBACK& callback = t_CALLBACK());
//          // Create a 'AsyncSystem' object which can be used to do important
//          // example work.  Optionally specify 'callback', a 't_CALLBACK',
//          // object to be used for giving feedback about progress.  If no
//          // 'callback' is specified, a default-constructed object of type
//          // 't_CALLBACK' will be used.
//
//      // MANIPULATORS
//      void run();
//          // Do the very important example work that this class is designed
//          // to do.  While doing said work, periodically invoke the
//          // 'callback' object provided to the constructor with an estimated
//          // percentage of the task complete as an 'int'.
//  };
//..
// Suppose that we are writing a client of this system that has no useful way
// to report the progress from the callback, then we can use 'bdlf::NoOp' to
// ignore the progress reports:
//..
//  void example2()
//  {
//      AsyncSystem<bdlf::NoOp> mySystem;
//      mySystem.run();
//  }
//..

#include <bdlscm_version.h>

#include <bslmf_istriviallycopyable.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsla_maybeunused.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Include version that can be compiled with C++03
// Generated on Wed Apr 12 13:24:41 2023
// Command line: sim_cpp11_features.pl bdlf_noop.h
# define COMPILING_BDLF_NOOP_H
# include <bdlf_noop_cpp03.h>
# undef COMPILING_BDLF_NOOP_H
#else

namespace BloombergLP {
namespace bdlf {

                                 // ==========
                                 // class NoOp
                                 // ==========

class NoOp {
    // This functor class provides a function-call operator that accepts any
    // number of arguments of any type, does nothing (leaving the arguments
    // unmodified), and returns 'void'.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(NoOp, bsl::is_trivially_copyable);

    // TYPES
    typedef void result_type;
        // 'result_type' is an alias to 'void', which is the type returned by
        // the function-call operator of this class

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    // ACCESSORS
    template <class... t_ARGS>
    inline
    BSLS_KEYWORD_CONSTEXPR_CPP14 void operator()(
                         const t_ARGS&... ignored) const BSLS_KEYWORD_NOEXCEPT;
        // Do nothing and ignore all specified 'ignored' arguments.
#endif
};

// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -AQb01
// BDE_VERIFY pragma: -TR17
#ifdef BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES
inline
constexpr NoOp noOp = NoOp();
#else
extern const NoOp noOp;
#endif
// BDE_VERIFY pragma: pop

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                                 // ----------
                                 // class NoOp
                                 // ----------

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class... t_ARGS>
BSLS_KEYWORD_CONSTEXPR_CPP14 void NoOp::operator()(
         BSLA_MAYBE_UNUSED const t_ARGS&...ignored) const BSLS_KEYWORD_NOEXCEPT
{
}
#endif

}  // close package namespace
}  // close enterprise namespace

#endif // End C++11 code

#endif  // INCLUDED_BDLF_NOOP

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
