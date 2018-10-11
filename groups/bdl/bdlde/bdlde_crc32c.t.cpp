// bdlde_crc32c.t.cpp                                                 -*-C++-*-
#include <bdlde_crc32c.h>

// BDE
#include <bdlde_crc32.h>

#include <bdlf_bind.h>

#include <bsl_algorithm.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_ctime.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_iterator.h>
#include <bsl_numeric.h>
#include <bsl_vector.h>

#include <bslmt_barrier.h>
#include <bslmt_threadgroup.h>

#include <bsls_asserttest.h>
#include <bsls_alignedbuffer.h>
#include <bsls_alignmentfromtype.h>
#include <bsls_alignmentutil.h>
#include <bsls_log.h>
#include <bsls_timeutil.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>

// CONVENIENCE
using namespace BloombergLP;
using namespace bdlde;
using namespace bsl;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test provides an interface for calculating a CRC32-C
// checksum as an unsigned integer vaalue.  The checksum is calculated on a
// sequence of data bytes and the data length using a hardware-acceselerated
// implementation if supported and a software implementation otherwise.  We
// need to verify that all CRC32-C implementations calculate the checksum
// correctly.
//
// Performance
// -----------
// Below are performance comparisons of the 64-bit Default (Hardware
// Accelerated) and software implementations against various alternative
// implementations that compute a  32-bit CRC checksum.  They were obtained on
// a Linux machine with the following CPU architecture:
//..
//  $ lscpu
//  Architecture:          x86_64
//  CPU op-mode(s):        32-bit, 64-bit
//  Byte Order:            Little Endian
//  CPU(s):                40
//  On-line CPU(s) list:   0-39
//  Thread(s) per core:    2
//  Core(s) per socket:    10
//  Socket(s):             2
//  NUMA node(s):          2
//  Vendor ID:             GenuineIntel
//  CPU family:            6
//  Model:                 62
//  Stepping:              4
//  CPU MHz:               3001.000
//  BogoMIPS:              5982.81
//  Virtualization:        VT-x
//  L1d cache:             32K
//  L1i cache:             32K
//  L2 cache:              256K
//  L3 cache:              25600K
//  NUMA node0 CPU(s):     0-9,20-29
//  NUMA node1 CPU(s):     10-19,30-39
//..
//
// Throughput
// ----------
//..
//  Default (Hardware Acceleration)| 20.363 GB per second
//  Software                       |  1.582 GB per second
//  BDE 'bdlde::crc32'             |  0.374 GB per second
//..
//
// Calculation Time
// ----------------
// In the tables below:
//: o !Time! is an average (in absolute nanoseconds) measured over a tight loop
//:   of 100,000 iterations.
//:
//: o !Size! is the size (in bytes) of a 'char *' of random input. Note that it
//:   uses IEC base2 notation (e.g. 1Ki = 2^10 = 1024, 1Mi = 2^20 = 1,048,576).
//
// 64-bit Default (Hardware Acceleration) vs. BDE's 'bdlde::crc32'
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//..
//  ===========================================================================
//  | Size(B) | Def time(ns) | bdlde::crc32 time(ns)| Ratio(bdlde::crc32 / Def)
//  ===========================================================================
//  |       11|             9|                    16|                     1.783
//  |       16|             9|                    28|                     3.089
//  |       21|            10|                    39|                     3.932
//  |       59|            13|                   136|                    10.323
//  |       64|            12|                   150|                    11.675
//  |       69|            13|                   161|                    11.669
//  |      251|            30|                   632|                    20.509
//  |      256|            30|                   640|                    20.660
//  |      261|            37|                   654|                    17.415
//  |     1019|           155|                  2588|                    16.698
//  |     1 Ki|            45|                  2602|                    57.324
//  |     1029|            50|                  2614|                    51.443
//  |     4091|           299|                 10436|                    34.865
//  |     4 Ki|           176|                 10448|                    59.040
//  |     4101|           190|                 10456|                    54.763
//  |    16379|           864|                 41806|                    48.366
//  |    16 Ki|           724|                 41829|                    57.721
//  |    16389|           754|                 40699|                    53.944
//  |    64 Ki|          2858|                162340|                    56.787
//  |   256 Ki|         11925|                654410|                    54.875
//  |     1 Mi|         50937|               2664898|                    52.317
//  |     4 Mi|        198662|              10562189|                    53.167
//  |    16 Mi|        796534|              42570294|                    53.444
//  |    64 Mi|       9976933|             169051561|                    16.944
//..
//
// 64-bit Software (SW) vs. BDE's 'bdlde::crc32'
// - - - - - - - - - - - - - - - - - - - - - - -
//..
//  ==========================================================================
//  | Size(B) | SW time(ns) | bdlde::crc32 time(ns) | Ratio(bdlde::crc32 / SW)
//  ==========================================================================
//  |       11|           13|                     17|                    1.229
//  |       16|           15|                     29|                    1.895
//  |       21|           26|                     39|                    1.500
//  |       59|           44|                    137|                    3.082
//  |       64|           43|                    150|                    3.428
//  |       69|           53|                    161|                    3.017
//  |      251|          158|                    629|                    3.977
//  |      256|          158|                    640|                    4.053
//  |      261|          173|                    654|                    3.777
//  |     1019|          621|                   2592|                    4.170
//  |     1 Ki|          621|                   2602|                    4.185
//  |     1029|          633|                   2614|                    4.128
//  |     4091|         2456|                  10435|                    4.248
//  |     4 Ki|         2457|                  10447|                    4.252
//  |     4101|         2464|                  10462|                    4.246
//  |    16379|         9795|                  41820|                    4.270
//  |    16 Ki|         9798|                  41838|                    4.270
//  |    16389|         9798|                  41846|                    4.271
//  |    64 Ki|        39222|                 167394|                    4.268
//  |   256 Ki|       156894|                 665589|                    4.242
//  |     1 Mi|       629828|                2656343|                    4.218
//  |     4 Mi|      2575623|               10601903|                    4.116
//  |    16 Mi|     10085862|               42775171|                    4.241
//  |    64 Mi|     40705975|              169682572|                    4.168
//..
//
// 64-bit Default vs. the 'serial' CRC32-C implementation
// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Compare the 64-bit Default to a hardware-based implementation for
// calculating CRC-32C in 64 bit mode (using SIMD - SSE 4.2) which issues the
// specialized hardware instruction in 'serial' over a loop (as opposed to
// 'triplet' -- three at a time).
//
// Both implementations ultimately leverage the same specialized hardware
// instructions for calculating CRC32-C (Intel SSE4.2 'crc32' instruction).
// However, there are differences in how fast they perform.  For sizes less
// than 1024B, the 'serial' implementation is faster.  For sizes equal to or
// larger than 1024B, the 'triplet' implementation is faster due to dividing
// the buffer into three non-overlapping parts and processing three CRC32
// calculations in parallel (see 'C(i)' in 'crc32c1024SseInt' in '.cpp' file or
// refer to the white paper linked in the implementation for
// 'crc32c1024SseInt').
//..
//  ========================================================================
//  | Size(B) | Default time(ns) | Serial time(ns) | Ratio(Serial / Triplet)
//  ========================================================================
//  |       11|                 9|                5|                   0.556
//  |       16|                 8|                4|                   0.500
//  |       21|                 9|                6|                   0.667
//  |       59|                12|                8|                   0.667
//  |       64|                12|                8|                   0.667
//  |       69|                13|               10|                   0.769
//  |      251|                30|               31|                   1.033
//  |      256|                32|               30|                   0.938
//  |      261|                37|               38|                   1.027
//  |     1019|               154|              153|                   0.994
//  |     1 Ki|                45|              151|                   3.356
//  |     1029|                50|              154|                   3.080
//  |     4091|               299|              634|                   2.120
//  |     4 Ki|               176|              636|                   3.614
//  |     4101|               187|              636|                   3.401
//  |    16379|               864|             2568|                   2.972
//  |    16 Ki|               724|             2567|                   3.546
//  |    16389|               751|             2572|                   3.425
//  |    64 Ki|              2947|            10296|                   3.494
//  |   256 Ki|             12275|            41235|                   3.359
//  |     1 Mi|             50918|           164927|                   3.239
//  |     4 Mi|            203426|           659671|                   3.243
//  |    16 Mi|            811794|          2638746|                   3.251
//  |    64 Mi|           7147904|         11091230|                   1.552
//..
//
///Performance (sparc)
///-------------------
// Below are software vs hardware performance comparison for different sparc
// CPUs:
//..
//  SPARC T5: 10.1465 times faster, at 710,138 iterations per second
//  SPARC T7: 10.1007 times faster, at 808,625 iterations per second
//  SPARC T8: 7.66392 times faster, at 1,013,937 iterations per second
//..
//-----------------------------------------------------------------------------
// CLASS METHODS
// [2] int Crc32c::calculate(const void *, size_t, unsigned int);
// [3] int Crc32c::calculate(const void *, size_t, unsigned int);
// [4] int Crc32c::calculate(const void *, size_t, unsigned int);
// [5] int Crc32c::calculate(const void *, size_t, unsigned int);
// [2] int Crc32c_Impl::calculateSoftware(const void *, size_t, uint);
// [3] int Crc32c_Impl::calculateSoftware(const void *, size_t, uint);
// [4] int Crc32c_Impl::calculateSoftware(const void *, size_t, uint);
// [6] int Crc32c_Impl::calculateSoftware(const void *, size_t, uint);
// [2] int Crc32c_Impl::calculateHardwareSerial(const void *, size_t, uint);
// [3] int Crc32c_Impl::calculateHardwareSerial(const void *, size_t, uint);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 7] USAGE EXAMPLE
// [-1] DEFAULT PERFORMANCE TEST
// [-2] SOFTWARE PERFORMANCE TEST
// [-3] THROUGPUT DEFAULT & SOFTWARE BENCHMARK
// [-4] DEFAULT & FOLLY PERFORMANCE TEST
// [-5] PERFORMANCE TEST ON USER INPUT
// ----------------------------------------------------------------------------

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------

