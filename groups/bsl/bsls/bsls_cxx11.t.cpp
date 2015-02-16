// bsls_cxx11..t.cpp                                                  -*-C++-*-

#include <bsls_cxx11.h>

#include <stdlib.h>
#include <iostream>

// Warning: the following 'using' declarations interfere with the testing of
// the macros defined in this component.  Please do not uncomment them.
// using namespace BloombergLP;
// using namespace std;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
//-----------------------------------------------------------------------------
// [ 1] BSLS_CXX11_EXPLICIT
// [ 2] BSLS_CXX11_FINAL (class)
// [ 3] BSLS_CXX11_FINAL (function)
// [ 4] BSLS_CXX11_OVERRIDE
//-----------------------------------------------------------------------------
// [ 5] MACRO SAFETY
// [ 6] USAGE EXAMPLE
//=============================================================================
//                       STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        std::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << std::endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================

namespace
{
    template <typename TYPE>
    class Optional
    {
        TYPE* d_value;
    public:
        Optional(): d_value() {}
        Optional(const TYPE& value): d_value(new TYPE(value)) {}
        ~Optional() { delete d_value; }
        // ...

        BSLS_CXX11_EXPLICIT operator bool() const { return d_value; }
    };
}

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;

    std::cout << "TEST " << __FILE__ << " CASE " << test << std::endl;

    switch (test) { case 0:
      case 6: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Run the usage example.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) std::cout << "\nTesting Usage Example"
                               << "\n=====================" << std::endl;

        Optional<int> value;
        ASSERT(bool(value) == false);
        if (value) { /*... */ }
#if __cplusplus < 201103 || defined(FAIL_USAGE_EXPLICIT)
        bool flag = value;
        ASSERT(flag == false);
#endif

        class FinalClass BSLS_CXX11_FINAL
        {
            int d_value;
        public:
            FinalClass(int value = 0): d_value(value) {}
            int value() const { return d_value; }
        };
        class FinalClassDerived
#if __cplusplus < 201103 || defined(FAIL_USAGE_FINAL_CLASS)
            : public FinalClass
