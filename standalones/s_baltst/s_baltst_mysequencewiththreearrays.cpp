// s_baltst_mysequencewiththreearrays.cpp  *DO NOT EDIT*   @generated -*-C++-*-

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_mysequencewiththreearrays_cpp, "$Id$ $CSID$")

#include <s_baltst_mysequencewiththreearrays.h>

#include <bdlat_formattingmode.h>
#include <bdlat_valuetypefunctions.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>
#include <bdlb_string.h>

#include <bsl_string.h>
#include <bsl_vector.h>
#include <bslim_printer.h>
#include <bsls_assert.h>

#include <bsl_cstring.h>
#include <bsl_iomanip.h>
#include <bsl_limits.h>
#include <bsl_ostream.h>
#include <bsl_utility.h>

namespace BloombergLP {
namespace s_baltst {

                      // -------------------------------
                      // class MySequenceWithThreeArrays
                      // -------------------------------

// CONSTANTS

const char MySequenceWithThreeArrays::CLASS_NAME[] = "MySequenceWithThreeArrays";

const bdlat_AttributeInfo MySequenceWithThreeArrays::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "attribute1",
        sizeof("attribute1") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE2,
        "attribute2",
        sizeof("attribute2") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE3,
        "attribute3",
        sizeof("attribute3") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *MySequenceWithThreeArrays::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 3; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    MySequenceWithThreeArrays::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *MySequenceWithThreeArrays::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
      case ATTRIBUTE_ID_ATTRIBUTE2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2];
      case ATTRIBUTE_ID_ATTRIBUTE3:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE3];
      default:
        return 0;
    }
}

// CREATORS

MySequenceWithThreeArrays::MySequenceWithThreeArrays(bslma::Allocator *basicAllocator)
: d_attribute2(basicAllocator)
, d_attribute3(basicAllocator)
, d_attribute1(basicAllocator)
{
}

MySequenceWithThreeArrays::MySequenceWithThreeArrays(const MySequenceWithThreeArrays& original,
                                                     bslma::Allocator *basicAllocator)
: d_attribute2(original.d_attribute2, basicAllocator)
, d_attribute3(original.d_attribute3, basicAllocator)
, d_attribute1(original.d_attribute1, basicAllocator)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
MySequenceWithThreeArrays::MySequenceWithThreeArrays(MySequenceWithThreeArrays&& original) noexcept
: d_attribute2(bsl::move(original.d_attribute2))
, d_attribute3(bsl::move(original.d_attribute3))
, d_attribute1(bsl::move(original.d_attribute1))
{
}

MySequenceWithThreeArrays::MySequenceWithThreeArrays(MySequenceWithThreeArrays&& original,
                                                     bslma::Allocator *basicAllocator)
: d_attribute2(bsl::move(original.d_attribute2), basicAllocator)
, d_attribute3(bsl::move(original.d_attribute3), basicAllocator)
, d_attribute1(bsl::move(original.d_attribute1), basicAllocator)
{
}
#endif

MySequenceWithThreeArrays::~MySequenceWithThreeArrays()
{
}

// MANIPULATORS

MySequenceWithThreeArrays&
MySequenceWithThreeArrays::operator=(const MySequenceWithThreeArrays& rhs)
{
    if (this != &rhs) {
        d_attribute1 = rhs.d_attribute1;
        d_attribute2 = rhs.d_attribute2;
        d_attribute3 = rhs.d_attribute3;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
MySequenceWithThreeArrays&
MySequenceWithThreeArrays::operator=(MySequenceWithThreeArrays&& rhs)
{
    if (this != &rhs) {
        d_attribute1 = bsl::move(rhs.d_attribute1);
        d_attribute2 = bsl::move(rhs.d_attribute2);
        d_attribute3 = bsl::move(rhs.d_attribute3);
    }

    return *this;
}
#endif

void MySequenceWithThreeArrays::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_attribute1);
    bdlat_ValueTypeFunctions::reset(&d_attribute2);
    bdlat_ValueTypeFunctions::reset(&d_attribute3);
}

// ACCESSORS

bsl::ostream& MySequenceWithThreeArrays::print(
                                            bsl::ostream& stream,
                                            int           level,
                                            int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("attribute1", this->attribute1());
    printer.printAttribute("attribute2", this->attribute2());
    printer.printAttribute("attribute3", this->attribute3());
    printer.end();
    return stream;
}


}  // close package namespace
}  // close enterprise namespace

// GENERATED BY BLP_BAS_CODEGEN_2025.08.21
// USING bas_codegen.pl s_baltst_mysequencewiththreearrays.xsd --mode msg --includedir . --msgComponent mysequencewiththreearrays --noRecurse --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2025 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
