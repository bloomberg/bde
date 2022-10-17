// bdlt_fuzzutil.h                                                    -*-C++-*-
#ifndef INCLUDED_BDLT_FUZZUTIL
#define INCLUDED_BDLT_FUZZUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide creation of 'bdlt' data types from fuzz data.
//
//@CLASSES:
//   bdlt::FuzzUtil: functions to create 'bdlt' data types from fuzz data
//
//@SEE_ALSO: bslim_fuzzdataview, bslim_fuzzutil
//
//@DESCRIPTION: This component defines a struct, 'bdlt::FuzzUtil', which serves
// as a namespace for functions to create 'bdlt' data types from supplied fuzz
// input data.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Provide 'bdlt::Datetime' within a Range
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// The provided fuzz data is here represented by an array of bytes:
//..
//  const uint8_t data[] = {0x8A, 0x19, 0x0D, 0x44, 0x37, 0x0D,
//                          0x38, 0x5E, 0x9B, 0xAA, 0xF3, 0xDA};
//..
// First, we default construct a 'bslim::FuzzDataView' object, 'fdv':
//..
//  bslim::FuzzDataView fdv(data, sizeof(data));
//
//  assert(12 == fdv.length());
//..
// Next, we construct 'Date' objects to represent the 'begin' and 'end' of the
// time interval in which we wish to construct our new 'Date' from the fuzz
// data:
//..
//   bdlt::Date begin(1833, 5, 7);
//   bdlt::Date end(1897, 4, 3);
//..
// Finally, we create a 'Date' object, 'within', by employing 'bdlt_fuzzutil':
//..
//  bdlt::Date within = bdlt::FuzzUtil::consumeDateInRange(&fdv, begin, end);
//
//  assert(begin  <= within);
//  assert(within <= end);
//..

#include <bdlscm_version.h>

#include <bdlt_date.h>

#include <bslim_fuzzdataview.h>
#include <bslim_fuzzutil.h>

namespace BloombergLP {
namespace bdlt {

                            // ===============
                            // struct FuzzUtil
                            // ===============

struct FuzzUtil {
    // This utility 'struct' provides a namespace for a suite of functions that
    // produce objects of date and time values from fuzz data.

  private:
    // PRIVATE CLASS METHODS
    static Date firstValidDate();
        // Return the first valid date represented by 'Date' objects.

    static Date lastValidDate();
        // Return the last valid date represented by 'Date' objects.

  public:
    // CLASS METHODS
    static bdlt::Date consumeDate(bslim::FuzzDataView *fuzzDataView);
        // Return a 'Date' based on the next bytes from the specified
        // 'fuzzDataView', and update 'fuzzDataView' to reflect the bytes
        // consumed.

    static bdlt::Date consumeDateInRange(bslim::FuzzDataView *fuzzDataView,
                                         const bdlt::Date&    begin,
                                         const bdlt::Date&    end);
        // Return a 'Date' between the specified 'begin' and 'end' dates based
        // on the next bytes from the specified 'fuzzDataView', and update
        // 'fuzzDataView' to reflect the bytes consumed.  The behavior is
        // undefined unless 'begin <= end'.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                              // ---------------
                              // struct FuzzUtil
                              // ---------------

// PRIVATE CLASS METHODS
inline
Date FuzzUtil::firstValidDate()
{
    return Date();
}

inline
Date FuzzUtil::lastValidDate()
{
    return Date(9999, 12, 31);
}

// CLASS METHODS
inline
bdlt::Date FuzzUtil::consumeDate(bslim::FuzzDataView *fuzzDataView)
{
    return consumeDateInRange(fuzzDataView, firstValidDate(), lastValidDate());
}

inline
bdlt::Date FuzzUtil::consumeDateInRange(bslim::FuzzDataView *fuzzDataView,
                                        const bdlt::Date&    begin,
                                        const bdlt::Date&    end)
{
    int beginDays = begin - firstValidDate();
    int endDays   = end - firstValidDate();

    int dateBetween = bslim::FuzzUtil::consumeNumberInRange<int>(
        fuzzDataView, beginDays, endDays);

    return firstValidDate() + dateBetween;
}

}  // close package namespace
}  // close enterprise namespace

#endif

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
