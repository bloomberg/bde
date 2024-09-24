// bsltf_copymovestate.h                                              -*-C++-*-
#ifndef INCLUDED_BSLTF_COPYMOVESTATE
#define INCLUDED_BSLTF_COPYMOVESTATE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an enumeration of copy/move state, including unsupported.
//
//@CLASSES:
//  bsltf::CopyMoveState: namespace for copy/move state enumeration
//
//@SEE_ALSO: bsltf_copymovetracker
//
//@DESCRIPTION: This component provides a `struct`, `bsltf::CopyMoveState`,
// which serves as a namespace for enumerating the copy and move state of an
// object, including an unknown value indicating that the test type does not
// support tracking this information.  A test type can, using a single instance
// of this `enum` track whether the object was most recently copied into, moved
// into, or moved from.
//
// In instances where more than one operatation needs to be tracked (i.e., the
// object was copied into, then moved from), the logical-OR of multiple
// enumerators can be stored in a single variable, but doing so does not
// preserve the order of the operations nor whether any operation occured more
// than once; if such information is needed, multiple instances of this `enum`
// can be used.
//
// The `CopyMoveState` namespace also provides a `toAscii` method that
// converts an enumerator into its name represented as a null-terminated ASCII
// string.
//
///Enumerators
///-----------
// ```
// Name                    Description
// ----------------------  ---------------------------------------------------
// e_ORIGINAL              Original value; object was not copied or moved into
// e_COPIED_INTO           Object was copied into
// e_COPIED_CONST_INTO     Object was copied into from a const reference
// e_COPIED_NONCONST_INTO  Object was copied into from a non-const reference
// e_MOVED_INTO            Object was moved into (from a movable reference)
// e_MOVED_FROM            Object was moved from
// e_UNKNOWN               Object does not track copy/move history
// ```
//
// All of the enumerators have unique values.  The `e_ORIGINAL` enumerator has
// value 0 (no bits set).  The values for `e_COPIED_CONST_INTO` and
// `e_COPIED_NONCONST_INTO` each have the `e_COPIED_INTO` bit set as well as
// one other bit.  The remaining enumerators have one bit set each, sharing no
// bits with any of the other enumerators.
//
// When converted to `bool`, `e_ORIGINAL` yields `false` whereas the rest yield
// `true`.  When `e_UNKNOWN` is not used, this conversion makes it easy create
// a binary test of whether an object was copied or moved.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Tracking an object's history
///- - - - - - - - - - - - - - - - - - - -
// In this example, we show how `CopyMoveState` can be used to track the most
// recent copy or move operation applied to an object.
//
// First, we define a `TrackedValue` type that contains an integer value and a
// `CopyMoveState`
// ```
// #include <bsltf_copymovestate.h>
// #include <bslmf_movableref.h>
// #include <cstring>
//
// struct TrackedValue {
//     int                        d_value;
//     bsltf::CopyMoveState::Enum d_copyMoveState;
// ```
// Next, we define a value constructor that indicates that the object was
// neither copied into, moved into, nor moved from:
// ```
//     TrackedValue(int v = 0)                                     // IMPLICIT
//         : d_value(v)
//         , d_copyMoveState(bsltf::CopyMoveState::e_ORIGINAL) { }
// ```
// Next, we define a copy constructor that records the fact that the object was
// copied into.  As in the case of most copy constructors, the original is
// accessed through a `const` reference, and the copy/moved state reflects that
// fact.
// ```
//     TrackedValue(const TrackedValue& original)
//         : d_value(original.d_value)
//         , d_copyMoveState(bsltf::CopyMoveState::e_COPIED_CONST_INTO) { }
// ```
// Next, we define a move constructor that records both that the object being
// constructed was moved into, but also that the original object was moved
// from.
// ```
//     TrackedValue(bslmf::MovableRef<TrackedValue> original)
//         : d_value(bslmf::MovableRefUtil::access(original).d_value)
//         , d_copyMoveState(bsltf::CopyMoveState::e_MOVED_INTO)
//     {
//         TrackedValue& originalRef   = original;
//         originalRef.d_value         = -1;
//         originalRef.d_copyMoveState = bsltf::CopyMoveState::e_MOVED_FROM;
//     }
// ```
// Next, we declare the destructor and assignment operators, but, for the
// purpose of this example, we don't need to see their implementations:
// ```
//     //! ~TrackedValue() = default;
//
//     TrackedValue& operator=(const TrackedValue&);
//     TrackedValue& operator=(bslmf::MovableRef<TrackedValue>);
// ```
// Then, we define accessors for the value and copy/move state.
// ```
//     int value() const { return d_value; }
//     bsltf::CopyMoveState::Enum copyMoveState() const
//         { return d_copyMoveState; }
// ```
// Now, outside of the class definition or (better yet) as a hidden friend
// function, we define an ADL customization point used to retrieve the
// copy/move state of the tracked value.  This free function will be called in
// generic code that doesn't know whether the type it's working with has a
// `copyMoveState` accessor; a default is provided for types that don't:
// ```
//     friend bsltf::CopyMoveState::Enum copyMoveState(const TrackedValue& v)
//     {
//         return v.copyMoveState();
//     }
// };
// ```
// Finally, we test our `TrackedValue` class by creating an object, copying it,
// and moving it.  After each step, we test that each object's copy/move state
// is as expected.  Note that `e_COPIED_INTO` names a bit that is set in both
// of the other `e_COPIED_*` enumerators.  At the end, we verify that the
// copy/move state is correctly converted to a string by the `toAscii` method
// and that the copy/move state of a plain `int` is always "e_UNKNOWN".
// ```
// int main()
// {
//     typedef bsltf::CopyMoveState Cms;
//
//     TrackedValue tv1(42);
//     assert(42                         == tv1.value());
//     assert(Cms::e_ORIGINAL            == tv1.copyMoveState());
//     assert(Cms::get(tv1)              == tv1.copyMoveState());
//
//     TrackedValue tv2(tv1);  // Copy
//     assert(42                         == tv2.value());
//     assert(Cms::e_COPIED_CONST_INTO   == tv2.copyMoveState());
//     assert(Cms::e_COPIED_INTO          & tv2.copyMoveState());
//
//     TrackedValue tv3(bslmf::MovableRefUtil::move(tv1));  // Move
//     assert(42                         == tv3.value());
//     assert(Cms::e_MOVED_INTO          == tv3.copyMoveState());
//     assert(-1                         == tv1.value());
//     assert(Cms::e_MOVED_FROM          == tv1.copyMoveState());
//
//     assert(0 == std::strcmp("MOVED_FROM",
//                             Cms::toAscii(tv1.copyMoveState())));
//
//     assert(Cms::e_UNKNOWN == Cms::get(5));  // 'int' doesn't track state
// }
// ```

