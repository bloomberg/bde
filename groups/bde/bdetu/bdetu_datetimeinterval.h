// bdetu_datetimeinterval.h                                           -*-C++-*-
#ifndef INCLUDED_BDETU_DATETIMEINTERVAL
#define INCLUDED_BDETU_DATETIMEINTERVAL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide non-primitive procedures on 'bdet_DatetimeInterval'.
//
//@CLASSES:
//  bdetu_DatetimeInterval: namespace for operations on 'bdet_DatetimeInterval'
//
//@AUTHOR: Marty Vasas (mvasas)
//
//@SEE_ALSO: bdet_datetimeinterval, bdet_timeinterval, bdetu_timeinterval
//
//@DESCRIPTION: This component provides operations on 'bdet_DatetimeInterval'
// objects.  In particular, this component supplies a method that converts a
// 'bdet_TimeInterval' object to a 'bdet_DatetimeInterval' object.  A
// 'bdet_TimeInterval' has greater range and precision than a
// 'bdet_DatetimeInterval' so the conversion will, in general, lose precision
// and can overflow at extreme ranges.
//
///Usage
///-----
// The following snippet of code illustrates how to use this utility component
// to convert a 'bdet_TimeInterval' to a 'bdet_DatetimeInterval':
//..
//  bdet_TimeInterval timeInterval(10801, 14031416);  // 10801.014031416 sec.
//  bdet_DatetimeInterval datetimeInterval;
//
//  bdet_DatetimeInterval::convertToDatetimeInterval(&datetimeInterval,
//                                                   timeInterval);
//  bsl::cout << datetimeInterval << bsl::endl;
//..
// The above code will produce the following on 'stdout':
//..
//  +0_03:00:01.014
//..
// This corresponds to an interval of 0 days, 3 hours and 1.014 seconds, with a
// loss of precision beyond the granularity of microseconds.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDET_DATETIMEINTERVAL
#include <bdet_datetimeinterval.h>
#endif

#ifndef INCLUDED_BDET_TIMEINTERVAL
#include <bdet_timeinterval.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {

                        // =============================
                        // struct bdetu_DatetimeInterval
                        // =============================

struct bdetu_DatetimeInterval {
    // This 'struct' provides a namespace for a suite of non-primitive pure
    // procedures operating on instances of type 'bdet_DatetimeInterval'.
    // These methods are alias-safe, thread-safe, and exception-neutral.

    // TYPES
    enum {
        BDETU_NANOSECS_PER_MILLISEC = 1000000

#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , NANOSECS_PER_MILLISEC = BDETU_NANOSECS_PER_MILLISEC
#endif
    };

  public:
    // CLASS METHODS
    static void convertToDatetimeInterval(
                                      bdet_DatetimeInterval    *result,
                                      const bdet_TimeInterval&  timeInterval);
        // Load into the specified 'result' the value of the specified
        // 'timeInterval'.  The behavior is undefined unless 'timeInterval' is
        // small enough to fit into a 'bdet_DatetimeInterval'.  Note that a
        // 'bdet_TimeInterval' has greater precision than a
        // 'bdet_DatetimeInterval', so there may be a loss of precision.
        // This method is exception-neutral, alias-safe, and thread-safe.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                       // -----------------------------
                       // struct bdetu_DatetimeInterval
                       // -----------------------------

// CLASS METHODS
inline
void bdetu_DatetimeInterval::convertToDatetimeInterval(
                                        bdet_DatetimeInterval    *result,
                                        const bdet_TimeInterval&  timeinterval)
{
    BSLS_ASSERT_SAFE(result);

    result->setInterval(0, 0, 0, timeinterval.seconds(),
                     timeinterval.nanoseconds() / BDETU_NANOSECS_PER_MILLISEC);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
