// bdetu_time.h                                                       -*-C++-*-
#ifndef INCLUDED_BDETU_TIME
#define INCLUDED_BDETU_TIME

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide common non-primitive operations on 'bdet_Time'.
//
//@CLASSES:
//   bdetu_Time: namespace for non-primitive pure procedures on 'bdet_Time'
//
//@SEE_ALSO: bdet_time, bdetu_systemtime
//
//@AUTHOR: Jeffrey Mendelsohn (jmendelsohn), Arthur Chiu (achiu21)
//
//@DESCRIPTION: This component provides non-primitive operations on 'bdet_Time'
// objects.  In particular, the 'bdetu_Time' namespace defined in this
// component provides conversions among 'bdet_Time' values and their
// corresponding non-negative integral values (e.g., 'convertFromHHMM',
// 'convertToHHMMSSmmm'), and methods to validate such integral values (e.g.,
// 'isValidHHMMSS') before passing them to the corresponding "convertFrom"
// method.  'bdetu_Time' also provides a 'hash' function for generating a hash
// value from a given 'bdet_Time' value.
//
///Usage
///------
// Following are examples illustrating basic use of this component.
//
///Example 1
///- - - - -
// The following snippet of code demonstrates how to use 'bdetu_Time' to
// convert from an integer representation of time in "HHMMSSmmm" format to a
// 'bdet_Time':
//..
//  //      format: HHMMSSmmm
//  int timeValue =  34502789;
//
//  bdet_Time result = bdetu_Time::convertFromHHMMSSmmm(timeValue);
//
//  bsl::cout << result << bsl::endl;
//..
// The code above produces the following on 'stdout':
//..
//  03:45:02.789
//..
// Note that 'bdetu_Time' provides methods that can be used to validate
// integral time values before passing them to the various "convert" methods.
// For example:
//..
//  assert( bdetu_Time::isValidHHMMSSmmm(timeValue));
//
//  //         format: HHMMSSmmm
//  int badTimeValue =  36102789;
//
//  assert(!bdetu_Time::isValidHHMMSSmmm(badTimeValue));
//..
//
///Example 2
///- - - - -
// The following snippet of code demonstrates how to use 'bdetu_Time' to
// convert from a 'bdet_Time' to an integer representation of time in "HHMM",
// "HHMMSS", and "HHMMSSmmm" formats:
//..
//  bdet_Time time(3, 45, 2, 789);
//  int       timeHHMM      = bdetu_Time::convertToHHMM(time);
//  int       timeHHMMSS    = bdetu_Time::convertToHHMMSS(time);
//  int       timeHHMMSSmmm = bdetu_Time::convertToHHMMSSmmm(time);
//
//  bsl::cout << "Time in HHMM:      " << timeHHMM      << bsl::endl;
//  bsl::cout << "Time in HHMMSS:    " << timeHHMMSS    << bsl::endl;
//  bsl::cout << "Time in HHMMSSmmm: " << timeHHMMSSmmm << bsl::endl;
//..
// The code above produces the following on 'stdout':
//..
//  Time in HHMM:      345
//  Time in HHMMSS:    34502
//  Time in HHMMSSmmm: 34502789
//..
// Note that the millisecond and/or second fields of 'bdet_Time' are ignored
// depending on the conversion method that is called.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDET_TIME
#include <bdet_time.h>
#endif

#ifndef INCLUDED_BDEU_HASHUTIL
#include <bdeu_hashutil.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {

                             // =================
                             // struct bdetu_Time
                             // =================

struct bdetu_Time {
    // This 'struct' provides a namespace for common non-primitive procedures
    // that operate on 'bdet_Time' objects.  These methods are alias-safe and
    // exception-neutral.  Due to its "read-only" nature, the following method
    // is also thread-safe:
    //..
    //  int hash(const bdet_Time& value, int modulus);
    //..

  private:
    // PRIVATE TYPES
    enum {
        BDETU_HHMMSSMMM_HH_FACTOR = 10000000,
        BDETU_HHMMSSMMM_MM_FACTOR =   100000,
        BDETU_HHMMSSMMM_SS_FACTOR =     1000,

        BDETU_HHMMSS_HH_FACTOR    =    10000,
        BDETU_HHMMSS_MM_FACTOR    =      100,

        BDETU_HHMM_HH_FACTOR      =      100
    };

  public:
    // CLASS METHODS
    static bdet_Time convertFromHHMM(int timeValue);
        // Return the 'bdet_Time' value corresponding to the specified
        // 'timeValue', where 'timeValue' is a non-negative integer that, when
        // expressed in decimal notation, contains exactly four digits
        // (counting leading zeros, if any): two digits for the hour and two
        // digits for the minute.  For example, 309 is converted to
        // 'bdet_Time(3, 9)' (03:09:00.000).  More formally, 'timeValue' is
        // interpreted as:
        //..
        //  hour * 100 + minute
        //..
        // The behavior is undefined unless 'timeValue' represents a valid time
        // in the allowable range for 'bdet_Time' (00:00:00.000 - 23:59:00.000,
        // and 24:00:00.000).

