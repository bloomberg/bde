// bslmf_integralconstant.h                                           -*-C++-*-
#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#define INCLUDED_BSLMF_INTEGRALCONSTANT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a mapping from integral constants to unique types.
//
//@CLASSES:
// bsl::integral_constant: A type representing a specific integer value
//     bsl::bool_constant: An alias template for `integral_constant<bool>`
//        bsl::false_type: `typedef` for `integral_constant<bool, false>`
//         bsl::true_type: `typedef` for `integral_constant<bool, true>`
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component describes a simple class template,
// `bsl::integral_constant`, that is used to map an integer constant to a C++
// type.  `integral_constant<t_TYPE, t_VALUE>` generates a unique type for each
// distinct compile-time integral `t_TYPE` and constant integer `t_VALUE`
// parameter.  That is, instantiations with different integer types and values
// form distinct types, so that `integral_constant<int, 0>` is a different type
// from `integral_constant<int, 1>`, which is also distinct from
// `integral_constant<unsigned, 1>`, and so on.  This mapping of integer values
// to types allows for "overloading by value", i.e., multiple functions with
// the same name can be overloaded on the "value" of an `integral_constant`
// argument, provided that the value is known at compile-time.  The typedefs
// `bsl::true_type` and `bsl::false_type` map the predicate values `true` and
// `false` to C++ types that are frequently useful for compile-time algorithms.
//
///Usage
///-----
// This section illustrates intended usage of this component
//
///Example 1: Compile-Time Function Dispatching
/// - - - - - - - - - - - - - - - - - - - - - -
// The most common use of this structure is to perform compile-time function
// dispatching based on a compile-time calculation.  Often the calculation is
// nothing more than a simple predicate, allowing us to select one of two
// functions based on whether the predicate holds.  The following function,
// `doSomething`, uses a fast implementation (e.g., using `memcpy`) if the
// parameterized type allows for such operations, otherwise it will use a more
// generic and slower implementation (e.g., using the copy constructor).  This
// example uses the types `true_type` and `false_type`, which are simple
// typedefs for `integral_constant<bool, true>` and
// `integral_constant<bool, false>`, respectively.
// ```
//   #include <bslmf_integralconstant.h>
//
//   template <class t_T>
//   int doSomethingImp(t_T *t, bsl::true_type)
//   {
//       // slow, generic implementation
//       // ...
//       (void) t;
//       return 11;
//   }
//
//   template <class t_T>
//   int doSomethingImp(t_T *t, bsl::false_type)
//   {
//       // fast implementation that works only for some types of 't_T'
//       // ...
//       (void) t;
//       return 55;
//   }
//
//   template <bool IsSlow, class t_T>
//   int doSomething(t_T *t)
//   {
//       // Dispatch to an implementation depending on the (compile-time)
//       // value of 'IsSlow'.
//       return doSomethingImp(t, bsl::integral_constant<bool, IsSlow>());
//   }
// ```
// For some parameter types, the fast version of `doSomethingImp` is not
// legal.  The power of this approach is that the compiler will not attempt
// semantic analysis on the implementation that does not match the appropriate
// `integral_constant` argument.
// ```
//   int main()
//   {
//       int r;
//
//       int i;
//       r = doSomething<false>(&i);   // select fast version for int
//       assert(55 == r);
//
//       double m;
//       r = doSomething<true>(&m); // select slow version for double
//       assert(11 == r);
//
//       return 0;
//   }
// ```
//
///Example 2: Base Class For Metafunctions
///- - - - - - - - - - - - - - - - - - - -
// Hard-coding the value of an `integral_constant` is not especially useful.
// Rather, `integral_constant` is typically used as the base class for
// "metafunction" classes, classes that yield the value of compile-time
// properties, including properties that are associated with types, rather
// than with values.  For example, the following metafunction can be used at
// compile time to determine whether a type is a floating point type:
// ```
//   template <class t_TYPE> struct IsFloatingPoint  : bsl::false_type { };
//   template <> struct IsFloatingPoint<float>       : bsl::true_type { };
//   template <> struct IsFloatingPoint<double>      : bsl::true_type { };
//   template <> struct IsFloatingPoint<long double> : bsl::true_type { };
// ```
// The value `IsFloatingPoint<int>::value` is false and
// `IsFloatingPoint<double>::value` is true.  The `integral_constant` base
// class has a member type, `type`, that refers to itself and is inherited by
// `IsFloatingPoint`.  Thus `IsFloatingPoint<float>::type` is `true_type` and
// `IsFloatingPoint<char>::type` is `false_type`.  `IsFloatingPoint` is an a
// member of a common category of metafunctions known as "type traits" because
// they express certain properties (traits) of a type.  Using this
// metafunction, we can rewrite the `doSomething` function from first example
// so that it does not require the user to specify the `IsSlow` template
// argument:
// ```
//   template <class t_T>
//   int doSomething2(t_T *t)
//   {
//       // Automatically detect whether to use slow or fast imp.
//       const bool isSlow = IsFloatingPoint<t_T>::value;
//       return doSomethingImp(t, bsl::integral_constant<bool, isSlow>());
//   }
//
//   int main()
//   {
//       int r;
//
//       int i;
//       r = doSomething2(&i); // select fast version for int
//       assert(55 == r);
//
//       double m;
//       r = doSomething2(&m); // select slow version for double
//       assert(11 == r);
//
//       return 0;
//   }
// ```

