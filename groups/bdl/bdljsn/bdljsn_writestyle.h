// bdljsn_writestyle.h                                                -*-C++-*-
#ifndef INCLUDED_BDLJSN_WRITESTYLE
#define INCLUDED_BDLJSN_WRITESTYLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Enumerate the formatting styles for a writing a JSON document.
//
//@CLASSES:
//  bdljsn::WriteStyle: namespace for styles for writing a JSON document
//
//@SEE_ALSO: bdljsn_writeoptions
//
//@DESCRIPTION: This component provides 'bdljsn::WriteStyle', a namespace for
// the 'enum' type 'bdljsn::WriteStyle::Enum', which enumerates the set of
// format styles that can be used when writing a JSON document.
//
///Enumerators
///-----------
//..
//  Name              Description
//  -------------     -------------------------------------------------------
//  e_PRETTY          A human friendly format with configurable new lines and
//                    indentation
//
//  e_ONELINE         A single-line format with a space after each comma and
//                    colon.
//
//  e_COMPACT         A maximally compact format with no white space.
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The following snippets of code provide a simple illustration of using
// 'bdljsn::WriteStyle'.
//
// First, we create a variable 'value' of type 'bdljsn::WriteStyle::Enum' and
// initialize it with the enumerator value 'bdljsn::WriteStyle::e_PRETTY':
//..
//  bdljsn::WriteStyle::Enum value = bdljsn::WriteStyle::e_PRETTY;
//..
// Now, we store the address of its ASCII representation in a pointer variable,
// 'asciiValue', of type 'const char *':
//..
//  const char *asciiValue = bdljsn::WriteStyle::toAscii(value);
//  assert(0 == bsl::strcmp(asciiValue, "PRETTY"));
//..
// Finally, we print 'value' to 'bsl::cout'.
//..
//  bsl::cout << value << bsl::endl;
//..
// This statement produces the following output on 'stdout':
//..
//  PRETTY
//..

#include <bdlscm_version.h>

#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace bdljsn {
                             // =================
                             // struct WriteStyle
                             // =================

struct WriteStyle {
    // This 'struct' provides a namespace for enumerating the formatting styles
    // that can be used for writing a JSON document.  See 'Enum' in the TYPES
    // sub-section for details.
    //
    // This class:
    //: o supports a complete set of *enumeration* operations
    //:   o except for 'bdex' serialization
    //: o is 'const' *thread-safe*
    // For terminology see 'bsldoc_glossary'.

  public:
    // TYPES
    enum Enum {
        e_PRETTY,   // human friendly, with indentation and spaces
        e_ONELINE,  // single-line format with whitespace for readability
        e_COMPACT   // maximally compact, no white-space
    };

  public:
    // CLASS METHODS
    static bsl::ostream& print(bsl::ostream&    stream,
                               WriteStyle::Enum value,
                               int              level          = 0,
                               int              spacesPerLevel = 4);
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
        // what constitutes the string representation of a 'WriteStyle::Enum'
        // value.

    static const char *toAscii(WriteStyle::Enum value);
        // Return the non-modifiable string representation corresponding to the
        // specified enumeration 'value', if it exists, and a unique (error)
        // string otherwise.  The string representation of 'value' matches its
        // corresponding enumerator name with the "e_" prefix elided.  For
        // example:
        //..
        //  bsl::cout << WriteStyle::toAscii(WriteStyle::e_PRETTY);
        //..
        // will print the following on standard output:
        //..
        //  PRETTY
        //..
        // Note that specifying a 'value' that does not match any of the
        // enumerators will result in a string representation that is distinct
        // from any of those corresponding to the enumerators, but is otherwise
        // unspecified.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, WriteStyle::Enum value);
    // Write the string representation of the specified enumeration 'value' to
    // the specified output 'stream' in a single-line format, and return a
    // reference to 'stream'.  See 'toAscii' for what constitutes the string
    // representation of a 'bdljsn::WriteStyle::Enum' value.  Note that this
    // method has the same behavior as
    //..
    //  bdljsn::WriteStyle::print(stream, value, 0, -1);
    //..

}  // close package namespace

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                             // -----------------
                             // struct WriteStyle
                             // -----------------

// FREE OPERATORS
inline
bsl::ostream& bdljsn::operator<<(bsl::ostream& stream, WriteStyle::Enum value)
{
    return WriteStyle::print(stream, value, 0, -1);
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
