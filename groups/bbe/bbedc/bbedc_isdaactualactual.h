// bbedc_isdaactualactual.h         -*-C++-*-
#ifndef INCLUDED_BBEDC_ISDAACTUALACTUAL
#define INCLUDED_BBEDC_ISDAACTUALACTUAL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT_RCSID(bbedc_isdaactualactual_h,"$Id$ $CSID$")
BDES_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide support for the ISDA Actual/Actual day-count convention.
//
//@CLASSES:
//   bbedc_IsdaActualActual: procedures relating to a day-count convention
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component provides a namespace for pure procedures that
// manipulate dates as prescribed by the International Swaps and Derivatives
// Association (ISDA) Actual/Actual day-count convention.  In the ISDA
// Actual/Actual convention the day count between two dates is exactly the
// number of days separating the dates as per a conventional calendar.
//
///Usage
///-----
// The following snippets of code illustrate how to use
// 'bbedc_IsdaActualActual' methods.  First, create two 'bdet_Dates' 'd1' and
// 'd2':
//..
//  const bdet_Date d1(2003, 10, 19);
//  const bdet_Date d2(2003, 12, 31);
//..
// To compute the day-count between these two dates:
//..
//  const int daysDiff = bbedc_IsdaActualActual::daysDiff(d1, d2);
//  assert(73 == daysDiff);
//..
// To compute the year fraction between these two dates:
//..
//  const double yearsDiff = bbedc_IsdaActualActual::yearsDiff(d1, d2);
//  // Need fuzzy comparison since 'yearsDiff' is a double.  Expect
//  // '0.2 == yearsDiff'.
//  assert(0.1999 < yearsDiff);
//  assert(0.2001 > yearsDiff);
//..

#ifndef INCLUDED_BBESCM_VERSION
#include <bbescm_version.h>
#endif

#ifndef INCLUDED_BDET_DATE
#include <bdet_date.h>
#endif

namespace BloombergLP {

                       // =============================
                       // struct bbedc_IsdaActualActual
                       // =============================

struct bbedc_IsdaActualActual {
    // This 'struct' provides a namespace for pure procedures determining
    // values based on dates according to the ISDA Actual/Actual day-count
    // convention.

    // CLASS METHODS
    static int daysDiff(const bdet_Date& beginDate, const bdet_Date& endDate);
        // Return the number of days between the specified 'beginDate' and
        // 'endDate according to the ISDA Actual/Actual day-count convention.
        // If 'beginDate <= endDate', then the result is non-negative.  Note
        // that reversing the order of 'beginDate' and 'endDate' negates the
        // result.

    static double yearsDiff(const bdet_Date& beginDate,
                            const bdet_Date& endDate);
        // Return the number of years between the specified 'beginDate' and
        // 'endDate according to the ISDA Actual/Actual day-count convention.
        // If 'beginDate <= endDate', then the result is non-negative.  The
        // behavior is undefined if either 'beginDate' or 'endDate' is in the
        // year 1752.  Note that reversing the order of 'beginDate' and
        // 'endDate' negates the result.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                       // -----------------------------
                       // struct bbedc_IsdaActualActual
                       // -----------------------------

// CLASS METHODS
inline
int bbedc_IsdaActualActual::daysDiff(const bdet_Date& beginDate,
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
