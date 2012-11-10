// bslma_deleterhelper.t.cpp                                          -*-C++-*-

#include <bslma_deleterhelper.h>

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>

#include <new>

using namespace BloombergLP;
//using namespace std;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// This test driver tests helper functions provided by the
// 'bslma::DeleterHelper' namespace.  We need to verify that the 'deleteObject'
// method destroys an object and calls the deallocate method of the supplied
// allocator or pool.
//-----------------------------------------------------------------------------
// [2] template <TYPE, ALLOC> deleteObject(const TYPE *, ALLOC *);
// [1] template <TYPE, ALLOC> deleteObjectRaw(const TYPE *, ALLOC *);
//-----------------------------------------------------------------------------
// [3] USAGE EXAMPLE
//=============================================================================

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

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

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

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bslma::DeleterHelper Obj;

//=============================================================================
//                          HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

class my_NewDeleteAllocator {
    // Test class used to verify examples.

    int d_count;

    enum { MAGIC   = 0xDEADBEEF,
           DELETED = 0xBADF000D };

  public:
    my_NewDeleteAllocator(): d_count(0) { }
    ~my_NewDeleteAllocator() { }

    void *allocate(unsigned size)  {
        unsigned *p = (unsigned *) operator new(size +
                                      bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);
        *p = MAGIC;

        ++d_count;
        return (char *) p + bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;
    }

    void deallocate(void *address)  {
        unsigned *p = (unsigned *)
                         ((bsls::AlignmentUtil::MaxAlignedType *) address - 1);
        ASSERT(MAGIC == *p);
        *p = DELETED;

        ++d_count;
        operator delete(p);
    }

    int getCount() const            { return d_count; }
};

static int globalObjectStatus = 0;  // global flag set by test-object d'tors
static int class3ObjectCount  = 0;  // count set by 'my_Class3' c'tor/d'tor

class my_Class1 {
  public:
    my_Class1()  { globalObjectStatus = 1; }
    ~my_Class1() { globalObjectStatus = 0; }
};

class my_Class2 {
  public:
    my_Class2()          { globalObjectStatus = 1; }
    virtual ~my_Class2() { globalObjectStatus = 0; }
};

class my_Class3Base {
  public:
    my_Class3Base() { }
    virtual ~my_Class3Base() = 0;
};

class my_Class3 : public my_Class3Base {
  public:
    my_Class3() { ++class3ObjectCount; }
    virtual ~my_Class3();
};

my_Class3Base::~my_Class3Base() { }
my_Class3::~my_Class3() { --class3ObjectCount; }

// The "dreaded diamond".

static int virtualBaseObjectCount = 0;
static int leftBaseObjectCount    = 0;
static int rightBaseObjectCount   = 0;
static int mostDerivedObjectCount = 0;

class my_VirtualBase {
    int x;
public:
    my_VirtualBase()          { ++virtualBaseObjectCount; }
    virtual ~my_VirtualBase() { --virtualBaseObjectCount; }
};

class my_LeftBase : virtual public my_VirtualBase {
    int x;
public:
    my_LeftBase()             { ++leftBaseObjectCount; }
    virtual ~my_LeftBase()    { --leftBaseObjectCount; }
};

class my_RightBase : virtual public my_VirtualBase {
    int x;
public:
    my_RightBase()            { ++rightBaseObjectCount; }
    virtual ~my_RightBase()   { --rightBaseObjectCount; }
};

