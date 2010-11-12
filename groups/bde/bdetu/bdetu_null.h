// bdetu_null.h                   -*-C++-*-
#ifndef INCLUDED_BDETU_NULL
#define INCLUDED_BDETU_NULL

//@PURPOSE: Provide support for null values for fundamental and 'bdet' types.
//
//@CLASSES:
// bdetu_Null: templatized namespace for getting and testing null values
// bdetu_HasNull: meta-function to determine if a type has a null value
//
//@AUTHOR: Paul Staniforth (pstaniforth)
//
//@DESCRIPTION: This component defines special values for ten specific types
// and establishes a convention that these special values will be used as
// "null" values for the corresponding types. The 'bdetu_Null' 'struct'
// provides a utility function, 'nullValue', which returns the null value for
// the instantiation type.  The 'nullValue' function is specialized for each
// of the types supported by this component.  The 'bdetu_HasNull'
// meta-function template evaluates to true for any of the supported types and
// false if the type does not have a 'nullValue' specialization.  Two
// additional 'bdetu_Null' functions are provided as optimizations: 'isNull'
// tests for the null value and 'setNull' sets an object to the null value.
// All functions within the 'bdetu_Null' struct are class (static) methods.
//
// For reference, the actual null values are as follows:
//..
//    Type            Null Value
//    -----------     ----------
//    bool            false
//
//    char            (char) 0
//
//    short           -32768
//                    The smallest possible signed
//                    2's complement 16 bit value.
//
//    int             -2147483648
//                    The smallest possible signed
//                    2's complement 32 bit value.
//
//    Int64           -9223372036854775808
//                    The smallest possible signed
//                    2's complement 64 bit value.
//
//    float           -7568.0/(1<<30)/(1<<30)
//                    An arbitary small negative
//                    number.
//
//    double          -27953.0/(1<<30)/(1<<30)
//                    An arbitary small negative
//                    number.
//
//    std::string     an empty (zero-length) string
//
//    bdet_Datetime   the default-constructed value
//
//    bdet_DatetimeTz the default-constructed value
//
//    bdet_Date       the default-constructed value
//
//    bdet_DateTz     the default-constructed value
//
//    bdet_Time       the default-constructed value
//
//    bdet_TimeTz     the default-constructed value
//..
///USAGE
///-----
// The following snippets of code illustrate how to use the null value for the
// type 'double'.  First, initialize 'nullValue' with the null value for
// 'double' and initialize a non-null reference 'value' to 0.0.
//..
//    double nullValue = bdetu_Null<double>::nullValue();
//    double value     = 0.0;
//..
// Test if 'value' is null; it should *not* be null.
//..
//    assert(bdetu_Null<double>::isNull(value) == 0);
//..
// Set 'value' to the null value using '.
//..
//    bdetu_Null<double>::setNull(value);
//..
// Test if 'value' is now null; it should now be.
//..
//    assert(bdetu_Null<double>::isNull(value) == 1);
//    assert(value == nullValue);
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_STRING
#include <string>
#define INCLUDED_STRING
#endif

#ifndef INCLUDED_BDEMF_ASSERT
#include <bdemf_assert.h>
#endif

#ifndef INCLUDED_BDEMF_ISSAME
#include <bdemf_issame.h>
#endif

#ifndef INCLUDED_BDES_PLATFORMUTIL
#include <bdes_platformutil.h>
#endif

#ifndef INCLUDED_BDET_DATETIME
#include <bdet_datetime.h>
#endif

#ifndef INCLUDED_BDET_DATETIMETZ
#include <bdet_datetimetz.h>
#endif

#ifndef INCLUDED_BDET_DATE
#include <bdet_date.h>
#endif

#ifndef INCLUDED_BDET_DATETZ
#include <bdet_datetz.h>
#endif

#ifndef INCLUDED_BDET_TIME
#include <bdet_time.h>
#endif

#ifndef INCLUDED_BDET_TIMETZ
#include <bdet_timetz.h>
#endif
namespace BloombergLP {


                        // ========================
                        // class bdetu_HasNullValue
                        // ========================

template <typename TYPE>
struct bdetu_HasNullValue {
    // Meta-function returns a true 'VALUE' at compile-time if 'TYPE' has a
    // null value defined in this component.

    enum {
        VALUE =    bdemf_IsSame<TYPE, bool>::VALUE
                || bdemf_IsSame<TYPE, char>::VALUE
                || bdemf_IsSame<TYPE, short>::VALUE
                || bdemf_IsSame<TYPE, int>::VALUE
                || bdemf_IsSame<TYPE, bdes_PlatformUtil::Int64>::VALUE
                || bdemf_IsSame<TYPE, float>::VALUE
                || bdemf_IsSame<TYPE, double>::VALUE
                || bdemf_IsSame<TYPE, std::string>::VALUE
                || bdemf_IsSame<TYPE, bdet_Datetime>::VALUE
                || bdemf_IsSame<TYPE, bdet_DatetimeTz>::VALUE
                || bdemf_IsSame<TYPE, bdet_Date>::VALUE
                || bdemf_IsSame<TYPE, bdet_DateTz>::VALUE
                || bdemf_IsSame<TYPE, bdet_Time>::VALUE
                || bdemf_IsSame<TYPE, bdet_TimeTz>::VALUE
    };
};


                        // ================
                        // class bdetu_Null
                        // ================

template <class TYPE>
struct bdetu_Null {
    // Implemenation structure for holding static inline functions for ten
    // fundamental and 'bdet' scalars.  Note that there is never any need for a
    // user of this component to instantiate an instance of this type.

    static TYPE nullValue();
        // Return the null value for the parameterized 'TYPE' if 'TYPE" is one
        // of the ten supported types; generate a compile time error otherwise.

