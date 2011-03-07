// bslmf_forwardingtype.t.cpp      -*-C++-*-
#include <bslmf_forwardingtype.h>

#include <bslmf_issame.h>          // for testing only

#include <bslmf_isarray.h>

#include <cstdlib>    // atoi()
#include <iostream>
#include <typeinfo>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// This component provides two meta-functions.  We simply verify that they
// return the proper type for a list of suitably chosen arguments.
//-----------------------------------------------------------------------------
// [ 1] bslmf_ForwardingType
// [ 2] bslmf_ConstForwardingType
// [ 3] USAGE EXAMPLE
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

#define ASSERT_SAME(X, Y) ASSERT(1 == (bslmf_IsSame<X, Y>::VALUE))

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum   Enum   {};
struct Struct {};
union  Union  {};
class  Class  {};

typedef int INT;

typedef void      F ();
typedef void ( & RF)();
typedef void (*  PF)();
typedef void (*&RPF)();

typedef void    Fi  (int);
typedef void (&RFi) (int);
typedef void (*PFi) (int);
typedef void    FRi (int&);
typedef void (&RFRi)(int&);

typedef char    A [5];
typedef char (&RA)[5];

//=============================================================================
//                     GLOBAL TYPES FOR USAGE EXAMPLE
//-----------------------------------------------------------------------------

struct MyType {};
typedef MyType& MyTypeRef;

