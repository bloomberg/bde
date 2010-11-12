// bdea_array_char.t.cpp       -*-C++-*-

#include <bdea_array_char.h>

#include <bdes_platformutil.h>                  // for testing only
#include <bdema_bufferallocator.h>              // for testing only
#include <bdema_testallocator.h>                // for testing only
#include <bdema_testallocatorexception.h>       // for testing only
#include <bdex_testoutstream.h>                 // for testing only
#include <bdex_testinstream.h>                  // for testing only
#include <bdex_testinstreamexception.h>         // for testing only
#include <bdex_byteoutstream.h>                 // for testing only
#include <bdex_byteinstream.h>                  // for testing only

#include <new>         // placement syntax
#include <cstring>     // strlen(), memset(), memcpy(), memcmp()
#include <cstdlib>     // atoi()
#include <iostream>
#include <strstream>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This test plan follows the standard approach for components implementing
// value-semantic containers.  We have chosen as *primary* *manipulators* the
// 'append' and 'removeAll' methods to be used by the generator functions
// 'g' and 'gg'.  Additional helper functions are provided to facilitate
// perturbation of internal state (e.g., capacity).  Note that each
// manipulator must support aliasing, and those that perform memory allocation
// must be tested for exception neutrality via the 'bdema_testallocator'
// component.  Exception neutrality involving streaming is verified using
// 'bdex_testinstream' (and 'bdex_testoutstream').
//
// Note that places where test drivers in this family are likely to require
// adjustment are indicated by the tag: "ADJ".
//-----------------------------------------------------------------------------
// [ 2] bdea_Array(bdema_Allocator *ba = 0);
// [11] bdea_Array(const Explicit& iLen, *ba = 0);
// [11] bdea_Array(int iLen, char iVal, *ba = 0);
// [17] bdea_Array(const InitialCapacity& ne, *ba = 0);
// [11] bdea_Array(const char *sa, int ne, *ba = 0);
// [ 7] bdea_Array(const bdea_Array<char>& original, *ba = 0);
// [ 2] ~bdea_Array();
// [ 9] bdea_Array<char>& operator=(const bdea_Array<char>& rhs);
// [14] char& operator[](int index);
// [13] void append(char item);
// [13] void append(const bdea_Array<char>& sa);
// [13] void append(const bdea_Array<char>& sa, int si, int ne);
// [13] void append(const char *sa, int si, int ne);
// [18] char *data();
// [13] void insert(int di, char item);
// [13] void insert(int di, const bdea_Array<char>& sa);
// [13] void insert(int di, const bdea_Array<char>& sa, int si, int ne);
// [13] void insert(int di, const char *sa, int si, int ne);
// [13] void remove(int index);
// [13] void remove(int index, int ne);
// [ 2] void removeAll();
// [14] void replace(int di, char item);
// [14] void replace(int di, const bdea_Array<char>& sa, int si, int ne);
// [17] void reserveCapacityRaw(int ne);
// [17] void reserveCapacity(int ne);
// [12] void setLengthRaw(int newLength);
// [12] void setLength(int newLength);
// [12] void setLength(int newLength, char iVal);
// [10] bdex_InStream& streamIn(bdex_InStream& stream);
// [10] bdex_InStream& streamIn(bdex_InStream& stream, int version);
// [16] void swap(int index1, int index2);
// [ 4] const char& operator[](int index) const;
// [18] const char *data() const;
// [ 4] int length() const;
// [15] ostream& print(ostream& stream, int level, int spl);
// [10] int maxSupportedBdexVersion() const;
// [10] bdex_OutStream& streamOut(bdex_OutStream& stream) const;
// [10] bdex_OutStream& streamOut(bdex_OutStream& stream, int version) const;
//
// [ 6] operator==(const bdea_Array<char>&, const bdea_Array<char>&);
// [ 6] operator!=(const bdea_Array<char>&, const bdea_Array<char>&);
// [ 5] operator<<(ostream&, const bdea_Array<char>&);
// [10] operator>>(bdex_InStream&, bdea_Array<char>&);
// [10] operator<<(bdex_OutStream&, const bdea_Array<char>&);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [19] USAGE EXAMPLE
// [ 2] BOOTSTRAP: void append(char item); // no aliasing
// [ 3] CONCERN: Is the internal memory organization behaving as intended?
//
// [ 3] void stretch(Obj *object, int size);
// [ 3] void stretchRemoveAll(Obj *object, int size);
// [ 3] int ggg(bdea_Array<char> *object, const char *spec, int vF = 1);
// [ 3] bdea_Array<char>& gg(bdea_Array<char>* object, const char *spec);
// [ 8] bdea_Array<char>   g(const char *spec);

//=============================================================================
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
            "### BDEMA EXCEPTION TEST -- (ENABLED) --" << endl;            \
        firstTime = 0;                                                     \
    }                                                                      \
    if (veryVeryVerbose) cout <<                                           \
        "### Begin bdema exception test." << endl;                         \
    int bdemaExceptionCounter = 0;                                         \
    static int bdemaExceptionLimit = 100;                                  \
    testAllocator.setAllocationLimit(bdemaExceptionCounter);               \
    do {                                                                   \
        try {

#define END_BDEMA_EXCEPTION_TEST                                           \
        } catch (bdema_TestAllocatorException& e) {                        \
            if (veryVerbose && bdemaExceptionLimit || veryVeryVerbose) {   \
                --bdemaExceptionLimit;                                     \
                cout << "(*** " <<                                         \
                bdemaExceptionCounter << ')';                              \
                if (veryVeryVerbose) { cout << " BEDMA_EXCEPTION: "        \
                    << "alloc limit = " << bdemaExceptionCounter << ", "   \
                    << "last alloc size = " << e.numBytes();               \
                }                                                          \
                else if (0 == bdemaExceptionLimit) {                       \
                     cout << " [ Note: 'bdemaExceptionLimit' reached. ]";  \
                }                                                          \
                cout << endl;                                              \
            }                                                              \
            testAllocator.setAllocationLimit(++bdemaExceptionCounter);     \
            continue;                                                      \
        }                                                                  \
        testAllocator.setAllocationLimit(-1);                              \
        break;                                                             \
    } while (1);                                                           \
    if (veryVeryVerbose) cout <<                                           \
        "### End bdema exception test." << endl;                           \
}
#else
#define BEGIN_BDEMA_EXCEPTION_TEST                                         \
{                                                                          \
    static int firstTime = 1;                                              \
    if (verbose && firstTime) { cout <<                                    \
        "### BDEMA EXCEPTION TEST -- (NOT ENABLED) --" << endl;            \
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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdea_Array<char> Obj;
typedef char Element;

const Element VALUES[] = { 'a', 'b', 'c', 'd', 'e' }; // avoid DEFAULT_VALUE

const Element &V0 = VALUES[0], &VA = V0,  // V0, V1, ... are used in
              &V1 = VALUES[1], &VB = V1,  // conjunction with the VALUES array.
              &V2 = VALUES[2], &VC = V2,
              &V3 = VALUES[3], &VD = V3,  // VA, VB, ... are used in
              &V4 = VALUES[4], &VE = V4;  // conjunction with 'g' and 'gg'.

const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

void stretch(Obj *object, int size)
   // Using only primary manipulators, extend the length of the specified
   // 'object' by the specified size.  The resulting value is not specified.
   // The behavior is undefined unless 0 <= size.
{
    ASSERT(object);
    ASSERT(0 <= size);
    for (int i = 0; i < size; ++i) {
        object->append(V0);
    }
    ASSERT(object->length() >= size);
}

void stretchRemoveAll(Obj *object, int size)
   // Using only primary manipulators, extend the capacity of the specified
   // 'object' to (at least) the specified size; then remove all elements
   // leaving 'object' empty.  The behavior is undefined unless 0 <= size.
{
    ASSERT(object);
    ASSERT(0 <= size);
    stretch(object, size);
    object->removeAll();
    ASSERT(0 == object->length());
}

void outerP(const char *leader, const Obj &X)
    // This function is used to avert an uncaught exception on Windows during
    // bdema exception testing.  This can happen, e.g., in test cases with
    // large DATA sets.
{
    cout << leader; P(X);
}

//=============================================================================
//              GENERATOR FUNCTIONS 'g' AND 'gg' FOR TESTING
//-----------------------------------------------------------------------------
// The following functions interpret the given 'spec' in order from left to
// right to configure the object according to a custom language.  Uppercase
// letters [A .. E] correspond to arbitrary (but unique) char values to be
// appended to the 'bdea_Array<char>' object.  A tilde ('~') indicates that
// the logical (but not necessarily physical) state of the object is to be set
// to its initial, empty state (via the 'removeAll' method).
//
// LANGUAGE SPECIFICATION:
// -----------------------
//
// <SPEC>       ::= <EMPTY>   | <LIST>
//
// <EMPTY>      ::=
//
// <LIST>       ::= <ITEM>    | <ITEM><LIST>
//
// <ITEM>       ::= <ELEMENT> | <REMOVE_ALL>
//
// <ELEMENT>    ::= 'A' | 'B' | 'C' | 'D' | 'E'
//                                      // unique but otherwise arbitrary
// <REMOVE_ALL> ::= '~'
//
// Spec String  Description
// -----------  ---------------------------------------------------------------
// ""           Has no effect; leaves the object empty.
// "A"          Append the value corresponding to A.
// "AA"         Append two values both corresponding to A.
// "ABC"        Append three values corresponding to A, B and C.
// "ABC~"       Append three values corresponding to A, B and C and then
//              remove all the elements (set array length to 0).  Note that
//              this spec yields an object that is logically equivalent
//              (but not necessarily identical internally) to one
//              yielded by ("").
// "ABC~DE"     Append three values corresponding to A, B, and C; empty
//              the object; and append values corresponding to D and E.
//
//-----------------------------------------------------------------------------

int ggg(bdea_Array<char> *object, const char *spec, int verboseFlag = 1)
    // Configure the specified 'object' according to the specified 'spec',
    // using only the primary manipulator function 'append' and white-box
    // manipulator 'removeAll'.  Optionally specify a zero 'verboseFlag' to
    // suppress 'spec' syntax error messages.  Return the index of the first
    // invalid character, and a negative value otherwise.  Note that this
    // function is used to implement 'gg' as well as allow for verification of
    // syntax error detection.
{
    enum { SUCCESS = -1 };
    for (int i = 0; spec[i]; ++i) {
        if ('A' <= spec[i] && spec[i] <= 'E') {
            object->append(VALUES[spec[i] - 'A']);
        }
        else if ('~' == spec[i]) {
            object->removeAll();
        }
        else {
            if (verboseFlag) {
                cout << "Error, bad character ('" << spec[i] << "') in spec \""
                     << spec << "\" at position " << i << '.' << endl;
            }
            return i;  // Discontinue processing this spec.
        }
   }
   return SUCCESS;
}

bdea_Array<char>& gg(bdea_Array<char> *object, const char *spec)
    // Return, by reference, the specified object with its value adjusted
    // according to the specified 'spec'.
{
    ASSERT(ggg(object, spec) < 0);
    return *object;
}

bdea_Array<char> g(const char *spec)
    // Return, by value, a new object corresponding to the specified 'spec'.
{
    bdea_Array<char> object((bdema_Allocator *)0);
    return gg(&object, spec);
}

class EqualityTester {
private:
        const Obj   *A;
        const Obj   *B;
public:
        EqualityTester(const Obj *a, const Obj *b);
        ~EqualityTester();
};

// CREATORS
inline EqualityTester::EqualityTester(const Obj *a, const Obj *b)
: A(a)
, B(b)
{
}

inline EqualityTester::~EqualityTester()
{
        ASSERT(*A == *B);
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

#if defined(BDES_PLATFORM__CMP_EPC)
    // The EPC C++ compiler on DGUX has a bug in the constructor for
    // ostrstream that take abuffer and a length.  The constructor will
    // destroy the last byte of the variable immediately preceeding the
    // buffer used to hold the stream.  If the parameter specifying the
    // buffer passed to the constructor is 1 less than the size of the buffer
    // stack integrity is maintained.
    const int epcBug = 1;
#else
    const int epcBug = 0;
#endif

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bdema_TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 19: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Usage Example" << endl
                          << "=====================" << endl;

        const char E1 = 'A';
        const char E2 = 'B';

        bdea_Array<char> a(&testAllocator);
                                        ASSERT( 0 == a.length());

        a.append(E1);                   ASSERT( 1 == a.length());
                                        ASSERT(E1 == a[0]);

        a.append(E2);                   ASSERT( 2 == a.length());
                                        ASSERT(E1 == a[0]);
                                        ASSERT(E2 == a[1]);

        const char E3 = 'C';

        a[0] = E3;                      ASSERT( 2 == a.length());
                                        ASSERT(E3 == a[0]);
                                        ASSERT(E2 == a[1]);

        const char E4 = 'D';

        a.insert(1, E4);                ASSERT( 3 == a.length());
                                        ASSERT(E3 == a[0]);
                                        ASSERT(E4 == a[1]);
                                        ASSERT(E2 == a[2]);

        char buf[100];  memset(buf, 0xff, sizeof buf);  // Scribble on buf.
        ostrstream out(buf, sizeof buf);

        out << '[';
        int len = a.length();
        for (int i = 0; i < len; ++i) {
            out << ' ' << a[i];
        }
        out << " ]" << endl;

        out << ends;  // Terminate the in-core string.

        const char *const EXPECTED = "[ C D B ]\n";

        if (veryVerbose) {
            cout << "EXPECTED: " << EXPECTED;
            cout << "  ACTUAL: " << buf;
        }
        ASSERT(0 == strcmp(EXPECTED, buf));

        a.remove(2);                    ASSERT( 2 == a.length());
                                        ASSERT(E3 == a[0]);
                                        ASSERT(E4 == a[1]);

        a.remove(0);                    ASSERT( 1 == a.length());
                                        ASSERT(E4 == a[0]);

        a.remove(0);                    ASSERT( 0 == a.length());

      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING INTERNAL DATA ACCESS METHODS
        //   The concerns are as follows:
        //     - Data can be called on both 'const' and non-'const' objects.
        //     - The address returned for both is identical to &array[0].
        //     - When called on a 'const' object, the value returned is 'const'
        //       (plan: pass to function with 'const' signatures as proof).
        //     - When called on a non-'const' object, the value returned is
        //       non-'const'  (plan: use it to modify contents as proof).
        //
        // Plan:
        //   For each object X in a set of 'bdea_Array<char>' objects ordered
        //   by increasing length L:
        //     For each valid index position P:
        //        Assert X.data() + P == &X[P] for 'const' and non-'const'
        //        methods.
        //
        //   Ensure (at compile time) that the non-const data() method returns
        //   the address of a modifiable element, while the const data() method
        //   return the address of a non-modifiable element.
        //
        // Testing:
        //   char *data();
        //   const char *data() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Internal Data Access Methods" << endl
                          << "====================================" << endl;

        if (verbose) cout <<
            "\nTesting const and non-const 'data()' methods." << endl;
        {

            Obj mX(&testAllocator);  const Obj& X = mX;  // object under test

            if (verbose) cout << "\tVerify X.data() + i same as &X[i]." <<endl;

            const int NUM_TRIALS = 10;
            for (int iLen = 0; iLen < NUM_TRIALS; ++iLen) {
                if (verbose) { cout << "\t\t"; P_(iLen); P(X); }
                for (int i = 0; i < iLen; ++i) {
                    if (veryVerbose) { cout << "\t\t\t"; P(i); }
                    LOOP2_ASSERT(iLen, i,    &X[i] == X.data() + i);
                    LOOP2_ASSERT(iLen, i, X.data() == mX.data());
                }
                const Element TEST_VALUE = VALUES[iLen % NUM_VALUES];
                mX.append(TEST_VALUE);  // Extend length of object under test.
            }

            if (verbose) cout <<
            "\tVerify const/non-const 'data()' return correct element access."
                                                                       << endl;
            Element *dummy1 = mX.data();       // data in modifiable object
            const Element *dummy2 = X.data();  // data in non-modifiable object
            ASSERT(dummy1 && dummy2);

            Element *(Obj::*f1)() = &Obj::data;          // non-const method
            const Element *(Obj::*f2)() const = &Obj::data;  // const method
            ASSERT(f1 && f2);
        }

      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING CAPACITY-RESERVING CONSTRUCTOR AND METHOD
        //   The concerns are as follows:
        //    1. capacity-reserving constructor:
        //       a. The initial value is correct (empty).
        //       b. The initial capacity is correct.
        //       c. The constructor is exception neutral w.r.t. allocation.
        //       d. The internal memory management system is hooked up properly
        //          so that *all* internally allocated memory draws from a
        //          user-supplied allocator whenever one is specified.
        //    2. 'reserveCapacityRaw' method:
        //       a. The resulting value is correct (unchanged).
        //       b. The resulting capacity is correct (not less than initial).
        //       c. The method is exception neutral w.r.t. allocation.
        //    3. 'reserveCapacity' method:
        //       a. The resulting value is correct (unchanged).
        //       b. The resulting capacity is correct (not less than initial).
        //       c. The method is exception neutral w.r.t. allocation.
        //       d. The resulting value is unchanged in the event of
        //          exceptions.
        //       e. The amount of memory allocated is current * 2^N
        //
        // Plan:
        //   In a loop, use the capacity-reserving constructor to create empty
        //   objects with increasing initial capacity.  Verify that each object
        //   has the same value as a control default object.  Then, append as
        //   many values as the requested initial capacity, and use
        //   'bdema_TestAllocator' to verify that no additional allocations
        //   have occurred.  Perform each test in the standard 'bdema'
        //   exception-testing macro block.
        //
        //   Repeat the constructor test initially specifying no allocator and
        //   again, specifying a static buffer allocator.  These tests (without
        //   specifying a 'bdema_TestAllocator') cannot confirm correct
        //   capacity-reserving behavior, but can test for rudimentary correct
        //   object behavior via the destructor and Purify, and, in
        //   'veryVerbose' mode, via the print statements.
        //
        //   To test 'reserveCapacity', specify a table of initial object
        //   values and subsequent capacities to reserve.  Construct each
        //   tabulated value, call 'reserveCapacity' with the tabulated number
        //   of elements, and confirm that the test object has the same value
        //   as a separately constructed control object.  Then, append as many
        //   values as required to bring the test object's length to the
        //   specified number of elements, and use 'bdema_TestAllocator' to
        //   verify that no additional allocations have occurred.  Perform each
        //   test in the standard 'bdema' exception-testing macro block.
        //
        // Testing:
        //   bdea_CharArray(const InitialCapacity& ne, *ba = 0);
        //   void reserveCapacityRaw(int ne);
        //   void reserveCapacity(int ne);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing Capacity Reserving Constructor and Methods" << endl
            << "==================================================" << endl;

        if (verbose) cout <<
            "\nTesting 'bdea_CharArray(capacity, ba)' Constructor" << endl;
        if (verbose) cout << "\twith a 'bdema_TestAllocator':" << endl;
        {
            const Obj W(&testAllocator);  // control value
            const int MAX_NUM_ELEMS = 9;
            for (int ne = 0; ne <= MAX_NUM_ELEMS; ++ne) {
                if (veryVerbose) { cout << "\t\t"; P(ne) }
              BEGIN_BDEMA_EXCEPTION_TEST {
                const Obj::InitialCapacity NE(ne);
                Obj mX(NE, &testAllocator);  const Obj &X = mX;
                LOOP_ASSERT(ne, W == X);
                const int NUM_BLOCKS = testAllocator.numBlocksTotal();
                const int NUM_BYTES  = testAllocator.numBytesInUse();
                if (veryVerbose) P_(X);
                for (int i = 0; i < ne; ++i) {
                    mX.append(V0);
                }
                if (veryVerbose) P(X);
                LOOP_ASSERT(ne, NUM_BLOCKS == testAllocator.numBlocksTotal());
                LOOP_ASSERT(ne, NUM_BYTES  == testAllocator.numBytesInUse());
              } END_BDEMA_EXCEPTION_TEST
            }
        }

        if (verbose) cout << "\twith no allocator (exercise only):" << endl;
        {
            const Obj W(&testAllocator);  // control value
            const int MAX_NUM_ELEMS = 9;
            for (int ne = 0; ne <= MAX_NUM_ELEMS; ++ne) {
                if (veryVerbose) { cout << "\t\t"; P(ne) }
                const Obj::InitialCapacity NE(ne);
                Obj mX(NE);  const Obj &X = mX;
                LOOP_ASSERT(ne, W == X);
                if (veryVerbose) P_(X);
                for (int i = 0; i < ne; ++i) {
                    mX.append(V0);
                }
                if (veryVerbose) P(X);
            }
        }

        if (verbose)
            cout << "\twith a buffer allocator (exercise only):" << endl;
        {
            char memory[1024];
            bdema_BufferAllocator a(memory, sizeof memory);
            const Obj W(&testAllocator);  // control value
            const int MAX_NUM_ELEMS = 9;
            for (int ne = 0; ne <= MAX_NUM_ELEMS; ++ne) {
                if (veryVerbose) { cout << "\t\t"; P(ne) }
                const Obj::InitialCapacity NE(ne);
                Obj *doNotDelete =
                    new (a.allocate(sizeof(Obj))) Obj(NE, &a);
                Obj &mX = *doNotDelete;  const Obj &X = mX;
                LOOP_ASSERT(ne, W == X);
                if (veryVerbose) P_(X);
                for (int i = 0; i < ne; ++i) {
                    mX.append(V0);
                }
                if (veryVerbose) P(X);
            }
             // No destructor is called; will produce memory leak in purify
             // if internal allocators are not hooked up properly.
        }

        // Test 'reserveCapacityRaw' and 'reserveCapacity' methods.

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_x;        // initial value
            int         d_ne;       // total number of elements to reserve
            int         d_alloc;    // number of elements allocated
        } DATA[] = {
                //line  initialValue   numElements  Allocated
                //----  ------------   -----------  --------
                { L_,   "",              0,            0       },
                { L_,   "",              1,            0       },
                { L_,   "",              2,            2       },
                { L_,   "",              3,            4       },
                { L_,   "",              4,            4       },
                { L_,   "",              5,            8       },
                { L_,   "",              15,           16      },
                { L_,   "",              16,           16      },
                { L_,   "",              17,           32      },

                { L_,   "A",             0,            0       },
                { L_,   "A",             1,            0       },
                { L_,   "A",             2,            2       },
                { L_,   "A",             3,            4       },
                { L_,   "A",             4,            4       },
                { L_,   "A",             5,            8       },
                { L_,   "A",             15,           16      },
                { L_,   "A",             16,           16      },
                { L_,   "A",             17,           32      },

                { L_,   "AB",            0,            0       },
                { L_,   "AB",            1,            0       },
                { L_,   "AB",            2,            0       },
                { L_,   "AB",            3,            4       },
                { L_,   "AB",            4,            4       },
                { L_,   "AB",            5,            8       },
                { L_,   "AB",            15,           16      },
                { L_,   "AB",            16,           16      },
                { L_,   "AB",            17,           32      },

                { L_,   "ABCDE",         0,            0       },
                { L_,   "ABCDE",         1,            0       },
                { L_,   "ABCDE",         2,            0       },
                { L_,   "ABCDE",         3,            0       },
                { L_,   "ABCDE",         4,            0       },
                { L_,   "ABCDE",         5,            0       },
                { L_,   "ABCDE",         15,           16      },
                { L_,   "ABCDE",         16,           16      },
                { L_,   "ABCDE",         17,           32      },

        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose)
            cout << "\nTesting the 'reserveCapacityRaw' method" << endl;
        {
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
              BEGIN_BDEMA_EXCEPTION_TEST {
                const int   LINE = DATA[ti].d_lineNum;
                const char *SPEC = DATA[ti].d_x;
                const int   NE   = DATA[ti].d_ne;
                if (veryVerbose) { cout << "\t\t"; P_(SPEC); P(NE); }

                const Obj W(g(SPEC), &testAllocator);
                Obj mX(W, &testAllocator);  const Obj &X = mX;
                const int BT = testAllocator.numBytesTotal();
                mX.reserveCapacityRaw(NE);
                const int exactSize = BT != testAllocator.numBytesTotal();
                LOOP_ASSERT(LINE, W == X);
                const int NUM_BLOCKS = testAllocator.numBlocksTotal();
                const int NUM_BYTES  = testAllocator.numBytesInUse();
                if (veryVerbose) P_(X);
                for (int i = X.length(); i < NE; ++i) {
                    mX.append(V0);
                }
                if (veryVerbose) P(X);
                LOOP_ASSERT(LINE, NUM_BLOCKS== testAllocator.numBlocksTotal());
                LOOP_ASSERT(LINE, NUM_BYTES == testAllocator.numBytesInUse());
                if (exactSize) {
                    mX.append(V0);
                    if (veryVerbose) P(X);
                    LOOP_ASSERT(LINE,
                                NUM_BLOCKS < testAllocator.numBlocksTotal());
                    LOOP_ASSERT(LINE,
                                NUM_BYTES  < testAllocator.numBytesInUse());
                }
              } END_BDEMA_EXCEPTION_TEST
            }
        }

        if (verbose)
           cout << "\nTesting the 'reserveCapacity' method" << endl;
        {
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
              BEGIN_BDEMA_EXCEPTION_TEST {
                const int   LINE    = DATA[ti].d_lineNum;
                const char *SPEC    = DATA[ti].d_x;
                const int   NE      = DATA[ti].d_ne;
                const int   ALLOC   = DATA[ti].d_alloc;
                if (veryVerbose) {
                        cout << "\t\t"; P_(SPEC); P_(NE); P(ALLOC);
                }

                const Obj W(g(SPEC), &testAllocator);
                Obj mX(W, &testAllocator); const Obj &X = mX;
                const int BT = testAllocator.numBytesTotal();
                {
                   // Verify that X doesn't change after exceptions in
                   // reserveCapacity by comparing it to the control W.

                   EqualityTester chX(&X, &W);

                   mX.reserveCapacity(NE);
                }
                LOOP_ASSERT(LINE, W == X);

                // Memory allocated should be as specified.
                ASSERT(ALLOC
                        == (testAllocator.numBytesTotal() - BT)
                        / (int)sizeof(Element))

                const int NUM_BLOCKS = testAllocator.numBlocksTotal();
                const int NUM_BYTES  = testAllocator.numBytesInUse();

                if (veryVerbose) P_(X);
                for (int i = X.length(); i < NE; ++i) {
                    mX.append(V0);
                }
                if (veryVerbose) P(X);
                LOOP_ASSERT(LINE, NUM_BLOCKS== testAllocator.numBlocksTotal());
                LOOP_ASSERT(LINE, NUM_BYTES == testAllocator.numBytesInUse());
              } END_BDEMA_EXCEPTION_TEST
            }
         }

      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING SWAP METHOD:
        //   We are concerned that, for an object of any length, 'swap' must
        //   exchange the values at any valid pair of index positions while
        //   leaving all other elements unaffected.  Note that, upon inspecting
        //   the implementation, we are explicitly not concerned about specific
        //   element values (i.e., a single pair of test values is sufficient,
        //   and cannot mask a "stuck at" error).
        //
        // Plan:
        //   For each object X in a set of objects ordered by increasing length
        //   L containing uniform values, V0.
        //     For each valid index position P1 in [0 .. L-1]:
        //       For each valid index position P2 in [0 .. L-1]:
        //         1.  Create a test object T from X using the copy ctor.
        //         2.  Replace the element at P1 with V1 and at P2 with V2.
        //         3.  Swap these elements in T.
        //         4.  Verify that
        //               (i)     V2 == T[P1]     always
        //         5.  Verify that
        //               (ii)    V1 == T[P2]     if (P1 != P2)
        //                       V2 == T[P2]     if (P1 == P2)
        //         6.  For each index position, i, in [0 .. L-1] verify that:
        //               (iii)   V0 == T[i]      if (P1 != i && P2 != i)
        //
        // Testing:
        //   void swap(int index1, int index2);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'swap' Method" << endl
                          << "=====================" << endl;

        if (verbose) cout << "\nTesting swap(index1, index2)" << endl;

        const int NUM_TRIALS = 10;

        Obj mX(&testAllocator);  const Obj& X = mX;  // control

        for (int iLen = 0; iLen < NUM_TRIALS; ++iLen) { // iLen: initial length
            if (verbose) { cout << "\t"; P_(iLen); P(X); }
            for (int pos1 = 0; pos1 < iLen; ++pos1) { // for each position
                for (int pos2 = 0; pos2 < iLen; ++pos2) { // for each position
                    Obj mT(X, &testAllocator);
                    const Obj& T = mT;                  // object under test
                    LOOP3_ASSERT(iLen, pos1, pos2, X == T);

                    mT[pos1] = V3;  // element values distinct from initial
                    mT[pos2] = V4;  // element values distinct from initial
                    if (veryVerbose) { cout << "\t\t";  P_(pos1);
                                       P_(pos2);        P(T); }
                    mT.swap(pos1, pos2);
                    if (veryVerbose) { cout << "\t\t";  P_(pos1);
                                       P_(pos2);        P(T); }

                    LOOP3_ASSERT(iLen, pos1, pos2, V4 == T[pos1]);

                    const Element& VX = pos1 == pos2 ? V4 : V3;
                    LOOP3_ASSERT(iLen, pos1, pos2, VX == T[pos2]);

                    for (int i = 0; i < iLen; ++i) {
                        if (i == pos1 || i == pos2) continue;
                        LOOP4_ASSERT(iLen, pos1, pos2, i,
                                     VALUES[i % 3] == T[i]);
                    }
                }
            }
            if (veryVerbose) cout <<
                "\t--------------------------------------------------" << endl;
            mX.append(VALUES[iLen % 3]);  // Extend control with cyclic values.
        }

      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING PRINT METHOD
        //   The print method formats the value of the object directly from
        //   the underlying state information according to supplied arguments.
        //   Ensure that the method formats properly for:
        //     - empty and non-empty values
        //     - negative, 0, and positive levels.
        //     - 0 and non-zero spaces per level.
        // Plan:
        //   For each of an enumerated set of object, 'level', and
        //   'spacesPerLevel' values, ordered by increasing object length, use
        //   'ostrstream' to 'print' that object's value, using the tabulated
        //   parameters, to two separate character buffers each with different
        //   initial values.  Compare the contents of these buffers with the
        //   literal expected output format and verify that the characters
        //   beyond the null characters are unaffected in both buffers.
        //
        // Testing:
        //   ostream& print(ostream& stream, int level, int spacesPerLevel);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'print' method" << endl
                          << "======================" << endl;

        if (verbose) cout << "\nTesting 'print' (ostream)." << endl;
