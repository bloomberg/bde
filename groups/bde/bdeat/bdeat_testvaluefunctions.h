// bdeat_testvaluefunctions.h                  -*-C++-*-
#ifndef INCLUDED_BDEAT_TESTVALUEFUNCTIONS
#define INCLUDED_BDEAT_TESTVALUEFUNCTIONS

//@PURPOSE: Provide a namespace for functions that generate test values.
//
//@CLASSES:
//   bdeat_TestValueFunctions: namespace for functions that supply test values
//
//@SEE_ALSO:
//
//@AUTHOR: Clay Wilson (cwilson9)
//
//@DESCRIPTION:
// This component provides a 'namespace', 'bdeat_TestValueFunctions', defining
// functions that generate test values for objects of sequence, choice, and
// enumeration type, as well as values for objects of any type that may be
// sequence attributes or choice selections.  Two arguments are supplied to
// the value-generating functions: the address of an object for which a value
// is to be generated and a non-negative integer index.  Each function
// generates a limited number of values, but in most cases this limit is quite
// large (e.g., INT_MAX).  If the supplied index corresponds to one of the
// available values, then the supplied object is assigned that value and 0 is
// returned.  If the supplied index does not correspond to any value (i.e., the
// index is out of range), a non-zero value is returned with no effect on the
// object.
//
///Usage
///-----
// TBD doc

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEAT_ATTRIBUTEINFO
#include <bdeat_attributeinfo.h>
#endif

#ifndef INCLUDED_BDEAT_ARRAYFUNCTIONS
#include <bdeat_arrayfunctions.h>
#endif

#ifndef INCLUDED_BDEAT_CHOICEFUNCTIONS
#include <bdeat_choicefunctions.h>
#endif

#ifndef INCLUDED_BDEAT_ENUMERATORINFO
#include <bdeat_enumeratorinfo.h>
#endif

#ifndef INCLUDED_BDEAT_ENUMFUNCTIONS
#include <bdeat_enumfunctions.h>
#endif

#ifndef INCLUDED_BDEAT_SEQUENCEFUNCTIONS
#include <bdeat_sequencefunctions.h>
#endif

#ifndef INCLUDED_BDEAT_TYPECATEGORY
#include <bdeat_typecategory.h>
#endif

#ifndef INCLUDED_BDET_DATE
#include <bdet_date.h>
#endif

#ifndef INCLUDED_BDET_DATETIME
#include <bdet_datetime.h>
#endif

#ifndef INCLUDED_BDET_TIME
#include <bdet_time.h>
#endif

#ifndef INCLUDED_BDEMF_ASSERT
#include <bdemf_assert.h>
#endif

#ifndef INCLUDED_BDES_ASSERT
#include <bdes_assert.h>
#endif

#ifndef INCLUDED_BDES_PLATFORM
#include <bdes_platform.h>
#endif

#ifndef INCLUDED_BDES_PLATFORMUTIL
#include <bdes_platformutil.h>
#endif

#ifndef INCLUDED_CLIMITS
#include <climits>
#define INCLUDED_CLIMITS
#endif

#ifndef INCLUDED_STDIO
#include <stdio.h>       // 'snprintf'
#define INCLUDED_STDIO
#endif

#ifndef INCLUDED_STRING
#include <string>
#define INCLUDED_STRING
#endif

#ifndef INCLUDED_VECTOR
#include <vector>
#define INCLUDED_VECTOR
#endif

namespace BloombergLP {

                  // ===========================================
                  // private struct bdeat_TestValueFunctions_Imp
                  // ===========================================

struct bdeat_TestValueFunctions_Imp {
    // This 'struct' provides some of the implementation details for the
    // functions in the 'bdeat_TestValueFunctions' namespace.

    int d_index;  // index of test value to generate

    // TYPES
    enum { SUCCESS = 0, FAILURE = -1 };
        // Return values for the methods in this 'struct'.

    // CLASS METHODS
    template <typename TYPE>
    static
    int loadTestValue(TYPE *object, int index, bdeat_TypeCategory::Array);
        // Load into the specified Array 'object' a test value indicated by the
        // specified 'index'.  Return 0 on success, and a non-zero value
        // otherwise with no effect on 'object'.  The behavior is undefined
        // unless index >= 0.

