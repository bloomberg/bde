// bdlb_numericparseutil.cpp                                          -*-C++-*-
#include <bdlb_numericparseutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlb_numericparseutil_cpp, "$Id$ $CSID$")

#include <bdlb_chartype.h>

#include <bslma_allocator.h>
#include <bslma_sequentialallocator.h>
#include <bslma_default.h>

#include <bslmf_assert.h>

#include <bsl_string_view.h>
#include <bsl_cctype.h>       // for 'bsl::isspace'
#include <bsl_climits.h>      // for 'CHAR_MIN'
#include <bsl_clocale.h>      // for 'bsl::setlocale'
#include <bsl_cstdlib.h>      // for 'bsl::strtod'

#include <bsls_assert.h>
#include <bsls_platform.h>

#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1900
    // Needed for fixing the broken 'strtod', see below.
#include <bdlb_chartype.h>
#include <bdlb_string.h>
#include <bsl_limits.h>
#endif

namespace {

using namespace BloombergLP;

namespace u {

typedef bsls::Types::Int64  Int64;
typedef bsls::Types::Uint64 Uint64;

static
bsl::string_view stripOptionalPlus(const bsl::string_view& positiveNum)
    // Return a string reference to the characters of the specified
    // 'positiveNum' that do not contain the optionally present first '+'
    // ASCII character.  The behavior is undefined if 'positiveNum.empty()'.
{
    BSLS_ASSERT(!positiveNum.empty());

    return ('+' == positiveNum[0]) ? positiveNum.substr(1) : positiveNum;
}

#if defined(BSLS_PLATFORM_OS_LINUX)
static int doubleSign(double number)
    // Return 0 if the specified 'number' is positive, and a non-zero value
    // otherwise.  This function is needed only until we have all platforms
    // updated to C++11 that supports the 'signbit' function.
{
    const unsigned char *bytes = reinterpret_cast<unsigned char *>(&number);

#ifdef BSLS_PLATFORM_IS_BIG_ENDIAN
    static bsl::size_t pos = 0;
#elif defined(BSLS_PLATFORM_IS_LITTLE_ENDIAN)
    static bsl::size_t pos = sizeof(double) - 1;
#else
    #error "Neither big or little endian?  Compilation aborted."
    BSLS_PLATFORM_COMPILER_ERROR;
#endif
    return bytes[pos] & 0x80;
}
#endif // defined(BSLS_PLATFORM_OS_LINUX)

#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1900
// 'strtod' in the Microsoft CRT (C RunTime Library) has a non-standard
// implementation prior to Microsoft Visual Studio 2015.  The function (in that
// non-standard implementation) does not parse the special 'double' values NaN
// and Infinity as specified on
// http://en.cppreference.com/w/cpp/string/byte/strtof.  When Microsoft's
// 'strtod' reports that it could not parse its input string, the following
// functions are used to attempt to parse them as NaN or Infinity.

static bool isPrefixCaseless(const bsl::string_view& inputString,
                             const bsl::string_view& prefix)
    // Check if the specified 'inputString' has the specified 'prefix' with
    // case insensitive comparison.  Return 'true' if the prefix matches and
    // 'false' otherwise.
{
    if (inputString.length() < prefix.length()) {
        return false;                                                 // RETURN
    }

    return String::areEqualCaseless(
        inputString.data(), prefix.length(), prefix.data(), prefix.length());
}

static bool safeCheck(const bsl::string_view&     inputString,
                      bsl::string_view::size_type pos,
                      char                        ch)
    // Check in the specified 'inputString', if the characters at the specified
    // 'pos', if such position exists, equals to the specified 'ch'.
{
    return (inputString.length() > pos) && inputString[pos] == ch;
}

static bool isNanString(bsl::string_view        *remainder,
                        const bsl::string_view&  inputString)
    // Check the specified 'inputString' if its prefix is a "NaN" string
    // representation (see
    // http://en.cppreference.com/w/cpp/string/byte/strtof).  If it is, load
    // the rest of the 'inputString' into the specified 'remainder' and return
    // 'true'; otherwise do not change 'remainder' and return 'false'.
{
    if (!u::isPrefixCaseless(inputString, "NaN")) {
        return false;                                                 // RETURN
    }

    if (!u::safeCheck(inputString, 3, '(')) {
        *remainder = inputString.substr(3);

        return true;                                                  // RETURN
    }

    typedef bsl::string_view::size_type size_type;
    for (size_type i = 4; i < inputString.length(); ++i) {
        const char ch = inputString[i];

        if (ch == ')') {
            ++i;  // Include the ')'
            *remainder = inputString.substr(i);

            return true;                                              // RETURN
        }

        if (ch != '_' && !CharType::isAlnum(ch)) {
            break;
        }
    }

    return false;
}

static int parseNanAndInf(double           *result,
                          bsl::string_view *remainder,
                          bsl::string_view  inputString)
    // Check the specified 'inputString' if its prefix is a "NaN" or Infinity
    // string representation (see
    // http://en.cppreference.com/w/cpp/string/byte/strtof).  If it is, load
    // the result of the conversion into the specified 'result', load the rest
    // of the 'inputString' into the specified 'remainder' and return zero;
    // otherwise do not change 'remainder' and return a non-zero value.
{
    int sign = 1;

    if (inputString[0] == '+' || inputString[0] == '-') {
        if (inputString[0] == '-') {
            sign = -1;
        }
        if (inputString.size() == 1) {
            return -5;                                                // RETURN
        }
        inputString.assign(inputString.data() + 1, inputString.length() - 1);
    }

    if (u::isPrefixCaseless(inputString, "INFINITY")) {
        *result = sign * bsl::numeric_limits<double>::infinity();
        *remainder = inputString.substr(8);

        return 0;                                                     // RETURN
    }

    if (u::isPrefixCaseless(inputString, "INF")) {
        *result = sign * bsl::numeric_limits<double>::infinity();
        *remainder = inputString.substr(3);

        return 0;                                                     // RETURN
    }

    if (u::isNanString(remainder, inputString)) {
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

    return -4;
}

#endif  // Microsoft Visual Studio prior 2015 non-standard 'strtod' workaround.

}  // close namespace u
}  // close unnamed namespace

namespace BloombergLP {
namespace bdlb {

