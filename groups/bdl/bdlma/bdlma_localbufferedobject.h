// bdlma_localbufferedobject.h                                        -*-C++-*-
#ifndef INCLUDED_BDLMA_LOCALBUFFEREDOBJECT
#define INCLUDED_BDLMA_LOCALBUFFEREDOBJECT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide easy way to create an object with a local arena allocator.
//
//@CLASSES:
//  bdlma::LocalBufferedObject: object with a local-arena allocator
//
//@SEE_ALSO: bdlma_localsequentialalallocator
//
//@DESCRIPTION: This component provides a mechanism, `LocalBufferedObject`,
// that contains a single instance of an allocator-aware object instantiated
// using an arena allocator that will allocate from "local" memory.  This type
// is primarily used to simplify the creation of temporary objects utilizing a
// local memory buffer (typically on the stack) for efficiency, and is
// equivalent to creating a temporary object and supplying it a
// `bdlma::LocalSequentialAllocator`.  There are three template parameters --
// the type of object contained, the size in bytes of the buffer in the local
// arena allocator, and a `bool` to indicate whether destruction of the
// contained object is to be disabled during `emplace` and destruction of the
// mechansim.  If the buffer used by the local arena allocator is exhausted,
// subsequent allocations come from the allocator passed at construction, or
// the default allocator if no allocator was passed at construction.  Note that
// calls to `deallocate` by the held object are ignored.
//
// The container has 4 types of constructors:
// 1. A constructor that takes an arbitary set of arguments, propagated to the
//    object.
// 2. A constructor like `1` above, but also passed an allocator.
// 3. A constructor that takes a `std::initializer_list`, propagated to the
//    object.
// 4. A constructor like `3` above, but also passed an allocator.
//
///`t_DISABLE_DESTRUCTION` Template Parameter:
///-------------------------------------------
// If `t_DISABLE_DESTRUCTOR` is set to `true`, the `LocalBufferedObject`, upon
// destruction or `emplace`, will *not* destroy the contained object but will
// instead simply release any allocated memory.  Eliding the contained objects
// destructor may improve efficiency, but is safe only if the contained object
// does not manage resources other than memory.  I.e., it is unsafe to set
// `t_DISABLE_DESTRUCTOR` to `true` if the contained object manages resources
// other than memory (e.g., file handles, locks).  By default
// `t_DISABLE_DESTRUCTOR` is `false`.
//
// It is important that `t_DISABLE_DESTRUCTION` be set to `false` (the default)
// if any resources other than memory, such as file handles or mutexes, are
// managed by the held object.
//
///`emplace` Manipulators:
///-----------------------
// The `class` has two `emplace` manipulators,
// * One taking an arbitraty set of arguments to be propagated to the
//   contained objects constructor.
// * One taking a `std::initializer_list`.
//
///Usage
///-----
//
///Example 1: Configuring an Object to Allocate From Stack Memory
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have an array of `bsl::string_view`s containing names, with a
// large number of redundant entries, and we want to count how many unique
// names exist in the array.  We write a function `countUniqueNames` which
// stores the names in an unordered set, and yields the `size` accessor as the
// total count of unique names.
//
// The function will be called many times, and `bsl::unordered_set` does a
// large number of small memory allocations.  These allocations would be faster
// if they came from a non-freeing allocator that gets its memory from a buffer
// on the stack.
//
// We can use a `LocalBufferedObject` to create an `unordered_set` with an
// 8192-byte stack buffer from which it is to allocate memory.
// ```
// size_t countUniqueNames(const bsl::string_view *rawNames,
//                         size_t                  numRawNames)
// {
//     bdlma::LocalBufferedObject<bsl::unordered_set<bsl::string_view>,
//                                8192> uset;
//
//     for (unsigned uu = 0; uu < numRawNames; ++uu) {
//         uset->insert(rawNames[uu]);
//     }
//
//     return uset->size();
// }
// ```
// Notice that this syntactic convenience equivalent to supplying a local
// `LocalSequentialAllocator` to the `bsl::unordered_set`.
//
// Below we show the allocation behavior of this function as the number of
// items in the `unordered_set` increases.  Note that when the memory in the
// 8192-byte stack buffer is exhausted, further memory comes from the default
// allocator:
// ```
// 'countUniqueNames':
// Names: (raw:   25, unique:  23), used default allocator: 0
// Names: (raw:   50, unique:  42), used default allocator: 0
// Names: (raw:  100, unique:  70), used default allocator: 0
// Names: (raw:  200, unique: 103), used default allocator: 0
// Names: (raw:  400, unique: 130), used default allocator: 1
// Names: (raw:  800, unique: 143), used default allocator: 1
// Names: (raw: 1600, unique: 144), used default allocator: 1
// ```
//
///Example 2: Eliding the Destructor
///- - - - - - - - - - - - - - - - -
// Because the only resource managed by the `unordered_set` is memory, we can
// improve the performance of the previous example using the template's boolean
// `t_DISABLE_DESTRUCTOR` parameter.
//
// `unordered_set` allocates a lot of small nodes, and when the container is
// destroyed, unordered set's destructor traverses the whole data structure,
// visting every node and calling `bslma::Allocator::deallocate` on each one,
// which is a non-inline virtual function call eventually handled by the
// sequential allocator's `deallocate` function, which does nothing.
//
// If we set the 3rd template parameter of `LocalBufferedObject`, which is
// `t_DISABLE_DESTRUCTION` of type `bool`, to the non-default value of `true`,
// the `LocalBufferedObject` will not call the destructor of the held
// `unordered_set`.  This isn't a problem because unordered set manages no
// resource other than memory, and all the memory it uses is managed by the
// local sequential allocator contained in the local buffered object.
// ```
// size_t countUniqueNamesFaster(const bsl::string_view *rawNames,
//                               size_t                  numRawNames)
// {
//     bdlma::LocalBufferedObject<bsl::unordered_set<bsl::string_view>,
//                                8192,
//                                true> uset;
//
//     for (unsigned uu = 0; uu < numRawNames; ++uu) {
//         uset->insert(rawNames[uu]);
//     }
//
//     return uset->size();
// }
// ```
// And we see the calculations are exactly the same:
// ```
// 'countUniqueNamesFaster': destructor disabled:
// Names: (raw:   25, unique:  23), used default allocator: 0
// Names: (raw:   50, unique:  42), used default allocator: 0
// Names: (raw:  100, unique:  70), used default allocator: 0
// Names: (raw:  200, unique: 103), used default allocator: 0
// Names: (raw:  400, unique: 130), used default allocator: 1
// Names: (raw:  800, unique: 143), used default allocator: 1
// Names: (raw: 1600, unique: 144), used default allocator: 1
// ```

