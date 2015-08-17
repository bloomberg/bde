// bdlmxxx_elemtype.cpp                                               -*-C++-*-
#include <bdlmxxx_elemtype.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmxxx_elemtype_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsls_performancehint.h>

#include <bsl_ostream.h>

namespace BloombergLP {

typedef bdlmxxx::ElemType Et;

struct ElemTypeMapElement {
    // DATA
    const char     *d_name;
    const Et::Type  d_arrayElemType;
    const Et::Type  d_baseElemType;
};

static const ElemTypeMapElement rawElemTypeMap[] = {
    // name                 to array type              from array type
    // -------------------  -------------------------  -------------------
    {  "CHOICE_ARRAY_ITEM", Et::BDEM_CHOICE_ARRAY,     Et::BDEM_VOID       },
    {  "ROW",               Et::BDEM_TABLE,            Et::BDEM_VOID       },
    {  "VOID",              Et::BDEM_VOID,             Et::BDEM_VOID       },

    {  "CHAR",              Et::BDEM_CHAR_ARRAY,       Et::BDEM_VOID       },
    {  "SHORT",             Et::BDEM_SHORT_ARRAY,      Et::BDEM_VOID       },
    {  "INT",               Et::BDEM_INT_ARRAY,        Et::BDEM_VOID       },
    {  "INT64",             Et::BDEM_INT64_ARRAY,      Et::BDEM_VOID       },
    {  "FLOAT",             Et::BDEM_FLOAT_ARRAY,      Et::BDEM_VOID       },
    {  "DOUBLE",            Et::BDEM_DOUBLE_ARRAY,     Et::BDEM_VOID       },
    {  "STRING",            Et::BDEM_STRING_ARRAY,     Et::BDEM_VOID       },
    {  "DATETIME",          Et::BDEM_DATETIME_ARRAY,   Et::BDEM_VOID       },
    {  "DATE",              Et::BDEM_DATE_ARRAY,       Et::BDEM_VOID       },
    {  "TIME",              Et::BDEM_TIME_ARRAY,       Et::BDEM_VOID       },
    {  "CHAR_ARRAY",        Et::BDEM_VOID,             Et::BDEM_CHAR       },
    {  "SHORT_ARRAY",       Et::BDEM_VOID,             Et::BDEM_SHORT      },
    {  "INT_ARRAY",         Et::BDEM_VOID,             Et::BDEM_INT        },
    {  "INT64_ARRAY",       Et::BDEM_VOID,             Et::BDEM_INT64      },
    {  "FLOAT_ARRAY",       Et::BDEM_VOID,             Et::BDEM_FLOAT      },
    {  "DOUBLE_ARRAY",      Et::BDEM_VOID,             Et::BDEM_DOUBLE     },
    {  "STRING_ARRAY",      Et::BDEM_VOID,             Et::BDEM_STRING     },
    {  "DATETIME_ARRAY",    Et::BDEM_VOID,             Et::BDEM_DATETIME   },
    {  "DATE_ARRAY",        Et::BDEM_VOID,             Et::BDEM_DATE       },
    {  "TIME_ARRAY",        Et::BDEM_VOID,             Et::BDEM_TIME       },
    {  "LIST",              Et::BDEM_TABLE,            Et::BDEM_VOID       },
    {  "TABLE",             Et::BDEM_VOID,             Et::BDEM_LIST       },
    {  "BOOL",              Et::BDEM_BOOL_ARRAY,       Et::BDEM_VOID       },
    {  "DATETIMETZ",        Et::BDEM_DATETIMETZ_ARRAY, Et::BDEM_VOID       },
    {  "DATETZ",            Et::BDEM_DATETZ_ARRAY,     Et::BDEM_VOID       },
    {  "TIMETZ",            Et::BDEM_TIMETZ_ARRAY,     Et::BDEM_VOID       },
    {  "BOOL_ARRAY",        Et::BDEM_VOID,             Et::BDEM_BOOL       },
    {  "DATETIMETZ_ARRAY",  Et::BDEM_VOID,             Et::BDEM_DATETIMETZ },
    {  "DATETZ_ARRAY",      Et::BDEM_VOID,             Et::BDEM_DATETZ     },
    {  "TIMETZ_ARRAY",      Et::BDEM_VOID,             Et::BDEM_TIMETZ     },
    {  "CHOICE",            Et::BDEM_CHOICE_ARRAY,     Et::BDEM_VOID       },
    {  "CHOICE_ARRAY",      Et::BDEM_VOID,             Et::BDEM_CHOICE     },
};

static const ElemTypeMapElement *const elemTypeMap =
                                    &rawElemTypeMap[Et::BDEM_NUM_PSEUDO_TYPES];

namespace bdlmxxx {
// Note that since the enumerators for "pseudo types" ('BDEM_VOID', 'BDEM_ROW',
// 'BDEM_CHOICE_ARRAY_ITEM') have negative values (-1, -2, -3, respectively),
// 'elemTypeMap' is initialized such that it can be indexed with those negative
// values.  For example, the entry for 'ElemType::BDEM_VOID' is at
// 'elemTypeMap[ElemType::BDEM_VOID]'.

