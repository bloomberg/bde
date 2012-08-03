// bcem_aggregateutil.cpp   -*-C++-*-
#include <bcem_aggregateutil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcem_aggregateutil_cpp,"$Id$ $CSID$")

namespace BloombergLP {

                          // ------------------------
                          // class bcem_AggregateUtil
                          // ------------------------

// PRIVATE CLASS METHODS

// Conversion From Primitive Types

int bcem_AggregateUtil::fromAggregatePrimitive(
                                      bool                       *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId)
{
    int rc = 0;
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        rc = value.errorCode();
    }
    else {
        *result = value.asBool();
    }

    return rc;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
                                      char                       *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId)
{
    int rc = 0;
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        rc = value.errorCode();
    }
    else {
        *result = value.asChar();
    }

    return rc;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
                                      short                      *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId)
{
    int rc = 0;
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        rc = value.errorCode();
    }
    else {
        *result = value.asShort();
    }

    return rc;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
                                      int                        *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId)
{
    int rc = 0;
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        rc = value.errorCode();
    }
    else {
        *result = value.asInt();
    }

    return rc;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
                                      float                      *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId)
{
    int rc = 0;
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        rc = value.errorCode();
    }
    else {
        *result = value.asFloat();
    }

    return rc;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
                                      double                     *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId)
{
    int rc = 0;
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        rc = value.errorCode();
    }
    else {
        *result = value.asDouble();
    }

    return rc;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
                                      unsigned char              *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId)
{
    int rc = 0;
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        rc = value.errorCode();
    }
    else {
        *result = static_cast<unsigned char>(value.asChar());
    }

    return rc;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
                                      unsigned short             *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId)
{
    int rc = 0;
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        rc = value.errorCode();
    }
    else {
        *result = static_cast<unsigned short>(value.asShort());
    }

    return rc;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
                                      unsigned int               *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId)
{
    int rc = 0;
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        rc = value.errorCode();
    }
    else {
        *result = static_cast<unsigned int>(value.asInt());
    }

    return rc;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
                                      bsl::string                *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId)
{
    int rc = 0;
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        rc = value.errorCode();
    }
    else {
        *result = value.asString();
    }

    return rc;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
                                      bsls_PlatformUtil::Int64   *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId)
{
    int rc = 0;
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        rc = value.errorCode();
    }
    else {
        *result = value.asInt64();
    }

    return rc;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
                                      bsls_PlatformUtil::Uint64  *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId)
{
    int rc = 0;
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        rc = value.errorCode();
    }
    else {
        *result = static_cast<bsls_PlatformUtil::Uint64>(value.asInt64());
    }

    return rc;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
                                      bdet_Date                  *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId)
{
    int rc = 0;
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        rc = value.errorCode();
    }
    else {
        *result = value.asDate();
    }

    return rc;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
                                      bdet_DateTz                *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId)
{
    int rc = 0;
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        rc = value.errorCode();
    }
    else {
        *result = value.asDateTz();
    }

    return rc;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
                                      bdet_Datetime              *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId)
{
    int rc = 0;
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        rc = value.errorCode();
    }
    else {
        *result = value.asDatetime();
    }

    return rc;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
                                      bdet_DatetimeTz            *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId)
{
    int rc = 0;
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        rc = value.errorCode();
    }
    else {
        *result = value.asDatetimeTz();
    }

    return rc;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
                                      bdet_Time                  *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId)
{
    int rc = 0;
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        rc = value.errorCode();
    }
    else {
        *result = value.asTime();
    }

    return rc;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
                                      bdet_TimeTz                *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId)
{
    int rc = 0;
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        rc = value.errorCode();
    }
    else {
        *result = value.asTimeTz();
    }

    return rc;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Conversion From Nullable Primitive Types

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableValue<bool>                      *result,
        const bcem_Aggregate&                           aggregate,
        int                                             fieldId)
{
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        return value.errorCode();
    }

//     if (false == value.fieldSpec()->isNullable()) {
//         return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
//     }

    if (value.isNul2()) {
        result->reset();
    }
    else {
        result->makeValue(value.asBool());
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableValue<char>                      *result,
        const bcem_Aggregate&                           aggregate,
        int                                             fieldId)
{
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        return value.errorCode();
    }

//     if (false == value.fieldSpec()->isNullable()) {
//         return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
//     }

    if (value.isNul2()) {
        result->reset();
    }
    else {
        result->makeValue(value.asChar());
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableValue<short>                     *result,
        const bcem_Aggregate&                           aggregate,
        int                                             fieldId)
{
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        return value.errorCode();
    }

//     if (false == value.fieldSpec()->isNullable()) {
//         return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
//     }

    if (value.isNul2()) {
        result->reset();
    }
    else {
        result->makeValue(value.asShort());
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableValue<int>                       *result,
        const bcem_Aggregate&                           aggregate,
        int                                             fieldId)
{
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        return value.errorCode();
    }

//     if (false == value.fieldSpec()->isNullable()) {
//         return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
//     }

    if (value.isNul2()) {
        result->reset();
    }
    else {
        result->makeValue(value.asInt());
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableValue<float>                     *result,
        const bcem_Aggregate&                           aggregate,
        int                                             fieldId)
{
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        return value.errorCode();
    }

//     if (false == value.fieldSpec()->isNullable()) {
//         return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
//     }

    if (value.isNul2()) {
        result->reset();
    }
    else {
        result->makeValue(value.asFloat());
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableValue<double>                    *result,
        const bcem_Aggregate&                           aggregate,
        int                                             fieldId)
{
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        return value.errorCode();
    }

//     if (false == value.fieldSpec()->isNullable()) {
//         return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
//     }

    if (value.isNul2()) {
        result->reset();
    }
    else {
        result->makeValue(value.asDouble());
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableValue<unsigned char>             *result,
        const bcem_Aggregate&                           aggregate,
        int                                             fieldId)
{
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        return value.errorCode();
    }

//     if (false == value.fieldSpec()->isNullable()) {
//         return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
//     }

    if (value.isNul2()) {
        result->reset();
    }
    else {
        result->makeValue(static_cast<unsigned char>(value.asChar()));
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableValue<unsigned short>            *result,
        const bcem_Aggregate&                           aggregate,
        int                                             fieldId)
{
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        return value.errorCode();
    }

//     if (false == value.fieldSpec()->isNullable()) {
//         return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
//     }

    if (value.isNul2()) {
        result->reset();
    }
    else {
        result->makeValue(static_cast<unsigned short>(value.asShort()));
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableValue<unsigned int>              *result,
        const bcem_Aggregate&                           aggregate,
        int                                             fieldId)
{
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        return value.errorCode();
    }

//     if (false == value.fieldSpec()->isNullable()) {
//         return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
//     }

    if (value.isNul2()) {
        result->reset();
    }
    else {
        result->makeValue(static_cast<unsigned int>(value.asInt()));
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableValue<bsl::string>               *result,
        const bcem_Aggregate&                           aggregate,
        int                                             fieldId)
{
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        return value.errorCode();
    }

//     if (false == value.fieldSpec()->isNullable()) {
//         return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
//     }

    if (value.isNul2()) {
        result->reset();
    }
    else {
        result->makeValue(value.asString());
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableValue<bsls_PlatformUtil::Int64>  *result,
        const bcem_Aggregate&                           aggregate,
        int                                             fieldId)
{
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        return value.errorCode();
    }

//     if (false == value.fieldSpec()->isNullable()) {
//         return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
//     }

    if (value.isNul2()) {
        result->reset();
    }
    else {
        result->makeValue(value.asInt64());
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableValue<bsls_PlatformUtil::Uint64> *result,
        const bcem_Aggregate&                           aggregate,
        int                                             fieldId)
{
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        return value.errorCode();
    }

//     if (false == value.fieldSpec()->isNullable()) {
//         return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
//     }

    if (value.isNul2()) {
        result->reset();
    }
    else {
        typedef bsls_PlatformUtil::Uint64 Uint64;
        result->makeValue(static_cast<Uint64>(value.asInt64()));
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableValue<bdet_DateTz>               *result,
        const bcem_Aggregate&                           aggregate,
        int                                             fieldId)
{
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        return value.errorCode();
    }

//     if (false == value.fieldSpec()->isNullable()) {
//         return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
//     }

    if (value.isNul2()) {
        result->reset();
    }
    else {
        result->makeValue(value.asDateTz());
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableValue<bdet_DatetimeTz>           *result,
        const bcem_Aggregate&                           aggregate,
        int                                             fieldId)
{
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        return value.errorCode();
    }

//     if (false == value.fieldSpec()->isNullable()) {
//         return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
//     }

    if (value.isNul2()) {
        result->reset();
    }
    else {
        result->makeValue(value.asDatetimeTz());
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableValue<bdet_TimeTz>               *result,
        const bcem_Aggregate&                           aggregate,
        int                                             fieldId)
{
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        return value.errorCode();
    }

//     if (false == value.fieldSpec()->isNullable()) {
//         return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
//     }

    if (value.isNul2()) {
        result->reset();
    }
    else {
        result->makeValue(value.asTimeTz());
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableValue<bdet_Date>                 *result,
        const bcem_Aggregate&                           aggregate,
        int                                             fieldId)
{
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        return value.errorCode();
    }

