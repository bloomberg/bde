// bslalg_untrustedarrayreadutil.t.cpp                                -*-C++-*-
#include <bslalg_untrustedarrayreadutil.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <cstddef>
#include <cstring>
#include <limits>
#include <sstream>
#include <streambuf>
#include <string>

#include <stdio.h>      // `printf`
#include <stdlib.h>     // `atoi`

using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test is a utility of pure functions on integral
// arguments.  Each function is tested with a table of inputs and expected
// results, including boundary values at which a naive implementation would
// overflow, and with negative tests of its preconditions.  The growth bound
// promised by `computeNextReadCount` is additionally verified by simulating
// read loops.
// ----------------------------------------------------------------------------
// CLASS METHODS
// [ 2] bool canReadAll(size_t, size_t, size_t);
// [ 3] size_t computeReserveCount(size_t, size_t, ptrdiff_t);
// [ 4] size_t computeNextReadCount(size_t, size_t, size_t, ptrdiff_t, size_t);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 5] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);
        fflush(stdout);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bslalg::UntrustedArrayReadUtil Util;

const std::size_t    MAX_SIZE = std::numeric_limits<std::size_t>::max();
const std::ptrdiff_t MAX_DIFF = std::numeric_limits<std::ptrdiff_t>::max();

// ============================================================================
//                       HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

/// Return the result of `Util::computeNextReadCount` for the specified
/// `numRead`, `numRemaining`, `elementSize`, `numBufferedBytes`, and
/// `minReadCount`, having verified that it has the documented properties, and
/// reporting any violation with the specified `line`.
std::size_t verifyNextReadCount(int            line,
                                std::size_t    numRead,
                                std::size_t    numRemaining,
                                std::size_t    elementSize,
                                std::ptrdiff_t numBufferedBytes,
                                std::size_t    minReadCount)
{
    const std::size_t COUNT = Util::computeNextReadCount(numRead,
                                                         numRemaining,
                                                         elementSize,
                                                         numBufferedBytes,
                                                         minReadCount);

    const std::size_t NUM_BUFFERED =
                 0 < numBufferedBytes
                     ? static_cast<std::size_t>(numBufferedBytes) / elementSize
                     : 0;

    ASSERTV(line, COUNT, numRemaining, COUNT <= numRemaining);

    ASSERTV(line,
            COUNT,
            minReadCount,
            numRemaining,
            minReadCount <= COUNT || numRemaining == COUNT);

    ASSERTV(line,
            COUNT,
            numRead,
            numRemaining,
            numRead <= COUNT || numRemaining == COUNT);

    ASSERTV(line,
            COUNT,
            NUM_BUFFERED,
            numRemaining,
            NUM_BUFFERED <= COUNT || numRemaining == COUNT);

    // `numRead + COUNT <= minReadCount + 2 * (numRead + NUM_BUFFERED)`
    ASSERTV(line,
            COUNT,
            numRead,
            minReadCount,
            NUM_BUFFERED,
            COUNT <= minReadCount || COUNT - minReadCount <= numRead ||
                COUNT - minReadCount - numRead <= 2 * NUM_BUFFERED);

    return COUNT;
}

// ============================================================================
//                             USAGE EXAMPLES
// ----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
namespace usage_example_1 {

///Example 1: Validating an Array Length Against an In-Memory Buffer
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we are reading 4-byte values from a buffer received from an
// untrusted source, and the number of values to read was itself read from that
// buffer.
//
// First, we define a simple type describing the input:
// ```

/// This `struct` describes a buffer of input bytes and the position of the
/// next byte to be read.
struct Input {