namespace {

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

// ============================================================================
//                      STANDARD BDE TEST DRIVER MACROS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

static int                test;
static int             verbose;
static int         veryVerbose;
static int     veryVeryVerbose;
static int veryVeryVeryVerbose;
static bslma::TestAllocator *pa;

//=============================================================================
//              GLOBAL HELPER FUNCTIONS AND CLASSES FOR TESTING
//-----------------------------------------------------------------------------
namespace {

struct TableRecord {
    // Struct representing a record in a table of performance statistics.
    bsls::Types::Int64 d_size;
    bsls::Types::Int64 d_timeOne;
    bsls::Types::Int64 d_timeTwo;
    double             d_ratio;
};

static
void threadFunction(bsl::vector<unsigned int> *out,
                    const bsl::vector<char*>&  in,
                    bslmt::Barrier            *barrier,
                    bool                       useSoftware)
    // Thread function: wait on the specified barrier and then, for each buffer
    // in the specified 'in', calculate its crc32c value and store it in 'out'.
{
    out->reserve(in.size());
    barrier->wait();

    for (unsigned int i = 0; i < in.size(); ++i)
    {
        unsigned int crc32c;
        if (useSoftware) {
            crc32c = Crc32c_Impl::calculateSoftware(in[i], i + 1);
        }
        else {
            crc32c = Crc32c::calculate(in[i], i + 1);
        }
        out->push_back(crc32c);
    }
}

static
int populateBufferLengthsSorted(bsl::vector<int> *bufferLengths)
    // Populate the specified 'bufferLengths' with various lengths in
    // increasing sorted order.  Return the maximum length populated.  Note
    // that 'bufferLengths' will be cleared.
{
    BSLS_ASSERT(bufferLengths);

    bufferLengths->clear();

    bufferLengths->push_back(11);
    bufferLengths->push_back(16);       // 2^4
    bufferLengths->push_back(21);
    bufferLengths->push_back(59);
    bufferLengths->push_back(64);       // 2^7
    bufferLengths->push_back(69);
    bufferLengths->push_back(251);
    bufferLengths->push_back(256);      // 2^8
    bufferLengths->push_back(261);
    bufferLengths->push_back(1019);
    bufferLengths->push_back(1024);     // 2^10 = 1 Ki
    bufferLengths->push_back(1029);
    bufferLengths->push_back(4091);
    bufferLengths->push_back(4096);     // 2^12 = 4 Ki
    bufferLengths->push_back(4101);
    bufferLengths->push_back(16379);
    bufferLengths->push_back(16384);    // 16 Ki
    bufferLengths->push_back(16389);
    bufferLengths->push_back(65536);    // 64 Ki
    bufferLengths->push_back(262144);   // 256 Ki
    bufferLengths->push_back(1048576);  // 1 Mi
    bufferLengths->push_back(4194304);  // 4 Mi
    bufferLengths->push_back(16777216); // 16 Mi
    bufferLengths->push_back(67108864); // 64 Mi

    bsl::sort(bufferLengths->begin(), bufferLengths->end());

    return bufferLengths->back();
}

static
void printTableHeader(bsl::ostream&                   out,
                      const bsl::vector<bsl::string>& headers)
    // Print the specified 'headers' to the specified 'out' in the following
    // format:
    //
    //..
    //  ===================================================================
    //  | <headers[0]> | <headers[1]> | ... | <headers[headers.size() - 1]>
    //  ===================================================================
    //..
{
    const bsl::size_t sumSizes = bsl::accumulate(headers.begin(),
                                                 headers.end(),
                                                 bsl::string()).size();

    const bsl::size_t headerLineLength = sumSizes + (3 * headers.size() - 1);

    bsl::fill_n(bsl::ostream_iterator<char>(bsl::cout), headerLineLength, '=');
    bsl::cout << '\n';
    for (unsigned int col = 0; col < headers.size(); ++col)
    {
        out << "| " << headers[col];
        out << ((col == (headers.size() - 1)) ? '\n' : ' ');
    }
    bsl::fill_n(bsl::ostream_iterator<char>(bsl::cout), headerLineLength, '=');
    bsl::cout << '\n';
}

static
void printTableRows(bsl::ostream&                   out,
                    const bsl::vector<TableRecord>& tableRecords,
                    const bsl::vector<bsl::string>& headerCols)
    // Print the specified 'tableRecords' to the specified 'out', using the
    // specified 'headerCols' to determine the appropriate width for each
    // column.
    // Print in the following format:
    //
    //..
    //  | <size> | <timeOne> | <timeTwo> | <ratio>
    //..
{
    // PRECONDITIONS
    BSLS_ASSERT(headerCols.size() == 4);

    bsl::ios_base::fmtflags flags = out.flags();

    out << bsl::right << bsl::fixed;

    for (unsigned int i = 0; i < tableRecords.size(); ++i)
    {
        const TableRecord& record = tableRecords[i];

        // size
        out << "| "
            << bsl::setw(static_cast<int>(headerCols[0].size() + 1))
            << record.d_size;
        // timeOne
        out << "| "
            << bsl::setw(static_cast<int>(headerCols[1].size() + 1))
            << record.d_timeOne;
        // timeTwo
        out << "| "
            << bsl::setw(static_cast<int>(headerCols[2].size() + 1))
            << record.d_timeTwo;
        // ratio
        out << "| "
            << bsl::setw(static_cast<int>(headerCols[3].size()))
            << bsl::setprecision(3)
            << record.d_ratio;

        out << '\n';
    }

    out.flags(flags);
}

void printTable(bsl::ostream&                   out,
                const bsl::vector<bsl::string>& headerCols,
                const bsl::vector<TableRecord>& tableRecords)
    // Print a table having the specified 'headerCols' and 'tableRecords' to
    // the specified 'out'.
{
    printTableHeader(out, headerCols);
    printTableRows(out, tableRecords, headerCols);
}

// ============================================================================
//                                    TESTS
// ----------------------------------------------------------------------------

void test1_breathingTest()
    // ------------------------------------------------------------------------
    // BREATHING TEST
    //
    // Concerns:
    //: 1 Exercise the basic functionality of the component.
    //
    // Plan:
    //: 1 Calculate CRC32-C on a buffer and compare the result to the known
    //:   correct CRC32-C.
    //:
    //: 2 Calculate CRC32-C on a null buffer using the previous crc as a
    //:   starting point and verify that the result is the same as the
    //:   previous result.
    //:
    //: 3 Calculate CRC32-C on a buffer with length 0 using the previous crc
    //:   as a starting point and verify that the result is the same as the
    //:   previous crc.
    //:
    //: 4 Calculate CRC32-C on a prefix of a buffer and then on the rest of
    //:   the buffer using the result of the first calculation as starting
    //:   point and compare the final result to the known correct CRC32-C
    //
    // Testing:
    //   Basic functionality
    // ------------------------------------------------------------------------
{
    if (verbose) bsl::cout << bsl::endl
                           << "Breathing Test" << bsl::endl
                           << "==============" << bsl::endl;

    {
        if (veryVerbose) Q(BUFFER);

        const char         *BUFFER         = "12345678";
        const bsl::size_t   LENGTH         = strlen(BUFFER);
        const unsigned int  EXPECTED       = 0x6087809A;
        unsigned int        crc32cDefault  = 0;
        unsigned int        crc32cSoftware = 0;
        unsigned int        crc32cHWSerial = 0;

        // Default
        crc32cDefault  = Crc32c::calculate(BUFFER, LENGTH);

        // Software
        crc32cSoftware = Crc32c_Impl::calculateSoftware(BUFFER, LENGTH);

        // Hardware Serial
        crc32cHWSerial = Crc32c_Impl::calculateHardwareSerial(BUFFER, LENGTH);

        ASSERTV(L_, crc32cDefault,  EXPECTED, crc32cDefault  == EXPECTED);
        ASSERTV(L_, crc32cSoftware, EXPECTED, crc32cSoftware == EXPECTED);
        ASSERTV(L_, crc32cHWSerial, EXPECTED, crc32cHWSerial == EXPECTED);
    }

    {
        if (veryVerbose) Q(NULL BUFFER);

        // Test edge case of null buffer and length = 0
        const unsigned int EXPECTED       = 0;
        unsigned int       crc32cDef      = 0;
        unsigned int       crc32cSW       = 0;
        unsigned int       crc32cHWSerial = 0;
        // Default
        crc32cDef = Crc32c::calculate(0, 0);

        // Software
        crc32cSW = Crc32c_Impl::calculateSoftware(0, 0);

        /// Hardware Serial
        crc32cHWSerial = Crc32c_Impl::calculateHardwareSerial(0,0);

        ASSERTV(L_, crc32cDef,      EXPECTED, crc32cDef      == EXPECTED);
        ASSERTV(L_, crc32cSW,       EXPECTED, crc32cSW       == EXPECTED);
        ASSERTV(L_, crc32cHWSerial, EXPECTED, crc32cHWSerial == EXPECTED);
    }

    {
        if (veryVerbose) Q(LENGTH PARAMETER EQUAL TO 0);

        const char   *BUFFER         = "12345678";
        bsl::size_t   LENGTH         = 0;
        unsigned int  EXPECTED       = 0; // because LENGTH = 0
        unsigned int  crc32cDefault  = 0;
        unsigned int  crc32cSoftware = 0;
        unsigned int  crc32cHWSerial = 0;

        // Default
        crc32cDefault = Crc32c::calculate(BUFFER, LENGTH);

        // Software
        crc32cSoftware = Crc32c_Impl::calculateSoftware(BUFFER, LENGTH);

        // Hardware Serial
        crc32cHWSerial = Crc32c_Impl::calculateHardwareSerial(BUFFER, LENGTH);

        ASSERTV(L_, crc32cDefault,  EXPECTED, crc32cDefault  == EXPECTED);
        ASSERTV(L_, crc32cSoftware, EXPECTED, crc32cSoftware == EXPECTED);
        ASSERTV(L_, crc32cHWSerial, EXPECTED, crc32cHWSerial == EXPECTED);

        // Compute value for whole BUFFER
        LENGTH         = strlen(BUFFER);
        EXPECTED       = 0x6087809A;
        crc32cDefault  = 0;
        crc32cSoftware = 0;
        crc32cHWSerial = 0;

        // Default
        crc32cDefault  = Crc32c::calculate(BUFFER, LENGTH);

        // Software
        crc32cSoftware = Crc32c_Impl::calculateSoftware(BUFFER, LENGTH);

        // Hardware Serial
        crc32cHWSerial = Crc32c_Impl::calculateHardwareSerial(BUFFER, LENGTH);

        ASSERTV(L_, crc32cDefault,  EXPECTED, crc32cDefault  == EXPECTED);
        ASSERTV(L_, crc32cSoftware, EXPECTED, crc32cSoftware == EXPECTED);
        ASSERTV(L_, crc32cHWSerial, EXPECTED, crc32cHWSerial == EXPECTED);

        // Now specify LENGTH 0 and pass the previous crc and expect the
        // previous crc to be the result.
        LENGTH  = 0;

        unsigned int prevCrc = crc32cDefault;

        // Default
        crc32cDefault = Crc32c::calculate(BUFFER, LENGTH, prevCrc);

        // Software
        crc32cSoftware = Crc32c_Impl::calculateSoftware(BUFFER,
                                                        LENGTH,
                                                        prevCrc);

        // Hardware Serial
        crc32cHWSerial = Crc32c_Impl::calculateHardwareSerial(BUFFER,
                                                              LENGTH,
                                                              prevCrc);

        ASSERTV(L_, crc32cDefault,  EXPECTED, crc32cDefault  == EXPECTED);
        ASSERTV(L_, crc32cSoftware, EXPECTED, crc32cSoftware == EXPECTED);
        ASSERTV(L_, crc32cHWSerial, EXPECTED, crc32cHWSerial == EXPECTED);
    }

    {
        if (veryVerbose) Q(SPLIT BUFFER WITH PREVIOUS CRC)
        // Prefix of a BUFFER, then the rest using the previous crc as the
        // starting point
        const char         *BUFFER         = "12345678";
        const unsigned int  EXPECTED       = 0x6087809A;
        unsigned int        crc32cDefault  = 0;
        unsigned int        crc32cSoftware = 0;
        unsigned int        crc32cHWSerial = 0;

        const unsigned int PREFIX_LENGTH = 3;

        // Default
        crc32cDefault = Crc32c::calculate(BUFFER, PREFIX_LENGTH);

        // Software
        crc32cSoftware = Crc32c_Impl::calculateSoftware(BUFFER,
                                                        PREFIX_LENGTH);

        // Hardware Serial
        crc32cHWSerial = Crc32c_Impl::calculateHardwareSerial(BUFFER,
                                                              PREFIX_LENGTH);

        const bsl::size_t REST_LENGTH = strlen(BUFFER) - PREFIX_LENGTH;

        // Default
        crc32cDefault = Crc32c::calculate(BUFFER + PREFIX_LENGTH,
                                          REST_LENGTH,
                                          crc32cDefault);
        // Software
        crc32cSoftware = Crc32c_Impl::calculateSoftware(BUFFER + PREFIX_LENGTH,
                                                        REST_LENGTH,
                                                        crc32cSoftware);

        // Hardware Serial
        crc32cHWSerial = Crc32c_Impl::calculateHardwareSerial(
                                                        BUFFER + PREFIX_LENGTH,
                                                        REST_LENGTH,
                                                        crc32cHWSerial);

        ASSERTV(L_, crc32cDefault,  EXPECTED, crc32cDefault  == EXPECTED);
        ASSERTV(L_, crc32cSoftware, EXPECTED, crc32cSoftware == EXPECTED);
        ASSERTV(L_, crc32cHWSerial, EXPECTED, crc32cHWSerial == EXPECTED);
    }
}

void test2_calculateOnBuffer()
    // ------------------------------------------------------------------------
    // CALCULATE CRC32-C ON BUFFER
    //
    // Concerns:
    //: 1 Verify the correctness of computing CRC32-C on a buffer w/o a
    //:   previous CRC (which would have to be taken into account if it were
    //:   provided) using both the default and software implementations.
    //:
    //: 2 Ensure that there are no CRC collisions for possible combination of
    //:   1, 2 bytes.
    //:
    //: 3 QoI: Asserted precondition violations are detected when enabled.
    //
    // Plan:
    //: 1 Calculate CRC32-C for various buffers and compare the results to the
    //:   known correct CRC32-C values for these buffers.
    //:
    //: 2 For C-2 create an integer array using 'bsl::vector<int>' and fill it
    //: as follows:
    //:   2.1 Loop through all the possible values for an 'unsigned char'
    //:       (0 .. 255) and add its CRC32-C value to the array.
    //:   2.2 Then loop through all the possible values for a 16-bit
    //:       'unsigned short' (0 .. 65535) and add its CRC32-C value to the
    //:       array.
    //:   2.3 Again, loop through all the possible values for a 16-bit
    //:       'unsigned short', but insert a 0-valued byte in between the
    //:       2 bytes.  Calculate the CRC32-C and add it to the array.
    //:   2.4 Repeat step (P-2.3), but insert a 1-valued byte instead of a
    //:       0-valued byte.
    //:   2.5 Sort the array, then assert that each CRC value in the array
    //:       (except for the 0th array element) differs from the CRC value
    //:       immediately preceding it in the array.
    //
    // Testing:
    //   bdlde::Crc32c::calculate(const void *, size_t, unsigned int);
    //   bdlde::Crc32c_Impl::calculateSoftware(const void *, size_t, uint);
    //   bdlde::Crc32c_Impl::calculateHardwareSerial(const void *,size_t,uint);
    // ------------------------------------------------------------------------
{
    if (verbose) bsl::cout << bsl::endl
                           << "CALCULATE CRC32-C ON BUFFER" << bsl::endl
                           << "===========================" << bsl::endl;

    struct {
        int          d_line;
        const char  *d_buffer;
        unsigned int d_expectedCrc32c;
    } DATA[] = {
        //--------------------------------------
        // LINE | BUFFER          | EXPECTED
        //--------------------------------------
        { L_,     "",               0          },
        { L_,     "DYB|O",          0          },
        { L_,     "0",              0x629E1AE0 },
        { L_,     "1",              0x90F599E3 },
        { L_,     "2",              0x83A56A17 },
        { L_,     "~",              0x8F9DB87B },
        { L_,     "22",             0x47B26CF9 },
        { L_,     "fa",             0x8B9F1387 },
        { L_,     "t0-",            0x77E2D1A9 },
        { L_,     "34v}",           0x031AD8A7 },
        { L_,     "shaii",          0xB0638FB5 },
        { L_,     "3jf-_3",         0xE186B745 },
        { L_,     "bonjour",        0x156088D2 },
        { L_,     "vbPHbvtB",       0x12AAFAA6 },
        { L_,     "aoDicgezd",      0xBF5E01C8 },
        { L_,     "123456789",      0xe3069283 },
        { L_,     "gaaXsSP1al",     0xC4E61D23 },
        { L_,     "2Wm9bbNDehd",    0x54A11873 },
        { L_,     "GamS0NJhAl8y",   0x0044AC66 }
    };
    const bsl::size_t NUM_DATA = sizeof(DATA) / sizeof(*DATA);

    for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti)
    {
        const int           LINE     = DATA[ti].d_line;
        const char         *BUFFER   = DATA[ti].d_buffer;
        const bsl::size_t   LENGTH   = strlen(BUFFER);
        const unsigned int  EXPECTED = DATA[ti].d_expectedCrc32c;

        if (veryVerbose) {
            T_  P(BUFFER);
        }

        // Default
        unsigned int crc32cDefault  = Crc32c::calculate(BUFFER, LENGTH);

        // Software
        unsigned int crc32cSoftware = Crc32c_Impl::calculateSoftware(BUFFER,
                                                                     LENGTH);
        // HW Serial
        unsigned int crc32cHWSerial = Crc32c_Impl::calculateHardwareSerial(
                                                                       BUFFER,
                                                                       LENGTH);

        // Verify correctness
        LOOP3_ASSERT(LINE, crc32cDefault,
                           EXPECTED,       crc32cDefault  == EXPECTED);
        LOOP3_ASSERT(LINE, crc32cSoftware,
                           EXPECTED,       crc32cSoftware == EXPECTED);
        LOOP3_ASSERT(LINE, crc32cHWSerial,
                           EXPECTED,       crc32cHWSerial == EXPECTED);

        // Test edge case of non-null buffer and LENGTH = 0

        // Default
        crc32cDefault  = Crc32c::calculate(BUFFER, 0);

        // Software
        crc32cSoftware = Crc32c_Impl::calculateSoftware(BUFFER, 0);

        // Hardware Serial
        crc32cHWSerial = Crc32c_Impl::calculateHardwareSerial(BUFFER, 0);

        LOOP2_ASSERT(LINE, crc32cDefault,  crc32cDefault  == 0u);
        LOOP2_ASSERT(LINE, crc32cSoftware, crc32cSoftware == 0u);
        LOOP2_ASSERT(LINE, crc32cHWSerial, crc32cHWSerial == 0u);
    }

