// bdljsn_jsonnull.t.cpp                                              -*-C++-*-
#include <bdljsn_jsonnull.h>

#include <bdlsb_fixedmemoutstreambuf.h>

#include <bsla_maybeunused.h>

#include <bslim_testutil.h>

#include <bslh_hash.h> // 'hashAppend(HASH_ALG, int)'

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bslalg_swaputil.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_libraryfeatures.h>
#include <bsls_review.h>

#include <bsl_cstdlib.h>  // 'bsl::realloc', 'bsl::free'
#include <bsl_cstring.h>  // 'bsl::memcmp', 'bsl::memcpy'
#include <bsl_iostream.h>
#include <bsl_sstream.h>  // 'bsl::ostringstring'

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
#include <variant>
#endif

#include <stddef.h>  // 'size_t'

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The 'bdljsn::JsonNull' class is a degenerate value-semantic type that
// defines just a single value and has few of the conventional value-semantic
// methods.  There are no manipulators; hence no "primary manipulators".  There
// are no accessors (other than 'print'); hence, no "basic accessors".
// Accordingly, the standard 10-step (to start) testing plan is stripped down
// to a few ad-hoc tests in case 1 with separate test cases for 'print',
// 'hashAppend', and (of course) the USAGE EXAMPLE.
// ----------------------------------------------------------------------------
// CREATORS
// [ 1] JsonNull();
//
// ACCESSORS
// [ 2] ostream& print(ostream& s, int level = 0, int sPL = 4) const;
//
// FREE OPERATORS
// [ 1] bool operator==(const bdljsn::JsonNull& lhs, rhs);
// [ 1] bool operator!=(const bdljsn::JsonNull& lhs, rhs);
// [ 2] operator<<(ostream& s, const bdljsn::JsonNull& d);
//
// FREE FUNCTIONS
// [ 1] void swap(bdljsn::JsonNull& a, b);
// [ 3] void hashAppend(HASHALG& hashAlgorithm, const JsonNull& object);
// ----------------------------------------------------------------------------
// [ 4] USAGE EXAMPLE
// [ 2] CONCERN: All accessor methods are declared 'const'.

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
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdljsn::JsonNull Obj;

// ============================================================================
//                      HELPER FUNCTIONS AND CLASSES FOR TESTING
// ----------------------------------------------------------------------------

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
        free(d_data_p);
    }

    void operator()(const void *voidPtr, size_t length)
        // Append the data of the specified 'length' at 'voidPtr' for later
        // inspection.
    {
        d_data_p = realloc(d_data_p, d_length += length);
        memcpy(getData() + d_length - length, voidPtr, length);
    }

    char *getData()
        // Return a pointer to the stored data.
    {
        return static_cast<char *>(d_data_p);
    }

    size_t getLength()
        // Return the length of the stored data.
    {
        return d_length;
    }
};

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    using namespace BloombergLP;

    const int test = argc > 1 ? atoi(argv[1]) : 0;

    BSLA_MAYBE_UNUSED const bool             verbose = argc > 2;
    BSLA_MAYBE_UNUSED const bool         veryVerbose = argc > 3;
    BSLA_MAYBE_UNUSED const bool     veryVeryVerbose = argc > 4;
    BSLA_MAYBE_UNUSED const bool veryVeryVeryVerbose = argc > 5;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

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

        if (verbose) {
            bsl::cout << bsl::endl
                      << "USAGE EXAMPLE" << bsl::endl
                      << "=============" << bsl::endl;
        }

///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The scenario below illustrates almost all of the supported operations on the
// 'bdljsn::JsonNull' type:
//
// First, we create a 'bdljsn::JsonNull' object:
//..
    bdljsn::JsonNull a;
//..
// Then, we examine the object's printed representation:
//..
    bsl::ostringstream oss;
    oss << a;
    ASSERT("null" == oss.str());
//..
// Now, we create a second object of that class and confirm that it equals the
// object created above.
//..
    bdljsn::JsonNull b;
    ASSERT( (a == b));
    ASSERT(!(a != b));
//..
// Finally, we confirm that swapping the two objects has no effect.
//..
    swap(a, b);
    ASSERT(a == b);
