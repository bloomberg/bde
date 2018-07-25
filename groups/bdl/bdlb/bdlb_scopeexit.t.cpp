// bdlb_scopeexit.t.cpp                                               -*-C++-*-
#include <bdlb_scopeexit.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bsl_functional.h>
#include <bslmf_movableref.h>
#include <bsls_buildtarget.h>
#include <bsls_compilerfeatures.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// .
//-----------------------------------------------------------------------------
// [ 2] ScopeExit(bslmf::MovableRef<ScopeExit> original)
// [ 2] ~ScopeExit()
// [ 3] release()
// [ 4] ScopeExitUtil::makeScopeExit()
// [ 5] template ScopeExit::ScopeExit(EXIT_FUNC_PARAM function)
// [ 6] ScopeExitAny
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 7] USAGE EXAMPLE
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
//                            TEST HELPERS UTILITY
// ----------------------------------------------------------------------------

                        // ========================
                        // struct ExitFunctionParam
                        // ========================

struct ExitFunctionParam {
    // A class that is convertible to 'ExitFunction'.

    // PUBLIC DATA
    int *d_counter_p;

    // CREATORS
    explicit ExitFunctionParam(int *counter);
        // Create an 'ExitFunctionParam' object with the specified 'counter'.

    // ACCESSORS
    void operator()() const;
        // Increment the counter by three.
};

                        // ------------------------
                        // struct ExitFunctionParam
                        // ------------------------

// CREATORS
ExitFunctionParam::ExitFunctionParam(int *counter)
: d_counter_p(counter)
{
}

// ACCESSORS
void ExitFunctionParam::operator()() const
{
    *d_counter_p += 3;
}

#ifdef BDE_BUILD_TARGET_EXC
                      // =============================
                      // struct ExitFunctionParamThrow
                      // =============================

struct ExitFunctionParamThrow {
    // A class that is convertible to 'ExitFunction', but the conversion
    // throws.

    // PUBLIC DATA
    int *d_counter_p;

    // CREATORS
    explicit ExitFunctionParamThrow(int *counter);
        // Create an 'ExitFunctionParam' object with the specified 'counter'.

    // ACCESSORS
    void operator()() const;
        // Increment the counter by two.
};

                      // -----------------------------
                      // struct ExitFunctionParamThrow
                      // -----------------------------

// CREATORS
ExitFunctionParamThrow::ExitFunctionParamThrow(int *counter)
: d_counter_p(counter)
{
}

// ACCESSORS
void ExitFunctionParamThrow::operator()() const
{
    *d_counter_p += 2;
}
#endif
                           // ==================
                           // class ExitFunction
                           // ==================

class ExitFunction {
    // Functor that increments a specified counter.

  private:
    // DATA
    int *d_counter_p;

  public:
    // CREATORS
    explicit ExitFunction(int *counter);
        // Create an 'ExitFunction' object that will increment the specified
        // 'counter' upon its destruction.

    ExitFunction(const ExitFunctionParam& parameter);               // IMPLICIT
        // Create an 'ExitFunction' object by "converting" the specified
        // 'parameter'.

#ifdef BDE_BUILD_TARGET_EXC
    ExitFunction(const ExitFunctionParamThrow&);                    // IMPLICIT
        // Throw an exception.
#endif

    // ACCESSORS
    void operator()() const;
        // Increment the counter.
};

                           // -------------------
                           // struct ExitFunction
                           // -------------------

// CREATORS
ExitFunction::ExitFunction(int *counter)
: d_counter_p(counter)
{
}

ExitFunction::ExitFunction(const ExitFunctionParam& parameter)
: d_counter_p(parameter.d_counter_p)
{
}

#ifdef BDE_BUILD_TARGET_EXC
ExitFunction::ExitFunction(const ExitFunctionParamThrow&)
{
    throw 42;
}
#endif

// ACCESSORS
void ExitFunction::operator()() const
{
    *d_counter_p += 1;
}

