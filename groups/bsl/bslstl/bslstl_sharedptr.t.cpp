// bslstl_sharedptr.t.cpp                                             -*-C++-*-
#include <bslstl_sharedptr.h>

#include <bslstl_badweakptr.h>

#include <bslalg_hastrait.h>
#include <bslalg_typetraitusesbslmaallocator.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_managedptr.h>
#include <bslma_newdeleteallocator.h>
#include <bslma_testallocator.h>
#include <bslma_usesbslmaallocator.h>
#include <bslmf_assert.h>
#include <bslmf_isconvertible.h>
#include <bsls_alignmenttotype.h>
#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_exceptionutil.h>
#include <bsls_platform.h>
#include <bsls_stopwatch.h>
#include <bsls_types.h>
#include <bsltf_stdstatefulallocator.h>

// Look what the usage examples drag in...
#include <bslstl_deque.h>
#include <bslstl_list.h>
#include <bslstl_map.h>
#include <bslstl_string.h>
#include <bslstl_vector.h>

#include <stdio.h>
#include <stdlib.h>      // 'atoi'
#include <string.h>      // 'strcmp', 'strcpy'

#ifdef BSLS_PLATFORM_CMP_MSVC   // Microsoft Compiler
#ifdef _MSC_EXTENSIONS          // Microsoft Extensions Enabled
#include <new>                  // if so, need to include <new> as well
#endif
#endif

// BDE_VERIFY pragma: -FE01  // Testing throws exceptions not derived from std

// Warnings that we hope to stop silencing one day:
// BDE_VERIFY pragma:-IND01 // Test blocks not indenting after if (printf(""))
// BDE_VERIFY pragma:-IND04 // Many violations of aligning variable declaration
// BDE_VERIFY pragma:-TP03  // Many tests without assigned test case
// BDE_VERIFY pragma:-UC01  // Warning on all-const identifiers is too strict

#undef ESP  //  From somewhere in SunOS2.10/x86 system headers

using namespace BloombergLP;

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// This test driver tests the functionality of a complex facility consisting of
// two in-core value-semantic types (one of which has pointer semantics), a
// utility class, and a simple functor (which does nothing!).  Testing is
// complicated that the two value-semantic types have a cycle in their public
// interface, so some parts of their testing must be deferred until after the
// majority of their respective functionality has been verified.  Testing is
// further complicated due to the wide variety of behaviors (states) that can
// be supported by the basic 'shared_ptr' type due to aliasing behavior, type-
// erasure of both deleter and allocator, support for incomplete types and
// owning objects other than the template parameter type.  Rather than test all
// of these possible states in the bootstrap basic manipulators test, we will
// incrementally add support for increasingly complex state behavior, which may
// involve additional tests of already validated functions, to cover the newly
// supported states.  Finally, this is all complicated by the routine concerns
// of testing a parameterized template facility over a range of awkward types.
//
// This current test driver is somewhat thorough but still incomplete: among
// the untested concerns:
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
//
// bsl::enable_shared_from_this
//-----------------------------
// CREATORS
// [35] enable_shared_from_this()
// [35] enable_shared_from_this(const enable_shared_from_this&)
// MANIPULATORS
// [35] ~enable_shared_from_this()
// [35] enable_shared_from_this& operator=(const enable_shared_from_this&)
//
// ACCESSORS
// [35] shared_ptr<T> shared_from_this()
// [35] shared_ptr<const T> shared_from_this() const
//
// bsl::shared_ptr
//----------------
// CREATORS
// [ 2] shared_ptr()
// [ 3] shared_ptr(nullptr_t)
// [ 3] shared_ptr(OTHER *ptr)
// [ 3] shared_ptr(OTHER *ptr, bslma::Allocator *basicAllocator)
// [ 3] shared_ptr(ELEM_TYPE *ptr, bslma::SharedPtrRep *rep)
// [ 3] shared_ptr(OTHER *ptr, DELETER *deleter)
// [ 3] shared_ptr(OTHER *ptr, DELETER, bslma::Allocator* = 0)
// [ 3] shared_ptr(OTHER *ptr, DELETER, ALLOCATOR, SFINAE)
// [ 3] shared_ptr(nullptr_t, bslma::Allocator *)
// [ 3] shared_ptr(nullptr_t, DELETER, bslma::Allocator * = 0)
// [ 3] shared_ptr(nullptr_t, DELETER, ALLOCATOR, SFINAE)
// [20] shared_ptr(bslma::ManagedPtr<OTHER>&, bslma::Allocator * = 0)
// [ 3] shared_ptr(std::auto_ptr<OTHER> autoPtr, bslma::Allocator*=0)
// [  ] shared_ptr(std::auto_ptr_ref<TYPE>, bslma::Allocator * = 0)
// [12] shared_ptr(const shared_ptr<ANY_TYPE>&, ELEMENT_TYPE *)
// [  ] shared_ptr(const shared_ptr<OTHER>& other)
// [ 7] shared_ptr(const shared_ptr& original)
// [  ] shared_ptr(const weak_ptr<OTHER>& alias)
// [ 2] ~shared_ptr()
//
// MANIPULATORS
// [ 9] bsl::shared_ptr& operator=(const bsl::shared_ptr& rhs)
// [ 9] bsl::shared_ptr& operator=(const bsl::shared_ptr<OTHER>& rhs)
// [ 9] bsl::shared_ptr& operator=(std::auto_ptr<OTHER> rhs)
// [ 2] void reset()
// [ 2] void reset(OTHER *ptr)
// [ 2] void reset(OTHER *ptr, DELETER deleter)
// [ 2] void reset(OTHER *ptr, DELETER deleter, ALLOCATOR basicAllocator)
// [15] void reset(const shared_ptr<OTHER>& source, TYPE *ptr)
// [ 8] void swap(shared_ptr& src)
// [23] void createInplace(bslma::Allocator *allocator=0)
// [23] void createInplace(bslma::Allocator *, const A1& a1)
// [23] void createInplace(bslma::Allocator *, const A1& a1, ...& a2)
// [23] void createInplace(bslma::Allocator *, const A1& a1, ...& a3)
// [23] void createInplace(bslma::Allocator *, const A1& a1, ...& a4)
// [23] void createInplace(bslma::Allocator *, const A1& a1, ...& a5)
// [23] void createInplace(bslma::Allocator *, const A1& a1, ...& a6)
// [23] void createInplace(bslma::Allocator *, const A1& a1, ...& a7)
// [23] void createInplace(bslma::Allocator *, const A1& a1, ...& a8)
// [23] void createInplace(bslma::Allocator *, const A1& a1, ...& a9)
// [23] void createInplace(bslma::Allocator *, const A1& a1, ...& a10)
// [23] void createInplace(bslma::Allocator *, const A1& a1, ...& a11)
// [23] void createInplace(bslma::Allocator *, const A1& a1, ...& a12)
// [23] void createInplace(bslma::Allocator *, const A1& a1, ...& a13)
// [23] void createInplace(bslma::Allocator *, const A1& a1, ...& a14)
// [14] void loadAlias(const bsl::shared_ptr<ANY_TYPE>&, ELEMENT_TYPE *)
// [17] pair<TYPE *, bslma::SharedPtrRep *> release()
//
// ACCESSORS
// [16] operator BoolType() const
// [ 4] add_lvalue_reference<ELEM_TYPE>::type operator[](ptrdiff_t) const
// [ 4] add_lvalue_reference<ELEM_TYPE>::type operator*() const
// [ 4] TYPE *operator->() const
// [ 4] TYPE *get() const
// [29] bool owner_before(const shared_ptr<OTHER_TYPE>& rhs)
// [29] bool owner_before(const weak_ptr<OTHER_TYPE>& rhs)
// [ 4] bool unique() const
// [ 4] long use_count() const
// [ 4] bslma::SharedPtrRep *rep() const
// [13] bslma::ManagedPtr<TYPE> managedPtr() const
//
// Free functions
//---------------
// [ 6] bool operator==(const shared_ptr<LHS>&, const shared_ptr<RHS>&)
// [ 6] bool operator==(const shared_ptr<LHS>&, bsl::nullptr_t)
// [ 6] bool operator==(bsl::nullptr_t,         const shared_ptr<RHS>&)
// [ 6] bool operator!=(const shared_ptr<LHS>&, const shared_ptr<RHS>&)
// [ 6] bool operator!=(const shared_ptr<LHS>&, bsl::nullptr_t)
// [ 6] bool operator!=(bsl::nullptr_t,         const shared_ptr<RHS>&)
// [ 6] bool operator< (const shared_ptr<LHS>&, const shared_ptr<RHS>&)
// [ 6] bool operator< (const shared_ptr<LHS>&, bsl::nullptr_t)
// [ 6] bool operator< (bsl::nullptr_t,         const shared_ptr<RHS>&)
// [ 6] bool operator<=(const shared_ptr<LHS>&, const shared_ptr<RHS>&)
// [ 6] bool operator<=(const shared_ptr<LHS>&, bsl::nullptr_t)
// [ 6] bool operator<=(bsl::nullptr_t,         const shared_ptr<RHS>&)
// [ 6] bool operator>=(const shared_ptr<LHS>&, const shared_ptr<RHS>&)
// [ 6] bool operator>=(const shared_ptr<LHS>&, bsl::nullptr_t)
// [ 6] bool operator>=(bsl::nullptr_t,         const shared_ptr<RHS>&)
// [ 6] bool operator> (const shared_ptr<LHS>&, const shared_ptr<RHS>&)
// [ 6] bool operator> (const shared_ptr<LHS>&, bsl::nullptr_t)
// [ 6] bool operator> (bsl::nullptr_t,         const shared_ptr<RHS>&)
// [ 5] ostream& operator<<(ostream&, const shared_ptr<TYPE>&)
// [ 8] void swap(shared_ptr<ELEM_TYPE>& a, shared_ptr<ELEM_TYPE>& b)
// [15] DELETER *get_deleter(const shared_ptr<ELEMENT_TYPE>&)
// [19] shared_ptr<TARGET> const_pointer_cast(const shared_ptr<SRC>& ptr)
// [19] shared_ptr<TARGET> dynamic_pointer_cast(const shared_ptr<SRC>& ptr)
// [19] shared_ptr<TARGET> static_pointer_cast(const shared_ptr<SRC>& ptr)
// [33] shared_ptr<T> allocate_shared<T, ALLOC>(ALLOC, ARGS&&...)
// [33] shared_ptr<T> allocate_shared<T, ALLOC>(ALLOC, const ARGS&...)
// [33] shared_ptr<T> allocate_shared<T, A>(A)
// [33] shared_ptr<T> allocate_shared<T, A>(A, const A1& a1)
// [33] shared_ptr<T> allocate_shared<T, A>(A, const A1& a1, ...& a2)
// [33] shared_ptr<T> allocate_shared<T, A>(A, const A1& a1, ...& a3)
// [33] shared_ptr<T> allocate_shared<T, A>(A, const A1& a1, ...& a4)
// [33] shared_ptr<T> allocate_shared<T, A>(A, const A1& a1, ...& a5)
// [33] shared_ptr<T> allocate_shared<T, A>(A, const A1& a1, ...& a6)
// [33] shared_ptr<T> allocate_shared<T, A>(A, const A1& a1, ...& a7)
// [33] shared_ptr<T> allocate_shared<T, A>(A, const A1& a1, ...& a8)
// [33] shared_ptr<T> allocate_shared<T, A>(A, const A1& a1, ...& a9)
// [33] shared_ptr<T> allocate_shared<T, A>(A, const A1& a1, ...& a10)
// [33] shared_ptr<T> allocate_shared<T, A>(A, const A1& a1, ...& a11)
// [33] shared_ptr<T> allocate_shared<T, A>(A, const A1& a1, ...& a12)
// [33] shared_ptr<T> allocate_shared<T, A>(A, const A1& a1, ...& a13)
// [33] shared_ptr<T> allocate_shared<T, A>(A, const A1& a1, ...& a14)
// [34] shared_ptr<T> allocate_shared<T, ALLOC>(ALLOC *, ARGS&&...)
// [34] shared_ptr<T> allocate_shared<T, ALLOC>(ALLOC *,const ARGS&...)
// [34] shared_ptr<T> allocate_shared<T, A>(A *)
// [34] shared_ptr<T> allocate_shared<T, A>(A *, const A1& a1)
// [34] shared_ptr<T> allocate_shared<T, A>(A *, const A1& a1, ...& a2)
// [34] shared_ptr<T> allocate_shared<T, A>(A *, const A1& a1, ...& a3)
// [34] shared_ptr<T> allocate_shared<T, A>(A *, const A1& a1, ...& a4)
// [34] shared_ptr<T> allocate_shared<T, A>(A *, const A1& a1, ...& a5)
// [34] shared_ptr<T> allocate_shared<T, A>(A *, const A1& a1, ...& a6)
// [34] shared_ptr<T> allocate_shared<T, A>(A *, const A1& a1, ...& a7)
// [34] shared_ptr<T> allocate_shared<T, A>(A *, const A1& a1, ...& a8)
// [34] shared_ptr<T> allocate_shared<T, A>(A *, const A1& a1, ...& a9)
// [34] shared_ptr<T> allocate_shared<T, A>(A *, const A1& a1, ...& a10)
// [34] shared_ptr<T> allocate_shared<T, A>(A *, const A1& a1, ...& a11)
// [34] shared_ptr<T> allocate_shared<T, A>(A *, const A1& a1, ...& a12)
// [34] shared_ptr<T> allocate_shared<T, A>(A *, const A1& a1, ...& a13)
// [34] shared_ptr<T> allocate_shared<T, A>(A *, const A1& a1, ...& a14)
// [32] shared_ptr<T> make_shared<T>(ARGS&&...args)
// [32] shared_ptr<T> make_shared<T>(const ARGS&...args)
// [32] shared_ptr<T> make_shared<T>()
// [32] shared_ptr<T> make_shared<T>(const A1& a1)
// [32] shared_ptr<T> make_shared<T>(const A1& a1, ...& a2)
// [32] shared_ptr<T> make_shared<T>(const A1& a1, ...& a3)
// [32] shared_ptr<T> make_shared<T>(const A1& a1, ...& a4)
// [32] shared_ptr<T> make_shared<T>(const A1& a1, ...& a5)
// [32] shared_ptr<T> make_shared<T>(const A1& a1, ...& a6)
// [32] shared_ptr<T> make_shared<T>(const A1& a1, ...& a7)
// [32] shared_ptr<T> make_shared<T>(const A1& a1, ...& a8)
// [32] shared_ptr<T> make_shared<T>(const A1& a1, ...& a9)
// [32] shared_ptr<T> make_shared<T>(const A1& a1, ...& a10)
// [32] shared_ptr<T> make_shared<T>(const A1& a1, ...& a11)
// [32] shared_ptr<T> make_shared<T>(const A1& a1, ...& a12)
// [32] shared_ptr<T> make_shared<T>(const A1& a1, ...& a13)
// [32] shared_ptr<T> make_shared<T>(const A1& a1, ...& a14)
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST (shared_ptr)
// [ 3] shared_ptr(TYPE *ptr) // synthesized
// [ 3] shared_ptr(TYPE *ptr, bslma::Allocator *allocator)  // synthesized
// [15] CONCERN: C++ 'bsl::shared_ptr' ISO CONFORMANCE
// [21] DRQS 26465543 [void reset()]
// [22] shared_ptr<cv-void>
// [  ] USAGE EXAMPLE (shared_ptr) // TBD
// [-1] PERFORMANCE
//-----------------------------------------------------------------------------
//
// ============================================================================
//                              TEST PLAN (weak_ptr)
//
// This component provides a mechanism to create weak references to
// reference-counted shared objects (managed by 'bsl::shared_ptr'.  The
// functions supported by 'bsl::weak_ptr' include creating weak references (via
// multiple constructors), changing the weak pointer object being referenced
// (via the assignment operators), getting a shared pointer (via the 'lock'
// function), resetting the weak pointer (via 'reset'), and destroying the weak
// pointer.
//
// All the functions in this component are reasonably straight-forward and
// typically increment or decrement the number of strong or weak references as
// a side effect.  In addition the destructor and the reset functions may
// destroy the representation.  To test these functions we create a simple test
// representation that allows us to check the current strong and weak count and
// additionally stores the number of times the data value and the
// representation were attempted to be destroyed.
// ----------------------------------------------------------------------------
// CREATORS
// [24] weak_ptr()
// [24] weak_ptr(const weak_ptr& original)
// [24] weak_ptr(const shared_ptr<COMPATIBLE_TYPE>& other)
// [24] weak_ptr(const weak_ptr<COMPATIBLE_TYPE>& other)
// [24] ~weak_ptr()
//
// MANIPULATORS
// [25] weak_ptr& operator=(const weak_ptr& rhs)
// [25] weak_ptr& operator=(const shared_ptr<COMPATIBLE_TYPE>& rhs)
// [25] weak_ptr& operator=(const weak_ptr<COMPATIBLE_TYPE>& rhs)
// [26] void reset()
// [28] void swap(weak_ptr& src)
//
// ACCESSORS
// [24] bslma::SharedPtrRep *rep() const
// [24] bool expired() const
// [24] long use_count() const
// [27] shared_ptr<TYPE> lock() const
// [29] bool owner_before(const shared_ptr<OTHER_TYPE>& rhs)
// [29] bool owner_before(const weak_ptr<OTHER_TYPE>& rhs)
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST (weak_ptr)
// [35] USAGE EXAMPLE 1: weak_ptr
// [36] USAGE EXAMPLE 2: weak_ptr
// [37] USAGE EXAMPLE 3: weak_ptr
//-----------------------------------------------------------------------------
//
// ============================================================================
//                   TEST PLAN (Utility struct SharedPtrUtil)
//
// ----------------------------------------------------------------------------
// bslstl::SharedPtrUtil
//----------------------
// [11] bsl::shared_ptr<char> createInplaceUninitializedBuffer(...)
// [  ] void throwBadWeakPtr()
// [10] shared_ptr<TARGET> constCast(const shared_ptr<SOURCE>& source)
// [10] shared_ptr<TARGET> dynamicCast(const shared_ptr<SOURCE>& source)
// [10] shared_ptr<TARGET> staticCast(const shared_ptr<SOURCE>& source)
// [10] void constCast(shared_ptr<TARGET> *, const shared_ptr<SOURCE>&)
// [10] void dynamicCast(shared_ptr<TARGET> *, const shared_ptr<SOURCE>&)
// [10] void staticCast(shared_ptr<TARGET> *, const shared_ptr<SOURCE>&)
//
// ============================================================================
//                       TEST PLAN (Additional functors)
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
// bsl::hash< shared_ptr<T> >
//---------------------------
// [31] size_t operator()(const shared_ptr<TYPE>& ptr) const
//
// bslstl::SharedPtrNilDeleter
//----------------------------
// [  ] void operator()(const void *) const
//
// bslstl::SharedPtr_DefaultDeleter<ANY_TYPE>
//-------------------------------------------
// [  ] void operator()(ANY_TYPE *ptr) const
//
// ============================================================================
//                         TEST PLAN (Test machinery)
// This test driver produced quite a bit of test machinery, including a couple
// of class hierarchies where the (virtual) base/derived relationships between
// types is important for the validity of tests.  Additionally, there are some
// factories to produce well-specified rvalues, a variety of deleters, and a
// custom implementation of a shared pointer representation to validate user
// extensibility of the protocol works as intended with the prime consumer of
// the protocol.
// ----------------------------------------------------------------------------
//
// class MyTestObject
//-------------------
// [  ] volatile bsls::Types::Int64 *deleteCounter() const
// [  ] volatile bsls::Types::Int64 *copyCounter() const
//
// class MyTestBaseObject
//-----------------------
//
// class MyTestDerivedObject
//--------------------------
// [  ] volatile bsls::Types::Int64 *deleteCounter() const
// [  ] volatile bsls::Types::Int64 *copyCounter() const
//
// class MyPDTestObject
//---------------------
// (only a private destructor)
//
// class MyTestObjectFactory
//--------------------------
// [  ] MyTestObjectFactory()
// [  ] MyTestObjectFactory(bslma::Allocator *basicAllocator)
// [  ] void deleteObject(MyTestObject *obj) const
//
// class MyTestDeleter
//--------------------
// [  ] MyTestDeleter(bslma::Allocator *basicAlloc = 0, int *callCounter = 0)
// [  ] MyTestDeleter(const MyTestDeleter& original)
// [  ] void operator() (OBJECT_TYPE *ptr) const
// [  ] bool operator==(const MyTestDeleter& rhs) const
//
// class MyAllocTestDeleter
//-------------------------
// [  ] MyAllocTestDeleter(bslma::Allocator *, bslma::Allocator * = 0)
// [  ] MyAllocTestDeleter(const MyAllocTestDeleter&, bslma::Allocator * = 0)
// [  ] ~MyAllocTestDeleter()
// [  ] MyAllocTestDeleter& operator=(const MyAllocTestDeleter& rhs)
// [  ] void operator()(OBJECT_TYPE *ptr) const
//
// class MyTestArg<N>
//-------------------
// [  ] MyTestArg(int value = -1)
// [  ] operator int&()
// [  ] operator int() const
//
// class MyInplaceTestObject
//--------------------------
//
// class TestSharedPtrRep
//-----------------------
// [  ] TestSharedPtrRep(TYPE *dataPtr_p, bslma::Allocator *basicAllocator)
// [  ] ~TestSharedPtrRep()
// [  ] void disposeRep()
// [  ] void disposeObject()
// [  ] void *originalPtr() const
// [  ] TYPE *ptr() const
// [  ] int disposeRepCount() const
// [  ] int disposeObjectCount() const
// [  ] void *getDeleter(const std::type_info& type)
//
// class ManagedPtrTestDeleter<TYPE>
//----------------------------------
// [  ] ManagedPtrTestDeleter()
// [  ] void deleteObject(TYPE* obj)
// [  ] TYPE* providedObj()
// [  ] void reset()
//
// class SelfReference
//--------------------
// [  ] void setData(const bsl::shared_ptr<SelfReference>& value)
// [  ] void release()
//
// Free functions to support testing
// ---------------------------------
// [  ] void myTestDeleterFunction(MyTestObject *)
// [  ] bsl::shared_ptr<int> NAMESPACE_TEST_CASE_16::ptrNilFun()
// [  ] bsl::shared_ptr<int> NAMESPACE_TEST_CASE_16::ptr1Fun()
// [  ] std::auto_ptr<MyTestObject> makeAuto()
// [  ] std::auto_ptr<MyTestObject> makeAuto(bsls::Types::Int64 *counter)
// [  ] void TestDriver::doNotDelete(TYPE *)
//
// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

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

// Disable specific bde_verify warnings where practice of usage example may
// differ.

// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -FABC01 // Functions ordered for expository purpose

// BDE_VERIFY pragma: -FD01  // Function needs contract, we probably should fix
// BDE_VERIFY pragma: -FD02  // Banners diagnose badly unless we fix for FD01
// BDE_VERIFY pragma: -FD03  // no contract, so no ticks

// ============================================================================
//                              USAGE EXAMPLES
// ----------------------------------------------------------------------------

namespace NAMESPACE_USAGE_EXAMPLE_1 {

// The following examples demonstrate various features and uses of shared
// pointers.
//
///Example 1 - Basic Usage
///- - - - - - - - - - - -
// The following example demonstrates the creation of a shared pointer.  First,
// we declare the type of object that we wish to manage:
//..
    class MyUser {
        // DATA
        bsl::string d_name;
        int         d_id;

      public:
        // CREATORS
        explicit MyUser(bslma::Allocator *alloc = 0)
        : d_name(alloc)
        , d_id(0)
        {
        }
        MyUser(const bsl::string& name, int id, bslma::Allocator *alloc = 0)
        : d_name(name, alloc)
        , d_id(id)
        {
        }
        MyUser(const MyUser& original, bslma::Allocator *alloc = 0) // IMPLICIT
        : d_name(original.d_name, alloc)
        , d_id(original.d_id)
        {
        }

        // MANIPULATORS
        void setId(int id) { d_id = id; }
        void setName(const bsl::string& name) { d_name = name; }

        // ACCESSORS
        int id() const { return d_id; }
        const bsl::string& name() const { return d_name; }
    };
//..
// The 'createUser' utility function (below) creates a 'MyUser' object using
// the provided allocator and returns a shared pointer to the newly-created
// object.  Note that the shared pointer's internal representation will also be
// allocated using the same allocator.  Also note that if 'allocator' is 0, the
// currently-installed default allocator is used.
//..
    bsl::shared_ptr<MyUser> createUser(bsl::string       name,
                                       int               id,
                                       bslma::Allocator *allocator = 0)
    {
        allocator = bslma::Default::allocator(allocator);
        MyUser *user = new (*allocator) MyUser(name, id, allocator);
        return bsl::shared_ptr<MyUser>(user, allocator);
    }
//..
// Since the 'createUser' function both allocates the object and creates the
// shared pointer, it can benefit from the in-place facilities to avoid an
// extra allocation.  Again, note that the representation will also be
// allocated using the same allocator (see the section "Correct Usage of the
// Allocator Model" above).  Also note that if 'allocator' is 0, the
// currently-installed default allocator is used.
//..
    bsl::shared_ptr<MyUser> createUser2(bsl::string       name,
                                        int               id,
                                        bslma::Allocator *allocator = 0)
    {
        bsl::shared_ptr<MyUser> user;
        user.createInplace(allocator, name, id, allocator);
        return user;
    }
//..
// Note that the shared pointer allocates both the reference count and the
// 'MyUser' object in a single region of memory (which is the memory that will
// eventually be deallocated), but refers to the 'MyUser' object only.
}  // close namespace NAMESPACE_USAGE_EXAMPLE_1

namespace NAMESPACE_USAGE_EXAMPLE_2 {
using     NAMESPACE_USAGE_EXAMPLE_1::MyUser;
//
///Using Custom Deleters
///- - - - - - - - - - -
// The following examples demonstrate the use of custom deleters with shared
// pointers.
//
///Example 2 - Nil deleters
///  -  -  -  -  -  -  -  -
// There are cases when an interface calls for an object to be passed as a
// shared pointer, but the object being passed is not owned by the caller
// (e.g., a pointer to a static variable).  In these cases, it is possible to
// create a shared pointer specifying 'bslstl::SharedPtrNilDeleter' as the
// deleter.  The deleter function provided by 'bslstl::SharedPtrNilDeleter' is
// a no-op and does not delete the object.  The following example demonstrates
// the use of 'bsl::shared_ptr' using a 'bslstl::SharedPtrNilDeleter'.  The
// code uses the 'MyUser' class defined in Example 1.  In this example, an
// asynchronous transaction manager is implemented.  Transactions are enqueued
// into the transaction manager to be processed at some later time.  The user
// associated with the transaction is passed as a shared pointer.  Transactions
// can originate from the "system" or from "users".
//
// We first declare the transaction manager and transaction info classes:
//..
    class MyTransactionInfo {
        // Transaction Info...
    };

    class MyTransactionManager {

        // PRIVATE MANIPULATORS
        int enqueueTransaction(bsl::shared_ptr<MyUser>  user,
                               const MyTransactionInfo& transaction);
      public:
        // CLASS METHODS
        static MyUser *systemUser(bslma::Allocator *basicAllocator = 0);

        // MANIPULATORS
        int enqueueSystemTransaction(const MyTransactionInfo& transaction);

        int enqueueUserTransaction(const MyTransactionInfo& transaction,
                                   bsl::shared_ptr<MyUser>  user);

    };
//..
// The 'systemUser' class method returns the same 'MyUser' object and should
// not be destroyed by its users:
//..
    MyUser *MyTransactionManager::systemUser(
                                        bslma::Allocator* /* basicAllocator */)
    {
        static MyUser *systemUserSingleton;
        if (!systemUserSingleton) {
            // instantiate singleton in a thread-safe manner passing
            // 'basicAllocator'

            // . . .
        }
        return systemUserSingleton;
    }
//..
// For enqueuing user transactions, simply proxy the information to
// 'enqueueTransaction'.
//..
    inline
    int MyTransactionManager::enqueueUserTransaction(
                                          const MyTransactionInfo& transaction,
                                          bsl::shared_ptr<MyUser>  user)
    {
        return enqueueTransaction(user, transaction);
    }
//..
// For system transactions, we must use the 'MyUser' objected returned from the
// 'systemUser' 'static' method.  Since we do not own the returned object, we
// cannot directly construct a 'bsl::shared_ptr' object for it: doing so would
// result in the singleton being destroyed when the last reference to the
// shared pointer is released.  To solve this problem, we construct a
// 'bsl::shared_ptr' object for the system user using a nil deleter.  When the
// last reference to the shared pointer is released, although the deleter will
// be invoked to destroy the object, it will do nothing.
//..
    int MyTransactionManager::enqueueSystemTransaction(
                                          const MyTransactionInfo& transaction)
    {
        bsl::shared_ptr<MyUser> user(systemUser(),
                                     bslstl::SharedPtrNilDeleter(),
                                     0);
        return enqueueTransaction(user, transaction);
    }
//..
}  // close namespace NAMESPACE_USAGE_EXAMPLE_2


// Function definitions elided from usage examples, but requiring definitions
// for the test driver to link.

namespace NAMESPACE_USAGE_EXAMPLE_2 {
int MyTransactionManager::enqueueTransaction(bsl::shared_ptr<MyUser>,
                                             const MyTransactionInfo&)
{
    // Dummy function provided so that usage example can link.

    return 0;
}

}  // close namespace NAMESPACE_USAGE_EXAMPLE_2

//=============================================================================
//                                USAGE EXAMPLE (weak_ptr)
//-----------------------------------------------------------------------------

// Example 2 - Breaking cyclical dependencies
//- - - - - - - - - - - - - - - - - - - - - -
// Weak pointers are frequently used to break cyclical dependencies between
// objects that store references to each other via a shared pointer.  Consider
// for example a simplified news alert system that sends news alerts to users
// based on keywords that they register for.  The user information is stored in
// the 'User' class and the details of the news alert are stored in the 'Alert'
// class.  The class definitions for 'User' and 'Alert' are provided below
// (with any code not relevant to this example elided):
//..
    class Alert;

    class User {
        // This class stores the user information required for listening to
        // alerts.

        bsl::vector<bsl::shared_ptr<Alert> > d_alerts;  // alerts user is
                                                        // registered for

        // ...

      public:
        // MANIPULATORS
        void addAlert(const bsl::shared_ptr<Alert>& alertPtr)
        {
            // Add the specified 'alertPtr' to the list of alerts being
            // monitored by this user.

            d_alerts.push_back(alertPtr);
        }


        // ...
    };
//..
// Now we define an alert class, 'Alert':
//..
    class Alert {
        // This class stores the alert information required for sending alerts.

        bsl::vector<bsl::shared_ptr<User> > d_users;  // users registered for
                                                      // this alert

      public:
        // MANIPULATORS
        void addUser(const bsl::shared_ptr<User>& userPtr)
        {
            // Add the specified 'userPtr' to the list of users monitoring this
            // alert.

            d_users.push_back(userPtr);
        }

        // ...
    };

//..
// Even though we have released 'alertPtr' and 'userPtr' there still exists a
// cyclic reference between the two objects, so none of the objects are
// destroyed.
//
// We can break this cyclical dependency we define a modified alert class
// 'ModifiedAlert' that stores a weak pointer to a 'ModifiedUser' object.
// Below is the definition for the 'ModifiedUser' class which is identical to
// the 'User' class, the only difference being that it stores shared pointer to
// 'ModifiedAlert's instead of 'Alert's:
//..
    class ModifiedAlert;

    class ModifiedUser {
        // This class stores the user information required for listening to
        // alerts.

        bsl::vector<bsl::shared_ptr<ModifiedAlert> > d_alerts;// alerts user is
                                                              // registered for

        // ...

      public:
        // MANIPULATORS
        void addAlert(const bsl::shared_ptr<ModifiedAlert>& alertPtr)
        {
            // Add the specified 'alertPtr' to the list of alerts being
            // monitored by this user.

            d_alerts.push_back(alertPtr);
        }


        // ...
    };
//..
// Now we define the 'ModifiedAlert' class:
//..
    class ModifiedAlert {
        // This class stores the alert information required for sending alerts.

//..
// Note that the user is stored by a weak pointer instead of by a shared
// pointer:
//..
        bsl::vector<bsl::weak_ptr<ModifiedUser> > d_users;  // users registered
                                                            // for this alert

      public:
        // MANIPULATORS
        void addUser(const bsl::weak_ptr<ModifiedUser>& userPtr)
        {
            // Add the specified 'userPtr' to the list of users monitoring this
            // alert.

            d_users.push_back(userPtr);
        }

        // ...
    };
//..

// Usage example 3 - Caching example
// - - - - - - - - - - - - - - - - -
// Suppose we want to implement a peer to peer file sharing system that allows
// users to search for files that match specific keywords.  A simplistic
// version of such a system with code not relevant to the usage example elided
// would have the following parts:
//
// a) A peer manager class that maintains a list of all connected peers and
// updates the list based on incoming peer requests and disconnecting peers.
// The following would be a simple interface for the Peer and PeerManager
// classes:
//..
    class Peer {
        // This class stores all the relevant information for a peer.

        // ...
    };

    class PeerManager {
        // This class acts as a manager of peers and adds and removes peers
        // based on peer requests and disconnections.

        // DATA
//..
// The peer objects are stored by shared pointer to allow peers to be passed to
// search results and still allow their asynchronous destruction when peers
// disconnect.
//..
        bsl::map<int, bsl::shared_ptr<Peer> > d_peers;

        // ...
    };
//..
// b) A peer cache class that stores a subset of the peers that are used for
// sending search requests.  The cache may select peers based on their
// connection bandwidth, relevancy of previous search results, etc.  For
// brevity the population and flushing of this cache is not shown:
//..
    class PeerCache {
        // This class caches a subset of all peers that match certain criteria
        // including connection bandwidth, relevancy of previous search
        // results, etc.

//..
// Note that the cached peers are stored as a weak pointer so as not to
// interfere with the cleanup of Peer objects by the PeerManager if a Peer goes
// down.
//..
        // DATA
        bsl::list<bsl::weak_ptr<Peer> > d_cachedPeers;

      public:
        // TYPES
        typedef bsl::list<bsl::weak_ptr<Peer> >::const_iterator PeerConstIter;

        // ...

        // ACCESSORS
        PeerConstIter begin() const { return d_cachedPeers.begin(); }
        PeerConstIter end() const   { return d_cachedPeers.end(); }
    };
//..
// c) A search result class that stores a search result and encapsulates a peer
// with the file name stored by the peer that best matches the specified
// keywords:
//..
    class SearchResult {
        // This class provides a search result and encapsulates a particular
        // peer and filename combination that matches a specified set of
        // keywords.

//..
// The peer is stored as a weak pointer because when the user decides to select
// a particular file to download from this peer, the peer might have
// disconnected.
//..
        // DATA
        bsl::weak_ptr<Peer> d_peer;
        bsl::string         d_filename;

      public:
        // CREATORS
        SearchResult(const bsl::weak_ptr<Peer>& peer,
                     const bsl::string&         filename)
        : d_peer(peer)
        , d_filename(filename)
        {
        }

        // ...

        // ACCESSORS
        const bsl::string& filename() const { return d_filename; }
        const bsl::weak_ptr<Peer>& peer() const { return d_peer; }
    };
//..
// d) A search function that takes a list of keywords and returns available
// results by searching the cached peers:
//..
    ;

    void search(bsl::vector<SearchResult>       */* results */,
                const PeerCache&                 peerCache,
                const bsl::vector<bsl::string>&  /* keywords */)
    {
        for (PeerCache::PeerConstIter iter = peerCache.begin();
             iter != peerCache.end();
             ++iter) {
//..
// First we check if the peer is still connected by acquiring a shared pointer
// to the peer.  If the acquire operation succeeds then we can send the peer a
// request to send back the file best matching the specified keywords:
//..
            bsl::shared_ptr<Peer> peerSharedPtr = iter->lock();
            if (peerSharedPtr) {

                // Search the peer for file best matching the specified
                // keywords and if a file is found add the returned
                // SearchResult object to result.

                // ...
            }
        }
    }
//..
// e) A download function that downloads a file selected by the user:
//..
    void download(const SearchResult& result)
    {
        bsl::shared_ptr<Peer> peerSharedPtr = result.peer().lock();
        if (peerSharedPtr) {
            // Download the result.filename() file from peer knowing that the
            // peer is still connected.
        }
    }