    template <typename TYPE>
    static
    int loadTestValue(TYPE *object, int index, bdeat_TypeCategory::Choice);
        // Load into the specified Choice 'object' a test value indicated by
        // the specified 'index'.  Return 0 on success, and a non-zero value
        // otherwise with no effect on 'object'.  The behavior is undefined
        // unless index >= 0.

    template <typename TYPE>
    static
    int loadTestValue(TYPE *object, int index,
                      bdeat_TypeCategory::Enumeration);
        // Load into the specified Enumeration 'object' a test value indicated
        // by the specified 'index'.  Return 0 on success, and a non-zero value
        // otherwise with no effect on 'object'.  The behavior is undefined
        // unless index >= 0.

    template <typename TYPE>
    static
    int loadTestValue(TYPE *object, int index, bdeat_TypeCategory::Sequence);
        // Load into the specified Sequence 'object' a test value indicated by
        // the specified 'index'.  Return 0 on success, and a non-zero value
        // otherwise with no effect on 'object'.  The behavior is undefined
        // unless index >= 0.

    // MANIPULATORS
    template <typename TYPE, typename INFO>
    int operator()(TYPE *object, const INFO&);
        // Load a test value into the specified 'object' of parameterized
        // 'TYPE'.  Return 0 on success, and a non-zero value otherwise.
};

                    // ==================================
                    // namespace bdeat_TestValueFunctions
                    // ==================================

namespace bdeat_TestValueFunctions {
    // The functions provided in this 'namespace' supply test values for
    // objects of sequence, choice, and enumeration type, as well as values
    // for objects of any type that may be sequence attributes or choice
    // selections.

    // TYPES
    enum { SUCCESS = 0, FAILURE = -1 };
        // Return values for the functions in this 'namespace'.

    // MANIPULATORS
    template <typename TYPE>
    int loadTestValue(TYPE *object, int index);
        // Load into the specified 'object' a test value indicated by the
        // specified 'index'.  Return 0 on success, and a non-zero value
        // otherwise with no effect on 'object'.  The behavior is undefined
        // unless index >= 0.

    int loadTestValue(bool *object, int index);
        // Load into the specified 'object' a test value of 'bool' type
        // indicated by the specified 'index'.  Return 0 on success, and a
        // non-zero value otherwise with no effect on 'object'.  The behavior
        // is undefined unless index >= 0.

    int loadTestValue(char *object, int index);
        // Load into the specified 'object' a test value of 'char' type
        // indicated by the specified 'index'.  Return 0 on success, and a
        // non-zero value otherwise with no effect on 'object'.  The behavior
        // is undefined unless index >= 0.

    int loadTestValue(signed char *object, int index);
        // Load into the specified 'object' a test value of 'signed char' type
        // indicated by the specified 'index'.  Return 0 on success, and a
        // non-zero value otherwise with no effect on 'object'.  The behavior
        // is undefined unless index >= 0.

    int loadTestValue(unsigned char *object, int index);
        // Load into the specified 'object' a test value of 'unsigned char'
        // type indicated by the specified 'index'.  Return 0 on success, and a
        // non-zero value otherwise with no effect on 'object'.  The behavior
        // is undefined unless index >= 0.

    int loadTestValue(short *object, int index);
        // Load into the specified 'object' a test value of 'short' type
        // indicated by the specified 'index'.  Return 0 on success, and a
        // non-zero value otherwise with no effect on 'object'.  The behavior
        // is undefined unless index >= 0.

    int loadTestValue(unsigned short *object, int index);
        // Load into the specified 'object' a test value of 'unsigned short'
        // type indicated by the specified 'index'.  Return 0 on success, and a
        // non-zero value otherwise with no effect on 'object'.  The behavior
        // is undefined unless index >= 0.

    int loadTestValue(int *object, int index);
        // Load into the specified 'object' a test value of 'int' type
        // indicated by the specified 'index'.  Return 0 on success, and a
        // non-zero value otherwise with no effect on 'object'.  The behavior
        // is undefined unless index >= 0.