// ============================================================================
//                                USAGE EXAMPLE
// ----------------------------------------------------------------------------

#if (__cplusplus >= 201103L) || (defined(_MSC_VER) && _MSC_VER >= 1800)
#define BDLB_SCOPEEXIT_HAVE_CPP11
    // 'BDLB_SCOPEEXIT_HAVE_CPP11' is defined if the compiler supports basic
    // C++11 features, such as 'auto' and lambdas.
#endif

#if defined(BDE_BUILD_TARGET_EXC) && defined(BDLB_SCOPEEXIT_HAVE_CPP11) &&    \
    defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
#define BDLB_SCOPEEXIT_COMPILE_CPP11_EXAMPLE
    // 'BDLB_SCOPEEXIT_COMPILE_CPP11_EXAMPLE' is defined if all compilers
    // features (exceptions, C++11 'auto' and lambdas, and R-value references)
    // are present that are needed to compile the C++11 usage example.
#endif

#ifdef BDE_BUILD_TARGET_EXC
///Using a scope exit guard in C++11
///- - - - - - - - - - - - - - - - -
// Suppose we are creating a simple database that stores names and their
// associated addresses and we store the names and addresses in two separate
// "tables".  While adding data, these tables may fail the insertion, in which
// case we need to "roll back" the already inserted data, such as if we
// inserted the address first, we need to remove it if insertion of the
// associated name fails.
//
// First we emulate our our database access with the following simple functions
// for the sake of a working example:
//..
    int insertAddress(const char *address)
    {
        // Implementation is omitted for brevity
        (void)address;
        return 42;
    }

    int insertName(const char *name, int addressId)
    {
        // Implementation is omitted for brevity
        (void)name;
        (void)addressId;
        throw 42; // Simulate failure
    }

    int removedAddress = 0;
    void removeAddress(int id)
    {
        // Implementation is omitted for brevity
        removedAddress = id;
    }
//..
// As it is shown in the example above, our 'insertName' function fails.  Next,
// we draw up our insert function.
//..
#ifdef BDLB_SCOPEEXIT_COMPILE_CPP11_EXAMPLE
    int insertCustomer(const char *name, const char *address)
    {
//..
// Then, in the insert function we use a 'try' block to swallow the exception
// thrown by our fake database function.  In this try block we successfully
// insert the address.
//..
        int addressId = -1;
        try {
            addressId = insertAddress(address);
//..
// Next, before we try to insert the name, we need to set up the guard to
// remove the already inserted address when the name insert fails.
//..
            auto guard = bdlb::ScopeExitUtil::makeScopeExit(
                                           [=](){ removeAddress(addressId); });
//..
// Then we attempt to insert the name (which will always fail in our toy
// example).
//..
            int nameId = insertName(name, addressId);
//..
// Next, if the insertion succeeds we need to release the guard and return the
// ID.
//..
            guard.release();

            return nameId;                                            // RETURN
        }
//..
// Then we ignore the exception for the sake of this toy example.
//..
        catch (int) {
            // We ignore the exception for this example
        }
//..
// Finally we verify that the 'removeAddress' function was called with the
// right identifier.
//..
        ASSERT(removedAddress == addressId);

        return -1;
    }
//..
#endif

///Using a scope exit guard in C++03
///- - - - - - - - - - - - - - - - -
// Suppose we are in the same situation as in the C++11 example, but we have to
// create a C++03 solution.  C++03 does not support lambdas, so first we have
// to hand-craft a functor that calls 'removeAddress' with a given ID.
//..
    struct RemoveAddress {
        int d_id;

        explicit RemoveAddress(int id)
        : d_id(id)
        {
        }

        void operator()() const
        {
            removeAddress(d_id);
        }
    };
