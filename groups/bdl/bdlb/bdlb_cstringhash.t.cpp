// bdlb_cstringhash.t.cpp                                             -*-C++-*-

#include <bdlb_cstringhash.h>
#include <bdlb_cstringequalto.h>

#include <bslalg_hastrait.h>
#include <bslalg_typetraits.h>

#include <bslh_spookyhashalgorithm.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_issame.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <bsl_iostream.h>
#include <bsl_unordered_map.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                              TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// 'bdlb::CStringHash' provides a stateless type and thus very little to test.
// The primary concern is that function call operator correctly generates hash
// codes based on the contents of C-strings.  CREATORS can be tested only for
// mechanical functioning.  And BSL traits presence should be checked as we
// declare that 'bdlb::CStringHash' is an empty POD.
//
// The tests for this component are table based, i.e., testing actual results
// against a table of expected results.
//
// Global Concerns:
//: o No memory is ever allocated from the global allocator.
//: o No memory is ever allocated from the default allocator.
//: o Precondition violations are detected in appropriate build modes.
// ----------------------------------------------------------------------------
// [ 3] operator()(const char *) const
// [ 2] CStringHash()
// [ 2] CStringHash(const CStringHash&)
// [ 2] ~CStringHash()
// [ 2] CStringHash& operator=(const CStringHash&)
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 7] USAGE EXAMPLE
// [ 4] Standard typedefs
// [ 5] BSL Traits
// [ 6] QoI: Support for empty base optimization
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
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                    GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
typedef bdlb::CStringHash Obj;

// ============================================================================
//                                 TYPE TRAITS
// ----------------------------------------------------------------------------

BSLMF_ASSERT(bsl::is_trivially_copyable<Obj>::value);
BSLMF_ASSERT(bsl::is_trivially_default_constructible<Obj>::value);

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of 'bdlb::CStringHash'
/// - - - - - - - - - - - - - - - - - - - - -
// Suppose we need an associative container to store some objects, uniquely
// identified by C-strings.   The following code illustrates how to use
// 'bdlb::CStringHash' as a hash function for the standard container
// 'unordered_map' taking C-string as a key.
//
// First, let us define our mapped type class:
//..
    class Security
    {
        // This class implements a value semantic type that represents
        // ownership of a security.

        // DATA
        char         *d_name_p;      // Security name
        unsigned int  d_sharesOwned; // The number of owned shares

      public:
        // CREATORS
        Security(const char *name, unsigned int sharesOwned);
            // Create a 'Security' object having the specified 'name' and
            // 'sharesOwned'.

        Security(const Security& original);
            // Create a 'Security' object having the value of the specified
            // 'original' security.

        ~Security();
            // Destroy this security object.


        // ACCESSORS
        unsigned int sharesOwned() const;
            // Return the value of the 'sharesOwned' attribute of this security
            // object.

        // MANIPULATORS
        Security& operator=(Security other);
            // Assign to this security object the value of the specified
            // 'other' object, and return a reference providing modifiable
            // access to this object.

        void swap(Security& other);
            // Efficiently exchange the value of this security with the value
            // of the specified 'other' security.
    };

    // CREATORS
    inline
    Security::Security(const char *name, unsigned int sharesOwned)
    : d_sharesOwned(sharesOwned)
    {
        d_name_p = new char [strlen(name) + 1];
        strncpy(d_name_p, name, strlen(name) + 1);
    }

    inline
    Security::Security(const Security& original)
    : d_sharesOwned(original.d_sharesOwned)
    {
        if (this != &original)
        {
            d_name_p = new char [strlen(original.d_name_p) + 1];
            strncpy(d_name_p,
                    original.d_name_p,
                    strlen(original.d_name_p) + 1);
        }
    }

    inline
    Security::~Security()
    {
        delete [] d_name_p;
    }

    // ACCESSORS

    inline    unsigned int Security::sharesOwned() const
    {
        return d_sharesOwned;
    }

    // MANIPULATORS
    inline
    Security& Security::operator=(Security other)
    {
        this->swap(other);
        return *this;
    }

    inline
    void Security::swap(Security& other)
    {
        char * tempPtr = d_name_p;
        d_name_p = other.d_name_p;
        other.d_name_p = tempPtr;
        unsigned int tempInt = d_sharesOwned;
        d_sharesOwned = other.d_sharesOwned;
        other.d_sharesOwned = tempInt;
    }
//..
// Next, we define container type using 'bdlb::CStringHash' as a hash function
// and 'bdlb::CstringEqualTo' as a comparator:
//..
    typedef unordered_map<const char *,
                          Security,
                          bdlb::CStringHash,
                          bdlb::CStringEqualTo> SecuritiesUM;
