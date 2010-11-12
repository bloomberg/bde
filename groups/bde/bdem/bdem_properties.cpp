// bdem_properties.cpp                  -*-C++-*-
#include <bdem_properties.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdem_properties_cpp,"$Id$ $CSID$")


#include <bdem_aggregateoption.h>
#include <bdem_elemtype.h>
#include <bdem_functiontemplates.h>

#include <bsls_alignmentfromtype.h>
#include <bsls_platformutil.h>

#include <bdet_datetime.h>
#include <bdet_datetimetz.h>
#include <bdet_date.h>
#include <bdet_datetz.h>
#include <bdet_time.h>
#include <bdet_timetz.h>

#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {

namespace {
    // Functions for the 'VOID' pseudo-type.

    void voidUnsetConstruct(void                                     *,
                            bdem_AggregateOption::AllocationStrategy  ,
                            bslma_Allocator                          *)
    {
    }

    void voidCopyConstruct(void                                     *,
                           const void                               *,
                           bdem_AggregateOption::AllocationStrategy  ,
                           bslma_Allocator                          *)
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
const bdem_Descriptor bdem_Properties::d_voidAttr = {
    bdem_ElemType::BDEM_VOID,                           // type
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
    bsls_AlignmentFromType<T>::VALUE,                   \
    &bdem_FunctionTemplates::unsetConstruct<T>,         \
    &bdem_FunctionTemplates::copyConstruct<T>,          \
    &bdem_FunctionTemplates::destroy<T>,                \
    &bdem_FunctionTemplates::assign<T>,                 \
    &bdem_FunctionTemplates::bitwiseMove<T>,            \
    &bdem_FunctionTemplates::makeUnset<T>,              \
    &bdem_FunctionTemplates::isUnset<T>,                \
    &bdem_FunctionTemplates::areEqual<T>,               \
    &bdem_FunctionTemplates::print<T>

// Array types use 'defaultConstruct' instead of 'unsetConstruct', 'removeAll'
// instead of 'makeUnset', and 'isEmpty' instead of 'isUnset'.
#define BDEM_ARRAY_DESCRIPTOR_INIT(T, ENUM)             \
    ENUM,                                               \
    sizeof(T),                                          \
    bsls_AlignmentFromType<T >::VALUE,                  \
    &bdem_FunctionTemplates::defaultConstruct<T >,      \
    &bdem_FunctionTemplates::copyConstruct<T >,         \
    &bdem_FunctionTemplates::destroy<T >,               \
    &bdem_FunctionTemplates::assign<T >,                \
    &bdem_FunctionTemplates::bitwiseMove<T >,           \
    &bdem_FunctionTemplates::removeAll<T >,             \
    &bdem_FunctionTemplates::isEmpty<T >,               \
    &bdem_FunctionTemplates::areEqual<T >,              \
    &bdem_FunctionTemplates::printArray<T >

const bdem_Descriptor bdem_Properties::d_boolAttr = {
    BDEM_SCALAR_DESCRIPTOR_INIT(bool, bdem_ElemType::BDEM_BOOL)
};

const bdem_Descriptor bdem_Properties::d_charAttr = {
    BDEM_SCALAR_DESCRIPTOR_INIT(char, bdem_ElemType::BDEM_CHAR)
};

const bdem_Descriptor bdem_Properties::d_shortAttr = {
    BDEM_SCALAR_DESCRIPTOR_INIT(short, bdem_ElemType::BDEM_SHORT)
};

const bdem_Descriptor bdem_Properties::d_intAttr = {
    BDEM_SCALAR_DESCRIPTOR_INIT(int, bdem_ElemType::BDEM_INT)
};

const bdem_Descriptor bdem_Properties::d_int64Attr = {
    BDEM_SCALAR_DESCRIPTOR_INIT(bsls_PlatformUtil::Int64,
                                bdem_ElemType::BDEM_INT64)
};

const bdem_Descriptor bdem_Properties::d_floatAttr = {
    BDEM_SCALAR_DESCRIPTOR_INIT(float, bdem_ElemType::BDEM_FLOAT)
};

const bdem_Descriptor bdem_Properties::d_doubleAttr = {
    BDEM_SCALAR_DESCRIPTOR_INIT(double, bdem_ElemType::BDEM_DOUBLE)
};

const bdem_Descriptor bdem_Properties::d_stringAttr = {
    BDEM_SCALAR_DESCRIPTOR_INIT(bsl::string, bdem_ElemType::BDEM_STRING)
};

const bdem_Descriptor bdem_Properties::d_datetimeAttr = {
    BDEM_SCALAR_DESCRIPTOR_INIT(bdet_Datetime, bdem_ElemType::BDEM_DATETIME)
};

const bdem_Descriptor bdem_Properties::d_datetimeTzAttr = {
    BDEM_SCALAR_DESCRIPTOR_INIT(bdet_DatetimeTz,
                                bdem_ElemType::BDEM_DATETIMETZ)
};

const bdem_Descriptor bdem_Properties::d_dateAttr = {
    BDEM_SCALAR_DESCRIPTOR_INIT(bdet_Date, bdem_ElemType::BDEM_DATE)
};

const bdem_Descriptor bdem_Properties::d_dateTzAttr = {
    BDEM_SCALAR_DESCRIPTOR_INIT(bdet_DateTz, bdem_ElemType::BDEM_DATETZ)
};

const bdem_Descriptor bdem_Properties::d_timeAttr = {
    BDEM_SCALAR_DESCRIPTOR_INIT(bdet_Time, bdem_ElemType::BDEM_TIME)
};

const bdem_Descriptor bdem_Properties::d_timeTzAttr = {
    BDEM_SCALAR_DESCRIPTOR_INIT(bdet_TimeTz, bdem_ElemType::BDEM_TIMETZ)
};

const bdem_Descriptor bdem_Properties::d_boolArrayAttr = {
    BDEM_ARRAY_DESCRIPTOR_INIT(bsl::vector<bool>,
                               bdem_ElemType::BDEM_BOOL_ARRAY)
};

const bdem_Descriptor bdem_Properties::d_charArrayAttr = {
    BDEM_ARRAY_DESCRIPTOR_INIT(bsl::vector<char>,
                               bdem_ElemType::BDEM_CHAR_ARRAY)
};

const bdem_Descriptor bdem_Properties::d_shortArrayAttr = {
    BDEM_ARRAY_DESCRIPTOR_INIT(bsl::vector<short>,
                               bdem_ElemType::BDEM_SHORT_ARRAY)
};

const bdem_Descriptor bdem_Properties::d_intArrayAttr = {
    BDEM_ARRAY_DESCRIPTOR_INIT(bsl::vector<int>, bdem_ElemType::BDEM_INT_ARRAY)
};

const bdem_Descriptor bdem_Properties::d_int64ArrayAttr = {
    BDEM_ARRAY_DESCRIPTOR_INIT(bsl::vector<bsls_PlatformUtil::Int64>,
                               bdem_ElemType::BDEM_INT64_ARRAY)
};

const bdem_Descriptor bdem_Properties::d_floatArrayAttr = {
    BDEM_ARRAY_DESCRIPTOR_INIT(bsl::vector<float>,
                               bdem_ElemType::BDEM_FLOAT_ARRAY)
};

const bdem_Descriptor bdem_Properties::d_doubleArrayAttr = {
    BDEM_ARRAY_DESCRIPTOR_INIT(bsl::vector<double>,
                               bdem_ElemType::BDEM_DOUBLE_ARRAY)
};

const bdem_Descriptor bdem_Properties::d_stringArrayAttr = {
    BDEM_ARRAY_DESCRIPTOR_INIT(bsl::vector<bsl::string>,
                               bdem_ElemType::BDEM_STRING_ARRAY)
};

const bdem_Descriptor bdem_Properties::d_datetimeArrayAttr = {
    BDEM_ARRAY_DESCRIPTOR_INIT(bsl::vector<bdet_Datetime>,
                               bdem_ElemType::BDEM_DATETIME_ARRAY)
};

const bdem_Descriptor bdem_Properties::d_datetimeTzArrayAttr = {
    BDEM_ARRAY_DESCRIPTOR_INIT(bsl::vector<bdet_DatetimeTz>,
                               bdem_ElemType::BDEM_DATETIMETZ_ARRAY)
};

const bdem_Descriptor bdem_Properties::d_dateArrayAttr = {
    BDEM_ARRAY_DESCRIPTOR_INIT(bsl::vector<bdet_Date>,
                               bdem_ElemType::BDEM_DATE_ARRAY)
};

const bdem_Descriptor bdem_Properties::d_dateTzArrayAttr = {
    BDEM_ARRAY_DESCRIPTOR_INIT(bsl::vector<bdet_DateTz>,
                               bdem_ElemType::BDEM_DATETZ_ARRAY)
};

const bdem_Descriptor bdem_Properties::d_timeArrayAttr = {
    BDEM_ARRAY_DESCRIPTOR_INIT(bsl::vector<bdet_Time>,
                               bdem_ElemType::BDEM_TIME_ARRAY)
};

const bdem_Descriptor bdem_Properties::d_timeTzArrayAttr = {
    BDEM_ARRAY_DESCRIPTOR_INIT(bsl::vector<bdet_TimeTz>,
                               bdem_ElemType::BDEM_TIMETZ_ARRAY)
};

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
