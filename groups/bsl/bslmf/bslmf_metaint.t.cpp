// bslmf_metaint.t.cpp                                                -*-C++-*-

#include <bslmf_metaint.h>

#include <cstdlib>      // atoi()
#include <cstring>      // strcmp()
#include <cstdio>

using namespace BloombergLP;
using namespace std;
using namespace bslmf;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] USAGE EXAMPLE
//=============================================================================

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {

int verbose = 0;
int veryVerbose = 0;
int veryVeryVerbose = 0;
int testStatus = 0;

void aSsErT(int c, const char *s, int i) {
    if (c) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
// NOTE: This implementation of LOOP_ASSERT macros must use printf since
//       cout uses new and must not be called during exception testing.

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\t"); \
                printf("%s", #J ": "); dbg_print(J); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) {                    \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\t"); \
                printf("%s", #J ": "); dbg_print(J); printf("\t"); \
                printf("%s", #K ": "); dbg_print(K); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) {                  \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\t"); \
                printf("%s", #J ": "); dbg_print(J); printf("\t"); \
                printf("%s", #K ": "); dbg_print(K); printf("\t"); \
                printf("%s", #L ": "); dbg_print(L); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) {                \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\t"); \
                printf("%s", #J ": "); dbg_print(J); printf("\t"); \
                printf("%s", #K ": "); dbg_print(K); printf("\t"); \
                printf("%s", #L ": "); dbg_print(L); printf("\t"); \
                printf("%s", #M ": "); dbg_print(M); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define Q(X) printf("<| " #X " |>\n");     // Quote identifier literally.
#define P(X) dbg_print(#X " = ", X, "\n")  // Print identifier and value.
#define P_(X) dbg_print(#X " = ", X, ", ") // P(X) without '\n'
#define L_ __LINE__                        // current Line number
#define T_ putchar('\t');                  // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

template <class TYPE, TYPE VALUE>
struct matchIntegerConstant
{
static bool test(bsltt::integer_constant<TYPE, VALUE>)
    // Return true when called with an 'integer_constant' of the specified
    // 'VALUE'.  Does not participate in overload resolution for
    // 'integer_constant's who's 'value' is not 'VALUE'.
{
    return true;
}

template <TYPE OTHER_VALUE>
static bool test(bsltt::integer_constant<TYPE, OTHER_VALUE>)
    // Return false.  Overload resolution will select this function only when
    // the argument is other than 'integer_constant<TYPE, VALUE>', i.e., when
    // the specified 'VALUE' is different from the specified 'OTHER_VALUE'.
{
    return false;
}
};

//=============================================================================
//                  CODE FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

///Example 1: Compile-Time Function Dispatching
/// - - - - - - - - - - - - - - - - - - - - - - 
// The most common use of this structure is to perform static function
// dispatching based on a compile-time calculation.  Often the calculation is
// nothing more than a simple predicate, allowing us to select one of two
// functions.  The following function, 'doSomething', uses a fast
// implementation (e.g., 'memcpy') if the parameterized type allows for such
// operations, otherwise it will use a more generic and slower implementation
// (e.g., copy constructor).
//..
    template <class T>
    void doSomethingImp(T *t, bslmf::MetaInt<0>)
    {
        // slow generic implementation
        (void) t;
        // ...
    }
  
    template <class T>
    void doSomethingImp(T *t, bslmf::MetaInt<1>)
    {
        // fast implementation (works only for some T's)
        (void) t;
        // ...
    }
  
    template <class T, bool IsFast>
    void doSomething(T *t)
    {
        doSomethingImp(t, bslmf::MetaInt<IsFast>());
    }
//..
// The power of this approach is that the compiler will compile only the
// implementation selected by the 'MetaInt' argument.  For some parameter
// types, the fast version of 'doSomethingImp' would be ill-formed.  This kind
// of compile-time dispatch prevents the ill-formed version from ever being
// instantiated.
//..
    int usageExample1()
    {
        int i;
        doSomething<int, true>(&i); // fast version selected for int
  
        double m;
        doSomething<double, false>(&m); // slow version selected for double
  
        return 0;
    }
//..
///Example 2: Reading the 'VALUE' member
/// - - - - - - - - - - - - - - - - - - 
// In addition to forming new types, the value of the integral paramameter to
// 'MetaInt' is "saved" in the enum member 'VALUE', and is accessible for use
// in compile-time or run-time operations.
//..
    template <int V>
    unsigned g()
    {
        bslmf::MetaInt<V> i;
        ASSERT(V == i.VALUE);
        ASSERT(V == bslmf::MetaInt<V>::VALUE);
        return bslmf::MetaInt<V>::VALUE;
    }
  
    int usageExample2()
    {
        int v = g<1>();
        ASSERT(1 == v);
        return 0;
    }
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting USAGE EXAMPLE"
                            "\n=====================\n");

        usageExample1();
        usageExample2();

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CONVERSION TO integer_constant
        //
        // Concerns:
        //: 1 'MetaInt<V>' is convertible to 'integer_constant<T, V>', where
        //:   'T' is an integer type.
        //:
        //: 2 'MetaInt<V>' is NOT convertible to 'integer_constant<T, X>',
        //:   where 'T' is an integer type and 'X != V'.
        //
        // Plan:
        //: 1 Create a function template, 'matchIntegerConstant<T, V>' having
        //:   two overloads: one that takes an argument of type
        //:   'integer_constant<T, V>' and returns 'true', and another which
        //:   takes an argument of *any* 'integer_constant' and returns
        //:   'false'.  For various integer types, 'T', and values 'V',
        //:   construct rvalues of type 'MetaInt<V>' and call
        //:   'matchIntegerConstant<T, V>', verifying that it returns 'true'.
        //:
        //: 2 For various integer types, 'T', and values 'V' and 'X' such that
        //:   'V != X', construct rvalues of type 'MetaInt<V>' and call
        //:   'matchIntegerConstant<T, X>', verifying that it returns 'false'.
        //
        // Testing:
        //     operator bsltt::integer_constant<TYPE, (TYPE)INT_VALUE>() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING CONVERSION TO integer_constant"
                            "\n======================================\n");

#if 0 // Not working yet
        if (veryVerbose) printf("Testing good conversions\n");
        ASSERT(  (matchIntegerConstant<bool, false>::test(MetaInt<false>())));
        ASSERT(  (matchIntegerConstant<int, 1>::test(MetaInt<1>())));
        ASSERT(  (matchIntegerConstant<unsigned, 2>::test(MetaInt<2>())));
        ASSERT(  (matchIntegerConstant<short, -3>::test(MetaInt<-3>())));
        ASSERT(  (matchIntegerConstant<long, 4>::test(MetaInt<4>())));
        ASSERT(  (matchIntegerConstant<unsigned char, 'A'>::test(MetaInt<'A'>())));

        if (veryVerbose) printf("Testing bad conversions\n");
        ASSERT(! (matchIntegerConstant<bool, false>::test(MetaInt<true>())));
        ASSERT(! (matchIntegerConstant<int, 1>::test(MetaInt<2>())));
        ASSERT(! (matchIntegerConstant<unsigned, 2>::test(MetaInt<4>())));
        ASSERT(! (matchIntegerConstant<short, -3>::test(MetaInt<-6>())));
        ASSERT(! (matchIntegerConstant<long, 4>::test(MetaInt<8>())));
        ASSERT(! (matchIntegerConstant<unsigned char, 'A'>::test(MetaInt<'Z'>())));
#endif

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // VALUE TEST
        //
        // Test Plan:
        //   Instantiate 'MetaInt' with various constant integral
        //   values and verify that their 'VALUE' member is initialized
        //   properly.
        // --------------------------------------------------------------------

        if (verbose) printf("\nVALUE TEST"
                            "\n==========\n");

        // verify that the 'VALUE' member is evaluated at compile-time
        enum {
            C1 = bslmf::MetaInt<1>::VALUE,
            C2 = bslmf::MetaInt<2>::VALUE,
            C0 = bslmf::MetaInt<0>::VALUE
        };

        ASSERT(0 == C0);
        ASSERT(1 == C1);
        ASSERT(2 == C2);

        bslmf::MetaInt<0> i0;
        bslmf::MetaInt<1> i1;
        bslmf::MetaInt<2> i2;

        ASSERT(0 == i0.VALUE);
        ASSERT(1 == i1.VALUE);
        ASSERT(2 == i2.VALUE);

        // Check inherited value
        ASSERT(0 == i0.value);
        ASSERT(1 == i1.value);
        ASSERT(2 == i2.value);

        ASSERT(-5 == bslmf::MetaInt<(unsigned)-5>::VALUE);

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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002-2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
