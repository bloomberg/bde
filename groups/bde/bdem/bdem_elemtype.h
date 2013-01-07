// bdem_elemtype.h                                                    -*-C++-*-
#ifndef INCLUDED_BDEM_ELEMTYPE
#define INCLUDED_BDEM_ELEMTYPE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an enumeration of the supported 'bdem' element types.
//
//@CLASSES:
//  bdem_ElemType: namespace for enumerating the set of 'bdem' element types
//
//@SEE_ALSO: bdem_aggregate
//
//@AUTHOR: John Lakos (jlakos)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION: This component provides a namespace for the enumerated type
// 'bdem_ElemType::Type'.  'Type' enumerates the set of scalar, array, and
// aggregate data "element" types used throughout the 'bdem' package.  This
// enumeration also includes a small number of values that represent 'bdem'
// "pseudo-types", which are distinguished by having negative values.
//
// The following table summarizes the enumerators provided, categorized by
// "scalar", "array", "aggregate", "choice", or "pseudo".  Note that some
// enumerators belong to more than one category:
//..
//  +=============================================+
//  | CATEGORY  | ENUMERATOR                      |
//  +=============================================+
//  | SCALAR    | BDEM_BOOL                       |
//  |           | BDEM_CHAR                       |
//  |           | BDEM_SHORT                      |
//  |           | BDEM_INT                        |
//  |           | BDEM_INT64                      |
//  |           | BDEM_FLOAT                      |
//  |           | BDEM_DOUBLE                     |
//  |           | BDEM_STRING                     |
//  |           | BDEM_DATETIME                   |
//  |           | BDEM_DATETIMETZ                 |
//  |           | BDEM_DATE                       |
//  |           | BDEM_DATETZ                     |
//  |           | BDEM_TIME                       |
//  |           | BDEM_TIMETZ                     |
//  +-----------+---------------------------------+
//  | ARRAY     | BDEM_BOOL_ARRAY                 |
//  |           | BDEM_CHAR_ARRAY                 |
//  |           | BDEM_SHORT_ARRAY                |
//  |           | BDEM_INT_ARRAY                  |
//  |           | BDEM_INT64_ARRAY                |
//  |           | BDEM_FLOAT_ARRAY                |
//  |           | BDEM_DOUBLE_ARRAY               |
//  |           | BDEM_STRING_ARRAY               |
//  |           | BDEM_DATETIME_ARRAY             |
//  |           | BDEM_DATETIMETZ_ARRAY           |
//  |           | BDEM_DATE_ARRAY                 |
//  |           | BDEM_DATETZ_ARRAY               |
//  |           | BDEM_TIME_ARRAY                 |
//  |           | BDEM_TIMETZ_ARRAY               |
//  |           | BDEM_TABLE                      |
//  |           | BDEM_CHOICE_ARRAY               |
//  +-----------+---------------------------------+
//  | AGGREGATE | BDEM_LIST                       |
//  |           | BDEM_TABLE                      |
//  |           | BDEM_CHOICE                     |
//  |           | BDEM_CHOICE_ARRAY               |
//  |           | BDEM_CHOICE_ARRAY_ITEM (pseudo) |
//  |           | BDEM_ROW               (pseudo) |
//  +-----------+---------------------------------+
//  | CHOICE    | BDEM_CHOICE                     |
//  |           | BDEM_CHOICE_ARRAY               |
//  |           | BDEM_CHOICE_ARRAY_ITEM (pseudo) |
//  +-----------+---------------------------------+
//  | PSEUDO    | BDEM_VOID                       |
//  |           | BDEM_ROW                        |
//  |           | BDEM_CHOICE_ARRAY_ITEM          |
//  +=============================================+
//..
// Templatized functions are provided to support the streaming in and out of
// the enumerated values.  Input streams must be compatible with the
// 'bdex_InStream' protocol, and output streams must be compatible with the
// 'bdex_OutStream' protocol or else convertible to a standard 'ostream'; in
// the latter case the value is written as its corresponding string
// representation.  In addition, this component supports functions that convert
// the 'Type' enumerations to a well-defined ASCII representation.
//
///Usage
///-----
// The following snippets of code provide a simple illustration of
// 'bdem_ElemType' operation.
//
// First, create a variable 'elemType' of type 'bdem_ElemType::Type' and
// initialize it to the value 'bdem_ElemType::BDEM_DATETIME':
//..
//  bdem_ElemType::Type elemType = bdem_ElemType::BDEM_DATETIME;
//..
// Next, store its string representation in a variable named 'rep' of type
// 'const char *':
//..
//  const char *rep = bdem_ElemType::toAscii(bdem_ElemType::BDEM_DATETIME);
//  assert(0 == bsl::strcmp(rep, "DATETIME"));
//..
// Finally, print the string representation of 'elemType' to 'bsl::cout':
//..
//  bsl::cout << elemType;
//..
// This statement produces the following output on 'stdout':
//..
//  DATETIME
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif


namespace BloombergLP {