    // P-2
    if (veryVerbose) cout << "\tCollision test." << endl;
    {
        bsl::vector<int> a(pa);
        bsl::vector<int> b(pa);
        bsl::vector<int> c(pa);

        // one byte
        for (int i = 0; i < 256; ++i) {
            unsigned char data = (unsigned char)i;
            a.push_back(Crc32c::calculate(&data, 1));
            b.push_back(Crc32c_Impl::calculateSoftware(&data, 1));
            c.push_back(Crc32c_Impl::calculateHardwareSerial(&data, 1));
        }

        // two bytes
        for (int i = 0; i < 65536; ++i) {
            unsigned char data[2];
            data[0] = (unsigned char)(i / 256);
            data[1] = (unsigned char)(i % 256);
            a.push_back(Crc32c::calculate(&data, 2));
            b.push_back(Crc32c_Impl::calculateSoftware(&data, 2));
            c.push_back(Crc32c_Impl::calculateHardwareSerial(&data, 2));
        }

        // two bytes with a zero separator
        for (int i = 0; i < 65536; ++i) {
            unsigned char data[3];
            data[0] = (unsigned char)(i / 256);
            data[1] = 0;
            data[2] = (unsigned char)(i % 256);
            a.push_back(Crc32c::calculate(&data, 3));
            b.push_back(Crc32c_Impl::calculateSoftware(&data, 3));
            c.push_back(Crc32c_Impl::calculateHardwareSerial(&data, 3));
        }

        // two bytes with a one separator
        for (int i = 0; i < 65536; ++i) {
            unsigned char data[3];
            data[0] = (unsigned char)(i / 256);
            data[1] = 1;
            data[2] = (unsigned char)(i % 256);
            a.push_back(Crc32c::calculate(&data, 3));
            b.push_back(Crc32c_Impl::calculateSoftware(&data, 3));
            c.push_back(Crc32c_Impl::calculateHardwareSerial(&data, 3));
        }

        bsl::sort(a.begin(), a.end());
        for (unsigned int i = 1; i < a.size(); ++i) {
            ASSERT(a[i] != a[i - 1]);
        }
        bsl::sort(b.begin(), b.end());
        for (unsigned int i = 1; i < b.size(); ++i) {
            ASSERT(b[i] != b[i - 1]);
        }
        bsl::sort(c.begin(), c.end());
        for (unsigned int i = 1; i < c.size(); ++i) {
            ASSERT(c[i] != c[i - 1]);
        }
    }

