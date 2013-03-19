// bslmf_isconvertibletoany.t.cpp                                     -*-C++-*-
#include <bslmf_isconvertibletoany.h>

#include <bsls_bsltestutil.h>

#include <cstdio>   // 'printf'
#include <cstdlib>  // 'atoi'

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines a meta-function,
// 'bslmf::IsConvertibleToAny', that determines whether a template parameter
// type can be converted to any type.  Thus, we need to ensure that the value
// returned by this meta-function is correct for the type with the appropriate
// conversion operator.
//
//-----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bslmf::IsEnum::value
//
// ----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

struct ConvertibleToSome {
    operator bool() { return false; }
    operator int() { return 1; }
    operator bsl::true_type() { return bsl::true_type(); }
};

struct ConvertibleToAny {
    template <typename T>
    operator T() { return T(); }
};

struct ConvertibleToAnyConst {
    template <typename T>
    operator T() const { return T(); }
};

}

//=============================================================================
//                              USAGE EXAMPLES
//-----------------------------------------------------------------------------

///Usage
//------
// In this section we show the intended use of this component.
//
///Example 1: Determine If a Class Has a Template Conversion Operator
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have a class 'ValueInitializer' defined as follows:
//..
class ValueInitializer {
    // This class provides a way to portably and predictably
    // value-initialize objects of any type.

  public:
    template <typename TYPE>
    operator TYPE() {
        return TYPE();
    }
};
//..
// Possible purpose of such class could be to provide a convenient way to
// value-initialize an object of any type.  For example:
//..
struct MyStruct {
    int d_val;
};

// initialize an object of a generic type
template <typename T>
void foo() {
    T val = ValueInitializer();
    ASSERT(val == T());
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
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

// initialize an object of a fundamental type
int val1 = ValueInitializer();
ASSERT(val1 == 0);

// initialize an object of an aggregate type
MyStruct val2 = ValueInitializer();
ASSERT(val2.d_val == 0);

// initialize an object of a generic type
foo<int>();
foo<double>();
//..
// To make the above code work 'ValueInitializer' needs to have a conversion
// operator that allows to convert it to any type.  The existance of such
// conversion operator can be checked with the 'bslmf::IsConvertibleToAny'
// meta-function.
//..
ASSERT(bslmf::IsConvertibleToAny<ValueInitializer>::value);
//..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bslmf::IsConvertibleToAny::value'
        //   Ensure that the static data member 'value' of
        //   'bslmf::IsConvertibleToAny' instantiation with the (template
        //   parameter) 'TYPE' having a template conversion operator has a
        //   correct value.
        //
        // Concerns:
        //: 1 'bslmf::IsConvertibleToAny::value' is 'false' when 'TYPE' is a
        //:   primitive type.
        //:
        //: 2 'bslmf::IsConvertibleToAny::value' is 'false' when 'TYPE' doesn't
        //:   have a conversion operator that allows it to be converted to any
        //:   type, but might have any other conversion operators.
        //:
        //: 3 'bslmf::IsConvertibleToAny::value' is 'true' when 'TYPE'
        //:   has a conversion operator that allows it to be converted to any
        //:   type.
        //
        // Plan:
        //   Verify that 'bslmf::IsConvertibleToAny::value' has the correct
        //   value for each (template parameter) 'TYPE' in the concerns.
        //   (C-1..3)
        //
        // Testing:
        //   bslmf::IsConvertibleToAny::value
        // --------------------------------------------------------------------

        if (verbose) printf("bslmf::IsConvertibleToAny::value\n"
                            "================================\n");

        ASSERT(!bslmf::IsConvertibleToAny<int>::value);
        ASSERT(!bslmf::IsConvertibleToAny<ConvertibleToSome>::value);
        ASSERT(bslmf::IsConvertibleToAny<ConvertibleToAny>::value);
        ASSERT(bslmf::IsConvertibleToAny<ConvertibleToAnyConst>::value);

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
