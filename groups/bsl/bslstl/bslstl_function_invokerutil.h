// bslstl_function_invokerutil.h                                      -*-C++-*-
#ifndef INCLUDED_BSLSTL_FUNCTION_INVOKERUTIL
#define INCLUDED_BSLSTL_FUNCTION_INVOKERUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide invoker adaptors for 'bsl::function'
//
//@CLASSES:
//  Function_InvokerUtil: Utility for returning an appropriate invoker function
//
//@SEE_ALSO: bslstl_function
//
//@DESCRIPTION: This component provides a struct, 'Function_InvokerUtil',
// containing a function template, 'invokerForFunc', that returns a pointer to
// a function that is used to invoke a callable object of a particular type.
// The returned type is custom-generated for each specific target type.  This
// component is for private use only.
//
// The client of this component, 'bsl::function', is a complex class that is
// templated in two ways:
//
//: 1 The class itself has a template parameter representing the prototype
//:   (argument and return types) of its call operator.  E.g., type
//:   'bsl::function<int(char*)>' has member 'int operator()(char*);'.
//:
//: 2 Several of its constructors take an argument of template parameter
//:   callable type and wrap it, using type erasure, so that the type of the
//:   wrapped target is not part of the type of the 'bsl::function'.
//
// Only the invocation mechanism needs both kinds of template parameters; other
// parts of 'bsl::function' (e.g., 'bslstl::Function_Rep') concern themselves
// with only the callable object type, not with the prototype of the call
// operator.  This component factors out most of that complexity so as to
// minimise code size, especially when using the 'sim_cpp11_features.pl' script
// to simulate C++11 variadic templates in C++03.
//
// The classes in this component are stateless and contain only static
// functions.
//
///Return value of 'Function_InvokerUtil::invokerForFunc'
///------------------------------------------------------
// The function pointer, 'invoker_p', returned by
// 'Function_InvokerUtil::invokerForFunc<RET(ARG0, ARG1, ...)>' takes an
// argument of type 'Function_Rep *' and zero or more argument types 'ARG0',
// 'ARG1', ..., as specified by the template argument.  Calling
// 'invoker_p(rep, arg0, arg1, ...)' gets the target, 'f' from 'rep' and
// invokes it with the supplied arguments.  Invocation of 'f' follows the
// definition of *INVOKE* in section [func.require] of the C++ standard.  These
// rules are summarized in the following table:
//..
//  +----------------------------+-----------------------+
//  | Type of target object, 'f' | Invocation expression |
//  +============================+=======================+
//  | Pointer to function or     | f(arg0, arg1, ...)    |
//  | functor class              |                       |
//  +----------------------------+-----------------------+
//  | Pointer to member function | (arg0X.*f)(arg1, ...) |
//  +----------------------------+-----------------------+
//  | pointer to data member     | arg0X.*f              |
//  +----------------------------+-----------------------+
//..
// The arguments to 'f' must be implicitly convertible from the corresponding
// argument types 'ARG0', 'ARG1', ... and the return value of the call
// expression must be implicitly convertible to 'RET', unless 'RET' is 'void'.
//
// In the case of a pointer to member function, 'R (T::*f)(...)', or pointer to
// data member 'R T::*f', 'arg0X' is one of the following:
//
//: o 'arg0' if 'ARG0' is 'T' or derived from 'T'.
//: o 'arg0.get()' if 'ARG0' is a specialization of 'reference_wrapper'.
//: o '(*arg0)' if 'ARG0' is a pointer type or pointer-like type (e.g., a smart
//:   pointer).
//
// Note that, consistent with the C++ Standard definition of *INVOKE*, we
// consider pointer-to-member-function and pointer-to-data-member types to be
// "callable" even though, strictly speaking, they lack an 'operator()'.

#include <bslscm_version.h>

#include <bslalg_nothrowmovableutil.h>

#include <bslmf_forwardingtype.h>
#include <bslmf_ismemberpointer.h>
#include <bslmf_memberfunctionpointertraits.h>
#include <bslmf_movableref.h>

