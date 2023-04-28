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
#include <bsls_review.h>

#include <bsl_string_view.h>
#include <bsl_cctype.h>       // 'isspace'
#include <bsl_climits.h>      // 'CHAR_MIN', 'CHAR_BIT'
#include <bsl_clocale.h>      // 'setlocale'
#include <bsl_cstddef.h>      // 'size_t/ptrdiff_t'
#include <bsl_cstdlib.h>      // 'strtod'
#include <bsl_cstring.h>      // 'strchr'
#include <bsl_limits.h>       // 'numeric_limits<double>::denorm_min()'

// Conditionally included headers
#ifdef BSLS_LIBRARYFEATURES_HAS_C99_FP_CLASSIFY
    #include <bsl_cmath.h>        // 'bsl::isinf', 'bsl::isnan', 'bsl::signbit'
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV
    #include <bsl_charconv.h>
    #include <bsl_cmath.h>        // 'bsl::isnan'
    #include <bsl_system_error.h>
#else   // has float 'from_chars'
    #include <bsl_cerrno.h>       // 'errno', 'ERANGE'
#endif  // no float 'from_chars'

// Solaris 'strtod' linked by us does not parse hexadecimal floats.
#if !defined(BSLS_PLATFORM_OS_SUNOS) && !defined(BSLS_PLATFORM_OS_SOLARIS)
    #define BDLB_NUMERICPARSEUTIL_HAS_HEXFLOAT                                1
#endif

#include <bsl_iostream.h>
#if defined(__GLIBC__) && __GLIBC__ <= 2
    // glibc is linked dynamically, and we may get linked to any 2.x major
    // version when compiling against it, regardless of the '__GLIBC_MINOR__'
    // macro value we see compile time.  The bugs we identify below may get
    // fixed during the lifetime of GNU libc major version 2, but the code has
    // to be able to handle either case in the same binary due to the dynamic
    // linking of libc.

    #define u_GLIBC2_STRTOD_HEX_DENORM_MIN_HALF_BUG                           1
        // Hexadecimal float representations of the value "half of IEEE-754
        // double precision minimum subnormal" (positive or negative) that has
        // only a single bit set in its significand (in the string format) gets
        // parsed into the value 'DBL_TRUE_MIN' ('<climits>'), or in C++
        // vernacular 'bsl::numeric_limits<double>::denorm_min()' instead of
        // zero.

    #define u_GLIBC2_STRTOD_EDOM_BUG                                          1
        // Up to version 2.14 GNU glibc 'strtod' implementation contains a bug
        // where certain underflows are reported using the 'EDOM == errno',
        // instead of the required 'ERANGE == errno'.  The corresponding issue
        // is 9696 (https://sourceware.org/bugzilla/show_bug.cgi?id=9696 ).
#endif

