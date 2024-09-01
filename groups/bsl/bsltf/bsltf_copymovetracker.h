// bsltf_copymovetracker.h                                            -*-C++-*-
#ifndef INCLUDED_BSLTF_COPYMOVETRACKER
#define INCLUDED_BSLTF_COPYMOVETRACKER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a type that tracks if it's been copied or moved
//
//@CLASSES:
//  bsltf::CopyMoveTracker
//
//@SEE_ALSO: bsltf_copymovestate, bsltf_argumenttype
//
//@DESCRIPTION: This component provides a class, `bsltf::CopyMoveTracker`, that
// keeps track of whether it has been copied to, moved to, or moved from.  It
// is useful for test drivers to ensure that copy and move operations are
// invoked when expected and not invoked when not expected.
//
// Each object of type `CopyMoveTracker` contains a bit mask of type
// `bsltf::CopyMoveState::Enum`.  The constructors and manipulators clear all
// of the bits, then set them as follows:
// ```
//                          e_MOVED_FROM ----------------------.
//                          e_MOVED_INTO ------------------.   |
//               Bits set:  e_COPIED_NONCONST_INTO ----.   |   |
//                          e_COPIED_CONST_INTO ---.   |   |   |
//                          e_COPIED_INTO -----.   |   |   |   |
// Operation:                                  V   V   V   V   V
// +------------------------------------------+---+---+---+---+---+
// | CopyMoveState()                          |   |   |   |   |   |
// | resetCopyMoveState()                     |   |   |   |   |   |
// +------------------------------------------+---+---+---+---+---+
// | CopyMoveState(const CopyMoveState&)      | X | X |   |   |   |
// | operator=(const CopyMoveState&)          | X | X |   |   |   |
// +------------------------------------------+---+---+---+---+---+
// | CopyMoveState(CopyMoveState&)            | X |   | X |   |   |
// | operator=(CopyMoveState&)                | X |   | X |   |   |
// +------------------------------------------+---+---+---+---+---+
// | CopyMoveState(MovableRef<CopyMoveState>) |   |   |   | X |   |
// | operator=(MovableRef<CopyMoveState>)     |   |   |   | X |   |
// +------------------------------------------+---+---+---+---+---+
// ```
// In addition, the move constructor and move assignment operator modify the
// *moved-from* object by setting the `e_MOVED_FROM` bit *in addition* to any
// other bits that might already be set.  Thus, if the `e_MOVED_FROM` bit is
// set, then the *most recent* change was caused by a move-from operation.
// Conversely, if a moved-from object is the target of an (copy or move)
// assignment, the `e_MOVED_FROM` bit is cleared.
//
// The `CopyMoveTracker` class is an in-core value-semantic type having no
// salient attributes, and therefore only one value.  The state of a
// `CopyMoveTracker` object is irrelevant to its value, so all
// `CopyMoveTracker` objects compare equal.  A `CopyMoveTracker` subobject of a
// larger *client* class does not contribute to that class's value (see Usage
// Example 1, below), but the subobject provides all of the necessary
// value-semantic operations, allowing the client class to default the copy and
// move constructors, the copy and move assignment operators, and/or (in C++20)
// the equality comparison operators.
//
///Use via composition vs. inheritance
///-----------------------------------
// When building a new type that tracks copy and move operations, it is
// tempting to include `CopyMoveTracker` via inheritance.  Indeed, this class
// provides features to make inheritance convenient, including providing
// numerous fine-grained accessors named so as to avoid conflicts with
// derived-class members.  By inheriting from `CopyMoveTracker`, these
// accessors become available in the derived class without manually defining
// forwarding functions.
//
// The convenience of using inheritance should be balanced against contravening
// factors.  The simultaneous use of interface and implementation inheritance
// is rarely a good design decision and is generally discouraged in our
// production code base.  Inheriting from this class subjects the user to the
// potential dangers of slicing, e.g., assigning to, or moving from, a
// derived-class object through a base-class reference.  Such issues can
// arguably be avoided fairly easily in relatively small,
// single-translation-unit programs such as test drivers, but users should be
// aware of the peril.  Inheriting from `CopyMoveTracker` should be avoided in
// public header files.
//
// To simplify the use of composition, which is preferred over inheritence, the
// `CopyMoveState` component on which this one is built provides a set of
// *psuedo* *accessors* that mirror the ones provided by `CopyMoveTracker`.  A
// client class `MyType` enables these psuedo accessors by defining an ADL
// customization point named `copyMoveState(const MyType&)` in the namespace in
// which `MyType` is defined.  With this customization point in place, a client
// program can call `bsltf::CopyMoveState::isMovedInto(obj)` instead of
// `obj.isMovedInto()`.  Using a short alias for `bsltf::CopyMoveState`, a call
// to a psuedo accessor for an object of `MyType` is scarcely more verbose than
// a call to a real accessor within `MyType`, without using inheritance and
// without writing a large number of forwarding functions within `MyType`.  The
// usage examples below show the use of composition applying this technique.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: A tracked value class
/// - - - - - - - - - - - - - - - -
// In this example, we create a class that holds an integer value and tracks
// moves and copies.
//
// First, we define the class and it's data members, including a
// `CopyMoveTracker` to keep track of the moves and copies:
// ```
// class TrackedValue {
//     bsltf::CopyMoveTracker d_tracker;
//     int                    d_value;
// ```
// Next, we define the constructors for `TrackedValue` such that they forward
// appropriately the `CopyMoveTracker` member appropriately:
// ```
//   public:
//     explicit TrackedValue(int v = 0) : d_tracker(), d_value(v) { }
//     TrackedValue(const TrackedValue& original)
//         : d_tracker(original.d_tracker), d_value(original.d_value) { }
//     TrackedValue(TrackedValue& original)
//         : d_tracker(original.d_tracker), d_value(original.d_value) { }
//     TrackedValue(bslmf::MovableRef<TrackedValue> original)
//         : d_tracker(bslmf::MovableRefUtil::move(
//                         bslmf::MovableRefUtil::access(original).d_tracker))
//     {
//         TrackedValue& originalLvalue = original;
//         d_value = originalLvalue.d_value;
//         originalLvalue.d_value = -1;
//     }
// ```
// For this example, we don't need to define the assignment operators, but
// their implemenation would be similar to the corresponding constructors.
//
// Next, we define an accessor to return the value of our tracked value.
// ```
//     int value() const { return d_value; }
// ```
// Then, we define a hidden friend function, `copyMoveState`, that returns the
// copy/move state.  This friend function is an ADL customization point that
// allows `CopyMoveState::get(obj)` to return the copy/move state when `obj` is
// a tracked value and allows boolean psuedo-accessors such as
// `CopyMoveState::isMovedFrom(obj)` to query the copy/move state:
// ```
//     friend bsltf::CopyMoveState::Enum copyMoveState(const TrackedValue& v)
//         { return v.d_tracker.copyMoveState(); }
// };
// ```
// Next, we define equality-comparison operators for `TrackedValue`.  Note that
// only the value attribute is compared; the copy/move state is not a salient
// attribute of the class and is thus not part of its value:
// ```
// bool operator==(const TrackedValue &a, const TrackedValue &b)
// {
//     return a.value() == b.value();
// }
//
// BSLS_ANNOTATION_UNUSED
// bool operator!=(const TrackedValue &a, const TrackedValue &b)
// {
//     return a.value() != b.value();
// }
// ```
// Now we use `TrackedValue` in a program, beginning by constructing a
// variable.  The variable is in the not-copied-or-moved state:
// ```
// int main()
// {
//     TrackedValue tv1(99);
//     assert(99 == tv1.value());
//     assert(bsltf::CopyMoveState::isOriginal(tv1));
// ```
// Finally, we make a copy of our `TrackedValue` variable.  The copy is in a
// copied-into state, but it still has the same *value* as `tv1`:
// ```
//     TrackedValue tv2(tv1);
//     assert(99 == tv2.value());
//     assert(bsltf::CopyMoveState::isCopiedInto(tv2));
//     assert(bsltf::CopyMoveState::isCopiedNonconstInto(tv2));
//     assert(tv2 == tv1);
// }
// ```
//
///Example 2: Testing a wrapper template
/// - - - - - - - - - - - - - - - - - -
// In this example, we test a simple wrapper template, `CountedWrapper<T>`,
// that holds an object of type `T` and counts the number of such wrapper
// object currently live in the program.  We begin by sketching the wrapper
// template being tested (with unnecessary details left out):
// ```
// #include <bslmf_util.h>
//
// template <class TYPE>
// class CountedWrapper {
//     // Hold an object of 'TYPE' and count the number of objects.
//
//     // CLASS DATA
//     static int s_count;
//
//     // DATA
//     TYPE d_object;
//
//   public:
//     // CLASS METHODS
//     static int count() { return s_count; }
//
//     // CREATORS
//     CountedWrapper() { ++s_count; }
//
//     template <class ARG>
//     explicit
//     CountedWrapper(BSLS_COMPILERFEATURES_FORWARD_REF(ARG) ctorArg)
//         // Construct the wrapped object by forwarding the specified
//         // 'ctorArg' to the constructor for 'TYPE'.
//         : d_object(BSLS_COMPILERFEATURES_FORWARD(ARG, ctorArg))
//         { ++s_count; }
//
//     ~CountedWrapper() { --s_count; }
//
//     // ...
//
//     // ACCESSORS
//     const TYPE& object() const { return d_object; }
// };
//
// template <class TYPE>
// int CountedWrapper<TYPE>::s_count = 0;
// ```
// Next, we instantiat our wrapper with the `TrackedValue` class from Example 1
// so that we can track whether the argument passed to the wrapper constructor
// is correctly passed to the wrapped object, including preserving its value
// category:
// ```
// typedef CountedWrapper<TrackedValue> WrappedValue;
// ```
// Next, we check that a value-constructed wrapper results in a tracked value
// that has not be copied or moved; i.e., no temporaries were created and then
// copied.  Checking the copy/move state requires calling static methods of
// `bsltf::CopyMoveState`; we make such calls terser by defining `Cms` as an
// abbreviation for `bsltf::CopyMoveState`:
// ```
// int main()
// {
//     typedef bsltf::CopyMoveState Cms;
//
//     WrappedValue wv1(99);  // Default constructor
//     assert(1 == WrappedValue::count());
//     assert(99 == wv1.object().value());
//     assert(Cms::isOriginal(wv1.object()));
// ```
// Next, we check that a wrapper constructed from a `TrackedValue` variable
// forwards the variable as an lvalue, resulting in a call to the copy
// constructor.  We also check that, in C++11, the lvalue is perfectly
// forwarded as a non-const lvalue:
// ```
//     TrackedValue t2(44);
//     assert(Cms::isOriginal(t2));
//
//     WrappedValue wv2(t2);
//     assert(44 == t2.value());                 // Unchanged
//     assert(Cms::isOriginal(t2));              // Unchanged
//     assert(2 == WrappedValue::count());
//     assert(44 == wv2.object().value());
//     assert(Cms::isCopiedInto(wv2.object()));  // Copy constructed
// #ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
//     // Copy constructed from non-const original
//     assert(Cms::isCopiedNonconstInto(wv2.object()));
// #endif
// ```
// Finally, we check that a wrapper constructed from a moved `TrackedValue`
// forwards the variable as an rvalue, resulting in a call to the move
// constructor.  Note that original variable is also modified by this
// operation:
// ```
//     TrackedValue t3(t2);
//     assert(44 == t3.value());
//     assert(Cms::isCopiedNonconstInto(t3));
//
//     WrappedValue wv3(bslmf::MovableRefUtil::move(t3));
//     assert(-1 == t3.value());
//     assert(Cms::isCopiedNonconstInto(t3));
//     assert(Cms::isMovedFrom(t3));
//     assert(3 == WrappedValue::count());
//     assert(44 == wv3.object().value());
//     assert(Cms::isMovedInto(wv3.object()));
// }
// ```

