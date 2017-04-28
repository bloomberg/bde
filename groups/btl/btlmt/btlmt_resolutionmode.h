// btlmt_resolutionmode.h                                             -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLMT_RESOLUTIONMODE
#define INCLUDED_BTLMT_RESOLUTIONMODE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Enumerate the set of channel closing modes.
//
//@CLASSES:
//   btlmt::ResolutionMode: namespace for channel closing modes
//
//@DESCRIPTION: This component provides a namespace, 'btlmt::ResolutionMode',
// for the 'enum' type 'btlmt::ResolutionMode::Enum', which enumerates the set
// of modes for resolving an DNS hostname to an IP address.
//
///Enumerators
///-----------
//..
//  Name                       Description
//  ----------------------     ------------------
//  e_RESOLVE_ONCE             Resolve host name once
//  e_RESOLVE_EACH_TIME        Resolve host name on each connection attempt
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The following snippets of code provide a simple illustration of
// 'btlmt::ResolutionMode' usage.
//
// First, we create a variable 'value' of type
// 'btlmt::ResolutionMode::Enum' and initialize it with the enumerator value
// 'btlmt::ResolutionMode::e_RESOLVE_ONCE':
//..
//  btlmt::ResolutionMode::Enum value = btlmt::ResolutionMode::e_RESOLVE_ONCE;
//..
// Now, we store a pointer to its ASCII representation in a variable
// 'asciiValue' of type 'const char *':
//..
//  const char *asciiValue = btlmt::ResolutionMode::toAscii(value);
//  assert(0 == bsl::strcmp(asciiValue, "RESOLVE_ONCE"));
//..
// Finally, we print 'value' to 'bsl::cout'.
//..
//  bsl::cout << value << bsl::endl;
//..
// This statement produces the following output on 'stdout':
//..
//  RESOLVE_ONCE
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {
namespace btlmt {

                        // =====================
                        // struct ResolutionMode
                        // =====================

struct ResolutionMode {
    // This 'struct' provides a namespace for enumerating channel close modes
    // that can be attributed to local time values.  See 'Enum' in the TYPES
    // sub-section for details.
    //
    // This class:
    //: o supports a complete set of *enumeration* operations
    //:   o except for 'bdex' serialization
    // For terminology see 'bsldoc_glossary'.

  public:
    // TYPES
    enum Enum {
        e_RESOLVE_ONCE = 0,
        e_RESOLVE_EACH_TIME
    };

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
        // 'ResolutionMode::Enum' value.

    static const char *toAscii(Enum value);
        // Return the non-modifiable string representation corresponding to the
        // specified enumeration 'value', if it exists, and a unique (error)
        // string otherwise.  The string representation of 'value' matches its
        // corresponding enumerator name with the "e_" prefix elided.  For
        // example:
        //..
        //  bsl::cout << ResolutionMode::toAscii(
        //                                     ResolutionMode::e_RESOLVE_ONCE);
        //..
        // will print the following on standard output:
        //..
        //  RESOLVE_ONCE
        //..
        // Note that specifying a 'value' that does not match any of the
        // enumerators will result in a string representation that is distinct
        // from any of those corresponding to the enumerators, but is otherwise
        // unspecified.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, ResolutionMode::Enum value);
    // Write the string representation of the specified enumeration 'value' to
    // the specified output 'stream' in a single-line format, and return a
    // reference to 'stream'.  See 'toAscii' for what constitutes the string
    // representation of a 'btlmt::ResolutionMode::Enum' value.  Note that this
    // method has the same behavior as
    //..
    //  btlmt::ResolutionMode::print(stream, value, 0, -1);
    //..

}  // close package namespace

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                          // ---------------------
                          // struct ResolutionMode
                          // ---------------------

// FREE OPERATORS
inline
bsl::ostream& btlmt::operator<<(bsl::ostream&        stream,
                                ResolutionMode::Enum value)
{
    return ResolutionMode::print(stream, value, 0, -1);
}

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
