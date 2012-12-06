// bdede_charconvertstatus.h                                          -*-C++-*-
#ifndef INCLUDED_BDEDE_CHARCONVERTSTATUS
#define INCLUDED_BDEDE_CHARCONVERTSTATUS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Enumerate masks for interpreting status from charconvert functions.
//
//@CLASSES:
//  bdede_CharConvertStatus: namespace for bit-wise mask of charconvert status
//
//@SEE_ALSO: bdede_charconvertutf16, bdede_charconvertucs2
//
//@AUTHOR: Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides a namespace for the 'enum' type
// 'bdede_CharConvertStatus::Enum', which enumerates the set of bit-wise masks
// that can be used to interpret return values from translation functions in
// components 'bdede_charconvertutf16' and 'bdede_charconvertucs2'.
//
///Enumerators
///-----------
//..
//  Name                      Description
//  ----------------------    ----------------------------------------
//  BDEDE_INVALID_CHARS_MASK  Invalid characters or sequences of characters
//                            were encountered in the input.
//  BDEDE_OUT_OF_SPACE_MASK   The space provided for the output was
//                            insufficient for the translation.
//..
//
///Usage
///-----
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

                     // ==============================
                     // struct bdede_CharConvertStatus
                     // ==============================

struct bdede_CharConvertStatus {
    // This 'struct' provides a namespace for enumerating the set of mask
    // codes that can be used to interpret 'int' return values from translation
    // functions in BDEDE.
    //
    // This class:
    //: o supports a complete set of *enumeration* operations
    //:   o except for 'bdex' serialization
    //: o is 'const' *thread-safe*
    // For terminology see 'bsldoc_glossary'.

  public:
    // TYPES
    enum Enum {
        BDEDE_INVALID_CHARS_BIT = 0x1,      // Invalid characters or sequences
                                            // of characters were encountered
                                            // in the input.
        BDEDE_OUT_OF_SPACE_BIT  = 0x2       // The space provided for the
                                            // output was insufficient for the
                                            // translation.
    };

  public:
    // CLASS METHODS
    static bsl::ostream& print(
                            bsl::ostream&                 stream,
                            bdede_CharConvertStatus::Enum value,
                            int                           level          = 0,
                            int                           spacesPerLevel = 4);
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
        // 'baetzo_LocalTimeValidity::Enum' value.

    static const char *toAscii(bdede_CharConvertStatus::Enum value);
        // Return the non-modifiable string representation corresponding to the
        // specified enumeration 'value', if it exists, and a unique (error)
        // string otherwise.  The string representation of 'value' matches its
        // corresponding enumerator name with the "BDEDE_" prefix elided.  For
        // example:
        //..
        //  bsl::cout << baetzo_LocalTimeValidity::toAscii(
        //                    bdede_CharConvertStatus::BDEDE_OUT_OF_SPACE_BIT);
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
bsl::ostream& operator<<(bsl::ostream&                 stream,
                         bdede_CharConvertStatus::Enum value);
    // Write the string representation of the specified enumeration 'value' to
    // the specified output 'stream' in a single-line format, and return a
    // reference to 'stream'.  See 'toAscii' for what constitutes the string
    // representation of a 'bdede_CharConvertStatus::Enum' value.  Note that
    // this method has the same behavior as
    //..
    //  bdede_CharConvertStatus::print(stream, value, 0, -1);
    //..

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                     // ------------------------------
                     // struct bdede_CharConvertStatus
                     // ------------------------------

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream&                 stream,
                         bdede_CharConvertStatus::Enum value)
{
    return bdede_CharConvertStatus::print(stream, value, 0, -1);
}

}  // close namespace BloombergLP

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
