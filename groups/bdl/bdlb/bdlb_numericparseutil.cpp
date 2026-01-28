// bdlb_numericparseutil.cpp                                          -*-C++-*-
#include <bdlb_numericparseutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlb_numericparseutil_cpp, "$Id$ $CSID$")

#include <bdlb_chartype.h>

#include <bslma_allocator.h>
#include <bslma_sequentialallocator.h>
#include <bslma_default.h>

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_libraryfeatures.h>
#include <bsls_platform.h>

#include <bsl_string_view.h>

// Conditionally included headers
#ifdef BSLS_LIBRARYFEATURES_HAS_C99_FP_CLASSIFY
  #include <bsl_cmath.h>      // isinf
#endif

// Currently only the MSVC compiler implements the proposed resolution for the
// LWG 3081 defect report, other implementations do not set the output value to
// +/-0 or +/-Infinity depending whether under-, or overflow occurred during
// parsing of the text.
#ifndef BSLS_LIBRARYFEATURES_STDCPP_MSVC
    #define BDLB_NUMERICPARSEUTIL_VALUE_NOT_SET_ON_RANGE_ERROR                1
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV
  #include <bsl_charconv.h>      // from_chars, from_chars_result
  #include <bsl_system_error.h>  // errc::result_out_of_range

  #include <bsl_cstddef.h>       // size_t
  #ifdef BDLB_NUMERICPARSEUTIL_VALUE_NOT_SET_ON_RANGE_ERROR
    #include <bsl_cstdlib.h>     // strtod - needed to parse 0/Inf on ERANGE
  #endif
#else   // end - has float 'from_chars'
  #include <bsl_cctype.h>        // isspace
  #include <bsl_cerrno.h>        // errno, EDOM, EINVAL, ERANGE
  #include <bsl_climits.h>       // CHAR_BIT
  #include <bsl_clocale.h>       // setlocale
  #include <bsl_cstddef.h>       // ptrdiff_t
  #include <bsl_cstdlib.h>       // strtod
  #include <bsl_cstring.h>       // memcpy
#endif  // end - no float 'from_chars'

// The Solaris 'strtod' linked by us somehow does not parse hexadecimal floats,
// but the one (also Solaris) we link when using gcc on Solaris links to a
// different C standard library that has an 'strtod' with hexfloat parsing.
#if !defined(BSLS_PLATFORM_OS_SOLARIS) || defined(BSLS_PLATFORM_CMP_GNU)
  #define BDLB_NUMERICPARSEUTIL_STRTOD_PARSES_HEXFLOAT                        1
#endif

#if defined(__GLIBC__) && __GLIBC__ <= 2
  // glibc is linked dynamically, and we may get linked to any 2.x major
  // version when compiling against it, regardless of the '__GLIBC_MINOR__'
  // macro value we see compile time.  The bugs we identify below may get fixed
  // during the lifetime of GNU libc major version 2, but the code has to be
  // able to handle either case in the same binary due to the dynamic linking
  // of libc.

  /// Up to version 2.14 GNU glibc `strtod` implementation contains a bug
  /// where certain underflows are reported using the `EDOM == errno`,
  /// instead of the required `ERANGE == errno`.  The corresponding issue is
  /// 9696 (https://sourceware.org/bugzilla/show_bug.cgi?id=9696 ).
  #define u_GLIBC2_STRTOD_EDOM_BUG                                            1
#endif

