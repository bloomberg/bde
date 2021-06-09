// bslmf_invokeresult.06.t.cpp                                        -*-C++-*-



// ============================================================================
//                 INCLUDE STANDARD TEST MACHINERY FROM CASE 0
// ----------------------------------------------------------------------------

#define BSLMF_INVOKERESULT_00T_AS_INCLUDE
#include <bslmf_invokeresult.00.t.cpp>


//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// See the test plan in 'bslmf_invokeresult.00.t.cpp'.
//
// This file contains the following test:
// [6] 'InvokeResultDeductionFailed'
//-----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

// These are defined as part of the standard machinery included from file
// bslmf_invokeresult.00.t.cpp
//
// We need to suppress the bde_verify error due to them not being in this file:
// BDE_VERIFY pragma: -TP19


// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

// These are defined as part of the standard machinery included from file
// bslmf_invokeresult.00.t.cpp
//
// We need to suppress the bde_verify error due to them not being in this file:
// BDE_VERIFY pragma: -TP19



using namespace BloombergLP;

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------


template <class TP>
TP returnObj()
    // Return a value-initialized object of type 'TP'.
{
    return TP();
}

template <class TP>
TP& returnNoCVRef()
    // Return a reference to a value-initialized object of type 'TP'. 'TP' is
    // assumed not to be cv-qualified.
{
    static TP obj;
    return obj;
}