                        // --------------------
                        // struct ElemType
                        // --------------------

// CLASS METHODS
ElemType::Type ElemType::toArrayType(ElemType::Type value)
{
    BSLS_ASSERT(-BDEM_NUM_PSEUDO_TYPES <= (int)value);
    BSLS_ASSERT((int)value < BDEM_NUM_TYPES);

    return elemTypeMap[value].d_arrayElemType;
}

ElemType::Type ElemType::fromArrayType(ElemType::Type value)
{
    BSLS_ASSERT(-BDEM_NUM_PSEUDO_TYPES <= (int)value);
    BSLS_ASSERT((int)value < BDEM_NUM_TYPES);

    return elemTypeMap[value].d_baseElemType;
}

bool ElemType::isArrayType(ElemType::Type value)
{
    switch (value) {
      case ElemType::BDEM_CHAR_ARRAY:
      case ElemType::BDEM_SHORT_ARRAY:
      case ElemType::BDEM_INT_ARRAY:
      case ElemType::BDEM_INT64_ARRAY:
      case ElemType::BDEM_FLOAT_ARRAY:
      case ElemType::BDEM_DOUBLE_ARRAY:
      case ElemType::BDEM_STRING_ARRAY:
      case ElemType::BDEM_DATETIME_ARRAY:
      case ElemType::BDEM_DATE_ARRAY:
      case ElemType::BDEM_TIME_ARRAY:
      case ElemType::BDEM_TABLE:
      case ElemType::BDEM_BOOL_ARRAY:
      case ElemType::BDEM_DATETIMETZ_ARRAY:
      case ElemType::BDEM_DATETZ_ARRAY:
      case ElemType::BDEM_TIMETZ_ARRAY:
      case ElemType::BDEM_CHOICE_ARRAY: {
        return true;                                                  // RETURN
      }
      default: {
        return false;                                                 // RETURN
      }
    }
}

bool ElemType::isAggregateType(ElemType::Type value)
{
    switch (value) {
      case ElemType::BDEM_CHOICE_ARRAY_ITEM:
      case ElemType::BDEM_ROW:
      case ElemType::BDEM_LIST:
      case ElemType::BDEM_TABLE:
      case ElemType::BDEM_CHOICE:
      case ElemType::BDEM_CHOICE_ARRAY: {
        return true;                                                  // RETURN
      }
      default: {
        return false;                                                 // RETURN
      }
    }
}

bool ElemType::isChoiceType(ElemType::Type value)
{
    switch (value) {
      case ElemType::BDEM_CHOICE_ARRAY_ITEM:
      case ElemType::BDEM_CHOICE:
      case ElemType::BDEM_CHOICE_ARRAY: {
        return true;                                                  // RETURN
      }
      default: {
        return false;                                                 // RETURN
      }
    }
}

bool ElemType::isScalarType(ElemType::Type value)
{
    switch (value) {
      case ElemType::BDEM_CHAR:
      case ElemType::BDEM_SHORT:
      case ElemType::BDEM_INT:
      case ElemType::BDEM_INT64:
      case ElemType::BDEM_FLOAT:
      case ElemType::BDEM_DOUBLE:
      case ElemType::BDEM_STRING:
      case ElemType::BDEM_DATETIME:
      case ElemType::BDEM_DATE:
      case ElemType::BDEM_TIME:
      case ElemType::BDEM_BOOL:
      case ElemType::BDEM_DATETIMETZ:
      case ElemType::BDEM_DATETZ:
      case ElemType::BDEM_TIMETZ: {
        return true;                                                  // RETURN
      }
      default: {
        return false;                                                 // RETURN
      }
    }
}

void ElemType::print(bsl::ostream& stream, ElemType::Type value)
{
    stream << ElemType::toAscii(value);
}

const char *ElemType::toAscii(ElemType::Type value)
{
    BSLS_ASSERT(-BDEM_NUM_PSEUDO_TYPES <= (int)value);
    BSLS_ASSERT((int)value < BDEM_NUM_TYPES);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
        (int)value < -BDEM_NUM_PSEUDO_TYPES || (int)value >= BDEM_NUM_TYPES)) {
        return "(* UNKNOWN *)";                                       // RETURN
    }

    return elemTypeMap[value].d_name;
}
}  // close package namespace

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
