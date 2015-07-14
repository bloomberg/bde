// bdlmxxx_properties.cpp                                                -*-C++-*-
#include <bdlmxxx_properties.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmxxx_properties_cpp,"$Id$ $CSID$")

#include <bdlmxxx_aggregateoption.h>
#include <bdlmxxx_elemtype.h>
#include <bdlmxxx_functiontemplates.h>

#include <bdlt_datetime.h>
#include <bdlt_datetimetz.h>
#include <bdlt_date.h>
#include <bdlt_datetz.h>
#include <bdlt_time.h>
#include <bdlt_timetz.h>

#include <bsls_alignmentfromtype.h>
#include <bsls_types.h>

#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {

namespace {
    // Functions for the 'VOID' pseudo-type.

    void voidUnsetConstruct(void                                     *,
                            bdlmxxx::AggregateOption::AllocationStrategy  ,
                            bslma::Allocator                         *)
    {
    }

    void voidCopyConstruct(void                                     *,
                           const void                               *,
                           bdlmxxx::AggregateOption::AllocationStrategy  ,
                           bslma::Allocator                         *)
    {
    }

    void voidDestroy(void *)
    {
    }

    void voidAssign(void *, const void *)
    {
    }

    void voidMove(void *, void *)
    {
    }

    void voidMakeUnset(void *)
    {
    }

    bool voidIsUnset(const void *)
    {
        return true;
    }

    bool voidAreEqual(const void *, const void *)
    {
        return true;
    }

    bsl::ostream& voidPrint(const void    *,
                            bsl::ostream&  stream,
                            int            ,
                            int            spacesPerLevel)
    {
        stream << "<Void>";
        if (0 <= spacesPerLevel) {
            stream << '\n';
        }
        return stream;
    }

}  // close unnamed namespace

// Initialization of descriptor for 'VOID'.
const bdlmxxx::Descriptor bdlmxxx::Properties::s_voidAttr = {
    bdlmxxx::ElemType::BDEM_VOID,                           // type
    0,                                                  // size
    1,                                                  // alignment
    &voidUnsetConstruct,
    &voidCopyConstruct,
    &voidDestroy,
    &voidAssign,
    &voidMove,
    &voidMakeUnset,
    &voidIsUnset,
    &voidAreEqual,
    &voidPrint
};

// Initialization of descriptor for scalar types, which use 'unsetConstruct'.
#define BDEM_SCALAR_DESCRIPTOR_INIT(T, ENUM)            \
    ENUM,                                               \
    sizeof(T),                                          \
    bsls::AlignmentFromType<T>::VALUE,                  \
    &bdlmxxx::FunctionTemplates::unsetConstruct<T>,         \
    &bdlmxxx::FunctionTemplates::copyConstruct<T>,          \
    &bdlmxxx::FunctionTemplates::destroy<T>,                \
    &bdlmxxx::FunctionTemplates::assign<T>,                 \
    &bdlmxxx::FunctionTemplates::bitwiseMove<T>,            \
    &bdlmxxx::FunctionTemplates::makeUnset<T>,              \
    &bdlmxxx::FunctionTemplates::isUnset<T>,                \
    &bdlmxxx::FunctionTemplates::areEqual<T>,               \
    &bdlmxxx::FunctionTemplates::print<T>

// Array types use 'defaultConstruct' instead of 'unsetConstruct', 'removeAll'
// instead of 'makeUnset', and 'isEmpty' instead of 'isUnset'.
#define BDEM_ARRAY_DESCRIPTOR_INIT(T, ENUM)             \
    ENUM,                                               \
    sizeof(T),                                          \
    bsls::AlignmentFromType<T >::VALUE,                 \
    &bdlmxxx::FunctionTemplates::defaultConstruct<T >,      \
    &bdlmxxx::FunctionTemplates::copyConstruct<T >,         \
    &bdlmxxx::FunctionTemplates::destroy<T >,               \
    &bdlmxxx::FunctionTemplates::assign<T >,                \
    &bdlmxxx::FunctionTemplates::bitwiseMove<T >,           \
    &bdlmxxx::FunctionTemplates::removeAll<T >,             \
    &bdlmxxx::FunctionTemplates::isEmpty<T >,               \
    &bdlmxxx::FunctionTemplates::areEqual<T >,              \
    &bdlmxxx::FunctionTemplates::printArray<T >