#include <bslscm_version.h>

#include <bslmf_enableif.h>
#include <bslmf_isconvertible.h>
#include <bslmf_movableref.h>
#include <bslmf_issame.h>

#include <bsls_assert.h>
#include <bsls_keyword.h>

#include <bsltf_copymovestate.h>

namespace BloombergLP {
namespace bsltf {

                        // ===================
                        // class CopyMoveTracker
                        // ===================

/// Type that tracks whether it has been copied into, moved into, or moved
/// from.
class CopyMoveTracker {

    // DATA
    CopyMoveState::Enum d_state;

  public:
    // CREATORS

    /// Create object an object in the `e_NOT_COPIED_OR_MOVE` state.
    CopyMoveTracker();

    /// Create object an object in the `e_COPIED_CONST_INTO` state.
    CopyMoveTracker(const CopyMoveTracker& original);

    /// Create object an object in the `e_COPIED_NONCONST_INTO` state.
    CopyMoveTracker(CopyMoveTracker& original);

    /// Create object an object in the `e_MOVED_INTO` state and set the
    /// state of `original` to the bitwise OR of its inititial state and
    /// `e_MOVED_FROM`.
    CopyMoveTracker(bslmf::MovableRef<CopyMoveTracker> original);

    /// Create object an object in the `e_MOVED_INTO` state and set the
    /// state of `original` to the bitwise OR of its initial state and
    /// `e_MOVED_FROM`.  This constructor will not participate in overload
    /// resolution unless a `DERIVED` is derived from `CopyMoveTracker`; in
    /// C++03, therefore, this constructor will simulate automatic
    /// derived-to-base conversion of rvalue references.
    template <class DERIVED>
    CopyMoveTracker(bslmf::MovableRef<DERIVED> original,
                    typename bsl::enable_if<bsl::is_convertible<DERIVED*,
                                        CopyMoveTracker*>::value>::type * = 0);

