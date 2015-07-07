// bslma_sharedptrrep.t.cpp                                           -*-C++-*-
#include <bslma_sharedptrrep.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_testallocator.h>
#include <bsls_bsltestutil.h>

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'


#ifdef BSLS_PLATFORM_CMP_MSVC  // Microsoft Compiler
#ifdef _MSC_EXTENSIONS         // Microsoft Extensions Enabled
#include <new>                 // if so, need to include new as well
#endif
#endif

// These warning suppressions will be eliminated in due course.
// BDE_VERIFY pragma: -FD01  // Test-machinery lacks a contract
// BDE_VERIFY pragma: -TP06  // Test-case indexing thing
// BDE_VERIFY pragma: -TP09  // Test-case indexing thing
// BDE_VERIFY pragma: -TP18  // Test-case banners are ALL-CAPS

using namespace BloombergLP;

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This test driver tests the functionality of the representation object for
// shared pointer.
// - The test plans are still to be written (so marked TBD).
//-----------------------------------------------------------------------------
// bslma::SharedPtrRep
//--------------------
// [ 9] void managedPtrDeleter(void *, bslma::SharedPtrRep *rep); // TBD
// [ 2] bslma::SharedPtrRep();
// [ 3] void acquireRef();
// [ 8] void incrementRefs(int incrementAmount = 1);
// [ 3] void acquireWeakRef();
// [ 4] void releaseRef();
// [ 4] void releaseWeakRef();
// [ 7] bool tryAcquireRef();
// [ 6] void resetCountsRaw(int numSharedReferences, int numWeakReferences);
// [ 5] void disposeRep();
// [ 5] void disposeObject();
// [ 2] int numReferences() const;
// [ 2] int numWeakReferences() const;
// [ 2] bool hasUniqueOwner() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [10] USAGE EXAMPLE // TBD
//-----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

// TEST IMPLEMENTATION (defined below)
class MyTestImplementation;

// TYPEDEFS
typedef bslma::SharedPtrRep Obj;
typedef MyTestImplementation TObj;

//=============================================================================
//             GLOBAL HELPER CLASSES AND FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

                         // ==========================
                         // class MyTestImplementation
                         // ==========================

class MyTestImplementation : public bslma::SharedPtrRep {
    // This class provides an implementation for 'bslma::SharedPtrRep' so that
    // it can be initialized and tested.

    // DATA
    int d_numRepDisposed;
    int d_numObjectDisposed;

  public:
    // CREATORS
    MyTestImplementation();

    // MANIPULATORS
    virtual void disposeObject();
    virtual void disposeRep();
    virtual void *getDeleter(const std::type_info& type);

    // ACCESSORS
    bool getNumObjectDisposed();
    bool getNumRepDisposed();
    virtual void *originalPtr() const;
};

                         // --------------------------
                         // class MyTestImplementation
                         // --------------------------

// CREATORS
MyTestImplementation::MyTestImplementation()
: d_numRepDisposed(0)
, d_numObjectDisposed(0)
{
}

// MANIPULATORS
void MyTestImplementation::disposeObject()
{
    // The implementation of this method allows verification on whether
    // 'disposeObject' is called or not.

    ++d_numObjectDisposed;
}

void MyTestImplementation::disposeRep()
{
    // The implementation of this method allows verification on whether
    // 'disposeRep' is called or not.

    ++d_numRepDisposed;
}

void *MyTestImplementation::getDeleter(const std::type_info& /*type*/)
{
    return 0;
}

// ACCESSORS
bool MyTestImplementation::getNumObjectDisposed()
{
    return d_numObjectDisposed;
}

bool MyTestImplementation::getNumRepDisposed()
{
    return d_numRepDisposed;
}

void *MyTestImplementation::originalPtr() const
{
    return NULL;
}

#if 0  // TBD Need an appropriately levelized usage example
                      // =============================
                      // class MySharedDatetimeRepImpl
                      // =============================

