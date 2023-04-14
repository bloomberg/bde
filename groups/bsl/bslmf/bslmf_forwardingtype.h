// bslmf_forwardingtype.h                                             -*-C++-*-
#ifndef INCLUDED_BSLMF_FORWARDINGTYPE
#define INCLUDED_BSLMF_FORWARDINGTYPE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for determining an optimal forwarding type.
//
//@CLASSES:
//  bslmf::ForwardingType: meta-function to determine optimal forwarding type
//  bslmf::ForwardingTypeUtil: Namespace for forwarding functions
//
//@SEE_ALSO: bslmf_removecvq
//
//@DESCRIPTION: This component, while not deprecated, is largely **superseded**
// by the simpler 'bslmf_forwardingreftype'.  Although there may be use cases
// for which 'bslmf_forwardingtype' is to be prefered, new users are encouraged
// to consider 'bslmf_forwardingreftype' and understand the differences between
// the two components (see {Comparison to 'bslmf_forwardingreftype'}).
//
// This component provides a meta function, 'bslmf::ForwardingType',
// determining the most efficient forwarding type for a given template type
// 't_TYPE'.  The forwarding type is used to pass an argument from the client
// of a component through a chain of nested function calls to the ultimate
// consumer of the argument.  This component also provides a utility class
// template, 'bslmf::ForwardingTypeUtil', supplying functions to most
// efficiently forward an argument to another function.
//
// For instance, basic types (e.g., fundamental types, pointer types, function
// references and pointers) can efficiently be passed by value down a chain of
// nested function calls.  However a large object or one with a non-trivial
// copy constructor would be better passed by const reference, even if the
// ultimate consumer takes that argument by value.
//
// Another form of optimization is the early decay of arrays to pointers,
// preventing a proliferation of different template instantiations for every
// array size being used.  Although the outermost function may still be
// instantiated on the full array type, intermediate functions are all
// instantiated on the same pointer type, regardless of array size.  This
// decay also applies to reference-to-array types.  The user can recover the
// original array type when forwarding to the final consumer by using
// 'bslmf::ForwardingTypeUtil<T>::forwardToTarget()' (see below).
//
// An argument 'v' of type 'T' can be passed as type 'ForwardingType<T>::Type'
// down an arbitrarily-long chain of function calls without ever calling
// 'std::forward'.  However, in order to avoid an extra copy as well as to
// select the correct overload and instantiation of the eventual target
// function, it should be converted back to a type that more closely resembles
// the original 'T' by calling 'ForwardingTypeUtil<T>::forwardToTarget(v)'.
//
// This component is intended to be used when performance is of highest concern
// or when creating function wrappers that are intended to minimize
// perturbations on the interface of the functions that they wrap.
//
// Note that previous versions of this component forwarded const references to
// basic types by value instead of by const reference.  This transformation was
// an attempt to avoid an extra dereference operation, but in real use cases
// the extra dereference happened anyway, on the call to the outermost
// forwarding function.  Moreover, such a transformation is subtly broken
// because, in the rare case where the target function cares about the address
// of the reference (e.g., if it compares it to some known address), it would
// wind up with the address of a temporary copy, rather than the address of the
// original argument.  Thus, the current component forwards references as
// references in all cases, including for basic types, except in the case of
// arrays and functions (that decay to pointers).
//
///Comparison to 'bslmf_forwardingreftype'
///---------------------------------------
// The components 'bslmf_forwardingtype' and 'bslmf_forwardingreftype' serve
// the same purpose but have small behavioral differences.  In general, we
// recommend 'bslmf_forwardingreftype' (the new component) in most contexts.
//
// Most notably, 'bslmf::ForwardingType' (the older class) forwards fundamental
// and pointer types by value, where as 'bslmf::ForwardingRefType' will forward
// fundamental and pointer types by const-reference.  For example,
// 'bslmf::ForwardingType<int>::Type' is 'int' where as
// 'bslmf::ForwardingRefType<int>::Type' is 'const int&'.  This applies to
// fundamental types, pointer types (including member-pointer types), and enum
// types (which we'll collectively call "basic types").  Forwarding these basic
// types by value was a performance optimization (and in some rare
// circumstances was hack needed by older compilers), which predated the
// standardization of many of the places where 'bslmf::ForwardingType' was used
// (function and bind components in particular).  The optimzation (potentially)
// being that passing an 'int' by value is more likely to be done through a
// register, where as passing by reference is more likely to require
// de-referencing memory.  Forwarding the types by const-reference, as the
// newer 'bslmf::ForwardingRefType' does', is generally simpler and more in
// line with the modern C++ standard.  Using 'bslmf::ForwardingRefType' avoids
// some awkward edge cases at the expense of a possible optimization in
// parameter passing.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Direct Look at Metafunction Results
/// - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we invoke 'ForwardingType' on a variety of types and look
// at the resulting 'Type' member:
//..
//  struct MyType {};
//  typedef MyType& MyTypeRef;
//
//  void main()
//      // Usage example.
//  {
//      typedef int                     T1;
//      typedef int&                    T2;
//      typedef const volatile double&  T3;
//      typedef const double &          T4;
//      typedef const float * &         T5;
//      typedef const float * const &   T6;
//      typedef MyType                  T7;
//      typedef const MyType&           T8;
//      typedef MyType&                 T9;
//      typedef MyType                 *T10;
//      typedef int                     T11[];
//      typedef int                     T12[3];
//
//      typedef int                     EXP1;
//      typedef int&                    EXP2;
//      typedef const volatile double&  EXP3;
//      typedef const double &          EXP4;
//      typedef const float * &         EXP5;
//      typedef const float * const &   EXP6;
//      typedef const MyType&           EXP7;
//      typedef const MyType&           EXP8;
//      typedef MyType&                 EXP9;
//      typedef MyType                 *EXP10;
//      typedef int                    *EXP11;
//      typedef int                    *EXP12;
//
//      assert((bsl::is_same<bslmf::ForwardingType<T1>::Type, EXP1>::value));
//      assert((bsl::is_same<bslmf::ForwardingType<T2>::Type, EXP2>::value));
//      assert((bsl::is_same<bslmf::ForwardingType<T3>::Type, EXP3>::value));
//      assert((bsl::is_same<bslmf::ForwardingType<T4>::Type, EXP4>::value));
//      assert((bsl::is_same<bslmf::ForwardingType<T5>::Type, EXP5>::value));
//      assert((bsl::is_same<bslmf::ForwardingType<T6>::Type, EXP6>::value));
//      assert((bsl::is_same<bslmf::ForwardingType<T7>::Type, EXP7>::value));
//      assert((bsl::is_same<bslmf::ForwardingType<T8>::Type, EXP8>::value));
//      assert((bsl::is_same<bslmf::ForwardingType<T9>::Type, EXP9>::value));
//      assert((bsl::is_same<bslmf::ForwardingType<T10>::Type, EXP10>::value));
//      assert((bsl::is_same<bslmf::ForwardingType<T11>::Type, EXP11>::value));
//      assert((bsl::is_same<bslmf::ForwardingType<T12>::Type, EXP12>::value));
//  }
//..
//
///Example 2: A Logging Invocation Wrapper
///- - - - - - - - - - - - - - - - - - - -
// This example illustrates the use of 'ForwardingType' to efficiently
// implement a wrapper class that holds a function pointer and logs
// information about each call to the pointed-to-function through the wrapper.
// Suppose the pointed-to-function takes three arguments whose types are
// specified via template arguments, where the first argument is required to
// be convertible to 'int'.
//
// First we create a wrapper class that holds a function pointer of the
// desired type:
//..
//  // Primary template is never defined.
//  template <class PROTOTYPE>
//  class LoggingWrapper;
//
//  template <class RET, class ARG1, class ARG2, class ARG3>
//  class LoggingWrapper<RET(ARG1, ARG2, ARG3)> {
//      // Specialization of wrapper for specified function prototype.
//
//      RET (*d_function_p)(ARG1, ARG2, ARG3);
//
//    public:
//      explicit LoggingWrapper(RET (*function_p)(ARG1, ARG2, ARG3))
//          // Create a 'LoggingWrapper' object for the specified 'function_p'
//          // function.
//        : d_function_p(function_p) { }
//..
// Then, we declare an overload of the function-call operator that actually
// invokes the wrapped function.  In order to avoid excessive copies of
// pass-by-value arguments, we use 'ForwardingType' to declare a more efficient
// intermediate argument type to forward to the wrapped function pointer:
//..
//      RET operator()(typename bslmf::ForwardingType<ARG1>::Type a1,
//                     typename bslmf::ForwardingType<ARG2>::Type a2,
//                     typename bslmf::ForwardingType<ARG3>::Type a3) const;
//          // Invoke the stored function pointer with the specified 'a1',
//          // 'a2', and 'a3' arguments, logging the invocation and returning
//          // the result of the function pointer invocation.
//  };
//..
// Next, we define logging functions that simply count the number of
// invocations and number of returns from invocations (e.g., to count how may
// invocations completed without exiting via exceptions):
//..
//  int invocations = 0, returns = 0;
//  void logInvocation(int /* ignored */) { ++invocations; }
//      // Log an invocation of the wrapped function.
//  void logReturn(int /* ignored */) { ++returns; }
//      // Log a return from the wrapped function.
//..
// Now, we implement 'operator()' to call the logging functions, either side of
// calling the logged function through the wrapped pointer.  To reconstitute
// the arguments to the function as close as possible to the types they were
// passed in as, we call the 'forwardToTarget' member of 'ForwardingTypeUtil':
//..
//  template <class RET, class ARG1, class ARG2, class ARG3>
//  RET LoggingWrapper<RET(ARG1, ARG2, ARG3)>::operator()(
//                       typename bslmf::ForwardingType<ARG1>::Type a1,
//                       typename bslmf::ForwardingType<ARG2>::Type a2,
//                       typename bslmf::ForwardingType<ARG3>::Type a3) const {
//      logInvocation(a1);
//      RET r = d_function_p(
//          bslmf::ForwardingTypeUtil<ARG1>::forwardToTarget(a1),
//          bslmf::ForwardingTypeUtil<ARG2>::forwardToTarget(a2),
//          bslmf::ForwardingTypeUtil<ARG3>::forwardToTarget(a3));
//      logReturn(a1);
//      return r;
//  }
//..
// Then, in order to see this wrapper in action, we must define a function we
// wish to wrap.  This function will take an argument of type 'ArgType' that
// holds an integer 'value' and keeps track of whether it has been directly
// constructed or copied from anther 'ArgType' object.  If it has been copied,
// it keeps track of how many "generations" of copy were made:
//..
//  class ArgType {
//      int d_value;
//      int d_copies;
//    public:
//      explicit ArgType(int v = 0) : d_value(v), d_copies(0) { }
//          // Create an 'ArgType' object.  Optionally specify 'v' as the
//          // initial value of this 'ArgType' object, otherwise this object
//          // will hold the value 0.
//
//      ArgType(const ArgType& original)
//          // Create an 'ArgType' object that is a copy of the specified
//          // 'original'.
//      : d_value(original.d_value)
//      , d_copies(original.d_copies + 1)
//      { }
//
//      int copies() const { return d_copies; }
//          // Return the number of copies that this object is from the
//          // original.
//
//      int value() const { return d_value; }
//          // Return the value of this object.
//  };
//
//  int myFunc(const short& i, ArgType& x, ArgType y)
//      // Assign the specified 'x' the value of the specified 'y' and return
//      // the 'value()' of 'x'.  Verify that the specified 'i' matches
//      // 'y.copies()'.  'x' is passed by reference in order to demonstrate
//      // forwarding of reference arguments.
//  {
//      assert(i == y.copies());
//      x = y;
//      return x.value();
//  }
//..
// Finally, we create a instance of 'LoggingWrapper' to wrap 'myFunc', and we
// invoke it.  Note that 'y' is copied into the second argument of 'operator()'
// and is copied again when 'myFunc' is invoked.  However, it is *not* copied
// when 'operator()' calls 'invoke()' because the 'ForwardType' of 'ArgType' is
// 'const ArgType&', which does not create another copy.  In C++11, if
// 'ArgType' had a move constructor, then the number of copies would be only 1,
// since the final forwarding would be a move instead of a copy.
//..
//  void usageExample2()
//      // Usage Example
//  {
//      ArgType x(0);
//      ArgType y(99);
//
//      LoggingWrapper<int(const short&, ArgType&, ArgType)> lw(myFunc);
//      assert(0 == invocations && 0 == returns);
//      lw(1, x, y);  // Expect exactly one copy of 'y'
//      assert(1 == invocations && 1 == returns);
//      assert(99 == x.value());
//  }
//..

