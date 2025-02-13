// bdlt_fuzzutil.cpp                                                  -*-C++-*-
#include <bdlt_fuzzutil.h>

#include <bsls_types.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlt_fuzzutil_cpp,"$Id$ $CSID$")

namespace BloombergLP {
namespace bdlt {

                              // ---------------
                              // struct FuzzUtil
                              // ---------------

// CLASS METHODS
DateTz FuzzUtil::consumeDateTz(bslim::FuzzDataView *fuzzDataView)
{
    return DateTz(consumeDate(fuzzDataView), consumeTz(fuzzDataView));
}

DateTz FuzzUtil::consumeDateTzInRange(bslim::FuzzDataView *fuzzDataView,
                                      const bdlt::Date&    begin,
                                      const bdlt::Date&    end)
{
    return DateTz(consumeDateInRange(fuzzDataView, begin, end),
                  consumeTz(fuzzDataView));
}

Datetime FuzzUtil::consumeDatetime(bslim::FuzzDataView *fuzzDataView)
{
    return Datetime(consumeDate(fuzzDataView), consumeTime(fuzzDataView));
}

Datetime FuzzUtil::consumeDatetimeInRange(bslim::FuzzDataView *fuzzDataView,
                                          const bdlt::Date&    begin,
                                          const bdlt::Date&    end)
{
    return Datetime(consumeDateInRange(fuzzDataView, begin, end),
                    consumeTime(fuzzDataView));
}

DatetimeTz FuzzUtil::consumeDatetimeTz(bslim::FuzzDataView *fuzzDataView)
{
    return DatetimeTz(consumeDatetime(fuzzDataView), consumeTz(fuzzDataView));
}

DatetimeTz FuzzUtil::consumeDatetimeTzInRange(
                                             bslim::FuzzDataView *fuzzDataView,
                                             const bdlt::Date&    begin,
                                             const bdlt::Date&    end)
{
    return DatetimeTz(consumeDatetimeInRange(fuzzDataView, begin, end),
                      consumeTz(fuzzDataView));
}

Time FuzzUtil::consumeTime(bslim::FuzzDataView *fuzzDataView)
{
    // 24h == 86'400'000'000us so we need at least 5 bytes
    bsls::Types::Uint64 t =
                        bslim::FuzzUtil::consumeNumber<unsigned>(fuzzDataView);
    t <<= 8;
    t |= bslim::FuzzUtil::consumeNumber<unsigned char>(fuzzDataView);

    int microsecond = static_cast<int>(t % 1000U);
    t /= 1000U;

    int millisecond = static_cast<int>(t % 1000U);
    t /= 1000U;

    int second = static_cast<int>(t % 60U);
    t /= 60U;

    int minute = static_cast<int>(t % 60U);
    t /= 60U;

    int hour = static_cast<int>(t % 24U);
    return Time(hour, minute, second, millisecond, microsecond);
}

TimeTz FuzzUtil::consumeTimeTz(bslim::FuzzDataView *fuzzDataView)
{
    return TimeTz(consumeTime(fuzzDataView), consumeTz(fuzzDataView));
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
