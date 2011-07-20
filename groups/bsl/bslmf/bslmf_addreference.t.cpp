// bslmf_addreference.t.cpp                                           -*-C++-*-

#include <bslmf_addreference.h>

#include <bslmf_issame.h>

#include <stdio.h>    // atoi()
#include <stdlib.h>    // atoi()

using namespace BloombergLP;
using namespace std;

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

# define LOOP_ASSERT(I,X) { \
    if (!(X)) { P_(I); aSsErT(!(X), #X, __LINE__); } }

# define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { P(I) P_(J);   \
                aSsErT(!(X), #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
    if (!(X)) { P(I) P(J) P_(K) \
                aSsErT(!(X), #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define Q(X) printf("<| " #X " |>\n");      // Quote identifier literally.
#define P(X) dbg_print(#X " = ", X, "\n");  // Print identifier and value.
#define P_(X) dbg_print(#X " = ", X, ", "); // P(X) without '\n'
#define L_ __LINE__                         // current Line number
#define T_ putchar('\t');                   // Print a tab (w/o newline)

#define ASSERT_SAME(X, Y) ASSERT((bslmf_IsSame<X, Y>::VALUE))

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

// Fundamental-type-specific print functions.
inline void dbg_print(bool b) { printf(b ? "true" : "false"); fflush(stdout); }
inline void dbg_print(char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(unsigned char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(signed char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(short val) { printf("%d", (int)val); fflush(stdout); }
inline void dbg_print(unsigned short val) {
    printf("%d", (int)val); fflush(stdout);
}
inline void dbg_print(int val) { printf("%d", val); fflush(stdout); }
inline void dbg_print(unsigned int val) { printf("%u", val); fflush(stdout); }
inline void dbg_print(long val) { printf("%ld", val); fflush(stdout); }
inline void dbg_print(unsigned long val) {
    printf("%lu", val); fflush(stdout);
}
inline void dbg_print(long long val) { printf("%lld", val); fflush(stdout); }
inline void dbg_print(unsigned long long val) {
    printf("%llu", val); fflush(stdout);
}
inline void dbg_print(float val) {
    printf("'%f'", (double)val); fflush(stdout);
}
inline void dbg_print(double val) { printf("'%f'", val); fflush(stdout); }
inline void dbg_print(long double val) {
    printf("'%Lf'", val); fflush(stdout);
}
inline void dbg_print(const char* s) { printf("\"%s\"", s); fflush(stdout); }
inline void dbg_print(char* s) { printf("\"%s\"", s); fflush(stdout); }
inline void dbg_print(void* p) { printf("%p", p); fflush(stdout); }

// Generic debug print function (3-arguments).
template <typename T>
void dbg_print(const char* s, const T& val, const char* nl) {
    printf("%s", s); dbg_print(val);
    printf("%s", nl);
    fflush(stdout);
}

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
//..

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
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