//..

// BDE_VERIFY pragma: pop

// Define traits outside of the text of the usage example as they distract from
// the core message.

namespace BloombergLP {
namespace bslma {

template<>
struct UsesBslmaAllocator<NAMESPACE_USAGE_EXAMPLE_1::MyUser>
     : bsl::true_type {};

}  // close namespace bslma
}  // close enterprise namespace

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

// 'MyTestObject' CLASS HIERARCHY (defined below)
class MyTestObject;
class MyInplaceTestObject;
class MyTestBaseObject;
class MyTestDerivedObject;

// OTHER TEST OBJECTS (defined below)
class MyPDTestObject;
class MyInplaceTestObject;

// SUPPORT TYPES FOR OTHER TEST OBJECTS (defined below)
template <int N>
class MyTestArg;

// TEST DELETERS SECTION (defined below)
class MyTestObjectFactory;
class MyTestDeleter;
class MyAllocTestDeleter;

// TEST-CASE SUPPORT TYPES
template <class POINTER>
struct PerformanceTester;

// TYPEDEFS
typedef bsl::shared_ptr<MyTestObject> Obj;
typedef bsl::shared_ptr<const MyTestObject> ConstObj;
typedef bsl::shared_ptr<MyInplaceTestObject> TCObj;
typedef MyTestObject TObj;

//=============================================================================
//              GLOBAL HELPER CLASSES AND FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

void myTestDeleterFunction(MyTestObject *);
    // This function can be used as a function-like deleter (by address) for a
    // 'bsl::shared_ptr' representation.

namespace NAMESPACE_TEST_CASE_16 {

int    x = 1;
int    y = 2;
double z = 3.0;

bslma::TestAllocator g_alloc16("test case 16");

const bsl::shared_ptr<int> ptrNil(0, &g_alloc16);
const bsl::shared_ptr<int> ptr1(&x, bslstl::SharedPtrNilDeleter(), &g_alloc16);
const bsl::shared_ptr<int> ptr2(&y, bslstl::SharedPtrNilDeleter(), &g_alloc16);
const bsl::shared_ptr<double>
                           ptr3(&z, bslstl::SharedPtrNilDeleter(), &g_alloc16);

bsl::shared_ptr<int> ptr1Fun()
    // Return a 'bsl::shared_ptr' rvalue that is not empty.
{
    return ptr1;
}

bsl::shared_ptr<int> ptrNilFun()
    // Return an empty 'bsl::shared_ptr' rvalue.
{
    return ptrNil;
}

}  // close namespace NAMESPACE_TEST_CASE_16

                   // *** 'MyTestObject' CLASS HIERARCHY ***

                           // ======================
                           // class MyTestBaseObject
                           // ======================

class MyTestBaseObject {
    // This class provides a test object used to verify that shared pointers
    // can statically and dynamically cast without slicing.

    // DATA
    char d_padding[32];    // Padding bytes that are never initialized

  public:
    // CREATORS
    MyTestBaseObject() {}
        // Create a 'MyTestBaseObject' object.

    MyTestBaseObject(const MyTestBaseObject&) {}
        // Create a 'MyTestBaseObject' object.  Note that this constructor does
        // not copy the unused (and uninitialized) values of the 'd_padding'
        // bytes.

    virtual ~MyTestBaseObject() {}
        // Destroy this object.

    MyTestBaseObject& operator=(const MyTestBaseObject&) { return *this; }
        // Return a reference to 'this' object.  There are no other effects.
};


                             // ==================
                             // class MyTestObject
                             // ==================

class MyTestObject : public MyTestBaseObject {
    // This class provides a test object that keeps track of how many objects
    // have been deleted.  Optionally, also keeps track of how many objects
    // have been copied.

    // DATA
    volatile bsls::Types::Int64 *d_deleteCounter_p;
    volatile bsls::Types::Int64 *d_copyCounter_p;

  public:
    // CREATORS
    MyTestObject(const MyTestObject& original);
        // Create a copy of the specified 'original' object.

    explicit MyTestObject(bsls::Types::Int64 *deleteCounter,
                          bsls::Types::Int64 *copyCounter = 0);

    virtual ~MyTestObject();
        // Destroy this object.

    // ACCESSORS
    volatile bsls::Types::Int64 *copyCounter() const;
        // Return a pointer to the counter (if any) used to track the number of
        // times an object of type 'MyTestObject' has been copied.

    volatile bsls::Types::Int64 *deleteCounter() const;
        // Return a pointer to the counter used to track the number of times an
        // object of type 'MyTestObject' has been copied.

};

                         // =========================
                         // class MyTestDerivedObject
                         // =========================

class MyTestObject2 : public MyTestBaseObject {};
    // This supporting class for 'MyTestDerivedObject' is simply to make sure
    // that test objects with multiple inheritance work fine.

class MyTestDerivedObject : public MyTestObject2, public MyTestObject {
    // This class provides a test derived object, in order to make sure that
    // that test objects with multiple inheritance work fine.

  public:
    // CREATORS
    explicit MyTestDerivedObject(const MyTestObject& original);
        // Create a 'MyTestDerivedObject' using the same counters (if any) as
        // the specified 'original' object.

    explicit MyTestDerivedObject(bsls::Types::Int64 *counter,
                                 bsls::Types::Int64 *copyCounter = 0);
};

                         // *** OTHER TEST OBJECTS ***

                       // ======================
                       // class MostEvilTestType
                       // ======================

class MostEvilTestType {
    // This class provides the most awkward type imaginable that should be
    // supported as an element type for the standard 'shared_ptr' template.

  private:
    int d_data;

  private:
    // NOT IMPLEMENTED
    explicit MostEvilTestType(const MostEvilTestType& original); // = delete;
    void operator=(MostEvilTestType&); // = delete

    void operator&();  // = delete;

    template<class ANY_TYPE>
    void operator,(const ANY_TYPE&); // = delete;

    template<class ANY_TYPE>
    void operator,(ANY_TYPE&); // = delete;

    static void* operator new(std::size_t size); // = delete
    static void* operator new(std::size_t size, void *ptr); // = delete
    static void operator delete(void *ptr); // = delete

  public:
    // CREATORS
    explicit MostEvilTestType(int value) : d_data(value) {}
        // Create a 'MostEvilTestType' object having the specified 'value' as
        // its 'data'.

    // ~MostEvilTestType() = default;
        // Destroy this object.

    // MANIPULATORS
    void setData(int value) { d_data = value; }
        // Set the 'data' attribute of this object to the specified 'value'.

    // ACCESSORS
    int data() const { return d_data; }
        // Return the value of the 'data' attribute of this object.
};

                     // =======================
                     // class ConstructorFailed
                     // =======================

struct ConstructorFailed {};

                     // ==========================
                     // class MyInstrumentedObject
                     // ==========================

class MyInstrumentedObject {
    // This class provides a test object that keeps track of how many objects
    // have been deleted.  Optionally, also keeps track of how many objects
    // have been copied.

    // DATA
    int *d_constructCounter_p;
    int *d_destroyCounter_p;

  private:
    // NOT IMPLEMENTED
    MyInstrumentedObject(const MyInstrumentedObject& original); // = delete
    MyInstrumentedObject& operator=(const MyInstrumentedObject& other);
                                                                    // = delete

  public:
    // CREATORS

    MyInstrumentedObject(int  *constructCounter,
                         int  *destroyCounter,
                         bool  throwAfterInit = false);
        // Create a 'MyInstrumentedObject' using the specified
        // 'constructCounter' to track the number of times a constructor is
        // called, the specified 'destroyCounter' to track the number of times
        // a destructor is called.  If the optionally specified
        // 'throwAfterInit' is 'true', throw a 'ConstructorFailed' exception
        // after initializing the data members (aborting this constructor).

    ~MyInstrumentedObject();
        // Destroy this object.

    // ACCESSORS
    int *constructCounter() const;
        // Return a pointer to the counter used to track the number of times an
        // object of type 'MyInstrumentedObject' has been constructed.

    int *destroyCounter() const;
        // Return a pointer to the counter used to track the number of times an
        // object of type 'MyInstrumentedObject' has been destroyed.

};

                            // ====================
                            // class MyPDTestObject
                            // ====================

class MyPDTestObject {
    // This class defines a private destructor, to prove objects that cannot be
    // destroyed.  This will be used to test the 'bsl::shared_ptr' conversion
    // to managed pointers.

  private:
    ~MyPDTestObject() {}
        // Destroy this object.
};

                         // ====================
                         // class MyTestArg<int>
                         // ====================

template <int N>
class MyTestArg {
    // This class template declares a separate type for each template parameter
    // value 'N', that wraps an integer value and provides implicit conversion
    // to and from 'int'.  Its main purpose is that having separate types for
    // testing enables distinguishing them when calling through a function
    // template interface, thereby avoiding ambiguities or accidental switching
    // of arguments in the implementation of in-place constructors.

    // DATA
    int d_value;

  public:
    // CREATORS
    explicit MyTestArg(int value = -1) : d_value(value) {}
        // Create a test argument object having the optionally specified
        // 'value', and having the value '-1' otherwise.

    // ACCESSORS
    operator int() const { return d_value; }
        // Return the value of this test argument object.
};

typedef MyTestArg< 1> MyTestArg1;
typedef MyTestArg< 2> MyTestArg2;
typedef MyTestArg< 3> MyTestArg3;
typedef MyTestArg< 4> MyTestArg4;
typedef MyTestArg< 5> MyTestArg5;
typedef MyTestArg< 6> MyTestArg6;
typedef MyTestArg< 7> MyTestArg7;
typedef MyTestArg< 8> MyTestArg8;
typedef MyTestArg< 9> MyTestArg9;
typedef MyTestArg<10> MyTestArg10;
typedef MyTestArg<11> MyTestArg11;
typedef MyTestArg<12> MyTestArg12;
typedef MyTestArg<13> MyTestArg13;
typedef MyTestArg<14> MyTestArg14;
    // Define fourteen test argument types 'MyTestArg1..14' to be used with the
    // in-place constructors of 'MyInplaceTestObject'.

                         // =========================
                         // class MyInplaceTestObject
                         // =========================

class MyInplaceTestObject {
    // This class provides a test object used to check that the arguments
    // passed for creating a shared pointer with an in-place representation are
    // of the correct types and values.

    // DATA
    MyTestArg1  d_a1;
    MyTestArg2  d_a2;
    MyTestArg3  d_a3;
    MyTestArg4  d_a4;
    MyTestArg5  d_a5;
    MyTestArg6  d_a6;
    MyTestArg7  d_a7;
    MyTestArg8  d_a8;
    MyTestArg9  d_a9;
    MyTestArg10 d_a10;
    MyTestArg11 d_a11;
    MyTestArg12 d_a12;
    MyTestArg13 d_a13;
    MyTestArg14 d_a14;
    static int  s_numDeletes;

  public:
    // CREATORS
    MyInplaceTestObject();
    explicit MyInplaceTestObject(MyTestArg1 a1);
    MyInplaceTestObject(MyTestArg1  a1,  MyTestArg2  a2);
    MyInplaceTestObject(MyTestArg1  a1,  MyTestArg2  a2,  MyTestArg3  a3);
    MyInplaceTestObject(MyTestArg1  a1,  MyTestArg2  a2,  MyTestArg3  a3,
                        MyTestArg4  a4);
    MyInplaceTestObject(MyTestArg1  a1,  MyTestArg2  a2,  MyTestArg3  a3,
                        MyTestArg4  a4,  MyTestArg5  a5);
    MyInplaceTestObject(MyTestArg1  a1,  MyTestArg2  a2,  MyTestArg3  a3,
                        MyTestArg4  a4,  MyTestArg5  a5,  MyTestArg6  a6);
    MyInplaceTestObject(MyTestArg1  a1,  MyTestArg2  a2,  MyTestArg3  a3,
                        MyTestArg4  a4,  MyTestArg5  a5,  MyTestArg6  a6,
                        MyTestArg7  a7);
    MyInplaceTestObject(MyTestArg1  a1,  MyTestArg2  a2,  MyTestArg3  a3,
                        MyTestArg4  a4,  MyTestArg5  a5,  MyTestArg6  a6,
                        MyTestArg7  a7,  MyTestArg8  a8);
    MyInplaceTestObject(MyTestArg1  a1,  MyTestArg2  a2,  MyTestArg3  a3,
                        MyTestArg4  a4,  MyTestArg5  a5,  MyTestArg6  a6,
                        MyTestArg7  a7,  MyTestArg8  a8,  MyTestArg9  a9);
    MyInplaceTestObject(MyTestArg1  a1,  MyTestArg2  a2,  MyTestArg3  a3,
                        MyTestArg4  a4,  MyTestArg5  a5,  MyTestArg6  a6,
                        MyTestArg7  a7,  MyTestArg8  a8,  MyTestArg9  a9,
                        MyTestArg10 a10);
    MyInplaceTestObject(MyTestArg1  a1,  MyTestArg2  a2,  MyTestArg3  a3,
                        MyTestArg4  a4,  MyTestArg5  a5,  MyTestArg6  a6,
                        MyTestArg7  a7,  MyTestArg8  a8,  MyTestArg9  a9,
                        MyTestArg10 a10, MyTestArg11 a11);
    MyInplaceTestObject(MyTestArg1  a1,  MyTestArg2  a2,  MyTestArg3  a3,
                        MyTestArg4  a4,  MyTestArg5  a5,  MyTestArg6  a6,
                        MyTestArg7  a7,  MyTestArg8  a8,  MyTestArg9  a9,
                        MyTestArg10 a10, MyTestArg11 a11, MyTestArg12 a12);
    MyInplaceTestObject(MyTestArg1  a1,  MyTestArg2  a2,  MyTestArg3  a3,
                        MyTestArg4  a4,  MyTestArg5  a5,  MyTestArg6  a6,
                        MyTestArg7  a7,  MyTestArg8  a8,  MyTestArg9  a9,
                        MyTestArg10 a10, MyTestArg11 a11, MyTestArg12 a12,
                        MyTestArg13 a13);
    MyInplaceTestObject(MyTestArg1  a1,  MyTestArg2  a2,  MyTestArg3  a3,
                        MyTestArg4  a4,  MyTestArg5  a5,  MyTestArg6  a6,
                        MyTestArg7  a7,  MyTestArg8  a8,  MyTestArg9  a9,
                        MyTestArg10 a10, MyTestArg11 a11, MyTestArg12 a12,
                        MyTestArg13 a13, MyTestArg14 a14);
        // Create a 'MyInplaceTestObject' by initializing the data members
        // 'd_a1'..'d_a14' with the specified 'a1'..'a14', and initializing any
        // remaining data members with their default value (-1).

    ~MyInplaceTestObject();
        // Increment the count of calls to this destructor, and destroy this
        // object.

    // ACCESSORS
    bool operator==(const MyInplaceTestObject& rhs) const;
        // Return 'true' if the specified 'rhs' has the same value as this
        // object, and 'false' otherwise.  Two 'MyInplaceTestObject' objects
        // have the same value if each of their corresponding data members
        // 'd1'..'d14' have the same value.

    static int getNumDeletes();
        // Return the number of times an object of this type has been
        // destroyed.
};

                         // -------------------------
                         // class MyInplaceTestObject
                         // -------------------------

int MyInplaceTestObject::s_numDeletes = 0;

// CREATORS
MyInplaceTestObject::MyInplaceTestObject()
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg1 a1)
: d_a1(a1)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg1 a1, MyTestArg2 a2)
: d_a1(a1)
, d_a2(a2)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg1 a1, MyTestArg2 a2,
                                         MyTestArg3 a3)
: d_a1(a1)
, d_a2(a2)
, d_a3(a3)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg1 a1, MyTestArg2 a2,
                                         MyTestArg3 a3, MyTestArg4 a4)
: d_a1(a1)
, d_a2(a2)
, d_a3(a3)
, d_a4(a4)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg1 a1, MyTestArg2 a2,
                                         MyTestArg3 a3, MyTestArg4 a4,
                                         MyTestArg5 a5)
: d_a1(a1)
, d_a2(a2)
, d_a3(a3)
, d_a4(a4)
, d_a5(a5)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg1 a1, MyTestArg2 a2,
                                         MyTestArg3 a3, MyTestArg4 a4,
                                         MyTestArg5 a5, MyTestArg6 a6)
: d_a1(a1)
, d_a2(a2)
, d_a3(a3)
, d_a4(a4)
, d_a5(a5)
, d_a6(a6)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg1 a1, MyTestArg2 a2,
                                         MyTestArg3 a3, MyTestArg4 a4,
                                         MyTestArg5 a5, MyTestArg6 a6,
                                         MyTestArg7 a7)
: d_a1(a1)
, d_a2(a2)
, d_a3(a3)
, d_a4(a4)
, d_a5(a5)
, d_a6(a6)
, d_a7(a7)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg1 a1, MyTestArg2 a2,
                                         MyTestArg3 a3, MyTestArg4 a4,
                                         MyTestArg5 a5, MyTestArg6 a6,
                                         MyTestArg7 a7, MyTestArg8 a8)
: d_a1(a1)
, d_a2(a2)
, d_a3(a3)
, d_a4(a4)
, d_a5(a5)
, d_a6(a6)
, d_a7(a7)
, d_a8(a8)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg1 a1, MyTestArg2 a2,
                                         MyTestArg3 a3, MyTestArg4 a4,
                                         MyTestArg5 a5, MyTestArg6 a6,
                                         MyTestArg7 a7, MyTestArg8 a8,
                                         MyTestArg9 a9)
: d_a1(a1)
, d_a2(a2)
, d_a3(a3)
, d_a4(a4)
, d_a5(a5)
, d_a6(a6)
, d_a7(a7)
, d_a8(a8)
, d_a9(a9)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg1 a1, MyTestArg2 a2,
                                         MyTestArg3 a3, MyTestArg4 a4,
                                         MyTestArg5 a5, MyTestArg6 a6,
                                         MyTestArg7 a7, MyTestArg8 a8,
                                         MyTestArg9 a9, MyTestArg10 a10)
: d_a1(a1)
, d_a2(a2)
, d_a3(a3)
, d_a4(a4)
, d_a5(a5)
, d_a6(a6)
, d_a7(a7)
, d_a8(a8)
, d_a9(a9)
, d_a10(a10)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg1  a1, MyTestArg2  a2,
                                         MyTestArg3  a3, MyTestArg4  a4,
                                         MyTestArg5  a5, MyTestArg6  a6,
                                         MyTestArg7  a7, MyTestArg8  a8,
                                         MyTestArg9  a9, MyTestArg10 a10,
                                         MyTestArg11 a11)
: d_a1(a1)
, d_a2(a2)
, d_a3(a3)
, d_a4(a4)
, d_a5(a5)
, d_a6(a6)
, d_a7(a7)
, d_a8(a8)
, d_a9(a9)
, d_a10(a10)
, d_a11(a11)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg1  a1,  MyTestArg2  a2,
                                         MyTestArg3  a3,  MyTestArg4  a4,
                                         MyTestArg5  a5,  MyTestArg6  a6,
                                         MyTestArg7  a7,  MyTestArg8  a8,
                                         MyTestArg9  a9,  MyTestArg10 a10,
                                         MyTestArg11 a11, MyTestArg12 a12)
: d_a1(a1)
, d_a2(a2)
, d_a3(a3)
, d_a4(a4)
, d_a5(a5)
, d_a6(a6)
, d_a7(a7)
, d_a8(a8)
, d_a9(a9)
, d_a10(a10)
, d_a11(a11)
, d_a12(a12)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg1  a1,  MyTestArg2  a2,
                                         MyTestArg3  a3,  MyTestArg4  a4,
                                         MyTestArg5  a5,  MyTestArg6  a6,
                                         MyTestArg7  a7,  MyTestArg8  a8,
                                         MyTestArg9  a9,  MyTestArg10 a10,
                                         MyTestArg11 a11, MyTestArg12 a12,
                                         MyTestArg13 a13)
: d_a1(a1)
, d_a2(a2)
, d_a3(a3)
, d_a4(a4)
, d_a5(a5)
, d_a6(a6)
, d_a7(a7)
, d_a8(a8)
, d_a9(a9)
, d_a10(a10)
, d_a11(a11)
, d_a12(a12)
, d_a13(a13)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg1  a1,  MyTestArg2  a2,
                                         MyTestArg3  a3,  MyTestArg4  a4,
                                         MyTestArg5  a5,  MyTestArg6  a6,
                                         MyTestArg7  a7,  MyTestArg8  a8,
                                         MyTestArg9  a9,  MyTestArg10 a10,
                                         MyTestArg11 a11, MyTestArg12 a12,
                                         MyTestArg13 a13, MyTestArg14 a14)
: d_a1(a1)
, d_a2(a2)
, d_a3(a3)
, d_a4(a4)
, d_a5(a5)
, d_a6(a6)
, d_a7(a7)
, d_a8(a8)
, d_a9(a9)
, d_a10(a10)
, d_a11(a11)
, d_a12(a12)
, d_a13(a13)
, d_a14(a14)
{
}

inline
MyInplaceTestObject::~MyInplaceTestObject()
{
    ++s_numDeletes;
}

// ACCESSORS
bool MyInplaceTestObject::operator==(const MyInplaceTestObject& rhs) const
{
    return d_a1  == rhs.d_a1  &&
           d_a1  == rhs.d_a1  &&
           d_a2  == rhs.d_a2  &&
           d_a3  == rhs.d_a3  &&
           d_a4  == rhs.d_a4  &&
           d_a5  == rhs.d_a5  &&
           d_a6  == rhs.d_a6  &&
           d_a7  == rhs.d_a7  &&
           d_a8  == rhs.d_a8  &&
           d_a9  == rhs.d_a9  &&
           d_a10 == rhs.d_a10 &&
           d_a11 == rhs.d_a11 &&
           d_a12 == rhs.d_a12 &&
           d_a13 == rhs.d_a13 &&
           d_a14 == rhs.d_a14;
}

int MyInplaceTestObject::getNumDeletes()
{
    return s_numDeletes;
}


                       // *** TEST DELETERS SECTION ***

                         // =========================
                         // class MyTestObjectFactory
                         // =========================

class MyTestObjectFactory {
    // This class implements a prototypical factory deleter that simply wraps a
    // 'bslma::Allocator' without implementing this protocol.

    // DATA
    bslma::Allocator *d_allocator_p;

  public:
    // CREATORS
    MyTestObjectFactory();
        // Create a 'MyTestObjectFactory' object that will destroy objects
        // using an unqualified call to 'delete'.

    explicit MyTestObjectFactory(bslma::Allocator *basicAllocator);
        // Create a 'MyTestObjectFactory' object that will destroy objects
        // using the 'deleteObject' method of the specified 'basicAllocator'.
        // If '0 == basicAllocator' then the default allocator currently
        // installed as this object is constructed will be used.

    // ACCESSORS
    void deleteObject(MyTestObject *obj) const;
        // Destroy the object pointed to by the specified 'obj' and reclaim its
        // memory using the allocator supplied at construction (of this
        // 'MyTestObjectFactory' object), or an unqualified call to 'delete' if
        // no such allocator was supplied.
};

                            // ===================
                            // class MyTestDeleter
                            // ===================

class MyTestDeleter {
    // This class provides a prototypical function-like deleter that optionally
    // can count the number of times it is called.

    // DATA
    bslma::Allocator *d_allocator_p;
    int              *d_callCount_p;

  public:
    // CREATORS
    explicit MyTestDeleter(bslma::Allocator *basicAllocator = 0,
                           int              *callCounter = 0);

    MyTestDeleter(const MyTestDeleter& original);
        // Create a copy of the specified 'original' object using the same
        // allocator to destroy objects passed to 'operator()', and the same
        // counter (if any) to count calls of that operator.

    // ACCESSORS
    template <class OBJECT_TYPE>
    void operator() (OBJECT_TYPE *ptr) const;
        // Destroy the object pointed to by the specified 'ptr' and reclaim its
        // memory using the allocator supplied at construction (of this
        // 'MyTestDeleter' object).  If a 'callCounter' was supplied when this
        // object was constructed, increment the referenced counter.

    bool operator==(const MyTestDeleter& rhs) const;
        // Return 'true' if this 'MyTestDeleter' object has the same value as
        // the specified 'rhs' and 'false' otherwise.  Two 'MyTestDeleter'
        // object have the same value if they use the same allocator to destroy
        // objects passed to 'operator()'.
};

BSLMF_ASSERT(!(bslalg::HasTrait<MyTestDeleter,
                                bslalg::TypeTraitUsesBslmaAllocator>::VALUE));

                         // ========================
                         // class MyAllocTestDeleter
                         // ========================

class MyAllocTestDeleter {
    // This class provides a prototypical function-like deleter that takes a
    // 'bslma::Allocator' at construction.  It is used to check that the
    // allocator used to construct the representation is passed correctly to
    // the deleter.

    // DATA
    bslma::Allocator *d_allocator_p;   // allocator for this object's state
    bslma::Allocator *d_deleter_p;     // allocator to use as a deleter
    void             *d_someMemory;    // dynamically allocated state

  public:
    // CREATORS
    explicit MyAllocTestDeleter(bslma::Allocator *deleter,
                                bslma::Allocator *basicAllocator = 0);
        // Create a 'MyAllocTestDeleter' using the specified 'deleter' to
        // destroy objects passed to the overloaded function call operator, and
        // using the optionally specified 'basicAllocator' to allocate some
        // additional state for test purposes only.  If no allocator is
        // supplied then the currently installed default allocator is used to
        // supply memory.

    MyAllocTestDeleter(const MyAllocTestDeleter&  original,
                       bslma::Allocator          *basicAllocator = 0);
        // Create a 'MyAllocTestDeleter' object having the same deleter as the
        // specified 'original' object, and having a copy of the dummy state.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    ~MyAllocTestDeleter();
        // Destroy this object.

    // MANIPULATORS
    MyAllocTestDeleter& operator=(const MyAllocTestDeleter& rhs);
        // Assign to this object the deleter of the specified 'rhs'.

    // ACCESSORS
    template <class OBJECT_TYPE>
    void operator()(OBJECT_TYPE *ptr) const;
        // Destroy the object pointed to by the specified 'ptr' using the
        // deleter supplied to this object's constructor.
};

                        // ======================
                        // class TestSharedPtrRep
                        // ======================

template <class TYPE>
class TestSharedPtrRep : public bslma::SharedPtrRep {
    // Partially implemented shared pointer representation ("letter") protocol.
    // This class provides accessors to report the number of times the disposal
    // methods 'disposeObject' and 'disposeRep' have been called.  When a call
    // to 'disposeRep' decrements the number references to zero, this object
    // destroys itself.

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

    // VIRTUAL (OVERRIDE) MANIPULATORS
    virtual void disposeObject();
        // Release the value stored by this representation.

    virtual void disposeRep();
        // Release this representation.

    virtual void *getDeleter(const std::type_info&) { return 0; }
        // Return a pointer to the deleter stored by the derived representation
        // (if any) if the deleter has the same type as that described by the
        // specified 'type', and a null pointer otherwise.

    // VIRTUAL (OVERRIDE) ACCESSORS
    virtual void *originalPtr() const;
        // Return the original pointer stored by this representation.

    // ACCESSORS
    int disposeObjectCount() const;
        // Return the number of time 'releaseValue' was called.

    int disposeRepCount() const;
        // Return the number of time 'release' was called.

    TYPE *ptr() const;
        // Return the data pointer stored by this representation.
};

template <class POINTER>
struct PerformanceTester
{
    // This class template provides a namespace for utilities to benchmark and
    // otherwise test the performance of a shared pointer implementation.  The
    // 'struct' is parameterized on the shared pointer it is instantiated with
    // in order to easily compare the performance of different implementations.

    static void test(bool verbose, bool allocVerbose);
        // Run a performance test of a broad spectrum of performance test using
        // the specified 'verbose' flag to indicate the desired level of
        // feedback to the user, and the specified 'allocVerbose' to indicate
        // the level of feedback on allocator operations.
};

class ShareThis : public bsl::enable_shared_from_this<ShareThis>
{
  protected:
    int *d_destructorCount_p;

  public:
    // CREATORS
    explicit ShareThis(int *destructorCount) : d_destructorCount_p(destructorCount) {}
    virtual ~ShareThis() { ++*d_destructorCount_p; }
};

class ShareThisDerived : public ShareThis
{
  public:
    // CREATORS
    explicit ShareThisDerived(int *destructorCount) : ShareThis(destructorCount) {}
    ~ShareThisDerived() { *d_destructorCount_p += 10; }
};

// Traits for test types:
namespace BloombergLP {
namespace bslma {
template <>
struct UsesBslmaAllocator<MyTestObjectFactory>
     : bsl::false_type {};

template <>
struct UsesBslmaAllocator<MyAllocTestDeleter>
     : bsl::true_type {};

template <class TYPE>
struct UsesBslmaAllocator<TestSharedPtrRep<TYPE> >
     : bsl::true_type {};

}  // close namespace bslma
}  // close enterprise namespace

// ============================================================================
//                      MEMBER- AND TEMPLATE-FUNCTION IMPLEMENTATIONS
// ============================================================================

                             // ------------------
                             // class MyTestObject
                             // ------------------

// CREATORS
MyTestObject::MyTestObject(const MyTestObject& original)
: MyTestBaseObject()
, d_deleteCounter_p(original.d_deleteCounter_p)
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

                         // -------------------------
                         // class MyTestDerivedObject
                         // -------------------------

// CREATORS
MyTestDerivedObject::MyTestDerivedObject(const MyTestObject& original)
: MyTestObject(original)
{
}

MyTestDerivedObject::MyTestDerivedObject(bsls::Types::Int64 *counter,
                                         bsls::Types::Int64 *copyCounter)
: MyTestObject(counter, copyCounter)
{
}

                           // ------------------------------
                           // function myTestDeleterFunction
                           // ------------------------------

void myTestDeleterFunction(MyTestObject *ptr)
{
    delete ptr;
}

                         // -------------------------
                         // class MyTestObjectFactory
                         // -------------------------

// CREATORS
MyTestObjectFactory::MyTestObjectFactory()
: d_allocator_p(0)
{
}

