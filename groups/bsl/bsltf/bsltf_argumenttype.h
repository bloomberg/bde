// bsltf_argumenttype.h                                               -*-C++-*-
#ifndef INCLUDED_BSLTF_ARGUMENTTYPE
#define INCLUDED_BSLTF_ARGUMENTTYPE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a non-allocating class to test variadic function arguments.
//
//@CLASSES:
//  bsltf::ArgumentType<N>: simple wrapper around an in-place 'int'
//  bsltf::ArgumentTypeByValue: pass-by-value wrapper for C++03 compatibility
//  bsltf::ArgumentTypeDefault: Default initializer for 'ArgumentType'
//
//@SEE_ALSO: bsltf_allocargumenttype, bsltf_templatetestfacility
//
//@DESCRIPTION: This component provides a class, 'bsltf::ArgumentType<N>', used
// for testing functions that take a variable number of template arguments.
// The integer template parameter enables specification of a number of types
// without requiring a separate component for each.  'bsltf::ArgumentType' does
// not allocate memory, and defines copy and move constructors and assignment
// operators that track whether the object was copied into, moved into, or
// moved from.
//
///Attributes
///----------
//..
//  Name                Type                 Default     Meaning
//  ------------------  -------------------  ----------  --------------------
//  conversion to int   int                  -1          value of object
//  copyMoveState       CopyMoveState::Enum  e_ORIGINAL  copy/move history
//..
// Note that the copy/move history is a *non-saliant* attribute, meaning that
// it is not part of the object's value and does not participate in equality
// comparisons.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Passing Arguments of the Correct Type and Order
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we wanted to test a function, 'forwardData', that takes a variable
// number of arguments and forwards them to another function (called
// 'delegateFunction', in this example).  Note, that the example below provides
// separate implementations for compilers that support C++11 and those that do
// not.  For clarity, we define 'forwardData' inline and limit our C++03
// expansion of the variadic template to 2 arguments.
//
// First, we define a set of 'delegateFunction' overloads.  The first
// argument, if any, is expected to have value '1' and the second argument is
// expected to have value '2'.  The return value of 'delegateFunction' is '1'
// if the first argument is passed as an rvalue, '2' if the second argument is
// passed as an rvalue, '3' if they are both passed as 'rvalues', and '0'
// otherwise.  Using parameters of type 'ArgumentTypeByValue' allows us to
// verify the type, value, and value-category (rvalue vs lvalue) of each
// argument in a manner that is compatible with C++03:
//..
//  int delegateFunction()
//  {
//      return 0;
//  }
//
//  int delegateFunction(bsltf::ArgumentTypeByValue<1> arg01)
//  {
//      assert(1 == arg01);
//      return bsltf::CopyMoveState::isMovedInto(arg01) ? 1 : 0;
//  }
//
//  int delegateFunction(bsltf::ArgumentTypeByValue<1> arg01,
//                       bsltf::ArgumentTypeByValue<2> arg02)
//  {
//      assert(1 == arg01);
//      assert(2 == arg02);
//
//      int ret = 0;
//      if (bsltf::CopyMoveState::isMovedInto(arg01)) ret += 1;
//      if (bsltf::CopyMoveState::isMovedInto(arg02)) ret += 2;
//      return ret;
//  }
//..
// Now, we define the forwarding function we intend to test, providing both
// C++03 and C++11 interfaces:
//..
//  #if defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES) && \_
//      defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
//
//  template <class... Args>
//  inline
//  int forwardData(Args&&... args)
//  {
//      return delegateFunction(BSLS_COMPILERFEATURES_FORWARD(Args, args)...);
//  }
//
//  #else // If variadic templates or rvalue references are not supported
//
//  inline
//  int forwardData()
//  {
//      return delegateFunction();
//  }
//
//  template <class Args1>
//  inline
//  int forwardData(BSLS_COMPILERFEATURES_FORWARD_REF(Args1)  args_01)
//  {
//      return delegateFunction(BSLS_COMPILERFEATURES_FORWARD(Args1, args_01));
//  }
//
//  template <class Args1, class Args2>
//  inline
//  int forwardData(BSLS_COMPILERFEATURES_FORWARD_REF(Args1)  args_01,
//                  BSLS_COMPILERFEATURES_FORWARD_REF(Args2)  args_02)
//  {
//      return delegateFunction(BSLS_COMPILERFEATURES_FORWARD(Args1, args_01),
//                              BSLS_COMPILERFEATURES_FORWARD(Args2, args_02));
//  }
//
//  #endif
//..
// Finally, we define a test case for 'forwardData' passing 'ArgumentType' as
// arguments to the 'forwardData' function and verifying that they are
// perfectly forwarded to 'delegateFunction'.
//..
//  int main()
//  {
//      int ret = forwardData();
//      assert(0 == ret);
//
//      bsltf::ArgumentType<1> A01(1);
//      ret = forwardData(A01);
//      assert(0 == ret);
//
//      bsltf::ArgumentType<1> A11(1);
//      bsltf::ArgumentType<2> A12(2);
//      ret = forwardData(A11, bslmf::MovableRefUtil::move(A12));
//      assert(2 == ret);
//      assert(!bsltf::CopyMoveState::isMovedFrom(A11));
//      assert( bsltf::CopyMoveState::isMovedFrom(A12));
//
//      // Note that passing arguments in a wrong order will fail to compile:
//      // ret = forwardData(A12, A11);  // ERROR
//  }
//..

