// bdlmxxx_elemattrlookup.cpp                                         -*-C++-*-
#include <bdlmxxx_elemattrlookup.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmxxx_elemattrlookup_cpp,"$Id$ $CSID$")


#include <bdlmxxx_choiceheader.h>
#include <bdlmxxx_properties.h>
#include <bdlmxxx_rowdata.h>

namespace BloombergLP {

namespace {

const int TOTAL_NUM_TYPES =
    bdlmxxx::ElemType::BDEM_NUM_TYPES + bdlmxxx::ElemType::BDEM_NUM_PSEUDO_TYPES;

const bdlmxxx::Descriptor *const rawLookupTable[TOTAL_NUM_TYPES] = {

    // *** element attribute structures for pseudo-types (negative indexes) ***

    &bdlmxxx::ChoiceHeader::s_choiceItemAttr,       // CHOICE_ARRAY_ITEM (-3)
    &bdlmxxx::RowData::s_rowAttr,                   // ROW               (-2)
    &bdlmxxx::Properties::s_voidAttr,               // VOID              (-1)

    // *** element attribute structures for scalar types ***

    &bdlmxxx::Properties::s_charAttr,
    &bdlmxxx::Properties::s_shortAttr,
    &bdlmxxx::Properties::s_intAttr,
    &bdlmxxx::Properties::s_int64Attr,
    &bdlmxxx::Properties::s_floatAttr,
    &bdlmxxx::Properties::s_doubleAttr,
    &bdlmxxx::Properties::s_stringAttr,
    &bdlmxxx::Properties::s_datetimeAttr,
    &bdlmxxx::Properties::s_dateAttr,
    &bdlmxxx::Properties::s_timeAttr,

    // *** element attribute structures for array types ***

    &bdlmxxx::Properties::s_charArrayAttr,
    &bdlmxxx::Properties::s_shortArrayAttr,
    &bdlmxxx::Properties::s_intArrayAttr,
    &bdlmxxx::Properties::s_int64ArrayAttr,
    &bdlmxxx::Properties::s_floatArrayAttr,
    &bdlmxxx::Properties::s_doubleArrayAttr,
    &bdlmxxx::Properties::s_stringArrayAttr,
    &bdlmxxx::Properties::s_datetimeArrayAttr,
    &bdlmxxx::Properties::s_dateArrayAttr,
    &bdlmxxx::Properties::s_timeArrayAttr,

    // *** element attribute structures for list and table types ***

    &bdlmxxx::ListImp::s_listAttr,
    &bdlmxxx::TableImp::s_tableAttr,

    // *** element attribute structures for newer 'bdem' types ***

    &bdlmxxx::Properties::s_boolAttr,
    &bdlmxxx::Properties::s_datetimeTzAttr,
    &bdlmxxx::Properties::s_dateTzAttr,
    &bdlmxxx::Properties::s_timeTzAttr,
    &bdlmxxx::Properties::s_boolArrayAttr,
    &bdlmxxx::Properties::s_datetimeTzArrayAttr,
    &bdlmxxx::Properties::s_dateTzArrayAttr,
    &bdlmxxx::Properties::s_timeTzArrayAttr,

    &bdlmxxx::ChoiceImp::s_choiceAttr,
    &bdlmxxx::ChoiceArrayImp::s_choiceArrayAttr
};

}  // close unnamed namespace

const bdlmxxx::Descriptor *const *const bdlmxxx::ElemAttrLookup::s_lookupTable =
    &rawLookupTable[bdlmxxx::ElemType::BDEM_NUM_PSEUDO_TYPES];

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
