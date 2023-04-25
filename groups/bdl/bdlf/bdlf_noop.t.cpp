// bdlf_noop.t.cpp                                                    -*-C++-*-
#include <bdlf_noop.h>

#include <bsla_maybeunused.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_review.h>

#include <bsl_functional.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::endl;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component under test implements a functor with no state.  There are no
// Primary Manipulators, and the only Accessor is the function-call operator.
//
// Basic Accessor:
//: o 'operator()'
//
// Global Concerns:
//: o The test driver is robust w.r.t reuse in other, similar components.
//: o ACCESSOR methods are declared 'const'.
//: o No memory is ever allocated from the global allocator.
//
// Global Assumptions:
//: o ACCESSOR methods are 'const' thread-safe.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] bdlf::NoOp();
// [ 2] bdlf::~NoOp();
// [ 4] bdlf::NoOp(const NoOp& o);
//
// MANIPULATORS
// [ 5] operator=(const bdlf::NoOp& rhs);
//
// ACCESSORS
// [ 3] void operator()() const;
// TYPES AND TRAITS
// [ 6] NoOp::result_type
// [ 6] NoOp::is_trivially_copyable
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 7] USAGE EXAMPLE
// [ *] CONCERN: The test driver reusable w/other, similar components.
// [ *] CONCERN: In no case does memory come from the global allocator.
// [ 3] CONCERN: ACCESSOR methods are declared 'const'.
// [ 5] CONCERN: There is no temporary allocation from any allocator.
// ----------------------------------------------------------------------------

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

#define ASSERT_EQ(X,Y) ASSERTV(X, Y, X == Y)
#define ASSERT_NE(X,Y) ASSERTV(X, Y, X != Y)
#define ASSERT_LT(X,Y) ASSERTV(X, Y, X <  Y)
#define ASSERT_LE(X,Y) ASSERTV(X, Y, X <= Y)
#define ASSERT_GT(X,Y) ASSERTV(X, Y, X >  Y)
#define ASSERT_GE(X,Y) ASSERTV(X, Y, X >= Y)

// ============================================================================
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)

// ============================================================================
//                        GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdlf::NoOp Obj;
class NeverDefined;

void example1();
    // This function runs example 1.

void example2();
    // This function runs example 2.

///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: An Unwanted 'bsl::function' Callback
///- - - - - - - - - - - - - - - - - - - - - - - -
// Asynchronous systems often provide callback-based interfaces that invoke
// supplied callbacks at later points in time, often when an event has
// occurred, like the receipt of a data packet on an internet socket.
// Occasionally, we may need to supply these interfaces with a callback even
// though there's nothing we need the callback to do. In these cases, it's
// often possible to supply a "no-op" callback that does nothing.
//
// Consider, for example, the following hypothetical interface to an
// asynchronous system:
//..
    struct MyAsyncSystemUtil {
        // TYPES
        typedef bsl::function<void(int)> StatusReceivedCallback;
            // 'StatusReceivedCallback' is an alias for a callback functor that
            // takes as an argument an integer status indicating success (0),
            // or failures (any non-zero value).

        static void sendPing(const StatusReceivedCallback& callback);
            // Send a message to the server that this client is still active.
            // Invoke the specified 'callback' with the value '0' on the worker
            // thread when this client receives, within the configured time
            // limit, an acknowledgement from the server that it received the
            // message.  Otherwise, invoke the 'callback' with a non-zero value
            // on the worker thread after the configured time limit has expired
            // or the server or network responds with an error, whichever comes
            // first.
    };
//..
// Suppose that we are writing a client for this system that does not need to
// be concerned about the status of 'ping' messages sent to the server, then we
// can use 'bdlf::NoOp' to ignore the acknowledgements:
//..
    void example1()
    {
        MyAsyncSystemUtil::sendPing(bdlf::NoOp());
    }
//..
//
///Example 2: An Unwanted Template Type Callback
///- - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we are working with an asynchronous system whose interface
// requires a template for a callback instead of a 'bsl::function'.
//
// Consider, for example, the following hypothetical interface to an
// asynchronous system:
//..
    template <class t_CALLBACK>
    class AsyncSystem
    {
        // This class implements an asynchronous system that does important
        // example work.  While the important example work is being done, a
        // callback mechanism of type 't_CALLBACK' is used to provide estimates
        // about the percentage of the work that is complete.  The type
        // 't_CALLBACK' must be callable with a single 'int' parameter [0,100].

        // DATA
        t_CALLBACK d_callback;  // callable that gives feedback when 'run' runs

        public:
        // CREATORS
        explicit AsyncSystem(const t_CALLBACK& callback = t_CALLBACK());
            // Create a 'AsyncSystem' object that can be used to do important
            // example work.  Optionally specify 'callback', a 't_CALLBACK',
            // object to be used for giving feedback about progress.  If no
            // 'callback' is specified, a default-constructed object of type
            // 't_CALLBACK' will be used.

        // MANIPULATORS
        void run();
            // Do the very important example work that this class is designed
            // to do.  While doing said work, periodically invoke the
            // 'callback' object provided to the constructor with an estimated
            // percentage of the task complete as an 'int'.
    };
