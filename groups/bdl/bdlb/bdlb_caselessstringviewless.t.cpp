// bdlb_caselessstringviewless.t.cpp                                  -*-C++-*-

#include <bdlb_caselessstringviewless.h>

#include <bdlb_chartype.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>

#include <bsl_iostream.h>

#include <bsl_cstdlib.h>
#include <bsl_set.h>
#include <bsl_string.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::ends;
using bsl::flush;

// ============================================================================
//                                TEST PLAN
// ----------------------------------------------------------------------------
//                                Overview
//                                --------
// 'bdlb::CaselessStringViewLess' provides a stateless type and thus very
// little to test.  The primary concern is that function call operator compares
// string views correctly.  CREATORS can be tested only for mechanical
// functioning.  And BSL traits presence should be checked as we declare that
// 'bdlb::CaselessStringViewLess' is an empty POD.
//
// The tests for this component are table based, i.e., testing actual results
// against a table of expected results.
//
// Global Concerns:
//: o No memory is ever allocated from the global allocator.
//: o No memory is ever allocated from the default allocator.
//: o Precondition violations are detected in appropriate build modes.
// ----------------------------------------------------------------------------
// [ 3] operator()(bsl::string_view, bsl::string_view) const;
// [ 2] Obj();
// [ 2] Obj(const Obj&);
// [ 2] ~Obj();
// [ 2] Obj& operator=(const Obj&);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 4] USAGE EXAMPLE

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
//                STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

//=============================================================================
//                    GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdlb::CaselessStringViewLess Obj;

// ============================================================================
//                     GLOBAL FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

namespace {
namespace u {

void toggleCase(char *pc)
    // If the character pointed at by the specified 'pc' is upper or lower
    // case, toggle the case of the character.
{
    if (bdlb::CharType::isUpper(*pc)) {
        *pc = bdlb::CharType::toLower(*pc);
    }
    else if (bdlb::CharType::isLower(*pc)) {
        *pc = bdlb::CharType::toUpper(*pc);
    }
}

class RandGen {
    // Random number generator using the high-order 32 bits of Donald Knuth's
    // MMIX algorithm.

    bsls::Types::Uint64 d_seed;

  public:
    explicit
    RandGen();
        // Default-construct the generator.

    unsigned operator()();
        // Return the next random number in the series;
};

// CREATOR
RandGen::RandGen()
: d_seed(0)
{
    for (int ii = 0; ii < 10; ++ii) {
        (void) (*this)();
    }
}

// MANIPULATOR
inline
unsigned RandGen::operator()()
{
    d_seed = d_seed * 6364136223846793005ULL + 1442695040888963407ULL;
    return static_cast<unsigned>(d_seed >> 32);
}

}  // close namespace u
}  // close unnamed namespace

// ============================================================================
//                                  TYPE TRAITS
// ----------------------------------------------------------------------------

BSLMF_ASSERT(bsl::is_trivially_copyable<Obj>::value);
BSLMF_ASSERT(bsl::is_trivially_default_constructible<Obj>::value);

// ============================================================================
//                                  TYPEDEFS
// ----------------------------------------------------------------------------

BSLMF_ASSERT((bsl::is_same<bool,           Obj::result_type>::value));
BSLMF_ASSERT((bsl::is_same<bsl::string_view,
                                           Obj::first_argument_type>::value));
BSLMF_ASSERT((bsl::is_same<bsl::string_view,
                                           Obj::second_argument_type>::value));
BSLMF_ASSERT((bsl::is_same<void,           Obj::is_transparent>::value));

// ============================================================================
//                                 EMPTY TYPE
//
// Observe that the type under test has no data members.
// ----------------------------------------------------------------------------

