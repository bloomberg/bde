// bslstl_bitset.t.cpp                                                -*-C++-*-
#include <bslstl_bitset.h>

#include <bslstl_string.h>

#include <bslmf_assert.h>

#include <bsls_nativestd.h>

#include <cstdlib>
#include <iostream>
#include <string>

#include <cmath>  // native_std::sqrt

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                                 TEST  PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
//
// ----------------------------------------------------------------------------
// CREATORS:
// [ 2] bitset()
// [ 2] bitset(unsigned long)
// [ 3] bitset(native_std::basic_string, size_type, size_type);
// [ 3] bitset(bslstl::basic_string, size_type, size_type);
//
// MANIPULATORS:
// [ 4] bitset& operator<<=(std::size_t pos);
// [ 4] bitset& operator>>=(std::size_t pos);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 5] USAGE EXAMPLE
//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//--------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
        << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ cout << "\t" << flush;             // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

template <size_t N>
bool verifyBitset(const bsl::bitset<N> obj, const char *expected)
{
    for (unsigned int i = 0; i < N; ++i) {
        ASSERT(expected[i] == '1' || expected[i] == '0');
        if ((expected[i] == '1') != obj[N - i - 1]) {
            return false;
        }
    }
    return true;
}

//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------
///Example 1: Determining if a number is prime (Sieve of Eratosthenes)
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to write a function to determine whether or not a given
// number is prime.  One way to implement this function is by using what's
// called the Sieve of Erathosthenes.  The basic idea of this algorithm is to
// repeatedly walk the sequence of integer values and mark any numbers up to
// and including the particular value of interest that are integer multiples of
// first 2, then 3, then 5 etc., (skipping 4 because it was previously marked
// when we walked the sequence by 2's).  Once we have walked the sequence
// with all primes up to and including the square root of the number of
// interest, we check to see if that number has been marked: If it has, it's
// composite; otherwise it's prime.
//
// When implementing this classic algorithm, we need an efficient way of
// representing a flag for each potential prime number.  The following
// illustrates how we can use 'bsl::bitset' to accomplish this result, provided
// we know an upper bound on supplied candidate values at compile time.
//
// First, we begin to define a function template that will determine whether or
// not a given candidate value is prime:
//..
template <unsigned int MAX_VALUE>
bool isPrime(int candidate)
    // Return 'true' if the specified 'candidate' value is a prime number,
    // and 'false' otherwise.  The behavior is undefined unless
    // '2 <= candidate <= MAX_VALUE'
{
    BSLMF_ASSERT(2 <= MAX_VALUE);
    BSLS_ASSERT(2 <= candidate); BSLS_ASSERT(candidate <= MAX_VALUE);
//..
// Then, we declare a 'bsl::bitset', 'compositeFlags', that will contain flags
// indicating whether or not each value is potentially prime, up to and
// including some compile-time constant template parameter, 'MAX_VALUE'.
//..
    // Potentially prime (indicated by 'false') or composite (indicated by
    // 'true') values in the range '[ 2 .. MAX_VALUE ]'.  At the end of the
    // sieve loop, any values in this set which remain 'false' indicate prime
    // numbers.

    bsl::bitset<MAX_VALUE + 1> compositeFlags;
//..
// Next, we compute 'sqrt(candidate)', which is as far as we need to look:
//..
    // We need to cast the 'sqrt' argument to avoid an overload ambiguity.
    const int sqrtOfCandidate = std::sqrt(static_cast<double>(candidate))
                                + 0.01;  // fudge factor
//..
// Now, we loop from 2 to 'sqrtOfCandidate', and use the sieve algorithm to
// eliminate non-primes:
//..
    for (int i = 2; i <= sqrtOfCandidate; ++i) {
        if (compositeFlags[i]) {
            continue;  // Skip this value: it's flagged as composite, so all
                       // of its multiples are already flagged as composite as
                       // well.
        }

        for (int flagValue = i; flagValue <= candidate; flagValue += i) {
            compositeFlags[flagValue] = true;
        }

        if (true == compositeFlags[candidate]) {
            return false;                                          // RETURN
        }
    }

    BSLS_ASSERT(false == compositeFlags[candidate]);

    return true;
}
//..
// Notice that if we don't return 'false' from the loop, none of the lower
// numbers evenly divided the candidate value; hence, it is a prime number.


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // zero is always the leading case
    case 5: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE TEST
        //
        // Finally, we can exercise our 'isPrime' function:
        // --------------------------------------------------------------------

        // Finally, we can exercise our 'isPrime' function with an upper bound
        // of 10,000:
        //..
        enum { UPPER_BOUND = 10000 };

        ASSERT(1 == isPrime<UPPER_BOUND>(2));
        ASSERT(1 == isPrime<UPPER_BOUND>(3));
        ASSERT(0 == isPrime<UPPER_BOUND>(4));
        ASSERT(1 == isPrime<UPPER_BOUND>(5));
        ASSERT(0 == isPrime<UPPER_BOUND>(6));
        ASSERT(1 == isPrime<UPPER_BOUND>(7));
        ASSERT(0 == isPrime<UPPER_BOUND>(8));
        ASSERT(0 == isPrime<UPPER_BOUND>(9));
        ASSERT(0 == isPrime<UPPER_BOUND>(10));
        // ...
        ASSERT(1 == isPrime<UPPER_BOUND>(9973));
        ASSERT(0 == isPrime<UPPER_BOUND>(9975));
        ASSERT(0 == isPrime<UPPER_BOUND>(10000));
        //..


    } break;
    case 4: {
        // --------------------------------------------------------------------
        // SHIFT OPERATOR TEST
        //
        // Concerns:
        //   1. That a bitset can be shifted across word boundaries.
        //
        //   2. That a bitset get filled by 0s for the most significant 'pos'
        //      bits if shifted right, or the least significant 'pos' bits if
        //      shifted left.
        //
        // Plan:
        //   Using the table-driven technique, construct a bitset.  Then shift
        //   the bitset and verify the value is as expected.
        //
        // Testing:
        //   bitset& operator<<=(std::size_t pos);
        //   bitset& operator>>=(std::size_t pos);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "SHIFT OPERATOR TEST"
                          << endl << "===================" << endl;

        {

        static const struct {
            unsigned int d_lineNum;    // source line number
            const char*  d_string;     // bitset string
            size_t       d_pos;        // amount to shift by
            const char*  d_shiftlexp;  // expected string after left shift
            const char*  d_shiftrexp;  // expected string after right shift
        } DATA[] = {
   //LINE  VALUE                POS SHIFTLEXP            SHIFTREXP
   //----  -------------------  --- -------------------  ------------------
   { L_,   "00000000000000000", 0,  "00000000000000000", "00000000000000000"},
   { L_,   "00000000000000000", 1,  "00000000000000000", "00000000000000000"},
   { L_,   "00000000000000000", 8,  "00000000000000000", "00000000000000000"},
   { L_,   "00000000000000000", 15, "00000000000000000", "00000000000000000"},
   { L_,   "00000000000000000", 16, "00000000000000000", "00000000000000000"},
   { L_,   "00000000000000000", 17, "00000000000000000", "00000000000000000"},

   { L_,   "11111111111111111", 0,  "11111111111111111", "11111111111111111"},
   { L_,   "11111111111111111", 1,  "11111111111111110", "01111111111111111"},
   { L_,   "11111111111111111", 8,  "11111111100000000", "00000000111111111"},
   { L_,   "11111111111111111", 15, "11000000000000000", "00000000000000011"},
   { L_,   "11111111111111111", 16, "10000000000000000", "00000000000000001"},
   { L_,   "11111111111111111", 17, "00000000000000000", "00000000000000000"},

   { L_,   "10000000000000001", 0,  "10000000000000001", "10000000000000001"},
   { L_,   "10000000000000001", 1,  "00000000000000010", "01000000000000000"},
   { L_,   "10000000000000001", 8,  "00000000100000000", "00000000100000000"},
   { L_,   "10000000000000001", 15, "01000000000000000", "00000000000000010"},
   { L_,   "10000000000000001", 16, "10000000000000000", "00000000000000001"},
   { L_,   "10000000000000001", 17, "00000000000000000", "00000000000000000"},

   { L_,   "00001000000010000", 0,  "00001000000010000", "00001000000010000"},
   { L_,   "00001000000010000", 1,  "00010000000100000", "00000100000001000"},
   { L_,   "00001000000010000", 8,  "00001000000000000", "00000000000010000"},
   { L_,   "00001000000010000", 15, "00000000000000000", "00000000000000000"},
   { L_,   "00001000000010000", 16, "00000000000000000", "00000000000000000"},
   { L_,   "00001000000010000", 17, "00000000000000000", "00000000000000000"},
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        const int TESTSIZE = 17;  // num bits of char + 1
        if (verbose) cout <<
           "\nTesting shift operators with bitset<" << TESTSIZE << ">" << endl;

        typedef bsl::bitset<TESTSIZE> Obj;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const unsigned int  LINE      = DATA[ti].d_lineNum;
            const char         *VALUE     = DATA[ti].d_string;
            size_t              POS       = DATA[ti].d_pos;
            const char         *SHIFTLEXP = DATA[ti].d_shiftlexp;
            const char         *SHIFTREXP = DATA[ti].d_shiftrexp;

            if (veryVerbose) {
                T_ P_(LINE) P_(VALUE) P_(POS) P_(SHIFTLEXP) P(SHIFTREXP);
            }

            bsl::string value(VALUE);
            bsl::string lexpected(SHIFTLEXP);
            bsl::string rexpected(SHIFTREXP);

            Obj mX1(value);    const Obj& X1 = mX1;  // shift left
            Obj mX2(value);    const Obj& X2 = mX2;  // shift right
            Obj mY(lexpected); const Obj& Y  = mY;
            Obj mZ(rexpected); const Obj& Z  = mZ;

            mX1 <<= POS;
            mX2 >>= POS;

            if (veryVeryVerbose) {
                T_ T_ P_(X1) P_(X2) P_(Y) P(Z);
            }

            LOOP_ASSERT(LINE, X1 == Y);
            LOOP_ASSERT(LINE, X2 == Z);
        }

        }

        {

        static const struct {
            unsigned int d_lineNum;    // source line number
            const char*  d_string;     // bitset string
            size_t       d_pos;        // amount to shift by
            const char*  d_shiftlexp;  // expected string after left shift
            const char*  d_shiftrexp;  // expected string after right shift
        } DATA[] = {
   //LINE  VALUE                                POS SHIFTLEXP     SHIFTREXP
   //----  -----------------------------------  --- ------------  ------------
   { L_,   "000000000000000000000000000000000", 0,
                                          "000000000000000000000000000000000",
                                          "000000000000000000000000000000000"},
   { L_,   "000000000000000000000000000000000", 1,
                                          "000000000000000000000000000000000",
                                          "000000000000000000000000000000000"},
   { L_,   "000000000000000000000000000000000", 16,
                                          "000000000000000000000000000000000",
                                          "000000000000000000000000000000000"},
   { L_,   "000000000000000000000000000000000", 31,
                                          "000000000000000000000000000000000",
                                          "000000000000000000000000000000000"},
   { L_,   "000000000000000000000000000000000", 32,
                                          "000000000000000000000000000000000",
                                          "000000000000000000000000000000000"},
   { L_,   "000000000000000000000000000000000", 33,
                                          "000000000000000000000000000000000",
                                          "000000000000000000000000000000000"},

   { L_,   "111111111111111111111111111111111", 0,
                                          "111111111111111111111111111111111",
                                          "111111111111111111111111111111111"},
   { L_,   "111111111111111111111111111111111", 1,
                                          "111111111111111111111111111111110",
                                          "011111111111111111111111111111111"},
   { L_,   "111111111111111111111111111111111", 16,
                                          "111111111111111110000000000000000",
                                          "000000000000000011111111111111111"},
   { L_,   "111111111111111111111111111111111", 31,
                                          "110000000000000000000000000000000",
                                          "000000000000000000000000000000011"},
   { L_,   "111111111111111111111111111111111", 32,
                                          "100000000000000000000000000000000",
                                          "000000000000000000000000000000001"},
   { L_,   "111111111111111111111111111111111", 33,
                                          "000000000000000000000000000000000",
                                          "000000000000000000000000000000000"},

   { L_,   "100000000000000000000000000000001", 0,
                                          "100000000000000000000000000000001",
                                          "100000000000000000000000000000001"},
   { L_,   "100000000000000000000000000000001", 1,
                                          "000000000000000000000000000000010",
                                          "010000000000000000000000000000000"},
   { L_,   "100000000000000000000000000000001", 16,
                                          "000000000000000010000000000000000",
                                          "000000000000000010000000000000000"},
   { L_,   "100000000000000000000000000000001", 31,
                                          "010000000000000000000000000000000",
                                          "000000000000000000000000000000010"},
   { L_,   "100000000000000000000000000000001", 32,
                                          "100000000000000000000000000000000",
                                          "000000000000000000000000000000001"},
   { L_,   "100000000000000000000000000000001", 33,
                                          "000000000000000000000000000000000",
                                          "000000000000000000000000000000000"},

   { L_,   "000010000100000000000001000010000", 0,
                                          "000010000100000000000001000010000",
                                          "000010000100000000000001000010000"},
   { L_,   "000010000100000000000001000010000", 1,
                                          "000100001000000000000010000100000",
                                          "000001000010000000000000100001000"},
   { L_,   "000010000100000000000001000010000", 16,
                                          "000000010000100000000000000000000",
                                          "000000000000000000001000010000000"},
   { L_,   "000010000100000000000001000010000", 31,
                                          "000000000000000000000000000000000",
                                          "000000000000000000000000000000000"},
   { L_,   "000010000100000000000001000010000", 32,
                                          "000000000000000000000000000000000",
                                          "000000000000000000000000000000000"},
   { L_,   "000010000100000000000001000010000", 33,
                                          "000000000000000000000000000000000",
                                          "000000000000000000000000000000000"},
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        const int TESTSIZE = 33;  // num bits of char + 1
        if (verbose) cout <<
           "\nTesting shift operators with bitset<" << TESTSIZE << ">" << endl;

        typedef bsl::bitset<TESTSIZE> Obj;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const unsigned int  LINE      = DATA[ti].d_lineNum;
            const char         *VALUE     = DATA[ti].d_string;
            size_t              POS       = DATA[ti].d_pos;
            const char         *SHIFTLEXP = DATA[ti].d_shiftlexp;
            const char         *SHIFTREXP = DATA[ti].d_shiftrexp;

            if (veryVerbose) {
                T_ P_(LINE) P_(VALUE) P_(POS) P_(SHIFTLEXP) P(SHIFTREXP);
            }

            bsl::string value(VALUE);
            bsl::string lexpected(SHIFTLEXP);
            bsl::string rexpected(SHIFTREXP);

            Obj mX1(value);    const Obj& X1 = mX1;  // shift left
            Obj mX2(value);    const Obj& X2 = mX2;  // shift right
            Obj mY(lexpected); const Obj& Y  = mY;
            Obj mZ(rexpected); const Obj& Z  = mZ;

            mX1 <<= POS;
            mX2 >>= POS;

            if (veryVeryVerbose) {
                T_ T_ P_(X1) P_(X2) P_(Y) P(Z);
            }

            LOOP_ASSERT(LINE, X1 == Y);
            LOOP_ASSERT(LINE, X2 == Z);
        }

        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // STRING CONSTRUCTOR TEST
        //
        // Concerns:
        //   Ensure that a bitset can be constructed from both native strings
        //   and bslstl strings.
        //
        // Plan:
        //   Using the table-driven technique, construct bitset from both
        //   native and bslstl strings.  Verify the value of the constructed
        //   bitset is as expected.
        //
        // Testing:
        //   bitset(native_std::basic_string, size_type, size_type);
        //   bitset(bsl::basic_string, size_type, size_type);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "STRING CONSTRUCTOR TEST"
                          << endl << "=======================" << endl;


        const int TESTSIZE = 32;  // 'bitset' size.
        typedef bsl::bitset<TESTSIZE> Obj;

        static const struct {
            unsigned int d_lineNum;  // source line number
            unsigned int d_value;    // bitset value
            const char*  d_string;   // bitset string
        } DATA[] = {
            //LINE  VALUE         STRING
            //----  ----------    -----------------------------------
            { L_,   0,            "00000000000000000000000000000000" },
            { L_,   0x10101010,   "00010000000100000001000000010000" },
            { L_,   0xabcdef01,   "10101011110011011110111100000001" },
            { L_,   0x12345678,   "00010010001101000101011001111000" },
            { L_,   0xffffffff,   "11111111111111111111111111111111" },
            { L_,   0x87654321,   "10000111011001010100001100100001" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nTesting constructor with native string"
                          << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const unsigned int LINE   = DATA[ti].d_lineNum;
            const unsigned int VALUE  = DATA[ti].d_value;
            const char *       STRING = DATA[ti].d_string;

            if (veryVerbose) { T_ P_(LINE) P_(VALUE) P(STRING) }

//            Obj mX(native_std::string(STRING));         // fails to compile
//            Obj mX(native_std::string(STRING), 0, ~0);  // works

            native_std::string s(STRING);
            Obj mX(s);
            const Obj& X = mX;

            if (veryVeryVerbose) { T_ T_ P(X) }
            LOOP_ASSERT(LINE, verifyBitset(mX, STRING));
            LOOP3_ASSERT(LINE, VALUE, X.to_ulong(), VALUE == X.to_ulong());
        }

        if (verbose) cout << "\nTesting constructor with bslstl string"
                          << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const unsigned int LINE   = DATA[ti].d_lineNum;
            const unsigned int VALUE  = DATA[ti].d_value;
            const char *       STRING = DATA[ti].d_string;

            if (veryVerbose) { T_ P_(LINE) P_(VALUE) P(STRING) }

//            Obj mX(bsl::string(STRING));         // fails to compile
//            Obj mX(bsl::string(STRING), 0, ~0);  // works

            bsl::string s(STRING);
            Obj mX(s);
            const Obj& X = mX;

            if (veryVeryVerbose) { T_ T_ P(X) }
            LOOP_ASSERT(LINE, verifyBitset(mX, STRING));
            LOOP3_ASSERT(LINE, VALUE, X.to_ulong(), VALUE == X.to_ulong());
        }

      } break;

      case 2: {
        // --------------------------------------------------------------------
        // DEFAULT CONSTRUCTOR TEST
        //   Ensure that the default constructor leaves the object in the
        //   expected state for different initial sizes.
        //
        // Concerns:
        //: 1 All N bits in a default-constructed bitset<N> are initially 0.
        //:
        //: 2 bitset<N> implies size()==N.
        //
        // Plan:
        //   Default construct bitsets of different sizes and check 'size',
        //   'none', and 'any'.
        //
        // TESTING:
        //  Default construction, 'size', 'none', 'any'
        //---------------------------------------------------------------------

        if (verbose) cout << endl << "DEFAULT CONSTRUCTOR TEST"
                          << endl << "========================" << endl;

        {
            enum { SIZE = 1 };
            if (verbose) cout << "\tCheck bitset<" << SIZE << ">" << endl;

            bsl::bitset<SIZE> v;

            ASSERT(SIZE == v.size());
            ASSERT(v.none());
            ASSERT(!v.any());

            v[0] = 1;

            ASSERT(!v.none());
            ASSERT(v.any());
        }

        {
            enum { SIZE = 2 };
            if (verbose) cout << "\tCheck bitset<" << SIZE << ">" << endl;

            bsl::bitset<SIZE> v;

            ASSERT(SIZE == v.size());
            ASSERT(v.none());
            ASSERT(!v.any());

            v[0] = 1;

            ASSERT(!v.none());
            ASSERT(v.any());
        }

        {
            enum { SIZE = 7 };
            if (verbose) cout << "\tCheck bitset<" << SIZE << ">" << endl;

            bsl::bitset<SIZE> v;

            ASSERT(SIZE == v.size());
            ASSERT(v.none());
            ASSERT(!v.any());

            v[0] = 1;

            ASSERT(!v.none());
            ASSERT(v.any());
        }

        {
            enum { SIZE = 8 };
            if (verbose) cout << "\tCheck bitset<" << SIZE << ">" << endl;

            bsl::bitset<SIZE> v;

            ASSERT(SIZE == v.size());
            ASSERT(v.none());
            ASSERT(!v.any());

            v[0] = 1;

            ASSERT(!v.none());
            ASSERT(v.any());
        }

        {
            enum { SIZE = 9 };
            if (verbose) cout << "\tCheck bitset<" << SIZE << ">" << endl;

            bsl::bitset<SIZE> v;

            ASSERT(SIZE == v.size());
            ASSERT(v.none());
            ASSERT(!v.any());

            v[0] = 1;

            ASSERT(!v.none());
            ASSERT(v.any());
        }

        {
            enum { SIZE = 31 };
            if (verbose) cout << "\tCheck bitset<" << SIZE << ">" << endl;

            bsl::bitset<SIZE> v;

            ASSERT(SIZE == v.size());
            ASSERT(v.none());
            ASSERT(!v.any());

            v[0] = 1;

            ASSERT(!v.none());
            ASSERT(v.any());
        }

        {
            enum { SIZE = 32 };
            if (verbose) cout << "\tCheck bitset<" << SIZE << ">" << endl;

            bsl::bitset<SIZE> v;

            ASSERT(SIZE == v.size());
            ASSERT(v.none());
            ASSERT(!v.any());

            v[0] = 1;

            ASSERT(!v.none());
            ASSERT(v.any());
        }

        {
            enum { SIZE = 33 };
            if (verbose) cout << "\tCheck bitset<" << SIZE << ">" << endl;

            bsl::bitset<SIZE> v;

            ASSERT(SIZE == v.size());
            ASSERT(v.none());
            ASSERT(!v.any());

            v[0] = 1;

            ASSERT(!v.none());
            ASSERT(v.any());
        }

        {
            enum { SIZE = 63 };
            if (verbose) cout << "\tCheck bitset<" << SIZE << ">" << endl;

            bsl::bitset<SIZE> v;

            ASSERT(SIZE == v.size());
            ASSERT(v.none());
            ASSERT(!v.any());

            v[0] = 1;

            ASSERT(!v.none());
            ASSERT(v.any());
        }

        {
            enum { SIZE = 64 };
            if (verbose) cout << "\tCheck bitset<" << SIZE << ">" << endl;

            bsl::bitset<SIZE> v;

            ASSERT(SIZE == v.size());
            ASSERT(v.none());
            ASSERT(!v.any());

            v[0] = 1;

            ASSERT(!v.none());
            ASSERT(v.any());
        }

        {
            enum { SIZE = 65 };
            if (verbose) cout << "\tCheck bitset<" << SIZE << ">" << endl;

            bsl::bitset<SIZE> v;

            ASSERT(SIZE == v.size());
            ASSERT(v.none());
            ASSERT(!v.any());

            v[0] = 1;

            ASSERT(!v.none());
            ASSERT(v.any());
        }

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   We want to demonstrate a base-line level of correct operation of
        //   the following methods and operators:
        //     - default and value constructors.
        //     - equality operators: 'operator==' and 'operator!='.
        //     - primary manipulators: 'reset' and 'operator|='.
        //     - basic accessors: 'operator[]'.
        //
        // Plan:
        //   Create four test objects using the default, initializing, and copy
        //   constructors.  Exercise the basic value-semantic methods and the
        //   equality operators using the test objects.  Invoke the primary
        //   manipulator [5, 6, 7].  Use the basic accessors to verify the
        //   expected results.  Display object values frequently in verbose
        //   mode.  Note that 'VA', 'VB' and 'VC' denote unique, but otherwise
        //   arbitrary, object values, while 'U' denotes the valid, but
        //   "unknown", default object value.
        //
        //    1. Create an object x1 (init to VA)   { x1:VA       }
        //    3. Create an object x3 (default ctor) { x1:VA x3:U  }
        //    5. Set x3 using 'update' (set to VB)  { x1:VA x3:VB }
        //    6. Change x1 using 'reset'            { x1:U  x3:VB }
        //    7. Change x1 ('update', set to VC)    { x1:VC x3:VB }
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST"
                          << endl << "==============" << endl;

        const int TESTSIZE = 32;  // 'bitset' size.
        typedef bsl::bitset<TESTSIZE> Obj;

        static const struct {
            int         d_lineNum;  // source line number
            int         d_value;    // bitset value
            const char* d_string;   // bitset string
        } DATA[] = {
            //LINE  VALUE         STRING
            //----  ----------    -----------------------------------
            { L_,   0,            "00000000000000000000000000000000" },
            { L_,   0x10101010,   "00010000000100000001000000010000" },
            { L_,   0xabcdef01,   "10101011110011011110111100000001" },
            { L_,   0x12345678,   "00010010001101000101011001111000" },
        };

        const int    SA = DATA[1].d_value,
                     SB = DATA[2].d_value,
                     SC = DATA[3].d_value;
        const char  *VA = DATA[1].d_string,
                    *VB = DATA[2].d_string,
                    *VC = DATA[3].d_string,
                    *VU = DATA[0].d_string;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1. Create an object x1 (init to SA)."
                             "\t\t{ x1:SA }" << endl;
        Obj mX1(SA);
        const Obj& X1 = mX1;
        if (verbose) { T_ P(X1); }

        if (verbose) cout << "\ta. Check initial state of x1." << endl;
        ASSERT(verifyBitset(X1, VA));

        if (verbose) cout << "\tb. Try equality operators: x1 <op> x1."
                          << endl;
        ASSERT(1 == (X1 == X1));
        ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Create an object x3 (default ctor)."
                             "\t\t{ x1:SA x2:SA x3:U }" << endl;
        Obj mX3;
        const Obj& X3 = mX3;
        if (verbose) { T_ P(X3); }

        if (verbose) cout << "\ta. Check initial state of x3." << endl;
        ASSERT(verifyBitset(X3, VU));

        if (verbose) cout << "\tb. Try equality operators: "
                             "x3 <op> x1, x2, x3." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5. Set x3 using primary manip (set to VB)."
                             "\t\t{ x1:SA x2:SA x3:SB x4:U }" << endl;
        mX3.reset();
        mX3 |= Obj(SB);
        if (verbose) { T_ P(X3); }

        if (verbose) cout << "\ta. Check new state of x3." << endl;
        ASSERT(verifyBitset(X3, VB));

        if (verbose) cout << "\tb. Try equality operators: "
                             "x3 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6. Change x1 using 'reset'."
                             "\t\t{ x1:U x2:SA x3:SB x4:U }" << endl;
        mX1.reset();
        if (verbose) { T_ P(X1); }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(verifyBitset(X1, VU));

        if (verbose) cout << "\tb. Try equality operators: "
                             "x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 7. Change x1 (primary manip, set to VC)."
                             "\t\t{ x1:SC x2:SA x3:SB x4:U }" << endl;
        mX1.reset();
        mX1 |= Obj(SC);
        if (verbose) { T_ P(X1); }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(verifyBitset(X1, VC));

        if (verbose) cout << "\tb. Try equality operators: "
                             "x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));


      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
