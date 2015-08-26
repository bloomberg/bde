// bdlb_nullableallocatedvalue.h                                      -*-C++-*-
#ifndef INCLUDED_BDLB_NULLABLEALLOCATEDVALUE
#define INCLUDED_BDLB_NULLABLEALLOCATEDVALUE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a template for nullable allocated values.
//
//@CLASSES:
//  bdlb::NullableAllocatedValue: template for nullable allocated values
//
//@AUTHOR: Shezan Baig (sbaig)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
//@SEE_ALSO: bdlb_nullablevalue
//
//@DESCRIPTION: This component provides a a 'bdlb::NullableAllocatedValue'
// template that has the same interface as 'bdlb::NullableValue', except that
// the implementation of this component does not require that the 'TYPE'
// parameter be fully defined when the *class* is instantiated.  However, the
// parameterized 'TYPE' must be fully defined when *methods* of the class are
// instantiated (also: constructors, destructor, operators).
//
///Usage
///-----
// The following snippets of code illustrate the usage of this component.
// Suppose we want to create a linked list of nodes that contain integers:
//..
//  struct LinkedListNode {
//      int                                          d_value;
//      bdlb::NullableAllocatedValue<LinkedListNode> d_next;
//  };
//..
// Note that 'bdlb::NullableValue<LinkedListNode>' cannot be used for 'd_next'
// because 'bdlb::NullableValue' requires that the parameterized 'TYPE' is
// fully defined when the class is instantiated.
//
// We can traverse this linked list and add a new value to at the end using the
// following code:
//..
//  void addValueAtEnd(LinkedListNode *node, int value)
//  {
//      while (!node->d_next.isNull()) {
//          node = &node->d_next.value();
//      }
//
//      node->d_next.makeValue();
//      node = &node->d_next.value();
//      node->d_value = value;
//  }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARPRIMITIVES
#include <bslalg_scalarprimitives.h>
#endif

#ifndef INCLUDED_BSLALG_SWAPUTIL
#include <bslalg_swaputil.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEALLOCATORPROCTOR
#include <bslma_deallocatorproctor.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLMF_IF
#include <bslmf_if.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENT
#include <bsls_alignment.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BDLB_PRINTMETHODS
#include <bdlb_printmethods.h>
#endif

#ifndef INCLUDED_BSLX_INSTREAMFUNCTIONS
#include <bslx_instreamfunctions.h>
#endif

#ifndef INCLUDED_BSLX_OUTSTREAMFUNCTIONS
#include <bslx_outstreamfunctions.h>
#endif

#ifndef INCLUDED_BSLX_VERSIONFUNCTIONS
#include <bslx_versionfunctions.h>
#endif

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>
#endif

namespace BloombergLP {

namespace bdlb {
                  // ========================================
                  // class NullableAllocatedValue<TYPE>
                  // ========================================

template <class TYPE>
class NullableAllocatedValue {
    // This is a template for nullable values.

    // PRIVATE DATA MEMBERS
    TYPE             *d_value_p;
    bslma::Allocator *d_allocator_p;

  public:
    // TYPES
    typedef TYPE ValueType;

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(NullableAllocatedValue,
                                   bslma::UsesBslmaAllocator);
    BSLMF_NESTED_TRAIT_DECLARATION(NullableAllocatedValue,
                                   bdlb::HasPrintMethod);

    // CREATORS
    explicit NullableAllocatedValue(
                                         bslma::Allocator *basicAllocator = 0);
       // Create a nullable object with no value, and use the specified
       // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
       // currently installed default allocator is used.

    NullableAllocatedValue(
                const NullableAllocatedValue<TYPE>&  original,
                bslma::Allocator                          *basicAllocator = 0);
        // Create a copy of the specified 'original' nullable object, and use
        // the specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    NullableAllocatedValue(const TYPE&       value,
                                 bslma::Allocator *basicAllocator = 0);
        // Create an nullable object with the specified 'value', and use the
        // specified 'basicAllocator' to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.

    ~NullableAllocatedValue();
        // Destroy this object.

