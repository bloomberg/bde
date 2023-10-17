// bdlm_defaultmetricsregistrar.h                                     -*-C++-*-
#ifndef INCLUDED_BDLM_DEFAULTMETRICSREGISTRAR
#define INCLUDED_BDLM_DEFAULTMETRICSREGISTRAR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide utilities to set/fetch the default metrics registrar.
//
//@CLASSES:
//  bdlm::DefaultMetricsRegistrar: namespace for default registrar utilities
//
//@DESCRIPTION: This component provides a set of utility functions that manage
// the address of the *default* metrics registrar.  This metric registrar is of
// type derived from 'bdlm::MetricsRegistrar'.  Note that for brevity, in the
// following we will generally refer to "the address of the default metrics
// registrar" as simply "the default metrics registrar".
//
// In general, the default metrics registrar is for all metrics registration in
// contexts where an alternative metrics registrar is not explicitly specified.
//
// If not set with 'setDefaultMetricsRegistrar', the default metrics registrar
// resolves to one that does not register collection objects with any
// monitoring system.
//
// Two methods provide access to the default metrics registrar,
// 'bdlm::DefaultMetricsRegistrar::defaultMetricsRegistrar' and
// 'bdlm::DefaultMetricsRegistrar::metricsRegistrar' (the latter when called
// with no argument, or an explicit 0).  When
// 'bdlm::DefaultMetricsRegistrar::metricsRegistrar' is supplied with a non-0
// argument, it simply returns that argument to the caller, (i.e., it acts as a
// pass-through).  A class that is designed to take advantage of a metrics
// registrar will typically revert to the default metrics registrar whenever a
// constructor is called without a metrics registrar (yielding the default
// argument value of 0).  The 'bdlm::DefaultMetricsRegistrar::metricsRegistrar'
// method facilitates this behavior.
//
// The default metrics registrar can be set *prior* to a call to
// 'bdlm::DefaultMetricsRegistrar::defaultMetricsRegistrar' or to
// 'bdlm::DefaultMetricsRegistrar::metricsRegistrar' with no argument or an
// explicit 0 by calling
// 'bdlm::DefaultMetricsRegistrar::setDefaultMetricsRegistrar'.  This method
// returns 0 on success and a non-zero value on failure.  This method fails
// when called after an invocation of
// 'bdlm::DefaultMetricsRegistrar::defaultMetricsRegistrar' or
// 'bdlm::DefaultMetricsRegistrar::metricsRegistrar' with no argument or an
// explicit 0.
//
///Thread Safety
///-------------
// This class is *thread* *aware* (see {'bsldoc_glossary'|Thread Aware}).
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Default Metrics Registrar Use
/// - - - - - - - - - - - - - - - - - - - - - - -
// This usage example illustrates the basics of class design that relate to
// proper use of the default metrics registrar, and introduces the standard
// pattern to apply when setting (and *locking*) the default metrics registrar.
// First we define a trivial class, 'my_ThreadPool', that uses a metrics
// registrar.  Note that the constructor is declared to take an *optional*
// 'bdlm::MetricsRegistrar *' as its last argument.  Also note that the
// expression
// 'bdlm::DefaultMetricsRegistrar::metricsRegistrar(basicMetricsRegistrar)' is
// used in the applicable member initializer to propagate the constructor's
// metrics registrar argument to the data members that require it.  If
// 'basicMetricsRegistrar' is 0, the object is created using the default
// metrics registrar.  Otherwise, the explicitly supplied metrics registrar is
// used:
//..
//  class my_ThreadPool {
//      // This is a trivial class solely intended to illustrate proper use
//      // of the default metrics registrar.
//
//      // DATA
//      bdlm::MetricsRegistrar *d_metricsRegistrar_p;  // held, not owned
//
//      // NOT IMPLEMENTED
//      my_ThreadPool(const my_ThreadPool&);
//      my_ThreadPool& operator=(const my_ThreadPool&);
//
//    public:
//      // CREATORS
//      explicit my_ThreadPool(
//                          bdlm::MetricsRegistrar *basicMetricsRegistrar = 0);
//          // Create an ThreadPool object.  Optionally specify a
//          // 'basicMetricsRegistrar' to be used for reporting metrics.  If
//          // 'basicMetricsRegistrar' is 0, the currently installed default
//          // metrics registrar is used.
//
//      ~my_ThreadPool();
//          // Destroy this thread pool object.
//  };
//
//  // CREATORS
//  inline
//  my_ThreadPool::my_ThreadPool(bdlm::MetricsRegistrar *basicMetricsRegistrar)
//  : d_metricsRegistrar_p(bdlm::DefaultMetricsRegistrar::metricsRegistrar(
//                                                      basicMetricsRegistrar))
//  {
//  }
//
//  inline
//  my_ThreadPool::~my_ThreadPool()
//  {
//  }
//..
// Now, we set the default metrics registrar to the unmonitored metrics
// registrar singleton provided in this component:
//..
//  int status = bdlm::DefaultMetricsRegistrar::setDefaultMetricsRegistrar(
//              bdlm::DefaultMetricsRegistrar::notMonitoredMetricsRegistrar());
//  assert(0 == status);
//..
// Finally, we instantiate an object of type 'my_ThreadPool', without supplying
// a metrics registrar.  Note that the creation of this object will lock the
// default metrics registrar, and subsequent invocations of
// 'setDefaultMetricsRegistrar' will fail:
//..
//  my_ThreadPool pool;
//
//  status = bdlm::DefaultMetricsRegistrar::setDefaultMetricsRegistrar(
//              bdlm::DefaultMetricsRegistrar::notMonitoredMetricsRegistrar());
//  assert(1 == status);
//..

