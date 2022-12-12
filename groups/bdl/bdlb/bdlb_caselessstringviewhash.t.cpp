// bdlb_caselessstringviewhash.t.cpp                                  -*-C++-*-

#include <bdlb_caselessstringviewhash.h>

#include <bdlb_caselessstringviewequalto.h>
#include <bdlb_chartype.h>
#include <bdlb_string.h>

#include <bslim_testutil.h>

#include <bslh_spookyhashalgorithm.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_string.h>
#include <bsl_unordered_map.h>
#include <bsl_unordered_set.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::ends;
using bsl::flush;

//=============================================================================
//                              TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// 'bdlb::CaselessStringViewHash' provides a stateless type and thus very
// little to test.  The primary concern is that function call operator
// correctly generates case insensitive hash codes based on the contents of
// string views.  CREATORS can be tested only for mechanical functioning.  And
// BSL traits presence should be checked as we declare that
// 'bdlb::CaselessStringViewHash' is an empty POD.
//
// The tests for this component are table based, i.e., testing actual results
// against a table of expected results.
//
// Global Concerns:
//: o No memory is ever allocated from the global allocator.
//: o No memory is ever allocated from the default allocator.
//: o Precondition violations are detected in appropriate build modes.
// ----------------------------------------------------------------------------
// [ 3] operator()(bsl::string_view) const;
// [ 2] CaselessStringViewHash();
// [ 2] CaselessStringViewHash(const CaselessStringViewHash&);
// [ 2] ~CaselessStringViewHash();
// [ 2] CaselessStringViewHash& operator=(const CaselessStringViewHash&);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 4] USAGE EXAMPLE
//-----------------------------------------------------------------------------

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

//=============================================================================
//                    GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdlb::CaselessStringViewHash Obj;

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

BSLMF_ASSERT((bsl::is_same<bsl::size_t,      Obj::result_type>::value));
BSLMF_ASSERT((bsl::is_same<bsl::string_view, Obj::argument_type>::value));
BSLMF_ASSERT((bsl::is_same<void,             Obj::is_transparent>::value));

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

struct DerivedInts : bdlb::CaselessStringViewHash {
    int d_a;
    int d_b;
};

struct IntWithMember {
    bdlb::CaselessStringViewHash d_dummy;
    int                          d_a;
};

BSLMF_ASSERT(sizeof(TwoInts) == sizeof(DerivedInts));
BSLMF_ASSERT(sizeof(int) < sizeof(IntWithMember));

}  // close namespace empty_type

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of 'bdlb::CaselessStringViewHash':
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we need an associative container to store, for any stock name, the
// number of shares of that stock we own:
//..
    typedef bsl::unordered_map<bsl::string, int> SecuritiesOwnedCS;
                                                        // case sensitive


    typedef bsl::unordered_map<bsl::string,
                               int,
                               bdlb::CaselessStringViewHash,
                               bdlb::CaselessStringViewEqualTo>
                                                             SecuritiesOwnedCI;
                                                        // case insensitive
//..
// This type of container stores quantities of shares and allows access to them
// by their names, in a case-insensitive manner.

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
        //   Copied to component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Get usage example working in test driver, then propagate it to
        //:   the header file.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "USAGE EXAMPLE\n"
                             "=============\n";

//
// First, we create a container for securities holdings, case-sensitive, and
// fill it:
//..
    SecuritiesOwnedCS securitiesOwnedCS;

    securitiesOwnedCS["IBM"]       = 616;
    securitiesOwnedCS["Microsoft"] = 6160000;

    ASSERT(2 == securitiesOwnedCS.size());
//..
// Then, we create a container for securities holdings, case-insensitive, and
// fill it:
//..
    SecuritiesOwnedCI securitiesOwnedCI;

    securitiesOwnedCI["IBM"]       = 616;
    securitiesOwnedCI["Microsoft"] = 6160000;

    ASSERT(2 == securitiesOwnedCI.size());
//..
// Now, we try accessing the case-sensitive 'securitiesc':
//..
    ASSERT(1   == securitiesOwnedCS.count("IBM"));
    ASSERT(616 == securitiesOwnedCS[      "IBM"]);

    ASSERT(0   == securitiesOwnedCS.count("ibm"));
    ASSERT(0   == securitiesOwnedCS.count("Ibm"));
    ASSERT(0   == securitiesOwnedCS.count("iBm"));

    ASSERT(1       == securitiesOwnedCS.count("Microsoft"));
    ASSERT(6160000 == securitiesOwnedCS[      "Microsoft"]);

    ASSERT(0       == securitiesOwnedCS.count("MICROSOFT"));
    ASSERT(0       == securitiesOwnedCS.count("microsoft"));
    ASSERT(0       == securitiesOwnedCS.count("MICROSOFT"));

    ASSERT(0 == securitiesOwnedCS.count("Google"));
