// bdeut_nullablevalue.h                                              -*-C++-*-
#ifndef INCLUDED_BDEUT_NULLABLEVALUE
#define INCLUDED_BDEUT_NULLABLEVALUE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a template for nullable types.
//
//@CLASSES:
//   bdeut_NullableValue: template for nullable types
//
//@AUTHOR: Ilougino Rocha (irocha), John Lakos (jlakos)
//
//@SEE_ALSO: bdeut_nullableallocatedvalue
//
//@DESCRIPTION:
// This component provides a template, 'bdeut_NullableValue<TYPE>', that can be
// used to augment an arbitrary value-semantic 'TYPE', such as 'int' or
// 'bsl::string', into one that also supports the notion of a "null" value.
// That is, the set of values representable by the parameterized 'TYPE' is
// extended to include the null value.  If the underlying 'TYPE' is fully
// value-semantic, then so will the augmented type 'bdeut_NullableValue<TYPE>'.
// Two homogeneous nullable objects have the same value if their underlying
// types have the same value, or both are null.
//
// In addition to the standard homogeneous, value-semantic, operations such as
// copy construction, copy assignment, equality comparison, and 'bdex'
// streaming, 'bdeut_NullableValue' also supports conversion between augmented
// types for which the underlying types are convertible, i.e., for
// heterogeneous copy construction, copy assignment, and equality comparison,
// e.g., between 'int' and 'double'; attempts at conversion between
// incompatible types, such as 'int' and 'bsl::string', will fail to compile.
// Note that these operational semantics are similar to those found in
// 'bcema_sharedptr'.
//
///Usage
///-----
// The following snippet of code illustrates common usage of this component:
//..
//  bdeut_NullableValue<int> nullableInt;
//  assert( nullableInt.isNull());
//
//  nullableInt.makeValue(123);
//  assert(!nullableInt.isNull());
//  assert(123 == nullableInt.value());
//
//  nullableInt.reset();
//  assert( nullableInt.isNull());
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEU_PRINTMETHODS
#include <bdeu_printmethods.h>
#endif

#ifndef INCLUDED_BDEX_INSTREAMFUNCTIONS
#include <bdex_instreamfunctions.h>
#endif

#ifndef INCLUDED_BDEX_OUTSTREAMFUNCTIONS
#include <bdex_outstreamfunctions.h>
#endif

#ifndef INCLUDED_BDEX_VERSIONFUNCTIONS
#include <bdex_versionfunctions.h>
#endif

#ifndef INCLUDED_BSLALG_SWAPUTIL
#include <bslalg_swaputil.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
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

#ifdef BSLS_PLATFORM_CMP_IBM
// This is a temporary workaround for a bug that occurs when using AIX xlC
// compiler.  When compiling a CPP file that uses a nullable value, AIX xlC
// compiler incorrectly gives an error that 'bdex_ByteInStreamFormatter' and
// 'bdex_ByteOutStreamFormatter' are not defined, even though the file
// correctly includes 'bdex_byteinstreamformatter.h' and
// 'bdex_byteoutstreamformatter.h'.  (Basically, it is the bug where the
// compiler processes an expression that depends on a template parameter,
// before the template has been instantiated).  The temporary workaround is to
// ensure that 'bdex_ByteInStreamFormatter' and 'bdex_ByteOutStreamFormatter'
// are defined before 'bdeut_NullableAllocatedValue' in the translation unit,
// so these files get included here (even though 'bdeut_nullableallocatedvalue'
// does not really depend on the two 'bdex' components).

#ifndef INCLUDED_BDEX_BYTEINSTREAMFORMATTER
#include <bdex_byteinstreamformatter.h>
#endif

#ifndef INCLUDED_BDEX_BYTEOUTSTREAMFORMATTER
#include <bdex_byteoutstreamformatter.h>
#endif