                        // -----------------------
                        // struct NumericParseUtil
                        // -----------------------
// CLASS METHODS
int NumericParseUtil::characterToDigit(char character, int base)
{
    BSLS_ASSERT_SAFE(2 <= base);
    BSLS_ASSERT_SAFE(base <= 36);

    BSLMF_ASSERT('9' - '0' ==  9);
    BSLMF_ASSERT('z' - 'a' == 25);

    BSLMF_ASSERT(0x30 == '0' && 0x39 == '9');  // Verify ASCII

    typedef CharType Ct;
    static const int aShift = 'a' - 10;
    const int digit = Ct::isDigit(character) ? character - '0'
                    : Ct::isAlpha(character) ? Ct::toLower(character) - aShift
                                             :                              -1;
    return digit < base ? digit : -1;
}

int NumericParseUtil::parseDouble(double                  *result,
                                  bsl::string_view        *remainder,
                                  const bsl::string_view&  inputString)
{
    BSLS_ASSERT(remainder);
    BSLS_ASSERT(result);

#ifdef BSLS_PLATFORM_OS_WINDOWS
    // 'setlocale' is slow on Windows, see '{drqs 161551330}'.

    BSLS_ASSERT_SAFE(bsl::setlocale(LC_NUMERIC, 0) == bsl::string_view("C"));
#else
    BSLS_ASSERT(bsl::setlocale(LC_NUMERIC, 0) == bsl::string_view("C"));
#endif

    // An empty string cannot be a number.

    if (inputString.empty()) {
        *remainder = inputString;
        return -1;                                                    // RETURN
    }

    // We need to ensure that the string does not start with a whitespace
    // (because our contract says so).  Our current implementation uses the
    // 'bsl::strtod' function and that happens to allow leading whitespace.  So
    // it *would* accept leading whitespace, we need to ensure there is none.
    // First check if the string starts with a 7-bit ASCII whitespace (using
    // our own ASCII-only function).  Then we must check for extra,
    // local-dependent whitespace as well, using 'bsl::isspace' that depends on
    // the 'LC_CTYPE' locale-category setting that we allow to be anything.
    // Because 'bsl::strtod' will skip any special whitespace enabled by the
    // uncontrolled (by preconditions) 'LC_CTYPE' locale-category but our
    // contract does not allow any special characters either we have to ensure
    // not to call 'strtod' if the string starts with such a whitespace.
    // 'bsl::isspace' (that is 'std::isspace') does not allow 'char' values
    // that are negative (not representable as 'unsigned char') so before
    // calling it (to check for 'LC_CTYPE' special whitespace) we convert the
    // first character to 'unsigned char'.  As we *know* that no text with any
    // non-ASCII character in it (for example UTF-8) can be a valid 'double' by
    // our own definition (function contract) we first also check that the
    // 'unsigned char' does not have its top bit set, and if it does, we return
    // an error.  We avoid calling into the 'std::isspace' implementation in
    // case of non-ASCII characters as an overly paranoid check to ensure not
    // to hit some performance bottleneck with that extra call unless we have
    // to.  This complicated approach will be soon replaced with a
    // comprehensive locale-independent solution for all code that assumes the
    // "C" locale is set, including this one.  This is just a quick fix to
    // enable this function to operate under node.js that changes the global
    // locale away from "C", except for 'LC_NUMERIC'.

    unsigned char firstAsUchar = static_cast<unsigned char>(inputString[0]);
    if (firstAsUchar & 0x80u              ||  // Not (7-bit) ASCII first char
        CharType::isSpace(inputString[0]) ||  //   or an ASCII whitespace char
        bsl::isspace(firstAsUchar))           //   or locale-enabled whitespace
    {                                         // so an invalid first character
        *remainder = inputString;             // for a 'double' as per contract
        return -3;                                                    // RETURN
    }

    static const size_type     k_BUFFER_SIZE = 128;
    char                       rawBuffer[k_BUFFER_SIZE];
    bslma::SequentialAllocator alloc(rawBuffer, k_BUFFER_SIZE);
    const bsl::string          nullTerminatedInput(inputString, &alloc);

    char         *endPtr;
    const char   *buffer = nullTerminatedInput.c_str();
    const double  rv     = bsl::strtod(buffer, &endPtr);

    *remainder = inputString.substr(endPtr - buffer);

    if (endPtr != buffer) {
        *result = rv;
#if defined(BSLS_PLATFORM_OS_LINUX)
        // 'strtod' is broken in libstdc++, parsing negative NaN into positive
        // NaN.  We are not checking for compilers because both clang and g++
        // exhibit the fault on Linux.

        if (*result != *result && inputString[0] == '-') {
            if (!u::doubleSign(*result)) {
                // If 'result' is (incorrectly) a positive NaN value, reverse
                // the sign

                *result = -*result;
            }
        }
#endif
        return 0;                                                     // RETURN
    }

#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1900
    // 'strtod' in the Microsoft CRT (C RunTime Library) has a non-standard
    // implementation prior to Microsoft Visual Studio 2015.  The function (in
    // that non-standard implementation) does not parse the special 'double'
    // values NaN and Infinity as specified on
    // http://en.cppreference.com/w/cpp/string/byte/strtof.  When Microsoft's
    // 'strtod' reports that it could not parse its input string, the
    // following function is used to attempt to parse them as NaN or Infinity.

    return u::parseNanAndInf(result, remainder, inputString);
#else
    return -4;
#endif
}

int NumericParseUtil::parseInt(int                     *result,
                               bsl::string_view        *remainder,
                               const bsl::string_view&  inputString,
                               int                      base)
{
    BSLS_ASSERT(remainder);
    BSLS_ASSERT(result);
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(base <= 36);

    u::Int64  res = 0;
    const int rv  = parseSignedInteger(&res,
                                       remainder,
                                       inputString,
                                       base,
                                       -static_cast<u::Int64>(0x80000000),
                                       static_cast< u::Int64>(0x7FFFFFFF));
    if (0 == rv) {
        *result = static_cast<int>(res);
    }
    return rv;
}

int NumericParseUtil::parseInt64(bsls::Types::Int64      *result,
                                 bsl::string_view        *remainder,
                                 const bsl::string_view&  inputString,
                                 int                      base)
{
    BSLS_ASSERT(remainder);
    BSLS_ASSERT(result);
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(base <= 36);

    u::Int64  res = 0;
    const int rv  = parseSignedInteger(
                             &res,
                             remainder,
                             inputString,
                             base,
                             -static_cast<u::Int64>(0x7FFFFFFFFFFFFFFFuLL) - 1,
                             static_cast< u::Int64>(0x7FFFFFFFFFFFFFFFuLL));
    if (0 == rv) {
        *result = res;
    }
    return rv;
}

int NumericParseUtil::parseUint(unsigned int            *result,
                                bsl::string_view        *remainder,
                                const bsl::string_view&  inputString,
                                int                      base)
{
    BSLS_ASSERT(remainder);
    BSLS_ASSERT(result);
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(base <= 36);

    if (inputString.empty()) {
        *remainder = inputString;
        return -1;                                                    // RETURN
    }
    const bsl::string_view num = u::stripOptionalPlus(inputString);

    u::Uint64 res = 0;
    const int rv  = parseUnsignedInteger(&res,
                                         remainder,
                                         num,
                                         base,
                                         static_cast<u::Int64>(0xFFFFFFFF));
    if (0 == rv) {
        *result = static_cast<unsigned int>(res);
    }
    return rv;
}

int NumericParseUtil::parseUint64(bsls::Types::Uint64     *result,
                                  bsl::string_view        *remainder,
                                  const bsl::string_view&  inputString,
                                  int                      base)
{
    BSLS_ASSERT(remainder);
    BSLS_ASSERT(result);
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(base <= 36);

    if (inputString.empty()) {
        *remainder = inputString;
        return -1;                                                    // RETURN
    }
    const bsl::string_view num = u::stripOptionalPlus(inputString);

    const u::Uint64 k_MAX = static_cast<u::Int64>(0xFFFFFFFFFFFFFFFFuLL);
    u::Uint64       res   = 0;
    const int       rv    = parseUnsignedInteger(&res,
                                                 remainder,
                                                 num,
                                                 base,
                                                 k_MAX);
    if (0 == rv) {
        *result = res;
    }
    return rv;
}

int NumericParseUtil::parseUshort(unsigned short          *result,
                                  bsl::string_view        *remainder,
                                  const bsl::string_view&  inputString,
                                  int                      base)
{
    BSLS_ASSERT(remainder);
    BSLS_ASSERT(result);
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(base <= 36);

    if (inputString.empty()) {
        *remainder = inputString;
        return -1;                                                    // RETURN
    }
    const bsl::string_view num = u::stripOptionalPlus(inputString);

    u::Uint64 res = 0;
    const int rv  = parseUnsignedInteger(&res,
                                         remainder,
                                         num,
                                         base,
                                         static_cast<u::Int64>(0xFFFF));
    if (0 == rv) {
        *result = static_cast<unsigned short>(res);
    }
    return rv;
}

int NumericParseUtil::parseShort(short                   *result,
                                 bsl::string_view        *remainder,
                                 const bsl::string_view&  inputString,
                                 int                      base)
{
    BSLS_ASSERT(remainder);
    BSLS_ASSERT(result);
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(base <= 36);

    u::Int64  res = 0;
    const int rv  = parseSignedInteger(&res,
                                       remainder,
                                       inputString,
                                       base,
                                       -static_cast<u::Int64>(32768),
                                       static_cast< u::Int64>(32767));
    if (0 == rv) {
        *result = static_cast<short>(res);
    }
    return rv;
}

int NumericParseUtil::parseSignedInteger(bsls::Types::Int64       *result,
                                         bsl::string_view         *remainder,
                                         const bsl::string_view&   inputString,
                                         int                       base,
                                         const bsls::Types::Int64  minValue,
                                         const bsls::Types::Int64  maxValue)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(remainder);
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(base <= 36);
    BSLS_ASSERT(minValue <= 0);
    BSLS_ASSERT(maxValue >= 0);