namespace {
namespace u {

using namespace BloombergLP;

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

#define u_OUT_OF_RANGE_REVIEW(number, isNegative) do {                        \
    const bool isInfinity = u::isInf((number));                               \
    BSLS_REVIEW_OPT(("'parseDouble' overflow: -Infinity, "                    \
                     "Please route this issue to BDE (Group 101)."            \
                    && !isInfinity) || !isNegative);                          \
    BSLS_REVIEW_OPT(("'parseDouble' overflow: +Infinity, "                    \
                     "Please route this issue to BDE (Group 101)."            \
                    && !isInfinity) || isNegative);                           \
    BSLS_REVIEW_OPT(("'parseDouble' underflow: -0.0, "                        \
                     "Please route this issue to BDE (Group 101)."            \
                    && isInfinity) || !isNegative);                           \
    BSLS_REVIEW_OPT(("'parseDouble' underflow: +0.0, "                        \
                     "Please route this issue to BDE (Group 101)."            \
                    && isInfinity) || isNegative);                            \
    } while(false)
    // The easiest way to have this informative block of code to report the
    // proper line number (even if there is no call stack available) in
    // 'BSLS_REVIEW_OPT' is to use a macro.  The macro is temporary, and will
    // be removed once we learn how frequent are under/overflow parsing events
    // and so we can decide to enable them in the contract, or implement the
    // original contract and disable them.
    //
    // The macro checks for infinity and not 0 because POSIX 'strtod' does not
    // require '+/-0.0' for all underflows, it allows any number that is
    // smaller than the smallest normal (not-subnormal) number.

static
bool hasHexPrefix(const bsl::string_view& stdlibInput)
    // Return 'true' if the first two characters of the specified 'stdlibInput'
    // are '0x' or '0X' and 'false' otherwise.
{
    const bsl::string_view pref = stdlibInput.substr(0, 2);

    BSLMF_ASSERT('0' == '\x30' && 'X' == '\x58'&& 'x' == '\x78');

    return ("0x" == pref || "0X" == pref);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV

#ifndef BSLS_LIBRARYFEATURES_STDCPP_MSVC
static
double reparseOutOfRange(const char              **restPtr,
                         const bsl::string_view&   strtodInput)
    // Parse the specified 'strtodInput' that is known to be a signless,
    // parsable representation of a number too small or too large to
    // be represented in a 'double' and return 0.0 if it underflows and
    // Infinity if it overflows.  Also fill the specified 'restPtr' with the
    // address of the first character of 'strtodInput' that was not parsed as
    // part of the returned value.
    //
    // Except when the proposed resolution to LWG 3081 is implemented, floating
    // point 'std::from_chars' does not "tell" us the difference between under-
    // and overflow by filling its 'double' output argument with 0.0 or
    // Infinity, it just reports there was a range error.  We have to parse the
    // number again using 'std::strtod' so we can tell which one of the 4
    // possibilities it was.  We have to do this because our original
    // implementation of 'parseDouble' did not implement the promised grammar
    // correctly and return an error on under/overflow correctly but it
    // returned success and filled value with +/-0.0 or +-Infinity.
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

#ifdef u_GLIBC2_STRTOD_HEX_DENORM_MIN_HALF_BUG
    // On libstdc++ 'from_chars' properly reports out-of-range for the "half of
    // 'denorm_min()' value.  However 'strtod' of GNU libc might mistakenly
    // parse certain hexfloat input expressions of that value into
    // 'denorm_min()'.  In this function we do not need to look at the input
    // string to know that we have hit that bug, because that is the only case
    // a 'from_chars' out-of-range input can parse into 'denorm_min' here.

    if (bsl::numeric_limits<double>::denorm_min() == rv) {
        return 0.0;                                                   // RETURN
    }
#endif
    return rv;
}

#endif  // Does not report under/overflow using the 'double' output value

#else  // 'double' 'from_chars' exists

static
bool couldBeHexFloat(bsl::string_view strtodInput)
    // Return 'true' if the specified 'strtodInput' has a valid hexadecimal
    // prefix that followed by an optional dot and a hexadecimal digit, and
    // 'false' otherwise.  The behavior is undefined if 'strtodInput' is empty.
{
    BSLS_ASSERT(!strtodInput.empty());

    if (!hasHexPrefix(strtodInput) || strtodInput.size() < 3) {
        return false;                                                 // RETURN
    }

    if ('.' == strtodInput[2]) {  // Skip the dot if we have one
        if (strtodInput.size() < 4) {
            return false;                                             // RETURN
        }
        strtodInput.remove_prefix(3);
    }
    else {
        strtodInput.remove_prefix(2);
    }

    return bdlb::CharType::isXdigit(strtodInput.front());
}

#endif  // 'double' 'from_chars' does not exist

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
    // sign gives us complications in hexfloat-prefix and 'from_chars' (that
    // does not handle it).  For simplification of later code we just manage
    // the negative sign ourselves.
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
    // Check for hexfloat prefix
    const bool isHex = u::hasHexPrefix(stdlibInput);

    if (isHex) {  // Skip the hex prefix if present
        stdlibInput.remove_prefix(2);
    }

    double value;  // We read into a separate variable to strictly control when
                   // 'result' is updated and with what value.

    // The return value here can be made 'const' once the
    // 'u::reparseOutOfRange' hack below can be removed.
    bsl::from_chars_result fromCharsResult =
                       bsl::from_chars(stdlibInput.data(),
                                       stdlibInput.data() + stdlibInput.size(),
                                       value,
                                       isHex ? bsl::chars_format::hex
                                             : bsl::chars_format::general);

    if (bsl::errc::result_out_of_range == fromCharsResult.ec) {
#ifndef BSLS_PLATFORM_CMP_MSVC
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
        {
            // This code is not pretty, but it is expected to be removed within
            // a few years as LWG 3081 is resolved and adopted, so it is
            // written in a way so as not to affect the rest of the code.
            const bsl::ptrdiff_t withHexPrefix =
                    stdlibInput.data() - inputString.data() - (isHex ? 2 : 0);
            value = u::reparseOutOfRange(&fromCharsResult.ptr,
                                         inputString.substr(withHexPrefix));
        }
#endif
        // Microsoft implements LWG 3081 and fills 'value' in under/overflow so
        // we can handle it as we handle success.
        u_OUT_OF_RANGE_REVIEW(value, isNegative);

        fromCharsResult.ec = bsl::errc{}; // Pretend success
    }