#endif  // BSLS_PLATFORM_CMP_IBM

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {

template <typename TYPE> struct bdeut_NullableValue_Traits;
template <typename TYPE> class  bdeut_NullableValue_WithAllocator;
template <typename TYPE> class  bdeut_NullableValue_WithoutAllocator;

                      // ===============================
                      // class bdeut_NullableValue<TYPE>
                      // ===============================

template <typename TYPE>
class bdeut_NullableValue {
    // This is a template extends the set of values of its parameterized
    // value-semantic 'TYPE' to include the notion of a "null" value.
    // If 'TYPE' is fully value semantic, then the augmented type
    // 'bdeut_Nullable<TYPE>' will be as well.  In addition to supporting
    // all homogeneous value semantic operations, conversions between
    // comparable underlying value types is also supported.  Note that
    // two nullable objects with different underlying types compare
    // equal if they their underlying types are comparable and either
    // (1) both objects are null or (2) the non-null values compare equal.
    // Attempts to copy construct, copy assign, or compare incompatible
    // values will fail to compile.

    // PRIVATE CONSTANTS
    enum {
        USES_ALLOCATOR
             = bslalg_HasTrait<TYPE, bslalg_TypeTraitUsesBslmaAllocator>::VALUE
    };

    // PRIVATE TYPES
    typedef typename
    bslmf_If<USES_ALLOCATOR,
             bdeut_NullableValue_WithAllocator<TYPE>,
             bdeut_NullableValue_WithoutAllocator<TYPE> >::Type Imp;

    typedef bdeut_NullableValue_Traits<TYPE> NullableValueTraits;

    // DATA
    Imp d_imp;

  public:
    // TYPES
    typedef TYPE ValueType;
        // 'ValueType' is an alias for the underlying 'TYPE' upon which
        // this template class is parameterized, and represents the type
        // a nullable object represents when it is not null.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdeut_NullableValue, NullableValueTraits);

    // CREATORS
    bdeut_NullableValue();
        // Create a nullable object that is initially null.  Note that
        // attempts to refer to the underlying 'ValueType' before it is
        // established will result in undefined behavior.

    explicit bdeut_NullableValue(bslma_Allocator *basicAllocator);
        // Create a nullable object that is initially null, and that uses the
        // specified 'basicAllocator' to supply memory.  Note that attempts
        // to refer to the underlying 'ValueType' before it is established
        // will result in undefined behavior.  Also note that attempts to call
        // this constructor for an underlying 'ValueType' that does not take
        // an optional allocator at construction will fail to compile.

    bdeut_NullableValue(const bdeut_NullableValue& original);
        // Create a nullable object that is initially null if the specified
        // 'original' object is null, and otherwise has the value (of
        // parameterized 'TYPE') of 'original'.

    bdeut_NullableValue(const bdeut_NullableValue&  original,
                        bslma_Allocator            *basicAllocator);
        // Create a nullable object that is initially null if the specified
        // 'original' object is null, and otherwise has the value (of
        // parameterized 'TYPE') of 'original', and that uses the specified
        // 'basicAllocator' to supply memory.  Note that attempts to call this
        // constructor for an underlying 'ValueType' that does not take an
        // optional allocator at construction will fail to compile.

    bdeut_NullableValue(const TYPE& value);
        // Create a nullable object having the specified (non-null) 'value' of
        // parameterized 'TYPE'.  Note that this object will initially not be
        // null.

    bdeut_NullableValue(const TYPE&      value,
                        bslma_Allocator *basicAllocator);
        // Create a nullable object having the specified (non-null) 'value' of
        // parameterized TYPE', and that uses the specified 'basicAllocator'
        // to supply memory.  Note that this object will initially not be null.
        // Also note that attempts to call this constructor for an underlying
        // 'ValueType' that does not take an optional allocator at construction
        // will fail to compile.

    template <typename BDE_OTHER_TYPE>
    explicit
    bdeut_NullableValue(const bdeut_NullableValue<BDE_OTHER_TYPE>& original);
        // Create a nullable object that is initially null if the specified
        // 'original' object is null, and otherwise has, as the value of its
        // underlying 'ValueType, a value converted from the value (of
        // parameterized 'BDE_OTHER_TYPE') of 'original'.  Note that attempts
        // to call this method with incompatible underlying types will fail to
        // compile.

    template <typename BDE_OTHER_TYPE>
    bdeut_NullableValue(
                       const bdeut_NullableValue<BDE_OTHER_TYPE>&  original,
                       bslma_Allocator                        *basicAllocator);
        // Create a nullable object that is initially null if the specified
        // 'original' object is null, and otherwise has, as the value of its
        // underlying 'ValueType, a value converted from the value (of
        // parameterized 'BDE_OTHER_TYPE') of 'original', and that uses the
        // specified 'basicAllocator' to supply memory.  Note that attempts
        // to call this constructor for an underlying 'ValueType' that does
        // not take an optional allocator at construction will fail to
        // compile as will attempts at coverting objects of incompatible
        // underlying types.

    // ~bdeut_NullableValue();
        // Destroy this nullable object.  Note that this destructor is
        // generated by the compiler.

    // MANIPULATORS
    bdeut_NullableValue<TYPE>& operator=(const bdeut_NullableValue& rhs);
        // Assign to this nullable object a null value if the specified 'rhs'
        // object is null, and otherwise the (non-null) value (of parameterized
        // 'TYPE') of 'rhs'.  Return a reference to this modifiable object.

    template <typename BDE_OTHER_TYPE>
    bdeut_NullableValue<TYPE>&
                     operator=(const bdeut_NullableValue<BDE_OTHER_TYPE>& rhs);
        // Assign to this nullable object a null value if the specified 'rhs'
        // object is null, and otherwise the value of parameterized 'TYPE'
        // converted from the (non-null) value (of parameterized
        // 'BDE_OTHER_TYPE') of 'rhs'.  Return a reference to this modifiable
        // nullable object.  Note that attempts to invoke this operator for
        // objects of incompatible underlying types will fail to compile.

    bdeut_NullableValue<TYPE>& operator=(const TYPE& rhs);
        // Assign to this nullable object the (non-null) value of the specified
        // 'rhs' object of parameterized 'TYPE'.  Return a reference to this
        // modifiable nullable object.

    template <typename BDE_OTHER_TYPE>
    bdeut_NullableValue<TYPE>& operator=(const BDE_OTHER_TYPE& rhs);
        // Assign to this nullable object the *(non-null) value of
        // parameterized 'TYPE' converted from the value of the specified
        // 'rhs' object of parameterized 'BDE_OTHER_TYPE'.  Return a reference
        // to this modifiable nullable object.  Note that attempts to invoke
        // this operator for a 'BDE_OTHER_TYPE' that is incompatible with the
        // underlying 'TYPE' will fail to compile.

    void swap(bdeut_NullableValue& other);
        // Swap the value of this object with the value of the specified
        // 'other' object.  This method provides the no-throw guarantee if the
        // 'TYPE' template parameter has a no-throw 'swap' and the result of
        // the 'isNull' method for the two objects being swapped is the same.
        // The behavior is undefined if the objects have non-equal allocators
        // for a 'TYPE' that requires an allocator.

    TYPE& makeValue(const TYPE& value);
        // Assign to this nullable object the specified (non-null) 'value' of
        // parameterized 'TYPE'.  Return a reference to the modifiable
        // 'ValueType' of this nullable object.

    template <typename BDE_OTHER_TYPE>
    TYPE& makeValue(const BDE_OTHER_TYPE& value);
        // Assign to this nullable object the value of parameterized 'TYPE'
        // converted from the specified 'value' of parameterized
        // 'BDE_OTHER_TYPE'.  Return a reference to the modifiable 'ValueType'
        // of this nullable object.   Note that attempts to invoke this method
        // for a 'BDE_OTHER_TYPE' that is incompatible with the underlying
        // 'TYPE' will fail to compile.

    TYPE& makeValue();
        // Assign to this nullable object the default value for the
        // parameterized 'TYPE'.  Return a reference to the modifiable
        // 'ValueType' of this object.  Note that, after invoking this
        // function, this object will not be null.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this nullable object the value read from the specified
        // input 'stream' using the specified 'version' format, and return a
        // reference to the modifiable 'stream'.  If 'stream' is initially
        // invalid, this operation has no effect.  If 'stream' becomes invalid
        // during this operation, this object is valid, but its value is
        // undefined.  If 'version' is not supported, 'stream' is marked
        // invalid, but this object is unaltered.  Note that no version is
        // read from 'stream'.  (See the 'bdex' package-level documentation
        // for more information on 'bdex' streaming of value-semantic types
        // and containers.)

    void reset();
        // Make this nullable object null.  Note that attempts to refer to
        // the underlying 'ValueType' of parameterized 'TYPE' until it is
        // reestablished will result in undefined behavior.

    TYPE& value();
        // Return a reference to the underlying modifiable 'ValueType' of this
        // object.  The behavior is undefined if this object is null.

    // ACCESSORS
    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this nullable object to the specified output
        // 'stream' using the specified 'version' format, and return a
        // reference to the modifiable 'stream'.  If 'version' is not
        // supported, 'stream' is unmodified.  Note that 'version' is not
        // written to 'stream'.  (See the 'bdex' package-level documentation
        // for more information on 'bdex' streaming of value-semantic types
        // and containers.)

    bool isNull() const;
        // Return 'true' if this object is null, and 'false' otherwise.

    int maxSupportedBdexVersion() const;
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.)

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
        // Return a reference to the underlying non-modifiable 'ValueType'
        // of this object.  The behavior is undefined if this object is null.
};

