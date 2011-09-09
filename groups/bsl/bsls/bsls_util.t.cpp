// bsls_util.t.cpp                                                    -*-C++-*-

#include <bsls_util.h>

#include <stdio.h>
#include <stdlib.h>

#include <new>   // required to support use of "placement new"

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// 'bsls_Util' is a very simple class consisting of only one utility function.
// The main concerns are that the function compiles and returns a pointer to an
// object even if that object's class has an overloaded 'operator&' and that
// the resulting pointer has the correct 'const' and/or 'volatile'
// qualifications.
//-----------------------------------------------------------------------------
// [3] TYPE *bsls_Util::addressOf(TYPE&);
// [4] BSLS_UTIL_ADDRESSOF macro
//-----------------------------------------------------------------------------
// [1] BREATHING TEST
// [2] TEST APPARATUS
// [5] USAGE EXAMPLE
//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {

int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
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
#define T_ putchar('\t');                  // Print a tab (w/o newline)
#define L_ __LINE__                        // current Line number

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

class TestType
{
    int        d_dummy;
    static int d_bogus;

  public:
    explicit TestType(int = 0) { }

    static TestType *bogusPtr()
        { return reinterpret_cast<TestType*>(&d_bogus); }

    TestType                *operator&()                { return bogusPtr(); }
    TestType const          *operator&() const          { return bogusPtr(); }
    TestType volatile       *operator&() volatile       { return bogusPtr(); }
    TestType const volatile *operator&() const volatile { return bogusPtr(); }
        // Overload operator& to return something other than the address of
        // this object.

    TestType                *realAddress()                { return this; }
    TestType const          *realAddress() const          { return this; }
    TestType volatile       *realAddress() volatile       { return this; }
    TestType const volatile *realAddress() const volatile { return this; }
        // Return the actual address of this object.
};

int TestType::d_bogus = 0;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

enum CvQualification {
    CVQ_UNQUALIFIED,
    CVQ_CONST,
    CVQ_VOLATILE,
    CVQ_CONST_VOLATILE
};

template <typename T>
inline
CvQualification cvqOfPtr(T *p) { return CVQ_UNQUALIFIED; }

template <typename T>
inline
CvQualification cvqOfPtr(const T *p) { return CVQ_CONST; }

template <typename T>
inline
CvQualification cvqOfPtr(volatile T *p) { return CVQ_VOLATILE; }

template <typename T>
inline
CvQualification cvqOfPtr(const volatile T *p) { return CVQ_CONST_VOLATILE; }

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Obtain the address of a 'class' that defines 'operator&'.
// There are times, especially within low-level library functions, where it is
// necessary to obtain the address of an object even if that object's class
// overloads 'operator&' to return something other than the object's address.
//
// Assume we have a special reference-like type that can refer to a single bit:
//..
    class BitReference {
        char *d_byteptr_p;
        int   d_bitpos;

      public:
        BitReference(char *byteptr = 0, int bitpos = 0)
            : d_byteptr(byteptr), d_bitpos(bitpos) { }

        operator bool() const { return (*d_byteptr >> d_bitpos) & 1; }

        char *byteptr() const { return d_byteptr; }
        int bitpos() const { return d_bitpos; }
    };
//..
// and a pointer-like type that can point to a single bit:
//..
    class BitPointer {
        char *d_byteptr_p;
        int   d_bitpos;

      public:
        BitPointer(char *byteptr = 0, int bitpos = 0)
            : d_byteptr(byteptr), d_bitpos(bitpos) { }

        BitReference operator*() const
            { return BitReference(d_byteptr, d_bitpos); }

        // etc.
    };
//..
// To complete the picture, we overload 'operator&' for 'BitReference' to
// return a 'BitPointer' instead of a raw pointer:
//..
    inline BitPointer operator&(const BitReference& ref) {
        return BitPointer(ref.byteptr(), ref.bitpos());
    }
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose = argc > 2;
    bool veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
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

        if (verbose) printf("\nTesting USAGE EXAMPLE"
                            "\n=====================\n");

// However, there are times when it might be desirable to get the true
// address of a 'BitReference'.  Since the above overload prevents
// the obvious syntax from working, we use 'bsls_Util::addressOf' to accomplish
// this task.
//
// First, we create a 'BitReference' object:
//..
    char c[4];
    BitReference br(c, 3);
