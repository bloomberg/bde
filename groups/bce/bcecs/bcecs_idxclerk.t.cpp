// bcecs_idxclerk.t.cpp   -*-C++-*-
#include <bcecs_idxclerk.h>

#include <bcema_testallocator.h>                // for testing only

#include <bdes_platformutil.h>                  // for testing only
#include <bdema_bufferallocator.h>              // for testing only
#include <bdema_testallocatorexception.h>       // for testing only
#include <bdex_testoutstream.h>                 // for testing only
#include <bdex_testinstream.h>                  // for testing only
#include <bdex_testinstreamexception.h>         // for testing only

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <strstream>

using namespace BloombergLP;
using namespace std;

//==========================================================================
//                             TEST PLAN
//--------------------------------------------------------------------------
//                              Overview
//                              --------
//   We have chosen the basic manipulators for 'bcecs_IdxClerk'
//   to be 'getIndex' and 'putIndex'.
//--------------------------------------------------------------------------
// [ 2] bcecs_IdxClerk(int maxIndex, bdema_Allocator *ba);
// [ 7] bcecs_IdxClerk(const bcecs_IdxClerk&, bdema_Allocator *ba);
// [ 2] ~bcecs_IdxClerk();
// [ 9] bcecs_IdxClerk& operator=(const bcecs_IdxClerk& rhs);
// [ 2] int getIndex(void);
// [ 2] void putIndex(int index);
// [ 6] void removeAll();
// [ 4] int numCommissionedIndices() const;
// [ 4] int numDecommissionedIndices() const;
// [ 4] int nextNewIndex() const;
//
// [ 6] operator==(const bcecs_IdxClerk& lhs, const bcecs_IdxClerk& rhs);
// [ 6] operator!=(const bcecs_IdxClerk& lhs, const bcecs_IdxClerk& rhs);
//--------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] bcecs_IdxClerk& gg(bcecs_IdxClerk* object, const char *spec);
// [ 8] bcecs_IdxClerk   g(const char *spec);

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


//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bcecs_IdxClerk      Obj;

//=============================================================================
//              GENERATOR FUNCTIONS 'g' and 'gg' FOR TESTING
//-----------------------------------------------------------------------------
// The following functions interpret the given 'spec' in order from left to
// right to configure the object according to a custom language.  The 'spec'
// has two parts separated by a delimiter [,].  Characters, '0'..'9', on
// the left of delimiter corresponds to the decommissioned indices.  The
// number on the right correspond to the next NEW index.  The object's state
// is undefined unless
// (1) there is exactly one delimiter,
// (2) there are no duplicate digits on the left of the delimiter,
// (3) there is at least one digit on the right of the delimiter, and
// (4) the maximum digit to the left of the delimiter is less
//     than the number on the right.
//
// LANGUAGE SPECIFICATION:
// ----------------------
//
// <SPEC>       ::= <LIST>
//
// <LIST>       ::= <DIGIT><LIST> | <DELIM><NUMBER>
//
// <DELIM>      ::= ','
//
// <NUMBER>     ::= <DIGIT>       | <DIGIT><NUMBER>
//
// <ITEM>       ::= <ELEMENT>     | <REMOVE_ALL>
//
// <DIGIT>      ::= 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
//
//  Spec           Decommissioned               Next
//  String         Indices                      New Index
//  -----------    --------------               ---------
//  ",0"                                                0
//  "301,7"        3, 0, 1                              7
//  "87,12"        8, 7                                12
//  ",354"                                            354
//  "543210,6"     5, 4, 3, 2, 1, 0                     6
//
//-----------------------------------------------------------------------------