// FREE OPERATORS
template <typename LHS_TYPE, typename RHS_TYPE>
bool operator==(const bdeut_NullableValue<LHS_TYPE>& lhs,
                const bdeut_NullableValue<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two nullable objects have the same value
    // if they are either both null or neither is null and their respective
    // underlying types are equality comparable and compare equal.

template <typename LHS_TYPE, typename RHS_TYPE>
bool operator!=(const bdeut_NullableValue<LHS_TYPE>& lhs,
                const bdeut_NullableValue<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two nullable objects do not have the
    // same value if one, but not both, are null, or neither is null and their
    // respective underlying type values are equality comparable and do not
    // compare equal.

template <typename TYPE>
bsl::ostream& operator<<(bsl::ostream&                    stream,
                         const bdeut_NullableValue<TYPE>& rhs);
    // Print the specified 'rhs' to the specified 'stream' in some
    // human-readable, single-line format, and return a reference to the
    // modifiable 'stream'.

// FREE FUNCTIONS
template <typename TYPE>
void swap(bdeut_NullableValue<TYPE>& a, bdeut_NullableValue<TYPE>& b);
    // Swap the values of the specified 'a' and 'b' objects.  This method
    // provides the no-throw guarantee if the 'TYPE' template parameter has a
    // no-throw 'swap' and the result of the 'isNull' method for the two
    // objects being swapped is the same.  The behavior is undefined if the
    // objects have non-equal allocators for a 'TYPE' that requires an
    // allocator.

// ----------------------------------------------------------------------------
// ---  Anything below this line is implementation specific.  Do not use.  ----
// ----------------------------------------------------------------------------

                   // =======================================
                   // struct bdeut_NullableValue_Traits<TYPE>
                   // =======================================

// TRAITS

// TBD: Meta-functions for passthrough traits are duplicated here, because
// TBD: passthrough traits are not available in BB branch.  When it becomes
// TBD: available, remove them from here.

template <typename TRAIT> struct bdeut_NullableValue_NotTrait
{
    // Private class: Given a trait, this template produces a unique type
    // which is NOT the trait type.
};

template <typename T, typename TRAIT>
struct bdeut_NullableValue_PassthroughTraitImp
{
    // Private implementation of bslalg_PassthroughTrait class.

    enum { HAS_TRAIT = (int) bslalg_HasTrait<T, TRAIT>::VALUE };

    typedef typename bslmf_If<HAS_TRAIT,
                              TRAIT,
                              bdeut_NullableValue_NotTrait<TRAIT> >::Type Type;
};

template <typename T, typename TRAIT>
struct bdeut_NullableValue_PassthroughTrait
                      : bdeut_NullableValue_PassthroughTraitImp<T, TRAIT>::Type
{
    // If 'T' has 'TRAIT', then evaluate to 'TRAIT', else evaluate to a unique
    // class that is not 'TRAIT'.  Users of this meta-function do not need to
    // expand the result with '::Type' (though they may).

    typedef typename bdeut_NullableValue_PassthroughTraitImp<T,
                                                             TRAIT>::Type Type;
};

template <typename TYPE>
struct bdeut_NullableValue_Traits
    : bdeut_NullableValue_PassthroughTrait<TYPE,
                                           bslalg_TypeTraitUsesBslmaAllocator>,
      bdeut_NullableValue_PassthroughTrait<TYPE,
                                           bslalg_TypeTraitBitwiseMoveable>,
      bdeut_NullableValue_PassthroughTrait<TYPE,
                                           bslalg_TypeTraitBitwiseCopyable>,
      bdeu_TypeTraitHasPrintMethod {
    // TBD: doc
};

               // =============================================
               // class bdeut_NullableValue_WithAllocator<TYPE>
               // =============================================

template <typename TYPE>
class bdeut_NullableValue_WithAllocator {
    // This class provides the implementation for a nullable object that
    // augments a type that DOES take an optional 'bslma_Allocator'.

    // DATA
    bsls_ObjectBuffer<TYPE>  d_buffer;
    bool                     d_isNull;
    bslma_Allocator         *d_allocator_p;

  public:
    // CREATORS
    explicit bdeut_NullableValue_WithAllocator(
                                          bslma_Allocator *basicAllocator = 0);
        // Create an implementation for a nullable object that is initially
        // null.  Optionally specify a 'basicAllocator' used to supply memory.
        // If 'basicAllocator' is 0, the currently installed default allocator
        // is used.

    bdeut_NullableValue_WithAllocator(
                 const bdeut_NullableValue_WithAllocator&  original,
                 bslma_Allocator                          *basicAllocator = 0);
        // Create an implementation for a nullable object that contains the
        // the same value as the specified 'original' object.  I.e., if
        // 'original' is null, this object will initially be null; otherwise,
        // this object will have the same value (of parameterized 'TYPE') as
        // that of 'original'.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~bdeut_NullableValue_WithAllocator();
        // Destroy this object.

    // MANIPULATORS
    bdeut_NullableValue_WithAllocator& operator=(
                                 const bdeut_NullableValue_WithAllocator& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference to this modifiable object.  If 'rhs' is null,
        // this object will be set to null; otherwise, this object will be set
        // to have the same value (of parameterized 'TYPE') as that of 'rhs'.

    void swap(bdeut_NullableValue_WithAllocator& other);
        // Swap the value of this object with the value of the specified
        // 'other' object.  This method provides the no-throw guarantee if the
        // 'TYPE' template parameter has a no-throw 'swap' and the result of
        // the 'isNull' method for the two objects being swapped is the same.
        // The behavior is undefined if the objects have non-equal allocators.

    void makeValue(const TYPE& value);
        // Set the value of this object to be that of the specified 'value' of
        // parameterized 'TYPE'.

    template <typename BDE_OTHER_TYPE>
    void makeValue(const BDE_OTHER_TYPE& value);
        // Assign to this nullable object the value of parameterized 'TYPE'
        // converted from the specified 'value' of parameterized
        // 'BDE_OTHER_TYPE'.  Note that attempts to invoke this method for a
        // 'BDE_OTHER_TYPE' that is incompatible with the underlying 'TYPE'
        // will fail to compile.

    void makeValue();
        // Set the value of this object to be the default value for the
        // parameterized 'TYPE'.

    void reset();
        // Set this object to have a null value.

    TYPE& value();
        // Return a reference to the modifiable underlying value of
        // parameterized 'TYPE'.  The behavior is undefined if this object
        // is null.

    // ACCESSORS
    bool isNull() const;
        // Return 'true' if this object is null, and 'false' otherwise.

    const TYPE& value() const;
        // Return a reference to the non-modifiable underlying value of
        // parameterized 'TYPE'.  The behavior is undefined if this object
        // is null.
};

              // ================================================
              // class bdeut_NullableValue_WithoutAllocator<TYPE>
              // ================================================

template <typename TYPE>
class bdeut_NullableValue_WithoutAllocator {
    // This class provides the implementation for a nullable object that
    // augments a type that does NOT take an optional 'bslma_Allocator'.

    // DATA
    bsls_ObjectBuffer<TYPE> d_buffer;
    bool                    d_isNull;

  public:
    // CREATORS
    bdeut_NullableValue_WithoutAllocator();
        // Create an implementation for a nullable object that is initially
        // null.

    bdeut_NullableValue_WithoutAllocator(
                         const bdeut_NullableValue_WithoutAllocator& original);
        // Create an implementation for a nullable object that contains the
        // the same value as the specified 'original' object.  I.e., if
        // 'original' is null, this object will initially be null; otherwise,
        // this object will have the same value (of parameterized 'TYPE') as
        // that of 'original'.

    ~bdeut_NullableValue_WithoutAllocator();
        // Destroy this object.

    // MANIPULATORS
    bdeut_NullableValue_WithoutAllocator& operator=(
                              const bdeut_NullableValue_WithoutAllocator& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference to this modifiable object.  If 'rhs' is null,
        // this object will be set to null; otherwise, this object will be set
        // to have the same value (of parameterized 'TYPE') as that of 'rhs'.

    void swap(bdeut_NullableValue_WithoutAllocator& other);
        // Swap the value of this object with the value of the specified
        // 'other' object.  This method provides the no-throw guarantee if the
        // 'TYPE' template parameter has a no-throw 'swap' and the result of
        // the 'isNull' method for the two objects being swapped is the same.

    void makeValue(const TYPE& value);
        // Set the value of this object to be that of the specified 'value' of
        // parameterized 'TYPE'.

    template <typename BDE_OTHER_TYPE>
    void makeValue(const BDE_OTHER_TYPE& value);
        // Assign to this nullable object the value of parameterized 'TYPE'
        // converted from the specified 'value' of parameterized
        // 'BDE_OTHER_TYPE'.  Note that attempts to invoke this method for a
        // 'BDE_OTHER_TYPE' that is incompatible with the underlying 'TYPE'
        // will fail to compile.

    void makeValue();
        // Set the value of this object to be the default value for the
        // parameterized 'TYPE'.

    void reset();
        // Set this object to have a null value.

    TYPE& value();
        // Return a reference to the modifiable underlying value of
        // parameterized 'TYPE'.  The behavior is undefined if this object
        // is null.

    // ACCESSORS
    bool isNull() const;
        // Return 'true' if this object is null, and 'false' otherwise.

    const TYPE& value() const;
        // Return a reference to the non-modifiable underlying value of
        // parameterized 'TYPE'.  The behavior is undefined if this object
        // is null.
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                      // -------------------------------
                      // class bdeut_NullableValue<TYPE>
                      // -------------------------------

// CREATORS
template <typename TYPE>
inline
bdeut_NullableValue<TYPE>::bdeut_NullableValue()
{
}

template <typename TYPE>
inline
bdeut_NullableValue<TYPE>::bdeut_NullableValue(bslma_Allocator *basicAllocator)
: d_imp(basicAllocator)
{
}

template <typename TYPE>
inline
bdeut_NullableValue<TYPE>::bdeut_NullableValue(
                                           const bdeut_NullableValue& original)
: d_imp(original.d_imp)
{
}

template <typename TYPE>
inline
bdeut_NullableValue<TYPE>::bdeut_NullableValue(
                                    const bdeut_NullableValue&  original,
                                    bslma_Allocator            *basicAllocator)
: d_imp(original.d_imp, basicAllocator)
{
}

template <typename TYPE>
inline
bdeut_NullableValue<TYPE>::bdeut_NullableValue(const TYPE& value)
{
    d_imp.makeValue(value);
}

template <typename TYPE>
inline
bdeut_NullableValue<TYPE>::bdeut_NullableValue(const TYPE&      value,
                                               bslma_Allocator *basicAllocator)
: d_imp(basicAllocator)
{
    d_imp.makeValue(value);
}

template <typename TYPE>
template <typename BDE_OTHER_TYPE>
inline
bdeut_NullableValue<TYPE>::bdeut_NullableValue(
                           const bdeut_NullableValue<BDE_OTHER_TYPE>& original)
{
    if (!original.isNull()) {
        d_imp.makeValue(original.value());
    }
}

template <typename TYPE>
template <typename BDE_OTHER_TYPE>
inline
bdeut_NullableValue<TYPE>::bdeut_NullableValue(
                        const bdeut_NullableValue<BDE_OTHER_TYPE>&  original,
                        bslma_Allocator                        *basicAllocator)
: d_imp(basicAllocator)
{
    if (!original.isNull()) {
        d_imp.makeValue(original.value());
    }
}

// MANIPULATORS
template <typename TYPE>
inline
bdeut_NullableValue<TYPE>&
bdeut_NullableValue<TYPE>::operator=(const bdeut_NullableValue& rhs)
{
    d_imp = rhs.d_imp;

    return *this;
}

template <typename TYPE>
template <typename BDE_OTHER_TYPE>
bdeut_NullableValue<TYPE>&
bdeut_NullableValue<TYPE>::operator=(
                                const bdeut_NullableValue<BDE_OTHER_TYPE>& rhs)
{
    if (rhs.isNull()) {
        d_imp.reset();
    }
    else {
        d_imp.makeValue(rhs.value());
    }
    return *this;
}

template <typename TYPE>
inline
bdeut_NullableValue<TYPE>&
bdeut_NullableValue<TYPE>::operator=(const TYPE& rhs)
{
    d_imp.makeValue(rhs);

    return *this;
}

template <typename TYPE>
template <typename BDE_OTHER_TYPE>
inline
bdeut_NullableValue<TYPE>&
bdeut_NullableValue<TYPE>::operator=(const BDE_OTHER_TYPE& rhs)
{
    d_imp.makeValue(rhs);

    return *this;
}

template <typename TYPE>
inline
void bdeut_NullableValue<TYPE>::swap(bdeut_NullableValue<TYPE>& other)
{
    d_imp.swap(other.d_imp);
}

template <typename TYPE>
inline
TYPE& bdeut_NullableValue<TYPE>::makeValue(const TYPE& value)
{
    d_imp.makeValue(value);

    return d_imp.value();
}

template <typename TYPE>
template <typename BDE_OTHER_TYPE>
inline
TYPE& bdeut_NullableValue<TYPE>::makeValue(const BDE_OTHER_TYPE& value)
{
    d_imp.makeValue(value);

    return d_imp.value();
}

template <typename TYPE>
inline
TYPE& bdeut_NullableValue<TYPE>::makeValue()
{
    d_imp.makeValue();

    return d_imp.value();
}

template <typename TYPE>
template <class STREAM>
STREAM& bdeut_NullableValue<TYPE>::bdexStreamIn(STREAM& stream, int version)
{
    char isNull;

    stream.getInt8(isNull);

    if (stream) {
        if (!isNull) {
            d_imp.makeValue();

            bdex_InStreamFunctions::streamIn(stream, d_imp.value(), version);
        }
        else {
            d_imp.reset();
        }
    }

    return stream;
}

template <typename TYPE>
inline
void bdeut_NullableValue<TYPE>::reset()
{
    d_imp.reset();
}

template <typename TYPE>
inline
TYPE& bdeut_NullableValue<TYPE>::value()
{
    return d_imp.value();
}

// ACCESSORS
template <typename TYPE>
template <class STREAM>
STREAM& bdeut_NullableValue<TYPE>::bdexStreamOut(STREAM& stream,
                                                 int     version) const
{
    const bool isNull = d_imp.isNull();

    stream.putInt8(isNull ? 1 : 0);

    if (!isNull) {
        bdex_OutStreamFunctions::streamOut(stream, d_imp.value(), version);
    }

    return stream;
}

template <typename TYPE>
inline
bool bdeut_NullableValue<TYPE>::isNull() const
{
    return d_imp.isNull();
}

template <typename TYPE>
inline
int bdeut_NullableValue<TYPE>::maxSupportedBdexVersion() const
{
    return bdex_VersionFunctions::maxSupportedVersion(d_imp.value());
}

template <typename TYPE>
bsl::ostream& bdeut_NullableValue<TYPE>::print(
                                            bsl::ostream& stream,
                                            int           level,
                                            int           spacesPerLevel) const
{
    if (d_imp.isNull()) {
        return bdeu_PrintMethods::print(stream, "NULL", level, spacesPerLevel);
    }

    return bdeu_PrintMethods::print(stream,
                                    d_imp.value(),
                                    level,
                                    spacesPerLevel);
}

template <typename TYPE>
inline
const TYPE& bdeut_NullableValue<TYPE>::value() const
{
    return d_imp.value();
}

// FREE OPERATORS

template <typename LHS_TYPE, typename RHS_TYPE>
inline
bool operator==(const bdeut_NullableValue<LHS_TYPE>& lhs,
                const bdeut_NullableValue<RHS_TYPE>& rhs)
{
    if (!lhs.isNull() && !rhs.isNull()) {
        return lhs.value() == rhs.value();
    }

    return lhs.isNull() == rhs.isNull();
}

template <typename LHS_TYPE, typename RHS_TYPE>
inline
bool operator!=(const bdeut_NullableValue<LHS_TYPE>& lhs,
                const bdeut_NullableValue<RHS_TYPE>& rhs)
{
    if (!lhs.isNull() && !rhs.isNull()) {
        return lhs.value() != rhs.value();
    }

    return lhs.isNull() != rhs.isNull();
}

template <typename TYPE>
inline
bsl::ostream& operator<<(bsl::ostream&                    stream,
                         const bdeut_NullableValue<TYPE>& rhs)
{
    return rhs.print(stream, 0, -1);
}

// FREE FUNCTIONS
template <typename TYPE>
inline
void swap(bdeut_NullableValue<TYPE>& a, bdeut_NullableValue<TYPE>& b)
{
    a.swap(b);
}

               // ---------------------------------------------
               // class bdeut_NullableValue_WithAllocator<TYPE>
               // ---------------------------------------------

// CREATORS
template <typename TYPE>
inline
bdeut_NullableValue_WithAllocator<TYPE>::bdeut_NullableValue_WithAllocator(
                                               bslma_Allocator *basicAllocator)
: d_isNull(true)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

template <typename TYPE>
inline
bdeut_NullableValue_WithAllocator<TYPE>::bdeut_NullableValue_WithAllocator(
                      const bdeut_NullableValue_WithAllocator&  original,
                      bslma_Allocator                          *basicAllocator)
: d_isNull(true)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    if (!original.isNull()) {
        makeValue(original.value());
    }
}

template <typename TYPE>
inline
bdeut_NullableValue_WithAllocator<TYPE>::~bdeut_NullableValue_WithAllocator()
{
    reset();
}

// MANIPULATORS
template <typename TYPE>
bdeut_NullableValue_WithAllocator<TYPE>&
bdeut_NullableValue_WithAllocator<TYPE>::operator=(
                                  const bdeut_NullableValue_WithAllocator& rhs)
{
    if (!rhs.isNull()) {
        makeValue(rhs.value());
    }
    else {
        reset();
    }

    return *this;
}

template <typename TYPE>
void bdeut_NullableValue_WithAllocator<TYPE>::swap(
                                      bdeut_NullableValue_WithAllocator& other)
{
    // 'swap' is undefined for non-equal allocators.

    BSLS_ASSERT(d_allocator_p == other.d_allocator_p);

    // same 'isNull' flags

    if (isNull() && other.isNull()) {
        return;                                                       // RETURN
    }

    if (!isNull() && !other.isNull()) {
        // swap typed values
        bslalg_SwapUtil::swap(&this->value(), &other.value());
        return;                                                       // RETURN
    }

    // different 'isNull' flags

    bdeut_NullableValue_WithAllocator *nullObj;
    bdeut_NullableValue_WithAllocator *nonNullObj;

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

template <typename TYPE>
void bdeut_NullableValue_WithAllocator<TYPE>::makeValue(const TYPE& value)
{
    if (d_isNull) {
        new (d_buffer.buffer()) TYPE(value, d_allocator_p);
        d_isNull = false;
    }
    else {
        d_buffer.object() = value;
    }
}

template <typename TYPE>
template <typename BDE_OTHER_TYPE>
void bdeut_NullableValue_WithAllocator<TYPE>::makeValue(
                                                   const BDE_OTHER_TYPE& value)
{
    if (d_isNull) {
        new (d_buffer.buffer()) TYPE(value, d_allocator_p);
        d_isNull = false;
    }
    else {
        d_buffer.object() = value;
    }
}

template <typename TYPE>
inline
void bdeut_NullableValue_WithAllocator<TYPE>::makeValue()
{
    reset();

    new (d_buffer.buffer()) TYPE(d_allocator_p);
    d_isNull = false;

    // Note that this alternative implementation provides stronger
    // exception-safety, but it breaks some client code that uses
    // bdeut_NullableValue with a non-value-semantic TYPE.
    //..
    // if (d_isNull) {
    //     new (d_buffer.buffer()) TYPE(d_allocator_p);
    //     d_isNull = false;
    // }
    // else {
    //     d_buffer.object() = TYPE(d_allocator_p);
    // }
    //..
}

template <typename TYPE>
inline
void bdeut_NullableValue_WithAllocator<TYPE>::reset()
{
    if (!d_isNull) {
        d_buffer.object().~TYPE();
        d_isNull = true;
    }
}

template <typename TYPE>
inline
TYPE& bdeut_NullableValue_WithAllocator<TYPE>::value()
{
    BSLS_ASSERT_SAFE(!d_isNull);

    return d_buffer.object();
}

// ACCESSORS
template <typename TYPE>
inline
bool bdeut_NullableValue_WithAllocator<TYPE>::isNull() const
{
    return d_isNull;
}

template <typename TYPE>
inline
const TYPE& bdeut_NullableValue_WithAllocator<TYPE>::value() const
{
    BSLS_ASSERT_SAFE(!d_isNull);

    return d_buffer.object();
}

              // ------------------------------------------------
              // class bdeut_NullableValue_WithoutAllocator<TYPE>
              // ------------------------------------------------

// CREATORS
template <typename TYPE>
inline
bdeut_NullableValue_WithoutAllocator<TYPE>
                                       ::bdeut_NullableValue_WithoutAllocator()
: d_isNull(true)
{
}

template <typename TYPE>
inline
bdeut_NullableValue_WithoutAllocator<TYPE>::
bdeut_NullableValue_WithoutAllocator(
                          const bdeut_NullableValue_WithoutAllocator& original)
: d_isNull(true)
{
    if (!original.isNull()) {
        makeValue(original.value());
    }
}

template <typename TYPE>
inline
bdeut_NullableValue_WithoutAllocator<TYPE>
                                      ::~bdeut_NullableValue_WithoutAllocator()
{
    reset();
}

// MANIPULATORS
template <typename TYPE>
bdeut_NullableValue_WithoutAllocator<TYPE>&
bdeut_NullableValue_WithoutAllocator<TYPE>::operator=(
                               const bdeut_NullableValue_WithoutAllocator& rhs)
{
    if (!rhs.isNull()) {
        makeValue(rhs.value());
    }
    else {
        reset();
    }

    return *this;
}

template <typename TYPE>
void bdeut_NullableValue_WithoutAllocator<TYPE>::swap(
                                   bdeut_NullableValue_WithoutAllocator& other)
{
    // same 'isNull' flags

    if (isNull() && other.isNull()) {
        return;                                                       // RETURN
    }

    if (!isNull() && !other.isNull()) {
        // swap typed values
        bslalg_SwapUtil::swap(&this->value(), &other.value());
        return;                                                       // RETURN
    }

    // different 'isNull' flags

    bdeut_NullableValue_WithoutAllocator *nullObj;
    bdeut_NullableValue_WithoutAllocator *nonNullObj;

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

template <typename TYPE>
void bdeut_NullableValue_WithoutAllocator<TYPE>::makeValue(const TYPE& value)
{
    if (d_isNull) {
        new (d_buffer.buffer()) TYPE(value);
        d_isNull = false;
    }
    else {
        d_buffer.object() = value;
    }
}

template <typename TYPE>
template <typename BDE_OTHER_TYPE>
void bdeut_NullableValue_WithoutAllocator<TYPE>::makeValue(
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

template <typename TYPE>
inline
void bdeut_NullableValue_WithoutAllocator<TYPE>::makeValue()
{
    reset();

    new (d_buffer.buffer()) TYPE();
    d_isNull = false;

    // Note that this alternative implementation provides stronger
    // exception-safety, but it breaks some client code that uses
    // bdeut_NullableValue with a non-value-semantic TYPE.
    //..
    // if (d_isNull) {
    //     new (d_buffer.buffer()) TYPE();
    //     d_isNull = false;
    // }
    // else {
    //     d_buffer.object() = TYPE();
    // }
}

template <typename TYPE>
inline
void bdeut_NullableValue_WithoutAllocator<TYPE>::reset()
{
    if (!d_isNull) {
        d_buffer.object().~TYPE();
        d_isNull = true;
    }
}

template <typename TYPE>
inline
TYPE& bdeut_NullableValue_WithoutAllocator<TYPE>::value()
{
    BSLS_ASSERT_SAFE(!d_isNull);

    return d_buffer.object();
}

// ACCESSORS
template <typename TYPE>
inline
bool bdeut_NullableValue_WithoutAllocator<TYPE>::isNull() const
{
    return d_isNull;
}

template <typename TYPE>
inline
const TYPE& bdeut_NullableValue_WithoutAllocator<TYPE>::value() const
{
    // TBD
    // The assert below was commented out because a call to this function is
    // sometimes used as an argument to a template function that only looks at
    // the value type (and does not access the value).

    // BSLS_ASSERT_SAFE(!d_isNull);

    return d_buffer.object();
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
