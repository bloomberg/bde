// bslma_polymorphicallocator.h                                       -*-C++-*-
#ifndef INCLUDED_BSLMA_POLYMORPHICALLOCATOR
#define INCLUDED_BSLMA_POLYMORPHICALLOCATOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an allocator interface for `bsl::memory_resource` objects.
//
//@CLASSES:
//  bsl::polymorphic_allocator: STL-compatible polymorphic allocator template
//
//@CANONICAL_HEADER: bsl_memory_resource.h
//
//@SEE_ALSO: bslma_memoryresource, bslma_bslallocator
//
//@DESCRIPTION: This component provides an STL-compatible proxy for any
// resource class derived from `bsl::memory_resource`.  The
// `bsl::polymorphic_allocator` interface is identical to that of
// `std::pmr::polymorphic_allocator` from the C++17 Standard Library; in fact,
// the former type is an alias for the latter type when using a C++17 or later
// library supplied by the platform.
//
// The proxy class, `bsl::polymorphic_allocator` is a template that adheres to
// the allocator requirements defined in section [allocator.requirements] of
// the C++ standard.  `bsl::polymorphic_allocator` may be used to instantiate
// any class template that is parameterized by a standard allocator.  The
// container is expected to allocate memory for its own use through the
// allocator.  A `bsl::polymorphic_allocator` object is initialized using a
// pointer to a resource object derived from `bsl::memory_resource`.  Different
// types of memory resources use different allocation mechanisms, so this
// approach gives the programmer run time control over how the container
// obtains memory.
//
// A container constructs its elements by calling the `construct` method on its
// allocator.  Importantly, `bsl::polymorphic_allocator` is a *scoped*
// *allocator* -- when its `construct` method is called, the allocator passes
// itself to the constructor of the object being constructed (if that object is
// allocator aware (AA) and uses a compatible the allocator type).  Thus, a
// container instantiated with a scoped allocator ensures that its elements use
// the same allocator as the container itself.
//
// A container using `bsl::polymorphic_allocator` should not copy its allocator
// on assignment and thus, to avoid errors, `bsl::polymorphic_allocator`, is
// not assignable.  By design, a member of type `bsl::polymorphic_allocator`
// will prevent the client class from having a compiler-generated (defaulted)
// assignment operator because such an assignment operator would almost
// certainly do the wrong thing -- copying the allocator and allocated objects
// instead of cloning those objects using the destination allocator.  Once
// constructed, there is no straightforward way to rebind a
// `bsl::polymorphic_allocator` to use a different resource.
//
// Instantiations of `bsl::polymorphic_allocator` have reference semantics.  A
// `bsl::polymorphic_allocator` object does not "own" the `bslma::Allocator`
// with which it is initialized; copying a `bsl::polymorphic_allocator` object
// does not copy its resource object and destroying a
// `bsl::polymorphic_allocator` does not destroy its resource object.  Two
// `bsl::polymorphic_allocator` objects compare equal if and only if the
// resource objects they refer to compare equal.
//
///Thread Safety
///-------------
// Because it is immutable, non-assignable, and has reference semantics, a
// single `bsl::polymorphic_allocator` object is safe for concurrent access by
// multiple threads if and only if the `bsl::memory_resource` it references is
// safe for concurrent access from multiple threads.  Separate objects of
// `bsl::polymorphic_allocator` type may safely be used in separate threads if
// and only if the `bsl::memory_resource` objects they reference are,
// themselves, safe for concurrent access.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: A class that allocates memory
///- - - - - - - - - - - - - - - - - - - -
// In this example, we define a class template, `Holder<TYPE>`, that holds a
// single instance of `TYPE` on the heap.  `Holder` is designed such that its
// memory use can be customized by supplying an appropriate allocator.  A
// holder object can be empty and it can be move-constructed even if
// `TYPE` is not movable.  In addition, the footprint of a `Holder` object is
// the same (typically the size of 2 pointers), regardless of the size of
// `TYPE`.
//
// First, we create a `CountingResource` class, derived from
// `bsl::memory_resource`, that keeps track of the number of blocks of memory
// that were allocated from the resource but not yet returned to the resource;
// see usage example 1 in `bslma_memoryresource`.
// ```
// #include <bslmf_movableref.h>
// #include <bsls_assert.h>
// #include <bsls_keyword.h>
// #include <stdint.h>  // 'uintptr_t'
//
// class CountingResource : public bsl::memory_resource {
//
//     // DATA
//     int d_blocksOutstanding;
//
//     CountingResource(const CountingResource&) BSLS_KEYWORD_DELETED;
//     CountingResource& operator=(const CountingResource&)
//                                                       BSLS_KEYWORD_DELETED;
//
//   private:
//     // PRIVATE MANIPULATORS
//     void* do_allocate(std::size_t bytes,
//                       std::size_t alignment) BSLS_KEYWORD_OVERRIDE;
//     void do_deallocate(void* p, std::size_t bytes,
//                        std::size_t alignment) BSLS_KEYWORD_OVERRIDE;
//
//     // PRIVATE ACCESSORS
//     bool do_is_equal(const bsl::memory_resource& other) const
//                                BSLS_KEYWORD_NOEXCEPT BSLS_KEYWORD_OVERRIDE;
//
//   public:
//     // CREATORS
//     CountingResource() : d_blocksOutstanding(0) { }
//     ~CountingResource() BSLS_KEYWORD_OVERRIDE;
//
//     // ACCESSORS
//     int blocksOutstanding() const { return d_blocksOutstanding; }
// };
//
// CountingResource::~CountingResource()
// {
//     BSLS_assert(0 == d_blocksOutstanding);
// }
//
// void *CountingResource::do_allocate(std::size_t bytes,
//                                     std::size_t alignment)
// {
//     void *ret = ::operator new(bytes);
//     if (uintptr_t(ret) & (alignment - 1)) {
//         ::operator delete(ret);
//         BSLS_THROW(this);  // Alignment failed
//     }
//     ++d_blocksOutstanding;
//     return ret;
// }
//
// void CountingResource::do_deallocate(void* p, std::size_t, std::size_t)
// {
//     ::operator delete(p);
//     --d_blocksOutstanding;
// }
//
// bool CountingResource::do_is_equal(const bsl::memory_resource& other) const
//                                                       BSLS_KEYWORD_NOEXCEPT
// {
//     return this == &other;
// }
// ```
// Now we define our actual `Holder` template with with data members to hold
// the memory allocator and a pointer to the contained object:
// ```
// template <class TYPE>
// class Holder {
//     bsl::polymorphic_allocator<TYPE>  d_allocator;
//     TYPE                             *d_data_p;
// ```
// Next, we declare the constructors.  Following the pattern for
// allocator-aware types used in BDE, the public interface contains an
// `allocator_type` typedef that can be passed to each constructor.:
// ```
//   public:
//     // TYPES
//     typedef bsl::polymorphic_allocator<TYPE> allocator_type;
//
//     // CREATORS
//     explicit Holder(const allocator_type& allocator = allocator_type());
//     explicit Holder(const TYPE&           value,
//                     const allocator_type& allocator = allocator_type());
//     Holder(const Holder&         other,
//            const allocator_type& allocator = allocator_type());
//     Holder(bslmf::MovableRef<Holder> other);                    // IMPLICIT
//     Holder(bslmf::MovableRef<Holder> other,
//            const allocator_type&     allocator);
//     ~Holder();
// ```
// Next, we declare the manipulators and accessors, allowing a `Holder` to be
// assigned and giving a client access to its value and allocator:
// ```
//     // MANIPULATORS
//     Holder& operator=(const Holder& rhs);
//     Holder& operator=(bslmf::MovableRef<Holder> rhs);
//     TYPE& value() { return *d_data_p; }
//
//     // ACCESSORS
//     bool isEmpty() const { return 0 == d_data_p; }
//     const TYPE& value() const { return *d_data_p; }
//     allocator_type get_allocator() const { return d_allocator; }
// };
// ```
// Next, we'll implement the first constructor, which creates an empty object;
// its only job is to store the allocator:
// ```
// template <class TYPE>
// Holder<TYPE>::Holder(const allocator_type& allocator)
//     : d_allocator(allocator)
//     , d_data_p(0)
// {
// }
// ```
// Next, we'll implement the second constructor, which allocates memory and
// constructs an object in it.  The `try`/`catch` block is needed to free the
// memory in case the constructor for `TYPE` throws and exception.  An
// alternative implementation would use an RAII object to automatically free
// the memory in the case of an exception (see `bslma_deallocatorproctor`):
// ```
// template <class TYPE>
// Holder<TYPE>::Holder(const TYPE& value, const allocator_type& allocator)
//     : d_allocator(allocator)
//     , d_data_p(0)
// {
//     d_data_p = d_allocator.allocate(1);
//     BSLS_TRY {
//         ::new(d_data_p) TYPE(value);
//     }
//     BSLS_CATCH(...) {
//         d_allocator.deallocate(d_data_p, 1);
//         BSLS_RETHROW;
//     }
// }
// ```
// Next, we'll implement a destructor that deletes the value object and
// deallocates the allocated memory:
// ```
// template <class TYPE>
// Holder<TYPE>::~Holder()
// {
//     if (! isEmpty()) {
//         d_data_p->~TYPE();                    // Destroy object.
//         d_allocator.deallocate(d_data_p, 1);  // Deallocate memory.
//     }
// }
// ```
// Finally, we've implemented enough of `Holder` to demonstrate its use.
// Below, we pass the `CountingResource` from Example 1 to the constructors
// several `Holder` objects.  Each non-empty `Holder` allocates one block of
// memory, which is reflected in the outstanding block count.  Note that the
// address of the resource can be passed directly to the constructors because
// `bsl::polymorphic_allocator` is implicitly convertible from
// `bsl::memory_resource *`:
// ```
// int main()
// {
//     CountingResource rsrc;
//
//     {
//         Holder<int> h1(&rsrc);   // Empty resource
//         assert(h1.isEmpty());
//         assert(0 == rsrc.blocksOutstanding());
//
//         Holder<int> h2(2, &rsrc);
//         assert(! h2.isEmpty());
//         assert(1 == rsrc.blocksOutstanding());
//
//         Holder<double> h3(3.0, &rsrc);
//         assert(! h3.isEmpty());
//         assert(2 == rsrc.blocksOutstanding());
//     }
//
//     assert(0 == rsrc.blocksOutstanding());  // Destructors freed memory
// }
// ```