    // MANIPULATORS
    NullableAllocatedValue<TYPE>& operator=(
                                const NullableAllocatedValue<TYPE>& rhs);
        // Assign to this object the value of the specified 'rhs' object, if
        // 'rhs' has a value.  Otherwise reset this object.  Return a reference
        // to this modifiable nullable object.

    TYPE& operator=(const TYPE& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    void swap(NullableAllocatedValue& other);
        // Swap the value of this object with the value of the specified
        // 'other' object.  This method provides the no-throw guarantee if the
        // 'TYPE' template parameter has a no-throw 'swap' and the result of
        // the 'isNull' method for the two objects being swapped is the same.
        // The behavior is undefined if the objects have non-equal allocators.

    TYPE& makeValue(const TYPE& rhs);
        // Assign the specified 'rhs' value to this nullable object (i.e., make
        // this object not null).

    TYPE& makeValue();
        // Assign the default value of 'TYPE' to this nullable object (i.e.,
        // make this object not null).

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format, and return a
        // reference to 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'version' is not supported, this object
        // is unaltered and 'stream' is invalidated, but otherwise unmodified.
        // If 'version' is supported but 'stream' becomes invalid during this
        // operation, this object has an undefined, but valid, state.  Note
        // that no version is read from 'stream'.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    void reset();
        // Make this object null.

    TYPE& value();
        // Return the value held by this object.  The behavior is undefined
        // unless the object is not null.

    // ACCESSORS
    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object, using the specified 'version'
        // format, to the specified output 'stream', and return a reference to
        // 'stream'.  If 'stream' is initially invalid, this operation has no
        // effect.  If 'version' is not supported, 'stream' is invalidated, but
        // otherwise unmodified.  Note that 'version' is not written to
        // 'stream'.  See the 'bslx' package-level documentation for more
        // information on BDEX streaming of value-semantic types and
        // containers.

    bool isNull() const;
        // Return true if this object is null, and false otherwise.

    int maxSupportedBdexVersion(int versionSelector) const;
        // Return the maximum valid BDEX format version, as indicated by the
        // specified 'versionSelector', to be passed to the 'bdexStreamOut'
        // method.  Note that it is highly recommended that 'versionSelector'
        // be formatted as "YYYYMMDD", a date representation.  Also note that
        // 'versionSelector' should be a *compile*-time-chosen value that
        // selects a format version supported by both externalizer and
        // unexternalizer.  See the 'bslx' package-level documentation for more
        // information on BDEX streaming of value-semantic types and
        // containers.

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    int maxSupportedBdexVersion() const;
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)
#endif  // BDE_OMIT_INTERNAL_DEPRECATED

    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the (absolute
        // value of) the optionally specified indentation 'level' and return a
        // reference to 'stream'.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.

    const TYPE& value() const;
        // Return the value held by this object.  The behavior is undefined
        // unless the object is not null.
};

// FREE OPERATORS
template <class LHS_TYPE, class RHS_TYPE>
bool operator==(const NullableAllocatedValue<LHS_TYPE>& lhs,
                const NullableAllocatedValue<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' is equal to the specified 'rhs',
    // and 'false' otherwise.

template <class LHS_TYPE, class RHS_TYPE>
bool operator!=(const NullableAllocatedValue<LHS_TYPE>& lhs,
                const NullableAllocatedValue<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' is different from the specified
    // 'rhs', and 'false' otherwise.

template <class TYPE>
bsl::ostream& operator<<(bsl::ostream&                             stream,
                         const NullableAllocatedValue<TYPE>& rhs);
    // Print the specified 'rhs' to the specified 'stream' in a single line.


// FREE FUNCTIONS
template <class TYPE>
void swap(NullableAllocatedValue<TYPE>& a,
          NullableAllocatedValue<TYPE>& b);
    // Swap the values of the specified 'a' and 'b' objects.  This method
    // provides the no-throw guarantee if the 'TYPE' template parameter has a
    // no-throw 'swap' and the result of the 'isNull' method for the two
    // objects being swapped is the same.  The behavior is undefined if the
    // objects have non-equal allocators.

// ---  Anything below this line is implementation specific.  Do not use.  ----


// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                  // ----------------------------------------
                  // class NullableAllocatedValue<TYPE>
                  // ----------------------------------------

// CREATORS

template <class TYPE>
inline
NullableAllocatedValue<TYPE>
::NullableAllocatedValue(bslma::Allocator *basicAllocator)
: d_value_p(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

template <class TYPE>
inline
NullableAllocatedValue<TYPE>
::NullableAllocatedValue(
                     const NullableAllocatedValue<TYPE>&  original,
                     bslma::Allocator                          *basicAllocator)
: d_value_p(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (!original.isNull()) {
        makeValue(original.value());
    }
}

template <class TYPE>
inline
NullableAllocatedValue<TYPE>
::NullableAllocatedValue(const TYPE&       value,
                               bslma::Allocator *basicAllocator)
: d_value_p(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    makeValue(value);
}

template <class TYPE>
inline
NullableAllocatedValue<TYPE>
::~NullableAllocatedValue()
{
    reset();
}

// MANIPULATORS

template <class TYPE>
inline
NullableAllocatedValue<TYPE>&
NullableAllocatedValue<TYPE>
::operator=(const NullableAllocatedValue<TYPE>& rhs)
{
    if (!rhs.isNull()) {
        makeValue(rhs.value());
    }
    else {
        reset();
    }

    return *this;
}

template <class TYPE>
inline
TYPE&
NullableAllocatedValue<TYPE>
::operator=(const TYPE& rhs)
{
    return makeValue(rhs);
}

template <class TYPE>
inline
void
NullableAllocatedValue<TYPE>
::swap(NullableAllocatedValue& other)
{
    // 'swap' is undefined for non-equal allocators.

    BSLS_ASSERT(d_allocator_p == other.d_allocator_p);

    // same 'isNull' flags

    if (isNull() && other.isNull()) {
        // nothing to do for empty objects
        return;                                                       // RETURN
    }

    if (!isNull() && !other.isNull()) {
        // swap typed values
        bslalg::SwapUtil::swap(&this->value(), &other.value());
        return;                                                       // RETURN
    }

    // different 'isNull' flags

    NullableAllocatedValue *nullObj;
    NullableAllocatedValue *nonNullObj;

    if (isNull()) {
        nullObj    = this;
        nonNullObj = &other;
    }
    else {
        nullObj    = &other;
        nonNullObj = this;
    }

    // copy-construct and reset
    nullObj->makeValue(nonNullObj->value()); // this can throw, and then 'swap'
                                             // is only strongly exception-safe
    nonNullObj->reset();
}

template <class TYPE>
inline
TYPE&
NullableAllocatedValue<TYPE>
::makeValue(const TYPE& rhs)
{
    if (d_value_p) {
        *d_value_p = rhs;
        return *d_value_p;                                            // RETURN
    }

    TYPE *value = reinterpret_cast<TYPE*>(
                                        d_allocator_p->allocate(sizeof(TYPE)));
    bslma::DeallocatorProctor<bslma::Allocator> proctor(value, d_allocator_p);
    bslalg::ScalarPrimitives::copyConstruct(value, rhs, d_allocator_p);
    proctor.release();
    d_value_p = value;

    return *d_value_p;
}

template <class TYPE>
inline
TYPE&
NullableAllocatedValue<TYPE>
::makeValue()
{
    reset();

    // Note that this alternative implementation instead of 'reset()' provides
    // stronger exception-safety, but it breaks some client code that uses
    // NullableAllocatedValue with a non-value-semantic TYPE.
    //..
    // if (d_value_p) {
    //     *d_value_p = TYPE(d_allocator_p);
    //     return *d_value_p;                                         // RETURN
    // }
    //..

    TYPE *value = reinterpret_cast<TYPE*>(
                                        d_allocator_p->allocate(sizeof(TYPE)));
    bslma::DeallocatorProctor<bslma::Allocator> proctor(value, d_allocator_p);
    bslalg::ScalarPrimitives::defaultConstruct(value, d_allocator_p);
    proctor.release();
    d_value_p = value;

    return *d_value_p;
}

template <class TYPE>
template <class STREAM>
STREAM&
NullableAllocatedValue<TYPE>
::bdexStreamIn(STREAM& stream, int     version)
{
    using bslx::InStreamFunctions::bdexStreamIn;

    char isNull;

    stream.getInt8(isNull);

    if (stream) {
        if (!isNull) {
            makeValue();
            bdexStreamIn(stream, value(), version);
        }
        else {
            reset();
        }
    }

    return stream;
}

template <class TYPE>
inline
void
NullableAllocatedValue<TYPE>
::reset()
{
    if (d_value_p) {
        d_value_p->~TYPE();
        d_allocator_p->deallocate(d_value_p);
        d_value_p = 0;
    }
}

template <class TYPE>
inline
TYPE&
NullableAllocatedValue<TYPE>
::value()
{
    BSLS_ASSERT_SAFE(!isNull());

    return *d_value_p;
}

// ACCESSORS

template <class TYPE>
template <class STREAM>
STREAM&
NullableAllocatedValue<TYPE>
::bdexStreamOut(STREAM& stream, int version) const
{
    using bslx::OutStreamFunctions::bdexStreamOut;

    stream.putInt8(isNull() ? 1 : 0);

    if (!isNull()) {
        bdexStreamOut(stream, value(), version);
    }

    return stream;
}

template <class TYPE>
inline
bool
NullableAllocatedValue<TYPE>
::isNull() const
{
    return 0 == d_value_p;
}

template <class TYPE>
inline
int NullableAllocatedValue<TYPE>::maxSupportedBdexVersion(int versionSelector) const
{
    using bslx::VersionFunctions::maxSupportedBdexVersion;

    // We need to call the 'bslx::VersionFunctions' helper function, because we
    // cannot guarantee that 'TYPE' implements 'maxSupportedBdexVersion' as a
    // class method.

    return maxSupportedBdexVersion(&value(), versionSelector);
}

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
template <class TYPE>
inline
int
NullableAllocatedValue<TYPE>
::maxSupportedBdexVersion() const
{
    return maxSupportedBdexVersion(0);
}
#endif  // BDE_OMIT_INTERNAL_DEPRECATED

template <class TYPE>
inline
bsl::ostream&
NullableAllocatedValue<TYPE>
::print(bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    if (isNull()) {
        return bdlb::PrintMethods::print(stream, "NULL", level, spacesPerLevel);
                                                                      // RETURN
    }

    return bdlb::PrintMethods::print(stream, value(), level, spacesPerLevel);
}

template <class TYPE>
inline
const TYPE&
NullableAllocatedValue<TYPE>
::value() const
{
    // TBD
    // The assert below was commented out because a call to this function is
    // sometimes used as an argument to a template function that only looks at
    // the value type (and does not access the value).

    // BSLS_ASSERT_SAFE(!isNull());

    return *d_value_p;
}
}  // close package namespace

// FREE OPERATORS

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator==(const NullableAllocatedValue<LHS_TYPE>& lhs,
                const NullableAllocatedValue<RHS_TYPE>& rhs)
{
    if (!lhs.isNull() && !rhs.isNull()) {
        return lhs.value() == rhs.value();                            // RETURN
    }

    return lhs.isNull() == rhs.isNull();
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator!=(const NullableAllocatedValue<LHS_TYPE>& lhs,
                const NullableAllocatedValue<RHS_TYPE>& rhs)
{
    if (!lhs.isNull() && !rhs.isNull()) {
        return lhs.value() != rhs.value();                            // RETURN
    }

    return lhs.isNull() != rhs.isNull();
}

template <class TYPE>
inline
bsl::ostream& bdlb::operator<<(bsl::ostream&                             stream,
                         const NullableAllocatedValue<TYPE>& rhs)
{
    return rhs.print(stream, 0, -1);
}

// FREE FUNCTIONS
template <class TYPE>
inline
void bdlb::swap(bdlb::NullableAllocatedValue<TYPE>& a,
                bdlb::NullableAllocatedValue<TYPE>& b)
{
    a.swap(b);
}

}  // close enterprise namespace

 #endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
