// bsltf_stdalloctesttype.h                                           -*-C++-*-
#ifndef INCLUDED_BSLTF_STDALLOCTESTTYPE
#define INCLUDED_BSLTF_STDALLOCTESTTYPE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a class for testing that allocates with standard allocator
//
//@CLASSES:
//  bsltf::StdAllocTestType: test class that allocates using standard allocator
//
//@SEE_ALSO: bsltf_templatetestfacility
//
//@DESCRIPTION: This component provides a single, unconstrained
// (value-semantic) attribute class, 'StdAllocTestType', that takes a standard
// allocator type as a template argument and uses an object of that type to
// allocate memory.  Note that this class does NOT set the trait
// 'bslma::UsesBslmaAllocator', but the trait will automatically be 'true' if
// 'ALLOC' is convertible from 'bslma::Allocator *' (e.g., if it is an
// instantiation of 'bsl::allocator').  Furthermore, this class is not
// bitwise-moveable, and will assert on destruction if it has been moved.  This
// class is primarily provided to facilitate testing of templates by defining a
// simple type representative of user-defined types using a standard allocator.
//
///Attributes
///----------
//..
//  Name                Type         Default
//  ------------------  -----------  -------
//  data                int          0
//..
//: o 'data': representation of the object's value
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Printing the Supported Traits of This Type
///- - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we wanted to print the supported traits of this test type.
//
// First, we create a function template 'printTypeTraits' with a parameterized
// 'TYPE':
//..
//  template <class TYPE>
//  void printTypeTraits()
//      // Prints the traits of the parameterized 'TYPE' to the console.
//  {
//      if (bslma::UsesBslmaAllocator<TYPE>::value) {
//          printf("Type defines bslma::UsesBslmaAllocator.\n");
//      }
//      else {
//          printf("Type does not define bslma::UsesBslmaAllocator.\n");
//      }
//
//      if (bslmf::IsBitwiseMoveable<TYPE>::value) {
//          printf("Type defines bslmf::IsBitwiseMoveable.\n");
//      }
//      else {
//          printf("Type does not define bslmf::IsBitwiseMoveable.\n");
//      }
//  }
//..
// Next, we create an STL-style allocator:
//..
//  template <class TYPE>
//  struct StlAllocator {
//      // An STL-compliant allocator type.
//
//      typedef TYPE value_type;
//
//      TYPE *allocate(std::size_t);
//      void deallocate(TYPE *, std::size_t);
//  };
//..
// Now, we invoke the 'printTypeTraits' function template using 'AllocTestType'
// as the parameterized 'TYPE', using both 'bsl::allocator' and 'StlAllocator'
// as the 'ALLOC' parameter:
//..
//  printTypeTraits<StdAllocTestType<bsl::allocator<int> > >();
//  printTypeTraits<StdAllocTestType<StlAllocator<int> > >();
//..
// Finally, we observe the console output:
//..
//  Type defines bslma::UsesBslmaAllocator.
//  Type does not define bslmf::IsBitwiseMoveable.
//  Type does not bslma::UsesBslmaAllocator.
//  Type does not define bslmf::IsBitwiseMoveable.
//..

#include <bslscm_version.h>

#include <bslma_allocator.h>
#include <bslma_allocatortraits.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_isconvertible.h>
#include <bslmf_nestedtraitdeclaration.h>

namespace BloombergLP {
namespace bsltf {

                        // ======================
                        // class StdAllocTestType
                        // ======================

template <class ALLOC>
class StdAllocTestType {
    // This unconstrained (value-semantic) attribute class uses a standard
    // allocator of (template parameter) type 'ALLOC' to allocate memory.  This
    // class does NOT define the type trait 'bslma::UsesBslmaAllocator', but
    // the trait will be 'true' if 'ALLOC' is convertible from
    // 'bslma::Allocator *' (e.g., if it is an instantiation of
    // 'bsl::allocator').  This class is primarily provided to facilitate
    // testing of templates by defining a simple type representative of
    // user-defined types using a standard allocator.  See the Attributes
    // section under @DESCRIPTION in the component-level documentation for
    // information on the class attributes.

    // DATA
    int                     *d_data_p;      // pointer to the data value

    ALLOC                    d_allocator;   // allocator used to supply memory

    StdAllocTestType<ALLOC> *d_self_p;      // pointer to self (to verify this
                                            // object is not bit-wise moved)

  public:
#ifndef BSLMA_USESBSLMAALLOCATOR_AUTODETECT_ALLOCATOR_TYPE
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(
                    StdAllocTestType,
                    bslma::UsesBslmaAllocator,
                    (bsl::is_convertible<bslma::Allocator *, ALLOC>::value));
#endif

    // PUBLIC TYPES
    typedef ALLOC allocator_type;           // needed by 'uses_allocator' trait

    // CREATORS
    explicit StdAllocTestType(const ALLOC& stdAllocator = ALLOC());
        // Create a 'StdAllocTestType' object having the (default) attribute
        // values:
        //..
        //  data() == 0
        //..
        // Optionally specify a 'stdAllocator' used to supply memory.  If
        // an allocator is not supplied, a default-constructed object of the
        // template parameter type 'ALLOC' is used.

    explicit StdAllocTestType(int value, const ALLOC& stdAllocator = ALLOC());
        // Create a 'StdAllocTestType' object having the specified 'data'
        // attribute value.  Optionally specify a 'stdAllocator' used to
        // supply memory.  If 'basicAllocator' is not supplied, a default
        // constructed object of (template parameter) type 'ALLOC' is used.

