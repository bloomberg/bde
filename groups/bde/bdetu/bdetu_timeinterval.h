// bdetu_timeinterval.h                                               -*-C++-*-
#ifndef INCLUDED_BDETU_TIMEINTERVAL
#define INCLUDED_BDETU_TIMEINTERVAL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide non-primitive procedures on 'bdet_TimeInterval'.
//
//@CLASSES:
//  bdetu_TimeInterval: namespace for operations on 'bdet_TimeInterval'
//
//@AUTHOR: Marty Vasas (mvasas)
//
//@SEE_ALSO: bdet_datetimeinterval, bdet_timeinterval, bdetu_epoch
//
//@DESCRIPTION: This component provides operations on 'bdet_TimeInterval'
// objects.  In particular, this component supplies a method that converts a
// 'bdet_DatetimeInterval' object to a 'bdet_TimeInterval' object.  A
// 'bdet_TimeInterval' has greater range and precision than a
// 'bdet_DatetimeInterval', so the conversion is always safe.
//
///Usage
///-----
// The following snippet of code demonstrates how to use this utility component
// to convert a 'bdet_DatetimeInterval' to a 'bdet_TimeInterval':
//..
//  bdet_DatetimeInterval dti(0, 8, 15, 14, 416); // 8 hr. 15 min. 14.416 sec.
//
//  bdet_TimeInterval timeInterval;
//
//  bdetu_TimeInterval::convertToTimeInterval(&timeInterval, dti);
//  bsl::cout << timeInterval << bsl::endl;
//..
// The above code will produce the following on 'stdout':
//..
//  (29714, 416000000)
//..
// This corresponds to a time interval of 29,714 seconds and 416,000,000
// nanoseconds.
//
// Another common usage is the conversion between 'bdet_Datetime' and
// 'bdet_TimeInterval'.  This functionality is provided in 'bdetu_epoch'.
// However, the following snippet of code demonstrates how to accomplish this
// with 'bdetu_TimeInterval':
//..
//  bdet_Datetime datetime(2000, 1, 1, 0, 0, 0, 0);
//
//  bdet_TimeInterval timeInterval;
//
//  bdetu_TimeInterval::convertToTimeInterval(&timeInterval,
//                                            datetime - bdetu_Epoch::epoch());
//  bsl::cout << timeInterval << bsl::endl;
//..
// The above code will produce the following on 'stdout':
//..
//  (946684800, 000000000)
//..
// This corresponds to a time interval of 946684800 seconds, 0 nanoseconds.

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

                        // =========================
                        // struct bdetu_TimeInterval
                        // =========================

struct bdetu_TimeInterval {
    // This 'struct' provides a namespace for a suite of non-primitive pure
    // procedures operating on instances of type 'bdet_TimeInterval'.
    // These methods are alias-safe, thread-safe, and exception-neutral.

    // TYPES
    enum {
        BDETU_NANOSECS_PER_MILLISEC = 1000000

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , NANOSECS_PER_MILLISEC       = BDETU_NANOSECS_PER_MILLISEC
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };

  public:
    // CLASS METHODS
    static void convertToTimeInterval(
                               bdet_TimeInterval            *result,
                               const bdet_DatetimeInterval&  datetimeInterval);
        // Load into the specified 'result' the value of the specified
        // 'bdet_DatetimeInterval'.  This method is thread-safe, alias-safe,
        // and exception-neutral.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                       // -------------------------
                       // struct bdetu_TimeInterval
                       // -------------------------

// CLASS METHODS
inline
void bdetu_TimeInterval::convertToTimeInterval(
                                bdet_TimeInterval            *result,
                                const bdet_DatetimeInterval&  datetimeInterval)
{
    BSLS_ASSERT_SAFE(result);

    result->setInterval(datetimeInterval.totalSeconds(),
                        datetimeInterval.milliseconds()
                                                * BDETU_NANOSECS_PER_MILLISEC);
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