#include <bslscm_version.h>

#include <bslmf_movableref.h>

#include <bsltf_copymovestate.h>
#include <bsltf_copymovetracker.h>
#include <bsltf_movestate.h>

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>

#include <cstring>
#include <cstdio>

namespace BloombergLP {
namespace bsltf {

                         // ==========================
                         // struct ArgumentTypeDefault
                         // ==========================

struct ArgumentTypeDefault {
    // Empty class used as a default argument initializer for 'ArgumentType'.
    // Example usage:
    //..
    //  void func(ArgumentTypeByValue<1> arg1 = ArgumentTypeDefault(),
    //            ArgumentTypeByValue<2> arg2 = ArgumentTypeDefault());
    //..
};

                     // ===========================
                     // class template ArgumentType
                     // ===========================

template <int N>
class ArgumentType {
    // This class template wraps an integer value and provides implicit
    // conversion to, and explicit conversion from, 'int', while keeping track
    // of copy and move operations.  Its main purpose is for following an
    // argument through a forwarding interface in test drivers.  Each value of
    // the template parameter 'N' yields a unique type, enabling a test driver
    // to distingusih them when calling through a function template interface,
    // thereby avoiding ambiguities or accidental switching of arguments in the
    // implementation of test-class methods and constructors or in calls to
    // methods and constructors of a class template under test.  When an object
    // of this type is copied or moved (via construction or assignment), the
    // value returned by the 'copyMoveState' accessor (the *copy/move*
    // *state*), is updated for the target object and, in the case of a move,
    // for the source object as well.

    // PRIVATE TYPES
    typedef bslmf::MovableRefUtil MoveUtil;

    // DATA
    CopyMoveTracker d_tracker;     // non-salient attribute
    int             d_data;        // attribute value

  public:
    // CREATORS
    ArgumentType(ArgumentTypeDefault = ArgumentTypeDefault());      // IMPLICIT
        // Create an 'ArgumentType' object having the default attribute value
        // '-1'.  Upon return from this constructor, the 'copyMoveState'
        // accessor will return 'e_ORIGINAL', indicating that this object holds
        // an original value, not moved or copied from another object.

    explicit ArgumentType(int value);
        // Create an 'ArgumentType' object having the specified 'value'.  Upon
        // return from this constructor, the 'copyMoveState' accessor will
        // return 'e_ORIGINAL', indicating that this object holds an original
        // value, not moved or copied from another object.  The behavior is
        // undefined unless 'value' is non-negative.

    ArgumentType(ArgumentType      & original);
    ArgumentType(ArgumentType const& original);
        // Create an 'ArgumentType' object having the same value as the
        // specified 'original'.  Upon return from this constructor, the
        // 'copyMoveState' accessor will return either 'e_COPIED_FROM_NONCONST'
        // or 'e_COPIED_FROM_CONST' reflecting the constness of 'original'.
        // The object referred to by 'original' is unchanged regardless of its
        // constness.

    ArgumentType(bslmf::MovableRef<ArgumentType> original);
        // Create an 'ArgumentType' object having the same value as the
        // specified 'original'.  Upon return from this constructor, the
        // integral value of 'original' will be unspecified, the value of
        // 'this->copyMoveState()' will be 'e_MOVED_INTO', and the value of
        // 'original->copyMoveState()' will have its 'e_MOVED_FROM' bit set
        // (unless it started as 'e_UNKNOWN).

    ~ArgumentType();
        // Destroy this object.

