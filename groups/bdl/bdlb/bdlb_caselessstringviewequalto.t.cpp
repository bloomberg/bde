// bdlb_caselessstringviewequalto.t.cpp                               -*-C++-*-

#include <bdlb_caselessstringviewequalto.h>

#include <bdlb_chartype.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>

#include <bsl_iostream.h>

#include <bsl_algorithm.h>
#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_functional.h>
#include <bsl_string.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::ends;
using bsl::flush;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// 'bdlb::CaselessStringViewEqualTo' provides a stateless type and thus very
// little to test.  The primary concern is that function call operator compares
// string views case-insensitively and correctly.  CREATORS can be tested only
// for mechanical functioning.  And BSL traits presence should be checked as we
// declare that 'bdlb::CaselessStringViewEqualTo' is an empty POD.
//
// The tests for this component are table based, i.e., testing actual results
// against a table of expected results.
//
// Global Concerns:
//: o No memory is ever allocated from the global allocator.
//: o No memory is ever allocated from the default allocator.
//: o Precondition violations are detected in appropriate build modes.
// ----------------------------------------------------------------------------
// [ 2] Obj();
// [ 2] Obj(const Obj&);
// [ 2] ~Obj();
// [ 2] Obj& operator=(const Obj&);
// [ 3] operator()(bsl::string_view, bsl::string_view);
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
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdlb::CaselessStringViewEqualTo Obj;

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
//                                 TYPE TRAITS
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

struct DerivedInts : bdlb::CaselessStringViewEqualTo {
    int d_a;
    int d_b;
};

struct IntWithMember {
    bdlb::CaselessStringViewEqualTo d_dummy;
    int                             d_a;
};

BSLMF_ASSERT(sizeof(TwoInts) == sizeof(DerivedInts));
BSLMF_ASSERT(sizeof(int) < sizeof(IntWithMember));

}  // close namespace empty_type

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)         veryVerbose;
    (void)     veryVeryVerbose;
    (void) veryVeryVeryVerbose;

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
// Example 1: Basic Use of 'bdlb::CaselessStringViewEqualTo':
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have some strings that compare equal, some that don't, and some
// of which are equal except for the cases of some of the letters.
//..
    const bsl::string_view a = "To be or not to be, that is the question.";
    const bsl::string_view b = "To be or not to be, THAT IS THE QUESTION.";
    const bsl::string_view c = "Sein oder nicht sein, das ist die frage.";
//..
// Now, we create an object of type 'bdlb::CaselessStringViewEqualTo' to do the
// comparisons:
//..
    bdlb::CaselessStringViewEqualTo        eq;
    const bdlb::CaselessStringViewEqualTo& equals = eq;
//..
// Finally, we observe that 'a' matches 'b', but neither matches 'c':
//..
    ASSERT( equals(a, b));
    ASSERT( equals(b, a));
    ASSERT(!equals(a, c));
    ASSERT(!equals(c, a));
    ASSERT(!equals(b, c));
    ASSERT(!equals(c, b));