    //! ~CopyMoveTracker() = default;

    // MANIPULATORS

    /// Set this object to the `e_COPIED_CONST_INTO` state and return a
    /// modifiable reference to this object.
    CopyMoveTracker& operator=(const CopyMoveTracker& rhs);

    /// Set this object to the `e_COPIED_NONCONST_INTO` state and return a
    /// modifiable reference to this object.
    CopyMoveTracker& operator=(CopyMoveTracker& rhs);

    /// Set this object to the `e_MOVED_INTO` state and return a modifiable
    /// reference to this object.  Set the state of `rhs` to the bitwise OR
    /// of its initial state and `e_MOVED_FROM`.
    CopyMoveTracker& operator=(bslmf::MovableRef<CopyMoveTracker> rhs);

    /// Set this object to the `e_MOVED_INTO` state and return a modifiable
    /// reference to this object.  Set the state of `rhs` to the bitwise OR
    /// of its initial state and `e_MOVED_FROM`.  This operator will not
    /// participate in overload resolution unless a `DERIVED` is derived
    /// from `CopyMoveTracker`; in C++03, therefore, this operator will
    /// simulate automatic derived-to-base conversion of rvalue references.
    template <class DERIVED>
    typename bsl::enable_if<
        bsl::is_convertible<DERIVED*, CopyMoveTracker*>::value,
        CopyMoveTracker&
    >::type
    operator=(bslmf::MovableRef<DERIVED> rhs);

