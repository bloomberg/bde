// bslma_allocatorutil.h                                              -*-C++-*-
#ifndef INCLUDED_BSLMA_ALLOCATORUTIL
#define INCLUDED_BSLMA_ALLOCATORUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a namespace for utility functions on allocators.
//
//@CLASSES:
//  bslma::AllocatorUtil: Namespace for utility functions on allocators
//
//@SEE_ALSO: bslma_aatypeutil, bslma_allocatortraits
//
//@DESCRIPTION: This component provides a namespace `struct`,
// `bslma::AllocatorUtil`, with functions that operate on both raw pointers to
// `bslma::Allocator` or derived classes and objects of C++11 compliant
// allocator classes.  The functions in this utility `struct` also free the
// user from worrying about rebinding the allocator and creating copies of
// rebound allocators.  Operations provided include `allocateBytes` and
// `deallocateBytes` to acquire and free raw bytes; `allocateObject` and
// `deallocateObject` to aquire and free uninitialized object storages; and
// `newObject` and `deleteObject` to allocate+construct and destroy+deallocate
// full objects.  There are also operations for conditionally assigning or
// swapping allocator objects themselves, depending on the allocator's
// propagation traits.
//
///Usage
///-----
//
///Example 1: Future-proofing Member Construction
/// - - - - - - - - - - - - - - - - - - - - - - -
// This example shows how we construct an AA member variable, using
// `bslma::AllocatorUtil::adapt` so that it is both self-documenting and robust
// in case the member type is modernized from *legacy-AA* (using
// `bslma::Allocator *` directly in its interface) to *bsl-AA* (using
// `bsl::allocator` in its interface).
//
// First, we define a class, `Data1`, that has a legacy-AA interface:
// ```
// /// Legacy-AA data class.
// class Data1 {
//
//     bslma::Allocator *d_allocator_p;
//     // ...
//
//   public:
//     explicit Data1(bslma::Allocator *basicAllocator = 0)
//         : d_allocator_p(basicAllocator) { /* ... */ }
//
//     bslma::Allocator *allocator() const { return d_allocator_p; }
// };
// ```
// Next, we define a class, `MyClass1`, that has a member of type `Data1`.
// `MyClass` uses a modern, bsl-AA interface:
// ```
// class MyClass1 {
//     bsl::allocator<char> d_allocator;
//     Data1                d_data;
//
//   public:
//     typedef bsl::allocator<char> allocator_type;
//
//     explicit MyClass1(const allocator_type& allocator = allocator_type());
//
//     const Data1& data() const { return d_data; }
//     allocator_type get_allocator() const { return d_allocator; }
// };
// ```
// Next, we define the constructor for `MyClass1`.  Since `MyClass1` uses
// `bsl::allocator` and the `Data1` uses `bslma::Allocator *`, we employ
// `bslma::AllocatorUtil::adapt` to obtain an allocator suitable for passing to
// the constructor for `d_data`:
// ```
// MyClass1::MyClass1(const allocator_type& allocator)
//     : d_allocator(allocator)
//     , d_data(bslma::AllocatorUtil::adapt(allocator))
// {
// }
// ```
// Next, assume that we update our `Data` class from legacy-AA to bsl-AA
// (renamed from `Data1` to `Data2` for illustrative purposes):
// ```
// /// Bsl-AA data class.
// class Data2 {
//
//     bsl::allocator<int> d_allocator;
//     // ...
//
//   public:
//     typedef bsl::allocator<int> allocator_type;
//
//     explicit Data2(const allocator_type& allocator = allocator_type())
//         : d_allocator(allocator) { /* ... */ }
//
//     allocator_type get_allocator() const { return d_allocator; }
// };
// ```
// Now, we notice that **nothing** about `MyClass` needs to change, not even
// the way its constructor passes an allocator to `d_data`:
// ```
// class MyClass2 {
//     bsl::allocator<char> d_allocator;
//     Data2                d_data;
//
//   public:
//     typedef bsl::allocator<char> allocator_type;
//
//     explicit MyClass2(const allocator_type& allocator = allocator_type());
//
//     const Data2& data() const { return d_data; }
//     allocator_type get_allocator() const { return d_allocator; }
// };
//
// MyClass2::MyClass2(const allocator_type& allocator)
//     : d_allocator(allocator)
//     , d_data(bslma::AllocatorUtil::adapt(allocator))
// {
// }
// ```
// Finally, we test both versions of `MyClass` and show that the allocator that
// is passed to the `MyClass` constructor gets forwarded to its data member:
// ```
// int main()
// {
//     bslma::TestAllocator ta;
//     bsl::allocator<char> alloc(&ta);
//
//     MyClass1 obj1(alloc);
//     assert(&ta == obj1.data().allocator());
//
//     MyClass2 obj2(alloc);
//     assert(alloc == obj2.data().get_allocator());
// }
// ```
//
///Example 2: Building an AA object on the heap
///- - - - - - - - - - - - - - - - - - - - - -
// This example shows how we can allocate a *bsl-AA* object from an allocator
// and construct the object, passing the allocator along, in one step.
//
// First, we define a simple class, `BslAAType`, that uses `bsl::allocator` to
// allocate memory (i.e., it is *bsl-AA*):
// ```
// #include <bslma_bslallocator.h>
// class BslAAType {
//     bsl::allocator<> d_allocator;
//     int              d_value;
//
//   public:
//     typedef bsl::allocator<> allocator_type;
//
//     explicit BslAAType(const allocator_type& a = allocator_type())
//         : d_allocator(a), d_value(0) { }
//     explicit BslAAType(int v, const allocator_type& a = allocator_type())
//         : d_allocator(a), d_value(v) { }
//
//     allocator_type get_allocator() const { return d_allocator; }
//     int            value()         const { return d_value;     }
// };
// ```
// Now we can use `bslma::AllocatorUtil::newObject` to, in a single operation,
// allocate and construct an `BslAAType` object.  We can see that the right
// allocator and value are passed to the new object:
// ```
// #include <bslma_testallocator.h>
// int main()
// {
//     bslma::TestAllocator ta;
//     BslAAType *p = bslma::AllocatorUtil::newObject<BslAAType>(&ta, 77);
//     assert(sizeof(BslAAType) == ta.numBytesInUse());
//     assert(77 == p->value());
//     assert(&ta == p->get_allocator().mechanism());
// ```
// Finally, we use `deleteObject` to destroy and return the object to the
// allocator:
// ```
//     bslma::AllocatorUtil::deleteObject(&ta, p);
//     assert(0 == ta.numBytesInUse());
// }
// ```
//
///Example 3: Safe container swap
///- - - - - - - - - - - - - - -
// In this example, we see how `bslma::AllocatorUtil::swap` can be used to swap
// allocators without the risk of calling a non-existant swap.
//
// First, we create a class, `StdAAType`, that uses any valid STL-compatible
// allocator (i.e., it is *stl-AA*).  Note that this class has non-default copy
// constructor and assignment operations (whose implementation is not shown)
// and a non-default `swap` operation:
// ```
// template <class t_TYPE, class t_ALLOCATOR = bsl::allocator<t_TYPE> >
// class StlAAType {
//     t_ALLOCATOR  d_allocator;
//     t_TYPE      *d_value_p;
//
//   public:
//     typedef t_ALLOCATOR allocator_type;
//
//     explicit StlAAType(const allocator_type& a = allocator_type())
//         : d_allocator(a)
//         , d_value_p(bslma::AllocatorUtil::newObject<t_TYPE>(a)) { }
//     explicit StlAAType(const t_TYPE&           v,
//                        const allocator_type& a = allocator_type())
//         : d_allocator(a)
//         , d_value_p(bslma::AllocatorUtil::newObject<t_TYPE>(a, v)) { }
//
//     StlAAType(const StlAAType&);
//
//     ~StlAAType() {
//         bslma::AllocatorUtil::deleteObject(d_allocator, d_value_p);
//     }
//
//     StlAAType operator=(const StlAAType&);
//
//     void swap(StlAAType& other);
//
//     allocator_type get_allocator() const { return d_allocator; }
//     const t_TYPE&    value()         const { return *d_value_p;  }
// };
//
// template <class t_TYPE, class t_ALLOCATOR>
// inline void swap(StlAAType<t_TYPE, t_ALLOCATOR>& a,
//                  StlAAType<t_TYPE, t_ALLOCATOR>& b)
// {
//     a.swap(b);
// }
// ```
// Next, we write the `swap` member function.  This function should follow our
// standard AA rule for member swap: if the allocators compare equal or if the
// allocators should propagate on swap, then perform a fast swap, moving only
// pointers and (possibly) allocators, rather than copying elements; otherwise
// revert to element-by-element swap:
// ```
// template <class t_TYPE, class t_ALLOCATOR>
// void StlAAType<t_TYPE, t_ALLOCATOR>::swap(StlAAType& other)
// {
//     typedef typename
//         bsl::allocator_traits<allocator_type>::propagate_on_container_swap
//         Propagate;
//
//     using std::swap;
//
//     if (Propagate::value || d_allocator == other.d_allocator) {
//         // Swap allocators and pointers, but not individual elements.
//         bslma::AllocatorUtil::swap(&d_allocator, &other.d_allocator,
//                                    Propagate());
//         swap(d_value_p, other.d_value_p);
//     }
//     else
//     {
//         // Swap element values
//         swap(*d_value_p, *other.d_value_p);
//     }
// }
// ```
// Note that, in the above implementation of `swap`, that we swap the
// allocators using `bslma::AllocatorUtil::swap` instead of calling `swap`
// directly.  If the `t_ALLOCATOR` type does not propagate on container
// assignment or swap, the allocator itself is not required to support
// assignment or swap.  By using this utility, we avoid trying to compile a
// call to allocator `swap` when it is not needed.
//
// Next, we'll define an allocator that illustrates this point.  Our `MyAlloc`
// allocator does not support allocator propogation and deletes the assignment
// operators (thus also disabling swap):
// ```
// #include <bsls_keyword.h>
//
// template <class t_TYPE>
// class MyAlloc {
//     bsl::allocator<t_TYPE> d_imp;
//
//     // Disable assignment
//     MyAlloc operator=(const MyAlloc&) BSLS_KEYWORD_DELETED;
//
//   public:
//     typedef t_TYPE value_type;
//
//     MyAlloc() { }
//     MyAlloc(bslma::Allocator *allocPtr) : d_imp(allocPtr) { }   // IMPLICIT
//     template <class U>
//     MyAlloc(const MyAlloc<U>& other) : d_imp(other.d_imp) { }
//
//     t_TYPE *allocate(std::size_t n) { return d_imp.allocate(n); }
//     void deallocate(t_TYPE* p, std::size_t n) { d_imp.deallocate(p, n); }
//
//     template <class T2>
//     friend bool operator==(const MyAlloc& a, const MyAlloc<T2>& b)
//         { return a.d_imp == b.d_imp; }
//     template <class T2>
//     friend bool operator!=(const MyAlloc& a, const MyAlloc<T2>& b)
//         { return a.d_imp != b.d_imp; }
// };
// ```
// Finally, we create two `StlAAType` objects with the same allocator and show
// that they can be swapped even though the allocator type cannot be swapped:
// ```
// int main()
// {
//     MyAlloc<int> alloc;
//
//     StlAAType<int, MyAlloc<int> > objA(1, alloc), objB(2, alloc);
//     assert(alloc == objA.get_allocator());
//     assert(alloc == objB.get_allocator());
//     assert(1 == objA.value());
//     assert(2 == objB.value());
//
//     objA.swap(objB);
//     assert(2 == objA.value());
//     assert(1 == objB.value());
// }
// ```