class MySharedDatetimeRepImpl : public bslma::SharedPtrRep {
  private:
    // Implementation of 'bslma::SharedPtrRep' for in-place object.

    // DATA
    bslma::Allocator *d_allocator_p; // memory allocator (held, not owned)
    bdet_Datetime     d_instance;    // in-place object

  public:
    // CREATORS
    MySharedDatetimeRepImpl(bslma::Allocator *basicAllocator,
                            int               year,
                            int               month,
                            int               day);
        // Create a shared representation of a 'bdet_Datetime' object having
        // the specified 'year', 'month' and 'day' using the specified
        // 'basicAllocator' to allocate memory.

    // MANIPULATORS
    virtual void disposeRep();
        // Dispose of this 'MySharedDatetimeRepImpl' object.

    virtual void disposeObject();
        // Dispose of the managed 'bdet_Datetime' object.

    // ACCESSORS
    bdet_Datetime *ptr();
        // Returns a modifiable pointer to the managed 'bdet_Datetime' object.

    virtual void *originalPtr() const;
        // Returns a void pointer to the to the managed 'bdet_Datetime' object;
};


                      // -----------------------------
                      // class MySharedDatetimeRepImpl
                      // -----------------------------

MySharedDatetimeRepImpl::MySharedDatetimeRepImpl(
                                              bslma::Allocator *basicAllocator,
                                              int               year,
                                              int               month,
                                              int               day)
: d_allocator_p(basicAllocator)
, d_instance(year, month, day)
{
}

void MySharedDatetimeRepImpl::disposeRep() {
    d_allocator_p->deallocate(this);
}

void MySharedDatetimeRepImpl::disposeObject() {
    d_instance.~bdet_Datetime();
}

bdet_Datetime *MySharedDatetimeRepImpl::ptr() {
    return &d_instance;
}

void *MySharedDatetimeRepImpl::originalPtr() const {
    return const_cast<void*>(static_cast<const void *>(&d_instance));
}

                      // ======================
                      // class MySharedDatetime
                      // ======================

class MySharedDatetime {
    // This class provide a reference counted managed pointer to supporting
    // shared ownership of a 'bdet_Datetime' object.

  private:
    bdet_Datetime       *d_ptr_p;  // pointer to the managed object
    bslma::SharedPtrRep *d_rep_p;  // pointer to the representation object

  public:
    // CREATORS
    MySharedDatetime();
        // Create an empty shared datetime.

    MySharedDatetime(bdet_Datetime* ptr, bslma::SharedPtrRep* rep);
        // Create a shared datetime that adopts ownership of the specified
        // 'ptr' and the specified 'rep.

    MySharedDatetime(const MySharedDatetime& original);
        // Create a shared datetime that refers to the same object managed by
        // the specified 'original'

    ~MySharedDatetime();
        // Destroy this shared datetime and release the reference any object it
        // might be referring to.

    // MANIPULATORS
    void createInplace(bslma::Allocator *basicAllocator,
                       int               year,
                       int               month,
                       int               day);
        // Create a new 'MySharedDatetimeRepImpl', using the specified
        // 'basicAllocator' to supply memory, using the specified 'year',
        // 'month' and 'day' to initialize the 'bdet_Datetime' within the newly
        // created 'MySharedDatetimeRepImpl', and make this 'MySharedDatetime'
        // refer to the newly created 'bdet_Datetime'.

    bdet_Datetime& operator*() const;
        // Return a modifiable reference to the shared datetime.

    bdet_Datetime *operator->() const;
        // Return the address of the modifiable 'bdet_Datetime' to which this
        // object refers.

    bdet_Datetime *ptr() const;
        // Return the address of the modifiable 'bdet_Datetime' to which this
        // object refers.
};

                         // ----------------------
                         // class MySharedDatetime
                         // ----------------------

MySharedDatetime::MySharedDatetime()
: d_ptr_p(0)
, d_rep_p(0)
{
}

