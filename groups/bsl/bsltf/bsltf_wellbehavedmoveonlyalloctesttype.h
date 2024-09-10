// bsltf_wellbehavedmoveonlyalloctesttype.h                           -*-C++-*-
#ifndef INCLUDED_BSLTF_WELLBEHAVEDMOVEONLYALLOCTESTTYPE
#define INCLUDED_BSLTF_WELLBEHAVEDMOVEONLYALLOCTESTTYPE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide well-behaved move-only type using `bslma` allocators.
//
//@CLASSES:
//   bsltf::WellBehavedMoveOnlyAllocTestType: move-only, allocating test class
//
//@SEE_ALSO: bsltf_templatetestfacility
//
//@DESCRIPTION: This component provides a single, unconstrained
// (value-semantic) attribute class, `WellBehavedMoveOnlyAllocTestType`, that
// uses a `bslma::Allocator` to supply memory (defines the type trait
// `bslma::UsesBslmaAllocator`) and provides only a move constructor and move
// assignment operator (disables copy constructor and copy assignment
// operator).  Furthermore, this class is not bitwise-moveable, and will assert
// on destruction if it has been bitwise-moved.  In addition, in the case of a
// move where source and destination objects use different allocators, the move
// acts like a copy and does not modify the source, or mark it as `moved-from`,
// and does not mark the destination as `moved-into`.  This class is primarily
// provided to facilitate testing of templates by defining a simple type
// representative of user-defined types having an allocator that cannot be
// copied (only moved).
//
///Attributes
///----------
// ```
// Name                Type         Default
// ------------------  -----------  -------
// data                int          0
// ```
// * `data`: representation of the object's value
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Type Traits and Move Construction
/// - - - - - - - - - - - - - - - - - - - - - -
// First, we observe that the type uses `bslma::Allocator`s:
// ```
// assert(true == bslma::UsesBslmaAllocator<
//                           bsltf::WellBehavedMoveOnlyAllocTestType>::value);
// ```
// Then, we observe that the type is not copy-constructible:
// ```
// assert(false == bsl::is_copy_constructible<
//                           bsltf::WellBehavedMoveOnlyAllocTestType>::value);
// ```
// Next, we observe that the type is not bitwise movable:
// ```
// assert(false == bslmf::IsBitwiseMoveable<
//                           bsltf::WellBehavedMoveOnlyAllocTestType>::value);
// ```
// Then, we create an instance of our type with the value `5`:
// ```
// bsltf::WellBehavedMoveOnlyAllocTestType a(5);
// ```
// Next, we move-construct another instance from the first:
// ```
// bsltf::WellBehavedMoveOnlyAllocTestType b(bslmf::MovableRefUtil::move(a));
// ```
// Now, we observe the salient state of both objects:
// ```
// assert(0 == a.data());
// assert(5 == b.data());
// ```
// And finally, the non-salient state:
// ```
// assert(bsltf::MoveState::e_MOVED == a.movedFrom());
// assert(bsltf::MoveState::e_MOVED == b.movedInto());
//
// assert(bsltf::MoveState::e_NOT_MOVED == a.movedInto());
// assert(bsltf::MoveState::e_NOT_MOVED == b.movedFrom());
// ```

#include <bslscm_version.h>

#include <bsltf_movestate.h>

