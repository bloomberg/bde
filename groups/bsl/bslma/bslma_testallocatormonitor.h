// bslma_testallocatormonitor.h                                       -*-C++-*-
#ifndef INCLUDED_BSLMA_TESTALLOCATORMONITOR
#define INCLUDED_BSLMA_TESTALLOCATORMONITOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a mechanism to summarize 'bslma_TestAllocator' object use.
//
//@CLASSES:
//  bslma_TestAllocatorMonitor: summarizes 'bslma_TestAllocator' use
//
//@AUTHOR: Rohan Bhindwale (rbhindwale), Steven Breitstein (sbreitstein)
//
//@SEE_ALSO: bslma_testallocator
//
//@DESCRIPTION: This component provides a single mechanism class,
// 'bslma_TestAllocatorMonitor', which is used, in concert with
// 'bslma_TestAllocator', in the implementation of test drivers.  The
// 'bslma_TestAllocatorMonitor' class provides boolean accessors indicating
// whether associated test allocator state has changed (or not) since
// construction of the monitor.  Using 'bslma_TestAllocatorMonitor' objects
// results in test cases that are more concise, easier to read, and less error
// prone than test cases that directly access the test allocator for state
// information.
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
// that if a monitor is created for an allocator with outstanding blocks, then
// it is possible for the allocator's count of outstanding blocks to drop below
// the value seen by the monitor at construction.
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Standard Usage
///- - - - - - - - - - - - -
// Classes taking 'bslma_allocator' objects have many requirements (and thus,
// many testing concerns) that other classes do not.  We here illustrate how
// 'bslma_TestAllocatorMonitor' objects (in conjunction with
// 'bslma_TestAllocator' objects) can be used in a test driver to succinctly
// address many concerns of an object's use of allocators.
//
// First, for a test subject, we introduce 'MyClass', an unconstrained
// attribute class with a single attribute, 'description', an ascii string.
// For the sake of brevity, 'MyClass' defines only a default constructor, a
// primary manipulator (the 'setDescription' method), and a basic accessor (the
// 'description' method).  These suffice for the purposes of these example.
// Note that proper attribute class would also implement value and copy
// constructors, 'operator==', and other methods.
//..
//  class MyClass {
//      // This unconstrained (value-semantic) attribute class has a single
//      // attribute, 'description', an ascii string.
//
//      // DATA
//      int              d_capacity;      // available memory
//      char            *d_description_p; // string data
//      bslma_Allocator *d_allocator_p;   // held, not owned
//
//    public:
//      // CREATORS
//      explicit MyClass(bslma_Allocator *basicAllocator = 0);
//          // Create a 'MyClass' object having the (default) attribute values:
//          //..
//          // description() == ""
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
//          // Set the 'description' attribute of this object the specified
//          // 'value'.  On completion, the 'description' method returns the
//          // address of a copy of the ascii string at 'value'.
//
//      // ACCESSORS
//      const char *description() const;
//          // Return an address providing non-modifiable access to the
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
//  MyClass::MyClass(bslma_Allocator *basicAllocator)
//  : d_capacity(0)
//  , d_description_p(0)
//  , d_allocator_p(bslma_Default::allocator(basicAllocator))
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
//      int length = std::strlen(value);
//      int size   = length + 1;
//      if (size > d_capacity) {
//          d_allocator_p->deallocate(d_description_p);
//          d_description_p = 0; // lest 'allocate' throws
//          d_capacity      = 0; // lest 'allocate' throws
//          d_description_p = (char *) d_allocator_p->allocate(size);
//          d_capacity      = size;
//      }
//      std::memcpy(d_description_p, value, length);
//      d_description_p[length] = '\0';
//  }
//
//  // ACCESSORS
//  inline
//  const char *MyClass::description() const
//  {
//      return d_description_p ? d_description_p : "";
//  }
//..
//
// Then, we consider the test-driver for 'MyClass'.  Our allocator-related
// concerns for 'MyClass' include:
//..
// Concerns:
//: 1 Any memory allocation is from the object allocator.
//:
//: 2 There is no temporary allocation from any allocator.
//:
//: 3 Every object releases any allocated memory at destruction.
//:
//: 4 QoI: The default constructor allocates no memory.
//:
//: 5 QoI: When possible, memory is cached for reuse.
//..
// Notice that some of these concerns (e.g., C-4, C-5) are not part of the
// class's documented, contractual behavior.  Hence, they are tagged a Quality
// of Implementation (QoI) concerns.
//
// Next, we define a test plan.  For example, a plan addressing these concerns
// is:
//..
// Plan:
//: 1 Create three 'bslma_TestAllocator' objects and for each of these a
//:   'bslma_TestAllocatorMonitor' object.  Install two allocators as the
//:   global and default allocators.  The remaining allocator will be
//:   passed to our test objects on construction.
//:
//: 2 In an inner block, default create a test object using the designated
//:   "object" allocator.  Then allow the object to go out of scope
//:   (destroyed).  Confirm that no memory has been allocated from any of the
//:   allocators.  (C-4).
//:
//: 3 Exercise a test object so that memory is allocated allocation, and and
//:   confirm that the object allocator (only) is used as expected:
//:   1 Create a new test object using the (as yet unused) object allocator.
//:   2 Force the test object to allocate memory by setting an attribute that
//:     exceeds the size of the object itself.  Confirm that the attribute was
//:     set and that memory was allocated.
//:   3 Change the attribute to a smaller value and confirm that the current
//:     memory was reused. (C-5)
//:   4 Force the test object to deallocate/allocate by setting a string
//:     that exceeds the capacity acquired earlier.  Confirm that the
//:     that the number of outstanding allocations is unchanged (one returned,
//:     one given) but the maximum number of allocations is unchanged so there
//:     were not extra (temporary) allocations. (C-2)
//:
//: 4 Destroy the test object and confirm that all allocations are returned.
//:   (C-3)
//: 5 Confirm that at no time were the global allocator or the default
//:   allocator were used. (C-1)
//..
// The implementation of the plan is shown below:
//
// Then, we implement the first portion of the plan.  We create the trio of
// test allocators, their respective test allocator monitors, and install two
// of the allocators as the global and default allocators:
//..
//  {
//      bslma_TestAllocator ga("global",  veryVeryVeryVerbose);
//      bslma_TestAllocator da("default", veryVeryVeryVerbose);
//      bslma_TestAllocator oa("object",  veryVeryVeryVerbose);
//
//      bslma_TestAllocatorMonitor gam(ga), dam(da), oam(oa);
//
//      bslma_Default::setGlobalAllocator(&ga);
//      bslma_Default::setDefaultAllocatorRaw(&da);
//..
// Then, we default construct a test object using the object allocator, and
// then, immediately destroy it.  The object allocator monitor, 'oam', shows
// that the allocator was not used.
//..
//      {
//          MyClass obj(&oa);
//      }
//      assert(oam.isTotalSame()); // object  allocator unused
//..
// Next, we pass the (still unused) object allocator to another test object.
// This time, we coerce the object into allocating memory by setting an
// attribute.  (Setting an attribute larger than the receiving object usually
// means that the object cannot store the data within its own footprint and
// must allocate memory.)
//..
//      {
//          MyClass obj(&oa);
//
//          const char DESCRIPTION1[]="abcdefghijklmnopqrstuvwyz"
//                                    "abcdefghijklmnopqrstuvwyz";
//          BSLMF_ASSERT(sizeof(obj) < sizeof(DESCRIPTION1));
//
//          obj.setDescription(DESCRIPTION1);
//          assert(0 == std::strcmp(DESCRIPTION1, obj.description()));
//
//          assert(oam.isTotalUp());  // object allocator was used
//          assert(oam.isInUseUp());  // some outstanding allocation(s)
//          assert(oam.isMaxUp());    // a maximum was set
//..
// Notice, as expected, memory was allocated from object allocator.
//
// Then, we create a second monitor to capture the current state of the test
// allocator, and reset the attribute of that same object, this time to a short
// string.
//..
//          bslma_TestAllocatorMonitor oam2(oa);
//
//          obj.setDescription("a");
//          assert(0 == std::strcmp("a", obj.description()));
//
//          assert(oam2.isTotalSame());  // no allocations
//..
// Notice that there are no allocations because the object had sufficient
// capacity in previously allocated memory to store the short string.
//
// Next, we make the object allocate additional memory by setting a longer
// attribute: one that exceeds the capacity allocated for 'DESCRIPTION1'.  Use
// the second monitor to confirm that an allocation was performed.
//..
//          const char DESCRIPTION2[]="abcdefghijklmnopqrstuvwyz"
//                                    "abcdefghijklmnopqrstuvwyz"
//                                    "abcdefghijklmnopqrstuvwyz"
//                                    "abcdefghijklmnopqrstuvwyz";
//          obj.setDescription(DESCRIPTION2);
//          assert(0 == std::strcmp(DESCRIPTION2, obj.description()));
//
//          assert(oam2.isTotalUp());    // object allocator used
//          assert(oam2.isInUseSame());  // outstanding block (allocation)
//                                       // count unchanged (even though byte
//                                       // outstanding byte count increased)
//          assert(oam2.isMaxSame());    // no extra (temporary) allocations
//      }
//..
// Notice that the number of outstanding allocations remained unchanged: one
// block was deallocated, one block (a larger block) was allocated.  Moreover,
// the lack of change in the maximum of outstanding blocks ('isMaxSame') shows
// there were no extra (temporary) allocations.
//
// Finally, check that none of these operations used the default or global
// allocators.
//..
//      assert(oam.isInUseSame()); // All object allocator memory deallocated.
//      assert(gam.isTotalSame()); // Global  allocator was never used.
//      assert(dam.isTotalSame()); // Default allocator was never used.
//  }
//..

