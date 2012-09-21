// bslmf_isrvaluereference.t.cpp                                      -*-C++-*-
#include <bslmf_isrvaluereference.h>

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
// The object under test is a meta-function, 'bsl::is_rvalue_reference', which
// determine whether a template parameter type is a function type.  Thus, we
// need to ensure that the value returned by this meta-functions is correct for
// each possible category of types.
//
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bsl::is_rvalue_reference::value
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
    // This user-defined 'enum' type is intended to be used during testing as
    // an argument for the template parameter 'TYPE' of
    // 'bsl::is_rvalue_reference'.
};

struct StructTestType {
    // This user-defined 'struct' type is intended to be used during testing as
    // an argument for the template parameter 'TYPE' of
    // 'bsl::is_rvalue_reference'.
};

union  UnionTestType {
    // This user-defined 'union' type is intended to be used during testing as
    // an argument for the template parameter 'TYPE' of
    // 'bsl::is_rvalue_reference'.
};

class  BaseClassTestType {
    // This user-defined base class type is intended to be used during testing
    // as an argument for the template parameter 'TYPE' of
    // 'bsl::is_rvalue_reference'.
};

class  DerivedClassTestType : public BaseClassTestType {
    // This user-defined derived class type is intended to be used during
    // testing as an argument for the template parameter 'TYPE' of
    // 'bsl::is_rvalue_reference'.
};

typedef int (StructTestType::*MethodPtrTestType) ();
    // This non-static function member type is intended to be used during
    // testing as an argument for the template parameter 'TYPE' of
    // 'bsl::is_rvalue_reference'.

typedef void (*FunctionPtrTestType) ();
    // This function pointer type is intended to be used during testing as an
    // argument as an argument for the template parameter 'TYPE' of
    // 'bsl::is_rvalue_reference'.

typedef int StructTestType::* PMD;
    // This class public data member pointer type is intended to be used during
    // testing as an argument as an argument for the template parameter 'TYPE'
    // of 'bsl::is_rvalue_reference'.

struct Incomplete;
    // This incomplete 'struct' type is intended to be used during testing as
    // an argument as an argument for the template parameter 'TYPE' of
    // 'bsl::is_rvalue_reference'.

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

