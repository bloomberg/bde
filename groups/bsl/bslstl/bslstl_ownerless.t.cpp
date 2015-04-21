// bslstl_ownerless.t.cpp                                             -*-C++-*-
#include <bslstl_ownerless.h>

#include <bslstl_sharedptr.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>
#include <bsls_bsltestutil.h>
#include <bsls_types.h>

#include <stdlib.h>      // atoi
#include <string.h>      // strcmp, strcpy

#undef ESP  // Solaris 10

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This test driver tests the functionality of a complex mechanism with pointer
// semantics.  It is somewhat thorough but still incomplete: among the untested
// concerns:
// - The test plans are still to be written (so marked TBD).
// - There is currently no testing of exception-safety behavior, even though
//   this is a documented concern (e.g., in the createInplace function-level
//   documentation) and there is code written specifically for avoiding memory
//   leaks in the constructors (untested).
// - The usage example is also untested.
// - Many test cases assume that the default allocator will be the NewDelete
//   allocator, and fail if a TestAllocator is installed as the default in
//   'main'.  This should be addressed as part of resolving DRQS 27411521.
//-----------------------------------------------------------------------------

// ============================================================================
//                        TEST PLAN (Additional functors)
//
// Most of these classes have trivial contracts that are almost too trivial to
// validate, such as a function-call operator to "do nothing".  The essence of
// validating these functors is that that are a valid, copy-constructible
// functor that than can be invoked with the expected arguments, and produce
// the expected observable result (if any).  In the trickier case of
// 'SharedPtrNilDeleter', it is not reasonable to check that the entire world
// has not changed, but it would be good to confirm that the object itself has
// not altered, nor the memory on the other end of the passed pointer.  The
// preferred way to do this would be to store the test object in a write-
// protected page of memory, and similarly invoke with a pointer to another
// write-protected page of memory.  Unfortunately, we do not have easy access
// to such utilities at this point in our levelized library hierarchy, so will
// settle for merely confirming that bit-patterns have not changed.
// ----------------------------------------------------------------------------
// bsl::owner_less<shared_ptr<TYPE> >
//-----------------------------------
// [ 1] bool operator()(const shared_ptr<TYPE>&, const shared_ptr<TYPE>&) const
// [ 1] bool operator()(const shared_ptr<TYPE>&, const weak_ptr<TYPE>&)   const
// [ 1] bool operator()(const weak_ptr<TYPE>&,   const shared_ptr<TYPE>&) const
//
// bsl::owner_less<weak_ptr<TYPE> >
//---------------------------------
// [ 1] bool operator()(const shared_ptr<TYPE>&, const shared_ptr<TYPE>&) const
// [ 1] bool operator()(const weak_ptr<TYPE>&,   const shared_ptr<TYPE>&) const
// [ 1] bool operator()(const weak_ptr<TYPE>&,   const weak_ptr<TYPE>&)   const

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


                             // ==================
                             // class MyTestObject
                             // ==================

class MyTestObject {
    // This class provides a test object that keeps track of how many objects
    // have been deleted.  Optionally, also keeps track of how many objects
    // have been copied.

    // DATA
    volatile bsls::Types::Int64 *d_deleteCounter_p;
    volatile bsls::Types::Int64 *d_copyCounter_p;

  public:
    // CREATORS
    MyTestObject(const MyTestObject& original);
    explicit MyTestObject(bsls::Types::Int64 *deleteCounter,
                          bsls::Types::Int64 *copyCounter = 0);
    virtual ~MyTestObject();

    // ACCESSORS
    volatile bsls::Types::Int64 *copyCounter() const;
    volatile bsls::Types::Int64 *deleteCounter() const;
};

                        // ======================
                        // class TestSharedPtrRep
                        // ======================

template <class TYPE>
class TestSharedPtrRep : public bslma::SharedPtrRep {
    // Partially implemented shared pointer representation ("letter") protocol.
    // This class provides a reference counter and a concrete implementation of
    // the 'bcema_Deleter' protocol that decrements the number references and
    // destroys itself if the number of references reaches zero.

    // DATA
    TYPE             *d_dataPtr_p;          // data ptr

    int               d_disposeRepCount;    // counter storing number of time
                                            // release is called

    int               d_disposeObjectCount; // counter storing number of time
                                            // releaseValue is called

    bslma::Allocator *d_allocator_p;        // allocator

    explicit TestSharedPtrRep(bslma::Allocator *basicAllocator);
        // Construct a test shared ptr rep object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.
        // AJM ADDED EXPLICIT, REMOVE THIS COMMENT IF NO NEW ISSUES ARISE

  public:
    // CREATORS

