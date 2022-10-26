// bslalg_nothrowmovableutil.h                                        -*-C++-*-

#ifndef INCLUDED_BSLALG_NOTHROWMOVABLEUTIL
#define INCLUDED_BSLALG_NOTHROWMOVABLEUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a wrapper that asserts a noexcept move constructor.
//
//@CLASSES:
// bslalg::NothrowMovableUtil: utilities for 'bslalg_nothrowmovablewrapper'
//
//@SEE_ALSO: bslalg_nothrowmovablewrapper
//
//@DESCRIPTION:  This component provides a utility struct,
// 'bslalg::NothrowMovableUtil', for managing 'bslalg_nothrowmovablewrapper'
// objects.  It provides a namespace for static functions 'wrap' and 'unwrap'
// with a uniform interface such that unwrapping an object that is not wrapped
// or wrapping an object that is already wrapped are no-ops.  This utility
// struct also provides type traits for determining whether a type is wrapped
// and for deducing the type of the wrapped and unwrapped object.
//
///Usage
///-----
//
///Example
///- - - -
// In this example, we define a class template, 'CountedType<TYPE>', a wrapper
// around 'TYPE' that counts the number of extant 'CountedType' objects.  We
// begin by defining the static count member along with the single value
// member:
//..
//  template <class TYPE>
//  class CountedType {
//      // CLASS DATA
//      static int s_count;
//
//      // DATA
//      TYPE       d_value;
//..
// Because of externally-imposed requirements, the move constructor for
// 'CountedType' must provide the strong guarantee; i.e., if the move
// constructor of 'TYPE' throws an exception, then the moved-from 'CountedType'
// object must be left unchanged.  To support this requirement, we next define
// a private static function, 'MoveIfNoexcept', similar to the standard
// 'std::move_if_noexcept', that returns a movable reference if its argument is
// no-throw move constructible and a const lvalue reference otherwise:
//..
//      // PRIVATE CLASS FUNCTIONS
//      template <class TP>
//      static typename
//      bsl::conditional<bsl::is_nothrow_move_constructible<TP>::value,
//                       bslmf::MovableRef<TP>, const TP&>::type
//      MoveIfNoexcept(TP& x);
//..
// We next finish out the class definition with a constructor, copy
// constructor, move constructor, destructor, and member functions to retrieve
// the count and value:
//..
//  public:
//      // CLASS FUNCTIONS
//      static int count() { return s_count; }
//
//      // CREATORS
//      CountedType(const TYPE& val);
//          // Construct 'CountedType' from the specified 'val'.
//
//      CountedType(const CountedType& original);
//          // Copy construct '*this' from the specified 'original' object.
//
//      CountedType(bslmf::MovableRef<CountedType> original);
//          // Move construct '*this' from 'original'.  If an exception is
//          // thrown, by the constructor for 'TYPE' 'original' is unchanged.
//
//      ~CountedType() { --s_count; }
//          // Destroy this object.
//
//      // MANIPULATORS
//      TYPE& value() { return d_value; }
//
//      // ACCESSORS
//      const TYPE& value() const { return d_value; }
//  };
//..
// Next, we implement 'MoveIfNoexcept', which calls 'move' on its argument,
// allowing it to convert back to an lvalue if the return type is an lvalue
// reference:
//..
//  template <class TYPE>
//  template <class TP>
//  inline typename
//  bsl::conditional<bsl::is_nothrow_move_constructible<TP>::value,
//                   bslmf::MovableRef<TP>, const TP&>::type
//  CountedType<TYPE>::MoveIfNoexcept(TP& x)
//  {
//      return bslmf::MovableRefUtil::move(x);
//  }
//..
// Next, we implement the value constructor and copy constructor, which simply
// copy their argument into the 'd_value' data members and increment the count:
//..
//  template <class TYPE>
//  CountedType<TYPE>::CountedType(const TYPE& val) : d_value(val)
//  {
//      ++s_count;
//  }
//
//  template <class TYPE>
//  CountedType<TYPE>::CountedType(const CountedType& original)
//      : d_value(original.d_value)
//  {
//      ++s_count;
//  }
//..
// We're now ready implement the move constructor.  Logically, we would simply
// move the value from 'original' into the 'd_value' member of '*this', but an
// exception thrown by 'TYPE''s move constructor would leave 'original' in a
// (valid but) unspecified state, violating the strong guarantee.  Instead, we
// move the value only if we know that the move will succeed; otherwise, we
// copy it.  This behavior is facilitated by the 'MoveIfNoexcept' function
// defined above:
//..
//  template <class TYPE>
//  CountedType<TYPE>::CountedType(bslmf::MovableRef<CountedType> original)
//      : d_value(
//          MoveIfNoexcept(bslmf::MovableRefUtil::access(original).d_value))
//  {
//      ++s_count;
//  }
//..
// Finally, we define the 's_count' member to complete the class
// implementation:
//..
//  template <class TYPE>
//  int CountedType<TYPE>::s_count = 0;
//..
// To test the 'CountedType' class template, assume a simple client type,
// 'SomeType' that makes it easy to detect if it was move constructed.
// 'SomeType' holds an 'int' value that is set to -1 when it is moved from, as
// shown here:
//..
//  class SomeType {
//      int d_value;
//  public:
//      SomeType(int v = 0) : d_value(v) { }                        // IMPLICIT
//      SomeType(const SomeType& original) : d_value(original.d_value) { }
//      SomeType(bslmf::MovableRef<SomeType> original)
//          : d_value(bslmf::MovableRefUtil::access(original).d_value)
//          { bslmf::MovableRefUtil::access(original).d_value = -1; }
//
//      int value() const { return d_value; }
//  };
//..
// Notice that 'SomeType' neglected to declare its move constructor as
// 'noexcept'.  This might be an oversight or it could be an old class that
// predates both 'noexcept' and the 'bsl::is_nothrow_move_constructible' trait.
// It is even be possible that the move constructor might throw (though, of
// course, it doesn't in this simplified example).  Regardless, the effect is
// that move-constructing a 'CountedType<SomeType>' will result in the move
// constructor actually performing a copy:
//..
//  void main()
//  {
//      CountedType<SomeType> obj1(1);
//      CountedType<SomeType> obj2(bslmf::MovableRefUtil::move(obj1));
//      assert(1 == obj1.value().value());  // Copied, not moved from
//      assert(1 == obj2.value().value());
//..
// For the purpose of this example, we can be sure that 'SomeThing' will not
// throw on move, at least not in our application.  In order to obtain the
// expected move optimization, we next wrap our 'SomeType in a
// 'bslalg::NothrowMovableWrapper':
//..
//      CountedType<bslalg::NothrowMovableWrapper<SomeType> >
//          obj3(SomeType(3));
//      CountedType<bslalg::NothrowMovableWrapper<SomeType> >
//          obj4(bslmf::MovableRefUtil::move(obj3));
//      assert(-1 == obj3.value().unwrap().value());  // moved from
//      assert(3 == obj4.value().unwrap().value());
//  }
//..
//
// Note that, in the last two lines of 'main', we must call 'unwrap' in order
// to access the 'SomeType' object inside of the 'NothrowMovableWrapper'.  This
// is one situation where it would be attractive to have an overloadable
// "operator dot" so that both 'CountedThing' and 'NothrowMovableWrapper' could
// be transparent proxy types.  C++ does not have overloadable operator dot,
// but we can create a 'CountedType' that is more intelligent about the
// existence of 'NothrowMovableWrapper' and automatically unwraps values for
// the user's convenience.
//
// Rather than starting from scratch, we'll build our new counted type,
// 'CountedType2' on 'CountedType'.  We start be defining a single data member
// of type 'CountedType':
//..
//  template <class TYPE>
//  class CountedType2 {
//      CountedType<TYPE> d_data;
//..
// Next, for convenience, we add a public data type, 'ValueType' for the value
// stored within 'CountedType2'.  However, rather than defining 'ValueType' as
// simply 'TYPE', we want to know if it is an instantiation of
// 'NothrowMovableWrapper<TP>'.  If it is, we want a type that represents the
// unwrapped 'TP' rather than the full 'TYPE'.  For this type transformation,
// we turn to the type traits defined in 'bslalg::NothrowMovableUtil':
//..
//  public:
//      // TYPES
//      typedef typename
//      bslalg::NothrowMovableUtil::UnwrappedType<TYPE>::type ValueType;
//..
// Note that the 'UnwrappedType' metafunction has no affect if 'TYPE' is not
// wrapped.
//
// Next, we declare (and define) the class functions, constructors, and
// destructor, simply forwarding to the corresponding 'CountedType' function,
// constructor, or destructor:
//..
//      // CLASS FUNCTIONS
//      static int count() { return CountedType<TYPE>::count(); }
//
//      // CREATORS
//      CountedType2(const TYPE& val) : d_data(val) { }
//      CountedType2(const CountedType2& original)
//          : d_data(original.d_data) { }
//      CountedType2(bslmf::MovableRef<CountedType2> original)
//          : d_data(bslmf::MovableRefUtil::move(
//                       bslmf::MovableRefUtil::access(original).d_data)) { }
//..
// Finally, we implement the 'value()' members such that the returned values do
// not need to be unwrapped.  As in the case of the 'UnwrappedType'
// metafunction, the 'unwrap()' function in 'NothrowMovableUtil' handles both
// wrapped and unwrapped arguments, unwrapping the latter and returning an
// unmodified reference to the former:
//..
//      // MANIPULATORS
//      ValueType& value()
//      {
//          return bslalg::NothrowMovableUtil::unwrap(d_data.value());
//          // Alternatively: 'return d_data.value();'
//      }
//
//      // ACCESSORS
//      const ValueType& value() const
//      {
//          return bslalg::NothrowMovableUtil::unwrap(d_data.value());
//          // Alternatively: 'return d_data.value();'
//      }
//  };
//..
// Note the alternative code for these members: A 'NothrowMovableWrapper<TP>'
// object is implicitly convertible to 'TP&', so if 'TYPE' is a
// 'NothrowMovableWrapper', the simple return statement will implicitly unwrap
// it.
//
// Using a similar example for 'CountedType2' as we used for 'CountedType', we
// see that the usage of 'CountedType2' with and without
// 'NothrowMovableWrapper' is the same:
//..
//  void main()
//  {
//      CountedType2<SomeType> obj1(1);
//      CountedType2<SomeType> obj2(bslmf::MovableRefUtil::move(obj1));
//      assert(1 == obj1.value().value());  // Copied, not moved from
//      assert(1 == obj2.value().value());
//
//      CountedType2<bslalg::NothrowMovableWrapper<SomeType> >
//          obj3(SomeType(3));
//      CountedType2<bslalg::NothrowMovableWrapper<SomeType> >
//          obj4(bslmf::MovableRefUtil::move(obj3));
//      assert(-1 == obj3.value().value());  // moved from
//      assert(3 == obj4.value().value());   // No need to call 'unwrap'
//  }
//..