MyTestObjectFactory::MyTestObjectFactory(bslma::Allocator* basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

// ACCESSORS
void MyTestObjectFactory::deleteObject(MyTestObject *obj) const
{
    if (d_allocator_p) {
        d_allocator_p->deleteObject(obj);
    }
    else {
        delete obj;
    }
}

                            // -------------------
                            // class MyTestDeleter
                            // -------------------

// CREATORS
MyTestDeleter::MyTestDeleter(bslma::Allocator *basicAllocator,
                             int              *callCounter)
: d_allocator_p(basicAllocator)
, d_callCount_p(callCounter)
{
}

MyTestDeleter::MyTestDeleter(const MyTestDeleter& original)
: d_allocator_p(original.d_allocator_p)
, d_callCount_p(original.d_callCount_p)
{
}

template <class OBJECT_TYPE>
void MyTestDeleter::operator() (OBJECT_TYPE *ptr) const
{
    if (d_callCount_p) {
        ++(*d_callCount_p);
    }

    bslma::Allocator *ba = bslma::Default::allocator(d_allocator_p);
    ba->deleteObject(ptr);
}

bool MyTestDeleter::operator==(const MyTestDeleter& rhs) const
{
    return d_allocator_p == rhs.d_allocator_p;
}

                          // ------------------------
                          // class MyAllocTestDeleter
                          // ------------------------

// CREATORS
MyAllocTestDeleter::MyAllocTestDeleter(bslma::Allocator *deleter,
                                       bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_deleter_p(deleter)
{
    d_someMemory = d_allocator_p->allocate(13);
}

MyAllocTestDeleter::MyAllocTestDeleter(
                                     const MyAllocTestDeleter&  original,
                                     bslma::Allocator          *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_deleter_p(original.d_deleter_p)
{
    d_someMemory = d_allocator_p->allocate(13);
}

MyAllocTestDeleter::~MyAllocTestDeleter()
{
    d_allocator_p->deallocate(d_someMemory);
}

// MANIPULATORS
MyAllocTestDeleter& MyAllocTestDeleter::operator=(
                                                 const MyAllocTestDeleter& rhs)
{
    ASSERT(!"I think we do not use operator =");
    d_deleter_p = rhs.d_deleter_p;
    return *this;
}

// ACCESSORS
template <class OBJECT_TYPE>
void MyAllocTestDeleter::operator()(OBJECT_TYPE *ptr) const
{
    d_deleter_p->deleteObject(ptr);
}

                     // --------------------------
                     // class MyInstrumentedObject
                     // --------------------------

// CREATORS
MyInstrumentedObject::MyInstrumentedObject(int  *constructCounter,
                                           int  *destroyCounter,
                                           bool  throwAfterInit)
: d_constructCounter_p(constructCounter)
, d_destroyCounter_p(destroyCounter)
{
    BSLS_ASSERT(constructCounter);
    BSLS_ASSERT(destroyCounter);

    if (throwAfterInit) {
        BSLS_THROW( ConstructorFailed() );
    }

    ++*d_constructCounter_p;
}

MyInstrumentedObject::~MyInstrumentedObject()
{
    BSLS_ASSERT(d_destroyCounter_p);

    ++*d_destroyCounter_p;
}

// ACCESSORS
inline
int *MyInstrumentedObject::constructCounter() const
{
    return d_constructCounter_p;
}

inline
int *MyInstrumentedObject::destroyCounter() const
{
    return d_destroyCounter_p;
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
TestSharedPtrRep<TYPE>::TestSharedPtrRep(TYPE             *dataPtr_p,
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

// ============================================================================
//              INLINE FUNCTION AND FUNCTION TEMPLATE DEFINITIONS
// ============================================================================

                         // ------------------------
                         // struct PerformanceTester
                         // ------------------------

static
inline
void printPerformanceStats(bsls::Types::Int64 numAllocations,
                           bsls::Types::Int64 numBytes,
                           bsls::Types::Int64 numCopies,
                           bsls::Types::Int64 numDeletes)
    // Print to the console a single line report of the current usage stats of
    // an allocator, consisting of the specified 'numAllocations', 'numBytes',
    // 'numCopies' and 'numDeletes'.
{
    printf("\t%lld allocations, %lld bytes\n"
           "\t%lld copies of test objects\n"
           "\t%lld deletions of test objects\n",
           numAllocations,
           numBytes,
           numCopies,
           numDeletes);
}

template <class POINTER>
void PerformanceTester<POINTER>::test(bool verbose, bool allocVerbose)
{
    bslma::TestAllocator ta(allocVerbose);

    enum {
        NUM_ITER        = 1000,
        VECTOR_SIZE     = 1000,
        BIG_VECTOR_SIZE = NUM_ITER * VECTOR_SIZE
    };

    bsls::Types::Int64 deleteCounter, copyCounter, numAlloc, numBytes;

    bsl::vector<TObj *>        mZ(&ta);
    const bsl::vector<TObj *>& Z = mZ;

    bsls::Stopwatch timer;

    mZ.resize(BIG_VECTOR_SIZE);
    deleteCounter = copyCounter = 0;
    numAlloc = ta.numAllocations();
    numBytes = ta.numBytesInUse();
    timer.start();
    for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
        mZ[i] = new(ta) TObj(&deleteCounter, &copyCounter);
    }
    timer.stop();
    printf("Creating %d owned objects in %gs (%gs each)\n",
           static_cast<int>(BIG_VECTOR_SIZE),
           timer.elapsedTime(),
           timer.elapsedTime() / BIG_VECTOR_SIZE);
    if (verbose) {
        printPerformanceStats(ta.numAllocations() - numAlloc,
                              ta.numBytesInUse() - numBytes,
                              copyCounter,
                              deleteCounter);
    }

    for (int i = 1; i < BIG_VECTOR_SIZE; ++i) {
        mZ[i]->~TObj();
    }

    deleteCounter = copyCounter = 0;
    numAlloc = ta.numAllocations();
    numBytes = ta.numBytesInUse();
    timer.start();
    for (int i = 1; i < BIG_VECTOR_SIZE; ++i) {
        new(mZ[i]) TObj(*Z[0]);
    }
    timer.stop();
    printf("Copy-constructing %d owned objects in %gs (%gs each)\n",
           static_cast<int>(BIG_VECTOR_SIZE - 1),
           timer.elapsedTime(),
           timer.elapsedTime() / (BIG_VECTOR_SIZE-1));
    if (verbose) {
        printPerformanceStats(ta.numAllocations() - numAlloc,
                              ta.numBytesInUse() - numBytes,
                              copyCounter,
                              deleteCounter);
    }

    deleteCounter = copyCounter = 0;
    numAlloc = ta.numAllocations();
    numBytes = ta.numBytesInUse();
    timer.start();
    for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
        static_cast<bslma::Allocator *>(&ta)->deleteObject(mZ[i]);
    }
    timer.stop();
    printf("Destroying %d owned objects in %gs (%gs each)\n",
           static_cast<int>(BIG_VECTOR_SIZE),
           timer.elapsedTime(),
           timer.elapsedTime() / BIG_VECTOR_SIZE);
    if (verbose) {
        printPerformanceStats(ta.numAllocations() - numAlloc,
                              ta.numBytesInUse() - numBytes,
                              copyCounter,
                              deleteCounter);
    }

    deleteCounter = copyCounter = 0;
    numAlloc = ta.numAllocations();
    numBytes = ta.numBytesInUse();
    for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
        mZ[i] = new(ta) TObj(&deleteCounter, &copyCounter);
    }
    printf("Rehydrated %d owned objects\n", static_cast<int>(BIG_VECTOR_SIZE));
    if (verbose) {
        printPerformanceStats(ta.numAllocations() - numAlloc,
                              ta.numBytesInUse() - numBytes,
                              copyCounter,
                              deleteCounter);
    }

    bsl::vector<POINTER> mX(&ta);
    const bsl::vector<POINTER>& X = mX;

    // -------------------------------------------------------------------
    printf("\nCreating out-of-place representations."
           "\n--------------------------------------\n");

    mX.resize(BIG_VECTOR_SIZE);
    for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
        // We first destroy the contents of mX in order to be able to recreate
        // them in place.  Using push_back instead would involve an additional
        // creation (for a temporary) and copy construction into the vector,
        // which is not what we intend to measure.

        (&mX[i])->~POINTER();
    }
    timer.reset();
    deleteCounter = copyCounter = 0;
    numAlloc = ta.numAllocations();
    numBytes = ta.numBytesInUse();
    timer.start();
    for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
        // If this code throws an exception, then the remaining elements will
        // be destroyed twice, once above and another time with the destruction
        // of mX.  But that is OK since they are empty.

        new(&mX[i]) POINTER(Z[i], &ta);
    }
    timer.stop();
    printf("Creating %d distinct shared pointers in %gs (%gs each)\n",
           static_cast<int>(BIG_VECTOR_SIZE),
           timer.elapsedTime(),
           timer.elapsedTime() / BIG_VECTOR_SIZE);
    if (verbose) {
        printPerformanceStats(ta.numAllocations() - numAlloc,
                              ta.numBytesInUse() - numBytes,
                              copyCounter,
                              deleteCounter);
    }

    timer.reset();
    deleteCounter = copyCounter = 0;
    numAlloc = ta.numAllocations();
    numBytes = ta.numBytesInUse();
    timer.start();
    for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
        (&mX[i])->~POINTER();
    }
    timer.stop();
    printf("Destroying %d distinct shared pointers in %gs (%gs each)\n",
           static_cast<int>(BIG_VECTOR_SIZE),
           timer.elapsedTime(),
           timer.elapsedTime() / BIG_VECTOR_SIZE);
    if (verbose) {
        printPerformanceStats(ta.numAllocations() - numAlloc,
                              ta.numBytesInUse() - numBytes,
                              copyCounter,
                              deleteCounter);
    }

    // Note:  Z now contains dangling pointers.  Rehydrate!
    deleteCounter = copyCounter = 0;
    numAlloc = ta.numAllocations();
    numBytes = ta.numBytesInUse();
    for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
        mZ[i] = new(ta) TObj(&deleteCounter, &copyCounter);
    }
    printf("Rehydrated %d owned objects\n", static_cast<int>(BIG_VECTOR_SIZE));
    if (verbose) {
        printPerformanceStats(ta.numAllocations() - numAlloc,
                              ta.numBytesInUse() - numBytes,
                              copyCounter,
                              deleteCounter);
    }

    {
        POINTER Y(Z[0], &ta);
        timer.reset();
        deleteCounter = copyCounter = 0;
        numAlloc = ta.numAllocations();
        numBytes = ta.numBytesInUse();
        timer.start();
        for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
            new(&mX[i]) POINTER(Y);
        }
        timer.stop();
        printf(
           "Creating %d copies of the same shared pointer in %gs (%gs each)\n",
           static_cast<int>(BIG_VECTOR_SIZE),
           timer.elapsedTime(),
           timer.elapsedTime() / BIG_VECTOR_SIZE);
        if (verbose) {
            printPerformanceStats(ta.numAllocations() - numAlloc,
                                  ta.numBytesInUse() - numBytes,
                                  copyCounter,
                                  deleteCounter);
        }
    }

    timer.reset();
    deleteCounter = copyCounter = 0;
    numAlloc = ta.numAllocations();
    numBytes = ta.numBytesInUse();
    timer.start();
    for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
        (&mX[i])->~POINTER();
    }
    timer.stop();
    printf("Destroying %d times the same shared pointer in %gs (%gs each)\n",
           static_cast<int>(BIG_VECTOR_SIZE),
           timer.elapsedTime(),
           timer.elapsedTime() / BIG_VECTOR_SIZE);
    if (verbose) {
        printPerformanceStats(ta.numAllocations() - numAlloc,
                              ta.numBytesInUse() - numBytes,
                              copyCounter,
                              deleteCounter);
    }

    // Note:  Z[0] is now dangling, and X contains only empty shared pointers.
    // Rehydrate, but with empty shared pointers!
    deleteCounter = copyCounter = 0;
    numAlloc = ta.numAllocations();
    numBytes = ta.numBytesInUse();
    mZ[0] = new(ta) TObj(&deleteCounter, &copyCounter);
    for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
        new(&mX[i]) POINTER();
    }
    printf("Rehydrated 1 owned object and %d empty shared pointers\n",
            static_cast<int>(BIG_VECTOR_SIZE));
    if (verbose) {
        printPerformanceStats(ta.numAllocations() - numAlloc,
                              ta.numBytesInUse() - numBytes,
                              copyCounter,
                              deleteCounter);
    }

    printf("\nCreating in-place representations."
           "\n----------------------------------\n");

    timer.reset();
    deleteCounter = copyCounter = 0;
    numAlloc = ta.numAllocations();
    numBytes = ta.numBytesInUse();
    timer.start();
    for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
        mX[i].createInplace(&ta, *Z[i]);
    }
    timer.stop();
    printf("Creating %d distinct in-place shared pointers in %gs (%gs each)\n",
           static_cast<int>(BIG_VECTOR_SIZE),
           timer.elapsedTime(),
           timer.elapsedTime() / BIG_VECTOR_SIZE);
    if (verbose) {
        printPerformanceStats(ta.numAllocations() - numAlloc,
                              ta.numBytesInUse() - numBytes,
                              copyCounter,
                              deleteCounter);
    }

    timer.reset();
    deleteCounter = copyCounter = 0;
    numAlloc = ta.numAllocations();
    numBytes = ta.numBytesInUse();
    timer.start();
    for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
        (&mX[i])->~POINTER();
    }
    timer.stop();
    printf(
         "Destroying %d distinct in-place shared pointers in %gs (%gs each)\n",
         static_cast<int>(BIG_VECTOR_SIZE),
         timer.elapsedTime(),
         timer.elapsedTime() / BIG_VECTOR_SIZE);
    if (verbose) {
        printPerformanceStats(ta.numAllocations() - numAlloc,
                              ta.numBytesInUse() - numBytes,
                              copyCounter,
                              deleteCounter);
    }

    printf("\nCreating aliased shared pointers."
           "\n---------------------------------\n");

    {
        POINTER Y(Z[0], &ta);
        timer.reset();
        deleteCounter = copyCounter = 0;
        numAlloc = ta.numAllocations();
        numBytes = ta.numBytesInUse();
        timer.start();
        for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
            new(&mX[i]) POINTER(Y, Z[i]);
        }
        timer.stop();
        printf("Creating %d aliases of the same shared pointer in %gs"
               " (%gs each)\n",
               static_cast<int>(BIG_VECTOR_SIZE),
               timer.elapsedTime(),
               timer.elapsedTime() / BIG_VECTOR_SIZE);
        if (verbose) {
            printPerformanceStats(ta.numAllocations() - numAlloc,
                                  ta.numBytesInUse() - numBytes,
                                  copyCounter,
                                  deleteCounter);
        }
    }

    timer.reset();
    deleteCounter = copyCounter = 0;
    numAlloc = ta.numAllocations();
    numBytes = ta.numBytesInUse();
    timer.start();
    for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
        (&mX[i])->~POINTER();
    }
    timer.stop();
    printf("Destroying %d aliases of the same shared pointer in %gs"
           " (%gs each)\n",
           static_cast<int>(BIG_VECTOR_SIZE),
           timer.elapsedTime(),
           timer.elapsedTime() / BIG_VECTOR_SIZE);
    if (verbose) {
        printPerformanceStats(ta.numAllocations() - numAlloc,
                              ta.numBytesInUse() - numBytes,
                              copyCounter,
                              deleteCounter);
    }

    // Note:  Z[0] is now dangling, and X contains only empty shared pointers.
    // Rehydrate!
    deleteCounter = copyCounter = 0;
    numAlloc = ta.numAllocations();
    numBytes = ta.numBytesInUse();
    mZ[0] = new(ta) TObj(&deleteCounter, &copyCounter);
    for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
        new(&mX[i]) POINTER(Z[i], &ta);
    }
    printf("Rehydrated 1 owned object and %d shared pointers\n",
           static_cast<int>(BIG_VECTOR_SIZE));
    if (verbose) {
        printPerformanceStats(ta.numAllocations() - numAlloc,
                              ta.numBytesInUse() - numBytes,
                              copyCounter,
                              deleteCounter);
    }

    // -------------------------------------------------------------------
    printf("\nAssignment."
           "\n-----------\n");

    timer.reset();
    deleteCounter = copyCounter = 0;
    numAlloc = ta.numAllocations();
    numBytes = ta.numBytesInUse();
    timer.start();
    {
        POINTER Y = X[0];
        for (int j = 1; j < BIG_VECTOR_SIZE; ++j) {
            mX[j - 1] = X[j];
        }
        mX.back() = Y;
    }
    timer.stop();
    printf("Assigning %d distinct shared pointers in %gs (%gs each)\n",
           static_cast<int>(BIG_VECTOR_SIZE + 1),
           timer.elapsedTime(),
           timer.elapsedTime() / (BIG_VECTOR_SIZE+1));
    if (verbose) {
        printPerformanceStats(ta.numAllocations() - numAlloc,
                              ta.numBytesInUse() - numBytes,
                              copyCounter,
                              deleteCounter);
    }

    timer.reset();
    deleteCounter = copyCounter = 0;
    numAlloc = ta.numAllocations();
    numBytes = ta.numBytesInUse();
    timer.start();
    {
        POINTER Y = X[0];
        for (int j = 1; j < BIG_VECTOR_SIZE; ++j) {
            mX[j] = Y;
        }
    }
    timer.stop();
    printf("Assigning %d times the same shared pointer in %gs (%gs each)\n",
           static_cast<int>(BIG_VECTOR_SIZE),
           timer.elapsedTime(),
           timer.elapsedTime() / BIG_VECTOR_SIZE);
    if (verbose) {
        printPerformanceStats(ta.numAllocations() - numAlloc,
                              ta.numBytesInUse() - numBytes,
                              copyCounter,
                              deleteCounter);
    }

    // Note:  Z now contains dangling pointers, except Z[0].  Rehydrate!
    // Note:  Z now contains dangling pointers.  Rehydrate!
    deleteCounter = copyCounter = 0;
    numAlloc = ta.numAllocations();
    numBytes = ta.numBytesInUse();
    for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
        mZ[i] = new(ta) TObj(&deleteCounter, &copyCounter);
    }
    printf("Rehydrated %d owned objects\n", static_cast<int>(BIG_VECTOR_SIZE));

    // -------------------------------------------------------------------
    printf("\nPooling out-of-place representations."
           "\n-------------------------------------\n");

    // TBD

    // -------------------------------------------------------------------
    for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
        ta.deleteObject(mZ[i]);
    }
}

template <class TYPE>
class ManagedPtrTestDeleter {

    TYPE *d_providedObj; // Address of the last object passed to 'deleteObject'

  public:
    ManagedPtrTestDeleter() : d_providedObj(0) {}
        // Create a 'ManagedPtrTestDeleter' object that has not yet destroyed
        // any objects.

    void deleteObject(TYPE *obj)
        // Record an attempt to destroy the specified 'obj', but otherwise take
        // no action to destroy the object pointed to by 'obj', nor reclaim its
        // memory or other resources.
    {
        ASSERT(static_cast<bool>(0 == d_providedObj));
        d_providedObj = obj;
    }

    TYPE *providedObj()
        // Return the address of the last object passed to a 'deleteObject'
        // call of this object.  Return a null pointer if there have been no
        // calls to 'deleteObject'.
    {
        return d_providedObj;
    }

    void reset()
        // Reset this object back to a default state where 'deleteObject' has
        // not been called.
    {
        d_providedObj = 0;
    }
};

class SelfReference
{
    // This class provides support for testing the correct lifetime of objects
    // that contain a shared pointer reference to themselves.

    // DATA
    bsl::shared_ptr<SelfReference> d_dataPtr;

  public:
    // MANIPULATORS
    void release() { d_dataPtr.reset(); }
        // Clear the internal reference of this object.

    void setData(const bsl::shared_ptr<SelfReference>& value)
        // Set the internal reference of this object to the specified 'value'.
    {
        d_dataPtr = value;
    }
};


std::auto_ptr<MyTestObject> makeAuto()
    // Return an empty 'auto_ptr' rvalue.
{
    return std::auto_ptr<TObj>(static_cast<TObj *>(0));
}

std::auto_ptr<MyTestObject> makeAuto(bsls::Types::Int64 *counter)
    // Return an 'auto_ptr' rvalue owning a new object constructed using the
    // specified 'counter'.
{
    BSLS_ASSERT_OPT(counter);

    return std::auto_ptr<TObj>(new TObj(counter));
}

//=============================================================================
//                          TEST CASE TEMPLATES
//-----------------------------------------------------------------------------

namespace TestDriver {

template <class TYPE>
void doNotDelete(TYPE *) {}
    // Do nothing

}  // close namespace TestDriver


template <class ALLOCATOR>
struct TestHarness {
    static void testCase33(bool verbose,
                           bool veryVerbose,
                           bool veryVeryVerbose,
                           bool veryVeryVeryVerbose);
        // Implement test case 33 using the specified 'verbose', 'veryVerbose',
        // 'veryVeryVerbose' and 'veryVeryVeryVerbose' flags to control the
        // level of user feedback.  See the test case function for documented
        // concerns and test plan.
};

template <class ALLOCATOR>
void TestHarness<ALLOCATOR>::testCase33(bool verbose,
                                        bool veryVerbose,
                                        bool veryVeryVerbose,
                                        bool veryVeryVeryVerbose)
{
    // --------------------------------------------------------------------
    // TESTING 'allocate_shared<T>(A, ...)'
    //
    // Concerns:
    //   All constructor is able to initialize the object correctly.
    //
    // Plan:
    //   Call all 14 different constructors and supply it with the
    //   appropriate arguments.  Then verify that the object created inside
    //   the representation is initialized using the arguments supplied.
    //
    // Testing:
    //   shared_ptr<T> allocate_shared<T, A>(A)
    //   shared_ptr<T> allocate_shared<T, A>(A, const A1& a1)
    //   shared_ptr<T> allocate_shared<T, A>(A, const A1& a1,..&a2)
    //   shared_ptr<T> allocate_shared<T, A>(A, const A1& a1,..&a3)
    //   shared_ptr<T> allocate_shared<T, A>(A, const A1& a1,..&a4)
    //   shared_ptr<T> allocate_shared<T, A>(A, const A1& a1,..&a5)
    //   shared_ptr<T> allocate_shared<T, A>(A, const A1& a1,..&a6)
    //   shared_ptr<T> allocate_shared<T, A>(A, const A1& a1,..&a7)
    //   shared_ptr<T> allocate_shared<T, A>(A, const A1& a1,..&a8)
    //   shared_ptr<T> allocate_shared<T, A>(A, const A1& a1,..&a9)
    //   shared_ptr<T> allocate_shared<T, A>(A, const A1& a1,..&a10)
    //   shared_ptr<T> allocate_shared<T, A>(A, const A1& a1,..&a11)
    //   shared_ptr<T> allocate_shared<T, A>(A, const A1& a1,..&a12)
    //   shared_ptr<T> allocate_shared<T, A>(A, const A1& a1,..&a13)
    //   shared_ptr<T> allocate_shared<T, A>(A, const A1& a1,..&a14)
    // --------------------------------------------------------------------
    if (verbose) printf("\nTESTING 'allocate_shared<T>(A, ...)'"
                        "\n====================================\n");

    (void)veryVerbose;
    (void)veryVeryVerbose;

    typedef typename bsl::allocator_traits<ALLOCATOR>::
                          template rebind_traits<MyTestObject> Obj_AllocTraits;
    typedef typename Obj_AllocTraits::allocator_type Obj_Alloc;

    typedef typename bsl::allocator_traits<ALLOCATOR>::
                 template rebind_traits<MyInplaceTestObject> TCObj_AllocTraits;
    typedef typename TCObj_AllocTraits::allocator_type TCObj_Alloc;

    static const MyTestArg1  V1(1);
    static const MyTestArg2  V2(20);
    static const MyTestArg3  V3(23);
    static const MyTestArg4  V4(44);
    static const MyTestArg5  V5(66);
    static const MyTestArg6  V6(176);
    static const MyTestArg7  V7(878);
    static const MyTestArg8  V8(8);
    static const MyTestArg9  V9(912);
    static const MyTestArg10 V10(102);
    static const MyTestArg11 V11(111);
    static const MyTestArg12 V12(333);
    static const MyTestArg13 V13(712);
    static const MyTestArg14 V14(1414);

    bslma::TestAllocator ta("Tese case 3", veryVeryVeryVerbose);
    ALLOCATOR alloc_base(&ta);

    Obj_Alloc alloc1(alloc_base);
    TCObj_Alloc alloc2(&ta);

    if (verbose) printf("\nTesting constructor with no arguments"
                        "\n-------------------------------------\n");

    bsls::Types::Int64 numAllocations = ta.numAllocations();
    bsls::Types::Int64 numDeallocations = ta.numDeallocations();
    {
        static const MyInplaceTestObject EXP = MyInplaceTestObject();

        {
            TCObj x = bsl::allocate_shared<MyInplaceTestObject>(alloc2);
            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *x);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
    }

    if (verbose) printf("\nTesting constructor with 1 argument"
                        "\n-----------------------------------\n");

    numAllocations = ta.numAllocations();
    numDeallocations = ta.numDeallocations();
    {
        static const MyInplaceTestObject EXP(V1);

        {
            TCObj x = bsl::allocate_shared<MyInplaceTestObject>(alloc2, V1);
            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *x);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
    }

    if (verbose) printf("\nTesting contructor with 2 arguments"
                        "\n-----------------------------------\n");

    numAllocations = ta.numAllocations();
    numDeallocations = ta.numDeallocations();
    {
        static const MyInplaceTestObject EXP(V1, V2);

        {
            TCObj x = bsl::allocate_shared<MyInplaceTestObject>(alloc2,
                                                                V1, V2);
            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *x);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
    }

    if (verbose) printf("\nTesting constructor with 3 arguments"
                        "\n------------------------------------\n");

    numAllocations = ta.numAllocations();
    numDeallocations = ta.numDeallocations();
    {
        static const MyInplaceTestObject EXP(V1, V2, V3);

        {
            TCObj x = bsl::allocate_shared<MyInplaceTestObject>(alloc2,
                                                                V1, V2, V3);
            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *x);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
    }

    if (verbose) printf("\nTesting constructor with 4 arguments"
                        "\n------------------------------------\n");

    numAllocations = ta.numAllocations();
    numDeallocations = ta.numDeallocations();
    {
        static const MyInplaceTestObject EXP(V1, V2, V3, V4);

        {
            TCObj x = bsl::allocate_shared<MyInplaceTestObject>(alloc2,
                                                               V1, V2, V3, V4);
            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *x);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
    }

    if (verbose) printf("\nTesting constructor with 5 arguments"
                        "\n------------------------------------\n");

    numAllocations = ta.numAllocations();
    numDeallocations = ta.numDeallocations();
    {
        static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5);

        {
            TCObj x = bsl::allocate_shared<MyInplaceTestObject>(alloc2,
                                                           V1, V2, V3, V4, V5);
            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *x);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
    }

    if (verbose) printf("\nTesting constructor with 6 arguments"
                        "\n------------------------------------\n");

    numAllocations = ta.numAllocations();
    numDeallocations = ta.numDeallocations();
    {
        static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6);

        {
            TCObj x = bsl::allocate_shared<MyInplaceTestObject>(alloc2,
                                                       V1, V2, V3, V4, V5, V6);
            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *x);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
    }

    if (verbose) printf("\nTesting constructor with 7 arguments"
                        "\n------------------------------------\n");

    numAllocations = ta.numAllocations();
    numDeallocations = ta.numDeallocations();
    {
        static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7);

        {
            TCObj x = bsl::allocate_shared<MyInplaceTestObject>(alloc2,
                                                   V1, V2, V3, V4, V5, V6, V7);
            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *x);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
    }

    if (verbose) printf("\nTesting constructor with 8 arguments"
                        "\n------------------------------------\n");

    numAllocations = ta.numAllocations();
    numDeallocations = ta.numDeallocations();
    {
        static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7, V8);

        {
            TCObj x = bsl::allocate_shared<MyInplaceTestObject>(alloc2,
                                               V1, V2, V3, V4, V5, V6, V7, V8);
            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *x);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
    }

    if (verbose) printf("\nTesting constructor with 9 arguments"
                        "\n------------------------------------\n");

    numAllocations = ta.numAllocations();
    numDeallocations = ta.numDeallocations();
    {
        static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                                                                       V8, V9);

        {
            TCObj x = bsl::allocate_shared<MyInplaceTestObject>(alloc2,
                                           V1, V2, V3, V4, V5, V6, V7, V8, V9);
            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *x);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
    }

    if (verbose) printf("\nTesting constructor with 10 arguments"
                        "\n-------------------------------------\n");

    numAllocations = ta.numAllocations();
    numDeallocations = ta.numDeallocations();
    {
        static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                                                                  V8, V9, V10);

        {
            TCObj x = bsl::allocate_shared<MyInplaceTestObject>(alloc2,
                                      V1, V2, V3, V4, V5, V6, V7, V8, V9, V10);
            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *x);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
    }

    if (verbose) printf("\nTesting constructor with 11 arguments"
                        "\n-------------------------------------\n");

    numAllocations = ta.numAllocations();
    numDeallocations = ta.numDeallocations();
    {
        static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                                                             V8, V9, V10, V11);

        {
            TCObj x = bsl::allocate_shared<MyInplaceTestObject>(alloc2,
                                 V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11);
            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *x);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
    }

    if (verbose) printf("\nTesting constructor with 12 arguments"
                        "\n-------------------------------------\n");

    numAllocations = ta.numAllocations();
    numDeallocations = ta.numDeallocations();
    {
        static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                                                        V8, V9, V10, V11, V12);

        {
            TCObj x = bsl::allocate_shared<MyInplaceTestObject>(alloc2,
                            V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12);
            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *x);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
    }

    if (verbose) printf("\nTesting constructor with 13 arguments"
                        "\n-------------------------------------\n");

    numAllocations = ta.numAllocations();
    numDeallocations = ta.numDeallocations();
    {
        static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                                                   V8, V9, V10, V11, V12, V13);

        {
            TCObj x = bsl::allocate_shared<MyInplaceTestObject>(alloc2,
                       V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12, V13);
            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *x);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
    }

    if (verbose) printf("\nTesting constructor with 14 arguments"
                        "\n-------------------------------------\n");

    numAllocations = ta.numAllocations();
    numDeallocations = ta.numDeallocations();
    {
        static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                                              V8, V9, V10, V11, V12, V13, V14);

        {
            TCObj x = bsl::allocate_shared<MyInplaceTestObject>(alloc2,
                  V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12, V13, V14);
            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *x);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
    }
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

    printf("TEST " __FILE__ " CASE %d\n", test);

    typedef bsl::shared_ptr<MyTestObject>         ObjSP;
    typedef bsl::weak_ptr<MyTestObject>           ObjWP;
    typedef bsl::weak_ptr<MyTestDerivedObject>    DerivedWP;
    typedef bsl::shared_ptr<MyTestDerivedObject>  DerivedSP;

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // Confirm no static initialization locked the global allocator
    ASSERT(&globalAllocator == bslma::Default::globalAllocator());

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    // Confirm no static initialization locked the default allocator
    ASSERT(&defaultAllocator == bslma::Default::defaultAllocator());


    bslma::TestAllocator ta("general", veryVeryVeryVerbose);

    bsls::Types::Int64 numDeallocations;
    bsls::Types::Int64 numAllocations;
    bsls::Types::Int64 numDeletes = 0;

    bsls::Types::Int64 numDefaultDeallocations =
                                           defaultAllocator.numDeallocations();
    bsls::Types::Int64 numDefaultAllocations =
                                             defaultAllocator.numAllocations();
    switch (test) { case 0:  // Zero is always the leading case.
      case 38: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE 3: 'weak_ptr'
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.  This usage test
        //   also happens to exhaustively test the entire component and is thus
        //   the only test in the suite.
        //
        // Concerns:
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //   Test each enumeration type by assigning a variable the value
        //   of each enumeration constant and verifying that the integral value
        //   of the variable after assignment is as expected.
        //
        // Testing:
        //   USAGE EXAMPLE 3: weak_ptr
        // --------------------------------------------------------------------

         if (verbose) printf("\nTESTING USAGE EXAMPLE 3: 'weak_ptr'"
                             "\n===================================\n");
//..
// Example 3 - SEE ABOVE
// - - - - - - - - - - -
//..
        if (verbose) printf("Confirm usage example 3 builds and runs.\n");
        {
            bsl::vector<bsl::string> keywords;
            PeerCache peerCache;
            bsl::vector<SearchResult> result;
            search(&result, peerCache, keywords);
        }
      } break;
      case 37: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE 2: 'weak_ptr'
        //   We know this example demonstrates a memory leak, so put the
        //   default allocator into quiet mode for regular (non-verbose)
        //   testing, while making the (expected) leak clear for veryVerbose or
        //   higher detail levels.
        //
        // Concerns:
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //   Test each enumeration type by assigning a variable the value
        //   of each enumeration constant and verifying that the integral value
        //   of the variable after assignment is as expected.
        //
        // Testing:
        //   USAGE EXAMPLE 2: weak_ptr
        // --------------------------------------------------------------------

         if (verbose) printf("\nTESTING USAGE EXAMPLE 2: 'weak_ptr'"
                             "\n===================================\n");

         defaultAllocator.setQuiet(!veryVerbose);