//     if (false == value.fieldSpec()->isNullable()) {
//         return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
//     }

    if (value.isNul2()) {
        result->reset();
    }
    else {
        result->makeValue(value.asDate());
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableValue<bdet_Datetime>             *result,
        const bcem_Aggregate&                           aggregate,
        int                                             fieldId)
{
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        return value.errorCode();
    }

//     if (false == value.fieldSpec()->isNullable()) {
//         return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
//     }

    if (value.isNul2()) {
        result->reset();
    }
    else {
        result->makeValue(value.asDatetime());
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableValue<bdet_Time>                 *result,
        const bcem_Aggregate&                           aggregate,
        int                                             fieldId)
{
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        return value.errorCode();
    }

//     if (false == value.fieldSpec()->isNullable()) {
//         return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
//     }

    if (value.isNul2()) {
        result->reset();
    }
    else {
        result->makeValue(value.asTime());
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
            bdeut_NullableValue< bsl::vector<char> >       *result,
            const bcem_Aggregate&                           aggregate,
            int                                             fieldId)
{
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        return value.errorCode();
    }

//     if (false == value.fieldSpec()->isNullable()) {
//         return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
//     }

    if (value.isNul2()) {
        result->reset();
    }
    else {
        result->makeValue(value.asElemRef().theCharArray());
    }

    return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Conversion From Nullable-Allocated Primitive Types

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableAllocatedValue<bool>                      *result,
        const bcem_Aggregate&                                    aggregate,
        int                                                      fieldId)
{
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        return value.errorCode();
    }

//     if (false == value.fieldSpec()->isNullable()) {
//         return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
//     }

    if (value.isNul2()) {
        result->reset();
    }
    else {
        result->makeValue(value.asBool());
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableAllocatedValue<char>                      *result,
        const bcem_Aggregate&                                    aggregate,
        int                                                      fieldId)
{
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        return value.errorCode();
    }

//     if (false == value.fieldSpec()->isNullable()) {
//         return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
//     }

    if (value.isNul2()) {
        result->reset();
    }
    else {
        result->makeValue(value.asChar());
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableAllocatedValue<short>                     *result,
        const bcem_Aggregate&                                    aggregate,
        int                                                      fieldId)
{
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        return value.errorCode();
    }

//     if (false == value.fieldSpec()->isNullable()) {
//         return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
//     }

    if (value.isNul2()) {
        result->reset();
    }
    else {
        result->makeValue(value.asShort());
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableAllocatedValue<int>                       *result,
        const bcem_Aggregate&                                    aggregate,
        int                                                      fieldId)
{
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        return value.errorCode();
    }

//     if (false == value.fieldSpec()->isNullable()) {
//         return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
//     }

    if (value.isNul2()) {
        result->reset();
    }
    else {
        result->makeValue(value.asInt());
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableAllocatedValue<float>                     *result,
        const bcem_Aggregate&                                    aggregate,
        int                                                      fieldId)
{
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        return value.errorCode();
    }

//     if (false == value.fieldSpec()->isNullable()) {
//         return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
//     }

    if (value.isNul2()) {
        result->reset();
    }
    else {
        result->makeValue(value.asFloat());
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableAllocatedValue<double>                    *result,
        const bcem_Aggregate&                                    aggregate,
        int                                                      fieldId)
{
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        return value.errorCode();
    }

//     if (false == value.fieldSpec()->isNullable()) {
//         return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
//     }

    if (value.isNul2()) {
        result->reset();
    }
    else {
        result->makeValue(value.asDouble());
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableAllocatedValue<unsigned char>             *result,
        const bcem_Aggregate&                                    aggregate,
        int                                                      fieldId)
{
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        return value.errorCode();
    }

//     if (false == value.fieldSpec()->isNullable()) {
//         return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
//     }

    if (value.isNul2()) {
        result->reset();
    }
    else {
        result->makeValue(static_cast<unsigned char>(value.asChar()));
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableAllocatedValue<unsigned short>            *result,
        const bcem_Aggregate&                                    aggregate,
        int                                                      fieldId)
{
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        return value.errorCode();
    }

//     if (false == value.fieldSpec()->isNullable()) {
//         return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
//     }

    if (value.isNul2()) {
        result->reset();
    }
    else {
        result->makeValue(static_cast<unsigned short>(value.asShort()));
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableAllocatedValue<unsigned int>              *result,
        const bcem_Aggregate&                                    aggregate,
        int                                                      fieldId)
{
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        return value.errorCode();
    }

//     if (false == value.fieldSpec()->isNullable()) {
//         return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
//     }

    if (value.isNul2()) {
        result->reset();
    }
    else {
        result->makeValue(static_cast<unsigned int>(value.asInt()));
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableAllocatedValue<bsl::string>               *result,
        const bcem_Aggregate&                                    aggregate,
        int                                                      fieldId)
{
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        return value.errorCode();
    }

//     if (false == value.fieldSpec()->isNullable()) {
//         return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
//     }

    if (value.isNul2()) {
        result->reset();
    }
    else {
        result->makeValue(value.asString());
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableAllocatedValue<bsls_PlatformUtil::Int64>  *result,
        const bcem_Aggregate&                                    aggregate,
        int                                                      fieldId)
{
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        return value.errorCode();
    }

//     if (false == value.fieldSpec()->isNullable()) {
//         return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
//     }

    if (value.isNul2()) {
        result->reset();
    }
    else {
        result->makeValue(value.asInt64());
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableAllocatedValue<bsls_PlatformUtil::Uint64> *result,
        const bcem_Aggregate&                                    aggregate,
        int                                                      fieldId)
{
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        return value.errorCode();
    }

//     if (false == value.fieldSpec()->isNullable()) {
//         return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
//     }

    if (value.isNul2()) {
        result->reset();
    }
    else {
        typedef bsls_PlatformUtil::Uint64 Uint64;
        result->makeValue(static_cast<Uint64>(value.asInt64()));
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableAllocatedValue<bdet_DateTz>               *result,
        const bcem_Aggregate&                                    aggregate,
        int                                                      fieldId)
{
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        return value.errorCode();
    }

//     if (false == value.fieldSpec()->isNullable()) {
//         return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
//     }

    if (value.isNul2()) {
        result->reset();
    }
    else {
        result->makeValue(value.asDateTz());
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableAllocatedValue<bdet_DatetimeTz>           *result,
        const bcem_Aggregate&                                    aggregate,
        int                                                      fieldId)
{
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        return value.errorCode();
    }

//     if (false == value.fieldSpec()->isNullable()) {
//         return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
//     }

    if (value.isNul2()) {
        result->reset();
    }
    else {
        result->makeValue(value.asDatetimeTz());
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableAllocatedValue<bdet_TimeTz>               *result,
        const bcem_Aggregate&                                    aggregate,
        int                                                      fieldId)
{
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        return value.errorCode();
    }

//     if (false == value.fieldSpec()->isNullable()) {
//         return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
//     }

    if (value.isNul2()) {
        result->reset();
    }
    else {
        result->makeValue(value.asTimeTz());
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableAllocatedValue<bdet_Date>                 *result,
        const bcem_Aggregate&                                    aggregate,
        int                                                      fieldId)
{
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        return value.errorCode();
    }

//     if (false == value.fieldSpec()->isNullable()) {
//         return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
//     }

    if (value.isNul2()) {
        result->reset();
    }
    else {
        result->makeValue(value.asDate());
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableAllocatedValue<bdet_Datetime>             *result,
        const bcem_Aggregate&                                    aggregate,
        int                                                      fieldId)
{
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        return value.errorCode();
    }

//     if (false == value.fieldSpec()->isNullable()) {
//         return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
//     }

    if (value.isNul2()) {
        result->reset();
    }
    else {
        result->makeValue(value.asDatetime());
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableAllocatedValue<bdet_Time>                 *result,
        const bcem_Aggregate&                                    aggregate,
        int                                                      fieldId)
{
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        return value.errorCode();
    }

