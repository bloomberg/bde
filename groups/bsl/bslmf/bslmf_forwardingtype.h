// bslmf_forwardingtype.h                                             -*-C++-*-
#ifndef INCLUDED_BSLMF_FORWARDINGTYPE
#define INCLUDED_BSLMF_FORWARDINGTYPE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for determining an optimal forwarding type.
//
//@CLASSES:
//  bslmf::ForwardingType: meta-function to determine optimal forwarding type
//  bslmf::ForwardingTypeUtil: Namespace for forwarding functions
//
//@SEE_ALSO: bslmf_removecvq
//
//@AUTHORS: Ilougino Rocha (irocha), Pablo Halpern (phalpern)
//
//@DESCRIPTION: This component provides a meta function determining the most
// efficient forwarding type for a given template type 'TYPE'.  The forwarding
// type is used to pass an argument from the client of a component through a
// chain of nested function calls to the ultimate consumer of the argument.
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
// instantiated on the same pointer type, regardless of array size.
//
// An argument 'v' of type 'T' can be passed as type 'ForwardingType<T>::Type'
// down an arbitrarily-long chain of function calls without every calling
// 'std::forward'. However, in order to avoid an extra copy as well as to
// select the correct overload and instantiation of the eventual target
// function, it should be converted back to a type that more closely resembles
// the original 'T' by calling 'ForwardingTypeUtil<T>::forwardToTarget(v)'.
//
// This component is intended to be used when performance is of highest
// concern or when creating function wrappers that are intended to minimize
// perturbations on the interface of the functions that they wrap.
//
// Note that previous versions of this component forwarded const references to
// basic types by value instead of by const reference.  This transformation
// was an attempt to avoid an extra dereference operation, but in real use
// cases the extra dereference happened anyway, on the call to the outermost
// forwarding function.  Moreover, such a transformation is subtly broken
// because, in the rare case where the target function cares about the address
// of the reference (e.g., if it compares it to some known address), it would
// wind up with the address of a temporary copy, rather than the address of
// the original argument.  Thus, the current component forward references as
// references in call cases, including for basic types.
//
///Usage
///-----
// These examples demonstrate the expected use of this component.
// In this section we show intended use of this component.
//
///Example 1: Direct look at metafunction results
///- - - - - - - - - - - - - - - - - - - - - - -
// In this example, we invoke 'ForwardingType' on a variety of types and look
// at the resulting 'Type' member:
//..
//  struct MyType {};
//  typedef MyType& MyTypeRef;
//
//  void main()
//  {
//      typedef int                    T1;
//      typedef int&                   T2;
//      typedef const volatile double& T3;
//      typedef const double &         T4;
//      typedef const float * &        T5;
//      typedef const float * const &  T6;
//      typedef MyType                 T7;
//      typedef const MyType&          T8;
//      typedef MyType&                T9;
//      typedef MyType*                T10;
//
//      typedef int                    EXP1;
//      typedef int&                   EXP2;
//      typedef const volatile double& EXP3;
//      typedef const double &         EXP4;
//      typedef const float * &        EXP5;
//      typedef const float * const &  EXP6;
//      typedef const MyType&          EXP7;
//      typedef const MyType&          EXP8;
//      typedef MyType&                EXP9;
//      typedef MyType*                EXP10;
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
//  }
//..
//
///Example 2: A logging invocation wrapper
///- - - - - - - - - - - - - - - - - - - - - - -
// This example illustrates the use of 'ForwardingType' to efficiently
// implement a wrapper class that holds a function pointer and logs
// information about each call to the pointed-to-function through the wrapper.
// The pointed-to-function takes three arguments whose types a4re specified
// via template arguments.  The first argument is required to be convertible
// to 'int'.  The class definition looks as follows:
//..
//  // Primary template is never defined
//  template <class PROTOTYPE> class LoggingWrapper;
//
//  template <class RET, class ARG1, class ARG2, class ARG3>
//  class LoggingWrapper<RET(ARG1, ARG2, ARG3)> {
//      // Specialization of wrapper for specified function prototype.
//
//      RET (*d_function_p)(ARG1, ARG2, ARG3);
//
//  public:
//      LoggingWrapper(RET (*function_p)(ARG1, ARG2, ARG3))
//        : d_function_p(function_p) { }
//
//      RET operator()(ARG1, ARG2, ARG3) const;
//..
// Next, we declare a private member function that actually invokes the
// function. This member function will be called by 'operator()' and must
// therefore receive arguments indirectly through 'operator()'. In order to
// avoid excessive copies of pass-by-value arguments, we use 'ForwardingType'
// to declare a more efficient intermediate argument type for our private
// member function:
//..
//  private:
//      RET invoke(typename bslmf::ForwardingType<ARG1>::Type a1,
//                 typename bslmf::ForwardingType<ARG2>::Type a2,
//                 typename bslmf::ForwardingType<ARG3>::Type a3) const;
//  };
//..
// Next, we define logging functions that simply count the number of
// invocations and return from invocations (e.g., to count how may invocations
// completed without exceptions):
//..
//  int invocations = 0, returns = 0;
//  void logInvocation(int /* ignored */) { ++invocations; }
//  void logReturn(int /* ignored */) { ++returns; }
//..
// Next, we implement 'operator()' to call the logging functions and call
// 'invoke()':
//..
//  template <class RET, class ARG1, class ARG2, class ARG3>
//  RET LoggingWrapper<RET(ARG1, ARG2, ARG3)>::operator()(ARG1 a1,
//                                                        ARG2 a2,
//                                                        ARG3 a3) const {
//      logInvocation(a1);
//      RET r = invoke(a1, a2, a3);
//      logReturn(a1);
//      return r;
//  }
//..
// Next, we implement 'invoke()' to actually call the function through the
// pointer. To reconstitute the arguments to the function as close as possible
// to the types they were passed in as, we call the 'forwardToTarget' member
// of 'ForwardingTypeUtil':
//..
//  template <class RET, class ARG1, class ARG2, class ARG3>
//  RET LoggingWrapper<RET(ARG1,ARG2,ARG3)>::invoke(
//      typename bslmf::ForwardingType<ARG1>::Type a1,
//      typename bslmf::ForwardingType<ARG2>::Type a2,
//      typename bslmf::ForwardingType<ARG3>::Type a3) const
//  {
//      return d_function_p(
//          bslmf::ForwardingTypeUtil<ARG1>::forwardToTarget(a1),
//          bslmf::ForwardingTypeUtil<ARG2>::forwardToTarget(a2),
//          bslmf::ForwardingTypeUtil<ARG3>::forwardToTarget(a3));
//  }
//..
// Next, in order to see this wrapper in action, we must define the function
// we wish to wrap.  This function will take an argument of type 'ArgType',
// which, among other things, keeps track of whether it has been directly
// constructed or copied from anther 'ArgType' object.  If it has been copied,
// it keeps track of how many "generations" of copy were done:
//..
//  class ArgType {
//      int d_value;
//      int d_copies;
//  public:
//      ArgType(int v = 0) : d_value(v), d_copies(0) { }
//      ArgType(const ArgType& other)
//        : d_value(other.d_value)
//        , d_copies(other.d_copies + 1) { }
//
//      int value() const { return d_value; }
//      int copies() const { return d_copies; }
//  };
//
//  int myFunc(const short& i, ArgType& x, ArgType y)
//      // Assign 'x' the value of 'y' and return the 'value()' of 'x'.
//  {
//      assert(i == y.copies());
//      x = y;
//      return x.value();
//  }
//..
// Finally, we create a instance of 'LoggingWrapper' to wrap 'myFunc', and we
// invoke it.  Note that 'y' is copied into the second argument of
// 'operator()' and is copied again when 'myFunc' is invoked.  However, it is
// *not* copied when 'operator()' calls 'invoke()' because the 'ForwardType'
// of 'ArgType' is 'const ArgType&', which does not create another copy.  In
// C++11, if 'ArgType' had a move constructor, then the number of copies would
// be only 1, since the final forwarding would be a move instead of a copy.
//..
//  void main() {
//      ArgType x(0);
//      ArgType y(99);
//
//      LoggingWrapper<int(const short&, ArgType&, ArgType)> lw(myFunc);
//      assert(0 == invocations && 0 == returns);
//      lw(2, x, y);  // Expect two copies of 'y'
//      assert(1 == invocations && 1 == returns);
//      assert(99 == x.value());
//  }
//..    

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ISENUM
#include <bslmf_isenum.h>
#endif

