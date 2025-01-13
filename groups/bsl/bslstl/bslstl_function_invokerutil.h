// bslstl_function_invokerutil.h                                      -*-C++-*-
#ifndef INCLUDED_BSLSTL_FUNCTION_INVOKERUTIL
#define INCLUDED_BSLSTL_FUNCTION_INVOKERUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide invoker adaptors for `bsl::function`
//
//@CLASSES:
//  Function_InvokerUtil: Utility for returning an appropriate invoker function
//  Function_InvokerUtilNullCheck: Customization point to detect null invocable
//
//@MACROS
//  BSLSTL_FUNCTION_INVOKERUTIL_SUPPORT_IS_FUNC_INVOCABLE: defined if supported
//
//@SEE_ALSO: bslstl_function
//
//@DESCRIPTION: This component provides a struct, `Function_InvokerUtil`,
// containing a function template, `invokerForFunc`, that returns a pointer to
// a function that is used to invoke a callable object of a particular type.
// The returned type is custom-generated for each specific target type.  This
// component is for private use only.
//
// The client of this component, `bsl::function`, is a complex class that is
// templated in two ways:
//
// 1. The class itself has a template parameter representing the prototype
//    (argument and return types) of its call operator.  E.g., type
//    `bsl::function<int(char*)>` has member `int operator()(char*);`.
// 2. Several of its constructors take an argument of template parameter
//    callable type and wrap it, using type erasure, so that the type of the
//    wrapped target is not part of the type of the `bsl::function`.
//
// Only the invocation mechanism needs both kinds of template parameters; other
// parts of `bsl::function` (e.g., `bslstl::Function_Rep`) concern themselves
// with only the callable object type, not with the prototype of the call
// operator.  This component factors out most of that complexity so as to
// minimise code size, especially when using the `sim_cpp11_features.pl` script
// to simulate C++11 variadic templates in C++03.
//
// The classes in this component are stateless and contain only static
// functions.
//
///Macros
///------
// If this component supports checking whether an object of an arbitrary
// non-cvref-qualified type is invocable under a particular prototype, the
// `BSLSTL_FUNCTION_INVOKERUTIL_SUPPORT_IS_FUNC_INVOCABLE` macro is defined,
// and it is not defined otherwise.
//
///Return value of `Function_InvokerUtil::invokerForFunc`
///------------------------------------------------------
// The function pointer, `invoker_p`, returned by
// `Function_InvokerUtil::invokerForFunc<RET(ARG0, ARG1, ...)>` takes an
// argument of type `Function_Rep *` and zero or more argument types `ARG0`,
// `ARG1`, ..., as specified by the template argument.  Calling
// `invoker_p(rep, arg0, arg1, ...)` gets the target, `f` from `rep` and
// invokes it with the supplied arguments.  Invocation of `f` follows the
// definition of *INVOKE* in section [func.require] of the C++ standard.  These
// rules are summarized in the following table:
// ```
// +----------------------------+-----------------------+
// | Type of target object, 'f' | Invocation expression |
// +============================+=======================+
// | Pointer to function or     | f(arg0, arg1, ...)    |
// | functor class              |                       |
// +----------------------------+-----------------------+
// | Pointer to member function | (arg0X.*f)(arg1, ...) |
// +----------------------------+-----------------------+
// | pointer to data member     | arg0X.*f              |
// +----------------------------+-----------------------+
// ```
// The arguments to `f` must be implicitly convertible from the corresponding
// argument types `ARG0`, `ARG1`, ... and the return value of the call
// expression must be implicitly convertible to `RET`, unless `RET` is `void`.
//
// In the case of a pointer to member function, `R (T::*f)(...)`, or pointer to
// data member `R T::*f`, `arg0X` is one of the following:
//
// * `arg0` if `ARG0` is `T` or derived from `T`.
// * `arg0.get()` if `ARG0` is a specialization of `reference_wrapper`.
// * `(*arg0)` if `ARG0` is a pointer type or pointer-like type (e.g., a smart
//   pointer).
//
// Note that, consistent with the C++ Standard definition of *INVOKE*, we
// consider pointer-to-member-function and pointer-to-data-member types to be
// "callable" even though, strictly speaking, they lack an `operator()`.
//
///Customization point `Function_InvokerUtilNullCheck`
///---------------------------------------------------
// The class template, `Function_InvokerUtilNullCheck<T>` provides a single,
// `static` member function `isNull(const T& f)` that returns `true` iff `f`
// represents a "null value".  By default,
// `Function_InvokerUtilNullCheck<T>::isNull` returns `true` iff `T` is a
// pointer or pointer-to-member type and has a null value.  For non-pointer
// types, the primary template always returns `false`.  However, other
// components can provide specializations of `Function_InvokerUtilNullCheck`
// that add the notion of "nullness" to other invocable types.  In particular,
// `bslstl_function.h` specializes this template for `bsl::function` and
// `bdef_function.h` specializes this template for `bdef_function`.  In both
// cases, a function object is considered null if it is empty, i.e., it does
// not wrap an invocable object.  Although it is theoretically possible to
// specialize `Function_InvokerUtilNullCheck` for types not related to
// `bsl::function`, doing so would go outside of the behavior of the standard
// `std::function` type that `bsl::function` is emulating.  For this reason,
// `Function_InvokerUtilNullCheck` is tucked away in this private component for
// use only through explicit collaboration.

#include <bslscm_version.h>

