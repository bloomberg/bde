// btlsc_flag.h                                                       -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLSC_FLAG
#define INCLUDED_BTLSC_FLAG

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Enumerate all flags for stream-based-channel transport.
//
//@CLASSES:
//  btlsc::Flag: namespace for enumerating all stream-based-channel flags
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides a namespace, 'btlsc::Flag', for
// enumerating all flags of use to the various stream-based channel and
// channel-allocator components of this package.  Functionality is provided to
// convert each of these enumerated values to its corresponding string
// representation, and to write its string form directly to a standard
// 'bsl::ostream'.
//
///Enumerators
///-----------
//..
//  Name                   Description
//  -----------------      ----------------------------------------------
//  k_ASYNC_INTERRUPT      Permit interruption be an asynchronous event.
//  k_RAW                  Permit suspension of a transmission operation.
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The following snippets of code provide a simple illustration of
// 'btlsc::Flag' operation.
//
// First, create a variable, 'flag', of type 'btlsc::Flag::Enum', and
// initialize it to the value 'btlsc::Flag::k_ASYNC_INTERRUPT':
//..
//  btlsc::Flag::Enum flag = btlsc::Flag::k_ASYNC_INTERRUPT;
//..
// Next, store its representation in a variable, 'rep', of type 'const char *':
//..
//  const char *rep = btlsc::Flag::toAscii(flag);
//  assert(0 == bsl::strcmp(rep, "ASYNC_INTERRUPT"));
//..
// Finally, we print the value of 'flag' to 'stdout':
//..
//  bsl::cout << flag << bsl::endl;
//..
// This statement produces the following output on 'stdout':
//..
//  ASYNC_INTERRUPT
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {
namespace btlsc {

                              // ==========
                              // class Flag
                              // ==========

struct Flag {
    // This class provides a namespace for enumerating all flags for the
    // 'btlsc' package.

    // TYPES
    enum Enum {
        k_ASYNC_INTERRUPT = 0x01  // If set, this flag permits an operation to
                                  // be interrupted by an unspecified
                                  // asynchronous event.  By default, the
                                  // implementation will ignore such events if
                                  // possible, or fail otherwise.

      , k_RAW             = 0x02  // If set, this flag permits a transmission
                                  // method to suspend itself between OS-level
                                  // atomic operations provided (1) at least
                                  // one additional byte was transmitted and
                                  // (2) no additional bytes can be transmitted
                                  // immediately -- e.g., without (potentially)
                                  // blocking.  By default, the implementation
                                  // will continue until it either succeeds,
                                  // fails, or returns a partial result for
                                  // some other, authorized reason.
    };

    enum {
        k_LENGTH = 2 // Define LENGTH to be the number of enumerators; they are
                     // flags and *not* consecutive, and so the length must be
                     // kept consistent with the 'enum' "by hand".
    };

    // CLASS METHODS
    static const char *toAscii(Enum value);
        // Return the abbreviated character-string representation of the
        // enumerator corresponding to the specified 'value'.  This
        // representation matches the enumerator's character name (e.g.,
        // 'k_RAW') with the "k_" prefix elided.  For example:
        //..
        //  bsl::cout << btlsc::Flag::toAscii(btlsc::Flag::k_RAW);
        //..
        // prints the following on standard output:
        //..
        //  RAW
        //..
        // Note that specifying a 'value' that does not match any of the
        // enumerators will result in an unspecified string representation
        // being returned that is distinct from the values returned for any
        // valid enumeration.

};

// FREE FUNCTIONS
bsl::ostream& operator<<(bsl::ostream& stream, Flag::Enum value);
    // Write the string representation of the specified enumeration 'value' to
    // the specified output 'stream' in a single-line format, and return a
    // reference to 'stream'.  See 'toAscii' for what constitutes the string
    // representation of a 'btlsc::Flag::Enum' value.

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================


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
