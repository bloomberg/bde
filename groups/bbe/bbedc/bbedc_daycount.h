// bbedc_daycount.h                                                   -*-C++-*-
#ifndef INCLUDED_BBEDC_DAYCOUNT
#define INCLUDED_BBEDC_DAYCOUNT

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT_RCSID(bbedc_daycount_h,"$Id$ $CSID$")
BDES_IDENT_PRAGMA_ONCE

//@PURPOSE: Support for day-count calculations of 'enum'-specified conventions.
//
//@CLASSES:
//   bbedc_DayCount: procedures for 'enum'-specified day-count calculations
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component provides a namespace for pure procedures that
// manipulate dates as prescribed by the enumerated day-count convention.
//
///Usage
///-----
// The following snippets of code illustrate how to use 'bbedc_DayCount'
// methods.  First, create two 'bdet_Dates' 'd1' and 'd2':
//..
//  const bdet_Date d1(2003, 10, 19);
//  const bdet_Date d2(2003, 12, 31);
//..
// To compute the day-count between these two dates according to the ISDA
// Actual/Actual convention:
//..
//  const int daysDiff = bbedc_DayCount::daysDiff(
//                         d1, 
//                         d2, 
//                         bbedc_DayCountConvention::BBEDC_ISDA_ACTUAL_ACTUAL);
//  assert(73 == daysDiff);
//..
// To compute the year fraction between these two dates according to the ISDA
// Actual/Actual convention:
//..
//  const double yearsDiff = bbedc_DayCount::yearsDiff(
//                         d1, 
//                         d2, 
//                         bbedc_DayCountConvention::BBEDC_ISDA_ACTUAL_ACTUAL);
//  // Need fuzzy comparison since 'yearsDiff' is a double.  Expect
//  // '0.2 == yearsDiff'.
//  assert(0.1999 < yearsDiff);
//  assert(0.2001 > yearsDiff);
//..

#ifndef INCLUDED_BBESCM_VERSION
#include <bbescm_version.h>
#endif

#ifndef INCLUDED_BBEDC_DAYCOUNTCONVENTION
#include <bbedc_daycountconvention.h>
#endif

namespace BloombergLP {

class bdet_Date;

                        // =====================
                        // struct bbedc_DayCount
                        // =====================

struct bbedc_DayCount {
    // This 'struct' provides a namespace for pure procedures determining
    // values based on dates according to enumerated day-count conventions.

    // CLASS METHODS
    static int daysDiff(const bdet_Date&               beginDate,
                        const bdet_Date&               endDate,
                        bbedc_DayCountConvention::Type type);
        // Return the number of days between the specified 'beginDate' and
        // 'endDate' according to the specified enumerated day-count convention
        // 'type'.  If 'beginDate <= endDate' then the result is non-negative.
        // Note that reversing the order of 'beginDate' and 'endDate' negates
        // the result.

    static double yearsDiff(const bdet_Date&               beginDate,
                            const bdet_Date&               endDate,
                            bbedc_DayCountConvention::Type type);
        // Return the number of years between the specified 'beginDate' and
        // 'endDate' according to the specified enumerated day-count convention
        // 'type'.  If 'beginDate <= endDate' then the result is non-negative.
        // The behavior is undefined if either 'beginDate' or 'endDate' is in
        // the year 1752.  Note that reversing the order of 'beginDate' and
        // 'endDate' negates the result.
};

// ============================================================================
//                          INLINE FUNCTION DEFINITIONS
// ============================================================================

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
