// bdetu_informixdatetime.h                                           -*-C++-*-
#ifndef INCLUDED_BDETU_INFORMIXDATETIME
#define INCLUDED_BDETU_INFORMIXDATETIME

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Supply informix datetime operations on 'bdet_Datetime'.
//
//@CLASSES:
//   bdetu_InformixDatetime: informix datetime procedures on 'bdet_Datetime'
//
//@SEE_ALSO: bdet_datetime
//
//@AUTHOR: Jason Ku (JKu)
//
//@DESCRIPTION: This component provides informix datetime operations on
// 'bdet_Datetime' objects.  In particular, 'bdetu_Datetime' supplies
// conversions to and from informix datetime in the format of
// "YYYY-MM-DD HH:MM:SS.HH"
//
///SYNOPSIS
///--------
// This utility component provides the following (static) methods:
//..
//  static void convertToInformixTime(char                 *result,
//                                    const bdet_Datetime&  datetime);
//  static void convertToInformixTime(bsl::string          *result,
//                                    const bdet_Datetime&  datetime);
//  static int  convertFromInformixTime(bdet_Datetime *result,
//                                      const char    *datetime);
//  static int  convertFromInformixTime(bdet_Datetime      *result,
//                                      const bsl::string&  datetime);
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDET_DATETIME
#include <bdet_datetime.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSL_IOMANIP
#include <bsl_iomanip.h>
#endif

#ifndef INCLUDED_BSL_SSTREAM
#include <bsl_sstream.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_C_STDIO
#include <bsl_c_stdio.h>       // for 'snprintf'
#endif

namespace BloombergLP {

                       // =============================
                       // struct bdetu_InformixDatetime
                       // =============================

struct bdetu_InformixDatetime {
    // This 'struct' provides a namespace for a suite of non-primitive pure
    // procedures operating on instances of type 'bdet_Datetime'.  These
    // methods are alias-safe, thread-safe, and exception-neutral.

    // CLASS METHODS
    static void convertToInformixTime(char                 *result,
                                      const bdet_Datetime&  datetime);
        // Load into the specified 'result' the value of the specified
        // 'datetime'.  The behavior is undefined unless the 'result' address
        // is at least 23 bytes.

    static void convertToInformixTime(bsl::string          *result,
                                      const bdet_Datetime&  datetime);
        // Load into the specified 'result' the value of the specified
        // 'datetime'.

    static int  convertFromInformixTime(bdet_Datetime *result,
                                        const char    *datetime);
        // Load into the specified 'result' the value of the specified
        // 'datetime'.  Return 0 on success, and a non-zero value with
        // no effect on result otherwise.

    static int  convertFromInformixTime(bdet_Datetime      *result,
                                        const bsl::string&  datetime);
        // Load into the specified 'result' the value of the specified
        // 'datetime'.  Return 0 on success, and a non-zero value with
        // no effect on result otherwise.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                       // -----------------------------
                       // struct bdetu_InformixDatetime
                       // -----------------------------

// CLASS METHODS
inline
void
bdetu_InformixDatetime::convertToInformixTime(char                 *result,
                                              const bdet_Datetime&  datetime)
{
    BSLS_ASSERT_SAFE(result);

    #if defined(BSLS_PLATFORM__CMP_MSVC)
    sprintf_s
    #else
    snprintf
    #endif
        (result, 23, "%04d-%02d-%02d %02d:%02d:%02d.%02d",
            datetime.year(), datetime.month(),  datetime.day(),
            datetime.hour(), datetime.minute(), datetime.second(),
            datetime.millisecond()/10);
}

inline
void
bdetu_InformixDatetime::convertToInformixTime(bsl::string          *result,
                                              const bdet_Datetime&  datetime)
{
    BSLS_ASSERT_SAFE(result);

    bsl::stringstream ifmx_dt;

    ifmx_dt << bsl::setw(4) << bsl::setfill('0') << datetime.year()
     << '-' << bsl::setw(2) << bsl::setfill('0') << datetime.month()
     << '-' << bsl::setw(2) << bsl::setfill('0') << datetime.day()
     << ' ' << bsl::setw(2) << bsl::setfill('0') << datetime.hour()
     << ':' << bsl::setw(2) << bsl::setfill('0') << datetime.minute()
     << ':' << bsl::setw(2) << bsl::setfill('0') << datetime.second()
     << '.' << bsl::setw(2) << bsl::setfill('0') << datetime.millisecond()/10;

    *result = ifmx_dt.str();
}

inline
int bdetu_InformixDatetime::convertFromInformixTime(bdet_Datetime *result,
                                                    const char    *datetime)
{
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(datetime);

    int year, month,  day;
    int hour, minute, second, hundredth;

    #if defined(BSLS_PLATFORM__CMP_MSVC)
        // sscanf_s and sscanf are equivalent for this purpose, and sscanf_s
        // is not deprecated on MSVC
        #define bdetu_idt_sscanf sscanf_s
    #else
        #define bdetu_idt_sscanf sscanf
    #endif

    if (bdetu_idt_sscanf(datetime, "%d-%d-%d %d:%d:%d.%d",
                         &year, &month, &day, &hour, &minute,
                         &second, &hundredth) != 7) {
        return -1;
    }

    #undef bdetu_idt_sscanf

    return result->setDatetimeIfValid(year,
                                      month,
                                      day,
                                      hour,
                                      minute,
                                      second,
                                      hundredth * 10);
}

inline
int
bdetu_InformixDatetime::convertFromInformixTime(bdet_Datetime      *result,
                                                const bsl::string&  datetime)
{
    BSLS_ASSERT_SAFE(result);

    return convertFromInformixTime(result, datetime.c_str());
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
