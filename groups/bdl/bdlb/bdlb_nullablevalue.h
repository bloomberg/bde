// bdlb_nullablevalue.h                                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLB_NULLABLEVALUE
#define INCLUDED_BDLB_NULLABLEVALUE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a template for nullable (in-place) objects.
//
//@CLASSES:
//  bdlb::NullableValue: template for nullable (in-place) objects
//
//@SEE_ALSO: bdlb_nullableallocatedvalue
//
//@DESCRIPTION: This component provides a template class,
// 'bdlb::NullableValue<TYPE>', that can be used to augment an arbitrary
// value-semantic 'TYPE', such as 'int' or 'bsl::string', so that it also
// supports the notion of a "null" value.  That is, the set of values
// representable by the template parameter 'TYPE' is extended to include null.
// If the underlying 'TYPE' is fully value-semantic, then so will the augmented
// type 'bdlb::NullableValue<TYPE>'.  Two homogeneous nullable objects have the
// same value if their underlying 'TYPE' values are the same, or both are null.
//
// Note that the object of template parameter 'TYPE' that is managed by a
// 'bdlb::NullableValue<TYPE>' object is created *in*-*place*.  Consequently,
// the template parameter 'TYPE' must be a complete type when the class is
// instantiated.  In contrast, 'bdlb::NullableAllocatedValue<TYPE>' (see
// 'bdlb_nullableallocatedvalue') does not require that 'TYPE' be complete when
// that class is instantiated, with the trade-off that the managed 'TYPE'
// object is always allocated out-of-place in that case.
//
// In addition to the standard homogeneous, value-semantic, operations such as
// copy construction, copy assignment, equality comparison, and BDEX streaming,
// 'bdlb::NullableValue' also supports conversion between augmented types for
// which the underlying types are convertible, i.e., for heterogeneous copy
// construction, copy assignment, and equality comparison (e.g., between 'int'
// and 'double'); attempts at conversion between incompatible types, such as
// 'int' and 'bsl::string', will fail to compile.  Note that these operational
// semantics are similar to those found in 'bsl::shared_ptr'.
//
///Usage
///-----
// The following snippets of code illustrate use of this component:
//
// First, create a nullable 'int' object:
//..
//  bdlb::NullableValue<int> nullableInt;
//  assert( nullableInt.isNull());
//..
// Next, give the 'int' object the value 123 (making it non-null):
//..
//  nullableInt.makeValue(123);
//  assert(!nullableInt.isNull());
//  assert(123 == nullableInt.value());
//..
// Finally, reset the object to its default constructed state (i.e., null):
//..
//  nullableInt.reset();
//  assert( nullableInt.isNull());
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLB_PRINTMETHODS
#include <bdlb_printmethods.h>
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

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLMF_IF
#include <bslmf_if.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_OBJECTBUFFER
#include <bsls_objectbuffer.h>
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

#ifndef INCLUDED_BSL_NEW
#include <bsl_new.h>
#endif

namespace BloombergLP {
namespace bdlb {

template <class TYPE>
class NullableValue_WithAllocator;

template <class TYPE>
class NullableValue_WithoutAllocator;

                      // =========================
                      // class NullableValue<TYPE>
                      // =========================

template <class TYPE>
class NullableValue {
    // This template class extends the set of values of its value-semantic
    // 'TYPE' parameter to include the notion of a "null" value.  If 'TYPE' is
    // fully value-semantic, then the augmented type 'Nullable<TYPE>' will be
    // as well.  In addition to supporting all homogeneous value-semantic
    // operations, conversions between comparable underlying value types is
    // also supported.  Two nullable objects with different underlying types
    // compare equal if their underlying types are comparable and either (1)
    // both objects are null or (2) the non-null values compare equal.
    // Attempts to copy construct, copy assign, or compare incompatible values
    // types will fail to compile.  The 'NullableValue' template cannot be
    // instantiated on an incomplete type or on a type that overloads
    // 'operator&'.

    // PRIVATE TYPES
    typedef typename
    bslmf::If<bslma::UsesBslmaAllocator<TYPE>::value,
              NullableValue_WithAllocator<TYPE>,
              NullableValue_WithoutAllocator<TYPE> >::Type Imp;

    // DATA
    Imp d_imp;  // managed nullable 'TYPE' object

