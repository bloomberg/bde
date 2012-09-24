// bslalg_dequeiterator.t.cpp                                         -*-C++-*-

#include <bslalg_dequeiterator.h>
#include <bslalg_scalarprimitives.h>
#include <bslalg_scalardestructionprimitives.h>

#include <bslmf_issame.h>

#include <cstring>
#include <cstdlib>
#include <cstdio>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test provides an iterator, which is an in-core
// value-semantic type.  It has two implementations (one for several elements
// per block, the other for when there is a single element per block), and so
// each test case must make sure to exercise both implementations.  We have
// chosen as *primary* *manipulators* the constructor taking a pointer to block
// and a pointer to a value within that block, 'operator++', and 'operator--',
// to be used by the generator functions 'g' and 'gg'.   In order to have the
// iterators point to a range, additional helper functions are provided to
// create a range of block pointers and blocks (taking care that the block are
// not allocated sequentially in the block sequence).  The basic accessors are
// 'blockPtr' and 'valuePtr', from which all the other accessors can be
// computed.  This test driver is for now TBD.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST

//==========================================================================
//                  STANDARD bsl ASSERT TEST MACRO
//--------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

namespace {
void aSsErT(int c, const char *s, int i) {
    if (c) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//--------------------------------------------------------------------------
// NOTE: This implementation of LOOP_ASSERT macros must use printf since
//       cout uses new and be called during exception testing.
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { printf("%s: %d\n", #I, I); aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) dbg_print(#X " = ", X, "\n")  // Print identifier and value.
#define P_(X) dbg_print(#X " = ", X, ", ") // P(X) without '\n'
#define Q(X) printf("<| " #X " |>\n");  // Quote identifier literally.
#define L_ __LINE__                           // current Line number
#define T_ printf("\t");             // Print a tab (w/o newline)

//=============================================================================
//                      GLOBAL PRINT FUNCTIONS
//-----------------------------------------------------------------------------

// Fundamental-type-specific print functions.
inline void dbg_print(char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(unsigned char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(signed char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(short val) { printf("%d", (int)val); fflush(stdout); }
inline void dbg_print(unsigned short val) {
    printf("%d", (int)val); fflush(stdout);
}
inline void dbg_print(int val) { printf("%d", val); fflush(stdout); }
inline void dbg_print(size_t val) { printf("%u", val); fflush(stdout); }
inline void dbg_print(float val) {
    printf("'%f'", (double)val); fflush(stdout);
}
inline void dbg_print(double val) { printf("'%f'", val); fflush(stdout); }
inline void dbg_print(const char* s) { printf("\"%s\"", s); fflush(stdout); }

// Iterator-specific print function.
template <class VALUE_TYPE, int BLOCK_LENGTH>
void
dbg_print(const bslalg::DequeIterator<VALUE_TYPE, BLOCK_LENGTH>& iter)
{
    if (iter.blockPtr() && iter.valuePtr()) {
#ifdef BSLS_PLATFORM_CPU_64_BIT
        printf("[0x%06p,0x%016p]", iter.blockPtr(), iter.valuePtr());
        fflush(stdout);
#else
        printf("[0x%08x,0x%08x]", iter.blockPtr(), iter.valuePtr());
        fflush(stdout);
#endif
    }
    else {
        printf("[0x00000000,0x00000000]");
        fflush(stdout);
    }
}


// Generic debug print function (3-arguments).
template <typename T>
void dbg_print(const char* s, const T& val, const char* nl)
{
    printf("%s", s); dbg_print(val);
    printf(nl);
    fflush(stdout);
}

//=============================================================================
//                  GLOBAL VARIABLES AND TYPES FOR TESTING
//-----------------------------------------------------------------------------

int verbose;
int veryVerbose;
int veryVeryVerbose;

char* globalArena1[65536];  // needed for 'blocks' in 'getValues' below
char* globalArena2[1024];   // needed for 'blockPtrs' in 'getValues' below

//=============================================================================
//                             TEST DRIVER
//-----------------------------------------------------------------------------

template <class VALUE_TYPE, int BLOCK_LENGTH>
struct TestDriver {

    // PUBLIC TYPES
    typedef bslalg::DequeImpUtil<VALUE_TYPE, BLOCK_LENGTH>        DequeImpUtil;
    typedef typename DequeImpUtil::Block                          Block;
    typedef typename DequeImpUtil::BlockPtr                       BlockPtr;

    typedef bslalg::DequeIterator<VALUE_TYPE, BLOCK_LENGTH>       Obj;
        // Type of iterator object under test.

    // TEST APPARATUS
    static int getValues(const Obj **values);
        // Load the specified 'values' with the address of an array containing
        // initialized values of the test object and return the length of that
        // array.

    static int ggg(Obj *object, const char *spec, int verboseFlag = 1);
        // Configure the specified 'object' according to the specified 'spec',
        // using only the primary manipulator function 'push_back' and
        // white-box manipulator 'clear'.  Optionally specify a zero
        // 'verboseFlag' to suppress 'spec' syntax error messages.  Return the
        // index of the first invalid character, and a negative value
        // otherwise.  Note that this function is used to implement 'gg' as
        // well as allow for verification of syntax error detection.

    static Obj& gg(Obj *object, const char *spec);
        // Return, by reference, the specified object with its value adjusted
        // according to the specified 'spec'.

    static Obj g(const char *spec);
        // Return, by value, a new object corresponding to the specified
        // 'spec'.

    // TEST CASES
    static void testCase11();
        // Test accessors.

    static void testCase10();
        // Test streaming functionality.  This test case tests nothing.

    static void testCase9();
        // Test assignment operator ('operator=').

    static void testCase8();
        // Test generator function 'g()'.

    static void testCase7();
        // Test copy constructor.

    static void testCase6();
        // Test equality operator ('operator==').

    static void testCase5();
        // Test output (<<) operator.  This test case tests nothing.

    static void testCase4();
        // Test basic accessors ('size' and 'operator[]').

    static void testCase3();
        // Test generator functions 'ggg()' and 'gg()'.

    static void testCase2();
        // Test primary manipulators ('push_back' and 'clear').

    static void testCase1();
        // Breathing test.  This test *exercises* basic functionality but
        // *test* nothing.
};

template <class VALUE_TYPE, int BLOCK_LENGTH>
int
TestDriver<VALUE_TYPE, BLOCK_LENGTH>::getValues(const Obj **valuesPtr)
{
    const int NUM_BLOCKS = 5;
    const int NUM_VALUES = NUM_BLOCKS * BLOCK_LENGTH;

    // We use global arenas because static (non-const) arrays don't play well
    // inside a function template.  For safety, we always fill the arenas with
    // junk and use padding at the boundaries.

    std::memset(globalArena1, 0xa7, sizeof globalArena1);
    std::memset(globalArena2, 0xa9, sizeof globalArena2);

    Block (&blocks)[NUM_BLOCKS] = *reinterpret_cast<Block(*)[NUM_BLOCKS]>
                                                   ((char**)globalArena1 + 16);
    Block (&BLOCKS)[NUM_BLOCKS] = blocks;

    Block *(&blockPtrs)[NUM_BLOCKS] = *reinterpret_cast<Block *(*)[NUM_BLOCKS]>
                                                   ((char**)globalArena2 + 16);
    Block *(&BLOCK_PTRS)[NUM_BLOCKS] = blockPtrs;

    static Obj values[NUM_VALUES];

    for (int i = 0, k = 0; i < NUM_BLOCKS; ++i) {
        BLOCK_PTRS[i] = &BLOCKS[(NUM_BLOCKS + 2 - i) % NUM_BLOCKS];

        for (int j = 0; j < BLOCK_LENGTH; ++j) {
            blockPtrs[i]->d_data[j] = VALUE_TYPE('A' + k);
            values[k++] = Obj(&blockPtrs[i], &blockPtrs[i]->d_data[j]);
        }
    }

    *valuesPtr = values;
    return NUM_VALUES;
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
void TestDriver<VALUE_TYPE, BLOCK_LENGTH>::testCase1()
{
    // --------------------------------------------------------------------
    // BREATHING TEST:
    //   We want to exercise basic value-semantic functionality.  In
    //   particular we want to demonstrate a base-line level of correct
    //   operation of the following methods and operators:
    //      - default and copy constructors (and also the destructor)
    //      - the assignment operator (including aliasing)
    //      - equality operators: 'operator=='
    //      - primary manipulators: 'Obj(blockPtr,valuePtr)' and 'operator++'
    //      - basic accessors: 'blockPtr' and 'valuePtr'
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
    // 1) Create an object x1 (default ctor).         { x1: }
    // 2) Create a second object x2 (copy from x1).   { x1: x2: }
    // 3) Set x1 to the first iterator value.         { x1:A x2: }
    // 4) Set x2 point to the same iterator value.    { x1:A x2:A }
    // 5) Increment x2.                               { x1:A x2:B }
    //    Decrement x2.                               { x1:A x2:A }
    //    Re-increment x2.                            { x1:A x2:B }
    // 6) Reset x1 to a default state.                { x1: x2:B }
    // 7) Create a third object x3 (default ctor).    { x1: x2:B x3: }
    // 8) Create a forth object x4 (copy of x2).      { x1: x2:B x3: x4:B }
    // 9) Assign x2 = x1 (non-empty becomes empty).   { x1: x2: x3: x4:B }
    // 10) Assign x3 = x4 (empty becomes non-empty).  { x1: x2: x3:B x4:B }
    // 11) Assign x4 = x4 (aliasing).                 { x1: x2: x3:B x4:B }
    //
    // Testing:
    //   This "test" *exercises* basic functionality.
    // --------------------------------------------------------------------

    const Obj         *values     = 0;
    const Obj *const&  VALUES     = values;
    const int          NUM_VALUES = getValues(&values);
    (void)NUM_VALUES;

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n1) Create an object x1 (ctor from A)."
                        "\t\t\t{ x1: }\n");

    Obj mX1;  const Obj& X1 = mX1;
    if (verbose) { T_;  T_; P(X1); }

    if (verbose) printf("\ta) Check initial state of x1.\n");

    ASSERT(0 == X1.blockPtr());
    ASSERT(0 == X1.valuePtr());

    if (verbose) printf("\tb) Try equality operators: x1 <op> x1.\n");
    ASSERT(  X1 == X1 );

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 2) Create a second object x2 (copy from x1)."
                         "\t\t{ x1: x2: }\n");
    Obj mX2(X1);  const Obj& X2 = mX2;
    if (verbose) { T_;  P(X2); }

    if (verbose) printf(
        "\ta) Check the initial state of x2.\n");
    ASSERT(0 == X2.blockPtr());
    ASSERT(0 == X2.valuePtr());

    if (verbose) printf(
        "\tb) Try equality operators: x2 <op> x1, x2.\n");
    ASSERT(  X2 == X1 );
    ASSERT(  X2 == X2 );

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 3) Set x1 to the first iterator value."
                        "\t\t\t{ x1:A x2: }\n");

    bslalg::ScalarDestructionPrimitives::destroy(&mX1);
    bslalg::ScalarPrimitives::construct(&mX1,
                                        VALUES[0].blockPtr(),
                                        VALUES[0].valuePtr());
    if (verbose) { T_;  P(X1); }

    if (verbose) printf(
        "\ta) Check new state of x1.\n");
    ASSERT(VALUES[0].blockPtr() == X1.blockPtr());
    ASSERT(VALUES[0].valuePtr() == X1.valuePtr());

    if (verbose) printf(
        "\tb) Try equality operators: x1 <op> x1, x2.\n");
    ASSERT(  X1 == X1 );
    ASSERT(!(X1 == X2));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 4) Set x2 to the same iterator value."
                         "\t\t{ x1:A x2:A }\n");
    mX2 = VALUES[0];
    if (verbose) { T_;  P(X2); }