//..
// Suppose that we are writing a client of this system that has no useful way
// to report the progress from the callback, then we can use 'bdlf::NoOp' to
// ignore the progress reports:
//..
    void example2()
    {
        AsyncSystem<bdlf::NoOp> mySystem;
        mySystem.run();
    }
//..

void MyAsyncSystemUtil::sendPing(const StatusReceivedCallback& callback)
{
    callback(0);
    callback(1);
}

template <class t_CALLBACK>
AsyncSystem<t_CALLBACK>::AsyncSystem(const t_CALLBACK& callback)
: d_callback(callback)
{}

template <class t_CALLBACK>
void AsyncSystem<t_CALLBACK>::run()
{
    d_callback(0);
    d_callback(1);
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14
constexpr void constexprFunction()
    // Tests that operator() is 'constexpr'
{
    Obj X;
    X();
}
#endif  // BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
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

        if (verbose) bsl::cout << "\nUSAGE EXAMPLE" << bsl::endl
                               <<   "=============" << bsl::endl;

        example1();
        example2();
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TYPES AND TRAITS
        //   Ensure that types and traits of the class are as expected.
        //
        // Concerns:
        //: 1 The type 'result_type' should be publicly accessible.
        //:
        //: 2 The type trait 'is_trivially_copyable' should be publicly
        //:   accessible.
        //
        // Plan:
        //: 1 Use metafunctions to tests for types and traits. (C-1..2)
        //
        // Testing:
        //   NoOp::result_type
        //   NoOp::is_trivially_copyable
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTYPES AND TRAITS" << endl
                          <<   "================" << endl;

        typedef bsl::is_same<void, Obj::result_type> IsSame;
        BSLMF_ASSERT(IsSame::value == true);
        BSLMF_ASSERT(bsl::is_trivially_copyable<Obj>::value == true);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        BSLMF_ASSERT(std::is_trivially_copyable<Obj>::value == true);
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // COPY-ASSIGNMENT OPERATOR
        //   Ensure that we can assign the value of any object of the class to
        //   any modifiable object of the class.
        //
        // Concerns:
        //: 1 The assignment operator can assign the value of any modifiable
        //:   target object to that of any source object.
        //:
        //: 2 No memory is allocated by assignment of one object to another.
        //:
        //: 3 The signature and return type are standard.
        //:
        //: 4 The reference returned is to the target object (i.e., '*this').
        //:
        //: 5 Assigning an object to itself behaves as expected (alias-safety).
        //
        // Plan:
        //: 1 Use the address of 'operator=' to initialize a member-function
        //:   pointer having the appropriate signature and return type for the
        //:   copy-assignment operator defined in this component.  (C-3)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Create two objects, 'mX' and 'mY', and two references providing
        //:   non-modifiable access, 'X' and 'Y', respectively.
        //:
        //: 4 Assign all permutations of references to all permutations of
        //:   references providing modifiable access, and capture the returned
        //:   reference.  Ensure the returned reference is a reference to the
        //:   target object.  (C-1,3..4)
        //:
        //: 5 Use the test allocator from P-1 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-3)
        //
        // Testing:
        //   operator=(const bdlf::NoOp& rhs);
        //   CONCERN: There is no temporary allocation from any allocator.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY-ASSIGNMENT OPERATOR" << endl
                          << "========================" << endl;

        if (verbose) cout <<
                 "\nAssign the address of the operator to a variable." << endl;
        {
            typedef Obj& (Obj::*operatorPtr)(const Obj&);

            BSLA_MAYBE_UNUSED operatorPtr operatorAssignment = &Obj::operator=;
        }
        if (verbose) cout <<
                      "\nInstall test allocator as default allocator." << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout << "\nAssign permutations of values." << endl;

        Obj mX;  const Obj& X = mX;
        Obj mY;  const Obj& Y = mY;

        ASSERT(&mX == &(mX = mX));
        ASSERT(&mX == &(mX =  X));
        ASSERT(&mX == &(mX = mY));
        ASSERT(&mX == &(mX =  Y));
        ASSERT(&mY == &(mY = mX));
        ASSERT(&mY == &(mY =  X));
        ASSERT(&mY == &(mY = mY));
        ASSERT(&mY == &(mY =  Y));

        if (verbose) cout << "\nEnsure no memory was allocated." << endl;

        ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        //   Ensure that we can create a distinct object of the class from any
        //   other one.
        //
        // Concerns:
        //: 1 The copy constructor creates an object.
        //:
        //: 2 The original object is passed as a 'const' reference.
        //:
        //: 3 No memory is allocated by copy construction.
        //
        // Plan:
        //: 1 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 2 Create a constant object 'X'.
        //:
        //: 3 Create a copy of 'X', called 'Y'.  (C-1..2)
        //:
        //: 4 Use the test allocator from P-1 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-3)
        //
        // Testing:
        //   bdlf::NoOp(const NoOp& o);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY CONSTRUCTOR" << endl
                          << "================" << endl;

        if (verbose) cout <<
                      "\nInstall test allocator as default allocator." << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout << "\nMake a copy." << endl;

        const Obj X;
        Obj       mY(X);

        if (verbose) cout << "\nEnsure no memory was allocated." << endl;

        ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // BASIC ACCESSOR
        //   Ensure the function-call operator work as expected by accepting
        //   any parameters of any type and doing nothing.
        //
        // Concerns:
        //: 1 The function-call operator can be invoked with any parameters.
        //:
        //: 2 The function-call operator return type is void.
        //:
        //: 3 The function-call operator is declared 'const'.
        //:
        //: 4 The function-call operator is declared 'constexpr' where
        //:   supported.
        //:
        //: 5 The function-call operator is declared 'noexcept' where
        //:   supported.
        //:
        //: 6 No memory is allocated by the function-call operator.
        //
        // Plan:
        //: 1 Verify the void return type of the function-call operator by
        //:   assigning it to a function pointer with void return type.  (C-2)
        //:
        //: 2 Where 'noexcept' is supported, assign the function call operator
        //:   to a 'noexcept' function pointer.  (C-5)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
        //:   current default allocator (note that a ubiquitous test allocator
        //:   is already installed as the global allocator).
        //:
        //: 3 Use the default constructor to create an object 'mX'.
        //:
        //: 4 Create a constant reference 'X' to 'mX'.
        //:
        //: 5 Invoke the function-call operator on 'mX' with many different
        //:   numbers of parameters of differrent types.  (C-1,3)
        //:
        //: 6 Use the test allocator from P1 to verify that no memory is
        //:   allocated by the default constructor.  (C-6)
        //
        // Testing:
        //   void operator()() const;
        //   CONCERN: ACCESSOR methods are declared 'const'.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BASIC ACCESSOR" << endl
                          << "==============" << endl;

        typedef void (Obj::*NoParamReturnVoidFnPtr)() const;
        typedef void (Obj::*IntParamReturnVoidFnPtr)(const int&) const;

        BSLA_MAYBE_UNUSED NoParamReturnVoidFnPtr  noParam  = &Obj::operator();
        BSLA_MAYBE_UNUSED IntParamReturnVoidFnPtr intParam = &Obj::operator();

#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
        // bde-verify doesn't like noexcept in typedefs
        BSLA_MAYBE_UNUSED
        void (Obj::*noExcept)() const noexcept = &Obj::operator();
#endif  // BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT

        bslma::TestAllocator da("default", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        Obj mX;  const Obj& X = mX;

        X();
        X(3);
        X(static_cast<NeverDefined*>(0));
        X(cout, 0, 3.14);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14
        constexpr Obj Y;
        Y();
        Y(3);
        Y(static_cast<NeverDefined*>(0));
        Y(cout, 0, 3.14);
        constexprFunction();
#endif  // BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14

        ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DEFAULT CTOR & DTOR
        //   Ensure that we can use the default constructor to create an
        //   object and use the destructor to destroy it safely.
        //
        // Concerns:
        //: 1 An object can be created with the default constructor.
        //:
        //: 2 Where 'constexpr' is supported, the default constructor can be
        //:   used to create a 'constexpr' object.
        //:
        //: 2 There is no allocation from the default allocator by the default
        //:   constructor.
        //
        // Plan:
        //: 1 Create a 'bslma::TestAllocator' object, and install it as the
        //:   current default allocator (note that a ubiquitous test allocator
        //:   is already installed as the global allocator).
        //:
        //: 2 Use the default constructor to create an object 'mX'.  (C-1)
        //:
        //: 3 Use the test allocator from P1 to verify that no memory is
        //:   allocated by the default constructor.  (C-2)
        //
        // Testing:
        //   bdlf::NoOp();
        //   bdlf::~NoOp();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "DEFAULT CTOR & DTOR" << endl
                          << "===================" << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        BSLA_MAYBE_UNUSED Obj mX;
#ifdef BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR
        BSLA_MAYBE_UNUSED constexpr Obj Y;
#endif  // BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR

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
        //: 1 Create an object 'x' (default ctor).
        //: 2 Create an object 'y' (copy ctor).
        //: 3 Assign 'x' to 'y' (assignment).
        //: 3 Apply function call operator to 'x' and 'y' with a variety of
        //:   parameters.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;


        Obj x;
        Obj y((x));  // double parens to avoid most vexing parse
        y = x;
        x("cstring", 0, 3.1415, y);
        y("cstring", 0, 3.1415, y);

        // Make sure bdlf::noOp is available
        bdlf::noOp(bsl::cout, 3.0221e23);
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
// Copyright 2023 Bloomberg Finance L.P.
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
