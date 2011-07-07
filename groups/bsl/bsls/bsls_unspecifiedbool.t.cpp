// bsls_unspecifiedbool.t.cpp                                         -*-C++-*-

#include <bsls_unspecifiedbool.h>

#include <stdio.h>
#include <stdlib.h>     // atoi()
#include <string.h>     // memcpy()

//#define TEST_FOR_COMPILE_ERRORS

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//                             ---------
// The 'bdema_managedptr' component provides a small number of classes that
// combine to provide a common solution to the problem of managing and
// transferring ownership of a dynamically allocated object.  We choose to test
// each class in turn, according to their
//
//-----------------------------------------------------------------------------
//                             Overview
//                             --------
// We are testing a proctor class that makes sure that only one instance holds
// a copy of an allocated pointer, along with the necessary information to
// deallocate it properly (the deleter).  The primary goal of this test program
// is to ascertain that no resource ever gets leaked, i.e., that when the
// proctor is re-assigned or destroyed, the managed pointer gets deleted
// properly.  In addition, we must also make sure that all the conversion and
// aliasing machinery works as documented.  At last, we must also check that
// a 'bdema_ManagedPtr' acts exactly as a pointer wherever one is expected.
//-----------------------------------------------------------------------------
// [ 2] Basic functionality
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] USAGE EXAMPLE
// [-1] VERIFYING FAILURES TO COMPILE

namespace {

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
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
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

//=============================================================================
//                         HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------


}  // close unnamed namespace

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------
namespace USAGE_EXAMPLE {

// A common requirement for "smart pointer" types is to emulate the native
// pointer types, and in particular support testing as a boolean value in 'if'
// and 'while' clauses.   Blah...:
//..
  template <class T>
  class simple_pointer
  {
  private:
      T *d_ptr;

  public:
      explicit simple_pointer(T *ptr = 0) : d_ptr(ptr) {}

      //! ~simpler_pointer() = default;

      T & operator*() const { return *d_ptr; }
      T * operator->() const { return d_ptr; }
//..
// blah...
//..
      typedef typename bsls_UnspecifiedBool<simple_pointer>::BoolType BoolType;

      operator BoolType() const {
          return d_ptr
               ? bsls_UnspecifiedBool<simple_pointer>::trueValue()
               : false;
      }
//..
// blah...
  }; // class simple_pointer
//..
// blah
//..
  void runTests() {
      int i = 3;
      simple_pointer<int> p1;
      simple_pointer<int> p2(&i);

      ASSERT(!p1);
      ASSERT(p2);
  }
//..
} // namespace USAGE_EXAMPLE

//=============================================================================
//                  TEST PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    (void) verbose;
    (void) veryVerbose;

    setbuf(stdout, 0);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);


    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        // Concerns:
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
          USAGE_EXAMPLE::runTests();
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING bdema_ManagedPtr_UnspecifiedBoolHelper
        //
        // Concerns:
        //: 1 Objects of type 'bdema_ManagedPtr_UnspecifiedBoolHelper' must be
        //:   implicitly convertible to bool.
        //: 2 Objects of type 'bdema_ManagedPtr_UnspecifiedBoolHelper' must not
        //:   promote to type 'int'.
        //: 3 A default constructed 'bdema_ManagedPtr_UnspecifiedBoolHelper'
        //:   should convert to 'false' when converted to a boolean value, the
        //:   same value as a default constructed 'bool'.
        //: 4 A 'bdema_ManagedPtr_UnspecifiedBoolHelper' object initialized
        //:   with the literal '0' should produce the value 'false' when
        //:   converted to a boolean value.
        //: 5 A class with a conversion operator to type
        //:   'bdema_ManagedPtr_UnspecifiedBoolHelper' should be implicitly
        //:   convertible to bool.
        //: 6 Two classes that are implicitly convertible to type
        //:   'bdema_ManagedPtr_UnspecifiedBoolHelper' should not accidentally
        //:   be comparable to each other using 'operator=='.
        //
        // Plan:
        //: 1 blah ...
        //
        // Testing:
        //   ... list class operations that are tested ...
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING bsls_UnspecifiedBool"
                            "\n----------------------------\n");

        if (verbose) printf("\tTest blah...\n");

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   1. That the functions exist with the documented signatures.
        //   2. That the basic functionality works as documented.
        //
        // Plan:
        //   Exercise each function in turn and devise an elementary test
        //   sequence to ensure that the basic functionality is as documented.
        //
        // Testing:
        //   This test exercises basic functionality but *tests* *nothing*.
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

      } break;
      case -1: {
        // --------------------------------------------------------------------
        // TESTING ADDITIONAL CONCERNS
        //
        // Concerns:
        //: 1 Two 'bdema_ManagedPtr<T>' objects should not be comparable with
		//:   the equality operator.
		//
        //: 2 Two objects of different instantiations of the 'bdema_ManagedPtr'
		//:   class template should not be comparable with the equality
		//:   operator
        //
        // Plan:
        //   The absence of a specific operator will be tested by failing to
		//   compile test code using that operator.  These tests will be
		//   configured to compile only when specific macros are defined as
		//   part of the build configuration, and not routinely tested.
        //
        // Testing:
        //   This test is checking for the *absence* of 'operator=='.
        // --------------------------------------------------------------------
//#define BSLS_UNSPECIFIEDBOOL_TEST_FLAG_1
//#define BSLS_UNSPECIFIEDBOOL_TEST_FLAG_2

#if defined BSLS_UNSPECIFIEDBOOL_TEST_FLAG_1
		bdema_ManagedPtr<int> x;
		bdema_ManagedPtr<int> y;
		// The following two lines should fail to compile
		ASSERT(x == y);
		ASSERT(x != y);
#endif

#if defined BSLS_UNSPECIFIEDBOOL_TEST_FLAG_2
		bdema_ManagedPtr<int> a;
		bdema_ManagedPtr<double> b;
		// The following two lines should fail to compile
		ASSERT(a == b);
		ASSERT(a != b);

		ASSERT(b == a);
		ASSERT(b != a);
#endif
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
