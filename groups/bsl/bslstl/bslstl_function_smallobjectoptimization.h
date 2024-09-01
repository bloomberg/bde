// bslstl_function_smallobjectoptimization.h                          -*-C++-*-
#ifndef INCLUDED_BSLSTL_FUNCTION_SMALLOBJECTOPTIMIZATION
#define INCLUDED_BSLSTL_FUNCTION_SMALLOBJECTOPTIMIZATION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide small-object optimization buffer for `bsl::function`.
//
//@CLASSES:
//  bslstl::Function_SmallObjectOptimization: impl utils for `bsl::function`.
//
//@SEE_ALSO: bslstl_function
//
//@DESCRIPTION:  This private, subordinate component provides a utility
// `struct`, `bslstl::Function_SmallObjectOptimization`, that provides a suite
// of types, type traits, and constants used in the implementation of
// `bsl::function`, and its "small-object optimization" in particular.

#include <bslscm_version.h>

#include <bslmf_integralconstant.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_isnothrowmoveconstructible.h>

#include <bsls_alignmentutil.h>

#include <cstddef>  // 'std::size_t'

namespace BloombergLP {
namespace bslstl {

                   // ======================================
                   // class Function_SmallObjectOptimization
                   // ======================================

/// This utility `struct` provides a namespace for several types, type
/// traits, and constants used in the implementation of the small-object
/// optimization for `bsl::function`.
class Function_SmallObjectOptimization {

    // PRIVATE TYPES
    class Dummy;
        // 'Dummy' is an incomplete type that this class uses to declare
        // pointers to member functions and pointers to member data.

    /// `MaxAlignedType` is an alias to a type that has the maximum
    /// alignment, and is not over-aligned, for the current platform.
    typedef bsls::AlignmentUtil::MaxAlignedType MaxAlignedType;

  public:
    // TYPES

    /// This `union` defines the storage area for a functor representation.
    /// The design uses the small-object optimization in an attempt to avoid
    /// allocations for objects that are no larger than `InplaceBuffer`.
    /// When the target object is no larger than `InplaceBuffer`, the
    /// small-object optimization can be used by storing the target directly
    /// within the `InplaceBuffer`.
    ///
    /// Note that union members other than `d_object_p` are just fillers to
    /// make sure that a function or member function pointer can fit without
    /// allocation, and that `InplaceBuffer` has maximum alignment.  The
    /// `d_minbuf` member ensures that `InplaceBuffer` is large enough to
    /// hold modestly complex functors, like `bdlf::Bind` objects and other
    /// functors that store embedded arguments, eliminating the need to
    /// allocate memory from the heap for the footprint of such objects.
    ///
    /// The size of this type ensures that the inplace buffer will be 6
    /// pointers in size, and the total footprint of a `bsl::function`
    /// object on most platforms will be 10 pointers, which matches the
    /// sizes of previous implementations of `bdef_Function`.
    union InplaceBuffer {

        // PUBLIC DATA
        void                  *d_object_p;     // pointer to external rep
        void                 (*d_func_p)();    // pointer to function
        void          (Dummy::*d_memFunc_p)(); // pointer to member function
        int            Dummy::*d_memData_p;    // pointer to member data
        MaxAlignedType         d_align;        // force alignment
        void                  *d_minbuf[6];    // force minimum size
    };

    // CONSTANTS

    /// `SooFuncSize` (below) adds this value to the size of a small
    /// stateful functor to indicate that, despite being small, it should
    /// not be allocated inplace using the small object optimization (SOO),
    /// i.e., because it does not have a nothrow move constructor and
    /// cannot, therefore, be swapped safely.  When a size larger than this
    /// constant is seen, the actual object size can be determined by
    /// subtracting this constant.  A useful quality of this encoding is
    /// that if `SZ <= sizeof(InplaceBuffer)` for some object size `SZ`,
    /// then `SZ + k_NON_SOO_SMALL_SIZE > sizeof(InplaceBuffer)`, thus
    /// `SooFuncSize` (below) for any object that should not be allocated
    /// inplace is larger than `sizeof(InplaceBuffer)`, and the
    /// `SooFuncSize` for any object that *should* be allocated inplace is
    /// smaller than or equal to `sizeof(InplaceBuffer)`, making the test
    /// for "is inplace function" simple.  Note that it is assumed that no
    /// actual object has a size larger than this constant.
    static const std::size_t k_NON_SOO_SMALL_SIZE = ~sizeof(InplaceBuffer);

    // TYPES

    /// This class template provides a metafunction that `bsl::function`
    /// uses to determine the size of an object, and whether to store it
    /// using the small-object optimization (SOO) or not.  The `value`
    /// member of this class encodes the size of the specified `TP` type as
    /// follows:
    ///
    /// 1. If `TP` is larger than `InplaceBuffer`, then
    ///    `value == sizeof(TP)`.
    /// 2. Otherwise, if `TP` has a non-throwing destructive move (i.e.,
    ///    it is bitwise movable or has a `noexcept` move constructor),
    ///    then `value == sizeof(TP)`.
    /// 3. Otherwise, `value == sizeof(TP) + k_NON_SOO_SMALL_SIZE`.  This
    ///    encoding indicates that move might throw and, therefore, `TP`
    ///    should not be allocated in place even though it would fit in the
    ///    footprint of an `InplaceBuffer`.
    ///
    /// Note that the `Soo` prefix is used to indicate that an identifier
    /// uses the above protocol.  Thus, a variable called `SooSize` is
    /// assumed to be encoded as above, whereas a variable called `size`
    /// can generally be assumed not to be encoded that way.
    template <class TP>
    class SooFuncSize {

        // PRIVATE CONSTANTS

        // This constant is `k_NON_SOO_SMALL_SIZE` if `TP` is small enough
        // to fit within the footprint of `InplaceBuffer` but should not be
        // placed there because it is neither bitwise movable nor
        // nothrow-move constructible; otherwise this constant is zero.
        static const std::size_t k_SOO_ENCODING_OFFSET =
                sizeof(TP) > sizeof(InplaceBuffer)               ? 0 :
                bsl::is_nothrow_move_constructible<TP>::value    ? 0 :
                bslmf::IsBitwiseMoveable<TP>::value              ? 0 :
                k_NON_SOO_SMALL_SIZE;

      public:
        // CONSTANTS

        /// `value` encodes the size of the `TP` type using the algorithm
        /// defined in the documentation for this class.
        static const std::size_t value = sizeof(TP) + k_SOO_ENCODING_OFFSET;
    };

    // TYPES

    /// This class is a Boolean metafunction that determines whether or not
    /// the specified `FN` template parameter should be allocated within the
    /// footprint of the `InplaceBuffer` (i.e., using the small-object
    /// optimization.)
    ///
    /// # Implementation Note
    /// In the future, `InplaceFunc` should also consider the alignment of
    /// `FN` in its determination of whether or not `bsl::function` will use
    /// the small-object optimization.  However, `bsl::function` currently
    /// has no way to specify alignment when it allocates memory.
    template <class FN>
    struct IsInplaceFunc
    : bsl::integral_constant<bool,
                             SooFuncSize<FN>::value <= sizeof(InplaceBuffer)> {
    };
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

             // ---------------------------------------------------
             // class Function_SmallObjectOptimization::SooFuncSize
             // ---------------------------------------------------

// PRIVATE CONSTANTS
template <class TP>
const std::size_t
    Function_SmallObjectOptimization::SooFuncSize<TP>::k_SOO_ENCODING_OFFSET;

// CONSTANTS
template <class TP>
const std::size_t Function_SmallObjectOptimization::SooFuncSize<TP>::value;

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
