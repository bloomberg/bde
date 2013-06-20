// bsls_nullptr.t.cpp                                                 -*-C++-*-

#include <bsls_nullptr.h>

#include <bsls_bsltestutil.h>  // for testing only

#include <stdio.h>      // sprintf()
#include <stdlib.h>     // atoi()

using namespace BloombergLP;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
//
//-----------------------------------------------------------------------------
// [ 2] class bsls::NullPtr_Impl
// [ 3] typedef bsl::nullptr_t
//-----------------------------------------------------------------------------
// [ 1] Breathing test
// [ 4] Usage example
//=============================================================================
//                       STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

static void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=========================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-------------------------------------------------------------------------
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

//=============================================================================
//                    GLOBAL CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

bool globalVerbose         = false;
bool globalVeryVerbose     = false;
bool globalVeryVeryVerbose = false;

//=============================================================================
//                  GLOBAL HELPER MACROS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                   SUPPORTING FUNCTIONS USED FOR TESTING
//-----------------------------------------------------------------------------

// Example 1: Constructing a "smart pointer"
// - - - - - - - - - - - - - - - - - - - - -

// First we define a smart pointer class template, as a guard to destroy a
// managed object as the smart pointer leaves scope.  This class will have a
// constructor template taking a pointer to a type potentially derived from
// the parameterized type of the smart pointer, and also a deletion-policy
// function.  By capturing the most-derived type through type-deduction when
// the smart pointer is constructed, we can ensure the correct destructor is
// called, even if the destructor of the base class has not been declared as
// 'virtual'.  However, relying on type-deduction means we cannot pass a null
// pointer to this constructor, as it is not possible to deduce what type a
// null pointer is supposed to refer to, therefore we must use a special null
// pointer type, such as 'bsls::nullptr_t'.  Note that in real code we would
// allocate and reclaim memory using a user-specified allocator, but defining
// such protocols in this low level component would further distract from the
// 'nullptr' usage in this example.
//..
    template<class TARGET_TYPE>
    class ScopedPointer {
        // This class template is a guard to manage a dynamically created
        // object of the parameterized 'TARGET_TYPE'.

      private:
        typedef void DeleterFn(TARGET_TYPE *);  // deleter type

        // DATA
        TARGET_TYPE *d_target_p;    // wrapped pointer
        DeleterFn   *d_deleter_fn;  // deleter function

        template<class SOURCE_TYPE>
        static void defaultDeleteFn(TARGET_TYPE *ptr);
            // Destroy the specified '*ptr' by calling 'delete' on the pointer
            // cast to the parameterized 'SOURCE_TYPE*'.  It is an error to
            // instantiate this template with a 'SOURCE_TYPE' that is not
            // derived from (and cv-compatible with) 'TARGET_TYPE'.

      private:
        // NOT IMPLEMENTED
        ScopedPointer(const ScopedPointer&);
        ScopedPointer& operator=(const ScopedPointer&);
            // Objects of this type cannot be copied.

      public:
        // CREATORS
        template<class SOURCE_TYPE>
        ScopedPointer(SOURCE_TYPE *pointer,
                      DeleterFn   *fn = &defaultDeleteFn<SOURCE_TYPE>);
            // Create a 'ScopedPointer' object owning the specified 'pointer'
            // and using the specified 'fn' to destroy the owned pointer when
            // this object is destroyed.

        ScopedPointer(bsl::nullptr_t = 0);
            // Create an empty 'ScopedPointer' object that does not own a
            // pointer.

        ~ScopedPointer();
            // Destroy this 'ScopedPointer' object and the target object
            // that it owns, using the stored deleter function.

        // Further methods appropriate to a smart pointer, such as
        // 'operator*' and 'operator->' elided from this example.
    };