#include <bslscm_version.h>

#include <bslma_allocatortraits.h>
#include <bslma_constructionutil.h>
#include <bslma_default.h>
#include <bslma_destructionutil.h>
#include <bslma_isstdallocator.h>
#include <bslma_memoryresource.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_isbitwiseequalitycomparable.h>
#include <bslmf_typeidentity.h>

#include <bsls_annotation.h>
#include <bsls_assert.h>
#include <bsls_bslexceptionutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>
#include <bsls_util.h>     // 'forward<T>(V)', `Util::addressOf`

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// clang-format off
// Include version that can be compiled with C++03
// Generated on Tue Jan 14 14:15:16 2025
// Command line: sim_cpp11_features.pl bslma_polymorphicallocator.h

# define COMPILING_BSLMA_POLYMORPHICALLOCATOR_H
# include <bslma_polymorphicallocator_cpp03.h>
# undef COMPILING_BSLMA_POLYMORPHICALLOCATOR_H

// clang-format on
#else

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
// Use `polymorphic_allocator` from native C++17 library, if available.

# include <memory_resource>

namespace bsl {

// Import 'polymorphic_allocator' into the 'bsl' namespace.
template <class TYPE = std::byte>
using polymorphic_allocator = std::pmr::polymorphic_allocator<TYPE>;

}  // close namespace bsl