#endif
        {
            int d_anotherValue;
        public:
            FinalClassDerived(int value)
                : d_anotherValue(2 * value) {
            }
            int anotherValue() const { return d_anotherValue; }
        };

        FinalClass finalValue(1);
        ASSERT(finalValue.value() == 1);
        FinalClassDerived derivedValue(2);
        ASSERT(derivedValue.anotherValue() == 4);

        struct FinalFunctionBase
        {
            virtual int f() { return 0; }
        };
        struct FinalFunctionDerived: FinalFunctionBase
        {
            int f()
#if __cplusplus < 201103 || defined(FAIL_USAGE_FINAL_FUNCTION)
                BSLS_CXX11_FINAL
#endif
            { return 1; }
        };
        struct FinalFunctionFailure: FinalFunctionDerived
        {
            int f() { return 2; }
        };

        FinalFunctionBase finalFunctionBase;
        ASSERT(finalFunctionBase.f() == 0);
        FinalFunctionDerived finalFunctionDerived;
        ASSERT(finalFunctionDerived.f() == 1);
        FinalFunctionFailure finalFunctionFailure;
        ASSERT(finalFunctionFailure.f() == 2);
        
        struct OverrideBase
        {
            virtual int f() const { return 0; }
        };
        struct OverrideSuccess: OverrideBase
        {
            int f() const BSLS_CXX11_OVERRIDE { return 1; }
        }; 
        struct OverrideFailure: OverrideBase
        {
            int f()
#if __cplusplus < 201103 || defined(FAIL_USAGE_OVERRIDE)
                BSLS_CXX11_OVERRIDE
#endif
            { return 2; }
        };

        OverrideBase overrideBase;
        ASSERT(overrideBase.f() == 0);
        OverrideSuccess overrideSuccess;
        ASSERT(overrideSuccess.f() == 1);
        ASSERT(static_cast<const OverrideBase&>(overrideSuccess).f() == 1);
        OverrideFailure overrideFailure;
        ASSERT(overrideFailure.f() == 2);
        ASSERT(static_cast<const OverrideBase&>(overrideFailure).f() == 0);

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING MACRO SAFETY
        //
        // Concerns:
        //   The macros should be usable outside 'namespace' 'BloombergLP' and
        //   'namespace' 'std'.
        //
        // Plan:
        //   The 'using' declarations at the top of the file are specifically
        //   commented out to test the concern.  If the concerns is violated,
        //   the test driver should fail to compile.
        //
        // Testing:
        //   MACRO SAFETY
        // --------------------------------------------------------------------

        if (verbose) std::cout << std::endl
                               << "TESTING MACRO SAFETY" << std::endl
                               << "====================" << std::endl;

        struct TestBase {
            virtual void f() = 0;
            virtual void g() = 0;
        };
        struct TestIntermediate
            : TestBase {
            void f() BSLS_CXX11_OVERRIDE BSLS_CXX11_FINAL {}
            void g() BSLS_CXX11_FINAL BSLS_CXX11_OVERRIDE {}
        };
        struct TestDerived BSLS_CXX11_FINAL
            : TestIntermediate {
            bool test() { return true; }
            BSLS_CXX11_EXPLICIT operator bool() const { return true; }
        };

        TestDerived object;
        ASSERT(object.test());
        ASSERT(bool(object));
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BSLS_CXX11_OVERRIDE
        //
        // Concerns:
        //   1) Marking an overriding function as 'override' using
        //      'BSLS_CXX11_OVERRIDE' shall compile OK both with C++03 and
        //      C++11 mode.
        //
        //   2) Marking a function which isn't overriding a 'virtual' function
        //      as 'override' using 'BSLS_CXX11_OVERRIDE' shall fail to
        //      compile when compiling using C++11 mode.  It may fail when
        //      using C++03 mode due to a warning about an overload hiding
        //      base class functions.
        //
        // Plan:
        //   Define a base class with a 'virtual' function and two derived
        //   classes which override the function correclty and incorrectly.  In
        //   both cases use the 'BSLS_CXX11_OVERRIDE' macro and determine if
        //   the compilation is successful.  The incorrect use is guarded by a
        //   the macro 'FAIL_OVERRIDE' to restrict compilation failure to
        //   compilations with this macro defined.
        // --------------------------------------------------------------------

        if (verbose) std::cout << std::endl
           << "TESTING BSLS_CXX11_OVERRIDE"
           << std::endl
           << "==============================================================="
           << std::endl;

        struct Base
        {
            virtual int f() const
            {
                return 0;
            }
        };
        struct OverrideOK
            : Base
        {
            int f() const BSLS_CXX11_OVERRIDE
            {
                return 1;
            }
        };
        struct OverrideFail
            : Base
        {
            int f()
#if __cplusplus < 201103 || defined(FAIL_OVERRIDE)
                BSLS_CXX11_OVERRIDE
#endif
            {
                return 2;
            }
        };

        OverrideOK ok;
        ASSERT(ok.f() == 1);
        ASSERT(static_cast<const Base&>(ok).f() == 1);
        OverrideFail fail;
        ASSERT(fail.f() == 2);
        ASSERT(static_cast<const Base&>(fail).f() == 0);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING BSLS_CXX11_FINAL (function)
        //
        // Concerns:
        //   1) Marking a 'virtual' function as 'final' should compile.
        //
        //   2) Trying to override a function marked as 'final' shall fail to
        //      to compile when compiling with C++11 mode.  Since
        //      'BSLS_CXX11_FINAL' is replaced by nothing when compiling with
        //      C++03 mode the could should compile in this case.
        //
        // Plan:
        //   Define a base class with a 'virtual' function and mark it 'final'
        //   using 'BSLS_CXX11_FINAL' in a derived class.  Creating a further
        //   derived class which also overrides the function marked as 'final'
        //   should fail compilation when compiling with C++11 mode.
        // --------------------------------------------------------------------

        if (verbose) std::cout << std::endl
           << "TESTING BSLS_CXX11_FINAL for functions"
           << std::endl
           << "==============================================================="
           << std::endl;
        
        struct FinalFunctionBase
        {
            virtual int f() { return 0; }
        };
        struct FinalFunctionDerived: FinalFunctionBase
        {
            int f()
#if __cplusplus < 201103 || defined(FAIL_FINAL_FUNCTION)
                BSLS_CXX11_FINAL
#endif
            { return 1; }
        };
        struct FinalFunctionFailure: FinalFunctionDerived
        {
            int f() { return 2; }
        };

        FinalFunctionBase finalFunctionBase;
        ASSERT(finalFunctionBase.f() == 0);
        FinalFunctionDerived finalFunctionDerived;
        ASSERT(finalFunctionDerived.f() == 1);
        FinalFunctionFailure finalFunctionFailure;
        ASSERT(finalFunctionFailure.f() == 2);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING BSLS_CXX11_FINAL (class)
        //
        // Concerns:
        //   1) Marking a class 'final' using 'BSLS_CXX11_FINAL' should result
        //      in a successful compilation.
        //
        //   2) Trying to further derive from a function marked as 'final'
        //      shall fail to compile when compiling with C++11 mode.  Since
        //      'BSLS_CXX11_FINAL' is replaced by nothing when compiling with
        //      C++03 mode the could should compile in this case.
        //
        // Plan:
        //   Define a class marking it 'final' using 'BSLS_CXX11_FINAL'.
        //   Creating a derived class from the 'final' class should fail
        //   compilation when compiling with C++11 mode.
        // --------------------------------------------------------------------

        if (verbose) std::cout << std::endl
           << "TESTING BSLS_CXX11_FINAL for classes"
           << std::endl
           << "==============================================================="
           << std::endl;
        
        class FinalClass BSLS_CXX11_FINAL
        {
            int d_value;
        public:
            FinalClass(int value = 0): d_value(value) {}
            int value() const { return d_value; }
        };
        class FinalClassDerived
