// bdlb_nullableallocatedvalue_pointerbitspair.h                      -*-C++-*-

#ifndef INCLUDED_BDLB_NULLABLEALLOCATEDVALUE_POINTERBITSPAIR
#define INCLUDED_BDLB_NULLABLEALLOCATEDVALUE_POINTERBITSPAIR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a mechanism for using the unused bits of a pointer.
//
//@CLASSES:
//  bdlb::NullableAllocatedValue_PointerBitsPair: pointer and bits together.
//
//@SEE_ALSO: bdlb_nullablesllocatedvalue
//
//@DESCRIPTION: This private, subordinate component to
// `bdlb_nullableallocatedvalue` provides a templated class,
// `bdlb::NullableAllocatedValue_PointerBitsPair`, that allows access to the
// otherwise unused bits in a pointer to an object.  The
// `bdlb::NullableAllocatedValue` class template uses
// `bdlb::NullableAllocatedValue_PointerBitsPair` to store some state about the
// object in the `pointer to allocator` field, saving 4 to 8 bytes (depending
// on the platform) for each instantiation.
//
// `bldb::NullableAllocatedValue_PointerBitsPair` is a non-allocator-aware,
// value-semantic type.

#include <bslscm_version.h>

#include <bslalg_swaputil.h>

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_alignmentfromtype.h>
#include <bsls_compilerfeatures.h>
#include <bsls_platform.h>

#include <bsl_cstddef.h>    // bsl::size_t

#include <cstddef>
#include <cstdlib>
#include <typeinfo>

#include <stdint.h>    // uintptr_t

namespace BloombergLP {
namespace bdlb {

                     // =======================
                     // class Pointer_Bits_Pair
                     // =======================

/// This is a component-private class.  Do not use.
///
/// This regular, value-semantic class provides a mechanism for storing a
/// number of bit flags in the otherwise unused bits of a pointer to the
/// template parameter class `t_TYPE`.   The number of bits that can be
/// stored is dependent upon the required alignment of the class `t_TYPE`.
template <class t_TYPE, unsigned t_NUM_BITS>
class NullableAllocatedValue_PointerBitsPair {

    // DATA

    /// `k_Mask` is a bitmask constant containing a `1` for each of the
    /// positions in the stored value where a flag can be stored, and a `0`
    /// for all of the other positions.
    enum { k_Mask = (1 << t_NUM_BITS ) - 1 };

    // Contains the pointer value and the flag bits; or-ed together.
    uintptr_t d_Value;

    BSLMF_ASSERT(t_NUM_BITS >  0); // Can't store zero bits
    BSLMF_ASSERT(t_NUM_BITS <= 8); // Too many bits

  public:
    // CREATORS

    /// Construct an object holding a null pointer value and all flags set
    /// to false.
    NullableAllocatedValue_PointerBitsPair();

    /// Construct an object holding the specified `ptr` and optionally
    /// specified `flags`.  The behavior is undefined unless
    /// `flags <= k_Mask`.
    explicit NullableAllocatedValue_PointerBitsPair(t_TYPE   *ptr,
                                                    unsigned  flags = 0);

    /// Create a `NullableAllocatedValue_PointerBitsPair` having the same
    /// value as the specified `original` object.
    //! NullableAllocatedValue_PointerBitsPair(
    //!       const NullableAllocatedValue_PointerBitsPair& original) = default;

    /// Destroy this object.
    //! ~NullableAllocatedValue_PointerBitsPair() = default;

    // ACCESSORS

    /// Return `true` if this object and the specified `other` have the same
    /// value, and `false` otherwise.  Two
    /// `NullableAllocatedValue_PointerBitsPair` objects have the same value
    /// when their pointer and flags are the same.
    bool equal(const NullableAllocatedValue_PointerBitsPair& other) const;

    /// Return the held pointer.
    t_TYPE* getPointer () const;

