// bsltf_allocargumenttype.h                                          -*-C++-*-
#ifndef INCLUDED_BSLTF_ALLOCARGUMENTTYPE
#define INCLUDED_BSLTF_ALLOCARGUMENTTYPE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an allocating test class to represent function arguments
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
    // value 'N', that wraps an integer value and provides implicit conversion
    // to and from 'int'.  The wrapped integer will be dynamically allocated
    // using the supplied allocator, or the default allocator if none is
    // supplied.  Its main purpose is that having separate types for testing
    // enables distinguishing them when calling through a function template
    // interface, thereby avoiding ambiguities or accidental switching of
    // arguments in the implementation of in-place constructors.  It further
    // tests that allocators are propagated correctly, or not, as required.

    // DATA
    bslma::Allocator *const d_allocator_p;
    int              *      d_value_p;
    int                     d_originalValue;

    typedef bslmf::MovableRefUtil MovUtil;

  private:
    // TBD: we need this because assignability is an important requirement in
    //      the implementation of containers
    // void operator=(const AllocArgumentType&);  // =delete;

  public:
    // CREATORS
    explicit AllocArgumentType(bslma::Allocator *basicAllocator =  0);
        // Create a test argument object  having the value '-1' using the
        // optionally specified 'basicAllocator' to supply memory, and using
        // the currently installed default allocator if no such allocator is
        // supplied.

    explicit AllocArgumentType(int               value,
                          bslma::Allocator *basicAllocator =  0);

    AllocArgumentType(const AllocArgumentType& other,
                      bslma::Allocator        *basicAllocator = 0);
        // Create a test argument object having the same value as the specified
        // 'other'.  The optionally specified 'basicAllocator' is used to
        // supply memory, and the currently installed default allocator is used
        // if no such allocator is supplied.

    AllocArgumentType(bslmf::MovableRef<AllocArgumentType> original);
        // Create a test argument object having the same value as the specified
        // 'original', and reset 'original' to have that value negated.

    AllocArgumentType(bslmf::MovableRef<AllocArgumentType> original,
                      bslma::Allocator                    *basicAllocator);
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
        // Return the allocator pointer currently held by this object.

    bool movedFrom() const;
        // Return 'true' if another 'AllocArgumentType' object has adopted
        // the resources of this object, and 'false' otherwise.
};

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
, d_value_p(0)
, d_originalValue(-1)
{
}

template <int N>
inline
AllocArgumentType<N>::AllocArgumentType(int               value,
                                        bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_value_p(new(*d_allocator_p) int(value))
, d_originalValue(*d_value_p)
{
}

template <int N>
inline
AllocArgumentType<N>::AllocArgumentType(
                                       const AllocArgumentType& other,
                                       bslma::Allocator        *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_value_p(new(*d_allocator_p) int(other))
, d_originalValue(*d_value_p)
{
}

template <int N>
inline
AllocArgumentType<N>::AllocArgumentType(
                    BloombergLP::bslmf::MovableRef<AllocArgumentType> original)
: d_allocator_p(MovUtil::access(original).d_allocator_p)
{
    AllocArgumentType& originalRef = MovUtil::access(original);

    d_value_p     = originalRef.d_value_p;
    originalRef.d_value_p = 0;

    d_originalValue =  d_value_p
                    ? *d_value_p
                    :  originalRef;
}

template <int N>
inline
AllocArgumentType<N>::AllocArgumentType(
              BloombergLP::bslmf::MovableRef<AllocArgumentType> original,
              bslma::Allocator                                 *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    AllocArgumentType& originalRef = MovUtil::access(original);

    if (d_allocator_p == originalRef.getAllocator()) {
        d_value_p = originalRef.d_value_p;
        originalRef.d_value_p = 0;
    }
    else {
        d_value_p = new(*d_allocator_p) int(originalRef);
    }

    d_originalValue = *d_value_p;
}

template <int N>
inline
AllocArgumentType<N>::~AllocArgumentType()
{
    if (d_value_p) {
        d_allocator_p->deleteObject(d_value_p);
    }
}

// MANIPULATORS
template <int N>
inline
AllocArgumentType<N>&
AllocArgumentType<N>::operator=(const AllocArgumentType& rhs)
{
    BSLS_ASSERT_SAFE(rhs.d_value_p);
    BSLS_ASSERT_SAFE(d_value_p);

    if (this != &rhs) {
        d_value_p = new(*d_allocator_p) int(*rhs.d_value_p);
        d_originalValue = *d_value_p;
    }
    return *this;
}

template <int N>
inline
AllocArgumentType<N>& AllocArgumentType<N>::operator=(
                         BloombergLP::bslmf::MovableRef<AllocArgumentType> rhs)
{
    AllocArgumentType& lvalue = rhs;

    BSLS_ASSERT_SAFE(lvalue.d_value_p);
    BSLS_ASSERT_SAFE(d_value_p);

    if (this != &lvalue) {
        if (d_allocator_p == lvalue.getAllocator()) {
            d_value_p = lvalue.d_value_p;
            lvalue.d_value_p = 0;
        }
        else {
            d_value_p = new(*d_allocator_p) int(lvalue);
        }
        d_originalValue = *d_value_p;
    }
    return *this;
}

// ACCESSORS
template <int N>
inline
AllocArgumentType<N>::operator int() const
{
    return d_value_p ? *d_value_p : d_originalValue;
}

template <int N>
inline
bslma::Allocator * AllocArgumentType<N>::getAllocator() const
{
    return d_allocator_p;
}

template <int N>
inline
bool AllocArgumentType<N>::movedFrom() const
{
    return !d_value_p;
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
