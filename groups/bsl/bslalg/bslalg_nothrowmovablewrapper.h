// bslalg_nothrowmovablewrapper.h                                     -*-C++-*-

#ifndef INCLUDED_BSLALG_NOTHROWMOVABLEWRAPPER
#define INCLUDED_BSLALG_NOTHROWMOVABLEWRAPPER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a wrapper that asserts a noexcept move constructor.
//
//@CLASSES:
// bslalg::NothrowMovableWrapper: wrapper class with noexcept move constructor
//
//@SEE_ALSO: bslalg_movablewrapperutil
//
//@DESCRIPTION:  This component provides a wrapper class template
// 'bslalg::NothrowMovableWrapper<TYPE>' holding an object of 'TYPE' and
// providing no other functionality other than returning the wrapped object.
// The use of this class communicates to specific clients (see
// 'bslstl_function') that the wrapped object should be treated as-if it has a
// 'noexcept' move constructor, even in C++03, where 'noexcept' does not exist.
// The client might, for example, move the object using efficient,
// non-exception-safe logic rather than, e.g., copying the object or storing it
// in heap memory so that its pointer can be moved.  The behavior is undefined
// if the move constructor is invoked and *does* throw; typically resulting in
// 'terminate' being invoked.
//
//
///Usage
///-----
//
///Example 1
///- - - - -
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
// exception thrown by 'TYPE's move constructor would leave 'original' in a
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

#include <bslscm_version.h>

#include <bslma_constructionutil.h>
#include <bslma_stdallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_allocatorargt.h>
#include <bslmf_assert.h>
#include <bslmf_conditional.h>
#include <bslmf_isarray.h>
#include <bslmf_isfunction.h>
#include <bslmf_isnothrowmoveconstructible.h>
#include <bslmf_movableref.h>
#include <bslmf_nestedtraitdeclaration.h>
#include <bslmf_usesallocatorargt.h>

#include <bsls_keyword.h>
#include <bsls_objectbuffer.h>

namespace BloombergLP {

namespace bslalg {

                    // ====================================
                    // class template NothrowMovableWrapper
                    // ====================================

template <class TYPE>
class NothrowMovableWrapper {
    // An object of this type wraps a value of the specified 'TYPE', and
    // provides no other functionality other than returning the wrapped object.
    // The move constructor is guaranteed not to throw, even if the move
    // constructor for 'TYPE' has no such guarantee.  The user is thus
    // asserting that the move constructor for the wrapped object *will not*
    // throw, even if it is allowed to.  Constraints: this class can be
    // instantiated on object types only, i.e., not references, arrays, or
    // function types (though function pointers are OK).

    // Cannot wrap reference types, array types, or function types.
    BSLMF_ASSERT(!bslmf::MovableRefUtil::IsReference<TYPE>::value);
    BSLMF_ASSERT(!bsl::is_array<TYPE>::value);
    BSLMF_ASSERT(!bsl::is_function<TYPE>::value);

    // PRIVATE TYPES
    struct DummyAllocator {
        // Private type that prevents allocator-argument overloads from
        // participating in overload resolution if 'TYPE' is not allocator
        // aware.  Does not meet the allocator requirements (or any other
        // requirements) and cannot be constructed by users.
    };

    typedef bslmf::MovableRefUtil MovableRefUtil;

    typedef typename bsl::remove_cv<TYPE>::type StoredType;

    // DATA
    bsls::ObjectBuffer<StoredType> d_buffer;

    // NOT IMPLEMENTED
    NothrowMovableWrapper& operator=(
                            const NothrowMovableWrapper&) BSLS_KEYWORD_DELETED;
        // Not assignable.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(NothrowMovableWrapper,
                                   bsl::is_nothrow_move_constructible);

    BSLMF_NESTED_TRAIT_DECLARATION_IF(NothrowMovableWrapper,
                                      bslma::UsesBslmaAllocator,
                                      bslma::UsesBslmaAllocator<TYPE>::value);

    // If this wrapper is allocator-aware (because 'TYPE' is allocator-aware),
    // then choose the leading-allocator convention.
    BSLMF_NESTED_TRAIT_DECLARATION_IF(NothrowMovableWrapper,
                                      bslmf::UsesAllocatorArgT,
                                      bslma::UsesBslmaAllocator<TYPE>::value);

    BSLMF_NESTED_TRAIT_DECLARATION_IF(NothrowMovableWrapper,
                                      bslmf::IsBitwiseMoveable,
                                      bslmf::IsBitwiseMoveable<TYPE>::value);

    // TYPES
    typedef typename bsl::conditional<bslma::UsesBslmaAllocator<TYPE>::value,
                                      bsl::allocator<char>,
                                      DummyAllocator>::type allocator_type;
        // Type of allocator to use.  If 'TYPE' is not allocator-aware, then
        // this is a private dummy type that will disable use of any
        // constructor that takes an allocator.

    typedef TYPE ValueType;