#define NL "\n"
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_indent;   // indentation level
                int         d_spaces;   // spaces per indentation level
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                //line  spec    indent +/-  spaces/Tab  format        // ADJUST
                //----  ----    ----------  ----------  ------------------
                { L_,   "",        0,         0,        "["           NL
                                                        "]"           NL  },

                { L_,   "",        0,         2,        "["           NL
                                                        "]"           NL  },

                { L_,   "",        1,         1,        " ["          NL
                                                        " ]"          NL  },

                { L_,   "",        1,         2,        "  ["         NL
                                                        "  ]"         NL  },

                { L_,   "",       -1,         2,        "["           NL
                                                        "  ]"         NL  },

                { L_,   "A",       0,         0,        "["           NL
                                                        "a"           NL
                                                        "]"           NL  },

                { L_,   "A",      -2,         1,        "["           NL
                                                        "   a"        NL
                                                        "  ]"         NL  },

                { L_,   "BC",      1,         2,        "  ["         NL
                                                        "    b"       NL
                                                        "    c"       NL
                                                        "  ]"         NL  },

                { L_,   "BC",      2,         1,        "  ["         NL
                                                        "   b"        NL
                                                        "   c"        NL
                                                        "  ]"         NL  },

                { L_,   "ABCDE",   1,         3,        "   ["        NL
                                                        "      a"     NL
                                                        "      b"     NL
                                                        "      c"     NL
                                                        "      d"     NL
                                                        "      e"     NL
                                                        "   ]"        NL  },
            };
