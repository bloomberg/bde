// bdet_month.h                                                       -*-C++-*-
#ifndef INCLUDED_BDET_MONTH
#define INCLUDED_BDET_MONTH

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a namespace for representing month-of-year values.
//
//@DEPRECATED: Use 'bdet_monthofyear' instead.
//
//@CLASSES:
//   bdet_Month: namespace for enumerated month-of-year values
//
//@DESCRIPTION: 'bdet_Month' provides a compatibility definition equivalent to
// 'bdet_MonthOfYear'.  See the 'bdet_monthofyear' component for the complete
// description.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDET_MONTHOFYEAR
#include <bdet_monthofyear.h>
#endif

#ifndef bdet_Month
#define bdet_Month bdet_MonthOfYear
#endif

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