#include <bslscm_version.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>

#ifdef BSLS_COMPILERFEATURES_FULL_CPP11
# include <type_traits>
#endif // BSLS_COMPILERFEATURES_FULL_CPP11

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
# include <bsls_libraryfeatures.h>
# include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifdef BSLS_COMPILERFEATURES_FULL_CPP11
namespace bsl {

                        // ================================
                        // class template integral_constant
                        // ================================

template <class t_TYPE, t_TYPE t_VALUE>
struct integral_constant : ::std::integral_constant<t_TYPE, t_VALUE> {
    // PUBLIC TYPES
    using type = integral_constant;

  public:
    // CREATORS

    //! integral_constant() = default;
    //! integral_constant(const integral_constant&) = default;
    //! integral_constant operator=(const integral_constant&) = default;
    //! ~integral_constant() = default;

    // ACCESSORS

    /// Return a copy of the template argument `t_VALUE`.
    constexpr t_TYPE operator()() const noexcept;
};

                        // ============================
                        // alias template bool_constant
                        // ============================

template <bool t_VALUE>
using bool_constant = integral_constant<bool, t_VALUE>;

                        // ===============
                        // type false_type
                        // ===============

using false_type = bool_constant<false>;

                        // ===============
                        // type true_type
                        // ===============

using true_type = bool_constant<true>;

}  // close namespace bsl
#else   // BSLS_COMPILERFEATURES_FULL_CPP11
namespace bsl {

                        // ================================
                        // class template integral_constant
                        // ================================

/// Generate a unique type for the given `t_TYPE` and `t_VALUE`.  This
/// `struct` is used for compile-time dispatch of overloaded functions and
/// as the base class for many metafunctions.
template <class t_TYPE, t_TYPE t_VALUE>
struct integral_constant {
    // PUBLIC TYPES
    typedef t_TYPE            value_type;
    typedef integral_constant type;

    // PUBLIC CLASS DATA
    static const t_TYPE value = t_VALUE;

  public:
    // CREATORS

    //! integral_constant() = default;
    //! integral_constant(const integral_constant&) = default;
    //! integral_constant operator=(const integral_constant&) = default;
    //! ~integral_constant() = default;

    // ACCESSORS

    /// Return a copy of the template argument `t_VALUE`.
    operator value_type() const;

    /// Return a copy of the template argument `t_VALUE`.
    value_type operator()() const;
};

                        // ===============
                        // type false_type
                        // ===============

typedef integral_constant<bool, false> false_type;

                        // ===============
                        // type true_type
                        // ===============

typedef integral_constant<bool, true> true_type;

}  // close namespace bsl
#endif // ! defined(BSLS_COMPILERFEATURES_FULL_CPP11)

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

// ACCESSORS
#ifdef BSLS_COMPILERFEATURES_FULL_CPP11
template <class t_TYPE, t_TYPE t_VALUE>
inline constexpr
t_TYPE bsl::integral_constant<t_TYPE, t_VALUE>::operator()() const noexcept
{
    return t_VALUE;
}

#else   // BSLS_COMPILERFEATURES_FULL_CPP11
template <class t_TYPE, t_TYPE t_VALUE>
inline
t_TYPE bsl::integral_constant<t_TYPE, t_VALUE>::operator()() const
{
    return t_VALUE;
}

template <class t_TYPE, t_TYPE t_VALUE>
inline
bsl::integral_constant<t_TYPE, t_VALUE>::operator t_TYPE() const
{
    return t_VALUE;
}

// STATIC MEMBER VARIABLE DEFINITIONS
template <class t_TYPE, t_TYPE t_VALUE>
const t_TYPE bsl::integral_constant<t_TYPE, t_VALUE>::value;
#endif // ! defined(BSLS_COMPILERFEATURES_FULL_CPP11)

#endif // ! defined(INCLUDED_BSLMF_INTEGRALCONSTANT)

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