    // We can start calculating 'remainder'
    const bsl::size_t restOffset = fromCharsResult.ptr - stdlibInput.data();
        // Position of the first bad character within 'stdlibInput'

    // Handle the more likely success case first
    if (bsl::errc{} == fromCharsResult.ec) {
        BSLS_REVIEW_OPT(!isHex && "Valid hexfloat parsed by 'from_chars', "
                                "Please route this issue to BDE (Group 101).");

        *remainder = stdlibInput.substr(restOffset);
        *result    = isNegative ? -value : value;

        return 0;                                                     // RETURN
    }

    // We have real errors reported at this point

    const bool allBad = (0 == restOffset);
        // 'from_chars' didn't parse even a single character from 'stdlibInput'

    // If nothing was parsed the whole input string is invalid
    *remainder = allBad ? inputString : stdlibInput.substr(restOffset);

    // An error that is not a special case
    if (!allBad || !isHex) {
        return -5;                                                    // RETRUN
    }

    // Imitate 'strtod' behavior for hexfloats that end right after '0x'
    // or have an invalid character right after the '0X'.
    // If 'from_chars' fails at position 0 for a hexfloat (we skipped
    // the hex prefix earlier and passed 'hex' for format) it means
    // that we had one of the following 3 "skipped" strings: "0x",
    // "+0x", "-0x", where 'x' may be also uppercase.  Since the prefix
    // is not followed by anything that can be parsed as a hexfloat we
    // do not really have a parsing failure, but we should parse the
    // '0' of "0x"and return a positive or negative floating point 0.
    *remainder = inputString.substr(inputString.find('0') + 1);
    *result    = isNegative ? -0.0 : 0.0;

    return 0;                                                         // RETURN

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

    const size_type            k_BUFFER_SIZE = 128;
    char                       rawBuffer[k_BUFFER_SIZE];
    bslma::SequentialAllocator alloc(rawBuffer, k_BUFFER_SIZE);

    // 'u_GLIBC2_STRTOD_HEX_DENORM_MIN_HALF_BUG': 'nullTerminatedInput' is not
    // 'const' because we may need to change the input and call 'strtod' again
    // to determine if we hit the bug.
    bsl::string                nullTerminatedInput(stdlibInput, &alloc);

    char *endPtr;
    const char *buffer = nullTerminatedInput.c_str();
    errno = 0;