#include <bslalg_nothrowmovableutil.h>

#include <bslmf_addrvaluereference.h>
#include <bslmf_conditional.h>
#include <bslmf_forwardingtype.h>
#include <bslmf_integralconstant.h>
#include <bslmf_invokeresult.h>
#include <bslmf_ismemberpointer.h>
#include <bslmf_isvoid.h>
#include <bslmf_memberfunctionpointertraits.h>
#include <bslmf_movableref.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>
#include <bsls_nullptr.h>

#include <bslstl_function_rep.h>
#include <bslstl_function_smallobjectoptimization.h>

#include <utility>

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// clang-format off
// Include version that can be compiled with C++03
// Generated on Mon Jan 13 08:31:39 2025
// Command line: sim_cpp11_features.pl bslstl_function_invokerutil.h

# define COMPILING_BSLSTL_FUNCTION_INVOKERUTIL_H
# include <bslstl_function_invokerutil_cpp03.h>
# undef COMPILING_BSLSTL_FUNCTION_INVOKERUTIL_H

// clang-format on
#else

#ifndef BSLS_PLATFORM_CMP_SUN
#define BSLSTL_FUNCTION_INVOKERUTIL_CAST_RESULT(RET, X) static_cast<RET>(X)
#else
#define BSLSTL_FUNCTION_INVOKERUTIL_CAST_RESULT(RET, X) (RET)(X)
#endif

#if defined(BSLMF_INVOKERESULT_SUPPORT_CPP17_SEMANTICS)      \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE)          \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
#define BSLSTL_FUNCTION_INVOKERUTIL_SUPPORT_IS_FUNC_INVOCABLE
#endif

namespace BloombergLP {
namespace bslstl {

#ifdef BSLSTL_FUNCTION_INVOKERUTIL_SUPPORT_IS_FUNC_INVOCABLE

/// forward declaration
template <class PROTOTYPE, class FUNC>
struct Function_InvokerUtil_IsFuncInvocable;

#endif

                        // ===========================
                        // struct Function_InvokerUtil
                        // ===========================

/// This struct is a namespace containing a single function template,
/// `invokerForFunc`, that returns a pointer to a function that is used to
/// invoke a callable object of a particular type.
struct Function_InvokerUtil {

    // TYPES
    enum {
        // Enumeration of the different types of callable objects.

        e_Null,
        e_FunctionPtr,
        e_MemFunctionPtr,
        e_MemDataPtr,
        e_InplaceFunctor,
        e_OutofplaceFunctor
    };

    /// Generic function pointer.  This type is as close as we can get to
    /// `void *` for function pointers.
    typedef Function_Rep::GenericInvoker GenericInvoker;

#ifdef BSLSTL_FUNCTION_INVOKERUTIL_SUPPORT_IS_FUNC_INVOCABLE
    /// This `struct` template implements a Boolean metafunction that
    /// publicly inherits from `bsl::true_type` if an object of the
    /// specified `FUNC` type is invocable under the specified `PROTOTYPE`,
    /// and publicly inherits from `bsl::false_type` otherwise.  An object
    /// of `FUNC` type is invocable under the `PROTOTYPE` if it is
    /// Lvalue-Callable with the arguments of the `PROTOTYPE` (as forwarded
    /// by the facilities of 'bslmf_forwardingtype), and returns an object
    /// of type explicitly convertible to the return type of the
    /// `PROTOTYPE`.  If the return type of the `PROTOTYPE` is `void`, then
    /// any type is considered explicitly convertible to the return type of
    /// the `PROTOTYPE`.  This `struct` template requires `PROTOTYPE` to be
    /// an unqualified function type.
    ///
    /// Note that `IsFuncInvocable` is qualitatively different than
    /// `std::is_invocable_r`, in that it makes concessions for supporting
    /// legacy behavior of `bsl::function`.
    /// `std::is_invocable_r<RET, FUNC, ARGS...>` requires that the return
    /// type of the invocation of `FUNC` with `ARGS...` be implicitly
    /// convertible to `RET`, as opposed to explicitly convertible.
    /// Further, the use of `bslmf::ForwardingType` to forward arguments in
    /// the invoker of a `bsl::function` creates qualitatively different
    /// behavior than the argument forwarding mechanism used by the standard
    /// `INVOKE` pseudo-expression.
    template <class PROTOTYPE, class FUNC>
    struct IsFuncInvocable
    : Function_InvokerUtil_IsFuncInvocable<PROTOTYPE, FUNC> {
    };
#endif

    // CLASS METHODS

    /// Return a null pointer.  Note that template argument `PROTOTYPE` must
    /// be supplied excplicitly, as there is no way to deduce it from the
    /// function arguments.
    template <class PROTOTYPE>
    static GenericInvoker *invokerForFunc(const bsl::nullptr_t&);

    /// Return a pointer to the invoker for a callable object of (template
    /// paramter) type `FUNC`.  If the specified `f` object is a null
    /// pointer or null pointer-to-member, return a null pointer.  Note that
    /// template argument `PROTOTYPE` must be supplied excplicitly, as there
    /// is no way to deduce it from the function arguments.
    template <class PROTOTYPE, class FUNC>
    static GenericInvoker *invokerForFunc(const FUNC& f);
};