#include <bslscm_version.h>

#include <bslalg_nothrowmovablewrapper.h>

#include <bslmf_assert.h>
#include <bslmf_conditional.h>
#include <bslmf_isarray.h>
#include <bslmf_isfunction.h>
#include <bslmf_movableref.h>
#include <bslmf_removecv.h>

namespace BloombergLP {
namespace bslalg {

// FORWARD DECLARATION
template <class TYPE, bool = bsl::is_function<TYPE>::value>
struct NothrowMovableUtil_Traits;

                         // =========================
                         // struct NothrowMovableUtil
                         // =========================

struct NothrowMovableUtil {
    // Namesapace for 'NothrowMovableWrapper' traits and utilities.

    // TRAITS
    template <class TYPE>
    struct IsWrapped : NothrowMovableUtil_Traits<TYPE>::IsWrapped {
        // Metafunction evaluating to 'true_type' if 'TYPE' is a specialization
        // of 'NothrowMovableWrapper'; otherwise, 'false_type'.
    };

    template <class TYPE>
    struct WrappedType {
        // Metafunction: If 'TYPE' is a specialization of
        // 'NothrowMovableWrapper', then 'WrappedType<TYPE>::type' is 'TYPE';
        // otherwise 'WrappedType<TYPE>::type' is
        // 'NothrowMovableWrapper<TYPE>'.