// FREE FUNCTIONS

// As per issue LWG 3683 (https://cplusplus.github.io/LWG/issue3683), the C++20
// standard as published does not provide `operator==` for comparing a
// `polymorphic_allocator` to a type convertible to `polymorphic_allocator`,
// e.g., a `pmr::memory_resource`.  The workaround below corresponds to
// proposed resolution 2, which differs from the accepted issue resolution
// (proposed resolution 3) because the accepted resolution requires modifying
// the library class itself.  This workaround does not depend on C++20
// automatic generation of multiple `operator==` and `operator!=` from a single
// declaration of `operator==`.  Because `bsl::polymorphic_allocator` is an
// alias for `std::pmr::polymorphic_allocator`, these operators must be defined
// in namespace `std::pmr` to be found by ADL.

namespace BloombergLP::bslma {

#if defined(BSLS_PLATFORM_CMP_MSVC)
// The MSVC compiler, up to and including version 19.33 (aka VS 2022), has a
// bug whereby supressing argument deduction using `type_identity` causes a
// linker error.  The `BSLMF_POLYMORPHICALLOCATOR_NODEDUCE_T` macro below
// provides a non-deduced `polymorphic_allocator` argument that works around
// this problem.
//
// Note that this workaround will not work for a type convertible to
// `polymorphic_allocator` via a conversion operator:
//..
//  class C { operator bsl::polymorphic_allocator<short>() const; };
//  assert(bsl::polymorphic_allocator<short>() == C());  // won`t compile
//..
// As this is a rare-to-nonexistant use case, we won`t bother to fix that for
// now.

struct PolymorphicAllocator_Unique
{
    // A unique type not used for anything else
};

#define BSLMF_POLYMORPHICALLOCATOR_NODEDUCE_T(VAL_T)                          \
    std::pmr::polymorphic_allocator<                                          \
                              BloombergLP::bslma::PolymorphicAllocator_Unique>

#else  // Not MSVC compiler

#define BSLMF_POLYMORPHICALLOCATOR_NODEDUCE_T(VAL_T)                          \
    typename bsl::type_identity<std::pmr::polymorphic_allocator<VAL_T>>::type

#endif

}  // close namespace BloombergLP::bslma

