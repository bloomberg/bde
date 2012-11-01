// bdef_memfn.h                    -*-C++-*-
#ifndef INCLUDED_BDEF_MEMFN
#define INCLUDED_BDEF_MEMFN

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide member function pointer wrapper classes and utility.
//
//@CLASSES:
//          bdef_MemFn: member function wrapper
//  bdef_MemFnInstance: member function wrapper with embedded instance pointer
//      bdef_MemFnUtil: utility for constructing wrapper objects
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@SEE_ALSO: bdef_function, bdef_bind
//
//@DESCRIPTION: This component provides a member function pointer wrapper
// that wraps a member function pointer such that it can be invoked in
// syntactically the same manner as a free function.  Two wrappers, each
// supporting member function pointers that accept from zero to fourteen
// arguments, are provided, as well as a utility to create such wrappers.
// Member function wrappers are commonly used as function objects for standard
// algorithms.
//
// The first wrapper, 'bdef_MemFn', contains a member function pointer and must
// be invoked with the first argument being a pointer or reference to the
// instance on which the function should be invoked, with the remaining
// arguments passed as arguments to the member function; that is, a wrapper
// 'memFn' containing a pointer to a given 'memberFunction' can be invoked as
// 'memFn(&object, args...)' as opposed to 'object.memberFunction(args...)'.
//
// The second wrapper, 'bdef_MemFnInstance', contains both the member function
// pointer and a pointer to an instance of the type which contains the member,
// and is invoked with arguments which are passed as arguments to the member
// function; that is, a wrapper 'memFnInstance' containing pointers to both a
// given 'memberFunction' and to an 'object' instance can be invoked as
// 'memFnInstance(args...)' as opposed to 'object.memberFunction(args...)'.
//
// Finally, the 'bdef_MemFnUtil' utility class provides utility functions for
// constructing 'bdef_MemFn' and 'bdef_MemFnInstance' objects.
//
///Usage
///-----
///Basic Usage
///- - - - - -
// To illustrate basic usage more concretely, let us introduce a generic type:
//..
//  class MyObject {
//    public:
//      void doSomething(int, const char *);
//  };
//..
// The following function invokes the member function 'doSomething' on the
// specified 'objectPtr', with the two arguments 100 and "Hello", in two
// different ways.  In both cases, 'object' is passed as parameter to a
// function, and a wrapper is built containing a pointer to the 'doSomething'
// member function.  In the 'bdef_MemFn' case, the wrapper can be built once,
// so we make it a 'static' local variable:
//..
//  void doSomethingWithMemFn(MyObject *objectPtr)
//  {
//      typedef bdef_MemFn<void (MyObject::*)(int, const char *)> MemFnType;
//      static MemFnType func(&MyObject::doSomething);
//
//      func(objectPtr, 100, "Hello");
//  }
//..
// In the 'bdef_MemFnInstance' case, the wrapper needs to contain the object as
// well, so it must be created at every function call:
//..
//  void doSomethingWithMemFnInstance(MyObject *objectPtr)
//  {
//      typedef bdef_MemFnInstance<void (MyObject::*)(int, const char*),
//                                 MyObject*> MemFnInstanceType;
//      MemFnInstanceType func(&MyObject::doSomething, objectPtr);
//      func(100, "Hello");
//  }
//..
// This latter example is for exposition only.  It would be much easier to
// invoke the member function directly.  Note that both function calls
// ultimately result in the member function call:
//..
//  objectPtr->doSomething(100, "Hello");
//..
///Usage With Standard Algorithms
/// - - - - - - - - - - - - - - -
// The following example demonstrates the use of 'bdef_MemFn' with the
// standard algorithms 'find_if' and 'for_each'.  First we declare the
// 'MyConnection' and 'MyConnectionManager' classes used in the example,
// keeping the class definitions short to highlight the member functions for
// which we will later build wrappers:
//..
//  class MyConnection {
//
//      // DATA (not shown)
//
//    public:
//      // CREATORS (not shown)
//
//      // MANIPULATORS
//      void disconnect();
//
//      // ACCESSORS
//      bool isAvailable() const;
//  };
//
//  class MyConnectionManager {
//
//      // PRIVATE TYPES
//      typedef bsl::list<MyConnection *> MyConnectionList;
//
//      // DATA
//      MyConnectionList d_list;
//
//    public:
//      // CREATORS (not shown)
//
//      // MANIPULATORS
//      void disconnectAll();
//
//      // ACCESSORS
//      MyConnection *nextAvailable() const;
//  };
//..
// The 'nextAvailable' function returns the next 'MyConnection' object that
// is available.  The 'find_if' algorithm is used to search the list for the
// first 'MyConnection' object that is available.  'find_if' invokes the
// provided function object for each item in the list until a 'true' result is
// returned, or the end of the list is reached.  A 'bdef_MemFn' object bound to
// the 'MyConnection::isAvailable' member function is used as the test
// functor.  Note that the type of this object is never spelled out, it is
// built on the fly using the 'bdef_MemFnUtil' utility before being passed as a
// functor to the 'bsl::find_if' algorithm:
//..
//  MyConnection *MyConnectionManager::nextAvailable() const
//  {
//      MyConnectionList::const_iterator it =
//             bsl::find_if(d_list.begin(),
//                          d_list.end(),
//                          bdef_MemFnUtil::memFn(&MyConnection::isAvailable));
//      return it == d_list.end() ? 0 : *it;
//  }
//..
// The 'disconnectAll' function calls 'disconnect' on each 'MyConnection'
// object in the list.  The 'for_each' algorithm is used to iterate through
// each 'MyConnection' object in the list and invoke the 'disconnect' method:
//..
//  void MyConnectionManager::disconnectAll()
//  {
//      bsl::for_each(d_list.begin(),
//                    d_list.end(),
//                    bdef_MemFnUtil::memFn(&MyConnection::disconnect));
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLALG_CONSTRUCTORPROXY
#include <bslalg_constructorproxy.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITHASPOINTERSEMANTICS
#include <bslalg_typetraithaspointersemantics.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_HASTRAIT
#include <bslalg_hastrait.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLMF_FORWARDINGTYPE
#include <bslmf_forwardingtype.h>
#endif