#ifndef INCLUDED_BSLMF_ISFUNDAMENTAL
#include <bslmf_isfundamental.h>
#endif

#ifndef INCLUDED_BSLMF_ISPOINTER
#include <bslmf_ispointer.h>
#endif

#ifndef INCLUDED_BSLMF_ISFUNCTION
#include <bslmf_isfunction.h>
#endif

#ifndef INCLUDED_BSLMF_ISARRAY
#include <bslmf_isarray.h>
#endif

#ifndef INCLUDED_BSLMF_ISPOINTERTOMEMBER
#include <bslmf_ispointertomember.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECVQ
#include <bslmf_removecvq.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVEREFERENCE
#include <bslmf_removereference.h>
#endif

#ifndef INCLUDED_BSLMF_CONDITIONAL
#include <bslmf_conditional.h>
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>       // 'std::size_t'
#define INCLUDED_CSTDDEF
#endif

namespace BloombergLP {


namespace bslmf {

enum {
    // Type categories
    BSLMF_FORWARDING_TYPE_FUNCTION,   // Function or pointer to function
    BSLMF_FORWARDING_TYPE_ARRAY,      // Array
    BSLMF_FORWARDING_TYPE_RVALUE_REF, // Rvalue reference
    BSLMF_FORWARDING_TYPE_BASIC,      // Built-in, pointer, or enum type
    BSLMF_FORWARDING_TYPE_CLASS       // Class, struct or union
};

template <class TYPE>
struct ForwardingTypeUtil;

template <class TYPE, int CATEGORY, bool IS_REFERENCE>
struct ForwardingType_Imp;

