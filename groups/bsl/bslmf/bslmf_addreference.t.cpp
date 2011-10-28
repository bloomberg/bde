// bslmf_addreference.t.cpp                                           -*-C++-*-

#include <bslmf_addreference.h>

#include <bslmf_issame.h>
#include <bsls_testutil.h>

#include <stdio.h>    // atoi()
#include <stdlib.h>    // atoi()

using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// TBD
//-----------------------------------------------------------------------------
// [ 1] bslmf_AddReference
// [ 2] USAGE EXAMPLE
//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

static int testStatus = 0;

static void aSsErT(bool b, const char *s, int i) {
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define ASSERT_SAME(X, Y) ASSERT((bslmf_IsSame<X, Y>::VALUE))

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
typedef void    FRi (int&);
typedef void (&RFRi)(int&);

typedef char    A [5];
typedef char (&RA)[5];

//=============================================================================
//                     GLOBAL TYPES FOR USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// For example:
//..
     struct MyType {};
     typedef MyType& MyTypeRef;

namespace USAGE_EXAMPLE_1 {

///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: A Simple Wrapper Class
///- - - - - - - - - - - - - - - - -
// First, let us write a simple class that can wrap any other type:
//..
    template <class TYPE>
    class Wrapper {
      private:
        // DATA
        TYPE d_data;

      public:
        // TYPES
        typedef typename bslmf_AddReference<TYPE>::Type WrappedType;

        // CREATORS
        Wrapper(TYPE value) : d_data(value) {}
            // Create a 'Wrapper' object having the specified 'value'.

        //! ~Wrapper() = default;
            // Destroy this object.
//..
// Then, we would like to expose access to the wrapped element through a
// method that returns a reference to the data member 'd_data'.  However,
// there would be a problem if the user supplied a parameterized type 'TYPE'
// that is a reference type, as references-to-references were not permitted by
// the language (prior the C++11 standard).  We can resolve such problems
// using the meta-function 'bslmf_AddReference'.
//..
        // MANIPULATORS
        typename bslmf_AddReference<TYPE>::Type value()
        {
            return d_data;
        }
//..
// Next, we supply an accessor function, 'value', that similarly wraps the
// parameterized type 'TYPE' with the 'bslmf_AddReference' meta-function.
// In this case we must remember to const-quality 'TYPE' before passing it
// on to the meta-function.
//..
        // ACCESSORS
        typename bslmf_AddReference<const TYPE>::Type value() const
        {
            return d_data;
        }
    };
//..
// Now, we write a test function, 'runTest', to verify our simple wrapper
// type.  We start by wrapping a simple 'int' value:
//..
    void runTests()
    {
        int i = 42;

        Wrapper<int> ti(i);  const Wrapper<int>& TI = ti;
        ASSERT(42 == i);
        ASSERT(42 == TI.value());

        ti.value() = 13;
        ASSERT(42 == i);
        ASSERT(13 == TI.value());
//..
// Finally, we test 'Wrapper' with a reference type:
//..
        Wrapper<int&> tr(i);  const Wrapper<int&>& TR = tr;
        ASSERT(42 == i);
        ASSERT(42 == TR.value());

        tr.value() = 13;
        ASSERT(13 == i);
        ASSERT(13 == TR.value());

        i = 42;
        ASSERT(42 == i);
        ASSERT(42 == TR.value());
    }
//..
}  // close namespace USAGE_EXAMPLE_1

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    (void) veryVerbose;      // eliminate unused variable warning

    setbuf(stdout, 0);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Simple example illustrating use of 'bslmf_AddReference'.
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

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");
//..
        ASSERT_SAME(bslmf_AddReference<int >::Type, int&);
        ASSERT_SAME(bslmf_AddReference<int&>::Type, int&);
        ASSERT_SAME(bslmf_AddReference<int volatile >::Type, volatile int&);
        ASSERT_SAME(bslmf_AddReference<int volatile&>::Type, volatile int&);

        ASSERT_SAME(bslmf_AddReference<MyType >::Type, MyType&);
        ASSERT_SAME(bslmf_AddReference<MyType&>::Type, MyType&);
        ASSERT_SAME(bslmf_AddReference<MyTypeRef>::Type, MyType&);
        ASSERT_SAME(bslmf_AddReference<MyType const >::Type, const MyType&);
        ASSERT_SAME(bslmf_AddReference<MyType const&>::Type, const MyType&);
        ASSERT_SAME(bslmf_AddReference<const MyTypeRef>::Type, MyType&);
//      ASSERT_SAME(bslmf_AddReference<const MyTypeRef&>::Type, MyType&); C++11

        ASSERT_SAME(bslmf_AddReference<void  >::Type, void);
        ASSERT_SAME(bslmf_AddReference<void *>::Type, void *&);
//..

      } break;
       case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Simple example illustrating use of 'bslmf_AddReference'.
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

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        USAGE_EXAMPLE_1::runTests();
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // Test Plan:
        //   Instantiate 'bslmf_AddReference' with various types and verify
        //   that its 'Type' typedef is set to a non-reference type.
        // --------------------------------------------------------------------

        if (verbose) printf("\nbslmf_AddReference"
                            "\n==================\n");

        ASSERT_SAME(bslmf_AddReference<int       >::Type, int&);
        ASSERT_SAME(bslmf_AddReference<int&      >::Type, int&);
        ASSERT_SAME(bslmf_AddReference<int const >::Type, const int&);
        ASSERT_SAME(bslmf_AddReference<int const&>::Type, const int&);

        ASSERT_SAME(bslmf_AddReference<void *          >::Type, void *&);
        ASSERT_SAME(bslmf_AddReference<void *&         >::Type, void *&);
        ASSERT_SAME(bslmf_AddReference<void volatile *&>::Type,
                                       volatile void *&);
        ASSERT_SAME(bslmf_AddReference<char const *const&>::Type,
                                       const char *const&);

        ASSERT_SAME(bslmf_AddReference<Enum        >::Type, Enum&);
        ASSERT_SAME(bslmf_AddReference<Enum&       >::Type, Enum&);
        ASSERT_SAME(bslmf_AddReference<Struct      >::Type, Struct&);
        ASSERT_SAME(bslmf_AddReference<Struct&     >::Type, Struct&);
        ASSERT_SAME(bslmf_AddReference<Union       >::Type, Union&);
        ASSERT_SAME(bslmf_AddReference<Union&      >::Type, Union&);
        ASSERT_SAME(bslmf_AddReference<Class       >::Type, Class&);
        ASSERT_SAME(bslmf_AddReference<const Class&>::Type, const Class&);

        ASSERT_SAME(bslmf_AddReference<INT >::Type, int&);
        ASSERT_SAME(bslmf_AddReference<INT&>::Type, int&);

        ASSERT_SAME(bslmf_AddReference<int Class::* >::Type, int Class::*&);
        ASSERT_SAME(bslmf_AddReference<int Class::*&>::Type, int Class::*&);

        ASSERT_SAME(bslmf_AddReference<  F>::Type,  F&);
        ASSERT_SAME(bslmf_AddReference< RF>::Type,  F&);
        ASSERT_SAME(bslmf_AddReference< PF>::Type, PF&);
        ASSERT_SAME(bslmf_AddReference<RPF>::Type, PF&);

        ASSERT_SAME(bslmf_AddReference< Fi >::Type, Fi&);
        ASSERT_SAME(bslmf_AddReference<RFi >::Type, Fi&);
        ASSERT_SAME(bslmf_AddReference< FRi>::Type, FRi&);
        ASSERT_SAME(bslmf_AddReference<RFRi>::Type, FRi&);

        ASSERT_SAME(bslmf_AddReference< A>::Type, A&);
        ASSERT_SAME(bslmf_AddReference<RA>::Type, A&);

        ASSERT_SAME(bslmf_AddReference<void         >::Type, void);
        ASSERT_SAME(bslmf_AddReference<void const   >::Type, const void);
        ASSERT_SAME(bslmf_AddReference<void volatile>::Type, volatile void);
        ASSERT_SAME(bslmf_AddReference<void const volatile>::Type,
                                       const volatile void);
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
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
