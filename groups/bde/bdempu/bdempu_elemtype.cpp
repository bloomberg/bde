// bdempu_elemtype.cpp                                                -*-C++-*-
#include <bdempu_elemtype.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdempu_elemtype_cpp,"$Id$ $CSID$")

#include <bdepu_parserimputil.h>

#include <bsls_assert.h>

// Enum used in parsing element types

namespace BloombergLP {

enum { O_ = 0,   // letter O
       A_ = 1,   // letter A
       I_ = 2,   // letter I
       C_ = 3,   // letter C
       D_ = 4,   // letter D
       L_ = 5,   // letter L
       F_ = 6,   // letter F
       S_ = 7,   // letter S
       T_ = 8,   // letter T
       H_ = 9,   // letter H
       U_ = 10,  // character 'dash'
       N6 = 11,  // number 6
       NA = 12    // not applicable to ElemType parsing
};

static const char CHARTYPES[256] = {
/*  0*/    NA,     NA,     NA,     NA,     NA,     NA,     NA,     NA,
/*  8*/    NA,     NA,     NA,     NA,     NA,     NA,     NA,     NA,
/* 16*/    NA,     NA,     NA,     NA,     NA,     NA,     NA,     NA,
/* 24*/    NA,     NA,     NA,     NA,     NA,     NA,     NA,     NA,
/* 32*/    NA,     NA,     NA,     NA,     NA,     NA,     NA,     NA,
/* 40*/    NA,     NA,     NA,     NA,     NA,     NA,     NA,     NA,
/* 48*/    NA,     NA,     NA,     NA,     NA,     NA,     N6,     NA,
/* 56*/    NA,     NA,     NA,     NA,     NA,     NA,     NA,     NA,
/* 64*/    NA,     A_,     NA,     C_,     D_,     NA,     F_,     NA,
/* 72*/    H_,     I_,     NA,     NA,     L_,     NA,     NA,     O_,
/* 80*/    NA,     NA,     NA,     S_,     T_,     NA,     NA,     NA,
/* 88*/    NA,     NA,     NA,     NA,     NA,     NA,     NA,     U_,
/* 96*/    NA,     NA,     NA,     NA,     NA,     NA,     NA,     NA,
/*104*/    NA,     NA,     NA,     NA,     NA,     NA,     NA,     NA,
/*112*/    NA,     NA,     NA,     NA,     NA,     NA,     NA,     NA,
/*120*/    NA,     NA,     NA,     NA,     NA,     NA,     NA,     NA,
/*128*/    NA,     NA,     NA,     NA,     NA,     NA,     NA,     NA,
/*136*/    NA,     NA,     NA,     NA,     NA,     NA,     NA,     NA,
/*144*/    NA,     NA,     NA,     NA,     NA,     NA,     NA,     NA,
/*152*/    NA,     NA,     NA,     NA,     NA,     NA,     NA,     NA,
/*160*/    NA,     NA,     NA,     NA,     NA,     NA,     NA,     NA,
/*168*/    NA,     NA,     NA,     NA,     NA,     NA,     NA,     NA,
/*176*/    NA,     NA,     NA,     NA,     NA,     NA,     NA,     NA,
/*184*/    NA,     NA,     NA,     NA,     NA,     NA,     NA,     NA,
/*192*/    NA,     NA,     NA,     NA,     NA,     NA,     NA,     NA,
/*200*/    NA,     NA,     NA,     NA,     NA,     NA,     NA,     NA,
/*208*/    NA,     NA,     NA,     NA,     NA,     NA,     NA,     NA,
/*216*/    NA,     NA,     NA,     NA,     NA,     NA,     NA,     NA,
/*224*/    NA,     NA,     NA,     NA,     NA,     NA,     NA,     NA,
/*232*/    NA,     NA,     NA,     NA,     NA,     NA,     NA,     NA,
/*240*/    NA,     NA,     NA,     NA,     NA,     NA,     NA,     NA,
/*248*/    NA,     NA,     NA,     NA,     NA,     NA,     NA,     NA
};

static const char *s_charTypes = CHARTYPES;

enum { BDEMPU_SUCCESS = 0, BDEMPU_FAILURE = 1 };

