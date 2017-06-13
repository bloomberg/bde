// balb_assertiontrackersingleton.h                                   -*-C++-*-

#ifndef INCLUDED_BALB_ASSERTIONTRACKERSINGLETON
#define INCLUDED_BALB_ASSERTIONTRACKERSINGLETON

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif

//@PURPOSE: Provide a means to install an assertion tracker singleton.
//
//@CLASSES:
//   balb::AssertionTrackerSingleton: manage an assertion tracker singleton
//
//@DESCRIPTION: The 'balb::AssertionTrackerSingleton' component creates and
// provides access to a singleton object that is responsible for accumulating
// and reporting on a sequence of failed assertions.
//
///Thread Safety
///-------------
// This component is thread-safe and thread-enabled: it is safe to access and
// manipulate multiple distinct instances from different threads, and it is
// safe to access and manipulate a single shared instance from different
// threads.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1:
/// - - - - -

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMT_ONCE
#include <bslmt_once.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {
namespace balb {

                        // ===============================
                        // class AssertionTrackerSingleton
                        // ===============================

template <class TRACKER>
class AssertionTrackerSingleton {
  public:
    // PUBLIC CLASS METHODS
    static void failTracker(const char *text, const char *file, int line);
        // Forward the specified 'text', 'file', and 'line' corresponding to a
        // failed assertion to the singleton object managed by this class via
        // function-call operator.  The behavior is undefined unless the
        // lifetime of the pointers 'text' and 'file' exceeds that of the
        // singleton.  (In normal usage those pointers will be string literals,
        // trivially satisfying this.)  This function is intended to be
        // installed as the assertion-handler function for 'bsls::Assert'.
        // Note that unlike proper handlers, this handler returns to its caller
        // and may trigger warnings on such behavior within 'bsls::Assert'.

    static TRACKER *singleton(bslma::Allocator *basicAllocator = 0);
        // Return a pointer to the 'TRACKER' singleton object that will be used
        // to track assertion failures.  When the singleton is created,
        // 'failTracker' will be installed as the assertion-handler function
        // for 'bsls::Assert'.  If 'failTracker' could not be installed, return
        // a null pointer instead.  Optionally specify a 'basicAllocator' used
        // to supply memory.  The currently installed failure handler and this
        // allocator are supplied to the constructor of the singleton.
};

                        // -------------------------------
                        // class AssertionTrackerSingleton
                        // -------------------------------

// CLASS METHODS
template <class TRACKER>
void AssertionTrackerSingleton<TRACKER>::failTracker(const char *text,
                                                     const char *file,
                                                     int         line)
{
    (*singleton())(text, file, line);
}

template <class TRACKER>
TRACKER *
AssertionTrackerSingleton<TRACKER>::singleton(bslma::Allocator *basicAllocator)
{
    static TRACKER *theSingleton_p;

    BSLMT_ONCE_DO
    {
        bsls::Assert::setFailureHandler(&failTracker);
        if (bsls::Assert::failureHandler() == &failTracker) {
            static TRACKER theSingleton(bsls::Assert::failureHandler(),
                                        basicAllocator);
            theSingleton_p = &theSingleton;
        }
    }

    return theSingleton_p;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