namespace {
namespace u {

using namespace BloombergLP;

typedef bsls::Types::Int64  Int64;
typedef bsls::Types::Uint64 Uint64;

/// Return a string reference to the characters of the specified
/// `positiveNum` that do not contain the optionally present first `+`
/// ASCII character.  The behavior is undefined if `positiveNum.empty()`.
static
inline
bsl::string_view stripOptionalPlus(const bsl::string_view& positiveNum)
{
    BSLS_ASSERT(!positiveNum.empty());

    return ('+' == positiveNum[0]) ? positiveNum.substr(1) : positiveNum;
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV
#ifdef BDLB_NUMERICPARSEUTIL_VALUE_NOT_SET_ON_RANGE_ERROR
/// Parse the specified `strtodInput` that is known to be a signless,
/// parsable representation of a number too small or too large to
/// be represented in a `double` and return 0.0 if it underflows and
/// Infinity if it overflows.  Also fill the specified `restPtr` with the
/// address of the first character of `strtodInput` that was not parsed as
/// part of the returned value.
///
/// Except when the proposed resolution to LWG 3081 is implemented, floating
/// point `std::from_chars` does not "tell" us the difference between under-
/// and overflow by filling its `double` output argument with 0.0 or
/// Infinity, it just reports there was a range error.  We have to parse the
/// number again using `std::strtod` so we can tell which one of the 4
/// possibilities it was.  We have to do this because our original
/// implementation of `parseDouble` did not implement the promised grammar
/// correctly and return an error on under/overflow correctly but it
/// returned success and filled value with +/-0.0 or +-Infinity.
static
double reparseOutOfRange(const char              **restPtr,
                         const bsl::string_view&   strtodInput)
{
    const bsl::size_t          k_BUFFER_SIZE = 128;
    char                       rawBuffer[k_BUFFER_SIZE];
    bslma::SequentialAllocator alloc(rawBuffer, k_BUFFER_SIZE);
    const bsl::string          nullTerminatedInput(strtodInput, &alloc);

    const char *buffer = nullTerminatedInput.c_str();
    char *endPtr;
    const double  rv = bsl::strtod(buffer, &endPtr);

    BSLS_ASSERT(endPtr != buffer);  // This should not fail, 'from_chars'
                                    // parsed this 'strtodInput' already.

    *restPtr = strtodInput.data() + (endPtr - buffer);

    return rv;
}
#endif  // 'from_string' doesn't set the output value on under/overflow

#else   // end - using 'double' 'from_chars' / begin - using 'strtod'

                              // Portability
static
bool isInf(double number)
    // Return 'true' if the specified 'number' is a positive or negative
    // Infinity, otherwise return 'false' if it is a finite value or NaN.  Not
    // using 'bdlb::Float' as its functions are not inline, not even if they
    // exist on the given platform.
{
#ifdef BSLS_LIBRARYFEATURES_HAS_C99_FP_CLASSIFY
    return bsl::isinf(number);
#else  // BSLS_LIBRARYFEATURES_HAS_C99_FP_CLASSIFY
    BSLMF_ASSERT(sizeof(unsigned long long) >= sizeof(number));

    unsigned long long bits;
    bsl::memcpy(&bits, &number, sizeof(number));

    const int                k_NUM_BITS = sizeof number * CHAR_BIT;
    const unsigned long long k_SIGN_BIT = 1ULL << (k_NUM_BITS - 1);
        // The sign bit is the top bit of the double.
    const unsigned long long k_INF_BITS = 0x7FFULL << 52;
        // Infinity is 11 set exponent and 52 unset significand bits & sign.

    return (bits & ~k_SIGN_BIT) == k_INF_BITS;
#endif  // not BSLS_LIBRARYFEATURES_HAS_C99_FP_CLASSIFY
}

#ifdef BDLB_NUMERICPARSEUTIL_STRTOD_PARSES_HEXFLOAT
static
bool hasHexPrefix(const bsl::string_view& stdlibInput)
    // Return 'true' if the first two characters of the specified 'stdlibInput'
    // are '0x' or '0X' and 'false' otherwise.
{
    const bsl::string_view pref = stdlibInput.substr(0, 2);

    BSLMF_ASSERT('0' == '\x30' && 'X' == '\x58'&& 'x' == '\x78');

    return ("0x" == pref || "0X" == pref);
}
#endif  // BDLB_NUMERICPARSEUTIL_STRTOD_PARSES_HEXFLOAT
#endif  // else -- 'double' 'from_chars' exists

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

    BSLMF_ASSERT('9' - '0' ==  9);  // Ensure contiguous numbers and letters
    BSLMF_ASSERT('z' - 'a' == 25);

    BSLMF_ASSERT(0x30 == '0' && 0x39 == '9');  // Ensure ASCII literal values

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

    // NOTE TO IMPLEMENTERS: Any return statement in this function *must* be
    //                       preceded by a setting of 'remainder' and, if zero
    //                       is returned, the setting of 'result'!

    if (inputString.empty()) {
        *remainder = inputString;
        return -1;                                                    // RETURN
    }

    // Leading whitespace is not allowed
    if (bdlb::CharType::isSpace(inputString.front())) {
        *remainder = inputString;
        return -2;                                                    // RETURN
    }

    // Some 'strtod' and 'from_chars' have trouble with NaN sign.  A negative
    // sign also give us complications in 'from_chars' (that does not handle
    // it).  For simplification of later code we just manage the negative sign
    // ourselves.
    const bool isNegative = ('-' == inputString.front());

    // To move the start so 'from_chars' works like our contract says
    bsl::string_view stdlibInput = inputString;

    // Remove optional, redundant '+' sign
    if (isNegative || '+' == stdlibInput.front()) {
        stdlibInput.remove_prefix(1);

        // Only a lonely sign, not a good sign for a number
        if (stdlibInput.empty()) {
            *remainder = inputString;
            return -3;                                                // RETURN
        }

        // Check if there are any more sign characters as they may be accepted
        // by the implementation we defer to for parsing the number, so an
        // input like '"++1234"' or '"--0.5"' would get parsed into bizarre
        // results.
        if ('-' == stdlibInput.front() || '+' == stdlibInput.front()) {
            *remainder = inputString;
            return -4;                                                // RETURN
        }
    }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV
    double value;  // We read into a separate variable to strictly control when
                   // 'result' is updated and with what value.

    const bsl::from_chars_result fromCharsResult =
                       bsl::from_chars(stdlibInput.data(),
                                       stdlibInput.data() + stdlibInput.size(),
                                       value,
                                       bsl::chars_format::general);

    // We can start calculating 'remainder'

    // Position of the first unparsed character within `stdlibInput`
    const bsl::size_t restOffset = fromCharsResult.ptr - stdlibInput.data();

    if (bsl::errc::result_out_of_range == fromCharsResult.ec) {
#ifdef BDLB_NUMERICPARSEUTIL_VALUE_NOT_SET_ON_RANGE_ERROR
        // This is the "normal" behavior that the ISO C++ Standard mandates at
        // the time of writing: do not fill the 'value' output argument in case
        // of under- or overflow, as opposed to Microsoft Visual Studio that
        // implements the first proposed resolution to issue LWG 3081 that asks
        // for 'value' to be set in addition to returning the
        // 'result_out_of_range' code.
        //
        // We know GNU libstdc++ does not implement LWG 3081 and assume that
        // other 'from_chars' implementations won't implement LWG 3081 either
        // at first, but we also verify these assumptions in the test driver.

        // This code is not pretty, but it is expected to be removed within
        // a few years as LWG 3081 is resolved and adopted, so it is written in
        // a way so as not to affect the rest of the code.
        const char *restPtr;
        value = u::reparseOutOfRange(&restPtr, stdlibInput);

        *remainder = stdlibInput.substr(restPtr - stdlibInput.data());
#else  // BDLB_NUMERICPARSEUTIL_VALUE_NOT_SET_ON_RANGE_ERROR
        *remainder = stdlibInput.substr(restOffset);
#endif  // 'double' value is set to 0 or Inf on range error
        *result = isNegative ? -value : value;
        return ERANGE;                                                // RETURN
    }

    // Handle the more likely success case first
    if (bsl::errc{} == fromCharsResult.ec) {
        *result    = isNegative ? -value : value;
        *remainder = stdlibInput.substr(restOffset);
        return 0;                                                     // RETURN
    }

    // We have real errors that need to be reported at this point

    // If nothing was parsed the whole input string is invalid, not just the
    // part 'from_chars' has seen.
    const bool nothingWasParsed = (0 == restOffset);
    *remainder = nothingWasParsed
               ? inputString
               : stdlibInput.substr(restOffset);

    return -5;                                                        // RETRUN

#else  // has 'double' 'from_chars'

#ifdef BSLS_PLATFORM_OS_WINDOWS
    // 'setlocale' is very slow on Windows, see '{fifw drqs 161551330}'.

    BSLS_ASSERT_SAFE(bsl::setlocale(LC_NUMERIC, 0) == bsl::string_view("C"));
#else
    BSLS_ASSERT(bsl::setlocale(LC_NUMERIC, 0) == bsl::string_view("C"));
#endif

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

    unsigned char firstAsUchar = static_cast<unsigned char>(stdlibInput[0]);
    if ((firstAsUchar & 0x80u) != 0 || bsl::isspace(firstAsUchar)) {
        // Not 7-bit ASCII or a whitespace according to current global C locale
        *remainder = inputString;
        return -6;                                                    // RETURN
    }

    const size_type offset = stdlibInput.data() - inputString.data();

#ifdef BDLB_NUMERICPARSEUTIL_STRTOD_PARSES_HEXFLOAT
    if (u::hasHexPrefix(stdlibInput)) {
        // Parsed a hexfloat, but we don't allow hexfloats.  Let's "emulate"
        // how 'strtod' worked before it supported hexfloats:

        *result = (isNegative ? -0.0 : 0.0);          // Parse '0x' into 0
        *remainder = inputString.substr(offset + 1);  // Stop parse on the 'xX'

        return 0;                                                     // RETURN
    }
#endif  // BDLB_NUMERICPARSEUTIL_STRTOD_PARSES_HEXFLOAT

    const size_type   k_BUFFER_SIZE = 128;
    char              rawBuffer[k_BUFFER_SIZE];
    bslma::SequentialAllocator alloc(rawBuffer, k_BUFFER_SIZE);

    const bsl::string nullTerminatedInput(stdlibInput, &alloc);

    char *endPtr;
    const char *buffer = nullTerminatedInput.c_str();
    errno = 0;
    const double rv = bsl::strtod(buffer, &endPtr);
#if u_GLIBC2_STRTOD_EDOM_BUG
    if (EDOM == errno) {
        errno = ERANGE;
    }
#endif
    BSLS_ASSERT(!errno || ERANGE == errno || EINVAL == errno);

    const bsl::ptrdiff_t parsedLen = endPtr - buffer;
    if (0 == parsedLen) {
        // Nothing could be parsed
        *remainder = inputString;
        return -8;                                                    // RETURN
    }

    // Don't report range error on subnormals
    if (ERANGE == errno && !u::isInf(rv) && rv != 0) {
        // The above could be made faster because we know 'rv' is always
        // *positive*.  So we do not need the expensive 'double' things, we can
        // bit-compare to positive infinity, and positive zero.
        errno = 0;
    }

    *remainder = inputString.substr(offset + parsedLen);

    *result = (isNegative ? -rv : rv);

    return errno;
#endif  // not 'BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV'
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
