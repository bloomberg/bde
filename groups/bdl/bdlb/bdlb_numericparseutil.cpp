// bdlb_numericparseutil.cpp                                          -*-C++-*-
#include <bdlb_numericparseutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlb_numericparseutil_cpp, "$Id$ $CSID$")

#include <bdlb_chartype.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsls_platform.h>
#include <bslmf_assert.h>

#include <bsl_cstdlib.h>  // strtod
#include <bsl_clocale.h>  // setlocale

#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1900
// Needed for fixing the broken 'strtod', see below.
#include <bdlb_string.h>
#include <bdlb_chartype.h>
#include <bsl_limits.h>
#endif

namespace BloombergLP {

namespace bdlb {

namespace {

#if defined(BSLS_PLATFORM_OS_LINUX)
static
int doubleSign(double number)
    // Return 0 if the specified 'number' is positive, and a non-zero value
    // otherwise.  This function is needed only until we have all platforms
    // updated to C++11 that supports the 'signbit' function.
{
    const unsigned char *bytes = reinterpret_cast<unsigned char *>(&number);

#ifdef BSLS_PLATFORM_IS_BIG_ENDIAN
    static bsl::size_t pos = 0;
#elif defined(BSLS_PLATFORM_IS_LITTLE_ENDIAN)
    static bsl::size_t pos = sizeof(double) - 1;
#endif
    return bytes[pos] & 0x80;
}
#endif

}  // close unnamed namespace


#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1900
// 'strtod' is broken in Visual Studio up to (including) Visual Studio 2013.
// The function does not parse the special 'double' values NaN and Infinity as
// specified on http://en.cppreference.com/w/cpp/string/byte/strtof.  When
// Microsoft's 'strod' reports that it could not parse its input string the
// following functions are used to attempt to parse them as NaN or Infinity.

namespace {

static bool isPrefixCaseless(const bslstl::StringRef& inputString,
                             const bslstl::StringRef& prefix)
    // Check if the specified 'inputString' has the specified 'prefix' with
    // case insensitive comparison.  Return 'true' if the prefix matches and
    // 'false' otherwise.
{
    if (inputString.length() < prefix.length()) {
        return false;                                                 // RETURN
    }

    return String::areEqualCaseless(inputString.data(), prefix.length(),
                                    prefix.data(),      prefix.length());
}

static bool safeCheck(const bslstl::StringRef&     inputString,
                      bslstl::StringRef::size_type pos,
                      char                         ch)
    // Check in the specified 'inputString', if the characters at the specified
    // 'pos', if such position exists, equals to the specified 'ch'.
{
    return (inputString.length() > pos) && inputString[pos] == ch;
}

static bool isNanString(bslstl::StringRef        *remainder,
                        const bslstl::StringRef&  inputString)
    // Check the specified 'inputString' if its prefix is a "NaN" string
    // representation (see
    // http://en.cppreference.com/w/cpp/string/byte/strtof).  If it is, load
    // the rest of the 'inputString' into the specified 'remainder' and return
    // 'true'; otherwise do not change 'remainder' and return 'false'.
{

    if (!isPrefixCaseless(inputString, "NaN")) {
        return false;                                                 // RETURN
    }

    if (!safeCheck(inputString, 3, '(')) {
        remainder->assign(inputString.data() + 3, inputString.length() - 3);
        return true;                                                  // RETURN
    }

    typedef bslstl::StringRef::size_type size_type;
    for (size_type i = 4; i < inputString.length(); ++i) {
        const char ch = inputString[i];

        if (ch == ')') {
            ++i; // Include the ')'
            remainder->assign(inputString.data() + i,
                              inputString.length() - i);
            return true;                                              // RETURN
        }

        if (ch != '_' && !CharType::isAlnum(ch)) {
            break;
        }
    }

    return false;
}

static int parseNanAndInf(double            *result,
                          bslstl::StringRef *remainder,
                          bslstl::StringRef  inputString)
    // Check the specified 'inputString' if its prefix is a "NaN" or Infinity
    // string representation
    // (see http://en.cppreference.com/w/cpp/string/byte/strtof).  If it is,
    // load the result of the conversion into the specified 'result', load the
    // rest of the 'inputString' into the specified 'remainder' and return
    // zero; otherwise do not change 'remainder' and return a non-zero value.
{
    int sign = 1;

    if (inputString[0] == '+' || inputString[0] == '-') {
        if (inputString[0] == '-') {
            sign = -1;
        }
        if (inputString.size() == 1) {
            return -4;                                                // RETURN
        }
        inputString.assign(inputString.data() + 1, inputString.length() - 1);
    }

    if (isPrefixCaseless(inputString, "INFINITY")) {
        *result = sign * bsl::numeric_limits<double>::infinity();
        remainder->assign(inputString.data() + 8, inputString.length() - 8);
        return 0;                                                     // RETURN
    }

    if (isPrefixCaseless(inputString, "INF")) {
        *result = sign * bsl::numeric_limits<double>::infinity();
        remainder->assign(inputString.data() + 3, inputString.length() - 3);
        return 0;                                                     // RETURN
    }

    if (isNanString(remainder, inputString)) {
        // This is the only way of making a negative quiet NaN on Microsoft
        // Visual C++.  Multiplying with -1 does not work, it remains positive.
        // Assigning first positive then negating changes the value from QNAN
        // to something called IND.
        if (sign == -1) {
            *result = -bsl::numeric_limits<double>::quiet_NaN();
        }
        else {
            *result = bsl::numeric_limits<double>::quiet_NaN();
        }
        return 0;                                                     // RETURN
    }

    return -3;
}

}  // close unnamed namespace

#endif  // End of Microsoft Visual Studio 2013 and lower specific code

typedef bsls::Types::Int64  Int64;
typedef bsls::Types::Uint64 Uint64;