    // PUBLIC DATA
    const char  *d_data_p;  // input bytes
    std::size_t  d_length;  // number of bytes in `d_data_p`
    std::size_t  d_cursor;  // index of the next byte to read
};
// ```
// Then, we implement a function that reads the values, using `canReadAll` to
// check that the buffer holds all of them:
// ```

/// Load into the specified `values` the specified `numValues` 4-byte
/// values read from the specified `input`, and advance its cursor past
/// them.  Return 0 on success, and a non-zero value, with no effect on
/// `values` or `input`, if `input` does not hold `numValues` more values.
int readValues(int *values, std::size_t numValues, Input *input)
{
    const std::size_t k_VALUE_SIZE = 4;

    if (!bslalg::UntrustedArrayReadUtil::canReadAll(
                                       numValues,
                                       k_VALUE_SIZE,
                                       input->d_length - input->d_cursor)) {
        return -1;                                                    // RETURN
    }

    const std::size_t numBytes = numValues * k_VALUE_SIZE;
    std::memcpy(values, input->d_data_p + input->d_cursor, numBytes);
    input->d_cursor += numBytes;
    return 0;
}
// ```
// Note that the multiplication is safe once `canReadAll` has returned `true`,
// as the product cannot then exceed the number of buffered bytes.
//
// Finally, we observe that a forged count is rejected even when multiplying it
// by the value size would wrap around to a small number:
// ```
void run()
{
    const char  buffer[16] = { 0 };
    Input       input      = { buffer, sizeof buffer, 0 };
    int         values[4];

    ASSERT(0 == readValues(values, 4, &input));
    ASSERT(16 == input.d_cursor);

    input.d_cursor = 0;
    const std::size_t k_FORGED = (std::numeric_limits<std::size_t>::max() / 4)
                               + 1;  // `k_FORGED * 4` wraps to 0
    ASSERT(0 != readValues(values, k_FORGED, &input));
    ASSERT(0 == input.d_cursor);
}
// ```

}  // close namespace usage_example_1