//..
// Example 2 - Breaking cyclical dependencies
//- - - - - - - - - - - - - - - - - - - - - -
//..
// Note that the 'User' and 'Alert' classes could typically be used as follows:
//..
        bslma::TestAllocator ta("Example 2");
        {
            ta.setQuiet(true);

            bsl::shared_ptr<User> userPtr;
            userPtr.createInplace(&ta);

            bsl::shared_ptr<Alert> alertPtr;
            alertPtr.createInplace(&ta);

            alertPtr->addUser(userPtr);
            userPtr->addAlert(alertPtr);

            alertPtr.reset();
            userPtr.reset();

        }

        // MEMORY LEAK !!

        {

            bsl::shared_ptr<ModifiedAlert> alertPtr;
            alertPtr.createInplace(&ta);

            bsl::shared_ptr<ModifiedUser> userPtr;
            userPtr.createInplace(&ta);

            bsl::weak_ptr<ModifiedUser> userWeakPtr(userPtr);

            alertPtr->addUser(userWeakPtr);
            userPtr->addAlert(alertPtr);

            alertPtr.reset();
            userPtr.reset();
        }

        // No memory leak now
      } break;
      case 36: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 1: 'weak_ptr'
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.  This usage test
        //   also happens to exhaustively test the entire component and is thus
        //   the only test in the suite.
        //
        // Concerns:
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //   Test each enumeration type by assigning a variable the value
        //   of each enumeration constant and verifying that the integral value
        //   of the variable after assignment is as expected.
        //
        // Testing:
        //   USAGE EXAMPLE 1: weak_ptr
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE 1: 'weak_ptr'"
                            "\n===========================\n");
        {
///Example 1 - Basic usage
///- - - - - - - - - - - -
// This example illustrates the basic syntax to create and use a
// 'bsl::weak_ptr'.  Lets suppose that we want to construct a weak pointer that
// references an 'int' managed by a shared pointer.  First we define the shared
// pointer and assign a value to the shared 'int':
//..
    bsl::shared_ptr<int> intPtr;
    intPtr.createInplace(bslma::Default::allocator());
    *intPtr = 10;
    ASSERT(10 == *intPtr);
//..
// Now we construct a weak pointer to the 'int':
//..
    bsl::weak_ptr<int> intWeakPtr(intPtr);
    ASSERT(!intWeakPtr.expired());
//..
// 'bsl::weak_ptr' does not provide direct access to the shared object being
// referenced.  So to access and manipulate the 'int' from the weak pointer we
// have to get the shared pointer from it:
//..
    bsl::shared_ptr<int> intPtr2 = intWeakPtr.lock();
    ASSERT(intPtr2);
    ASSERT(10 == *intPtr2);

    *intPtr2 = 20;
    ASSERT(20 == *intPtr);
    ASSERT(20 == *intPtr2);
//..
// We can remove the weak reference to the shared 'int' by calling the 'reset'
// function:
//..
    intWeakPtr.reset();
    ASSERT(intWeakPtr.expired());
//..
// Note that resetting the weak pointer does not affect the shared pointers
// referencing the 'int' object:
//..
    ASSERT(20 == *intPtr);
    ASSERT(20 == *intPtr2);
//..
// Finally, we construct another weak pointer referencing the shared 'int':
//..
    bsl::weak_ptr<int> intWeakPtr2(intPtr);
    ASSERT(!intWeakPtr2.expired());
//..
// We now 'release' all shared references to the 'int'.  This causes the weak
// pointer to be 'expired' and any attempt to get a shared pointer from it will
// return an empty shared pointer:
//..
    intPtr.reset();
    intPtr2.reset();
    ASSERT(intWeakPtr2.expired());
    ASSERT(!intWeakPtr2.lock());
        }
    } break;
    case 35:{
        // --------------------------------------------------------------------
        // TESTING 'enable_shared_from_this' CONSTRUCTORS
        //
        // Concerns:
        //   1) Shared_ptr constructors are able to identify correctly the
        //      enable_shared_from_this (possibly indirect) base class and
        //      initalize the d_weak_this weak ptr.
        //   2) Converting from a managedPtr or auto_ptr to a shared_ptr will
        //      initalize weak_this_ weak_ptr correctly.
        //   3) Calling shared_from_this() will create a new reference to the
        //      shared_ptr.
        //   2) shared_ptr<const T> constructors are able to initialize
        //      enable_shared_from_this<T>::d_weak_this.
        //
        // Plan:
        //   Create a shared_ptrs from a class with enable_shared_from_this as
        //   the base class. From this shared pointer call share_from_this and
        //   ensure that the use_count() of the shared_pointer has incermented.
        //
        // Testing:
        //   enable_shared_from_this()
        //   enable_shared_from_this(const enable_shared_from_this&)
        //   ~enable_shared_from_this()
        //   enable_shared_from_this& operator=(const enable_shared_from_this&)
        //   shared_ptr<T> shared_from_this()
        //   shared_ptr<const T> shared_from_this() const
        // --------------------------------------------------------------------
        typedef bsl::shared_ptr<ShareThis> SharedPtr;
        typedef bsl::shared_ptr<const ShareThis> ConstSharedPtr;
        typedef bsl::shared_ptr<ShareThisDerived> SharedPtrDerived;
        typedef bsl::shared_ptr<const ShareThisDerived> ConstSharedPtrDerived;

        bslma::TestAllocator ta("enable_shared_from_this test",
                                veryVeryVeryVerbose);

        if (verbose) printf("\nTESTING 'enable_share_from_this<T>()'"
                            "\n======================================\n");

        int destructorCount = 0;

        if (verbose) printf("\nBasic usage\n");
        {
            SharedPtr ptr(new ShareThis(&destructorCount));
            ASSERT(ptr.use_count() == 1);
            ConstSharedPtr ptr_cp = ptr->shared_from_this();
            ASSERT(ptr.get() == ptr_cp.get());
            ASSERT(ptr.use_count() == 2);

            ASSERTV(destructorCount, 0 == destructorCount);
        }
        ASSERTV(destructorCount, 1 == destructorCount);
        destructorCount = 0;    // reset 'destructorCount' for next test.

        if (verbose) printf("\nBasic usage with 'const' element type\n");
        {
            ConstSharedPtr ptr(static_cast<const ShareThis*>(
                                             new ShareThis(&destructorCount)));
            ASSERT(ptr.use_count() == 1);
            ConstSharedPtr ptr_cp = ptr->shared_from_this();
            ASSERT(ptr.get() == ptr_cp.get());
            ASSERT(ptr.use_count() == 2);

            ASSERTV(destructorCount, 0 == destructorCount);
        }
        ASSERTV(destructorCount, 1 == destructorCount);
        destructorCount = 0;    // reset 'destructorCount' for next test.

        if (verbose) printf("\nBasic usage of most-derived type\n");
        {
            SharedPtrDerived ptr(new ShareThisDerived(&destructorCount));
            ASSERT(ptr.use_count() == 1);
            SharedPtr ptr_cp = ptr->shared_from_this();
            ASSERT(ptr.get() == ptr_cp.get());
            ASSERT(ptr.use_count() == 2);

            ASSERTV(destructorCount, 0 == destructorCount);
        }
        ASSERTV(destructorCount, 11 == destructorCount);
        destructorCount = 0;    // reset 'destructorCount' for next test.

        if (verbose) printf("\nBasic usage of 'const' most-derived type\n");
        {
            ConstSharedPtrDerived ptr(static_cast<const ShareThisDerived*>(
                                      new ShareThisDerived(&destructorCount)));
            ASSERT(ptr.use_count() == 1);
            ConstSharedPtr ptr_cp = ptr->shared_from_this();
            ASSERT(ptr.get() == ptr_cp.get());
            ASSERT(ptr.use_count() == 2);

            ASSERTV(destructorCount, 0 == destructorCount);
        }
        ASSERTV(destructorCount, 11 == destructorCount);
        destructorCount = 0;    // reset 'destructorCount' for next test.

        if (verbose) printf("\nTest making a copy\n");
        {
            SharedPtr ptr(new ShareThis(&destructorCount));
            SharedPtr ptr_cp(ptr);
            ASSERT(ptr.use_count() == 2);
            SharedPtr ptr_cp2 = ptr->shared_from_this();
            ASSERT(ptr.use_count() == 3);
            ASSERT(ptr.get() == ptr_cp2.get());

            ASSERTV(destructorCount, 0 == destructorCount);
        }
        ASSERTV(destructorCount, 1 == destructorCount);
        destructorCount = 0;    // reset 'destructorCount' for next test.

        if (verbose) printf("\nTest with BDE allocator\n");
        {
            SharedPtr ptr(new (ta) ShareThis(&destructorCount), &ta);
            ASSERT(ptr.use_count() == 1);
            SharedPtr ptr_cp = ptr->shared_from_this();
            ASSERT(ptr.get() == ptr_cp.get());
            ASSERT(ptr.use_count() == 2);

            ASSERTV(destructorCount, 0 == destructorCount);
        }
        ASSERTV(destructorCount, 1 == destructorCount);
        destructorCount = 0;    // reset 'destructorCount' for next test.

        MyTestDeleter d1(&ta);  // custom deleter for the next few tests

        if (verbose) printf("\nTest with BDE allocator and deleter\n");
        {
            SharedPtr ptr(new (ta) ShareThis(&destructorCount), d1, &ta);
            ASSERT(ptr.use_count() == 1);
            SharedPtr ptr_cp = ptr->shared_from_this();
            ASSERT(ptr.get() == ptr_cp.get());
            ASSERT(ptr.use_count() == 2);

            ASSERTV(destructorCount, 0 == destructorCount);
        }
        ASSERTV(destructorCount, 1 == destructorCount);
        destructorCount = 0;    // reset 'destructorCount' for next test.

        if (verbose) printf("\nTest with standard allocator and deleter\n");
        {
            bsltf::StdStatefulAllocator<TObj, false, false, false, false>
                                                                 stdalloc(&ta);
            SharedPtr ptr(new (ta) ShareThis(&destructorCount), d1, stdalloc);
            ASSERT(ptr.use_count() == 1);
            SharedPtr ptr_cp = ptr->shared_from_this();
            ASSERT(ptr.get() == ptr_cp.get());
            ASSERT(ptr.use_count() == 2);

            ASSERTV(destructorCount, 0 == destructorCount);
        }
        ASSERTV(destructorCount, 1 == destructorCount);
        destructorCount = 0;    // reset 'destructorCount' for next test.

        if (verbose) printf("\nTest with in-place buffer\n");
        {
            SharedPtr ptr = bsl::make_shared<ShareThis>(&destructorCount);
            ASSERT(ptr.use_count() == 1);
            SharedPtr ptr_cp = ptr->shared_from_this();
            ASSERT(ptr.get() == ptr_cp.get());
            ASSERT(ptr.use_count() == 2);

            ASSERTV(destructorCount, 0 == destructorCount);
        }
        ASSERTV(destructorCount, 1 == destructorCount);
        destructorCount = 0;    // reset 'destructorCount' for next test.

        if (verbose) printf("\nTest with in-place buffer and BDE allocator\n");
        {
            SharedPtr ptr = bsl::allocate_shared<ShareThis>(&ta,
                                                            &destructorCount);
            ASSERT(ptr.use_count() == 1);
            SharedPtr ptr_cp = ptr->shared_from_this();
            ASSERT(ptr.get() == ptr_cp.get());
            ASSERT(ptr.use_count() == 2);

            ASSERTV(destructorCount, 0 == destructorCount);
        }
        ASSERTV(destructorCount, 1 == destructorCount);
        destructorCount = 0;    // reset 'destructorCount' for next test.

        if (verbose) printf("\nTest with 'ManagedPtr'\n");
        {
            bslma::ManagedPtr<ShareThis> managedPtr(
                                              new ShareThis(&destructorCount));
            SharedPtr ptr(managedPtr);
            ASSERT(ptr.use_count() == 1);
            SharedPtr ptr_cp = ptr->shared_from_this();
            ASSERT(ptr.get() == ptr_cp.get());
            ASSERT(ptr.use_count() == 2);

            ASSERTV(destructorCount, 0 == destructorCount);
        }
        ASSERTV(destructorCount, 1 == destructorCount);
        destructorCount = 0;    // reset 'destructorCount' for next test.

        if (verbose) printf("\nTest with 'auto_ptr'\n");
        {
            std::auto_ptr<ShareThis> autoPtr(new ShareThis(&destructorCount));
            SharedPtr ptr(autoPtr);
            ASSERT(ptr.use_count() == 1);
            SharedPtr ptr_cp = ptr->shared_from_this();
            ASSERT(ptr.get() == ptr_cp.get());
            ASSERT(ptr.use_count() == 2);

            ASSERTV(destructorCount, 0 == destructorCount);
        }
        ASSERTV(destructorCount, 1 == destructorCount);
        destructorCount = 0;    // reset 'destructorCount' for next test.

        if (verbose) printf("\nTest with aliased 'shared_ptr'\n");
        {
            ShareThis *data_p = new ShareThis(&destructorCount);
            bsl::shared_ptr<void> voidPtr(data_p);
            SharedPtr ptr(voidPtr, data_p);
            ASSERT(ptr.use_count() == 2);
            SharedPtr ptr_cp = ptr->shared_from_this();
            ASSERT(ptr.get() == ptr_cp.get());
            ASSERT(ptr.use_count() == 3);

            ASSERTV(destructorCount, 0 == destructorCount);
        }
        ASSERTV(destructorCount, 1 == destructorCount);
        destructorCount = 0;    // reset 'destructorCount' for next test.

        if (verbose) printf("\nTest extended lifetimes\n");
        {
            struct LocalFactory {
                static SharedPtr make(int *destructorCount_p) {
                    ShareThis *data_p = new ShareThis(destructorCount_p);
                    bsl::shared_ptr<void> voidPtr(data_p);
                    SharedPtr ptr(voidPtr, data_p);
                    ASSERT(ptr.use_count() == 2);
                    return ptr->shared_from_this();
                }
            };

            bsl::weak_ptr<ShareThis> weak;
            {
                SharedPtr ptr = LocalFactory::make(&destructorCount);
                ASSERTV(ptr.use_count(), ptr.use_count() == 1);
                weak = ptr;

                ASSERTV(destructorCount, 0 == destructorCount);
                SharedPtr ptr_cp = weak.lock();
                ASSERT(ptr.get() == ptr_cp.get());
                ASSERTV(ptr_cp.use_count(), ptr_cp.use_count() == 2);
            }

            ASSERTV(destructorCount, 1 == destructorCount);
            SharedPtr failPtr = weak.lock();
            ASSERT(!failPtr);
        }
        ASSERTV(destructorCount, 1 == destructorCount);
        destructorCount = 0;    // reset 'destructorCount' for next test.

      } break;
      case 34: {
        // --------------------------------------------------------------------
        // TESTING 'allocate_shared<T>(A *, ...)'
        //
        // Concerns:
        //   All constructor is able to initialize the object correctly.
        //
        // Plan:
        //   Call all 14 different constructors and supply it with the
        //   appropriate arguments.  Then verify that the object created inside
        //   the representation is initialized using the arguments supplied.
        //
        // Testing:
        //   shared_ptr<T> allocate_shared<T, ALLOC>(ALLOC *, ARGS&&...)
        //   shared_ptr<T> allocate_shared<T, ALLOC>(ALLOC *,const ARGS&...)
        //   shared_ptr<T> allocate_shared<T, A>(A *)
        //   shared_ptr<T> allocate_shared<T, A>(A *, const A1& a1)
        //   shared_ptr<T> allocate_shared<T, A>(A *, const A1& a1, ...& a2)
        //   shared_ptr<T> allocate_shared<T, A>(A *, const A1& a1, ...& a3)
        //   shared_ptr<T> allocate_shared<T, A>(A *, const A1& a1, ...& a4)
        //   shared_ptr<T> allocate_shared<T, A>(A *, const A1& a1, ...& a5)
        //   shared_ptr<T> allocate_shared<T, A>(A *, const A1& a1, ...& a6)
        //   shared_ptr<T> allocate_shared<T, A>(A *, const A1& a1, ...& a7)
        //   shared_ptr<T> allocate_shared<T, A>(A *, const A1& a1, ...& a8)
        //   shared_ptr<T> allocate_shared<T, A>(A *, const A1& a1, ...& a9)
        //   shared_ptr<T> allocate_shared<T, A>(A *, const A1& a1, ...& a10)
        //   shared_ptr<T> allocate_shared<T, A>(A *, const A1& a1, ...& a11)
        //   shared_ptr<T> allocate_shared<T, A>(A *, const A1& a1, ...& a12)
        //   shared_ptr<T> allocate_shared<T, A>(A *, const A1& a1, ...& a13)
        //   shared_ptr<T> allocate_shared<T, A>(A *, const A1& a1, ...& a14)
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING 'allocate_shared<T>(A *, ...)'"
                            "\n======================================\n");

        static const MyTestArg1  V1(1);
        static const MyTestArg2  V2(20);
        static const MyTestArg3  V3(23);
        static const MyTestArg4  V4(44);
        static const MyTestArg5  V5(66);
        static const MyTestArg6  V6(176);
        static const MyTestArg7  V7(878);
        static const MyTestArg8  V8(8);
        static const MyTestArg9  V9(912);
        static const MyTestArg10 V10(102);
        static const MyTestArg11 V11(111);
        static const MyTestArg12 V12(333);
        static const MyTestArg13 V13(712);
        static const MyTestArg14 V14(1414);

        bslma::TestAllocator ta("allocate_shared", veryVeryVeryVerbose);

        if (verbose) printf("\nTesting constructor with no arguments"
                            "\n-------------------------------------\n");

        bsls::Types::Int64 numAllocations = ta.numAllocations();
        bsls::Types::Int64 numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP = MyInplaceTestObject();

            {
                TCObj x = bsl::allocate_shared<MyInplaceTestObject>(&ta);
                ASSERT(++numAllocations == ta.numAllocations());
                ASSERT(EXP == *x);
            }
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

        if (verbose) printf("\nTesting constructor with 1 argument"
                            "\n-----------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1);

            {
                TCObj x = bsl::allocate_shared<MyInplaceTestObject>(&ta, V1);
                ASSERT(++numAllocations == ta.numAllocations());
                ASSERT(EXP == *x);
            }
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

        if (verbose) printf("\nTesting contructor with 2 arguments"
                            "\n-----------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2);

            {
                TCObj x = bsl::allocate_shared<MyInplaceTestObject>(&ta,
                                                                     V1, V2);
                ASSERT(++numAllocations == ta.numAllocations());
                ASSERT(EXP == *x);
            }
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

        if (verbose) printf("\nTesting constructor with 3 arguments"
                            "\n------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3);

            {
                TCObj x = bsl::allocate_shared<MyInplaceTestObject>(&ta,
                                                                   V1, V2, V3);
                ASSERT(++numAllocations == ta.numAllocations());
                ASSERT(EXP == *x);
            }
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

        if (verbose) printf("\nTesting constructor with 4 arguments"
                            "\n------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4);

            {
                TCObj x = bsl::allocate_shared<MyInplaceTestObject>(&ta,
                                                               V1, V2, V3, V4);
                ASSERT(++numAllocations == ta.numAllocations());
                ASSERT(EXP == *x);
            }
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

        if (verbose) printf("\nTesting constructor with 5 arguments"
                            "\n------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5);

            {
                TCObj x = bsl::allocate_shared<MyInplaceTestObject>(&ta,
                                                           V1, V2, V3, V4, V5);
                ASSERT(++numAllocations == ta.numAllocations());
                ASSERT(EXP == *x);
            }
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

        if (verbose) printf("\nTesting constructor with 6 arguments"
                            "\n------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6);

            {
                TCObj x = bsl::allocate_shared<MyInplaceTestObject>(&ta,
                                                       V1, V2, V3, V4, V5, V6);
                ASSERT(++numAllocations == ta.numAllocations());
                ASSERT(EXP == *x);
            }
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

        if (verbose) printf("\nTesting constructor with 7 arguments"
                            "\n------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7);

            {
                TCObj x = bsl::allocate_shared<MyInplaceTestObject>(&ta,
                                                   V1, V2, V3, V4, V5, V6, V7);
                ASSERT(++numAllocations == ta.numAllocations());
                ASSERT(EXP == *x);
            }
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

        if (verbose) printf("\nTesting constructor with 8 arguments"
                            "\n------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                                                                           V8);

            {
                TCObj x = bsl::allocate_shared<MyInplaceTestObject>(&ta,
                                               V1, V2, V3, V4, V5, V6, V7, V8);
                ASSERT(++numAllocations == ta.numAllocations());
                ASSERT(EXP == *x);
            }
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

        if (verbose) printf("\nTesting constructor with 9 arguments"
                            "\n------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                                                                       V8, V9);

            {
                TCObj x = bsl::allocate_shared<MyInplaceTestObject>(&ta,
                                           V1, V2, V3, V4, V5, V6, V7, V8, V9);
                ASSERT(++numAllocations == ta.numAllocations());
                ASSERT(EXP == *x);
            }
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

        if (verbose) printf("\nTesting constructor with 10 arguments"
                            "\n-------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                                                                  V8, V9, V10);

            {
                TCObj x = bsl::allocate_shared<MyInplaceTestObject>(&ta,
                                      V1, V2, V3, V4, V5, V6, V7, V8, V9, V10);
                ASSERT(++numAllocations == ta.numAllocations());
                ASSERT(EXP == *x);
            }
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

        if (verbose) printf("\nTesting constructor with 11 arguments"
                            "\n-------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                                                             V8, V9, V10, V11);

            {
                TCObj x = bsl::allocate_shared<MyInplaceTestObject>(&ta,
                                 V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11);
                ASSERT(++numAllocations == ta.numAllocations());
                ASSERT(EXP == *x);
            }
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

        if (verbose) printf("\nTesting constructor with 12 arguments"
                            "\n-------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                                                        V8, V9, V10, V11, V12);

            {
                TCObj x = bsl::allocate_shared<MyInplaceTestObject>(&ta,
                            V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12);
                ASSERT(++numAllocations == ta.numAllocations());
                ASSERT(EXP == *x);
            }
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

        if (verbose) printf("\nTesting constructor with 13 arguments"
                            "\n-------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                                                   V8, V9, V10, V11, V12, V13);

            {
                TCObj x = bsl::allocate_shared<MyInplaceTestObject>(
                  &ta, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12, V13);
                ASSERT(++numAllocations == ta.numAllocations());
                ASSERT(EXP == *x);
            }
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

        if (verbose) printf("\nTesting constructor with 14 arguments"
                            "\n-------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                                              V8, V9, V10, V11, V12, V13, V14);

            {
                TCObj x = bsl::allocate_shared<MyInplaceTestObject>(&ta,
                  V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12, V13, V14);
                ASSERT(++numAllocations == ta.numAllocations());
                ASSERT(EXP == *x);
            }
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

#if defined(BDE_BUILD_TARGET_EXC)
        // Test for no leaks when allocated object's constructor throws..
        int constructCount = 0;
        int destroyCount = 0;
        try {
            bsl::allocate_shared<MyInstrumentedObject>(&ta,
                                                       &constructCount,
                                                       &destroyCount,
                                                        true);
            ASSERT(!"The previous expression should throw");
        }
        catch (const ConstructorFailed&) {
        }

        ASSERTV(constructCount, destroyCount, constructCount == destroyCount);
#endif
      } break;
      case 33: {
        // --------------------------------------------------------------------
        // TESTING 'allocate_shared'
        //
        // Concerns:
        //
        // Plan: TBD
        //
        // Testing:
        //   shared_ptr<T> allocate_shared<T, ALLOC>(ALLOC, ARGS&&...)
        //   shared_ptr<T> allocate_shared<T, ALLOC>(ALLOC, const ARGS&...)
        //   shared_ptr<T> allocate_shared<T, A>(A)
        //   shared_ptr<T> allocate_shared<T, A>(A, const A1& a1)
        //   shared_ptr<T> allocate_shared<T, A>(A, const A1& a1, ...& a2)
        //   shared_ptr<T> allocate_shared<T, A>(A, const A1& a1, ...& a3)
        //   shared_ptr<T> allocate_shared<T, A>(A, const A1& a1, ...& a4)
        //   shared_ptr<T> allocate_shared<T, A>(A, const A1& a1, ...& a5)
        //   shared_ptr<T> allocate_shared<T, A>(A, const A1& a1, ...& a6)
        //   shared_ptr<T> allocate_shared<T, A>(A, const A1& a1, ...& a7)
        //   shared_ptr<T> allocate_shared<T, A>(A, const A1& a1, ...& a8)
        //   shared_ptr<T> allocate_shared<T, A>(A, const A1& a1, ...& a9)
        //   shared_ptr<T> allocate_shared<T, A>(A, const A1& a1, ...& a10)
        //   shared_ptr<T> allocate_shared<T, A>(A, const A1& a1, ...& a11)
        //   shared_ptr<T> allocate_shared<T, A>(A, const A1& a1, ...& a12)
        //   shared_ptr<T> allocate_shared<T, A>(A, const A1& a1, ...& a13)
        //   shared_ptr<T> allocate_shared<T, A>(A, const A1& a1, ...& a14)
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING 'allocate_shared'"
                            "\n=========================\n");

        using BloombergLP::bsltf::StdStatefulAllocator;

        typedef bsl::allocator<int> ALLOC_1;
        typedef StdStatefulAllocator<int, true, true, true, true> ALLOC_2;
        typedef StdStatefulAllocator<int, false, false, false, false> ALLOC_3;

        typedef TestHarness<ALLOC_1> T1;
        typedef TestHarness<ALLOC_2> T2;
        typedef TestHarness<ALLOC_3> T3;

        T1::testCase33(verbose,
                       veryVerbose,
                       veryVeryVerbose,
                       veryVeryVeryVerbose);
        T2::testCase33(verbose,
                       veryVerbose,
                       veryVeryVerbose,
                       veryVeryVeryVerbose);
        T3::testCase33(verbose,
                       veryVerbose,
                       veryVeryVerbose,
                       veryVeryVeryVerbose);

#if defined(BDE_BUILD_TARGET_EXC)
        // Test for no leaks when allocated object's constructor throws..
        int constructCount = 0;
        int destroyCount = 0;
        try {
            bslma::TestAllocator ta("double-destruct test allocator");
            StdStatefulAllocator<MyInstrumentedObject, true, true, true, true>
                                                                    alloc(&ta);
            bsl::allocate_shared<MyInstrumentedObject>(alloc,
                                                      &constructCount,
                                                      &destroyCount,
                                                       true);
            ASSERT(!"The previous expression should throw");
        }
        catch (const ConstructorFailed&) {
        }

        ASSERTV(constructCount, destroyCount, constructCount == destroyCount);
#endif
      } break;
      case 32: {
        // --------------------------------------------------------------------
        // TESTING 'make_shared'
        //
        // Concerns:
        //
        // Plan: TBD
        //
        // Testing:
        //   shared_ptr<T> make_shared<T>(ARGS&&...args)
        //   shared_ptr<T> make_shared<T>(const ARGS&...args)
        //   shared_ptr<T> make_shared<T>()
        //   shared_ptr<T> make_shared<T>(const A1& a1)
        //   shared_ptr<T> make_shared<T>(const A1& a1, ...& a2)
        //   shared_ptr<T> make_shared<T>(const A1& a1, ...& a3)
        //   shared_ptr<T> make_shared<T>(const A1& a1, ...& a4)
        //   shared_ptr<T> make_shared<T>(const A1& a1, ...& a5)
        //   shared_ptr<T> make_shared<T>(const A1& a1, ...& a6)
        //   shared_ptr<T> make_shared<T>(const A1& a1, ...& a7)
        //   shared_ptr<T> make_shared<T>(const A1& a1, ...& a8)
        //   shared_ptr<T> make_shared<T>(const A1& a1, ...& a9)
        //   shared_ptr<T> make_shared<T>(const A1& a1, ...& a10)
        //   shared_ptr<T> make_shared<T>(const A1& a1, ...& a11)
        //   shared_ptr<T> make_shared<T>(const A1& a1, ...& a12)
        //   shared_ptr<T> make_shared<T>(const A1& a1, ...& a13)
        //   shared_ptr<T> make_shared<T>(const A1& a1, ...& a14)
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING 'make_shared'"
                            "\n=====================\n");

        static const MyTestArg1  V1(1);
        static const MyTestArg2  V2(20);
        static const MyTestArg3  V3(23);
        static const MyTestArg4  V4(44);
        static const MyTestArg5  V5(66);
        static const MyTestArg6  V6(176);
        static const MyTestArg7  V7(878);
        static const MyTestArg8  V8(8);
        static const MyTestArg9  V9(912);
        static const MyTestArg10 V10(102);
        static const MyTestArg11 V11(111);
        static const MyTestArg12 V12(333);
        static const MyTestArg13 V13(712);
        static const MyTestArg14 V14(1414);

        if (verbose) printf("\nTesting 'make_shared' with no arguments"
                            "\n---------------------------------------\n");

        bslma::TestAllocator ta(veryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&ta);

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP = MyInplaceTestObject();

            bsl::shared_ptr<MyInplaceTestObject> x =
                                       bsl::make_shared<MyInplaceTestObject>();
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.get()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'make_shared' with 1 argument"
                            "\n---------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1);

            bsl::shared_ptr<MyInplaceTestObject> x =
                                     bsl::make_shared<MyInplaceTestObject>(V1);
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.get()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'make_shared' with 2 arguments"
                            "\n----------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2);

            bsl::shared_ptr<MyInplaceTestObject> x =
                                 bsl::make_shared<MyInplaceTestObject>(V1, V2);
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.get()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'make_shared' with 3 arguments"
                            "\n----------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3);

            bsl::shared_ptr<MyInplaceTestObject> x =
                             bsl::make_shared<MyInplaceTestObject>(V1, V2, V3);
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.get()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'make_shared' with 4 arguments"
                            "\n----------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4);

            bsl::shared_ptr<MyInplaceTestObject> x =
                         bsl::make_shared<MyInplaceTestObject>(V1, V2, V3, V4);
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.get()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'make_shared' with 5 arguments"
                            "\n----------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5);

            bsl::shared_ptr<MyInplaceTestObject> x =
                     bsl::make_shared<MyInplaceTestObject>(V1, V2, V3, V4, V5);
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.get()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'make_shared' with 6 arguments"
                            "\n----------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6);

            bsl::shared_ptr<MyInplaceTestObject> x =
                 bsl::make_shared<MyInplaceTestObject>(V1, V2, V3, V4, V5, V6);
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.get()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'make_shared' with 7 arguments"
                            "\n----------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7);

            bsl::shared_ptr<MyInplaceTestObject> x =
                  bsl::make_shared<MyInplaceTestObject>(V1, V2, V3, V4, V5, V6,
                                                        V7);
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.get()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'make_shared' with 9 arguments"
                            "\n----------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                                                 V8);

            bsl::shared_ptr<MyInplaceTestObject> x =
                  bsl::make_shared<MyInplaceTestObject>(V1, V2, V3, V4, V5, V6,
                                                        V7, V8);
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.get()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'make_shared' with 10 arguments"
                            "\n-----------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                                                 V8, V9);

            bsl::shared_ptr<MyInplaceTestObject> x =
                  bsl::make_shared<MyInplaceTestObject>(V1, V2, V3, V4, V5, V6,
                                                        V7, V8, V9);
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.get()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'make_shared' with 2 arguments"
                            "\n----------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                                                 V8, V9, V10);

            bsl::shared_ptr<MyInplaceTestObject> x =
                  bsl::make_shared<MyInplaceTestObject>(V1, V2, V3, V4, V5, V6,
                                                        V7, V8, V9, V10);
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.get()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'make_shared' with 11 arguments"
                            "\n----------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                                                 V8, V9, V10, V11);

            bsl::shared_ptr<MyInplaceTestObject> x =
                  bsl::make_shared<MyInplaceTestObject>(V1, V2, V3, V4, V5, V6,
                                                        V7, V8, V9, V10, V11);
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.get()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'make_shared' with 12 arguments"
                            "\n-----------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                                                 V8, V9, V10, V11, V12);

            bsl::shared_ptr<MyInplaceTestObject> x =
                  bsl::make_shared<MyInplaceTestObject>(V1, V2, V3, V4, V5, V6,
                                                        V7, V8, V9, V10, V11,
                                                        V12);
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.get()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'make_shared' with 13 arguments"
                            "\n-----------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                                                 V8, V9, V10, V11, V12, V13);

            bsl::shared_ptr<MyInplaceTestObject> x =
                  bsl::make_shared<MyInplaceTestObject>(V1, V2, V3, V4, V5, V6,
                                                        V7, V8, V9, V10, V11,
                                                        V12, V13);
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.get()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'make_shared' with 14 arguments"
                            "\n-----------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                    V8, V9, V10, V11, V12, V13, V14);

            bsl::shared_ptr<MyInplaceTestObject> x =
                  bsl::make_shared<MyInplaceTestObject>(V1, V2, V3, V4, V5, V6,
                                                        V7, V8, V9, V10, V11,
                                                        V12, V13, V14);
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.get()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'make_shared' with evil type"
                            "\n------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bsl::shared_ptr<MostEvilTestType> x =
                                        bsl::make_shared<MostEvilTestType>(13);
            const bsl::shared_ptr<MostEvilTestType>& X=x;

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(13 == X.get()->data());
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

#if defined(BDE_BUILD_TARGET_EXC)
        // Test for no leaks when allocated object's constructor throws..
        int constructCount = 0;
        int destroyCount = 0;
        try {
            bsl::make_shared<MyInstrumentedObject>(&constructCount,
                                                   &destroyCount,
                                                    true);
            ASSERT(!"The previous expression should throw");
        }
        catch (const ConstructorFailed&) {
        }

        ASSERTV(constructCount, destroyCount, constructCount == destroyCount);
#endif
    } break;
    case 31: {
      // --------------------------------------------------------------------
      // TESTING 'hash' FUNCTOR ('shared_ptr')
      //
      // Concerns:
      //   Test that the 'hash' specialization works as expected.
      //   'hash' is a CopyConstructible POD
      //   'operator()' produces distinct hash values for distinct inputs
      //   'operator()' produces the same result for shared pointers aliasing
      //      the same object, regardless of ownership
      //   'operator()' supports empty shared pointers
      //   'operator()' is const-qualified and can be called with const objects
      //   'operator()' does not modify its argument
      //
      // Plan:
      //
      // Testing:
      //   size_t operator()(const shared_ptr<TYPE>& ptr) const
      // --------------------------------------------------------------------

      if (verbose) printf("\nTESTING 'hash' FUNCTOR ('shared_ptr')"
                          "\n=====================================\n");

      {
          bslma::TestAllocator ta;
          MyTestObject *OBJ_PTR_1 = new(ta) MyTestObject(&numDeletes);
          const Obj obj1(OBJ_PTR_1, &ta);

          MyTestObject *OBJ_PTR_2 = new(ta) MyTestObject(&numDeletes);
          const Obj obj2(OBJ_PTR_2, &ta);

          // wacky aliased hybrid relies on nested object lifetime for validity
          const Obj obj3(obj2, obj1.get());

          {
              const bsl::hash<Obj> hashX = bsl::hash<Obj>();
              bsl::hash<Obj> hashY = hashX;
              (void) hashY;  // Suppress 'unused variable' warning

              Obj x;
              const size_t hashValueNull = bsl::hash<Obj>()(x);
              ASSERT(!x);

              const size_t hashValue_1   = bsl::hash<Obj>()(obj1);
              const size_t hashValue_2   = bsl::hash<Obj>()(obj2);

              ASSERTV(hashValueNull,   hashValue_1,
                      hashValueNull != hashValue_1);

              ASSERTV(hashValueNull,   hashValue_2,
                      hashValueNull != hashValue_2);

              ASSERTV(hashValue_1, hashValue_2, hashValue_1 != hashValue_2);

              const size_t hashValue_3 = bsl::hash<Obj>()(obj3);
              ASSERTV(hashValue_1, hashValue_3, hashValue_1 == hashValue_3);

              x = obj1;
              const Obj X = x;
              const size_t hashValue_4 = bsl::hash<Obj>()(x);
              ASSERTV(hashValue_1, hashValue_4, hashValue_1 == hashValue_4);
              ASSERT(X == x);
          }
      }
    } break;
    case 30: {
      // --------------------------------------------------------------------
      // TESTING NOTHING, THIS SLOT IS AVAILABLE TO BE RE-USED
      //
      // Concerns:
      //
      // Plan:
      //
      // Testing:
      // --------------------------------------------------------------------

      if (verbose)
           printf("\nTESTING NOTHING, THIS SLOT IS AVAILABLE TO BE RE-USED"
                  "\n=====================================================\n");

    } break;
    case 29: {
      // --------------------------------------------------------------------
      // TESTING 'owner_before' METHODS
      //
      // Concerns:
      //   Test that the 'owner_before' function works as expected.
      //
      // Plan:
      //
      // Testing:
      //   bool shared_ptr::owner_before(const shared_ptr<OTHER_TYPE>& rhs)
      //   bool shared_ptr::owner_before(const weak_ptr<OTHER_TYPE>& rhs)
      //   bool weak_ptr::owner_before(const shared_ptr<OTHER_TYPE>& rhs)
      //   bool weak_ptr::owner_before(const weak_ptr<OTHER_TYPE>& rhs)
      // --------------------------------------------------------------------

      if (verbose) printf("\nTESTING 'owner_before' METHODS"
                          "\n==============================\n");

      {
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

              ObjSP mSA(PTR1, &rep1); const ObjSP& SA = mSA;
              ObjSP mSB(PTR2, &rep2); const ObjSP& SB = mSB;

              ObjWP mWA(SA); const ObjWP& WA = mWA;
              ObjWP mWB(SB); const ObjWP& WB = mWB;

              ASSERT(false == EWP1.owner_before(EWP1));
              ASSERT(false == EWP1.owner_before(EWP2));
              ASSERT(false == EWP1.owner_before(ESP));
              ASSERT(true  == EWP1.owner_before(SA));
              ASSERT(true  == EWP1.owner_before(WA));
              ASSERT(true  == EWP1.owner_before(SB));
              ASSERT(true  == EWP1.owner_before(WB));

              ASSERT(false == WA.owner_before(EWP1));
              ASSERT(false == WA.owner_before(ESP));
              ASSERT(false == WA.owner_before(SA));
              ASSERT(false == WA.owner_before(WA));
              ASSERT(WA.owner_before(SB) == (&REP1 < &REP2));
              ASSERT(WA.owner_before(WB) == (&REP1 < &REP2));

              ASSERT(false == SA.owner_before(EWP1));
              ASSERT(false == SA.owner_before(ESP));
              ASSERT(false == SA.owner_before(SA));
              ASSERT(false == SA.owner_before(WA));
              ASSERT(SA.owner_before(SB) == (&REP1 < &REP2));
              ASSERT(SA.owner_before(WB) == (&REP1 < &REP2));
          }
      }
    } break;
    case 28: {
      // --------------------------------------------------------------------
      // TESTING 'swap' ('weak_ptr')
      //
      // Concerns:
      //   Test that the 'swap' function works as expected.
      //
      // Plan:
      //
      // Testing:
      //   void swap(weak_ptr& src)
      // --------------------------------------------------------------------

      if (verbose) printf("\nTESTING 'swap' ('weak_ptr')"
                          "\n===========================\n");

      {
          bslma::TestAllocator ta;
          MyTestObject *REP_PTR = new(ta) MyTestObject(&numDeletes);
          TestSharedPtrRep<MyTestObject> rep(REP_PTR, &ta);
          const TestSharedPtrRep<MyTestObject>& REP = rep;
          MyTestObject *PTR = REP.ptr();
          ASSERTV(REP_PTR, PTR,          REP_PTR == PTR);
          {
              ObjWP mX; const ObjWP& X = mX;
              ObjWP mY; const ObjWP& Y = mY;

              ASSERT(X.expired());
              ASSERT(0 == X.rep());

              ASSERT(Y.expired());
              ASSERT(0 == Y.rep());

              mX.swap(mY);

              ASSERT(X.expired());
              ASSERT(0 == X.rep());

              ASSERT(Y.expired());
              ASSERT(0 == Y.rep());

              ObjSP mS(PTR, &rep);
              const ObjSP& S = mS;

              mX = S;

              ASSERT(!X.expired());
              ASSERT(&REP == X.rep());
              ASSERT(1 == REP.numReferences());
              ASSERT(1 == REP.numWeakReferences());

              mX.swap(mY);
              ASSERT(X.expired());
              ASSERT(!Y.expired());
              ASSERT(0    == X.rep());
              ASSERT(&REP == Y.rep());
              ASSERT(1 == REP.numReferences());
          }
          LOOP_ASSERT(REP.numReferences(),     0 == REP.numReferences());
          LOOP_ASSERT(REP.disposeRepCount(),
                      1 == REP.disposeRepCount());
          LOOP_ASSERT(REP.disposeObjectCount(),
                      1 == REP.disposeObjectCount());
      }

      {
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
              ObjSP mS1(PTR1, &rep1);
              const ObjSP& S1 = mS1;

              ObjSP mS2(PTR2, &rep2);
              const ObjSP& S2 = mS2;

              ObjWP mX(S1); const ObjWP& X = mX;
              ObjWP mY(S2); const ObjWP& Y = mY;

              ASSERT(&REP1  == X.rep());
              ASSERT(&REP2  == Y.rep());

              LOOP_ASSERT(REP1.numReferences(), 1 == REP1.numReferences());
              LOOP_ASSERT(REP2.numReferences(), 1 == REP2.numReferences());

              mX.swap(mY);

              ASSERT(&REP2  == X.rep());
              ASSERT(&REP1  == Y.rep());

              LOOP_ASSERT(REP1.numReferences(), 1 == REP1.numReferences());
              LOOP_ASSERT(REP2.numReferences(), 1 == REP2.numReferences());
          }

          LOOP_ASSERT(REP1.numReferences(), 0 == REP1.numReferences());
          LOOP_ASSERT(REP1.disposeRepCount(), 1 == REP1.disposeRepCount());
          LOOP_ASSERT(REP1.disposeObjectCount(),
                                             1 == REP1.disposeObjectCount());

          LOOP_ASSERT(REP2.numReferences(), 0 == REP2.numReferences());
          LOOP_ASSERT(REP2.disposeRepCount(), 1 == REP2.disposeRepCount());
          LOOP_ASSERT(REP2.disposeObjectCount(),
                                             1 == REP2.disposeObjectCount());
      }
    } break;
    case 27: {
      // --------------------------------------------------------------------
      // TESTING 'lock'
      //
      // Concerns:
      //   Test that 'lock' works as expected.
      //
      // Plan:
      //
      // Testing:
      //   shared_ptr<TYPE> lock() const
      // --------------------------------------------------------------------

      if (verbose) printf("\nTESTING 'lock'"
                          "\n==============\n");

      {
          ObjWP mX; const ObjWP& X = mX;
          ObjWP mY; const ObjWP& Y = mY;

          ASSERT(X.expired());
          ASSERT(0 == X.rep());

          ASSERT(Y.expired());
          ASSERT(0 == Y.rep());

          ObjSP S1 = Y.lock();
          ASSERT(Y.expired());
          ASSERT(0 == Y.rep());
          ASSERT(!S1);
          ASSERT(0 == S1.get());
          ASSERT(0 == S1.rep());

      }

      {
          bslma::TestAllocator ta;
          MyTestObject *REP_PTR = new(ta) MyTestObject(&numDeletes);
          TestSharedPtrRep<MyTestObject> rep(REP_PTR, &ta);
          const TestSharedPtrRep<MyTestObject>& REP = rep;
          MyTestObject *PTR = REP.ptr();
          ASSERTV(REP_PTR, PTR, REP_PTR == PTR);

          ObjSP SC;
          {
              ObjSP mS(PTR, &rep);
              const ObjSP& S = mS;

              ObjWP mX(S); const ObjWP& X = mX;
              ObjWP mY(S); const ObjWP& Y = mY;
              LOOP_ASSERT(REP.numReferences(), 1 == REP.numReferences());

              ObjSP SA = X.lock();
              ASSERT(SA);
              ASSERT(PTR  == SA.get());
              ASSERT(&REP == SA.rep());
              LOOP_ASSERT(REP.numReferences(), 2 == REP.numReferences());

              ObjSP SB = Y.lock();
              ASSERT(SB);
              ASSERT(PTR  == SB.get());
              ASSERT(&REP == SB.rep());

              LOOP_ASSERT(REP.numReferences(), 3 == REP.numReferences());

              SC = X.lock();
              LOOP_ASSERT(REP.numReferences(), 4 == REP.numReferences());
              ASSERT(PTR  == SC.get());
              ASSERT(&REP == SC.rep());
          }

          LOOP_ASSERT(REP.numReferences(),     1 == REP.numReferences());
          LOOP_ASSERT(REP.disposeRepCount(),   0 == REP.disposeRepCount());
          LOOP_ASSERT(REP.disposeObjectCount(),
                      0 == REP.disposeObjectCount());
      }
    } break;
    case 26: {
      // --------------------------------------------------------------------
      // TESTING 'reset' ('weak_ptr')
      //
      // Concerns:
      //   Test that the 'reset' works as expected.
      //
      // Plan:
      //
      // Testing:
      //   void reset()
      // --------------------------------------------------------------------

      if (verbose) printf("\nTESTING 'reset' ('weak_ptr')"
                          "\n============================\n");

      {
          ObjWP mX; const ObjWP& X = mX;
          ObjWP mY; const ObjWP& Y = mY;

          ASSERT(X.expired());
          ASSERT(0 == X.rep());

          ASSERT(Y.expired());
          ASSERT(0 == Y.rep());

          mY.reset();
          ASSERT(Y.expired());
          ASSERT(0 == Y.rep());
      }

      {
          bslma::TestAllocator ta;
          MyTestObject *REP_PTR = new(ta) MyTestObject(&numDeletes);
          TestSharedPtrRep<MyTestObject> rep(REP_PTR, &ta);
          const TestSharedPtrRep<MyTestObject>& REP = rep;
          LOOP_ASSERT(REP.numReferences(),     1 == REP.numReferences());
          LOOP_ASSERT(REP.disposeRepCount(),   0 == REP.disposeRepCount());
          LOOP_ASSERT(REP.disposeObjectCount(),
                      0 == REP.disposeObjectCount());
          MyTestObject *PTR = REP.ptr();
          ASSERTV(REP_PTR, PTR,                REP_PTR == PTR);
          {
              ObjWP mZ; const ObjWP& Z = mZ;
              ASSERT(0 == Z.rep());
              {
                  ObjSP mS(PTR, &rep); const ObjSP& S = mS;
                  {
                      ObjWP mX(S); const ObjWP& X = mX;
                      ObjWP mY(S); const ObjWP& Y = mY;
                      LOOP_ASSERT(REP.numReferences(),
                                                   1 == REP.numReferences());
                      LOOP_ASSERT(REP.disposeRepCount(),
                                                 0 == REP.disposeRepCount());
                      LOOP_ASSERT(REP.disposeObjectCount(),
                                              0 == REP.disposeObjectCount());
                      ASSERT(&REP == X.rep());
                      ASSERT(&REP == Y.rep());

                      mX.reset();
                      LOOP_ASSERT(REP.numReferences(),
                                                   1 == REP.numReferences());
                      LOOP_ASSERT(REP.disposeRepCount(),
                                                 0 == REP.disposeRepCount());
                      LOOP_ASSERT(REP.disposeObjectCount(),
                                              0 == REP.disposeObjectCount());
                      ASSERT(0    == X.rep());
                      ASSERT(&REP == Y.rep());

                      mY.reset();
                      LOOP_ASSERT(REP.numReferences(),
                                                   1 == REP.numReferences());
                      LOOP_ASSERT(REP.disposeRepCount(),
                                                 0 == REP.disposeRepCount());
                      LOOP_ASSERT(REP.disposeObjectCount(),
                                              0 == REP.disposeObjectCount());
                      ASSERT(0 == X.rep());
                      ASSERT(0 == Y.rep());
                  }

                  LOOP_ASSERT(REP.numReferences(), 1 == REP.numReferences());
                  LOOP_ASSERT(REP.disposeRepCount(),
                              0 == REP.disposeRepCount());
                  LOOP_ASSERT(REP.disposeObjectCount(),
                                              0 == REP.disposeObjectCount());

                  mZ = S;
                  LOOP_ASSERT(REP.numReferences(), 1 == REP.numReferences());
                  LOOP_ASSERT(REP.disposeRepCount(),
                              0 == REP.disposeRepCount());
                  LOOP_ASSERT(REP.disposeObjectCount(),
                                              0 == REP.disposeObjectCount());
                  ASSERT(&REP == Z.rep());
              }

              LOOP_ASSERT(REP.numReferences(), 0 == REP.numReferences());
              LOOP_ASSERT(REP.disposeRepCount(), 0 == REP.disposeRepCount());
              LOOP_ASSERT(REP.disposeObjectCount(),
                                              1 == REP.disposeObjectCount());

              mZ.reset();
              LOOP_ASSERT(REP.numReferences(), 0 == REP.numReferences());
              LOOP_ASSERT(REP.disposeRepCount(), 1 == REP.disposeRepCount());
              LOOP_ASSERT(REP.disposeObjectCount(),
                                              1 == REP.disposeObjectCount());
              ASSERT(0 == Z.rep());
          }

          LOOP_ASSERT(REP.numReferences(), 0 == REP.numReferences());
          LOOP_ASSERT(REP.disposeRepCount(),  1 == REP.disposeRepCount());
          LOOP_ASSERT(REP.disposeObjectCount(),
                      1 == REP.disposeObjectCount());
      }
    } break;
    case 25: {
      // --------------------------------------------------------------------
      // TESTING ASSIGNMENT OPERATORS ('weak_ptr')
      //
      // Concerns:
      //   Test that each of the assignment operators work as expected.
      //   Confirm that each works correctly with same and different types as
      //   target for the shared/weak_ptr - despite there not being a specific
      //   signature to check for 'shared_ptr<SAME_TYPE>'.
      //
      // Plan:
      //
      // Testing:
      //   weak_ptr& operator=(const weak_ptr& rhs)
      //   weak_ptr& operator=(const shared_ptr<COMPATIBLE_TYPE>& rhs)
      //   weak_ptr& operator=(const weak_ptr<COMPATIBLE_TYPE>& rhs)
      // --------------------------------------------------------------------

      if (verbose) printf("\nTESTING ASSIGNMENT OPERATORS ('weak_ptr')"
                          "\n=========================================\n");

      if (verbose) printf("\nTesting assignment from same TYPE"
                          "\n---------------------------------\n");
      {
          bslma::TestAllocator ta;
          MyTestObject *REP_PTR = new(ta) MyTestObject(&numDeletes);
          ASSERTV(REP_PTR, REP_PTR );
          TestSharedPtrRep<MyTestObject> rep(REP_PTR, &ta);
          const TestSharedPtrRep<MyTestObject>& REP = rep;
          MyTestObject *PTR = REP.ptr();
          ASSERTV(REP_PTR, PTR,          REP_PTR == PTR);
          {
              ObjSP mS(PTR, &rep); const ObjSP& S = mS;

              ObjWP mX; const ObjWP& X = mX;
              ObjWP mY; const ObjWP& Y = mY;
              ObjWP mZ; const ObjWP& Z = mZ;

              ASSERT(1 == REP.numReferences());

              mX = S;
              ASSERT(!X.expired());
              ASSERT(1 == REP.numReferences());
              ASSERT(&REP == X.rep());

              mY = X;
              ASSERT(!Y.expired());
              ASSERT(1 == REP.numReferences());
              ASSERT(&REP == Y.rep());

              mY = Z;
              ASSERT(Y.expired());
              ASSERT(1 == REP.numReferences());
              ASSERT(0 == Y.rep());
          }

          LOOP_ASSERT(REP.numReferences(),     0 == REP.numReferences());
          LOOP_ASSERT(REP.disposeRepCount(),   1 == REP.disposeRepCount());
          LOOP_ASSERT(REP.disposeObjectCount(),
                                            1 == REP.disposeObjectCount());
      }

      {
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
              ObjSP mS1(PTR1, &rep1); const ObjSP& S1 = mS1;
              ObjSP mS2(PTR2, &rep2); const ObjSP& S2 = mS2;

              ObjWP mX(S1); const ObjWP& X = mX;
              LOOP_ASSERT(REP1.numReferences(), 1 == REP1.numReferences());
              ASSERT(&REP1 == X.rep());

              ObjWP mY(S2); const ObjWP& Y = mY;
              ObjWP mZ(S2); const ObjWP& Z = mZ;
              ASSERT(&REP2 == Y.rep());
              ASSERT(&REP2 == Z.rep());

              LOOP_ASSERT(REP2.numReferences(), 1 == REP2.numReferences());

              mY = S1;
              LOOP_ASSERT(REP1.numReferences(), 1 == REP1.numReferences());

              LOOP_ASSERT(REP2.numReferences(), 1 == REP2.numReferences());
              ASSERT(&REP1 == Y.rep());

              mZ = X;
              LOOP_ASSERT(REP1.numReferences(), 1 == REP1.numReferences());

              LOOP_ASSERT(REP2.numReferences(), 1 == REP2.numReferences());
              ASSERT(&REP1 == Z.rep());
          }
          LOOP_ASSERT(REP1.numReferences(), 0 == REP1.numReferences());

          LOOP_ASSERT(REP2.numReferences(), 0 == REP2.numReferences());
      }

      if (verbose) printf("\nTesting different TYPE operators"
                          "\n--------------------------------\n");
      {
          bslma::TestAllocator ta;
          MyTestDerivedObject *REP_PTR =
                                      new(ta) MyTestDerivedObject(&numDeletes);
          TestSharedPtrRep<MyTestDerivedObject> rep(REP_PTR, &ta);
          const TestSharedPtrRep<MyTestDerivedObject>& REP = rep;
          MyTestDerivedObject *PTR = REP.ptr();
          ASSERTV(REP_PTR, PTR,          REP_PTR == PTR);
          {
              DerivedSP mS1(PTR, &rep);
              const DerivedSP& S1 = mS1;
              DerivedSP mS2; const DerivedSP& S2 = mS2;

              DerivedWP mC1(S1); const DerivedWP& C1 = mC1;
              ASSERT(&REP == C1.rep());

              DerivedWP mC2; const DerivedWP& C2 = mC2;
              ASSERT(0 == C2.rep());

              ObjWP mX; const ObjWP& X = mX;
              ObjWP mY; const ObjWP& Y = mY;
              ASSERT(0 == X.rep());
              ASSERT(0 == Y.rep());

              mX = S1;
              ASSERT(!X.expired());
              ASSERT(1 == REP.numReferences());
              ASSERT(&REP == X.rep());

              mY = C1;
              ASSERT(!Y.expired());
              ASSERT(1 == REP.numReferences());
              ASSERT(&REP == Y.rep());

              mX = S2;
              ASSERT( X.expired());
              ASSERT(1 == REP.numReferences());
              ASSERT(0 == X.rep());

              mY = C2;
              ASSERT( Y.expired());
              ASSERT(1 == REP.numReferences());
              ASSERT(0 == Y.rep());
          }

          LOOP_ASSERT(REP.numReferences(),     0 == REP.numReferences());
          LOOP_ASSERT(REP.disposeRepCount(),   1 == REP.disposeRepCount());
          LOOP_ASSERT(REP.disposeObjectCount(),
                      1 == REP.disposeObjectCount());
      }
      {
          bslma::TestAllocator ta;
          MyTestDerivedObject *REP_PTR1 =
                                      new(ta) MyTestDerivedObject(&numDeletes);
          TestSharedPtrRep<MyTestDerivedObject> rep1(REP_PTR1, &ta);
          const TestSharedPtrRep<MyTestDerivedObject>& REP1 = rep1;

          MyTestDerivedObject *REP_PTR2 =
                                      new(ta) MyTestDerivedObject(&numDeletes);
          TestSharedPtrRep<MyTestDerivedObject> rep2(REP_PTR2, &ta);
          const TestSharedPtrRep<MyTestDerivedObject>& REP2 = rep2;

          MyTestDerivedObject *PTR1 = REP1.ptr();
          MyTestDerivedObject *PTR2 = REP2.ptr();
          ASSERTV(REP_PTR1, PTR1,        REP_PTR1 == PTR1);
          ASSERTV(REP_PTR2, PTR2,        REP_PTR2 == PTR2);
          {
              DerivedSP mS1(PTR1, &rep1);
              const DerivedSP& S1 = mS1;
              DerivedSP mS2(PTR2, &rep2);
              const DerivedSP& S2 = mS2;

              ObjWP mX(S1); const ObjWP& X = mX;
              LOOP_ASSERT(REP1.numReferences(),   1 == REP1.numReferences());
              ASSERT(&REP1 == X.rep());

              mX = S2;
              LOOP_ASSERT(REP1.numReferences(),   1 == REP1.numReferences());

              LOOP_ASSERT(REP2.numReferences(),   1 == REP2.numReferences());
              ASSERT(&REP2 == X.rep());

              ObjWP mY(S1); const ObjWP& Y = mY;
              LOOP_ASSERT(REP1.numReferences(),   1 == REP1.numReferences());
              ASSERT(&REP1 == Y.rep());

              mY = X;
              LOOP_ASSERT(REP1.numReferences(),   1 == REP1.numReferences());

              LOOP_ASSERT(REP2.numReferences(),   1 == REP2.numReferences());
              ASSERT(&REP2 == Y.rep());
          }

          LOOP_ASSERT(REP1.numReferences(), 0 == REP1.numReferences());
          LOOP_ASSERT(REP1.disposeRepCount(), 1 == REP1.disposeRepCount());
          LOOP_ASSERT(REP1.disposeObjectCount(),
                                             1 == REP1.disposeObjectCount());

          LOOP_ASSERT(REP2.numReferences(), 0 == REP2.numReferences());
          LOOP_ASSERT(REP2.disposeRepCount(), 1 == REP2.disposeRepCount());
          LOOP_ASSERT(REP2.disposeObjectCount(),
                                             1 == REP2.disposeObjectCount());
      }
    } break;
    case 24: {
      // --------------------------------------------------------------------
      // TESTING CREATORS AND ACCESSORS ('weak_ptr')
      //
      // Concerns:
      //   Test that the creators work as expected.
      //
      // Plan:
      //
      // Testing:
      //   weak_ptr()
      //   weak_ptr(const weak_ptr& original)
      //   weak_ptr(const shared_ptr<COMPATIBLE_TYPE>& other)
      //   weak_ptr(const weak_ptr<COMPATIBLE_TYPE>& other)
      //   ~weak_ptr()
      //   bslma::SharedPtrRep *rep() const
      //   bool expired() const
      //   long use_count() const
      // --------------------------------------------------------------------

      if (verbose) printf("\nTESTING CREATORS AND ACCESSORS ('weak_ptr')"
                          "\n===========================================\n");

      if (verbose) printf("\nTesting default constructor"
                          "\n---------------------------\n");
      {
          ObjWP mX; const ObjWP& X = mX;
          ASSERT(X.expired());
          ASSERT(0 == X.use_count());
          ASSERT(0 == X.rep());
          ObjSP mS1 = X.lock(); const ObjSP& S1 = mS1;

          ASSERT(!S1);
          ASSERT(0 == S1.get());
      }

      if (verbose) printf("\nTesting same TYPE constructors"
                          "\n------------------------------\n");
      {
          bslma::TestAllocator ta;
          MyTestObject *REP_PTR = new(ta) MyTestObject(&numDeletes);
          TestSharedPtrRep<MyTestObject> rep(REP_PTR, &ta);
          const TestSharedPtrRep<MyTestObject>& REP = rep;
          ASSERTV(REP_PTR, REP.ptr(),          REP_PTR == REP.ptr());
          LOOP_ASSERT(REP.numReferences(),     1 == REP.numReferences());
          LOOP_ASSERT(REP.disposeRepCount(),   0 == REP.disposeRepCount());
          LOOP_ASSERT(REP.disposeObjectCount(),
                      0 == REP.disposeObjectCount());
          MyTestObject *PTR = REP.ptr();
          {
              ObjSP mS(PTR, &rep);
              const ObjSP& S = mS;
              LOOP_ASSERT(REP.numReferences(), 1 == REP.numReferences());
              LOOP_ASSERT(REP.disposeRepCount(), 0 == REP.disposeRepCount());
              LOOP_ASSERT(REP.disposeObjectCount(),
                                              0 == REP.disposeObjectCount());

              ObjWP mX(S); const ObjWP& X = mX;
              ASSERT(!X.expired());
              ASSERT(&REP == X.rep());
              LOOP_ASSERT(X.use_count(),         1 == X.use_count());
              LOOP_ASSERT(REP.numReferences(),   1 == REP.numReferences());
              LOOP_ASSERT(REP.disposeRepCount(), 0 == REP.disposeRepCount());
              LOOP_ASSERT(REP.disposeObjectCount(),
                                              0 == REP.disposeObjectCount());

              ObjWP mY(X); const ObjWP& Y = mY;
              ASSERT(!Y.expired());
              ASSERT(&REP == Y.rep());
              LOOP_ASSERT(Y.use_count(),         1 == Y.use_count());

              LOOP_ASSERT(X.use_count(),         1 == X.use_count());

              LOOP_ASSERT(REP.numReferences(), 1 == REP.numReferences());
              LOOP_ASSERT(REP.disposeRepCount(), 0 == REP.disposeRepCount());
              LOOP_ASSERT(REP.disposeObjectCount(),
                                              0 == REP.disposeObjectCount());
          }
          LOOP_ASSERT(REP.numReferences(),     0 == REP.numReferences());
          LOOP_ASSERT(REP.disposeRepCount(),   1 == REP.disposeRepCount());
          LOOP_ASSERT(REP.disposeObjectCount(),
                      1 == REP.disposeObjectCount());
      }

      if (verbose) printf("\nTesting different TYPE constructors"
                          "\n-----------------------------------\n");
      {
          bslma::TestAllocator ta;
          MyTestDerivedObject *REP_PTR =
                                      new(ta) MyTestDerivedObject(&numDeletes);
          TestSharedPtrRep<MyTestDerivedObject> rep(REP_PTR, &ta);
          const TestSharedPtrRep<MyTestDerivedObject>& REP = rep;
          ASSERTV(REP_PTR, REP.ptr(),          REP_PTR == REP.ptr());
          LOOP_ASSERT(REP.numReferences(),     1 == REP.numReferences());
          LOOP_ASSERT(REP.disposeRepCount(),   0 == REP.disposeRepCount());
          LOOP_ASSERT(REP.disposeObjectCount(),
                                            0 == REP.disposeObjectCount());
          MyTestDerivedObject *PTR = REP.ptr();
          {
              DerivedSP mS(PTR, &rep);
              const DerivedSP& S = mS;
              LOOP_ASSERT(REP.numReferences(), 1 == REP.numReferences());
              LOOP_ASSERT(REP.disposeRepCount(), 0 == REP.disposeRepCount());
              LOOP_ASSERT(REP.disposeObjectCount(),
                                              0 == REP.disposeObjectCount());

              ObjWP mX(S); const ObjWP& X = mX;
              ASSERT(!X.expired());
              ASSERT(&REP == X.rep());
              LOOP_ASSERT(X.use_count(),         1 == X.use_count());
              LOOP_ASSERT(REP.numReferences(),   1 == REP.numReferences());
              LOOP_ASSERT(REP.disposeRepCount(), 0 == REP.disposeRepCount());
              LOOP_ASSERT(REP.disposeObjectCount(),
                                              0 == REP.disposeObjectCount());

              DerivedWP mZ(S); const DerivedWP& Z = mZ;
              ObjWP mY(Z); const ObjWP& Y = mY;
              ASSERT(!Y.expired());
              ASSERT(&REP == Y.rep());
              LOOP_ASSERT(Y.use_count(),         1 == Y.use_count());

              LOOP_ASSERT(X.use_count(),         1 == X.use_count());

              LOOP_ASSERT(REP.numReferences(),   1 == REP.numReferences());
              LOOP_ASSERT(REP.disposeRepCount(), 0 == REP.disposeRepCount());
              LOOP_ASSERT(REP.disposeObjectCount(),
                                              0 == REP.disposeObjectCount());
          }
          LOOP_ASSERT(REP.numReferences(),     0 == REP.numReferences());
          LOOP_ASSERT(REP.disposeRepCount(),   1 == REP.disposeRepCount());
          LOOP_ASSERT(REP.disposeObjectCount(),
                      1 == REP.disposeObjectCount());
      }

      if (verbose) printf("\nTesting destructor"
                          "\n------------------\n");
      {
          bslma::TestAllocator ta;
          MyTestObject *REP_PTR = new(ta) MyTestObject(&numDeletes);
          TestSharedPtrRep<MyTestObject> rep(REP_PTR, &ta);
          const TestSharedPtrRep<MyTestObject>& REP = rep;
          ASSERTV(REP_PTR, REP.ptr(),          REP_PTR == REP.ptr());
          LOOP_ASSERT(REP.numReferences(),     1 == REP.numReferences());
          LOOP_ASSERT(REP.disposeRepCount(),   0 == REP.disposeRepCount());
          LOOP_ASSERT(REP.disposeObjectCount(),
                      0 == REP.disposeObjectCount());
          MyTestObject *PTR = REP.ptr();
          {
              ObjWP mY; const ObjWP& Y = mY;
              (void) Y;  // Suppress 'unused variable' warning
              {
                  ObjSP mS(PTR, &rep); const ObjSP& S = mS;
                  LOOP_ASSERT(REP.numReferences(), 1 == REP.numReferences());
                  LOOP_ASSERT(REP.disposeRepCount(),
                                                 0 == REP.disposeRepCount());
                  LOOP_ASSERT(REP.disposeObjectCount(),
                                              0 == REP.disposeObjectCount());

                  {
                      ObjWP mX(S); const ObjWP& X = mX;
                      (void) X;  // Suppress 'unused variable' warning
                      LOOP_ASSERT(REP.numReferences(),
                                                   1 == REP.numReferences());
                      LOOP_ASSERT(REP.disposeRepCount(),
                                                 0 == REP.disposeRepCount());
                      LOOP_ASSERT(REP.disposeObjectCount(),
                                              0 == REP.disposeObjectCount());
                  }

                  LOOP_ASSERT(REP.numReferences(), 1 == REP.numReferences());
                  LOOP_ASSERT(REP.disposeRepCount(),
                                                 0 == REP.disposeRepCount());
                  LOOP_ASSERT(REP.disposeObjectCount(),
                                              0 == REP.disposeObjectCount());

                  mY = S;
                  LOOP_ASSERT(REP.numReferences(), 1 == REP.numReferences());
                  LOOP_ASSERT(REP.disposeRepCount(),
                              0 == REP.disposeRepCount());
                  LOOP_ASSERT(REP.disposeObjectCount(),
                              0 == REP.disposeObjectCount());
              }

              LOOP_ASSERT(REP.numReferences(), 0 == REP.numReferences());
              LOOP_ASSERT(REP.disposeRepCount(),
                          0 == REP.disposeRepCount());
              LOOP_ASSERT(REP.disposeObjectCount(),
                          1 == REP.disposeObjectCount());
          }

          LOOP_ASSERT(REP.numReferences(),   0 == REP.numReferences());
          LOOP_ASSERT(REP.disposeRepCount(), 1 == REP.disposeRepCount());
          LOOP_ASSERT(REP.disposeObjectCount(),
                                             1 == REP.disposeObjectCount());
      }
    } break;
    case 23: {
        // --------------------------------------------------------------------
        // TESTING 'createInplace'
        //
        // Concerns:
        //
        // Plan: TBD
        //
        // Testing:
        //   void createInplace(bslma::Allocator *allocator=0)
        //   void createInplace(bslma::Allocator *, const A1& a1)
        //   void createInplace(bslma::Allocator *, const A1& a1, ...& a2)
        //   void createInplace(bslma::Allocator *, const A1& a1, ...& a3)
        //   void createInplace(bslma::Allocator *, const A1& a1, ...& a4)
        //   void createInplace(bslma::Allocator *, const A1& a1, ...& a5)
        //   void createInplace(bslma::Allocator *, const A1& a1, ...& a6)
        //   void createInplace(bslma::Allocator *, const A1& a1, ...& a7)
        //   void createInplace(bslma::Allocator *, const A1& a1, ...& a8)
        //   void createInplace(bslma::Allocator *, const A1& a1, ...& a9)
        //   void createInplace(bslma::Allocator *, const A1& a1, ...& a10)
        //   void createInplace(bslma::Allocator *, const A1& a1, ...& a11)
        //   void createInplace(bslma::Allocator *, const A1& a1, ...& a12)
        //   void createInplace(bslma::Allocator *, const A1& a1, ...& a13)
        //   void createInplace(bslma::Allocator *, const A1& a1, ...& a14)
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING 'createInplace'"
                            "\n=======================\n");

        static const MyTestArg1  V1(1);
        static const MyTestArg2  V2(20);
        static const MyTestArg3  V3(23);
        static const MyTestArg4  V4(44);
        static const MyTestArg5  V5(66);
        static const MyTestArg6  V6(176);
        static const MyTestArg7  V7(878);
        static const MyTestArg8  V8(8);
        static const MyTestArg9  V9(912);
        static const MyTestArg10 V10(102);
        static const MyTestArg11 V11(111);
        static const MyTestArg12 V12(333);
        static const MyTestArg13 V13(712);
        static const MyTestArg14 V14(1414);

        if (verbose) printf("\nTesting 'createInplace' with 0 arguments"
                            "\n----------------------------------------\n");

        bslma::TestAllocator ta("Rep. allocator", veryVeryVerbose);

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bsl::shared_ptr<MyInplaceTestObject> x;
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;
            static const MyInplaceTestObject EXP = MyInplaceTestObject();

            x.createInplace(&ta);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.get()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'createInplace' with 1 argument"
                            "\n---------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bsl::shared_ptr<MyInplaceTestObject> x;
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;
            static const MyInplaceTestObject EXP(V1);

            x.createInplace(&ta, V1);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.get()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'createInplace' with 2 arguments"
                            "\n----------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bsl::shared_ptr<MyInplaceTestObject> x;
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;
            static const MyInplaceTestObject EXP(V1, V2);

            x.createInplace(&ta, V1, V2);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.get()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'createInplace' with 3 arguments"
                            "\n----------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bsl::shared_ptr<MyInplaceTestObject> x;
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;
            static const MyInplaceTestObject EXP(V1, V2, V3);

            x.createInplace(&ta, V1, V2, V3);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.get()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'createInplace' with 4 arguments"
                            "\n----------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bsl::shared_ptr<MyInplaceTestObject> x;
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;
            static const MyInplaceTestObject EXP(V1, V2, V3, V4);

            x.createInplace(&ta, V1, V2, V3, V4);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.get()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'createInplace' with 5 arguments"
                            "\n----------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bsl::shared_ptr<MyInplaceTestObject> x;
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5);

            x.createInplace(&ta, V1, V2, V3, V4, V5);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.get()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'createInplace' with 6 arguments"
                            "\n----------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bsl::shared_ptr<MyInplaceTestObject> x;
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6);

            x.createInplace(&ta, V1, V2, V3, V4, V5, V6);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.get()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'createInplace' with 7 arguments"
                            "\n----------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bsl::shared_ptr<MyInplaceTestObject> x;
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7);

            x.createInplace(&ta, V1, V2, V3, V4, V5, V6,V7);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.get()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'createInplace' with 9 arguments"
                            "\n----------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bsl::shared_ptr<MyInplaceTestObject> x;
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                                                 V8);

            x.createInplace(&ta, V1, V2, V3, V4, V5, V6,V7, V8);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.get()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'createInplace' with 10 arguments"
                            "\n-----------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bsl::shared_ptr<MyInplaceTestObject> x;
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                                                 V8, V9);

            x.createInplace(&ta, V1, V2, V3, V4, V5, V6,V7, V8, V9);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.get()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'createInplace' with 2 arguments"
                            "\n----------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bsl::shared_ptr<MyInplaceTestObject> x;
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                                                 V8, V9, V10);

            x.createInplace(&ta, V1, V2, V3, V4, V5, V6,V7, V8, V9, V10);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.get()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'createInplace' with 11 arguments"
                            "\n----------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bsl::shared_ptr<MyInplaceTestObject> x;
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                                                 V8, V9, V10, V11);

            x.createInplace(&ta, V1, V2, V3, V4, V5, V6,V7, V8, V9, V10, V11);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.get()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'createInplace' with 12 arguments"
                            "\n-----------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bsl::shared_ptr<MyInplaceTestObject> x;
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                                                 V8, V9, V10, V11, V12);

            x.createInplace(&ta, V1, V2, V3, V4, V5, V6,V7, V8, V9, V10, V11,
                                 V12);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.get()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'createInplace' with 13 arguments"
                            "\n-----------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bsl::shared_ptr<MyInplaceTestObject> x;
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                                                 V8, V9, V10, V11, V12, V13);

            x.createInplace(&ta, V1, V2, V3, V4, V5, V6,V7, V8, V9, V10,
                                 V11, V12, V13);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.get()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'createInplace' with 14 arguments"
                            "\n-----------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bsl::shared_ptr<MyInplaceTestObject> x;
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                                                 V8, V9, V10, V11, V12, V13,
                                                 V14);

            x.createInplace(&ta, V1, V2, V3, V4, V5, V6,V7, V8, V9, V10, V11,
                                 V12, V13, V14);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.get()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

