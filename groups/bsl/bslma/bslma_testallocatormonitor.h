// bslma_testallocatormonitor.h                                       -*-C++-*-
#ifndef INCLUDED_BSLMA_TESTALLOCATORMONITOR
#define INCLUDED_BSLMA_TESTALLOCATORMONITOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a mechanism to summarize 'bslma::TestAllocator' object use.
//
//@CLASSES:
//  bslma::TestAllocatorMonitor: 'bslma::TestAllocator' summary mechanism
//
//@SEE_ALSO: bslma_testallocator
//
//@DESCRIPTION: This component provides a single mechanism class,
// 'bslma::TestAllocatorMonitor', which is used, in concert with
// 'bslma::TestAllocator', in the implementation of test drivers.  The
// 'bslma::TestAllocatorMonitor' class provides boolean accessors indicating
// whether associated test allocator state has changed (or not) since
// construction of the monitor.  Using 'bslma::TestAllocatorMonitor' objects
// often result in test cases that are more concise, easier to read, and less
// error prone than test cases that directly access the test allocator for
// state information.
//
///Statistics
///----------
// The test allocator statistics tracked by the test allocator monitor along
// with the boolean accessors used to observe a change in those statistics are
// shown in the table below.  The change (or lack of change) reported by these
// accessors are relative to the value of the test allocator statistic at the
// construction of the monitor.  Note that each of these statistics count
// blocks of memory (i.e., number of allocations from the allocator), and do
// not depend on the number of bytes in those allocated blocks.
//..
//  Statistic        Is-Same Method Is-Up Method Is-Down Method
//  --------------   -------------- ------------ --------------
//  numBlocksInUse   isInUseSame    isInUseUp    isInUseDown
//  numBlocksMax     isMaxSame      isMaxUp      none
//  numBlocksTotal   isTotalSame    isTotalUp    none
//..
// The 'numBlocksMax' and 'numBlocksTotal' statistics have values that are
// monotonically non-decreasing; hence, they need no "Is-Down" methods.  Note
// that if a monitor is created for an allocator with outstanding blocks ("in
// use"), then it is possible for the allocator's count of outstanding blocks
// to drop below the value seen by the monitor at construction.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Standard Usage
///- - - - - - - - - - - - -
// Classes taking 'bslma::allocator' objects have many requirements (and thus,
// many testing concerns) that other classes do not.  Here we illustrate how
// 'bslma::TestAllocatorMonitor' objects (in conjunction with
// 'bslma::TestAllocator' objects) can be used in a test driver to succinctly
// address many concerns of an object's use of allocators.
//
// First, for a test subject, we introduce 'MyClass', an unconstrained
// attribute class having a single, null-terminated ascii string attribute,
// 'description'.  For the sake of brevity, 'MyClass' defines only a default
// constructor, a primary manipulator (the 'setDescription' method), and a
// basic accessor (the 'description' method).  These suffice for the purposes
// of these example.  Note that a proper attribute class would also implement
// value and copy constructors, 'operator==', an accessor for the allocator,
// and other methods.
//..
//  class MyClass {
//      // This unconstrained (value-semantic) attribute class has a single,
//      // null-terminated ascii string attribute, 'description'.
//
//      // DATA
//      size_t            d_capacity;      // available memory
//      char             *d_description_p; // string data
//      bslma::Allocator *d_allocator_p;   // held, not owned
//
//    public:
//      // CREATORS
//      explicit MyClass(bslma::Allocator *basicAllocator = 0);
//          // Create a 'MyClass' object having the (default) attribute values:
//          //..
//          //  description() == ""
//          //..
//          // Optionally specify a 'basicAllocator' used to supply memory.  If
//          // 'basicAllocator' is 0, the currently installed default allocator
//          // is used.
//
//      ~MyClass();
//          // Destroy this object.
//
//      // MANIPULATORS
//      void setDescription(const char *value);
//          // Set the null-terminated ascii string 'description' attribute of
//          // this object to the specified 'value'.  On completion, the
//          // 'description' method returns the address of a copy of the ascii
//          // string at 'value'.
//
//      // ACCESSORS
//      const char *description() const;
//          // Return the value of the null-terminated ascii string
//          // 'description' attribute of this object.
//  };
//
//  // ========================================================================
//  //                      INLINE FUNCTION DEFINITIONS
//  // ========================================================================
//
//                          // -------------
//                          // class MyClass
//                          // -------------
//
//  // CREATORS
//  inline
//  MyClass::MyClass(bslma::Allocator *basicAllocator)
//  : d_capacity(0)
//  , d_description_p(0)
//  , d_allocator_p(bslma::Default::allocator(basicAllocator))
//  {
//  }
//
//  inline
//  MyClass::~MyClass()
//  {
//      BSLS_ASSERT_SAFE(0 <= d_capacity);
//
//      d_allocator_p->deallocate(d_description_p);
//  }
//
//  // MANIPULATORS
//  inline
//  void MyClass::setDescription(const char *value)
//  {
//      BSLS_ASSERT_SAFE(value);
//
//      size_t size = std::strlen(value) + 1;
//      if (size > d_capacity) {
//          char *newMemory = (char *) d_allocator_p->allocate(size);
//          d_allocator_p->deallocate(d_description_p);
//          d_description_p = newMemory;
//          d_capacity      = size;
//
//      }
//      std::memcpy(d_description_p, value, size);
//  }
//..
// Notice that the implementation of the manipulator allocates/deallocates
// memory *before* updating the object.  This ordering leaves the object
// unchanged in case the allocator throws an exception (part of the strong
// exception guarantee).  This is an implementation detail, not a part of the
// contract (in this example).
//..
//
//  // ACCESSORS
//  inline
//  const char *MyClass::description() const
//  {
//      return d_description_p ? d_description_p : "";
//  }
//..
// Then, we design a test-driver for 'MyClass'.  Our allocator-related concerns
// for 'MyClass' include:
//..
// Concerns:
//: 1 Any memory allocation is from the object allocator.
//:
//: 2 Every object releases any allocated memory at destruction.
//:
//: 3 No accessor allocates any memory.
//:
//: 4 All memory allocation is exception-neutral.
//:
//: 5 QoI: The default constructor allocates no memory.
//:
//: 6 QoI: When possible, memory is cached for reuse.
//..
// Notice that some of these concerns (e.g., C-5..6) are not part of the
// class's documented, contractual behavior.  These are classified as Quality
// of Implementation (QoI) concerns.
//
// Next, we define a test plan.  For example, a plan to test these concerns is:
//..
// Plan:
//: 1 Setup global and default allocators:
//:
//:   1 Create two 'bslma::TestAllocator' objects and, for each of these,
//:     create an associated 'bslma::TestAllocatorMonitor' object.
//:
//:   2 Install the two allocators as the global and default allocators.
//:
//: 2 Confirm that default construction allocates no memory: (C-5)
//:
//:   1 Construct a 'bslma::TestAllocatorMonitor' object to be used passed to
//:     test objects on their construction, and an associated
//:
//:   2 In an inner block, default construct an object of 'MyClass' using the
//:     designated "object" test allocator.
//:
//:   3 Allow the object to go out of scope (destroyed).  Confirm that no
//:     memory has been allocated from any of the allocators.
//:
//: 3 Exercise an object of 'MyClass' such that memory should be allocated, and
//:   then confirm that the object allocator (only) is used: (C-2..4,6)
//:
//:   1 In another inner block, default construct a new test object using the
//:     (as yet unused) object allocator.
//:
//:   2 Force the test object to allocate memory by setting its 'descriptor'
//:     attribute to a value whose size exceeds the size of the object itself.
//:     Confirm that the attribute was set and that memory was allocated.
//:
//:   3 Confirm the the primary manipulator (the 'setDescription' method) is
//:     exception-neutral (i.e., exceptions from the allocator are propagated
//:     and no memory is leaked).  Use the
//:     'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros to manage the test,
//:     and use the test allocator monitor to confirm that memory is allocated
//:     on the no-exception code path.  (C-4)
//:
//:   4 When the object is holding memory, create an additional test allocator
//:     monitor allocator for the object allocator.  Use the basic accessor
//:     (i.e., the 'description' method) to confirm that the object has the
//:     expected value.  Check this test allocator monitor to confirm that
//:     accessor allocated no memory.  (C-3)
//:
//:   5 Change the attribute to a smaller value and confirm that the current
//:     memory was reused (i.e., no memory is allocated). (C-6)
//:
//:   6 Destroy the test object by allowing it to go out of scope, and confirm
//:     that all allocations are returned.  (C-2)
//:
//: 4 Confirm that at no time were the global allocator or the default
//:   allocator were used.  (C-1)
//..
// The implementation of the plan is shown below:
//
// Then, we implement the first portion of the plan.  We create the trio of
// test allocators, their respective test allocator monitors, and install two
// of the allocators as the global and default allocators:
//..
//  {
//      if (verbose) cout << "Setup global and default allocators" << endl;
//
//      bslma::TestAllocator        ga("global",  veryVeryVeryVerbose);
//      bslma::TestAllocator        da("default", veryVeryVeryVerbose);
//      bslma::TestAllocatorMonitor gam(&ga);
//      bslma::TestAllocatorMonitor dam(&da);
//
//      bslma::Default::setGlobalAllocator(&ga);
//      bslma::Default::setDefaultAllocatorRaw(&da);
//..
// Then, we default construct a test object using the object allocator, and
// then, immediately destroy it.  The object allocator monitor, 'oam', shows
// that the allocator was not used.
//..
//  if (verbose) cout << "No allocation by Default Constructor " << endl;
//
//  bslma::TestAllocator        oa("object", veryVeryVeryVerbose);
//  bslma::TestAllocatorMonitor oam(&oa);
//
//  {
//      MyClass obj(&oa);
//      assert(oam.isTotalSame()); // object allocator unused
//  }
//..
// Next, we pass the (still unused) object allocator to another test object.
// This time, we coerce the object into allocating memory by setting an
// attribute.  (Setting an attribute larger than the receiving object means
// that the object cannot store the data within its own footprint and must
// allocate memory.)
//..
//  if (verbose) cout << "Exercise object" << endl;
//
//  {
//      MyClass obj(&oa);
//
//      const char DESCRIPTION1[]="abcdefghijklmnopqrstuvwyz"
//                                "abcdefghijklmnopqrstuvwyz";
//      assert(sizeof(obj) < sizeof(DESCRIPTION1));
//
//      if (veryVerbose) cout << "\tPrimary Manipulator Allocates" << endl;
//
//      BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
//          if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }
//
//          obj.setDescription(DESCRIPTION1);
//          assert(oam.isTotalUp());  // object allocator was used
//          assert(oam.isInUseUp());  // some outstanding allocation(s)
//          assert(oam.isMaxUp());    // a maximum was set
//      } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
//..
// Notice, as expected, memory was allocated from object allocator.
//
// Now that the allocator has been used, we create a second monitor to capture
// the that state.  Confirm that the basic accessor (the 'description' method)
// does not use the allocator.
//..
//  if (veryVerbose) cout << "\tBasic Accessor does not allocate"
//                        << endl;
//
//  bslma::TestAllocatorMonitor oam2(&oa); // Captures state of 'oa'
//                                        // with outstanding
//                                        // allocations.
//
//  assert(0 == strcmp(DESCRIPTION1, obj.description()));
//  assert(oam2.isTotalSame());  // object allocator was not used
//..
// Next, confirm that when a shorter value is assigned, the existing memory is
// reused.
//..
//
//  obj.setDescription("a");
//  assert(0 == std::strcmp("a", obj.description()));
//
//  assert(oam2.isTotalSame());  // no allocations
//..
// Notice that there are no allocations because the object had sufficient
// capacity in previously allocated memory to store the short string.
//
// Next, as an additional test, we make the object allocate additional memory
// by setting a longer attribute: one that exceeds the capacity allocated for
// 'DESCRIPTION1'.  Use the second monitor to confirm that an allocation was
// performed.
//
// There are tests where using a test allocator monitor does not suffice.  Our
// test object is currently holding memory, if we assign a value that exceeds
// its current capacity there will be two operations on the object allocator:
// the allocation of larger memory, and the deallocation of its current memory:
// in that order, as part of the strong exception guarantee.  Thus, the maximum
// number of allocations should go up by one, and no more.
//
// Note that absence of memory leaks due to exceptions (the other part of the
// strong exception guarantee is confirmed during the destruction of the object
// test allocator at the end of this test, which featured exceptions.
//..
//  bsls::Types::Int64 maxBeforeSet   = oa.numBlocksMax();
//  const char        DESCRIPTION2[] = "abcdefghijklmnopqrstuvwyz"
//                                     "abcdefghijklmnopqrstuvwyz"
//                                     "abcdefghijklmnopqrstuvwyz"
//                                     "abcdefghijklmnopqrstuvwyz"
//                                     "abcdefghijklmnopqrstuvwyz";
//  assert(sizeof(DESCRIPTION1) < sizeof(DESCRIPTION2));
//
//  obj.setDescription(DESCRIPTION2);
//  assert(0 == std::strcmp(DESCRIPTION2, obj.description()));
//
//  assert(oam2.isTotalUp());    // The object allocator used.
//
//  assert(oam2.isInUseSame());  // The outstanding block (allocation)
//                               // count unchanged (even though byte
//                               // outstanding byte count increased).
//
//  assert(oam2.isMaxUp());      // Max increased as expected, but was
//                               // did it change only by one?  The
//                               // monitor cannot answer that
//                               // question.
//
//  bsls::Types::Int64 maxAfterSet = oa.numBlocksMax();
//
//  assert(1 == maxAfterSet - maxBeforeSet);
//..
// Notice that our test allocator monitor cannot confirm that the allocator's
// maximum increased by exactly one.  In this case, we must extract our
// statistics directly from the test allocator.
//
// Note that increment in "max" occurs only the first time through the the
// allocate/deallocate scenario in 'setDescription'.
//..
//  bslma::TestAllocatorMonitor oam3(&oa);
//
//  const char DESCRIPTION3[] = "abcdefghijklmnopqrstuvwyz"
//                              "abcdefghijklmnopqrstuvwyz"
//                              "abcdefghijklmnopqrstuvwyz"
//                              "abcdefghijklmnopqrstuvwyz"
//                              "abcdefghijklmnopqrstuvwyz"
//                              "abcdefghijklmnopqrstuvwyz"
//                              "abcdefghijklmnopqrstuvwyz"
//                              "abcdefghijklmnopqrstuvwyz"
//                              "abcdefghijklmnopqrstuvwyz";
//  assert(sizeof(DESCRIPTION2) < sizeof(DESCRIPTION3));
//
//  obj.setDescription(DESCRIPTION3);
//  assert(0 == std::strcmp(DESCRIPTION3, obj.description()));
//
//  assert(oam3.isTotalUp());    // The object allocator used.
//
//  assert(oam3.isInUseSame());  // The outstanding block (allocation)
//                               // count unchanged (even though byte
//                               // outstanding byte count increased).
//
//  assert(oam3.isMaxSame());    // A repeat of the scenario for
//                               // 'DESCRIPTION2', so no change in the
//                               // allocator's maximum.
//..
// Now, we close scope and check that all object memory was deallocated
//..
//  }
//
//  if (veryVerbose) cout << "\tAll memory returned object allocator"
//                        << endl;
//
//  assert(oam.isInUseSame());
//..
// Finally, we check that none of these operations used the default or global
// allocators.
//..
//      if (verbose) cout << "Global and Default allocators never used"
//                        << endl;
//
//      assert(gam.isTotalSame());
//      assert(dam.isTotalSame());
//  }
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_TESTALLOCATOR
#include <bslma_testallocator.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {

namespace bslma {

                        // ==========================
                        // class TestAllocatorMonitor
                        // ==========================

class TestAllocatorMonitor {
    // This mechanism provides a set of boolean accessor methods indicating
    // whether a change has occurred in the state of the 'TestAllocator' object
    // (supplied at construction) since the construction of the monitor.  See
    // the Statistics section of @DESCRIPTION for the statics tracked.

    // DATA
    bsls::Types::Int64   d_initialInUse;    // 'numBlocksInUse'
    bsls::Types::Int64   d_initialMax;      // 'numBlocksMax'
    bsls::Types::Int64   d_initialTotal;    // 'numBlocksTotal'
    const TestAllocator *d_testAllocator_p; // held, not owned

    // PRIVATE CLASS METHODS
    static const TestAllocator *validateArgument(
                                               const TestAllocator *allocator);
        // Return the specified 'allocator', and, if compiled in "SAFE" mode,
        // assert that 'allocator' is not 0.  Note that this static function is
        // needed to perform validation on the allocator address supplied at
        // construction, prior to that address being dereferenced to initialize
        // the 'const' data members of this type.

  public:
    // CREATORS
    explicit TestAllocatorMonitor(const TestAllocator *testAllocator);
        // Create a 'TestAllocatorMonitor' object to track changes in
        // statistics of the specified 'testAllocator'.