#include <bslscm_version.h>

#include <bslmf_matchanytype.h>

namespace BloombergLP {
namespace bsltf {

                            //=====================
                            // struct CopyMoveState
                            //=====================

struct CopyMoveState {
  public:
    // TYPES
    enum Enum {
        // Enumeration of copy state.

        e_ORIGINAL             = 0x00,
        e_COPIED_INTO          = 0x01,
        e_COPIED_CONST_INTO    = 0x02 | e_COPIED_INTO,
        e_COPIED_NONCONST_INTO = 0x04 | e_COPIED_INTO,
        e_MOVED_INTO           = 0x08,
        e_MOVED_FROM           = 0x10,
        e_UNKNOWN              = 0x20,
        e_MAX_ENUM             = e_UNKNOWN
    };

    // CLASS METHODS

    /// For the specified `obj` return `copyMoveState(obj)`.  Note that
    /// `getCopyMoveState` is an ADL customization point; if not customized
    /// for a specific `TYPE`, a default implementation returning
    /// `e_UNKNOWN` is invoked.
    template <class TYPE>
    static Enum get(const TYPE& obj);

    /// Return `true` if, for the specified `v`, `CopyMoveState::get(v)`
    /// includes the `e_COPIED_CONST_INTO` bits but not `e_UNKNOWN`.
    template <class TYPE>
    static bool isCopiedConstInto(const TYPE& v);

    /// Return `true` if, for the specified `v`, `CopyMoveState::get(v)`
    /// includes the `e_COPIED_INTO` bit but not `e_UNKNOWN`; otherwise
    /// return `false`.
    template <class TYPE>
    static bool isCopiedInto(const TYPE& v);

    /// Return `true` if, for the specified `v`, `CopyMoveState::get(v)`
    /// includes the `e_COPIED_NONCONST_INTO` bits but not `e_UNKNOWN`.
    template <class TYPE>
    static bool isCopiedNonconstInto(const TYPE& v);

