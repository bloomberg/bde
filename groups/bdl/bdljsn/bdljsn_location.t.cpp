// bdljsn_location.t.cpp                                              -*-C++-*-
#include <bdljsn_location.h>

#include <bsla_maybeunused.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslh_hash.h>     // 'bslh::hashAppend'

#include <bslmf_assert.h>

#include <bsl_cstddef.h>   // 'bsl::size_t'
#include <bsl_cstdint.h>   // 'bsl::uint64_t', 'UINT64_MAX'
#include <bsl_cstdlib.h>   // 'bsl::atoi', 'bsl::realloc', 'bsl::free'
#include <bsl_cstring.h>   // 'bsl::memcpy', 'bsl::memcmp'
#include <bsl_ios.h>       // 'bsl::ios::badbit'
#include <bsl_iostream.h>
#include <bsl_limits.h>    // 'bsls::numeric_limits'
#include <bsl_memory.h>    // 'bsl::uses_allocator'
#include <bsl_utility.h>   // 'bsl::swap'
#include <bsl_sstream.h>   // 'bsl::ostringstream'
#include <bsl_streambuf.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::endl;;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//
// The component under test implements a single, unconstrained
// (value-semantic) attribute class.  The Primary Manipulators and Basic
// Accessors are therefore, respectively, the attribute setters and getters,
// each of which follows our standard unconstrained attribute-type naming
// conventions:
//
// Primary Manipulators:
//: o 'setOffset'
//
// Basic Accessors:
//: o 'offset'
//
// Testing this class follows the usual pattern for testing of a value-semantic
// type with the following provisos:
//
//: o The class has a value constructor that can be used in lieu of a 'gg' test
//:   helper function.
//:
//: o The class is not allocator aware so we need not test for the correct
//:   installation and use of an object allocators nor for exception safety on
//:   failure of that allocator.
//:
//:   o We still do check for gratuitous use of temporary memory from the
//:     default allocator and for any allocation from the global allocator.
//:
//: o The class is has no narrow contracts so negative testing of defensive
//:   checks is not needed.
//:
//:  o There are no move operations.
//
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] Location();
// [ 3] explicit Location(bsl::uint64_t offset);
// [ 6] Location(const Location& original);
// [ 2] ~Location();
//
// MANIPULATORS
// [ 8] Location& operator=(const Location& rhs);
// [ 9] void reset();
// [ 2] void setOffset(bsl::uint64_t value);
//
// [ 7] void swap(Location& other);
//
// ACCESSORS
// [ 2] bsl::uint64_t offset() const;
//
// [ 4] ostream& print(ostream& s, int level = 0, int sPL = 4) const;
//
// FREE OPERATORS
// [ 5] bool operator==(const Location& lhs, const Location& rhs);
// [ 5] bool operator!=(const Location& lhs, const Location& rhs);
// [ 4] ostream& operator<<(bsl::ostream& stream, const Location& object);
//
// FREE FUNCTIONS
// [10] void hashAppend(HASHALG& algorithm, const Location& object);
// [ 7] void swap(Location& a, Location& b);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [11] USAGE EXAMPLE
// [ *] CONCERN: In no case does memory come from the global allocator.
// [ *] CONCERN: In no case does memory come from the default allocator.

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

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                  GLOBAL TYPEDEFS AND CLASSES FOR TESTING
// ----------------------------------------------------------------------------

typedef bdljsn::Location Obj;
typedef bsl::uint64_t    Uint64;
typedef bsl::int64_t     Int64;

class MockAccumulatingHashingAlgorithm {
    // This class implements a mock hashing algorithm that provides a way to
    // accumulate and then examine data that is being passed into hashing
    // algorithms by 'hashAppend'.

    void   *d_data_p;  // Data we were asked to hash
    size_t  d_length;  // Length of the data we were asked to hash

  public:
    MockAccumulatingHashingAlgorithm()
    : d_data_p(0)
    , d_length(0)
        // Create an object of this type.
    {
    }

    ~MockAccumulatingHashingAlgorithm()
        // Destroy this object
    {
        bsl::free(d_data_p);
    }

    void operator()(const void *voidPtr, size_t length)
        // Append the data of the specified 'length' at 'voidPtr' for later
        // inspection.
    {
        d_data_p = bsl::realloc(d_data_p, d_length += length);
        bsl::memcpy(getData() + d_length - length, voidPtr, length);
    }

    char *getData()
        // Return a pointer to the stored data.
    {
        return static_cast<char *>(d_data_p);
    }

    size_t getLength() const
        // Return the length of the stored data.
    {
        return d_length;
    }
};

// FREE OPERATORS
bool operator==(MockAccumulatingHashingAlgorithm& lhs,
                MockAccumulatingHashingAlgorithm& rhs)
    // Return 'true' if the specified 'lhs' and 'rhs' have the same value and
    // 'false' otherwise.  Two objects have the same value if they both have
    // the same length and if they both point to data that compares equal.
    // Note that for simplicity of implementation the arguments are
    // (unconventionally) non-'const'.
{
    if (lhs.getLength() != rhs.getLength()) {
        return false;                                                 // RETURN
    }

    return 0 == bsl::memcmp(lhs.getData(), rhs.getData(), rhs.getLength());
}

// ============================================================================
//                                TYPE TRAITS
// ----------------------------------------------------------------------------