MySharedDatetime::MySharedDatetime(bdet_Datetime      *ptr,
                                   bslma::SharedPtrRep *rep)
: d_ptr_p(ptr)
, d_rep_p(rep)
{
}

MySharedDatetime::MySharedDatetime(const MySharedDatetime& original)
: d_ptr_p(original.d_ptr_p)
, d_rep_p(original.d_rep_p)
{
    if (d_ptr_p) {
        d_rep_p->acquireRef();
    } else {
        d_rep_p = 0;
    }
}

MySharedDatetime::~MySharedDatetime()
{
    if (d_rep_p) {
        d_rep_p->releaseRef();
    }
}

void MySharedDatetime::createInplace(bslma::Allocator *basicAllocator,
                                     int               year,
                                     int               month,
                                     int               day)
{
    basicAllocator = bslma::Default::allocator(basicAllocator);
    MySharedDatetimeRepImpl *rep = new (*basicAllocator)
                                        MySharedDatetimeRepImpl(basicAllocator,
                                                                year,
                                                                month,
                                                                day);
    MySharedDatetime temp(rep->ptr(), rep);
    bsl::swap(d_ptr_p, temp.d_ptr_p);
    bsl::swap(d_rep_p, temp.d_rep_p);
}

bdet_Datetime& MySharedDatetime::operator*() const {
    return *d_ptr_p;
}

bdet_Datetime *MySharedDatetime::operator->() const {
    return d_ptr_p;
}

bdet_Datetime *MySharedDatetime::ptr() const {
    return d_ptr_p;
}
#endif

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)veryVerbose;           // suppress unused variable warning
    (void)veryVeryVerbose;       // suppress unused variable warning

    printf("TEST " __FILE__ " CASE %d\n", test);

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // Confirm no static initialization locked the global allocator
    ASSERT(&globalAllocator == bslma::Default::globalAllocator());

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    // Confirm no static initialization locked the default allocator
    ASSERT(&defaultAllocator == bslma::Default::defaultAllocator());

    bslma::TestAllocator ta;

    switch (test) { case 0:  // Zero is always the leading case.
#if 0  // TBD Need an appropriately levelized usage example
      case 10: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concern:
        //   Usage example described in header doc compiles and run.
        //
        // Plan:
        //   Copy the usage example and strip the comments.  Then create simple
        //   test case to use the implementation described in doc.
        //
        // Testing:
        //   Usage example
        // --------------------------------------------------------------------
        {
            MySharedDatetime dt1;
            ASSERT(0 == ta.numAllocations());

            dt1.createInplace(&ta, 2011, 1, 1);

            ASSERT(1 == ta.numAllocations());

            MySharedDatetime dt2(dt1);

            ASSERT(dt2.ptr() == dt1.ptr());
            ASSERT(*dt2 == *dt1);

            dt2->addDays(31);
            ASSERT(2 == dt2->month());
            ASSERT(*dt1 == *dt2);
            ASSERT(0 == ta.numDeallocations());
        }
        ASSERT(1 == ta.numDeallocations());
      } break;
