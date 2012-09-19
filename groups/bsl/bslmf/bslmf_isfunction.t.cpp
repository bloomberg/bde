// bslmf_isfunction.t.cpp                                             -*-C++-*-
#include <bslmf_isfunction.h>

#include <iostream>
#include <cstdlib>

using namespace std;
using namespace bsl;
using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The objects under test are two meta-functions, 'bsl::is_function' and
// 'bslmf::IsPointer', that determine whether a template parameter type is a
// pointer type.  Thus, we need to ensure that the value returned by these
// meta-functions are correct for each possible category of types.  Since the
// two meta-functions are functionally equivalent, we will use the same set of
// types for both.
//
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bsl::is_function::value
//
// ----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
        << J << "\n"; aSsErT(1, #X, __LINE__); } }
//=============================================================================
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_() cout << '\t' << flush;           // Print tab w/o linefeed.

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

struct TestType
   // This user-defined type is intended to be used during testing as an
   // argument for the template parameter 'TYPE' of 'bsl::is_function'.
{
};

typedef int (TestType::*MethodPtrTestType) ();
    // This non-static function member type is intended to be used during
    // testing as an argument for the template parameter 'TYPE' of
    // 'bsl::is_function'.

typedef void (*FunctionPtrTestType) ();
    // This function pointer type is intended to be used during testing as an
    // argument as an argument for the template parameter 'TYPE' of
    // 'bsl::is_function'.

}  // close unnamed namespace

#define TYPE_ASSERT_PREFIX(TYPE, result)                             \
    ASSERT(result == bsl::is_function<TYPE>::value);                 \
    ASSERT(result == bsl::is_function<const TYPE>::value);           \
    ASSERT(result == bsl::is_function<volatile TYPE>::value);        \
    ASSERT(result == bsl::is_function<const volatile TYPE>::value);

#define TYPE_ASSERT_SUFFIX(TYPE, result)                             \
    ASSERT(result == bsl::is_function<TYPE>::value);                 \
    ASSERT(result == bsl::is_function<TYPE const>::value);           \
    ASSERT(result == bsl::is_function<TYPE volatile>::value);        \
    ASSERT(result == bsl::is_function<TYPE const volatile>::value);

#define TYPE_ASSERT(TYPE, result)                     \
    TYPE_ASSERT_PREFIX(TYPE, result);                 \
    TYPE_ASSERT_PREFIX(TYPE const, result);           \
    TYPE_ASSERT_PREFIX(TYPE volatile, result);        \
    TYPE_ASSERT_PREFIX(TYPE const volatile, result);  \

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 2: {
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Function Types
/// - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a particular type is a pointer type.
//
// Now, we instantiate the 'bsl::is_function' template for a non-function type
// and a function type, and assert the 'value' static data member of each
// instantiation:
//..
    ASSERT(false == bsl::is_function<int>::value);
    ASSERT(true  == bsl::is_function<int (int)>::value);
//..
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::is_function::value'
        //   Ensure that the static data member 'value' of 'bsl::is_function'
        //   instantiations having various (template parameter) 'TYPES' has the
        //   correct value.
        //
        // Concerns:
        //: 1 'is_function::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) primitve type.
        //
        //: 2 'is_function::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) user-defined type.
        //:
        //: 3 'is_function::value' is 'false' when 'TYPE' is a pointer to a
        //:   non-static member (that may be const-qualified or
        //:   volatile-qualifie.
        //:
        //: 4 'is_function::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualifie) pointer to a (possibly cv-qualifie) type.
        //:
        //: 5 'is_function::value' is 'true' when 'TYPE' is a (possibly
        //:   cv-qualifie) function type.
        //
        // Plan:
        //   Verify that 'bsl::is_function::value' has the correct value for
        //   each (template parameter) 'TYPE' in the concerns.
        //
        // Testing:
        //   bsl::is_function::value
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "bsl::is_function::value" << endl
                          << "======================" << endl;

        // C-1
        TYPE_ASSERT_SUFFIX(int, false);

        // C-2
        TYPE_ASSERT_SUFFIX(TestType, false);

        // C-3
        TYPE_ASSERT_SUFFIX(MethodPtrTestType, false);

        // C-4
        TYPE_ASSERT(int*, false);
        TYPE_ASSERT(TestType*, false);
        TYPE_ASSERT(FunctionPtrTestType*, false);

        // C-5
        TYPE_ASSERT_PREFIX(int  (int),  true);
        TYPE_ASSERT_PREFIX(void (void), true);
        TYPE_ASSERT_PREFIX(int  (void), true);
        TYPE_ASSERT_PREFIX(void (int),  true);
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = "
             << testStatus << "." << endl;
    }

    return testStatus;
}
