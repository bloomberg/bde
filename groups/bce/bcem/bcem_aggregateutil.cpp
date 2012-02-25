// bcem_aggregateutil.cpp   -*-C++-*-
#include <bcem_aggregateutil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcem_aggregateutil_cpp,"$Id$ $CSID$")

namespace BloombergLP {

namespace { 

namespace s {
    // the Sun compiler actually compiles AggregateRaw.cpp along with 
    // this file, and we've (temporarily) got make_signed defined there too.
    // so here, we'll embed it inside another namespace to disambiguate them.  
    
    // TBD TBD TBD TBD TBD 
    // replace this custom type with bsl::make_signed<T> when that's available.
template <typename TYPE>
struct make_signed {
    typedef TYPE type;
};

template <>
struct make_signed<unsigned char> {
    typedef char type;
};

template <>
struct make_signed<unsigned short> {
    typedef short type;
};

template <>
struct make_signed<unsigned int> {
    typedef int type;
};

template <>
struct make_signed<bsls_Types::Uint64> {
    typedef bsls_Types::Int64 type;
};    
} // namespace s; please remove when make_signed is available
    // TBD TBD TBD TBD
    // TBD replace the code above with bsl::make_signed when available. 

template<typename T> 
bsl::vector<T>& getArray(const bdem_ElemRef& data)
{
    BSLS_ASSERT_OPT(!"Unreachable");
    return *(bsl::vector<T>*)0;
}

template<>
bsl::vector<bool>& getArray<bool>(const bdem_ElemRef& data)
{
    return data.theModifiableBoolArray();
}

template<>
bsl::vector<char>& getArray<char>(const bdem_ElemRef& data)
{
    return data.theModifiableCharArray();
}
    
template<>
bsl::vector<short>& getArray<short>(const bdem_ElemRef& data)
{
    return data.theModifiableShortArray();
}
    
template<>
bsl::vector<int>& getArray<int>(const bdem_ElemRef& data)
{
    return data.theModifiableIntArray();
}

template<>
bsl::vector<float>& getArray<float>(const bdem_ElemRef& data)
{
    return data.theModifiableFloatArray();
}

template<>
bsl::vector<double>& getArray<double>(const bdem_ElemRef& data)
{
    return data.theModifiableDoubleArray();
}

template<>
bsl::vector<bsl::string>& getArray<bsl::string>(
                                                const bdem_ElemRef& data)
{
    return data.theModifiableStringArray();
}

template<>
bsl::vector<bsls_Types::Int64>& getArray<bsls_Types::Int64>(
                                                const bdem_ElemRef& data)
{
    return data.theModifiableInt64Array();
}

template<>
bsl::vector<bdet_DateTz>& getArray<bdet_DateTz>(
                                                const bdem_ElemRef& data)
{
    return data.theModifiableDateTzArray();
}

template<>
bsl::vector<bdet_DatetimeTz>& getArray<bdet_DatetimeTz>(
                                                const bdem_ElemRef& data)
{
    return data.theModifiableDatetimeTzArray();
}

template<>
bsl::vector<bdet_TimeTz>& getArray<bdet_TimeTz>(
                                                const bdem_ElemRef& data)
{
    return data.theModifiableTimeTzArray();
}

template<>
bsl::vector<bdet_Date>& getArray<bdet_Date>(const bdem_ElemRef& data)
{
    return data.theModifiableDateArray();
}

template<>
bsl::vector<bdet_Datetime>& getArray<bdet_Datetime>(
                                                const bdem_ElemRef& data)
{
    return data.theModifiableDatetimeArray();
}

template<>
bsl::vector<bdet_Time>& getArray<bdet_Time>(const bdem_ElemRef& data)
{
    return data.theModifiableTimeArray();
}

template<typename FROMTYPE, typename TOTYPE>
struct SignCast
{
    // This struct facilitates efficient casting of unsigned to signed types
    // when any generic type may be involved.  For types where TOTYPE
    // is the same as FROMTYPE (e.g., double, string, Date, etc), the 
    // 'cast' function returns the passed-in const reference.  Otherwise, 
    // 'cast' returns the corresponding TOTYPE *by value*.  

