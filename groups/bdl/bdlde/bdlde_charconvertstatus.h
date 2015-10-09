// bdlde_charconvertstatus.h                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLDE_CHARCONVERTSTATUS
#define INCLUDED_BDLDE_CHARCONVERTSTATUS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide masks for interpreting status from charconvert functions.
//
//@CLASSES:
//  bdlde::CharConvertStatus: namespace for bit-wise mask of charconvert status
//
//@SEE_ALSO: bdlde_charconvertutf16, bdlde_charconvertucs2
//
//@DESCRIPTION: This component provides a namespace for the 'enum' type
// 'bdlde::CharConvertStatus::Enum', which enumerates the set of bit-wise masks
// that can be used to interpret return values from translation functions in
// components 'bdlde_charconvertutf16' and 'bdlde_charconvertucs2'.
//
///Enumerators
///-----------
//..
//  Name                      Description
//  -------------------   ---------------------------------------------
//  e_INVALID_CHARS_BIT   Invalid characters or sequences of characters
//                        were encountered in the input.
//  e_OUT_OF_SPACE_BIT    The space provided for the output was
//                        insufficient for the translation.
//..
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The following snippets of code provide a simple illustration of
// 'bdlde::CharConvertStatus' usage.
//
// First, we create a variable 'value' of type 'bdlde::CharConvertStatus::Enum'
// and initialize it with the value 3, which is not a valid value of the
// 'enum'.
//..
//      bdlde::CharConvertStatus::Enum value =
//                               bdlde::CharConvertStatus::e_INVALID_CHARS_BIT;
//..
// Next, we store a pointer to its ASCII representation in a variable
// 'asciiValue' of type 'const char *':
//..
//      const char *asciiValue = bdlde::CharConvertStatus::toAscii(value);
//      assert(0 == bsl::strcmp(asciiValue, "INVALID_CHARS_BIT"));
//..
// Finally, we print 'value' to 'bsl::cout'.
//..
//      if (veryVerbose) {
//          bsl::cout << value << bsl::endl;
//      }
//..
// This statement produces the following output on 'stdout':
//..
// INVALID_CHARS_BIT
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {
namespace bdlde {

                          // ========================
                          // struct CharConvertStatus
                          // ========================

struct CharConvertStatus {
    // This 'struct' provides a namespace for enumerating the set of mask
    // codes that can be used to interpret 'int' return values from translation
    // functions in 'bdede'.
    //
    // This class:
    //: o supports a complete set of *enumeration* operations
    //:   o except for 'bdex' serialization
    //: o is 'const' *thread-safe*
    // For terminology see 'bsldoc_glossary'.

  public:
    // TYPES
    enum Enum {
        k_INVALID_CHARS_BIT = 0x1,      // Invalid characters or sequences of
                                        // characters were encountered in the
                                        // input.
        k_OUT_OF_SPACE_BIT  = 0x2       // The space provided for the output
                                        // was insufficient for the
                                        // translation.

    };

  public:
    // CLASS METHODS
    static bsl::ostream& print(bsl::ostream&           stream,
                               CharConvertStatus::Enum value,
                               int                     level          = 0,
                               int                     spacesPerLevel = 4);
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
        // 'CharConvertStatus::Enum' value.

    static const char *toAscii(CharConvertStatus::Enum value);
        // Return the non-modifiable string representation corresponding to the
        // specified enumeration 'value', if it exists, and a unique (error)
        // string otherwise.  The string representation of 'value' matches its
        // corresponding enumerator name with the "k_" prefix elided.  For
        // example:
        //..
        //  bsl::cout << CharConvertStatus::toAscii(
        //                    CharConvertStatus::k_OUT_OF_SPACE_BIT);
        //..
        // will print the following on standard output:
        //..
        //  OUT_OF_SPACE_BIT
        //..
        // Note that specifying a 'value' that does not match any of the
        // enumerators will result in a string representation that is distinct
        // from any of those corresponding to the enumerators, but is otherwise
        // unspecified.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&           stream,
                         CharConvertStatus::Enum value);
}  // close package namespace
    // Write the string representation of the specified enumeration 'value' to
    // the specified output 'stream' in a single-line format, and return a
    // reference to 'stream'.  See 'toAscii' for what constitutes the string
    // representation of a 'bdlde::CharConvertStatus::Enum' value.  Note that
    // this method has the same behavior as
    //..
    //  bdlde::CharConvertStatus::print(stream, value, 0, -1);
    //..

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                      // -------------------------------
                      // struct bdlde::CharConvertStatus
                      // -------------------------------

// FREE OPERATORS
inline
bsl::ostream& bdlde::operator<<(bsl::ostream&           stream,
                                CharConvertStatus::Enum value)
{
    return CharConvertStatus::print(stream, value, 0, -1);
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