#if 0 // TBD Waiting on a usable 'bind' facility to port to a good level
        if (verbose)
            printf("\nTesting 'createInplace' passing allocator to args"
                   "\n-------------------------------------------------\n");

        bslma::TestAllocator ta0;
        bslma::TestAllocator ta1;
        bslma::TestAllocator ta2;

        bslma::TestAllocator *Z0 = &ta0;
        bslma::TestAllocator *Z1 = &ta1;
        bslma::TestAllocator *Z2 = &ta2;

        bdef_Bind_TestSlotsAlloc::setZ0(Z0);
        bdef_Bind_TestSlotsAlloc::setZ1(Z1);
        bdef_Bind_TestSlotsAlloc::setZ2(Z2);

        bslma::DefaultAllocatorGuard allocGuard(Z0);

        bdef_Bind_TestArgAlloc<1>  VA1(1, Z2);
        bdef_Bind_TestArgAlloc<2>  VA2(2, Z2);
        bdef_Bind_TestArgAlloc<3>  VA3(3, Z2);
        bdef_Bind_TestArgAlloc<4>  VA4(4, Z2);
        bdef_Bind_TestArgAlloc<5>  VA5(5, Z2);
        bdef_Bind_TestArgAlloc<6>  VA6(6, Z2);
        bdef_Bind_TestArgAlloc<7>  VA7(7, Z2);
        bdef_Bind_TestArgAlloc<8>  VA8(8, Z2);
        bdef_Bind_TestArgAlloc<9>  VA9(9, Z2);
        bdef_Bind_TestArgAlloc<10> VA10(10, Z2);
        bdef_Bind_TestArgAlloc<11> VA11(11, Z2);
        bdef_Bind_TestArgAlloc<12> VA12(12, Z2);
        bdef_Bind_TestArgAlloc<13> VA13(13, Z2);
        bdef_Bind_TestArgAlloc<14> VA14(14, Z2);

        bslma::Allocator *ALLOC_SLOTS[] = {
            //   1,  2   3   4   5   6   7   8   9  10  11  12  13  14
            Z0, Z1, Z1, Z1, Z1, Z1, Z1, Z1, Z1, Z1, Z1, Z1, Z1, Z1, Z1
        };

        {
            bsl::shared_ptr<bdef_Bind_TestTypeAlloc> x;
            const bsl::shared_ptr<bdef_Bind_TestTypeAlloc>& X=x;

            const bdef_Bind_TestTypeAlloc EXP(Z0, VA1, VA2, VA3, VA4,
                    VA5, VA6, VA7, VA8, VA9, VA10, VA11, VA12, VA13);

            x.createInplace(Z1, Z1, VA1, VA2, VA3, VA4, VA5, VA6,VA7, VA8, VA9,
                    VA10, VA11, VA12, VA13);

            bdef_Bind_TestSlotsAlloc::resetSlots(Z0);
            x->setSlots(); // should set slots to Z1, as this should be the
                           // allocator used by data members of *(X.get())

            ASSERT(X);
            ASSERT(EXP == *(X.get()));
            ASSERT(bdef_Bind_TestSlotsAlloc::verifySlots(ALLOC_SLOTS,
                                                         verbose));
        }
#endif
        ASSERTV(defaultAllocator.numAllocations(),   numDefaultAllocations,
                defaultAllocator.numAllocations() == numDefaultAllocations);
        ASSERTV(defaultAllocator.numDeallocations(),  numDefaultDeallocations,
               defaultAllocator.numDeallocations() == numDefaultDeallocations);

        if (verbose) printf(
                         "\nTesting 'createInplace' with default allocator"
                         "\n----------------------------------------------\n");


        numAllocations = defaultAllocator.numAllocations();
        numDeallocations = defaultAllocator.numDeallocations();
        {
            bsl::shared_ptr<MyInplaceTestObject> x;
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;
            static const MyInplaceTestObject EXP = MyInplaceTestObject();

            x.createInplace();

            ASSERT(++numAllocations == defaultAllocator.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.get()));
        }
        ASSERT(++numDeallocations == defaultAllocator.numDeallocations());

      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING 'bsl::shared_ptr<cv-void> (DRQS 33549823)
        //
        // Concerns:
        //: 1 Can construct a shared pointer to a cv-qualified 'void' type.
        //:
        //: 2 Can perform basic operations that do not dereference such a
        //:   pointer.
        //
        // Plan:
        //: 1 Create a 'shared_ptr<cv-void>' object for each cv variant.
        //:
        //: 2 Run through a quick set of reasonable operations for a non-
        //:   dereferenceable smart pointer.
        //:
        //: 3 Verify that an assertion failure does not happen (in any mode).
        //
        // Testing:
        //   shared_ptr<cv-void>
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING 'bsl::shared_ptr<cv-void> (DRQS 33549823)"
                   "\n=================================================\n");

        if (verbose) printf("Confirming bsl::shared_ptr<void> support.\n");
        {
            typedef bsl::shared_ptr<void> TestObj;
            int iX = 42;
            TestObj pX(&iX, bslstl::SharedPtrNilDeleter(), 0);
            TestObj pY = pX;  const TestObj& Y = pY;

            ASSERT(   Y == pX );
            ASSERT(!(pX != Y) );

            ASSERT(&iX == static_cast<int *>(Y.get()));

            double dY = 3.14159;
            pY.reset(&dY, bslstl::SharedPtrNilDeleter());

            ASSERT(   Y != pX );
            ASSERT(!(pX == Y) );

            ASSERT(&dY == static_cast<double *>(Y.get()));
        }

        if (verbose) printf(
                          "Confirming bsl::shared_ptr<const void> support.\n");
        {
            typedef bsl::shared_ptr<const void> TestObj;
            const int iX = 42;
            TestObj pX(&iX, bslstl::SharedPtrNilDeleter(), 0);
            TestObj pY = pX;  const TestObj& Y = pY;

            ASSERT(   Y == pX );
            ASSERT(!(pX != Y) );

            ASSERT(&iX == static_cast<const int *>(Y.get()));

            double dY = 3.14159;
            pY.reset(&dY, bslstl::SharedPtrNilDeleter());

            ASSERT(   Y != pX );
            ASSERT(!(pX == Y) );

            ASSERT(&dY == static_cast<const double *>(Y.get()));
        }

#if 0   // volatile types are not yet supported in general.  The test case is
        // retained as a demonstration of what might need fixing if support for
        // volatile types becomes desirable, such as for standard conformance.
        // Note that the current failures occur in the out-of-place rep type.

        if (verbose) printf(
                       "Confirming bsl::shared_ptr<volatile void> support.\n");
        {
            typedef bsl::shared_ptr<volatile void> TestObj;
            int iX = 42;
            TestObj pX(&iX, bslstl::SharedPtrNilDeleter(), 0);
            TestObj pY = pX;  const TestObj& Y = pY;

            ASSERT(   Y == pX );
            ASSERT(!(pX != Y) );

            ASSERT(&iX == static_cast<volatile int *>(Y.get()));

            volatile double dY = 3.14159;
            pY.reset(&dY, bslstl::SharedPtrNilDeleter());

            ASSERT(   Y != pX );
            ASSERT(!(pX == Y) );

            ASSERT(&dY == static_cast<volatile double *>(Y.get()));
        }

        if (verbose) printf(
                 "Confirming bsl::shared_ptr<const volatile void> support.\n");
        {
            typedef bsl::shared_ptr<const volatile void> TestObj;
            volatile int iX = 42;
            TestObj pX(&iX, bslstl::SharedPtrNilDeleter(), 0);
            TestObj pY = pX;  const TestObj& Y = pY;

            ASSERT(   Y == pX );
            ASSERT(!(pX != Y) );

            ASSERT(&iX == static_cast<const volatile int *>(Y.get()));

            const double dY = 3.14159;
            pY.reset(&dY, bslstl::SharedPtrNilDeleter());

            ASSERT(   Y != pX );
            ASSERT(!(pX == Y) );

            ASSERT(&dY == static_cast<const volatile double *>(Y.get()));
        }