#ifndef INCLUDED_BSLMF_ISPOINTER
#include <bslmf_ispointer.h>
#endif

#ifndef INCLUDED_BSLMF_MEMBERFUNCTIONPOINTERTRAITS
#include <bslmf_memberfunctionpointertraits.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

#ifndef INCLUDED_BSLMF_TYPELIST
#include <bslmf_typelist.h>
#endif


namespace BloombergLP {

                        // ============================
                        // class bdef_MemFn_Dereference
                        // ============================

template <class OBJTYPE>
struct bdef_MemFn_Dereference {
    // This utility is used to convert user supplied values to references to
    // 'OBJTYPE' for object references that are directly convertible to
    // 'OBJTYPE', the reference is returned directly.  For pointers and objects
    // that are not directly convertible (e.g., "smart pointers to 'OBJTYPE'"),
    // the result of '*OBJECT' is returned, where 'OBJECT' is the pointer or
    // object reference.

    static inline OBJTYPE& derefImp(OBJTYPE& obj, bslmf_MetaInt<0> *)
    {
        return obj;
    }

    template <class TYPE>
    static inline OBJTYPE& derefImp(TYPE& obj, bslmf_MetaInt<1> *)
    {
        return *obj;
    }

    template <class TYPE>
    static inline OBJTYPE& derefImp(const TYPE& obj, bslmf_MetaInt<1> *)
    {
        return *obj;
    }

    template <class TYPE>
    static inline OBJTYPE& deref(TYPE& obj)
    {
        enum { VALUE = bslmf_IsPointer<TYPE>::VALUE
                    || bslalg_HasTrait<TYPE,
                                 bslalg_TypeTraitHasPointerSemantics>::VALUE };

        return derefImp(obj, (bslmf_MetaInt<VALUE> *)0);
    }

    template <class TYPE>
    static inline OBJTYPE& deref(const TYPE& obj)
    {
        enum { VALUE = bslmf_IsPointer<TYPE>::VALUE
                    || bslalg_HasTrait<TYPE,
                                 bslalg_TypeTraitHasPointerSemantics>::VALUE };

        return derefImp(obj, (bslmf_MetaInt<VALUE> *)0);
    }
};

                              // ================
                              // class bdef_MemFn
                              // ================

template <class PROTOTYPE>
class bdef_MemFn {
    // This class encapsulates a member function pointer having the
    // parameterized 'PROTOTYPE', such that the member function pointer can be
    // invoked in syntactically the same manner as a free function pointer.
    // When invoking a member function through this wrapper, the first argument
    // must be a pointer or reference to the instance on which to invoke the
    // member function pointer.  Zero to fourteen additional arguments may be
    // specified depending on 'PROTOTYPE'.

    // PRIVATE TYPES
    typedef bslmf_MemberFunctionPointerTraits<PROTOTYPE> Traits;

    // ASSERTIONS
    BSLMF_ASSERT(Traits::IS_MEMBER_FUNCTION_PTR);  // otherwise none of the
                                                   // 'typedef's below make any
                                                   // sense

  public:
    // TYPES
    typedef typename Traits::ResultType   ResultType;
        // 'ResultType' is an alias for the type of the object returned by an
        // invocation of this member function wrapper.

    typedef typename Traits::ArgumentList Args;
        // 'Args' is an alias for the list of arguments passed to an invocation
        // of this member function wrapper, expressed as a 'bslmf_Typelist'.

    typedef typename Traits::ClassType    ObjectType;
        // 'ObjectType' is an alias for the class type to which the member
        // function that is wrapped belongs.

    typedef PROTOTYPE                     Prototype;
        // 'ProtoType' is an alias for the parameterized 'PROTOTYPE' passed as
        // a template argument to this wrapper.

    typedef PROTOTYPE                     ProtoType;
        // 'ProtoType' is an alias for the parameterized 'PROTOTYPE' passed as
        // a template argument to this wrapper.
        //
        // *DEPRECATED*: Use 'Prototype' instead.

  private:
    // PRIVATE TYPES
    typedef typename bslmf_ForwardingType<
        typename bslmf_TypeListTypeOf< 1, Args>::TypeOrDefault>::Type A1;
    typedef typename bslmf_ForwardingType<
        typename bslmf_TypeListTypeOf< 2, Args>::TypeOrDefault>::Type A2;
    typedef typename bslmf_ForwardingType<
        typename bslmf_TypeListTypeOf< 3, Args>::TypeOrDefault>::Type A3;
    typedef typename bslmf_ForwardingType<
        typename bslmf_TypeListTypeOf< 4, Args>::TypeOrDefault>::Type A4;
    typedef typename bslmf_ForwardingType<
        typename bslmf_TypeListTypeOf< 5, Args>::TypeOrDefault>::Type A5;
    typedef typename bslmf_ForwardingType<
        typename bslmf_TypeListTypeOf< 6, Args>::TypeOrDefault>::Type A6;
    typedef typename bslmf_ForwardingType<
        typename bslmf_TypeListTypeOf< 7, Args>::TypeOrDefault>::Type A7;
    typedef typename bslmf_ForwardingType<
        typename bslmf_TypeListTypeOf< 8, Args>::TypeOrDefault>::Type A8;
    typedef typename bslmf_ForwardingType<
        typename bslmf_TypeListTypeOf< 9, Args>::TypeOrDefault>::Type A9;
    typedef typename bslmf_ForwardingType<
        typename bslmf_TypeListTypeOf<10, Args>::TypeOrDefault>::Type A10;
    typedef typename bslmf_ForwardingType<
        typename bslmf_TypeListTypeOf<11, Args>::TypeOrDefault>::Type A11;
    typedef typename bslmf_ForwardingType<
        typename bslmf_TypeListTypeOf<12, Args>::TypeOrDefault>::Type A12;
    typedef typename bslmf_ForwardingType<
        typename bslmf_TypeListTypeOf<13, Args>::TypeOrDefault>::Type A13;
    typedef typename bslmf_ForwardingType<
        typename bslmf_TypeListTypeOf<14, Args>::TypeOrDefault>::Type A14;
        // 'AN', for 'N' from 1 up to 14, is an alias for the type of the 'N'th
        // argument in the 'Args' list.

