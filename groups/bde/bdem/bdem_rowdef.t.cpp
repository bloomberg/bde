// bdem_rowdef.t.cpp                   -*-C++-*-

#include <bdem_rowdef.h>

#include <bdem_aggregateoption.h>
#include <bdem_descriptor.h>
#include <bdem_elemtype.h>
#include <bdem_idxoffsetmap.h>

#include <bdema_bufferallocator.h>            // for testing only
#include <bdema_testallocator.h>              // for testing only
#include <bdema_testallocatorexception.h>     // for testing only

#include <bdet_date.h>                        // for testing only
#include <bdet_datetime.h>                    // for testing only
#include <bdet_time.h>                        // for testing only

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctype.h>                            // isspace()

#include <iostream>
#include <string>
#include <vector>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test consists of four constructors, a destructor,
// one manipulator, and four direct accessors.  All of the methods are very
// simple.  Moreover, the copying of objects is not (directly) allowed
// since the copy constructor and assignment operator are declared 'private'
// and left unimplemented.  So we are primarily concerned that the internal
// memory management system functions as expected and that the manipulator and
// accessors return the expected result.  The lone *primary* *manipulator* is
// the (non-const) 'columnTypes' method.  Note that memory allocation must be
// tested for exception neutrality via the 'bdema_testallocator' component.
//-----------------------------------------------------------------------------
// [ 2] bdem_RowDef(int option, int hint, bdema_Allocator *ba);
// [ 5] bdem_RowDef(const bdem_ElemTypeArray& types, option, hint, *ba);
// [ 6] bdem_RowDef(const bdem_IdxOffsetMap& map, option, hint, *ba);
// [ 4] bdem_RowDef(int ne, int option, int hint, *ba);
// [ 2] ~bdem_RowDef();
// [ 2] bdem_IdxOffsetMap& columnTypes();
// [ 3] const bdem_IdxOffsetMap& columnTypes() const;
// [ 3] int infrequentDeleteHint() const;
// [ 3] int allocationMode() const;
// [ 3] bdema_Allocator *allocator() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] BOOTSTRAP
//=============================================================================
//            STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

#if 0
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
#define LOOP_ASSERT(I,X) {                                                    \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) {                                                 \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": "                  \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) {                                               \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"     \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) {                                             \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\n";                    \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) {                                           \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" <<                  \
       #M << ": " << M << "\n";                                               \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" <<                  \
       #M << ": " << M << "\t" << #N << ": " << N << "\n";                    \
       aSsErT(1, #X, __LINE__); } }
#endif

//=============================================================================
//                  STANDARD BDEX EXCEPTION TEST MACROS
//-----------------------------------------------------------------------------

#ifdef BDE_BUILD_TARGET_EXC
#define BEGIN_BDEX_EXCEPTION_TEST                                             \
{                                                                             \
    {                                                                         \
        static int firstTime = 1;                                             \
        if (veryVerbose && firstTime)  cout <<                                \
        "### BDEX EXCEPTION TEST -- (ENABLED) --" << endl;                    \
    firstTime = 0;                                                            \
    }                                                                         \
    if (veryVeryVerbose) cout <<                                              \
    "### Begin bdex exception test." << endl;                                 \
    int bdexExceptionCounter = 0;                                             \
    static int bdexExceptionLimit = 100;                                      \
    testInStream.setInputLimit(bdexExceptionCounter);                         \
    do {                                                                      \
        try {

#define END_BDEX_EXCEPTION_TEST                                               \
        } catch (bdex_TestInStreamException& e) {                             \
        if (veryVerbose && bdexExceptionLimit || veryVeryVerbose)             \
        {                                                                     \
        --bdexExceptionLimit;                                                 \
        cout << "(" <<                                                        \
                bdexExceptionCounter << ')';                                  \
                if (veryVeryVerbose) { cout << " BDEX_EXCEPTION: "            \
                    << "input limit = " << bdexExceptionCounter << ", "       \
                    << "last data type = " << e.dataType();                   \
        }                                                                     \
        else if (0 == bdexExceptionLimit) {                                   \
            cout << " [ Note: 'bdexExceptionLimit' reached. ]";               \
            }                                                                 \
        cout << endl;                                                         \
        }                                                                     \
            testInStream.setInputLimit(++bdexExceptionCounter);               \
            continue;                                                         \
        }                                                                     \
        catch (...) { throw; }                                                \
        testInStream.setInputLimit(-1);                                       \
        break;                                                                \
    } while (1);                                                              \
    if (veryVeryVerbose) cout <<                                              \
    "### End bdex exception test." << endl;                                   \
}
#else
#define BEGIN_BDEX_EXCEPTION_TEST                                             \
{                                                                             \
    static int firstTime = 1;                                                 \
    if (verbose && firstTime) { cout <<                                       \
    "### BDEX EXCEPTION TEST -- (NOT ENABLED) --" << endl;                    \
    firstTime = 0;                                                            \
    }                                                                         \
}
#define END_BDEX_EXCEPTION_TEST
#endif

//=============================================================================
//                  STANDARD BDEMA EXCEPTION TEST MACROS
//-----------------------------------------------------------------------------

#ifdef BDE_BUILD_TARGET_EXC
#define BEGIN_BDEMA_EXCEPTION_TEST {                                          \
    {                                                                         \
        static int firstTime = 1;                                             \
        if (veryVerbose && firstTime) cout <<                                 \
        "### BDEMA EXCEPTION TEST -- (ENABLED) --" << endl;                   \
    firstTime = 0;                                                            \
    }                                                                         \
    if (veryVeryVerbose) cout <<                                              \
    "### Begin bdema exception test." << endl;                                \
    int bdemaExceptionCounter = 0;                                            \
    static int bdemaExceptionLimit = 100;                                     \
    testAllocator.setAllocationLimit(bdemaExceptionCounter);                  \
    do {                                                                      \
        try {

#define END_BDEMA_EXCEPTION_TEST                                              \
        } catch (bdema_TestAllocatorException& e) {                           \
            if (veryVerbose && bdemaExceptionLimit || veryVeryVerbose) {      \
        --bdemaExceptionLimit;                                                \
        cout << "(*** " <<                                                    \
                bdemaExceptionCounter << ')';                                 \
                if (veryVeryVerbose) { cout << " BEDMA_EXCEPTION: "           \
                    << "alloc limit = " << bdemaExceptionCounter << ", "      \
                    << "last alloc size = " << e.numBytes();                  \
            }                                                                 \
        else if (0 == bdemaExceptionLimit) {                                  \
             cout << " [ Note: 'bdemaExceptionLimit' reached. ]";             \
        }                                                                     \
            cout << endl;                                                     \
        }                                                                     \
            testAllocator.setAllocationLimit(++bdemaExceptionCounter);        \
            continue;                                                         \
        }                                                                     \
        testAllocator.setAllocationLimit(-1);                                 \
        break;                                                                \
    } while (1);                                                              \
    if (veryVeryVerbose) cout <<                                              \
    "### End bdema exception test." << endl;                                  \
}
#else
#define BEGIN_BDEMA_EXCEPTION_TEST                                            \
{                                                                             \
    static int firstTime = 1;                                                 \
    if (verbose && firstTime) { cout <<                                       \
    "### BDEMA EXCEPTION TEST -- (NOT ENABLED) --" << endl;                   \
    firstTime = 0;                                                            \
    }                                                                         \
}
#define END_BDEMA_EXCEPTION_TEST
#endif

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                              // current Line number
#define PS(X) cout << #X " = \n" << (X) << endl; // Print identifier and value.
#define T_()  cout << "\t" << flush;             // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdes_PlatformUtil::Int64 Int64;

