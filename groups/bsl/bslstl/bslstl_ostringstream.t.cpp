// bslstl_ostringstream.t.cpp                                         -*-C++-*-
#include <bslstl_ostringstream.h>
#include <bslstl_string.h>
#include <bslstl_allocator.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <algorithm>

#include <cstdio>
#include <cstdlib>
#include <cstring>

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// 'bsl::basic_ostringstream' is a simple wrapper over 'std::basic_ostream'
// that uses 'bsl::basic_stringbuf' as an input buffer.  Therefore we don't
// neeed to test all the functionality derived from 'std::basic_ostream', only
// some basic output functions and the new methods provided by
// 'bsl::basic_ostringstream' itself.
// =============================
// OSTRINGSTREAM:
// [ 2] TESTING OSTRINGSTREAM
//-----------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE
// [ 1] BREATHING TEST

//==========================================================================
//                       STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------

using std::printf;
using std::fflush;
using std::atoi;

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

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

#define ASSERT_FAIL(expr) BSLS_ASSERTTEST_ASSERT_FAIL(expr)
#define ASSERT_PASS(expr) BSLS_ASSERTTEST_ASSERT_PASS(expr)
#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

#define ASSERT_FAIL_RAW(expr) BSLS_ASSERTTEST_ASSERT_FAIL_RAW(expr)
#define ASSERT_PASS_RAW(expr) BSLS_ASSERTTEST_ASSERT_PASS_RAW(expr)
#define ASSERT_SAFE_FAIL_RAW(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(expr)
#define ASSERT_SAFE_PASS_RAW(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(expr)

//=============================================================================
//                    STANDARD BDE LOOP-ASSERT TEST MACROS
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

