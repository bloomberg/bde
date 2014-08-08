// bdldfp_decimalimputil_inteldfp.t.cpp                               -*-C++-*-
#include <bdldfp_decimalimputil_inteldfp.h>

#ifndef BDLDFP_DECIMALPLATFORM_INTELDFP
int main()
{
    return -1;
}

#else

#include <bsls_assert.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>


#include <cassert>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::flush;
using bsl::endl;
using bsl::hex;
using bsl::atoi;
using bsl::stringstream;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The 'bldfp_decimalimputil_inteldfp' component implements all routines as
// forwarding functions to corresponding code in the Intel DFP library.  All
// routines are inline, and get used by 'bdldfp_decimalimputil' itself.  The
// tests can be found there.
//
// ----------------------------------------------------------------------------
// CLASS METHODS
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] USAGE EXAMPLE
// ----------------------------------------------------------------------------

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                    STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define C_(X)   << #X << ": " << X << '\t'
#define A_(X,S) { if (!(X)) { cout S << endl; aSsErT(1, #X, __LINE__); } }
#define LOOP_ASSERT(I,X)            A_(X,C_(I))
#define LOOP2_ASSERT(I,J,X)         A_(X,C_(I)C_(J))
#define LOOP3_ASSERT(I,J,K,X)       A_(X,C_(I)C_(J)C_(K))
#define LOOP4_ASSERT(I,J,K,L,X)     A_(X,C_(I)C_(J)C_(K)C_(L))
#define LOOP5_ASSERT(I,J,K,L,M,X)   A_(X,C_(I)C_(J)C_(K)C_(L)C_(M))
#define LOOP6_ASSERT(I,J,K,L,M,N,X) A_(X,C_(I)C_(J)C_(K)C_(L)C_(M)C_(N))

#define Q   BDLS_TESTUTIL_Q   // Quote identifier literally.
#define P   BDLS_TESTUTIL_P   // Print identifier and value.
#define P_  BDLS_TESTUTIL_P_  // P(X) without '\n'.
#define T_  BDLS_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BDLS_TESTUTIL_L_  // current Line number


// Usage example code:
///Usage
///-----
// This section shows the intended use of this component.
//
///Example 1: Adding two intel-formatted Decimal Values
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// Intel formatted Decimal Floating Point values are highly efficient for
// arithmetic.  Often times it is necessary to sum up stock prices or other
// securities.
//
// Suppose that we had a list of stock prices to total.  This list is a
// sequence of 'DecimalImpUtil_IntelDFP::ValueType64' values.
//
// First, we write a function which totals security prices, and returns a
// Decimal Floating Point number in Intel Format:
//
    bdldfp::DecimalImpUtil_IntelDFP::ValueType64
    totalSecurities(bdldfp::DecimalImpUtil_IntelDFP::ValueType64 *prices,
                    int                                           numPrices)
        // Return an Intel formatted Decimal Floating Point number representing
        // the arithmetic total of the values specified by 'prices' and
        // 'numPrices'.
    {
//
// Then, we create a local variable to hold the total value, and set it to
// zero:
//
        bdldfp::DecimalImpUtil_IntelDFP::ValueType64 total;
        total = bdldfp::DecimalImpUtil_IntelDFP::int32ToDecimal64(0);
//
// Next, we loop over the values in 'prices':
//
        for (int i = 0; i < numPrices; ++i) {
//
// Then, we add the price at each index in the array to the total:
//
            total = bdldfp::DecimalImpUtil_IntelDFP::add(total, prices[i]);
        }
//
// Now, we return the computed total value of the securities:
//
        return total;
    }
//
// Notice that the computation is performed using a functional notation.  This
// is because the 'bdldfp::DecimalImpUtil_IntelDFP' utility is intended to be
// used in the implementation of operator overloads on a more full fledged
// type.
//

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 Usage example compiles.
        //: 2 Usage example produces a reasonable result.
        //
        // Plan:
        //: 1 Copy Usage example from the header.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;
// Finally, we call the function with some sample data, and check the result:
//
    bdldfp::DecimalImpUtil_IntelDFP::ValueType64 data[16];
//
    for (int i = 0; i < 16; ++i) {
        data[i] = bdldfp::DecimalImpUtil_IntelDFP::int32ToDecimal64(i + 1);
    }
//
    bdldfp::DecimalImpUtil_IntelDFP::ValueType64 result;
    result = totalSecurities(data, 16);
//
    bdldfp::DecimalImpUtil_IntelDFP::ValueType64 expected;
//
    expected = bdldfp::DecimalImpUtil_IntelDFP::int32ToDecimal64(16);
//
    // Totals of values from 1 to 'x' are '(x * x + x) / 2':
//
    expected = bdldfp::DecimalImpUtil_IntelDFP::add(
                 bdldfp::DecimalImpUtil_IntelDFP::multiply(expected, expected),
                 expected);
    expected = bdldfp::DecimalImpUtil_IntelDFP::divide(
                         expected,
                         bdldfp::DecimalImpUtil_IntelDFP::int32ToDecimal64(2));
//
    ASSERT(bdldfp::DecimalImpUtil_IntelDFP::equal(expected, result));
//
// Notice that arithmetic is unwieldy and hard to visualize.  This is by
// design, as the DecimalImpUtil and subordinate components are not intended
// for public consumption, for decimal arithmetic.
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 TBD
        //
        // Plan:
        //: 1 TBD
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;
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

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2014 Bloomberg L.P.
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