#include <bsls_compilerfeatures.h>
#include <bsls_nullptr.h>

#include <bslstl_function_rep.h>
#include <bslstl_function_smallobjectoptimization.h>

#ifndef BSLS_PLATFORM_CMP_SUN
#define BSLSTL_FUNCTION_INVOKERUTIL_CAST_RESULT(RET, X) static_cast<RET>(X)
#else
#define BSLSTL_FUNCTION_INVOKERUTIL_CAST_RESULT(RET, X) (RET)(X)
#endif

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Include version that can be compiled with C++03
// Generated on Fri Dec  4 19:11:45 2020
// Command line: sim_cpp11_features.pl bslstl_function_invokerutil.h
# define COMPILING_BSLSTL_FUNCTION_INVOKERUTIL_H
# include <bslstl_function_invokerutil_cpp03.h>
# undef COMPILING_BSLSTL_FUNCTION_INVOKERUTIL_H
#else

namespace BloombergLP {

namespace bslstl {

                        // ===========================
                        // struct Function_InvokerUtil
                        // ===========================

struct Function_InvokerUtil {
    // This struct is a namespace containing a single function template,
    // 'invokerForFunc', that returns a pointer to a function that is used to
    // invoke a callable object of a particular type.

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

    typedef Function_Rep::GenericInvoker GenericInvoker;
        // Generic function pointer.  This type is as close as we can get to
        // 'void *' for function pointers.

    // CLASS METHODS
    template <class PROTOTYPE>
    static GenericInvoker *invokerForFunc(const bsl::nullptr_t&);
        // Return a null pointer.  Note that template argument 'PROTOTYPE' must
        // be supplied excplicitly, as there is no way to deduce it from the
        // function arguments.

    template <class PROTOTYPE, class FUNC>
    static GenericInvoker *invokerForFunc(const FUNC& f);
        // Return a pointer to the invoker for a callable object of (template
        // paramter) type 'FUNC'.  If the specified 'f' object is a null
        // pointer or null pointer-to-member, return a null pointer.  Note that
        // template argument 'PROTOTYPE' must be supplied excplicitly, as there
        // is no way to deduce it from the function arguments.
};