BSLMF_ASSERT( bslmf::IsBitwiseMoveable<Obj>::value);
BSLMF_ASSERT(!bslma::UsesBslmaAllocator<Obj>::value);

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int test = argc > 1 ? bsl::atoi(argv[1]) : 0;

    BSLA_MAYBE_UNUSED const bool             verbose = argc > 2;
    BSLA_MAYBE_UNUSED const bool         veryVerbose = argc > 3;
    BSLA_MAYBE_UNUSED const bool     veryVeryVerbose = argc > 4;
    BSLA_MAYBE_UNUSED const bool veryVeryVeryVerbose = argc > 5;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // Define 'DEFAULT_DATA' that is used by:
    //..
    //   +-------+--------------------------------+
    //   | Case# | Description                    |
    //   +-------+--------------------------------+
    //   |     3 | VALUE CONSTRUCTOR              |
    //   |     6 | COPY CONSTRUCTOR               |
    //   |     7 | SWAP MEMBER AND FREE FUNCTIONS |
    //   |     8 | COPY-ASSIGNMENT OPERATOR       |
    //   |     9 | TEST 'reset'                   |
    //   |    10 | TEST 'hashAppend'              |
    //   +-------+--------------------------------+
    //..

    struct DefaultDataRow {
        int    d_line;           // source line number
        Uint64 d_offset;
    };

    static
    const DefaultDataRow DEFAULT_DATA[] =
    {
        //LINE  OFFSET
        //----  ------------------------------------------
        { L_,                                        0ULL }
      , { L_,                                        1ULL }
      , { L_,   bsl::numeric_limits<Uint64>::max() - 1ULL }
      , { L_,   bsl::numeric_limits<Uint64>::max() - 0ULL }
    };
    enum { DEFAULT_NUM_DATA = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA };


    switch (test) { case 0:
      case 11: {
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

        if (verbose) {
            bsl::cout << bsl::endl
                      << "USAGE EXAMPLE" << bsl::endl
                      << "=============" << bsl::endl;
        }

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::TestAllocatorMonitor  dam(&da);
        bslma::DefaultAllocatorGuard dag(&da);

        bslma::TestAllocator  usageAllocator("usage", veryVeryVeryVerbose);
        bsl::ostringstream    redirected(&usageAllocator);
        bsl::streambuf       *saved = bsl::cout.rdbuf();

        if (!veryVerbose) {
            bsl::cout.rdbuf(redirected.rdbuf());
        } else {
            (void) redirected;
            (void) saved;
        }

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// This example exercises each of the methods of the 'bdljsn::Location' class.
//
// First, create a 'bdljsn::Location' object (having the default value):
//..
    bdljsn::Location locationA;
    ASSERT(0 == locationA.offset());
//..
// Then, set 'locationA' to some other offset:
//..
    locationA.setOffset(1);
    ASSERT(1 == locationA.offset());
//..
// Next, use the value constructor to create a second location having the same
// offset as the first:
//..
        bdljsn::Location locationB(1);
        ASSERT(1         == locationB.offset());
        ASSERT(locationA == locationB);
//..
// Then, set the second location to the maximum offset:
//..
    const bsl::uint64_t maxOffset = bsl::numeric_limits<bsl::uint64_t>::max();

    locationB.setOffset(maxOffset);
    ASSERT(maxOffset == locationB.offset());
//..
// Next, create another 'Location` that is a copy of the one at 'maxOffset':
//..
    bdljsn::Location locationC(locationB);
    ASSERT(locationB == locationC);
//..
// Then, set the first location back to the default state:
//..
    locationA.reset();
    ASSERT(0                  == locationA.offset());
    ASSERT(bdljsn::Location() == locationA);
//..
//  Next, print the value of each:
//..
    bsl::cout << locationA << "\n"
              << locationB << bsl::endl;

    bsl::cout << "\n";

    locationC.print(bsl::cout, 2, 3);
//..
// and observe:
//..
//   0
//   18446744073709551615
//
//        [
//           offset = 18446744073709551615
//        ]
//..
// Finally, set each location equal to the first:
//..
    locationC = locationB = locationA;
    ASSERT(0 == locationA.offset());
    ASSERT(0 == locationB.offset());
    ASSERT(0 == locationC.offset());
//..

        ASSERT(dam.isInUseSame());

        if (!veryVerbose) {
            bsl::cout.rdbuf(saved);

            const char EXPECTED[] = " 0\n"
                                    " 18446744073709551615\n"
                                    "\n"
                                    "      [\n"
                                    "         offset = 18446744073709551615\n"
                                    "      ]\n";

            ASSERTV(EXPECTED,   redirected.str(),
                    EXPECTED == redirected.str());
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TEST 'hashAppend'
        //   Verify that the 'hashAppend' free functions have been implemented
        //   for all of the fundamental types and do not truncate or pass extra
        //   data into the algorithms.
        //
        // Concerns:
        //: 1 The 'hashAppend' function ("the function") uses the specified
        //:   algorithm (and no other).
        //:
        //: 2 The function passes the object's value to the supplied algorithm.
        //
        // Plan:
        //: 1 Use a locally defined algorithm class,
        //:   'MockAccumulatingHashingAlgorithm', to show that 'hashAppend'
        //:   uses the supplied algorithm object.  (C-1)
        //:
        //: 2 The 'MockAccumulatingHashingAlgorithm' functor stores a
        //:   concatenation of each of the input supplied.  This allows one to
        //:   compare the results of independent invocations of the algorithm
        //:   on the object's value with invocations on the object itself.
        //:
        //: 4 In P-2 use elements of different representative values that
        //:   include the extremes of the allowed range.  Any omission,
        //:   duplication, or mis-ordering of element visits would become
        //:   manifest when comparing the concatenations of the two
        //:   computations.  (C-2)
        //
        // Testing:
        //   void hashAppend(HASHALG& algorithm, const Location& object);
        // --------------------------------------------------------------------

        if (verbose) cout << "TEST 'hashAppend'" << endl
                          << "=================" << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::TestAllocatorMonitor  dam(&da);
        bslma::DefaultAllocatorGuard dag(&da);

        const int              NUM_DATA        = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int    LINE   = DATA[ti].d_line;
            const Uint64 OFFSET = DATA[ti].d_offset;

            if (veryVerbose) { T_ P_(LINE) P(OFFSET) }

            Obj mX(OFFSET); const Obj& X = mX;

            MockAccumulatingHashingAlgorithm expd;
            MockAccumulatingHashingAlgorithm calc;

            using bslh::hashAppend;
                    hashAppend(expd, OFFSET);
            bdljsn::hashAppend(calc, X);                                // TEST

            ASSERTV(LINE, OFFSET, expd == calc);
        }

        ASSERT(dam.isInUseSame());
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TEST 'reset'
        //
        // Concerns:
        //: 1 The 'reset' method set an object to the default state
        //:   irrespective of the state of the object.
        //
        // Plan:
        //: 1 Create a series of objects each having a value representative of
        //:   the range the object might hold -- including extreme values.
        //:
        //: 2 For each of those objects, invoke the 'reset' method and confirm
        //:   that the object is in the default state.
        //
        // Testing:
        //   void reset();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST 'reset'" << endl
                          << "============" << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::TestAllocatorMonitor  dam(&da);
        bslma::DefaultAllocatorGuard dag(&da);

        const int              NUM_DATA        = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int    LINE   = DATA[ti].d_line;
            const Uint64 OFFSET = DATA[ti].d_offset;

            if (veryVerbose) { T_ P_(LINE) P(OFFSET) }

            Obj mX(OFFSET); const Obj& X = mX;
            mX.reset();                                                 // TEST
            ASSERTV(OFFSET,  Obj() == X);
        }

        ASSERT(dam.isInUseSame());
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // COPY-ASSIGNMENT OPERATOR
        //   Ensure that we can assign the value of any object of the class to
        //   any object of the class, such that the two objects subsequently
        //   have the same value.
        //
        // Concerns:
        //: 1 The assignment operator can change the value of any modifiable
        //:   target object to that of any source object.
        //:
        //: 2 The signature and return type are standard.
        //:
        //: 3 The reference returned is to the target object (i.e., '*this').
        //:
        //: 4 The value of the source object is not modified.
        //:
        //: 5 Assigning an object to itself behaves as expected (alias-safety).
        //
        // Plan:
        //: 1 Use the address of 'operator=' to initialize a member-function
        //:   pointer having the appropriate signature and return type for the
        //:   copy-assignment operator defined in this component.  (C-2)
        //:
        //: 2 Install a test allocator as the default allocator.
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain.
        //:
        //:     o For this class, there is a single attribute so that table has
        //:       just two rows.
        //:
        //: 4 For each row 'R1' (representing a distinct object value, 'V') in
        //:   the table described in P-3:  (C-1,4)
        //:
        //:   1 Use the value constructor allocator to create
        //:     two 'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
        //:
        //:   2 Execute an inner loop that iterates over each row 'R2'
        //:     (representing a distinct object value, 'W') in the table
        //:     described in P-3:
        //:
        //:   3 For each of the iterations (P-4.2):  (C-1,2,4)
        //:
        //:     2 Use the value constructor and 'oa' to create a modifiable
        //:       'Obj', 'mX', having the value 'W'.
        //:
        //:     4 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-5)
        //:
        //:     5 Use the equality-comparison operator to verify that: (C-1, 4)
        //:
        //:       1 The target object, 'mX', now has the same value as that of
        //:         'Z'.  (C-1)
        //:
        //:       2 'Z' still has the same value as that of 'ZZ'.  (C-4)
        //:
        //: 5 Repeat steps similar to those described in P-2 except that, this
        //:   time, there is no inner loop (as in P-4.2); instead, the source
        //:   object, 'Z', is a reference to the target object, 'mX', and both
        //:   'mX' and 'ZZ' are initialized to have the value 'V'.  For each
        //:   row (representing a distinct object value, 'V') in the table
        //:   described in P-3:  (C-5)
        //:
        //:   2 Use the value constructor to create a modifiable 'Obj'
        //:     'mX'; also use the value constructor and a distinct "scratch"
        //:     allocator to create a 'const' 'Obj' 'ZZ'.
        //:
        //:   3 Let 'Z' be a 'const' reference to 'mX'.
        //:
        //:   5 Verify that the address of the return value is the same as that
        //:     of 'mX'.
        //:
        //:   6 Use the equality-comparison operator to verify that the
        //:     target object, 'Z', still has the same value as that of 'ZZ'.
        //:     (C-10)
        //:
        //: 6 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-3)
        //
        // Testing:
        //   Location& operator=(const Location& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY-ASSIGNMENT OPERATOR" << endl
                          << "========================" << endl;

        if (verbose) cout <<
                 "\nAssign the address of the operator to a variable." << endl;
        {
            typedef Obj& (Obj::*operatorPtr)(const Obj&);

            // Verify that the signature and return type are standard.

            operatorPtr operatorAssignment = &Obj::operator=;

            (void)operatorAssignment;  // quash potential compiler warning
        }

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::TestAllocatorMonitor  dam(&da);
        bslma::DefaultAllocatorGuard dag(&da);

        const int              NUM_DATA        = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int    LINE1   = DATA[ti].d_line;
            const Uint64 OFFSET1 = DATA[ti].d_offset;

            const Obj Z (OFFSET1);
            const Obj ZZ(OFFSET1);

            if (veryVerbose) { T_ P_(LINE1) P_(Z) P(ZZ) }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int    LINE2   = DATA[tj].d_line;
                const Uint64 OFFSET2 = DATA[tj].d_offset;

                {
                    Obj mX(OFFSET2);  const Obj& X = mX;

                    if (veryVerbose) { T_ P_(LINE2) P(X) }

                    ASSERTV(LINE1, LINE2, Z, X, (Z == X) == (LINE1 == LINE2));

                    Obj *mR = &(mX = Z);                            // TEST
                    ASSERTV(LINE1, LINE2,  Z,   X,  Z == X);
                    ASSERTV(LINE1, LINE2, mR, &mX, mR == &mX);

                    ASSERTV(LINE1, LINE2, ZZ, Z, ZZ == Z);
                }
            }

            // self-assignment

            {
                Obj mX(OFFSET1); const Obj ZZ(OFFSET1); const Obj& Z = mX;

                ASSERTV(LINE1, ZZ, Z, ZZ == Z);

                Obj *mR = &(mX = Z);                                    // TEST
                ASSERTV(LINE1, ZZ,   Z, ZZ == Z);
                ASSERTV(LINE1, mR, &mX, mR == &mX);
            }
        }

        ASSERT(dam.isInUseSame());
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // SWAP MEMBER AND FREE FUNCTIONS
        //   Ensure that the free 'swap' function is implemented and can
        //   exchange the values of any two objects.  Ensure that member
        //   'swap' is implemented and can exchange the values of any two
        //   objects that use the same allocator.
        //
        // Concerns:
        //: 1 Both functions exchange the values of the (two) supplied objects.
        //:
        //: 2 Both functions have standard signatures and return types.
        //:
        //: 3 Using either function to swap an object with itself does not
        //:   affect the value of the object (alias-safety).
        //:
        //: 4 The free 'swap' function is discoverable through ADL (Argument
        //:   Dependent Lookup).
        //:
        //: 5 No memory is allocated from the default allocator.
        //
        // Plan:
        //: 1 Use the addresses of the 'swap' member and free functions defined
        //:   in this component to initialize, respectively, member-function
        //:   and free-function pointers having the appropriate signatures and
        //:   return types.  (C-2)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain.
        //:
        //:     o For this class, there is a single attribute so that table has
        //:       just two rows.
        //:
        //: 4 For each row 'R1' in the table of P-3: (C-2..3)
        //:
        //:   2 Use the value constructor to create a modifiable 'Obj', 'mW',
        //:     having the value described by 'R1'; also use the copy
        //:     constructor to create a 'const' 'Obj' 'XX' from 'mW'.
        //:
        //:   3 Use the member and free 'swap' functions to swap the value of
        //:     'mW' with itself; verify, after each swap, that: (C-6)
        //:
        //:     1 The value is unchanged.  (C-3)
        //:
        //:   4 For each row 'R2' in the table of P-3: (C-1)
        //:
        //:     1 Use the copy constructor to create a modifiable 'Obj', 'mX',
        //:       from 'XX' (P-4.2).
        //:
        //:     2 Use the value constructor to create a modifiable 'Obj', 'mY',
        //:       having the value described by 'R2'; also use the copy
        //:       constructor to create a 'const' 'Obj', 'YY', from 'Y'.
        //:
        //:     3 Use, in turn, the member and free 'swap' functions to swap
        //:       the values of 'mX' and 'mY'; verify, after each swap, that:
        //:       (C-1)
        //:
        //:       1 The values have been exchanged.  (C-1)
        //:
        //: 5 Verify that the free 'swap' function is discoverable through ADL:
        //:   (C-4)
        //:
        //:   1 Create a set of attribute values, 'A', distinct from the values
        //:     corresponding to the default-constructed object.
        //:
        //:   3 Use the default constructor to create a modifiable 'Obj' 'mX'
        //:     (having default attribute values); also use the copy to create
        //:     a 'const' 'Obj' 'XX' from 'mX'.
        //:
        //:   4 Use the value constructor to create a modifiable 'Obj' 'mY'
        //:     having the value described by the 'A1' attribute; also use the
        //:     copy constructor to create a 'const' 'Obj' 'YY' from 'mY'.
        //:
        //:   5 Use the 'bslalg::SwapUtil' helper function template to swap the
        //:     values of 'mX' and 'mY', using the free 'swap' function defined
        //:     in this component, then verify that: (C-4)
        //:
        //:     1 The values have been exchanged.
        //:
        //: 6 Use the test allocator from P-2 to verify that no memory was
        //:   allocated from the default allocator.  (C-5)
        //
        // Testing:
        //   void swap(Location& other);
        //   void swap(Location& a, Location& b);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SWAP MEMBER AND FREE FUNCTIONS" << endl
                          << "==============================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each function to a variable." << endl;
        {
            typedef void (Obj::*funcPtr)(Obj&);
            typedef void (*freeFuncPtr)(Obj&, Obj&);

            // Verify that the signatures and return types are standard.

            funcPtr     memberSwap = &Obj::swap;
            freeFuncPtr freeSwap   = bsl::swap;

            (void)memberSwap;  // quash potential compiler warnings
            (void)freeSwap;
        }

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::TestAllocatorMonitor  dam(&da);
        bslma::DefaultAllocatorGuard dag(&da);

        const int              NUM_DATA        = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int    LINE1   = DATA[ti].d_line;
            const Uint64 OFFSET1 = DATA[ti].d_offset;

            Obj mW(OFFSET1); const Obj& W = mW; const Obj XX(W);

            if (veryVerbose) { T_ P_(LINE1) P_(W) P(XX) }

            // member 'swap'
            {
                mW.swap(mW);                                            // TEST

                ASSERTV(LINE1, XX, W, XX == W);
            }

            // free function 'swap'
            {
                swap(mW, mW);                                           // TEST

                ASSERTV(LINE1, XX, W, XX == W);
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int    LINE2   = DATA[tj].d_line;
                const Uint64 OFFSET2 = DATA[tj].d_offset;

                Obj mX(XX);      const Obj& X = mX;
                Obj mY(OFFSET2); const Obj& Y = mY; const Obj YY(Y);

                if (veryVerbose) { T_ P_(LINE2) P_(X) P_(Y) P(YY) }

                // member 'swap'
                {
                    mX.swap(mY);                                        // TEST

                    ASSERTV(LINE1, LINE2, YY, X, YY == X);
                    ASSERTV(LINE1, LINE2, XX, Y, XX == Y);
                }

                {
                    swap(mX, mY);                                       // TEST

                    ASSERTV(LINE1, LINE2, XX, X, XX == X);
                    ASSERTV(LINE1, LINE2, YY, Y, YY == Y);
                }
            }

        }

        if (verbose) cout <<
                "\nInvoke free 'swap' function in a context where ADL is used."
                                                                       << endl;
        {
            // 'A' values: Should cause memory allocation if possible.

            const Uint64 A1 = bsl::numeric_limits<Int64>::max();

            Obj mX;      const Obj& X = mX; const Obj XX(X);
            Obj mY(A1);  const Obj& Y = mY; const Obj YY(Y);

            if (veryVeryVerbose) { T_ P_(X) P(Y) }

            bslalg::SwapUtil::swap(&mX, &mY);                           // TEST

            ASSERTV(YY, X, YY == X);
            ASSERTV(XX, Y, XX == Y);

            if (veryVeryVerbose) { T_ P_(X) P(Y) }
        }

        ASSERT(dam.isInUseSame());

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        //   Ensure that we can create a distinct object of the class from any
        //   other one, such that the two objects have the same value.
        //
        // Concerns:
        //: 1 The copy constructor
        //:   creates an object having the same value as that of the supplied
        //:   original object.
        //:
        //: 2 There is no memory allocation, not even temporary, from the
        //:   default allocator.
        //:
        //: 3 The original object is passed as a 'const' reference.
        //:
        //: 4 The value of the original object is unchanged.
        //
        // Plan:
        //: 1 Using a table of representative values that include the
        //:   the extremes of the supported range.
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1, create two identical objects, 'Z', that
        //:   will be copied, and 'ZZ', that will be used for control.
        //:
        //: 3 Use the copy constructor to create object 'X' from 'Z'.
        //:
        //: 4 Confirm that 'X' equals 'Z', the object from which 'X' was copied
        //:   and 'Z' equals 'ZZ', showing that the original was not changed.
        //:
        //: 3 A test allocator is installed as the default allocator and
        //:   checked to confirm that no memory is allocated by the copy
        //:   constructor.
        //
        // Testing:
        //   Location(const Location& original);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY CONSTRUCTOR" << endl
                          << "================" << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::TestAllocatorMonitor  dam(&da);
        bslma::DefaultAllocatorGuard dag(&da);

        const int              NUM_DATA        = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int    LINE   = DATA[ti].d_line;
            const Uint64 OFFSET = DATA[ti].d_offset;

            const Obj Z (OFFSET);
            const Obj ZZ(OFFSET);

            if (veryVerbose) { T_ P_(Z) P(ZZ) }

            Obj mX = Obj(Z);   const Obj& X = mX;                   // TEST

            if (veryVerbose) { T_ P(X) }

            // Verify the value of the object.

            ASSERTV(LINE, Z, X,  Z == X);

            // Verify that the value of 'Z' has not changed.

            ASSERTV(LINE, ZZ, Z, ZZ == Z);
        }

        ASSERT(dam.isInUseSame());

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // EQUALITY-COMPARISON OPERATORS
        //   Ensure that '==' and '!=' are the operational definition of value.
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y', compare equal if and only if each of
        //:   their corresponding salient attributes respectively compares
        //:   equal.
        //
        //:   o Note that this class has just a single attribute, 'offset'.
        //:
        //  2 Expected logical relations are supported:
        //
        //:   1 'true  == (X == X)'  (i.e., identity)
        //:   2 'false == (X != X)'  (i.e., identity)
        //:   3 'X == Y' if and only if 'Y == X'  (i.e., commutativity)
        //:   4 'X != Y' if and only if 'Y != X'  (i.e., commutativity)
        //:   5 'X != Y' if and only if '!(X == Y)'
        //:
        //: 3 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., both comparison operators are free functions).
        //:
        //: 4 Non-modifiable objects can be compared (i.e., 'const' objects and
        //:   'const' references).
        //:
        //: 5 No memory allocation occurs as a result of comparison.
        //:
        //: 6 Signature and return type are standard for both the equality
        //:   and inequality operators.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the two homogeneous, free equality-
        //:   comparison operators defined in this component.  (C-6)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their individual salient
        //:   attributes such that (a) for each salient attribute, there exists
        //:   a pair of rows that differ (slightly) in only the column
        //:   corresponding to that attribute, and (b) all attribute values
        //:   that can allocate memory on construction do so.
        //:
        //:   o As this class has just one attribute, the table consists
        //:     of two rows.
        //:
        //: 4 For each row 'R1' in the table of P-3:  (C-2..4)
        //:
        //:   1 Create a single object, using a "scratch" allocator, and
        //:     use it to verify the reflexive (anti-reflexive) property of
        //:     equality (inequality) in the presence of aliasing.
        //:
        //:   2 For each row 'R2' in the table of P-3:
        //:
        //:     1 Record, in 'EXP', whether or not distinct objects created
        //:       from 'R1' and 'R2', respectively, are expected to have the
        //:       same value.
        //:
        //:     2 Create an object 'X' having the value 'R1'.
        //:
        //:     3 Create an object 'Y' having the value 'R2'.
        //:
        //:     4 Verify the commutativity property and expected return value
        //:       for both '==' and '!='.
        //:
        //: 5 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-5)
        //
        // Testing:
        //   bool operator==(const Location& lhs, const Location& rhs);
        //   bool operator!=(const Location& lhs, const Location& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "EQUALITY-COMPARISON OPERATORS" << endl
                          << "=============================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each operator to a variable." << endl;
        {
            using namespace bdljsn;
            typedef bool (*operatorPtr)(const Obj&, const Obj&);

            // Verify that the signatures and return types are standard.

            operatorPtr operatorEq = operator==;
            operatorPtr operatorNe = operator!=;

            (void)operatorEq;  // quash potential compiler warnings
            (void)operatorNe;
        }

        if (verbose) cout <<
            "\nDefine appropriate individual attribute values, 'Ai' and 'Bi'."
                                                                       << endl;

        // Attribute Types

        typedef Uint64      T1;                // 'offset'

        // Attribute 1 Values: 'utcOffsetInSeconds'

        const T1 A1 = 0;                      // baseline
        const T1 B1 = bsl::numeric_limits<Int64>::max();

        if (verbose) cout <<
            "\nCreate a table of distinct, but similar object values." << endl;

        static const struct {
            int         d_line;           // source line number
            T1          d_offset;
        } DATA[] = {

        // The first row of the table below represents an object value
        // consisting of "baseline" attribute values (A1..An).  Each subsequent
        // row differs (slightly) from the first in exactly one attribute
        // value (Bi).  Since, in this class, there is a single attribute,
        // just two rows are needed.

        //LINE  OFFSET
        //----  ------

        { L_,       A1 },          // baseline
        { L_,       B1 }

        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        if (verbose) cout << "\nCompare every value with every value." << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::TestAllocatorMonitor  dam(&da);
        bslma::DefaultAllocatorGuard dag(&da);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE1   = DATA[ti].d_line;
            const T1  OFFSET1 = DATA[ti].d_offset;

            if (veryVerbose) { T_ P_(LINE1) P(OFFSET1) }

            // Ensure an object compares correctly with itself (alias test).
            {
                const Obj X(OFFSET1);

                ASSERTV(LINE1, X,   X == X);                            // TEST
                ASSERTV(LINE1, X, !(X != X));                           // TEST
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int LINE2   = DATA[tj].d_line;
                const T1  OFFSET2 = DATA[tj].d_offset;

                if (veryVerbose) {
                               T_ T_ P_(LINE2) P(OFFSET2) }

                const bool EXP = ti == tj;  // expected for equality comparison

                const Obj X(OFFSET1);
                const Obj Y(OFFSET2);

                if (veryVerbose) {
                    T_ T_ T_ P_(EXP) P_(X) P(Y) }

                // Verify value and commutativity.

                ASSERTV(LINE1, LINE2, X, Y,  EXP == (X == Y));          // TEST
                ASSERTV(LINE1, LINE2, Y, X,  EXP == (Y == X));          // TEST

                ASSERTV(LINE1, LINE2, X, Y, !EXP == (X != Y));          // TEST
                ASSERTV(LINE1, LINE2, Y, X, !EXP == (Y != X));          // TEST
            }
        }

        ASSERT(dam.isInUseSame());

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // PRINT AND OUTPUT OPERATOR
        //   Ensure that the value of the object can be formatted appropriately
        //   on an 'ostream' in some standard, human-readable form.
        //
        // Concerns:
        //: 1 The 'print' method and the output 'operator<<' have standard
        //:   signatures and return types.
        //
        //: 2 The 'print' method and the output 'operator<<' return the
        //:   supplied 'ostream'.
        //:
        //: 3 The 'print' method writes the object's value in decimal format to
        //:   the specified 'ostream' at the intended indentation and followed
        //:   by '\n' unless the third argument ('spacesPerLevel') is negative.
        //:
        //: 4 The optional 'level' and 'spacesPerLevel' parameters have the
        //:   correct default values.
        //:
        //: 5 There is no output when the stream is invalid.
        //
        // Plan:
        //: 1 Use the addresses of the 'print' member function and 'operator<<'
        //:   free function defined in this component to initialize,
        //:   respectively, member-function and free-function pointers having
        //:   the appropriate signatures and return types.  (C-1)
        //:
        //: 2 Using the table-driven technique: (C-2..5)
        //:
        //:   1 Create a table having combinations of the two formatting
        //:     parameters, 'level' and 'spacesPerLevel', and the expected
        //:     output.
        //:
        //:   2 The formatting parameter '-9' and '-8' are "magic".  They are
        //:     never used as arguments.  Rather they direct flow of the test
        //:     case to code that confirms that the default argument values are
        //:     correct and confirms that 'operator<<' works as expected.
        //:
        //:   3 Each table entry is used twice: First with a valid output
        //:     stream and then again with the output stream in a 'bad' state.
        //:     In the former case, the output should match that specified in
        //:     in the table.  In the later case, there should be no output.
        //
        // Testing:
        //   ostream& print(ostream& s, int level = 0, int sPL = 4) const;
        //   ostream& operator<<(bsl::ostream& stream, const Location& object);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRINT AND OUTPUT OPERATOR" << endl
                          << "=========================" << endl;

        if (veryVerbose) cout << "\nAssign the addresses of 'print' and "
                                  "the output 'operator<<' to variables."
                              << endl;
        {
            using namespace bdljsn;
            using bsl::ostream;

            typedef ostream& (Obj::*funcPtr)(ostream&, int, int) const;
            typedef ostream&  (*operatorPtr)(ostream&, const Obj&);

            // Verify that the signatures and return types are standard.

            funcPtr     printMember = &Obj::print;
            operatorPtr operatorOp  = operator<<;

            (void)printMember;  // quash potential compiler warnings
            (void)operatorOp;
        }

        if (veryVerbose) cout <<
             "\nCreate a table of distinct format combinations." << endl;