struct ListRep {
    void  *d_rowDef_p;
    void  *d_row_p;
    int    d_size;
    void  *d_originalAllocator_p;
};


struct TableRep {
    void   *d_rowDef_p;
    void   *d_rowPool_p;
    void   *d_rows_p;
    void   *d_originalAllocator_p;
};

template<bdem_ElemType::Type e, typename t>
struct ElemAttGenerator {
    static bdem_Descriptor s;
};

template<bdem_ElemType::Type e, typename t>
bdem_Descriptor ElemAttGenerator<e,t>::s =
{
    e,
    sizeof(t),
    bdes_AlignmentOf<t>::VALUE,
    0,0,0,0,0,0,0,0,0
};

const int NUM_TYPES = 22;

const bdem_Descriptor *const typesLookupTable[NUM_TYPES] =
{
    &ElemAttGenerator<bdem_ElemType::CHAR,char>::s,
    &ElemAttGenerator<bdem_ElemType::SHORT,short>::s,
    &ElemAttGenerator<bdem_ElemType::INT,int>::s,
    &ElemAttGenerator<bdem_ElemType::INT64,Int64>::s,
    &ElemAttGenerator<bdem_ElemType::FLOAT,float>::s,
    &ElemAttGenerator<bdem_ElemType::DOUBLE,double>::s,
    &ElemAttGenerator<bdem_ElemType::STRING,std::string>::s,
    &ElemAttGenerator<bdem_ElemType::DATETIME,bdet_Datetime>::s,
    &ElemAttGenerator<bdem_ElemType::DATE,bdet_Date>::s,
    &ElemAttGenerator<bdem_ElemType::TIME,bdet_Time>::s,
    &ElemAttGenerator<bdem_ElemType::CHAR_ARRAY,std::vector<char> >::s,
    &ElemAttGenerator<bdem_ElemType::SHORT_ARRAY,std::vector<short> >::s,
    &ElemAttGenerator<bdem_ElemType::INT_ARRAY,std::vector<int> >::s,
    &ElemAttGenerator<bdem_ElemType::INT64_ARRAY,std::vector<Int64> >::s,
    &ElemAttGenerator<bdem_ElemType::FLOAT_ARRAY,std::vector<float> >::s,
    &ElemAttGenerator<bdem_ElemType::DOUBLE_ARRAY,std::vector<double> >::s,
    &ElemAttGenerator<bdem_ElemType::STRING_ARRAY,
                                               std::vector<std::string> >::s,
    &ElemAttGenerator<bdem_ElemType::DATETIME_ARRAY,
                                               std::vector<bdet_Datetime> >::s,
    &ElemAttGenerator<bdem_ElemType::DATE_ARRAY,std::vector<bdet_Date> >::s,
    &ElemAttGenerator<bdem_ElemType::TIME_ARRAY,std::vector<bdet_Time> >::s,
    &ElemAttGenerator<bdem_ElemType::LIST,ListRep >::s,
    &ElemAttGenerator<bdem_ElemType::TABLE,TableRep >::s,
};

typedef bdem_RowDef Obj;

const bdem_AggregateOption::AllocationStrategy POOL_OPT =
                                              bdem_AggregateOption::WRITE_MANY;

#if 0
const bdem_ElemType::Type VALUES[] = {
    bdem_ElemType::CHAR,
    bdem_ElemType::SHORT,
    bdem_ElemType::INT,
    bdem_ElemType::INT64,
    bdem_ElemType::FLOAT,
    bdem_ElemType::DOUBLE,
    bdem_ElemType::DATETIME,
    bdem_ElemType::DATE,
    bdem_ElemType::TIME,
};