//..
// Then we provide a definition for each of the methods.
//..
    template<class TARGET_TYPE>
    template<class SOURCE_TYPE>
    void ScopedPointer<TARGET_TYPE>::defaultDeleteFn(TARGET_TYPE *ptr)
    {
        delete static_cast<SOURCE_TYPE *>(ptr);
    }

    template<class TARGET_TYPE>
    template<class SOURCE_TYPE>
    inline
    ScopedPointer<TARGET_TYPE>::ScopedPointer(SOURCE_TYPE *pointer,
                                              DeleterFn   *fn)
    : d_target_p(pointer)
    , d_deleter_fn(fn)
    {
    }

    template<class TARGET_TYPE>
    inline
    ScopedPointer<TARGET_TYPE>::ScopedPointer(bsl::nullptr_t)
    : d_target_p(0)
    , d_deleter_fn(0)
    {
    }

    template<class TARGET_TYPE>
    inline
    ScopedPointer<TARGET_TYPE>::~ScopedPointer()
    {
        if (d_deleter_fn) {
            d_deleter_fn(d_target_p);
        }
    }
//..
// Finally, we can construct a 'ScopedPointer' with a null pointer literal,
// that would otherwise be non-deducible, using our 'bsl::nullptr_t' overload.
//..
    void testScopedPointer()
    {
        ScopedPointer<int> x(0);
    }
//..

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    (void)veryVerbose;

    setbuf(stdout, 0);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //  The usage example involves defining templates that cannot be
        //  declared as local classes, so the test case is defined out-of-line
        //  above.
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

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        testScopedPointer();
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING STANDARD TYPEDEF
        //   The primary interface of this component is a type alias that will
        //   match a null pointer literals in overload resolution.  This should
        //   be the declared type of 'nullptr' on conforming C++11 compilers,
        //   and an emulated type otherwise.
        //
        // Concerns:
        //: 1 There exists a type alias 'bsl::nullptr_t', aliasing a type that
        //:   represents null pointer literals.
        //: 2 Functions having parameters of type 'bsl::nullptr_t' can be
        //:   invoked only with null pointer literals, '0' and 'NULL'.
        //: 3 Functions having parameters of type 'bsl::nullptr_t' cannot be
        //:   invoked pointers or pointer-to-members, even if they hold the
        //:   null pointer value.
        //
        // Plan:
        //: 1 Define a local class with two overloads of a static method.
        //:   1 The first overload accepts an argument of type 'bsl::nullptr_t'
        //:     and returns 'true' to indicate it was selected by overload
        //:     resolution. (C-1)
        //:   2 The second overload uses an ellipsis parameter list to weakly
        //:     match an argument of fundamental type, and returns 'false'.
        //: 2 Call the static method all valid null pointer literals, and check
        //:   that the result is 'true' in each case. (C-2)
        //: 3 Call the static method with various objects, including pointers
        //:   and pointer-to-members, and check that the result is 'false' in
        //:   each case. (C-3)
        //
        // Testing:
        //   bsl::nullptr_t
        // --------------------------------------------------------------------

        struct local {
            // This local utility 'struct' provides a namespace for testing
            // overload resolution for the type under test, 'bsl::nullptr_t'.

            static bool isNullPointer(bsl::nullptr_t) { return true;  }
            static bool isNullPointer(...)            { return false; }
                // Return 'true' is the argument is a null pointer literal, and
                // 'false' otherwise.
        };

        // null pointer literals
        static const int s_cZero = 0;
        const int cZero = 0;

        // not null pointer literals
        static void *const Cptr = 0;
        void *ptr = 0;
        int local::*mem = 0;
        static const int& s_zeroRef = 0;
        int zero = 0;

        enum { MY_NULL = 0 };

        ASSERT(local::isNullPointer(0));
        ASSERT(local::isNullPointer(NULL));
        ASSERT(local::isNullPointer(false));
        ASSERT(local::isNullPointer(s_cZero));
        ASSERT(local::isNullPointer(cZero));
        ASSERT(local::isNullPointer(1-1));
        ASSERT(local::isNullPointer(0*1));

        ASSERT(!local::isNullPointer(Cptr));
        ASSERT(!local::isNullPointer(ptr));
        ASSERT(!local::isNullPointer(mem));
        ASSERT(!local::isNullPointer((void*)0));
        ASSERT(!local::isNullPointer(zero));
        ASSERT(!local::isNullPointer(s_zeroRef));
        ASSERT(!local::isNullPointer(1));
        ASSERT(!local::isNullPointer(s_zeroRef*1));
        ASSERT(!local::isNullPointer(MY_NULL));

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING COMPONENT TYPEDEF
        //   The component supplies an implementation-private class with a
        //   nested 'typedef' that should emulate matching a null pointer
        //   literal, unless using a compiler that supports the necessary
        //   language features introduced by C++11, in which case the
        //   implementation class should not be present.
        //
        // Concerns:
        //: 1 The metafunction struct 'bsls::Nullptr_Impl' contains a nested
        //:   alias named 'Type'.
        //: 2 Functions having parameters of type 'bsls::Nullptr_Impl::Type'
        //:   can be invoked only with null pointer literals, such as '0' and
        //:   'NULL'.
        //: 3 Functions having parameters of type 'bsls::Nullptr_Impl::Type'
        //:   cannot be invoked by pointers or pointer-to-members, even if they
        //:   hold the null pointer value.
        //
        // Plan:
        //: 1 Define a local class with two overloads of a static method.
        //:   1 The first overload accepts an argument of type
        //:     'bsls::Nullptr_Impl::Type' and returns 'true' to indicate it
        //:     was selected by overload resolution. (C-1)
        //:   2 The second overload uses an ellipsis parameter list to weakly
        //:     match an argument of fundamental type, and returns 'false'.
        //: 2 Call the static method all valid null pointer literals, and check
        //:   that the result is 'true' in each case. (C-2)
        //: 3 Call the static method with various objects, including pointers
        //:   and pointer-to-members, and check that the result is 'false' in
        //:   each case. (C-3)
        //
        // Testing:
        //   bsls::Nullptr_Impl::Type
        // --------------------------------------------------------------------

