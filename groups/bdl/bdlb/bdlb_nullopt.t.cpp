// bdlb_nullopt.t.cpp                                                 -*-C++-*-
#include <bdlb_nullopt.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_objectbuffer.h>
#include <bsls_review.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>


using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// This component consists of an empty tag type and a constant object of that
// type.  There is minimal functionality to test.
//-----------------------------------------------------------------------------
// [1] nullOpt value
// [1] NullOptType copy/destroy operations
//-----------------------------------------------------------------------------
// [2] USAGE EXAMPLE

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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------


//=============================================================================
//                             USAGE EXAMPLES
//-----------------------------------------------------------------------------

// BDE_VERIFY pragma: push    // Usage examples relax rules for doc clarity
// BDE_VERIFY pragma: -AQJ02  // Headers are included where examples need them
// BDE_VERIFY pragma: -FABC01 // Functions ordered for expository purpose
// BDE_VERIFY pragma: -FD01   // Function contracts replaced by expository text
// BDE_VERIFY pragma: -FD03   // Function contracts replaced by expository text

namespace test_case_1 {
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: ...
/// - - - - - - -
// Suppose we are implementing a type that has an optional null (empty) state,
// such as 'NullableValue'.  First we declare the class template.
//..
    namespace xyza {
    template <class TYPE>
    class NullableValue {
        // This class can hold a value of (the template parameter) 'TYPE' or be
        // in an empty "null" state.
//..
// Then, we privide a buffer than can be uninitialized or hold a valid object,
// and a 'bool' variable to indicate whether an object is stored in the buffer.
//..
        bsls::ObjectBuffer<TYPE> d_value;
        bool                     d_isNull;
//..
// Then, we provide a constructor that makes a copy of an argument of the given
// 'TYPE'.
//..
      public:
        // CREATORS
        explicit NullableValue(const TYPE& value);
            // Create an object having the specified 'value'.
//..
// Next, we provide an implicit constructor using 'bdlb::NullOptType' to allow
// our users to explicitly create a nullable object in the null state.
//..
        NullableValue(bdlb::NullOptType);                           // IMPLICIT
            // Create a null object.  Note that the passed argument is used
            // only to select this constructor overload, the actual value is
            // not used.
//..
// Then we provide accessors to indicate whether the object is in the null
// state, and to return the wrapped value if one is avaialble.
//..
        // ACCESSORS
        const TYPE& getValue() const;
            // Return a reference to the non-modifiable object held by this
            // 'NullableValue'.  The behavior is undefined if this object is
            // null.

        bool isNull() const;
            // Return 'false' if this object holds a value, and 'true'
            // otherwise.  Note that a 'NullableValue' that does not hold an
            // object is said to be "null".
    };

    }  // close namespace xyza
//..
// Now we can write a function that consumes a nullable value.
//..
    namespace xyzb {

    struct Utility {
        static int unwrap(const xyza::NullableValue<int>& optionalValue);
            // Return the value of the specified 'optionalValue' if it holds a
            // value; otherwise, return 0.
    };

    int Utility::unwrap(const xyza::NullableValue<int>& optionalValue) {
        if (optionalValue.isNull()) {
            return 0;                                                 // RETURN
        }

        return optionalValue.getValue();
    }

    }  // close namespace xyzb
//..
// Finally, our clients can call 'xyza::Utility::unwrap' with either a nullable
// value of their own, or with 'bdlb::nullOpt' to indicate a null object.
//..
    int xyz_main() {
        return xyzb::Utility::unwrap(bdlb::nullOpt);
    }
//..

// Implementation details elided from the example

namespace xyza {

template <class TYPE>
inline
NullableValue<TYPE>::NullableValue(const TYPE& value)
: d_value()
, d_isNull(false)
{
    new(d_value.address()) TYPE(value);
}

template <class TYPE>
inline
NullableValue<TYPE>::NullableValue(bdlb::NullOptType)
: d_value()
, d_isNull(true)
{
}

template <class TYPE>
inline
const TYPE& NullableValue<TYPE>::getValue() const {
    return d_value.object();
}

template <class TYPE>
inline
bool  NullableValue<TYPE>::isNull() const {
    return d_isNull;
}

}  // close namespace xyza


}  // close namespace test_case_1
// BDE_VERIFY pragma: pop  // end of usage example-example relaxed rules

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)        veryVerbose;  // unused variable warning
    (void)    veryVeryVerbose;  // unused variable warning
    (void)veryVeryVeryVerbose;  // unused variable warning

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example compiles and runs.
        //
        // Plan:
        //: 1 For concern 1, copy the usage example from the header file.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

        ASSERT(0 == test_case_1::xyz_main());

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 There is a constant value of type 'bdlb::NullOptType' named
        //:   'bdlb::nullOpt'.
        //:
        //: 2 Values of type 'NullOptType' can be copied and assigned to.
        //
        // Plan:
        //: 1 Pass the address of the 'nullOpt' value to a function overload
        //:   set that can determine that it has the proper cv-qualification.
        //:   (C-1)
        //:
        //: 2 Declare a local object of type 'NullOptType' that is a copy of
        //:   'nullOpt', relying on the copy constructor as the only user
        //:   accessible constructor for this type. (C-2)
        //:
        //: 3 Assign a second copy of 'nullOpt' to the local object, showing
        //:   that the implicitly declare copy operation is available (and
        //:   useless). (C-2)
        //
        // Testing:
        //   nullOpt value
        //   NullOptType copy/destroy operations
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        struct Local {
            static bool isConst(bdlb::NullOptType*)           { return false; }
            static bool isConst(const bdlb::NullOptType*)     { return true;  }
            static bool isConst(volatile bdlb::NullOptType*)  { return false; }
            static bool isConst(const volatile bdlb::NullOptType*)
                                                              { return false; }
        };

        ASSERT(Local::isConst(&bdlb::nullOpt));

        bdlb::NullOptType copy = bdlb::nullOpt;
        copy = bdlb::nullOpt;

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