                        // ====================
                        // struct bdem_ElemType
                        // ====================

struct bdem_ElemType {
    // This 'struct' provides a namespace for enumerating the set of scalar,
    // array, and aggregate data "element" types used throughout the 'bdem'
    // package.

    // TYPES
    enum Type {
        BDEM_CHAR              =  0,
        BDEM_SHORT             =  1,
        BDEM_INT               =  2,
        BDEM_INT64             =  3,
        BDEM_FLOAT             =  4,
        BDEM_DOUBLE            =  5,
        BDEM_STRING            =  6,
        BDEM_DATETIME          =  7,
        BDEM_DATE              =  8,
        BDEM_TIME              =  9,
        BDEM_CHAR_ARRAY        = 10,
        BDEM_SHORT_ARRAY       = 11,
        BDEM_INT_ARRAY         = 12,
        BDEM_INT64_ARRAY       = 13,
        BDEM_FLOAT_ARRAY       = 14,
        BDEM_DOUBLE_ARRAY      = 15,
        BDEM_STRING_ARRAY      = 16,
        BDEM_DATETIME_ARRAY    = 17,
        BDEM_DATE_ARRAY        = 18,
        BDEM_TIME_ARRAY        = 19,
        BDEM_LIST              = 20,
        BDEM_TABLE             = 21,

        BDEM_BOOL              = 22,
        BDEM_DATETIMETZ        = 23,
        BDEM_DATETZ            = 24,
        BDEM_TIMETZ            = 25,
        BDEM_BOOL_ARRAY        = 26,
        BDEM_DATETIMETZ_ARRAY  = 27,
        BDEM_DATETZ_ARRAY      = 28,
        BDEM_TIMETZ_ARRAY      = 29,
        BDEM_CHOICE            = 30,
        BDEM_CHOICE_ARRAY      = 31,

        BDEM_VOID              = -1,  // Pseudo-types have negative values.
        BDEM_ROW               = -2,
        BDEM_CHOICE_ARRAY_ITEM = -3
    };

    enum {
        BDEM_NUM_TYPES = BDEM_CHOICE_ARRAY + 1,  // number of real (non-pseudo)
                                                 // types

        BDEM_NUM_PSEUDO_TYPES = 3                // number of pseudo types
    };

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-level documentation for more
        // information on 'bdex' streaming of container types.)

    static bdem_ElemType::Type toArrayType(bdem_ElemType::Type value);
        // Return the array 'bdem' element type corresponding to the specified
        // 'value'; return 'bdem_ElemType::BDEM_VOID' if 'value' has no
        // corresponding array type.  The two aggregate types that are also
        // array types ('BDEM_TABLE' and 'BDEM_CHOICE_ARRAY') are mapped as
        // follows:
        //..
        //  'value' parameter         return value
        //  ----------------------    -----------------
        //  BDEM_LIST                 BDEM_TABLE
        //  BDEM_ROW                  BDEM_TABLE
        //  BDEM_CHOICE               BDEM_CHOICE_ARRAY
        //  BDEM_CHOICE_ARRAY_ITEM    BDEM_CHOICE_ARRAY
        //..

    static bdem_ElemType::Type fromArrayType(bdem_ElemType::Type value);
        // Return the base 'bdem' element type corresponding to the specified
        // 'value'; return 'bdem_ElemType::BDEM_VOID' if 'value' has no
        // corresponding base type.  The two aggregate types that are also
        // array types ('BDEM_TABLE' and 'BDEM_CHOICE_ARRAY') are mapped as
        // follows:
        //..
        //  'value' parameter    return value
        //  -----------------    ------------
        //  BDEM_TABLE           BDEM_LIST
        //  BDEM_CHOICE_ARRAY    BDEM_CHOICE
        //..

    static bool isArrayType(bdem_ElemType::Type value);
        // Return 'true' if the specified 'value' is a 'bdem' array type, and
        // 'false' otherwise.  (See the component-level documentation for the
        // list of array 'bdem' types.)

    static bool isAggregateType(bdem_ElemType::Type value);
        // Return 'true' if the specified 'value' is a 'bdem' aggregate type,
        // and 'false' otherwise.  (See the component-level documentation for
        // the list of aggregate 'bdem' types.)

    static bool isChoiceType(bdem_ElemType::Type value);
        // Return 'true' if the specified 'value' is a 'bdem' choice type, and
        // 'false' otherwise.  (See the component-level documentation for the
        // list of choice 'bdem' types.)

    static bool isScalarType(bdem_ElemType::Type value);
        // Return 'true' if the specified 'value' is a 'bdem' scalar type, and
        // 'false' otherwise.  (See the component-level documentation for the
        // list of scalar 'bdem' types.)

    static void print(bsl::ostream& stream, bdem_ElemType::Type value);
        // Write to the specified 'stream' the string representation of the
        // specified enumerator 'value'.  The string representation of 'value'
        // matches its corresponding enumerator name with the "BDEM_" prefix
        // elided.  For example:
        //..
        //  print(bsl::cout, bdem_ElemType::BDEM_DATETIME);
        //..
        // will print the following on standard output:
        //..
        //  DATETIME
        //..

