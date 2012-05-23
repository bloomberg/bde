// bbedc_sia30360neom.h         -*-C++-*-
#ifndef INCLUDED_BBEDC_SIA30360NEOM
#define INCLUDED_BBEDC_SIA30360NEOM

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT_RCSID(bbedc_sia30360neom_h,"$Id$ $CSID$")
BDES_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide support for the SIA-30/360-neom day-count convention.
//
//@CLASSES:
//   bbedc_Sia30360neom: procedures for SIA-30/360-neom day-count convention
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component provides a namespace for pure procedures that
// manipulate dates as prescribed by the Standard Industry Association (SIA)
// 30/360 no-end-of-month day-count convention.  In this convention (also known
// as "US 30/360 no-end-of-month" or just "30/360 no-end-of-month"), each year
// is assumed to have 12 months and 360 days, each month consisting of exactly
// 30 days.  No end-of-month rule adjustments are applied for the last day of
// any month (including February).
//..
//                      SIA-30/360-neom Day-Count Algorithm
//                      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Given 'beginDate' and 'endDate' let:
//
// Ye =  year of earlier date           Yl =  year of later date
// Me = month of earlier date           Ml = month of later date
// De =   day of earlier date           Dl =   day of later date
//
// daysDiff ::= sign(endDate - beginDate) *
//                                   (Yl - Ye) * 360 + (Ml - Me) * 30 + Dl - De
//..
// Ref: Standard Securities Calculation Methods (1996) ISBN 1-882936-01-9.
//
///Usage
///-----
// The following snippets of code illustrate how to use 'bbedc_Sia30360neom'
// methods.  First, create two 'bdet_Dates' 'd1' and 'd2':
//..
//  const bdet_Date d1(2004,  9, 30);
//  const bdet_Date d2(2004, 12, 30);
//..
// To compute the day-count between these two dates:
//..
//  const int daysDiff = bbedc_Sia30360neom::daysDiff(d1, d2);
//  assert(90 == daysDiff);
//..
// To compute the year fraction between these two dates:
//..
//  const double yearsDiff = bbedc_Sia30360neom::yearsDiff(d1, d2);
//  assert(0.25 == yearsDiff);
//..

#ifndef INCLUDED_BBESCM_VERSION
#include <bbescm_version.h>
#endif

namespace BloombergLP {

class bdet_Date;

                        // =========================
                        // struct bbedc_Sia30360neom
                        // =========================

struct bbedc_Sia30360neom {
    // This 'struct' provides a namespace for pure procedures determining
    // values based on dates according to the SIA-30/360-neom day-count
    // convention.

    // CLASS METHODS
    static int daysDiff(const bdet_Date& beginDate, const bdet_Date& endDate);
        // Return the number of days between the specified 'beginDate' and
        // 'endDate according to the SIA 30/360 no-end-of-month day-count
        // convention.  If 'beginDate <= endDate', then the result is
        // non-negative.  Note that reversing the order of 'beginDate' and
        // 'endDate' negates the result.

    static double yearsDiff(const bdet_Date& beginDate,
                            const bdet_Date& endDate);
        // Return the number of years between the specified 'beginDate' and
        // 'endDate according to the SIA 30/360 no-end-of-month day-count
        // convention.  If 'beginDate <= endDate', then the result is
        // non-negative.  Note that reversing the order of the specified
        // 'beginDate' and 'endDate' negates the result.
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
