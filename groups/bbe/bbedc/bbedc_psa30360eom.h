// bbedc_psa30360eom.h          -*-C++-*-
#ifndef INCLUDED_BBEDC_PSA30360EOM
#define INCLUDED_BBEDC_PSA30360EOM

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT_RCSID(bbedc_psa30360eom_h,"$Id$ $CSID$")
BDES_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide support for PSA 30/360 end-of-month day-count convention.
//
//@CLASSES:
//   bbedc_Psa30360eom: procedures for the PSA 30/360 eom day-count convention
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component provides a namespace for pure procedures that
// manipulate dates as prescribed by the Public Securities Association (PSA)
// 30/360 day-count convention with end of month (eom) adjustments.
//..
//                      PSA 30/360-eom Day-Count Algorithm
//                      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Given 'beginDate' and 'endDate' let:
//
// Ye =  year of earlier date           Yl =  year of later date
// Me = month of earlier date           Ml = month of later date
// De =   day of earlier date           Dl =   day of later date
//
//   o If De is the last day of February (29 in a leap year, else 28),
//     change De to 30.
//
//   o If De is 31, change De to 30.
//
//   o If at this point De is 30 and Dl is 31, change Dl to 30.
//
// daysDiff ::= sign(endDate - beginDate) *
//                           max((Yl - Ye) * 360 + (Ml - Me) * 30 + Dl - De, 0)
//..
// The max() function is required because Dl has no February adjustment
// and simple diffs like (19990228 - 19990228) produce -2 without max().
//
// Ref: PSA Standard Formulas, page SF-17
//
///Usage
///-----
// The following snippets of code illustrate how to use 'bbedc_Psa30360eom'
// methods.  First, create two 'bdet_Dates' 'd1' and 'd2':
//..
//  const bdet_Date d1(2004,  9, 30);
//  const bdet_Date d2(2004, 12, 31);
//..
// To compute the day-count between these two dates:
//..
//  const int daysDiff = bbedc_Psa30360eom::daysDiff(d1, d2);
//  assert(90 == daysDiff);
//..
// To compute the year fraction between these two dates:
//..
//  const double yearsDiff = bbedc_Psa30360eom::yearsDiff(d1, d2);
//  assert(0.25 == yearsDiff);
//..

#ifndef INCLUDED_BBESCM_VERSION
#include <bbescm_version.h>
#endif

namespace BloombergLP {

class bdet_Date;

                        // ========================
                        // struct bbedc_Psa30360eom
                        // ========================

struct bbedc_Psa30360eom {
    // This 'struct' provides a namespace for pure procedures determining
    // values based on dates according to the PSA 30/360 end-of-month day-count
    // convention.

    // CLASS METHODS
    static int daysDiff(const bdet_Date& beginDate, const bdet_Date& endDate);
        // Return the number of days between the specified 'beginDate' and
        // 'endDate' according to the PSA 30/360 end-of-month day-count
        // convention.  If 'beginDate <= endDate', then the result is
        // non-negative.  Note that reversing the order of 'beginDate' and
        // 'endDate' negates the result.

    static double yearsDiff(const bdet_Date& beginDate,
                            const bdet_Date& endDate);
        // Return the number of years between the specified 'beginDate' and
        // 'endDate' according to the PSA 30/360 end-of-month day-count
        // convention.  If 'beginDate <= endDate', then the result is
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