  public:
    // TYPES
    typedef TYPE ValueType;
        // 'ValueType' is an alias for the underlying 'TYPE' upon which this
        // template class is instantiated, and represents the type of the
        // managed object.

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(NullableValue,
                                      bslma::UsesBslmaAllocator,
                                      bslma::UsesBslmaAllocator<TYPE>::value);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(NullableValue,
                                      bsl::is_trivially_copyable,
                                      bsl::is_trivially_copyable<TYPE>::value);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(NullableValue,
                                      bslmf::IsBitwiseMoveable,
                                      bslmf::IsBitwiseMoveable<TYPE>::value);
    BSLMF_NESTED_TRAIT_DECLARATION(NullableValue, bdlb::HasPrintMethod);
        // 'UsesBslmaAllocator', 'IsBitwiseCopyable', and 'IsBitwiseMoveable'
        // are true for 'NullableValue' only if the corresponding trait is true
        // for 'TYPE'.  'HasPrintMethod' is always true for 'NullableValue'.

    // CREATORS
    NullableValue();
        // Create a nullable object having the null value.  If 'TYPE' takes an
        // optional allocator at construction, use the currently installed
        // default allocator to supply memory.

    explicit NullableValue(bslma::Allocator *basicAllocator);
        // Create a nullable object that has the null value and that uses the
        // specified 'basicAllocator' to supply memory.  Note that this method
        // will fail to compile if 'TYPE' does not take an optional allocator
        // at construction.

    NullableValue(const NullableValue& original);
        // Create a nullable object having the value of the specified
        // 'original' object.  If 'TYPE' takes an optional allocator at
        // construction, use the currently installed default allocator to
        // supply memory.

    NullableValue(const NullableValue&  original,
                  bslma::Allocator     *basicAllocator);
        // Create a nullable object that has the value of the specified
        // 'original' object and that uses the specified 'basicAllocator' to
        // supply memory.  Note that this method will fail to compile if 'TYPE'
        // does not take an optional allocator at construction.

    NullableValue(const TYPE& value);                         // IMPLICIT
        // Create a nullable object having the specified 'value'.  If 'TYPE'
        // takes an optional allocator at construction, use the currently
        // installed default allocator to supply memory.

    NullableValue(const TYPE& value, bslma::Allocator *basicAllocator);
        // Create a nullable object that has the specified 'value' and that
        // uses the specified 'basicAllocator' to supply memory.  Note that
        // this method will fail to compile if 'TYPE' does not take an optional
        // allocator at construction.

    template <class BDE_OTHER_TYPE>
    explicit NullableValue(const NullableValue<BDE_OTHER_TYPE>& original);
        // Create a nullable object having the null value if the specified
        // 'original' object is null, and the value of 'original.value()' (of
        // 'BDE_OTHER_TYPE') converted to 'TYPE' otherwise.  If 'TYPE' takes an
        // optional allocator at construction, use the currently installed
        // default allocator to supply memory.  Note that this method will fail
        // to compile if 'TYPE and 'BDE_OTHER_TYPE' are not compatible.

    template <class BDE_OTHER_TYPE>
    NullableValue(const NullableValue<BDE_OTHER_TYPE>&  original,
                  bslma::Allocator                     *basicAllocator);
        // Create a nullable object that has the null value if the specified
        // 'original' object is null, and the value of 'original.value()' (of
        // 'BDE_OTHER_TYPE') converted to 'TYPE' otherwise; use the specified
        // 'basicAllocator' to supply memory.  Note that this method will fail
        // to compile if 'TYPE' does not take an optional allocator at
        // construction, or if 'TYPE and 'BDE_OTHER_TYPE' are not compatible.

    // ~NullableValue();
        // Destroy this object.  Note that this destructor is generated by the
        // compiler.

    // MANIPULATORS
    NullableValue<TYPE>& operator=(const NullableValue& rhs);
        // Assign to this object the value of the specified 'rhs', and return a
        // reference providing modifiable access to this object.

    template <class BDE_OTHER_TYPE>
    NullableValue<TYPE>& operator=(const NullableValue<BDE_OTHER_TYPE>& rhs);
        // Assign to this object the null value if the specified 'rhs' object
        // is null, and the value of 'rhs.value()' (of 'BDE_OTHER_TYPE')
        // converted to 'TYPE' otherwise.  Return a reference providing
        // modifiable access to this object.  Note that this method will fail
        // to compile if 'TYPE and 'BDE_OTHER_TYPE' are not compatible.

