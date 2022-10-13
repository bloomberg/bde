// bslma_aatypeutil.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLMA_AATYPEUTIL
#define INCLUDED_BSLMA_AATYPEUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a namespace for utility functions on allocator-aware types.
//
//@CLASSES:
//  bslma::AATypeUtil: namespace for utility functions on allocator-aware types
//
//@SEE_ALSO: bslma_aamodel
//
//@DESCRIPTION: This component provides a namespace 'struct',
// 'bslma::AATypeUtil', that provides functions for extracting the allocator
// from an allocator-aware (AA) type.  The functions in this 'struct' choose
// the appropriate machinery to get the allocator from an object of a specified
// 'TYPE' based on the AA model supported by 'TYPE'.  For example, if
// 'AAModel<TYPE>' is 'AAModelLegacy' (see 'bslma_aamodel'), then the allocator
// is extracted using the 'allocator' mechanism.  Some of the methods in this
// component are used to request allocators of a specific type; if the object's
// allocator is not convertible to that type, then the program will not
// compile.
//
///Usage
///-----
//
///Example 1: Constructing a New Member Using an Existing Member's Allocator
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example illustrates how 'bslma::AATypeUtil::getAdaptedAllocator'
// can be used to extract the allocator from an Allocator-Aware (AA) object and
// use it to construct a different AA object without regard to whether either
// object is *legacy-AA* (using 'bslma::Allocator *') or *bsl-AA* (using
// 'bsl::allocator').  We begin by defining two *legacy-AA* classes, 'Larry',
// and 'Curly':
//..
//  class Larry {
//      // A *legacy-AA* class.
//
//      // DATA
//      bslma::Allocator *d_allocator_p;
//      int               d_value;
//
//  public:
//      // TYPE TRAITS
//      BSLMF_NESTED_TRAIT_DECLARATION(Larry, bslma::UsesBslmaAllocator);
//
//      // CREATORS
//      explicit Larry(int v, bslma::Allocator *basicAllocator = 0)
//          : d_allocator_p(bslma::Default::allocator(basicAllocator))
//          , d_value(v) { }
//
//      // ACCESSORS
//      bslma::Allocator *allocator() const { return d_allocator_p; }
//      int               value()     const { return d_value; }
//  };
//
//  class Curly {
//      // Another *legacy-AA* class.
//
//      // DATA
//      bslma::Allocator *d_allocator_p;
//      int               d_value;
//
//  public:
//      // TYPE TRAITS
//      BSLMF_NESTED_TRAIT_DECLARATION(Curly, bslma::UsesBslmaAllocator);
//
//      // CREATORS
//      explicit Curly(int v, bslma::Allocator *basicAllocator = 0)
//          : d_allocator_p(bslma::Default::allocator(basicAllocator))
//          , d_value(v) { }
//
//      // ACCESSORS
//      bslma::Allocator *allocator() const { return d_allocator_p; }
//      int               value()     const { return d_value; }
//  };
//..
// Next, consider a class, 'LarryMaybeCurly', that holds a 'Larry' object and
// optionally, holds a 'Curly' object.  The data members for 'LarryMaybeCurly'
// include a 'Larry' object, a flag indicating the existence of a 'Curly'
// object, and an aligned buffer to hold the optional 'Curly' object, if it
// exists.  Because the 'Larry' member holds an allocator, there is no need for
// a separate allocator data member:
//..
//  class LarryMaybeCurly {
//      // Holds a 'Larry' object and possibly a 'Curly' object.
//
//      // DATA
//      bool                      d_hasCurly; // True if 'd_curly' is populated
//      Larry                     d_larry;
//      bsls::ObjectBuffer<Curly> d_curly;    // Maybe holds a 'Curly' object
//..
// Next we complete the public interface, which includes a constructor that
// sets the value of the 'Larry' object, a manipulator for setting the value of
// the 'Curly' object, and accessors for retrieving the 'Larry' and 'Curly'
// objects.  Because 'LarryMaybeCurly' is allocator-aware (AA), we must have an
// 'allocator_type' member, and a 'get_allocator' accessor; every constructor
// should also take an optional allocator argument.
//..
//    public:
//      // TYPES
//      typedef bsl::allocator<char> allocator_type;
//
//      // CREATORS
//      explicit LarryMaybeCurly(int                   v,
//                               const allocator_type& a = allocator_type());
//          // Create an object having a 'Larry' member with the specified 'v'
//          // value and having no 'Curly' member.  Optionally specify an
//          // allocator 'a' to supply memory.
//
//      // ...
//
//      // MANIPULATORS
//      void setCurly(int v);
//          // initialize the 'Curly' member to the specified 'v' value.
//
//      // ACCESSORS
//      bool         hasCurly() const { return d_hasCurly; }
//      const Larry& larry()    const { return d_larry; }
//      const Curly& curly()    const { return d_curly.object(); }
//
//      allocator_type get_allocator() const;
//  };
//..
// Now we implement the constructor that initializes value of the 'Larry'
// member and leaves the 'Curly' member unset.  Notice that we use
// 'bslma::AllocatorUtil::adapt' to smooth out the mismatch between the
// 'bsl::allocator' used by 'LarryMaybeCurly' and the 'bslma::Allocator *'
// expected by 'Larry'.
//..
//  LarryMaybeCurly::LarryMaybeCurly(int v, const allocator_type& a)
//      : d_hasCurly(false), d_larry(v, bslma::AllocatorUtil::adapt(a)) { }
//..
// Next, we implement the manipulator for setting the 'Curly' object.  This
// manipulator must use the allocator stored in 'd_larry'.  The function,
// 'getAdaptedAllocator' yields this allocator in a form that can be
// consumed by the 'Curly' constructor:
//..
//      // MANIPULATORS
//  void LarryMaybeCurly::setCurly(int v)
//  {
//      new (d_curly.address())
//          Curly(v, bslma::AATypeUtil::getAdaptedAllocator(d_larry));
//      d_hasCurly = true;
//  }
//..
// Finally, we can use a test allocator to verify that, when a
// 'LarryMaybeCurly' object is constructed with an allocator, that same
// allocator is used to construct both the 'Larry' and 'Curly' objects within
// it:
//..
//  int main()
//  {
//      bslma::TestAllocator ta;
//      bsl::allocator<char> bslAlloc(&ta);
//
//      LarryMaybeCurly obj1(5, bslAlloc);
//      assert(5   == obj1.larry().value());
//      assert(&ta == obj1.larry().allocator());
//      assert(! obj1.hasCurly());
//
//      obj1.setCurly(10);
//      assert(5   == obj1.larry().value());
//      assert(&ta == obj1.larry().allocator());
//      assert(obj1.hasCurly());
//      assert(10  == obj1.curly().value());
//      assert(&ta == obj1.curly().allocator());
//..
// It may not be immediately obvious that 'getAdaptedAllocator' provides
// much benefit; indeed, the example would work just fine if we called
// 'Larry::allocator()' and passed the result directly to the constructor of
// 'Curly':
//..
//      Larry larryObj(5, &ta);
//      Curly curlyObj(10, larryObj.allocator());
//      assert(&ta == curlyObj.allocator());
//
//      return 0;
//  }
//..
// The code above is brittle, however, as updating 'Larry' to be *bsl-AA* would
// require calling 'larryObj.get_allocator().mechanism()' instead of
// 'larryObj.allocator().  By using 'getAdaptedAllocator', the 'setCurly'
// implementation above is robust in the face of such future evolution.  This
// benefit is even more important in generic code, especially when *pmr-AA*
// types are added into the mix in the future.
//
///Example 2: Retrieving a Specific Allocator Type from a Subobject
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example illustrates how 'bslma::AATypeUtil::getAllocatorFromSubobject'
// can be used to retrieve an allocator of a specific type from a subobject
// even if that subobject uses an allocator with a smaller interface.
//
// First, continuing from the previous example, we implement the
// 'get_allocator' accessor.  As we know, the allocator for a 'LarryMaybeCurly'
// object is stored in the 'd_larry' subobject, obviating a separate
// 'd_allocator_p' member.  However, the allocator within 'd_larry' is a
// 'bslma::Allocator *' whereas the 'allocator_type' for 'LarryMaybeCurly' is
// 'bsl::allocator<char>'.  When the 'LarryMaybeCurly' object was constructed,
// some type information was lost in the conversion to 'bslma::Allocator'.
// That information is recovered through the use of
// 'getAllocatorFromSubobject':
//..
//  bsl::allocator<char> LarryMaybeCurly::get_allocator() const
//  {
//      typedef bslma::AATypeUtil Util;
//      return Util::getAllocatorFromSubobject<allocator_type>(d_larry);
//  }
//..
// Now we can construct a 'LarryMaybeCurly' object with a specific allocator
// and recover that allocator using the 'get_allocator' accessor:
//..
//  int main()
//  {
//      bslma::TestAllocator ta;
//      bsl::allocator<char> bslAlloc(&ta);
//
//      LarryMaybeCurly obj1(5, bslAlloc);
//      assert(bslAlloc == obj1.get_allocator());
//..
// As in the previous example, it is possible to get the same effect without
// using the utilities in this component because 'bslma::Allocator *' is
// implicitly convertible to 'bsl::allocator<char>':
//..
//      Larry                larryObj(5, &ta);
//      bsl::allocator<char> objAlloc = larryObj.allocator();
//      assert(objAlloc == bslAlloc);
//      return 0;
//  }
//..
// However, the preceding 'get_allocator' implementation, like the 'setCurly'
// implementation in the previous example, is more robust because it need not
// be changed if 'Larry' is changed from *legacy-AA* to *bsl-AA* or if it is
// replaced by a template parameter that might use either AA model.  When
// *pmr-AA* is added to the Bloomberg codebase, using the
// 'getAllocatorFromSubobject' idiom will be vital to recovering 'bsl'
// allocators stored within *pmr-AA* objects.

