// bdemf_switch.t.cpp                  -*-C++-*-

#include <bdemf_switch.h>

#include <bslmf_nil.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// The component under test is a meta-function returning a 'TYPE' as a function
// of its template arguments.  As such, the concerns are limited to the
// correctness of the return "value", which is established by using overload
// resolution as the mechanism to discriminate among types.
//
// [ 2] bdemf_SwitchN<SELECTOR,T0,. . .,T{N-1}>::Type
// [ 1] bdemf_Switch<SELECTOR,T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>::Type
//-----------------------------------------------------------------------------
// [ 1] FUNCTIONALITY TEST (CLASS 'bdemf_Switch')
// [ 2] CLASSES 'bdemf_SwitchN'

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        std::printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//--------------------------------------------------------------------------

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
// #define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) std::printf("<| " #X " |>\n");  // Quote identifier literally.
//#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ std::printf("\t");             // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

struct A { };
struct B { };
struct C { };
struct D { };
struct E { };
struct F { };
struct G { };
struct H { };
struct I { };
struct J { };

char f(A) { return 'A'; }
char f(B) { return 'B'; }
char f(C) { return 'C'; }
char f(D) { return 'D'; }
char f(E) { return 'E'; }
char f(F) { return 'F'; }
char f(G) { return 'G'; }
char f(H) { return 'H'; }
char f(I) { return 'I'; }
char f(J) { return 'J'; }
char f(bslmf_Nil) { return '0'; }

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

// Assume an external server API for storing and retrieving data:
//..
    class data_Server {
        // Dummy implementation of data server

        int d_data;

      public:
        void store(char  data) { d_data = data | 0Xefface00; }
        void store(short data) { d_data = data | 0Xdead0000; }
        void store(int   data) { d_data = data; }

        void retrieve(char  *data) { *data = d_data & 0x000000ff; }
        void retrieve(short *data) { *data = d_data & 0x0000ffff; }
        void retrieve(int   *data) { *data = d_data; }
    };
//..
// In our application, we need some very small (1, 2, and 4-byte),
// special-purpose string types, so we create the following 'ShortString'
// class template:
//..
    template <int LEN>
    class ShortString {
        // Store a short, fixed-length string.

        char d_buffer[LEN];

      public:
        ShortString(const char *s = "") { std::strncpy(d_buffer, s, LEN); }
            // Construct a 'ShortString' from a NTCS.

        void retrieve(data_Server *server);
            // Retrieve this string from a data server.

        void store(data_Server *server) const;
            // Store this string to a data server.

        char operator[](int n) const { return d_buffer[n]; }
            // Return the nth byte in this string.
    };

    template <int LEN>
    bool operator==(const ShortString<LEN>& lhs, const ShortString<LEN>& rhs)
        // Return true if a 'lhs' is equal to 'rhs'
    {
        return 0 == std::memcmp(&lhs, &rhs, LEN);
    }

    template <int LEN>
    bool operator==(const ShortString<LEN>& lhs, const char *rhs)
        // Return true if a 'ShortString' 'lhs' is equal to a NTCS 'rhs'.
    {
        int i;
        for (i = 0; LEN > i && lhs[i]; ++i) {
            if (lhs[i] != rhs[i]) {
                return false;
            }
        }

        return ('\0' == rhs[i]);
    }
//..
// We would like to store our short strings in the data server, but the data
// server only handles 'char', 'short' and 'int' types.  Since our strings fit
// into these simple types, we can transform 'ShortString' into these integral
// types when calling 'store' and 'retrieve', using 'bdemf_Switch' to choose
// which integral type to use for each 'ShortString' type:
//..
    template <int LEN>
    void ShortString<LEN>::retrieve(data_Server *server)
    {
        // 'transferType will be 'char' if 'LEN' is 1, 'short' if 'LEN' is 2,
        // and 'int' if 'LEN' 4.  Will choose 'void' and thus not compile if
        // 'LEN' is 0 or 3.
        typedef typename
           bdemf_Switch5<LEN, void, char, short, void, int>::Type transferType;

        transferType x = 0;
        server->retrieve(&x);
        std::memcpy(d_buffer, &x, LEN);
    }

    template <int LEN>
    void ShortString<LEN>::store(data_Server *server) const
    {
        // 'transferType will be 'char' if 'LEN' is 1, 'short' if 'LEN' is 2,
        // and 'int' if 'LEN' 4.  Will choose 'void' and thus not compile if
        // 'LEN' is 0 or 3.
        typedef typename
           bdemf_Switch5<LEN, void, char, short, void, int>::Type transferType;

        transferType x = 0;
        std::memcpy(&x, d_buffer, LEN);
        server->store(x);
    }