               // =============================================
               // template struct Function_InvokerUtilNullCheck
               // =============================================

/// Provides an `isNull` static method that that returns whether or not its
/// argument is "null", i.e., it cannot be invoked.  For must `FUNC` types
/// `isNull` always returns `false` as every instance of `FUNC` is
/// invocable.  However, specializations of this class, especially for
/// pointer types, have `isNull` functions that sometimes return `true`.
/// This class is a customization point: types outside of this component can
/// (but rarely should) specialize this template.  In particular,
/// `bslstl_function` contains a specialization for `bsl::function`.
template <class FUNC>
struct Function_InvokerUtilNullCheck {

    // CLASS METHODS

    /// Return `false`.
    static bool isNull(const FUNC&);
};

/// Specialization of dispatcher for pointer objects.
template <class FUNC>
struct Function_InvokerUtilNullCheck<FUNC *> {

    // CLASS METHODS

    /// Return `true` if the specified `f` pointer is null; otherwise
    /// `false`.
    static bool isNull(FUNC *f);
};

/// Specialization of dispatcher for pointer-to-member objects.
template <class CLASS, class MEMTYPE>
struct Function_InvokerUtilNullCheck<MEMTYPE CLASS::*> {

  public:
    // CLASS METHODS

    /// Return `true` if the specified `f` pointer to member is null;
    /// otherwise `false`.
    static bool isNull(MEMTYPE CLASS::* f);
};

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=13

            // ====================================================
            // template struct Function_InvokerUtil_IsFuncInvocable
            // ====================================================

#ifdef BSLSTL_FUNCTION_INVOKERUTIL_SUPPORT_IS_FUNC_INVOCABLE

/// forward declaration
template <class VOID_TYPE, class RET, class FUNC, class... ARGS>
struct Function_InvokerUtil_IsFuncInvocableImp;

/// This `struct` template implements a Boolean metafunction that publicly
/// inherits from `bsl::true_type` if an object of the specified `FUNC` type
/// is invocable with the specified `RET` and `ARGS...`, and publicly
/// inherits from `bsl::false_type` otherwise.  An object of `FUNC` type is
/// invocable with `RET` and `ARGS...` if it is Lvalue-Callable with
/// arguments having the types of the `ARGS...`, and returns an object of
/// type explicitly convertible to `RET`.  If `RET` is `void`, then any type
/// is considered explicitly convertible to `RET`.
template <class RET, class FUNC, class... ARGS>
struct Function_InvokerUtil_IsFuncInvocable<RET(ARGS...), FUNC>
: Function_InvokerUtil_IsFuncInvocableImp<void, RET, FUNC, ARGS...> {
};

          // =======================================================
          // template struct Function_InvokerUtil_IsFuncInvocableImp
          // =======================================================

/// forward declaration
template <class ARG>
struct Function_InvokerUtil_ForwardType;

/// forward declaration
template <class FROM, class TO>
struct Function_InvokerUtil_IsExplicitlyConvertible;

/// forward declaration
template <class FUNC, class... ARGS>
struct Function_InvokerUtil_ResultType;

/// This component-private `struct` template provides the primary template
/// definition for the `Function_InvokerUtil_IsFuncInvocableImp` Boolean
/// metafunction, which is an implementation detail of the
/// `Function_InvokerUtil_IsFuncInvocable` Boolean metafunction.  This
/// specialization is instantiated when `std::invoke_result<T, FWD_ARGS...>`
/// does not provide a nested `type` typedef, where `T` is the result of
/// stripping a nothrow-movable wrapper from `FUNC`, if present, and adding
/// an lvalue reference, and where `FWD_ARGS...` are the types that result
/// from forwarding objects of respective `ARGS...` types.  This `struct`
/// template requires that the specified `VOID_TYPE` type to be `void`.
template <class VOID_TYPE, class RET, class FUNC, class... ARGS>
struct Function_InvokerUtil_IsFuncInvocableImp : bsl::false_type {
};

/// This component-private `struct` template provides a partial
/// specialization of `Function_InvokerUtil_IsFuncInvocableImp` for `FUNC`
/// types for which `bsl::invoke_result<T, FWD_ARGS...>` provides a nested
/// `type` typedef, where `T` is the result of stripping a nothrow-movable
/// wrapper from `FUNC`, if present, and adding an lvalue reference, and
/// where `FWD_ARGS...` are the types that result from forwarding objects
/// of respective `ARGS...` types.
///
/// Specifically, `T` is
/// `bslalg::NothrowMovableUtil::UnwrappedType<FUNC>::type&`, and
/// `FWD_ARGS...` are the types of respective
/// `bslmf::ForwardingTypeUtil<ARGS...>::forwardToTarget(args...)`
/// expressions where `args...` are lvalue expressions of respective
/// `bslmf::ForwardingType<ARGS>::Type...` types.  See the class-level
/// documentation of the primary `Function_InvokerUtil::IsFuncInvocable`
/// declaration for more information about the behavior of this `struct`
/// template.
template <class RET, class FUNC, class... ARGS>
struct Function_InvokerUtil_IsFuncInvocableImp<
    typename bslmf::VoidType<
        typename Function_InvokerUtil_ResultType<FUNC, ARGS...>::type>::type,
    RET,
    FUNC,
    ARGS...>
: bsl::conditional<
      bsl::is_void<RET>::value,
      bsl::true_type,
      Function_InvokerUtil_IsExplicitlyConvertible<
          typename Function_InvokerUtil_ResultType<FUNC, ARGS...>::type,
          RET> >::type {
};

