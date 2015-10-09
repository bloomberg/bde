// bdlb_nullableallocatedvalue.h                                      -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLB_NULLABLEALLOCATEDVALUE
#define INCLUDED_BDLB_NULLABLEALLOCATEDVALUE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a template for nullable allocated (out-of-place) objects.
//
//@CLASSES:
//  bdlb::NullableAllocatedValue: template for nullable allocated objects
//
//@SEE_ALSO: bdlb_nullablevalue
//
//@DESCRIPTION: This component provides a template class,
// 'bdlb::NullableAllocatedValue<TYPE>', that has nearly the same interface as
// 'bdlb::NullableValue' (see 'bdlb_nullablevalue'), but, in contrast with that
// template class, the implementation of 'bdlb::NullableAllocatedValue' does
// not require that the 'TYPE' parameter be a complete type when the *class* is
// instantiated.  However, the template parameter 'TYPE' must be complete when
// *methods* of the class (and free operators) are instantiated.
//
// Note that, as a consequence, the object of template parameter 'TYPE' that
// is managed by a 'bdlb::NullableAllocatedValue<TYPE>' object is necessarily
// allocated out-of-place.  This implies that an allocator is in effect for
// *any* 'bdlb::NullableAllocatedValue<TYPE>' object, regardless of the 'TYPE'.
// In particular, a 'bdlb::NullableAllocatedValue<int>' object, for example,
// incurs use of the default allocator (in effect at the time of creation of
// the object) unless an alternative allocator is supplied at construction.
//
///Usage
///-----
// The following snippets of code illustrate use of this component.
//
// Suppose we want to create a linked list of nodes that contain integers:
//..
//  struct LinkedListNode {
//      int                                          d_value;
//      bdlb::NullableAllocatedValue<LinkedListNode> d_next;
//  };
//..
// Note that 'bdlb::NullableValue<LinkedListNode>' cannot be used for 'd_next'
// because 'bdlb::NullableValue' requires that the template parameter 'TYPE' be
// a complete type when the class is instantiated.
//
// We can now traverse a linked list and add a new value at the end using the
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

#ifndef INCLUDED_BDLB_PRINTMETHODS
#include <bdlb_printmethods.h>
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