#include <bslscm_version.h>

#include <bslma_aamodel.h>
#include <bslma_allocatorutil.h>

#include <bslmf_enableif.h>

namespace BloombergLP {
namespace bslma {

                        // =================
                        // struct AATypeUtil
                        // =================

struct AATypeUtil {
    // Namespace for utility functions on allocator-aware types

    // CLASS METHODS
    template <class TYPE>
    static
    typename bsl::enable_if<AAModel<TYPE>::value == AAModelLegacy::value ||
                            AAModel<TYPE>::value == AAModelBsl::value,
                            bslma::Allocator *>::type
    getAdaptedAllocator(const TYPE& object);
        // Return the allocator held by the specified 'object', adapted to be
        // usable as a constructor argument for the widest possible set of AA
        // types.  This overload will be selected if 'TYPE' is *bsl-AA* or
        // *legacy-AA*.

    template <class TYPE>
    static
    typename bsl::enable_if<AAModel<TYPE>::value != AAModelBsl::value,
                            typename TYPE::allocator_type>::type
    getAdaptedAllocator(const TYPE& object);
        // Return the allocator held used by the specified 'object'.  This
        // overload will be selected if 'TYPE' is *pmr-AA* or *stl-AA*.

    template <class ALLOCATOR, class TYPE>
    static
    ALLOCATOR getAllocatorFromSubobject(const TYPE& object);
        // Return the allocator for the specified 'object', converted to the
        // specified 'ALLOCATOR' template argument, which must be explicitly
        // provided.  This function can recover allocator-type information if
        // 'TYPE' holds an allocator that is compatible with 'ALLOCATOR', but
        // is not necessarily directly convertible -- e.g., 'TYPE' is *bsl-AA*
        // (uses 'bsl::allocator') and 'ALLOCATOR' is 'bslma::Allocator *'.
        // The behavior is undefined unless 'object' was constructed with an
        // allocator that is convertible to 'ALLOCATOR' without loss of
        // information.