const bdlmxxx::Descriptor bdlmxxx::Properties::s_boolAttr = {
    BDEM_SCALAR_DESCRIPTOR_INIT(bool, bdlmxxx::ElemType::BDEM_BOOL)
};

const bdlmxxx::Descriptor bdlmxxx::Properties::s_charAttr = {
    BDEM_SCALAR_DESCRIPTOR_INIT(char, bdlmxxx::ElemType::BDEM_CHAR)
};

const bdlmxxx::Descriptor bdlmxxx::Properties::s_shortAttr = {
    BDEM_SCALAR_DESCRIPTOR_INIT(short, bdlmxxx::ElemType::BDEM_SHORT)
};

const bdlmxxx::Descriptor bdlmxxx::Properties::s_intAttr = {
    BDEM_SCALAR_DESCRIPTOR_INIT(int, bdlmxxx::ElemType::BDEM_INT)
};

const bdlmxxx::Descriptor bdlmxxx::Properties::s_int64Attr = {
    BDEM_SCALAR_DESCRIPTOR_INIT(bsls::Types::Int64, bdlmxxx::ElemType::BDEM_INT64)
};

const bdlmxxx::Descriptor bdlmxxx::Properties::s_floatAttr = {
    BDEM_SCALAR_DESCRIPTOR_INIT(float, bdlmxxx::ElemType::BDEM_FLOAT)
};

const bdlmxxx::Descriptor bdlmxxx::Properties::s_doubleAttr = {
    BDEM_SCALAR_DESCRIPTOR_INIT(double, bdlmxxx::ElemType::BDEM_DOUBLE)
};

const bdlmxxx::Descriptor bdlmxxx::Properties::s_stringAttr = {
    BDEM_SCALAR_DESCRIPTOR_INIT(bsl::string, bdlmxxx::ElemType::BDEM_STRING)
};

const bdlmxxx::Descriptor bdlmxxx::Properties::s_datetimeAttr = {
    BDEM_SCALAR_DESCRIPTOR_INIT(bdlt::Datetime, bdlmxxx::ElemType::BDEM_DATETIME)
};

const bdlmxxx::Descriptor bdlmxxx::Properties::s_datetimeTzAttr = {
    BDEM_SCALAR_DESCRIPTOR_INIT(bdlt::DatetimeTz,
                                bdlmxxx::ElemType::BDEM_DATETIMETZ)
};

const bdlmxxx::Descriptor bdlmxxx::Properties::s_dateAttr = {
    BDEM_SCALAR_DESCRIPTOR_INIT(bdlt::Date, bdlmxxx::ElemType::BDEM_DATE)
};

const bdlmxxx::Descriptor bdlmxxx::Properties::s_dateTzAttr = {
    BDEM_SCALAR_DESCRIPTOR_INIT(bdlt::DateTz, bdlmxxx::ElemType::BDEM_DATETZ)
};

const bdlmxxx::Descriptor bdlmxxx::Properties::s_timeAttr = {
    BDEM_SCALAR_DESCRIPTOR_INIT(bdlt::Time, bdlmxxx::ElemType::BDEM_TIME)
};

const bdlmxxx::Descriptor bdlmxxx::Properties::s_timeTzAttr = {
    BDEM_SCALAR_DESCRIPTOR_INIT(bdlt::TimeTz, bdlmxxx::ElemType::BDEM_TIMETZ)
};

const bdlmxxx::Descriptor bdlmxxx::Properties::s_boolArrayAttr = {
    BDEM_ARRAY_DESCRIPTOR_INIT(bsl::vector<bool>,
                               bdlmxxx::ElemType::BDEM_BOOL_ARRAY)
};

const bdlmxxx::Descriptor bdlmxxx::Properties::s_charArrayAttr = {
    BDEM_ARRAY_DESCRIPTOR_INIT(bsl::vector<char>,
                               bdlmxxx::ElemType::BDEM_CHAR_ARRAY)
};

const bdlmxxx::Descriptor bdlmxxx::Properties::s_shortArrayAttr = {
    BDEM_ARRAY_DESCRIPTOR_INIT(bsl::vector<short>,
                               bdlmxxx::ElemType::BDEM_SHORT_ARRAY)
};