    typedef bdef_MemFn_Dereference<ObjectType>                        Deref;

    // DATA
    PROTOTYPE d_func_p;  // pointer to member function

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdef_MemFn,
                                 bslalg_TypeTraitBitwiseCopyable);

    // CREATORS
    explicit
    bdef_MemFn(PROTOTYPE func);
        // Create a member function pointer wrapper holding the address of
        // the specified 'func' member function having the parameterized
        // 'PROTOTYPE'.

    bdef_MemFn(const bdef_MemFn<PROTOTYPE>& original);
        // Create a member function pointer wrapper holding the address of the
        // same member function as the specified 'original' object.

    // ~bdef_MemFn();
        // Destroy this object.  Note that this trivial destructor is generated
        // by the compiler.

    // ACCESSORS
    template <class INSTANCE>
    ResultType operator()(INSTANCE& object) const
        // Invoke the member function pointer held by this wrapper on the
        // specified 'object' reference to a modifiable instance of the
        // parameterized 'INSTANCE' type, with no specified arguments, and
        // return the result of this invocation, or 'void' if this member
        // function pointer does not return a result.
    {
        return (Deref::deref(object).*d_func_p)();
    }

    template <class INSTANCE>
    ResultType operator()(INSTANCE& object, A1 a1) const
        // Invoke the member function pointer held by this wrapper on the
        // specified 'object' reference to a modifiable instance of the
        // parameterized 'INSTANCE' type, with the specified argument 'a1', and
        // return the result of this invocation, or 'void' if this member
        // function pointer does not return a result.
    {
        return (Deref::deref(object).*d_func_p)(a1);  // see NOTES above
    }

    template <class INSTANCE>
    ResultType operator()(INSTANCE& object, A1 a1, A2 a2) const
        // Invoke the member function pointer held by this wrapper on the
        // specified 'object' reference to a modifiable instance of the
        // parameterized 'INSTANCE' type, with the specified argument 'a1' up
        // to 'a2', and return the result of this invocation, or 'void' if this
        // member function pointer does not return a result.
    {
        return (Deref::deref(object).*d_func_p)(a1, a2);  // etc.
    }

    template <class INSTANCE>
    ResultType operator()(INSTANCE& object, A1 a1, A2 a2, A3 a3) const
        // Invoke the member function pointer held by this wrapper on the
        // specified 'object' reference to a modifiable instance of the
        // parameterized 'INSTANCE' type, with the specified argument 'a1' up
        // to 'a3', and return the result of this invocation, or 'void' if this
        // member function pointer does not return a result.
    {
        return (Deref::deref(object).*d_func_p)(a1, a2, a3);
    }

    template <class INSTANCE>
    ResultType operator()(INSTANCE& object, A1 a1, A2 a2, A3 a3, A4 a4) const
        // Invoke the member function pointer held by this wrapper on the
        // specified 'object' reference to a modifiable instance of the
        // parameterized 'INSTANCE' type, with the specified argument 'a1' up
        // to 'a4', and return the result of this invocation, or 'void' if this
        // member function pointer does not return a result.
    {
        return (Deref::deref(object).*d_func_p)(a1, a2, a3, a4);
    }

    template <class INSTANCE>
    ResultType operator()(INSTANCE& object,
                          A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) const
        // Invoke the member function pointer held by this wrapper on the
        // specified 'object' reference to a modifiable instance of the
        // parameterized 'INSTANCE' type, with the specified argument 'a1' up
        // to 'a5', and return the result of this invocation, or 'void' if this
        // member function pointer does not return a result.
    {
        return (Deref::deref(object).*d_func_p)(a1, a2, a3, a4, a5);
    }

    template <class INSTANCE>
    ResultType operator()(INSTANCE& object,
                          A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) const
        // Invoke the member function pointer held by this wrapper on the
        // specified 'object' reference to a modifiable instance of the
        // parameterized 'INSTANCE' type, with the specified argument 'a1' up
        // to 'a6', and return the result of this invocation, or 'void' if this
        // member function pointer does not return a result.
    {
        return (Deref::deref(object).*d_func_p)(a1, a2, a3, a4, a5, a6);
    }

    template <class INSTANCE>
    ResultType operator()(INSTANCE& object,
                          A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6,
                          A7 a7) const
        // Invoke the member function pointer held by this wrapper on the
        // specified 'object' reference to a modifiable instance of the
        // parameterized 'INSTANCE' type, with the specified argument 'a1' up
        // to 'a7', and return the result of this invocation, or 'void' if this
        // member function pointer does not return a result.
    {
        return (Deref::deref(object).*d_func_p)(a1, a2, a3, a4, a5, a6, a7);
    }

    template <class INSTANCE>
    ResultType operator()(INSTANCE& object,
                          A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7,
                          A8 a8) const
        // Invoke the member function pointer held by this wrapper on the
        // specified 'object' reference to a modifiable instance of the
        // parameterized 'INSTANCE' type, with the specified argument 'a1' up
        // to 'a8', and return the result of this invocation, or 'void' if this
        // member function pointer does not return a result.
    {
        return (Deref::deref(object).*d_func_p)(a1, a2, a3, a4, a5, a6, a7,
                                                a8);
    }

    template <class INSTANCE>
    ResultType operator()(INSTANCE& object,
                          A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7,
                          A8 a8, A9 a9) const
        // Invoke the member function pointer held by this wrapper on the
        // specified 'object' reference to a modifiable instance of the
        // parameterized 'INSTANCE' type, with the specified argument 'a1' up
        // to 'a9', and return the result of this invocation, or 'void' if this
        // member function pointer does not return a result.
    {
        return (Deref::deref(object).*d_func_p)(a1, a2, a3, a4, a5, a6, a7, a8,
                                                a9);
    }

