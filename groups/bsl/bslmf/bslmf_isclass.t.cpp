// bslmf_isclass.t.cpp                                                -*-C++-*-

#include <bslmf_isclass.h>

#include <bsls_platform.h>            // for testing only

#include <cstdlib>    // atoi()
#include <iostream>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The objects under test are two meta-functions, 'bsl::is_class' and
// 'bslmf::IsClass', that determine whether a template parameter type is a
// class type.  Thus, we need to ensure that the value returned by these
// meta-functions are correct for each possible category of types.  Since the
// two meta-functions are functionally equivalent, we will use the same set of
// types for both.
//
//-----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 2] BloombergLP::bslmf::IsClass::VALUE
// [ 1] bsl::is_class::value
//
// ----------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE

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
   // argument for the template parameter 'TYPE' of 'bsl::is_class' and
   // 'bslmf::IsClass'.
};

struct StructTestType {
   // This user-defined 'struct' type is intended to be used during testing as
   // an argument for the template parameter 'TYPE' of 'bsl::is_class' and
   // 'bslmf::IsClass'.
};

union  UnionTestType {
   // This user-defined 'union' type is intended to be used during testing as
   // an argument for the template parameter 'TYPE' of 'bsl::is_class' and
   // 'bslmf::IsClass'.
};

class  BaseClassTestType {
   // This user-defined base class type is intended to be used during testing
   // as an argument for the template parameter 'TYPE' of 'bsl::is_class' and
   // 'bslmf::IsClass'.
};

class  DerivedClassTestType : public BaseClassTestType {
   // This user-defined derived class type is intended to be used during
   // testing as an argument for the template parameter 'TYPE' of
   // 'bsl::is_class' and 'bslmf::IsClass'.
};

typedef int (StructTestType::*MethodPtrTestType) ();
    // This non-static function member type is intended to be used during
    // testing as an argument for the template parameter 'TYPE' of
    // 'bsl::is_class' and 'bslmf::IsClass'.

typedef void (*FunctionPtrTestType) ();
    // This function pointer type is intended to be used during testing as an
    // argument as an argument for the template parameter 'TYPE' of
    // 'bsl::is_class' and 'bslmf::IsClass'.

typedef int StructTestType::* PMD;
    // This class public data member pointer type is intended to be used during
    // testing as an argument as an argument for the template parameter 'TYPE'
    // of 'bsl::is_class' and 'bslmf::IsClass'.

struct Incomplete;
    // This incomplete 'struct' type is intended to be used during testing as
    // an argument as an argument for the template parameter 'TYPE' of
    // 'bsl::is_class' and 'bslmf::IsClass'.

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
//                 CLASSES, ETC. FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Class Types
///- - - - - - - - - - - - - - -
// Suppose that we want to assert whether a particular type is a class type.
//
// First, we create a class type 'MyClass':
//..
    class MyClass
    {
    };
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
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

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;
// Now, we instantiate the 'bsl::is_class' template for both a non-class type
// and the defined type 'MyClass', asserting the 'value' static data member of
// each instantiation:
//..
    ASSERT(false == bsl::is_class<int>::value);
    ASSERT(true  == bsl::is_class<MyClass>::value);