#include <bslma_usesbslmaallocator.h>
#include <bslmf_iscopyconstructible.h>
#include <bslmf_isnothrowmoveconstructible.h>
#include <bslmf_movableref.h>
#include <bsls_keyword.h>

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace bsltf {

                    // ======================================
                    // class WellBehavedMoveOnlyAllocTestType
                    // ======================================

/// This unconstrained (value-semantic) attribute class that uses a
/// `bslma::Allocator` to supply memory and defines the type trait
/// `bslma::UsesBslmaAllocator`.  This class is primarily provided to
/// facilitate testing of templates by defining a simple type representative of
/// user-defined types having an allocator.  See the [](#Attributes) section
/// for information on the class attributes.
class WellBehavedMoveOnlyAllocTestType {

    // DATA
    int                     *d_data_p;       // pointer to the data value

    bslma::Allocator        *d_allocator_p;  // allocator used to supply memory
                                             // (held, not owned)

    void                    *d_self_p;       // pointer to self (to verify not
                                             // bit-wise moved)

    MoveState::Enum          d_movedFrom;    // moved-from state

    MoveState::Enum          d_movedInto;    // moved-to state

  private:
    // NOT IMPLEMENTED
    WellBehavedMoveOnlyAllocTestType& operator=(
                                      const WellBehavedMoveOnlyAllocTestType&);
    WellBehavedMoveOnlyAllocTestType(const WellBehavedMoveOnlyAllocTestType&);

  public:
    // CREATORS

    /// Create a `WellBehavedMoveOnlyAllocTestType` object having the (default)
    /// attribute values:
    /// ```
    /// data() == 0
    /// ```
    /// Optionally specify a `basicAllocator` used to supply memory.  If
    /// `basicAllocator` is 0, the currently installed default allocator is
    /// used.
    explicit WellBehavedMoveOnlyAllocTestType(
                                         bslma::Allocator *basicAllocator = 0);

    /// Create a `WellBehavedMoveOnlyAllocTestType` object having the specified
    /// `data` attribute value.  Optionally specify a `basicAllocator` used to
    /// supply memory.  If `basicAllocator` is 0, the currently installed
    /// default allocator is used.
    explicit WellBehavedMoveOnlyAllocTestType(
                                         int               data,
                                         bslma::Allocator *basicAllocator = 0);

    WellBehavedMoveOnlyAllocTestType(
                  bslmf::MovableRef<WellBehavedMoveOnlyAllocTestType> original)
                                                         BSLS_KEYWORD_NOEXCEPT;

    /// Create a `WellBehavedMoveAllocTestType` object having the same value as
    /// the specified `original` object.  Optionally specify a `basicAllocator`
    /// used to supply memory.  If `basicAllocator` is 0, the currently
    /// installed default allocator is used.  After construction, this object
    /// will be in a `movedInto` state, and `original` will be in a `movedFrom`
    /// state.  No allocations shall occur (so no exception will be thrown)
    /// unless 'basicAllocator != original.allocator()).
    WellBehavedMoveOnlyAllocTestType(
          bslmf::MovableRef<WellBehavedMoveOnlyAllocTestType>  original,
          bslma::Allocator                                    *basicAllocator);

    /// Destroy this object.
    ~WellBehavedMoveOnlyAllocTestType();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a reference providing modifiable access to this object.  If
    /// `rhs` is a reference to this object, there are no other effects;
    /// otherwise, the object referenced by `rhs` will be reset to a default
    /// constructed state, `rhs` shall be in a `movedFrom` state, and this
    /// object will be in a `movedTo` state.  No allocations shall occur (so no
    /// exception will be thrown) unless this object and `rhs` have different
    /// allocators.  Note that the moved-from state is specified, rather than
    /// "valid but unspecified", as this type is intended for verifying test
    /// drivers that want to ensure that moves occur correctly where expected.
    WellBehavedMoveOnlyAllocTestType& operator=(
                      bslmf::MovableRef<WellBehavedMoveOnlyAllocTestType> rhs);

    /// Set the `data` attribute of this object to the specified `value`.
    void setData(int value);

    /// Set the moved-into state of this object to the specified `value`.
    void setMovedInto(MoveState::Enum value);

    // ACCESSORS

    /// Return the value of the `data` attribute of this object.
    int data() const;

                                  // Aspects

    /// Return the allocator used by this object to supply memory.
    bslma::Allocator *allocator() const;

    /// Return the move state of this object as target of a move operation.
    MoveState::Enum movedInto() const;

    /// Return the move state of this object as source of a move operation.
    MoveState::Enum movedFrom() const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` objects have the same value,
/// and `false` otherwise.  Two `WellBehavedMoveOnlyAllocTestType` objects have
/// the same if their `data` attributes are the same.
bool operator==(const WellBehavedMoveOnlyAllocTestType& lhs,
                const WellBehavedMoveOnlyAllocTestType& rhs);

/// Return `true` if the specified `lhs` and `rhs` objects do not have the same
/// value, and `false` otherwise.  Two `WellBehavedMoveOnlyAllocTestType`
/// objects do not have the same value if their `data` attributes are not the
/// same.
bool operator!=(const WellBehavedMoveOnlyAllocTestType& lhs,
                const WellBehavedMoveOnlyAllocTestType& rhs);

// FREE FUNCTIONS

/// Return the move-from state of the specified `object`.
MoveState::Enum getMovedFrom(const WellBehavedMoveOnlyAllocTestType& object);

/// Return the move-into state of the specified `object`.
MoveState::Enum getMovedInto(const WellBehavedMoveOnlyAllocTestType& object);

/// Set the moved-into state of the specified `object` to the specified
/// `value`.
void setMovedInto(WellBehavedMoveOnlyAllocTestType *object,
                                                        MoveState::Enum value);

/// Exchange the states of the specified `a` and `b`.  If the allocators match,
/// both `a` and `b` will be left in a moved-into state, otherwise, both will
/// not.  If `a` and `b` are the same object, this function will have no
/// effect.  No allocations shall occur (so no exceptions will be thrown)
/// unless `a` and `b` have different allocators.
void swap(WellBehavedMoveOnlyAllocTestType& a,
          WellBehavedMoveOnlyAllocTestType& b);

// ============================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ============================================================================

                    // --------------------------------------
                    // class WellBehavedMoveOnlyAllocTestType
                    // --------------------------------------

// ACCESSORS
inline
int WellBehavedMoveOnlyAllocTestType::data() const
{
    return d_data_p ? *d_data_p : 0;
}

// MANIPULATORS
inline
void WellBehavedMoveOnlyAllocTestType::setMovedInto(MoveState::Enum value)
{
    d_movedInto = value;
}

                                  // Aspects

inline
bslma::Allocator *WellBehavedMoveOnlyAllocTestType::allocator() const
{
    return d_allocator_p;
}

inline
MoveState::Enum WellBehavedMoveOnlyAllocTestType::movedFrom() const
{
    return d_movedFrom;
}

inline
MoveState::Enum WellBehavedMoveOnlyAllocTestType::movedInto() const
{
    return d_movedInto;
}


// FREE FUNCTIONS
inline
MoveState::Enum getMovedFrom(const WellBehavedMoveOnlyAllocTestType& object)
{
    return object.movedFrom();
}

inline
MoveState::Enum getMovedInto(const WellBehavedMoveOnlyAllocTestType& object)
{
    return object.movedInto();
}

inline
void setMovedInto(WellBehavedMoveOnlyAllocTestType *object,
                  MoveState::Enum                   value)
{
    object->setMovedInto(value);
}

}  // close package namespace

// FREE OPERATORS
inline
bool bsltf::operator==(const WellBehavedMoveOnlyAllocTestType& lhs,
                       const WellBehavedMoveOnlyAllocTestType& rhs)
{
    return lhs.data() == rhs.data();
}

inline
bool bsltf::operator!=(const WellBehavedMoveOnlyAllocTestType& lhs,
                       const WellBehavedMoveOnlyAllocTestType& rhs)
{
    return lhs.data() != rhs.data();
}

// TRAITS
namespace bslma {
template <>
struct UsesBslmaAllocator<bsltf::WellBehavedMoveOnlyAllocTestType>
                                                           : bsl::true_type {};
}  // close namespace bslma

}  // close enterprise namespace

namespace bsl {
template <>
struct is_copy_constructible<
                          BloombergLP::bsltf::WellBehavedMoveOnlyAllocTestType>
                                                          : bsl::false_type {};
template <>
struct is_nothrow_move_constructible<
                          BloombergLP::bsltf::WellBehavedMoveOnlyAllocTestType>
                                                           : bsl::true_type {};
}  // close namespace bsl

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