    /// Set this object to the `e_NOT_COPIED_OR_MOVE` state.
    void resetCopyMoveState();

    /// Set this object to the specified `state`, which might be the bitwise
    /// OR of `e_MOVED_FROM` and one of the other enumerator values.  The
    /// behavior is undefined unless `CopyMoveState::isValid(state)` is
    /// `true`.
    void setCopyMoveState(CopyMoveState::Enum state);

    /// Set the state of both the specified `this` and `b` objects to the
    /// bitwise OR of `e_MOVED_INTO` and `e_MOVED_FROM`.
    void swapCopyMoveStates(CopyMoveTracker& b);

    // ACCESSORS

    /// Return this object's state.
    CopyMoveState::Enum copyMoveState() const;

    /// Return `true` if this object's state includes the `e_UNKNOWN`
    /// bit.  This attribute can be `true` only if `setCopyMoveState` was
    /// called with an enumerator value that includes the `e_UNKNOWN` bit.
    bool hasUnknownCopyMoveState() const;

    /// Return `true` if this object's state includes the
    /// `e_COPIED_CONST_INTO` bits but not `e_UNKNOWN`.  This attribute is
    /// `true` if this object was copy constructed or copy assigned from a
    /// `const` lvalue (or, in C++03, from a prvalue).
    bool isCopiedConstInto() const;

    /// Return `true` if this object's state includes the `e_COPIED_INTO`
    /// bit but not `e_UNKNOWN`.  This attribute is `true` if either
    /// `isCopiedConstInto()` or `isCopiedNonconstInto()` is true.
    bool isCopiedInto() const;

    /// Return `true` if this object's state includes the
    /// `e_COPIED_NONCONST_INTO` bits but not `e_UNKNOWN`.  This attribute
    /// is `true` if this object was copy constructed or copy assigned from
    /// a non-`const` lvalue.
    bool isCopiedNonconstInto() const;

