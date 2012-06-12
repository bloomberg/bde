// bbedc_daycountinterface.h            -*-C++-*-
#ifndef INCLUDED_BBEDC_DAYCOUNTINTERFACE
#define INCLUDED_BBEDC_DAYCOUNTINTERFACE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT_RCSID(bbedc_daycountinterface_h,"$Id$ $CSID$")
BDES_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide an interface for day-count calculations.
//
//@CLASSES:
//   bbedc_DayCountInterface: interface for day-count calculations
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component provides an interface for implementing an
// arbitrary day-count convention.  Concrete implementations of this interface
// may implement, e.g., the ISMA 30/360 day-count convention, or a custom
// convention appropriate for some niche market.
//
// Several of the components in 'bbedc' already provide individual day-count
// convention support through interfaces that happen to be functionally
// identical to this contained abstract interface, except that do not inherit
// from 'bbedc_DayCountInterface'.  In conjunction with
// 'bbedc_DayCountAdapter', this component interface is intended for allowing
// run-time binding of these and other similar day-count implementations.
//
///Usage
///-----
// This example shows the definition and use of a simple concrete day-count
// convention.  This functionality suffices to demonstrate the requisite steps
// for having a working day-count convention:
//..
//  1. Define a concrete day-count type derived from 'bbedc_DayCountInterface'.
//  2. Implement the pure virtual 'daysDiff' and 'yearsDiff' methods.
//  3. Instantiate and use an object of the concrete type.
//..
// We first define the (derived) 'my_DayCountConvention' class and implement
// its constructor inline (for convenience, directly within the derived-class
// definition):
//..
//  // my_daycountconvention.h
//
//  class my_DayCountConvention : public bbedc_DayCountInterface {
//    public:
//      my_DayCountConvention() { }
//      virtual ~my_DayCountConvention();
//      virtual int daysDiff(const bdet_Date& beginDate,
//                           const bdet_Date& endDate) const;
//          // Return the number of days between the specified ...
//      virtual double yearsDiff(const bdet_Date& beginDate,
//                               const bdet_Date& endDate) const;
//          // Return the number of years between the specified ...
//  };
//..
// Note, however, that we always implement a virtual destructor (non-inline) in
// the .cpp file (to indicate the *unique* location of the class's virtual
// table):
//..
//  // my_daycountconvention.cpp
//
//  // ...
//
//  my_DayCountConvention::~my_DayCountConvention() { }
//..
// We next implement the (virtual) 'daysDiff' and 'yearsDiff' methods, which
// incorporate the "policy" of what it means for this convention to calculate
// these values.
//..
//  int my_DayCountConvention::daysDiff(const bdet_Date& beginDate,
//                                      const bdet_Date& endDate) const
//  {
//      return endDate - beginDate;
//  }
//
//  double my_DayCountConvention::yearsDiff(const bdet_Date& beginDate,
//                                          const bdet_Date& endDate) const
//  {
//      return (double)(endDate - beginDate) / 365.0;
//  }
//..
// We now want to use the 'my_DayCountConvention' object and its day-count
// convention methods.  We illustrate this use by instantiating the object and
// then performing calculations through the 'bbedc_DayCountInterface'.  First,
// create two 'bdet_Dates' 'd1' and 'd2':
//..
//  const bdet_Date d1(2003, 10, 19);
//  const bdet_Date d2(2003, 12, 31);
//..
// Then obtain a 'bbedc_DayCountInterface' reference from an instantiated
// 'my_DayCountConvention':
//..
//  my_DayCountConvention myDcc;
//  const bbedc_DayCountInterface& dcc = myDcc;
//..
// To compute the day-count between these two dates:
//..
//  const int daysDiff = dcc.daysDiff(d1, d2);
//  assert(73 == daysDiff);
//..
// To compute the year fraction between these two dates:
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

namespace BloombergLP {

class bdet_Date;

                      // =============================
                      // class bbedc_DayCountInterface
                      // =============================

class bbedc_DayCountInterface {
    // This 'class' provides an interface for determining values based on dates
    // according to derived implementations of specific day-count conventions.

  public:
    // CREATORS
    virtual ~bbedc_DayCountInterface();
        // Destroy this object.

    // ACCESSORS
    virtual int daysDiff(const bdet_Date& beginDate,
                         const bdet_Date& endDate) const = 0;
        // Return the number of days between the specified 'beginDate' and
        // 'endDate'.  If 'beginDate <= endDate', then the result is
        // non-negative.  Note that reversing the order of 'beginDate' and
        // 'endDate' negates the result.

    virtual double yearsDiff(const bdet_Date& beginDate,
                             const bdet_Date& endDate) const = 0;
        // Return the number of years between the specified 'beginDate' and
        // 'endDate'.  If 'beginDate <= endDate', then the result is
        // non-negative.  The behavior is undefined if either 'beginDate' or
        // 'endDate' is in the year 1752.  Note that reversing the order of
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