#include <bslscm_version.h>

#include <bslma_allocator.h>
#include <bslma_allocatortraits.h>
#include <bslma_memoryresource.h>
#include <bslma_polymorphicallocator.h>
#include <bslma_bslallocator.h>

#include <bslmf_assert.h>
#include <bslmf_enableif.h>
#include <bslmf_isconst.h>
#include <bslmf_isconvertible.h>
#include <bslmf_integralconstant.h>
#include <bslmf_isvolatile.h>

#include <bsls_alignmentfromtype.h>
#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_exceptionutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_util.h>

#include <algorithm>  // 'std::swap'

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Include version that can be compiled with C++03
// Generated on Sat Jul 23 20:07:37 2022
// Command line: sim_cpp11_features.pl bslma_allocatorutil.h
# define COMPILING_BSLMA_ALLOCATORUTIL_H
# include <bslma_allocatorutil_cpp03.h>
# undef COMPILING_BSLMA_ALLOCATORUTIL_H
#else

namespace BloombergLP {
namespace bslma {

// FORWARD DECLARATIONS
template <class t_ALLOCATOR, class t_TYPE = char>
struct AllocatorUtil_Traits;

                        // ===================
                        // class AllocatorUtil
                        // ===================

/// Namespace for utility functions on allocators
struct AllocatorUtil {