    /// Return `true` if, for the specified `v`, `CopyMoveState::get(v)`
    /// includes the `e_MOVED_FROM` bit but not `e_UNKNOWN`; otherwise
    /// return `false`.
    template <class TYPE>
    static bool isMovedFrom(const TYPE& v);

    /// Return `true` if, for the specified `v`, `CopyMoveState::get(v)`
    /// includes the `e_MOVED_INTO` bit but not `e_UNKNOWN`; otherwise
    /// return `false`.
    template <class TYPE>
    static bool isMovedInto(const TYPE& v);

    /// Return `true` if, for the specified `v`, `CopyMoveState::get(v)`
    /// does not include any of the `e_COPIED_INTO`, `e_MOVED_INTO`, or
    /// `e_UNKNOWN` bits; otherwise return 'false.  Note that this
    /// function's return value is unaffected by the `e_MOVED_FROM` bit.
    template <class TYPE>
    static bool isOriginal(const TYPE& v);

    /// Return `true` if, for the specified `v`, `CopyMoveState::get(v)`
    /// includes the `e_UNKNOWN` bit; otherwise return 'false.
    template <class TYPE>
    static bool isUnknown(const TYPE& v);

    /// Return `true` if the specified `val` is a valid value of `Enum`;
    /// otherwise `false`.  A valid value is any of the enumerators or the
    /// bitwise OR of any of the enumerators except `e_UNKNOWN` with
    /// `e_MOVED_FROM`.
    static bool isValid(Enum val);

    /// For the specified `obj_p` address and specified `state`, invoke
    /// `setCopyMoveState(obj_p, state)`.  Note that `setCopyMoveState` is
    /// an ADL customization point; if not customized for a specific `TYPE`,
    /// a no-op default implementation is invoked.
    template <class TYPE>
    static void set(TYPE *obj_p, Enum state);

    /// Return the non-modifiable string representation corresponding to the
    /// specified enumeration `value`, if it exists, and a unique (error)
    /// string otherwise.  The string representation of `value` matches its
    /// corresponding enumerator name with the "e_" prefix elided.  For
    /// example:
    /// ```
    /// bsl::cout << CopyMoveState::toAscii(CopyMoveState::e_COPIED_INTO);
    /// ```
    /// will print the following on standard output:
    /// ```
    /// COPIED_INTO
    /// ```
    /// Any of the *_INTO enums can be bitwise ORed with `e_MOVED_FROM`, in
    /// which case the resulting string will be a comma-separated list of
    /// two names, e.g., "COPIED_CONST_INTO, MOVED_FROM".  Note that
    /// specifying a `value` that does not match any valid combination of
    /// enumerators will result in a string representation that is distinct
    /// from any of those corresponding to the enumerators, but is otherwise
    /// unspecified.
    static const char *toAscii(CopyMoveState::Enum value);
};

// FREE FUNCTIONS

/// Return `true` if `value` is `e_ORIGINAL`; otherwise return `false`.
bool operator!(CopyMoveState::Enum value);

/// Return `true` if the integral values of the specified `a` and `b`
/// compare equal; otherwise return `false`.
bool operator==(CopyMoveState::Enum a, int b);
bool operator==(int a, CopyMoveState::Enum b);

/// Return `true` if the integral values of the specified `a` and `b` do not
/// compare equal; otherwise return `false`.
bool operator!=(CopyMoveState::Enum a, int b);
bool operator!=(int a, CopyMoveState::Enum b);

/// Return `e_UNKWOWN`.  Called from `CopyMoveState::get`, this default
/// implementation is called if a customized `copyMoveState` is not found
/// via ADL for a specific type.  Note that this overload is defined such
/// that, if `copyMoveState` is customized for a type, the customization is
/// "inherited" by derived types.
inline
CopyMoveState::Enum copyMoveState(bslmf::MatchAnyType);

/// Do nothing.  Called from `CopyMoveState::set`, this default
/// implementation is called if a customized `getCopyMoveState` is not found
/// via ADL for a specific type.  Note that this overload is defined such
/// that, if `setCopyMoveState` is customized for a type, the customization
/// is "inherited" by derived types.
void setCopyMoveState(void *, CopyMoveState::Enum);

/// Print the specified `value` as a string.  This is an ADL customization
/// point.
void debugprint(const CopyMoveState::Enum& value);

// ============================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ============================================================================