//..
// Finally, we access the case-insensitive 'securitiesci':
//..
    ASSERT(1   == securitiesOwnedCI.count("IBM"));
    ASSERT(616 == securitiesOwnedCI[      "IBM"]);
    ASSERT(1   == securitiesOwnedCI.count("ibm"));
    ASSERT(616 == securitiesOwnedCI[      "ibm"]);
    ASSERT(1   == securitiesOwnedCI.count("Ibm"));
    ASSERT(616 == securitiesOwnedCI[      "Ibm"]);
    ASSERT(1   == securitiesOwnedCI.count("iBm"));
    ASSERT(616 == securitiesOwnedCI[      "iBm"]);

    ASSERT(1       == securitiesOwnedCI.count("Microsoft"));
    ASSERT(6160000 == securitiesOwnedCI[      "Microsoft"]);
    ASSERT(1       == securitiesOwnedCI.count("MICROSOFT"));
    ASSERT(6160000 == securitiesOwnedCI[      "MICROSOFT"]);
    ASSERT(1       == securitiesOwnedCI.count("microsoft"));
    ASSERT(6160000 == securitiesOwnedCI[      "microsoft"]);
    ASSERT(1       == securitiesOwnedCI.count("MICROSOFT"));
    ASSERT(6160000 == securitiesOwnedCI[      "MICROSOFT"]);

    ASSERT(0 == securitiesOwnedCI.count("Google"));