//..

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'hashAppend'
        //
        // Concerns:
        //: 1 'hashAppend' has been implemented for 'bdljsn::JsonNull' and,
        //:   when invoked, it passes the expected value to the user-provided
        //:   'HASHALG' object.
        //
        // Plan:
        //: 1 Use a mock hashing algorithm to test 'hashAppend'.
        //
        // Testing:
        //   void hashAppend(HASHALG& hashAlgorithm, const JsonNull& object);
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << bsl::endl
                      << "TESTING 'hashAppend'" << bsl::endl
                      << "====================" << bsl::endl;
        }

        MockAccumulatingHashingAlgorithm forJsonNull;
        MockAccumulatingHashingAlgorithm expJsonNull;

        Obj mX; const Obj& X = mX;

        bdljsn::hashAppend(forJsonNull,  X);
        bslh::  hashAppend(expJsonNull, -1);

        if (veryVerbose) {
            P_(forJsonNull.getLength()) P(expJsonNull.getLength())
        }

        ASSERT(forJsonNull.getLength() == expJsonNull.getLength());
        ASSERT(0 <  forJsonNull.getLength());
        ASSERT(0 == memcmp(forJsonNull.getData(),
                           expJsonNull.getData(),
                           expJsonNull.getLength()));
      } break;
      case 2: {
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
        //: 3 The 'print' method writes the string "null" to the specified
        //:   'ostream' at the intended indentation and followed by '\n'
        //:   unless the third argument ('spacesPerLevel') is negative.
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
        //:   2 Atypically, this table does *not* have parameters for creating
        //:     test object of different values.  The type under test supports
        //:     just a single value that is formatted as "null".
        //:
        //:   3 The formatting parameter '-9' and '-8' are "magic".  They are
        //:     never used as arguments.  Rather they direct flow of the test
        //:     case to code that confirms that the default argument values are
        //:     correct and confirms that 'operator<<' works as expected.
        //:
        //:   4 Each table entry is used twice: First with a valid output
        //:     stream and then again with the output stream in a 'bad' state.
        //:     In the former case, the output should match that specified in
        //:     in the table.  In the later case, there should be no output.
        //
        // Testing:
        //   ostream& print(ostream& s, int level = 0, int sPL = 4) const;
        //   operator<<(ostream& s, const bdljsn::JsonNull& d);
        //   CONCERN: All accessor methods are declared 'const'.
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

        static const struct {
            int         d_line;           // source line number
            int         d_level;
            int         d_spacesPerLevel;
            const char *d_expected_p;
        } DATA[] = {
// BDE_VERIFY pragma: -SP01 // spelling: "spl", "NL"
#define NL "\n"
#define TEST_DFLT -8
#define TEST_OPER -9
            //line  level    spl        expected result
            //----  -------  ---------  -----------------
            {  L_,        0,         4, "null"         NL },

            { L_,         0,        -1, "null"            },  // no trailing NL
            { L_,         0,         0, "null"         NL },
            { L_,         0,         2, "null"         NL },
            { L_,         1,         1, " null"        NL },
            { L_,         1,         2, "  null"       NL },
            { L_,        -1,         2, "null"         NL },
            { L_,        -2,         1, "null"         NL },
            { L_,         2,         1, "  null"       NL },

            // Default 'spl': 'X.print(os, level)'
            { L_,         0, TEST_DFLT, "null"         NL },
            { L_,         1, TEST_DFLT, "    null"     NL },
            { L_,         2, TEST_DFLT, "        null" NL },

            // Default 'level' and 'spl': 'X.print(os)'
            { L_, TEST_DFLT, TEST_DFLT, "null"         NL },

            // 'operator<<'
            { L_, TEST_OPER, TEST_OPER, "null"            }   // no trailing NL
#undef NL
// BDE_VERIFY pragma: +SP01
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        if (verbose) cout << "\nTesting with various print specifications."
                          << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE   = DATA[ti].d_line;
                const int         L      = DATA[ti].d_level;
                const int         SPL    = DATA[ti].d_spacesPerLevel;
                const char *const EXP    = DATA[ti].d_expected_p;

                if (veryVerbose) { T_ P_(L) P(SPL) }

                if (veryVeryVerbose) { T_ T_ Q(EXPECTED) cout << EXP; }

                const Obj X;  // Same value on each iteration.

                for (int pass = 1; pass <= 2; ++pass) {

                    bsl::ostringstream os;

                    if (2 == pass) {
                        os.setstate(ios::badbit);
                    }

                    // Verify supplied stream is returned by reference.

                    if (TEST_OPER == L && TEST_OPER == SPL) {
                        ASSERTV(LINE, pass, &os == &(os << X));         // TEST

                        if (veryVeryVerbose) { T_ T_ Q(operator<<) }
                    }
                    else {
                        ASSERTV(LINE, pass, TEST_DFLT == SPL
                                         || TEST_DFLT != L);

                        if (TEST_DFLT != SPL) {
                            ASSERTV(LINE, pass, &os == &X.print(os, L, SPL));
                                                                        // TEST
                        }
                        else if (TEST_DFLT != L) {
                            ASSERTV(LINE, pass, &os == &X.print(os, L));
                                                                        // TEST
                        }
                        else {
                            ASSERTV(LINE, pass, &os == &X.print(os));   // TEST
                        }
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
      case 1: {
        // --------------------------------------------------------------------
        // TEST CONSTRUCTOR, EQUALITY-COMPARISON OPERATORS, AND 'swap'
        //
        // Concerns:
        //: 1 The equality operator's and 'swap' functions's signatures and
        //:   return types are standard.
        //:
        //: 2 The constructor creates a object.
        //:
        //: 3 All created objects compare equal.
        //:   1 'operator==' always returns 'true'.
        //:   2 'operator!=' always returns 'false'.
        //:
        //: 4 Equality operators are commutative and transitive.
        //:
        //: 5 Swapping objects does not change the equality relationship.
        //:
        //: 6 The 'swap' function can be found via ADL.
        //
        // Plan:
        //: 1 Use the "function-signature" idiom to confirm the operator's
        //:   signature and return type.  (C-1)
        //:
        //: 2 Ad-hoc tests (C-2..6)
        //
        // Testing:
        //   JsonNull();
        //   bool operator==(const bdljsn::JsonNull& lhs, rhs);
        //   bool operator!=(const bdljsn::JsonNull& lhs, rhs);
        //   void swap(bdljsn::JsonNull& a, b);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
      << "TEST CONSTRUCTOR, EQUALITY-COMPARISON OPERATORS, AND 'swap'" << endl
      << "===========================================================" << endl;

        if (veryVerbose) cout <<
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

        if (veryVerbose) cout <<
                "\nAssign the address of 'swap' to a variable." << endl;
        {
            typedef void (*freeFuncPtr)(Obj&, Obj&);

            // Verify that the signature and return type are standard.

            freeFuncPtr freeSwap = bdljsn::swap;

            (void)freeSwap; // quash potential compiler warnings
        }

        if (verbose) cout << "\nAd hoc tests: commutativity and transitivity"
                          << endl;

        Obj mX;  const Obj& X = mX;
        Obj mY;  const Obj& Y = mY;
        Obj mZ;  const Obj& Z = mZ;

        ASSERT( (X == Y)); ASSERT( (Y == X));
        ASSERT( (X == Y)); ASSERT(!(Y != X));

        ASSERT( (Y == Z)); ASSERT( (Z == Y));
        ASSERT(!(Y != Z)); ASSERT(!(Z != Y));

        ASSERT( (X == Z)); ASSERT( (Z == X));
        ASSERT( (X == Z)); ASSERT(!(Z != X));

        if (veryVerbose) cout << "\nAd hoc tests: 'swap'." << endl;

        bdljsn::swap(mX, mY);

        ASSERT( (X == Y)); ASSERT( (Y == X));
        ASSERT( (X == Y)); ASSERT(!(Y != X));

        bdljsn::swap(mY, mZ);

        ASSERT( (Y == Z)); ASSERT( (Z == Y));
        ASSERT(!(Y != Z)); ASSERT(!(Z != Y));

        ASSERT( (X == Z)); ASSERT( (Z == X));
        ASSERT( (X == Z)); ASSERT(!(Z != X));

        if (veryVerbose) cout << "\nAd hoc test: 'swap' ADL." << endl;

        Obj mU;  const Obj& U = mU;
        Obj mV;  const Obj& V = mV;

        ASSERT(U == V)

        bslalg::SwapUtil::swap(&mU, &mV);

        ASSERT(U == V)

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
