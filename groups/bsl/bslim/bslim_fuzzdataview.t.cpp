// bslim_fuzzdataview.t.cpp                                           -*-C++-*-
#include <bslim_fuzzdataview.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_algorithm.h>  // bsl::generate_n
#include <bsl_cstdint.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>

// On some older platforms, 'bsl_cstdint.h' does not define 'SIZE_MAX'.  On
// these platforms, define 'SIZE_MAX' using 'numeric_limits'.

#if !defined(SIZE_MAX)
    #include <bsl_limits.h>
    #define SIZE_MAX bsl::numeric_limits<bsl::size_t>::max()
#endif

using namespace BloombergLP;
using bsl::cerr;
using bsl::cout;
using bsl::endl;
using bsl::flush;

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test implements a reference-semantic view of a buffer of
// fuzz data bytes.  This test driver tests each implemented function.
//
// Global Concerns:
//: o ACCESSOR methods are declared 'const'.
//: o No memory is ever allocated from the global allocator.
//-----------------------------------------------------------------------------
// CREATOR
// [ 2] FuzzDataView(const bsl::uint8_t *data, bsl::size_t size);
// MANIPULATORS
// [ 3] FuzzDataView removePrefix(bsl::size_t numBytes);
// [ 4] FuzzDataView removeSuffix(bsl::size_t numBytes);
// ACCESSORS
// [ 2] const bsl::uint8_t *begin() const;
// [ 2] const bsl::uint8_t *end() const;
// [ 2] bsl::size_t length() const;
// [ 2] const bsl::uint8_t *data() const;
// ----------------------------------------------------------------------------
// [ 5] USAGE EXAMPLE
// [ 1] BREATHING TEST

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT BSLIM_TESTUTIL_ASSERT
#define ASSERTV BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q BSLIM_TESTUTIL_Q    // Quote identifier literally.
#define P BSLIM_TESTUTIL_P    // Print identifier and value.
#define P_ BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_ BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_ BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//         GLOBAL TYPEDEFS, CONSTANTS, ROUTINES & MACROS FOR TESTING
// ----------------------------------------------------------------------------

typedef bslim::FuzzDataView Obj;

static int verbose;
static int veryVerbose;
static int veryVeryVerbose;
static int veryVeryVeryVerbose;