    // CREATORS
    NothrowMovableWrapper();
    NothrowMovableWrapper(bsl::allocator_arg_t, const allocator_type& alloc);
        // Value-initialize the object wrapped by '*this'.  For allocator-aware
        // 'TYPE', optionally specify an 'alloc' (e.g., the address of a
        // 'bslma::Allocator' object) to supply memory; otherwise, the default
        // allocator is used.

    NothrowMovableWrapper(const TYPE& val);                         // IMPLICIT
    NothrowMovableWrapper(bsl::allocator_arg_t,
                          const allocator_type& alloc,
                          const TYPE&           val);
        // Wrap the specified 'val', using 'TYPE's (possibly extended) copy
        // constructor.  For allocator-aware 'TYPE', optionally specify an
        // 'alloc' (e.g., the address of a 'bslma::Allocator' object) to supply
        // memory; otherwise, the default allocator is used.

    NothrowMovableWrapper(bslmf::MovableRef<TYPE> val);             // IMPLICIT
        // Wrap the specified 'val', using 'TYPE's move constructor.

    NothrowMovableWrapper(bsl::allocator_arg_t,
                          const allocator_type&   alloc,
                          bslmf::MovableRef<TYPE> val);
        // Wrap the specified 'val', using 'TYPE's extended move constructor.
        // Use the specified 'alloc' (e.g., the address of a 'bslma::Allocator'
        // object) to supply memory.  Note that this constructor will not be
        // selected by overload resolution unless 'TYPE' is allocator aware.

    NothrowMovableWrapper(const NothrowMovableWrapper& original);
        // Copy construct from the specified 'original' wrapper using 'TYPE's
        // copy constructor.

    NothrowMovableWrapper(bsl::allocator_arg_t,
                          const allocator_type&        alloc,
                          const NothrowMovableWrapper& original);
        // Copy construct from the specified 'original' wrapper using 'TYPE's
        // extended copy constructor.  Use the specified 'alloc' (e.g., the
        // address of a 'bslma::Allocator' object) to supply memory.  Note that
        // this constructor will not be selected by overload resolution unless
        // 'TYPE' is allocator aware.

    NothrowMovableWrapper(
      bslmf::MovableRef<NothrowMovableWrapper> original) BSLS_KEYWORD_NOEXCEPT;
                                                                    // IMPLICIT
        // Move construct from the specified 'original' wrapper using 'TYPE's
        // move constructor.  Note that this move constructor is
        // unconditionally 'noexcept', as that is the entire purpose of this
        // wrapper.

    NothrowMovableWrapper(bsl::allocator_arg_t,
                          const allocator_type&                    alloc,
                          bslmf::MovableRef<NothrowMovableWrapper> original);
        // Move construct from the specified 'original' wrapper using 'TYPE's
        // extended move constructor.  Use the specified 'alloc' (e.g., the
        // address of a 'bslma::Allocator' object) to supply memory.  Note that
        // this constructor will not be selected by overload resolution unless
        // 'TYPE' is allocator aware.

    ~NothrowMovableWrapper();
        // Destroy this object, invoking 'TYPE's destructor.

    // MANIPULATORS
    ValueType& unwrap();
        // Return a reference offering modifiable access to the wrapped
        // object.

    operator ValueType&()
        // Return a reference offering modifiable access to the wrapped
        // object.
    {
        // Must be in-place inline to work around MSVC 2013 bug.
        return unwrap();
    }

    // ACCESSORS
    allocator_type get_allocator() const;
        // Return the allocator used to construct this object.  Note that this
        // method will fail to instantiate unless 'TYPE' is allocator-aware.

    ValueType const& unwrap() const;
        // Return a reference offering const access to the wrapped object.

    operator ValueType const&() const
        // Return a reference offering const access to the wrapped object.
    {
        // Must be in-place inline to work around MSVC 2013 bug.
        return unwrap();
    }
};

template <class TYPE>
class NothrowMovableWrapper<NothrowMovableWrapper<TYPE> > {
    // This specialization is for wrapped types.  We do not support wrapping a
    // wrapped type.
    BSLMF_ASSERT(!sizeof(TYPE) && "Cannot wrap a wrapped object");
};

template <class TYPE>
class NothrowMovableWrapper<const NothrowMovableWrapper<TYPE> > {
    // This specialization is for wrapped types.  We do not support wrapping a
    // wrapped type.
    BSLMF_ASSERT(!sizeof(TYPE) && "Cannot wrap a wrapped object");
};

}  // close package namespace

                    // ------------------------------------
                    // class template NothrowMovableWrapper
                    // ------------------------------------

// CREATORS
template <class TYPE>
inline
bslalg::NothrowMovableWrapper<TYPE>::NothrowMovableWrapper()
{
    bslma::ConstructionUtil::construct(d_buffer.address(), (void *)0);
}

template <class TYPE>
inline
bslalg::NothrowMovableWrapper<TYPE>::NothrowMovableWrapper(
                                                   bsl::allocator_arg_t,
                                                   const allocator_type& alloc)
{
    bslma::ConstructionUtil::construct(d_buffer.address(), alloc.mechanism());
}