               // =============================================
               // template struct Function_InvokerUtil_Dispatch
               // =============================================

template <int INVOCATION_TYPE, class PROTOTYPE, class FUNC>
struct Function_InvokerUtil_Dispatch;
    // Specializations of this class contain a static 'invoke' method that can
    // invoke a callable object of type 'FUNC', converting each argument in
    // 'PROTOTYPE' (a function prototype) to the corresponding argument in the
    // invocation of the callable object and converting the return value of the
    // invocation to the return type of 'PROTOTYPE'.  The 'INVOCATION_TYPE'
    // specifies the category of callable object: pointer to function, pointer
    // to member function, pointer to data member, inplace functor (i.e., one
    // that qualifies for the small-object optimization) and out-of-place
    // functor (i.e., one that is not stored in the small-object buffer).
    // Specializations also contain a static 'isNull' method that identifies if
    // an instance of 'FUNC' should be treated as null.  The primary template
    // is never instantiated and has no body.  Compilation will fail unless
    // 'FUNC' is invokable with the arguments in 'PROTOTYPE' and the return
    // type of 'PROTOTYPE' is 'void' or the return type of that invokation is
    // convertible to the return type of 'PROTOTYPE'.

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=13

// STATIC METHODS OF PRIVATE NESTED *Invoker CLASS TEMPLATES
template <class FUNC, class RET, class... ARGS>
struct Function_InvokerUtil_Dispatch<Function_InvokerUtil::e_FunctionPtr,
                                     RET(ARGS...), FUNC> {
    // Specialization of dispatcher for pointer-to-function objects.

    // CLASS METHODS
    static RET invoke(const Function_Rep                            *rep,
                      typename bslmf::ForwardingType<ARGS>::Type...  args);
        // For the specified 'rep' and 'args', return
        // 'rep.target<FUNC>()(args...)'.

    static bool isNull(FUNC f);
        // Return 'true' if the specified 'f' pointer-to-function is null;
        // otherwise 'false'.
};

template <class FUNC, class RET, class ARG0, class... ARGS>
struct Function_InvokerUtil_Dispatch<Function_InvokerUtil::e_MemFunctionPtr,
                                     RET(ARG0, ARGS...), FUNC> {
    // Specialization of dispatcher for pointer to member function objects.

  private:
    // PRIVATE CLASS METHODS
    static RET invokeImp(bsl::true_type                         /* isDirect */,
                         FUNC                                           f,
                         typename bslmf::ForwardingType<ARG0>::Type     obj,
                         typename bslmf::ForwardingType<ARGS>::Type...  args);
        // Use the specified 'obj' to invoke the specified 'f' pointer to
        // member function with the specified 'args' and return the result,
        // i.e., '(obj.*f)(args...)'.

    static RET invokeImp(bsl::false_type                        /* isDirect */,
                         FUNC                                           f,
                         typename bslmf::ForwardingType<ARG0>::Type     obj,
                         typename bslmf::ForwardingType<ARGS>::Type...  args);
        // Dereference the specified 'obj' and usit to invoke the specified 'f'
        // pointer to member function with the specified 'args' and return the
        // result, i.e., '((*obj).*f)(args...)'.

  public:
    // CLASS METHODS
    static RET invoke(const Function_Rep                            *rep,
                      typename bslmf::ForwardingType<ARG0>::Type     obj,
                      typename bslmf::ForwardingType<ARGS>::Type...  args);
        // Given pointer to member function, 'f', as the target of the
        // specified 'rep', use the specified 'obj' to invoke 'f' with the
        // specified 'args' and return the result, i.e., '(obj.*f)(args...)'.
        // If 'obj' is a pointer or smart-pointer, dereference 'obj' first,
        // i.e., call '((*obj).*f)(args...)'.

    static bool isNull(FUNC f);
        // Return 'true' if the specified 'f' pointer to member function is
        // null; otherwise 'false'
};

template <class MEMBER_TYPE, class CLASS_TYPE, class RET, class ARG0>
struct Function_InvokerUtil_Dispatch<Function_InvokerUtil::e_MemDataPtr,
                                     RET(ARG0), MEMBER_TYPE CLASS_TYPE::*> {
    // Specialization of dispatcher for pointer to data member objects.
  private:
    // PRIVATE CLASS METHODS
    typedef MEMBER_TYPE CLASS_TYPE::* Func;

    static RET invokeImp(bsl::true_type                         /* isDirect */,
                         Func                                        f,
                         typename bslmf::ForwardingType<ARG0>::Type  obj);
        // Return the specified 'f' member of the specified 'obj', i.e.,
        // 'obj.*f'.

    static RET invokeImp(bsl::false_type                        /* isDirect */,
                         Func                                           f,
                         typename bslmf::ForwardingType<ARG0>::Type     obj);
        // Return the specified 'f' member of the object obtained by
        // dereferencing the specified 'obj', i.e., '(*obj).f'.

  public:
    // CLASS METHODS
    static RET invoke(const Function_Rep                         *rep,
                      typename bslmf::ForwardingType<ARG0>::Type  obj);
        // Given pointer to data member, 'f', as the target of the specified
        // 'rep', return the specified 'f' member of the specified 'obj', i.e.,
        // 'obj.*f'.  If 'obj' is a pointer or smart-pointer, dereference 'obj'
        // first, i.e., return '(*obj).*f'.

    static bool isNull(Func f);
        // Return 'true' if the specified 'f' pointer to data member is null;
        // otherwise 'false'
};

template <class FUNC, class RET, class... ARGS>
struct Function_InvokerUtil_Dispatch<Function_InvokerUtil::e_InplaceFunctor,
                                     RET(ARGS...), FUNC> {
    // Specialization of dispatcher functor class objects that are suitable for
    // the small-object optimization and are thus allocated inplace.

    // CLASS METHODS
    static RET invoke(const Function_Rep                            *rep,
                      typename bslmf::ForwardingType<ARGS>::Type...  args);
        // For the specified 'args' and 'rep', return
        // '(*rep.target<FUNC>())(args...)'.

    static bool isNull(const FUNC&);
        // Return false
};

template <class FUNC, class RET, class... ARGS>
struct Function_InvokerUtil_Dispatch<Function_InvokerUtil::e_OutofplaceFunctor,
                                     RET(ARGS...), FUNC> {
    // Specialization of dispatcher functor class objects that are not suitable
    // for the small-object optimization and are thus allocated out of place.

    // CLASS METHODS
    static RET invoke(const Function_Rep                            *rep,
                      typename bslmf::ForwardingType<ARGS>::Type...  args);
        // For the specified 'args' and 'rep', return
        // '(*rep.target<FUNC>())(args...)'.

    static bool isNull(const FUNC&);
        // Return false
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

    // Instantiate the class for dispatching the invoker
    typedef Function_InvokerUtil_Dispatch<k_INVOCATION_TYPE,
                                          PROTOTYPE,
                                          UwFuncType> DispatcherClass;

    // If a pointer-to-function or pointer-to-member-function is null, then
    // return null.
    if (DispatcherClass::isNull(bslalg::NothrowMovableUtil::unwrap(f)))
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

template <class FUNC, class RET, class... ARGS>
inline
bool Function_InvokerUtil_Dispatch<Function_InvokerUtil::e_FunctionPtr,
                                   RET(ARGS...), FUNC>::isNull(FUNC f)
{
    return 0 == f;
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

    typedef typename bsl::is_convertible<
            typename bslmf::MovableRefUtil::RemoveReference<ARG0>::type *,
            ClassType *
        >::type IsDirect;
        // 'true_type' if 'ARG0' is a reference to 'ClassType' or class derived
        // from 'ClassType; otherwise, 'false_type'.

    FUNC f = *rep->targetRaw<FUNC, true>();

    return BSLSTL_FUNCTION_INVOKERUTIL_CAST_RESULT(RET,
                                                  invokeImp(IsDirect(), f, obj,
                                                            args...));
}

template <class FUNC, class RET, class ARG0, class... ARGS>
inline
bool Function_InvokerUtil_Dispatch<Function_InvokerUtil::e_MemFunctionPtr,
                                   RET(ARG0, ARGS...), FUNC>::isNull(FUNC f)
{
    return 0 == f;
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
    typedef typename bsl::is_convertible<
            typename bslmf::MovableRefUtil::RemoveReference<ARG0>::type *,
            const volatile CLASS_TYPE *
        >::type IsDirect;
        // 'true_type' if 'ARG0' is a reference to 'CLASS_TYPE' or class
        // derived from 'CLASS_TYPE; otherwise, 'false_type'.  Note that this
        // differs from the corresponding check for pointers to member
        // functions because a pointer to data member for class type 'T' can
        // be used to access (as const) a member of a cv-qualified 'T'.

    Func f = *rep->targetRaw<Func, true>();

    return BSLSTL_FUNCTION_INVOKERUTIL_CAST_RESULT(
        RET, invokeImp(IsDirect(), f, obj));
}

template <class MEMBER_TYPE, class CLASS_TYPE, class RET, class ARG0>
inline
bool Function_InvokerUtil_Dispatch<Function_InvokerUtil::e_MemDataPtr,
                                   RET(ARG0), MEMBER_TYPE CLASS_TYPE::*>::
isNull(Func f)
{
    return 0 == f;
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
inline
bool Function_InvokerUtil_Dispatch<Function_InvokerUtil::e_InplaceFunctor,
                                   RET(ARGS...), FUNC>::isNull(const FUNC&)
{
    return false;
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

template <class FUNC, class RET, class... ARGS>
inline
bool Function_InvokerUtil_Dispatch<Function_InvokerUtil::e_OutofplaceFunctor,
                                   RET(ARGS...), FUNC>::isNull(const FUNC&)
{
    return false;
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