              // ===============================================
              // template struct Function_InvokerUtil_ResultType
              // ===============================================

/// forward declaration
template <class VOID_TYPE, class FUNC, class... ARGS>
struct Function_InvokerUtil_ResultTypeImp;

/// This component-private `struct` template provides a metafunction that
/// conditionally defines a nested `type` typedef if the standard
/// `INVOKE(f, fwdArgs...)` pseudo-expression is well-formed, where `f` is
/// an lvalue of `bslalg::NothrowMoveableUtil::UnwrappedType<FUNC>` type,
/// and `fwdArgs...` are defined to be the expressions
/// `bslmf::ForwardingTypeUtil<ARGS>::forwardToTarget(args)...` where
/// `args...` are lvalue expressions of the corresponding
/// `bslmf::ForwardingType<ARGS>::Type...` types.  If such an `INVOKE`
/// expression is well-formed, this struct defines `type` to be the result
/// type of the expression, and defines no such typedef otherwise.
template <class FUNC, class... ARGS>
struct Function_InvokerUtil_ResultType
: Function_InvokerUtil_ResultTypeImp<void, FUNC, ARGS...> {
};

             // ==================================================
             // template struct Function_InvokerUtil_ResultTypeImp
             // ==================================================

/// This component-private `struct` template provides the primary template
/// definition for the `Function_InvokerUtil_ResultTypeImp` metafunction,
/// which is an implementation detail of the
/// `Function_InvokerUtil_ResultType` metafunction.  This specialization is
/// instantiated when the standard `INVOKE(f, fwdArgs...)` pseudo-expression
/// is not well-formed, where `f` is an lvalue of
/// `bslalg::NothrowMoveableUtil::UnwrappedType<FUNC>` type, and
/// `fwdArgs...` are defined to be the expressions
/// `bslmf::ForwardingTypeUtil<ARGS>::forwardToTarget(args)...` where
/// `args...` are lvalue expressions of the corresponding
/// `bslmf::ForwardingType<ARGS>::Type...` types.
template <class VOID_TYPE, class FUNC, class... ARGS>
struct Function_InvokerUtil_ResultTypeImp {
};

template <class FUNC, class... ARGS>
struct Function_InvokerUtil_ResultTypeImp<
    typename bslmf::VoidType<
        typename Function_InvokerUtil_ForwardType<ARGS>::Type...>::type,
    FUNC,
    ARGS...>
: bsl::invoke_result<
      typename bsl::add_lvalue_reference<typename bslalg::NothrowMovableUtil::
                                             UnwrappedType<FUNC>::type>::type,
      typename Function_InvokerUtil_ForwardType<ARGS>::Type...> {

    // This component-private 'struct' template provides a partial
    // specialization of 'Function_InvokerUtil_ResultTypeImp' for 'FUNC' and
    // 'ARGS...' types for which standard 'INVOKE(f, fwdArgs...)'
    // pseudo-expression is well-formed, where 'f' is an lvalue of
    // 'bslalg::NothrowMoveableUtil::UnwrappedType<FUNC>' type, and
    // 'fwdArgs...' are defined to be the expressions
    // 'bslmf::ForwardingTypeUtil<ARGS>::forwardToTarget(args)...' where
    // 'args...' are lvalue expressions of the corresponding
    // 'bslmf::ForwardingType<ARGS>::Type...' types.  This 'struct' defines a
    // nested typedef 'type' that is the type of this expression.
};

              // ================================================
              // template struct Function_InvokerUtil_ForwardType
              // ================================================

/// forward declaration
template <class VOID_TYPE, class ARG>
struct Function_InvokerUtil_ForwardTypeImp;

/// This component-private `struct` template provides a metafunction that
/// conditionally defines a nested `type` typedef that is the type resulting
/// from forwarding an lvalue of the specified `ARG` type using the
/// machinery from `bslmf_forwardingtype`.  If it is not possible to forward
/// an object of `ARG` type (if, for example, `ARG` is `void`) then this
/// `struct` template does not define a `type` typedef.
///
/// Specifically, `type` is defined to be the type of the expression
/// `bslmf::ForwardingTypeUtil<ARG>::forwardingToTarget(arg)`, where `arg`
/// is an lvalue expression of `bslmf::ForwardingType<ARG>::Type` type.
/// `type` is not defined when this expression is not well-formed.
template <class ARG>
struct Function_InvokerUtil_ForwardType
: Function_InvokerUtil_ForwardTypeImp<void, ARG> {
};