    /// Return `true` if this object's state includes the `e_MOVED_INTO` bit
    /// but not `e_UNKNOWN`.  This attribute is `true` if this object was
    /// move constructed or was the lhs the of move assignment operator.
    bool isMovedInto() const;

    /// Return `true` if this object's state includes the `e_MOVED_FROM` bit
    /// but not `e_UNKNOWN`.  This attribute is `true` if this object was
    /// the argument of the move constructor or the rhs of the move
    /// assignment operator and was not subsequently modified.
    bool isMovedFrom() const;

    /// Return `true` if this object is not in a copied-into, moved-into, or
    /// unknown state.  This attribute is `true` if this object was
    /// default constructed or reset and not subsequently assigned to.
    bool isOriginal()  const;

    // HIDDEN FRIENDS

    /// Return `true`; the copy/move state is *not* a salient attribute, so
    /// all `CopyMoveTracker` objects compare equal.  Note that this
    /// operator is called implicitly if a client class defines a defaulted
    /// comparison operator (C++20), but will not be selected by overload
    /// resolution for a derived class having no comparison operator.
    template <class OTHER>
    friend BSLS_KEYWORD_CONSTEXPR typename
    bsl::enable_if<bsl::is_same<CopyMoveTracker, OTHER>::value, bool>::type
    operator==(const CopyMoveTracker&, const OTHER&)
        { return true; }

    /// Return `false`; the copy/move state is *not* a salient attribute, so
    /// all `CopyMoveTracker` objects compare equal.  Note that this
    /// operator is called implicitly if a client class defines a defaulted
    /// comparison operator (C++20), but will not be selected by overload
    /// resolution for a derived class having no comparison operator.
    template <class OTHER>
    friend BSLS_KEYWORD_CONSTEXPR typename
    bsl::enable_if<bsl::is_same<CopyMoveTracker, OTHER>::value, bool>::type
    operator!=(const CopyMoveTracker&, const OTHER&)
        { return false; }

    /// Return the state of the specified `tracker`.  This function is an
    /// ADL customization point used by `CopyMoveState::get(obj)`.
    friend
    CopyMoveState::Enum copyMoveState(const CopyMoveTracker& tracker)
        { return tracker.copyMoveState(); }

