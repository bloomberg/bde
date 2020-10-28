// bslh_fibonaccibadhashwrapper.t.cpp                                 -*-C++-*-

#include <bslh_fibonaccibadhashwrapper.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_types.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//
//                              Overview
//                              --------
// The component under test is a hash functor wrapper.  The component will have
// all its functionality directly tested.  This container is implemented in the
// form of a class template, and thus its proper instantiation for several
// types is a concern.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] FibonacciBadHashWrapper();
// [ 2] FibonacciBadHashWrapper(const HASH& hash);
// [ 2] FibonacciBadHashWrapper(const FBHAW& original);
// [ 2] ~FibonacciBadHashWrapper();
//
// MANIPULATORS
// [ 3] FibonacciBadHashWrapper& operator=(const FBHAW& rhs);
//
// ACCESSORS
// [ 4] size_t operator()(const KEY& key) const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 5] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

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
//                  GLOBAL CLASSES/STRUCTS FOR TESTING
// ----------------------------------------------------------------------------

                           // ====================
                           // class IntValueIsHash
                           // ====================

class IntValueIsHash {
    // This class provides a hash algorithm that provides the "identity"
    // mapping from key to hash.

  public:
    size_t operator()(const int key) const
        // Return the specified 'key'.
    {
        return static_cast<size_t>(key);
    }
};

                             // ================
                             // class SeedIsHash
                             // ================

class SeedIsHash {
    // This class provides a hash algorithm that returns the specified seed
    // value for all hash requests.

    size_t d_seed;  // value to return for all hash requests

  public:
    // CREATORS
    SeedIsHash()
        // Create a 'SeedIsHash' object having 0 as the seed value.
    : d_seed(0)
    {
    }

    explicit SeedIsHash(size_t seed)
        // Create a 'SeedIsHash' object having the specified 'seed'.
    : d_seed(seed)
    {
    }

    SeedIsHash(const SeedIsHash& original)
        // Create a 'SeedIsHash' object having the value of the specified
        // 'original' object.
    : d_seed(original.d_seed)
    {
    }

    // MANIPULATORS
    SeedIsHash& operator=(const SeedIsHash& rhs)
        // Assign to this object the value of the specified 'rhs', and return a
        // reference providing modifiable access to this object.
    {
        d_seed = rhs.d_seed;
        return *this;
    }

    // ACCESSORS
    size_t operator()(const int&) const
        // Return the provided-at-constuction seed value.
    {
        return d_seed;
    }
};

                            // ==================
                            // class LengthIsHash
                            // ==================

class LengthIsHash {
    // This class provides a hash algorithm that returns the length of the
    // provided 'const char *' as the hash value.

  public:
    size_t operator()(const char *key) const
        // Return the specified 'key'.
    {
        return strlen(key);
    }
};

// ============================================================================
//                                USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Obtaining Most-Significant and Least-Significant Bit Variation
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we are using a hash table that requires variation in the
// most-significant and least-significant bits to operate efficiently (e.g.,
// Abseil's flat hash map).  The keys we will be using are small integer
// values, and we would like to use the identity function as the hash functor
// since it is efficient.  A simple and efficient method to obtain a hash
// functor with the necessary qualities is to wrap the identity functor with
// 'bslh::FibonaccaBadHashWrapper'.
//
// First, we define our 'IdentityHash' class.
//..
    class IdentityHash {
        // This class provides a hash algorithm that provides the "identity"
        // mapping from key to hash.

      public:
        size_t operator()(const int key) const
            // Return the specified 'key'.
        {
            return static_cast<size_t>(key);
        }
    };
//..

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int     test = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose = argc > 2;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
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

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

// Then, we instantiate an instance of the identity functor and the wrapped
// functor.
//..
    IdentityHash                                identity;
    bslh::FibonacciBadHashWrapper<IdentityHash> wrapped;
//..
// Finally, we examine the range of values obtained from small integer values:
//..
    if (8 == sizeof(size_t)) {
        ASSERT(18446744073709551614ull == identity(-2));
        ASSERT(18446744073709551615ull == identity(-1));
        ASSERT(                      0 == identity( 0));
        ASSERT(                      1 == identity( 1));
        ASSERT(                      2 == identity( 2));

        ASSERT(14092058508772706262ull == wrapped(-2));
        ASSERT( 7046029254386353131ull == wrapped(-1));
        ASSERT(                      0 == wrapped( 0));
        ASSERT(11400714819323198485ull == wrapped( 1));
        ASSERT( 4354685564936845354ull == wrapped( 2));
    }
    else {
        ASSERT(4294967294u == identity(-2));
        ASSERT(4294967295u == identity(-1));
        ASSERT(          0 == identity( 0));
        ASSERT(          1 == identity( 1));
        ASSERT(          2 == identity( 2));

        ASSERT(  23791574u == wrapped(-2));
        ASSERT(2159379435u == wrapped(-1));
        ASSERT(          0 == wrapped( 0));
        ASSERT(2135587861u == wrapped( 1));
        ASSERT(4271175722u == wrapped( 2));
    }