//..
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // FUNCTION CALL OPERATOR
        //   Verify that the class offers an operator()
        //
        // Concerns:
        //: 1 Objects of 'bdlb::CaselessStringViewHash' type can be invoked as
        //:   a predicate function returning 'bsl::size_t' and taking a
        //:   'bsl::string' or 'bsl::string_view' argument.
        //:
        //: 2 The function call operator can be invoked on constant objects.
        //:
        //: 3 The function call returns the same result as a usage of
        //:   'bslh::SpookyHashAlgorithm' passing the argument string converted
        //:   to lower case and its length.
        //:
        //: 4 The result doesn't depend on the case of any of the characters in
        //:   the string.
        //:
        //: 5 No memory is allocated from the default or global allocators.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of 'bsl::string's
        //:   for comparison.
        //:
        //: 2 Iterate through the set of strings in the table, and explicitly
        //:   call 'SpookyHashAlgorithm' on each string cast to lower case and
        //:   observe the result is the same as the hash on the original
        //:   string.
        //:
        //: 3 Iterate 'LHS' through the set of strings in the table.
        //:
        //: 4 In a nested loop, iterate 'rhs' through the set of strings in
        //:   the table, and 'const bsl::string& RHS = rhs':
        //:   o Iterate several times.
        //:     1 Compare 'LHS' to 'rhs' with 'CaselessStringViewEqualTo' and
        //:       observe that we get 'true' only when both loops are on the
        //:       same line.
        //:
        //:     2 Randomly perturb the case of one character of 'rhs'.
        //:
        //: 5 Conduct some tests on a default-constructed string view.
        //:
        //: 6 Verify that no memory have been allocated from the default
        //:   allocator.  (C-5)
        //
        // Testing:
        //   operator()(bsl::string_view) const;
        // --------------------------------------------------------------------

        if (verbose) cout << "FUNCTION CALL OPERATOR\n"
                             "======================\n";

        bslma::TestAllocator         ta("test",    veryVeryVeryVerbose);
        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        static const struct Data {
            int               d_line;
            bsl::string_view  d_str;
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

        const Obj hash = Obj();
        u::RandGen generator;

        if (verbose) cout << "Comparing with 'SpookyHashAlgorithm' directly\n";

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const Data&       datai = DATA[ti];
            const int         LINEI = datai.d_line;
            const bsl::string STR(     datai.d_str, &ta);
            bsl::string       strLower(datai.d_str, &ta);

            bdlb::String::toLower(&strLower);

            bslh::SpookyHashAlgorithm sh;
            sh(strLower.data(), strLower.length());

#ifdef BSLS_PLATFORM_CPU_64_BIT
            const bsl::size_t ret = sh.computeHash();
#else
            const bsls::Types::Uint64 ret64 = sh.computeHash();
            const bsl::size_t ret = static_cast<bsl::size_t>(
                                                        ret64 ^ (ret64 >> 32));
#endif
            ASSERTV(LINEI, hash(STR) == ret);
        }

        if (verbose) cout <<
                         "Comparing hashes of two strings with varying case\n";

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const Data&       datai = DATA[ti];
            const int         LINEI = datai.d_line;
            const bsl::string LHS(datai.d_str, &ta);

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const Data&  dataj    = DATA[tj];
                const int    LINEJ    = dataj.d_line;
                bsl::string  rhs(dataj.d_str, &ta);

                const bsl::size_t numToggles = 3 * rhs.length();
                for (unsigned tj = 0; tj <= numToggles; ++tj) {
                    const Obj hash1 = bdlb::CaselessStringViewHash();
                    const Obj hash2 = bdlb::CaselessStringViewHash();

                    ASSERTV(LINEI, LINEJ, LHS, rhs, (LINEI == LINEJ) ==
                                                     (hash(LHS) == hash(rhs)));

                    ASSERTV(LHS, rhs, (LINEI == LINEJ) ==
                                                   (hash1(LHS) == hash1(rhs)));
                    ASSERTV(LHS, rhs, (LINEI == LINEJ) ==
                                                   (hash2(LHS) == hash1(rhs)));
                    ASSERTV(LHS, rhs, (LINEI == LINEJ) ==
                                                   (hash1(LHS) == hash2(rhs)));

                    // toggle the case of one character of 'rhs', chosen at
                    // random

                    if (!rhs.empty()) {
                        const bsl::size_t index = generator() % rhs.length();
                        u::toggleCase(&rhs[index]);
                    }
                }
            }
        }

        if (verbose) cout << "Hash default-constructed string view.\n";
        {
            const bsl::string_view dflt, zeroLength("");

            ASSERT(dflt == zeroLength);
            ASSERT(0 == dflt.data());
            ASSERT(0 != zeroLength.data());
            ASSERT(0 == *zeroLength.data());

            ASSERT(hash(dflt) == hash(zeroLength));
            ASSERT(hash(dflt) != hash(bsl::string_view("woof")));
        }

        ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING IMPLICITLY DEFINED OPERATIONS
        //
        //   Ensure that the four implicitly declared and defined special
        //   member functions are publicly callable and have no unexpected side
        //   effects such as allocating memory.  As there is no observable
        //   state to inspect, there is little to verify other than that the
        //   expected expressions all compile, and
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
        //:   by default-constructing a 'const bdlb::CaselessStringViewHash'
        //:   object.  (C-1)
        //:
        //: 2 Verify the copy constructor is publicly accessible and not
        //:   'explicit' by using the copy-initialization syntax to create a
        //:   second 'bdlb::CaselessStringViewHash' from the first.  (C-2..3)
        //:
        //: 3 Assign the value of the first ('const') object to the second.
        //:   (C-4)
        //:
        //: 4 Chain the assignment of the value of the first ('const') object
        //:   to the second, into a self-assignment of the second object to
        //:   itself.  (C-5)
        //:
        //: 5 Verify the destructor is publicly accessible by allowing the two
        //:   'bdlb::CaselessStringViewHash' object to leave scope and be
        //:   destroyed.  (C-6)
        //:
        //: 6 Verify that no memory have been allocated from the default
        //:   allocator.  (C-7)
        //
        // Testing:
        //   CaselessStringViewHash();
        //   CaselessStringViewHash(const CaselessStringViewHash&);
        //   ~CaselessStringViewHash();
        //   CaselessStringViewHash& operator=(const CaselessStringViewHash&);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING IMPLICITLY DEFINED OPERATIONS\n"
                             "=====================================\n";

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        {
            if (verbose) cout << "\tValue initialization" << endl;
            const Obj obj1 = bdlb::CaselessStringViewHash();

            if (verbose) cout << "\tCopy initialization" << endl;
            Obj obj2 = obj1;

            if (verbose) cout << "\tCopy assignment" << endl;
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
        //: 1 Developer test sandbox.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "BREATHING TEST\n"
                             "==============\n";

        Obj hash;
        bsl::string a("a"), A("A");

        ASSERT(hash(a) > 0);
        ASSERT(hash(a) == hash(A));

        {
            bsl::unordered_set<bsl::string,
                               bdlb::CaselessStringViewHash,
                               bdlb::CaselessStringViewEqualTo>
                stringSet;
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