    ~TestAllocatorMonitor();
        // Destroy this object.

    // MANIPULATOR
    void reset(const bslma::TestAllocator *testAllocator = 0);
        // Change the allocator monitored by this object to the specified
        // 'testAllocator' and initialize the allocator properties monitored by
        // this object to the current state of 'testAllocator'.  If no
        // 'testAllocator' is passed, do not modify the allocator held by this
        // object and re-initialize the allocator properties monitored by this
        // object to the current state of that allocator.

    // ACCESSORS
    bool isInUseDown() const;
        // Return 'true' if the 'numBlocksInUse' statistic of the tracked test
        // allocator has decreased since construction of this monitor, and
        // 'false' otherwise.

    bool isInUseSame() const;
        // Return 'true' if the 'numBlocksInUse' statistic of the tracked test
        // allocator has not changed since construction of this monitor, and
        // 'false' otherwise.

    bool isInUseUp() const;
        // Return 'true' if the 'numBlocksInUse' statistic of the tracked test
        // allocator has increased since construction of this monitor, and
        // 'false' otherwise.

    bool isMaxSame() const;
        // Return 'true' if the 'numBlocksMax' statistic of the tracked test
        // allocator has not changed since construction of this monitor, and
        // 'false' otherwise.

    bool isMaxUp() const;
        // Return 'true' if the 'numBlocksMax' statistic of the tracked test
        // allocator has increased since construction of this monitor, and
        // 'false' otherwise.