//..
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // 'operator()'
        //
        // Ensure the 'FibonacciBadHashWrapper' hash computing operator works
        // as expected.
        //
        // Concerns:
        //: 1 The signature and return type are as expected.
        //:
        //: 2 The 'operator()' return value is as expected for the
        //:   user-specified primary hash functor.
        //:
        //: 3 Various types can be supplied as the 'KEY'.
        //
        // Plan:
        //: 1 Use the address of 'operator()' to initialize a function pointer
        //:   having the appropriate structure for the operator defined in this
        //:   component.  (C-1)
        //:
        //: 1 Using 'SeedIsHash' and 'IntValueIsHash', directly test
        //:   'operator()'.  (C-2)
        //:
        //: 2 Use 'LengthIsHash' to verify other 'KEY' types can be used.
        //:   (C-3)
        //
        // Testing:
        //   size_t operator()(const KEY& key) const;
        // --------------------------------------------------------------------

        if (verbose) printf("'operator()'\n"
                            "============\n");

        if (verbose) printf("Testing signature and return value.\n");
        {
            typedef bslh::FibonacciBadHashWrapper<IntValueIsHash> Obj;

            typedef size_t (Obj::*funcPtr)(const int&) const;

            // Verify that the signature and return type are as expected.

            funcPtr oper = &Obj::operator();

            // Quash unused variable warnings.

            Obj    a;
            size_t rv = (a.*oper)(0);
            ASSERT(0 == rv);
        }

        if (verbose) printf("Testing return value.\n");
        {
            {
                typedef bslh::FibonacciBadHashWrapper<SeedIsHash> Obj;

                Obj mX(SeedIsHash(0));  const Obj& X = mX;

                ASSERT(0 == X(0));
                ASSERT(0 == X(1));
                ASSERT(0 == X(2));
            }
            {
                typedef bslh::FibonacciBadHashWrapper<SeedIsHash> Obj;

                const size_t k_FIBONACCI_HASH_OF_ONE =
                         static_cast<size_t>(Obj::k_FIBONACCI_HASH_MULTIPLIER);

                Obj mX(SeedIsHash(1));  const Obj& X = mX;

                ASSERT(k_FIBONACCI_HASH_OF_ONE == X(0));
                ASSERT(k_FIBONACCI_HASH_OF_ONE == X(1));
                ASSERT(k_FIBONACCI_HASH_OF_ONE == X(2));
            }
            {
                typedef bslh::FibonacciBadHashWrapper<SeedIsHash> Obj;

                const size_t k_FIBONACCI_HASH_OF_TWO =
                     static_cast<size_t>(Obj::k_FIBONACCI_HASH_MULTIPLIER * 2);

                Obj mX(SeedIsHash(2));  const Obj& X = mX;

                ASSERT(k_FIBONACCI_HASH_OF_TWO == X(0));
                ASSERT(k_FIBONACCI_HASH_OF_TWO == X(1));
                ASSERT(k_FIBONACCI_HASH_OF_TWO == X(2));
            }
            {
                typedef bslh::FibonacciBadHashWrapper<IntValueIsHash> Obj;

                const size_t k_FIBONACCI_HASH_OF_ONE =
                         static_cast<size_t>(Obj::k_FIBONACCI_HASH_MULTIPLIER);
                const size_t k_FIBONACCI_HASH_OF_TWO =
                     static_cast<size_t>(Obj::k_FIBONACCI_HASH_MULTIPLIER * 2);

                Obj mX;  const Obj& X = mX;

                ASSERT(                      0 == X(0));
                ASSERT(k_FIBONACCI_HASH_OF_ONE == X(1));
                ASSERT(k_FIBONACCI_HASH_OF_TWO == X(2));
            }
            {
                typedef bslh::FibonacciBadHashWrapper<LengthIsHash> Obj;

                const size_t k_FIBONACCI_HASH_OF_ONE =
                         static_cast<size_t>(Obj::k_FIBONACCI_HASH_MULTIPLIER);
                const size_t k_FIBONACCI_HASH_OF_TWO =
                     static_cast<size_t>(Obj::k_FIBONACCI_HASH_MULTIPLIER * 2);

                Obj mX;  const Obj& X = mX;

                ASSERT(                      0 == X(""));
                ASSERT(k_FIBONACCI_HASH_OF_ONE == X("a"));
                ASSERT(k_FIBONACCI_HASH_OF_TWO == X("ab"));
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // COPY-ASSIGNMENT OPERATOR
        //
        // Ensure the 'FibonacciBadHashWrapper' copy-assignment operator works
        // as expected.
        //
        // Concerns:
        //: 1 The signature and return type are as expected.
        //:
        //: 2 The copy-assignment operator can change the value of any
        //:   modifiable target object to that of any source object.
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
        //:   copy-assignment operator defined in this component.  (C-1)
        //:
        //: 2 Create three objects of distinct value: X, Y, and Z.  Copy-assign
        //:   the value of Y to X.  Verify the change to object X, the lack of
        //:   change to object Y, and the return value of the operation is a
        //:   reference to X.  Copy-assign the value of Z to X.  Verify the
        //:   change to object X, the lack of change to object Z, and the
        //:   return value of the operation is a reference to X.  (C-2..4)
        //:
        //: 3 Create two objects of distinct value, X and Y.  Copy-assign X to
        //:   itself and verify the value of X has not changed.  Copy-assign Y
        //:   to itself and verify the value of Y has not changed.  (C-5)
        //
        // Testing:
        //   FibonacciBadHashWrapper& operator=(const FBHAW& rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("COPY-ASSIGNMENT OPERATOR\n"
                            "========================\n");

        if (verbose) printf("Testing signature and return value.\n");
        {
            typedef bslh::FibonacciBadHashWrapper<IntValueIsHash> Obj;

            typedef Obj& (Obj::*operatorPtr)(const Obj&);

            // Verify that the signature and return type are standard.

            operatorPtr operatorAssignment = &Obj::operator=;

            (void)operatorAssignment;  // quash potential compiler warning
        }

        if (verbose) printf("Testing assignment from another object.\n");
        {
            typedef bslh::FibonacciBadHashWrapper<SeedIsHash> Obj;

            const size_t k_FIBONACCI_HASH_OF_ONE =
                         static_cast<size_t>(Obj::k_FIBONACCI_HASH_MULTIPLIER);
            const size_t k_FIBONACCI_HASH_OF_TWO =
                     static_cast<size_t>(Obj::k_FIBONACCI_HASH_MULTIPLIER * 2);

            Obj mX(SeedIsHash(0));  const Obj& X = mX;
            Obj mY(SeedIsHash(1));  const Obj& Y = mY;
            Obj mZ(SeedIsHash(2));  const Obj& Z = mZ;

            {
                Obj& rv = (mX = Y);

                ASSERT(&rv == &mX);
            }

            ASSERT(k_FIBONACCI_HASH_OF_ONE == X(0));
            ASSERT(k_FIBONACCI_HASH_OF_ONE == Y(0));
            ASSERT(k_FIBONACCI_HASH_OF_TWO == Z(0));

            {
                Obj& rv = (mX = Z);

                ASSERT(&rv == &mX);
            }

            ASSERT(k_FIBONACCI_HASH_OF_TWO == X(0));
            ASSERT(k_FIBONACCI_HASH_OF_ONE == Y(0));
            ASSERT(k_FIBONACCI_HASH_OF_TWO == Z(0));
        }

        if (verbose) printf("Testing self-assignment.\n");
        {
            typedef bslh::FibonacciBadHashWrapper<SeedIsHash> Obj;

            const size_t k_FIBONACCI_HASH_OF_ONE =
                         static_cast<size_t>(Obj::k_FIBONACCI_HASH_MULTIPLIER);
            const size_t k_FIBONACCI_HASH_OF_TWO =
                     static_cast<size_t>(Obj::k_FIBONACCI_HASH_MULTIPLIER * 2);

            Obj mX(SeedIsHash(1));  const Obj& X = mX;
            Obj mY(SeedIsHash(2));  const Obj& Y = mY;

            {
                Obj& rv = (mX = X);

                ASSERT(&rv == &mX);
            }

            ASSERT(k_FIBONACCI_HASH_OF_ONE == X(0));
            ASSERT(k_FIBONACCI_HASH_OF_TWO == Y(0));

            {
                Obj& rv = (mY = Y);

                ASSERT(&rv == &mY);
            }

            ASSERT(k_FIBONACCI_HASH_OF_ONE == X(0));
            ASSERT(k_FIBONACCI_HASH_OF_TWO == Y(0));
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CREATORS
        //   Ensure the 'FibonacciBadHashWrapper' constructors and
        //   destructor operate as expected.
        //
        // Concerns:
        //: 1 A default constructed 'FibonacciBadHashWrapper' stores a
        //:   default constructed hash functor.
        //:
        //: 2 The value constructor stores the supplied hash functor.
        //:
        //: 3 The copy constructor functions as expected.
        //:
        //: 4 The 'FibonacciBadHashWrapper' can wrap all compliant
        //:   'HASH' types.
        //:
        //: 5 The destructor functions as expected.
        //
        // Plan:
        //: 1 Default construct 'FibonacciBadHashWrapper<SeedIsHash>'
        //:   objects and directly verify the return value of 'operator()' for
        //:   a variety of key values.  (C-1)
        //:
        //: 2 Create 'FibonacciBadHashWrapper<SeedIsHash>' objects
        //:   with 'SeedIsHash' objects and directly verify the return value of
        //:   'operator()' for a variety of key values.  Note that the
        //:   consistency between the return value of 'operator()' and the
        //:   expected return value is a provisional test; 'operator()' is
        //:   fully tested in TC 4.  (C-2)
        //:
        //: 3 Directly test the copy constructor.  (C-3)
        //:
        //: 4 Create and exercise a
        //:   'FibonacciBadHashWrapper<IntValueIsHash>' object.  (C-4)
        //:
        //: 5 Allow all created objects to go out-of-scope.  (C-5)
        //
        // Testing:
        //   FibonacciBadHashWrapper();
        //   FibonacciBadHashWrapper(const HASH& hash);
        //   FibonacciBadHashWrapper(const FBHAW& original);
        //   ~FibonacciBadHashWrapper();
        // --------------------------------------------------------------------

        if (verbose) printf("CREATORS\n"
                            "========\n");

        if (verbose) printf("Testing default constructor.\n");
        {
            typedef bslh::FibonacciBadHashWrapper<SeedIsHash> Obj;

            Obj hash;

            ASSERT(0 == hash(0));
            ASSERT(0 == hash(1));
        }

        if (verbose) printf("Testing value constructor.\n");
        {
            typedef bslh::FibonacciBadHashWrapper<SeedIsHash> Obj;

            SeedIsHash primaryHash(0);
            Obj        hash(primaryHash);

            ASSERT(0 == hash(0));
            ASSERT(0 == hash(1));
        }
        {
            typedef bslh::FibonacciBadHashWrapper<SeedIsHash> Obj;

            const size_t k_FIBONACCI_HASH_OF_ONE =
                         static_cast<size_t>(Obj::k_FIBONACCI_HASH_MULTIPLIER);

            SeedIsHash primaryHash(1);
            Obj        hash(primaryHash);

            ASSERT(k_FIBONACCI_HASH_OF_ONE == hash(0));
            ASSERT(k_FIBONACCI_HASH_OF_ONE == hash(1));
        }

        if (verbose) printf("Testing copy constructor.\n");
        {
            typedef bslh::FibonacciBadHashWrapper<SeedIsHash> Obj;

            const size_t k_FIBONACCI_HASH_OF_ONE =
                         static_cast<size_t>(Obj::k_FIBONACCI_HASH_MULTIPLIER);

            SeedIsHash primaryHash(1);
            Obj        original(primaryHash);
            Obj        hash(original);

            ASSERT(k_FIBONACCI_HASH_OF_ONE == hash(0));
            ASSERT(k_FIBONACCI_HASH_OF_ONE == hash(1));

            ASSERT(original(0) == hash(0));
            ASSERT(original(1) == hash(1));
        }
        {
            typedef bslh::FibonacciBadHashWrapper<SeedIsHash> Obj;

            const size_t k_FIBONACCI_HASH_OF_ONE =
                         static_cast<size_t>(Obj::k_FIBONACCI_HASH_MULTIPLIER);

            SeedIsHash primaryHash(2);
            Obj        original(primaryHash);
            Obj        hash(original);

            ASSERT(2 * k_FIBONACCI_HASH_OF_ONE == hash(0));
            ASSERT(2 * k_FIBONACCI_HASH_OF_ONE == hash(1));

            ASSERT(original(0) == hash(0));
            ASSERT(original(1) == hash(1));
        }

        if (verbose) printf("Testing a different wrapped type.\n");
        {
            typedef bslh::FibonacciBadHashWrapper<IntValueIsHash> Obj;

            const size_t k_FIBONACCI_HASH_OF_ONE =
                         static_cast<size_t>(Obj::k_FIBONACCI_HASH_MULTIPLIER);

            Obj hash;

            ASSERT(                      0 == hash(0));
            ASSERT(k_FIBONACCI_HASH_OF_ONE == hash(1));
        }
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
        //: 1 Create an object wrapping 'IntValueIsHash' and verify the results
        //:   of 'operator()' for a few values.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("BREATHING TEST\n"
                            "==============\n");

        typedef bslh::FibonacciBadHashWrapper<IntValueIsHash> Obj;

        Obj mX;  const Obj& X = mX;

        if (8 == sizeof(size_t)) {
            ASSERT(                      0 == X(0));
            ASSERT(11400714819323198485ull == X(1));
            ASSERT( 4354685564936845354ull == X(2));
        }
        else {
            ASSERT(          0 == X(0));
            ASSERT(2135587861u == X(1));
            ASSERT(4271175722u == X(2));
        }
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