namespace std::pmr {

/// Return `true` if the specified `a` and specified `b` polymorphic
/// allocators have equal memory resources; otherwise `false`.
template <class TYPE>
bool operator==(const polymorphic_allocator<TYPE>&                 a,
                const BSLMF_POLYMORPHICALLOCATOR_NODEDUCE_T(TYPE)& b)
                                                         BSLS_KEYWORD_NOEXCEPT;

/// Return `true` if the specified `a` and specified `b` polymorphic
/// allocators have equal memory resources; otherwise `false`.
template <class TYPE>
bool operator==(const BSLMF_POLYMORPHICALLOCATOR_NODEDUCE_T(TYPE)& a,
                const polymorphic_allocator<TYPE>&                 b)
                                                         BSLS_KEYWORD_NOEXCEPT;

/// Return `false` if the specified `a` and specified `b` polymorphic
/// allocators have equal memory resources; otherwise `true`.
template <class TYPE>
bool operator!=(const polymorphic_allocator<TYPE>&                 a,
                const BSLMF_POLYMORPHICALLOCATOR_NODEDUCE_T(TYPE)& b)
                                                         BSLS_KEYWORD_NOEXCEPT;

/// Return `false` if the specified `a` and specified `b` polymorphic
/// allocators have equal memory resources; otherwise `true`.
template <class TYPE>
bool operator!=(const BSLMF_POLYMORPHICALLOCATOR_NODEDUCE_T(TYPE)& a,
                const polymorphic_allocator<TYPE>&                 b)
                                                         BSLS_KEYWORD_NOEXCEPT;

}  // close namespace std::pmr

#else  // If C++17 library is not available

                // =========================================
                // class template bsl::polymorphic_allocator
                // =========================================

namespace bsl {

/// An STL-compatible proxy for any resource class derived from
/// `bsl::memory_resource`.  This class template is a pre-C++17
/// implementation of `std::pmr::polymorphic_allocator` from the C++17
/// Standard Library.  Note that there are a number of methods (e.g.,
/// `max_size`) that are not in the C++17 version of this class.  These
/// members exist for compatibility with C++03 versions of the standard
/// library, which don't use `allocator_traits`.
template <class TYPE = unsigned char>
class polymorphic_allocator {

    // DATA
    memory_resource *d_resource;

    // NOT IMPLEMENTED
    polymorphic_allocator&
    operator=(const polymorphic_allocator&) BSLS_KEYWORD_DELETED;

  public:
    // TYPES
    typedef std::size_t     size_type;
    typedef std::ptrdiff_t  difference_type;
    typedef TYPE           *pointer;
    typedef const TYPE     *const_pointer;
    typedef TYPE&           reference;
    typedef const TYPE&     const_reference;
    typedef TYPE            value_type;

    /// This nested `struct` template, parameterized by `ANY_TYPE`, provides
    /// a namespace for an `other` type alias, which is an allocator type
    /// following the same template as this one but that allocates elements
    /// of `ANY_TYPE`.  Note that this allocator type is convertible to and
    /// from `other` for any type, including `void`.
    template <class ANY_TYPE>
    struct rebind {
        typedef polymorphic_allocator<ANY_TYPE> other;
    };

    // CREATORS

    /// Create an allocator that will forward allocation calls to the
    /// object pointed to by `bslma::Default::defaultAllocator()`.
    /// Postcondition:
    /// ```
    ///  this->resource() == bslma::Default::defaultAllocator()
    /// ```
    /// Note that in this C++03 implementation, the default memory resource
    /// is the same as `bslma::Default::defaultAllocator()`.
    polymorphic_allocator() BSLS_KEYWORD_NOEXCEPT;

    /// Convert a `memory_resource` pointer to a `polymorphic_allocator`
    /// object that forwards allocation calls to the object pointed to by
    /// the specified `r`.  Postcondition:
    /// ```
    ///  this->resource() == r
    /// ```
    /// The behavior is undefined if `r` is null.
    polymorphic_allocator(memory_resource *r);                      // IMPLICIT

    /// Create an allocator sharing the same resource object as the
    /// specified `original`.  The newly constructed allocator will compare
    /// equal to `original`, even though they may be instantiated on
    /// different types.  Postconditions:
    /// ```
    ///  *this == original
    ///  this->resource() == original.resource();
    /// ```
#ifdef BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
    polymorphic_allocator(const polymorphic_allocator& original)
                                               BSLS_KEYWORD_NOEXCEPT = default;
#else
    polymorphic_allocator(const polymorphic_allocator& original)
                                                         BSLS_KEYWORD_NOEXCEPT;
#endif
    template<class ANY_TYPE>
    polymorphic_allocator(const polymorphic_allocator<ANY_TYPE>& original)
                                                         BSLS_KEYWORD_NOEXCEPT;

    /// Destroy this object.  Note that this does not delete the object
    /// pointed to by 'resource()'.
    //! ~polymorphic_allocator() = default;

    // MANIPULATORS

