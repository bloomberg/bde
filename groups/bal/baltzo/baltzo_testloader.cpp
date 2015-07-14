// baltzo_testloader.cpp                                              -*-C++-*-
#include <baltzo_testloader.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baltzo_testloader_cpp,"$Id$ $CSID$")

#include <baltzo_errorcode.h>
#include <baltzo_zoneinfo.h>
#include <baltzo_zoneinfobinaryreader.h>

#include <bdlsb_fixedmeminstreambuf.h>

#include <bslim_printer.h>
#include <bslma_allocator.h>

#include <bsl_istream.h>
#include <bsl_utility.h>

namespace BloombergLP {

namespace baltzo {
                       // -----------------------
                       // class TestLoader
                       // -----------------------

// CREATORS
TestLoader::~TestLoader()
{
}

// MANIPULATORS
void TestLoader::setTimeZone(const Zoneinfo& timeZone)
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

int TestLoader::setTimeZone(const char *timeZoneId,
                                   const char *timeZoneData,
                                   int         timeZoneDataNumBytes)
{
    BSLS_ASSERT(0 != timeZoneId);
    BSLS_ASSERT(0 != timeZoneData);
    BSLS_ASSERT(0 <= timeZoneDataNumBytes);

    bdlsb::FixedMemInStreamBuf inBuf(timeZoneData, timeZoneDataNumBytes);
    bsl::istream              input(&inBuf);
    Zoneinfo           timeZone;

    if (0 != ZoneinfoBinaryReader::read(&timeZone,
                                               input)) {
        return 1;                                                     // RETURN
    }

    timeZone.setIdentifier(timeZoneId);
    setTimeZone(timeZone);

    return 0;
}

int TestLoader::loadTimeZone(Zoneinfo *result,
                                    const char      *timeZoneId)
{
    BSLS_ASSERT(0 != result);
    BSLS_ASSERT(0 != timeZoneId);

    TimeZoneMap::const_iterator it = d_timeZones.find(timeZoneId);

    if (it == d_timeZones.end()) {
        return ErrorCode::BAETZO_UNSUPPORTED_ID;               // RETURN
    }

    *result = it->second;
    return 0;
}

bsl::ostream& TestLoader::print(bsl::ostream& stream,
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
        printer.printAttribute("Zoneinfo", itr->second);
    }
    printer.end();

    return stream;
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