#include <bdlscm_version.h>

#include <bdlma_localsequentialallocator.h>

#include <bslma_allocatorutil.h>
#include <bslma_constructionutil.h>
#include <bslma_stdallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_assert.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_objectbuffer.h>

#include <bsl_type_traits.h>

#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
# include <initializer_list>
#endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS

#include <bsl_cstddef.h>    // size_t
#include <bsl_utility.h>

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// clang-format off
// Include version that can be compiled with C++03
// Generated on Mon Jan 13 08:32:13 2025
// Command line: sim_cpp11_features.pl bdlma_localbufferedobject.h

# define COMPILING_BDLMA_LOCALBUFFEREDOBJECT_H
# include <bdlma_localbufferedobject_cpp03.h>
# undef COMPILING_BDLMA_LOCALBUFFEREDOBJECT_H

// clang-format on
#else

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
#define BDLMA_LOCAL_BUFFERED_VALUE_IS_ASSIGNABLE(DST, SRC)                    \
                                            std::is_assignable<DST, SRC>::value
#else
#define BDLMA_LOCAL_BUFFERED_VALUE_IS_ASSIGNABLE(DST, SRC) true
#endif

namespace BloombergLP {
namespace bdlma {

                        // =========================
                        // class LocalBufferedObject
                        // =========================

/// This `class` contains an object of type `t_TYPE` and a local sequential
/// allocator with an arena size of `t_BUFFER_SIZE`, from which the `t_TYPE`
/// object allocates memory, in a single object.  The
/// `t_DISABLE_DESTRUCTION` template parameter can be used to prevent this
/// `class` from calling `~t_TYPE()` in cases where it is known that
/// `t_TYPE` manages no resources other than memory, since the memory will
/// be adequately managed by the local sequential allocator.
template <class       t_TYPE,
          bsl::size_t t_BUFFER_SIZE         = 1024,
          bool        t_DISABLE_DESTRUCTION = false>
class LocalBufferedObject {