    /// Return a block of memory having sufficient size and alignment to
    /// hold the specified `n` objects of `value_type`, allocated from the
    /// memory resource held by this allocator.
    BSLS_ANNOTATION_NODISCARD TYPE *allocate(std::size_t n);

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    /// Return a block of memory having sufficient size and alignment to
    /// hold the specified `n` objects of `value_type`, allocated from the
    /// memory resource held by this allocator, ignoring the specified
    /// `hint`, which is used by other allocators as a locality hint.  Note
    /// that this overload is not part of C++17
    /// `std::pmr::polymorphic_allocator` but it is a requirement for all
    /// C++03 allocators.
    BSLS_ANNOTATION_NODISCARD TYPE *allocate(std::size_t n, const void *hint);
#endif

    /// Deallocate a block of memory having sufficient size and alignment to
    /// hold the specified `n` objects of `value_type` by returning it to
    /// the memory resource held by this allocator.  The behavior is
    /// undefined unless `p` is the address of a block previously allocated
    /// by a call to `allocate` with the same `n` and not yet deallocated.
    void deallocate(TYPE *p, std::size_t n);

    /// Create a default-constructed object of (template parameter)
    /// `ELEMENT_TYPE` at the specified `address`.  If `ELEMENT_TYPE`
    /// supports `bslma`-style allocation, this allocator passes itself to
    /// the extended default constructor.  If the constructor throws, the
    /// memory at `address` is left in an unspecified state.  The behavior
    /// is undefined unless `address` refers to a block of memory having
    /// sufficient size and alignment for an object of `ELEMENT_TYPE`.
    template <class ELEMENT_TYPE>
    void construct(ELEMENT_TYPE *address);

    /// Create an object of (template parameter) `ELEMENT_TYPE` at the
    /// specified `address`, constructed by forwarding the specified
    /// `argument1` and the (variable number of) additional specified
    /// `arguments` to the corresponding constructor of `ELEMENT_TYPE`.  If
    /// `ELEMENT_TYPE` supports `bslma`-style allocation, this allocator
    /// passes itself to the constructor.  If the constructor throws, the
    /// memory at `address` is left in an unspecified state.  Note that, in
    /// C++03, perfect forwarding is limited such that any lvalue reference
    /// in the `arguments` parameter pack is const-qualified when forwarded
    /// to the `ELEMENT_TYPE` constructor; only `argument1` can be forwarded
    /// as an unqualified lvalue.  The behavior is undefined unless
    /// `address` refers to a block of memory having sufficient size and
    /// alignment for an object of `ELEMENT_TYPE`.
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=14
    template <class ELEMENT_TYPE, class ARG1, class... ARGS>
    void construct(ELEMENT_TYPE            *address,
                   ARG1&                    argument1,
                   ARGS&&...                arguments);
    template <class ELEMENT_TYPE, class ARG1, class... ARGS>
    void construct(ELEMENT_TYPE                            *address,
                   BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  argument1,
                   ARGS&&...                                arguments);
#endif

    /// Call the `ELEMENT_TYPE` destructor for the object at the specified
    /// `address` but do not deallocate the memory at `address`.
    template <class ELEMENT_TYPE>
    void destroy(ELEMENT_TYPE *address);

    // ACCESSORS

    /// Return the address of the object referred to by the specified `x`
    /// reference, even if the (template parameter) `TYPE` overloads the
    /// unary `operator&`.
    pointer       address(reference x) const;
    const_pointer address(const_reference x) const;

    /// Return the maximum number of elements of (template parameter) `TYPE`
    /// that can be allocated using this allocator.  Note that there is no
    /// guarantee that attempts at allocating fewer elements than the value
    /// returned by `max_size` will not throw.
    BSLS_KEYWORD_CONSTEXPR size_type max_size() const;

    /// Return the address of the memory resource supplied on construction.
    memory_resource *resource() const;

    /// Return a default-constructed `polymorphic_allocator`.
    polymorphic_allocator select_on_container_copy_construction() const;

    // HIDDEN FRIENDS

    /// Return `true` if memory allocated from either of the specified `a`
    /// or `b` allocators can be deallocated from the other; otherwise
    /// return `false`.  This operator is selected by overload resolution if
    /// at least one argument is a specialization of `polymorphic_allocator`
    /// and the other is either the same specialization or is convertible to
    /// `polymorphic_allocator`.  Note that this operator is a "hidden
    /// friend" so that it is found by only by ADL and is not considered
    /// during overload resoution if neither argument is a specialization of
    /// `polymorphic_allocator`; see
    /// https://cplusplus.github.io/LWG/issue3683.
    friend
    bool operator==(const polymorphic_allocator& a,
                    const polymorphic_allocator& b) BSLS_KEYWORD_NOEXCEPT
    {
        return a.resource() == b.resource() || *a.resource() == *b.resource();
    }