                    // =====================
                    // local helper function
                    // =====================

static int parseTypeOrArray(const char                 **endPos,
                            bdem_ElemType::Type         *result,
                            const char                  *typeString,
                            const char                  *inputString,
                            const bdem_ElemType::Type&   elemType,
                            const bdem_ElemType::Type&   arrayType)
    // Skip past the value of the specified 'typeString' in the specified
    // 'inputString'.  If the next character is !not! '_', assign the value of
    // the specified 'elemType' to the specified 'result'.  If the next
    // character is '_', then skip past the string "_ARRAY" in the rest of the
    // 'inputString'.  If the string "_ARRAY" is found, assign the value of
    // the specified 'arrayType' to the specified 'result'.  Store in the
    // specified '*endPos' the address of the non-modifiable character in
    // 'inputString' immediately following the successfully parsed text, or
    // the position at which the parse failure was detected.  Return zero on
    // success and a non-zero value otherwise.   The behavior is undefined if
    // 0 == 'endPos', 0 == 'result', 0 == 'typeString', or 0 == 'inputString'.
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(typeString);
    BSLS_ASSERT(inputString);

    if (0 == bdepu_ParserImpUtil::
                    skipRequiredToken(&inputString, inputString, typeString)) {
        *result = elemType;
        *endPos = inputString;
        if (0 == bdepu_ParserImpUtil::
                      skipRequiredToken(&inputString, inputString, "_ARRAY")) {
            *result = arrayType;
            *endPos = inputString;
        }
        return BDEMPU_SUCCESS;
    }

    *endPos = inputString;
    return BDEMPU_FAILURE;
}

                    // ======================
                    // struct bdempu_ElemType
                    // ======================

int bdempu_ElemType::parseElemType(const char          **endPos,
                                   bdem_ElemType::Type  *result,
                                   const char           *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);

    switch (s_charTypes[(unsigned char)*inputString]) {
      case I_: {
        // This could be "INT", "INT64", "INT_ARRAY", or "INT64_ARRAY".
        ++inputString;
        if (0 == bdepu_ParserImpUtil::
                          skipRequiredToken(&inputString, inputString, "NT")) {
            *result = bdem_ElemType::BDEM_INT;
            *endPos = inputString;
            switch (s_charTypes[(unsigned char)*inputString]) {
              case N6: {
                // This could be "INT64" or "INT64_ARRAY".
                ++inputString;
                if (0 == parseTypeOrArray(&inputString,
                                          result,
                                          "4",
                                          inputString,
                                          bdem_ElemType::BDEM_INT64,
                                          bdem_ElemType::BDEM_INT64_ARRAY)) {
                  *endPos = inputString;
                }
              } break;
              case U_: {
                // This could be "INT_ARRAY".
                ++inputString;
                if (0 == bdepu_ParserImpUtil::
                       skipRequiredToken(&inputString, inputString, "ARRAY")) {
                    *result = bdem_ElemType::BDEM_INT_ARRAY;
                    *endPos = inputString;
                }
              } break;
            }
            return BDEMPU_SUCCESS;
        }
      } break;
      case C_: {
        // This could be "CHAR" or "CHAR_ARRAY".
        ++inputString;
        return parseTypeOrArray(endPos,
                                result,
                                "HAR",
                                inputString,
                                bdem_ElemType::BDEM_CHAR,
                                bdem_ElemType::BDEM_CHAR_ARRAY);
      }
      case D_: {
        // This could be "DOUBLE", "DATE", "DATETIME", or their corresponding
        // arrays.
        ++inputString;
        switch (s_charTypes[(unsigned char)*inputString]) {
          case O_: {
            // This could be "DOUBLE" or "DOUBLE_ARRAY".
            ++inputString;
            return parseTypeOrArray(endPos,
                                    result,
                                    "UBLE",
                                    inputString,
                                    bdem_ElemType::BDEM_DOUBLE,
                                    bdem_ElemType::BDEM_DOUBLE_ARRAY);
          }
          case A_: {
            // This could be "DATE", "DATETIME", or their corresponding arrays.
            ++inputString;
            if (0 == bdepu_ParserImpUtil::
                          skipRequiredToken(&inputString, inputString, "TE")) {
                *endPos = inputString;
                *result = bdem_ElemType::BDEM_DATE;
                switch (s_charTypes[(unsigned char)*inputString]) {
                  case U_: {
                    // This could be "DATE_ARRAY".
                    ++inputString;
                    if (0 == bdepu_ParserImpUtil::
                                                skipRequiredToken(&inputString,
                                                                  inputString,
                                                                  "ARRAY")) {
                        *result = bdem_ElemType::BDEM_DATE_ARRAY;
                        *endPos = inputString;
                    }
                    return BDEMPU_SUCCESS;
                  }
                  case T_: {
                    // This could be "DATETIME" or "DATETIME_ARRAY".
                    ++inputString;
                    if (0 == parseTypeOrArray(
                                         &inputString,
                                         result,
                                         "IME",
                                         inputString,
                                         bdem_ElemType::BDEM_DATETIME,
                                         bdem_ElemType::BDEM_DATETIME_ARRAY)) {
                        *endPos = inputString;
                    }
                    return BDEMPU_SUCCESS;
                  }
                }
                return BDEMPU_SUCCESS;
            }
          }
        }
      } break;
      case L_: {
        // This could be "LIST".
        ++inputString;
        if (0 == bdepu_ParserImpUtil::
                         skipRequiredToken(&inputString, inputString, "IST")) {
            *result = bdem_ElemType::BDEM_LIST;
            *endPos = inputString;
            return BDEMPU_SUCCESS;
        }
      } break;
      case F_: {
        // This could be "FLOAT" or "FLOAT_ARRAY".
        ++inputString;
        return parseTypeOrArray(endPos,
                                result,
                                "LOAT",
                                inputString,
                                bdem_ElemType::BDEM_FLOAT,
                                bdem_ElemType::BDEM_FLOAT_ARRAY);
      }
      case S_: {
        // This could be "STRING", "SHORT", or their corresponding arrays.
        ++inputString;
        switch (s_charTypes[(unsigned char)*inputString]) {
          case T_: {
              // This could be "STRING" or "STRING_ARRAY".
              ++inputString;
              return parseTypeOrArray(endPos,
                                      result,
                                      "RING",
                                      inputString,
                                      bdem_ElemType::BDEM_STRING,
                                      bdem_ElemType::BDEM_STRING_ARRAY);
          }
          case H_: {
              // This could be "SHORT" or "SHORT_ARRAY".
              ++inputString;
              return parseTypeOrArray(endPos,
                                      result,
                                      "ORT",
                                      inputString,
                                      bdem_ElemType::BDEM_SHORT,
                                      bdem_ElemType::BDEM_SHORT_ARRAY);
          }
        }
      } break;
      case T_: {
        // This could be "TABLE", "TIME", or "TIME_ARRAY".
        ++inputString;
        switch (s_charTypes[(unsigned char)*inputString]) {
          case A_: {
              // This could be "TABLE".
              ++inputString;
              if (0 == bdepu_ParserImpUtil::
                         skipRequiredToken(&inputString, inputString, "BLE")) {
                  *result = bdem_ElemType::BDEM_TABLE;
                  *endPos = inputString;
                  return BDEMPU_SUCCESS;
              }
          } break;
          case I_: {
              // This could be "TIME" or "TIME_ARRAY".
              ++inputString;
              return parseTypeOrArray(endPos,
                                      result,
                                      "ME",
                                      inputString,
                                      bdem_ElemType::BDEM_TIME,
                                      bdem_ElemType::BDEM_TIME_ARRAY);
          }
        }
      } break;
    }

    *endPos = inputString;
    return BDEMPU_FAILURE;
}

