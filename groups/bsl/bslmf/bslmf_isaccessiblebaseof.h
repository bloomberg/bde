// bslmf_isaccessiblebaseof.h                                         -*-C++-*-
#ifndef INCLUDED_BSLMF_ISACCESSIBLEBASEOF
#define INCLUDED_BSLMF_ISACCESSIBLEBASEOF

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for derived classes.
//
//@CLASSES:
//  bslmf::IsAccessibleBaseOf: metafunction for detecting derived classes
//
//@DESCRIPTION: This component provides a metafunction,
// `bslmf::IsAccessibleBaseOf`, that determines whether one class is an
// unambiguous public base class of another class.
// `bslmf::IsAccessibleBaseOf::value` is `true` if the template parameter
// `t_BASE` is the template parameter `t_DERIVED` itself or an unambiguous
// public base class of `t_DERIVED`, ignoring cv-qualifiers, and `false`
// otherwise.  In particular `value` is `false` if either type is not a class,
// for example an enumeration, a reference, a pointer, or `void`.  Note that,
// as for `bsl::is_class`, a union is treated as a class, so unlike
// `bsl::is_base_of`, `value` is `true` if `t_BASE` and `t_DERIVED` are the
// same union type ignoring cv-qualifiers.  The result does not depend on the
// context in which it is evaluated.  Without full C++11 support a private,
// protected, or ambiguous base causes a compilation error instead of
// producing `false`.  An incomplete `t_DERIVED` that is not the same type as
// `t_BASE` ignoring cv-qualifiers produces `false`.  Note that this result can
// differ from the result once `t_DERIVED` is complete, so evaluating
// `bslmf::IsAccessibleBaseOf` for the same types both before and after
// `t_DERIVED` is defined risks violating the One Definition Rule.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Base And Derived Classes
///- - - - - - - - - - - - - - - - - -
// Define a base class, a class publicly derived from it, and a class privately
// derived from it.
// ```
// struct Base {};
// struct Derived : Base {};
// struct PrivatelyDerived : private Base {};
// ```
// Evaluate `bslmf::IsAccessibleBaseOf::value`.  Note that `Base` is a private
// base class of `PrivatelyDerived`, so `std::is_base_of` would evaluate `true`
// but `bslmf::IsAccessibleBaseOf` evaluates `false` unless building without
// full C++11 support, in which case a private base would cause a compilation
// error instead.
// ```
// void example1()
// {
//     assert((true  == bslmf::IsAccessibleBaseOf<Base,    Derived>::value));
//     assert((true  == bslmf::IsAccessibleBaseOf<Base,    Base>   ::value));
//     assert((true  == bslmf::IsAccessibleBaseOf<Derived, Derived>::value));
//     assert((false == bslmf::IsAccessibleBaseOf<Derived, Base>   ::value));
// #if BSLS_COMPILERFEATURES_FULL_CPP11
//     assert((false ==
//             bslmf::IsAccessibleBaseOf<Base, PrivatelyDerived>::value));
// #endif
// }
// ```
///Example 2: Unrelated Classes
/// - - - - - - - - - - - - - -
// Define two classes that are not related to each other.
// ```
// class Unrelated  {};
// class Unrelated2 {};
// ```
// Evaluate `bslmf::IsAccessibleBaseOf::value`.
// ```
// void example2()
// {
//     assert((false ==
//             bslmf::IsAccessibleBaseOf<Unrelated, Unrelated2>::value));
// }
// ```

#include <bslscm_version.h>

#include <bslmf_integralconstant.h>
#include <bslmf_isclass.h>

#include <bsls_compilerfeatures.h>
#include <bsls_platform.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
# include <bslmf_isconvertible.h>
#endif  // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace BloombergLP {
namespace bslmf {

                      // ===============================
                      // struct IsAccessibleBaseOf_Probe
                      // ===============================

/// This component-private utility `struct` provides an overload set, declared
/// but not defined, whose `check<t_BASE, t_DERIVED>` call returns `YesType` if
/// a pointer to `t_DERIVED` implicitly converts to a pointer to `t_BASE`, and
/// `NoType` otherwise.  Without full C++11 support an inaccessible or
/// ambiguous base is a compilation error instead.
struct IsAccessibleBaseOf_Probe {

    // TYPES

    /// A type whose size differs from that of `NoType`.
    typedef char YesType;

    /// A type whose size differs from that of `YesType`.
    struct NoType {
        char d_padding[2];
    };

    // CLASS METHODS
#if BSLS_COMPILERFEATURES_FULL_CPP11
    /// Accept an argument implicitly convertible to the specified `t_TYPE`.
    /// Note that a call with any other argument is ill-formed.
    template <class t_TYPE>
    static void implicitCast(t_TYPE);