    static bdet_Time convertFromHHMMSS(int timeValue);
        // Return the 'bdet_Time' value corresponding to the specified
        // 'timeValue', where 'timeValue' is a non-negative integer that, when
        // expressed in decimal notation, contains exactly six digits (counting
        // leading zeros, if any): two digits for the hour, two digits for the
        // minute, and two digits for the second.  For example, 30907 is
        // converted to 'bdet_Time(3, 9, 7)' (03:09:07.000).  More formally,
        // 'timeValue' is interpreted as:
        //..
        //  hour * 10000 + minute * 100 + second
        //..
        // The behavior is undefined unless 'timeValue' represents a valid time
        // in the allowable range for 'bdet_Time' (00:00:00.000 - 23:59:59.000,
        // and 24:00:00.000).

    static bdet_Time convertFromHHMMSSmmm(int timeValue);
        // Return the 'bdet_Time' value corresponding to the specified
        // 'timeValue', where 'timeValue' is a non-negative integer that, when
        // expressed in decimal notation, contains exactly nine digits
        // (counting leading zeros, if any): two digits for the hour, two
        // digits for the minute, two digits for the second, and three digits
        // for the millisecond.  For example, 30907056 is converted to
        // 'bdet_Time(3, 9, 7, 56)' (03:09:07.056).  More formally, 'timeValue'
        // is interpreted as:
        //..
        //  hour * 10000000 + minute * 100000 + second * 1000 + millisecond
        //..
        // The behavior is undefined unless 'timeValue' represents a valid time
        // in the allowable range for 'bdet_Time' (00:00:00.000 - 23:59:59.999,
        // and 24:00:00.000).

    static int convertToHHMM(const bdet_Time& value);
        // Return the non-negative integer representing the same time as the
        // specified 'value' that, when expressed in decimal notation, contains
        // exactly four digits (counting leading zeros, if any): two digits
        // for the hour and two digits for the minute.  For example,
        // 'bdet_Time(3, 9, sec, ms)', where '0 <= sec < 60' and
        // '0 <= ms < 1000', is converted to 309.  More formally, this method
        // returns:
        //..
        //  value.hour() * 100 + value.minute()
        //..

    static int convertToHHMMSS(const bdet_Time& value);
        // Return the non-negative integer representing the same time as the
        // specified 'value' that, when expressed in decimal notation, contains
        // exactly six digits (counting leading zeros, if any): two digits for
        // the hour, two digits for the minute, and two digits for the second.
        // For example, 'bdet_Time(3, 9, 7, ms)', where '0 <= ms < 1000',
        // is converted to 30907.  More formally, this method returns:
        //..
        //  value.hour() * 10000 + value.minute() * 100 + value.second()
        //..

    static int convertToHHMMSSmmm(const bdet_Time& value);
        // Return the non-negative integer representing the same time as the
        // specified 'value' that, when expressed in decimal notation, contains
        // exactly nine digits (counting leading zeros, if any): two digits
        // for the hour, two digits for the minute, two digits for the second,
        // and three digits for the millisecond.  For example,
        // 'bdet_Time(3, 9, 7, 56)' is converted to 30907056.  More
        // formally, this method returns:
        //..
        //  value.hour() * 10000000 + value.minute() * 100000
        //                          + value.second() * 1000
        //                          + value.millisecond()
        //..

    static int hash(const bdet_Time& value, int modulus);
        // Return an integer, in the range from 0 to one less than the
        // specified 'modulus', corresponding to the specified 'value'.  The
        // behavior is undefined unless '0 < modulus'.  Note that 'modulus' is
        // expected to be a prime that is not close to an integral power of 2.
        // Also note that specifying a 'modulus' of 1 will cause 0 to be
        // returned for every 'value'.

    static bool isValidHHMM(int timeValue);
        // Return 'true' if the specified 'timeValue' is a non-negative integer
        // that represents a valid four-digit time value suitable for passing
        // to 'convertFromHHMM', and 'false' otherwise.  'timeValue' is a valid
        // four-digit time value if, when expressed in decimal notation, it
        // contains exactly four digits (counting leading zeros, if any): two
        // digits for the hour and two digits for the minute, where either
        // '0 <= hour < 24' and '0 <= minute < 60', or '2400 == timeValue'.

    static bool isValidHHMMSS(int timeValue);
        // Return 'true' if the specified 'timeValue' is a non-negative integer
        // that represents a valid six-digit time value suitable for passing to
        // 'convertFromHHMMSS', and 'false' otherwise.  'timeValue' is a valid
        // six-digit time value if, when expressed in decimal notation, it
        // contains exactly six digits (counting leading zeros, if any): two
        // digits for the hour, two digits for the minute, and two digits for
        // the second, where either '0 <= hour < 24', '0 <= minute < 60', and
        // '0 <= second < 60', or '240000 == timeValue'.