  private:
    // PRIVATE CONSTANTS
    enum { k_MAX_ALIGNMENT = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT };

    // PRIVATE CLASS METHODS
    template <class t_TYPE>
    static char matchBslAlloc(bsl::allocator<t_TYPE> *, int);
    static long matchBslAlloc(void *,                 ...);
        // DECLARED BUT NOT DEFINED

    /// Return the specified `nbytes` raw bytes having the specified
    /// `alignment` allocated from the specified `allocator`.  If
    /// `alignment` is larger than the largest supported alignment, the
    /// behavior is determined by the allocator.
    template <class t_TYPE>
    static
    void *allocateBytesImp(
                          const bsl::polymorphic_allocator<t_TYPE>& allocator,
                          std::size_t                               nbytes,
                          std::size_t                               alignment);
    template <class t_TYPE>
    static
    void *allocateBytesImp(const bsl::allocator<t_TYPE>& allocator,
                           std::size_t                   nbytes,
                           std::size_t                   alignment);
    template <class t_ALLOCATOR>
    static
    typename AllocatorUtil_Traits<t_ALLOCATOR>::void_pointer
    allocateBytesImp(const t_ALLOCATOR&          allocator,
                     std::size_t                 nbytes,
                     std::size_t                 alignment);

    /// Return, to the specified `allocator`, the block of raw memory at the
    /// specified `p` address having the specified `nbytes` size and the
    /// specified `alignment`.  The behavior is undefined unless `p` refers
    /// to a block having the same size and alignment allocated from a copy
    /// of `allocator` and not yet deallocated.
    template <class t_TYPE>
    static void
    deallocateBytesImp(const bsl::polymorphic_allocator<t_TYPE>&  allocator,
                       void                                      *p,
                       std::size_t                                nbytes,
                       std::size_t                                alignment);
    template <class t_TYPE>
    static void
    deallocateBytesImp(const bsl::allocator<t_TYPE>&  allocator,
                       void                          *p,
                       std::size_t                    nbytes,
                       std::size_t                    alignment);
    template <class t_ALLOCATOR>
    static void deallocateBytesImp(
        const t_ALLOCATOR&                                       allocator,
        typename AllocatorUtil_Traits<t_ALLOCATOR>::void_pointer p,
        std::size_t                                              nbytes,
        std::size_t                                              alignment);

    template <class t_ALLOCATOR, class t_POINTER, class t_VALUE_TYPE>
    static void deallocateObjectImp(const t_ALLOCATOR&  allocator,
                                    t_POINTER           p,
                                    std::size_t         n,
                                    const t_VALUE_TYPE& );