    if (veryVerbose) cout << "\tNegative testing." << endl;
    {
        bsls::AssertTestHandlerGuard hG;
        bsl::size_t VALID   = 0;
        bsl::size_t INVALID = 100;

        ASSERT_PASS(0 == Crc32c::calculate(0, VALID));
        ASSERT_FAIL(0 == Crc32c::calculate(0, INVALID));

        ASSERT_PASS(0 == Crc32c_Impl::calculateSoftware(0, VALID));
        ASSERT_FAIL(0 == Crc32c_Impl::calculateSoftware(0, INVALID));

        ASSERT_PASS(0 == Crc32c_Impl::calculateHardwareSerial(0, VALID));
        ASSERT_FAIL(0 == Crc32c_Impl::calculateHardwareSerial(0, INVALID));
    }
}

void test3_calculateOnMisalignedBuffer()
    // ------------------------------------------------------------------------
    // CALCULATE CRC32-C ON MISALIGNED BUFFER
    //
    // Concerns:
    //: 1 Ensure that calculating CRC32-C on a buffer that is not at an
    //:   alignment boundary yields the same result as for one that is.
    //:
    //: 2 Ensure that CRC32-C is calculated on a buffer of the specified
    //:   length.
    //
    // Plan:
    //: 1 Generate buffers at an alignment boundary consisting of '1 <= i <= 7'
    //:   characters preceding the test buffers from the previous previous test
    //:   cases and calculate CRC32-C for the test buffers at 'i' bytes from
    //:   the beginning of the aligned buffer.
    //
    // Testing:
    //   bdlde::Crc32c::calculate(const void *, size_t, unsigned int);
    //   bdlde::Crc32c_Impl::calculateSoftware(const void *, size_t, uint);
    //   bdlde::Crc32c_Impl::calculateHardwareSerial(const void *,size_t,uint);
    // ------------------------------------------------------------------------
{
    if (verbose) bsl::cout
                     << bsl::endl
                     << "CALCULATE CRC32-C ON MISALIGNED BUFFER" << bsl::endl
                     << "======================================" << bsl::endl;

    const int k_BUFFER_SIZE  = 1024;
    const int k_MY_ALIGNMENT =
           bsls::AlignmentFromType<bsls::AlignmentUtil::MaxAlignedType>::VALUE;

    bsls::AlignedBuffer<k_BUFFER_SIZE, k_MY_ALIGNMENT> allocBuffer;
        // Aligned buffer sufficiently large to contain every buffer generated
        // for testing below

    char *allocPtr = allocBuffer.buffer();

    // Sanity check: test.d_buffer at alignment boundary
    BSLS_ASSERT_OPT(bsls::AlignmentUtil::calculateAlignmentOffset(
                                                                allocPtr,
                                                                k_MY_ALIGNMENT)
                    == 0);

    struct {
        int          d_line;
        const char  *d_buffer;
        unsigned int d_expectedCrc32c;
    } DATA[] = {
        { L_, "",               0},
        { L_, "DYB|O",          0},
        { L_, "0",              0x629E1AE0},
        { L_, "1",              0x90F599E3},
        { L_, "2",              0x83A56A17},
        { L_, "~",              0x8F9DB87B},
        { L_, "22",             0x47B26CF9},
        { L_, "fa",             0x8B9F1387},
        { L_, "t0-",            0x77E2D1A9},
        { L_, "34v}",           0x031AD8A7},
        { L_, "shaii",          0xB0638FB5},
        { L_, "3jf-_3",         0xE186B745},
        { L_, "bonjour",        0x156088D2},
        { L_, "vbPHbvtB",       0x12AAFAA6},
        { L_, "aoDicgezd",      0xBF5E01C8},
        { L_, "123456789",      0xe3069283},
        { L_, "gaaXsSP1al",     0xC4E61D23},
        { L_, "2Wm9bbNDehd",    0x54A11873},
        { L_, "GamS0NJhAl8y",   0x0044AC66}
    };

    const bsl::size_t NUM_DATA = sizeof(DATA) / sizeof(*DATA);

    for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {

        const int           LINE     = DATA[ti].d_line;
        const char         *BUFFER   = DATA[ti].d_buffer;
        const bsl::size_t   LENGTH   = strlen(BUFFER);
        const unsigned int  EXPECTED = DATA[ti].d_expectedCrc32c;

        if (veryVerbose) {
            T_  P(BUFFER);
        }
        for (unsigned int i = 1; i < k_MY_ALIGNMENT; ++i)
        {
            const bsl::size_t TAIL_LENGTH = 10;
            const bsl::size_t NEW_LENGTH = LENGTH + i + TAIL_LENGTH;

            // Sanity check: aligned buffer sufficiently large for some prefix
            //               padding plus the test data, and the test data is
            //               *not* starting at an alignment boundary inside the
            //               aligned buffer.
            BSLS_ASSERT_OPT(k_BUFFER_SIZE >= NEW_LENGTH);
            BSLS_ASSERT_OPT(bsls::AlignmentUtil::calculateAlignmentOffset(
                                                                allocPtr + i,
                                                                k_MY_ALIGNMENT)
                            != 0);

            // Create a C-string with 'i' characters preceding the test buffer:
            // 'XX...X<test.d_bufer>XX...X'
            bsl::memset(allocPtr, 'X', NEW_LENGTH);
            allocPtr[NEW_LENGTH - 1] = '\0';
            bsl::memcpy(allocPtr + i, BUFFER, LENGTH);

            if (veryVeryVerbose) {
                T_ T_;
                bsl::cout <<  '[' << bsl::hex
                          << static_cast<void *>(allocPtr + i) << "], " << i
                          << " bytes past an alignment boundary: "
                          << allocPtr << bsl::endl;
            }
            // Default
            unsigned int crc32cDefault = Crc32c::calculate(allocPtr + i,
                                                           LENGTH);
            // Software
            unsigned int crc32cSoftware = Crc32c_Impl::calculateSoftware(
                                                                allocPtr + i,
                                                                LENGTH);

            // Hardware Serial
            unsigned int crc32cHWSerial =
                           Crc32c_Impl::calculateHardwareSerial(allocPtr + i,
                                                                LENGTH);

            // Verify correctness
            LOOP3_ASSERT(LINE, crc32cDefault,
                               EXPECTED,       crc32cDefault  == EXPECTED);
            LOOP3_ASSERT(LINE, crc32cSoftware,
                               EXPECTED,       crc32cSoftware == EXPECTED);
            LOOP3_ASSERT(LINE, crc32cHWSerial,
                               EXPECTED,       crc32cHWSerial == EXPECTED);
        }
    }
}

void test4_calculateOnBufferWithPreviousCrc()
    // ------------------------------------------------------------------------
    // CALCULATE CRC32-C ON BUFFER WITH PREVIOUS CRC
    //
    // Concerns:
    //: 1 Ensure that calculating CRC32-C on a buffer with previous crc results
    //:   in the expected value.
    //
    // Plan:
    //: 1 For various buffers, calculate CRC32-C for a prefix chunk of the
    //:   buffer, and pass the calculated CRC32-C as a parameter ('crc') for
    //:   calculating the final CRC32-C value.  Compare the final CRC32-C
    //:   values to the known correct crc32c values for these buffers.
    //
    // Testing:
    //   bdlde::Crc32c::calculate(const void *, size_t, unsigned int);
    //   bdlde::Crc32c_Impl::calculateSoftware(const void *, size_t, uint);
    // ------------------------------------------------------------------------
{
    if (verbose) bsl::cout
               << bsl::endl
               << "CALCULATE CRC32-C ON BUFFER WITH PREVIOUS CRC" << bsl::endl
               << "=============================================" << bsl::endl;

    struct {
        int           d_line;
        const char   *d_buffer;
        bsl::size_t   d_prefixLen;
        unsigned int  d_expectedCrc32c;
    } DATA[] = {
        // Note that for d_buffer[0:d_prefixLen] (the prefix string ) we
        // already asserted that we calculated the correct CRC32-C values in
        // test case 2. So this data is suitable for checking that
        // incorporating another buffer into the CRC32-C of the prefix behaves
        // correctly.
        { L_, "",                   0,  0},
        { L_, "DYB|O--",            5,  0xD1436CCE},
        { L_, "0sef",               1,  0x50588062},
        { L_, "13",                 1,  0x813E4763},
        { L_, "2s34faw",            1,  0xED5E0C1C},
        { L_, "~ahaer",             1,  0x45F10742},
        { L_, "22aasd",             2,  0x22B28122},
        { L_, "faghar",             2,  0xD9253928},
        { L_, "t0-aavk",            3,  0x8A752D3F},
        { L_, "34v}acv",            4,  0xC36C7D1D},
        { L_, "shaiig5bg",          5,  0x9E26CF81},
        { L_, "123456789",          9,  0xe3069283},
        { L_, "3jf-_3adfg",         6,  0xEDA627B3},
        { L_, "bonjour421h",        7,  0xD23EF1DF},
        { L_, "vbPHbvtB45gga",      8,  0xFCC29260},
        { L_, "aoDicgezd==7h",      9,  0x171D042A},
        { L_, "gaaXsSP1aldsafad",   10, 0xFD5078EF},
        { L_, "2Wm9bbNDehd32qf",    11, 0x9F7277C6},
        { L_, "GamS0NJhAl8yw3th",   12, 0x6033D909}
    };

    const bsl::size_t NUM_DATA = sizeof(DATA) / sizeof(*DATA);

    for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti)
    {
        const int           LINE       = DATA[ti].d_line;
        const char         *BUFFER     = DATA[ti].d_buffer;
        const bsl::size_t   PREFIX_LEN = DATA[ti].d_prefixLen;
        const bsl::size_t   SUFFIX_LEN = strlen(BUFFER) - PREFIX_LEN;
        const unsigned int  EXPECTED   = DATA[ti].d_expectedCrc32c;

        if (veryVerbose) {
            T_  P(BUFFER);
        }

        // Default
        unsigned int crc32cDefault = Crc32c::calculate(BUFFER, PREFIX_LEN);

        crc32cDefault = Crc32c::calculate(BUFFER + PREFIX_LEN,
                                          SUFFIX_LEN,
                                          crc32cDefault);

        LOOP3_ASSERT(LINE, crc32cDefault, EXPECTED, crc32cDefault == EXPECTED);

        // Software
        unsigned int crc32cSoftware = Crc32c_Impl::calculateSoftware(
                                                                   BUFFER,
                                                                   PREFIX_LEN);

        crc32cSoftware = Crc32c_Impl::calculateSoftware(BUFFER + PREFIX_LEN,
                                                        SUFFIX_LEN,
                                                        crc32cSoftware);

        LOOP3_ASSERT(LINE, crc32cSoftware,
                           EXPECTED,       crc32cSoftware == EXPECTED);

        // Test edge case where non-null buffer and length = 0 with previous
        // CRC returns the previous CRC
        const unsigned int previousCrc = crc32cDefault;

        // Default
        crc32cDefault = Crc32c::calculate(BUFFER, 0, previousCrc);

        LOOP3_ASSERT(LINE, crc32cDefault,
                           previousCrc,   crc32cDefault == previousCrc);

        // Software
        crc32cSoftware = Crc32c_Impl::calculateSoftware(BUFFER,
                                                        0,
                                                        previousCrc);

        LOOP3_ASSERT(LINE, crc32cSoftware,
                           previousCrc,    crc32cSoftware == previousCrc);

        // Test edge case where null buffer and length = 0 with previous
        // CRC returns the previous CRC
        // Note: We could have not put this test block inside the loop but
        //       we might as well because it tells us that a null buffer with a
        //       length of 0 will yield the previous CRC for multiple different
        //       previous CRCs (not just one or a couple).

        // Default
        crc32cDefault = Crc32c::calculate(0, 0, previousCrc);

        LOOP3_ASSERT(LINE, crc32cDefault,
                           previousCrc,   crc32cDefault == previousCrc);

        // Software
        crc32cSoftware = Crc32c_Impl::calculateSoftware(0,
                                                        0,
                                                        previousCrc);

        LOOP3_ASSERT(LINE, crc32cSoftware,
                           previousCrc,    crc32cSoftware == previousCrc);
    }
}

