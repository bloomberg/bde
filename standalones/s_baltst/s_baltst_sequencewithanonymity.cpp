// s_baltst_sequencewithanonymity.cpp        *DO NOT EDIT*         @generated -*-C++-*-

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_sequencewithanonymity_cpp,"$Id$ $CSID$")

#include <s_baltst_sequencewithanonymity.h>

#include <bdlat_formattingmode.h>
#include <bdlat_valuetypefunctions.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>
#include <bdlb_string.h>

#include <bdlb_nullablevalue.h>
#include <s_baltst_address.h>
#include <s_baltst_basicrecord.h>
#include <s_baltst_customint.h>
#include <s_baltst_customstring.h>
#include <s_baltst_customizedstring.h>
#include <s_baltst_enumerated.h>
#include <s_baltst_mychoice.h>
#include <s_baltst_myenumeration.h>
#include <s_baltst_mysequence.h>
#include <s_baltst_mysequencewithanonymouschoicechoice.h>
#include <s_baltst_mysequencewitharray.h>
#include <s_baltst_mysequencewithattributes.h>
#include <s_baltst_mysequencewithnillable.h>
#include <s_baltst_mysequencewithnullable.h>
#include <s_baltst_mysimplecontent.h>
#include <s_baltst_mysimpleintcontent.h>
#include <s_baltst_rawdata.h>
#include <s_baltst_rawdataswitched.h>
#include <s_baltst_rawdataunformatted.h>
#include <s_baltst_sequencewithanonymitychoice1.h>
#include <s_baltst_simplerequest.h>
#include <s_baltst_sqrt.h>
#include <s_baltst_unsignedsequence.h>
#include <s_baltst_voidsequence.h>
#include <s_baltst_bigrecord.h>
#include <s_baltst_employee.h>
#include <s_baltst_mysequencewithanonymouschoice.h>
#include <s_baltst_mysequencewithnillables.h>
#include <s_baltst_mysequencewithnullables.h>
#include <s_baltst_ratsnest.h>
#include <s_baltst_ratsnest.h>
#include <s_baltst_ratsnest.h>
#include <s_baltst_ratsnest.h>
#include <s_baltst_sequencewithanonymitychoice.h>
#include <s_baltst_timingrequest.h>
#include <s_baltst_ratsnest.h>
#include <s_baltst_ratsnest.h>
#include <s_baltst_ratsnest.h>
#include <s_baltst_ratsnest.h>
#include <s_baltst_ratsnest.h>
#include <s_baltst_sequencewithanonymitychoice2.h>
#include <bslim_printer.h>
#include <bsls_assert.h>

#include <bsl_iomanip.h>
#include <bsl_limits.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace s_baltst {

                        // ---------------------------
                        // class SequenceWithAnonymity
                        // ---------------------------

// CONSTANTS

const char SequenceWithAnonymity::CLASS_NAME[] = "SequenceWithAnonymity";

const bdlat_AttributeInfo SequenceWithAnonymity::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_CHOICE,
        "Choice",
        sizeof("Choice") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_UNTAGGED
    },
    {
        ATTRIBUTE_ID_CHOICE1,
        "Choice-1",
        sizeof("Choice-1") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_UNTAGGED
    },
    {
        ATTRIBUTE_ID_CHOICE2,
        "Choice-2",
        sizeof("Choice-2") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_UNTAGGED
    },
    {
        ATTRIBUTE_ID_ELEMENT4,
        "element4",
        sizeof("element4") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *SequenceWithAnonymity::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    if (bdlb::String::areEqualCaseless("selection1", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
    }

    if (bdlb::String::areEqualCaseless("selection2", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
    }

    if (bdlb::String::areEqualCaseless("selection3", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
    }

    if (bdlb::String::areEqualCaseless("selection4", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
    }

    if (bdlb::String::areEqualCaseless("selection5", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE1];
    }

    if (bdlb::String::areEqualCaseless("selection6", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE1];
    }

    if (bdlb::String::areEqualCaseless("selection7", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE2];
    }

    if (bdlb::String::areEqualCaseless("selection8", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE2];
    }

    for (int i = 0; i < 4; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    SequenceWithAnonymity::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *SequenceWithAnonymity::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_CHOICE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
      case ATTRIBUTE_ID_CHOICE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE1];
      case ATTRIBUTE_ID_CHOICE2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE2];
      case ATTRIBUTE_ID_ELEMENT4:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4];
      default:
        return 0;
    }
}

// CREATORS

SequenceWithAnonymity::SequenceWithAnonymity(bslma::Allocator *basicAllocator)
: d_choice2(basicAllocator)
, d_choice1(basicAllocator)
, d_choice(basicAllocator)
, d_element4(basicAllocator)
{
}

SequenceWithAnonymity::SequenceWithAnonymity(const SequenceWithAnonymity& original,
                                             bslma::Allocator *basicAllocator)
: d_choice2(original.d_choice2, basicAllocator)
, d_choice1(original.d_choice1, basicAllocator)
, d_choice(original.d_choice, basicAllocator)
, d_element4(original.d_element4, basicAllocator)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
SequenceWithAnonymity::SequenceWithAnonymity(SequenceWithAnonymity&& original) noexcept
: d_choice2(bsl::move(original.d_choice2))
, d_choice1(bsl::move(original.d_choice1))
, d_choice(bsl::move(original.d_choice))
, d_element4(bsl::move(original.d_element4))
{
}

SequenceWithAnonymity::SequenceWithAnonymity(SequenceWithAnonymity&& original,
                                             bslma::Allocator *basicAllocator)
: d_choice2(bsl::move(original.d_choice2), basicAllocator)
, d_choice1(bsl::move(original.d_choice1), basicAllocator)
, d_choice(bsl::move(original.d_choice), basicAllocator)
, d_element4(bsl::move(original.d_element4), basicAllocator)
{
}
#endif

SequenceWithAnonymity::~SequenceWithAnonymity()
{
}

// MANIPULATORS

SequenceWithAnonymity&
SequenceWithAnonymity::operator=(const SequenceWithAnonymity& rhs)
{
    if (this != &rhs) {
        d_choice = rhs.d_choice;
        d_choice1 = rhs.d_choice1;
        d_choice2 = rhs.d_choice2;
        d_element4 = rhs.d_element4;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
SequenceWithAnonymity&
SequenceWithAnonymity::operator=(SequenceWithAnonymity&& rhs)
{
    if (this != &rhs) {
        d_choice = bsl::move(rhs.d_choice);
        d_choice1 = bsl::move(rhs.d_choice1);
        d_choice2 = bsl::move(rhs.d_choice2);
        d_element4 = bsl::move(rhs.d_element4);
    }

    return *this;
}
#endif

void SequenceWithAnonymity::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_choice);
    bdlat_ValueTypeFunctions::reset(&d_choice1);
    bdlat_ValueTypeFunctions::reset(&d_choice2);
    bdlat_ValueTypeFunctions::reset(&d_element4);
}

// ACCESSORS

bsl::ostream& SequenceWithAnonymity::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("choice", d_choice);
    printer.printAttribute("choice1", d_choice1);
    printer.printAttribute("choice2", d_choice2);
    printer.printAttribute("element4", d_element4);
    printer.end();
    return stream;
}


}  // close package namespace
}  // close enterprise namespace

// GENERATED BY BLP_BAS_CODEGEN_2020.03.30
// USING bas_codegen.pl s_baltst.xsd -m msg -p s_baltst -C tmp_singles --msgSplit 1 --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2020 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
