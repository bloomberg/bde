// baltzo_dstpolicy.h                                                 -*-C++-*-
#ifndef INCLUDED_BALTZO_DSTPOLICY
#define INCLUDED_BALTZO_DSTPOLICY

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Enumerate the set of daylight-saving time (DST) policy values.
//
//@CLASSES:
//  baltzo::DstPolicy: namespace for a daylight-saving time policy 'enum'
//
//@SEE_ALSO: baltzo_localtimevalidity, baltzo_timezoneutil
//
//@DESCRIPTION: This component provides a namespace for the 'enum' type
// 'baltzo::DstPolicy::Enum', which enumerates the set of policies for
// interpreting whether an associated local time is a daylight-saving time
// value.  A 'baltzo::DstPolicy' is particularly important when interpreting a
// local time that is not associated with a UTC offset (e.g., a
// 'bdlt::Datetime' object), as such a representation may be ambiguous or
// invalid (see 'baltzo_localtimevalidity').
//
///Enumerators
///-----------
//..
//  Name            Description
//  -------------   ---------------------------------------------------
//  e_DST           Local time is interpreted as daylight-saving time.
//
//  e_STANDARD      Local time is interpreted as standard time.
//
//  e_UNSPECIFIED   Local time is interpreted as either daylight-saving time or
//                  standard time (as appropriate).
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The following snippets of code provide a simple illustration of using
// 'baltzo::DstPolicy'.
//
// First, we create a variable 'value' of type 'baltzo::DstPolicy::Enum' and
// initialize it with the enumerator value
// 'baltzo::DstPolicy::e_STANDARD':
//..
//  baltzo::DstPolicy::Enum value = baltzo::DstPolicy::e_STANDARD;
//..
// Now, we store the address of its ASCII representation in a pointer variable,
// 'asciiValue', of type 'const char *':
//..
//  const char *asciiValue = baltzo::DstPolicy::toAscii(value);
//  assert(0 == bsl::strcmp(asciiValue, "STANDARD"));
//..
// Finally, we print 'value' to 'bsl::cout'.
//..
//  bsl::cout << value << bsl::endl;
//..
// This statement produces the following output on 'stdout':
//..
//  STANDARD
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
                              // struct DstPolicy
                              // ================

struct DstPolicy {
    // This 'struct' provides a namespace for enumerating the set of policies
    // for interpreting whether a local time is a daylight-saving time.  See
    // 'Enum' in the TYPES sub-section for details.
    //
    // This class:
    //: o supports a complete set of *enumeration* operations
    //:   o except for 'bdex' serialization
    //: o is 'const' *thread-safe*
    // For terminology see 'bsldoc_glossary'.

  public:
    // TYPES
    enum Enum {
        e_DST,         // Local time is interpreted as daylight-saving
                            // time.

        e_STANDARD,    // Local time is interpreted as standard time.

        e_UNSPECIFIED  // Local time is interpreted as either
                            // daylight-saving time or standard time (as
                            // appropriate).


    };

  public:
    // CLASS METHODS
    static bsl::ostream& print(bsl::ostream&   stream,
                               DstPolicy::Enum value,
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
        // what constitutes the string representation of a 'DstPolicy::Enum'
        // value.

    static const char *toAscii(DstPolicy::Enum value);
        // Return the non-modifiable string representation corresponding to the
        // specified enumeration 'value', if it exists, and a unique (error)
        // string otherwise.  The string representation of 'value' matches its
        // corresponding enumerator name with the "e_" prefix elided.  For
        // example:
        //..
        //  bsl::cout << DstPolicy::toAscii(DstPolicy::e_STANDARD);
        //..
        // will print the following on standard output:
        //..
        //  STANDARD
        //..
        // Note that specifying a 'value' that does not match any of the
        // enumerators will result in a string representation that is distinct
        // from any of those corresponding to the enumerators, but is otherwise
        // unspecified.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, DstPolicy::Enum value);
    // Write the string representation of the specified enumeration 'value' to
    // the specified output 'stream' in a single-line format, and return a
    // reference to 'stream'.  See 'toAscii' for what constitutes the string
    // representation of a 'baltzo::DstPolicy::Enum' value.  Note that this
    // method has the same behavior as
    //..
    //  baltzo::DstPolicy::print(stream, value, 0, -1);
    //..

}  // close package namespace

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                              // ----------------
                              // struct DstPolicy
                              // ----------------

// FREE OPERATORS
inline
bsl::ostream& baltzo::operator<<(bsl::ostream& stream, DstPolicy::Enum value)
{
    return DstPolicy::print(stream, value, 0, -1);
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
