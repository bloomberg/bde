// bsltf_allocargumenttype.h                                          -*-C++-*-
#ifndef INCLUDED_BSLTF_ALLOCARGUMENTTYPE
#define INCLUDED_BSLTF_ALLOCARGUMENTTYPE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an allocating test class to represent function arguments.
//
//@REVIEW_FOR_MASTER:
//
//@CLASSES:
//   bsltf::AllocArgumentType<N>: simple wrapper around an allocated 'int'
//
//@SEE_ALSO: bsltf_templatetestfacility
//
//@AUTHOR:
//
//@DESCRIPTION: This component provides a representation of an allocating
//argument type template class, 'bsltf::AllocArgumentType<N>', used for testing
//functions that take a variable number of template arguments.  The integer
//template parameter enables specification of a number of types without
//requiring a separate component for each.  Note that there are no
//manipulators, not even an assignment operator, for the class and the value
//constructor is the only way to assign an object a value.  Copy and move
//constructors are defined.
//
///Attributes
///----------
//..
//  Name                Type         Default
//  ------------------  -----------  -------
//  d_data_p            int          0
//
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

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_MOVABLEREF
#include <bslmf_movableref.h>
#endif

namespace BloombergLP {
namespace bsltf {

                         // ============================
                         // class AllocArgumentType<int>
                         // ============================

template <int N>
class AllocArgumentType {
    // This class template declares a separate type for each template parameter
    // value 'N', 'bsltf::AllocArgumentType<N>, that wraps an integer value and
    // provides implicit conversion to and from 'int'.  The wrapped integer
    // will be dynamically allocated using the supplied allocator, or the
    // default allocator if none is supplied.  Its main purpose is that having
    // separate types for testing enables distinguishing them when calling
    // through a function template interface, thereby avoiding ambiguities or
    // accidental switching of arguments in the implementation of in-place
    // constructors.  It further tests that allocators are propagated
    // correctly, or not, as required.

    // PRIVATE TYPES
    typedef bslmf::MovableRefUtil MoveUtil;

    // DATA
    bslma::Allocator *d_allocator_p;        // allocator used to supply memory
                                            // (held, not owned)

    int              *d_data_p;             // pointer to the data value

    int               d_originalValue;      // original data value supplied at
                                            // construction

    MoveState::Enum   d_movedFrom;          // moved-from state

    MoveState::Enum   d_movedInto;          // moved-from state

  public:
    // CREATORS
    explicit AllocArgumentType(bslma::Allocator *basicAllocator =  0);
        // Create a 'AllocArgumentType' object having the (default) attribute
        // value '-1'.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    explicit AllocArgumentType(int               value,
                               bslma::Allocator *basicAllocator =  0);

    AllocArgumentType(const AllocArgumentType&  other,
                      bslma::Allocator         *basicAllocator = 0);
        // Create a test argument object having the same value as the specified
        // 'other'.  The optionally specified 'basicAllocator' is used to
        // supply memory, and the currently installed default allocator is used
        // if no such allocator is supplied.

    AllocArgumentType(bslmf::MovableRef<AllocArgumentType> original);
        // Create a test argument object having the same value as the specified
        // 'original', and reset 'original' to have that value negated.

    AllocArgumentType(bslmf::MovableRef<AllocArgumentType>  original,
                      bslma::Allocator                     *basicAllocator);
        // Create a test argument object having the same value as the specified
        // 'original', and reset 'original' to have that value negated.

    ~AllocArgumentType();
        // Destroy this object.

    // MANIPULATORS
    // TBD: we need this because assignability is an important requirement in
    //      the implementation of containers
    AllocArgumentType& operator=(const AllocArgumentType& rhs);
        // Assign to this test argument the value of the specified 'rhs'
        // object.

    AllocArgumentType&
    operator=(BloombergLP::bslmf::MovableRef<AllocArgumentType> rhs);
        // Assign to this test argument the value of the specified 'rhs'
        // object.  Note that 'rhs' is left in a valid but unspecified state.

    // ACCESSORS
    operator int() const;
        // Return the value of this test argument object.

    bslma::Allocator *getAllocator() const;
        // Return the allocator used by this object to supply memory.

    MoveState::Enum movedInto() const;
        // Return the move state of this object as target of a move operation.

    MoveState::Enum movedFrom() const;
        // Return the move state of this object as source of a move operation.

};

// FREE FUNCTIONS
template <int N>
MoveState::Enum getMovedFromState(const AllocArgumentType<N>& object);

template <int N>
MoveState::Enum getMovedIntoState(const AllocArgumentType<N>& object);

// ============================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ============================================================================