    int loadTestValue(unsigned int *object, int index);
        // Load into the specified 'object' a test value of 'unsigned int' type
        // indicated by the specified 'index'.  Return 0 on success, and a
        // non-zero value otherwise with no effect on 'object'.  The behavior
        // is undefined unless index >= 0.

    int loadTestValue(bdes_PlatformUtil::Int64 *object, int index);
        // Load into the specified 'object' a test value of 'long long' type
        // indicated by the specified 'index'.  Return 0 on success, and a
        // non-zero value otherwise with no effect on 'object'.  The behavior
        // is undefined unless index >= 0.

    int loadTestValue(bdes_PlatformUtil::Uint64 *object, int index);
        // Load into the specified 'object' a test value of
        // 'unsigned long long' type indicated by the specified 'index'.
        // Return 0 on success, and a non-zero value otherwise with no effect
        // on 'object'.  The behavior is undefined unless index >= 0.

    int loadTestValue(float *object, int index);
        // Load into the specified 'object' a test value of 'float' type
        // indicated by the specified 'index'.  Return 0 on success, and a
        // non-zero value otherwise with no effect on 'object'.  The behavior
        // is undefined unless index >= 0.

    int loadTestValue(double *object, int index);
        // Load into the specified 'object' a test value of 'double' type
        // indicated by the specified 'index'.  Return 0 on success, and a
        // non-zero value otherwise with no effect on 'object'.  The behavior
        // is undefined unless index >= 0.

    int loadTestValue(std::string *object, int index);
        // Load into the specified 'object' a test value of 'std::string' type
        // indicated by the specified 'index'.  Return 0 on success, and a
        // non-zero value otherwise with no effect on 'object'.  The behavior
        // is undefined unless index >= 0.

    int loadTestValue(bdet_Date *object, int index);
        // Load into the specified 'object' a test value of 'bdet_Date' type
        // indicated by the specified 'index'.  Return 0 on success, and a
        // non-zero value otherwise with no effect on 'object'.  The behavior
        // is undefined unless index >= 0.

    int loadTestValue(bdet_Datetime *object, int index);
        // Load into the specified 'object' a test value of 'bdet_Datetime'
        // type indicated by the specified 'index'.  Return 0 on success, and a
        // non-zero value otherwise with no effect on 'object'.  The behavior
        // is undefined unless index >= 0.