    /// Return `true` if memory allocated from either of the specified `a`
    /// or `b` allocators cannot necessarily be deallocated from the other;
    /// otherwise return `false`.  This operator is selected by overload
    /// resolution if at least one argument is a specialization of
    /// `polymorphic_allocator` and the other is either the same
    /// specialization or is convertible to `polymorphic_allocator`.  Note
    /// that this operator is a "hidden friend" so that it is found by only
    /// by ADL and is not considered during overload resoution if neither
    /// argument is a specialization of `polymorphic_allocator`; see
    /// https://cplusplus.github.io/LWG/issue3683.
    friend
    bool operator!=(const polymorphic_allocator& a,
                    const polymorphic_allocator& b)  BSLS_KEYWORD_NOEXCEPT
    {
        return a.resource() != b.resource() && *a.resource() != *b.resource();
    }
};

// FREE FUNCTIONS

/// Return `true` if memory allocated from either of the specified `a` or
/// `b` allocators can be deallocated from the other; otherwise return
/// `false`.  Note that, when `T1` and `T2` are different, this free
/// operator is a better match than the hidden friend operator, which would
/// otherwise be ambiguous.
template <class T1, class T2>
bool operator==(const polymorphic_allocator<T1>& a,
                const polymorphic_allocator<T2>& b) BSLS_KEYWORD_NOEXCEPT;

/// Return `true` if memory allocated from either of the specified `a` or
/// `b` allocators cannot necessarily be deallocated from the other;
/// otherwise return `false`.  Note that, when `T1` and `T2` are different,
/// this free operator is a better match than the hidden friend operator,
/// which would otherwise be ambiguous.
template <class T1, class T2>
bool operator!=(const polymorphic_allocator<T1>& a,
                const polymorphic_allocator<T2>& b) BSLS_KEYWORD_NOEXCEPT;

}  // close namespace bsl

#endif // End C++03 code

namespace bsl {

              // ====================================================
              // class allocator_traits<polymorphic_allocator<TYPE> >
              // ====================================================

/// This `struct` template provides a specialization of the
/// `allocator_traits` class template for `bsl::polymorphic_allocator`.
/// This specialization is not strictly necessary, but its presence speeds
/// up compilation by bypassing a significant amount of metaprogramming.
template <class TYPE>
struct allocator_traits<polymorphic_allocator<TYPE> > {

    // PUBLIC TYPES
    typedef polymorphic_allocator<TYPE> allocator_type;
    typedef TYPE                        value_type;

    typedef TYPE                       *pointer;
    typedef const TYPE                 *const_pointer;
    typedef void                       *void_pointer;
    typedef const void                 *const_void_pointer;
    typedef std::ptrdiff_t              difference_type;
    typedef std::size_t                 size_type;

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
    template <class TYPE2>
    using rebind_alloc = polymorphic_allocator<TYPE2>;

    template <class TYPE2>
    using rebind_traits = allocator_traits<polymorphic_allocator<TYPE2> >;
#else
    template <class TYPE2>
    struct rebind_alloc : polymorphic_allocator<TYPE2> {
        rebind_alloc()
        : polymorphic_allocator<TYPE2>()
        {
        }

        /// Convert from anything that can be used to cosntruct the base type.
        /// This might be better if SFINAE-ed out using `is_convertible`, but
        /// stressing older compilers more seems unwise.
        template <typename ARG>
        rebind_alloc(const ARG& allocatorArg)
        : polymorphic_allocator<TYPE2>(allocatorArg)
        {
        }
    };

    template <class TYPE2>
    struct rebind_traits : allocator_traits<polymorphic_allocator<TYPE2> > {
    };
#endif

    static pointer allocate(allocator_type& m, size_type n)
    {
        return m.allocate(n);
    }

    static pointer allocate(allocator_type&    m,
                            size_type          n,
                            const_void_pointer /* hint */)
    {
        return m.allocate(n);
    }

    static void deallocate(allocator_type& m, pointer p, size_type n)
    {
        m.deallocate(p, n);
    }

    template <class TYPE2>
    static void construct(allocator_type&  m,
                          TYPE2           *p)
    {
        m.construct(p);
    }

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=14
    template <class TYPE2, class ARG1, class... ARGS>
    static void construct(allocator_type&  m,
                          TYPE2           *p,
                          ARG1&            argument1,
                          ARGS&&...        arguments)
    {
        m.construct(p,
                    argument1,
                    BSLS_COMPILERFEATURES_FORWARD(ARGS, arguments)...);
    }

