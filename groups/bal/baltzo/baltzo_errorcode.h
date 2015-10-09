// baltzo_errorcode.h                                                 -*-C++-*-
#ifndef INCLUDED_BALTZO_ERRORCODE
#define INCLUDED_BALTZO_ERRORCODE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Enumerate the set of named errors for the 'baltzo' package.
//
//@CLASSES:
//  baltzo::ErrorCode: namespace for error status 'enum'
//
//@SEE_ALSO: baltzo_localtimedescriptor, baltzo_timezoneutil
//
//@DESCRIPTION: This component provides a namespace, 'baltzo::ErrorCode', for
// the 'enum' type 'baltzo::ErrorCode::Enum', which enumerates the set of named
// error codes returned by functions across the 'baltzo' package.
//
///Enumerators
///-----------
//..
//  Name               Description
//  ----------------   --------------------------------------
//  k_UNSUPPORTED_ID   Time zone identifier is not supported.
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The following snippets of code provide a simple illustration of
// 'baltzo::ErrorCode' usage.
//
// First, we create a variable 'value' of type 'baltzo::ErrorCode::Enum' and
// initialize it with the enumerator value
// 'baltzo::ErrorCode::k_UNSUPPORTED_ID':
//..
//  baltzo::ErrorCode::Enum value = baltzo::ErrorCode::k_UNSUPPORTED_ID;
//..
// Now, we store a pointer to its ASCII representation in a variable
// 'asciiValue' of type 'const char *':
//..
//  const char *asciiValue = baltzo::ErrorCode::toAscii(value);
//  assert(0 == bsl::strcmp(asciiValue, "UNSUPPORTED_ID"));
//..
// Finally, we print 'value' to 'bsl::cout'.
//..
//  bsl::cout << value << bsl::endl;
//..
// This statement produces the following output on 'stdout':
//..
//  UNSUPPORTED_ID
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {
namespace baltzo {
                              // ================
                              // struct ErrorCode
                              // ================

struct ErrorCode {
    // This 'struct' provides a namespace for enumerating the set of named
    // error codes that may be returned by functions in the 'baltzo' package
    // that take a Zoneinfo string identifier as input.  See 'Enum' in the
    // TYPES sub-section for details.
    //
    // This class:
    //: o supports a complete set of *enumeration* operations
    //:   o except for 'bdex' serialization
    //: o is 'const' *thread-safe*
    // For terminology see 'bsldoc_glossary'.

  public:
    // TYPES
    enum Enum {
        k_UNSUPPORTED_ID = 1  // Time zone identifier is not supported.


    };

  public:
    // CLASS METHODS
    static bsl::ostream& print(bsl::ostream&   stream,
                               ErrorCode::Enum value,
                               int             level          = 0,
                               int             spacesPerLevel = 4);
        // Write the string representation of the specified enumeration 'value'
        // to the specified output 'stream', and return a reference to
        // 'stream'.  Optionally specify an initial indentation 'level', whose
        // absolute value is incremented recursively for nested objects.  If
        // 'level' is specified, optionally specify 'spacesPerLevel', whose
        // absolute value indicates the number of spaces per indentation level
        // for this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  See 'toAscii' for
        // what constitutes the string representation of a 'ErrorCode::Enum'
        // value.

    static const char *toAscii(ErrorCode::Enum value);
        // Return the non-modifiable string representation corresponding to the
        // specified enumeration 'value', if it exists, and a unique (error)
        // string otherwise.  The string representation of 'value' matches its
        // corresponding enumerator name with the "k_" prefix elided.  For
        // example:
        //..
        //  bsl::cout << ErrorCode::toAscii(ErrorCode::k_UNSUPPORTED_ID);
        //..
        // will print the following on standard output:
        //..
        //  UNSUPPORTED_ID
        //..
        // Note that specifying a 'value' that does not match any of the
        // enumerators will result in a string representation that is distinct
        // from any of those corresponding to the enumerators, but is otherwise
        // unspecified.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, ErrorCode::Enum value);
    // Write the string representation of the specified enumeration 'value' to
    // the specified output 'stream' in a single-line format, and return a
    // reference to 'stream'.  See 'toAscii' for what constitutes the string
    // representation of a 'baltzo::ErrorCode::Enum' value.  Note that this
    // method has the same behavior as
    //..
    //  baltzo::ErrorCode::print(stream, value, 0, -1);
    //..

}  // close package namespace

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                              // ----------------
                              // struct ErrorCode
                              // ----------------

// FREE OPERATORS
inline
bsl::ostream& baltzo::operator<<(bsl::ostream& stream, ErrorCode::Enum value)
{
    return ErrorCode::print(stream, value, 0, -1);
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