    static inline
    typename bslmf_If<bslmf_IsSame<FROMTYPE, TOTYPE>::VALUE, 
                      const FROMTYPE&, TOTYPE>::Type 
    cast(const FROMTYPE& s) {
        return (const TOTYPE&)s;
    }
};

template <typename TYPE>
inline
void assignArray(const bcem_AggregateRaw& result, 
                 const bsl::vector<TYPE>& value) {

    typedef s::make_signed<TYPE>::type SignedType;
    
    bsl::vector<SignedType>& array = getArray<SignedType>(result.asElemRef());
    array.clear();
    
    bsl::transform(value.begin(), value.end(), bsl::back_inserter(array), 
                   SignCast<TYPE, SignedType>::cast);
}

template <typename TYPE>
inline
void assignArray(bsl::vector<TYPE>         *result, 
                 const bcem_AggregateRaw&   value)
{
    typedef s::make_signed<TYPE>::type SignedType;
    
    const bsl::vector<SignedType>& array = getArray<SignedType>(
                                                            value.asElemRef());
    result->clear();
    
    bsl::transform(array.begin(), array.end(), bsl::back_inserter(*result), 
                   SignCast<SignedType, TYPE>::cast);
}

template <typename PRIMITIVE_TYPE>
inline bool isPrimitiveArrayType(bdem_ElemType::Type) {
    return false;
}

template <>
inline bool isPrimitiveArrayType<bool>(bdem_ElemType::Type type) {
    return type == bdem_ElemType::BDEM_BOOL_ARRAY;
}

template <>
inline bool isPrimitiveArrayType<char>(bdem_ElemType::Type type) {
    return type == bdem_ElemType::BDEM_CHAR_ARRAY;
}

template <>
inline bool isPrimitiveArrayType<unsigned char>(bdem_ElemType::Type type) {
    return type == bdem_ElemType::BDEM_CHAR_ARRAY;
}

template <>
inline bool isPrimitiveArrayType<short>(bdem_ElemType::Type type) {
    return type == bdem_ElemType::BDEM_SHORT_ARRAY;
}

template <>
inline bool isPrimitiveArrayType<unsigned short>(bdem_ElemType::Type type) {
    return type == bdem_ElemType::BDEM_SHORT_ARRAY;
}

template <>
inline bool isPrimitiveArrayType<int>(bdem_ElemType::Type type) {
    return type == bdem_ElemType::BDEM_INT_ARRAY;
}

template <>
inline bool isPrimitiveArrayType<unsigned int>(bdem_ElemType::Type type) {
    return type == bdem_ElemType::BDEM_INT_ARRAY;
}

template <>
inline bool isPrimitiveArrayType<float>(bdem_ElemType::Type type) {
    return type == bdem_ElemType::BDEM_FLOAT_ARRAY;
}

template <>
inline bool isPrimitiveArrayType<double>(bdem_ElemType::Type type) {
    return type == bdem_ElemType::BDEM_DOUBLE_ARRAY;
}

template <>
inline bool isPrimitiveArrayType<bsl::string>(bdem_ElemType::Type type) {
    return type == bdem_ElemType::BDEM_STRING_ARRAY;
}

template <>
inline bool isPrimitiveArrayType<bsls_Types::Int64>(bdem_ElemType::Type type) {
    return type == bdem_ElemType::BDEM_INT64_ARRAY;
}

template <>
inline bool isPrimitiveArrayType<bsls_Types::Uint64>(bdem_ElemType::Type type) {
    return type == bdem_ElemType::BDEM_INT64_ARRAY;
}

template <>
inline bool isPrimitiveArrayType<bdet_DateTz>(bdem_ElemType::Type type) {
    return type == bdem_ElemType::BDEM_DATETZ_ARRAY;
}

template <>
inline bool isPrimitiveArrayType<bdet_DatetimeTz>(bdem_ElemType::Type type) {
    return type == bdem_ElemType::BDEM_DATETIMETZ_ARRAY;
}

template <>
inline bool isPrimitiveArrayType<bdet_TimeTz>(bdem_ElemType::Type type) {
    return type == bdem_ElemType::BDEM_TIMETZ_ARRAY;
}

template <>
inline bool isPrimitiveArrayType<bdet_Date>(bdem_ElemType::Type type) {
    return type == bdem_ElemType::BDEM_DATE_ARRAY;
}

template <>
inline bool isPrimitiveArrayType<bdet_Time>(bdem_ElemType::Type type) {
    return type == bdem_ElemType::BDEM_TIME_ARRAY;
}

template <>
inline bool isPrimitiveArrayType<bdet_Datetime>(bdem_ElemType::Type type) {
    return type == bdem_ElemType::BDEM_DATETIME_ARRAY;
}

template <typename TYPE>
inline
void assignPrimitive(TYPE *result, const bcem_AggregateRaw& value) {
    if (value.isNull()) {
        *result = static_cast<TYPE>(
                       bdetu_Unset<s::make_signed<TYPE>::type>::unsetValue());
    }
    else {
        *result = value.convertScalar<TYPE>();
    }
}

template <typename TYPE>
inline
void assignPrimitive(bdeut_NullableValue<TYPE> *result, 
                     const bcem_AggregateRaw&   value) {
    if (value.isNull()) {
        result->reset();
    }
    else {
        *result = value.convertScalar<TYPE>();
    }
}

template <typename TYPE>
inline
void assignPrimitive(bdeut_NullableAllocatedValue<TYPE> *result, 
                     const bcem_AggregateRaw&            value) {
    if (value.isNull()) {
        result->reset();
    }
    else {
        *result = value.convertScalar<TYPE>();
    }
}

inline
void assignPrimitive(bdeut_NullableValue<bsl::vector<char> >  *result, 
                     const bcem_AggregateRaw&                  value) {
    if (value.isNull()) {
        result->reset();
    }
    else {
        *result = value.asElemRef().theCharArray();
    }
}

inline
void assignPrimitive(bdeut_NullableAllocatedValue<bsl::vector<char> > *result, 
                     const bcem_AggregateRaw&                          value) {
    if (value.isNull()) {
        result->reset();
    }
    else {
        *result = value.asElemRef().theCharArray();
    }
}

} // unnamed namespace
                          // ------------------------
                          // class bcem_AggregateUtil
                          // ------------------------

// PRIVATE CLASS METHODS

// Conversion From Primitive Types
template <typename PRIMITIVE_TYPE>
inline
int fromAggregatePrimitiveImp(
        PRIMITIVE_TYPE                                 *result,
        const bcem_Aggregate&                           aggregate,
        int                                             fieldId)
{
    bcem_AggregateRaw data = aggregate.rawData();
    bcem_AggregateError error;
    if (0 != data.descendIntoFieldById(&error, fieldId)) {
        return error.code();
    }
    assignPrimitive(result, data);
    return 0;    
}

int bcem_AggregateUtil::fromAggregatePrimitive(
                                      bool                       *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
                                      char                       *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
                                      short                      *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
                                      int                        *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
                                      float                      *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
                                      double                     *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
                                      unsigned char              *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
                                      unsigned short             *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
                                      unsigned int               *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
                                      bsl::string                *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
                                      bsls_PlatformUtil::Int64   *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
                                      bsls_PlatformUtil::Uint64  *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
                                      bdet_Date                  *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId)
{
     return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
                                      bdet_DateTz                *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
                                      bdet_Datetime              *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
                                      bdet_DatetimeTz            *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
                                      bdet_Time                  *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
                                      bdet_TimeTz                *result,
                                      const bcem_Aggregate&       aggregate,
                                      int                         fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Conversion From Nullable Primitive Types

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableValue<bool>                      *result,
        const bcem_Aggregate&                           aggregate,
        int                                             fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableValue<char>                      *result,
        const bcem_Aggregate&                           aggregate,
        int                                             fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableValue<short>                     *result,
        const bcem_Aggregate&                           aggregate,
        int                                             fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableValue<int>                       *result,
        const bcem_Aggregate&                           aggregate,
        int                                             fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableValue<float>                     *result,
        const bcem_Aggregate&                           aggregate,
        int                                             fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableValue<double>                    *result,
        const bcem_Aggregate&                           aggregate,
        int                                             fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableValue<unsigned char>             *result,
        const bcem_Aggregate&                           aggregate,
        int                                             fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableValue<unsigned short>            *result,
        const bcem_Aggregate&                           aggregate,
        int                                             fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableValue<unsigned int>              *result,
        const bcem_Aggregate&                           aggregate,
        int                                             fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableValue<bsl::string>               *result,
        const bcem_Aggregate&                           aggregate,
        int                                             fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableValue<bsls_PlatformUtil::Int64>  *result,
        const bcem_Aggregate&                           aggregate,
        int                                             fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableValue<bsls_PlatformUtil::Uint64> *result,
        const bcem_Aggregate&                           aggregate,
        int                                             fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableValue<bdet_DateTz>               *result,
        const bcem_Aggregate&                           aggregate,
        int                                             fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableValue<bdet_DatetimeTz>           *result,
        const bcem_Aggregate&                           aggregate,
        int                                             fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableValue<bdet_TimeTz>               *result,
        const bcem_Aggregate&                           aggregate,
        int                                             fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableValue<bdet_Date>                 *result,
        const bcem_Aggregate&                           aggregate,
        int                                             fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableValue<bdet_Datetime>             *result,
        const bcem_Aggregate&                           aggregate,
        int                                             fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableValue<bdet_Time>                 *result,
        const bcem_Aggregate&                           aggregate,
        int                                             fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
            bdeut_NullableValue< bsl::vector<char> >       *result,
            const bcem_Aggregate&                           aggregate,
            int                                             fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Conversion From Nullable-Allocated Primitive Types

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableAllocatedValue<bool>                      *result,
        const bcem_Aggregate&                                    aggregate,
        int                                                      fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableAllocatedValue<char>                      *result,
        const bcem_Aggregate&                                    aggregate,
        int                                                      fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableAllocatedValue<short>                     *result,
        const bcem_Aggregate&                                    aggregate,
        int                                                      fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableAllocatedValue<int>                       *result,
        const bcem_Aggregate&                                    aggregate,
        int                                                      fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableAllocatedValue<float>                     *result,
        const bcem_Aggregate&                                    aggregate,
        int                                                      fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableAllocatedValue<double>                    *result,
        const bcem_Aggregate&                                    aggregate,
        int                                                      fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableAllocatedValue<unsigned char>             *result,
        const bcem_Aggregate&                                    aggregate,
        int                                                      fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableAllocatedValue<unsigned short>            *result,
        const bcem_Aggregate&                                    aggregate,
        int                                                      fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableAllocatedValue<unsigned int>              *result,
        const bcem_Aggregate&                                    aggregate,
        int                                                      fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableAllocatedValue<bsl::string>               *result,
        const bcem_Aggregate&                                    aggregate,
        int                                                      fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableAllocatedValue<bsls_PlatformUtil::Int64>  *result,
        const bcem_Aggregate&                                    aggregate,
        int                                                      fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableAllocatedValue<bsls_PlatformUtil::Uint64> *result,
        const bcem_Aggregate&                                    aggregate,
        int                                                      fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableAllocatedValue<bdet_DateTz>               *result,
        const bcem_Aggregate&                                    aggregate,
        int                                                      fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableAllocatedValue<bdet_DatetimeTz>           *result,
        const bcem_Aggregate&                                    aggregate,
        int                                                      fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableAllocatedValue<bdet_TimeTz>               *result,
        const bcem_Aggregate&                                    aggregate,
        int                                                      fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableAllocatedValue<bdet_Date>                 *result,
        const bcem_Aggregate&                                    aggregate,
        int                                                      fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableAllocatedValue<bdet_Datetime>             *result,
        const bcem_Aggregate&                                    aggregate,
        int                                                      fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bdeut_NullableAllocatedValue<bdet_Time>                 *result,
        const bcem_Aggregate&                                    aggregate,
        int                                                      fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
            bdeut_NullableAllocatedValue< bsl::vector<char> >       *result,
            const bcem_Aggregate&                                    aggregate,
            int                                                      fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Conversion From Arrays Of Primitive Types
template <typename TYPE>
inline
int fromAggregatePrimitiveImp(
        bsl::vector<TYPE>                    *result,
        const bcem_Aggregate&                 aggregate,
        int                                   fieldId)
{
    bcem_AggregateRaw data = aggregate.rawData();
    bcem_AggregateError error;
    if (0 != data.descendIntoFieldById(&error, fieldId)) {
        return error.code();
    }
    
    if (isPrimitiveArrayType<TYPE>(data.dataType())) {
        assignArray(result, data);
        return 0;
    }

    const int length = data.length(); 
    if (length < 0) {
        // error code rather than length
        return length;
    }

    result->clear();
    result->reserve(length);
    for (int i = 0; i < length; ++i) {
        bcem_AggregateRaw element;
        int rc = data.getField(&element, &error, false, i);
        BSLS_ASSERT(0 == rc);

        result->push_back(TYPE());
        assignPrimitive(&result->back(), element);
    }
    return 0;
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector<bool>                      *result,
        const bcem_Aggregate&                   aggregate,
        int                                     fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector<char>                      *result,
        const bcem_Aggregate&                   aggregate,
        int                                     fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector<short>                     *result,
        const bcem_Aggregate&                   aggregate,
        int                                     fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector<int>                       *result,
        const bcem_Aggregate&                   aggregate,
        int                                     fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector<float>                     *result,
        const bcem_Aggregate&                   aggregate,
        int                                     fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector<double>                    *result,
        const bcem_Aggregate&                   aggregate,
        int                                     fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector<unsigned char>             *result,
        const bcem_Aggregate&                   aggregate,
        int                                     fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector<unsigned short>            *result,
        const bcem_Aggregate&                   aggregate,
        int                                     fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector<unsigned int>              *result,
        const bcem_Aggregate&                   aggregate,
        int                                     fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector<bsl::string>               *result,
        const bcem_Aggregate&                   aggregate,
        int                                     fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector<bsls_PlatformUtil::Int64>  *result,
        const bcem_Aggregate&                   aggregate,
        int                                     fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}


int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector<bsls_PlatformUtil::Uint64> *result,
        const bcem_Aggregate&                   aggregate,
        int                                     fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector<bdet_DateTz>               *result,
        const bcem_Aggregate&                   aggregate,
        int                                     fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector<bdet_DatetimeTz>           *result,
        const bcem_Aggregate&                   aggregate,
        int                                     fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector<bdet_TimeTz>               *result,
        const bcem_Aggregate&                   aggregate,
        int                                     fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector<bdet_Date>                 *result,
        const bcem_Aggregate&                   aggregate,
        int                                     fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector<bdet_Datetime>             *result,
        const bcem_Aggregate&                   aggregate,
        int                                     fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector<bdet_Time>                 *result,
        const bcem_Aggregate&                   aggregate,
        int                                     fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Conversion From Arrays Of Nullable Primitive Types
int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableValue<bool> > *result,
        const bcem_Aggregate&                     aggregate,
        int                                       fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableValue<char> > *result,
        const bcem_Aggregate&                     aggregate,
        int                                       fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableValue<short> > *result,
        const bcem_Aggregate&                      aggregate,
        int                                        fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableValue<int> > *result,
        const bcem_Aggregate&                    aggregate,
        int                                      fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableValue<float> > *result,
        const bcem_Aggregate&                      aggregate,
        int                                        fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableValue<double> > *result,
        const bcem_Aggregate&                       aggregate,
        int                                         fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableValue<unsigned char> > *result,
        const bcem_Aggregate&                              aggregate,
        int                                                fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableValue<unsigned short> > *result,
        const bcem_Aggregate&                               aggregate,
        int                                                 fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableValue<unsigned int> > *result,
        const bcem_Aggregate&                             aggregate,
        int                                               fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableValue<bsl::string> > *result,
        const bcem_Aggregate&                            aggregate,
        int                                              fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableValue<bsls_PlatformUtil::Int64> > *result,
        const bcem_Aggregate&                                        aggregate,
        int                                                          fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableValue<bsls_PlatformUtil::Uint64> > *result,
        const bcem_Aggregate&                                       aggregate,
        int                                                         fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableValue<bdet_DateTz> > *result,
        const bcem_Aggregate&                            aggregate,
        int                                              fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableValue<bdet_DatetimeTz> > *result,
        const bcem_Aggregate&                                aggregate,
        int                                                  fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableValue<bdet_TimeTz> > *result,
        const bcem_Aggregate&                            aggregate,
        int                                              fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableValue<bdet_Date> > *result,
        const bcem_Aggregate&                          aggregate,
        int                                            fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableValue<bdet_Datetime> > *result,
        const bcem_Aggregate&                              aggregate,
        int                                                fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableValue<bdet_Time> > *result,
        const bcem_Aggregate&                          aggregate,
        int                                            fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<bool> > *result,
        const bcem_Aggregate&                              aggregate,
        int                                                fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<char> > *result,
        const bcem_Aggregate&                              aggregate,
        int                                                fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<short> > *result,
        const bcem_Aggregate&                               aggregate,
        int                                                 fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<int> > *result,
        const bcem_Aggregate&                    aggregate,
        int                                      fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<float> > *result,
        const bcem_Aggregate&                               aggregate,
        int                                                 fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<double> > *result,
        const bcem_Aggregate&                                aggregate,
        int                                                  fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<unsigned char> > *result,
        const bcem_Aggregate&                                       aggregate,
        int                                                         fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<unsigned short> > *result,
        const bcem_Aggregate&                                        aggregate,
        int                                                          fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<unsigned int> > *result,
        const bcem_Aggregate&                                      aggregate,
        int                                                        fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<bsl::string> > *result,
        const bcem_Aggregate&                                     aggregate,
        int                                                       fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<bsls_PlatformUtil::Int64> >
                                                                    *result,
        const bcem_Aggregate&                                        aggregate,
        int                                                          fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<bsls_PlatformUtil::Uint64> >
                                                                   *result,
        const bcem_Aggregate&                                       aggregate,
        int                                                         fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<bdet_DateTz> > *result,
        const bcem_Aggregate&                                     aggregate,
        int                                                       fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
       bsl::vector< bdeut_NullableAllocatedValue<bdet_DatetimeTz> > *result,
       const bcem_Aggregate&                                         aggregate,
       int                                                           fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<bdet_TimeTz> > *result,
        const bcem_Aggregate&                                     aggregate,
        int                                                       fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<bdet_Date> > *result,
        const bcem_Aggregate&                                   aggregate,
        int                                                     fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<bdet_Datetime> > *result,
        const bcem_Aggregate&                                       aggregate,
        int                                                         fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
}

int bcem_AggregateUtil::fromAggregatePrimitive(
        bsl::vector< bdeut_NullableAllocatedValue<bdet_Time> > *result,
        const bcem_Aggregate&                                   aggregate,
        int                                                     fieldId)
{
    return fromAggregatePrimitiveImp(result, aggregate, fieldId);
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
    bcem_AggregateRaw data = result->rawData();
    bcem_AggregateError error;
    if (0 != data.descendIntoFieldById(&error, fieldId)) {
        return error.code();
    }
    if (0 != data.setValue(&error, value)) {
        return error.code();
    }
    return 0;
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
    bcem_AggregateRaw data = result->rawData();
    bcem_AggregateError error;
    if (0 != data.descendIntoFieldById(&error, fieldId)) {
        return error.code();
    }


    if (value.isNull()) {
        data.makeNull();
    }
    else if (0 != data.setValue(&error, value.value())) {
        return error.code();
    }

    return 0;
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

    if (value.isNull()) {
        return aggregate.makeNull().errorCode();
    }
    else {
        const bdem_ElemRef& elemRef = aggregate.makeValue().asElemRef();
        if (bdem_ElemType::BDEM_CHAR_ARRAY != elemRef.type()) {
            return bcem_AggregateError::BCEM_ERR_BAD_CONVERSION;
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
    bcem_AggregateRaw data = result->rawData();
    bcem_AggregateError error;
    if (0 != data.descendIntoFieldById(&error, fieldId)) {
        return error.code();
    }


    if (value.isNull()) {
        data.makeNull();
    }
    else if (0 != data.setValue(&error, value.value())) {
        return error.code();
    }

    return 0;
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
        int                                 fieldId, 
        const bsl::vector<PRIMITIVE_TYPE>&  value)
{
    typedef typename bsl::vector<PRIMITIVE_TYPE>::const_iterator ConstIter;

    bcem_AggregateRaw data = result->rawData();
    bcem_AggregateError error;
    if (0 != data.descendIntoFieldById(&error, fieldId)) {
        return error.code();
    }
    
    if (isPrimitiveArrayType<PRIMITIVE_TYPE>(data.dataType())) {
        assignArray(data, value);
        return 0;
    }

    bcem_AggregateRaw dummy;
    data.removeItems(&error, 0, data.length());

    for (ConstIter it = value.begin(); it != value.end(); ++it) {
        if (0 != data.insertItem(&dummy, &error, 
                                 data.length(), *it))
        {
            return error.code();
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
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                               *result,
        int                                           fieldId,
        const bsl::vector<char>&                      value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                               *result,
        int                                           fieldId,
        const bsl::vector<short>&                     value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                               *result,
        int                                           fieldId,
        const bsl::vector<int>&                       value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                               *result,
        int                                           fieldId,
        const bsl::vector<float>&                     value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                               *result,
        int                                           fieldId,
        const bsl::vector<double>&                    value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                               *result,
        int                                           fieldId,
        const bsl::vector<unsigned char>&             value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                               *result,
        int                                           fieldId,
        const bsl::vector<unsigned short>&            value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                               *result,
        int                                           fieldId,
        const bsl::vector<unsigned int>&              value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                               *result,
        int                                           fieldId,
        const bsl::vector<bsl::string>&               value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                               *result,
        int                                           fieldId,
        const bsl::vector<bsls_PlatformUtil::Int64>&  value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                                *result,
        int                                            fieldId,
        const bsl::vector<bsls_PlatformUtil::Uint64>&  value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                               *result,
        int                                           fieldId,
        const bsl::vector<bdet_DateTz>&               value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                               *result,
        int                                           fieldId,
        const bsl::vector<bdet_DatetimeTz>&           value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                               *result,
        int                                           fieldId,
        const bsl::vector<bdet_TimeTz>&               value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                               *result,
        int                                           fieldId,
        const bsl::vector<bdet_Date>&                 value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                               *result,
        int                                           fieldId,
        const bsl::vector<bdet_Datetime>&             value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
}

int bcem_AggregateUtil::toAggregatePrimitive(
        bcem_Aggregate                               *result,
        int                                           fieldId,
        const bsl::vector<bdet_Time>&                 value)
{
    return toAggregatePrimitiveImp(result, fieldId, value);
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
    bcem_AggregateRaw data = result->rawData(), item;
    bcem_AggregateError error;
    if (0 != data.descendIntoFieldById(&error, fieldId)) {
        return error.code();
    }
    typedef typename bsl::vector< bdeut_NullableValue<PRIMITIVE_TYPE>
                                >::const_iterator ConstIter;

    data.removeItems(&error, 0, data.length());

    for (ConstIter it = value.begin(); it != value.end(); ++it) {
        if (0 != data.insertItemRaw(&item, &error, data.length())) {
            return error.code();
        }
        if (it->isNull()) {
            item.makeNull();
        }
        else {
            if (0 != item.setValue(&error, it->value())) {
                return error.code();
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
    bcem_AggregateRaw data = result->rawData(), item;
    bcem_AggregateError error;
    if (0 != data.descendIntoFieldById(&error, fieldId)) {
        return error.code();
    }
    typedef typename bsl::vector< bdeut_NullableAllocatedValue<PRIMITIVE_TYPE>
                                >::const_iterator ConstIter;

    data.removeItems(&error, 0, data.length());

    for (ConstIter it = value.begin(); it != value.end(); ++it) {
        if (0 != data.insertItemRaw(&item, &error, data.length())) {
            return error.code();
        }
        if (it->isNull()) {
            item.makeNull();
        }
        else {
            if (0 != item.setValue(&error, it->value())) {
                return error.code();
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

int bcem_AggregateUtil::setValue(const bcem_AggregateRaw&   result,
                                 const bsl::string&         value)
{
    bcem_AggregateError errorDescription;
    if (0 == result.setValue(&errorDescription, value)) {
        return 0;
    }
    return errorDescription.code();
}

const char *bcem_AggregateUtil::errorString(int errorCode)
{
    switch (errorCode) {
      case bcem_AggregateError::BCEM_ERR_NOT_A_RECORD:
          return "Attempt to access a field (by name, ID, or "
                 "index) of an aggregate that does not reference a "
                 "list, row, choice, or choice array item.";
      case bcem_AggregateError::BCEM_ERR_NOT_A_SEQUENCE:
          return "Attempt to perform a list or row operation on an "
                 "aggregate that does not refer to a list or row "
                 "(e.g., initialize from a non-sequence record def).";
      case bcem_AggregateError::BCEM_ERR_NOT_A_CHOICE:
          return "Attempt to perform a choice or choice array item "
                 "operation (make selection, get selection, etc.) "
                 "on an aggregate that is not a choice or choice "
                 "array item.";
      case bcem_AggregateError::BCEM_ERR_NOT_AN_ARRAY:
          return "Attempt to perform an array operation (index, "
                 "insert, etc.) on an aggregate that is not an "
                 "array, table, or choice array.";
      case bcem_AggregateError::BCEM_ERR_BAD_FIELDNAME:
          return "Field name does not exist in the record def.";
      case bcem_AggregateError::BCEM_ERR_BAD_FIELDID:
          return "Field ID does not exist in record def.";
      case bcem_AggregateError::BCEM_ERR_BAD_FIELDINDEX:
          return "Field index is not a positive integer less than "
                 "the length of the field definition.";
      case bcem_AggregateError::BCEM_ERR_BAD_ARRAYINDEX:
          return "Array (or table) index is out of bounds.";
      case bcem_AggregateError::BCEM_ERR_NOT_SELECTED:
          return "Attempt to access a choice field that is not "
                 "the currently selected object.";
      case bcem_AggregateError::BCEM_ERR_BAD_CONVERSION:
          return "Attempt to set an aggregate using a value that "
                 "is not convertible to the aggregate's type.";
      case bcem_AggregateError::BCEM_ERR_BAD_ENUMVALUE:
          return "Attempt to set the value of an enumeration "
                 "aggregate to a string that is not an enumerator "
                 "name in the enumeration definition or to an "
                 "integer that is not an enumerator ID in the "
                 "enumeration definition. ";
      case bcem_AggregateError::BCEM_ERR_NON_CONFORMANT:
          return "Attempt to set a list, row, table, choice, "
                 "choice array item, or choice array aggregate to "
                 "a value of the correct type, but which doesn't "
                 "conform to the aggregate's record definition.";
      case bcem_AggregateError::BCEM_ERR_AMBIGUOUS_ANON:
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