    template <class TYPE2, class ARG1, class... ARGS>
    static void construct(allocator_type&                          m,
                          TYPE2                                   *p,
                          BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  argument1,
                          ARGS&&...                                arguments)
    {
        m.construct(p,
                    BSLS_COMPILERFEATURES_FORWARD(ARG1, argument1),
                    BSLS_COMPILERFEATURES_FORWARD(ARGS, arguments)...);
    }
#endif

    template <class TYPE2>
    static void destroy(allocator_type&, TYPE2 *p)
    {
        p->~TYPE2();
    }

    BSLS_KEYWORD_CONSTEXPR
    static size_type max_size(const allocator_type&)
    {
        // Return the largest value, 'v', such that 'v * sizeof(T)' fits in a
        // 'size_type'.  Note that we cannot call
        // 'allocator_type::max_size()' because that method does not exist in
        // the C++17 standard library.

        return (~size_type(0)) / sizeof(TYPE);
    }

    // Allocator propagation traits
    static allocator_type
    select_on_container_copy_construction(const allocator_type&)
    {
        return allocator_type();
    }

    typedef false_type is_always_equal;
    typedef false_type propagate_on_container_copy_assignment;
    typedef false_type propagate_on_container_move_assignment;
    typedef false_type propagate_on_container_swap;
};

}  // close namespace bsl

// ============================================================================
//                  TEMPLATES AND INLINE FUNCTION DEFINITIONS
// ============================================================================

// TRAITS
namespace bsl {

/// Note that the `bsl::is_trivially_copyable` trait automatically sets the
/// `bslmf::IsBitwiseMoveable` trait.
template <class TYPE>
struct is_trivially_copyable<polymorphic_allocator<TYPE> > : true_type { };
}  // close namespace bsl

namespace BloombergLP {
namespace bslma {

/// Declare `polymorphic_allocator` as a C++11 compatible allocator for
/// all versions of C++.
template <class TYPE>
struct IsStdAllocator< ::bsl::polymorphic_allocator<TYPE> >
    : bsl::true_type
{
};

/// An allocator is not *itself* an allocator-aware type, even though it is
/// convertible from `bsl::Allocator *`.
template <class TYPE>
struct UsesBslmaAllocator< ::bsl::polymorphic_allocator<TYPE> >
    : bsl::false_type {
};

}  // close namespace bslma

namespace bslmf {

template <class TYPE>
struct IsBitwiseEqualityComparable< ::bsl::polymorphic_allocator<TYPE> >
    : bsl::true_type { };

}  // close namespace bslmf
}  // close enterprise namespace

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR

                // ----------------------------------------------
                // class template std::pmr::polymorphic_allocator
                // ----------------------------------------------

// FREE FUNCTIONS

// Put extra operators in the 'std::pmr' namespace, not 'bsl' namespace.
template <class TYPE>
inline
bool std::pmr::operator==(const std::pmr::polymorphic_allocator<TYPE>&       a,
                          const BSLMF_POLYMORPHICALLOCATOR_NODEDUCE_T(TYPE)& b)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return a.resource() == b.resource() || *a.resource() == *b.resource();
}

template <class TYPE>
inline
bool std::pmr::operator==(const BSLMF_POLYMORPHICALLOCATOR_NODEDUCE_T(TYPE)& a,
                          const std::pmr::polymorphic_allocator<TYPE>&       b)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return a.resource() == b.resource() || *a.resource() == *b.resource();
}

template <class TYPE>
inline
bool std::pmr::operator!=(const std::pmr::polymorphic_allocator<TYPE>&       a,
                          const BSLMF_POLYMORPHICALLOCATOR_NODEDUCE_T(TYPE)& b)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return a.resource() != b.resource() && *a.resource() != *b.resource();
}

template <class TYPE>
inline
bool std::pmr::operator!=(const BSLMF_POLYMORPHICALLOCATOR_NODEDUCE_T(TYPE)& a,
                          const std::pmr::polymorphic_allocator<TYPE>&       b)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return a.resource() != b.resource() && *a.resource() != *b.resource();
}

#else // if ! defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)

namespace bsl {