        typedef typename NothrowMovableUtil_Traits<TYPE>::WrappedType type;
    };

    template <class TYPE>
    struct UnwrappedType {
        // Metafunction: If 'TYPE' is a specialization of
        // 'NothrowMovableWrapper', then 'Unwrapped<TYPE>::type' is
        // 'TYPE::ValueType'; otherwise 'Unwrapped<TYPE>::type' is 'TYPE'.

        typedef typename NothrowMovableUtil_Traits<TYPE>::UnwrappedType type;
    };

    // CLASS METHODS
    template <class TYPE>
    static typename UnwrappedType<TYPE>::type& unwrap(TYPE& f);
    template <class TYPE>
    static typename UnwrappedType<TYPE>::type const& unwrap(TYPE const& f);
    template <class TYPE>
    static bslmf::MovableRef<const typename UnwrappedType<TYPE>::type>
        unwrap(BSLMF_MOVABLEREF_DEDUCE(const TYPE) f);
    template <class TYPE>
    static bslmf::MovableRef<typename UnwrappedType<TYPE>::type>
        unwrap(BSLMF_MOVABLEREF_DEDUCE(TYPE) f);
        // Return a reference to the object wrapped in the specified 'f'
        // object.  If 'f' is not wrapped, simply return a reference to 'f'.  Note
        // that the overloads taking an lvalue argument prevent the overload
        // taking an rvalue argument from treating the argument as a forwarding
        // reference.  The return type is chosen so both the value category and
        // constness of the parameter are preserved even in the case of const
        // rvalue references.

