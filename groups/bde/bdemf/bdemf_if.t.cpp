// bdemf_if.t.cpp            -*-C++-*-

#include <bdemf_if.h>

#include <bslmf_nil.h>  // for testing only

#include <bsl_iostream.h>

#include <bsl_cstdlib.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test contains the simple meta-function 'bdemf_If'.  The
// meta-function is tested by enumeration of all combinations of (1) 'true' and
// 'false' conditions, and (2) defaulting of zero, one, or two of the type
// arguments to 'bslmf_Nil'.
//-----------------------------------------------------------------------------
// [ 1] bdemf_If<CONDITION, IF_TRUE_TYPE, IF_FALSE_TYPE>

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

    // 'SizeOf' is a meta-function that returns the non-zero size of its type
    // argument, or 0 for 'bslmf_Nil'.

    template <class T>
    struct SizeOf {
        static const size_t VALUE = sizeof(T);
    };

    template <>
    struct SizeOf<bslmf_Nil> {
        static const size_t VALUE = 0;
    };

#define SIZEOF(TYPE) SizeOf<TYPE>::VALUE

    // The compile-time evaluations depend on the following types being of
    // different sizes.

    typedef char   TrueType;   // sizeof(TrueType) == 1
    typedef double FalseType;  // sizeof(FalseType) > 1

    const size_t True  = SIZEOF(TrueType);
    const size_t False = SIZEOF(FalseType);
    const size_t Nil   = 0;

    typedef bdemf_If<true, TrueType, FalseType>::Type CT0;
    const size_t C0 = SIZEOF(CT0);                        // True

    typedef bdemf_If<false, TrueType, FalseType>::Type CT1;
    const size_t C1 = SIZEOF(CT1);                        // False

    typedef bdemf_If<true>::Type CT2;
    const size_t C2 = SIZEOF(CT2);                        // Nil

    typedef bdemf_If<false>::Type CT3;
    const size_t C3 = SIZEOF(CT3);                        // Nil

    typedef bdemf_If<true, TrueType>::Type CT4;
    const size_t C4 = SIZEOF(CT4);                        // True

    typedef bdemf_If<false, TrueType>::Type CT5;
    const size_t C5 = SIZEOF(CT5);                        // Nil

    typedef bdemf_If<true, TrueType, FalseType>::Type CT10;
    const size_t C10 = SIZEOF(CT10);                      // True

    typedef bdemf_If<false, TrueType, FalseType>::Type CT11;
    const size_t C11 = SIZEOF(CT11);                      // False

    typedef bdemf_If<true>::Type CT12;
    const size_t C12 = SIZEOF(CT12);                      // Nil

    typedef bdemf_If<false>::Type CT13;
    const size_t C13 = SIZEOF(CT13);                      // Nil

    typedef bdemf_If<true, TrueType>::Type CT14;
    const size_t C14 = SIZEOF(CT14);                      // True

    typedef bdemf_If<false, TrueType>::Type CT15;
    const size_t C15 = SIZEOF(CT15);                      // Nil

}  // close namespace 'anonymous'

// Test DEPRECATED 'bdemf_if' until it is removed from the .h file.

namespace Deprecated {

    // 'SizeOf' is a meta-function that returns the non-zero size of its type
    // argument, or 0 for 'bslmf_Nil'.

    template <class T>
    struct SizeOf {
        static const size_t VALUE = sizeof(T);
    };

    template <>
    struct SizeOf<bslmf_Nil> {
        static const size_t VALUE = 0;
    };

    // The compile-time evaluations depend on the following types being of
    // different sizes.

    typedef char   TrueType;   // sizeof(TrueType) == 1
    typedef double FalseType;  // sizeof(FalseType) > 1

    const size_t True  = SizeOf<TrueType>::VALUE;
    const size_t False = SizeOf<FalseType>::VALUE;
    const size_t Nil   = 0;

    typedef bdemf_if<true, TrueType, FalseType>::Type CT0;
    const size_t C0 = SizeOf<CT0>::VALUE;                        // True

    typedef bdemf_if<false, TrueType, FalseType>::Type CT1;
    const size_t C1 = SizeOf<CT1>::VALUE;                        // False

    typedef bdemf_if<true>::Type CT2;
    const size_t C2 = SizeOf<CT2>::VALUE;                        // Nil

    typedef bdemf_if<false>::Type CT3;
    const size_t C3 = SizeOf<CT3>::VALUE;                        // Nil

    typedef bdemf_if<true, TrueType>::Type CT4;
    const size_t C4 = SizeOf<CT4>::VALUE;                        // True

    typedef bdemf_if<false, TrueType>::Type CT5;
    const size_t C5 = SizeOf<CT5>::VALUE;                        // Nil

    typedef bdemf_if<true, TrueType, FalseType>::Type CT10;
    const size_t C10 = SizeOf<CT10>::VALUE;                      // True

