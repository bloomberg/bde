// bslim_fuzzutil.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLIM_FUZZUTIL
#define INCLUDED_BSLIM_FUZZUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide fuzz test utilities for basic types.
//
//@CLASSES:
// bslim::FuzzUtil: functions to create basic types from fuzz data
//
//@SEE_ALSO: bslim_fuzzdataview
//
//@DESCRIPTION: This component provides a namespace, 'bslim::FuzzUtil',
// containing functions that create fundamental and standard library types from
// fuzz data provided by a fuzz harness (e.g., 'libFuzzer').
//
// See {http://bburl/BDEFuzzTesting} for details on how to build and run with
// fuzz testing enabled.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Consuming Integers in a Range to Pass to an Interface
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we wish to fuzz test a function with preconditions.
//
// First, we define the 'TradingInterfaceUnderTest' 'struct':
//..
//  struct TradingInterfaceUnderTest {
//      // This utility class provides sample functionality to demonstrate how
//      // fuzz data might be used.
//
//      // CLASS METHODS
//      static int numEarningsAnnouncements(int year, int month)
//          // Return a value containing the number of earnings announcements
//          // in the specified 'year' and 'month'.  The behavior is undefined
//          // unless '1950 < year < 2030' and 'month' is in '[1 .. 12]'.  Note
//          // that the values here are arbitrary, and in the real-world this
//          // data would be obtained from a database or an API.
//      {
//          BSLS_ASSERT(1950 <  year  && year  < 2030);
//          BSLS_ASSERT(   1 <= month && month <=  12);
//
//          if (2020 < year && 6 < month) {
//              return 11;                                            // RETURN
//          }
//          return 6;
//      }
//  };
//..
// Then, we need a block of raw bytes.  This would normally come from a fuzz
// harness (e.g., the 'LLVMFuzzerTestOneInput' entry point function from
// 'libFuzzer').  Since 'libFuzzer' is not available here, we initialize a
// 'myFuzzData' array that we will use instead.
//..
//  const bsl::uint8_t  myFuzzData[] = {0x43, 0x19, 0x0D, 0x44, 0x37, 0x0D,
//                                      0x38, 0x5E, 0x9B, 0xAA, 0xF3, 0xDA};
//..
// Next, we create a 'FuzzDataView' to wrap the raw bytes.
//..
//  bslim::FuzzDataView fdv(myFuzzData, sizeof myFuzzData);
//..
// Now, we pass this 'FuzzDataView' to 'FuzzUtil' to generate values within the
// permissible range of the function under test:
//..
//  int month = bslim::FuzzUtil::consumeNumberInRange<int>(&fdv,    1,   12);
//  int year  = bslim::FuzzUtil::consumeNumberInRange<int>(&fdv, 1951, 2029);
//  assert(   1 <= month && month <=   12);
//  assert(1951 <= year  && year  <= 2029);
//..
// Finally, we can use these 'int' values to pass to a function that returns
// the number of earnings announcements scheduled in a given month.
//..
//  int numEarnings =
//      TradingInterfaceUnderTest::numEarningsAnnouncements(year, month);
//  (void) numEarnings;
//..

#include <bslscm_version.h>

#include <bslim_fuzzdataview.h>

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_libraryfeatures.h>
#include <bsls_types.h>           // 'bsls::Types::Uint64'

#include <bsl_cmath.h>            // 'bsl::isfinite'
#include <bsl_cstdint.h>          // 'bsl::uint8_t'
#include <bsl_limits.h>           // 'bsl::numeric_limits'
#include <bsl_string.h>
#include <bsl_type_traits.h>      // 'bsl::is_same'
#include <bsl_vector.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
#include <memory_resource>
#endif
#include <string>
#include <vector>

namespace BloombergLP {
namespace bslim {

                              // ===============
                              // struct FuzzUtil
                              // ===============

struct FuzzUtil {
    // This utility 'struct' provides a namespace for a suite of functions
    // operating on objects of type 'FuzzDataView'and providing the consumption
    // of fuzz data bytes into fundamental and standard library types.

    // CLASS METHODS
    static bool consumeBool(FuzzDataView *fuzzDataView);
        // Return a 'bool' value based upon consuming a single byte from the
        // specified 'fuzzDataView'.  If 'fuzzDataView->length()' is 0, return
        // 'false'.