    template <class t_ALLOCATOR, class t_POINTER, class t_VALUE_TYPE>
    static void deleteObjectImp(const t_ALLOCATOR&  allocator,
                                t_POINTER           p,
                                const t_VALUE_TYPE& );

    /// Return `true` if the specified `alignment` is a (positive) power of
    /// two; otherwise return false.
    static bool isPowerOf2(std::size_t alignment);

    // PRIVATE TYPES

    /// Metafunction derives from `true_type` if (template argument)
    /// `t_ALLOC` is derived from any specialization of `bsl::allocator`;
    /// else derives from `false_type`.
    template <class t_ALLOC>
    struct IsDerivedFromBslAllocator
        : bsl::integral_constant<bool,
                                 1 == sizeof(matchBslAlloc((t_ALLOC *) 0, 0))>
    {
    };

  public:
    // CLASS METHODS

    /// Return the specified `from` allocator adapted to a type most likely
    /// to be usable for initializing another AA object.  Specifically,
    /// return `from.mechanism()` if `from` is a specialization of
    /// `bsl::allocator` (or a class derived from `bsl::allocator`);
    /// otherwise return `from` unchanged.
    template <class t_ALLOC>
    static typename bsl::enable_if<
        ! IsDerivedFromBslAllocator<t_ALLOC>::value,
        t_ALLOC>::type
    adapt(const t_ALLOC& from);
    template <class t_TYPE>
    static bslma::Allocator *adapt(const bsl::allocator<t_TYPE>& from);

    /// Return a pointer to a block of raw memory allocated from the
    /// specified `allocator` having the specified `nbytes` size and
    /// optionally specified `alignment`.  If `alignment` is larger than the
    /// largest supported alignment, either the block will be aligned to the
    /// maximum supported alignment or an exception will be thrown.  The
    /// specific choice of behavior is determined by the allocator: for
    /// polymorphic allocators the behavior is determined by the memory
    /// resource, whereas for non-polymorphic allocators, the alignment is
    /// always truncated to the maximum non-extended alignment.
    template <class t_ALLOCATOR>
    static typename AllocatorUtil_Traits<t_ALLOCATOR>::void_pointer
    allocateBytes(const t_ALLOCATOR& allocator,
                  std::size_t        nbytes,
                  std::size_t        alignment = k_MAX_ALIGNMENT);

    /// Return a pointer to a block of raw memory allocated from the
    /// specified `allocator` having a size and alignment appropriate for an
    /// object of (templatize parameter) `t_TYPE`.  Optionally specify `n`
    /// for the number of objects; otherwise space for a single object is
    /// allocated.  Since `t_TYPE` cannot be deduced from the function
    /// parameters, it must be supplied explicitly (in `<>` brackets) by the
    /// caller.
    template <class t_TYPE, class t_ALLOCATOR>
    static typename AllocatorUtil_Traits<t_ALLOCATOR, t_TYPE>::pointer
    allocateObject(const t_ALLOCATOR& allocator, std::size_t n = 1);

    /// If the specified `allowed` tag is `bsl::true_type` assign the
    /// allocator object at the specified `lhs` address the value of the
    /// specified `rhs`; otherwise, do nothing, and, in both cases, return a
    /// modifiable reference to `*lhs`.  The `t_TYPE` template parameter is
    /// typically an allocator type and the `allowed` flag is typically a
    /// propagation trait dependant on the calling context, such as
    /// `propagate_on_container_copy_assignment` or
    /// `propagate_on_container_move_assignment`.  Instantiation will fail
    /// if `allowed` is `true_type` and `t_TYPE` lacks a publically
    /// accessible copy assignment operator.  The behavior is undefined
    /// unless `allowed` is `true_type` or `*lhs == rhs` before the call.
    template <class t_TYPE>
    static t_TYPE& assign(t_TYPE         *lhs,
                          const t_TYPE&   rhs,
                          bsl::true_type  allowed);
    template <class t_TYPE>
    static t_TYPE& assign(t_TYPE          *lhs,
                          const t_TYPE&    rhs,
                          bsl::false_type  allowed);

    /// Return to the specified allocator the block raw memory at the
    /// specified `p` address having the specified `nbytes` size and
    /// optionally specified `alignment`.  The behavior is undefined unless
    /// `p` refers to a block having the same size and alignment previously
    /// allocated from a copy of `allocator` and not yet deallocated.
    template <class t_ALLOCATOR>
    static void deallocateBytes(
        const t_ALLOCATOR&                                       allocator,
        typename AllocatorUtil_Traits<t_ALLOCATOR>::void_pointer p,
        std::size_t                                              nbytes,
        std::size_t                                              alignment
                                                            = k_MAX_ALIGNMENT);

    /// Return to the specified `allocator` a block of raw memory at the
    /// specified `p` address that is suitably sized and aligned to hold an
    /// object of (templatize parameter) `t_TYPE`.  Optionally specify `n`
    /// for the number of objects; otherwise a single object is assumed.
    /// The behavior is undefined unless `p` refers to a block with the same
    /// type and number of objects previously allocated from a copy of
    /// `allocator` and not yet deallocated.
    template <class t_ALLOCATOR, class t_POINTER>
    static void deallocateObject(const t_ALLOCATOR& allocator,
                                 t_POINTER          p,
                                 std::size_t        n = 1);