    if (verbose) printf(
        "\ta) Check new state of x2.\n");
    ASSERT(VALUES[0].blockPtr() == X2.blockPtr());
    ASSERT(VALUES[0].valuePtr() == X2.valuePtr());

    if (verbose) printf(
        "\tb) Try equality operators: x2 <op> x1, x2.\n");
    ASSERT(  X2 == X1 );
    ASSERT(  X2 == X2 );

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 5a) Increment x2."
                         "\t\t{ x1:A x2:B }\n");
    ++mX2;
    if (verbose) { T_;  P(X2); }

    if (verbose) printf(
        "\ta) Check new state of x2.\n");
    ASSERT(VALUES[1].blockPtr() == X2.blockPtr());
    ASSERT(VALUES[1].valuePtr() == X2.valuePtr());

    if (verbose) printf(
        "\tb) Try equality operators: x2 <op> x1, x2.\n");
    ASSERT(!(X2 == X1));
    ASSERT(  X2 == X2 );

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 5b) Decrement x2."
                         "\t\t{ x1:A x2:A }\n");
    --mX2;
    if (verbose) { T_;  P(X2); }

    if (verbose) printf(
        "\ta) Check new state of x2.\n");
    ASSERT(VALUES[0].blockPtr() == X2.blockPtr());
    ASSERT(VALUES[0].valuePtr() == X2.valuePtr());

    if (verbose) printf(
        "\tb) Try equality operators: x2 <op> x1, x2.\n");
    ASSERT(  X2 == X1 );
    ASSERT(  X2 == X2 );

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 5c) Re-increment x2."
                         "\t\t{ x1:A x2:B }\n");
    ++mX2;
    if (verbose) { T_;  P(X2); }

    if (verbose) printf(
        "\ta) Check new state of x2.\n");
    ASSERT(VALUES[1].blockPtr() == X2.blockPtr());
    ASSERT(VALUES[1].valuePtr() == X2.valuePtr());

    if (verbose) printf(
        "\tb) Try equality operators: x2 <op> x1, x2.\n");
    ASSERT(!(X2 == X1));
    ASSERT(  X2 == X2 );

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 6) Reset x1 to default state."
                         "\t\t\t{ x1: x2:B }\n");
    mX1 = Obj();
    if (verbose) { T_;  P(X1); }

    if (verbose) printf(
        "\ta) Check new state of x1.\n");
    ASSERT(0 == X1.blockPtr());
    ASSERT(0 == X1.valuePtr());

    if (verbose) printf(
        "\tb) Try equality operators: x1 <op> x1, x2.\n");
    ASSERT(  X1 == X1 );
    ASSERT(!(X1 == X2));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 7) Create a third object x3 (default ctor)."
                         "\t\t{ x1: x2:B x3: }\n");

    Obj mX3;  const Obj& X3 = mX3;
    if (verbose) { T_;  P(X3); }

    if (verbose) printf(
        "\ta) Check new state of x3.\n");
    ASSERT(0 == X3.blockPtr());
    ASSERT(0 == X3.valuePtr());

    if (verbose) printf(
        "\tb) Try equality operators: x3 <op> x1, x2, x3.\n");
    ASSERT(  X3 == X1 );
    ASSERT(!(X3 == X2));
    ASSERT(  X3 == X3 );

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 8) Create a forth object x4 (copy of x2)."
                         "\t\t{ x1: x2:B x3: x4:B }\n");

    Obj mX4(X2);  const Obj& X4 = mX4;
    if (verbose) { T_;  P(X4); }

    if (verbose) printf(
        "\ta) Check new state of x4.\n");
    ASSERT(VALUES[1].blockPtr() == X4.blockPtr());
    ASSERT(VALUES[1].valuePtr() == X4.valuePtr());

    if (verbose) printf(
        "\tb) Try equality operators: x4 <op> x1, x2, x3, x4.\n");
    ASSERT(!(X4 == X1));
    ASSERT(  X4 == X2 );
    ASSERT(!(X4 == X3));
    ASSERT(  X4 == X4 );

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 9) Assign x2 = x1 (non-empty becomes empty)."
                         "\t\t{ x1: x2: x3: x4:B }\n");

    mX2 = X1;
    if (verbose) { T_;  P(X2); }

    if (verbose) printf(
        "\ta) Check new state of x2.\n");
    ASSERT(0 == X2.blockPtr());
    ASSERT(0 == X2.valuePtr());

    if (verbose) printf(
        "\tb) Try equality operators: x2 <op> x1, x2, x3, x4.\n");
    ASSERT(  X2 == X1 );
    ASSERT(  X2 == X2 );
    ASSERT(  X2 == X3 );
    ASSERT(!(X2 == X4));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n10) Assign x3 = x4 (empty becomes non-empty)."
                         "\t\t{ x1: x2: x3:B x4:B }\n");

    mX3 = X4;
    if (verbose) { T_;  P(X3); }

    if (verbose) printf(
        "\ta) Check new state of x3.\n");
    ASSERT(VALUES[1].blockPtr() == X3.blockPtr());
    ASSERT(VALUES[1].valuePtr() == X3.valuePtr());

    if (verbose) printf(
        "\tb) Try equality operators: x3 <op> x1, x2, x3, x4.\n");
    ASSERT(!(X3 == X1));
    ASSERT(!(X3 == X2));
    ASSERT(  X3 == X3 );
    ASSERT(  X3 == X4 );

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n11. Assign x4 = x4 (aliasing)."
                         "\t\t\t\t{ x1: x2: x3:B x4:B }\n");

    mX4 = X4;
    if (verbose) { T_;  P(X4); }

    if (verbose) printf(
        "\ta) Check new state of x4.\n");
    ASSERT(VALUES[1].blockPtr() == X4.blockPtr());
    ASSERT(VALUES[1].valuePtr() == X4.valuePtr());

    if (verbose)
        printf("\tb) Try equality operators: x4 <op> x1, x2, x3, x4.\n");
    ASSERT(!(X4 == X1));
    ASSERT(!(X4 == X2));
    ASSERT(  X4 == X3 );
    ASSERT(  X4 == X4 );
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
        verbose = argc > 2;
        veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'bslstl::DequeIterator' TEMPLATE
        //
        // Concerns:
        //
        // Plan:  Since the component under test is a template, we cannot use
        //   the table data selection method, but instead use a test macro,
        //   with two parameters containing the value of the template
        //   arguments.  We then instantiate the macro for various parameters
        //   that exercise the various branches and boundary conditions.
        //
        // Testing:
        //   class template 'bslstl::DequeIterator'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'bslalg::DequeIterator"
                            "\n==============================\n");

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING/USAGE TEST
        //
        // Concerns:
        //   That the basic usage is functional and correct.
        //
        // Plan:  Exercise basic usage of this component.
        //
        // Testing:
        //   This test exercises basic usage but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============");

        if (verbose) printf("\nWith five 'char' per block.\n");
        TestDriver<char, 5>::testCase1();

        if (verbose) printf("\nWith two 'char' per block.\n");
        TestDriver<char, 2>::testCase1();

        if (verbose) printf("\nWith one 'char' per block.\n");
        TestDriver<char, 1>::testCase1();

        if (verbose) printf("\nWith five 'int' per block.\n");
        TestDriver<int, 5>::testCase1();

        if (verbose) printf("\nWith two 'int' per block.\n");
        TestDriver<int, 2>::testCase1();

        if (verbose) printf("\nWith one 'int' per block.\n");
        TestDriver<int, 1>::testCase1();

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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
