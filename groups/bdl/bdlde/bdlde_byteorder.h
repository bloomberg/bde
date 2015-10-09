// bdlde_byteorder.h                                                  -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLDE_BYTEORDER
#define INCLUDED_BDLDE_BYTEORDER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an enumeration of the set of possible byte orders.
//
//@CLASSES:
//  bdlde::ByteOrder: namespace for a byte order 'enum'
//
//@SEE_ALSO: bdlde_charconvertutf16
//
//@DESCRIPTION: This component provides a namespace for the 'enum' type
// 'bdlde::ByteOrder::Enum', which enumerates the set of possible byte orders.
//
///Enumerators
///-----------
//..
//  Name              Description
//  ---------------   ---------------------------------------------------
//  e_LITTLE_ENDIAN   little-endian
//  e_BIG_ENDIAN      big-endian
//  e_NETWORK         network byte order (aliased to 'e_BIG_ENDIAN')
//  e_HOST            aliased to 'e_LITTLE_ENDIAN' or 'e_BIG_ENDIAN' (depending
//                    on platform)
//..
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The following snippets of code provide a simple illustration of
// 'bdlde::ByteOrder' usage.
//
// First, we create a variable 'value' of type 'bdlde::ByteOrder::Enum' and
// initialize it with the enumerator value
// 'bdlde::ByteOrder::e_LITTLE_ENDIAN':
//..
//  bdlde::ByteOrder::Enum value = bdlde::ByteOrder::e_LITTLE_ENDIAN;
//..
// Next, we store a pointer to its ASCII representation in a variable
// 'asciiValue' of type 'const char *':
//..
//  const char *asciiValue = bdlde::ByteOrder::toAscii(value);
//  assert(0 == bsl::strcmp(asciiValue, "LITTLE_ENDIAN"));
//..
// Then, we try one of the aliased identifiers, and we get a string
// corresponding to the value it is aliased to:
//..
//  value      = bdlde::ByteOrder::e_NETWORK;
//  asciiValue = bdlde::ByteOrder::toAscii(value);
//
//  assert(0 == bsl::strcmp(asciiValue, "BIG_ENDIAN"));
//..
// Finally, we print 'value' to 'bsl::cout':
//..
//  bsl::cout << value << bsl::endl;
//..
// This statement produces the following output on 'stdout':
//..
//  BIG_ENDIAN
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

namespace bdlde {
                              // ================
                              // struct ByteOrder
                              // ================

struct ByteOrder {
    // This 'struct' provides a namespace for enumerating the set of byte
    // orders.  See 'Enum' in the TYPES sub-section for details.
    //
    // This class:
    //: o supports a complete set of *enumeration* operations
    //:   o except for 'bdex' serialization
    // For terminology see 'bsldoc_glossary'.

  public:
    // TYPES
    enum Enum {
        e_LITTLE_ENDIAN,                // base enumeration
        e_BIG_ENDIAN,                   // base enumeration

        e_NETWORK = e_BIG_ENDIAN,       // Network byte order is always
                                        // aliased to big-endian.

#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
        e_HOST    = e_LITTLE_ENDIAN     // Host byte order is always aliased to
#else                                   // a base enumeration, varying
        e_HOST    = e_BIG_ENDIAN        // depending on the platform.
#endif
    };

  public:
    // CLASS METHODS
    static bsl::ostream& print(bsl::ostream& stream,
                               Enum          value,
                               int           level          = 0,
                               int           spacesPerLevel = 4);
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
        // what constitutes the string representation of a
        // 'ByteOrder::Enum' value.

    static const char *toAscii(Enum value);
        // Return the non-modifiable string representation corresponding to the
        // specified enumeration 'value', if it exists, and a unique (error)
        // string otherwise.  The string representation of 'value' matches the
        // name of its corresponding base enumeration with the "e_" prefix
        // elided.  For example:
        //..
        //  bsl::cout << ByteOrder::toAscii(ByteOrder::e_NETWORK);
        //..
        // will print the following on standard output:
        //..
        //  BIG_ENDIAN
        //..
        // Note that specifying a 'value' that does not match any of the
        // enumerators will result in a string representation that is distinct
        // from any of those corresponding to the enumerators, but is otherwise
        // unspecified.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, ByteOrder::Enum value);
}  // close package namespace
    // Write the string representation of the specified enumeration 'value' to
    // the specified output 'stream' in a single-line format, and return a
    // reference to 'stream'.  See 'toAscii' for what constitutes the string
    // representation of a 'bdlde::ByteOrder::Enum' value.  Note that this
    // method has the same behavior as
    //..
    //  bdlde::ByteOrder::print(stream, value, 0, -1);
    //..

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                          // -----------------------
                          // struct bdlde::ByteOrder
                          // -----------------------

// FREE OPERATORS
inline
bsl::ostream& bdlde::operator<<(bsl::ostream& stream, ByteOrder::Enum value)
{
    return ByteOrder::print(stream, value, 0, -1);
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