void bdempu_ElemType::
                generateElemTypeRaw(bsl::vector<char>          *buffer,
                                    const bdem_ElemType::Type&  value,
                                    int                         level,
                                    int                         spacesPerLevel)
{
    BSLS_ASSERT(buffer);

    if (level > 0) {
        bdepu_ParserImpUtil::
                            generateIndentation(buffer, level, spacesPerLevel);
    }

#define CASE(X) case bdem_ElemType::BDEM_ ## X: { \
                  const bsl::string str = #X; \
                  buffer->insert(buffer->end(), str.begin(), str.end()); \
                } break;

    switch (value) {
      CASE(CHAR)
      CASE(SHORT)
      CASE(INT)
      CASE(INT64)
      CASE(FLOAT)
      CASE(DOUBLE)
      CASE(STRING)
      CASE(DATETIME)
      CASE(DATE)
      CASE(TIME)
      CASE(CHAR_ARRAY)
      CASE(SHORT_ARRAY)
      CASE(INT_ARRAY)
      CASE(INT64_ARRAY)
      CASE(FLOAT_ARRAY)
      CASE(DOUBLE_ARRAY)
      CASE(STRING_ARRAY)
      CASE(DATETIME_ARRAY)
      CASE(DATE_ARRAY)
      CASE(TIME_ARRAY)
      CASE(LIST)
      CASE(TABLE)
      // TBD
      // The following catches 'BDEM_BOOL', 'BDEM_DATETIMETZ', etc.  This
      // package is deprecated, so it is unlikely to be extended to support
      // the new types.
      default: {
        BSLS_ASSERT(0);  // Should *not* reach here!
      } break;
    }

#undef CASE

}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