    template <class TYPE>
    static bsl::allocator<char> getBslAllocator(const TYPE& object);
        // Return the allocator for the specified 'object', converted to
        // 'bsl::allocator<char>'.  Instantiation will fail unless 'TYPE' is
        // allocator-aware and has an allocator *type* that is statically
        // convertible to 'bsl::allocator'.  The behavior is undefined unless
        // 'object' was constructed with an allocator *value* that is
        // dynamically convertible to a valid 'bsl::allocator'.

    template <class TYPE>
    static
    typename bsl::enable_if<AAModel<TYPE>::value == AAModelLegacy::value,
                            Allocator *>::type
    getNativeAllocator(const TYPE& object);
        // Return the address of the 'bslma::Allocator' used by the specified
        // AA 'object'.  This function does not participate in overload
        // resolution unless 'TYPE' is *legacy-AA* (i.e., uses
        // 'bslma::Allocator *' as its allocator vocabulary type).  Note that
        // instantiation will fail unless 'object.allocator()' is well formed
        // and returns a type convertible to 'bslma::Allocator *'.

    template <class TYPE>
    static
    typename TYPE::allocator_type getNativeAllocator(const TYPE& object);
        // Return the allocator used by the specified AA 'object'.  This
        // function does not participate in overload resolution unless 'TYPE'
        // has an 'allocator_type' member.  Note that instantiation will fail
        // unless 'object.get_allocator()' is well formed and returns a type
        // convertible to 'TYPE::allocator_type'.
};

}  // close package namespace

// ============================================================================
//                TEMPLATE AND INLINE FUNCTION IMPLEMENTATIONS
// ============================================================================

namespace bslma {

// CLASS METHODS
template <class TYPE>
inline
typename bsl::enable_if<AAModel<TYPE>::value == AAModelLegacy::value ||
                        AAModel<TYPE>::value == AAModelBsl::value,
                        bslma::Allocator *>::type
AATypeUtil::getAdaptedAllocator(const TYPE& object)
{
    return AllocatorUtil::adapt(getNativeAllocator(object));
}

template <class TYPE>
inline
typename bsl::enable_if<AAModel<TYPE>::value != AAModelBsl::value,
                        typename TYPE::allocator_type>::type
AATypeUtil::getAdaptedAllocator(const TYPE& object)
{
    return AllocatorUtil::adapt(getNativeAllocator(object));
}

template <class ALLOCATOR, class TYPE>
inline
ALLOCATOR AATypeUtil::getAllocatorFromSubobject(const TYPE& object)
{
    return static_cast<ALLOCATOR>(
                             AllocatorUtil::adapt(getNativeAllocator(object)));
}

template <class TYPE>
inline
bsl::allocator<char> AATypeUtil::getBslAllocator(const TYPE& object)
{
    // TBD: When 'bsl::polymorphic_allocator' is introduced, add defensive
    // checks that 'object.get_allocator().resource()' can be dynamically
    // cast to 'bslma::Allocator *'.
    return static_cast<bsl::allocator<char> >(getNativeAllocator(object));
}

template <class TYPE>
inline
typename bsl::enable_if<AAModel<TYPE>::value == AAModelLegacy::value,
                        Allocator *>::type
AATypeUtil::getNativeAllocator(const TYPE& object)
{
    return object.allocator();
}

template <class TYPE>
inline
typename TYPE::allocator_type
AATypeUtil::getNativeAllocator(const TYPE& object)
{
    return object.get_allocator();
}

}  // close package namespace
}  // close enterprise namespace

#endif // ! defined(INCLUDED_BSLMA_AATYPEUTIL)

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