    template <class TYPE>
    static typename bsl::enable_if<bsl::is_integral<TYPE>::value, TYPE>::type
    consumeNumber(FuzzDataView *fuzzDataView);

    template <class TYPE>
    static typename
    bsl::enable_if<bsl::is_floating_point<TYPE>::value, TYPE>::type
    consumeNumber(FuzzDataView *fuzzDataView);
        // Return a value of (template parameter) 'TYPE' in the range
        // [min .. max] -- where 'min' and 'max' are the minimum and maximum
        // values representable by the 'TYPE' -- based on at most the next
        // 'sizeof(TYPE) + 1' bytes from the specified 'fuzzDataView', and
        // update 'fuzzDataView' to reflect the bytes consumed.  If
        // '0 == fuzzDataView->length()', return the minimum value of 'TYPE'.
        // This function does not participate in overload resolution unless
        // either 'bsl::is_integral<TYPE>::value' or
        // 'bsl::is_floating_point<TYPE>::value' is 'true'.  The behavior is
        // undefined if 'bsl::is_same<TYPE, bool>::value' or
        // 'bsl::is_same<TYPE, long double>' is 'true'.

    template <class TYPE>
    static typename
    bsl::enable_if<bsl::is_integral<TYPE>::value, TYPE>::type
    consumeNumberInRange(FuzzDataView                *fuzzDataView,
                         TYPE                         min,
                         TYPE                         max);
    template <class TYPE>
    static typename
    bsl::enable_if<bsl::is_floating_point<TYPE>::value, TYPE>::type
    consumeNumberInRange(FuzzDataView *fuzzDataView, TYPE min, TYPE max);
        // Return a value of (template parameter) 'TYPE' in the specified range
        // [min .. max] based on at most the next 'sizeof(TYPE) + 1' bytes from
        // the specified 'fuzzDataView', and update 'fuzzDataView' to reflect
        // the bytes consumed.  If '0 == fuzzDataView->length()', return the
        // specified 'min'.  This function does not participate in overload
        // resolution unless either 'bsl::is_integral<TYPE>::value' or
        // 'bsl::is_floating_point<TYPE>::value' is 'true'.  The behavior is
        // undefined if 'min > max', 'min' or 'max' is not finite, or either
        // 'bsl::is_same<TYPE, bool>::value' or
        // 'bsl::is_same<TYPE, long double>' is 'true'.

    static void consumeRandomLengthChars(bsl::vector<char> *output,
                                         FuzzDataView      *fuzzDataView,
                                         bsl::size_t        maxLength);
    static void consumeRandomLengthChars(std::vector<char> *output,
                                         FuzzDataView      *fuzzDataView,
                                         bsl::size_t        maxLength);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    static void consumeRandomLengthChars(std::pmr::vector<char> *output,
                                         FuzzDataView           *fuzzDataView,
                                         bsl::size_t             maxLength);
#endif
    // Load into the specified 'output' a sequence of characters of length from
    // 0 to the specified 'maxLength'.  If the specified 'fuzzDataView' has
    // fewer bytes than 'maxLength', load at most 'fuzzDataView->length()'
    // bytes into 'output'.  If the buffer in 'fuzzDataView' contains two
    // successive backslash characters, then in 'output' they will be converted
    // to a single backslash ('\\') character; if a single backslash character
    // is encountered, the consumption of bytes is terminated.  Note that
    // because double backslashes are mapped to single backslashes, more than
    // 'maxLength' bytes may be consumed from the buffer to produce the
    // 'output'.  Also note that the purpose of this function is to enable the
    // creation of a non-zero-terminated 'string_view', which is not possible
    // with the 'string' counterpart.

