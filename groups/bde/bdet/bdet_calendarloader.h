// bdet_calendarloader.h                                              -*-C++-*-
#ifndef INCLUDED_BDET_CALENDARLOADER
#define INCLUDED_BDET_CALENDARLOADER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a protocol (or pure interface) for loading calendars.
//
//@CLASSES:
//  bdet_CalendarLoader: pure interface for loading calendars
//
//@AUTHOR: Guillaume Morin (gmorin1)
//
//@SEE_ALSO: bdet_calendarcache, bdet_packedcalendar
//
//@DESCRIPTION: This class defines a protocol class used to load calendars
// from a specific source.  Different repositories can be supported by a
// specific implementation.  This class can be used alone or as a loading
// plug-in for 'bdet_CalendarCache'.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

namespace BloombergLP {

class bdet_PackedCalendar;

                         // =========================
                         // class bdet_CalendarLoader
                         // =========================

class bdet_CalendarLoader {
    // This class defines a protocol class used to load calendars from
    // a specific source.  Different repositories can be supported by a
    // specific implementation.

  public:
    // CREATORS
    virtual ~bdet_CalendarLoader();
        // Destroy this object.

    // MANIPULATORS
    virtual int load(bdet_PackedCalendar *result,
                     const char          *calendarName) = 0;
        // Load, into the specified 'result', the calendar corresponding to the
        // specified 'calendarName'.  Return 0 on success, and a non-zero value
        // otherwise.  If the return value is 1, the calendar was not found and
        // '*result' is unchanged.  If the return value is any other non-zero
        // value, then some other error occurred, and the state of the object
        // pointed to by 'result' is valid, but its value is undefined.
};

} // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
