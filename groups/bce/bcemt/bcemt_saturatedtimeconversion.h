// bcemt_saturatedtimeconversion.h                                    -*-C++-*-
#ifndef INCLUDED_BCEMT_SATURATEDTIMECONVERSION
#define INCLUDED_BCEMT_oSATURATEDTIMECONVERSION

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide saturating converssions for time values.
//
//@CLASSES:
//   bcemt_SaturatedTimeConversion: namespace for saturating time conversions.
//
//@SEE_ALSO:
//
//@AUTHOR: Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component defines a namespace containing static functions
// suitable for performing narrowing conversions of numbers such that if an
// attempt is made to assign a value that is outside the range that is
// representable by the destination variable, the destination variable will
// take on its maximum or minimum value, whichever is closer to the value being
// assigned.
//
///Usage
///-----

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BSL_CLIMITS
#include <bsl_climits.h>
#endif

#ifndef INCLUDED_BCES_PLATFORM
#include <bces_platform.h>
#endif

#ifndef INCLUDED_BDET_TIMEINTERVAL
#include <bdet_timeinterval.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifdef BSLS_PLATFORM_OS_UNIX

#ifndef INCLUDED_BSL_CTIME
#include <bsl_ctime.h>
#endif

#else

#ifndef INCLUDED_WINDOWS
#include <windows.h>
#endif

#endif

namespace BloombergLP {

                             // ===================
                             // class bcemt_Barrier
                             // ===================

class bcemt_SaturatedTimeConversion {

  private:

  public:
#ifdef BCES_PLATFORM_POSIX_THREADS
    void toTimeSpec(timespec *dst, const bdet_TimeInterval& src);
    void toTimeT(bsl::time_t *dst, const bsls::Types::Int64);
#endif

#ifdef bces_platform_WIN32_THREADS
    void toMillesec(DWORD *dst, const bdet_TimeInterval& src);
#endif

};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

// PRIVATE CLASS METHODS
template <typename TYPE>
inline
TYPE bcemt_SaturatedTimeConversion::max(const TYPE&) {
    return numeric_limits<TYPE>::max();
};

template <typename TYPE>
inline
TYPE bcemt_SaturatedTimeConversion::min(const TYPE&) {
    return numeric_limits<TYPE>::min();
};

// CLASS METHODS



}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
