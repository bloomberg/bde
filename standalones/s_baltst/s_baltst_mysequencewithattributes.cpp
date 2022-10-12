// s_baltst_mysequencewithattributes.cpp   *DO NOT EDIT*   @generated -*-C++-*-

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_mysequencewithattributes_cpp, "$Id$ $CSID$")

#include <s_baltst_mysequencewithattributes.h>

#include <bdlat_formattingmode.h>
#include <bdlat_valuetypefunctions.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>
#include <bdlb_string.h>

#include <bdlb_nullablevalue.h>
#include <bsl_string.h>
#include <bslim_printer.h>
#include <bsls_assert.h>

#include <bsl_cstring.h>
#include <bsl_iomanip.h>
#include <bsl_limits.h>
#include <bsl_ostream.h>
#include <bsl_utility.h>

namespace BloombergLP {
namespace s_baltst {

                       // ------------------------------
                       // class MySequenceWithAttributes
                       // ------------------------------

// CONSTANTS

const char MySequenceWithAttributes::CLASS_NAME[] = "MySequenceWithAttributes";

const bdlat_AttributeInfo MySequenceWithAttributes::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "attribute1",
        sizeof("attribute1") - 1,
        "",
        bdlat_FormattingMode::e_DEC
      | bdlat_FormattingMode::e_ATTRIBUTE
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE2,
        "attribute2",
        sizeof("attribute2") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
      | bdlat_FormattingMode::e_ATTRIBUTE
    },
    {
        ATTRIBUTE_ID_ELEMENT1,
        "element1",
        sizeof("element1") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_ELEMENT2,
        "element2",
        sizeof("element2") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *MySequenceWithAttributes::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 4; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    MySequenceWithAttributes::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *MySequenceWithAttributes::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
      case ATTRIBUTE_ID_ATTRIBUTE2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2];
      case ATTRIBUTE_ID_ELEMENT1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1];
      case ATTRIBUTE_ID_ELEMENT2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2];
      default:
        return 0;
    }
}

// CREATORS

MySequenceWithAttributes::MySequenceWithAttributes(bslma::Allocator *basicAllocator)
: d_attribute2(basicAllocator)
, d_element2(basicAllocator)
, d_attribute1()
, d_element1()
{
}

MySequenceWithAttributes::MySequenceWithAttributes(const MySequenceWithAttributes& original,
                                                   bslma::Allocator *basicAllocator)
: d_attribute2(original.d_attribute2, basicAllocator)
, d_element2(original.d_element2, basicAllocator)
, d_attribute1(original.d_attribute1)
, d_element1(original.d_element1)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
MySequenceWithAttributes::MySequenceWithAttributes(MySequenceWithAttributes&& original) noexcept
: d_attribute2(bsl::move(original.d_attribute2))
, d_element2(bsl::move(original.d_element2))
, d_attribute1(bsl::move(original.d_attribute1))
, d_element1(bsl::move(original.d_element1))
{
}

MySequenceWithAttributes::MySequenceWithAttributes(MySequenceWithAttributes&& original,
                                                   bslma::Allocator *basicAllocator)
: d_attribute2(bsl::move(original.d_attribute2), basicAllocator)
, d_element2(bsl::move(original.d_element2), basicAllocator)
, d_attribute1(bsl::move(original.d_attribute1))
, d_element1(bsl::move(original.d_element1))
{
}
#endif

MySequenceWithAttributes::~MySequenceWithAttributes()
{
}

// MANIPULATORS

MySequenceWithAttributes&
MySequenceWithAttributes::operator=(const MySequenceWithAttributes& rhs)
{
    if (this != &rhs) {
        d_attribute1 = rhs.d_attribute1;
        d_attribute2 = rhs.d_attribute2;
        d_element1 = rhs.d_element1;
        d_element2 = rhs.d_element2;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
MySequenceWithAttributes&
MySequenceWithAttributes::operator=(MySequenceWithAttributes&& rhs)
{
    if (this != &rhs) {
        d_attribute1 = bsl::move(rhs.d_attribute1);
        d_attribute2 = bsl::move(rhs.d_attribute2);
        d_element1 = bsl::move(rhs.d_element1);
        d_element2 = bsl::move(rhs.d_element2);
    }

    return *this;
}
#endif

void MySequenceWithAttributes::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_attribute1);
    bdlat_ValueTypeFunctions::reset(&d_attribute2);
    bdlat_ValueTypeFunctions::reset(&d_element1);
    bdlat_ValueTypeFunctions::reset(&d_element2);
}

// ACCESSORS

bsl::ostream& MySequenceWithAttributes::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("attribute1", this->attribute1());
    printer.printAttribute("attribute2", this->attribute2());
    printer.printAttribute("element1", this->element1());
    printer.printAttribute("element2", this->element2());
    printer.end();
    return stream;
}


}  // close package namespace
}  // close enterprise namespace

// GENERATED BY @BLP_BAS_CODEGEN_VERSION@
// USING bas_codegen.pl s_baltst_mysequencewithattributes.xsd --mode msg --includedir . --msgComponent mysequencewithattributes --noRecurse --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2022 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