    int loadTestValue(bdet_Time *object, int index);
        // Load into the specified 'object' a test value of 'bdet_Time' type
        // indicated by the specified 'index'.  Return 0 on success, and a
        // non-zero value otherwise with no effect on 'object'.  The behavior
        // is undefined unless index >= 0.

}  // close namespace bdeat_TestValueFunctions

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                // -------------------------------------------
                // private struct bdeat_TestValueFunctions_Imp
                // -------------------------------------------

// CLASS METHODS

template <typename TYPE>
inline
int bdeat_TestValueFunctions_Imp::loadTestValue(TYPE *object,
                                                int   index,
                                                bdeat_TypeCategory::Array)
{
    BDEMF_ASSERT((bdeat_ArrayFunctions::IsArray<TYPE>::VALUE));
    BDE_ASSERT_H(index >= 0);

    bdeat_ArrayFunctions::resize(object, index);

    return SUCCESS;
}

template <typename TYPE>
inline
int bdeat_TestValueFunctions_Imp::loadTestValue(TYPE *object,
                                                int   index,
                                                bdeat_TypeCategory::Choice)
{
    BDEMF_ASSERT((bdeat_ChoiceFunctions::IsChoice<TYPE>::VALUE));
    BDE_ASSERT_H(index >= 0);

    const int numSelections = bdeat_ChoiceFunctions::numSelections(*object);

    if (0 == index) {
        return bdeat_ChoiceFunctions::makeSelection(object, 0);       // RETURN
    }
    const int selectionId = index % numSelections + 1;
    if (bdeat_ChoiceFunctions::makeSelection(object, selectionId)) {
        return FAILURE;                                               // RETURN
    }

    bdeat_TestValueFunctions_Imp manipulator = { index - 1 };  // index > 0
    return bdeat_ChoiceFunctions::manipulateSelection(object, manipulator);
}

template <typename TYPE>
inline
int bdeat_TestValueFunctions_Imp::loadTestValue(
                                               TYPE *object,
                                               int   index,
                                               bdeat_TypeCategory::Enumeration)
{
    BDEMF_ASSERT((bdeat_EnumFunctions::IsEnumeration<TYPE>::VALUE));
    BDE_ASSERT_H(index >= 0);

    if (index >= bdeat_EnumFunctions::numEnumerators(*object)) {
        return FAILURE;                                               // RETURN
    }
    bdeat_EnumeratorInfo info =
                           bdeat_EnumFunctions::enumeratorInfo(*object, index);
    *object = static_cast<TYPE>(info.value());
    return SUCCESS;
}

template <typename TYPE>
inline
int bdeat_TestValueFunctions_Imp::loadTestValue(TYPE *object,
                                                int   index,
                                                bdeat_TypeCategory::Sequence)
{
    BDEMF_ASSERT((bdeat_SequenceFunctions::IsSequence<TYPE>::VALUE));
    BDE_ASSERT_H(index >= 0);

    const int numAttributes = bdeat_SequenceFunctions::numAttributes(*object);

    int numRollover = 0;
    for (int i = 0; i < numAttributes; ++i) {
        bdeat_AttributeInfo info =
                            bdeat_SequenceFunctions::attributeInfo(*object, i);
        bdeat_TestValueFunctions_Imp manipulator = { index };
        if (bdeat_SequenceFunctions::manipulateAttribute(object,
                                                         manipulator,
                                                         info.id())) {
            BDE_ASSERT_H(index > 0);
            manipulator.d_index = 0;
            if (bdeat_SequenceFunctions::manipulateAttribute(object,
                                                             manipulator,
                                                             info.id())) {
                return FAILURE;                                       // RETURN
            }
            ++numRollover;
        }
    }
    if (numAttributes == numRollover) {  // equivalent to index == 0 on entry
        return FAILURE;                                               // RETURN
    }
    return SUCCESS;
}

// MANIPULATORS

template <typename TYPE, typename INFO>
inline
int bdeat_TestValueFunctions_Imp::operator()(TYPE *object, const INFO&)
{
    return bdeat_TestValueFunctions::loadTestValue(object, d_index);
}