                          // -----------------------
                          // struct NumericParseUtil
                          // -----------------------
// CLASS METHODS
int NumericParseUtil::characterToDigit(char character, int base)
{
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(     base <= 36);

    BSLMF_ASSERT('Z' - 'A' == 25); // Verify our assumption that the letters
    BSLMF_ASSERT('z' - 'a' == 25); // are contiguous.

    int digit = CharType::isDigit(static_cast<unsigned char>(character))
                ? int(character - '0')
                : 'A' <= character && character <= 'Z'
                  ? int(character - 'A' + 10)
                  : 'a' <= character && character <= 'z'
                    ? int(character - 'a' + 10)
                    : -1;

    return digit < base ? digit : -1;
}

int NumericParseUtil::parseDouble(double                   *result,
                                  bslstl::StringRef        *remainder,
                                  const bslstl::StringRef&  inputString)
{
    BSLS_ASSERT(remainder);
    BSLS_ASSERT(result);

#ifdef BSLS_PLATFORM_OS_WINDOWS
    // 'setlocale' is slow on Windows, see '{drqs 161551330}'.
    BSLS_ASSERT_SAFE(bsl::setlocale(0, 0) == bslstl::StringRef("C"));
#else
    BSLS_ASSERT(bsl::setlocale(0, 0) == bslstl::StringRef("C"));
#endif

    // An empty string cannot be a number.

    if (inputString.empty()) {
        *remainder = inputString;
        return -1;                                                    // RETURN
    }

    // We need to ensure that the string does not start with a whitespace
    // because our contract says so, but our current implementation happens to
    // allow leading whitespace.

    if (CharType::isSpace(inputString[0])) {
        *remainder = inputString;
        return -2;                                                    // RETURN
    }

    static const size_type k_BUFFER_SIZE = 128;

    const bool             useLocalBuffer =
                                       (inputString.length() <= k_BUFFER_SIZE);

    char                   localBuffer[k_BUFFER_SIZE + 1];
    char                  *buffer = localBuffer;

    bslma::Allocator      *allocator = bslma::Default::defaultAllocator();

    if (!useLocalBuffer) {
        buffer = static_cast<char *>(allocator->allocate(
                                                    inputString.length() + 1));
    }

    memcpy(buffer, inputString.data(), inputString.length());
    buffer[inputString.length()] = 0;

    char         *endPtr;
    const double  rv = bsl::strtod(buffer, &endPtr);
    size_type endPos = (endPtr - buffer);
    remainder->assign(&inputString[0] + endPos, inputString.length() - endPos);

    if (!useLocalBuffer) {
        allocator->deallocate(buffer);
    }

    if (endPtr != buffer) {
        *result = rv;
#if defined(BSLS_PLATFORM_OS_LINUX)
        // 'strtod' is broken in libstdc++, parsing negative NaN into positive
        // NaN.  We are not checking for compilers because both clang and g++
        // exhibit the fault on Linux.
        if (*result != *result && inputString[0] == '-') {
            if (!doubleSign(*result)) {
                // If 'result' is (incorrectly) a positive NaN value, reverse
                // the sign
                *result = -*result;
            }
        }
#endif
        return 0;                                                     // RETURN
    }

#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1900
    // 'strtod' is broken in Visual Studio up to (including) Visual Studio
    // 2013.  The 'strtod' function does not parse the special 'double' values
    // NaN and Infinity as specified on
    // http://en.cppreference.com/w/cpp/string/byte/strtof.  We are here when
    // Microsoft's 'strod' reports that it could not parse the input string so
    // we call following function to attempt to parse them as NaN or Infinity.
    return parseNanAndInf(result, remainder, inputString);
#else
    return -3;
#endif
}

int NumericParseUtil::parseInt(int                      *result,
                               bslstl::StringRef        *remainder,
                               const bslstl::StringRef&  inputString,
                               int                       base)
{
    BSLS_ASSERT(remainder);
    BSLS_ASSERT(result);
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(     base <= 36);

    Int64 res = *result;
    int   rv = parseSignedInteger(&res,
                                  remainder,
                                  inputString,
                                  base,
                                  -static_cast<Int64>(0x80000000),
                                  static_cast<Int64>(0x7FFFFFFF));
    *result = static_cast<int>(res);
    return rv;
}

int NumericParseUtil::parseInt64(bsls::Types::Int64       *result,
                                 bslstl::StringRef        *remainder,
                                 const bslstl::StringRef&  inputString,
                                 int                       base)
{
    BSLS_ASSERT(remainder);
    BSLS_ASSERT(result);
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(     base <= 36);

    Int64 res = *result;
    int   rv = parseSignedInteger(
                                &res,
                                remainder,
                                inputString,
                                base,
                                -static_cast<Int64>(0x7FFFFFFFFFFFFFFFuLL) - 1,
                                static_cast<Int64>(0x7FFFFFFFFFFFFFFFuLL));
    *result = res;
    return rv;
}

int NumericParseUtil::parseUint(unsigned int             *result,
                                bslstl::StringRef        *remainder,
                                const bslstl::StringRef&  inputString,
                                int                       base)
{
    BSLS_ASSERT(remainder);
    BSLS_ASSERT(result);
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(     base <= 36);

    if (inputString.empty()) {
        *remainder = inputString;
        return -1;                                                    // RETURN
    }

    Uint64            res = *result;
    const bool        hasPlus = ('+' == inputString[0]);
    bslstl::StringRef sub(inputString.data() + hasPlus,
                          inputString.length() - hasPlus);
    int               rv = parseUnsignedInteger(
                                               &res,
                                               remainder,
                                               sub,
                                               base,
                                               static_cast<Int64>(0xFFFFFFFF));
    *result = static_cast<unsigned int>(res);
    return rv;
}

int NumericParseUtil::parseUint64(bsls::Types::Uint64      *result,
                                  bslstl::StringRef        *remainder,
                                  const bslstl::StringRef&  inputString,
                                  int                       base)
{
    BSLS_ASSERT(remainder);
    BSLS_ASSERT(result);
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(     base <= 36);

    if (inputString.empty()) {
        *remainder = inputString;
        return -1;                                                    // RETURN
    }

    Uint64            res = *result;
    const bool        hasPlus = ('+' == inputString[0]);
    bslstl::StringRef sub(inputString.data() + hasPlus,
                          inputString.length() - hasPlus);
    int               rv = parseUnsignedInteger(
                                    &res,
                                    remainder,
                                    sub,
                                    base,
                                    static_cast<Int64>(0xFFFFFFFFFFFFFFFFuLL));
    *result = res;
    return rv;
}

int NumericParseUtil::parseUshort(unsigned short           *result,
                                  bslstl::StringRef        *remainder,
                                  const bslstl::StringRef&  inputString,
                                  int                       base)
{
    BSLS_ASSERT(remainder);
    BSLS_ASSERT(result);
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(     base <= 36);

    if (inputString.empty()) {
        *remainder = inputString;
        return -1;                                                    // RETURN
    }

    Uint64            res = *result;
    const bool        hasPlus = ('+' == inputString[0]);
    bslstl::StringRef sub(inputString.data() + hasPlus,
                          inputString.length() - hasPlus);
    int               rv = parseUnsignedInteger(
                                               &res,
                                               remainder,
                                               sub,
                                               base,
                                               static_cast<Int64>(0xFFFF));
    *result = static_cast<unsigned short>(res);
    return rv;
}

int NumericParseUtil::parseShort(short                    *result,
                                 bslstl::StringRef        *remainder,
                                 const bslstl::StringRef&  inputString,
                                 int                       base)
{
    BSLS_ASSERT(remainder);
    BSLS_ASSERT(result);
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(     base <= 36);

    Int64 res = *result;
    int   rv = parseSignedInteger(&res,
                                  remainder,
                                  inputString,
                                  base,
                                  -static_cast<Int64>(32768),
                                  static_cast<Int64>(32767));
    *result = static_cast<short>(res);
    return rv;
}

int NumericParseUtil::parseSignedInteger(bsls::Types::Int64       *result,
                                         bslstl::StringRef        *remainder,
                                         const bslstl::StringRef&  inputString,
                                         int                       base,
                                         const bsls::Types::Int64  minValue,
                                         const bsls::Types::Int64  maxValue)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(remainder);
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(     base <= 36);
    BSLS_ASSERT(minValue <= 0);
    BSLS_ASSERT(maxValue >= 0);

