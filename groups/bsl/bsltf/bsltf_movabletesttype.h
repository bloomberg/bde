// bsltf_movabletesttype.h                                       -*-C++-*-
#ifndef INCLUDED_BSLTF_MOVABLETESTTYPE
#define INCLUDED_BSLTF_MOVABLETESTTYPE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a non-allocating test class that records when moved from.
//
//@CLASSES:
//   bsltf::MovableTestType: non-allocating test class that records moves
//
//@SEE_ALSO: bsltf_templatetestfacility
//
//@DESCRIPTION: This component provides a single, unconstrained, non-allocating
// (value-semantic) attribute class, 'MovableTestType', that records when
// the move constructor or assignment operator is called with the instance as
// the source argument.  This class is not bitwise-moveable, and will assert on
// destruction if it has been copied (or moved) without calling a constructor.
// This class is primarily provided to facilitate testing of templates where
// move semantics need to be differentiated versus copy semantics.
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
// 'MovableTestType' as the parameterized 'TYPE':
//..
//  printTypeTraits<MovableTestType>();
//..
// Finally, we observe the console output:
//..
//  Type does not define bslma::UsesBslmaAllocator.
//  Type does not define bslmf::IsBitwiseMoveable.
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLTF_MOVESTATE
#include <bsltf_movestate.h>
#endif

#ifndef INCLUDED_BSLMF_MOVABLEREF
#include <bslmf_movableref.h>
#endif

#ifndef INCLUDED_BSLS_CPP11
#include <bsls_cpp11.h>
#endif

namespace BloombergLP {
namespace bsltf {

                        // =====================
                        // class MovableTestType
                        // =====================

class MovableTestType {
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
    int                d_data;         // data value

    MovableTestType   *d_self_p;       // pointer to self (to verify this
                                       // object is not bit-wise moved
    MoveState::Enum    d_movedFrom;    // moved-from state
    MoveState::Enum    d_movedInto;    // moved-into state


  public:
    // CREATORS
    MovableTestType();
        // Create a 'MovableTestType' object having the (default)
        // attribute values:
        //..
        //  data() == -1
        //..
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    explicit MovableTestType(int data);
        // Create a 'MovableTestType' object having the specified 'data'
        // attribute value.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    MovableTestType(const MovableTestType&  original);
        // Create a 'MovableTestType' object having the same value as the
        // specified 'original' object.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    MovableTestType(bslmf::MovableRef<MovableTestType>
                                                 original) BSLS_CPP11_NOEXCEPT;
        // TBD: comment this

    ~MovableTestType();
        // Destroy this object.

    // MANIPULATORS
    MovableTestType& operator=(const MovableTestType& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    MovableTestType& operator=(bslmf::MovableRef<MovableTestType> rhs);
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
};

// FREE OPERATORS
bool operator==(const MovableTestType& lhs,
                const MovableTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'MovableTestType' objects have
    // the same if their 'data' attributes are the same.
    // TBD: think about the behavior when specified on an object that was
    // moved-from on this as well as other functions/methods if appropriate.

bool operator!=(const MovableTestType& lhs,
                const MovableTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'MovableTestType' objects
    // do not have the same value if their 'data' attributes are not the same.

// FREE FUNCTIONS
MoveState::Enum getMovedFrom(const MovableTestType& object);
    // Return the move-from state of the specified 'object'.

MoveState::Enum getMovedInto(const MovableTestType& object);
    // Return the move-into state of the specified 'object'.

void setMovedInto(MovableTestType *object, MoveState::Enum value);
    // Set the moved-into state of the specified 'object' to the specified
    // 'value'.

// ============================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ============================================================================

                        // ---------------------
                        // class MovableTestType
                        // ---------------------

// MANIPULATORS
inline
void MovableTestType::setMovedInto(MoveState::Enum value)
{
    d_movedInto = value;
}

// ACCESSORS
inline
int MovableTestType::data() const
{
    return d_data;
}

inline
MoveState::Enum MovableTestType::movedFrom() const
{
    return d_movedFrom;
}

inline
MoveState::Enum MovableTestType::movedInto() const
{
    return d_movedInto;
}

// FREE FUNCTIONS
inline
MoveState::Enum getMovedFrom(const MovableTestType& object)
{
    return object.movedFrom();
}

inline
MoveState::Enum getMovedInto(const MovableTestType& object)
{
    return object.movedInto();
}

inline
void setMovedInto(MovableTestType *object, MoveState::Enum value)
{
    object->setMovedInto(value);
}

}  // close package namespace

// FREE OPERATORS
inline
bool bsltf::operator==(const MovableTestType& lhs,
                       const MovableTestType& rhs)
{
    return lhs.data() == rhs.data();
}

inline
bool bsltf::operator!=(const MovableTestType& lhs,
                       const MovableTestType& rhs)
{
    return lhs.data() != rhs.data();
}

}  // close enterprise namespace

// TRAITS
namespace bsl {
template <>
struct is_nothrow_move_constructible<BloombergLP::bsltf::MovableTestType>
        : bsl::true_type {};
}  // close namespace bsl

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