//..

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // 'bslmf::IsClass::VALUE'
        //   Ensure that the static data member 'VALUE' of 'bslmf::IsClass'
        //   instantiations having various (template parameter) 'TYPES' has the
        //   correct value.
        //
        // Concerns:
        //: 1 'IsClass::VALUE' is 0 when 'TYPE' is a (possibly cv-qualified)
        //:   primitve type.
        //:
        //: 2 'IsClass::VALUE' is 0 when 'TYPE' is a (possibly cv-qualified)
        //:   'enum' type.
        //:
        //: 3 'IsClass::VALUE' is 1 when 'TYPE' is a (possibly cv-qualified)
        //:   'class', 'struct', or 'union' type.
        //:
        //: 4 'IsClass::VALUE' is 0 when 'TYPE' is a (possibly cv-qualified)
        //:   pointer type.
        //:
        //: 5 'IsClass::VALUE' is 0 when 'TYPE' is a (possibly cv-qualifie)
        //:   function type.
        //
        // Plan:
        //   Verify that 'bsl::IsClass::VALUE' has the correct value for each
        //   (template parameter) 'TYPE' in the concerns.
        //
        // Testing:
        //   bslmf::IsClass::VALUE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "bslmf::IsClass" << endl
                          << "==============" << endl;

        // C-1
        TYPE_ASSERT_CVQ_SUFFIX(bslmf::IsClass, void, 0);
        TYPE_ASSERT_CVQ_SUFFIX(bslmf::IsClass, int,  0);
        TYPE_ASSERT_CVQ_REF   (bslmf::IsClass, int,  0);

        // C-2
        TYPE_ASSERT_CVQ_SUFFIX(bslmf::IsClass, EnumTestType, 0);

        // C-3
        TYPE_ASSERT_CVQ_SUFFIX(bslmf::IsClass, StructTestType,       1);
        TYPE_ASSERT_CVQ_REF   (bslmf::IsClass, StructTestType,       1);
        TYPE_ASSERT_CVQ_SUFFIX(bslmf::IsClass, UnionTestType,        1);
        TYPE_ASSERT_CVQ_REF   (bslmf::IsClass, UnionTestType,        1);
        TYPE_ASSERT_CVQ_SUFFIX(bslmf::IsClass, Incomplete,           1);
        TYPE_ASSERT_CVQ_REF   (bslmf::IsClass, Incomplete,           1);
        TYPE_ASSERT_CVQ_SUFFIX(bslmf::IsClass, BaseClassTestType,    1);
        TYPE_ASSERT_CVQ_REF   (bslmf::IsClass, BaseClassTestType,    1);
        TYPE_ASSERT_CVQ_SUFFIX(bslmf::IsClass, DerivedClassTestType, 1);
        TYPE_ASSERT_CVQ_REF   (bslmf::IsClass, DerivedClassTestType, 1);

        // C-4
        TYPE_ASSERT_CVQ(bslmf::IsClass, int*,                       0);
        TYPE_ASSERT_CVQ(bslmf::IsClass, StructTestType*,            0);
        TYPE_ASSERT_CVQ(bslmf::IsClass, int StructTestType::*,      0);
        TYPE_ASSERT_CVQ(bslmf::IsClass, int StructTestType::* *,    0);
        TYPE_ASSERT_CVQ(bslmf::IsClass, UnionTestType*,             0);
        TYPE_ASSERT_CVQ(bslmf::IsClass, PMD BaseClassTestType::*,   0);
        TYPE_ASSERT_CVQ(bslmf::IsClass, PMD BaseClassTestType::* *, 0);
        TYPE_ASSERT_CVQ(bslmf::IsClass, BaseClassTestType*,         0);
        TYPE_ASSERT_CVQ(bslmf::IsClass, DerivedClassTestType*,      0);
        TYPE_ASSERT_CVQ(bslmf::IsClass, Incomplete*,                0);
        TYPE_ASSERT_CVQ_SUFFIX(bslmf::IsClass, MethodPtrTestType,   0);
        TYPE_ASSERT_CVQ_SUFFIX(bslmf::IsClass, FunctionPtrTestType, 0);

        TYPE_ASSERT_CVQ_REF(bslmf::IsClass, int*,                       0);
        TYPE_ASSERT_CVQ_REF(bslmf::IsClass, StructTestType*,            0);
        TYPE_ASSERT_CVQ_REF(bslmf::IsClass, int StructTestType::*,      0);
        TYPE_ASSERT_CVQ_REF(bslmf::IsClass, int StructTestType::* *,    0);
        TYPE_ASSERT_CVQ_REF(bslmf::IsClass, UnionTestType*,             0);
        TYPE_ASSERT_CVQ_REF(bslmf::IsClass, PMD BaseClassTestType::*,   0);
        TYPE_ASSERT_CVQ_REF(bslmf::IsClass, PMD BaseClassTestType::* *, 0);
        TYPE_ASSERT_CVQ_REF(bslmf::IsClass, BaseClassTestType*,         0);
        TYPE_ASSERT_CVQ_REF(bslmf::IsClass, DerivedClassTestType*,      0);
        TYPE_ASSERT_CVQ_REF(bslmf::IsClass, Incomplete*,                0);
        TYPE_ASSERT_CVQ_REF(bslmf::IsClass, MethodPtrTestType,          0);
        TYPE_ASSERT_CVQ_REF(bslmf::IsClass, FunctionPtrTestType,        0);

        // C-5
        TYPE_ASSERT_CVQ_PREFIX(bslmf::IsClass, int  (int),  0);
        TYPE_ASSERT_CVQ_PREFIX(bslmf::IsClass, void (void), 0);
        TYPE_ASSERT_CVQ_PREFIX(bslmf::IsClass, int  (void), 0);
        TYPE_ASSERT_CVQ_PREFIX(bslmf::IsClass, void (int),  0);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::is_class::value'
        //   Ensure that the static data member 'value' of 'bsl::is_class'
        //   instantiations having various (template parameter) 'TYPES' has the
        //   correct value.
        //
        // Concerns:
        //: 1 'is_class::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) primitve type.
        //:
        //: 2 'is_class::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) 'enum' type.
        //:
        //: 3 'is_class::value' is 'true' when 'TYPE' is a (possibly
        //:   cv-qualified) 'class', 'struct', or 'union' type.
        //:
        //: 4 'is_class::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) pointer type.
        //:
        //: 5 'is_class::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualifie) function type.
        //
        // Plan:
        //   Verify that 'bsl::is_class::value' has the correct value for
        //   each (template parameter) 'TYPE' in the concerns.
        //
        // Testing:
        //   bsl::is_class::value
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "bsl::is_class" << endl
                          << "=============" << endl;

        // C-1
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_class, void, false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_class, int,  false);
        TYPE_ASSERT_CVQ_REF   (bsl::is_class, int,  false);

        // C-2
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_class, EnumTestType, false);

        // C-3
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_class, StructTestType,       true);
        TYPE_ASSERT_CVQ_REF   (bsl::is_class, StructTestType,       true);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_class, UnionTestType,        true);
        TYPE_ASSERT_CVQ_REF   (bsl::is_class, UnionTestType,        true);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_class, Incomplete,           true);
        TYPE_ASSERT_CVQ_REF   (bsl::is_class, Incomplete,           true);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_class, BaseClassTestType,    true);
        TYPE_ASSERT_CVQ_REF   (bsl::is_class, BaseClassTestType,    true);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_class, DerivedClassTestType, true);
        TYPE_ASSERT_CVQ_REF   (bsl::is_class, DerivedClassTestType, true);

        // C-4
        TYPE_ASSERT_CVQ(bsl::is_class, int*,                       false);
        TYPE_ASSERT_CVQ(bsl::is_class, StructTestType*,            false);
        TYPE_ASSERT_CVQ(bsl::is_class, int StructTestType::*,      false);
        TYPE_ASSERT_CVQ(bsl::is_class, int StructTestType::* *,    false);
        TYPE_ASSERT_CVQ(bsl::is_class, UnionTestType*,             false);
        TYPE_ASSERT_CVQ(bsl::is_class, PMD BaseClassTestType::*,   false);
        TYPE_ASSERT_CVQ(bsl::is_class, PMD BaseClassTestType::* *, false);
        TYPE_ASSERT_CVQ(bsl::is_class, BaseClassTestType*,         false);
        TYPE_ASSERT_CVQ(bsl::is_class, DerivedClassTestType*,      false);
        TYPE_ASSERT_CVQ(bsl::is_class, Incomplete*,                false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_class, MethodPtrTestType,   false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_class, FunctionPtrTestType, false);

        TYPE_ASSERT_CVQ_REF(bsl::is_class, int*,                       false);
        TYPE_ASSERT_CVQ_REF(bsl::is_class, StructTestType*,            false);
        TYPE_ASSERT_CVQ_REF(bsl::is_class, int StructTestType::*,      false);
        TYPE_ASSERT_CVQ_REF(bsl::is_class, int StructTestType::* *,    false);
        TYPE_ASSERT_CVQ_REF(bsl::is_class, UnionTestType*,             false);
        TYPE_ASSERT_CVQ_REF(bsl::is_class, PMD BaseClassTestType::*,   false);
        TYPE_ASSERT_CVQ_REF(bsl::is_class, PMD BaseClassTestType::* *, false);
        TYPE_ASSERT_CVQ_REF(bsl::is_class, BaseClassTestType*,         false);
        TYPE_ASSERT_CVQ_REF(bsl::is_class, DerivedClassTestType*,      false);
        TYPE_ASSERT_CVQ_REF(bsl::is_class, Incomplete*,                false);
        TYPE_ASSERT_CVQ_REF(bsl::is_class, MethodPtrTestType,          false);
        TYPE_ASSERT_CVQ_REF(bsl::is_class, FunctionPtrTestType,        false);

        // C-5
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_class, int  (int),  false);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_class, void (void), false);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_class, int  (void), false);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_class, void (int),  false);
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
