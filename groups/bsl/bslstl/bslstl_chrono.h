// bslstl_chrono.h                                                    -*-C++-*-
#ifndef INCLUDED_BSLSTL_CHRONO
#define INCLUDED_BSLSTL_CHRONO

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@CANONICAL_HEADER: bsl_chrono.h
//
//@DESCRIPTION: This component is for internal use only. Please include
// '<bsl_chrono.h>' directly.  This component imports symbols declared in the
// <chrono> header file implemented in the standard library provided by the
// compiler vendor.
//
///User-defined literals
///---------------------
// This component provides a set of user-defined literals (UDL) to form
// 'bsl::chrono::duration' objects with various duration periods such as hours,
// minutes, seconds, milliseconds, microseconds and nanoseconds.  The
// ud-suffixes are preceded with the '_' symbol to distinguish between the
// 'bsl'-chrono's UDLs and the 'std'-chrono's UDLs introduced in the C++14
// standard and implemented in the standard library.  Note that 'bsl'-chrono's
// UDLs, unlike the 'std'-chrono's UDLs, can be used in a client's code if the
// current compiler supports the C++11 standard.
//
// Also note that 'bsl'-chrono's UDL operators are declared in the
// 'bsl::literals::chrono_literals' namespace, where 'literals' and
// 'chrono_literals' are inline namespaces.  Access to these operators can be
// gained with either 'using namespace bsl::literals',
// 'using namespace bsl::chrono_literals' or
// 'using namespace bsl::literals::chrono_literals'.  But we recommend
// 'using namespace bsl::chrono_literals' to minimize the scope of the using
// declaration.
//
///C++20 Calendar/TZ feature on Windows
///------------------------------------
// This feature has been provided by MSVC++ compiler since VS 2019 16.10.  But
// that release was shipped with the following important note:
//
// "While the STL generally provides all features on all supported versions of
// Windows, leap seconds and time zones (which change over time) require OS
// support that was added to Windows 10.  Specifically, updating the leap
// second database requires Windows 10 version 1809 or later, and time zones
// require 'icu.dll' which is provided by Windows 10 version 1903/19H1 or
// later.  This applies to both client and server OSes; note that Windows
// Server 2019 is based on Windows 10 version 1809."
//
// If the feature is used on a host that doesn't provide 'icu.dll', an
// exception with "The specified module could not be found." message will be
// thrown.  That is why this feature is disabled by default on Windows.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Basic 'bsl'-chrono's UDLs Usage
/// - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates basic use of user-defined literal operators.
//
// First, we provide an access to 'bsl'-chrono's UDLs.
//..
//  using namespace bsl::chrono_literals;
//..
// Then, we construct two duration objects that represent a 24-hours and a half
// an hour time intervals using 'operator "" _h'.
//..
//  auto hours_in_a_day = 24_h;
//  auto halfhour       = 0.5_h;
//..
// Finally, stream the two objects to 'stdout':
//..
//  printf("one day is %ld hours\n", hours_in_a_day.count());
//  printf("half an hour is %.1f hours\n",
//         static_cast<double>(halfhour.count()));
//..
// The streaming operator produces output in the following format on 'stdout':
//..
//  one day is 24 hours
//  half an hour is 0.5 hours
//..

#include <bslscm_version.h>

#include <bslstl_ratio.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>
#include <bsls_platform.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

#include <chrono>

namespace bsl {

    namespace chrono {
        using std::chrono::duration;
        using std::chrono::time_point;
        using std::chrono::system_clock;
        using std::chrono::steady_clock;
        using std::chrono::high_resolution_clock;
        using std::chrono::treat_as_floating_point;
        using std::chrono::duration_values;
        using std::chrono::duration_cast;
        using std::chrono::time_point_cast;
        using std::chrono::hours;
        using std::chrono::minutes;
        using std::chrono::seconds;
        using std::chrono::milliseconds;
        using std::chrono::microseconds;
        using std::chrono::nanoseconds;

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY)
        template <class TYPE>
        constexpr bool treat_as_floating_point_v =
                      std::chrono::treat_as_floating_point<TYPE>::value;
            // This template variable represents the result value of the
            // 'std::chrono::treat_as_floating_point' meta-function.
#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY)
        using std::chrono::abs;
        using std::chrono::ceil;
        using std::chrono::floor;
        using std::chrono::round;
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CALENDAR
        // Calendar and Time Zones
        using std::chrono::days;
        using std::chrono::weeks;
        using std::chrono::months;
        using std::chrono::years;