#if !defined(BSLS_NULLPTR_USING_NATIVE_NULLPTR_T)
        struct local {
            // This local utility 'struct' provides a namespace for testing
            // overload resolution for the type under test,
            // 'bsls::Nullptr_Impl::Type'.

            static bool isNullPointer(bsls::Nullptr_Impl::Type){ return true; }
            static bool isNullPointer(...)                    { return false; }
                // Return 'true' if the argument is a null pointer literal, and
                // 'false' otherwise.
        };

        // null pointer literals
        static const int s_cZero = 0;
        const int cZero = 0;

        // not null pointer literals
        static void *const Cptr = 0;
        void *ptr = 0;
        int local::*mem = 0;
        static const int& s_zeroRefGcc = 0;
        int zero = 0;

        enum { MY_NULL = 0 };

        ASSERT(local::isNullPointer(0));
        ASSERT(local::isNullPointer(NULL));
        ASSERT(local::isNullPointer(false));
        ASSERT(local::isNullPointer(s_cZero));
        ASSERT(local::isNullPointer(cZero));
        ASSERT(local::isNullPointer(1-1));
        ASSERT(local::isNullPointer(0*1));

        ASSERT(!local::isNullPointer(Cptr));
        ASSERT(!local::isNullPointer(ptr));
        ASSERT(!local::isNullPointer(mem));
        ASSERT(!local::isNullPointer((void*)0));
        ASSERT(!local::isNullPointer(zero));
        ASSERT(!local::isNullPointer(s_zeroRefGcc));
        ASSERT(!local::isNullPointer(1));
        ASSERT(!local::isNullPointer(s_zeroRefGcc*1));
        ASSERT(!local::isNullPointer(MY_NULL));

#endif
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The component is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Provide a function taking an argument of type 'bsl::nullptr_t',
        //:   and then call this function with the literal values '0' and
        //:   'NULL'. (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        struct local {
            static void test(bsl::nullptr_t) { }
        };

        local::test(0);
        local::test(NULL);

      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