    TestSharedPtrRep(TYPE *dataPtr_p, bslma::Allocator *basicAllocator);
        // Construct a test shared ptr rep object owning the object pointed to
        // by the specified 'dataPtr_p' and that should be destroyed using the
        // specified 'basicAllocator'.
        // AJM CHANGING THE CONTRACT, TO SHARE TEST TYPES WITH THE ORIGINAL
        // SHARED_PTR TEST DRIVER, WHICH ALSO MORE THOROUGHLY TESTS AWKWARD
        // MULTIPLE-INHERITANCE CASES.

    ~TestSharedPtrRep();
        // Destroy this test shared ptr rep object.

    // MANIPULATORS
    virtual void disposeObject();
        // Release the value stored by this representation.

    virtual void disposeRep();
        // Release this representation.

    virtual void *getDeleter(const std::type_info&) { return 0; }
        // Return a pointer to the deleter stored by the derived representation
        // (if any) if the deleter has the same type as that described by the
        // specified 'type', and a null pointer otherwise.

    // ACCESSORS
    virtual void *originalPtr() const;
        // Return the original pointer stored by this representation.

    int disposeObjectCount() const;
        // Return the number of time 'releaseValue' was called.

    int disposeRepCount() const;
        // Return the number of time 'release' was called.

    TYPE *ptr() const;
        // Return the data pointer stored by this representation.
};

// ============================================================================
//                  MEMBER- AND FUNCTION-TEMPLATE IMPLEMENTATIONS
// ============================================================================

                             // ------------------
                             // class MyTestObject
                             // ------------------

// CREATORS
MyTestObject::MyTestObject(const MyTestObject& original)
: d_deleteCounter_p(original.d_deleteCounter_p)
, d_copyCounter_p(original.d_copyCounter_p)
{
    if (d_copyCounter_p) ++(*d_copyCounter_p);
}

MyTestObject::MyTestObject(bsls::Types::Int64 *deleteCounter,
                           bsls::Types::Int64 *copyCounter)
: d_deleteCounter_p(deleteCounter)
, d_copyCounter_p(copyCounter)
{
}

MyTestObject::~MyTestObject()
{
    ++(*d_deleteCounter_p);
}

// ACCESSORS
volatile bsls::Types::Int64* MyTestObject::copyCounter() const
{
    return d_copyCounter_p;
}

volatile bsls::Types::Int64* MyTestObject::deleteCounter() const
{
    return d_deleteCounter_p;
}

                        // ----------------------
                        // class TestSharedPtrRep
                        // ----------------------

// CREATORS
template <class TYPE>
inline
TestSharedPtrRep<TYPE>::TestSharedPtrRep(bslma::Allocator *basicAllocator)
: d_dataPtr_p(0)
, d_disposeRepCount(0)
, d_disposeObjectCount(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_dataPtr_p = new (*d_allocator_p) TYPE();
}

template <class TYPE>
inline
TestSharedPtrRep<TYPE>::TestSharedPtrRep(TYPE *dataPtr_p,
                                         bslma::Allocator *basicAllocator)
: d_dataPtr_p(dataPtr_p)
, d_disposeRepCount(0)
, d_disposeObjectCount(0)
, d_allocator_p(basicAllocator)
{
    BSLS_ASSERT_OPT(d_dataPtr_p);
    BSLS_ASSERT_OPT(basicAllocator);
}

template <class TYPE>
TestSharedPtrRep<TYPE>::~TestSharedPtrRep()
{
    LOOP_ASSERT(numReferences(),      0 == numReferences());
    LOOP_ASSERT(d_disposeRepCount,    1 == d_disposeRepCount);
    LOOP_ASSERT(d_disposeObjectCount, 1 == d_disposeObjectCount);
}

// MANIPULATORS
template <class TYPE>
inline
void TestSharedPtrRep<TYPE>::disposeObject()
{
    ++d_disposeObjectCount;
    d_allocator_p->deleteObject(d_dataPtr_p);
}

template <class TYPE>
inline
void TestSharedPtrRep<TYPE>::disposeRep()
{
    ++d_disposeRepCount;
}

// ACCESSORS
template <class TYPE>
inline
int TestSharedPtrRep<TYPE>::disposeObjectCount() const
{
    return d_disposeObjectCount;
}

template <class TYPE>
inline
int TestSharedPtrRep<TYPE>::disposeRepCount() const
{
    return d_disposeRepCount;
}

template <class TYPE>
inline
void *TestSharedPtrRep<TYPE>::originalPtr() const
{
    return static_cast<void *>(d_dataPtr_p);
}

template <class TYPE>
inline
TYPE *TestSharedPtrRep<TYPE>::ptr() const
{
    return d_dataPtr_p;
}

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

    (void)veryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    typedef bsl::shared_ptr<MyTestObject>         ObjSP;
    typedef bsl::weak_ptr<MyTestObject>           ObjWP;

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // Confirm no static intialization locekd the global allocator
    ASSERT(&globalAllocator == bslma::Default::globalAllocator());

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    // Confirm no static intialization locked the default allocator
    ASSERT(&defaultAllocator == bslma::Default::defaultAllocator());


    bslma::TestAllocator ta("general", veryVeryVerbose);

