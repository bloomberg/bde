// baltzo_localtimevalidity.h                                         -*-C++-*-
#ifndef INCLUDED_BALTZO_LOCALTIMEVALIDITY
#define INCLUDED_BALTZO_LOCALTIMEVALIDITY

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Enumerate the set of local time validity codes.
//
//@CLASSES:
//  baltzo::LocalTimeValidity: namespace for local time validity 'enum'
//
//@SEE_ALSO: baltzo_localtimedescriptor, baltzo_timezoneutil
//
//@DESCRIPTION: This component provides a namespace,
// 'baltzo::LocalTimeValidity', for the 'enum' type
// 'baltzo::LocalTimeValidity::Enum', which enumerates the set of validity
// codes that can be attributed to a local time.  Due to the vagaries of time
// zones, and transitions among daylight-saving time and standard time, a
// particular representation of local clock time may be deemed to be strictly
// invalid, or (uniquely or ambiguously) valid.
//
///Enumerators
///-----------
//..
//  Name               Description
//  -----------------  ---------------------------------------
//  e_VALID_UNIQUE     Local time is *valid* and *unique*.
//  e_VALID_AMBIGUOUS  Local time is *valid*, but *ambiguous*.
//  e_INVALID          Local time is *invalid*.
//..
// For example, consider the following purported New York local times and their
// corresponding 'baltzo::LocalTimeValidity::Enum' values:
//..
//  New York Local Time     Validity Code
//  -------------------     ----------------------
//  Jan  1, 2010 2:30am     e_VALID_UNIQUE
//  Mar 14, 2010 2:30am     e_INVALID
//  Nov  7, 2010 1:30am     e_VALID_AMBIGUOUS
//..
// On January 1, 2010 in New York, Eastern Standard Time (EST) was in effect
// and the transition to Eastern Daylight-Saving Time (EDT) was still weeks
// away; "Jan 1, 2010 2:30am" is a patently valid -- and unique -- New York
// local time.  The transition to EDT in New York in 2010 occurred on March 14
// as of 2:00am EST, or more precisely, as of 7:00am UTC (which would have been
// 2:00am EST, but became 3:00am EDT).  Consequently, "Mar 14, 2010 2:30am" is
// an invalid New York local time, since clocks were advanced by one hour as of
// 2:00am EST.  The change from EDT back to EST in New York in 2010 occurred on
// November 7 as of 2:00am EDT.  Due to this transition, "Nov 7, 2010 1:30am"
// is a valid New York local time.  However, that local time is ambiguous
// because it corresponds to two possible clock times, 1:30am EDT and 1:30am
// EST, since clocks were regressed by one hour as of 2:00am EDT (7:00am UTC).
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The following snippets of code provide a simple illustration of
// 'baltzo::LocalTimeValidity' usage.
//
// First, we create a variable 'value' of type
// 'baltzo::LocalTimeValidity::Enum' and initialize it with the enumerator
// value 'baltzo::LocalTimeValidity::e_VALID_AMBIGUOUS':
//..
//  baltzo::LocalTimeValidity::Enum value =
//                                baltzo::LocalTimeValidity::e_VALID_AMBIGUOUS;
//..
// Now, we store a pointer to its ASCII representation in a variable
// 'asciiValue' of type 'const char *':
//..
//  const char *asciiValue = baltzo::LocalTimeValidity::toAscii(value);
//  assert(0 == bsl::strcmp(asciiValue, "VALID_AMBIGUOUS"));
//..
// Finally, we print 'value' to 'bsl::cout'.
//..
//  bsl::cout << value << bsl::endl;
//..
// This statement produces the following output on 'stdout':
//..
//  VALID_AMBIGUOUS
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {
namespace baltzo {
                          // ========================
                          // struct LocalTimeValidity
                          // ========================

struct LocalTimeValidity {
    // This 'struct' provides a namespace for enumerating the set of validity
    // codes that can be attributed to local time values.  See 'Enum' in the
    // TYPES sub-section for details.
    //
    // This class:
    //: o supports a complete set of *enumeration* operations
    //:   o except for 'bdex' serialization
    // For terminology see 'bsldoc_glossary'.

  public:
    // TYPES
    enum Enum {
        e_VALID_UNIQUE,     // Local time is *valid* and *unique*.
        e_VALID_AMBIGUOUS,  // Local time is *valid*, but *ambiguous*.
        e_INVALID           // Local time is *invalid*.


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
        // 'LocalTimeValidity::Enum' value.

    static const char *toAscii(Enum value);
        // Return the non-modifiable string representation corresponding to the
        // specified enumeration 'value', if it exists, and a unique (error)
        // string otherwise.  The string representation of 'value' matches its
        // corresponding enumerator name with the "e_" prefix elided.  For
        // example:
        //..
        //  bsl::cout << LocalTimeValidity::toAscii(
        //                                  LocalTimeValidity::e_VALID_UNIQUE);
        //..
        // will print the following on standard output:
        //..
        //  VALID_UNIQUE
        //..
        // Note that specifying a 'value' that does not match any of the
        // enumerators will result in a string representation that is distinct
        // from any of those corresponding to the enumerators, but is otherwise
        // unspecified.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, LocalTimeValidity::Enum value);
    // Write the string representation of the specified enumeration 'value' to
    // the specified output 'stream' in a single-line format, and return a
    // reference to 'stream'.  See 'toAscii' for what constitutes the string
    // representation of a 'baltzo::LocalTimeValidity::Enum' value.  Note that
    // this method has the same behavior as
    //..
    //  baltzo::LocalTimeValidity::print(stream, value, 0, -1);
    //..

}  // close package namespace

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                          // ------------------------
                          // struct LocalTimeValidity
                          // ------------------------

// FREE OPERATORS
inline
bsl::ostream& baltzo::operator<<(bsl::ostream&           stream,
                                 LocalTimeValidity::Enum value)
{
    return LocalTimeValidity::print(stream, value, 0, -1);
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