//..
// Now, we invoke 'bsls_Util::addressOf' to obtain and save the address of
// 'br':
//..
    BitReference *p = bsls_Util::addressOf(br);  // OK
    // BitReference *p = &br;                    // Won't compile
//..
// Notice that the commented line illustrates canonical use of 'operator&' that
// would not compile in this example.
//
// Finally, we verify that address obtained is the correct one, running some
// sanity checks:
//..
    ASSERT(0 != p);
    ASSERT(c == p->byteptr());
    ASSERT(3 == p->bitpos());
//..
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TEST: bsls_Util::addressOf
        //
        // Concerns:
        //: 1 The macro applies 'bsls_Util::addressOf' on Windows and
        //:   'operator&' on every other platform.
        //
        // Plan:
        //: 1 Create an object of type 'TestType' (see GLOBAL
        //:   TYPEDEFS/CONSTANTS FOR TESTING AND VARIABLES) with an overloaded
        //:   'operator&' and verify that 'bsls_Util::addressOf' invokes
        //:   'operator&' on UNIX, and that on Windows it returns the address
        //:    of the created object instead.
        //
        // Testing:
        //   bsls_Util::addressOf(X)
        // --------------------------------------------------------------------

        if (verbose) printf("TEST: bsls_Util::addressOf\n"
                            "==========================");

        TestType mX; const TestType& X = mX;

#ifndef BDE_USE_ADDRESSOF
        if (verbose)
              printf("\nTest that 'bsls_Util::addressOf' return 'bogusPtr'\n");
        ASSERT(X.bogusPtr() == BSLS_UTIL_ADDRESSOF(X));
#else
        if (verbose)
              printf("\nTest that 'bsls_Util::addressOf' return '&X'\n");
        ASSERT(X.realAddress() == BSLS_UTIL_ADDRESSOF(X));
