// bdlf_overloaded.h                                                  -*-C++-*-
#ifndef INCLUDED_BDLF_OVERLOADED
#define INCLUDED_BDLF_OVERLOADED

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a type for constructing overload sets.
//
//@CLASSES:
//  Overloaded: template for overload sets
//
//@SEE_ALSO: bslstl_variant, bdljsn_json
//
//@DESCRIPTION: This component provides a template class, `bdlf::Overloaded`,
// that allows you to construct callable objects that contain several function
// call overloads.
//
// This is especially useful when dealing with variant objects, as they can be
// passed to `std::visit` or to `bsl::visit`, and depending on the type stored
// in the variant, the correct element of the overload set will be called.
//
// This component requires C++17.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// First, create a bsl::variant object that can contain several different
// types.
// ```
// bsl::variant<unsigned, double, bsl::string> v;
// ```
// Next, Create an Overload object containing several options:
// ```
// bdlf::Overloaded over{
//       [] (unsigned)           {return 1;}
//     , [] (double)             {return 2;}
//     , [] (const bsl::string&) {return 3;}
//     };
// ```
// Set the value of variant, and then call std::visit, passing the overload set
// and the variant.  Check the return value to see that the right lambda was
// called.
// ```
// v = 2U;
// assert(1 == bsl::visit(over, v));
// v = 2.0;
// assert(2 == bsl::visit(over, v));
// v = bsl::string("2.0");
// assert(3 == bsl::visit(over, v));
// ```

#include <bslscm_version.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>

#include <utility>   // std::forward

namespace BloombergLP {
namespace bdlf {

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD

                       // ================================
                       // class Overloaded_FunctionPointer
                       // ================================

/// Overloaded_FunctionPointer wraps a function pointer.  It is implicitly
/// constructible from the pointer, and provides an `operator()` method that
/// calls through to the underlying function.
template <class RET, class ...ARGS>
struct Overloaded_FunctionPointer
{
    // TYPES
    using FpType = RET (*)(ARGS...);

    // PUBLIC DATA
    FpType d_fp;

    // CREATORS

    /// Construct this object from the specified `f`.
    Overloaded_FunctionPointer(FpType f);                           // IMPLICIT

    // ACCESSORS

    /// Call the function passed to the constructor with the specified `args`.
    RET operator()(ARGS&&... args) const;
};

                   // ========================================
                   // class Overloaded_NoexceptFunctionPointer
                   // ========================================

/// Overloaded_NoexceptFunctionPointer wraps a function pointer.  It is
/// implicitly constructible from the pointer, and provides an `operator()`
/// method that calls through to the underlying function.
template <class RET, class ...ARGS>
struct Overloaded_NoexceptFunctionPointer
{

    // TYPES
    using FpType = RET (*)(ARGS...) noexcept;

    // PUBLIC DATA
    FpType d_fp;

    // CREATORS

    /// Construct this object from the specified `f`.
    Overloaded_NoexceptFunctionPointer(FpType f);                   // IMPLICIT

    // ACCESSORS

    /// Call the function passed to the constructor with the specified `args`.
    RET operator()(ARGS&&... args) const noexcept;
};

                   // ======================================
                   // class Overloaded_MemberFunctionPointer
                   // ======================================

/// The class Overloaded_MemberFunctionPointer wraps a member function
/// pointer.  It is implicitly constructible from the pointer, and provides
/// an `operator()` that takes an object of the appropriate type and other
/// arguments, and forwards them on to the underlying function.
template <class OBJ, class RET, class ...ARGS>
struct Overloaded_MemberFunctionPointer
{
    // TYPES
    using FpType = RET (OBJ::*)(ARGS...);

    // PUBLIC DATA
    FpType d_fp;

    // CREATORS

    /// Construct this object from the specified `f`.
    Overloaded_MemberFunctionPointer(FpType f);                     // IMPLICIT

    // ACCESSORS

    /// Call the member function passed to the constructor using the specified
    /// `obj` with the specified `args`, and return the result of the call.
    RET operator()(OBJ * obj, ARGS&&... args) const;
};

                 // ===========================================
                 // class Overloaded_ConstMemberFunctionPointer
                 // ===========================================

/// The class Overloaded_ConstMemberFunctionPointer wraps a member function
/// pointer.  It is implicitly constructible from the pointer, and provides
/// an `operator()` that takes an object of the appropriate type and other
/// arguments, and forwards them on to the underlying function.
template <class OBJ, class RET, class ...ARGS>
struct Overloaded_ConstMemberFunctionPointer
{
    // TYPES
    using FpType = RET (OBJ::*)(ARGS...) const;

    // PUBLIC DATA
    FpType d_fp;

    // CREATORS

    /// Construct this object from the specified `f`.
    Overloaded_ConstMemberFunctionPointer(FpType f);                // IMPLICIT