#ifndef INCLUDED_BSLMA_TESTALLOCATOR
#include <bslma_testallocator.h>
#endif

namespace BloombergLP {

                        // ================================
                        // class bslma_TestAllocatorMonitor
                        // ================================

class bslma_TestAllocatorMonitor {
    // This mechanism provides a set of boolean accessor methods indicating
    // whether a change has occurred since the construction of the monitor in
    // the state of the 'bslma_TestAllocator' object supplied at construction.
    // See the Statistics section of @DESCRIPTION for the statics tracked.

    // DATA
    const bsls_Types::Int64          d_initialInUse;    // 'numBlocksInUse'
    const bsls_Types::Int64          d_initialMax;      // 'numBlocksMax'
    const bsls_Types::Int64          d_initialTotal;    // 'numBlocksTotal'
    const bslma_TestAllocator *const d_testAllocator_p; // held, not owned

  public:
    // CREATORS
    explicit bslma_TestAllocatorMonitor(
                                     const bslma_TestAllocator& testAllocator);
        // Create a 'bslma_TestAllocatorMonitor' object to track changes in
        // statistics of the specified 'testAllocator'.

    //! ~bslma_TestAllocatorMonitor() = default;
        // Destroy this object.

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
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // --------------------------------
                        // class bslma_TestAllocatorMonitor
                        // --------------------------------

// CREATORS
inline
bslma_TestAllocatorMonitor::bslma_TestAllocatorMonitor(
                                      const bslma_TestAllocator& testAllocator)
: d_initialInUse(testAllocator.numBlocksInUse())
, d_initialMax(testAllocator.numBlocksMax())
, d_initialTotal(testAllocator.numBlocksTotal())
, d_testAllocator_p(&testAllocator)
{
}

// ACCESSORS
inline
bool bslma_TestAllocatorMonitor::isInUseDown() const
{
    return d_initialInUse > d_testAllocator_p->numBlocksInUse();
}

inline
bool bslma_TestAllocatorMonitor::isInUseSame() const
{
    return d_initialInUse == d_testAllocator_p->numBlocksInUse();
}

inline
bool bslma_TestAllocatorMonitor::isInUseUp() const
{
    return d_initialInUse < d_testAllocator_p->numBlocksInUse();
}

inline
bool bslma_TestAllocatorMonitor::isMaxSame() const
{
    return d_initialMax == d_testAllocator_p->numBlocksMax();
}

inline
bool bslma_TestAllocatorMonitor::isMaxUp() const
{
    return d_initialMax != d_testAllocator_p->numBlocksMax();
}

inline
bool bslma_TestAllocatorMonitor::isTotalSame() const
{
    return d_initialTotal == d_testAllocator_p->numBlocksTotal();
}

inline
bool bslma_TestAllocatorMonitor::isTotalUp() const
{
    return d_initialTotal != d_testAllocator_p->numBlocksTotal();
}

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
