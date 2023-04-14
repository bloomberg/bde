// bslstl_sharedptr.t.cpp                                             -*-C++-*-

// ----------------------------------------------------------------------------
//                            U_ENABLE_DEPRECATIONS
//
// Set 'U_ENABLE_DEPRECATIONS' to 1 get warnings about uses of deprecated
// methods.  These warnings are quite voluminous.  Test case 14 will fail
// unless '0 == U_ENABLE_DEPRECATIONS' to make sure we don't ship with these
// warnings enabled.
// ----------------------------------------------------------------------------

#undef  U_ENABLE_DEPRECATIONS
#define U_ENABLE_DEPRECATIONS 0
#if U_ENABLE_DEPRECATIONS
# define BSLS_DEPRECATE_FEATURE_ENABLE_ALL_DEPRECATIONS_FOR_TESTING 1
# include <bsls_deprecatefeature.h>
#endif

#include <bslstl_sharedptr.h>

#include <bslstl_badweakptr.h>

#include <bslalg_autoscalardestructor.h>
#include <bslalg_hastrait.h>
#include <bslalg_typetraitusesbslmaallocator.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_newdeleteallocator.h>
#include <bslma_stdallocator.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_assert.h>
#include <bslmf_integralconstant.h>
#include <bslmf_issame.h>

#include <bsls_alignmenttotype.h>
#include <bsls_alignmentutil.h>
#include <bsls_asserttest.h>
#include <bsls_compilerfeatures.h>
#include <bsls_bsltestutil.h>
#include <bsls_buildtarget.h>
#include <bsls_exceptionutil.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>
#include <bsls_nameof.h>
#include <bsls_platform.h>
#include <bsls_objectbuffer.h>
#include <bsls_stopwatch.h>
#include <bsls_types.h>

#include <bsltf_allocargumenttype.h>
#include <bsltf_allocemplacabletesttype.h>
#include <bsltf_argumenttype.h>
#include <bsltf_emplacabletesttype.h>
#include <bsltf_movestate.h>
#include <bsltf_stdstatefulallocator.h>

// Look what the usage examples drag in...
//#include <bslstl_deque.h>
#include <bslstl_list.h>
#include <bslstl_map.h>
#include <bslstl_string.h>
#include <bslstl_vector.h>

#include <new>           // 'operator delete'
#include <stdio.h>
#include <stdlib.h>      // 'atoi'
#include <string.h>      // 'strcmp', 'strcpy'

#ifdef BSLS_PLATFORM_CMP_MSVC   // Microsoft Compiler
# ifdef _MSC_EXTENSIONS          // Microsoft Extensions Enabled
#   include <new>                  // if so, need to include <new> as well
# endif
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER)
# include <type_traits>
#endif

// BDE_VERIFY pragma: -FE01  // Testing throws exceptions not derived from std

// Warnings that we hope to stop silencing one day:
// BDE_VERIFY pragma:-IND01 // Test blocks not indenting after if (printf(""))
// BDE_VERIFY pragma:-IND04 // Many violations of aligning variable declaration
// BDE_VERIFY pragma:-IND06 // Many violations of aligning template parameters
// BDE_VERIFY pragma:-TP03  // Many tests without assigned test case
// BDE_VERIFY pragma:-TP29  // Formatting of test concerns as a numbered list
// BDE_VERIFY pragma:-TP32  // Formatting of test concerns as a numbered list
// BDE_VERIFY pragma:-UC01  // Warning on all-const identifiers is too strict
//
// BDE_VERIFY pragma:-AN01  // declaration/definition mismatch on parameters

#undef ESP  //  From somewhere in SunOS2.10/x86 system headers

#if defined BSLS_PLATFORM_CMP_SUN
# define BSLSTL_SHAREDPTR_LIMIT_TESTING_COMPLEXITY 1
// Some compilers struggle for resources when trying to compile all of the
// template instantiations required for this test driver.  We will perform
// slightly less thorough testing on those compilers, testing sufficient to
// rely on the more thorough testing offered by platforms that are not as
// constrained.
#endif

#if defined(BSLS_COMPILERFEATURES_SIMULATE_FORWARD_WORKAROUND)
# define BSL_DO_NOT_TEST_MOVE_FORWARDING 1
// Some compilers produce ambiguities when trying to construct our test types
// for 'emplace'-type functionality with the C++03 move-emulation.  This is a
// compiler bug triggering in lower level components, so we simply disable
// those aspects of testing, and rely on the extensive test coverage on other
// platforms.
#endif

#if defined(BSLS_PLATFORM_OS_AIX) && BSLS_COMPILERFEATURES_CPLUSPLUS < 201103L
#define BSLSTL_SHAREDPTR_DONT_TEST_UNBOUNDED_ARRAYS
// Some compilers (AIX) do not like unbounded arrays in types in C++03.  Things
// like 'shared_ptr<int[]>' cause them to complain.
#endif

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
// [35] weak_ptr<T> weak_from_this()
// [35] weak_ptr<const T> weak_from_this() const
//
// bsl::shared_ptr
//----------------
// TYPE NAMES
// [  ] element_type
// [  ] weak_type
//
// CREATORS
// [ 2] shared_ptr()
// [ 3] shared_ptr(OTHER *ptr)
// [ 3] shared_ptr(OTHER *ptr, DELETER *deleter)
// [ 3] shared_ptr(OTHER *ptr, DELETER, ALLOCATOR, SFINAE)
// [ 3] shared_ptr(nullptr_t, DELETER, bslma::Allocator * = 0)
// [ 3] shared_ptr(nullptr_t, DELETER, ALLOCATOR, SFINAE)
// [12] shared_ptr(const shared_ptr<ANY_TYPE>&, ELEMENT_TYPE *)
// [ 7] shared_ptr(const shared_ptr& original) noexcept
// [ 7] shared_ptr(const shared_ptr<OTHER>& other) noexcept
// [ 7] shared_ptr(shared_ptr&& other) noexcept
// [ 7] shared_ptr(shared_ptr<OTHER>&& other) noexcept
// [  ] shared_ptr(const weak_ptr<OTHER>& alias)
// [ 3] shared_ptr(unique_ptr<OTHER, DELETER>&& adoptee)
// [ 3] shared_ptr(nullptr_t)
// [ 2] ~shared_ptr()
//
// DEPRECATED CREATORS
// [ 3] shared_ptr(std::auto_ptr<OTHER> autoPtr, bslma::Allocator*=0)
// [  ] shared_ptr(std::auto_ptr_ref<TYPE>, bslma::Allocator * = 0)
//
// BDE SPECIFIC CREATORS
// [ 3] shared_ptr(OTHER *ptr, bslma::Allocator *basicAllocator)
// [ 3] shared_ptr(ELEM_TYPE *ptr, bslma::SharedPtrRep *rep)
// [ 3] shared_ptr(ELEM_TYPE *, bslma::SharedPtrRep *, FromSharedTag)
// [ 3] shared_ptr(OTHER *ptr, DELETER, bslma::Allocator* = 0)
// [ 3] shared_ptr(nullptr_t, bslma::Allocator *)
// [ 3] shared_ptr(unique_ptr<OTHER, DELETER>&&, bslma::Allocator *)
// [20] shared_ptr(bslma::ManagedPtr<OTHER>&, bslma::Allocator * = 0)
//
// MANIPULATORS
// [ 9] shared_ptr& operator=(const shared_ptr& rhs) noexcept
// [ 9] shared_ptr& operator=(const shared_ptr<OTHER>& rhs) noexcept
// [ 9] shared_ptr& operator=(shared_ptr&& rhs) noexcept
// [ 9] shared_ptr& operator=(shared_ptr<OTHER>&& rhs) noexcept
// [ 9] shared_ptr& operator=(std::unique_ptr<OTHER, DELETER>&& rhs)
// [ 8] void swap(shared_ptr& src) noexcept
// [ 2] void reset() noexcept
// [ 2] void reset(OTHER *ptr)
// [ 2] void reset(OTHER *ptr, DELETER deleter)
// [ 2] void reset(OTHER *ptr, DELETER deleter, ALLOCATOR basicAllocator)
//
// DEPRECATED MANIPULATORS
// [ 9] shared_ptr& operator=(std::auto_ptr<OTHER> rhs)
//
// BDE SPECIFIC MANIPULATORS
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
// [14] void reset(const shared_ptr<OTHER>& source, TYPE *ptr)
// [ 9] shared_ptr& operator=(bslma::ManagedPtr<OTHER> rhs)
//
// ACCESSORS
// [ 4] TYPE *get() const
// [ 4] add_lvalue_reference<ELEM_TYPE>::type operator*() const
// [ 4] TYPE *operator->() const
// [ 4] long use_count() const
// [ 4] bool unique() const
// [16] operator BoolType() const
// [29] bool shared_ptr::owner_before(const shared_ptr<BDE_OTHER_TYPE>& rhs)
// [29] bool shared_ptr::owner_before(const weak_ptr<BDE_OTHER_TYPE>& rhs)
//
// BDE SPECIFIC ACCESSORS
// [ 4] add_lvalue_reference<ELEM_TYPE>::type operator[](ptrdiff_t) const
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
// [19] shared_ptr<TARGET> reinterpret_pointer_cast(const shared_ptr<SRC>&)
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
// [30] CONCERN: Shared -> Managed -> Shared uses same 'rep' object
// [21] DRQS 26465543 [void reset()]
// [22] shared_ptr<cv-void>
// [36] CONCERN: 'shared_ptr' constructors SFINAE on compatible pointers
// [36] CONCERN: 'shared_ptr' = operators SFINAE on compatible pointers
// [37] CONCERN: 'shared_ptr<bslma::Allocator>' behaves correctly
// [38] CONCERN: Methods qualified 'noexcept' in standard are so implemented.
// [43] REGRESSIONS
// [44] CLASS TEMPLATE DEDUCTION GUIDES
// [45] TESTING BITWISE AND NOTHROW MOVEABILITY
// [46] 0 == U_ENABLE_DEPRECATIONS
// [-1] PERFORMANCE
// [  ] USAGE EXAMPLE (shared_ptr) // TBD
// [  ] CONCERN: 'shared_ptr<FactoryClass>' behaves correctly
// [  ] CONCERN: C++ 'bsl::shared_ptr' ISO CONFORMANCE
// Further, there are a number of behaviors that explicitly should not compile
// by accident that we will provide tests for.  These tests should fail to
// compile if the appropriate macro is defined.  Each such test will use a
// unique macro for its feature test, and provide a commented-out definition of
// that macro immediately above the test, to easily enable compiling that test
// while in development.  Below is the list of all macros that control the
// availability of these tests:
//  #define BSLSTL_SHARED_PTR_UNIQUE_PTR_COMPILE_FAIL_NOT_AN_ALLOCATOR
//  #define BSLSTL_SHARED_PTR_MANAGED_PTR_COMPILE_FAIL_NOT_AN_ALLOCATOR

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
// [24] weak_ptr() noexcept
// [24] weak_ptr(weak_ptr&& original) noexcept
// [24] weak_ptr(weak_ptr<COMPATIBLE_TYPE>&& other) noexcept
// [24] weak_ptr(const weak_ptr& original) noexcept
// [24] weak_ptr(const weak_ptr<COMPATIBLE_TYPE>& other) noexcept
// [24] weak_ptr(const shared_ptr<COMPATIBLE_TYPE>& other) noexcept
// [24] weak_ptr(weak_ptr<COMPATIBLE_TYPE>&& other) noexcept
// [24] ~weak_ptr()
//
// MANIPULATORS
// [25] weak_ptr& operator=(weak_ptr&& rhs) noexcept
// [25] weak_ptr& operator=(weak_ptr<COMPATIBLE_TYPE>&& rhs) noexcept
// [25] weak_ptr& operator=(const weak_ptr& rhs) noexcept
// [25] weak_ptr& operator=(const weak_ptr<COMPATIBLE_TYPE>& rhs) noexcept
// [25] weak_ptr& operator=(const shared_ptr<COMPATIBLE_TYPE>& rhs) noexcept
// [26] void reset()
// [28] void swap(weak_ptr& src)
//
// ACCESSORS
// [24] bslma::SharedPtrRep *rep() const
// [24] bool expired() const
// [24] long use_count() const
// [27] shared_ptr<TYPE> lock() const
// [29] bool weak_ptr::owner_before(const shared_ptr<BDE_OTHER_TYPE>& rhs)
// [29] bool weak_ptr::owner_before(const weak_ptr<BDE_OTHER_TYPE>& rhs)
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST (weak_ptr)
// [39] USAGE EXAMPLE 1: weak_ptr
// [40] USAGE EXAMPLE 2: weak_ptr
// [41] USAGE EXAMPLE 3: weak_ptr
// [36] 'weak_ptr' constructors SFINAE on compatible pointers
// [36] 'weak_ptr' = operators SFINAE on compatible pointers
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
// validating these functors is that there are a valid, copy-constructible
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
// class NonPolymorphicTestBaseObject
// ----------------------------------
//
// class NonPolymorphicTestObject
//-------------------------------
// [  ] volatile bsls::Types::Int64 *deleteCounter() const
// [  ] volatile bsls::Types::Int64 *copyCounter() const
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
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
// bsl::shared_ptr (DEPRECATED INTERFACE)
//---------------------------------------
// DEPRECATED MANIPULATORS
// [ 2] void clear()
// [ 2] void load(OTHER *ptr, bslma::Allocator *allocator=0)
// [ 2] void load(OTHER *ptr, const DELETER&, bslma::Allocator *)
//
// DEPRECATED ACCESSORS
// [ 4] int numReferences() const
// [ 4] TYPE *ptr() const
//
// bsl::weak_ptr (DEPRECATED INTERFACE)
//-------------------------------------
// DEPRECATED ACCESSORS
// [24] int numReferences() const
// [27] shared_ptr<TYPE> acquireSharedPtr() const
#endif // BDE_OMIT_INTERNAL_DEPRECATED

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

// ============================================================================
//              ADDITIONAL TEST MACROS FOR THIS TEST DRIVER
// ----------------------------------------------------------------------------

#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
# define ASSERT_NOEXCEPT(...) ASSERT(noexcept(__VA_ARGS__))
#else
# define ASSERT_NOEXCEPT(...)
#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT)
# define DECLARE_BOOL_CONSTANT(NAME, EXPRESSION)                              \
    const BSLS_KEYWORD_CONSTEXPR bsl::bool_constant<EXPRESSION> NAME{}
    // This leading branch is the preferred version for C++17, but the feature
    // test macro is (currently) for documentation purposes only, and never
    // defined.  This is the ideal (simplest) form for such declarations:
#elif defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR)
# define DECLARE_BOOL_CONSTANT(NAME, EXPRESSION)                              \
    constexpr bsl::integral_constant<bool, EXPRESSION> NAME{}
    // This is the preferred C++11 form for the definition of integral constant
    // variables.  It assumes the presence of 'constexpr' in the compiler as an
    // indication that brace-initialization and traits are available, as it has
    // historically been one of the last C++11 features to ship.
#else
# define DECLARE_BOOL_CONSTANT(NAME, EXPRESSION)                              \
    static const bsl::integral_constant<bool, EXPRESSION> NAME =              \
                 bsl::integral_constant<bool, EXPRESSION>()
    // 'bsl::integral_constant' is not an aggregate prior to C++17 extending
    // the rules, so a C++03 compiler must explicitly initialize integral
    // constant variables in a way that is unambiguously not a vexing parse
    // that declares a function instead.
#endif

#if defined(BSLSTL_SHAREDPTR_SUPPORTS_SFINAE_CHECKS)
# if !defined(BSLS_PLATFORM_CMP_CLANG)                             \
  || !defined(__APPLE_CC__) && BSLS_PLATFORM_CMP_VERSION >= 120000 \
  ||  defined(__APPLE_CC__) && BSLS_PLATFORM_CMP_VERSION >  130000
    // There are some compilers that, while they support expression SFINAE, do
    // not check for substitution failures in discarded-value expressions (as
    // in, for example, a 'static_cast<void>(expression)').  As a result, for
    // these compilers, 'bsl::shared_ptr' does not discard the value of SFINAE
    // conditions when used as the left-hand side of a comma operator.
    // Ordinarily, 'bsl::shared_ptr' would discard SFINAE conditions when used
    // on the left-hand side of the comma operator in order to prevent the
    // possibility of the expression having unintended meaning due to the
    // operator being overloaded.  See the notes above the definition of
    // 'BSLSTL_SHAREDPTR_SFINAE_DISCARD' in this component's header for more
    // information.
#  define BSLSTL_SHAREDPTR_SUPPORTS_SFINAE_DISCARDING
# endif
#endif

// ============================================================================
//                              USAGE EXAMPLES
// ----------------------------------------------------------------------------

// Disable specific bde_verify warnings where practice of usage example may
// differ.

// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -FABC01 // Functions ordered for expository purpose

// BDE_VERIFY pragma: -FD01  // Function needs contract, we probably should fix
// BDE_VERIFY pragma: -FD02  // Banners diagnose badly unless we fix for FD01
// BDE_VERIFY pragma: -FD03  // no contract, so no ticks

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

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
#if 0  // Note that usage example 3, 4 and 5 rely on both mutex and condition
       // variable objects that have not yet been ported down to 'bsl'.
namespace NAMESPACE_USAGE_EXAMPLE_3 {
//
///Example 3 - Custom Deleters
/// -  -  -  -  -  -  -  -  -
// The role of a "deleter" is to allow users to define a custom "cleanup" for a
// shared object.  Although cleanup generally involves destroying the object,
// this need not be the case.  The following example demonstrates the use of a
// custom deleter to construct "locked" pointers.  First we declare a custom
// deleter that, when invoked, releases the specified mutex and signals the
// specified condition variable.
//..
    class my_MutexUnlockAndBroadcastDeleter {

        // DATA
        bslmt::Mutex     *d_mutex_p;  // mutex to lock (held, not owned)
        bslmt::Condition *d_cond_p;   // condition variable used to broadcast
                                      // (held, not owned)

      public:
        // CREATORS
        my_MutexUnlockAndBroadcastDeleter(bslmt::Mutex     *mutex,
                                          bslmt::Condition *cond)
            // Create this 'my_MutexUnlockAndBroadcastDeleter' object.  Use the
            // specified 'cond' to broadcast a signal and the specified 'mutex'
            // to serialize access to 'cond'.  The behavior is undefined unless
            // 'mutex' is not 0 and 'cond' is not 0.
        : d_mutex_p(mutex)
        , d_cond_p(cond)
        {
            BSLS_ASSERT(mutex);
            BSLS_ASSERT(cond);

            d_mutex_p->lock();
        }

        my_MutexUnlockAndBroadcastDeleter(
                                   my_MutexUnlockAndBroadcastDeleter& original)
        : d_mutex_p(original.d_mutex_p)
        , d_cond_p(original.d_cond_p)
        {
        }
//..
// Since this deleter does not actually delete anything, 'void *' is used in
// the signature of 'operator()', allowing it to be used with any type of
// object.
//..
        void operator()(void *)
        {
            d_cond_p->broadcast();
            d_mutex_p->unlock();
        }
    };
//..
// Next we declare a thread-safe queue 'class'.  The 'class' uses a
// non-thread-safe 'bsl::deque' to implement the queue.  Thread-safe 'push' and
// 'pop' operations that push and pop individual items are provided.  For
// callers that wish to gain direct access to the queue, the 'queue' method
// returns a shared pointer to the queue using the
// 'my_MutexUnlockAndBroadcastDeleter'.  Callers can safely access the queue
// through the returned shared pointer.  Once the last reference to the pointer
// is released, the mutex will be unlocked and the condition variable will be
// signaled to allow waiting threads to re-evaluate the state of the queue.
//..
    template <class ELEMENT_TYPE>
    class my_SafeQueue {

        // DATA
        bslmt::Mutex             d_mutex;
        bslmt::Condition         d_cond;
        bsl::deque<ELEMENT_TYPE> d_queue;

        // . . .

      public:
        // MANIPULATORS
        void push(const ELEMENT_TYPE& obj);

        ELEMENT_TYPE pop();

        bsl::shared_ptr<bsl::deque<ELEMENT_TYPE> > queue();
    };

    template <class ELEMENT_TYPE>
    void my_SafeQueue<ELEMENT_TYPE>::push(const ELEMENT_TYPE& obj)
    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
        d_queue.push_back(obj);
        d_cond.signal();
    }

    template <class ELEMENT_TYPE>
    ELEMENT_TYPE my_SafeQueue<ELEMENT_TYPE>::pop()
    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
        while (!d_queue.size()) {
           d_cond.wait(&d_mutex);
        }
        ELEMENT_TYPE value(d_queue.front());
        d_queue.pop_front();
        return value;
    }

    template <class ELEMENT_TYPE>
    bsl::shared_ptr<bsl::deque<ELEMENT_TYPE> >
    my_SafeQueue<ELEMENT_TYPE>::queue()
    {
        return bsl::shared_ptr<bsl::deque<ELEMENT_TYPE> >(
                          &d_queue,
                          my_MutexUnlockAndBroadcastDeleter(&d_mutex, &d_cond),
                          0);
    }
//..
}  // close namespace NAMESPACE_USAGE_EXAMPLE_3

namespace NAMESPACE_USAGE_EXAMPLE_4 {
//..
//
///Implementation Hiding
///- - - - - - - - - - -
// 'bsl::shared_ptr' refers to the parameterized type on which it is
// instantiated "in name only".  This allows for the instantiation of shared
// pointers to incomplete or 'void' types.  This feature is useful for
// constructing interfaces where returning a pointer to a shared object is
// desirable, but in order to control access to the object its interface cannot
// be exposed.  The following examples demonstrate two techniques for achieving
// this goal using a 'bsl::shared_ptr'.
//
///Example 4 - Hidden Interfaces
/// -  -  -  -  -  -  -  -  -  -
// Example 4 demonstrates the use of incomplete types to hide the interface of
// a 'my_Session' type.  We begin by declaring the 'my_SessionManager' 'class',
// which allocates and manages 'my_Session' objects.  The interface ('.h')
// merely forward declares 'my_Session'.  The actual definition of the
// interface is in the implementation ('.cpp') file.
//
// We forward-declare 'my_Session' to be used (in name only) in the definition
// of 'my_SessionManager':
//..
    class my_Session;
//..
// Next, we define the 'my_SessionManager' class:
//..
    class my_SessionManager {

        // TYPES
        typedef bsl::map<int, bsl::shared_ptr<my_Session> > HandleMap;

        // DATA
        bslmt::Mutex      d_mutex;
        HandleMap         d_handles;
        int               d_nextSessionId;
        bslma::Allocator *d_allocator_p;

//..
// It is useful to have a designated name for the 'bsl::shared_ptr' to
// 'my_Session':
//..
      public:
        // TYPES
        typedef bsl::shared_ptr<my_Session> my_Handle;
//..
// We need only a default constructor:
//..
        // CREATORS
        my_SessionManager(bslma::Allocator *allocator = 0);
//..
// The 3 methods that follow construct a new session object and return a
// 'bsl::shared_ptr' to it.  Callers can transfer the pointer, but they cannot
// directly access the object's methods since they do not have access to its
// interface.
//..
        // MANIPULATORS
        my_Handle openSession(const bsl::string& sessionName);
        void closeSession(my_Handle handle);

        // ACCESSORS
        bsl::string getSessionName(my_Handle handle) const;
    };
//..
// Now, in the implementation of the code, we can define and implement the
// 'my_Session' class:
//..
    class my_Session {

        // DATA
        bsl::string d_sessionName;
        int         d_handleId;

      public:
        // CREATORS
        my_Session(const bsl::string&  sessionName,
                   int                 handleId,
                   bslma::Allocator   *basicAllocator = 0);

        // ACCESSORS
        int handleId() const;
        const bsl::string& sessionName() const;
    };

    // CREATORS
    inline
    my_Session::my_Session(const bsl::string&  sessionName,
                           int                 handleId,
                           bslma::Allocator   *basicAllocator)
    : d_sessionName(sessionName, basicAllocator)
    , d_handleId(handleId)
    {
    }

    // ACCESSORS
    inline
    int my_Session::handleId() const
    {
        return d_handleId;
    }

    inline
    const bsl::string& my_Session::sessionName() const
    {
        return d_sessionName;
    }
//..
// The following shows the implementation of 'my_SessionManager'.  Note that
// the interface for 'my_Session' is not known:
//..
    inline
    my_SessionManager::my_SessionManager(bslma::Allocator *allocator)
    : d_nextSessionId(1)
    , d_allocator_p(bslma::Default::allocator(allocator))
    {
    }

    inline
    my_SessionManager::my_Handle
    my_SessionManager::openSession(const bsl::string& sessionName)
    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
        my_Handle session(new(*d_allocator_p) my_Session(sessionName,
                                                         d_nextSessionId++,
                                                         d_allocator_p));
        d_handles[session->handleId()] = session;
        return session;
    }

    inline
    void my_SessionManager::closeSession(my_Handle handle)
    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
        HandleMap::iterator it = d_handles.find(handle->handleId());
        if (it != d_handles.end()) {
            d_handles.erase(it);
        }
    }

    inline
    bsl::string my_SessionManager::getSessionName(my_Handle handle) const
    {
        return handle->sessionName();
    }
//..
}  // close namespace NAMESPACE_USAGE_EXAMPLE_4

namespace NAMESPACE_USAGE_EXAMPLE_5 {
    using NAMESPACE_USAGE_EXAMPLE_4::my_Session;
//..
///Example 5 - Opaque Types
///  -  -  -  -  -  -  -  -
// In the above example, users could infer that 'my_Handle' is a pointer to a
// 'my_Session' but have no way to directly access it's methods since the
// interface is not exposed.  In the following example, 'my_SessionManager' is
// re-implemented to provide an even more opaque session handle.  In this
// implementation, 'my_Handle' is redefined using 'void' providing no
// indication of its implementation.  Note that using 'void' will require
// casting in the implementation and, therefore, will be a little more
// expensive.
//
// In the interface, define 'my_SessionManager' as follows:
//..
    class my_SessionManager {

        // TYPES
        typedef bsl::map<int, bsl::shared_ptr<void> > HandleMap;

        // DATA
        bslmt::Mutex      d_mutex;
        HandleMap         d_handles;
        int               d_nextSessionId;
        bslma::Allocator *d_allocator_p;
//..
// It is useful to have a name for the 'void' 'bsl::shared_ptr' handle.
//..
       public:
        // TYPES
        typedef bsl::shared_ptr<void> my_Handle;

        // CREATORS
        my_SessionManager(bslma::Allocator *allocator = 0);

        // MANIPULATORS
        my_Handle openSession(const bsl::string& sessionName);
        void closeSession(my_Handle handle);

        // ACCESSORS
        bsl::string getSessionName(my_Handle handle) const;
    };
//..
// Next we define the methods of 'my_SessionManager':
//..
    // CREATORS
    inline
    my_SessionManager::my_SessionManager(bslma::Allocator *allocator)
    : d_nextSessionId(1)
    , d_allocator_p(bslma::Default::allocator(allocator))
    {
    }

    // MANIPULATORS
    inline
    my_SessionManager::my_Handle
    my_SessionManager::openSession(const bsl::string& sessionName)
    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
//..
// Notice that 'my_Handle', which is a shared pointer to 'void', can be
// transparently assigned to a shared pointer to a 'my_Session' object.  This
// is because the 'bsl::shared_ptr' interface allows shared pointers to types
// that can be cast to one another to be assigned directly.
//..
        my_Handle session(new(*d_allocator_p) my_Session(sessionName,
                                                         d_nextSessionId++,
                                                         d_allocator_p));
        bsl::shared_ptr<my_Session> myhandle =
                        bslstl::SharedPtrUtil::staticCast<my_Session>(session);
        d_handles[myhandle->handleId()] = session;
        return session;
    }

    inline
    void my_SessionManager::closeSession(my_Handle handle)
    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
//..
// Perform a static cast from 'bsl::shared_ptr<void>' to
// 'bsl::shared_ptr<my_Session>'.
//..
        bsl::shared_ptr<my_Session> myhandle =
                         bslstl::SharedPtrUtil::staticCast<my_Session>(handle);
//..
// Test to make sure that the pointer is non-null before using 'myhandle':
//..
        if (!myhandle.get()) {
            return;                                                   // RETURN
        }

        HandleMap::iterator it = d_handles.find(myhandle->handleId());
        if (it != d_handles.end()) {
            d_handles.erase(it);
        }
    }

    bsl::string my_SessionManager::getSessionName(my_Handle handle) const
    {
        bsl::shared_ptr<my_Session> myhandle =
                         bslstl::SharedPtrUtil::staticCast<my_Session>(handle);

        if (!myhandle.get()) {
            return bsl::string();
        } else {
            return myhandle->sessionName();
        }
    }
//..
}  // close usage example namespace
//..
#endif
#endif // BDE_OMIT_INTERNAL_DEPRECATED

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

    void search(bsl::vector<SearchResult>       * /* results */,
                const PeerCache&                 peerCache,
                const bsl::vector<bsl::string>&   /* keywords */)
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

typedef BloombergLP::bslmf::MovableRefUtil MoveUtil;

// 'MyTestObject' CLASS HIERARCHY (defined below)
class MyTestObject;
class MyTestBaseObject;
class MyTestDerivedObject;

// OTHER TEST OBJECTS (defined below)
class MyPDTestObject;

// TEST DELETERS SECTION (defined below)
class MyTestObjectFactory;
class MyTestDeleter;
class MyAllocTestDeleter;
class MyAllocArgTestDeleter;
class MyBslAllocArgTestDeleter;

// TEST-CASE SUPPORT TYPES
template <class POINTER>
struct PerformanceTester;

// TYPEDEFS
typedef bsltf::AllocEmplacableTestType      MyInplaceAllocatableObject;
typedef bsltf::EmplacableTestType           MyInplaceTestObject;
typedef bsltf::MoveState                    MoveState;

typedef bsl::shared_ptr<MyTestObject>               Obj;
typedef bsl::shared_ptr<const MyTestObject>         ConstObj;
typedef bsl::shared_ptr<MyInplaceTestObject>        TCObj;
typedef bsl::shared_ptr<MyInplaceAllocatableObject> TCAObj;
typedef MyTestObject                                TObj;

// GLOBAL VERBOSITY FLAGS
bool             verbose = false;
bool         veryVerbose = false;
bool     veryVeryVerbose = false;
bool veryVeryVeryVerbose = false;

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

namespace support {

class CommaTest {
    CommaTest() {}

  public:
    static const CommaTest value;
};

const CommaTest CommaTest::value;

class Evil {
    Evil(const Evil&);
    void operator=(const Evil&);

  public:
    // PUBLIC TYPEDEFS
    typedef Evil Fail;
};

template <class OTHER>
Evil::Fail operator,(const CommaTest&, const OTHER&) BSLS_KEYWORD_DELETED;


struct FactoryDeleter {
    ///Implementation Note
    ///-------------------
    // The constructor sfinae checks of 'bsl::shared_ptr' are resilient to
    // overloading of the comma operator only when the compiler is correctly
    // able to require well-formedness of discarded-value expressions.

#if defined(BSLSTL_SHAREDPTR_SUPPORTS_SFINAE_DISCARDING)
    template <class TYPE>
    const CommaTest& deleteObject(TYPE *)
        // Return a type that overloads the comma operator in an attempt to
        // catch the most awkward metaprogramming corners.  Similarly, this
        // method is not 'const'-qualified.
    {
        return CommaTest::value;
    }
#else
    template <class TYPE>
    void deleteObject(TYPE *)
        // Do nothing.
    {
    }
#endif

#if 0   // This matches current test driver expectations, but should actually
        // be removed to provide a more interesting test case
    const CommaTest& deleteObject(bsl::nullptr_t)
        // Return a type that overloads the comma operator in an attempt to
        // catch the most awkward metaprogramming corners.  Similarly, this
        // method is not 'const'-qualified.
    {
        return CommaTest::value;
    }
#endif
};


template <class TYPE>
struct TypedFactory {

    void deleteObject(TYPE *) const
        // This function has no effect.
    {
    }
};


struct UniversalPointer {

    void *d_data;

    operator const void *() const { return d_data; }

    template <class T>
    operator T *() const { return 0; }
};

struct DoNotDeleter {
    typedef UniversalPointer pointer;

    void operator()(const void *) const {}  // do nothing deleter
};

template <class T>
struct TypedDeleter{
    typedef T * pointer;

    void operator()(T *) const {}  // do nothing deleter
};

}  // close namespace support

          // *** 'NonPolymorphicTestBaseObject' CLASS HIERARCHY ***

                     // ==================================
                     // class NonPolymorphicTestBaseObject
                     // ==================================

class NonPolymorphicTestBaseObject {
    // This class provides a non-polymorphic test object used to verify that
    // shared pointers can statically cast without slicing.

    // DATA
    char d_padding[32];    // Padding bytes that are never initialized

  public:
    // CREATORS
    NonPolymorphicTestBaseObject() { (void)d_padding; }
        // Create a 'NonPolymorphicTestBaseObject' object.  Note that the
        // padding bytes are deliberately never initialized.

    NonPolymorphicTestBaseObject(const NonPolymorphicTestBaseObject&) {}
        // Create a 'NonPolymorphicTestBaseObject' object.  Note that this
        // constructor does not copy the unused (and uninitialized) values of
        // the 'd_padding' bytes.

    // ~NonPolymorphicTestBaseObject() = default;
        // Destroy this object.  Note that this descructor is deliberately not
        // virtual!

    NonPolymorphicTestBaseObject&
    operator=(const NonPolymorphicTestBaseObject&) { return *this; }
        // Return a reference to 'this' object.  There are no other effects.

};

                       // ==============================
                       // class NonPolymorphicTestObject
                       // ==============================

class NonPolymorphicTestObject : public NonPolymorphicTestBaseObject {
    // This class provides a non-polymorphic test object that keeps track of
    // how many objects have been deleted.  Optionally, also keeps track of how
    // many objects have been copied.

    // DATA
    volatile bsls::Types::Int64 *d_deleteCounter_p;
    volatile bsls::Types::Int64 *d_copyCounter_p;

  public:
    // CREATORS
    NonPolymorphicTestObject(const NonPolymorphicTestObject& original);
        // Create a copy of the specified 'original' object.

    explicit NonPolymorphicTestObject(bsls::Types::Int64 *deleteCounter,
                                      bsls::Types::Int64 *copyCounter = 0);

    ~NonPolymorphicTestObject();
        // Destroy this object.  Note that this destructor is deliberately not
        // virtual.

    // ACCESSORS
    volatile bsls::Types::Int64 *copyCounter() const;
        // Return a pointer to the counter (if any) used to track the number of
        // times an object of type 'NonPolymorphicTestObject' has been copied.

    volatile bsls::Types::Int64 *deleteCounter() const;
        // Return a pointer to the counter used to track the number of times an
        // object of type 'NonPolymorphicTestObject' has been copied.

};

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
    MyTestBaseObject() { (void)d_padding; }
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

    explicit MyTestDerivedObject(bsls::Types::Int64 *deleteCounter,
                                 bsls::Types::Int64 *copyCounter = 0);
};

                         // *** OTHER TEST OBJECTS ***

                             // =================
                             // class MyAggregate
                             // =================

struct MyAggregate {
    int    d_first;
    double d_second;
};

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

#if defined(BSLSTL_SHAREDPTR_TEST_EVIL_BUILD_FAIL)
namespace ConfirmTypeIsEvil
{
char g_buffer[sizeof(MostEvilTestType)];
MostEvilTestType *g_addr = reinterpret_cast<MostEvilTestType*>(&g_buffer[0]);
void *g_fail = new(g_addr) MostEvilTestType(5);
}
#endif

                             // =================
                             // class DeleteChecker
                             // =================

struct DeleteChecker {
    static size_t s_singleDeleteCount;
    static size_t s_arrayDeleteCount;

    static void operator delete (void* ptr) BSLS_KEYWORD_NOEXCEPT
        // Record the fact that the single form of operator delete' has been
        // called, and then pass the specified 'ptr' to the global
        // 'operator delete' to do the actual deletion.
    {
        ++s_singleDeleteCount;
        ::operator delete(ptr);
    }

    static void operator delete[] (void* ptr) BSLS_KEYWORD_NOEXCEPT
        // Record the fact that the array form of operator delete' has been
        // called, and then pass the specified 'ptr' to the global
        // 'operator delete' to do the actual deletion.
    {
        ++s_arrayDeleteCount;
        ::operator delete[](ptr);
    }
};

size_t DeleteChecker::s_singleDeleteCount = 0;
size_t DeleteChecker::s_arrayDeleteCount = 0;

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

                             // =================
                             // class MyTestArgNN
                             // =================

typedef bsltf::ArgumentType< 1> MyTestArg01;
typedef bsltf::ArgumentType< 2> MyTestArg02;
typedef bsltf::ArgumentType< 3> MyTestArg03;
typedef bsltf::ArgumentType< 4> MyTestArg04;
typedef bsltf::ArgumentType< 5> MyTestArg05;
typedef bsltf::ArgumentType< 6> MyTestArg06;
typedef bsltf::ArgumentType< 7> MyTestArg07;
typedef bsltf::ArgumentType< 8> MyTestArg08;
typedef bsltf::ArgumentType< 9> MyTestArg09;
typedef bsltf::ArgumentType<10> MyTestArg10;
typedef bsltf::ArgumentType<11> MyTestArg11;
typedef bsltf::ArgumentType<12> MyTestArg12;
typedef bsltf::ArgumentType<13> MyTestArg13;
typedef bsltf::ArgumentType<14> MyTestArg14;
    // Define fourteen test argument types 'MyTestArg01..14' to be used with
    // the in-place constructors of 'MyInplaceTestObject'.

                          // ========================
                          // class MyAllocatableArgNN
                          // ========================

typedef bsltf::AllocArgumentType< 1> MyAllocatableArg01;
typedef bsltf::AllocArgumentType< 2> MyAllocatableArg02;
typedef bsltf::AllocArgumentType< 3> MyAllocatableArg03;
typedef bsltf::AllocArgumentType< 4> MyAllocatableArg04;
typedef bsltf::AllocArgumentType< 5> MyAllocatableArg05;
typedef bsltf::AllocArgumentType< 6> MyAllocatableArg06;
typedef bsltf::AllocArgumentType< 7> MyAllocatableArg07;
typedef bsltf::AllocArgumentType< 8> MyAllocatableArg08;
typedef bsltf::AllocArgumentType< 9> MyAllocatableArg09;
typedef bsltf::AllocArgumentType<10> MyAllocatableArg10;
typedef bsltf::AllocArgumentType<11> MyAllocatableArg11;
typedef bsltf::AllocArgumentType<12> MyAllocatableArg12;
typedef bsltf::AllocArgumentType<13> MyAllocatableArg13;
typedef bsltf::AllocArgumentType<14> MyAllocatableArg14;
    // Define fourteen test argument types 'MyAllocatableArg01..14' to be used
    // with the in-place constructors of 'MyInplaceAllocatableObject'.


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

BSLMF_ASSERT(!bslma::UsesBslmaAllocator<MyTestDeleter>::VALUE);

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
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(MyAllocTestDeleter,
                                   bslma::UsesBslmaAllocator);

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

                        // ===========================
                        // class MyAllocArgTestDeleter
                        // ===========================

class MyAllocArgTestDeleter {
    // This class provides a prototypical function-like deleter that takes a
    // 'bslma::Allocator' at construction using the leading-allocator
    // convention.  It is used to check that the allocator used to construct
    // the representation is passed correctly to the deleter.

    // DATA
    bslma::Allocator *d_allocator_p;  // allocator for object's state
    bslma::Allocator *d_deleter_p;    // allocator to use as a deleter
    void             *d_memory_p;     // dynamically allocated data

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(MyAllocArgTestDeleter,
                                   bslma::UsesBslmaAllocator);

    BSLMF_NESTED_TRAIT_DECLARATION(MyAllocArgTestDeleter,
                                   bslmf::UsesAllocatorArgT);

    // CREATORS
    MyAllocArgTestDeleter(bsl::allocator_arg_t,
                          bslma::Allocator     *basicAllocator,
                          bslma::Allocator     *deleter);
        // Create a 'MyAllocArgTestDeleter' using the specified 'deleter' to
        // destroy objects passed to the overloaded function call operator,
        // and using the specified 'basicAllocator' to allocate some additional
        // state for test purposes only.

    MyAllocArgTestDeleter(const MyAllocArgTestDeleter& original);
    MyAllocArgTestDeleter(bsl::allocator_arg_t,
                          bslma::Allocator             *basicAllocator,
                          const MyAllocArgTestDeleter&  original);
        // Create a 'MyAllocArgTestDeleter' object having the same deleter as
        // the specified 'original' object, and having a copy of the dummy
        // state.  Optionally specify a 'basicAllocator' used to supply memory.
        // If 'basicAllocator' is 0, the currently installed default allocator
        // is used.

    ~MyAllocArgTestDeleter();
        // Destroy this object.

    // MANIPULATORS
    MyAllocArgTestDeleter& operator=(const MyAllocArgTestDeleter& rhs);
        // Assign to this object the deleter of the specified 'rhs'.

    // ACCESSORS
    bslma::Allocator *allocator() const;
        // Return the 'basicAllocator' supplied to this object's constructor.

    template <class OBJECT_TYPE>
    void operator()(OBJECT_TYPE *ptr) const;
        // Destroy the object pointed to by the specified 'ptr' using the
        // deleter supplied to this object's constructor.
};

                       // ==============================
                       // class MyBslAllocArgTestDeleter
                       // ==============================

class MyBslAllocArgTestDeleter {
    // This class provides a prototypical function-like deleter that takes a
    // 'bsl::allocator<char>' at construction using the leading-allocator
    // convention.  It is used to check that the allocator used to construct
    // the representation is passed correctly to the deleter.

  public:
    // TYPES
    typedef bsl::allocator<char> allocator_type;

  private:
    // DATA
    allocator_type    d_allocator;  // allocator for object's state
    bslma::Allocator *d_deleter_p;  // allocator for use as a deleter
    void             *d_memory_p;   // dynamically allocated data

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(MyBslAllocArgTestDeleter,
                                   bslma::UsesBslmaAllocator);

    BSLMF_NESTED_TRAIT_DECLARATION(MyBslAllocArgTestDeleter,
                                   bslmf::UsesAllocatorArgT);

    // CREATORS
    MyBslAllocArgTestDeleter(bsl::allocator_arg_t,
                             const allocator_type&  allocator,
                             bslma::Allocator      *deleter);
        // Create a 'MyBslAllocArgTestDeleter' using the specified 'deleter' to
        // destroy objects passed to the overloaded function call operator, and
        // using the specified 'allocator' to allocate some additional state
        // for test purposes only.

    MyBslAllocArgTestDeleter(const MyBslAllocArgTestDeleter& original);
    MyBslAllocArgTestDeleter(bsl::allocator_arg_t,
                             const allocator_type&           allocator,
                             const MyBslAllocArgTestDeleter& original);
        // Create a 'MyBslAllocArgTestDeleter' object having the same deleter
        // as the specified 'original' object, and having a copy of the dummy
        // state.  Optionally specify an 'allocator' used to supply memory.  If
        // 'allocator' is not specified, the currently installed default
        // allocator is used.

    ~MyBslAllocArgTestDeleter();
        // Destroy this object.

    // MANIPULATORS
    MyBslAllocArgTestDeleter& operator=(const MyBslAllocArgTestDeleter& rhs);
        // Assign to this object the deleter of the specified 'rhs'.

    // ACCESSORS
    allocator_type allocator() const;
        // Return the allocator supplied to this object's constructor.

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

  public:
    // CREATORS

    TestSharedPtrRep(TYPE *dataPtr_p, bslma::Allocator *basicAllocator);
        // Construct a test shared ptr rep object owning the object pointed to
        // by the specified 'dataPtr_p', which should be destroyed using the
        // specified 'basicAllocator'.

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

                             // ==================
                             // class NonOwningRep
                             // ==================

template <class TYPE>
class NonOwningRep : public bslma::SharedPtrRep {
    // This 'class' implements a shared pointer representation that does not
    // "own" the lifetime of its shared object, i.e., it will not delete the
    // shared object when the final strong reference is released.  In order to
    // support testing scenarios, this class provides accessors to report the
    // number of times the disposal methods 'disposeObject' and 'disposeRep'
    // have been called.  When a call to 'disposeRep' decrements the number
    // references to zero, this object destroys itself.

    // DATA
    TYPE             *d_dataPtr_p;            // data ptr

    int              *d_disposeObjectCount_p; // counter storing the number of
                                              // times 'releaseValue' is called

    int              *d_disposeRepCount_p;    // counter storing the number of
                                              // times 'release' is called

    bslma::Allocator *d_allocator_p;          // allocator

  public:
    // CREATORS

    NonOwningRep(TYPE            *dataPtr_p,
                int              *disposeObjectCount,
                int              *disposeRepCount,
                bslma::Allocator *basicAllocator = 0);
        // Construct a non-owning shared ptr test-rep object referring to (but
        // not owning) the object pointed to by the specified 'dataPtr_p', and
        // reporting the every call to 'disposeObject' by incrementing the
        // specified 'disposeObjectCount', and reporting every call to the
        // 'disposeRep' method by incrementing the specified 'disposeRepCount'.
        // Optionally specify the 'basicAllocator' that was used to supply the
        // memory that holds this object.  If 'basicAllocator' is 0, the
        // currently installed default allocator is presumed to have been used.
        // Note that this object does not allocate any memory itself.

    ~NonOwningRep();
        // Destroy this test shared ptr rep object.

    // VIRTUAL (OVERRIDE) MANIPULATORS
    virtual void disposeObject();
        // Release the value stored by this representation.

    virtual void disposeRep();
        // Release this representation.

    virtual void *getDeleter(const std::type_info&) { return 0; }
        // Return a null pointer.  Note that this rep type does not support
        // 'shared_ptr' custom deleters.

    // VIRTUAL (OVERRIDE) ACCESSORS
    virtual void *originalPtr() const;
        // Return the original pointer stored by this representation.

    // ACCESSORS
    TYPE *ptr() const;
        // Return the data pointer stored by this representation.
};

                      // ================================
                      // class template PerformanceTester
                      // ================================

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

                              // ===============
                              // class ShareThis
                              // ===============

class ShareThis : public bsl::enable_shared_from_this<ShareThis>
{
    // This class publicly derives from 'bsl::enable_shared_from_this' to
    // support testing of the 'shared_from_this' method.  It is instrumented
    // with a destructor that updates an externally managed integer to track
    // when destruction occurs.

  private:
      // not defined
      ShareThis(const ShareThis&);            // = delete
      ShareThis& operator=(const ShareThis&); // = delete

  protected:
    int *d_destructorCount_p;

  public:
    // CREATORS
    explicit ShareThis(int *destructorCount)
        // Create a 'ShareThis' object using the specified 'destructorCount' to
        // report when this object is destroyed.
        : d_destructorCount_p(destructorCount)
    {
    }

    virtual ~ShareThis()
        // Increment the integer supplied at construction.
    {
        ++*d_destructorCount_p;
    }
};

                           // ======================
                           // class ShareThisDerived
                           // ======================

class ShareThisDerived : public ShareThis
{
    // This class publicly derives from 'ShareThis' to support testing of the
    // 'shared_from_this' method where a base/derived relationship exists
    // between the shared pointer-to-base and a derived object.  It updates the
    // base class instrumented destructor to updates the externally managed
    // integer with a different value when the derived class destructor is run.

  public:
    // CREATORS
    explicit ShareThisDerived(int *destructorCount)
        // Create a 'ShareThisDerived' object using the specified
        // 'destructorCount' to report when this object is destroyed.
        : ShareThis(destructorCount)
    {
    }

    ~ShareThisDerived()
        // Increment by 10 the integer referenced by 'd_destructorCount_p',
        // then destroy this object.  Note that the base class destructor will
        // also increment the same integer, for a combined raise of 11.
    {
        *d_destructorCount_p += 10;
    }
};

                   // ======================================
                   // class NothrowAndBitwiseMovableTestType
                   // ======================================

struct NothrowAndBitwiseMovableTestType {
    // This trivial type specializes the 'bslmf::IsBitwiseMoveable' and
    // 'bsl::is_nothrow_move_constructible' traits to be 'true'.

  private:
    // DATA

    // 'bslmf::IsBitwiseMoveable' has special logic for empty types, so we need
    // to include data to ensure that our tests work as intended.
    BSLA_MAYBE_UNUSED int dummy_d;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(NothrowAndBitwiseMovableTestType,
                                   bslmf::IsBitwiseMoveable);
    BSLMF_NESTED_TRAIT_DECLARATION(NothrowAndBitwiseMovableTestType,
                                   bsl::is_nothrow_move_constructible);

    // CREATORS
    NothrowAndBitwiseMovableTestType(int)
        // Test user defined constructor.
    {
    }

    NothrowAndBitwiseMovableTestType(const NothrowAndBitwiseMovableTestType&)
        // Test user defined copy constructor.
    {
    }

    NothrowAndBitwiseMovableTestType(
     bslmf::MovableRef<NothrowAndBitwiseMovableTestType>) BSLS_KEYWORD_NOEXCEPT
        // Test user defined nothrow move constructor.
    {
    }
};

                // ============================================
                // class NonNothrowAndNonBitwiseMovableTestType
                // ============================================

struct NonNothrowAndNonBitwiseMovableTestType {
    // This trivial type is defined in a way such that the
    // 'bslmf::IsBitwiseMoveable' and 'bsl::is_nothrow_move_constructible'
    // traits are 'false'.

  private:
    // DATA

    // 'bslmf::IsBitwiseMoveable' has special logic for empty types, so we need
    // to include data to ensure that our tests work as intended.
    BSLA_MAYBE_UNUSED int dummy_d;

  public:
    // CREATORS
    NonNothrowAndNonBitwiseMovableTestType(int)
        // Test user defined constructor.
    {
    }

    NonNothrowAndNonBitwiseMovableTestType(
                                 const NonNothrowAndNonBitwiseMovableTestType&)
        // Test user defined copy constructor.
    {
    }
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

template <class TYPE>
struct UsesBslmaAllocator<NonOwningRep<TYPE> >
     : bsl::false_type {};

}  // close namespace bslma
}  // close enterprise namespace

// ============================================================================
//                      MEMBER- AND TEMPLATE-FUNCTION IMPLEMENTATIONS
// ============================================================================

                      // ------------------------------
                      // class NonPolymorphicTestObject
                      // ------------------------------

// CREATORS
NonPolymorphicTestObject::NonPolymorphicTestObject(
                                      const NonPolymorphicTestObject& original)
: NonPolymorphicTestBaseObject()
, d_deleteCounter_p(original.d_deleteCounter_p)
, d_copyCounter_p(original.d_copyCounter_p)
{
    if (d_copyCounter_p) ++(*d_copyCounter_p);
}

NonPolymorphicTestObject::NonPolymorphicTestObject(
                                             bsls::Types::Int64 *deleteCounter,
                                             bsls::Types::Int64 *copyCounter)
: d_deleteCounter_p(deleteCounter)
, d_copyCounter_p(copyCounter)
{
}

NonPolymorphicTestObject::~NonPolymorphicTestObject()
{
    ++(*d_deleteCounter_p);
}

// ACCESSORS
volatile bsls::Types::Int64* NonPolymorphicTestObject::copyCounter() const
{
    return d_copyCounter_p;
}

volatile bsls::Types::Int64* NonPolymorphicTestObject::deleteCounter() const
{
    return d_deleteCounter_p;
}

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

MyTestDerivedObject::MyTestDerivedObject(bsls::Types::Int64 *deleteCounter,
                                         bsls::Types::Int64 *copyCounter)
: MyTestObject(deleteCounter, copyCounter)
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
    return d_allocator_p == rhs.d_allocator_p
        && d_callCount_p == rhs.d_callCount_p;
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

                        // ---------------------------
                        // class MyAllocArgTestDeleter
                        // ---------------------------

// CREATORS
MyAllocArgTestDeleter::MyAllocArgTestDeleter(
                                          bsl::allocator_arg_t,
                                          bslma::Allocator     *basicAllocator,
                                          bslma::Allocator     *deleter)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_deleter_p(deleter)
, d_memory_p(d_allocator_p->allocate(13))
{
}

MyAllocArgTestDeleter::MyAllocArgTestDeleter(
                                         const MyAllocArgTestDeleter& original)
: d_allocator_p(bslma::Default::allocator())
, d_deleter_p(original.d_deleter_p)
, d_memory_p(d_allocator_p->allocate(13))
{
}

MyAllocArgTestDeleter::MyAllocArgTestDeleter(
                                  bsl::allocator_arg_t,
                                  bslma::Allocator             *basicAllocator,
                                  const MyAllocArgTestDeleter&  original)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_deleter_p(original.d_deleter_p)
, d_memory_p(d_allocator_p->allocate(13))
{
}

MyAllocArgTestDeleter::~MyAllocArgTestDeleter()
{
    d_allocator_p->deallocate(d_memory_p);
}

// MANIPULATORS
MyAllocArgTestDeleter& MyAllocArgTestDeleter::operator=(
                                              const MyAllocArgTestDeleter& rhs)
{
    ASSERT(!"'MyAllocArgTestDeleter::operator=(const MyAllocArgTestDeleter&)'"
            " should not be used.");
    d_deleter_p = rhs.d_deleter_p;
    return *this;
}

// ACCESSORS
bslma::Allocator *MyAllocArgTestDeleter::allocator() const
{
    return d_allocator_p;
}

template <class OBJECT_TYPE>
void MyAllocArgTestDeleter::operator()(OBJECT_TYPE *ptr) const
{
    d_deleter_p->deleteObject(ptr);
}

                       // ------------------------------
                       // class MyBslAllocArgTestDeleter
                       // ------------------------------

// CREATORS
MyBslAllocArgTestDeleter::MyBslAllocArgTestDeleter(
                                              bsl::allocator_arg_t,
                                              const allocator_type&  allocator,
                                              bslma::Allocator      *deleter)
: d_allocator(allocator)
, d_deleter_p(deleter)
, d_memory_p(d_allocator.allocate(13))
{
}

MyBslAllocArgTestDeleter::MyBslAllocArgTestDeleter(
                                      const MyBslAllocArgTestDeleter& original)
: d_allocator(allocator_type())
, d_deleter_p(original.d_deleter_p)
, d_memory_p(d_allocator.allocate(13))
{
}

MyBslAllocArgTestDeleter::MyBslAllocArgTestDeleter(
                                     bsl::allocator_arg_t,
                                     const allocator_type&           allocator,
                                     const MyBslAllocArgTestDeleter& original)
: d_allocator(allocator)
, d_deleter_p(original.d_deleter_p)
, d_memory_p(d_allocator.allocate(13))
{
}

MyBslAllocArgTestDeleter::~MyBslAllocArgTestDeleter()
{
    d_allocator.deallocate(static_cast<char *>(d_memory_p));
}

// MANIPULATORS
MyBslAllocArgTestDeleter& MyBslAllocArgTestDeleter::operator=(
                                           const MyBslAllocArgTestDeleter& rhs)
{
    ASSERT(!"'MyBslAllocArgTestDeleter::operator=("
            "const MyBslAllocArgTestDeleter&)' should not be used.");
    d_deleter_p = rhs.d_deleter_p;
    return *this;
}

// ACCESSORS
MyBslAllocArgTestDeleter::allocator_type
MyBslAllocArgTestDeleter::allocator() const
{
    return d_allocator;
}

template <class OBJECT_TYPE>
void MyBslAllocArgTestDeleter::operator()(OBJECT_TYPE *ptr) const
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

                             // ------------------
                             // class NonOwningRep
                             // ------------------

// CREATORS
template <class TYPE>
inline
NonOwningRep<TYPE>::NonOwningRep(TYPE             *dataPtr_p,
                                 int              *disposeObjectCount,
                                 int              *disposeRepCount,
                                 bslma::Allocator *basicAllocator)
: d_dataPtr_p(dataPtr_p)
, d_disposeObjectCount_p(disposeObjectCount)
, d_disposeRepCount_p(disposeRepCount)
, d_allocator_p(basicAllocator)
{
    BSLS_ASSERT_OPT(d_dataPtr_p);
    BSLS_ASSERT_OPT(d_disposeObjectCount_p);
    BSLS_ASSERT_OPT(d_disposeRepCount_p);
    BSLS_ASSERT_OPT(basicAllocator);
}

template <class TYPE>
NonOwningRep<TYPE>::~NonOwningRep()
{
    BSLS_ASSERT_OPT(0 == numReferences());
}

// MANIPULATORS
template <class TYPE>
inline
void NonOwningRep<TYPE>::disposeObject()
{
    ++*d_disposeObjectCount_p;
}

template <class TYPE>
inline
void NonOwningRep<TYPE>::disposeRep()
{
    ++*d_disposeRepCount_p;
    d_allocator_p->deleteObject(this);  // suicide
}

// ACCESSORS
template <class TYPE>
inline
void *NonOwningRep<TYPE>::originalPtr() const
{
    return static_cast<void *>(d_dataPtr_p);
}

template <class TYPE>
inline
TYPE *NonOwningRep<TYPE>::ptr() const
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
    const double               k_BIG_VECTOR_SIZE =
                                          static_cast<double>(BIG_VECTOR_SIZE);
    bsls::Stopwatch            timer;

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
           timer.elapsedTime() / k_BIG_VECTOR_SIZE);
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
           timer.elapsedTime() / (k_BIG_VECTOR_SIZE-1));
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
           timer.elapsedTime() / k_BIG_VECTOR_SIZE);
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
           timer.elapsedTime() / k_BIG_VECTOR_SIZE);
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
           timer.elapsedTime() / k_BIG_VECTOR_SIZE);
    if (verbose) {
        printPerformanceStats(ta.numAllocations() - numAlloc,
                              ta.numBytesInUse() - numBytes,
                              copyCounter,
                              deleteCounter);
    }

    // Note: Z now contains dangling pointers.  Rehydrate!
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
           timer.elapsedTime() / k_BIG_VECTOR_SIZE);
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
           timer.elapsedTime() / k_BIG_VECTOR_SIZE);
    if (verbose) {
        printPerformanceStats(ta.numAllocations() - numAlloc,
                              ta.numBytesInUse() - numBytes,
                              copyCounter,
                              deleteCounter);
    }

    // Note: Z[0] is now dangling, and X contains only empty shared pointers.
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
           timer.elapsedTime() / k_BIG_VECTOR_SIZE);
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
         timer.elapsedTime() / k_BIG_VECTOR_SIZE);
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
               timer.elapsedTime() / k_BIG_VECTOR_SIZE);
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
           timer.elapsedTime() / k_BIG_VECTOR_SIZE);
    if (verbose) {
        printPerformanceStats(ta.numAllocations() - numAlloc,
                              ta.numBytesInUse() - numBytes,
                              copyCounter,
                              deleteCounter);
    }

    // Note: Z[0] is now dangling, and X contains only empty shared pointers.
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
           timer.elapsedTime() / (k_BIG_VECTOR_SIZE+1));
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
           timer.elapsedTime() / k_BIG_VECTOR_SIZE);
    if (verbose) {
        printPerformanceStats(ta.numAllocations() - numAlloc,
                              ta.numBytesInUse() - numBytes,
                              copyCounter,
                              deleteCounter);
    }

    // Note: Z now contains dangling pointers, except Z[0].  Rehydrate!  Note:
    // Z now contains dangling pointers.  Rehydrate!
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


#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)

# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif

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
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic pop
# endif
#endif

                         // ==============================
                         // class AllocPropagationTestType
                         // ==============================

template <bool USES_BSLMA_ALLOC>
class AllocPropagationTestType {
    // This class provides a type to test allocator propagation from its
    // constructor, and declares the 'UsesBsmlaAllocator' trait if the template
    // parameter 'USES_BSLMA_ALLOC' is 'true', and does not declare the trait
    // otherwise.  This class can be used to check that 'make_shared' passes
    // only supplied parameters to the managed object constructor (and does not
    // add pointer to the default allocator in the last position regardless of
    // whether managed object's type defines 'bslma::UsesBslmaAllocator' trait
    // or not.

  private:
    // DATA
    bslma::Allocator *d_allocator_p;  // allocator (held, not owned)

  public:
    // CREATORS
    AllocPropagationTestType(bslma::Allocator *basicAllocator = 0);
    AllocPropagationTestType(MyAllocatableArg01,
                             bslma::Allocator   *basicAllocator = 0);
    AllocPropagationTestType(MyAllocatableArg01,
                             MyAllocatableArg02,
                             bslma::Allocator   *basicAllocator = 0);
    AllocPropagationTestType(MyAllocatableArg01,
                             MyAllocatableArg02,
                             MyAllocatableArg03,
                             bslma::Allocator   *basicAllocator = 0);
    AllocPropagationTestType(MyAllocatableArg01,
                             MyAllocatableArg02,
                             MyAllocatableArg03,
                             MyAllocatableArg04,
                             bslma::Allocator   *basicAllocator = 0);
    AllocPropagationTestType(MyAllocatableArg01,
                             MyAllocatableArg02,
                             MyAllocatableArg03,
                             MyAllocatableArg04,
                             MyAllocatableArg05,
                             bslma::Allocator   *basicAllocator = 0);
    AllocPropagationTestType(MyAllocatableArg01,
                             MyAllocatableArg02,
                             MyAllocatableArg03,
                             MyAllocatableArg04,
                             MyAllocatableArg05,
                             MyAllocatableArg06,
                             bslma::Allocator   *basicAllocator = 0);
    AllocPropagationTestType(MyAllocatableArg01,
                             MyAllocatableArg02,
                             MyAllocatableArg03,
                             MyAllocatableArg04,
                             MyAllocatableArg05,
                             MyAllocatableArg06,
                             MyAllocatableArg07,
                             bslma::Allocator   *basicAllocator = 0);
    AllocPropagationTestType(MyAllocatableArg01,
                             MyAllocatableArg02,
                             MyAllocatableArg03,
                             MyAllocatableArg04,
                             MyAllocatableArg05,
                             MyAllocatableArg06,
                             MyAllocatableArg07,
                             MyAllocatableArg08,
                             bslma::Allocator   *basicAllocator = 0);
    AllocPropagationTestType(MyAllocatableArg01,
                             MyAllocatableArg02,
                             MyAllocatableArg03,
                             MyAllocatableArg04,
                             MyAllocatableArg05,
                             MyAllocatableArg06,
                             MyAllocatableArg07,
                             MyAllocatableArg08,
                             MyAllocatableArg09,
                             bslma::Allocator   *basicAllocator = 0);
    AllocPropagationTestType(MyAllocatableArg01,
                             MyAllocatableArg02,
                             MyAllocatableArg03,
                             MyAllocatableArg04,
                             MyAllocatableArg05,
                             MyAllocatableArg06,
                             MyAllocatableArg07,
                             MyAllocatableArg08,
                             MyAllocatableArg09,
                             MyAllocatableArg10,
                             bslma::Allocator   *basicAllocator = 0);
    AllocPropagationTestType(MyAllocatableArg01,
                             MyAllocatableArg02,
                             MyAllocatableArg03,
                             MyAllocatableArg04,
                             MyAllocatableArg05,
                             MyAllocatableArg06,
                             MyAllocatableArg07,
                             MyAllocatableArg08,
                             MyAllocatableArg09,
                             MyAllocatableArg10,
                             MyAllocatableArg11,
                             bslma::Allocator   *basicAllocator = 0);
    AllocPropagationTestType(MyAllocatableArg01,
                             MyAllocatableArg02,
                             MyAllocatableArg03,
                             MyAllocatableArg04,
                             MyAllocatableArg05,
                             MyAllocatableArg06,
                             MyAllocatableArg07,
                             MyAllocatableArg08,
                             MyAllocatableArg09,
                             MyAllocatableArg10,
                             MyAllocatableArg11,
                             MyAllocatableArg12,
                             bslma::Allocator   *basicAllocator = 0);
    AllocPropagationTestType(MyAllocatableArg01,
                             MyAllocatableArg02,
                             MyAllocatableArg03,
                             MyAllocatableArg04,
                             MyAllocatableArg05,
                             MyAllocatableArg06,
                             MyAllocatableArg07,
                             MyAllocatableArg08,
                             MyAllocatableArg09,
                             MyAllocatableArg10,
                             MyAllocatableArg11,
                             MyAllocatableArg12,
                             MyAllocatableArg13,
                             bslma::Allocator   *basicAllocator = 0);
    AllocPropagationTestType(MyAllocatableArg01,
                             MyAllocatableArg02,
                             MyAllocatableArg03,
                             MyAllocatableArg04,
                             MyAllocatableArg05,
                             MyAllocatableArg06,
                             MyAllocatableArg07,
                             MyAllocatableArg08,
                             MyAllocatableArg09,
                             MyAllocatableArg10,
                             MyAllocatableArg11,
                             MyAllocatableArg12,
                             MyAllocatableArg13,
                             MyAllocatableArg14,
                             bslma::Allocator   *basicAllocator = 0);
        // Create an 'AllocPropagationTestType'.  Optionally specify a
        // 'basicAllocator' used to indicate whether pointer to an allocator is
        // passed in the last position or not.  Note that all other parameters
        // are not used and therefore anonymous.

    //! AllocPropagationTestType(
    //!                    const AllocPropagationTestType& original) = default;

        // Create a test object having the same value as the specified
        // 'original'.

    //! ~AllocPropagationTestType() = default;
        // Destroy this object.

    // MANIPULATORS
    //! AllocPropagationTestType& operator=(
    //!                         const AllocPropagationTestType& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    // ACCESSORS
    bslma::Allocator *allocator() const;
        // Return the allocator used to supply memory for this object.
};


                        // ------------------------------
                        // class AllocPropagationTestType
                        // ------------------------------

// CREATORS
template <bool USES_BSLMA_ALLOC>
AllocPropagationTestType<USES_BSLMA_ALLOC>::AllocPropagationTestType(
                                              bslma::Allocator *basicAllocator)
: d_allocator_p(basicAllocator)
{
}

template <bool USES_BSLMA_ALLOC>
AllocPropagationTestType<USES_BSLMA_ALLOC>::AllocPropagationTestType(
                                            MyAllocatableArg01,
                                            bslma::Allocator   *basicAllocator)
: d_allocator_p(basicAllocator)
{
}

template <bool USES_BSLMA_ALLOC>
AllocPropagationTestType<USES_BSLMA_ALLOC>::AllocPropagationTestType(
                                            MyAllocatableArg01,
                                            MyAllocatableArg02,
                                            bslma::Allocator   *basicAllocator)
: d_allocator_p(basicAllocator)
{
}

template <bool USES_BSLMA_ALLOC>
AllocPropagationTestType<USES_BSLMA_ALLOC>::AllocPropagationTestType(
                                            MyAllocatableArg01,
                                            MyAllocatableArg02,
                                            MyAllocatableArg03,
                                            bslma::Allocator   *basicAllocator)
: d_allocator_p(basicAllocator)
{
}

template <bool USES_BSLMA_ALLOC>
AllocPropagationTestType<USES_BSLMA_ALLOC>::AllocPropagationTestType(
                                            MyAllocatableArg01,
                                            MyAllocatableArg02,
                                            MyAllocatableArg03,
                                            MyAllocatableArg04,
                                            bslma::Allocator   *basicAllocator)
: d_allocator_p(basicAllocator)
{
}

template <bool USES_BSLMA_ALLOC>
AllocPropagationTestType<USES_BSLMA_ALLOC>::AllocPropagationTestType(
                                            MyAllocatableArg01,
                                            MyAllocatableArg02,
                                            MyAllocatableArg03,
                                            MyAllocatableArg04,
                                            MyAllocatableArg05,
                                            bslma::Allocator   *basicAllocator)
: d_allocator_p(basicAllocator)
{
}

template <bool USES_BSLMA_ALLOC>
AllocPropagationTestType<USES_BSLMA_ALLOC>::AllocPropagationTestType(
                                            MyAllocatableArg01,
                                            MyAllocatableArg02,
                                            MyAllocatableArg03,
                                            MyAllocatableArg04,
                                            MyAllocatableArg05,
                                            MyAllocatableArg06,
                                            bslma::Allocator   *basicAllocator)
: d_allocator_p(basicAllocator)
{
}

template <bool USES_BSLMA_ALLOC>
AllocPropagationTestType<USES_BSLMA_ALLOC>::AllocPropagationTestType(
                                            MyAllocatableArg01,
                                            MyAllocatableArg02,
                                            MyAllocatableArg03,
                                            MyAllocatableArg04,
                                            MyAllocatableArg05,
                                            MyAllocatableArg06,
                                            MyAllocatableArg07,
                                            bslma::Allocator   *basicAllocator)
: d_allocator_p(basicAllocator)
{
}

template <bool USES_BSLMA_ALLOC>
AllocPropagationTestType<USES_BSLMA_ALLOC>::AllocPropagationTestType(
                                            MyAllocatableArg01,
                                            MyAllocatableArg02,
                                            MyAllocatableArg03,
                                            MyAllocatableArg04,
                                            MyAllocatableArg05,
                                            MyAllocatableArg06,
                                            MyAllocatableArg07,
                                            MyAllocatableArg08,
                                            bslma::Allocator   *basicAllocator)
: d_allocator_p(basicAllocator)
{
}

template <bool USES_BSLMA_ALLOC>
AllocPropagationTestType<USES_BSLMA_ALLOC>::AllocPropagationTestType(
                                            MyAllocatableArg01,
                                            MyAllocatableArg02,
                                            MyAllocatableArg03,
                                            MyAllocatableArg04,
                                            MyAllocatableArg05,
                                            MyAllocatableArg06,
                                            MyAllocatableArg07,
                                            MyAllocatableArg08,
                                            MyAllocatableArg09,
                                            bslma::Allocator   *basicAllocator)
: d_allocator_p(basicAllocator)
{
}

template <bool USES_BSLMA_ALLOC>
AllocPropagationTestType<USES_BSLMA_ALLOC>::AllocPropagationTestType(
                                            MyAllocatableArg01,
                                            MyAllocatableArg02,
                                            MyAllocatableArg03,
                                            MyAllocatableArg04,
                                            MyAllocatableArg05,
                                            MyAllocatableArg06,
                                            MyAllocatableArg07,
                                            MyAllocatableArg08,
                                            MyAllocatableArg09,
                                            MyAllocatableArg10,
                                            bslma::Allocator   *basicAllocator)
: d_allocator_p(basicAllocator)
{
}

template <bool USES_BSLMA_ALLOC>
AllocPropagationTestType<USES_BSLMA_ALLOC>::AllocPropagationTestType(
                                            MyAllocatableArg01,
                                            MyAllocatableArg02,
                                            MyAllocatableArg03,
                                            MyAllocatableArg04,
                                            MyAllocatableArg05,
                                            MyAllocatableArg06,
                                            MyAllocatableArg07,
                                            MyAllocatableArg08,
                                            MyAllocatableArg09,
                                            MyAllocatableArg10,
                                            MyAllocatableArg11,
                                            bslma::Allocator   *basicAllocator)
: d_allocator_p(basicAllocator)
{
}

template <bool USES_BSLMA_ALLOC>
AllocPropagationTestType<USES_BSLMA_ALLOC>::AllocPropagationTestType(
                                            MyAllocatableArg01,
                                            MyAllocatableArg02,
                                            MyAllocatableArg03,
                                            MyAllocatableArg04,
                                            MyAllocatableArg05,
                                            MyAllocatableArg06,
                                            MyAllocatableArg07,
                                            MyAllocatableArg08,
                                            MyAllocatableArg09,
                                            MyAllocatableArg10,
                                            MyAllocatableArg11,
                                            MyAllocatableArg12,
                                            bslma::Allocator   *basicAllocator)
: d_allocator_p(basicAllocator)
{
}

template <bool USES_BSLMA_ALLOC>
AllocPropagationTestType<USES_BSLMA_ALLOC>::AllocPropagationTestType(
                                            MyAllocatableArg01,
                                            MyAllocatableArg02,
                                            MyAllocatableArg03,
                                            MyAllocatableArg04,
                                            MyAllocatableArg05,
                                            MyAllocatableArg06,
                                            MyAllocatableArg07,
                                            MyAllocatableArg08,
                                            MyAllocatableArg09,
                                            MyAllocatableArg10,
                                            MyAllocatableArg11,
                                            MyAllocatableArg12,
                                            MyAllocatableArg13,
                                            bslma::Allocator   *basicAllocator)
: d_allocator_p(basicAllocator)
{
}

template <bool USES_BSLMA_ALLOC>
AllocPropagationTestType<USES_BSLMA_ALLOC>::AllocPropagationTestType(
                                            MyAllocatableArg01,
                                            MyAllocatableArg02,
                                            MyAllocatableArg03,
                                            MyAllocatableArg04,
                                            MyAllocatableArg05,
                                            MyAllocatableArg06,
                                            MyAllocatableArg07,
                                            MyAllocatableArg08,
                                            MyAllocatableArg09,
                                            MyAllocatableArg10,
                                            MyAllocatableArg11,
                                            MyAllocatableArg12,
                                            MyAllocatableArg13,
                                            MyAllocatableArg14,
                                            bslma::Allocator   *basicAllocator)
: d_allocator_p(basicAllocator)
{
}

// ACCESSORS
template <bool USES_BSLMA_ALLOC>
inline
bslma::Allocator *AllocPropagationTestType<USES_BSLMA_ALLOC>::allocator() const
{
    return d_allocator_p;
}

// TRAITS
namespace BloombergLP {
namespace bslma {

// We need to test 'make_shared' function for classes that define
// 'bslma::UsesBslmaAllocator' trait and for those that do not.  To avoid code
// duplication test class is created as a template and different
// specializations have different trait values.

template <> struct UsesBslmaAllocator<AllocPropagationTestType<true> > :
                                                                 bsl::true_type
{};

template <> struct UsesBslmaAllocator<AllocPropagationTestType<false> > :
                                                                bsl::false_type
{};
}  // close namespace bslma
}  // close enterprise namespace

//=============================================================================
//                      TEST CASE TEMPLATES
//-----------------------------------------------------------------------------

// One allocator or an array of 14?
static bslma::TestAllocator g_argAlloc("global arguments allocator",
                                       veryVeryVeryVerbose);

static const MyTestArg01 V01(1);
static const MyTestArg02 V02(20);
static const MyTestArg03 V03(23);
static const MyTestArg04 V04(44);
static const MyTestArg05 V05(66);
static const MyTestArg06 V06(176);
static const MyTestArg07 V07(878);
static const MyTestArg08 V08(8);
static const MyTestArg09 V09(912);
static const MyTestArg10 V10(102);
static const MyTestArg11 V11(111);
static const MyTestArg12 V12(333);
static const MyTestArg13 V13(712);
static const MyTestArg14 V14(1414);

static const MyAllocatableArg01 VA01(1,    &g_argAlloc);
static const MyAllocatableArg02 VA02(20,   &g_argAlloc);
static const MyAllocatableArg03 VA03(23,   &g_argAlloc);
static const MyAllocatableArg04 VA04(44,   &g_argAlloc);
static const MyAllocatableArg05 VA05(66,   &g_argAlloc);
static const MyAllocatableArg06 VA06(176,  &g_argAlloc);
static const MyAllocatableArg07 VA07(878,  &g_argAlloc);
static const MyAllocatableArg08 VA08(8,    &g_argAlloc);
static const MyAllocatableArg09 VA09(912,  &g_argAlloc);
static const MyAllocatableArg10 VA10(102,  &g_argAlloc);
static const MyAllocatableArg11 VA11(111,  &g_argAlloc);
static const MyAllocatableArg12 VA12(333,  &g_argAlloc);
static const MyAllocatableArg13 VA13(712,  &g_argAlloc);
static const MyAllocatableArg14 VA14(1414, &g_argAlloc);

namespace TestDriver {

template <class TYPE>
void doNotDelete(TYPE *) {}
    // Do nothing

}  // close namespace TestDriver


struct Harness {
    template <class ALLOCATOR>
    static bslma::Allocator *extractBslma(ALLOCATOR basicAllocator) {
        return basicAllocator.allocator();
    }

    template <class ALLOCATOR>
    static bslma::Allocator *extractBslma(ALLOCATOR *basicAllocator) {
        return basicAllocator;
    }

    template <int N_ARGS,
              int N01,
              int N02,
              int N03,
              int N04,
              int N05,
              int N06,
              int N07,
              int N08,
              int N09,
              int N10,
              int N11,
              int N12,
              int N13,
              int N14>
    static void prepareObject(MyInplaceAllocatableObject* target,
                              MyAllocatableArg01&         A01,
                              MyAllocatableArg02&         A02,
                              MyAllocatableArg03&         A03,
                              MyAllocatableArg04&         A04,
                              MyAllocatableArg05&         A05,
                              MyAllocatableArg06&         A06,
                              MyAllocatableArg07&         A07,
                              MyAllocatableArg08&         A08,
                              MyAllocatableArg09&         A09,
                              MyAllocatableArg10&         A10,
                              MyAllocatableArg11&         A11,
                              MyAllocatableArg12&         A12,
                              MyAllocatableArg13&         A13,
                              MyAllocatableArg14&         A14);

    template <int N_ARGS,
              int N01,
              int N02,
              int N03,
              int N04,
              int N05,
              int N06,
              int N07,
              int N08,
              int N09,
              int N10,
              int N11,
              int N12,
              int N13,
              int N14>
    static void testCase23_RunTest(
                     bsl::shared_ptr<const MyInplaceAllocatableObject> target);
        // Implement test case 23 for a single set of arguments to
        // 'createInplace'.  See the test case function for documented concerns
        // and test plan.

    static void testCase23(
                     bsl::shared_ptr<const MyInplaceAllocatableObject> target);
        // Implement test case 23 iterating over all sets of arguments where
        // there is a concern to demonstrate that arguments forward correctly.
        // See the test case function for documented concerns and test plan.

    template <int N_ARGS,
              int N01,
              int N02,
              int N03,
              int N04,
              int N05,
              int N06,
              int N07,
              int N08,
              int N09,
              int N10,
              int N11,
              int N12,
              int N13,
              int N14>
    static void testCase32_DefaultAllocator();
        // Implement test case 32 for the specified (template type parameter)
        // 'ALLOCATOR'.  See the test case function for documented concerns and
        // test plan.

    template <int N_ARGS,
              int N01,
              int N02,
              int N03,
              int N04,
              int N05,
              int N06,
              int N07,
              int N08,
              int N09,
              int N10,
              int N11,
              int N12,
              int N13,
              int N14>
    static void testCase32_LocalAllocator();
        // Implement test case 32 for the specified (template type parameter)
        // 'ALLOCATOR'.  See the test case function for documented concerns and
        // test plan.

    template <int N_ARGS, bool USES_BSLMA_ALLOCATOR>
    static void testCase32_AllocatorPropagation();
        // Check that 'make_shared' passes only supplied parameters to the
        // managed object constructor regardless of whether its type defines
        // the 'UsesBslmaAllocator' trait or not.

    template <class ALLOCATOR>
    static void testCase33(ALLOCATOR basicAllocator);
        // Implement test case 33 for the specified (template type parameter)
        // 'ALLOCATOR'.  See the test case function for documented concerns and
        // test plan.

    template <int N_ARGS,
              int N01,
              int N02,
              int N03,
              int N04,
              int N05,
              int N06,
              int N07,
              int N08,
              int N09,
              int N10,
              int N11,
              int N12,
              int N13,
              int N14,
              class ALLOCATOR>
    static void testCase33_RunTest(ALLOCATOR basicAllocator);
        // Implement test case 33 for the specified (template type parameter)
        // 'ALLOCATOR'.  See the test case function for documented concerns and
        // test plan.

    template <int N_ARGS,
              int N01,
              int N02,
              int N03,
              int N04,
              int N05,
              int N06,
              int N07,
              int N08,
              int N09,
              int N10,
              int N11,
              int N12,
              int N13,
              int N14>
    static void testCase34_AllocatorAware();
        // Implement test case 34 for the specified (template type parameter)
        // 'ALLOCATOR'.  See the test case function for documented concerns and
        // test plan.

    template <class T, class Y>
    static void testCase38(int value);
        // Test 'noexcept' specifications

    template <class T>
    static bslmf::MovableRef<T> testArg(T& t, bsl::true_type );
    template <class T>
    static const T&             testArg(T& t, bsl::false_type);
        // TBD write a contract for these overloads
};

// Inline methods are defined before the remaining class methods.

template <class T>
inline
bslmf::MovableRef<T> Harness::testArg(T& t, bsl::true_type)
{
    return MoveUtil::move(t);
}

template <class T>
inline
const T& Harness::testArg(T& t, bsl::false_type)
{
    return  t;
}


// Remaining class methods for 'Harness'.

template <int N_ARGS,
          int N01,
          int N02,
          int N03,
          int N04,
          int N05,
          int N06,
          int N07,
          int N08,
          int N09,
          int N10,
          int N11,
          int N12,
          int N13,
          int N14>
void Harness::prepareObject(MyInplaceAllocatableObject *target,
                            MyAllocatableArg01&         A01,
                            MyAllocatableArg02&         A02,
                            MyAllocatableArg03&         A03,
                            MyAllocatableArg04&         A04,
                            MyAllocatableArg05&         A05,
                            MyAllocatableArg06&         A06,
                            MyAllocatableArg07&         A07,
                            MyAllocatableArg08&         A08,
                            MyAllocatableArg09&         A09,
                            MyAllocatableArg10&         A10,
                            MyAllocatableArg11&         A11,
                            MyAllocatableArg12&         A12,
                            MyAllocatableArg13&         A13,
                            MyAllocatableArg14&         A14)
{
    bslma::TestAllocator *da =
             dynamic_cast<bslma::TestAllocator *>(bslma::Default::allocator());
    BSLS_ASSERT(da);

    DECLARE_BOOL_CONSTANT(MOVE_01, N01 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_02, N02 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_03, N03 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_04, N04 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_05, N05 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_06, N06 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_07, N07 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_08, N08 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_09, N09 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_10, N10 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_11, N11 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_12, N12 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_13, N13 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_14, N14 == 1);

    switch (N_ARGS) {
      case 0: {
        new(target) MyInplaceAllocatableObject();
      }  break;
      case 1: {
        new(target) MyInplaceAllocatableObject(testArg(A01, MOVE_01));
      }  break;
      case 2: {
        new(target) MyInplaceAllocatableObject(testArg(A01, MOVE_01),
                                               testArg(A02, MOVE_02));
      }  break;
      case 3: {
        new(target) MyInplaceAllocatableObject(testArg(A01, MOVE_01),
                                               testArg(A02, MOVE_02),
                                               testArg(A03, MOVE_03));
      }  break;
      case 4: {
        new(target) MyInplaceAllocatableObject(testArg(A01, MOVE_01),
                                               testArg(A02, MOVE_02),
                                               testArg(A03, MOVE_03),
                                               testArg(A04, MOVE_04));
      }  break;
      case 5: {
        new(target) MyInplaceAllocatableObject(testArg(A01, MOVE_01),
                                               testArg(A02, MOVE_02),
                                               testArg(A03, MOVE_03),
                                               testArg(A04, MOVE_04),
                                               testArg(A05, MOVE_05));
      }  break;
      case 6: {
        new(target) MyInplaceAllocatableObject(testArg(A01, MOVE_01),
                                               testArg(A02, MOVE_02),
                                               testArg(A03, MOVE_03),
                                               testArg(A04, MOVE_04),
                                               testArg(A05, MOVE_05),
                                               testArg(A06, MOVE_06));
      }  break;
      case 7: {
        new(target) MyInplaceAllocatableObject(testArg(A01, MOVE_01),
                                               testArg(A02, MOVE_02),
                                               testArg(A03, MOVE_03),
                                               testArg(A04, MOVE_04),
                                               testArg(A05, MOVE_05),
                                               testArg(A06, MOVE_06),
                                               testArg(A07, MOVE_07));
      }  break;
      case 8: {
        new(target) MyInplaceAllocatableObject(testArg(A01, MOVE_01),
                                               testArg(A02, MOVE_02),
                                               testArg(A03, MOVE_03),
                                               testArg(A04, MOVE_04),
                                               testArg(A05, MOVE_05),
                                               testArg(A06, MOVE_06),
                                               testArg(A07, MOVE_07),
                                               testArg(A08, MOVE_08));
      }  break;
      case 9: {
        new(target) MyInplaceAllocatableObject(testArg(A01, MOVE_01),
                                               testArg(A02, MOVE_02),
                                               testArg(A03, MOVE_03),
                                               testArg(A04, MOVE_04),
                                               testArg(A05, MOVE_05),
                                               testArg(A06, MOVE_06),
                                               testArg(A07, MOVE_07),
                                               testArg(A08, MOVE_08),
                                               testArg(A09, MOVE_09));
      }  break;
      case 10: {
        new(target) MyInplaceAllocatableObject(testArg(A01, MOVE_01),
                                               testArg(A02, MOVE_02),
                                               testArg(A03, MOVE_03),
                                               testArg(A04, MOVE_04),
                                               testArg(A05, MOVE_05),
                                               testArg(A06, MOVE_06),
                                               testArg(A07, MOVE_07),
                                               testArg(A08, MOVE_08),
                                               testArg(A09, MOVE_09),
                                               testArg(A10, MOVE_10));
      }  break;
      case 11: {
        new(target) MyInplaceAllocatableObject(testArg(A01, MOVE_01),
                                               testArg(A02, MOVE_02),
                                               testArg(A03, MOVE_03),
                                               testArg(A04, MOVE_04),
                                               testArg(A05, MOVE_05),
                                               testArg(A06, MOVE_06),
                                               testArg(A07, MOVE_07),
                                               testArg(A08, MOVE_08),
                                               testArg(A09, MOVE_09),
                                               testArg(A10, MOVE_10),
                                               testArg(A11, MOVE_11));
      }  break;
      case 12: {
        new(target) MyInplaceAllocatableObject(testArg(A01, MOVE_01),
                                               testArg(A02, MOVE_02),
                                               testArg(A03, MOVE_03),
                                               testArg(A04, MOVE_04),
                                               testArg(A05, MOVE_05),
                                               testArg(A06, MOVE_06),
                                               testArg(A07, MOVE_07),
                                               testArg(A08, MOVE_08),
                                               testArg(A09, MOVE_09),
                                               testArg(A10, MOVE_10),
                                               testArg(A11, MOVE_11),
                                               testArg(A12, MOVE_12));
      }  break;
      case 13: {
        new(target) MyInplaceAllocatableObject(testArg(A01, MOVE_01),
                                               testArg(A02, MOVE_02),
                                               testArg(A03, MOVE_03),
                                               testArg(A04, MOVE_04),
                                               testArg(A05, MOVE_05),
                                               testArg(A06, MOVE_06),
                                               testArg(A07, MOVE_07),
                                               testArg(A08, MOVE_08),
                                               testArg(A09, MOVE_09),
                                               testArg(A10, MOVE_10),
                                               testArg(A11, MOVE_11),
                                               testArg(A12, MOVE_12),
                                               testArg(A13, MOVE_13));
      }  break;
      case 14: {
        new(target) MyInplaceAllocatableObject(testArg(A01, MOVE_01),
                                               testArg(A02, MOVE_02),
                                               testArg(A03, MOVE_03),
                                               testArg(A04, MOVE_04),
                                               testArg(A05, MOVE_05),
                                               testArg(A06, MOVE_06),
                                               testArg(A07, MOVE_07),
                                               testArg(A08, MOVE_08),
                                               testArg(A09, MOVE_09),
                                               testArg(A10, MOVE_10),
                                               testArg(A11, MOVE_11),
                                               testArg(A12, MOVE_12),
                                               testArg(A13, MOVE_13),
                                               testArg(A14, MOVE_14));
      }  break;
    };

    // The next two blocks are testing 'MyInplaceAllocatableObject', and belong
    // in its own test driver.  Retained for now.

    const MyInplaceAllocatableObject& EXP = *target;

    ASSERTV(VA01, EXP.arg01(), VA01 == EXP.arg01() || 2 == N01);
    ASSERTV(VA02, EXP.arg02(), VA02 == EXP.arg02() || 2 == N02);
    ASSERTV(VA03, EXP.arg03(), VA03 == EXP.arg03() || 2 == N03);
    ASSERTV(VA04, EXP.arg04(), VA04 == EXP.arg04() || 2 == N04);
    ASSERTV(VA05, EXP.arg05(), VA05 == EXP.arg05() || 2 == N05);
    ASSERTV(VA06, EXP.arg06(), VA06 == EXP.arg06() || 2 == N06);
    ASSERTV(VA07, EXP.arg07(), VA07 == EXP.arg07() || 2 == N07);
    ASSERTV(VA08, EXP.arg08(), VA08 == EXP.arg08() || 2 == N08);
    ASSERTV(VA09, EXP.arg09(), VA09 == EXP.arg09() || 2 == N09);
    ASSERTV(VA10, EXP.arg10(), VA10 == EXP.arg10() || 2 == N10);
    ASSERTV(VA11, EXP.arg11(), VA11 == EXP.arg11() || 2 == N11);
    ASSERTV(VA12, EXP.arg12(), VA12 == EXP.arg12() || 2 == N12);
    ASSERTV(VA13, EXP.arg13(), VA13 == EXP.arg13() || 2 == N13);
    ASSERTV(VA14, EXP.arg14(), VA14 == EXP.arg14() || 2 == N14);

    ASSERTV(da, EXP.arg01().allocator(), da == EXP.arg01().allocator());
    ASSERTV(da, EXP.arg02().allocator(), da == EXP.arg02().allocator());
    ASSERTV(da, EXP.arg03().allocator(), da == EXP.arg03().allocator());
    ASSERTV(da, EXP.arg04().allocator(), da == EXP.arg04().allocator());
    ASSERTV(da, EXP.arg05().allocator(), da == EXP.arg05().allocator());
    ASSERTV(da, EXP.arg06().allocator(), da == EXP.arg06().allocator());
    ASSERTV(da, EXP.arg07().allocator(), da == EXP.arg07().allocator());
    ASSERTV(da, EXP.arg08().allocator(), da == EXP.arg08().allocator());
    ASSERTV(da, EXP.arg09().allocator(), da == EXP.arg09().allocator());
    ASSERTV(da, EXP.arg10().allocator(), da == EXP.arg10().allocator());
    ASSERTV(da, EXP.arg11().allocator(), da == EXP.arg11().allocator());
    ASSERTV(da, EXP.arg12().allocator(), da == EXP.arg12().allocator());
    ASSERTV(da, EXP.arg13().allocator(), da == EXP.arg13().allocator());
    ASSERTV(da, EXP.arg14().allocator(), da == EXP.arg14().allocator());
}

template <int N_ARGS,
          int N01,
          int N02,
          int N03,
          int N04,
          int N05,
          int N06,
          int N07,
          int N08,
          int N09,
          int N10,
          int N11,
          int N12,
          int N13,
          int N14>
void Harness::testCase23_RunTest(
                      bsl::shared_ptr<const MyInplaceAllocatableObject> target)
{
    // This function will call itself recursively until it makes a 'shared_ptr'
    // with a shared ownership that survives the 'createInplace' call.  This
    // may be 0, 1, or 2 recursions, depending on the initial 'use_count' of
    // 'target'.

    bslma::TestAllocator *da =
             dynamic_cast<bslma::TestAllocator *>(bslma::Default::allocator());
    BSLS_ASSERT(da);

    DECLARE_BOOL_CONSTANT(MOVE_01, N01 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_02, N02 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_03, N03 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_04, N04 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_05, N05 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_06, N06 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_07, N07 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_08, N08 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_09, N09 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_10, N10 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_11, N11 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_12, N12 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_13, N13 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_14, N14 == 1);

    // First prepare a test object to compare against the shared object
    // constructed by allocate_shared.  In particular, we want to pay attention
    // to the moved-from state of each argument, and the final value of the
    // whole test object.

    bslma::TestAllocator localArgAlloc("local arguments allocator",
                                               veryVeryVeryVerbose);

    // 14 arguments, all using a local test allocator
    MyAllocatableArg01 A01(VA01, &localArgAlloc);
    MyAllocatableArg02 A02(VA02, &localArgAlloc);
    MyAllocatableArg03 A03(VA03, &localArgAlloc);
    MyAllocatableArg04 A04(VA04, &localArgAlloc);
    MyAllocatableArg05 A05(VA05, &localArgAlloc);
    MyAllocatableArg06 A06(VA06, &localArgAlloc);
    MyAllocatableArg07 A07(VA07, &localArgAlloc);
    MyAllocatableArg08 A08(VA08, &localArgAlloc);
    MyAllocatableArg09 A09(VA09, &localArgAlloc);
    MyAllocatableArg10 A10(VA10, &localArgAlloc);
    MyAllocatableArg11 A11(VA11, &localArgAlloc);
    MyAllocatableArg12 A12(VA12, &localArgAlloc);
    MyAllocatableArg13 A13(VA13, &localArgAlloc);
    MyAllocatableArg14 A14(VA14, &localArgAlloc);

    bsls::ObjectBuffer<MyInplaceAllocatableObject> buffer;

    prepareObject<
              N_ARGS,N01,N02,N03,N04,N05,N06,N07,N08,N09,N10,N11,N12,N13,N14>(
    buffer.address(),A01,A02,A03,A04,A05,A06,A07,A08,A09,A10,A11,A12,A13,A14 );

    bslalg::AutoScalarDestructor<MyInplaceAllocatableObject> proctor(
                                                             buffer.address());
    const MyInplaceAllocatableObject& EXP = buffer.object();

    ASSERTV(MOVE_01, A01.movedFrom(),
            MOVE_01 == (MoveState::e_MOVED == A01.movedFrom()));
    ASSERTV(MOVE_02, A02.movedFrom(),
            MOVE_02 == (MoveState::e_MOVED == A02.movedFrom()));
    ASSERTV(MOVE_03, A03.movedFrom(),
            MOVE_03 == (MoveState::e_MOVED == A03.movedFrom()));
    ASSERTV(MOVE_04, A04.movedFrom(),
            MOVE_04 == (MoveState::e_MOVED == A04.movedFrom()));
    ASSERTV(MOVE_05, A05.movedFrom(),
            MOVE_05 == (MoveState::e_MOVED == A05.movedFrom()));
    ASSERTV(MOVE_06, A06.movedFrom(),
            MOVE_06 == (MoveState::e_MOVED == A06.movedFrom()));
    ASSERTV(MOVE_07, A07.movedFrom(),
            MOVE_07 == (MoveState::e_MOVED == A07.movedFrom()));
    ASSERTV(MOVE_08, A08.movedFrom(),
            MOVE_08 == (MoveState::e_MOVED == A08.movedFrom()));
    ASSERTV(MOVE_09, A09.movedFrom(),
            MOVE_09 == (MoveState::e_MOVED == A09.movedFrom()));
    ASSERTV(MOVE_10, A10.movedFrom(),
            MOVE_10 == (MoveState::e_MOVED == A10.movedFrom()));
    ASSERTV(MOVE_11, A11.movedFrom(),
            MOVE_11 == (MoveState::e_MOVED == A11.movedFrom()));
    ASSERTV(MOVE_12, A12.movedFrom(),
            MOVE_12 == (MoveState::e_MOVED == A12.movedFrom()));
    ASSERTV(MOVE_13, A13.movedFrom(),
            MOVE_13 == (MoveState::e_MOVED == A13.movedFrom()));
    ASSERTV(MOVE_14, A14.movedFrom(),
            MOVE_14 == (MoveState::e_MOVED == A14.movedFrom()));

    // Here starts the actual test case
    bslma::TestAllocatorMonitor dam(da);

    bslma::TestAllocator ta("test allocator for createInplace tests",
                            veryVeryVeryVerbose);

    bsls::Types::Int64 numAllocations   = ta.numAllocations();
    bsls::Types::Int64 numDeallocations = ta.numDeallocations();

    {
        bslma::TestAllocator testArgAlloc("test arguments allocator",
                                          veryVeryVeryVerbose);

        // 14 arguments, all using a specific test allocator
        MyAllocatableArg01 B01(VA01, &testArgAlloc);
        MyAllocatableArg02 B02(VA02, &testArgAlloc);
        MyAllocatableArg03 B03(VA03, &testArgAlloc);
        MyAllocatableArg04 B04(VA04, &testArgAlloc);
        MyAllocatableArg05 B05(VA05, &testArgAlloc);
        MyAllocatableArg06 B06(VA06, &testArgAlloc);
        MyAllocatableArg07 B07(VA07, &testArgAlloc);
        MyAllocatableArg08 B08(VA08, &testArgAlloc);
        MyAllocatableArg09 B09(VA09, &testArgAlloc);
        MyAllocatableArg10 B10(VA10, &testArgAlloc);
        MyAllocatableArg11 B11(VA11, &testArgAlloc);
        MyAllocatableArg12 B12(VA12, &testArgAlloc);
        MyAllocatableArg13 B13(VA13, &testArgAlloc);
        MyAllocatableArg14 B14(VA14, &testArgAlloc);

        bsl::shared_ptr<const MyInplaceAllocatableObject>
                                                    mX(MoveUtil::move(target));
        const bsl::shared_ptr<const MyInplaceAllocatableObject>& X = mX;

        long initialUseCount = X.use_count();

        switch (N_ARGS) {
          case 0: {
            mX.createInplace(&ta);
          } break;
          case 1: {
            mX.createInplace(&ta, testArg(B01, MOVE_01));
          } break;
          case 2: {
            mX.createInplace(&ta,
                              testArg(B01, MOVE_01),
                              testArg(B02, MOVE_02));
          } break;
          case 3: {
            mX.createInplace(&ta,
                              testArg(B01, MOVE_01),
                              testArg(B02, MOVE_02),
                              testArg(B03, MOVE_03));
          } break;
          case 4: {
            mX.createInplace(&ta,
                              testArg(B01, MOVE_01),
                              testArg(B02, MOVE_02),
                              testArg(B03, MOVE_03),
                              testArg(B04, MOVE_04));
          } break;
          case 5: {
            mX.createInplace(&ta,
                              testArg(B01, MOVE_01),
                              testArg(B02, MOVE_02),
                              testArg(B03, MOVE_03),
                              testArg(B04, MOVE_04),
                              testArg(B05, MOVE_05));
          } break;
          case 6: {
            mX.createInplace(&ta,
                              testArg(B01, MOVE_01),
                              testArg(B02, MOVE_02),
                              testArg(B03, MOVE_03),
                              testArg(B04, MOVE_04),
                              testArg(B05, MOVE_05),
                              testArg(B06, MOVE_06));
          } break;
          case 7: {
            mX.createInplace(&ta,
                              testArg(B01, MOVE_01),
                              testArg(B02, MOVE_02),
                              testArg(B03, MOVE_03),
                              testArg(B04, MOVE_04),
                              testArg(B05, MOVE_05),
                              testArg(B06, MOVE_06),
                              testArg(B07, MOVE_07));
          } break;
          case 8: {
            mX.createInplace(&ta,
                              testArg(B01, MOVE_01),
                              testArg(B02, MOVE_02),
                              testArg(B03, MOVE_03),
                              testArg(B04, MOVE_04),
                              testArg(B05, MOVE_05),
                              testArg(B06, MOVE_06),
                              testArg(B07, MOVE_07),
                              testArg(B08, MOVE_08));
          } break;
          case 9: {
            mX.createInplace(&ta,
                              testArg(B01, MOVE_01),
                              testArg(B02, MOVE_02),
                              testArg(B03, MOVE_03),
                              testArg(B04, MOVE_04),
                              testArg(B05, MOVE_05),
                              testArg(B06, MOVE_06),
                              testArg(B07, MOVE_07),
                              testArg(B08, MOVE_08),
                              testArg(B09, MOVE_09));
          } break;
          case 10: {
            mX.createInplace(&ta,
                              testArg(B01, MOVE_01),
                              testArg(B02, MOVE_02),
                              testArg(B03, MOVE_03),
                              testArg(B04, MOVE_04),
                              testArg(B05, MOVE_05),
                              testArg(B06, MOVE_06),
                              testArg(B07, MOVE_07),
                              testArg(B08, MOVE_08),
                              testArg(B09, MOVE_09),
                              testArg(B10, MOVE_10));
          } break;
          case 11: {
            mX.createInplace(&ta,
                              testArg(B01, MOVE_01),
                              testArg(B02, MOVE_02),
                              testArg(B03, MOVE_03),
                              testArg(B04, MOVE_04),
                              testArg(B05, MOVE_05),
                              testArg(B06, MOVE_06),
                              testArg(B07, MOVE_07),
                              testArg(B08, MOVE_08),
                              testArg(B09, MOVE_09),
                              testArg(B10, MOVE_10),
                              testArg(B11, MOVE_11));
          } break;
          case 12: {
            mX.createInplace(&ta,
                              testArg(B01, MOVE_01),
                              testArg(B02, MOVE_02),
                              testArg(B03, MOVE_03),
                              testArg(B04, MOVE_04),
                              testArg(B05, MOVE_05),
                              testArg(B06, MOVE_06),
                              testArg(B07, MOVE_07),
                              testArg(B08, MOVE_08),
                              testArg(B09, MOVE_09),
                              testArg(B10, MOVE_10),
                              testArg(B11, MOVE_11),
                              testArg(B12, MOVE_12));
          } break;
          case 13: {
            mX.createInplace(&ta,
                              testArg(B01, MOVE_01),
                              testArg(B02, MOVE_02),
                              testArg(B03, MOVE_03),
                              testArg(B04, MOVE_04),
                              testArg(B05, MOVE_05),
                              testArg(B06, MOVE_06),
                              testArg(B07, MOVE_07),
                              testArg(B08, MOVE_08),
                              testArg(B09, MOVE_09),
                              testArg(B10, MOVE_10),
                              testArg(B11, MOVE_11),
                              testArg(B12, MOVE_12),
                              testArg(B13, MOVE_13));
          } break;
          case 14: {
            mX.createInplace(&ta,
                              testArg(B01, MOVE_01),
                              testArg(B02, MOVE_02),
                              testArg(B03, MOVE_03),
                              testArg(B04, MOVE_04),
                              testArg(B05, MOVE_05),
                              testArg(B06, MOVE_06),
                              testArg(B07, MOVE_07),
                              testArg(B08, MOVE_08),
                              testArg(B09, MOVE_09),
                              testArg(B10, MOVE_10),
                              testArg(B11, MOVE_11),
                              testArg(B12, MOVE_12),
                              testArg(B13, MOVE_13),
                              testArg(B14, MOVE_14));
          } break;
        };

        ASSERT(++numAllocations == ta.numAllocations());
        ASSERT(1 == X.use_count());
        ASSERT(X.get());
        ASSERT(EXP == *X);
        ASSERTV(da, X->allocator(), da == X->allocator() );

        ASSERTV(A01.movedFrom(),   B01.movedFrom(),
                A01.movedFrom() == B01.movedFrom());
        ASSERTV(A02.movedFrom(),   B02.movedFrom(),
                A02.movedFrom() == B02.movedFrom());
        ASSERTV(A03.movedFrom(),   B03.movedFrom(),
                A03.movedFrom() == B03.movedFrom());
        ASSERTV(A04.movedFrom(),   B04.movedFrom(),
                A04.movedFrom() == B04.movedFrom());
        ASSERTV(A05.movedFrom(),   B05.movedFrom(),
                A05.movedFrom() == B05.movedFrom());
        ASSERTV(A06.movedFrom(),   B06.movedFrom(),
                A06.movedFrom() == B06.movedFrom());
        ASSERTV(A07.movedFrom(),   B07.movedFrom(),
                A07.movedFrom() == B07.movedFrom());
        ASSERTV(A08.movedFrom(),   B08.movedFrom(),
                A08.movedFrom() == B08.movedFrom());
        ASSERTV(A09.movedFrom(),   B09.movedFrom(),
                A09.movedFrom() == B09.movedFrom());
        ASSERTV(A10.movedFrom(),   B10.movedFrom(),
                A10.movedFrom() == B10.movedFrom());
        ASSERTV(A11.movedFrom(),   B11.movedFrom(),
                A11.movedFrom() == B11.movedFrom());
        ASSERTV(A12.movedFrom(),   B12.movedFrom(),
                A12.movedFrom() == B12.movedFrom());
        ASSERTV(A13.movedFrom(),   B13.movedFrom(),
                A13.movedFrom() == B13.movedFrom());
        ASSERTV(A14.movedFrom(),   B14.movedFrom(),
                A14.movedFrom() == B14.movedFrom());

        if (1 == initialUseCount) {
            // TBD: confirm that the original shared object was destroyed, that
            // the 'rep' object was de-allocated, and that de-allocation was
            // performed with the correct allocator.
        }

        if (initialUseCount < 2) {
            testCase23_RunTest<N_ARGS,
                               N01, N02, N03, N04, N05, N06, N07,
                               N08, N09, N10, N11, N12, N13, N14>(X);
        }
    }
    ASSERT(++numDeallocations == ta.numDeallocations());

    ASSERT(dam.isInUseSame());
}

void Harness::testCase23(
                      bsl::shared_ptr<const MyInplaceAllocatableObject> target)
{
    // --------------------------------------------------------------------
    // TESTING 'createInplace(A, ...)'
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

#if !defined(BSL_DO_NOT_TEST_MOVE_FORWARDING)
    if (verbose) printf("\nTesting 'createInplace' with 0 arguments"
                        "\n----------------------------------------\n");

    testCase23_RunTest<0,2,2,2,2,2,2,2,2,2,2,2,2,2,2>(target);

    if (verbose) printf("\nTesting 'createInplace' with 1 argument"
                        "\n---------------------------------------\n");

    testCase23_RunTest<1,0,2,2,2,2,2,2,2,2,2,2,2,2,2>(target);
    testCase23_RunTest<1,1,2,2,2,2,2,2,2,2,2,2,2,2,2>(target);

    if (verbose) printf("\nTesting 'createInplace' with 2 arguments"
                        "\n----------------------------------------\n");

    testCase23_RunTest<2,0,0,2,2,2,2,2,2,2,2,2,2,2,2>(target);
    testCase23_RunTest<2,1,0,2,2,2,2,2,2,2,2,2,2,2,2>(target);
    testCase23_RunTest<2,0,1,2,2,2,2,2,2,2,2,2,2,2,2>(target);
    testCase23_RunTest<2,1,1,2,2,2,2,2,2,2,2,2,2,2,2>(target);

    if (verbose) printf("\nTesting 'createInplace' with 3 arguments"
                        "\n----------------------------------------\n");

    testCase23_RunTest<3,0,0,0,2,2,2,2,2,2,2,2,2,2,2>(target);
    testCase23_RunTest<3,1,0,0,2,2,2,2,2,2,2,2,2,2,2>(target);
    testCase23_RunTest<3,0,1,0,2,2,2,2,2,2,2,2,2,2,2>(target);
    testCase23_RunTest<3,0,0,1,2,2,2,2,2,2,2,2,2,2,2>(target);
    testCase23_RunTest<3,1,1,1,2,2,2,2,2,2,2,2,2,2,2>(target);

# if !defined(BSLSTL_SHAREDPTR_LIMIT_TESTING_COMPLEXITY)
    if (verbose) printf("\nTesting 'createInplace' with 4 arguments"
                        "\n----------------------------------------\n");

    testCase23_RunTest<4,0,0,0,0,2,2,2,2,2,2,2,2,2,2>(target);
    testCase23_RunTest<4,1,0,0,0,2,2,2,2,2,2,2,2,2,2>(target);
    testCase23_RunTest<4,0,1,0,0,2,2,2,2,2,2,2,2,2,2>(target);
    testCase23_RunTest<4,0,0,1,0,2,2,2,2,2,2,2,2,2,2>(target);
    testCase23_RunTest<4,0,0,0,1,2,2,2,2,2,2,2,2,2,2>(target);
    testCase23_RunTest<4,1,1,1,1,2,2,2,2,2,2,2,2,2,2>(target);

    if (verbose) printf("\nTesting 'createInplace' with 5 arguments"
                        "\n----------------------------------------\n");

    testCase23_RunTest<5,0,0,0,0,0,2,2,2,2,2,2,2,2,2>(target);
    testCase23_RunTest<5,1,0,0,0,0,2,2,2,2,2,2,2,2,2>(target);
    testCase23_RunTest<5,0,1,0,0,0,2,2,2,2,2,2,2,2,2>(target);
    testCase23_RunTest<5,0,0,1,0,0,2,2,2,2,2,2,2,2,2>(target);
    testCase23_RunTest<5,0,0,0,1,0,2,2,2,2,2,2,2,2,2>(target);
    testCase23_RunTest<5,0,0,0,0,1,2,2,2,2,2,2,2,2,2>(target);
    testCase23_RunTest<5,1,1,1,1,1,2,2,2,2,2,2,2,2,2>(target);

    if (verbose) printf("\nTesting 'createInplace' with 6 arguments"
                        "\n----------------------------------------\n");

    testCase23_RunTest<6,0,0,0,0,0,0,2,2,2,2,2,2,2,2>(target);
    testCase23_RunTest<6,1,0,0,0,0,0,2,2,2,2,2,2,2,2>(target);
    testCase23_RunTest<6,0,1,0,0,0,0,2,2,2,2,2,2,2,2>(target);
    testCase23_RunTest<6,0,0,1,0,0,0,2,2,2,2,2,2,2,2>(target);
    testCase23_RunTest<6,0,0,0,1,0,0,2,2,2,2,2,2,2,2>(target);
    testCase23_RunTest<6,0,0,0,0,1,0,2,2,2,2,2,2,2,2>(target);
    testCase23_RunTest<6,0,0,0,0,0,1,2,2,2,2,2,2,2,2>(target);
    testCase23_RunTest<6,1,1,1,1,1,1,2,2,2,2,2,2,2,2>(target);

    if (verbose) printf("\nTesting 'createInplace' with 7 arguments"
                        "\n----------------------------------------\n");

    testCase23_RunTest<7,0,0,0,0,0,0,0,2,2,2,2,2,2,2>(target);
    testCase23_RunTest<7,1,0,0,0,0,0,0,2,2,2,2,2,2,2>(target);
    testCase23_RunTest<7,0,1,0,0,0,0,0,2,2,2,2,2,2,2>(target);
    testCase23_RunTest<7,0,0,1,0,0,0,0,2,2,2,2,2,2,2>(target);
    testCase23_RunTest<7,0,0,0,1,0,0,0,2,2,2,2,2,2,2>(target);
    testCase23_RunTest<7,0,0,0,0,1,0,0,2,2,2,2,2,2,2>(target);
    testCase23_RunTest<7,0,0,0,0,0,1,0,2,2,2,2,2,2,2>(target);
    testCase23_RunTest<7,0,0,0,0,0,0,1,2,2,2,2,2,2,2>(target);
    testCase23_RunTest<7,1,1,1,1,1,1,1,2,2,2,2,2,2,2>(target);

    if (verbose) printf("\nTesting 'createInplace' with 8 arguments"
                        "\n----------------------------------------\n");

    testCase23_RunTest<8,0,0,0,0,0,0,0,0,2,2,2,2,2,2>(target);
    testCase23_RunTest<8,1,0,0,0,0,0,0,0,2,2,2,2,2,2>(target);
    testCase23_RunTest<8,0,1,0,0,0,0,0,0,2,2,2,2,2,2>(target);
    testCase23_RunTest<8,0,0,1,0,0,0,0,0,2,2,2,2,2,2>(target);
    testCase23_RunTest<8,0,0,0,1,0,0,0,0,2,2,2,2,2,2>(target);
    testCase23_RunTest<8,0,0,0,0,1,0,0,0,2,2,2,2,2,2>(target);
    testCase23_RunTest<8,0,0,0,0,0,1,0,0,2,2,2,2,2,2>(target);
    testCase23_RunTest<8,0,0,0,0,0,0,1,0,2,2,2,2,2,2>(target);
    testCase23_RunTest<8,0,0,0,0,0,0,0,1,2,2,2,2,2,2>(target);
    testCase23_RunTest<8,1,1,1,1,1,1,1,1,2,2,2,2,2,2>(target);

    if (verbose) printf("\nTesting 'createInplace' with 9 arguments"
                        "\n-------------....-----------------------\n");

    testCase23_RunTest<9,0,0,0,0,0,0,0,0,0,2,2,2,2,2>(target);
    testCase23_RunTest<9,1,0,0,0,0,0,0,0,0,2,2,2,2,2>(target);
    testCase23_RunTest<9,0,1,0,0,0,0,0,0,0,2,2,2,2,2>(target);
    testCase23_RunTest<9,0,0,1,0,0,0,0,0,0,2,2,2,2,2>(target);
    testCase23_RunTest<9,0,0,0,1,0,0,0,0,0,2,2,2,2,2>(target);
    testCase23_RunTest<9,0,0,0,0,1,0,0,0,0,2,2,2,2,2>(target);
    testCase23_RunTest<9,0,0,0,0,0,1,0,0,0,2,2,2,2,2>(target);
    testCase23_RunTest<9,0,0,0,0,0,0,1,0,0,2,2,2,2,2>(target);
    testCase23_RunTest<9,0,0,0,0,0,0,0,1,0,2,2,2,2,2>(target);
    testCase23_RunTest<9,0,0,0,0,0,0,0,0,1,2,2,2,2,2>(target);
    testCase23_RunTest<9,1,1,1,1,1,1,1,1,1,2,2,2,2,2>(target);

    if (verbose) printf("\nTesting 'createInplace' with 10 arguments"
                        "\n-----------------....--------------------\n");

    testCase23_RunTest<10,0,0,0,0,0,0,0,0,0,0,2,2,2,2>(target);
    testCase23_RunTest<10,1,0,0,0,0,0,0,0,0,0,2,2,2,2>(target);
    testCase23_RunTest<10,0,1,0,0,0,0,0,0,0,0,2,2,2,2>(target);
    testCase23_RunTest<10,0,0,1,0,0,0,0,0,0,0,2,2,2,2>(target);
    testCase23_RunTest<10,0,0,0,1,0,0,0,0,0,0,2,2,2,2>(target);
    testCase23_RunTest<10,0,0,0,0,1,0,0,0,0,0,2,2,2,2>(target);
    testCase23_RunTest<10,0,0,0,0,0,1,0,0,0,0,2,2,2,2>(target);
    testCase23_RunTest<10,0,0,0,0,0,0,1,0,0,0,2,2,2,2>(target);
    testCase23_RunTest<10,0,0,0,0,0,0,0,1,0,0,2,2,2,2>(target);
    testCase23_RunTest<10,0,0,0,0,0,0,0,0,1,0,2,2,2,2>(target);
    testCase23_RunTest<10,0,0,0,0,0,0,0,0,0,1,2,2,2,2>(target);
    testCase23_RunTest<10,1,1,1,1,1,1,1,1,1,1,2,2,2,2>(target);

    if (verbose) printf("\nTesting 'createInplace' with 11 arguments"
                        "\n------------------....-------------------\n");

    testCase23_RunTest<11,0,0,0,0,0,0,0,0,0,0,0,2,2,2>(target);
    testCase23_RunTest<11,1,0,0,0,0,0,0,0,0,0,0,2,2,2>(target);
    testCase23_RunTest<11,0,1,0,0,0,0,0,0,0,0,0,2,2,2>(target);
    testCase23_RunTest<11,0,0,1,0,0,0,0,0,0,0,0,2,2,2>(target);
    testCase23_RunTest<11,0,0,0,1,0,0,0,0,0,0,0,2,2,2>(target);
    testCase23_RunTest<11,0,0,0,0,1,0,0,0,0,0,0,2,2,2>(target);
    testCase23_RunTest<11,0,0,0,0,0,1,0,0,0,0,0,2,2,2>(target);
    testCase23_RunTest<11,0,0,0,0,0,0,1,0,0,0,0,2,2,2>(target);
    testCase23_RunTest<11,0,0,0,0,0,0,0,1,0,0,0,2,2,2>(target);
    testCase23_RunTest<11,0,0,0,0,0,0,0,0,1,0,0,2,2,2>(target);
    testCase23_RunTest<11,0,0,0,0,0,0,0,0,0,1,0,2,2,2>(target);
    testCase23_RunTest<11,0,0,0,0,0,0,0,0,0,0,1,2,2,2>(target);
    testCase23_RunTest<11,1,1,1,1,1,1,1,1,1,1,1,2,2,2>(target);

    if (verbose) printf("\nTesting 'createInplace' with 12 arguments"
                        "\n-------------------....------------------\n");

    testCase23_RunTest<12,0,0,0,0,0,0,0,0,0,0,0,0,2,2>(target);
    testCase23_RunTest<12,1,0,0,0,0,0,0,0,0,0,0,0,2,2>(target);
    testCase23_RunTest<12,0,1,0,0,0,0,0,0,0,0,0,0,2,2>(target);
    testCase23_RunTest<12,0,0,1,0,0,0,0,0,0,0,0,0,2,2>(target);
    testCase23_RunTest<12,0,0,0,1,0,0,0,0,0,0,0,0,2,2>(target);
    testCase23_RunTest<12,0,0,0,0,1,0,0,0,0,0,0,0,2,2>(target);
    testCase23_RunTest<12,0,0,0,0,0,1,0,0,0,0,0,0,2,2>(target);
    testCase23_RunTest<12,0,0,0,0,0,0,1,0,0,0,0,0,2,2>(target);
    testCase23_RunTest<12,0,0,0,0,0,0,0,1,0,0,0,0,2,2>(target);
    testCase23_RunTest<12,0,0,0,0,0,0,0,0,1,0,0,0,2,2>(target);
    testCase23_RunTest<12,0,0,0,0,0,0,0,0,0,1,0,0,2,2>(target);
    testCase23_RunTest<12,0,0,0,0,0,0,0,0,0,0,1,0,2,2>(target);
    testCase23_RunTest<12,0,0,0,0,0,0,0,0,0,0,0,1,2,2>(target);
    testCase23_RunTest<12,1,1,1,1,1,1,1,1,1,1,1,1,2,2>(target);

    if (verbose) printf("\nTesting 'createInplace' with 13 arguments"
                        "\n-----------------....--------------------\n");

    testCase23_RunTest<13,0,0,0,0,0,0,0,0,0,0,0,0,0,2>(target);
    testCase23_RunTest<13,1,0,0,0,0,0,0,0,0,0,0,0,0,2>(target);
    testCase23_RunTest<13,0,1,0,0,0,0,0,0,0,0,0,0,0,2>(target);
    testCase23_RunTest<13,0,0,1,0,0,0,0,0,0,0,0,0,0,2>(target);
    testCase23_RunTest<13,0,0,0,1,0,0,0,0,0,0,0,0,0,2>(target);
    testCase23_RunTest<13,0,0,0,0,1,0,0,0,0,0,0,0,0,2>(target);
    testCase23_RunTest<13,0,0,0,0,0,1,0,0,0,0,0,0,0,2>(target);
    testCase23_RunTest<13,0,0,0,0,0,0,1,0,0,0,0,0,0,2>(target);
    testCase23_RunTest<13,0,0,0,0,0,0,0,1,0,0,0,0,0,2>(target);
    testCase23_RunTest<13,0,0,0,0,0,0,0,0,1,0,0,0,0,2>(target);
    testCase23_RunTest<13,0,0,0,0,0,0,0,0,0,1,0,0,0,2>(target);
    testCase23_RunTest<13,0,0,0,0,0,0,0,0,0,0,1,0,0,2>(target);
    testCase23_RunTest<13,0,0,0,0,0,0,0,0,0,0,0,1,0,2>(target);
    testCase23_RunTest<13,0,0,0,0,0,0,0,0,0,0,0,0,1,2>(target);
    testCase23_RunTest<13,1,1,1,1,1,1,1,1,1,1,1,1,1,2>(target);
# else // BSLSTL_SHAREDPTR_LIMIT_TESTING_COMPLEXITY)
    if (verbose) printf("\nTesting 'createInplace' with 4 arguments"
                        "\n----------------------------------------\n");

    testCase23_RunTest<4,0,0,0,0,2,2,2,2,2,2,2,2,2,2>(target);
    testCase23_RunTest<4,1,1,1,1,2,2,2,2,2,2,2,2,2,2>(target);

    if (verbose) printf("\nTesting 'createInplace' with 5 arguments"
                        "\n----------------------------------------\n");

    testCase23_RunTest<5,0,0,0,0,0,2,2,2,2,2,2,2,2,2>(target);
    testCase23_RunTest<5,1,1,1,1,1,2,2,2,2,2,2,2,2,2>(target);

    if (verbose) printf("\nTesting 'createInplace' with 6 arguments"
                        "\n----------------------------------------\n");

    testCase23_RunTest<6,0,0,0,0,0,0,2,2,2,2,2,2,2,2>(target);
    testCase23_RunTest<6,1,1,1,1,1,1,2,2,2,2,2,2,2,2>(target);

    if (verbose) printf("\nTesting 'createInplace' with 7 arguments"
                        "\n----------------------------------------\n");

    testCase23_RunTest<7,0,0,0,0,0,0,0,2,2,2,2,2,2,2>(target);
    testCase23_RunTest<7,1,1,1,1,1,1,1,2,2,2,2,2,2,2>(target);

    if (verbose) printf("\nTesting 'createInplace' with 8 arguments"
                        "\n----------------------------------------\n");

    testCase23_RunTest<8,0,0,0,0,0,0,0,0,2,2,2,2,2,2>(target);
    testCase23_RunTest<8,1,1,1,1,1,1,1,1,2,2,2,2,2,2>(target);

    if (verbose) printf("\nTesting 'createInplace' with 9 arguments"
                        "\n-------------....-----------------------\n");

    testCase23_RunTest<9,0,0,0,0,0,0,0,0,0,2,2,2,2,2>(target);
    testCase23_RunTest<9,1,1,1,1,1,1,1,1,1,2,2,2,2,2>(target);

    if (verbose) printf("\nTesting 'createInplace' with 10 arguments"
                        "\n-----------------....--------------------\n");

    testCase23_RunTest<10,0,0,0,0,0,0,0,0,0,0,2,2,2,2>(target);
    testCase23_RunTest<10,1,1,1,1,1,1,1,1,1,1,2,2,2,2>(target);

    if (verbose) printf("\nTesting 'createInplace' with 11 arguments"
                        "\n------------------....-------------------\n");

    testCase23_RunTest<11,0,0,0,0,0,0,0,0,0,0,0,2,2,2>(target);
    testCase23_RunTest<11,1,1,1,1,1,1,1,1,1,1,1,2,2,2>(target);

    if (verbose) printf("\nTesting 'createInplace' with 12 arguments"
                        "\n-------------------....------------------\n");

    testCase23_RunTest<12,0,0,0,0,0,0,0,0,0,0,0,0,2,2>(target);
    testCase23_RunTest<12,1,1,1,1,1,1,1,1,1,1,1,1,2,2>(target);

    if (verbose) printf("\nTesting 'createInplace' with 13 arguments"
                        "\n-----------------....--------------------\n");

    testCase23_RunTest<13,0,0,0,0,0,0,0,0,0,0,0,0,0,2>(target);
    testCase23_RunTest<13,1,1,1,1,1,1,1,1,1,1,1,1,1,2>(target);
# endif // BSLSTL_SHAREDPTR_LIMIT_TESTING_COMPLEXITY)

    if (verbose) printf("\nTesting 'createInplace' with 14 arguments"
                        "\n-----------------------------------------\n");

    testCase23_RunTest<14,0,0,0,0,0,0,0,0,0,0,0,0,0,0>(target);
    testCase23_RunTest<14,1,0,0,0,0,0,0,0,0,0,0,0,0,0>(target);
    testCase23_RunTest<14,0,1,0,0,0,0,0,0,0,0,0,0,0,0>(target);
    testCase23_RunTest<14,0,0,1,0,0,0,0,0,0,0,0,0,0,0>(target);
    testCase23_RunTest<14,0,0,0,1,0,0,0,0,0,0,0,0,0,0>(target);
    testCase23_RunTest<14,0,0,0,0,1,0,0,0,0,0,0,0,0,0>(target);
    testCase23_RunTest<14,0,0,0,0,0,1,0,0,0,0,0,0,0,0>(target);
    testCase23_RunTest<14,0,0,0,0,0,0,1,0,0,0,0,0,0,0>(target);
    testCase23_RunTest<14,0,0,0,0,0,0,0,1,0,0,0,0,0,0>(target);
    testCase23_RunTest<14,0,0,0,0,0,0,0,0,1,0,0,0,0,0>(target);
    testCase23_RunTest<14,0,0,0,0,0,0,0,0,0,1,0,0,0,0>(target);
    testCase23_RunTest<14,0,0,0,0,0,0,0,0,0,0,1,0,0,0>(target);
    testCase23_RunTest<14,0,0,0,0,0,0,0,0,0,0,0,1,0,0>(target);
    testCase23_RunTest<14,0,0,0,0,0,0,0,0,0,0,0,0,1,0>(target);
    testCase23_RunTest<14,0,0,0,0,0,0,0,0,0,0,0,0,0,1>(target);
    testCase23_RunTest<14,1,1,1,1,1,1,1,1,1,1,1,1,1,1>(target);
#else // BSL_DO_NOT_TEST_MOVE_FORWARDING
    testCase23_RunTest< 0,2,2,2,2,2,2,2,2,2,2,2,2,2,2>(target);
    testCase23_RunTest< 1,0,2,2,2,2,2,2,2,2,2,2,2,2,2>(target);
    testCase23_RunTest< 2,0,0,2,2,2,2,2,2,2,2,2,2,2,2>(target);
    testCase23_RunTest< 3,0,0,0,2,2,2,2,2,2,2,2,2,2,2>(target);
    testCase23_RunTest< 4,0,0,0,0,2,2,2,2,2,2,2,2,2,2>(target);
    testCase23_RunTest< 5,0,0,0,0,0,2,2,2,2,2,2,2,2,2>(target);
    testCase23_RunTest< 6,0,0,0,0,0,0,2,2,2,2,2,2,2,2>(target);
    testCase23_RunTest< 7,0,0,0,0,0,0,0,2,2,2,2,2,2,2>(target);
    testCase23_RunTest< 8,0,0,0,0,0,0,0,0,2,2,2,2,2,2>(target);
    testCase23_RunTest< 9,0,0,0,0,0,0,0,0,0,2,2,2,2,2>(target);
    testCase23_RunTest<10,0,0,0,0,0,0,0,0,0,0,2,2,2,2>(target);
    testCase23_RunTest<11,0,0,0,0,0,0,0,0,0,0,0,2,2,2>(target);
    testCase23_RunTest<12,0,0,0,0,0,0,0,0,0,0,0,0,2,2>(target);
    testCase23_RunTest<13,0,0,0,0,0,0,0,0,0,0,0,0,0,2>(target);
    testCase23_RunTest<14,0,0,0,0,0,0,0,0,0,0,0,0,0,0>(target);
#endif // BSL_DO_NOT_TEST_MOVE_FORWARDING
}

template <int N_ARGS,
          int N01,
          int N02,
          int N03,
          int N04,
          int N05,
          int N06,
          int N07,
          int N08,
          int N09,
          int N10,
          int N11,
          int N12,
          int N13,
          int N14>
void Harness::testCase32_DefaultAllocator()
{
    bslma::TestAllocator *da =
             dynamic_cast<bslma::TestAllocator *>(bslma::Default::allocator());
    BSLS_ASSERT(da);

    DECLARE_BOOL_CONSTANT(MOVE_01, N01 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_02, N02 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_03, N03 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_04, N04 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_05, N05 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_06, N06 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_07, N07 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_08, N08 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_09, N09 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_10, N10 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_11, N11 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_12, N12 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_13, N13 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_14, N14 == 1);

    // First prepare a test object to compare against the shared object
    // constructed by allocate_shared.  In particular, we want to pay attention
    // to the moved-from state of each argument, and the final value of the
    // whole test object.

    // 14 arguments, all using a local test allocator
    MyAllocatableArg01 A01(VA01);
    MyAllocatableArg02 A02(VA02);
    MyAllocatableArg03 A03(VA03);
    MyAllocatableArg04 A04(VA04);
    MyAllocatableArg05 A05(VA05);
    MyAllocatableArg06 A06(VA06);
    MyAllocatableArg07 A07(VA07);
    MyAllocatableArg08 A08(VA08);
    MyAllocatableArg09 A09(VA09);
    MyAllocatableArg10 A10(VA10);
    MyAllocatableArg11 A11(VA11);
    MyAllocatableArg12 A12(VA12);
    MyAllocatableArg13 A13(VA13);
    MyAllocatableArg14 A14(VA14);

    bsls::ObjectBuffer<MyInplaceAllocatableObject> buffer;
    prepareObject<
              N_ARGS,N01,N02,N03,N04,N05,N06,N07,N08,N09,N10,N11,N12,N13,N14>(
    buffer.address(),A01,A02,A03,A04,A05,A06,A07,A08,A09,A10,A11,A12,A13,A14 );

    bslalg::AutoScalarDestructor<MyInplaceAllocatableObject> proctor(
                                                             buffer.address());
    const MyInplaceAllocatableObject& EXP = buffer.object();

    ASSERTV(MOVE_01, A01.movedFrom(),
            MOVE_01 == (MoveState::e_MOVED == A01.movedFrom()));
    ASSERTV(MOVE_02, A02.movedFrom(),
            MOVE_02 == (MoveState::e_MOVED == A02.movedFrom()));
    ASSERTV(MOVE_03, A03.movedFrom(),
            MOVE_03 == (MoveState::e_MOVED == A03.movedFrom()));
    ASSERTV(MOVE_04, A04.movedFrom(),
            MOVE_04 == (MoveState::e_MOVED == A04.movedFrom()));
    ASSERTV(MOVE_05, A05.movedFrom(),
            MOVE_05 == (MoveState::e_MOVED == A05.movedFrom()));
    ASSERTV(MOVE_06, A06.movedFrom(),
            MOVE_06 == (MoveState::e_MOVED == A06.movedFrom()));
    ASSERTV(MOVE_07, A07.movedFrom(),
            MOVE_07 == (MoveState::e_MOVED == A07.movedFrom()));
    ASSERTV(MOVE_08, A08.movedFrom(),
            MOVE_08 == (MoveState::e_MOVED == A08.movedFrom()));
    ASSERTV(MOVE_09, A09.movedFrom(),
            MOVE_09 == (MoveState::e_MOVED == A09.movedFrom()));
    ASSERTV(MOVE_10, A10.movedFrom(),
            MOVE_10 == (MoveState::e_MOVED == A10.movedFrom()));
    ASSERTV(MOVE_11, A11.movedFrom(),
            MOVE_11 == (MoveState::e_MOVED == A11.movedFrom()));
    ASSERTV(MOVE_12, A12.movedFrom(),
            MOVE_12 == (MoveState::e_MOVED == A12.movedFrom()));
    ASSERTV(MOVE_13, A13.movedFrom(),
            MOVE_13 == (MoveState::e_MOVED == A13.movedFrom()));
    ASSERTV(MOVE_14, A14.movedFrom(),
            MOVE_14 == (MoveState::e_MOVED == A14.movedFrom()));

    // Here starts the actual test case
    bslma::TestAllocatorMonitor dam(da);

    bsls::Types::Int64 numAllocations   = da->numAllocations();
    bsls::Types::Int64 numDeallocations = da->numDeallocations();

    // In the next block of code we will create 14 argument objects using the
    // default allocator, and then either copy or move a certain number of them
    // according to the template arguments N1 -> N14.  When we move an "arg"
    // object, no new allocation occurs, but when we copy an "arg" object an
    // additional allocation will occur.  We need to account the additional
    // allocations within the scope below, but we also need to account for the
    // de-allocations /outside/ that scope.  Also, remember the allocation of
    // the shared 'rep' object when accounting for total allocations and
    // deallocations.

    bsls::Types::Int64 nArgCopies = 0;
    switch (N_ARGS) {
      case 14: if (!N14) { ++nArgCopies; }  // fall-through
      case 13: if (!N13) { ++nArgCopies; }  // fall-through
      case 12: if (!N12) { ++nArgCopies; }  // fall-through
      case 11: if (!N11) { ++nArgCopies; }  // fall-through
      case 10: if (!N10) { ++nArgCopies; }  // fall-through
      case  9: if (!N09) { ++nArgCopies; }  // fall-through
      case  8: if (!N08) { ++nArgCopies; }  // fall-through
      case  7: if (!N07) { ++nArgCopies; }  // fall-through
      case  6: if (!N06) { ++nArgCopies; }  // fall-through
      case  5: if (!N05) { ++nArgCopies; }  // fall-through
      case  4: if (!N04) { ++nArgCopies; }  // fall-through
      case  3: if (!N03) { ++nArgCopies; }  // fall-through
      case  2: if (!N02) { ++nArgCopies; }  // fall-through
      case  1: if (!N01) { ++nArgCopies; }  // fall-through
      case  0: break;                       // silence warnings
    };

    {
        bslma::TestAllocator testArgAlloc("test arguments allocator",
                                          veryVeryVeryVerbose);

        // 14 arguments, all using a specific test allocator
        MyAllocatableArg01 B01(VA01);
        MyAllocatableArg02 B02(VA02);
        MyAllocatableArg03 B03(VA03);
        MyAllocatableArg04 B04(VA04);
        MyAllocatableArg05 B05(VA05);
        MyAllocatableArg06 B06(VA06);
        MyAllocatableArg07 B07(VA07);
        MyAllocatableArg08 B08(VA08);
        MyAllocatableArg09 B09(VA09);
        MyAllocatableArg10 B10(VA10);
        MyAllocatableArg11 B11(VA11);
        MyAllocatableArg12 B12(VA12);
        MyAllocatableArg13 B13(VA13);
        MyAllocatableArg14 B14(VA14);

        bsl::shared_ptr<const MyInplaceAllocatableObject> x;

        switch (N_ARGS) {
          case 0: {
            x = bsl::make_shared<const MyInplaceAllocatableObject>();
          } break;
          case 1: {
            x = bsl::make_shared<const MyInplaceAllocatableObject>(
                                                        testArg(B01, MOVE_01));
          } break;
          case 2: {
            x = bsl::make_shared<const MyInplaceAllocatableObject>(
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02));
          } break;
          case 3: {
            x = bsl::make_shared<const MyInplaceAllocatableObject>(
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03));
          } break;
          case 4: {
            x = bsl::make_shared<const MyInplaceAllocatableObject>(
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04));
          } break;
          case 5: {
            x = bsl::make_shared<const MyInplaceAllocatableObject>(
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04),
                                                        testArg(B05, MOVE_05));
          } break;
          case 6: {
            x = bsl::make_shared<const MyInplaceAllocatableObject>(
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04),
                                                        testArg(B05, MOVE_05),
                                                        testArg(B06, MOVE_06));
          } break;
          case 7: {
            x = bsl::make_shared<const MyInplaceAllocatableObject>(
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04),
                                                        testArg(B05, MOVE_05),
                                                        testArg(B06, MOVE_06),
                                                        testArg(B07, MOVE_07));
          } break;
          case 8: {
            x = bsl::make_shared<const MyInplaceAllocatableObject>(
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04),
                                                        testArg(B05, MOVE_05),
                                                        testArg(B06, MOVE_06),
                                                        testArg(B07, MOVE_07),
                                                        testArg(B08, MOVE_08));
          } break;
          case 9: {
            x = bsl::make_shared<const MyInplaceAllocatableObject>(
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04),
                                                        testArg(B05, MOVE_05),
                                                        testArg(B06, MOVE_06),
                                                        testArg(B07, MOVE_07),
                                                        testArg(B08, MOVE_08),
                                                        testArg(B09, MOVE_09));
          } break;
          case 10: {
            x = bsl::make_shared<const MyInplaceAllocatableObject>(
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04),
                                                        testArg(B05, MOVE_05),
                                                        testArg(B06, MOVE_06),
                                                        testArg(B07, MOVE_07),
                                                        testArg(B08, MOVE_08),
                                                        testArg(B09, MOVE_09),
                                                        testArg(B10, MOVE_10));
          } break;
          case 11: {
            x = bsl::make_shared<const MyInplaceAllocatableObject>(
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04),
                                                        testArg(B05, MOVE_05),
                                                        testArg(B06, MOVE_06),
                                                        testArg(B07, MOVE_07),
                                                        testArg(B08, MOVE_08),
                                                        testArg(B09, MOVE_09),
                                                        testArg(B10, MOVE_10),
                                                        testArg(B11, MOVE_11));
          } break;
          case 12: {
            x = bsl::make_shared<const MyInplaceAllocatableObject>(
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04),
                                                        testArg(B05, MOVE_05),
                                                        testArg(B06, MOVE_06),
                                                        testArg(B07, MOVE_07),
                                                        testArg(B08, MOVE_08),
                                                        testArg(B09, MOVE_09),
                                                        testArg(B10, MOVE_10),
                                                        testArg(B11, MOVE_11),
                                                        testArg(B12, MOVE_12));
          } break;
          case 13: {
            x = bsl::make_shared<const MyInplaceAllocatableObject>(
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04),
                                                        testArg(B05, MOVE_05),
                                                        testArg(B06, MOVE_06),
                                                        testArg(B07, MOVE_07),
                                                        testArg(B08, MOVE_08),
                                                        testArg(B09, MOVE_09),
                                                        testArg(B10, MOVE_10),
                                                        testArg(B11, MOVE_11),
                                                        testArg(B12, MOVE_12),
                                                        testArg(B13, MOVE_13));
          } break;
          case 14: {
            x = bsl::make_shared<const MyInplaceAllocatableObject>(
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04),
                                                        testArg(B05, MOVE_05),
                                                        testArg(B06, MOVE_06),
                                                        testArg(B07, MOVE_07),
                                                        testArg(B08, MOVE_08),
                                                        testArg(B09, MOVE_09),
                                                        testArg(B10, MOVE_10),
                                                        testArg(B11, MOVE_11),
                                                        testArg(B12, MOVE_12),
                                                        testArg(B13, MOVE_13),
                                                        testArg(B14, MOVE_14));
          } break;
        };

        ASSERTV(numAllocations + 15 + nArgCopies,   da->numAllocations(),
                numAllocations + 15 + nArgCopies == da->numAllocations());
        ASSERTV(numDeallocations,   da->numDeallocations(),
                numDeallocations == da->numDeallocations());

        numAllocations   = da->numAllocations();
        numDeallocations = da->numDeallocations();

        ASSERT(1 == x.use_count());
        ASSERT(EXP == *x);
        ASSERTV(da, x->allocator(), da == x->allocator() );

        ASSERT(A01.movedFrom() == B01.movedFrom());
        ASSERT(A02.movedFrom() == B02.movedFrom());
        ASSERT(A03.movedFrom() == B03.movedFrom());
        ASSERT(A04.movedFrom() == B04.movedFrom());
        ASSERT(A05.movedFrom() == B05.movedFrom());
        ASSERT(A06.movedFrom() == B06.movedFrom());
        ASSERT(A07.movedFrom() == B07.movedFrom());
        ASSERT(A08.movedFrom() == B08.movedFrom());
        ASSERT(A09.movedFrom() == B09.movedFrom());
        ASSERT(A10.movedFrom() == B10.movedFrom());
        ASSERT(A11.movedFrom() == B11.movedFrom());
        ASSERT(A12.movedFrom() == B12.movedFrom());
        ASSERT(A13.movedFrom() == B13.movedFrom());
        ASSERT(A14.movedFrom() == B14.movedFrom());
    }
    ASSERTV(numAllocations,   da->numAllocations(),
            numAllocations == da->numAllocations());
    ASSERTV(numDeallocations + 15 + nArgCopies,   da->numDeallocations(),
            numDeallocations + 15 + nArgCopies == da->numDeallocations());

    ASSERT(dam.isInUseSame());
}

template <int N_ARGS,
          int N01,
          int N02,
          int N03,
          int N04,
          int N05,
          int N06,
          int N07,
          int N08,
          int N09,
          int N10,
          int N11,
          int N12,
          int N13,
          int N14>
void Harness::testCase32_LocalAllocator()
{
    bslma::TestAllocator *da =
             dynamic_cast<bslma::TestAllocator *>(bslma::Default::allocator());
    BSLS_ASSERT(da);

    bslma::TestAllocator ta("test allocator for allocator-aware testCase34",
                            veryVeryVeryVerbose);

    DECLARE_BOOL_CONSTANT(MOVE_01, N01 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_02, N02 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_03, N03 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_04, N04 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_05, N05 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_06, N06 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_07, N07 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_08, N08 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_09, N09 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_10, N10 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_11, N11 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_12, N12 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_13, N13 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_14, N14 == 1);

    // First prepare a test object to compare against the shared object
    // constructed by allocate_shared.  In particular, we want to pay attention
    // to the moved-from state of each argument, and the final value of the
    // whole test object.

    bslma::TestAllocator localArgAlloc("local arguments allocator",
                                               veryVeryVeryVerbose);
    // 14 arguments, all using a local test allocator
    MyAllocatableArg01 A01(VA01, &localArgAlloc);
    MyAllocatableArg02 A02(VA02, &localArgAlloc);
    MyAllocatableArg03 A03(VA03, &localArgAlloc);
    MyAllocatableArg04 A04(VA04, &localArgAlloc);
    MyAllocatableArg05 A05(VA05, &localArgAlloc);
    MyAllocatableArg06 A06(VA06, &localArgAlloc);
    MyAllocatableArg07 A07(VA07, &localArgAlloc);
    MyAllocatableArg08 A08(VA08, &localArgAlloc);
    MyAllocatableArg09 A09(VA09, &localArgAlloc);
    MyAllocatableArg10 A10(VA10, &localArgAlloc);
    MyAllocatableArg11 A11(VA11, &localArgAlloc);
    MyAllocatableArg12 A12(VA12, &localArgAlloc);
    MyAllocatableArg13 A13(VA13, &localArgAlloc);
    MyAllocatableArg14 A14(VA14, &localArgAlloc);

    bsls::ObjectBuffer<MyInplaceAllocatableObject> buffer;
    prepareObject<
              N_ARGS,N01,N02,N03,N04,N05,N06,N07,N08,N09,N10,N11,N12,N13,N14>(
    buffer.address(),A01,A02,A03,A04,A05,A06,A07,A08,A09,A10,A11,A12,A13,A14 );

    bslalg::AutoScalarDestructor<MyInplaceAllocatableObject> proctor(
                                                             buffer.address());
    const MyInplaceAllocatableObject& EXP = buffer.object();

    ASSERTV(MOVE_01, A01.movedFrom(),
            MOVE_01 == (MoveState::e_MOVED == A01.movedFrom()));
    ASSERTV(MOVE_02, A02.movedFrom(),
            MOVE_02 == (MoveState::e_MOVED == A02.movedFrom()));
    ASSERTV(MOVE_03, A03.movedFrom(),
            MOVE_03 == (MoveState::e_MOVED == A03.movedFrom()));
    ASSERTV(MOVE_04, A04.movedFrom(),
            MOVE_04 == (MoveState::e_MOVED == A04.movedFrom()));
    ASSERTV(MOVE_05, A05.movedFrom(),
            MOVE_05 == (MoveState::e_MOVED == A05.movedFrom()));
    ASSERTV(MOVE_06, A06.movedFrom(),
            MOVE_06 == (MoveState::e_MOVED == A06.movedFrom()));
    ASSERTV(MOVE_07, A07.movedFrom(),
            MOVE_07 == (MoveState::e_MOVED == A07.movedFrom()));
    ASSERTV(MOVE_08, A08.movedFrom(),
            MOVE_08 == (MoveState::e_MOVED == A08.movedFrom()));
    ASSERTV(MOVE_09, A09.movedFrom(),
            MOVE_09 == (MoveState::e_MOVED == A09.movedFrom()));
    ASSERTV(MOVE_10, A10.movedFrom(),
            MOVE_10 == (MoveState::e_MOVED == A10.movedFrom()));
    ASSERTV(MOVE_11, A11.movedFrom(),
            MOVE_11 == (MoveState::e_MOVED == A11.movedFrom()));
    ASSERTV(MOVE_12, A12.movedFrom(),
            MOVE_12 == (MoveState::e_MOVED == A12.movedFrom()));
    ASSERTV(MOVE_13, A13.movedFrom(),
            MOVE_13 == (MoveState::e_MOVED == A13.movedFrom()));
    ASSERTV(MOVE_14, A14.movedFrom(),
            MOVE_14 == (MoveState::e_MOVED == A14.movedFrom()));

    // Here starts the actual test case
    bslma::TestAllocatorMonitor dam(da);

    bsls::Types::Int64 numAllocations   = da->numAllocations();
    bsls::Types::Int64 numDeallocations = da->numDeallocations();

    {
        bslma::TestAllocator testArgAlloc("test arguments allocator",
                                          veryVeryVeryVerbose);

        // 14 arguments, all using a specific test allocator
        MyAllocatableArg01 B01(VA01, &testArgAlloc);
        MyAllocatableArg02 B02(VA02, &testArgAlloc);
        MyAllocatableArg03 B03(VA03, &testArgAlloc);
        MyAllocatableArg04 B04(VA04, &testArgAlloc);
        MyAllocatableArg05 B05(VA05, &testArgAlloc);
        MyAllocatableArg06 B06(VA06, &testArgAlloc);
        MyAllocatableArg07 B07(VA07, &testArgAlloc);
        MyAllocatableArg08 B08(VA08, &testArgAlloc);
        MyAllocatableArg09 B09(VA09, &testArgAlloc);
        MyAllocatableArg10 B10(VA10, &testArgAlloc);
        MyAllocatableArg11 B11(VA11, &testArgAlloc);
        MyAllocatableArg12 B12(VA12, &testArgAlloc);
        MyAllocatableArg13 B13(VA13, &testArgAlloc);
        MyAllocatableArg14 B14(VA14, &testArgAlloc);

        bsl::shared_ptr<const MyInplaceAllocatableObject> x;

        switch (N_ARGS) {
          case 0: {
            x = bsl::make_shared<const MyInplaceAllocatableObject>();
          } break;
          case 1: {
            x = bsl::make_shared<const MyInplaceAllocatableObject>(
                                                        testArg(B01, MOVE_01));
          } break;
          case 2: {
            x = bsl::make_shared<const MyInplaceAllocatableObject>(
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02));
          } break;
          case 3: {
            x = bsl::make_shared<const MyInplaceAllocatableObject>(
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03));
          } break;
          case 4: {
            x = bsl::make_shared<const MyInplaceAllocatableObject>(
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04));
          } break;
          case 5: {
            x = bsl::make_shared<const MyInplaceAllocatableObject>(
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04),
                                                        testArg(B05, MOVE_05));
          } break;
          case 6: {
            x = bsl::make_shared<const MyInplaceAllocatableObject>(
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04),
                                                        testArg(B05, MOVE_05),
                                                        testArg(B06, MOVE_06));
          } break;
          case 7: {
            x = bsl::make_shared<const MyInplaceAllocatableObject>(
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04),
                                                        testArg(B05, MOVE_05),
                                                        testArg(B06, MOVE_06),
                                                        testArg(B07, MOVE_07));
          } break;
          case 8: {
            x = bsl::make_shared<const MyInplaceAllocatableObject>(
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04),
                                                        testArg(B05, MOVE_05),
                                                        testArg(B06, MOVE_06),
                                                        testArg(B07, MOVE_07),
                                                        testArg(B08, MOVE_08));
          } break;
          case 9: {
            x = bsl::make_shared<const MyInplaceAllocatableObject>(
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04),
                                                        testArg(B05, MOVE_05),
                                                        testArg(B06, MOVE_06),
                                                        testArg(B07, MOVE_07),
                                                        testArg(B08, MOVE_08),
                                                        testArg(B09, MOVE_09));
          } break;
          case 10: {
            x = bsl::make_shared<const MyInplaceAllocatableObject>(
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04),
                                                        testArg(B05, MOVE_05),
                                                        testArg(B06, MOVE_06),
                                                        testArg(B07, MOVE_07),
                                                        testArg(B08, MOVE_08),
                                                        testArg(B09, MOVE_09),
                                                        testArg(B10, MOVE_10));
          } break;
          case 11: {
            x = bsl::make_shared<const MyInplaceAllocatableObject>(
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04),
                                                        testArg(B05, MOVE_05),
                                                        testArg(B06, MOVE_06),
                                                        testArg(B07, MOVE_07),
                                                        testArg(B08, MOVE_08),
                                                        testArg(B09, MOVE_09),
                                                        testArg(B10, MOVE_10),
                                                        testArg(B11, MOVE_11));
          } break;
          case 12: {
            x = bsl::make_shared<const MyInplaceAllocatableObject>(
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04),
                                                        testArg(B05, MOVE_05),
                                                        testArg(B06, MOVE_06),
                                                        testArg(B07, MOVE_07),
                                                        testArg(B08, MOVE_08),
                                                        testArg(B09, MOVE_09),
                                                        testArg(B10, MOVE_10),
                                                        testArg(B11, MOVE_11),
                                                        testArg(B12, MOVE_12));
          } break;
          case 13: {
            x = bsl::make_shared<const MyInplaceAllocatableObject>(
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04),
                                                        testArg(B05, MOVE_05),
                                                        testArg(B06, MOVE_06),
                                                        testArg(B07, MOVE_07),
                                                        testArg(B08, MOVE_08),
                                                        testArg(B09, MOVE_09),
                                                        testArg(B10, MOVE_10),
                                                        testArg(B11, MOVE_11),
                                                        testArg(B12, MOVE_12),
                                                        testArg(B13, MOVE_13));
          } break;
          case 14: {
            x = bsl::make_shared<const MyInplaceAllocatableObject>(
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04),
                                                        testArg(B05, MOVE_05),
                                                        testArg(B06, MOVE_06),
                                                        testArg(B07, MOVE_07),
                                                        testArg(B08, MOVE_08),
                                                        testArg(B09, MOVE_09),
                                                        testArg(B10, MOVE_10),
                                                        testArg(B11, MOVE_11),
                                                        testArg(B12, MOVE_12),
                                                        testArg(B13, MOVE_13),
                                                        testArg(B14, MOVE_14));
          } break;
        };

        // Argument types are always moved, if passed through a movable
        // reference, as our test type takes its constructor arguments by
        // value.  However, the emplaced members of the test type used the
        // default allocator used by 'make_shared', rather than propagating the
        // allocator from the argument, so the overall observation is that the
        // default allocator always allocates 1 item for the 'rep' object, plus
        // one allocation per 'N_ARGS'.  A matching number of deallocations
        // occur on exiting the block.  We do not attempt to track the specific
        // local allocator allocations, as they do not transmit into the shared
        // object itself.

        ASSERTV(numAllocations + N_ARGS + 1,   da->numAllocations(),
                numAllocations + N_ARGS + 1 == da->numAllocations());
        ASSERTV(numDeallocations,   da->numDeallocations(),
                numDeallocations == da->numDeallocations());

        numAllocations   = da->numAllocations();
        numDeallocations = da->numDeallocations();

        ASSERT(1 == x.use_count());
        ASSERT(EXP == *x);
        ASSERTV(da, x->allocator(), da == x->allocator() );

        ASSERT(A01.movedFrom() == B01.movedFrom());
        ASSERT(A02.movedFrom() == B02.movedFrom());
        ASSERT(A03.movedFrom() == B03.movedFrom());
        ASSERT(A04.movedFrom() == B04.movedFrom());
        ASSERT(A05.movedFrom() == B05.movedFrom());
        ASSERT(A06.movedFrom() == B06.movedFrom());
        ASSERT(A07.movedFrom() == B07.movedFrom());
        ASSERT(A08.movedFrom() == B08.movedFrom());
        ASSERT(A09.movedFrom() == B09.movedFrom());
        ASSERT(A10.movedFrom() == B10.movedFrom());
        ASSERT(A11.movedFrom() == B11.movedFrom());
        ASSERT(A12.movedFrom() == B12.movedFrom());
        ASSERT(A13.movedFrom() == B13.movedFrom());
        ASSERT(A14.movedFrom() == B14.movedFrom());
    }
    ASSERTV(numAllocations,   da->numAllocations(),
            numAllocations == da->numAllocations());
    ASSERTV(numDeallocations + N_ARGS + 1,   da->numDeallocations(),
            numDeallocations + N_ARGS + 1 == da->numDeallocations());

    ASSERT(dam.isInUseSame());
}

template <int N_ARGS,
          bool USES_BSLMA_ALLOCATOR>
void Harness::testCase32_AllocatorPropagation()
{
    typedef AllocPropagationTestType<USES_BSLMA_ALLOCATOR>
                                                          AllocPropagationType;

    bslma::TestAllocator *da =
             dynamic_cast<bslma::TestAllocator *>(bslma::Default::allocator());
    BSLS_ASSERT(da);

    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

    // 14 arguments for constructor
    MyAllocatableArg01 A01(VA01);
    MyAllocatableArg02 A02(VA02);
    MyAllocatableArg03 A03(VA03);
    MyAllocatableArg04 A04(VA04);
    MyAllocatableArg05 A05(VA05);
    MyAllocatableArg06 A06(VA06);
    MyAllocatableArg07 A07(VA07);
    MyAllocatableArg08 A08(VA08);
    MyAllocatableArg09 A09(VA09);
    MyAllocatableArg10 A10(VA10);
    MyAllocatableArg11 A11(VA11);
    MyAllocatableArg12 A12(VA12);
    MyAllocatableArg13 A13(VA13);
    MyAllocatableArg14 A14(VA14);

    // Here starts the actual test case
    bslma::TestAllocatorMonitor dam(da);

    bsls::Types::Int64 numAllocationsDA   = da->numAllocations();
    bsls::Types::Int64 numDeallocationsDA = da->numDeallocations();

    const bsls::Types::Int64 EXPECTED_DA_ALLOCATIONS_NUM   =
                                           numAllocationsDA + 2 * (1 + N_ARGS);
    const bsls::Types::Int64 EXPECTED_DA_DEALLOCATIONS_NUM =
                                         numDeallocationsDA + 2 * (1 + N_ARGS);
    {
        bsl::shared_ptr<const AllocPropagationType>        mX;
        const bsl::shared_ptr<const AllocPropagationType>& X = mX;
        bsl::shared_ptr<const AllocPropagationType>        mXA;
        const bsl::shared_ptr<const AllocPropagationType>& XA = mXA;

        switch (N_ARGS) {
          case 0: {
            mX  = bsl::make_shared<const AllocPropagationType>();
            mXA = bsl::make_shared<const AllocPropagationType>(&sa);
          } break;
          case 1: {
            mX  = bsl::make_shared<const AllocPropagationType>(A01);
            mXA = bsl::make_shared<const AllocPropagationType>(A01, &sa);
          } break;
          case 2: {
            mX  = bsl::make_shared<const AllocPropagationType>(A01, A02);
            mXA = bsl::make_shared<const AllocPropagationType>(A01, A02, &sa);
          } break;
          case 3: {
            mX  = bsl::make_shared<const AllocPropagationType>(A01, A02, A03);
            mXA = bsl::make_shared<const AllocPropagationType>(A01, A02, A03,
                                                               &sa);
          } break;
          case 4: {
            mX  = bsl::make_shared<const AllocPropagationType>(A01, A02, A03,
                                                               A04);
            mXA = bsl::make_shared<const AllocPropagationType>(A01, A02, A03,
                                                               A04, &sa);
          } break;
          case 5: {
            mX  = bsl::make_shared<const AllocPropagationType>(A01, A02, A03,
                                                               A04, A05);
            mXA = bsl::make_shared<const AllocPropagationType>(A01, A02, A03,
                                                               A04, A05, &sa);
          } break;
          case 6: {
            mX  = bsl::make_shared<const AllocPropagationType>(A01, A02, A03,
                                                               A04, A05, A06);
            mXA = bsl::make_shared<const AllocPropagationType>(A01, A02, A03,
                                                               A04, A05, A06,
                                                               &sa);
          } break;
          case 7: {
            mX  = bsl::make_shared<const AllocPropagationType>(A01, A02, A03,
                                                               A04, A05, A06,
                                                               A07);
            mXA = bsl::make_shared<const AllocPropagationType>(A01, A02, A03,
                                                               A04, A05, A06,
                                                               A07, &sa);
          } break;
          case 8: {
            mX  = bsl::make_shared<const AllocPropagationType>(A01, A02, A03,
                                                               A04, A05, A06,
                                                               A07, A08);
            mXA = bsl::make_shared<const AllocPropagationType>(A01, A02, A03,
                                                               A04, A05, A06,
                                                               A07, A08, &sa);
          } break;
          case 9: {
            mX  = bsl::make_shared<const AllocPropagationType>(A01, A02, A03,
                                                               A04, A05, A06,
                                                               A07, A08, A09);
            mXA = bsl::make_shared<const AllocPropagationType>(A01, A02, A03,
                                                               A04, A05, A06,
                                                               A07, A08, A09,
                                                               &sa);
          } break;
          case 10: {
            mX  = bsl::make_shared<const AllocPropagationType>(A01, A02, A03,
                                                               A04, A05, A06,
                                                               A07, A08, A09,
                                                               A10);
            mXA = bsl::make_shared<const AllocPropagationType>(A01, A02, A03,
                                                               A04, A05, A06,
                                                               A07, A08, A09,
                                                               A10, &sa);
          } break;
          case 11: {
            mX  = bsl::make_shared<const AllocPropagationType>(A01, A02, A03,
                                                               A04, A05, A06,
                                                               A07, A08, A09,
                                                               A10, A11);
            mXA = bsl::make_shared<const AllocPropagationType>(A01, A02, A03,
                                                               A04, A05, A06,
                                                               A07, A08, A09,
                                                               A10, A11, &sa);
          } break;
          case 12: {
            mX  = bsl::make_shared<const AllocPropagationType>(A01, A02, A03,
                                                               A04, A05, A06,
                                                               A07, A08, A09,
                                                               A10, A11, A12);
            mXA = bsl::make_shared<const AllocPropagationType>(A01, A02, A03,
                                                               A04, A05, A06,
                                                               A07, A08, A09,
                                                               A10, A11, A12,
                                                               &sa);
          } break;
          case 13: {
            mX  = bsl::make_shared<const AllocPropagationType>(A01, A02, A03,
                                                               A04, A05, A06,
                                                               A07, A08, A09,
                                                               A10, A11, A12,
                                                               A13);
            mXA = bsl::make_shared<const AllocPropagationType>(A01, A02, A03,
                                                               A04, A05, A06,
                                                               A07, A08, A09,
                                                               A10, A11, A12,
                                                               A13, &sa);
          } break;
          case 14: {
            mX  = bsl::make_shared<const AllocPropagationType>(A01, A02, A03,
                                                               A04, A05, A06,
                                                               A07, A08, A09,
                                                               A10, A11, A12,
                                                               A13, A14);

            // Note that we are not able to pass 15 parameters here, because
            // some compilers do not support variadic templates, while
            // automatically generated 'make_shared' overloads accept up to 14
            // arguments only.  In order not to violate the structure of the
            // test, we simply pass 14 parameters here again.
            mXA = bsl::make_shared<const AllocPropagationType>(A01, A02, A03,
                                                               A04, A05, A06,
                                                               A07, A08, A09,
                                                               A10, A11, A12,
                                                               A13, A14);
          } break;
        };

        // Verify that default allocator is used for managed object creation
        // (note that ctor of managed object creates N_ARGS temporary objects
        // using the default allocator).


        ASSERTV(N_ARGS, EXPECTED_DA_ALLOCATIONS_NUM,   da->numAllocations(),
                        EXPECTED_DA_ALLOCATIONS_NUM == da->numAllocations());
        ASSERTV(N_ARGS, sa.numAllocations(), 0 == sa.numAllocations());

        // Verify that 'make_shared' does not pass an extra default allocator.

        ASSERTV(0 == X->allocator());

        // Verify that 'make_shared' does propagate a supplied allocator.

        if (14 != N_ARGS) {
            ASSERTV(&sa == XA->allocator());
        }
        else {
            // We have not pass an allocator at the final position.
            ASSERTV(0 == XA->allocator());
        }
    }

    ASSERTV(N_ARGS, EXPECTED_DA_DEALLOCATIONS_NUM,   da->numDeallocations(),
                    EXPECTED_DA_DEALLOCATIONS_NUM == da->numDeallocations());
    ASSERTV(N_ARGS, sa.numDeallocations(), 0 == sa.numDeallocations());

    ASSERT(dam.isInUseSame());
}

template <class ALLOCATOR>
void Harness::testCase33(ALLOCATOR basicAllocator)
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

#if !defined(BSL_DO_NOT_TEST_MOVE_FORWARDING)
    if (verbose) printf("\nTesting constructor with 0 arguments"
                        "\n-------------------------------------\n");

    testCase33_RunTest<0,2,2,2,2,2,2,2,2,2,2,2,2,2,2>(basicAllocator);

    if (verbose) printf("\nTesting constructor with 1 argument"
                        "\n-------------------------------------\n");

    testCase33_RunTest<1,0,2,2,2,2,2,2,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<1,1,2,2,2,2,2,2,2,2,2,2,2,2,2>(basicAllocator);

    if (verbose) printf("\nTesting constructor with 2 arguments"
                        "\n-------------------------------------\n");

    testCase33_RunTest<2,0,0,2,2,2,2,2,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<2,1,0,2,2,2,2,2,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<2,0,1,2,2,2,2,2,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<2,1,1,2,2,2,2,2,2,2,2,2,2,2,2>(basicAllocator);

    if (verbose) printf("\nTesting constructor with 3 arguments"
                        "\n-------------------------------------\n");

    testCase33_RunTest<3,0,0,0,2,2,2,2,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<3,1,0,0,2,2,2,2,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<3,0,1,0,2,2,2,2,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<3,0,0,1,2,2,2,2,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<3,1,1,1,2,2,2,2,2,2,2,2,2,2,2>(basicAllocator);

# if !defined(BSLSTL_SHAREDPTR_LIMIT_TESTING_COMPLEXITY)
    if (verbose) printf("\nTesting constructor with 4 arguments"
                        "\n-------------------------------------\n");

    testCase33_RunTest<4,0,0,0,0,2,2,2,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<4,1,0,0,0,2,2,2,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<4,0,1,0,0,2,2,2,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<4,0,0,1,0,2,2,2,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<4,0,0,0,1,2,2,2,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<4,1,1,1,1,2,2,2,2,2,2,2,2,2,2>(basicAllocator);

    if (verbose) printf("\nTesting constructor with 5 arguments"
                        "\n-------------------------------------\n");

    testCase33_RunTest<5,0,0,0,0,0,2,2,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<5,1,0,0,0,0,2,2,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<5,0,1,0,0,0,2,2,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<5,0,0,1,0,0,2,2,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<5,0,0,0,1,0,2,2,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<5,0,0,0,0,1,2,2,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<5,1,1,1,1,1,2,2,2,2,2,2,2,2,2>(basicAllocator);

    if (verbose) printf("\nTesting constructor with 6 arguments"
                        "\n-------------------------------------\n");

    testCase33_RunTest<6,0,0,0,0,0,0,2,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<6,1,0,0,0,0,0,2,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<6,0,1,0,0,0,0,2,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<6,0,0,1,0,0,0,2,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<6,0,0,0,1,0,0,2,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<6,0,0,0,0,1,0,2,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<6,0,0,0,0,0,1,2,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<6,1,1,1,1,1,1,2,2,2,2,2,2,2,2>(basicAllocator);

    if (verbose) printf("\nTesting constructor with 7 arguments"
                        "\n-------------------------------------\n");

    testCase33_RunTest<7,0,0,0,0,0,0,0,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<7,1,0,0,0,0,0,0,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<7,0,1,0,0,0,0,0,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<7,0,0,1,0,0,0,0,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<7,0,0,0,1,0,0,0,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<7,0,0,0,0,1,0,0,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<7,0,0,0,0,0,1,0,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<7,0,0,0,0,0,0,1,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<7,1,1,1,1,1,1,1,2,2,2,2,2,2,2>(basicAllocator);

    if (verbose) printf("\nTesting constructor with 8 arguments"
                        "\n-------------------------------------\n");

    testCase33_RunTest<8,0,0,0,0,0,0,0,0,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<8,1,0,0,0,0,0,0,0,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<8,0,1,0,0,0,0,0,0,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<8,0,0,1,0,0,0,0,0,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<8,0,0,0,1,0,0,0,0,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<8,0,0,0,0,1,0,0,0,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<8,0,0,0,0,0,1,0,0,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<8,0,0,0,0,0,0,1,0,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<8,0,0,0,0,0,0,0,1,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<8,1,1,1,1,1,1,1,1,2,2,2,2,2,2>(basicAllocator);

    if (verbose) printf("\nTesting constructor with 9 arguments"
                        "\n-------------------------------------\n");

    testCase33_RunTest<9,0,0,0,0,0,0,0,0,0,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<9,1,0,0,0,0,0,0,0,0,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<9,0,1,0,0,0,0,0,0,0,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<9,0,0,1,0,0,0,0,0,0,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<9,0,0,0,1,0,0,0,0,0,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<9,0,0,0,0,1,0,0,0,0,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<9,0,0,0,0,0,1,0,0,0,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<9,0,0,0,0,0,0,1,0,0,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<9,0,0,0,0,0,0,0,1,0,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<9,0,0,0,0,0,0,0,0,1,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<9,1,1,1,1,1,1,1,1,1,2,2,2,2,2>(basicAllocator);

    if (verbose) printf("\nTesting constructor with 10 arguments"
                        "\n-------------------------------------\n");

    testCase33_RunTest<10,0,0,0,0,0,0,0,0,0,0,2,2,2,2>(basicAllocator);
    testCase33_RunTest<10,1,0,0,0,0,0,0,0,0,0,2,2,2,2>(basicAllocator);
    testCase33_RunTest<10,0,1,0,0,0,0,0,0,0,0,2,2,2,2>(basicAllocator);
    testCase33_RunTest<10,0,0,1,0,0,0,0,0,0,0,2,2,2,2>(basicAllocator);
    testCase33_RunTest<10,0,0,0,1,0,0,0,0,0,0,2,2,2,2>(basicAllocator);
    testCase33_RunTest<10,0,0,0,0,1,0,0,0,0,0,2,2,2,2>(basicAllocator);
    testCase33_RunTest<10,0,0,0,0,0,1,0,0,0,0,2,2,2,2>(basicAllocator);
    testCase33_RunTest<10,0,0,0,0,0,0,1,0,0,0,2,2,2,2>(basicAllocator);
    testCase33_RunTest<10,0,0,0,0,0,0,0,1,0,0,2,2,2,2>(basicAllocator);
    testCase33_RunTest<10,0,0,0,0,0,0,0,0,1,0,2,2,2,2>(basicAllocator);
    testCase33_RunTest<10,0,0,0,0,0,0,0,0,0,1,2,2,2,2>(basicAllocator);
    testCase33_RunTest<10,1,1,1,1,1,1,1,1,1,1,2,2,2,2>(basicAllocator);

    if (verbose) printf("\nTesting constructor with 11 arguments"
                        "\n-------------------------------------\n");

    testCase33_RunTest<11,0,0,0,0,0,0,0,0,0,0,0,2,2,2>(basicAllocator);
    testCase33_RunTest<11,1,0,0,0,0,0,0,0,0,0,0,2,2,2>(basicAllocator);
    testCase33_RunTest<11,0,1,0,0,0,0,0,0,0,0,0,2,2,2>(basicAllocator);
    testCase33_RunTest<11,0,0,1,0,0,0,0,0,0,0,0,2,2,2>(basicAllocator);
    testCase33_RunTest<11,0,0,0,1,0,0,0,0,0,0,0,2,2,2>(basicAllocator);
    testCase33_RunTest<11,0,0,0,0,1,0,0,0,0,0,0,2,2,2>(basicAllocator);
    testCase33_RunTest<11,0,0,0,0,0,1,0,0,0,0,0,2,2,2>(basicAllocator);
    testCase33_RunTest<11,0,0,0,0,0,0,1,0,0,0,0,2,2,2>(basicAllocator);
    testCase33_RunTest<11,0,0,0,0,0,0,0,1,0,0,0,2,2,2>(basicAllocator);
    testCase33_RunTest<11,0,0,0,0,0,0,0,0,1,0,0,2,2,2>(basicAllocator);
    testCase33_RunTest<11,0,0,0,0,0,0,0,0,0,1,0,2,2,2>(basicAllocator);
    testCase33_RunTest<11,0,0,0,0,0,0,0,0,0,0,1,2,2,2>(basicAllocator);
    testCase33_RunTest<11,1,1,1,1,1,1,1,1,1,1,1,2,2,2>(basicAllocator);

    if (verbose) printf("\nTesting constructor with 12 arguments"
                        "\n-------------------------------------\n");

    testCase33_RunTest<12,0,0,0,0,0,0,0,0,0,0,0,0,2,2>(basicAllocator);
    testCase33_RunTest<12,1,0,0,0,0,0,0,0,0,0,0,0,2,2>(basicAllocator);
    testCase33_RunTest<12,0,1,0,0,0,0,0,0,0,0,0,0,2,2>(basicAllocator);
    testCase33_RunTest<12,0,0,1,0,0,0,0,0,0,0,0,0,2,2>(basicAllocator);
    testCase33_RunTest<12,0,0,0,1,0,0,0,0,0,0,0,0,2,2>(basicAllocator);
    testCase33_RunTest<12,0,0,0,0,1,0,0,0,0,0,0,0,2,2>(basicAllocator);
    testCase33_RunTest<12,0,0,0,0,0,1,0,0,0,0,0,0,2,2>(basicAllocator);
    testCase33_RunTest<12,0,0,0,0,0,0,1,0,0,0,0,0,2,2>(basicAllocator);
    testCase33_RunTest<12,0,0,0,0,0,0,0,1,0,0,0,0,2,2>(basicAllocator);
    testCase33_RunTest<12,0,0,0,0,0,0,0,0,1,0,0,0,2,2>(basicAllocator);
    testCase33_RunTest<12,0,0,0,0,0,0,0,0,0,1,0,0,2,2>(basicAllocator);
    testCase33_RunTest<12,0,0,0,0,0,0,0,0,0,0,1,0,2,2>(basicAllocator);
    testCase33_RunTest<12,0,0,0,0,0,0,0,0,0,0,0,1,2,2>(basicAllocator);
    testCase33_RunTest<12,1,1,1,1,1,1,1,1,1,1,1,1,2,2>(basicAllocator);

    if (verbose) printf("\nTesting constructor with 13 arguments"
                        "\n-------------------------------------\n");

    testCase33_RunTest<13,0,0,0,0,0,0,0,0,0,0,0,0,0,2>(basicAllocator);
    testCase33_RunTest<13,1,0,0,0,0,0,0,0,0,0,0,0,0,2>(basicAllocator);
    testCase33_RunTest<13,0,1,0,0,0,0,0,0,0,0,0,0,0,2>(basicAllocator);
    testCase33_RunTest<13,0,0,1,0,0,0,0,0,0,0,0,0,0,2>(basicAllocator);
    testCase33_RunTest<13,0,0,0,1,0,0,0,0,0,0,0,0,0,2>(basicAllocator);
    testCase33_RunTest<13,0,0,0,0,1,0,0,0,0,0,0,0,0,2>(basicAllocator);
    testCase33_RunTest<13,0,0,0,0,0,1,0,0,0,0,0,0,0,2>(basicAllocator);
    testCase33_RunTest<13,0,0,0,0,0,0,1,0,0,0,0,0,0,2>(basicAllocator);
    testCase33_RunTest<13,0,0,0,0,0,0,0,1,0,0,0,0,0,2>(basicAllocator);
    testCase33_RunTest<13,0,0,0,0,0,0,0,0,1,0,0,0,0,2>(basicAllocator);
    testCase33_RunTest<13,0,0,0,0,0,0,0,0,0,1,0,0,0,2>(basicAllocator);
    testCase33_RunTest<13,0,0,0,0,0,0,0,0,0,0,1,0,0,2>(basicAllocator);
    testCase33_RunTest<13,0,0,0,0,0,0,0,0,0,0,0,1,0,2>(basicAllocator);
    testCase33_RunTest<13,0,0,0,0,0,0,0,0,0,0,0,0,1,2>(basicAllocator);
    testCase33_RunTest<13,1,1,1,1,1,1,1,1,1,1,1,1,1,2>(basicAllocator);
# else // BSLSTL_SHAREDPTR_LIMIT_TESTING_COMPLEXITY)
    if (verbose) printf("\nTesting constructor with 4 arguments"
                        "\n-------------------------------------\n");

    testCase33_RunTest<4,0,0,0,0,2,2,2,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<4,1,1,1,1,2,2,2,2,2,2,2,2,2,2>(basicAllocator);

    if (verbose) printf("\nTesting constructor with 5 arguments"
                        "\n-------------------------------------\n");

    testCase33_RunTest<5,0,0,0,0,0,2,2,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<5,1,1,1,1,1,2,2,2,2,2,2,2,2,2>(basicAllocator);

    if (verbose) printf("\nTesting constructor with 6 arguments"
                        "\n-------------------------------------\n");

    testCase33_RunTest<6,0,0,0,0,0,0,2,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<6,1,1,1,1,1,1,2,2,2,2,2,2,2,2>(basicAllocator);

    if (verbose) printf("\nTesting constructor with 7 arguments"
                        "\n-------------------------------------\n");

    testCase33_RunTest<7,0,0,0,0,0,0,0,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<7,1,1,1,1,1,1,1,2,2,2,2,2,2,2>(basicAllocator);

    if (verbose) printf("\nTesting constructor with 8 arguments"
                        "\n-------------------------------------\n");

    testCase33_RunTest<8,0,0,0,0,0,0,0,0,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<8,1,1,1,1,1,1,1,1,2,2,2,2,2,2>(basicAllocator);

    if (verbose) printf("\nTesting constructor with 9 arguments"
                        "\n-------------------------------------\n");

    testCase33_RunTest<9,0,0,0,0,0,0,0,0,0,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<9,1,1,1,1,1,1,1,1,1,2,2,2,2,2>(basicAllocator);

    if (verbose) printf("\nTesting constructor with 10 arguments"
                        "\n-------------------------------------\n");

    testCase33_RunTest<10,0,0,0,0,0,0,0,0,0,0,2,2,2,2>(basicAllocator);
    testCase33_RunTest<10,1,1,1,1,1,1,1,1,1,1,2,2,2,2>(basicAllocator);

    if (verbose) printf("\nTesting constructor with 11 arguments"
                        "\n-------------------------------------\n");

    testCase33_RunTest<11,0,0,0,0,0,0,0,0,0,0,0,2,2,2>(basicAllocator);
    testCase33_RunTest<11,1,1,1,1,1,1,1,1,1,1,1,2,2,2>(basicAllocator);

    if (verbose) printf("\nTesting constructor with 12 arguments"
                        "\n-------------------------------------\n");

    testCase33_RunTest<12,0,0,0,0,0,0,0,0,0,0,0,0,2,2>(basicAllocator);
    testCase33_RunTest<12,1,1,1,1,1,1,1,1,1,1,1,1,2,2>(basicAllocator);

    if (verbose) printf("\nTesting constructor with 13 arguments"
                        "\n-------------------------------------\n");

    testCase33_RunTest<13,0,0,0,0,0,0,0,0,0,0,0,0,0,2>(basicAllocator);
    testCase33_RunTest<13,1,1,1,1,1,1,1,1,1,1,1,1,1,2>(basicAllocator);
# endif // BSLSTL_SHAREDPTR_LIMIT_TESTING_COMPLEXITY)

    if (verbose) printf("\nTesting constructor with 14 arguments"
                        "\n-------------------------------------\n");

    testCase33_RunTest<14,0,0,0,0,0,0,0,0,0,0,0,0,0,0>(basicAllocator);
    testCase33_RunTest<14,1,0,0,0,0,0,0,0,0,0,0,0,0,0>(basicAllocator);
    testCase33_RunTest<14,0,1,0,0,0,0,0,0,0,0,0,0,0,0>(basicAllocator);
    testCase33_RunTest<14,0,0,1,0,0,0,0,0,0,0,0,0,0,0>(basicAllocator);
    testCase33_RunTest<14,0,0,0,1,0,0,0,0,0,0,0,0,0,0>(basicAllocator);
    testCase33_RunTest<14,0,0,0,0,1,0,0,0,0,0,0,0,0,0>(basicAllocator);
    testCase33_RunTest<14,0,0,0,0,0,1,0,0,0,0,0,0,0,0>(basicAllocator);
    testCase33_RunTest<14,0,0,0,0,0,0,1,0,0,0,0,0,0,0>(basicAllocator);
    testCase33_RunTest<14,0,0,0,0,0,0,0,1,0,0,0,0,0,0>(basicAllocator);
    testCase33_RunTest<14,0,0,0,0,0,0,0,0,1,0,0,0,0,0>(basicAllocator);
    testCase33_RunTest<14,0,0,0,0,0,0,0,0,0,1,0,0,0,0>(basicAllocator);
    testCase33_RunTest<14,0,0,0,0,0,0,0,0,0,0,1,0,0,0>(basicAllocator);
    testCase33_RunTest<14,0,0,0,0,0,0,0,0,0,0,0,1,0,0>(basicAllocator);
    testCase33_RunTest<14,0,0,0,0,0,0,0,0,0,0,0,0,1,0>(basicAllocator);
    testCase33_RunTest<14,0,0,0,0,0,0,0,0,0,0,0,0,0,1>(basicAllocator);
    testCase33_RunTest<14,1,1,1,1,1,1,1,1,1,1,1,1,1,1>(basicAllocator);
#else // BSL_DO_NOT_TEST_MOVE_FORWARDING
    testCase33_RunTest< 0,2,2,2,2,2,2,2,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest< 1,0,2,2,2,2,2,2,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest< 2,0,0,2,2,2,2,2,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest< 3,0,0,0,2,2,2,2,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest< 4,0,0,0,0,2,2,2,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest< 5,0,0,0,0,0,2,2,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest< 6,0,0,0,0,0,0,2,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest< 7,0,0,0,0,0,0,0,2,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest< 8,0,0,0,0,0,0,0,0,2,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest< 9,0,0,0,0,0,0,0,0,0,2,2,2,2,2>(basicAllocator);
    testCase33_RunTest<10,0,0,0,0,0,0,0,0,0,0,2,2,2,2>(basicAllocator);
    testCase33_RunTest<11,0,0,0,0,0,0,0,0,0,0,0,2,2,2>(basicAllocator);
    testCase33_RunTest<12,0,0,0,0,0,0,0,0,0,0,0,0,2,2>(basicAllocator);
    testCase33_RunTest<13,0,0,0,0,0,0,0,0,0,0,0,0,0,2>(basicAllocator);
    testCase33_RunTest<14,0,0,0,0,0,0,0,0,0,0,0,0,0,0>(basicAllocator);
#endif // BSL_DO_NOT_TEST_MOVE_FORWARDING
}

template <int N_ARGS,
          int N01,
          int N02,
          int N03,
          int N04,
          int N05,
          int N06,
          int N07,
          int N08,
          int N09,
          int N10,
          int N11,
          int N12,
          int N13,
          int N14,
          class ALLOCATOR>
void Harness::testCase33_RunTest(ALLOCATOR basicAllocator)
{
    bslma::TestAllocator *da =
             dynamic_cast<bslma::TestAllocator *>(bslma::Default::allocator());
    BSLS_ASSERT(da);

    bslma::Allocator      *wrappedAllocator = extractBslma(basicAllocator);
    bslma::TestAllocator  *wrappedTest = dynamic_cast<bslma::TestAllocator *>(
                                                             wrappedAllocator);
    if (!wrappedTest) {
        ASSERT(!"Allocator for case 33 does not wrap a bslma::TestAllocator");
        return;                                                       // RETURN
    }

    bslma::TestAllocator&  ta = *wrappedTest;

    DECLARE_BOOL_CONSTANT(MOVE_01, N01 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_02, N02 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_03, N03 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_04, N04 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_05, N05 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_06, N06 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_07, N07 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_08, N08 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_09, N09 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_10, N10 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_11, N11 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_12, N12 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_13, N13 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_14, N14 == 1);

    MyTestArg01 A01(V01);
    MyTestArg02 A02(V02);
    MyTestArg03 A03(V03);
    MyTestArg04 A04(V04);
    MyTestArg05 A05(V05);
    MyTestArg06 A06(V06);
    MyTestArg07 A07(V07);
    MyTestArg08 A08(V08);
    MyTestArg09 A09(V09);
    MyTestArg10 A10(V10);
    MyTestArg11 A11(V11);
    MyTestArg12 A12(V12);
    MyTestArg13 A13(V13);
    MyTestArg14 A14(V14);

    bsls::ObjectBuffer<MyInplaceTestObject> buffer;
    const MyInplaceTestObject& EXP = buffer.object();

    switch (N_ARGS) {
      case 0: {
        new(buffer.address()) MyInplaceTestObject();
        }  break;
      case 1: {
        new(buffer.address()) MyInplaceTestObject(testArg(A01, MOVE_01));
        }  break;
      case 2: {
        new(buffer.address()) MyInplaceTestObject(testArg(A01, MOVE_01),
                                                  testArg(A02, MOVE_02));
        }  break;
      case 3: {
        new(buffer.address()) MyInplaceTestObject(testArg(A01, MOVE_01),
                                                  testArg(A02, MOVE_02),
                                                  testArg(A03, MOVE_03));
        }  break;
      case 4: {
        new(buffer.address()) MyInplaceTestObject(testArg(A01, MOVE_01),
                                                  testArg(A02, MOVE_02),
                                                  testArg(A03, MOVE_03),
                                                  testArg(A04, MOVE_04));
        }  break;
      case 5: {
        new(buffer.address()) MyInplaceTestObject(testArg(A01, MOVE_01),
                                                  testArg(A02, MOVE_02),
                                                  testArg(A03, MOVE_03),
                                                  testArg(A04, MOVE_04),
                                                  testArg(A05, MOVE_05));
        }  break;
      case 6: {
        new(buffer.address()) MyInplaceTestObject(testArg(A01, MOVE_01),
                                                  testArg(A02, MOVE_02),
                                                  testArg(A03, MOVE_03),
                                                  testArg(A04, MOVE_04),
                                                  testArg(A05, MOVE_05),
                                                  testArg(A06, MOVE_06));
        }  break;
      case 7: {
        new(buffer.address()) MyInplaceTestObject(testArg(A01, MOVE_01),
                                                  testArg(A02, MOVE_02),
                                                  testArg(A03, MOVE_03),
                                                  testArg(A04, MOVE_04),
                                                  testArg(A05, MOVE_05),
                                                  testArg(A06, MOVE_06),
                                                  testArg(A07, MOVE_07));
        }  break;
      case 8: {
        new(buffer.address()) MyInplaceTestObject(testArg(A01, MOVE_01),
                                                  testArg(A02, MOVE_02),
                                                  testArg(A03, MOVE_03),
                                                  testArg(A04, MOVE_04),
                                                  testArg(A05, MOVE_05),
                                                  testArg(A06, MOVE_06),
                                                  testArg(A07, MOVE_07),
                                                  testArg(A08, MOVE_08));
        }  break;
      case 9: {
        new(buffer.address()) MyInplaceTestObject(testArg(A01, MOVE_01),
                                                  testArg(A02, MOVE_02),
                                                  testArg(A03, MOVE_03),
                                                  testArg(A04, MOVE_04),
                                                  testArg(A05, MOVE_05),
                                                  testArg(A06, MOVE_06),
                                                  testArg(A07, MOVE_07),
                                                  testArg(A08, MOVE_08),
                                                  testArg(A09, MOVE_09));
        }  break;
      case 10: {
        new(buffer.address()) MyInplaceTestObject(testArg(A01, MOVE_01),
                                                  testArg(A02, MOVE_02),
                                                  testArg(A03, MOVE_03),
                                                  testArg(A04, MOVE_04),
                                                  testArg(A05, MOVE_05),
                                                  testArg(A06, MOVE_06),
                                                  testArg(A07, MOVE_07),
                                                  testArg(A08, MOVE_08),
                                                  testArg(A09, MOVE_09),
                                                  testArg(A10, MOVE_10));
        }  break;
      case 11: {
        new(buffer.address()) MyInplaceTestObject(testArg(A01, MOVE_01),
                                                  testArg(A02, MOVE_02),
                                                  testArg(A03, MOVE_03),
                                                  testArg(A04, MOVE_04),
                                                  testArg(A05, MOVE_05),
                                                  testArg(A06, MOVE_06),
                                                  testArg(A07, MOVE_07),
                                                  testArg(A08, MOVE_08),
                                                  testArg(A09, MOVE_09),
                                                  testArg(A10, MOVE_10),
                                                  testArg(A11, MOVE_11));
        }  break;
      case 12: {
        new(buffer.address()) MyInplaceTestObject(testArg(A01, MOVE_01),
                                                  testArg(A02, MOVE_02),
                                                  testArg(A03, MOVE_03),
                                                  testArg(A04, MOVE_04),
                                                  testArg(A05, MOVE_05),
                                                  testArg(A06, MOVE_06),
                                                  testArg(A07, MOVE_07),
                                                  testArg(A08, MOVE_08),
                                                  testArg(A09, MOVE_09),
                                                  testArg(A10, MOVE_10),
                                                  testArg(A11, MOVE_11),
                                                  testArg(A12, MOVE_12));
        }  break;
      case 13: {
        new(buffer.address()) MyInplaceTestObject(testArg(A01, MOVE_01),
                                                  testArg(A02, MOVE_02),
                                                  testArg(A03, MOVE_03),
                                                  testArg(A04, MOVE_04),
                                                  testArg(A05, MOVE_05),
                                                  testArg(A06, MOVE_06),
                                                  testArg(A07, MOVE_07),
                                                  testArg(A08, MOVE_08),
                                                  testArg(A09, MOVE_09),
                                                  testArg(A10, MOVE_10),
                                                  testArg(A11, MOVE_11),
                                                  testArg(A12, MOVE_12),
                                                  testArg(A13, MOVE_13));
        }  break;
      case 14: {
        new(buffer.address()) MyInplaceTestObject(testArg(A01, MOVE_01),
                                                  testArg(A02, MOVE_02),
                                                  testArg(A03, MOVE_03),
                                                  testArg(A04, MOVE_04),
                                                  testArg(A05, MOVE_05),
                                                  testArg(A06, MOVE_06),
                                                  testArg(A07, MOVE_07),
                                                  testArg(A08, MOVE_08),
                                                  testArg(A09, MOVE_09),
                                                  testArg(A10, MOVE_10),
                                                  testArg(A11, MOVE_11),
                                                  testArg(A12, MOVE_12),
                                                  testArg(A13, MOVE_13),
                                                  testArg(A14, MOVE_14));
        }  break;
    };
    bslalg::AutoScalarDestructor<MyInplaceTestObject> proctor(
                                                             &buffer.object());

    ASSERTV(MOVE_01, A01.movedFrom(),
            MOVE_01 == (MoveState::e_MOVED == A01.movedFrom()));
    ASSERTV(MOVE_02, A02.movedFrom(),
            MOVE_02 == (MoveState::e_MOVED == A02.movedFrom()));
    ASSERTV(MOVE_03, A03.movedFrom(),
            MOVE_03 == (MoveState::e_MOVED == A03.movedFrom()));
    ASSERTV(MOVE_04, A04.movedFrom(),
            MOVE_04 == (MoveState::e_MOVED == A04.movedFrom()));
    ASSERTV(MOVE_05, A05.movedFrom(),
            MOVE_05 == (MoveState::e_MOVED == A05.movedFrom()));
    ASSERTV(MOVE_06, A06.movedFrom(),
            MOVE_06 == (MoveState::e_MOVED == A06.movedFrom()));
    ASSERTV(MOVE_07, A07.movedFrom(),
            MOVE_07 == (MoveState::e_MOVED == A07.movedFrom()));
    ASSERTV(MOVE_08, A08.movedFrom(),
            MOVE_08 == (MoveState::e_MOVED == A08.movedFrom()));
    ASSERTV(MOVE_09, A09.movedFrom(),
            MOVE_09 == (MoveState::e_MOVED == A09.movedFrom()));
    ASSERTV(MOVE_10, A10.movedFrom(),
            MOVE_10 == (MoveState::e_MOVED == A10.movedFrom()));
    ASSERTV(MOVE_11, A11.movedFrom(),
            MOVE_11 == (MoveState::e_MOVED == A11.movedFrom()));
    ASSERTV(MOVE_12, A12.movedFrom(),
            MOVE_12 == (MoveState::e_MOVED == A12.movedFrom()));
    ASSERTV(MOVE_13, A13.movedFrom(),
            MOVE_13 == (MoveState::e_MOVED == A13.movedFrom()));
    ASSERTV(MOVE_14, A14.movedFrom(),
            MOVE_14 == (MoveState::e_MOVED == A14.movedFrom()));

    // The next blocks is testing 'MyInplaceTestObject', and belongs in its own
    // test driver.  Retained for now.

    ASSERTV(V01, EXP.arg01(), V01 == EXP.arg01() || 2 == N01);
    ASSERTV(V02, EXP.arg02(), V02 == EXP.arg02() || 2 == N02);
    ASSERTV(V03, EXP.arg03(), V03 == EXP.arg03() || 2 == N03);
    ASSERTV(V04, EXP.arg04(), V04 == EXP.arg04() || 2 == N04);
    ASSERTV(V05, EXP.arg05(), V05 == EXP.arg05() || 2 == N05);
    ASSERTV(V06, EXP.arg06(), V06 == EXP.arg06() || 2 == N06);
    ASSERTV(V07, EXP.arg07(), V07 == EXP.arg07() || 2 == N07);
    ASSERTV(V08, EXP.arg08(), V08 == EXP.arg08() || 2 == N08);
    ASSERTV(V09, EXP.arg09(), V09 == EXP.arg09() || 2 == N09);
    ASSERTV(V10, EXP.arg10(), V10 == EXP.arg10() || 2 == N10);
    ASSERTV(V11, EXP.arg11(), V11 == EXP.arg11() || 2 == N11);
    ASSERTV(V12, EXP.arg12(), V12 == EXP.arg12() || 2 == N12);
    ASSERTV(V13, EXP.arg13(), V13 == EXP.arg13() || 2 == N13);
    ASSERTV(V14, EXP.arg14(), V14 == EXP.arg14() || 2 == N14);

    // Here starts the actual test case
    bslma::TestAllocatorMonitor dam(da);

    bsls::Types::Int64 numAllocations   = ta.numAllocations();
    bsls::Types::Int64 numDeallocations = ta.numDeallocations();

    {
        MyTestArg01 B01(V01);
        MyTestArg02 B02(V02);
        MyTestArg03 B03(V03);
        MyTestArg04 B04(V04);
        MyTestArg05 B05(V05);
        MyTestArg06 B06(V06);
        MyTestArg07 B07(V07);
        MyTestArg08 B08(V08);
        MyTestArg09 B09(V09);
        MyTestArg10 B10(V10);
        MyTestArg11 B11(V11);
        MyTestArg12 B12(V12);
        MyTestArg13 B13(V13);
        MyTestArg14 B14(V14);

        bsl::shared_ptr<const MyInplaceTestObject> x;

        switch (N_ARGS) {
          case 0: {
            x = bsl::allocate_shared<const MyInplaceTestObject>(
                                                        basicAllocator);
          } break;
          case 1: {
            x = bsl::allocate_shared<const MyInplaceTestObject>(
                                                        basicAllocator,
                                                        testArg(B01, MOVE_01));
          } break;
          case 2: {
            x = bsl::allocate_shared<const MyInplaceTestObject>(
                                                        basicAllocator,
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02));
          } break;
          case 3: {
            x = bsl::allocate_shared<const MyInplaceTestObject>(
                                                        basicAllocator,
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03));
          } break;
          case 4: {
            x = bsl::allocate_shared<const MyInplaceTestObject>(
                                                        basicAllocator,
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04));
          } break;
          case 5: {
            x = bsl::allocate_shared<const MyInplaceTestObject>(
                                                        basicAllocator,
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04),
                                                        testArg(B05, MOVE_05));
          } break;
          case 6: {
            x = bsl::allocate_shared<const MyInplaceTestObject>(
                                                        basicAllocator,
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04),
                                                        testArg(B05, MOVE_05),
                                                        testArg(B06, MOVE_06));
          } break;
          case 7: {
            x = bsl::allocate_shared<const MyInplaceTestObject>(
                                                        basicAllocator,
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04),
                                                        testArg(B05, MOVE_05),
                                                        testArg(B06, MOVE_06),
                                                        testArg(B07, MOVE_07));
          } break;
          case 8: {
            x = bsl::allocate_shared<const MyInplaceTestObject>(
                                                        basicAllocator,
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04),
                                                        testArg(B05, MOVE_05),
                                                        testArg(B06, MOVE_06),
                                                        testArg(B07, MOVE_07),
                                                        testArg(B08, MOVE_08));
          } break;
          case 9: {
            x = bsl::allocate_shared<const MyInplaceTestObject>(
                                                        basicAllocator,
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04),
                                                        testArg(B05, MOVE_05),
                                                        testArg(B06, MOVE_06),
                                                        testArg(B07, MOVE_07),
                                                        testArg(B08, MOVE_08),
                                                        testArg(B09, MOVE_09));
          } break;
          case 10: {
            x = bsl::allocate_shared<const MyInplaceTestObject>(
                                                        basicAllocator,
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04),
                                                        testArg(B05, MOVE_05),
                                                        testArg(B06, MOVE_06),
                                                        testArg(B07, MOVE_07),
                                                        testArg(B08, MOVE_08),
                                                        testArg(B09, MOVE_09),
                                                        testArg(B10, MOVE_10));
          } break;
          case 11: {
            x = bsl::allocate_shared<const MyInplaceTestObject>(
                                                        basicAllocator,
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04),
                                                        testArg(B05, MOVE_05),
                                                        testArg(B06, MOVE_06),
                                                        testArg(B07, MOVE_07),
                                                        testArg(B08, MOVE_08),
                                                        testArg(B09, MOVE_09),
                                                        testArg(B10, MOVE_10),
                                                        testArg(B11, MOVE_11));
          } break;
          case 12: {
            x = bsl::allocate_shared<const MyInplaceTestObject>(
                                                        basicAllocator,
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04),
                                                        testArg(B05, MOVE_05),
                                                        testArg(B06, MOVE_06),
                                                        testArg(B07, MOVE_07),
                                                        testArg(B08, MOVE_08),
                                                        testArg(B09, MOVE_09),
                                                        testArg(B10, MOVE_10),
                                                        testArg(B11, MOVE_11),
                                                        testArg(B12, MOVE_12));
          } break;
          case 13: {
            x = bsl::allocate_shared<const MyInplaceTestObject>(
                                                        basicAllocator,
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04),
                                                        testArg(B05, MOVE_05),
                                                        testArg(B06, MOVE_06),
                                                        testArg(B07, MOVE_07),
                                                        testArg(B08, MOVE_08),
                                                        testArg(B09, MOVE_09),
                                                        testArg(B10, MOVE_10),
                                                        testArg(B11, MOVE_11),
                                                        testArg(B12, MOVE_12),
                                                        testArg(B13, MOVE_13));
          } break;
          case 14: {
            x = bsl::allocate_shared<const MyInplaceTestObject>(
                                                        basicAllocator,
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04),
                                                        testArg(B05, MOVE_05),
                                                        testArg(B06, MOVE_06),
                                                        testArg(B07, MOVE_07),
                                                        testArg(B08, MOVE_08),
                                                        testArg(B09, MOVE_09),
                                                        testArg(B10, MOVE_10),
                                                        testArg(B11, MOVE_11),
                                                        testArg(B12, MOVE_12),
                                                        testArg(B13, MOVE_13),
                                                        testArg(B14, MOVE_14));
          } break;
        };

        ASSERT(++numAllocations == ta.numAllocations());
        ASSERT(1 == x.use_count());
        ASSERT(EXP == *x);

        ASSERT(A01.movedFrom() == B01.movedFrom());
        ASSERT(A02.movedFrom() == B02.movedFrom());
        ASSERT(A03.movedFrom() == B03.movedFrom());
        ASSERT(A04.movedFrom() == B04.movedFrom());
        ASSERT(A05.movedFrom() == B05.movedFrom());
        ASSERT(A06.movedFrom() == B06.movedFrom());
        ASSERT(A07.movedFrom() == B07.movedFrom());
        ASSERT(A08.movedFrom() == B08.movedFrom());
        ASSERT(A09.movedFrom() == B09.movedFrom());
        ASSERT(A10.movedFrom() == B10.movedFrom());
        ASSERT(A11.movedFrom() == B11.movedFrom());
        ASSERT(A12.movedFrom() == B12.movedFrom());
        ASSERT(A13.movedFrom() == B13.movedFrom());
        ASSERT(A14.movedFrom() == B14.movedFrom());
    }
    ASSERT(++numDeallocations == ta.numDeallocations());

    ASSERT(dam.isMaxSame());
    ASSERT(dam.isInUseSame());
}

template <int N_ARGS,
          int N01,
          int N02,
          int N03,
          int N04,
          int N05,
          int N06,
          int N07,
          int N08,
          int N09,
          int N10,
          int N11,
          int N12,
          int N13,
          int N14>
void Harness::testCase34_AllocatorAware()
{
    bslma::TestAllocator *da =
             dynamic_cast<bslma::TestAllocator *>(bslma::Default::allocator());
    BSLS_ASSERT(da);

    bslma::TestAllocator ta("test allocator for allocator-aware testCase34",
                            veryVeryVeryVerbose);

    DECLARE_BOOL_CONSTANT(MOVE_01, N01 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_02, N02 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_03, N03 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_04, N04 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_05, N05 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_06, N06 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_07, N07 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_08, N08 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_09, N09 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_10, N10 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_11, N11 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_12, N12 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_13, N13 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_14, N14 == 1);

    // First prepare a test object to compare against the shared object
    // constructed by allocate_shared.  In particular, we want to pay attention
    // to the moved-from state of each argument, and the final value of the
    // whole test object.

    bslma::TestAllocator localArgAlloc("local arguments allocator",
                                               veryVeryVeryVerbose);

    // 14 arguments, all using a local test allocator
    MyAllocatableArg01 A01(VA01, &localArgAlloc);
    MyAllocatableArg02 A02(VA02, &localArgAlloc);
    MyAllocatableArg03 A03(VA03, &localArgAlloc);
    MyAllocatableArg04 A04(VA04, &localArgAlloc);
    MyAllocatableArg05 A05(VA05, &localArgAlloc);
    MyAllocatableArg06 A06(VA06, &localArgAlloc);
    MyAllocatableArg07 A07(VA07, &localArgAlloc);
    MyAllocatableArg08 A08(VA08, &localArgAlloc);
    MyAllocatableArg09 A09(VA09, &localArgAlloc);
    MyAllocatableArg10 A10(VA10, &localArgAlloc);
    MyAllocatableArg11 A11(VA11, &localArgAlloc);
    MyAllocatableArg12 A12(VA12, &localArgAlloc);
    MyAllocatableArg13 A13(VA13, &localArgAlloc);
    MyAllocatableArg14 A14(VA14, &localArgAlloc);

    bsls::ObjectBuffer<MyInplaceAllocatableObject> buffer;
    prepareObject<
              N_ARGS,N01,N02,N03,N04,N05,N06,N07,N08,N09,N10,N11,N12,N13,N14>(
    buffer.address(),A01,A02,A03,A04,A05,A06,A07,A08,A09,A10,A11,A12,A13,A14 );

    bslalg::AutoScalarDestructor<MyInplaceAllocatableObject> proctor(
                                                             buffer.address());
    const MyInplaceAllocatableObject& EXP = buffer.object();

    ASSERTV(MOVE_01, A01.movedFrom(),
            MOVE_01 == (MoveState::e_MOVED == A01.movedFrom()));
    ASSERTV(MOVE_02, A02.movedFrom(),
            MOVE_02 == (MoveState::e_MOVED == A02.movedFrom()));
    ASSERTV(MOVE_03, A03.movedFrom(),
            MOVE_03 == (MoveState::e_MOVED == A03.movedFrom()));
    ASSERTV(MOVE_04, A04.movedFrom(),
            MOVE_04 == (MoveState::e_MOVED == A04.movedFrom()));
    ASSERTV(MOVE_05, A05.movedFrom(),
            MOVE_05 == (MoveState::e_MOVED == A05.movedFrom()));
    ASSERTV(MOVE_06, A06.movedFrom(),
            MOVE_06 == (MoveState::e_MOVED == A06.movedFrom()));
    ASSERTV(MOVE_07, A07.movedFrom(),
            MOVE_07 == (MoveState::e_MOVED == A07.movedFrom()));
    ASSERTV(MOVE_08, A08.movedFrom(),
            MOVE_08 == (MoveState::e_MOVED == A08.movedFrom()));
    ASSERTV(MOVE_09, A09.movedFrom(),
            MOVE_09 == (MoveState::e_MOVED == A09.movedFrom()));
    ASSERTV(MOVE_10, A10.movedFrom(),
            MOVE_10 == (MoveState::e_MOVED == A10.movedFrom()));
    ASSERTV(MOVE_11, A11.movedFrom(),
            MOVE_11 == (MoveState::e_MOVED == A11.movedFrom()));
    ASSERTV(MOVE_12, A12.movedFrom(),
            MOVE_12 == (MoveState::e_MOVED == A12.movedFrom()));
    ASSERTV(MOVE_13, A13.movedFrom(),
            MOVE_13 == (MoveState::e_MOVED == A13.movedFrom()));
    ASSERTV(MOVE_14, A14.movedFrom(),
            MOVE_14 == (MoveState::e_MOVED == A14.movedFrom()));

    // Here starts the actual test case
    bslma::TestAllocatorMonitor dam(da);

    bsls::Types::Int64 numAllocations   = ta.numAllocations();
    bsls::Types::Int64 numDeallocations = ta.numDeallocations();

    {
        bslma::TestAllocator testArgAlloc("test arguments allocator",
                                          veryVeryVeryVerbose);

        // 14 arguments, all using a specific test allocator
        MyAllocatableArg01 B01(VA01, &testArgAlloc);
        MyAllocatableArg02 B02(VA02, &testArgAlloc);
        MyAllocatableArg03 B03(VA03, &testArgAlloc);
        MyAllocatableArg04 B04(VA04, &testArgAlloc);
        MyAllocatableArg05 B05(VA05, &testArgAlloc);
        MyAllocatableArg06 B06(VA06, &testArgAlloc);
        MyAllocatableArg07 B07(VA07, &testArgAlloc);
        MyAllocatableArg08 B08(VA08, &testArgAlloc);
        MyAllocatableArg09 B09(VA09, &testArgAlloc);
        MyAllocatableArg10 B10(VA10, &testArgAlloc);
        MyAllocatableArg11 B11(VA11, &testArgAlloc);
        MyAllocatableArg12 B12(VA12, &testArgAlloc);
        MyAllocatableArg13 B13(VA13, &testArgAlloc);
        MyAllocatableArg14 B14(VA14, &testArgAlloc);

        bsl::shared_ptr<const MyInplaceAllocatableObject> x;

        switch (N_ARGS) {
          case 0: {
            x = bsl::allocate_shared<const MyInplaceAllocatableObject>(&ta);
          } break;
          case 1: {
            x = bsl::allocate_shared<const MyInplaceAllocatableObject>(
                                                       &ta,
                                                        testArg(B01, MOVE_01));
          } break;
          case 2: {
            x = bsl::allocate_shared<const MyInplaceAllocatableObject>(
                                                       &ta,
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02));
          } break;
          case 3: {
            x = bsl::allocate_shared<const MyInplaceAllocatableObject>(
                                                       &ta,
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03));
          } break;
          case 4: {
            x = bsl::allocate_shared<const MyInplaceAllocatableObject>(
                                                       &ta,
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04));
          } break;
          case 5: {
            x = bsl::allocate_shared<const MyInplaceAllocatableObject>(
                                                       &ta,
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04),
                                                        testArg(B05, MOVE_05));
          } break;
          case 6: {
            x = bsl::allocate_shared<const MyInplaceAllocatableObject>(
                                                       &ta,
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04),
                                                        testArg(B05, MOVE_05),
                                                        testArg(B06, MOVE_06));
          } break;
          case 7: {
            x = bsl::allocate_shared<const MyInplaceAllocatableObject>(
                                                       &ta,
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04),
                                                        testArg(B05, MOVE_05),
                                                        testArg(B06, MOVE_06),
                                                        testArg(B07, MOVE_07));
          } break;
          case 8: {
            x = bsl::allocate_shared<const MyInplaceAllocatableObject>(
                                                       &ta,
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04),
                                                        testArg(B05, MOVE_05),
                                                        testArg(B06, MOVE_06),
                                                        testArg(B07, MOVE_07),
                                                        testArg(B08, MOVE_08));
          } break;
          case 9: {
            x = bsl::allocate_shared<const MyInplaceAllocatableObject>(
                                                       &ta,
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04),
                                                        testArg(B05, MOVE_05),
                                                        testArg(B06, MOVE_06),
                                                        testArg(B07, MOVE_07),
                                                        testArg(B08, MOVE_08),
                                                        testArg(B09, MOVE_09));
          } break;
          case 10: {
            x = bsl::allocate_shared<const MyInplaceAllocatableObject>(
                                                       &ta,
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04),
                                                        testArg(B05, MOVE_05),
                                                        testArg(B06, MOVE_06),
                                                        testArg(B07, MOVE_07),
                                                        testArg(B08, MOVE_08),
                                                        testArg(B09, MOVE_09),
                                                        testArg(B10, MOVE_10));
          } break;
          case 11: {
            x = bsl::allocate_shared<const MyInplaceAllocatableObject>(
                                                       &ta,
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04),
                                                        testArg(B05, MOVE_05),
                                                        testArg(B06, MOVE_06),
                                                        testArg(B07, MOVE_07),
                                                        testArg(B08, MOVE_08),
                                                        testArg(B09, MOVE_09),
                                                        testArg(B10, MOVE_10),
                                                        testArg(B11, MOVE_11));
          } break;
          case 12: {
            x = bsl::allocate_shared<const MyInplaceAllocatableObject>(
                                                       &ta,
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04),
                                                        testArg(B05, MOVE_05),
                                                        testArg(B06, MOVE_06),
                                                        testArg(B07, MOVE_07),
                                                        testArg(B08, MOVE_08),
                                                        testArg(B09, MOVE_09),
                                                        testArg(B10, MOVE_10),
                                                        testArg(B11, MOVE_11),
                                                        testArg(B12, MOVE_12));
          } break;
          case 13: {
            x = bsl::allocate_shared<const MyInplaceAllocatableObject>(
                                                       &ta,
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04),
                                                        testArg(B05, MOVE_05),
                                                        testArg(B06, MOVE_06),
                                                        testArg(B07, MOVE_07),
                                                        testArg(B08, MOVE_08),
                                                        testArg(B09, MOVE_09),
                                                        testArg(B10, MOVE_10),
                                                        testArg(B11, MOVE_11),
                                                        testArg(B12, MOVE_12),
                                                        testArg(B13, MOVE_13));
          } break;
          case 14: {
            x = bsl::allocate_shared<const MyInplaceAllocatableObject>(
                                                       &ta,
                                                        testArg(B01, MOVE_01),
                                                        testArg(B02, MOVE_02),
                                                        testArg(B03, MOVE_03),
                                                        testArg(B04, MOVE_04),
                                                        testArg(B05, MOVE_05),
                                                        testArg(B06, MOVE_06),
                                                        testArg(B07, MOVE_07),
                                                        testArg(B08, MOVE_08),
                                                        testArg(B09, MOVE_09),
                                                        testArg(B10, MOVE_10),
                                                        testArg(B11, MOVE_11),
                                                        testArg(B12, MOVE_12),
                                                        testArg(B13, MOVE_13),
                                                        testArg(B14, MOVE_14));
          } break;
        };

        ASSERTV(numAllocations + N_ARGS + 1,   ta.numAllocations(),
                numAllocations + N_ARGS + 1 == ta.numAllocations());
        ASSERTV(numDeallocations,   ta.numDeallocations(),
                numDeallocations == ta.numDeallocations());

        numAllocations   = ta.numAllocations();
        numDeallocations = ta.numDeallocations();

        ASSERT(1 == x.use_count());
        ASSERT(EXP == *x);
        ASSERTV(&ta, x->allocator(), &ta == x->allocator() );

        ASSERT(A01.movedFrom() == B01.movedFrom());
        ASSERT(A02.movedFrom() == B02.movedFrom());
        ASSERT(A03.movedFrom() == B03.movedFrom());
        ASSERT(A04.movedFrom() == B04.movedFrom());
        ASSERT(A05.movedFrom() == B05.movedFrom());
        ASSERT(A06.movedFrom() == B06.movedFrom());
        ASSERT(A07.movedFrom() == B07.movedFrom());
        ASSERT(A08.movedFrom() == B08.movedFrom());
        ASSERT(A09.movedFrom() == B09.movedFrom());
        ASSERT(A10.movedFrom() == B10.movedFrom());
        ASSERT(A11.movedFrom() == B11.movedFrom());
        ASSERT(A12.movedFrom() == B12.movedFrom());
        ASSERT(A13.movedFrom() == B13.movedFrom());
        ASSERT(A14.movedFrom() == B14.movedFrom());
    }
    ASSERTV(numAllocations,   ta.numAllocations(),
            numAllocations == ta.numAllocations());
    ASSERTV(numDeallocations + N_ARGS + 1,   ta.numDeallocations(),
            numDeallocations + N_ARGS + 1 == ta.numDeallocations());

    ASSERT(dam.isInUseSame());
}

template <class T, class Y>
void Harness::testCase38(int value)
{
    // ------------------------------------------------------------------------
    // 'noexcept' SPECIFICATION
    //
    // Concerns:
    //: 1 The 'noexcept' specification has been applied to all class interfaces
    //:   required by the standard.
    //
    // Plan:
    //: 1 Apply the unary 'noexcept' operator to expressions that mimic those
    //:   appearing in the standard and confirm that calculated boolean value
    //:   matches the expected value.
    //:
    //: 2 Since the 'noexcept' specification does not vary with the 'TYPE' of
    //:   the container, we need test for just one general type and any 'TYPE'
    //:   specializations.
    //
    // Testing:
    //   CONCERN: Methods qualifed 'noexcept' in standard are so implemented.
    // ------------------------------------------------------------------------

    if (verbose) {
        P(bsls::NameOf<T>())
        P(bsls::NameOf<Y>())
        P(value)
    }

    if (verbose) {
        printf("bsl::shared_ptr<T>\n");
    }

    // N4594: page 590: 20.10.2.2 Class template 'shared_ptr'

    // page 590
    //..
    //  // 20.10.2.2.1, constructors:
    //  constexpr shared_ptr() noexcept;
    //  template<class Y> shared_ptr(const shared_ptr<Y>& r, T* p) noexcept;
    //  shared_ptr(const shared_ptr& r) noexcept;
    //  template<class Y> shared_ptr(const shared_ptr<Y>& r) noexcept;
    //  shared_ptr(shared_ptr&& r) noexcept;
    //  template<class Y> shared_ptr(shared_ptr<Y>&& r) noexcept;
    //  constexpr shared_ptr(nullptr_t) noexcept : shared_ptr() { }
    //..

    {
        bsl::shared_ptr<Y> r;
        T                  p;

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(bsl::shared_ptr<T>()));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(bsl::shared_ptr<T>(r, &p)));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(bsl::shared_ptr<T>(r)));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(
                          bsl::shared_ptr<T>(MoveUtil::move(r))));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(
                          bsl::shared_ptr<T>(MoveUtil::move(r))));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(bsl::shared_ptr<T>(0)));
    }

    // page 590
    //..
    //  // 20.10.2.2.3, assignment:
    //  shared_ptr& operator=(const shared_ptr& r) noexcept;
    //  template<class Y> shared_ptr& operator=(const shared_ptr<Y>& r)
    //                                                                noexcept;
    //  shared_ptr& operator=(shared_ptr&& r) noexcept;
    //  template<class Y> shared_ptr& operator=(shared_ptr<Y>&& r) noexcept;
    //..

    {
        bsl::shared_ptr<T> mX;
        bsl::shared_ptr<T> mR; const bsl::shared_ptr<T>& R = mR;

        bsl::shared_ptr<Y> mZ; const bsl::shared_ptr<Y>& Z = mZ;

        (void) R;  // suppress compiler warning
        (void) Z;  // suppress compiler warning

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(mX = R));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(mX = Z));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(mX = MoveUtil::move(mR)));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(mX = MoveUtil::move(mZ)));
    }

    // page 591
    //..
    //  // 20.10.2.2.4, modifiers:
    //  void swap(shared_ptr& r) noexcept;
    //  void reset() noexcept;
    //..

    {
        bsl::shared_ptr<T> mX;
        bsl::shared_ptr<T> mR;

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(mX.swap(mR)));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(mX.reset()));
    }

    // page 591
    //..
    //  // 20.10.2.2.5, observers:
    //  T* get() const noexcept;
    //  T& operator*() const noexcept;
    //  T* operator->() const noexcept;
    //  long use_count() const noexcept;
    //  bool unique() const noexcept;
    //  explicit operator bool() const noexcept;
    //..

    {
        bsl::shared_ptr<T> mX; const bsl::shared_ptr<T>& X = mX;
        (void) X;  // suppress compiler warning

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(X.get()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(X.operator*()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(X.operator->()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(X.unique()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(static_cast<bool>(X)));
    }

    // page 591 - 592
    //..
    //  // 20.10.2.2.7, shared_ptr comparisons:
    //  template<class T, class U>
    //  bool operator==(const shared_ptr<T>& a, const shared_ptr<U>& b)
    //                                                                noexcept;
    //  template<class T, class U>
    //  bool operator!=(const shared_ptr<T>& a, const shared_ptr<U>& b)
    //                                                                noexcept;
    //  template<class T, class U>
    //  bool operator<(const shared_ptr<T>& a, const shared_ptr<U>& b)
    //                                                                noexcept;
    //  template<class T, class U>
    //  bool operator>(const shared_ptr<T>& a, const shared_ptr<U>& b)
    //                                                                noexcept;
    //  template<class T, class U>
    //  bool operator<=(const shared_ptr<T>& a, const shared_ptr<U>& b)
    //                                                                noexcept;
    //  template<class T, class U>
    //  bool operator>=(const shared_ptr<T>& a, const shared_ptr<U>& b)
    //                                                                noexcept;
    //  template <class T>
    //  bool operator==(const shared_ptr<T>& a, nullptr_t) noexcept;
    //  template <class T>
    //  bool operator==(nullptr_t, const shared_ptr<T>& b) noexcept;
    //  template <class T>
    //  bool operator!=(const shared_ptr<T>& a, nullptr_t) noexcept;
    //  template <class T>
    //  bool operator!=(nullptr_t, const shared_ptr<T>& b) noexcept;
    //  template <class T>
    //  bool operator<(const shared_ptr<T>& a, nullptr_t) noexcept;
    //  template <class T>
    //  bool operator<(nullptr_t, const shared_ptr<T>& b) noexcept;
    //  template <class T>
    //  bool operator<=(const shared_ptr<T>& a, nullptr_t) noexcept;
    //  template <class T>
    //  bool operator<=(nullptr_t, const shared_ptr<T>& b) noexcept;
    //  template <class T>
    //  bool operator>(const shared_ptr<T>& a, nullptr_t) noexcept;
    //  template <class T>
    //  bool operator>(nullptr_t, const shared_ptr<T>& b) noexcept;
    //  template <class T>
    //  bool operator>=(const shared_ptr<T>& a, nullptr_t) noexcept;
    //  template <class T>
    //  bool operator>=(nullptr_t, const shared_ptr<T>& b) noexcept;
    //..

    {
        typedef Y U;

        bsl::shared_ptr<T> mA; const bsl::shared_ptr<T>& A = mA;
        bsl::shared_ptr<U> mB; const bsl::shared_ptr<U>& B = mB;
        (void) A;  // suppress compiler warning
        (void) B;  // suppress compiler warning

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(A == B));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(A != B));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(A <  B));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(A >  B));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(A <= B));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(A >= B));

        // Test 'bsl::nullptr_t' overloads

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(A == 0));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(0 == B));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(A != 0));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(0 != B));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(A <  0));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(0 <  B));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(A >  0));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(0 >  B));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(A <= 0));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(0 <= B));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(A >= 0));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(0 >= B));
    }

    // page 592
    //..
    //  // 20.10.2.2.8, shared_ptr specialized algorithms:
    //  template<class T> void swap(shared_ptr<T>& a, shared_ptr<T>& b)
    //                                                                noexcept;
    //..

    {
        bsl::shared_ptr<T> mA;
        bsl::shared_ptr<T> mB;

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(swap(mA, mB)));
    }

    // page 592
    //..
    //  // 20.10.2.2.9, shared_ptr casts:
    //  template<class T, class U>
    //  shared_ptr<T> static_pointer_cast(const shared_ptr<U>& r) noexcept;
    //  template<class T, class U>
    //  shared_ptr<T> dynamic_pointer_cast(const shared_ptr<U>& r) noexcept;
    //  template<class T, class U>
    //  shared_ptr<T> const_pointer_cast(const shared_ptr<U>& r) noexcept;
    //..

    {
        typedef Y U;

        bsl::shared_ptr<U> mR; const bsl::shared_ptr<U>& R = mR;
        (void) R;  // suppress compiler warning

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(bsl:: static_pointer_cast<T>(R)));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(bsl::dynamic_pointer_cast<T>(R)));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(bsl::  const_pointer_cast<T>(R)));
    }

    // page 592
    //..
    //  // 20.10.2.2.10, shared_ptr get_deleter:
    //  template<class D, class T> D* get_deleter(const shared_ptr<T>& p)
    //                                                                noexcept;
    //..

    {
        bsl::shared_ptr<T> mX; const bsl::shared_ptr<T>& X = mX;
        (void) X;  // suppress compiler warning

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(bsl::get_deleter<T>(X)));
    }

    if (verbose) {
        printf("bsl::weak_ptr<T>\n");
    }

    // N4594: page 598: 20.10.2.3 Class template weak_ptr [util.smartptr.weak]

    // page 599
    //..
    //  // 20.10.2.3.1, constructors
    //  constexpr weak_ptr() noexcept;
    //  template<class Y> weak_ptr(shared_ptr<Y> const& r) noexcept;
    //  weak_ptr(weak_ptr const& r) noexcept;
    //  template<class Y> weak_ptr(weak_ptr<Y> const& r) noexcept;
    //  weak_ptr(weak_ptr&& r) noexcept;
    //  template<class Y> weak_ptr(weak_ptr<Y>&& r) noexcept;
    //..

    {
        {
            ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
                == BSLS_KEYWORD_NOEXCEPT_OPERATOR(bsl::weak_ptr<T>()));
        }
        {
            bsl::shared_ptr<Y> mR; const bsl::shared_ptr<Y>& r = mR;
            (void) r;  // suppress compiler warning

            ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
                == BSLS_KEYWORD_NOEXCEPT_OPERATOR(bsl::weak_ptr<T>(r)));
        }
        {
            bsl::weak_ptr<T> mR; const bsl::weak_ptr<T>& r = mR;
            (void) r;  // suppress compiler warning

            ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
                == BSLS_KEYWORD_NOEXCEPT_OPERATOR(bsl::weak_ptr<T>(r)));
        }
        {
            bsl::weak_ptr<Y> mR; const bsl::weak_ptr<Y>& r = mR;
            (void) r;  // suppress compiler warning

            ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
                == BSLS_KEYWORD_NOEXCEPT_OPERATOR(bsl::weak_ptr<T>(r)));
        }
        {
            bsl::weak_ptr<T> r;

            ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
                == BSLS_KEYWORD_NOEXCEPT_OPERATOR(bsl::weak_ptr<T>(
                                                          MoveUtil::move(r))));
        }
        {
            bsl::weak_ptr<Y> r;

            ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
                == BSLS_KEYWORD_NOEXCEPT_OPERATOR(bsl::weak_ptr<T>(
                                                          MoveUtil::move(r))));
        }
    }

    // page 599
    //..
    //  // 20.10.2.3.3, assignment
    //  weak_ptr& operator=(weak_ptr const& r) noexcept;
    //  template<class Y> weak_ptr& operator=(weak_ptr<Y> const& r) noexcept;
    //  template<class Y> weak_ptr& operator=(shared_ptr<Y> const& r) noexcept;
    //  weak_ptr& operator=(weak_ptr&& r) noexcept;
    //  template<class Y> weak_ptr& operator=(weak_ptr<Y>&& r) noexcept;
    //..

    {
        {
            bsl::weak_ptr<T> mX;
            bsl::weak_ptr<T> mR; const bsl::weak_ptr<T>& r = mR;
            (void) r;  // suppress compiler warning

            ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
                == BSLS_KEYWORD_NOEXCEPT_OPERATOR(mX = r));
        }
        {
            bsl::weak_ptr<T> mX;
            bsl::weak_ptr<Y> mR; const bsl::weak_ptr<Y>& r = mR;
            (void) r;  // suppress compiler warning

            ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
                == BSLS_KEYWORD_NOEXCEPT_OPERATOR(mX = r));
        }
        {
            bsl::weak_ptr<T>   mX;
            bsl::shared_ptr<Y> mR; const bsl::shared_ptr<Y>& r = mR;
            (void) r;  // suppress compiler warning

            ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
                == BSLS_KEYWORD_NOEXCEPT_OPERATOR(mX = r));
        }
        {
            bsl::weak_ptr<T> mX;
            bsl::weak_ptr<T>  r;

            ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
                == BSLS_KEYWORD_NOEXCEPT_OPERATOR(mX = MoveUtil::move(r)));
        }
        {
            bsl::weak_ptr<T> mX;
            bsl::weak_ptr<Y>  r;

            ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
                == BSLS_KEYWORD_NOEXCEPT_OPERATOR(mX = MoveUtil::move(r)));
        }
    }

    // page 599
    //..
    //  // 20.10.2.3.4, modifiers
    //  void swap(weak_ptr& r) noexcept;
    //  void reset() noexcept;
    //..

    {
        bsl::weak_ptr<T> x;
        bsl::weak_ptr<T> r;

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(x.swap(r)));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(x.reset()));
    }

    // page 599
    //..
    //  // 20.10.2.3.5, observers
    //  long use_count() const noexcept;
    //  bool expired() const noexcept;
    //  shared_ptr<T> lock() const noexcept;
    //..

    {
        bsl::weak_ptr<T> mR; const bsl::weak_ptr<T>& R = mR;
        (void) R;  // suppress compiler warning

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(R.use_count()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(R.expired()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(R.lock()));
    }

    // page 599
    //..
    //  // 20.10.2.3.6, specialized algorithms
    //  template<class T> void swap(weak_ptr<T>& a, weak_ptr<T>& b) noexcept;
    //..

    {
        bsl::weak_ptr<T> a;
        bsl::weak_ptr<T> b;

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(swap(a, b)));
    }
}

template <int N>
void testMyTestArg()
{
    if (veryVerbose) {
        printf("\nMyTestArg<%d>\n", N);
    }

    bslma::TestAllocator *da_p = dynamic_cast<bslma::TestAllocator *>(
                                                  bslma::Default::allocator());
    ASSERT(0 != da_p);
    if (!da_p) {     // Avoid undefined behavior, this test has already failed.
        return;                                                       // RETURN
    }

    bsls::Types::Int64 numAllocations   = da_p->numAllocations();
    bsls::Types::Int64 numDeallocations = da_p->numDeallocations();

    {
        const bsltf::ArgumentType<N>  V1;
        const bsltf::ArgumentType<N>  V2 = V1;

        ASSERT(V1 == V2);
        ASSERT(-1 == V1);
        ASSERT(-1 == V2);
    }

    ASSERT(da_p->numAllocations()   == numAllocations);
    ASSERT(da_p->numDeallocations() == numDeallocations);
}

template <int N>
void testMyTestArg(const int value)
{
    if (veryVerbose) {
        printf("\nMyTestArg<%d>\n", N);
    }

    bslma::TestAllocator *da_p = dynamic_cast<bslma::TestAllocator *>(
                                                  bslma::Default::allocator());
    ASSERT(0 != da_p);
    if (!da_p) {     // Avoid undefined behavior, this test has already failed.
        return;                                                       // RETURN
    }

    bsls::Types::Int64 numAllocations   = da_p->numAllocations();
    bsls::Types::Int64 numDeallocations = da_p->numDeallocations();

    {
        const bsltf::ArgumentType<N>  V1(value);
        const bsltf::ArgumentType<N>  V2 = V1;

        ASSERT(V1 == V2);
        ASSERT(value == V1);
        ASSERT(value == V2);
    }

    ASSERT(da_p->numAllocations()   == numAllocations);
    ASSERT(da_p->numDeallocations() == numDeallocations);
}

template <int N>
void testMyAllocatableArg()
{
    if (veryVerbose) {
        printf("\tMyAllocatableArg<%d>\n", N);
    }

    bslma::TestAllocator *da_p = dynamic_cast<bslma::TestAllocator *>(
                                                  bslma::Default::allocator());
    ASSERT(0 != da_p);
    if (!da_p) {     // Avoid undefined behavior, this test has already failed.
        return;                                                       // RETURN
    }

    bsls::Types::Int64 numAllocations   = da_p->numAllocations();
    bsls::Types::Int64 numDeallocations = da_p->numDeallocations();

    {
        const bsltf::AllocArgumentType<N> V1;        // +1 use of default alloc
        const bsltf::AllocArgumentType<N> V2 = V1;   // +1 use of default alloc
        ASSERT(V1 == V2);
        ASSERT(-1 == V1);
        ASSERT(-1 == V2);
    }

    ASSERT(da_p->numAllocations()   == 2 + numAllocations);
    ASSERT(da_p->numDeallocations() == 2 + numDeallocations);
}

template <int N>
void testMyAllocatableArg(const int value)
{
    if (veryVerbose) {
        printf("\tMyAllocatableArg<%d>\n", N);
    }

    bslma::TestAllocator *da_p = dynamic_cast<bslma::TestAllocator *>(
                                                  bslma::Default::allocator());
    ASSERT(0 != da_p);
    if (!da_p) {     // Avoid undefined behavior, this test has already failed.
        return;                                                       // RETURN
    }

    bsls::Types::Int64 numAllocations   = da_p->numAllocations();
    bsls::Types::Int64 numDeallocations = da_p->numDeallocations();

    {
        const bsltf::AllocArgumentType<N> V1(value); // +1 use of default alloc
        const bsltf::AllocArgumentType<N> V2 = V1;   // +1 use of default alloc
        ASSERT(value == V1);
        ASSERT(value == V2);
    }

    ASSERT(da_p->numAllocations()   == 2 + numAllocations);
    ASSERT(da_p->numDeallocations() == 2 + numDeallocations);
}


template <int N>
void testMyAllocatableArgWithAllocator()
{
    if (veryVerbose) {
        printf("\tMyAllocatableArg<%d>\n", N);
    }

    bslma::TestAllocator *da_p = dynamic_cast<bslma::TestAllocator *>(
                                                  bslma::Default::allocator());
    ASSERT(0 != da_p);
    if (!da_p) {     // Avoid undefined behavior, this test has already failed.
        return;                                                       // RETURN
    }

    bslma::TestAllocator ta("Test machinery", veryVeryVeryVerbose);

    const bsls::Types::Int64 numDefaultAllocations   = da_p->numAllocations();
    const bsls::Types::Int64 numDefaultDeallocations =
                                                      da_p->numDeallocations();

    const bsls::Types::Int64 numTestAllocations   = ta.numAllocations();
    const bsls::Types::Int64 numTestDeallocations = ta.numDeallocations();

    {
        const bsltf::AllocArgumentType<N>  V1(&ta);
        const bsltf::AllocArgumentType<N>  V2(V1, &ta);
        ASSERT(-1 == V1);
        ASSERT(-1 == V2);
    }

    ASSERT(ta.numAllocations()   == 1 + numTestAllocations);
    ASSERT(ta.numDeallocations() == 1 + numTestDeallocations);

    ASSERT(da_p->numAllocations()   == numDefaultAllocations);
    ASSERT(da_p->numDeallocations() == numDefaultDeallocations);
}

template <int N>
void testMyAllocatableArgWithAllocator(const int value)
{
    if (veryVerbose) {
        printf("\tMyAllocatableArg<%d>\n", N);
    }

    bslma::TestAllocator *da_p = dynamic_cast<bslma::TestAllocator *>(
                                                  bslma::Default::allocator());
    ASSERT(0 != da_p);
    if (!da_p) {     // Avoid undefined behavior, this test has already failed.
        return;                                                       // RETURN
    }

    bslma::TestAllocator ta("Test machinery", veryVeryVeryVerbose);

    const bsls::Types::Int64 numDefaultAllocations   = da_p->numAllocations();
    const bsls::Types::Int64 numDefaultDeallocations =
                                                      da_p->numDeallocations();

    const bsls::Types::Int64 numTestAllocations   = ta.numAllocations();
    const bsls::Types::Int64 numTestDeallocations = ta.numDeallocations();

    {
        const bsltf::AllocArgumentType<N>  V1(value, &ta);
        const bsltf::AllocArgumentType<N>  V2(V1, &ta);
        ASSERT(V1 == V2);
        ASSERT(value == V1);
        ASSERT(value == V2);
    }

    ASSERT(ta.numAllocations()   == 2 + numTestAllocations);
    ASSERT(ta.numDeallocations() == 2 + numTestDeallocations);

    ASSERT(da_p->numAllocations()   == numDefaultAllocations);
    ASSERT(da_p->numDeallocations() == numDefaultDeallocations);
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
struct TestSharedPtrDeductionGuides {
    // This struct provides a namespace for functions testing deduction guides.
    // The tests are compile-time only; it is not necessary that these routines
    // be called at run-time.  Note that the following constructors do not have
    // associated deduction guides because the template parameters for
    // 'bsl::shared_ptr' cannot be deduced from the constructor parameters.
    //..
    // shared_ptr()
    // shared_ptr(bsl::nullptr_t)
    // shared_ptr(bsl::nullptr_t, BloombergLP::bslma::Allocator *)
    // shared_ptr(bsl::nullptr_t, DELETER, BloombergLP::bslma::Allocator *)
    // shared_ptr(bsl::nullptr_t, DELETER, BloombergLP::bslma::Allocator *,
    //                                                 ALLOCATOR::value_type *)
    // shared_ptr(T *);
    // shared_ptr(T *, BloombergLP::bslma::Allocator *);
    // All the converting constructors
    //..
    // We also choose not to deduce from 'auto_ptr' and 'auto_ptr_ref'
    //..
    // shared_ptr(std::auto_ptr<T> &)
    // shared_ptr(std::auto_ptr<T> &, BloombergLP::bslma::Allocator *);
    // shared_ptr(std::auto_ptr_ref<T>)
    // shared_ptr(std::auto_ptr_ref<T>,BloombergLP::bslma::Allocator *);
    //..

#define ASSERT_SAME_TYPE(...) \
 static_assert((bsl::is_same<__VA_ARGS__>::value), "Types differ unexpectedly")

    static void SimpleConstructors ()
        // Test that constructing a 'bsl::shared_ptr' from different smart
        // pointer types and optionally an allocator deduces the correct type.
        //..
        // shared_ptr(const shared_ptr<T>&)
        // shared_ptr(      shared_ptr<T>&&)
        // shared_ptr(unique_ptr<T, D>&&);
        // shared_ptr(unique_ptr<T, D>&&, BloombergLP::bslma::Allocator *);
        // shared_ptr(const weak_ptr<T>&);
        // shared_ptr(      weak_ptr<T>&&);
        // shared_ptr(ManagedPtr<T>&);
        // shared_ptr(ManagedPtr<T>&, BloombergLP::bslma::Allocator *);
        //..
    {
        bslma::Allocator *a = nullptr;

        bsl::shared_ptr<int> sp1;
        bsl::shared_ptr sp1a(sp1);
        ASSERT_SAME_TYPE(decltype(sp1a), bsl::shared_ptr<int>);

        bsl::shared_ptr<float> sp2;
        bsl::shared_ptr sp2a(std::move(sp2));
        ASSERT_SAME_TYPE(decltype(sp2a), bsl::shared_ptr<float>);

        std::unique_ptr<double> up3;
        bsl::shared_ptr sp3a(std::move(up3));
        ASSERT_SAME_TYPE(decltype(sp3a), bsl::shared_ptr<double>);

        std::unique_ptr<long double> up4;
        bsl::shared_ptr sp4a(std::move(up4), a);
        ASSERT_SAME_TYPE(decltype(sp4a), bsl::shared_ptr<long double>);

        bsl::weak_ptr<short> wp5;
        bsl::shared_ptr sp5a(wp5);
        ASSERT_SAME_TYPE(decltype(sp5a), bsl::shared_ptr<short>);

        bsl::weak_ptr<long> wp6;
        bsl::shared_ptr sp6a(std::move(wp6));
        ASSERT_SAME_TYPE(decltype(sp6a), bsl::shared_ptr<long>);

        BloombergLP::bslma::ManagedPtr<char> mp7;
        bsl::shared_ptr sp7a(mp7);
        ASSERT_SAME_TYPE(decltype(sp7a), bsl::shared_ptr<char>);

        BloombergLP::bslma::ManagedPtr<unsigned char> mp8;
        bsl::shared_ptr sp8a(mp8, a);
        ASSERT_SAME_TYPE(decltype(sp8a), bsl::shared_ptr<unsigned char>);

//#define BSLSTL_SHARED_PTR_UNIQUE_PTR_COMPILE_FAIL_NOT_AN_ALLOCATOR
#if defined(BSLSTL_SHARED_PTR_UNIQUE_PTR_COMPILE_FAIL_NOT_AN_ALLOCATOR)
        float                 *pf98 = nullptr;
        std::unique_ptr<char>  up98;
        bsl::shared_ptr        sp99a(up98, pf98);
        // this should fail to compile ('float *' is not an allocator)
#endif

//#define BSLSTL_SHARED_PTR_MANAGED_PTR_COMPILE_FAIL_NOT_AN_ALLOCATOR
#if defined(BSLSTL_SHARED_PTR_MANAGED_PTR_COMPILE_FAIL_NOT_AN_ALLOCATOR)
        float                                *pf99 = nullptr;
        BloombergLP::bslma::ManagedPtr<char>  mp99;
        bsl::shared_ptr                       sp99a(mp99, pf99);
        // this should fail to compile ('float *' is not an allocator)
#endif
    }

#undef ASSERT_SAME_TYPE
};

struct TestWeakPtrDeductionGuides {
    // This struct provides a namespace for functions testing deduction guides.
    // The tests are compile-time only; it is not necessary that these routines
    // be called at run-time.  Note that the following constructors do not have
    // associated deduction guides because the template parameters for
    // 'bsl::weak_ptr' cannot be deduced from the constructor parameters.
    //..
    // weak_ptr()
    // All the converting constructors
    //..

#define ASSERT_SAME_TYPE(...) \
 static_assert((bsl::is_same<__VA_ARGS__>::value), "Types differ unexpectedly")

// Simple constructors

    static void SimpleConstructors ()
        // Test that constructing a 'bsl::weak_ptr' from different smart
        // pointer types deduces the correct type.
        //..
        // weak_ptr(const weak_ptr<T>&)
        // weak_ptr(      weak_ptr<T>&&)
        // weak_ptr(const shared_ptr<T>&);
        //..
    {
        bsl::weak_ptr<int> wp1;
        bsl::weak_ptr      wp1a(wp1);
        ASSERT_SAME_TYPE(decltype(wp1a), bsl::weak_ptr<int>);

        bsl::weak_ptr<double> wp2;
        bsl::weak_ptr         wp2a(std::move(wp2));
        ASSERT_SAME_TYPE(decltype(wp2a), bsl::weak_ptr<double>);

        bsl::shared_ptr<float> sp3;
        bsl::weak_ptr wp3(sp3);
        ASSERT_SAME_TYPE(decltype(wp3), bsl::weak_ptr<float>);
    }

#undef ASSERT_SAME_TYPE
};
#endif


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

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
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

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
      case 46: {
        // ------------------------------------------------------------------
        // TESTING IF 'U_ENABLE_DEPRECATIONS' IS DISABLED
        //
        // Concern:
        //: 1 That we don't ship with 'U_ENABLE_DEPRECATIONS' set.
        //
        // Plan:
        //: 1 Assert that the 'U_ENABLE_DEPRECATIONS' macro is defined as '0'.
        //
        // Testing:
        //   0 == U_ENABLE_DEPRECATIONS
        // ------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING IF 'U_ENABLE_DEPRECATIONS' IS DISABLED"
                   "\n==============================================\n");

        ASSERT(0 == U_ENABLE_DEPRECATIONS);
      } break;
      case 45: {
        //---------------------------------------------------------------------
        // TESTING BITWISE AND NOTHROW MOVEABILITY
        //   Ensure that 'shared_ptr' and 'weak_ptr' are both bitwise moveable
        //   and nothrow move constructible
        //
        // Concerns:
        //: 1 That the test types 'NothrowAndBitwiseMovableTestType' and
        //:   'NonNothrowAndNonBitwiseMovableTestType' satisfy the relevant
        //:   type traits for this test.
        //:
        //: 2 That 'shared_ptr' and 'weak_ptr' are both bitwise moveable and
        //:   nothrow move constructible, regardless of whether the type of the
        //:   object to which they point satisfied these constraints.
        //:
        //
        // Plan:
        //: 1 Verify that 'NothrowAndBitwiseMovableTestType' is both bitwise
        //:   moveable and nothrow move constructible.
        //:
        //: 2 Verify that 'NonNothrowAndNonBitwiseMovableTestType' is neither
        //:   bitwise moveable nor nothrow move constructible.
        //:
        //: 3 Verify that 'shared_ptr<NothrowAndBitwiseMovableTestType>' is
        //:   both bitwise moveable and nothrow move constructible.
        //:
        //: 4 Verify that 'weak_ptr<NothrowAndBitwiseMovableTestType>' is both
        //:   bitwise moveable and nothrow move constructible.
        //:
        //: 5 Verify that 'shared_ptr<NonNothrowAndNonBitwiseMovableTestType>'
        //:   is both bitwise moveable and nothrow move constructible.
        //:
        //: 6 Verify that 'weak_ptr<NonNothrowAndNonBitwiseMovableTestType>' is
        //:   both bitwise moveable and nothrow move constructible.
        //
        // Testing:
        //   TESTING BITWISE AND NOTHROW MOVEABILITY
        //---------------------------------------------------------------------
        if (verbose)
            printf(
              "\nTESTING BITWISE AND NOTHROW MOVEABILITY"
              "\n=======================================\n");

        // Verify the bitwise moveability of the test classes is as expected.
        ASSERT(bslmf::IsBitwiseMoveable<
               NothrowAndBitwiseMovableTestType>::value);
        ASSERT(!bslmf::IsBitwiseMoveable<
               NonNothrowAndNonBitwiseMovableTestType>::value);

        // Verify shared_ptr is always bitwise moveable.
        ASSERT(bslmf::IsBitwiseMoveable<bsl::shared_ptr<
               NothrowAndBitwiseMovableTestType> >::value);
        ASSERT(bslmf::IsBitwiseMoveable<bsl::shared_ptr<
               NonNothrowAndNonBitwiseMovableTestType> >::value);

        // Verify weak_ptr is always bitwise moveable.
        ASSERT(bslmf::IsBitwiseMoveable<bsl::weak_ptr<
               NothrowAndBitwiseMovableTestType> >::value);
        ASSERT(bslmf::IsBitwiseMoveable<bsl::weak_ptr<
               NonNothrowAndNonBitwiseMovableTestType> >::value);

        // Verify the nothrow move constructibility of the test classes is as
        // expected.
        ASSERT(bsl::is_nothrow_move_constructible<
               NothrowAndBitwiseMovableTestType>::value);
        ASSERT(!bsl::is_nothrow_move_constructible<
               NonNothrowAndNonBitwiseMovableTestType>::value);

        // Verify shared_ptr is always nothrow move constructible.
        ASSERT(bsl::is_nothrow_move_constructible<bsl::shared_ptr<
               NothrowAndBitwiseMovableTestType> >::value);
        ASSERT(bsl::is_nothrow_move_constructible<bsl::shared_ptr<
               NonNothrowAndNonBitwiseMovableTestType> >::value);

        // Verify weak_ptr is always nothrow move constructible.
        ASSERT(bsl::is_nothrow_move_constructible<bsl::weak_ptr<
               NothrowAndBitwiseMovableTestType> >::value);
        ASSERT(bsl::is_nothrow_move_constructible<bsl::weak_ptr<
               NonNothrowAndNonBitwiseMovableTestType> >::value);

      } break;
      case 44: {
        //---------------------------------------------------------------------
        // TESTING CLASS TEMPLATE DEDUCTION GUIDES (AT COMPILE TIME)
        //   Ensure that the deduction guides are properly specified to deduce
        //   the template arguments from the arguments supplied to the
        //   constructors.
        //
        // Concerns:
        //: 1 Constructing a 'shared_ptr' from a 'weak_ptr' deduces the
        //:   arguments.
        //:
        //: 2 Constructing a 'shared_ptr' from a 'ManagedPtr' with an optional
        //:   allocator deduces the arguments.
        //:
        //: 3 Constructing a 'shared_ptr' from a 'unique_ptr' with an optional
        //:   allocator deduces the  arguments.
        //:
        //: 4 Constructing a 'weak_ptr' from a 'shared_ptr' deduces the
        //:   arguments.
        //:
        //
        // Plan:
        //: 1 Create a shared_ptr/weak_ptr by invoking the constructor without
        //:   supplying the template arguments explicitly.
        //:
        //: 2 Verify that the deduced type is correct.
        //
        // Testing:
        //   CLASS TEMPLATE DEDUCTION GUIDES
        //---------------------------------------------------------------------
        if (verbose)
            printf(
              "\nTESTING CLASS TEMPLATE DEDUCTION GUIDES (AT COMPILE TIME)"
              "\n=========================================================\n");

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
        // This is a compile-time only test case.
        TestSharedPtrDeductionGuides testShared;
        (void) testShared; // This variable exists for ease of IDE navigation.

        TestWeakPtrDeductionGuides   testWeak;
        (void) testWeak;   // This variable exists for ease of IDE navigation.
#endif
      } break;
      case 43: {
        // --------------------------------------------------------------------
        // TESTING REGRESSIONS
        //   This test case verifies that fixed bugs did not creep back.
        //
        // Concerns:
        //:  1 A 'shared_ptr' to a class inheriting from
        //:    'enable_shared_from_this' can be constructed from a non-literal
        //:    NULL pointer without triggering an assert. {DRQS 165213908}
        //
        // Plan:
        //: 1 Create a pointer to 'ShareThis' that is initialized to be a NULL
        //:   pointer.  In exception enabled builds use the assertion testing
        //:   facility to verify that no assertion fails while constructing a
        //:   'shared_ptr<ShareThis>' instance from the former non-literal NULL
        //:   pointer.  In builds with exceptions disabled just create the
        //:   shared pointer and allow it to assert (if asserts are enabled).
        //
        // Testing:
        //   REGRESSIONS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING REGRESSIONS"
                            "\n===================\n");

        ShareThis *np = 0;

#if defined(BDE_BUILD_TARGET_EXC)
        bsls::AssertTestHandlerGuard g; (void)g;
        ASSERT_PASS(bsl::shared_ptr<ShareThis> obj(np));
#endif
        bsl::shared_ptr<ShareThis> obj(np);

        ASSERT(0 == obj.get());
    } break;
    case 42: {
        // --------------------------------------------------------------------
        // TESTING THE TEST MACHINERY
        //   This test case will test all of the test machinery supplied by the
        //   test driver prior to its use to prove the rest of the component
        //   works as required.
        //
        //   TBD: THIS TEST CASE HAS A VERY HIGH NUMBER AS IT WAS MISSED AS A
        //        MAJOR OVERSIGHT.  IT WILL MOVE DOWN TO TEST CASE 3 REQUIRING
        //        A WHOLESALE RENUMBERING, ONCE IT IS COMPLETE AND HAS PROVEN
        //        ITSELF CORRECT.
        //
        // Concerns:
        //:  1 ...
        //
        // Plan:
        //: 1 Provide at least a breathing test for all of the test machinery
        //:   that remains untested.
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING THE TEST MACHINERY"
                            "\n==========================\n");

        bslma::TestAllocator *da_p = dynamic_cast<bslma::TestAllocator *>(
                                                  bslma::Default::allocator());
        ASSERT(0 != da_p);

      } break;
      case 41: {
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
      case 40: {
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
      case 39: {
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
      case 38: {
        // --------------------------------------------------------------------
        // 'noexcept' SPECIFICATION
        //
        //  See: 'Harness::testCase38'
        // --------------------------------------------------------------------

        if (verbose) printf("\n" "'noexcept' SPECIFICATION" "\n"
                                 "========================" "\n");

        Harness::testCase38<MyTestBaseObject, MyTestObject2>(40);

      } break;
      case 37: {
        // --------------------------------------------------------------------
        // TESTING 'shared_ptr' OF BDE ALLOCATORS
        //   Confirm that there are no complications for 'shared_ptr's of BDE
        //   style allocators, where a pointer to an intended target object may
        //   be confused with use as an allocator or factory deleter.
        //
        // Concerns:
        //: 1 There are no ambiguities when constructing a shared (or weak)
        //:   pointer to a 'bslma::Allocator' when passes a pointer to the
        //:   exact base type, or to an allocator implementing the protocol.
        //: 2 There are no ambiguities when constructing a shared (or weak)
        //:   pointer to a 'bslma::TestAllocator', or to any other type that
        //:   implements the 'bslma::Allocator' protocol
        //
        // Plan:
        //: 1 Exercise all the tests from test case 36, substituting
        //:   'bslma::Allocator' for 'const int', and substituting
        //:   'bslma::TestAllocator' for 'int', as this preserves the sane
        //:   rules for convertability of pointers.  Strike any duplicate tests
        //:   for constructing a 'bsl::shared_ptr<void>' (or the equivalent
        //:   weak pointer).
        //
        //
        // Testing:
        //  CONCERN: 'shared_ptr<bslma::Allocator>' behaves correctly
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING SFINAE CONCERNS"
                            "\n=======================\n");

        // Test with pointers that are an exact match, a valid conversion, and
        // a failing conversion.  Deleters may be function-pointers or
        // functors.  Allocators may be standard allocators,
        // 'bslma::Allocator *', or convertible to 'bslma::Allocator *'.  There
        // should be an overload taking a (convertible to) 'bslma::Allocator *'
        // for each constructor that might create a new rep object.  There are
        // a limited number of constructors that also support a std allocator.

#if defined(BSLSTL_SHAREDPTR_SUPPORTS_SFINAE_CHECKS)
        using std::is_constructible;

        // TBD: TESTS STILL MISSING
        //  SharedPtrRep * constructors
        //  class-type convertible to pointer (to catch over-eager deductions)
        //
        //  actual runtime checks with '0' as null literal, to see if any issue
        //      deducing as 'int' or a SFINAE-tag.


        // Basic default-constructible tests: breathing test for types that
        // follow.

        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::TestAllocator>
                                >::value));

        // Copying constructors

        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::shared_ptr<bslma::Allocator>
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::shared_ptr<bslma::TestAllocator>
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::shared_ptr<void>
                                >::value));

        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::TestAllocator>
                                , bsl::shared_ptr<bslma::Allocator>
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::TestAllocator>
                                , bsl::shared_ptr<bslma::TestAllocator>
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::TestAllocator>
                                , bsl::shared_ptr<void>
                                >::value));


        // Moving constructors

        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslmf::MovableRef<
                                             bsl::shared_ptr<bslma::Allocator>>
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslmf::MovableRef<
                                         bsl::shared_ptr<bslma::TestAllocator>>
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslmf::MovableRef<bsl::shared_ptr<void>>
                                >::value));

        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::TestAllocator>
                                , bslmf::MovableRef<
                                             bsl::shared_ptr<bslma::Allocator>>
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::TestAllocator>
                                , bslmf::MovableRef<
                                         bsl::shared_ptr<bslma::TestAllocator>>
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::TestAllocator>
                                , bslmf::MovableRef<bsl::shared_ptr<void>>
                                >::value));


        // Aliasing constructors

        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::shared_ptr<bslma::Allocator>
                                , bsl::nullptr_t
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::shared_ptr<bslma::TestAllocator>
                                , bsl::nullptr_t
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::shared_ptr<void>
                                , bsl::nullptr_t
                                >::value));

        ASSERT( (is_constructible<bsl::shared_ptr<bslma::TestAllocator>
                                , bsl::shared_ptr<bslma::Allocator>
                                , bsl::nullptr_t
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::TestAllocator>
                                , bsl::shared_ptr<bslma::TestAllocator>
                                , bsl::nullptr_t
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::TestAllocator>
                                , bsl::shared_ptr<void>
                                , bsl::nullptr_t
                                >::value));

        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::shared_ptr<bslma::Allocator>
                                , bslma::Allocator *
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::shared_ptr<bslma::TestAllocator>
                                , bslma::Allocator *
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::shared_ptr<void>
                                , bslma::Allocator *
                                >::value));

        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::TestAllocator>
                                , bsl::shared_ptr<bslma::Allocator>
                                , bslma::Allocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::TestAllocator>
                                , bsl::shared_ptr<bslma::TestAllocator>
                                , bslma::Allocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::TestAllocator>
                                , bsl::shared_ptr<void>
                                , bslma::Allocator *
                                >::value));

        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::shared_ptr<bslma::Allocator>
                                , bslma::TestAllocator *
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::shared_ptr<bslma::TestAllocator>
                                , bslma::TestAllocator *
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::shared_ptr<void>
                                , bslma::TestAllocator *
                                >::value));

        ASSERT( (is_constructible<bsl::shared_ptr<bslma::TestAllocator>
                                , bsl::shared_ptr<bslma::Allocator>
                                , bslma::TestAllocator *
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::TestAllocator>
                                , bsl::shared_ptr<bslma::TestAllocator>
                                , bslma::TestAllocator *
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::TestAllocator>
                                , bsl::shared_ptr<     void>
                                , bslma::TestAllocator *
                                >::value));

        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::shared_ptr<bslma::Allocator>
                                , void *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::shared_ptr<bslma::TestAllocator>
                                , void *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::shared_ptr<     void>
                                , void *
                                >::value));

        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::TestAllocator>
                                , bsl::shared_ptr<bslma::Allocator>
                                , void *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::TestAllocator>
                                , bsl::shared_ptr<bslma::TestAllocator>
                                , void *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::TestAllocator>
                                , bsl::shared_ptr<     void>
                                , void *
                                >::value));

        ASSERT( (is_constructible<bsl::shared_ptr<bslma::TestAllocator>
                                , bsl::nullptr_t
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::TestAllocator>
                                , bslma::TestAllocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::TestAllocator>
                                , bslma::Allocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::TestAllocator>
                                , void *
                                >::value));
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::TestAllocator>
                                , std::auto_ptr<bslma::TestAllocator>
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::TestAllocator>
                                , std::auto_ptr<bslma::Allocator>
                                >::value));
        // Note that this converts through auto_ptr_ref, and cannot be blocked.
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic pop
# endif
#endif
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR)
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::TestAllocator>
                                , std::unique_ptr<bslma::TestAllocator>
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::TestAllocator>
                                , std::unique_ptr<bslma::Allocator>
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::TestAllocator>
                                , std::unique_ptr<int,
                                                  support::TypedDeleter<void> >
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::TestAllocator>
                                , std::unique_ptr<bslma::Allocator,
                                                  support::DoNotDeleter >
                                >::value));
#endif
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::TestAllocator>
                                , bsl::weak_ptr<bslma::TestAllocator>
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::TestAllocator>
                                , bsl::weak_ptr<bslma::Allocator>
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::TestAllocator>
                                , bsl::weak_ptr<void>
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::TestAllocator>
                                , bslma::ManagedPtr<bslma::TestAllocator>
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::TestAllocator>
                                , bslma::ManagedPtr<bslma::Allocator>
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::TestAllocator>
                                , bslma::ManagedPtr<void>
                                >::value));

        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::nullptr_t
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::TestAllocator *
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::Allocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , void *
                                >::value));
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::auto_ptr<bslma::TestAllocator>
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::auto_ptr<bslma::Allocator>
                                >::value));
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic pop
# endif
#endif
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR)
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::unique_ptr<bslma::TestAllocator>
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::unique_ptr<bslma::Allocator>
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::unique_ptr<bslma::TestAllocator,
                                                  support::TypedDeleter<void> >
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::unique_ptr<bslma::Allocator,
                                                  support::DoNotDeleter >
                                >::value));
#endif
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<bslma::TestAllocator>
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<bslma::Allocator>
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<void>
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<bslma::TestAllocator>
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<bslma::Allocator>
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<void>
                                >::value));


        // 2 arguments: [target, bslma-factory deleter]
        // Note that sometimes the factory-deleter is really an allocator.

        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::nullptr_t
                                , bslma::TestAllocator *
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::TestAllocator *
                                , bslma::TestAllocator *
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::Allocator *
                                , bslma::TestAllocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , void *
                                , bslma::TestAllocator *
                                >::value));
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::auto_ptr<bslma::TestAllocator>
                                , bslma::TestAllocator *
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::auto_ptr<bslma::Allocator>
                                , bslma::TestAllocator *
                                >::value));
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic pop
# endif
#endif
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR)
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::unique_ptr<bslma::TestAllocator>
                                , bslma::TestAllocator *
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::unique_ptr<bslma::Allocator>
                                , bslma::TestAllocator *
                                >::value));
#endif
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<bslma::TestAllocator>
                                , bslma::TestAllocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<bslma::Allocator>
                                , bslma::TestAllocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<void>
                                , bslma::TestAllocator *
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<bslma::TestAllocator>
                                , bslma::TestAllocator *
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<bslma::Allocator>
                                , bslma::TestAllocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<void>
                                , bslma::TestAllocator *
                                >::value));


        // 2 arguments: [target, factory deleter object]

        ASSERT( (is_constructible<bsl::shared_ptr<bslma::TestAllocator>
                                , bsl::nullptr_t
                                , support::TypedFactory<bslma::TestAllocator> *
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::nullptr_t
                                , support::TypedFactory<bslma::TestAllocator> *
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::TestAllocator *
                                , support::TypedFactory<bslma::TestAllocator> *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::Allocator *
                                , support::TypedFactory<bslma::TestAllocator> *
                                >::value));

        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::nullptr_t
                                , support::FactoryDeleter *
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::TestAllocator *
                                , support::FactoryDeleter *
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::Allocator *
                                , support::FactoryDeleter *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , void *
                                , support::FactoryDeleter *
                                >::value));
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::auto_ptr<bslma::TestAllocator>
                                , support::FactoryDeleter *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::auto_ptr<bslma::Allocator>
                                , support::FactoryDeleter *
                                >::value));
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic pop
# endif
#endif
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR)
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::unique_ptr<bslma::TestAllocator>
                                , support::FactoryDeleter *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::unique_ptr<bslma::Allocator>
                                , support::FactoryDeleter *
                                >::value));
#endif
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<bslma::TestAllocator>
                                , support::FactoryDeleter *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<bslma::Allocator>
                                , support::FactoryDeleter *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<void>
                                , support::FactoryDeleter *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<bslma::TestAllocator>
                                , support::FactoryDeleter *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<bslma::Allocator>
                                , support::FactoryDeleter *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<void>
                                , support::FactoryDeleter *
                                >::value));


        // 2 arguments: [target, functor-pointer deleter]

        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::nullptr_t
                                , bslstl::SharedPtrNilDeleter
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::TestAllocator *
                                , bslstl::SharedPtrNilDeleter
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::Allocator *
                                , bslstl::SharedPtrNilDeleter
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , void *
                                , bslstl::SharedPtrNilDeleter
                                >::value));
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::auto_ptr<bslma::TestAllocator>
                                , bslstl::SharedPtrNilDeleter
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::auto_ptr<bslma::Allocator>
                                , bslstl::SharedPtrNilDeleter
                                >::value));
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic pop
# endif
#endif
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR)
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::unique_ptr<bslma::TestAllocator>
                                , bslstl::SharedPtrNilDeleter
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::unique_ptr<bslma::Allocator>
                                , bslstl::SharedPtrNilDeleter
                                >::value));
#endif
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<bslma::TestAllocator>
                                , bslstl::SharedPtrNilDeleter
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<bslma::Allocator>
                                , bslstl::SharedPtrNilDeleter
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<void>
                                , bslstl::SharedPtrNilDeleter
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<bslma::TestAllocator>
                                , bslstl::SharedPtrNilDeleter
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<bslma::Allocator>
                                , bslstl::SharedPtrNilDeleter
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<void>
                                , bslstl::SharedPtrNilDeleter
                                >::value));


        // 2 arguments: [target, function-pointer deleter]

        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::nullptr_t
                                , void(*)(bslma::Allocator *)
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::TestAllocator *
                                , void(*)(bslma::Allocator *)
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::Allocator *
                                , void(*)(bslma::Allocator *)
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , void *
                                , void(*)(void *)
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::nullptr_t
                                , void(*)(bslma::TestAllocator *)
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::TestAllocator *
                                , void(*)(bslma::TestAllocator *)
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::Allocator *
                                , void(*)(bslma::TestAllocator *)
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , void *
                                , void(*)(bslma::Allocator *)
                                >::value));
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::auto_ptr<bslma::TestAllocator>
                                , void(*)(bslma::Allocator *)
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::auto_ptr<bslma::Allocator>
                                , void(*)(bslma::Allocator *)
                                >::value));
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic pop
# endif
#endif
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR)
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::unique_ptr<bslma::TestAllocator>
                                , void(*)(bslma::Allocator *)
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::unique_ptr<bslma::Allocator>
                                , void(*)(bslma::Allocator *)
                                >::value));
#endif
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<bslma::TestAllocator>
                                , void(*)(bslma::Allocator *)
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<bslma::Allocator>
                                , void(*)(bslma::Allocator *)
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<void>
                                , void(*)(bslma::Allocator *)
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<bslma::TestAllocator>
                                , void(*)(bslma::Allocator *)
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<bslma::Allocator>
                                , void(*)(bslma::Allocator *)
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<void>
                                , void(*)(bslma::Allocator *)
                                >::value));



        // 3 arguments: [target, bslma-factory, bslma allocator]

        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::nullptr_t
                                , bslma::Allocator *
                                , bslma::Allocator *
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::TestAllocator *
                                , bslma::Allocator *
                                , bslma::Allocator *
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::Allocator *
                                , bslma::Allocator *
                                , bslma::Allocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , void *
                                , bslma::Allocator *
                                , bslma::Allocator *
                                >::value));
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::auto_ptr<bslma::TestAllocator>
                                , bslma::Allocator *
                                , bslma::Allocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::auto_ptr<bslma::Allocator>
                                , bslma::Allocator *
                                , bslma::Allocator *
                                >::value));
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic pop
# endif
#endif
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR)
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::unique_ptr<bslma::TestAllocator>
                                , bslma::Allocator *
                                , bslma::Allocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::unique_ptr<bslma::Allocator>
                                , bslma::Allocator *
                                , bslma::Allocator *
                                >::value));
#endif
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<bslma::TestAllocator>
                                , bslma::Allocator *
                                , bslma::Allocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<bslma::Allocator>
                                , bslma::Allocator *
                                , bslma::Allocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<void>
                                , bslma::Allocator *
                                , bslma::Allocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<bslma::TestAllocator>
                                , bslma::Allocator *
                                , bslma::Allocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<bslma::Allocator>
                                , bslma::Allocator *
                                , bslma::Allocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<void>
                                , bslma::Allocator *
                                , bslma::Allocator *
                                >::value));

        // 3 arguments: [target, functor-deleter, bslma allocator]

        ASSERT( (is_constructible<bsl::shared_ptr<bslma::TestAllocator>
                                , bsl::nullptr_t
                                , support::TypedFactory<bslma::TestAllocator> *
                                , bslma::Allocator *
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::nullptr_t
                                , support::TypedFactory<bslma::TestAllocator> *
                                , bslma::Allocator *
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::TestAllocator *
                                , support::TypedFactory<bslma::TestAllocator> *
                                , bslma::Allocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::Allocator *
                                , support::TypedFactory<bslma::TestAllocator> *
                                , bslma::Allocator *
                                >::value));

        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::nullptr_t
                                , bslstl::SharedPtrNilDeleter
                                , bslma::Allocator *
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::TestAllocator *
                                , bslstl::SharedPtrNilDeleter
                                , bslma::Allocator *
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::Allocator *
                                , bslstl::SharedPtrNilDeleter
                                , bslma::Allocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , void *
                                , bslstl::SharedPtrNilDeleter
                                , bslma::Allocator *
                                >::value));
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::auto_ptr<bslma::TestAllocator>
                                , bslstl::SharedPtrNilDeleter
                                , bslma::Allocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::auto_ptr<bslma::Allocator>
                                , bslstl::SharedPtrNilDeleter
                                , bslma::Allocator *
                                >::value));
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic pop
# endif
#endif
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR)
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::unique_ptr<bslma::TestAllocator>
                                , bslstl::SharedPtrNilDeleter
                                , bslma::Allocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::unique_ptr<bslma::Allocator>
                                , bslstl::SharedPtrNilDeleter
                                , bslma::Allocator *
                                >::value));
#endif
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<bslma::TestAllocator>
                                , bslstl::SharedPtrNilDeleter
                                , bslma::Allocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<bslma::Allocator>
                                , bslstl::SharedPtrNilDeleter
                                , bslma::Allocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<void>
                                , bslstl::SharedPtrNilDeleter
                                , bslma::Allocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<bslma::TestAllocator>
                                , bslstl::SharedPtrNilDeleter
                                , bslma::Allocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<bslma::Allocator>
                                , bslstl::SharedPtrNilDeleter
                                , bslma::Allocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<void>
                                , bslstl::SharedPtrNilDeleter
                                , bslma::Allocator *
                                >::value));


        // 3 arguments: [target, function-pointer, bslma allocator]

        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::nullptr_t
                                , void(*)(bslma::TestAllocator *)
                                , bslma::Allocator *
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::TestAllocator *
                                , void(*)(bslma::TestAllocator *)
                                , bslma::Allocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::Allocator *
                                , void(*)(bslma::TestAllocator *)
                                , bslma::Allocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , void *
                                , void(*)(void *)
                                , bslma::Allocator *
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::nullptr_t
                                , void(*)(bslma::Allocator *)
                                , bslma::Allocator *
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::TestAllocator *
                                , void(*)(bslma::Allocator *)
                                , bslma::Allocator *
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::Allocator *
                                , void(*)(bslma::Allocator *)
                                , bslma::Allocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , void *
                                , void(*)(bslma::Allocator *)
                                , bslma::Allocator *
                                >::value));
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::auto_ptr<bslma::TestAllocator>
                                , void(*)(bslma::Allocator *)
                                , bslma::Allocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::auto_ptr<bslma::Allocator>
                                , void(*)(bslma::TestAllocator *)
                                , bslma::Allocator *
                                >::value));
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic pop
# endif
#endif
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR)
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::unique_ptr<bslma::TestAllocator>
                                , void(*)(bslma::Allocator *)
                                , bslma::Allocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::unique_ptr<bslma::Allocator>
                                , void(*)(bslma::Allocator *)
                                , bslma::Allocator *
                                >::value));
#endif
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<bslma::TestAllocator>
                                , void(*)(bslma::Allocator *)
                                , bslma::Allocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<bslma::Allocator>
                                , void(*)(bslma::Allocator *)
                                , bslma::Allocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<void>
                                , void(*)(bslma::Allocator *)
                                , bslma::Allocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<bslma::TestAllocator>
                                , void(*)(bslma::Allocator *)
                                , bslma::Allocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<bslma::Allocator>
                                , void(*)(bslma::Allocator *)
                                , bslma::Allocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<void>
                                , void(*)(bslma::Allocator *)
                                , bslma::Allocator *
                                >::value));

        // 3 arguments: [target, bslma factory, derived-bslma allocator]

        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::nullptr_t
                                , bslma::Allocator *
                                , bslma::TestAllocator *
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::TestAllocator *
                                , bslma::Allocator *
                                , bslma::TestAllocator *
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::Allocator *
                                , bslma::Allocator *
                                , bslma::TestAllocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , void *
                                , bslma::Allocator *
                                , bslma::TestAllocator *
                                >::value));
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::auto_ptr<bslma::TestAllocator>
                                , bslma::Allocator *
                                , bslma::TestAllocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::auto_ptr<bslma::Allocator>
                                , bslma::Allocator *
                                , bslma::TestAllocator *
                                >::value));
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic pop
# endif
#endif
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR)
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::unique_ptr<bslma::TestAllocator>
                                , bslma::Allocator *
                                , bslma::TestAllocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::unique_ptr<bslma::Allocator>
                                , bslma::Allocator *
                                , bslma::TestAllocator *
                                >::value));
#endif
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<bslma::TestAllocator>
                                , bslma::Allocator *
                                , bslma::TestAllocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<bslma::Allocator>
                                , bslma::Allocator *
                                , bslma::TestAllocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<void>
                                , bslma::Allocator *
                                , bslma::TestAllocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<bslma::TestAllocator>
                                , bslma::Allocator *
                                , bslma::TestAllocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<bslma::Allocator>
                                , bslma::Allocator *
                                , bslma::TestAllocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<void>
                                , bslma::Allocator *
                                , bslma::TestAllocator *
                                >::value));


        // 3 arguments: [target, functor-deleter, derived-bslma allocator]

        ASSERT( (is_constructible<bsl::shared_ptr<bslma::TestAllocator>
                                , bsl::nullptr_t
                                , support::TypedFactory<bslma::TestAllocator> *
                                , bslma::TestAllocator *
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::nullptr_t
                                , support::TypedFactory<bslma::TestAllocator> *
                                , bslma::TestAllocator *
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::TestAllocator *
                                , support::TypedFactory<bslma::TestAllocator> *
                                , bslma::TestAllocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::Allocator *
                                , support::TypedFactory<bslma::TestAllocator> *
                                , bslma::TestAllocator *
                                >::value));

        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::nullptr_t
                                , bslstl::SharedPtrNilDeleter
                                , bslma::TestAllocator *
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::TestAllocator *
                                , bslstl::SharedPtrNilDeleter
                                , bslma::TestAllocator *
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::Allocator *
                                , bslstl::SharedPtrNilDeleter
                                , bslma::TestAllocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , void *
                                , bslstl::SharedPtrNilDeleter
                                , bslma::TestAllocator *
                                >::value));
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::auto_ptr<bslma::TestAllocator>
                                , bslstl::SharedPtrNilDeleter
                                , bslma::TestAllocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::auto_ptr<bslma::Allocator>
                                , bslstl::SharedPtrNilDeleter
                                , bslma::TestAllocator *
                                >::value));
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic pop
# endif
#endif
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR)
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::unique_ptr<bslma::TestAllocator>
                                , bslstl::SharedPtrNilDeleter
                                , bslma::TestAllocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::unique_ptr<bslma::Allocator>
                                , bslstl::SharedPtrNilDeleter
                                , bslma::TestAllocator *
                                >::value));
#endif
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<bslma::TestAllocator>
                                , bslstl::SharedPtrNilDeleter
                                , bslma::TestAllocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<bslma::Allocator>
                                , bslstl::SharedPtrNilDeleter
                                , bslma::TestAllocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<void>
                                , bslstl::SharedPtrNilDeleter
                                , bslma::TestAllocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<bslma::TestAllocator>
                                , bslstl::SharedPtrNilDeleter
                                , bslma::TestAllocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<bslma::Allocator>
                                , bslstl::SharedPtrNilDeleter
                                , bslma::TestAllocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<void>
                                , bslstl::SharedPtrNilDeleter
                                , bslma::TestAllocator *
                                >::value));


        // 3 arguments: [target, function-deleter, derived-bslma allocator]

        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::nullptr_t
                                , void(*)(bslma::TestAllocator *)
                                , bslma::TestAllocator *
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::TestAllocator *
                                , void(*)(bslma::TestAllocator *)
                                , bslma::TestAllocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::Allocator *
                                , void(*)(bslma::TestAllocator *)
                                , bslma::TestAllocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , void *
                                , void(*)(void *)
                                , bslma::TestAllocator *
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::nullptr_t
                                , void(*)(bslma::Allocator *)
                                , bslma::TestAllocator *
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::TestAllocator *
                                , void(*)(bslma::Allocator *)
                                , bslma::TestAllocator *
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::Allocator *
                                , void(*)(bslma::Allocator *)
                                , bslma::TestAllocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , void *
                                , void(*)(bslma::Allocator *)
                                , bslma::TestAllocator *
                                >::value));
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::auto_ptr<bslma::TestAllocator>
                                , void(*)(bslma::TestAllocator *)
                                , bslma::TestAllocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::auto_ptr<bslma::Allocator>
                                , void(*)(bslma::Allocator *)
                                , bslma::TestAllocator *
                                >::value));
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic pop
# endif
#endif
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR)
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::unique_ptr<bslma::TestAllocator>
                                , void(*)(bslma::Allocator *)
                                , bslma::TestAllocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::unique_ptr<bslma::Allocator>
                                , void(*)(bslma::Allocator *)
                                , bslma::TestAllocator *
                                >::value));
#endif
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<bslma::TestAllocator>
                                , void(*)(bslma::Allocator *)
                                , bslma::TestAllocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<bslma::Allocator>
                                , void(*)(bslma::Allocator *)
                                , bslma::TestAllocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<void>
                                , void(*)(bslma::Allocator *)
                                , bslma::TestAllocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<bslma::TestAllocator>
                                , void(*)(bslma::Allocator *)
                                , bslma::TestAllocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<bslma::Allocator>
                                , void(*)(bslma::Allocator *)
                                , bslma::TestAllocator *
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<void>
                                , void(*)(bslma::Allocator *)
                                , bslma::TestAllocator *
                                >::value));


        // 3 arguments: [target, bslma-factory, std allocator]

        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::nullptr_t
                                , bslma::Allocator *
                                , bsl::allocator<char>
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::TestAllocator *
                                , bslma::Allocator *
                                , bsl::allocator<char>
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::Allocator *
                                , bslma::Allocator *
                                , bsl::allocator<char>
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , void *
                                , bslma::Allocator *
                                , bsl::allocator<char>
                                >::value));
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::auto_ptr<bslma::TestAllocator>
                                , bslma::Allocator *
                                , bsl::allocator<char>
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::auto_ptr<bslma::Allocator>
                                , bslma::Allocator *
                                , bsl::allocator<char>
                                >::value));
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic pop
# endif
#endif
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR)
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::unique_ptr<bslma::TestAllocator>
                                , bslma::Allocator *
                                , bsl::allocator<char>
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::unique_ptr<bslma::Allocator>
                                , bslma::Allocator *
                                , bsl::allocator<char>
                                >::value));
#endif
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<bslma::TestAllocator>
                                , bslma::Allocator *
                                , bsl::allocator<char>
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<bslma::Allocator>
                                , bslma::Allocator *
                                , bsl::allocator<char>
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<void>
                                , bslma::Allocator *
                                , bsl::allocator<char>
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<bslma::TestAllocator>
                                , bslma::Allocator *
                                , bsl::allocator<char>
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<bslma::Allocator>
                                , bslma::Allocator *
                                , bsl::allocator<char>
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<void>
                                , bslma::Allocator *
                                , bsl::allocator<char>
                                >::value));


        // 3 arguments: [target, functor-deleter, std allocator]

        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::nullptr_t
                                , bslstl::SharedPtrNilDeleter
                                , bsl::allocator<char>
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::TestAllocator *
                                , bslstl::SharedPtrNilDeleter
                                , bsl::allocator<char>
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::Allocator *
                                , bslstl::SharedPtrNilDeleter
                                , bsl::allocator<char>
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , void *
                                , bslstl::SharedPtrNilDeleter
                                , bsl::allocator<char>
                                >::value));
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::auto_ptr<bslma::TestAllocator>
                                , bslstl::SharedPtrNilDeleter
                                , bsl::allocator<char>
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::auto_ptr<bslma::Allocator>
                                , bslstl::SharedPtrNilDeleter
                                , bsl::allocator<char>
                                >::value));
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic pop
# endif
#endif
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR)
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::unique_ptr<bslma::TestAllocator>
                                , bslstl::SharedPtrNilDeleter
                                , bsl::allocator<char>
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::unique_ptr<bslma::Allocator>
                                , bslstl::SharedPtrNilDeleter
                                , bsl::allocator<char>
                                >::value));
#endif
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<bslma::TestAllocator>
                                , bslstl::SharedPtrNilDeleter
                                , bsl::allocator<char>
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<bslma::Allocator>
                                , bslstl::SharedPtrNilDeleter
                                , bsl::allocator<char>
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<void>
                                , bslstl::SharedPtrNilDeleter
                                , bsl::allocator<char>
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<bslma::TestAllocator>
                                , bslstl::SharedPtrNilDeleter
                                , bsl::allocator<char>
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<bslma::Allocator>
                                , bslstl::SharedPtrNilDeleter
                                , bsl::allocator<char>
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<void>
                                , bslstl::SharedPtrNilDeleter
                                , bsl::allocator<char>
                                >::value));


        // 3 arguments: [target, function-deleter, std allocator]

        ASSERT( (is_constructible<bsl::shared_ptr<bslma::TestAllocator>
                                , bsl::nullptr_t
                                , support::TypedFactory<bslma::TestAllocator> *
                                , bsl::allocator<char>
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::nullptr_t
                                , support::TypedFactory<bslma::TestAllocator> *
                                , bsl::allocator<char>
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::TestAllocator *
                                , support::TypedFactory<bslma::TestAllocator> *
                                , bsl::allocator<char>
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::Allocator *
                                , support::TypedFactory<bslma::TestAllocator> *
                                , bsl::allocator<char>
                                >::value));

        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::nullptr_t
                                , void(*)(bslma::TestAllocator *)
                                , bsl::allocator<char>
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::TestAllocator *
                                , void(*)(bslma::TestAllocator *)
                                , bsl::allocator<char>
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::Allocator *
                                , void(*)(bslma::TestAllocator *)
                                , bsl::allocator<char>
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , void *
                                , void(*)(void *)
                                , bsl::allocator<char>
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::nullptr_t
                                , void(*)(bslma::Allocator *)
                                , bsl::allocator<char>
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::TestAllocator *
                                , void(*)(bslma::Allocator *)
                                , bsl::allocator<char>
                                >::value));
        ASSERT( (is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::Allocator *
                                , void(*)(bslma::Allocator *)
                                , bsl::allocator<char>
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , void *
                                , void(*)(bslma::Allocator *)
                                , bsl::allocator<char>
                                >::value));
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::auto_ptr<bslma::TestAllocator>
                                , void(*)(bslma::Allocator *)
                                , bsl::allocator<char>
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::auto_ptr<bslma::Allocator>
                                , void(*)(bslma::Allocator *)
                                , bsl::allocator<char>
                                >::value));
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic pop
# endif
#endif
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR)
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::unique_ptr<bslma::TestAllocator>
                                , void(*)(bslma::Allocator *)
                                , bsl::allocator<char>
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , std::unique_ptr<bslma::Allocator>
                                , void(*)(bslma::Allocator *)
                                , bsl::allocator<char>
                                >::value));
#endif
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<bslma::TestAllocator>
                                , void(*)(bslma::Allocator *)
                                , bsl::allocator<char>
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<bslma::Allocator>
                                , void(*)(bslma::Allocator *)
                                , bsl::allocator<char>
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bsl::weak_ptr<void>
                                , void(*)(bslma::Allocator *)
                                , bsl::allocator<char>
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<bslma::TestAllocator>
                                , void(*)(bslma::Allocator *)
                                , bsl::allocator<char>
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<bslma::Allocator>
                                , void(*)(bslma::Allocator *)
                                , bsl::allocator<char>
                                >::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<bslma::Allocator>
                                , bslma::ManagedPtr<void>
                                , void(*)(bslma::Allocator *)
                                , bsl::allocator<char>
                                >::value));


        // WEAK POINTER CONSTRUCTORS


        // Basic default-constructible tests: breathing test for types that
        // follow.

        ASSERT( (is_constructible<bsl::weak_ptr<bslma::Allocator>
                                >::value));
        ASSERT( (is_constructible<bsl::weak_ptr<bslma::TestAllocator>
                                >::value));
        ASSERT( (is_constructible<bsl::weak_ptr<void>
                                >::value));

        // Copying constructors

        ASSERT( (is_constructible<bsl::weak_ptr<bslma::Allocator>
                                , bsl::weak_ptr<bslma::Allocator>
                                >::value));
        ASSERT( (is_constructible<bsl::weak_ptr<bslma::Allocator>
                                , bsl::weak_ptr<bslma::TestAllocator>
                                >::value));
        ASSERT(!(is_constructible<bsl::weak_ptr<bslma::Allocator>
                                , bsl::weak_ptr<void>
                                >::value));

        ASSERT(!(is_constructible<bsl::weak_ptr<bslma::TestAllocator>
                                , bsl::weak_ptr<bslma::Allocator>
                                >::value));
        ASSERT( (is_constructible<bsl::weak_ptr<bslma::TestAllocator>
                                , bsl::weak_ptr<bslma::TestAllocator>
                                >::value));
        ASSERT(!(is_constructible<bsl::weak_ptr<bslma::TestAllocator>
                                , bsl::weak_ptr<void>
                                >::value));

        ASSERT( (is_constructible<bsl::weak_ptr<bslma::Allocator>
                                , bsl::shared_ptr<bslma::Allocator>
                                >::value));
        ASSERT( (is_constructible<bsl::weak_ptr<bslma::Allocator>
                                , bsl::shared_ptr<bslma::TestAllocator>
                                >::value));
        ASSERT(!(is_constructible<bsl::weak_ptr<bslma::Allocator>
                                , bsl::shared_ptr<void>
                                >::value));

        ASSERT(!(is_constructible<bsl::weak_ptr<bslma::TestAllocator>
                                , bsl::shared_ptr<bslma::Allocator>
                                >::value));
        ASSERT( (is_constructible<bsl::weak_ptr<bslma::TestAllocator>
                                , bsl::shared_ptr<bslma::TestAllocator>
                                >::value));
        ASSERT(!(is_constructible<bsl::weak_ptr<bslma::TestAllocator>
                                , bsl::shared_ptr<void>
                                >::value));

        // Moving constructors

        ASSERT( (is_constructible<bsl::weak_ptr<bslma::Allocator>
                                , bslmf::MovableRef<
                                               bsl::weak_ptr<bslma::Allocator>>
                                >::value));
        ASSERT( (is_constructible<bsl::weak_ptr<bslma::Allocator>
                                , bslmf::MovableRef<
                                           bsl::weak_ptr<bslma::TestAllocator>>
                                >::value));
        ASSERT(!(is_constructible<bsl::weak_ptr<bslma::Allocator>
                                , bslmf::MovableRef<bsl::weak_ptr<void>>
                                >::value));

        ASSERT(!(is_constructible<bsl::weak_ptr<bslma::TestAllocator>
                                , bslmf::MovableRef<
                                               bsl::weak_ptr<bslma::Allocator>>
                                >::value));
        ASSERT( (is_constructible<bsl::weak_ptr<bslma::TestAllocator>
                                , bslmf::MovableRef<
                                           bsl::weak_ptr<bslma::TestAllocator>>
                                >::value));
        ASSERT(!(is_constructible<bsl::weak_ptr<bslma::TestAllocator>
                                , bslmf::MovableRef<bsl::weak_ptr<void>>
                                >::value));

        ASSERT( (is_constructible<bsl::weak_ptr<bslma::Allocator>
                                , bslmf::MovableRef<
                                             bsl::shared_ptr<bslma::Allocator>>
                                >::value));
        ASSERT( (is_constructible<bsl::weak_ptr<bslma::Allocator>
                                , bslmf::MovableRef<
                                         bsl::shared_ptr<bslma::TestAllocator>>
                                >::value));
        ASSERT(!(is_constructible<bsl::weak_ptr<bslma::Allocator>
                                , bslmf::MovableRef<bsl::shared_ptr<void>>
                                >::value));

        ASSERT(!(is_constructible<bsl::weak_ptr<bslma::TestAllocator>
                                , bslmf::MovableRef<
                                             bsl::shared_ptr<bslma::Allocator>>
                                >::value));
        ASSERT( (is_constructible<bsl::weak_ptr<bslma::TestAllocator>
                                , bslmf::MovableRef<
                                         bsl::shared_ptr<bslma::TestAllocator>>
                                >::value));
        ASSERT(!(is_constructible<bsl::weak_ptr<bslma::TestAllocator>
                                , bslmf::MovableRef<bsl::shared_ptr<void>>
                                >::value));
#endif
      } break;
      case 36: {
        // --------------------------------------------------------------------
        // TESTING SFINAE CONCERNS
        //   Confirm that the operations mandated by the standard to not be
        //   available to overload resolution fail as expected according to the
        //   SFINAE rules.  This is most evident when testing using the C++11
        //   type trait 'is_constructible'.
        //
        // Concerns:
        //: 1 All overloads can be called with pointer types that satisfy the
        //:   convertability constraints specified in the standard, without
        //:   introducing ambiguities in the overload set.
        //: 2 An overload that would rely on an incompatible conversion fails
        //:   due to not finding an appropriate overload, rather than matching
        //:   and causing a 'hard' failing to instantiate the template.
        //: 3 The constructors taking 'unique_ptr' properly account for fancy
        //:   pointers indicated by the 'unique_ptr' deleter policy.
        //
        // Plan:
        //
        // Testing:
        //   'shared_ptr' constructors SFINAE on compatible pointers
        //   'shared_ptr' = operators SFINAE on compatible pointers
        //   'weak_ptr' constructors SFINAE on compatible pointers
        //   'weak_ptr' = operators SFINAE on compatible pointers
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING SFINAE CONCERNS"
                            "\n=======================\n");

        // Test with pointers that are an exact match, a valid conversion, and
        // a failing conversion.  Deleters may be function-pointers or
        // functors.  Allocators may be standard allocators,
        // 'bslma::Allocator *', or convertible to 'bslma::Allocator *'.  There
        // should be an overload taking a (convertible to) 'bslma::Allocator *'
        // for each constructor that might create a new rep object.  There are
        // a limited number of constructors that also support a std allocator.

#if defined(BSLSTL_SHAREDPTR_SUPPORTS_SFINAE_CHECKS)
        using std::is_constructible;

        // TBD: TESTS STILL MISSING
        //  SharedPtrRep * constructors
        //  class-type convertible to pointer (to catch over-eager deductions)
        //
        //  actual runtime checks with '0' as null literal, to see if any issue
        //      deducing as 'int' and a SFINAE-tag.

        // First, some quick checks of the component-specific traits classes.

        ASSERT( (bslstl::SharedPtr_IsCallable<void (int *),  int *>::k_VALUE));
        ASSERT(!(bslstl::SharedPtr_IsCallable<void (int *), void *>::k_VALUE));
        ASSERT( (bslstl::SharedPtr_IsCallable<int (void *), char *>::k_VALUE));
        ASSERT( (bslstl::SharedPtr_IsCallable<int (void *),
                                                    bsl::nullptr_t>::k_VALUE));
        ASSERT( (bslstl::SharedPtr_IsCallable<int(&)(void *),
                                                            char *>::k_VALUE));
        ASSERT(!(bslstl::SharedPtr_IsCallable<int(&)(void *),
                                                      const char *>::k_VALUE));
        ASSERT( (bslstl::SharedPtr_IsCallable<int(&)(void *),
                                                    bsl::nullptr_t>::k_VALUE));
        ASSERT( (bslstl::SharedPtr_IsCallable<int(*)(void *),
                                                            char *>::k_VALUE));
        ASSERT(!(bslstl::SharedPtr_IsCallable<int(*)(void *),
                                                      const char *>::k_VALUE));
        ASSERT( (bslstl::SharedPtr_IsCallable<int(*)(void *),
                                                    bsl::nullptr_t>::k_VALUE));
        ASSERT( (bslstl::SharedPtr_IsCallable<
                               bslstl::SharedPtrNilDeleter, char *>::k_VALUE));
        ASSERT( (bslstl::SharedPtr_IsCallable<
                               bslstl::SharedPtrNilDeleter,
                                                    bsl::nullptr_t>::k_VALUE));

        ASSERT(!(bslstl::SharedPtr_IsCallable< int, int           >::k_VALUE));
        ASSERT(!(bslstl::SharedPtr_IsCallable< int, bsl::nullptr_t>::k_VALUE));
        ASSERT(!(bslstl::SharedPtr_IsCallable< int, void          >::k_VALUE));
        ASSERT(!(bslstl::SharedPtr_IsCallable<void, int           >::k_VALUE));
        ASSERT(!(bslstl::SharedPtr_IsCallable<void, bsl::nullptr_t>::k_VALUE));
        ASSERT(!(bslstl::SharedPtr_IsCallable<void, void          >::k_VALUE));


        ASSERT( (bslstl::SharedPtr_IsFactoryFor<
                              BloombergLP::bslma::Allocator *, int>::k_VALUE));
        ASSERT(!(bslstl::SharedPtr_IsFactoryFor<
                              BloombergLP::bslma::Allocator,   int>::k_VALUE));
        ASSERT(!(bslstl::SharedPtr_IsFactoryFor<int *,         int>::k_VALUE));
        ASSERT( (bslstl::SharedPtr_IsFactoryFor<
                                               BloombergLP::bslma::Allocator *,
                                               bsl::nullptr_t>::k_VALUE));
        ASSERT(!(bslstl::SharedPtr_IsFactoryFor<BloombergLP::bslma::Allocator,
                                                bsl::nullptr_t>::k_VALUE));


        // Basic default-constructible tests: breathing test for types that
        // follow.

        ASSERT( (is_constructible<bsl::shared_ptr<const int>>::value));
        ASSERT( (is_constructible<bsl::shared_ptr<      int>>::value));
        ASSERT( (is_constructible<bsl::shared_ptr<     void>>::value));

        // Copying constructors

        ASSERT( (is_constructible<bsl::shared_ptr<const int>,
                                  bsl::shared_ptr<const int>>::value));
        ASSERT( (is_constructible<bsl::shared_ptr<const int>,
                                  bsl::shared_ptr<      int>>::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<const int>,
                                  bsl::shared_ptr<     void>>::value));

        ASSERT(!(is_constructible<bsl::shared_ptr<      int>,
                                  bsl::shared_ptr<const int>>::value));
        ASSERT( (is_constructible<bsl::shared_ptr<      int>,
                                  bsl::shared_ptr<      int>>::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<      int>,
                                  bsl::shared_ptr<     void>>::value));

        ASSERT(!(is_constructible<bsl::shared_ptr<     void>,
                                  bsl::shared_ptr<const int>>::value));
        ASSERT( (is_constructible<bsl::shared_ptr<     void>,
                                  bsl::shared_ptr<      int>>::value));
        ASSERT( (is_constructible<bsl::shared_ptr<     void>,
                                  bsl::shared_ptr<     void>>::value));


        // Moving constructors

        ASSERT( (is_constructible<
                       bsl::shared_ptr<const int>,
                       bslmf::MovableRef<bsl::shared_ptr<const int>>>::value));
        ASSERT( (is_constructible<
                       bsl::shared_ptr<const int>,
                       bslmf::MovableRef<bsl::shared_ptr<      int>>>::value));
        ASSERT(!(is_constructible<
                       bsl::shared_ptr<const int>,
                       bslmf::MovableRef<bsl::shared_ptr<     void>>>::value));

        ASSERT(!(is_constructible<
                       bsl::shared_ptr<      int>,
                       bslmf::MovableRef<bsl::shared_ptr<const int>>>::value));
        ASSERT( (is_constructible<
                       bsl::shared_ptr<      int>,
                       bslmf::MovableRef<bsl::shared_ptr<      int>>>::value));
        ASSERT(!(is_constructible<
                       bsl::shared_ptr<      int>,
                       bslmf::MovableRef<bsl::shared_ptr<     void>>>::value));

        ASSERT(!(is_constructible<
                       bsl::shared_ptr<     void>,
                       bslmf::MovableRef<bsl::shared_ptr<const int>>>::value));
        ASSERT( (is_constructible<
                       bsl::shared_ptr<      void>,
                       bslmf::MovableRef<bsl::shared_ptr<      int>>>::value));
        ASSERT( (is_constructible<
                       bsl::shared_ptr<     void>,
                       bslmf::MovableRef<bsl::shared_ptr<     void>>>::value));


        // Aliasing constructors

        ASSERT( (is_constructible<bsl::shared_ptr<const int>,
                                  bsl::shared_ptr<const int>,
                                  bsl::nullptr_t>::value));
        ASSERT( (is_constructible<bsl::shared_ptr<const int>,
                                  bsl::shared_ptr<      int>,
                                  bsl::nullptr_t>::value));
        ASSERT( (is_constructible<bsl::shared_ptr<const int>,
                                  bsl::shared_ptr<     void>,
                                  bsl::nullptr_t>::value));

        ASSERT( (is_constructible<bsl::shared_ptr<      int>,
                                  bsl::shared_ptr<const int>,
                                  bsl::nullptr_t>::value));
        ASSERT( (is_constructible<bsl::shared_ptr<      int>,
                                  bsl::shared_ptr<      int>,
                                  bsl::nullptr_t>::value));
        ASSERT( (is_constructible<bsl::shared_ptr<      int>,
                                  bsl::shared_ptr<     void>,
                                  bsl::nullptr_t>::value));

        ASSERT( (is_constructible<bsl::shared_ptr<     void>,
                                  bsl::shared_ptr<const int>,
                                  bsl::nullptr_t>::value));
        ASSERT( (is_constructible<bsl::shared_ptr<     void>,
                                  bsl::shared_ptr<      int>,
                                  bsl::nullptr_t>::value));
        ASSERT( (is_constructible<bsl::shared_ptr<     void>,
                                  bsl::shared_ptr<     void>,
                                  bsl::nullptr_t>::value));


        ASSERT( (is_constructible<bsl::shared_ptr<const int>,
                                  bsl::shared_ptr<const int>,
                                  const int *>::value));
        ASSERT( (is_constructible<bsl::shared_ptr<const int>,
                                  bsl::shared_ptr<      int>,
                                  const int *>::value));
        ASSERT( (is_constructible<bsl::shared_ptr<const int>,
                                  bsl::shared_ptr<     void>,
                                  const int *>::value));

        ASSERT(!(is_constructible<bsl::shared_ptr<      int>,
                                  bsl::shared_ptr<const int>,
                                  const int *>::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<      int>,
                                  bsl::shared_ptr<      int>,
                                  const int *>::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<      int>,
                                  bsl::shared_ptr<     void>,
                                  const int *>::value));

        ASSERT(!(is_constructible<bsl::shared_ptr<     void>,
                                  bsl::shared_ptr<const int>,
                                  const int *>::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<     void>,
                                  bsl::shared_ptr<      int>,
                                  const int *>::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<     void>,
                                  bsl::shared_ptr<     void>,
                                  const int *>::value));


        ASSERT( (is_constructible<bsl::shared_ptr<const int>,
                                  bsl::shared_ptr<const int>,
                                  int *>::value));
        ASSERT( (is_constructible<bsl::shared_ptr<const int>,
                                  bsl::shared_ptr<      int>,
                                  int *>::value));
        ASSERT( (is_constructible<bsl::shared_ptr<const int>,
                                  bsl::shared_ptr<     void>,
                                  int *>::value));

        ASSERT( (is_constructible<bsl::shared_ptr<      int>,
                                  bsl::shared_ptr<const int>,
                                  int *>::value));
        ASSERT( (is_constructible<bsl::shared_ptr<      int>,
                                  bsl::shared_ptr<      int>,
                                  int *>::value));
        ASSERT( (is_constructible<bsl::shared_ptr<      int>,
                                  bsl::shared_ptr<     void>,
                                  int *>::value));

        ASSERT( (is_constructible<bsl::shared_ptr<     void>,
                                  bsl::shared_ptr<const int>,
                                  int *>::value));
        ASSERT( (is_constructible<bsl::shared_ptr<     void>,
                                  bsl::shared_ptr<      int>,
                                  int *>::value));
        ASSERT( (is_constructible<bsl::shared_ptr<     void>,
                                  bsl::shared_ptr<     void>,
                                  int *>::value));


        ASSERT(!(is_constructible<bsl::shared_ptr<const int>,
                                  bsl::shared_ptr<const int>,
                                  void *>::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<const int>,
                                  bsl::shared_ptr<      int>,
                                  void *>::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<const int>,
                                  bsl::shared_ptr<     void>,
                                  void *>::value));

        ASSERT(!(is_constructible<bsl::shared_ptr<      int>,
                                  bsl::shared_ptr<const int>,
                                  void *>::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<      int>,
                                  bsl::shared_ptr<      int>,
                                  void *>::value));
        ASSERT(!(is_constructible<bsl::shared_ptr<      int>,
                                  bsl::shared_ptr<     void>,
                                  void *>::value));

        ASSERT( (is_constructible<bsl::shared_ptr<     void>,
                                  bsl::shared_ptr<const int>,
                                  void *>::value));
        ASSERT( (is_constructible<bsl::shared_ptr<     void>,
                                  bsl::shared_ptr<      int>,
                                  void *>::value));
        ASSERT( (is_constructible<bsl::shared_ptr<     void>,
                                  bsl::shared_ptr<     void>,
                                  void *>::value));



        ASSERT( (is_constructible<
                             bsl::shared_ptr<      int>,
                                          bsl::nullptr_t>::value));
        ASSERT( (is_constructible<
                             bsl::shared_ptr<      int>,
                                                   int *>::value));
        ASSERT(!(is_constructible<
                             bsl::shared_ptr<      int>,
                                             const int *>::value));
        ASSERT(!(is_constructible<
                             bsl::shared_ptr<      int>,
                                                  void *>::value));
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
        ASSERT( (is_constructible<
                             bsl::shared_ptr<      int>,
                                     std::auto_ptr<int> >::value));
        ASSERT( (is_constructible<
                             bsl::shared_ptr<      int>,
                               std::auto_ptr<const int> >::value));
        // Note that this converts through auto_ptr_ref, and cannot be blocked.
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic pop
# endif
#endif
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR)
        ASSERT( (is_constructible<
                             bsl::shared_ptr<      int>,
                                   std::unique_ptr<int> >::value));
        ASSERT(!(is_constructible<
                             bsl::shared_ptr<      int>,
                             std::unique_ptr<const int> >::value));
        ASSERT(!(is_constructible<
                 bsl::shared_ptr<      int>,
                 std::unique_ptr<int, support::TypedDeleter<void> > >::value));
        ASSERT( (is_constructible<
                 bsl::shared_ptr<      int>,
                 std::unique_ptr<const int, support::DoNotDeleter > >::value));
#endif
        ASSERT( (is_constructible<
                             bsl::shared_ptr<      int>,
                                     bsl::weak_ptr<int> >::value));
        ASSERT(!(is_constructible<
                             bsl::shared_ptr<      int>,
                               bsl::weak_ptr<const int> >::value));
        ASSERT(!(is_constructible<
                             bsl::shared_ptr<      int>,
                                    bsl::weak_ptr<void> >::value));
        ASSERT( (is_constructible<
                             bsl::shared_ptr<      int>,
                                 bslma::ManagedPtr<int> >::value));
        ASSERT(!(is_constructible<
                             bsl::shared_ptr<      int>,
                           bslma::ManagedPtr<const int> >::value));
        ASSERT(!(is_constructible<
                             bsl::shared_ptr<      int>,
                                bslma::ManagedPtr<void> >::value));


        ASSERT( (is_constructible<
                             bsl::shared_ptr<const int>,
                                          bsl::nullptr_t>::value));
        ASSERT( (is_constructible<
                             bsl::shared_ptr<const int>,
                                                   int *>::value));
        ASSERT( (is_constructible<
                             bsl::shared_ptr<const int>,
                                             const int *>::value));
        ASSERT(!(is_constructible<
                             bsl::shared_ptr<const int>,
                                                  void *>::value));
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
        ASSERT( (is_constructible<
                             bsl::shared_ptr<const int>,
                                     std::auto_ptr<int> >::value));
        ASSERT( (is_constructible<
                             bsl::shared_ptr<const int>,
                               std::auto_ptr<const int> >::value));
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic pop
# endif
#endif
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR)
        ASSERT( (is_constructible<
                             bsl::shared_ptr<const int>,
                                   std::unique_ptr<int> >::value));
        ASSERT( (is_constructible<
                             bsl::shared_ptr<const int>,
                             std::unique_ptr<const int> >::value));
        ASSERT(!(is_constructible<
                 bsl::shared_ptr<const int>,
                 std::unique_ptr<int, support::TypedDeleter<void> > >::value));
        ASSERT( (is_constructible<
                 bsl::shared_ptr<const int>,
                 std::unique_ptr<const int, support::DoNotDeleter > >::value));
#endif
        ASSERT( (is_constructible<
                             bsl::shared_ptr<const int>,
                                     bsl::weak_ptr<int> >::value));
        ASSERT( (is_constructible<
                             bsl::shared_ptr<const int>,
                               bsl::weak_ptr<const int> >::value));
        ASSERT(!(is_constructible<
                             bsl::shared_ptr<const int>,
                                    bsl::weak_ptr<void> >::value));
        ASSERT( (is_constructible<
                             bsl::shared_ptr<const int>,
                                 bslma::ManagedPtr<int> >::value));
        ASSERT( (is_constructible<
                             bsl::shared_ptr<const int>,
                           bslma::ManagedPtr<const int> >::value));
        ASSERT(!(is_constructible<
                             bsl::shared_ptr<const int>,
                                bslma::ManagedPtr<void> >::value));


        ASSERT( (is_constructible<
                             bsl::shared_ptr<     void>,
                                          bsl::nullptr_t>::value));
        ASSERT( (is_constructible<
                             bsl::shared_ptr<     void>,
                                                   int *>::value));
        ASSERT(!(is_constructible<
                             bsl::shared_ptr<     void>,
                                             const int *>::value));
        ASSERT( (is_constructible<
                             bsl::shared_ptr<     void>,
                                                  void *>::value));
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
        ASSERT( (is_constructible<
                             bsl::shared_ptr<     void>,
                                     std::auto_ptr<int> >::value));
        ASSERT( (is_constructible<
                             bsl::shared_ptr<     void>,
                               std::auto_ptr<const int> >::value));
        // Note that this converts through auto_ptr_ref, and cannot be blocked.
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic pop
# endif
#endif
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR)
        ASSERT( (is_constructible<
                             bsl::shared_ptr<     void>,
                                   std::unique_ptr<int> >::value));
        ASSERT(!(is_constructible<
                             bsl::shared_ptr<     void>,
                             std::unique_ptr<const int> >::value));
        ASSERT(!(is_constructible<
           bsl::shared_ptr<     void>,
           std::unique_ptr<int, support::TypedDeleter<const void> > >::value));
        ASSERT( (is_constructible<
                 bsl::shared_ptr<     void>,
                 std::unique_ptr<const int, support::DoNotDeleter > >::value));
#endif
        ASSERT( (is_constructible<
                             bsl::shared_ptr<     void>,
                                     bsl::weak_ptr<int> >::value));
        ASSERT(!(is_constructible<
                             bsl::shared_ptr<     void>,
                               bsl::weak_ptr<const int> >::value));
        ASSERT( (is_constructible<
                             bsl::shared_ptr<     void>,
                                    bsl::weak_ptr<void> >::value));
        ASSERT( (is_constructible<
                             bsl::shared_ptr<     void>,
                                 bslma::ManagedPtr<int> >::value));
        ASSERT(!(is_constructible<
                             bsl::shared_ptr<     void>,
                           bslma::ManagedPtr<const int> >::value));
        ASSERT( (is_constructible<
                             bsl::shared_ptr<     void>,
                                bslma::ManagedPtr<void> >::value));



        // 2 arguments: [target, bslma-factory deleter]
        // Note that sometimes the factory-deleter is really an allocator.

        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , bsl::nullptr_t
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , int *
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , const int *
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , void *
                                      , bslma::TestAllocator *
                                      >::value));
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , std::auto_ptr<int>
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , std::auto_ptr<const int>
                                      , bslma::TestAllocator *
                                      >::value));
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic pop
# endif
#endif
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR)
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , std::unique_ptr<int>
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , std::unique_ptr<const int>
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<     void>
                                      , std::unique_ptr<const int>
                                      , bslma::TestAllocator *
                                      >::value));
#endif
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bsl::weak_ptr<int>
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bsl::weak_ptr<const int>
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bsl::weak_ptr<void>
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , bslma::ManagedPtr<int>
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , bslma::ManagedPtr<const int>
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bslma::ManagedPtr<void>
                                      , bslma::TestAllocator *
                                      >::value));

        // 2 arguments: [target, factory deleter object]

        ASSERT( (is_constructible< bsl::shared_ptr<      int>
                                      , bsl::nullptr_t
                                      , support::TypedFactory<int> *
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , bsl::nullptr_t
                                      , support::TypedFactory<int> *
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , int *
                                      , support::TypedFactory<int> *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , const int *
                                      , support::TypedFactory<int> *
                                      >::value));

        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bsl::nullptr_t
                                      , support::FactoryDeleter *
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , int *
                                      , support::FactoryDeleter *
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , const int *
                                      , support::FactoryDeleter *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , void *
                                      , support::FactoryDeleter *
                                      >::value));
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::auto_ptr<int>
                                      , support::FactoryDeleter *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::auto_ptr<const int>
                                      , support::FactoryDeleter *
                                      >::value));
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic pop
# endif
#endif
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR)
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::unique_ptr<int>
                                      , support::FactoryDeleter *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::unique_ptr<const int>
                                      , support::FactoryDeleter *
                                      >::value));
#endif
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bsl::weak_ptr<int>
                                      , support::FactoryDeleter *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bsl::weak_ptr<const int>
                                      , support::FactoryDeleter *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bsl::weak_ptr<void>
                                      , support::FactoryDeleter *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bslma::ManagedPtr<int>
                                      , support::FactoryDeleter *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bslma::ManagedPtr<const int>
                                      , support::FactoryDeleter *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bslma::ManagedPtr<void>
                                      , support::FactoryDeleter *
                                      >::value));


        // 2 arguments: [target, functor-pointer deleter]

        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , bsl::nullptr_t
                                      , bslstl::SharedPtrNilDeleter
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , int *
                                      , bslstl::SharedPtrNilDeleter
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , const int *
                                      , bslstl::SharedPtrNilDeleter
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , void *
                                      , bslstl::SharedPtrNilDeleter
                                      >::value));
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::auto_ptr<int>
                                      , bslstl::SharedPtrNilDeleter
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::auto_ptr<const int>
                                      , bslstl::SharedPtrNilDeleter
                                      >::value));
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic pop
# endif
#endif
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR)
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::unique_ptr<int>
                                      , bslstl::SharedPtrNilDeleter
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::unique_ptr<const int>
                                      , bslstl::SharedPtrNilDeleter
                                      >::value));
#endif
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bsl::weak_ptr<int>
                                      , bslstl::SharedPtrNilDeleter
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bsl::weak_ptr<const int>
                                      , bslstl::SharedPtrNilDeleter
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bsl::weak_ptr<void>
                                      , bslstl::SharedPtrNilDeleter
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bslma::ManagedPtr<int>
                                      , bslstl::SharedPtrNilDeleter
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bslma::ManagedPtr<const int>
                                      , bslstl::SharedPtrNilDeleter
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bslma::ManagedPtr<void>
                                      , bslstl::SharedPtrNilDeleter
                                      >::value));


        // 2 arguments: [target, function-pointer deleter]

        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , bsl::nullptr_t
                                      , void(*)(const int *)
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , int *
                                      , void(*)(const int *)
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , const int *
                                      , void(*)(const int *)
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , void *
                                      , void(*)(void *)
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , bsl::nullptr_t
                                      , void(*)(int *)
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , int *
                                      , void(*)(int *)
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , const int *
                                      , void(*)(int *)
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , void *
                                      , void(*)(const int *)
                                      >::value));
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::auto_ptr<int>
                                      , void(*)(const int *)
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::auto_ptr<const int>
                                      , void(*)(const int *)
                                      >::value));
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic pop
# endif
#endif
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR)
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::unique_ptr<int>
                                      , void(*)(const int *)
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::unique_ptr<const int>
                                      , void(*)(const int *)
                                      >::value));
#endif
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bsl::weak_ptr<int>
                                      , void(*)(const int *)
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bsl::weak_ptr<const int>
                                      , void(*)(const int *)
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bsl::weak_ptr<void>
                                      , void(*)(const int *)
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bslma::ManagedPtr<int>
                                      , void(*)(const int *)
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bslma::ManagedPtr<const int>
                                      , void(*)(const int *)
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bslma::ManagedPtr<void>
                                      , void(*)(const int *)
                                      >::value));



        // 3 arguments: [target, bslma-factory, bslma allocator]

        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , bsl::nullptr_t
                                      , bslma::Allocator *
                                      , bslma::Allocator *
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , int *
                                      , bslma::Allocator *
                                      , bslma::Allocator *
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , const int *
                                      , bslma::Allocator *
                                      , bslma::Allocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , void *
                                      , bslma::Allocator *
                                      , bslma::Allocator *
                                      >::value));
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::auto_ptr<int>
                                      , bslma::Allocator *
                                      , bslma::Allocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::auto_ptr<const int>
                                      , bslma::Allocator *
                                      , bslma::Allocator *
                                      >::value));
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic pop
# endif
#endif
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR)
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::unique_ptr<int>
                                      , bslma::Allocator *
                                      , bslma::Allocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::unique_ptr<const int>
                                      , bslma::Allocator *
                                      , bslma::Allocator *
                                      >::value));
#endif
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bsl::weak_ptr<int>
                                      , bslma::Allocator *
                                      , bslma::Allocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bsl::weak_ptr<const int>
                                      , bslma::Allocator *
                                      , bslma::Allocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bsl::weak_ptr<void>
                                      , bslma::Allocator *
                                      , bslma::Allocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bslma::ManagedPtr<int>
                                      , bslma::Allocator *
                                      , bslma::Allocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bslma::ManagedPtr<const int>
                                      , bslma::Allocator *
                                      , bslma::Allocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bslma::ManagedPtr<void>
                                      , bslma::Allocator *
                                      , bslma::Allocator *
                                      >::value));

        // 3 arguments: [target, functor-deleter, bslma allocator]

        ASSERT( (is_constructible< bsl::shared_ptr<      int>
                                      , bsl::nullptr_t
                                      , support::TypedFactory<int> *
                                      , bslma::Allocator *
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , bsl::nullptr_t
                                      , support::TypedFactory<int> *
                                      , bslma::Allocator *
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , int *
                                      , support::TypedFactory<int> *
                                      , bslma::Allocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , const int *
                                      , support::TypedFactory<int> *
                                      , bslma::Allocator *
                                      >::value));

        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , bsl::nullptr_t
                                      , bslstl::SharedPtrNilDeleter
                                      , bslma::Allocator *
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , int *
                                      , bslstl::SharedPtrNilDeleter
                                      , bslma::Allocator *
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , const int *
                                      , bslstl::SharedPtrNilDeleter
                                      , bslma::Allocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , void *
                                      , bslstl::SharedPtrNilDeleter
                                      , bslma::Allocator *
                                      >::value));
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::auto_ptr<int>
                                      , bslstl::SharedPtrNilDeleter
                                      , bslma::Allocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::auto_ptr<const int>
                                      , bslstl::SharedPtrNilDeleter
                                      , bslma::Allocator *
                                      >::value));
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic pop
# endif
#endif
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR)
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::unique_ptr<int>
                                      , bslstl::SharedPtrNilDeleter
                                      , bslma::Allocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::unique_ptr<const int>
                                      , bslstl::SharedPtrNilDeleter
                                      , bslma::Allocator *
                                      >::value));
#endif
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bsl::weak_ptr<int>
                                      , bslstl::SharedPtrNilDeleter
                                      , bslma::Allocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bsl::weak_ptr<const int>
                                      , bslstl::SharedPtrNilDeleter
                                      , bslma::Allocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bsl::weak_ptr<void>
                                      , bslstl::SharedPtrNilDeleter
                                      , bslma::Allocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bslma::ManagedPtr<int>
                                      , bslstl::SharedPtrNilDeleter
                                      , bslma::Allocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bslma::ManagedPtr<const int>
                                      , bslstl::SharedPtrNilDeleter
                                      , bslma::Allocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bslma::ManagedPtr<void>
                                      , bslstl::SharedPtrNilDeleter
                                      , bslma::Allocator *
                                      >::value));


        // 3 arguments: [target, function-pointer, bslma allocator]

        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , bsl::nullptr_t
                                      , void(*)(int *)
                                      , bslma::Allocator *
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , int *
                                      , void(*)(int *)
                                      , bslma::Allocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , const int *
                                      , void(*)(int *)
                                      , bslma::Allocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , void *
                                      , void(*)(void *)
                                      , bslma::Allocator *
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , bsl::nullptr_t
                                      , void(*)(const int *)
                                      , bslma::Allocator *
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , int *
                                      , void(*)(const int *)
                                      , bslma::Allocator *
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , const int *
                                      , void(*)(const int *)
                                      , bslma::Allocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , void *
                                      , void(*)(const int *)
                                      , bslma::Allocator *
                                      >::value));
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::auto_ptr<int>
                                      , void(*)(const int *)
                                      , bslma::Allocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::auto_ptr<const int>
                                      , void(*)(int *)
                                      , bslma::Allocator *
                                      >::value));
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic pop
# endif
#endif
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR)
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::unique_ptr<int>
                                      , void(*)(const int *)
                                      , bslma::Allocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::unique_ptr<const int>
                                      , void(*)(const int *)
                                      , bslma::Allocator *
                                      >::value));
#endif
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bsl::weak_ptr<int>
                                      , void(*)(const int *)
                                      , bslma::Allocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bsl::weak_ptr<const int>
                                      , void(*)(const int *)
                                      , bslma::Allocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bsl::weak_ptr<void>
                                      , void(*)(const int *)
                                      , bslma::Allocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bslma::ManagedPtr<int>
                                      , void(*)(const int *)
                                      , bslma::Allocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bslma::ManagedPtr<const int>
                                      , void(*)(const int *)
                                      , bslma::Allocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bslma::ManagedPtr<void>
                                      , void(*)(const int *)
                                      , bslma::Allocator *
                                      >::value));

        // 3 arguments: [target, bslma factory, derived-bslma allocator]

        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , bsl::nullptr_t
                                      , bslma::Allocator *
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , int *
                                      , bslma::Allocator *
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , const int *
                                      , bslma::Allocator *
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , void *
                                      , bslma::Allocator *
                                      , bslma::TestAllocator *
                                      >::value));
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::auto_ptr<int>
                                      , bslma::Allocator *
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::auto_ptr<const int>
                                      , bslma::Allocator *
                                      , bslma::TestAllocator *
                                      >::value));
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic pop
# endif
#endif
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR)
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::unique_ptr<int>
                                      , bslma::Allocator *
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::unique_ptr<const int>
                                      , bslma::Allocator *
                                      , bslma::TestAllocator *
                                      >::value));
#endif
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bsl::weak_ptr<int>
                                      , bslma::Allocator *
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bsl::weak_ptr<const int>
                                      , bslma::Allocator *
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bsl::weak_ptr<void>
                                      , bslma::Allocator *
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bslma::ManagedPtr<int>
                                      , bslma::Allocator *
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bslma::ManagedPtr<const int>
                                      , bslma::Allocator *
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bslma::ManagedPtr<void>
                                      , bslma::Allocator *
                                      , bslma::TestAllocator *
                                      >::value));


        // 3 arguments: [target, functor-deleter, derived-bslma allocator]

        ASSERT( (is_constructible< bsl::shared_ptr<      int>
                                      , bsl::nullptr_t
                                      , support::TypedFactory<int> *
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , bsl::nullptr_t
                                      , support::TypedFactory<int> *
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , int *
                                      , support::TypedFactory<int> *
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , const int *
                                      , support::TypedFactory<int> *
                                      , bslma::TestAllocator *
                                      >::value));

        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , bsl::nullptr_t
                                      , bslstl::SharedPtrNilDeleter
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , int *
                                      , bslstl::SharedPtrNilDeleter
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , const int *
                                      , bslstl::SharedPtrNilDeleter
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , void *
                                      , bslstl::SharedPtrNilDeleter
                                      , bslma::TestAllocator *
                                      >::value));
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::auto_ptr<int>
                                      , bslstl::SharedPtrNilDeleter
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::auto_ptr<const int>
                                      , bslstl::SharedPtrNilDeleter
                                      , bslma::TestAllocator *
                                      >::value));
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic pop
# endif
#endif
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR)
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::unique_ptr<int>
                                      , bslstl::SharedPtrNilDeleter
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::unique_ptr<const int>
                                      , bslstl::SharedPtrNilDeleter
                                      , bslma::TestAllocator *
                                      >::value));
#endif
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bsl::weak_ptr<int>
                                      , bslstl::SharedPtrNilDeleter
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bsl::weak_ptr<const int>
                                      , bslstl::SharedPtrNilDeleter
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bsl::weak_ptr<void>
                                      , bslstl::SharedPtrNilDeleter
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bslma::ManagedPtr<int>
                                      , bslstl::SharedPtrNilDeleter
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bslma::ManagedPtr<const int>
                                      , bslstl::SharedPtrNilDeleter
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bslma::ManagedPtr<void>
                                      , bslstl::SharedPtrNilDeleter
                                      , bslma::TestAllocator *
                                      >::value));


        // 3 arguments: [target, function-deleter, derived-bslma allocator]

        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , bsl::nullptr_t
                                      , void(*)(int *)
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , int *
                                      , void(*)(int *)
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , const int *
                                      , void(*)(int *)
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , void *
                                      , void(*)(void *)
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , bsl::nullptr_t
                                      , void(*)(const int *)
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , int *
                                      , void(*)(const int *)
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , const int *
                                      , void(*)(const int *)
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , void *
                                      , void(*)(const int *)
                                      , bslma::TestAllocator *
                                      >::value));
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::auto_ptr<int>
                                      , void(*)(int *)
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::auto_ptr<const int>
                                      , void(*)(const int *)
                                      , bslma::TestAllocator *
                                      >::value));
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic pop
# endif
#endif
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR)
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::unique_ptr<int>
                                      , void(*)(const int *)
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::unique_ptr<const int>
                                      , void(*)(const int *)
                                      , bslma::TestAllocator *
                                      >::value));
#endif
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bsl::weak_ptr<int>
                                      , void(*)(const int *)
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bsl::weak_ptr<const int>
                                      , void(*)(const int *)
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bsl::weak_ptr<void>
                                      , void(*)(const int *)
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bslma::ManagedPtr<int>
                                      , void(*)(const int *)
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bslma::ManagedPtr<const int>
                                      , void(*)(const int *)
                                      , bslma::TestAllocator *
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bslma::ManagedPtr<void>
                                      , void(*)(const int *)
                                      , bslma::TestAllocator *
                                      >::value));


        // 3 arguments: [target, bslma-factory, std allocator]

        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , bsl::nullptr_t
                                      , bslma::Allocator *
                                      , bsl::allocator<char>
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , int *
                                      , bslma::Allocator *
                                      , bsl::allocator<char>
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , const int *
                                      , bslma::Allocator *
                                      , bsl::allocator<char>
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , void *
                                      , bslma::Allocator *
                                      , bsl::allocator<char>
                                      >::value));
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::auto_ptr<int>
                                      , bslma::Allocator *
                                      , bsl::allocator<char>
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::auto_ptr<const int>
                                      , bslma::Allocator *
                                      , bsl::allocator<char>
                                      >::value));
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic pop
# endif
#endif
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR)
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::unique_ptr<int>
                                      , bslma::Allocator *
                                      , bsl::allocator<char>
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::unique_ptr<const int>
                                      , bslma::Allocator *
                                      , bsl::allocator<char>
                                      >::value));
#endif
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bsl::weak_ptr<int>
                                      , bslma::Allocator *
                                      , bsl::allocator<char>
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bsl::weak_ptr<const int>
                                      , bslma::Allocator *
                                      , bsl::allocator<char>
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bsl::weak_ptr<void>
                                      , bslma::Allocator *
                                      , bsl::allocator<char>
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bslma::ManagedPtr<int>
                                      , bslma::Allocator *
                                      , bsl::allocator<char>
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bslma::ManagedPtr<const int>
                                      , bslma::Allocator *
                                      , bsl::allocator<char>
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bslma::ManagedPtr<void>
                                      , bslma::Allocator *
                                      , bsl::allocator<char>
                                      >::value));


        // 3 arguments: [target, functor-deleter, std allocator]

        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , bsl::nullptr_t
                                      , bslstl::SharedPtrNilDeleter
                                      , bsl::allocator<char>
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , int *
                                      , bslstl::SharedPtrNilDeleter
                                      , bsl::allocator<char>
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , const int *
                                      , bslstl::SharedPtrNilDeleter
                                      , bsl::allocator<char>
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , void *
                                      , bslstl::SharedPtrNilDeleter
                                      , bsl::allocator<char>
                                      >::value));
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::auto_ptr<int>
                                      , bslstl::SharedPtrNilDeleter
                                      , bsl::allocator<char>
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::auto_ptr<const int>
                                      , bslstl::SharedPtrNilDeleter
                                      , bsl::allocator<char>
                                      >::value));
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic pop
# endif
#endif
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR)
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::unique_ptr<int>
                                      , bslstl::SharedPtrNilDeleter
                                      , bsl::allocator<char>
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::unique_ptr<const int>
                                      , bslstl::SharedPtrNilDeleter
                                      , bsl::allocator<char>
                                      >::value));
#endif
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bsl::weak_ptr<int>
                                      , bslstl::SharedPtrNilDeleter
                                      , bsl::allocator<char>
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bsl::weak_ptr<const int>
                                      , bslstl::SharedPtrNilDeleter
                                      , bsl::allocator<char>
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bsl::weak_ptr<void>
                                      , bslstl::SharedPtrNilDeleter
                                      , bsl::allocator<char>
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bslma::ManagedPtr<int>
                                      , bslstl::SharedPtrNilDeleter
                                      , bsl::allocator<char>
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bslma::ManagedPtr<const int>
                                      , bslstl::SharedPtrNilDeleter
                                      , bsl::allocator<char>
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bslma::ManagedPtr<void>
                                      , bslstl::SharedPtrNilDeleter
                                      , bsl::allocator<char>
                                      >::value));


        // 3 arguments: [target, function-deleter, std allocator]

        ASSERT( (is_constructible< bsl::shared_ptr<      int>
                                      , bsl::nullptr_t
                                      , support::TypedFactory<int> *
                                      , bsl::allocator<char>
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , bsl::nullptr_t
                                      , support::TypedFactory<int> *
                                      , bsl::allocator<char>
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , int *
                                      , support::TypedFactory<int> *
                                      , bsl::allocator<char>
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , const int *
                                      , support::TypedFactory<int> *
                                      , bsl::allocator<char>
                                      >::value));

        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , bsl::nullptr_t
                                      , void(*)(int *)
                                      , bsl::allocator<char>
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , int *
                                      , void(*)(int *)
                                      , bsl::allocator<char>
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , const int *
                                      , void(*)(int *)
                                      , bsl::allocator<char>
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , void *
                                      , void(*)(void *)
                                      , bsl::allocator<char>
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , bsl::nullptr_t
                                      , void(*)(const int *)
                                      , bsl::allocator<char>
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , int *
                                      , void(*)(const int *)
                                      , bsl::allocator<char>
                                      >::value));
        ASSERT( (is_constructible< bsl::shared_ptr<const int>
                                      , const int *
                                      , void(*)(const int *)
                                      , bsl::allocator<char>
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , void *
                                      , void(*)(const int *)
                                      , bsl::allocator<char>
                                      >::value));
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::auto_ptr<int>
                                      , void(*)(const int *)
                                      , bsl::allocator<char>
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::auto_ptr<const int>
                                      , void(*)(const int *)
                                      , bsl::allocator<char>
                                      >::value));
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic pop
# endif
#endif
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR)
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::unique_ptr<int>
                                      , void(*)(const int *)
                                      , bsl::allocator<char>
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , std::unique_ptr<const int>
                                      , void(*)(const int *)
                                      , bsl::allocator<char>
                                      >::value));
#endif
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bsl::weak_ptr<int>
                                      , void(*)(const int *)
                                      , bsl::allocator<char>
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bsl::weak_ptr<const int>
                                      , void(*)(const int *)
                                      , bsl::allocator<char>
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bsl::weak_ptr<void>
                                      , void(*)(const int *)
                                      , bsl::allocator<char>
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bslma::ManagedPtr<int>
                                      , void(*)(const int *)
                                      , bsl::allocator<char>
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bslma::ManagedPtr<const int>
                                      , void(*)(const int *)
                                      , bsl::allocator<char>
                                      >::value));
        ASSERT(!(is_constructible< bsl::shared_ptr<const int>
                                      , bslma::ManagedPtr<void>
                                      , void(*)(const int *)
                                      , bsl::allocator<char>
                                      >::value));


        // WEAK POINTER CONSTRUCTORS


        // Basic default-constructible tests: breathing test for types that
        // follow.

        ASSERT( (is_constructible<bsl::weak_ptr<const int>>::value));
        ASSERT( (is_constructible<bsl::weak_ptr<      int>>::value));
        ASSERT( (is_constructible<bsl::weak_ptr<     void>>::value));

        // Copying constructors

        ASSERT( (is_constructible<bsl::weak_ptr<const int>,
                                  bsl::weak_ptr<const int>>::value));
        ASSERT( (is_constructible<bsl::weak_ptr<const int>,
                                  bsl::weak_ptr<      int>>::value));
        ASSERT(!(is_constructible<bsl::weak_ptr<const int>,
                                  bsl::weak_ptr<     void>>::value));

        ASSERT(!(is_constructible<bsl::weak_ptr<      int>,
                                  bsl::weak_ptr<const int>>::value));
        ASSERT( (is_constructible<bsl::weak_ptr<      int>,
                                  bsl::weak_ptr<      int>>::value));
        ASSERT(!(is_constructible<bsl::weak_ptr<      int>,
                                  bsl::weak_ptr<     void>>::value));

        ASSERT(!(is_constructible<bsl::weak_ptr<     void>,
                                  bsl::weak_ptr<const int>>::value));
        ASSERT( (is_constructible<bsl::weak_ptr<     void>,
                                  bsl::weak_ptr<      int>>::value));
        ASSERT( (is_constructible<bsl::weak_ptr<     void>,
                                  bsl::weak_ptr<     void>>::value));


        ASSERT( (is_constructible<bsl::weak_ptr  <const int>,
                                  bsl::shared_ptr<const int>>::value));
        ASSERT( (is_constructible<bsl::weak_ptr  <const int>,
                                  bsl::shared_ptr<      int>>::value));
        ASSERT(!(is_constructible<bsl::weak_ptr  <const int>,
                                  bsl::shared_ptr<     void>>::value));

        ASSERT(!(is_constructible<bsl::weak_ptr  <      int>,
                                  bsl::shared_ptr<const int>>::value));
        ASSERT( (is_constructible<bsl::weak_ptr  <      int>,
                                  bsl::shared_ptr<      int>>::value));
        ASSERT(!(is_constructible<bsl::weak_ptr  <      int>,
                                  bsl::shared_ptr<     void>>::value));

        ASSERT(!(is_constructible<bsl::weak_ptr  <     void>,
                                  bsl::shared_ptr<const int>>::value));
        ASSERT( (is_constructible<bsl::weak_ptr  <     void>,
                                  bsl::shared_ptr<      int>>::value));
        ASSERT( (is_constructible<bsl::weak_ptr  <     void>,
                                  bsl::shared_ptr<     void>>::value));

        // Moving constructors

        ASSERT( (is_constructible<
                         bsl::weak_ptr<const int>,
                         bslmf::MovableRef<bsl::weak_ptr<const int>>>::value));
        ASSERT( (is_constructible<
                         bsl::weak_ptr<const int>,
                         bslmf::MovableRef<bsl::weak_ptr<      int>>>::value));
        ASSERT(!(is_constructible<
                         bsl::weak_ptr<const int>,
                         bslmf::MovableRef<bsl::weak_ptr<     void>>>::value));

        ASSERT(!(is_constructible<
                         bsl::weak_ptr<      int>,
                         bslmf::MovableRef<bsl::weak_ptr<const int>>>::value));
        ASSERT( (is_constructible<
                         bsl::weak_ptr<      int>,
                         bslmf::MovableRef<bsl::weak_ptr<      int>>>::value));
        ASSERT(!(is_constructible<
                         bsl::weak_ptr<      int>,
                         bslmf::MovableRef<bsl::weak_ptr<     void>>>::value));

        ASSERT(!(is_constructible<
                         bsl::weak_ptr<     void>,
                         bslmf::MovableRef<bsl::weak_ptr<const int>>>::value));
        ASSERT( (is_constructible<
                         bsl::weak_ptr<      void>,
                         bslmf::MovableRef<bsl::weak_ptr<      int>>>::value));
        ASSERT( (is_constructible<
                         bsl::weak_ptr<     void>,
                         bslmf::MovableRef<bsl::weak_ptr<     void>>>::value));


        ASSERT( (is_constructible<
                       bsl::weak_ptr<const int>,
                       bslmf::MovableRef<bsl::shared_ptr<const int>>>::value));
        ASSERT( (is_constructible<
                       bsl::weak_ptr<const int>,
                       bslmf::MovableRef<bsl::shared_ptr<      int>>>::value));
        ASSERT(!(is_constructible<
                       bsl::weak_ptr<const int>,
                       bslmf::MovableRef<bsl::shared_ptr<     void>>>::value));

        ASSERT(!(is_constructible<
                       bsl::weak_ptr<      int>,
                       bslmf::MovableRef<bsl::shared_ptr<const int>>>::value));
        ASSERT( (is_constructible<
                       bsl::weak_ptr<      int>,
                       bslmf::MovableRef<bsl::shared_ptr<      int>>>::value));
        ASSERT(!(is_constructible<
                       bsl::weak_ptr<      int>,
                       bslmf::MovableRef<bsl::shared_ptr<     void>>>::value));

        ASSERT(!(is_constructible<
                       bsl::weak_ptr<     void>,
                       bslmf::MovableRef<bsl::shared_ptr<const int>>>::value));
        ASSERT( (is_constructible<
                       bsl::weak_ptr<      void>,
                       bslmf::MovableRef<bsl::shared_ptr<      int>>>::value));
        ASSERT( (is_constructible<
                       bsl::weak_ptr<     void>,
                       bslmf::MovableRef<bsl::shared_ptr<     void>>>::value));
#endif
      } break;
      case 35:{
        // --------------------------------------------------------------------
        // TESTING 'enable_shared_from_this<T>'
        //   Test the whole 'enable_shared_from_this' facility, which includes
        //   a variety of interactions with 'shared_ptr' and 'weak_ptr', and
        //   not just directly testing the 'enable_shared_from_this' class
        //   template itself.
        //
        // Concerns:
        //   1) 'shared_ptr' constructors are able to identify correctly the
        //      'enable_shared_from_this' (possibly indirect) base class and
        //      initialize the 'd_weakThis' 'weak ptr'.
        //   2) Converting from a 'ManagedPtr' or 'auto_ptr' to a 'shared_ptr'
        //      will initialize 'd_weakThis' 'weak_ptr' correctly.
        //   3) Calling 'shared_from_this()' will create a new reference to the
        //      original owning 'shared_ptr'.
        //   4) All relevant 'shared_ptr<const T>' constructors are able to
        //      initialize 'enable_shared_from_this<T>::d_weakThis'.
        //   5) The 'd_weakThis' member does not rebind on creation of a
        //      subsequent owner-group, unless it has already expired.
        //   6) In addition to constructors, the 'd_weakThis' member binds
        //      correctly for 'load', 'reset', and 'createInplace' methods.
        //
        // Plan:
        //   First demonstrate that the 'shared_from_this' facility behaves
        //   correctly when it is NOT in use, i,e,, when used with a shared
        //   pointer that does not own its target.  Then test that everything
        //   works correctly when creating 'shared_ptr' objects from raw
        //   'SharedPtrRep' objects, as this is the basis for 'weak_ptr::lock'
        //   that drives much of the subsequent testing.  Next, test the basic
        //   facility behaves correctly when used with each possible
        //   'shared_ptr' constructor.  Then check additional behaviors with
        //   extended lifetimes and multiple 'shared_ptr' owner-groups.
        //   Finally, check that the various factory functions that can create
        //   a new shared pointer value also behave correctly with respect to
        //   this facility.
        //
        // Testing:
        //   enable_shared_from_this()
        //   enable_shared_from_this(const enable_shared_from_this&)
        //   ~enable_shared_from_this()
        //   enable_shared_from_this& operator=(const enable_shared_from_this&)
        //   shared_ptr<T> shared_from_this()
        //   shared_ptr<const T> shared_from_this() const
        //   weak_ptr<T> weak_from_this()
        //   weak_ptr<const T> weak_from_this() const
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'enable_shared_from_this<T>'"
                            "\n====================================\n");

        typedef bsl::shared_ptr<ShareThis> SharedPtr;
        typedef bsl::shared_ptr<const ShareThis> ConstSharedPtr;
        typedef bsl::shared_ptr<ShareThisDerived> SharedPtrDerived;
        typedef bsl::shared_ptr<const ShareThisDerived> ConstSharedPtrDerived;

        typedef bsl::weak_ptr<ShareThis> WeakPtr;
        typedef bsl::weak_ptr<const ShareThis> ConstWeakPtr;

        int destructorCount = 0;   // observe 'ShareThis' objects are destroyed

        bslma::TestAllocator ta("enable_shared_from_this test",
                                veryVeryVeryVerbose);

        if (verbose) printf("\nTesting when the facility is not used"
                            "\n-------------------------------------\n");

        if (verbose) printf("\n\tBasic usage\n");
        {
            ShareThis stackThis(&destructorCount);
            {
#if defined(BDE_BUILD_TARGET_EXC)
                // Test exception thrown when not yet owned by a 'shared_ptr'
                bool caughtException = false;
                try {
                    SharedPtr badPtr = stackThis.shared_from_this();
                    ASSERTV("Should have thrown a 'bad_weak_ptr'.", false);
                }
                catch(const bsl::bad_weak_ptr&) {
                    caughtException = true;
                }
                ASSERT(caughtException);
#endif

                WeakPtr wPtr = stackThis.weak_from_this();
                ASSERT(wPtr.expired());
            }
        }
        ASSERTV(destructorCount, 1 == destructorCount);
        destructorCount = 0;    // reset 'destructorCount' for next test.

        if (verbose) printf("\n\tBasic usage with 'const' element type\n");
        {
            const ShareThis stackThis(&destructorCount);
            {
#if defined(BDE_BUILD_TARGET_EXC)
                // Test exception thrown when not yet owned by a 'shared_ptr'
                bool caughtException = false;
                try {
                    ConstSharedPtr badPtr = stackThis.shared_from_this();
                    ASSERTV("Should have thrown a 'bad_weak_ptr'.", false);
                }
                catch(const bsl::bad_weak_ptr&) {
                    caughtException = true;
                }
                ASSERT(caughtException);
#endif
                ConstWeakPtr wPtr = stackThis.weak_from_this();
                ASSERT(wPtr.expired());
            }
        }
        ASSERTV(destructorCount, 1 == destructorCount);
        destructorCount = 0;    // reset 'destructorCount' for next test.

        if (verbose) printf("\n\tBasic usage of most-derived type\n");
        {
            ShareThisDerived stackThis(&destructorCount);
            {
#if defined(BDE_BUILD_TARGET_EXC)
                // Test exception thrown when not yet owned by a 'shared_ptr'
                bool caughtException = false;
                try {
                    SharedPtr badPtr = stackThis.shared_from_this();
                    ASSERTV("Should have thrown a 'bad_weak_ptr'.", false);
                }
                catch(const bsl::bad_weak_ptr&) {
                    caughtException = true;
                }
                ASSERT(caughtException);
#endif
                WeakPtr wPtr = stackThis.weak_from_this();
                ASSERT(wPtr.expired());
            }
        }
        ASSERTV(destructorCount, 11 == destructorCount);
        destructorCount = 0;    // reset 'destructorCount' for next test.

        if (verbose) printf("\n\tBasic usage of 'const' most-derived type\n");
        {
            const ShareThisDerived stackThis(&destructorCount);
            {
#if defined(BDE_BUILD_TARGET_EXC)
                // Test exception thrown when not yet owned by a 'shared_ptr'
                bool caughtException = false;
                try {
                    ConstSharedPtr badPtr = stackThis.shared_from_this();
                    ASSERTV("Should have thrown a 'bad_weak_ptr'.", false);
                }
                catch(const bsl::bad_weak_ptr&) {
                    caughtException = true;
                }
                ASSERT(caughtException);
#endif
                ConstWeakPtr wPtr = stackThis.weak_from_this();
                ASSERT(wPtr.expired());
            }
        }
        ASSERTV(destructorCount, 11 == destructorCount);
        destructorCount = 0;    // reset 'destructorCount' for next test.

        if (verbose) printf("\n\tTest with un-owned pointer\n");
        {
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR)
            // Prefer 'unique_ptr' as future-proof solution after 'auto_ptr' is
            // removed by C++17.
            std::unique_ptr<ShareThis> originalPtr(
                                              new ShareThis(&destructorCount));
#else
            std::auto_ptr<ShareThis> originalPtr(
                                              new ShareThis(&destructorCount));
#endif
            SharedPtr unownedPtr(SharedPtr(), originalPtr.get());

            {
#if defined(BDE_BUILD_TARGET_EXC)
                // Test exception thrown when not yet owned by a 'shared_ptr'
                bool caughtException = false;
                try {
                    SharedPtr badPtr = unownedPtr->shared_from_this();
                    ASSERTV("Should have thrown a 'bad_weak_ptr'.", false);
                }
                catch(const bsl::bad_weak_ptr&) {
                    caughtException = true;
                }
                ASSERT(caughtException);
#endif

                WeakPtr wPtr = unownedPtr->weak_from_this();
                ASSERT(wPtr.expired());
            }
        }
        ASSERTV(destructorCount, 1 == destructorCount);
        destructorCount = 0;    // reset 'destructorCount' for next test.

        if (verbose) printf(
                      "\nTesting facility with 'SharedPtrRep' constructors"
                      "\n-------------------------------------------------\n");

        int disposeObjectCount = 0;
        int disposeRepCount = 0;

        if (verbose) printf("\nTesting constructor with abstract rep"
                            "\n-------------------------------------\n");
        {
            ShareThis *pThis = new(ta) ShareThis(&destructorCount);
            WeakPtr weakBefore = pThis->weak_from_this();
            ASSERT(weakBefore.expired());

            bslma::ManagedPtr<ShareThis> manager(pThis, &ta);  // Clean-up

            NonOwningRep<ShareThis> *rep = new(ta) NonOwningRep<ShareThis>(
                                                            pThis,
                                                           &disposeObjectCount,
                                                           &disposeRepCount,
                                                           &ta);
            const NonOwningRep<ShareThis>& REP = *rep;

            ASSERTV(disposeRepCount,     0 == disposeRepCount);
            ASSERTV(disposeObjectCount,  0 == disposeObjectCount);
            ASSERTV(REP.numReferences(), 1 == REP.numReferences());
            ASSERTV(REP.ptr(), pThis,    REP.ptr() == pThis);

            {
                bslma::SharedPtrRep *pRep = rep;  // Cast to base
                SharedPtr ptr(pThis, pRep);
                ASSERTV(ptr.use_count(),     ptr.use_count() == 1);
                ASSERTV(disposeRepCount,     0 == disposeRepCount);
                ASSERTV(disposeObjectCount,  0 == disposeObjectCount);
                ASSERTV(REP.numReferences(), 1 == REP.numReferences());

                WeakPtr weakAfter = pThis->weak_from_this();
                ASSERT(!weakAfter.expired());
                ASSERT(weakBefore.expired());

                SharedPtr sharedThisPtr = ptr->shared_from_this();
                ASSERT(ptr.get() == sharedThisPtr.get());
                ASSERT(ptr.use_count() == 2);

                // Confirm weak and shared pointers use the same rep object.
                ASSERT(!sharedThisPtr.owner_before(weakAfter));
                ASSERT(!weakAfter.owner_before(sharedThisPtr));

                ASSERTV(destructorCount, 0 == destructorCount);
            }
            ASSERTV(destructorCount,     0 == destructorCount);
            ASSERTV(disposeRepCount,     0 == disposeRepCount);
            ASSERTV(disposeObjectCount,  1 == disposeObjectCount);
            ASSERTV(REP.numReferences(), 0 == REP.numReferences());
        }
        ASSERTV(destructorCount,    1 == destructorCount);
        ASSERTV(disposeRepCount,    1 == disposeRepCount);
        ASSERTV(disposeObjectCount, 1 == disposeObjectCount);
        destructorCount    = 0; // reset 'destructorCount' for next test.
        disposeRepCount    = 0;
        disposeObjectCount = 0;

        if (verbose) printf("\nTesting constructor with typed-rep"
                            "\n----------------------------------\n");
        {
            ShareThis *pThis = new(ta) ShareThis(&destructorCount);
            WeakPtr weakBefore = pThis->weak_from_this();
            ASSERT(weakBefore.expired());

            bslma::ManagedPtr<ShareThis> manager(pThis, &ta);  // Clean-up

            NonOwningRep<ShareThis> *rep = new(ta) NonOwningRep<ShareThis>(
                                                            pThis,
                                                           &disposeObjectCount,
                                                           &disposeRepCount,
                                                           &ta);
            const NonOwningRep<ShareThis>& REP = *rep;

            ASSERTV(REP.ptr(), pThis,    REP.ptr() == pThis);
            ASSERTV(REP.numReferences(), 1 == REP.numReferences());
            ASSERTV(disposeRepCount,     0 == disposeRepCount);
            ASSERTV(disposeObjectCount,  0 == disposeObjectCount);

            {
                SharedPtr ptr(pThis, rep);
                ASSERTV(ptr.use_count(),     ptr.use_count() == 1);
                ASSERTV(disposeRepCount,     0 == disposeRepCount);
                ASSERTV(disposeObjectCount,  0 == disposeObjectCount);
                ASSERTV(REP.numReferences(), 1 == REP.numReferences());

                WeakPtr weakAfter = pThis->weak_from_this();
                ASSERT(!weakAfter.expired());
                ASSERT(weakBefore.expired());

                SharedPtr sharedThisPtr = ptr->shared_from_this();
                ASSERT(ptr.get() == sharedThisPtr.get());
                ASSERT(ptr.use_count() == 2);

                // Confirm weak and shared pointers use the same rep object.
                ASSERT(!sharedThisPtr.owner_before(weakAfter));
                ASSERT(!weakAfter.owner_before(sharedThisPtr));

                ASSERTV(destructorCount, 0 == destructorCount);
            }
            ASSERTV(destructorCount,     0 == destructorCount);
            ASSERTV(REP.numReferences(), 0 == REP.numReferences());
            ASSERTV(disposeRepCount,     0 == disposeRepCount);
            ASSERTV(disposeObjectCount,  1 == disposeObjectCount);
        }
        ASSERTV(destructorCount,    1 == destructorCount);
        ASSERTV(disposeRepCount,    1 == disposeRepCount);
        ASSERTV(disposeObjectCount, 1 == disposeObjectCount);
        destructorCount    = 0; // reset 'destructorCount' for next test.
        disposeRepCount    = 0;
        disposeObjectCount = 0;

        if (verbose) printf(
        "\nTesting constructor with abstract rep and from-'shared_ptr' tag"
        "\n---------------------------------------------------------------\n");
        {
            ShareThis *pThis = new(ta) ShareThis(&destructorCount);
            WeakPtr weakBefore = pThis->weak_from_this();
            ASSERT(weakBefore.expired());

            bslma::ManagedPtr<ShareThis> manager(pThis, &ta);  // Clean-up

            NonOwningRep<ShareThis> *rep = new(ta) NonOwningRep<ShareThis>(
                                                            pThis,
                                                           &disposeObjectCount,
                                                           &disposeRepCount,
                                                           &ta);
            const NonOwningRep<ShareThis>& REP = *rep;
            bslma::SharedPtrRep *pRep = rep;   // Cast to base

            ASSERTV(disposeRepCount,     0 == disposeRepCount);
            ASSERTV(disposeObjectCount,  0 == disposeObjectCount);
            ASSERTV(REP.numReferences(), 1 == REP.numReferences());
            ASSERTV(REP.numWeakReferences(), 0 == REP.numWeakReferences());
            ASSERTV(REP.ptr(), pThis,    REP.ptr() == pThis);

            {
                // Establish the pre-condition of a rep already owned by a
                // 'shared_ptr' object.
                SharedPtr firstPtr(pThis, pRep);
                ASSERTV(disposeRepCount,      0 == disposeRepCount);
                ASSERTV(disposeObjectCount,   0 == disposeObjectCount);
                ASSERTV(REP.numReferences(),  1 == REP.numReferences());
                ASSERTV(REP.numWeakReferences(), 1 == REP.numWeakReferences());

                WeakPtr weakFirst = pThis->weak_from_this();
                ASSERT(!weakFirst.expired());
                ASSERT(weakBefore.expired());
                ASSERTV(REP.numReferences(),     1 == REP.numReferences());
                ASSERTV(REP.numWeakReferences(), 2 == REP.numWeakReferences());

                manager.reset();    // Destroy the SharedThis object managed,
                                    // but not owned, by '*rep'.  'd_weakThis'
                                    // will release on weak reference, and the
                                    // 'destructorCount' will increment, but
                                    // NOT 'disposeObjectCount' as the 'rep'
                                    // maintains a non-owning strong reference
                                    // to the destroyed object.

                ASSERTV(destructorCount,     1 == destructorCount);
                ASSERTV(disposeRepCount,     0 == disposeRepCount);
                ASSERTV(disposeObjectCount,  0 == disposeObjectCount);
                ASSERTV(REP.numReferences(), 1 == REP.numReferences());
                ASSERTV(REP.numWeakReferences(),
                                                1  == REP.numWeakReferences());

                const bool acquiredRef = pRep->tryAcquireRef();
                ASSERT(acquiredRef);

                {
                    SharedPtr testPtr(
                                 pThis,
                                 pRep,
                                 bslstl::SharedPtr_RepFromExistingSharedPtr());
                    ASSERTV(testPtr.use_count(), 2 == testPtr.use_count());
                    ASSERTV(disposeRepCount,     0 == disposeRepCount);
                    ASSERTV(disposeObjectCount,  0 == disposeObjectCount);
                    ASSERTV(REP.numReferences(), 2 == REP.numReferences());
                    ASSERTV(REP.numWeakReferences(),
                                                1  == REP.numWeakReferences());
                }
                ASSERTV(destructorCount,     1 == destructorCount);
                ASSERTV(disposeRepCount,     0 == disposeRepCount);
                ASSERTV(disposeObjectCount,  0 == disposeObjectCount);
                ASSERTV(REP.numReferences(), 1 == REP.numReferences());
                ASSERTV(REP.numWeakReferences(),
                                                1  == REP.numWeakReferences());

                weakBefore = weakFirst;  // ensure a weak pointer outlives the
                                         // last 'shared_ptr'
                ASSERTV(destructorCount,     1 == destructorCount);
                ASSERTV(disposeRepCount,     0 == disposeRepCount);
                ASSERTV(disposeObjectCount,  0 == disposeObjectCount);
                ASSERTV(REP.numReferences(), 1 == REP.numReferences());
                ASSERTV(REP.numWeakReferences(),
                                                2  == REP.numWeakReferences());

            }
            ASSERTV(destructorCount,     1 == destructorCount);
            ASSERTV(disposeRepCount,     0 == disposeRepCount);
            ASSERTV(disposeObjectCount,  1 == disposeObjectCount);
            ASSERTV(REP.numReferences(), 0 == REP.numReferences());
            ASSERTV(REP.numWeakReferences(), 1  == REP.numWeakReferences());
        }
        ASSERTV(destructorCount,    1 == destructorCount);
        ASSERTV(disposeRepCount,    1 == disposeRepCount);
        ASSERTV(disposeObjectCount, 1 == disposeObjectCount);
        destructorCount    = 0; // reset 'destructorCount' for next test.
        disposeRepCount    = 0;
        disposeObjectCount = 0;

        if (verbose) printf(
                 "\nTesting constructor with rep and from-'shared_ptr' tag"
                 "\n------------------------------------------------------\n");
        {
            ShareThis *pThis = new(ta) ShareThis(&destructorCount);
            WeakPtr weakBefore = pThis->weak_from_this();
            ASSERT(weakBefore.expired());

            bslma::ManagedPtr<ShareThis> manager(pThis, &ta);  // Clean-up

            NonOwningRep<ShareThis> *rep = new(ta) NonOwningRep<ShareThis>(
                                                            pThis,
                                                           &disposeObjectCount,
                                                           &disposeRepCount,
                                                           &ta);
            const NonOwningRep<ShareThis>& REP = *rep;

            ASSERTV(disposeRepCount,     0 == disposeRepCount);
            ASSERTV(disposeObjectCount,  0 == disposeObjectCount);
            ASSERTV(REP.numReferences(), 1 == REP.numReferences());
            ASSERTV(REP.numWeakReferences(), 0 == REP.numWeakReferences());
            ASSERTV(REP.ptr(), pThis,    REP.ptr() == pThis);

            {
                // Establish the pre-condition of a rep already owned by a
                // 'shared_ptr' object.
                SharedPtr firstPtr(pThis, rep);
                ASSERTV(disposeRepCount,      0 == disposeRepCount);
                ASSERTV(disposeObjectCount,   0 == disposeObjectCount);
                ASSERTV(REP.numReferences(),  1 == REP.numReferences());
                ASSERTV(REP.numWeakReferences(), 1 == REP.numWeakReferences());

                WeakPtr weakFirst = pThis->weak_from_this();
                ASSERT(!weakFirst.expired());
                ASSERT(weakBefore.expired());
                ASSERTV(REP.numReferences(),     1 == REP.numReferences());
                ASSERTV(REP.numWeakReferences(), 2 == REP.numWeakReferences());

                manager.reset();    // Destroy the SharedThis object managed,
                                    // but not owned, by '*rep'.  'd_weakThis'
                                    // will release on weak reference, and the
                                    // 'destructorCount' will increment, but
                                    // NOT 'disposeObjectCount' as the 'rep'
                                    // maintains a non-owning strong reference
                                    // to the destroyed object.

                ASSERTV(destructorCount,     1 == destructorCount);
                ASSERTV(disposeRepCount,     0 == disposeRepCount);
                ASSERTV(disposeObjectCount,  0 == disposeObjectCount);
                ASSERTV(REP.numReferences(), 1 == REP.numReferences());
                ASSERTV(REP.numWeakReferences(),
                                                1  == REP.numWeakReferences());

                const bool acquiredRef = rep->tryAcquireRef();
                ASSERT(acquiredRef);

                {
                    SharedPtr testPtr(
                                 pThis,
                                 rep,
                                 bslstl::SharedPtr_RepFromExistingSharedPtr());
                    ASSERTV(testPtr.use_count(), 2 == testPtr.use_count());
                    ASSERTV(disposeRepCount,     0 == disposeRepCount);
                    ASSERTV(disposeObjectCount,  0 == disposeObjectCount);
                    ASSERTV(REP.numReferences(), 2 == REP.numReferences());
                    ASSERTV(REP.numWeakReferences(),
                                                1  == REP.numWeakReferences());
                }
                ASSERTV(destructorCount,     1 == destructorCount);
                ASSERTV(disposeRepCount,     0 == disposeRepCount);
                ASSERTV(disposeObjectCount,  0 == disposeObjectCount);
                ASSERTV(REP.numReferences(), 1 == REP.numReferences());
                ASSERTV(REP.numWeakReferences(),
                                                1  == REP.numWeakReferences());

                weakBefore = weakFirst;  // ensure a weak pointer outlives the
                                         // last 'shared_ptr'
                ASSERTV(destructorCount,     1 == destructorCount);
                ASSERTV(disposeRepCount,     0 == disposeRepCount);
                ASSERTV(disposeObjectCount,  0 == disposeObjectCount);
                ASSERTV(REP.numReferences(), 1 == REP.numReferences());
                ASSERTV(REP.numWeakReferences(),
                                                2  == REP.numWeakReferences());

            }
            ASSERTV(destructorCount,     1 == destructorCount);
            ASSERTV(disposeRepCount,     0 == disposeRepCount);
            ASSERTV(disposeObjectCount,  1 == disposeObjectCount);
            ASSERTV(REP.numReferences(), 0 == REP.numReferences());
            ASSERTV(REP.numWeakReferences(), 1  == REP.numWeakReferences());
        }
        ASSERTV(destructorCount,    1 == destructorCount);
        ASSERTV(disposeRepCount,    1 == disposeRepCount);
        ASSERTV(disposeObjectCount, 1 == disposeObjectCount);
        destructorCount    = 0; // reset 'destructorCount' for next test.
        disposeRepCount    = 0;
        disposeObjectCount = 0;

        if (verbose) printf("\nTesting simple usage of the facility"
                            "\n------------------------------------\n");

        if (verbose) printf("\n\tBasic usage\n");
        {
            ShareThis *pThis = new ShareThis(&destructorCount);
            WeakPtr weakBefore = pThis->weak_from_this();
            ASSERT(weakBefore.expired());

            SharedPtr ptr(pThis);
            ASSERT(ptr.use_count() == 1);

            WeakPtr weakAfter = pThis->weak_from_this();
            ASSERT(!weakAfter.expired());
            ASSERT(weakBefore.expired());

            SharedPtr sharedThisPtr = ptr->shared_from_this();
            ASSERT(ptr.get() == sharedThisPtr.get());
            ASSERT(ptr.use_count() == 2);

            // Confirm that weak and shared pointer have the same rep object.
            ASSERT(!sharedThisPtr.owner_before(weakAfter));
            ASSERT(!weakAfter.owner_before(sharedThisPtr));

            ASSERTV(destructorCount, 0 == destructorCount);
        }
        ASSERTV(destructorCount, 1 == destructorCount);
        destructorCount = 0;    // reset 'destructorCount' for next test.

        if (verbose) printf("\n\tBasic usage with 'const' element type\n");
        {
            const ShareThis *pThis = new ShareThis(&destructorCount);
            ConstWeakPtr weakBefore = pThis->weak_from_this();
            ASSERT(weakBefore.expired());

            ConstSharedPtr ptr(pThis);
            ASSERT(ptr.use_count() == 1);

            ConstWeakPtr weakAfter = pThis->weak_from_this();
            ASSERT(!weakAfter.expired());
            ASSERT(weakBefore.expired());

            ConstSharedPtr sharedThisPtr = ptr->shared_from_this();
            ASSERT(ptr.get() == sharedThisPtr.get());
            ASSERT(ptr.use_count() == 2);

            // Confirm that weak and shared pointer have the same rep object.
            ASSERT(!sharedThisPtr.owner_before(weakAfter));
            ASSERT(!weakAfter.owner_before(sharedThisPtr));

            ASSERTV(destructorCount, 0 == destructorCount);
        }
        ASSERTV(destructorCount, 1 == destructorCount);
        destructorCount = 0;    // reset 'destructorCount' for next test.

        if (verbose) printf("\n\t'shared_from_this' into shared<constT>'\n");
        {
            ShareThis *pThis = new ShareThis(&destructorCount);
            WeakPtr weakBefore = pThis->weak_from_this();
            ASSERT(weakBefore.expired());

            SharedPtr ptr(pThis);
            ASSERT(ptr.use_count() == 1);

            WeakPtr weakAfter = pThis->weak_from_this();
            ASSERT(!weakAfter.expired());
            ASSERT(weakBefore.expired());

            ConstSharedPtr sharedThisPtr = ptr->shared_from_this();
            ASSERT(ptr.get() == sharedThisPtr.get());
            ASSERT(ptr.use_count() == 2);

            // Confirm that weak and shared pointer have the same rep object.
            ASSERT(!sharedThisPtr.owner_before(weakAfter));
            ASSERT(!weakAfter.owner_before(sharedThisPtr));

            ASSERTV(destructorCount, 0 == destructorCount);
        }
        ASSERTV(destructorCount, 1 == destructorCount);
        destructorCount = 0;    // reset 'destructorCount' for next test.

        if (verbose) printf("\n\tBasic usage of most-derived type\n");
        {
            ShareThisDerived *pThis = new ShareThisDerived(&destructorCount);
            WeakPtr weakBefore = pThis->weak_from_this();
            ASSERT(weakBefore.expired());

            SharedPtrDerived ptr(pThis);
            ASSERT(ptr.use_count() == 1);

            WeakPtr weakAfter = pThis->weak_from_this();
            ASSERT(!weakAfter.expired());
            ASSERT(weakBefore.expired());

            SharedPtr sharedThisPtr = ptr->shared_from_this();
            ASSERT(ptr.get() == sharedThisPtr.get());
            ASSERT(ptr.use_count() == 2);

            // Confirm that weak and shared pointer have the same rep object.
            ASSERT(!sharedThisPtr.owner_before(weakAfter));
            ASSERT(!weakAfter.owner_before(sharedThisPtr));

            ASSERTV(destructorCount, 0 == destructorCount);
        }
        ASSERTV(destructorCount, 11 == destructorCount);
        destructorCount = 0;    // reset 'destructorCount' for next test.

        if (verbose) printf("\n\tBasic usage of 'const' most-derived type\n");
        {
            const ShareThisDerived *pThis =
                                        new ShareThisDerived(&destructorCount);
            ConstWeakPtr weakBefore = pThis->weak_from_this();
            ASSERT(weakBefore.expired());

            ConstSharedPtrDerived ptr(pThis);
            ASSERT(ptr.use_count() == 1);

            ConstWeakPtr weakAfter = pThis->weak_from_this();
            ASSERT(!weakAfter.expired());
            ASSERT(weakBefore.expired());

            ConstSharedPtr sharedThisPtr = ptr->shared_from_this();
            ASSERT(ptr.get() == sharedThisPtr.get());
            ASSERT(ptr.use_count() == 2);

            // Confirm that weak and shared pointer have the same rep object.
            ASSERT(!sharedThisPtr.owner_before(weakAfter));
            ASSERT(!weakAfter.owner_before(sharedThisPtr));

            ASSERTV(destructorCount, 0 == destructorCount);
        }
        ASSERTV(destructorCount, 11 == destructorCount);
        destructorCount = 0;    // reset 'destructorCount' for next test.

        if (verbose) printf("\n\tTest making a copy\n");
        {
            ShareThis *pThis = new ShareThis(&destructorCount);
            WeakPtr weakBefore = pThis->weak_from_this();
            ASSERT(weakBefore.expired());

            SharedPtr ptr(pThis);
            SharedPtr ptrCopy(ptr);
            ASSERT(ptr.use_count() == 2);

            WeakPtr weakAfter = pThis->weak_from_this();
            ASSERT(!weakAfter.expired());
            ASSERT(weakBefore.expired());

            SharedPtr sharedThisPtr = ptrCopy->shared_from_this();
            ASSERT(ptr.use_count() == 3);
            ASSERT(ptr.get() == sharedThisPtr.get());

            // Confirm that weak and shared pointer have the same rep object.
            ASSERT(!sharedThisPtr.owner_before(weakAfter));
            ASSERT(!weakAfter.owner_before(sharedThisPtr));

            ASSERTV(destructorCount, 0 == destructorCount);
        }
        ASSERTV(destructorCount, 1 == destructorCount);
        destructorCount = 0;    // reset 'destructorCount' for next test.

        if (verbose) printf("\n\tTest with BDE allocator\n");
        {
            ShareThis *pThis = new(ta) ShareThis(&destructorCount);
            WeakPtr weakBefore = pThis->weak_from_this();
            ASSERT(weakBefore.expired());

            SharedPtr ptr(pThis, &ta);
            ASSERT(ptr.use_count() == 1);

            WeakPtr weakAfter = pThis->weak_from_this();
            ASSERT(!weakAfter.expired());
            ASSERT(weakBefore.expired());

            SharedPtr sharedThisPtr = ptr->shared_from_this();
            ASSERT(ptr.get() == sharedThisPtr.get());
            ASSERT(ptr.use_count() == 2);

            // Confirm that weak and shared pointer have the same rep object.
            ASSERT(!sharedThisPtr.owner_before(weakAfter));
            ASSERT(!weakAfter.owner_before(sharedThisPtr));

            ASSERTV(destructorCount, 0 == destructorCount);
        }
        ASSERTV(destructorCount, 1 == destructorCount);
        destructorCount = 0;    // reset 'destructorCount' for next test.

        int callsToMyDeleter = 0;
        MyTestDeleter d1(&ta, &callsToMyDeleter);  // custom deleter for tests
                                                   // of deleter support.

        if (verbose) printf("\n\tTest with BDE allocator and deleter\n");
        {
            ShareThis *pThis = new(ta) ShareThis(&destructorCount);
            WeakPtr weakBefore = pThis->weak_from_this();
            ASSERT(weakBefore.expired());

            SharedPtr ptr(pThis, d1, &ta);
            ASSERT(ptr.use_count() == 1);

            WeakPtr weakAfter = pThis->weak_from_this();
            ASSERT(!weakAfter.expired());
            ASSERT(weakBefore.expired());

            SharedPtr sharedThisPtr = ptr->shared_from_this();
            ASSERT(ptr.get() == sharedThisPtr.get());
            ASSERT(ptr.use_count() == 2);

            // Confirm that weak and shared pointer have the same rep object.
            ASSERT(!sharedThisPtr.owner_before(weakAfter));
            ASSERT(!weakAfter.owner_before(sharedThisPtr));

            ASSERTV(destructorCount,  0 == destructorCount);
            ASSERTV(callsToMyDeleter, 0 == callsToMyDeleter);
        }
        ASSERTV(destructorCount,  1 == destructorCount);
        ASSERTV(callsToMyDeleter, 1 == callsToMyDeleter);
        destructorCount  = 0;   // reset 'destructorCount' for next test.
        callsToMyDeleter = 0;   // reset 'callsToMyDeleter' for next test.

        if (verbose) printf("\n\tTest with standard allocator and deleter\n");
        {
            ShareThis *pThis = new(ta) ShareThis(&destructorCount);
            WeakPtr weakBefore = pThis->weak_from_this();
            ASSERT(weakBefore.expired());

            bsltf::StdStatefulAllocator<TObj, false, false, false, false>
                                                                 stdAlloc(&ta);
            SharedPtr ptr(pThis, d1, stdAlloc);
            ASSERT(ptr.use_count() == 1);

            WeakPtr weakAfter = pThis->weak_from_this();
            ASSERT(!weakAfter.expired());
            ASSERT(weakBefore.expired());

            SharedPtr sharedThisPtr = ptr->shared_from_this();
            ASSERT(ptr.get() == sharedThisPtr.get());
            ASSERT(ptr.use_count() == 2);

            // Confirm that weak and shared pointer have the same rep object.
            ASSERT(!sharedThisPtr.owner_before(weakAfter));
            ASSERT(!weakAfter.owner_before(sharedThisPtr));

            ASSERTV(destructorCount,  0 == destructorCount);
            ASSERTV(callsToMyDeleter, 0 == callsToMyDeleter);
        }
        ASSERTV(destructorCount,  1 == destructorCount);
        ASSERTV(callsToMyDeleter, 1 == callsToMyDeleter);
        destructorCount  = 0;   // reset 'destructorCount' for next test.
        callsToMyDeleter = 0;   // reset 'callsToMyDeleter' for next test.

        if (verbose) printf("\n\tTest with in-place buffer\n");
        {
            SharedPtr ptr = bsl::make_shared<ShareThis>(&destructorCount);
            ASSERT(ptr.use_count() == 1);

            SharedPtr sharedThisPtr = ptr->shared_from_this();
            ASSERT(ptr.get() == sharedThisPtr.get());
            ASSERT(ptr.use_count() == 2);

            WeakPtr weakAfter = sharedThisPtr->weak_from_this();
            ASSERT(!weakAfter.expired());
            ASSERT(!weakAfter.owner_before(sharedThisPtr));
            ASSERT(!sharedThisPtr.owner_before(weakAfter));

            ASSERTV(destructorCount, 0 == destructorCount);
        }
        ASSERTV(destructorCount, 1 == destructorCount);
        destructorCount = 0;    // reset 'destructorCount' for next test.

        if (verbose) printf(
                          "\n\tTest with in-place buffer and BDE allocator\n");
        {
            SharedPtr ptr = bsl::allocate_shared<ShareThis>(&ta,
                                                            &destructorCount);
            ASSERT(ptr.use_count() == 1);

            SharedPtr sharedThisPtr = ptr->shared_from_this();
            ASSERT(ptr.get() == sharedThisPtr.get());
            ASSERT(ptr.use_count() == 2);

            WeakPtr weakAfter = sharedThisPtr->weak_from_this();
            ASSERT(!weakAfter.expired());
            ASSERT(!weakAfter.owner_before(sharedThisPtr));
            ASSERT(!sharedThisPtr.owner_before(weakAfter));

            ASSERTV(destructorCount, 0 == destructorCount);
        }
        ASSERTV(destructorCount, 1 == destructorCount);
        destructorCount = 0;    // reset 'destructorCount' for next test.

        if (verbose) printf("\n\tTest with 'ManagedPtr'\n");
        {
            ShareThis *pThis = new ShareThis(&destructorCount);
            WeakPtr weakBefore = pThis->weak_from_this();
            ASSERT(weakBefore.expired());

            bslma::ManagedPtr<ShareThis> managedPtr(pThis);
            SharedPtr ptr(managedPtr);
            ASSERT(ptr.use_count() == 1);

            WeakPtr weakAfter = pThis->weak_from_this();
            ASSERT(!weakAfter.expired());
            ASSERT(weakBefore.expired());

            SharedPtr sharedThisPtr = ptr->shared_from_this();
            ASSERT(ptr.get() == sharedThisPtr.get());
            ASSERT(ptr.use_count() == 2);

            // Confirm that weak and shared pointer have the same rep object.
            ASSERT(!sharedThisPtr.owner_before(weakAfter));
            ASSERT(!weakAfter.owner_before(sharedThisPtr));

            ASSERTV(destructorCount, 0 == destructorCount);
        }
        ASSERTV(destructorCount, 1 == destructorCount);
        destructorCount = 0;    // reset 'destructorCount' for next test.

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)

# if defined(BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
        if (verbose) printf("\n\tTest with 'auto_ptr'\n");
        {
            ShareThis *pThis = new ShareThis(&destructorCount);
            WeakPtr weakBefore = pThis->weak_from_this();
            ASSERT(weakBefore.expired());

            std::auto_ptr<ShareThis> autoPtr(pThis);
            SharedPtr ptr(autoPtr);

            WeakPtr weakAfter = pThis->weak_from_this();
            ASSERT(!weakAfter.expired());
            ASSERT(weakBefore.expired());

            ASSERT(ptr.use_count() == 1);
            SharedPtr sharedThisPtr = ptr->shared_from_this();
            ASSERT(ptr.get() == sharedThisPtr.get());
            ASSERT(ptr.use_count() == 2);

            // Confirm that weak and shared pointer have the same rep object.
            ASSERT(!sharedThisPtr.owner_before(weakAfter));
            ASSERT(!weakAfter.owner_before(sharedThisPtr));

            ASSERTV(destructorCount, 0 == destructorCount);
        }
        ASSERTV(destructorCount, 1 == destructorCount);
        destructorCount = 0;    // reset 'destructorCount' for next test.
# if defined(BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC)
#   pragma GCC diagnostic pop
# endif
#endif

        if (verbose) printf("\n\tTest with 'shared_ptr<void>'\n");
        {
            ShareThis *pThis = new ShareThis(&destructorCount);
            WeakPtr weakBefore = pThis->weak_from_this();
            ASSERT(weakBefore.expired());

            bsl::shared_ptr<void> voidPtr(pThis);

            WeakPtr weakAfter = pThis->weak_from_this();
            ASSERT(!weakAfter.expired());
            ASSERT(weakBefore.expired());

            SharedPtr sharedThisPtr = pThis->shared_from_this();
            ASSERT(voidPtr.get() == sharedThisPtr.get());
            ASSERT(voidPtr.use_count() == 2);

            // Confirm that weak and shared pointer have the same rep object.
            ASSERT(!sharedThisPtr.owner_before(weakAfter));
            ASSERT(!weakAfter.owner_before(sharedThisPtr));

            ASSERTV(destructorCount, 0 == destructorCount);
        }
        ASSERTV(destructorCount, 1 == destructorCount);
        destructorCount = 0;    // reset 'destructorCount' for next test.

        if (verbose) printf("\n\tTest with aliased 'shared_ptr'\n");
        {
            ShareThis *pThis = new ShareThis(&destructorCount);
            WeakPtr weakBefore = pThis->weak_from_this();
            ASSERT(weakBefore.expired());

            {
                int data = 0;
                bsl::shared_ptr<void> voidPtr(&data,
                                               bslstl::SharedPtrNilDeleter());
                SharedPtr ptr(voidPtr, pThis);
                ASSERT(ptr.use_count() == 2);

                WeakPtr weakAfter = ptr->weak_from_this();
                ASSERT( weakAfter.expired());
                ASSERT(weakBefore.expired());

#if defined(BDE_BUILD_TARGET_EXC)
                // Test exception thrown when alias not owned by a 'shared_ptr'
                bool caughtException = false;
                try {
                    SharedPtr sharedThisPtr = ptr->shared_from_this();
                    ASSERTV("Should have thrown a 'bad_weak_ptr'.", false);
                }
                catch(const bsl::bad_weak_ptr&) {
                    caughtException = true;
                }
                ASSERT(caughtException);
#endif

                ASSERTV(destructorCount, 0 == destructorCount);
            }
            ASSERTV(destructorCount, 0 == destructorCount);

            delete pThis;    // manual memory management safe if all tests pass
            ASSERTV(destructorCount, 1 == destructorCount);
        }
        ASSERTV(destructorCount, 1 == destructorCount);
        destructorCount = 0;    // reset 'destructorCount' for next test.

        if (verbose) printf("\n\tTest with aliased 'managed_ptr'\n");
        {
            ShareThis *pThis = new ShareThis(&destructorCount);
            WeakPtr weakThisBefore = pThis->weak_from_this();
            ASSERT(weakThisBefore.expired());

            ShareThis *pThat = new ShareThis(&destructorCount);
            WeakPtr weakThatBefore = pThis->weak_from_this();
            ASSERT(weakThatBefore.expired());

            // Create aliased managed pointer, which owns 'pThis' but points to
            // 'pThat'.
            bslma::ManagedPtr<ShareThis> managedPtr(pThis);
            managedPtr.loadAlias(managedPtr, pThat);

            SharedPtr ptr(managedPtr);
            ASSERT(ptr.use_count() == 1);
            ASSERT(ptr.get() == pThat);

            // 'ptr' owns 'pThis' but points to 'pThat'.  The enable-shared
            // base object has a weak reference to 'pThis', and should alias
            // 'pThis'.
            ASSERT( weakThisBefore.expired());
            ASSERT( weakThatBefore.expired());

            weakThisBefore = pThis->weak_from_this();
            weakThatBefore = pThat->weak_from_this();

            ASSERT(!weakThisBefore.expired());
            ASSERT( weakThatBefore.expired());

            WeakPtr weakAfter = pThis->weak_from_this();
            ASSERT(!weakAfter.expired());
            ASSERT(!weakThisBefore.expired());
            ASSERT( weakThatBefore.expired());

            // Note that we cannot easily make a "from this" 'shared_ptr' as
            // the aliased pointer is not under shared ownership, and the
            // managed pointer is not directly exposed after the 'ptr' takes
            // ownership.

            SharedPtr sharedThisPtr = ptr->weak_from_this().lock();
            ASSERT(!sharedThisPtr);

            delete pThat;  // manual memory management is safe if driver passes
            ASSERTV(destructorCount, 1 == destructorCount);
        }
        ASSERTV(destructorCount, 2 == destructorCount);
        destructorCount = 0;    // reset 'destructorCount' for next test.

        if (verbose) printf(
                      "\n\tTest 'managed_ptr' managing aliased 'shared_ptr\n");
        {
            ShareThis *pThis = new ShareThis(&destructorCount);
            WeakPtr weakBefore = pThis->weak_from_this();
            ASSERT(weakBefore.expired());

            {
                int data = 0;
                bsl::shared_ptr<void> voidPtr(&data,
                                               bslstl::SharedPtrNilDeleter());
                SharedPtr ptr(voidPtr, pThis);
                ASSERT(ptr.use_count() == 2);

                // Pass the aliased pointer to a ManagedPtr, and verify that no
                // 'enable_shared_from_this' dance occurs when passed again to
                // a 'shared_ptr' constructor.

                bslma::ManagedPtr<ShareThis> managedPtr = ptr.managedPtr();
                ASSERTV(ptr.use_count(), 3 == ptr.use_count());
                SharedPtr finalAlias(ptr);
                ASSERTV(finalAlias.use_count(), 4 == finalAlias.use_count());

                WeakPtr weakAfter = finalAlias->weak_from_this();
                ASSERT( weakAfter.expired());
                ASSERT(weakBefore.expired());

#if defined(BDE_BUILD_TARGET_EXC)
                // Test exception thrown when alias not owned by a 'shared_ptr'
                bool caughtException = false;
                try {
                    SharedPtr sharedThisPtr = finalAlias->shared_from_this();
                    ASSERTV("Should have thrown a 'bad_weak_ptr'.", false);
                }
                catch(const bsl::bad_weak_ptr&) {
                    caughtException = true;
                }
                ASSERT(caughtException);
#endif

                ASSERTV(destructorCount, 0 == destructorCount);
            }
            ASSERTV(destructorCount, 0 == destructorCount);

            delete pThis;    // manual memory management safe if all tests pass
            ASSERTV(destructorCount, 1 == destructorCount);
        }
        ASSERTV(destructorCount, 1 == destructorCount);
        destructorCount = 0;    // reset 'destructorCount' for next test.

        if (verbose) printf("\nTesting awkward lifetime concerns"
                            "\n---------------------------------\n");

        if (verbose) printf("\n\tTest extended lifetimes\n");
        {
            struct LocalFactory {
                static SharedPtr make(int *destructorCount)
                    // Return (by value) a 'SharedPtr' object that used the
                    // specified 'destructorCount' to report the eventual
                    // destruction of the shared object.
                {
                    ShareThis *data_p = new ShareThis(destructorCount);
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

        if (verbose) printf("\n\tTest multiple 'shared_ptr' groups\n");
        {
            SharedPtr ptr(new (ta) ShareThis(&destructorCount), d1, &ta);
            ASSERT(ptr.use_count() == 1);
            SharedPtr sharedThisPtr = ptr->shared_from_this();
            ASSERT(ptr.get() == sharedThisPtr.get());
            ASSERT(ptr.use_count() == 2);

            // confirm that both shared pointers have the same rep object
            ASSERT(!ptr.owner_before(sharedThisPtr) &&
                   !sharedThisPtr.owner_before(ptr));

            ShareThis *const OUTER_P = ptr.get();
            {
                SharedPtr innerPtr(OUTER_P, bslstl::SharedPtrNilDeleter());
                bslstl::SharedPtrNilDeleter *delPtr =
                      bsl::get_deleter<bslstl::SharedPtrNilDeleter>(innerPtr);
                ASSERT(delPtr);

                SharedPtr sharedInnerPtr = innerPtr->shared_from_this();

                // confirm that shared pointers do not have the same rep object
                ASSERT(innerPtr.owner_before(sharedInnerPtr) ||
                       sharedInnerPtr.owner_before(innerPtr));

                bslstl::SharedPtrNilDeleter *innerDelPtr =
                 bsl::get_deleter<bslstl::SharedPtrNilDeleter>(sharedInnerPtr);

                ASSERTV(delPtr, innerDelPtr, delPtr != innerDelPtr);
            }
            SharedPtr latterPtr = ptr->shared_from_this();

            // confirm that both shared pointers have the same rep object
            ASSERT(!ptr.owner_before(latterPtr) &&
                   !latterPtr.owner_before(ptr));

            ASSERTV(destructorCount,  0 == destructorCount);
            ASSERTV(callsToMyDeleter, 0 == callsToMyDeleter);
        }
        ASSERTV(destructorCount,  1 == destructorCount);
        ASSERTV(callsToMyDeleter, 1 == callsToMyDeleter);
        destructorCount  = 0;   // reset 'destructorCount' for next test.
        callsToMyDeleter = 0;   // reset 'callsToMyDeleter' for next test.

        if (verbose) printf("\n\tTest additional shares without ownership\n");
        {
            SharedPtr ptr(new (ta) ShareThis(&destructorCount), d1, &ta);
            SharedPtr sharedThisPtr = ptr->shared_from_this();

            // confirm that both shared pointers have the same rep object
            ASSERT(!ptr.owner_before(sharedThisPtr) &&
                   !sharedThisPtr.owner_before(ptr));
            ASSERT(ptr.get() == sharedThisPtr.get());
            ASSERT(ptr.use_count() == 2);

            ShareThis *const OUTER_P = ptr.get();
            {
                SharedPtr innerPtr(SharedPtr(), OUTER_P);
                ASSERT(0 == innerPtr.use_count());

                WeakPtr innerWeakThis = innerPtr->weak_from_this();
                WeakPtr innerWeak     = innerPtr;

                ASSERT(!innerWeakThis.expired());
                ASSERT(innerWeak.expired());

                ASSERT(innerWeak.owner_before(innerWeakThis) ||
                       innerWeakThis.owner_before(innerWeak));

                ASSERT(!innerWeakThis.owner_before(sharedThisPtr) &&
                       !sharedThisPtr.owner_before(innerWeakThis));

                SharedPtr sharedInnerPtr = innerPtr->shared_from_this();

                // confirm that shared pointers do not have the same rep object
                ASSERT(innerPtr.owner_before(sharedInnerPtr) ||
                       sharedInnerPtr.owner_before(innerPtr));
            }
            SharedPtr latterPtr = ptr->shared_from_this();

            // confirm that both shared pointers have the same rep object
            ASSERT(!ptr.owner_before(latterPtr) &&
                   !latterPtr.owner_before(ptr));

            ASSERTV(destructorCount,  0 == destructorCount);
            ASSERTV(callsToMyDeleter, 0 == callsToMyDeleter);
        }
        ASSERTV(destructorCount,  1 == destructorCount);
        ASSERTV(callsToMyDeleter, 1 == callsToMyDeleter);
        destructorCount  = 0;   // reset 'destructorCount' for next test.
        callsToMyDeleter = 0;   // reset 'callsToMyDeleter' for next test.


        if (verbose) printf("\n\tExtended 'shared_ptr' groups test\n");
        {
            // First confirm that weak pointers rebind after expiring, before
            // repeating a similar test with 'shared_from_this' to manage the
            // lifetime of the shared object itself.

            if (veryVeryVerbose) printf(
                        "\t\tFirst check that weak pointers bind correctly\n");

            ShareThis *pThis = new (ta) ShareThis(&destructorCount);
            WeakPtr weakThis = pThis->weak_from_this();
            ASSERT(weakThis.expired());

            WeakPtr weak1, weak2;
            {
                SharedPtr shared1(pThis, bslstl::SharedPtrNilDeleter());
                weak1 = shared1->weak_from_this();
                ASSERT(!weak1.expired());

                // Confirm that 'weak1' and 'weakThis' use different reps
                ASSERT(weak1.owner_before(weakThis) ||
                       weakThis.owner_before(weak1));

                {
                    SharedPtr shared2(pThis, bslstl::SharedPtrNilDeleter());
                    weak2 = shared2->weak_from_this();
                    ASSERT(!weak1.expired());
                    ASSERT(!weak2.expired());

                    // Confirm that both weak pointers use the same rep
                    ASSERT(!weak1.owner_before(weak2) &&
                           !weak2.owner_before(weak1));

                    // Confirm that 'shared2' uses a different rep than 'weak2'
                    ASSERT(weak2.owner_before(shared2) ||
                           shared2.owner_before(weak2));
                }
            }

            ASSERT(weakThis.expired());
            ASSERT(weak1.expired());
            ASSERT(weak2.expired());

            // Repeat, to see that the weak pointer rebinds again, now that all
            // weak pointers have expired

            if (veryVeryVerbose) printf(
                      "\t\tThen check that weak pointers re-bind correctly\n");
            {
                // Verify assumption on entry that 'weak1' and 'weak2' wrap the
                // same rep object:
                ASSERT(!weak1.owner_before(weak2) &&
                       !weak2.owner_before(weak1));

                SharedPtr shared1(pThis, bslstl::SharedPtrNilDeleter());
                weak1 = shared1->weak_from_this();
                ASSERT(!weak1.expired());
                ASSERT( weak2.expired());

                // Confirm that 'weak1' and 'weakThis' use different reps, and
                // that 'weak1' and 'weak2' use different reps
                ASSERT(weak1.owner_before(weakThis) ||
                       weakThis.owner_before(weak1));

                ASSERT(weak1.owner_before(weak2) ||
                       weak2.owner_before(weak1));

            }

            ASSERTV(destructorCount,  0 == destructorCount);
            ASSERTV(callsToMyDeleter, 0 == callsToMyDeleter);

            if (veryVeryVerbose) printf(
                "\t\tNext, check that shared_ptrs hand ownership correctly\n");
            {
                SharedPtr shared1(pThis, bslstl::SharedPtrNilDeleter());
                weak1 = shared1->weak_from_this();
                ASSERT(!weak1.expired());

                // Confirm that 'shared1' uses the same rep as 'weak1'
                ASSERT(!weak1.owner_before(shared1) &&
                       !shared1.owner_before(weak1));

                // Confirm that 'weak1' and 'weakThis' use different reps
                ASSERT(weak1.owner_before(weakThis) ||
                       weakThis.owner_before(weak1));

                {
                    SharedPtr shared2(pThis, d1, &ta);
                    weak2 = shared2->weak_from_this();
                    ASSERT(!weak1.expired());
                    ASSERT(!weak2.expired());

                    // Confirm that both weak pointers use the same rep
                    ASSERT(!weak1.owner_before(weak2) &&
                           !weak2.owner_before(weak1));

                    // Confirm that 'shared2' uses a different rep than 'weak2'
                    ASSERT(weak2.owner_before(shared2) ||
                           shared2.owner_before(weak2));
                }

                // Note that '*pThis' is now destroyed, so should *not* attempt
                // to call its methods through any of the smart pointers now.
                ASSERTV(destructorCount,  1 == destructorCount);
                ASSERTV(callsToMyDeleter, 1 == callsToMyDeleter);

                if (veryVeryVerbose) printf(
                        "\t\tCheck we can still lock the non-expired weak1\n");
                ASSERT(!weak1.expired());
                ASSERT(1 == shared1.use_count());
                WeakPtr weakAfter(shared1);
                {
                    SharedPtr sharedAfter = weakAfter.lock();
                    ASSERT(sharedAfter);
                }
                SharedPtr locked = weak1.lock();
                SharedPtr pDeadPointer(weak1);         // can still lock, but
                ASSERT(pDeadPointer.get() == pThis);   // target is destroyed
            }

            ASSERTV(destructorCount,  1 == destructorCount);
            ASSERTV(callsToMyDeleter, 1 == callsToMyDeleter);

            ASSERT(weakThis.expired());
            ASSERT(weak1.expired());
            ASSERT(weak2.expired());
        }
        ASSERTV(destructorCount,  1 == destructorCount);
        ASSERTV(callsToMyDeleter, 1 == callsToMyDeleter);
        destructorCount  = 0;   // reset 'destructorCount' for next test.
        callsToMyDeleter = 0;   // reset 'callsToMyDeleter' for next test.

        if (verbose) printf("\nTesting 'reset' overloads"
                            "\n-------------------------\n");

        if (verbose) printf("\n\tTest basic 'reset'\n");
        {
            SharedPtr ptr;
            ASSERT(0 == ptr.use_count());
            ShareThis *pThis = new ShareThis(&destructorCount);
            WeakPtr weakBefore = pThis->weak_from_this();
            ASSERT(weakBefore.expired());

            ptr.reset(pThis);
            {
            WeakPtr weakAfter = pThis->weak_from_this();
            ASSERT(!weakAfter.expired());
            ASSERT(weakBefore.expired());

            SharedPtr sharedThisPtr = ptr->shared_from_this();
            ASSERT(ptr.get() == sharedThisPtr.get());
            ASSERT(ptr.use_count() == 2);

            // Confirm that weak and shared pointer have the same rep object.
            ASSERT(!sharedThisPtr.owner_before(weakAfter));
            ASSERT(!weakAfter.owner_before(sharedThisPtr));

            ASSERTV(destructorCount, 0 == destructorCount);
            }
            ASSERTV(destructorCount, 0 == destructorCount);
        }
        ASSERTV(destructorCount, 1 == destructorCount);
        destructorCount = 0;    // reset 'destructorCount' for next test.

        if (verbose) printf("\n\tTest 'reset' with derived class\n");
        {
            SharedPtr ptr;
            ASSERT(0 == ptr.use_count());
            ShareThis *pThis = new ShareThisDerived(&destructorCount);
            WeakPtr weakBefore = pThis->weak_from_this();
            ASSERT(weakBefore.expired());

            ptr.reset(pThis);
            {
                WeakPtr weakAfter = pThis->weak_from_this();
                ASSERT(!weakAfter.expired());
                ASSERT(weakBefore.expired());

                SharedPtr sharedThisPtr = ptr->shared_from_this();
                ASSERT(ptr.get() == sharedThisPtr.get());
                ASSERT(ptr.use_count() == 2);

                // Confirm that weak and shared pointer have same rep object.
                ASSERT(!sharedThisPtr.owner_before(weakAfter));
                ASSERT(!weakAfter.owner_before(sharedThisPtr));

                ASSERTV(destructorCount, 0 == destructorCount);
            }
            ASSERTV(destructorCount, 0 == destructorCount);
        }
        ASSERTV(destructorCount, 11 == destructorCount);
        destructorCount = 0;    // reset 'destructorCount' for next test.

        if (verbose) printf("\n\tTest 'reset' with allocator\n");
        {
            SharedPtr ptr;
            ASSERT(0 == ptr.use_count());
            ShareThis *pThis = new(ta) ShareThis(&destructorCount);
            WeakPtr weakBefore = pThis->weak_from_this();
            ASSERT(weakBefore.expired());

            ptr.reset(pThis, &ta);
            {
                WeakPtr weakAfter = pThis->weak_from_this();
                ASSERT(!weakAfter.expired());
                ASSERT(weakBefore.expired());

                SharedPtr sharedThisPtr = ptr->shared_from_this();
                ASSERT(ptr.get() == sharedThisPtr.get());
                ASSERT(ptr.use_count() == 2);

                // Confirm that weak and shared pointer have same rep object.
                ASSERT(!sharedThisPtr.owner_before(weakAfter));
                ASSERT(!weakAfter.owner_before(sharedThisPtr));

                ASSERTV(destructorCount, 0 == destructorCount);
            }
            ASSERTV(destructorCount, 0 == destructorCount);
        }
        ASSERTV(destructorCount, 1 == destructorCount);
        destructorCount = 0;    // reset 'destructorCount' for next test.

        if (verbose) printf("\n\tTest 'reset' with deleter\n");
        {
            SharedPtr ptr;
            ASSERT(0 == ptr.use_count());
            ShareThis *pThis = new(ta) ShareThis(&destructorCount);
            WeakPtr weakBefore = pThis->weak_from_this();
            ASSERT(weakBefore.expired());

            ptr.reset(pThis, d1);
            {
                WeakPtr weakAfter = pThis->weak_from_this();
                ASSERT(!weakAfter.expired());
                ASSERT(weakBefore.expired());

                SharedPtr sharedThisPtr = ptr->shared_from_this();
                ASSERT(ptr.get() == sharedThisPtr.get());
                ASSERT(ptr.use_count() == 2);

                // Confirm that weak and shared pointer have same rep object.
                ASSERT(!sharedThisPtr.owner_before(weakAfter));
                ASSERT(!weakAfter.owner_before(sharedThisPtr));

                ASSERTV(destructorCount,  0 == destructorCount);
                ASSERTV(callsToMyDeleter, 0 == callsToMyDeleter);
            }
            ASSERTV(destructorCount,  0 == destructorCount);
            ASSERTV(callsToMyDeleter, 0 == callsToMyDeleter);
        }
        ASSERTV(destructorCount,  1 == destructorCount);
        ASSERTV(callsToMyDeleter, 1 == callsToMyDeleter);
        destructorCount  = 0;   // reset 'destructorCount' for next test.
        callsToMyDeleter = 0;   // reset 'callsToMyDeleter' for next test.

        if (verbose) printf("\n\tTest 'reset' with deleter and allocator\n");
        {
            SharedPtr ptr;
            ASSERT(0 == ptr.use_count());
            ShareThis *pThis = new(ta) ShareThis(&destructorCount);
            WeakPtr weakBefore = pThis->weak_from_this();
            ASSERT(weakBefore.expired());

            ptr.reset(pThis, d1, &ta);
            {
                WeakPtr weakAfter = pThis->weak_from_this();
                ASSERT(!weakAfter.expired());
                ASSERT(weakBefore.expired());

                SharedPtr sharedThisPtr = ptr->shared_from_this();
                ASSERT(ptr.get() == sharedThisPtr.get());
                ASSERT(ptr.use_count() == 2);

                // Confirm that weak and shared pointer have same rep object.
                ASSERT(!sharedThisPtr.owner_before(weakAfter));
                ASSERT(!weakAfter.owner_before(sharedThisPtr));

                ASSERTV(destructorCount,  0 == destructorCount);
                ASSERTV(callsToMyDeleter, 0 == callsToMyDeleter);
            }
            ASSERTV(destructorCount,  0 == destructorCount);
            ASSERTV(callsToMyDeleter, 0 == callsToMyDeleter);
        }
        ASSERTV(destructorCount,  1 == destructorCount);
        ASSERTV(callsToMyDeleter, 1 == callsToMyDeleter);
        destructorCount  = 0;   // reset 'destructorCount' for next test.
        callsToMyDeleter = 0;   // reset 'callsToMyDeleter' for next test.

        if (verbose) printf(
                     "\n\tTest 'reset' with standard allocator and deleter\n");
        {
            SharedPtr ptr;
            ASSERT(0 == ptr.use_count());
            ShareThis *pThis = new(ta) ShareThis(&destructorCount);
            WeakPtr weakBefore = pThis->weak_from_this();
            ASSERT(weakBefore.expired());

            bsltf::StdStatefulAllocator<TObj, false, false, false, false>
                                                                 stdAlloc(&ta);
            ptr.reset(pThis, d1, stdAlloc);
            {
                WeakPtr weakAfter = pThis->weak_from_this();
                ASSERT(!weakAfter.expired());
                ASSERT(weakBefore.expired());

                SharedPtr sharedThisPtr = ptr->shared_from_this();
                ASSERT(ptr.get() == sharedThisPtr.get());
                ASSERT(ptr.use_count() == 2);

                // Confirm that weak and shared pointer have same rep object.
                ASSERT(!sharedThisPtr.owner_before(weakAfter));
                ASSERT(!weakAfter.owner_before(sharedThisPtr));

                ASSERTV(destructorCount,  0 == destructorCount);
                ASSERTV(callsToMyDeleter, 0 == callsToMyDeleter);
            }
            ASSERTV(destructorCount,  0 == destructorCount);
            ASSERTV(callsToMyDeleter, 0 == callsToMyDeleter);
        }
        ASSERTV(destructorCount,  1 == destructorCount);
        ASSERTV(callsToMyDeleter, 1 == callsToMyDeleter);
        destructorCount  = 0;   // reset 'destructorCount' for next test.
        callsToMyDeleter = 0;   // reset 'callsToMyDeleter' for next test.

        if (verbose) printf("\n\tTest 'createInplace'\n");
        {
            SharedPtr ptr;
            ASSERT(0 == ptr.use_count());

            ptr.createInplace(&ta, &destructorCount);
            {
                WeakPtr weakAfter = ptr->weak_from_this();
                ASSERT(!weakAfter.expired());

                SharedPtr sharedThisPtr = ptr->shared_from_this();
                ASSERT(ptr.get() == sharedThisPtr.get());
                ASSERT(ptr.use_count() == 2);

                // Confirm weak and shared pointer have the same rep object.
                ASSERT(!sharedThisPtr.owner_before(weakAfter));
                ASSERT(!weakAfter.owner_before(sharedThisPtr));

                ASSERTV(destructorCount, 0 == destructorCount);
            }
            ASSERTV(destructorCount, 0 == destructorCount);
        }
        ASSERTV(destructorCount, 1 == destructorCount);
        destructorCount = 0;    // reset 'destructorCount' for next test.

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

        if (verbose) printf("\n\tTest basic 'load'\n");
        {
            SharedPtr ptr;
            ASSERT(0 == ptr.use_count());
            ShareThis *pThis = new ShareThis(&destructorCount);
            WeakPtr weakBefore = pThis->weak_from_this();
            ASSERT(weakBefore.expired());

            ptr.load(pThis);
            {
            WeakPtr weakAfter = pThis->weak_from_this();
            ASSERT(!weakAfter.expired());
            ASSERT(weakBefore.expired());

            SharedPtr sharedThisPtr = ptr->shared_from_this();
            ASSERT(ptr.get() == sharedThisPtr.get());
            ASSERT(ptr.use_count() == 2);

            // Confirm that weak and shared pointer have the same rep object.
            ASSERT(!sharedThisPtr.owner_before(weakAfter));
            ASSERT(!weakAfter.owner_before(sharedThisPtr));

            ASSERTV(destructorCount, 0 == destructorCount);
            }
            ASSERTV(destructorCount, 0 == destructorCount);
        }
        ASSERTV(destructorCount, 1 == destructorCount);
        destructorCount = 0;    // reset 'destructorCount' for next test.

        if (verbose) printf("\n\tTest 'load' with derived class\n");
        {
            SharedPtr ptr;
            ASSERT(0 == ptr.use_count());
            ShareThis *pThis = new ShareThisDerived(&destructorCount);
            WeakPtr weakBefore = pThis->weak_from_this();
            ASSERT(weakBefore.expired());

            ptr.load(pThis);
            {
                WeakPtr weakAfter = pThis->weak_from_this();
                ASSERT(!weakAfter.expired());
                ASSERT(weakBefore.expired());

                SharedPtr sharedThisPtr = ptr->shared_from_this();
                ASSERT(ptr.get() == sharedThisPtr.get());
                ASSERT(ptr.use_count() == 2);

                // Confirm that weak and shared pointer have same rep object.
                ASSERT(!sharedThisPtr.owner_before(weakAfter));
                ASSERT(!weakAfter.owner_before(sharedThisPtr));

                ASSERTV(destructorCount, 0 == destructorCount);
            }
            ASSERTV(destructorCount, 0 == destructorCount);
        }
        ASSERTV(destructorCount, 11 == destructorCount);
        destructorCount = 0;    // reset 'destructorCount' for next test.

        if (verbose) printf("\n\tTest 'load' with allocator\n");
        {
            SharedPtr ptr;
            ASSERT(0 == ptr.use_count());
            ShareThis *pThis = new(ta) ShareThis(&destructorCount);
            WeakPtr weakBefore = pThis->weak_from_this();
            ASSERT(weakBefore.expired());

            ptr.load(pThis, &ta);
            {
                WeakPtr weakAfter = pThis->weak_from_this();
                ASSERT(!weakAfter.expired());
                ASSERT(weakBefore.expired());

                SharedPtr sharedThisPtr = ptr->shared_from_this();
                ASSERT(ptr.get() == sharedThisPtr.get());
                ASSERT(ptr.use_count() == 2);

                // Confirm that weak and shared pointer have same rep object.
                ASSERT(!sharedThisPtr.owner_before(weakAfter));
                ASSERT(!weakAfter.owner_before(sharedThisPtr));

                ASSERTV(destructorCount, 0 == destructorCount);
            }
            ASSERTV(destructorCount, 0 == destructorCount);
        }
        ASSERTV(destructorCount, 1 == destructorCount);
        destructorCount = 0;    // reset 'destructorCount' for next test.

        if (verbose) printf("\n\tTest 'load' with deleter and allocator\n");
        {
            SharedPtr ptr;
            ASSERT(0 == ptr.use_count());
            ShareThis *pThis = new(ta) ShareThis(&destructorCount);
            WeakPtr weakBefore = pThis->weak_from_this();
            ASSERT(weakBefore.expired());

            ptr.load(pThis, d1, &ta);
            {
                WeakPtr weakAfter = pThis->weak_from_this();
                ASSERT(!weakAfter.expired());
                ASSERT(weakBefore.expired());

                SharedPtr sharedThisPtr = ptr->shared_from_this();
                ASSERT(ptr.get() == sharedThisPtr.get());
                ASSERT(ptr.use_count() == 2);

                // Confirm that weak and shared pointer have same rep object.
                ASSERT(!sharedThisPtr.owner_before(weakAfter));
                ASSERT(!weakAfter.owner_before(sharedThisPtr));

                ASSERTV(destructorCount,  0 == destructorCount);
                ASSERTV(callsToMyDeleter, 0 == callsToMyDeleter);
            }
            ASSERTV(destructorCount,  0 == destructorCount);
            ASSERTV(callsToMyDeleter, 0 == callsToMyDeleter);
        }
        ASSERTV(destructorCount,  1 == destructorCount);
        ASSERTV(callsToMyDeleter, 1 == callsToMyDeleter);
        destructorCount  = 0;   // reset 'destructorCount' for next test.
        callsToMyDeleter = 0;   // reset 'callsToMyDeleter' for next test.
#endif // BDE_OMIT_INTERNAL_DEPRECATED
      } break;
      case 34: {
        // --------------------------------------------------------------------
        // TESTING 'allocate_shared<T>(A *, ...)'
        //   This test is concerned with the 'allocate_shared' free-function
        //   that takes BDE allocators, as an extension to the standard.
        //   Concern for the overloads taking standard-conforming allocators is
        //   handled by the immediately preceding test case.
        //
        // Concerns:
        //: 1.  'allocate_shared' returns a 'shared_ptr' pointing to a new
        //:     object of the specified 'T' type, where 'T' must be a complete
        //:     type.
        //: 2.  The supplied allocator is used to create the shared rep holding
        //:     the ref-counts, deleter, and shared object.
        //: 3.  There is only one allocation to create the shared object,
        //:     unless the shared object performs additional allocations.
        //: 4.  The appropriate constructor for 'T' is called, forwarding all
        //:     of the supplied arguments to that constructor, in the supplied
        //:     order.
        //: 5.  An allocator for 'T' can be supplied as an argument if 'T'
        //:     supports allocators.
        //: 6   The leading allocator argument is *not* passed to the shared
        //:     object, even if it uses BDE allocators.
        //: 7.  The reference count of the 'shared_ptr' object shall be 1.
        //: 8.  No memory is leaked if the constructor of 'T' throws an
        //:     exception.
        //: 9.  The default allocator should not be used, unless called by the
        //:     constructor of the shared object.
        //: 10. The global allocator should not be used, unless called by the
        //:     constructor of the shared object.
        //: 11. 'allocate_shared<POD-TYPE> passed no arguments other than the
        //:     allocator value-initializes the shared object.
        //: 12  'allocate_shared<const TYPE>' compiles and respects all the
        //:      concerns above.
        //
        // Plan:
        //: 1. Install a test allocator as the default, to verify that the
        //:    default is never used.
        //:
        //: 2. The (lack of) use of the global allocator is tested as a global
        //:    test concern.
        //:
        //: 3. Create a test allocator object to observe allocation by the
        //:    'allocate_shared' calls.
        //:
        //: 4. For each of the 15 overloads, taking 0-14 arguments plus a
        //:    leading allocator:
        //:    4.1. Install an allocator monitor on the default, to verify that
        //:         it is not used.
        //:    4.2  Create a 'shared_ptr' using the 'allocate_shared' function.
        //:         passing '&ta' as the (first) allocator argument.
        //:    4.3  verify exactly one allocation occurred using 'ta', implying
        //:         the shared object is in the same allocated region as the
        //;         reference counts.
        //:    4.4  verify the shared object pointed to by the returned shared
        //:         pointer has the expected value.
        //:    4.5  verify the shared pointer has a reference count of 1
        //:    4.6  allow the shared pointer to drop out of scope, and verify
        //:         that the shared object is destroyed, and that 'ta' observes
        //:         a single de-allocation.
        //   Call all 15 overloads for a different number of arguments, and
        //   provide each an instrumented 'bslma::TestAllocator'
        //   and supply it with the
        //   appropriate arguments and an instrumented test allocator.  Then
        //   verify that only one allocation is performed, and the object
        //   created inside the representation is initialized using the
        //   arguments supplied,.
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

        bslma::TestAllocator *da = dynamic_cast<bslma::TestAllocator *>(
                                                  bslma::Default::allocator());
        ASSERT(0 != da);

        bslma::TestAllocator ta("allocate_shared", veryVeryVeryVerbose);
        bslma::TestAllocator oa("shared object alloctor", veryVeryVeryVerbose);

        if (verbose) printf("\nTesting 'int' with no arguments"
                            "\n-------------------------------\n");

        bsls::Types::Int64 numAllocations = ta.numAllocations();
        bsls::Types::Int64 numDeallocations = ta.numDeallocations();
        {
            bslma::TestAllocatorMonitor dam(da);

            {
                bsl::shared_ptr<int> x = bsl::allocate_shared<int>(&ta);
                ASSERT(++numAllocations == ta.numAllocations());
                ASSERT(0 == *x);
                ASSERT(1 == x.use_count());
            }
            ASSERT(++numDeallocations == ta.numDeallocations());

            ASSERT(dam.isMaxSame());
        }

        if (verbose) printf("\nTesting 'int' with a value"
                            "\n--------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bslma::TestAllocatorMonitor dam(da);

            {
                bsl::shared_ptr<int> x = bsl::allocate_shared<int>(&ta, 42);
                ASSERT(++numAllocations == ta.numAllocations());
                ASSERT(42 == *x);
                ASSERT(1 == x.use_count());
            }
            ASSERT(++numDeallocations == ta.numDeallocations());

            ASSERT(dam.isMaxSame());
        }

        if (verbose) printf("\nTesting const int' with no arguments"
                            "\n-------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bslma::TestAllocatorMonitor dam(da);

            {
                bsl::shared_ptr<const int> x =
                                          bsl::allocate_shared<const int>(&ta);
                ASSERT(++numAllocations == ta.numAllocations());
                ASSERTV(*x, 0 == *x);
                ASSERTV(x.use_count(), 1 == x.use_count());
            }
            ASSERT(++numDeallocations == ta.numDeallocations());

            ASSERT(dam.isMaxSame());
        }

        if (verbose) printf("\nTesting const int' with a value"
                            "\n-------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP = MyInplaceTestObject();

            bslma::TestAllocatorMonitor dam(da);

            {
                bsl::shared_ptr<const int> x =
                                      bsl::allocate_shared<const int>(&ta, 13);
                ASSERT(++numAllocations == ta.numAllocations());
                ASSERT(13 == *x);
                ASSERT(1 == x.use_count());
            }
            ASSERT(++numDeallocations == ta.numDeallocations());

            ASSERT(dam.isMaxSame());
        }

        if (verbose) printf("\nTesting deliberately engineered evil type"
                            "\n-----------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bslma::TestAllocatorMonitor dam(da);

            {
                bsl::shared_ptr<const MostEvilTestType> x =
                         bsl::allocate_shared<const MostEvilTestType>(&ta, 13);
                ASSERT(++numAllocations == ta.numAllocations());
                ASSERT(13 == x->data());
                ASSERT(1 == x.use_count());
            }
            ASSERT(++numDeallocations == ta.numDeallocations());

            ASSERT(dam.isMaxSame());
        }

        if (verbose) printf("\nTesting aggregate type"
                            "\n----------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bslma::TestAllocatorMonitor dam(da);

            {
                bsl::shared_ptr<const MyAggregate> x =
                                  bsl::allocate_shared<const MyAggregate>(&ta);
                ASSERT(++numAllocations == ta.numAllocations());
                ASSERTV(x->d_first,    0 == x->d_first);
                ASSERTV(x->d_second,   0 == x->d_second);
                ASSERTV(x.use_count(), 1 == x.use_count());
            }
            ASSERT(++numDeallocations == ta.numDeallocations());

            ASSERT(dam.isMaxSame());
        }

#if defined(BSLSTL_SHAREDPTR_ALLOCATE_SHARED_USING_BRACE_LIST)
        // This branch should never expand, but can be used to demonstrate that
        // 'allocate_shared' uses the correct form of new operator syntax.  If
        // a brace-list were chosen instead, this #if branch would compile
        // correctly and the tests should pass.  However, the C++ standard (as
        // of early-2016 working paper) requires that these two checks fail to
        // compile.  There are open Library Working Group isses in this area
        // though.

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bslma::TestAllocatorMonitor dam(da);

            {
                bsl::shared_ptr<const MyAggregate> x =
                              bsl::allocate_shared<const MyAggregate>(&ta, 13);
                ASSERT(++numAllocations == ta.numAllocations());
                ASSERT(13 == x->d_first);
                ASSERT( 0 == x->d_second);
                ASSERT( 1 == x.use_count());
            }
            ASSERT(++numDeallocations == ta.numDeallocations());

            ASSERT(dam.isMaxSame());
        }

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bslma::TestAllocatorMonitor dam(da);

            {
                bsl::shared_ptr<const MyAggregate> x =
                         bsl::allocate_shared<const MyAggregate>(&ta, 42, 8);
                ASSERT(++numAllocations == ta.numAllocations());
                ASSERT(42 == x->d_first);
                ASSERT( 8 == x->d_second);
                ASSERT( 1 == x.use_count());
            }
            ASSERT(++numDeallocations == ta.numDeallocations());

            ASSERT(dam.isMaxSame());
        }
#else
        // If 'allocate_shared' incorrectly used brace-initialization to
        // construct the 'new' object, then this #if branch would fail to
        // compile, at least prior to C++17.
        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bslma::TestAllocatorMonitor dam(da);

            MyAggregate a1 = { 13, 42 };
            {
                bsl::shared_ptr<const MyAggregate> x =
                              bsl::allocate_shared<const MyAggregate>(&ta, a1);
                ASSERT(++numAllocations == ta.numAllocations());
                ASSERT(13 == x->d_first);
                ASSERT(42 == x->d_second);
                ASSERT(1 == x.use_count());
            }
            ASSERT(++numDeallocations == ta.numDeallocations());

            ASSERT(dam.isMaxSame());
        }
#endif

        if (verbose) printf("\nTesting constructor with no arguments"
                            "\n-------------------------------------\n");

        numAllocations   = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP = MyInplaceTestObject();

            bslma::TestAllocatorMonitor dam(da);

            {
                TCObj x = bsl::allocate_shared<MyInplaceTestObject>(&ta);
                ASSERT(++numAllocations == ta.numAllocations());
                ASSERT(EXP == *x);
                ASSERT(1 == x.use_count());
            }
            ASSERT(++numDeallocations == ta.numDeallocations());

            ASSERT(dam.isMaxSame());
        }

        numAllocations   = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP = MyInplaceTestObject();

            bslma::TestAllocatorMonitor dam(da);

            {
                bsl::shared_ptr<const MyInplaceTestObject> x =
                          bsl::allocate_shared<const MyInplaceTestObject>(&ta);
                ASSERT(++numAllocations == ta.numAllocations());
                ASSERT(EXP == *x);
                ASSERT(1 == x.use_count());
            }
            ASSERT(++numDeallocations == ta.numDeallocations());

            ASSERT(dam.isMaxSame());
        }

        numAllocations   = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            const MyInplaceAllocatableObject EXP =
                                                  MyInplaceAllocatableObject();

            bslma::TestAllocatorMonitor dam(da);

            {
                bsl::shared_ptr<const MyInplaceAllocatableObject> x =
                   bsl::allocate_shared<const MyInplaceAllocatableObject>(&ta);
                ++numAllocations;
                ASSERTV(numAllocations,   ta.numAllocations(),
                        numAllocations == ta.numAllocations());
                ASSERT(EXP == *x);
                ASSERT(1 == x.use_count());
            }
            ++numDeallocations;
            ASSERTV(numDeallocations,   ta.numDeallocations(),
                    numDeallocations == ta.numDeallocations());

            ASSERT(dam.isInUseSame());
        }

#if 0   // The use of a separate allocator for the element is not supported,
        // unless the type has a constructor taking an allocator, but NOT
        // marked with the 'UsesBslmaAllocator' trait.  This path exists to
        // test an evolution of the 'allocate_shared' function where this was
        // the expected (and therefore tested) behavior and is retained for
        // reference purposes only.

        numAllocations   = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            const MyInplaceAllocatableObject EXP =
                                                  MyInplaceAllocatableObject();

            bslma::TestAllocatorMonitor dam(da);

            bslma::TestAllocator sa("shared object allocator",
                                    veryVeryVeryVerbose);
            {
                bsl::shared_ptr<const MyInplaceAllocatableObject> x =
                   bsl::allocate_shared<const MyInplaceAllocatableObject>(&ta,
                                                                          &sa);
                ++numAllocations;
                ASSERTV(numAllocations,   ta.numAllocations(),
                        numAllocations == ta.numAllocations());
                ASSERT(EXP == *x);
                ASSERT(&sa == x->allocator());
                ASSERT(1 == x.use_count());
                ASSERTV(sa.numAllocations(), 0 == sa.numAllocations());
            }
            ++numDeallocations;
            ASSERTV(numDeallocations,   ta.numDeallocations(),
                    numDeallocations == ta.numDeallocations());

            ASSERT(dam.isMaxSame());
        }
#endif
        if (verbose) printf("\nTesting with allocator-UNaware types"
                            "\n====================================\n");

        Harness::testCase33(&ta);

        if (verbose) printf("\nTesting with allocator-aware types"
                            "\n==================================\n");

#if !defined(BSL_DO_NOT_TEST_MOVE_FORWARDING)
        if (verbose) printf("\nTesting constructor with 0-13 arguments"
                            "\n---------------------------------------\n");

        // Provide overloads to support the following tests
        Harness::testCase34_AllocatorAware< 0,2,2,2,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase34_AllocatorAware< 1,1,2,2,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase34_AllocatorAware< 2,1,1,2,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase34_AllocatorAware< 3,1,1,1,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase34_AllocatorAware< 4,1,1,1,1,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase34_AllocatorAware< 5,1,1,1,1,1,2,2,2,2,2,2,2,2,2>();
        Harness::testCase34_AllocatorAware< 6,1,1,1,1,1,1,2,2,2,2,2,2,2,2>();
        Harness::testCase34_AllocatorAware< 7,1,1,1,1,1,1,1,2,2,2,2,2,2,2>();
        Harness::testCase34_AllocatorAware< 8,1,1,1,1,1,1,1,1,2,2,2,2,2,2>();
        Harness::testCase34_AllocatorAware< 9,1,1,1,1,1,1,1,1,1,2,2,2,2,2>();
        Harness::testCase34_AllocatorAware<10,1,1,1,1,1,1,1,1,1,1,2,2,2,2>();
        Harness::testCase34_AllocatorAware<11,1,1,1,1,1,1,1,1,1,1,1,2,2,2>();
        Harness::testCase34_AllocatorAware<12,1,1,1,1,1,1,1,1,1,1,1,1,2,2>();
        Harness::testCase34_AllocatorAware<13,1,1,1,1,1,1,1,1,1,1,1,1,1,2>();

        if (verbose) printf("\nTesting constructor with 14 arguments"
                            "\n-------------------------------------\n");

        //  Main test of 14 parameters
        //  Can we generate the integer sequences by macro?
        //  If we can automate cycle this way, could efficiently generate for
        //  0-13 arguments as well, rather than rely on a special test case.
        Harness::testCase34_AllocatorAware<14,0,0,0,0,0,0,0,0,0,0,0,0,0,0>();
        Harness::testCase34_AllocatorAware<14,1,0,0,0,0,0,0,0,0,0,0,0,0,0>();
        Harness::testCase34_AllocatorAware<14,0,1,0,0,0,0,0,0,0,0,0,0,0,0>();
        Harness::testCase34_AllocatorAware<14,0,0,1,0,0,0,0,0,0,0,0,0,0,0>();
        Harness::testCase34_AllocatorAware<14,0,0,0,1,0,0,0,0,0,0,0,0,0,0>();
        Harness::testCase34_AllocatorAware<14,0,0,0,0,1,0,0,0,0,0,0,0,0,0>();
        Harness::testCase34_AllocatorAware<14,0,0,0,0,0,1,0,0,0,0,0,0,0,0>();
        Harness::testCase34_AllocatorAware<14,0,0,0,0,0,0,1,0,0,0,0,0,0,0>();
        Harness::testCase34_AllocatorAware<14,0,0,0,0,0,0,0,1,0,0,0,0,0,0>();
        Harness::testCase34_AllocatorAware<14,0,0,0,0,0,0,0,0,1,0,0,0,0,0>();
        Harness::testCase34_AllocatorAware<14,0,0,0,0,0,0,0,0,0,1,0,0,0,0>();
        Harness::testCase34_AllocatorAware<14,0,0,0,0,0,0,0,0,0,0,1,0,0,0>();
        Harness::testCase34_AllocatorAware<14,0,0,0,0,0,0,0,0,0,0,0,1,0,0>();
        Harness::testCase34_AllocatorAware<14,0,0,0,0,0,0,0,0,0,0,0,0,1,0>();
        Harness::testCase34_AllocatorAware<14,0,0,0,0,0,0,0,0,0,0,0,0,0,1>();
        Harness::testCase34_AllocatorAware<14,1,1,1,1,1,1,1,1,1,1,1,1,1,1>();


        if (verbose) printf("\nTesting allocation when all arguments move"
                            "\n------------------------------------------\n");

        numAllocations   = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            // 14 arguments, all using the object allocator
            MyAllocatableArg01 A01(VA01, &ta);
            MyAllocatableArg02 A02(VA02, &ta);
            MyAllocatableArg03 A03(VA03, &ta);
            MyAllocatableArg04 A04(VA04, &ta);
            MyAllocatableArg05 A05(VA05, &ta);
            MyAllocatableArg06 A06(VA06, &ta);
            MyAllocatableArg07 A07(VA07, &ta);
            MyAllocatableArg08 A08(VA08, &ta);
            MyAllocatableArg09 A09(VA09, &ta);
            MyAllocatableArg10 A10(VA10, &ta);
            MyAllocatableArg11 A11(VA11, &ta);
            MyAllocatableArg12 A12(VA12, &ta);
            MyAllocatableArg13 A13(VA13, &ta);
            MyAllocatableArg14 A14(VA14, &ta);

            ASSERTV(numAllocations + 14,   ta.numAllocations(),
                    numAllocations + 14 == ta.numAllocations());
            ASSERTV(numDeallocations,    ta.numDeallocations(),
                    numDeallocations  == ta.numDeallocations());

            numAllocations   = ta.numAllocations();
            numDeallocations = ta.numDeallocations();

            const MyInplaceAllocatableObject EXP(MoveUtil::move(A01),
                                                 MoveUtil::move(A02),
                                                 MoveUtil::move(A03),
                                                 MoveUtil::move(A04),
                                                 MoveUtil::move(A05),
                                                 MoveUtil::move(A06),
                                                 MoveUtil::move(A07),
                                                 MoveUtil::move(A08),
                                                 MoveUtil::move(A09),
                                                 MoveUtil::move(A10),
                                                 MoveUtil::move(A11),
                                                 MoveUtil::move(A12),
                                                 MoveUtil::move(A13),
                                                 MoveUtil::move(A14),
                                                &ta);
            ASSERTV( &ta, EXP.allocator(), &ta == EXP.allocator() );
            ASSERTV(VA01, EXP.arg01(), VA01 == EXP.arg01());
            ASSERTV(VA02, EXP.arg02(), VA02 == EXP.arg02());
            ASSERTV(VA03, EXP.arg03(), VA03 == EXP.arg03());
            ASSERTV(VA04, EXP.arg04(), VA04 == EXP.arg04());
            ASSERTV(VA05, EXP.arg05(), VA05 == EXP.arg05());
            ASSERTV(VA06, EXP.arg06(), VA06 == EXP.arg06());
            ASSERTV(VA07, EXP.arg07(), VA07 == EXP.arg07());
            ASSERTV(VA08, EXP.arg08(), VA08 == EXP.arg08());
            ASSERTV(VA09, EXP.arg09(), VA09 == EXP.arg09());
            ASSERTV(VA10, EXP.arg10(), VA10 == EXP.arg10());
            ASSERTV(VA11, EXP.arg11(), VA11 == EXP.arg11());
            ASSERTV(VA12, EXP.arg12(), VA12 == EXP.arg12());
            ASSERTV(VA13, EXP.arg13(), VA13 == EXP.arg13());
            ASSERTV(VA14, EXP.arg14(), VA14 == EXP.arg14());

            ASSERTV(MoveState::e_MOVED == A01.movedFrom());
            ASSERTV(MoveState::e_MOVED == A02.movedFrom());
            ASSERTV(MoveState::e_MOVED == A03.movedFrom());
            ASSERTV(MoveState::e_MOVED == A04.movedFrom());
            ASSERTV(MoveState::e_MOVED == A05.movedFrom());
            ASSERTV(MoveState::e_MOVED == A06.movedFrom());
            ASSERTV(MoveState::e_MOVED == A07.movedFrom());
            ASSERTV(MoveState::e_MOVED == A08.movedFrom());
            ASSERTV(MoveState::e_MOVED == A09.movedFrom());
            ASSERTV(MoveState::e_MOVED == A10.movedFrom());
            ASSERTV(MoveState::e_MOVED == A11.movedFrom());
            ASSERTV(MoveState::e_MOVED == A12.movedFrom());
            ASSERTV(MoveState::e_MOVED == A13.movedFrom());
            ASSERTV(MoveState::e_MOVED == A14.movedFrom());

            ASSERTV(numAllocations,   ta.numAllocations(),
                    numAllocations == ta.numAllocations());
            ASSERTV(numDeallocations,    ta.numDeallocations(),
                    numDeallocations  == ta.numDeallocations());

            bslma::TestAllocatorMonitor dam(da);

            {
                // 14 arguments, all using the object allocator
                MyAllocatableArg01 B01(VA01, &ta);
                MyAllocatableArg02 B02(VA02, &ta);
                MyAllocatableArg03 B03(VA03, &ta);
                MyAllocatableArg04 B04(VA04, &ta);
                MyAllocatableArg05 B05(VA05, &ta);
                MyAllocatableArg06 B06(VA06, &ta);
                MyAllocatableArg07 B07(VA07, &ta);
                MyAllocatableArg08 B08(VA08, &ta);
                MyAllocatableArg09 B09(VA09, &ta);
                MyAllocatableArg10 B10(VA10, &ta);
                MyAllocatableArg11 B11(VA11, &ta);
                MyAllocatableArg12 B12(VA12, &ta);
                MyAllocatableArg13 B13(VA13, &ta);
                MyAllocatableArg14 B14(VA14, &ta);

                ASSERTV(numAllocations + 14,   ta.numAllocations(),
                        numAllocations + 14 == ta.numAllocations());
                ASSERTV(numDeallocations,    ta.numDeallocations(),
                        numDeallocations  == ta.numDeallocations());

                numAllocations   = ta.numAllocations();
                numDeallocations = ta.numDeallocations();

                bsl::shared_ptr<const MyInplaceAllocatableObject> x =
                       bsl::allocate_shared<const MyInplaceAllocatableObject>(
                                                          &ta,
                                                          MoveUtil::move(B01),
                                                          MoveUtil::move(B02),
                                                          MoveUtil::move(B03),
                                                          MoveUtil::move(B04),
                                                          MoveUtil::move(B05),
                                                          MoveUtil::move(B06),
                                                          MoveUtil::move(B07),
                                                          MoveUtil::move(B08),
                                                          MoveUtil::move(B09),
                                                          MoveUtil::move(B10),
                                                          MoveUtil::move(B11),
                                                          MoveUtil::move(B12),
                                                          MoveUtil::move(B13),
                                                          MoveUtil::move(B14));
                ASSERTV(numAllocations + 1,   ta.numAllocations(),
                        numAllocations + 1 == ta.numAllocations());
                ASSERTV(numDeallocations,    ta.numDeallocations(),
                        numDeallocations  == ta.numDeallocations());

                numAllocations   = ta.numAllocations();
                numDeallocations = ta.numDeallocations();

                ASSERT(EXP == *x);
                ASSERT(1 == x.use_count());
                ASSERTV(&ta, x->allocator(), &ta == x->allocator() );

                ASSERTV(MoveState::e_MOVED == B01.movedFrom());
                ASSERTV(MoveState::e_MOVED == B02.movedFrom());
                ASSERTV(MoveState::e_MOVED == B03.movedFrom());
                ASSERTV(MoveState::e_MOVED == B04.movedFrom());
                ASSERTV(MoveState::e_MOVED == B05.movedFrom());
                ASSERTV(MoveState::e_MOVED == B06.movedFrom());
                ASSERTV(MoveState::e_MOVED == B07.movedFrom());
                ASSERTV(MoveState::e_MOVED == B08.movedFrom());
                ASSERTV(MoveState::e_MOVED == B09.movedFrom());
                ASSERTV(MoveState::e_MOVED == B10.movedFrom());
                ASSERTV(MoveState::e_MOVED == B11.movedFrom());
                ASSERTV(MoveState::e_MOVED == B12.movedFrom());
                ASSERTV(MoveState::e_MOVED == B13.movedFrom());
                ASSERTV(MoveState::e_MOVED == B14.movedFrom());
            }

            ASSERTV(numAllocations,   ta.numAllocations(),
                    numAllocations == ta.numAllocations());
            ASSERTV(numDeallocations + 15,    ta.numDeallocations(),
                    numDeallocations + 15 == ta.numDeallocations());

            numAllocations   = ta.numAllocations();
            numDeallocations = ta.numDeallocations();

            ASSERT(dam.isTotalSame());
        }
#else // BSL_DO_NOT_TEST_MOVE_FORWARDING
        Harness::testCase34_AllocatorAware< 0,2,2,2,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase34_AllocatorAware< 1,0,2,2,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase34_AllocatorAware< 2,0,0,2,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase34_AllocatorAware< 3,0,0,0,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase34_AllocatorAware< 4,0,0,0,0,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase34_AllocatorAware< 5,0,0,0,0,0,2,2,2,2,2,2,2,2,2>();
        Harness::testCase34_AllocatorAware< 6,0,0,0,0,0,0,2,2,2,2,2,2,2,2>();
        Harness::testCase34_AllocatorAware< 7,0,0,0,0,0,0,0,2,2,2,2,2,2,2>();
        Harness::testCase34_AllocatorAware< 8,0,0,0,0,0,0,0,0,2,2,2,2,2,2>();
        Harness::testCase34_AllocatorAware< 9,0,0,0,0,0,0,0,0,0,2,2,2,2,2>();
        Harness::testCase34_AllocatorAware<10,0,0,0,0,0,0,0,0,0,0,2,2,2,2>();
        Harness::testCase34_AllocatorAware<11,0,0,0,0,0,0,0,0,0,0,0,2,2,2>();
        Harness::testCase34_AllocatorAware<12,0,0,0,0,0,0,0,0,0,0,0,0,2,2>();
        Harness::testCase34_AllocatorAware<13,0,0,0,0,0,0,0,0,0,0,0,0,0,2>();
        Harness::testCase34_AllocatorAware<14,0,0,0,0,0,0,0,0,0,0,0,0,0,0>();
#endif // BSL_DO_NOT_TEST_MOVE_FORWARDING

        // TBD: EXCEPTION-SAFETY TESTS (0-14 arguments)
        //      (no negative testing as contract is wide)

#if defined(BDE_BUILD_TARGET_EXC)
        // Test for no leaks when allocated object's constructor throws..
        int constructCount = 0;
        int destroyCount = 0;
        try {
            bsl::allocate_shared<const MyInstrumentedObject>(&ta,
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
        // TESTING 'allocate_shared<T>(A, ...)'
        //   This test is concerned with the 'allocate_shared' free-function
        //   that takes a standard-confirming allocator, per the C++11/14 ISO
        //   standard.  Concern for the overloads taking BDE allocator is
        //   deferred to the next test case.
        //
        // Concerns:
        //: 1  'allocate_shared' returns a 'shared_ptr' pointing to a new
        //:    object of the specified 'T' type, where 'T' must be a complete
        //:    type.
        //: 2  The supplied allocator is used to create the shared rep holding
        //:    the ref-counts, deleter, and shared object.
        //: 3  There is only one allocation to create the shared object,
        //:    unless the shared object performs additional allocations.
        //: 4  The appropriate constructor for 'T' is called, forwarding all
        //:    of the supplied arguments to that constructor, in the supplied
        //:    order.
        //: 5  An allocator for 'T' can be supplied as an argument if 'T'
        //:    supports allocators.
        //: 6  The reference count of the 'shared_ptr' object shall be 1.
        //: 7  No memory is leaked if the constructor of 'T' throws an
        //:    exception.
        //: 8.  The default allocator should not be used, unless called by the
        //:     constructor of the shared object.
        //: 9.  The global allocator should not be used, unless called by the
        //:     constructor of the shared object.
        //
        // Plan:
        //: 1. TBD
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

        if (verbose) printf("\nTESTING 'allocate_shared<T>(A, ...)'"
                            "\n====================================\n");

        using BloombergLP::bsltf::StdStatefulAllocator;

        typedef StdStatefulAllocator<int, true,  true,  true,  true > ALLOC_A;
        typedef StdStatefulAllocator<int, false, false, false, false> ALLOC_B;

        bslma::TestAllocator ta("Test Case 33 wrapped test-allocator",
                                veryVeryVeryVerbose);

        ALLOC_A allocA(&ta);
        Harness::testCase33(allocA);

        ALLOC_B allocB(&ta);
        Harness::testCase33(allocB);

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
        //: 1.  'make_shared' returns a 'shared_ptr' pointing to a new object
        //:     of the specified 'T' type, where 'T' must be a complete type.
        //: 2.  The default allocator is used to create the shared rep holding
        //:     the ref-counts, deleter, and shared object.
        //: 3.  There is only one allocation to create the shared object,
        //:     unless the shared object performs additional allocations.
        //: 4.  The appropriate constructor for 'T' is called, forwarding all
        //:     of the supplied arguments to that constructor, in the supplied
        //:     order.
        //: 5.  An allocator for 'T' can be supplied as an argument if 'T'
        //:     supports allocators.
        //: 6.  The reference count of the 'shared_ptr' object shall be 1.
        //: 7.  No memory is leaked if the constructor of 'T' throws an
        //:     exception.
        //: 8.  'make_shared' works for allocator types, DRQS 75629850
        //
        // Plan:
        //  TBD
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

        bslma::TestAllocator ta("Test case 32 default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&ta);

        if (verbose) printf(
                          "\n'make_shared' with default allocator for args"
                          "\n---------------------------------------------\n");

#if !defined(BSL_DO_NOT_TEST_MOVE_FORWARDING)
        if (verbose) printf("\nTesting constructor with no arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_DefaultAllocator<0,2,2,2,2,2,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 1 argument"
                            "\n-----------------------------------\n");

        Harness::testCase32_DefaultAllocator<1,0,2,2,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<1,1,2,2,2,2,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 2 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_DefaultAllocator<2,0,0,2,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<2,1,0,2,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<2,0,1,2,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<2,1,1,2,2,2,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 3 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_DefaultAllocator<3,0,0,0,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<3,1,0,0,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<3,0,1,0,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<3,0,0,1,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<3,1,1,1,2,2,2,2,2,2,2,2,2,2,2>();

#if !defined(BSLSTL_SHAREDPTR_LIMIT_TESTING_COMPLEXITY)
        if (verbose) printf("\nTesting constructor with 4 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_DefaultAllocator<4,0,0,0,0,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<4,1,0,0,0,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<4,0,1,0,0,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<4,0,0,1,0,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<4,0,0,0,1,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<4,1,1,1,1,2,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 5 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_DefaultAllocator<5,0,0,0,0,0,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<5,1,0,0,0,0,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<5,0,1,0,0,0,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<5,0,0,1,0,0,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<5,0,0,0,1,0,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<5,0,0,0,0,1,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<5,1,1,1,1,1,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 6 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_DefaultAllocator<6,0,0,0,0,0,0,2,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<6,1,0,0,0,0,0,2,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<6,0,1,0,0,0,0,2,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<6,0,0,1,0,0,0,2,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<6,0,0,0,1,0,0,2,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<6,0,0,0,0,1,0,2,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<6,0,0,0,0,0,1,2,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<6,1,1,1,1,1,1,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 7 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_DefaultAllocator<7,0,0,0,0,0,0,0,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<7,1,0,0,0,0,0,0,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<7,0,1,0,0,0,0,0,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<7,0,0,1,0,0,0,0,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<7,0,0,0,1,0,0,0,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<7,0,0,0,0,1,0,0,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<7,0,0,0,0,0,1,0,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<7,0,0,0,0,0,0,1,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<7,1,1,1,1,1,1,1,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 8 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_DefaultAllocator<8,0,0,0,0,0,0,0,0,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<8,1,0,0,0,0,0,0,0,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<8,0,1,0,0,0,0,0,0,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<8,0,0,1,0,0,0,0,0,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<8,0,0,0,1,0,0,0,0,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<8,0,0,0,0,1,0,0,0,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<8,0,0,0,0,0,1,0,0,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<8,0,0,0,0,0,0,1,0,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<8,0,0,0,0,0,0,0,1,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<8,1,1,1,1,1,1,1,1,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 9 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_DefaultAllocator<9,0,0,0,0,0,0,0,0,0,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<9,1,0,0,0,0,0,0,0,0,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<9,0,1,0,0,0,0,0,0,0,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<9,0,0,1,0,0,0,0,0,0,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<9,0,0,0,1,0,0,0,0,0,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<9,0,0,0,0,1,0,0,0,0,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<9,0,0,0,0,0,1,0,0,0,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<9,0,0,0,0,0,0,1,0,0,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<9,0,0,0,0,0,0,0,1,0,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<9,0,0,0,0,0,0,0,0,1,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<9,1,1,1,1,1,1,1,1,1,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 10 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_DefaultAllocator<10,0,0,0,0,0,0,0,0,0,0,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<10,1,0,0,0,0,0,0,0,0,0,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<10,0,1,0,0,0,0,0,0,0,0,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<10,0,0,1,0,0,0,0,0,0,0,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<10,0,0,0,1,0,0,0,0,0,0,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<10,0,0,0,0,1,0,0,0,0,0,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<10,0,0,0,0,0,1,0,0,0,0,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<10,0,0,0,0,0,0,1,0,0,0,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<10,0,0,0,0,0,0,0,1,0,0,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<10,0,0,0,0,0,0,0,0,1,0,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<10,0,0,0,0,0,0,0,0,0,1,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<10,1,1,1,1,1,1,1,1,1,1,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 11 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_DefaultAllocator<11,0,0,0,0,0,0,0,0,0,0,0,2,2,2>();
        Harness::testCase32_DefaultAllocator<11,1,0,0,0,0,0,0,0,0,0,0,2,2,2>();
        Harness::testCase32_DefaultAllocator<11,0,1,0,0,0,0,0,0,0,0,0,2,2,2>();
        Harness::testCase32_DefaultAllocator<11,0,0,1,0,0,0,0,0,0,0,0,2,2,2>();
        Harness::testCase32_DefaultAllocator<11,0,0,0,1,0,0,0,0,0,0,0,2,2,2>();
        Harness::testCase32_DefaultAllocator<11,0,0,0,0,1,0,0,0,0,0,0,2,2,2>();
        Harness::testCase32_DefaultAllocator<11,0,0,0,0,0,1,0,0,0,0,0,2,2,2>();
        Harness::testCase32_DefaultAllocator<11,0,0,0,0,0,0,1,0,0,0,0,2,2,2>();
        Harness::testCase32_DefaultAllocator<11,0,0,0,0,0,0,0,1,0,0,0,2,2,2>();
        Harness::testCase32_DefaultAllocator<11,0,0,0,0,0,0,0,0,1,0,0,2,2,2>();
        Harness::testCase32_DefaultAllocator<11,0,0,0,0,0,0,0,0,0,1,0,2,2,2>();
        Harness::testCase32_DefaultAllocator<11,0,0,0,0,0,0,0,0,0,0,1,2,2,2>();
        Harness::testCase32_DefaultAllocator<11,1,1,1,1,1,1,1,1,1,1,1,2,2,2>();

        if (verbose) printf("\nTesting constructor with 12 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_DefaultAllocator<12,0,0,0,0,0,0,0,0,0,0,0,0,2,2>();
        Harness::testCase32_DefaultAllocator<12,1,0,0,0,0,0,0,0,0,0,0,0,2,2>();
        Harness::testCase32_DefaultAllocator<12,0,1,0,0,0,0,0,0,0,0,0,0,2,2>();
        Harness::testCase32_DefaultAllocator<12,0,0,1,0,0,0,0,0,0,0,0,0,2,2>();
        Harness::testCase32_DefaultAllocator<12,0,0,0,1,0,0,0,0,0,0,0,0,2,2>();
        Harness::testCase32_DefaultAllocator<12,0,0,0,0,1,0,0,0,0,0,0,0,2,2>();
        Harness::testCase32_DefaultAllocator<12,0,0,0,0,0,1,0,0,0,0,0,0,2,2>();
        Harness::testCase32_DefaultAllocator<12,0,0,0,0,0,0,1,0,0,0,0,0,2,2>();
        Harness::testCase32_DefaultAllocator<12,0,0,0,0,0,0,0,1,0,0,0,0,2,2>();
        Harness::testCase32_DefaultAllocator<12,0,0,0,0,0,0,0,0,1,0,0,0,2,2>();
        Harness::testCase32_DefaultAllocator<12,0,0,0,0,0,0,0,0,0,1,0,0,2,2>();
        Harness::testCase32_DefaultAllocator<12,0,0,0,0,0,0,0,0,0,0,1,0,2,2>();
        Harness::testCase32_DefaultAllocator<12,0,0,0,0,0,0,0,0,0,0,0,1,2,2>();
        Harness::testCase32_DefaultAllocator<12,1,1,1,1,1,1,1,1,1,1,1,1,2,2>();

        if (verbose) printf("\nTesting constructor with 13 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_DefaultAllocator<13,0,0,0,0,0,0,0,0,0,0,0,0,0,2>();
        Harness::testCase32_DefaultAllocator<13,1,0,0,0,0,0,0,0,0,0,0,0,0,2>();
        Harness::testCase32_DefaultAllocator<13,0,1,0,0,0,0,0,0,0,0,0,0,0,2>();
        Harness::testCase32_DefaultAllocator<13,0,0,1,0,0,0,0,0,0,0,0,0,0,2>();
        Harness::testCase32_DefaultAllocator<13,0,0,0,1,0,0,0,0,0,0,0,0,0,2>();
        Harness::testCase32_DefaultAllocator<13,0,0,0,0,1,0,0,0,0,0,0,0,0,2>();
        Harness::testCase32_DefaultAllocator<13,0,0,0,0,0,1,0,0,0,0,0,0,0,2>();
        Harness::testCase32_DefaultAllocator<13,0,0,0,0,0,0,1,0,0,0,0,0,0,2>();
        Harness::testCase32_DefaultAllocator<13,0,0,0,0,0,0,0,1,0,0,0,0,0,2>();
        Harness::testCase32_DefaultAllocator<13,0,0,0,0,0,0,0,0,1,0,0,0,0,2>();
        Harness::testCase32_DefaultAllocator<13,0,0,0,0,0,0,0,0,0,1,0,0,0,2>();
        Harness::testCase32_DefaultAllocator<13,0,0,0,0,0,0,0,0,0,0,1,0,0,2>();
        Harness::testCase32_DefaultAllocator<13,0,0,0,0,0,0,0,0,0,0,0,1,0,2>();
        Harness::testCase32_DefaultAllocator<13,0,0,0,0,0,0,0,0,0,0,0,0,1,2>();
        Harness::testCase32_DefaultAllocator<13,1,1,1,1,1,1,1,1,1,1,1,1,1,2>();
#else  // BSLSTL_SHAREDPTR_LIMIT_TESTING_COMPLEXITY
        if (verbose) printf("\nTesting constructor with 4 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_DefaultAllocator<4,0,0,0,0,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<4,1,1,1,1,2,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 5 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_DefaultAllocator<5,0,0,0,0,0,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<5,1,1,1,1,1,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 6 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_DefaultAllocator<6,0,0,0,0,0,0,2,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<6,1,1,1,1,1,1,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 7 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_DefaultAllocator<7,0,0,0,0,0,0,0,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<7,1,1,1,1,1,1,1,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 8 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_DefaultAllocator<8,0,0,0,0,0,0,0,0,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<8,1,1,1,1,1,1,1,1,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 9 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_DefaultAllocator<9,0,0,0,0,0,0,0,0,0,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<9,1,1,1,1,1,1,1,1,1,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 10 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_DefaultAllocator<10,0,0,0,0,0,0,0,0,0,0,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<10,1,1,1,1,1,1,1,1,1,1,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 11 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_DefaultAllocator<11,0,0,0,0,0,0,0,0,0,0,0,2,2,2>();
        Harness::testCase32_DefaultAllocator<11,1,1,1,1,1,1,1,1,1,1,1,2,2,2>();

        if (verbose) printf("\nTesting constructor with 12 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_DefaultAllocator<12,0,0,0,0,0,0,0,0,0,0,0,0,2,2>();
        Harness::testCase32_DefaultAllocator<12,1,1,1,1,1,1,1,1,1,1,1,1,2,2>();

        if (verbose) printf("\nTesting constructor with 13 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_DefaultAllocator<13,0,0,0,0,0,0,0,0,0,0,0,0,0,2>();
        Harness::testCase32_DefaultAllocator<13,1,1,1,1,1,1,1,1,1,1,1,1,1,2>();
#endif // BSLSTL_SHAREDPTR_LIMIT_TESTING_COMPLEXITY

        if (verbose) printf("\nTesting constructor with 14 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_DefaultAllocator<14,0,0,0,0,0,0,0,0,0,0,0,0,0,0>();
        Harness::testCase32_DefaultAllocator<14,1,0,0,0,0,0,0,0,0,0,0,0,0,0>();
        Harness::testCase32_DefaultAllocator<14,0,1,0,0,0,0,0,0,0,0,0,0,0,0>();
        Harness::testCase32_DefaultAllocator<14,0,0,1,0,0,0,0,0,0,0,0,0,0,0>();
        Harness::testCase32_DefaultAllocator<14,0,0,0,1,0,0,0,0,0,0,0,0,0,0>();
        Harness::testCase32_DefaultAllocator<14,0,0,0,0,1,0,0,0,0,0,0,0,0,0>();
        Harness::testCase32_DefaultAllocator<14,0,0,0,0,0,1,0,0,0,0,0,0,0,0>();
        Harness::testCase32_DefaultAllocator<14,0,0,0,0,0,0,1,0,0,0,0,0,0,0>();
        Harness::testCase32_DefaultAllocator<14,0,0,0,0,0,0,0,1,0,0,0,0,0,0>();
        Harness::testCase32_DefaultAllocator<14,0,0,0,0,0,0,0,0,1,0,0,0,0,0>();
        Harness::testCase32_DefaultAllocator<14,0,0,0,0,0,0,0,0,0,1,0,0,0,0>();
        Harness::testCase32_DefaultAllocator<14,0,0,0,0,0,0,0,0,0,0,1,0,0,0>();
        Harness::testCase32_DefaultAllocator<14,0,0,0,0,0,0,0,0,0,0,0,1,0,0>();
        Harness::testCase32_DefaultAllocator<14,0,0,0,0,0,0,0,0,0,0,0,0,1,0>();
        Harness::testCase32_DefaultAllocator<14,0,0,0,0,0,0,0,0,0,0,0,0,0,1>();
        Harness::testCase32_DefaultAllocator<14,1,1,1,1,1,1,1,1,1,1,1,1,1,1>();
#else // BSL_DO_NOT_TEST_MOVE_FORWARDING
        Harness::testCase32_DefaultAllocator< 0,2,2,2,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator< 1,0,2,2,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator< 2,0,0,2,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator< 3,0,0,0,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator< 4,0,0,0,0,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator< 5,0,0,0,0,0,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator< 6,0,0,0,0,0,0,2,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator< 7,0,0,0,0,0,0,0,2,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator< 8,0,0,0,0,0,0,0,0,2,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator< 9,0,0,0,0,0,0,0,0,0,2,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<10,0,0,0,0,0,0,0,0,0,0,2,2,2,2>();
        Harness::testCase32_DefaultAllocator<11,0,0,0,0,0,0,0,0,0,0,0,2,2,2>();
        Harness::testCase32_DefaultAllocator<12,0,0,0,0,0,0,0,0,0,0,0,0,2,2>();
        Harness::testCase32_DefaultAllocator<13,0,0,0,0,0,0,0,0,0,0,0,0,0,2>();
        Harness::testCase32_DefaultAllocator<14,0,0,0,0,0,0,0,0,0,0,0,0,0,0>();
#endif // BSL_DO_NOT_TEST_MOVE_FORWARDING


        if (verbose) printf("\n'make_shared' with local allocator for args"
                            "\n-------------------------------------------\n");

#if !defined(BSL_DO_NOT_TEST_MOVE_FORWARDING)
        if (verbose) printf("\nTesting constructor with no arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_LocalAllocator<0,2,2,2,2,2,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 1 argument"
                            "\n-----------------------------------\n");

        Harness::testCase32_LocalAllocator<1,0,2,2,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<1,1,2,2,2,2,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 2 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_LocalAllocator<2,0,0,2,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<2,1,0,2,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<2,0,1,2,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<2,1,1,2,2,2,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 3 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_LocalAllocator<3,0,0,0,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<3,1,0,0,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<3,0,1,0,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<3,0,0,1,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<3,1,1,1,2,2,2,2,2,2,2,2,2,2,2>();

#if !defined(BSLSTL_SHAREDPTR_LIMIT_TESTING_COMPLEXITY)
        if (verbose) printf("\nTesting constructor with 4 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_LocalAllocator<4,0,0,0,0,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<4,1,0,0,0,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<4,0,1,0,0,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<4,0,0,1,0,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<4,0,0,0,1,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<4,1,1,1,1,2,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 5 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_LocalAllocator<5,0,0,0,0,0,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<5,1,0,0,0,0,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<5,0,1,0,0,0,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<5,0,0,1,0,0,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<5,0,0,0,1,0,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<5,0,0,0,0,1,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<5,1,1,1,1,1,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 6 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_LocalAllocator<6,0,0,0,0,0,0,2,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<6,1,0,0,0,0,0,2,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<6,0,1,0,0,0,0,2,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<6,0,0,1,0,0,0,2,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<6,0,0,0,1,0,0,2,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<6,0,0,0,0,1,0,2,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<6,0,0,0,0,0,1,2,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<6,1,1,1,1,1,1,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 7 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_LocalAllocator<7,0,0,0,0,0,0,0,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<7,1,0,0,0,0,0,0,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<7,0,1,0,0,0,0,0,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<7,0,0,1,0,0,0,0,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<7,0,0,0,1,0,0,0,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<7,0,0,0,0,1,0,0,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<7,0,0,0,0,0,1,0,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<7,0,0,0,0,0,0,1,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<7,1,1,1,1,1,1,1,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 8 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_LocalAllocator<8,0,0,0,0,0,0,0,0,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<8,1,0,0,0,0,0,0,0,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<8,0,1,0,0,0,0,0,0,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<8,0,0,1,0,0,0,0,0,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<8,0,0,0,1,0,0,0,0,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<8,0,0,0,0,1,0,0,0,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<8,0,0,0,0,0,1,0,0,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<8,0,0,0,0,0,0,1,0,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<8,0,0,0,0,0,0,0,1,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<8,1,1,1,1,1,1,1,1,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 9 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_LocalAllocator<9,0,0,0,0,0,0,0,0,0,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<9,1,0,0,0,0,0,0,0,0,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<9,0,1,0,0,0,0,0,0,0,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<9,0,0,1,0,0,0,0,0,0,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<9,0,0,0,1,0,0,0,0,0,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<9,0,0,0,0,1,0,0,0,0,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<9,0,0,0,0,0,1,0,0,0,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<9,0,0,0,0,0,0,1,0,0,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<9,0,0,0,0,0,0,0,1,0,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<9,0,0,0,0,0,0,0,0,1,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<9,1,1,1,1,1,1,1,1,1,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 10 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_LocalAllocator<10,0,0,0,0,0,0,0,0,0,0,2,2,2,2>();
        Harness::testCase32_LocalAllocator<10,1,0,0,0,0,0,0,0,0,0,2,2,2,2>();
        Harness::testCase32_LocalAllocator<10,0,1,0,0,0,0,0,0,0,0,2,2,2,2>();
        Harness::testCase32_LocalAllocator<10,0,0,1,0,0,0,0,0,0,0,2,2,2,2>();
        Harness::testCase32_LocalAllocator<10,0,0,0,1,0,0,0,0,0,0,2,2,2,2>();
        Harness::testCase32_LocalAllocator<10,0,0,0,0,1,0,0,0,0,0,2,2,2,2>();
        Harness::testCase32_LocalAllocator<10,0,0,0,0,0,1,0,0,0,0,2,2,2,2>();
        Harness::testCase32_LocalAllocator<10,0,0,0,0,0,0,1,0,0,0,2,2,2,2>();
        Harness::testCase32_LocalAllocator<10,0,0,0,0,0,0,0,1,0,0,2,2,2,2>();
        Harness::testCase32_LocalAllocator<10,0,0,0,0,0,0,0,0,1,0,2,2,2,2>();
        Harness::testCase32_LocalAllocator<10,0,0,0,0,0,0,0,0,0,1,2,2,2,2>();
        Harness::testCase32_LocalAllocator<10,1,1,1,1,1,1,1,1,1,1,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 11 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_LocalAllocator<11,0,0,0,0,0,0,0,0,0,0,0,2,2,2>();
        Harness::testCase32_LocalAllocator<11,1,0,0,0,0,0,0,0,0,0,0,2,2,2>();
        Harness::testCase32_LocalAllocator<11,0,1,0,0,0,0,0,0,0,0,0,2,2,2>();
        Harness::testCase32_LocalAllocator<11,0,0,1,0,0,0,0,0,0,0,0,2,2,2>();
        Harness::testCase32_LocalAllocator<11,0,0,0,1,0,0,0,0,0,0,0,2,2,2>();
        Harness::testCase32_LocalAllocator<11,0,0,0,0,1,0,0,0,0,0,0,2,2,2>();
        Harness::testCase32_LocalAllocator<11,0,0,0,0,0,1,0,0,0,0,0,2,2,2>();
        Harness::testCase32_LocalAllocator<11,0,0,0,0,0,0,1,0,0,0,0,2,2,2>();
        Harness::testCase32_LocalAllocator<11,0,0,0,0,0,0,0,1,0,0,0,2,2,2>();
        Harness::testCase32_LocalAllocator<11,0,0,0,0,0,0,0,0,1,0,0,2,2,2>();
        Harness::testCase32_LocalAllocator<11,0,0,0,0,0,0,0,0,0,1,0,2,2,2>();
        Harness::testCase32_LocalAllocator<11,0,0,0,0,0,0,0,0,0,0,1,2,2,2>();
        Harness::testCase32_LocalAllocator<11,1,1,1,1,1,1,1,1,1,1,1,2,2,2>();

        if (verbose) printf("\nTesting constructor with 12 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_LocalAllocator<12,0,0,0,0,0,0,0,0,0,0,0,0,2,2>();
        Harness::testCase32_LocalAllocator<12,1,0,0,0,0,0,0,0,0,0,0,0,2,2>();
        Harness::testCase32_LocalAllocator<12,0,1,0,0,0,0,0,0,0,0,0,0,2,2>();
        Harness::testCase32_LocalAllocator<12,0,0,1,0,0,0,0,0,0,0,0,0,2,2>();
        Harness::testCase32_LocalAllocator<12,0,0,0,1,0,0,0,0,0,0,0,0,2,2>();
        Harness::testCase32_LocalAllocator<12,0,0,0,0,1,0,0,0,0,0,0,0,2,2>();
        Harness::testCase32_LocalAllocator<12,0,0,0,0,0,1,0,0,0,0,0,0,2,2>();
        Harness::testCase32_LocalAllocator<12,0,0,0,0,0,0,1,0,0,0,0,0,2,2>();
        Harness::testCase32_LocalAllocator<12,0,0,0,0,0,0,0,1,0,0,0,0,2,2>();
        Harness::testCase32_LocalAllocator<12,0,0,0,0,0,0,0,0,1,0,0,0,2,2>();
        Harness::testCase32_LocalAllocator<12,0,0,0,0,0,0,0,0,0,1,0,0,2,2>();
        Harness::testCase32_LocalAllocator<12,0,0,0,0,0,0,0,0,0,0,1,0,2,2>();
        Harness::testCase32_LocalAllocator<12,0,0,0,0,0,0,0,0,0,0,0,1,2,2>();
        Harness::testCase32_LocalAllocator<12,1,1,1,1,1,1,1,1,1,1,1,1,2,2>();

        if (verbose) printf("\nTesting constructor with 13 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_LocalAllocator<13,0,0,0,0,0,0,0,0,0,0,0,0,0,2>();
        Harness::testCase32_LocalAllocator<13,1,0,0,0,0,0,0,0,0,0,0,0,0,2>();
        Harness::testCase32_LocalAllocator<13,0,1,0,0,0,0,0,0,0,0,0,0,0,2>();
        Harness::testCase32_LocalAllocator<13,0,0,1,0,0,0,0,0,0,0,0,0,0,2>();
        Harness::testCase32_LocalAllocator<13,0,0,0,1,0,0,0,0,0,0,0,0,0,2>();
        Harness::testCase32_LocalAllocator<13,0,0,0,0,1,0,0,0,0,0,0,0,0,2>();
        Harness::testCase32_LocalAllocator<13,0,0,0,0,0,1,0,0,0,0,0,0,0,2>();
        Harness::testCase32_LocalAllocator<13,0,0,0,0,0,0,1,0,0,0,0,0,0,2>();
        Harness::testCase32_LocalAllocator<13,0,0,0,0,0,0,0,1,0,0,0,0,0,2>();
        Harness::testCase32_LocalAllocator<13,0,0,0,0,0,0,0,0,1,0,0,0,0,2>();
        Harness::testCase32_LocalAllocator<13,0,0,0,0,0,0,0,0,0,1,0,0,0,2>();
        Harness::testCase32_LocalAllocator<13,0,0,0,0,0,0,0,0,0,0,1,0,0,2>();
        Harness::testCase32_LocalAllocator<13,0,0,0,0,0,0,0,0,0,0,0,1,0,2>();
        Harness::testCase32_LocalAllocator<13,0,0,0,0,0,0,0,0,0,0,0,0,1,2>();
        Harness::testCase32_LocalAllocator<13,1,1,1,1,1,1,1,1,1,1,1,1,1,2>();
#else  // BSLSTL_SHAREDPTR_LIMIT_TESTING_COMPLEXITY
        if (verbose) printf("\nTesting constructor with 4 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_LocalAllocator<4,0,0,0,0,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<4,1,1,1,1,2,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 5 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_LocalAllocator<5,0,0,0,0,0,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<5,1,1,1,1,1,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 6 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_LocalAllocator<6,0,0,0,0,0,0,2,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<6,1,1,1,1,1,1,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 7 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_LocalAllocator<7,0,0,0,0,0,0,0,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<7,1,1,1,1,1,1,1,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 8 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_LocalAllocator<8,0,0,0,0,0,0,0,0,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<8,1,1,1,1,1,1,1,1,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 9 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_LocalAllocator<9,0,0,0,0,0,0,0,0,0,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<9,1,1,1,1,1,1,1,1,1,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 10 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_LocalAllocator<10,0,0,0,0,0,0,0,0,0,0,2,2,2,2>();
        Harness::testCase32_LocalAllocator<10,1,1,1,1,1,1,1,1,1,1,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 11 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_LocalAllocator<11,0,0,0,0,0,0,0,0,0,0,0,2,2,2>();
        Harness::testCase32_LocalAllocator<11,1,1,1,1,1,1,1,1,1,1,1,2,2,2>();

        if (verbose) printf("\nTesting constructor with 12 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_LocalAllocator<12,0,0,0,0,0,0,0,0,0,0,0,0,2,2>();
        Harness::testCase32_LocalAllocator<12,1,1,1,1,1,1,1,1,1,1,1,1,2,2>();

        if (verbose) printf("\nTesting constructor with 13 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_LocalAllocator<13,0,0,0,0,0,0,0,0,0,0,0,0,0,2>();
        Harness::testCase32_LocalAllocator<13,1,1,1,1,1,1,1,1,1,1,1,1,1,2>();
#endif // BSLSTL_SHAREDPTR_LIMIT_TESTING_COMPLEXITY

        if (verbose) printf("\nTesting constructor with 14 arguments"
                            "\n-------------------------------------\n");

        Harness::testCase32_LocalAllocator<14,0,0,0,0,0,0,0,0,0,0,0,0,0,0>();
        Harness::testCase32_LocalAllocator<14,1,0,0,0,0,0,0,0,0,0,0,0,0,0>();
        Harness::testCase32_LocalAllocator<14,0,1,0,0,0,0,0,0,0,0,0,0,0,0>();
        Harness::testCase32_LocalAllocator<14,0,0,1,0,0,0,0,0,0,0,0,0,0,0>();
        Harness::testCase32_LocalAllocator<14,0,0,0,1,0,0,0,0,0,0,0,0,0,0>();
        Harness::testCase32_LocalAllocator<14,0,0,0,0,1,0,0,0,0,0,0,0,0,0>();
        Harness::testCase32_LocalAllocator<14,0,0,0,0,0,1,0,0,0,0,0,0,0,0>();
        Harness::testCase32_LocalAllocator<14,0,0,0,0,0,0,1,0,0,0,0,0,0,0>();
        Harness::testCase32_LocalAllocator<14,0,0,0,0,0,0,0,1,0,0,0,0,0,0>();
        Harness::testCase32_LocalAllocator<14,0,0,0,0,0,0,0,0,1,0,0,0,0,0>();
        Harness::testCase32_LocalAllocator<14,0,0,0,0,0,0,0,0,0,1,0,0,0,0>();
        Harness::testCase32_LocalAllocator<14,0,0,0,0,0,0,0,0,0,0,1,0,0,0>();
        Harness::testCase32_LocalAllocator<14,0,0,0,0,0,0,0,0,0,0,0,1,0,0>();
        Harness::testCase32_LocalAllocator<14,0,0,0,0,0,0,0,0,0,0,0,0,1,0>();
        Harness::testCase32_LocalAllocator<14,0,0,0,0,0,0,0,0,0,0,0,0,0,1>();
        Harness::testCase32_LocalAllocator<14,1,1,1,1,1,1,1,1,1,1,1,1,1,1>();
#else // BSL_DO_NOT_TEST_MOVE_FORWARDING
        Harness::testCase32_LocalAllocator< 0,2,2,2,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator< 1,0,2,2,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator< 2,0,0,2,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator< 3,0,0,0,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator< 4,0,0,0,0,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator< 5,0,0,0,0,0,2,2,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator< 6,0,0,0,0,0,0,2,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator< 7,0,0,0,0,0,0,0,2,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator< 8,0,0,0,0,0,0,0,0,2,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator< 9,0,0,0,0,0,0,0,0,0,2,2,2,2,2>();
        Harness::testCase32_LocalAllocator<10,0,0,0,0,0,0,0,0,0,0,2,2,2,2>();
        Harness::testCase32_LocalAllocator<11,0,0,0,0,0,0,0,0,0,0,0,2,2,2>();
        Harness::testCase32_LocalAllocator<12,0,0,0,0,0,0,0,0,0,0,0,0,2,2>();
        Harness::testCase32_LocalAllocator<13,0,0,0,0,0,0,0,0,0,0,0,0,0,2>();
        Harness::testCase32_LocalAllocator<14,0,0,0,0,0,0,0,0,0,0,0,0,0,0>();
#endif // BSL_DO_NOT_TEST_MOVE_FORWARDING

        if (verbose) printf("\nTesting allocator propagation"
                            "\n-----------------------------\n");

        Harness::testCase32_AllocatorPropagation< 0, false>();
        Harness::testCase32_AllocatorPropagation< 1, false>();
#if !defined(BSLSTL_SHAREDPTR_LIMIT_TESTING_COMPLEXITY)
        Harness::testCase32_AllocatorPropagation< 2, false>();
        Harness::testCase32_AllocatorPropagation< 3, false>();
        Harness::testCase32_AllocatorPropagation< 4, false>();
        Harness::testCase32_AllocatorPropagation< 5, false>();
        Harness::testCase32_AllocatorPropagation< 6, false>();
        Harness::testCase32_AllocatorPropagation< 7, false>();
        Harness::testCase32_AllocatorPropagation< 8, false>();
        Harness::testCase32_AllocatorPropagation< 9, false>();
        Harness::testCase32_AllocatorPropagation<10, false>();
        Harness::testCase32_AllocatorPropagation<11, false>();
        Harness::testCase32_AllocatorPropagation<12, false>();
        Harness::testCase32_AllocatorPropagation<13, false>();
#endif // BSLSTL_SHAREDPTR_LIMIT_TESTING_COMPLEXITY
        Harness::testCase32_AllocatorPropagation<14, false>();

        Harness::testCase32_AllocatorPropagation< 0,  true>();
#if !defined(BSLSTL_SHAREDPTR_LIMIT_TESTING_COMPLEXITY)
        Harness::testCase32_AllocatorPropagation< 1,  true>();
        Harness::testCase32_AllocatorPropagation< 2,  true>();
        Harness::testCase32_AllocatorPropagation< 3,  true>();
        Harness::testCase32_AllocatorPropagation< 4,  true>();
        Harness::testCase32_AllocatorPropagation< 5,  true>();
        Harness::testCase32_AllocatorPropagation< 6,  true>();
        Harness::testCase32_AllocatorPropagation< 7,  true>();
        Harness::testCase32_AllocatorPropagation< 8,  true>();
        Harness::testCase32_AllocatorPropagation< 9,  true>();
        Harness::testCase32_AllocatorPropagation<10,  true>();
        Harness::testCase32_AllocatorPropagation<11,  true>();
        Harness::testCase32_AllocatorPropagation<12,  true>();
        Harness::testCase32_AllocatorPropagation<13,  true>();
#endif // BSLSTL_SHAREDPTR_LIMIT_TESTING_COMPLEXITY
        Harness::testCase32_AllocatorPropagation<14,  true>();

        if (verbose) printf("\nTesting 'make_shared' with evil type"
                            "\n------------------------------------\n");

        numAllocations   = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bsl::shared_ptr<MostEvilTestType> x =
                                        bsl::make_shared<MostEvilTestType>(13);
            const bsl::shared_ptr<MostEvilTestType>& X=x;

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(13 == X.get()->data());
            ASSERT(1 == x.use_count());
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'make_shared' with allocator type"
                            "\n-----------------------------------------\n");

        numAllocations   = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bsl::shared_ptr<bslma::TestAllocator> x =
                                bsl::make_shared<bslma::TestAllocator>("Name");
            const bsl::shared_ptr<bslma::TestAllocator>& X=x;

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERTV(X->name(), 0 == strcmp("Name", X->name()));
            ASSERT(0 == X.get()->numAllocations());
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
        //   Test that the 'hash' specialization works as expected.
        //
        // Concerns:
        //: 1 'hash' is a CopyConstructible POD.
        //:
        //: 2 'operator()' produces distinct hash values for distinct inputs.
        //:
        //: 3 'operator()' produces the same result for shared pointers
        //:   aliasing the same object, regardless of ownership.
        //:
        //: 4 'operator()' supports empty shared pointers.
        //:
        //: 5 'operator()' is 'const'-qualified and can be called with 'const'
        //:   objects.
        //:
        //: 6 'operator()' does not modify its argument.
        //
        // Plan:
        //
        // Testing:
        //   size_t operator()(const shared_ptr<TYPE>& ptr) const
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'hash' FUNCTOR ('shared_ptr')"
                            "\n=====================================\n");

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
      } break;
      case 30: {
        // --------------------------------------------------------------------
        // TESTING ROUND-TRIP THROUGH 'bslma::ManagedPtr'
        //
        // Concerns:
        //: 1 Making a 'bslma::ManagedPtr' from a 'shared_ptr', and then making
        //:   a new 'shared_ptr' from that 'ManagedPtr', does not allocate a
        //:   new 'rep' object, but re-uses the managed 'rep'.
        //:
        //: 2 Making a 'bslma::ManagedPtr' from an aliased 'shared_ptr' handles
        //:   the unusual empty/shared-null states when round-tripped.
        //
        // Plan:
        //  TBD
        //
        // Testing:
        //   CONCERN: Shared -> Managed -> Shared uses same 'rep' object
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING ROUND-TRIP THROUGH 'bslma::ManagedPtr'"
                   "\n==============================================\n");

        for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
            const char CONFIG = cfg;  // construct or assign ownership transfer

            if (veryVerbose) { T_ T_ P(CONFIG) }

            bslma::TestAllocatorMonitor dam(&defaultAllocator);

            bslma::TestAllocator nullAllocator("Null Allocator for case 30");
            nullAllocator.setAllocationLimit(0);

            numAllocations   = ta.numAllocations();
            numDeallocations = ta.numDeallocations();

            if (verbose) printf("\nTesting simple round-trip"
                                "\n-------------------------\n");
            {
                numDeletes = 0;
                TObj *p = new(ta) TObj(&numDeletes);
                ASSERTV(1 + numAllocations,   ta.numAllocations(),
                        1 + numAllocations == ta.numAllocations());

                Obj x(p, &ta);      const Obj&  X   = x;
                bslma::SharedPtrRep const*const REP = X.rep();
                ASSERTV(2 + numAllocations,   ta.numAllocations(),
                        2 + numAllocations == ta.numAllocations());

                if (veryVerbose) {
                    T_ T_ P_(numDeletes); P(X.use_count());
                }
                ASSERT(0 == numDeletes);
                ASSERT(p == X.get());
                ASSERT(1 == X.use_count());

                {
                    bslma::ManagedPtr<TObj> y(X.managedPtr());
                    const bslma::ManagedPtr<TObj>& Y=y;

                    ASSERT(REP == X.rep());
                    ASSERT(0 == numDeletes);
                    ASSERT(p == X.get());
                    ASSERT(2 == X.use_count());
                    ASSERT(p == Y.ptr());
                    ASSERT(2 + numAllocations == ta.numAllocations());
                    ASSERT(numDeallocations   == ta.numDeallocations());

                    // Give up original shared ownership, so 'y' is unique
                    // owner of shared object.

                    x.reset();
                    ASSERT(0 == X.rep());
                    ASSERT(0 == numDeletes);
                    ASSERT(0 == X.get());
                    ASSERT(0 == X.use_count());
                    ASSERT(p == Y.ptr());
                    ASSERT(2 + numAllocations == ta.numAllocations());
                    ASSERT(numDeallocations   == ta.numDeallocations());

                    // Transfer ownership to a new 'shared_ptr'.  No memory
                    // should be allocated.

                    bsls::ObjectBuffer<Obj> zBuf;
                    Obj& z = zBuf.object(); const Obj& Z = z;

                    switch (CONFIG) {
                      case 'a': {
                        new (zBuf.buffer()) Obj(y, &nullAllocator);
                      } break;
                      case 'b': {
                        bslma::DefaultAllocatorGuard dag(&nullAllocator);
                        (void)dag;
                        new (zBuf.buffer()) Obj();
                        z = y;
                      } break;
                      default: {
                        ASSERTV(CONFIG, !"Bad config.");
                        BSLS_ASSERT_INVOKE_NORETURN("Bad test config.");
                      } break;
                    }

                    ASSERT(REP == Z.rep());
                    ASSERT(  p == Z.get());
                    ASSERT(  1 == Z.use_count());

                    ASSERT(0 == X.rep());
                    ASSERT(0 == X.get());
                    ASSERT(0 == X.use_count());
                    ASSERT(0 == Y.ptr());

                    ASSERT(0 == numDeletes);
                    ASSERT(2 + numAllocations == ta.numAllocations());
                    ASSERT(numDeallocations   == ta.numDeallocations());

                    // Transfer ownership back out of the nested block.

                    x = MoveUtil::move(z);

                    zBuf.object().~Obj();
                }
                ASSERT(0 == numDeletes);
                ASSERT(p == X.get());
                ASSERT(1 == X.use_count());
                ASSERT(REP == X.rep());
                ASSERT(2 + numAllocations == ta.numAllocations());
                ASSERT(numDeallocations   == ta.numDeallocations());
            }
            ASSERTV(numDeletes, 1 == numDeletes);
            ASSERTV(2 + numDeallocations,   ta.numDeallocations(),
                    2 + numDeallocations == ta.numDeallocations());

            // Reset counters.

            numAllocations   = ta.numAllocations();
            numDeallocations = ta.numDeallocations();

            if (verbose) printf("\nTesting counted-null round-trip"
                                "\n-------------------------------\n");
            {
                TObj *p = 0;    // null ptr must be the right type to ref-count

                Obj x(p, &ta);      const Obj&  X   = x;
                bslma::SharedPtrRep const*const REP = X.rep();
                ASSERTV(1 + numAllocations,   ta.numAllocations(),
                        1 + numAllocations == ta.numAllocations());

                if (veryVerbose) {
                    T_ T_ P_(numDeletes); P(X.use_count());
                }
                ASSERT(p == X.get());
                ASSERT(1 == X.use_count());

                {
                    bslma::ManagedPtr<TObj> y(X.managedPtr());
                    const bslma::ManagedPtr<TObj>& Y=y;

                    ASSERT(REP == X.rep());
                    ASSERT(0 == X.get());
                    ASSERT(1 == X.use_count());
                    ASSERT(0 == Y.ptr());
                    ASSERT(1 + numAllocations == ta.numAllocations());
                    ASSERT(numDeallocations   == ta.numDeallocations());

                    // Transfer ownership to a new 'shared_ptr'.  No memory
                    // should be allocated.

                    bsls::ObjectBuffer<Obj> zBuf;
                    Obj& z = zBuf.object(); const Obj& Z = z;

                    switch (CONFIG) {
                      case 'a': {
                        new (zBuf.buffer()) Obj(y, &nullAllocator);
                      } break;
                      case 'b': {
                        bslma::DefaultAllocatorGuard dag(&nullAllocator);
                        (void)dag;
                        new (zBuf.buffer()) Obj();
                        z = y;
                      } break;
                      default: {
                        ASSERTV(CONFIG, !"Bad config.");
                        BSLS_ASSERT_INVOKE_NORETURN("Bad test config.");
                      } break;
                    }

                    ASSERT(0 == Z.rep()); // doesn't round trip null ownership
                    ASSERT(0 == Z.get());
                    ASSERT(0 == Z.use_count());

                    ASSERT(0 == Y.ptr());

                    ASSERT(1 + numAllocations == ta.numAllocations());
                    ASSERT(numDeallocations   == ta.numDeallocations());

                    zBuf.object().~Obj();
                }
                ASSERT(  p == X.get());
                ASSERT(  1 == X.use_count());
                ASSERT(REP == X.rep());

                ASSERT(1 + numAllocations == ta.numAllocations());
                ASSERT(numDeallocations   == ta.numDeallocations());
            }
            ASSERTV(numDeletes, 1 == numDeletes);
            ASSERTV(1 + numDeallocations,   ta.numDeallocations(),
                    1 + numDeallocations == ta.numDeallocations());

            // Reset counters.

            numAllocations   = ta.numAllocations();
            numDeallocations = ta.numDeallocations();

            if (verbose) printf("\nTesting empty non-null is supported"
                                "\n-----------------------------------\n");
            {
                numDeletes = 0;
                TObj dummy(&numDeletes);
                TObj *p = &dummy;

                Obj nullPointer;
                Obj x(nullPointer, p); const Obj& X = x;
                bslma::SharedPtrRep const*const REP = X.rep();

                ASSERTV(REP, 0 == REP);
                ASSERTV(numAllocations,   ta.numAllocations(),
                        numAllocations == ta.numAllocations());

                if (veryVerbose) {
                    T_ T_ P_(numDeletes); P(X.use_count());
                }
                ASSERT(0 == numDeletes);
                ASSERT(p == X.get());
                ASSERT(0 == X.use_count());

                {
                    bslma::ManagedPtr<TObj> y(X.managedPtr());
                    const bslma::ManagedPtr<TObj>& Y=y;

                    ASSERT(REP == X.rep());
                    ASSERT(0 == numDeletes);
                    ASSERT(p == X.get());
                    ASSERT(0 == X.use_count());
                    ASSERT(p == Y.ptr());
                    ASSERT(numAllocations   == ta.numAllocations());
                    ASSERT(numDeallocations == ta.numDeallocations());

                    // Give up original shared ownership, so 'y' is unique
                    // owner of shared object.

                    x.reset();
                    ASSERT(0 == X.rep());
                    ASSERT(0 == numDeletes);
                    ASSERT(0 == X.get());
                    ASSERT(0 == X.use_count());
                    ASSERT(p == Y.ptr());
                    ASSERT(numAllocations   == ta.numAllocations());
                    ASSERT(numDeallocations == ta.numDeallocations());

                    // Now release ownership, which should have no effect.

                    y.reset();
                    ASSERT(0 == X.rep());
                    ASSERT(0 == numDeletes);
                    ASSERT(0 == X.get());
                    ASSERT(0 == X.use_count());
                    ASSERT(0 == Y.ptr());
                    ASSERT(numAllocations   == ta.numAllocations());
                    ASSERT(numDeallocations == ta.numDeallocations());

                    // Transfer remains back out of the nested block.

                    x = y;
                }
                ASSERT(0 == numDeletes);
                ASSERT(0 == X.get());
                ASSERT(0 == X.use_count());
                ASSERT(0 == X.rep());
                ASSERT(numAllocations   == ta.numAllocations());
                ASSERT(numDeallocations == ta.numDeallocations());
            }
            ASSERTV(numDeletes, 1 == numDeletes);
            ASSERTV(numDeallocations,   ta.numDeallocations(),
                    numDeallocations == ta.numDeallocations());

            // Reset counters.

            numAllocations   = ta.numAllocations();
            numDeallocations = ta.numDeallocations();

            if (verbose) printf("\nTesting empty non-null round-trip"
                                "\n---------------------------------\n");
            {
                numDeletes = 0;
                TObj dummy(&numDeletes);
                TObj *p = &dummy;

                Obj nullPointer;
                Obj x(nullPointer, p); const Obj& X = x;
                bslma::SharedPtrRep const*const REP = X.rep();

                ASSERTV(REP, 0 == REP);
                ASSERTV(numAllocations,   ta.numAllocations(),
                        numAllocations == ta.numAllocations());

                if (veryVerbose) {
                    T_ T_ P_(numDeletes); P(X.use_count());
                }
                ASSERT(0 == numDeletes);
                ASSERT(p == X.get());
                ASSERT(0 == X.use_count());

                {
                    bslma::ManagedPtr<TObj> y(X.managedPtr());
                    const bslma::ManagedPtr<TObj>& Y=y;

                    ASSERT(REP == X.rep());
                    ASSERT(0 == numDeletes);
                    ASSERT(p == X.get());
                    ASSERT(0 == X.use_count());
                    ASSERT(p == Y.ptr());
                    ASSERT(numAllocations   == ta.numAllocations());
                    ASSERT(numDeallocations == ta.numDeallocations());

                    // Give up original shared ownership, so 'y' is unique
                    // owner of shared object.

                    x.reset();
                    ASSERT(0 == X.rep());
                    ASSERT(0 == numDeletes);
                    ASSERT(0 == X.get());
                    ASSERT(0 == X.use_count());
                    ASSERT(p == Y.ptr());
                    ASSERT(numAllocations   == ta.numAllocations());
                    ASSERT(numDeallocations == ta.numDeallocations());

                    // Transfer ownership to a new 'shared_ptr'.  No memory
                    // should be allocated.

                    bsls::ObjectBuffer<Obj> zBuf;
                    Obj& z = zBuf.object(); const Obj& Z = z;

                    switch (CONFIG) {
                      case 'a': {
                        new (zBuf.buffer()) Obj(y, &nullAllocator);
                      } break;
                      case 'b': {
                        bslma::DefaultAllocatorGuard dag(&nullAllocator);
                        (void)dag;
                        new (zBuf.buffer()) Obj();
                        z = y;
                      } break;
                      default: {
                        ASSERTV(CONFIG, !"Bad config.");
                        BSLS_ASSERT_INVOKE_NORETURN("Bad test config.");
                      } break;
                    }

                    ASSERT(REP == Z.rep());
                    ASSERT(  p == Z.get());
                    ASSERT(  0 == Z.use_count());

                    ASSERT(0 == X.rep());
                    ASSERT(0 == X.get());
                    ASSERT(0 == X.use_count());

                    ASSERT(0 == Y.ptr());

                    ASSERT(               0 == numDeletes);
                    ASSERT(numAllocations   == ta.numAllocations());
                    ASSERT(numDeallocations == ta.numDeallocations());

                    // Transfer ownership back out of the nested block.

                    x = MoveUtil::move(z);

                    zBuf.object().~Obj();
                }
                ASSERT(0 == numDeletes);
                ASSERT(p == X.get());
                ASSERT(0 == X.use_count());
                ASSERT(REP == X.rep());
                ASSERT(numAllocations   == ta.numAllocations());
                ASSERT(numDeallocations == ta.numDeallocations());
            }
            ASSERTV(numDeletes, 1 == numDeletes);
            ASSERTV(numDeallocations,   ta.numDeallocations(),
                    numDeallocations == ta.numDeallocations());

            ASSERT(dam.isTotalSame());
        }
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
        //   bool shared_ptr::owner_before(const shared_ptr<BDE_OTHER_TYPE>& r)
        //   bool shared_ptr::owner_before(const weak_ptr<BDE_OTHER_TYPE>& rhs)
        //   bool weak_ptr::owner_before(const shared_ptr<BDE_OTHER_TYPE>& rhs)
        //   bool weak_ptr::owner_before(const weak_ptr<BDE_OTHER_TYPE>& rhs)
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'owner_before' METHODS"
                            "\n==============================\n");

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
        //   TBD
        //
        // Testing:
        //   shared_ptr<TYPE> lock() const
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        //   shared_ptr<TYPE> acquireSharedPtr() const
#endif // BDE_OMIT_INTERNAL_DEPRECATED
        // --------------------------------------------------------------------

      if (verbose) printf("\nTESTING 'lock'"
                          "\n==============\n");

      // Confirm that the default allocator never allocates in this test case
      bslma::TestAllocatorMonitor dam(&defaultAllocator);

      if (verbose) printf("\ndefault-constructed 'weak_ptr'"
                          "\n------------------------------\n");
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

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
          ObjSP S2 = mX.acquireSharedPtr();
          ASSERT(X.expired());
          ASSERT(0 == X.rep());
          ASSERT(!S2);
          ASSERT(0 == S2.get());
          ASSERT(0 == S2.rep());
#endif // BDE_OMIT_INTERNAL_DEPRECATED
      }

      if (verbose) printf("\n'regular' shared and weak pointers"
                          "\n----------------------------------\n");
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
              ASSERTV(REP.numReferences(), 1 == REP.numReferences());

              ObjSP SA = X.lock();
              ASSERT(SA);
              ASSERT(PTR  == SA.get());
              ASSERT(&REP == SA.rep());
              ASSERTV(REP.numReferences(), 2 == REP.numReferences());

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
              ObjSP SB = Y.acquireSharedPtr();
#else
              ObjSP SB = Y.lock();
#endif // BDE_OMIT_INTERNAL_DEPRECATED
              ASSERT(SB);
              ASSERT(PTR  == SB.get());
              ASSERT(&REP == SB.rep());

              ASSERTV(REP.numReferences(), 3 == REP.numReferences());

              SC = X.lock();
              LOOP_ASSERT(REP.numReferences(), 4 == REP.numReferences());
              ASSERTV( PTR, SC.get(),  PTR == SC.get());
              ASSERTV(&REP, SC.rep(), &REP == SC.rep());
          }

          ASSERTV(REP.numReferences(),      1 == REP.numReferences());
          ASSERTV(REP.disposeRepCount(),    0 == REP.disposeRepCount());
          ASSERTV(REP.disposeObjectCount(), 0 == REP.disposeObjectCount());
      }

      // This test scenario might be a little neater if we used a shared object
      // that flagged when its destructor runs, or an instrumented custom
      // deleter.  Similarly, running through the scenario twice to track the
      // 'weak_ptr' to the aliased and non-aliased 'shared_ptr' would reduce
      // the risk of masking bugs when we extend the lifetime of the one 'rep'
      // object until all references are released.

      if (verbose) printf("\nweak-ref to aliased shared pointer"
                          "\n----------------------------------\n");
      {
          bslma::TestAllocator ta;

          bsl::shared_ptr<int> mOriginal;
          const bsl::shared_ptr<int>& ORIGINAL = mOriginal;
          mOriginal.createInplace(&ta, 42);
          bslma::SharedPtrRep const*const REP = ORIGINAL.rep();

          ASSERTV(ORIGINAL.use_count(), 1 == ORIGINAL.use_count());

          int target = 13;
          bsl::shared_ptr<int> mAlias(ORIGINAL, &target);
          const bsl::shared_ptr<int>& ALIAS = mAlias;

          ASSERTV(ORIGINAL.use_count(),  2 == ORIGINAL.use_count());
          ASSERTV(ALIAS.use_count(),     2 == ALIAS.use_count());
          ASSERTV(REP, ORIGINAL.rep(), REP == ORIGINAL.rep());
          ASSERTV(REP, ALIAS.rep(),    REP == ALIAS.rep());

          {
              bsl::weak_ptr<int> mX(ORIGINAL); const bsl::weak_ptr<int>& X=mX;
              bsl::weak_ptr<int> mY(ALIAS);    const bsl::weak_ptr<int>& Y=mY;
              ASSERTV(ORIGINAL.use_count(), 2  == ORIGINAL.use_count());
              ASSERTV(ALIAS.use_count(),    2  == ALIAS.use_count());
              ASSERTV(REP, ORIGINAL.rep(), REP == ORIGINAL.rep());
              ASSERTV(REP, ALIAS.rep(),    REP == ALIAS.rep());

              ASSERTV(X.use_count(), 2  == X.use_count());
              ASSERTV(Y.use_count(), 2  == Y.use_count());
              ASSERTV(REP, X.rep(), REP == X.rep());
              ASSERTV(REP, Y.rep(), REP == Y.rep());

              bsl::shared_ptr<int> lockOriginal = X.lock();
              ASSERTV(ORIGINAL.get() == lockOriginal.get());
              ASSERTV(ORIGINAL.rep() == lockOriginal.rep());
              ASSERTV(3 == lockOriginal.use_count());
              ASSERTV(3 == ORIGINAL.use_count());

              bsl::shared_ptr<int> lockAlias = Y.lock();
              ASSERTV(ALIAS.get() == lockAlias.get());
              ASSERTV(ALIAS.rep() == lockAlias.rep());
              ASSERTV(4 == lockAlias.use_count());
              ASSERTV(4 == ALIAS.use_count());

              mOriginal.reset();
              mAlias.reset();
              ASSERTV(2 == lockOriginal.use_count());
              ASSERTV(0 == ORIGINAL.use_count());
              ASSERTV(2 == lockAlias.use_count());
              ASSERTV(0 == ALIAS.use_count());
              ASSERTV(X.use_count(), 2 == X.use_count());
              ASSERTV(Y.use_count(), 2 == Y.use_count());

              // lock the weak-pointers again, cross-assigning the lock results
              // note that the reference count should not drop to zero for the
              // one control block, so even though there is no 'shared_ptr' to
              // '42' until 'lockAlias' is re-assigned, the shared 'rep' object
              // remains alive throughout.

              lockOriginal = Y.lock();
              lockAlias = X.lock();
              ASSERTV(*lockOriginal, 13 == *lockOriginal);
              ASSERTV(*lockAlias,    42 == *lockAlias);

              ASSERTV(2 == lockOriginal.use_count());
              ASSERTV(0 == ORIGINAL.use_count());
              ASSERTV(2 == lockAlias.use_count());
              ASSERTV(0 == ALIAS.use_count());
              ASSERTV(X.use_count(), 2 == X.use_count());
              ASSERTV(Y.use_count(), 2 == Y.use_count());

              // Confirm there has been exactly one memory allocation so far,
              // and nothing de-allocated

              ASSERT(1 == ta.numAllocations());
              ASSERT(0 == ta.numDeallocations());

              // reset the last two active 'shared_ptr's, releasing ownership
              // of the shared 'int'

              lockOriginal.reset();
              lockAlias.reset();
              ASSERTV(0 == lockOriginal.use_count());
              ASSERTV(0 == ORIGINAL.use_count());
              ASSERTV(0 == lockAlias.use_count());
              ASSERTV(0 == ALIAS.use_count());
              ASSERTV(X.use_count(), 0 == X.use_count());
              ASSERTV(Y.use_count(), 0 == Y.use_count());

              // Confirm that despite releasing final ownership of the 'int',
              // the 'weak_ptr' objects retain ownership of the 'rep' object
              // footprint

              ASSERT(1 == ta.numAllocations());
              ASSERT(0 == ta.numDeallocations());

              // locking expired 'weak_ptr' objects should not produce an empty
              // 'shared_ptr' aliasing the original values, but always a null
              // and empty 'shared_ptr'

              lockOriginal = X.lock();
              lockAlias = Y.lock();
              ASSERT(!ORIGINAL);
              ASSERT(!ORIGINAL.rep());
              ASSERT(!ALIAS);
              ASSERT(!ALIAS.rep());

              // finally, allow the 'rep' object memory to be reclaimed too
              mX.reset();
              mY.reset();

              ASSERT(1 == ta.numAllocations());
              ASSERT(1 == ta.numDeallocations());
          }
          // Confirm no double-deletes
          ASSERT(1 == ta.numAllocations());
          ASSERT(1 == ta.numDeallocations());
      }

      // This test scenario would benefit greatly from an instrumented deleter
      // separately tracking the end of shared ownership, while the allocator
      // tracks the end of the 'rep' object itself.

      if (verbose) printf("\nweak-ref to counted null"
                          "\n------------------------\n");
      {
          bslma::TestAllocator ta;

          int *p = 0;
          bsl::shared_ptr<int> mOriginal(p, &ta);
          const bsl::shared_ptr<int>& ORIGINAL = mOriginal;
          bslma::SharedPtrRep const*const REP = ORIGINAL.rep();

          ASSERTV(ORIGINAL.use_count(), 1 == ORIGINAL.use_count());

          int target = 13;
          bsl::shared_ptr<int> mAlias(ORIGINAL, &target);
          const bsl::shared_ptr<int>& ALIAS = mAlias;

          ASSERTV(ORIGINAL.use_count(),  2 == ORIGINAL.use_count());
          ASSERTV(ALIAS.use_count(),     2 == ALIAS.use_count());
          ASSERTV(REP, ORIGINAL.rep(), REP == ORIGINAL.rep());
          ASSERTV(REP, ALIAS.rep(),    REP == ALIAS.rep());

          {
              bsl::weak_ptr<int> mX(ORIGINAL); const bsl::weak_ptr<int>& X=mX;
              bsl::weak_ptr<int> mY(ALIAS);    const bsl::weak_ptr<int>& Y=mY;
              ASSERTV(ORIGINAL.use_count(), 2  == ORIGINAL.use_count());
              ASSERTV(ALIAS.use_count(),    2  == ALIAS.use_count());
              ASSERTV(REP, ORIGINAL.rep(), REP == ORIGINAL.rep());
              ASSERTV(REP, ALIAS.rep(),    REP == ALIAS.rep());

              ASSERTV(X.use_count(), 2  == X.use_count());
              ASSERTV(Y.use_count(), 2  == Y.use_count());
              ASSERTV(REP, X.rep(), REP == X.rep());
              ASSERTV(REP, Y.rep(), REP == Y.rep());

              bsl::shared_ptr<int> lockOriginal = X.lock();
              ASSERTV(ORIGINAL.get() == lockOriginal.get());
              ASSERTV(ORIGINAL.rep() == lockOriginal.rep());
              ASSERTV(3 == lockOriginal.use_count());
              ASSERTV(3 == ORIGINAL.use_count());

              bsl::shared_ptr<int> lockAlias = Y.lock();
              ASSERTV(ALIAS.get() == lockAlias.get());
              ASSERTV(ALIAS.rep() == lockAlias.rep());
              ASSERTV(4 == lockAlias.use_count());
              ASSERTV(4 == ALIAS.use_count());

              mOriginal.reset();
              mAlias.reset();
              ASSERTV(2 == lockOriginal.use_count());
              ASSERTV(0 == ORIGINAL.use_count());
              ASSERTV(2 == lockAlias.use_count());
              ASSERTV(0 == ALIAS.use_count());
              ASSERTV(X.use_count(), 2 == X.use_count());
              ASSERTV(Y.use_count(), 2 == Y.use_count());

              // lock the weak-pointers again, cross-assigning the lock results
              // note that the reference count should not drop to zero for the
              // one control block, so even though there is no 'shared_ptr' to
              // null until 'lockAlias' is re-assigned, the shared 'rep' object
              // remains alive throughout.

              lockOriginal = Y.lock();
              lockAlias = X.lock();
              ASSERTV(*lockOriginal, 13 == *lockOriginal);
              ASSERTV(lockAlias.get(), !lockAlias);

              ASSERTV(2 == lockOriginal.use_count());
              ASSERTV(0 == ORIGINAL.use_count());
              ASSERTV(2 == lockAlias.use_count());
              ASSERTV(0 == ALIAS.use_count());
              ASSERTV(X.use_count(), 2 == X.use_count());
              ASSERTV(Y.use_count(), 2 == Y.use_count());

              // Confirm there has been exactly one memory allocation so far,
              // and nothing de-allocated

              ASSERT(1 == ta.numAllocations());
              ASSERT(0 == ta.numDeallocations());

              // reset the last two active 'shared_ptr's, releasing ownership
              // of the shared null

              lockOriginal.reset();
              lockAlias.reset();
              ASSERTV(0 == lockOriginal.use_count());
              ASSERTV(0 == ORIGINAL.use_count());
              ASSERTV(0 == lockAlias.use_count());
              ASSERTV(0 == ALIAS.use_count());
              ASSERTV(X.use_count(), 0 == X.use_count());
              ASSERTV(Y.use_count(), 0 == Y.use_count());

              // Confirm that despite releasing final ownership of the null
              // pointer, the 'weak_ptr' objects retain ownership of the 'rep'
              // object footprint

              ASSERT(1 == ta.numAllocations());
              ASSERT(0 == ta.numDeallocations());

              // locking expired 'weak_ptr' objects should not produce an empty
              // 'shared_ptr' aliasing the original values, but always a null
              // and empty 'shared_ptr'

              lockOriginal = X.lock();
              lockAlias = Y.lock();
              ASSERT(!ORIGINAL);
              ASSERT(!ORIGINAL.rep());
              ASSERT(!ALIAS);
              ASSERT(!ALIAS.rep());

              // finally, allow the 'rep' object memory to be reclaimed too
              mX.reset();
              mY.reset();

              ASSERT(1 == ta.numAllocations());
              ASSERT(1 == ta.numDeallocations());
          }
          // Confirm no double-deletes
          ASSERT(1 == ta.numAllocations());
          ASSERT(1 == ta.numDeallocations());
      }

      if (verbose) printf("\nweak-ref to empty non-null 'shared_ptr'"
                          "\n---------------------------------------\n");
      {
          bsl::shared_ptr<int> mOriginal;
          const bsl::shared_ptr<int>& ORIGINAL = mOriginal;

          ASSERTV(ORIGINAL.use_count(), 0 == ORIGINAL.use_count());

          int target = 13;
          bsl::shared_ptr<int> mAlias(ORIGINAL, &target);
          const bsl::shared_ptr<int>& ALIAS = mAlias;

          ASSERTV(ORIGINAL.use_count(), 0 == ORIGINAL.use_count());
          ASSERTV(ALIAS.use_count(),    0 == ALIAS.use_count());
          ASSERTV(ORIGINAL.rep(),       0 == ORIGINAL.rep());
          ASSERTV(ALIAS.rep(),          0 == ALIAS.rep());

          {
              bsl::weak_ptr<int> mX(ORIGINAL); const bsl::weak_ptr<int>& X=mX;
              bsl::weak_ptr<int> mY(ALIAS);    const bsl::weak_ptr<int>& Y=mY;
              ASSERTV(ORIGINAL.use_count(), 0 == ORIGINAL.use_count());
              ASSERTV(ALIAS.use_count(),    0 == ALIAS.use_count());
              ASSERTV(ORIGINAL.rep(),       0 == ORIGINAL.rep());
              ASSERTV(ALIAS.rep(),          0 == ALIAS.rep());

              ASSERTV(X.use_count(), 0 == X.use_count());
              ASSERTV(Y.use_count(), 0 == Y.use_count());
              ASSERTV(X.rep(),       0 == X.rep());
              ASSERTV(Y.rep(),       0 == Y.rep());

              bsl::shared_ptr<int> lockOriginal = X.lock();
              ASSERTV(lockOriginal.get(),       0 == lockOriginal.get());
              ASSERTV(lockOriginal.rep(),       0 == lockOriginal.rep());
              ASSERTV(lockOriginal.use_count(), 0 == lockOriginal.use_count());
              ASSERTV(ORIGINAL.use_count(),     0 == ORIGINAL.use_count());

              bsl::shared_ptr<int> lockAlias = Y.lock();
              ASSERTV(lockAlias.get(),       0 == lockAlias.get());
              ASSERTV(lockAlias.rep(),       0 == lockAlias.rep());
              ASSERTV(lockAlias.use_count(), 0 == lockAlias.use_count());
              ASSERTV(ALIAS.use_count(),     0 == ALIAS.use_count());
          }
      }

      ASSERT(dam.isTotalSame());
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
      // ----------------------------------------------------------------------
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
      //   weak_ptr& operator=(weak_ptr&& rhs) noexcept
      //   weak_ptr& operator=(weak_ptr<COMPATIBLE_TYPE>&& rhs) noexcept
      //   weak_ptr& operator=(const weak_ptr& rhs) noexcept
      //   weak_ptr& operator=(const weak_ptr<COMPATIBLE_TYPE>& rhs) noexcept
      //   weak_ptr& operator=(const shared_ptr<COMPATIBLE_TYPE>& rhs) noexcept
      // ----------------------------------------------------------------------

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

          LOOP_ASSERT(REP1.numReferences(),   0 == REP1.numReferences());
          LOOP_ASSERT(REP1.disposeRepCount(), 1 == REP1.disposeRepCount());
          LOOP_ASSERT(REP1.disposeObjectCount(),
                                              1 == REP1.disposeObjectCount());

          LOOP_ASSERT(REP2.numReferences(),   0 == REP2.numReferences());
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
        //   weak_ptr() noexcept
        //   weak_ptr(const shared_ptr<COMPATIBLE_TYPE>& other) noexcept
        //   weak_ptr(const weak_ptr& original) noexcept
        //   weak_ptr(const weak_ptr<COMPATIBLE_TYPE>& other) noexcept
        //   weak_ptr(weak_ptr&& original) noexcept
        //   weak_ptr(weak_ptr<COMPATIBLE_TYPE>&& other) noexcept
        //   ~weak_ptr()
        //   bslma::SharedPtrRep *rep() const
        //   bool expired() const
        //   long use_count() const
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        //   int numReferences() const
#endif // BDE_OMIT_INTERNAL_DEPRECATED
        // --------------------------------------------------------------------

      if (verbose) printf("\nTESTING CREATORS AND ACCESSORS ('weak_ptr')"
                          "\n===========================================\n");

      if (verbose) printf("\nTesting default constructor"
                          "\n---------------------------\n");
      {
          ObjWP mX; const ObjWP& X = mX;
          ASSERT(X.expired());
          ASSERT(0 == X.rep());
          ASSERT(0 == X.use_count());
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
          ASSERT(X.use_count() == X.numReferences());
#endif // BDE_OMIT_INTERNAL_DEPRECATED

          ObjSP mS1 = X.lock(); const ObjSP& S1 = mS1;
          ASSERT(!S1);
          ASSERT(0 == S1.rep());
          ASSERT(0 == S1.use_count());

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
          ObjSP mS2 = X.acquireSharedPtr(); const ObjSP& S2 = mS2;
          ASSERT(!S2);
          ASSERT(0 == S2.rep());
          ASSERT(0 == S2.get());
#endif // BDE_OMIT_INTERNAL_DEPRECATED
      }

      if (verbose) printf("\nTesting copy of the default"
                          "\n---------------------------\n");
      {
          ObjWP mX;       const ObjWP& X = mX;

          ObjWP mY = X;   const ObjWP& Y = mY;
          ASSERT(Y.expired());
          ASSERT(0 == Y.rep());
          ASSERT(0 == Y.use_count());
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
          ASSERT(Y.use_count() == Y.numReferences());
#endif // BDE_OMIT_INTERNAL_DEPRECATED
      }

      if (verbose) printf("\nTesting move of the default"
                          "\n---------------------------\n");
      {
          ObjWP mX;

          ObjWP mY(MoveUtil::move(mX)); const ObjWP& Y = mY;
          ASSERT(Y.expired());
          ASSERT(0 == Y.rep());
          ASSERT(0 == Y.use_count());
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
          ASSERT(Y.use_count() == Y.numReferences());
#endif // BDE_OMIT_INTERNAL_DEPRECATED
      }

      if (verbose) printf("\nTesting same TYPE constructors"
                          "\n-------------------------------\n");
      {
          bslma::TestAllocator ta;
          MyTestObject *const REP_PTR = new(ta) MyTestObject(&numDeletes);

          TestSharedPtrRep<MyTestObject> rep(REP_PTR, &ta);
          const TestSharedPtrRep<MyTestObject>& REP = rep;

          ASSERTV(REP_PTR, REP.ptr(), REP_PTR == REP.ptr());
          ASSERTV(REP.numReferences(),      1 == REP.numReferences());
          ASSERTV(REP.numWeakReferences(),  0 == REP.numWeakReferences());
          ASSERTV(REP.disposeRepCount(),    0 == REP.disposeRepCount());
          ASSERTV(REP.disposeObjectCount(), 0 == REP.disposeObjectCount());

          MyTestObject *PTR = REP.ptr();
          {
              ObjSP mS(PTR, &rep);
              const ObjSP& S = mS;
              ASSERTV(REP.numReferences(),      1 == REP.numReferences());
              ASSERTV(REP.numWeakReferences(),  0 == REP.numWeakReferences());
              ASSERTV(REP.disposeRepCount(),    0 == REP.disposeRepCount());
              ASSERTV(REP.disposeObjectCount(), 0 == REP.disposeObjectCount());

              ObjWP mX(S); const ObjWP& X = mX;   // testing this constructor

              ASSERT(!X.expired());
              ASSERT(&REP == X.rep());
              ASSERTV(X.use_count(),            1 == X.use_count());
              ASSERTV(REP.numReferences(),      1 == REP.numReferences());
              ASSERTV(REP.numWeakReferences(),  1 == REP.numWeakReferences());
              ASSERTV(REP.disposeRepCount(),    0 == REP.disposeRepCount());
              ASSERTV(REP.disposeObjectCount(), 0 == REP.disposeObjectCount());
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
              ASSERTV(X.use_count(),   X.numReferences(),
                      X.use_count() == X.numReferences());
#endif // BDE_OMIT_INTERNAL_DEPRECATED

              ObjWP mY(X); const ObjWP& Y = mY;   // testing this constructor

              ASSERT(!Y.expired());
              ASSERT(&REP == Y.rep());
              ASSERTV(Y.use_count(),            1 == Y.use_count());
              ASSERTV(X.use_count(),            1 == X.use_count());

              ASSERTV(REP.numReferences(),      1 == REP.numReferences());
              ASSERTV(REP.numWeakReferences(),  2 == REP.numWeakReferences());
              ASSERTV(REP.disposeRepCount(),    0 == REP.disposeRepCount());
              ASSERTV(REP.disposeObjectCount(), 0 == REP.disposeObjectCount());

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
              ASSERTV(Y.use_count(),   Y.numReferences(),
                      Y.use_count() == Y.numReferences());
              ASSERTV(X.use_count(),   X.numReferences(),
                      X.use_count() == X.numReferences());
#endif // BDE_OMIT_INTERNAL_DEPRECATED
          }
          ASSERTV(REP.numReferences(),      0 == REP.numReferences());
          ASSERTV(REP.numWeakReferences(),  0 == REP.numWeakReferences());
          ASSERTV(REP.disposeRepCount(),    1 == REP.disposeRepCount());
          ASSERTV(REP.disposeObjectCount(), 1 == REP.disposeObjectCount());
      }

      if (verbose) printf("\nTesting compatible TYPE constructors"
                          "\n------------------------------------\n");
      {
          bslma::TestAllocator ta;
          MyTestDerivedObject *const REP_PTR =
                                      new(ta) MyTestDerivedObject(&numDeletes);

          TestSharedPtrRep<MyTestDerivedObject> rep(REP_PTR, &ta);
          const TestSharedPtrRep<MyTestDerivedObject>& REP = rep;

          ASSERTV(REP_PTR, REP.ptr(), REP_PTR == REP.ptr());
          ASSERTV(REP.numReferences(),      1 == REP.numReferences());
          ASSERTV(REP.numWeakReferences(),  0 == REP.numWeakReferences());
          ASSERTV(REP.disposeRepCount(),    0 == REP.disposeRepCount());
          ASSERTV(REP.disposeObjectCount(), 0 == REP.disposeObjectCount());

          MyTestDerivedObject *PTR = REP.ptr();
          {
              DerivedSP mS(PTR, &rep);
              const DerivedSP& S = mS;
              ASSERTV(REP.numReferences(),      1 == REP.numReferences());
              ASSERTV(REP.numWeakReferences(),  0 == REP.numWeakReferences());
              ASSERTV(REP.disposeRepCount(),    0 == REP.disposeRepCount());
              ASSERTV(REP.disposeObjectCount(), 0 == REP.disposeObjectCount());

              ObjWP mX(S); const ObjWP& X = mX;   // testing this constructor
              ASSERT(!X.expired());
              ASSERT(&REP == X.rep());
              ASSERTV(X.use_count(),            1 == X.use_count());
              ASSERTV(REP.numReferences(),      1 == REP.numReferences());
              ASSERTV(REP.numWeakReferences(),  1 == REP.numWeakReferences());
              ASSERTV(REP.disposeRepCount(),    0 == REP.disposeRepCount());
              ASSERTV(REP.disposeObjectCount(), 0 == REP.disposeObjectCount());

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
              ASSERTV(X.use_count(),   X.numReferences(),
                      X.use_count() == X.numReferences());
#endif // BDE_OMIT_INTERNAL_DEPRECATED

              DerivedWP mZ(S); const DerivedWP& Z = mZ;
              ObjWP mY(Z); const ObjWP& Y = mY;   // testing this constructor
              ASSERT(!Y.expired());
              ASSERT(&REP == Y.rep());
              ASSERTV(Y.use_count(),            1 == Y.use_count());
              ASSERTV(X.use_count(),            1 == X.use_count());

              ASSERTV(REP.numReferences(),      1 == REP.numReferences());
              ASSERTV(REP.numWeakReferences(),  3 == REP.numWeakReferences());
              ASSERTV(REP.disposeRepCount(),    0 == REP.disposeRepCount());
              ASSERTV(REP.disposeObjectCount(), 0 == REP.disposeObjectCount());

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
              ASSERTV(Y.use_count(),   Y.numReferences(),
                      Y.use_count() == Y.numReferences());
              ASSERTV(X.use_count(),   X.numReferences(),
                      X.use_count() == X.numReferences());
#endif // BDE_OMIT_INTERNAL_DEPRECATED
          }
          ASSERTV(REP.numReferences(),      0 == REP.numReferences());
          ASSERTV(REP.numWeakReferences(),  0 == REP.numWeakReferences());
          ASSERTV(REP.disposeRepCount(),    1 == REP.disposeRepCount());
          ASSERTV(REP.disposeObjectCount(), 1 == REP.disposeObjectCount());
      }

      if (verbose) printf("\nTesting destructor"
                          "\n------------------\n");
      {
          bslma::TestAllocator ta;
          MyTestObject *const REP_PTR = new(ta) MyTestObject(&numDeletes);

          TestSharedPtrRep<MyTestObject> rep(REP_PTR, &ta);
          const TestSharedPtrRep<MyTestObject>& REP = rep;

          ASSERTV(REP_PTR, REP.ptr(), REP_PTR == REP.ptr());
          ASSERTV(REP.numReferences(),      1 == REP.numReferences());
          ASSERTV(REP.numWeakReferences(),  0 == REP.numWeakReferences());
          ASSERTV(REP.disposeRepCount(),    0 == REP.disposeRepCount());
          ASSERTV(REP.disposeObjectCount(), 0 == REP.disposeObjectCount());

          MyTestObject *PTR = REP.ptr();
          {
              ObjWP mY;   const ObjWP& Y = mY;
              (void) Y;   // Suppress 'unused variable' warning
              {
                  ObjSP mS(PTR, &rep);    const ObjSP& S = mS;
                  ASSERTV(REP.numReferences(), 1 == REP.numReferences());
                  ASSERTV(REP.numWeakReferences(),
                                               0 == REP.numWeakReferences());
                  ASSERTV(REP.disposeRepCount(),
                                               0 == REP.disposeRepCount());
                  ASSERTV(REP.disposeObjectCount(),
                                               0 == REP.disposeObjectCount());

                  {
                      ObjWP mX(S);    const ObjWP& X = mX;
                      (void) X;       // Suppress 'unused variable' warning
                      ASSERTV(REP.numReferences(),
                                               1 == REP.numReferences());
                      ASSERTV(REP.numWeakReferences(),
                                               1 == REP.numWeakReferences());
                      ASSERTV(REP.disposeRepCount(),
                                               0 == REP.disposeRepCount());
                      ASSERTV(REP.disposeObjectCount(),
                                               0 == REP.disposeObjectCount());
                  }

                  ASSERTV(REP.numReferences(), 1 == REP.numReferences());
                  ASSERTV(REP.numWeakReferences(),
                                               0 == REP.numWeakReferences());
                  ASSERTV(REP.disposeRepCount(),
                                               0 == REP.disposeRepCount());
                  ASSERTV(REP.disposeObjectCount(),
                                               0 == REP.disposeObjectCount());

                  mY = S;    // NOT YET TESTED
                  ASSERTV(REP.numReferences(), 1 == REP.numReferences());
                  ASSERTV(REP.numWeakReferences(),
                                               1 == REP.numWeakReferences());
                  ASSERTV(REP.disposeRepCount(),
                                               0 == REP.disposeRepCount());
                  ASSERTV(REP.disposeObjectCount(),
                                               0 == REP.disposeObjectCount());
              }

              ASSERTV(REP.numReferences(),      0 == REP.numReferences());
              ASSERTV(REP.numWeakReferences(),  1 == REP.numWeakReferences());
              ASSERTV(REP.disposeRepCount(),    0 == REP.disposeRepCount());
              ASSERTV(REP.disposeObjectCount(), 1 == REP.disposeObjectCount());
          }

          ASSERTV(REP.numReferences(),      0 == REP.numReferences());
          ASSERTV(REP.numWeakReferences(),  0 == REP.numWeakReferences());
          ASSERTV(REP.disposeRepCount(),    1 == REP.disposeRepCount());
          ASSERTV(REP.disposeObjectCount(), 1 == REP.disposeObjectCount());
      }
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING 'createInplace'
        //
        // Concerns:
        //  TBD
        //
        // Plan:
        //  TBD
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

        bsl::shared_ptr<const MyInplaceAllocatableObject> x;
        Harness::testCase23(x);

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

      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING 'bsl::shared_ptr<cv-void>' (DRQS 33549823)
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
            printf("\nTESTING 'bsl::shared_ptr<cv-void>' (DRQS 33549823)"
                   "\n==================================================\n");

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
        // Note that the current failures occur only in the out-of-place rep
        // type.

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
        //   3) Can convert from rvalues (function results) as well as from
        //      lvalue.
        //
        //   4) The target shared pointer can point to the same type as the
        //      managed pointer, or to a base class.
        //
        //   5) No memory is allocated when simply transferring ownership from
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
            // Direct initialization
            using bslma::ManagedPtrUtil;
            bsl::shared_ptr<bsls::Types::Int64> sp1(
                      ManagedPtrUtil::allocateManaged<bsls::Types::Int64>(&ta,
                                                                          13));

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
            // Copy initialization from rvalue does not work on AIX currently
            bsl::shared_ptr<bsls::Types::Int64> sp2 =
                       ManagedPtrUtil::allocateManaged<bsls::Types::Int64>(&ta,
                                                                            2);
#endif

            // Copy initialization from lvalues works on all compilers
            bslma::ManagedPtr<bsls::Types::Int64> mp3 =
                       ManagedPtrUtil::allocateManaged<bsls::Types::Int64>(&ta,
                                                                            3);
            bsl::shared_ptr<bsls::Types::Int64> sp3 = mp3;
        }

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
        // Concerns:
        //  TBD
        //
        // Plan:
        //  TBD
        //
        // Testing:
        //  shared_ptr<TARGET> const_pointer_cast(const shared_ptr<SRC>& ptr)
        //  shared_ptr<TARGET> dynamic_pointer_cast(const shared_ptr<SRC>& ptr)
        //  shared_ptr<TARGET> static_pointer_cast(const shared_ptr<SRC>& ptr)
        //  shared_ptr<TARGET> reinterpret_pointer_cast(const shared_ptr<SRC>&)
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

        if (verbose) printf("\nTesting 'reinterpret_pointer_cast'"
                            "\n---------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            MyTestDerivedObject *p = new(ta) MyTestDerivedObject(&numDeletes);
            bsl::shared_ptr<MyTestDerivedObject> x(p, &ta, 0);
            const bsl::shared_ptr<MyTestDerivedObject>& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(p == X.get());
            ASSERT(1 == X.use_count());

            typedef bsl::shared_ptr<MyTestObject2> REObj;
            REObj        y(bsl::reinterpret_pointer_cast<MyTestObject2>(X));
            const REObj& Y=y;

            ASSERT(reinterpret_cast<MyTestObject2*>(p) == Y.get());
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
        //  This appears to be properly the concern of the out-of-place rep
        //  component's test driver, and should be removed from here.
        //
        // Concerns:
        //: 1 'bslma::SharedPtrOutofplaceRep' passes allocator to the deleter's
        //:   constructor.
        //
        // Plan:
        //  TBD
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
        //:   'SharedPtrRep' : that has not yet released its ownership of the
        //:   reference held by the 'shared_ptr' object prior to calling
        //:   'release'.
        //: 3 'first' has the correct value when the 'shared_ptr' is aliasing a
        //:   completely unrelated type and data structure.
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
        // Plan:
        //   Create shared pointers with various representations, release them
        //   (getting back a pointer to the representation object) and assert
        //   that the 'originalPtr' of that representation is identical to the
        //   address of the managed object.
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
        // Concerns:
        //   Implicit conversion to 'bool' are troublesome, because a 'bool' is
        //   further convertible to integral types, and thus such conversions
        //   should be banned.  Specifically, we are concerned that
        //     o SharedPtr can be used in "boolean" contexts such as 'if (p)',
        //       'if (!p)', 'if
        //     o SharedPtr cannot be converted to an 'int'.
        //     o SharedPtr returned by a function (as a temporary) does not
        //       lead to erroneous bool value (DRQS 12252806).
        //
        // Plan:
        //   We test the conversion in a variety of "boolean" contexts and
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
        ASSERT(  ptrNil != ptr1);

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
        if (verbose) printf("Should not compile.\n\n");

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
        ASSERT(2 == ptr1);  printf("r on CC, error on x.\n\n");
#endif

      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING 'get_deleter'
        //
        // Concerns:
        //
        // Plan:
        //   For 'get_deleter' we would need to test that
        //   'get_deleter' of objects created empty, or with the default or
        //   test allocator, a factory, or function-like deleters does return a
        //   pointer to the deleter if the correct type is passed as template
        //   argument of 'get_deleter' and 0 otherwise.
        //
        // Testing:
        //   DELETER *get_deleter(const shared_ptr<ELEMENT_TYPE>&)
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'get_deleter'"
                            "\n=====================\n");

        int    x = 42;
        double y = 3.14;

        support::TypedFactory<double> factoryDeleter = {};

        bsl::shared_ptr<int> pX(&x, bslstl::SharedPtrNilDeleter());
        ASSERT(0 != bsl::get_deleter<bslstl::SharedPtrNilDeleter>(pX));
        ASSERT(0 == bsl::get_deleter<support::TypedFactory<double> *>(pX));

        bsl::shared_ptr<void> pY;
        ASSERT(0 == bsl::get_deleter<bslstl::SharedPtrNilDeleter>(pY));
        ASSERT(0 == bsl::get_deleter<support::TypedFactory<double> *>(pY));

        pY.reset(&y, &factoryDeleter);
        support::TypedFactory<double> **pDeleter =
                         bsl::get_deleter<support::TypedFactory<double> *>(pY);
        ASSERT(0 == bsl::get_deleter<bslstl::SharedPtrNilDeleter>(pY));
        ASSERT(pDeleter && (&factoryDeleter == *pDeleter));

        pX.loadAlias(pY, &x);
        pDeleter = bsl::get_deleter<support::TypedFactory<double> *>(pX);
        ASSERT(0 == bsl::get_deleter<bslstl::SharedPtrNilDeleter>(pX));
        ASSERT(pDeleter && (&factoryDeleter == *pDeleter));

        if (verbose) printf(
               "Confirm that 'pY.reset()' has no effect on the 'pX' deleter.");
        pY.reset();
        pDeleter = bsl::get_deleter<support::TypedFactory<double> *>(pX);
        ASSERT(pDeleter && (&factoryDeleter == *pDeleter));
        ASSERT(0 == bsl::get_deleter<bslstl::SharedPtrNilDeleter>(pX));
        ASSERT(0 == bsl::get_deleter<bslstl::SharedPtrNilDeleter>(pY));
        ASSERT(0 == bsl::get_deleter<support::TypedFactory<double> *>(pY));
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING ALIAS OPERATIONS
        //
        // Concerns:
        //  TBD
        //
        // Plan:
        //  TBD
        //
        // Testing:
        //   void loadAlias(const bsl::shared_ptr<ANY_TYPE>&, ELEMENT_TYPE *)
        //   void reset(const shared_ptr<OTHER>& source, TYPE *ptr)
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ALIAS OPERATIONS"
                            "\n========================\n");

        if (verbose) printf("\nTesting 'loadAlias' (unset target)"
                            "\n----------------------------------\n");

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

        if (verbose) printf("\nTesting 'reset' (unset target)"
                            "\n------------------------------\n");

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
            y.reset(X, &dummy);
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

        if (verbose) printf("\nTesting 'reset' (unset target and src)"
                            "\n--------------------------------------\n");
        {
            bsl::shared_ptr<MyTestObject2> x;
            const bsl::shared_ptr<MyTestObject2>& X=x;

            ASSERT(0 == X.get());
            ASSERT(0 == X.use_count());

            bsl::shared_ptr<double> y; const bsl::shared_ptr<double>& Y=y;

            double dummy;
            y.reset(X, &dummy);

            ASSERT(&dummy == Y.get());
            ASSERT(0 == Y.use_count());
        }

        if (verbose) printf("\nTesting 'reset' (partially unset)"
                            "\n---------------------------------\n");

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

            y.reset(X, static_cast<double *>(0));
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

        if (verbose) printf("\nTesting 'reset'(set)"
                            "\n--------------------\n");

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
            z.reset(Y, &dummy2);
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
        // Concerns:
        //  TBD
        //
        // Plan:
        //  TBD
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
        // Concerns:
        //  TBD
        //
        // Plan:
        //  TBD
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


        if (verbose) printf("\nRetesting constructors with strange aliases"
                            "\n-------------------------------------------\n");

        // Now that we can create obscure 'shared_ptr' states with the aliasing
        // constructor, verify that passing aliased shared pointers do not
        // cause surprising problems when passed to already-tested constructors
        // that take 'shared_ptr' parameters,

        // First, alias a null-pointer of a different type
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

            // Make a copy of the same kind of 'shared_ptr'
            {
                bsl::shared_ptr<double> z(Y);
                const bsl::shared_ptr<double>& Z = z;

                ASSERTV(Z.get(),       0 == Z.get());
                ASSERTV(X.use_count(), 3 == X.use_count());
                ASSERTV(Y.use_count(), 3 == Y.use_count());
                ASSERTV(Z.use_count(), 3 == Z.use_count());
                ASSERT(numAllocations   == ta.numAllocations());
                ASSERT(numDeallocations == ta.numDeallocations());
            }
            ASSERTV(X.use_count(), 2 == X.use_count());
            ASSERTV(Y.use_count(), 2 == Y.use_count());
            ASSERT(numAllocations   == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());

            // Move a 'shared_ptr' of the same kind
            {
                bsl::shared_ptr<double> temp(Y);
                bsl::shared_ptr<double> z(MoveUtil::move(temp));
                const bsl::shared_ptr<double>& Z = z;

                ASSERTV(temp.get(),       0 == temp.use_count());
                ASSERTV(temp.use_count(), 0 == temp.use_count());

                ASSERTV(Z.get(),       0 == Z.get());
                ASSERTV(X.use_count(), 3 == X.use_count());
                ASSERTV(Y.use_count(), 3 == Y.use_count());
                ASSERTV(Z.use_count(), 3 == Z.use_count());
                ASSERT(numAllocations   == ta.numAllocations());
                ASSERT(numDeallocations == ta.numDeallocations());
            }
            ASSERTV(X.use_count(), 2 == X.use_count());
            ASSERTV(Y.use_count(), 2 == Y.use_count());
            ASSERT(numAllocations   == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());

            // Make a copy of a pointer-compatible 'shared_ptr'
            {
                bsl::shared_ptr<void> z(Y);
                const bsl::shared_ptr<void>& Z = z;

                ASSERTV(Z.get(),       0 == Z.get());
                ASSERTV(X.use_count(), 3 == X.use_count());
                ASSERTV(Y.use_count(), 3 == Y.use_count());
                ASSERTV(Z.use_count(), 3 == Z.use_count());
                ASSERT(numAllocations   == ta.numAllocations());
                ASSERT(numDeallocations == ta.numDeallocations());
            }
            ASSERTV(X.use_count(), 2 == X.use_count());
            ASSERTV(Y.use_count(), 2 == Y.use_count());
            ASSERT(numAllocations   == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());

            // Move a pointer-compatible 'shared_ptr'
            {
                bsl::shared_ptr<double> temp(Y);
                bsl::shared_ptr<void> z(MoveUtil::move(temp));
                const bsl::shared_ptr<void>& Z = z;

                ASSERTV(temp.get(),       0 == temp.use_count());
                ASSERTV(temp.use_count(), 0 == temp.use_count());

                ASSERTV(Z.get(),       0 == Z.get());
                ASSERTV(X.use_count(), 3 == X.use_count());
                ASSERTV(Y.use_count(), 3 == Y.use_count());
                ASSERTV(Z.use_count(), 3 == Z.use_count());
                ASSERT(numAllocations   == ta.numAllocations());
                ASSERT(numDeallocations == ta.numDeallocations());
            }
            ASSERTV(X.use_count(), 2 == X.use_count());
            ASSERTV(Y.use_count(), 2 == Y.use_count());
            ASSERT(numAllocations   == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());

            ASSERT(0 == numDeletes);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
        ASSERT(1 == numDeletes);

        // Second, alias a non-null-pointer without ownership

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

            // Make a copy of the same kind of 'shared_ptr'
            {
                bsl::shared_ptr<double> z(Y);
                const bsl::shared_ptr<double>& Z = z;

                ASSERTV(&dummy, Z.get(), &dummy == Z.get());
                ASSERTV(X.use_count(), 0 == X.use_count());
                ASSERTV(Y.use_count(), 0 == Y.use_count());
                ASSERTV(Z.use_count(), 0 == Z.use_count());
                ASSERT(numAllocations   == ta.numAllocations());
                ASSERT(numDeallocations == ta.numDeallocations());
            }
            ASSERTV(X.use_count(), 0 == X.use_count());
            ASSERTV(Y.use_count(), 0 == Y.use_count());
            ASSERT(numAllocations   == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());

            // Move a 'shared_ptr' of the same kind
            {
                bsl::shared_ptr<double> temp(Y);
                bsl::shared_ptr<double> z(MoveUtil::move(temp));
                const bsl::shared_ptr<double>& Z = z;

                ASSERTV(temp.get(),       0 == temp.use_count());
                ASSERTV(temp.use_count(), 0 == temp.use_count());

                ASSERTV(&dummy, Z.get(), &dummy == Z.get());
                ASSERTV(X.use_count(), 0 == X.use_count());
                ASSERTV(Y.use_count(), 0 == Y.use_count());
                ASSERTV(Z.use_count(), 0 == Z.use_count());
                ASSERT(numAllocations   == ta.numAllocations());
                ASSERT(numDeallocations == ta.numDeallocations());
            }
            ASSERTV(X.use_count(), 0 == X.use_count());
            ASSERTV(Y.use_count(), 0 == Y.use_count());
            ASSERT(numAllocations   == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());

            // Make a copy of a pointer-compatible 'shared_ptr'
            {
                bsl::shared_ptr<void> z(Y);
                const bsl::shared_ptr<void>& Z = z;

                ASSERTV(&dummy, Z.get(), &dummy == Z.get());
                ASSERTV(X.use_count(), 0 == X.use_count());
                ASSERTV(Y.use_count(), 0 == Y.use_count());
                ASSERTV(Z.use_count(), 0 == Z.use_count());
                ASSERT(numAllocations   == ta.numAllocations());
                ASSERT(numDeallocations == ta.numDeallocations());
            }
            ASSERTV(X.use_count(), 0 == X.use_count());
            ASSERTV(Y.use_count(), 0 == Y.use_count());
            ASSERT(numAllocations   == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());

            // Move a pointer-compatible 'shared_ptr'
            {
                bsl::shared_ptr<double> temp(Y);
                bsl::shared_ptr<void> z(MoveUtil::move(temp));
                const bsl::shared_ptr<void>& Z = z;

                ASSERTV(temp.get(),       0 == temp.use_count());
                ASSERTV(temp.use_count(), 0 == temp.use_count());

                ASSERTV(&dummy, Z.get(), &dummy == Z.get());
                ASSERTV(X.use_count(), 0 == X.use_count());
                ASSERTV(Y.use_count(), 0 == Y.use_count());
                ASSERTV(Z.use_count(), 0 == Z.use_count());
                ASSERT(numAllocations   == ta.numAllocations());
                ASSERT(numDeallocations == ta.numDeallocations());
            }
            ASSERTV(X.use_count(), 0 == X.use_count());
            ASSERTV(Y.use_count(), 0 == Y.use_count());
            ASSERT(numAllocations   == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());

            ASSERT(0 == numDeletes);
        }
        ASSERT(numDeallocations == ta.numDeallocations());


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
        // Concerns:
        //   TBD
        //
        // Plan:
        //   For every size between 1 and 5 times the maximal alignment, create
        //   a shared pointer to a buffer of this size using
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
        // Concerns:
        //  TBD
        //
        // Plan:
        //  TBD
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
        // Concerns:
        //  TBD
        //
        // Plan:
        //  TBD
        //
        // Testing:
        //   shared_ptr& operator=(const shared_ptr& rhs) noexcept
        //   shared_ptr& operator=(shared_ptr&& rhs) noexcept
        //   shared_ptr& operator=(const shared_ptr<OTHER>& rhs) noexcept
        //   shared_ptr& operator=(shared_ptr<OTHER>&& rhs) noexcept
        //   shared_ptr& operator=(bslma::ManagedPtr<OTHER> rhs)
        //   shared_ptr& operator=(std::auto_ptr<OTHER> rhs)
        //   shared_ptr& operator=(std::unique_ptr<OTHER, DELETER>&& rhs)
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ASSIGNMENT OPERATORS"
                            "\n============================\n");

        if (verbose) printf("\nTesting COPY-ASSIGNMENT to empty object"
                            "\n---------------------------------------\n");
        {
            Obj x1;
            const Obj& X1 = x1;
            ASSERT(0 == x1.get());
            ASSERT(0 == x1.use_count());

            numDeletes = 0;
            TObj *p = new TObj(&numDeletes);

            Obj x2(p); const Obj& X2 = x2;

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
                P(X2.use_count());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X2.get());
            ASSERT(1 == X2.use_count());

            x1 = X2;
            ASSERT_NOEXCEPT(x1 = X2);

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
                P(X2.use_count());
            }
            ASSERT(0 == numDeletes);

            ASSERT(X1.rep() == X2.rep());
            ASSERT(X1.get() == X2.get());

            ASSERT(p == X2.get());
            ASSERT(p == X1.get());

            ASSERT(2 == X2.use_count());
            ASSERT(2 == X1.use_count());
        }
        ASSERT(1 == numDeletes);

        if (verbose) printf("\nTesting COPY-ASSIGNMENT of empty object"
                            "\n---------------------------------------\n");
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
            ASSERT_NOEXCEPT(x1 = X2);

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
                P(X2.use_count());
            }
            ASSERT(1 == numDeletes);

            ASSERT(X1.rep() == X2.rep());
            ASSERT(X1.get() == X2.get());

            ASSERT(0 == X2.get());
            ASSERT(0 == X1.get());

            ASSERT(0 == X2.use_count());
            ASSERT(0 == X1.use_count());
        }
        ASSERT(1 == numDeletes);

        if (verbose) printf("\nTesting COPY-ASSIGNMENT to non-empty object"
                            "\n-------------------------------------------\n");
        {
            bsls::Types::Int64 localDeletes = 0;
            TObj *p1 = new TObj(&localDeletes);

            Obj x1(p1); const Obj& X1 = x1;
            ASSERT(p1 == x1.get());
            ASSERT( 1 == x1.use_count());

            numDeletes = 0;
            TObj *p2 = new TObj(&numDeletes);

            Obj x2(p2); const Obj& X2 = x2;

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
                P(X2.use_count());
            }
            ASSERT( 0 == localDeletes);
            ASSERT( 0 == numDeletes);
            ASSERT(p2 == X2.get());
            ASSERT( 1 == X2.use_count());

            x1 = X2;
            ASSERT_NOEXCEPT(x1 = X2);

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
                P(X2.use_count());
            }
            ASSERT( 1 == localDeletes);
            ASSERT( 0 == numDeletes);

            ASSERT(X1.rep() == X2.rep());
            ASSERT(X1.get() == X2.get());

            ASSERT(p2 == X2.get());
            ASSERT(p2 == X1.get());

            ASSERT(2 == X2.use_count());
            ASSERT(2 == X1.use_count());
        }
        ASSERT(1 == numDeletes);

        if (verbose) printf("\nTesting COPY-ASSIGNMENT to self"
                            "\n-------------------------------\n");
        {
            numDeletes = 0;
            TObj *p = new TObj(&numDeletes);

            Obj x1(p); const Obj& X1=x1;

            x1 = X1;
            ASSERT_NOEXCEPT(x1 = X1);

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
            }
            ASSERT(0 == numDeletes);

            ASSERT(p == X1.get());
            ASSERT(1 == X1.use_count());
        }

        if (verbose) printf(
        "\nTesting COPY-ASSIGNMENT (of compatible pointer) to empty object"
        "\n---------------------------------------------------------------\n");
        {
            ConstObj x1;
            const ConstObj& X1 = x1;
            ASSERT(0 == x1.get());
            ASSERT(0 == x1.use_count());

            numDeletes = 0;
            TObj *p = new TObj(&numDeletes);

            Obj x2(p); const Obj& X2 = x2;

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
                P(X2.use_count());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X2.get());
            ASSERT(1 == X2.use_count());

            x1 = X2;
            ASSERT_NOEXCEPT(x1 = X2);

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
                P(X2.use_count());
            }
            ASSERT(0 == numDeletes);

            ASSERT(X1.rep() == X2.rep());
            ASSERT(X1.get() == X2.get());

            ASSERT(p == X2.get());
            ASSERT(p == X1.get());

            ASSERT(2 == X2.use_count());
            ASSERT(2 == X1.use_count());
        }
        ASSERT(1 == numDeletes);

        if (verbose) printf(
    "\nTesting COPY-ASSIGNMENT (of compatible pointer) to non-empty object"
    "\n-------------------------------------------------------------------\n");
        {
            bsls::Types::Int64 localDeletes = 0;
            TObj *p1 = new TObj(&localDeletes);

            ConstObj x1(p1); const ConstObj& X1 = x1;
            ASSERT(p1 == x1.get());
            ASSERT( 1 == x1.use_count());

            numDeletes = 0;
            TObj *p2 = new TObj(&numDeletes);

            Obj x2(p2); const Obj& X2 = x2;

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
                P(X2.use_count());
            }
            ASSERT( 0 == localDeletes);
            ASSERT( 0 == numDeletes);
            ASSERT(p2 == X2.get());
            ASSERT( 1 == X2.use_count());

            x1 = X2;
            ASSERT_NOEXCEPT(x1 = X2);

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
                P(X2.use_count());
            }
            ASSERT( 1 == localDeletes);
            ASSERT( 0 == numDeletes);

            ASSERT(X1.rep() == X2.rep());
            ASSERT(X1.get() == X2.get());

            ASSERT(p2 == X2.get());
            ASSERT(p2 == X1.get());

            ASSERT(2 == X2.use_count());
            ASSERT(2 == X1.use_count());
        }
        ASSERT(1 == numDeletes);

        if (verbose) printf("\nTesting MOVE-ASSIGNMENT to empty object"
                            "\n---------------------------------------\n");
        {
            Obj x1;
            const Obj& X1 = x1;
            ASSERT(0 == x1.get());
            ASSERT(0 == x1.use_count());

            numDeletes = 0;
            TObj *p = new TObj(&numDeletes);

            Obj x2(p); const Obj& X2 = x2;

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
                P(X2.use_count());
            }
            ASSERT(0 == numDeletes);

            ASSERT(p == X2.get());
            ASSERT(1 == X2.use_count());

            bslma::SharedPtrRep const*const REP = X2.rep();

            x1 = MoveUtil::move(x2);
            ASSERT_NOEXCEPT(x1 = MoveUtil::move(x2));

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
                P(X2.use_count());
            }
            ASSERT(0 == numDeletes);

            ASSERT(  0 == X2.rep());
            ASSERT(REP == X1.rep());

            ASSERT(0 == X2.get());
            ASSERT(p == X1.get());

            ASSERT(0 == X2.use_count());
            ASSERT(1 == X1.use_count());
        }
        ASSERT(1 == numDeletes);

        if (verbose) printf("\nTesting MOVE-ASSIGNMENT of empty object"
                            "\n---------------------------------------\n");
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

            bslma::SharedPtrRep const*const REP = X2.rep();

            x1 = MoveUtil::move(x2);
            ASSERT_NOEXCEPT(x1 = MoveUtil::move(x2));

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
                P(X2.use_count());
            }
            ASSERT(1 == numDeletes);

            ASSERT(  0 == X2.rep());
            ASSERT(REP == X1.rep());

            ASSERT(0 == X2.get());
            ASSERT(0 == X1.get());

            ASSERT(0 == X2.use_count());
            ASSERT(0 == X1.use_count());
        }
        ASSERT(1 == numDeletes);

        if (verbose) printf("\nTesting MOVE-ASSIGNMENT to non-empty object"
                            "\n-------------------------------------------\n");
        {
            bsls::Types::Int64 localDeletes = 0;
            TObj *p1 = new TObj(&localDeletes);

            Obj x1(p1); const Obj& X1 = x1;
            ASSERT(p1 == x1.get());
            ASSERT( 1 == x1.use_count());

            numDeletes = 0;
            TObj *p2 = new TObj(&numDeletes);

            Obj x2(p2); const Obj& X2 = x2;

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
                P(X2.use_count());
            }
            ASSERT( 0 == localDeletes);
            ASSERT( 0 == numDeletes);

            ASSERT(p2 == X2.get());
            ASSERT( 1 == X2.use_count());

            bslma::SharedPtrRep const*const REP = X2.rep();

            x1 = MoveUtil::move(x2);
            ASSERT_NOEXCEPT(x1 = MoveUtil::move(x2));

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
                P(X2.use_count());
            }
            ASSERT( 1 == localDeletes);
            ASSERT( 0 == numDeletes);

            ASSERT(  0 == X2.rep());
            ASSERT(REP == X1.rep());

            ASSERT( 0 == X2.get());
            ASSERT(p2 == X1.get());

            ASSERT(0 == X2.use_count());
            ASSERT(1 == X1.use_count());
        }
        ASSERT(1 == numDeletes);

        if (verbose) printf("\nTesting MOVE-ASSIGNMENT to self"
                            "\n-------------------------------\n");
        {
            numDeletes = 0;
            TObj *p = new TObj(&numDeletes);

            Obj x1(p); const Obj& X1=x1;

            x1 = MoveUtil::move(x1);
            ASSERT_NOEXCEPT(x1 = MoveUtil::move(x1));

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
            }
            ASSERT(0 == numDeletes);

            ASSERT(p == X1.get());
            ASSERT(1 == X1.use_count());
        }

        if (verbose) printf(
        "\nTesting MOVE-ASSIGNMENT (of compatible pointer) to empty object"
        "\n---------------------------------------------------------------\n");
        {
            ConstObj x1;
            const ConstObj& X1 = x1;
            ASSERT(0 == x1.get());
            ASSERT(0 == x1.use_count());

            numDeletes = 0;
            TObj *p = new TObj(&numDeletes);

            Obj x2(p); const Obj& X2 = x2;

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
                P(X2.use_count());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X2.get());
            ASSERT(1 == X2.use_count());

            bslma::SharedPtrRep const*const REP = X2.rep();

            x1 = MoveUtil::move(x2);
            ASSERT_NOEXCEPT(x1 = MoveUtil::move(x2));

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
                P(X2.use_count());
            }
            ASSERT(0 == numDeletes);

            ASSERT(  0 == X2.rep());
            ASSERT(REP == X1.rep());

            ASSERT(0 == X2.get());
            ASSERT(p == X1.get());

            ASSERT(0 == X2.use_count());
            ASSERT(1 == X1.use_count());
        }
        ASSERT(1 == numDeletes);

        if (verbose) printf(
    "\nTesting MOVE-ASSIGNMENT (of compatible pointer) to non-empty object"
    "\n-------------------------------------------------------------------\n");
        {
            bsls::Types::Int64 localDeletes = 0;
            TObj *p1 = new TObj(&localDeletes);

            ConstObj x1(p1); const ConstObj& X1 = x1;
            ASSERT(p1 == x1.get());
            ASSERT( 1 == x1.use_count());

            numDeletes = 0;
            TObj *p2 = new TObj(&numDeletes);

            Obj x2(p2); const Obj& X2 = x2;

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
                P(X2.use_count());
            }
            ASSERT( 0 == localDeletes);
            ASSERT( 0 == numDeletes);
            ASSERT(p2 == X2.get());
            ASSERT( 1 == X2.use_count());

            bslma::SharedPtrRep const*const REP = X2.rep();

            x1 = MoveUtil::move(x2);
            ASSERT_NOEXCEPT(x1 = MoveUtil::move(x2));

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
                P(X2.use_count());
            }
            ASSERT( 1 == localDeletes);
            ASSERT( 0 == numDeletes);

            ASSERT(  0 == X2.rep());
            ASSERT(REP == X1.rep());

            ASSERT( 0 == X2.get());
            ASSERT(p2 == X1.get());

            ASSERT(0 == X2.use_count());
            ASSERT(1 == X1.use_count());
        }
        ASSERT(1 == numDeletes);

        if (verbose) printf("\nTesting ASSIGNMENT of ManagedPtr"
                            "\n--------------------------------\n");
        //   3) A managed pointer can be assigned to a shared pointer, through
        //      the same conversion sequence.  DRQS 38359639, DRQS 142188247

        ManagedPtrTestDeleter<bsls::Types::Int64> deleter;

        bsls::Types::Int64 obj1;

        {
            bslma::ManagedPtr<bsls::Types::Int64> mp(&obj1, &deleter);

            bsl::shared_ptr<bsls::Types::Int64> x;
            const bsl::shared_ptr<bsls::Types::Int64>& X = x;
            ASSERT(0 != mp.ptr());
            ASSERT(0 == X.get());
            ASSERT(0 == X.use_count());

            x = mp;

            if (veryVerbose) {
                P_(X.use_count());
                P(mp.ptr());
            }

            ASSERT(0 == mp.ptr());
            ASSERT(&obj1 == X.get());
            ASSERT(1 == X.use_count());

            ASSERT(0 == deleter.providedObj());
        }
        ASSERT(&obj1 == deleter.providedObj());

        if (verbose) printf("\nTesting ASSIGNMENT of ManagedPtr rvalue"
                            "\n-------------------------------------\n");
        {
            bsl::shared_ptr<bsls::Types::Int64> x;
            const bsl::shared_ptr<bsls::Types::Int64>& X = x;
            ASSERT(0 == X.get());
            ASSERT(0 == X.use_count());

            x = bslma::ManagedPtrUtil::allocateManaged<bsls::Types::Int64>(&ta,
                                                                           38);

            if (veryVerbose) {
                P_(X.use_count());
                P(X.get());
            }

            ASSERT(0 != X.get());
            ASSERT(1 == X.use_count());
        }

        if (verbose)
                printf("\nTesting ASSIGNMENT of polymorphic ManagedPtr"
                       "\n--------------------------------------------\n");
        {
            ObjSP baseSp;
            const ObjSP& BASE_SP = baseSp;
            ASSERT(0 == BASE_SP.get());
            ASSERT(0 == BASE_SP.use_count());

            numDeletes = 0;

            baseSp = bslma::ManagedPtrUtil::allocateManaged<
                                        MyTestDerivedObject>(&ta, &numDeletes);

            if (veryVerbose) {
                P_(BASE_SP.use_count());
                P(BASE_SP.get());
            }

            ASSERT(0 != BASE_SP.get());
            ASSERT(1 == BASE_SP.use_count());

            ASSERT(0 == numDeletes);

            bslma::ManagedPtr<MyTestDerivedObject> mpd =
                    bslma::ManagedPtrUtil::allocateManaged<
                                        MyTestDerivedObject>(&ta, &numDeletes);

            const MyTestObject * const mpdPtrWas = mpd.get();

            baseSp = mpd;
            ASSERT(1 == numDeletes);
            numDeletes = 0;

            if (veryVerbose) {
                P_(BASE_SP.use_count());
                P(BASE_SP.get());
            }

            ASSERT(0 != BASE_SP.get());
            ASSERT(1 == BASE_SP.use_count());

            ASSERT(mpdPtrWas == BASE_SP.get());
            ASSERT(        0 == mpd.get());

            ASSERT(0 == numDeletes);
        }
        ASSERT(1 == numDeletes);

        if (verbose)
                printf("\nTesting ASSIGNMENT of convertible ManagedPtr"
                       "\n--------------------------------------------\n");
        {
            typedef bsl::shared_ptr<NonPolymorphicTestBaseObject> BaseSp;
            BaseSp baseSp;
            const BaseSp& BASE_SP = baseSp;
            ASSERT(0 == BASE_SP.get());
            ASSERT(0 == BASE_SP.use_count());

            numDeletes = 0;

            baseSp = bslma::ManagedPtrUtil::allocateManaged<
                                   NonPolymorphicTestObject>(&ta, &numDeletes);

            if (veryVerbose) {
                P_(BASE_SP.use_count());
                P(BASE_SP.get());
            }

            ASSERT(0 != BASE_SP.get());
            ASSERT(1 == BASE_SP.use_count());

            ASSERT(0 == numDeletes);

            bslma::ManagedPtr<NonPolymorphicTestObject> mpd =
                 bslma::ManagedPtrUtil::allocateManaged<
                                   NonPolymorphicTestObject>(&ta, &numDeletes);

            const NonPolymorphicTestBaseObject * const mpdPtrWas = mpd.get();

            baseSp = mpd;

            ASSERT(1 == numDeletes);
            numDeletes = 0;

            if (veryVerbose) {
                P_(BASE_SP.use_count());
                P(BASE_SP.get());
            }

            ASSERT(0 != BASE_SP.get());
            ASSERT(1 == BASE_SP.use_count());

            ASSERT(mpdPtrWas == BASE_SP.get());
            ASSERT(        0 == mpd.get());

            ASSERT(0 == numDeletes);
        }
        ASSERT(1 == numDeletes);

        if (verbose) printf("\nTesting ASSIGNMENT of ManagedPtr to void SP"
                            "\n--------------------------------\n");
        {
            bslma::ManagedPtr<bsls::Types::Int64> mpi =
                bslma::ManagedPtrUtil::allocateManaged<bsls::Types::Int64>(&ta,
                                                                           42);

            bslma::ManagedPtr<bsls::Types::Int64> mp(mpi);

            bsl::shared_ptr<void> x;
            const bsl::shared_ptr<void>& X = x;
            ASSERT(0 != mp.ptr());
            ASSERT(0 == X.get());
            ASSERT(0 == X.use_count());

            x = mp;

            if (veryVerbose) {
                P_(X.use_count());
                P(mp.ptr());
            }

            ASSERT(0 == mp.ptr());
            ASSERT(0 != X.get());
            ASSERT(1 == X.use_count());
        }
        {
            bsl::shared_ptr<void> x;
            const bsl::shared_ptr<void>& X = x;
            ASSERT(0 == X.get());
            ASSERT(0 == X.use_count());

            x = bslma::ManagedPtrUtil::allocateManaged<bsls::Types::Int64>(&ta,
                                                                           24);

            if (veryVerbose) {
                P_(X.use_count());
                P(X.get());
            }

            ASSERT(0 != X.get());
            ASSERT(1 == X.use_count());
        }
        {
            bsl::shared_ptr<void> baseSp;
            const bsl::shared_ptr<void>& BASE_SP = baseSp;
            ASSERT(0 == BASE_SP.get());
            ASSERT(0 == BASE_SP.use_count());

            numDeletes = 0;

            baseSp = bslma::ManagedPtrUtil::allocateManaged<
                                        MyTestDerivedObject>(&ta, &numDeletes);

            if (veryVerbose) {
                P_(BASE_SP.use_count());
                P(BASE_SP.get());
            }

            ASSERT(0 != BASE_SP.get());
            ASSERT(1 == BASE_SP.use_count());

            ASSERT(0 == numDeletes);

            bslma::ManagedPtr<MyTestDerivedObject> mpd =
                    bslma::ManagedPtrUtil::allocateManaged<
                                        MyTestDerivedObject>(&ta, &numDeletes);

            const void * const mpdPtrWas = mpd.get();

            baseSp = mpd;
            ASSERT(1 == numDeletes);
            numDeletes = 0;

            if (veryVerbose) {
                P_(BASE_SP.use_count());
                P(BASE_SP.get());
            }

            ASSERT(0 != BASE_SP.get());
            ASSERT(1 == BASE_SP.use_count());

            ASSERT(mpdPtrWas == BASE_SP.get());
            ASSERT(        0 == mpd.get());

            ASSERT(0 == numDeletes);
        }
        ASSERT(1 == numDeletes);

        {
            bsl::shared_ptr<void> x;
            const bsl::shared_ptr<void>& X = x;
            ASSERT(0 == X.get());
            ASSERT(0 == X.use_count());

            bslma::ManagedPtr<bsls::Types::Int64> mpi =
                bslma::ManagedPtrUtil::allocateManaged<bsls::Types::Int64>(&ta,
                                                                          144);
            bslma::ManagedPtr<void> mpv(mpi);
            x = mpv;

            if (veryVerbose) {
                P_(X.use_count());
                P(X.get());
            }

            ASSERT(0 != X.get());
            ASSERT(1 == X.use_count());
        }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)

# if defined(BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
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
# if defined(BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC)
#   pragma GCC diagnostic pop
# endif
#endif

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

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR)
        if (verbose)
         printf("\nTesting MOVE-ASSIGNMENT (of unique_ptr) to empty object"
                "\n-------------------------------------------------------\n");
        {
            typedef std::unique_ptr<TObj> UPtr;

            Obj x1;
            const Obj& X1 = x1;
            ASSERT(0 == x1.get());
            ASSERT(0 == x1.use_count());

            numDeletes = 0;
            TObj *p = new TObj(&numDeletes);

            UPtr x2(p); const UPtr &X2 = x2;

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
                P(X2 != nullptr);
            }
            ASSERT(0 == numDeletes);

            ASSERT(p == X2.get());

            x1 = MoveUtil::move(x2);

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
            }
            ASSERT(0 == numDeletes);

            ASSERT(0 == X2.get());
            ASSERT(p == X1.get());

            ASSERT(1 == X1.use_count());
        }
        ASSERT(1 == numDeletes);

        if (verbose) printf("\nTesting MOVE-ASSIGNMENT of empty unique_ptr"
                            "\n-------------------------------------------\n");
        {
            typedef std::unique_ptr<TObj> UPtr;

            numDeletes = 0;
            TObj *p = new TObj(&numDeletes);

            Obj x1(p); const Obj& X1=x1;

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
            }
            ASSERT(0 == numDeletes);

            ASSERT(p == X1.get());
            ASSERT(1 == X1.use_count());

            UPtr x2;
            const UPtr& X2 = x2;
            ASSERT(0 == x2.get());

            x1 = MoveUtil::move(x2);

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
            }
            ASSERT(1 == numDeletes);

            ASSERT(0 == X2.get());
            ASSERT(0 == X1.get());

            ASSERT(0 == X1.use_count());
        }
        ASSERT(1 == numDeletes);

        if (verbose)
     printf("\nTesting MOVE-ASSIGNMENT (of unique_ptr) to non-empty object"
            "\n-----------------------------------------------------------\n");
        {
            typedef std::unique_ptr<TObj> UPtr;

            bsls::Types::Int64 localDeletes = 0;
            TObj *p1 = new TObj(&localDeletes);

            Obj x1(p1); const Obj& X1 = x1;
            ASSERT(p1 == x1.get());
            ASSERT( 1 == x1.use_count());

            numDeletes = 0;
            TObj *p2 = new TObj(&numDeletes);

            UPtr x2(p2); const UPtr &X2 = x2;

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
            }
            ASSERT( 0 == localDeletes);
            ASSERT( 0 == numDeletes);

            ASSERT(p2 == X2.get());

            x1 = MoveUtil::move(x2);

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
            }
            ASSERT( 1 == localDeletes);
            ASSERT( 0 == numDeletes);

            ASSERT( 0 == X2.get());
            ASSERT(p2 == X1.get());

            ASSERT(1 == X1.use_count());
        }
        ASSERT(1 == numDeletes);

        if (verbose)
 printf("\nTesting MOVE-ASSIGNMENT (of unique_ptr+deleter) to empty object"
        "\n---------------------------------------------------------------\n");
        {
            typedef MyTestDeleter Deleter;
            int testDeleteCount = 0;
            Deleter deleter(&bslma::NewDeleteAllocator::singleton(),
                            &testDeleteCount);

            typedef std::unique_ptr<TObj,Deleter> UPtr;

            Obj x1;
            const Obj& X1 = x1;
            ASSERT(0 == x1.get());
            ASSERT(0 == x1.use_count());
            ASSERT(0 == testDeleteCount);

            numDeletes = 0;
            TObj *p = new TObj(&numDeletes);

            UPtr x2(p,deleter); const UPtr &X2 = x2;

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
                P_(testDeleteCount); P(X2 != nullptr);
            }
            ASSERT(0 == numDeletes);
            ASSERT(0 == testDeleteCount);

            ASSERT(p == X2.get());

            x1 = MoveUtil::move(x2);

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
            }
            ASSERT(0 == numDeletes);
            ASSERT(0 == testDeleteCount);

            ASSERT(0 == X2.get());
            ASSERT(p == X1.get());

            ASSERT(1 == X1.use_count());

            x1.reset();
            ASSERT( 0 == X1.get());
            ASSERT( 1 == testDeleteCount);
            ASSERT( 1 == numDeletes);
        }
        ASSERT(1 == numDeletes);

        if (verbose)
             printf("\nTesting MOVE-ASSIGNMENT of empty unique_ptr+deleter"
                    "\n---------------------------------------------------\n");
        {
            typedef MyTestDeleter Deleter;
            int testDeleteCount = 0;
            Deleter deleter(&bslma::NewDeleteAllocator::singleton(),
                            &testDeleteCount);

            typedef std::unique_ptr<TObj,Deleter> UPtr;

            numDeletes = 0;
            TObj *p = new TObj(&numDeletes);

            Obj x1(p); const Obj& X1=x1;

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
                P_(testDeleteCount);
            }
            ASSERT(0 == numDeletes);
            ASSERT(0 == testDeleteCount);

            ASSERT(p == X1.get());
            ASSERT(1 == X1.use_count());

            UPtr x2(0,deleter); const UPtr& X2 = x2;
            ASSERT(0 == x2.get());

            x1 = MoveUtil::move(x2);

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
            }
            ASSERT(1 == numDeletes);
            ASSERT(0 == testDeleteCount);

            ASSERT(0 == X2.get());
            ASSERT(0 == X1.get());

            ASSERT(0 == X1.use_count());
        }
        ASSERT(1 == numDeletes);

        if (verbose)
    printf("\nTesting MOVE-ASSIGNMENT (of unique_ptr+deleter) to non-empty"
           "\n------------------------------------------------------------\n");
        {
            typedef MyTestDeleter Deleter;
            int testDeleteCount = 0;
            Deleter deleter(&bslma::NewDeleteAllocator::singleton(),
                            &testDeleteCount);

            typedef std::unique_ptr<TObj,Deleter> UPtr;

            bsls::Types::Int64 localDeletes = 0;
            TObj *p1 = new TObj(&localDeletes);

            Obj x1(p1); const Obj& X1 = x1;
            ASSERT(p1 == x1.get());
            ASSERT( 1 == x1.use_count());
            ASSERT( 0 == testDeleteCount);

            numDeletes = 0;
            TObj *p2 = new TObj(&numDeletes);

            UPtr x2(p2,deleter); const UPtr &X2 = x2;

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
                P_(testDeleteCount);
            }
            ASSERT( 0 == localDeletes);
            ASSERT( 0 == numDeletes);
            ASSERT( 0 == testDeleteCount);

            ASSERT(p2 == X2.get());

            x1 = MoveUtil::move(x2);

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
            }
            ASSERT( 1 == localDeletes);
            ASSERT( 0 == numDeletes);
            ASSERT( 0 == testDeleteCount);

            ASSERT( 0 == X2.get());
            ASSERT(p2 == X1.get());

            ASSERT(1 == X1.use_count());

            x1.reset();
            ASSERT( 0 == X1.get());
            ASSERT( 1 == testDeleteCount);
            ASSERT( 1 == numDeletes);
        }
        ASSERT(1 == numDeletes);
#endif

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'swap'
        //
        // Concerns:
        //  TBD
        //
        // Plan:
        //  TBD
        //
        // Testing:
        //   void swap(shared_ptr& src) noexcept
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
                ASSERT_NOEXCEPT(x.swap(y));

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

        if (verbose) printf("\tWith mix of allocators.\n");

        bslma::TestAllocator da(veryVeryVerbose);
        bslma::DefaultAllocatorGuard defaultGuard(&da);

        // WARNING: Installing a test allocator as the default means that
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

        if (verbose) printf("\tWith mix of deleters.\n");

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
        // TESTING COPY AND MOVE CONSTRUCTORS
        //
        // Concerns:
        //  TBD
        //
        // Plan:
        //  TBD
        //
        // Testing:
        //   shared_ptr(const shared_ptr& original) noexcept
        //   shared_ptr(const shared_ptr<OTHER>& other) noexcept
        //   shared_ptr(shared_ptr&& other) noexcept
        //   shared_ptr(shared_ptr<OTHER>&& other) noexcept
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING COPY AND MOVE CONSTRUCTORS"
                            "\n==================================\n");

        if (verbose) printf("\nTesting COPY-CONSTRUCTION of empty object"
                            "\n-----------------------------------------\n");
        {
            Obj x1;     const Obj& X1 = x1;
            ASSERT(0 == x1.get());
            ASSERT(0 == x1.use_count());

            numDeletes = 0;

            Obj x2(X1); const Obj& X2 = x2;
            ASSERT_NOEXCEPT(Obj(X1));

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
                P(X2.use_count());
            }
            ASSERT(0 == numDeletes);

            ASSERT(X1.rep() == X2.rep());
            ASSERT(X1.get() == X2.get());

            ASSERT(0 == X2.get());
            ASSERT(0 == X2.use_count());

            ASSERT(0 == X1.get());
            ASSERT(0 == X1.use_count());
        }
        ASSERT(0 == numDeletes);
        numDeletes = 0;

        if (verbose) printf(
                         "\nTesting COPY-CONSTRUCTION of simply owned object"
                         "\n----------------------------------------------\n");
        {
            TObj *p = new TObj(&numDeletes);

            Obj x1(p);  const Obj& X1 = x1;
            ASSERT(p == x1.get());
            ASSERT(1 == x1.use_count());

            Obj x2(X1); const Obj& X2 = x2;
            ASSERT_NOEXCEPT(Obj(X1));

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
                P(X2.use_count());
            }
            ASSERT(0 == numDeletes);

            ASSERT(X1.rep() == X2.rep());
            ASSERT(X1.get() == X2.get());

            ASSERT(p == X2.get());
            ASSERT(2 == X2.use_count());

            ASSERT(p == X1.get());
            ASSERT(2 == X1.use_count());
        }
        ASSERT(1 == numDeletes);
        numDeletes = 0;

        if (verbose) printf(
                        "\nTesting COPY-CONSTRUCTION of owned null-pointer"
                        "\n-----------------------------------------------\n");
        {
            Obj x1((TObj *)0);  const Obj& X1 = x1;
            ASSERT(0 == x1.get());
            ASSERT(1 == x1.use_count());

            Obj x2(X1); const Obj& X2 = x2;
            ASSERT_NOEXCEPT(Obj(X1));

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
                P(X2.use_count());
            }
            ASSERT(0 == numDeletes);

            ASSERT(X1.rep() == X2.rep());
            ASSERT(X1.get() == X2.get());

            ASSERT(0 == X2.get());
            ASSERT(2 == X2.use_count());

            ASSERT(0 == X1.get());
            ASSERT(2 == X1.use_count());
        }
        ASSERT(0 == numDeletes);
        numDeletes = 0;

        if (verbose) printf(
                    "\nTesting COPY-CONSTRUCTION from null-pointer literal"
                    "\n---------------------------------------------------\n");
        {
            // Test null-pointer literal specially, to remind ourselves this
            // behaves as testing the default constructor, not testing an owned
            // null pointer (the immediately preceding test).

            Obj x1(0);  const Obj& X1 = x1;
            ASSERT(0 == x1.get());
            ASSERT(0 == x1.use_count());

            Obj x2(X1); const Obj& X2 = x2;
            ASSERT_NOEXCEPT(Obj(X1));

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
                P(X2.use_count());
            }
            ASSERT(0 == numDeletes);

            ASSERT(X1.rep() == X2.rep());
            ASSERT(X1.get() == X2.get());

            ASSERT(0 == X2.get());
            ASSERT(0 == X2.use_count());

            ASSERT(0 == X1.get());
            ASSERT(0 == X1.use_count());
        }
        ASSERT(0 == numDeletes);
        numDeletes = 0;

        if (verbose) printf(
                 "\nTesting \"COPY-CONSTRUCTION\" from compatible shared_ptr"
                 "\n------------------------------------------------------\n");
        {
            TObj *p = new TObj(&numDeletes);

            Obj x1(p);  const Obj& X1 = x1;
            ASSERT(p == x1.get());
            ASSERT(1 == x1.use_count());

            ConstObj x2(X1); const ConstObj& X2 = x2;
            ASSERT_NOEXCEPT(Obj(X1));

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
                P(X2.use_count());
            }
            ASSERT(0 == numDeletes);

            ASSERT(X1.rep() == X2.rep());
            ASSERT(X1.get() == X2.get());

            ASSERT(p == X2.get());
            ASSERT(2 == X2.use_count());

            ASSERT(p == X1.get());
            ASSERT(2 == X1.use_count());
        }
        ASSERT(1 == numDeletes);
        numDeletes = 0;



        if (verbose) printf("\nTesting MOVE-CONSTRUCTION of empty object"
                            "\n-----------------------------------------\n");
        {
            Obj x1;     const Obj& X1 = x1;
            ASSERT(0 == x1.get());
            ASSERT(0 == x1.use_count());

            numDeletes = 0;

            Obj x2(MoveUtil::move(x1)); const Obj& X2 = x2;
            ASSERT_NOEXCEPT(Obj(MoveUtil::move(x1)));

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
                P(X2.use_count());
            }
            ASSERT(0 == numDeletes);

            ASSERT(X1.rep() == X2.rep());
            ASSERT(X1.get() == X2.get());

            ASSERT(0 == X2.get());
            ASSERT(0 == X2.use_count());

            ASSERT(0 == X1.get());
            ASSERT(0 == X1.use_count());
        }
        ASSERT(0 == numDeletes);
        numDeletes = 0;

        if (verbose) printf(
                         "\nTesting MOVE-CONSTRUCTION of simply owned object"
                         "\n----------------------------------------------\n");
        {
            TObj *p = new TObj(&numDeletes);

            Obj x1(p);  const Obj& X1 = x1;
            ASSERT(p == x1.get());
            ASSERT(1 == x1.use_count());

            bslma::SharedPtrRep const*const REP = X1.rep();

            Obj x2(MoveUtil::move(x1)); const Obj& X2 = x2;
            ASSERT_NOEXCEPT(Obj(MoveUtil::move(x1)));

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
                P(X2.use_count());
            }
            ASSERT(0 == numDeletes);

            ASSERT(REP == X2.rep());
            ASSERT(  0 == X1.rep());

            ASSERT(p == X2.get());
            ASSERT(1 == X2.use_count());

            ASSERT(0 == X1.get());
            ASSERT(0 == X1.use_count());
        }
        ASSERT(1 == numDeletes);
        numDeletes = 0;

        if (verbose) printf(
                 "\nTesting \"MOVE-CONSTRUCTION\" from compatible shared_ptr"
                 "\n------------------------------------------------------\n");
        {
            TObj *p = new TObj(&numDeletes);

            Obj x1(p);  const Obj& X1 = x1;
            ASSERT(p == x1.get());
            ASSERT(1 == x1.use_count());

            bslma::SharedPtrRep const*const REP = X1.rep();

            ConstObj x2(MoveUtil::move(x1));
            const ConstObj& X2 = x2;
            ASSERT_NOEXCEPT(Obj(MoveUtil::move(x1)));

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
                P(X2.use_count());
            }
            ASSERT(0 == numDeletes);

            ASSERT(REP == X2.rep());
            ASSERT(  0 == X1.rep());

            ASSERT(p == X2.get());
            ASSERT(1 == X2.use_count());

            ASSERT(0 == X1.get());
            ASSERT(0 == X1.use_count());
        }
        ASSERT(1 == numDeletes);
        numDeletes = 0;

        if (verbose) printf(
                        "\nTesting MOVE-CONSTRUCTION of owned null-pointer"
                        "\n-----------------------------------------------\n");
        {
            Obj x1((TObj *)0);  const Obj& X1 = x1;
            ASSERT(0 == x1.get());
            ASSERT(1 == x1.use_count());

            bslma::SharedPtrRep const*const REP = X1.rep();

            Obj x2(MoveUtil::move(x1)); const Obj& X2 = x2;
            ASSERT_NOEXCEPT(Obj(MoveUtil::move(x1)));

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
                P(X2.use_count());
            }
            ASSERT(0 == numDeletes);

            ASSERT(REP == X2.rep());
            ASSERT(  0 == X1.rep());

            ASSERT(0 == X2.get());
            ASSERT(1 == X2.use_count());

            ASSERT(0 == X1.get());
            ASSERT(0 == X1.use_count());
        }
        ASSERT(0 == numDeletes);
        numDeletes = 0;

        if (verbose) printf(
                    "\nTesting MOVE-CONSTRUCTION from null-pointer literal"
                    "\n---------------------------------------------------\n");
        {
            // Test null-pointer literal specially, to remind ourselves this
            // behaves as testing the default constructor, not testing an owned
            // null pointer (the immediately preceding test).

            Obj x1(0);  const Obj& X1 = x1;
            ASSERT(0 == x1.get());
            ASSERT(0 == x1.use_count());

            Obj x2(MoveUtil::move(x1)); const Obj& X2 = x2;
            ASSERT_NOEXCEPT(Obj(MoveUtil::move(x1)));

            if (veryVerbose) {
                P_(numDeletes); P_(X1.use_count());
                P(X2.use_count());
            }
            ASSERT(0 == numDeletes);

            ASSERT(0 == X2.rep());
            ASSERT(0 == X1.rep());

            ASSERT(0 == X2.get());
            ASSERT(0 == X2.use_count());

            ASSERT(0 == X1.get());
            ASSERT(0 == X1.use_count());
        }
        ASSERT(0 == numDeletes);
        numDeletes = 0;

        if (verbose) printf(
                    "\nTesting COPY-CONSTRUCTION of shared_ptr to array"
                    "\n------------------------------------------------\n");
        {
            // Test construction of 'shared_ptr<T[N]>' and 'shared_ptr<T[]>'
            // from other types.  All the shared_ptrs in this test are null,
            // so no memory allocations or deletions should occur.

            bsl::shared_ptr<int> sp0;
            ASSERT(0 == sp0.get());
            ASSERT(0 == sp0.use_count());

            // shared_ptr<int> -> shared_ptr<const int>
            bsl::shared_ptr<const int> sp0c = sp0;
            ASSERT(0 == sp0c.get());
            ASSERT(0 == sp0c.use_count());

            // shared_ptr<int> -> shared_ptr<volatile int>
            bsl::shared_ptr<volatile int> sp0v = sp0;
            ASSERT(0 == sp0v.get());
            ASSERT(0 == sp0v.use_count());

            // shared_ptr<int> -> shared_ptr<const volatile int>
            bsl::shared_ptr<const volatile int> sp0cv = sp0;
            ASSERT(0 == sp0cv.get());
            ASSERT(0 == sp0cv.use_count());

            bsl::shared_ptr<int[3]> sp1;
            ASSERT(0 == sp1.get());
            ASSERT(0 == sp1.use_count());

            // shared_ptr<int[N]> -> shared_ptr<const int[N]>
            bsl::shared_ptr<const int[3]> sp1c = sp1;
            ASSERT(0 == sp1c.get());
            ASSERT(0 == sp1c.use_count());

            // shared_ptr<int[N]> -> shared_ptr<volatile int[N]>
            bsl::shared_ptr<volatile int[3]> sp1v = sp1;
            ASSERT(0 == sp1v.get());
            ASSERT(0 == sp1v.use_count());

            // shared_ptr<int[N]> -> shared_ptr<const volatile int[N]>
            bsl::shared_ptr<const volatile int[3]> sp1cv = sp1;
            ASSERT(0 == sp1cv.get());
            ASSERT(0 == sp1cv.use_count());

#ifndef BSLSTL_SHAREDPTR_DONT_TEST_UNBOUNDED_ARRAYS
            // shared_ptr<int[N]> -> shared_ptr<int[]>
            bsl::shared_ptr<int[]> sp2 = sp1;
            ASSERT(0 == sp2.get());
            ASSERT(0 == sp2.use_count());

            // shared_ptr<int[N]> -> shared_ptr<const int[]>
            bsl::shared_ptr<const int[]> sp2c = sp1;
            ASSERT(0 == sp2c.get());
            ASSERT(0 == sp2c.use_count());

            // shared_ptr<int[N]> -> shared_ptr<volatile int[]>
            bsl::shared_ptr<volatile int[]> sp2v = sp1;
            ASSERT(0 == sp2v.get());
            ASSERT(0 == sp2v.use_count());

            // shared_ptr<int[N]> -> shared_ptr<const volatile int[]>
            bsl::shared_ptr<const volatile int[]> sp2cv = sp1;
            ASSERT(0 == sp2cv.get());
            ASSERT(0 == sp2cv.use_count());
#endif

        }
        ASSERT(0 == numDeletes);
        numDeletes = 0;
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
        //:
        //: 4 Comparison compares only the addresses of the pointed-to objects,
        //:   and not the owned objects.
        //:
        //: 5 Shared pointer aliasing has no effect on comparison operators.
        //:   (Note that this concern cannot be tested until alias constructors
        //:   and 'reset' functions are tested at higher-numbered test cases.)
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
        // Concerns:
        //  TBD
        //
        // Plan:
        //  TBD
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
        // Concerns:
        //  TBD
        //
        // Plan:
        //  TBD
        //
        // Testing:
        //   add_lvalue_reference<ELEM_TYPE>::type operator[](ptrdiff_t) const
        //   add_lvalue_reference<ELEM_TYPE>::type operator*() const
        //   TYPE *operator->() const
        //   bslma::SharedPtrRep *rep() const
        //   TYPE *get() const
        //   bool unique() const
        //   long use_count() const
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        //   int numReferences() const
        //   TYPE *ptr() const
#endif // BDE_OMIT_INTERNAL_DEPRECATED
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
            ASSERT(!X);
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            ASSERT(0 == X.ptr());
            ASSERT(0 == X.numReferences());
#endif // BDE_OMIT_INTERNAL_DEPRECATED

            if (verbose) printf("\nNegative Testing.\n");
            {
                bsls::AssertTestHandlerGuard hG;

             // ASSERT_SAFE_FAIL(*x); // TBD: negative testing of 'noexcept's
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
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());
#endif // BDE_OMIT_INTERNAL_DEPRECATED
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
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());
#endif // BDE_OMIT_INTERNAL_DEPRECATED

            x.reset();

            numDeallocations += 2;
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(1 == numDeletes);
            ASSERT(0 == X.get());
            ASSERT(0 == X.rep());
            ASSERT(0 == X.use_count());
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            ASSERT(0 == X.ptr());
            ASSERT(0 == X.numReferences());
#endif // BDE_OMIT_INTERNAL_DEPRECATED
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
        // Concerns:
        //  TBD
        //
        // Plan:
        //  TBD
        //
        // Testing:
        //   shared_ptr(TYPE *ptr) // synthesized
        //   shared_ptr(TYPE *ptr, bslma::Allocator *allocator)  // synthesized
        //   shared_ptr(OTHER *ptr)
        //   shared_ptr(OTHER *ptr, bslma::Allocator *basicAllocator)
        //   shared_ptr(ELEM_TYPE *ptr, bslma::SharedPtrRep *rep)
        //   shared_ptr(ELEM_TYPE *, bslma::SharedPtrRep *, FromSharedTag)
        //   shared_ptr(OTHER *ptr, DELETER *deleter)
        //   shared_ptr(OTHER *ptr, DELETER, bslma::Allocator* = 0)
        //   shared_ptr(OTHER *ptr, DELETER, ALLOCATOR, SFINAE)
        //   shared_ptr(nullptr_t)
        //   shared_ptr(nullptr_t, bslma::Allocator *)
        //   shared_ptr(nullptr_t, DELETER, bslma::Allocator * = 0)
        //   shared_ptr(nullptr_t, DELETER, ALLOCATOR, SFINAE)
        //   shared_ptr(std::auto_ptr<OTHER> autoPtr, bslma::Allocator*=0)
        //   shared_ptr(unique_ptr<OTHER, DELETER>&& adoptee)
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

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)

# if defined(BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
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
# if defined(BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC)
#   pragma GCC diagnostic pop
# endif
#endif

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


#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)

# if defined(BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
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
# if defined(BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC)
#   pragma GCC diagnostic pop
# endif
#endif

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

        if (verbose)
            printf(
         "\nTesting constructor (with leading-AA deleter and bslma::allocator)"
         "\n------------------------------------------------------------------"
         "\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new (ta) TObj(&numDeletes);
            numAllocations = ta.numAllocations();

            MyAllocArgTestDeleter deleter(bsl::allocator_arg, &ta, &ta);
            bslma::Allocator *ba = &ta;
            Obj x(p, deleter, ba); const Obj& X = x;
            ASSERT(numAllocations+=2 == ta.numAllocations());

            if (veryVerbose) {
                P_(numDeletes); P(X.use_count());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.get());
            ASSERT(1 == X.use_count());

            MyAllocArgTestDeleter *const deleterPtr =
                                    bsl::get_deleter<MyAllocArgTestDeleter>(x);
            ASSERT(0 != deleterPtr);
            ASSERT(deleterPtr->allocator() == &ta);
        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }

        ASSERT(1 == numDeletes);
        ASSERT((numDeallocations+4) == ta.numDeallocations());

        if (verbose)
            printf(
     "\nTesting constructor (with leading-bsl-AA deleter and bslma::allocator)"
     "\n----------------------------------------------------------------------"
     "\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new (ta) TObj(&numDeletes);
            numAllocations = ta.numAllocations();

            MyBslAllocArgTestDeleter deleter(bsl::allocator_arg, &ta, &ta);
            bslma::Allocator *ba = &ta;
            Obj x(p, deleter, ba); const Obj& X = x;
            ASSERT(numAllocations+=2 == ta.numAllocations());

            if (veryVerbose) {
                P_(numDeletes); P(X.use_count());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.get());
            ASSERT(1 == X.use_count());

            MyBslAllocArgTestDeleter *const deleterPtr =
                                 bsl::get_deleter<MyBslAllocArgTestDeleter>(x);
            ASSERT(0 != deleterPtr);
            ASSERT(deleterPtr->allocator() == &ta);
        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }

        ASSERT(1 == numDeletes);
        ASSERT((numDeallocations+4) == ta.numDeallocations());

        if (verbose)
            printf(
               "\nTesting constructor (with bsl-AA deleter and bsl::allocator)"
               "\n------------------------------------------------------------"
               "\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new (ta) TObj(&numDeletes);
            numAllocations = ta.numAllocations();

            MyBslAllocArgTestDeleter deleter(bsl::allocator_arg, &ta, &ta);
            bslma::Allocator *ba = &ta;
            Obj x(p, deleter, bsl::allocator<char>(ba)); const Obj& X = x;
            ASSERT(numAllocations+=2 == ta.numAllocations());

            if (veryVerbose) {
                P_(numDeletes); P(X.use_count());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.get());
            ASSERT(1 == X.use_count());

            MyBslAllocArgTestDeleter *const deleterPtr =
                                 bsl::get_deleter<MyBslAllocArgTestDeleter>(x);
            ASSERT(0 != deleterPtr);
            ASSERT(deleterPtr->allocator() == &defaultAllocator);

        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }

        ASSERT(1 == numDeletes);
        ASSERT((numDeallocations+3) == ta.numDeallocations());

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


        if (verbose) printf("\nTesting constructor (with typed-rep)"
                            "\n------------------------------------\n");

        {
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

        if (verbose) printf(
                   "\nTesting constructor (with rep and \"from-shared\" tag)"
                   "\n----------------------------------------------------\n");

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

            Obj xx(p, rep, bslstl::SharedPtr_RepFromExistingSharedPtr());
            const Obj& XX = xx;
            ASSERT(p == XX.get());
            ASSERT(rep ==  XX.rep());
            ASSERT(1 == XX.use_count());
        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }
        ASSERT((numDeallocations+2) == ta.numDeallocations());

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR)
        if (verbose) printf("\nTesting unique_ptr rvalue constructor"
                            "\n------------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            typedef std::unique_ptr<TObj> UPtr;

            numDeletes = 0;
            TObj *p = new TObj(&numDeletes);
            UPtr up(p);

            numAllocations = ta.numAllocations();

            Obj x(MoveUtil::move(up), &ta); const Obj& X = x;
            ASSERT(0 == up.get());
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


        if (verbose) printf(
                   "\nTesting unique_ptr rvalue constructor (with deleter)"
                   "\n----------------------------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            typedef MyTestDeleter Deleter;
            int testDeleteCount = 0;
            Deleter deleter(&bslma::NewDeleteAllocator::singleton(),
                            &testDeleteCount);

            typedef std::unique_ptr<TObj,Deleter> UPtr;

            numDeletes = 0;
            TObj *p = new TObj(&numDeletes);
            UPtr up(p,deleter);

            numAllocations = ta.numAllocations();

            Obj x(MoveUtil::move(up), &ta); const Obj& X = x;
            ASSERT(0 == up.get());
            ASSERT(++numAllocations == ta.numAllocations());

            if (veryVerbose) {
                P_(numDeletes); P(X.use_count());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.get());
            ASSERT(1 == X.use_count());

            x.reset();
            ASSERT(1 == numDeletes);
            ASSERT(0 == x.get());
            ASSERT(1 == testDeleteCount);
        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT((++numDeallocations) == ta.numDeallocations());

#endif
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
        // Concerns:
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
        //   void reset() noexcept
        //   void reset(OTHER *ptr)
        //   void reset(OTHER *ptr, DELETER deleter)
        //   void reset(OTHER *ptr, DELETER deleter, ALLOCATOR basicAllocator)
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        //   void clear()
        //   void load(OTHER *ptr, bslma::Allocator *allocator=0)
        //   void load(OTHER *ptr, const DELETER&, bslma::Allocator *)
#endif // BDE_OMIT_INTERNAL_DEPRECATED
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


#ifndef BDE_OMIT_INTERNAL_DEPRECATED
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

            x.clear();

            numDeallocations += 2;
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(1 == numDeletes);
            ASSERT(0 == X.get());
            ASSERT(0 == X.rep());
            ASSERT(0 == X.use_count());
        }

        ASSERT(1 == numDeletes);
        ASSERT(numDeallocations == ta.numDeallocations());


        if (verbose) printf("\nTesting load of null ptr(on empty object)"
                            "\n-----------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        numDefaultDeallocations = defaultAllocator.numDeallocations();
        numDefaultAllocations   = defaultAllocator.numAllocations();
        {
            Obj x; const Obj& X=x;
            x.load(static_cast<TObj *>(0));
            ASSERT(0 == X.get());
            ASSERT(1 == X.use_count());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(++numDefaultAllocations ==
                                            defaultAllocator.numAllocations());
            ASSERT(numDefaultDeallocations ==
                                          defaultAllocator.numDeallocations());

            Obj y; const Obj& Y=y;
            y.load(static_cast<TObj *>(0), &ta);
            ASSERT(0 == Y.get());
            ASSERT(1 == Y.use_count());
            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(numDefaultAllocations == defaultAllocator.numAllocations());
            ASSERT(numDefaultDeallocations ==
                                          defaultAllocator.numDeallocations());

            Obj z; const Obj& Z=z;
            z.load(static_cast<TObj *>(0), &ta, &ta);
            ASSERT(0 == Z.get());
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

        if (verbose) printf(
                          "\nTesting load of null ptr(on non-empty object)"
                          "\n---------------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        numDefaultDeallocations = defaultAllocator.numDeallocations();
        numDefaultAllocations   = defaultAllocator.numAllocations();
        {
            numDeletes = 0;
            Obj x(new (ta) TObj(&numDeletes), &ta, 0); const Obj& X=x;
            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(++numDefaultAllocations ==
                                            defaultAllocator.numAllocations());
            ASSERT(numDefaultDeallocations ==
                                          defaultAllocator.numDeallocations());
            ASSERT(0 == numDeletes);

            x.load(static_cast<TObj *>(0));
            ASSERT(1 == numDeletes);
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(++numDeallocations == ta.numDeallocations());
            ASSERT(++numDefaultAllocations ==
                                            defaultAllocator.numAllocations());
            ASSERT(++numDefaultDeallocations ==
                                          defaultAllocator.numDeallocations());
            ASSERT(0 == X.get());
            ASSERT(1 == X.use_count());
        }
        ASSERT(numAllocations == ta.numAllocations());
        ASSERT(numDeallocations == ta.numDeallocations());
        ASSERT(numDefaultAllocations == defaultAllocator.numAllocations());
        ASSERT(++numDefaultDeallocations ==
                                          defaultAllocator.numDeallocations());

        {
            numDeletes = 0;
            Obj y(new (ta) TObj(&numDeletes), &ta, 0); const Obj& Y=y;
            numAllocations = ta.numAllocations();
            numDeallocations = ta.numDeallocations();
            ASSERT(0 == numDeletes);
            y.load(static_cast<TObj *>(0), &ta);
            ASSERT(1 == numDeletes);
            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(++numDeallocations == ta.numDeallocations());
            ASSERT(0 == Y.get());
            ASSERT(1 == Y.use_count());
        }
        ASSERT(numAllocations == ta.numAllocations());
        ASSERT(++numDeallocations == ta.numDeallocations());

        {
            numDeletes = 0;
            Obj z(new (ta) TObj(&numDeletes), &ta, 0); const Obj& Z=z;
            numAllocations = ta.numAllocations();
            numDeallocations = ta.numDeallocations();
            ASSERT(0 == numDeletes);
            z.load(static_cast<TObj *>(0), &ta, &ta);
            ASSERT(1 == numDeletes);
            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(++numDeallocations == ta.numDeallocations());
            ASSERT(0 == Z.get());
            ASSERT(1 == Z.use_count());
        }
        ASSERT(numAllocations == ta.numAllocations());
        ASSERT(++numDeallocations == ta.numDeallocations());


        if (verbose) printf(
                     "\nTesting load of non-null ptr (on non-empty object)"
                     "\n--------------------------------------------------\n");

        {
            numDeletes = 0;
            Obj x(new (ta) TObj(&numDeletes), &ta, 0); const Obj& X=x;
            numAllocations = ta.numAllocations();
            numDeallocations = ta.numDeallocations();
            ASSERT(0 == numDeletes);
            TObj *p = new TObj(&numDeletes);
            x.load(p);
            ASSERT(1 == numDeletes);
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(++numDeallocations == ta.numDeallocations());
            ASSERT(p == X.get());
            ASSERT(1 == X.use_count());
        }
        ASSERT(2 == numDeletes);
        ASSERT(numAllocations == ta.numAllocations());
        ASSERT(numDeallocations == ta.numDeallocations());

        {
            numDeletes = 0;
            Obj y(new (ta) TObj(&numDeletes), &ta, 0); const Obj& Y=y;
            numAllocations = ta.numAllocations();
            numDeallocations = ta.numDeallocations();
            ASSERT(0 == numDeletes);
            TObj *p = new(ta) TObj(&numDeletes);
            numAllocations = ta.numAllocations();
            y.load(p, &ta);
            ASSERT(1 == numDeletes);
            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(++numDeallocations == ta.numDeallocations());
            ASSERT(p == Y.get());
            ASSERT(1 == Y.use_count());
        }
        ASSERT(2 == numDeletes);
        ASSERT(numAllocations == ta.numAllocations());
        ASSERT((numDeallocations+2) == ta.numDeallocations());

        {
            numDeletes = 0;
            Obj z(new (ta) TObj(&numDeletes), &ta, 0); const Obj& Z=z;
            numAllocations = ta.numAllocations();
            numDeallocations = ta.numDeallocations();
            ASSERT(0 == numDeletes);
            TObj *p = new(ta) TObj(&numDeletes);
            numAllocations = ta.numAllocations();
            z.load(p, &ta, &ta);
            ASSERT(1 == numDeletes);
            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(++numDeallocations == ta.numDeallocations());
            ASSERT(p == Z.get());
            ASSERT(1 == Z.use_count());
            ASSERT(numAllocations == ta.numAllocations());
        }
        ASSERT(2 == numDeletes);
        ASSERT(numAllocations == ta.numAllocations());
        ASSERT((numDeallocations+2) == ta.numDeallocations());
#endif // BDE_OMIT_INTERNAL_DEPRECATED
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Testing:
        //   BREATHING TEST (shared_ptr)
        //   BREATHING TEST (weak_ptr)
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        if (verbose) printf("\nTesting shared_ptr"
                            "\n------------------\n");

        // Ensure that the typedefs 'element_type' and 'weak_type' in
        // shared_ptr are correct.
        ASSERT((bsl::is_same<int,
                             bsl::shared_ptr<int>::element_type>::value));
        ASSERT((bsl::is_same<bsl::weak_ptr<int>,
                             bsl::shared_ptr<int>::weak_type>::value));
        ASSERT((bsl::is_same<float,
                             bsl::shared_ptr<float[5]>::element_type>::value));
        ASSERT((bsl::is_same<bsl::weak_ptr<float[5]>,
                             bsl::shared_ptr<float[5]>::weak_type>::value));
#ifndef BSLSTL_SHAREDPTR_DONT_TEST_UNBOUNDED_ARRAYS
        ASSERT((bsl::is_same<long,
                             bsl::shared_ptr<long[]>::element_type>::value));
        ASSERT((bsl::is_same<bsl::weak_ptr<long[]>,
                             bsl::shared_ptr<long[]>::weak_type>::value));
#endif

        // Ensure that the typedef 'element_type' in weak_ptr is correct.
        ASSERT((bsl::is_same<int,
                             bsl::weak_ptr<int>::element_type>::value));
        ASSERT((bsl::is_same<float,
                             bsl::weak_ptr<float[5]>::element_type>::value));
#ifndef BSLSTL_SHAREDPTR_DONT_TEST_UNBOUNDED_ARRAYS
        ASSERT((bsl::is_same<long,
                             bsl::weak_ptr<long[]>::element_type>::value));
#endif

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


        {
            typedef DeleteChecker D;
            D *p1 = new D;
            D *p2 = new D[5];

            D::s_singleDeleteCount = 0;
            D::s_arrayDeleteCount = 0;
            {
                bsl::shared_ptr<D> sp1(p1);
                {
                    bsl::shared_ptr<D[5]> sp2(p2);
                }
                ASSERT(1 == D::s_arrayDeleteCount);
                ASSERT(0 == D::s_singleDeleteCount);
            }
            ASSERT(1 == D::s_arrayDeleteCount);
            ASSERT(1 == D::s_singleDeleteCount);

            //  (1) This should not call operator delete
            //  (2) There is no make_shared<T[]>
            {
                bsl::shared_ptr<D> sp3 = bsl::make_shared<D>();
            }
            ASSERT(1 == DeleteChecker::s_arrayDeleteCount);
            ASSERT(1 == DeleteChecker::s_singleDeleteCount);
        }


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

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            ObjSP mT = mX.acquireSharedPtr(); const ObjSP& T = mT;
            ASSERT(mT);
            ASSERT(!X.expired());
            ASSERT(2 == A.use_count());
            ASSERT(S == T);
#endif // BDE_OMIT_INTERNAL_DEPRECATED
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
        // Concerns: We should have a benchmark to time performance and keep
        //   track of performance improvements or losses as the code evolves.
        //   Performance here means both runtime and memory usage.
        //
        // Test plan: First measure the basics: 1.  time to construct and
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
// Copyright 2023 Bloomberg Finance L.P.
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