    // 'u_GLIBC2_STRTOD_HEX_DENORM_MIN_HALF_BUG': 'rv' is not const because we
    // need to change it to zero when we determine that we have hit the bug.
    double      rv = bsl::strtod(buffer, &endPtr);

#if u_GLIBC2_STRTOD_HEX_DENORM_MIN_HALF_BUG
    if (ERANGE == errno && u::hasHexPrefix(nullTerminatedInput) &&
        bsl::numeric_limits<double>::denorm_min() == rv)
    {
        // GNU libc may have a very interesting bug in parsing hexadecimal
        // floats: it parses 'denorm_min()/2' into 'denorm_min()', but only if
        // the (hexadecimal) significand (in the string we parse) has only one
        // bit set.  We patch the bug as it can be detected and fixed by simple
        // and fast code.
        //
        // First (above), we detect the situation that *may* be the bug.  Next,
        // we determine if we have "fixed" or a "scientific" hexfloat by
        // looking for a 'p' or 'P' (backwards, as that part is probably still
        // hot in cache, and the exponent is usually much closer to the end of
        // the input string).

        typedef bsl::string_view::size_type Size;

        static const Size k_NPOS = bsl::string_view::npos;

        bsl::string_view parsed(buffer, endPtr - buffer);

        Size pPos = parsed.find_last_of("pP");

        // Handle the special case of an exponent present and being '0'
        if (k_NPOS != pPos &&
            k_NPOS == parsed.find_first_not_of("0-+", pPos + 1)) {
            // Cut of the 0 exponent
            parsed = parsed.substr(0, pPos);
            pPos   = k_NPOS;
        }

        if (k_NPOS == pPos) {
            // When there is no exponent the hexfloat string must follow a very
            // strict format for GNU libc 'strtod' to return 'denom_min()'.
            // The integer part *must* have the value zero (zero or more '0'
            // characters).  A radix dot *must* be present, and it *must* be
            // followed by *exactly* 268 '0' characters, followed by the
            // characters '2' or '4'.  The string may also have additional
            // trailing zeros in case we are hitting the bug itself, and it may
            // have additional significant digits after '4' when the value is
            // really 'denorm_min()'.  In any other case would not be in this
            // branch of the code.  Therefore, at this point, we just need to
            // verify that the only significant digit in the input string is a
            // single '2'.  If it is, the real value is 0 (we hit the bug).  If
            // it is not '2', or there are any other non-zero digits present
            // 'denorm_min()' was the right value.  (That last statement was
            // tested against two other 'strtod' implementations.)

            const Size sigDigPos = parsed.find_last_not_of('0');
            if ('2' == parsed[sigDigPos] &&
                '.' == parsed[parsed.find_last_not_of('0', sigDigPos - 1)]) {
                rv = 0.0;  // Fix up the value from 'denorm_min()' to zero.
            }
        }
        else {
            // We found a 'p': The number has a non-zero binary exponent.

            // We need to multiply the number by two by increasing the value of
            // the exponent by one.

            const Size maybeSignPos = pPos + 1;
            const bool expNegative  = ('-' == parsed[maybeSignPos]);
            const bool expHasPlus   = ('+' == parsed[maybeSignPos]);
            const bool hasSign      = expNegative || expHasPlus;

            const Size expTopEnd = pPos + hasSign;
            bool uoflow = true;
            for (Size p = parsed.size() - 1; p > expTopEnd; --p) {
                if (expNegative) {  // Negative exponent value goes down
                    if ((--nullTerminatedInput[p]) >= '0') {
                        uoflow = false;
                        break;                                         // BREAK
                    }
                    nullTerminatedInput[p] = '9';
                }
                else { // Positive exponent value goes up
                    if ((++nullTerminatedInput[p]) <= '9') {
                        uoflow = false;
                        break;                                         // BREAK
                    }
                    nullTerminatedInput[p] = '0';
                }
            }

            // Handle under/overflow in exponent value change
            if (uoflow) {
                if (expNegative) {  // Negative value went to zero
                    nullTerminatedInput.resize(pPos);
                }
                else {
                    // Positive exponent needs one more decimal digit in case
                    // of overflow.  If there was a redundant '+' we overwrite
                    // it with '1', otherwise we replace the top zero with '1':
                    nullTerminatedInput[expTopEnd + !expHasPlus] = '1';

                    // If there was no redundant plus we need to add one more
                    // zero digit to the end of the exponent:
                    if (!expHasPlus) {
                        // If there is space in the input we add the digit w/o
                        // triggering an allocation by overwriting whatever was
                        // after the parsed characters, and truncate the input
                        // to avoid concatenating a digit to the end of the
                        // exponent if it happened to be present there (e.g.,
                        // "...p100&9bbb" => "...p10009bb").
                        if (nullTerminatedInput.size() > parsed.size()) {
                            nullTerminatedInput[parsed.size()] = '0';
                            nullTerminatedInput.resize(parsed.size() + 1);
                        }
                        else {
                            // The string ends right after the original
                            // exponent value, we sadly may allocate to add the
                            // needed '0' digits to the end:
                            nullTerminatedInput += '0';
                        }
                    }
                }
            }

            errno = 0;
            const double rv_twice = bsl::strtod(buffer, 0);
            BSLS_ASSERT(rv_twice == rv || rv_twice == 2 * rv);

            if (rv_twice == rv) {
                // This mean we had 'denorm_min()/2' originally that hit the
                // bug, because twice the value also gives 'denorm_min()'.
                rv = 0.0;  // Fix the value from 'denorm_min()' to zero.
            }
        }
    }
#endif // u_GLIBC2_STRTOD_HEX_DENORM_MIN_HALF_BUG

#if u_GLIBC2_STRTOD_EDOM_BUG
    if (ERANGE == errno || EDOM == errno) {
#else
    if (ERANGE == errno) {
#endif
        u_OUT_OF_RANGE_REVIEW(rv, isNegative);
    }

    const bsl::ptrdiff_t parsedLen = endPtr - buffer;
    if (0 == parsedLen) {
        // Nothing could be parsed
        *remainder = inputString.substr();
    }
    else {
        // The problem is identified beyond what we have skipped
        const size_type skipped = stdlibInput.data() - inputString.data();
        *remainder = inputString.substr(parsedLen + skipped);
    }

    if (endPtr != buffer) {
#ifdef BDLB_NUMERICPARSEUTIL_HAS_HEXFLOAT
        BSLS_REVIEW_OPT("Valid hexfloat parsed by strtod, "
                        "Please route this issue to BDE (Group 101)."
                        && !u::couldBeHexFloat(stdlibInput));
#else
        BSLS_REVIEW_OPT("Possible hexfloat parsed on SunOS, "
                        "Please route this issue to BDE (Group 101)."
                        && 0.0 != rv || !u::couldBeHexFloat(stdlibInput));
#endif
        *result = (isNegative ? -rv : rv);

        return 0;                                                     // RETURN
    }

    return -7;
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