void test5_multithreadedCrc32cDefault()
    // ------------------------------------------------------------------------
    // MULTITHREAD DEFAULT CRC32-C
    //
    // Concerns:
    //: 1 Test bdlde::Crc32c::calculate(const void *data, unsigned int length)
    //:   in a multi-threaded environment, making sure that each CRC32-C is
    //:   computed correctly across all threads when they perform the
    //:   calculation concurrently.
    //
    // Plan:
    //: 1 Generate a large set of random payloads of different sizes.
    //:
    //: 2 In serial, calculate the CRC32-C value for each of these payloads.
    //:
    //: 3 Spawn a few threads and have them calculate the CRC32-C value for
    //:   each payload. Once they are done, make sure all calculated CRC32-C
    //:   values from the threads match the CRC32-C values calculated in
    //:   serial.
    //
    // Testing:
    //   bdlde::Crc32c::calculate(const void *, size_t, unsigned int);
    // ------------------------------------------------------------------------
{
    if (verbose) bsl::cout << bsl::endl
                           << "MULTITHREAD DEFAULT CRC32-C" << bsl::endl
                           << "===========================" << bsl::endl;

    enum {
        k_NUM_PAYLOADS = 10000,
        k_NUM_THREADS  = 10
    };

    // Input buffers
    bsl::vector<char *> payloads(pa);
    payloads.reserve(k_NUM_PAYLOADS);

    // Populate with random data
    for (unsigned int i = 0; i < k_NUM_PAYLOADS; ++i)
    {
        char *buffer = static_cast<char *> (pa->allocate(i + 1));
        bsl::generate_n(buffer, i + 1, bsl::rand);
        payloads.push_back(buffer);
    }

    // [1] Serial calculation
    bsl::vector<unsigned int> serialCrc32cData(pa);
    serialCrc32cData.reserve(k_NUM_PAYLOADS);
    for (unsigned int i = 0; i < k_NUM_PAYLOADS; ++i)
    {
        const unsigned int crc32c = Crc32c::calculate(payloads[i], i + 1);
        serialCrc32cData.push_back(crc32c);
    }

    // [2] Multithreaded Calculation
    bslmt::ThreadGroup threadGroup(pa);
    bslmt::Barrier     barrier(k_NUM_THREADS + 1);
        // Barrier to get each thread to start at the same time; '+1' for this
        // (main) thread.

    bsl::vector<bsl::vector<unsigned int> > threadsCrc32cData(k_NUM_THREADS,
                                                              pa);
    for (unsigned int i  = 0; i < k_NUM_THREADS; ++i)
    {
        int rc = threadGroup.addThread(bdlf::BindUtil::bind(
                                                         &threadFunction,
                                                         &threadsCrc32cData[i],
                                                         payloads,
                                                         &barrier,
                                                         false));
        BSLS_ASSERT_OPT(rc == 0);
    }

    barrier.wait();
    threadGroup.joinAll();

    // Compare CRC32-C: serial result vs. each thread's result
    for (unsigned int i = 0; i < k_NUM_THREADS; ++i)
    {
        const bsl::vector<unsigned int>& currThreadCrc32cData =
                                                          threadsCrc32cData[i];
        for (unsigned int j = 0; j < k_NUM_PAYLOADS; ++j)
        {
            LOOP4_ASSERT(i, j, serialCrc32cData[j],   currThreadCrc32cData[j],
                               serialCrc32cData[j] == currThreadCrc32cData[j]);
        }
    }

    // Delete allocated payloads
    for (unsigned int i = 0; i < k_NUM_PAYLOADS; ++i) {
        pa->deallocate(payloads[i]);
    }
}