        using std::chrono::is_clock;
        using std::chrono::is_clock_v;
        //using std::chrono::system_clock;
            using std::chrono::sys_time;
            using std::chrono::sys_seconds;
            using std::chrono::sys_days;
        using std::chrono::file_clock;
            using std::chrono::file_time;
        using std::chrono::local_t;
            using std::chrono::local_time;
            using std::chrono::local_seconds;
            using std::chrono::local_days;

        using std::chrono::clock_time_conversion;
        using std::chrono::clock_cast;

        using std::chrono::last_spec;
        using std::chrono::last;
        using std::chrono::day;
        using std::chrono::month;
            using std::chrono::January;
            using std::chrono::February;
            using std::chrono::March;
            using std::chrono::April;
            using std::chrono::May;
            using std::chrono::June;
            using std::chrono::July;
            using std::chrono::August;
            using std::chrono::September;
            using std::chrono::October;
            using std::chrono::November;
            using std::chrono::December;
        using std::chrono::year;
        using std::chrono::weekday;
            using std::chrono::Sunday;
            using std::chrono::Monday;
            using std::chrono::Tuesday;
            using std::chrono::Wednesday;
            using std::chrono::Thursday;
            using std::chrono::Friday;
            using std::chrono::Saturday;
        using std::chrono::weekday_indexed;
        using std::chrono::weekday_last;

        using std::chrono::month_day;
        using std::chrono::month_day_last;
        using std::chrono::month_weekday;
        using std::chrono::month_weekday_last;
        using std::chrono::year_month;
        using std::chrono::year_month_day;
        using std::chrono::year_month_day_last;
        using std::chrono::year_month_weekday;
        using std::chrono::year_month_weekday_last;

        using std::chrono::hh_mm_ss;

        using std::chrono::is_am;
        using std::chrono::is_pm;
        using std::chrono::make12;
        using std::chrono::make24;

#ifndef BSLS_PLATFORM_OS_WINDOWS
        using std::chrono::utc_clock;
            using std::chrono::utc_time;
            using std::chrono::utc_seconds;
        using std::chrono::tai_clock;
            using std::chrono::tai_time;
            using std::chrono::tai_seconds;
        using std::chrono::gps_clock;
            using std::chrono::gps_time;
            using std::chrono::gps_seconds;

        using std::chrono::tzdb;
        using std::chrono::tzdb_list;
        using std::chrono::sys_info;
        using std::chrono::local_info;
        using std::chrono::get_tzdb_list;
        using std::chrono::get_tzdb;
        using std::chrono::remote_version;
        using std::chrono::reload_tzdb;

        using std::chrono::time_zone;
        using std::chrono::choose;
        using std::chrono::zoned_traits;
        using std::chrono::zoned_time;
        using std::chrono::zoned_seconds;
        using std::chrono::time_zone_link;

        using std::chrono::nonexistent_local_time;
        using std::chrono::ambiguous_local_time;

        using std::chrono::leap_second;
        using std::chrono::leap_second_info;
        using std::chrono::get_leap_second_info;
#endif

        using std::chrono::from_stream;
        using std::chrono::parse;
#endif
    }  // close namespace chrono

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
inline namespace literals {
inline namespace chrono_literals {
    using namespace std::literals::chrono_literals;
}  // close chrono_literals namespace
}  // close literals namespace
#endif

#if defined (BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY) && \
    defined (BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE)