    // ACCESSORS

    /// Call the member function passed to the constructor using the specified
    /// `obj` with the specified `args`, and return the result of the call.
    RET operator()(const OBJ * obj, ARGS&&... args) const;
};

                // ==============================================
                // class Overloaded_NoexceptMemberFunctionPointer
                // ==============================================

/// The class Overloaded_NoexceptMemberFunctionPointer wraps a member function
/// pointer.  It is implicitly constructible from the pointer, and provides an
/// `operator()` that takes an object of the appropriate type and other
/// arguments, and forwards them on to the underlying function.
template <class OBJ, class RET, class ...ARGS>
struct Overloaded_NoexceptMemberFunctionPointer
{
    // TYPES
    using FpType = RET (OBJ::*)(ARGS...) noexcept;

    // PUBLIC DATA
    FpType d_fp;

    // CREATORS

    /// Construct this object from the specified `f`.
    Overloaded_NoexceptMemberFunctionPointer(FpType f);             // IMPLICIT

    // ACCESSORS

    /// Call the member function passed to the constructor using the specified
    /// `obj` with the specified `args`, and return the result of the call.
    RET operator()(OBJ * obj, ARGS&&... args) const noexcept;
};

             // ===================================================
             // class Overloaded_ConstNoexceptMemberFunctionPointer
             // ===================================================

/// The class Overloaded_ConstNoexceptMemberFunctionPointer wraps a member
/// function pointer.  It is implicitly constructible from the pointer, and
/// provides an `operator()` that takes an object of the appropriate type
/// and other arguments, and forwards them on to the underlying function.
template <class OBJ, class RET, class ...ARGS>
struct Overloaded_ConstNoexceptMemberFunctionPointer
{
    // TYPES
    using FpType = RET (OBJ::*)(ARGS...) const noexcept;

    // PUBLIC DATA
    FpType d_fp;

    // CREATORS

    /// Construct this object from the specified `f`.
    Overloaded_ConstNoexceptMemberFunctionPointer(FpType f);        // IMPLICIT

    // ACCESSORS

    /// Call the member function passed to the constructor using the specified
    /// `obj` with the specified `args`, and return the result of the call.
    RET operator()(const OBJ * obj, ARGS&&... args) const noexcept;

};

// Implementation Notes
// --------------------
// Given a callable, figure out what the appropriate wrapper is.  For class
// types, (lambdas, say) there is no wrapper; the class can be used directly.
// For function pointers, we use Overloaded_FunctionPointer or
// Overloaded_NoexceptFunctionPointer. For member functions, we have four
// varieties.

                         // ========================
                         // class Overloaded_Wrapper
                         // ========================

template <class TYPE>
struct Overloaded_Wrapper {
    // TYPES
    using Type = TYPE;
};

// -- Wrappers for function pointers
template <class RET, class ...ARGS>
struct Overloaded_Wrapper<RET (*)(ARGS...)> {
    // TYPES
    using Type = Overloaded_FunctionPointer<RET, ARGS...>;
};

template <class RET, class ...ARGS>
struct Overloaded_Wrapper<RET (*)(ARGS...) noexcept> {
    // TYPES
    using Type = Overloaded_NoexceptFunctionPointer<RET, ARGS...>;
};

// -- Wrappers for member function pointers
template <class OBJ, class RET, class ...ARGS>
struct Overloaded_Wrapper<RET (OBJ::*)(ARGS...)> {
    // TYPES
    using Type = Overloaded_MemberFunctionPointer<OBJ, RET, ARGS...>;
};

template <class OBJ, class RET, class ...ARGS>
struct Overloaded_Wrapper<RET (OBJ::*)(ARGS...) const> {
    // TYPES
    using Type = Overloaded_ConstMemberFunctionPointer<OBJ, RET, ARGS...>;
};

template <class OBJ, class RET, class ...ARGS>
struct Overloaded_Wrapper<RET (OBJ::*)(ARGS...) noexcept> {
    // TYPES
    using Type = Overloaded_NoexceptMemberFunctionPointer<OBJ, RET, ARGS...>;
};

template <class OBJ, class RET, class ...ARGS>
struct Overloaded_Wrapper<RET (OBJ::*)(ARGS...) const noexcept> {
    // TYPES
    using Type =
              Overloaded_ConstNoexceptMemberFunctionPointer<OBJ, RET, ARGS...>;
};

                                // ================
                                // class Overloaded
                                // ================

// `Overloaded` is a structure that holds a series of objects that have
// an `operator()`, and provides the ability to call one of them determined
// by the parameters that are passed to the operator.
template<class... TS>
struct Overloaded : Overloaded_Wrapper<TS>::Type...
{
    using Overloaded_Wrapper<TS>::Type::operator()...;
};

// CLASS TEMPLATE DEDUCTION GUIDES
template<class... TS>
Overloaded(TS...) -> Overloaded<TS...>;

// ============================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