void generateBytes(bsl::uint8_t *buffer,
                   bsl::size_t   bufLen,
                   bsl::size_t   numBytes)
    // Load into the specified 'buffer' of the specified 'bufLen' a total of
    // 'numBytes' random bytes.  The behavior is undefined unless
    // 'numBytes <= bufLen'.
{
    ASSERT(numBytes <= bufLen);
    bsl::generate_n(buffer, numBytes, bsl::rand);
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

    verbose             = argc > 2;
    veryVerbose         = argc > 3;
    veryVeryVerbose     = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // CONCERN: In no case does memory come from the default allocator.

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    switch (test) {
      case 0:  // Zero is always the leading case.
      case 5: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl << "USAGE EXAMPLE" << endl << "=============" << endl;
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating a 'bsl::string'
///- - - - - - - - - - - - - - - - - -
// The following example demonstrates how to create a 'bsl::string' object from
// a 'FuzzDataView'.
//
// First, we construct a 'FuzzDataView' object, 'view0', from an array of
// bytes:
//..
   const uint8_t data[] = {0x8A, 0x19, 0x0D, 0x44, 0x37, 0x0D,
                           0x38, 0x5E, 0x9B, 0xAA, 0xF3, 0xDA};
//
   bslim::FuzzDataView view0(data, sizeof(data));
//
   ASSERT(12 == view0.length());
//..
// Next, we take the first 3 bytes from 'view0' and store them in a new
// 'FuzzDataView' object, 'view1':
//..
   bslim::FuzzDataView view1 = view0.removePrefix(3);
//
   ASSERT(3 == view1.length());
   ASSERT(9 == view0.length());
//..
// We confirm that 'removePrefix(3)' removed 3 bytes from 'view0' and
// that 'view1' has length 3.
//
// Then, we create a 'bsl::string' object from 'view1':
//..
   bsl::string s1(view1.begin(), view1.end());

   ASSERT(3 == s1.length());
//..
// Finally, we create another 'bsl::string' with the remaining bytes of
// 'view0':
//..
   bsl::string s2(view0.begin(), view0.end());

   ASSERT(9 == s2.length());
//..
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // METHOD 'removeSuffix'
        //
        // Concerns:
        //: 1 That the source object has the 'min(numBytes, source.length())'
        //:   last bytes removed.
        //:
        //: 2 That the object returned has those bytes that were removed.
        //:
        //: 3 That if we try to remove more bytes than contained in the source,
        //:   the method will return all bytes contained in the source.
        //:
        //: 4 That corner cases of removing 0 bytes and SIZE_MAX are handled
        //:   appropriately.
        //:
        //: 5 That no bytes in the original data buffer are modified.
        //:
        //: 6 That the function behaves as expected if the source object was
        //:   created with (0, 0).
        //
        // Plan:
        //: 1 First, using the table-driven approach, define a representative
        //:   set of valid inputs.  Verify that the function returns the
        //:   correct value.  (C-2)
        //:
        //: 2 Verify that 'min(numBytes, source.length())' bytes were removed
        //:   from the source object.  (C-1)
        //:
        //: 3 Before invoking 'removeSuffix', make a copy of the original
        //:   and then verify that the returned bytes are those expected.
        //:   (C-2)
        //:
        //: 4 Compute the expected number of bytes (i.e., min(NUM_BYTES,
        //:   length()) and verify that this number of bytes was consumed.
        //:   (C-3)
        //:
        //: 5 Using the table-driven approach, test the corner cases of
        //:   removing 0 and SIZE_MAX bytes.  (C-4)
        //:
        //: 6 Generate random fuzz data buffers and exercise the function
        //:   with random data.  Verify that if we try to remove more bytes
        //:   than contained in the source, the method will return all bytes
        //:   contained in the source.  (C-3)
        //:
        //: 7 For each iteration through the table, create a source object with
        //:   null data and size of 0, and verify that the function behaves as
        //:   expected.  (C-6)
        //:
        //: 8 Using 'bsl::equal', verify that no bytes in the original data
        //:   buffer were modified.  (C-5)
        //
        // Testing:
        //   FuzzDataView removeSuffix(bsl::size_t numBytes);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "METHOD 'removeSuffix'" << endl
                 << "=====================" << endl;

        static const struct {
            int            d_line;              // source line number
            bsl::size_t    d_numBytesToRemove;  // suffix size
        } DATA[] = {
            //LINE  NUM_BYTES_TO_REMOVE
            //----  -------------------
            {L_,             0 },
            {L_,             1 },
            {L_,             2 },
            {L_,             4 },
            {L_,             7 },
            {L_,             8 },
            {L_,            15 },
            {L_,           128 },
            {L_,           256 },
            {L_,      SIZE_MAX }
        };

        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         BUFLEN = 128;
            for (int inBytes = 0; inBytes < BUFLEN; ++inBytes) {
                bsl::uint8_t      FUZZ_BUF[BUFLEN];
                bsl::uint8_t      SAVE_BUF[BUFLEN];
                const int         LINE          = DATA[ti].d_line;
                const bsl::size_t NUM_BYTES     = DATA[ti].d_numBytesToRemove;

                generateBytes(FUZZ_BUF, BUFLEN, inBytes);
                bsl::copy(FUZZ_BUF, FUZZ_BUF + inBytes, SAVE_BUF);

                Obj mX(FUZZ_BUF, inBytes); const Obj& X = mX;

                const bsl::size_t EXP_SUFFIX_LENGTH =
                    bsl::min(NUM_BYTES, X.length());

                Obj        mS = mX.removeSuffix(NUM_BYTES);
                const Obj& S  = mS;

                if (veryVerbose) {
                    T_ P_(LINE) P_(inBytes) P(NUM_BYTES);
                }

                ASSERT(EXP_SUFFIX_LENGTH                      == S.length());
                ASSERT(FUZZ_BUF + inBytes - EXP_SUFFIX_LENGTH == S.data());
                ASSERT(FUZZ_BUF + inBytes - EXP_SUFFIX_LENGTH == S.begin());
                ASSERT(FUZZ_BUF + inBytes                     == S.end());

                ASSERT(inBytes - EXP_SUFFIX_LENGTH            == X.length());
                ASSERT(FUZZ_BUF                               == X.data());
                ASSERT(FUZZ_BUF                               == X.begin());
                ASSERT(FUZZ_BUF + inBytes - EXP_SUFFIX_LENGTH == X.end());

                ASSERT(true ==
                       bsl::equal(FUZZ_BUF, FUZZ_BUF + inBytes, SAVE_BUF));
            }
        }
        {
            if (verbose) {
                cout << "\nTesting 'removeSuffix' with FuzzDataView(0, 0)"
                     << endl;
            }
            Obj        mX(0, 0);
            const Obj& X = mX;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE      = DATA[ti].d_line;
                const bsl::size_t NUM_BYTES = DATA[ti].d_numBytesToRemove;

                Obj        mS = mX.removeSuffix(NUM_BYTES);
                const Obj& S  = mS;

                if (veryVerbose) {
                    T_ P_(LINE) P(NUM_BYTES);
                }

                ASSERT(0 == S.length());
                ASSERT(0 == S.data());
                ASSERT(0 == S.begin());
                ASSERT(0 == S.end());

                ASSERT(0 == X.length());
                ASSERT(0 == X.data());
                ASSERT(0 == X.begin());
                ASSERT(0 == X.end());
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // METHOD 'removePrefix'
        //
        // Concerns:
        //: 1 That the source object has the 'min(numBytes, source.length())'
        //:   first bytes removed.
        //:
        //: 2 That the object returned has those bytes that were removed.
        //:
        //: 3 That if we try to remove more bytes than contained in the source,
        //:   the method will return all bytes contained in the source.
        //:
        //: 4 That corner cases of removing 0 bytes and SIZE_MAX are handled
        //:   appropriately.
        //:
        //: 5 That no bytes in the original data buffer are modified.
        //:
        //: 6 That the function behaves as expected if the source object was
        //:   created with (0, 0).
        //
        // Plan:
        //: 1 First, using the table-driven approach, define a representative
        //:   set of valid inputs.  Verify that the function returns the
        //:   correct value.  (C-2)
        //:
        //: 2 Verify that 'min(numBytes, source.length())' bytes were removed
        //:   from the source object.  (C-1)
        //:
        //: 3 Before invoking 'removePrefix', make a copy of the original
        //:   and then verify that the returned bytes are those expected.
        //:   (C-2)
        //:
        //: 4 Compute the expected number of bytes (i.e., min(NUM_BYTES,
        //:   length()) and verify that this number of bytes was consumed.
        //:   (C-3)
        //:
        //: 5 Using the table-driven approach, test the corner cases of
        //:   removing 0 and SIZE_MAX bytes.  (C-4)
        //:
        //: 6 Generate random fuzz data buffers and exercise the function
        //:   with random data.  Verify that if we try to remove more bytes
        //:   than contained in the source, the method will return all bytes
        //:   contained in the source.  (C-3)
        //:
        //: 7 For each iteration through the table, create a source object with
        //:   null data and size of 0, and verify that the function behaves as
        //:   expected.  (C-6)
        //:
        //: 8 Using 'bsl::equal', verify that no bytes in the original data
        //:   buffer were modified.  (C-5)
        //
        // Testing:
        //   FuzzDataView removePrefix(bsl::size_t numBytes);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "METHOD 'removePrefix'" << endl
                 << "=====================" << endl;

        static const struct {
            int            d_line;              // source line number
            bsl::size_t    d_numBytesToRemove;  // suffix size
        } DATA[] = {
            //LINE  NUM_BYTES_TO_REMOVE
            //----  -------------------
            {L_,             0 },
            {L_,             1 },
            {L_,             2 },
            {L_,             4 },
            {L_,             7 },
            {L_,             8 },
            {L_,            15 },
            {L_,           128 },
            {L_,           256 },
            {L_,      SIZE_MAX }
        };

        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         BUFLEN = 128;
            for (int inBytes = 0; inBytes < BUFLEN; ++inBytes) {
                bsl::uint8_t      FUZZ_BUF[BUFLEN];
                bsl::uint8_t      SAVE_BUF[BUFLEN];
                const int         LINE          = DATA[ti].d_line;
                const bsl::size_t NUM_BYTES     = DATA[ti].d_numBytesToRemove;

                generateBytes(FUZZ_BUF, BUFLEN, inBytes);
                bsl::copy(FUZZ_BUF, FUZZ_BUF + inBytes, SAVE_BUF);



                Obj mX(FUZZ_BUF, inBytes); const Obj& X = mX;

                const bsl::size_t maxLengthReturned =
                        bsl::min(NUM_BYTES, X.length());

                Obj        mP = mX.removePrefix(NUM_BYTES);
                const Obj& P  = mP;

                if (veryVerbose) {
                    T_ P_(LINE) P_(inBytes) P(NUM_BYTES);
                }

                ASSERT(maxLengthReturned            == P.length());
                ASSERT(FUZZ_BUF                     == P.data());
                ASSERT(FUZZ_BUF                     == P.begin());
                ASSERT(FUZZ_BUF + maxLengthReturned == P.end());

                ASSERT(inBytes - maxLengthReturned  == X.length());
                ASSERT(FUZZ_BUF + maxLengthReturned == X.data());
                ASSERT(FUZZ_BUF + maxLengthReturned == X.begin());
                ASSERT(FUZZ_BUF + inBytes           == X.end());

                ASSERT(true ==
                       bsl::equal(FUZZ_BUF, FUZZ_BUF + inBytes, SAVE_BUF));
            }
        }
        {
            if (verbose) {
                cout << "\nTesting 'removePrefix' with FuzzDataView(0, 0)"
                     << endl;
            }
            Obj        mX(0, 0);
            const Obj& X = mX;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE      = DATA[ti].d_line;
                const bsl::size_t NUM_BYTES = DATA[ti].d_numBytesToRemove;

                Obj        mS = mX.removePrefix(NUM_BYTES);
                const Obj& S  = mS;

                if (veryVerbose) {
                    T_ P_(LINE) P(NUM_BYTES);
                }

                ASSERT(0 == S.length());
                ASSERT(0 == S.data());
                ASSERT(0 == S.begin());
                ASSERT(0 == S.end());

                ASSERT(0 == X.length());
                ASSERT(0 == X.data());
                ASSERT(0 == X.begin());
                ASSERT(0 == X.end());
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CTOR, ACCESSORS, & DTOR
        //
        // Concerns:
        //: 1 An object is correctly constructed from the given data.
        //:
        //: 2 Accessor methods return the correct values.
        //:
        //: 3 No bytes were modified in the original data buffer.
        //:
        //: 4 An object can be created with 'size == 0' as well as both
        //:   'data == 0' and 'size == 0'.  The resulting object behaves as
        //:   expected.
        //
        // Plan:
        //: 1 Using the table-driven approach,define a representative set of
        //:   valid inputs.  Verify that the function returns the correct
        //:   value and that the source object is correct.  (C-1..2)
        //:
        //: 2 Using 'bsl::equal' verify that the original data buffer used to
        //:   construct the 'FuzzDataView' has not been modified.  (C-3)
        //:
        //: 3 Create objects passing '0' for 'size' as well as '0, 0' to the
        //:   constructor ('data' and 'size').  Invoke the accessors. (C-4)
        //
        // Testing:
        //   FuzzDataView(const bsl::uint8_t *data, bsl::size_t size);
        //   const bsl::uint8_t *begin() const;
        //   const bsl::uint8_t *end() const;
        //   bsl::size_t length() const;
        //   const bsl::uint8_t *data() const;
        // --------------------------------------------------------------------
        if (verbose)
            cout << endl
                 << "CTOR, ACCESSORS, & DTOR" << endl
                 << "=======================" << endl;

        const int    BUFLEN = 128;
        bsl::uint8_t FUZZ_BUF[BUFLEN];
        bsl::uint8_t SAVE_BUF[BUFLEN];

        static const struct {
            int            d_line;        // source line number
            bsl::size_t    d_length;      // suffix size
        } DATA[] = {
            //LINE        LENGTH     USE_NULL_DATA
            //----        ------     -------------
            {L_,             0 },
            {L_,             1 },
            {L_,             2 },
            {L_,             4 },
            {L_,             7 },
            {L_,             8 },
            {L_,            15 },
            {L_,        BUFLEN }
        };

        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE          = DATA[ti].d_line;
            const bsl::size_t LEN           = DATA[ti].d_length;

            generateBytes(FUZZ_BUF, BUFLEN, LEN);
            bsl::copy(FUZZ_BUF, FUZZ_BUF + LEN, SAVE_BUF);

            Obj mX(FUZZ_BUF, LEN);  const Obj& X = mX;

            ASSERT(LEN            == X.length());
            ASSERT(FUZZ_BUF       == X.begin());
            ASSERT(FUZZ_BUF       == X.data());
            ASSERT(FUZZ_BUF + LEN == X.end());

            ASSERT(true ==
                   bsl::equal(FUZZ_BUF, FUZZ_BUF + LEN, SAVE_BUF));

            if (veryVerbose) {
                T_ P_(LINE) P_(X.length()) P(LEN);
            }
        }
        {
            if (verbose)
                cout << "\nTesting 'FuzzDataView(0, 0)'." << endl;
            Obj        mX(0, 0);
            const Obj& X = mX;

            ASSERT(0 == X.length());
            ASSERT(0 == X.begin());
            ASSERT(0 == X.data());
            ASSERT(0 == X.end());
        }
        {
            if (verbose)
                cout << "\nNegative Testing." << endl;
            {
                bsls::AssertTestHandlerGuard hG;
                {
                    ASSERT_PASS(Obj(0, 0));
                    ASSERT_FAIL(Obj(0, 1));
                }
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Perform and ad-hoc test of the primary modifiers and accessors.
        //: 2 Create an object from an array of bytes and its size.
        //: 3 Exercise all of the ACCESSORS: 'begin', 'end', 'length', 'data'
        //: 4 Exercise the MANIPULATORS: 'removePrefix' and 'removeSuffix'
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------
        const int    BUFLEN = 128;
        bsl::uint8_t FUZZ_BUF[BUFLEN];

        if (verbose)
            cout << endl
                 << "BREATHING TEST" << endl
                 << "==============" << endl;
        {
            if (verbose) cout << "\n Testing CTOR and ACCESSORS" << endl;
            const bsl::size_t viewLen = 12;
            generateBytes(FUZZ_BUF, BUFLEN, viewLen);
            Obj           mX(FUZZ_BUF, viewLen);
            const Obj&    X = mX;
            ASSERT(viewLen == mX.length());

            if (veryVerbose) {
                cout << "\tX.length():  = %zu\n", X.length();
                for (const uint8_t *it = X.begin(); it != X.end(); it++) {
                    cout << '\t' << *it << endl;
                }
            }
        }
        {
            if (verbose) cout << "\n Testing CTOR and 'removePrefix'" << endl;
            const bsl::size_t viewLen = 12;
            generateBytes(FUZZ_BUF, BUFLEN, viewLen);
            Obj           view0(FUZZ_BUF, viewLen);
            ASSERT(12 == view0.length());

            Obj view1 = view0.removePrefix(1);
            ASSERT(FUZZ_BUF[0] == *(view1.data()));

            ASSERT(1  == view1.length());
            ASSERT(11 == view0.length());
            bsl::string s(view1.begin(), view1.end());

            ASSERT(1 == s.length());

            bsl::string s2(view0.begin(), view0.end());

            ASSERT(11 == s2.length());
        }
        {
            if (verbose) cout << "\n Testing CTOR and 'removeSuffix'" << endl;
            const bsl::size_t viewLen = 12;
            generateBytes(FUZZ_BUF, BUFLEN, viewLen);
            Obj           view0(FUZZ_BUF, viewLen);
            ASSERT(12 == view0.length());

            Obj view1 = view0.removeSuffix(1);

            ASSERT(1  == view1.length());
            ASSERT(11 == view0.length());
            ASSERT(FUZZ_BUF[11] == *view1.data());
        }
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    // CONCERN: In no case does memory come from the default allocator.

    LOOP_ASSERT(defaultAllocator.numBlocksTotal(),
                0 == defaultAllocator.numBlocksTotal());

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }
    return testStatus;
}

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