    typedef bdemf_if<false, TrueType, FalseType>::Type CT11;
    const size_t C11 = SizeOf<CT11>::VALUE;                      // False

    typedef bdemf_if<true>::Type CT12;
    const size_t C12 = SizeOf<CT12>::VALUE;                      // Nil

    typedef bdemf_if<false>::Type CT13;
    const size_t C13 = SizeOf<CT13>::VALUE;                      // Nil

    typedef bdemf_if<true, TrueType>::Type CT14;
    const size_t C14 = SizeOf<CT14>::VALUE;                      // True

    typedef bdemf_if<false, TrueType>::Type CT15;
    const size_t C15 = SizeOf<CT15>::VALUE;                      // Nil

}  // close namespace 'Deprecated'

//=============================================================================
//                GLOBAL TYPES AND FUNCTIONS FOR USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
//------
// The following snippets of code illustrate basic use of the 'bdemf_If'
// meta-function.  The examples make use of the following declarations to
// identify the type that is selected by a given constant integral expression:
//..
    enum TypeCode { T_UNKNOWN = 0, T_CHAR = 1, T_INT = 2, T_NIL = 3 };

    TypeCode whatType(char)      { return T_CHAR; }
    TypeCode whatType(int)       { return T_INT; }
    TypeCode whatType(bslmf_Nil) { return T_NIL; }
    TypeCode whatType(...)       { return T_UNKNOWN; }
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
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   This case verifies that the usage example works as advertised.
        //
        // Concerns:
        //   That the usage example compiles, links, and runs as expected.
        //
        // Plan:
        //   Replicate the usage example from the component header here.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

// In the following example, the meta-function condition (the first argument to
// 'bdemf_If') evaluates to true (non-zero).  Thus, 'bdemf_If<...>::Type' is a
// synonym for 'int'; i.e., it "evaluates" (at compile time) to 'int':
//..
     typedef int  T1;  ASSERT(1 <  sizeof(T1));
     typedef char T2;  ASSERT(1 == sizeof(T2));

     typedef bdemf_If<(sizeof(T1) > sizeof(T2)), T1, T2>::Type LargerType;
     ASSERT(T_INT == whatType(LargerType()));
//..
// In the next example, the condition argument evaluates to false (zero).  In
// this case, 'bdemf_If<...>::Type' evaluates to 'bslmf_Nil' since the third
// template argument (the "else" type) is not explicitly specified:
//..
     typedef bdemf_If<(sizeof(T2) > 1), int>::Type Type2;
     ASSERT(T_NIL == whatType(Type2()));
//..

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // TESTING BDEMF_IF
        //   Test the 'bdemf_If' meta-function.
        //
        // Concerns:
        //   - That 'true' and 'false' conditions select the first and second
        //     type arguments, respectively.
        //   - That selected types that are not explicitly specified resolve to
        //     the default 'bslmf_Nil' type.
        //
        // Plan:
        //   - Provide distinct type arguments with both 'true' and 'false'
        //     conditions and verify that the correct type is selected.
        //   - Repeat the tests such that:
        //      1. The second type parameter (only) defaults to 'bslmf_Nil'.
        //      2. Both type parameters default to 'bslmf_Nil'.
        //
        // Testing:
        //   bdemf_If<CONDITION, IF_TRUE_TYPE, IF_FALSE_TYPE>
        // --------------------------------------------------------------------

        if (verbose) cout << "bdemf_If" << endl
                          << "========" << endl;

        ASSERT(1 == True);
        ASSERT(1 <  False);

        ASSERT(True  == C0);
        ASSERT(False == C1);
        ASSERT(Nil   == C2);
        ASSERT(Nil   == C3);
        ASSERT(True  == C4);
        ASSERT(Nil   == C5);

        ASSERT(C10   == C0);
        ASSERT(C11   == C1);
        ASSERT(C12   == C2);
        ASSERT(C13   == C3);
        ASSERT(C14   == C4);
        ASSERT(C15   == C5);

        // Test DEPRECATED 'bdemf_if' until it is removed from the .h file.
        {
            ASSERT(1 == Deprecated::True);
            ASSERT(1 <  Deprecated::False);

            ASSERT(Deprecated::True  == Deprecated::C0);
            ASSERT(Deprecated::False == Deprecated::C1);
            ASSERT(Deprecated::Nil   == Deprecated::C2);
            ASSERT(Deprecated::Nil   == Deprecated::C3);
            ASSERT(Deprecated::True  == Deprecated::C4);
            ASSERT(Deprecated::Nil   == Deprecated::C5);

            ASSERT(Deprecated::C10   == Deprecated::C0);
            ASSERT(Deprecated::C11   == Deprecated::C1);
            ASSERT(Deprecated::C12   == Deprecated::C2);
            ASSERT(Deprecated::C13   == Deprecated::C3);
            ASSERT(Deprecated::C14   == Deprecated::C4);
            ASSERT(Deprecated::C15   == Deprecated::C5);
        }

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
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