    // MANIPULATORS
    ArgumentType& operator=(ArgumentType      & rhs);
    ArgumentType& operator=(ArgumentType const& rhs);
        // Assign to this object the value of the specified 'rhs' object and
        // return a reference providing modifiable access to this object. After
        // the assignment, the 'copyMoveState' accessor will return either
        // 'e_COPIED_FROM_NONCONST' or 'e_COPIED_FROM_CONST' reflecting the
        // constness of 'original'.  The object referred to by 'rhs' is
        // unchanged regardless of its constness.

    ArgumentType& operator=(bslmf::MovableRef<ArgumentType> rhs);
        // Assign to this object the value of the specified 'rhs' object and
        // return a reference providing modifiable access to this object.
        // After the assignment, the integral value of 'rhs' will be
        // unspecified, the value of 'this->copyMoveState()' will be
        // 'e_MOVED_INTO', and the value of 'rhs->copyMoveState()' will have
        // its 'e_MOVED_FROM' bit set (unless it started as 'e_UNKNOWN).

    ArgumentType& operator=(int rhs);
        // Assign to this object the specified 'rhs' and return a reference
        // providing modifiable access to this object.  After the assignment,
        // the 'copyMoveState' accessor will return 'e_ORIGINAL', indicating
        // that this object holds an original value, not moved or copied from
        // another object.  The behavior is undefined unless 'value' is
        // non-negative.

    void reset();
        // Set this object to the default-constructed state.

    void set(int value, CopyMoveState::Enum state);
       // Set this object to the specified 'value' having the specified
       // copy/move 'state'.  The behavior is undefined if the arguments would
       // violate any class invariants, i.e., an invalid copy/move state, a
       // value less than -1, or a moved-from state with value other than -1.

    // ACCESSORS
    operator int() const;
        // Return the value of this object.

    CopyMoveState::Enum copyMoveState() const;
        // Return the copy/move state of this object.

    MoveState::Enum movedFrom() const;
        // !DEPRECATED!: Use 'CopyMoveState::isMovedFrom' instead.
        // Return 'MoveState::e_MOVED' if this object was the source of a move
        // construction or move-assignment operation, 'MoveState::e_NOT_MOVED'
        // if it was not the source of a move operation, and
        // 'MoveState::e_UNKNOWN' if its copy/move state is not known.  Note
        // that converting the result to 'bool' will yield 'true' if this
        // object was moved from and 'false' otherwise.  Note also that the
        // value returned by this accessor may change if this object is
        // subsequently the target of an assignment.

    MoveState::Enum movedInto() const;
        // !DEPRECATED!: Use 'CopyMoveState::isMovedInto' instead.
        // Return 'MoveState::e_MOVED' if this object was the target of a move
        // construction or move-assignment operation, 'MoveState::e_NOT_MOVED'
        // if it was not the target of a move operation, and
        // 'MoveState::e_UNKNOWN' if its copy/move state is not known.  Note
        // that converting the result to 'bool' will yield 'true' if this
        // object was moved into and 'false' otherwise.  Note that the value
        // returned by this accessor may change if this object is subsequently
        // the source of a move operation or the target of an assignment.

    friend
    CopyMoveState::Enum copyMoveState(const ArgumentType& obj)
        // Return the copy/move state of the specified 'obj'.  This function is
        // an ADL customization point used by 'CopyMoveState::get(obj)'.
        { return obj.copyMoveState(); }
};

// FREE FUNCTIONS
template <int N>
void debugprint(const ArgumentType<N>& object);
    // Print the specified 'value' as a string.  This is an ADL customization
    // point.

                 // ==================================
                 // class template ArgumentTypeByValue
                 // ==================================

template <int N>
class ArgumentTypeByValue : public ArgumentType<N> {
    // This class works around a limitation in C++03 whereby initializing a
    // pass-by-value parameter of type 'bslmf::ArgumentType<N>' from a
    // 'MovableRef<bslmf::ArgumentType<N> >' results in an ambiguous
    // conversion sequence (i.e., 'MovableRef::operator T&'
    // vs. 'ArgumentType(MovableRef)' are equally good conversions).
    // Pass-by-value use cases (e.g., recording the value category of multiple
    // function arguments) can use this class as a parameter type, instead,
    // eliminating the ambiguity because 'MovableRef<AT>' -> 'AT&' ->
    // 'ArgumentTypeByValue' requires *two* user-defined conversions and is
    // therefore eliminated during overload resolution.

    typedef bsltf::ArgumentType<N> Base;

  public:
    ArgumentTypeByValue(ArgumentTypeDefault = ArgumentTypeDefault());
                                                                    // IMPLICIT
        // Default-construct the 'ArgumentType<N>' base object.

