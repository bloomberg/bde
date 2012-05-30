// bbedc_actual360.h            -*-C++-*-
#ifndef INCLUDED_BBEDC_ACTUAL360
#define INCLUDED_BBEDC_ACTUAL360

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT_RCSID(bbedc_actual360_h,"$Id$ $CSID$")
BDES_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide support for the Actual/360 day-count convention.
//
//@CLASSES:
//   bbedc_Actual360: procedures relating to Actual/360 day-count convention
//
//@AUTHOR: Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides a namespace for pure procedures that
// manipulate dates as prescribed by the Actual/360 day-count convention.  In
// this convention, we simply measure the Julian days that have occurred in
// a time period, and to calculate years, divide that by 360.  Note that this
// means the number of years between January 1, 2005 and January 1, 2006 comes
// out to about 1.0139.  No end-of-month rule adjustments are made.  Given
// 'beginDate' and 'endDate':
//..
//  yearsDiff ::= sign(endDate - beginDate) *
//                         (Julian days between beginDate and endDate) / 360.0
//..
///Usage
///-----
// The following snippets of code illustrate how to use 'bbedc_Actual360'
// methods.  First, create two 'bdet_Dates' 'd1' and 'd2':
//..
//  const bdet_Date dA(2004,  2,  1);
//  const bdet_Date dB(2004,  3,  1);
//  const bdet_Date dC(2004,  5,  1);
//  const bdet_Date dD(2005,  2,  1);
//..
// To compute the day-count between these two dates:
//..
//  int daysDiff;
//  daysDiff = bbedc_Actual360::daysDiff(dA, dB);
//  assert( 29 == daysDiff);
//  daysDiff = bbedc_Actual360::daysDiff(dA, dC);
//  assert( 90 == daysDiff);
//  daysDiff = bbedc_Actual360::daysDiff(dA, dD);
//  assert(366 == daysDiff);
//  daysDiff = bbedc_Actual360::daysDiff(dB, dC);
//  assert( 61 == daysDiff);
//..
// To compute the year fraction between these two dates:
//..
//  double yearsDiff;
//  yearsDiff = bbedc_Actual360::yearsDiff(dA, dC);
//  assert(0.25 == yearsDiff);
//  yearsDiff = bbedc_Actual360::yearsDiff(dA, dD);
//  assert(yearsDiff < 1.0167 && yearsDiff > 1.0166);
//..

#ifndef INCLUDED_BBESCM_VERSION
#include <bbescm_version.h>
#endif

#ifndef INCLUDED_BDET_DATE
#include <bdet_date.h>
#endif

namespace BloombergLP {

class bdet_Date;

                        // ======================
                        // struct bbedc_Actual360
                        // ======================

struct bbedc_Actual360 {
    // This 'struct' provides a namespace for pure procedures determining
    // values based on dates according to the Actual/360 day-count convention.

    // CLASS METHODS
    static int daysDiff(const bdet_Date& beginDate, const bdet_Date& endDate);
        // Return the number of days between the specified 'beginDate' and
        // 'endDate' according to the Actual/360 day-count convention.  If
        // 'beginDate <= endDate' then the result is non-negative.  Note that
        // reversing the order of 'beginDate' and 'endDate' negates the result.

    static double yearsDiff(const bdet_Date& beginDate,
                            const bdet_Date& endDate);
        // Return the number of years between the specified 'beginDate' and
        // 'endDate' according to the Actual/360 day-count convention.  If
        // 'beginDate <= endDate' then the result is non-negative.  Note that
        // reversing the order of 'beginDate' and 'endDate' negates the result.
};

// ============================================================================
//                          INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ----------------------
                        // struct bbedc_Actual360
                        // ----------------------

// CLASS METHODS
inline
int bbedc_Actual360::daysDiff(const bdet_Date& beginDate,
                              const bdet_Date& endDate)
{
    return endDate - beginDate;
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