#include <bslscm_version.h>

#include <bslmf_isarray.h>
#include <bslmf_isenum.h>
#include <bslmf_isfunction.h>
#include <bslmf_isfundamental.h>
#include <bslmf_ispointer.h>
#include <bslmf_ismemberpointer.h>
#include <bslmf_isreference.h>
#include <bslmf_isrvaluereference.h>
#include <bslmf_movableref.h>
#include <bslmf_removecv.h>
#include <bslmf_removereference.h>

#include <bsls_compilerfeatures.h>
#include <bsls_platform.h>

#include <stddef.h>

#if defined(BSLS_PLATFORM_CMP_IBM) &&                                         \
    ((BSLS_PLATFORM_CMP_VERSION  < 0x0c10) ||                                 \
     (BSLS_PLATFORM_CMP_VERSION == 0x0c10  && __xlC_ver__ < 0x00000013))
# define BSLMF_FORWARDINGTYPE_NO_INLINE
// THe IBM xlC compiler trips an ICE or infinite compile loop when certain
// functions are inlined for optimized builds.  This is resolved by the August
// 2017 PTF release, 12.1.0.19.
#endif

namespace BloombergLP {


namespace bslmf {

// FORWARD DECLARATIONS
template <class t_TYPE>
class ForwardingTypeUtil;

template <class t_TYPE, int k_CATEGORY>
struct ForwardingType_Imp;