                     // ----------------------------------
                     // namespace bdeat_TestValueFunctions
                     // ----------------------------------

// MANIPULATORS

template <typename TYPE>
inline
int bdeat_TestValueFunctions::loadTestValue(TYPE *object, int index)
{
    typedef typename bdeat_TypeCategory::Select<TYPE>::Type Type;

    return bdeat_TestValueFunctions_Imp::loadTestValue(object, index, Type());
}

inline
int bdeat_TestValueFunctions::loadTestValue(bool *object, int index)
{
    BDE_ASSERT_H(index >= 0);

    if (index > 1) {
        return FAILURE;                                               // RETURN
    }
    *object = index ? true : false;
    return SUCCESS;
}

inline
int bdeat_TestValueFunctions::loadTestValue(char *object, int index)
{
    BDE_ASSERT_H(index >= 0);

    if (index > UCHAR_MAX) {
        return FAILURE;                                               // RETURN
    }

    int value;
    if (index + '!' <= UCHAR_MAX) {
        value = index + '!';  // start with first printable char beyond ' '
    }
    else {
        value = index - (UCHAR_MAX - '!' + 1);
    }
    *object = static_cast<char>(value);
    return SUCCESS;
}

inline
int bdeat_TestValueFunctions::loadTestValue(signed char *object, int index)
{
    BDE_ASSERT_H(index >= 0);

    return loadTestValue(reinterpret_cast<char *>(object), index);
}

inline
int bdeat_TestValueFunctions::loadTestValue(unsigned char *object, int index)
{
    BDE_ASSERT_H(index >= 0);

    return loadTestValue(reinterpret_cast<char *>(object), index);
}

inline
int bdeat_TestValueFunctions::loadTestValue(short *object, int index)
{
    BDE_ASSERT_H(index >= 0);

    if (index > USHRT_MAX) {
        return FAILURE;                                               // RETURN
    }

    *object = static_cast<short>(index);
    return SUCCESS;
}

inline
int bdeat_TestValueFunctions::loadTestValue(unsigned short *object, int index)
{
    BDE_ASSERT_H(index >= 0);

    if (index > USHRT_MAX) {
        return FAILURE;                                               // RETURN
    }

    *object = static_cast<unsigned short>(index);
    return SUCCESS;
}

inline
int bdeat_TestValueFunctions::loadTestValue(int *object, int index)
{
    BDE_ASSERT_H(index >= 0);

    *object = index;
    return SUCCESS;
}

inline
int bdeat_TestValueFunctions::loadTestValue(unsigned int *object, int index)
{
    BDE_ASSERT_H(index >= 0);

    *object = static_cast<unsigned int>(index);
    return SUCCESS;
}

inline
int bdeat_TestValueFunctions::loadTestValue(bdes_PlatformUtil::Int64 *object,
                                            int                       index)
{
    BDE_ASSERT_H(index >= 0);

    *object = index;
    return SUCCESS;
}

inline
int bdeat_TestValueFunctions::loadTestValue(bdes_PlatformUtil::Uint64 *object,
                                            int                        index)
{
    BDE_ASSERT_H(index >= 0);

    *object = static_cast<bdes_PlatformUtil::Uint64>(index);
    return SUCCESS;
}

inline
int bdeat_TestValueFunctions::loadTestValue(float *object, int index)
{
    BDE_ASSERT_H(index >= 0);

    *object = index;
    return SUCCESS;
}

inline
int bdeat_TestValueFunctions::loadTestValue(double *object, int index)
{
    BDE_ASSERT_H(index >= 0);

    *object = index;
    return SUCCESS;
}

#if defined(BDES_PLATFORM__CMP_MSVC)
#define snprintf _snprintf
#endif

inline
int bdeat_TestValueFunctions::loadTestValue(std::string *object, int index)
{
    BDE_ASSERT_H(index >= 0);

    const int BUFSIZE = 64;
    char buffer[BUFSIZE];

    if (snprintf(buffer, BUFSIZE, "%d", index) > BUFSIZE) {
        return FAILURE;                                               // RETURN
    }
    buffer[BUFSIZE - 1] = '\0';
    *object = buffer;
    return SUCCESS;
}

#if defined(BDES_PLATFORM__CMP_MSVC)
#undef snprintf
#endif

inline
int bdeat_TestValueFunctions::loadTestValue(bdet_Date *object, int index)
{
    BDE_ASSERT_H(index >= 0);

    // Treat 'index' as a number of days.  For simplicity, all months are
    // assumed to have 28 days.

    const int DAYS_IN_MONTH = 28;  // avoid February 29, April 31, etc.

    const int day   = index % DAYS_IN_MONTH + 1;  index /= DAYS_IN_MONTH;
    const int month = index % 12 + 1;             index /= 12;
    const int year  = index + 1753;  // avoid Sept. 1752

    if (year > 9999) {
        return FAILURE;                                               // RETURN
    }

    BDE_ASSERT_H(bdet_Date::isValid(year, month, day));
    object->setYearMonthDay(year, month, day);
    return SUCCESS;
}

inline
int bdeat_TestValueFunctions::loadTestValue(bdet_Datetime *object, int index)
{
    BDE_ASSERT_H(index >= 0);

    bdet_Date date;
    if (loadTestValue(&date, index)) {
        return FAILURE;                                               // RETURN
    }
    object->setDate(date);
    return SUCCESS;
}

inline
int bdeat_TestValueFunctions::loadTestValue(bdet_Time *object, int index)
{
    BDE_ASSERT_H(index >= 0);

    // Treat 'index' as a number of milliseconds.

    const int milliseconds = index % 1000;  index /= 1000;
    const int seconds      = index %   60;  index /=   60;
    const int minutes      = index %   60;  index /=   60;
    const int hours        = index;

    if (hours > 23) {
        return FAILURE;                                               // RETURN
    }

    BDE_ASSERT_H(bdet_Time::isValid(hours, minutes, seconds, milliseconds));
    object->setTime(hours, minutes, seconds, milliseconds);
    return SUCCESS;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