//..
// In our main program, we first assert our basic assumptions, then we
// store and retrieve strings using our 'ShortString' template.
//..
    int usageExample()
    {
        ASSERT(2 == sizeof(short));
        ASSERT(4 == sizeof(int));

        data_Server server;

        ShortString<1> a("A");
        ShortString<1> b("B");
        ASSERT(a == "A");
        ASSERT(b == "B");
        ASSERT(! (a == b));

        a.store(&server);
        b.retrieve(&server);
        ASSERT(a == "A");
        ASSERT(b == "A");
        ASSERT(a == b);

        ShortString<2> cd("CD");
        ShortString<2> ef("EF");
        ASSERT(cd == "CD");
        ASSERT(ef == "EF");
        ASSERT(! (cd == ef));

        cd.store(&server);
        ef.retrieve(&server);
        ASSERT(cd == "CD");
        ASSERT(ef == "CD");
        ASSERT(cd == ef);

        ShortString<4> ghij("GHIJ");
        ShortString<4> klmn("KLMN");
        ASSERT(ghij == "GHIJ");
        ASSERT(klmn == "KLMN");
        ASSERT(! (ghij == klmn));

        ghij.store(&server);
        klmn.retrieve(&server);
        ASSERT(ghij == "GHIJ");
        ASSERT(klmn == "GHIJ");
        ASSERT(ghij == klmn);

        return 0;
    }
//..
      