#endif
    } break;
    case 21: {
        // --------------------------------------------------------------------
        // TESTING 'reset' USING A SELF-REFERENCED 'shared_ptr'
        //   Verify that 'reset' can be called safely on a 'shared_ptr' that is
        //   indirectly holding its last reference to itself.  This test was
        //   added to address issues identified by the internal ticket DRQS
        //   26465543.
        //
        // Concerns:
        //: 1 Resetting the last reference to a self-referenced shared pointer
        //:   calls 'releaseRef' only once.
        //
        // Plan:
        //: 1 Create a self-referring shared pointer.
        //:
        //: 2 Call 'reset' on the referenced object.
        //:
        //: 3 Verify that an assertion failure does not happen (in any mode).
        //
        // Testing:
        //   DRQS 26465543 [void reset()]
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING 'reset' USING A SELF-REFERENCED 'shared_ptr'"
                   "\n====================================================\n");

        SelfReference *ptr;
        {
            bsl::shared_ptr<SelfReference> mX;
            mX.createInplace();
            mX->setData(mX);
            ptr = mX.get();
        }

        ptr->release();
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTION FROM 'ManagedPtr'
        //
        // Concerns:
        //   1) When constructing from a managed-ptr, the original deleter
        //      specified in the managed-ptr is used to destroy the object
        //
        //   2) When constructing from an aliased managed-ptr, the original
        //      deleter is supplied the correct address
        //
        //   3) A managed pointer can be assigned to a shared pointer, through
        //      the same conversion sequence.  DRQS 38359639.
        //
        //   4) Can convert from rvalues (function results) as well as from
        //      lvalue.
        //
        //   5) The target shared pointer can point to the same type as the
        //      managed pointer, or to a base class.
        //
        //   6) No memory is allocated when simply transferring ownership from
        //      a managed pointer created from a previous shared pointer.
        //
        // Plan:
        //   TBD
        //
        // Testing:
        //   shared_ptr(bslma::ManagedPtr<OTHER>&, bslma::Allocator * = 0)
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING CONSTRUCTION FROM 'ManagedPtr'"
                            "\n======================================\n");

        ManagedPtrTestDeleter<bsls::Types::Int64> deleter;

        bsls::Types::Int64 obj1, obj2;

        struct Local {
            static
            bslma::ManagedPtr<bsls::Types::Int64> makeManagedInt(
                                                          bsls::Types::Int64 x)
            {
                bslma::Allocator *pda = bslma::Default::defaultAllocator();
                bsls::Types::Int64 *pX = new(*pda) bsls::Types::Int64(x);
                return bslma::ManagedPtr<bsls::Types::Int64>(pX, pda);
            }
        };

        bsl::shared_ptr<bsls::Types::Int64> outerSp;
        {
            bslma::ManagedPtr<bsls::Types::Int64> mp1 (&obj1, &deleter);

            bsl::shared_ptr<bsls::Types::Int64> sp1 (mp1);
            sp1.reset();

            // check non-aliased managed-ptr assignment
            ASSERT(&obj1 == deleter.providedObj());

            deleter.reset();
            mp1.load(&obj2, &deleter);

            bslma::ManagedPtr<bsls::Types::Int64> mp2 (mp1, &obj1);
            bsl::shared_ptr<bsls::Types::Int64> sp2 (mp2);
            outerSp = sp2;
        }
        outerSp.reset();
        // check aliased managed-ptr assignment
        ASSERT(&obj2 == deleter.providedObj());

        {
            bsl::shared_ptr<bsls::Types::Int64> sp1(Local::makeManagedInt(13));
//          bsl::shared_ptr<bsls::Types::Int64> sp2 = Local::makeManagedInt(2);
        }

        // It is not clear if assignment from a ManagedPtr is intended to be
        // supported by anything other than through implicit conversion to a
        // temporary shared_ptr, and then binding to that assignment operator.
        // As such, we would support only assigning from ManagedPtr lvalues,
        // but not from rvalues.

//        outerSp = Local::makeManagedInt(42);

        bslma::ManagedPtr<MyTestObject>   mpd1(new MyTestObject(&obj1));
        bsl::shared_ptr<MyTestBaseObject> spd1(mpd1);

        bslma::ManagedPtr<MyTestObject>   mpd2(new MyTestObject(&obj2));
        bsl::shared_ptr<MyTestBaseObject> spd2 = mpd2;
      } break;

      case 19: {
        // --------------------------------------------------------------------
        // TESTING EXPLICIT CAST OPERATIONS
        //   Test that explicit cast operations properly loads the object
        //
        // Plan: TBD
        //
        // Testing:
        //  shared_ptr<TARGET> const_pointer_cast(const shared_ptr<SRC>& ptr)
        //  shared_ptr<TARGET> dynamic_pointer_cast(const shared_ptr<SRC>& ptr)
        //  shared_ptr<TARGET> static_pointer_cast(const shared_ptr<SRC>& ptr)
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING EXPLICIT CAST OPERATIONS"
                            "\n================================\n");

        if (verbose) printf("\nTesting 'dynamic_pointer_cast'"
                            "\n------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            MyTestDerivedObject *p = new(ta) MyTestDerivedObject(&numDeletes);
            Obj x(p, &ta, 0); const Obj& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(static_cast<MyTestObject*>(p) == X.get());
            ASSERT(1 == X.use_count());

            bsl::shared_ptr<MyTestObject2> y;
            const bsl::shared_ptr<MyTestObject2>& Y = y;

            {
                // This inner block necessary against Sun CC bug, the lifetime
                // of the temporary copied into y would otherwise pollute the
                // Y.use_count below.
                y = ::bsl::dynamic_pointer_cast<MyTestObject2>(X);
            }
            if (veryVerbose) {
                P_(Y.get());
                P_(X.use_count());
                P(Y.use_count());
            }
            ASSERT(p == Y.get());
            ASSERT(2 == X.use_count());
            ASSERT(2 == Y.use_count());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
        ASSERT(1 == numDeletes);

        if (verbose) printf(
                           "\nTesting 'dynamic_pointer_cast' that fails"
                           "\n-----------------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            MyTestDerivedObject *p = new(ta) MyTestDerivedObject(&numDeletes);
            Obj x(p, &ta, 0); const Obj& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(static_cast<MyTestObject*>(p) == X.get());
            ASSERT(1 == X.use_count());

            {
                // Cast to any other type with a vtable.  We know that
                // 'bsl::bad_weak_ptr' has a virtual destructor, and will be
                // tested as part of this component.

                bsl::shared_ptr<bsl::bad_weak_ptr> y;
                const bsl::shared_ptr<bsl::bad_weak_ptr>& Y = y;

                {
                    // This inner block necessary against Sun CC bug, the
                    // lifetime of the temporary copied into y would otherwise
                    // pollute the Y.use_count below.
                    y = ::bsl::dynamic_pointer_cast<bsl::bad_weak_ptr>(X);
                }
                if (veryVerbose) {
                    P_(Y.get());
                    P_(X.use_count());
                    P(Y.use_count());
                }
                ASSERT(0 == Y.get());
                ASSERT(1 == X.use_count());
                ASSERT(0 == Y.use_count());
                ASSERT(numAllocations == ta.numAllocations());
                ASSERT(numDeallocations == ta.numDeallocations());
                ASSERT(0 == numDeletes);
            }
            ASSERT(1 == X.use_count());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
        ASSERT(1 == numDeletes);

        if (verbose) printf(
                           "\nTesting 'dynamic_pointer_cast' aliasing null"
                           "\n--------------------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            MyTestDerivedObject *p = new(ta) MyTestDerivedObject(&numDeletes);
            Obj z(p, &ta, 0); const Obj& Z = z;
            Obj x(Z, 0); const Obj& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(0 == X.get());
            ASSERT(2 == X.use_count());

            {
                // Cast to any other type with a vtable.  We know that
                // 'bsl::bad_weak_ptr' has a virtual destructor, and will be
                // tested as part of this component.

                bsl::shared_ptr<MyTestDerivedObject> y;
                const bsl::shared_ptr<MyTestDerivedObject>& Y = y;

                {
                    // This inner block necessary against Sun CC bug, the
                    // lifetime of the temporary copied into y would otherwise
                    // pollute the Y.use_count below.
                    y = ::bsl::dynamic_pointer_cast<MyTestDerivedObject>(X);
                }
                if (veryVerbose) {
                    P_(Y.get());
                    P_(X.use_count());
                    P(Y.use_count());
                }
                ASSERT(0 == Y.get());
                ASSERT(2 == X.use_count());
                ASSERT(0 == Y.use_count());
                ASSERT(numAllocations == ta.numAllocations());
                ASSERT(numDeallocations == ta.numDeallocations());
                ASSERT(0 == numDeletes);
            }
            ASSERT(2 == X.use_count());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
        ASSERT(1 == numDeletes);

        if (verbose) printf(
                 "\nTesting 'dynamic_pointer_cast' aliasing empty non-null"
                 "\n------------------------------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            MyTestDerivedObject D(&numDeletes); MyTestDerivedObject *p = &D;
            Obj x(Obj(), p); const Obj& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(static_cast<MyTestObject *>(p) == X.get());
            ASSERT(0 == X.use_count());

            bsl::shared_ptr<MyTestDerivedObject> y;
            const bsl::shared_ptr<MyTestDerivedObject>& Y = y;

            {
                // This inner block necessary against Sun CC bug, the lifetime
                // of the temporary copied into y would otherwise pollute the
                // Y.use_count below.
                y = ::bsl::dynamic_pointer_cast<MyTestDerivedObject>(X);
            }
            if (veryVerbose) {
                P_(Y.get());
                P_(X.use_count());
                P(Y.use_count());
            }
            ASSERT(p == Y.get());
            ASSERT(0 == X.use_count());
            ASSERT(0 == Y.use_count());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);
        }
        ASSERT(numDeallocations == ta.numDeallocations());
        ASSERT(1 == numDeletes);

        if (verbose) printf("\nTesting 'static_pointer_cast'"
                            "\n-----------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            MyTestDerivedObject *p = new(ta) MyTestDerivedObject(&numDeletes);
            bsl::shared_ptr<MyTestDerivedObject> x(p, &ta, 0);
            const bsl::shared_ptr<MyTestDerivedObject>& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(p == X.get());
            ASSERT(1 == X.use_count());

            Obj y(::bsl::static_pointer_cast<TObj>(X)); const Obj& Y=y;

            ASSERT(static_cast<MyTestObject*>(p) == Y.get());
            ASSERT(2 == X.use_count());
            ASSERT(2 == Y.use_count());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
        ASSERT(1 == numDeletes);

        if (verbose) printf("\nTesting 'const_pointer_cast'"
                            "\n----------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            // Construct 'ConstObj' with a nil deleter.  This exposes a former
            // const-safety bug.
            bsls::Types::Int64 counter = 0;
            const MyTestObject V(&counter);
            ConstObj x(&V, bslstl::SharedPtrNilDeleter(), 0);
        }
        {
            numDeletes = 0;
            TObj *p = new(ta) TObj(&numDeletes);
            ConstObj x(p, &ta, 0); const ConstObj& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(p == X.get());
            ASSERT(1 == X.use_count());

            Obj y(::bsl::const_pointer_cast<TObj>(X)); const Obj& Y=y;

            ASSERT(const_cast<TObj*>(p) == Y.get());
            ASSERT(2 == X.use_count());
            ASSERT(2 == Y.use_count());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
        ASSERT(1 == numDeletes);
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING 'bslma::SharedPtrOutofplaceRep' CONSTRUCTORS
        //
        // CONCERN: bslma::SharedPtrOutofplaceRep passes allocator to the
        // deleter's constructor.
        //
        // This appears to be properly the concern of the out-of-place rep
        // component's test driver, and should be removed from here.
        //
        // Plan: TBD
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING 'bslma::SharedPtrOutofplaceRep' CONSTRUCTORS"
                   "\n====================================================\n");

        if (verbose) printf("\nConcern: bslma::SharedPtrOutofplaceRep passes"
                            "\nallocator to the deleter's constructor."
                            "\n=======================================\n");

        bslma::TestAllocator ta1(veryVeryVerbose);
        bslma::TestAllocator ta2(veryVeryVerbose);
        bsls::Types::Int64 numDeletes1 = 0;

        {
            bslma::DefaultAllocatorGuard allocGuard(&ta1);

            MyTestObject *p1 = new (ta2) MyTestObject(&numDeletes1);
            MyAllocTestDeleter d(&ta2, &ta2);
            Obj x1(p1,d,&ta2);
            ASSERT(0 == ta1.numBytesInUse());
        }

        ASSERT(1 == numDeletes1);
        ASSERT(0 == ta1.numBytesInUse());
        ASSERT(0 == ta2.numBytesInUse());

      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING 'release'
        //
        // Concerns:
        //: 1 'release' returns a 'pair' where 'first' is a pointer to the same
        //:   object as the 'shared_ptr' points to before calling 'release'.
        //: 2 'release' returns a 'pair' where 'second' is a pointer to a
        //:   'SharedPtrRep' :   that has not yet released its ownership of the
        //:   reference held by the 'shared_ptr' object prior to calling
        //:   'release'.
        //: 3 'first' has the correct value when the 'shared_ptr' is aliasing
        //:   a completely unrelated type and data structure.
        //: 4 'second' has the correct value when the 'shared_ptr' is aliasing
        //:   a completely unrelated type and data structure.
        //: 5 'second' returns a Rep that can destroy the last reference when
        //:   the 'shared_ptr' held an out-of-place representation.
        //: 6 'second' returns a Rep that can destroy the last reference when
        //:   the 'shared_ptr' held an in-place representation.
        //: 7 'second' returns a Rep that can destroy the last reference when
        //:   the 'shared_ptr' held an custom (user-supplied) representation.
        //: 8 Do the right thing for empty null pointers, which means tracking
        //:   our expected behavior for reference-counting deleters.
        //
        // Plan:  Create shared pointers with various representations, release
        // them (getting back a pointer to the representation object) and
        // assert that the 'originalPtr' of that representation is identical to
        // the address of the managed object.
        //
        // Testing:
        //   pair<TYPE *, bslma::SharedPtrRep *> release()
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING 'release'"
                   "\n=================\n");

        if (verbose)
            printf("\nConcern: 'bslma::SharedPtrRep::originalPtr' returns"
                   "\ncorrect value for 'bslma::SharedPtrOutofplaceRep'"
                   "\n=================================================\n");

        {
            bslma::TestAllocator ta(veryVeryVerbose);
            bsls::Types::Int64 numDeletes1 = 0;
            MyTestObject *p1 = new (ta) MyTestObject(&numDeletes1);
            Obj x1(p1, &ta);

            bsl::pair<MyTestObject *, bslma::SharedPtrRep *> r = x1.release();

            ASSERT(0 == x1.get());

            ASSERT(r.first == r.second->originalPtr());
            ASSERT(p1 == r.first);

            Obj x2(r.first, r.second);
            x2.reset();

            ASSERT(1 == numDeletes1);
            ASSERT(2 == ta.numAllocations());
            ASSERT(0 == ta.numBytesInUse());
        }

        if (verbose)
            printf("\nConcern: 'bslma::SharedPtrRep::originalPtr' returns"
                   "\ncorrect value for 'bslma::SharedPtrInplaceRep'"
                   "\n====================================================\n");

        {
            bslma::TestAllocator ta(veryVeryVerbose);
            bsls::Types::Int64 numDeletes1 = 0;
            Obj x1;
            x1.createInplace(&ta, &numDeletes1);
            bsl::pair<MyTestObject *, bslma::SharedPtrRep *> r = x1.release();

            ASSERT(0 == x1.get());

            ASSERT(r.first == r.second->originalPtr());

            Obj x2(r.first, r.second);
            x2.reset();

            ASSERT(1 == numDeletes1);
            ASSERT(1 == ta.numAllocations());
            ASSERT(0 == ta.numBytesInUse());
        }

        if (verbose)
            printf("\nConcern: 'bslma::SharedPtrRep::originalPtr' returns"
                   "\ncorrect value when aliased"
                   "\n===================================================\n");

        {
            typedef bsl::vector<MyTestObject2> V;
            bslma::TestAllocator ta(veryVeryVerbose);
            V *v1 = new (ta) V(&ta);
            bsl::shared_ptr<V> x1(v1,&ta);
            v1->resize(2);

            bsl::shared_ptr<MyTestObject2> a1(x1,&v1->at(1));
            bsl::pair<MyTestObject2 *, bslma::SharedPtrRep *> r = a1.release();
            ASSERT(0 == a1.get());

            ASSERT(r.first != r.second->originalPtr());
            ASSERT(v1 == r.second->originalPtr());

            r.second->releaseRef();
            x1.reset();

            ASSERT(0 == ta.numBytesInUse());
        }

      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING CONVERSION TO BOOL
        //
        // Concerns: Implicit conversion to 'bool' are troublesome, because a
        //   bool is further convertible to integral types, and thus such
        //   conversions should be banned.  Specifically, we are concerned that
        //     o SharedPtr can be used in "boolean" contexts such as 'if (p)',
        //       'if (!p)', 'if
        //     o SharedPtr cannot be converted to an 'int'.
        //     o SharedPtr returned by a function (as a temporary) does not
        //       lead to erroneous bool value (DRQS 12252806).
        //
        // Plan: We test the conversion in a variety of "boolean" contexts and
        //   assert that the result is as expected.  In order to test for the
        //   *absence* of conversion, we can use 'bslmf_isconvertible'.  In
        //   order to test for the absence of 'operator<', we use our own
        //   definition of 'operator<', which will be picked up and that will
        //   create an ambiguity if one is already defined.  We verify that our
        //   'operator<' has been picked up by using a helper function, which
        //   has two matchings, one to the return type of our definition of
        //   'operator<', and the other to '...'.
        //
        // Testing:
        //   operator BoolType() const
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING CONVERSION TO BOOL"
                            "\n==========================\n");

        using namespace NAMESPACE_TEST_CASE_16;

        if (verbose) printf("Not convertible to ints.\n");

        ASSERT((0 == bslmf::IsConvertible<bsl::shared_ptr<int>, int>::VALUE));

        if (verbose) printf("Simple boolean expressions.\n");

        ASSERT(!ptrNil);
        ASSERT(ptr1);

        if (verbose) printf("Comparisons.\n");

            // COMPARISON SHR PTR TO SHR PTR
        ASSERT(!(ptrNil == ptr1));
        ASSERT(ptrNil != ptr1);

            // COMPARISON SHR PTR TO BOOL
        ASSERT(static_cast<bool>(ptrNil) == false);
        ASSERT(static_cast<bool>(ptr1)   != false);

            // COMPARISON BOOL TO SHR PTR
        ASSERT(false == static_cast<bool>(ptrNil));
        ASSERT(false != static_cast<bool>(ptr1));
        ASSERT(true  && ptr1);

            // COMPARISON SHR PTR TO NULL POINTER LITERAL
        ASSERT(ptrNil == 0);
        ASSERT(ptr1 != 0);
        ASSERT(0 == ptrNil);
        ASSERT(0 != ptr1);

        if (verbose) printf("Boolean operators.\n");

        ASSERT(!ptrNil && true);
        ASSERT(!ptrNil || false);
        ASSERT(ptr1 && true);
        ASSERT(ptr1 || false);

        ASSERT(true && !ptrNil);
        ASSERT(false || !ptrNil);
        ASSERT(true && ptr1);
        ASSERT(false || ptr1);

        ASSERT(!ptrNil && 1);
        ASSERT(!ptrNil || 0);
        ASSERT(ptr1 && 1);
        ASSERT(ptr1 || 0);

        ASSERT(1 && !ptrNil);
        ASSERT(0 || !ptrNil);
        ASSERT(1 && ptr1);
        ASSERT(0 || ptr1);

        if (verbose) printf("With function return values.\n");
        // DRQS 12252806

        ASSERT(ptr1Fun() && true);
        ASSERT(!ptrNilFun() && true);

        ASSERT(!ptrNilFun() && ptrNilFun() == ptrNil);
        ASSERT(!ptrNilFun() || ptrNilFun() != ptr1);
        ASSERT(ptr1Fun() && ptr1Fun() == ptr1);
        ASSERT(ptr1Fun() || ptr1Fun() != ptrNil);

        ASSERT(!(ptrNilFun() && *ptrNilFun() != *ptr1));

#if 0
        if (verbose) printf("Should not compile.\n\n");;

        // COMPARISON SHR PTR TO INT
        ASSERT(ptrNil != 1);
        ASSERT(ptr1 == 1);
        ASSERT(ptrNil != 2);
        ASSERT(ptr1 == 2);

        // COMPARISON INT TO SHR PTR
        ASSERT(0 == ptrNil);  // should compile, comparing with nullptr
        ASSERT(0 != ptr1);    // should compile, comparing with nullptr
        ASSERT(1 != ptrNil);
        ASSERT(1 == ptr1);
        ASSERT(2 != ptrNil);
        ASSERT(2 == ptr1);  printf("r on CC, error on x.\n\n");;
#endif

      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: C++ 'bsl::shared_ptr' ISO CONFORMANCE
        //   Note that these tests assume that the default allocator is the
        //   NewDelete allocator; installing a test allocator for the default
        //   will break this test.
        //
        // Plan:  Check that the 'reset' function works, by simply exercising
        //   them on a non-empty shared pointer, and checking that the result
        //   and reference counts are as expected.  Also check that the 'swap'
        //   global function works, using the same test plan as in case 11.
        //
        //   No longer tested because no longer part of this component, but
        //   part of the C++ compliance test, we would check that the
        //   '..._pointer_cast' global functions work, using the same test plan
        //   as in case 9.  For 'get_deleter' we would need to test that
        //   'get_deleter' of objects created empty, or with the default or
        //   test allocator, a factory, or function-like deleters does return a
        //   pointer to the deleter if the correct type is passed as template
        //   argument of 'get_deleter' and 0 otherwise.  Next, we would test
        //   that the comparison operators and functors do return the correct
        //   comparison results.  Finally, we would test the 'make_shared' and
        //   'allocate_shared' functions using the same test plan as in case 5.
        //
        // Testing:
        //   CONCERN: C++ 'bsl::shared_ptr' ISO CONFORMANCE
        //   void reset(const shared_ptr<OTHER>& source, TYPE *ptr)
        //   DELETER *get_deleter(const shared_ptr<ELEMENT_TYPE>&)
        // --------------------------------------------------------------------

        if (verbose) printf(
                 "\nTESTING CONCERN: C++ 'bsl::shared_ptr' ISO CONFORMANCE"
                 "\n======================================================\n");

        if (verbose) printf("\nTesting 'reset(source, ptr)'."
                            "\n-----------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new(ta) TObj(&numDeletes);
            Obj x(p,&ta, 0); const Obj& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(p == X.get());
            ASSERT(1 == X.use_count());

            bsl::shared_ptr<double> y; const bsl::shared_ptr<double>& Y=y;
            double dummy;

            y.reset(X, &dummy);
            ASSERT(&dummy == Y.get());
            ASSERT(2 == X.use_count());
            ASSERT(2 == Y.use_count());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);

            int dummy2;
            bsl::shared_ptr<int> z;
            const bsl::shared_ptr<int>& Z=z;
            z.loadAlias(Y, &dummy2);
            ASSERT(&dummy2 == Z.get());
            ASSERT(3 == Z.use_count());
            ASSERT(3 == Y.use_count());
            ASSERT(&dummy == Y.get());
            ASSERT(3 == X.use_count());
            ASSERT(p == X.get());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);

            ASSERT(0 == bsl::get_deleter<void(*)(void)>(x));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
        ASSERT(1 == numDeletes);
        if (veryVerbose) {
            T_ P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING ALIAS OPERATIONS
        //
        //
        // Plan: TBD
        //
        // Testing:
        //   void loadAlias(const bsl::shared_ptr<ANY_TYPE>&, ELEMENT_TYPE *)
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ALIAS OPERATIONS"
                            "\n========================\n");

        if (verbose) printf("\nTesting 'loadAlias' (unset target)"
                            "\n----------------------------------\n");