    /// Destroy the object at the specified `p` address and return the block
    /// of memory at `p` to the specified `allocator`.  The behavior is
    /// undefined unless `p` refers to a fully constructed object allocated
    /// from a copy of `allocator` and not yet destroyed or deallocated.
    template <class t_ALLOCATOR, class t_POINTER>
    static void deleteObject(const t_ALLOCATOR& allocator, t_POINTER p);

    /// Return an object of (template parameter) `t_TYPE` allocated from the
    /// specified `allocator` and constructed with no arguments except that,
    /// for scoped allocator types such as `bsl::allocator` and
    /// `bsl::polymorphic_allocator`, `allocator` may be passed to the
    /// `t_TYPE` constructor (i.e., if `t_TYPE` is AA).
    template <class t_TYPE, class t_ALLOCATOR>
    static typename AllocatorUtil_Traits<t_ALLOCATOR, t_TYPE>::pointer
    newObject(const t_ALLOCATOR& allocator);

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=13
# ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    // 't_ARG1' lvalue overloaded unneeded in C++11 and hits bug in gcc < 10.2.
    template <class t_TYPE, class t_ALLOCATOR, class t_ARG1, class... t_ARGS>
    static typename AllocatorUtil_Traits<t_ALLOCATOR, t_TYPE>::pointer
    newObject(const t_ALLOCATOR& allocator,
              t_ARG1&            argument1,
              t_ARGS&&...        arguments);
# endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    /// Return an object of (template parameter) `t_TYPE` allocated from the
    /// specified `allocator` and constructed from the specified `argument1`
    /// and other specified `arguments`.  For scoped allocator types such as
    /// `bsl::allocator` and `bsl::polymorphic_allocator`, `allocator` may
    /// be passed to the `t_TYPE` constructor as an additional argument
    /// (i.e., if `t_TYPE` is AA).  Note that, in C++03, perfect forwarding
    /// is limited such that any lvalue reference in the `arguments`
    /// parameter pack is const-qualified when forwarded to the
    /// `TARGET_TYPE` constructor; only `argument1` can be forwarded as an
    /// unqualified lvalue.
    template <class t_TYPE, class t_ALLOCATOR, class t_ARG1, class... t_ARGS>
    static typename AllocatorUtil_Traits<t_ALLOCATOR, t_TYPE>::pointer
    newObject(const t_ALLOCATOR&                        allocator,
              BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG1) argument1,
              t_ARGS&&...                               arguments);
#endif

    /// If the specified `allowed` tag is `bsl::true_type`, swap the values
    /// of allocators at the specified `pa` and `pb` addresses using ADL
    /// swap (with `std::swap` in scope); otherwise, do nothing.  The
    /// `t_TYPE` template parameter is typically an allocator type and the
    /// `allowed` flag is typically a propagation trait dependant on the
    /// calling context, such as `propagate_on_container_swap`.
    /// Instantiation will fail if `allowed` is `false_type` and `t_TYPE` is
    /// not swappable (i.e., because it lacks a publically available
    /// assignment operator).  The behavior is undefined unless `allowed` is
    /// `true_type` or '*pa ==
    /// *pb' before the call.
    template <class t_TYPE>
    static void swap(t_TYPE *pa, t_TYPE *pb, bsl::false_type allowed);
    template <class t_TYPE>
    static void swap(t_TYPE *pa, t_TYPE *pb, bsl::true_type  allowed);
};

// ============================================================================
//                TEMPLATE AND INLINE FUNCTION IMPLEMENTATIONS
// ============================================================================

                    // --------------------------
                    // class AllocatorUtil_Traits
                    // --------------------------

/// Extend the notion of `allocator_traits` to apply to both standard
/// allocator and to pointer-to-memory-resource types.  If the (template
/// parameter) `t_ALLOCATOR` is a non-pointer type (i.e., an allocator
/// class), then inherits from
/// `bsl::allocator_traits<t_ALLOCATOR>::rebind_traits<t_TYPE>`.  However,
/// if `t_ALLOCATOR` is a pointer type, then inherits from
/// `bsl::allocator_traits<bsl::allocator<t_TYPE>>` for pointers to classes
/// derived from `bslma::Allocator` and from
/// `bsl::allocator_traits<bsl::polymorphic_allocator<t_TYPE>>` for pointers
/// to other classes derived from `bsl::memory_resource`.  This primary
/// template is for non-pointer `t_ALLOCATOR` template arguments.
template <class t_ALLOCATOR, class t_TYPE>
struct AllocatorUtil_Traits
    : bsl::allocator_traits<t_ALLOCATOR>::template rebind_traits<t_TYPE> {

    BSLMF_ASSERT(! bsl::is_const<t_TYPE>::value);
    BSLMF_ASSERT(! bsl::is_volatile<t_TYPE>::value);
};

/// This specialization is for allocators expressed as a pointer to class
/// derived from `bsl::memory_resource`.  The base class will be
/// `bsl::allocator_traits<bsl::allocator<t_TYPE>>` if `t_MEMORY_RSRC` is
/// derived from `bsl::Allocator`; otherwise the base class will be
/// `bsl::allocator_traits<bsl::polymorphic_allocator<t_TYPE>>`.
template <class t_MEMORY_RSRC, class t_TYPE>
struct AllocatorUtil_Traits<t_MEMORY_RSRC *, t_TYPE> : bsl::allocator_traits<
    typename bsl::conditional<
            bsl::is_convertible<t_MEMORY_RSRC *, bslma::Allocator *>::value,
            bsl::allocator<t_TYPE>,
            bsl::polymorphic_allocator<t_TYPE> >::type
    >
{

    // MANDATES
    BSLMF_ASSERT((bsl::is_convertible<t_MEMORY_RSRC *,
                                      bsl::memory_resource *>::value));
    BSLMF_ASSERT(! bsl::is_const<t_TYPE>::value);
    BSLMF_ASSERT(! bsl::is_volatile<t_TYPE>::value);
};