#endif

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // FULL TEST
        //
        // Concerns:
        //: 1 Calling 'bsls_Util::addressOf' on an object will return the
        //:   address of the object whether or not the object's class has an
        //:   overloaded 'operator&'.
        //:
        //: 2 The returned pointer has the same cv-qualification as the
        //:   argument.
        //:
        //: 3 The result of calling addressof on a reference has the same
        //:   effect as on an (lvalue) object.
        //
        // Plan:
        //: 1 Create a number of objects of types 'int' and 'TestType', where
        //:   'TestType' has an overloaded 'operator&'.  Test that the pointer
        //:   returned from calling 'addressOf' on each points to that object.
        //:   (In the case of 'TestType', the 'realAddress' method returns the
        //:   true address of the object. (C-1, 3)
        //:
        //: 2 Using objects and references with different cv qualifications,
        //:   verify that the pointer returned by 'addressOf' has the correct
        //:   qualification (using the 'cvqOfPtr' function). (C-2)
        //
        // Testing:
        //   TYPE *bsls_Util::addressOf(TYPE&);
        // --------------------------------------------------------------------

        if (verbose) printf("\nFULL TEST"
                            "\n=========\n");

        int                i   = 0;
        int const          ci  = 0;
        int       volatile vi  = 0;
        int const volatile cvi = 0;

        ASSERT(&i   == bsls_Util::addressOf(i));
        ASSERT(&ci  == bsls_Util::addressOf(ci));
        ASSERT(&vi  == bsls_Util::addressOf(vi));
        ASSERT(&cvi == bsls_Util::addressOf(cvi));

        TestType                x(0);
        TestType const          cx(0);
        TestType       volatile vx(0);
        TestType const volatile cvx(0);

        ASSERT(x.realAddress()   == bsls_Util::addressOf(x));
        ASSERT(cx.realAddress()  == bsls_Util::addressOf(cx));
        ASSERT(vx.realAddress()  == bsls_Util::addressOf(vx));
        ASSERT(cvx.realAddress() == bsls_Util::addressOf(cvx));

        ASSERT(CVQ_UNQUALIFIED    == cvqOfPtr(bsls_Util::addressOf(x)));
        ASSERT(CVQ_CONST          == cvqOfPtr(bsls_Util::addressOf(cx)));
        ASSERT(CVQ_VOLATILE       == cvqOfPtr(bsls_Util::addressOf(vx)));
        ASSERT(CVQ_CONST_VOLATILE == cvqOfPtr(bsls_Util::addressOf(cvx)));


        int               & ri   = i;
        int const         & cri  = i;
        int       volatile& vri  = i;
        int const volatile& cvri = i;

        ASSERT(&i == bsls_Util::addressOf(ri));
        ASSERT(&i == bsls_Util::addressOf(cri));
        ASSERT(&i == bsls_Util::addressOf(vri));
        ASSERT(&i == bsls_Util::addressOf(cvri));

        TestType               & rx   = x;
        TestType const         & crx  = x;
        TestType       volatile& vrx  = x;
        TestType const volatile& cvrx = x;

        ASSERT(x.realAddress() == bsls_Util::addressOf(rx));
        ASSERT(x.realAddress() == bsls_Util::addressOf(crx));
        ASSERT(x.realAddress() == bsls_Util::addressOf(vrx));
        ASSERT(x.realAddress() == bsls_Util::addressOf(cvrx));

        ASSERT(CVQ_UNQUALIFIED    == cvqOfPtr(bsls_Util::addressOf(rx)));
        ASSERT(CVQ_CONST          == cvqOfPtr(bsls_Util::addressOf(crx)));
        ASSERT(CVQ_VOLATILE       == cvqOfPtr(bsls_Util::addressOf(vrx)));
        ASSERT(CVQ_CONST_VOLATILE == cvqOfPtr(bsls_Util::addressOf(cvrx)));
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING TEST APPARATUS
        //
        // Concerns:
        //: 1 That 'cvqOfPtr' returns the correct result for each
        //:   pointer-to-cv-qualified type.
        //:
        //: 2 That 'realAddress' returns the true address of a 'TestType'
        //:   object.
        //:
        //: 3 That 'TestType' overloads 'operator&' such that it returns
        //:   something other than the address of the object.
        //
        // Test Plan:
        //: 1 Call 'cvqOfPtr' with pointers of different cv-qualification and
        //:   verify the return value. (C-1)
        //:
        //: 2 Construct a 'TestType' object at a known address, by using an
        //:   array of sufficient 'char's.  Initialize a reference of each
        //:   cv-qualification type referring to this object, and verify that
        //:   calling 'realAddress' returns the same address as the backing
        //:   array. (C-2)
        //:
        //: 3 Call 'operator&' on each reference above and verify that they
        //:   never return the same result as the 'realAddress' method. (C-3)
        //
        // Testing:
        //   CvQualification cvqOfPtr(T *p);
        //   TestType *TestType::operator&();
        //   TestType *TestType::realAddress();
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING TEST APPARATUS"
                            "\n======================\n");

        if (verbose) printf("\nTesting cvqOfPtr\n");

        int dummy = 7;

        void                *p   = &dummy;
        void const          *cp  = &dummy;
        int        volatile *vp  = &dummy;
        int  const volatile *cvp = &dummy;

        ASSERT(CVQ_UNQUALIFIED    == cvqOfPtr(p));
        ASSERT(CVQ_CONST          == cvqOfPtr(cp));
        ASSERT(CVQ_VOLATILE       == cvqOfPtr(vp));
        ASSERT(CVQ_CONST_VOLATILE == cvqOfPtr(cvp));

        if (verbose) printf("\nTesting TestType::realAddress()\n");

        char buffer[sizeof(TestType)];
        void * const trueAddress = buffer;

        TestType *pX = new(trueAddress)TestType(0);

        TestType&                x    = *pX;
        const TestType&          cx   = *pX;
        volatile TestType&       vx   = *pX;
        const volatile TestType& cvx  = *pX;

        ASSERT(trueAddress == x.realAddress());
        ASSERT(trueAddress == cx.realAddress());
        ASSERT(trueAddress == vx.realAddress());
        ASSERT(trueAddress == cvx.realAddress());

        if (verbose) printf("\nTesting operator&(TestType)\n");

        ASSERT(&x   != x.realAddress());
        ASSERT(&cx  != cx.realAddress());
        ASSERT(&vx  != vx.realAddress());
        ASSERT(&cvx != cvx.realAddress());
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Plan:
        //   We want to exercise basic functionality by using 'addressOf' on
        //   both a "normal" type ('int') and a type for which 'operator&' is
        //   overloaded ('TestType').  In both cases, we verify that the
        //   return value of 'addressOf' is the true address of the object.
        //
        // Testing:
        //   This "test" *exercises* basic functionality.
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING TEST APPARATUS"
                            "\n======================\n");

        double d;
        double *dp = bsls_Util::addressOf(d);
        ASSERT(&d == dp);

        const TestType x(0);
        const TestType *xp = bsls_Util::addressOf(x);
        ASSERT(x.realAddress() == xp);

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
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
