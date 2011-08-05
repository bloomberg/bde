// baetzo_errorcode.h                                                 -*-C++-*-
#ifndef INCLUDED_BAETZO_ERRORCODE
#define INCLUDED_BAETZO_ERRORCODE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Enumerate the set of named errors for the 'baetzo' package.
//
//@CLASSES:
//  baetzo_ErrorCode: namespace for error status 'enum'
//
//@SEE_ALSO: baetzo_localtimedescriptor, baetzo_timezoneutil
//
//@AUTHOR: Stefano Pacifico (spacifico1)
//
//@DESCRIPTION: This component provides a namespace for the 'enum' type
// 'baetzo_ErrorCode::Enum', which enumerates the set of named error codes
// returned by functions across the 'baetzo' package.
//
///Enumerators
///-----------
//..
//  Name                    Description
//  ---------------------   --------------------------------------
//  BAETZO_UNSUPPORTED_ID   Time zone identifier is not supported.
//..
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The following snippets of code provide a simple illustration of
// 'baetzo_ErrorCode' usage.
//
// First, we create a variable 'value' of type 'baetzo_ErrorCode::Enum' and
// initialize it with the enumerator value
// 'baetzo_ErrorCode::BAETZO_UNSUPPORTED_ID':
//..
//  baetzo_ErrorCode::Enum value = baetzo_ErrorCode::BAETZO_UNSUPPORTED_ID;
//..
// Now, we store a pointer to its ASCII representation in a variable
// 'asciiValue' of type 'const char *':
//..
//  const char *asciiValue = baetzo_ErrorCode::toAscii(value);
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

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

                          // =======================
                          // struct baetzo_ErrorCode
                          // =======================

struct baetzo_ErrorCode {
    // This 'struct' provides a namespace for enumerating the set of named
    // error codes that may be returned by functions in the 'baetzo' package
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
        BAETZO_UNSUPPORTED_ID = 1  // Time zone identifier is not supported.
    };

  public:
    // CLASS METHODS
    static bsl::ostream& print(bsl::ostream&          stream,
                               baetzo_ErrorCode::Enum value,
                               int                    level          = 0,
                               int                    spacesPerLevel = 4);
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
        // 'baetzo_ErrorCode::Enum' value.

    static const char *toAscii(baetzo_ErrorCode::Enum value);
        // Return the non-modifiable string representation corresponding to the
        // specified enumeration 'value', if it exists, and a unique (error)
        // string otherwise.  The string representation of 'value' matches its
        // corresponding enumerator name with the "BAETZO_" prefix elided.  For
        // example:
        //..
        //  bsl::cout << baetzo_ErrorCode::toAscii(
        //                            baetzo_ErrorCode::BAETZO_UNSUPPORTED_ID);
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
bsl::ostream& operator<<(bsl::ostream& stream, baetzo_ErrorCode::Enum value);
    // Write the string representation of the specified enumeration 'value' to
    // the specified output 'stream' in a single-line format, and return a
    // reference to 'stream'.  See 'toAscii' for what constitutes the string
    // representation of a 'baetzo_ErrorCode::Enum' value.  Note that this
    // method has the same behavior as
    //..
    //  baetzo_ErrorCode::print(stream, value, 0, -1);
    //..

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                          // -----------------------
                          // struct baetzo_ErrorCode
                          // -----------------------

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream& stream, baetzo_ErrorCode::Enum value)
{
    return baetzo_ErrorCode::print(stream, value, 0, -1);
}

}  // close namespace BloombergLP

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