#define LOOP6_ASSERT(I,J,K,L,M,N,X) {                \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\t"); \
                printf("%s", #J ": "); dbg_print(J); printf("\t"); \
                printf("%s", #K ": "); dbg_print(K); printf("\t"); \
                printf("%s", #L ": "); dbg_print(L); printf("\t"); \
                printf("%s", #M ": "); dbg_print(M); printf("\n"); \
                printf("%s", #N ": "); dbg_print(N); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                      SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define Q(X) printf("<| " #X " |>\n");     // Quote identifier literally.
#define P(X) dbg_print(#X " = ", X, "\n")  // Print identifier and value.
#define P_(X) dbg_print(#X " = ", X, ", ") // P(X) without '\n'
#define L_ __LINE__                        // current Line number
#define T_ putchar('\t');                  // Print a tab (w/o newline)

//=============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
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

//=============================================================================
//               GLOBAL HELPER CLASSES AND FUNCTION FOR TESTING
//-----------------------------------------------------------------------------

namespace
{

template <typename Stream, typename BaseStream>
void testCreateDefaultStream()
{
    Stream strm1;
    ASSERT(strm1.str().empty());

    BaseStream & strmref = strm1;
    ASSERT(strm1.rdbuf());
    ASSERT(strm1.rdbuf() == strmref.rdbuf());

    Stream strm2(std::ios_base::in);
    ASSERT(strm2.str().empty());

    Stream strm3(std::ios_base::out);
    ASSERT(strm3.str().empty());

    Stream strm4(std::ios_base::in | std::ios_base::out);
    ASSERT(strm4.str().empty());
}

template <typename Ostream>
void testCreateOstreamWithAllocator()
{
    using namespace BloombergLP;

    Ostream strm1(std::ios_base::out, bsl::allocator<char>());
    Ostream strm2(std::ios_base::out, bslma::Default::allocator());
    Ostream strm3(bsl::string("something"),
                  std::ios_base::out,
                  bslma::Default::allocator());
}

template <typename Ostream>
void testCreateOstreamWithString()
{
    std::string init("abc");

    Ostream strm1(init);
    ASSERT(strm1.str() == init);

    Ostream strm2(init, std::ios_base::in);
    ASSERT(strm2.str() == init);

    Ostream strm3(init, std::ios_base::out);
    ASSERT(strm3.str() == init);

    Ostream strm4(init, std::ios_base::in | std::ios_base::out);
    ASSERT(strm4.str() == init);
}

template <typename Ostream>
void testOstreamBasicOutput()
{
    Ostream strm;
    strm << 'a';
    ASSERT(strm.str() == "a");

    strm << 'b';
    ASSERT(strm.str() == "ab");
    ASSERT(strm.tellp() == native_std::streampos(2));

    strm.seekp(1);
    strm << "cd";
    ASSERT(strm.str() == "acd");
    ASSERT(strm.tellp() == native_std::streampos(3));
}

}

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------

namespace
{
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic input operations
///- - - - - - - - - - - - - - - - -
// The following example demonstrates the use of 'bsl::ostringstream' to write
// data of various types into a 'bsl::string' object.
//
// Suppose we want to implement a simplified converter from a generic type
// 'TYPE' to 'bsl::string'.  We use 'bsl::ostringstream' to implement the
// 'toString' function.  We write the data into the stream with 'operator<<'
// and then use the 'str' method to retrieve the resulting string from the
// stream:
//..
template <typename TYPE>
bsl::string toString(const TYPE& what)
{
    bsl::ostringstream out;
    out << what;
    return out.str();
}
//..
}

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    using namespace BloombergLP;

    int test = argc > 1 ? atoi(argv[1]) : 0;

    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;
    int veryVeryVeryVerbose = argc > 5;

    // As part of our overall allocator testing strategy, we will create
    // three test allocators.

    bslma::TestAllocator *globalAllocator_p,
                         *defaultAllocator_p,
                         *objectAllocator_p;

    // Object Test Allocator.
    bslma::TestAllocator objectAllocator("Object Allocator",
                                         veryVeryVeryVerbose);
    objectAllocator_p = &objectAllocator;
    (void) objectAllocator_p;

    // Default Test Allocator.
    bslma::TestAllocator defaultAllocator("Default Allocator",
                                          veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard guard(&defaultAllocator);
    defaultAllocator_p = &defaultAllocator;
    (void) defaultAllocator_p;

    // Global Test Allocator.
    bslma::TestAllocator  globalAllocator("Global Allocator",
                                          veryVeryVeryVerbose);
    bslma::Allocator *originalGlobalAllocator =
                          bslma::Default::setGlobalAllocator(&globalAllocator);
    globalAllocator_p = &globalAllocator;
    (void) globalAllocator_p;

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 Usage example compiles and executes without failures.
        //
        // Plan:
        //: 1 Copy and paste the usage example from the component header into
        //:   the test driver and replace 'assert' with 'ASSERT'.
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

// Finally, we verify that our 'toString' function works on some simple test
// cases:
//..
        ASSERT(toString(1234) == "1234");
        ASSERT(toString<short>(-5) == "-5");
        ASSERT(toString("abc") == "abc");
//..
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING OSTRINGSTREAM
        //
        // Concerns:
        //: 1 'ostringstream' object can be created with a with default
        //:   constructor.
        //: 2 'ostringstream' object can be created with a constructor with
        //:   allocator.
        //: 3 'ostringstream' object can be created with a constructor with
        //:   string.
        //: 4 'ostringstream' object can be used to perform basic output
        //:   operations.
        //
        // Plan:
        //: 1 Create 'ostringstream' object with a default constructor.
        //: 2 Create 'ostringstream' object with a constructor with allocator.
        //: 3 Create 'ostringstream' object with a constructor with string.
        //: 4 Exersice basic 'ostringstream' output operations.
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING OSTRINGSTREAM"
                            "\n=====================\n");

        if (veryVerbose) printf("\tcreate default ostringstream\n");

        testCreateDefaultStream<bsl::ostringstream, std::ostream>();

        if (veryVerbose) printf("\tcreate ostringstream with allocator\n");

        testCreateOstreamWithAllocator<bsl::ostringstream>();

        if (veryVerbose) printf("\tcreate ostringstream with string\n");

        testCreateOstreamWithString<bsl::ostringstream>();

        if (veryVerbose) printf("\tbasic output to ostringstream\n");

        testOstreamBasicOutput<bsl::ostringstream>();

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   We want to exercise the basic functionality.
        //
        // Plan:
        //   This "test" *exercises* basic functionality.
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    bslma::Default::setGlobalAllocator(originalGlobalAllocator);

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
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
