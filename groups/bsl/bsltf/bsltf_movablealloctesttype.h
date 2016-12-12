// bsltf_movablealloctesttype.h                                       -*-C++-*-
#ifndef INCLUDED_BSLTF_MOVABLEALLOCTESTTYPE
#define INCLUDED_BSLTF_MOVABLEALLOCTESTTYPE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an allocating test class that records when moved from.
//
//@CLASSES:
//   bsltf::MovableAllocTestType: allocating test class that records moves
//
//@SEE_ALSO: bsltf_templatetestfacility
//
//@DESCRIPTION: This component provides a single, unconstrained
// (value-semantic) attribute class, 'MovableAllocTestType', that records when
// the move constructor or assignment operator is called with the instance as
// the source argument.  Furthermore, this class uses 'bslma::Allocator' to
// allocate memory, and defines the type trait 'bslma::UsesBslmaAllocator'.
// This class is not bitwise-moveable, and will assert on destruction if it has
// been copied (or moved) without calling a constructor.  This class is
// primarily provided to facilitate testing of templates where move semantics
// need to be differentiated versus copy semantics.
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
///Example 1: Printing the Supported Traits
/// - - - - - - - - - - - - - - - - - - - -
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
//          printf(
//              "Type does not define bslma::UsesBslmaAllocator.\n");
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
// Now, we invoke the 'printTypeTraits' function template using
// 'MovableAllocTestType' as the parameterized 'TYPE':
//..
//  printTypeTraits<MovableAllocTestType>();
//..
// Finally, we observe the console output:
//..
//  Type defines bslma::UsesBslmaAllocator.
//  Type does not define bslmf::IsBitwiseMoveable.
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLTF_MOVESTATE
#include <bsltf_movestate.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_MOVABLEREF
#include <bslmf_movableref.h>
#endif

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace bsltf {

                        // ==========================
                        // class MovableAllocTestType
                        // ==========================

class MovableAllocTestType {
    // This class provides an unconstrained (value-semantic) attribute type
    // that records when move semantics have been invoked with the object
    // instance as the source parameter.  The class uses a 'bslma::Allocator'
    // to allocate memory and defines the type trait
    // 'bslma::UsesBslmaAllocator'.  This class is primarily provided
    // to facilitate testing of templates where move semantics need to be
    // differentiated versus copy semantics.  See the 'Attributes' section
    // under @DESCRIPTION in the component-level documentation for information
    // on the class attributes.

    // DATA
    int                    *d_data_p;       // pointer to the data value
    bslma::Allocator       *d_allocator_p;  // allocator used to supply memory
                                            // (held, not owned)

    MovableAllocTestType   *d_self_p;       // pointer to self (to verify this
                                            // object is not bit-wise moved
    MoveState::Enum         d_movedFrom;    // moved-from state
    MoveState::Enum         d_movedInto;    // moved-from state


  public:
    // CREATORS
    explicit MovableAllocTestType(bslma::Allocator *basicAllocator = 0);
        // Create a 'MovableAllocTestType' object having the (default)
        // attribute values:
        //..
        //  data() == -1
        //..
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    explicit MovableAllocTestType(int data,
                                  bslma::Allocator *basicAllocator = 0);
        // Create a 'MovableAllocTestType' object having the specified 'data'
        // attribute value.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    MovableAllocTestType(bslmf::MovableRef<MovableAllocTestType>  original);
    MovableAllocTestType(bslmf::MovableRef<MovableAllocTestType>  original,
                         bslma::Allocator *basicAllocator);
        // TBD: comment this

    MovableAllocTestType(const MovableAllocTestType&  original,
                         bslma::Allocator      *basicAllocator = 0);
        // Create a 'MovableAllocTestType' object having the same value as the
        // specified 'original' object.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    ~MovableAllocTestType();
        // Destroy this object.

    // MANIPULATORS
    MovableAllocTestType& operator=(const MovableAllocTestType& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    MovableAllocTestType& operator=(
                                  bslmf::MovableRef<MovableAllocTestType> rhs);
        // TBD: comment this

    void setData(int value);
        // Set the 'data' attribute of this object to the specified 'value'.

    void setMovedInto(MoveState::Enum value);
        // Set the moved-into state of this object to the specified 'value'.

    // ACCESSORS
    int data() const;
        // Return the value of the 'data' attribute of this object.

    MoveState::Enum movedInto() const;
        // Return the move state of this object as target of a move operation.

    MoveState::Enum movedFrom() const;
        // Return the move state of this object as source of a move operation.

                                  // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.
};

// FREE OPERATORS
bool operator==(const MovableAllocTestType& lhs,
                const MovableAllocTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'MovableAllocTestType' objects have
    // the same if their 'data' attributes are the same.
    // TBD: think about the behavior when specified on an object that was
    // moved-from on this as well as other functions/methods if appropriate.

bool operator!=(const MovableAllocTestType& lhs,
                const MovableAllocTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'MovableAllocTestType' objects
    // do not have the same value if their 'data' attributes are not the same.

// FREE FUNCTIONS
MoveState::Enum getMovedFrom(const MovableAllocTestType& object);
    // Return the move-from state of the specified 'object'.

MoveState::Enum getMovedInto(const MovableAllocTestType& object);
    // Return the move-into state of the specified 'object'.

void setMovedInto(MovableAllocTestType *object, MoveState::Enum value);
    // Set the moved-into state of the specified 'object' to the specified
    // 'value'.

// ============================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ============================================================================

                        // --------------------------
                        // class MovableAllocTestType
                        // --------------------------

// MANIPULATORS
inline
void MovableAllocTestType::setMovedInto(MoveState::Enum value)
{
    d_movedInto = value;
}

// ACCESSORS
inline
int MovableAllocTestType::data() const
{
    return d_data_p ? *d_data_p : 0;
}

inline
MoveState::Enum MovableAllocTestType::movedFrom() const
{
    return d_movedFrom;
}

inline
MoveState::Enum MovableAllocTestType::movedInto() const
{
    return d_movedInto;
}

                                  // Aspects

inline
bslma::Allocator *MovableAllocTestType::allocator() const
{
    return d_allocator_p;
}

// FREE FUNCTION
inline
MoveState::Enum getMovedFrom(const MovableAllocTestType& object)
{
    return object.movedFrom();
}

inline
MoveState::Enum getMovedInto(const MovableAllocTestType& object)
{
    return object.movedInto();
}

inline
void setMovedInto(MovableAllocTestType *object, MoveState::Enum value)
{
    object->setMovedInto(value);
}

}  // close package namespace

// FREE OPERATORS
inline
bool bsltf::operator==(const MovableAllocTestType& lhs,
                       const MovableAllocTestType& rhs)
{
    return lhs.data() == rhs.data();
}

inline
bool bsltf::operator!=(const MovableAllocTestType& lhs,
                       const MovableAllocTestType& rhs)
{
    return lhs.data() != rhs.data();
}

// TRAITS
namespace bslma {
template <>
struct UsesBslmaAllocator<bsltf::MovableAllocTestType>
    : bsl::true_type {};
}  // close namespace bslma

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
