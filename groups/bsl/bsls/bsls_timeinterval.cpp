// bsls_timeinterval.cpp                                              -*-C++-*-
#include <bsls_timeinterval.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bsls_timeinterval_cpp,"$Id$ $CSID$")

#include <bsls_asserttest.h>   // for testing only
#include <bsls_bsltestutil.h>  // for testing only
#include <bsls_platform.h>

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

// PRIVATE CLASS METHODS
bool TimeInterval::isSumValidInt64(bsls::Types::Int64 lhs,
                                   bsls::Types::Int64 rhs)
{
    if (lhs > 0 && (LLONG_MAX - lhs) < rhs) {
        return false;                                                 // RETURN
    }

    if (lhs < 0 && (LLONG_MIN - lhs) > rhs) {
        return false;                                                 // RETURN
    }
    return true;
}

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
             (static_cast<double>(d_seconds) - seconds) * k_NANOSECS_PER_SEC;
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
               (seconds - static_cast<double>(d_seconds)) * k_NANOSECS_PER_SEC;
        d_nanoseconds = static_cast<int>(nanoseconds + 0.5);
        if (d_nanoseconds == k_NANOSECS_PER_SEC) {
            ++d_seconds;
            d_nanoseconds = 0;
        }
    }
}

// MANIPULATORS
TimeInterval& TimeInterval::addSeconds(bsls::Types::Int64 seconds)
{
    BSLS_ASSERT_SAFE(isSumValidInt64(seconds, d_seconds));

    d_seconds += seconds;
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

void TimeInterval::addInterval(bsls::Types::Int64 seconds,
                               int                nanoseconds)
{
    BSLS_ASSERT(isSumValidInt64(d_seconds, seconds));
    BSLS_ASSERT(isSumValidInt64(
       d_seconds + seconds,
       (static_cast<bsls::Types::Int64>(d_nanoseconds) + nanoseconds) /
                                                          k_NANOSECS_PER_SEC));

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
}

void TimeInterval::setInterval(bsls::Types::Int64 seconds,
                               int                nanoseconds)
{
    BSLS_ASSERT(isValid(seconds, nanoseconds));

    d_seconds = seconds;
    if (nanoseconds >= k_NANOSECS_PER_SEC
     || nanoseconds <= -k_NANOSECS_PER_SEC) {
        d_seconds += nanoseconds / k_NANOSECS_PER_SEC;
        d_nanoseconds = static_cast<int>(nanoseconds % k_NANOSECS_PER_SEC);
    }
    else {
        d_nanoseconds = static_cast<int>(nanoseconds);
    }

    if (d_seconds > 0 && d_nanoseconds < 0) {
        --d_seconds;
        d_nanoseconds += k_NANOSECS_PER_SEC;
    }
    else if (d_seconds < 0 && d_nanoseconds > 0) {
        ++d_seconds;
        d_nanoseconds -= k_NANOSECS_PER_SEC;
    }

}

native_std::ostream& TimeInterval::print(
                                     native_std::ostream& stream,
                                     int                  level,
                                     int                  spacesPerLevel) const
{
    if (level > 0 && spacesPerLevel != 0) {
        // If 'level <= 0' the value will not be indented, otherwise the
        // indentation is 'level * abs(spacesPerLevel)'.

        // Use 'unsigned' to suppress gcc compiler warning.

        unsigned int indentation = level *
                      (spacesPerLevel < 0 ? -spacesPerLevel : spacesPerLevel);
        for (unsigned int i = 0; i < indentation; ++i) {
            stream << ' ';
        }
    }

    stream << '(' << d_seconds     << ", "
                  << d_nanoseconds << ')';

    // We suppress the trailing end-of-line if 'spacesPerLevel < 0'.

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }
    return stream;
}

}  // close package namespace

native_std::ostream& bsls::operator<<(native_std::ostream&      stream,
                                      const bsls::TimeInterval& timeInterval)
{
    return timeInterval.print(stream, 0, -1);
}

}  // close enterprise namespace

// BDE_VERIFY pragma: pop

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
