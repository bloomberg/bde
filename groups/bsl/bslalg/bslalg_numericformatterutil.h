// bslalg_numericformatterutil.h                                      -*-C++-*-
#ifndef INCLUDED_BSLALG_NUMERICFORMATTERUTIL
#define INCLUDED_BSLALG_NUMERICFORMATTERUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a utility for formatting numbers into strings.
//
//@CLASSES:
//  bslalg::NumericFormatterUtil: namespace for 'toChars' and support functions
//
//@DESCRIPTION: This component, 'bslalg_numericformatterutil' provides a
// namespace 'struct', 'bslalg::NumericFormatterUtil', containing the template
// function 'toChars', that translates integral fundamental types into ASCII
// strings.
//
///Usage
///-----
// In this section we show the intended use of this component.
//
///Example 1: Demonstrating Writing a Number to a 'streambuf'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to define a function that writes an 'int' to a 'streambuf'.
// We can use 'bsl::to_chars' to write the 'int' to a buffer, then write the
// buffer to the 'streambuf'.
//
// First, we declare our function:
//..
//  void writeJsonScalar(std::streambuf *result, int value)
//      // Write the specified 'value', in decimal, to the specified 'result'.
//  {
//..
// Then, we declare a buffer long enough to store any 'int' value in decimal.
//..
//      char buffer[11];        // size large enough to write 'INT_MIN', the
//                              // worst-case value, in decimal.
//..
// Next, we call the function:
//..
//      char *ret = bslalg::NumericFormatterUtil::toChars(buffer,
//                                                        buffer + 11,
//                                                        value);
//..
// Then, we check that the buffer was long enough, which should always be the
// case:
//..
//      assert(0 != ret);
//..
// Now, we check that 'ret' is in the range
// '[ buffer + 1, buffer + sizeof(buffer) ]', which will always be the case if
// 'toChars' succeeded.
//..
//      assert(buffer <  ret);
//      assert(ret    <= buffer + sizeof(buffer));
//..
// Finally, we write our buffer to the 'streambuf':
//..
//      result->sputn(buffer, ret - buffer);
//  }
//..

#include <bslscm_version.h>

#include <bslmf_assert.h>
#include <bslmf_conditional.h>
#include <bslmf_isintegral.h>
#include <bslmf_issame.h>
#include <bslmf_removecv.h>
#include <bsls_assert.h>
#include <bsls_keyword.h>
#include <bsls_types.h>

namespace BloombergLP {
namespace bslalg {

                          // ==========================
                          // class NumericFormatterUtil
                          // ==========================

class NumericFormatterUtil {
    // Namespace 'class' for free functions supporting 'to_chars'.

    // PRIVATE CLASS METHODS
    static char *toCharsImpl(char     *first,
                             char     *last,
                             unsigned  value,
                             int       base) BSLS_KEYWORD_NOEXCEPT;
        // Write the specified 'value' into the character buffer starting a the
        // specified 'first' and ending at the specified 'last', rendering the
        // value in the specified base 'base'.  On success, return a the
        // address one past the lowest order digit written, on failure, return
        // 0.  The only reason for failure is if the range '[ first, last )' is
        // not large enough to contain the result.  The written result is to
        // begin at 'first' with leftover room following the return value.  The
        // behavior is undefined unless 'first <= last' and 'base' is in the
        // range '[ 2 .. 36 ]'.

    static char *toCharsImpl(char                *first,
                             char                *last,
                             bsls::Types::Uint64  value,
                             int                  base) BSLS_KEYWORD_NOEXCEPT;
        // Write the specified 'value' into the character buffer starting a the
        // specified 'first' and ending at the specified 'last', ing the value
        // in the specified base 'base'.  On success, return a the address one
        // past the lowest order digit written, on failure, return 0.  The only
        // reason for failure is if the range '[ first, last )' is not large
        // enough to contain the result.  The written result is to begin at
        // 'first' with leftover room following the return value.  The behavior
        // is undefined unless 'first <= last' and 'base' is in the range
        // '[ 2 .. 36 ]'.

  public:
    // PUBLIC CLASS METHOD
    template <class TYPE>
    static char *toChars(char *first,
                         char *last,
                         TYPE  value,
                         int   base = 10) BSLS_KEYWORD_NOEXCEPT;
        // Write the specified 'value' into the character buffer starting a the
        // specified 'first' and ending at the specified 'last'.  Optionally
        // specify 'base', the base in which to render the number.  If 'base'
        // is not specified, base 10 is used.  Return the address one past the
        // lowest order digit written on success, or 0 on failure.  The only
        // reason for failure is if the range '[ first, last )' is not large
        // enough to contain the result.  The written result is to begin at
        // 'first' with leftover room following the return value.  The behavior
        // is undefined unless 'first < last' and 'base' is in the range
        // '[ 2 .. 36 ]'.
};

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // -----------------------------
                        // struct 'NumericFormatterUtil'
                        // -----------------------------

// PUBLIC CLASS METHOD
template <class TYPE>
inline
char *NumericFormatterUtil::toChars(char *first,
                                    char *last,
                                    TYPE  value,
                                    int   base) BSLS_KEYWORD_NOEXCEPT
{
    BSLS_ASSERT_SAFE(2 <= base);
    BSLS_ASSERT_SAFE(base <= 36);
    BSLS_ASSERT_SAFE(first < last);

    BSLMF_ASSERT(bsl::is_integral<TYPE>::value);
    BSLMF_ASSERT(!(bsl::is_same<typename bsl::remove_cv<TYPE>::type,
                                bool>::value));
    BSLMF_ASSERT(sizeof(TYPE) <= sizeof(bsls::Types::Uint64));

    typedef typename bsl::conditional<(sizeof(unsigned) < sizeof(TYPE)),
                                      bsls::Types::Uint64,
                                      unsigned>::type VirtualUnsignedType;

    // Note that if 'value' is a negative value and 'TYPE' is smaller than
    // 'VirtualUnsignedType', assigning it here will extend the sign, even
    // though 'VirtualUnsignedType' is an unsigned type.

    VirtualUnsignedType uValue = value;

    if (value < 0) {
        uValue = ~uValue + 1;   // Absolute value -- note this works even for
                                // 'numeric_limits<TYPE>::min()'.
        *first++ = '-';
    }

    return toCharsImpl(first, last, uValue, base);
}

}  // close package namespace
}  // close enterprise namespace

#endif

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