    BSLMF_ASSERT(bslma::UsesBslmaAllocator<t_TYPE>::value);

  public:
    // PUBLIC TYPES
    typedef t_TYPE                  value_type;
    typedef bsl::allocator<>        allocator_type;

    enum { k_BUFFER_SIZE = t_BUFFER_SIZE };   // The 'size' template
                                              // parameter to 'LocalAllocator'
                                              // takes an 'int', not 'size_t'.

  private:
    // DATA
    bdlma::LocalSequentialAllocator<k_BUFFER_SIZE> d_arenaAllocator;
    bsls::ObjectBuffer<value_type>                 d_object;

  private:
    // NOT IMPLEMENTED
    LocalBufferedObject(const LocalBufferedObject&) BSLS_KEYWORD_DELETED;
    LocalBufferedObject& operator=(const LocalBufferedObject&)
                                                          BSLS_KEYWORD_DELETED;

    // PRIVATE MANIPULATORS

    /// Call `d_arenaAllocator.release()`.  If `t_DISABLE_DESTRUCTION` is
    /// `false`, destroy the held object first.
    void destroyHeldObject();

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(LocalBufferedObject,
                                   BloombergLP::bslmf::UsesAllocatorArgT);

    // CREATORS
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    /// Create a `value_type` object using the specified `args` that will
    /// allocate memory from a sequential allocator based on a local stack
    /// buffer of (template parameter) `t_BUFFER_SIZE` size; if local stack
    /// memory is exhausted, use the default allocator to supply additional
    /// heap memory.
    template <class... ARGS>
    explicit LocalBufferedObject(
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...  args);

    /// Create a `value_type` object using the specified `args` that will
    /// allocate memory from a sequential allocator based on a local stack
    /// buffer of (template parameter) `t_BUFFER_SIZE` size; if local stack
    /// memory is exhausted, use the specified `allocator` to supply
    /// additional heap memory.
    template <class... ARGS>
    LocalBufferedObject(bsl::allocator_arg_t                         ,
                        allocator_type                              allocator,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...  args);
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    /// Create the `value_type` object using the specified
    /// `initializer_list` and using the sequential allocator based on a
    /// local stack buffer of (template parameter) `t_BUFFER_SIZE` size; if
    /// local stack memory is exausted, use the default allocator to supply
    /// additional heap memory.
    template <class INIT_LIST_TYPE>
    LocalBufferedObject(std::initializer_list<INIT_LIST_TYPE>  il);

    /// Create the `value_type` object using the specified
    /// `initializer_list` and using the sequential allocator based on a
    /// local stack buffer of (template parameter) `t_BUFFER_SIZE` size; if
    /// local stack memory is exausted, use the specified `allocator` to
    /// supply additional heap memory.
    template <class INIT_LIST_TYPE>
    LocalBufferedObject(bsl::allocator_arg_t                   ,
                        allocator_type                         allocator,
                        std::initializer_list<INIT_LIST_TYPE>  il);

#endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS

    /// Destroy this object and free any memory it uses, and if the
    /// (template parameter) `t_DISABLE_DESTRUCTION` is `true`, do this
    /// *without* calling the destructor of `value_type` (see
    /// `t_DISABLE_DESTRUCTION` template parameter in the component doc).
    ~LocalBufferedObject();

    // MANIPULATORS
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    /// Assign the object held by this container to the specified `value`.
    template <class t_ANY_TYPE>
    typename bsl::enable_if<BDLMA_LOCAL_BUFFERED_VALUE_IS_ASSIGNABLE(
                               t_TYPE,
                               BSLS_COMPILERFEATURES_FORWARD_REF(t_ANY_TYPE) ),
                            LocalBufferedObject>::type&
    operator=(BSLS_COMPILERFEATURES_FORWARD_REF(t_ANY_TYPE) value);
#endif

    /// Return a pointer providing modifiable access to the underlying
    /// `t_TYPE` object.
    value_type *operator->();