    /// Return the value of the flag specified by `idx`.  The behavior is
    /// undefined unless `idx < t_NUM_BITS`.
    bool readFlag(unsigned idx) const;

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a reference providing modifiable access to this object.
    //! NullableAllocatedValue_PointerBitsPair& operator=(
    //!           const NullableAllocatedValue_PointerBitsPair& rhs) = default;

    /// Clear the flag specified by `idx`.  The behavior is undefined unless
    /// `idx < t_NUM_BITS`.
    void clearFlag(unsigned idx);

    /// Set the flag specified by `idx`.  The behavior is undefined unless
    /// `idx < t_NUM_BITS`.
    void setFlag(unsigned idx);

    /// Set the held pointer to the value of the specified `new_ptr`.
    void setPointer (t_TYPE *new_ptr);

    /// Efficiently exchange the value of this object with the value of the
    /// specified `other` object.  This method provides the no-throw
    /// exception-safety guarantee.
    void swap(NullableAllocatedValue_PointerBitsPair& other);

    };

// FREE FUNCTIONS

/// Return `true` if the specified `lhs` and `rhs` objects have the same
/// value, and `false` otherwise.  Two
/// `NullableAllocatedValue_PointerBitsPair` objects have the same value
/// when their pointer and flags are the same.
template <class t_TYPE, unsigned t_NUM_BITS>
bool operator==(
        const NullableAllocatedValue_PointerBitsPair<t_TYPE, t_NUM_BITS>& lhs,
        const NullableAllocatedValue_PointerBitsPair<t_TYPE, t_NUM_BITS>& rhs);

/// Return `false` if the specified `lhs` and `rhs` objects have the same
/// value, and `true` otherwise.  Two
/// `NullableAllocatedValue_PointerBitsPair` objects have the same value
/// when their pointer and flags are the same.
template <class t_TYPE, unsigned t_NUM_BITS>
bool operator!=(
        const NullableAllocatedValue_PointerBitsPair<t_TYPE, t_NUM_BITS>& lhs,
        const NullableAllocatedValue_PointerBitsPair<t_TYPE, t_NUM_BITS>& rhs);

/// Exchange the values of the specified `a` and `b` objects.  This function
/// provides the no-throw exception-safety guarantee.
template <class t_TYPE, unsigned t_NUM_BITS>
void swap(NullableAllocatedValue_PointerBitsPair<t_TYPE, t_NUM_BITS>& a,
          NullableAllocatedValue_PointerBitsPair<t_TYPE, t_NUM_BITS>& b);

}  // close package namespace


// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