                        // =============================
                        // class ForwardingType_Category
                        // =============================

struct ForwardingType_Category {
    // This component-private struct provides a namespace for thpe type
    // dispatch category enumeration values.

    // CONSTANTS
    enum {
        e_LVALUE_REF,  // Lvalue reference
        e_MOVABLE_REF, // Movable (rvalue) reference
        e_FUNCTION,    // Function or pointer to function
        e_ARRAY,       // Array
        e_BASIC,       // Built-in, pointer, or enum type
        e_CLASS        // Class, struct or union
    };
};

                        // =============================
                        // class ForwardingType_Dispatch
                        // =============================

template <class t_TYPE>
class ForwardingType_Dispatch {
    // This component-private class template is a metafunction whose 'value'
    // member is the forwarding category for the specified 't_TYPE'.

    // PRIVATE TYPES
    typedef ForwardingType_Category Cat;  // Abbreviation

  public:
    enum {
        value = (MovableRefUtil::IsLvalueReference<t_TYPE>::value
                     ? Cat::e_LVALUE_REF
                 : MovableRefUtil::IsMovableReference<t_TYPE>::value
                     ? Cat::e_MOVABLE_REF
                 : bsl::is_function<t_TYPE>::value       ? Cat::e_FUNCTION
                 : bsl::is_array<t_TYPE>::value          ? Cat::e_ARRAY
                 : bsl::is_fundamental<t_TYPE>::value    ? Cat::e_BASIC
                 : bsl::is_pointer<t_TYPE>::value        ? Cat::e_BASIC
                 : bsl::is_member_pointer<t_TYPE>::value ? Cat::e_BASIC
                 : bsl::is_enum<t_TYPE>::value           ? Cat::e_BASIC
                                                         : Cat::e_CLASS)
    };
};