            // ===================================================
            // template struct Function_InvokerUtil_ForwardTypeImp
            // ===================================================

/// This component-private `struct` template provides the primary template
/// definition for the `Function_InvokerUtil_ForwardTypeImp` metafunction,
/// which is an implementation detail of the
/// `Function_InvokerUtil_IsFuncInvocable` metafunction.  This
/// specialization is instantiated when the expression
/// `bslmf::ForwardingTypeUtil<ARG>::forwardToTarget(arg)` is not
/// well-formed, where `arg` is an lvalue expression of
/// `bslmf::ForwardingType<ARG>::Type` type.
template <class VOID_TYPE, class ARG>
struct Function_InvokerUtil_ForwardTypeImp {
};

/// This component-private `struct` template provides a partial
/// specialization of `Function_InvokerUtil_ForwardTypeImp` for `ARG` types
/// for which the expression
/// `bslmf::ForwardingTypeUtil<ARG>::forwardToTarget(arg)` is well-formed,
/// where `arg` is an lvalue expression of
/// `bslmf::ForwardingType<ARG>::Type` type.  This `struct` template
/// defines a nested `type` typedef that is the type of the expression.
/// This is the type resulting from forwarding an lvalue of the specified
/// `ARG` type using the machinery from `bslmf_forwardingtype`.
template <class ARG>
struct Function_InvokerUtil_ForwardTypeImp<
    typename bslmf::VoidType<decltype(
        bslmf::ForwardingTypeUtil<ARG>::forwardToTarget(
            std::declval<typename bsl::add_lvalue_reference<
                typename bslmf::ForwardingType<ARG>::Type>::type>()))>::type,
    ARG> {

    // TYPES

    /// `Type` is an alias to the "forwarded" type of the specified `ARG`
    /// type.  Specifically, it is the type of the expression
    /// `bslmf::ForwardingTypeUtil<ARG>::forwardingToTarget(arg)`, where
    /// `arg` is an lvalue expression of `bslmf::ForwardingType<ARG>::Type`
    /// type.
    typedef decltype(bslmf::ForwardingTypeUtil<ARG>::forwardToTarget(
        std::declval<typename bsl::add_lvalue_reference<
            typename bslmf::ForwardingType<ARG>::Type>::type>())) Type;
};

        // ============================================================
        // template struct Function_InvokerUtil_IsExplicitlyConvertible
        // ============================================================

/// forward declaration
template <class VOID_TYPE, class FROM, class TO>
struct Function_InvokerUtil_IsExplicitlyConvertibleImp;

/// This component-private `struct` template provides a Boolean metafunction
/// that publicly derives from `bsl::true_type` if the specified `FROM` type
/// can be explicitly converted to the specified `TO` type, and derives from
/// `bsl::false_type` otherwise.  The type `FROM` is explicitly convertible
/// to the type `TO` if the expression
/// `static_cast<TO>(std::declval<FROM>())` is well-formed.
template <class FROM, class TO>
struct Function_InvokerUtil_IsExplicitlyConvertible
: Function_InvokerUtil_IsExplicitlyConvertibleImp<void, FROM, TO> {
};

/// A type is always explicitly convertible to itself.  This is important
/// for classes with deleted copy constructors and/or assignment operators,
/// because starting in C++17, they can be returned from functions, due to
/// "return value optimization (RVO)".
template <class FROM_TO>
struct Function_InvokerUtil_IsExplicitlyConvertible<FROM_TO, FROM_TO>
: bsl::true_type {
};

      // ===============================================================
      // template struct Function_InvokerUtil_IsExplicitlyConvertibleImp
      // ===============================================================

/// This component-private `struct` template provides the primary
/// specialization of the `Function_InvokerUtil_IsExplicitlyConvertibleImp`
/// metafunction.  This specialization is instantiated when the specified
/// `FROM` type is not explicitly convertible to the specified `TO` type,
/// and publicly ihherits from `bsl::false_type`.  This `struct` template
/// requires the specified `VOID_TYPE` type to be `void`.
template <class VOID_TYPE, class FROM, class TO>
struct Function_InvokerUtil_IsExplicitlyConvertibleImp :  bsl::false_type {
};

/// This component-private `struct` template provides a partial
/// specialization of the `Function_InvokerUtil_IsExplicitlyConvertibleImp`
/// metafunction.  This specialization is instantiated when the specified
/// `FROM` type is explicitly convertible to the specified `TO` type, and
/// publicly inherits from `bsl::true_type`.
template <class FROM, class TO>
struct Function_InvokerUtil_IsExplicitlyConvertibleImp<
    typename bslmf::VoidType<decltype(
       static_cast<TO>(std::declval<FROM>()))>::type,
    FROM,
    TO> : bsl::true_type {
};

#endif // defined(BSLSTL_FUNCTION_INVOKERUTIL_SUPPORT_IS_FUNC_INVOCABLE)
#endif

