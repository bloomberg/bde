// bdem_elemattrlookup.cpp                  -*-C++-*-
#include <bdem_elemattrlookup.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdem_elemattrlookup_cpp,"$Id$ $CSID$")


#include <bdem_choiceheader.h>
#include <bdem_properties.h>
#include <bdem_rowdata.h>

namespace BloombergLP {

namespace {

const int TOTAL_NUM_TYPES =
    bdem_ElemType::BDEM_NUM_TYPES + bdem_ElemType::BDEM_NUM_PSEUDO_TYPES;

const bdem_Descriptor *const rawLookupTable[TOTAL_NUM_TYPES] = {

    // *** element attribute structures for pseudo-types (negative indexes) ***

    &bdem_ChoiceHeader::s_choiceItemAttr,       // CHOICE_ARRAY_ITEM (-3)
    &bdem_RowData::s_rowAttr,                   // ROW               (-2)
    &bdem_Properties::s_voidAttr,               // VOID              (-1)

    // *** element attribute structures for scalar types ***

    &bdem_Properties::s_charAttr,
    &bdem_Properties::s_shortAttr,
    &bdem_Properties::s_intAttr,
    &bdem_Properties::s_int64Attr,
    &bdem_Properties::s_floatAttr,
    &bdem_Properties::s_doubleAttr,
    &bdem_Properties::s_stringAttr,
    &bdem_Properties::s_datetimeAttr,
    &bdem_Properties::s_dateAttr,
    &bdem_Properties::s_timeAttr,

    // *** element attribute structures for array types ***

    &bdem_Properties::s_charArrayAttr,
    &bdem_Properties::s_shortArrayAttr,
    &bdem_Properties::s_intArrayAttr,
    &bdem_Properties::s_int64ArrayAttr,
    &bdem_Properties::s_floatArrayAttr,
    &bdem_Properties::s_doubleArrayAttr,
    &bdem_Properties::s_stringArrayAttr,
    &bdem_Properties::s_datetimeArrayAttr,
    &bdem_Properties::s_dateArrayAttr,
    &bdem_Properties::s_timeArrayAttr,

    // *** element attribute structures for list and table types ***

    &bdem_ListImp::s_listAttr,
    &bdem_TableImp::s_tableAttr,

    // *** element attribute structures for newer 'bdem' types ***

    &bdem_Properties::s_boolAttr,
    &bdem_Properties::s_datetimeTzAttr,
    &bdem_Properties::s_dateTzAttr,
    &bdem_Properties::s_timeTzAttr,
    &bdem_Properties::s_boolArrayAttr,
    &bdem_Properties::s_datetimeTzArrayAttr,
    &bdem_Properties::s_dateTzArrayAttr,
    &bdem_Properties::s_timeTzArrayAttr,

    &bdem_ChoiceImp::s_choiceAttr,
    &bdem_ChoiceArrayImp::s_choiceArrayAttr
};

}  // close unnamed namespace

const bdem_Descriptor *const *const bdem_ElemAttrLookup::s_lookupTable =
    &rawLookupTable[bdem_ElemType::BDEM_NUM_PSEUDO_TYPES];

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