    ArgumentTypeByValue(ArgumentType<N> const& original);
    ArgumentTypeByValue(ArgumentType<N>      & original);
    ArgumentTypeByValue(bslmf::MovableRef<ArgumentType<N> > original);
        // Initialize the 'ArgumentType<N>' base object from the specified
        // 'original'.
};

// ============================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ============================================================================

                        // ---------------------------
                        // class template ArgumentType
                        // ---------------------------

// CREATORS
template <int N>
inline
ArgumentType<N>::ArgumentType(ArgumentTypeDefault) : d_tracker(), d_data(-1)
{
}

template <int N>
inline
ArgumentType<N>::ArgumentType(int value) : d_tracker(), d_data(value)
{
    BSLS_ASSERT(value >= 0);
}

template <int N>
inline
ArgumentType<N>::ArgumentType(const ArgumentType& original)
    : d_tracker(original.d_tracker), d_data(original.d_data)
{
}

template <int N>
inline
ArgumentType<N>::ArgumentType(ArgumentType& original)
    : d_tracker(original.d_tracker), d_data(original.d_data)
{
}

template <int N>
inline
ArgumentType<N>::ArgumentType(bslmf::MovableRef<ArgumentType> original)
    : d_tracker(MoveUtil::move(MoveUtil::access(original).d_tracker))
    , d_data(MoveUtil::access(original).d_data)
{
    MoveUtil::access(original).d_data = -1;
}

template <int N>
inline
ArgumentType<N>::~ArgumentType()
{
    const int garbage = 0xa5;
    std::memset(static_cast<void*>(this), garbage, sizeof(*this));
}

// MANIPULATORS
template <int N>
inline
ArgumentType<N>& ArgumentType<N>::operator=(ArgumentType& rhs)
{
    if (this != &rhs) {
        d_tracker = rhs.d_tracker;
        d_data    = rhs.d_data;
    }
    return *this;
}

template <int N>
inline
ArgumentType<N>& ArgumentType<N>::operator=(const ArgumentType& rhs)
{
    if (this != &rhs) {
        d_tracker = rhs.d_tracker;
        d_data    = rhs.d_data;
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
        d_tracker     = MoveUtil::move(lvalue.d_tracker);
        d_data        = lvalue.d_data;
        lvalue.d_data = -1;
    }
    return *this;
}

template <int N>
inline
ArgumentType<N>&
ArgumentType<N>::operator=(int rhs)
{
    BSLS_ASSERT(rhs >= 0);

    d_tracker.resetCopyMoveState();
    d_data = rhs;

    return *this;
}

template <int N>
inline
void ArgumentType<N>::reset()
{
    d_tracker.resetCopyMoveState();
    d_data = -1;
}

template <int N>
inline
void ArgumentType<N>::set(int value, CopyMoveState::Enum state)
{
    BSLS_ASSERT(CopyMoveState::isValid(state));
    d_tracker.setCopyMoveState(state);

    BSLS_ASSERT(d_tracker.isMovedFrom() ? value == -1 : value >= -1);
    d_data = value;
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
CopyMoveState::Enum ArgumentType<N>::copyMoveState() const
{
    return d_tracker.copyMoveState();
}

template <int N>
inline
MoveState::Enum ArgumentType<N>::movedFrom() const
{
    return bsltf::getMovedFrom(*this);  // See 'bsltf_movestate.h'
}

template <int N>
inline
MoveState::Enum ArgumentType<N>::movedInto() const
{
    return bsltf::getMovedInto(*this);  // See 'bsltf_movestate.h'
}

// FREE FUNCTIONS
template <int N>
inline
void debugprint(const ArgumentType<N>& object)
{
    std::printf("{ <%d>, %d, %s }", N, int(object),
                CopyMoveState::toAscii(object.copyMoveState()));
}

                // ----------------------------------
                // class template ArgumentTypeByValue
                // ----------------------------------

template <int N>
inline
ArgumentTypeByValue<N>::ArgumentTypeByValue(ArgumentTypeDefault) : Base()
{
}

template <int N>
inline
ArgumentTypeByValue<N>::ArgumentTypeByValue(Base const& original)
    : Base(original)
{
}

template <int N>
inline
ArgumentTypeByValue<N>::ArgumentTypeByValue(Base & original) : Base(original)
{
}

template <int N>
inline
ArgumentTypeByValue<N>::ArgumentTypeByValue(bslmf::MovableRef<Base> original)
    : Base(bslmf::MovableRefUtil::move(original))
{
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