void test6_multithreadedCrc32cSoftware()
    // ------------------------------------------------------------------------
    // MULTITHREAD SOFTWARE CRC32-C
    //
    // Concerns:
    //: 1 Test 'bdlde::Crc32c_Impl::calculateSoftware(const void *, size_t)'
    //:   in a multi-threaded environment, making sure that each CRC32-C is
    //:   computed correctly across all threads when they perform the
    //:   calculation concurrently.
    //
    // Plan:
    //: 1 Generate a large set of random payloads of different sizes.
    //:
    //: 2 In serial, calculate the CRC32-C value for each of these payloads.
    //:
    //: 3 Spawn a few threads and have them calculate the CRC32-C value for
    //:   each payload using the software-based utility. Once they are done,
    //:   make sure all calculated CRC32-C values from the threads match the
    //:   CRC32-C values calculated in serial.
    //
    // Testing:
    //   bdlde::Crc32c_Impl::calculateSoftware(const void *, size_t, uint);
    // ------------------------------------------------------------------------
{
    if (verbose) bsl::cout << bsl::endl
                           << "MULTITHREAD SOFTWARE CRC32-C" << bsl::endl
                           << "============================" << bsl::endl;

    enum {
        k_NUM_PAYLOADS = 10000,
        k_NUM_THREADS  = 10
    };

    // Input buffers
    bsl::vector<char *> payloads(pa);
    payloads.reserve(k_NUM_PAYLOADS);

    // Populate with random data
    for (unsigned int i = 0; i < k_NUM_PAYLOADS; ++i)
    {
        char *buffer = static_cast<char *> (pa->allocate(i + 1));
        bsl::generate_n(buffer, i + 1, bsl::rand);
        payloads.push_back(buffer);
    }

    // [1] Serial calculation
    bsl::vector<unsigned int> serialCrc32cData(pa);
    serialCrc32cData.reserve(k_NUM_PAYLOADS);
    for (unsigned int i = 0; i < k_NUM_PAYLOADS; ++i)
    {
        const unsigned int crc32c = Crc32c_Impl::calculateSoftware(payloads[i],
                                                                   i + 1);
        serialCrc32cData.push_back(crc32c);
    }

    // [2] Multithreaded Calculation
    bslmt::ThreadGroup threadGroup(pa);
    bslmt::Barrier      barrier(k_NUM_THREADS + 1);
        // Barrier to get each thread to start at the same time; '+1' for this
        // (main) thread.

    bsl::vector<bsl::vector<unsigned int> > threadsCrc32cData(pa);
    threadsCrc32cData.resize(k_NUM_THREADS);
    for (unsigned int i  = 0; i < k_NUM_THREADS; ++i)
    {
        int rc = threadGroup.addThread(bdlf::BindUtil::bind(
                                                         &threadFunction,
                                                         &threadsCrc32cData[i],
                                                         payloads,
                                                         &barrier,
                                                         true));
        BSLS_ASSERT_OPT(rc == 0);
    }

    barrier.wait();
    threadGroup.joinAll();

    // Compare crc32c: serial result vs. each thread's result
    for (unsigned int i = 0; i < k_NUM_THREADS; ++i)
    {
        const bsl::vector<unsigned int>& currThreadCrc32cData =
                                                          threadsCrc32cData[i];
        for (unsigned int j = 0; j < k_NUM_PAYLOADS; ++j) {
            LOOP4_ASSERT(i, j, serialCrc32cData[j],   currThreadCrc32cData[j],
                               serialCrc32cData[j] == currThreadCrc32cData[j]);
        }
    }

    // Delete allocated payloads
    for (unsigned int i = 0; i < k_NUM_PAYLOADS; ++i) {
        pa->deallocate(payloads[i]);
    }
}

// ============================================================================
//                              PERFORMANCE TESTS
// ----------------------------------------------------------------------------

void testN1_performanceDefault()
    // ------------------------------------------------------------------------
    // PERFORMANCE: CALCULATE CRC32-C ON BUFFER DEFAULT
    //
    // Concerns:
    //: 1 Test the performance of bdlde::Crc32c::calculate(const void *,
    //:                                                    unsigned int);
    //:   in a single thread environment.  On a supported platform (see
    //:   'Support for Hardware Acceleration' in the header file), this will
    //:   use a hardware-accelerated implementation.  Otherwise, it will use a
    //:   portable software implementation.
    //
    // Plan:
    //: 1 Time a large number of CRC32-C calculations for buffers of varying
    //:   sizes in a single thread and take the average.  Compare the average
    //:   to that of 'bdlde::crc32'.
    //
    // Testing:
    //   bdlde::Crc32c::calculate(const void *, size_t int, unsigned int);
    // ------------------------------------------------------------------------
{
    if (verbose) bsl::cout << bsl::endl
            << "PERFORMANCE: CALCULATE CRC32-C ON BUFFER DEFAULT" << bsl::endl
            << "================================================" << bsl::endl;

    const int k_NUM_ITERS = 100000; // 100K

    bsl::vector<int> bufferLengths(pa);
    const int        k_MAX_SIZE = populateBufferLengthsSorted(&bufferLengths);

    // Read in random input
    char *buffer = static_cast<char *>(pa->allocate(k_MAX_SIZE));
    bsl::generate_n(buffer, k_MAX_SIZE, bsl::rand);

    // Measure calculation time and report
    bsl::vector<TableRecord> tableRecords(pa);
    for (unsigned i = 0; i < bufferLengths.size(); ++i) {
        const int length = bufferLengths[i];

        if (veryVerbose) bsl::cout
                             << "---------------------"         << '\n'
                             << " SIZE = " << length            << '\n'
                             << " ITERATIONS = " << k_NUM_ITERS << '\n'
                             << "---------------------"         << bsl::endl;

        //===================================================================//
        //                        [1] Crc32c (default)
        unsigned int crc32c = Crc32c::calculate(buffer, length);

        // <time>
        bsls::Types::Int64 startTime = bsls::TimeUtil::getTimer();
        for (unsigned int l = 0; l < k_NUM_ITERS; ++l) {
            crc32c = Crc32c::calculate(buffer, length);
        }
        bsls::Types::Int64 t1 = bsls::TimeUtil::getTimer() - startTime;
        // </time>

        //===================================================================//
        //                         [2] BDE bdlde::crc32
        bdlde::Crc32 crcBde(buffer, length);
        unsigned int crc32 = crcBde.checksumAndReset();

        // <time>
        startTime = bsls::TimeUtil::getTimer();
        for (unsigned int l = 0; l < k_NUM_ITERS; ++l) {
            crcBde.update(buffer, length);
            crc32 = crcBde.checksumAndReset();
        }
        bsls::Types::Int64 t2 = bsls::TimeUtil::getTimer() - startTime;
        // </time>

        //===================================================================//
        //                            Report
        TableRecord rcd;
        rcd.d_size    = length;
        rcd.d_timeOne = t1 / k_NUM_ITERS;
        rcd.d_timeTwo = t2 / k_NUM_ITERS;
        rcd.d_ratio   = static_cast<double>(t2) / static_cast<double>(t1);

        tableRecords.push_back(rcd);

        if (veryVerbose) {
            unsigned char *sum;
            bsl::cout << "Checksum\n";
            sum = reinterpret_cast<unsigned char*>(&crc32c);
            printf("  CRC32-C (Default): %02x%02x%02x%02x\n",
                   sum[0], sum[1], sum[2], sum[3]);
            sum = reinterpret_cast<unsigned char*>(&crc32);
            printf("  CRC32 (BDE)      : %02x%02x%02x%02x\n",
                   sum[0], sum[1], sum[2], sum[3]);

            bsl::cout
               << "Average Time(ns)\n"
               << "  Default                       : " << rcd.d_timeOne << '\n'
               << "  bdlde::Crc32                  : " << rcd.d_timeTwo << '\n'
               << "  Ratio (bdlde::Crc32 / Default): " << rcd.d_ratio   << '\n'
               << bsl::endl;
        }
    }

    if (verbose) {
        // Print performance comparison table
        bsl::vector<bsl::string> headerCols(pa);
        headerCols.emplace_back("Size(B)");
        headerCols.emplace_back("Def time(ns)");
        headerCols.emplace_back("bdlde::crc32 time(ns)");
        headerCols.emplace_back("Ratio(bdlde::crc32 / Def)");

        printTable(bsl::cout, headerCols, tableRecords);
    }

    pa->deallocate(buffer);
}