                        // -------------------
                        // class AllocatorUtil
                        // -------------------

// PRIVATE CLASS METHODS
template <class t_TYPE>
inline
void *AllocatorUtil::allocateBytesImp(
                       const bsl::polymorphic_allocator<t_TYPE>& allocator,
                       std::size_t                               nbytes,
                       std::size_t                               alignment)
{
    return allocator.resource()->allocate(nbytes, alignment);
}

template <class t_TYPE>
inline
void *AllocatorUtil::allocateBytesImp(const bsl::allocator<t_TYPE>& allocator,
                                      std::size_t                   nbytes,
                                      std::size_t                   alignment)
{
    return allocator.resource()->allocate(nbytes, alignment);
}

template <class t_ALLOCATOR>
typename AllocatorUtil_Traits<t_ALLOCATOR>::void_pointer
AllocatorUtil::allocateBytesImp(const t_ALLOCATOR& allocator,
                                std::size_t        nbytes,
                                std::size_t        alignment)
{
    BSLMF_ASSERT(4 <= k_MAX_ALIGNMENT && k_MAX_ALIGNMENT <= 32);

    static const int k_8  = k_MAX_ALIGNMENT <  8 ? k_MAX_ALIGNMENT :  8;
    static const int k_16 = k_MAX_ALIGNMENT < 16 ? k_MAX_ALIGNMENT : 16;

    typedef typename bsls::AlignmentToType<  1>::Type             AlignType1;
    typedef typename bsls::AlignmentToType<  2>::Type             AlignType2;
    typedef typename bsls::AlignmentToType<  4>::Type             AlignType4;
    typedef typename bsls::AlignmentToType<k_8>::Type             AlignType8;
    typedef typename bsls::AlignmentToType<k_16>::Type            AlignType16;
    typedef typename bsls::AlignmentToType<k_MAX_ALIGNMENT>::Type AlignTypeMax;

    if (alignment > k_MAX_ALIGNMENT) {
        alignment = k_MAX_ALIGNMENT;
    }

    std::size_t n = (nbytes + alignment - 1) / alignment;

    switch (alignment) {
      case  1: return allocateObject<AlignType1  >(allocator, n);
      case  2: return allocateObject<AlignType2  >(allocator, n);
      case  4: return allocateObject<AlignType4  >(allocator, n);
      case  8: return allocateObject<AlignType8  >(allocator, n);
      case 16: return allocateObject<AlignType16 >(allocator, n);
      default: return allocateObject<AlignTypeMax>(allocator, n);
    }
}

template <class t_TYPE>
inline
void AllocatorUtil::deallocateBytesImp(
                          const bsl::polymorphic_allocator<t_TYPE>&  allocator,
                          void                                      *p,
                          std::size_t                                nbytes,
                          std::size_t                                alignment)
{
    return allocator.resource()->deallocate(p, nbytes, alignment);
}

template <class t_TYPE>
inline
void AllocatorUtil::deallocateBytesImp(
                                      const bsl::allocator<t_TYPE>&  allocator,
                                      void                          *p,
                                      std::size_t                    nbytes,
                                      std::size_t                    alignment)
{
    return allocator.resource()->deallocate(p, nbytes, alignment);
}

template <class t_ALLOCATOR>
void AllocatorUtil::deallocateBytesImp(
    const t_ALLOCATOR&                                       allocator,
    typename AllocatorUtil_Traits<t_ALLOCATOR>::void_pointer p,
    std::size_t                                              nbytes,
    std::size_t                                              alignment)
{
    BSLMF_ASSERT(4 <= k_MAX_ALIGNMENT && k_MAX_ALIGNMENT <= 32);

    static const int k_8  = k_MAX_ALIGNMENT <  8 ? k_MAX_ALIGNMENT :  8;
    static const int k_16 = k_MAX_ALIGNMENT < 16 ? k_MAX_ALIGNMENT : 16;

    typedef typename bsls::AlignmentToType<  1>::Type             AlignType1;
    typedef typename bsls::AlignmentToType<  2>::Type             AlignType2;
    typedef typename bsls::AlignmentToType<  4>::Type             AlignType4;
    typedef typename bsls::AlignmentToType<k_8>::Type             AlignType8;
    typedef typename bsls::AlignmentToType<k_16>::Type            AlignType16;
    typedef typename bsls::AlignmentToType<k_MAX_ALIGNMENT>::Type AlignTypeMax;

    typedef typename AllocatorUtil_Traits<t_ALLOCATOR,
                                          AlignType1  >::pointer Ptr1;
    typedef typename AllocatorUtil_Traits<t_ALLOCATOR,
                                          AlignType2  >::pointer Ptr2;
    typedef typename AllocatorUtil_Traits<t_ALLOCATOR,
                                          AlignType4  >::pointer Ptr4;
    typedef typename AllocatorUtil_Traits<t_ALLOCATOR,
                                          AlignType8  >::pointer Ptr8;
    typedef typename AllocatorUtil_Traits<t_ALLOCATOR,
                                          AlignType16 >::pointer Ptr16;
    typedef typename AllocatorUtil_Traits<t_ALLOCATOR,
                                          AlignTypeMax>::pointer PtrMax;

    if (alignment > k_MAX_ALIGNMENT) {
        alignment = k_MAX_ALIGNMENT;
    }

    std::size_t n = (nbytes + alignment - 1) / alignment;

    switch (alignment) {
      case  1: deallocateObject(allocator, static_cast<Ptr1  >(p), n); break;
      case  2: deallocateObject(allocator, static_cast<Ptr2  >(p), n); break;
      case  4: deallocateObject(allocator, static_cast<Ptr4  >(p), n); break;
      case  8: deallocateObject(allocator, static_cast<Ptr8  >(p), n); break;
      case 16: deallocateObject(allocator, static_cast<Ptr16 >(p), n); break;
      default: deallocateObject(allocator, static_cast<PtrMax>(p), n); break;
    }
}

template <class t_ALLOCATOR, class t_POINTER, class t_VALUE_TYPE>
inline
void AllocatorUtil::deallocateObjectImp(const t_ALLOCATOR&  allocator,
                                        t_POINTER           p,
                                        std::size_t         n,
                                        const t_VALUE_TYPE& )
{
    typedef AllocatorUtil_Traits<t_ALLOCATOR, t_VALUE_TYPE> Traits;

    BSLMF_ASSERT((bsl::is_same<t_POINTER, typename Traits::pointer>::value));

    typename Traits::allocator_type reboundAlloc(allocator);
    reboundAlloc.deallocate(p, n);
}

template <class t_ALLOCATOR, class t_POINTER, class t_VALUE_TYPE>
inline
void AllocatorUtil::deleteObjectImp(const t_ALLOCATOR&  allocator,
                                    t_POINTER           p,
                                    const t_VALUE_TYPE& )
{
    typedef AllocatorUtil_Traits<t_ALLOCATOR, t_VALUE_TYPE> Traits;

    BSLMF_ASSERT((bsl::is_same<t_POINTER, typename Traits::pointer>::value));

    typename Traits::allocator_type reboundAlloc(allocator);
    Traits::destroy(reboundAlloc, BSLS_UTIL_ADDRESSOF(*p));
    reboundAlloc.deallocate(p, 1);
}

inline
bool AllocatorUtil::isPowerOf2(std::size_t alignment)
{
    return (0 < alignment) && (0 == (alignment & (alignment - 1)));
}


// CLASS METHODS
template <class t_ALLOC>
inline
typename bsl::enable_if<
        ! AllocatorUtil::IsDerivedFromBslAllocator<t_ALLOC>::value,
        t_ALLOC>::type
AllocatorUtil::adapt(const t_ALLOC& from)
{
    return from;
}

template <class t_TYPE>
inline
bslma::Allocator *AllocatorUtil::adapt(const bsl::allocator<t_TYPE>& from)
{
    return from.mechanism();
}

template <class t_ALLOCATOR>
inline
typename AllocatorUtil_Traits<t_ALLOCATOR>::void_pointer
AllocatorUtil::allocateBytes(const t_ALLOCATOR& allocator,
                             std::size_t        nbytes,
                             std::size_t        alignment)
{
    BSLS_ASSERT(isPowerOf2(alignment));

    typedef
        typename AllocatorUtil_Traits<t_ALLOCATOR>::allocator_type StdAlloc;
    return allocateBytesImp(StdAlloc(allocator), nbytes, alignment);
}

template <class t_TYPE, class t_ALLOCATOR>
inline
typename AllocatorUtil_Traits<t_ALLOCATOR, t_TYPE>::pointer
AllocatorUtil::allocateObject(const t_ALLOCATOR& allocator, std::size_t n)
{
    typename AllocatorUtil_Traits<t_ALLOCATOR, t_TYPE>::allocator_type
        reboundAlloc(allocator);
    return reboundAlloc.allocate(n);
}

template <class t_TYPE>
inline
t_TYPE& AllocatorUtil::assign(t_TYPE *lhs, const t_TYPE& rhs, bsl::false_type)
{
    BSLS_ASSERT(*lhs == rhs);
    (void)rhs;
    return *lhs;
}

template <class t_TYPE>
inline
t_TYPE& AllocatorUtil::assign(t_TYPE *lhs, const t_TYPE& rhs, bsl::true_type)
{
    *lhs = rhs;
    return *lhs;
}

template <class t_ALLOCATOR>
inline
void AllocatorUtil::deallocateBytes(
    const t_ALLOCATOR&                                       allocator,
    typename AllocatorUtil_Traits<t_ALLOCATOR>::void_pointer p,
    std::size_t                                              nbytes,
    std::size_t                                              alignment)
{
    BSLS_ASSERT(isPowerOf2(alignment));

    typedef
        typename AllocatorUtil_Traits<t_ALLOCATOR>::allocator_type StdAlloc;
    deallocateBytesImp(StdAlloc(allocator), p, nbytes, alignment);
}

template <class t_ALLOCATOR, class t_POINTER>
inline
void AllocatorUtil::deallocateObject(const t_ALLOCATOR& allocator,
                                     t_POINTER          p,
                                     std::size_t        n)
{
    BSLS_ASSERT(t_POINTER() != p);
    deallocateObjectImp(allocator, p, n, *p);
}

template <class t_ALLOCATOR, class t_POINTER>
inline void
AllocatorUtil::deleteObject(const t_ALLOCATOR& allocator, t_POINTER p)
{
    BSLS_ASSERT(t_POINTER() != p);
    deleteObjectImp(allocator, p, *p);
}

template <class t_TYPE, class t_ALLOCATOR>
inline
typename AllocatorUtil_Traits<t_ALLOCATOR, t_TYPE>::pointer
AllocatorUtil::newObject(const t_ALLOCATOR& allocator)
{
    typedef AllocatorUtil_Traits<t_ALLOCATOR, t_TYPE> Traits;

    typename Traits::allocator_type reboundAlloc(allocator);
    typename Traits::pointer        p = reboundAlloc.allocate(1);
    // Use a 'try' block because the proctor components are at a higher
    // dependency level than this component.  As there is only one possibly
    // throwing statement, correctness of the 'try' block is easily verified.
    BSLS_TRY {
        Traits::construct(reboundAlloc, BSLS_UTIL_ADDRESSOF(*p));
    }
    BSLS_CATCH(...) {
        reboundAlloc.deallocate(p, 1);
        BSLS_RETHROW;
    }
    return p;
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
# ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
template <class t_TYPE, class t_ALLOCATOR, class t_ARG1, class... t_ARGS>
inline
typename AllocatorUtil_Traits<t_ALLOCATOR, t_TYPE>::pointer
AllocatorUtil::newObject(const t_ALLOCATOR& allocator,
                         t_ARG1&            argument1,
                         t_ARGS&&...        arguments)
{
    typedef AllocatorUtil_Traits<t_ALLOCATOR, t_TYPE> Traits;

    typename Traits::allocator_type reboundAlloc(allocator);
    typename Traits::pointer        p = reboundAlloc.allocate(1);
    // Use a 'try' block because the proctor components are at a higher
    // dependency level than this component.  As there is only one possibly
    // throwing statement, correctness of the 'try' block is easily verified.
    BSLS_TRY {
        Traits::construct(reboundAlloc,
                          BSLS_UTIL_ADDRESSOF(*p),
                          argument1,
                          BSLS_COMPILERFEATURES_FORWARD(t_ARGS, arguments)...);
    }
    BSLS_CATCH(...) {
        reboundAlloc.deallocate(p, 1);
        BSLS_RETHROW;
    }
    return p;
}
# endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

template <class t_TYPE, class t_ALLOCATOR, class t_ARG1, class... t_ARGS>
inline
typename AllocatorUtil_Traits<t_ALLOCATOR, t_TYPE>::pointer
AllocatorUtil::newObject(const t_ALLOCATOR&                        allocator,
                         BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG1) argument1,
                         t_ARGS&&...                               arguments)
{
    typedef AllocatorUtil_Traits<t_ALLOCATOR, t_TYPE> Traits;

    typename Traits::allocator_type reboundAlloc(allocator);
    typename Traits::pointer        p = reboundAlloc.allocate(1);
    // Use a 'try' block because the proctor components are at a higher
    // dependency level than this component.  As there is only one possibly
    // throwing statement, correctness of the 'try' block is easily verified.
    BSLS_TRY {
        Traits::construct(reboundAlloc,
                          BSLS_UTIL_ADDRESSOF(*p),
                          BSLS_COMPILERFEATURES_FORWARD(t_ARG1, argument1),
                          BSLS_COMPILERFEATURES_FORWARD(t_ARGS, arguments)...);
    }
    BSLS_CATCH(...) {
        reboundAlloc.deallocate(p, 1);
        BSLS_RETHROW;
    }
    return p;
}
#endif

template <class t_TYPE>
inline
void AllocatorUtil::swap(t_TYPE *pa, t_TYPE *pb, bsl::false_type)
{
    BSLS_ASSERT(*pa == *pb);
    (void)pa; (void)pb;
}

template <class t_TYPE>
inline
void AllocatorUtil::swap(t_TYPE *pa, t_TYPE *pb, bsl::true_type)
{
    using std::swap;
    swap(*pa, *pb);
}

}  // close package namespace
}  // close enterprise namespace

#endif // End C++11 code

#endif // ! defined(INCLUDED_BSLMA_ALLOCATORUTIL)

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