#endif
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'managedPtrDeleter'
        //
        // Concerns:
        //   'managedPtrDeleter' decrement the number of references as
        //   expected.
        //
        // Plan:
        //   Call 'managedPtrDeleter' and verify the number of references
        //   change as specified.
        //
        // Testing:
        //   void managedPtrDeleter(void*, bslma::SharedPtrRep *rep);
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING 'managedPtrDeleter'"
                            "\n===========================\n");
        {
            TObj t;
            Obj& x = t;
            Obj const& X = x;

            x.managedPtrDeleter(0, &x);
            ASSERT(0 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(false == X.hasUniqueOwner());
            ASSERT(1 == t.getNumObjectDisposed());
            ASSERT(1 == t.getNumRepDisposed());
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'incrementRefs'
        //
        // Concerns:
        //   'incrementRefs' increase the number of references by the specified
        //   amount.
        //
        // Plan:
        //   Call 'incrementRefs' with different parameters and verify the
        //   number of references change as specified.
        //
        // Testing:
        //   void incrementRefs(int incrementAmount = 1);
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING 'incrementRefs'"
                            "\n=======================\n");
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'tryAcquireRef'
        //
        // Concerns:
        //   'tryAcquireRef' increments the number of shared references if
        //   'numReference' > 0 and do nothing otherwise.
        //
        // Plan:
        //   Call 'tryAcquireRef'.  Verify that 'tryAcquireRef' returns true
        //   and the 'numReference' is correct.  Call 'acquireWeakRef' and two
        //   'releaseRef', then call 'tryAcquireRef'.   Verify that
        //   'tryAcquireRef' return false and it does not change
        //   'numReference'.
        //
        // Testing:
        //   bool tryAcquireRef();
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING 'tryAcquireRef'"
                            "\n=======================\n");
        {
            TObj t;
            Obj& x = t;
            Obj const& X = x;

            ASSERT(x.tryAcquireRef());
            ASSERT(2 == X.numReferences());

            x.acquireWeakRef();
            x.releaseRef();
            x.releaseRef();

            ASSERT(!x.tryAcquireRef());
            ASSERT(0 == X.numReferences());
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'resetCountsRaw'
        //
        // Concerns:
        //   'resetCountsRaw' is setting number of shared references and weak
        //   references to the correct value.
        //
        // Plan:
        //   Call 'resetCountsRaw' with enumeration in two dimension up to two
        //   levels deep and verify the number of shared/weak references are
        //   correct.
        //
        // Testing:
        //   void resetCountsRaw(int numSharedReferences,
        //                       int numWeakReferences);
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING 'resetCountsRaw'"
                            "\n========================\n");
        {
            TObj t;
            Obj& x = t;
            Obj const& X = x;

            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < i + 1; ++j) {
                    int numRef = j;
                    int numWeakRef = i - j;
                    x.resetCountsRaw(numRef, numWeakRef);
                    LOOP2_ASSERT(numRef, numWeakRef,
                                                  numRef == X.numReferences());
                    LOOP2_ASSERT(numRef, numWeakRef,
                                          numWeakRef == X.numWeakReferences());
                    LOOP2_ASSERT(numRef, numWeakRef,
                                                0 == t.getNumObjectDisposed());
                    LOOP2_ASSERT(numRef, numWeakRef,
                                                   0 == t.getNumRepDisposed());
                }
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'disposeObject' AND 'disposeRep'
        //
        // Concerns:
        //   1) disposeObject() is called when there is no shared reference.
        //   2) disposeRep() is called only when there is no shared reference
        //   and no weak reference.
        //
        // Plan:
        //   Create an object and call 'releaseRef'.  Then verify that both
        //   'disposeObject' and 'disposeRep' is called.  Create another object
        //   and call 'acquireWeakRef' before calling 'releaseRef'.  Verify
        //   that only 'disposeObject' is called.  Then call 'releaseWeakRef'
        //   and verify that 'disposeRep' is called.
        //
        // Testing:
        //   void disposeObject();
        //   void disposeRep();
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING 'disposeObject' AND 'disposeRep'"
                            "\n========================================\n");
        {
            TObj t;
            Obj& x = t;
            Obj const& X = x;

            ASSERT(0 == t.getNumObjectDisposed());
            ASSERT(0 == t.getNumRepDisposed());

            x.releaseRef();

            ASSERT(0 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(false == X.hasUniqueOwner());
            ASSERT(1 == t.getNumObjectDisposed());
            ASSERT(1 == t.getNumRepDisposed());
        }
        {
            TObj t;
            Obj& x = t;
            Obj const& X = x;

            ASSERT(0 == t.getNumObjectDisposed());
            ASSERT(0 == t.getNumRepDisposed());

            x.acquireWeakRef();
            x.releaseRef();

            ASSERT(0 == X.numReferences());
            ASSERT(1 == X.numWeakReferences());
            ASSERT(false == X.hasUniqueOwner());
            ASSERT(1 == t.getNumObjectDisposed());
            ASSERT(0 == t.getNumRepDisposed());

            x.releaseWeakRef();
            ASSERT(0 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(1 == t.getNumRepDisposed());
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'releaseRef' AND 'releaseWeakRef'
        //
        // Concerns:
        //   'releaseRef' and 'releaseWeakRef' is decrementing the reference
        //   count correctly.
        //
        // Plan:
        //   Call 'acquireRef' then 'releaseRef' and verify 'numReference' did
        //   not change.  Call 'acquireWeakRef' then 'releaseWeakRef' and
        //   verify 'numWeakReference' did not change.
        //
        // Testing:
        //   void releaseRef();
        //   void releaseWeakRef();
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING 'releaseRef' AND 'releaseWeakRef'"
                            "\n=========================================\n");
        {
            TObj t;
            Obj& x = t;
            Obj const& X = x;

            x.acquireRef();
            x.acquireRef();
            x.releaseRef();

            ASSERT(2 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(false == X.hasUniqueOwner());

            x.releaseRef();

            ASSERT(1 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(true == X.hasUniqueOwner());

            x.acquireWeakRef();
            x.acquireWeakRef();

            x.releaseWeakRef();

            ASSERT(1 == X.numReferences());
            ASSERT(1 == X.numWeakReferences());
            ASSERT(false == X.hasUniqueOwner());

            x.releaseWeakRef();

            ASSERT(1 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(true == X.hasUniqueOwner());
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'acquireRef' AND 'acquireWeakRef'
        //
        // Concerns:
        //   'acquireRef' and 'acquireWeakRef' is incrementing the reference
        //   count correctly.
        //
        // Plan:
        //   Call 'acquireRef' and 'acquireWeakRef' and verify 'numReference'
        //   and 'numWeakReference' is incremented correctly.
        //
        // Testing:
        //   void acquireRef();
        //   void acquireWeakRef();
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING 'acquireRef' AND 'acquireWeakRef'"
                            "\n=========================================\n");
        {
            TObj t;
            Obj& x = t;
            Obj const& X = x;

            x.acquireRef();

            ASSERT(2 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(false == X.hasUniqueOwner());

            x.acquireRef();

            ASSERT(3 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(false == X.hasUniqueOwner());

            x.acquireWeakRef();

            ASSERT(3 == X.numReferences());
            ASSERT(1 == X.numWeakReferences());
            ASSERT(false == X.hasUniqueOwner());

            x.acquireWeakRef();

            ASSERT(3 == X.numReferences());
            ASSERT(2 == X.numWeakReferences());
            ASSERT(false == X.hasUniqueOwner());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING BASIC CONSTRUCTORS AND ACCESSOR
        //
        // Concerns:
        //   Object is properly initialized.
        //
        // Plan:
        //   Create an object and verify accessors are returning the expected
        //   values.
        //
        // Testing:
        //   bslma::SharedPtrRep();
        //   int numReferences() const;
        //   int numWeakReferences() const;
        //   bool hasUniqueOwner() const;
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING BASIC CONSTRUCTORS AND ACCESSOR"
                            "\n=======================================\n");

        if (verbose) printf("\nTesting default constructor"
                            "\n---------------------------\n");
        {
            TObj t;
            Obj& x = t;
            Obj const& X = x;

            ASSERT(1 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(true == X.hasUniqueOwner());
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   This test exercises basic functionality but tests nothing.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");
        {
            TObj t;
            Obj& x = t;
            Obj const& X = x;

            ASSERT(1 == X.numReferences());

            x.acquireRef();

            ASSERT(2 == X.numReferences());

            x.releaseRef();

            ASSERT(1 == X.numReferences());

            x.releaseRef();

            ASSERT(0 == X.numReferences());
            ASSERT(1 == t.getNumObjectDisposed());
            ASSERT(1 == t.getNumRepDisposed());
        }
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

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