void usageExample()
{

///Usage
///-----
// For example:
//..
    typedef int                    T1;
    typedef int&                   T2;
    typedef const volatile double& T3;
    typedef const double &         T4;
    typedef const float * &        T5;
    typedef const float * const &  T6;
    typedef MyType                 T7;
    typedef const MyType&          T8;
    typedef MyType&                T9;
    typedef MyType*                T10;

    typedef int                    EXP1;
    typedef int&                   EXP2;
    typedef const volatile double& EXP3;
    typedef double                 EXP4;
    typedef const float * &        EXP5;
    typedef const float *          EXP6;
    typedef const MyType&          EXP7;
    typedef const MyType&          EXP8;
    typedef MyType&                EXP9;
    typedef MyType*                EXP10;

    ASSERT(1 == (bslmf_IsSame<bslmf_ForwardingType<T1>::Type,  EXP1>::VALUE));
    ASSERT(1 == (bslmf_IsSame<bslmf_ForwardingType<T2>::Type,  EXP2>::VALUE));
    ASSERT(1 == (bslmf_IsSame<bslmf_ForwardingType<T3>::Type,  EXP3>::VALUE));
    ASSERT(1 == (bslmf_IsSame<bslmf_ForwardingType<T4>::Type,  EXP4>::VALUE));
    ASSERT(1 == (bslmf_IsSame<bslmf_ForwardingType<T5>::Type,  EXP5>::VALUE));
    ASSERT(1 == (bslmf_IsSame<bslmf_ForwardingType<T6>::Type,  EXP6>::VALUE));
    ASSERT(1 == (bslmf_IsSame<bslmf_ForwardingType<T7>::Type,  EXP7>::VALUE));
    ASSERT(1 == (bslmf_IsSame<bslmf_ForwardingType<T8>::Type,  EXP8>::VALUE));
    ASSERT(1 == (bslmf_IsSame<bslmf_ForwardingType<T9>::Type,  EXP9>::VALUE));
    ASSERT(1 == (bslmf_IsSame<bslmf_ForwardingType<T10>::Type, EXP10>::VALUE));
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    (void) verbose;      // eliminate unused variable warning
    (void) veryVerbose;  // eliminate unused variable warning

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Simple example illustrating use of 'bslmf_RemoveReference'.
        //
        // Concerns:
        //
        // Plan:
        //
        // Tactics:
        //   - Add-Hoc Data Selection Method
        //   - Brute-Force implementation technique
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

        usageExample();

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // Test Plan:
        //   Instantiate 'bslmf_ConstForwardingType' with various types and
        //   verify that its 'Type' typedef is set to the expected type.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "bslmf_ConstForwardingType" << endl
                          << "=========================" << endl;

        ASSERT_SAME(bslmf_ConstForwardingType<int       >::Type, int);
        ASSERT_SAME(bslmf_ConstForwardingType<int&      >::Type, int&);
        if (verbose) P(bslmf_ConstForwardingType<int&>::BSLMF_FORWARDING_TYPE__ID);
        ASSERT_SAME(bslmf_ConstForwardingType<int const&>::Type, int);

        ASSERT_SAME(bslmf_ConstForwardingType<void *>::Type, void *);
        if (verbose) P(bslmf_ConstForwardingType<void *>::BSLMF_FORWARDING_TYPE__ID);
        ASSERT_SAME(bslmf_ConstForwardingType<void *&        >::Type, void *&);
        ASSERT_SAME(bslmf_ConstForwardingType<void volatile *&>::Type,
                    volatile void *&);

        ASSERT_SAME(bslmf_ConstForwardingType<char const *const&>::Type,
                                         char const *);

        ASSERT_SAME(bslmf_ConstForwardingType<Enum    >::Type, Enum);
        ASSERT_SAME(bslmf_ConstForwardingType<Enum&   >::Type, Enum&);
        ASSERT_SAME(bslmf_ConstForwardingType<Struct  >::Type, const Struct&);
        ASSERT_SAME(bslmf_ConstForwardingType<Struct& >::Type, Struct&);
        ASSERT_SAME(bslmf_ConstForwardingType<Union   >::Type, const Union&);
        ASSERT_SAME(bslmf_ConstForwardingType<Union&  >::Type, Union&);
        ASSERT(0 == bslmf_IsArray<Class>::VALUE);
        ASSERT_SAME(bslmf_ConstForwardingType<Class   >::Type,const Class&);
        if (verbose) P(bslmf_ConstForwardingType<Class>::BSLMF_FORWARDING_TYPE__ID);
        if (verbose) P(bslmf_ConstForwardingType<Class>::IS_BASIC_TYPE);
        ASSERT_SAME(bslmf_ConstForwardingType<const Class&>::Type,
                    const Class&);

        ASSERT_SAME(bslmf_ConstForwardingType<INT >::Type, int);
        ASSERT_SAME(bslmf_ConstForwardingType<INT&>::Type, int&);

        ASSERT_SAME(bslmf_ConstForwardingType<int Class::* >::Type,
                    int Class::*);
        ASSERT_SAME(bslmf_ConstForwardingType<int Class::* const& >::Type,
                    int Class::*);

        ASSERT_SAME(bslmf_ConstForwardingType<int Class::*&>::Type,
                    int Class::*&);

        ASSERT_SAME(bslmf_ConstForwardingType< PF>::Type, PF);
        ASSERT_SAME(bslmf_ConstForwardingType<RPF>::Type, PF&);

        ASSERT_SAME(bslmf_ConstForwardingType< A>::Type, const char*);
        ASSERT_SAME(bslmf_ConstForwardingType<RA>::Type, char*);
        if (verbose) P(bslmf_ConstForwardingType<RA>::BSLMF_FORWARDING_TYPE__ID);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // Test Plan:
        //   Instantiate 'bslmf_ForwardingType' with various types and verify
        //   that its 'Type' typedef is set to the expected type.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "bslmf_ForwardingType" << endl
                          << "=====================" << endl;

        ASSERT_SAME(bslmf_ForwardingType<int       >::Type, int);
        if (verbose) P(typeid(bslmf_ForwardingType<int>::Type).name());
        ASSERT_SAME(bslmf_ForwardingType<int&      >::Type, int&);
        ASSERT_SAME(bslmf_ForwardingType<int const&>::Type, int);

        ASSERT_SAME(bslmf_ForwardingType<void *          >::Type, void *);
        ASSERT_SAME(bslmf_ForwardingType<void *&         >::Type, void *&);
        ASSERT_SAME(bslmf_ForwardingType<void volatile *&>::Type,
                                          volatile void *&);

        ASSERT_SAME(bslmf_ForwardingType<char const *const&>::Type,
                                         char const *);

        ASSERT_SAME(bslmf_ForwardingType<Enum        >::Type, Enum);
        ASSERT_SAME(bslmf_ForwardingType<Enum&       >::Type, Enum&);
        ASSERT_SAME(bslmf_ForwardingType<Struct      >::Type, const Struct&);
        ASSERT_SAME(bslmf_ForwardingType<Struct&     >::Type, Struct&);
        ASSERT_SAME(bslmf_ForwardingType<Union       >::Type, const Union&);
        ASSERT_SAME(bslmf_ForwardingType<Union&      >::Type, Union&);
        ASSERT(0 == bslmf_IsArray<Class>::VALUE);
        ASSERT_SAME(bslmf_ForwardingType<Class       >::Type, const Class&);
        if (verbose) P(bslmf_ForwardingType<Class>::BSLMF_FORWARDING_TYPE__ID);
        if (verbose) P(bslmf_ForwardingType<Class>::IS_BASIC_TYPE);
        ASSERT_SAME(bslmf_ForwardingType<const Class&>::Type, const Class&);

        ASSERT_SAME(bslmf_ForwardingType<INT >::Type, int);
        ASSERT_SAME(bslmf_ForwardingType<INT&>::Type, int&);

        ASSERT_SAME(bslmf_ForwardingType<int Class::* >::Type, int Class::*);
        ASSERT_SAME(bslmf_ForwardingType<int Class::* const& >::Type,
                    int Class::*);

        ASSERT_SAME(bslmf_ForwardingType<int Class::*&>::Type, int Class::*&);

#if !defined(BSLS_PLATFORM__CMP_MSVC)    \
    && (!defined(BSLS_PLATFORM__CMP_IBM) \
        || (BSLS_PLATFORM__CMP_VER_MAJOR < 0x0800))
        // xlc-8 and MSVC 2005 seem to have problems with function types.

        ASSERT_SAME(bslmf_ForwardingType<  F>::Type,  F);
#endif
        ASSERT_SAME(bslmf_ForwardingType< RF>::Type,  RF);

        ASSERT_SAME(bslmf_ForwardingType< PF>::Type, PF);
        ASSERT_SAME(bslmf_ForwardingType<RPF>::Type, PF&);

#if !defined(BSLS_PLATFORM__CMP_MSVC)    \
    && (!defined(BSLS_PLATFORM__CMP_IBM) \
        || (BSLS_PLATFORM__CMP_VER_MAJOR < 0x0800))
        // xlc-8 and MSVC 2005 seem to have problems with function types.

        ASSERT_SAME(bslmf_ForwardingType< Fi >::Type, Fi);
        ASSERT_SAME(bslmf_ForwardingType< FRi>::Type, FRi);
#endif
        ASSERT_SAME(bslmf_ForwardingType<RFi >::Type, RFi);
        ASSERT_SAME(bslmf_ForwardingType<RFRi>::Type, FRi&);

        ASSERT_SAME(bslmf_ForwardingType< A>::Type, char*);
        ASSERT_SAME(bslmf_ForwardingType<RA>::Type, char *);
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