    template <class INSTANCE>
    ResultType operator()(INSTANCE& object,
                          A1 a1, A2 a2, A3 a3, A4  a4, A5  a5, A6  a6,
                          A7 a7, A8 a8, A9 a9, A10 a10) const
        // Invoke the member function pointer held by this wrapper on the
        // specified 'object' reference to a modifiable instance of the
        // parameterized 'INSTANCE' type, with the specified argument 'a1' up
        // to 'a10', and return the result of this invocation, or 'void' if
        // this member function pointer does not return a result.
    {
        return (Deref::deref(object).*d_func_p)(a1, a2, a3, a4, a5, a6, a7, a8,
                                                a9, a10);
    }

    template <class INSTANCE>
    ResultType operator()(INSTANCE& object,
                          A1 a1, A2 a2, A3 a3, A4  a4,  A5  a5, A6  a6,
                          A7 a7, A8 a8, A9 a9, A10 a10, A11 a11) const
        // Invoke the member function pointer held by this wrapper on the
        // specified 'object' reference to a modifiable instance of the
        // parameterized 'INSTANCE' type, with the specified argument 'a1' up
        // to 'a11', and return the result of this invocation, or 'void' if
        // this member function pointer does not return a result.
    {
        return (Deref::deref(object).*d_func_p)(a1,  a2, a3, a4, a5, a6, a7,
                                                a8, a9, a10, a11);
    }

    template <class INSTANCE>
    ResultType operator()(INSTANCE& object,
                          A1 a1, A2 a2, A3 a3, A4  a4,  A5  a5, A6 a6,
                          A7 a7, A8 a8, A9 a9, A10 a10, A11 a11,
                          A12 a12) const
        // Invoke the member function pointer held by this wrapper on the
        // specified 'object' reference to a modifiable instance of the
        // parameterized 'INSTANCE' type, with the specified argument 'a1' up
        // to 'a12', and return the result of this invocation, or 'void' if
        // this member function pointer does not return a result.
    {
        return (Deref::deref(object).*d_func_p)(a1, a2, a3,  a4,  a5,  a6,  a7,
                                                a8, a9, a10, a11, a12);
    }

    template <class INSTANCE>
    ResultType operator()(INSTANCE& object,
                          A1  a1, A2  a2, A3  a3, A4  a4,  A5  a5,  A6  a6,
                          A7  a7, A8  a8, A9  a9, A10 a10, A11 a11, A12 a12,
                          A13 a13) const
        // Invoke the member function pointer held by this wrapper on the
        // specified 'object' reference to a modifiable instance of the
        // parameterized 'INSTANCE' type, with the specified argument 'a1' up
        // to 'a13', and return the result of this invocation, or 'void' if
        // this member function pointer does not return a result.
    {
        return (Deref::deref(object).*d_func_p)(a1, a2, a3,  a4,  a5,  a6,  a7,
                                                a8, a9, a10, a11, a12, a13);
    }

    template <class INSTANCE>
    ResultType operator()(INSTANCE& object,
                          A1  a1,  A2  a2, A3  a3, A4  a4,  A5  a5,  A6  a6,
                          A7  a7,  A8  a8, A9  a9, A10 a10, A11 a11, A12 a12,
                          A13 a13, A14 a14) const
        // Invoke the member function pointer held by this wrapper on the
        // specified 'object' reference to a modifiable instance of the
        // parameterized 'INSTANCE' type, with the specified argument 'a1' up
        // to 'a14', and return the result of this invocation, or 'void' if
        // this member function pointer does not return a result.
    {
        return (Deref::deref(object).*d_func_p)(a1, a2, a3,  a4,  a5,  a6,  a7,
                                                a8, a9, a10, a11, a12, a13,
                                                a14);
    }

    template <class INSTANCE>
    ResultType operator()(const INSTANCE& object) const
        // Invoke the member function pointer held by this wrapper on the
        // specified 'object' reference to a non-modifiable instance of the
        // parameterized 'INSTANCE' type, with no arguments, and return the
        // result of this invocation, or 'void' if this member function pointer
        // does not return a result.
    {
        return (Deref::deref(object).*d_func_p)();
    }

    template <class INSTANCE>
    ResultType operator()(const INSTANCE& object, A1 a1) const
        // Invoke the member function pointer held by this wrapper on the
        // specified 'object' reference to a non-modifiable instance of the
        // parameterized 'INSTANCE' type, with the specified argument 'a1', and
        // return the result of this invocation, or 'void' if this member
        // function pointer does not return a result.
    {
        return (Deref::deref(object).*d_func_p)(a1);
    }

    template <class INSTANCE>
    ResultType operator()(const INSTANCE& object, A1 a1, A2 a2) const
        // Invoke the member function pointer held by this wrapper on the
        // specified 'object' reference to a non-modifiable instance of the
        // parameterized 'INSTANCE' type, with the specified arguments 'a1' up
        // to 'a2', and return the result of this invocation, or 'void' if this
        // member function pointer does not return a result.
    {
        return (Deref::deref(object).*d_func_p)(a1, a2);
    }

    template <class INSTANCE>
    ResultType operator()(const INSTANCE& object, A1 a1, A2 a2, A3 a3) const
        // Invoke the member function pointer held by this wrapper on the
        // specified 'object' reference to a non-modifiable instance of the
        // parameterized 'INSTANCE' type, with the specified arguments 'a1' up
        // to 'a3', and return the result of this invocation, or 'void' if this
        // member function pointer does not return a result.
    {
        return (Deref::deref(object).*d_func_p)(a1, a2, a3);
    }

    template <class INSTANCE>
    ResultType operator()(const INSTANCE& object,
                          A1 a1, A2 a2, A3 a3, A4 a4) const
        // Invoke the member function pointer held by this wrapper on the
        // specified 'object' reference to a non-modifiable instance of the
        // parameterized 'INSTANCE' type, with the specified arguments 'a1' up
        // to 'a4', and return the result of this invocation, or 'void' if this
        // member function pointer does not return a result.
    {
        return (Deref::deref(object).*d_func_p)(a1, a2, a3, a4);
    }

