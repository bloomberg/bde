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

    &bdem_ChoiceHeader::d_choiceItemAttr,       // CHOICE_ARRAY_ITEM (-3)
    &bdem_RowData::d_rowAttr,                   // ROW               (-2)
    &bdem_Properties::d_voidAttr,               // VOID              (-1)

    // *** element attribute structures for scalar types ***

    &bdem_Properties::d_charAttr,
    &bdem_Properties::d_shortAttr,
    &bdem_Properties::d_intAttr,
    &bdem_Properties::d_int64Attr,
    &bdem_Properties::d_floatAttr,
    &bdem_Properties::d_doubleAttr,
    &bdem_Properties::d_stringAttr,
    &bdem_Properties::d_datetimeAttr,
    &bdem_Properties::d_dateAttr,
    &bdem_Properties::d_timeAttr,

    // *** element attribute structures for array types ***

    &bdem_Properties::d_charArrayAttr,
    &bdem_Properties::d_shortArrayAttr,
    &bdem_Properties::d_intArrayAttr,
    &bdem_Properties::d_int64ArrayAttr,
    &bdem_Properties::d_floatArrayAttr,
    &bdem_Properties::d_doubleArrayAttr,
    &bdem_Properties::d_stringArrayAttr,
    &bdem_Properties::d_datetimeArrayAttr,
    &bdem_Properties::d_dateArrayAttr,
    &bdem_Properties::d_timeArrayAttr,

    // *** element attribute structures for list and table types ***

    &bdem_ListImp::d_listAttr,
    &bdem_TableImp::d_tableAttr,

    // *** element attribute structures for newer 'bdem' types ***

    &bdem_Properties::d_boolAttr,
    &bdem_Properties::d_datetimeTzAttr,
    &bdem_Properties::d_dateTzAttr,
    &bdem_Properties::d_timeTzAttr,
    &bdem_Properties::d_boolArrayAttr,
    &bdem_Properties::d_datetimeTzArrayAttr,
    &bdem_Properties::d_dateTzArrayAttr,
    &bdem_Properties::d_timeTzArrayAttr,

    &bdem_ChoiceImp::d_choiceAttr,
    &bdem_ChoiceArrayImp::d_choiceArrayAttr
};

}  // close unnamed namespace

const bdem_Descriptor *const *const bdem_ElemAttrLookup::d_lookupTable =
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
