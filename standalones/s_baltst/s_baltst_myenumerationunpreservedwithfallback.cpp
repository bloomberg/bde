// s_baltst_myenumerationunpreservedwithfallback.cpp *DO NOT EDIT* @generated -*-C++-*-

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_myenumerationunpreservedwithfallback_cpp, "$Id$ $CSID$")

#include <s_baltst_myenumerationunpreservedwithfallback.h>

#include <bdlat_formattingmode.h>
#include <bdlat_valuetypefunctions.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>
#include <bdlb_string.h>

#include <bslim_printer.h>
#include <bsls_assert.h>

#include <bsl_cstring.h>
#include <bsl_iomanip.h>
#include <bsl_limits.h>
#include <bsl_ostream.h>
#include <bsl_utility.h>

namespace BloombergLP {
namespace s_baltst {

                 // ------------------------------------------
                 // class MyEnumerationUnpreservedWithFallback
                 // ------------------------------------------

// CONSTANTS

const char MyEnumerationUnpreservedWithFallback::CLASS_NAME[] = "MyEnumerationUnpreservedWithFallback";

const bdlat_EnumeratorInfo MyEnumerationUnpreservedWithFallback::ENUMERATOR_INFO_ARRAY[] = {
    {
        MyEnumerationUnpreservedWithFallback::UNKNOWN,
        "UNKNOWN",
        sizeof("UNKNOWN") - 1,
        ""
    },
    {
        MyEnumerationUnpreservedWithFallback::VALUE1,
        "VALUE1",
        sizeof("VALUE1") - 1,
        ""
    },
    {
        MyEnumerationUnpreservedWithFallback::VALUE2,
        "VALUE2",
        sizeof("VALUE2") - 1,
        ""
    }
};

// CLASS METHODS

int MyEnumerationUnpreservedWithFallback::fromInt(MyEnumerationUnpreservedWithFallback::Value *result, int number)
{
    switch (number) {
      case MyEnumerationUnpreservedWithFallback::UNKNOWN:
      case MyEnumerationUnpreservedWithFallback::VALUE1:
      case MyEnumerationUnpreservedWithFallback::VALUE2:
        *result = static_cast<MyEnumerationUnpreservedWithFallback::Value>(number);
        return 0;
      default:
        return -1;
    }
}

int MyEnumerationUnpreservedWithFallback::fromString(
        MyEnumerationUnpreservedWithFallback::Value *result,
        const char         *string,
        int                 stringLength)
{
    for (int i = 0; i < 3; ++i) {
        const bdlat_EnumeratorInfo& enumeratorInfo =
                    MyEnumerationUnpreservedWithFallback::ENUMERATOR_INFO_ARRAY[i];

        if (stringLength == enumeratorInfo.d_nameLength
        &&  0 == bsl::memcmp(enumeratorInfo.d_name_p, string, stringLength))
        {
            *result = static_cast<MyEnumerationUnpreservedWithFallback::Value>(enumeratorInfo.d_value);
            return 0;
        }
    }

    return -1;
}

const char *MyEnumerationUnpreservedWithFallback::toString(MyEnumerationUnpreservedWithFallback::Value value)
{
    switch (value) {
      case UNKNOWN: {
        return "UNKNOWN";
      }
      case VALUE1: {
        return "VALUE1";
      }
      case VALUE2: {
        return "VALUE2";
      }
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}


              // ------------------------------------------------
              // class MyEnumerationUnpreservedWithFallbackRecord
              // ------------------------------------------------

// CONSTANTS

const char MyEnumerationUnpreservedWithFallbackRecord::CLASS_NAME[] = "MyEnumerationUnpreservedWithFallbackRecord";

const bdlat_AttributeInfo MyEnumerationUnpreservedWithFallbackRecord::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_VALUE,
        "value",
        sizeof("value") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *MyEnumerationUnpreservedWithFallbackRecord::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 1; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    MyEnumerationUnpreservedWithFallbackRecord::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *MyEnumerationUnpreservedWithFallbackRecord::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_VALUE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALUE];
      default:
        return 0;
    }
}

// CREATORS

MyEnumerationUnpreservedWithFallbackRecord::MyEnumerationUnpreservedWithFallbackRecord()
: d_value(static_cast<MyEnumerationUnpreservedWithFallback::Value>(0))
{
}

MyEnumerationUnpreservedWithFallbackRecord::MyEnumerationUnpreservedWithFallbackRecord(const MyEnumerationUnpreservedWithFallbackRecord& original)
: d_value(original.d_value)
{
}

MyEnumerationUnpreservedWithFallbackRecord::~MyEnumerationUnpreservedWithFallbackRecord()
{
}

// MANIPULATORS

MyEnumerationUnpreservedWithFallbackRecord&
MyEnumerationUnpreservedWithFallbackRecord::operator=(const MyEnumerationUnpreservedWithFallbackRecord& rhs)
{
    if (this != &rhs) {
        d_value = rhs.d_value;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
MyEnumerationUnpreservedWithFallbackRecord&
MyEnumerationUnpreservedWithFallbackRecord::operator=(MyEnumerationUnpreservedWithFallbackRecord&& rhs)
{
    if (this != &rhs) {
        d_value = bsl::move(rhs.d_value);
    }

    return *this;
}
#endif

void MyEnumerationUnpreservedWithFallbackRecord::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_value);
}

// ACCESSORS

bsl::ostream& MyEnumerationUnpreservedWithFallbackRecord::print(
                                            bsl::ostream& stream,
                                            int           level,
                                            int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("value", this->value());
    printer.end();
    return stream;
}


}  // close package namespace
}  // close enterprise namespace

// GENERATED BY BLP_BAS_CODEGEN_2023.09.30
// USING bas_codegen.pl s_baltst_myenumerationunpreservedwithfallback.xsd --mode msg --includedir . --msgComponent myenumerationunpreservedwithfallback --noRecurse --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2023 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
