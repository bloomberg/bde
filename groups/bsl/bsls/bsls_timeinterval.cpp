// bsls_timeinterval.cpp                                              -*-C++-*-
#include <bsls_timeinterval.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bsls_timeinterval_cpp,"$Id$ $CSID$")

#include <bsls_asserttest.h>   // for testing only
#include <bsls_bsltestutil.h>  // for testing only
#include <bsls_nameof.h>       // for testing only

#include <stdio.h>             // snprintf()

#include <ostream>

// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -FABC01

namespace BloombergLP {
namespace bsls {

namespace {

struct bsls_TimeInterval_Assertions {
    char assertion1[-3 / 2 == -1 ? 1 : -1];
    char assertion2[-5 % 4 == -1 ? 1 : -1];
        // Ensure that the compiler maintains the sign of the remainder (the
        // resulting sign is compiler implementation defined, though our
        // current production compilers maintain the sign).
};

}  // close unnamed namespace

                        // ------------------
                        // class TimeInterval
                        // ------------------

// CREATORS
TimeInterval::TimeInterval(double seconds)
{
    if (seconds < 0) {
        d_seconds = -bsls::Types::Int64(-seconds);

        // The use of a volatile storage classifier is necessary on x86 builds
        // with gcc.

#if defined(BSLS_PLATFORM_CPU_X86)                                            \
 && (defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG))
        const volatile double nanoseconds =
#else
        const double nanoseconds =
#endif
             (static_cast<double>(d_seconds) - seconds) *
                                       static_cast<double>(k_NANOSECS_PER_SEC);
        d_nanoseconds = -static_cast<int>(nanoseconds + 0.5);
        if (d_nanoseconds == -k_NANOSECS_PER_SEC) {
            --d_seconds;
            d_nanoseconds = 0;
        }
    }
    else {
        d_seconds = static_cast<bsls::Types::Int64>(seconds);

        // The use of a volatile storage classifier is necessary on x86 builds
        // with gcc.

#if defined(BSLS_PLATFORM_CPU_X86)                                            \
 && (defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG))
        const volatile double nanoseconds =
#else
        const double nanoseconds =
#endif
               (seconds - static_cast<double>(d_seconds)) *
                                       static_cast<double>(k_NANOSECS_PER_SEC);
        d_nanoseconds = static_cast<int>(nanoseconds + 0.5);
        if (d_nanoseconds == k_NANOSECS_PER_SEC) {
            ++d_seconds;
            d_nanoseconds = 0;
        }
    }
}

// MANIPULATORS
TimeInterval& TimeInterval::addInterval(bsls::Types::Int64 seconds,
                                        int                nanoseconds)
{
    BSLS_PRECONDITIONS_BEGIN();
    BSLS_ASSERT(isSumValidInt64(d_seconds, seconds));
    BSLS_ASSERT(isSumValidInt64(
       d_seconds + seconds,
       (static_cast<bsls::Types::Int64>(d_nanoseconds) + nanoseconds) /
                                                          k_NANOSECS_PER_SEC));
    BSLS_PRECONDITIONS_END();

    d_seconds += seconds;
    bsls::Types::Int64 nanosecs = static_cast<bsls::Types::Int64>(nanoseconds)
                                + d_nanoseconds;

    if (nanosecs >=  k_NANOSECS_PER_SEC
     || nanosecs <= -k_NANOSECS_PER_SEC) {
        d_seconds     +=                  nanosecs / k_NANOSECS_PER_SEC;
        d_nanoseconds  = static_cast<int>(nanosecs % k_NANOSECS_PER_SEC);
    }
    else {
        d_nanoseconds = static_cast<int>(nanosecs);
    }

    if (d_seconds > 0 && d_nanoseconds < 0) {
        --d_seconds;
        d_nanoseconds += k_NANOSECS_PER_SEC;
    }
    else if (d_seconds < 0 && d_nanoseconds > 0) {
        ++d_seconds;
        d_nanoseconds -= k_NANOSECS_PER_SEC;
    }
    return *this;
}

// ACCESSORS
std::ostream& TimeInterval::print(std::ostream& stream,
                                  int           level,
                                  int           spacesPerLevel) const
{
    if (level > 0 && spacesPerLevel != 0) {
        // If 'level <= 0' the value will not be indented, otherwise the
        // indentation is 'level * abs(spacesPerLevel)'.

        unsigned absSpacesPerLevel = spacesPerLevel;

        if (spacesPerLevel < 0) {
            absSpacesPerLevel = -spacesPerLevel;
        }

        // Use nested loops rather than multiplication to avoid gcc warning.
        for (unsigned i = 0; i < absSpacesPerLevel; ++i) {
            for (int j = 0; j < level; ++j) {
                stream << ' ';
            }
        }
    }

    enum { k_BUFFER_SIZE = 64 };
        // The buffer must have enough space to fit the maximum output which is
        // four fixed characters "(, )" plus max ten for the nanoseconds plus
        // 19 characters for LLONG_MAX plus two for the sign plus one for
        // terminating NULL.  Sums up to 4 + 10 + 19 + 2 + 1 = 36.

    char buffer[k_BUFFER_SIZE] = { 0 };

    int rc = snprintf(buffer,
                      k_BUFFER_SIZE,
                      "(%lld, %d)",
                      d_seconds,
                      d_nanoseconds);
    (void) rc;
    BSLS_ASSERT(0 <= rc);

    stream << buffer;

    // We suppress the trailing end-of-line if 'spacesPerLevel < 0'.

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }
    return stream;
}

}  // close package namespace

std::ostream& bsls::operator<<(std::ostream&             stream,
                               const bsls::TimeInterval& timeInterval)
{
    return timeInterval.print(stream, 0, -1);
}

}  // close enterprise namespace

// BDE_VERIFY pragma: pop

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