    /// Return a reference providing modifiable access to the underlying
    /// `t_TYPE` object.
    value_type& operator*();

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    /// Destroy the `value_type` object unless `t_DISABLE_DESTRUCTION` is
    /// true, then release all allocated memory, the re-construct a new
    /// `value_type` object using the specified `args` and using the
    /// sequential allocator based on the local stack buffer.
    template <class... ARGS>
    void emplace(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args);
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    /// Destroy the `value_type` object unless `t_DISABLE_DESTRUCTION` is
    /// true, then release all allocated memory, the re-construct a new
    /// `value_type` object using the specified `il` and using the
    /// sequential allocator based on the local stack buffer.
    template <class INIT_LIST_TYPE>
    void emplace(std::initializer_list<INIT_LIST_TYPE> il);

#endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS

    // ACCESSORS

    /// Return a pointer providing const access to the underlying `t_TYPE`
    /// object.
    const value_type *operator->() const;

    /// Return a reference providing const access to the underlying `t_TYPE`
    /// object.
    const value_type& operator*() const;

    /// Return the alloctor passed at construction, used to provide heap
    /// memory after the local stack buffer is exhausted.  Note that this
    /// is not the arena allocator contained in this object.
    allocator_type get_allocator() const;
};

// ============================================================================
//                       INLINE & TEMPLATE DEFINITIONS
// ============================================================================