    bool isTotalSame() const;
        // Return 'true' if the 'numBlocksTotal' statistic of the tracked test
        // allocator has not changed since construction of this monitor, and
        // 'false' otherwise.

    bool isTotalUp() const;
        // Return 'true' if the 'numBlocksTotal' statistic of the tracked test
        // allocator has increased since construction of this monitor, and
        // 'false' otherwise.

    bsls::Types::Int64 numBlocksInUseChange() const;
        // Return the change in the 'numBlocksInUse' statistic of the tracked
        // test allocator since construction of this monitor.

    bsls::Types::Int64 numBlocksMaxChange() const;
        // Return the change in the 'numBlocksMax' statistic of the tracked
        // test allocator since construction of this monitor.

    bsls::Types::Int64 numBlocksTotalChange() const;
        // Return the change in the 'numBlocksTotal' statistic of the tracked
        // test allocator since construction of this monitor.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // --------------------------
                        // class TestAllocatorMonitor
                        // --------------------------

// CLASS METHODS
inline
const TestAllocator *
TestAllocatorMonitor::validateArgument(const TestAllocator *allocator)
{
    BSLS_ASSERT_SAFE(allocator);

    return allocator;
}

// MANIPULATOR
inline
void TestAllocatorMonitor::reset(const TestAllocator *testAllocator)
{
    // This method is called inline by c'tor, hence it should precede it.

    if (testAllocator) {
        d_testAllocator_p = testAllocator;
    }

    d_initialInUse = d_testAllocator_p->numBlocksInUse();
    d_initialMax   = d_testAllocator_p->numBlocksMax();
    d_initialTotal = d_testAllocator_p->numBlocksTotal();

    BSLS_ASSERT_SAFE(0 <= d_initialMax);
    BSLS_ASSERT_SAFE(0 <= d_initialTotal);
}

// CREATORS
inline
TestAllocatorMonitor::TestAllocatorMonitor(const TestAllocator *testAllocator)
: d_testAllocator_p(testAllocator)
{
    BSLS_ASSERT_SAFE(d_testAllocator_p);

    reset();
}

}  // close package namespace

namespace bslma {

inline
TestAllocatorMonitor::~TestAllocatorMonitor()
{
    BSLS_ASSERT_SAFE(d_testAllocator_p);
    BSLS_ASSERT_SAFE(0 <= d_initialMax);
    BSLS_ASSERT_SAFE(0 <= d_initialTotal);
}

}  // close package namespace

namespace bslma {

// ACCESSORS
inline
bool TestAllocatorMonitor::isInUseDown() const
{
    return d_testAllocator_p->numBlocksInUse() < d_initialInUse;
}

inline
bool TestAllocatorMonitor::isInUseSame() const
{
    return d_testAllocator_p->numBlocksInUse() == d_initialInUse;
}

inline
bool TestAllocatorMonitor::isInUseUp() const
{
    return d_testAllocator_p->numBlocksInUse() > d_initialInUse;
}

inline
bool TestAllocatorMonitor::isMaxSame() const
{
    return d_initialMax == d_testAllocator_p->numBlocksMax();
}

inline
bool TestAllocatorMonitor::isMaxUp() const
{
    return d_testAllocator_p->numBlocksMax() != d_initialMax;
}

inline
bool TestAllocatorMonitor::isTotalSame() const
{
    return d_testAllocator_p->numBlocksTotal() == d_initialTotal;
}

inline
bool TestAllocatorMonitor::isTotalUp() const
{
    return d_testAllocator_p->numBlocksTotal() != d_initialTotal;
}

inline
bsls::Types::Int64 TestAllocatorMonitor::numBlocksInUseChange() const
{
    return d_testAllocator_p->numBlocksInUse() - d_initialInUse;
}

inline
bsls::Types::Int64 TestAllocatorMonitor::numBlocksMaxChange() const
{
    return d_testAllocator_p->numBlocksMax() - d_initialMax;
}

inline
bsls::Types::Int64 TestAllocatorMonitor::numBlocksTotalChange() const
{
    return d_testAllocator_p->numBlocksTotal() - d_initialTotal;
}

}  // close package namespace


}  // close enterprise namespace

#endif

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
