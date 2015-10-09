// bsls_util.t.cpp                                                    -*-C++-*-
#include <bsls_util.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>

#include <new>   // required to support use of "placement new"

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// 'bsls::Util' is a utility class, where each function will be tested in a
// separate test case.  Any significant test machinery will be tested before
// any function whose test case relies upon it.
//-----------------------------------------------------------------------------
// [2] TYPE *bsls::Util::addressOf(TYPE&);
// [3] BSLS_UTIL_ADDRESSOF macro
//-----------------------------------------------------------------------------
// [4] USAGE EXAMPLE
//
// Test apparatus
// [1]  CvQualification cvqOfPtr(T *p);
// [1]  EvilType *EvilType::operator&();
// [1]  EvilType *EvilType::realAddress();

//-----------------------------------------------------------------------------

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {

int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

// ============================================================================
//                      STANDARD BDE TEST DRIVER MACROS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Obtain the address of a 'class' that defines 'operator&'.
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// There are times, especially within low-level library functions, where it is
// necessary to obtain the address of an object even if that object's class
// overloads 'operator&' to return something other than the object's address.
//
// First, we create a special reference-like type that can refer to a single
// bit within a byte (inline implementations are provided in class scope for
// ease of exposition):
//..
    class BitReference {

        // DATA
        char *d_byte_p;
        int   d_bitpos;

      public:
        // CREATORS
        BitReference(char *byteptr = 0, int bitpos = 0)             // IMPLICIT
        : d_byte_p(byteptr)
        , d_bitpos(bitpos)
        {
        }

        // ACCESSORS
        operator bool() const { return (*d_byte_p >> d_bitpos) & 1; }

        int bitpos() const { return d_bitpos; }
        char *byteptr() const { return d_byte_p; }
    };
//..
// Then, we create a pointer-like type that can point to a single bit:
//..
    class BitPointer {

        // DATA
        char *d_byte_p;
        int   d_bitpos;

      public:
        // CREATORS
        BitPointer(char *byteptr = 0, int bitpos = 0)               // IMPLICIT
        : d_byte_p(byteptr)
        , d_bitpos(bitpos)
        {
        }

        // ACCESSORS
        BitReference operator*() const
        {
            return BitReference(d_byte_p, d_bitpos);
        }

        // etc.
    };
//..
// Next, we overload 'operator&' for 'BitReference' to return a 'BitPointer'
// instead of a raw pointer, completing the setup:
//..
    inline BitPointer operator&(const BitReference& ref)
    {
        return BitPointer(ref.byteptr(), ref.bitpos());
    }
//..

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

class EvilType {
    // This class supplies an overload for the unary 'operator&' in order to
    // test the correct behavior of the 'bsls::Util::addressOf' function.  Each
    // 'EvilType' object will reports its address as that returned by the
    // 'bogusPtr' method, which points to some statically allocated memory
    // outside the object itself.  The true address of such an object can be
    // discovered by calling the 'realAddress' method.

    // DATA
    int        d_dummy; // Some data to avoid empty-class optimizations
    static int d_bogus; // statically allocated object representing bad address

  public:
    // CLASS METHODS
    static EvilType *bogusPtr() {
        // Return the address of some valid memory that is not within the
        // footprint of any 'EvilType' object.
        return reinterpret_cast<EvilType*>(&d_bogus);
    }

    // CREATORS
    explicit EvilType(int val = 0) : d_dummy(val) { }
        // Create a 'EvilType' object.

    // ACCESSORS
    EvilType                *operator&()                { return bogusPtr(); }
    EvilType const          *operator&() const          { return bogusPtr(); }
    EvilType volatile       *operator&() volatile       { return bogusPtr(); }
    EvilType const volatile *operator&() const volatile { return bogusPtr(); }
        // Overload operator& to return something other than the address of
        // this object.

    EvilType                *realAddress()                { return this; }
    EvilType const          *realAddress() const          { return this; }
    EvilType volatile       *realAddress() volatile       { return this; }
    EvilType const volatile *realAddress() const volatile { return this; }
        // Return the actual address of this object.
};

int EvilType::d_bogus = 0;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

enum CvQualification {
    CVQ_UNQUALIFIED,
    CVQ_CONST,
    CVQ_VOLATILE,
    CVQ_CONST_VOLATILE
};

template <class TYPE>
inline
CvQualification cvqOfPtr(TYPE *) { return CVQ_UNQUALIFIED; }

template <class TYPE>
inline
CvQualification cvqOfPtr(const TYPE *) { return CVQ_CONST; }

template <class TYPE>
inline
CvQualification cvqOfPtr(volatile TYPE *) { return CVQ_VOLATILE; }

template <class TYPE>
inline
CvQualification cvqOfPtr(const volatile TYPE *) { return CVQ_CONST_VOLATILE; }

namespace TestFuncs
{
void a() {}
int b(double&) { return 0; }
double *c(const int &, volatile double) { return 0; }
}  // close namespace TestFuncs

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int             test = argc > 1 ? atoi(argv[1]) : 0;
    bool         verbose = argc > 2;
    bool     veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;

    (void) veryVeryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 4: {
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

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

// Then, we note that there are times when it might be desirable to get the
// true address of a 'BitReference'.  Since the above overload prevents the
// obvious syntax from working, we use 'bsls::Util::addressOf' to accomplish
// this task.
//
// Next, we create a 'BitReference' object:
//..
    char c[4];
    BitReference br(c, 3);
//..
// Now, we invoke 'bsls::Util::addressOf' to obtain and save the address of
// 'br':
//..
    BitReference *p = bsls::Util::addressOf(br);  // OK
    // BitReference *p = &br;                     // Won't compile
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
      case 3: {
        // --------------------------------------------------------------------
        // TESTING MACRO BSLS_UTIL_ADDRESSOF
        //
        // Concerns:
        //: 1 The macro applies 'bsls::Util::addressOf' on Windows and
        //:   'operator&' on every other platform.
        //
        // Plan:
        //: 1 Create an object of type 'EvilType' (see GLOBAL
        //:   TYPEDEFS/CONSTANTS FOR TESTING AND VARIABLES) with an overloaded
        //:   'operator&' and verify that 'bsls::Util::addressOf' invokes
        //:   'operator&' on UNIX, and that on Windows it returns the address
        //:    of the created object instead. (C-1)
        //
        // Testing:
        //   BSLS_UTIL_ADDRESSOF macro
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING MACRO BSLS_UTIL_ADDRESSOF"
                            "\n=================================\n");

        EvilType mX; const EvilType& X = mX;

#ifndef BDE_USE_ADDRESSOF
        if (verbose)
            printf("\nTest that 'bsls::Util::addressOf' returns 'bogusPtr'\n");
        ASSERT(X.bogusPtr() == BSLS_UTIL_ADDRESSOF(X));
#else
        if (verbose)
             printf("\nTest that 'bsls::Util::addressOf' returns '&X'\n");
        ASSERT(X.realAddress() == BSLS_UTIL_ADDRESSOF(X));
#endif

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'addressOf'
        //
        // Concerns:
        //: 1 Calling 'bsls::Util::addressOf' on an object will return the
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
        //: 1 Create a number of objects of types 'int' and 'EvilType', where
        //:   'EvilType' has an overloaded 'operator&'.  Test that the pointer
        //:   returned from calling 'addressOf' on each object points to that
        //:   object.  (In the case of 'EvilType', the 'realAddress' method
        //:   returns the true address of the object. (C-1, 3)
        //:
        //: 2 Using objects and references with all four different
        //:   cv-qualifications, verify that the pointer returned by
        //:   'addressOf' has the correct qualification (using the 'cvqOfPtr'
        //:   function). (C-2)
        //
        // Testing:
        //   TYPE *bsls::Util::addressOf(TYPE&);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'addressOf'"
                            "\n===================\n");

        if (verbose) printf("\nTESTING addressof objects\n");

        if (veryVerbose) printf("\nTesting addressOf(int)\n");

        int                  i = 0;
        int const           ci = 0;
        int       volatile  vi = 0;
        int const volatile cvi = 0;

        ASSERT(&  i == bsls::Util::addressOf(  i));
        ASSERT(& ci == bsls::Util::addressOf( ci));
        ASSERT(& vi == bsls::Util::addressOf( vi));
        ASSERT(&cvi == bsls::Util::addressOf(cvi));

        ASSERT(CVQ_UNQUALIFIED    == cvqOfPtr(bsls::Util::addressOf(  i)));
        ASSERT(CVQ_CONST          == cvqOfPtr(bsls::Util::addressOf( ci)));
        ASSERT(CVQ_VOLATILE       == cvqOfPtr(bsls::Util::addressOf( vi)));
        ASSERT(CVQ_CONST_VOLATILE == cvqOfPtr(bsls::Util::addressOf(cvi)));

        if (veryVerbose) printf("\nTesting addressOf(EvilType)\n");

        EvilType                  x(0);
        EvilType const           cx(0);
        EvilType       volatile  vx(0);
        EvilType const volatile cvx(0);

        ASSERT(  x.realAddress() == bsls::Util::addressOf(  x));
        ASSERT( cx.realAddress() == bsls::Util::addressOf( cx));
        ASSERT( vx.realAddress() == bsls::Util::addressOf( vx));
        ASSERT(cvx.realAddress() == bsls::Util::addressOf(cvx));

        ASSERT(CVQ_UNQUALIFIED    == cvqOfPtr(bsls::Util::addressOf(  x)));
        ASSERT(CVQ_CONST          == cvqOfPtr(bsls::Util::addressOf( cx)));
        ASSERT(CVQ_VOLATILE       == cvqOfPtr(bsls::Util::addressOf( vx)));
        ASSERT(CVQ_CONST_VOLATILE == cvqOfPtr(bsls::Util::addressOf(cvx)));

        if (verbose) printf("\nTESTING addressof references\n");

        if (veryVerbose) printf("\nTesting addressOf(reference to int)\n");

        int               &   ri = i;
        int const         &  cri = i;
        int       volatile&  vri = i;
        int const volatile& cvri = i;

        ASSERT(&i == bsls::Util::addressOf(  ri));
        ASSERT(&i == bsls::Util::addressOf( cri));
        ASSERT(&i == bsls::Util::addressOf( vri));
        ASSERT(&i == bsls::Util::addressOf(cvri));

        ASSERT(CVQ_UNQUALIFIED    == cvqOfPtr(bsls::Util::addressOf(  ri)));
        ASSERT(CVQ_CONST          == cvqOfPtr(bsls::Util::addressOf( cri)));
        ASSERT(CVQ_VOLATILE       == cvqOfPtr(bsls::Util::addressOf( vri)));
        ASSERT(CVQ_CONST_VOLATILE == cvqOfPtr(bsls::Util::addressOf(cvri)));

        if (veryVerbose)
                        printf("\nTesting addressOf(reference to EvilType)\n");

        EvilType               &   rx = x;
        EvilType const         &  crx = x;
        EvilType       volatile&  vrx = x;
        EvilType const volatile& cvrx = x;

        ASSERT(x.realAddress() == bsls::Util::addressOf(  rx));
        ASSERT(x.realAddress() == bsls::Util::addressOf( crx));
        ASSERT(x.realAddress() == bsls::Util::addressOf( vrx));
        ASSERT(x.realAddress() == bsls::Util::addressOf(cvrx));

        ASSERT(CVQ_UNQUALIFIED    == cvqOfPtr(bsls::Util::addressOf(  rx)));
        ASSERT(CVQ_CONST          == cvqOfPtr(bsls::Util::addressOf( crx)));
        ASSERT(CVQ_VOLATILE       == cvqOfPtr(bsls::Util::addressOf( vrx)));
        ASSERT(CVQ_CONST_VOLATILE == cvqOfPtr(bsls::Util::addressOf(cvrx)));

        ASSERT(TestFuncs::a == bsls::Util::addressOf(TestFuncs::a));
        ASSERT(TestFuncs::b == bsls::Util::addressOf(TestFuncs::b));
        ASSERT(TestFuncs::c == bsls::Util::addressOf(TestFuncs::c));
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING TEST APPARATUS
        //
        // Concerns:
        //: 1 That 'cvqOfPtr' returns the correct result for each
        //:   pointer-to-cv-qualified type.
        //:
        //: 2 That 'realAddress' returns the true address of a 'EvilType'
        //:   object.
        //:
        //: 3 That 'EvilType' overloads 'operator&' such that it returns
        //:   something other than the address of the object.
        //
        // Test Plan:
        //: 1 Call 'cvqOfPtr' with pointers of each of the four different
        //:   cv-qualifications and verify the returned value. (C-1)
        //:
        //: 2 Construct a 'EvilType' object at a known address, by using an
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
        //   EvilType *EvilType::operator&();
        //   EvilType *EvilType::realAddress();
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING TEST APPARATUS"
                            "\n======================\n");

        if (verbose) printf("\nTesting cvqOfPtr\n");

        int dummy = 7;

        void                *  p = &dummy;
        void const          * cp = &dummy;
        int        volatile * vp = &dummy;
        int  const volatile *cvp = &dummy;

        ASSERT(CVQ_UNQUALIFIED    == cvqOfPtr(  p));
        ASSERT(CVQ_CONST          == cvqOfPtr( cp));
        ASSERT(CVQ_VOLATILE       == cvqOfPtr( vp));
        ASSERT(CVQ_CONST_VOLATILE == cvqOfPtr(cvp));

        if (verbose) printf("\nTesting EvilType::realAddress()\n");

        char buffer[sizeof(EvilType)];
        void * const trueAddress = buffer;

        EvilType *pX = new(trueAddress)EvilType(0);

        EvilType               &   x  = *pX;
        EvilType const         &  cx  = *pX;
        EvilType       volatile&  vx  = *pX;
        EvilType const volatile& cvx  = *pX;

        ASSERT(trueAddress ==   x.realAddress());
        ASSERT(trueAddress ==  cx.realAddress());
        ASSERT(trueAddress ==  vx.realAddress());
        ASSERT(trueAddress == cvx.realAddress());

        if (verbose) printf("\nTesting operator&(EvilType)\n");

        ASSERT(&  x !=   x.realAddress());
        ASSERT(& cx !=  cx.realAddress());
        ASSERT(& vx !=  vx.realAddress());
        ASSERT(&cvx != cvx.realAddress());
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
// Copyright 2013 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