               // =============================================
               // template struct Function_InvokerUtil_Dispatch
               // =============================================

/// Specializations of this class contain a static `invoke` method that can
/// invoke a callable object of type `FUNC`, converting each argument in
/// `PROTOTYPE` (a function prototype) to the corresponding argument in the
/// invocation of the callable object and converting the return value of the
/// invocation to the return type of `PROTOTYPE`.  The `INVOCATION_TYPE`
/// specifies the category of callable object: pointer to function, pointer
/// to member function, pointer to data member, inplace functor (i.e., one
/// that qualifies for the small-object optimization) and out-of-place
/// functor (i.e., one that is not stored in the small-object buffer).
template <int INVOCATION_TYPE, class PROTOTYPE, class FUNC>
struct Function_InvokerUtil_Dispatch;

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=13

// STATIC METHODS OF PRIVATE NESTED *Invoker CLASS TEMPLATES

/// Specialization of dispatcher for pointer-to-function objects.
template <class FUNC, class RET, class... ARGS>
struct Function_InvokerUtil_Dispatch<Function_InvokerUtil::e_FunctionPtr,
                                     RET(ARGS...), FUNC> {

    // CLASS METHODS

    /// For the specified `rep` and `args`, return
    /// `rep.target<FUNC>()(args...)`.
    static RET invoke(const Function_Rep                            *rep,
                      typename bslmf::ForwardingType<ARGS>::Type...  args);
};

/// Specialization of dispatcher for pointer to member function objects.
template <class FUNC, class RET, class ARG0, class... ARGS>
struct Function_InvokerUtil_Dispatch<Function_InvokerUtil::e_MemFunctionPtr,
                                     RET(ARG0, ARGS...), FUNC> {

  private:
    // PRIVATE CLASS METHODS

    /// Use the specified `obj` to invoke the specified `f` pointer to
    /// member function with the specified `args` and return the result,
    /// i.e., `(obj.*f)(args...)`.
    static RET invokeImp(bsl::true_type                         /* isDirect */,
                         FUNC                                           f,
                         typename bslmf::ForwardingType<ARG0>::Type     obj,
                         typename bslmf::ForwardingType<ARGS>::Type...  args);

    /// Dereference the specified `obj` and usit to invoke the specified `f`
    /// pointer to member function with the specified `args` and return the
    /// result, i.e., `((*obj).*f)(args...)`.
    static RET invokeImp(bsl::false_type                        /* isDirect */,
                         FUNC                                           f,
                         typename bslmf::ForwardingType<ARG0>::Type     obj,
                         typename bslmf::ForwardingType<ARGS>::Type...  args);

  public:
    // CLASS METHODS

    /// Given pointer to member function, `f`, as the target of the
    /// specified `rep`, use the specified `obj` to invoke `f` with the
    /// specified `args` and return the result, i.e., `(obj.*f)(args...)`.
    /// If `obj` is a pointer or smart-pointer, dereference `obj` first,
    /// i.e., call `((*obj).*f)(args...)`.
    static RET invoke(const Function_Rep                            *rep,
                      typename bslmf::ForwardingType<ARG0>::Type     obj,
                      typename bslmf::ForwardingType<ARGS>::Type...  args);
};

/// Specialization of dispatcher for pointer to data member objects.
template <class MEMBER_TYPE, class CLASS_TYPE, class RET, class ARG0>
struct Function_InvokerUtil_Dispatch<Function_InvokerUtil::e_MemDataPtr,
                                     RET(ARG0), MEMBER_TYPE CLASS_TYPE::*> {
  private:
    // PRIVATE CLASS METHODS
    typedef MEMBER_TYPE CLASS_TYPE::* Func;

    /// Return the specified `f` member of the specified `obj`, i.e.,
    /// `obj.*f`.
    static RET invokeImp(bsl::true_type                         /* isDirect */,
                         Func                                        f,
                         typename bslmf::ForwardingType<ARG0>::Type  obj);

    /// Return the specified `f` member of the object obtained by
    /// dereferencing the specified `obj`, i.e., `(*obj).f`.
    static RET invokeImp(bsl::false_type                        /* isDirect */,
                         Func                                           f,
                         typename bslmf::ForwardingType<ARG0>::Type     obj);

  public:
    // CLASS METHODS

    /// Given pointer to data member, `f`, as the target of the specified
    /// `rep`, return the specified `f` member of the specified `obj`, i.e.,
    /// `obj.*f`.  If `obj` is a pointer or smart-pointer, dereference `obj`
    /// first, i.e., return `(*obj).*f`.
    static RET invoke(const Function_Rep                         *rep,
                      typename bslmf::ForwardingType<ARG0>::Type  obj);
};

/// Specialization of dispatcher functor class objects that are suitable for
/// the small-object optimization and are thus allocated inplace.
template <class FUNC, class RET, class... ARGS>
struct Function_InvokerUtil_Dispatch<Function_InvokerUtil::e_InplaceFunctor,
                                     RET(ARGS...), FUNC> {

    // CLASS METHODS

    /// For the specified `args` and `rep`, return
    /// `(*rep.target<FUNC>())(args...)`.
    static RET invoke(const Function_Rep                            *rep,
                      typename bslmf::ForwardingType<ARGS>::Type...  args);
};

/// Specialization of dispatcher functor class objects that are not suitable
/// for the small-object optimization and are thus allocated out of place.
template <class FUNC, class RET, class... ARGS>
struct Function_InvokerUtil_Dispatch<Function_InvokerUtil::e_OutofplaceFunctor,
                                     RET(ARGS...), FUNC> {

    // CLASS METHODS

    /// For the specified `args` and `rep`, return
    /// `(*rep.target<FUNC>())(args...)`.
    static RET invoke(const Function_Rep                            *rep,
                      typename bslmf::ForwardingType<ARGS>::Type...  args);
};

#endif

// ===========================================================================
//                TEMPLATE AND INLINE FUNCTION IMPLEMENTATIONS
// ===========================================================================

