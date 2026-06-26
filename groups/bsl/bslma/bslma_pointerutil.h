// bslma_pointerutil.h                                                -*-C++-*-
#ifndef INCLUDED_BSLMA_POINTERUTIL
#define INCLUDED_BSLMA_POINTERUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide utilities for pointer manipulation.
//
//@CLASSES:
//  bslma::PointerUtil: namespace for pointer utility functions
//
//@DESCRIPTION: This component provides a utility `struct`,
// `bslma::PointerUtil`, that serves as a namespace for `static` functions that
// perform low-level pointer manipulations useful for implementing portable
// generic facilities.  `voidify` returns a `void *` from a pointer to a
// potentially cv-qualified type, or from a function pointer on platforms that
// support the conversion.  `unqualify` returns a pointer to the cv-unqualified
// version of the pointed-to type, or returns a function pointer unchanged.
//
///Function Pointers and `void *`
/// - - - - - - - - - - - - - - -
// Conversion from a pointer-to-function to `void *` requires a
// `reinterpret_cast` and is conditionally-supported behavior in C++17; prior
// to C++17 it is undefined behavior.  In practice, this conversion is
// well-defined on all platforms that BDE is known to support, and is formally
// a part of the POSIX Standard.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using `voidify` for Placement New
/// - - - - - - - - - - - - - - - - - - - - - -
// Suppose we are implementing a simplified `optional`-like container that
// holds a value of (template parameter) `TYPE` in a raw buffer.  `TYPE` may
// be const-qualified (e.g., `MyOptional<const int>`).
//
// First, we define the class template with an aligned buffer and a flag:
// ```
//  template <class TYPE>
//  class MyOptional {
//      // DATA
//      union {
//          bsls::AlignmentUtil::MaxAlignedType d_align;
//          char                                d_buf[sizeof(TYPE)];
//      };
//      bool d_hasValue;
//
//    public:
//      // CREATORS
//      MyOptional() : d_hasValue(false) {}
//
//      // MANIPULATORS
//
//      /// Construct a `TYPE` object in this object's buffer having its
//      /// default value, and return a reference to the newly created
//      /// object.
//      TYPE& emplace();
//
//      // ACCESSORS
//      bool        hasValue() const { return d_hasValue; }
//      const TYPE& value()    const
//      {
//          return *reinterpret_cast<const TYPE *>(d_buf);
//      }
//  };
// ```
// Then we implement `emplace`.  We declare a pointer of the correct type to
// the return value so that we can efficiently return a reference to the object
// we are about to create.
// ```
//  template <class TYPE>
//  TYPE& MyOptional<TYPE>::emplace()
//  {
//      BSLS_ASSERT(!d_hasValue);
//
//      TYPE *addr = reinterpret_cast<TYPE *>(d_buf);
// ```
// Now, we use `voidify` to provide the address at which to construct the new
// object.  A placement-new expression requires a `void *` operand, but `TYPE`
// may be cv-qualified and `static_cast` alone cannot produce `void *` from a
// pointer to a cv-qualified type.
// ```
//      addr = ::new (bslma::PointerUtil::voidify(addr)) TYPE();
//      d_hasValue = true;
//      return *addr;
//  }
// ```
// Finally, we can use `MyOptional` with a const-qualified type.  Without
// `voidify`, the placement new inside `emplace` would not compile:
// ```
//  MyOptional<const int> opt;
//  const int& ref = opt.emplace();
//  assert(opt.hasValue());
//  assert(0 == ref);
// ```
//
///Example 2: Using `unqualify` for Generic Forwarding
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we are given a construction facility that constructs an object of
// (template parameter) `TYPE` at a specified address having its default
// value, and returns a pointer to the newly created object:
// ```
//  /// Construct an object of the specified (template parameter) `TYPE` at
//  /// the specified `address` having its default value, and return
//  /// `address`.
//  template <class TYPE>
//  TYPE *constructInPlace(TYPE *address);
// ```
// When forwarding to such a facility from our own generic code, we can strip
// any cv-qualification with `unqualify` so that the downstream facility is
// always invoked with the unqualified type, avoiding a redundant template
// instantiation for each cv-variant of the same underlying type across the
// entire program.  Casting away `const` is well-defined when the underlying
// storage was not originally declared `const` -- as is typical for raw buffers
// managed by containers and allocators.  Observe that the `int *` returned by
// `constructInPlace` implicitly converts to `const int *` or `volatile int *`
// when assigned back, correctly preserving the original cv-qualification:
// ```
//  union {
//      bsls::AlignmentUtil::MaxAlignedType d_align;
//      char                                d_buf[sizeof(int)];
//  } u = {};
//  int          *ip = reinterpret_cast<int *>(u.d_buf);
//  const int    *cp = reinterpret_cast<const int *>(u.d_buf);
//  volatile int *vp = reinterpret_cast<volatile int *>(u.d_buf);
//
//  ip = constructInPlace(bslma::PointerUtil::unqualify(ip));
//  assert(0 == *ip);
//
//  cp = constructInPlace(bslma::PointerUtil::unqualify(cp));
//  assert(0 == *cp);
//
//  vp = constructInPlace(bslma::PointerUtil::unqualify(vp));
//  assert(0 == *vp);
// ```

#include <bslscm_version.h>

#include <bslmf_integralconstant.h>
#include <bslmf_isfunction.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_platform.h>

namespace BloombergLP {
namespace bslma {

                          // ==================
                          // struct PointerUtil
                          // ==================

/// This `struct` provides a namespace for utility functions that perform
/// low-level pointer manipulations.
struct PointerUtil {
    // CLASS METHODS