#ifndef INCLUDED_BSLMF_ISBITWISEMOVEABLE
#include <bslmf_isbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
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

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {
namespace bdlb {

                  // ==================================
                  // class NullableAllocatedValue<TYPE>
                  // ==================================

template <class TYPE>
class NullableAllocatedValue {
    // This template class extends the set of values of its value-semantic
    // 'TYPE' parameter to include the notion of a "null" value.  If 'TYPE' is
    // fully value-semantic, then the augmented type
    // 'NullableAllocatedValue<TYPE>' will be as well.  In addition to
    // supporting all homogeneous value-semantic operations, conversions
    // between comparable underlying value types is also supported.  Two
    // nullable objects with different underlying types compare equal if their
    // underlying types are comparable and either (1) both objects are null or
    // (2) the non-null values compare equal.  Attempts to copy construct, copy
    // assign, or compare incompatible values types will fail to compile.  The
    // 'NullableAllocatedValue' template can be instantiated on an incomplete
    // type, but it cannot be instantiated on a type that overloads
    // 'operator&'.

    // DATA
    TYPE             *d_value_p;      // managed out-of-place 'TYPE' object
    bslma::Allocator *d_allocator_p;  // held, not owned

  public:
    // TYPES
    typedef TYPE ValueType;
        // 'ValueType' is an alias for the underlying 'TYPE' upon which this
        // template class is instantiated, and represents the type of the
        // managed object.

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(NullableAllocatedValue,
                                   bslma::UsesBslmaAllocator);
    BSLMF_NESTED_TRAIT_DECLARATION(NullableAllocatedValue,
                                   bslmf::IsBitwiseMoveable);
    BSLMF_NESTED_TRAIT_DECLARATION(NullableAllocatedValue,
                                   bdlb::HasPrintMethod);

    // CREATORS
    explicit NullableAllocatedValue(bslma::Allocator *basicAllocator = 0);
        // Create a nullable object having the null value.  Optionally specify
        // a 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    NullableAllocatedValue(const NullableAllocatedValue&  original,
                           bslma::Allocator              *basicAllocator = 0);
        // Create a nullable object having the value of the specified
        // 'original' object.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    NullableAllocatedValue(const TYPE&       value,
                           bslma::Allocator *basicAllocator = 0);
        // Create a nullable object having the specified 'value'.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    ~NullableAllocatedValue();
        // Destroy this object.

    // MANIPULATORS
    NullableAllocatedValue<TYPE>& operator=(const NullableAllocatedValue& rhs);
        // Assign to this object the value of the specified 'rhs', and return a
        // reference providing modifiable access to this object.

    TYPE& operator=(const TYPE& rhs);
        // Assign to this object the value of the specified 'rhs', and return a
        // reference providing modifiable access to the underlying 'TYPE'
        // object.

    void swap(NullableAllocatedValue& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee if the template parameter 'TYPE' provides
        // that guarantee and the result of the 'isNull' method for the two
        // objects being swapped is the same.  The behavior is undefined unless
        // this object was created with the same allocator as 'other'.

    TYPE& makeValue(const TYPE& value);
        // Assign to this object the specified 'value', and return a reference
        // providing modifiable access to the underlying 'TYPE' object.

    TYPE& makeValue();
        // Assign to this object the default value for 'TYPE', and return a
        // reference providing modifiable access to the underlying 'TYPE'
        // object.

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
        // Reset this object to the default constructed state (i.e., to have
        // the null value).

    TYPE& value();
        // Return a reference providing modifiable access to the underlying
        // 'TYPE' object.  The behavior is undefined unless this object is
        // non-null.

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
        // Return 'true' if this object is null, and 'false' otherwise.

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
        // Return a reference providing non-modifiable access to the underlying
        // 'TYPE' object.  The behavior is undefined unless this object is
        // non-null.
};

// FREE OPERATORS
template <class LHS_TYPE, class RHS_TYPE>
bool operator==(const NullableAllocatedValue<LHS_TYPE>& lhs,
                const NullableAllocatedValue<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' nullable objects have the
    // same value, and 'false' otherwise.  Two nullable objects have the same
    // value if both are null, or if both are non-null and the values of their
    // underlying objects compare equal.  Note that this function will fail to
    // compile if 'LHS_TYPE' and 'RHS_TYPE' are not compatible.

template <class LHS_TYPE, class RHS_TYPE>
bool operator!=(const NullableAllocatedValue<LHS_TYPE>& lhs,
                const NullableAllocatedValue<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' nullable objects do not
    // have the same value, and 'false' otherwise.  Two nullable objects do not
    // have the same value if one is null and the other is non-null, or if both
    // are non-null and the values of their underlying objects do not compare
    // equal.  Note that this function will fail to compile if 'LHS_TYPE' and
    // 'RHS_TYPE' are not compatible.

template <class TYPE>
bsl::ostream& operator<<(bsl::ostream&                       stream,
                         const NullableAllocatedValue<TYPE>& object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a reference to 'stream'.
    // If 'stream' is not valid on entry, this operation has no effect.  Note
    // that this human-readable format is not fully specified, can change
    // without notice, and is logically equivalent to:
    //..
    //  print(stream, 0, -1);
    //..

// FREE FUNCTIONS
template <class TYPE>
void swap(NullableAllocatedValue<TYPE>& a,
          NullableAllocatedValue<TYPE>& b);
    // Efficiently exchange the values of the specified 'a' and 'b' objects.
    // This method provides the no-throw exception-safety guarantee if the
    // template parameter 'TYPE' provides that guarantee and the result of
    // the 'isNull' method for 'a' and 'b' is the same.  The behavior is
    // undefined unless both objects were created with the same allocator.

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                  // ----------------------------------
                  // class NullableAllocatedValue<TYPE>
                  // ----------------------------------

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
::NullableAllocatedValue(const NullableAllocatedValue<TYPE>&  original,
                         bslma::Allocator                    *basicAllocator)
: d_value_p(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (!original.isNull()) {
        makeValue(original.value());
    }
}

template <class TYPE>
inline
NullableAllocatedValue<TYPE>::NullableAllocatedValue(
                                              const TYPE&       value,
                                              bslma::Allocator *basicAllocator)
: d_value_p(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    makeValue(value);
}

template <class TYPE>
inline
NullableAllocatedValue<TYPE>::~NullableAllocatedValue()
{
    reset();
}

// MANIPULATORS
template <class TYPE>
inline
NullableAllocatedValue<TYPE>&
NullableAllocatedValue<TYPE>::operator=(
                                       const NullableAllocatedValue<TYPE>& rhs)
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
TYPE& NullableAllocatedValue<TYPE>::operator=(const TYPE& rhs)
{
    return makeValue(rhs);
}

template <class TYPE>
void NullableAllocatedValue<TYPE>::swap(NullableAllocatedValue& other)
{
    // 'swap' is undefined for non-equal allocators.

    BSLS_ASSERT(d_allocator_p == other.d_allocator_p);

    // Nothing to do if both objects are null.

    if (isNull() && other.isNull()) {
        return;                                                       // RETURN
    }

    // Otherwise, simply swap the pointers to the out-of-place objects.

    bslalg::SwapUtil::swap(&d_value_p, &other.d_value_p);
}

template <class TYPE>
inline
TYPE& NullableAllocatedValue<TYPE>::makeValue(const TYPE& value)
{
    if (d_value_p) {
        *d_value_p = value;
        return *d_value_p;                                            // RETURN
    }

    TYPE *tmpPtr = reinterpret_cast<TYPE *>(
                                        d_allocator_p->allocate(sizeof(TYPE)));
    bslma::DeallocatorProctor<bslma::Allocator> proctor(tmpPtr, d_allocator_p);
    bslalg::ScalarPrimitives::copyConstruct(tmpPtr, value, d_allocator_p);
    proctor.release();
    d_value_p = tmpPtr;

    return *d_value_p;
}

template <class TYPE>
inline
TYPE& NullableAllocatedValue<TYPE>::makeValue()
{
    reset();

    // Note that this alternative implementation, instead of 'reset()',
    // provides stronger exception-safety, but it breaks some client code that
    // uses 'NullableAllocatedValue' with a non-value-semantic 'TYPE'.
    //..
    //  if (d_value_p) {
    //      *d_value_p = TYPE(d_allocator_p);
    //      return *d_value_p;                                        // RETURN
    //  }
    //..

    TYPE *value = reinterpret_cast<TYPE *>(
                                        d_allocator_p->allocate(sizeof(TYPE)));
    bslma::DeallocatorProctor<bslma::Allocator> proctor(value, d_allocator_p);
    bslalg::ScalarPrimitives::defaultConstruct(value, d_allocator_p);
    proctor.release();
    d_value_p = value;

    return *d_value_p;
}

template <class TYPE>
template <class STREAM>
STREAM& NullableAllocatedValue<TYPE>::bdexStreamIn(STREAM& stream, int version)
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
void NullableAllocatedValue<TYPE>::reset()
{
    if (d_value_p) {
        d_value_p->~TYPE();
        d_allocator_p->deallocate(d_value_p);
        d_value_p = 0;
    }
}

template <class TYPE>
inline
TYPE& NullableAllocatedValue<TYPE>::value()
{
    BSLS_ASSERT_SAFE(!isNull());

    return *d_value_p;
}

// ACCESSORS
template <class TYPE>
template <class STREAM>
STREAM& NullableAllocatedValue<TYPE>::bdexStreamOut(STREAM& stream,
                                                    int     version) const
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
bool NullableAllocatedValue<TYPE>::isNull() const
{
    return 0 == d_value_p;
}

template <class TYPE>
inline
int NullableAllocatedValue<TYPE>::maxSupportedBdexVersion(
                                                     int versionSelector) const
{
    using bslx::VersionFunctions::maxSupportedBdexVersion;

    // We need to call the 'bslx::VersionFunctions' helper function, because we
    // cannot guarantee that 'TYPE' implements 'maxSupportedBdexVersion' as a
    // class method.

    return maxSupportedBdexVersion(reinterpret_cast<TYPE *>(0),
                                   versionSelector);
}


template <class TYPE>
inline
bsl::ostream& NullableAllocatedValue<TYPE>::print(
                                            bsl::ostream& stream,
                                            int           level,
                                            int           spacesPerLevel) const
{
    if (isNull()) {
        return bdlb::PrintMethods::print(stream,
                                         "NULL",
                                         level,
                                         spacesPerLevel);             // RETURN
    }

    return bdlb::PrintMethods::print(stream, value(), level, spacesPerLevel);
}

template <class TYPE>
inline
const TYPE& NullableAllocatedValue<TYPE>::value() const
{
    BSLS_ASSERT_SAFE(!isNull());

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
bsl::ostream& bdlb::operator<<(bsl::ostream&                       stream,
                               const NullableAllocatedValue<TYPE>& object)
{
    return object.print(stream, 0, -1);
}

// FREE FUNCTIONS
template <class TYPE>
inline
void bdlb::swap(NullableAllocatedValue<TYPE>& a,
                NullableAllocatedValue<TYPE>& b)
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