    template <class INSTANCE>
    ResultType operator()(const INSTANCE& object,
                          A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) const
        // Invoke the member function pointer held by this wrapper on the
        // specified 'object' reference to a non-modifiable instance of the
        // parameterized 'INSTANCE' type, with the specified arguments 'a1' up
        // to 'a5', and return the result of this invocation, or 'void' if this
        // member function pointer does not return a result.
    {
        return (Deref::deref(object).*d_func_p)(a1, a2, a3, a4, a5);
    }

    template <class INSTANCE>
    ResultType operator()(const INSTANCE& object,
                          A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) const
        // Invoke the member function pointer held by this wrapper on the
        // specified 'object' reference to a non-modifiable instance of the
        // parameterized 'INSTANCE' type, with the specified arguments 'a1' up
        // to 'a6', and return the result of this invocation, or 'void' if this
        // member function pointer does not return a result.
    {
        return (Deref::deref(object).*d_func_p)(a1, a2, a3, a4, a5, a6);
    }

    template <class INSTANCE>
    ResultType operator()(const INSTANCE& object,
                          A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6,
                          A7 a7) const
        // Invoke the member function pointer held by this wrapper on the
        // specified 'object' reference to a non-modifiable instance of the
        // parameterized 'INSTANCE' type, with the specified arguments 'a1' up
        // to 'a7', and return the result of this invocation, or 'void' if this
        // member function pointer does not return a result.
    {
        return (Deref::deref(object).*d_func_p)(a1, a2,  a3,  a4,  a5, a6, a7);
    }

    template <class INSTANCE>
    ResultType operator()(const INSTANCE& object,
                          A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7,
                          A8 a8) const
        // Invoke the member function pointer held by this wrapper on the
        // specified 'object' reference to a non-modifiable instance of the
        // parameterized 'INSTANCE' type, with the specified arguments 'a1' up
        // to 'a8', and return the result of this invocation, or 'void' if this
        // member function pointer does not return a result.
    {
        return (Deref::deref(object).*d_func_p)(a1, a2, a3, a4, a5, a6, a7,
                                                a8);
    }

    template <class INSTANCE>
    ResultType operator()(const INSTANCE& object,
                          A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7,
                          A8 a8, A9 a9) const
        // Invoke the member function pointer held by this wrapper on the
        // specified 'object' reference to a non-modifiable instance of the
        // parameterized 'INSTANCE' type, with the specified arguments 'a1' up
        // to 'a9', and return the result of this invocation, or 'void' if this
        // member function pointer does not return a result.
    {
        return (Deref::deref(object).*d_func_p)(a1, a2, a3, a4, a5, a6, a7,
                                                a8, a9);
    }

    template <class INSTANCE>
    ResultType operator()(const INSTANCE& object,
                          A1 a1, A2 a2, A3 a3, A4  a4, A5  a5, A6  a6,
                          A7 a7, A8 a8, A9 a9, A10 a10) const
        // Invoke the member function pointer held by this wrapper on the
        // specified 'object' reference to a non-modifiable instance of the
        // parameterized 'INSTANCE' type, with the specified arguments 'a1' up
        // to 'a10', and return the result of this invocation, or 'void' if
        // this member function pointer does not return a result.
    {
        return (Deref::deref(object).*d_func_p)(a1, a2, a3, a4, a5, a6, a7,
                                                a8, a9, a10);
    }

    template <class INSTANCE>
    ResultType operator()(const INSTANCE& object,
                          A1 a1, A2 a2, A3 a3, A4  a4, A5  a5, A6  a6,
                          A7 a7, A8 a8, A9 a9, A10 a10, A11 a11) const
        // Invoke the member function pointer held by this wrapper on the
        // specified 'object' reference to a non-modifiable instance of the
        // parameterized 'INSTANCE' type, with the specified arguments 'a1' up
        // to 'a11', and return the result of this invocation, or 'void' if
        // this member function pointer does not return a result.
    {
        return (Deref::deref(object).*d_func_p)(a1, a2, a3,  a4, a5, a6, a7,
                                                a8, a9, a10, a11);
    }

    template <class INSTANCE>
    ResultType operator()(const INSTANCE& object,
                          A1  a1, A2 a2, A3 a3, A4  a4,  A5  a5,  A6  a6,
                          A7  a7, A8 a8, A9 a9, A10 a10, A11 a11,
                          A12 a12) const
        // Invoke the member function pointer held by this wrapper on the
        // specified 'object' reference to a non-modifiable instance of the
        // parameterized 'INSTANCE' type, with the specified arguments 'a1' up
        // to 'a12', and return the result of this invocation, or 'void' if
        // this member function pointer does not return a result.
    {
        return (Deref::deref(object).*d_func_p)(a1, a2, a3,  a4,  a5, a6, a7,
                                                a8, a9, a10, a11, a12);
    }

    template <class INSTANCE>
    ResultType operator()(const INSTANCE& object,
                          A1  a1, A2 a2, A3 a3, A4  a4,  A5  a5,  A6  a6,
                          A7  a7, A8 a8, A9 a9, A10 a10, A11 a11, A12 a12,
                          A13 a13) const
        // Invoke the member function pointer held by this wrapper on the
        // specified 'object' reference to a non-modifiable instance of the
        // parameterized 'INSTANCE' type, with the specified arguments 'a1' up
        // to 'a13', and return the result of this invocation, or 'void' if
        // this member function pointer does not return a result.
    {
        return (Deref::deref(object).*d_func_p)(a1, a2, a3,  a4,  a5,  a6,  a7,
                                                a8, a9, a10, a11, a12, a13);
    }