class my_MostDerived : public my_LeftBase, public my_RightBase {
    int x;
public:
    my_MostDerived()          { ++mostDerivedObjectCount; }
    ~my_MostDerived()         { --mostDerivedObjectCount; }
};

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// The following 'my_RawDeleterGuard' class defines a guard that
// unconditionally deletes a managed object upon destruction.  Via the
// 'deleteObjectRaw' method supplied by this component, the guard's destructor
// first destroys the managed object, then deallocates the footprint of the
// object.  The declaration of 'my_RawDeleterGuard' follows:
//..
    template <class TYPE, class ALLOCATOR>
    class my_RawDeleterGuard {
        // This class implements a guard that unconditionally deletes a managed
        // object upon destruction by first invoking the object's destructor,
        // and then invoking the 'deallocate' method of an allocator (or pool)
        // of parameterized 'ALLOCATOR' type supplied at construction.

        // DATA
        TYPE      *d_object_p;     // managed object
        ALLOCATOR *d_allocator_p;  // allocator or pool (held, not owned)

        // NOT IMPLEMENTED
        my_RawDeleterGuard(const my_RawDeleterGuard&);
        my_RawDeleterGuard& operator=(const my_RawDeleterGuard&);

      public:
        // CREATORS
        my_RawDeleterGuard(TYPE *object, ALLOCATOR *allocator);
            // Create a raw deleter guard that unconditionally manages the
            // specified 'object', and that uses the specified 'allocator' to
            // delete 'object' upon the destruction of this guard.  The
            // behavior is undefined unless 'object' and 'allocator' are
            // non-zero, and 'allocator' supplied the memory for 'object'.
            // Note that 'allocator' must remain valid throughout the lifetime
            // of this guard.

        ~my_RawDeleterGuard();
            // Destroy this raw deleter guard and delete the object it manages
            // by first invoking the destructor of the (managed) object, and
            // then invoking the 'deallocate' method of the allocator (or pool)
            // that was supplied with the object at construction.
    };
//..
// The 'deleteObjectRaw' method is used in the destructor as follows:
//..
    template <class TYPE, class ALLOCATOR>
    inline
    my_RawDeleterGuard<TYPE, ALLOCATOR>::~my_RawDeleterGuard()
    {
        bslma::DeleterHelper::deleteObjectRaw(d_object_p, d_allocator_p);
    }