                           // --------------------------------
                           // class Overloaded_FunctionPointer
                           // --------------------------------

// ACCESSORS
template <class RET, class ...ARGS>
Overloaded_FunctionPointer<RET, ARGS...>::Overloaded_FunctionPointer(
                   typename Overloaded_FunctionPointer<RET, ARGS...>::FpType f)
: d_fp(f)
{
}

template <class RET, class ...ARGS>
inline
RET Overloaded_FunctionPointer<RET, ARGS...>::operator()(ARGS&&... args) const
{
    return d_fp(std::forward<ARGS>(args)...);
}

                       // ----------------------------------------
                       // class Overloaded_NoexceptFunctionPointer
                       // ----------------------------------------

// ACCESSORS
template <class RET, class ...ARGS>
Overloaded_NoexceptFunctionPointer<RET, ARGS...>
    ::Overloaded_NoexceptFunctionPointer(
           typename Overloaded_NoexceptFunctionPointer<RET, ARGS...>::FpType f)
: d_fp(f)
{
}

template <class RET, class ...ARGS>
inline
RET Overloaded_NoexceptFunctionPointer<RET, ARGS...>::operator()
                                                (ARGS&&... args) const noexcept
{
    return d_fp(std::forward<ARGS>(args)...);
}

                       // --------------------------------------
                       // class Overloaded_MemberFunctionPointer
                       // --------------------------------------

// ACCESSORS
template <class OBJ, class RET, class ...ARGS>
Overloaded_MemberFunctionPointer<OBJ, RET, ARGS...>
    ::Overloaded_MemberFunctionPointer(
        typename Overloaded_MemberFunctionPointer<OBJ, RET, ARGS...>::FpType f)
: d_fp(f)
{
}

template <class OBJ, class RET, class ...ARGS>
inline
RET Overloaded_MemberFunctionPointer<OBJ, RET, ARGS...>::operator()
                                              (OBJ * obj, ARGS&&... args) const
{
    return (obj->*d_fp)(std::forward<ARGS>(args)...);
}

                     // -------------------------------------------
                     // class Overloaded_ConstMemberFunctionPointer
                     // -------------------------------------------

// ACCESSORS
template <class OBJ, class RET, class ...ARGS>
Overloaded_ConstMemberFunctionPointer<OBJ, RET, ARGS...>
    ::Overloaded_ConstMemberFunctionPointer(typename
            Overloaded_ConstMemberFunctionPointer<OBJ, RET, ARGS...>::FpType f)
: d_fp(f)
{
}

template <class OBJ, class RET, class ...ARGS>
inline
RET Overloaded_ConstMemberFunctionPointer<OBJ, RET, ARGS...>::operator()
                                        (const OBJ * obj, ARGS&&... args) const
{
    return (obj->*d_fp)(std::forward<ARGS>(args)...);
}

                    // ----------------------------------------------
                    // class Overloaded_NoexceptMemberFunctionPointer
                    // ----------------------------------------------

// ACCESSORS
template <class OBJ, class RET, class ...ARGS>
Overloaded_NoexceptMemberFunctionPointer<OBJ, RET, ARGS...>
    ::Overloaded_NoexceptMemberFunctionPointer(typename
         Overloaded_NoexceptMemberFunctionPointer<OBJ, RET, ARGS...>::FpType f)
: d_fp(f)
{
}

template <class OBJ, class RET, class ...ARGS>
inline
RET Overloaded_NoexceptMemberFunctionPointer<OBJ, RET, ARGS...>
                         ::operator()(OBJ * obj, ARGS&&... args) const noexcept
{
    return (obj->*d_fp)(std::forward<ARGS>(args)...);
}

                 // ---------------------------------------------------
                 // class Overloaded_ConstNoexceptMemberFunctionPointer
                 // ---------------------------------------------------

// ACCESSORS
template <class OBJ, class RET, class ...ARGS>
Overloaded_ConstNoexceptMemberFunctionPointer<OBJ, RET, ARGS...>
    ::Overloaded_ConstNoexceptMemberFunctionPointer(typename
    Overloaded_ConstNoexceptMemberFunctionPointer<OBJ, RET, ARGS...>::FpType f)
: d_fp(f)
{
}

template <class OBJ, class RET, class ...ARGS>
inline
RET Overloaded_ConstNoexceptMemberFunctionPointer<OBJ, RET, ARGS...>
                   ::operator()(const OBJ * obj, ARGS&&... args) const noexcept
{
    return (obj->*d_fp)(std::forward<ARGS>(args)...);
}

#endif

}  // close package namespace
}  // close enterprise namespace

#endif // ! defined(INCLUDED_BDLF_OVERLOADED)

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