    /// Return `YesType` if a pointer to the specified `t_DERIVED` implicitly
    /// converts to a pointer to the specified `t_BASE`.  An inaccessible or
    /// ambiguous base is a substitution failure that removes this overload.
    template <class t_BASE,
              class t_DERIVED,
              class = decltype(implicitCast<t_BASE *>(
                                         static_cast<t_DERIVED *>(nullptr)))>
    static YesType check(t_DERIVED *);
#else
    /// Return `YesType` for an argument implicitly convertible to a pointer to
    /// the specified `t_BASE`.  Note that the specified `t_DERIVED` is unused
    /// and keeps the call the same in all language modes, and that an
    /// inaccessible or ambiguous base is a compilation error here.
    template <class t_BASE, class t_DERIVED>
    static YesType check(t_BASE *);
#endif  // BSLS_COMPILERFEATURES_FULL_CPP11

    /// Return `NoType` for any argument.  Note that the specified `t_BASE`
    /// and `t_DERIVED` are unused.
    template <class t_BASE, class t_DERIVED>
    static NoType check(...);
};

                         // =========================
                         // struct IsAccessibleBaseOf
                         // =========================

/// This `struct` template implements a metafunction that derives from
/// `bsl::true_type` if the specified `t_BASE` is the specified `t_DERIVED`
/// itself or an unambiguous public base class of `t_DERIVED`, ignoring
/// cv-qualifiers, and from `bsl::false_type` otherwise.  In particular the
/// result is `bsl::false_type` if either type is not a class type.  Note that
/// unions are class types.  The result does not depend on the context of use,
/// so a private or protected base yields `bsl::false_type` even where that
/// base is accessible.  Without full C++11 support a private, protected, or
/// ambiguous base causes a compilation error instead.  An incomplete
/// `t_DERIVED` that is not the same type as `t_BASE` ignoring cv-qualifiers
/// yields `bsl::false_type`.  Note that this result can differ from the result
/// once `t_DERIVED` is complete, which risks violating the One Definition
/// Rule.  Also note that any template parameter beyond `t_DERIVED` is an
/// implementation detail that clients must not supply.
#if !defined(BSLS_PLATFORM_CMP_SUN)
template <class t_BASE,
          class t_DERIVED,
          bool  k_ARE_CLASSES = bsl::is_class<t_BASE>::value &&
                                bsl::is_class<t_DERIVED>::value>
struct IsAccessibleBaseOf : bsl::false_type {
};

/// This partial specialization optimizes the case where `t_BASE` and
/// `t_DERIVED` are the same class type, which need not be complete.  Note that
/// this avoids the SFINAE probe unless the two types differ only in
/// cv-qualification.
template <class t_TYPE>
struct IsAccessibleBaseOf<t_TYPE, t_TYPE, true> : bsl::true_type {
};

/// This partial specialization handles the remaining cases where `t_BASE` and
/// `t_DERIVED` are class types.
template <class t_BASE, class t_DERIVED>
struct IsAccessibleBaseOf<t_BASE, t_DERIVED, true>
: bsl::integral_constant<
      bool,
      sizeof(IsAccessibleBaseOf_Probe::YesType) ==
          sizeof(IsAccessibleBaseOf_Probe::check<const volatile t_BASE,
                                                 const volatile t_DERIVED>(
                                  static_cast<const volatile t_DERIVED *>(0)))
      > {
};
#else
// Sun CC fails to match an out-of-line member template definition to its
// declaration when the signature names a template having a dependent default
// template argument, so the class-type dispatch moves to a separate template.

/// This component-private `struct` template derives from `bsl::false_type`,
/// and handles the case where the specified `k_ARE_CLASSES` is `false`, i.e.,
/// where the specified `t_BASE` or `t_DERIVED` is not a class type.
template <class t_BASE, class t_DERIVED, bool k_ARE_CLASSES>
struct IsAccessibleBaseOf_Imp : bsl::false_type {
};

/// This partial specialization derives from `bsl::true_type`, and optimizes
/// the case where `t_BASE` and `t_DERIVED` are the same class type, `t_TYPE`,
/// which need not be complete.
template <class t_TYPE>
struct IsAccessibleBaseOf_Imp<t_TYPE, t_TYPE, true> : bsl::true_type {
};

/// This partial specialization handles the remaining cases where `t_BASE` and
/// `t_DERIVED` are class types.
template <class t_BASE, class t_DERIVED>
struct IsAccessibleBaseOf_Imp<t_BASE, t_DERIVED, true>
: bsl::integral_constant<
      bool,
      sizeof(IsAccessibleBaseOf_Probe::YesType) ==
          sizeof(IsAccessibleBaseOf_Probe::check<const volatile t_BASE,
                                                 const volatile t_DERIVED>(
                                  static_cast<const volatile t_DERIVED *>(0)))
      > {
};

/// This `struct` template provides, for Sun CC, the `IsAccessibleBaseOf`
/// metafunction documented above.
template <class t_BASE, class t_DERIVED>
struct IsAccessibleBaseOf
: IsAccessibleBaseOf_Imp<t_BASE,
                         t_DERIVED,
                         bsl::is_class<t_BASE>::value &&
                             bsl::is_class<t_DERIVED>::value> {
};
#endif  // !defined(BSLS_PLATFORM_CMP_SUN)

}  // close package namespace
}  // close enterprise namespace

#endif  // INCLUDED_BSLMF_ISACCESSIBLEBASEOF

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