                        // --------------------------
                        // class AllocArgumentType<N>
                        // --------------------------

// CREATORS
template <int N>
inline
AllocArgumentType<N>::AllocArgumentType(bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_data_p(0)
, d_originalValue(-1)
, d_movedFrom(bsltf::MoveState::e_NOT_MOVED)
, d_movedInto(bsltf::MoveState::e_NOT_MOVED)
{
    // Note that the default constructor does not allocate.  This is done to
    // correctly count allocation when not the whole set of arguments is passed
    // to emplacable test types.
}

template <int N>
inline
AllocArgumentType<N>::AllocArgumentType(int               value,
                                        bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_data_p(0)
, d_originalValue(value)
, d_movedFrom(bsltf::MoveState::e_NOT_MOVED)
, d_movedInto(bsltf::MoveState::e_NOT_MOVED)
{
    d_data_p = reinterpret_cast<int *>(d_allocator_p->allocate(sizeof(int)));
    *d_data_p = d_originalValue;
}

template <int N>
inline
AllocArgumentType<N>::AllocArgumentType(
                                      const AllocArgumentType&  other,
                                      bslma::Allocator         *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_data_p(0)
, d_originalValue(int(other))
, d_movedFrom(bsltf::MoveState::e_NOT_MOVED)
, d_movedInto(bsltf::MoveState::e_NOT_MOVED)
{
    d_data_p = reinterpret_cast<int *>(d_allocator_p->allocate(sizeof(int)));
    *d_data_p = d_originalValue;
}

template <int N>
inline
AllocArgumentType<N>::AllocArgumentType(
                    BloombergLP::bslmf::MovableRef<AllocArgumentType> original)
: d_allocator_p(MoveUtil::access(original).d_allocator_p)
, d_movedFrom(bsltf::MoveState::e_NOT_MOVED)
, d_movedInto(bsltf::MoveState::e_MOVED)
{
    AllocArgumentType& lvalue = original;

    d_originalValue = int(lvalue);

    d_data_p = lvalue.d_data_p;
    lvalue.d_data_p = 0;

    lvalue.d_movedFrom = bsltf::MoveState::e_MOVED;
}

template <int N>
inline
AllocArgumentType<N>::AllocArgumentType(
              BloombergLP::bslmf::MovableRef<AllocArgumentType> original,
              bslma::Allocator                                 *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_movedFrom(bsltf::MoveState::e_NOT_MOVED)
, d_movedInto(bsltf::MoveState::e_MOVED)
{
    AllocArgumentType& lvalue = original;

    d_originalValue = int(lvalue);

    if (d_allocator_p == lvalue.d_allocator_p) {
        d_data_p = lvalue.d_data_p;
        lvalue.d_data_p = 0;
    }
    else {
        d_data_p = reinterpret_cast<int *>(
                                         d_allocator_p->allocate(sizeof(int)));
        *d_data_p = d_originalValue;
    }

    lvalue.d_movedFrom = bsltf::MoveState::e_MOVED;
}

template <int N>
inline
AllocArgumentType<N>::~AllocArgumentType()
{
    d_allocator_p->deallocate(d_data_p);
}

// MANIPULATORS
template <int N>
AllocArgumentType<N>&
AllocArgumentType<N>::operator=(const AllocArgumentType& rhs)
{
    if (this != &rhs) {
        int *newValue = reinterpret_cast<int *>(
                                         d_allocator_p->allocate(sizeof(int)));
        d_allocator_p->deallocate(d_data_p);
        d_data_p = newValue;
        d_originalValue  = int(rhs);
        *d_data_p = d_originalValue;

        d_movedInto = bsltf::MoveState::e_NOT_MOVED;
        d_movedFrom = bsltf::MoveState::e_NOT_MOVED;
    }
    return *this;
}

template <int N>
AllocArgumentType<N>& AllocArgumentType<N>::operator=(
                         BloombergLP::bslmf::MovableRef<AllocArgumentType> rhs)
{
    AllocArgumentType& lvalue = rhs;

    if (this != &lvalue) {
        if (d_allocator_p == lvalue.d_allocator_p) {
            d_allocator_p->deallocate(d_data_p);
            d_originalValue = int(lvalue);
            d_data_p = lvalue.d_data_p;
            lvalue.d_data_p = 0;
        }
        else {
            int *newValue = reinterpret_cast<int *>(
                                         d_allocator_p->allocate(sizeof(int)));
            d_allocator_p->deallocate(d_data_p);
            d_data_p = newValue;
            d_originalValue = int(lvalue);
            *d_data_p = d_originalValue;
        }

        d_movedInto        = bsltf::MoveState::e_MOVED;
        lvalue.d_movedFrom = bsltf::MoveState::e_MOVED;
    }
    return *this;
}

// ACCESSORS
template <int N>
inline
AllocArgumentType<N>::operator int() const
{
    return d_data_p ? *d_data_p : d_originalValue;
}

template <int N>
inline
bslma::Allocator * AllocArgumentType<N>::getAllocator() const
{
    return d_allocator_p;
}

template <int N>
inline
MoveState::Enum AllocArgumentType<N>::movedFrom() const
{
    return d_movedFrom;
}

template <int N>
inline
MoveState::Enum AllocArgumentType<N>::movedInto() const
{
    return d_movedInto;
}

                                  // Aspects

template <int N>
inline
MoveState::Enum getMovedFrom(const AllocArgumentType<N>& object)
{
    return object.movedFrom();
}

template <int N>
inline
MoveState::Enum getMovedInto(const AllocArgumentType<N>& object)
{
    return object.movedInto();
}

}  // close package namespace

// TRAITS
namespace bslma {

template <int N>
struct UsesBslmaAllocator<bsltf::AllocArgumentType<N> > : bsl::true_type {};

}  // close namespace bslma
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
