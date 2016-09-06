// bsltf_argumenttype.h                                               -*-C++-*-
#ifndef INCLUDED_BSLTF_ARGUMENTTYPE
#define INCLUDED_BSLTF_ARGUMENTTYPE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a non-allocating test class to represent a func argument.
//
//@REVIEW_FOR_MASTER:
//
//@CLASSES:
//   bsltf::ArgumentType<N>: simple wrapper around an in-place 'int'
//
//@SEE_ALSO: bsltf_templatetestfacility
//
//@AUTHOR:
//
//@DESCRIPTION: This component provides a representation of a non-allocating
// argument type template class, 'bsltf::ArgumentType<N>', used for testing
// functions that take a variable number of template arguments.  The integer
// template parameter enables specification of a number of types without
// requiring a separate component for each.  Copy and move constructors are
// defined.
//
///Attributes
///----------
//..
//  Name                Type         Default
//  ------------------  -----------  -------
//  data                int          -1
//..
//: o 'data': representation of the class value
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: TBD
/// - - - - - - -
// Suppose we wanted to ...

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLTF_MOVESTATE
#include <bsltf_movestate.h>
#endif

#ifndef INCLUDED_BSLMF_MOVABLEREF
#include <bslmf_movableref.h>
#endif

namespace BloombergLP {
namespace bsltf {

                         // =======================
                         // class ArgumentType<int>
                         // =======================

template <int N>
class ArgumentType {
    // This class template declares a separate type for each template parameter
    // value 'N', 'bsltf::ArgumentType<N>', that wraps an integer value and
    // provides implicit conversion to and from 'int'.  Its main purpose is
    // that having separate types for testing enables distinguishing them when
    // calling through a function template interface, thereby avoiding
    // ambiguities or accidental switching of arguments in the implementation
    // of in-place constructors.

    // PRIVATE TYPES
    typedef bslmf::MovableRefUtil MoveUtil;

    // DATA
    int              d_data;       // attribute value
    MoveState::Enum  d_movedFrom;  // moved-from state
    MoveState::Enum  d_movedInto;  // moved-into state

  public:
    // CREATORS
    ArgumentType();
        // Create an 'ArgumentType' object having the default attribute value
        // '-1'.

    explicit ArgumentType(int value);
        // Create an 'ArgumentType' object having the specified 'value'. The
        // behavior is undefined unless 'value >= 0'.

    ArgumentType(const ArgumentType& original);
        // Create an 'ArgumentType' object having the same value as the
        // specified 'original'.

    ArgumentType(BloombergLP::bslmf::MovableRef<ArgumentType> original);
        // Create an 'ArgumentType' object having the same value as the
        // specified 'original'.  Note that 'original' is left in a valid but
        // unspecified state.

    //! ~ArgumentType() = default;
        // Destroy this object.

    // MANIPULATORS
    ArgumentType& operator=(const ArgumentType& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    ArgumentType& operator=(BloombergLP::bslmf::MovableRef<ArgumentType> rhs);
        // Assign to this object the value of the specified 'rhs' object.  Note
        // that 'rhs' is left in a valid but unspecified state.

    // ACCESSORS
    operator int() const;
        // Return the value of this object.

    MoveState::Enum movedFrom() const;
        // Return the move state of this object as source of a move operation.

    MoveState::Enum movedInto() const;
        // Return the move state of this object as target of a move operation.

};

// FREE FUNCTIONS
template <int N>
MoveState::Enum getMovedFrom(const ArgumentType<N>& object);
    // Return the move-from state of the specified 'object'.

template <int N>
MoveState::Enum getMovedInto(const ArgumentType<N>& object);
    // Return the move-into state of the specified 'object'.

// ============================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ============================================================================

                        // ---------------------
                        // class ArgumentType<N>
                        // ---------------------

// CREATORS
template <int N>
inline
ArgumentType<N>::ArgumentType()
: d_data(-1)
, d_movedFrom(MoveState::e_NOT_MOVED)
, d_movedInto(MoveState::e_NOT_MOVED)
{
}

template <int N>
inline
ArgumentType<N>::ArgumentType(int value)
: d_movedFrom(MoveState::e_NOT_MOVED)
, d_movedInto(MoveState::e_NOT_MOVED)
{
    BSLS_ASSERT_SAFE(value >= 0);
    d_data = value;
}

template <int N>
inline
ArgumentType<N>::ArgumentType(const ArgumentType& original)
: d_data(original.d_data)
, d_movedFrom(MoveState::e_NOT_MOVED)
, d_movedInto(MoveState::e_NOT_MOVED)
{
}

template <int N>
inline
ArgumentType<N>::ArgumentType(
                  BloombergLP::bslmf::MovableRef<ArgumentType> original)
: d_movedFrom(MoveState::e_NOT_MOVED)
, d_movedInto(MoveState::e_MOVED)
{
    ArgumentType& lvalue = original;

    d_data = lvalue.d_data;

    lvalue.d_data = -1;
    lvalue.d_movedFrom = MoveState::e_MOVED;
    lvalue.d_movedInto = MoveState::e_NOT_MOVED;
}

// MANIPULATORS
template <int N>
inline
ArgumentType<N>& ArgumentType<N>::operator=(const ArgumentType& rhs)
{
    if (this != &rhs) {
        d_data = rhs.d_data;
        d_movedFrom = MoveState::e_NOT_MOVED;
        d_movedInto = MoveState::e_NOT_MOVED;
    }
    return *this;
}

template <int N>
inline
ArgumentType<N>&
ArgumentType<N>::operator=(BloombergLP::bslmf::MovableRef<ArgumentType> rhs)
{
    ArgumentType& lvalue = rhs;

    if (this != &lvalue) {
        d_data = lvalue.d_data;
        d_movedFrom = MoveState::e_NOT_MOVED;
        d_movedInto = MoveState::e_MOVED;

        lvalue.d_data = -1;
        lvalue.d_movedFrom = MoveState::e_MOVED;
        lvalue.d_movedInto = MoveState::e_NOT_MOVED;
    }
    return *this;
}

// ACCESSORS
template <int N>
inline
ArgumentType<N>::operator int() const
{
    return d_data;
}

template <int N>
inline
MoveState::Enum ArgumentType<N>::movedFrom() const
{
    return d_movedFrom;
}

template <int N>
inline
MoveState::Enum ArgumentType<N>::movedInto() const
{
    return d_movedInto;
}

// FREE FUNCTIONS
template <int N>
inline
MoveState::Enum getMovedFrom(const ArgumentType<N>& object)
{
    return object.movedFrom();
}

template <int N>
inline
MoveState::Enum getMovedInto(const ArgumentType<N>& object)
{
    return object.movedInto();
}

}  // close package namespace
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