const bdlmxxx::Descriptor bdlmxxx::Properties::s_intArrayAttr = {
    BDEM_ARRAY_DESCRIPTOR_INIT(bsl::vector<int>, bdlmxxx::ElemType::BDEM_INT_ARRAY)
};

const bdlmxxx::Descriptor bdlmxxx::Properties::s_int64ArrayAttr = {
    BDEM_ARRAY_DESCRIPTOR_INIT(bsl::vector<bsls::Types::Int64>,
                               bdlmxxx::ElemType::BDEM_INT64_ARRAY)
};

const bdlmxxx::Descriptor bdlmxxx::Properties::s_floatArrayAttr = {
    BDEM_ARRAY_DESCRIPTOR_INIT(bsl::vector<float>,
                               bdlmxxx::ElemType::BDEM_FLOAT_ARRAY)
};

const bdlmxxx::Descriptor bdlmxxx::Properties::s_doubleArrayAttr = {
    BDEM_ARRAY_DESCRIPTOR_INIT(bsl::vector<double>,
                               bdlmxxx::ElemType::BDEM_DOUBLE_ARRAY)
};

const bdlmxxx::Descriptor bdlmxxx::Properties::s_stringArrayAttr = {
    BDEM_ARRAY_DESCRIPTOR_INIT(bsl::vector<bsl::string>,
                               bdlmxxx::ElemType::BDEM_STRING_ARRAY)
};

const bdlmxxx::Descriptor bdlmxxx::Properties::s_datetimeArrayAttr = {
    BDEM_ARRAY_DESCRIPTOR_INIT(bsl::vector<bdlt::Datetime>,
                               bdlmxxx::ElemType::BDEM_DATETIME_ARRAY)
};

const bdlmxxx::Descriptor bdlmxxx::Properties::s_datetimeTzArrayAttr = {
    BDEM_ARRAY_DESCRIPTOR_INIT(bsl::vector<bdlt::DatetimeTz>,
                               bdlmxxx::ElemType::BDEM_DATETIMETZ_ARRAY)
};

const bdlmxxx::Descriptor bdlmxxx::Properties::s_dateArrayAttr = {
    BDEM_ARRAY_DESCRIPTOR_INIT(bsl::vector<bdlt::Date>,
                               bdlmxxx::ElemType::BDEM_DATE_ARRAY)
};

const bdlmxxx::Descriptor bdlmxxx::Properties::s_dateTzArrayAttr = {
    BDEM_ARRAY_DESCRIPTOR_INIT(bsl::vector<bdlt::DateTz>,
                               bdlmxxx::ElemType::BDEM_DATETZ_ARRAY)
};

const bdlmxxx::Descriptor bdlmxxx::Properties::s_timeArrayAttr = {
    BDEM_ARRAY_DESCRIPTOR_INIT(bsl::vector<bdlt::Time>,
                               bdlmxxx::ElemType::BDEM_TIME_ARRAY)
};

const bdlmxxx::Descriptor bdlmxxx::Properties::s_timeTzArrayAttr = {
    BDEM_ARRAY_DESCRIPTOR_INIT(bsl::vector<bdlt::TimeTz>,
                               bdlmxxx::ElemType::BDEM_TIMETZ_ARRAY)
};

// The identifiers below are DEPRECATED and are only provided for backward
// compatibility.  Each 'd_*' identifier is just a reference to the
// corresponding non-deprecated 's_*' identifier.  These identifiers will be
// eliminated at a future date.

const bdlmxxx::Descriptor& bdlmxxx::Properties::d_intAttr = bdlmxxx::Properties::s_intAttr;
const bdlmxxx::Descriptor& bdlmxxx::Properties::d_doubleAttr =
                                                 bdlmxxx::Properties::s_doubleAttr;
const bdlmxxx::Descriptor& bdlmxxx::Properties::d_stringAttr =
                                                 bdlmxxx::Properties::s_stringAttr;

const bdlmxxx::Descriptor& bdlmxxx::Properties::d_charArrayAttr =
                                              bdlmxxx::Properties::s_charArrayAttr;

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