//        bslma::TestAllocator ta(veryVeryVerbose);

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new(ta) TObj(&numDeletes);
            Obj x(p, &ta, 0); const Obj& X=x;

            numAllocations = ta.numAllocations();
            ASSERT(0 == numDeletes);
            ASSERT(p == X.get());
            ASSERT(1 == X.use_count());

            bsl::shared_ptr<double> y; const bsl::shared_ptr<double>& Y=y;

            double dummy;
            y.loadAlias(X, &dummy);
            ASSERT(&dummy == Y.get());
            ASSERT(2 == Y.use_count());
            ASSERT(0 == numDeletes);
            ASSERT(p == X.get());
            ASSERT(2 == X.use_count());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'loadAlias' (unset target and src)"
                            "\n------------------------------------------\n");
        {
            bsl::shared_ptr<MyTestObject2> x;
            const bsl::shared_ptr<MyTestObject2>& X=x;

            ASSERT(0 == X.get());
            ASSERT(0 == X.use_count());

            bsl::shared_ptr<double> y; const bsl::shared_ptr<double>& Y=y;

            double dummy;
            y.loadAlias(X, &dummy);

            ASSERT(&dummy == Y.get());
            ASSERT(0 == Y.use_count());
        }

        if (verbose) printf("\nTesting 'loadAlias' (partially unset)"
                            "\n-------------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new(ta) TObj(&numDeletes);
            Obj x(p, &ta, 0); const Obj& X=x;

            numAllocations = ta.numAllocations();
            ASSERT(0 == numDeletes);
            ASSERT(p == X.get());
            ASSERT(1 == X.use_count());

            double dummy;
            bsl::shared_ptr<double> y(X, &dummy);
            const bsl::shared_ptr<double>& Y=y;
            ASSERT(2 == X.use_count());
            ASSERT(2 == Y.use_count());

            y.loadAlias(X, static_cast<double *>(0));
            ASSERT(0 == Y.get());
            ASSERT(2 == Y.use_count());
            ASSERT(0 == numDeletes);
            ASSERT(p == X.get());
            ASSERT(2 == X.use_count());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'loadAlias'(set)"
                            "\n------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new(ta) TObj(&numDeletes);
            Obj x(p,&ta, 0); const Obj& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(p == X.get());
            ASSERT(1 == X.use_count());

            bsl::shared_ptr<double> y; const bsl::shared_ptr<double>& Y=y;
            double dummy;

            y.loadAlias(X, &dummy);
            ASSERT(&dummy == Y.get());
            ASSERT(2 == X.use_count());
            ASSERT(2 == Y.use_count());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);

            int dummy2;
            bsl::shared_ptr<int> z;
            const bsl::shared_ptr<int>& Z=z;
            z.loadAlias(Y, &dummy2);
            ASSERT(&dummy2 == Z.get());
            ASSERT(3 == Z.use_count());
            ASSERT(3 == Y.use_count());
            ASSERT(&dummy == Y.get());
            ASSERT(3 == X.use_count());
            ASSERT(p == X.get());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);

        }
        ASSERT(++numDeallocations == ta.numDeallocations());
        ASSERT(1 == numDeletes);

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING CONVERSION TO 'bslma::ManagedPtr'
        //
        // Plan: TBD
        //
        // Testing:
        //   bslma::ManagedPtr<TYPE> managedPtr() const
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING CONVERSION TO 'bslma::ManagedPtr'"
                   "\n=========================================\n");

        bslma::ManagedPtr<MyPDTestObject> mp;
        bsl::shared_ptr<MyPDTestObject>  sp(mp);

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new(ta) TObj(&numDeletes);

            Obj x(p, &ta, 0); const Obj& X=x;
            numAllocations = ta.numAllocations();

            if (veryVerbose) {
                T_ T_ P_(numDeletes); P(X.use_count());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.get());
            ASSERT(1 == X.use_count());

            {
                bslma::ManagedPtr<TObj> y(X.managedPtr());
                const bslma::ManagedPtr<TObj>& Y=y;

                ASSERT(0 == numDeletes);
                ASSERT(p == X.get());
                ASSERT(2 == X.use_count());
                ASSERT(p == Y.ptr());
                ASSERT(numAllocations == ta.numAllocations());
                ASSERT(numDeallocations == ta.numDeallocations());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.get());
            ASSERT(1 == X.use_count());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT(++numDeallocations == ta.numDeallocations());

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new(ta) TObj(&numDeletes);

            Obj x(p, &ta, 0); const Obj& X=x;
            numAllocations = ta.numAllocations();

            if (veryVerbose) {
                T_ T_ P_(numDeletes); P(X.use_count());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.get());
            ASSERT(1 == X.use_count());

            bslma::ManagedPtr<TObj> y(X.managedPtr());
            const bslma::ManagedPtr<TObj>& Y=y;

            ASSERT(0 == numDeletes);
            ASSERT(p == X.get());
            ASSERT(2 == X.use_count());
            ASSERT(p == Y.ptr());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());

            x.reset(static_cast<TObj *>(0));

            ASSERT(0 == numDeletes);
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT(numAllocations == ta.numAllocations());
        ASSERT(++numDeallocations == ta.numDeallocations());

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING ALIASING CONSTRUCTOR
        //   Test that the aliasing constructor work as expected
        //
        // Plan: TBD
        //
        // Testing:
        //   shared_ptr(const shared_ptr<ANY_TYPE>&, ELEMENT_TYPE *)
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING ALIASING CONSTRUCTOR"
                   "\n============================\n");

        if (verbose) printf("\nTesting \"alias\" constructor"
                            "\n---------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            MyTestDerivedObject *p = new(ta) MyTestDerivedObject(&numDeletes);
            Obj x(p,&ta, 0); const Obj& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(p == X.get());
            ASSERT(1 == X.use_count());

            double dummy;
            bsl::shared_ptr<double> y(X, &dummy);
            const bsl::shared_ptr<double>& Y=y;

            ASSERT(&dummy == Y.get());
            ASSERT(2 == X.use_count());
            ASSERT(2 == Y.use_count());
            ASSERT(numAllocations   == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
        ASSERT(1 == numDeletes);

        if (verbose) printf("\nTesting \"alias\" constructor (nil object)"
                            "\n----------------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            MyTestDerivedObject *p = new(ta) MyTestDerivedObject(&numDeletes);
            Obj x(p,&ta, 0); const Obj& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(p == X.get());
            ASSERT(1 == X.use_count());

            bsl::shared_ptr<double> y(X, 0);
            const bsl::shared_ptr<double>& Y = y;

            ASSERT(0 == Y.get());
            ASSERT(2 == X.use_count());
            ASSERT(2 == Y.use_count());
            ASSERT(numAllocations   == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
        ASSERT(1 == numDeletes);

        if (verbose) printf("\nTesting \"alias\" constructor (unset target)"
                            "\n------------------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            Obj x; const Obj& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(0 == X.get());
            ASSERT(0 == X.use_count());

            double dummy;
            bsl::shared_ptr<double> y(X, &dummy);
            const bsl::shared_ptr<double>& Y = y;

            ASSERT(&dummy == Y.get());
            ASSERT(0 == X.use_count());
            ASSERT(0 == Y.use_count());
            ASSERT(numAllocations   == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);
        }
        ASSERT(numDeallocations == ta.numDeallocations());

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            Obj x(new (ta) TObj(&numDeletes), &ta, 0); const Obj& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(0 != X.get());
            ASSERT(1 == X.use_count());

            bsl::shared_ptr<double> y(X, static_cast<double *>(0));
            const bsl::shared_ptr<double>& Y=y;

            ASSERT(0 == Y.get());
            ASSERT(2 == X.use_count());
            ASSERT(2 == Y.use_count());
            ASSERT(numAllocations   == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());


        if (verbose) printf("\nTesting global 'swap' function."
                            "\n-------------------------------\n");

        bsls::Types::Int64 numDeletes1 = 0;
        numDeletes = 0;
        {
            MyTestObject *p1 = new MyTestObject(&numDeletes);
            MyTestObject *p2 = new MyTestObject(&numDeletes1);
            Obj x(p1); const Obj &X = x;

            ASSERT(p1 == X.get());
            ASSERT(1 == X.use_count());

            ASSERT(0 == numDeletes);
            ASSERT(0 == numDeletes1);
            {
                Obj y(p2); const Obj &Y = y;
                ASSERT(p2 == Y.get());
                ASSERT(1 == Y.use_count());
                ASSERT(0 == numDeletes);
                ASSERT(0 == numDeletes1);

                swap(x, y);

                ASSERT(p2 == X.get());
                ASSERT(p1 == Y.get());
                ASSERT(0 == numDeletes);
                ASSERT(0 == numDeletes1);
                ASSERT(1 == X.use_count());
                ASSERT(1 == Y.use_count());
            }
            ASSERT(p2 == X.get());
            ASSERT(1 == X.use_count());
            ASSERT(1 == numDeletes);
            ASSERT(0 == numDeletes1);
        }
        ASSERT(1 == numDeletes);
        ASSERT(1 == numDeletes1);
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'createInplaceUninitializedBuffer'
        //   Test that 'createInplaceUninitializedBuffer' creates a buffer of
        //   the specified size, properly aligned, and that the buffer is
        //   deallocated properly.
        //
        // Plan:  For every size between 1 and 5 times the maximal alignment,
        //   create a shared pointer to a buffer of this size using
        //   'createInplaceUninitializedBuffer', and verify that the returned
        //   pointer is at least naturally aligned, that we can write into that
        //   buffer, and that the allocated buffer has at least the requested
        //   size.
        //
        // Testing:
        //   bsl::shared_ptr<char> createInplaceUninitializedBuffer(...)
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'createInplaceUninitializedBuffer'"
                            "\n==========================================\n");

        static const char EXP[] = "createInplaceUninitializedBuffer";

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();

        if (verbose) {
            printf("\nSizes of InplaceRepImpl<TYPE>:"
                   "\n------------------------------\n");

            enum {
                ONE_ALIGN = 1,
                TWO_ALIGN = (bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT > 2)
                                     ? 2
                                     : bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT,
                FOUR_ALIGN = (bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT > 4)
                                     ? 4
                                     : bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT,
                EIGHT_ALIGN = (bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT > 8)
                                     ? 8
                                     : bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT
            };

            P(sizeof(bslma::SharedPtrInplaceRep<
                                bsls::AlignmentToType<ONE_ALIGN>::Type[1] >));
            P(sizeof(bslma::SharedPtrInplaceRep<
                                bsls::AlignmentToType<TWO_ALIGN>::Type[1] >));
            P(sizeof(bslma::SharedPtrInplaceRep<
                                bsls::AlignmentToType<ONE_ALIGN>::Type[3] >));
            P(sizeof(bslma::SharedPtrInplaceRep<
                                bsls::AlignmentToType<FOUR_ALIGN>::Type[1] >));
            P(sizeof(bslma::SharedPtrInplaceRep<
                                bsls::AlignmentToType<ONE_ALIGN>::Type[5] >));
            P(sizeof(bslma::SharedPtrInplaceRep<
                                bsls::AlignmentToType<TWO_ALIGN>::Type[3] >));
            P(sizeof(bslma::SharedPtrInplaceRep<
                                bsls::AlignmentToType<ONE_ALIGN>::Type[7] >));
            P(sizeof(bslma::SharedPtrInplaceRep<
                               bsls::AlignmentToType<EIGHT_ALIGN>::Type[1] >));
            P(sizeof(bslma::SharedPtrInplaceRep<
                                bsls::AlignmentToType<ONE_ALIGN>::Type[9] >));
            P(sizeof(bslma::SharedPtrInplaceRep<
                                bsls::AlignmentToType<TWO_ALIGN>::Type[5] >));
            P(sizeof(bslma::SharedPtrInplaceRep<
                                bsls::AlignmentToType<ONE_ALIGN>::Type[11] >));
            P(sizeof(bslma::SharedPtrInplaceRep<
                                bsls::AlignmentToType<FOUR_ALIGN>::Type[3] >));
            P(sizeof(bslma::SharedPtrInplaceRep<
                                bsls::AlignmentToType<ONE_ALIGN>::Type[13] >));
            P(sizeof(bslma::SharedPtrInplaceRep<
                                bsls::AlignmentToType<TWO_ALIGN>::Type[7] >));
            P(sizeof(bslma::SharedPtrInplaceRep<
                                bsls::AlignmentToType<ONE_ALIGN>::Type[15] >));
        }

        if (verbose)
            printf("\nTesting 'createInplaceUninitializedBuffer'."
                   "\n-------------------------------------------\n");

        for (int size = 1;
             size < 5 * bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;
             ++size) {
            {
                bsl::shared_ptr<char> x;
                const bsl::shared_ptr<char>& X = x;

                x = bslstl::SharedPtrUtil::createInplaceUninitializedBuffer(
                                                                    size, &ta);
                strncpy(X.get(), EXP, size);

                static const size_t ALLOC_SIZE =
                    sizeof(bslma::SharedPtrInplaceRep<char>) + size - 1;
                LOOP_ASSERT(size, ++numAllocations == ta.numAllocations());
                LOOP_ASSERT(size, ALLOC_SIZE <= ta.lastAllocatedNumBytes());

                LOOP_ASSERT(size, X);
                LOOP_ASSERT(size, 0 == strncmp(EXP, X.get(), size));

                int alignment =
                         bsls::AlignmentUtil::calculateAlignmentFromSize(size);
                int alignmentOffset =
                                 bsls::AlignmentUtil::calculateAlignmentOffset(
                                                           X.get(), alignment);
                LOOP3_ASSERT(size, alignment, alignmentOffset,
                              0 == alignmentOffset);

                void* repAddr = ta.lastAllocatedAddress();
                bslma::TestAllocator::size_type repAllocSize =
                                                    ta.lastAllocatedNumBytes();
                LOOP4_ASSERT(repAddr,
                             repAllocSize,
                             static_cast<void *>(X.get()),
                             size,
                 static_cast<char*>(repAddr) + repAllocSize >= X.get() + size);

                if (veryVerbose) {
                    P_(size);
                    P_(ta.numAllocations());
                    P(ta.lastAllocatedNumBytes());
                    P_(alignment);
                    P((static_cast<void *>(X.get())));
                    P(bsls::AlignmentUtil::calculateAlignmentOffset(X.get(),
                                                               alignment));
                }
            }
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'SharedPtrUtil' CAST OPERATIONS
        //   Test that explicit cast operations properly loads the object
        //
        // Plan: TBD
        //
        // Testing:
        //  shared_ptr<TARGET> constCast(const shared_ptr<SOURCE>& source)
        //  shared_ptr<TARGET> dynamicCast(const shared_ptr<SOURCE>& source)
        //  shared_ptr<TARGET> staticCast(const shared_ptr<SOURCE>& source)
        //  void constCast(shared_ptr<TARGET> *, const shared_ptr<SOURCE>&)
        //  void dynamicCast(shared_ptr<TARGET> *, const shared_ptr<SOURCE>&)
        //  void staticCast(shared_ptr<TARGET> *, const shared_ptr<SOURCE>&)
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'SharedPtrUtil' CAST OPERATIONS"
                            "\n=======================================\n");

        using bslstl::SharedPtrUtil;

        if (verbose) printf(
                           "\nTest factories that return the cast by value"
                           "\n--------------------------------------------\n");

        if (verbose) printf("\nTesting 'dynamicCast'"
                            "\n---------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            MyTestDerivedObject *p = new(ta) MyTestDerivedObject(&numDeletes);
            Obj x(p, &ta, 0); const Obj& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(static_cast<MyTestObject*>(p) == X.get());
            ASSERT(1 == X.use_count());

            bsl::shared_ptr<MyTestObject2> y;
            const bsl::shared_ptr<MyTestObject2>& Y = y;

            {
                // This inner block necessary against Sun CC bug, the lifetime
                // of the temporary copied into y would otherwise pollute the
                // Y.use_count below.
                y = SharedPtrUtil::dynamicCast<MyTestObject2>(X);
            }
            if (veryVerbose) {
                P_(Y.get());
                P_(X.use_count());
                P(Y.use_count());
            }
            ASSERT(p == Y.get());
            ASSERT(2 == X.use_count());
            ASSERT(2 == Y.use_count());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
        ASSERT(1 == numDeletes);

        if (verbose) printf(
                           "\nTesting 'dynamicCast' that fails"
                           "\n--------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            MyTestDerivedObject *p = new(ta) MyTestDerivedObject(&numDeletes);
            Obj x(p, &ta, 0); const Obj& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(static_cast<MyTestObject*>(p) == X.get());
            ASSERT(1 == X.use_count());

            {
                // Cast to any other type with a vtable.  We know that
                // 'bsl::bad_weak_ptr' has a virtual destructor, and will be
                // tested as part of this component.

                bsl::shared_ptr<bsl::bad_weak_ptr> y;
                const bsl::shared_ptr<bsl::bad_weak_ptr>& Y = y;

                {
                    // This inner block necessary against Sun CC bug, the
                    // lifetime of the temporary copied into y would otherwise
                    // pollute the Y.use_count below.
                    y = SharedPtrUtil::dynamicCast<bsl::bad_weak_ptr>(X);
                }
                if (veryVerbose) {
                    P_(Y.get());
                    P_(X.use_count());
                    P(Y.use_count());
                }
                ASSERT(0 == Y.get());
                ASSERT(1 == X.use_count());
                ASSERT(0 == Y.use_count());
                ASSERT(numAllocations == ta.numAllocations());
                ASSERT(numDeallocations == ta.numDeallocations());
                ASSERT(0 == numDeletes);
            }
            ASSERT(1 == X.use_count());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
        ASSERT(1 == numDeletes);

        if (verbose) printf(
                           "\nTesting 'dynamicCast' aliasing null"
                           "\n-----------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            MyTestDerivedObject *p = new(ta) MyTestDerivedObject(&numDeletes);
            Obj z(p, &ta, 0); const Obj& Z = z;
            Obj x(Z, 0); const Obj& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(0 == X.get());
            ASSERT(2 == X.use_count());

            {
                // Cast to any other type with a vtable.  We know that
                // 'bsl::bad_weak_ptr' has a virtual destructor, and will be
                // tested as part of this component.

                bsl::shared_ptr<MyTestDerivedObject> y;
                const bsl::shared_ptr<MyTestDerivedObject>& Y = y;

                {
                    // This inner block necessary against Sun CC bug, the
                    // lifetime of the temporary copied into y would otherwise
                    // pollute the Y.use_count below.
                    y = SharedPtrUtil::dynamicCast<MyTestDerivedObject>(X);
                }
                if (veryVerbose) {
                    P_(Y.get());
                    P_(X.use_count());
                    P(Y.use_count());
                }
                ASSERT(0 == Y.get());
                ASSERT(2 == X.use_count());
                ASSERT(0 == Y.use_count());
                ASSERT(numAllocations == ta.numAllocations());
                ASSERT(numDeallocations == ta.numDeallocations());
                ASSERT(0 == numDeletes);
            }
            ASSERT(2 == X.use_count());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
        ASSERT(1 == numDeletes);

        if (verbose) printf(
                 "\nTesting 'dynamicCast' aliasing empty non-null"
                 "\n---------------------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            MyTestDerivedObject D(&numDeletes); MyTestDerivedObject *p = &D;
            Obj x(Obj(), p); const Obj& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(static_cast<MyTestObject *>(p) == X.get());
            ASSERT(0 == X.use_count());

            bsl::shared_ptr<MyTestDerivedObject> y;
            const bsl::shared_ptr<MyTestDerivedObject>& Y = y;

            {
                // This inner block necessary against Sun CC bug, the lifetime
                // of the temporary copied into y would otherwise pollute the
                // Y.use_count below.
                y = SharedPtrUtil::dynamicCast<MyTestDerivedObject>(X);
            }
            if (veryVerbose) {
                P_(Y.get());
                P_(X.use_count());
                P(Y.use_count());
            }
            ASSERT(p == Y.get());
            ASSERT(0 == X.use_count());
            ASSERT(0 == Y.use_count());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);
        }
        ASSERT(numDeallocations == ta.numDeallocations());
        ASSERT(1 == numDeletes);

        if (verbose) printf("\nTesting 'staticCast'"
                            "\n--------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            MyTestDerivedObject *p = new(ta) MyTestDerivedObject(&numDeletes);
            bsl::shared_ptr<MyTestDerivedObject> x(p,&ta, 0);
            const bsl::shared_ptr<MyTestDerivedObject>& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(p == X.get());
            ASSERT(1 == X.use_count());

            Obj y(SharedPtrUtil::staticCast<TObj>(X)); const Obj& Y=y;

            ASSERT(static_cast<MyTestObject*>(p) == Y.get());
            ASSERT(2 == X.use_count());
            ASSERT(2 == Y.use_count());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
        ASSERT(1 == numDeletes);

        if (verbose) printf("\nTesting 'constCast'"
                            "\n-------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            // Construct 'ConstObj' with a nil deleter.  This exposes a former
            // const-safety bug.
            bsls::Types::Int64 counter = 0;
            const MyTestObject V(&counter);
            ConstObj x(&V, bslstl::SharedPtrNilDeleter(), 0);
        }
        {
            numDeletes = 0;
            TObj *p = new(ta) TObj(&numDeletes);
            ConstObj x(p,&ta, 0); const ConstObj& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(p == X.get());
            ASSERT(1 == X.use_count());

            Obj y(SharedPtrUtil::constCast<TObj>(X)); const Obj& Y=y;

            ASSERT(const_cast<TObj*>(p) == Y.get());
            ASSERT(2 == X.use_count());
            ASSERT(2 == Y.use_count());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
        ASSERT(1 == numDeletes);

        // Repeat the tests using 'bslstl::SharedPtrUtil' and out-params

        if (verbose) printf(
                  "\nRepeat tests for casts returning through an out-param"
                  "\n-----------------------------------------------------\n");

        if (verbose) printf("\nTesting 'dynamicCast'"
                            "\n---------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            MyTestDerivedObject *p = new(ta) MyTestDerivedObject(&numDeletes);
            Obj x(p, &ta, 0); const Obj& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(static_cast<MyTestObject*>(p) == X.get());
            ASSERT(1 == X.use_count());

            bsl::shared_ptr<MyTestObject2> y;
            const bsl::shared_ptr<MyTestObject2>& Y = y;

            SharedPtrUtil::dynamicCast<MyTestObject2>(&y, X);

            if (veryVerbose) {
                P_(Y.get());
                P_(X.use_count());
                P(Y.use_count());
            }
            ASSERT(p == Y.get());
            ASSERT(2 == X.use_count());
            ASSERT(2 == Y.use_count());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
        ASSERT(1 == numDeletes);

        if (verbose) printf(
                           "\nTesting 'dynamicCast' that fails"
                           "\n--------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            MyTestDerivedObject *p = new(ta) MyTestDerivedObject(&numDeletes);
            Obj x(p, &ta, 0); const Obj& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(static_cast<MyTestObject*>(p) == X.get());
            ASSERT(1 == X.use_count());

            {
                // Cast to any other type with a vtable.  We know that
                // 'bsl::bad_weak_ptr' has a virtual destructor, and will be
                // tested as part of this component.

                bsl::shared_ptr<bsl::bad_weak_ptr> y;
                const bsl::shared_ptr<bsl::bad_weak_ptr>& Y = y;

                SharedPtrUtil::dynamicCast<bsl::bad_weak_ptr>(&y, X);

                if (veryVerbose) {
                    P_(Y.get());
                    P_(X.use_count());
                    P(Y.use_count());
                }
                ASSERT(0 == Y.get());
                ASSERT(1 == X.use_count());
                ASSERT(0 == Y.use_count());
                ASSERT(numAllocations == ta.numAllocations());
                ASSERT(numDeallocations == ta.numDeallocations());
                ASSERT(0 == numDeletes);
            }
            ASSERT(1 == X.use_count());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
        ASSERT(1 == numDeletes);

        if (verbose) printf(
                           "\nTesting 'dynamicCast' aliasing null"
                           "\n-----------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            MyTestDerivedObject *p = new(ta) MyTestDerivedObject(&numDeletes);
            Obj z(p, &ta, 0); const Obj& Z = z;
            Obj x(Z, 0); const Obj& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(0 == X.get());
            ASSERT(2 == X.use_count());

            {
                // Cast to any other type with a vtable.  We know that
                // 'bsl::bad_weak_ptr' has a virtual destructor, and will be
                // tested as part of this component.

                bsl::shared_ptr<MyTestDerivedObject> y;
                const bsl::shared_ptr<MyTestDerivedObject>& Y = y;

                SharedPtrUtil::dynamicCast<MyTestDerivedObject>(&y, X);

                if (veryVerbose) {
                    P_(Y.get());
                    P_(X.use_count());
                    P(Y.use_count());
                }
                ASSERT(0 == Y.get());
                ASSERT(2 == X.use_count());
                ASSERT(0 == Y.use_count());
                ASSERT(numAllocations == ta.numAllocations());
                ASSERT(numDeallocations == ta.numDeallocations());
                ASSERT(0 == numDeletes);
            }
            ASSERT(2 == X.use_count());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
        ASSERT(1 == numDeletes);

        if (verbose) printf(
                 "\nTesting 'dynamicCast' aliasing empty non-null"
                 "\n---------------------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            MyTestDerivedObject D(&numDeletes); MyTestDerivedObject *p = &D;
            Obj x(Obj(), p); const Obj& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(static_cast<MyTestObject *>(p) == X.get());
            ASSERT(0 == X.use_count());

            bsl::shared_ptr<MyTestDerivedObject> y;
            const bsl::shared_ptr<MyTestDerivedObject>& Y = y;

            SharedPtrUtil::dynamicCast<MyTestDerivedObject>(&y, X);

            if (veryVerbose) {
                P_(Y.get());
                P_(X.use_count());
                P(Y.use_count());
            }
            ASSERT(p == Y.get());
            ASSERT(0 == X.use_count());
            ASSERT(0 == Y.use_count());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);
        }
        ASSERT(numDeallocations == ta.numDeallocations());
        ASSERT(1 == numDeletes);

        if (verbose) printf("\nTesting 'staticCast'"
                            "\n--------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            MyTestDerivedObject *p = new(ta) MyTestDerivedObject(&numDeletes);
            bsl::shared_ptr<MyTestDerivedObject> x(p,&ta, 0);
            const bsl::shared_ptr<MyTestDerivedObject>& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(p == X.get());
            ASSERT(1 == X.use_count());

            Obj y; const Obj& Y=y;
            SharedPtrUtil::staticCast<TObj>(&y, X);

            ASSERT(static_cast<MyTestObject*>(p) == Y.get());
            ASSERT(2 == X.use_count());
            ASSERT(2 == Y.use_count());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
        ASSERT(1 == numDeletes);

        if (verbose) printf("\nTesting 'constCast'"
                            "\n-------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            // Construct 'ConstObj' with a nil deleter.  This exposes a former
            // const-safety bug.
            bsls::Types::Int64 counter = 0;
            const MyTestObject V(&counter);
            ConstObj x(&V, bslstl::SharedPtrNilDeleter(), 0);
        }
        {
            numDeletes = 0;
            TObj *p = new(ta) TObj(&numDeletes);
            ConstObj x(p,&ta, 0); const ConstObj& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(p == X.get());
            ASSERT(1 == X.use_count());

            Obj y; const Obj& Y=y;
            SharedPtrUtil::constCast<TObj>(&y, X);

            ASSERT(const_cast<TObj*>(p) == Y.get());
            ASSERT(2 == X.use_count());
            ASSERT(2 == Y.use_count());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
        ASSERT(1 == numDeletes);

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATORS
        //   Note that these tests assume that the default allocator is the
        //   NewDelete allocator; installing a test allocator for the default
        //   will break this test.
        //
        // Plan: TBD
        //
        // Testing:
        //   bsl::shared_ptr& operator=(const bsl::shared_ptr& rhs)
        //   bsl::shared_ptr& operator=(const bsl::shared_ptr<OTHER>& rhs)
        //   bsl::shared_ptr& operator=(std::auto_ptr<OTHER> rhs)
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING ASSIGNMENT OPERATORS"
                            "\n============================\n");

        if (verbose) printf("\nTesting ASSIGNMENT to empty object"
                            "\n----------------------------------\n");
        {
            Obj x1;
            const Obj& X1 = x1;
            ASSERT(0 == x1.get());
            ASSERT(0 == x1.use_count());

            numDeletes = 0;
            TObj *p = new TObj(&numDeletes);

            Obj x2(p); const Obj& X2=x2;

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
                P(X2.use_count());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X2.get());
            ASSERT(1 == X2.use_count());

            x1 = X2;

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
                P(X2.use_count());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X2.get());
            ASSERT(p == X1.get());

            ASSERT(2 == X2.use_count());
            ASSERT(2 == X1.use_count());
        }
        ASSERT(1 == numDeletes);

        if (verbose) printf("\nTesting ASSIGNMENT of auto_ptr"
                            "\n------------------------------\n");
        {
            Obj x;
            const Obj& X = x;
            ASSERT(0 == x.get());
            ASSERT(0 == x.use_count());

            numDeletes = 0;
            TObj *p = new TObj(&numDeletes);
            std::auto_ptr<TObj> ap(p);

            x = ap;

            if (veryVerbose) {
                P_(numDeletes);        P_(X.use_count());
                P(ap.get());
            }

            ASSERT(0 == ap.get());
            ASSERT(0 == numDeletes);
            ASSERT(p == X.get());
            ASSERT(1 == X.use_count());
        }
        ASSERT(1 == numDeletes);

        if (verbose) printf("\nTesting ASSIGNMENT of auto_ptr rvalue"
                            "\n-------------------------------------\n");
        {
            Obj x;
            const Obj& X = x;
            ASSERT(0 == x.get());
            ASSERT(0 == x.use_count());

            numDeletes = 0;

            x = makeAuto(&numDeletes);

            if (veryVerbose) {
                P_(numDeletes);        P_(X.use_count());
                P(X.get());
            }

            ASSERT(0 == numDeletes);
            ASSERT(0 != X.get());
            ASSERT(1 == X.use_count());
        }
        ASSERT(1 == numDeletes);

        if (verbose) printf("\nTesting ASSIGNMENT of empty object"
                            "\n----------------------------------\n");
        {
            numDeletes = 0;
            TObj *p = new TObj(&numDeletes);

            Obj x1(p); const Obj& X1=x1;

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X1.get());
            ASSERT(1 == X1.use_count());

            Obj x2;
            const Obj& X2 = x2;
            ASSERT(0 == x2.get());
            ASSERT(0 == x2.use_count());

            x1 = X2;

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
                P(X2.use_count());
            }
            ASSERT(1 == numDeletes);
            ASSERT(0 == X2.get());
            ASSERT(0 == X1.get());

            ASSERT(0 == X2.use_count());
            ASSERT(0 == X1.use_count());
        }
        ASSERT(1 == numDeletes);

        if (verbose) printf("\nTesting ASSIGNMENT of null pointer"
                            "\n----------------------------------\n");
        {
            numDeletes = 0;
            TObj *p = new TObj(&numDeletes);

            Obj x1(p); const Obj& X1=x1;

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X1.get());
            ASSERT(1 == X1.use_count());

            Obj x2;
            const Obj& X2 = x2;
            ASSERT(0 == x2.get());
            ASSERT(0 == x2.use_count());

            x1 = NULL;

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
                P(X2.use_count());
            }
            ASSERT(1 == numDeletes);
            ASSERT(0 == X2.get());
            ASSERT(0 == X1.get());

            ASSERT(0 == X2.use_count());
            ASSERT(0 == X1.use_count());
        }
        ASSERT(1 == numDeletes);

        if (verbose) printf("\nTesting ASSIGNMENT of loaded object"
                            "\n----------------------------------\n");
        {
            bsls::Types::Int64 numDeletes1 = 0;
            numDeletes = 0;
            TObj *p1 = new TObj(&numDeletes1);

            Obj x1(p1); const Obj& X1=x1;

            if (veryVerbose) {
                P_(numDeletes1); P_(X1.use_count());
            }
            ASSERT(0 == numDeletes1);
            ASSERT(p1 == X1.get());
            ASSERT(1 == X1.use_count());

            numDeletes = 0;
            TObj *p2 = new TObj(&numDeletes);
            Obj x2(p2);
            const Obj& X2 = x2;

            ASSERT(0 == numDeletes);
            ASSERT(p2 == x2.get());
            ASSERT(1 == x2.use_count());

            x1 = X2;

            if (veryVerbose) {
                P_(numDeletes1); P_(numDeletes); P_(X1.use_count());
                P(X2.use_count());
            }

            ASSERT(1 == numDeletes1);
            ASSERT(0 == numDeletes);
            ASSERT(p2 == X2.get());
            ASSERT(p2 == X1.get());

            ASSERT(2 == X2.use_count());
            ASSERT(2 == X1.use_count());
        }
        ASSERT(1 == numDeletes);

        if (verbose) printf("\nTesting ASSIGNMENT to self"
                            "\n--------------------------\n");
        {
            numDeletes = 0;
            TObj *p = new TObj(&numDeletes);

            Obj x1(p); const Obj& X1=x1;

            x1 = X1;

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X1.get());
            ASSERT(1 == X1.use_count());
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'swap'
        //
        // Plan: TBD
        //
        // Testing:
        //   void swap(shared_ptr& src)
        //   void swap(shared_ptr<ELEM_TYPE>& a, shared_ptr<ELEM_TYPE>& b)
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING 'swap'"
                            "\n==============\n");

        if (verbose) printf("\tWith default allocator.\n");

        bsls::Types::Int64 numDeletes1 = 0;
        numDeletes = 0;
        {
            MyTestObject *p1 = new MyTestObject(&numDeletes);
            MyTestObject *p2 = new MyTestObject(&numDeletes1);
            Obj x(p1); const Obj &X = x;

            ASSERT(p1 == X.get());
            ASSERT(1 == X.use_count());

            ASSERT(0 == numDeletes);
            ASSERT(0 == numDeletes1);
            {
                Obj y(p2); const Obj &Y = y;
                ASSERT(p2 == Y.get());
                ASSERT(1 == Y.use_count());
                ASSERT(0 == numDeletes);
                ASSERT(0 == numDeletes1);

                x.swap(y);

                ASSERT(p2 == X.get());
                ASSERT(p1 == Y.get());
                ASSERT(0 == numDeletes);
                ASSERT(0 == numDeletes1);
                ASSERT(1 == X.use_count());
                ASSERT(1 == Y.use_count());
            }
            ASSERT(p2 == X.get());
            ASSERT(1 == X.use_count());
            ASSERT(1 == numDeletes);
            ASSERT(0 == numDeletes1);
        }
        ASSERT(1 == numDeletes);
        ASSERT(1 == numDeletes1);

        if (verbose) printf("\tWith mix of allocators.\n");;

        bslma::TestAllocator da(veryVeryVerbose);
        bslma::DefaultAllocatorGuard defaultGuard(&da);

        // WARNING:  Installing a test allocator as the default means that
        //    bsl::shared_ptr<T> x(new T());
        // will break, as it tries to delete the object using the installed
        // default allocator (i.e., the test allocator) and not the new/delete
        // allocator.  Therefore, from then on in this test case, whenever
        // using the expression 'new T()', should be replaced by 'new(da) T()'.

        numDeletes1 = 0;
        numDeletes = 0;
        {
            MyTestObject *p1 = new(da) MyTestObject(&numDeletes);
            MyTestObject *p2 = new(ta) MyTestObject(&numDeletes1);
            Obj x(p1, &da); const Obj &X = x;

            ASSERT(p1 == X.get());
            ASSERT(1 == X.use_count());

            ASSERT(0 == numDeletes);
            ASSERT(0 == numDeletes1);
            {
                Obj y(p2, &ta); const Obj &Y = y;
                ASSERT(p2 == Y.get());
                ASSERT(1 == Y.use_count());
                ASSERT(0 == numDeletes);
                ASSERT(0 == numDeletes1);

                x.swap(y);

                ASSERT(p2 == X.get());
                ASSERT(p1 == Y.get());
                ASSERT(0 == numDeletes);
                ASSERT(0 == numDeletes1);
                ASSERT(1 == X.use_count());
                ASSERT(1 == Y.use_count());
            }
            ASSERT(p2 == X.get());
            ASSERT(1 == X.use_count());
            ASSERT(1 == numDeletes);
            ASSERT(0 == numDeletes1);
        }
        ASSERT(1 == numDeletes);
        ASSERT(1 == numDeletes1);

        if (verbose) printf("\tWith mix of deleters.\n");;

        numDeletes1 = 0;
        numDeletes = 0;
        {
            MyTestObject *p1 = new(da) MyTestObject(&numDeletes);
            MyTestObject *p2 = new(ta) MyTestObject(&numDeletes1);
            MyTestDeleter deleter(&ta);
            Obj x(p1, &da); const Obj &X = x;

            ASSERT(p1 == X.get());
            ASSERT(1 == X.use_count());

            ASSERT(0 == numDeletes);
            ASSERT(0 == numDeletes1);
            {
                Obj y(p2, deleter, &da); const Obj &Y = y;
                ASSERT(p2 == Y.get());
                ASSERT(1 == Y.use_count());
                ASSERT(0 == numDeletes);
                ASSERT(0 == numDeletes1);

                x.swap(y);

                ASSERT(p2 == X.get());
                ASSERT(p1 == Y.get());
                ASSERT(0 == numDeletes);
                ASSERT(0 == numDeletes1);
                ASSERT(1 == X.use_count());
                ASSERT(1 == Y.use_count());
            }
            ASSERT(p2 == X.get());
            ASSERT(1 == X.use_count());
            ASSERT(1 == numDeletes);
            ASSERT(0 == numDeletes1);
        }
        ASSERT(1 == numDeletes);
        ASSERT(1 == numDeletes1);

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR
        //
        // Plan: TBD
        //
        // Testing:
        //   shared_ptr(const shared_ptr& original)
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING COPY CONSTRUCTOR"
                            "\n========================\n");

        if (verbose) printf("\nThis test has not yet been implemented.\n");

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING RELATIONAL OPERATORS
        //   For an in-core value-semantic type, validate the (in)equality
        //   comparison operators before validating copy and assignment.  We
        //   take advantage of the regularity of testing these operators to
        //   include the ordered comparison operators in this same test case.
        //
        // Concerns:
        //: 1 Can compare two constant shared pointer objects using any
        //:   comparison operator.
        //:
        //: 2 Can compare two shared pointer objects pointing to different
        //:   target types.
        //:
        //: 3 Can correctly compare a shared pointer with a null pointer in
        //:   either order.
        //
        // Plan:
        //: 1 TBD
        //
        // Testing:
        //   bool operator==(const shared_ptr<LHS>&, const shared_ptr<RHS>&)
        //   bool operator==(const shared_ptr<LHS>&, bsl::nullptr_t)
        //   bool operator==(bsl::nullptr_t,         const shared_ptr<RHS>&)
        //   bool operator!=(const shared_ptr<LHS>&, const shared_ptr<RHS>&)
        //   bool operator!=(const shared_ptr<LHS>&, bsl::nullptr_t)
        //   bool operator!=(bsl::nullptr_t,         const shared_ptr<RHS>&)
        //   bool operator< (const shared_ptr<LHS>&, const shared_ptr<RHS>&)
        //   bool operator< (const shared_ptr<LHS>&, bsl::nullptr_t)
        //   bool operator< (bsl::nullptr_t,         const shared_ptr<RHS>&)
        //   bool operator<=(const shared_ptr<LHS>&, const shared_ptr<RHS>&)
        //   bool operator<=(const shared_ptr<LHS>&, bsl::nullptr_t)
        //   bool operator<=(bsl::nullptr_t,         const shared_ptr<RHS>&)
        //   bool operator>=(const shared_ptr<LHS>&, const shared_ptr<RHS>&)
        //   bool operator>=(const shared_ptr<LHS>&, bsl::nullptr_t)
        //   bool operator>=(bsl::nullptr_t,         const shared_ptr<RHS>&)
        //   bool operator> (const shared_ptr<LHS>&, const shared_ptr<RHS>&)
        //   bool operator> (const shared_ptr<LHS>&, bsl::nullptr_t)
        //   bool operator> (bsl::nullptr_t,         const shared_ptr<RHS>&)
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING RELATIONAL OPERATORS"
                            "\n============================\n");

        typedef bsl::shared_ptr<const int> IntPtr;
        typedef bsl::shared_ptr<void>      VoidPtr;

        bslstl::SharedPtrNilDeleter doNothing = {};

        int sampleArray[] = { 42, 13 };
        int *const pA = &sampleArray[0];
        int *const pB = &sampleArray[1];

        IntPtr  mX;   const IntPtr&  X = mX;
        VoidPtr mY;   const VoidPtr& Y = mY;

        mX.reset(pA, doNothing);
        mY.reset(pB, doNothing);

        ASSERT(  X == X  );
        ASSERT(!(X != X) );
        ASSERT(!(X <  X) );
        ASSERT(  X <= X  );
        ASSERT(  X >= X  );
        ASSERT(!(X >  X) );

        ASSERT(!(X == Y) );
        ASSERT(  X != Y  );
        ASSERT(  X <  Y  );
        ASSERT(  X <= Y  );
        ASSERT(!(X >= Y) );
        ASSERT(!(X >  Y) );

        ASSERT(!(Y == X) );
        ASSERT(  Y != X  );
        ASSERT(!(Y <  X) );
        ASSERT(!(Y <= X) );
        ASSERT(  Y >= X  );
        ASSERT(  Y >  X  );

        ASSERT(  Y == Y  );
        ASSERT(!(Y != Y) );
        ASSERT(!(Y <  Y) );
        ASSERT(  Y <= Y  );
        ASSERT(  Y >= Y  );
        ASSERT(!(Y >  Y) );

        const IntPtr Z;
        ASSERT(  Z == 0  );
        ASSERT(!(Z != 0) );
        ASSERT(!(Z <  0) );
        ASSERT(  Z <= 0  );
        ASSERT(  Z >= 0  );
        ASSERT(!(Z >  0) );

        ASSERT(  0 == Z  );
        ASSERT(!(0 != Z) );
        ASSERT(!(0 <  Z) );
        ASSERT(  0 <= Z  );
        ASSERT(  0 >= Z  );
        ASSERT(!(0 >  Z) );

        ASSERT(!(X == 0) );
        ASSERT(  X != 0  );
        ASSERT(!(X <  0) );
        ASSERT(!(X <= 0) );
        ASSERT(  X >= 0  );
        ASSERT(  X >  0  );

        ASSERT(!(0 == X) );
        ASSERT(  0 != X  );
        ASSERT(  0 <  X  );
        ASSERT(  0 <= X  );
        ASSERT(!(0 >= X) );
        ASSERT(!(0 >  X) );

        ASSERT(!(X == Z) );
        ASSERT(  X != Z  );
        ASSERT(!(X <  Z) );
        ASSERT(!(X <= Z) );
        ASSERT(  X >= Z  );
        ASSERT(  X >  Z  );

        ASSERT(!(Z == X) );
        ASSERT(  Z != X  );
        ASSERT(  Z <  X  );
        ASSERT(  Z <= X  );
        ASSERT(!(Z >= X) );
        ASSERT(!(Z >  X) );
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING PRINTING
        //
        // Plan: TBD
        //
        // Testing:
        //   ostream& operator<<(ostream&, const shared_ptr<TYPE>&)
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING PRINTING"
                            "\n================\n");

        if (verbose) printf("\nThis test has not yet been implemented.\n");

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //   This test basically copies the original testing from test case 2,
        //   which now omits the non-essential accessors, but is far from a
        //   thorough test.  There is no intent to handle aliased states at
        //   this stage of the test driver, although full support for deleters
        //   and allocators is expected.
        //
        // Plan: TBD
        //
        // Testing:
        //   add_lvalue_reference<ELEM_TYPE>::type operator[](ptrdiff_t) const
        //   add_lvalue_reference<ELEM_TYPE>::type operator*() const
        //   TYPE *operator->() const
        //   bslma::SharedPtrRep *rep() const
        //   TYPE *get() const
        //   bool unique() const
        //   long use_count() const
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING BASIC ACCESSORS"
                            "\n=======================\n");

        if (verbose) printf("\nTesting default constructor"
                            "\n---------------------------\n");
        {
            Obj x; const Obj& X=x;

            ASSERT(0 == X.get());
            ASSERT(0 == X.rep());
            ASSERT(0 == X.use_count());
            ASSERT(false == X.unique());
            ASSERT(false == static_cast<bool>(X));

            if (verbose) printf("\nNegative Testing.\n");
            {
                bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

                ASSERT_SAFE_FAIL(*x);
                ASSERT_SAFE_FAIL(x[0]);
            }
        }

        if (verbose) printf("\nTesting basic constructor"
                            "\n-------------------------\n");
        {
            numDeletes = 0;
            TObj *p = new TObj(&numDeletes);

            Obj x(p); const Obj& X=x;

            if (veryVerbose) {
                P_(numDeletes); P(X.use_count());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.get());
            ASSERT(0 != X.rep());
            ASSERT(1 == X.rep()->numReferences());
            ASSERT(static_cast<void *>(p) == X.rep()->originalPtr());
            ASSERT(1 == X.use_count());
            ASSERT(true == X.unique());
            ASSERT(X);
            ASSERT(p == X.operator->());
            ASSERT(p == &X.operator*());
            ASSERT(p == &X.operator[](0));
        }


        if (verbose) printf("\nTesting 'clear'"
                            "\n---------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new(ta) TObj(&numDeletes);

            Obj x(p, &ta); const Obj& X=x;
            numAllocations = ta.numAllocations();

            if (veryVerbose) {
                P_(numDeletes); P(X.use_count());
            }

            ASSERT(0 == numDeletes);
            ASSERT(p == X.get());
            ASSERT(0 != X.rep());
            ASSERT(1 == X.rep()->numReferences());
            ASSERT(static_cast<void *>(p) == X.rep()->originalPtr());
            ASSERT(1 == X.use_count());

            x.reset();

            numDeallocations += 2;
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(1 == numDeletes);
            ASSERT(0 == X.get());
            ASSERT(0 == X.rep());
            ASSERT(0 == X.use_count());
        }

        ASSERT(1 == numDeletes);
        ASSERT(numDeallocations == ta.numDeallocations());


      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTORS
        //   Verify that upon construction the object is properly initialized,
        //   and the reference count is updated.  Check that memory is
        //   deallocated upon object destruction.  Check that no memory is
        //   leaked in the presence of exceptions.
        //   Note that these tests assume that the default allocator is the
        //   NewDelete allocator; installing a test allocator for the default
        //   will break this test.
        //   Note that we do not appear to test construction from a pointer to
        //   some 'OTHER' type, nor the copy constructor, despite being listed
        //   here in the component test plan.
        //
        // Plan: TBD
        //
        // Testing:
        //   shared_ptr(TYPE *ptr) // synthesized
        //   shared_ptr(TYPE *ptr, bslma::Allocator *allocator)  // synthesized
        //   shared_ptr(OTHER *ptr)
        //   shared_ptr(OTHER *ptr, bslma::Allocator *basicAllocator)
        //   shared_ptr(ELEM_TYPE *ptr, bslma::SharedPtrRep *rep)
        //   shared_ptr(OTHER *ptr, DELETER *deleter)
        //   shared_ptr(OTHER *ptr, DELETER, bslma::Allocator* = 0)
        //   shared_ptr(OTHER *ptr, DELETER, ALLOCATOR, SFINAE)
        //   shared_ptr(nullptr_t)
        //   shared_ptr(nullptr_t, bslma::Allocator *)
        //   shared_ptr(nullptr_t, DELETER, bslma::Allocator * = 0)
        //   shared_ptr(nullptr_t, DELETER, ALLOCATOR, SFINAE)
        //   shared_ptr(std::auto_ptr<OTHER> autoPtr, bslma::Allocator*=0)
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING CONSTRUCTORS"
                            "\n====================\n");

        if (verbose) printf("\nTesting null pointer literal constructors"
                            "\n-----------------------------------------\n");

        numAllocations   = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            Obj v = NULL; const Obj& V = v;
            ASSERT(0 == V.get());
            ASSERT(0 == V.use_count());
            ASSERT(numAllocations == ta.numAllocations());

            Obj w(0); const Obj& W = w;
            ASSERT(0 == W.get());
            ASSERT(0 == W.use_count());
            ASSERT(numAllocations == ta.numAllocations());

            Obj x(0, &ta); const Obj& X = x;
            ASSERT(0 == X.get());
            ASSERT(0 == X.use_count());
            ASSERT(numAllocations == ta.numAllocations());

            Obj z(0, &ta, &ta); const Obj& Z = z;
            ASSERT(0 == Z.get());
            ASSERT(0 == Z.use_count());
            ASSERT(numAllocations == ta.numAllocations());
        }
        ASSERT(numDeallocations == ta.numDeallocations());


        if (verbose) printf("\nTesting (typed) null pointer constructors"
                            "\n-----------------------------------------\n");

        numDefaultDeallocations = defaultAllocator.numDeallocations();
        numDefaultAllocations   = defaultAllocator.numAllocations();
        numAllocations   = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            Obj w(static_cast<TObj *>(0));  // Rep with default allocator
            const Obj& W = w;
            ASSERT(0 == W.get());
            ASSERT(1 == W.use_count());

            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(++numDefaultAllocations ==
                                            defaultAllocator.numAllocations());

            Obj x(static_cast<TObj *>(0), &ta); const Obj& X = x;
            ASSERT(0 == X.get());
            ASSERT(1 == X.use_count());
            ASSERT(++numAllocations == ta.numAllocations());

            std::auto_ptr<TObj> apY(0);
            Obj y(apY, &ta); const Obj& Y = y;
            ASSERT(0 == Y.get());
            ASSERT(0 == Y.use_count());
            ASSERT(numAllocations == ta.numAllocations());

            Obj y2(makeAuto(), &ta); const Obj& Y2 = y2;
            (void) Y2;  // Suppress 'unused variable' warning
            ASSERT(0 == Y.get());
            ASSERT(0 == Y.use_count());
            ASSERT(numAllocations == ta.numAllocations());

            Obj z(static_cast<TObj *>(0), &ta, &ta); const Obj& Z = z;
            ASSERT(0 == Z.get());
            ASSERT(1 == Z.use_count());
            ASSERT(++numAllocations == ta.numAllocations());
        }

        ASSERT(2 + numDeallocations == ta.numDeallocations());
        ASSERT(++numDefaultDeallocations ==
                                          defaultAllocator.numDeallocations());


        if (verbose) printf("\nTesting basic constructor"
                            "\n-------------------------\n");
        {
            numDeletes = 0;
            TObj *p = new TObj(&numDeletes);

            Obj x(p); const Obj& X=x;

            if (veryVerbose) {
                P_(numDeletes); P(X.use_count());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.get());
            ASSERT(0 != X.rep());
            ASSERT(1 == X.rep()->numReferences());
            ASSERT(static_cast<void *>(p) == X.rep()->originalPtr());
            ASSERT(1 == X.use_count());
            ASSERT(true == X.unique());
        }


        if (verbose) printf("\nTesting constructor (with factory)"
                            "\n----------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new (ta) TObj(&numDeletes);
            numAllocations = ta.numAllocations();

            Obj x(p, &ta); const Obj& X = x;
            ASSERT(++numAllocations == ta.numAllocations());

            if (veryVerbose) {
                P_(numDeletes); P(X.use_count());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.get());
            ASSERT(1 == X.use_count());
        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT((numDeallocations+2) == ta.numDeallocations());


        if (verbose)
            printf("\nTesting auto_ptr constructor (with allocator)"
                   "\n---------------------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj  *p = new TObj(&numDeletes);
            std::auto_ptr<TObj> ap(p);

            numAllocations = ta.numAllocations();

            Obj x(ap, &ta); const Obj& X = x;
            ASSERT(0 == ap.get());
            ASSERT(++numAllocations == ta.numAllocations());

            if (veryVerbose) {
                P_(numDeletes); P(X.use_count());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.get());
            ASSERT(1 == X.use_count());
        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT((++numDeallocations) == ta.numDeallocations());


        if (verbose)
            printf("\nTesting auto_ptr rvalue constructor (with allocator)"
                   "\n----------------------------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            numAllocations = ta.numAllocations();

            Obj x(makeAuto(&numDeletes), &ta); const Obj& X = x;
            ASSERT(++numAllocations == ta.numAllocations());

            if (veryVerbose) {
                P_(numDeletes); P(X.use_count());
            }
            ASSERT(0 == numDeletes);
            ASSERT(0 != X.get());
            ASSERT(1 == X.use_count());
        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT((++numDeallocations) == ta.numDeallocations());

        // The auto_ptr constructors are explicit, so we do *not* test for
        // copy-initialization: 'Obj x = makeAuto(&numDeletes);'.  In fact it
        // appears to be impossible to support this syntax, even if we wanted
        // to, due to the language implying an extra user-defined conversion in
        // the chain compared to using the same technique that 'auto_ptr'
        // itself uses.

        if (verbose)
            printf("\nTesting constructor (with factory and allocator)"
                   "\n-----------------------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new (ta) TObj(&numDeletes);
            numAllocations = ta.numAllocations();

            // Note - not a great test when factor and allocator are the same.

            Obj x(p, &ta, &ta); const Obj& X = x;
            ASSERT(++numAllocations == ta.numAllocations());

            if (veryVerbose) {
                P_(numDeletes); P(X.use_count());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.get());
            ASSERT(1 == X.use_count());
        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT(numDeallocations + 2 == ta.numDeallocations());


        if (verbose)
            printf("\nTesting constructor (with deleter object)"
                   "\n-----------------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new (ta) TObj(&numDeletes);
            numAllocations = ta.numAllocations();

            MyTestDeleter deleter(&ta);
            Obj x(p, deleter);  const Obj& X = x;
            ASSERT(numAllocations == ta.numAllocations());

            if (veryVerbose) {
                P_(numDeletes); P(X.use_count());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.get());
            ASSERT(1 == X.use_count());
        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT(numDeallocations + 1 == ta.numDeallocations());


        if (verbose)
            printf("\nTesting constructor (with deleter function pointer)"
                   "\n---------------------------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj testObject(&numDeletes);
            numAllocations = ta.numAllocations();

            Obj x(&testObject, &TestDriver::doNotDelete<TObj>);
            const Obj& X = x;
            ASSERT(numAllocations == ta.numAllocations());

            if (veryVerbose) {
                P_(numDeletes); P(X.use_count());
            }
            ASSERT(0 == numDeletes);
            ASSERT(&testObject == X.get());
            ASSERT(1 == X.use_count());
        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT(numDeallocations == ta.numDeallocations());

#if !defined(BSLS_PLATFORM_CMP_IBM)
        if (verbose)
            printf("\nTesting constructor (with deleter function type)"
                   "\n------------------------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj testObject(&numDeletes);
            numAllocations = ta.numAllocations();

            Obj x(&testObject, TestDriver::doNotDelete<TObj>);
            const Obj& X = x;
            ASSERT(numAllocations == ta.numAllocations());

            if (veryVerbose) {
                P_(numDeletes); P(X.use_count());
            }
            ASSERT(0 == numDeletes);
            ASSERT(&testObject == X.get());
            ASSERT(1 == X.use_count());
        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT(numDeallocations == ta.numDeallocations());
#endif  // BSLS_PLATFORM_CMP_IBM

        if (verbose) printf(
                "\nTesting constructor (with deleter and bslma::allocator)"
                "\n-------------------------------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new (ta) TObj(&numDeletes);
            numAllocations = ta.numAllocations();

            MyTestDeleter deleter(&ta);
            bslma::Allocator *ba = &ta;
            Obj x(p, deleter, ba); const Obj& X = x;
            ASSERT(++numAllocations == ta.numAllocations());

            if (veryVerbose) {
                P_(numDeletes); P(X.use_count());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.get());
            ASSERT(1 == X.use_count());
        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT((numDeallocations+2) == ta.numDeallocations());


        if (verbose) printf(
               "\nTesting constructor (with deleter and derived allocator)"
               "\n--------------------------------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new (ta) TObj(&numDeletes);
            numAllocations = ta.numAllocations();

            MyTestDeleter deleter(&ta);
            Obj x(p, deleter, &ta); const Obj& X = x;
            ASSERT(++numAllocations == ta.numAllocations());

            if (veryVerbose) {
                P_(numDeletes); P(X.use_count());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.get());
            ASSERT(1 == X.use_count());
        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT((numDeallocations+2) == ta.numDeallocations());


        if (verbose) printf(
              "\nTesting constructor (with deleter and standard allocator)"
              "\n---------------------------------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new (ta) TObj(&numDeletes);
            numAllocations = ta.numAllocations();

            MyTestDeleter deleter(&ta);
            bsltf::StdStatefulAllocator<TObj, false, false, false, false>
                                                                    alloc(&ta);
            Obj x(p, deleter, alloc);  const Obj& X = x;
            ASSERT(++numAllocations == ta.numAllocations());

            if (veryVerbose) {
                P_(numDeletes); P(X.use_count());
            }
            ASSERTV(numDeletes, 0 == numDeletes);
            ASSERTV(p, X.get(), p == X.get());
            ASSERTV(X.use_count(), 1 == X.use_count());
        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }
        ASSERTV(numDeletes, 1 == numDeletes);
        ASSERTV((numDeallocations+2),   ta.numDeallocations(),
                (numDeallocations+2) == ta.numDeallocations());


        if (verbose) printf(
           "\nTesting constructor (with deleter and propagating allocator)"
           "\n------------------------------------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new (ta) TObj(&numDeletes);
            numAllocations = ta.numAllocations();

            MyTestDeleter deleter(&ta);
            bsltf::StdStatefulAllocator<TObj> alloc(&ta);
            Obj x(p, deleter, alloc);  const Obj& X = x;
            ASSERT(++numAllocations == ta.numAllocations());

            if (veryVerbose) {
                P_(numDeletes); P(X.use_count());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.get());
            ASSERT(1 == X.use_count());
        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT((numDeallocations+2) == ta.numDeallocations());


        if (verbose) printf(
               "\nTesting ctor (with function pointer and bslma allocator)"
               "\n--------------------------------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj testObject(&numDeletes);
            numAllocations = ta.numAllocations();

            bslma::Allocator *ba = &ta;
            Obj x(&testObject, &TestDriver::doNotDelete<TObj>, ba);
            const Obj& X = x;
            ASSERT(++numAllocations == ta.numAllocations());

            if (veryVerbose) {
                P_(numDeletes); P(X.use_count());
            }
            ASSERT(0 == numDeletes);
            ASSERT(&testObject == X.get());
            ASSERT(1 == X.use_count());
        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT(numDeallocations+1 == ta.numDeallocations());


        if (verbose) printf(
             "\nTesting ctor (with function pointer and derived allocator)"
             "\n----------------------------------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj testObject(&numDeletes);
            numAllocations = ta.numAllocations();

            Obj x(&testObject, &TestDriver::doNotDelete<TObj>, &ta);
            const Obj& X = x;
            ASSERT(++numAllocations == ta.numAllocations());

            if (veryVerbose) {
                P_(numDeletes); P(X.use_count());
            }
            ASSERT(0 == numDeletes);
            ASSERT(&testObject == X.get());
            ASSERT(1 == X.use_count());
        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT(numDeallocations+1 == ta.numDeallocations());


        if (verbose) printf(
            "\nTesting ctor (with function pointer and standard allocator)"
            "\n-----------------------------------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj testObject(&numDeletes);
            numAllocations = ta.numAllocations();

            bsltf::StdStatefulAllocator<TObj, false, false, false, false>
                                                                    alloc(&ta);
            Obj x(&testObject, &TestDriver::doNotDelete<TObj>, alloc);
            const Obj& X = x;
            ASSERT(++numAllocations == ta.numAllocations());

            if (veryVerbose) {
                P_(numDeletes); P(X.use_count());
            }
            ASSERT(0 == numDeletes);
            ASSERT(&testObject == X.get());
            ASSERT(1 == X.use_count());
        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT(numDeallocations+1 == ta.numDeallocations());


        if (verbose) printf(
         "\nTesting ctor (with function pointer and propagating allocator)"
         "\n--------------------------------------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj testObject(&numDeletes);
            numAllocations = ta.numAllocations();

            bsltf::StdStatefulAllocator<TObj> alloc(&ta);
            Obj x(&testObject, &TestDriver::doNotDelete<TObj>, alloc);
            const Obj& X = x;
            ASSERT(++numAllocations == ta.numAllocations());

            if (veryVerbose) {
                P_(numDeletes); P(X.use_count());
            }
            ASSERT(0 == numDeletes);
            ASSERT(&testObject == X.get());
            ASSERT(1 == X.use_count());
        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT(numDeallocations+1 == ta.numDeallocations());


#if !defined(BSLS_PLATFORM_CMP_IBM)
        if (verbose) printf(
                  "\nTesting ctor (with function type and bslma allocator)"
                  "\n-----------------------------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj testObject(&numDeletes);
            numAllocations = ta.numAllocations();

            bslma::Allocator *ba = &ta;
            Obj x(&testObject, TestDriver::doNotDelete<TObj>, ba);
            const Obj& X = x;
            ASSERT(++numAllocations == ta.numAllocations());

            if (veryVerbose) {
                P_(numDeletes); P(X.use_count());
            }
            ASSERT(0 == numDeletes);
            ASSERT(&testObject == X.get());
            ASSERT(1 == X.use_count());
        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT(numDeallocations+1 == ta.numDeallocations());


        if (verbose) printf(
                "\nTesting ctor (with function type and derived allocator)"
                "\n-------------------------------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj testObject(&numDeletes);
            numAllocations = ta.numAllocations();

            Obj x(&testObject, TestDriver::doNotDelete<TObj>, &ta);
            const Obj& X = x;
            ASSERT(++numAllocations == ta.numAllocations());

            if (veryVerbose) {
                P_(numDeletes); P(X.use_count());
            }
            ASSERT(0 == numDeletes);
            ASSERT(&testObject == X.get());
            ASSERT(1 == X.use_count());
        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT(numDeallocations+1 == ta.numDeallocations());


        if (verbose) printf(
               "\nTesting ctor (with function type and standard allocator)"
               "\n--------------------------------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj testObject(&numDeletes);
            numAllocations = ta.numAllocations();

            bsltf::StdStatefulAllocator<TObj, false, false, false, false>
                                                                    alloc(&ta);
            Obj x(&testObject, TestDriver::doNotDelete<TObj>, alloc);
            const Obj& X = x;
            ASSERT(++numAllocations == ta.numAllocations());

            if (veryVerbose) {
                P_(numDeletes); P(X.use_count());
            }
            ASSERT(0 == numDeletes);
            ASSERT(&testObject == X.get());
            ASSERT(1 == X.use_count());
        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT(numDeallocations+1 == ta.numDeallocations());


        if (verbose) printf(
            "\nTesting ctor (with function type and propagating allocator)"
            "\n-----------------------------------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj testObject(&numDeletes);
            numAllocations = ta.numAllocations();

            bsltf::StdStatefulAllocator<TObj> alloc(&ta);
            Obj x(&testObject, TestDriver::doNotDelete<TObj>, alloc);
            const Obj& X = x;
            ASSERT(++numAllocations == ta.numAllocations());

            if (veryVerbose) {
                P_(numDeletes); P(X.use_count());
            }
            ASSERT(0 == numDeletes);
            ASSERT(&testObject == X.get());
            ASSERT(1 == X.use_count());
        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT(numDeallocations+1 == ta.numDeallocations());

#endif  // ! BSLS_PLATFORM_CMP_IBM

        if (verbose) printf("\nTesting constructor (with rep)"
                            "\n------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new (ta) TObj(&numDeletes);
            numAllocations = ta.numAllocations();

            Obj x(p, &ta); const Obj& X = x;
            ASSERT(++numAllocations == ta.numAllocations());

            if (veryVerbose) {
                P(X.use_count());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.get());
            ASSERT(1 == X.use_count());

            bslma::SharedPtrRep *rep = x.rep();
            x.release();

            Obj xx(p, rep); const Obj& XX = xx;
            ASSERT(p == XX.get());
            ASSERT(rep ==  XX.rep());
            ASSERT(1 == XX.use_count());
        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }
        ASSERT((numDeallocations+2) == ta.numDeallocations());


      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING BASIC CONSTRUCTOR AND PRIMARY MANIPULATORS
        //   To bootstrap testing, verify that the default constructor creates
        //   an empty shared pointer object that owns nothing, and then using
        //   'reset' as the primary manipulator verify that a shared pointer
        //   can take ownership of objects that are pointer-convertible to the
        //   template parameter type, releasing ownership of any previously
        //   owned objects (from a previous 'reset' call).  Further test that
        //   'reset' can supply a deleter to be used to destroy the owned
        //   object when the final shared reference is destroyed, and an
        //   allocator to be used to create any dynamic storage required to
        //   hold the shared state.  The allocate may be a 'bslma::Allocator *'
        //   pointer (or a pointer to a derived implementation) or any object
        //   that satisfied the C++11 allocator requirements.
        //
        //   The state of the shared pointer object will be inspected using the
        //   primary accessors only, but these will not be deemed to be tested
        //   until test case 4, and taken on trust until then.
        //
        //   Note that while this test will verify that allocators and deleters
        //   are correctly used to allocate storage and destroy objects, there
        //   is no testing of shared-ownership behavior at this step as the
        //   copy constructor, which establishes shared ownership states, is
        //   not tested until test case 7.
        //
        //   Note that this test will offer basic but incomplete test coverage
        //   of the boolean conversion operator.
        //
        // Concerns
        //
        // Plan:
        //   Install a test allocator as the default allocator to track
        //   allocation of 'Rep' object.  Then, incrementally build confidence
        //   testing operations in the following order:
        //
        //: default constructor
        //
        //: reset to clear on an empty (default constructed) 'shared_ptr'
        //:
        //: reset with a pointer
        //:
        //: reset to clear
        //:
        //: reset pointer, and then reset with another to replace
        //:    TestAllocator as default to test exception safety.
        //:
        //: reset with derived / cv-qualified pointers
        //:
        //: reset with deleter
        //:    (deleter as functor)
        //:    (deleter as function pointer)
        //:    (deleter as function with decay)
        //:    (deleter as BDE factory-pointer)
        //:    test exception safety with throwing deleter copy
        //:
        //: reset with nullptr and deleter
        //:
        //: reset with deleter, followed by another reset
        //:    (second reset equivalent to clear  - no allocations)
        //:    (second reset equivalent new value/deleter, exception concerns)
        //:
        //: reset with deleter and BDE allocator
        //:    (deleter as functor)
        //:    (deleter as function pointer)
        //:    test exception safety via test allocator
        //:
        //: reset with deleter and std allocator
        //:    (deleter as functor)
        //:    (deleter as function pointer)
        //:    test exception safety via (wrapped) test allocator
        //:
        //: reset with nullptr, deleter and BDE allocator
        //:
        //: reset with nullptr, deleter and STD allocator
        //:
        //: reset with deleter and allocator, followed by another reset
        //:    (second reset equivalent to clear  - no allocations)
        //:    (second reset equivalent new value/deleter, exception concerns)
        //:    (second reset equivalent new value/deleter/allocator)
        //:
        //: Repeat the above for 'clear' and 'load'
        //
        // Testing:
        //   shared_ptr()
        //   ~shared_ptr()
        //   void reset()
        //   void reset(OTHER *ptr)
        //   void reset(OTHER *ptr, DELETER deleter)
        //   void reset(OTHER *ptr, DELETER deleter, ALLOCATOR basicAllocator)
        // --------------------------------------------------------------------

        if (verbose) printf(
                     "\nTESTING BASIC CONSTRUCTOR AND PRIMARY MANIPULATORS"
                     "\n==================================================\n");

        defaultAllocator.setAllocationLimit(0); // No allocations for default

        if (verbose) printf("\nTesting default constructor"
                            "\n---------------------------\n");
        {
            Obj x; const Obj& X=x;

            ASSERT(0 == X.get());
            ASSERT(0 == X.rep());
            ASSERT(0 == X.use_count());
        }

        if (verbose)
               printf("\nTesting 'reset' with a null ptr (on empty object)"
                      "\n-------------------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        numDefaultDeallocations = defaultAllocator.numDeallocations();
        numDefaultAllocations   = defaultAllocator.numAllocations();
        {
            Obj w; const Obj& W=w;
            w.reset();
            ASSERT(0 == W.get());
            ASSERT(0 == W.rep());
            ASSERT(0 == W.use_count());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(numDefaultAllocations == defaultAllocator.numAllocations());
            ASSERT(numDefaultDeallocations ==
                                          defaultAllocator.numDeallocations());

            // Note that while we could easily implement the next test to use
            // no allocated memory, 'reset((T*)0)', the ISO standard formally
            // places a post-condition that '1 == use_count()'.

            defaultAllocator.setAllocationLimit(1);

            Obj x; const Obj& X=x;
            x.reset(static_cast<TObj *>(0));
            ASSERT(0 == X.get());
            ASSERT(0 != X.rep());
            ASSERT(1 == X.use_count());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(++numDefaultAllocations ==
                                            defaultAllocator.numAllocations());
            ASSERT(numDefaultDeallocations ==
                                          defaultAllocator.numDeallocations());

            // With deleters/allocators, expect to perform one allocation to
            // store management info on the empty object.

            defaultAllocator.setAllocationLimit(1);

            Obj y; const Obj& Y=y;
            y.reset(static_cast<TObj *>(0), &ta);
            ASSERT(0 == Y.get());
            ASSERT(0 != Y.rep());
            ASSERT(1 == Y.use_count());
            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(numDefaultAllocations == defaultAllocator.numAllocations());
            ASSERT(numDefaultDeallocations ==
                                          defaultAllocator.numDeallocations());

            defaultAllocator.setAllocationLimit(1);

            Obj z; const Obj& Z=z;
            z.reset(static_cast<TObj *>(0), &ta, &ta);
            ASSERT(0 == Z.get());
            ASSERT(0 != Z.rep());
            ASSERT(1 == Z.use_count());
            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(numDefaultAllocations == defaultAllocator.numAllocations());
            ASSERT(numDefaultDeallocations ==
                                          defaultAllocator.numDeallocations());
        }
        ASSERT(numAllocations == ta.numAllocations());
        ASSERT(2+numDeallocations == ta.numDeallocations());
        ASSERT(numDefaultAllocations == defaultAllocator.numAllocations());
        ASSERT(++numDefaultDeallocations ==
                                          defaultAllocator.numDeallocations());


        defaultAllocator.setAllocationLimit(-1);

        if (verbose) printf("\nTesting reset(ptr)."
                            "\n-------------------\n");

        numDeletes = 0;
        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        numDefaultDeallocations = defaultAllocator.numDeallocations();
        numDefaultAllocations   = defaultAllocator.numAllocations();
        {
            TObj *p = new TObj(&numDeletes);

            Obj x; const Obj& X=x;

            x.reset(p);

            if (veryVerbose) {
                T_ P_(numDeletes) P(X.use_count())
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.get());
            ASSERT(0 != X.rep());
            ASSERT(1 == X.use_count());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(++numDefaultAllocations
                                         == defaultAllocator.numAllocations());
            ASSERT(numDefaultDeallocations ==
                                          defaultAllocator.numDeallocations());
        }
        if (veryVerbose) {
            T_ P(numDeletes);
        }
        ASSERT(1 == numDeletes);
        ASSERT(numAllocations == ta.numAllocations());
        ASSERT(numDeallocations == ta.numDeallocations());
        ASSERT(numDefaultAllocations == defaultAllocator.numAllocations());
        ASSERT(++numDefaultDeallocations ==
                                          defaultAllocator.numDeallocations());


        if (verbose) printf("\nTesting 'reset' back to 'empty'."
                            "\n--------------------------------\n");

        numDeletes = 0;
        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        numDefaultDeallocations = defaultAllocator.numDeallocations();
        numDefaultAllocations   = defaultAllocator.numAllocations();
        {
            Obj x; const Obj& X=x;

            TObj *p = new TObj(&numDeletes);
            x.reset(p);

            if (veryVerbose) {
                T_ P_(numDeletes) P(X.use_count())
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.get());
            ASSERT(0 != X.rep());
            ASSERT(1 == X.use_count());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(++numDefaultAllocations
                                         == defaultAllocator.numAllocations());
            ASSERT(numDefaultDeallocations ==
                                          defaultAllocator.numDeallocations());

            x.reset();  // This is the operation under test.

            ASSERT(1 == numDeletes);
            ASSERT(0 == X.get());
            ASSERT(0 == X.rep());
            ASSERT(0 == X.use_count());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(numDefaultAllocations == defaultAllocator.numAllocations());
            ASSERT(++numDefaultDeallocations ==
                                          defaultAllocator.numDeallocations());
        }
        if (veryVerbose) {
            T_ P(numDeletes);
        }
        ASSERT(1 == numDeletes);
        ASSERT(numAllocations == ta.numAllocations());
        ASSERT(numDeallocations == ta.numDeallocations());
        ASSERT(numDefaultAllocations == defaultAllocator.numAllocations());
        ASSERT(numDefaultDeallocations == defaultAllocator.numDeallocations());


        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new(ta) TObj(&numDeletes);

            Obj x(p, &ta); const Obj& X=x;

            numAllocations = ta.numAllocations();

            if (veryVerbose) {
                T_ P_(numDeletes) P(X.use_count())
            }

            ASSERT(0 == numDeletes);
            ASSERT(p == X.get());
            ASSERT(1 == X.use_count());

            x.reset();  // This is the operation under test.

            numDeallocations += 2;
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(1 == numDeletes);
            ASSERT(0 == X.get());
            ASSERT(0 == X.use_count());

            ASSERT(0 == bsl::get_deleter<void(*)(void)>(x));
        }
        ASSERT(1 == numDeletes);
        ASSERT(numDeallocations == ta.numDeallocations());
        if (veryVerbose) {
            T_ P_(numDeletes)
            P_(numDeallocations)
            P(ta.numDeallocations())
        }


        if (verbose) printf
                         ("\nTesting reset(ptr, deleter function pointer)."
                          "\n---------------------------------------------\n");

        // TBD


        if (verbose) printf("\nTesting reset(ptr, deleter functor)."
                            "\n------------------------------------\n");

        numDeletes = 0;
        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        numDefaultDeallocations = defaultAllocator.numDeallocations();
        numDefaultAllocations   = defaultAllocator.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            MyTestDeleter deleter(&ta);
            Obj x; const Obj& X=x;

            TObj *p = new(ta) TObj(&numDeletes);
            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(numDefaultAllocations == defaultAllocator.numAllocations());
            ASSERT(numDefaultDeallocations ==
                                          defaultAllocator.numDeallocations());

            x.reset(p, deleter);  // This is the operation under test.

            if (veryVerbose) {
                T_ P_(numDeletes) P(X.use_count())
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.get());
            ASSERT(0 != X.rep());
            ASSERT(1 == X.use_count());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(++numDefaultAllocations
                                         == defaultAllocator.numAllocations());
            ASSERT(numDefaultDeallocations ==
                                          defaultAllocator.numDeallocations());

            ASSERT(0 == bsl::get_deleter<void(*)(void)>(x));
            ASSERT(0 != bsl::get_deleter<MyTestDeleter>(x));
        }

        if (veryVerbose) {
            T_ P_(numDeletes) P_(numDeallocations) P(ta.numDeallocations())
        }
        ASSERT(1 == numDeletes);
        ASSERT(numAllocations == ta.numAllocations());
        ASSERT(++numDeallocations == ta.numDeallocations());
        ASSERT(numDefaultAllocations == defaultAllocator.numAllocations());
        ASSERT(++numDefaultDeallocations ==
                                          defaultAllocator.numDeallocations());


        if (verbose) printf("\nTesting reset(ptr, factory-deleter)."
                            "\n------------------------------------\n");

        // TBD


        if (verbose) printf("\nTesting reset(ptr, factory is allocator)."
                            "\n-----------------------------------------\n");

        numDeletes = 0;
        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        numDefaultDeallocations = defaultAllocator.numDeallocations();
        numDefaultAllocations   = defaultAllocator.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            Obj x; const Obj& X=x;

            TObj *p = new(ta) TObj(&numDeletes);
            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(numDefaultAllocations == defaultAllocator.numAllocations());
            ASSERT(numDefaultDeallocations ==
                                          defaultAllocator.numDeallocations());

            x.reset(p, &ta);  // This is the operation under test.

            if (veryVerbose) {
                T_ P_(numDeletes) P(X.use_count())
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.get());
            ASSERT(0 != X.rep());
            ASSERT(1 == X.use_count());
            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(numDefaultAllocations == defaultAllocator.numAllocations());
            ASSERT(numDefaultDeallocations ==
                                          defaultAllocator.numDeallocations());

            ASSERT(0 == bsl::get_deleter<void(*)(void)>(x));
            ASSERT(0 != bsl::get_deleter<bslma::Allocator *>(x));
        }
        if (veryVerbose) {
            T_ P_(numDeletes) P_(numDeallocations) P(ta.numDeallocations())
        }
        ASSERT(1 == numDeletes);
        ASSERT(numAllocations == ta.numAllocations());
        ASSERT((numDeallocations+=2) == ta.numDeallocations());
        ASSERT(numDefaultAllocations == defaultAllocator.numAllocations());
        ASSERT(numDefaultDeallocations == defaultAllocator.numDeallocations());


        if (verbose) printf(
           "\nTesting reset(ptr, deleter function pointer, std allocator)."
           "\n------------------------------------------------------------\n");

        // TBD


        if (verbose) printf(
                    "\nTesting reset(ptr, deleter functor, std allocator)."
                    "\n---------------------------------------------------\n");

        // TBD


        if (verbose) printf(
                    "\nTesting reset(ptr, factory-deleter, std allocator)."
                    "\n---------------------------------------------------\n");

        // TBD


        if (verbose) printf(
               "\nTesting reset(ptr, factory is allocator, std allocator)."
               "\n--------------------------------------------------------\n");

        // TBD


        if (verbose) printf(
           "\nTesting reset(ptr, deleter function pointer, BDE allocator)."
           "\n------------------------------------------------------------\n");

        // TBD


        if (verbose) printf(
                    "\nTesting reset(ptr, deleter functor, BDE allocator)."
                    "\n---------------------------------------------------\n");

        // TBD


        if (verbose) printf(
                    "\nTesting reset(ptr, factory-deleter, BDE allocator)."
                    "\n---------------------------------------------------\n");

        // TBD


        if (verbose) printf(
               "\nTesting reset(ptr, factory is allocator, BDE allocator)."
               "\n--------------------------------------------------------\n");

        // TBD


      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This test exercises basic functionality but tests nothing.
        //
        //   Note that these tests assume that the default allocator is the
        //   NewDelete allocator; installing a test allocator for the default
        //   will break this test.
        //
        // Testing:
        //   BREATHING TEST (shared_ptr)
        //   BREATHING TEST (weak_ptr)
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        if (verbose) printf("\nTesting shared_ptr"
                            "\n------------------\n");

        bsls::Types::Int64 numDeletes = 0;
        {
            MyTestObject *obj = new MyTestObject(&numDeletes);
            ASSERT(0 == numDeletes);

            Obj x(obj); const Obj& X=x;
            ASSERT(0 == numDeletes);
            ASSERT(obj == X.get());
            ASSERT(X);
        }
        ASSERT(1 == numDeletes);
        if (veryVerbose) { P(numDeletes); }

        numDeletes = 0;
        {
            MyTestObject *obj = new MyTestObject(&numDeletes);
            MyTestObjectFactory deleter;
            ASSERT(0 == numDeletes);

            Obj x(obj, &deleter, 0); const Obj& X = x;

            ASSERT(0 == numDeletes);
            ASSERT(obj == X.get());
            ASSERT(X);
        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
        {
            bslma::Allocator *da = bslma::Default::allocator();
            MyTestObject *obj = new(*da) MyTestObject(&numDeletes);
            MyTestDeleter deleter;
            ASSERT(0 == numDeletes);

            Obj x(obj, deleter, 0); const Obj& X = x;
            ASSERT(0 == numDeletes);
            ASSERT(obj == X.get());
            ASSERT(X);

        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
        {
            MyTestObject *obj = new MyTestObject(&numDeletes);

            Obj x1(obj); const Obj& X1=x1;
            Obj x2; const Obj& X2=x2;
            (void) X2;  // Suppress 'unused variable' warning
            ASSERT(0 == numDeletes);
            ASSERT(obj == X1.get());
            ASSERT(X1);
        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
        {
            ConstObj x1(new MyTestObject(&numDeletes));
            const ConstObj& X1=x1;

            Obj x2(bslstl::SharedPtrUtil::constCast<TObj>(x1));
            const Obj& X2 = x2;

            if (veryVeryVerbose) {
                P(numDeletes);
            }
            ASSERT(0 == numDeletes);
            ASSERT(X1.get() == X2.get());
            if (veryVeryVerbose) {
                P(numDeletes);
            }

        }
        if (veryVeryVerbose) {
            P(numDeletes);
        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
        {
            TObj obj(&numDeletes);

            Obj x1; const Obj &X1 = x1;

            ASSERT(0 == X1.get());
            x1.createInplace(0, obj);

            ASSERT(0 != X1.get());
            ASSERT(0 == numDeletes);
            ASSERT(0 == X1->copyCounter());
            ASSERT(&numDeletes == X1->deleteCounter());
        }
        ASSERT(2 == numDeletes);

        numDeletes = 0;
        {
            TObj *p = new TObj(&numDeletes);

            Obj x1( p,
                   &myTestDeleterFunction,
                    static_cast<bslma::Allocator *>(0));
            const Obj &X1 = x1;
            (void) X1;  // Suppress 'unused variable' warning.

        }
        ASSERT(1 == numDeletes);


        if (verbose) printf("\nTesting weak_ptr"
                            "\n----------------\n");

        numDeletes = 0;
        {
            MyTestObject *obj = new MyTestObject(&numDeletes);

            ObjSP mS(obj); const ObjSP& S = mS;
            ASSERT(1 == S.use_count());

            ObjWP mX(S); const ObjWP& X = mX;
            ASSERT(!X.expired());
            ASSERT(1 == X.use_count());

            ObjWP mY(X); const ObjWP& Y = mY;
            ASSERT(!Y.expired());
            ASSERT(1 == Y.use_count());

            ASSERT(1 == X.use_count());

            ObjWP mA; const ObjWP& A = mA;

            mA = S;
            ASSERT(!A.expired());
            ASSERT(1 == A.use_count());

            ObjWP mB; const ObjWP& B = mB;

            mB = X;
            ASSERT(!B.expired());
            ASSERT(1 == B.use_count());

            mA = Y;
            ASSERT(!A.expired());
            ASSERT(1 == A.use_count());

        }

        numDeletes = 0;
        {
            MyTestDerivedObject *obj = new MyTestDerivedObject(&numDeletes);

            DerivedSP mS(obj); const DerivedSP& S = mS;
            ASSERT(1 == S.use_count());

            ObjWP mX(S); const ObjWP& X = mX;
            ASSERT(!X.expired());
            ASSERT(1 == X.use_count());

            DerivedWP mY(S); const DerivedWP& Y = mY;
            ASSERT(!Y.expired());
            ASSERT(1 == Y.use_count());

            ObjWP mZ(Y); const ObjWP& Z = mZ;
            ASSERT(!Z.expired());
            ASSERT(1 == Z.use_count());

            ObjWP mA; const ObjWP& A = mA;

            mA = S;
            ASSERT(!A.expired());
            ASSERT(1 == A.use_count());

            ObjWP mB; const ObjWP& B = mB;

            mB = Y;
            ASSERT(!B.expired());
            ASSERT(1 == B.use_count());

            mA = Y;
            ASSERT(!A.expired());
            ASSERT(1 == A.use_count());
        }
      } break;

      case -1: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST
        //
        // Concerns:  We should have a benchmark to time performance and keep
        //   track of performance improvements or losses as the code evolves.
        //   Performance here means both runtime and memory usage.
        //
        // Test plan: First measure the basics:  1. time to construct and
        //   destroy a shared pointer (executed in a loop), for various kinds
        //   of representations, and for various kinds of creation (allocating
        //   a new representation or simply incrementing a reference count); 2.
        //   time to assign copy shared pointers (measured by shuffling a
        //   vector of shared pointers).  Note that the type that is pointed to
        //   matters in (1), for in-place construction, but what matters is the
        //   base line, not the absolute timings; the type that is pointed to
        //   does *not* matter in (2), since this only measures the time to
        //   increment/decrement the shared pointers.  We measure the baselines
        //   (creation, copying and destruction of the test objects) so that
        //   they can be deducted from the shared pointers runtimes (e.g., when
        //   constructing inplace).
        //       Then we measure the times in various more specialized
        //   scenarios such as releasing in-place representations into a pool.
        //
        // Testing:
        //   PERFORMANCE
        // --------------------------------------------------------------------

        if (verbose) printf("\nPERFORMANCE TEST"
                            "\n================\n");

        PerformanceTester<Obj>::test(verbose, veryVeryVerbose);

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