    static const char *toAscii(bdem_ElemType::Type value);
        // Return the string representation of the specified enumerator
        // 'value'.  The string representation of 'value' matches its
        // corresponding enumerator name with the "BDEM_" prefix elided.  For
        // example:
        //..
        //  print(bsl::cout, toAscii(bdem_ElemType::BDEM_DATETIME));
        //..
        // will print the following on standard output:
        //..
        //  DATETIME
        //..

    template <class STREAM>
    static STREAM& bdexStreamIn(STREAM&              stream,
                                bdem_ElemType::Type& value,
                                int                  version);
        // Assign to the specified 'value' the value read from the specified
        // input 'stream' using the specified 'version' format and return a
        // reference to the modifiable 'stream'.  If 'stream' is initially
        // invalid, this operation has no effect.  If 'stream' becomes invalid
        // during this operation, the 'value' is valid, but its value is
        // undefined.  If the specified 'version' is not supported, 'stream' is
        // marked invalid, but 'value' is unaltered.  Note that no version is
        // read from 'stream'.  (See the package-group-level documentation for
        // more information on 'bdex' streaming of container types.)

    template <class STREAM>
    static STREAM& bdexStreamOut(STREAM&             stream,
                                 bdem_ElemType::Type value,
                                 int                 version);
        // Write the specified 'value' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.  Optionally specify
        // an explicit 'version' format; by default, the maximum supported
        // version is written to 'stream' and used as the format.  If 'version'
        // is specified, that format is used but *not* written to 'stream'.  If
        // 'version' is not supported, 'stream' is left unmodified.  (See the
        // package-group-level documentation for more information on 'bdex'
        // streaming of container types).

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

    static int maxSupportedVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)
        //
        // DEPRECATED: replaced by 'maxSupportedBdexVersion()'

#endif // BDE_OMIT_INTERNAL_DEPRECATED
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, bdem_ElemType::Type value);
    // Write to the specified 'stream' the string representation of the
    // specified enumerator 'value', and return a reference to the modifiable
    // 'stream'.  The string representation of 'value' matches its
    // corresponding enumerator name with the "BDEM_" prefix elided.  For
    // example:
    //..
    //  bsl::cout << bdem_ElemType::BDEM_DATETIME;
    //..
    // will print the following on standard output:
    //..
    //  DATETIME
    //..

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // --------------------
                        // struct bdem_ElemType
                        // --------------------

// CLASS METHODS
inline
int bdem_ElemType::maxSupportedBdexVersion()
{
    return 1;
}

template <class STREAM>
STREAM& bdem_ElemType::bdexStreamIn(STREAM&              stream,
                                    bdem_ElemType::Type& value,
                                    int                  version)
{
    switch(version) {
      case 1: {
        signed char newValue;
        stream.getInt8(newValue);
        if (stream) {
            if (0 <= newValue && newValue < bdem_ElemType::BDEM_NUM_TYPES) {
                value = bdem_ElemType::Type(newValue);
            }
            else {
                stream.invalidate();  // Bad value in stream.
            }
        }
      } break;
      default: {
        stream.invalidate();          // Unrecognized version number.
      }
    }
    return stream;
}

template <class STREAM>
STREAM& bdem_ElemType::bdexStreamOut(STREAM&             stream,
                                     bdem_ElemType::Type value,
                                     int                 version)
{
    switch (version) {
      case 1: {
        stream.putInt8((signed char)value);  // Write value as a single byte.
      } break;
    }
    return stream;
}

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

inline
int bdem_ElemType::maxSupportedVersion()
{
    return maxSupportedBdexVersion();
}

#endif // BDE_OMIT_INTERNAL_DEPRECATED

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream& stream, bdem_ElemType::Type value)
{
    bdem_ElemType::print(stream, value);
    return stream;
}

// ============================================================================
//                     namespace bdex_InStreamFunctions
// ============================================================================

namespace bdex_InStreamFunctions {

template <typename STREAM>
inline
STREAM& streamIn(STREAM&              stream,
                 bdem_ElemType::Type& value,
                 int                  version)
{
    return bdem_ElemType::bdexStreamIn(stream, value, version);
}

}  // close namespace bdex_InStreamFunctions;

// ============================================================================
//                     namespace bdex_VersionFunctions
// ============================================================================

namespace bdex_VersionFunctions {

inline
int maxSupportedVersion(bdem_ElemType::Type)
{
    return bdem_ElemType::maxSupportedBdexVersion();
}

}  // close namespace bdex_VersionFunctions;

// ============================================================================
//                     namespace bdex_OutStreamFunctions
// ============================================================================

namespace bdex_OutStreamFunctions {

template <typename STREAM>
inline
STREAM& streamOut(STREAM&                    stream,
                  const bdem_ElemType::Type& value,
                  int                        version)
{
    return bdem_ElemType::bdexStreamOut(stream, value, version);
}

}  // close namespace bdex_OutStreamFunctions

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