inline namespace literals {
inline namespace chrono_literals {

    BSLS_KEYWORD_CONSTEXPR
    bsl::chrono::hours operator "" _h(unsigned long long hrs);
    BSLS_KEYWORD_CONSTEXPR
    bsl::chrono::duration<long double, bsl::ratio<3600, 1>> operator "" _h(
                                                              long double hrs);
       // Create a 'bsl::chrono::duration' object having the
       // 'bsl::ratio<3600, 1>' duration period and initialized with the
       // specified 'hrs' number of hours.

    BSLS_KEYWORD_CONSTEXPR
    bsl::chrono::minutes operator "" _min(unsigned long long mins);
    BSLS_KEYWORD_CONSTEXPR
    bsl::chrono::duration<long double, bsl::ratio<60, 1>> operator "" _min(
                                                             long double mins);
       // Create a 'bsl::chrono::duration' object having the
       // 'bsl::ratio<60, 1>' duration period and initialized with the
       // specified 'mins' number of minutes.

    BSLS_KEYWORD_CONSTEXPR
    bsl::chrono::seconds operator "" _s(unsigned long long secs);
    BSLS_KEYWORD_CONSTEXPR
    bsl::chrono::duration<long double> operator "" _s(long double secs);
        // Create a 'bsl::chrono::duration' object having the 'bsl::ratio<1>'
        // duration period and initialized with the specified 'secs' number of
        // seconds.

    BSLS_KEYWORD_CONSTEXPR
    bsl::chrono::milliseconds operator "" _ms(unsigned long long ms);
    BSLS_KEYWORD_CONSTEXPR
    bsl::chrono::duration<long double, bsl::milli> operator "" _ms(
                                                               long double ms);
       // Create a 'bsl::chrono::duration' object having the
       // 'bsl::milli' duration period and initialized with the specified 'ms'
       // number of milliseconds.

    BSLS_KEYWORD_CONSTEXPR
    bsl::chrono::microseconds operator "" _us(unsigned long long us);
    BSLS_KEYWORD_CONSTEXPR
    bsl::chrono::duration<long double, bsl::micro> operator "" _us(
                                                               long double us);
       // Create a 'bsl::chrono::duration' object having the
       // 'bsl::micro' duration period and initialized with the specified 'us'
       // number of microseconds.

    BSLS_KEYWORD_CONSTEXPR
    bsl::chrono::nanoseconds operator "" _ns(unsigned long long ns);
    BSLS_KEYWORD_CONSTEXPR
    bsl::chrono::duration<long double, bsl::nano> operator "" _ns(
                                                               long double ns);
       // Create a 'bsl::chrono::duration' object having the 'bsl::nano'
       // duration period and initialized with the specified 'ns' number of
       // microseconds.

}  // close chrono_literals namespace
}  // close literals namespace
#endif

// ============================================================================
//                   INLINE AND TEMPLATE FUNCTION DEFINITIONS
// ============================================================================

#if defined (BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY) && \
    defined (BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE)

inline namespace literals {
inline namespace chrono_literals {
    inline BSLS_KEYWORD_CONSTEXPR
    bsl::chrono::hours operator "" _h(unsigned long long hrs)
    {
        return bsl::chrono::hours(hrs);
    }

    inline BSLS_KEYWORD_CONSTEXPR
    bsl::chrono::duration<long double, bsl::ratio<3600, 1>> operator "" _h(
                                                               long double hrs)
    {
        return bsl::chrono::duration<long double, bsl::ratio<3600, 1>>(hrs);
    }

    inline BSLS_KEYWORD_CONSTEXPR
    bsl::chrono::minutes operator "" _min(unsigned long long mins)
    {
        return bsl::chrono::minutes(mins);
    }

    inline BSLS_KEYWORD_CONSTEXPR
    bsl::chrono::duration<long double, bsl::ratio<60, 1>>
        operator "" _min(long double mins)
    {
        return bsl::chrono::duration<long double, bsl::ratio<60, 1>>(mins);
    }

    inline BSLS_KEYWORD_CONSTEXPR
    bsl::chrono::seconds operator "" _s(unsigned long long secs)
    {
        return bsl::chrono::seconds(secs);
    }

    inline BSLS_KEYWORD_CONSTEXPR
    bsl::chrono::duration<long double> operator "" _s(long double secs)
    {
        return bsl::chrono::duration<long double>(secs);
    }

    inline BSLS_KEYWORD_CONSTEXPR
    bsl::chrono::milliseconds operator "" _ms(unsigned long long ms)
    {
        return bsl::chrono::milliseconds(ms);
    }

    inline BSLS_KEYWORD_CONSTEXPR
    bsl::chrono::duration<long double, bsl::milli> operator "" _ms(
                                                                long double ms)
    {
        return bsl::chrono::duration<long double, bsl::milli>(ms);
    }

    inline BSLS_KEYWORD_CONSTEXPR
    bsl::chrono::microseconds operator "" _us(unsigned long long us)
    {
        return bsl::chrono::microseconds(us);
    }

    inline BSLS_KEYWORD_CONSTEXPR
    bsl::chrono::duration<long double, bsl::micro> operator "" _us(
                                                                long double us)
    {
        return bsl::chrono::duration<long double, bsl::micro>(us);
    }

    inline BSLS_KEYWORD_CONSTEXPR
    bsl::chrono::nanoseconds operator "" _ns(unsigned long long ns)
    {
        return bsl::chrono::nanoseconds(ns);
    }

    inline BSLS_KEYWORD_CONSTEXPR
    bsl::chrono::duration<long double, bsl::nano> operator "" _ns(
                                                                long double ns)
    {
        return bsl::chrono::duration<long double, bsl::nano>(ns);
    }

}  // close chrono_literals namespace
}  // close literals namespace
#endif
}  // close package namespace

#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
#endif  // INCLUDED_BSLSTL_CHRONO

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