    if (0 == inputString.length()) {
        // ERROR: The number must have at least one digit.
        *remainder = inputString;
        return -1;                                                    // RETURN
    }

    typedef bsls::Types::Uint64 Uint64;

    Uint64 res = *result;
    int    rv;

    if ('-' == inputString[0]) {
        bslstl::StringRef sub(inputString.data() + 1,
                              inputString.length() - 1);
        rv = parseUnsignedInteger(&res,
                                  remainder,
                                  sub,
                                  base,
                                  static_cast<Uint64>(~minValue) + 1);
        if (!rv) {
            res = -res;
        }

    }
    else {
        const bool        hasPlus = ('+' == inputString[0]);
        bslstl::StringRef sub(inputString.data() + hasPlus,
                              inputString.length() - hasPlus);
        rv = parseUnsignedInteger(&res,
                                  remainder,
                                  sub,
                                  base,
                                  maxValue);
    }

    *result = res;
    return rv;
}

int NumericParseUtil::parseUnsignedInteger(
                                         bsls::Types::Uint64      *result,
                                         bslstl::StringRef        *remainder,
                                         const bslstl::StringRef&  inputString,
                                         int                       base,
                                         const bsls::Types::Uint64 maxValue)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(remainder);
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(     base <= 36);

    const bsls::Types::Uint64 maxCheck = maxValue / base;
    const size_type           length   = inputString.length();
    if (0 == length) {
        // ERROR: The number must have at least one digit.
        *remainder = inputString;
        return -1;                                                    // RETURN
    }