    static void consumeRandomLengthString(bsl::string      *output,
                                          FuzzDataView     *fuzzDataView,
                                          bsl::size_t       maxLength);
    static void consumeRandomLengthString(std::string      *output,
                                          FuzzDataView     *fuzzDataView,
                                          bsl::size_t       maxLength);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    static void consumeRandomLengthString(std::pmr::string *output,
                                          FuzzDataView     *fuzzDataView,
                                          bsl::size_t       maxLength);
#endif
        // Load into the specified 'output' a string of length from 0 to the
        // specified 'maxLength'.  If the specified 'fuzzDataView' has fewer
        // bytes than 'maxLength', load at most 'fuzzDataView->length()' bytes
        // into 'output'.  If the buffer in 'fuzzDataView' contains two
        // successive backslash characters, then in 'output' they will be
        // converted to a single backslash ('\\') character; if a single
        // backslash character is encountered, the consumption of bytes is
        // terminated.  Note that because double backslashes are mapped to
        // single backslashes, more than 'maxLength' bytes may be consumed
        // from the buffer to produce the 'output'.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                              // ---------------
                              // struct FuzzUtil
                              // ---------------

// CLASS METHODS
inline
bool FuzzUtil::consumeBool(FuzzDataView *fuzzDataView)
{
    return 1 & consumeNumber<bsl::uint8_t>(fuzzDataView);
}

template <class TYPE>
typename bsl::enable_if<bsl::is_integral<TYPE>::value, TYPE>::type
FuzzUtil::consumeNumber(FuzzDataView *fuzzDataView)
{
    return consumeNumberInRange(fuzzDataView,
                                bsl::numeric_limits<TYPE>::min(),
                                bsl::numeric_limits<TYPE>::max());
}

template <class TYPE>
typename
bsl::enable_if<bsl::is_floating_point<TYPE>::value, TYPE>::type
FuzzUtil::consumeNumber(FuzzDataView *fuzzDataView)
{
    return consumeNumberInRange(fuzzDataView,
                                -bsl::numeric_limits<TYPE>::max(),
                                bsl::numeric_limits<TYPE>::max());
}

template <class TYPE>
typename bsl::enable_if<bsl::is_integral<TYPE>::value, TYPE>::type
FuzzUtil::consumeNumberInRange(FuzzDataView *fuzzDataView, TYPE min, TYPE max)
{
    BSLMF_ASSERT(bsl::is_integral<TYPE>::value);
    BSLMF_ASSERT((!bsl::is_same<TYPE, bool>::value));
    BSLMF_ASSERT(sizeof(TYPE) <= sizeof(bsls::Types::Uint64));
    BSLS_ASSERT(min <= max);

    bsls::Types::Uint64 range = static_cast<bsls::Types::Uint64>(max) - min;

    int numBytes = 0;

    for (bsls::Types::Uint64 rangeCpy = range; 0 != rangeCpy;
         rangeCpy >>= 8, ++numBytes) {
    }

    bsls::Types::Uint64 addend = 0;

    FuzzDataView prefix = fuzzDataView->removePrefix(numBytes);

    for (const bsl::uint8_t *it = prefix.begin(); it != prefix.end(); it++) {
        addend = (addend << 8) | *it;
    }

    if (bsl::numeric_limits<bsls::Types::Uint64>::max() != range) {
        addend %= (range + 1);
    }

    return static_cast<TYPE>(min + addend);
}

template <class TYPE>
typename bsl::enable_if<bsl::is_floating_point<TYPE>::value, TYPE>::type
FuzzUtil::consumeNumberInRange(FuzzDataView *fuzzDataView, TYPE min, TYPE max)
{
    BSLMF_ASSERT((!bsl::is_same<TYPE, long double>::value));
    BSLMF_ASSERT(bsl::numeric_limits<TYPE>::has_infinity);

    BSLS_ASSERT(min <= max);

    BSLS_ASSERT(min == min && max == max);
    BSLS_ASSERT(bsl::numeric_limits<TYPE>::infinity() != max &&
                -bsl::numeric_limits<TYPE>::infinity() != min);

    TYPE       addend = min;
    TYPE       range  = 0;
    const TYPE k_HALF = 0.5;

    if (max > min + bsl::numeric_limits<TYPE>::max()) {
        range = max * k_HALF - min * k_HALF;
        if (consumeBool(fuzzDataView)) {
            addend = min + range;
        }
    }
    else {
        range = max - min;
    }

    typedef typename bsl::conditional<(sizeof(TYPE) <= sizeof(bsl::uint32_t)),
                                      bsl::uint32_t,
                                      bsls::Types::Uint64>::type IntegralType;

    TYPE factor =
        static_cast<TYPE>(consumeNumber<IntegralType>(fuzzDataView)) /
        static_cast<TYPE>(
            bsl::numeric_limits<IntegralType>::max());  // between 0-1

    return addend + range * factor;
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
