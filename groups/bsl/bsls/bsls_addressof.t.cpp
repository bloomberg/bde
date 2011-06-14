// bsls_addressof.t.cpp                                               -*-C++-*-

#include <bsls_addressof.h>

#include <cstdio>
#include <cstdlib>

using namespace std;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// 'bsls_addressof' is a very simple component consisting of only one
// function.  The main concerns are that the function compiles and returns a
// pointer to an object even if that object's class has an overloaded
// 'operator&' and that the resulting pointer has the correct 'const' and/or
// 'volatile' qualifications.
//-----------------------------------------------------------------------------
// FREE FUNCTIONS
// [3] TYPE *addressof(TYPE&);
//-----------------------------------------------------------------------------
// [1] BREATHING TEST
// [2] TEST APPARATUS
// [4] USAGE EXAMPLE
//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {

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

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

class TestType
{
    int        d_dummy;
    static int d_bogus;

public:
    TestType(int = 0) { }

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
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

// USAGE EXAMPLE

// Assume we have a special reference-like type that can refer to a single bit:
//..
    class BitReference {
        char *d_byteptr;
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
        char *d_byteptr;
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
// However, there are times when it might be desirable to get the true
// address of a 'BitReference'.  Unfortunately, the above overload prevents
// the obvious syntax from working:
//..
    int usageExample()
    {
        char c[4];
        BitReference br(c, 3);
        // BitReference *p = &br;  // Won't compile
//..
// The 'addressof' function was defined for exactly this purpose.  It returns
// the address of an object even of that object's class overloads 'operator&':
//..
        BitReference *p = bsl::addressof(br);  // OK
        ASSERT(0 != p);
        ASSERT(p->byteptr() == c);
        ASSERT(p->bitpos()  == 3);

        return 0;
    }
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //  1 The usage examples in the header file compile.
        //  2 The usage examples in the header produce correct results
        //
        // Test plan:
        //   Copy the usage examples from the header into this test driver.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting USAGE EXAMPLE"
                            "\n=====================\n");

        usageExample();

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // FULL TEST
        //
        // Concerns:
        //  1 Calling addressof() on an object will return the address of
        //    the object whether or not the object's class has an overloaded
        //    'operator&'.
        //  2 The returned pointer has the same cv-qualification as the
        //    argument.
        //  3 The result of calling addressof on a reference has the same
        //    effect as on an (lvalue) object.
        //
        // Plan:
        //   Create a number of objects of types 'int' and 'TestType', where
        //   'TestType' has an overloaded 'operator&'.  Test that the pointer
        //   returned from calling 'addressof' on each points to that
        //   object.  (In the case of 'TestType', the 'realAddress' method
        //   returns the true address of the object.  For concern 2, use
        //   objects and references with different cv qualifications and
        //   verify that the returned pointer has the correct qualification
        //   (using the 'cvqOfPtr' function).
        //
        // Testing:
        //   TYPE *bsl::addressof(TYPE&);
        // --------------------------------------------------------------------

        if (verbose) printf("\nFULL TEST"
                            "\n=========\n");

        int                i   = 0;
        int const          ci  = 0;
        int       volatile vi  = 0;
        int const volatile cvi = 0;

        ASSERT(&i   == addressof(i));
        ASSERT(&ci  == addressof(ci));
        ASSERT(&vi  == addressof(vi));
        ASSERT(&cvi == addressof(cvi));

        TestType                x(0);
        TestType const          cx(0);
        TestType       volatile vx(0);
        TestType const volatile cvx(0);

        ASSERT(x.realAddress()   == addressof(x));
        ASSERT(cx.realAddress()  == addressof(cx));
        ASSERT(vx.realAddress()  == addressof(vx));
        ASSERT(cvx.realAddress() == addressof(cvx));

        int               & ri   = i;
        int const         & cri  = i;
        int       volatile& vri  = i;
        int const volatile& cvri = i;

        ASSERT(&i == addressof(ri));
        ASSERT(&i == addressof(cri));
        ASSERT(&i == addressof(vri));
        ASSERT(&i == addressof(cvri));

        TestType               & rx   = x;
        TestType const         & crx  = x;
        TestType       volatile& vrx  = x;
        TestType const volatile& cvrx = x;

        ASSERT(x.realAddress() == addressof(rx));
        ASSERT(x.realAddress() == addressof(crx));
        ASSERT(x.realAddress() == addressof(vrx));
        ASSERT(x.realAddress() == addressof(cvrx));

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING TEST APPARATUS
        //
        // Concerns:
        //  1 That 'cvqOfPtr' returns the correct result for each
        //    pointer-to-cv-qualified type.
        //  2 That 'TestType' overloads 'operator&' such that it returns
        //    something other than the address of the object.
        //
        // Test Plan:
        //   Call 'cvqOfPtr' with pointers of different cv-qualification and
        //   verify the return value.  Call 'operator&' on objects of type
        //   'TestType' and verify that they never return the same result as
        //   the 'realAddress' method.
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

        if (verbose) printf("\nTesting TestType\n");

        TestType                x(0);
        TestType const          cx(0);
        TestType       volatile vx(0);
        TestType const volatile cvx(0);

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
        //   We want to exercise basic functionality by using 'addressof' on
        //   both a "normal" type ('int') and a type for which 'operator&' is
        //   overloaded ('TestType').  In both cases, we verify that the
        //   return value of 'addressof' is the true address of the object.
        //
        // Testing:
        //   This "test" *exercises* basic functionality.
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING TEST APPARATUS"
                            "\n======================\n");

        double d;
        double *dp = addressof(d);
        ASSERT(&d == dp);

        const TestType x(0);
        const TestType *xp = addressof(x);
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