#define TYPE_ASSERT_CVQ_RREF(META_FUNC, TYPE, result)           \
    ASSERT(result == META_FUNC<TYPE&&>::value);                 \
    ASSERT(result == META_FUNC<TYPE const&&>::value);           \
    ASSERT(result == META_FUNC<TYPE volatile&&>::value);        \
    ASSERT(result == META_FUNC<TYPE const volatile&&>::value);

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


      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::is_rvalue_reference::value'
        //   Ensure that the static data member 'value' of
        //   'bsl::is_rvalue_reference' instantiations having various (template
        //   parameter) 'TYPES' has the correct value.
        //
        // Concerns:
        //: 1 'is_rvalue_reference::value' is 'false' when 'TYPE' is a
        //:   (possibly cv-qualified) primitive type.
        //:
        //: 2 'is_rvalue_reference::value' is 'false' when 'TYPE' is a
        //:   (possibly cv-qualified) user-defined type.
        //:
        //: 3 'is_rvalue_reference::value' is 'false' when 'TYPE' is a
        //:   (possibly cv-qualified) pointer type.
        //:
        //: 4 'is_rvalue_reference::value' is 'true' when 'TYPE' is a
        //:   (possibly cv-qualified) lvalue reference type.
        //:
        //: 5 'is_rvalue_reference::value' is 'true' when 'TYPE' is a
        //:   (possibly cv-qualified) rvalue reference type.
        //:
        //: 6 'is_rvalue_reference::value' is 'false' when 'TYPE' is a
        //:   (possibly cv-qualified) function type.
        //
        // Plan:
        //   Verify that 'bsl::is_rvalue_reference::value' has the correct
        //   value for each (template parameter) 'TYPE' in the concerns.
        //
        // Testing:
        //   bsl::is_rvalue_reference::value
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "bsl::is_rvalue_reference::value" << endl
                          << "===============================" << endl;

        // C-1
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_rvalue_reference, void, false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_rvalue_reference, int,  false);

        // C-2
        TYPE_ASSERT_CVQ_SUFFIX(
                        bsl::is_rvalue_reference, EnumTestType,         false);
        TYPE_ASSERT_CVQ_SUFFIX(
                        bsl::is_rvalue_reference, StructTestType,       false);
        TYPE_ASSERT_CVQ_SUFFIX(
                        bsl::is_rvalue_reference, UnionTestType,        false);
        TYPE_ASSERT_CVQ_SUFFIX(
                        bsl::is_rvalue_reference, BaseClassTestType,    false);
        TYPE_ASSERT_CVQ_SUFFIX(
                        bsl::is_rvalue_reference, DerivedClassTestType, false);

        // C-3
        TYPE_ASSERT_CVQ(
                  bsl::is_rvalue_reference, int*,                       false);
        TYPE_ASSERT_CVQ(
                  bsl::is_rvalue_reference, StructTestType*,            false);
        TYPE_ASSERT_CVQ(
                  bsl::is_rvalue_reference, int StructTestType::*,      false);
        TYPE_ASSERT_CVQ(
                  bsl::is_rvalue_reference, int StructTestType::* *,    false);
        TYPE_ASSERT_CVQ(
                  bsl::is_rvalue_reference, UnionTestType*,             false);
        TYPE_ASSERT_CVQ(
                  bsl::is_rvalue_reference, PMD BaseClassTestType::*,   false);
        TYPE_ASSERT_CVQ(
                  bsl::is_rvalue_reference, PMD BaseClassTestType::* *, false);
        TYPE_ASSERT_CVQ(
                  bsl::is_rvalue_reference, BaseClassTestType*,         false);
        TYPE_ASSERT_CVQ(
                  bsl::is_rvalue_reference, DerivedClassTestType*,      false);
        TYPE_ASSERT_CVQ(
                  bsl::is_rvalue_reference, Incomplete*,                false);
        TYPE_ASSERT_CVQ_SUFFIX(
                         bsl::is_rvalue_reference, MethodPtrTestType,   false);
        TYPE_ASSERT_CVQ_SUFFIX(
                         bsl::is_rvalue_reference, FunctionPtrTestType, false);

        // C-4
        TYPE_ASSERT_CVQ_REF(bsl::is_rvalue_reference, int,  false);

        TYPE_ASSERT_CVQ_REF(
                        bsl::is_rvalue_reference, EnumTestType,         false);
        TYPE_ASSERT_CVQ_REF(
                        bsl::is_rvalue_reference, StructTestType,       false);
        TYPE_ASSERT_CVQ_REF(
                        bsl::is_rvalue_reference, UnionTestType,        false);
        TYPE_ASSERT_CVQ_REF(
                        bsl::is_rvalue_reference, BaseClassTestType,    false);
        TYPE_ASSERT_CVQ_REF(
                        bsl::is_rvalue_reference, DerivedClassTestType, false);

        TYPE_ASSERT_CVQ_REF(
                    bsl::is_rvalue_reference, int*,                     false);
        TYPE_ASSERT_CVQ_REF(
                    bsl::is_rvalue_reference, StructTestType*,          false);
        TYPE_ASSERT_CVQ_REF(
                    bsl::is_rvalue_reference, int StructTestType::*,    false);
        TYPE_ASSERT_CVQ_REF(
                    bsl::is_rvalue_reference, int StructTestType::* *,  false);
        TYPE_ASSERT_CVQ_REF(
                    bsl::is_rvalue_reference, UnionTestType*,           false);
        TYPE_ASSERT_CVQ_REF(
                    bsl::is_rvalue_reference, PMD BaseClassTestType::*, false);
        TYPE_ASSERT_CVQ_REF(
                  bsl::is_rvalue_reference, PMD BaseClassTestType::* *, false);
        TYPE_ASSERT_CVQ_REF(
                    bsl::is_rvalue_reference, BaseClassTestType*,       false);
        TYPE_ASSERT_CVQ_REF(
                    bsl::is_rvalue_reference, DerivedClassTestType*,    false);
        TYPE_ASSERT_CVQ_REF(
                    bsl::is_rvalue_reference, Incomplete*,              false);
        TYPE_ASSERT_CVQ_REF(
                    bsl::is_rvalue_reference, MethodPtrTestType,        false);
        TYPE_ASSERT_CVQ_REF(
                    bsl::is_rvalue_reference, FunctionPtrTestType,      false);

        // C-5
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        TYPE_ASSERT_CVQ_RREF(bsl::is_rvalue_reference, int,  true);

        TYPE_ASSERT_CVQ_RREF(
                         bsl::is_rvalue_reference, EnumTestType,         true);
        TYPE_ASSERT_CVQ_RREF(
                         bsl::is_rvalue_reference, StructTestType,       true);
        TYPE_ASSERT_CVQ_RREF(
                         bsl::is_rvalue_reference, UnionTestType,        true);
        TYPE_ASSERT_CVQ_RREF(
                         bsl::is_rvalue_reference, BaseClassTestType,    true);
        TYPE_ASSERT_CVQ_RREF(
                         bsl::is_rvalue_reference, DerivedClassTestType, true);

        TYPE_ASSERT_CVQ_RREF(
                     bsl::is_rvalue_reference, int*,                     true);
        TYPE_ASSERT_CVQ_RREF(
                     bsl::is_rvalue_reference, StructTestType*,          true);
        TYPE_ASSERT_CVQ_RREF(
                     bsl::is_rvalue_reference, int StructTestType::*,    true);
        TYPE_ASSERT_CVQ_RREF(
                     bsl::is_rvalue_reference, int StructTestType::* *,  true);
        TYPE_ASSERT_CVQ_RREF(
                     bsl::is_rvalue_reference, UnionTestType*,           true);
        TYPE_ASSERT_CVQ_RREF(
                     bsl::is_rvalue_reference, PMD BaseClassTestType::*, true);
        TYPE_ASSERT_CVQ_RREF(
                   bsl::is_rvalue_reference, PMD BaseClassTestType::* *, true);
        TYPE_ASSERT_CVQ_RREF(
                     bsl::is_rvalue_reference, BaseClassTestType*,       true);
        TYPE_ASSERT_CVQ_RREF(
                     bsl::is_rvalue_reference, DerivedClassTestType*,    true);
        TYPE_ASSERT_CVQ_RREF(
                     bsl::is_rvalue_reference, Incomplete*,              true);
        TYPE_ASSERT_CVQ_RREF(
                     bsl::is_rvalue_reference, MethodPtrTestType,        true);
        TYPE_ASSERT_CVQ_RREF(
                     bsl::is_rvalue_reference, FunctionPtrTestType,      true);
#endif

        // C-6
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_rvalue_reference, int  (int),  false);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_rvalue_reference, void (void), false);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_rvalue_reference, int  (void), false);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_rvalue_reference, void (int),  false);
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