const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
#endif

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//              GENERATOR FUNCTIONS 'g' AND 'gg' FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    // int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bdema_TestAllocator  testAllocator(veryVeryVerbose);
    // bdema_Allocator     *Z = &testAllocator;

    switch (test) { case 0:  // Zero is always the leading case.
#if 0  // TBD
// VKVK the scope of changes to rowdef made this test driver almost useless
      case 6: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTOR TAKING AN IDXOFFSETMAP
        // Concerns:
        //   1. The initial value is correct.
        //   2. The constructor is exception neutral w.r.t. memory allocation.
        //   3. The internal memory management system is hooked up properly
        //      so that *all* internally allocated memory draws from a
        //      user-supplied allocator whenever one is specified.
        //   4. The string usage hint, pooling option, and memory allocator
        //      are properly recorded in the object.
        //
        // Plan:
        //   In a loop, use the constructor taking a 'bdem_IdxOffsetMap' to
        //   create test objects.  Increase the size of the index-offset map
        //   in each iteration of the loop.  Verify that each object has the
        //   expected value.  Perform each test in the standard 'bdema'
        //   exception-testing macro block.
        //
        //   Concern 4 is addressed by applying the suite of direct accessors
        //   to the test objects.
        //
        //   White-Box Testing: Repeat the constructor test initially
        //   specifying no allocator and again specifying a static buffer
        //   allocator.  These test for rudimentary correct object behavior
        //   via the destructor and Purify.  Note: When tested stand-alone,
        //   'bdem_RowDef' works correctly when passed a null allocator.
        //   However, clients are expected to instantiate 'bdem_RowDef' with
        //   a non-null allocator.
        //
        // Testing:
        //   bdem_RowDef(const bdem_IdxOffsetMap& map, option, hint, *ba);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing Constructor Taking 'bdem_IdxOffsetMap'" << endl
                 << "==============================================" << endl;

        if (verbose) cout <<
            "\nTesting 'bdem_RowDef(map, option, hint, ba)' ctor" << endl;

        if (verbose) cout << "\tPassing in an allocator." << endl;
        if (verbose) cout << "\t\tWith no exceptions." << endl;
        {
            for (int ne = 0; ne <= NUM_VALUES; ++ne) {
                bdem_IdxOffsetMap M;
                for (int i = 0; i < ne; i++) {
                    M.append(typesLookupTable[VALUES[i]]);
                }
                if (veryVerbose) { P_(ne); P(M); }
                const int NUM_BLOCKS = testAllocator.numBlocksInUse();
                const int NUM_BYTES  = testAllocator.numBytesInUse();
                Obj mX(M, POOL_OPT, DEL_HINT, Z);
                const Obj &X = mX;
                LOOP_ASSERT(ne, NUM_BLOCKS < testAllocator.numBlocksInUse());
                LOOP_ASSERT(ne, NUM_BYTES  < testAllocator.numBytesInUse());
                LOOP_ASSERT(ne, DEL_HINT  == X.infrequentDeleteHint());
                LOOP_ASSERT(ne, POOL_OPT  == X.allocationMode());
                LOOP_ASSERT(ne, Z         == X.allocator());
                LOOP_ASSERT(ne, ne        == X.columnTypes().length());
                LOOP_ASSERT(ne, M         == X.columnTypes());
            }
        }

        if (verbose) cout << "\t\tWith exceptions." << endl;
        {
            for (int ne = 0; ne <= NUM_VALUES; ++ne) {
              bdem_IdxOffsetMap M;
              for (int i = 0; i < ne; i++) {
                  M.append(typesLookupTable[VALUES[i]]);
              }
              if (veryVerbose) { P_(ne); P(M); }
              BEGIN_BDEMA_EXCEPTION_TEST {
                const int NUM_BLOCKS = testAllocator.numBlocksInUse();
                const int NUM_BYTES  = testAllocator.numBytesInUse();
                Obj mX(M, POOL_OPT, DEL_HINT, Z);
                const Obj &X = mX;
                LOOP_ASSERT(ne, NUM_BLOCKS < testAllocator.numBlocksInUse());
                LOOP_ASSERT(ne, NUM_BYTES  < testAllocator.numBytesInUse());
                LOOP_ASSERT(ne, DEL_HINT  == X.infrequentDeleteHint());
                LOOP_ASSERT(ne, POOL_OPT  == X.allocationMode());
                LOOP_ASSERT(ne, Z         == X.allocator());
                LOOP_ASSERT(ne, ne        == X.columnTypes().length());
                LOOP_ASSERT(ne, M         == X.columnTypes());
              } END_BDEMA_EXCEPTION_TEST
            }
        }

        if (verbose) cout << "\tWith no allocator (exercise only)." << endl;
        {
            for (int ne = 0; ne <= NUM_VALUES; ++ne) {
                bdem_IdxOffsetMap M;
                for (int i = 0; i < ne; i++) {
                    M.append(typesLookupTable[VALUES[i]]);
                }
                if (veryVerbose) { P_(ne); P(M); }
                Obj mX(M, POOL_OPT, DEL_HINT, 0);  const Obj &X = mX;
                LOOP_ASSERT(ne, 0  == X.allocator());
                LOOP_ASSERT(ne, ne == X.columnTypes().length());
                LOOP_ASSERT(ne, M  == X.columnTypes());
            }
        }

        if (verbose)
            cout << "\tWith a buffer allocator (exercise only)." << endl;
        {
            char memory[4096];
            bdema_BufferAllocator a(memory, sizeof memory);
            for (int ne = 0; ne <= NUM_VALUES; ++ne) {
                bdem_IdxOffsetMap M;
                for (int i = 0; i < ne; i++) {
                    M.append(typesLookupTable[VALUES[i]]);
                }
                if (veryVerbose) { P_(ne); P(M); }
                Obj *doNotDelete =
                  new(a.allocate(sizeof(Obj))) Obj(M, POOL_OPT, DEL_HINT, &a);
                Obj &mX = *doNotDelete;  const Obj &X = mX;
                LOOP_ASSERT(ne, &a == X.allocator());
                LOOP_ASSERT(ne, ne == X.columnTypes().length());
                LOOP_ASSERT(ne, M  == X.columnTypes());
            }
            // No destructor is called; will produce memory leak in purify
            // if internal allocators are not hooked up properly.
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTOR TAKING AN ELEMTYPEARRAY
        // Concerns:
        //   1. The initial value is correct.
        //   2. The constructor is exception neutral w.r.t. memory allocation.
        //   3. The internal memory management system is hooked up properly
        //      so that *all* internally allocated memory draws from a
        //      user-supplied allocator whenever one is specified.
        //   4. The string usage hint, pooling option, and memory allocator
        //      are properly recorded in the object.
        //
        // Plan:
        //   In a loop, use the constructor taking a 'bdem_ElemTypeArray' to
        //   create test objects.  Increase the size of the element type array
        //   in each iteration of the loop.  Verify that each object has the
        //   expected value.  Perform each test in the standard 'bdema'
        //   exception-testing macro block.
        //
        //   Concern 4 is addressed by applying the suite of direct accessors
        //   to the test objects.
        //
        //   White-Box Testing: Repeat the constructor test initially
        //   specifying no allocator and again specifying a static buffer
        //   allocator.  These test for rudimentary correct object behavior
        //   via the destructor and Purify.  Note: When tested stand-alone,
        //   'bdem_RowDef' works correctly when passed a null allocator.
        //   However, clients are expected to instantiate 'bdem_RowDef'
        //   with a non-null allocator.
        //
        // Testing:
        //   bdem_RowDef(const bdem_ElemTypeArray& types, option, hint, *ba);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing Constructor Taking 'bdem_ElemTypeArray'" << endl
                 << "===============================================" << endl;

        if (verbose) cout <<
            "\nTesting 'bdem_RowDef(types, option, hint, ba)' ctor" << endl;

        if (verbose) cout << "\tPassing in an allocator." << endl;
        if (verbose) cout << "\t\tWith no exceptions." << endl;
        {
            for (int ne = 0; ne <= NUM_VALUES; ++ne) {
                bdem_ElemTypeArray A;
                for (int i = 0; i < ne; i++) {
                    A.append(VALUES[i]);
                }
                if (veryVerbose) { P_(ne); P(A); }
                const int NUM_BLOCKS = testAllocator.numBlocksInUse();
                const int NUM_BYTES  = testAllocator.numBytesInUse();
                Obj mX(A, typesLookupTable, POOL_OPT, DEL_HINT, Z);
                const Obj &X = mX;
                LOOP_ASSERT(ne, NUM_BLOCKS < testAllocator.numBlocksInUse());
                LOOP_ASSERT(ne, NUM_BYTES  < testAllocator.numBytesInUse());
                LOOP_ASSERT(ne, DEL_HINT == X.infrequentDeleteHint());
                LOOP_ASSERT(ne, POOL_OPT == X.allocationMode());
                LOOP_ASSERT(ne, Z        == X.allocator());
                LOOP_ASSERT(ne, ne       == X.columnTypes().length());
                for (int i = 0; i < ne; ++i) {
                     LOOP2_ASSERT(ne, i, A[i] == X.columnTypes().getType(i));
                }
            }
        }

        if (verbose) cout << "\t\tWith exceptions." << endl;
        {
            for (int ne = 0; ne <= NUM_VALUES; ++ne) {
              bdem_ElemTypeArray A;
              for (int i = 0; i < ne; i++) {
                  A.append(VALUES[i]);
              }
              if (veryVerbose) { P_(ne); P(A); }
              BEGIN_BDEMA_EXCEPTION_TEST {
                const int NUM_BLOCKS = testAllocator.numBlocksInUse();
                const int NUM_BYTES  = testAllocator.numBytesInUse();
                Obj mX(A, typesLookupTable, POOL_OPT, DEL_HINT, Z);
                const Obj &X = mX;
                LOOP_ASSERT(ne, NUM_BLOCKS < testAllocator.numBlocksInUse());
                LOOP_ASSERT(ne, NUM_BYTES  < testAllocator.numBytesInUse());
                LOOP_ASSERT(ne, DEL_HINT == X.infrequentDeleteHint());
                LOOP_ASSERT(ne, POOL_OPT == X.allocationMode());
                LOOP_ASSERT(ne, Z        == X.allocator());
                LOOP_ASSERT(ne, ne       == X.columnTypes().length());
                for (int i = 0; i < ne; ++i) {
                     LOOP2_ASSERT(ne, i, A[i] == X.columnTypes().getType(i));
                }
              } END_BDEMA_EXCEPTION_TEST
            }
        }

        if (verbose) cout << "\tWith no allocator (exercise only)." << endl;
        {
            for (int ne = 0; ne <= NUM_VALUES; ++ne) {
                bdem_ElemTypeArray A;
                for (int i = 0; i < ne; i++) {
                    A.append(VALUES[i]);
                }
                if (veryVerbose) { P_(ne); P(A); }
                Obj mX(A, typesLookupTable, POOL_OPT, DEL_HINT, 0);
                const Obj &X = mX;
                LOOP_ASSERT(ne, 0  == X.allocator());
                LOOP_ASSERT(ne, ne == X.columnTypes().length());
                for (int i = 0; i < ne; ++i) {
                    LOOP2_ASSERT(ne, i, A[i] == X.columnTypes().getType(i));
                }
            }
        }

        if (verbose)
            cout << "\tWith a buffer allocator (exercise only)." << endl;
        {
            char memory[4096];
            bdema_BufferAllocator a(memory, sizeof memory);
            for (int ne = 0; ne <= NUM_VALUES; ++ne) {
                bdem_ElemTypeArray A;
                for (int i = 0; i < ne; i++) {
                    A.append(VALUES[i]);
                }
                if (veryVerbose) { P_(ne); P(A); }
                Obj *doNotDelete = new(a.allocate(sizeof(Obj)))
                              Obj(A, typesLookupTable, POOL_OPT, DEL_HINT, &a);
                Obj &mX = *doNotDelete;  const Obj &X = mX;
                LOOP_ASSERT(ne, &a == X.allocator());
                LOOP_ASSERT(ne, ne == X.columnTypes().length());
                for (int i = 0; i < ne; ++i) {
                    LOOP2_ASSERT(ne, i, A[i] == X.columnTypes().getType(i));
                }
            }
            // No destructor is called; will produce memory leak in purify
            // if internal allocators are not hooked up properly.
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING CAPACITY-RESERVING CONSTRUCTOR
        // Concerns:
        //   1. The initial value is correct (empty).
        //   2. The initial capacity is correct.
        //   3. The constructor is exception neutral w.r.t. allocation.
        //   4. The internal memory management system is hooked up properly
        //      so that *all* internally allocated memory draws from a
        //      user-supplied allocator whenever one is specified.
        //   5. The string usage hint, pooling option, and memory allocator are
        //      properly recorded in the object.
        //
        // Plan:
        //   In a loop, use the capacity-reserving constructor to create empty
        //   objects with increasing initial capacity.  Verify that each object
        //   has the same value as a control default object.  Then append as
        //   many values as the requested initial capacity and use
        //   'bdema_TestAllocator' to verify that no additional allocations
        //   have occurred.  Perform each test in the standard 'bdema'
        //   exception-testing macro block.
        //
        //   Concern 5 is addressed by applying the suite of direct accessors
        //   to the control and test objects.
        //
        //   White-Box Testing: Repeat the constructor test initially
        //   specifying no allocator and again specifying a static buffer
        //   allocator.  These tests (without specifying a
        //   'bdema_TestAllocator') cannot confirm correct capacity-reserving
        //   behavior, but can test for rudimentary correct object behavior
        //   via the destructor and Purify.  Note: When tested stand-alone,
        //   'bdem_RowDef' works correctly when passed a null allocator.
        //   However, clients are expected to instantiate 'bdem_RowDef'
        //   with a non-null allocator.
        //
        // Testing:
        //   bdem_RowDef(int ne, int option, int hint, *ba);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Capacity-Reserving Constructor" << endl
                          << "======================================" << endl;

        if (verbose) cout <<
            "\nTesting 'bdem_RowDef(ne, option, hint, ba)' ctor" << endl;

        const Obj W(0, POOL_OPT, DEL_HINT, Z);  // control object
        ASSERT(DEL_HINT == W.infrequentDeleteHint());
        ASSERT(POOL_OPT == W.allocationMode());
        ASSERT(Z        == W.allocator());
        ASSERT(0        == W.columnTypes().length());

        const int MAX_NUM_ELEMS = 9;

        if (verbose) cout << "\tPassing in an allocator." << endl;
        if (verbose) cout << "\t\tWith no exceptions." << endl;
        {
            for (int ne = 0; ne <= MAX_NUM_ELEMS; ++ne) {
                if (veryVerbose) { cout << "\t\t"; P(ne) }
                Obj mX(ne, POOL_OPT, DEL_HINT, Z);
                const Obj &X = mX;
                LOOP_ASSERT(ne, DEL_HINT        == X.infrequentDeleteHint());
                LOOP_ASSERT(ne, POOL_OPT        == X.allocationMode());
                LOOP_ASSERT(ne, Z               == X.allocator());
                LOOP_ASSERT(ne, 0               == X.columnTypes().length());
                LOOP_ASSERT(ne, W.columnTypes() == X.columnTypes());
                const int NUM_BLOCKS = testAllocator.numBlocksInUse();
                const int NUM_BYTES  = testAllocator.numBytesInUse();
                for (int i = 0; i < ne; ++i) {
                    mX.columnTypes().append(typesLookupTable[VALUES[i]]);
                }
                LOOP_ASSERT(ne, NUM_BLOCKS == testAllocator.numBlocksInUse());
                LOOP_ASSERT(ne, NUM_BYTES  == testAllocator.numBytesInUse());
                LOOP_ASSERT(ne, DEL_HINT   == X.infrequentDeleteHint());
                LOOP_ASSERT(ne, POOL_OPT   == X.allocationMode());
                LOOP_ASSERT(ne, Z          == X.allocator());
                LOOP_ASSERT(ne, ne         == X.columnTypes().length());
                if (0!=ne) LOOP_ASSERT(ne, W.columnTypes() != X.columnTypes());
            }
        }

        if (verbose) cout << "\t\tWith exceptions." << endl;
        {
            for (int ne = 0; ne <= MAX_NUM_ELEMS; ++ne) {
              if (veryVerbose) { cout << "\t\t"; P(ne) }
              BEGIN_BDEMA_EXCEPTION_TEST {
                Obj mX(ne, POOL_OPT, DEL_HINT, Z);
                const Obj &X = mX;
                LOOP_ASSERT(ne, DEL_HINT        == X.infrequentDeleteHint());
                LOOP_ASSERT(ne, POOL_OPT        == X.allocationMode());
                LOOP_ASSERT(ne, Z               == X.allocator());
                LOOP_ASSERT(ne, 0               == X.columnTypes().length());
                LOOP_ASSERT(ne, W.columnTypes() == X.columnTypes());
                const int NUM_BLOCKS = testAllocator.numBlocksInUse();
                const int NUM_BYTES  = testAllocator.numBytesInUse();
                for (int i = 0; i < ne; ++i) {
                    mX.columnTypes().append(typesLookupTable[VALUES[i]]);
                }
                LOOP_ASSERT(ne, NUM_BLOCKS == testAllocator.numBlocksInUse());
                LOOP_ASSERT(ne, NUM_BYTES  == testAllocator.numBytesInUse());
                LOOP_ASSERT(ne, DEL_HINT   == X.infrequentDeleteHint());
                LOOP_ASSERT(ne, POOL_OPT   == X.allocationMode());
                LOOP_ASSERT(ne, Z          == X.allocator());
                LOOP_ASSERT(ne, ne         == X.columnTypes().length());
                if (ne!=0) LOOP_ASSERT(ne, W.columnTypes() != X.columnTypes());
              } END_BDEMA_EXCEPTION_TEST
            }
        }

        if (verbose) cout << "\tWith no allocator (exercise only)." << endl;
        {
            for (int ne = 0; ne <= MAX_NUM_ELEMS; ++ne) {
                if (veryVerbose) { cout << "\t\t"; P(ne) }
                Obj mX(ne, POOL_OPT, DEL_HINT, 0);
                const Obj &X = mX;
                LOOP_ASSERT(ne, 0               == X.columnTypes().length());
                LOOP_ASSERT(ne, W.columnTypes() == X.columnTypes());
                for (int i = 0; i < ne; ++i) {
                    mX.columnTypes().append(typesLookupTable[VALUES[i]]);
                }
                LOOP_ASSERT(ne, ne == X.columnTypes().length());
            }
        }

        if (verbose)
            cout << "\tWith a buffer allocator (exercise only)." << endl;
        {
            char memory[4096];
            bdema_BufferAllocator a(memory, sizeof memory);
            for (int ne = 0; ne <= MAX_NUM_ELEMS; ++ne) {
                if (veryVerbose) { cout << "\t\t"; P(ne) }
                Obj *doNotDelete =
                  new(a.allocate(sizeof(Obj))) Obj(ne, POOL_OPT, DEL_HINT, &a);
                Obj &mX = *doNotDelete;  const Obj &X = mX;
                LOOP_ASSERT(ne, 0               == X.columnTypes().length());
                LOOP_ASSERT(ne, W.columnTypes() == X.columnTypes());
                for (int i = 0; i < ne; ++i) {
                    mX.columnTypes().append(typesLookupTable[VALUES[i]]);
                }
                LOOP_ASSERT(ne, ne == X.columnTypes().length());
            }
            // No destructor is called; will produce memory leak in purify
            // if internal allocators are not hooked up properly.
        }
      } break;
      case 3: {
        //--------------------------------------------------------------------
        // TESTING DIRECT ACCESSORS
        // Concerns:
        //   Each of the accessors under test directly returns a data
        //   member, so our concerns are minimal.
        //
        // Plan:
        //   - Test that the direct accessors have the expected signature.
        //   - Create test objects using the 3-argument constructor with
        //     various values for the 3 arguments.  Directly test that the
        //     values returned by the 'infrequentDeleteHint', 'allocationMode',
        //     and 'allocator' accessors match those which were initially
        //     passed to the constructor.
        //   - To test the (const) 'columnTypes' direct accessor, vary the
        //     state of an object's index-offset map within a loop and verify
        //     that the map referenced by the accessor has the expected value
        //     in each iteration.
        //
        // Testing:
        //   const bdem_IdxOffsetMap& columnTypes() const;
        //   int infrequentDeleteHint() const;
        //   int allocationMode() const;
        //   bdema_Allocator *allocator() const;
        //--------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Direct Accessors" << endl
                                  << "========================" << endl;

        if (verbose) cout << endl << "Testing method signatures." << endl;
        {
            const bdem_IdxOffsetMap& (Obj::*cf1)() const = &Obj::columnTypes;
            bdem_AggregateOption::InfrequentDeleteHint (Obj::*cf2)() const =
                                                    &Obj::infrequentDeleteHint;
            bdem_AggregateOption::AllocationMode (Obj::*cf3)() const =
                                                    &Obj::allocationMode;
            bdema_Allocator         *(Obj::*cf4)() const = &Obj::allocator;
        }

        if (verbose) cout << endl
                          << "Testing 'infrequentDeleteHint', 'allocationMode'"
                          << " and 'allocator' methods." << endl;

        {
            static const bdem_AggregateOption::AllocationMode POOL_DATA[] =
            {
                bdem_AggregateOption::PASS_THROUGH,
                bdem_AggregateOption::WRITE_MANY,
                bdem_AggregateOption::WRITE_ONCE,
            };
            const int NUM_POOL_DATA = sizeof POOL_DATA / sizeof POOL_DATA[0];

            char memory[256];
            bdema_BufferAllocator a(memory, sizeof memory);
            static bdema_Allocator *const ALLOC_DATA[] = {
                0,
                Z,
                &testAllocator,
                &a,
            };
            const int NUM_ALLOC_DATA =
                                      sizeof ALLOC_DATA / sizeof ALLOC_DATA[0];

            for (int i = 0; i < NUM_POOL_DATA; ++i) {
                for (int j = 0; j < NUM_HINT_DATA; ++j) {
                    for (int k = 0; k < NUM_ALLOC_DATA; ++k) {
                        Obj mX(POOL_DATA[i], HINT_DATA[j], ALLOC_DATA[k]);
                        const Obj &X = mX;
                        ASSERT(POOL_DATA[i]   == X.allocationMode());
                        ASSERT(HINT_DATA[j]   == X.infrequentDeleteHint());
                        ASSERT(ALLOC_DATA[k]  == X.allocator());
                    }
                }
            }
        }

        if (verbose) cout << endl
                          << "Testing 'const' 'columnTypes' method." << endl;

        {
            bdem_IdxOffsetMap M;  // control index-offset map
            Obj mX(POOL_OPT, DEL_HINT, Z);
            const Obj &X = mX;
            ASSERT(M == X.columnTypes());
            for (int i = 0; i < NUM_VALUES; i++) {
                bdem_IdxOffsetMap& mXM = mX.columnTypes();
                mXM.append(typesLookupTable[VALUES[i]]);
                M.append(typesLookupTable[VALUES[i]]);
                LOOP_ASSERT(i, M == X.columnTypes());
            }
        }
      } break;
      case 2: {
        //--------------------------------------------------------------------
        // BOOTSTRAP TEST
        // Concerns:
        //   1. The 3-argument 'bdem_RowDef' constructor works properly:
        //      a. The initial value is correct.
        //      b. The constructor is exception neutral w.r.t. memory
        //         allocation.
        //      c. The internal memory management system is hooked up properly
        //         so that *all* internally allocated memory draws from a
        //         user-supplied allocator whenever one is specified.
        //      d. The string usage hint, pooling option, and memory allocator
        //         are properly recorded in the object.
        //   2. The destructor works properly as implicitly tested in the
        //      various scopes of this test and in the presence of exceptions.
        //   3. The (non-const) 'columnTypes' manipulator works properly.
        //
        // Plan:
        //   Create a test object using the 3-argument constructor: 1) without
        //   exceptions and 2) in the presence of exceptions during memory
        //   allocations using a 'bdema_TestAllocator' and varying its
        //   *allocation* *limit*.  In a loop, obtain a reference to the
        //   modifiable offset-index map held by the object using the
        //   'columnTypes' method.   Append an element to the map in each
        //   iteration and verify that the object has the expected value.
        //   When the object goes out of scope, verify that the destructor
        //   properly dealloates all memory that had been allocated to it.
        //
        //   Concern 1d. is addressed by applying the suite of direct
        //   accessors to the test objects.
        //
        //   White-Box Testing: Repeat the constructor test initially
        //   specifying no allocator and again specifying a static buffer
        //   allocator.  These test for rudimentary correct object behavior
        //   via the destructor and Purify.  Note: When tested stand-alone,
        //   'bdem_RowDef' works correctly when passed a null allocator.
        //   However, clients are expected to instantiate 'bdem_RowDef'
        //   with a non-null allocator.
        //
        // Testing:
        //   bdem_RowDef(int option, int hint, bdema_Allocator *ba);
        //   ~bdem_RowDef();
        //   bdem_IdxOffsetMap& columnTypes();
        //--------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Primary Manipulators" << endl
                          << "============================" << endl;

        if (verbose) cout <<
            "\nTesting 'bdem_RowDef(option, hint, ba)' ctor" << endl;

        if (verbose) cout << "\tPassing in an allocator." << endl;
        if (verbose) cout << "\t\tWith no exceptions." << endl;
        {
            const int NUM_BLOCKS = testAllocator.numBlocksInUse();
            const int NUM_BYTES  = testAllocator.numBytesInUse();
            {
                bdem_IdxOffsetMap M;  // control index-offset map
                Obj mX(POOL_OPT, DEL_HINT, Z);
                const Obj &X = mX;
                ASSERT(NUM_BLOCKS < testAllocator.numBlocksInUse());
                ASSERT(NUM_BYTES  < testAllocator.numBytesInUse());
                ASSERT(DEL_HINT  == X.infrequentDeleteHint());
                ASSERT(POOL_OPT  == X.allocationMode());
                ASSERT(Z         == X.allocator());
                ASSERT(0         == X.columnTypes().length());
                for (int i = 0; i < NUM_VALUES; i++) {
                    bdem_IdxOffsetMap& mXM = mX.columnTypes();
                    mXM.append(typesLookupTable[VALUES[i]]);
                    LOOP_ASSERT(i, i + 1 == X.columnTypes().length());
                    M.append(typesLookupTable[VALUES[i]]);
                    LOOP_ASSERT(i, M == X.columnTypes());
                }
            }
            ASSERT(NUM_BLOCKS == testAllocator.numBlocksInUse());
            ASSERT(NUM_BYTES  == testAllocator.numBytesInUse());
        }

        if (verbose) cout << "\t\tWith exceptions." << endl;
        {
          const int NUM_BLOCKS = testAllocator.numBlocksInUse();
          const int NUM_BYTES  = testAllocator.numBytesInUse();
          BEGIN_BDEMA_EXCEPTION_TEST {
            bdem_IdxOffsetMap M;  // control index-offset map
            Obj mX(POOL_OPT, DEL_HINT, Z);
            const Obj &X = mX;
            ASSERT(NUM_BLOCKS < testAllocator.numBlocksInUse());
            ASSERT(NUM_BYTES  < testAllocator.numBytesInUse());
            ASSERT(DEL_HINT  == X.infrequentDeleteHint());
            ASSERT(POOL_OPT  == X.allocationMode());
            ASSERT(Z         == X.allocator());
            ASSERT(0         == X.columnTypes().length());
            for (int i = 0; i < NUM_VALUES; i++) {
                bdem_IdxOffsetMap& mXM = mX.columnTypes();
                mXM.append(typesLookupTable[VALUES[i]]);
                LOOP_ASSERT(i, i + 1 == X.columnTypes().length());
                M.append(typesLookupTable[VALUES[i]]);
                LOOP_ASSERT(i, M == X.columnTypes());
            }
          } END_BDEMA_EXCEPTION_TEST
          ASSERT(NUM_BLOCKS == testAllocator.numBlocksInUse());
          ASSERT(NUM_BYTES  == testAllocator.numBytesInUse());
        }

        if (verbose) cout << "\tWith no allocator (exercise only)." << endl;
        {
            bdem_IdxOffsetMap M;  // control index-offset map
            Obj mX(POOL_OPT, DEL_HINT, 0);  const Obj &X = mX;
            ASSERT(0 == X.allocator());
            ASSERT(0 == X.columnTypes().length());
            for (int i = 0; i < NUM_VALUES; i++) {
                bdem_IdxOffsetMap& mXM = mX.columnTypes();
                mXM.append(typesLookupTable[VALUES[i]]);
                LOOP_ASSERT(i, i + 1 == X.columnTypes().length());
                M.append(typesLookupTable[VALUES[i]]);
                LOOP_ASSERT(i, M == X.columnTypes());
            }
        }

        if (verbose)
            cout << "\tWith a buffer allocator (exercise only)." << endl;
        {
            bdem_IdxOffsetMap M;  // control index-offset map
            char memory[4096];
            bdema_BufferAllocator a(memory, sizeof memory);
            Obj *doNotDelete =
                      new(a.allocate(sizeof(Obj))) Obj(POOL_OPT, DEL_HINT, &a);
            Obj &mX = *doNotDelete;  const Obj &X = mX;
            ASSERT(&a == X.allocator());
            ASSERT(0  == X.columnTypes().length());
            for (int i = 0; i < NUM_VALUES; i++) {
                bdem_IdxOffsetMap& mXM = mX.columnTypes();
                mXM.append(typesLookupTable[VALUES[i]]);
                LOOP_ASSERT(i, i + 1 == X.columnTypes().length());
                M.append(typesLookupTable[VALUES[i]]);
                LOOP_ASSERT(i, M == X.columnTypes());
            }
            // No destructor is called; will produce memory leak in purify
            // if internal allocators are not hooked up properly.
        }
      } break;
      case 1: {
        //--------------------------------------------------------------------
        // BREATHING TEST
        // Concerns:
        //   We are concerned that the basic functionality of 'bdem_RowDef'
        //   works properly.
        //
        // Plan:
        //   We exercise basic functionality in 6 steps, inspired by the
        //   'bdem_IdxOffsetMap' test driver's 12-step breathing test from
        //   which we've removed steps that aren't relevant to 'bdem_RowfDef',
        ///  namely those steps which involve copy construction and assignment.
        //
        //   We will use 2 objects for our tests, x1 and x2.
        //
        //   After each step, we will confirm that: a) the object we just
        //   modified is in the expected state (by checking the state using
        //   direct accessors), and b) the object is equal/not equal to the
        //   other existing object.
        //
        //   The exercises are as follows:
        //      1) Build x1 using 3-argument constructor.
        //          a)  0 == x1.columnTypes().length()
        //          b) no other object to compare yet
        //      2) Build x2 using 3-argument constructor.
        //          a)  0 == x2.columnTypes().length()
        //          b) x1 == x2
        //      3) Append a CHAR to index-offset map of x1.
        //          a)  1 == x1.columnTypes().length()
        //          b) x1 != x2
        //      4) Append a CHAR to index-offset map of x2.
        //          a)  1 == x2.columnTypes().length()
        //          b) x1 == x2
        //      5) Reset x1 to its initial state.
        //          a)  0 == x1.columnTypes().length()
        //          b) x1 != x2
        //      6) Re-append a CHAR to index-offset map of x1.
        //          a)  1 == x1.columnTypes().length()
        //          b) x1 == x2
        //
        // Testing:
        //   This breathing test exercises basic functionality as a preliminary
        //   to the thorough testing done in subsequent cases.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Breathing Test" << endl
                          << "==============" << endl;

        if (verbose) cout << "\n 1. Create an object x1 (3-arg. ctor)."
                             "\t\t{ x1: }" << endl;
        Obj x1(POOL_OPT, DEL_HINT, Z);  const Obj& X1 = x1;
        ASSERT(DEL_HINT == X1.infrequentDeleteHint());
        ASSERT(POOL_OPT == X1.allocationMode());
        ASSERT(Z        == X1.allocator());
        ASSERT(0        == X1.columnTypes().length());

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) {
            cout << "\n 2. Create a second object x2 (3-arg. ctor)."
                    "\t{ x1:  x2: }" << endl;
        }
        Obj x2(POOL_OPT, DEL_HINT, Z);  const Obj& X2 = x2;
        ASSERT(DEL_HINT         == X2.infrequentDeleteHint());
        ASSERT(POOL_OPT         == X2.allocationMode());
        ASSERT(Z                == X2.allocator());
        ASSERT(0                == X2.columnTypes().length());
        ASSERT(DEL_HINT         == X1.infrequentDeleteHint());
        ASSERT(POOL_OPT         == X1.allocationMode());
        ASSERT(Z                == X1.allocator());
        ASSERT(0                == X1.columnTypes().length());
        ASSERT(X1.columnTypes() == X2.columnTypes());

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Append a CHAR to index-offset map of x1."
                             "\t\t\t{ x1:V x2: }" << endl;
        x1.columnTypes().append(typesLookupTable[bdem_ElemType::CHAR]);
        ASSERT(DEL_HINT            == X1.infrequentDeleteHint());
        ASSERT(POOL_OPT            == X1.allocationMode());
        ASSERT(Z                   == X1.allocator());
        ASSERT(1                   == X1.columnTypes().length());
        ASSERT(X1.columnTypes()    != X2.columnTypes());
        ASSERT(bdem_ElemType::CHAR == X1.columnTypes().getType(0));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4. Append a CHAR to index-offset map of x2."
                             "\t\t\t{ x1:V x2:V }" << endl;
        x2.columnTypes().append(typesLookupTable[bdem_ElemType::CHAR]);
        ASSERT(DEL_HINT            == X2.infrequentDeleteHint());
        ASSERT(POOL_OPT            == X2.allocationMode());
        ASSERT(Z                   == X2.allocator());
        ASSERT(1                   == X2.columnTypes().length());
        ASSERT(X1.columnTypes()    == X2.columnTypes());
        ASSERT(bdem_ElemType::CHAR == X1.columnTypes().getType(0));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5. Empty x1 using 'removeAll'."
                             "\t\t\t{ x1:  x2:V }" << endl;
        x1.columnTypes().removeAll();
        ASSERT(DEL_HINT            == X1.infrequentDeleteHint());
        ASSERT(POOL_OPT            == X1.allocationMode());
        ASSERT(Z                   == X1.allocator());
        ASSERT(0                   == X1.columnTypes().length());
        ASSERT(X1.columnTypes()    != X2.columnTypes());
        ASSERT(bdem_ElemType::CHAR == X2.columnTypes().getType(0));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose)
            cout << "\n 6. Append a CHAR back to index-offset map of x1."
                    "\t\t\t{ x1:V x2:V }" << endl;
        x1.columnTypes().append(typesLookupTable[bdem_ElemType::CHAR]);
        ASSERT(DEL_HINT            == X1.infrequentDeleteHint());
        ASSERT(POOL_OPT            == X1.allocationMode());
        ASSERT(Z                   == X1.allocator());
        ASSERT(1                   == X1.columnTypes().length());
        ASSERT(X1.columnTypes()    == X2.columnTypes());
        ASSERT(bdem_ElemType::CHAR == X1.columnTypes().getType(0));
      } break;
#endif
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
