// bbedc_daycountadapter.h            -*-C++-*-
#ifndef INCLUDED_BBEDC_DAYCOUNTADAPTER
#define INCLUDED_BBEDC_DAYCOUNTADAPTER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT_RCSID(bbedc_daycountadapter_h,"$Id$ $CSID$")
BDES_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide a parameterized day-count convention implementation.
//
//@CLASSES:
//   bbedc_DayCountAdapter: 'bbedc_DayCountInterface' parameterized realization
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component provides a parameterized (template)
// implementation of the 'bbedc_DayCountInterface' protocol.  The template
// argument can be any type supporting the following two class methods.
//..
//  int daysDiff(const bdet_Date& beginDate,
//               const bdet_Date& endDate) const;
//
//  double yearsDiff(const bdet_Date& beginDate,
//                   const bdet_Date& endDate) const;
//..
// The template class 'bbedc_DayCountAdapter' provides convenient support for
// run-time polymorphic choice of day-count conventions (via conventional use
// of a base-class pointer or reference) without having to implement each
// derived type explicitly.  In this sense, 'bbedc_DayCountAdapter' adapts the
// various concrete day-count convention classes (e.g., 'bbedc_Isma30360') to a
// run-time binding mechanism.
//
///Usage
///-----
// This example shows the procedure for using 'bbedc_DayCountAdapter' to adapt
// the 'bbedc_Isma30360' day-count convention to the 'bbedc_DayCountInterface'
// and then the use of the day-count methods.  We first define an instance of
// the adapted day-count convention and obtain a reference to the
// 'bbedc_DayCountInterface':
//..
//  bbedc_DayCountAdapter<bbedc_Isma30360> myDcc;
//  bbedc_DayCountInterface& dcc = myDcc;
//..
// We now want to use the day-count convention methods.  First, create two
// 'bdet_Dates' 'd1' and 'd2'.
//..
//  const bdet_Date d1(2003, 10, 18);
//  const bdet_Date d2(2003, 12, 31);
//..
// To compute the day-count between these two dates, we can use a base-class
// reference:
//..
//  const int daysDiff = dcc.daysDiff(d1, d2);
//  assert(72 == daysDiff);
//..
// To compute the year fraction between these two dates, we can again use the
// base-class reference:
//..
//  const double yearsDiff = dcc.yearsDiff(d1, d2);
//  // Need fuzzy comparison since 'yearsDiff' is a double.  Expect
//  // '0.2 == yearsDiff'.
//  assert(0.1999 < yearsDiff);
//  assert(0.2001 > yearsDiff);
//..

#ifndef INCLUDED_BBESCM_VERSION
#include <bbescm_version.h>
#endif

#ifndef INCLUDED_BBEDC_DAYCOUNTINTERFACE
#include <bbedc_daycountinterface.h>
#endif

namespace BloombergLP {

class bdet_Date;

                       // ===========================
                       // class bbedc_DayCountAdapter
                       // ===========================

template <typename CONVENTION>
class bbedc_DayCountAdapter : public bbedc_DayCountInterface {
    // This 'class' provides an "adapter" from the specified 'CONVENTION' to
    // the 'bbedc_DayCountInterface' that can be used for determining values
    // based on dates according to the 'CONVENTION' day-count convention.

  public:
    // ACCESSORS
    int daysDiff(const bdet_Date& beginDate, const bdet_Date& endDate) const;
        // Return the number of days between the specified 'beginDate' and
        // 'endDate' as per the 'CONVENTION' template policy.  If
        // 'beginDate <= endDate', then the result is non-negative.  Note that
        // reversing the order of 'beginDate' and 'endDate' negates the result.

    double yearsDiff(const bdet_Date& beginDate,
                     const bdet_Date& endDate) const;
        // Return the number of years between the specified 'beginDate' and
        // 'endDate' as per the 'CONVENTION' template policy.  If
        // 'beginDate <= endDate', then the result is non-negative.  The
        // behavior is undefined if either 'beginDate' or 'endDate' is in the
        // year 1752.  Note that reversing the order of 'beginDate' and
        // 'endDate' negates the result.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                       // ---------------------------
                       // class bbedc_DayCountAdapter
                       // ---------------------------

// ACCESSORS
template <typename CONVENTION>
inline
int bbedc_DayCountAdapter<CONVENTION>::daysDiff(const bdet_Date& beginDate,
                                                const bdet_Date& endDate) const
{
    return CONVENTION::daysDiff(beginDate, endDate);
}

template <typename CONVENTION>
inline
double
bbedc_DayCountAdapter<CONVENTION>:: yearsDiff(const bdet_Date& beginDate,
                                              const bdet_Date& endDate) const
{
    return CONVENTION::yearsDiff(beginDate, endDate);
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