void testN2_performanceSoftware()
    // ------------------------------------------------------------------------
    // PERFORMANCE: CALCULATE CRC32-C ON BUFFER SOFTWARE
    //
    // Concerns:
    //: 1 Test the performance of
    //:   'bdlde::Crc32c_Impl::calculateSoftware(const void *, size_t)' in a
    //:   single thread environment. This will explicitly use the software
    //:   implementation.
    //
    // Plan:
    //: 1 Time a large number of CRC32-C calculations for buffers of varying
    //:   sizes in a single thread and take the average.  Compare the average
    //:   to that of 'bdlde::crc32'.
    //
    // Testing:
    //   bdlde::Crc32c_Impl::calculateSoftware(const void *, size_t, uint);
    // ------------------------------------------------------------------------
{
    if (verbose) bsl::cout << bsl::endl
            << "PERFORMANCE: CALCULATE CRC32-C ON BUFFER SOFTWARE" << bsl::endl
            << "=================================================" << bsl::endl;

    const int k_NUM_ITERS = 100000; // 100K

    bsl::vector<int> bufferLengths(pa);
    const int        k_MAX_SIZE = populateBufferLengthsSorted(&bufferLengths);

    // Read in random input
    char *buffer = static_cast<char *>(pa->allocate(k_MAX_SIZE));
    bsl::generate_n(buffer, k_MAX_SIZE, bsl::rand);

    // Measure calculation time and report
    bsl::vector<TableRecord> tableRecords(pa);
    for (unsigned i = 0; i < bufferLengths.size(); ++i) {
        const int length = bufferLengths[i];

        if (veryVerbose) bsl::cout << "---------------------\n"
                                   << " SIZE = " << length << '\n'
                                   << " ITERATIONS = " << k_NUM_ITERS << '\n'
                                   << "---------------------\n";

        //===================================================================//
        //                        [1] Crc32c (software)
        unsigned int crc32c = Crc32c_Impl::calculateSoftware(buffer, length);
        // <time>
        bsls::Types::Int64 startTime = bsls::TimeUtil::getTimer();
        for (unsigned int l = 0; l < k_NUM_ITERS; ++l) {
            crc32c = Crc32c_Impl::calculateSoftware(buffer, length);
        }
        bsls::Types::Int64 t1 = bsls::TimeUtil::getTimer() - startTime;
        // </time>

        //===================================================================//
        //                         [2] BDE bdlde::crc32
        bdlde::Crc32 crcBde(buffer, length);
        unsigned int crc32 = crcBde.checksumAndReset();

        // <time>
        startTime = bsls::TimeUtil::getTimer();
        for (unsigned int l = 0; l < k_NUM_ITERS; ++l) {
            crcBde.update(buffer, length);
            crc32 = crcBde.checksumAndReset();
        }
        bsls::Types::Int64 t2 = bsls::TimeUtil::getTimer() - startTime;
        // </time>

        //===================================================================//
        //                            Report
        TableRecord rcd;
        rcd.d_size    = length;
        rcd.d_timeOne = t1 / k_NUM_ITERS;
        rcd.d_timeTwo = t2 / k_NUM_ITERS;
        rcd.d_ratio   = static_cast<double>(t2) / static_cast<double>(t1);

        tableRecords.push_back(rcd);

        if (veryVerbose) {
            unsigned char *sum;
            bsl::cout << "Checksum\n";
            sum = reinterpret_cast<unsigned char*>(&crc32c);
            printf("  CRC32-C (Software): %02x%02x%02x%02x\n",
                   sum[0], sum[1], sum[2], sum[3]);
            sum = reinterpret_cast<unsigned char*>(&crc32);
            printf("  CRC32 (BDE)       : %02x%02x%02x%02x\n",
                   sum[0], sum[1], sum[2], sum[3]);

            bsl::cout << "Average Time(ns)\n"
                      << "  Software                       : " << rcd.d_timeOne
                      << '\n'
                      << "  bdlde::Crc32                   : " << rcd.d_timeTwo
                      << '\n'
                      << "  Ratio (bdlde::Crc32 / Software): " << rcd.d_ratio
                      << "\n\n";
        }
    }
    // Print performance comparison table
    if (verbose) {
        bsl::vector<bsl::string> headerCols(pa);
        headerCols.emplace_back("Size(B)");
        headerCols.emplace_back("SW time(ns)");
        headerCols.emplace_back("bdlde::crc32 time(ns)");
        headerCols.emplace_back("Ratio(bdlde::crc32 / SW)");

        printTable(bsl::cout, headerCols, tableRecords);
    }

    pa->deallocate(buffer);
}

void testN3_calculateThroughput()
    // ------------------------------------------------------------------------
    // BENCHMARK: CALCULATE CRC32-C THROUGPUT DEFAULT & SOFTWARE
    //
    // Concerns:
    //: 1 Test the throughput (GB/s) of CRC32-C calculation using the default
    //:   and software implementations in a single thread environment.  On a
    //:   supported platform (see 'Support for Hardware Acceleration' in the
    //:   header file), the default method will use a hardware-accelerated
    //:   implementation.  Otherwise, the default method will use a portable
    //:   software implementation.
    //
    // Plan:
    //: 1 Time a large number of CRC32-C calculations using the default and
    //:    software implementations for a buffer of fixed size in a single
    //:    thread and take the average for each implementation.  Calculate the
    //:    throughput of each implementation using its average.
    //
    // Testing:
    //   bdlde::Crc32c::calculate(const void *, size_t, unsigned int);
    //   bdlde::Crc32c_Impl::calculateSoftware(const void *, size_t, uint);
    // ------------------------------------------------------------------------
{
    if (verbose) bsl::cout
           << bsl::endl
           << "BENCHMARK: CALCULATE CRC32-C THROUGPUT DEFAULT & SOFTWARE & BDE"
           << bsl::endl
           << "==============================================================="
           << bsl::endl;

    const bsls::Types::Uint64  k_NUM_ITERS = 1000000; // 1M
    unsigned int               resultDef   = 0;
    unsigned int               resultSW    = 0;
    unsigned int               resultBde   = 0;
    const int                  bufLen      = 12345;
    char                      *buf         = static_cast<char*>(pa->allocate(
                                                                      bufLen));
    bsl::memset(buf, 'x', bufLen);
    //=======================================================================//
    //              [1] Crc32c (default -- hardware on linux)
    resultDef = Crc32c::calculate(buf, bufLen);

    // <time>
    bsls::Types::Int64 startDef = bsls::TimeUtil::getTimer();
    for (bsl::size_t i = 0; i < k_NUM_ITERS; ++i) {
        resultDef = Crc32c::calculate(buf, bufLen);
    }
    bsls::Types::Int64 diffDef = bsls::TimeUtil::getTimer() - startDef;
    // </time>

    //=======================================================================//
    //                        [2] Crc32c (software)
    resultSW = Crc32c_Impl::calculateSoftware(buf, bufLen);

    // <time>
    bsls::Types::Int64 startSW = bsls::TimeUtil::getTimer();
    for (bsl::size_t i = 0; i < k_NUM_ITERS; ++i) {
        resultSW = Crc32c_Impl::calculateSoftware(buf, bufLen);
    }
    bsls::Types::Int64 diffSW = bsls::TimeUtil::getTimer() - startSW;
    // </time>

    //=======================================================================//
    //                           [3] BDE bdlde::crc32
    bdlde::Crc32 crcBde(buf, bufLen);
    resultBde = crcBde.checksumAndReset();

    // <time>
    bsls::Types::Int64 startBde = bsls::TimeUtil::getTimer();
    for (bsl::size_t i = 0; i < k_NUM_ITERS; ++i) {
        crcBde.update(buf, bufLen);
        resultBde = crcBde.checksumAndReset();
    }
    bsls::Types::Int64 diffBde = bsls::TimeUtil::getTimer() - startBde;
     // </time>

    //=======================================================================//
    //                        [4] Report

    if (verbose) {

        const int k_NS_PER_S = 1000000000;

        ASSERTV(resultDef, resultSW, resultDef == resultSW); // Sanity Check

        cout << '\n'
             << "=========================\n"
#ifdef BSLS_PLATFORM_CPU_64_BIT
             << "DEFAULT {SSE 4.2, 64 bit}\n"
#else
             << "DEFUALT {SSE 4.2, 32 bit}\n"
#endif
             << "=========================\n"
             << "For a payload of length " << bufLen << ", completed "
             << k_NUM_ITERS << " HW-version iterations in "
             << diffDef << " ns\n"
             << ".\n"
             << "Above implies that 1 HW-version iteration was calculated in "
             << diffDef / k_NUM_ITERS << " nano seconds.\n"
             << "In other words: " << k_NUM_ITERS * k_NS_PER_S / diffDef
             << " HW-version iterations per second.\n"
             << "HW-version throughput: "
             << k_NUM_ITERS * k_NS_PER_S * bufLen / diffDef
             << " per second.\n\n";

        cout << "=========================\n"
             << "        SOFTWARE         \n"
             << "=========================\n"
             << "For a payload of length " << bufLen << ", completed "
             << k_NUM_ITERS << " SW-version iterations in "
             << diffSW << " ns\n"
             << ".\n"
             << "Above implies that 1 SW-version iteration was calculated in "
             << diffSW / k_NUM_ITERS << " nano seconds.\n"
             << "In other words: " << k_NUM_ITERS * k_NS_PER_S / diffSW
             << " SW-version iterations per second.\n"
             << "SW-version throughput: "
             << (k_NUM_ITERS * k_NS_PER_S * bufLen) / diffSW
             << " per second.\n\n\n";

        cout << "SW / Default (HW) ratio: "
             << static_cast<double>(diffSW) / static_cast<double>(diffDef)
             << "\n\n";

        cout << "==========================\n"
             << "     BDE bdlde::crc32     \n"
             << "==========================\n"
             << "For a payload of length " << bufLen << ", completed "
             << k_NUM_ITERS << " BDE-version iterations in "
             << diffBde << " ns\n"
             << ".\n"
             << "Above implies that 1 BDE-version iteration was calculated in "
             << diffBde / k_NUM_ITERS << " nano seconds.\n"
             << "In other words: "
             << (k_NUM_ITERS * k_NS_PER_S) / diffBde
             << " BDE-version iterations per second.\n"
             << "BDE-version throughput: "
             << (k_NUM_ITERS * k_NS_PER_S * bufLen) / diffBde
             << " per second.\n\n\n";
    }

    pa->deallocate(buf);
    static_cast<void>(resultDef);
    static_cast<void>(resultSW);
    static_cast<void>(resultBde);
}