namespace empty_type {

struct TwoInts {
    int d_a;
    int d_b;
};

struct DerivedInts : bdlb::CaselessStringViewLess {
    int d_a;
    int d_b;
};

struct IntWithMember {
    bdlb::CaselessStringViewLess d_dummy;
    int                          d_a;
};

BSLMF_ASSERT(sizeof(TwoInts) == sizeof(DerivedInts));
BSLMF_ASSERT(sizeof(int) < sizeof(IntWithMember));

}  // close namespace empty_type

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

    (void)veryVerbose;
    (void)veryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 4: {
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

        if (verbose) cout << "USAGE EXAMPLE\n"
                             "=============\n";
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of 'bdlb::CaselessStringViewLess'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we need a container to store set of unique strings.  The following
// code illustrates how to use 'bdlb::CaselessStringViewLess' as a comparator
// for the standard container 'set', to create a set of unique case-insensitive
// string values.
//
// First, we create several strings:
//..
    const bsl::string newYork        = "NY";
    const bsl::string losAngeles     = "LA";
    const bsl::string newJersey      = "NJ";
    const bsl::string sanFrancisco   = "SF";
    const bsl::string anotherNewYork = "ny";
//..
// Next, we create two containers, one with default comparator and another
// using 'bdlb::CstringLess' as a comparator:
//..
    bsl::set<bsl::string>                               caseSensitiveSet;
    bsl::set<bsl::string, bdlb::CaselessStringViewLess> caseInsensitiveSet;
//..
// Then, we fill containers with the same contents:
//..
    caseSensitiveSet.insert(newYork);
    caseSensitiveSet.insert(losAngeles);
    caseSensitiveSet.insert(newJersey);
    caseSensitiveSet.insert(sanFrancisco);
    caseSensitiveSet.insert(anotherNewYork);

    caseInsensitiveSet.insert(newYork);
    caseInsensitiveSet.insert(losAngeles);
    caseInsensitiveSet.insert(newJersey);
    caseInsensitiveSet.insert(sanFrancisco);
    caseInsensitiveSet.insert(anotherNewYork);
//..
// Next, we observe that the container created with 'CaselessStringViewLess'
// ('caseInsensitiveSet') contains the correct number of unique string values
// (4), while the container using the default comparator does not:
//..
    ASSERT(5 == caseSensitiveSet.size());
    ASSERT(4 == caseInsensitiveSet.size());
//..
// Now, we observe the members of the case-sensitive set:
//..
    ASSERT( caseSensitiveSet.count("NY"));
    ASSERT(!caseSensitiveSet.count("nY"));
    ASSERT(!caseSensitiveSet.count("Ny"));
    ASSERT( caseSensitiveSet.count("ny"));

    ASSERT( caseSensitiveSet.count("SF"));
    ASSERT(!caseSensitiveSet.count("sF"));
    ASSERT(!caseSensitiveSet.count("Sf"));
    ASSERT(!caseSensitiveSet.count("sf"));
//..
// Finally, we observe that we can do case-insensitive access to
// 'caseInsensiveSet':
//..
    ASSERT( caseInsensitiveSet.count("NY"));
    ASSERT( caseInsensitiveSet.count("nY"));
    ASSERT( caseInsensitiveSet.count("Ny"));
    ASSERT( caseInsensitiveSet.count("ny"));

    ASSERT( caseInsensitiveSet.count("LA"));
    ASSERT( caseInsensitiveSet.count("lA"));
    ASSERT( caseInsensitiveSet.count("La"));
    ASSERT( caseInsensitiveSet.count("la"));

    ASSERT( caseInsensitiveSet.count("nj"));
    ASSERT( caseInsensitiveSet.count("nJ"));
    ASSERT( caseInsensitiveSet.count("Nj"));
    ASSERT( caseInsensitiveSet.count("NJ"));

    ASSERT( caseInsensitiveSet.count("sf"));
    ASSERT( caseInsensitiveSet.count("sF"));
    ASSERT( caseInsensitiveSet.count("Sf"));
    ASSERT( caseInsensitiveSet.count("SF"));

    ASSERT(!caseInsensitiveSet.count("GA"));
    ASSERT(!caseInsensitiveSet.count("gA"));
    ASSERT(!caseInsensitiveSet.count("Ga"));
    ASSERT(!caseInsensitiveSet.count("ga"));
//..
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // FUNCTION CALL OPERATOR
        //
        // Concerns:
        //: 1 Objects of type 'bdlb::CaselessStringViewLess' can be invoked as
        //:   a binary predicate returning 'bool' and taking two
        //:   'bsl::string_view' arguments.
        //:
        //: 2 The function call operator can be invoked on constant objects.
        //:
        //: 3 The function call returns 'true' or 'false' indicating whether
        //:   the two supplied string arguments are supplied in lexical order.
        //:
        //: 4 No memory is allocated from the default allocator.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of 'bsl::string's
        //:   for comparison.
        //:
        //: 2 Iterate 'LHS' through the set of strings in the table.
        //:
        //: 3 In a nested loop, iterate 'rhs' through the set of strings in
        //:   the table, and 'const bsl::string& RHS = rhs':
        //:   o Iterate several times.
        //:     1 Compare 'LHS' to 'rhs' with 'CaselessStringViewEqualTo' and
        //:       observe that we get 'true' only when both loops are on the
        //:       same line.
        //:
        //:     2 Randomly perturb the case of one character of 'rhs'.
        //:
        //: 4 Conduct some tests on a default-constructed string view.
        //:
        //: 5 Verify that no memory have been allocated from the default
        //:   allocator.  (C-4)
        //
        // Testing:
        //   operator()(bsl::string_view, bsl::string_view) const;
        // --------------------------------------------------------------------

        if (verbose) cout << "FUNCTION CALL OPERATOR\n"
                             "======================\n";

        if (verbose) cout <<
                    "Create a test allocator and install it as the default.\n";

        bslma::TestAllocator         ta("test",    veryVeryVeryVerbose);
        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        static const struct Data {
            int              d_line;
            int              d_index;
            bsl::string_view d_str;
        } DATA[] = {
            // LINE  IDX STR
            // ----  ---  ---
            {  L_,   0,  "" },
            {  L_,   0,  bsl::string_view() },
            {  L_,   1,  "45" },
            {  L_,   2,  "arf" },
            {  L_,   3,  "meow" },
            {  L_,   4,  "There are more things in heaven and Earth,\n"
                         "Horatio, than are dreamt of in your philosophy.\n" },
            {  L_,   5,  "To be or not to be, that is the question." },
            {  L_,   6,  "woof" },
        };
        enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

        u::RandGen generator;
        Obj        less;

        ASSERT(DATA[0].d_str.data() != 0);
        ASSERT(DATA[1].d_str.data() == 0);
        ASSERT(DATA[0].d_str == DATA[1].d_str);

        if (verbose) cout << "Iterate, comparing strings with varying case\n";

        for (int ti = 0; ti < k_NUM_DATA; ++ti) {
            const Data& datai = DATA[ti];
            const int   LINEI = datai.d_line;
            const int   IDXI  = datai.d_index;
            bsl::string LHS(datai.d_str, &ta);

            const bsl::string_view dflt;
            ASSERT(0 == dflt.data());
            ASSERT((0 < LHS.length()) == less(dflt, LHS));
            ASSERT(!less(LHS, dflt));

            for (int tj = 0; tj < k_NUM_DATA; ++tj) {
                const Data& dataj = DATA[tj];
                const int          LINEJ = dataj.d_line;
                const int          IDXJ  = dataj.d_index;
                bsl::string        rhs(dataj.d_str, &ta);
                const bsl::string& RHS = rhs;
                const bool         EXP = IDXI < IDXJ;

                bsl::size_t numToggles = 3 * rhs.length();
                for (unsigned tk = 0; tk <= numToggles; ++tk) {
                    ASSERTV(LINEI, LINEJ, LHS, rhs, EXP, tk,
                                                       EXP == less(LHS, RHS));
                    ASSERTV(LINEI, LINEJ, LHS, rhs, EXP, tk,
                                                       EXP == Obj()(LHS, RHS));

                    // toggle the case of one character of 'rhs', chosen at
                    // random

                    if (!rhs.empty()) {
                        bsl::size_t charIdx = generator() % rhs.length();
                        u::toggleCase(&rhs[charIdx]);
                    }
                }
            }
        }

        if (verbose) cout << "Test with default-constructed string view.\n";
        {
            bsl::string_view dflt, emptyString("");

            ASSERT(0 == dflt.data());
            ASSERT(0 != emptyString.data());
            ASSERT(0 == *emptyString.data());

            ASSERT(!less(dflt, emptyString));
            ASSERT(!less(emptyString, dflt));
            ASSERT( less(dflt, bsl::string_view("woof")));
            ASSERT(!less(bsl::string_view("woof"), dflt));
            ASSERT(!less(bsl::string_view("ABC"), bsl::string_view("abc")));
            ASSERT(!less(bsl::string_view("abc"), bsl::string_view("ABC")));

            ASSERT( less(bsl::string_view("a"), bsl::string_view("b")));
            ASSERT( less(bsl::string_view("A"), bsl::string_view("b")));
            ASSERT( less(bsl::string_view("a"), bsl::string_view("B")));
            ASSERT( less(bsl::string_view("A"), bsl::string_view("B")));

            ASSERT(!less(bsl::string_view("b"), bsl::string_view("a")));
            ASSERT(!less(bsl::string_view("b"), bsl::string_view("A")));
            ASSERT(!less(bsl::string_view("B"), bsl::string_view("a")));
            ASSERT(!less(bsl::string_view("B"), bsl::string_view("A")));

            ASSERT( less(dflt, bsl::string_view("a")));
            ASSERT( less(dflt, bsl::string_view("A")));
        }

        ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING IMPLICITLY DEFINED OPERATIONS
        //   Ensure that the four implicitly declared and defined special
        //   member functions are publicly callable and have no unexpected side
        //   effects such as allocating memory.  As there is no observable
        //   state to inspect, there is little to verify other than that the
        //   expected expressions all compile.
        //
        // Concerns:
        //: 1 Objects can be created using the default constructor.
        //:
        //: 2 Objects can be created using the copy constructor.
        //:
        //: 3 The copy constructor is not declared as explicit.
        //:
        //: 4 Objects can be assigned to from constant objects.
        //:
        //: 5 Assignments operations can be chained.
        //:
        //: 6 Objects can be destroyed.
        //:
        //: 7 No memory is allocated by the default allocator.
        //
        // Plan:
        //: 1 Verify the default constructor exists and is publicly accessible
        //:   by default-constructing a 'const bdlb::CaselessStringViewLess'
        //:   object.  (C-1)
        //:
        //: 2 Verify the copy constructor is publicly accessible and not
        //:   'explicit' by using the copy-initialization syntax to create a
        //:   second 'bdlb::CaselessStringViewLess' from the first.  (C-2..3)
        //:
        //: 3 Assign the value of the first ('const') object to the second.
        //:   (C-4)
        //:
        //: 4 Chain the assignment of the value of the first ('const') object
        //:   to the second, into a self-assignment of the second object to
        //:   itself.  (C-5)
        //:
        //: 5 Verify the destructor is publicly accessible by allowing the two
        //:   'bdlb::CaselessStringViewLess' object to leave scope and be
        //:   destroyed.  (C-6)
        //:
        //: 6 Verify that no memory have been allocated from the default
        //:   allocator.  (C-7)
        //
        // Testing:
        //   Obj();
        //   Obj(const Obj&);
        //   ~Obj();
        //   Obj& operator=(const Obj&);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING IMPLICITLY DEFINED OPERATIONS\n"
                             "=====================================\n";

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        {
            if (verbose) cout << "Value initialization" << endl;
            const bdlb::CaselessStringViewLess obj1 =
                                                bdlb::CaselessStringViewLess();

            if (verbose) cout << "Copy initialization" << endl;
            bdlb::CaselessStringViewLess obj2 = obj1;

            if (verbose) cout << "Copy assignment" << endl;
            obj2 = obj1;
            obj2 = obj2 = obj1;
        }

        ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
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
        //: 1 Create an object 'less' using the default ctor.
        //:
        //: 2 Call the 'less' functor with two string literals in lexical
        //:   order.
        //:
        //: 3 Call the 'less' functor with two string literals in reverse
        //:   lexical order.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "BREATHING TEST\n"
                             "==============\n";

        bdlb::CaselessStringViewLess less;

        ASSERT( less("A", "Z"));
        ASSERT( less("a", "Z"));
        ASSERT( less("A", "z"));
        ASSERT( less("a", "z"));

        ASSERT(!less("A", "A"));
        ASSERT(!less("a", "A"));
        ASSERT(!less("A", "a"));
        ASSERT(!less("a", "a"));

        ASSERT(!less("Z", "A"));
        ASSERT(!less("z", "A"));
        ASSERT(!less("Z", "a"));
        ASSERT(!less("z", "a"));

        {
            bsl::set<bsl::string, bdlb::CaselessStringViewLess> stringSet;
            stringSet.find(                 "some string" );
            stringSet.find(bsl::string(     "some string"));
            stringSet.find(bsl::string_view("some string"));
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    ASSERTV(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
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