    if (0 == inputString.length()) {
        // ERROR: The number must have at least one digit.

        *remainder = inputString;
        return -1;                                                    // RETURN
    }


    u::Uint64 res = *result;

    if ('-' == inputString[0]) {
        const bsl::string_view num = inputString.substr(1);
        const int              rv  = parseUnsignedInteger(
                                        &res,
                                        remainder,
                                        num,
                                        base,
                                        static_cast<u::Uint64>(~minValue) + 1);
        if (0 == rv) {
            *result = -res;
        }
        return rv;                                                    // RETURN
    }

    // Positive number
    const int rv = parseUnsignedInteger(&res,
                                        remainder,
                                        u::stripOptionalPlus(inputString),
                                        base,
                                        maxValue);
    if (0 == rv) {
        *result = res;
    }
    return rv;
}

int NumericParseUtil::parseUnsignedInteger(
                                        bsls::Types::Uint64       *result,
                                        bsl::string_view          *remainder,
                                        const bsl::string_view&    inputString,
                                        int                        base,
                                        const bsls::Types::Uint64  maxValue)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(remainder);
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(base <= 36);

    const u::Uint64 maxCheck = maxValue / base;
    const size_type length   = inputString.length();
    if (0 == length) {
        // ERROR: The number must have at least one digit.

        *remainder = inputString;
        return -1;                                                    // RETURN
    }

    u::Uint64 res   = 0;
    int       digit = characterToDigit(inputString[0], base);
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
            break;  // BREAK
        }
    }

    *remainder = inputString.substr(i);
    *result    = res;

    return 0;
}

int NumericParseUtil::parseUnsignedInteger(
                                       bsls::Types::Uint64       *result,
                                       bsl::string_view          *remainder,
                                       const bsl::string_view&    inputString,
                                       int                        base,
                                       const bsls::Types::Uint64  maxValue,
                                       int                        maxNumDigits)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(remainder);
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(base <= 36);
    BSLS_ASSERT(0 <= maxNumDigits);

    const u::Uint64 maxCheck = maxValue / base;
    const size_type length   = inputString.length();

    u::Uint64 res   = 0;
    int       digit = characterToDigit(inputString[0], base);
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

    *remainder = inputString.substr(i);
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