    StdAllocTestType(const StdAllocTestType& original);
    StdAllocTestType(const StdAllocTestType& original,
                     const ALLOC&            stdAllocator);
        // Create a 'StdAllocTestType' object having the same value as the
        // specified 'original' object.  Optionally specify a 'stdAllocator'
        // used to supply memory.  If 'basicAllocator' is not supplied, a
        // default constructed object of (template parameter) type 'ALLOC' is
        // used.

    ~StdAllocTestType();
        // Destroy this object.

    // MANIPULATORS
    StdAllocTestType& operator=(const StdAllocTestType& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void setData(int value);
        // Set the 'data' attribute of this object to the specified 'value'.

    // ACCESSORS
    int data() const;
        // Return the value of the 'data' attribute of this object.

                                  // Aspects

    const ALLOC& allocator() const;
        // Return the allocator used by this object to supply memory.
};

// FREE OPERATORS
template <class ALLOC>
bool operator==(const StdAllocTestType<ALLOC>& lhs,
                const StdAllocTestType<ALLOC>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'AllocTestType' objects have the same
    // if their 'data' attributes are the same.

template <class ALLOC>
bool operator!=(const StdAllocTestType<ALLOC>& lhs,
                const StdAllocTestType<ALLOC>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'AllocTestType' objects do not
    // have the same value if their 'data' attributes are not the same.

// ============================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ============================================================================

                        // ----------------------
                        // class StdAllocTestType
                        // ----------------------

// CREATORS
template <class ALLOC>
inline
StdAllocTestType<ALLOC>::StdAllocTestType(const ALLOC& stdAllocator)
: d_allocator(stdAllocator)
, d_self_p(this)
{
    d_data_p = bsl::allocator_traits<ALLOC>::allocate(d_allocator, 1);
    bsl::allocator_traits<ALLOC>::construct(d_allocator, d_data_p);
}

template <class ALLOC>
inline
StdAllocTestType<ALLOC>::StdAllocTestType(int value, const ALLOC& stdAllocator)
: d_allocator(stdAllocator)
, d_self_p(this)
{
    d_data_p = bsl::allocator_traits<ALLOC>::allocate(d_allocator, 1);
    bsl::allocator_traits<ALLOC>::construct(d_allocator, d_data_p, value);
}

template <class ALLOC>
inline
StdAllocTestType<ALLOC>::StdAllocTestType(const StdAllocTestType& original)
: d_allocator(
      bsl::allocator_traits<ALLOC>::select_on_container_copy_construction(
          original.d_allocator))
, d_self_p(this)
{
    d_data_p = bsl::allocator_traits<ALLOC>::allocate(d_allocator, 1);
    bsl::allocator_traits<ALLOC>::construct(d_allocator,
                                            d_data_p,
                                            *original.d_data_p);
}

template <class ALLOC>
inline
StdAllocTestType<ALLOC>::StdAllocTestType(const StdAllocTestType& original,
                                          const ALLOC&            stdAllocator)
: d_allocator(stdAllocator)
, d_self_p(this)
{
    d_data_p = bsl::allocator_traits<ALLOC>::allocate(d_allocator, 1);
    bsl::allocator_traits<ALLOC>::construct(d_allocator,
                                            d_data_p,
                                            *original.d_data_p);
}

template <class ALLOC>
inline
StdAllocTestType<ALLOC>::~StdAllocTestType()
{
    if (d_data_p) {
        bsl::allocator_traits<ALLOC>::destroy(d_allocator, d_data_p);
        bsl::allocator_traits<ALLOC>::deallocate(d_allocator, d_data_p, 1);
        d_data_p = 0;
    }

    BSLS_ASSERT_OPT(this == d_self_p);
}

template <class ALLOC>
inline
StdAllocTestType<ALLOC>&
StdAllocTestType<ALLOC>::operator=(const StdAllocTestType& rhs)
{
    if (this != &rhs) {
        if (bsl::allocator_traits<ALLOC>::
                                 propagate_on_container_copy_assignment::value)
        {
            StdAllocTestType other(rhs, rhs.allocator());
            ALLOC tmp = d_allocator;                // can't throw
            d_allocator = other.d_allocator;
            other.d_allocator = tmp;
            int *t = d_data_p;
            d_data_p = other.d_data_p;
            other.d_data_p = t;
        }
        else {
            StdAllocTestType other(rhs, d_allocator);
            int *t = d_data_p;
            d_data_p = other.d_data_p;
            other.d_data_p = t;
        }
    }
    return *this;
}

// MANIPULATORS
template <class ALLOC>
inline
void StdAllocTestType<ALLOC>::setData(int value)
{
    *d_data_p = value;
}

// ACCESSORS
template <class ALLOC>
inline
int StdAllocTestType<ALLOC>::data() const
{
    return *d_data_p;
}

                                  // Aspects
template <class ALLOC>
inline
const ALLOC& StdAllocTestType<ALLOC>::allocator() const
{
    return d_allocator;
}

}  // close package namespace

// FREE OPERATORS
template <class ALLOC>
inline
bool bsltf::operator==(const StdAllocTestType<ALLOC>& lhs,
                       const StdAllocTestType<ALLOC>& rhs)
{
    return lhs.data() == rhs.data();
}

template <class ALLOC>
inline
bool bsltf::operator!=(const StdAllocTestType<ALLOC>& lhs,
                       const StdAllocTestType<ALLOC>& rhs)
{
    return lhs.data() != rhs.data();
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
