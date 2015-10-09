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

#include <bsls_assert.h>

#include <bsl_istream.h>
#include <bsl_set.h>
#include <bsl_utility.h>

namespace BloombergLP {

                              // ----------------
                              // class TestLoader
                              // ----------------

// CREATORS
baltzo::TestLoader::~TestLoader()
{
}

// MANIPULATORS
void baltzo::TestLoader::setTimeZone(const Zoneinfo& timeZone)
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

int baltzo::TestLoader::setTimeZone(const char *timeZoneId,
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

int baltzo::TestLoader::loadTimeZone(Zoneinfo *result, const char *timeZoneId)
{
    BSLS_ASSERT(0 != result);
    BSLS_ASSERT(0 != timeZoneId);

    TimeZoneMap::const_iterator it = d_timeZones.find(timeZoneId);

    if (it == d_timeZones.end()) {
        return ErrorCode::k_UNSUPPORTED_ID;                           // RETURN
    }

    *result = it->second;
    return 0;
}

bsl::ostream& baltzo::TestLoader::print(bsl::ostream& stream,
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

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
