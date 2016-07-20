// bsltf_argumenttype.h                                               -*-C++-*-
#ifndef INCLUDED_BSLTF_ARGUMENTTYPE
#define INCLUDED_BSLTF_ARGUMENTTYPE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a non-allocating test class to represent a func argument
//
//@REVIEW_FOR_MASTER:
//
//@CLASSES:
//   bsltf::ArgumentType<N>: simple wrapper around an in-place 'int'
//
//@SEE_ALSO: bsltf_templatetestfacility
//
//@DESCRIPTION: This component provides a representation of a non-allocating
// argument type template class used for testing functions that take a variable
// number of template arguments.  The integer template parameter enables
// specification of a number of types without requiring a separate component
// for each.  Note that there are no manipulators, not even an assignment
// operator, for the class and the value constructor is the only way to assign
// an object a value.  Copy and move constructors are defined.
//
///Attributes
///----------
//..
//  Name                Type         Default
//  ------------------  -----------  -------
//  data                int          0
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

                         // =======================
                         // class ArgumentType<int>
                         // =======================

template <int N>
class ArgumentType {
    // This class template declares a separate type for each template parameter
    // value 'N', that wraps an integer value and provides implicit conversion
    // to and from 'int'.  Its main purpose is that having separate types for
    // testing enables distinguishing them when calling through a function
    // template interface, thereby avoiding ambiguities or accidental switching
    // of arguments in the implementation of in-place constructors.

    // DATA
    int  d_value;
    bool d_movedFrom;

    typedef bslmf::MovableRefUtil MovUtil;

  private:
    // TBD: we need this because assignability is an important requirement in
    //      the implementation of containers
    // void operator=(const ArgumentType&);  // =delete;

  public:
    // CREATORS
    explicit ArgumentType(int value = -1);
        // Create a test argument object having the optionally specified
        // 'value', and having the value '-1' otherwise.

    ArgumentType(const ArgumentType& other);
        // Create a test argument object having the same value as the specified
        // 'other'.

    ArgumentType(BloombergLP::bslmf::MovableRef<ArgumentType> original);
        // Create a test argument object having the same value as the specified
        // 'original'.  Note that 'original' is left in a valid but unspecified
        // state.

    // MANIPULATORS

    // TBD: we need this because assignability is an important requirement in
    //      the implementation of containers
    ArgumentType& operator=(const ArgumentType& rhs);
        // Assign to this test argument the value of the specified 'rhs'
        // object.

    ArgumentType& operator=(BloombergLP::bslmf::MovableRef<ArgumentType> rhs);
        // Assign to this test argument the value of the specified 'rhs'
        // object.  Note that 'rhs' is left in a valid but unspecified state.

    // ACCESSORS
    operator int() const;
        // Return the value of this test argument object.

    bool movedFrom() const;
        // Return 'true' if another 'ArgumentType' object has adopted the state
        // of this object, and 'false' otherwise.
};


// ============================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ============================================================================

                        // ---------------------
                        // class ArgumentType<N>
                        // ---------------------

// CREATORS
template <int N>
inline
ArgumentType<N>::ArgumentType(int value)
: d_value(value)
, d_movedFrom(false)
{
}

template <int N>
inline
ArgumentType<N>::ArgumentType(const ArgumentType& other)
: d_value(other.d_value)
, d_movedFrom(other.d_movedFrom)
{
}

template <int N>
inline
ArgumentType<N>::ArgumentType(
                  BloombergLP::bslmf::MovableRef<ArgumentType> original)
: d_value(MovUtil::access(original).d_value)
, d_movedFrom(MovUtil::access(original).d_movedFrom)
{
    MovUtil::access(original).d_movedFrom = true;
}

// MANIPULATORS
template <int N>
inline
ArgumentType<N>& ArgumentType<N>::operator=(const ArgumentType& rhs)
{
    BSLS_ASSERT_SAFE(false == rhs.d_movedFrom);
    BSLS_ASSERT_SAFE(false == d_movedFrom);

    if (this != &rhs) {
        d_value = rhs.d_value;
        d_movedFrom = rhs.d_movedFrom;
    }
    return *this;
}

template <int N>
inline
ArgumentType<N>&
ArgumentType<N>::operator=(BloombergLP::bslmf::MovableRef<ArgumentType> rhs)
{
    ArgumentType& lvalue = rhs;

    BSLS_ASSERT_SAFE(false == rhs.d_movedFrom);
    BSLS_ASSERT_SAFE(false == d_movedFrom);

    if (this != &lvalue) {
        d_value = lvalue.d_value;
        d_movedFrom = lvalue.d_movedFrom;
        lvalue.d_movedFrom = true;
    }
    return *this;
}

// ACCESSORS
template <int N>
inline
ArgumentType<N>::operator int() const
{
    return d_value;
}

template <int N>
inline
bool
ArgumentType<N>::movedFrom() const
{
    return d_movedFrom;
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