//    bsls::Types::Int64 numDeallocations;
//    bsls::Types::Int64 numAllocations;
    bsls::Types::Int64 numDeletes = 0;

//    bsls::Types::Int64 numDefaultDeallocations =
//                                         defaultAllocator.numDeallocations();
//    bsls::Types::Int64 numDefaultAllocations =
//                                           defaultAllocator.numAllocations();
    switch (test) { case 0:  // Zero is always the leading case.
    case 1: {
      // --------------------------------------------------------------------
      // TEST 'owner_less' FUNCTOR
      //
      // Concerns:
      //   Test that the 'owner_less' functor works as expected.
      //
      // Plan:
      //   Create two shared pointer representation objects, with a known
      //   relationship between their addresses.  Then create shared and weak
      //   ptr objects from these representations, and confirm the correct
      //   runtime behavior when invoking the function call operator of the
      //   'owner_less' functor.
      //
      // Testing:
      //  bsl::owner_less<shared_ptr<T> >::
      //   bool operator()(const shared_ptr<T>&, const shared_ptr<T>&) const
      //   bool operator()(const shared_ptr<T>&, const weak_ptr<T>&)   const
      //   bool operator()(const weak_ptr<T>&,   const shared_ptr<T>&) const
      //
      //  bsl::owner_less<weak_ptr<TYPE> >::
      //   bool operator()(const shared_ptr<T>&, const shared_ptr<T>&) const
      //   bool operator()(const weak_ptr<T>&,   const shared_ptr<T>&) const
      //   bool operator()(const weak_ptr<T>&,   const weak_ptr<T>&)   const
      // --------------------------------------------------------------------

      if (verbose) printf("\nTESTING 'owner_less' FUNCTOR"
                          "\n============================\n");

      {
//          typedef BloombergLP::bslma::SharedPtrInplaceRep<MyTestObject> Rep;
//          Rep *rep = new(ta) Rep(&ta, &numDeletes);
//          shared_ptr<MyTestObject> example(rep->ptr(), rep);


          bslma::TestAllocator ta;
          MyTestObject *REP_PTR1 = new(ta) MyTestObject(&numDeletes);
          TestSharedPtrRep<MyTestObject> rep1(REP_PTR1, &ta);
          const TestSharedPtrRep<MyTestObject>& REP1 = rep1;

          MyTestObject *REP_PTR2 = new(ta) MyTestObject(&numDeletes);
          TestSharedPtrRep<MyTestObject> rep2(REP_PTR2, &ta);
          const TestSharedPtrRep<MyTestObject>& REP2 = rep2;

          MyTestObject *PTR1 = REP1.ptr();
          MyTestObject *PTR2 = REP2.ptr();
          ASSERTV(REP_PTR1, PTR1,        REP_PTR1 == PTR1);
          ASSERTV(REP_PTR2, PTR2,        REP_PTR2 == PTR2);
          {
              const ObjWP EWP1;
              const ObjWP EWP2;
              const ObjSP ESP;

              bsl::owner_less<ObjSP> fnSP = {};
              const bsl::owner_less<ObjSP>& FN_SP = fnSP;

              bsl::owner_less<ObjWP> fnWP = {};
              const bsl::owner_less<ObjWP>& FN_WP = fnWP;

              ObjSP mSA(PTR1, &rep1); const ObjSP& SA = mSA;
              ObjSP mSB(PTR2, &rep2); const ObjSP& SB = mSB;

              ObjWP mWA(SA); const ObjWP& WA = mWA;
              ObjWP mWB(SB); const ObjWP& WB = mWB;

              ASSERT(false == FN_WP(EWP1, EWP1));
              ASSERT(false == FN_WP(EWP1, EWP2));
              ASSERT(false == FN_WP(EWP1, ESP));
              ASSERT(true  == FN_WP(EWP1, SA));
              ASSERT(true  == FN_WP(EWP1, WA));
              ASSERT(true  == FN_WP(EWP1, SB));
              ASSERT(true  == FN_WP(EWP1, WB));

              ASSERT(false == FN_WP(WA, EWP1));
              ASSERT(false == FN_WP(WA, ESP));
              ASSERT(false == FN_WP(WA, SA));
              ASSERT(false == FN_WP(WA, WA));
              ASSERT(FN_WP(WA, SB) == (&REP1 < &REP2));
              ASSERT(FN_WP(WA, WB) == (&REP1 < &REP2));

              ASSERT(false == FN_SP(SA, EWP1));
              ASSERT(false == FN_SP(SA, ESP));
              ASSERT(false == FN_SP(SA, SA));
              ASSERT(false == FN_SP(SA, WA));
              ASSERT(FN_SP(SA, SB) == (&REP1 < &REP2));
              ASSERT(FN_SP(SA, WB) == (&REP1 < &REP2));
          }
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
// Copyright 2014 Bloomberg Finance L.P.
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