    bsls::Types::Uint64 res = 0;
    int                 digit = characterToDigit(inputString[0], base);
    if (digit == -1) {
        *remainder = inputString;
        // ERROR: The first character must be a digit.
        return -2;                                                    // RETURN
    }

    size_type i = 0;
    while (-1 != digit) {
        if (res < maxCheck) {
            res = res * base + digit;
            if (++i >= length) {
                break;                                                 // BREAK
            }
            digit = characterToDigit(inputString[i], base);
        }
        else if (res == maxCheck &&
                 digit <= static_cast<int>(maxValue % base)) {
            res = res * base + digit;
            ++i;
            break;                                                     // BREAK
        }
        else {
            break;                                                     // BREAK
        }
    }

    remainder->assign(&inputString[0] + i, length - i);
    *result = res;
    return 0;
}

int NumericParseUtil::parseUnsignedInteger(
                                        bsls::Types::Uint64      *result,
                                        bslstl::StringRef        *remainder,
                                        const bslstl::StringRef&  inputString,
                                        int                       base,
                                        const bsls::Types::Uint64 maxValue,
                                        int                       maxNumDigits)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(remainder);
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(     base <= 36);
    BSLS_ASSERT(0 <= maxNumDigits);

    const bsls::Types::Uint64 maxCheck = maxValue / base;
    const size_type           length   = inputString.length();

    bsls::Types::Uint64       res = 0;
    int                       digit = characterToDigit(inputString[0], base);
    if (digit == -1) {
        // ERROR: The number must have at least one digit.
        *remainder = inputString;
        return -1;                                                    // RETURN
    }

    size_type i = 0;
    while (-1 != digit && maxNumDigits--) {
        if (res < maxCheck) {
            res = res * base + digit;
            if (++i >= length) {
                break;                                                 // BREAK
            }
            digit = characterToDigit(inputString[i], base);
        }
        else if (res == maxCheck &&
                 digit <= static_cast<int>(maxValue % base)) {
            res = res * base + digit;
            ++i;
            break;                                                     // BREAK
        }
        else {
            break;                                                     // BREAK
        }
    }

    remainder->assign(&inputString[0] + i, length - i);
    *result = res;
    return 0;
}


}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