    static int isNull(const TYPE& value);
        // Return 1 if the specified 'value' is equal to the null value for the
        // parameterized 'TYPE'.  Specializations are defined for this function
        // for non-fundamental types.  This is functionally equivalent to:
        // bdetu_Null<TYPE>::nullValue() == value

    static void setNull(TYPE *value);
        // Set the specified 'value' equal to the null value.  This is
        // functionally equivalent to: *value = bdetu_Null<TYPE>::nullValue();
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

template <class TYPE>
inline int bdetu_Null<TYPE>::isNull(const TYPE& value)
{
    return value == nullValue();
}

template <class TYPE>
inline void bdetu_Null<TYPE>::setNull(TYPE *value)
{
    *value = nullValue();
}

// PRIVATE CONSTANTS
namespace {
    // Assert that this platform uses twos-compliment integer representation:
    BDEMF_ASSERT(~0 == -1);

    // Integral null values are the smallest negative value that fits
    // in a specific number of bits.
    const short bdetu_Null_ShortValue = -0x7fff - 1;               // 16-bits
    const int   bdetu_Null_IntValue   = -0x7fffffff - 1;           // 32-bits
    const bdes_PlatformUtil::Int64
                bdetu_Null_Int64Value = -0x7fffffffffffffffLL - 1; // 64-bits

    // Arbitrary, unlikely floating-point values.
    const float  bdetu_Null_FloatValue  = -7568.0/(1<<30)/(1<<30);
    const double bdetu_Null_DoubleValue = -27953.0/(1<<30)/(1<<30);
}

template <>
inline bool bdetu_Null<bool>::nullValue()
    // Return the null value for the type 'bool'.
{
    return false;
}

template <>
inline char bdetu_Null<char>::nullValue()
    // Return the null value for the type 'char'.
{
    return 0;
}

template <>
inline short bdetu_Null<short>::nullValue()
    // Return the null value for the type 'short'.
{
    return bdetu_Null_ShortValue;
}

template <>
inline int bdetu_Null<int>::nullValue()
    // Return the null value for the type 'int'.
{
    return bdetu_Null_IntValue;
}

template <>
inline bdes_PlatformUtil::Int64
    bdetu_Null<bdes_PlatformUtil::Int64>::nullValue()
    // Return the null value for the type 'bdes_PlatformUtil::Int64'.
{
    return bdetu_Null_Int64Value;
}

template <>
inline float bdetu_Null<float>::nullValue()
{
    return bdetu_Null_FloatValue;
}

template <>
inline double bdetu_Null<double>::nullValue()
{
    return bdetu_Null_DoubleValue;
}

template <>
inline std::string bdetu_Null<std::string>::nullValue()
{
    // null value for 'bdet_String' is defined as a string with zero length.

    return std::string();
}

template <>
inline bdet_Date bdetu_Null<bdet_Date>::nullValue()
{
    // null value for 'bdet_Date' is defined as the value created by the
    // default constructor.

    return bdet_Date();
}

template <>
inline bdet_DateTz bdetu_Null<bdet_DateTz>::nullValue()
{
    // null value for 'bdet_DateTz' is defined as the value created by the
    // default constructor.

    return bdet_DateTz();
}

template <>
inline bdet_Time bdetu_Null<bdet_Time>::nullValue()
{
    // null value for 'bdet_Time' is defined as the value created by the
    // default constructor.

    return bdet_Time();
}

template <>
inline bdet_TimeTz bdetu_Null<bdet_TimeTz>::nullValue()
{
    // null value for 'bdet_TimeTz' is defined as the value created by the
    // default constructor.

    return bdet_TimeTz();
}

template <>
inline bdet_Datetime bdetu_Null<bdet_Datetime>::nullValue()
{
    // null value for 'bdet_Datetime' is defined as the value created by the
    // default constructor.

    return bdet_Datetime();
}

template <>
inline bdet_DatetimeTz bdetu_Null<bdet_DatetimeTz>::nullValue()
{
    // null value for 'bdet_DatetimeTz' is defined as the value created by the
    // default constructor.

    return bdet_DatetimeTz();
}

template <>
inline int bdetu_Null<bool>::isNull(const bool& value)
    // Return true if 'value' is the null value.
{
    return false == value;
}

template <>
inline int bdetu_Null<char>::isNull(const char& value)
    // Specialized optimization for 'isNull' function.
{
    return 0 == value;
}

template <>
inline int bdetu_Null<short>::isNull(const short& value)
    // Specialized optimization for 'isNull' function.
{
    return bdetu_Null_ShortValue == value;
}

template <>
inline int bdetu_Null<int>::isNull(const int& value)
    // Specialized optimization for 'isNull' function.
{
    return bdetu_Null_IntValue == value;
}

template <>
inline int bdetu_Null<bdes_PlatformUtil::Int64>::isNull(
    const bdes_PlatformUtil::Int64& value)
    // Specialized optimization for 'isNull' function.
{
    return bdetu_Null_Int64Value == value;
}

template <>
inline int bdetu_Null<float>::isNull(const float& value)
    // Specialized optimization for 'isNull' function.
{
    return bdetu_Null_FloatValue == value;
}

template <>
inline int bdetu_Null<double>::isNull(const double& value)
    // Specialized optimization for 'isNull' function.
{
    return bdetu_Null_DoubleValue == value;
}

template <>
inline int bdetu_Null<std::string>::isNull(const std::string& value)
    // Specialized optimization for 'isNull' function.
{
    return value.length() == 0;
}

template <>
inline void bdetu_Null<std::string>::setNull(std::string *value)
    // set the specified 'value' equal to the null value.
{
    // This specialization provides a more optimal implementation of setNull()
    // for 'std::string'.
    value->erase();
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