namespace usage_example_2 {

///Example 2: Reading a String of Untrusted Length from a Stream Buffer
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we are reading a string whose length was read from a
// `std::streambuf` that delivers data from an untrusted source.  Resizing the
// string to the claimed length up front would let a forged length cause an
// arbitrarily large allocation, so we instead grow the string as we read.
//
// First, we implement a function that reads the string, using
// `computeNextReadCount` to choose how much to read on each step:
// ```

/// Load into the specified `result` the specified `length` bytes read
/// from the specified `streamBuf`.  Return 0 on success, and a non-zero
/// value, leaving `result` in a valid but unspecified state, if
/// `streamBuf` does not deliver `length` bytes.
int readString(std::string    *result,
               std::streambuf *streamBuf,
               std::size_t     length)
{
    const std::size_t k_MIN_READ_COUNT = 4 * 1024;

    result->clear();
    while (result->size() < length) {
        const std::size_t numRead   = result->size();
        const std::size_t numToRead =
            bslalg::UntrustedArrayReadUtil::computeNextReadCount(
                                                      numRead,
                                                      length - numRead,
                                                      1,
                                                      streamBuf->in_avail(),
                                                      k_MIN_READ_COUNT);

        result->resize(numRead + numToRead);
        const std::streamsize numBytes =
                                    static_cast<std::streamsize>(numToRead);
        if (numBytes != streamBuf->sgetn(&(*result)[numRead], numBytes)) {
            return -1;                                                // RETURN
        }
    }
    return 0;
}
// ```
// Then, we read a string whose length is accurate:
// ```
void run()
{
    std::stringbuf input("Hello, world!");
    std::string    result;

    ASSERT(0 == readString(&result, &input, 13));
    ASSERT("Hello, world!" == result);
// ```
// Finally, we observe that a forged length causes the read to fail after an
// allocation bounded by the minimum read count plus twice the buffered input,
// rather than by the length:
// ```
    std::stringbuf shortInput("Hello, world!");

    ASSERT(0 != readString(&result, &shortInput, 1024 * 1024 * 1024));
    ASSERT(4 * 1024 + 2 * 13 >= result.size());
}
// ```

}  // close namespace usage_example_2

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)veryVeryVerbose;      // suppress warning
    (void)veryVeryVeryVerbose;  // suppress warning

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, remove
        //    leading comment characters, and replace `assert` with `ASSERT`.
        //    (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        usage_example_1::run();
        usage_example_2::run();
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING `computeNextReadCount`
        //
        // Concerns:
        // 1. The result does not exceed `numRemaining`, and is at least
        //    `minReadCount`, or `numRemaining` if less.
        //
        // 2. The result is at least `numRead`, or `numRemaining` if less, so
        //    that the number of elements read at least doubles on each step.
        //
        // 3. The result is at least the number of whole elements of
        //    `elementSize` bytes in `numBufferedBytes` bytes, or
        //    `numRemaining` if less, and a non-positive `numBufferedBytes`
        //    indicates that no elements are known to be buffered.
        //
        // 4. Growing a destination of `numRead` elements by the result never
        //    makes it larger than `minReadCount` plus twice the number of
        //    elements read plus buffered.
        //
        // 5. Concerns 1..4 hold for the extreme values of each argument.
        //
        // 6. A loop that reads the result on each step completes in a number
        //    of steps logarithmic in the total number of elements.
        //
        // 7. QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        // 1. Using the table-driven technique, verify concerns 1..4 for a set
        //    of representative and extreme inputs.  (C-1..5)
        //
        // 2. For a set of totals, minimum read counts, and per-step buffered
        //    byte counts, simulate a read loop, verify concerns 1..4 on each
        //    step, and verify that the loop completes in at most 128 steps
        //    (twice the number of bits in a 64-bit `std::size_t`).  (C-1..4,
        //    C-6)
        //
        // 3. Verify that, in appropriate build modes, defensive checks are
        //    triggered for invalid arguments.  (C-7)
        //
        // Testing:
        //   size_t computeNextReadCount(size_t, size_t, size_t, ptrdiff_t,
        //                               size_t);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING `computeNextReadCount`"
                            "\n==============================\n");

        if (verbose) printf("\nTable-driven test.\n");
        {
            const struct {
                int            d_line;
                std::size_t    d_numRead;
                std::size_t    d_numRemaining;
                std::size_t    d_elementSize;
                std::ptrdiff_t d_numBufferedBytes;
                std::size_t    d_minReadCount;
            } DATA[] = {
                // clang-format off
    // LINE READ          REMAINING  SIZE      BUFFERED   MIN
    // ---- ------------  ---------  --------  ---------  --------
    { L_,  0,                    0,        1,         0,        1 },
    { L_,  0,                  100,        1,         0,       10 },
    { L_,  0,                    5,        1,         0,       10 },
    { L_,  20,                 100,        1,         0,       10 },
    { L_,  20,                  10,        1,         0,       10 },
    { L_,  0,                  100,        1,        -1,       10 },
    { L_,  0,                  100,        1,        30,       10 },
    { L_,  10,                 100,        1,        30,        1 },
    { L_,  10,                  50,        1,        30,        1 },
    { L_,  0,                  100,        4,        30,        1 },
    { L_,  0,                  100,        4,         3,        1 },
    { L_,  0,                  100, MAX_SIZE,  MAX_DIFF,        1 },
    { L_,  MAX_SIZE,             5,        1,  MAX_DIFF,        1 },
    { L_,  0,             MAX_SIZE,        1,  MAX_DIFF,        1 },
    { L_,  MAX_SIZE - 1,  MAX_SIZE,        1,  MAX_DIFF,        1 },
    { L_,  MAX_SIZE,      MAX_SIZE,        1,  MAX_DIFF, MAX_SIZE },
    { L_,  5,             MAX_SIZE,        1,         0, MAX_SIZE },
            };
            // clang-format on
            const std::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

            for (std::size_t ti = 0; ti < NUM_DATA; ++ti) {
                const int            LINE      = DATA[ti].d_line;
                const std::size_t    READ      = DATA[ti].d_numRead;
                const std::size_t    REMAINING = DATA[ti].d_numRemaining;
                const std::size_t    SIZE      = DATA[ti].d_elementSize;
                const std::ptrdiff_t BUFFERED  = DATA[ti].d_numBufferedBytes;
                const std::size_t    MIN       = DATA[ti].d_minReadCount;

                if (veryVerbose) {
                    T_ P_(LINE) P_(READ) P_(REMAINING) P_(SIZE) P_(BUFFERED)
                    P(MIN)
                }

                verifyNextReadCount(LINE,
                                    READ,
                                    REMAINING,
                                    SIZE,
                                    BUFFERED,
                                    MIN);
            }
        }

        if (verbose) printf("\nSimulated read loops.\n");
        {
            const std::size_t    TOTALS[] = {1, 1000, 1000000, MAX_SIZE / 2};
            const std::size_t    MINS[]   = {1, 1024};
            const std::ptrdiff_t BUFFERED_SIZES[] = {-1, 0, 1, 4096};

            const std::size_t NUM_TOTALS = sizeof TOTALS / sizeof *TOTALS;
            const std::size_t NUM_MINS   = sizeof MINS / sizeof *MINS;
            const std::size_t NUM_BUFFERED_SIZES = sizeof BUFFERED_SIZES /
                                                   sizeof *BUFFERED_SIZES;

            for (std::size_t ti = 0; ti < NUM_TOTALS; ++ti) {
            for (std::size_t mi = 0; mi < NUM_MINS; ++mi) {
            for (std::size_t bi = 0; bi < NUM_BUFFERED_SIZES; ++bi) {
                const std::size_t    TOTAL    = TOTALS[ti];
                const std::size_t    MIN      = MINS[mi];
                const std::ptrdiff_t BUFFERED = BUFFERED_SIZES[bi];

                if (veryVerbose) { T_ P_(TOTAL) P_(MIN) P(BUFFERED) }

                std::size_t numRead  = 0;
                int         numSteps = 0;
                while (numRead < TOTAL && numSteps <= 128) {
                    numRead += verifyNextReadCount(L_,
                                                   numRead,
                                                   TOTAL - numRead,
                                                   1,
                                                   BUFFERED,
                                                   MIN);
                    ++numSteps;
                }
                ASSERTV(TOTAL, MIN, BUFFERED, numRead, TOTAL == numRead);
                ASSERTV(TOTAL, MIN, BUFFERED, numSteps, numSteps <= 128);
            }
            }
            }
        }

        if (verbose) printf("\nNegative testing.\n");
        {
            bsls::AssertTestHandlerGuard hG;

            ASSERT_PASS(Util::computeNextReadCount(0, 1, 1, 0, 1));
            ASSERT_FAIL(Util::computeNextReadCount(0, 1, 0, 0, 1));
            ASSERT_FAIL(Util::computeNextReadCount(0, 1, 1, 0, 0));
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING `computeReserveCount`
        //
        // Concerns:
        // 1. The result is the lesser of `numElements` and the number of
        //    `elementSize`-byte elements that `numBufferedBytes` bytes can
        //    hold.
        //
        // 2. The result is 0 when `numBufferedBytes` is not positive.
        //
        // 3. The result is correct for the maximum `std::ptrdiff_t` value of
        //    `numBufferedBytes` and the maximum `std::size_t` value of
        //    `numElements`.
        //
        // 4. QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        // 1. Using the table-driven technique, verify the result for a set of
        //    representative and boundary inputs.  (C-1..3)
        //
        // 2. Verify that, in appropriate build modes, defensive checks are
        //    triggered for invalid arguments.  (C-4)
        //
        // Testing:
        //   size_t computeReserveCount(size_t, size_t, ptrdiff_t);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING `computeReserveCount`"
                            "\n=============================\n");

        if (verbose) printf("\nTable-driven test.\n");
        {
            const struct {
                int            d_line;
                std::size_t    d_numElements;
                std::size_t    d_elementSize;
                std::ptrdiff_t d_numBufferedBytes;
                std::size_t    d_expected;
            } DATA[] = {
                // clang-format off
    //  LINE  ELEMENTS   SIZE      BUFFERED   EXPECTED
    //  ----  ---------  --------  ---------  --------------------------------
    {   L_,          0,        1,       100,                               0 },
    {   L_,         10,        1,       100,                              10 },
    {   L_,       1000,        1,       100,                             100 },
    {   L_,       1000,        2,       100,                              50 },
    {   L_,       1000,        3,       100,                              33 },
    {   L_,       1000,        1,         0,                               0 },
    {   L_,       1000,        1,        -1,                               0 },
    {   L_,   MAX_SIZE,        1,  MAX_DIFF, static_cast<std::size_t>(
                                                                   MAX_DIFF) },
    {   L_,          5,        1,  MAX_DIFF,                               5 },
                // clang-format on
            };
            const std::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

            for (std::size_t ti = 0; ti < NUM_DATA; ++ti) {
                const int            LINE      = DATA[ti].d_line;
                const std::size_t    ELEMENTS  = DATA[ti].d_numElements;
                const std::size_t    SIZE      = DATA[ti].d_elementSize;
                const std::ptrdiff_t BUFFERED  = DATA[ti].d_numBufferedBytes;
                const std::size_t    EXPECTED  = DATA[ti].d_expected;

                if (veryVerbose) {
                    T_ P_(LINE) P_(ELEMENTS) P_(SIZE) P_(BUFFERED) P(EXPECTED)
                }

                const std::size_t RESULT = Util::computeReserveCount(ELEMENTS,
                                                                     SIZE,
                                                                     BUFFERED);
                ASSERTV(LINE, EXPECTED, RESULT, EXPECTED == RESULT);
            }
        }

        if (verbose) printf("\nNegative testing.\n");
        {
            bsls::AssertTestHandlerGuard hG;

            ASSERT_PASS(Util::computeReserveCount(1, 1, 1));
            ASSERT_FAIL(Util::computeReserveCount(1, 0, 1));
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING `canReadAll`
        //
        // Concerns:
        // 1. The result is `true` if and only if `numElements * elementSize`
        //    does not exceed `numBufferedBytes`, computed without overflow.
        //
        // 2. The result is `false` when `numElements * elementSize` would wrap
        //    around to a value not exceeding `numBufferedBytes`.
        //
        // 3. QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        // 1. Using the table-driven technique, verify the result for a set of
        //    representative and boundary inputs, including products that
        //    wrap around to 0.  (C-1..2)
        //
        // 2. Verify that, in appropriate build modes, defensive checks are
        //    triggered for invalid arguments.  (C-3)
        //
        // Testing:
        //   bool canReadAll(size_t, size_t, size_t);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING `canReadAll`"
                            "\n====================\n");

        if (verbose) printf("\nTable-driven test.\n");
        {
            const struct {
                int         d_line;
                std::size_t d_numElements;
                std::size_t d_elementSize;
                std::size_t d_numBufferedBytes;
                bool        d_expected;
            } DATA[] = {
                // clang-format off
    //  LINE  ELEMENTS          SIZE      BUFFERED   EXPECTED
    //  ----  ----------------  --------  ---------  --------
    {   L_,                  0,        1,         0,    true },
    {   L_,                  1,        1,         0,   false },
    {   L_,                  4,        4,        16,    true },
    {   L_,                  5,        4,        16,   false },
    {   L_,                  4,        4,        15,   false },
    {   L_,           MAX_SIZE,        1,  MAX_SIZE,    true },
    {   L_,           MAX_SIZE,        2,  MAX_SIZE,   false },
    {   L_,   MAX_SIZE / 8 + 1,        8,        16,   false },  // wraps to 0
    {   L_,                  0, MAX_SIZE,         0,    true },
                // clang-format on
            };
            const std::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

            for (std::size_t ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE      = DATA[ti].d_line;
                const std::size_t ELEMENTS  = DATA[ti].d_numElements;
                const std::size_t SIZE      = DATA[ti].d_elementSize;
                const std::size_t BUFFERED  = DATA[ti].d_numBufferedBytes;
                const bool        EXPECTED  = DATA[ti].d_expected;

                if (veryVerbose) {
                    T_ P_(LINE) P_(ELEMENTS) P_(SIZE) P_(BUFFERED) P(EXPECTED)
                }

                const bool RESULT = Util::canReadAll(ELEMENTS, SIZE, BUFFERED);
                ASSERTV(LINE, EXPECTED, RESULT, EXPECTED == RESULT);
            }
        }

        if (verbose) printf("\nNegative testing.\n");
        {
            bsls::AssertTestHandlerGuard hG;

            ASSERT_PASS(Util::canReadAll(1, 1, 1));
            ASSERT_FAIL(Util::canReadAll(1, 0, 1));
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        // 1. The class is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        // 1. Call each function with typical arguments and verify the result.
        //    (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nBREATHING TEST"
                   "\n==============\n");

        ASSERT(Util::canReadAll(4, 4, 16));
        ASSERT(!Util::canReadAll(5, 4, 16));

        ASSERT(10 == Util::computeReserveCount(10, 2, 100));
        ASSERT(50 == Util::computeReserveCount(1000, 2, 100));
        ASSERT(0 == Util::computeReserveCount(1000, 2, -1));

        ASSERT(1024 == Util::computeNextReadCount(0, 1000000, 1, 0, 1024));
        ASSERT(10 == Util::computeNextReadCount(0, 10, 1, 100, 1));

        const std::size_t COUNT =
                             Util::computeNextReadCount(0, 1000000, 1, 100, 1);
        ASSERTV(COUNT, 100 <= COUNT && COUNT <= 201);
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2026 Bloomberg Finance L.P.
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