    template <class INSTANCE>
    ResultType operator()(const INSTANCE& object,
                          A1  a1,  A2  a2, A3 a3, A4  a4,  A5  a5,  A6  a6,
                          A7  a7,  A8  a8, A9 a9, A10 a10, A11 a11, A12 a12,
                          A13 a13, A14 a14) const
        // Invoke the member function pointer held by this wrapper on the
        // specified 'object' reference to a non-modifiable instance of the
        // parameterized 'INSTANCE' type, with the specified arguments 'a1' up
        // to 'a14', and return the result of this invocation, or 'void' if
        // this member function pointer does not return a result.
    {
        return (Deref::deref(object).*d_func_p)(a1, a2, a3,  a4,  a5,  a6,  a7,
                                                a8, a9, a10, a11, a12, a13,
                                                a14);
    }
};

                          // ========================
                          // class bdef_MemFnInstance
                          // ========================

template <class PROTOTYPE, class INSTANCE>
class bdef_MemFnInstance {
    // This class encapsulates a member function pointer having the
    // parameterized 'PROTOTYPE' and a value of the parameterized 'INSTANCE'
    // type, which can be either a pointer or reference to an associated
    // instance of the object type referred to by the 'PROTOTYPE', such that
    // the member function pointer can be invoked on the wrapped instance in
    // syntactically the same manner as a free function pointer.  Zero to
    // fourteen additional arguments may be specified depending on the
    // 'PROTOTYPE'.  Note that whether 'INSTANCE' is a pointer or a reference
    // is determined by whether it has pointer semantics or not (as determined
    // by the 'bslalg_TypeTraitHasPointerSemantics' type trait).

    // PRIVATE TYPES
    typedef bslmf_MemberFunctionPointerTraits<PROTOTYPE> Traits;

  public:
    // TYPES
    typedef typename Traits::ResultType   ResultType;
        // 'ResultType' is an alias for the type of the object returned by an
        // invocation of this member function wrapper.

    typedef typename Traits::ArgumentList Args;
        // 'Args' is an alias for the list of arguments passed to an invocation
        // of this member function wrapper, expressed as a 'bslmf_Typelist'.

    typedef typename Traits::ClassType    ObjectType;
        // 'ObjectType' is an alias for the class type to which the member
        // function that is wrapped belongs.

    typedef PROTOTYPE                     Prototype;
        // 'ProtoType' is an alias for the parameterized 'PROTOTYPE' passed as
        // a template argument to this wrapper.

    typedef PROTOTYPE                     ProtoType;
        // 'ProtoType' is an alias for the parameterized 'PROTOTYPE' passed as
        // a template argument to this wrapper.
        //
        // *DEPRECATED*: Use 'Prototype' instead.

    typedef bdef_MemFn_Dereference<ObjectType> Deref;


  private:
    // PRIVATE TYPES
    typedef typename bslmf_ForwardingType<
        typename bslmf_TypeListTypeOf< 1, Args>::TypeOrDefault>::Type A1;
    typedef typename bslmf_ForwardingType<
        typename bslmf_TypeListTypeOf< 2, Args>::TypeOrDefault>::Type A2;
    typedef typename bslmf_ForwardingType<
        typename bslmf_TypeListTypeOf< 3, Args>::TypeOrDefault>::Type A3;
    typedef typename bslmf_ForwardingType<
        typename bslmf_TypeListTypeOf< 4, Args>::TypeOrDefault>::Type A4;
    typedef typename bslmf_ForwardingType<
        typename bslmf_TypeListTypeOf< 5, Args>::TypeOrDefault>::Type A5;
    typedef typename bslmf_ForwardingType<
        typename bslmf_TypeListTypeOf< 6, Args>::TypeOrDefault>::Type A6;
    typedef typename bslmf_ForwardingType<
        typename bslmf_TypeListTypeOf< 7, Args>::TypeOrDefault>::Type A7;
    typedef typename bslmf_ForwardingType<
        typename bslmf_TypeListTypeOf< 8, Args>::TypeOrDefault>::Type A8;
    typedef typename bslmf_ForwardingType<
        typename bslmf_TypeListTypeOf< 9, Args>::TypeOrDefault>::Type A9;
    typedef typename bslmf_ForwardingType<
        typename bslmf_TypeListTypeOf<10, Args>::TypeOrDefault>::Type A10;
    typedef typename bslmf_ForwardingType<
        typename bslmf_TypeListTypeOf<11, Args>::TypeOrDefault>::Type A11;
    typedef typename bslmf_ForwardingType<
        typename bslmf_TypeListTypeOf<12, Args>::TypeOrDefault>::Type A12;
    typedef typename bslmf_ForwardingType<
        typename bslmf_TypeListTypeOf<13, Args>::TypeOrDefault>::Type A13;
    typedef typename bslmf_ForwardingType<
        typename bslmf_TypeListTypeOf<14, Args>::TypeOrDefault>::Type A14;
        // 'AN', for 'N' from 1 up to 14, is an alias for the type of the 'N'th
        // argument in the 'Args' list.

    // DATA
    PROTOTYPE                         d_func_p;  // pointer to member function

    bslalg_ConstructorProxy<INSTANCE> d_obj;     // object instance, or pointer
                                                 // to such