    template <class TYPE>
    static typename WrappedType<TYPE>::type wrap(TYPE& f);
    template <class TYPE>
    static typename WrappedType<TYPE>::type wrap(TYPE const& f);
    template <class TYPE>
    static typename WrappedType<TYPE>::type
        wrap(BSLMF_MOVABLEREF_DEDUCE(const TYPE) f);
    template <class TYPE>
    static typename WrappedType<TYPE>::type
        wrap(BSLMF_MOVABLEREF_DEDUCE(TYPE) f);
        // Return a wrapped copy of the specified 'f' object.  If 'f' is
        // already wrapped, return a simple copy of 'f' without wrapping it
        // again.  Note that the overloads taking an lvalue argument prevent
        // the overload taking an rvalue argument from treating the argument as
        // a forwarding reference.
};

}  // close package namespace

                  // ----------------------------------------
                  // class template NothrowMovableUtil_Traits
                  // ----------------------------------------

namespace bslalg {

template <class TYPE>
struct NothrowMovableUtil_Traits<TYPE, false /* is_function */> {
    // Component-private class -- do not use.  This specialization of traits is
    // for non-function types that are not wrapped.

    // Should not be instantiated on reference types or array types.  Assert
    // for 'IsReference' needs to be commented out because in C++03
    // 'NothrowMovableUtil_Traits' may be instantiated with a 'MovableRef'
    // while doing overload resolution for calls to 'wrap'/'unwrap'.
    // BSLMF_ASSERT(!bslmf::MovableRefUtil::IsReference<TYPE>::value);
    BSLMF_ASSERT(!bsl::is_array<TYPE>::value);

    //TYPES

