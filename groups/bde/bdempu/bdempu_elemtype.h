// bdempu_elemtype.h            -*-C++-*-
#ifndef INCLUDED_BDEMPU_ELEMTYPE
#define INCLUDED_BDEMPU_ELEMTYPE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Convert 'bdem_ElemType' enum value codes to/from symbolic names.
//
//@DEPRECATED: Use 'bdem_ElemType' 'toAscii' function instead.
//
//@CLASSES:
//  bdempu_ElemType: namespace for parser/generator of 'bdem_ElemType::Type'
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component provides utility functions for parsing and
// generating symbolic names corresponding to 'bdem_ElemType::Type' codes.
//..
//      Scalar Types              Array Types           Aggregate Types
//      ------------            --------------          ---------------
//           CHAR                   CHAR_ARRAY                LIST
//          SHORT                  SHORT_ARRAY               TABLE
//            INT                    INT_ARRAY
//          INT64                  INT64_ARRAY
//          FLOAT                  FLOAT_ARRAY
//         DOUBLE                 DOUBLE_ARRAY
//         STRING                 STRING_ARRAY
//       DATETIME               DATETIME_ARRAY
//           DATE                   DATE_ARRAY
//           TIME                   TIME_ARRAY
//..
// The parsing function that parses symbolic type names, 'parseElemType',
// ignores leading whitespace (including C- and C++-style comments).
//
///USAGE
///-----
// The following function illustrates how to parse a string containing a
// 'bdem_ElemType' symbolic name followed by a value of the corresponding type.
//..
//  #include <bdempu_elemtype.h>
//  #include <bdepu_typesparser.h>
//
//  int extractTypeValuePair(const char *s)
//  {
//      enum { SUCCESS = 0, FAILURE = 1 };
//      bdem_ElemType::Type t;
//
//      if (0 != bdempu_ElemType::parseElemType(&s, &t, s)) {
//          return FAILURE;
//      }
//
//      switch (t) {
//        case bdem_ElemType::BDEM_CHAR: {
//          char value; // Note that 'value' will be extracted as a 'char'.
//
//          if (0 != bdepu_TypesParser::parseChar(&s, &value, s)) {
//              return FAILURE;
//          }
//
//          // do something with the 'char' value...
//
//        } break;
//
//        case bdem_ElemType::BDEM_SHORT: {
//          short value;  // Note that 'value' will be extracted as an 'short'.
//
//          if (0 != bdepu_TypesParser::parseShort(&s, &value, s)) {
//              return FAILURE;
//          }
//
//          // do something with the 'short' value...
//
//        } break;
//
//        // similarly for the remaining 20 'bdem' types...
//      }
//
//      return SUCCESS;
//  }
//..
#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEM_ELEMTYPE
#include <bdem_elemtype.h>
#endif

#ifndef INCLUDED_BDEPU_NTYPESPARSER
#include <bdepu_ntypesparser.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif


namespace BloombergLP {



                        // ===============
                        // bdempu_ElemType
                        // ===============

struct bdempu_ElemType {
    // This struct provides a namespace for a suite of stateless procedures
    // to convert 'bdem_ElemType::Type' values to and from textual
    // representations.

    static int parseElemType(const char          **endPos,
                             bdem_ElemType::Type  *result,
                             const char           *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the symbolic name of one of the 'bdem' element types and
        // place the corresponding enumeration code into the specified
        // 'result'.  Store in the specified '*endPos' the address of the
        // non-modifiable character immediately following the successfully
        // parsed text, or the position at which the parse failure was
        // detected.  Return zero on success, and a non-zero value otherwise.
        // The behavior is undefined if any argument is 0.  Note that the
        // left-to-right scan will move past (ignore) leading whitespace
        // characters (as defined by 'isspace'), as well as any C- or
        // C++-style comments.

    static void generateElemType(
                              bsl::vector<char>           *buffer,
                              const bdem_ElemType::Type&  value,
                              int                         level = 0,
                              int                         spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer' while maintaining (or adding if not present)
        // null-character termination.  When 'level' is positive, indent the
        // symbolic name by ('level' * 'spacesPerLevel') spaces.  A negative
        // 'level' suppresses indentation.  The behavior is undefined if
        // 0 == 'buffer' and unless 0 <= 'spacesPerLevel'.  Note that 'value'
        // is emitted in a form parsable by the corresponding 'parseElemType'
        // function.

    static void generateElemTypeRaw(
                             bsl::vector<char>           *buffer,
                             const bdem_ElemType::Type&  value,
                             int                         level = 0,
                             int                         spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer'.  When 'level' is positive, indent the
        // symbolic name by ('level' * 'spacesPerLevel') spaces.  A negative
        // 'level' suppresses indentation.  The behavior is undefined if
        // 0 == 'buffer' and unless 0 <= 'spacesPerLevel'.  Note that this
        // method is similar to 'generateElemType' except that no trailing
        // null is ever removed or supplied.
};


// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

inline
void bdempu_ElemType::generateElemType(
                                  bsl::vector<char>           *buffer,
                                  const bdem_ElemType::Type&  value,
                                  int                         level,
                                  int                         spacesPerLevel)
{
    bdepu_NTypesParser::stripNull(buffer);
    generateElemTypeRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

}  // close namespace BloombergLP



#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
