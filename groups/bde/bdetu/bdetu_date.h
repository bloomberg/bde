// bdetu_date.h                                                       -*-C++-*-
#ifndef INCLUDED_BDETU_DATE
#define INCLUDED_BDETU_DATE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide common non-primitive operations on 'bdet_Date'.
//
//@CLASSES:
//   bdetu_Date: non-primitive pure procedures on 'bdet_Date'
//
//@AUTHOR: Jeffrey Mendelsohn (jmendelsohn)
//
//@DESCRIPTION: This component provides non-primitive operations on 'bdet_Date'
// objects.
//
///Usage
///------
// Following are examples illustrating basic use of this component.
//
///Example 1
///- - - - -
// The following snippet of code demonstrates how to use 'bdetu_Date' to
// convert from an integer representation of a date in "YYYYMMDD" format to a
// 'bdet_Date':
//..
//  const int date   = 20091106;
//  bdet_Date result = bdetu_Date::convertFromYYYYMMDDRaw(date);
//
//  bsl::cout << result << bsl::endl;
//..
// The code above produces the following on 'stdout':
//..
//  06NOV2009
//..
// Note that 'bdetu_Date::convertFromYYYYMMDDRaw' performs *no* validation of
// the supplied 'date' value whatsoever.  It is the client's responsibility to
// provide a valid integral 'date' value in the allowable range for 'bdet_Date'
// (1/1/1 - 12/31/9999).  However, the 'bdetu_Date::convertFromYYYYMMDD' method
// is available to validate suspect integral dates as follows:
//..
//  const int badDate = 20091131;
//  if (bdetu_Date::convertFromYYYYMMDD(&result, badDate)) {
//      bsl::cout << "Bad date: "           << badDate << bsl::endl;
//      bsl::cout << "'result' unchanged: " << result  << bsl::endl;
//  }
//..
// The code above produces the following on 'stdout':
//..
//  Bad date: 20091131
//  'result' unchanged: 06NOV2009
//..
///Example 2
///- - - - -
// The following snippet of code demonstrates how to use 'bdetu_Date' to
// convert from a 'bdet_Date' to an integer representation of time in
// "YYYYMMDD" format:
//..
//  bdet_Date date(2009, 11, 06);
//  int dateYYYYMMDD = bdetu_Date::convertToYYYYMMDD(date);
//
//  bsl::cout << "Date in YYYYMMDD: " << dateYYYYMMDD << bsl::endl;
//..
// The code above produces the following on 'stdout':
//..
//  Date in YYYYMMDD: 20091106
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEIMP_DATEUTIL
#include <bdeimp_dateutil.h>
#endif

#ifndef INCLUDED_BDET_DATE
#include <bdet_date.h>
#endif

#ifndef INCLUDED_BDEU_HASHUTIL
#include <bdeu_hashutil.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {

                             // =================
                             // struct bdetu_Date
                             // =================

struct bdetu_Date {
    // This 'struct' provides a namespace for common non-primitive procedures
    // that operate on 'bdet_Date' objects.  These methods are alias-safe and
    // exception-neutral.  Due to its "read-only" nature, the following method
    // is also thread-safe:
    //..
    //  int hash(const bdet_Date& value, int modulus);
    //..

  public:
    // CLASS METHODS
    static bool isValidYYYYMMDD(int dateValue);
        // Return 'true' if the specified 'dateValue' represents a valid date
        // in the allowable range for 'bdet_Date' (1/1/1 - 12/31/9999), and
        // 'false' otherwise.

    static bdet_Date convertFromYYYYMMDDRaw(int dateValue);
        // Return the 'bdet_Date' representation of the specified 'dateValue',
        // where 'dateValue' is a positive integer that, when expressed in
        // decimal notation, contains exactly eight digits (counting leading
        // zeros, if any): four digits for the year, two digits for the month,
        // and two digits for the day of the month, respectively.  For example,
        // 20061018 would be converted to 'bdet_Date(2006, 10, 18)'
        // (October 18, 2006).  More formally, 'dateValue' is interpreted as:
        //..
        //  year * 10000 + month * 100 + day
        //..
        // The behavior is undefined unless 'dateValue' represents a valid
        // date in the allowable range for 'bdet_Date' (1/1/1 - 12/31/9999).

    static int convertFromYYYYMMDD(bdet_Date *result, int dateValue);
        // Load into the specified 'result' the 'bdet_Date' representation of
        // the specified 'dateValue', where 'dateValue' is a positive integer
        // that, when expressed in decimal notation, contains exactly eight
        // digits (counting leading zeros, if any): four digits for the year,
        // two digits for the month, and two digits for the day of the month,
        // respectively.  For example, 20061018 would be converted to
        // 'bdet_Date(2006, 10, 18)' (October 18, 2006).  More formally,
        // 'dateValue' is interpreted as:
        //..
        //  year * 10000 + month * 100 + day
        //..
        // Return 0 on success, and a non-zero value (with no effect on
        // 'result') if 'dateValue' does not represent a valid date in the
        // allowable range for 'bdet_Date' (1/1/1 - 12/31/9999).