//..
// Then we basically have the same code as for C++11, except for the guard
// specification.
//..
    int insertCustomer03(const char *name, const char *address)
    {
        int addressId = -1;
        try {
            addressId = insertAddress(address);
//..
// The guards type is explicitly specified and the functor is used instead of
// a lambda.  Alternatively (for example if binding is involved) we could use
// 'ScopeExitAny' instead of a specific type.  Notice the extra parenthesis to
// avoid "the most vexing parse"
// (https://en.wikipedia.org/wiki/Most_vexing_parse) issue.  Since we are in
// C++03, we cannot use curly braces to avoid the issue.
//..
            bdlb::ScopeExit<RemoveAddress> guard((RemoveAddress(addressId)));

            int nameId = insertName(name, addressId);

            guard.release();

            return nameId;                                            // RETURN
        }
        catch (int) {
            // We ignore the exception for this example
        }
//..
// Finally we verify that the 'removeAddress' function was called with the
// right identifier.
//..
        ASSERT(removedAddress == addressId);

        return -1;
    }
//..
#endif // BDE_BUILD_TARGET_EXC

// ============================================================================
//                                 MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3; (void)veryVerbose;
    bool     veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: No memory is leaked.

    bslma::TestAllocator defaultAllocator("default", veryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 7: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //    leading comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;
#ifdef BDE_BUILD_TARGET_EXC
#ifdef BDLB_SCOPEEXIT_COMPILE_CPP11_EXAMPLE
        ///Using a scope exit guard in C++11
        ///- - - - - - - - - - - - - - - - -
        // See documentation at the definition of the function
        insertCustomer("Frank", "4 Goldfield Rd. Honolulu, HI 96815");
#endif
        removedAddress = 0;

        insertCustomer03("Joe", "151 S. Silver Spear St. La Porte, IN 46350");
#endif // BDE_BUILD_TARGET_EXC
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // SCOPEEXITANY
        //
        // Concerns:
        //: 1 'ScopeExitAny' can store and properly execute any guard.
        //
        // Plan:
        //: 1 Create 'bdlb::ScopeExitAny' guards using a different argument
        //:   types for the constructor ('ExitFunction'/'ExitFunctionParam').
        //:
        //: 2 Verify that the guard is created and invokes the exit function
        //:   upon its destruction.
        //
        // Testing:
        //   ScopeExitAny
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SCOPEEXITANY" << endl
                          << "============" << endl;

        int counter = 0;
        {
            bdlb::ScopeExitAny guard((ExitFunction(&counter)));
        }
        ASSERT(1 == counter);

        {
            bdlb::ScopeExitAny guard((ExitFunctionParam(&counter)));
        }
        ASSERT(4 == counter);
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // CONVERTING CONSTRUCTOR
        //
        // Concerns:
        //: 1 The converting constructor is in the overload set when the
        //:   constructor argument is convertible to the template argument
        //:   'EXIT_FUNC' but its not the same type (after decay).
        //:
        //: 2 The constructor runs the exit function if the copy construction
        //:   of the exit function throws.
        //
        // Plan:
        //: 1 Create a 'bdlb::ScopeExit<ExitFunction>' guard using a different
        //:   argument type for the constructor ('ExitFunctionParam').
        //:
        //: 2 Verify that the guard is created and invokes the exit function
        //:   upon its destruction.
        //:
        //: 3 Create a 'bdlb::ScopeExit<ExitFunction>' guard using an argument
        //:   type for the constructor ('ExitFunctionParamThrow') that causes
        //:   the constructor to throw.
        //:
        //: 4 Verify that the guard invokes 'operator()' of the
        //:   'ExitFunctionParamThrow' argument.
        //
        // Testing:
        //   template ScopeExit::ScopeExit(EXIT_FUNC_PARAM function)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CONVERTING CONSTRUCTOR" << endl
                          << "======================" << endl;

        int counter = 0;
        {
            bdlb::ScopeExit<ExitFunction> guard((ExitFunctionParam(&counter)));
        }
        ASSERT(1 == counter);

#ifdef BDE_BUILD_TARGET_EXC
        bool reallyThrewAnException = false;
        try {
            bdlb::ScopeExit<ExitFunction>
                                     guard((ExitFunctionParamThrow(&counter)));
        }
        catch (int) {
            reallyThrewAnException = true;
        }

        // Sanity check
        ASSERT(reallyThrewAnException);

        // 'ExitFunctionParamThrow' has increased the counter by two.
        ASSERT(3 == counter);
#endif // BDE_BUILD_TARGET_EXC
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // FACTORY FUNCTION
        //
        // Concerns:
        //: 1 The factory function allocates no memory.
        //: 2 The factory function creates a working guard.
        //
        // Plan:
        //: 1 Create a 'bdlb::ScopeExit' guard using 'bdlb::ScopeExitUtil'.
        //:
        //: 2 Using an allocator monitor verify that no memory was allocated
        //:
        //: 3 Verify that a functional guard was returned by testing that the
        //:   exit action is run during destruction.
        //
        // Testing:
        //   ScopeExitUtil::makeScopeExit()
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "FACTORY FUNCTION" << endl
                          << "================" << endl;

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

        int counter = 0;
        {
            bslma::TestAllocatorMonitor dam(&defaultAllocator);

            auto guard = bdlb::ScopeExitUtil::makeScopeExit(
                                                          [&](){ ++counter; });

            ASSERT(dam.isTotalSame());
        }

        // The guard runs the lambda on its destruction
        ASSERTV(counter, 1 == counter);
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // RELEASE
        //
        // Concerns:
        //: 1 The 'release()' method prevents the action from running.
        //
        // Plan:
        //: 1 Create a 'ScopeExit' guard, release it, and verify that its
        //:   action is not run on destruction.
        //
        // Testing:
        //   release()
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "RELEASE" << endl
                          << "=======" << endl;

        int counter = 0;
        {
            bdlb::ScopeExit<ExitFunction> guard((ExitFunction(&counter)));
            guard.release();

            ASSERTV(counter, 0 == counter);
        }

        // The guard does nothing on destruction.
        ASSERTV(counter, 0 == counter);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // MOVE CONSTRUCTOR
        //
        // Concerns:
        //: 1 The move constructor moves the action out of the guard into the
        //:   move target (action is executed once only).
        //
        // Plan:
        //: 1 Create a guard with a verifiable exit action.
        //:
        //: 2 Create a second guard by moving the first guard into it.
        //:
        //: 3 Verify no action has been taken.
        //:
        //: 4 Destroy the second guard.  Verify that the action has been taken.
        //:
        //: 5 Destroy the first guard.  Verify that the action is not taken
        //:   again.
        //
        // Testing:
        //   ScopeExit(bslmf::MovableRef<ScopeExit> original)
        //   ~ScopeExit()
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "MOVE CONSTRUCTOR" << endl
                          << "================" << endl;

        int counter = 0;
        {
            bdlb::ScopeExit<ExitFunction> guard1((ExitFunction(&counter)));
            {
                bdlb::ScopeExit<ExitFunction>
                                   guard2(bslmf::MovableRefUtil::move(guard1));

                // The action is not run yet.
                ASSERTV(counter, 0 == counter);
            }

            // The second guard has invoked the action.
            ASSERTV(counter, 1 == counter);
        }

        // The first guard did nothing on destruction, because has been
        // released by the second guard as part of the move operation.
        ASSERTV(counter, 1 == counter);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Create a scope exit guard that updates a counter on exit.
        //: 2 Destroy the guard.
        //: 3 Verify that the counter has been updated.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        int counter = 0;
        {
            bdlb::ScopeExit<ExitFunction> guard((ExitFunction(&counter)));

            // The action is not run yet.
            ASSERTV(counter, 0 == counter);
        }

        // The guard has invoked the action ('ExitFunction::operator()').
        ASSERTV(counter, 1 == counter);

      } break;

      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      } break;
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
