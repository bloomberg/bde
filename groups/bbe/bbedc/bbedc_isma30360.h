// bbedc_isma30360.h            -*-C++-*-
#ifndef INCLUDED_BBEDC_ISMA30360
#define INCLUDED_BBEDC_ISMA30360

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT_RCSID(bbedc_isma30360_h,"$Id$ $CSID$")
BDES_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide support for the ISMA 30/360 day-count convention.
//
//@CLASSES:
//   bbedc_Isma30360: procedures relating to ISMA 30/360 day-count convention
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component provides a namespace for pure procedures that
// manipulate dates as prescribed by the International Securities Market
// Association (ISMA) 30/360 day-count convention.  In this convention (also
// known as "European 30/360"), each year is assumed to have 12 months and 360
// days, each month consisting of exactly 30 days.  End-of-month rule
// adjustments are *NOT* made to account for the last day of February.
//..
//                      ISMA 30/360 Day-Count Algorithm
//                      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Given 'beginDate' and 'endDate', let:
//
// Ye =  year of earlier date           Yl =  year of later date
// Me = month of earlier date           Ml = month of later date
// De =   day of earlier date           Dl =   day of later date
//
//   o If De is 31, change De to 30.
//
//   o If Dl is 31, change Dl to 30.
//
// daysDiff ::= sign(endDate - beginDate) *
//                                   (Yl - Ye) * 360 + (Ml - Me) * 30 + Dl - De
//..
// Ref: Formulae For Yield And Other Calculations (1992) ISBN: 0-9515474-0-2.
//
// Note that in this convention, dates (De and Dl) are adjusted independently
// of each other.
//
///Usage
///-----
// The following snippets of code illustrate how to use 'bbedc_Isma30360'
// methods.  First, create two 'bdet_Dates' 'd1' and 'd2':
//..
//  const bdet_Date d1(2004,  9, 30);
//  const bdet_Date d2(2004, 12, 31);
//..
// To compute the day-count between these two dates:
//..
//  const int daysDiff = bbedc_Isma30360::daysDiff(d1, d2);
//  assert(90 == daysDiff);
//..
// To compute the year fraction between these two dates:
//..
//  const double yearsDiff = bbedc_Isma30360::yearsDiff(d1, d2);
//  assert(0.25 == yearsDiff);
//..

#ifndef INCLUDED_BBESCM_VERSION
#include <bbescm_version.h>
#endif

namespace BloombergLP {

class bdet_Date;

                        // ======================
                        // struct bbedc_Isma30360
                        // ======================

struct bbedc_Isma30360 {
    // This 'struct' provides a namespace for pure procedures determining
    // values based on dates according to the ISMA 30/360 day-count convention.

    // CLASS METHODS
    static int daysDiff(const bdet_Date& beginDate, const bdet_Date& endDate);
        // Return the number of days between the specified 'beginDate' and
        // 'endDate' according to the ISMA 30/360 day-count convention.  If
        // 'beginDate <= endDate', then the result is non-negative.  Note that
        // reversing the order of 'beginDate' and 'endDate' negates the result.

    static double yearsDiff(const bdet_Date& beginDate,
                            const bdet_Date& endDate);
        // Return the number of years between the specified 'beginDate' and
        // 'endDate' according to the ISMA 30/360 day-count convention.  If
        // 'beginDate <= endDate', then the result is non-negative.  Note that
        // reversing the order of 'beginDate' and 'endDate' negates the result.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

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