//     if (false == value.fieldSpec()->isNullable()) {
//         return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
//     }

    if (value.isNul2()) {
        result->reset();
    }
    else {
        result->makeValue(value.asTime());
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
            bdeut_NullableAllocatedValue< bsl::vector<char> >       *result,
            const bcem_Aggregate&                                    aggregate,
            int                                                      fieldId)
{
    bcem_Aggregate value = aggregate.fieldById(fieldId);
    if (value.isError()) {
        return value.errorCode();
    }

//     if (false == value.fieldSpec()->isNullable()) {
//         return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
//     }

    if (value.isNul2()) {
        result->reset();
    }
    else {
        result->makeValue(value.asElemRef().theCharArray());
    }

    return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Conversion From Arrays Of Primitive Types

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector<bool>                      *result,
        const bcem_Aggregate&                   aggregate,
        int                                     fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    if (bdem_ElemType::BDEM_BOOL_ARRAY != array.dataType()) {
        return -1;
    }
    *result = array.asElemRef().theBoolArray();

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector<char>                      *result,
        const bcem_Aggregate&                   aggregate,
        int                                     fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    if (bdem_ElemType::BDEM_CHAR_ARRAY != array.dataType()) {
        return -1;
    }
    *result = array.asElemRef().theCharArray();

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector<short>                     *result,
        const bcem_Aggregate&                   aggregate,
        int                                     fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    if (bdem_ElemType::BDEM_SHORT_ARRAY != array.dataType()) {
        return -1;
    }
    *result = array.asElemRef().theShortArray();

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector<int>                       *result,
        const bcem_Aggregate&                   aggregate,
        int                                     fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    if (bdem_ElemType::BDEM_INT_ARRAY != array.dataType()) {
        return -1;
    }
    *result = array.asElemRef().theIntArray();

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector<float>                     *result,
        const bcem_Aggregate&                   aggregate,
        int                                     fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    if (bdem_ElemType::BDEM_FLOAT_ARRAY != array.dataType()) {
        return -1;
    }
    *result = array.asElemRef().theFloatArray();

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector<double>                    *result,
        const bcem_Aggregate&                   aggregate,
        int                                     fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    if (bdem_ElemType::BDEM_DOUBLE_ARRAY != array.dataType()) {
        return -1;
    }
    *result = array.asElemRef().theDoubleArray();

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector<unsigned char>             *result,
        const bcem_Aggregate&                   aggregate,
        int                                     fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    if (bdem_ElemType::BDEM_CHAR_ARRAY == array.dataType()) {
        typedef bsl::vector<unsigned char> Vector;

        const Vector *vector =
           reinterpret_cast<const Vector *>(&array.asElemRef().theCharArray());
        *result = *vector;

        return 0;                                                     // RETURN
    }

    const int length = array.length();
    if (length < 0) {
        // The 'length' method returned an error code instead of a length.
        return length;                                                // RETURN
    }

    result->clear();
    result->reserve(length);
    for (int i = 0; i < length; ++i) {
        result->push_back(static_cast<unsigned char>(array[i].asChar()));
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector<unsigned short>            *result,
        const bcem_Aggregate&                   aggregate,
        int                                     fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    if (bdem_ElemType::BDEM_SHORT_ARRAY == array.dataType()) {
        typedef bsl::vector<unsigned short> Vector;

        const Vector *vector =
          reinterpret_cast<const Vector *>(&array.asElemRef().theShortArray());
        *result = *vector;

        return 0;                                                     // RETURN
    }

    const int length = array.length();
    if (length < 0) {
        // The 'length' method returned an error code instead of a length.
        return length;                                                // RETURN
    }

    result->clear();
    result->reserve(length);
    for (int i = 0; i < length; ++i) {
        result->push_back(static_cast<unsigned short>(array[i].asShort()));
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector<unsigned int>              *result,
        const bcem_Aggregate&                   aggregate,
        int                                     fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    if (bdem_ElemType::BDEM_INT_ARRAY == array.dataType()) {
        typedef bsl::vector<unsigned int> Vector;

        const Vector *vector =
            reinterpret_cast<const Vector *>(&array.asElemRef().theIntArray());
        *result = *vector;

        return 0;                                                     // RETURN
    }

    const int length = array.length();
    if (length < 0) {
        // The 'length' method returned an error code instead of a length.
        return length;                                                // RETURN
    }

    result->clear();
    result->reserve(length);
    for (int i = 0; i < length; ++i) {
        result->push_back(static_cast<unsigned int>(array[i].asInt()));
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector<bsl::string>               *result,
        const bcem_Aggregate&                   aggregate,
        int                                     fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    if (bdem_ElemType::BDEM_STRING_ARRAY != array.dataType()) {
        return -1;
    }
    *result = array.asElemRef().theStringArray();

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector<bsls_PlatformUtil::Int64>  *result,
        const bcem_Aggregate&                   aggregate,
        int                                     fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    if (bdem_ElemType::BDEM_INT64_ARRAY != array.dataType()) {
        return -1;
    }
    *result = array.asElemRef().theInt64Array();

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector<bsls_PlatformUtil::Uint64> *result,
        const bcem_Aggregate&                   aggregate,
        int                                     fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    if (bdem_ElemType::BDEM_INT64_ARRAY != array.dataType()) {
        return -1;
    }
    typedef bsl::vector<bsls_PlatformUtil::Uint64> Vector;

    const Vector *vector =
          reinterpret_cast<const Vector *>(&array.asElemRef().theInt64Array());
    *result = *vector;

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector<bdet_DateTz>               *result,
        const bcem_Aggregate&                   aggregate,
        int                                     fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    if (bdem_ElemType::BDEM_DATETZ_ARRAY != array.dataType()) {
        return -1;
    }
    *result = array.asElemRef().theDateTzArray();

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector<bdet_DatetimeTz>           *result,
        const bcem_Aggregate&                   aggregate,
        int                                     fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    if (bdem_ElemType::BDEM_DATETIMETZ_ARRAY != array.dataType()) {
        return -1;
    }
    *result = array.asElemRef().theDatetimeTzArray();

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector<bdet_TimeTz>               *result,
        const bcem_Aggregate&                   aggregate,
        int                                     fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    if (bdem_ElemType::BDEM_TIMETZ_ARRAY != array.dataType()) {
        return -1;
    }
    *result = array.asElemRef().theTimeTzArray();

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector<bdet_Date>                 *result,
        const bcem_Aggregate&                   aggregate,
        int                                     fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    if (bdem_ElemType::BDEM_DATE_ARRAY != array.dataType()) {
        return -1;
    }
    *result = array.asElemRef().theDateArray();

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector<bdet_Datetime>             *result,
        const bcem_Aggregate&                   aggregate,
        int                                     fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    if (bdem_ElemType::BDEM_DATETIME_ARRAY != array.dataType()) {
        return -1;
    }
    *result = array.asElemRef().theDatetimeArray();

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector<bdet_Time>                 *result,
        const bcem_Aggregate&                   aggregate,
        int                                     fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    if (bdem_ElemType::BDEM_TIME_ARRAY != array.dataType()) {
        return -1;
    }

    *result = array.asElemRef().theTimeArray();

    return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Conversion From Arrays Of Nullable Primitive Types

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableValue<bool> > *result,
        const bcem_Aggregate&                     aggregate,
        int                                       fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    int length = array.length();
    if (length < 0) {
        // The 'length()' method returned an error code instead of a length
        return length;
    }

    result->clear();
    result->reserve(length);
    for (int i = 0; i < length; ++i) {
        bcem_Aggregate fieldElement = array[i];
        if (fieldElement.isNul2()) {
            result->push_back(bdeut_NullableValue<bool>());
        }
        else {
            result->push_back(fieldElement.asBool());
        }
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableValue<char> > *result,
        const bcem_Aggregate&                     aggregate,
        int                                       fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    int length = array.length();
    if (length < 0) {
        // The 'length()' method returned an error code instead of a length
        return length;
    }

    result->clear();
    result->reserve(length);
    for (int i = 0; i < length; ++i) {
        bcem_Aggregate fieldElement = array[i];
        if (fieldElement.isNul2()) {
            result->push_back(bdeut_NullableValue<char>());
        }
        else {
            result->push_back(fieldElement.asChar());
        }
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableValue<short> > *result,
        const bcem_Aggregate&                      aggregate,
        int                                        fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    int length = array.length();
    if (length < 0) {
        // The 'length()' method returned an error code instead of a length
        return length;
    }

    result->clear();
    result->reserve(length);
    for (int i = 0; i < length; ++i) {
        bcem_Aggregate fieldElement = array[i];
        if (fieldElement.isNul2()) {
            result->push_back(bdeut_NullableValue<short>());
        }
        else {
            result->push_back(fieldElement.asShort());
        }
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableValue<int> > *result,
        const bcem_Aggregate&                    aggregate,
        int                                      fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    int length = array.length();
    if (length < 0) {
        // The 'length()' method returned an error code instead of a length
        return length;
    }

    result->clear();
    result->reserve(length);
    for (int i = 0; i < length; ++i) {
        bcem_Aggregate fieldElement = array[i];
        if (fieldElement.isNul2()) {
            result->push_back(bdeut_NullableValue<int>());
        }
        else {
            result->push_back(fieldElement.asInt());
        }
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableValue<float> > *result,
        const bcem_Aggregate&                      aggregate,
        int                                        fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    int length = array.length();
    if (length < 0) {
        // The 'length()' method returned an error code instead of a length
        return length;
    }

    result->clear();
    result->reserve(length);
    for (int i = 0; i < length; ++i) {
        bcem_Aggregate fieldElement = array[i];
        if (fieldElement.isNul2()) {
            result->push_back(bdeut_NullableValue<float>());
        }
        else {
            result->push_back(fieldElement.asFloat());
        }
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableValue<double> > *result,
        const bcem_Aggregate&                       aggregate,
        int                                         fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    int length = array.length();
    if (length < 0) {
        // The 'length()' method returned an error code instead of a length
        return length;
    }

    result->clear();
    result->reserve(length);
    for (int i = 0; i < length; ++i) {
        bcem_Aggregate fieldElement = array[i];
        if (fieldElement.isNul2()) {
            result->push_back(bdeut_NullableValue<double>());
        }
        else {
            result->push_back(fieldElement.asDouble());
        }
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableValue<unsigned char> > *result,
        const bcem_Aggregate&                              aggregate,
        int                                                fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    int length = array.length();
    if (length < 0) {
        // The 'length()' method returned an error code instead of a length
        return length;
    }

    result->clear();
    result->reserve(length);
    for (int i = 0; i < length; ++i) {
        bcem_Aggregate fieldElement = array[i];
        if (fieldElement.isNul2()) {
            result->push_back(bdeut_NullableValue<unsigned char>());
        }
        else {
            result->push_back((unsigned char)fieldElement.asChar());
        }
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableValue<unsigned short> > *result,
        const bcem_Aggregate&                               aggregate,
        int                                                 fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    int length = array.length();
    if (length < 0) {
        // The 'length()' method returned an error code instead of a length
        return length;
    }

    result->clear();
    result->reserve(length);
    for (int i = 0; i < length; ++i) {
        bcem_Aggregate fieldElement = array[i];
        if (fieldElement.isNul2()) {
            result->push_back(bdeut_NullableValue<unsigned short>());
        }
        else {
            result->push_back((unsigned short)fieldElement.asShort());
        }
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableValue<unsigned int> > *result,
        const bcem_Aggregate&                             aggregate,
        int                                               fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    int length = array.length();
    if (length < 0) {
        // The 'length()' method returned an error code instead of a length
        return length;
    }

    result->clear();
    result->reserve(length);
    for (int i = 0; i < length; ++i) {
        bcem_Aggregate fieldElement = array[i];
        if (fieldElement.isNul2()) {
            result->push_back(bdeut_NullableValue<unsigned int>());
        }
        else {
            result->push_back((unsigned int)fieldElement.asInt());
        }
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableValue<bsl::string> > *result,
        const bcem_Aggregate&                            aggregate,
        int                                              fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    int length = array.length();
    if (length < 0) {
        // The 'length()' method returned an error code instead of a length
        return length;
    }

    result->clear();
    result->reserve(length);
    for (int i = 0; i < length; ++i) {
        bcem_Aggregate fieldElement = array[i];
        if (fieldElement.isNul2()) {
            result->push_back(bdeut_NullableValue<bsl::string>());
        }
        else {
            result->push_back(fieldElement.asString());
        }
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableValue<bsls_PlatformUtil::Int64> > *result,
        const bcem_Aggregate&                                        aggregate,
        int                                                          fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    int length = array.length();
    if (length < 0) {
        // The 'length()' method returned an error code instead of a length
        return length;
    }

    result->clear();
    result->reserve(length);
    for (int i = 0; i < length; ++i) {
        bcem_Aggregate fieldElement = array[i];
        if (fieldElement.isNul2()) {
            result->push_back(bdeut_NullableValue<bsls_PlatformUtil::Int64>());
        }
        else {
            result->push_back(fieldElement.asInt64());
        }
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableValue<bsls_PlatformUtil::Uint64> > *result,
        const bcem_Aggregate&                                       aggregate,
        int                                                         fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    int length = array.length();
    if (length < 0) {
        // The 'length()' method returned an error code instead of a length
        return length;
    }

    result->clear();
    result->reserve(length);
    for (int i = 0; i < length; ++i) {
        typedef bsls_PlatformUtil::Uint64 Uint64;
        bcem_Aggregate fieldElement = array[i];
        if (fieldElement.isNul2()) {
            result->push_back(bdeut_NullableValue<Uint64>());
        }
        else {
            result->push_back((Uint64)fieldElement.asInt64());
        }
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableValue<bdet_DateTz> > *result,
        const bcem_Aggregate&                            aggregate,
        int                                              fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    int length = array.length();
    if (length < 0) {
        // The 'length()' method returned an error code instead of a length
        return length;
    }

    result->clear();
    result->reserve(length);
    for (int i = 0; i < length; ++i) {
        bcem_Aggregate fieldElement = array[i];
        if (fieldElement.isNul2()) {
            result->push_back(bdeut_NullableValue<bdet_DateTz>());
        }
        else {
            result->push_back(fieldElement.asDateTz());
        }
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableValue<bdet_DatetimeTz> > *result,
        const bcem_Aggregate&                                aggregate,
        int                                                  fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    int length = array.length();
    if (length < 0) {
        // The 'length()' method returned an error code instead of a length
        return length;
    }

    result->clear();
    result->reserve(length);
    for (int i = 0; i < length; ++i) {
        bcem_Aggregate fieldElement = array[i];
        if (fieldElement.isNul2()) {
            result->push_back(bdeut_NullableValue<bdet_DatetimeTz>());
        }
        else {
            result->push_back(fieldElement.asDatetimeTz());
        }
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableValue<bdet_TimeTz> > *result,
        const bcem_Aggregate&                            aggregate,
        int                                              fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    int length = array.length();
    if (length < 0) {
        // The 'length()' method returned an error code instead of a length
        return length;
    }

    result->clear();
    result->reserve(length);
    for (int i = 0; i < length; ++i) {
        bcem_Aggregate fieldElement = array[i];
        if (fieldElement.isNul2()) {
            result->push_back(bdeut_NullableValue<bdet_TimeTz>());
        }
        else {
            result->push_back(fieldElement.asTimeTz());
        }
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableValue<bdet_Date> > *result,
        const bcem_Aggregate&                          aggregate,
        int                                            fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    int length = array.length();
    if (length < 0) {
        // The 'length()' method returned an error code instead of a length
        return length;
    }

    result->clear();
    result->reserve(length);
    for (int i = 0; i < length; ++i) {
        bcem_Aggregate fieldElement = array[i];
        if (fieldElement.isNul2()) {
            result->push_back(bdeut_NullableValue<bdet_Date>());
        }
        else {
            result->push_back(fieldElement.asDate());
        }
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableValue<bdet_Datetime> > *result,
        const bcem_Aggregate&                              aggregate,
        int                                                fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    int length = array.length();
    if (length < 0) {
        // The 'length()' method returned an error code instead of a length
        return length;
    }

    result->clear();
    result->reserve(length);
    for (int i = 0; i < length; ++i) {
        bcem_Aggregate fieldElement = array[i];
        if (fieldElement.isNul2()) {
            result->push_back(bdeut_NullableValue<bdet_Datetime>());
        }
        else {
            result->push_back(fieldElement.asDatetime());
        }
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableValue<bdet_Time> > *result,
        const bcem_Aggregate&                          aggregate,
        int                                            fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    int length = array.length();
    if (length < 0) {
        // The 'length()' method returned an error code instead of a length
        return length;
    }

    result->clear();
    result->reserve(length);
    for (int i = 0; i < length; ++i) {
        bcem_Aggregate fieldElement = array[i];
        if (fieldElement.isNul2()) {
            result->push_back(bdeut_NullableValue<bdet_Time>());
        }
        else {
            result->push_back(fieldElement.asTime());
        }
    }

    return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Conversion From Arrays Of Nullable-Allocated Primitive Types

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<bool> > *result,
        const bcem_Aggregate&                              aggregate,
        int                                                fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    int length = array.length();
    if (length < 0) {
        // The 'length()' method returned an error code instead of a length
        return length;
    }

    result->clear();
    result->reserve(length);
    for (int i = 0; i < length; ++i) {
        bcem_Aggregate fieldElement = array[i];
        if (fieldElement.isNul2()) {
            result->push_back(bdeut_NullableAllocatedValue<bool>());
        }
        else {
            result->push_back(fieldElement.asBool());
        }
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<char> > *result,
        const bcem_Aggregate&                              aggregate,
        int                                                fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    int length = array.length();
    if (length < 0) {
        // The 'length()' method returned an error code instead of a length
        return length;
    }

    result->clear();
    result->reserve(length);
    for (int i = 0; i < length; ++i) {
        bcem_Aggregate fieldElement = array[i];
        if (fieldElement.isNul2()) {
            result->push_back(bdeut_NullableAllocatedValue<char>());
        }
        else {
            result->push_back(fieldElement.asChar());
        }
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<short> > *result,
        const bcem_Aggregate&                               aggregate,
        int                                                 fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    int length = array.length();
    if (length < 0) {
        // The 'length()' method returned an error code instead of a length
        return length;
    }

    result->clear();
    result->reserve(length);
    for (int i = 0; i < length; ++i) {
        bcem_Aggregate fieldElement = array[i];
        if (fieldElement.isNul2()) {
            result->push_back(bdeut_NullableAllocatedValue<short>());
        }
        else {
            result->push_back(fieldElement.asShort());
        }
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<int> > *result,
        const bcem_Aggregate&                    aggregate,
        int                                      fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    int length = array.length();
    if (length < 0) {
        // The 'length()' method returned an error code instead of a length
        return length;
    }

    result->clear();
    result->reserve(length);
    for (int i = 0; i < length; ++i) {
        bcem_Aggregate fieldElement = array[i];
        if (fieldElement.isNul2()) {
            result->push_back(bdeut_NullableAllocatedValue<int>());
        }
        else {
            result->push_back(fieldElement.asInt());
        }
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<float> > *result,
        const bcem_Aggregate&                               aggregate,
        int                                                 fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    int length = array.length();
    if (length < 0) {
        // The 'length()' method returned an error code instead of a length
        return length;
    }

    result->clear();
    result->reserve(length);
    for (int i = 0; i < length; ++i) {
        bcem_Aggregate fieldElement = array[i];
        if (fieldElement.isNul2()) {
            result->push_back(bdeut_NullableAllocatedValue<float>());
        }
        else {
            result->push_back(fieldElement.asFloat());
        }
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<double> > *result,
        const bcem_Aggregate&                                aggregate,
        int                                                  fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    int length = array.length();
    if (length < 0) {
        // The 'length()' method returned an error code instead of a length
        return length;
    }

    result->clear();
    result->reserve(length);
    for (int i = 0; i < length; ++i) {
        bcem_Aggregate fieldElement = array[i];
        if (fieldElement.isNul2()) {
            result->push_back(bdeut_NullableAllocatedValue<double>());
        }
        else {
            result->push_back(fieldElement.asDouble());
        }
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<unsigned char> > *result,
        const bcem_Aggregate&                                       aggregate,
        int                                                         fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    int length = array.length();
    if (length < 0) {
        // The 'length()' method returned an error code instead of a length
        return length;
    }

    result->clear();
    result->reserve(length);
    for (int i = 0; i < length; ++i) {
        bcem_Aggregate fieldElement = array[i];
        if (fieldElement.isNul2()) {
            result->push_back(bdeut_NullableAllocatedValue<unsigned char>());
        }
        else {
            result->push_back((unsigned char)fieldElement.asChar());
        }
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<unsigned short> > *result,
        const bcem_Aggregate&                                        aggregate,
        int                                                          fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    int length = array.length();
    if (length < 0) {
        // The 'length()' method returned an error code instead of a length
        return length;
    }

    result->clear();
    result->reserve(length);
    for (int i = 0; i < length; ++i) {
        bcem_Aggregate fieldElement = array[i];
        if (fieldElement.isNul2()) {
            result->push_back(bdeut_NullableAllocatedValue<unsigned short>());
        }
        else {
            result->push_back((unsigned short)fieldElement.asShort());
        }
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<unsigned int> > *result,
        const bcem_Aggregate&                                      aggregate,
        int                                                        fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    int length = array.length();
    if (length < 0) {
        // The 'length()' method returned an error code instead of a length
        return length;
    }

    result->clear();
    result->reserve(length);
    for (int i = 0; i < length; ++i) {
        bcem_Aggregate fieldElement = array[i];
        if (fieldElement.isNul2()) {
            result->push_back(bdeut_NullableAllocatedValue<unsigned int>());
        }
        else {
            result->push_back((unsigned int)fieldElement.asInt());
        }
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<bsl::string> > *result,
        const bcem_Aggregate&                                     aggregate,
        int                                                       fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    int length = array.length();
    if (length < 0) {
        // The 'length()' method returned an error code instead of a length
        return length;
    }

    result->clear();
    result->reserve(length);
    for (int i = 0; i < length; ++i) {
        bcem_Aggregate fieldElement = array[i];
        if (fieldElement.isNul2()) {
            result->push_back(bdeut_NullableAllocatedValue<bsl::string>());
        }
        else {
            result->push_back(fieldElement.asString());
        }
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<bsls_PlatformUtil::Int64> >
                                                                    *result,
        const bcem_Aggregate&                                        aggregate,
        int                                                          fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    int length = array.length();
    if (length < 0) {
        // The 'length()' method returned an error code instead of a length
        return length;
    }

    result->clear();
    result->reserve(length);
    for (int i = 0; i < length; ++i) {
        bcem_Aggregate fieldElement = array[i];
        if (fieldElement.isNul2()) {
            result->push_back(
                    bdeut_NullableAllocatedValue<bsls_PlatformUtil::Int64>());
        }
        else {
            result->push_back(fieldElement.asInt64());
        }
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<bsls_PlatformUtil::Uint64> >
                                                                   *result,
        const bcem_Aggregate&                                       aggregate,
        int                                                         fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    int length = array.length();
    if (length < 0) {
        // The 'length()' method returned an error code instead of a length
        return length;
    }

    result->clear();
    result->reserve(length);
    for (int i = 0; i < length; ++i) {
        typedef bsls_PlatformUtil::Uint64 Uint64;
        bcem_Aggregate fieldElement = array[i];
        if (fieldElement.isNul2()) {
            result->push_back(bdeut_NullableAllocatedValue<Uint64>());
        }
        else {
            result->push_back((Uint64)fieldElement.asInt64());
        }
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<bdet_DateTz> > *result,
        const bcem_Aggregate&                                     aggregate,
        int                                                       fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    int length = array.length();
    if (length < 0) {
        // The 'length()' method returned an error code instead of a length
        return length;
    }

    result->clear();
    result->reserve(length);
    for (int i = 0; i < length; ++i) {
        bcem_Aggregate fieldElement = array[i];
        if (fieldElement.isNul2()) {
            result->push_back(bdeut_NullableAllocatedValue<bdet_DateTz>());
        }
        else {
            result->push_back(fieldElement.asDateTz());
        }
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
       bsl::vector< bdeut_NullableAllocatedValue<bdet_DatetimeTz> > *result,
       const bcem_Aggregate&                                         aggregate,
       int                                                           fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    int length = array.length();
    if (length < 0) {
        // The 'length()' method returned an error code instead of a length
        return length;
    }

    result->clear();
    result->reserve(length);
    for (int i = 0; i < length; ++i) {
        bcem_Aggregate fieldElement = array[i];
        if (fieldElement.isNul2()) {
            result->push_back(bdeut_NullableAllocatedValue<bdet_DatetimeTz>());
        }
        else {
            result->push_back(fieldElement.asDatetimeTz());
        }
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<bdet_TimeTz> > *result,
        const bcem_Aggregate&                                     aggregate,
        int                                                       fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    int length = array.length();
    if (length < 0) {
        // The 'length()' method returned an error code instead of a length
        return length;
    }

    result->clear();
    result->reserve(length);
    for (int i = 0; i < length; ++i) {
        bcem_Aggregate fieldElement = array[i];
        if (fieldElement.isNul2()) {
            result->push_back(bdeut_NullableAllocatedValue<bdet_TimeTz>());
        }
        else {
            result->push_back(fieldElement.asTimeTz());
        }
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<bdet_Date> > *result,
        const bcem_Aggregate&                                   aggregate,
        int                                                     fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    int length = array.length();
    if (length < 0) {
        // The 'length()' method returned an error code instead of a length
        return length;
    }

    result->clear();
    result->reserve(length);
    for (int i = 0; i < length; ++i) {
        bcem_Aggregate fieldElement = array[i];
        if (fieldElement.isNul2()) {
            result->push_back(bdeut_NullableAllocatedValue<bdet_Date>());
        }
        else {
            result->push_back(fieldElement.asDate());
        }
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<bdet_Datetime> > *result,
        const bcem_Aggregate&                                       aggregate,
        int                                                         fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    int length = array.length();
    if (length < 0) {
        // The 'length()' method returned an error code instead of a length
        return length;
    }

    result->clear();
    result->reserve(length);
    for (int i = 0; i < length; ++i) {
        bcem_Aggregate fieldElement = array[i];
        if (fieldElement.isNul2()) {
            result->push_back(bdeut_NullableAllocatedValue<bdet_Datetime>());
        }
        else {
            result->push_back(fieldElement.asDatetime());
        }
    }

    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<bdet_Time> > *result,
        const bcem_Aggregate&                                   aggregate,
        int                                                     fieldId)
{
    bcem_Aggregate array = aggregate.fieldById(fieldId);
    if (array.isError()) {
        return array.errorCode();
    }

    int length = array.length();
    if (length < 0) {
        // The 'length()' method returned an error code instead of a length
        return length;
    }

    result->clear();
    result->reserve(length);
    for (int i = 0; i < length; ++i) {
        bcem_Aggregate fieldElement = array[i];
        if (fieldElement.isNul2()) {
            result->push_back(bdeut_NullableAllocatedValue<bdet_Time>());
        }
        else {
            result->push_back(fieldElement.asTime());
        }
    }

    return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Conversion To Primitive Types

namespace {

template <typename PRIMITIVE_TYPE>
inline
int toAggregatePrimitiveImp(bcem_Aggregate        *result,
                            int                    fieldId,
                            const PRIMITIVE_TYPE&  value)
{
    return result->setFieldById(fieldId, value).errorCode();
}

}  // close unnamed namespace

int bcem_AggregateUtil::toAggregatePrimitive(
                                    bcem_Aggregate            *result,
                                    int                        fieldId,
                                    bool                       value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
                                    bcem_Aggregate            *result,
                                    int                        fieldId,
                                    char                       value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
                                    bcem_Aggregate            *result,
                                    int                        fieldId,
                                    short                      value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
                                    bcem_Aggregate            *result,
                                    int                        fieldId,
                                    int                        value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
                                    bcem_Aggregate            *result,
                                    int                        fieldId,
                                    float                      value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
                                    bcem_Aggregate            *result,
                                    int                        fieldId,
                                    double                     value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
                                    bcem_Aggregate            *result,
                                    int                        fieldId,
                                    unsigned char              value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
                                    bcem_Aggregate            *result,
                                    int                        fieldId,
                                    unsigned short             value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
                                    bcem_Aggregate            *result,
                                    int                        fieldId,
                                    unsigned int               value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
                                    bcem_Aggregate            *result,
                                    int                        fieldId,
                                    const bsl::string&         value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
                                    bcem_Aggregate            *result,
                                    int                        fieldId,
                                    Int64                      value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
                                    bcem_Aggregate            *result,
                                    int                        fieldId,
                                    Uint64                     value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
                                    bcem_Aggregate            *result,
                                    int                        fieldId,
                                    const bdet_Date&           value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
                                    bcem_Aggregate            *result,
                                    int                        fieldId,
                                    const bdet_DateTz&         value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
                                    bcem_Aggregate            *result,
                                    int                        fieldId,
                                    const bdet_Datetime&       value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
                                    bcem_Aggregate            *result,
                                    int                        fieldId,
                                    const bdet_DatetimeTz&     value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
                                    bcem_Aggregate            *result,
                                    int                        fieldId,
                                    const bdet_Time&           value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
                                    bcem_Aggregate            *result,
                                    int                        fieldId,
                                    const bdet_TimeTz&         value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Conversion To Nullable Primitive Types

namespace {

template <typename PRIMITIVE_TYPE>
inline
int toAggregatePrimitiveImp(
        bcem_Aggregate                             *result,
        int                                         fieldId,
        const bdeut_NullableValue<PRIMITIVE_TYPE>&  value)
{
    bcem_Aggregate aggregate = result->fieldById(fieldId);
    if (aggregate.isError()) {
        return aggregate.errorCode();
    }

//     if (false == aggregate.fieldSpec()->isNullable()) {
//         return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
//     }

    if (value.isNull()) {
        return aggregate.makeNull().errorCode();
    }
    else {
        return aggregate.makeValue().setValue(value.value()).errorCode();
    }
}

int toAggregatePrimitiveImp(
        bcem_Aggregate                                  *result,
        int                                              fieldId,
        const bdeut_NullableValue< bsl::vector<char> >&  value)
{
    bcem_Aggregate aggregate = result->fieldById(fieldId);
    if (aggregate.isError()) {
        return aggregate.errorCode();
    }

//     if (false == aggregate.fieldSpec()->isNullable()) {
//         return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
//     }

    if (value.isNull()) {
        return aggregate.makeNull().errorCode();
    }
    else {
        const bdem_ElemRef& elemRef = aggregate.makeValue().asElemRef();
        if (bdem_ElemType::BDEM_CHAR_ARRAY != elemRef.type()) {
            return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
        }

        elemRef.theModifiableCharArray() = value.value();
        return 0;
    }
}

}  // close unnamed namespace

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                       *result,
        int                                                   fieldId,
        const bdeut_NullableValue<bool>&                      value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                       *result,
        int                                                   fieldId,
        const bdeut_NullableValue<char>&                      value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                       *result,
        int                                                   fieldId,
        const bdeut_NullableValue<short>&                     value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                       *result,
        int                                                   fieldId,
        const bdeut_NullableValue<int>&                       value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                       *result,
        int                                                   fieldId,
        const bdeut_NullableValue<float>&                     value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                       *result,
        int                                                   fieldId,
        const bdeut_NullableValue<double>&                    value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                       *result,
        int                                                   fieldId,
        const bdeut_NullableValue<unsigned char>&             value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                       *result,
        int                                                   fieldId,
        const bdeut_NullableValue<unsigned short>&            value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                       *result,
        int                                                   fieldId,
        const bdeut_NullableValue<unsigned int>&              value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                       *result,
        int                                                   fieldId,
        const bdeut_NullableValue<bsl::string>&               value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                       *result,
        int                                                   fieldId,
        const bdeut_NullableValue<bsls_PlatformUtil::Int64>&  value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                       *result,
        int                                                   fieldId,
        const bdeut_NullableValue<bsls_PlatformUtil::Uint64>& value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                       *result,
        int                                                   fieldId,
        const bdeut_NullableValue<bdet_DateTz>&               value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                       *result,
        int                                                   fieldId,
        const bdeut_NullableValue<bdet_DatetimeTz>&           value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                       *result,
        int                                                   fieldId,
        const bdeut_NullableValue<bdet_TimeTz>&               value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                       *result,
        int                                                   fieldId,
        const bdeut_NullableValue<bdet_Date>&                 value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                       *result,
        int                                                   fieldId,
        const bdeut_NullableValue<bdet_Datetime>&             value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                       *result,
        int                                                   fieldId,
        const bdeut_NullableValue<bdet_Time>&                 value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                       *result,
        int                                                   fieldId,
        const bdeut_NullableValue< bsl::vector<char> >&       value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Conversion To Nullable-Allocated Primitive Types

namespace {

template <typename PRIMITIVE_TYPE>
inline
int toAggregatePrimitiveImp(
        bcem_Aggregate                                      *result,
        int                                                  fieldId,
        const bdeut_NullableAllocatedValue<PRIMITIVE_TYPE>&  value)
{
    bcem_Aggregate aggregate = result->fieldById(fieldId);
    if (aggregate.isError()) {
        return aggregate.errorCode();
    }

//     if (false == aggregate.fieldSpec()->isNullable()) {
//         return bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
//     }

    if (value.isNull()) {
        return aggregate.makeNull().errorCode();
    }
    else {
        return aggregate.makeValue().setValue(value.value()).errorCode();
    }
}

}  // close unnamed namespace

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bdeut_NullableAllocatedValue<bool>&                      value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bdeut_NullableAllocatedValue<char>&                      value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bdeut_NullableAllocatedValue<short>&                     value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bdeut_NullableAllocatedValue<int>&                       value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bdeut_NullableAllocatedValue<float>&                     value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bdeut_NullableAllocatedValue<double>&                    value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bdeut_NullableAllocatedValue<unsigned char>&             value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bdeut_NullableAllocatedValue<unsigned short>&            value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bdeut_NullableAllocatedValue<unsigned int>&              value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bdeut_NullableAllocatedValue<bsl::string>&               value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bdeut_NullableAllocatedValue<bsls_PlatformUtil::Int64>&  value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bdeut_NullableAllocatedValue<bsls_PlatformUtil::Uint64>& value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bdeut_NullableAllocatedValue<bdet_DateTz>&               value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bdeut_NullableAllocatedValue<bdet_DatetimeTz>&           value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bdeut_NullableAllocatedValue<bdet_TimeTz>&               value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bdeut_NullableAllocatedValue<bdet_Date>&                 value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bdeut_NullableAllocatedValue<bdet_Datetime>&             value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bdeut_NullableAllocatedValue<bdet_Time>&                 value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bdeut_NullableAllocatedValue< bsl::vector<char> >&       value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Conversion To Arrays Of Primitive Types

namespace {

template <typename PRIMITIVE_TYPE>
inline
int toAggregatePrimitiveImp(
        bcem_Aggregate                     *result,
        const bsl::vector<PRIMITIVE_TYPE>&  value)
{
    typedef typename bsl::vector<PRIMITIVE_TYPE>::const_iterator ConstIter;

    result->removeAllItems();
    for (ConstIter it = value.begin(); it != value.end(); ++it) {
        bcem_Aggregate item = result->append(*it);
        if (item.isError()) {
            return item.errorCode();
        }
    }

    return 0;
}

}  // close unnamed namespace

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                               *result,
        int                                           fieldId,
        const bsl::vector<bool>&                      value)
{
    bcem_Aggregate aggregate = result->fieldById(fieldId);
    if (aggregate.isError()) {
        return aggregate.errorCode();
    }

    int rc = 0;
    if (bdem_ElemType::BDEM_BOOL_ARRAY == aggregate.dataType()) {
        aggregate.asElemRef().theModifiableBoolArray() = value;
    }
    else {
        rc = toAggregatePrimitiveImp(&aggregate, value);
    }

    return rc;
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                               *result,
        int                                           fieldId,
        const bsl::vector<char>&                      value)
{
    bcem_Aggregate aggregate = result->fieldById(fieldId);
    if (aggregate.isError()) {
        return aggregate.errorCode();
    }

    int rc = 0;
    if (bdem_ElemType::BDEM_CHAR_ARRAY == aggregate.dataType()) {
        aggregate.asElemRef().theModifiableCharArray() = value;
    }
    else {
        rc = toAggregatePrimitiveImp(&aggregate, value);
    }

    return rc;
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                               *result,
        int                                           fieldId,
        const bsl::vector<short>&                     value)
{
    bcem_Aggregate aggregate = result->fieldById(fieldId);
    if (aggregate.isError()) {
        return aggregate.errorCode();
    }

    int rc = 0;
    if (bdem_ElemType::BDEM_SHORT_ARRAY == aggregate.dataType()) {
        aggregate.asElemRef().theModifiableShortArray() = value;
    }
    else {
        rc = toAggregatePrimitiveImp(&aggregate, value);
    }

    return rc;
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                               *result,
        int                                           fieldId,
        const bsl::vector<int>&                       value)
{
    bcem_Aggregate aggregate = result->fieldById(fieldId);
    if (aggregate.isError()) {
        return aggregate.errorCode();
    }

    int rc = 0;
    if (bdem_ElemType::BDEM_INT_ARRAY == aggregate.dataType()) {
        aggregate.asElemRef().theModifiableIntArray() = value;
    }
    else {
        rc = toAggregatePrimitiveImp(&aggregate, value);
    }

    return rc;
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                               *result,
        int                                           fieldId,
        const bsl::vector<float>&                     value)
{
    bcem_Aggregate aggregate = result->fieldById(fieldId);
    if (aggregate.isError()) {
        return aggregate.errorCode();
    }

    int rc = 0;
    if (bdem_ElemType::BDEM_FLOAT_ARRAY == aggregate.dataType()) {
        aggregate.asElemRef().theModifiableFloatArray() = value;
    }
    else {
        rc = toAggregatePrimitiveImp(&aggregate, value);
    }

    return rc;
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                               *result,
        int                                           fieldId,
        const bsl::vector<double>&                    value)
{
    bcem_Aggregate aggregate = result->fieldById(fieldId);
    if (aggregate.isError()) {
        return aggregate.errorCode();
    }

    int rc = 0;
    if (bdem_ElemType::BDEM_DOUBLE_ARRAY == aggregate.dataType()) {
        aggregate.asElemRef().theModifiableDoubleArray() = value;
    }
    else {
        rc = toAggregatePrimitiveImp(&aggregate, value);
    }

    return rc;
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                               *result,
        int                                           fieldId,
        const bsl::vector<unsigned char>&             value)
{
    bcem_Aggregate aggregate = result->fieldById(fieldId);
    if (aggregate.isError()) {
        return aggregate.errorCode();
    }

    typedef bsl::vector<unsigned char> Vector;

    int rc = 0;
    if (bdem_ElemType::BDEM_CHAR_ARRAY == aggregate.dataType()) {
        Vector *vector =
            reinterpret_cast<Vector *>(&aggregate.asElemRef()
                                                .theModifiableCharArray());
        *vector = value;
    }
    else {
        rc = toAggregatePrimitiveImp(&aggregate, value);
    }

    return rc;
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                               *result,
        int                                           fieldId,
        const bsl::vector<unsigned short>&            value)
{
    bcem_Aggregate aggregate = result->fieldById(fieldId);
    if (aggregate.isError()) {
        return aggregate.errorCode();
    }

    typedef bsl::vector<unsigned short> Vector;

    int rc = 0;
    if (bdem_ElemType::BDEM_SHORT_ARRAY == aggregate.dataType()) {
        Vector *vector =
            reinterpret_cast<Vector *>(&aggregate.asElemRef()
                                                .theModifiableShortArray());
        *vector = value;
    }
    else {
        rc = toAggregatePrimitiveImp(&aggregate, value);
    }

    return rc;
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                               *result,
        int                                           fieldId,
        const bsl::vector<unsigned int>&              value)
{
    bcem_Aggregate aggregate = result->fieldById(fieldId);
    if (aggregate.isError()) {
        return aggregate.errorCode();
    }

    typedef bsl::vector<unsigned int> Vector;

    int rc = 0;
    if (bdem_ElemType::BDEM_INT_ARRAY == aggregate.dataType()) {
        Vector *vector =
            reinterpret_cast<Vector *>(&aggregate.asElemRef()
                                                .theModifiableIntArray());
        *vector = value;
    }
    else {
        rc = toAggregatePrimitiveImp(&aggregate, value);
    }

    return rc;
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                               *result,
        int                                           fieldId,
        const bsl::vector<bsl::string>&               value)
{
    bcem_Aggregate aggregate = result->fieldById(fieldId);
    if (aggregate.isError()) {
        return aggregate.errorCode();
    }

    int rc = 0;
    if (bdem_ElemType::BDEM_STRING_ARRAY == aggregate.dataType()) {
        aggregate.asElemRef().theModifiableStringArray() = value;
    }
    else {
        rc = toAggregatePrimitiveImp(&aggregate, value);
    }

    return rc;
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                               *result,
        int                                           fieldId,
        const bsl::vector<bsls_PlatformUtil::Int64>&  value)
{
    bcem_Aggregate aggregate = result->fieldById(fieldId);
    if (aggregate.isError()) {
        return aggregate.errorCode();
    }

    int rc = 0;
    if (bdem_ElemType::BDEM_INT64_ARRAY == aggregate.dataType()) {
        aggregate.asElemRef().theModifiableInt64Array() = value;
    }
    else {
        rc = toAggregatePrimitiveImp(&aggregate, value);
    }

    return rc;
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                *result,
        int                                            fieldId,
        const bsl::vector<bsls_PlatformUtil::Uint64>&  value)
{
    bcem_Aggregate aggregate = result->fieldById(fieldId);
    if (aggregate.isError()) {
        return aggregate.errorCode();
    }

    typedef bsl::vector<bsls_PlatformUtil::Uint64> Vector;

    int rc = 0;
    if (bdem_ElemType::BDEM_INT64_ARRAY == aggregate.dataType()) {
        Vector *vector =
            reinterpret_cast<Vector *>(&aggregate.asElemRef()
                                                .theModifiableInt64Array());
        *vector = value;
    }
    else {
        rc = toAggregatePrimitiveImp(&aggregate, value);
    }

    return rc;
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                               *result,
        int                                           fieldId,
        const bsl::vector<bdet_DateTz>&               value)
{
    bcem_Aggregate aggregate = result->fieldById(fieldId);
    if (aggregate.isError()) {
        return aggregate.errorCode();
    }

    int rc = 0;
    if (bdem_ElemType::BDEM_DATETZ_ARRAY == aggregate.dataType()) {
        aggregate.asElemRef().theModifiableDateTzArray() = value;
    }
    else {
        rc = toAggregatePrimitiveImp(&aggregate, value);
    }

    return rc;
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                               *result,
        int                                           fieldId,
        const bsl::vector<bdet_DatetimeTz>&           value)
{
    bcem_Aggregate aggregate = result->fieldById(fieldId);
    if (aggregate.isError()) {
        return aggregate.errorCode();
    }

    int rc = 0;
    if (bdem_ElemType::BDEM_DATETIMETZ_ARRAY == aggregate.dataType()) {
        aggregate.asElemRef().theModifiableDatetimeTzArray() = value;
    }
    else {
        rc = toAggregatePrimitiveImp(&aggregate, value);
    }

    return rc;
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                               *result,
        int                                           fieldId,
        const bsl::vector<bdet_TimeTz>&               value)
{
    bcem_Aggregate aggregate = result->fieldById(fieldId);
    if (aggregate.isError()) {
        return aggregate.errorCode();
    }

    int rc = 0;
    if (bdem_ElemType::BDEM_TIMETZ_ARRAY == aggregate.dataType()) {
        aggregate.asElemRef().theModifiableTimeTzArray() = value;
    }
    else {
        rc = toAggregatePrimitiveImp(&aggregate, value);
    }

    return rc;
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                               *result,
        int                                           fieldId,
        const bsl::vector<bdet_Date>&                 value)
{
    bcem_Aggregate aggregate = result->fieldById(fieldId);
    if (aggregate.isError()) {
        return aggregate.errorCode();
    }

    int rc = 0;
    if (bdem_ElemType::BDEM_DATE_ARRAY == aggregate.dataType()) {
        aggregate.asElemRef().theModifiableDateArray() = value;
    }
    else {
        rc = toAggregatePrimitiveImp(&aggregate, value);
    }

    return rc;
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                               *result,
        int                                           fieldId,
        const bsl::vector<bdet_Datetime>&             value)
{
    bcem_Aggregate aggregate = result->fieldById(fieldId);
    if (aggregate.isError()) {
        return aggregate.errorCode();
    }

    int rc = 0;
    if (bdem_ElemType::BDEM_DATETIME_ARRAY == aggregate.dataType()) {
        aggregate.asElemRef().theModifiableDatetimeArray() = value;
    }
    else {
        rc = toAggregatePrimitiveImp(&aggregate, value);
    }

    return rc;
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                               *result,
        int                                           fieldId,
        const bsl::vector<bdet_Time>&                 value)
{
    bcem_Aggregate aggregate = result->fieldById(fieldId);
    if (aggregate.isError()) {
        return aggregate.errorCode();
    }

    int rc = 0;
    if (bdem_ElemType::BDEM_TIME_ARRAY == aggregate.dataType()) {
        aggregate.asElemRef().theModifiableTimeArray() = value;
    }
    else {
        rc = toAggregatePrimitiveImp(&aggregate, value);
    }

    return rc;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Conversion To Arrays Of Nullable Primitive Types

namespace {

template <typename PRIMITIVE_TYPE>
inline
int toAggregatePrimitiveImp(
        bcem_Aggregate                                            *result,
        int                                                        fieldId,
        const bsl::vector< bdeut_NullableValue<PRIMITIVE_TYPE> >&  value)
{
    bcem_Aggregate aggregate = result->fieldById(fieldId);
    if (aggregate.isError()) {
        return aggregate.errorCode();
    }

    typedef typename bsl::vector< bdeut_NullableValue<PRIMITIVE_TYPE>
                                >::const_iterator ConstIter;

    aggregate.removeAllItems();

    for (ConstIter it = value.begin(); it != value.end(); ++it) {
        if (it->isNull()) {
            bcem_Aggregate item = aggregate.appendItems(1);
            if (item.isError()) {
                return item.errorCode();
            }
        }
        else {
            bcem_Aggregate item = aggregate.append(it->value());
            if (item.isError()) {
                return item.errorCode();
            }
        }
    }

    return 0;
}

}  // close unnamed namespace

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                  *result,
        int                                             fieldId,
        const bsl::vector< bdeut_NullableValue<bool> >& value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                  *result,
        int                                              fieldId,
        const bsl::vector< bdeut_NullableValue<char> >&  value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                   *result,
        int                                               fieldId,
        const bsl::vector< bdeut_NullableValue<short> >&  value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                 *result,
        int                                             fieldId,
        const bsl::vector< bdeut_NullableValue<int> >&  value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                   *result,
        int                                               fieldId,
        const bsl::vector< bdeut_NullableValue<float> >&  value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                    *result,
        int                                                fieldId,
        const bsl::vector< bdeut_NullableValue<double> >&  value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                           *result,
        int                                                       fieldId,
        const bsl::vector< bdeut_NullableValue<unsigned char> >&  value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                            *result,
        int                                                        fieldId,
        const bsl::vector< bdeut_NullableValue<unsigned short> >&  value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                          *result,
        int                                                      fieldId,
        const bsl::vector< bdeut_NullableValue<unsigned int> >&  value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                         *result,
        int                                                     fieldId,
        const bsl::vector< bdeut_NullableValue<bsl::string> >&  value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bsl::vector< bdeut_NullableValue<bsls_PlatformUtil::Int64> >&
                                                                       value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bsl::vector< bdeut_NullableValue<bsls_PlatformUtil::Uint64> >&
                                                                       value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                         *result,
        int                                                     fieldId,
        const bsl::vector< bdeut_NullableValue<bdet_DateTz> >&  value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                             *result,
        int                                                         fieldId,
        const bsl::vector< bdeut_NullableValue<bdet_DatetimeTz> >&  value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                         *result,
        int                                                     fieldId,
        const bsl::vector< bdeut_NullableValue<bdet_TimeTz> >&  value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                       *result,
        int                                                   fieldId,
        const bsl::vector< bdeut_NullableValue<bdet_Date> >&  value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                           *result,
        int                                                       fieldId,
        const bsl::vector< bdeut_NullableValue<bdet_Datetime> >&  value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                       *result,
        int                                                   fieldId,
        const bsl::vector< bdeut_NullableValue<bdet_Time> >&  value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Conversion To Arrays Of Nullable-Allocated Primitive Types

namespace {

template <typename PRIMITIVE_TYPE>
inline
int toAggregatePrimitiveImp(
        bcem_Aggregate                                            *result,
        int                                                        fieldId,
        const bsl::vector<
            bdeut_NullableAllocatedValue<PRIMITIVE_TYPE> >&        value)
{
    bcem_Aggregate aggregate = result->fieldById(fieldId);
    if (aggregate.isError()) {
        return aggregate.errorCode();
    }

    typedef typename bsl::vector<bdeut_NullableAllocatedValue<PRIMITIVE_TYPE>
                                >::const_iterator ConstIter;

    aggregate.removeAllItems();

    for (ConstIter it = value.begin(); it != value.end(); ++it) {
        if (it->isNull()) {
            bcem_Aggregate item = aggregate.appendItems(1);
            if (item.isError()) {
                return item.errorCode();
            }
        }
        else {
            bcem_Aggregate item = aggregate.append(it->value());
            if (item.isError()) {
                return item.errorCode();
            }
        }
    }

    return 0;
}

}  // close unnamed namespace

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                          *result,
        int                                                      fieldId,
        const bsl::vector< bdeut_NullableAllocatedValue<bool> >& value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                           *result,
        int                                                       fieldId,
        const bsl::vector< bdeut_NullableAllocatedValue<char> >&  value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                            *result,
        int                                                        fieldId,
        const bsl::vector< bdeut_NullableAllocatedValue<short> >&  value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                          *result,
        int                                                      fieldId,
        const bsl::vector< bdeut_NullableAllocatedValue<int> >&  value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                            *result,
        int                                                        fieldId,
        const bsl::vector< bdeut_NullableAllocatedValue<float> >&  value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                             *result,
        int                                                         fieldId,
        const bsl::vector< bdeut_NullableAllocatedValue<double> >&  value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                     *result,
        int                                                 fieldId,
        const bsl::vector<
            bdeut_NullableAllocatedValue<unsigned char> >&  value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                      *result,
        int                                                  fieldId,
        const bsl::vector<
            bdeut_NullableAllocatedValue<unsigned short> >&  value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                    *result,
        int                                                fieldId,
        const bsl::vector<
            bdeut_NullableAllocatedValue<unsigned int> >&  value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                   *result,
        int                                               fieldId,
        const bsl::vector<
            bdeut_NullableAllocatedValue<bsl::string> >&  value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bsl::vector<
            bdeut_NullableAllocatedValue<bsls_PlatformUtil::Int64> >&  value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bsl::vector<
            bdeut_NullableAllocatedValue<bsls_PlatformUtil::Uint64> >& value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                   *result,
        int                                               fieldId,
        const bsl::vector<
            bdeut_NullableAllocatedValue<bdet_DateTz> >&  value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                       *result,
        int                                                   fieldId,
        const bsl::vector<
            bdeut_NullableAllocatedValue<bdet_DatetimeTz> >&  value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                   *result,
        int                                               fieldId,
        const bsl::vector<
            bdeut_NullableAllocatedValue<bdet_TimeTz> >&  value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bsl::vector< bdeut_NullableAllocatedValue<bdet_Date> >&  value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                     *result,
        int                                                 fieldId,
        const bsl::vector<
            bdeut_NullableAllocatedValue<bdet_Datetime> >&  value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                                *result,
        int                                                            fieldId,
        const bsl::vector< bdeut_NullableAllocatedValue<bdet_Time> >&  value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int bcem_AggregateUtil::setValue(const bcem_Aggregate&   result,
                                 const bsl::string&      value)
{
    return result.setValue(value).errorCode();
}

const char *bcem_AggregateUtil::errorString(int errorCode)
{
    switch (errorCode) {
      case bcem_Aggregate::BCEM_ERR_NOT_A_RECORD:
          return "Attempt to access a field (by name, ID, or "
                 "index) of an aggregate that does not reference a "
                 "list, row, choice, or choice array item.";
      case bcem_Aggregate::BCEM_ERR_NOT_A_SEQUENCE:
          return "Attempt to perform a list or row operation on an "
                 "aggregate that does not refer to a list or row "
                 "(e.g., initialize from a non-sequence record def).";
      case bcem_Aggregate::BCEM_ERR_NOT_A_CHOICE:
          return "Attempt to perform a choice or choice array item "
                 "operation (make selection, get selection, etc.) "
                 "on an aggregate that is not a choice or choice "
                 "array item.";
      case bcem_Aggregate::BCEM_ERR_NOT_AN_ARRAY:
          return "Attempt to perform an array operation (index, "
                 "insert, etc.) on an aggregate that is not an "
                 "array, table, or choice array.";
      case bcem_Aggregate::BCEM_ERR_BAD_FIELDNAME:
          return "Field name does not exist in the record def.";
      case bcem_Aggregate::BCEM_ERR_BAD_FIELDID:
          return "Field ID does not exist in record def.";
      case bcem_Aggregate::BCEM_ERR_BAD_FIELDINDEX:
          return "Field index is not a positive integer less than "
                 "the length of the field definition.";
      case bcem_Aggregate::BCEM_ERR_BAD_ARRAYINDEX:
          return "Array (or table) index is out of bounds.";
      case bcem_Aggregate::BCEM_ERR_NOT_SELECTED:
          return "Attempt to access a choice field that is not "
                 "the currently selected object.";
      case bcem_Aggregate::BCEM_ERR_BAD_CONVERSION:
          return "Attempt to set an aggregate using a value that "
                 "is not convertible to the aggregate's type.";
      case bcem_Aggregate::BCEM_ERR_BAD_ENUMVALUE:
          return "Attempt to set the value of an enumeration "
                 "aggregate to a string that is not an enumerator "
                 "name in the enumeration definition or to an "
                 "integer that is not an enumerator ID in the "
                 "enumeration definition. ";
      case bcem_Aggregate::BCEM_ERR_NON_CONFORMANT:
          return "Attempt to set a list, row, table, choice, "
                 "choice array item, or choice array aggregate to "
                 "a value of the correct type, but which doesn't "
                 "conform to the aggregate's record definition.";
      case bcem_Aggregate::BCEM_ERR_AMBIGUOUS_ANON:
          return "A reference to an anonymous field is ambiguous, "
                 "typically because the aggregate contains more "
                 "than one anonymous field.";
    };

    return "An unknown error has occurred.";
}

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