#include <bdlscm_version.h>

#include <bdlm_metricsregistrar.h>

#include <bsls_assert.h>
#include <bsls_atomicoperations.h>
#include <bsls_types.h>

namespace BloombergLP {
namespace bdlm {

struct DefaultMetricsRegistrar_TestUtil;

                      // ==============================
                      // struct DefaultMetricsRegistrar
                      // ==============================

struct DefaultMetricsRegistrar {
    // This struct is a mechanism with global state, i.e., all state is held in
    // global variables and all functions are class methods.  The state
    // consists of the default metrics registrar.  The address of the metrics
    // registrar is stored without assuming ownership.

  private:
    // TYPES
    typedef bsls::AtomicOperations AtomicOp;

    // CONSTANTS
    static const bsls::Types::Uint64 k_LOCKED_FLAG  = 0x0000000000000001LL;
    static const bsls::Types::Uint64 k_POINTER_MASK = 0xfffffffffffffffeLL;

    // CLASS DATA
    static bsls::AtomicOperations::AtomicTypes::Uint64
                s_defaultMetricsRegistrar;  // the default metrics registrar

    // FRIENDS
    friend struct DefaultMetricsRegistrar_TestUtil;

  public:
    // CLASS METHODS
    static MetricsRegistrar *defaultMetricsRegistrar();
        // Return the address of the default metrics registrar and disable all
        // subsequent calls to the 'setDefaultMetricsRegistrar' method.  Note
        // that prior to the first call to 'setDefaultMetricsRegistrar' method,
        // the address of the default metrics registrar is that of a
        // 'NotMonitoredMetricsRegistrar'.

    static MetricsRegistrar *metricsRegistrar(
                                  MetricsRegistrar *basicMetricsRegistrar = 0);
        // Return the metrics registrar returned by 'defaultMetricsRegistrar'
        // and disable all subsequent calls to the 'setDefaultMetricsRegistrar'
        // method if the optionally-specified 'basicMetricsRegistrar' is 0;
        // return 'basicMetricsRegistrar' otherwise.

    static MetricsRegistrar *notMonitoredMetricsRegistrar();
        // Return the metrics registrar used by 'DefaultMetricsRegistrar' when
        // no registrar is set.

    static int setDefaultMetricsRegistrar(
                                      MetricsRegistrar *basicMetricsRegistrar);
        // Set the address of the default metrics registrar to the specified
        // 'basicMetricsRegistrar' unless calls to this method have been
        // disabled.  Return 0 on success and a non-zero value otherwise.  This
        // method will fail if 'defaultMetricsRegistrar' or 'metricsRegistrar'
        // with argument 0 has been called previously in this process.  The
        // behavior is undefined unless 'basicMetricsRegistrar' is the address
        // of an metrics registrar with sufficient lifetime to satisfy all
        // registration requests within this process.
};

                 // =======================================
                 // struct DefaultMetricsRegistrar_TestUtil
                 // =======================================

struct DefaultMetricsRegistrar_TestUtil {
    // This component-private utility 'struct' is a mechanism with class
    // methods to aid in testing 'DefaultMetricsRegistrar'.

    // CLASS METHODS
    static void reset();
        // Reset the default metric registrar's state to its initial value.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                      // ------------------------------
                      // struct DefaultMetricsRegistrar
                      // ------------------------------

// CLASS METHODS
inline
MetricsRegistrar *DefaultMetricsRegistrar::metricsRegistrar(
                                       MetricsRegistrar *basicMetricsRegistrar)
{
    return basicMetricsRegistrar ? basicMetricsRegistrar
                                 : defaultMetricsRegistrar();
}

                 // ---------------------------------------
                 // struct DefaultMetricsRegistrar_TestUtil
                 // ---------------------------------------

// CLASS METHODS
inline
void DefaultMetricsRegistrar_TestUtil::reset()
{
    bsls::AtomicOperations::setUint64Release(
                       &DefaultMetricsRegistrar::s_defaultMetricsRegistrar, 0);
}

}  // close package namespace
}  // close enterprise namespace

#endif

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