    static bool isValidHHMMSSmmm(int timeValue);
        // Return 'true' if the specified 'timeValue' is a non-negative integer
        // that represents a valid nine-digit time value suitable for passing
        // to 'convertFromHHMMSSmmm', and 'false' otherwise.  'timeValue' is a
        // valid nine-digit time value if, when expressed in decimal notation,
        // it contains exactly nine digits (counting leading zeros, if any):
        // two digits for the hour, two digits for the minute, two digits for
        // the second, and three digits for the millisecond, where either
        // '0 <= hour < 24', '0 <= minute < 60', '0 <= second < 60', and
        // '0 <= millisecond < 1000', or '240000000 == timeValue'.

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    static bdet_Time currentTime();
        // Return the current time in the UTC timezone.
        //
        // DEPRECATED: See 'bdetu_systemtime'.

    static void loadCurrentTime(bdet_Time *result);
        // Load into the specified 'result' the current time in the UTC
        // timezone.  The behavior is undefined unless 'result' is non-zero.
        //
        // DEPRECATED: See 'bdetu_systemtime'.
#endif
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                            // -----------------
                            // struct bdetu_Time
                            // -----------------

                            // -----------------
                            // Level-0 Functions
                            // -----------------

// CLASS METHODS
inline
bool bdetu_Time::isValidHHMM(int timeValue)
{
    return bdet_Time::isValid(timeValue / BDETU_HHMM_HH_FACTOR,
                              timeValue % BDETU_HHMM_HH_FACTOR);
}

inline
bool bdetu_Time::isValidHHMMSS(int timeValue)
{
    return bdet_Time::isValid(timeValue / BDETU_HHMMSS_HH_FACTOR,
                             (timeValue % BDETU_HHMMSS_HH_FACTOR)
                                                      / BDETU_HHMMSS_MM_FACTOR,
                              timeValue % BDETU_HHMMSS_MM_FACTOR);
}

inline
bool bdetu_Time::isValidHHMMSSmmm(int timeValue)
{
    return bdet_Time::isValid(timeValue / BDETU_HHMMSSMMM_HH_FACTOR,
                             (timeValue % BDETU_HHMMSSMMM_HH_FACTOR)
                                                   / BDETU_HHMMSSMMM_MM_FACTOR,
                             (timeValue % BDETU_HHMMSSMMM_MM_FACTOR)
                                                   / BDETU_HHMMSSMMM_SS_FACTOR,
                              timeValue % BDETU_HHMMSSMMM_SS_FACTOR);
}

                            // -------------------
                            // All Other Functions
                            // -------------------

inline
bdet_Time bdetu_Time::convertFromHHMM(int timeValue)
{
    BSLS_ASSERT_SAFE(bdetu_Time::isValidHHMM(timeValue));

    return bdet_Time(timeValue / BDETU_HHMM_HH_FACTOR,
                     timeValue % BDETU_HHMM_HH_FACTOR);
}

inline
bdet_Time bdetu_Time::convertFromHHMMSS(int timeValue)
{
    BSLS_ASSERT_SAFE(bdetu_Time::isValidHHMMSS(timeValue));

    return bdet_Time(timeValue / BDETU_HHMMSS_HH_FACTOR,
                    (timeValue % BDETU_HHMMSS_HH_FACTOR)
                                                      / BDETU_HHMMSS_MM_FACTOR,
                     timeValue % BDETU_HHMMSS_MM_FACTOR);
}

inline
bdet_Time bdetu_Time::convertFromHHMMSSmmm(int timeValue)
{
    BSLS_ASSERT_SAFE(bdetu_Time::isValidHHMMSSmmm(timeValue));

    return bdet_Time(timeValue / BDETU_HHMMSSMMM_HH_FACTOR,
                    (timeValue % BDETU_HHMMSSMMM_HH_FACTOR)
                                                   / BDETU_HHMMSSMMM_MM_FACTOR,
                    (timeValue % BDETU_HHMMSSMMM_MM_FACTOR)
                                                   / BDETU_HHMMSSMMM_SS_FACTOR,
                     timeValue % BDETU_HHMMSSMMM_SS_FACTOR);
}

inline
int bdetu_Time::convertToHHMM(const bdet_Time& value)
{
    return value.hour() * BDETU_HHMM_HH_FACTOR + value.minute();
}

inline
int bdetu_Time::convertToHHMMSS(const bdet_Time& value)
{
    return value.hour()   * BDETU_HHMMSS_HH_FACTOR
         + value.minute() * BDETU_HHMMSS_MM_FACTOR
         + value.second();
}

inline
int bdetu_Time::convertToHHMMSSmmm(const bdet_Time& value)
{
    return value.hour()   * BDETU_HHMMSSMMM_HH_FACTOR
         + value.minute() * BDETU_HHMMSSMMM_MM_FACTOR
         + value.second() * BDETU_HHMMSSMMM_SS_FACTOR
         + value.millisecond();
}

inline
int bdetu_Time::hash(const bdet_Time& value, int modulus)
{
    BSLS_ASSERT_SAFE(0 < modulus);

    return bdeu_HashUtil::hash0(
                              (value - bdet_Time(0, 0, 0)).totalMilliseconds(),
                              modulus);
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