               // --------------------------------------------
               // class NullableAllocatedValue_PointerBitsPair
               // --------------------------------------------


// CREATORS
template <class t_TYPE, unsigned t_NUM_BITS>
inline
bdlb::NullableAllocatedValue_PointerBitsPair<t_TYPE, t_NUM_BITS>
                                     ::NullableAllocatedValue_PointerBitsPair()
: d_Value(0)
{
    // ensure that t_TYPE is sufficiently aligned to store t_NUM_BITS flags
    BSLMF_ASSERT(
             static_cast<bsl::size_t>(k_Mask) <
             static_cast<bsl::size_t>(bsls::AlignmentFromType<t_TYPE>::VALUE));
}

template <class t_TYPE, unsigned t_NUM_BITS>
inline
bdlb::NullableAllocatedValue_PointerBitsPair<t_TYPE, t_NUM_BITS>
          ::NullableAllocatedValue_PointerBitsPair(t_TYPE *ptr, unsigned flags)
: d_Value(reinterpret_cast<uintptr_t>(ptr))
{
    BSLS_ASSERT_OPT((d_Value & k_Mask) == 0); // pointer is correctly aligned
    BSLS_ASSERT_OPT(flags <= k_Mask);         // flags are too large to fit
    d_Value |= (flags & k_Mask);              // set the flags
}

// ACCESSORS
template <class t_TYPE, unsigned t_NUM_BITS>
inline
bool bdlb::NullableAllocatedValue_PointerBitsPair<t_TYPE, t_NUM_BITS>::equal(
 const NullableAllocatedValue_PointerBitsPair<t_TYPE, t_NUM_BITS>& other) const
{
    return d_Value == other.d_Value;
}


template <class t_TYPE, unsigned t_NUM_BITS>
inline
t_TYPE* bdlb::NullableAllocatedValue_PointerBitsPair<t_TYPE, t_NUM_BITS>
                                                           ::getPointer() const
{
    return reinterpret_cast<t_TYPE *>(d_Value & ~uintptr_t(k_Mask));
}

template <class t_TYPE, unsigned t_NUM_BITS>
inline
bool bdlb::NullableAllocatedValue_PointerBitsPair<t_TYPE, t_NUM_BITS>
                                                 ::readFlag(unsigned idx) const
{
    BSLS_ASSERT_OPT(idx <= t_NUM_BITS);
    return 0 != (d_Value & (1 << idx));
}

// MANIPULATORS
template <class t_TYPE, unsigned t_NUM_BITS>
inline
void bdlb::NullableAllocatedValue_PointerBitsPair<t_TYPE, t_NUM_BITS>
                                                      ::clearFlag(unsigned idx)
{
    BSLS_ASSERT_OPT(idx <= t_NUM_BITS);

    const uintptr_t mask = 1U << idx;
    d_Value &= ~mask;
}

template <class t_TYPE, unsigned t_NUM_BITS>
inline
void bdlb::NullableAllocatedValue_PointerBitsPair<t_TYPE, t_NUM_BITS>
                                                        ::setFlag(unsigned idx)
{
    BSLS_ASSERT_OPT(idx <= t_NUM_BITS);

    const uintptr_t mask = 1U << idx;
    d_Value |= mask;
}

template <class t_TYPE, unsigned t_NUM_BITS>
inline
void bdlb::NullableAllocatedValue_PointerBitsPair<t_TYPE, t_NUM_BITS>
                                                  ::setPointer(t_TYPE *new_ptr)
{
    const uintptr_t value = reinterpret_cast<uintptr_t>(new_ptr);
    BSLS_ASSERT_OPT((value & k_Mask) == 0); // pointer is sufficiently aligned

    // Copy the existing flags over the new pointer
    d_Value = value | (d_Value & k_Mask);
}

template <class t_TYPE, unsigned t_NUM_BITS>
inline
void bdlb::NullableAllocatedValue_PointerBitsPair<t_TYPE, t_NUM_BITS>::swap(
                                 NullableAllocatedValue_PointerBitsPair& other)
{
    bslalg::SwapUtil::swap(&d_Value, &other.d_Value);
}


// FREE FUNCTIONS
template <class t_TYPE, unsigned t_NUM_BITS>
inline
bool bdlb::operator==(
   const bdlb::NullableAllocatedValue_PointerBitsPair<t_TYPE, t_NUM_BITS>& lhs,
   const bdlb::NullableAllocatedValue_PointerBitsPair<t_TYPE, t_NUM_BITS>& rhs)
{
    return lhs.equal(rhs);
}

template <class t_TYPE, unsigned t_NUM_BITS>
inline
bool bdlb::operator!=(
   const bdlb::NullableAllocatedValue_PointerBitsPair<t_TYPE, t_NUM_BITS>& lhs,
   const bdlb::NullableAllocatedValue_PointerBitsPair<t_TYPE, t_NUM_BITS>& rhs)
{
    return !(lhs == rhs);
}

template <class t_TYPE, unsigned t_NUM_BITS>
inline
void bdlb::swap(
           bdlb::NullableAllocatedValue_PointerBitsPair<t_TYPE, t_NUM_BITS>& a,
           bdlb::NullableAllocatedValue_PointerBitsPair<t_TYPE, t_NUM_BITS>& b)
{
    a.swap(b);
}

}  // close enterprise namespace

#endif // ! defined(INCLUDED_BDLB_NULLABLEALLOCATEDVALUE_POINTERBITSPAIR)

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
