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
// This component is intended to be used except when performance is of highest
// concern or when creating function wrappers that are intended to minimize
// perturbations on the interface of the functions that they wrap.
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
// to the types they were passed in as, we call the 'finalForward' member of
// 'ForwardingType':
//..
//  template <class RET, class ARG1, class ARG2, class ARG3>
//  RET LoggingWrapper<RET(ARG1,ARG2,ARG3)>::invoke(
//      typename bslmf::ForwardingType<ARG1>::Type a1,
//      typename bslmf::ForwardingType<ARG2>::Type a2,
//      typename bslmf::ForwardingType<ARG3>::Type a3) const
//  {
//      return d_function_p(bslmf::ForwardingType<ARG1>::finalForward(a1),
//                          bslmf::ForwardingType<ARG2>::finalForward(a2),
//                          bslmf::ForwardingType<ARG3>::finalForward(a3));
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

struct ForwardingType_Category {
    enum {
        RVALUE_REF_TYPE,
        FUNCTION_TYPE,
        ARRAY_TYPE,
        BASIC_TYPE,
        USER_TYPE
    };
};

template <class TYPE, int CATEGORY, bool IS_REFERENCE>
struct ForwardingType_Imp;


                        // ====================
                        // class ForwardingType
                        // ====================

template <class TYPE>
struct ForwardingType : ForwardingType_Category {
    // This template is used to specialize 'TYPE' such that arguments of type
    // 'TYPE' can be efficiently forwarded by reference or pointer.

    // Inheritance is used just to make the 'ForwardingType_Category' enum
    // visible without qualification.

private:
    typedef typename bsl::remove_reference<TYPE>::type UnrefType;

    enum {
        IS_REFERENCE = bsl::is_reference<TYPE>::value,

        CATEGORY = (bsl::is_rvalue_reference<TYPE>::value ? RVALUE_REF_TYPE :
                    bsl::is_function<UnrefType>::value    ? FUNCTION_TYPE   :
                    bsl::is_array<UnrefType>::value       ? ARRAY_TYPE      :
                    bsl::is_fundamental<TYPE>::value ||
                    bsl::is_pointer<TYPE>::value ||
                    bsl::is_member_pointer<TYPE>::value ||
                    IsFunctionPointer<TYPE>::value ||
                    bsl::is_enum<TYPE>::value             ? BASIC_TYPE      :
                    USER_TYPE)
    };

    typedef ForwardingType_Imp<UnrefType, CATEGORY, IS_REFERENCE> Imp;

public:

    enum { BSLMF_FORWARDING_TYPE_ID = CATEGORY * (IS_REFERENCE ? -1 : 1) };

    typedef typename Imp::Type Type;

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    // If 'TYPE' is an rvalue of user type (class or union), then the final
    // forwarding is a move operation.
    typedef typename bsl::conditional<
            (int) CATEGORY == (int) USER_TYPE && ! IS_REFERENCE,
            Type&&, Type
        >::type FinalType;
#else
    typedef Type FinalType;
#endif

    static FinalType finalForward(Type v) {
        typedef typename bsl::remove_cv<Type>::type UnconstType;
        return static_cast<FinalType>(const_cast<UnconstType>(v));
    }
};


                        // =========================
                        // class ConstForwardingType
                        // =========================

template <class TYPE>
struct ConstForwardingType : public ForwardingType<const TYPE> {
    // @DEPRECATED
    // Use 'ForwardingType' instead.
};

                        // ========================
                        // class ForwardingType_Imp
                        // ========================

// PRIMARY TEMPLATE HAS NO DEFINITION

// PARTIAL SPECIALIZATIONS
template <class UNREF_TYPE>
struct ForwardingType_Imp<UNREF_TYPE,
                          ForwardingType_Category::RVALUE_REF_TYPE, true>
{
    // Rvalue reference is forwarded unchanged.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    typedef UNREF_TYPE&& Type;
#endif
};

template <class UNREF_TYPE, bool IS_REFERENCE>
struct ForwardingType_Imp<UNREF_TYPE,
                          ForwardingType_Category::FUNCTION_TYPE, IS_REFERENCE>
{
    // Function and function reference is forwarded as function reference.

    typedef UNREF_TYPE& Type;
};

template <class UNREF_TYPE, std::size_t NUM_ELEMENTS, bool IS_REFERENCE>
struct ForwardingType_Imp<UNREF_TYPE [NUM_ELEMENTS],
                          ForwardingType_Category::ARRAY_TYPE, IS_REFERENCE>
{
    // Array of known size and reference to array of known size is forwarded
    // as pointer to array element type.

    typedef UNREF_TYPE *Type;
};

template <class UNREF_TYPE, bool IS_REFERENCE>
struct ForwardingType_Imp<UNREF_TYPE [],
                          ForwardingType_Category::ARRAY_TYPE, IS_REFERENCE> {
    // Array of unknown size and reference to array of unknown size is
    // forwarded as pointer to array element type.

    typedef UNREF_TYPE *Type;
};

template <class UNREF_TYPE>
struct ForwardingType_Imp<UNREF_TYPE,
                          ForwardingType_Category::BASIC_TYPE, false> {
    // Rvalue of basic type is forwarded with cvq removed.

    typedef typename bsl::remove_cv<UNREF_TYPE>::type Type;
};

template <class UNREF_TYPE>
struct ForwardingType_Imp<UNREF_TYPE,
                          ForwardingType_Category::BASIC_TYPE, true> {
    // Lvalue reference to basic type is forwarded unchanged.

    typedef UNREF_TYPE& Type;
};

template <class UNREF_TYPE>
struct ForwardingType_Imp<UNREF_TYPE,
                          ForwardingType_Category::USER_TYPE, false> {
    // Rvalue of user type (i.e., class or union) is forwarded as a const
    // reference.

    typedef const UNREF_TYPE& Type;
};

template <class UNREF_TYPE>
struct ForwardingType_Imp<UNREF_TYPE,
                          ForwardingType_Category::USER_TYPE, true> {
    typedef UNREF_TYPE& Type;
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
// Copyright (C) 2013 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
