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
// The object under test is a meta-functions, 'bsl::is_function', which
// determine whether a template parameter type is a function type.  Thus, we
// need to ensure that the value returned by these meta-functions are correct
// for each possible category of types.
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

static void aSsErT(int c, const char *s, int i)
{
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

enum   EnumTestType {
   // This user-defined 'enum' type is intended to be used during testing as an
   // argument for the template parameter 'TYPE' of 'bsl::is_function'.
};

struct StructTestType {
   // This user-defined 'struct' type is intended to be used during testing as
   // an argument for the template parameter 'TYPE' of 'bsl::is_function'.
};

union  UnionTestType {
   // This user-defined 'union' type is intended to be used during testing as
   // an argument for the template parameter 'TYPE' of 'bsl::is_function'.
};

class  BaseClassTestType {
   // This user-defined base class type is intended to be used during testing
   // as an argument for the template parameter 'TYPE' of 'bsl::is_function'.
};

class  DerivedClassTestType : public BaseClassTestType {
   // This user-defined derived class type is intended to be used during
   // testing as an argument for the template parameter 'TYPE' of
   // 'bsl::is_function'.
};

typedef int (StructTestType::*MethodPtrTestType) ();
    // This non-static function member type is intended to be used during
    // testing as an argument for the template parameter 'TYPE' of
    // 'bsl::is_function'.

typedef void (*FunctionPtrTestType) ();
    // This function pointer type is intended to be used during testing as an
    // argument as an argument for the template parameter 'TYPE' of
    // 'bsl::is_function'.

typedef int StructTestType::* PMD;
    // This class public data member pointer type is intended to be used during
    // testing as an argument as an argument for the template parameter 'TYPE'
    // of 'bsl::is_function'.

struct Incomplete;
    // This incomplete 'struct' type is intended to be used during testing as
    // an argument as an argument for the template parameter 'TYPE' of
    // 'bsl::is_function'.

}  // close unnamed namespace

#define TYPE_ASSERT_CVQ_PREFIX(META_FUNC, TYPE, result)       \
    ASSERT(result == META_FUNC<TYPE>::value);                 \
    ASSERT(result == META_FUNC<const TYPE>::value);           \
    ASSERT(result == META_FUNC<volatile TYPE>::value);        \
    ASSERT(result == META_FUNC<const volatile TYPE>::value);

#define TYPE_ASSERT_CVQ_SUFFIX(META_FUNC, TYPE, result)       \
    ASSERT(result == META_FUNC<TYPE>::value);                 \
    ASSERT(result == META_FUNC<TYPE const>::value);           \
    ASSERT(result == META_FUNC<TYPE volatile>::value);        \
    ASSERT(result == META_FUNC<TYPE const volatile>::value);

#define TYPE_ASSERT_CVQ_REF(META_FUNC, TYPE, result)           \
    ASSERT(result == META_FUNC<TYPE&>::value);                 \
    ASSERT(result == META_FUNC<TYPE const&>::value);           \
    ASSERT(result == META_FUNC<TYPE volatile&>::value);        \
    ASSERT(result == META_FUNC<TYPE const volatile&>::value);

#define TYPE_ASSERT_CVQ(META_FUNC, TYPE, result)                     \
    TYPE_ASSERT_CVQ_PREFIX(META_FUNC, TYPE, result);                 \
    TYPE_ASSERT_CVQ_PREFIX(META_FUNC, TYPE const, result);           \
    TYPE_ASSERT_CVQ_PREFIX(META_FUNC, TYPE volatile, result);        \
    TYPE_ASSERT_CVQ_PREFIX(META_FUNC, TYPE const volatile, result);  \

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
// Suppose that we want to assert whether a particular type is a function type.
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
        //:   cv-qualified) primitive type.
        //
        //: 2 'is_function::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) user-defined type.
        //:
        //: 3 'is_function::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) pointer type.
        //:
        //: 4 'is_function::value' is 'true' when 'TYPE' is a (possibly
        //:   cv-qualified) function type.
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
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_function, void, false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_function, int,  false);
        TYPE_ASSERT_CVQ_REF   (bsl::is_function, int,  false);

        // C-2
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_function, EnumTestType,         false);
        TYPE_ASSERT_CVQ_REF   (bsl::is_function, EnumTestType,         false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_function, StructTestType,       false);
        TYPE_ASSERT_CVQ_REF   (bsl::is_function, StructTestType,       false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_function, UnionTestType,        false);
        TYPE_ASSERT_CVQ_REF   (bsl::is_function, UnionTestType,        false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_function, BaseClassTestType,    false);
        TYPE_ASSERT_CVQ_REF   (bsl::is_function, BaseClassTestType,    false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_function, DerivedClassTestType, false);
        TYPE_ASSERT_CVQ_REF   (bsl::is_function, DerivedClassTestType, false);

        // C-3
        TYPE_ASSERT_CVQ(bsl::is_function, int*,                       false);
        TYPE_ASSERT_CVQ(bsl::is_function, StructTestType*,            false);
        TYPE_ASSERT_CVQ(bsl::is_function, int StructTestType::*,      false);
        TYPE_ASSERT_CVQ(bsl::is_function, int StructTestType::* *,    false);
        TYPE_ASSERT_CVQ(bsl::is_function, UnionTestType*,             false);
        TYPE_ASSERT_CVQ(bsl::is_function, PMD BaseClassTestType::*,   false);
        TYPE_ASSERT_CVQ(bsl::is_function, PMD BaseClassTestType::* *, false);
        TYPE_ASSERT_CVQ(bsl::is_function, BaseClassTestType*,         false);
        TYPE_ASSERT_CVQ(bsl::is_function, DerivedClassTestType*,      false);
        TYPE_ASSERT_CVQ(bsl::is_function, Incomplete*,                false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_function, MethodPtrTestType,   false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_function, FunctionPtrTestType, false);

        TYPE_ASSERT_CVQ_REF(bsl::is_function, int*,                     false);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, StructTestType*,          false);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, int StructTestType::*,    false);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, int StructTestType::* *,  false);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, UnionTestType*,           false);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, PMD BaseClassTestType::*, false);
        TYPE_ASSERT_CVQ_REF(
                          bsl::is_function, PMD BaseClassTestType::* *, false);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, BaseClassTestType*,       false);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, DerivedClassTestType*,    false);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, Incomplete*,              false);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, MethodPtrTestType,        false);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, FunctionPtrTestType,      false);

        // C-4
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_function, int  (int),  true);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_function, void (void), true);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_function, int  (void), true);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_function, void (int),  true);
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