//..
// Note that we've denoted our guard to be a "raw" guard in keeping with this
// use of 'deleteObjectRaw' (as opposed to 'deleteObject').

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
//  int veryVerbose = argc > 3;        // unused so far
//  int veryVeryVerbose = argc > 4;    // unused so far

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE TEST
        //
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

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        // The Usage example from the component header file is replicated
        // above.

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // MEMBER TEMPLATE METHOD 'deleteObject' TEST:
        //   We want to make sure that when 'deleteObject' is used both
        //   destructor and 'deallocate' are invoked.
        //
        // Plan:
        //   Using an allocator and placement new operator construct objects of
        //   two different classes.  Invoke 'deleteObjectRaw' to delete
        //   constructed objects and check that both destructor and
        //   'deallocate' have been called.  Repeat tests with a derived-class
        //   object with a virtual destructor.  Test with null pointer.
        //
        // Testing:
        //   template <TYPE, ALLOC> deleteObject(const TYPE *, ALLOC *);
        // --------------------------------------------------------------------

        if (verbose) printf("\n'deleteObjectRaw' TEST"
                            "\n======================\n");

        if (verbose) printf("\nTesting 'deleteObject':\n");
        {
            my_NewDeleteAllocator a;

            if (verbose) printf("\twith a my_Class1 object\n");

            if (verbose) { T_;  T_;  P(globalObjectStatus); }
            ASSERT(0 == globalObjectStatus);

            ASSERT(0 == a.getCount());
            my_Class1 *pC1 = (my_Class1 *) a.allocate(sizeof(my_Class1));
            const my_Class1 *pC1CONST = pC1;
            ASSERT(1 == a.getCount());

            new(pC1) my_Class1;
            if (verbose) { T_;  T_;  P(globalObjectStatus); }
            ASSERT(1 == globalObjectStatus);

            ASSERT(1 == a.getCount());
            Obj::deleteObject(pC1CONST, &a);
            if (verbose) { T_;  T_;  P(globalObjectStatus); }
            ASSERT(0 == globalObjectStatus);   ASSERT(2 == a.getCount());

            if (verbose) printf("\twith a my_Class2 object\n");

            if (verbose) { T_;  T_;  P(globalObjectStatus); }
            ASSERT(0 == globalObjectStatus);

            ASSERT(2 == a.getCount());
            my_Class2 *pC2 = (my_Class2 *) a.allocate(sizeof(my_Class2));
            const my_Class2 *pC2CONST = pC2;
            ASSERT(3 == a.getCount());

            new(pC2) my_Class2;
            if (verbose) { T_;  T_;  P(globalObjectStatus); }
            ASSERT(1 == globalObjectStatus);

            ASSERT(3 == a.getCount());
            Obj::deleteObject(pC2CONST, &a);
            if (verbose) { T_;  T_;  P(globalObjectStatus); }
            ASSERT(0 == globalObjectStatus);   ASSERT(4 == a.getCount());

            if (verbose) printf("\tWith a polymorphic object\n");

            ASSERT(0 == class3ObjectCount);
            my_Class3 *pC3 = (my_Class3 *) a.allocate(sizeof(my_Class3));
            ASSERT(0 == class3ObjectCount);
            ASSERT(0 == globalObjectStatus);
            ASSERT(5 == a.getCount());

            new(pC3) my_Class3;
            if (verbose) { T_;  T_;  P(class3ObjectCount); }
            ASSERT(1 == class3ObjectCount);
            ASSERT(0 == globalObjectStatus);

            ASSERT(5 == a.getCount());
            Obj::deleteObject(pC3, &a);
            if (verbose) { T_;  T_;  P(class3ObjectCount); }
            ASSERT(0 == class3ObjectCount);
            ASSERT(0 == globalObjectStatus);
            ASSERT(6 == a.getCount());

            if (verbose) printf("\tWith a null my_Class3 pointer\n");

            pC3 = 0;
            Obj::deleteObject(pC3, &a);
            if (verbose) { T_;  T_;  P(class3ObjectCount); }
            ASSERT(0 == class3ObjectCount);
            ASSERT(0 == globalObjectStatus);
            ASSERT(6 == a.getCount());
        }
        {
            my_NewDeleteAllocator a;

            if (verbose) printf("\tdeleteObject(my_MostDerived*)\n");

            ASSERT(0 == a.getCount());
            my_MostDerived *pMost =
                         (my_MostDerived *) a.allocate(sizeof(my_MostDerived));
            const my_MostDerived *pMostCONST = pMost;
            ASSERT(1 == a.getCount());

            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);
            new(pMost) my_MostDerived;
            ASSERT(1 == mostDerivedObjectCount);
            ASSERT(1 == rightBaseObjectCount);
            ASSERT(1 == leftBaseObjectCount);
            ASSERT(1 == virtualBaseObjectCount);

            Obj::deleteObject(pMostCONST, &a);
            ASSERT(2 == a.getCount());
            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);
        }
        {
            my_NewDeleteAllocator a;

            if (verbose) printf(
                         "\tdeleteObject(my_RightBase*) with TestAllocator\n");

            my_MostDerived *pMost =
                         (my_MostDerived *) a.allocate(sizeof(my_MostDerived));
            const my_MostDerived *pMostCONST = pMost;

            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);
            new(pMost) my_MostDerived;
            ASSERT(1 == mostDerivedObjectCount);
            ASSERT(1 == rightBaseObjectCount);
            ASSERT(1 == leftBaseObjectCount);
            ASSERT(1 == virtualBaseObjectCount);

            const my_RightBase *pRBC = pMostCONST;
            ASSERT((void *) pRBC != (void *) pMost);

            Obj::deleteObject(pRBC, &a);
            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);
        }
        {
            if (verbose) printf("\tNegative testing\n");

            bsls::AssertFailureHandlerGuard guard(
                                            &bsls::AssertTest::failTestDriver);

            my_NewDeleteAllocator a;
            my_NewDeleteAllocator *null = 0;

            my_Class1 *pC1 = (my_Class1 *) a.allocate(sizeof(my_Class1));
            new(pC1) my_Class1;

            ASSERT_SAFE_FAIL(Obj::deleteObject(pC1, null));
            ASSERT_SAFE_PASS(Obj::deleteObject(pC1, &a));
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // MEMBER TEMPLATE METHOD 'deleteObjectRaw' TEST:
        //   We want to make sure that when 'deleteObjRaw' is used both
        //   destructor and 'deallocate' are invoked.
        //
        // Plan:
        //   Using an allocator and placement new operator construct objects of
        //   two different classes.  Invoke 'deleteObjectRaw' to delete
        //   constructed objects and check that both destructor and
        //   'deallocate' have been called.  Repeat tests with a derived-class
        //   object with a virtual destructor.  Test with null pointer.
        //
        // Testing:
        //   template <TYPE, ALLOC> deleteObjectRaw(const TYPE *, ALLOC *);
        // --------------------------------------------------------------------

        if (verbose) printf("\n'deleteObjectRaw' TEST"
                            "\n======================\n");

        if (verbose) printf("\nTesting 'deleteObjectRaw':\n");
        {
            my_NewDeleteAllocator a;

            if (verbose) printf("\twith a my_Class1 object\n");

            if (verbose) { T_;  T_;  P(globalObjectStatus); }
            ASSERT(0 == globalObjectStatus);

            ASSERT(0 == a.getCount());
            my_Class1 *pC1 = (my_Class1 *) a.allocate(sizeof(my_Class1));
            const my_Class1 *pC1CONST = pC1;
            ASSERT(1 == a.getCount());

            new(pC1) my_Class1;
            if (verbose) { T_;  T_;  P(globalObjectStatus); }
            ASSERT(1 == globalObjectStatus);

            ASSERT(1 == a.getCount());
            Obj::deleteObjectRaw(pC1CONST, &a);
            if (verbose) { T_;  T_;  P(globalObjectStatus); }
            ASSERT(0 == globalObjectStatus);   ASSERT(2 == a.getCount());

            if (verbose) printf("\twith a my_Class2 object\n");

            if (verbose) { T_;  T_;  P(globalObjectStatus); }
            ASSERT(0 == globalObjectStatus);

            ASSERT(2 == a.getCount());
            my_Class2 *pC2 = (my_Class2 *) a.allocate(sizeof(my_Class2));
            const my_Class2 *pC2CONST = pC2;
            ASSERT(3 == a.getCount());

            new(pC2) my_Class2;
            if (verbose) { T_;  T_;  P(globalObjectStatus); }
            ASSERT(1 == globalObjectStatus);

            ASSERT(3 == a.getCount());
            Obj::deleteObjectRaw(pC2CONST, &a);
            if (verbose) { T_;  T_;  P(globalObjectStatus); }
            ASSERT(0 == globalObjectStatus);   ASSERT(4 == a.getCount());

            if (verbose) printf("\tWith a polymorphic object\n");

            ASSERT(0 == class3ObjectCount);
            my_Class3 *pC3 = (my_Class3 *) a.allocate(sizeof(my_Class3));
            ASSERT(0 == class3ObjectCount);
            ASSERT(0 == globalObjectStatus);
            ASSERT(5 == a.getCount());

            new(pC3) my_Class3;
            if (verbose) { T_;  T_;  P(class3ObjectCount); }
            ASSERT(1 == class3ObjectCount);
            ASSERT(0 == globalObjectStatus);

            ASSERT(5 == a.getCount());
            Obj::deleteObjectRaw(pC3, &a);
            if (verbose) { T_;  T_;  P(class3ObjectCount); }
            ASSERT(0 == class3ObjectCount);
            ASSERT(0 == globalObjectStatus);
            ASSERT(6 == a.getCount());

            if (verbose) printf("\tWith a null my_Class3 pointer\n");

            pC3 = 0;
            Obj::deleteObject(pC3, &a);
            if (verbose) { T_;  T_;  P(class3ObjectCount); }
            ASSERT(0 == class3ObjectCount);
            ASSERT(0 == globalObjectStatus);
            ASSERT(6 == a.getCount());
        }
        {
            my_NewDeleteAllocator a;

            if (verbose) printf("\tdeleteObjectRaw(my_MostDerived*)\n");

            ASSERT(0 == a.getCount());
            my_MostDerived *pMost =
                         (my_MostDerived *) a.allocate(sizeof(my_MostDerived));
            const my_MostDerived *pMostCONST = pMost;
            ASSERT(1 == a.getCount());

            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);
            new(pMost) my_MostDerived;
            ASSERT(1 == mostDerivedObjectCount);
            ASSERT(1 == rightBaseObjectCount);
            ASSERT(1 == leftBaseObjectCount);
            ASSERT(1 == virtualBaseObjectCount);

            Obj::deleteObjectRaw(pMostCONST, &a);
            ASSERT(2 == a.getCount());
            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);
        }
        {
            if (verbose) printf("\tNegative testing\n");

            bsls::AssertFailureHandlerGuard guard(
                                            &bsls::AssertTest::failTestDriver);

            my_NewDeleteAllocator a;
            my_NewDeleteAllocator *null = 0;

            my_Class1 *pC1 = (my_Class1 *) a.allocate(sizeof(my_Class1));
            new(pC1) my_Class1;

            ASSERT_SAFE_FAIL(Obj::deleteObjectRaw(pC1, null));
            ASSERT_SAFE_PASS(Obj::deleteObjectRaw(pC1, &a));
        }

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
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