                // -----------------------------------------
                // class template bsl::polymorphic_allocator
                // -----------------------------------------

// CREATORS
template <class TYPE>
polymorphic_allocator<TYPE>::polymorphic_allocator() BSLS_KEYWORD_NOEXCEPT
: d_resource(BloombergLP::bslma::Default::defaultAllocator())
{
}

template <class TYPE>
inline
polymorphic_allocator<TYPE>::polymorphic_allocator(memory_resource *r)
: d_resource(r)
{
    BSLS_ASSERT(r != 0);
}

#ifndef BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
// In C++11 or later, this copy constructor is defaulted.
template <class TYPE>
inline
polymorphic_allocator<TYPE>::
polymorphic_allocator(const polymorphic_allocator& other) BSLS_KEYWORD_NOEXCEPT
: d_resource(other.resource())
{
}
#endif

template <class TYPE>
template<class ANY_TYPE>
inline
polymorphic_allocator<TYPE>::
polymorphic_allocator(const polymorphic_allocator<ANY_TYPE>& other)
                                                          BSLS_KEYWORD_NOEXCEPT
: d_resource(other.resource())
{
}

// MANIPULATORS
template <class TYPE>
inline
TYPE *polymorphic_allocator<TYPE>::allocate(std::size_t n)
{
    const size_t k_TYPE_ALIGNMENT =
        BloombergLP::bsls::AlignmentFromType<TYPE>::VALUE;

    if (n > this->max_size()) {
        BloombergLP::bsls::BslExceptionUtil::throwBadAlloc();
    }

    return static_cast<TYPE *>(d_resource->allocate(n * sizeof(TYPE),
                                                    k_TYPE_ALIGNMENT));
}

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
template <class TYPE>
inline
TYPE *
polymorphic_allocator<TYPE>::allocate(std::size_t n, const void * /* hint */)
{
    return this->allocate(n);
}
#endif

template <class TYPE>
inline
void polymorphic_allocator<TYPE>::deallocate(TYPE *p, std::size_t n)
{
    const std::size_t k_TYPE_ALIGNMENT =
        BloombergLP::bsls::AlignmentFromType<TYPE>::VALUE;

    d_resource->deallocate(p, n * sizeof(TYPE), k_TYPE_ALIGNMENT);
}

template <class TYPE>
template <class ELEMENT_TYPE>
inline
void polymorphic_allocator<TYPE>::construct(ELEMENT_TYPE *address)
{
    BloombergLP::bslma::ConstructionUtil::construct(address, *this);
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class TYPE>
template <class ELEMENT_TYPE, class ARG1, class... ARGS>
inline
void polymorphic_allocator<TYPE>::construct(ELEMENT_TYPE *address,
                                            ARG1&         argument1,
                                            ARGS&&...     arguments)
{
    BloombergLP::bslma::ConstructionUtil::construct(
        address,
        *this,
        argument1,
        BSLS_COMPILERFEATURES_FORWARD(ARGS, arguments)...);
}

template <class TYPE>
template <class ELEMENT_TYPE, class ARG1, class... ARGS>
inline
void polymorphic_allocator<TYPE>::construct(
                            ELEMENT_TYPE                            *address,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  argument1,
                            ARGS&&...                                arguments)
{
    BloombergLP::bslma::ConstructionUtil::construct(
        address,
        *this,
        BSLS_COMPILERFEATURES_FORWARD(ARG1, argument1),
        BSLS_COMPILERFEATURES_FORWARD(ARGS, arguments)...);
}
#endif

template <class TYPE>
template <class ELEMENT_TYPE>
inline
void polymorphic_allocator<TYPE>::destroy(ELEMENT_TYPE *address)
{
    BloombergLP::bslma::DestructionUtil::destroy(address);
}


// ACCESSORS
template <class TYPE>
inline
typename polymorphic_allocator<TYPE>::pointer
polymorphic_allocator<TYPE>::address(reference x) const
{
    return BloombergLP::bsls::Util::addressOf(x);
}

template <class TYPE>
inline
typename polymorphic_allocator<TYPE>::const_pointer
polymorphic_allocator<TYPE>::address(const_reference x) const
{
    return BloombergLP::bsls::Util::addressOf(x);
}

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR inline
typename polymorphic_allocator<TYPE>::size_type
polymorphic_allocator<TYPE>::max_size() const
{
    // Return the largest value, 'v', such that 'v * sizeof(T)' fits in a
    // 'size_type'.

    return (~size_type(0)) / sizeof(TYPE);
}

template <class TYPE>
inline
memory_resource *polymorphic_allocator<TYPE>::resource() const
{
    return d_resource;
}

template <class TYPE>
inline
polymorphic_allocator<TYPE>
polymorphic_allocator<TYPE>::select_on_container_copy_construction() const
{
    return polymorphic_allocator();
}

}  // Close namespace bsl

// FREE FUNCTIONS
template <class T1, class T2>
inline
bool
bsl::operator==(const bsl::polymorphic_allocator<T1>& a,
                const bsl::polymorphic_allocator<T2>& b) BSLS_KEYWORD_NOEXCEPT
{
    return a.resource() == b.resource() || *a.resource() == *b.resource();
}

template <class T1, class T2>
inline
bool
bsl::operator!=(const bsl::polymorphic_allocator<T1>& a,
                const bsl::polymorphic_allocator<T2>& b) BSLS_KEYWORD_NOEXCEPT
{
    return a.resource() != b.resource() && *a.resource() != *b.resource();
}

#endif // defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)

#endif // End C++11 code

#endif // ! defined(INCLUDED_BSLMA_POLYMORPHICALLOCATOR)

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