//..
// This container stores objects of 'Security' class and allow access to them
// by their names.

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
      case 7: {
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

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

// Then, we create several C-strings with security names:
//..
    const char *ibm  = "IBM";
    const char *msft = "Microsoft";
    const char *goog = "Google";
//..
// Now, we create a container for securities and fill it:
//..
    SecuritiesUM securities;

    securities.insert(std::make_pair(ibm, Security(ibm, 616)));
    securities.insert(std::make_pair(msft, Security(msft, 6150000)));
//..
// Finally, we make sure, that we able to access securities by their names:
//..
    SecuritiesUM::iterator it = securities.find(ibm);
    ASSERT(616 == it->second.sharesOwned());
    it = securities.find(msft);
    ASSERT(6150000 == it->second.sharesOwned());
    it = securities.find(goog);
    ASSERT(securities.end() == it);
//..
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING QOI: 'CStringHash' IS AN EMPTY TYPE
        //   As a quality of implementation issue, the class has no state and
        //   should support the use of the empty base class optimization on
        //   compilers that support it.
        //
        // Concerns:
        //: 1 Class 'bdlb::CStringHash' does not increase the size of an object
        //:   when used as a base class.
        //:
        //: 2 Object of 'bdlb::CStringHash' class increases size of an object
        //:   when used as a class member.
        //
        // Plan:
        //: 1 Define two identical non-empty classes with no padding, but
        //:   derive one of them from 'bdlb::CStringHash', then assert that
        //:   both classes have the same size. (C-1)
        //:
        //: 2 Create a non-empty class with an 'bdlb::CStringHash' additional
        //:   data member, assert that class size is larger than sum of other
        //:   data member's sizes. (C-2)
        //
        // Testing:
        //   QoI: Support for empty base optimization
        // --------------------------------------------------------------------

        if (verbose) cout
                      << endl
                      << "TESTING QOI: 'CStringHash' IS AN EMPTY TYPE" << endl
                      << "===========================================" << endl;

        struct TwoInts {
            int a;
            int b;
        };

        struct DerivedInts : bdlb::CStringHash {
            int a;
            int b;
        };

        struct IntWithMember {
            bdlb::CStringHash dummy;
            int               a;
        };

        ASSERT(sizeof(TwoInts) == sizeof(DerivedInts));
        ASSERT(sizeof(int) < sizeof(IntWithMember));

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING BSL TRAITS
        //   The functor is an empty POD, and should have the appropriate BSL
        //   type traits to reflect this.
        //
        // Concerns:
        //: 1 The class is trivially copyable.
        //:
        //: 2 The class has the trivial default constructor trait.
        //
        // Plan:
        //: 1 ASSERT the presence of each trait required by the type. (C-1..2)
        //
        // Testing:
        //   BSL Traits
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING BSL TRAITS" << endl
                          << "==================" << endl;

        ASSERT((bsl::is_trivially_copyable<Obj>::value));
        ASSERT((bsl::is_trivially_default_constructible<Obj>::value));
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING STANDARD TYPEDEFS
        //   Verify that the class offers the two typedefs required of a
        //   standard adaptable unary function.
        //
        // Concerns:
        //: 1 The typedef 'argument_type' is publicly accessible and an alias
        //:   for 'const char *'.
        //:
        //: 2 The typedef 'result_type' is publicly accessible and an alias for
        //:   'bsl::size_t'.
        //
        // Plan:
        //: 1 ASSERT each of the typedefs has accessibly aliases the correct
        //:   type using 'bsl::is_same'. (C-1..2)
        //
        // Testing:
        //  Standard typedefs
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING STANDARD TYPEDEFS" << endl
                          << "=========================" << endl;

        ASSERT((bsl::is_same<bsl::size_t,  Obj::result_type>::value));
        ASSERT((bsl::is_same<const char *, Obj::argument_type>::value));

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // FUNCTION CALL OPERATOR
        //   Verify that the class offers an operator()
        //
        // Concerns:
        //: 1 Objects of 'bdlb::CStringHash' type can be invoked as a predicate
        //:   function returning 'bsl::size_t' and taking a 'const char *'
        //:   argument.
        //:
        //: 2 The function call operator can be invoked on constant objects.
        //:
        //: 3 The function call returns the same result as a
        //:   usage of 'bslh::SpookyHashAlgorithm' passing the argument string
        //:   and its length.
        //:
        //: 4 The function call operator returns the same value for two
        //:   differently valued pointers that point to equal null-terminated
        //:   character sequences.
        //:
        //: 5 The hash computed by the objects of 'bdlb::CStringHash' type does
        //:   not account for any characters past null-terminated character
        //:   sequence.
        //:
        //: 6 Asserted precondition violations are detected when enabled.
        //:
        //: 7 No memory is allocated from the default or global allocators.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of c-string for
        //:   hash values calculating.
        //:
        //: 2 For each row 'R' in the table of P-1 verify that the function
        //:   call operator, when invoked on c-string values from 'R', returns
        //:   the same value as 'bslh::SpookyHashAlgorithm' does.  (C-1..3)
        //:
        //: 3 Invoke function call operator on two differently valued pointers
        //:   that point to the equal null-terminated character sequences and
        //:   verify that the return hash values are the same.  (C-4)
        //:
        //: 4 Invoke function call operator on a pointer that points to the
        //:   c-string with embedded null character and verify that the hash
        //:   is equal to the substring that ends at the null character.  (C-5)
        //
        //: 5 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-5)
        //:
        //: 6 Verify that no memory have been allocated from the default
        //:   allocator.  (C-7)
        //
        // Testing:
        //   operator()(const char *) const
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "FUNCTION CALL OPERATOR" << endl
                          << "======================" << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout << "\nTesting computed hash values." << endl;
        {
            static const struct {
                int          d_line;
                const char  *d_string;
            } DATA[] = {
                // LINE  STRING
                // ----  ------------
                {   L_,  ""           },
                {   L_,  "AAA"        },
                {   L_,  "aBcXyZ"     },
                {   L_,  "0123456789" },
                {   L_,  "~!@#$%^&*(" },
            };
            const long unsigned int NUM_DATA = sizeof DATA / sizeof *DATA;

            const Obj hash = bdlb::CStringHash();

            for (int i = 0; i != NUM_DATA; ++i) {
                const int          LINE      = DATA[i].d_line;
                const char        *STRING    = DATA[i].d_string;

                bslh::SpookyHashAlgorithm spookyHash;
                spookyHash(STRING,  strlen(STRING));
                const bsl::size_t HASHCODE  =
                    static_cast<bsl::size_t>(spookyHash.computeHash());

                ASSERTV(LINE, hash(STRING) == HASHCODE);
                // Second invocation to make sure that the hash computation is
                // stateless.
                ASSERTV(LINE, hash(STRING) == HASHCODE);
            }
        }

        if (verbose)
            cout << "\nTesting hash values of equal c-strings." << endl;
        {
            char value1[] = "Value0123456789";
            char value2[] = "Value0123456789";

            ASSERTV(value1, value2, &value1[0] != &value2[0]);

            const Obj hash1 = bdlb::CStringHash();
            const Obj hash2 = bdlb::CStringHash();

            ASSERTV(value1, value2, hash1(value1) == hash1(value2));
            ASSERTV(value1, value2, hash2(value1) == hash1(value2));
            ASSERTV(value1, value2, hash1(value1) == hash2(value2));
        }

        if (verbose)
            cout << "\nTesting hash values of c-strings with embedded null."
                 << endl;
        {
            char value1[] = "Value\000Traling value";
            char value2[] = "Value";

            const Obj hash = bdlb::CStringHash();

            ASSERTV(value1, value2, hash(value1) == hash(value2));
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard guard;

            const Obj hash = bdlb::CStringHash();

            ASSERT_SAFE_FAIL(hash(0));
            ASSERT_SAFE_PASS(hash("Hello world"));
        }

        ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // IMPLICITLY DEFINED OPERATIONS
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
        //:   by default-constructing a 'const bdlb::CStringHash' object. (C-1)
        //:
        //: 2 Verify the copy constructor is publicly accessible and not
        //:   'explicit' by using the copy-initialization syntax to create a
        //:   second 'bdlb::CStringHash' from the first. (C-2..3)
        //:
        //: 3 Assign the value of the first ('const') object to the second.
        //:   (C-4)
        //:
        //: 4 Chain the assignment of the value of the first ('const') object
        //:   to the second, into a self-assignment of the second object to
        //:   itself. (C-5)
        //:
        //: 5 Verify the destructor is publicly accessible by allowing the two
        //:   'bdlb::CStringHash' object to leave scope and be destroyed. (C-6)
        //:
        //: 6 Verify that no memory have been allocated from the default
        //:   allocator.  (C-7)
        //
        // Testing:
        //   CStringHash()
        //   CStringHash(const CStringHash&)
        //   ~CStringHash()
        //   CStringHash& operator=(const CStringHash&)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "IMPLICITLY DEFINED OPERATIONS" << endl
                          << "=============================" << endl;

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        {
            if (verbose) cout << "\tValue initialization" << endl;
            const Obj obj1 = bdlb::CStringHash();


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

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        Obj hash;
        ASSERT(hash("A") > 0);

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
// Copyright 2015 Bloomberg Finance L.P.
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