                            // -------------------
                            // LocalBufferedObject
                            // -------------------

// PRIVATE MANIPULATORS
template <class t_TYPE, bsl::size_t t_BUFFER_SIZE, bool t_DISABLE_DESTRUCTION>
inline
void LocalBufferedObject<t_TYPE, t_BUFFER_SIZE, t_DISABLE_DESTRUCTION>::
                                                            destroyHeldObject()
{
    if (!t_DISABLE_DESTRUCTION) {
        d_object.address()->~value_type();
    }
}

// CREATORS
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class t_TYPE, bsl::size_t t_BUFFER_SIZE, bool t_DISABLE_DESTRUCTION>
template <class... ARGS>
inline
LocalBufferedObject<t_TYPE, t_BUFFER_SIZE, t_DISABLE_DESTRUCTION>::
        LocalBufferedObject(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...   args)
: d_arenaAllocator()
{
    bslma::ConstructionUtil::construct(
                                d_object.address(),
                                bslma::AllocatorUtil::adapt(&d_arenaAllocator),
                                BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
}

template <class t_TYPE, bsl::size_t t_BUFFER_SIZE, bool t_DISABLE_DESTRUCTION>
template <class... ARGS>
inline
LocalBufferedObject<t_TYPE, t_BUFFER_SIZE, t_DISABLE_DESTRUCTION>::
    LocalBufferedObject(bsl::allocator_arg_t                         ,
                        bsl::allocator<>                             allocator,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...   args)
: d_arenaAllocator(bslma::AllocatorUtil::adapt(allocator))
{
    bslma::ConstructionUtil::construct(
                                d_object.address(),
                                bslma::AllocatorUtil::adapt(&d_arenaAllocator),
                                BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
}
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class t_TYPE, bsl::size_t t_BUFFER_SIZE, bool t_DISABLE_DESTRUCTION>
template <class INIT_LIST_TYPE>
inline
LocalBufferedObject<t_TYPE, t_BUFFER_SIZE, t_DISABLE_DESTRUCTION>::
                 LocalBufferedObject(std::initializer_list<INIT_LIST_TYPE>  il)
: d_arenaAllocator()
{
    bslma::ConstructionUtil::construct(
                                d_object.address(),
                                bslma::AllocatorUtil::adapt(&d_arenaAllocator),
                                il);
}

template <class t_TYPE, bsl::size_t t_BUFFER_SIZE, bool t_DISABLE_DESTRUCTION>
template <class INIT_LIST_TYPE>
inline
LocalBufferedObject<t_TYPE, t_BUFFER_SIZE, t_DISABLE_DESTRUCTION>::
          LocalBufferedObject(bsl::allocator_arg_t                   ,
                              bsl::allocator<>                       allocator,
                              std::initializer_list<INIT_LIST_TYPE>  il)
: d_arenaAllocator(bslma::AllocatorUtil::adapt(allocator))
{
    bslma::ConstructionUtil::construct(
                                d_object.address(),
                                bslma::AllocatorUtil::adapt(&d_arenaAllocator),
                                il);
}
#endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS

template <class t_TYPE, bsl::size_t t_BUFFER_SIZE, bool t_DISABLE_DESTRUCTION>
inline
LocalBufferedObject<t_TYPE, t_BUFFER_SIZE, t_DISABLE_DESTRUCTION>::
                                                         ~LocalBufferedObject()
{
    destroyHeldObject();
}

// MANIPULATORS
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
/// Assign the object held by this container to the specified `value`.
template <class t_TYPE, bsl::size_t t_BUFFER_SIZE, bool t_DISABLE_DESTRUCTION>
template <class t_ANY_TYPE>
typename bsl::enable_if<
     BDLMA_LOCAL_BUFFERED_VALUE_IS_ASSIGNABLE(
                               t_TYPE,
                               BSLS_COMPILERFEATURES_FORWARD_REF(t_ANY_TYPE) ),
     LocalBufferedObject<t_TYPE, t_BUFFER_SIZE, t_DISABLE_DESTRUCTION> >::type&
LocalBufferedObject<t_TYPE, t_BUFFER_SIZE, t_DISABLE_DESTRUCTION>::operator=(
                           BSLS_COMPILERFEATURES_FORWARD_REF(t_ANY_TYPE) value)
{
    d_object.object() = BSLS_COMPILERFEATURES_FORWARD(t_ANY_TYPE, value);

    return *this;
}
#endif

template <class t_TYPE, bsl::size_t t_BUFFER_SIZE, bool t_DISABLE_DESTRUCTION>
inline
t_TYPE *LocalBufferedObject<t_TYPE, t_BUFFER_SIZE, t_DISABLE_DESTRUCTION>::
                                                                   operator->()
{
    return d_object.address();
}

template <class t_TYPE, bsl::size_t t_BUFFER_SIZE, bool t_DISABLE_DESTRUCTION>
inline
t_TYPE& LocalBufferedObject<t_TYPE, t_BUFFER_SIZE, t_DISABLE_DESTRUCTION>::
                                                                    operator*()
{
    return d_object.object();
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class t_TYPE, bsl::size_t t_BUFFER_SIZE, bool t_DISABLE_DESTRUCTION>
template <class... ARGS>
void LocalBufferedObject<t_TYPE, t_BUFFER_SIZE, t_DISABLE_DESTRUCTION>::
                       emplace(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args)
{
    destroyHeldObject();
    d_arenaAllocator.release();
    bslma::ConstructionUtil::construct(
                                d_object.address(),
                                bslma::AllocatorUtil::adapt(&d_arenaAllocator),
                                BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
}
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class t_TYPE, bsl::size_t t_BUFFER_SIZE, bool t_DISABLE_DESTRUCTION>
template <class INIT_LIST_TYPE>
void LocalBufferedObject<t_TYPE, t_BUFFER_SIZE, t_DISABLE_DESTRUCTION>::
                              emplace(std::initializer_list<INIT_LIST_TYPE> il)
{
    destroyHeldObject();
    d_arenaAllocator.release();
    bslma::ConstructionUtil::construct(
                                d_object.address(),
                                bslma::AllocatorUtil::adapt(&d_arenaAllocator),
                                il);
}
#endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS

// ACCESSORS
template <class t_TYPE, bsl::size_t t_BUFFER_SIZE, bool t_DISABLE_DESTRUCTION>
inline
const t_TYPE *
LocalBufferedObject<t_TYPE, t_BUFFER_SIZE, t_DISABLE_DESTRUCTION>::
                                                             operator->() const
{
    return d_object.address();
}

template <class t_TYPE, bsl::size_t t_BUFFER_SIZE, bool t_DISABLE_DESTRUCTION>
inline
const t_TYPE&
LocalBufferedObject<t_TYPE, t_BUFFER_SIZE, t_DISABLE_DESTRUCTION>::
                                                              operator*() const
{
    return d_object.object();
}

template <class t_TYPE, bsl::size_t t_BUFFER_SIZE, bool t_DISABLE_DESTRUCTION>
inline
bsl::allocator<>
LocalBufferedObject<t_TYPE, t_BUFFER_SIZE, t_DISABLE_DESTRUCTION>::
                                                          get_allocator() const
{
    return allocator_type(d_arenaAllocator.allocator());
}

#undef BDLMA_LOCAL_BUFFERED_VALUE_IS_ASSIGNABLE

}  // close package namespace
}  // close enterprise namespace

#endif // End C++11 code

#endif // ifndef INCLUDED_BDLMA_LOCALBUFFEREDOBJECT

// ----------------------------------------------------------------------------
// Copyright 2024 Bloomberg Finance L.P.
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