template <class TYPE>
inline
bslalg::NothrowMovableWrapper<TYPE>::NothrowMovableWrapper(const TYPE& val)
{
    bslma::ConstructionUtil::construct(d_buffer.address(), (void *)0, val);
}

template <class TYPE>
inline
bslalg::NothrowMovableWrapper<TYPE>::NothrowMovableWrapper(
                                                   bsl::allocator_arg_t,
                                                   const allocator_type& alloc,
                                                   const TYPE&           val)
{
    bslma::ConstructionUtil::construct(
        d_buffer.address(), alloc.mechanism(), val);
}

template <class TYPE>
inline
bslalg::NothrowMovableWrapper<TYPE>::NothrowMovableWrapper(
                                                   bslmf::MovableRef<TYPE> val)
{
    bslma::ConstructionUtil::construct(
        d_buffer.address(), (void *)0, bslmf::MovableRefUtil::move(val));
}

template <class TYPE>
inline
bslalg::NothrowMovableWrapper<TYPE>::NothrowMovableWrapper(
                                                 bsl::allocator_arg_t,
                                                 const allocator_type&   alloc,
                                                 bslmf::MovableRef<TYPE> val)
{
    bslma::ConstructionUtil::construct(d_buffer.address(),
                                       alloc.mechanism(),
                                       bslmf::MovableRefUtil::move(val));
}

template <class TYPE>
inline
bslalg::NothrowMovableWrapper<TYPE>::NothrowMovableWrapper(
                                         const NothrowMovableWrapper& original)
{
    bslma::ConstructionUtil::construct(
        d_buffer.address(), (void *)0, original.unwrap());
}

template <class TYPE>
inline
bslalg::NothrowMovableWrapper<TYPE>::NothrowMovableWrapper(
                                         bsl::allocator_arg_t,
                                         const allocator_type&        alloc,
                                         const NothrowMovableWrapper& original)
{
    bslma::ConstructionUtil::construct(
        d_buffer.address(), alloc.mechanism(), original.unwrap());
}

template <class TYPE>
inline
bslalg::NothrowMovableWrapper<TYPE>::NothrowMovableWrapper(
       bslmf::MovableRef<NothrowMovableWrapper> original) BSLS_KEYWORD_NOEXCEPT
{
    bslma::ConstructionUtil::construct(
        d_buffer.address(),
        (void *)0,
        bslmf::MovableRefUtil::move(
            bslmf::MovableRefUtil::access(original).unwrap()));
}

template <class TYPE>
inline
bslalg::NothrowMovableWrapper<TYPE>::NothrowMovableWrapper(
                             bsl::allocator_arg_t,
                             const allocator_type&                    alloc,
                             bslmf::MovableRef<NothrowMovableWrapper> original)
{
    bslma::ConstructionUtil::construct(
        d_buffer.address(),
        alloc.mechanism(),
        bslmf::MovableRefUtil::move(
            bslmf::MovableRefUtil::access(original).unwrap()));
}

template <class TYPE>
inline
bslalg::NothrowMovableWrapper<TYPE>::~NothrowMovableWrapper()
{
    d_buffer.object().~TYPE();
}

// MANIPULATORS
template <class TYPE>
inline
typename bslalg::NothrowMovableWrapper<TYPE>::ValueType&
bslalg::NothrowMovableWrapper<TYPE>::unwrap()
{
    return d_buffer.object();
}

// ACCESSORS
template <class TYPE>
inline
typename bslalg::NothrowMovableWrapper<TYPE>::allocator_type
bslalg::NothrowMovableWrapper<TYPE>::get_allocator() const
{
    return d_buffer.object().allocator();
}

template <class TYPE>
inline
typename bslalg::NothrowMovableWrapper<TYPE>::ValueType const&
bslalg::NothrowMovableWrapper<TYPE>::unwrap() const
{
    return d_buffer.object();
}

}  // close enterprise namespace

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
// 'bslalg::NothrowMovableWrapper' replaced 'bsl::Function_NothrowWrapper'.
// These declarations exist for backward compatibility.
namespace bsl {
namespace bsl_deprecated {

using BloombergLP::bslalg::NothrowMovableWrapper;

}  // close namespace bsl_deprecated
}  // close namespace bsl

#define Function_NothrowWrapper bsl_deprecated::NothrowMovableWrapper
    // !DEPRECATED! Use 'bslalg::NothrowMovableWrapper' instead.
    // The old name, 'bsl::Function_NothrowWrapper' will expand to
    // 'bsl::bsl_deprecated::NothrowMovableWrapper', which will resolve to
    // 'BloombergLP::bslalg::NothrowMovableWrapper'.  Note that not all of the
    // 'Function_NothrowWrapper' interface has been duplicated in
    // 'NothrowMovableWrapper', but the single-argument constructor is the only
    // part of the interface known to be used in legacy code.

#endif // BDE_OMIT_INTERNAL_DEPRECATED

#endif  // ! defined(INCLUDED_BSLALG_NOTHROWMOVABLEWRAPPER)

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
