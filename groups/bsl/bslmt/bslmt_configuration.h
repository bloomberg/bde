// bslmt_configuration.h                                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BSLMT_CONFIGURATION
#define INCLUDED_BSLMT_CONFIGURATION

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide utilities to allow configuration of values for BCE.
//
//@CLASSES:
//  bslmt::Configuration: namespace for utilities managing default BCE values
//
//@SEE_ALSO: bslmt_threadattributes, bslmt_threadutil
//
//@DESCRIPTION: This component defines a utility 'struct',
// 'bslmt::Configuration', that is a name space for pure functions used for
// providing access to, and configuring, default values for BCE-relevant
// parameters.  The 'bslmt::Configuration' utility currently provides static
// methods to access and modify the BCE library's default stack size, as well
// as functions that access the platform's native default stack size and guard
// size.  The BCE default stack size is initially configured to
// 'bslmt::ThreadAttributes::BSLMT_UNSET_STACK_SIZE', in which case thread
// creation is to use the native default thread stack size.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Demonstrate Accessing & Modifying the Default Thread Stack Size
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we demonstrate how to access both the platform's native and
// BCE configured default stack sizes, and then to set the default stack size
// used by BCE.  Note that the value returned by 'defaultThreadStackSize' may
// be adjusted from that provided by the underlying operating system to reflect
// the actual amount of stack memory available to a created thread.  For
// example, on Itanium platforms (HPUX) the value will be scaled down from the
// operating system supplied value to account for the extra stack space devoted
// to storing registers.  Note that operations creating a thread should perform
// a similar inverse adjustment when configuring the new thread's stack size
// (see 'bslmt_threadutil').
//
// First, we examine the platform's native thread stack size:
//..
//  const int nativeDefault =
//                        bslmt::Configuration::nativeDefaultThreadStackSize();
//
//  assert(nativeDefault > 0);
//..
// Then, we verify that 'defaultThreadStackSize' is unset.
//..
//  assert(bslmt::ThreadAttributes::e_UNSET_STACK_SIZE ==
//                             bslmt::Configuration::defaultThreadStackSize());
//..
// Next, we define 'newDefaultStackSize' to some size other than the platform's
// native default stack size:
//..
//  const int newDefaultStackSize = nativeDefault * 2;
//..
// Now, we set the default size for BCE to the new size:
//..
//  bslmt::Configuration::setDefaultThreadStackSize(newDefaultStackSize);
//..
// Finally, we verify that BCE's default thread stack size has been set to the
// value we specified:
//..
//  assert(bslmt::Configuration::defaultThreadStackSize() ==
//                                                        newDefaultStackSize);
//  assert(bslmt::Configuration::defaultThreadStackSize() != nativeDefault);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

namespace BloombergLP {
namespace bslmt {

                           // ====================
                           // struct Configuration
                           // ====================

struct Configuration {
    // This 'struct' provides a namespace for a suite of functions that are
    // used to manage the configuration of default values used in 'bce'.
    // Specifically, these functions manage the default value of thread stack
    // size and provide access to the platform's native guard size, but may be
    // extended to govern more traits in the future.

    // CLASS METHODS
    static int defaultThreadStackSize();
        // Return the value set by the last call to
        // 'setDefaultThreadStackSize'; if 'setDefaultThreadStackSize' has
        // never been called, return 'ThreadAttributes::BSLMT_UNSET_STACK_SIZE'
        // which will signal thread creation to use the thread stack size
        // native to the platform.

    static int nativeDefaultThreadStackSize();
        // Return the "native" default thread stack size (in bytes) as
        // determined by the underlying platform.  Note that this value may be
        // influenced by the choice of platform, environment variables,
        // compiler/linker options, or shell configuration, and typically
        // varies wildly among different platforms.

    static int nativeDefaultThreadGuardSize();
        // Return the default thread stack guard size (in bytes) determined by
        // the underlying platform.  Note that this value reflects semantics,
        // and may be influenced by the choice of platform, environment
        // variables, compiler/linker options, or shell configuration, and may
        // vary somewhat among different platforms.

    static int recommendedDefaultThreadStackSize();
        // Return a "reasonable" value for the default thread stack size (in
        // bytes), which, unlike 'nativeDefaultThreadStackSize', is constant
        // across all platforms of a given word size.  This value is large
        // enough to guarantee that an automatic array of at least 250 * 1024
        // pointers may be declared in the top level routine of the thread.

    static void setDefaultThreadStackSize(int numBytes);
        // Set the default thread stack size to the specified 'numBytes'.  If a
        // minimum thread stack size is known for the underlying platform (i.e.
        // 'PTHREAD_STACK_MIN' is defined) and 'numBytes' is below that
        // minimum, the stack size will be that minimum.  The behavior is
        // undefined unless '0 <= numBytes'.
};

}  // close package namespace
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