                        // ====================
                        // class ForwardingType
                        // ====================

template <class t_TYPE>
class ForwardingType
: private ForwardingType_Imp<t_TYPE, ForwardingType_Dispatch<t_TYPE>::value> {
    // This template metafunction has a member 'Type' computed such that, for a
    // specified 't_TYPE' parameter, a function with argument of 't_TYPE' can
    // be called efficiently from another function (e.g., a wrapper) by
    // declaring the corresponding parameter of the other wrapper as 'typename
    // ForwardingType<t_TYPE>::Type'.  The 'Type' member is computed to
    // minimize the number of expensive copies while forwarding the arguments
    // as faithfully as possible.

    // PRIVATE TYPES
    typedef ForwardingType_Imp<t_TYPE, ForwardingType_Dispatch<t_TYPE>::value>
        Imp;

  public:
    // TYPES
    typedef typename Imp::Type Type;
        // The type that should be used to forward 't_TYPE' through a chain of
        // function calls.

    typedef typename Imp::TargetType TargetType;
        // The closest type used to "reconstitute" 't_TYPE' from
        // 'ForwardingType<t_TYPE>::Type'.  This type may differ from 't_TYPE'
        // through the addition of a reference.
};

                        // ========================
                        // class ForwardingTypeUtil
                        // ========================

template <class t_TYPE>
class ForwardingTypeUtil
: private ForwardingType_Imp<t_TYPE, ForwardingType_Dispatch<t_TYPE>::value> {
    // Provide a namespace for the 'forwardToTarget' function.

    // PRIVATE TYPES
    typedef ForwardingType_Imp<t_TYPE, ForwardingType_Dispatch<t_TYPE>::value>
        Imp;

  public:
    // TYPES
    typedef typename Imp::TargetType TargetType;
        // The closest type used to "reconstitute" 't_TYPE' from
        // 'ForwardingType<t_TYPE>::Type'.  This type may differ from 't_TYPE'
        // through the addition of a reference.

    // CLASS METHODS static TargetType forwardToTarget(typename
    // ForwardingType<t_TYPE>::Type v);
    using Imp::forwardToTarget;
        // Return (for the specified 'v' parameter) 'std::forward<t_TYPE>(v)',
        // where 'v' is assumed to originally have been an argument of 't_TYPE'
        // after forwarding through an intermediate call chain.  Specifically,
        // if 't_TYPE' is an rvalue type, return an rvalue reference to 'v',
        // otherwise return 'v' unchanged, thus converting an rvalue copy into
        // an rvalue move when possible.  For compilers that do not support
        // rvalue references, return 'v' unchanged.  This function is intended
        // to be called to forward an argument to the final target function of
        // a forwarding call chain.  Note that this function is not intended
        // for use with 't_TYPE' parameters of 'volatile'-qualified rvalue
        // type, which are effectively unheard of in real code and have strange
        // and hard-to-understand rules.
};

