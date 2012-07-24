// bcem_errorcode.h                                                   -*-C++-*-
#ifndef INCLUDED_BCEM_ERRORCODE
#define INCLUDED_BCEM_ERRORCODE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Enumerate set of errors associated with accessing 'bcem' types. 
//
//@CLASSES:
//  bcem_ErrorCode: namespace for errors associated with 'bcem' types
//
//@SEE_ALSO: bcem_aggregateerror, bcem_aggregate
//
//@AUTHOR: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION: This component provides a namespace for the 'enum' type
// 'bcem_ErrorCode::Code', which enumerates the set of error codes
// that can happen while accessing 'bcem' types.
//
///Enumerators
///-----------
//..
//  Name                    Description
//  ----------------------  ---------------------------------------
//  BCEM_SUCCESS            No error (numerical value 0)
//
//  BCEM_NOT_A_RECORD       Attempt to access a field (by name, ID, or
//                          index) of an aggregate that does not reference a
//                          list, row, choice, or choice array item.
//
//  BCEM_NOT_A_SEQUENCE     Attempt to perform a list or row operation on an
//                          aggregate that does not refer to a list or row
//                          (e.g., initialize from a non-sequence record
//                          definition).
//
//  BCEM_NOT_A_CHOICE       Attempt to perform a choice or choice array item
//                          operation (make selection, get selection, etc.)
//                          on an aggregate that is not a choice or choice
//                          array item.
//
//  BCEM_NOT_AN_ARRAY       Attempt to perform an array operation (index,
//                          insert, etc.) on an aggregate that is not an
//                          array, table, or choice array.
//
//  BCEM_BAD_FIELDNAME      Field name does not exist in the record def.
//
//  BCEM_BAD_FIELDID        Field ID does not exist in record def.
//
//  BCEM_BAD_FIELDINDEX     Field index is not a positive integer less than
//                          the length of the field definition.
//
//  BCEM_BAD_ARRAYINDEX     Array (or table) index is out of bounds.
//
//  BCEM_NOT_SELECTED       Attempt to access a choice field that is not
//                          the currently selected object.
//
//  BCEM_BAD_CONVERSION     Attempt to set an aggregate using a value that
//                          is not convertible to the aggregate's type.  (See
//                          "Extended Type Conversions" below.)
//
//  BCEM_BAD_ENUMVALUE      Attempt to set the value of an enumeration
//                          aggregate to a string that is not an enumerator
//                          name in the enumeration definition or to an
//                          integer that is not an enumerator ID in the
//                          enumeration definition.
//
//  BCEM_NON_CONFORMANT     Attempt to set a list, row, table, choice,
//                          choice array item, or choice array aggregate to
//                          a value of the correct type, but which does not
//                          conform to the aggregate's record definition.
//
//  BCEM_AMBIGUOUS_ANON     A reference to an anonymous field is ambiguous,
//                          typically because the aggregate contains more
//                          than one anonymous field.
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The following snippets of code provide a simple illustration of
// 'bcem_ErrorCode' usage.
//
// First, we create a variable 'value' of type 'bcem_ErrorCode::Code' and
// initialize it with the enumerator value
// 'bcem_ErrorCode::BCEM_BAD_FIELDNAME':
//..
//  bcem_ErrorCode::Code value = bcem_ErrorCode::BCEM_BAD_FIELDNAME;
//..
// Now, we store a pointer to its ASCII representation in a variable
// 'asciiValue' of type 'const char *':
//..
//  const char *asciiValue = bcem_ErrorCode::toAscii(value);
//  assert(0 == bsl::strcmp(asciiValue, "BAD_FIELDNAME"));
//..
// Finally, we print 'value' to 'bsl::cout'.
//..
//  bsl::cout << value << bsl::endl;
//..
// This statement produces the following output on 'stdout':
//..
//  BAD_FIELDNAME
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BSL_CLIMITS
#include <bsl_climits.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

                     // =====================
                     // struct bcem_ErrorCode
                     // =====================

struct bcem_ErrorCode {
    // This 'struct' provides a namespace for enumerating the set of error
    // codes that can happen when accessing 'bcem' types.  See 'Code' in the
    // TYPES sub-section for details.
    //
    // This class:
    //: o supports a complete set of *enumeration* operations
    //:   o except for 'bdex' serialization
    //: o is 'const' *thread-safe*
    // For terminology see 'bsldoc_glossary'.

  public:
    // TYPES
    enum Code {
        BCEM_SUCCESS = 0,
        BCEM_UNKNOWN_ERROR = INT_MIN + 1,
        BCEM_NOT_A_RECORD,    // Aggregate is not a sequence or choice.
        BCEM_NOT_A_SEQUENCE,  // Aggregate is not a sequence.
        BCEM_NOT_A_CHOICE,    // Aggregate is not a choice.
        BCEM_NOT_AN_ARRAY,    // Aggregate is not an array.
        BCEM_BAD_FIELDNAME,   // Field name does not exist in record.
        BCEM_BAD_FIELDID,     // Field ID does not exist in record.
        BCEM_BAD_FIELDINDEX,  // Field index does not exist in record.
        BCEM_BAD_ARRAYINDEX,  // Array index is out of bounds.
        BCEM_NOT_SELECTED,    // Field does not match current selection.
        BCEM_BAD_CONVERSION,  // Cannot convert value.
        BCEM_BAD_ENUMVALUE,   // Invalid enumerator value
        BCEM_NON_CONFORMANT,  // Value does not conform to the schema.
        BCEM_AMBIGUOUS_ANON   // Anonymous field reference is ambiguous.
    };

  public:
    // CLASS METHODS
    static bsl::ostream& print(bsl::ostream&        stream,
                               bcem_ErrorCode::Code value,
                               int                  level          = 0,
                               int                  spacesPerLevel = 4);
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
        // 'bcem_ErrorCode::Enum' value.

    static const char *toAscii(bcem_ErrorCode::Code value);
        // Return the non-modifiable string representation corresponding to the
        // specified enumeration 'value', if it exists, and a unique (error)
        // string otherwise.  The string representation of 'value' matches its
        // corresponding enumerator name with the "BCEM_" prefix elided.  For
        // example:
        //..
        //  bsl::cout << bcem_ErrorCode::toAscii(
        //                                bcem_ErrorCode::BCEM_BAD_ARRAYINDEX);
        //..
        // will print the following on standard output:
        //..
        //  BAD_ARRAYINDEX
        //..
        // Note that specifying a 'value' that does not match any of the
        // enumerators will result in a string representation that is distinct
        // from any of those corresponding to the enumerators, but is otherwise
        // unspecified.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, bcem_ErrorCode::Code value);
    // Write the string representation of the specified enumeration 'value' to
    // the specified output 'stream' in a single-line format, and return a
    // reference to 'stream'.  See 'toAscii' for what constitutes the string
    // representation of a 'bcem_ErrorCode::Code' value.  Note that
    // this method has the same behavior as
    //..
    //  bcem_ErrorCode::print(stream, value, 0, -1);
    //..

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                     // ---------------------
                     // struct bcem_ErrorCode
                     // ---------------------

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream& stream, bcem_ErrorCode::Code value)
{
    return bcem_ErrorCode::print(stream, value, 0, -1);
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
