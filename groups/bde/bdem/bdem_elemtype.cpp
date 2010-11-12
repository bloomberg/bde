// bdem_elemtype.cpp                                                  -*-C++-*-
#include <bdem_elemtype.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdem_elemtype_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsls_performancehint.h>

#include <bsl_ostream.h>

namespace BloombergLP {

typedef bdem_ElemType Et;

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

// Note that since the enumerators for "pseudo types" ('BDEM_VOID', 'BDEM_ROW',
// 'BDEM_CHOICE_ARRAY_ITEM') have negative values (-1, -2, -3, respectively),
// 'elemTypeMap' is initialized such that it can be indexed with those negative
// values.  For example, the entry for 'bdem_ElemType::BDEM_VOID' is at
// 'elemTypeMap[bdem_ElemType::BDEM_VOID]'.

                        // --------------------
                        // struct bdem_ElemType
                        // --------------------

// CLASS METHODS
bdem_ElemType::Type bdem_ElemType::toArrayType(bdem_ElemType::Type value)
{
    BSLS_ASSERT(-BDEM_NUM_PSEUDO_TYPES <= (int)value);
    BSLS_ASSERT((int)value < BDEM_NUM_TYPES);

    return elemTypeMap[value].d_arrayElemType;
}

bdem_ElemType::Type bdem_ElemType::fromArrayType(bdem_ElemType::Type value)
{
    BSLS_ASSERT(-BDEM_NUM_PSEUDO_TYPES <= (int)value);
    BSLS_ASSERT((int)value < BDEM_NUM_TYPES);

    return elemTypeMap[value].d_baseElemType;
}

bool bdem_ElemType::isArrayType(bdem_ElemType::Type value)
{
    switch (value) {
      case bdem_ElemType::BDEM_CHAR_ARRAY:
      case bdem_ElemType::BDEM_SHORT_ARRAY:
      case bdem_ElemType::BDEM_INT_ARRAY:
      case bdem_ElemType::BDEM_INT64_ARRAY:
      case bdem_ElemType::BDEM_FLOAT_ARRAY:
      case bdem_ElemType::BDEM_DOUBLE_ARRAY:
      case bdem_ElemType::BDEM_STRING_ARRAY:
      case bdem_ElemType::BDEM_DATETIME_ARRAY:
      case bdem_ElemType::BDEM_DATE_ARRAY:
      case bdem_ElemType::BDEM_TIME_ARRAY:
      case bdem_ElemType::BDEM_TABLE:
      case bdem_ElemType::BDEM_BOOL_ARRAY:
      case bdem_ElemType::BDEM_DATETIMETZ_ARRAY:
      case bdem_ElemType::BDEM_DATETZ_ARRAY:
      case bdem_ElemType::BDEM_TIMETZ_ARRAY:
      case bdem_ElemType::BDEM_CHOICE_ARRAY: {
        return true;
      }
      default: {
        return false;
      }
    }
}

bool bdem_ElemType::isAggregateType(bdem_ElemType::Type value)
{
    switch (value) {
      case bdem_ElemType::BDEM_CHOICE_ARRAY_ITEM:
      case bdem_ElemType::BDEM_ROW:
      case bdem_ElemType::BDEM_LIST:
      case bdem_ElemType::BDEM_TABLE:
      case bdem_ElemType::BDEM_CHOICE:
      case bdem_ElemType::BDEM_CHOICE_ARRAY: {
        return true;
      }
      default: {
        return false;
      }
    }
}

bool bdem_ElemType::isChoiceType(bdem_ElemType::Type value)
{
    switch (value) {
      case bdem_ElemType::BDEM_CHOICE_ARRAY_ITEM:
      case bdem_ElemType::BDEM_CHOICE:
      case bdem_ElemType::BDEM_CHOICE_ARRAY: {
        return true;
      }
      default: {
        return false;
      }
    }
}

bool bdem_ElemType::isScalarType(bdem_ElemType::Type value)
{
    switch (value) {
      case bdem_ElemType::BDEM_CHAR:
      case bdem_ElemType::BDEM_SHORT:
      case bdem_ElemType::BDEM_INT:
      case bdem_ElemType::BDEM_INT64:
      case bdem_ElemType::BDEM_FLOAT:
      case bdem_ElemType::BDEM_DOUBLE:
      case bdem_ElemType::BDEM_STRING:
      case bdem_ElemType::BDEM_DATETIME:
      case bdem_ElemType::BDEM_DATE:
      case bdem_ElemType::BDEM_TIME:
      case bdem_ElemType::BDEM_BOOL:
      case bdem_ElemType::BDEM_DATETIMETZ:
      case bdem_ElemType::BDEM_DATETZ:
      case bdem_ElemType::BDEM_TIMETZ: {
        return true;
      }
      default: {
        return false;
      }
    }
}

void bdem_ElemType::print(bsl::ostream& stream, bdem_ElemType::Type value)
{
    stream << bdem_ElemType::toAscii(value);
}

const char *bdem_ElemType::toAscii(bdem_ElemType::Type value)
{
    BSLS_ASSERT(-BDEM_NUM_PSEUDO_TYPES <= (int)value);
    BSLS_ASSERT((int)value < BDEM_NUM_TYPES);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
        (int)value < -BDEM_NUM_PSEUDO_TYPES || (int)value >= BDEM_NUM_TYPES)) {
        return "(* UNKNOWN *)";
    }

    return elemTypeMap[value].d_name;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