bcecs_IdxClerk& gg(bcecs_IdxClerk *object, const char *spec)
    // This function configures and returns a reference to the specified
    // (initially empty) 'bcecs_IdxClerk' object according to the specified
    // spec, using only the primary manipulator functions 'getIndex' and
    // 'putIndex'.
{
    ASSERT(object);                ASSERT(spec);

    int i;                      // loop variable
    int maxDecommIndex   = -1;
    int numDecommIndices =  0;
    int nextNewIndex     =  0;
    int delimFoundFlag   =  0;

    const char DELIM = ',';

    if (object->nextNewIndex() != 0) {
        cout << "Error: supplied object not in intial state."  << endl;
        ASSERT(0);
        return *object;
    }

    char digitInUseFlag[10];
    memset(digitInUseFlag, 0, sizeof(digitInUseFlag));

    for (i = 0; spec[i]; ++i) {
        if (DELIM == spec[i])  {        // if (isDelim(c))
            if (delimFoundFlag) {
                cout << "Error at position " << i << ": too "
                     << "many delimiters." << endl;
                ASSERT(0); return *object;
            }
            delimFoundFlag = 1;

            // ASSERT (nextNewIndex >= 0)
            if (spec[i + 1] < '0' || spec[i + 1] > '9') {
                cout << "Error at position " << i + 1 << ": missing "
                     << "required digit after delimiter." << endl;
                ASSERT(0); return *object;
            }

            nextNewIndex = atoi(spec + i + 1);
            if (nextNewIndex <= maxDecommIndex) {
                cout << "Error at position " << i + 1 << ": nextNewIndex, "
                     << nextNewIndex << " <= " << "maxDecommissionedIndex, "
                     << maxDecommIndex << "." << endl;
                ASSERT(0); return *object;
            }
            continue;
        }

        if (spec[i] < '0' || spec[i] > '9') {
            cout << "Error at position " << i << ": '" << spec[i]
                 << "' is not a valid spec character."  << endl;
            ASSERT(0); return *object;
        }

        if (!delimFoundFlag) {
            int dig = spec[i] - '0';
            if (digitInUseFlag[dig]) {
                cout << "Error at position " << i
                     << ": decommissioned index "
                     << dig << " already seen." << endl;
                ASSERT(0); return *object;
            }
            digitInUseFlag[dig] = 1;

            if (dig > maxDecommIndex) {
                maxDecommIndex = dig;
            }
            ++numDecommIndices;
        }
    }

    if (!delimFoundFlag) {                      // Check for a delimiter.
        cout << "Error: missing delimiter, '" << DELIM << "'." << endl;
        ASSERT(0); return *object;
    }

    for (i = 0; i < nextNewIndex; ++i) {        // Set the state of the object.
        ASSERT(i == object->getIndex());
    }
    for (i = numDecommIndices - 1; i >= 0; --i) {
        object->putIndex(spec[i] - '0');
    }

    return *object;
}