void testN4_calculateHardwareSerial()
    // ------------------------------------------------------------------------
    // PERFORMANCE: CALCULATE CRC32-C DEFAULT & FOLLY
    //
    // Concerns:
    //: 1 Test the performance of
    //:   'bdlde::Crc32c::calculate(const void *, size_t length)'
    //:   and compare its performance to Facebook folly's implementation
    //:   'bdlde::Crc32c_Impl::calculateHardwareSerial(const void *, size_t)'.
    //
    // Plan:
    //: 1 Time a large number of crc32c calculations for buffers of varying
    //:   sizes, single threaded.
    //
    // Testing:
    //   bdlde::Crc32c::calculate(const void *, size_t, unsigned int);
    //   bdlde::Crc32c_Impl::calculateHardwareSerial(const void *,size_t,uint);
    // ------------------------------------------------------------------------
{
    if (verbose) bsl::cout << bsl::endl
               << "PERFORMANCE: CALCULATE CRC32-C DEFAULT & FOLLY" << bsl::endl
               << "==============================================" << bsl::endl;

    const int k_NUM_ITERS = 100000; // 100K

    bsl::vector<int> bufferLengths(pa);
    const int        k_MAX_SIZE = populateBufferLengthsSorted(&bufferLengths);

    // Read in random input
    char *buffer = static_cast<char *>(pa->allocate(k_MAX_SIZE));
    bsl::generate_n(buffer, k_MAX_SIZE, bsl::rand);

    // Measure calculation time and report
    bsl::vector<TableRecord> tableRecords(pa);
    for (unsigned i = 0; i < bufferLengths.size(); ++i) {
        const int length = bufferLengths[i];

        if (verbose) bsl::cout << "---------------------"         << '\n'
                               << " SIZE = " << length            << '\n'
                               << " ITERATIONS = " << k_NUM_ITERS << '\n'
                               << "---------------------"         << bsl::endl;

        //===============================================================//
        //                     [1] Crc32c (default)
        unsigned int crc32c;
        // <time>
        bsls::Types::Int64 startDef = bsls::TimeUtil::getTimer();
        for (bsl::size_t k = 0; k < k_NUM_ITERS; ++k) {
            crc32c = Crc32c::calculate(buffer, length);
        }
        bsls::Types::Int64 endDef = bsls::TimeUtil::getTimer();
        // </time>
        //===============================================================//
        //                [2] Crc32c (hardware serial)
        unsigned int crcFolly;
        // <time>
        bsls::Types::Int64 startFolly = bsls::TimeUtil::getTimer();
        for (bsl::size_t k = 0; k < k_NUM_ITERS; ++k) {
            crcFolly = Crc32c_Impl::calculateHardwareSerial(buffer, length);
        }
        bsls::Types::Int64 endFolly = bsls::TimeUtil::getTimer();
        // </time>
        //===============================================================//
        //                            Report
        TableRecord record;
        record.d_size    = length;
        record.d_timeOne = (endDef   - startDef)   / k_NUM_ITERS;
        record.d_timeTwo = (endFolly - startFolly) / k_NUM_ITERS;
        record.d_ratio   =  static_cast<double>(record.d_timeTwo)
                            / static_cast<double>(record.d_timeOne);

        tableRecords.push_back(record);

        if (verbose) {
            ASSERTV(crc32c, crcFolly, crc32c == crcFolly); // Sanity Check

            const unsigned char *sum;
            bsl::cout << "Checksum\n";
            sum = reinterpret_cast<const unsigned char *>(&crc32c);
            printf("  CRC32-C (Default)  : %02x%02x%02x%02x\n",
                   sum[0], sum[1], sum[2], sum[3]);
            sum = reinterpret_cast<const unsigned char *>(&crcFolly);
            printf("  CRC32-C (HW Serial): %02x%02x%02x%02x\n",
                   sum[0], sum[1], sum[2], sum[3]);

            bsl::cout
               << "Average Times(ns)\n"
               << "  CRC32-C (Default)          : " << record.d_timeOne << '\n'
               << "  CRC32-C (HW Serial)        : " << record.d_timeTwo << '\n'
               << "  Ratio (HW Serial / Default): " << record.d_ratio   << '\n'
               << bsl::endl;
        }
        // Print performance comparison table
        bsl::vector<bsl::string> headerCols(pa);
        headerCols.emplace_back("Size(B)");
        headerCols.emplace_back("Default time(ns)");
        headerCols.emplace_back("Folly time(ns)");
        headerCols.emplace_back("Ratio(Folly / Default)");

        printTable(bsl::cout, headerCols, tableRecords);
    }
    pa->deallocate(buffer);
}

void testN5_performanceDefaultUserInput()
    // ------------------------------------------------------------------------
    // PERFORMANCE: CALCULATE CRC32-C DEFAULT ON USER INPUT
    //
    // Concerns:
    //: 1 Test the performance of
    //:   'bdlde::Crc32c::calculate(const void *, unsigned int)' on an user
    //:   input.
    //
    // Testing:
    //   bdlde::Crc32c::calculate(const void *, size_t, unsigned int);
    // ------------------------------------------------------------------------
{
    if (verbose) bsl::cout << bsl::endl
         << "PERFORMANCE: CALCULATE CRC32-C DEFAULT ON USER INPUT" << bsl::endl
         << "====================================================" << bsl::endl;

    cout << "Please enter the string:" << endl << "> ";

    // Read from stdin
    bsl::string input(pa);
    bsl::getline(bsl::cin, input);

    const bsl::size_t  k_NUM_ITERS = 100000; // 100K
    unsigned int       crc32c      = 0;
    bsls::Types::Int64 startTime   = bsls::TimeUtil::getTimer();
    for (bsl::size_t i = 0; i < k_NUM_ITERS; ++i)
    {
        crc32c = Crc32c::calculate(input.c_str(),
                                   static_cast<unsigned int>(input.size()));
    }
    bsls::Types::Int64 endTime = bsls::TimeUtil::getTimer();

    const unsigned char *sum = reinterpret_cast<const unsigned char*>(&crc32c);

    printf("\nCRC32-C : %02x%02x%02x%02x\n", sum[0], sum[1], sum[2], sum[3]);

    cout << "Average Time (nano sec): " << (endTime - startTime) / k_NUM_ITERS
         << "\n\n";
}

}  // close unnamed namespace

// ============================================================================
//                                 MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
                   test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;  // always the last

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma::TestAllocator  ta(veryVeryVeryVerbose);
    pa = &ta;

    bsls::TimeUtil::initialize();

    bsls::Log::setSeverityThreshold(bsls::LogSeverity::e_INFO);

    switch(test) { case 0:
      case 7: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE 1
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Run the usage example 1
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example 1"
                          << "\n=======================" << endl;

///Example 1: Computing and updating a checksum
/// - - - - - - - - - - - - - - - - - - - - - -
// The following code illustrates how to calculate and update a CRC32-C
// checksum for a message over the course of building the full message.
//
// First, prepare a message.
//..
        bsl::string message = "This is a test message.";
//..
// Now, generate a checksum for 'message'.
//..
        unsigned int checksum = bdlde::Crc32c::calculate(message.c_str(),
                                                         message.size());
//..
// Finally, if we learn that our message has grown by another chunk and we want
// to compute the checksum of the original message plus the new chunk, let's
// update the checksum by using it as a starting point.
//..
        // New chunk
        bsl::string newChunk = "This is a chunk appended to original message";
        message += newChunk;

        // Update checksum using previous value as starting point
        checksum = bdlde::Crc32c::calculate(newChunk.c_str(),
                                            newChunk.size(),
                                            checksum);
//..
      } break;
      case  6: {
        test6_multithreadedCrc32cSoftware();
      } break;
      case  5: {
        test5_multithreadedCrc32cDefault();
      } break;
      case  4: {
        test4_calculateOnBufferWithPreviousCrc();
      } break;
      case  3: {
        test3_calculateOnMisalignedBuffer();
      } break;
      case  2: {
        test2_calculateOnBuffer();
      } break;
      case  1: {
        test1_breathingTest();
      } break;
      case -1: {
        testN1_performanceDefault();
      } break;
      case -2: {
        testN2_performanceSoftware();
      } break;
      case -3: {
        testN3_calculateThroughput();
      } break;
      case -4: {
        testN4_calculateHardwareSerial();
      } break;
      case -5: {
        testN5_performanceDefaultUserInput();
      } break;
      default: {
        cerr << "WARNING: CASE '" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    LOOP2_ASSERT(test, globalAllocator.numBlocksTotal(),
                 0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

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
// ------------------------------- END-OF-FILE --------------------------------