#define NL "\n"
#define SP " "
#define TEST_DFLT -8
#define TEST_OPER -9
        static const struct {
            int         d_line;           // source line number
            int         d_level;
            int         d_spacesPerLevel;

            Uint64      d_offset;

            const char *d_expected_p;
        } DATA[] = {

        // ------------------------------------------------------------------
        // P-2.1.1: { A } x { 0 }     x { 0, 1, -1, -8 } -->  4 expected o/ps
        // ------------------------------------------------------------------

        //LINE  LEVEL      SPL        OFFSET  EXPECTED
        //----  --------   ---------  ------  ---------------------------------

        { L_,          0,          0,     89, "["                            NL
                                              "offset = 89"                  NL
                                              "]"                            NL
                                                                             },

        { L_,          0,          1,     89,  "["                           NL
                                               " offset = 89"                NL
                                               "]"                           NL
                                                                             },

        { L_,          0,          -1,    89,  "["                           SP
                                               "offset = 89"                 SP
                                               "]"
                                                                             },

        { L_,          0,   TEST_DFLT,    89,  "["                           NL
                                               "    offset = 89"             NL
                                               "]"                           NL
                                                                             },

        // ------------------------------------------------------------------
        // P-2.1.2: { A } x { 3, -3 } x { 0, 2, -2, -8 } -->  6 expected o/ps
        // ------------------------------------------------------------------

        //LINE  LEVEL      SPL        OFFSET  EXPECTED
        //----  --------   ---------  ------  ---------------------------------

        { L_,          3,          0,     89, "["                            NL
                                              "offset = 89"                  NL
                                              "]"                            NL
                                                                             },

        { L_,          3,          2,     89,  "      ["                     NL
                                               "        offset = 89"         NL
                                               "      ]"                     NL
                                                                             },

        { L_,          3,         -2,     89,  "      ["                     SP
                                               "offset = 89"                 SP
                                               "]"
                                                                             },

        { L_,          3,  TEST_DFLT,     89,  "            ["               NL
                                               "                offset = 89" NL
                                               "            ]"               NL
                                                                             },

        { L_,         -3,          0,     89,  "["                           NL
                                               "offset = 89"                 NL
                                               "]"                           NL
                                                                             },

        { L_,         -3,          2,     89,  "["                           NL
                                               "        offset = 89"         NL
                                               "      ]"                     NL
                                                                             },

        { L_,         -3,        -2,      89,  "["                           SP
                                               "offset = 89"                 SP
                                               "]"
                                                                             },

        { L_,         -3, TEST_DFLT,      89,  "["                           NL
                                               "                offset = 89" NL
                                               "            ]"               NL
                                                                             },

        // -----------------------------------------------------------------
        // P-2.1.3: { B } x { 2 }     x { 3 }            -->  1 expected o/p
        // -----------------------------------------------------------------

        //LINE  LEVEL      SPL        OFFSET  EXPECTED
        //----  --------   ---------  ------  ---------------------------------

        { L_,          2,          3,      7, "      ["                      NL
                                              "         offset = 7"          NL
                                              "      ]"                      NL
                                                                             },

        // -----------------------------------------------------------------
        // P-2.1.4: { A B } x { -8 }   x { -8 }         -->  2 expected o/ps
        // -----------------------------------------------------------------

        //LINE  LEVEL      SPL        OFFSET  EXPECTED
        //----  --------   ---------  ------  ---------------------------------

        { L_,   TEST_DFLT, TEST_DFLT,  89,    "["                            NL
                                              "    offset = 89"              NL
                                              "]"                            NL
                                                                             },

        { L_,   TEST_DFLT, TEST_DFLT,   7,    "["                            NL
                                              "    offset = 7"               NL
                                              "]"                            NL
                                                                             },

        // -----------------------------------------------------------------
        // P-2.1.5: { A B } x { -9 }   x { -9 }         -->  2 expected o/ps
        // -----------------------------------------------------------------

        //LINE  LEVEL      SPL        OFFSET  EXPECTED
        //----  --------   ---------  ------  ---------------------------------

        { L_,   TEST_OPER, TEST_OPER,     89, " 89"                          },

        { L_,   TEST_OPER, TEST_OPER,      7, " 7"                           }

        };
