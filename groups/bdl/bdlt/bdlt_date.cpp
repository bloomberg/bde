// bdlt_date.cpp                                                      -*-C++-*-
#include <bdlt_date.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlt_date_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsls_performancehint.h>
#include <bsls_platform.h>

#include <bsl_ostream.h>

#include <bsl_c_stdio.h>   // 'snprintf'

namespace BloombergLP {
namespace bdlt {

static const char *const months[] = {
    0,
    "JAN", "FEB", "MAR", "APR", "MAY", "JUN",
    "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
};

                                  // ----------
                                  // class Date
                                  // ----------

// MANIPULATORS
int Date::addDaysIfValid(int numDays)
{
    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    const int tmpSerialDate = d_serialDate + numDays;

    if (!Date::isValidSerial(tmpSerialDate)) {
        return k_FAILURE;                                             // RETURN
    }

    d_serialDate = tmpSerialDate;

    return k_SUCCESS;
}

// ACCESSORS
bsl::ostream& Date::print(bsl::ostream& stream,
                          int           level,
                          int           spacesPerLevel) const
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(stream.bad())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return stream;                                                // RETURN
    }

    // Typical space usage (10 bytes): ddMMMyyyy nil.  Reserve 128 bytes for
    // possible BAD DATE result, which is sufficient space for the bad date
    // verbose message even on a 64-bit system.

    char buffer[128];

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                         !Date::isValidSerial(d_serialDate))) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

#if defined(BSLS_PLATFORM_CMP_MSVC)
#define snprintf _snprintf
#endif

        snprintf(buffer,
                 sizeof buffer,
                 "*BAD*DATE:%p->d_serialDate=%d",
                 this,
                 d_serialDate);

        BSLS_ASSERT_SAFE(
                 !"'bdlt::Date::print' attempted on date with invalid state.");
    }
    else {
#endif  // defined(BSLS_ASSERT_OPT_IS_ACTIVE)

        int y, m, d;
        getYearMonthDay(&y, &m, &d);

        const char *const month = months[m];

        buffer[0] = static_cast<char>(d / 10 + '0');
        buffer[1] = static_cast<char>(d % 10 + '0');
        buffer[2] = month[0];
        buffer[3] = month[1];
        buffer[4] = month[2];
        buffer[5] = static_cast<char>(  y / 1000         + '0');
        buffer[6] = static_cast<char>(((y % 1000) / 100) + '0');
        buffer[7] = static_cast<char>(((y %  100) /  10) + '0');
        buffer[8] = static_cast<char>(  y %   10         + '0');
        buffer[9] = 0;

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
    }
#endif  // defined(BSLS_ASSERT_OPT_IS_ACTIVE)

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start(true);  // 'true' -> suppress '['
    stream << buffer;
    printer.end(true);    // 'true' -> suppress ']'

    return stream;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
