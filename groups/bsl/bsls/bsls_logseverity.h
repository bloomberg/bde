// bsls_logseverity.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLS_LOGSEVERITY
#define INCLUDED_BSLS_LOGSEVERITY

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Enumerate a set of logging severity levels.
//
//@CLASSES:
//  bsls::LogSeverity: namespace for enumerating logging severity levels
//
//@SEE_ALSO: bsls_log
//
//@DESCRIPTION: This component provides a namespace for the 'enum' type
// 'bsls::LogSeverity::Enum', which enumerates a set of severity levels used
// for logging with 'bsls_log'.
//
///Enumerators
///-----------
//..
//  Name          Description
//  -----------   -------------------------------------------------------------
//  e_FATAL       A severity appropriate for log messages accompanying a fatal
//                event (i.e., one that will cause a *crash*).
//
//  e_ERROR       A severity appropriate for log messages accompanying an
//                unexpected error (i.e., one that will cause incorrect
//                behavior).
//
//  e_WARN        A severity appropriate for log messages accompanying an
//                event that may indicate a problem.
//
//  e_INFO        A severity appropriate for log messages providing informative
//                status about the running process.
//
//  e_DEBUG       A severity appropriate for log messages providing information
//                useful for debugging.
//
//  e_TRACE       A severity appropriate for log messages providing detailed
//                trace information.
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The following snippets of code provide a simple illustration of using
// 'bsls::LogSeverity'.
//
// First, we create a variable 'value' of type 'bsls::LogSeverity::Enum'
// and initialize it with the enumerator value
// 'bsls::LogSeverity::e_DEBUG':
//..
//  bsls::LogSeverity::Enum value = bsls::LogSeverity::e_DEBUG;
//..
// Now, we store the address of its ASCII representation in a pointer variable,
// 'asciiValue', of type 'const char *':
//..
//  const char *asciiValue = bsls::LogSeverity::toAscii(value);
//..
// Finally, we verify the value of 'asciiValue':
//..
//  assert(0 == strcmp(asciiValue, "DEBUG"));
//..

namespace BloombergLP {
namespace bsls {

                         // ==================
                         // struct LogSeverity
                         // ==================

struct LogSeverity {
    // This 'struct' provides a namespace for enumerating the set of logging
    // severity levels used in the 'bsls' logging framework (see 'bsls_log').
    // See 'Enum' in the TYPES sub-section for details.

  public:
    // TYPES
    enum Enum {
        // Enumeration of logging severity levels.

        e_FATAL = 0,  // a condition that will (likely) cause a *crash*
        e_ERROR = 1,  // a condition that *will* cause incorrect behavior
        e_WARN  = 2,  // a *potentially* problematic condition
        e_INFO  = 3,  // data about the running process
        e_DEBUG = 4,  // information useful while debugging
        e_TRACE = 5   // execution trace data
    };

  public:
    static const char *toAscii(LogSeverity::Enum value);
        // Return the non-modifiable string representation corresponding to the
        // specified enumeration 'value', if it exists, and a unique (error)
        // string otherwise.  The string representation of 'value' matches its
        // corresponding enumerator name with the "e_" prefix elided.  For
        // example:
        //..
        //  bsl::cout << LogSeverity::toAscii(LogSeverity::e_DEBUG);
        //..
        // will print the following on standard output:
        //..
        //  DEBUG
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
// Copyright 2016 Bloomberg Finance L.P.
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
