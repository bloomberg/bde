// bbedc_sia30360eom.h          -*-C++-*-
#ifndef INCLUDED_BBEDC_SIA30360EOM
#define INCLUDED_BBEDC_SIA30360EOM

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT_RCSID(bbedc_sia30360eom_h,"$Id$ $CSID$")
BDES_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide support for SIA 30/360 end-of-month day-count convention.
//
//@CLASSES:
//   bbedc_Sia30360eom: procedures for the SIA 30/360 eom day-count convention
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component provides a namespace for pure procedures that
// manipulate dates as prescribed by the Standard Industry Association (SIA)
// 30/360 day-count convention with end of month (eom) adjustments.  In this
// convention (also known as "US 30/360" or just "30/360"), each year is
// assumed to have 12 months and 360 days, each month consisting of exactly 30
// days.  Special end-of-month rule adjustments *are* made to account for the
// last day of February.  Note that in this convention, the second date (D2)
// may or may not be adjusted depending on the first date (D1).
//..
//                      SIA-30/360-eom Day-Count Algorithm
//                      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Given 'beginDate' and 'endDate' let:
//
// Ye =  year of earlier date           Yl =  year of later date
// Me = month of earlier date           Ml = month of later date
// De =   day of earlier date           Dl =   day of later date
//
//   o If Dl is the last day of February (29 in a leap year, else 28)
//     and De is the last day of February, change Dl to 30.
//
//   o If De is the last day of February, change De to 30.
//
//   o If Dl is 31 and De is 30 or 31, change Dl to 30.
//
//   o If De is 31, change De to 30.
//
// daysDiff ::= sign(endDate - beginDate) *
//                                   (Yl - Ye) * 360 + (Ml - Me) * 30 + Dl - De
//..
// Ref: Standard Securities Calculation Methods (1996) ISBN 1-882936-01-9.
//
///Usage
///-----
// The following snippets of code illustrate how to use 'bbedc_Sia30360eom'
// methods.  First, create two 'bdet_Dates' 'd1' and 'd2':
//..
//  const bdet_Date d1(2004,  9, 30);
//  const bdet_Date d2(2004, 12, 31);
//..
// To compute the day-count between these two dates:
//..
//  const int daysDiff = bbedc_Sia30360eom::daysDiff(d1, d2);
//  assert(90 == daysDiff);
//..
// To compute the year fraction between these two dates:
//..
//  const double yearsDiff = bbedc_Sia30360eom::yearsDiff(d1, d2);
//  assert(0.25 == yearsDiff);
//..

#ifndef INCLUDED_BBESCM_VERSION
#include <bbescm_version.h>
#endif

namespace BloombergLP {

class bdet_Date;

                        // ========================
                        // struct bbedc_Sia30360eom
                        // ========================

struct bbedc_Sia30360eom {
    // This 'struct' provides a namespace for pure procedures determining
    // values based on dates according to the SIA 30/360 end-of-month day-count
    // convention.

    // CLASS METHODS
    static int daysDiff(const bdet_Date& beginDate, const bdet_Date& endDate);
        // Return the number of days between the specified 'beginDate' and
        // 'endDate' according to the SIA 30/360 end-of-month day-count
        // convention.  If 'beginDate < endDate', then the result is
        // non-negative.  Note that reversing the order of 'beginDate' and
        // 'endDate' negates the result.

    static double yearsDiff(const bdet_Date& beginDate,
                            const bdet_Date& endDate);
        // Return the number of years between the specified 'beginDate' and
        // 'endDate' according to the SIA 30/360 end-of-month day-count
        // convention.  If 'beginDate < endDate', then the result is
        // non-negative.  Note that reversing the order of 'beginDate' and
        // 'endDate' negates the result.
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