                            //---------------------
                            // struct CopyMoveState
                            //---------------------

// CLASS METHODS
template <class TYPE>
inline
CopyMoveState::Enum CopyMoveState::get(const TYPE& obj)
{
    return copyMoveState(obj);
}

template <class TYPE>
inline bool CopyMoveState::isCopiedConstInto(const TYPE& v)
{
    return e_COPIED_CONST_INTO == (get(v) & (e_COPIED_CONST_INTO | e_UNKNOWN));
}

template <class TYPE>
inline bool CopyMoveState::isCopiedInto(const TYPE& v)
{
    return e_COPIED_INTO == (get(v) & (e_COPIED_INTO | e_UNKNOWN));
}

template <class TYPE>
inline bool CopyMoveState::isCopiedNonconstInto(const TYPE& v)
{
    return e_COPIED_NONCONST_INTO == (get(v) &
                                      (e_COPIED_NONCONST_INTO | e_UNKNOWN));
}

template <class TYPE>
inline bool CopyMoveState::isMovedFrom(const TYPE& v)
{
    return e_MOVED_FROM == (get(v) & (e_MOVED_FROM | e_UNKNOWN));
}

template <class TYPE>
inline bool CopyMoveState::isMovedInto(const TYPE& v)
{
    return e_MOVED_INTO == (get(v) & (e_MOVED_INTO | e_UNKNOWN));
}

template <class TYPE>
inline bool CopyMoveState::isOriginal(const TYPE& v)
{
    return 0 == (get(v) & (e_COPIED_INTO | e_MOVED_INTO | e_UNKNOWN));
}

template <class TYPE>
inline bool CopyMoveState::isUnknown(const TYPE& v)
{
    return bool(get(v) & e_UNKNOWN);
}

template <class TYPE>
inline void CopyMoveState::set(TYPE *obj_p, Enum state)
{
    setCopyMoveState(obj_p, state);
}

inline bool CopyMoveState::isValid(Enum val)
{
    // 'VALID_MASK' is a bitset with a bit representing every enumerated value
    // less than or equal to 'e_UNKNOWN', with a 1 bit representing a valid
    // enumerator value.
    static const unsigned long long
        k_VALID_MASK = (1ULL <<  e_ORIGINAL                             |
                        1ULL <<  e_COPIED_INTO                          |
                        1ULL <<  e_COPIED_CONST_INTO                    |
                        1ULL <<  e_COPIED_NONCONST_INTO                 |
                        1ULL <<  e_MOVED_INTO                           |
                        1ULL <<  e_MOVED_FROM                           |

                        1ULL << (e_COPIED_INTO          | e_MOVED_FROM) |
                        1ULL << (e_COPIED_CONST_INTO    | e_MOVED_FROM) |
                        1ULL << (e_COPIED_NONCONST_INTO | e_MOVED_FROM) |
                        1ULL << (e_MOVED_INTO           | e_MOVED_FROM) |

                        1ULL <<  e_UNKNOWN                              );

    if (val > e_UNKNOWN) {
        return false;
    }
    else {
        return (1ULL << val) & k_VALID_MASK;  // Find 'e' in bitset
    }
}

}  // close package namespace

// FREE FUNCTIONS
inline bool bsltf::operator!(bsltf::CopyMoveState::Enum value)
{
    return value == 0;
}

inline bool bsltf::operator==(bsltf::CopyMoveState::Enum a, int b)
{
    return int(a) == b;
}

inline bool bsltf::operator==(int a, bsltf::CopyMoveState::Enum b)
{
    return a == int(b);
}

inline bool bsltf::operator!=(bsltf::CopyMoveState::Enum a, int b)
{
    return int(a) != b;
}

inline bool bsltf::operator!=(int a, bsltf::CopyMoveState::Enum b)
{
    return a != int(b);
}

inline bsltf::CopyMoveState::Enum bsltf::copyMoveState(bslmf::MatchAnyType)
{
    return CopyMoveState::e_UNKNOWN;
}

inline void bsltf::setCopyMoveState(void *, bsltf::CopyMoveState::Enum)
{
}

}  // close enterprise namespace

#endif  // INCLUDED_BSLTF_COPYMOVESTATE

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