                        // ---------------------------
                        // struct Function_InvokerUtil
                        // ---------------------------

template <class PROTOTYPE>
inline
bslstl::Function_InvokerUtil::GenericInvoker *
bslstl::Function_InvokerUtil::invokerForFunc(const bsl::nullptr_t&)
{
    return 0;
}

template <class PROTOTYPE, class FUNC>
bslstl::Function_InvokerUtil::GenericInvoker *
bslstl::Function_InvokerUtil::invokerForFunc(const FUNC& f)
{
    typedef bslstl::Function_SmallObjectOptimization Soo;

    // Strip 'NothrowMovableWrapper' (if any) off of 'FUNC' type.
    typedef typename
        bslalg::NothrowMovableUtil::UnwrappedType<FUNC>::type UwFuncType;

    // Categorize the type of invocable corresponding to 'FUNC'.  Note that the
    // parameter to 'Soo::Inplace' is 'FUNC', not 'UwFuncType'.  That is
    // because 'Soo::Inplace' takes the wrapper into account when determining
    // whether the type should be inplace or not.
    static const int k_INVOCATION_TYPE =
        bslmf::IsFunctionPointer<UwFuncType>::value       ? e_FunctionPtr    :
        bslmf::IsMemberFunctionPointer<UwFuncType>::value ? e_MemFunctionPtr :
        bsl::is_member_pointer<UwFuncType>::value         ? e_MemDataPtr     :
        Soo::IsInplaceFunc<FUNC>::value                   ? e_InplaceFunctor :
        e_OutofplaceFunctor;

    // Instantiate the class for checking for null object
    typedef Function_InvokerUtilNullCheck<UwFuncType> NullCheckerClass;

    // Instantiate the class for dispatching the invoker
    typedef Function_InvokerUtil_Dispatch<k_INVOCATION_TYPE,
                                          PROTOTYPE,
                                          UwFuncType> DispatcherClass;

    // If a the object is "null", e.g., for a pointer, then return null.
    if (NullCheckerClass::isNull(bslalg::NothrowMovableUtil::unwrap(f)))
    {
        return 0;                                                     // RETURN
    }

    // Verify the assumption that all function pointers are the same size.
    BSLMF_ASSERT(sizeof(&DispatcherClass::invoke) ==
                 sizeof(Function_Rep::GenericInvoker *));

    // Return a pointer to the actual invoker function
    return reinterpret_cast<Function_Rep::GenericInvoker *>(
                                                 &DispatcherClass::invoke);
}

               // ---------------------------------------------
               // struct template Function_InvokerUtilNullCheck
               // ---------------------------------------------

// STATIC MEMBER FUNCTIONS

template <class FUNC>
inline
bool Function_InvokerUtilNullCheck<FUNC>::isNull(const FUNC&)
{
    return false;
}

template <class FUNC>
inline
bool Function_InvokerUtilNullCheck<FUNC *>::isNull(FUNC* f)
{
    return 0 == f;
}

template <class CLASS, class MEMTYPE>
inline
bool
Function_InvokerUtilNullCheck<MEMTYPE CLASS::*>::isNull(MEMTYPE CLASS::* f)
{
    return 0 == f;
}