    static int convertToYYYYMMDD(const bdet_Date& value);
        // Return the specified 'value' as a positive integer representation
        // of the same date that, when expressed in decimal notation, contains
        // exactly eight digits (counting leading zeros, if any): four digits
        // for the year, two digits for the month and two digits for the day of
        // the month, respectively.  For example, 'bdet_Date(2006, 10, 18)'
        // (October 18, 2006) would be converted to 20061018.  More formally,
        // this method returns:
        //..
        //  value.year() * 10000 + value.month() * 100 + value.day()
        //..

    static int hash(const bdet_Date& value, int modulus);
        // Return an integer in the range from 0 to one less than the specified
        // 'modulus' corresponding to the specified 'value'.  The behavior is
        // undefined unless '0 < modulus'.  Note that 'modulus' is expected to
        // be a prime not close to an integral power of 2.  Also note that
        // specifying a 'modulus' of 1 will cause 0 to be returned for every
        // 'value'.

    static bdet_Date convertFromYYYYMMDD(int dateValue);
        // Return the 'bdet_Date' representation of the specified 'dateValue',
        // where 'dateValue' is a positive integer that, when expressed in
        // decimal notation, contains exactly eight digits (counting leading
        // zeros, if any): four digits for the year, two digits for the month,
        // and two digits for the day of the month, respectively.  For example,
        // 20061018 would be converted to 'bdet_Date(2006, 10, 18)'
        // (October 18, 2006).  More formally, 'dateValue' is interpreted as:
        //..
        //  year * 10000 + month * 100 + day
        //..
        // The behavior is undefined unless 'dateValue' represents a valid
        // date in the allowable range for 'bdet_Date' (1/1/1 - 12/31/9999).
        //
        // DEPRECATED: use 'convertFromYYYYMMDDRaw(int dateValue)' instead.

    static bdet_Date fromYyyyMmDd(int dateValue);
        // Return the 'bdet_Date' representation of the specified 'dateValue',
        // where 'dateValue' is a positive integer that, when expressed in
        // decimal notation, contains exactly eight digits (counting leading
        // zeros, if any): four digits for the year, two digits for the month,
        // and two digits for the day of the month, respectively.  For example,
        // 20061018 would be converted to 'bdet_Date(2006, 10, 18)'
        // (October 18, 2006).  More formally, 'dateValue' is interpreted as:
        //..
        //  year * 10000 + month * 100 + day
        //..
        // The behavior is undefined unless 'dateValue' represents a valid
        // date in the allowable range for 'bdet_Date' (1/1/1 - 12/31/9999).
        //
        // DEPRECATED: use 'convertFromYYYYMMDDRaw(int dateValue)' instead.

    static int toYyyyMmDd(const bdet_Date& value);
        // Return the specified 'value' as a positive integer representation
        // of the same date that, when expressed in decimal notation, contains
        // exactly eight digits (counting leading zeros, if any): four digits
        // for the year, two digits for the month and two digits for the day of
        // the month, respectively.  For example, 'bdet_Date(2006, 10, 18)'
        // (October 18, 2006) would be converted to 20061018.  More formally,
        // this method returns:
        //..
        //  value.year() * 10000 + value.month() * 100 + value.day()
        //..
        //
        // DEPRECATED: use 'convertToYYYYMMDD(const bdet_Date& value)' instead.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                            // -----------------
                            // struct bdetu_Date
                            // -----------------

// CLASS METHODS
inline
bool bdetu_Date::isValidYYYYMMDD(int dateValue)
{
    const int day   = dateValue % 100;  // DD
    dateValue /= 100;
    const int month = dateValue % 100;  // MM

    return bdeimp_DateUtil::isValidCalendarDate(dateValue / 100, month, day);
}

inline
bdet_Date bdetu_Date::convertFromYYYYMMDDRaw(int dateValue)
{
    BSLS_ASSERT_SAFE(bdetu_Date::isValidYYYYMMDD(dateValue));

    return bdet_Date(dateValue / 10000,
                    (dateValue / 100) % 100,
                     dateValue % 100);
}

inline
int bdetu_Date::convertFromYYYYMMDD(bdet_Date *result, int dateValue)
{
    if (!isValidYYYYMMDD(dateValue)) {
        return -1;
    }
    *result = convertFromYYYYMMDDRaw(dateValue);

    return 0;
}

inline
int bdetu_Date::convertToYYYYMMDD(const bdet_Date& value)
{
    return value.year() * 10000 + value.month() * 100 + value.day();
}

inline
int bdetu_Date::hash(const bdet_Date& value, int modulus)
{
    BSLS_ASSERT_SAFE(0 < modulus);

    return bdeu_HashUtil::hash0(value - bdet_Date(1, 1, 1), modulus);
}

inline
bdet_Date bdetu_Date::convertFromYYYYMMDD(int dateValue)
{
    return convertFromYYYYMMDDRaw(dateValue);
}

inline
bdet_Date bdetu_Date::fromYyyyMmDd(int dateValue)
{
    return convertFromYYYYMMDDRaw(dateValue);
}

inline
int bdetu_Date::toYyyyMmDd(const bdet_Date& value)
{
    return convertToYYYYMMDD(value);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