//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? std::atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    std::setbuf(stdout, 0);    // Use unbuffered output

    std::printf("TEST " __FILE__ " CASE %d\n", test);

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

        if (verbose) std::printf("\nUSAGE EXAMPLE"
                                 "\n=============\n");

        usageExample();

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CLASSES 'bdemf_SwitchN'
        // 
        // Concerns:
        //   Returns the right type for 'SELECTOR' values 0-N.
        //   Returns 'bslmf_Nil' for 'SELECTOR' < 0 or N < 'SELECTOR'
        //
        // Plan:  For each 'N' between 2 and 9:
        //   Instantiate with N different types each value in the range 0-(N-1)
        //       as 'SELECTOR'.  Call 'f()' on the result to verify the type.
        //   Instantiate with 'SELECTOR' out of range 0-(N-1) and verify that
        //       'f' returns '0'.
        //   Note that, compared to case 1, we do not worry about defaulted nor
        //   repeated template arguments.
        //
        // Testing:
        //     bdemf_SwitchN<SELECTOR,T0,. . .,T{N-1}>::Type
        // --------------------------------------------------------------------

        if (verbose) std::printf("\nFUNCTIONALITY TEST"
                                 "\n==================\n");

        if (verbose) std::printf("\tTesting 'bdemf_Switch2.\n");
        {
            if (verbose) std::printf("Testing SELECTOR in range\n");
            ASSERT('A' == f(bdemf_Switch2<0,A,B>::Type()));
            ASSERT('B' == f(bdemf_Switch2<1,A,B>::Type()));

            if (verbose) std::printf("Testing SELECTOR out of range\n");
            ASSERT('0' == f(bdemf_Switch2<2,A,B>::Type()));
            ASSERT('0' == f(bdemf_Switch2<10,A,B>::Type()));
            ASSERT('0' == f(bdemf_Switch2<-1,A,B>::Type()));
        }
        
        if (verbose) std::printf("\tTesting 'bdemf_Switch3.\n");
        {
            if (verbose) std::printf("Testing SELECTOR in range\n");
            ASSERT('A' == f(bdemf_Switch3<0,A,B,C>::Type()));
            ASSERT('B' == f(bdemf_Switch3<1,A,B,C>::Type()));
            ASSERT('C' == f(bdemf_Switch3<2,A,B,C>::Type()));

            if (verbose) std::printf("Testing SELECTOR out of range\n");
            ASSERT('0' == f(bdemf_Switch3<3,A,B,C>::Type()));
            ASSERT('0' == f(bdemf_Switch3<10,A,B,C>::Type()));
            ASSERT('0' == f(bdemf_Switch3<-1,A,B,C>::Type()));
        }
        
        if (verbose) std::printf("\tTesting 'bdemf_Switch4.\n");
        {
            if (verbose) std::printf("Testing SELECTOR in range\n");
            ASSERT('A' == f(bdemf_Switch4<0,A,B,C,D>::Type()));
            ASSERT('B' == f(bdemf_Switch4<1,A,B,C,D>::Type()));
            ASSERT('C' == f(bdemf_Switch4<2,A,B,C,D>::Type()));
            ASSERT('D' == f(bdemf_Switch4<3,A,B,C,D>::Type()));

            if (verbose) std::printf("Testing SELECTOR out of range\n");
            ASSERT('0' == f(bdemf_Switch4<4,A,B,C,D>::Type()));
            ASSERT('0' == f(bdemf_Switch4<10,A,B,C,D>::Type()));
            ASSERT('0' == f(bdemf_Switch4<-1,A,B,C,D>::Type()));
        }
        
        if (verbose) std::printf("\tTesting 'bdemf_Switch5.\n");
        {
            if (verbose) std::printf("Testing SELECTOR in range\n");
            ASSERT('A' == f(bdemf_Switch5<0,A,B,C,D,E>::Type()));
            ASSERT('B' == f(bdemf_Switch5<1,A,B,C,D,E>::Type()));
            ASSERT('C' == f(bdemf_Switch5<2,A,B,C,D,E>::Type()));
            ASSERT('D' == f(bdemf_Switch5<3,A,B,C,D,E>::Type()));
            ASSERT('E' == f(bdemf_Switch5<4,A,B,C,D,E>::Type()));

            if (verbose) std::printf("Testing SELECTOR out of range\n");
            ASSERT('0' == f(bdemf_Switch5<5,A,B,C,D,E>::Type()));
            ASSERT('0' == f(bdemf_Switch5<10,A,B,C,D,E>::Type()));
            ASSERT('0' == f(bdemf_Switch5<-1,A,B,C,D,E>::Type()));
        }
        
        if (verbose) std::printf("\tTesting 'bdemf_Switch6.\n");
        {
            if (verbose) std::printf("Testing SELECTOR in range\n");
            ASSERT('A' == f(bdemf_Switch6<0,A,B,C,D,E,F>::Type()));
            ASSERT('B' == f(bdemf_Switch6<1,A,B,C,D,E,F>::Type()));
            ASSERT('C' == f(bdemf_Switch6<2,A,B,C,D,E,F>::Type()));
            ASSERT('D' == f(bdemf_Switch6<3,A,B,C,D,E,F>::Type()));
            ASSERT('E' == f(bdemf_Switch6<4,A,B,C,D,E,F>::Type()));
            ASSERT('F' == f(bdemf_Switch6<5,A,B,C,D,E,F>::Type()));

            if (verbose) std::printf("Testing SELECTOR out of range\n");
            ASSERT('0' == f(bdemf_Switch6<6,A,B,C,D,E,F>::Type()));
            ASSERT('0' == f(bdemf_Switch6<10,A,B,C,D,E,F>::Type()));
            ASSERT('0' == f(bdemf_Switch6<-1,A,B,C,D,E,F>::Type()));
        }
        
        if (verbose) std::printf("\tTesting 'bdemf_Switch7.\n");
        {
            if (verbose) std::printf("Testing SELECTOR in range\n");
            ASSERT('A' == f(bdemf_Switch7<0,A,B,C,D,E,F,G>::Type()));
            ASSERT('B' == f(bdemf_Switch7<1,A,B,C,D,E,F,G>::Type()));
            ASSERT('C' == f(bdemf_Switch7<2,A,B,C,D,E,F,G>::Type()));
            ASSERT('D' == f(bdemf_Switch7<3,A,B,C,D,E,F,G>::Type()));
            ASSERT('E' == f(bdemf_Switch7<4,A,B,C,D,E,F,G>::Type()));
            ASSERT('F' == f(bdemf_Switch7<5,A,B,C,D,E,F,G>::Type()));
            ASSERT('G' == f(bdemf_Switch7<6,A,B,C,D,E,F,G>::Type()));

            if (verbose) std::printf("Testing SELECTOR out of range\n");
            ASSERT('0' == f(bdemf_Switch7<7,A,B,C,D,E,F,G>::Type()));
            ASSERT('0' == f(bdemf_Switch7<10,A,B,C,D,E,F,G>::Type()));
            ASSERT('0' == f(bdemf_Switch7<-1,A,B,C,D,E,F,G>::Type()));
        }
        
        if (verbose) std::printf("\tTesting 'bdemf_Switch8.\n");
        {
            if (verbose) std::printf("Testing SELECTOR in range\n");
            ASSERT('A' == f(bdemf_Switch8<0,A,B,C,D,E,F,G,H>::Type()));
            ASSERT('B' == f(bdemf_Switch8<1,A,B,C,D,E,F,G,H>::Type()));
            ASSERT('C' == f(bdemf_Switch8<2,A,B,C,D,E,F,G,H>::Type()));
            ASSERT('D' == f(bdemf_Switch8<3,A,B,C,D,E,F,G,H>::Type()));
            ASSERT('E' == f(bdemf_Switch8<4,A,B,C,D,E,F,G,H>::Type()));
            ASSERT('F' == f(bdemf_Switch8<5,A,B,C,D,E,F,G,H>::Type()));
            ASSERT('G' == f(bdemf_Switch8<6,A,B,C,D,E,F,G,H>::Type()));
            ASSERT('H' == f(bdemf_Switch8<7,A,B,C,D,E,F,G,H>::Type()));

            if (verbose) std::printf("Testing SELECTOR out of range\n");
            ASSERT('0' == f(bdemf_Switch8<8,A,B,C,D,E,F,G,H>::Type()));
            ASSERT('0' == f(bdemf_Switch8<10,A,B,C,D,E,F,G,H>::Type()));
            ASSERT('0' == f(bdemf_Switch8<-1,A,B,C,D,E,F,G,H>::Type()));
        }
        
        if (verbose) std::printf("\tTesting 'bdemf_Switch9.\n");
        {
            if (verbose) std::printf("Testing SELECTOR in range\n");
            ASSERT('A' == f(bdemf_Switch9<0,A,B,C,D,E,F,G,H,I>::Type()));
            ASSERT('B' == f(bdemf_Switch9<1,A,B,C,D,E,F,G,H,I>::Type()));
            ASSERT('C' == f(bdemf_Switch9<2,A,B,C,D,E,F,G,H,I>::Type()));
            ASSERT('D' == f(bdemf_Switch9<3,A,B,C,D,E,F,G,H,I>::Type()));
            ASSERT('E' == f(bdemf_Switch9<4,A,B,C,D,E,F,G,H,I>::Type()));
            ASSERT('F' == f(bdemf_Switch9<5,A,B,C,D,E,F,G,H,I>::Type()));
            ASSERT('G' == f(bdemf_Switch9<6,A,B,C,D,E,F,G,H,I>::Type()));
            ASSERT('H' == f(bdemf_Switch9<7,A,B,C,D,E,F,G,H,I>::Type()));
            ASSERT('I' == f(bdemf_Switch9<8,A,B,C,D,E,F,G,H,I>::Type()));

            if (verbose) std::printf("Testing SELECTOR out of range\n");
            ASSERT('0' == f(bdemf_Switch9<9,A,B,C,D,E,F,G,H,I>::Type()));
            ASSERT('0' == f(bdemf_Switch9<10,A,B,C,D,E,F,G,H,I>::Type()));
            ASSERT('0' == f(bdemf_Switch9<-1,A,B,C,D,E,F,G,H,I>::Type()));
        }
        
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // FUNCTIONALITY TEST
        // 
        // Concerns:
        //   Returns the right type for 'SELECTOR' values 0-4.
        //   Returns 'bslmf_Nil' for 'SELECTOR' < 0 or 4 < 'SELECTOR'
        //   Returns 'bslmf_Nil' if 'SELECTOR' > number of template arguments
        //
        // Plan:
        //   Define 10 overloaded versions of a function, 'f' returning
        //       a different letter for each of the types 'A' - 'J' and
        //       returning '0' for 'bslmf_Nil'.
        //   Instantiate with 10 different types each value in the range 0-9 as
        //       'SELECTOR'.  Call 'f()' on the result to verify the type.
        //   Instantiate with 'SELECTOR' out of range 0-9 and verify that 'f'
        //       returns '0'.
        //   Instantiate with fewer than 9 different types and verify results.
        //
        // Testing:
        //     bdemf_Switch<SELECTOR,T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>::Type
        // --------------------------------------------------------------------

        if (verbose) std::printf("\nFUNCTIONALITY TEST"
                                 "\n==================\n");

        if (verbose) std::printf("Testing SELECTOR in range\n");
        ASSERT('A' == f(bdemf_Switch<0,A,B,C,D,E,F,G,H,I,J>::Type()));
        ASSERT('B' == f(bdemf_Switch<1,A,B,C,D,E,F,G,H,I,J>::Type()));
        ASSERT('C' == f(bdemf_Switch<2,A,B,C,D,E,F,G,H,I,J>::Type()));
        ASSERT('D' == f(bdemf_Switch<3,A,B,C,D,E,F,G,H,I,J>::Type()));
        ASSERT('E' == f(bdemf_Switch<4,A,B,C,D,E,F,G,H,I,J>::Type()));
        ASSERT('F' == f(bdemf_Switch<5,A,B,C,D,E,F,G,H,I,J>::Type()));
        ASSERT('G' == f(bdemf_Switch<6,A,B,C,D,E,F,G,H,I,J>::Type()));
        ASSERT('H' == f(bdemf_Switch<7,A,B,C,D,E,F,G,H,I,J>::Type()));
        ASSERT('I' == f(bdemf_Switch<8,A,B,C,D,E,F,G,H,I,J>::Type()));
        ASSERT('J' == f(bdemf_Switch<9,A,B,C,D,E,F,G,H,I,J>::Type()));

        if (verbose) std::printf("Testing SELECTOR out of range\n");
        ASSERT('0' == f(bdemf_Switch<10,A,B,C,D,E,F,G,H,I,J>::Type()));
        ASSERT('0' == f(bdemf_Switch<100,A,B,C,D,E,F,G,H,I,J>::Type()));
        ASSERT('0' == f(bdemf_Switch<-1,A,B,C,D,E,F,G,H,I,J>::Type()));

        if (verbose) std::printf("Testing Default arguments\n");
        ASSERT('A' == f(bdemf_Switch<0,A>::Type()));
        ASSERT('0' == f(bdemf_Switch<1,A>::Type()));
        ASSERT('0' == f(bdemf_Switch<2,A>::Type()));
        ASSERT('0' == f(bdemf_Switch<3,A>::Type()));
        ASSERT('0' == f(bdemf_Switch<4,A>::Type()));
        ASSERT('0' == f(bdemf_Switch<5,A>::Type()));
        ASSERT('0' == f(bdemf_Switch<6,A>::Type()));
        ASSERT('0' == f(bdemf_Switch<7,A>::Type()));
        ASSERT('0' == f(bdemf_Switch<8,A>::Type()));
        ASSERT('0' == f(bdemf_Switch<10,A>::Type()));
        ASSERT('0' == f(bdemf_Switch<-1,A>::Type()));
        ASSERT('B' == f(bdemf_Switch<1,A,B>::Type()));
        ASSERT('0' == f(bdemf_Switch<2,A,B>::Type()));

        if (verbose) std::printf("Testing Repeated arguments\n");
        ASSERT('A' == f(bdemf_Switch<0,A,B,A,B,A,C,C,C,C>::Type()));
        ASSERT('B' == f(bdemf_Switch<1,A,B,A,B,A,C,C,C,C>::Type()));
        ASSERT('A' == f(bdemf_Switch<2,A,B,A,B,A,C,C,C,C>::Type()));
        ASSERT('B' == f(bdemf_Switch<3,A,B,A,B,A,C,C,C,C>::Type()));
        ASSERT('A' == f(bdemf_Switch<4,A,B,A,B,A,C,C,C,C>::Type()));
        ASSERT('C' == f(bdemf_Switch<5,A,B,A,B,A,C,C,C,C>::Type()));
        ASSERT('C' == f(bdemf_Switch<6,A,B,A,B,A,C,C,C,C>::Type()));
        ASSERT('C' == f(bdemf_Switch<7,A,B,A,B,A,C,C,C,C>::Type()));
        ASSERT('C' == f(bdemf_Switch<8,A,B,A,B,A,C,C,C,C>::Type()));
        ASSERT('0' == f(bdemf_Switch<9,A,B,A,B,A,C,C,C,C>::Type()));
        
      } break;

      default: {
        std::fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        std::fprintf(stderr, "Error, non-zero test status = %d.\n",
                     testStatus);
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