               // ---------------------------------------------
               // struct template Function_InvokerUtil_Dispatch
               // ---------------------------------------------

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES

template <class FUNC, class RET, class... ARGS>
RET
Function_InvokerUtil_Dispatch<Function_InvokerUtil::e_FunctionPtr,
                              RET(ARGS...), FUNC>::
invoke(const Function_Rep                            *rep,
       typename bslmf::ForwardingType<ARGS>::Type...  args)
{
    FUNC f = *rep->targetRaw<FUNC, true>();

    // Note that 'FUNC' might be different than 'RET(*)(ARGS...)'.  All that is
    // required is that it be Callable with 'ARGS...' and return something
    // convertible to 'RET'.

    // Cast to 'RET' is needed to avoid compilation error if 'RET' is 'void'
    // and 'f' returns non-void.
    return BSLSTL_FUNCTION_INVOKERUTIL_CAST_RESULT(
        RET,
        f(bslmf::ForwardingTypeUtil<ARGS>::forwardToTarget(args)...));
}

template <class FUNC, class RET, class ARG0, class... ARGS>
inline
RET
Function_InvokerUtil_Dispatch<Function_InvokerUtil::e_MemFunctionPtr,
                              RET(ARG0, ARGS...), FUNC>::
invokeImp(bsl::true_type                              /* isDirect */,
          FUNC                                           f,
          typename bslmf::ForwardingType<ARG0>::Type     obj,
          typename bslmf::ForwardingType<ARGS>::Type...  args)
{
    typedef typename
        bslmf::MovableRefUtil::RemoveReference<ARG0>::type& Arg0Ref;

    // In C++03, an rvalue is forwarded as a const lvalue.  Fortunately, we can
    // recover the correct constness by casting 'obj' back to a reference to
    // the original type.
    return BSLSTL_FUNCTION_INVOKERUTIL_CAST_RESULT(
        RET, (const_cast<Arg0Ref>(obj).*f)(
            bslmf::ForwardingTypeUtil<ARGS>::forwardToTarget(args)...));
}

template <class FUNC, class RET, class ARG0, class... ARGS>
inline
RET
Function_InvokerUtil_Dispatch<Function_InvokerUtil::e_MemFunctionPtr,
                              RET(ARG0, ARGS...), FUNC>::
invokeImp(bsl::false_type                             /* isDirect */,
          FUNC                                           f,
          typename bslmf::ForwardingType<ARG0>::Type     obj,
          typename bslmf::ForwardingType<ARGS>::Type...  args)
{
    typedef typename
        bslmf::MovableRefUtil::RemoveReference<ARG0>::type& Arg0Ref;

    // In C++03, an rvalue is forwarded as a const lvalue.  Fortunately, we can
    // recover the correct constness from by casting back to a reference to
    // the original type.
    return BSLSTL_FUNCTION_INVOKERUTIL_CAST_RESULT(
        RET, ((*const_cast<Arg0Ref>(obj)).*f)(
            bslmf::ForwardingTypeUtil<ARGS>::forwardToTarget(args)...));
}

template <class FUNC, class RET, class ARG0, class... ARGS>
RET
Function_InvokerUtil_Dispatch<Function_InvokerUtil::e_MemFunctionPtr,
                              RET(ARG0, ARGS...), FUNC>::
invoke(const Function_Rep                            *rep,
       typename bslmf::ForwardingType<ARG0>::Type     obj,
       typename bslmf::ForwardingType<ARGS>::Type...  args)
{
    typedef typename
        bslmf::MemberFunctionPointerTraits<FUNC>::ClassType ClassType;

    /// `true_type` if `ARG0` is a reference to `ClassType` or class derived
    /// from `ClassType; otherwise, `false_type'.
    typedef typename bsl::is_convertible<
            typename bslmf::MovableRefUtil::RemoveReference<ARG0>::type *,
            ClassType *
        >::type IsDirect;

    FUNC f = *rep->targetRaw<FUNC, true>();

    return BSLSTL_FUNCTION_INVOKERUTIL_CAST_RESULT(RET,
                                                  invokeImp(IsDirect(), f, obj,
                                                            args...));
}

template <class MEMBER_TYPE, class CLASS_TYPE, class RET, class ARG0>
inline
RET
Function_InvokerUtil_Dispatch<Function_InvokerUtil::e_MemDataPtr,
                              RET(ARG0), MEMBER_TYPE CLASS_TYPE::*>::
invokeImp(bsl::true_type                              /* isDirect */,
          Func                                           f,
          typename bslmf::ForwardingType<ARG0>::Type     obj)
{
    typedef typename
        bslmf::MovableRefUtil::RemoveReference<ARG0>::type& Arg0Ref;

    // In C++03, an rvalue is forwarded as a const lvalue.  Fortunately, we can
    // recover the correct constness by casting 'obj' back to a reference to
    // the original type.
    return BSLSTL_FUNCTION_INVOKERUTIL_CAST_RESULT(
        RET, (const_cast<Arg0Ref>(obj).*f));
}

template <class MEMBER_TYPE, class CLASS_TYPE, class RET, class ARG0>
inline
RET
Function_InvokerUtil_Dispatch<Function_InvokerUtil::e_MemDataPtr,
                              RET(ARG0), MEMBER_TYPE CLASS_TYPE::*>::
invokeImp(bsl::false_type                             /* isDirect */,
          Func                                           f,
          typename bslmf::ForwardingType<ARG0>::Type     obj)
{
    typedef typename
        bslmf::MovableRefUtil::RemoveReference<ARG0>::type& Arg0Ref;

    // In C++03, an rvalue is forwarded as a const lvalue.  Fortunately, we can
    // recover the correct constness from by casting back to a reference to
    // the original type.
    return BSLSTL_FUNCTION_INVOKERUTIL_CAST_RESULT(
        RET, ((*const_cast<Arg0Ref>(obj)).*f));
}

template <class MEMBER_TYPE, class CLASS_TYPE, class RET, class ARG0>
RET
Function_InvokerUtil_Dispatch<Function_InvokerUtil::e_MemDataPtr,
                              RET(ARG0), MEMBER_TYPE CLASS_TYPE::*>::
invoke(const Function_Rep                         *rep,
       typename bslmf::ForwardingType<ARG0>::Type  obj)
{
    /// `true_type` if `ARG0` is a reference to `CLASS_TYPE` or class
    /// derived from `CLASS_TYPE; otherwise, `false_type'.  Note that this
    /// differs from the corresponding check for pointers to member
    /// functions because a pointer to data member for class type `T` can
    /// be used to access (as const) a member of a cv-qualified `T`.
    typedef typename bsl::is_convertible<
            typename bslmf::MovableRefUtil::RemoveReference<ARG0>::type *,
            const volatile CLASS_TYPE *
        >::type IsDirect;

    Func f = *rep->targetRaw<Func, true>();

    return BSLSTL_FUNCTION_INVOKERUTIL_CAST_RESULT(
        RET, invokeImp(IsDirect(), f, obj));
}

template <class FUNC, class RET, class... ARGS>
RET
Function_InvokerUtil_Dispatch<Function_InvokerUtil::e_InplaceFunctor,
                              RET(ARGS...), FUNC>::
invoke(const Function_Rep                            *rep,
       typename bslmf::ForwardingType<ARGS>::Type...  args)
{
    FUNC& f = *rep->targetRaw<FUNC, true>();

    // Cast to 'RET' is needed to avoid compilation error if 'RET' is void and
    // 'f' returns non-void.
    return BSLSTL_FUNCTION_INVOKERUTIL_CAST_RESULT(
        RET,
        f(bslmf::ForwardingTypeUtil<ARGS>::forwardToTarget(args)...));
}

template <class FUNC, class RET, class... ARGS>
RET
Function_InvokerUtil_Dispatch<Function_InvokerUtil::e_OutofplaceFunctor,
                              RET(ARGS...), FUNC>::
invoke(const Function_Rep                            *rep,
       typename bslmf::ForwardingType<ARGS>::Type...  args)
{
    FUNC& f = *rep->targetRaw<FUNC, false>();

    // Cast to 'RET' is needed to avoid compilation error if 'RET' is void and
    // 'f' returns non-void.
    return BSLSTL_FUNCTION_INVOKERUTIL_CAST_RESULT(
        RET,
        f(bslmf::ForwardingTypeUtil<ARGS>::forwardToTarget(args)...));
}

#endif

}  // close package namespace
}  // close enterprise namespace

#endif // End C++11 code

#endif // ! defined(INCLUDED_BSLSTL_FUNCTION_INVOKERUTIL)

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