#undef NL

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int SIZE = 1000; // Must be big enough to hold output string.
            const char Z1 = (char) 0xFF;  // Used to represent an unset char.
            const char Z2 = 0x00;  // Value 2 used to represent an unset char.

            char mCtrlBuf1[SIZE];  memset(mCtrlBuf1, Z1, SIZE);
            char mCtrlBuf2[SIZE];  memset(mCtrlBuf2, Z2, SIZE);
            const char *CTRL_BUF1 = mCtrlBuf1;
            const char *CTRL_BUF2 = mCtrlBuf2;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int         LINE   = DATA[ti].d_lineNum;
                const char *const SPEC   = DATA[ti].d_spec_p;
                const int         IND    = DATA[ti].d_indent;
                const int         SPL    = DATA[ti].d_spaces;
                const char *const FMT    = DATA[ti].d_fmt_p;
                const int         curLen = strlen(SPEC);

                char buf1[SIZE + epcBug], buf2[SIZE + epcBug];
                memcpy(buf1, CTRL_BUF1, SIZE); // Preset buf1 to Z1 values.
                memcpy(buf2, CTRL_BUF2, SIZE); // Preset buf2 to Z2 values.

                Obj mX(&testAllocator);  const Obj& X = gg(&mX, SPEC);
                LOOP_ASSERT(ti, curLen == X.length()); // same lengths

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton objects of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen < curLen); // strictly increasing
                    oldLen = curLen;
                }

                if (verbose) { cout << "\t\tSpec = \"" << SPEC << "\", ";
                               P_(IND); P(SPL); }
                if (veryVerbose) cout << "EXPECTED FORMAT:" << endl<<FMT<<endl;
                ostrstream out1(buf1, SIZE);  X.print(out1, IND, SPL) << ends;
                ostrstream out2(buf2, SIZE);  X.print(out2, IND, SPL) << ends;
                if (veryVerbose) cout << "ACTUAL FORMAT:" << endl<<buf1<<endl;

                const int SZ = strlen(FMT) + 1;
                const int REST = SIZE - SZ;
                LOOP_ASSERT(ti, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(ti, Z1 == buf1[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(ti, Z2 == buf2[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(ti, 0 == memcmp(buf1, FMT, SZ));
                LOOP_ASSERT(ti, 0 == memcmp(buf2, FMT, SZ));
                LOOP_ASSERT(ti, 0 == memcmp(buf1 + SZ, CTRL_BUF1 + SZ, REST));
                LOOP_ASSERT(ti, 0 == memcmp(buf2 + SZ, CTRL_BUF2 + SZ, REST));
            }
        }

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING REPLACE AND 'operator[]'
        //   Concerns:
        //     For the 'replace' method, the following properties must hold:
        //       1. The source is left unaffected (apart from aliasing).
        //       2. The subsequent existence of the source has no effect on the
        //          result object (apart from aliasing).
        //       3. The function is alias safe.
        //       4. The function preserves object invariants.
        //     Note that the 'replace' methods cannot allocate, but is tested
        //     for exceptions anyway.                                 // ADJUST
        //
        //     For 'operator[]', the method must be able to modify its indexed
        //     element when used as an lvalue, but must not modify its indexed
        //     element when used as an rvalue.
        //
        // Plan:
        //   Use the enumeration technique to a depth of 5 for both the normal
        //   and alias cases.  Data is tabulated explicitly for the 'replace'
        //   method that takes a range from a source array (or itself, for the
        //   aliasing test); the "scalar" 'replace' test selects a subset of
        //   the table by testing 'if (1 == NE)' where 'NE' is the tabulated
        //   number of elements to replace.  'operator[]' is also tested using
        //   the scalar 'replace' data, but using explicit assignment to
        //   achieve the "expected" result.
        //     - In the "canonical state" (black-box) tests, we confirm that
        //       the source is unmodified by the method call, and that its
        //       subsequent destruction has no effect on the destination
        //       object.
        //     - In all cases we want to make sure that after the application
        //       of the operation, the object is allowed to go out of scope
        //       directly to enable the destructor to assert object invariants.
        //     - Each object constructed should be wrapped in separate
        //       BDEMA test assert macros and use gg as an optimization.
        // Testing:
        //   void replace(int di, char item);
        //   void replace(int di, const bdea_Array<char>& sa, int si, int ne);
        //   char& operator[](int index);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                << "Testing 'replace' and 'operator[]'" << endl
                << "==================================" << endl;

        if (verbose) cout <<
            "\nTesting replace(di, sa, si, ne) et. al. (no aliasing)" << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_daSpec;   // initial (destination) array
                int         d_di;       // index at which to replace into da
                const char *d_saSpec;   // source array
                int         d_si;       // index at which to replace from sa
                int         d_ne;       // number of elements to replace
                const char *d_expSpec;  // expected array value
            } DATA[] = {
                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 0
                { L_,   "",      0,  "",      0,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 1
                { L_,   "A",     0,  "",      0,  0, "A"     },
                { L_,   "A",     1,  "",      0,  0, "A"     },

                { L_,   "",      0,  "B",     0,  0, ""      },
                { L_,   "",      0,  "B",     1,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 2
                { L_,   "AB",    0,  "",      0,  0, "AB"    },
                { L_,   "AB",    1,  "",      0,  0, "AB"    },
                { L_,   "AB",    2,  "",      0,  0, "AB"    },

                { L_,   "A",     0,  "B",     0,  0, "A"     },
                { L_,   "A",     0,  "B",     0,  1, "B"     },
                { L_,   "A",     0,  "B",     1,  0, "A"     },
                { L_,   "A",     1,  "B",     0,  0, "A"     },
                { L_,   "A",     1,  "B",     1,  0, "A"     },

                { L_,   "",      0,  "AB",    0,  0, ""      },
                { L_,   "",      0,  "AB",    1,  0, ""      },
                { L_,   "",      0,  "AB",    2,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 3
                { L_,   "ABC",   0,  "",      0,  0, "ABC"   },
                { L_,   "ABC",   1,  "",      0,  0, "ABC"   },
                { L_,   "ABC",   2,  "",      0,  0, "ABC"   },
                { L_,   "ABC",   3,  "",      0,  0, "ABC"   },

                { L_,   "AB",    0,  "C",     0,  0, "AB"    },
                { L_,   "AB",    0,  "C",     0,  1, "CB"    },
                { L_,   "AB",    0,  "C",     1,  0, "AB"    },
                { L_,   "AB",    1,  "C",     0,  0, "AB"    },
                { L_,   "AB",    1,  "C",     0,  1, "AC"    },
                { L_,   "AB",    1,  "C",     1,  0, "AB"    },
                { L_,   "AB",    2,  "C",     0,  0, "AB"    },
                { L_,   "AB",    2,  "C",     1,  0, "AB"    },

                { L_,   "A",     0,  "BC",    0,  0, "A"     },
                { L_,   "A",     0,  "BC",    0,  1, "B"     },
                { L_,   "A",     0,  "BC",    1,  0, "A"     },
                { L_,   "A",     0,  "BC",    1,  1, "C"     },
                { L_,   "A",     0,  "BC",    2,  0, "A"     },
                { L_,   "A",     1,  "BC",    0,  0, "A"     },
                { L_,   "A",     1,  "BC",    1,  0, "A"     },
                { L_,   "A",     1,  "BC",    2,  0, "A"     },

                { L_,   "",      0,  "ABC",   0,  0, ""      },
                { L_,   "",      0,  "ABC",   1,  0, ""      },
                { L_,   "",      0,  "ABC",   2,  0, ""      },
                { L_,   "",      0,  "ABC",   3,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 4
                { L_,   "ABCD",  0,  "",      0,  0, "ABCD"  },
                { L_,   "ABCD",  1,  "",      0,  0, "ABCD"  },
                { L_,   "ABCD",  2,  "",      0,  0, "ABCD"  },
                { L_,   "ABCD",  3,  "",      0,  0, "ABCD"  },
                { L_,   "ABCD",  4,  "",      0,  0, "ABCD"  },

                { L_,   "ABC",   0,  "D",     0,  0, "ABC"   },
                { L_,   "ABC",   0,  "D",     0,  1, "DBC"   },
                { L_,   "ABC",   0,  "D",     1,  0, "ABC"   },
                { L_,   "ABC",   1,  "D",     0,  0, "ABC"   },
                { L_,   "ABC",   1,  "D",     0,  1, "ADC"   },
                { L_,   "ABC",   1,  "D",     1,  0, "ABC"   },
                { L_,   "ABC",   2,  "D",     0,  0, "ABC"   },
                { L_,   "ABC",   2,  "D",     0,  1, "ABD"   },
                { L_,   "ABC",   2,  "D",     1,  0, "ABC"   },
                { L_,   "ABC",   3,  "D",     0,  0, "ABC"   },
                { L_,   "ABC",   3,  "D",     1,  0, "ABC"   },

                { L_,   "AB",    0,  "CD",    0,  0, "AB"    },
                { L_,   "AB",    0,  "CD",    0,  1, "CB"    },
                { L_,   "AB",    0,  "CD",    0,  2, "CD"    },
                { L_,   "AB",    0,  "CD",    1,  0, "AB"    },
                { L_,   "AB",    0,  "CD",    1,  1, "DB"    },
                { L_,   "AB",    0,  "CD",    2,  0, "AB"    },
                { L_,   "AB",    1,  "CD",    0,  0, "AB"    },
                { L_,   "AB",    1,  "CD",    0,  1, "AC"    },
                { L_,   "AB",    1,  "CD",    1,  0, "AB"    },
                { L_,   "AB",    1,  "CD",    1,  1, "AD"    },
                { L_,   "AB",    1,  "CD",    2,  0, "AB"    },
                { L_,   "AB",    2,  "CD",    0,  0, "AB"    },
                { L_,   "AB",    2,  "CD",    1,  0, "AB"    },
                { L_,   "AB",    2,  "CD",    2,  0, "AB"    },

                { L_,   "A",     0,  "BCD",   0,  0, "A"     },
                { L_,   "A",     0,  "BCD",   0,  1, "B"     },
                { L_,   "A",     0,  "BCD",   1,  0, "A"     },
                { L_,   "A",     0,  "BCD",   1,  1, "C"     },
                { L_,   "A",     0,  "BCD",   2,  0, "A"     },
                { L_,   "A",     0,  "BCD",   2,  1, "D"     },
                { L_,   "A",     0,  "BCD",   3,  0, "A"     },
                { L_,   "A",     1,  "BCD",   0,  0, "A"     },
                { L_,   "A",     1,  "BCD",   1,  0, "A"     },
                { L_,   "A",     1,  "BCD",   2,  0, "A"     },
                { L_,   "A",     1,  "BCD",   3,  0, "A"     },

                { L_,   "",      0,  "ABCD",  0,  0, ""      },
                { L_,   "",      0,  "ABCD",  1,  0, ""      },
                { L_,   "",      0,  "ABCD",  2,  0, ""      },
                { L_,   "",      0,  "ABCD",  3,  0, ""      },
                { L_,   "",      0,  "ABCD",  4,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 5
                { L_,   "ABCDE", 0,  "",      0,  0, "ABCDE" },
                { L_,   "ABCDE", 1,  "",      0,  0, "ABCDE" },
                { L_,   "ABCDE", 2,  "",      0,  0, "ABCDE" },
                { L_,   "ABCDE", 3,  "",      0,  0, "ABCDE" },
                { L_,   "ABCDE", 4,  "",      0,  0, "ABCDE" },
                { L_,   "ABCDE", 5,  "",      0,  0, "ABCDE" },

                { L_,   "ABCD",  0,  "E",     0,  0, "ABCD" },
                { L_,   "ABCD",  0,  "E",     0,  1, "EBCD" },
                { L_,   "ABCD",  0,  "E",     1,  0, "ABCD" },
                { L_,   "ABCD",  1,  "E",     0,  0, "ABCD" },
                { L_,   "ABCD",  1,  "E",     0,  1, "AECD" },
                { L_,   "ABCD",  1,  "E",     1,  0, "ABCD" },
                { L_,   "ABCD",  2,  "E",     0,  0, "ABCD" },
                { L_,   "ABCD",  2,  "E",     0,  1, "ABED" },
                { L_,   "ABCD",  2,  "E",     1,  0, "ABCD" },
                { L_,   "ABCD",  3,  "E",     0,  0, "ABCD" },
                { L_,   "ABCD",  3,  "E",     0,  1, "ABCE" },
                { L_,   "ABCD",  3,  "E",     1,  0, "ABCD" },
                { L_,   "ABCD",  4,  "E",     0,  0, "ABCD" },
                { L_,   "ABCD",  4,  "E",     1,  0, "ABCD" },

                { L_,   "ABC",   0,  "DE",    0,  0, "ABC"   },
                { L_,   "ABC",   0,  "DE",    0,  1, "DBC"   },
                { L_,   "ABC",   0,  "DE",    0,  2, "DEC"   },
                { L_,   "ABC",   0,  "DE",    1,  0, "ABC"   },
                { L_,   "ABC",   0,  "DE",    1,  1, "EBC"   },
                { L_,   "ABC",   0,  "DE",    2,  0, "ABC"   },
                { L_,   "ABC",   1,  "DE",    0,  0, "ABC"   },
                { L_,   "ABC",   1,  "DE",    0,  1, "ADC"   },
                { L_,   "ABC",   1,  "DE",    0,  2, "ADE"   },
                { L_,   "ABC",   1,  "DE",    1,  0, "ABC"   },
                { L_,   "ABC",   1,  "DE",    1,  1, "AEC"   },
                { L_,   "ABC",   1,  "DE",    2,  0, "ABC"   },
                { L_,   "ABC",   2,  "DE",    0,  0, "ABC"   },
                { L_,   "ABC",   2,  "DE",    0,  1, "ABD"   },
                { L_,   "ABC",   2,  "DE",    1,  0, "ABC"   },
                { L_,   "ABC",   2,  "DE",    1,  1, "ABE"   },
                { L_,   "ABC",   2,  "DE",    2,  0, "ABC"   },
                { L_,   "ABC",   3,  "DE",    0,  0, "ABC"   },
                { L_,   "ABC",   3,  "DE",    1,  0, "ABC"   },
                { L_,   "ABC",   3,  "DE",    2,  0, "ABC"   },

                { L_,   "AB",    0,  "CDE",   0,  0, "AB"    },
                { L_,   "AB",    0,  "CDE",   0,  1, "CB"    },
                { L_,   "AB",    0,  "CDE",   0,  2, "CD"    },
                { L_,   "AB",    0,  "CDE",   1,  0, "AB"    },
                { L_,   "AB",    0,  "CDE",   1,  1, "DB"    },
                { L_,   "AB",    0,  "CDE",   2,  0, "AB"    },
                { L_,   "AB",    0,  "CDE",   2,  1, "EB"    },
                { L_,   "AB",    0,  "CDE",   3,  0, "AB"    },
                { L_,   "AB",    1,  "CDE",   0,  0, "AB"    },
                { L_,   "AB",    1,  "CDE",   0,  1, "AC"    },
                { L_,   "AB",    1,  "CDE",   1,  0, "AB"    },
                { L_,   "AB",    1,  "CDE",   1,  1, "AD"    },
                { L_,   "AB",    1,  "CDE",   2,  0, "AB"    },
                { L_,   "AB",    1,  "CDE",   2,  1, "AE"    },
                { L_,   "AB",    1,  "CDE",   3,  0, "AB"    },
                { L_,   "AB",    2,  "CDE",   0,  0, "AB"    },
                { L_,   "AB",    2,  "CDE",   1,  0, "AB"    },
                { L_,   "AB",    2,  "CDE",   2,  0, "AB"    },
                { L_,   "AB",    2,  "CDE",   3,  0, "AB"    },

                { L_,   "A",     0,  "BCDE",  0,  0, "A"     },
                { L_,   "A",     0,  "BCDE",  0,  1, "B"     },
                { L_,   "A",     0,  "BCDE",  1,  0, "A"     },
                { L_,   "A",     0,  "BCDE",  1,  1, "C"     },
                { L_,   "A",     0,  "BCDE",  2,  0, "A"     },
                { L_,   "A",     0,  "BCDE",  2,  1, "D"     },
                { L_,   "A",     0,  "BCDE",  3,  0, "A"     },
                { L_,   "A",     0,  "BCDE",  3,  1, "E"     },
                { L_,   "A",     0,  "BCDE",  4,  0, "A"     },
                { L_,   "A",     1,  "BCDE",  0,  0, "A"     },
                { L_,   "A",     1,  "BCDE",  1,  0, "A"     },
                { L_,   "A",     1,  "BCDE",  2,  0, "A"     },
                { L_,   "A",     1,  "BCDE",  3,  0, "A"     },
                { L_,   "A",     1,  "BCDE",  4,  0, "A"     },

                { L_,   "",      0,  "ABCDE", 0,  0, ""      },
                { L_,   "",      0,  "ABCDE", 1,  0, ""      },
                { L_,   "",      0,  "ABCDE", 2,  0, ""      },
                { L_,   "",      0,  "ABCDE", 3,  0, ""      },
                { L_,   "",      0,  "ABCDE", 4,  0, ""      },
                { L_,   "",      0,  "ABCDE", 5,  0, ""      },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *D_SPEC = DATA[ti].d_daSpec;
                const int   DI     = DATA[ti].d_di;
                const char *S_SPEC = DATA[ti].d_saSpec;
                const int   SI     = DATA[ti].d_si;
                const int   NE     = DATA[ti].d_ne;
                const char *E_SPEC = DATA[ti].d_expSpec;

                const int   DEPTH  = strlen(D_SPEC) + strlen(S_SPEC);
                if (DEPTH > oldDepth) {
                    oldDepth = DEPTH;
                    if (verbose) { cout << '\t';  P(DEPTH); }
                }

                Obj DD(g(D_SPEC));              // control for destination
                Obj SS(g(S_SPEC));              // control for source
                Obj EE(g(E_SPEC));              // control for expected value

                if (veryVerbose) {
                    cout << "\t  =================================="
                                "==================================" << endl;
                    cout << "\t  "; P_(D_SPEC); P_(DI); P_(S_SPEC);
                                                P_(SI); P_(NE); P(E_SPEC);
                    cout << "\t\t"; P(DD);
                    cout << "\t\t"; P(SS);
                    cout << "\t\t"; P(EE);

                    cout << "\t\t\t---------- BLACK BOX ----------" << endl;
                }

                if (veryVerbose) cout << "\t\treplace(di, sa, si, ne)" << endl;
                {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    {
                        Obj s(SS, &testAllocator);  const Obj &S = s;
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                        x.replace(DI, s, SI, NE);       // source non-'const'
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                  } END_BDEMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\treplace(di, item)" << endl;
                if (1 == NE) {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    {
                        Obj s(SS, &testAllocator);  const Obj &S = s;
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                        x.replace(DI, s[SI]);           // source non-'const'
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                  } END_BDEMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\toperator[](index)" << endl;
                if (1 == NE) {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    {
                        Obj s(SS, &testAllocator);  const Obj &S = s;
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                        x[DI] = s[SI];                  // source non-'const'
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                  } END_BDEMA_EXCEPTION_TEST
                }


                if (veryVerbose) {
                    cout << "\t\t\t---------- WHITE BOX ----------" << endl;
                }

                Obj x(&testAllocator);  const Obj &X = x;

                const int STRETCH_SIZE = 50;

                stretchRemoveAll(&x, STRETCH_SIZE);

                const int NUM_BLOCKS = testAllocator.numBlocksTotal();
                const int NUM_BYTES  = testAllocator.numBytesInUse();

                if (veryVerbose) cout << "\t\treplace(di, sa, si, ne)" << endl;
                {
                    x.removeAll();  gg(&x, D_SPEC);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.replace(DI, SS, SI, NE);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                if (veryVerbose) cout << "\t\treplace(di, item)" << endl;
                if (1 == NE) {
                    x.removeAll();  gg(&x, D_SPEC);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.replace(DI, SS[SI]);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                if (veryVerbose) cout << "\t\toperator[](int index)" << endl;
                if (1 == NE) {
                    x.removeAll();  gg(&x, D_SPEC);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x[DI] = SS[SI];
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                LOOP_ASSERT(LINE, NUM_BLOCKS ==testAllocator.numBlocksTotal());
                LOOP_ASSERT(LINE, NUM_BYTES == testAllocator.numBytesInUse());
            }
        }

        //---------------------------------------------------------------------

        if (verbose) cout <<
            "\nTesting x.replace(di, sa, si, ne) et. al. (aliasing)" << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_xSpec;    // initial array (= srcArray)
                int         d_di;       // index at which to insert into x
                int         d_si;       // index at which to insert from x
                int         d_ne;       // number of elements to insert
                const char *d_expSpec;  // expected array value
            } DATA[] = {
                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 0
                { L_,   "",      0,  0,  0,  ""      },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 1
                { L_,   "A",     0,  0,  0,  "A"     },
                { L_,   "A",     0,  0,  1,  "A"     },
                { L_,   "A",     0,  1,  0,  "A"     },

                { L_,   "A",     1,  0,  0,  "A"     },
                { L_,   "A",     1,  1,  0,  "A"     },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 2
                { L_,   "AB",    0,  0,  0,  "AB"    },
                { L_,   "AB",    0,  0,  1,  "AB"    },
                { L_,   "AB",    0,  0,  2,  "AB"    },
                { L_,   "AB",    0,  1,  0,  "AB"    },
                { L_,   "AB",    0,  1,  1,  "BB"    },
                { L_,   "AB",    0,  2,  0,  "AB"    },

                { L_,   "AB",    1,  0,  0,  "AB"    },
                { L_,   "AB",    1,  0,  1,  "AA"    },
                { L_,   "AB",    1,  1,  0,  "AB"    },
                { L_,   "AB",    1,  1,  1,  "AB"    },
                { L_,   "AB",    1,  2,  0,  "AB"    },

                { L_,   "AB",    2,  0,  0,  "AB"    },
                { L_,   "AB",    2,  1,  0,  "AB"    },
                { L_,   "AB",    2,  2,  0,  "AB"    },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 3
                { L_,   "ABC",   0,  0,  0,  "ABC"   },
                { L_,   "ABC",   0,  0,  1,  "ABC"   },
                { L_,   "ABC",   0,  0,  2,  "ABC"   },
                { L_,   "ABC",   0,  0,  3,  "ABC"   },
                { L_,   "ABC",   0,  1,  0,  "ABC"   },
                { L_,   "ABC",   0,  1,  1,  "BBC"   },
                { L_,   "ABC",   0,  1,  2,  "BCC"   },
                { L_,   "ABC",   0,  2,  0,  "ABC"   },
                { L_,   "ABC",   0,  2,  1,  "CBC"   },
                { L_,   "ABC",   0,  3,  0,  "ABC"   },

                { L_,   "ABC",   1,  0,  0,  "ABC"   },
                { L_,   "ABC",   1,  0,  1,  "AAC"   },
                { L_,   "ABC",   1,  0,  2,  "AAB"   },
                { L_,   "ABC",   1,  1,  0,  "ABC"   },
                { L_,   "ABC",   1,  1,  1,  "ABC"   },
                { L_,   "ABC",   1,  1,  2,  "ABC"   },
                { L_,   "ABC",   1,  2,  0,  "ABC"   },
                { L_,   "ABC",   1,  2,  1,  "ACC"   },
                { L_,   "ABC",   1,  3,  0,  "ABC"   },

                { L_,   "ABC",   2,  0,  0,  "ABC"   },
                { L_,   "ABC",   2,  0,  1,  "ABA"   },
                { L_,   "ABC",   2,  1,  0,  "ABC"   },
                { L_,   "ABC",   2,  1,  1,  "ABB"   },
                { L_,   "ABC",   2,  2,  0,  "ABC"   },
                { L_,   "ABC",   2,  2,  1,  "ABC"   },
                { L_,   "ABC",   2,  3,  0,  "ABC"   },

                { L_,   "ABC",   3,  0,  0,  "ABC"   },
                { L_,   "ABC",   3,  1,  0,  "ABC"   },
                { L_,   "ABC",   3,  2,  0,  "ABC"   },
                { L_,   "ABC",   3,  3,  0,  "ABC"   },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 4
                { L_,   "ABCD",  0,  0,  0,  "ABCD"  },
                { L_,   "ABCD",  0,  0,  1,  "ABCD"  },
                { L_,   "ABCD",  0,  0,  2,  "ABCD"  },
                { L_,   "ABCD",  0,  0,  3,  "ABCD"  },
                { L_,   "ABCD",  0,  0,  4,  "ABCD"  },
                { L_,   "ABCD",  0,  1,  0,  "ABCD"  },
                { L_,   "ABCD",  0,  1,  1,  "BBCD"  },
                { L_,   "ABCD",  0,  1,  2,  "BCCD"  },
                { L_,   "ABCD",  0,  1,  3,  "BCDD"  },
                { L_,   "ABCD",  0,  2,  0,  "ABCD"  },
                { L_,   "ABCD",  0,  2,  1,  "CBCD"  },
                { L_,   "ABCD",  0,  2,  2,  "CDCD"  },
                { L_,   "ABCD",  0,  3,  0,  "ABCD"  },
                { L_,   "ABCD",  0,  3,  1,  "DBCD"  },
                { L_,   "ABCD",  0,  4,  0,  "ABCD"  },

                { L_,   "ABCD",  1,  0,  0,  "ABCD"  },
                { L_,   "ABCD",  1,  0,  1,  "AACD"  },
                { L_,   "ABCD",  1,  0,  2,  "AABD"  },
                { L_,   "ABCD",  1,  0,  3,  "AABC"  },
                { L_,   "ABCD",  1,  1,  0,  "ABCD"  },
                { L_,   "ABCD",  1,  1,  1,  "ABCD"  },
                { L_,   "ABCD",  1,  1,  2,  "ABCD"  },
                { L_,   "ABCD",  1,  1,  3,  "ABCD"  },
                { L_,   "ABCD",  1,  2,  0,  "ABCD"  },
                { L_,   "ABCD",  1,  2,  1,  "ACCD"  },
                { L_,   "ABCD",  1,  2,  2,  "ACDD"  },
                { L_,   "ABCD",  1,  3,  0,  "ABCD"  },
                { L_,   "ABCD",  1,  3,  1,  "ADCD"  },
                { L_,   "ABCD",  1,  4,  0,  "ABCD"  },

                { L_,   "ABCD",  2,  0,  0,  "ABCD"  },
                { L_,   "ABCD",  2,  0,  1,  "ABAD"  },
                { L_,   "ABCD",  2,  0,  2,  "ABAB"  },
                { L_,   "ABCD",  2,  1,  0,  "ABCD"  },
                { L_,   "ABCD",  2,  1,  1,  "ABBD"  },
                { L_,   "ABCD",  2,  1,  2,  "ABBC"  },
                { L_,   "ABCD",  2,  2,  0,  "ABCD"  },
                { L_,   "ABCD",  2,  2,  1,  "ABCD"  },
                { L_,   "ABCD",  2,  2,  2,  "ABCD"  },
                { L_,   "ABCD",  2,  3,  0,  "ABCD"  },
                { L_,   "ABCD",  2,  3,  1,  "ABDD"  },
                { L_,   "ABCD",  2,  4,  0,  "ABCD"  },

                { L_,   "ABCD",  3,  0,  0,  "ABCD"  },
                { L_,   "ABCD",  3,  0,  1,  "ABCA"  },
                { L_,   "ABCD",  3,  1,  0,  "ABCD"  },
                { L_,   "ABCD",  3,  1,  1,  "ABCB"  },
                { L_,   "ABCD",  3,  2,  0,  "ABCD"  },
                { L_,   "ABCD",  3,  2,  1,  "ABCC"  },
                { L_,   "ABCD",  3,  3,  0,  "ABCD"  },
                { L_,   "ABCD",  3,  3,  1,  "ABCD"  },
                { L_,   "ABCD",  3,  4,  0,  "ABCD"  },

                { L_,   "ABCD",  4,  0,  0,  "ABCD"  },
                { L_,   "ABCD",  4,  1,  0,  "ABCD"  },
                { L_,   "ABCD",  4,  2,  0,  "ABCD"  },
                { L_,   "ABCD",  4,  3,  0,  "ABCD"  },
                { L_,   "ABCD",  4,  4,  0,  "ABCD"  },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 5
                { L_,   "ABCDE", 0,  0,  0,  "ABCDE" },
                { L_,   "ABCDE", 0,  0,  1,  "ABCDE" },
                { L_,   "ABCDE", 0,  0,  2,  "ABCDE" },
                { L_,   "ABCDE", 0,  0,  3,  "ABCDE" },
                { L_,   "ABCDE", 0,  0,  4,  "ABCDE" },
                { L_,   "ABCDE", 0,  0,  5,  "ABCDE" },
                { L_,   "ABCDE", 0,  1,  0,  "ABCDE" },
                { L_,   "ABCDE", 0,  1,  1,  "BBCDE" },
                { L_,   "ABCDE", 0,  1,  2,  "BCCDE" },
                { L_,   "ABCDE", 0,  1,  3,  "BCDDE" },
                { L_,   "ABCDE", 0,  1,  4,  "BCDEE" },
                { L_,   "ABCDE", 0,  2,  0,  "ABCDE" },
                { L_,   "ABCDE", 0,  2,  1,  "CBCDE" },
                { L_,   "ABCDE", 0,  2,  2,  "CDCDE" },
                { L_,   "ABCDE", 0,  2,  3,  "CDEDE" },
                { L_,   "ABCDE", 0,  3,  0,  "ABCDE" },
                { L_,   "ABCDE", 0,  3,  1,  "DBCDE" },
                { L_,   "ABCDE", 0,  3,  2,  "DECDE" },
                { L_,   "ABCDE", 0,  4,  0,  "ABCDE" },
                { L_,   "ABCDE", 0,  4,  1,  "EBCDE" },
                { L_,   "ABCDE", 0,  5,  0,  "ABCDE" },

                { L_,   "ABCDE", 1,  0,  0,  "ABCDE" },
                { L_,   "ABCDE", 1,  0,  1,  "AACDE" },
                { L_,   "ABCDE", 1,  0,  2,  "AABDE" },
                { L_,   "ABCDE", 1,  0,  3,  "AABCE" },
                { L_,   "ABCDE", 1,  0,  4,  "AABCD" },
                { L_,   "ABCDE", 1,  1,  0,  "ABCDE" },
                { L_,   "ABCDE", 1,  1,  1,  "ABCDE" },
                { L_,   "ABCDE", 1,  1,  2,  "ABCDE" },
                { L_,   "ABCDE", 1,  1,  3,  "ABCDE" },
                { L_,   "ABCDE", 1,  1,  4,  "ABCDE" },
                { L_,   "ABCDE", 1,  2,  0,  "ABCDE" },
                { L_,   "ABCDE", 1,  2,  1,  "ACCDE" },
                { L_,   "ABCDE", 1,  2,  2,  "ACDDE" },
                { L_,   "ABCDE", 1,  2,  3,  "ACDEE" },
                { L_,   "ABCDE", 1,  3,  0,  "ABCDE" },
                { L_,   "ABCDE", 1,  3,  1,  "ADCDE" },
                { L_,   "ABCDE", 1,  3,  2,  "ADEDE" },
                { L_,   "ABCDE", 1,  4,  0,  "ABCDE" },
                { L_,   "ABCDE", 1,  4,  1,  "AECDE" },
                { L_,   "ABCDE", 1,  5,  0,  "ABCDE" },

                { L_,   "ABCDE", 2,  0,  0,  "ABCDE" },
                { L_,   "ABCDE", 2,  0,  1,  "ABADE" },
                { L_,   "ABCDE", 2,  0,  2,  "ABABE" },
                { L_,   "ABCDE", 2,  0,  3,  "ABABC" },
                { L_,   "ABCDE", 2,  1,  0,  "ABCDE" },
                { L_,   "ABCDE", 2,  1,  1,  "ABBDE" },
                { L_,   "ABCDE", 2,  1,  2,  "ABBCE" },
                { L_,   "ABCDE", 2,  1,  3,  "ABBCD" },
                { L_,   "ABCDE", 2,  2,  0,  "ABCDE" },
                { L_,   "ABCDE", 2,  2,  1,  "ABCDE" },
                { L_,   "ABCDE", 2,  2,  2,  "ABCDE" },
                { L_,   "ABCDE", 2,  2,  3,  "ABCDE" },
                { L_,   "ABCDE", 2,  3,  0,  "ABCDE" },
                { L_,   "ABCDE", 2,  3,  1,  "ABDDE" },
                { L_,   "ABCDE", 2,  3,  2,  "ABDEE" },
                { L_,   "ABCDE", 2,  4,  0,  "ABCDE" },
                { L_,   "ABCDE", 2,  4,  1,  "ABEDE" },
                { L_,   "ABCDE", 2,  5,  0,  "ABCDE" },

                { L_,   "ABCDE", 3,  0,  0,  "ABCDE" },
                { L_,   "ABCDE", 3,  0,  1,  "ABCAE" },
                { L_,   "ABCDE", 3,  0,  2,  "ABCAB" },
                { L_,   "ABCDE", 3,  1,  0,  "ABCDE" },
                { L_,   "ABCDE", 3,  1,  1,  "ABCBE" },
                { L_,   "ABCDE", 3,  1,  2,  "ABCBC" },
                { L_,   "ABCDE", 3,  2,  0,  "ABCDE" },
                { L_,   "ABCDE", 3,  2,  1,  "ABCCE" },
                { L_,   "ABCDE", 3,  2,  2,  "ABCCD" },
                { L_,   "ABCDE", 3,  3,  0,  "ABCDE" },
                { L_,   "ABCDE", 3,  3,  1,  "ABCDE" },
                { L_,   "ABCDE", 3,  3,  2,  "ABCDE" },
                { L_,   "ABCDE", 3,  4,  0,  "ABCDE" },
                { L_,   "ABCDE", 3,  4,  1,  "ABCEE" },
                { L_,   "ABCDE", 3,  5,  0,  "ABCDE" },

                { L_,   "ABCDE", 4,  0,  0,  "ABCDE" },
                { L_,   "ABCDE", 4,  0,  1,  "ABCDA" },
                { L_,   "ABCDE", 4,  1,  0,  "ABCDE" },
                { L_,   "ABCDE", 4,  1,  1,  "ABCDB" },
                { L_,   "ABCDE", 4,  2,  0,  "ABCDE" },
                { L_,   "ABCDE", 4,  2,  1,  "ABCDC" },
                { L_,   "ABCDE", 4,  3,  0,  "ABCDE" },
                { L_,   "ABCDE", 4,  3,  1,  "ABCDD" },
                { L_,   "ABCDE", 4,  4,  0,  "ABCDE" },
                { L_,   "ABCDE", 4,  4,  1,  "ABCDE" },
                { L_,   "ABCDE", 4,  5,  0,  "ABCDE" },

                { L_,   "ABCDE", 5,  0,  0,  "ABCDE" },
                { L_,   "ABCDE", 5,  1,  0,  "ABCDE" },
                { L_,   "ABCDE", 5,  2,  0,  "ABCDE" },
                { L_,   "ABCDE", 5,  3,  0,  "ABCDE" },
                { L_,   "ABCDE", 5,  4,  0,  "ABCDE" },
                { L_,   "ABCDE", 5,  5,  0,  "ABCDE" },

            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *X_SPEC = DATA[ti].d_xSpec;
                const int   DI     = DATA[ti].d_di;
                const int   SI     = DATA[ti].d_si;
                const int   NE     = DATA[ti].d_ne;
                const char *E_SPEC = DATA[ti].d_expSpec;

                const int   DEPTH  = strlen(X_SPEC);
                if (DEPTH > oldDepth) {
                    oldDepth = DEPTH;
                    if (verbose) { cout << '\t';  P(DEPTH); }
                }

                Obj DD(g(X_SPEC));              // control for destination
                Obj EE(g(E_SPEC));              // control for expected value

                if (veryVerbose) {
                    cout << "\t  =================================="
                                "==================================" << endl;
                    cout << "\t  "; P_(X_SPEC); P_(DI);
                                                P_(SI); P_(NE); P(E_SPEC);
                    cout << "\t\t"; P(DD);
                    cout << "\t\t"; P(EE);


                    cout << "\t\t\t---------- BLACK BOX ----------" << endl;
                }

                // CONTROL: ensure same table result as non-alias case.
                {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    Obj x2(DD, &testAllocator);  const Obj &X2 = x2; // control
                    x2.replace(DI, X, SI, NE);
                    LOOP_ASSERT(LINE, EE == X2);
                }

                if (veryVerbose) cout << "\t\treplace(di, sa, si, ne)" << endl;
                {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.replace(DI, X, SI, NE);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                  } END_BDEMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\treplace(di, item)" << endl;
                if (1 == NE) {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.replace(DI, X[SI]);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                  } END_BDEMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\toperator[](index)" << endl;
                if (1 == NE) {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x[DI] = X[SI];
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                  } END_BDEMA_EXCEPTION_TEST
                }

                if (veryVerbose) {
                    cout << "\t\t\t---------- WHITE BOX ----------" << endl;
                }

                Obj x(&testAllocator);  const Obj &X = x;

                const int STRETCH_SIZE = 50;

                stretchRemoveAll(&x, STRETCH_SIZE);

                if (veryVerbose) cout << "\t\treplace(di, sa, si, ne)" << endl;
                {
                    x.removeAll();  gg(&x, X_SPEC);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.replace(DI, X, SI, NE);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                if (veryVerbose) cout << "\t\treplace(di, item)" << endl;
                if (1 == NE) {
                    x.removeAll();  gg(&x, X_SPEC);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.replace(DI, X[SI]);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                if (veryVerbose) cout << "\t\toperator[](item)" << endl;
                if (1 == NE) {
                    x.removeAll();  gg(&x, X_SPEC);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x[DI] = X[SI];
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                }
            }
        }

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING APPEND, INSERT, REMOVE
        //   Concerns:
        //     For the 'append' and 'insert' methods, the following properties
        //     must hold:
        //       1. The source is left unaffected (apart from aliasing).
        //       2. The subsequent existing of the source has no effect on the
        //          result object (apart from aliasing).
        //       3. The function is alias safe.
        //       4. The function is exception neutral (w.r.t. allocation).
        //       5. The function preserves object invariants.
        //       6. The function is independent of internal representation.
        //     Note that all (contingent) reallocations occur strictly before
        //     the essential implementation of each method.  Therefore,
        //     concerns 1, 2, and 4 above are valid for objects in the
        //     "canonical state", but need not be repeated when concern 6
        //     ("white-box test") is addressed.
        //
        //     For the 'remove' methods, the concerns are simply to cover the
        //     full range of possible indices and numbers of elements.
        //
        // Plan:
        //   Use the enumeration technique to a depth of 5 for both the normal
        //   and alias cases.  Data is tabulated explicitly for the 'insert'
        //   method that takes a range from a source array (or itself, for the
        //   aliasing test); other methods are tested using a subset of the
        //   full test vector table.  In particular, the 'append' methods use
        //   data where the destination index equals the destination length
        //   (strlen(D_SPEC) == DI).  All methods using the entire source
        //   object use test data where the source length equals the number of
        //   elements (strlen(S_SPEC) == NE), while the "scalar" methods use
        //   data where the number of elements equals 1 (1 == NE).  In
        //   addition, the 'remove' methods switch the "d-array" and "expected"
        //   values from the 'insert' table.
        //     - In the "canonical state" (black-box) tests, we confirm that
        //       the source is unmodified by the method call, and that its
        //       subsequent destruction has no effect on the destination
        //       object.
        //     - In all cases we want to make sure that after the application
        //       of the operation, the object is allowed to go out of scope
        //       directly to enable the destructor to assert object invariants.
        //     - Each object constructed should be wrapped in separate
        //       BDEMA test assert macros and use gg as an optimization.
        //
        // Testing:
        //   void append(char item);
        //   void append(const bdea_Array<char>& sa);
        //   void append(const bdea_Array<char>& sa, int si, int ne);
        //   void append(const char *sa, int si, int ne);
        //
        //   void insert(int di, char item);
        //   void insert(int di, const bdea_Array<char>& sa);
        //   void insert(int di, const bdea_Array<char>& sa, int si, int ne);
        //   void insert(int di, const char *sa, int si, int ne);
        //
        //   void remove(int index);
        //   void remove(int index, int ne);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                << "Testing 'append', 'insert', and 'remove'" << endl
                << "========================================" << endl;

        if (verbose) cout <<
            "\nTesting x.insert(di, sa, si, ne) et. al. (no aliasing)" << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_daSpec;   // initial (destination) array
                int         d_di;       // index at which to insert into da
                const char *d_saSpec;   // source array
                int         d_si;       // index at which to insert from sa
                int         d_ne;       // number of elements to insert
                const char *d_expSpec;  // expected array value
            } DATA[] = {
                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 0
                { L_,   "",      0,  "",      0,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 1
                { L_,   "A",     0,  "",      0,  0, "A"     },
                { L_,   "A",     1,  "",      0,  0, "A"     },

                { L_,   "",      0,  "A",     0,  0, ""      },
                { L_,   "",      0,  "A",     0,  1, "A"     },
                { L_,   "",      0,  "A",     1,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 2
                { L_,   "AB",    0,  "",      0,  0, "AB"    },
                { L_,   "AB",    1,  "",      0,  0, "AB"    },
                { L_,   "AB",    2,  "",      0,  0, "AB"    },

                { L_,   "A",     0,  "B",     0,  0, "A"     },
                { L_,   "A",     0,  "B",     0,  1, "BA"    },
                { L_,   "A",     0,  "B",     1,  0, "A"     },
                { L_,   "A",     1,  "B",     0,  0, "A"     },
                { L_,   "A",     1,  "B",     0,  1, "AB"    },
                { L_,   "A",     1,  "B",     1,  0, "A"     },

                { L_,   "",      0,  "AB",    0,  0, ""      },
                { L_,   "",      0,  "AB",    0,  1, "A"     },
                { L_,   "",      0,  "AB",    0,  2, "AB"    },
                { L_,   "",      0,  "AB",    1,  0, ""      },
                { L_,   "",      0,  "AB",    1,  1, "B"     },
                { L_,   "",      0,  "AB",    2,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 3
                { L_,   "ABC",   0,  "",      0,  0, "ABC"   },
                { L_,   "ABC",   1,  "",      0,  0, "ABC"   },
                { L_,   "ABC",   2,  "",      0,  0, "ABC"   },
                { L_,   "ABC",   3,  "",      0,  0, "ABC"   },

                { L_,   "AB",    0,  "C",     0,  0, "AB"    },
                { L_,   "AB",    0,  "C",     0,  1, "CAB"   },
                { L_,   "AB",    0,  "C",     1,  0, "AB"    },
                { L_,   "AB",    1,  "C",     0,  0, "AB"    },
                { L_,   "AB",    1,  "C",     0,  1, "ACB"   },
                { L_,   "AB",    1,  "C",     1,  0, "AB"    },
                { L_,   "AB",    2,  "C",     0,  0, "AB"    },
                { L_,   "AB",    2,  "C",     0,  1, "ABC"   },
                { L_,   "AB",    2,  "C",     1,  0, "AB"    },

                { L_,   "A",     0,  "BC",    0,  0, "A"     },
                { L_,   "A",     0,  "BC",    0,  1, "BA"    },
                { L_,   "A",     0,  "BC",    0,  2, "BCA"   },
                { L_,   "A",     0,  "BC",    1,  0, "A"     },
                { L_,   "A",     0,  "BC",    1,  1, "CA"    },
                { L_,   "A",     0,  "BC",    2,  0, "A"     },
                { L_,   "A",     1,  "BC",    0,  0, "A"     },
                { L_,   "A",     1,  "BC",    0,  1, "AB"    },
                { L_,   "A",     1,  "BC",    0,  2, "ABC"   },
                { L_,   "A",     1,  "BC",    1,  0, "A"     },
                { L_,   "A",     1,  "BC",    1,  1, "AC"    },
                { L_,   "A",     1,  "BC",    2,  0, "A"     },

                { L_,   "",      0,  "ABC",   0,  0, ""      },
                { L_,   "",      0,  "ABC",   0,  1, "A"     },
                { L_,   "",      0,  "ABC",   0,  2, "AB"    },
                { L_,   "",      0,  "ABC",   0,  3, "ABC"   },
                { L_,   "",      0,  "ABC",   1,  0, ""      },
                { L_,   "",      0,  "ABC",   1,  1, "B"     },
                { L_,   "",      0,  "ABC",   1,  2, "BC"    },
                { L_,   "",      0,  "ABC",   2,  0, ""      },
                { L_,   "",      0,  "ABC",   2,  1, "C"     },
                { L_,   "",      0,  "ABC",   3,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 4
                { L_,   "ABCD",  0,  "",      0,  0, "ABCD"  },
                { L_,   "ABCD",  1,  "",      0,  0, "ABCD"  },
                { L_,   "ABCD",  2,  "",      0,  0, "ABCD"  },
                { L_,   "ABCD",  3,  "",      0,  0, "ABCD"  },
                { L_,   "ABCD",  4,  "",      0,  0, "ABCD"  },

                { L_,   "ABC",   0,  "D",     0,  0, "ABC"   },
                { L_,   "ABC",   0,  "D",     0,  1, "DABC"  },
                { L_,   "ABC",   0,  "D",     1,  0, "ABC"   },
                { L_,   "ABC",   1,  "D",     0,  0, "ABC"   },
                { L_,   "ABC",   1,  "D",     0,  1, "ADBC"  },
                { L_,   "ABC",   1,  "D",     1,  0, "ABC"   },
                { L_,   "ABC",   2,  "D",     0,  0, "ABC"   },
                { L_,   "ABC",   2,  "D",     0,  1, "ABDC"  },
                { L_,   "ABC",   2,  "D",     1,  0, "ABC"   },
                { L_,   "ABC",   3,  "D",     0,  0, "ABC"   },
                { L_,   "ABC",   3,  "D",     0,  1, "ABCD"  },
                { L_,   "ABC",   3,  "D",     1,  0, "ABC"   },

                { L_,   "AB",    0,  "CD",    0,  0, "AB"    },
                { L_,   "AB",    0,  "CD",    0,  1, "CAB"   },
                { L_,   "AB",    0,  "CD",    0,  2, "CDAB"  },
                { L_,   "AB",    0,  "CD",    1,  0, "AB"    },
                { L_,   "AB",    0,  "CD",    1,  1, "DAB"   },
                { L_,   "AB",    0,  "CD",    2,  0, "AB"    },
                { L_,   "AB",    1,  "CD",    0,  0, "AB"    },
                { L_,   "AB",    1,  "CD",    0,  1, "ACB"   },
                { L_,   "AB",    1,  "CD",    0,  2, "ACDB"  },
                { L_,   "AB",    1,  "CD",    1,  0, "AB"    },
                { L_,   "AB",    1,  "CD",    1,  1, "ADB"   },
                { L_,   "AB",    1,  "CD",    2,  0, "AB"    },
                { L_,   "AB",    2,  "CD",    0,  0, "AB"    },
                { L_,   "AB",    2,  "CD",    0,  1, "ABC"   },
                { L_,   "AB",    2,  "CD",    0,  2, "ABCD"  },
                { L_,   "AB",    2,  "CD",    1,  0, "AB"    },
                { L_,   "AB",    2,  "CD",    1,  1, "ABD"   },
                { L_,   "AB",    2,  "CD",    2,  0, "AB"    },

                { L_,   "A",     0,  "BCD",   0,  0, "A"     },
                { L_,   "A",     0,  "BCD",   0,  1, "BA"    },
                { L_,   "A",     0,  "BCD",   0,  2, "BCA"   },
                { L_,   "A",     0,  "BCD",   0,  3, "BCDA"  },
                { L_,   "A",     0,  "BCD",   1,  0, "A"     },
                { L_,   "A",     0,  "BCD",   1,  1, "CA"    },
                { L_,   "A",     0,  "BCD",   1,  2, "CDA"   },
                { L_,   "A",     0,  "BCD",   2,  0, "A"     },
                { L_,   "A",     0,  "BCD",   2,  1, "DA"    },
                { L_,   "A",     0,  "BCD",   3,  0, "A"     },
                { L_,   "A",     1,  "BCD",   0,  0, "A"     },
                { L_,   "A",     1,  "BCD",   0,  1, "AB"    },
                { L_,   "A",     1,  "BCD",   0,  2, "ABC"   },
                { L_,   "A",     1,  "BCD",   0,  3, "ABCD"  },
                { L_,   "A",     1,  "BCD",   1,  0, "A"     },
                { L_,   "A",     1,  "BCD",   1,  1, "AC"    },
                { L_,   "A",     1,  "BCD",   1,  2, "ACD"   },
                { L_,   "A",     1,  "BCD",   2,  0, "A"     },
                { L_,   "A",     1,  "BCD",   2,  1, "AD"    },
                { L_,   "A",     1,  "BCD",   3,  0, "A"     },

                { L_,   "",      0,  "ABCD",  0,  0, ""      },
                { L_,   "",      0,  "ABCD",  0,  1, "A"     },
                { L_,   "",      0,  "ABCD",  0,  2, "AB"    },
                { L_,   "",      0,  "ABCD",  0,  3, "ABC"   },
                { L_,   "",      0,  "ABCD",  0,  4, "ABCD"  },
                { L_,   "",      0,  "ABCD",  1,  0, ""      },
                { L_,   "",      0,  "ABCD",  1,  1, "B"     },
                { L_,   "",      0,  "ABCD",  1,  2, "BC"    },
                { L_,   "",      0,  "ABCD",  1,  3, "BCD"   },
                { L_,   "",      0,  "ABCD",  2,  0, ""      },
                { L_,   "",      0,  "ABCD",  2,  1, "C"     },
                { L_,   "",      0,  "ABCD",  2,  2, "CD"    },
                { L_,   "",      0,  "ABCD",  3,  0, ""      },
                { L_,   "",      0,  "ABCD",  3,  1, "D"     },
                { L_,   "",      0,  "ABCD",  4,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 5
                { L_,   "ABCDE", 0,  "",      0,  0, "ABCDE" },
                { L_,   "ABCDE", 1,  "",      0,  0, "ABCDE" },
                { L_,   "ABCDE", 2,  "",      0,  0, "ABCDE" },
                { L_,   "ABCDE", 3,  "",      0,  0, "ABCDE" },
                { L_,   "ABCDE", 4,  "",      0,  0, "ABCDE" },
                { L_,   "ABCDE", 5,  "",      0,  0, "ABCDE" },

                { L_,   "ABCD",  0,  "E",     0,  0, "ABCD"  },
                { L_,   "ABCD",  0,  "E",     0,  1, "EABCD" },
                { L_,   "ABCD",  0,  "E",     1,  0, "ABCD"  },
                { L_,   "ABCD",  1,  "E",     0,  0, "ABCD"  },
                { L_,   "ABCD",  1,  "E",     0,  1, "AEBCD" },
                { L_,   "ABCD",  1,  "E",     1,  0, "ABCD"  },
                { L_,   "ABCD",  2,  "E",     0,  0, "ABCD"  },
                { L_,   "ABCD",  2,  "E",     0,  1, "ABECD" },
                { L_,   "ABCD",  2,  "E",     1,  0, "ABCD"  },
                { L_,   "ABCD",  3,  "E",     0,  0, "ABCD"  },
                { L_,   "ABCD",  3,  "E",     0,  1, "ABCED" },
                { L_,   "ABCD",  3,  "E",     1,  0, "ABCD"  },
                { L_,   "ABCD",  4,  "E",     0,  0, "ABCD"  },
                { L_,   "ABCD",  4,  "E",     0,  1, "ABCDE" },
                { L_,   "ABCD",  4,  "E",     1,  0, "ABCD"  },

                { L_,   "ABC",   0,  "DE",    0,  0, "ABC"   },
                { L_,   "ABC",   0,  "DE",    0,  1, "DABC"  },
                { L_,   "ABC",   0,  "DE",    0,  2, "DEABC" },
                { L_,   "ABC",   0,  "DE",    1,  0, "ABC"   },
                { L_,   "ABC",   0,  "DE",    1,  1, "EABC"  },
                { L_,   "ABC",   0,  "DE",    2,  0, "ABC"   },
                { L_,   "ABC",   1,  "DE",    0,  0, "ABC"   },
                { L_,   "ABC",   1,  "DE",    0,  1, "ADBC"  },
                { L_,   "ABC",   1,  "DE",    0,  2, "ADEBC" },
                { L_,   "ABC",   1,  "DE",    1,  0, "ABC"   },
                { L_,   "ABC",   1,  "DE",    1,  1, "AEBC"  },
                { L_,   "ABC",   1,  "DE",    2,  0, "ABC"   },
                { L_,   "ABC",   2,  "DE",    0,  0, "ABC"   },
                { L_,   "ABC",   2,  "DE",    0,  1, "ABDC"  },
                { L_,   "ABC",   2,  "DE",    0,  2, "ABDEC" },
                { L_,   "ABC",   2,  "DE",    1,  0, "ABC"   },
                { L_,   "ABC",   2,  "DE",    1,  1, "ABEC"  },
                { L_,   "ABC",   2,  "DE",    2,  0, "ABC"   },
                { L_,   "ABC",   3,  "DE",    0,  0, "ABC"   },
                { L_,   "ABC",   3,  "DE",    0,  1, "ABCD"  },
                { L_,   "ABC",   3,  "DE",    0,  2, "ABCDE" },
                { L_,   "ABC",   3,  "DE",    1,  0, "ABC"   },
                { L_,   "ABC",   3,  "DE",    1,  1, "ABCE"  },
                { L_,   "ABC",   3,  "DE",    2,  0, "ABC"   },

                { L_,   "AB",    0,  "CDE",   0,  0, "AB"    },
                { L_,   "AB",    0,  "CDE",   0,  1, "CAB"   },
                { L_,   "AB",    0,  "CDE",   0,  2, "CDAB"  },
                { L_,   "AB",    0,  "CDE",   0,  3, "CDEAB" },
                { L_,   "AB",    0,  "CDE",   1,  0, "AB"    },
                { L_,   "AB",    0,  "CDE",   1,  1, "DAB"   },
                { L_,   "AB",    0,  "CDE",   1,  2, "DEAB"  },
                { L_,   "AB",    0,  "CDE",   2,  0, "AB"    },
                { L_,   "AB",    0,  "CDE",   2,  1, "EAB"   },
                { L_,   "AB",    0,  "CDE",   3,  0, "AB"    },
                { L_,   "AB",    1,  "CDE",   0,  0, "AB"    },
                { L_,   "AB",    1,  "CDE",   0,  1, "ACB"   },
                { L_,   "AB",    1,  "CDE",   0,  2, "ACDB"  },
                { L_,   "AB",    1,  "CDE",   0,  3, "ACDEB" },
                { L_,   "AB",    1,  "CDE",   1,  0, "AB"    },
                { L_,   "AB",    1,  "CDE",   1,  1, "ADB"   },
                { L_,   "AB",    1,  "CDE",   1,  2, "ADEB"  },
                { L_,   "AB",    1,  "CDE",   2,  0, "AB"    },
                { L_,   "AB",    1,  "CDE",   2,  1, "AEB"   },
                { L_,   "AB",    1,  "CDE",   3,  0, "AB"    },
                { L_,   "AB",    2,  "CDE",   0,  0, "AB"    },
                { L_,   "AB",    2,  "CDE",   0,  1, "ABC"   },
                { L_,   "AB",    2,  "CDE",   0,  2, "ABCD"  },
                { L_,   "AB",    2,  "CDE",   0,  3, "ABCDE" },
                { L_,   "AB",    2,  "CDE",   1,  0, "AB"    },
                { L_,   "AB",    2,  "CDE",   1,  1, "ABD"   },
                { L_,   "AB",    2,  "CDE",   1,  2, "ABDE"  },
                { L_,   "AB",    2,  "CDE",   2,  0, "AB"    },
                { L_,   "AB",    2,  "CDE",   2,  1, "ABE"   },
                { L_,   "AB",    2,  "CDE",   3,  0, "AB"    },

                { L_,   "A",     0,  "BCDE",  0,  0, "A"     },
                { L_,   "A",     0,  "BCDE",  0,  1, "BA"    },
                { L_,   "A",     0,  "BCDE",  0,  2, "BCA"   },
                { L_,   "A",     0,  "BCDE",  0,  3, "BCDA"  },
                { L_,   "A",     0,  "BCDE",  1,  0, "A"     },
                { L_,   "A",     0,  "BCDE",  1,  1, "CA"    },
                { L_,   "A",     0,  "BCDE",  1,  2, "CDA"   },
                { L_,   "A",     0,  "BCDE",  2,  0, "A"     },
                { L_,   "A",     0,  "BCDE",  2,  1, "DA"    },
                { L_,   "A",     0,  "BCDE",  3,  0, "A"     },
                { L_,   "A",     1,  "BCDE",  0,  0, "A"     },
                { L_,   "A",     1,  "BCDE",  0,  1, "AB"    },
                { L_,   "A",     1,  "BCDE",  0,  2, "ABC"   },
                { L_,   "A",     1,  "BCDE",  0,  3, "ABCD"  },
                { L_,   "A",     1,  "BCDE",  1,  0, "A"     },
                { L_,   "A",     1,  "BCDE",  1,  1, "AC"    },
                { L_,   "A",     1,  "BCDE",  1,  2, "ACD"   },
                { L_,   "A",     1,  "BCDE",  2,  0, "A"     },
                { L_,   "A",     1,  "BCDE",  2,  1, "AD"    },
                { L_,   "A",     1,  "BCDE",  3,  0, "A"     },

                { L_,   "",      0,  "ABCDE", 0,  0, ""      },
                { L_,   "",      0,  "ABCDE", 0,  1, "A"     },
                { L_,   "",      0,  "ABCDE", 0,  2, "AB"    },
                { L_,   "",      0,  "ABCDE", 0,  3, "ABC"   },
                { L_,   "",      0,  "ABCDE", 0,  4, "ABCD"  },
                { L_,   "",      0,  "ABCDE", 0,  5, "ABCDE" },
                { L_,   "",      0,  "ABCDE", 1,  0, ""      },
                { L_,   "",      0,  "ABCDE", 1,  1, "B"     },
                { L_,   "",      0,  "ABCDE", 1,  2, "BC"    },
                { L_,   "",      0,  "ABCDE", 1,  3, "BCD"   },
                { L_,   "",      0,  "ABCDE", 1,  4, "BCDE"  },
                { L_,   "",      0,  "ABCDE", 2,  0, ""      },
                { L_,   "",      0,  "ABCDE", 2,  1, "C"     },
                { L_,   "",      0,  "ABCDE", 2,  2, "CD"    },
                { L_,   "",      0,  "ABCDE", 2,  3, "CDE"   },
                { L_,   "",      0,  "ABCDE", 3,  0, ""      },
                { L_,   "",      0,  "ABCDE", 3,  1, "D"     },
                { L_,   "",      0,  "ABCDE", 3,  2, "DE"    },
                { L_,   "",      0,  "ABCDE", 4,  0, ""      },
                { L_,   "",      0,  "ABCDE", 4,  1, "E"     },
                { L_,   "",      0,  "ABCDE", 5,  0, ""      },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *D_SPEC = DATA[ti].d_daSpec;
                const int   DI     = DATA[ti].d_di;
                const char *S_SPEC = DATA[ti].d_saSpec;
                const int   SI     = DATA[ti].d_si;
                const int   NE     = DATA[ti].d_ne;
                const char *E_SPEC = DATA[ti].d_expSpec;

                const int   DEPTH  = strlen(D_SPEC) + strlen(S_SPEC);
                if (DEPTH > oldDepth) {
                    oldDepth = DEPTH;
                    if (verbose) { cout << '\t';  P(DEPTH); }
                }

                Obj DD(g(D_SPEC));              // control for destination
                Obj SS(g(S_SPEC));              // control for source
                Obj EE(g(E_SPEC));              // control for expected value

                if (veryVerbose) {
                    cout << "\t  =================================="
                                "==================================" << endl;
                    cout << "\t  "; P_(D_SPEC); P_(DI); P_(S_SPEC);
                                                P_(SI); P_(NE); P(E_SPEC);
                    cout << "\t\t"; P(DD);
                    cout << "\t\t"; P(SS);
                    cout << "\t\t"; P(EE);

                    cout << "\t\t\t---------- BLACK BOX ----------" << endl;
                }

                if (veryVerbose) cout << "\t\tinsert(di, sa, si, ne)" << endl;
                {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    {
                        Obj s(SS, &testAllocator);  const Obj &S = s;
                        if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                        x.insert(DI, s, SI, NE);        // source non-'const'
                        if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                  } END_BDEMA_EXCEPTION_TEST

                  BEGIN_BDEMA_EXCEPTION_TEST {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    {
                        Obj s(SS, &testAllocator);  const Obj &S = s;
                        if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                        x.insert(DI, s.data(), SI, NE); // source non-'const'
                        if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                  } END_BDEMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\tinsert(di, sa)" << endl;
                if ((int)strlen(S_SPEC) == NE) {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    {
                        Obj s(SS, &testAllocator);  const Obj &S = s;
                        if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                        x.insert(DI, s);                // source non-'const'
                        if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                  } END_BDEMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\tinsert(di, item)" << endl;
                if (1 == NE) {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    {
                        Obj s(SS, &testAllocator);  const Obj &S = s;
                        if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                        x.insert(DI, s[SI]);            // source non-'const'
                        if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                  } END_BDEMA_EXCEPTION_TEST
                }

                // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                if (veryVerbose) cout << "\t\tappend(sa, si, ne)" << endl;
                if ((int)strlen(D_SPEC) == DI) {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    {
                        Obj s(SS, &testAllocator);  const Obj &S = s;
                        if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                        x.append(s, SI, NE);            // source non-'const'
                        if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                  } END_BDEMA_EXCEPTION_TEST

                  BEGIN_BDEMA_EXCEPTION_TEST {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    {
                        Obj s(SS, &testAllocator);  const Obj &S = s;
                        if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                        x.append(s.data(), SI, NE);     // source non-'const'
                        if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                  } END_BDEMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\tappend(sa)" << endl;
                if ((int)strlen(D_SPEC) == DI && (int)strlen(S_SPEC) == NE) {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    {
                        Obj s(SS, &testAllocator);  const Obj &S = s;
                        if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                        x.append(s);                    // source non-'const'
                        if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                  } END_BDEMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\tappend(item)" << endl;
                if ((int)strlen(D_SPEC) == DI && 1 == NE) {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    {
                        Obj s(SS, &testAllocator);  const Obj &S = s;
                        if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                        x.append(s[SI]);                // source non-'const'
                        if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                  } END_BDEMA_EXCEPTION_TEST
                }

                // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                if (veryVerbose) cout << "\t\tremove(index, ne)" << endl;
                {
                  BEGIN_BDEMA_EXCEPTION_TEST {  // Note specs are switched.
                    Obj x(EE, &testAllocator);  const Obj &X = x;
                    if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                    x.remove(DI, NE);
                    if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                    LOOP_ASSERT(LINE, DD == X);
                  } END_BDEMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\tremove(index)" << endl;
                if (1 == NE) {
                  BEGIN_BDEMA_EXCEPTION_TEST {  // Note specs are switched
                    Obj x(EE, &testAllocator);  const Obj &X = x;
                    if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                    x.remove(DI);
                    if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                    LOOP_ASSERT(LINE, DD == X);
                  } END_BDEMA_EXCEPTION_TEST
                }

                if (veryVerbose) {
                    cout << "\t\t\t---------- WHITE BOX ----------" << endl;
                }

                Obj x(&testAllocator);  const Obj &X = x;

                const int STRETCH_SIZE = 50;

                stretchRemoveAll(&x, STRETCH_SIZE);

                const int NUM_BLOCKS = testAllocator.numBlocksTotal();
                const int NUM_BYTES  = testAllocator.numBytesInUse();

                if (veryVerbose) cout << "\t\tinsert(di, sa, si, ne)" << endl;
                {
                    x.removeAll();  gg(&x, D_SPEC);
                    if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                    x.insert(DI, SS, SI, NE);
                    if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                    LOOP_ASSERT(LINE, EE == X);

                    x.removeAll();  gg(&x, D_SPEC);
                    if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                    x.insert(DI, SS.data(), SI, NE);
                    if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                if (veryVerbose) cout << "\t\tinsert(di, sa)" << endl;
                if ((int)strlen(S_SPEC) == NE) {
                    x.removeAll();  gg(&x, D_SPEC);
                    if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                    x.insert(DI, SS);
                    if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                if (veryVerbose) cout << "\t\tinsert(di, item)" << endl;
                if (1 == NE) {
                    x.removeAll();  gg(&x, D_SPEC);
                    if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                    x.insert(DI, SS[SI]);
                    if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                if (veryVerbose) cout << "\t\tappend(sa, si, ne)" << endl;
                if ((int)strlen(D_SPEC) == DI) {
                    x.removeAll();  gg(&x, D_SPEC);
                    if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                    x.append(SS, SI, NE);
                    if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                    LOOP_ASSERT(LINE, EE == X);

                    x.removeAll();  gg(&x, D_SPEC);
                    if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                    x.append(SS.data(), SI, NE);
                    if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                if (veryVerbose) cout << "\t\tappend(sa)" << endl;
                if ((int)strlen(D_SPEC) == DI && (int)strlen(S_SPEC) == NE) {
                    x.removeAll();  gg(&x, D_SPEC);
                    if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                    x.append(SS);
                    if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                if (veryVerbose) cout << "\t\tappend(item)" << endl;
                if ((int)strlen(D_SPEC) == DI && 1 == NE) {
                    x.removeAll();  gg(&x, D_SPEC);
                    if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                    x.append(SS[SI]);
                    if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                if (veryVerbose) cout << "\t\tremove(index, ne)" << endl;
                {
                    x.removeAll();  gg(&x, E_SPEC);  // Note: specs switched!
                    if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                    x.remove(DI, NE);
                    if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                    LOOP_ASSERT(LINE, DD == X);
                }

                if (veryVerbose) cout << "\t\tremove(index)" << endl;
                if (1 == NE) {
                    x.removeAll();  gg(&x, E_SPEC);  // Note: specs switched!
                    if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                    x.remove(DI);
                    if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                    LOOP_ASSERT(LINE, DD == X);
                }

                LOOP_ASSERT(LINE, NUM_BLOCKS ==testAllocator.numBlocksTotal());
                LOOP_ASSERT(LINE, NUM_BYTES == testAllocator.numBytesInUse());
            }
        }

        //---------------------------------------------------------------------

        if (verbose) cout <<
            "\nTesting x.insert(di, sa, si, ne) et. al. (aliasing)" << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_xSpec;    // initial array (= srcArray)
                int         d_di;       // index at which to insert into x
                int         d_si;       // index at which to insert from x
                int         d_ne;       // number of elements to insert
                const char *d_expSpec;  // expected array value
            } DATA[] = {
                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 0
                { L_,   "",      0,   0,  0,  ""      },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 1
                { L_,   "A",     0,   0,  0,  "A"     },
                { L_,   "A",     0,   0,  1,  "AA"    },
                { L_,   "A",     0,   1,  0,  "A"     },

                { L_,   "A",     1,   0,  0,  "A"     },
                { L_,   "A",     1,   0,  1,  "AA"    },
                { L_,   "A",     1,   1,  0,  "A"     },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 2
                { L_,   "AB",    0,   0,  0,  "AB"    },
                { L_,   "AB",    0,   0,  1,  "AAB"   },
                { L_,   "AB",    0,   0,  2,  "ABAB"  },
                { L_,   "AB",    0,   1,  0,  "AB"    },
                { L_,   "AB",    0,   1,  1,  "BAB"   },
                { L_,   "AB",    0,   2,  0,  "AB"    },

                { L_,   "AB",    1,   0,  0,  "AB"    },
                { L_,   "AB",    1,   0,  1,  "AAB"   },
                { L_,   "AB",    1,   0,  2,  "AABB"  },
                { L_,   "AB",    1,   1,  0,  "AB"    },
                { L_,   "AB",    1,   1,  1,  "ABB"   },
                { L_,   "AB",    1,   2,  0,  "AB"    },

                { L_,   "AB",    2,   0,  0,  "AB"    },
                { L_,   "AB",    2,   0,  1,  "ABA"   },
                { L_,   "AB",    2,   0,  2,  "ABAB"  },
                { L_,   "AB",    2,   1,  0,  "AB"    },
                { L_,   "AB",    2,   1,  1,  "ABB"   },
                { L_,   "AB",    2,   2,  0,  "AB"    },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 3
                { L_,   "ABC",   0,   0,  0,  "ABC"     },
                { L_,   "ABC",   0,   0,  1,  "AABC"    },
                { L_,   "ABC",   0,   0,  2,  "ABABC"   },
                { L_,   "ABC",   0,   0,  3,  "ABCABC"  },
                { L_,   "ABC",   0,   1,  0,  "ABC"     },
                { L_,   "ABC",   0,   1,  1,  "BABC"    },
                { L_,   "ABC",   0,   1,  2,  "BCABC"   },
                { L_,   "ABC",   0,   2,  0,  "ABC"     },
                { L_,   "ABC",   0,   2,  1,  "CABC"    },
                { L_,   "ABC",   0,   3,  0,  "ABC"     },

                { L_,   "ABC",   1,   0,  0,  "ABC"     },
                { L_,   "ABC",   1,   0,  1,  "AABC"    },
                { L_,   "ABC",   1,   0,  2,  "AABBC"   },
                { L_,   "ABC",   1,   0,  3,  "AABCBC"  },
                { L_,   "ABC",   1,   1,  0,  "ABC"     },
                { L_,   "ABC",   1,   1,  1,  "ABBC"    },
                { L_,   "ABC",   1,   1,  2,  "ABCBC"   },
                { L_,   "ABC",   1,   2,  0,  "ABC"     },
                { L_,   "ABC",   1,   2,  1,  "ACBC"    },
                { L_,   "ABC",   1,   3,  0,  "ABC"     },

                { L_,   "ABC",   2,   0,  0,  "ABC"     },
                { L_,   "ABC",   2,   0,  1,  "ABAC"    },
                { L_,   "ABC",   2,   0,  2,  "ABABC"   },
                { L_,   "ABC",   2,   0,  3,  "ABABCC"  },
                { L_,   "ABC",   2,   1,  0,  "ABC"     },
                { L_,   "ABC",   2,   1,  1,  "ABBC"    },
                { L_,   "ABC",   2,   1,  2,  "ABBCC"   },
                { L_,   "ABC",   2,   2,  0,  "ABC"     },
                { L_,   "ABC",   2,   2,  1,  "ABCC"    },
                { L_,   "ABC",   2,   3,  0,  "ABC"     },

                { L_,   "ABC",   3,   0,  0,  "ABC"     },
                { L_,   "ABC",   3,   0,  1,  "ABCA"    },
                { L_,   "ABC",   3,   0,  2,  "ABCAB"   },
                { L_,   "ABC",   3,   0,  3,  "ABCABC"  },
                { L_,   "ABC",   3,   1,  0,  "ABC"     },
                { L_,   "ABC",   3,   1,  1,  "ABCB"    },
                { L_,   "ABC",   3,   1,  2,  "ABCBC"   },
                { L_,   "ABC",   3,   2,  0,  "ABC"     },
                { L_,   "ABC",   3,   2,  1,  "ABCC"    },
                { L_,   "ABC",   3,   3,  0,  "ABC"     },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 4
                { L_,   "ABCD",  0,   0,  0,  "ABCD"      },
                { L_,   "ABCD",  0,   0,  1,  "AABCD"     },
                { L_,   "ABCD",  0,   0,  2,  "ABABCD"    },
                { L_,   "ABCD",  0,   0,  3,  "ABCABCD"   },
                { L_,   "ABCD",  0,   0,  4,  "ABCDABCD"  },
                { L_,   "ABCD",  0,   1,  0,  "ABCD"      },
                { L_,   "ABCD",  0,   1,  1,  "BABCD"     },
                { L_,   "ABCD",  0,   1,  2,  "BCABCD"    },
                { L_,   "ABCD",  0,   1,  3,  "BCDABCD"   },
                { L_,   "ABCD",  0,   2,  0,  "ABCD"      },
                { L_,   "ABCD",  0,   2,  1,  "CABCD"     },
                { L_,   "ABCD",  0,   2,  2,  "CDABCD"    },
                { L_,   "ABCD",  0,   3,  0,  "ABCD"      },
                { L_,   "ABCD",  0,   3,  1,  "DABCD"     },
                { L_,   "ABCD",  0,   4,  0,  "ABCD"      },

                { L_,   "ABCD",  1,   0,  0,  "ABCD"      },
                { L_,   "ABCD",  1,   0,  1,  "AABCD"     },
                { L_,   "ABCD",  1,   0,  2,  "AABBCD"    },
                { L_,   "ABCD",  1,   0,  3,  "AABCBCD"   },
                { L_,   "ABCD",  1,   0,  4,  "AABCDBCD"  },
                { L_,   "ABCD",  1,   1,  0,  "ABCD"      },
                { L_,   "ABCD",  1,   1,  1,  "ABBCD"     },
                { L_,   "ABCD",  1,   1,  2,  "ABCBCD"    },
                { L_,   "ABCD",  1,   1,  3,  "ABCDBCD"   },
                { L_,   "ABCD",  1,   2,  0,  "ABCD"      },
                { L_,   "ABCD",  1,   2,  1,  "ACBCD"     },
                { L_,   "ABCD",  1,   2,  2,  "ACDBCD"    },
                { L_,   "ABCD",  1,   3,  0,  "ABCD"      },
                { L_,   "ABCD",  1,   3,  1,  "ADBCD"     },
                { L_,   "ABCD",  1,   4,  0,  "ABCD"      },

                { L_,   "ABCD",  2,   0,  0,  "ABCD"      },
                { L_,   "ABCD",  2,   0,  1,  "ABACD"     },
                { L_,   "ABCD",  2,   0,  2,  "ABABCD"    },
                { L_,   "ABCD",  2,   0,  3,  "ABABCCD"   },
                { L_,   "ABCD",  2,   0,  4,  "ABABCDCD"  },
                { L_,   "ABCD",  2,   1,  0,  "ABCD"      },
                { L_,   "ABCD",  2,   1,  1,  "ABBCD"     },
                { L_,   "ABCD",  2,   1,  2,  "ABBCCD"    },
                { L_,   "ABCD",  2,   1,  3,  "ABBCDCD"   },
                { L_,   "ABCD",  2,   2,  0,  "ABCD"      },
                { L_,   "ABCD",  2,   2,  1,  "ABCCD"     },
                { L_,   "ABCD",  2,   2,  2,  "ABCDCD"    },
                { L_,   "ABCD",  2,   3,  0,  "ABCD"      },
                { L_,   "ABCD",  2,   3,  1,  "ABDCD"     },
                { L_,   "ABCD",  2,   4,  0,  "ABCD"      },

                { L_,   "ABCD",  3,   0,  0,  "ABCD"      },
                { L_,   "ABCD",  3,   0,  1,  "ABCAD"     },
                { L_,   "ABCD",  3,   0,  2,  "ABCABD"    },
                { L_,   "ABCD",  3,   0,  3,  "ABCABCD"   },
                { L_,   "ABCD",  3,   0,  4,  "ABCABCDD"  },
                { L_,   "ABCD",  3,   1,  0,  "ABCD"      },
                { L_,   "ABCD",  3,   1,  1,  "ABCBD"     },
                { L_,   "ABCD",  3,   1,  2,  "ABCBCD"    },
                { L_,   "ABCD",  3,   1,  3,  "ABCBCDD"   },
                { L_,   "ABCD",  3,   2,  0,  "ABCD"      },
                { L_,   "ABCD",  3,   2,  1,  "ABCCD"     },
                { L_,   "ABCD",  3,   2,  2,  "ABCCDD"    },
                { L_,   "ABCD",  3,   3,  0,  "ABCD"      },
                { L_,   "ABCD",  3,   3,  1,  "ABCDD"     },
                { L_,   "ABCD",  3,   4,  0,  "ABCD"      },

                { L_,   "ABCD",  4,   0,  0,  "ABCD"      },
                { L_,   "ABCD",  4,   0,  1,  "ABCDA"     },
                { L_,   "ABCD",  4,   0,  2,  "ABCDAB"    },
                { L_,   "ABCD",  4,   0,  3,  "ABCDABC"   },
                { L_,   "ABCD",  4,   0,  4,  "ABCDABCD"  },
                { L_,   "ABCD",  4,   1,  0,  "ABCD"      },
                { L_,   "ABCD",  4,   1,  1,  "ABCDB"     },
                { L_,   "ABCD",  4,   1,  2,  "ABCDBC"    },
                { L_,   "ABCD",  4,   1,  3,  "ABCDBCD"   },
                { L_,   "ABCD",  4,   2,  0,  "ABCD"      },
                { L_,   "ABCD",  4,   2,  1,  "ABCDC"     },
                { L_,   "ABCD",  4,   2,  2,  "ABCDCD"    },
                { L_,   "ABCD",  4,   3,  0,  "ABCD"      },
                { L_,   "ABCD",  4,   3,  1,  "ABCDD"     },
                { L_,   "ABCD",  4,   4,  0,  "ABCD"      },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 5
                { L_,   "ABCDE", 0,   0,  0,  "ABCDE"      },
                { L_,   "ABCDE", 0,   0,  1,  "AABCDE"     },
                { L_,   "ABCDE", 0,   0,  2,  "ABABCDE"    },
                { L_,   "ABCDE", 0,   0,  3,  "ABCABCDE"   },
                { L_,   "ABCDE", 0,   0,  4,  "ABCDABCDE"  },
                { L_,   "ABCDE", 0,   0,  5,  "ABCDEABCDE" },
                { L_,   "ABCDE", 0,   1,  0,  "ABCDE"      },
                { L_,   "ABCDE", 0,   1,  1,  "BABCDE"     },
                { L_,   "ABCDE", 0,   1,  2,  "BCABCDE"    },
                { L_,   "ABCDE", 0,   1,  3,  "BCDABCDE"   },
                { L_,   "ABCDE", 0,   1,  4,  "BCDEABCDE"  },
                { L_,   "ABCDE", 0,   2,  0,  "ABCDE"      },
                { L_,   "ABCDE", 0,   2,  1,  "CABCDE"     },
                { L_,   "ABCDE", 0,   2,  2,  "CDABCDE"    },
                { L_,   "ABCDE", 0,   2,  3,  "CDEABCDE"   },
                { L_,   "ABCDE", 0,   3,  0,  "ABCDE"      },
                { L_,   "ABCDE", 0,   3,  1,  "DABCDE"     },
                { L_,   "ABCDE", 0,   3,  2,  "DEABCDE"    },
                { L_,   "ABCDE", 0,   4,  0,  "ABCDE"      },
                { L_,   "ABCDE", 0,   4,  1,  "EABCDE"     },
                { L_,   "ABCDE", 0,   5,  0,  "ABCDE"      },

                { L_,   "ABCDE", 1,   0,  0,  "ABCDE"      },
                { L_,   "ABCDE", 1,   0,  1,  "AABCDE"     },
                { L_,   "ABCDE", 1,   0,  2,  "AABBCDE"    },
                { L_,   "ABCDE", 1,   0,  3,  "AABCBCDE"   },
                { L_,   "ABCDE", 1,   0,  4,  "AABCDBCDE"  },
                { L_,   "ABCDE", 1,   0,  5,  "AABCDEBCDE" },
                { L_,   "ABCDE", 1,   1,  0,  "ABCDE"      },
                { L_,   "ABCDE", 1,   1,  1,  "ABBCDE"     },
                { L_,   "ABCDE", 1,   1,  2,  "ABCBCDE"    },
                { L_,   "ABCDE", 1,   1,  3,  "ABCDBCDE"   },
                { L_,   "ABCDE", 1,   1,  4,  "ABCDEBCDE"  },
                { L_,   "ABCDE", 1,   2,  0,  "ABCDE"      },
                { L_,   "ABCDE", 1,   2,  1,  "ACBCDE"     },
                { L_,   "ABCDE", 1,   2,  2,  "ACDBCDE"    },
                { L_,   "ABCDE", 1,   2,  3,  "ACDEBCDE"   },
                { L_,   "ABCDE", 1,   3,  0,  "ABCDE"      },
                { L_,   "ABCDE", 1,   3,  1,  "ADBCDE"     },
                { L_,   "ABCDE", 1,   3,  2,  "ADEBCDE"    },
                { L_,   "ABCDE", 1,   4,  0,  "ABCDE"      },
                { L_,   "ABCDE", 1,   4,  1,  "AEBCDE"     },
                { L_,   "ABCDE", 1,   5,  0,  "ABCDE"      },

                { L_,   "ABCDE", 2,   0,  0,  "ABCDE"      },
                { L_,   "ABCDE", 2,   0,  1,  "ABACDE"     },
                { L_,   "ABCDE", 2,   0,  2,  "ABABCDE"    },
                { L_,   "ABCDE", 2,   0,  3,  "ABABCCDE"   },
                { L_,   "ABCDE", 2,   0,  4,  "ABABCDCDE"  },
                { L_,   "ABCDE", 2,   0,  5,  "ABABCDECDE" },
                { L_,   "ABCDE", 2,   1,  0,  "ABCDE"      },
                { L_,   "ABCDE", 2,   1,  1,  "ABBCDE"     },
                { L_,   "ABCDE", 2,   1,  2,  "ABBCCDE"    },
                { L_,   "ABCDE", 2,   1,  3,  "ABBCDCDE"   },
                { L_,   "ABCDE", 2,   1,  4,  "ABBCDECDE"  },
                { L_,   "ABCDE", 2,   2,  0,  "ABCDE"      },
                { L_,   "ABCDE", 2,   2,  1,  "ABCCDE"     },
                { L_,   "ABCDE", 2,   2,  2,  "ABCDCDE"    },
                { L_,   "ABCDE", 2,   2,  3,  "ABCDECDE"   },
                { L_,   "ABCDE", 2,   3,  0,  "ABCDE"      },
                { L_,   "ABCDE", 2,   3,  1,  "ABDCDE"     },
                { L_,   "ABCDE", 2,   3,  2,  "ABDECDE"    },
                { L_,   "ABCDE", 2,   4,  0,  "ABCDE"      },
                { L_,   "ABCDE", 2,   4,  1,  "ABECDE"     },
                { L_,   "ABCDE", 2,   5,  0,  "ABCDE"      },

                { L_,   "ABCDE", 3,   0,  0,  "ABCDE"      },
                { L_,   "ABCDE", 3,   0,  1,  "ABCADE"     },
                { L_,   "ABCDE", 3,   0,  2,  "ABCABDE"    },
                { L_,   "ABCDE", 3,   0,  3,  "ABCABCDE"   },
                { L_,   "ABCDE", 3,   0,  4,  "ABCABCDDE"  },
                { L_,   "ABCDE", 3,   0,  5,  "ABCABCDEDE" },
                { L_,   "ABCDE", 3,   1,  0,  "ABCDE"      },
                { L_,   "ABCDE", 3,   1,  1,  "ABCBDE"     },
                { L_,   "ABCDE", 3,   1,  2,  "ABCBCDE"    },
                { L_,   "ABCDE", 3,   1,  3,  "ABCBCDDE"   },
                { L_,   "ABCDE", 3,   1,  4,  "ABCBCDEDE"  },
                { L_,   "ABCDE", 3,   2,  0,  "ABCDE"      },
                { L_,   "ABCDE", 3,   2,  1,  "ABCCDE"     },
                { L_,   "ABCDE", 3,   2,  2,  "ABCCDDE"    },
                { L_,   "ABCDE", 3,   2,  3,  "ABCCDEDE"   },
                { L_,   "ABCDE", 3,   3,  0,  "ABCDE"      },
                { L_,   "ABCDE", 3,   3,  1,  "ABCDDE"     },
                { L_,   "ABCDE", 3,   3,  2,  "ABCDEDE"    },
                { L_,   "ABCDE", 3,   4,  0,  "ABCDE"      },
                { L_,   "ABCDE", 3,   4,  1,  "ABCEDE"     },
                { L_,   "ABCDE", 3,   5,  0,  "ABCDE"      },

                { L_,   "ABCDE", 4,   0,  0,  "ABCDE"      },
                { L_,   "ABCDE", 4,   0,  1,  "ABCDAE"     },
                { L_,   "ABCDE", 4,   0,  2,  "ABCDABE"    },
                { L_,   "ABCDE", 4,   0,  3,  "ABCDABCE"   },
                { L_,   "ABCDE", 4,   0,  4,  "ABCDABCDE"  },
                { L_,   "ABCDE", 4,   0,  5,  "ABCDABCDEE" },
                { L_,   "ABCDE", 4,   1,  0,  "ABCDE"      },
                { L_,   "ABCDE", 4,   1,  1,  "ABCDBE"     },
                { L_,   "ABCDE", 4,   1,  2,  "ABCDBCE"    },
                { L_,   "ABCDE", 4,   1,  3,  "ABCDBCDE"   },
                { L_,   "ABCDE", 4,   1,  4,  "ABCDBCDEE"  },
                { L_,   "ABCDE", 4,   2,  0,  "ABCDE"      },
                { L_,   "ABCDE", 4,   2,  1,  "ABCDCE"     },
                { L_,   "ABCDE", 4,   2,  2,  "ABCDCDE"    },
                { L_,   "ABCDE", 4,   2,  3,  "ABCDCDEE"   },
                { L_,   "ABCDE", 4,   3,  0,  "ABCDE"      },
                { L_,   "ABCDE", 4,   3,  1,  "ABCDDE"     },
                { L_,   "ABCDE", 4,   3,  2,  "ABCDDEE"    },
                { L_,   "ABCDE", 4,   4,  0,  "ABCDE"      },
                { L_,   "ABCDE", 4,   4,  1,  "ABCDEE"     },
                { L_,   "ABCDE", 4,   5,  0,  "ABCDE"      },

                { L_,   "ABCDE", 5,   0,  0,  "ABCDE"      },
                { L_,   "ABCDE", 5,   0,  1,  "ABCDEA"     },
                { L_,   "ABCDE", 5,   0,  2,  "ABCDEAB"    },
                { L_,   "ABCDE", 5,   0,  3,  "ABCDEABC"   },
                { L_,   "ABCDE", 5,   0,  4,  "ABCDEABCD"  },
                { L_,   "ABCDE", 5,   0,  5,  "ABCDEABCDE" },
                { L_,   "ABCDE", 5,   1,  0,  "ABCDE"      },
                { L_,   "ABCDE", 5,   1,  1,  "ABCDEB"     },
                { L_,   "ABCDE", 5,   1,  2,  "ABCDEBC"    },
                { L_,   "ABCDE", 5,   1,  3,  "ABCDEBCD"   },
                { L_,   "ABCDE", 5,   1,  4,  "ABCDEBCDE"  },
                { L_,   "ABCDE", 5,   2,  0,  "ABCDE"      },
                { L_,   "ABCDE", 5,   2,  1,  "ABCDEC"     },
                { L_,   "ABCDE", 5,   2,  2,  "ABCDECD"    },
                { L_,   "ABCDE", 5,   2,  3,  "ABCDECDE"   },
                { L_,   "ABCDE", 5,   3,  0,  "ABCDE"      },
                { L_,   "ABCDE", 5,   3,  1,  "ABCDED"     },
                { L_,   "ABCDE", 5,   3,  2,  "ABCDEDE"    },
                { L_,   "ABCDE", 5,   4,  0,  "ABCDE"      },
                { L_,   "ABCDE", 5,   4,  1,  "ABCDEE"     },
                { L_,   "ABCDE", 5,   5,  0,  "ABCDE"      },

            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *X_SPEC = DATA[ti].d_xSpec;
                const int   DI     = DATA[ti].d_di;
                const int   SI     = DATA[ti].d_si;
                const int   NE     = DATA[ti].d_ne;
                const char *E_SPEC = DATA[ti].d_expSpec;

                const int   DEPTH  = (int)strlen(X_SPEC);
                if (DEPTH > oldDepth) {
                    oldDepth = DEPTH;
                    if (verbose) { cout << '\t';  P(DEPTH); }
                }

                Obj DD(g(X_SPEC));              // control for destination
                Obj EE(g(E_SPEC));              // control for expected value

                if (veryVerbose) {
                    cout << "\t  =================================="
                                "==================================" << endl;
                    cout << "\t  "; P_(X_SPEC); P_(DI);
                                                P_(SI); P_(NE); P(E_SPEC);
                    cout << "\t\t"; P(DD);
                    cout << "\t\t"; P(EE);


                    cout << "\t\t\t---------- BLACK BOX ----------" << endl;
                }

                // CONTROL: ensure same table result as non-alias case.
                {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    Obj x2(DD, &testAllocator);  const Obj &X2 = x2; // control
                    x2.insert(DI, X, SI, NE);
                    LOOP_ASSERT(LINE, EE == X2);
                }

                {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    Obj x2(DD, &testAllocator);  const Obj &X2 = x2; // control
                    x2.insert(DI, X.data(), SI, NE);
                    LOOP_ASSERT(LINE, EE == X2);
                }

                if (veryVerbose) cout << "\t\tinsert(di, sa, si, ne)" << endl;
                {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                    x.insert(DI, X, SI, NE);
                    if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                    LOOP_ASSERT(LINE, EE == X);
                  } END_BDEMA_EXCEPTION_TEST
                }

                {
                    for (int d = 0, si = SI; si >= 0; ++d, --si) {
                      BEGIN_BDEMA_EXCEPTION_TEST {
                        Obj x(DD, &testAllocator);  const Obj &X = x;
                        if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                        x.insert(DI, X.data()+d, si, NE);
                        if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                        LOOP_ASSERT(LINE, EE == X);
                      } END_BDEMA_EXCEPTION_TEST
                    }
                }

                if (veryVerbose) cout << "\t\tinsert(di, sa)" << endl;
                if ((int)strlen(X_SPEC) == NE) {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                    x.insert(DI, X);
                    if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                    LOOP_ASSERT(LINE, EE == X);
                  } END_BDEMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\tinsert(di, item)" << endl;
                if (1 == NE) {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                    x.insert(DI, X[SI]);
                    if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                    LOOP_ASSERT(LINE, EE == X);
                  } END_BDEMA_EXCEPTION_TEST
                }

                // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                if (veryVerbose) cout << "\t\tappend(sa, si, ne)" << endl;
                if ((int)strlen(X_SPEC) == DI) {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                    x.append(X, SI, NE);
                    if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                    LOOP_ASSERT(LINE, EE == X);
                  } END_BDEMA_EXCEPTION_TEST
                }

                if ((int)strlen(X_SPEC) == DI) {
                    for (int d = 0, si = SI; si >= 0; ++d, --si) {
                      BEGIN_BDEMA_EXCEPTION_TEST {
                        Obj x(DD, &testAllocator);  const Obj &X = x;
                        if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                        x.append(X.data()+d, si, NE);
                        if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                        LOOP_ASSERT(LINE, EE == X);
                      } END_BDEMA_EXCEPTION_TEST
                    }
                }

                if (veryVerbose) cout << "\t\tappend(sa)" << endl;
                if ((int)strlen(X_SPEC) == DI && (int)strlen(X_SPEC) == NE) {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                    x.append(X);
                    if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                    LOOP_ASSERT(LINE, EE == X);
                  } END_BDEMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\tappend(item)" << endl;
                if ((int)strlen(X_SPEC) == DI && 1 == NE) {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                    x.append(X[SI]);
                    if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                    LOOP_ASSERT(LINE, EE == X);
                  } END_BDEMA_EXCEPTION_TEST
                }

                if (veryVerbose) {
                    cout << "\t\t\t---------- WHITE BOX ----------" << endl;
                }

                Obj x(&testAllocator);  const Obj &X = x;

                const int STRETCH_SIZE = 50;

                stretchRemoveAll(&x, STRETCH_SIZE);

                const int NUM_BLOCKS = testAllocator.numBlocksTotal();
                const int NUM_BYTES  = testAllocator.numBytesInUse();

                if (veryVerbose) cout << "\t\tinsert(di, sa, si, ne)" << endl;
                {
                    x.removeAll();  gg(&x, X_SPEC);
                    if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                    x.insert(DI, X, SI, NE);
                    if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                    LOOP_ASSERT(LINE, EE == X);

                    for (int d = 0, si = SI; si >= 0; ++d, --si) {
                        x.removeAll();  gg(&x, X_SPEC);
                        if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                        x.insert(DI, X.data()+d, si, NE);
                        if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                        LOOP_ASSERT(LINE, EE == X);
                    }
                }

                if (veryVerbose) cout << "\t\tinsert(di, sa)" << endl;
                if ((int)strlen(X_SPEC) == NE) {
                    x.removeAll();  gg(&x, X_SPEC);
                    if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                    x.insert(DI, X);
                    if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                if (veryVerbose) cout << "\t\tinsert(di, item)" << endl;
                if (1 == NE) {
                    x.removeAll();  gg(&x, X_SPEC);
                    if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                    x.insert(DI, X[SI]);
                    if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                if (veryVerbose) cout << "\t\tappend(sa, si, ne)" << endl;
                if ((int)strlen(X_SPEC) == DI) {
                    x.removeAll();  gg(&x, X_SPEC);
                    if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                    x.append(X, SI, NE);
                    if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                    LOOP_ASSERT(LINE, EE == X);

                    for (int d = 0, si = SI; si >= 0; ++d, --si) {
                        x.removeAll();  gg(&x, X_SPEC);
                        if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                        x.append(X.data()+d, si, NE);
                        if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                        LOOP_ASSERT(LINE, EE == X);
                    }
                }

                if (veryVerbose) cout << "\t\tappend(sa)" << endl;
                if ((int)strlen(X_SPEC) == DI && (int)strlen(X_SPEC) == NE) {
                    x.removeAll();  gg(&x, X_SPEC);
                    if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                    x.append(X);
                    if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                if (veryVerbose) cout << "\t\tappend(item)" << endl;
                if ((int)strlen(X_SPEC) == DI && 1 == NE) {
                    x.removeAll();  gg(&x, X_SPEC);
                    if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                    x.append(X[SI]);
                    if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                LOOP_ASSERT(LINE, NUM_BLOCKS ==testAllocator.numBlocksTotal());
                LOOP_ASSERT(LINE, NUM_BYTES == testAllocator.numBytesInUse());
            }
        }

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING SET-LENGTH FUNCTIONS:
        //   We have the following concerns:
        //    - The resulting length is correct.
        //    - The resulting element values are correct when:
        //        new length <  initial length
        //        new length == initial length
        //        new length >  initial length (undefined for 'setLengthRaw')
        //   We are also concerned that the test data include sufficient
        //   differences in initial and final length that resizing is
        //   guaranteed to occur.  Beyond that, no explicit "white box" test is
        //   required.
        //
        // Plan:
        //   Specify a set A of lengths.  For each a1 in A construct an object
        //   x of length a1 with each element in x initialized to an arbitrary
        //   but known value V.  For each a2 in A use the 'setLength' method
        //   under test to set the length of x and potentially remove or set
        //   element values as per the method's contract.  Use the basic
        //   accessors to verify the length and element values of the modified
        //   object x.
        //
        // Testing:
        //   void setLengthRaw(int newLength);
        //   void setLength(int newLength);
        //   void setLength(int newLength, char iVal);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Set-Length Functions" << endl
                          << "============================" << endl;

        if (verbose) cout << "\nTesting 'setLengthRaw'" << endl;
        {
            const int lengths[] = { 0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17 };
            const int NUM_TESTS = sizeof lengths / sizeof lengths[0];
            const Element VALUE  = 'Q'; // ADJUST
            for (int i = 0; i < NUM_TESTS; ++i) {
                const int a1 = lengths[i];
                if (verbose) { cout << "\t"; P(a1); }
                for (int j = 0; j < NUM_TESTS; ++j) {
                    const int a2 = lengths[j];
                    if (veryVerbose) { cout << "\t\t"; P(a2); }
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    Obj mX(a1, VALUE, &testAllocator);
                    const Obj &X = mX;
                    if (veryVerbose) P(X);
                    LOOP2_ASSERT(i, j, a1 == X.length());
                    mX.setLengthRaw(a2);
                    if (veryVerbose) P(X);
                    LOOP2_ASSERT(i, j, a2 == X.length());
                    for (int k = 0; k < (a2 < a1 ? a2 : a1); ++k) {
                        LOOP3_ASSERT(i, j, k, VALUE == X[k]);
                    }
                  } END_BDEMA_EXCEPTION_TEST
                }
            }
        }

        if (verbose) cout << "\nTesting 'setLength(int)'" << endl;
        {
            const int lengths[] = { 0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17 };
            const int NUM_TESTS = sizeof lengths / sizeof lengths[0];
            const Element I_VALUE       = 'Q';
            const Element DEFAULT_VALUE = 0x00; // As specified in component
            for (int i = 0; i < NUM_TESTS; ++i) {
                const int a1 = lengths[i];
                if (verbose) { cout << "\t"; P(a1); }
                for (int j = 0; j < NUM_TESTS; ++j) {
                    const int a2 = lengths[j];
                    if (veryVerbose) { cout << "\t\t"; P(a2); }
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    Obj mX(a1, I_VALUE, &testAllocator);
                    const Obj &X = mX;
                    if (veryVerbose) P(X);
                    LOOP2_ASSERT(i, j, a1 == X.length());
                    mX.setLength(a2);
                    if (veryVerbose) P(X);
                    LOOP2_ASSERT(i, j, a2 == X.length());
                    for (int k = 0; k < a2; ++k) {
                        if (k < a1) {
                            LOOP3_ASSERT(i, j, k, I_VALUE == X[k]);
                        }
                        else {
                            LOOP3_ASSERT(i, j, k, DEFAULT_VALUE == X[k]);
                        }
                    }
                  } END_BDEMA_EXCEPTION_TEST
                }
            }
        }

        if (verbose) cout << "\nTesting 'setLength(int, char)'" << endl;
        {
            const int lengths[] = { 0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17 };
            const int NUM_TESTS = sizeof lengths / sizeof lengths[0];
            const Element I_VALUE = 'Q';
            const Element F_VALUE = 'R';
            for (int i = 0; i < NUM_TESTS; ++i) {
                const int a1 = lengths[i];
                if (verbose) { cout << "\t"; P(a1); }
                for (int j = 0; j < NUM_TESTS; ++j) {
                    const int a2 = lengths[j];
                    if (veryVerbose) { cout << "\t\t"; P(a2); }
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    Obj mX(a1, I_VALUE, &testAllocator);
                    const Obj &X = mX;
                    if (veryVerbose) P(X);
                    LOOP2_ASSERT(i, j, a1 == X.length());
                    mX.setLength(a2, F_VALUE);
                    if (veryVerbose) P(X);
                    LOOP2_ASSERT(i, j, a2 == X.length());
                    for (int k = 0; k < a2; ++k) {
                        if (k < a1) {
                            LOOP3_ASSERT(i, j, k, I_VALUE == X[k]);
                        }
                        else {
                            LOOP3_ASSERT(i, j, k, F_VALUE == X[k]);
                        }
                    }
                  } END_BDEMA_EXCEPTION_TEST
                }
            }
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING INITIAL-LENGTH CONSTRUCTORS:
        //   We have the following concerns:
        //    1. The initial value is correct.
        //    2. The constructor is exception neutral w.r.t. memory allocation.
        //    3. The internal memory management system is hooked up properly
        //       so that *all* internally allocated memory draws from a
        //       user-supplied allocator whenever one is specified.
        //
        // Plan:
        //   For each constructor we will create objects
        //    - With and without passing in an allocator.
        //    - In the presence of exceptions during memory allocations using
        //        a 'bdema_TestAllocator' and varying its *allocation* *limit*.
        //    - Where the object is constructed entirely in static memory
        //        (using a 'bdema_BufferAllocator') and never destroyed.
        //   and use basic accessors to verify
        //      - length
        //      - element value at each index position { 0 .. length - 1 }.
        //
        // Testing:
        //   bdea_CharArray(const Explicit& iLen, *ba = 0);
        //   bdea_CharArray(int iLen, char iVal, *ba = 0);
        //   bdea_CharArray(const char *sa, int ne, *ba = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Initial-Length Constructor" << endl
                          << "==================================" << endl;

        if (verbose) cout << "\nTesting initial-length ctor with unspecified "
                             "(default) initial value." << endl;

        if (verbose) cout << "\tWithout passing in an allocator." << endl;
        {
            const Element DEFAULT_VALUE = 0x00;
            const int MAX_LENGTH = 10;
            for (int length = 0; length <= MAX_LENGTH; ++length) {
                if (verbose) P(length);
                Obj mX(length);  const Obj &X = mX;
                if (veryVerbose) P(X);
                LOOP_ASSERT(length, length == X.length());
                for (int i = 0; i < length; ++i) {
                    LOOP2_ASSERT(length, i, DEFAULT_VALUE == X[i]);
                }
            }
        }

        if (verbose) cout << "\tPassing in an allocator." << endl;

        if (verbose) cout << "\t\tWith no exceptions." << endl;
        {
            const Element DEFAULT_VALUE = 0x00;
            const int MAX_LENGTH = 10;
            for (int length = 0; length <= MAX_LENGTH; ++length) {
                if (verbose) P(length);
                Obj mX(length, &testAllocator);  const Obj &X = mX;
                if (veryVerbose) P(X);
                LOOP_ASSERT(length, length == X.length());
                for (int i = 0; i < length; ++i) {
                    LOOP2_ASSERT(length, i, DEFAULT_VALUE == X[i]);
                }
            }
        }

        if (verbose) cout << "\t\tWith exceptions." << endl;
        {
            const Element DEFAULT_VALUE = 0x00;
            const int MAX_LENGTH = 10;
            for (int length = 0; length <= MAX_LENGTH; ++length) {
              BEGIN_BDEMA_EXCEPTION_TEST {
                if (verbose) P(length);
                Obj mX(length, &testAllocator);  const Obj &X = mX;
                if (veryVerbose) P(X);
                LOOP_ASSERT(length, length == X.length());
                for (int i = 0; i < length; ++i) {
                    LOOP2_ASSERT(length, i, DEFAULT_VALUE == X[i]);
                }
              } END_BDEMA_EXCEPTION_TEST
            }
        }

        if (verbose) cout << "\tIn place using a buffer allocator." << endl;
        {
            char memory[1024];
            bdema_BufferAllocator a(memory, sizeof memory);
            const Element DEFAULT_VALUE = 0x00;
            const int MAX_LENGTH = 10;
            for (int length = 0; length <= MAX_LENGTH; ++length) {
                if (verbose) P(length);
                Obj::Explicit e(length);
                Obj *doNotDelete = new(a.allocate(sizeof(Obj))) Obj(e, &a);
                Obj &mX = *doNotDelete;  const Obj &X = mX;
                if (veryVerbose) P(X);
                LOOP_ASSERT(length, length == X.length());
                for (int i = 0; i < length; ++i) {
                    LOOP2_ASSERT(length, i, DEFAULT_VALUE == X[i]);
                }
            }

             // No destructor is called; will produce memory leak in purify
             // if internal allocators are not hooked up properly.
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout <<
            "\nTesting initial-length ctor with user-specified initial value."
                                                                       << endl;
        if (verbose) cout << "\tWithout passing in an allocator." << endl;
        {
            const Element VALUE = 'G';  // ADJUST
            const int MAX_LENGTH = 10;
            for (int length = 0; length <= MAX_LENGTH; ++length) {
                if (verbose) P(length);
                Obj mX(length, VALUE);  const Obj &X = mX;
                if (veryVerbose) P(X);
                LOOP_ASSERT(length, length == X.length());
                for (int i = 0; i < length; ++i) {
                    LOOP2_ASSERT(length, i, VALUE == X[i]);
                }
            }
        }

        if (verbose) cout << "\tPassing in an allocator." << endl;

        if (verbose) cout << "\t\tWith no exceptions." << endl;
        {
            const Element VALUE = 'H';  // ADJUST
            const int MAX_LENGTH = 10;
            for (int length = 0; length <= MAX_LENGTH; ++length) {
                if (verbose) P(length);
                Obj mX(length, VALUE, &testAllocator);  const Obj &X = mX;
                if (veryVerbose) P(X);
                LOOP_ASSERT(length, length == X.length());
                for (int i = 0; i < length; ++i) {
                    LOOP2_ASSERT(length, i, VALUE == X[i]);
                }
            }
        }

        if (verbose) cout << "\t\tWith exceptions." << endl;
        {
            const Element VALUE = 'I';  // ADJUST
            const int MAX_LENGTH = 10;
            for (int length = 0; length <= MAX_LENGTH; ++length) {
              BEGIN_BDEMA_EXCEPTION_TEST {
                if (verbose) P(length);
                Obj mX(length, VALUE, &testAllocator);  const Obj &X = mX;
                if (veryVerbose) P(X);
                LOOP_ASSERT(length, length == X.length());
                for (int i = 0; i < length; ++i) {
                    LOOP2_ASSERT(length, i, VALUE == X[i]);
                }
              } END_BDEMA_EXCEPTION_TEST
            }
        }

        if (verbose) cout << "\tIn place using a buffer allocator." << endl;
        {
            char memory[1024];
            bdema_BufferAllocator a(memory, sizeof memory);
            const Element VALUE = 'J';  // ADJUST
            const int MAX_LENGTH = 10;
            for (int length = 0; length <= MAX_LENGTH; ++length) {
                if (verbose) P(length);
                Obj *doNotDelete =
                    new(a.allocate(sizeof(Obj))) Obj(length, VALUE, &a);
                Obj &mX = *doNotDelete;  const Obj &X = mX;
                if (veryVerbose) P(X);
                LOOP_ASSERT(length, length == X.length());
                for (int i = 0; i < length; ++i) {
                    LOOP2_ASSERT(length, i, VALUE == X[i]);
                }
            }
            // No destructor is called; will produce memory leak in purify
            // if internal allocators are not hooked up properly.
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout <<
                     "\nTesting ctor from primitive C-style array." << endl;

        {
            const Element DA[] =
                { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J' }; // ADJUST
            const int NUM_ELEMENTS = sizeof DA / sizeof *DA;

            if (verbose) cout << "\tWithout passing in an allocator." << endl;
            {
                for (int ne = 0; ne <= NUM_ELEMENTS; ++ne) {
                    if (verbose) P(ne);
                    Obj mX(DA, ne);  const Obj &X = mX;
                    if (veryVerbose) P(X);
                    LOOP_ASSERT(ne, ne == X.length());
                    for (int i = 0; i < ne; ++i) {
                        LOOP2_ASSERT(ne, i, DA[i] == X[i]);
                    }
                }
            }

            if (verbose) cout << "\tPassing in an allocator." << endl;

            if (verbose) cout << "\t\tWith no exceptions." << endl;
            {
                for (int ne = 0; ne <= NUM_ELEMENTS; ++ne) {
                    if (verbose) P(ne);
                    Obj mX(DA, ne, &testAllocator);  const Obj &X = mX;
                    if (veryVerbose) P(X);
                    LOOP_ASSERT(ne, ne == X.length());
                    for (int i = 0; i < ne; ++i) {
                        LOOP2_ASSERT(ne, i, DA[i] == X[i]);
                    }
                }
            }

            if (verbose) cout << "\t\tWith exceptions." << endl;
            {
                for (int ne = 0; ne <= NUM_ELEMENTS; ++ne) {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    if (verbose) P(ne);
                    Obj mX(DA, ne, &testAllocator);  const Obj &X = mX;
                    if (veryVerbose) P(X);
                    LOOP_ASSERT(ne, ne == X.length());
                    for (int i = 0; i < ne; ++i) {
                        LOOP2_ASSERT(ne, i, DA[i] == X[i]);
                    }
                  } END_BDEMA_EXCEPTION_TEST
                }
            }

            if (verbose) cout << "\tIn place using a buffer allocator." <<endl;
            {
                char memory[1024];
                bdema_BufferAllocator a(memory, sizeof memory);
                for (int ne = 0; ne <= NUM_ELEMENTS; ++ne) {
                    if (verbose) P(ne);
                    Obj *doNotDelete =
                        new(a.allocate(sizeof(Obj))) Obj(DA, ne, &a);
                    Obj &mX = *doNotDelete;  const Obj &X = mX;
                    if (veryVerbose) P(X);
                    LOOP_ASSERT(ne, ne == X.length());
                    for (int i = 0; i < ne; ++i) {
                        LOOP2_ASSERT(ne, i, DA[i] == X[i]);
                    }
                }
                // No destructor is called; will produce memory leak in purify
                // if internal allocators are not hooked up properly.
            }
        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING STREAMING FUNCTIONALITY:
        //   1. The (free) streaming operators '<<' and '>>' are implemented
        //      using the respective member functions 'streamOut' and
        //      'streamIn'.
        //   2. Streaming must be neutral to exceptions thrown as a result of
        //      either allocating memory or streaming in values.
        //   3. Ensure that streaming works under the following conditions:
        //       VALID - may contain any sequence of valid values.
        //       EMPTY - valid, but contains no data.
        //       INVALID - may or may not be empty.
        //       INCOMPLETE - the stream is truncated, but otherwise valid.
        //       CORRUPTED - the data contains explicitly inconsistent fields.
        //
        //   Plan:
        //     First perform a trivial direct (breathing) test of the
        //     'outStream' and 'inStream' methods (to address concern 1).  Note
        //     that the rest of the testing will use the stream operators.
        //
        //     Next, specify a set S of unique object values with substantial
        //     and varied differences, ordered by increasing length.  For each
        //     value in S, construct an object x along with a sequence of
        //     similarly constructed duplicates x1, x2, ..., xN.  Attempt to
        //     affect every aspect of white-box state by altering each xi in
        //     a unique way.  Let the union of all such objects be the set T.
        //
        //   VALID STREAMS (and exceptions)
        //      Using all combinations of (u, v) in T X T, stream-out the value
        //      of u into a buffer and stream it back into (an independent
        //      instance of) v, and assert that u == v.
        //
        //   EMPTY AND INVALID STREAMS
        //     For each x in T, attempt to stream into (a temporary copy of) x
        //     from an empty and then invalid stream.  Verify after each try
        //     that the object is unchanged and that the stream is invalid.
        //
        //   INCOMPLETE (BUT OTHERWISE VALID) DATA
        //     Write 3 distinct objects to an output stream buffer of total
        //     length N.  For each partial stream length from 0 to N - 1,
        //     construct a truncated input stream and attempt to read into
        //     objects initialized with distinct values.  Verify values of
        //     objects that are either successfully modified or left entirely
        //     unmodified,  and that the stream became invalid immediately
        //     after the first incomplete read.  Finally ensure that each
        //     object streamed into is in some valid state by creating a copy
        //     and then assigning a known value to that copy; allow the
        //     original instance to leave scope without further modification,
        //     so that the destructor asserts internal object invariants
        //     appropriately.
        //
        //   CORRUPTED DATA
        //     We will assume that the incomplete test fail every field,
        //     including a char (multi-byte representation) hence we need
        //     only to produce values that are inconsistent with a valid
        //     value and verify that they are detected.  Use the underlying
        //     stream package to simulate an instance of a typical valid
        //     (control) stream and verify that it can be streamed in
        //     successfully.  Then for each data field in the stream (beginning
        //     with the version number), provide one or more similar tests with
        //     that data field corrupted.  After each test, verify that the
        //     object is in some valid state after streaming, and that the
        //     input stream has gone invalid.
        //
        //   Finally, tests of the explicit wire format will be performed.
        //
        // Testing:
        //   int maxSupportedBdexVersion() const;
        //   bdex_InStream& streamIn(bdex_InStream& stream);
        //   bdex_InStream& streamIn(bdex_InStream& stream, int version);
        //   bdex_OutStream& streamOut(bdex_OutStream& stream) const;
        //   bdex_OutStream& streamOut(bdex_OutStream& stream, int) const;
        //   operator>>(bdex_InStream&, bdea_Array<char>&);
        //   operator<<(bdex_OutStream&, const bdea_Array<char>&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Streaming Functionality" << endl
                          << "===============================" << endl;

        if (verbose) cout << "\nDirect initial trial of 'streamOut' and"
                             " (valid) 'streamIn' functionality." << endl;
        {
            const Obj X(g("ABC"), &testAllocator);
            if (veryVerbose) { cout << "\t   Value being streamed: "; P(X); }

            bdex_TestOutStream out;  out << X;

            const char *const OD  = out.data();
            const int         LOD = out.length();

            bdex_TestInStream in(OD, LOD);  ASSERT(in);  ASSERT(!in.isEmpty());

            Obj t(g("DE"), &testAllocator);

            if (veryVerbose) { cout << "\tValue being overwritten: "; P(t); }
            ASSERT(X != t);

            in >> t;                         ASSERT(in);  ASSERT(in.isEmpty());

            if (veryVerbose) { cout << "\t  Value after overwrite: "; P(t); }
            ASSERT(X == t);
        }

        if (verbose) cout <<
            "\nTesting stream operators ('<<' and '>>')." << endl;

        if (verbose) cout << "\tOn valid, non-empty stream data "
                          << "(with allocation exceptions)." << endl;
        {
            static const char *SPECS[] = { // len: 0 - 5,
                "",       "A",      "BC",     "CDE",    "DEAB",   "EABCD",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5,
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int uOldLen = -1;
            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const int uLen = (int)strlen(U_SPEC);

                if (verbose) {
                    cout << "\t\tFor source objects of length "
                                                        << uLen << ":\t";
                    P(U_SPEC);
                }

                LOOP_ASSERT(U_SPEC, uOldLen < uLen);  // strictly increasing
                uOldLen = uLen;

                const Obj UU = g(U_SPEC);               // control
                LOOP_ASSERT(ui, uLen == UU.length());   // same lengths

                for (int uj = 0; uj < NUM_EXTEND; ++uj) {
                    const int U_N = EXTEND[uj];

                    Obj mU(&testAllocator);     stretchRemoveAll(&mU, U_N);
                    const Obj& U = mU;          gg(&mU, U_SPEC);

                    bdex_TestOutStream out;

                    out << U;   // testing stream-out operator here

                    const char *const OD  = out.data();
                    const int         LOD = out.length();

                    // Must reset stream for each iteration of inner loop.
                    bdex_TestInStream testInStream(OD, LOD);
                    LOOP2_ASSERT(U_SPEC, U_N, testInStream);
                    LOOP2_ASSERT(U_SPEC, U_N, !testInStream.isEmpty());

                    for (int vi = 0; SPECS[vi]; ++vi) {
                        const char *const V_SPEC = SPECS[vi];
                        const int vLen = (int)strlen(V_SPEC);

                        const Obj VV = g(V_SPEC);               // control

                        if (0 == uj && veryVerbose || veryVeryVerbose) {
                            cout << "\t\t\tFor destination objects of length "
                                                        << vLen << ":\t";
                            P(V_SPEC);
                        }

                        const int Z = ui == vi; // flag indicating same values

                        for (int vj = 0; vj < NUM_EXTEND; ++vj) {
                          BEGIN_BDEMA_EXCEPTION_TEST {
                            testInStream.reset();
                            const int AL = testAllocator.allocationLimit();
                            testAllocator.setAllocationLimit(-1);
                            const int V_N = EXTEND[vj];
            //--------------^
            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, testInStream);
            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, !testInStream.isEmpty());
            //--------------v
                            Obj mV(&testAllocator); stretchRemoveAll(&mV, V_N);
                            const Obj& V = mV;      gg(&mV, V_SPEC);

                            static int firstFew = 2 * NUM_EXTEND * NUM_EXTEND;
                            if (veryVeryVerbose||veryVerbose && firstFew > 0) {
                                cout << "\t |"; P_(U_N); P_(V_N); P_(U); P(V);
                                --firstFew;
                            }

                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, UU == U);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, Z==(U==V));

                            testAllocator.setAllocationLimit(AL);
                            testInStream >> mV; // test stream-in operator here

                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, UU == U);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, UU == V);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N,  U == V);

                          } END_BDEMA_EXCEPTION_TEST
                        }
                    }
                }
            }
        }

        if (verbose) cout << "\tOn valid, non-empty stream data "
                          << "(with streaming exceptions)." << endl;
        {
            static const char *SPECS[] = { // len: 0 - 5,
                "",       "A",      "BC",     "CDE",    "DEAB",   "EABCD",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5,
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int uOldLen = -1;
            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const int uLen = (int)strlen(U_SPEC);

                if (verbose) {
                    cout << "\t\tFor source objects of length "
                                                        << uLen << ":\t";
                    P(U_SPEC);
                }

                LOOP_ASSERT(U_SPEC, uOldLen < uLen);  // strictly increasing
                uOldLen = uLen;

                const Obj UU = g(U_SPEC);               // control
                LOOP_ASSERT(ui, uLen == UU.length());   // same lengths

                for (int uj = 0; uj < NUM_EXTEND; ++uj) {
                    const int U_N = EXTEND[uj];

                    Obj mU(&testAllocator);     stretchRemoveAll(&mU, U_N);
                    const Obj& U = mU;          gg(&mU, U_SPEC);

                    bdex_TestOutStream out;

                    out << U;   // testing stream-out operator here

                    const char *const OD  = out.data();
                    const int         LOD = out.length();

                    // Must reset stream for each iteration of inner loop.
                    bdex_TestInStream testInStream(OD, LOD);
                    LOOP2_ASSERT(U_SPEC, U_N, testInStream);
                    LOOP2_ASSERT(U_SPEC, U_N, !testInStream.isEmpty());

                    for (int vi = 0; SPECS[vi]; ++vi) {
                        const char *const V_SPEC = SPECS[vi];
                        const int vLen = (int)strlen(V_SPEC);

                        const Obj VV = g(V_SPEC);               // control

                        if (0 == uj && veryVerbose || veryVeryVerbose) {
                            cout << "\t\t\tFor destination objects of length "
                                                        << vLen << ":\t";
                            P(V_SPEC);
                        }

                        const int Z = ui == vi; // flag indicating same values

                        for (int vj = 0; vj < NUM_EXTEND; ++vj) {
                          BEGIN_BDEX_EXCEPTION_TEST {
                            testInStream.reset();
                            const int V_N = EXTEND[vj];
            //--------------^
            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, testInStream);
            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, !testInStream.isEmpty());
            //--------------v
                            Obj mV(&testAllocator); stretchRemoveAll(&mV, V_N);
                            const Obj& V = mV;      gg(&mV, V_SPEC);

                            static int firstFew = 2 * NUM_EXTEND * NUM_EXTEND;
                            if (veryVeryVerbose||veryVerbose && firstFew > 0) {
                                cout << "\t |"; P_(U_N); P_(V_N); P_(U); P(V);
                                --firstFew;
                            }

                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, UU == U);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, Z==(U==V));

                            testInStream >> mV; // test stream-in operator here

                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, UU == U);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, UU == V);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N,  U == V);

                          } END_BDEX_EXCEPTION_TEST
                        }
                    }
                }
            }
        }

        if (verbose) cout << "\tOn empty and invalid streams "
                          << "(with allocation exceptions)." << endl;
        {
            static const char *SPECS[] = { // len: 0 - 5, 7, 8, 9, 15, 16, 17
                "",      "A",      "BC",     "CDE",    "DEAB",   "EABCD",
                "ABCDEAB",         "ABCDEABC",         "ABCDEABCD",
                "ABCDEABCDEABCDE", "ABCDEABCDEABCDEA", "ABCDEABCDEABCDEAB",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            bdex_TestInStream testInStream("", 0);

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];
                const int curLen = (int)strlen(SPEC);

                if (verbose) cout << "\t\tFor objects of length "
                                                    << curLen << '.' << endl;
                LOOP_ASSERT(SPEC, oldLen < curLen); // strictly increasing
                oldLen = curLen;

                if (veryVerbose) { cout << "\t\t\t"; P(SPEC); }

                // Create control object X.

                Obj mX(&testAllocator); gg(&mX, SPEC); const Obj& X = mX;
                LOOP_ASSERT(ti, curLen == X.length());  // same lengths

                for (int ei = 0; ei < NUM_EXTEND; ++ei) {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    testInStream.reset();

                    const int N = EXTEND[ei];

                    if (veryVerbose) { cout << "\t\t\t\t"; P(N); }

                    Obj t(&testAllocator);      gg(&t, SPEC);
                    stretchRemoveAll(&t, N);    gg(&t, SPEC);

                  // Ensure that reading from an empty or invalid input stream
                  // leaves the stream invalid and the target object unchanged.

                                        LOOP2_ASSERT(ti, ei, testInStream);
                                        LOOP2_ASSERT(ti, ei, X == t);

                    testInStream >> t;  LOOP2_ASSERT(ti, ei, !testInStream);
                                        LOOP2_ASSERT(ti, ei, X == t);

                    testInStream >> t;  LOOP2_ASSERT(ti, ei, !testInStream);
                                        LOOP2_ASSERT(ti, ei, X == t);

                  } END_BDEMA_EXCEPTION_TEST
                }
            }
        }

        if (verbose) cout << "\tOn empty and invalid streams "
                          << "(with streaming exceptions)." << endl;
        {
            static const char *SPECS[] = { // len: 0 - 5, 7, 8, 9, 15, 16, 17
                "",      "A",      "BC",     "CDE",    "DEAB",   "EABCD",
                "ABCDEAB",         "ABCDEABC",         "ABCDEABCD",
                "ABCDEABCDEABCDE", "ABCDEABCDEABCDEA", "ABCDEABCDEABCDEAB",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            bdex_TestInStream testInStream("", 0);

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];
                const int curLen = (int)strlen(SPEC);

                if (verbose) cout << "\t\tFor objects of length "
                                                    << curLen << '.' << endl;
                LOOP_ASSERT(SPEC, oldLen < curLen); // strictly increasing
                oldLen = curLen;

                if (veryVerbose) { cout << "\t\t\t"; P(SPEC); }

                // Create control object X.

                Obj mX(&testAllocator); gg(&mX, SPEC); const Obj& X = mX;
                LOOP_ASSERT(ti, curLen == X.length());  // same lengths

                for (int ei = 0; ei < NUM_EXTEND; ++ei) {
                  BEGIN_BDEX_EXCEPTION_TEST {
                    testInStream.reset();

                    const int N = EXTEND[ei];

                    if (veryVerbose) { cout << "\t\t\t\t"; P(N); }

                    Obj t(&testAllocator);      gg(&t, SPEC);
                    stretchRemoveAll(&t, N);    gg(&t, SPEC);

                  // Ensure that reading from an empty or invalid input stream
                  // leaves the stream invalid and the target object unchanged.

                                        LOOP2_ASSERT(ti, ei, testInStream);
                                        LOOP2_ASSERT(ti, ei, X == t);

                    testInStream >> t;  LOOP2_ASSERT(ti, ei, !testInStream);
                                        LOOP2_ASSERT(ti, ei, X == t);

                    testInStream >> t;  LOOP2_ASSERT(ti, ei, !testInStream);
                                        LOOP2_ASSERT(ti, ei, X == t);

                  } END_BDEX_EXCEPTION_TEST
                }
            }
        }

        if (verbose) cout <<
            "\tOn incomplete (but otherwise valid) data." << endl;
        {
            const Obj X1 = g("CDCD"),   Y1 = g("BB"),    Z1 = g("ABCDE");
            const Obj X2 = g("ADE"),    Y2 = g("CABDE"), Z2 = g("B");
            const Obj X3 = g("DEEDDE"), Y3 = g("C"),     Z3 = g("DBED");

            bdex_TestOutStream out;
            out << Y1;  const int LOD1 = out.length();
            out << Y2;  const int LOD2 = out.length();
            out << Y3;  const int LOD  = out.length();
            const char *const OD = out.data();

            for (int i = 0; i < LOD; ++i) {
                bdex_TestInStream testInStream(OD, i);
                bdex_TestInStream& in = testInStream;
                LOOP_ASSERT(i, in);  LOOP_ASSERT(i, !i == in.isEmpty());

                if (veryVerbose) { cout << "\t\t"; P(i); }

                Obj t1(X1, &testAllocator),
                    t2(X2, &testAllocator),
                    t3(X3, &testAllocator);

                if (i < LOD1) {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                  BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    LOOP_ASSERT(i, in);  LOOP_ASSERT(i, !i == in.isEmpty());
                    t1 = X1;
                    t2 = X2;
                    t3 = X3;

                    testAllocator.setAllocationLimit(AL);
                    in >> t1; LOOP_ASSERT(i, !in);
                                        if (0 == i) LOOP_ASSERT(i, X1 == t1);
                    in >> t2; LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, X2 == t2);
                    in >> t3; LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, X3 == t3);
                  } END_BDEX_EXCEPTION_TEST
                  } END_BDEMA_EXCEPTION_TEST
                }
                else if (i < LOD2) {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                  BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    LOOP_ASSERT(i, in);  LOOP_ASSERT(i, !i == in.isEmpty());
                    t1 = X1;
                    t2 = X2;
                    t3 = X3;

                    testAllocator.setAllocationLimit(AL);
                    in >> t1; LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, Y1 == t1);
                    in >> t2; LOOP_ASSERT(i, !in);
                                     if (LOD1 == i) LOOP_ASSERT(i, X2 == t2);
                    in >> t3; LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, X3 == t3);
                  } END_BDEX_EXCEPTION_TEST
                  } END_BDEMA_EXCEPTION_TEST
                }
                else {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                  BEGIN_BDEX_EXCEPTION_TEST {
                    in.reset();
                    testAllocator.setAllocationLimit(-1);
                    LOOP_ASSERT(i, in);  LOOP_ASSERT(i, !i == in.isEmpty());
                    t1 = X1;
                    t2 = X2;
                    t3 = X3;

                    in >> t1; LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, Y1 == t1);
                    in >> t2; LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, Y2 == t2);
                    in >> t3; LOOP_ASSERT(i, !in);
                                     if (LOD2 == i) LOOP_ASSERT(i, X3 == t3);
                  } END_BDEX_EXCEPTION_TEST
                  } END_BDEMA_EXCEPTION_TEST
                }

                Obj w1(t1), w2(t2), w3(t3);  // make copies to be sure we can

                                LOOP_ASSERT(i, Z1 != w1);
                w1 = Z1;        LOOP_ASSERT(i, Z1 == w1);

                                LOOP_ASSERT(i, Z2 != w2);
                w2 = Z2;        LOOP_ASSERT(i, Z2 == w2);

                                LOOP_ASSERT(i, Z3 != w3);
                w3 = Z3;        LOOP_ASSERT(i, Z3 == w3);
            }
        }

        if (verbose) cout << "\tOn corrupted data." << endl;

        const Obj W = g("");            // default value
        const Obj X = g("ABCDE");       // original value
        const Obj Y = g("DCB");         // new value

        enum { A_LEN = 3 };
        Element a[A_LEN];  a[0] = VD;  a[1] = VC;  a[2] = VB;

        if (verbose) cout << "\t\tGood stream (for control)." << endl;
        {
            const char version = 1;
            const int length   = 3;

            bdex_TestOutStream out;
            out.putVersion(version);
            out.putLength(length);
            out.putArrayInt8(a, A_LEN);
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);   ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
            bdex_TestInStream in(OD, LOD); ASSERT(in); in >> t; ASSERT(in);
                        ASSERT(W != t);    ASSERT(X != t);      ASSERT(Y == t);
        }

        if (verbose) cout << "\t\tBad version." << endl;
        {
            const char version = 0; // too small
            const int length   = 3;

            bdex_TestOutStream out;
            out.putVersion(version);
            out.putLength(length);
            out.putArrayUint8(a, A_LEN);
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);   ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
            bdex_TestInStream in(OD, LOD);
            in.setSuppressVersionCheck(1);
            ASSERT(in); in >> t; ASSERT(!in);
            ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
        }
        {
            const char version = 2; // too large
            const int length   = 3;

            bdex_TestOutStream out;
            out.putVersion(version);
            out.putLength(length);
            out.putArrayUint8(a, A_LEN);
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);   ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
            bdex_TestInStream in(OD, LOD); ASSERT(in); in >> t; ASSERT(!in);
                        ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
        }

        if (verbose) cout << "\nTesting 'maxSupportedBdexVersion()'." << endl;
        {
            if (verbose) cout << "\tusing object syntax:" << endl;
            const Obj X;
            ASSERT(1 == X.maxSupportedBdexVersion());
            if (verbose) cout << "\tusing class method syntax:" << endl;
            ASSERT(1 == Obj::maxSupportedBdexVersion());
        }

        if (verbose) cout << "\nWire format direct tests." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_version;  // version to stream with
                int         d_length;   // expect output length
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                //line  spec           ver  len  format
                //----  -------------  ---  ---  --------------------------
                { L_,   "",              0,   0, ""                         },
                { L_,   "A",             0,   0, ""                         },

                { L_,   "",              1,   1, "\x00"                     },
                { L_,   "A",             1,   2, "\x01\x61"                 },
                { L_,   "AB",            1,   3, "\x02\x61\x62"             },
                { L_,   "ABC",           1,   4, "\x03\x61\x62\x63"         },
                { L_,   "ABCD",          1,   5, "\x04\x61\x62\x63\x64"     },
                { L_,   "ABCDE",         1,   6, "\x05\x61\x62\x63\x64\x65" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE         = DATA[i].d_lineNum;
                const char *const SPEC = DATA[i].d_spec_p;
                const int VERSION      = DATA[i].d_version;
                const int LEN          = DATA[i].d_length;
                const char *const FMT  = DATA[i].d_fmt_p;

                Obj mX(&testAllocator);
                mX = g(SPEC);
                const Obj& X = mX;
                bdex_ByteOutStream out;  X.bdexStreamOut(out, VERSION);

                LOOP_ASSERT(LINE, LEN == out.length());
                LOOP_ASSERT(LINE, 0 == memcmp(out.data(),
                                              FMT,
                                              LEN));

                if (verbose && LEN != out.length()) {
                    P_(LINE);
                    P(out.length());
                }
                if (verbose && memcmp(out.data(), FMT, LEN)) {
                    const char *hex = "0123456789abcdef";
                    P_(LINE);
                    for (int j = 0; j < out.length(); ++j) {
                        cout << "\\x"
                             << hex[(unsigned char)*
                                          (out.data() + j) >> 4]
                             << hex[(unsigned char)*
                                       (out.data() + j) & 0x0f];
                    }
                    cout << endl;
                }

                Obj mY(&testAllocator);  const Obj& Y = mY;
                if (LEN) { // version is supported
                    bdex_ByteInStream in(out.data(),
                                         out.length());
                    mY.bdexStreamIn(in, VERSION);
                }
                else { // version is not supported
                    mY = X;
                    bdex_ByteInStream in;
                    mY.bdexStreamIn(in, VERSION);
                    LOOP_ASSERT(LINE, !in);
                }
                LOOP_ASSERT(LINE, X == Y);
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR:
        // We have the following concerns:
        //   1.  The value represented by any instance can be assigned to any
        //         other instance regardless of how either value is represented
        //         internally.
        //   2.  The 'rhs' value must not be affected by the operation.
        //   3.  'rhs' going out of scope has no effect on the value of 'lhs'
        //       after the assignment.
        //   4.  Aliasing (x = x): The assignment operator must always work --
        //         even when the lhs and rhs are identically the same object.
        //   5.  The assignment operator must be neutral with respect to memory
        //       allocation exceptions.
        // Plan:
        //   Specify a set S of unique object values with substantial and
        //   varied differences, ordered by increasing length.  For each value
        //   in S, construct an object x along with a sequence of similarly
        //   constructed duplicates x1, x2, ..., xN.  Attempt to affect every
        //   aspect of white-box state by altering each xi in a unique way.
        //   Let the union of all such objects be the set T.
        //
        //   To address concerns 1, 2, and 5, construct tests u = v for all
        //   (u, v) in T X T.  Using canonical controls UU and VV, assert
        //   before the assignment that UU == u, VV == v, and v == u iff
        //   VV == UU.  After the assignment, assert that VV == u, VV == v,
        //   and, for grins, that v == u.  Let v go out of scope and confirm
        //   that VV == u.  All of these tests are performed within the 'bdema'
        //   exception testing apparatus.
        //
        //   As a separate exercise, we address 4 and 5 by constructing tests
        //   y = y for all y in T.  Using a canonical control X, we will verify
        //   that X == y before and after the assignment, again within
        //   the bdema exception testing apparatus.
        //
        // Testing:
        //   bdea_Array<char>& operator=(const bdea_Array<char>& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Assignment Operator" << endl
                          << "===========================" << endl;

        if (verbose) cout <<
            "\nAssign cross product of values with varied representations."
                                                                       << endl;
        {
            static const char *SPECS[] = { // len: 0 - 5, 7, 8, 9,
                "",        "A",    "BC",     "CDE",    "DEAB",   "EABCD",
                "AEDCBAE",         "CBAEDCBA",         "EDCBAEDCB",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 7, 8, 9,
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int uOldLen = -1;
            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const int uLen = (int)strlen(U_SPEC);

                if (verbose) {
                    cout << "\tFor lhs objects of length " << uLen << ":\t";
                    P(U_SPEC);
                }

                LOOP_ASSERT(U_SPEC, uOldLen < uLen);  // strictly increasing
                uOldLen = uLen;

                const Obj UU = g(U_SPEC);               // control
                LOOP_ASSERT(ui, uLen == UU.length());   // same lengths

                // int vOldLen = -1;
                for (int vi = 0; SPECS[vi]; ++vi) {
                    const char *const V_SPEC = SPECS[vi];
                    const int vLen = (int)strlen(V_SPEC);

                    if (veryVerbose) {
                        cout << "\t\tFor rhs objects of length " << vLen
                                                                 << ":\t";
                        P(V_SPEC);
                    }

                    const Obj VV = g(V_SPEC);           // control

                    const int Z = ui == vi; // flag indicating same values

                    for (int uj = 0; uj < NUM_EXTEND; ++uj) {
                        const int U_N = EXTEND[uj];
                        for (int vj = 0; vj < NUM_EXTEND; ++vj) {
                            const int V_N = EXTEND[vj];

                          BEGIN_BDEMA_EXCEPTION_TEST {
                            const int AL = testAllocator.allocationLimit();
                            testAllocator.setAllocationLimit(-1);

                            Obj mU(&testAllocator); stretchRemoveAll(&mU, U_N);
                            const Obj& U = mU; gg(&mU, U_SPEC);
                            {
                            //--^
                            Obj mV(&testAllocator); stretchRemoveAll(&mV, V_N);
                            const Obj& V = mV; gg(&mV, V_SPEC);

                            static int firstFew = 2 * NUM_EXTEND * NUM_EXTEND;
                            if (veryVeryVerbose||veryVerbose && firstFew > 0) {
                                cout << "\t| "; P_(U_N); P_(V_N); P_(U); P(V);
                                --firstFew;
                            }

                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, UU == U);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, Z==(V==U));

                            testAllocator.setAllocationLimit(AL);
                            mU = V; // test assignment here

                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N,  V == U);
                            //--v
                            }
                            // 'mV' (and therefore 'V') now out of scope
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                          } END_BDEMA_EXCEPTION_TEST
                        }
                    }
                }
            }
        }

        if (verbose) cout << "\nTesting self assignment (Aliasing)." << endl;
        {
            static const char *SPECS[] = { // len: 0 - 5, 7, 8, 9, 15, 16, 17
                "",      "A",      "BC",     "CDE",    "DEAB",   "EABCD",
                "ABCDEAB",         "ABCDEABC",         "ABCDEABCD",
                "ABCDEABCDEABCDE", "ABCDEABCDEABCDEA", "ABCDEABCDEABCDEAB",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];
                const int curLen = (int)strlen(SPEC);

                if (verbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(SPEC);
                }
                LOOP_ASSERT(SPEC, oldLen < curLen);  // strictly increasing
                oldLen = curLen;

                const Obj X = g(SPEC);                  // control
                LOOP_ASSERT(ti, curLen == X.length());  // same lengths

                for (int tj = 0; tj < NUM_EXTEND; ++tj) {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);

                    const int N = EXTEND[tj];
                    Obj mY(&testAllocator);  stretchRemoveAll(&mY, N);
                    const Obj& Y = mY;       gg(&mY, SPEC);

                    if (veryVerbose) { cout << "\t\t"; P_(N); P(Y); }

                    LOOP2_ASSERT(SPEC, N, Y == Y);
                    LOOP2_ASSERT(SPEC, N, X == Y);

                    testAllocator.setAllocationLimit(AL);
                    mY = Y; // test assignment here

                    LOOP2_ASSERT(SPEC, N, Y == Y);
                    LOOP2_ASSERT(SPEC, N, X == Y);

                  } END_BDEMA_EXCEPTION_TEST
                }
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION, g:
        //   Since 'g' is implemented almost entirely using 'gg', we need to
        //   verify only that the arguments are properly forwarded, that 'g'
        //   does not affect the test allocator, and that 'g' returns an
        //   object by value.
        //
        // Plan:
        //   For each SPEC in a short list of specifications, compare the
        //   object returned (by value) from the generator function, 'g(SPEC)'
        //   with the value of a newly constructed OBJECT configured using
        //   'gg(&OBJECT, SPEC)'.  Compare the results of calling the
        //   allocator's 'numBlocksTotal' and 'numBytesInUse' methods before
        //   and after calling 'g' in order to demonstrate that 'g' has no
        //   effect on the test allocator.  Finally, use 'sizeof' to confirm
        //   that the (temporary) returned by 'g' differs in size from that
        //   returned by 'gg'.
        //
        // Testing:
        //   bdea_Array<char> g(const char *spec);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Generator Function 'g'" << endl
                          << "==============================" << endl;

        static const char *SPECS[] = {
            "", "~", "A", "B", "C", "D", "E", "A~B~C~D~E", "ABCDE", "ABC~DE",
        0}; // Null string required as last element.

        if (verbose) cout <<
            "\nCompare values produced by 'g' and 'gg' for various inputs."
                                                                       << endl;
        for (int ti = 0; SPECS[ti]; ++ti) {
            const char *spec = SPECS[ti];
            if (veryVerbose) { P_(ti);  P(spec); }
            Obj mX(&testAllocator);  gg(&mX, spec);  const Obj& X = mX;
            if (veryVerbose) {
                cout << "\t g = " << g(spec) << endl;
                cout << "\tgg = " << X       << endl;
            }
            const int TOTAL_BLOCKS_BEFORE = testAllocator.numBlocksTotal();
            const int IN_USE_BYTES_BEFORE = testAllocator.numBytesInUse();
            LOOP_ASSERT(ti, X == g(spec));
            const int TOTAL_BLOCKS_AFTER = testAllocator.numBlocksTotal();
            const int IN_USE_BYTES_AFTER = testAllocator.numBytesInUse();
            LOOP_ASSERT(ti, TOTAL_BLOCKS_BEFORE == TOTAL_BLOCKS_AFTER);
            LOOP_ASSERT(ti, IN_USE_BYTES_BEFORE == IN_USE_BYTES_AFTER);
        }

        if (verbose) cout << "\nConfirm return-by-value." << endl;
        {
            const char *spec = "ABCDE";

            ASSERT(sizeof(Obj) == sizeof g(spec));      // compile-time fact

            Obj x(&testAllocator);                      // runtime tests
            Obj& r1 = gg(&x, spec);
            Obj& r2 = gg(&x, spec);
            const Obj& r3 = g(spec);
            const Obj& r4 = g(spec);
            ASSERT(&r2 == &r1);
            ASSERT(&x  == &r1);
            ASSERT(&r4 != &r3);
            ASSERT(&x  != &r3);
        }

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR:
        //   We have the following concerns:
        //   1. The new object's value is the same as that of the original
        //       object (relying on the previously tested equality operators).
        //   2. All internal representations of a given value can be used to
        //        create a new object of equivalent value.
        //   3. The value of the original object is left unaffected.
        //   4. Subsequent changes in or destruction of the source object have
        //      no effect on the copy-constructed object.
        //   5. The function is exception neutral w.r.t. memory allocation.
        //   6. The object has its internal memory management system hooked up
        //         properly so that *all* internally allocated memory draws
        //         from a user-supplied allocator whenever one is specified.
        //
        // Plan:
        //   To address concerns 1 - 3, specify a set S of object values with
        //   substantial and varied differences, ordered by increasing length.
        //   For each value in S, initialize objects w and x, copy construct y
        //   from x and use 'operator==' to verify that both x and y
        //   subsequently have the same value as w.  Let x go out of scope and
        //   again verify that w == x.  Repeat this test with x having the same
        //   *logical* value, but perturbed so as to have potentially different
        //   internal representations.
        //
        //   To address concern 5, we will perform each of the above tests in
        //   the presence of exceptions during memory allocations using a
        //   'bdema_TestAllocator' and varying its *allocation* *limit*.
        //
        //   To address concern 6, we will repeat the above tests:
        //     - When passing in no allocator.
        //     - When passing in a null pointer: (bdema_Allocator *)0.
        //     - When passing in a test allocator (see concern 5).
        //     - Where the object is constructed entirely in static memory
        //       (using a 'bdema_BufferAllocator') and never destroyed.
        //     - After the (dynamically allocated) source object is
        //       deleted and its footprint erased (see concern 4).
        //
        // Testing:
        //   bdea_CharArray(const bdea_Array<char>& original, *ba = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Copy Constructor" << endl
                          << "========================" << endl;

        if (verbose) cout <<
            "\nCopy construct values with varied representations." << endl;
        {
            static const char *SPECS[] = { // len: 0 - 5, 7, 8, 9, 15, 16, 17
                "",      "A",      "BC",     "CDE",    "DEAB",   "EABCD",
                "ABCDEAB",         "ABCDEABC",         "ABCDEABCD",
                "ABCDEABCDEABCDE", "ABCDEABCDEABCDEA", "ABCDEABCDEABCDEAB",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];
                const int curLen = (int)strlen(SPEC);

                if (verbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(SPEC);
                }

                LOOP_ASSERT(SPEC, oldLen < curLen); // strictly increasing
                oldLen = curLen;

                // Create control object w.
                Obj mW(&testAllocator); gg(&mW, SPEC); const Obj& W = mW;
                LOOP_ASSERT(ti, curLen == W.length()); // same lengths
                if (veryVerbose) { cout << "\t"; P(W); }

                // Stretch capacity of x object by different amounts.

                for (int ei = 0; ei < NUM_EXTEND; ++ei) {
                    const int N = EXTEND[ei];
                    if (veryVerbose) { cout << "\t\t"; P(N) }

                    Obj *pX = new Obj(&testAllocator);
                    Obj &mX = *pX;              stretchRemoveAll(&mX, N);
                    const Obj& X = mX;          gg(&mX, SPEC);
                    if (veryVerbose) { cout << "\t\t"; P(X); }

                    {                                   // No allocator.
                        const Obj Y0(X);
                        if (veryVerbose) { cout << "\t\t\t"; P(Y0); }
                        LOOP2_ASSERT(SPEC, N, W == Y0);
                        LOOP2_ASSERT(SPEC, N, W == X);
                    }

                    {                                   // Null allocator.
                        const Obj Y1(X, (bdema_Allocator *) 0);
                        if (veryVerbose) { cout << "\t\t\t"; P(Y1); }
                        LOOP2_ASSERT(SPEC, N, W == Y1);
                        LOOP2_ASSERT(SPEC, N, W == X);
                    }

                    BEGIN_BDEMA_EXCEPTION_TEST {        // Test allocator.
                        const Obj Y2(X, &testAllocator);
                        if (veryVerbose) { cout << "\t\t\t"; P(Y2); }
                        LOOP2_ASSERT(SPEC, N, W == Y2);
                        LOOP2_ASSERT(SPEC, N, W == X);
                    } END_BDEMA_EXCEPTION_TEST

                    {                                   // Buffer Allocator.
                        char memory[1024];
                        bdema_BufferAllocator a(memory, sizeof memory);
                        Obj *Y = new(a.allocate(sizeof(Obj))) Obj(X, &a);
                        if (veryVerbose) { cout << "\t\t\t"; P(*Y); }
                        LOOP2_ASSERT(SPEC, N, W == *Y);
                        LOOP2_ASSERT(SPEC, N, W == X);
                    }

                    {                             // with 'original' destroyed
                        const Obj Y2(X, &testAllocator);
                        delete pX;
                        if (veryVerbose) { cout << "\t\t\t"; P(Y2); }
                        LOOP2_ASSERT(SPEC, N, W == Y2);
                        //LOOP2_ASSERT(SPEC, N, W == X);// This work before!!
                    }
                }
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS:
        //   Since 'operators==' is implemented in terms of basic accessors,
        //   it is sufficient to verify only that a difference in value of any
        //   one basic accessor for any two given objects implies inequality.
        //   However, to test that no other internal state information is
        //   being considered, we want also to verify that 'operator==' reports
        //   true when applied to any two objects whose internal
        //   representations may be different yet still represent the same
        //   (logical) value:
        //      - d_size
        //      - the (corresponding) amount of dynamically allocated memory
        //
        //   Note also that both equality operators must return either 1 or 0,
        //   and neither 'lhs' nor 'rhs' value may be modified.
        //
        // Plan:
        //   First specify a set S of unique object values having various minor
        //   or subtle differences, ordered by non-decreasing length.  Verify
        //   the correctness of 'operator==' and 'operator!=' (returning either
        //   1 or 0) using all elements (u, v) of the cross product S X S.
        //
        //   Next specify a second set S' containing a representative variety
        //   of (black-box) box values ordered by increasing (logical) length.
        //   For each value in S', construct an object x along with a sequence
        //   of similarly constructed duplicates x1, x2, ..., xN.  Attempt to
        //   affect every aspect of white-box state by altering each xi in a
        //   unique way.  Verify correctness of 'operator==' and 'operator!='
        //   by asserting that each element in { x, x1, x2, ..., xN } is
        //   equivalent to every other element.
        //
        // Testing:
        //   operator==(const bdea_Array<char>&, const bdea_Array<char>&);
        //   operator!=(const bdea_Array<char>&, const bdea_Array<char>&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Equality Operators" << endl
                          << "==========================" << endl;

        if (verbose) cout <<
            "\nCompare each pair of similar values (u, v) in S X S." << endl;
        {
            static const char *SPECS[] = {
                "",
                "A",      "B",
                "AA",     "AB",     "BB",     "BA",
                "AAA",    "BAA",    "ABA",    "AAB",
                "AAAA",   "BAAA",   "ABAA",   "AABA",   "AAAB",
                "AAAAA",  "BAAAA",  "ABAAA",  "AABAA",  "AAABA",  "AAAAB",
                "AAAAAA", "BAAAAA", "AABAAA", "AAABAA", "AAAAAB",
                "AAAAAAA",          "BAAAAAA",          "AAAAABA",
                "AAAAAAAA",         "ABAAAAAA",         "AAAAABAA",
                "AAAAAAAAA",        "AABAAAAAA",        "AAAAABAAA",
                "AAAAAAAAAA",       "AAABAAAAAA",       "AAAAABAAAA",
            0}; // Null string required as last element.

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const U_SPEC = SPECS[ti];
                const int curLen = (int)strlen(U_SPEC);

                Obj mU(&testAllocator); gg(&mU, U_SPEC); const Obj& U = mU;
                LOOP_ASSERT(ti, curLen == U.length()); // same lengths

                if (curLen != oldLen) {
                    if (verbose) cout << "\tUsing lhs objects of length "
                                  << curLen << '.' << endl;
                    LOOP_ASSERT(U_SPEC, oldLen <= curLen); // non-decreasing
                    oldLen = curLen;
                }

                if (veryVerbose) { P_(ti); P_(U_SPEC); P(U); }

                for (int tj = 0; SPECS[tj]; ++tj) {
                    const char *const V_SPEC = SPECS[tj];
                    Obj mV(&testAllocator); gg(&mV, V_SPEC); const Obj& V = mV;

                    if (veryVerbose) { cout << "  "; P_(tj); P_(V_SPEC); P(V);}
                    const int isSame = ti == tj;
                    LOOP2_ASSERT(ti, tj,  isSame == (U == V));
                    LOOP2_ASSERT(ti, tj, !isSame == (U != V));
                }
            }
        }

        if (verbose) cout << "\nCompare objects of equal value having "
                             "potentially different internal state." << endl;
        {
            static const char *SPECS[] = { // len: 0 - 5, 7, 8, 9, 15, 16, 17
                "",      "A",      "AB",     "ABC",    "ABCD",   "ABCDE",
                "ABCDEAB",         "ABCDEABC",         "ABCDEABCD",
                "ABCDEABCDEABCDE", "ABCDEABCDEABCDEA", "ABCDEABCDEABCDEAB",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];
                const int curLen = (int)strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\tUsing objects having (logical) "
                                         "length " << curLen << '.' << endl;
                    LOOP_ASSERT(SPEC, oldLen < curLen); // strictly increasing
                    oldLen = curLen;
                }

                Obj mX(&testAllocator); gg(&mX, SPEC); const Obj& X = mX;
                LOOP_ASSERT(ti, curLen == X.length()); // same lengths
                if (veryVerbose) { cout << "\t\t"; P_(ti); P_(SPEC); P(X)}

                for (int u = 0; u < NUM_EXTEND; ++u) {
                    const int U_N = EXTEND[u];
                    Obj mU(&testAllocator);  stretchRemoveAll(&mU, U_N);
                    const Obj& U = mU;       gg(&mU, SPEC);

                    if (veryVerbose) { cout << "\t\t\t"; P_(U_N); P(U)}

                    // compare canonical representation with every variation

                    LOOP2_ASSERT(SPEC, U_N, 1 == (U == X));
                    LOOP2_ASSERT(SPEC, U_N, 1 == (X == U));
                    LOOP2_ASSERT(SPEC, U_N, 0 == (U != X));
                    LOOP2_ASSERT(SPEC, U_N, 0 == (X != U));

                    for (int v = 0; v < NUM_EXTEND; ++v) {
                        const int V_N = EXTEND[v];
                        Obj mV(&testAllocator);  stretchRemoveAll(&mV, V_N);
                        const Obj& V = mV;       gg(&mV, SPEC);

                        static int firstFew = 2 * NUM_EXTEND * NUM_EXTEND;
                        if (veryVeryVerbose || veryVerbose && firstFew > 0) {
                            cout << "\t| "; P_(U_N); P_(V_N); P_(U); P(V);
                            --firstFew;
                        }

                        // compare every variation with every other one

                        LOOP3_ASSERT(SPEC, U_N, V_N, 1 == (U == V));
                        LOOP3_ASSERT(SPEC, U_N, V_N, 0 == (U != V));
                    }
                }
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR:
        //   Since the output operator is layered on basic accessors, it is
        //   sufficient to test only the output *format* (and to ensure that no
        //   additional characters are written past the terminating null).
        //
        // Plan:
        //   For each of a small representative set of object values, ordered
        //   by increasing length, use 'ostrstream' to write that object's
        //   value to two separate character buffers each with different
        //   initial values.  Compare the contents of these buffers with the
        //   literal expected output format and verify that the characters
        //   beyond the null characters are unaffected in both buffers.
        //
        // Testing:
        //   operator<<(ostream&, const bdea_Array<char>&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Output (<<) Operator" << endl
                          << "============================" << endl;

        if (verbose) cout << "\nTesting 'operator<<' (ostream)." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                //line  spec            output format     // ADJUST
                //----  --------------  ------------------
                { L_,   "",             "[ ]"            },
                { L_,   "A",            "[ a ]"          },
                { L_,   "BC",           "[ b c ]"        },
                { L_,   "ABCDE",        "[ a b c d e ]"  },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int SIZE = 1000; // Must be big enough to hold output string.
            const char Z1 = (char) 0xFF;  // Used to represent an unset char.
            const char Z2 = 0x00;         // Used to represent an unset char.

            char mCtrlBuf1[SIZE];  memset(mCtrlBuf1, Z1, SIZE);
            char mCtrlBuf2[SIZE];  memset(mCtrlBuf2, Z2, SIZE);
            const char *CTRL_BUF1 = mCtrlBuf1;
            const char *CTRL_BUF2 = mCtrlBuf2;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const char *const FMT  = DATA[ti].d_fmt_p;
                const int curLen = (int)strlen(SPEC);

                char buf1[SIZE + epcBug], buf2[SIZE + epcBug];
                memcpy(buf1, CTRL_BUF1, SIZE); // Preset buf1 to Z1 values.
                memcpy(buf2, CTRL_BUF2, SIZE); // Preset buf2 to Z2 values.

                Obj mX(&testAllocator);  const Obj& X = gg(&mX, SPEC);
                LOOP_ASSERT(ti, curLen == X.length()); // same lengths

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton objects of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen < curLen); // strictly increasing
                    oldLen = curLen;
                }

                if (verbose) cout << "\t\tSpec = \"" << SPEC << '"' << endl;
                if (veryVerbose) cout << "EXPECTED FORMAT:" << endl<<FMT<<endl;
                ostrstream out1(buf1, SIZE);  out1 << X << ends;
                ostrstream out2(buf2, SIZE);  out2 << X << ends;
                if (veryVerbose) cout << "ACTUAL FORMAT:" << endl<<buf1<<endl;

                const int SZ = (int)strlen(FMT) + 1;
                const int REST = SIZE - SZ;
                LOOP_ASSERT(ti, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(ti, Z1 == buf1[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(ti, Z2 == buf2[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(ti, 0 == memcmp(buf1, FMT, SZ));
                LOOP_ASSERT(ti, 0 == memcmp(buf2, FMT, SZ));
                LOOP_ASSERT(ti, 0 == memcmp(buf1 + SZ, CTRL_BUF1 + SZ, REST));
                LOOP_ASSERT(ti, 0 == memcmp(buf2 + SZ, CTRL_BUF2 + SZ, REST));
            }
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS:
        //   Having implemented an effective generation mechanism, we now
        //   would like to test thoroughly the basic accessor functions
        //     - length() const
        //     - operator[](int) const
        //   Also, we want to ensure that various internal state
        //   representations for a given value produce identical results.
        //
        // Plan:
        //   Specify a set S of representative object values ordered by
        //   increasing length.  For each value w in S, initialize a newly
        //   constructed object x with w using 'gg' and verify that each basic
        //   accessor returns the expected result.  Reinitialize and repeat
        //   the same test on an existing object y after perturbing y so as to
        //   achieve an internal state representation of w that is potentially
        //   different from that of x.
        //
        // Testing:
        //   int length() const;
        //   const char& operator[](int index) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Basic Accessors" << endl
                          << "=======================" << endl;

        if (verbose) cout << "\nTesting 'length' & 'operator[]'" << endl;
        {
            const int SZ = 10;
            static const struct {
                int         d_lineNum;          // source line number
                const char *d_spec_p;           // specification string
                int         d_length;           // expected length
                Element     d_elements[SZ];     // expected element values
            } DATA[] = {
                //line  spec            length  elements
                //----  --------------  ------  ------------------------
                { L_,   "",             0,      { 0 }                   },
                { L_,   "A",            1,      { VA }                  },
                { L_,   "B",            1,      { VB }                  },
                { L_,   "AB",           2,      { VA, VB }              },
                { L_,   "BC",           2,      { VB, VC }              },
                { L_,   "BCA",          3,      { VB, VC, VA }          },
                { L_,   "CAB",          3,      { VC, VA, VB }          },
                { L_,   "CDAB",         4,      { VC, VD, VA, VB }      },
                { L_,   "DABC",         4,      { VD, VA, VB, VC }      },
                { L_,   "ABCDE",        5,      { VA, VB, VC, VD, VE }  },
                { L_,   "EDCBA",        5,      { VE, VD, VC, VB, VA }  },
                { L_,   "ABCDEAB",      7,      { VA, VB, VC, VD, VE,
                                                  VA, VB }              },
                { L_,   "BACDEABC",     8,      { VB, VA, VC, VD, VE,
                                                  VA, VB, VC }          },
                { L_,   "CBADEABCD",    9,      { VC, VB, VA, VD, VE,
                                                  VA, VB, VC, VD }      },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            Obj mY(&testAllocator);  // object with extended internal capacity
            const int EXTEND = 50; stretch(&mY, EXTEND); ASSERT(mY.length());
            if (veryVerbose) cout << "\tEXTEND = " << EXTEND << endl;

            int oldLen= -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int LENGTH       = DATA[ti].d_length;
                const Element *const e = DATA[ti].d_elements;
                const int curLen = LENGTH;

                Obj mX(&testAllocator);

                const Obj& X = gg(&mX, SPEC);   // canonical organization
                mY.removeAll();
                const Obj& Y = gg(&mY, SPEC);   // has extended capacity

                LOOP_ASSERT(ti, curLen == X.length()); // same lengths

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton objects of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                if (verbose) cout << "\t\tSpec = \"" << SPEC << '"' << endl;
                if (veryVerbose) { cout << "\t\t\t"; P(X);
                                   cout << "\t\t\t"; P(Y); }

                LOOP_ASSERT(LINE, LENGTH == X.length());
                LOOP_ASSERT(LINE, LENGTH == Y.length());
                int i;
                for (i = 0; i < LENGTH; ++i) {
                    LOOP2_ASSERT(LINE, i, e[i] == X[i]);
                    LOOP2_ASSERT(LINE, i, e[i] == Y[i]);
                }
                for (; i < SZ; ++i) {
                    LOOP2_ASSERT(LINE, i, 0 == e[i]);
                }
            }
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PRIMITIVE GENERATOR FUNCTION gg
        //   Having demonstrated that our primary manipulators work as expected
        //   under normal conditions, we want to verify (1) that valid
        //   generator syntax produces expected results and (2) that invalid
        //   syntax is detected and reported.
        //
        //   We want also to make trustworthy some additional test helper
        //   functionality that we will use within the first 10 test cases:
        //    - 'stretch'          Tested separately to observe stretch occurs.
        //    - 'stretchRemoveAll' Deliberately implemented using 'stretch'.
        //
        //   Finally we want to make sure that we can rationalize the internal
        //   memory management with respect to the primary manipulators (i.e.,
        //   precisely when new blocks are allocated and deallocated).
        //
        // Plan:
        //   For each of an enumerated sequence of 'spec' values, ordered by
        //   increasing 'spec' length, use the primitive generator function
        //   'gg' to set the state of a newly created object.  Verify that 'gg'
        //   returns a valid reference to the modified argument object and,
        //   using basic accessors, that the value of the object is as
        //   expected.  Repeat the test for a longer 'spec' generated by
        //   prepending a string ending in a '~' character (denoting
        //   'removeAll').  Note that we are testing the parser only; the
        //   primary manipulators are already assumed to work.
        //
        //   To verify that the stretching functions work as expected (and to
        //   cross-check that internal memory is being managed as intended),
        //   create a depth-ordered enumeration of initial values and sizes by
        //   which to extend the initial value.  Record as expected values the
        //   total number of memory blocks allocated during the first and
        //   second modifications of each object.  For each test vector,
        //   construct two identical objects X and Y and bring each to the
        //   initial state.  Assert that the memory allocation for the two
        //   operations are identical and consistent with the first expected
        //   value.  Next apply the 'stretch' and 'stretchRemoveAll' functions
        //   to X and Y (respectively) and again compare the memory allocation
        //   characteristics for the two functions.  Note that we will track
        //   the *total* number of *blocks* allocated as well as the *current*
        //   number of *bytes* in use -- this to measure different aspects of
        //   operation while remaining insensitive to the array 'Element' size.
        //
        // Testing:
        //   bdea_Array<char>& gg(bdea_Array<char>* object, const char *spec);
        //   int ggg(bdea_Array<char> *object, const char *spec, int vF = 1);
        //   void stretch(Obj *object, int size);
        //   void stretchRemoveAll(Obj *object, int size);
        //   CONCERN: Is the internal memory organization behaving as intended?
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing Primitive Generator Function 'gg'" << endl
                 << "=========================================" << endl;

        if (verbose) cout << "\nTesting generator on valid specs." << endl;
        {
            const int SZ = 10;
            static const struct {
                int         d_lineNum;          // source line number
                const char *d_spec_p;           // specification string
                int         d_length;           // expected length
                Element     d_elements[SZ];     // expected element values
            } DATA[] = {
                //line  spec            length  elements
                //----  --------------  ------  ------------------------
                { L_,   "",             0,      { 0 }                   },

                { L_,   "A",            1,      { VA }                  },
                { L_,   "B",            1,      { VB }                  },
                { L_,   "~",            0,      { 0 }                   },

                { L_,   "CD",           2,      { VC, VD }              },
                { L_,   "E~",           0,      { 0 }                   },
                { L_,   "~E",           1,      { VE }                  },
                { L_,   "~~",           0,      { 0 }                   },

                { L_,   "ABC",          3,      { VA, VB, VC }          },
                { L_,   "~BC",          2,      { VB, VC }              },
                { L_,   "A~C",          1,      { VC }                  },
                { L_,   "AB~",          0,      { 0 }                   },
                { L_,   "~~C",          1,      { VC }                  },
                { L_,   "~B~",          0,      { 0 }                   },
                { L_,   "A~~",          0,      { 0 }                   },
                { L_,   "~~~",          0,      { 0 }                   },

                { L_,   "ABCD",         4,      { VA, VB, VC, VD }      },
                { L_,   "~BCD",         3,      { VB, VC, VD }          },
                { L_,   "A~CD",         2,      { VC, VD }              },
                { L_,   "AB~D",         1,      { VD }                  },
                { L_,   "ABC~",         0,      { 0 }                   },

                { L_,   "ABCDE",        5,      { VA, VB, VC, VD, VE }  },
                { L_,   "~BCDE",        4,      { VB, VC, VD, VE }      },
                { L_,   "AB~DE",        2,      { VD, VE }              },
                { L_,   "ABCD~",        0,      { 0 }                   },
                { L_,   "A~C~E",        1,      { VE }                  },
                { L_,   "~B~D~",        0,      { 0 }                   },

                { L_,   "~CBA~~ABCDE",  5,      { VA, VB, VC, VD, VE }  },

                { L_,   "ABCDE~CDEC~E", 1,      { VE }                  },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int LENGTH       = DATA[ti].d_length;
                const Element *const e = DATA[ti].d_elements;
                const int curLen = (int)strlen(SPEC);

                Obj mX(&testAllocator);
                const Obj& X = gg(&mX, SPEC);   // original spec

                static const char *const MORE_SPEC = "~ABCDEABCDEABCDEABCDE~";
                char buf[100]; strcpy(buf, MORE_SPEC); strcat(buf, SPEC);

                Obj mY(&testAllocator);
                const Obj& Y = gg(&mY, buf);    // extended spec

                if (curLen != oldLen) {
                    if (verbose) cout << "\tof length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                if (veryVerbose) {
                    cout << "\t\t   Spec = \"" << SPEC << '"' << endl;
                    cout << "\t\tBigSpec = \"" << buf << '"' << endl;
                    cout << "\t\t\t"; P(X);
                    cout << "\t\t\t"; P(Y);
                }

                LOOP_ASSERT(LINE, LENGTH == X.length());
                LOOP_ASSERT(LINE, LENGTH == Y.length());
                for (int i = 0; i < LENGTH; ++i) {
                    LOOP2_ASSERT(LINE, i, e[i] == X[i]);
                    LOOP2_ASSERT(LINE, i, e[i] == Y[i]);
                }
            }
        }

        if (verbose) cout << "\nTesting generator on invalid specs." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_index;    // offending character index
            } DATA[] = {
                //line  spec            index
                //----  -------------   -----
                { L_,   "",             -1,     }, // control

                { L_,   "~",            -1,     }, // control
                { L_,   " ",             0,     },
                { L_,   ".",             0,     },
                { L_,   "F",             0,     },

                { L_,   "AE",           -1,     }, // control
                { L_,   "aE",            0,     },
                { L_,   "Ae",            1,     },
                { L_,   ".~",            0,     },
                { L_,   "~!",            1,     },
                { L_,   "  ",            0,     },

                { L_,   "ABC",          -1,     }, // control
                { L_,   " BC",           0,     },
                { L_,   "A C",           1,     },
                { L_,   "AB ",           2,     },
                { L_,   "?#:",           0,     },
                { L_,   "   ",           0,     },

                { L_,   "ABCDE",        -1,     }, // control
                { L_,   "aBCDE",         0,     },
                { L_,   "ABcDE",         2,     },
                { L_,   "ABCDe",         4,     },
                { L_,   "AbCdE",         1,     },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int INDEX        = DATA[ti].d_index;
                const int curLen       = (int)strlen(SPEC);

                Obj mX(&testAllocator);

                if (curLen != oldLen) {
                    if (verbose) cout << "\tof length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                if (veryVerbose) cout <<
                    "\t\tSpec = \"" << SPEC << '"' << endl;

                int result = ggg(&mX, SPEC, veryVerbose);

                LOOP_ASSERT(LINE, INDEX == result);
            }
        }

        if (verbose) cout <<
            "\nTesting 'stretch' and 'stretchRemoveAll'." << endl;
        {
            static const struct {
                int         d_lineNum;       // source line number
                const char *d_spec_p;        // specification string
                int         d_size;          // amount to grow (also length)
                int         d_firstResize;   // total blocks allocated
                int         d_secondResize;  // total blocks allocated

                // Note: total blocks (first/second Resize) and whether or not
                // 'removeAll' deallocates memory depends on 'Element' type.

            } DATA[] = {
                //line  spec            size    firstResize     secondResize
                //----  -------------   ----    -----------     ------------
                { L_,   "",             0,      0,              0       },

                { L_,   "",             1,      0,              0       },
                { L_,   "A",            0,      0,              0       },

                { L_,   "",             2,      0,              1       },
                { L_,   "A",            1,      0,              1       },
                { L_,   "AB",           0,      1,              0       },

                { L_,   "",             3,      0,              2       },
                { L_,   "A",            2,      0,              2       },
                { L_,   "AB",           1,      1,              1       },
                { L_,   "ABC",          0,      2,              0       },

                { L_,   "",             4,      0,              2       },
                { L_,   "A",            3,      0,              2       },
                { L_,   "AB",           2,      1,              1       },
                { L_,   "ABC",          1,      2,              0       },
                { L_,   "ABCD",         0,      2,              0       },

                { L_,   "",             5,      0,              3       },
                { L_,   "A",            4,      0,              3       },
                { L_,   "AB",           3,      1,              2       },
                { L_,   "ABC",          2,      2,              1       },
                { L_,   "ABCD",         1,      2,              1       },
                { L_,   "ABCDE",        0,      3,              0       },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int size         = DATA[ti].d_size;
                const int firstResize  = DATA[ti].d_firstResize;
                const int secondResize = DATA[ti].d_secondResize;
                const int curLen       = (int)strlen(SPEC);
                const int curDepth     = curLen + size;

                Obj mX(&testAllocator);  const Obj& X = mX;
                Obj mY(&testAllocator);  const Obj& Y = mY;

                if (curDepth != oldDepth) {
                    if (verbose) cout << "\ton test vectors of depth "
                                      << curDepth << '.' << endl;
                    LOOP_ASSERT(LINE, oldDepth <= curDepth); // non-decreasing
                    oldDepth = curDepth;
                }

                if (veryVerbose) {
                    cout << "\t\t"; P_(SPEC); P(size);
                    P_(firstResize); P_(secondResize);
                    P_(curLen);      P(curDepth);
                }

                // Create identical objects using the gg function.
                {
                    int blocks1A = testAllocator.numBlocksTotal();
                    int bytes1A = testAllocator.numBytesInUse();

                    gg(&mX, SPEC);

                    int blocks2A = testAllocator.numBlocksTotal();
                    int bytes2A = testAllocator.numBytesInUse();

                    gg(&mY, SPEC);

                    int blocks3A = testAllocator.numBlocksTotal();
                    int bytes3A = testAllocator.numBytesInUse();

                    int blocks12A = blocks2A - blocks1A;
                    int bytes12A = bytes2A - bytes1A;

                    int blocks23A = blocks3A - blocks2A;
                    int bytes23A = bytes3A - bytes2A;

                    if (veryVerbose) { P_(bytes12A);  P_(bytes23A);
                                       P_(blocks12A); P(blocks23A); }

                    LOOP_ASSERT(LINE, curLen == X.length()); // same lengths
                    LOOP_ASSERT(LINE, curLen == Y.length()); // same lengths

                    LOOP_ASSERT(LINE, firstResize == blocks12A);

                    LOOP_ASSERT(LINE, blocks12A == blocks23A);
                    LOOP_ASSERT(LINE, bytes12A == bytes23A);
                }

                // Apply both functions under test to the respective objects.
                {

                    int blocks1B = testAllocator.numBlocksTotal();
                    int bytes1B = testAllocator.numBytesInUse();

                    stretch(&mX, size);

                    int blocks2B = testAllocator.numBlocksTotal();
                    int bytes2B = testAllocator.numBytesInUse();

                    stretchRemoveAll(&mY, size);

                    int blocks3B = testAllocator.numBlocksTotal();
                    int bytes3B = testAllocator.numBytesInUse();

                    int blocks12B = blocks2B - blocks1B;
                    int bytes12B = bytes2B - bytes1B;

                    int blocks23B = blocks3B - blocks2B;
                    int bytes23B = bytes3B - bytes2B;

                    if (veryVerbose) { P_(bytes12B);  P_(bytes23B);
                                       P_(blocks12B); P(blocks23B); }

                    LOOP_ASSERT(LINE, curDepth == X.length());
                    LOOP_ASSERT(LINE,        0 == Y.length());

                    LOOP_ASSERT(LINE, secondResize == blocks12B);

                    LOOP_ASSERT(LINE, blocks12B == blocks23B); // Always true.

                    LOOP_ASSERT(LINE, bytes12B == bytes23B);   // True for POD;
                }                                              // else > or >=.
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
        //   The basic concern is that the default constructor, the destructor,
        //   and, under normal conditions (i.e., no aliasing), the primary
        //   manipulators
        //      - append                (black-box)
        //      - removeAll             (white-box)
        //   operate as expected.  We have the following specific concerns:
        //    1. The default Constructor
        //        1a. creates the correct initial value.
        //        1b. is exception neutral with respect to memory allocation.
        //        1c. has the internal memory management system hooked up
        //              properly so that *all* internally allocated memory
        //              draws from the same user-supplied allocator whenever
        //              one is specified.
        //    2. The destructor properly deallocates all allocated memory to
        //         its corresponding allocator from any attainable state.
        //    3. 'append'
        //        3a. produces the expected value.
        //        3b. increases capacity as needed.
        //        3c. maintains valid internal state.
        //        3d. is exception neutral with respect to memory allocation.
        //    4. 'removeAll'
        //        4a. produces the expected value (empty).
        //        4b. properly destroys each contained element value.
        //        4c. maintains valid internal state.
        //        4d. does not allocate memory.
        //
        // Plan:
        //   To address concerns 1a - 1c, create an object using the default
        //   constructor:
        //    - With and without passing in an allocator.
        //    - In the presence of exceptions during memory allocations using
        //        a 'bdema_TestAllocator' and varying its *allocation* *limit*.
        //    - Where the object is constructed entirely in static memory
        //        (using a 'bdema_BufferAllocator') and never destroyed.
        //
        //   To address concerns 3a - 3c, construct a series of independent
        //   objects, ordered by increasing length.  In each test, allow the
        //   object to leave scope without further modification, so that the
        //   destructor asserts internal object invariants appropriately.
        //   After the final append operation in each test, use the (untested)
        //   basic accessors to cross-check the value of the object
        //   and the 'bdema_TestAllocator' to confirm whether a resize has
        //   occurred.
        //
        //   To address concerns 4a-4c, construct a similar test, replacing
        //   'append' with 'removeAll'; this time, however, use the test
        //   allocator to record *numBlocksInUse* rather than *numBlocksTotal*.
        //
        //   To address concerns 2, 3d, 4d, create a small "area" test that
        //   exercises the construction and destruction of objects of various
        //   lengths and capacities in the presence of memory allocation
        //   exceptions.  Two separate tests will be performed.
        //
        //   Let S be the sequence of integers { 0 .. N - 1 }.
        //      (1) for each i in S, use the default constructor and 'append'
        //          to create an instance of length i, confirm its value (using
        //           basic accessors), and let it leave scope.
        //      (2) for each (i, j) in S X S, use 'append' to create an
        //          instance of length i, use 'removeAll' to erase its value
        //          and confirm (with 'length'), use append to set the instance
        //          to a value of length j, verify the value, and allow the
        //          instance to leave scope.
        //
        //   The first test acts as a "control" in that 'removeAll' is not
        //   called; if only the second test produces an error, we know that
        //   'removeAll' is to blame.  We will rely on 'bdema_TestAllocator'
        //   and purify to address concern 2, and on the object invariant
        //   assertions in the destructor to address concerns 3d and 4d.
        //
        // Testing:
        //   bdea_CharArray(bdema_Allocator *ba);
        //   ~bdea_CharArray();
        //   BOOTSTRAP: void append(char item); // no aliasing
        //   void removeAll();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Primary Manipulators" << endl
                          << "============================" << endl;

        if (verbose) cout << "\nTesting default ctor (thoroughly)." << endl;

        if (verbose) cout << "\tWithout passing in an allocator." << endl;
        {
            const Obj X((bdema_Allocator *)0);
            if (veryVerbose) { cout << "\t\t"; P(X); }
            ASSERT(0 == X.length());
        }
        if (verbose) cout << "\tPassing in an allocator." << endl;

        if (verbose) cout << "\t\tWith no exceptions." << endl;
        {
            const Obj X(&testAllocator);
            if (veryVerbose) { cout << "\t\t"; P(X); }
            ASSERT(0 == X.length());
        }

        if (verbose) cout << "\t\tWith exceptions." << endl;
        {
          BEGIN_BDEMA_EXCEPTION_TEST {
            if (veryVerbose) cout <<
                "\tTesting Exceptions In Default Ctor" << endl;
            const Obj X(&testAllocator);
            if (veryVerbose) { cout << "\t\t"; P(X); }
            ASSERT(0 == X.length());
          } END_BDEMA_EXCEPTION_TEST
        }

        if (verbose) cout << "\tIn place using a buffer allocator." << endl;
        {
            char memory[1024];
            bdema_BufferAllocator a(memory, sizeof memory);
            void *doNotDelete = new(a.allocate(sizeof(Obj))) Obj(&a);
            ASSERT(doNotDelete);

            // No destructor is called; will produce memory leak in purify
            // if internal allocators are not hooked up properly.
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'append' (bootstrap)." << endl;
        {
            if (verbose) cout << "\tOn an object of initial length 0." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;

            const int BB = testAllocator.numBlocksTotal();
            const int B  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.append(V0);
            const int AA = testAllocator.numBlocksTotal();
            const int A  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            ASSERT(BB + 0 == AA); // ADJUST
            ASSERT(B  - 0 == A);  // ADJUST
            ASSERT(1 == X.length());
            ASSERT(V0 == X[0]);
        }
        {
            if (verbose) cout << "\tOn an object of initial length 1." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.append(V0);

            const int BB = testAllocator.numBlocksTotal();
            const int B  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.append(V1);
            const int AA = testAllocator.numBlocksTotal();
            const int A  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            ASSERT(2 == X.length());
            ASSERT(BB + 1 == AA); // ADJUST
            ASSERT(B  - 0 == A);  // ADJUST
            ASSERT(V0 == X[0]);
            ASSERT(V1 == X[1]);
        }
        {
            if (verbose) cout << "\tOn an object of initial length 2." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.append(V0); mX.append(V1);

            const int BB = testAllocator.numBlocksTotal();
            const int B  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.append(V2);
            const int AA = testAllocator.numBlocksTotal();
            const int A  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            ASSERT(BB + 1 == AA); // ADJUST
            ASSERT(B  - 0 == A);  // ADJUST
            ASSERT(3 == X.length());
            ASSERT(V0 == X[0]);
            ASSERT(V1 == X[1]);
            ASSERT(V2 == X[2]);
        }
        {
            if (verbose) cout << "\tOn an object of initial length 3." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.append(V0); mX.append(V1); mX.append(V2);

            const int BB = testAllocator.numBlocksTotal();
            const int B  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.append(V3);
            const int AA = testAllocator.numBlocksTotal();
            const int A  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            ASSERT(BB + 0 == AA); // ADJUST
            ASSERT(B  - 0 == A);  // ADJUST
            ASSERT(4 == X.length());
            ASSERT(V0 == X[0]);
            ASSERT(V1 == X[1]);
            ASSERT(V2 == X[2]);
            ASSERT(V3 == X[3]);
        }
        {
            if (verbose) cout << "\tOn an object of initial length 4." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.append(V0); mX.append(V1); mX.append(V2); mX.append(V3);

            const int BB = testAllocator.numBlocksTotal();
            const int B  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.append(V4);
            const int AA = testAllocator.numBlocksTotal();
            const int A  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            ASSERT(BB + 1 == AA); // ADJUST
            ASSERT(B  - 0 == A);  // ADJUST
            ASSERT(5 == X.length());
            ASSERT(V0 == X[0]);
            ASSERT(V1 == X[1]);
            ASSERT(V2 == X[2]);
            ASSERT(V3 == X[3]);
            ASSERT(V4 == X[4]);
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'removeAll'." << endl;
        {
            if (verbose) cout << "\tOn an object of initial length 0." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            ASSERT(0 == X.length());

            const int BB = testAllocator.numBlocksTotal();
            const int B = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.removeAll();
            const int AA = testAllocator.numBlocksTotal();
            const int A = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            ASSERT(BB == AA);   // always
            ASSERT(B - 0 == A); // ADJUST
            ASSERT(0 == X.length());
        }
        {
            if (verbose) cout << "\tOn an object of initial length 1." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.append(V0);
            ASSERT(1 == X.length());

            const int BB = testAllocator.numBlocksTotal();
            const int B = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.removeAll();
            const int AA = testAllocator.numBlocksTotal();
            const int A = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            ASSERT(BB == AA);    // always
            ASSERT(B - 0 == A);  // ADJUST
            ASSERT(0 == X.length());
        }
        {
            if (verbose) cout << "\tOn an object of initial length 2." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.append(V0); mX.append(V1);
            ASSERT(2 == X.length());

            const int BB = testAllocator.numBlocksTotal();
            const int B = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.removeAll();
            const int AA = testAllocator.numBlocksTotal();
            const int A = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            ASSERT(BB == AA);    // always
            ASSERT(B - 0 == A);  // ADJUST
            ASSERT(0 == X.length());
        }
        {
            if (verbose) cout << "\tOn an object of initial length 3." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.append(V0); mX.append(V1); mX.append(V2);
            ASSERT(3 == X.length());

            const int BB = testAllocator.numBlocksTotal();
            const int B = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.removeAll();
            const int AA = testAllocator.numBlocksTotal();
            const int A = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            ASSERT(BB == AA);    // always
            ASSERT(B - 0 == A);  // ADJUST
            ASSERT(0 == X.length());
        }

        // --------------------------------------------------------------------

        if (verbose) cout <<
          "\nTesting the destructor and exception neutrality." << endl;

        if (verbose) cout << "\tWith 'append' only" << endl;
        {
            // For each lengths i up to some modest limit:
            //    1. create an instance
            //    2. append { V0, V1, V2, V3, V4, V0, ... }  up to length i
            //    3. verify initial length and contents
            //    4. allow the instance to leave scope

            const int NUM_TRIALS = 10;
            for (int i = 0; i < NUM_TRIALS; ++i) { // i is the length
                if (verbose) cout <<
                    "\t\tOn an object of length " << i << '.' << endl;

              BEGIN_BDEMA_EXCEPTION_TEST {
                int k; // loop index

                Obj mX(&testAllocator);  const Obj& X = mX;             // 1.
                for (k = 0; k < i; ++k) {                               // 2.
                    mX.append(VALUES[k % NUM_VALUES]);
                }

                LOOP_ASSERT(i, i == X.length());                        // 3.
                for (k = 0; k < i; ++k) {
                    LOOP2_ASSERT(i, k, VALUES[k % NUM_VALUES] == X[k]);
                }

              } END_BDEMA_EXCEPTION_TEST                                // 4.
            }
        }

        if (verbose) cout << "\tWith 'append' and 'removeAll'" << endl;
        {
            // For each pair of lengths (i, j) up to some modest limit:
            //    1. create an instance
            //    2. append V0 values up to a length of i
            //    3. verify initial length and contents
            //    4. removeAll contents from instance
            //    5. verify length is 0
            //    6. append { V0, V1, V2, V3, V4, V0, ... }  up to length j
            //    7. verify new length and contents
            //    8. allow the instance to leave scope

            const int NUM_TRIALS = 10;
            for (int i = 0; i < NUM_TRIALS; ++i) { // i is first length
                if (verbose) cout <<
                    "\t\tOn an object of initial length " << i << '.' << endl;

                for (int j = 0; j < NUM_TRIALS; ++j) { // j is second length
                    if (veryVerbose) cout <<
                        "\t\t\tAnd with final length " << j << '.' << endl;

                  BEGIN_BDEMA_EXCEPTION_TEST {
                    int k; // loop index

                    Obj mX(&testAllocator);  const Obj& X = mX;         // 1.
                    for (k = 0; k < i; ++k) {                           // 2.
                        mX.append(V0);
                    }

                    LOOP2_ASSERT(i, j, i == X.length());                // 3.
                    for (k = 0; k < i; ++k) {
                        LOOP3_ASSERT(i, j, k, V0 == X[k]);
                    }

                    mX.removeAll();                                     // 4.
                    LOOP2_ASSERT(i, j, 0 == X.length());                // 5.

                    for (k = 0; k < j; ++k) {                           // 6.
                        mX.append(VALUES[k % NUM_VALUES]);
                    }

                    LOOP2_ASSERT(i, j, j == X.length());                // 7.
                    for (k = 0; k < j; ++k) {
                        LOOP3_ASSERT(i, j, k, VALUES[k % NUM_VALUES] == X[k]);
                    }
                  } END_BDEMA_EXCEPTION_TEST                            // 8.
                }
            }
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   We want to exercise basic value-semantic functionality.  In
        //   particular we want to demonstrate a base-line level of correct
        //   operation of the following methods and operators:
        //      - default and copy constructors (and also the destructor)
        //      - the assignment operator (including aliasing)
        //      - equality operators: 'operator==' and 'operator!='
        //      - the output operator: 'operator<<'
        //      - primary manipulators: 'append' and 'removeAll' methods
        //      - basic accesors: 'length' and 'operator[]'
        //   In addition we would like to exercise objects with potentially
        //   different internal organizations representing the same value.
        //
        // Plan:
        //   Create four objects using both the default and copy constructors.
        //   Exercise these objects using primary manipulators, basic
        //   accessors, equality operators, and the assignment operator.
        //   Invoke the primary manipulator [1&5], copy constructor [2&8], and
        //   assignment operator [9&10] in situations where the internal data
        //   (i) does *not* and (ii) *does* have to resize.  Try aliasing with
        //   assignment for a non-empty instance [11] and allow the result to
        //   leave scope, enabling the destructor to assert internal object
        //   invariants.  Display object values frequently in verbose mode:
        //
        // 1. Create an object x1 (default ctor).       { x1: }
        // 2. Create a second object x2 (copy from x1). { x1: x2: }
        // 3. Append an element value A to x1).         { x1:A x2: }
        // 4. Append the same element value A to x2).   { x1:A x2:A }
        // 5. Append another element value B to x2).    { x1:A x2:AB }
        // 6. Remove all elements from x1.              { x1: x2:AB }
        // 7. Create a third object x3 (default ctor).  { x1: x2:AB x3: }
        // 8. Create a forth object x4 (copy of x2).    { x1: x2:AB x3: x4:AB }
        // 9. Assign x2 = x1 (non-empty becomes empty). { x1: x2: x3: x4:AB }
        // 10. Assign x3 = x4 (empty becomes non-empty).{ x1: x2: x3:AB x4:AB }
        // 11. Assign x4 = x4 (aliasing).               { x1: x2: x3:AB x4:AB }
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.

        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1. Create an object x1 (default ctor)."
                             "\t\t\t{ x1: }" << endl;
        Obj mX1(&testAllocator);  const Obj& X1 = mX1;
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout <<
            "\ta. Check initial state of x1." << endl;
        ASSERT(0 == X1.length());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1." << endl;
        ASSERT(X1 == X1 == 1);          ASSERT(X1 != X1 == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2. Create a second object x2 (copy from x1)."
                             "\t\t{ x1: x2: }" << endl;
        Obj mX2(X1, &testAllocator);  const Obj& X2 = mX2;
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout <<
            "\ta. Check the initial state of x2." << endl;
        ASSERT(0 == X2.length());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(X2 == X1 == 1);          ASSERT(X2 != X1 == 0);
        ASSERT(X2 == X2 == 1);          ASSERT(X2 != X2 == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Append an element value A to x1)."
                             "\t\t\t{ x1:A x2: }" << endl;
        mX1.append(VA);
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout <<
            "\ta. Check new state of x1." << endl;
        ASSERT(1 == X1.length());
        ASSERT(VA == X1[0]);

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT(X1 == X1 == 1);          ASSERT(X1 != X1 == 0);
        ASSERT(X1 == X2 == 0);          ASSERT(X1 != X2 == 1);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4. Append the same element value A to x2)."
                             "\t\t{ x1:A x2:A }" << endl;
        mX2.append(VA);
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout <<
            "\ta. Check new state of x2." << endl;
        ASSERT(1 == X2.length());
        ASSERT(VA == X2[0]);

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(X2 == X1 == 1);          ASSERT(X2 != X1 == 0);
        ASSERT(X2 == X2 == 1);          ASSERT(X2 != X2 == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5. Append another element value B to x2)."
                             "\t\t{ x1:A x2:AB }" << endl;
        mX2.append(VB);
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout <<
            "\ta. Check new state of x2." << endl;
        ASSERT(2 == X2.length());
        ASSERT(VA == X2[0]);
        ASSERT(VB == X2[1]);

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(X2 == X1 == 0);          ASSERT(X2 != X1 == 1);
        ASSERT(X2 == X2 == 1);          ASSERT(X2 != X2 == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6. Remove all elements from x1."
                             "\t\t\t{ x1: x2:AB }" << endl;
        mX1.removeAll();
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout <<
            "\ta. Check new state of x1." << endl;
        ASSERT(0 == X1.length());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT(X1 == X1 == 1);          ASSERT(X1 != X1 == 0);
        ASSERT(X1 == X2 == 0);          ASSERT(X1 != X2 == 1);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 7. Create a third object x3 (default ctor)."
                             "\t\t{ x1: x2:AB x3: }" << endl;

        Obj mX3(&testAllocator);  const Obj& X3 = mX3;
        if (verbose) { cout << '\t';  P(X3); }

        if (verbose) cout <<
            "\ta. Check new state of x3." << endl;
        ASSERT(0 == X3.length());

        if (verbose) cout <<
            "\tb. Try equality operators: x3 <op> x1, x2, x3." << endl;
        ASSERT(X3 == X1 == 1);          ASSERT(X3 != X1 == 0);
        ASSERT(X3 == X2 == 0);          ASSERT(X3 != X2 == 1);
        ASSERT(X3 == X3 == 1);          ASSERT(X3 != X3 == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 8. Create a forth object x4 (copy of x2)."
                             "\t\t{ x1: x2:AB x3: x4:AB }" << endl;

        Obj mX4(X2, &testAllocator);  const Obj& X4 = mX4;
        if (verbose) { cout << '\t';  P(X4); }

        if (verbose) cout <<
            "\ta. Check new state of x4." << endl;

        ASSERT(2 == X4.length());
        ASSERT(VA == X4[0]);
        ASSERT(VB == X4[1]);

        if (verbose) cout <<
            "\tb. Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(X4 == X1 == 0);          ASSERT(X4 != X1 == 1);
        ASSERT(X4 == X2 == 1);          ASSERT(X4 != X2 == 0);
        ASSERT(X4 == X3 == 0);          ASSERT(X4 != X3 == 1);
        ASSERT(X4 == X4 == 1);          ASSERT(X4 != X4 == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 9. Assign x2 = x1 (non-empty becomes empty)."
                             "\t\t{ x1: x2: x3: x4:AB }" << endl;

        mX2 = X1;
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout <<
            "\ta. Check new state of x2." << endl;
        ASSERT(0 == X2.length());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(X2 == X1 == 1);          ASSERT(X2 != X1 == 0);
        ASSERT(X2 == X2 == 1);          ASSERT(X2 != X2 == 0);
        ASSERT(X2 == X3 == 1);          ASSERT(X2 != X3 == 0);
        ASSERT(X2 == X4 == 0);          ASSERT(X2 != X4 == 1);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n10. Assign x3 = x4 (empty becomes non-empty)."
                             "\t\t{ x1: x2: x3:AB x4:AB }" << endl;

        mX3 = X4;
        if (verbose) { cout << '\t';  P(X3); }

        if (verbose) cout <<
            "\ta. Check new state of x3." << endl;
        ASSERT(2 == X3.length());
        ASSERT(VA == X3[0]);
        ASSERT(VB == X3[1]);

        if (verbose) cout <<
            "\tb. Try equality operators: x3 <op> x1, x2, x3, x4." << endl;
        ASSERT(X3 == X1 == 0);          ASSERT(X3 != X1 == 1);
        ASSERT(X3 == X2 == 0);          ASSERT(X3 != X2 == 1);
        ASSERT(X3 == X3 == 1);          ASSERT(X3 != X3 == 0);
        ASSERT(X3 == X4 == 1);          ASSERT(X3 != X4 == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n11. Assign x4 = x4 (aliasing)."
                             "\t\t\t\t{ x1: x2: x3:AB x4:AB }" << endl;

        mX4 = X4;
        if (verbose) { cout << '\t';  P(X4); }

        if (verbose) cout <<
            "\ta. Check new state of x4." << endl;
        ASSERT(2 == X4.length());
        ASSERT(VA == X4[0]);
        ASSERT(VB == X4[1]);

        if (verbose) cout <<
            "\tb. Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(X4 == X1 == 0);          ASSERT(X4 != X1 == 1);
        ASSERT(X4 == X2 == 0);          ASSERT(X4 != X2 == 1);
        ASSERT(X4 == X3 == 1);          ASSERT(X4 != X3 == 0);
        ASSERT(X4 == X4 == 1);          ASSERT(X4 != X4 == 0);

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
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