//..
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // FUNCTION CALL OPERATOR
        //
        // Concerns:
        //: 1 Objects of type 'bdlb::CaselessStringViewEqualTo' can be invoked
        //:   as a binary predicate returning 'bool' and taking two
        //:   'bsl::string' or 'bsl::string_view' arguments.
        //:
        //: 2 The function call operator can be invoked on constant objects.
        //:
        //: 3 The function call returns 'true' or 'false' indicating whether
        //:   the two supplied string arguments have the same string value.
        //:
        //: 4 No memory is allocated from the default or global allocators.
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
        //: 4 Verify that 'CaselessStringViewEqualTo' operates properly on
        //:   default-constructed string views.
        //:
        //: 5 Verify that no memory have been allocated from the default
        //:   allocator.  (C-5)
        //
        // Testing:
        //   operator()(bsl::string_view, bsl::string_view);
        // --------------------------------------------------------------------

        if (verbose) cout << "FUNCTION CALL OPERATOR\n"
                             "======================\n";

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         ta("test",    veryVeryVeryVerbose);
        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        static const struct Data {
            int                       d_line;
            const bsl::string_view    d_str;
        } DATA[] = {
            // LINE STR
            // ---- ---
            {  L_,  ""                     },
            {  L_,  "a"                    },
            {  L_,  "AA"                   },
            {  L_,  "WOOF"                 },
            {  L_,  "meoW"                 },
            {  L_,  "To be or not to be, that is the question.\n" },
            {  L_,  "There are more things in heaven and Earth, Horatio,\n"
                    "than are dreamt of in your philosophy.\n" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        // XLC version 16.1 (on AIX) complains about creating a const object
        // that does not have an "initializer or user-defined default
        // constructor".  To work around that, we create a non-const object,
        // and a const reference to that object, and use the reference.
        Obj        eq;
        const Obj& equals = eq;
        u::RandGen generator;

        if (verbose) cout << "Comparing two strings with varying case\n";

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const Data&       datai = DATA[ti];
            const int         LINEI = datai.d_line;
            const bsl::string LHS(datai.d_str, &ta);

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const Data& dataj    = DATA[tj];
                const int   LINEJ    = dataj.d_line;
                bsl::string rhs(dataj.d_str, &ta);
                const bsl::string& RHS = rhs;

                const bsl::size_t numToggles = 3 * rhs.length();
                for (unsigned tj = 0; tj <= numToggles; ++tj) {
                    ASSERTV(LINEI, LINEJ, LHS, rhs, (LINEI == LINEJ) ==
                                                             equals(LHS, RHS));

                    // toggle the case of one character of 'rhs', chosen at
                    // random

                    if (!rhs.empty()) {
                        const bsl::size_t index = generator() % rhs.length();
                        u::toggleCase(&rhs[index]);
                    }
                }
            }
        }

        if (verbose) cout << "Test default-constructed string view.\n";
        {
            bsl::string_view dflt, emptyString("");

            ASSERT(0 == dflt.data());
            ASSERT(0 != emptyString.data());

            ASSERT( equals(dflt, emptyString));
            ASSERT( equals(emptyString, dflt));
            ASSERT(!equals(dflt, bsl::string_view("woof")));
            ASSERT( equals(bsl::string_view("woof"),
                                                    bsl::string_view("WOOF")));
        }

        ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING IMPLICITLY DEFINED OPERATIONS
        //   Ensure that the four implicitly declared and defined special
        //   member functions are publicly callable and have no unexpected side
        //   effects such as allocating memory.  As there is no observable
        //   state to inspect, there is little to verify other than that all
        //   expected expressions compile.
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
        //: 7 No memory is allocated by the default and global allocators.
        //
        // Plan:
        //: 1 Verify the default constructor exists and is publicly accessible
        //:   by default-constructing a 'const CaselessStringViewEqualTo'
        //:   object. (C-1)
        //:
        //: 2 Verify the copy constructor is publicly accessible and not
        //:   'explicit' by using the copy-initialization syntax to create a
        //:   second 'CaselessStringViewEqualTo' from the first. (C-2,3)
        //:
        //: 3 Assign the value of the first ('const') object to the second.
        //:   (C-4)
        //:
        //: 4 Chain the assignment of the value of the first ('const') object
        //:   to the second, into a self-assignment of the second object to
        //:   itself. (C-5)
        //:
        //: 5 Verify the destructor is publicly accessible by allowing the
        //:   'CaselessStringViewEqualTo' object(s) to leave scope and be
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
            const Obj obj1 = Obj();

            if (verbose) cout << "Copy initialization" << endl;
            Obj obj2 = obj1;

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
        //: 1 Create an object 'equals' using the default constructor. (C-1)
        //:
        //: 2 Call the 'equals' functor with two string literals. (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "BREATHING TEST\n"
                             "==============\n";

        Obj equals;

        bsl::string_view a("a"), A("A"), b("b"), B("B");

        ASSERT( equals(a, A));
        ASSERT( equals(A, a));
        ASSERT( equals(b, B));
        ASSERT( equals(B, b));

        ASSERT(!equals(a, b));
        ASSERT(!equals(a, B));
        ASSERT(!equals(A, b));
        ASSERT(!equals(A, B));

        ASSERT(!equals(b, a));
        ASSERT(!equals(B, a));
        ASSERT(!equals(b, A));
        ASSERT(!equals(B, A));

        ASSERT( equals(a, "A"));
        ASSERT(!equals(a, "B"));

        ASSERT( equals(a, bsl::string(A)));
        ASSERT(!equals(a, bsl::string(B)));

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