    NullableValue<TYPE>& operator=(const TYPE& rhs);
        // Assign to this object the value of the specified 'rhs', and return a
        // reference providing modifiable access to this object.

    template <class BDE_OTHER_TYPE>
    NullableValue<TYPE>& operator=(const BDE_OTHER_TYPE& rhs);
        // Assign to this object the value of the specified 'rhs' object (of
        // 'BDE_OTHER_TYPE') converted to 'TYPE', and return a reference
        // providing modifiable access to this object.  Note that this method
        // will fail to compile if 'TYPE and 'BDE_OTHER_TYPE' are not
        // compatible.

    void swap(NullableValue& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee if the template parameter 'TYPE' provides
        // that guarantee and the result of the 'isNull' method for the two
        // objects being swapped is the same.  The behavior is undefined unless
        // this object was created with the same allocator, if any, as 'other'.

    TYPE& makeValue(const TYPE& value);
        // Assign to this object the specified 'value', and return a reference
        // providing modifiable access to the underlying 'TYPE' object.

    template <class BDE_OTHER_TYPE>
    TYPE& makeValue(const BDE_OTHER_TYPE& value);
        // Assign to this object the specified 'value' (of 'BDE_OTHER_TYPE')
        // converted to 'TYPE', and return a reference providing modifiable
        // access to the underlying 'TYPE' object.  Note that this method will
        // fail to compile if 'TYPE and 'BDE_OTHER_TYPE' are not compatible.

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

    TYPE valueOr(const TYPE& value) const;
        // Return the value of the underlying 'TYPE' object if this object is
        // non-null, and the specified 'value' otherwise.  Note that this
        // method returns *by* *value*, so may be inefficient in some contexts.

    const TYPE *valueOr(const TYPE *value) const;
        // Return an address providing non-modifiable access to the underlying
        // 'TYPE' object if this object is non-null, and the specified 'value'
        // otherwise.

    const TYPE *valueOrNull() const;
        // Return an address providing non-modifiable access to the underlying
        // 'TYPE' object if this object is non-null, and 0 otherwise.
};

// FREE OPERATORS
template <class LHS_TYPE, class RHS_TYPE>
bool operator==(const NullableValue<LHS_TYPE>& lhs,
                const NullableValue<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' nullable objects have the
    // same value, and 'false' otherwise.  Two nullable objects have the same
    // value if both are null, or if both are non-null and the values of their
    // underlying objects compare equal.  Note that this function will fail to
    // compile if 'LHS_TYPE' and 'RHS_TYPE' are not compatible.

template <class LHS_TYPE, class RHS_TYPE>
bool operator!=(const NullableValue<LHS_TYPE>& lhs,
                const NullableValue<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' nullable objects do not
    // have the same value, and 'false' otherwise.  Two nullable objects do not
    // have the same value if one is null and the other is non-null, or if both
    // are non-null and the values of their underlying objects do not compare
    // equal.  Note that this function will fail to compile if 'LHS_TYPE' and
    // 'RHS_TYPE' are not compatible.

template <class TYPE>
bool operator==(const NullableValue<TYPE>& lhs,
                const TYPE&                rhs);
template <class TYPE>
bool operator==(const TYPE&                lhs,
                const NullableValue<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  A nullable object and a value of the
    // underlying 'TYPE' have the same value if the nullable object is non-null
    // and its underlying value compares equal to the other value.

template <class TYPE>
bool operator!=(const NullableValue<TYPE>& lhs,
                const TYPE&                rhs);
template <class TYPE>
bool operator!=(const TYPE&                lhs,
                const NullableValue<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  A nullable object and a value of the
    // underlying 'TYPE' do not have the same value if either the nullable
    // object is null, or its underlying value does not compare equal to the
    // other value.

template <class TYPE>
bsl::ostream& operator<<(bsl::ostream&              stream,
                         const NullableValue<TYPE>& object);
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
void swap(NullableValue<TYPE>& a, NullableValue<TYPE>& b);
    // Efficiently exchange the values of the specified 'a' and 'b' objects.
    // This method provides the no-throw exception-safety guarantee if the
    // template parameter 'TYPE' provides that guarantee and the result of the
    // 'isNull' method for 'a' and 'b' is the same.  The behavior is undefined
    // unless both objects were created with the same allocator, if any.

               // =======================================
               // class NullableValue_WithAllocator<TYPE>
               // =======================================

template <class TYPE>
class NullableValue_WithAllocator {
    // This template class provides the implementation of 'NullableValue' for
    // types that require an allocator.

    // DATA
    bsls::ObjectBuffer<TYPE>  d_buffer;       // in-place 'TYPE' object

    bool                      d_isNull;       // 'true' if null, otherwise
                                              // 'false'

    bslma::Allocator         *d_allocator_p;  // held, not owned

  private:
    // FRIENDS
    friend class NullableValue<TYPE>;

  public:
    // CREATORS
    explicit NullableValue_WithAllocator(bslma::Allocator *basicAllocator = 0);
        // Create a nullable object having the null value.  Optionally specify
        // a 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    NullableValue_WithAllocator(
                       const NullableValue_WithAllocator&  original,
                       bslma::Allocator                   *basicAllocator = 0);
        // Create a nullable object having the value of the specified
        // 'original' object.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~NullableValue_WithAllocator();
        // Destroy this object.

    // MANIPULATORS
    NullableValue_WithAllocator& operator=(
                                       const NullableValue_WithAllocator& rhs);
        // Assign to this object the value of the specified 'rhs', and return a
        // reference providing modifiable access to this object.

    void swap(NullableValue_WithAllocator& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee if the template parameter 'TYPE' provides
        // that guarantee and the result of the 'isNull' method for the two
        // objects being swapped is the same.  The behavior is undefined unless
        // this object was created with the same allocator as 'other'.

    void makeValue(const TYPE& value);
        // Assign to this object the specified 'value'.

    template <class BDE_OTHER_TYPE>
    void makeValue(const BDE_OTHER_TYPE& value);
        // Assign to this object the specified 'value' (of 'BDE_OTHER_TYPE')
        // converted to 'TYPE'.  Note that this method will fail to compile if
        // 'TYPE and 'BDE_OTHER_TYPE' are not compatible.

    void makeValue();
        // Assign to this object the default value for 'TYPE'.

    void reset();
        // Reset this object to the default constructed state (i.e., to have
        // the null value).

    TYPE& value();
        // Return a reference providing modifiable access to the underlying
        // 'TYPE' object.  The behavior is undefined unless this object is
        // non-null.

    // ACCESSORS
    bool isNull() const;
        // Return 'true' if this object is null, and 'false' otherwise.

    const TYPE& value() const;
        // Return a reference providing non-modifiable access to the underlying
        // 'TYPE' object.  The behavior is undefined unless this object is
        // non-null.
};

              // ==========================================
              // class NullableValue_WithoutAllocator<TYPE>
              // ==========================================

template <class TYPE>
class NullableValue_WithoutAllocator {
    // This template class provides the implementation of 'NullableValue' for
    // types that do *not* require an allocator.

    // DATA
    bsls::ObjectBuffer<TYPE> d_buffer;  // in-place 'TYPE' object
    bool                     d_isNull;  // 'true' if null, otherwise 'false'

  private:
    // FRIENDS
    friend class NullableValue<TYPE>;

  public:
    // CREATORS
    NullableValue_WithoutAllocator();
        // Create a nullable object having the null value.

    NullableValue_WithoutAllocator(
                               const NullableValue_WithoutAllocator& original);
        // Create a nullable object having the value of the specified
        // 'original' object.

    ~NullableValue_WithoutAllocator();
        // Destroy this object.

    // MANIPULATORS
    NullableValue_WithoutAllocator& operator=(
                                    const NullableValue_WithoutAllocator& rhs);
        // Assign to this object the value of the specified 'rhs', and return a
        // reference providing modifiable access to this object.

    void swap(NullableValue_WithoutAllocator& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee if the template parameter 'TYPE' provides
        // that guarantee and the result of the 'isNull' method for the two
        // objects being swapped is the same.

    void makeValue(const TYPE& value);
        // Assign to this object the specified 'value'.

    template <class BDE_OTHER_TYPE>
    void makeValue(const BDE_OTHER_TYPE& value);
        // Assign to this object the specified 'value' (of 'BDE_OTHER_TYPE')
        // converted to 'TYPE'.  Note that this method will fail to compile if
        // 'TYPE and 'BDE_OTHER_TYPE' are not compatible.

    void makeValue();
        // Assign to this object the default value for 'TYPE'.

    void reset();
        // Reset this object to the default constructed state (i.e., to have
        // the null value).

    TYPE& value();
        // Return a reference providing modifiable access to the underlying
        // 'TYPE' object.  The behavior is undefined unless this object is
        // non-null.

    // ACCESSORS
    bool isNull() const;
        // Return 'true' if this object is null, and 'false' otherwise.

    const TYPE& value() const;
        // Return a reference providing non-modifiable access to the underlying
        // 'TYPE' object.  The behavior is undefined unless this object is
        // non-null.
};

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                      // -------------------------
                      // class NullableValue<TYPE>
                      // -------------------------

// CREATORS
template <class TYPE>
inline
NullableValue<TYPE>::NullableValue()
{
}

template <class TYPE>
inline
NullableValue<TYPE>::NullableValue(bslma::Allocator *basicAllocator)
: d_imp(basicAllocator)
{
}

template <class TYPE>
inline
NullableValue<TYPE>::NullableValue(const NullableValue& original)
: d_imp(original.d_imp)
{
}

template <class TYPE>
inline
NullableValue<TYPE>::NullableValue(const NullableValue&  original,
                                   bslma::Allocator     *basicAllocator)
: d_imp(original.d_imp, basicAllocator)
{
}

template <class TYPE>
inline
NullableValue<TYPE>::NullableValue(const TYPE& value)
{
    d_imp.makeValue(value);
}

template <class TYPE>
inline
NullableValue<TYPE>::NullableValue(const TYPE&       value,
                                   bslma::Allocator *basicAllocator)
: d_imp(basicAllocator)
{
    d_imp.makeValue(value);
}

template <class TYPE>
template <class BDE_OTHER_TYPE>
inline
NullableValue<TYPE>::NullableValue(
                                 const NullableValue<BDE_OTHER_TYPE>& original)
{
    if (!original.isNull()) {
        d_imp.makeValue(original.value());
    }
}

template <class TYPE>
template <class BDE_OTHER_TYPE>
inline
NullableValue<TYPE>::NullableValue(
                          const NullableValue<BDE_OTHER_TYPE>&  original,
                          bslma::Allocator                     *basicAllocator)
: d_imp(basicAllocator)
{
    if (!original.isNull()) {
        d_imp.makeValue(original.value());
    }
}

// MANIPULATORS
template <class TYPE>
inline
NullableValue<TYPE>& NullableValue<TYPE>::operator=(const NullableValue& rhs)
{
    d_imp = rhs.d_imp;

    return *this;
}

template <class TYPE>
template <class BDE_OTHER_TYPE>
NullableValue<TYPE>& NullableValue<TYPE>::operator=(
                                      const NullableValue<BDE_OTHER_TYPE>& rhs)
{
    if (rhs.isNull()) {
        d_imp.reset();
    }
    else {
        d_imp.makeValue(rhs.value());
    }
    return *this;
}

template <class TYPE>
inline
NullableValue<TYPE>& NullableValue<TYPE>::operator=(const TYPE& rhs)
{
    d_imp.makeValue(rhs);

    return *this;
}

template <class TYPE>
template <class BDE_OTHER_TYPE>
inline
NullableValue<TYPE>& NullableValue<TYPE>::operator=(const BDE_OTHER_TYPE& rhs)
{
    d_imp.makeValue(rhs);

    return *this;
}

template <class TYPE>
inline
void NullableValue<TYPE>::swap(NullableValue<TYPE>& other)
{
    d_imp.swap(other.d_imp);
}

template <class TYPE>
inline
TYPE& NullableValue<TYPE>::makeValue(const TYPE& value)
{
    d_imp.makeValue(value);

    return d_imp.value();
}

template <class TYPE>
template <class BDE_OTHER_TYPE>
inline
TYPE& NullableValue<TYPE>::makeValue(const BDE_OTHER_TYPE& value)
{
    d_imp.makeValue(value);

    return d_imp.value();
}

template <class TYPE>
inline
TYPE& NullableValue<TYPE>::makeValue()
{
    d_imp.makeValue();

    return d_imp.value();
}

template <class TYPE>
template <class STREAM>
STREAM& NullableValue<TYPE>::bdexStreamIn(STREAM& stream, int version)
{
    using bslx::InStreamFunctions::bdexStreamIn;

    char isNull;

    stream.getInt8(isNull);

    if (stream) {
        if (!isNull) {
            d_imp.makeValue();

            bdexStreamIn(stream, d_imp.value(), version);
        }
        else {
            d_imp.reset();
        }
    }

    return stream;
}

template <class TYPE>
inline
void NullableValue<TYPE>::reset()
{
    d_imp.reset();
}

template <class TYPE>
inline
TYPE& NullableValue<TYPE>::value()
{
    return d_imp.value();
}

// ACCESSORS
template <class TYPE>
template <class STREAM>
STREAM& NullableValue<TYPE>::bdexStreamOut(STREAM& stream, int version) const
{
    using bslx::OutStreamFunctions::bdexStreamOut;

    const bool isNull = d_imp.isNull();

    stream.putInt8(isNull ? 1 : 0);

    if (!isNull) {
        bdexStreamOut(stream, d_imp.value(), version);
    }

    return stream;
}

template <class TYPE>
inline
bool NullableValue<TYPE>::isNull() const
{
    return d_imp.isNull();
}

template <class TYPE>
inline
int NullableValue<TYPE>::maxSupportedBdexVersion(int versionSelector) const
{
    using bslx::VersionFunctions::maxSupportedBdexVersion;

    // We need to call the 'bslx::VersionFunctions' helper function, because we
    // cannot guarantee that 'TYPE' implements 'maxSupportedBdexVersion' as a
    // class method.

    return maxSupportedBdexVersion(reinterpret_cast<TYPE *>(0),
                                   versionSelector);
}


template <class TYPE>
bsl::ostream& NullableValue<TYPE>::print(bsl::ostream& stream,
                                         int           level,
                                         int           spacesPerLevel) const
{
    if (d_imp.isNull()) {
        return bdlb::PrintMethods::print(stream,
                                         "NULL",
                                         level,
                                         spacesPerLevel);             // RETURN
    }

    return bdlb::PrintMethods::print(stream,
                                     d_imp.value(),
                                     level,
                                     spacesPerLevel);
}

template <class TYPE>
inline
const TYPE& NullableValue<TYPE>::value() const
{
    return d_imp.value();
}

template <class TYPE>
inline
const TYPE *NullableValue<TYPE>::valueOrNull() const
{
    return d_imp.isNull() ? 0 : &d_imp.value();
}

template <class TYPE>
inline
TYPE NullableValue<TYPE>::valueOr(const TYPE& value) const
{
    return d_imp.isNull() ? value : d_imp.value();
}

template <class TYPE>
inline
const TYPE *NullableValue<TYPE>::valueOr(const TYPE *value) const
{
    return d_imp.isNull() ? value : &d_imp.value();
}

}  // close package namespace

// FREE OPERATORS
template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator==(const NullableValue<LHS_TYPE>& lhs,
                      const NullableValue<RHS_TYPE>& rhs)
{
    if (!lhs.isNull() && !rhs.isNull()) {
        return lhs.value() == rhs.value();                            // RETURN
    }

    return lhs.isNull() == rhs.isNull();
}

template <class TYPE>
inline
bool bdlb::operator==(const NullableValue<TYPE>& lhs,
                      const TYPE&                rhs)
{
    return !lhs.isNull() && lhs.value() == rhs;
}

template <class TYPE>
inline
bool bdlb::operator==(const TYPE&                lhs,
                      const NullableValue<TYPE>& rhs)
{
    return !rhs.isNull() && rhs.value() == lhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator!=(const NullableValue<LHS_TYPE>& lhs,
                      const NullableValue<RHS_TYPE>& rhs)
{
    if (!lhs.isNull() && !rhs.isNull()) {
        return lhs.value() != rhs.value();                            // RETURN
    }

    return lhs.isNull() != rhs.isNull();
}

template <class TYPE>
inline
bool bdlb::operator!=(const NullableValue<TYPE>& lhs,
                      const TYPE&                rhs)
{
    return lhs.isNull() || lhs.value() != rhs;
}

template <class TYPE>
inline
bool bdlb::operator!=(const TYPE&                lhs,
                      const NullableValue<TYPE>& rhs)
{
    return rhs.isNull() || rhs.value() != lhs;
}

template <class TYPE>
inline
bsl::ostream& bdlb::operator<<(bsl::ostream&              stream,
                               const NullableValue<TYPE>& object)
{
    return object.print(stream, 0, -1);
}

// FREE FUNCTIONS
template <class TYPE>
inline
void bdlb::swap(NullableValue<TYPE>& a, NullableValue<TYPE>& b)
{
    a.swap(b);
}

namespace bdlb {

               // ---------------------------------------
               // class NullableValue_WithAllocator<TYPE>
               // ---------------------------------------

// CREATORS
template <class TYPE>
inline
NullableValue_WithAllocator<TYPE>::NullableValue_WithAllocator(
                                              bslma::Allocator *basicAllocator)
: d_isNull(true)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

template <class TYPE>
inline
NullableValue_WithAllocator<TYPE>::NullableValue_WithAllocator(
                            const NullableValue_WithAllocator&  original,
                            bslma::Allocator                   *basicAllocator)
: d_isNull(true)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (!original.isNull()) {
        makeValue(original.value());
    }
}

template <class TYPE>
inline
NullableValue_WithAllocator<TYPE>::~NullableValue_WithAllocator()
{
    reset();
}

// MANIPULATORS
template <class TYPE>
NullableValue_WithAllocator<TYPE>&
NullableValue_WithAllocator<TYPE>::operator=(
                                        const NullableValue_WithAllocator& rhs)
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
void NullableValue_WithAllocator<TYPE>::swap(
                                            NullableValue_WithAllocator& other)
{
    // 'swap' is undefined for non-equal allocators.

    BSLS_ASSERT(d_allocator_p == other.d_allocator_p);

    // same 'isNull' flags

    if (isNull() && other.isNull()) {
        return;                                                       // RETURN
    }

    if (!isNull() && !other.isNull()) {
        // swap underlying values

        bslalg::SwapUtil::swap(&this->value(), &other.value());
        return;                                                       // RETURN
    }

    // different 'isNull' flags

    NullableValue_WithAllocator *nullObj;
    NullableValue_WithAllocator *nonNullObj;

    if (isNull()) {
        nullObj    = this;
        nonNullObj = &other;
    }
    else {
        nullObj    = &other;
        nonNullObj = this;
    }

    // Copy-construct and reset.

    nullObj->makeValue(nonNullObj->value());  // This can throw, so 'swap' is
                                              // only strongly exception-safe.
    nonNullObj->reset();
}

template <class TYPE>
void NullableValue_WithAllocator<TYPE>::makeValue(const TYPE& value)
{
    if (d_isNull) {
        new (d_buffer.buffer()) TYPE(value, d_allocator_p);
        d_isNull = false;
    }
    else {
        d_buffer.object() = value;
    }
}

template <class TYPE>
template <class BDE_OTHER_TYPE>
void NullableValue_WithAllocator<TYPE>::makeValue(const BDE_OTHER_TYPE& value)
{
    if (d_isNull) {
        new (d_buffer.buffer()) TYPE(value, d_allocator_p);
        d_isNull = false;
    }
    else {
        d_buffer.object() = value;
    }
}

template <class TYPE>
inline
void NullableValue_WithAllocator<TYPE>::makeValue()
{
    reset();

    new (d_buffer.buffer()) TYPE(d_allocator_p);
    d_isNull = false;

    // Note that this alternative implementation provides stronger
    // exception-safety, but it breaks some client code that uses
    // 'NullableValue' with a non-value-semantic 'TYPE'.
    //..
    //  if (d_isNull) {
    //      new (d_buffer.buffer()) TYPE(d_allocator_p);
    //      d_isNull = false;
    //  }
    //  else {
    //      d_buffer.object() = TYPE(d_allocator_p);
    //  }
    //..
}

template <class TYPE>
inline
void NullableValue_WithAllocator<TYPE>::reset()
{
    if (!d_isNull) {
        d_buffer.object().~TYPE();
        d_isNull = true;
    }
}

template <class TYPE>
inline
TYPE& NullableValue_WithAllocator<TYPE>::value()
{
    BSLS_ASSERT_SAFE(!d_isNull);

    return d_buffer.object();
}

// ACCESSORS
template <class TYPE>
inline
bool NullableValue_WithAllocator<TYPE>::isNull() const
{
    return d_isNull;
}

template <class TYPE>
inline
const TYPE& NullableValue_WithAllocator<TYPE>::value() const
{
    BSLS_ASSERT_SAFE(!d_isNull);

    return d_buffer.object();
}

              // ------------------------------------------
              // class NullableValue_WithoutAllocator<TYPE>
              // ------------------------------------------

// CREATORS
template <class TYPE>
inline
NullableValue_WithoutAllocator<TYPE>::NullableValue_WithoutAllocator()
: d_isNull(true)
{
}

template <class TYPE>
inline
NullableValue_WithoutAllocator<TYPE>::
NullableValue_WithoutAllocator(const NullableValue_WithoutAllocator& original)
: d_isNull(true)
{
    if (!original.isNull()) {
        makeValue(original.value());
    }
}

template <class TYPE>
inline
NullableValue_WithoutAllocator<TYPE>::~NullableValue_WithoutAllocator()
{
    reset();
}

// MANIPULATORS
template <class TYPE>
NullableValue_WithoutAllocator<TYPE>&
NullableValue_WithoutAllocator<TYPE>::operator=(
                                     const NullableValue_WithoutAllocator& rhs)
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
void NullableValue_WithoutAllocator<TYPE>::swap(
                                         NullableValue_WithoutAllocator& other)
{
    // same 'isNull' flags

    if (isNull() && other.isNull()) {
        return;                                                       // RETURN
    }

    if (!isNull() && !other.isNull()) {
        // swap underlying values

        bslalg::SwapUtil::swap(&this->value(), &other.value());
        return;                                                       // RETURN
    }

    // different 'isNull' flags

    NullableValue_WithoutAllocator *nullObj;
    NullableValue_WithoutAllocator *nonNullObj;

    if (isNull()) {
        nullObj    = this;
        nonNullObj = &other;
    }
    else {
        nullObj    = &other;
        nonNullObj = this;
    }

    // Copy-construct and reset.

    nullObj->makeValue(nonNullObj->value());  // This can throw, so 'swap' is
                                              // only strongly exception-safe.
    nonNullObj->reset();
}

template <class TYPE>
void NullableValue_WithoutAllocator<TYPE>::makeValue(const TYPE& value)
{
    if (d_isNull) {
        new (d_buffer.buffer()) TYPE(value);
        d_isNull = false;
    }
    else {
        d_buffer.object() = value;
    }
}

template <class TYPE>
template <class BDE_OTHER_TYPE>
void NullableValue_WithoutAllocator<TYPE>::makeValue(
                                                   const BDE_OTHER_TYPE& value)
{
    if (d_isNull) {
        new (d_buffer.buffer()) TYPE(value);
        d_isNull = false;
    }
    else {
        d_buffer.object() = value;
    }
}

template <class TYPE>
inline
void NullableValue_WithoutAllocator<TYPE>::makeValue()
{
    reset();

    new (d_buffer.buffer()) TYPE();
    d_isNull = false;

    // Note that this alternative implementation provides stronger
    // exception-safety, but it breaks some client code that uses
    // 'NullableValue' with a non-value-semantic 'TYPE'.
    //..
    //  if (d_isNull) {
    //      new (d_buffer.buffer()) TYPE();
    //      d_isNull = false;
    //  }
    //  else {
    //      d_buffer.object() = TYPE();
    //  }
    //..
}

template <class TYPE>
inline
void NullableValue_WithoutAllocator<TYPE>::reset()
{
    if (!d_isNull) {
        d_buffer.object().~TYPE();
        d_isNull = true;
    }
}

template <class TYPE>
inline
TYPE& NullableValue_WithoutAllocator<TYPE>::value()
{
    BSLS_ASSERT_SAFE(!d_isNull);

    return d_buffer.object();
}

// ACCESSORS
template <class TYPE>
inline
bool NullableValue_WithoutAllocator<TYPE>::isNull() const
{
    return d_isNull;
}

template <class TYPE>
inline
const TYPE& NullableValue_WithoutAllocator<TYPE>::value() const
{
    BSLS_ASSERT_SAFE(!d_isNull);

    return d_buffer.object();
}

}  // close package namespace
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
