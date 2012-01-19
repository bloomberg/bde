// bslalg_arraydestructionprimitives.h                                -*-C++-*-
#ifndef INCLUDED_BSLALG_ARRAYDESTRUCTIONPRIMITIVES
#define INCLUDED_BSLALG_ARRAYDESTRUCTIONPRIMITIVES

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide primitive algorithms that destroy arrays.
//
//@CLASSES:
//  bslalg::ArrayDestructionPrimitives: namespace for array algorithms
//
//@SEE_ALSO: bslalg_scalarprimitives, bslalg_typetraits
//
//@AUTHOR: Herve Bronnimann (hbronnim)
//
//@DESCRIPTION: This component provides utilies to destroy arrays with a
// uniform interface, but selecting a different implementation according to the
// traits possessed by the underlying type.
//
// The traits under consideration by this component are:
//..
//  Trait                             Note
//  -----                             -------------------------------------
//  bslalg::TypeTraitBitwiseCopyable  Expressed in English as "TYPE has the
//                                    bit-wise copyable trait", or "TYPE is
//                                    bit-wise copyable", this trait also
//                                    implies that destructor calls can be
//                                    elided with no effect on observable
//                                    behavior.
//
//..
///Usage
///-----
// This component is for use by the 'bslstl' package.  Other clients should use
// STL algorithms (in headers '<algorithm>' and '<memory>').

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_HASTRAIT
#include <bslalg_hastrait.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>  // size_t
#define INCLUDED_CSTDDEF
#endif

#ifndef INCLUDED_CSTRING
#include <cstring>  // memset, memcpy, memmove
#define INCLUDED_CSTRING
#endif

namespace BloombergLP {

namespace bslalg {

                  // =================================
                  // struct ArrayDestructionPrimitives
                  // =================================

struct ArrayDestructionPrimitives {
    // This 'struct' provides a namespace for a suite of utility functions that
    // destroy arrays of elements of the parameterized type 'TARGET_TYPE'.
    // Depending on the traits of 'TARGET_TYPE', the destructor may be invoked,
    // or not (optimized away by no-op).

    // PRIVATE CLASS METHODS
    template <class TARGET_TYPE>
    static void destroy(TARGET_TYPE        *begin,
                        TARGET_TYPE        *end,
                        bslmf::MetaInt<1>);
    template <class TARGET_TYPE>
    static void destroy(TARGET_TYPE        *begin,
                        TARGET_TYPE        *end,
                        bslmf::MetaInt<0>);
        // Destroy each instance of 'TARGET_TYPE' in the array beginning at the
        // specified 'begin' address and ending immediately before the
        // specified 'end' address.  Use the destructor of the parameterized
        // 'TARGET_TYPE', or do nothing if the 'TARGET_TYPE' is bit-wise
        // copyable (i.e., if the last argument is of type
        // 'bslmf::MetaInt<1>').  Note that the last argument is for
        // overloading resolution only and its value is ignored.

  public:
    // CLASS METHODS
    template <class TARGET_TYPE>
    static void destroy(TARGET_TYPE *begin, TARGET_TYPE *end);
        // Destroy of the elements in the segment of an array of parameterized
        // 'TARGET_TYPE' beginning at the specified 'begin' address and ending
        // immediately before the specified 'end' address, as if by calling the
        // destructor on each object.  If 'begin == 0' and 'end == 0' this
        // function has no effect.  The behavior is undefined unless either (1)
        // 'begin <= end', 'begin != 0', and 'end != 0', or (2)
        // 'begin == 0 && end == 0'.  Note that this method does not deallocate
        // any memory (except memory deallocated by the element destructor
        // calls).
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                  // ---------------------------------
                  // struct ArrayDestructionPrimitives
                  // ---------------------------------

// PRIVATE CLASS METHODS
template <class TARGET_TYPE>
inline
void ArrayDestructionPrimitives::destroy(TARGET_TYPE       *begin,
                                         TARGET_TYPE       *end,
                                         bslmf::MetaInt<1>)
{
    // 'BitwiseCopyable' is a valid surrogate for 'HasTrivialDestructor'.

#ifdef BDE_BUILD_TARGET_SAFE
    bsls::Types::size_type numBytes = (const char*)end - (const char*)begin;
    std::memset(begin, 0xa5, numBytes);
#else
    (void)begin;
    (void)end;
#endif
}

template <class TARGET_TYPE>
void ArrayDestructionPrimitives::destroy(TARGET_TYPE       *begin,
                                         TARGET_TYPE       *end,
                                         bslmf::MetaInt<0>)
{
    for (; begin != end; ++begin) {
        begin->~TARGET_TYPE();
    }
}

// CLASS METHODS
template <class TARGET_TYPE>
inline
void ArrayDestructionPrimitives::destroy(TARGET_TYPE *begin,
                                         TARGET_TYPE *end)
{
    BSLS_ASSERT_SAFE(begin || !end);
    BSLS_ASSERT_SAFE(end   || !begin);
    BSLS_ASSERT_SAFE(begin <= end);

    typedef typename HasTrait<TARGET_TYPE,
                              TypeTraitBitwiseCopyable>::Type Trait;
    destroy(begin, end, Trait());
}

}  // close package namespace

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
