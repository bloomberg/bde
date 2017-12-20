// bsls_systemclocktype.h                                             -*-C++-*-
#ifndef INCLUDED_BSLS_SYSTEMCLOCKTYPE
#define INCLUDED_BSLS_SYSTEMCLOCKTYPE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Enumerate the set of system clock types.
//
//@CLASSES:
//  bsls::SystemClockType: namespace for a clock type 'enum'
//
//@DESCRIPTION: This component provides a namespace for the 'enum' type
// 'bsls::SystemClockType::Enum', which enumerates the set of system clock
// types.  A 'bsls::SystemClockType' is particularly important when providing
// time-out values to synchronization methods where those time-outs must be
// consistent in environments where the system clocks may be changed.
//
///Enumerators
///-----------
//..
//  Name          Description
//  -----------   -------------------------------------------------------------
//  e_REALTIME    System clock that returns the current wall time maintained by
//                the system, and which is affected by adjustments to the
//                system's clock (i.e., time values returned may jump
//                "forwards" and "backwards" as the current time-of-day is
//                changed on the system).
//
//  e_MONOTONIC   System clock that returns the elapsed time since some
//                unspecified starting point in the past.  The returned values
//                are monotonically non-decreasing, and are generally not
//                affected by changes to the system time.
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The following snippets of code provide a simple illustration of using
// 'bsls::SystemClockType'.
//
// First, we create a variable 'value' of type 'bsls::SystemClockType::Enum'
// and initialize it with the enumerator value
// 'bsls::SystemClockType::e_MONOTONIC':
//..
//  bsls::SystemClockType::Enum value = bsls::SystemClockType::e_MONOTONIC;
//..
// Now, we store the address of its ASCII representation in a pointer variable,
// 'asciiValue', of type 'const char *':
//..
//  const char *asciiValue = bsls::SystemClockType::toAscii(value);
//..
// Finally, we verify the value of 'asciiValue':
//..
//  assert(0 == strcmp(asciiValue, "MONOTONIC"));
//..

namespace BloombergLP {
namespace bsls {

                         // ======================
                         // struct SystemClockType
                         // ======================

struct SystemClockType {
    // This 'struct' provides a namespace for enumerating the set of system
    // clock type for use in distinguishing which system clock to use for
    // measuring time.  See 'Enum' in the TYPES sub-section for details.

  public:
    // TYPES
    enum Enum {
        // Enumeration clock type values.

        e_REALTIME,   // System clock that returns the current wall time
                      // maintained by the system, and which is affected by
                      // adjustments to the system's clock (i.e., time values
                      // returned may jump "forwards" and "backwards" as the
                      // current time-of-day is changed on the system).

        e_MONOTONIC   // System clock that returns the elapsed time since some
                      // unspecified starting point in the past.  The returned
                      // values are monotonically non-decreasing, and are
                      // generally not affected by changes to the system time.
    };

  public:
    static const char *toAscii(SystemClockType::Enum value);
        // Return the non-modifiable string representation corresponding to the
        // specified enumeration 'value', if it exists, and a unique (error)
        // string otherwise.  The string representation of 'value' matches its
        // corresponding enumerator name with the "e_" prefix elided.  For
        // example:
        //..
        //  bsl::cout << SystemClockType::toAscii(SystemClockType::e_REALTIME);
        //..
        // will print the following on standard output:
        //..
        //  REALTIME
        //..
        // Note that specifying a 'value' that does not match any of the
        // enumerators will result in a string representation that is distinct
        // from any of those corresponding to the enumerators, but is otherwise
        // unspecified.
};

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
