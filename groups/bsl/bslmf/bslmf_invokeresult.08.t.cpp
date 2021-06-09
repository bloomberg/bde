// bslmf_invokeresult.08.t.cpp                                        -*-C++-*-



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
// [8] USAGE EXAMPLE
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
//                             USAGE EXAMPLES
//-----------------------------------------------------------------------------

// Suppose we want to create a wrapper that executes an invocable object and
// sets a 'done' flag.  The 'done' flag will not be set if the invocation
// exits via an exception.  The wrapper takes an invocable 'f' and an argument
// 'x' and evaluates 'f(x)', returning the result.  In the absence of C++14
// automatically-deduced function return declarations, we use
// 'bsl::invoke_result' to deduce the return type of 'f(x)'.
//
// First, we write the wrapper template as follows:
//..
    template <class FT, class XT>
    typename bsl::invoke_result<FT, XT>::type
    invokeAndSetFlag(bool *done, FT f, XT x)
        // Use the specified parameters 'done', 'f', and 'x' to call 'f(x)' and
        // return the result, having set '*done' to true if no exception (or
        // false if exception).
    {
        typedef typename bsl::invoke_result<FT, XT>::type ResultType;
        *done = false; // Clear flag in case of exception
        ResultType result = f(x);
        *done = true;  // Set flag on success
        return result;
    }
//..
// Note that additional metaprogramming would be required to make this
// template work for return type 'void'; such metaprogramming is beyond the
// scope of this usage example.
//
// Then we define a couple of simple functors to be used with the wrapper.
// The first functor is a simple template that triples its invocation
// argument:
//..
    template <class TP>
    struct Triple {
        // Functor that triples its argument.

        TP operator()(TP v) const { return static_cast<TP>(v * 3); }
            // Return three times the specified 'v' value.
    };
//..
// Next, we define a second functor that returns an enumerator 'ODD' or
// 'EVEN', depending on whether its argument is exactly divisible by 2.  Since
// the return type is not a fundamental type, this functor indicates its
// return type using the 'ResultType' idiom:
//..
    enum EvenOdd { e_EVEN, e_ODD };

    struct CalcEvenOdd {
        // Functor that determines whether its argument is odd or even.

        typedef EvenOdd ResultType;

        EvenOdd operator()(int i) const { return (i & 1) ? e_ODD : e_EVEN; }
            // Return 'e_ODD' if the specified 'i' is odd; otherwise return
            // 'e_EVEN'
    };
//..
// Finally, we can invoke these functors through our wrapper:
//..
    int usageExample()
        // Run the usage example.
    {
        bool done = false;

        Triple<short> ts = {};
        short         r0 = invokeAndSetFlag(&done, ts, short(9));
        ASSERT(done && 27 == r0);

        CalcEvenOdd ceo = {};
        done            = false;
        EvenOdd r1      = invokeAndSetFlag(&done, ceo, 5);
        ASSERT(done && e_ODD == r1);

        done = false;
        EvenOdd r2 = invokeAndSetFlag(&done, ceo, 8);
        ASSERT(done && e_EVEN == r2);

        return 0;
    }


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
      case 8: {
        // --------------------------------------------------------------------
        // USAGE TEST
        //
        // Concerns:
        //: 1 The usage example in the component documentation compiles and
        //:   runs.
        //
        // Plan:
        //: 1  Copy the usage example verbatim but replace 'assert' with
        //:   'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE TEST"
                            "\n==========\n");
        if (verbose)
            printf(
                "This test only exists in file bslm_invokeresult.%02d.t.cpp\n"
                "(versions in other files are no-ops)\n",
                test);

        usageExample();

      } break;
      case 7: BSLA_FALLTHROUGH;
      case 6: BSLA_FALLTHROUGH;
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