                        // ====================
                        // class ForwardingType
                        // ====================

template <class TYPE>
struct ForwardingType {
    // This template metafunction has a member 'Type' computed such that is used to specialize 'TYPE' such that arguments of type
    // 'TYPE' can be efficiently forwarded by reference or pointer.

    // Inheritance is used just to make the 'ForwardingType_Category' enum
    // visible without qualification.

private:
    typedef typename bsl::remove_reference<TYPE>::type UnrefType;

    enum {
        IS_REFERENCE = bsl::is_reference<TYPE>::value,

        CATEGORY = (bsl::is_function<UnrefType>::value    ?
                                             BSLMF_FORWARDING_TYPE_FUNCTION   :
                    bsl::is_array<UnrefType>::value       ?
                                             BSLMF_FORWARDING_TYPE_ARRAY      :
                    bsl::is_rvalue_reference<TYPE>::value ?
                                             BSLMF_FORWARDING_TYPE_RVALUE_REF :
                    bsl::is_fundamental<TYPE>::value ||
                    bsl::is_pointer<TYPE>::value ||
                    bsl::is_member_pointer<TYPE>::value ||
                    IsFunctionPointer<TYPE>::value ||
                    bsl::is_enum<TYPE>::value             ?
                                             BSLMF_FORWARDING_TYPE_BASIC      :
                                             BSLMF_FORWARDING_TYPE_CLASS)
    };

    typedef ForwardingType_Imp<UnrefType, CATEGORY, IS_REFERENCE> Imp;

    friend struct ForwardingTypeUtil<TYPE>;

public:

    enum {
        // 'BSLMF_FORWARDING_TYPE_ID' is the composed of the type category for
        // non-reference types and the negation of the type category for
        // reference types.
        BSLMF_FORWARDING_TYPE_ID = (IS_REFERENCE ? -CATEGORY : CATEGORY)
    };

    typedef typename Imp::Type       Type;
        // The type that should be used to forward 'TYPE' through a chain of
        // function calls.

    typedef typename Imp::TargetType TargetType;
        // The closest type used to "reconstitute" 'TYPE' from
        // 'ForwardingType<TYPE>::Type'.  This type may differ from 'TYPE'
        // through the addition of a reference.
};

                        // ========================
                        // class ForwardingTypeUtil
                        // ========================

template <class TYPE>
struct ForwardingTypeUtil {
    // Provide a namespace for the 'forwardToTarget' function.

    typedef typename ForwardingType<TYPE>::Imp::TargetType TargetType;

    static TargetType
    forwardToTarget(typename ForwardingType<TYPE>::Type v);
        // Return 'std::forward<TYPE>(v)' for the specified 'v' argument of
        // type 'ForwardingType<TYPE>::Type', where 'v' is assumed to
        // originally have been an argument of 'TYPE' after forwarding through
        // an intermediate call chain.  Specifically, if 'TYPE' is an rvalue
        // type, return an rvalue reference to 'v', otherwise return 'v'
        // unchanged, thus converting an rvalue copy into an rvalue move when
        // possible.  For compilers that do not supprt rvalue references,
        // return 'v' unchanged.  This function is intended to be called to
        // forward an argument to the final target function of a forwarding
        // call chain.
};

                        // =========================
                        // class ConstForwardingType
                        // =========================

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

template <class TYPE>
struct ConstForwardingType : public ForwardingType<TYPE> {
    // @DEPRECATED
    // Use 'ForwardingType' instead.
};

#endif // BDE_OMIT_INTERNAL_DEPRECATED

// ===========================================================================
//                           IMPLEMENTATION
// ===========================================================================

template <class TYPE>
inline typename ForwardingTypeUtil<TYPE>::TargetType
ForwardingTypeUtil<TYPE>::forwardToTarget(
                                         typename ForwardingType<TYPE>::Type v)
{
    return ForwardingType<TYPE>::Imp::forwardToTarget(v);
}