    // PRIVATE ACCESSORS
  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdef_MemFnInstance,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    bdef_MemFnInstance(PROTOTYPE        func,
                       const INSTANCE&  object,
                       bslma_Allocator *basicAllocator = 0);
        // Create a member function pointer wrapper around the specified 'func'
        // member function pointer of the parameterized 'PROTOTYPE', that is
        // invokable on the specified 'object' instance of the parameterized
        // 'INSTANCE'.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    bdef_MemFnInstance(const bdef_MemFnInstance&  original,
                       bslma_Allocator           *basicAllocator = 0);
        // Create a member function pointer wrapper around the same member
        // function and instance pointed to by the specified 'original' object.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    ~bdef_MemFnInstance();
        // Destroy this object.

    // MANIPULATORS
    bdef_MemFnInstance& operator=(const bdef_MemFnInstance& rhs);
        // Assign to this member function pointer wrapper the same member
        // function and instance pointed to by the specified 'rhs' member
        // function pointer wrapper, and return a reference to this modifiable
        // member function pointer wrapper.

    // ACCESSORS
    ResultType operator()() const;
    ResultType operator()(A1 a1) const;
    ResultType operator()(A1 a1, A2 a2) const;
    ResultType operator()(A1 a1, A2 a2, A3 a3) const;
    ResultType operator()(A1 a1, A2 a2, A3 a3, A4 a4) const;
    ResultType operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) const;
    ResultType operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) const;
    ResultType operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6,
                          A7 a7) const;
    ResultType operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6,
                          A7 a7, A8 a8) const;
    ResultType operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6,
                          A7 a7, A8 a8, A9 a9) const;
    ResultType operator()(A1 a1, A2 a2, A3 a3, A4  a4, A5 a5, A6 a6,
                          A7 a7, A8 a8, A9 a9, A10 a10) const;
    ResultType operator()(A1 a1, A2 a2, A3 a3, A4  a4,  A5  a5, A6 a6,
                          A7 a7, A8 a8, A9 a9, A10 a10, A11 a11) const;
    ResultType operator()(A1 a1, A2 a2, A3 a3, A4  a4,  A5  a5, A6 a6,
                          A7 a7, A8 a8, A9 a9, A10 a10, A11 a11,
                          A12 a12) const;
    ResultType operator()(A1 a1, A2 a2, A3 a3, A4  a4,  A5  a5, A6 a6,
                          A7 a7, A8 a8, A9 a9, A10 a10, A11 a11,
                          A12 a12, A13 a13) const;
    ResultType operator()(A1 a1, A2 a2, A3 a3, A4  a4,  A5  a5, A6 a6,
                          A7 a7, A8 a8, A9 a9, A10 a10, A11 a11,
                          A12 a12, A13 a13, A14 a14) const;
        // Invoke the member function pointer held by this wrapper on the
        // provided object with the specified 'a1' up to 'aN' as arguments,
        // with 'N' being the number of arguments of the member function, and
        // return the result of this invocation, or 'void' if this member
        // function pointer does not return a result.
};

                            // =====================
                            // struct bdef_MemFnUtil
                            // =====================

struct bdef_MemFnUtil {
    // The methods provided in this utility are used for constructing
    // 'bdef_MemFn' and 'bdef_MemFnInstance' objects from function pointers.

    // CLASS METHODS
    template <class PROTOTYPE>
    static
    bdef_MemFn<PROTOTYPE> memFn(PROTOTYPE func);
        // Return a 'bdef_MemFn' member function wrapper that encapsulates the
        // specified 'func' member function pointer of the parameterized
        // 'PROTOTYPE'.

    template <class PROTOTYPE, class INSTANCE>
    static
    bdef_MemFnInstance<PROTOTYPE, INSTANCE> memFn(PROTOTYPE       func,
                                                  const INSTANCE& object);
        // Return a 'bdef_MemFnInstance' member function wrapper that
        // encapsulates the specified 'func' member function pointer of the
        // parameterized 'PROTOTYPE' and the specified 'object' instance of the
        // parameterized 'INSTANCE' type.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                          // ----------------
                          // class bdef_MemFn
                          // ----------------

// CREATORS
template <class PROTOTYPE>
inline
bdef_MemFn<PROTOTYPE>::bdef_MemFn(PROTOTYPE func)
: d_func_p(func)
{
}

template <class PROTOTYPE>
inline
bdef_MemFn<PROTOTYPE>::bdef_MemFn(const bdef_MemFn<PROTOTYPE>& original)
: d_func_p(original.d_func_p)
{
}