#if __cplusplus < 201103 || defined(FAIL_FINAL_CLASS)
            : public FinalClass
#endif
        {
            int d_anotherValue;
        public:
            FinalClassDerived(int value)
                : d_anotherValue(2 * value) {
            }
            int anotherValue() const { return d_anotherValue; }
        };

        FinalClass finalValue(1);
        ASSERT(finalValue.value() == 1);
        FinalClassDerived derivedValue(2);
        ASSERT(derivedValue.anotherValue() == 4);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING BSLS_CXX11_EXPLICIT
        //
        // Concerns:
        //   1) Marking a conversion operator 'explicit' using
        //      'BSLS_CXX_EXPLICIT' needs to allow explicit conversions.
        //
        //   2) Marking a conersion operator 'explicit' using
        //      'BSLS_CXX_EXPLICIT' should prevent attempts of implicit
        //      conversion when compiling with C++11 mode. When compiling with
        //      C++03 mode compilation will succeed.
        //
        // Plan:
        //   Define a class with an explicit conversion operator and 
        //   verify that explicit and implicit conversions succeed when using
        //   C++03 mode. When compiling with C++11 mode the implicit conversion
        //   should fail.
        // --------------------------------------------------------------------

        if (verbose) std::cout << std::endl
           << "TESTING BSLS_CXX11_EXPLICIT"
           << std::endl
           << "==============================================================="
           << std::endl;
        
        struct Explicit
        {
            BSLS_CXX11_EXPLICIT operator int() const { return 3; }
        };

        Explicit explicitObject;

        int explicitResult(explicitObject);
        ASSERT(explicitResult == 3);
#if __cplusplus < 201103 || defined(FAIL_EXPLICIT)
        int implicitResult = explicitObject;
        ASSERT(implicitResult == 3);
#endif
      } break;
      default: {
        std::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << std::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        std::cerr << "Error, non-zero test status = " << testStatus << "."
                  << std::endl;
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