                        // =========================
                        // class ConstForwardingType
                        // =========================

#ifndef BDE_OMIT_DEPRECATED

template <class t_TYPE>
struct ConstForwardingType : public ForwardingType<t_TYPE> {
    //!DEPRECATED!: Use 'ForwardingType' instead.
};

#endif // BDE_OMIT_DEPRECATED

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

// BDE_VERIFY pragma: push  // Relax some bde_verify rules in the imp section
// BDE_VERIFY pragma: -CD01 // Member function defined in class definition

                        // ========================
                        // class ForwardingType_Imp
                        // ========================

// PRIMARY TEMPLATE HAS NO DEFINITION

// PARTIAL SPECIALIZATIONS
template <class t_TYPE>
struct ForwardingType_Imp<t_TYPE, ForwardingType_Category::e_LVALUE_REF> {
    // lvalue reference is forwarded unmodified.

    // TYPES
    typedef t_TYPE             Type;
    typedef t_TYPE             TargetType;

    // CLASS METHODS
    static TargetType forwardToTarget(Type v)
        // Return the specified 'v' argument.
    {
        return v;
    }
};

template <class t_TYPE>
struct ForwardingType_Imp<t_TYPE, ForwardingType_Category::e_MOVABLE_REF> {
    // Rvalue reference is forwarded as a reference to const lvalue.

    // TYPES
    typedef typename MovableRefUtil::RemoveReference<t_TYPE>::type UnrefType;
    typedef const UnrefType&                                       Type;
    typedef t_TYPE                                                 TargetType;

    // CLASS METHODS
    static TargetType forwardToTarget(Type v)
    {
        // Since rvalues are forwarded as *const* lvalues, we must cast away
        // the constness before converting to an rvalue reference.  If 't_TYPE'
        // is a const reference, then the constness will be reinstated on
        // return.

        // We split this cast up into two lines because Visual Studio 2015 and
        // early versions of Visual Studio 2017 create a temporary in the
        // one-liner.
        UnrefType& result = const_cast<UnrefType&>(v);
        return MovableRefUtil::move(result);
    }
};

template <class t_TYPE>
struct ForwardingType_Imp<t_TYPE, ForwardingType_Category::e_FUNCTION> {
    // Function type and is forwarded as function reference.

    // TYPES
    typedef t_TYPE& Type;
    typedef t_TYPE& TargetType;

