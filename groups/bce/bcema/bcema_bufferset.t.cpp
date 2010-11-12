// bcema_bufferset.h -*-C++-*-
#include <bcema_bufferset.h>

#include <bdema_bufferallocator.h>              // for testing only
#include <bdema_testallocator.h>                // for testing only
#include <bdema_testallocatorexception.h>       // for testing only
#include <bdema_defaultallocatorguard.h>        // for testing only

#include <cstdlib>     // atoi()
#include <iostream>
#include <sstream>
#include <vector>
#include <cctype>      // isdigit() isupper() islower()

using namespace BloombergLP;
using std::cout; using std::flush; using std::endl; using std::cerr;

//==========================================================================
//                             TEST PLAN
//--------------------------------------------------------------------------
//                              Overview
//                              --------
// All classes in this component are a value-semantic types that represent
// big-endian integer types.  They have the same value if they have the
// same in-core big endian representation.
//
//--------------------------------------------------------------------------
//
// CLASS METHODS
// [10] static int maxSupportedBdexVersion();
//
// CREATORS
//
// MANIPULATORS
//
// ACCESSORS
//
// FREE OPERATORS
//--------------------------------------------------------------------------
// [ 1] BREATHING TEST

//==========================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

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
//                  STANDARD BDEMA EXCEPTION TEST MACROS
//-----------------------------------------------------------------------------

#ifdef BDE_BUILD_TARGET_EXC
#define BEGIN_BDEMA_EXCEPTION_TEST {                                       \
    {                                                                      \
        static int firstTime = 1;                                          \
        if (veryVerbose && firstTime) cout <<                              \
            "\t\tBDEMA EXCEPTION TEST -- (ENABLED) --" << endl;            \
        firstTime = 0;                                                     \
    }                                                                      \
    if (veryVeryVerbose) cout <<                                           \
        "\t\tBegin bdema exception test." << endl;                         \
    int bdemaExceptionCounter = 0;                                         \
    testAllocator.setAllocationLimit(bdemaExceptionCounter);               \
    do {                                                                   \
        try {

#define END_BDEMA_EXCEPTION_TEST                                           \
        } catch (bdema_TestAllocatorException& e) {                        \
            if (veryVeryVerbose) cout << endl << "\t*** BEDMA_EXCEPTION: " \
                << "alloc limit = " << bdemaExceptionCounter << ", "       \
                << "last alloc size = " << e.numBytes() << " ***" << endl; \
            testAllocator.setAllocationLimit(++bdemaExceptionCounter);     \
            continue;                                                      \
        }                                                                  \
        testAllocator.setAllocationLimit(-1);                              \
        break;                                                             \
    } while (1);                                                           \
    if (veryVeryVerbose) cout <<                                           \
        "\t\tEnd bdema exception test." << endl;                           \
}
#else
#define BEGIN_BDEMA_EXCEPTION_TEST                                         \
{                                                                          \
    static int firstTime = 1;                                              \
    if (verbose && firstTime) { cout <<                                    \
        "\t\tBDEMA EXCEPTION TEST -- (NOT ENABLED) --" << endl;            \
        firstTime = 0;                                                     \
    }                                                                      \
}
#define END_BDEMA_EXCEPTION_TEST
#endif

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_() cout << '\t' << flush;


class testBufferSetManager : public bcema_BufferSetManager
{
    private:
    // not implemented
    testBufferSetManager(const testBufferSetManager&);
    testBufferSetManager& operator=(const testBufferSetManager&);
    
    public:
    // CREATORS
    testBufferSetManager() {};
    ~testBufferSetManager() {};