template <class TP>
TP& returnLvalueRef()
    // Return an lvalue reference to a value-initialized object of type
    // 'TP'.  'TP' may be cv-qualified.
{
    return returnNoCVRef<typename bsl::remove_cv<TP>::type>();
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
template <class TP>
TP&& returnRvalueRef()
    // Return an rvalue reference to a value-initialized object of type 'TP'.
{
    return std::move(returnLvalueRef<TP>());
}
#endif

template <class TP>
bslmf::InvokeResultDeductionFailed discardObj()
    // Return an 'bslmf::InvokeResultDeductionFailed' object initialized from
    // an rvalue of type 'TP'.
{
    return returnObj<TP>();
}

template <class TP>
bslmf::InvokeResultDeductionFailed discardLvalueRef()
    // Return an 'bslmf::InvokeResultDeductionFailed' object initialized from
    // an lvalue reference of type 'TP&'.  'TP' may be cv-qualified.
{
    return returnLvalueRef<TP>();
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
template <class TP>
bslmf::InvokeResultDeductionFailed discardRvalueRef()
    // Return an 'bslmf::InvokeResultDeductionFailed' object initialized from
    // an rvalue reference of type 'TP&&'.  'TP' may be cv-qualified.
{
    return returnRvalueRef<TP>();
}
#endif


// ============================================================================
//                                MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int test = argc > 1 ? atoi(argv[1]) : 0;

    BSLA_MAYBE_UNUSED const bool verbose             = argc > 2;
    BSLA_MAYBE_UNUSED const bool veryVerbose         = argc > 3;
    BSLA_MAYBE_UNUSED const bool veryVeryVerbose     = argc > 4;
    BSLA_MAYBE_UNUSED const bool veryVeryVeryVerbose = argc > 5;

    using BloombergLP::bslmf::InvokeResultDeductionFailed;

    printf("TEST " __FILE__ " CASE %d\n", test);

    // As test cases are defined elsewhere, we need to suppress bde_verify
    // warnings about missing test case comments/banners.
    // BDE_VERIFY pragma: push
    // BDE_VERIFY pragma: -TP05
    // BDE_VERIFY pragma: -TP17

    switch (test) { case 0:  // Zero is always the leading case.
      case 8: BSLA_FALLTHROUGH;
      case 7: {
        referUserToElsewhere(test, verbose);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'InvokeResultDeductionFailed'
        //
        // Concerns:
        //: 1 'bslmf::InvokeResultDeductionFailed' can be constructed from any
        //:   object type.
        //: 2 'bslmf::InvokeResultDeductionFailed' can be constructed from any
        //:   lvalue reference.
        //: 3 The previous concerns apply if the initializer is cv qualified.
        //: 4 In C++11 and later compilations,
        //:   'bslmf::InvokeResultDeductionFailed' can be constructed from any
        //:   rvalue reference.
        //: 5 The above concerns apply to values that are the result of a
        //:   function return.
        //: 6 The 'return' statement of a function returning a
        //:   'bslmf::InvokeResultDeductionFailed' can specify an lvalue or
        //:   rvalue of any type, resulting in the value being ignored.
        //
        // Plan:
        //: 1 For concern 1, construct 'bslmf::InvokeResultDeductionFailed'
        //:   objects from value-initialized objects of numeric type,
        //:   pointer type, class type, and enumeration type. There is nothing
        //:   to verify -- simply compiling successfully is enough.
        //: 2 For concern 2, create variables of the same types as in the
        //:   previous step. Construct a 'bslmf::InvokeResultDeductionFailed'
        //:   object from each (lvalue) variable.
        //: 3 For concern 3, repeat step 2, using a 'const_cast' to add cv
        //:   qualifiers to the lvalues.
        //: 4 For concern 4, repeat step 2, applying 'std::move' to each
        //:   variable used to construct an object (C++11 and later only).
        //: 5 For concern 5, implement a function 'returnObj<TP>' that returns
        //:   an object of type 'TP', a function 'returnLvalueRef<TP>', that
        //:   returns a reference of type 'TP&' and (for C++11 and later)
        //:   'returnRvalueRef<TP>' that returns a reference of type
        //:   'TP&&'. Construct a 'bslmf::InvokeResultDeductionFailed' object
        //:   from a call to each function template instantiated with each of
        //:   the types from step 1 and various cv-qualifier combinations.
        //: 6 For concern 6, implement function templates returning
        //:   'bslmf::InvokeResultDeductionFailed' objects 'discardObj<TP>',
        //:   'discardLvalueRef<TP>', and 'discardRvalueRef<TP>'. These
        //:   functions respectively contain the statements 'return
        //:   returnObj<TP>', 'return returnLvalueRef<TP>', and 'return
        //:   returnRvalueRef<TP>'. Invoke each function with the same types
        //:   as in step 5.
        //
        // Testing
        //     'InvokeResultDeductionFailed'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'InvokeResultDeductionFailed'"
                            "\n=====================================\n");
        if (verbose)
            printf(
                "This test only exists in file bslm_invokeresult.%02d.t.cpp\n"
                "(versions in other files are no-ops)\n",
                test);

        int            v1 = 0;
        bsl::nullptr_t v2;
        const char*    v3 = "hello";
        MyEnum         v4 = MY_ENUMERATOR;
        MyClass        v5;

        ASSERT(true); // Suppress "not used" warning

        // Step 1: Conversion from rvalue
        {
            bslmf::InvokeResultDeductionFailed x1(0);
            (void) x1; // Suppress "set but not used" warning

            bslmf::InvokeResultDeductionFailed x2 = bsl::nullptr_t();
            bslmf::InvokeResultDeductionFailed x3("hello");
            bslmf::InvokeResultDeductionFailed x4(MY_ENUMERATOR); // MyEnum
            bslmf::InvokeResultDeductionFailed x5 = MyClass();

            (void) x2; // Suppress "set but not used" warning
            (void) x3; // Suppress "set but not used" warning
            (void) x4; // Suppress "set but not used" warning
            (void) x5; // Suppress "set but not used" warning
        }

        // Step 2: Conversion from lvalue
        {
            bslmf::InvokeResultDeductionFailed x1 = v1;
            (void) x1; // Suppress "set but not used" warning
            bslmf::InvokeResultDeductionFailed x2 = v2;
            bslmf::InvokeResultDeductionFailed x3 = v3;
            bslmf::InvokeResultDeductionFailed x4 = v4;
            bslmf::InvokeResultDeductionFailed x5 = v5;

            (void) x2; // Suppress "set but not used" warning
            (void) x3; // Suppress "set but not used" warning
            (void) x4; // Suppress "set but not used" warning
            (void) x5; // Suppress "set but not used" warning
        }

        // Step 3: Conversion from cv-qualified lvalue
        {
            bslmf::InvokeResultDeductionFailed x1 = const_cast<const int&>(v1);
            bslmf::InvokeResultDeductionFailed x3 =
                const_cast<const char *volatile& >(v3);
            bslmf::InvokeResultDeductionFailed x4 =
                const_cast<const volatile MyEnum&>(v4);
            bslmf::InvokeResultDeductionFailed x5 =
                const_cast<const MyClass&>(v5);

            (void) x1; // Suppress "set but not used" warning
            (void) x3; // Suppress "set but not used" warning
            (void) x4; // Suppress "set but not used" warning
            (void) x5; // Suppress "set but not used" warning
        }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        // Step 4: Conversion from rvalue reference
        {
            bslmf::InvokeResultDeductionFailed x1 = std::move(v1);
            bslmf::InvokeResultDeductionFailed x2 = std::move(v2);
            bslmf::InvokeResultDeductionFailed x3 = std::move(v3);
            bslmf::InvokeResultDeductionFailed x4 = std::move(v4);
            bslmf::InvokeResultDeductionFailed x5 = std::move(v5);

            (void) x1; // Suppress "set but not used" warning
            (void) x2; // Suppress "set but not used" warning
            (void) x3; // Suppress "set but not used" warning
            (void) x4; // Suppress "set but not used" warning
            (void) x5; // Suppress "set but not used" warning
        }
#endif

        // Step 5: Initialization from function return
        {
            bslmf::InvokeResultDeductionFailed x1 = returnObj<int>();
            (void) x1; // Suppress "set but not used" warning
            bslmf::InvokeResultDeductionFailed x2 =
                returnObj<bsl::nullptr_t>();
            bslmf::InvokeResultDeductionFailed x3 = returnObj<const char *>();
            bslmf::InvokeResultDeductionFailed x4 = returnObj<MyEnum>();
            bslmf::InvokeResultDeductionFailed x5 = returnObj<MyClass>();

            (void) x2; // Suppress "set but not used" warning
            (void) x3; // Suppress "set but not used" warning
            (void) x4; // Suppress "set but not used" warning
            (void) x5; // Suppress "set but not used" warning
        }

        {
            bslmf::InvokeResultDeductionFailed x1 = returnLvalueRef<int>();
            bslmf::InvokeResultDeductionFailed x2 =
                returnLvalueRef<bsl::nullptr_t>();
            bslmf::InvokeResultDeductionFailed x3 =
                returnLvalueRef<const char *const>();
            bslmf::InvokeResultDeductionFailed x4 =
                returnLvalueRef<volatile MyEnum>();
            bslmf::InvokeResultDeductionFailed x5 =
                returnLvalueRef<const volatile MyClass>();

            (void) x1; // Suppress "set but not used" warning
            (void) x2; // Suppress "set but not used" warning
            (void) x3; // Suppress "set but not used" warning
            (void) x4; // Suppress "set but not used" warning
            (void) x5; // Suppress "set but not used" warning
        }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        {
            bslmf::InvokeResultDeductionFailed x1 = returnRvalueRef<int>();
            bslmf::InvokeResultDeductionFailed x2 =
                returnRvalueRef<bsl::nullptr_t>();
            bslmf::InvokeResultDeductionFailed x3 =
                returnRvalueRef<const char *>();
            bslmf::InvokeResultDeductionFailed x4 = returnRvalueRef<MyEnum>();
            bslmf::InvokeResultDeductionFailed x5 = returnRvalueRef<MyClass>();

            (void) x1; // Suppress "set but not used" warning
            (void) x2; // Suppress "set but not used" warning
            (void) x3; // Suppress "set but not used" warning
            (void) x4; // Suppress "set but not used" warning
            (void) x5; // Suppress "set but not used" warning
        }
#endif
        // Step 6: Return 'bslmf::InvokeResultDeductionFailed'
        {
            discardObj<int>();
            discardObj<bsl::nullptr_t>();
            discardObj<const char *>();
            discardObj<MyEnum>();
            discardObj<MyClass>();

            discardLvalueRef<int>();
            discardLvalueRef<bsl::nullptr_t>();
            discardLvalueRef<const char *const>();
            discardLvalueRef<volatile MyEnum>();
            discardLvalueRef<const volatile MyClass>();

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            discardRvalueRef<int>();
            discardRvalueRef<bsl::nullptr_t>();
            discardRvalueRef<const char *>();
            discardRvalueRef<MyEnum>();
            discardRvalueRef<MyClass>();
#endif
        }

      } break;
      case 5: BSLA_FALLTHROUGH;
      case 4: BSLA_FALLTHROUGH;
      case 3: BSLA_FALLTHROUGH;
      case 2: BSLA_FALLTHROUGH;
      case 1: {
        referUserToElsewhere(test, verbose);
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // BDE_VERIFY pragma: pop

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
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
