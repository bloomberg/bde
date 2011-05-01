// baetzo_testloader.cpp                                              -*-C++-*-
#include <baetzo_testloader.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baetzo_testloader_cpp,"$Id$ $CSID$")

#include <baetzo_errorcode.h>
#include <baetzo_zoneinfo.h>
#include <baetzo_zoneinfobinaryreader.h>

#include <bdesb_fixedmeminstreambuf.h>

#include <bslim_printer.h>
#include <bslma_allocator.h>

#include <bsl_istream.h>

namespace BloombergLP {

                       // -----------------------
                       // class baetzo_TestLoader
                       // -----------------------

// CREATORS
baetzo_TestLoader::~baetzo_TestLoader()
{
}

// MANIPULATORS
void baetzo_TestLoader::setTimeZone(const baetzo_Zoneinfo& timeZone)
{
    TimeZoneMap::iterator it = d_timeZones.find(timeZone.identifier());
    if (it == d_timeZones.end()) {
        d_timeZones.insert(bsl::make_pair(timeZone.identifier(),
                                          timeZone));
    }
    else {
        it->second = timeZone;
    }
}

int baetzo_TestLoader::setTimeZone(const char *timeZoneId,
                                   const char *timeZoneData,
                                   int         timeZoneDataNumBytes)
{
    BSLS_ASSERT(0 != timeZoneId);
    BSLS_ASSERT(0 != timeZoneData);
    BSLS_ASSERT(0 <= timeZoneDataNumBytes);

    bdesb_FixedMemInStreamBuf inBuf(timeZoneData, timeZoneDataNumBytes);
    bsl::istream              input(&inBuf);
    baetzo_Zoneinfo           timeZone;

    if (0 != baetzo_ZoneinfoBinaryReader::read(&timeZone,
                                               input)) {
        return 1;                                                     // RETURN
    }

    timeZone.setIdentifier(timeZoneId);
    setTimeZone(timeZone);

    return 0;
}

int baetzo_TestLoader::loadTimeZone(baetzo_Zoneinfo *result,
                                    const char      *timeZoneId)
{
    BSLS_ASSERT(0 != result);
    BSLS_ASSERT(0 != timeZoneId);

    TimeZoneMap::const_iterator it = d_timeZones.find(timeZoneId);

    if (it == d_timeZones.end()) {
        return baetzo_ErrorCode::BAETZO_UNSUPPORTED_ID;               // RETURN
    }

    *result = it->second;
    return 0;
}

bsl::ostream& baetzo_TestLoader::print(bsl::ostream& stream,
                                       int           level,
                                       int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    for (TimeZoneMap::const_iterator itr  = d_timeZones.begin(),
                                     end  = d_timeZones.end();
                                     end != itr; ++itr) {
        printer.print(itr->second, "Zoneinfo");
    }
    printer.end();

    return stream;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