    /// Set the object at the specified `tracker` address to the specified
    /// `state`.  This function is an ADL customization point used by
    /// `CopyMoveState::set(obj, state)`.  The behavior is undefined unless
    /// `CopyMoveState::isValid(state)` is `true`.
    friend
    void setCopyMoveState(CopyMoveTracker* tracker, CopyMoveState::Enum state)
        { tracker->setCopyMoveState(state); }
};

// ============================================================================
//                TEMPLATE AND INLINE FUNCTION IMPLEMENTATIONS
// ============================================================================

// CREATORS
inline CopyMoveTracker::CopyMoveTracker()
    : d_state(CopyMoveState::e_ORIGINAL)
{
}

inline CopyMoveTracker::CopyMoveTracker(CopyMoveTracker&)
    : d_state(CopyMoveState::e_COPIED_NONCONST_INTO)
{
}

inline CopyMoveTracker::CopyMoveTracker(CopyMoveTracker const&)
    : d_state(CopyMoveState::e_COPIED_CONST_INTO)
{
}

inline
CopyMoveTracker::CopyMoveTracker(bslmf::MovableRef<CopyMoveTracker> original)
    : d_state(CopyMoveState::e_MOVED_INTO)
{
    CopyMoveTracker& originalLvalue = original;
    originalLvalue.d_state = CopyMoveState::Enum(originalLvalue.d_state |
                                                 CopyMoveState::e_MOVED_FROM);
}

template <class DERIVED>
CopyMoveTracker::CopyMoveTracker(
    bslmf::MovableRef<DERIVED> original,
    typename bsl::enable_if<bsl::is_convertible<DERIVED*,
                            CopyMoveTracker*>::value>::type *)
    : d_state(CopyMoveState::e_MOVED_INTO)
{
    // Upcast to base class
    CopyMoveTracker& originalLvalue = bslmf::MovableRefUtil::access(original);
    originalLvalue.d_state = CopyMoveState::Enum(originalLvalue.d_state |
                                                 CopyMoveState::e_MOVED_FROM);
}

// MANIPULATORS
inline CopyMoveTracker& CopyMoveTracker::operator=(CopyMoveTracker& rhs)
{
    if (&rhs != this) {
        d_state = CopyMoveState::e_COPIED_NONCONST_INTO;
    }
    return *this;
}

inline CopyMoveTracker& CopyMoveTracker::operator=(CopyMoveTracker const& rhs)
{
    if (&rhs != this) {
        d_state = CopyMoveState::e_COPIED_CONST_INTO;
    }
    return *this;
}

inline
CopyMoveTracker&
CopyMoveTracker::operator=(bslmf::MovableRef<CopyMoveTracker> rhs)
{
    CopyMoveTracker& rhsLvalue = rhs;
    if (&rhsLvalue != this) {
        d_state = CopyMoveState::e_MOVED_INTO;
        rhsLvalue.d_state = CopyMoveState::Enum(rhsLvalue.d_state |
                                                CopyMoveState::e_MOVED_FROM);
    }
    return *this;
}

template <class DERIVED>
inline
typename bsl::enable_if<
    bsl::is_convertible<DERIVED*, CopyMoveTracker*>::value,
    CopyMoveTracker&
>::type
CopyMoveTracker::operator=(bslmf::MovableRef<DERIVED> rhs)
{
    // Upcast to base class
    CopyMoveTracker& rhsLvalue = bslmf::MovableRefUtil::access(rhs);
    if (&rhsLvalue != this) {
        d_state = CopyMoveState::e_MOVED_INTO;
        rhsLvalue.d_state = CopyMoveState::Enum(rhsLvalue.d_state |
                                                CopyMoveState::e_MOVED_FROM);
    }
    return *this;
}

inline void CopyMoveTracker::resetCopyMoveState()
{
    d_state = CopyMoveState::e_ORIGINAL;
}

inline void CopyMoveTracker::setCopyMoveState(CopyMoveState::Enum state)
{
    BSLS_ASSERT(CopyMoveState::isValid(state));
    d_state = state;
}

inline void CopyMoveTracker::swapCopyMoveStates(CopyMoveTracker& b)
{
    d_state = b.d_state = CopyMoveState::Enum(CopyMoveState::e_MOVED_INTO |
                                              CopyMoveState::e_MOVED_FROM);
}

// ACCESSORS
inline CopyMoveState::Enum CopyMoveTracker::copyMoveState() const
{
    return d_state;
}

inline bool CopyMoveTracker::hasUnknownCopyMoveState() const
{
    return bool(d_state & CopyMoveState::e_UNKNOWN);
}

inline bool CopyMoveTracker::isCopiedConstInto() const
{
    return CopyMoveState::e_COPIED_CONST_INTO ==
        (d_state & (CopyMoveState::e_COPIED_CONST_INTO |
                    CopyMoveState::e_UNKNOWN));
}

inline bool CopyMoveTracker::isCopiedInto() const
{
    return CopyMoveState::e_COPIED_INTO ==
        (d_state & (CopyMoveState::e_COPIED_INTO | CopyMoveState::e_UNKNOWN));
}

inline bool CopyMoveTracker::isCopiedNonconstInto() const
{
    return CopyMoveState::e_COPIED_NONCONST_INTO ==
        (d_state & (CopyMoveState::e_COPIED_NONCONST_INTO |
                    CopyMoveState::e_UNKNOWN));
}

inline bool CopyMoveTracker::isMovedFrom() const
{
    return CopyMoveState::e_MOVED_FROM ==
        (d_state & (CopyMoveState::e_MOVED_FROM | CopyMoveState::e_UNKNOWN));
}

inline bool CopyMoveTracker::isMovedInto() const
{
    return CopyMoveState::e_MOVED_INTO ==
        (d_state & (CopyMoveState::e_MOVED_INTO | CopyMoveState::e_UNKNOWN));
}

inline bool CopyMoveTracker::isOriginal() const
{
    return 0 == (d_state & (CopyMoveState::e_COPIED_INTO |
                            CopyMoveState::e_MOVED_INTO |
                            CopyMoveState::e_UNKNOWN));
}

}  // close package namespace

}  // close enterprise namespace

#endif // ! defined(INCLUDED_BSLTF_COPYMOVETRACKER)

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