    // CLASS METHODS
    static TargetType forwardToTarget(Type v)
        // Return the specified 'v' argument.
    {
        return v;
    }
};

template <class t_TYPE, size_t k_NUM_ELEMENTS>
struct ForwardingType_Imp<t_TYPE[k_NUM_ELEMENTS],
                          ForwardingType_Category::e_ARRAY> {
    // Array of known size and reference to array of known size is forwarded as
    // pointer to array element type.

    // TYPES
    typedef t_TYPE *Type;
    typedef t_TYPE (&TargetType)[k_NUM_ELEMENTS];

    // CLASS METHODS
    static TargetType forwardToTarget(Type v)
        // Return the specified 'v', cast to a reference to array.
    {
        return reinterpret_cast<TargetType>(*v);
    }
};

template <class t_TYPE>
struct ForwardingType_Imp<t_TYPE[], ForwardingType_Category::e_ARRAY> {
    // Array of unknown size and reference to array of unknown size is
    // forwarded as pointer to array element type.

    // TYPES
    typedef t_TYPE *Type;
    typedef t_TYPE (&TargetType)[];

    // CLASS METHODS
    static TargetType forwardToTarget(Type v)
        // Return the specified 'v' argument cast to a reference to array of
        // unknown size.
    {
        return reinterpret_cast<TargetType>(*v);
    }
};

template <class t_TYPE>
struct ForwardingType_Imp<t_TYPE, ForwardingType_Category::e_BASIC> {
    // Rvalue of basic type is forwarded with any cv-qualifier removed.

    // TYPES
    typedef typename bsl::remove_cv<t_TYPE>::type Type;
    typedef t_TYPE                              TargetType;

    // CLASS METHODS
    static TargetType& forwardToTarget(Type& v)
        // Return the specified 'v' argument with cv qualifiers added to match
        // the specified 'UNREF_TYPE'.  The return type is a reference to a
        // (possibly modifiable) 't_TYPE' object in order to work around
        // certain dpkg bugs where an rvalue is being forwarded to a function
        // that expects an lvalue.
    {
        return v;
    }
};

template <class t_TYPE>
struct ForwardingType_Imp<t_TYPE, ForwardingType_Category::e_CLASS> {
    // Rvalue of user type (i.e., class or union) is forwarded as a const
    // reference.

    // TYPES
    typedef ForwardingType_Imp Imp;
    typedef const t_TYPE&      Type;
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    typedef MovableRef<t_TYPE>   TargetType;

    static TargetType forwardToTarget(Type v)
        // Return the specified 'v' argument cast to a modifiable movable
        // reference.
    {
        // Since rvalues are forwarded as *const* lvalues, we must cast away
        // the constness before converting to an rvalue reference.  If 't_TYPE'
        // is a const reference, then the constness will be reinstated on
        // return.

# if defined(BSLS_PLATFORM_CMP_MSVC)
        // We use a C-style cast because Visual Studio 2013, 2015, and early
        // versions of Visual Studio 2017 create a temporary with various
        // formulations using C++ casts.
        return MovableRefUtil::move((t_TYPE&)(v));
#else
        // However, other platforms are known to complain about casting away
        // the 'const' qualifier in 'Type' (i.e., in 'const t_TYPE&') unless a
        // 'const_cast' is explicitly used.
        return MovableRefUtil::move(const_cast<t_TYPE&>(v));
# endif
    }
#else
    typedef const t_TYPE&        TargetType;

    static TargetType forwardToTarget(Type v)
    {
        return v;
    }
#endif
};

}  // close package namespace
}  // close enterprise namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

#ifdef bslmf_ForwardingType
#undef bslmf_ForwardingType
#endif
#define bslmf_ForwardingType bslmf::ForwardingType
    // !DEPRECATED! This alias is defined for backward compatibility.
#endif  // BDE_OPENSOURCE_PUBLICATION -- BACKWARD_COMPATIBILITY

#ifdef bslmf_ConstForwardingType
#undef bslmf_ConstForwardingType
#endif
// BDE_VERIFY pragma: -SLM01 // Allow non-standard macro to leak from header
#define bslmf_ConstForwardingType bslmf::ConstForwardingType
    // !DEPRECATED! This alias is defined for backward compatibility.

// BDE_VERIFY pragma: pop // Restore bde_verify rules

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