#undef SP
#undef NL
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        if (verbose) cout << "\nTesting with various print specifications."
                          << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE = DATA[ti].d_line;
                const int         LVL  = DATA[ti].d_level;
                const int         SPL  = DATA[ti].d_spacesPerLevel;

                const Uint64      OFFSET = DATA[ti].d_offset;
                const char *const EXP    = DATA[ti].d_expected_p;

                if (veryVerbose) { T_ P_(LVL) P(SPL) }

                if (veryVeryVerbose) { T_ T_ Q(EXPECTED) cout << EXP; }

                const Obj X(OFFSET);

                for (int pass = 1; pass <= 2; ++pass) {

                    bslma::TestAllocator         da("default",
                                                    veryVeryVeryVerbose);
                    bslma::TestAllocatorMonitor  dam(&da);
                    bslma::DefaultAllocatorGuard dag(&da);

                    bslma::TestAllocator printAllocator("print",
                                                         veryVeryVeryVerbose);
                    bsl::ostringstream   os(&printAllocator);

                    if (2 == pass) {
                        os.setstate(bsl::ios::badbit);
                    }

                    // Verify supplied stream is returned by reference.

                    if (TEST_OPER == LVL && TEST_OPER == SPL) {
                        ASSERTV(LINE, pass, &os == &(os << X));         // TEST

                        if (veryVeryVerbose) { T_ T_ Q(operator<<) }
                    }
                    else {
                        ASSERTV(LINE, pass, TEST_DFLT == SPL
                                         || TEST_DFLT != LVL);

                        if (TEST_DFLT != SPL) {
                            ASSERTV(LINE, pass, &os == &X.print(os, LVL, SPL));
                                                                        // TEST
                        }
                        else if (TEST_DFLT != LVL) {
                            ASSERTV(LINE, pass, &os == &X.print(os, LVL));
                                                                        // TEST
                        }
                        else {
                            ASSERTV(LINE, pass, &os == &X.print(os));   // TEST
                        }

                        ASSERT(dam.isInUseSame());
#undef TEST_OPER
#undef TEST_DFLT
                        if (veryVeryVerbose) { T_ T_ Q(print) }
                    }

                    // Verify output is formatted as expected.

                    if (veryVeryVerbose) { P(os.str()) }

                    if (2 == pass) {  // Writing to a "bad" stream.
                        ASSERTV(LINE, EXP, os.str(), ""  == os.str());
                    } else {          // Should match table entry.
                        ASSERTV(LINE, EXP, os.str(), EXP == os.str());
                    }
                }
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // VALUE CONSTRUCTOR
        //   Ensure that we can put an object into any initial state relevant
        //   for thorough testing.
        //
        // Concerns:
        //: 1 The value constructor can create an object having any value that
        //:   does not violate the constructor's documented preconditions.
        //:
        //: 2 QoI: The default constructor allocates no memory, not even
        //:   temporarily, from the default allocator.
        //
        //: 3 Any argument can be 'const'.
        //
        // Plan:
        //:  1 Use the value constructor to create a series of objects having
        //:   a representative set of values that include the extremes
        //:    of the allowed range.  Confirm the state of the object
        //:   using the basic allocator tested in TC2.
        //:
        //:  2 A test allocator is installed as the default allocator and
        //:    checked to confirm that no memory is allocated by the value
        //:    constructor.
        //
        // Testing:
        //   explicit Location(bsl::uint64_t offset);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "VALUE CONSTRUCTOR" << endl
                          << "=================" << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::TestAllocatorMonitor  dam(&da);
        bslma::DefaultAllocatorGuard dag(&da);

        const int              NUM_DATA        = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int    LINE   = DATA[ti].d_line;
            const Uint64 OFFSET = DATA[ti].d_offset;

            if (veryVerbose) { T_ P_(LINE) P(OFFSET) }

            Obj mX(OFFSET); const Obj& X = mX;                          // TEST
            ASSERTV(OFFSET,  OFFSET == X.offset());
        }

        ASSERT(dam.isInUseSame());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DEFAULT CTOR, PRIMARY MANIPULATOR, BASIC ACCESSOR, & DTOR
        //   Ensure that we can use the default constructor to create an
        //   object (having the default-constructed value), use the primary
        //   manipulators to put that object into any state relevant for
        //   thorough testing, use basic assessors to show that state
        //   was set as expected, and use the destructor to destroy it safely.
        //
        // Concerns:
        //: 1 An object created with the default constructor has the
        //:   contractually specified default value.
        //:
        //: 2 QoI: The default constructor allocates no memory, not even
        //:   temporarily from the default constructor.
        //:
        //: 3 Each attribute is modifiable independently.
        //:
        //: 4 Each attribute can be set to represent any value that does not
        //:   violate that attribute's documented constraints.
        //:
        //: 5 The values reported by the basic accessors is consistent with the
        //:   values expected from the default constructor and subsequent
        //:   calls to the primary manipulator.
        //:
        //: 6 Any argument can be 'const'.
        //
        // Plan:
        //:  1 An ad hoc series of operations:
        //:
        //:    1 Default constructs an object.
        //:
        //:    2 Sets the object to representative values that include
        //:      the extremes of the allowed values.
        //:
        //:    3 Use the basic accessor to confirm the set values.
        //:
        //:  2 The 'const'-ness of the basic accessor is confirmed by invoking
        //:    it on a 'cons'-reference to the object.
        //:
        //:  3 A test allocator is installed as the default allocator and
        //:    checked to confirm that no memory is allocated by the
        //:    constructor, and no temporary memory is allocated by either
        //:    the primary manipulator or basic accessor.
        //
        // Testing:
        //   Location();
        //   ~Location();
        //   void setOffset(bsl::uint64_t value);
        //   bsl::uint64_t offset() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
        << "DEFAULT CTOR, PRIMARY MANIPULATOR, BASIC ACCESSOR, & DTOR" << endl
        << "=========================================================" << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::TestAllocatorMonitor  dam(&da);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        // 'D' values: These are the default-constructed values.

        const Uint64 D1 = 0UL;        // 'offset'

        // 'A' values

        const Uint64 A1 = 1UL;

        // 'B' values

        const Uint64 B1 = bsl::numeric_limits<Uint64>::max();

        // Default-construct an object.

        Obj mX; const Obj& X = mX;                                      // TEST
        ASSERTV(D1, D1 == X.offset());                                  // TEST

        // Confirm that the attribute can be set to any value in the supportred
        // range.

        mX.setOffset(A1);                                               // TEST
        ASSERTV(A1, A1 == X.offset());                                  // TEST

        mX.setOffset(B1);                                               // TEST
        ASSERTV(B1, B1 == X.offset());                                  // TEST

        mX.setOffset(D1);                                               // TEST
        ASSERTV(D1, D1 == X.offset());                                  // TEST

        ASSERT(dam.isInUseSame());
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
        //: 1 Create an object 'w' (default ctor).       { w:D             }
        //: 2 Create an object 'x' (copy from 'w').      { w:D x:D         }
        //: 3 Set 'x' to 'A' (value distinct from 'D').  { w:D x:A         }
        //: 4 Create an object 'y' (init. to 'A').       { w:D x:A y:A     }
        //: 5 Create an object 'z' (copy from 'y').      { w:D x:A y:A z:A }
        //: 6 Set 'z' to 'D' (the default value).        { w:D x:A y:A z:D }
        //: 7 Assign 'w' from 'x'.                       { w:A x:A y:A z:D }
        //: 8 Assign 'w' from 'z'.                       { w:D x:A y:A z:D }
        //: 9 Assign 'x' from 'x' (aliasing).            { w:D x:A y:A z:D }
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl
                               << "BREATHING TEST" << bsl::endl
                               << "==============" << bsl::endl;

        // Attribute Types

        typedef bsl::int64_t T1;       // 'offset'

        // Attribute 1 Values: 'utcOffsetInSeconds'

        const T1 D1 = 0;               // default value
        const T1 A1 = 1;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 1. Create an object 'w' (default ctor)."
                             "\t\t{ w:D             }" << endl;

        Obj mW;  const Obj& W = mW;

        if (verbose) cout << "\ta. Check initial value of 'w'." << endl;
        if (veryVeryVerbose) { T_ T_ P(W) }

        ASSERT(D1 == W.offset());

        if (verbose) cout <<
                  "\tb. Try equality operators: 'w' <op> 'w'." << endl;

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 2. Create an object 'x' (copy from 'w')."
                             "\t\t{ w:D x:D         }" << endl;

        Obj mX(W);  const Obj& X = mX;

        if (verbose) cout << "\ta. Check initial value of 'x'." << endl;
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(D1 == X.offset());

        if (verbose) cout <<
                   "\tb. Try equality operators: 'x' <op> 'w', 'x'." << endl;

        ASSERT(1 == (X == W));        ASSERT(0 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 3. Set 'x' to 'A' (value distinct from 'D')."
                             "\t\t{ w:D x:A         }" << endl;

        mX.setOffset(A1);

        if (verbose) cout << "\ta. Check new value of 'x'." << endl;
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(A1 == X.offset());

        if (verbose) cout <<
             "\tb. Try equality operators: 'x' <op> 'w', 'x'." << endl;

        ASSERT(0 == (X == W));        ASSERT(1 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 4. Create an object 'y' (init. to 'A')."
                             "\t\t{ w:D x:A y:A     }" << endl;

        Obj mY(A1);  const Obj& Y = mY;

        if (verbose) cout << "\ta. Check initial value of 'y'." << endl;
        if (veryVeryVerbose) { T_ T_ P(Y) }

        ASSERT(A1 == Y.offset());

        if (verbose) cout <<
             "\tb. Try equality operators: 'y' <op> 'w', 'x', 'y'" << endl;

        ASSERT(0 == (Y == W));        ASSERT(1 == (Y != W));
        ASSERT(1 == (Y == X));        ASSERT(0 == (Y != X));
        ASSERT(1 == (Y == Y));        ASSERT(0 == (Y != Y));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 5. Create an object 'z' (copy from 'y')."
                             "\t\t{ w:D x:A y:A z:A }" << endl;

        Obj mZ(Y);  const Obj& Z = mZ;

        if (verbose) cout << "\ta. Check initial value of 'z'." << endl;
        if (veryVeryVerbose) { T_ T_ P(Z) }

        ASSERT(A1 == Z.offset());

        if (verbose) cout <<
           "\tb. Try equality operators: 'z' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(0 == (Z == W));        ASSERT(1 == (Z != W));
        ASSERT(1 == (Z == X));        ASSERT(0 == (Z != X));
        ASSERT(1 == (Z == Y));        ASSERT(0 == (Z != Y));
        ASSERT(1 == (Z == Z));        ASSERT(0 == (Z != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 6. Set 'z' to 'D' (the default value)."
                             "\t\t\t{ w:D x:A y:A z:D }" << endl;

        mZ.setOffset(D1);

        if (verbose) cout << "\ta. Check new value of 'z'." << endl;
        if (veryVeryVerbose) { T_ T_ P(Z) }

        ASSERT(D1 == Z.offset());

        if (verbose) cout <<
           "\tb. Try equality operators: 'z' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(1 == (Z == W));        ASSERT(0 == (Z != W));
        ASSERT(0 == (Z == X));        ASSERT(1 == (Z != X));
        ASSERT(0 == (Z == Y));        ASSERT(1 == (Z != Y));
        ASSERT(1 == (Z == Z));        ASSERT(0 == (Z != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 7. Assign 'w' from 'x'."
                             "\t\t\t\t{ w:A x:A y:A z:D }" << endl;
        mW = X;

        if (verbose) cout << "\ta. Check new value of 'w'." << endl;
        if (veryVeryVerbose) { T_ T_ P(W) }

        ASSERT(A1 == W.offset());

        if (verbose) cout <<
           "\tb. Try equality operators: 'w' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));
        ASSERT(1 == (W == X));        ASSERT(0 == (W != X));
        ASSERT(1 == (W == Y));        ASSERT(0 == (W != Y));
        ASSERT(0 == (W == Z));        ASSERT(1 == (W != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 8. Assign 'w' from 'z'."
                             "\t\t\t\t{ w:D x:A y:A z:D }" << endl;
        mW = Z;

        if (verbose) cout << "\ta. Check new value of 'w'." << endl;
        if (veryVeryVerbose) { T_ T_ P(W) }

        ASSERT(D1 == W.offset());

        if (verbose) cout <<
           "\tb. Try equality operators: 'x' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));
        ASSERT(0 == (W == X));        ASSERT(1 == (W != X));
        ASSERT(0 == (W == Y));        ASSERT(1 == (W != Y));
        ASSERT(1 == (W == Z));        ASSERT(0 == (W != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 9. Assign 'x' from 'x' (aliasing)."
                             "\t\t\t{ w:D x:A y:A z:D }" << endl;
        mX = X;

        if (verbose) cout << "\ta. Check (same) value of 'x'." << endl;
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(A1 == X.offset());

        if (verbose) cout <<
           "\tb. Try equality operators: 'x' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(0 == (X == W));        ASSERT(1 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));
        ASSERT(1 == (X == Y));        ASSERT(0 == (X != Y));
        ASSERT(0 == (X == Z));        ASSERT(1 == (X != Z));

      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      } break;
    }

    // CONCERN: In no case does memory come from the global allocator.

    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = "
                  << testStatus
                  << "."
                  << bsl::endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
