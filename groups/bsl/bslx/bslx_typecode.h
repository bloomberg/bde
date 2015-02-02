// bslx_typecode.h                                                    -*-C++-*-
#ifndef INCLUDED_BSLX_TYPECODE
#define INCLUDED_BSLX_TYPECODE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Enumerate the fundamental types supported by BDEX.
//
//@CLASSES:
//  bslx::TypeCode: namespace for enumerating BDEX-supported fundamental types
//
//@DESCRIPTION: This component provides a namespace for the 'enum' type
// 'bslx::TypeCode::Enum', which enumerates the set of fundamental types
// supported within 'bslx'.  A 'bslx::TypeCode' is particularly important when
// testing 'bslx' implementations (see 'bslx::TestOutStream' and
// 'bslx::TestInStream').
//
///Enumerators
///-----------
//..
//  Name                 Description
//  ------------------   ---------------------------------------------------
//  e_INT8               Value is an 8-bit integer.
//
//  e_UINT8              Value is an unsigned 8-bit integer.
//
//  e_INT16              Value is a 16-bit integer.
//
//  e_UINT16             Value is an unsigned 16-bit integer.
//
//  e_INT24              Value is a 24-bit integer.
//
//  e_UINT24             Value is an unsigned 24-bit integer.
//
//  e_INT32              Value is a 32-bit integer.
//
//  e_UINT32             Value is an unsigned 32-bit integer.
//
//  e_INT40              Value is a 40-bit integer.
//
//  e_UINT40             Value is an unsigned 40-bit integer.
//
//  e_INT48              Value is a 48-bit integer.
//
//  e_UINT48             Value is an unsigned 48-bit integer.
//
//  e_INT56              Value is a 56-bit integer.
//
//  e_UINT56             Value is an unsigned 56-bit integer.
//
//  e_INT64              Value is a 64-bit integer.
//
//  e_UINT64             Value is an unsigned 64-bit integer.
//
//  e_FLOAT32            Value is a 32-bit floating-point number.
//
//  e_FLOAT64            Value is a 64-bit floating-point number.
//
//  e_INVALID            Value is not valid.
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The following snippets of code provide a simple illustration of using
// 'bslx::TypeCode'.
//
// First, we create a variable 'value' of type 'bslx::TypeCode::Enum' and
// initialize it with the enumerator value 'bslx::TypeCode::e_INT32':
//..
//  bslx::TypeCode::Enum value = bslx::TypeCode::e_INT32;
//..
// Now, we store the address of its ASCII representation in a pointer variable,
// 'asciiValue', of type 'const char *':
//..
//  const char *asciiValue = bslx::TypeCode::toAscii(value);
//  assert(0 == bsl::strcmp(asciiValue, "INT32"));
//..
// Finally, we print 'value' to 'bsl::cout':
//..
//  bsl::cout << value << bsl::endl;
//..
// This statement produces the following output on 'stdout':
//..
//  INT32
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {
namespace bslx {

                         // ===============
                         // struct TypeCode
                         // ===============

struct TypeCode {
    // This 'struct' provides a namespace for enumerating the set of
    // fundamental types supported within 'bslx'.  See 'Enum' in the 'TYPES'
    // sub-section for details.

  private:
    // PRIVATE TYPES
    enum { k_OFFSET = 0xe0 };  // Value to offset each enumerator in 'Enum'.

  public:
    // TYPES
    enum Enum {
        e_INT8    = k_OFFSET +  0,  // Value is an 8-bit integer.

        e_UINT8   = k_OFFSET +  1,  // Value is an 8-bit unsigned integer.

        e_INT16   = k_OFFSET +  2,  // Value is a 16-bit integer.

        e_UINT16  = k_OFFSET +  3,  // Value is a 16-bit unsigned integer.

        e_INT24   = k_OFFSET +  4,  // Value is a 24-bit integer.

        e_UINT24  = k_OFFSET +  5,  // Value is a 24-bit unsigned integer.

        e_INT32   = k_OFFSET +  6,  // Value is a 32-bit integer.

        e_UINT32  = k_OFFSET +  7,  // Value is a 32-bit unsigned integer.

        e_INT40   = k_OFFSET +  8,  // Value is a 40-bit integer.

        e_UINT40  = k_OFFSET +  9,  // Value is a 40-bit unsigned integer.

        e_INT48   = k_OFFSET + 10,  // Value is a 48-bit integer.

        e_UINT48  = k_OFFSET + 11,  // Value is a 48-bit unsigned integer.

        e_INT56   = k_OFFSET + 12,  // Value is a 56-bit integer.

        e_UINT56  = k_OFFSET + 13,  // Value is a 56-bit unsigned integer.

        e_INT64   = k_OFFSET + 14,  // Value is a 64-bit integer.

        e_UINT64  = k_OFFSET + 15,  // Value is a 64-bit unsigned integer.

        e_FLOAT32 = k_OFFSET + 16,  // Value is a 32-bit floating-point number.

        e_FLOAT64 = k_OFFSET + 17,  // Value is a 64-bit floating-point number.

        e_INVALID = k_OFFSET + 18   // Value is not valid.
    };

  public:
    // CLASS METHODS
    static bsl::ostream& print(bsl::ostream&  stream,
                               TypeCode::Enum value,
                               int            level          = 0,
                               int            spacesPerLevel = 4);
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
        // what constitutes the string representation of a 'TypeCode::Enum'
        // value.

    static const char *toAscii(TypeCode::Enum value);
        // Return the non-modifiable string representation corresponding to the
        // specified enumeration 'value', if it exists, and a unique (error)
        // string otherwise.  The string representation of 'value' matches its
        // corresponding enumerator name with the "e_" prefix elided.  For
        // example:
        //..
        //  bsl::cout << TypeCode::toAscii(TypeCode::e_INT32);
        //..
        // will print the following on standard output:
        //..
        //  INT32
        //..
        // Note that specifying a 'value' that does not match any of the
        // enumerators will result in a string representation that is distinct
        // from any of those corresponding to the enumerators, but is otherwise
        // unspecified.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, TypeCode::Enum value);
    // Write the string representation of the specified enumeration 'value' to
    // the specified output 'stream' in a single-line format, and return a
    // reference to 'stream'.  See 'toAscii' for what constitutes the string
    // representation of a 'TypeCode::Enum' value.  Note that this method has
    // the same behavior as
    //..
    //  TypeCode::print(stream, value, 0, -1);
    //..

// ============================================================================
//                        INLINE DEFINITIONS
// ============================================================================

                        // ---------------
                        // struct TypeCode
                        // ---------------

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream& stream, TypeCode::Enum value)
{
    return TypeCode::print(stream, value, 0, -1);
}

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