    typedef bsl::false_type             IsWrapped;
    typedef TYPE                        UnwrappedType;
    typedef NothrowMovableWrapper<TYPE> WrappedType;
};

template <class TYPE>
struct NothrowMovableUtil_Traits<TYPE, true /* is_function */> {
    // Component-private class -- do not use.  This specialization of traits is
    // for function types that are not wrapped.  This specialization is NOT for
    // function pointers or function references.
    //
    // 'UnwrappedType' is an object type, and is thus the decayed version of
    // 'TYPE', i.e., a function pointer.  Since 'unwrap' always returns a
    // reference, this decay means that, 'unwrap(f)' will not compile if 'f' is
    // a function or reference to function.  However 'UnwrappedType pf = f;'
    // will work whether 'f' is a function or not.

    // TYPES
    typedef bsl::false_type                IsWrapped;
    typedef TYPE                          *UnwrappedType;
    typedef NothrowMovableWrapper<TYPE *>  WrappedType;
};

template <class TYPE>
struct NothrowMovableUtil_Traits<NothrowMovableWrapper<TYPE>, false> {
    // Component-private class -- do not use.  This specialization is for
    // wrapped types.

    // TYPES
    typedef bsl::true_type              IsWrapped;
    typedef TYPE                        UnwrappedType;
    typedef NothrowMovableWrapper<TYPE> WrappedType;
};

}  // close package namespace

// ============================================================================
//                TEMPLATE AND INLINE IMPLEMENTATIONS
// ============================================================================

                 // -----------------------------------------
                 // struct template NothrowMovableUtil
                 // -----------------------------------------

// PUBLIC CLASS METHODS

template <class TYPE>
inline
typename bslalg::NothrowMovableUtil::UnwrappedType<TYPE>::type&
bslalg::NothrowMovableUtil::unwrap(TYPE& f)
{
    return f;
}

template <class TYPE>
inline
typename bslalg::NothrowMovableUtil::UnwrappedType<TYPE>::type const&
bslalg::NothrowMovableUtil::unwrap(TYPE const& f)
{
    return f;
}

template <class TYPE>
inline
bslmf::MovableRef<
    const typename bslalg::NothrowMovableUtil::UnwrappedType<TYPE>::type>
    bslalg::NothrowMovableUtil::unwrap(BSLMF_MOVABLEREF_DEDUCE(const TYPE) f)
{
    const typename bslalg::NothrowMovableUtil::UnwrappedType<TYPE>::type& r =
                                              bslmf::MovableRefUtil::access(f);
    return bslmf::MovableRefUtil::move(r);
}

template <class TYPE>
inline
bslmf::MovableRef<
    typename bslalg::NothrowMovableUtil::UnwrappedType<TYPE>::type>
    bslalg::NothrowMovableUtil::unwrap(BSLMF_MOVABLEREF_DEDUCE(TYPE) f)
{
    typename bslalg::NothrowMovableUtil::UnwrappedType<TYPE>::type& r =
                                              bslmf::MovableRefUtil::access(f);
    return bslmf::MovableRefUtil::move(r);
}

template <class TYPE>
inline
typename bslalg::NothrowMovableUtil::WrappedType<TYPE>::type
bslalg::NothrowMovableUtil::wrap(TYPE& f)
{
    return f;
}

template <class TYPE>
inline
typename bslalg::NothrowMovableUtil::WrappedType<TYPE>::type
bslalg::NothrowMovableUtil::wrap(const TYPE& f)
{
    return f;
}

template <class TYPE>
inline
typename bslalg::NothrowMovableUtil::WrappedType<TYPE>::type
    bslalg::NothrowMovableUtil::wrap(BSLMF_MOVABLEREF_DEDUCE(const TYPE) f)
{
    return typename WrappedType<TYPE>::type(bslmf::MovableRefUtil::move(f));
}

template <class TYPE>
inline
typename bslalg::NothrowMovableUtil::WrappedType<TYPE>::type
    bslalg::NothrowMovableUtil::wrap(
        BSLMF_MOVABLEREF_DEDUCE(TYPE) f)
{
    return
        typename WrappedType<typename bslmf::MovableRefUtil::RemoveReference<
            TYPE>::type>::type(bslmf::MovableRefUtil::move(f));
}

}  // close enterprise namespace

#endif  // ! defined(INCLUDED_BSLALG_NOTHROWMOVABLEUTIL)

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