bcecs_IdxClerk g(const char *spec)
    // Return, by value, a new object corresponding to the specified spec.
{
    bcecs_IdxClerk object;       // HP's aCC would not allow this method
    return gg(&object, spec);    // to be implemented as a single statement.
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

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bcema_TestAllocator testAllocator(veryVeryVerbose);
    bcema_Allocator *Z = &testAllocator;

    switch (test) { case 0:  // Zero is always the leading case.
      case 11: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR:
        //   Use 'g' to construct a set, S, of unique 'bcecs_IdxClerk'
        //   objects with substantial and varied differences in value.
        //   Using all combinations (u,v) in the cross product S X S,
        //   assign v to (a temporary copy of) u and assert that u == v.
        //   Then test aliasing by assigning (a temporary copy of) each
        //   u to itself, and verifying that its value remains unchanged.
        //
        // Testing:
        //   bcecs_IdxClerk& operator=(const bcecs_IdxClerk& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Assignment Operator" << endl
                          << "===========================" << endl;

        static const char *SPECS[] = {
            ",0",    ",1",    ",2",    ",3",    ",4",    ",5",    ",6",
            ",7",    ",8",    ",9",    "0,1",   "1,2",   "2,3",   "3,4",
            "4,5",   "5,6",   "6,7",   "7,8",   "8,9",   "01,2",  "12,3",
            "23,4",  "34,5",  "45,6",  "56,7",  "67,8",  "78,9",  "012,3",
            "123,4", "234,5", "345,6", "456,7", "567,8", "678,9", "13579,100",
            "02468,200",      "0123456789,111", "876543210,999",
        0}; // Null string required as last element.

        if (verbose) cout << "\nTesting Assignment u = V." << endl;

        for (int j = 0; SPECS[j]; ++j) {
            const Obj V(g(SPECS[j]));
            for (int i = 0; SPECS[i]; ++i) {
                Obj u(g(SPECS[i]), Z);
                u = V;
                LOOP2_ASSERT(j, i, V == u);
            }
        }

        if (verbose) cout << "\nTesting assignment u = u (Aliasing)." << endl;

        for (int i = 0; SPECS[i]; ++i) {
            const Obj U(g(SPECS[i]));
            Obj t(U, Z);
            t = t;
            LOOP_ASSERT(i, U == t);
        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION, g:
        //   For each SPEC in a representative set of specifications, compare
        //   the object returned (by value) from the generator function,
        //   'g(SPEC)' with the value of a newly constructed OBJECT configured
        //   using 'gg(&OBJECT, SPEC)'.
        //
        // Testing:
        //   bcecs_IdxClerk g(const char *spec);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Generator Function 'g'" << endl
                          << "==============================" << endl;

        static const char *SPECS[] = {
            ",0",    ",1",    ",2",    ",3",    ",4",    ",5",    ",6",
            ",7",    ",8",    ",9",    "0,1",   "1,2",   "2,3",   "3,4",
            "4,5",   "5,6",   "6,7",   "7,8",   "8,9",   "01,2",  "12,3",
            "23,4",  "34,5",  "45,6",  "56,7",  "67,8",  "78,9",  "012,3",
            "123,4", "234,5", "345,6", "456,7", "567,8", "678,9", "13579,2000",
            "02468,1000",     "0123456789,111", "876543210,999",
        0}; // Null string required as last element.

        if (verbose) cout <<
            "\nCompare values produced by 'g', 'gg' on various inputs."<< endl;

        for (int i = 0; SPECS[i]; ++i) {
            Obj x(Z); gg(&x, SPECS[i]);
            LOOP_ASSERT(i, x == g(SPECS[i]));
        }

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR:
        //   Use 'gg' to construct a set of 'bcecs_IdxClerk' objects with
        //   substantial and varied differences in value.  For each object x
        //   in the set, copy construct an object y from x.  Assert that x and
        //   y are logically equivalent using the equality operators.
        //
        // Testing:
        //   bcecs_IdxClerk(const bcecs_IdxClerk&, bdema_Allocator *ba);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Copy Constructor" << endl
                          << "========================" << endl;

        static const char *SPECS[] = {
            ",0",    ",1",    ",2",    ",3",    ",4",    ",5",    ",6",
            ",7",    ",8",    ",9",    "0,1",   "1,2",   "2,3",   "3,4",
            "4,5",   "5,6",   "6,7",   "7,8",   "8,9",   "01,2",  "12,3",
            "23,4",  "34,5",  "45,6",  "56,7",  "67,8",  "78,9",  "012,3",
            "123,4", "234,5", "345,6", "456,7", "567,8", "678,9", "13579,2000",
            "02468,1000",     "0123456789,111", "876543210,999",
        0}; // Null string required as last element.

        if (verbose) cout <<
            "\nCopy construct various simple values." << endl;

        int i;  // loop index

        for (i = 0; SPECS[i]; ++i) {
            Obj mX(Z); gg(&mX, SPECS[i]); const Obj& X = mX;
            Obj y(mX, Z);
            LOOP_ASSERT(i, X == y);
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS:
        //   Specify a set, S, of unique 'bcecs_IdxClerk' values with
        //   various minor or subtle differences.  Verify the correctness
        //   of operator== and operator!= using all elements (u,v) of the
        //   cross product S X S.
        //
        // Testing:
        //   int operator==(const bcecs_IdxClerk&, const bcecs_IdxClerk&);
        //   int operator!=(const bcecs_IdxClerk&, const bcecs_IdxClerk& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing equality operator" << endl
                          << "=========================" << endl;

        static const char *SPECS[] = {
            ",0",    ",1",     "0,1",   ",2",    "0,2",     "1,2",    "01,2",
            "10,2",  ",3",     "0,3",   "1,3",   "2,3",     "01,3",   "02,3",
            "12,3",  "10,3",   "20,3",  "21,3",  "012,3",   "021,3",  "120,3",
            "102,3", "201,3",  "210,3", "0123456789,9998",  "0123456789,9999",
            "012345678,9999",  "012345679,9999",
        0}; // Null string required as last element.

        if (verbose) cout <<
            "\nCompare each pair of similar values (u,v) in S X S." << endl;

        int i;  // loop index

        for (i = 0; SPECS[i]; ++i) {
            Obj mU(Z); gg(&mU, SPECS[i]); const Obj& U = mU;
            for (int j = 0; SPECS[j]; ++j) {
                Obj mV(Z); gg(&mV, SPECS[j]); const Obj& V = mV;
                int isSame = (i == j);
                LOOP2_ASSERT(i, j, U == V ==  isSame);
                LOOP2_ASSERT(i, j, U != V == !isSame);
            }
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PRIMITIVE GENERATOR FUNCTION, gg:
        //   For each of an enumerated sequence of specific test cases (ordered
        //   by the next new index value of the object), use the primitive
        //   generator function 'gg' to set the state of a newly created
        //   object.  Verify the object's state (using the iterator and basic
        //   accessors) and assert that the 'gg' function returns a valid
        //   reference to the object.
        //
        // Testing:
        //   bcecs_IdxClerk& gg(bcecs_IdxClerk* object, const char *spec);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing Primitive Generator Function, 'gg'" << endl
                 << "==========================================" << endl;

        static const struct {
            const char *d_spec;
            int d_next;
            int d_numDecom;
            int d_decomArray[10];
        } TEST[] = {
            // spec             next    numDecom        decomArray
            // -----------      ----    --------        ----------
            { ",0",               0,          0,                    },
            { ",1",               1,          0,                    },
            { "0,1",              1,          1,        { 0 }       },
            { ",2",               2,          0,                    },
            { "0,2",              2,          1,        { 0 }       },
            { "1,2",              2,          1,        { 1 }       },
            { "01,2",             2,          2,        { 0, 1 }    },
            { "10,2",             2,          2,        { 1, 0 }    },
            { ",3",               3,          0,                    },
            { "0,3",              3,          1,        { 0 }       },
            { "1,3",              3,          1,        { 1 }       },
            { "2,3",              3,          1,        { 2 }       },
            { "01,3",             3,          2,        { 0, 1 }    },
            { "02,3",             3,          2,        { 0, 2 }    },
            { "12,3",             3,          2,        { 1, 2 }    },
            { "10,3",             3,          2,        { 1, 0 }    },
            { "20,3",             3,          2,        { 2, 0 }    },
            { "21,3",             3,          2,        { 2, 1 }    },
            { "012,3",            3,          3,        { 0, 1, 2 } },
            { "021,3",            3,          3,        { 0, 2, 1 } },
            { "120,3",            3,          3,        { 1, 2, 0 } },
            { "102,3",            3,          3,        { 1, 0, 2 } },
            { "201,3",            3,          3,        { 2, 0, 1 } },
            { "210,3",            3,          3,        { 2, 1, 0 } },
            {  0                                                    }
        };

        for (int i = 0; TEST[i].d_spec; ++i) {
            Obj mX(Z);  const Obj& X = mX;
            LOOP_ASSERT(i, &mX == &gg(&mX, TEST[i].d_spec));
            LOOP_ASSERT(i, TEST[i].d_next == X.nextNewIndex());
            LOOP_ASSERT(i, TEST[i].d_numDecom ==
                                        X.numDecommissionedIndices());
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS:
        //   Enumerate a sequence of independent tests, ordered by increasing
        //   next new index value of 'bcecs_IdxClerk' object.  In each test,
        //   use the default constructor to create an empty object and then
        //   use the primary manipulator functions 'getIndex' and 'putIndex'.
        //   Verify the correctness of the above functions using the basic
        //   accessors 'numCommissionedIndices', 'numDecommissionIndices',
        //   'nextNewIndex', and the iterator, 'bcecs_IdxClerkIter'.  Note that
        //   the destructor is exercised on each configuration as the object
        //   under test leaves scope.
        //
        // Testing:
        //   bcecs_IdxClerk(bdema_Allocator *ba);
        //   ~bcecs_IdxClerk();
        //   int getIndex(void);
        //   void putIndex(int index);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Primary Manipulators" << endl
                          << "============================" << endl;

        if (verbose) cout << "\nTesting default ctor." << endl;
        {
            // c = [ (0) ]
            Obj c(Z);
            ASSERT(0 == c.nextNewIndex());
            ASSERT(0 == c.numCommissionedIndices());
            ASSERT(0 == c.numDecommissionedIndices());

            ASSERT(0 == c.getIndex());
        }

        if (verbose) cout << "\nTesting 'getIndex' and 'putIndex'." << endl;

        if (verbose) cout << "\tTesting when nextNewIndex == 1." << endl;
        {
            // [ (1) ]
            Obj c(Z);
            ASSERT(0 == c.getIndex());
            ASSERT(1 == c.nextNewIndex());
            ASSERT(1 == c.numCommissionedIndices());
            ASSERT(0 == c.numDecommissionedIndices());

            ASSERT(1 == c.getIndex());
        }
        {
            // [ 0 (1) ]
            Obj c(Z);
            ASSERT(0 == c.getIndex());        c.putIndex(0);
            ASSERT(1 == c.nextNewIndex());
            ASSERT(0 == c.numCommissionedIndices());
            ASSERT(1 == c.numDecommissionedIndices());

            ASSERT(0 == c.getIndex());
            ASSERT(1 == c.getIndex());
        }

        if (verbose) cout << "\tTesting when nextNewIndex == 2." << endl;
        {
            // [ (2) ]
            Obj c(Z);
            ASSERT(0 == c.getIndex());
            ASSERT(1 == c.getIndex());
            ASSERT(2 == c.nextNewIndex());
            ASSERT(2 == c.numCommissionedIndices());
            ASSERT(0 == c.numDecommissionedIndices());

            c.putIndex(1);
            ASSERT(1 == c.getIndex());
            ASSERT(2 == c.getIndex());
        }
        {
            // [ 0 (2) ]
            Obj c(Z);
            ASSERT(0 == c.getIndex());
            ASSERT(1 == c.getIndex());        c.putIndex(0);
            ASSERT(2 == c.nextNewIndex());
            ASSERT(1 == c.numCommissionedIndices());
            ASSERT(1 == c.numDecommissionedIndices());

            c.putIndex(1);
            ASSERT(1 == c.getIndex());
            ASSERT(0 == c.getIndex());
            ASSERT(2 == c.getIndex());
        }
        {
            // [ 1 (2) ]
            Obj c(Z);
            ASSERT(0 == c.getIndex());
            ASSERT(1 == c.getIndex());        c.putIndex(1);
            ASSERT(2 == c.nextNewIndex());
            ASSERT(1 == c.numCommissionedIndices());
            ASSERT(1 == c.numDecommissionedIndices());

            ASSERT(1 == c.getIndex());
            ASSERT(2 == c.getIndex());
        }
        {
            // [ 0 1 (2) ]
            Obj c(Z);
            ASSERT(0 == c.getIndex());
            ASSERT(1 == c.getIndex());
            c.putIndex(1);                      c.putIndex(0);
            ASSERT(2 == c.nextNewIndex());
            ASSERT(0 == c.numCommissionedIndices());
            ASSERT(2 == c.numDecommissionedIndices());

            ASSERT(0 == c.getIndex());
            ASSERT(1 == c.getIndex());
            ASSERT(2 == c.getIndex());
        }
        {
            // [ 1 0 (2) ]
            Obj c(Z);
            ASSERT(0 == c.getIndex());
            ASSERT(1 == c.getIndex());
            c.putIndex(0);                      c.putIndex(1);
            ASSERT(2 == c.nextNewIndex());
            ASSERT(0 == c.numCommissionedIndices());
            ASSERT(2 == c.numDecommissionedIndices());

            ASSERT(1 == c.getIndex());
            ASSERT(0 == c.getIndex());
            ASSERT(2 == c.getIndex());
        }

        if (verbose) cout << "\tTesting when nextNewIndex == 3." << endl;
        {
            // [ (3) ]
            Obj c(Z);
            ASSERT(0 == c.getIndex());        ASSERT(1 == c.getIndex());
            ASSERT(2 == c.getIndex());
            ASSERT(3 == c.nextNewIndex());
            ASSERT(3 == c.numCommissionedIndices());
            ASSERT(0 == c.numDecommissionedIndices());
        }
        {
            // [ 0 (3) ]
            Obj c(Z);
            ASSERT(0 == c.getIndex());
            ASSERT(1 == c.getIndex());
            ASSERT(2 == c.getIndex());        c.putIndex(0);
            ASSERT(3 == c.nextNewIndex());
            ASSERT(2 == c.numCommissionedIndices());
            ASSERT(1 == c.numDecommissionedIndices());

            ASSERT(0 == c.getIndex());
            ASSERT(3 == c.getIndex());
        }
        {
            // [ 1 (3) ]
            Obj c(Z);
            ASSERT(0 == c.getIndex());
            ASSERT(1 == c.getIndex());
            ASSERT(2 == c.getIndex());        c.putIndex(1);
            ASSERT(3 == c.nextNewIndex());
            ASSERT(2 == c.numCommissionedIndices());
            ASSERT(1 == c.numDecommissionedIndices());

            ASSERT(1 == c.getIndex());
            ASSERT(3 == c.getIndex());
        }
        {
            // [ 2 (3) ]
            Obj c(Z);
            ASSERT(0 == c.getIndex());
            ASSERT(1 == c.getIndex());
            ASSERT(2 == c.getIndex());        c.putIndex(2);
            ASSERT(3 == c.nextNewIndex());
            ASSERT(2 == c.numCommissionedIndices());
            ASSERT(1 == c.numDecommissionedIndices());

            ASSERT(2 == c.getIndex());
            ASSERT(3 == c.getIndex());
        }

        if (verbose) cout <<
            "\tStress test on 'getIndex' and 'putIndex'." << endl;
        {
            const int MAX_TRAILS = 100;

            for (int size = 0; size < MAX_TRAILS; ++size) {
                int i;  // loop index
                Obj c(Z);

                for (i = 0; i < size; ++i) {
                    LOOP2_ASSERT(size, i, 0 == c.numDecommissionedIndices());
                    LOOP2_ASSERT(size, i, i == c.numCommissionedIndices());
                    LOOP2_ASSERT(size, i, i == c.nextNewIndex());

                    LOOP2_ASSERT(size, i, i == c.getIndex());
                }

                LOOP_ASSERT(size, size == c.nextNewIndex());

                for (i = 0; i < size; ++i) {
                    int idx = i * 2 % size;
                    LOOP_ASSERT(size,size-i == c.numCommissionedIndices());
                    LOOP_ASSERT(size,     i == c.numDecommissionedIndices());

                    c.putIndex(idx);
                    LOOP2_ASSERT(size, i, idx == c.getIndex());
                    c.putIndex(idx);
                }
            }
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Create 'bcecs_IdxClerk' objects using default and copy
        //   constructors.  Exercise these objects using primary manipulators,
        //   basic accessors, equality operators, and the assignment operator.
        //   Display object values frequently in verbose mode.
        //
        // Testing:
        //   This test *exercises* basic functionality, but "tests" nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        if (verbose) cout <<
            "\nCreate an object, x1 (using default ctor)." << endl;
        Obj x1(Z);
//      if (verbose) { cout << '\t'; P(x1); }

        if (verbose) cout << "\tCheck initial state of x1." << endl;
        ASSERT(0 == x1.nextNewIndex());
        ASSERT(0 == x1.numCommissionedIndices());
        ASSERT(0 == x1.numDecommissionedIndices());

        if (verbose) cout << "\tTry equality operators: x1 & x1." << endl;
        ASSERT(x1 == x1 == 1);        ASSERT(x1 != x1 == 0);


        if (verbose) cout <<
            "\nCreate a second object, x2 (from x1 using copy ctor)." << endl;
        Obj x2(x1, Z);                  // Copy empty object.
//      if (verbose) { cout << '\t'; P(x2); }

        if (verbose) cout << "\tCheck initial state of x2." << endl;
        ASSERT(0 == x1.nextNewIndex());
        ASSERT(0 == x1.numCommissionedIndices());
        ASSERT(0 == x1.numDecommissionedIndices());

        if (verbose) cout << "\tTry equality operators: x2 & x1, x2." << endl;
        ASSERT(x2 == x1 == 1);        ASSERT(x2 != x1 == 0);
        ASSERT(x2 == x2 == 1);        ASSERT(x2 != x2 == 0);


        int i, j;
        const int NUM_ISSUED = 10;
        if (verbose) cout <<
            "\nModify x1 using the primary manipulators." << endl;

        for (i = 0; i < NUM_ISSUED; ++i) {
            ASSERT(i == x1.getIndex());
        }
        for (j = 0; j < NUM_ISSUED/2; ++j) {
            x1.putIndex(j);
        }
//      if (verbose) { cout << '\t'; P(x1); }

        if (verbose) cout << "\tCheck new state of x1." << endl;
        ASSERT(NUM_ISSUED   == x1.nextNewIndex());
        ASSERT(NUM_ISSUED/2 == x1.numCommissionedIndices());
        ASSERT(NUM_ISSUED/2 == x1.numDecommissionedIndices());

        if (verbose) cout << "\tTry equality operators: x1 & x2, x1." << endl;
        ASSERT(x1 == x2 == 0);        ASSERT(x1 != x2 == 1);
        ASSERT(x1 == x1 == 1);        ASSERT(x1 != x1 == 0);


        if (verbose) cout <<
            "\nModify x2 using the primary manipulators." << endl;

        for (i = 0; i < NUM_ISSUED; ++i) {
            ASSERT(i == x2.getIndex());
        }
        for (j = 0; j < NUM_ISSUED/2; ++j) {
            x2.putIndex(j);
        }
//      if (verbose) { cout << '\t'; P(x1); }

        if (verbose) cout << "\tCheck new state of x1." << endl;
        ASSERT(NUM_ISSUED   == x2.nextNewIndex());
        ASSERT(NUM_ISSUED/2 == x2.numCommissionedIndices());
        ASSERT(NUM_ISSUED/2 == x2.numDecommissionedIndices());

        if (verbose) cout << "\tTry equality operators: x2 & x1, x2." << endl;
        ASSERT(x2 == x1 == 1);        ASSERT(x2 != x1 == 0);
        ASSERT(x2 == x2 == 1);        ASSERT(x2 != x2 == 0);


        if (verbose) cout << "\nRemove all elements from x1." << endl;
        x1.removeAll();
//      if (verbose) { cout << '\t'; P(x1); }

        if (verbose) cout << "\tCheck new state of x1." << endl;
        ASSERT(0 == x1.nextNewIndex());
        ASSERT(0 == x1.numCommissionedIndices());
        ASSERT(0 == x1.numDecommissionedIndices());

        if (verbose) cout << "\tTry equality operators: x1 & x2, x1." << endl;
        ASSERT(x1 == x2 == 0);        ASSERT(x1 != x2 == 1);
        ASSERT(x1 == x1 == 1);        ASSERT(x1 != x1 == 0);


        if (verbose) cout <<
            "\nCreate a third object, x3 (using default ctor)." << endl;
        Obj x3(Z);
//      if (verbose) { cout << '\t'; P(x3); }

        if (verbose) cout << "\tCheck initial state of x3." << endl;
        ASSERT(0 == x3.nextNewIndex());
        ASSERT(0 == x3.numCommissionedIndices());
        ASSERT(0 == x3.numDecommissionedIndices());

        if (verbose) cout <<
            "\tTry equality operators: x3 & x1, x2, x3." << endl;
        ASSERT(x3 == x1 == 1);        ASSERT(x3 != x1 == 0);
        ASSERT(x3 == x2 == 0);        ASSERT(x3 != x2 == 1);
        ASSERT(x3 == x3 == 1);        ASSERT(x3 != x3 == 0);


        if (verbose) cout <<
            "\nCreate a fourth object, x4 (from x2 using copy ctor)." << endl;
        Obj x4(x2, Z);                  // object x2 not empty
//      if (verbose) { cout << '\t'; P(x4); }

        if (verbose) cout << "\tCheck initial state of x4." << endl;
        ASSERT(NUM_ISSUED   == x4.nextNewIndex());
        ASSERT(NUM_ISSUED/2 == x4.numCommissionedIndices());
        ASSERT(NUM_ISSUED/2 == x4.numDecommissionedIndices());

        if (verbose) cout <<
            "\tTry equality operators: x4 & x1, x2, x3, x4." << endl;
        ASSERT(x4 == x1 == 0);        ASSERT(x4 != x1 == 1);
        ASSERT(x4 == x2 == 1);        ASSERT(x4 != x2 == 0);
        ASSERT(x4 == x3 == 0);        ASSERT(x4 != x3 == 1);
        ASSERT(x4 == x4 == 1);        ASSERT(x4 != x4 == 0);


        if (verbose) cout << "\nAssign x2 to x1 (x1 = x2)." << endl;
        x1 = x2;        // empty <- non-empty
        if (verbose) cout << "\tTry equality operators: x1 & x2, x1." << endl;
        ASSERT(x1 == x2 == 1);        ASSERT(x1 != x2 == 0);
        ASSERT(x1 == x1 == 1);        ASSERT(x1 != x1 == 0);


        if (verbose) cout << "\nAssign x3 to x4 (x4 = x3)." << endl;
        x4 = x3;        // non-empty <- empty
        if (verbose) cout << "\tTry equality operators: x4 & x3, x4." << endl;
        ASSERT(x4 == x3 == 1);        ASSERT(x4 != x3 == 0);
        ASSERT(x4 == x4 == 1);        ASSERT(x4 != x4 == 0);


        if (verbose) cout << "\nAssign x2 to x2 (x2 = x2) [aliasing]." << endl;
        x2 = x2;        // non-empty <- non-empty
        if (verbose) cout << "\tTry equality operators: x2 & x2." << endl;
        ASSERT(x2 == x2 == 1);        ASSERT(x2 != x2 == 0);

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
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