    void deallocateBuffers(bcema_BufferSet*) {};
    void growBufferSet(bcema_BufferSet*, std::size_t) {};
};

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? std::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bdema_TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 11: {
        // --------------------------------------------------------------------
        // TESTING INITIAL-VALUE CONSTRUCTORS:
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting bcema_Buffer." << endl;
        {
            typedef bcema_Buffer Obj;

            Obj mX; const Obj& X = mX;
            mX.setBuffer((char *) ULONG_MAX, 2);
            Obj mY((char *) ULONG_MAX, 2); const Obj& Y = mY;
            ASSERT(X == Y);
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING STREAMING FUNCTIONALITY:
        //
        // Concerns:
        //  1. The (free) streaming operators '<<' and '>>' are implemented
        //     using the respective member functions 'bdexStreamOut' and
        //     'bdexStreamIn'.
        //  2. Ensure that streaming works under the following conditions:
        //      VALID - may contain any sequence of valid values.
        //      EMPTY - valid, but contains no data.
        //      INVALID - may or may not be empty.
        //      INCOMPLETE - the stream is truncated, but otherwise valid.
        //      CORRUPTED - the data contains explicitly inconsistent fields.
        //
        // Plan:
        //  To address concern 1, perform a trivial direct (breathing) test of
        //  the 'bdexStreamOut' and 'bdexStreamIn' methods.  Note that the rest
        //  of the testing will use the stream operators.
        //
        //  To address concern 2, specify a set S of unique object values with
        //  substantial and varied differences, ordered by increasing
        //  complexity.
        //
        //  VALID STREAMS (and exceptions)
        //    Using all combinations of (u, v) in S X S, stream-out the value
        //    of u into a buffer and stream it back into (an independent
        //    instance of) v, and assert that u == v.
        //
        //  EMPTY AND INVALID STREAMS
        //    For each x in S, attempt to stream into (a temporary copy of) x
        //    from an empty and then invalid stream.  Verify after each try
        //    that the object is unchanged and that the stream is invalid.
        //
        //  INCOMPLETE (BUT OTHERWISE VALID) DATA
        //    Write 3 distinct objects to an output stream buffer of total
        //    length N.  For each partial stream length from 0 to N - 1,
        //    construct a truncated input stream and attempt to read into
        //    objects initialized with distinct values.  Verify values of
        //    objects that are either successfully modified or left entirely
        //    unmodified,  and that the stream became invalid immediately
        //    after the first incomplete read.
        //
        //  CORRUPTED DATA
        //    We will assume that the incomplete test fails every field,
        //    including a char (multi-byte representation).  Hence we need to
        //    produce values that are inconsistent with a valid value and
        //    verify that they are detected.  Use the underlying stream package
        //    to simulate an instance of a typical valid (control) stream and
        //    verify that it can be streamed in successfully.  Then for each
        //    data field in the stream (beginning with the version number),
        //    provide one or more similar tests with that data field corrupted.
        //    After each test, verify that the object is unchanged after
        //    streaming.
        //
        // Testing:
        //   int maxSupportedBdexVersion() const
        //   STREAM& bdexStreamIn(STREAM&, int)
        //   STREAM& bdexStreamOut(STREAM&, int) const
        //   operator>>(bdex_InStream&, Obj&)
        //   operator<<(bdex_OutStream&, Obj bdecs_PackedCalendar&)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Streaming Functionality" << endl
                          << "===============================" << endl;
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR:
        //
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting bcema_Buffer." << endl;
        {
            typedef bcema_Buffer Obj;

            Obj mX; const Obj& X = mX;
            mX.setBuffer((char *) ULONG_MAX, 2);

            Obj mY(mX); const Obj& Y = mY;
            Obj mZ; const Obj& Z = mZ;
            ASSERT(X == Y);
            ASSERT(Z != Y);

            mZ = mX;
            ASSERT(Z == X);
            ASSERT(Z == Y);

            mX.setBuffer(0, 0);
            ASSERT(Z != X);
            ASSERT(Z == Y);
        }
            
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION, g:
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR:
        //
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting bcema_Buffer." << endl;
        {
            typedef bcema_Buffer Obj;

            Obj mX; const Obj& X = mX;
            Obj mY(mX); const Obj& Y = mY;
            ASSERT(X == Y);
            
            mX.setBuffer((char *) ULONG_MAX, 2);
            ASSERT(X != Y);
            ASSERT(0 == Y.buffer());
            ASSERT(0 == Y.length());

            Obj mZ(X); const Obj& Z = mZ;
            ASSERT(X == Z);
            ASSERT(Y != Z);
        }
            

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS: '==' and '!='
        //
        // --------------------------------------------------------------------
        
        if (verbose) cout << "\nTesting bcema_Buffer." << endl;
        {
            typedef bcema_Buffer Obj;

            Obj mX; const Obj& X = mX;
            Obj mY; const Obj& Y = mY;
            ASSERT(  (X == Y));
            ASSERT(! (X != Y));
            ASSERT(0 == X.buffer());
            ASSERT(0 == X.length());
            
            mX.setBuffer((char *) ULONG_MAX, X.length());
            ASSERT(! (X == Y));
            ASSERT(  (X != Y));

            mY.setBuffer((char *) ULONG_MAX, X.length());
            ASSERT(  (X == Y));
            ASSERT(! (X != Y));
            
            mX.setBuffer(mX.buffer(), 1);
            ASSERT(! (X == Y));
            ASSERT(  (X != Y));

            mY.setBuffer(mX.buffer(), 1);
            ASSERT(  (X == Y));
            ASSERT(! (X != Y));
        }
            

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR and 'print' method:
        //
        // Concerns:
        //  We want to ensure that the 'print' method correctly formats
        //  our objects output with any valid 'level' and 'spacesPerLevel'
        //  values and returns the specified stream and does not use
        //  any memory.
        //
        // Plan:
        //  Exercise the print method with different levels and spaces and
        //  compare the result against a generated string.
        //
        // Tactics:
        //      - Ad-Hoc Data Selection Method
        //      - Table-Driven Implementation Technique
        //
        // Testing:
        //      std::ostream& Obj::print(
        //                                  std::ostream& stream,
        //                                  int           level,
        //                                  int           spacesPerLevel) const
        //      std::ostream& operator<<(std::ostream& stream,
        //                               const Obj&    calendar)
        // --------------------------------------------------------------------

#if 0
        static const struct {
            int   d_level;
            int   d_spacesPerLevel;
        } DATA[] = {
           //LEVEL SPACE PER LEVEL
            {0,    -1,             },
            {0,    -1,             },
            {0,    -1,             },
            {0,    -1,             },
            {0,    -1,             },
            {0,    -1,             },
            {0,    -1,             },
            {2,    -1,             },
            {3,    -2,             },
            {1,     2,             },
            {1,     2,             },
            {1,    -2,             },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;
#endif

        if (verbose) cout << endl
                          << "Testing 'print' and 'operator<<'." << endl
                          << "=================================" << endl;

#if 0
        if (verbose) cout << "\nTesting BigEndianInt16." << endl;
        {
            typedef BigEndianInt16 Obj;
            const short VALUES[] = {1, SHRT_MAX, SHRT_MIN, -1, 0, 123, -123};
            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const Obj X = Obj::make(VALUES[i]);

                std::stringstream streamValue;
                streamValue << VALUES[i];

                for (int j = 0; j < NUM_DATA; ++j) {

                    const int LEVEL = DATA[j].d_level;
                    const int SPACES = DATA[j].d_spacesPerLevel;

                    {
                        std::stringstream ss;
                        X.print(ss, LEVEL, SPACES);

                        std::stringstream ss2;
                        if (LEVEL > 0) {
                            ss2 << std::string(std::abs(SPACES) * LEVEL, ' ');
                        }
                        ss2 << streamValue.str();
                        if (SPACES >= 0) {
                            ss2 << '\n';
                        }
                        LOOP2_ASSERT(i, j, ss2.str() == ss.str());
                    }

                }
                std::stringstream ss;
                ss << X;
                LOOP_ASSERT(i, ss.str() == streamValue.str());
            }
        }
#endif
        if (verbose) cout << "\nTesting bcema_Buffer." << endl;
        {
            typedef bcema_Buffer Obj;
            std::ostringstream os;

            char *buf = new char[150];
            std::memset(buf, 'A', 150);
            Obj mX; const Obj& X = mX;
            mX.setBuffer(buf, 150);
            X.print(os);

            delete [] buf;
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //
        // Concerns:
        //  1. operator T() returns the expected value
        //
        // Plan:
        //  To address concern 1, we will construct different objects
        //  and verify that T() returns the expected value.
        //
        //  Tactics:
        //      - Ad Hoc test data selection method
        //      - Table-Driven test case implementation technique
        //
        //  Testing:
        //      BigEndianInt16::operator  short() const
        //      BigEndianUint16::operator unsigned short() const
        //      BigEndianInt32::operator  int() const
        //      BigEndianUint32::operator unsigned int() const
        //      BigEndianInt64::operator bdes_Int64::Int64() const
        //      BigEndianUint64::operator bdes_Int64::Uint64() const
        //  -------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING BASIC ACCESSORS" << endl
                          << "=======================" << endl;

#if 0
        if (verbose) cout << "\nTesting BigEndianInt16" << endl;
        {
            typedef BigEndianInt16 Obj;

            {
                const Obj X = Obj::make(0);
                ASSERT(0 == short(X));
            }
            for (int i = 1; i < sizeof(short) * 8; ++i) {
                const Obj X = Obj::make(1 << (i - 1));
                LOOP_ASSERT(i, (1 << (i - 1)) == short(X));
            }
        }
#endif
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTIONS
        //
        // --------------------------------------------------------------------

        // Nothing here

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS
        //
        // We want to exercise the set of primary manipulators, which can put
        // the object in any state.
        //
        // Concerns:
        //
        //  Note that there is no "stretching" in this object.  We are adopting
        //  a black-box attitude while testing this function with regard to the
        //  containers used by the object.
        //
        // Plan:
        //
        //  Tactics:
        //      - Ad-Hoc Data Selection Method
        //      - Array Implementation technique
        //
        //  Testing:
        //      bcema_Buffer::bcema_Buffer()
        //      bcema_Buffer::setBuffer(char *buffer, std::size_t length)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING PRIMARY MANIPULATORS" << endl
                          << "============================" << endl;

        if (verbose) cout << "\nTesting bcema_Buffer." << endl;
        {
            typedef bcema_Buffer Obj;
            char* PTRVALUES[] = {0, (char *) 1, (char *) 0xff,
                                      (char *) 0x0ff00, (char *) ULONG_MAX};
            const std::size_t SIZEVALUES[] = {0, 1, 138, INT_MAX, -1};
            const int NUM_PTRVALUES = sizeof PTRVALUES / sizeof *PTRVALUES;
            const int NUM_SIZEVALUES = sizeof SIZEVALUES / sizeof *SIZEVALUES;

            const bdema_DefaultAllocatorGuard DAG(&testAllocator);
            for (int i = 0; i < NUM_PTRVALUES; ++i) {
                for (int j = 0; i < NUM_SIZEVALUES; ++i) {
                    const int previousTotal = testAllocator.numBlocksTotal();

                    Obj mX; const Obj& X = mX;
                    ASSERT(0 == X.buffer());
                    ASSERT(0 == X.length());

                    mX.setBuffer(PTRVALUES[i], SIZEVALUES[j]);
                    ASSERT(PTRVALUES[i]  ==  X.buffer());
                    ASSERT(PTRVALUES[i]  == mX.buffer());
                    ASSERT(SIZEVALUES[j] ==  X.length());

                    ASSERT(testAllocator.numBlocksTotal() == previousTotal);
                }
            }
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Developers' Sandbox.
        //
        // Concerns:
        //   We want to exercise basic value-semantic functionality.  In
        //   particular we want to demonstrate a base-line level of correct
        //   operation of the following methods and operators:
        //      - default and copy constructors (and also the destructor)
        //      - the assignment operator (including aliasing)
        //      - equality operators: 'operator==()' and 'operator!=()'
        //      - the (test-driver supplied) output operator: 'operator<<()'
        //      - primary manipulators: 'push_back' and 'clear' methods
        //      - basic accessors: 'size' and 'operator[]()'
        //   In addition we would like to exercise objects with potentially
        //   different internal organizations representing the same value.
        //
        // Plan:
        //   Create four objects using the default, make and
        //   copy constructors.  Exercise these objects using primary
        //   manipulators, basic accessors, equality operators, and the
        //   assignment operator.  Invoke the primary (black box) manipulator
        //   [3&5], copy constructor [2&8], and assignment operator [10&9] in
        //   situations where the internal data (i) does *not* and (ii) *does*
        //   have to resize.  Try aliasing with assignment for a non-empty
        //   instance [11] and allow the result to leave scope, enabling the
        //   destructor to assert internal object invariants.  Display object
        //   values frequently in verbose mode:
        //    1. Create an object x1 (dctor + =).           x1:
        //    2. Create a second object x2 (copy from x1).  x1: x2:
        //    3. Append an element value A to x1).          x1:A x2:
        //    4. Append the same element value A to x2).    x1:A x2:A
        //    5. Append another element value B to x2).     x1:A x2:AB
        //    6. Remove all elements from x1.               x1: x2:AB
        //    7. Create a third object x3 (dctor + =)       x1: x2:AB x3:
        //    8. Create a fourth object x4 (copy of x2).    x1: x2:AB x3: x4:AB
        //    9. Assign x2 = x1 (non-empty becomes empty).  x1: x2: x3: x4:AB
        //   10. Assign x3 = x4 (empty becomes non-empty).  x1: x2: x3:AB x4:AB
        //   11. Assign x4 = x4 (aliasing).                 x1: x2: x3:AB x4:AB
        //
        // Tactics:
        //   - Ad Hoc Test Data Selection Method
        //   - Bruit Force Implemenation Technique
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        if (verbose) cout << "\nTesting bcema_Buffer." << endl;
        {
            typedef bcema_Buffer Obj;
            Obj mVA(0, 1); const Obj& VA = mVA;
            Obj mVB((char *) 0x1, 2); const Obj& VB = mVB;
            Obj mVC((char *) ULONG_MAX, UINT_MAX); const Obj& VC = mVC;
            Obj mVD((char *) 0xdeadbeef, 153); const Obj& VD0= mVD;

            if (verbose) {
                cout << "\n 1. Create an object x1 (init. to VA)."
                     << "\t\t{ x1:VA }"
                     << endl;
            }

            Obj        mX1 = VA;
            const Obj& X1 = mX1;
            if (veryVerbose) {
                T_();  P(X1);
            }

            if (verbose) {
                cout << "\ta. Check initial state of x1." << endl;
            }
            ASSERT(VA == X1);

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 2. Create an object x2 (copy from x1)."
                     << "\t\t{ x1:VA x2:VA }"
                     << endl;
            }

            Obj mX2(X1);  const Obj& X2 = mX2;
            if (veryVerbose) {
                T_();  P(X2);
            }

            if (verbose) {
                cout << "\ta. Check the initial state of x2." << endl;
            }
            ASSERT(VA == X2);

            if (verbose) {
                cout << "\tb. Try equality operators: x2 <op> x1, x2."
                     << endl;
            }
            ASSERT(true == (X2 == X1));    ASSERT(false == (X2 != X1));
            ASSERT(true == (X2 == X2));    ASSERT(false == (X2 != X2));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 3. Set x1 to a new value VB."
                     << "\t\t\t{ x1:VB x2:VA }"
                     << endl;
            }

            mX1 = VB;
            if (veryVerbose) {
                T_();  P(X1);
            }

            if (verbose) {
                cout << "\ta. Check new state of x1." << endl;
            }
            ASSERT(VB == X1);

            if (verbose) {
                cout << "\tb. Try equality operators: x1 <op> x1, x2."
                     << endl;
            }
            ASSERT(true  == (X1 == X1));    ASSERT(false == (X1 != X1));
            ASSERT(false == (X1 == X2));    ASSERT(true  == (X1 != X2));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 4. Create a default object x3()."
                     << "\t\t{ x1:VB x2:VA x3:U }"
                     << endl;
            }

            Obj mX3;  const Obj& X3 = mX3;
            if (veryVerbose) {
                T_();  P(X3);
            }

            //if (verbose) {
            //    cout << "\ta. Check initial state of x3." << endl;
            //}

            if (verbose) {
                cout << "\tb. Try equality operators: x3 <op> x1, x2, x3."
                     << endl;
            }
            ASSERT(false == (X3 == X1));    ASSERT(true  == (X3 != X1));
            ASSERT(false == (X3 == X2));    ASSERT(true  == (X3 != X2));
            ASSERT(true  == (X3 == X3));    ASSERT(false == (X3 != X3));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 5. Create an object x4 (copy from x3)."
                     << "\t\t{ x1:VA x2:VA x3:U  x4:U }"
                     << endl;
            }

            Obj mX4(X3);  const Obj& X4 = mX4;
            if (veryVerbose) {
                T_();  P(X4);
            }

            //if (verbose) {
            //    cout << "\ta. Check initial state of x4." << endl;
            //}
            //ASSERT(0 == X4);
            //ASSERT(isInCoreValueCorrect(0, X4));

            if (verbose) {
                cout << "\tb. Try equality operators: x4 <op> x1, x2, x3, x4."
                     << endl;
            }
            ASSERT(false == (X4 == X1));    ASSERT(true  == (X4 != X1));
            ASSERT(false == (X4 == X2));    ASSERT(true  == (X4 != X2));
            ASSERT(true  == (X4 == X3));    ASSERT(false == (X4 != X3));
            ASSERT(true  == (X4 == X4));    ASSERT(false == (X4 != X4));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 6. Set x3 to a new value VC."
                     << "\t\t\t{ x1:VB x2:VA x3:VC x4:U }"
                     << endl;
            }

            mX3 = VC;
            if (veryVerbose) {
                T_();  P(X3);
            }

            if (verbose) {
                cout << "\ta. Check new state of x3." << endl;
            }
            ASSERT(VC == X3);

            if (verbose) {
                cout << "\tb. Try equality operators: x3 <op> x1, x2, x3, x4."
                     << endl;
            }
            ASSERT(false == (X3 == X1));    ASSERT(true  == (X3 != X1));
            ASSERT(false == (X3 == X2));    ASSERT(true  == (X3 != X2));
            ASSERT(true  == (X3 == X3));    ASSERT(false == (X3 != X3));
            ASSERT(false == (X3 == X4));    ASSERT(true  == (X3 != X4));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 7. Assign x2 = x1."
                     << "\t\t\t\t{ x1:VB x2:VB x3:VC x4:U }"
                     << endl;
            }

            mX2 = X1;
            if (veryVerbose) {
                T_();  P(X2);
            }

            if (verbose) {
                cout << "\ta. Check new state of x2." << endl;
            }
            ASSERT(VB == X2);

            if (verbose) {
                cout << "\tb. Try equality operators: x2 <op> x1, x2, x3, x4."
                     << endl;
            }
            ASSERT(true  == (X2 == X1));    ASSERT(false == (X2 != X1));
            ASSERT(true  == (X2 == X2));    ASSERT(false == (X2 != X2));
            ASSERT(false == (X2 == X3));    ASSERT(true  == (X2 != X3));
            ASSERT(false == (X2 == X4));    ASSERT(true  == (X2 != X4));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 8. Assign x2 = x3."
                     << "\t\t\t\t{ x1:VB x2:VC x3:VC x4:U }"
                     << endl;
            }

            mX2 = X3;
            if (veryVerbose) {
                T_();  P(X2);
            }

            if (verbose) {
                cout << "\ta. Check new state of x2." << endl;
            }
            ASSERT(VC == X2);

            if (verbose) {
                cout << "\tb. Try equality operators: x2 <op> x1, x2, x3, x4."
                     << endl;
            }
            ASSERT(false == (X2 == X1));    ASSERT(true  == (X2 != X1));
            ASSERT(true  == (X2 == X2));    ASSERT(false == (X2 != X2));
            ASSERT(true  == (X2 == X3));    ASSERT(false == (X2 != X3));
            ASSERT(false == (X2 == X4));    ASSERT(true  == (X2 != X4));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\n 9. Assign x1 = x1 (aliasing)."
                     << "\t\t\t{ x1:VB x2:VC x3:VC x4:U }"
                     << endl;
            }

            mX1 = X1;
            if (veryVerbose) {
                T_();  P(X1);
            }

            if (verbose) {
                cout << "\ta. Check new state of x1." << endl;
            }
            ASSERT(VB == X1);

            if (verbose) {
                cout << "\tb. Try equality operators: x1 <op> x1, x2, x3, x4."
                     << endl;
            }
            ASSERT(true  == (X1 == X1));    ASSERT(false == (X1 != X1));
            ASSERT(false == (X1 == X2));    ASSERT(true  == (X1 != X2));
            ASSERT(false == (X1 == X3));    ASSERT(true  == (X1 != X3));
            ASSERT(false == (X1 == X4));    ASSERT(true  == (X1 != X4));
        }
        if (verbose) cout << "\nTesting bcema_BufferSetManager." << endl;
        {
            testBufferSetManager test;
        }
        if (verbose) cout << "\nTesting bcema_BufferSet." << endl;
        {
            typedef bcema_BufferSet Obj;
            Obj mX; const Obj& X = mX;
            ASSERT(0 == X.numBuffers());
            ASSERT(0 == X.length());

            bcema_Buffer buf((char *) 1, 3);
            bcema_Buffer buf2((char *) 2, 4);
            mX.append(buf);
            ASSERT(1 == X.numBuffers());
            ASSERT(3 == X.length());
            mX.append(buf2);
            ASSERT(2 == X.numBuffers());
            ASSERT(7 == X.length());

            ASSERT((char *) 1 ==  X[0].buffer());
            ASSERT((char *) 2 ==  X[1].buffer());
            ASSERT((char *) 1 == mX[0].buffer());
            ASSERT((char *) 2 ==  X[1].buffer());
            ASSERT(3 ==  X[0].length());
            ASSERT(4 ==  X[1].length());

            mX.removeAll();
            ASSERT(0 == X.numBuffers());
            ASSERT(0 == X.length());
        }
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
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