    /// Return the specified `address` cast to a pointer to the
    /// cv-unqualified form of the (template parameter) `TYPE`; if `TYPE` is a
    /// function type, `address` is returned unchanged.
#ifdef BSLS_PLATFORM_CMP_SUN
    template <class TYPE>
    static TYPE *unqualify(const volatile TYPE *address);
#else
    template <class TYPE>
    static BSLS_KEYWORD_CONSTEXPR TYPE *unqualify(
                         TYPE *address) BSLS_KEYWORD_NOEXCEPT;
    template <class TYPE>
    static BSLS_KEYWORD_CONSTEXPR TYPE *unqualify(
                         const TYPE *address) BSLS_KEYWORD_NOEXCEPT;
    template <class TYPE>
    static BSLS_KEYWORD_CONSTEXPR TYPE *unqualify(
                         volatile TYPE *address) BSLS_KEYWORD_NOEXCEPT;
    template <class TYPE>
    static BSLS_KEYWORD_CONSTEXPR TYPE *unqualify(
                         const volatile TYPE *address) BSLS_KEYWORD_NOEXCEPT;
#endif

    /// Return the specified `address` cast to `void *`, stripping any
    /// cv-qualification from the (template parameter) `TYPE`.  Note that if
    /// `TYPE` is a function type, the conversion relies on conditionally-
    /// supported behavior (see {Function Pointers and `void *`}).
    template <class TYPE>
    static BSLS_KEYWORD_CONSTEXPR void *voidify(TYPE *address)
                                                         BSLS_KEYWORD_NOEXCEPT;

  private:
    // PRIVATE CLASS METHODS

#ifdef BSLS_PLATFORM_CMP_SUN
    /// Return the specified `address` cast to a pointer to the
    /// cv-unqualified form of `TYPE`.  The `bsl::false_type` tag indicates
    /// that `TYPE` is not a function type.
    template <class TYPE>
    static TYPE *unqualify(const volatile TYPE *address, bsl::false_type);

    /// Return the specified `address` unchanged.  The `bsl::true_type` tag
    /// indicates that `TYPE` is a function type.
    template <class TYPE>
    static TYPE *unqualify(const volatile TYPE *address, bsl::true_type);
#endif

#ifndef BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP17
    /// Return the specified `address` cast to `void *`, stripping any cv-
    /// qualification from `TYPE`.  The `bsl::false_type` tag indicates that
    /// `TYPE` is not a function type.
    template <class TYPE>
    static BSLS_KEYWORD_CONSTEXPR void *voidify(TYPE *address, bsl::false_type)
                                                         BSLS_KEYWORD_NOEXCEPT;

    /// Return the specified `address` cast to `void *`.  The `bsl::true_type`
    /// tag indicates that `TYPE` is a function type; see {Function Pointers
    /// and `void *`}.
    template <class TYPE>
    static void *voidify(TYPE *address, bsl::true_type) BSLS_KEYWORD_NOEXCEPT;
#endif
};

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

                          // ------------------
                          // struct PointerUtil
                          // ------------------

// CLASS METHODS
#ifdef BSLS_PLATFORM_CMP_SUN
template <class TYPE>
inline
TYPE *PointerUtil::unqualify(const volatile TYPE *address)
{
    return unqualify(address, bsl::is_function<TYPE>());
}

template <class TYPE>
inline
TYPE *PointerUtil::unqualify(const volatile TYPE *address, bsl::false_type)
{
    return const_cast<TYPE *>(address);
}

template <class TYPE>
inline
TYPE *PointerUtil::unqualify(const volatile TYPE *address, bsl::true_type)
{
    return address;
}
#else
template <class TYPE>
inline
BSLS_KEYWORD_CONSTEXPR TYPE *PointerUtil::unqualify(TYPE *address)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return address;
}

template <class TYPE>
inline
BSLS_KEYWORD_CONSTEXPR TYPE *PointerUtil::unqualify(const TYPE *address)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return const_cast<TYPE *>(address);
}

template <class TYPE>
inline
BSLS_KEYWORD_CONSTEXPR TYPE *PointerUtil::unqualify(volatile TYPE *address)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return const_cast<TYPE *>(address);
}

template <class TYPE>
inline
BSLS_KEYWORD_CONSTEXPR TYPE *PointerUtil::unqualify(
                                             const volatile TYPE *address)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return const_cast<TYPE *>(address);
}
#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP17
template <class TYPE>
inline
constexpr void *PointerUtil::voidify(TYPE *address) noexcept
{
    if constexpr (bsl::is_function_v<TYPE>) {
        // See {Function Pointers and 'void *'}.

        return reinterpret_cast<void *>(address);                     // RETURN
    }
    else {
        return unqualify(address);                                    // RETURN
    }
}
#else
template <class TYPE>
inline
BSLS_KEYWORD_CONSTEXPR void *PointerUtil::voidify(TYPE *address)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return voidify(address, bsl::is_function<TYPE>());
}

// PRIVATE CLASS METHODS
template <class TYPE>
inline
BSLS_KEYWORD_CONSTEXPR void *PointerUtil::voidify(TYPE           *address,
                                                  bsl::false_type)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return unqualify(address);
}

template <class TYPE>
inline
void *PointerUtil::voidify(TYPE *address, bsl::true_type) BSLS_KEYWORD_NOEXCEPT
{
    // See {Function Pointers and 'void *'}.

    return reinterpret_cast<void *>(address);
}
#endif

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2026 Bloomberg Finance L.P.
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