                        // ========================
                        // class ForwardingType_Imp
                        // ========================

// PRIMARY TEMPLATE HAS NO DEFINITION

// PARTIAL SPECIALIZATIONS
template <class UNREF_TYPE>
struct ForwardingType_Imp<UNREF_TYPE,
                          BSLMF_FORWARDING_TYPE_RVALUE_REF, true>
{
    // Rvalue reference is forwarded as a reference to const lvalue.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    typedef const UNREF_TYPE& Type;
    typedef UNREF_TYPE&&      TargetType;
    static TargetType forwardToTarget(Type v) {
        // Since rvalues are forwarded as *const* lvalues, we must cast away
        // the constness before converting to an rvalue reference.  If 'TYPE'
        // is a const reference, then the constness will be re-instated on
        // return.
        return static_cast<TargetType>(const_cast<UNREF_TYPE&>(v));
    }
#endif
};

template <class UNREF_TYPE, bool IS_REFERENCE>
struct ForwardingType_Imp<UNREF_TYPE,
                          BSLMF_FORWARDING_TYPE_FUNCTION, IS_REFERENCE>
{
    // Function and function reference is forwarded as function reference.

    typedef UNREF_TYPE& Type;
    typedef UNREF_TYPE& TargetType;
    static TargetType forwardToTarget(Type v) { return v; }
};

template <class UNREF_TYPE, std::size_t NUM_ELEMENTS, bool IS_REFERENCE>
struct ForwardingType_Imp<UNREF_TYPE [NUM_ELEMENTS],
                          BSLMF_FORWARDING_TYPE_ARRAY, IS_REFERENCE>
{
    // Array of known size and reference to array of known size is forwarded
    // as pointer to array element type.

    typedef UNREF_TYPE  *Type;
    typedef UNREF_TYPE (&TargetType)[NUM_ELEMENTS];
    static TargetType forwardToTarget(Type v) {
        return reinterpret_cast<TargetType>(*v);
    }
};

template <class UNREF_TYPE, bool IS_REFERENCE>
struct ForwardingType_Imp<UNREF_TYPE [],
                          BSLMF_FORWARDING_TYPE_ARRAY, IS_REFERENCE> {
    // Array of unknown size and reference to array of unknown size is
    // forwarded as pointer to array element type.

    typedef UNREF_TYPE *Type;
    typedef UNREF_TYPE *TargetType;
    static TargetType forwardToTarget(Type v) {
        return reinterpret_cast<TargetType>(*v);
    }
};

template <class UNREF_TYPE>
struct ForwardingType_Imp<UNREF_TYPE,
                          BSLMF_FORWARDING_TYPE_BASIC, false> {
    // Rvalue of basic type is forwarded with cvq removed.

    typedef typename bsl::remove_cv<UNREF_TYPE>::type Type;
    typedef UNREF_TYPE                                TargetType;
    static TargetType forwardToTarget(Type v) { return v; }
};

template <class UNREF_TYPE>
struct ForwardingType_Imp<UNREF_TYPE,
                          BSLMF_FORWARDING_TYPE_BASIC, true> {
    // Lvalue reference to basic type is forwarded unchanged.

    typedef UNREF_TYPE& Type;
    typedef UNREF_TYPE& TargetType;
    static TargetType forwardToTarget(Type v) { return v; }
};

template <class UNREF_TYPE>
struct ForwardingType_Imp<UNREF_TYPE,
                          BSLMF_FORWARDING_TYPE_CLASS, false> {
    // Rvalue of user type (i.e., class or union) is forwarded as a const
    // reference.

    typedef const UNREF_TYPE& Type;
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    typedef UNREF_TYPE&&      TargetType;
    static TargetType forwardToTarget(Type v) {
        // Since rvalues are forwarded as *const* lvalues, we must cast away
        // the constness before converting to an rvalue reference.  If 'TYPE'
        // is a const reference, then the constness will be re-instated on
        // return.
        return static_cast<TargetType>(const_cast<UNREF_TYPE&>(v));
    }
#else
    typedef const UNREF_TYPE& TargetType;
    static TargetType forwardToTarget(Type v) { return v; }
#endif
};

template <class UNREF_TYPE>
struct ForwardingType_Imp<UNREF_TYPE,
                          BSLMF_FORWARDING_TYPE_CLASS, true> {
    typedef UNREF_TYPE& Type;
    typedef UNREF_TYPE& TargetType;
    static TargetType forwardToTarget(Type v) { return v; }
};

}  // close package namespace

#ifndef BDE_OMIT_TRANSITIONAL  // BACKWARD_COMPATIBILITY
// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

#ifdef bslmf_ConstForwardingType
#undef bslmf_ConstForwardingType
#endif
#define bslmf_ConstForwardingType bslmf::ConstForwardingType
    // This alias is defined for backward compatibility.

#ifdef bslmf_ForwardingType
#undef bslmf_ForwardingType
#endif
#define bslmf_ForwardingType bslmf::ForwardingType
    // This alias is defined for backward compatibility.
#endif  // BDE_OMIT_TRANSITIONAL -- BACKWARD_COMPATIBILITY

}  // close enterprise namespace

#endif

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