                          // ------------------------
                          // class bdef_MemFnInstance
                          // ------------------------

// CREATORS
template <class PROTOTYPE, class INSTANCE>
inline
bdef_MemFnInstance<PROTOTYPE, INSTANCE>::bdef_MemFnInstance(
        const bdef_MemFnInstance<PROTOTYPE, INSTANCE>&  original,
        bslma_Allocator                                *basicAllocator)
: d_func_p(original.d_func_p)
, d_obj(original.d_obj, bslma_Default::allocator(basicAllocator))
{
}

template <class PROTOTYPE, class INSTANCE>
inline
bdef_MemFnInstance<PROTOTYPE, INSTANCE>::bdef_MemFnInstance(
                                               PROTOTYPE        func,
                                               const INSTANCE&  obj,
                                               bslma_Allocator *basicAllocator)
: d_func_p(func)
, d_obj(obj, bslma_Default::allocator(basicAllocator))
{
}

template <class PROTOTYPE, class INSTANCE>
inline
bdef_MemFnInstance<PROTOTYPE, INSTANCE>::~bdef_MemFnInstance()
{
}

// MANIPULATORS
template <class PROTOTYPE, class INSTANCE>
inline
bdef_MemFnInstance<PROTOTYPE, INSTANCE>&
bdef_MemFnInstance<PROTOTYPE, INSTANCE>::operator=(
                                                 const bdef_MemFnInstance& rhs)
{
    d_func_p       = rhs.d_func_p;
    d_obj.object() = rhs.d_obj.object();
    return *this;
}

// ACCESSORS
template <class PROTOTYPE, class INSTANCE>
inline
typename bdef_MemFnInstance<PROTOTYPE, INSTANCE>::ResultType
bdef_MemFnInstance<PROTOTYPE, INSTANCE>::operator()() const
{
    return (Deref::deref(d_obj.object()).*d_func_p)();
}

template <class PROTOTYPE, class INSTANCE>
inline
typename bdef_MemFnInstance<PROTOTYPE, INSTANCE>::ResultType
bdef_MemFnInstance<PROTOTYPE, INSTANCE>::operator()(A1 a1) const
{
    return (Deref::deref(d_obj.object()).*d_func_p)(a1);
}

template <class PROTOTYPE, class INSTANCE>
inline
typename bdef_MemFnInstance<PROTOTYPE, INSTANCE>::ResultType
bdef_MemFnInstance<PROTOTYPE, INSTANCE>::operator()(A1 a1, A2 a2) const
{
    return (Deref::deref(d_obj.object()).*d_func_p)(a1, a2);
}

template <class PROTOTYPE, class INSTANCE>
inline
typename bdef_MemFnInstance<PROTOTYPE, INSTANCE>::ResultType
bdef_MemFnInstance<PROTOTYPE, INSTANCE>::operator()(A1 a1, A2 a2, A3 a3) const
{
    return (Deref::deref(d_obj.object()).*d_func_p)(a1, a2, a3);
}

template <class PROTOTYPE, class INSTANCE>
inline
typename bdef_MemFnInstance<PROTOTYPE, INSTANCE>::ResultType
bdef_MemFnInstance<PROTOTYPE, INSTANCE>::operator()(
                                              A1 a1, A2 a2, A3 a3, A4 a4) const
{
    return (Deref::deref(d_obj.object()).*d_func_p)(a1, a2, a3, a4);
}

template <class PROTOTYPE, class INSTANCE>
inline
typename bdef_MemFnInstance<PROTOTYPE, INSTANCE>::ResultType
bdef_MemFnInstance<PROTOTYPE, INSTANCE>::operator()(
                                       A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) const
{
    return (Deref::deref(d_obj.object()).*d_func_p)(a1, a2, a3, a4, a5);
}

template <class PROTOTYPE, class INSTANCE>
inline
typename bdef_MemFnInstance<PROTOTYPE, INSTANCE>::ResultType
bdef_MemFnInstance<PROTOTYPE, INSTANCE>::operator()(
                                A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) const
{
    return (Deref::deref(d_obj.object()).*d_func_p)(a1, a2, a3, a4, a5, a6);
}

template <class PROTOTYPE, class INSTANCE>
inline
typename bdef_MemFnInstance<PROTOTYPE, INSTANCE>::ResultType
bdef_MemFnInstance<PROTOTYPE, INSTANCE>::operator()(
                         A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) const
{
    return (Deref::deref(d_obj.object()).*d_func_p)(a1, a2, a3, a4, a5, a6,
                                                    a7);
}

template <class PROTOTYPE, class INSTANCE>
inline
typename bdef_MemFnInstance<PROTOTYPE, INSTANCE>::ResultType
bdef_MemFnInstance<PROTOTYPE, INSTANCE>::operator()(
                  A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) const
{
    return (Deref::deref(d_obj.object()).*d_func_p)(a1, a2, a3, a4, a5, a6, a7,
                                                    a8);
}

template <class PROTOTYPE, class INSTANCE>
inline
typename bdef_MemFnInstance<PROTOTYPE, INSTANCE>::ResultType
bdef_MemFnInstance<PROTOTYPE, INSTANCE>::operator()(
           A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9) const
{
    return (Deref::deref(d_obj.object()).*d_func_p)(a1, a2, a3, a4, a5, a6, a7,
                                                    a8, a9);
}

template <class PROTOTYPE, class INSTANCE>
inline
typename bdef_MemFnInstance<PROTOTYPE, INSTANCE>::ResultType
bdef_MemFnInstance<PROTOTYPE, INSTANCE>::operator()(
                A1  a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9,
                A10 a10) const
{
    return (Deref::deref(d_obj.object()).*d_func_p)(a1, a2, a3, a4, a5, a6, a7,
                                                    a8, a9, a10);
}

template <class PROTOTYPE, class INSTANCE>
inline
typename bdef_MemFnInstance<PROTOTYPE, INSTANCE>::ResultType
bdef_MemFnInstance<PROTOTYPE, INSTANCE>::operator()(
              A1  a1,  A2  a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9,
              A10 a10, A11 a11) const
{
    return (Deref::deref(d_obj.object()).*d_func_p)(a1, a2, a3, a4, a5, a6,
                                                    a7, a8, a9, a10, a11);
}

template <class PROTOTYPE, class INSTANCE>
inline
typename bdef_MemFnInstance<PROTOTYPE, INSTANCE>::ResultType
bdef_MemFnInstance<PROTOTYPE, INSTANCE>::operator()(
           A1  a1,  A2  a2,  A3  a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9,
           A10 a10, A11 a11, A12 a12) const
{
    return (Deref::deref(d_obj.object()).*d_func_p)(a1, a2, a3, a4, a5, a6, a7,
                                                    a8, a9, a10, a11, a12);
}

template <class PROTOTYPE, class INSTANCE>
inline
typename bdef_MemFnInstance<PROTOTYPE, INSTANCE>::ResultType
bdef_MemFnInstance<PROTOTYPE, INSTANCE>::operator()(
          A1  a1,  A2  a2,  A3  a3,  A4  a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9,
          A10 a10, A11 a11, A12 a12, A13 a13) const
{
    return (Deref::deref(d_obj.object()).*d_func_p)(a1, a2, a3, a4, a5, a6, a7,
                                                    a8, a9, a10, a11, a12,
                                                    a13);
}

template <class PROTOTYPE, class INSTANCE>
inline
typename bdef_MemFnInstance<PROTOTYPE, INSTANCE>::ResultType
bdef_MemFnInstance<PROTOTYPE, INSTANCE>::operator()(
        A1  a1,  A2  a2,  A3  a3,  A4  a4,  A5  a5, A6 a6, A7 a7, A8 a8, A9 a9,
        A10 a10, A11 a11, A12 a12, A13 a13, A14 a14) const
{
    return (Deref::deref(d_obj.object()).*d_func_p)(a1, a2, a3, a4, a5, a6, a7,
                                                    a8, a9, a10, a11, a12, a13,
                                                    a14);
}

                            // ---------------------
                            // struct bdef_MemFnUtil
                            // ---------------------

// CLASS METHODS
template <class PROTOTYPE>
inline
bdef_MemFn<PROTOTYPE>
bdef_MemFnUtil::memFn(PROTOTYPE func)
{
    return bdef_MemFn<PROTOTYPE>(func);
}

template <class PROTOTYPE, class INSTANCE>
inline
bdef_MemFnInstance<PROTOTYPE, INSTANCE>
bdef_MemFnUtil::memFn(PROTOTYPE func, const INSTANCE& object)
{
    return bdef_MemFnInstance<PROTOTYPE, INSTANCE>(func, object);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
