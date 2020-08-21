// bdlb_nullablevalue.h                                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLB_NULLABLEVALUE
#define INCLUDED_BDLB_NULLABLEVALUE

#include <bsls_ident.h>
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
// same value if their underlying (non-null) 'TYPE' values are the same, or
// both are null.
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
// Furthermore, a move constructor (taking an optional allocator) and a
// move-assignment operator are also provided.  Note that move semantics are
// emulated with C++03 compilers.
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

#include <bdlscm_version.h>

#include <bdlb_nullopt.h>
#include <bdlb_printmethods.h>

#include <bslalg_swaputil.h>

#include <bslma_constructionutil.h>
#include <bslma_stdallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_conditional.h>
#include <bslmf_enableif.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_isconvertible.h>
#include <bslmf_istriviallycopyable.h>
#include <bslmf_movableref.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_deprecate.h>
#include <bsls_keyword.h>
#include <bsls_objectbuffer.h>

#include <bslx_instreamfunctions.h>
#include <bslx_outstreamfunctions.h>
#include <bslx_versionfunctions.h>

#include <bsl_algorithm.h>
#include <bsl_iosfwd.h>
#include <bsl_new.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslalg_typetraits.h>
#include <bslmf_if.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

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
    // both objects are null or (2) the non-null values compare equal.  A null
    // nullable object is considered ordered before any non-null nullable
    // object.  Attempts to copy construct, copy assign, or compare
    // incompatible values types will fail to compile.  The 'NullableValue'
    // template cannot be instantiated on an incomplete type, a type that
    // overloads unary 'operator&', or 'bdlb::NullOptType'.

    // PRIVATE TYPES
    typedef typename
    bsl::conditional<bslma::UsesBslmaAllocator<TYPE>::value,
                     NullableValue_WithAllocator<TYPE>,
                     NullableValue_WithoutAllocator<TYPE> >::type Imp;

    typedef bslmf::MovableRefUtil                                 MoveUtil;

    struct EnableType { };

    // DATA
    Imp d_imp;  // managed nullable 'TYPE' object

    // FRIENDS
    template <class FRIEND_TYPE>
    friend void swap(NullableValue<FRIEND_TYPE>&, NullableValue<FRIEND_TYPE>&);

  public:
    // TYPES
    typedef TYPE ValueType;
        // 'ValueType' is an alias for the underlying 'TYPE' upon which this
        // template class is instantiated, and represents the type of the
        // managed object.

    typedef typename Imp::AllocType allocator_type;
        // The type of allocator used by this object.  If 'TYPE' is not
        // allocator aware, 'allocator_type' is a private non-allocator type
        // that effectively removes the allocator-specific constructors from
        // consideration during overload resolution.

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
    NullableValue() BSLS_KEYWORD_NOEXCEPT;
        // Create a nullable object having the null value.  If 'TYPE' takes an
        // optional allocator at construction, use the currently installed
        // default allocator to supply memory.

    explicit NullableValue(const allocator_type& allocator)
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Create a nullable object that has the null value and that uses the
        // specified 'allocator' (e.g., the address of a 'bslma::Allocator'
        // object) to supply memory.  Note that this constructor will not
        // participate in overload resolution unless 'TYPE' is allocator aware.

    NullableValue(const NullableValue& original);
        // Create a nullable object having the value of the specified
        // 'original' object.  If 'TYPE' takes an optional allocator at
        // construction, use the currently installed default allocator to
        // supply memory.

    NullableValue(bslmf::MovableRef<NullableValue> original);
        // Create a nullable object having the same value as the specified
        // 'original' object by moving the contents of 'original' to the
        // newly-created object.  If 'TYPE' takes an optional allocator at
        // construction, the allocator associated with 'original' is propagated
        // for use in the newly-created object.  'original' is left in a valid
        // but unspecified state.

    NullableValue(const NullableValue&  original,
                  const allocator_type& allocator);
        // Create a nullable object that has the value of the specified
        // 'original' object and uses the specified 'allocator' (e.g., the
        // address of a 'bslma::Allocator' object) to supply memory.  Note that
        // this constructor will not participate in overload resolution unless
        // 'TYPE' is allocator aware.

    NullableValue(bslmf::MovableRef<NullableValue>  original,
                  const allocator_type&             allocator);
        // Create a nullable object having the same value as the specified
        // 'original' object but using the specified 'allocator' (e.g., the
        // address of a 'bslma::Allocator' object) to supply memory.  The
        // contents of 'original' are moved to the newly-created object using
        // the extended move constructor for 'TYPE'.  'original' is left in a
        // valid but unspecified state.  Note that this constructor will not
        // participate in overload resolution unless 'TYPE' is allocator aware.

    template <class BDE_OTHER_TYPE>
    NullableValue(BSLS_COMPILERFEATURES_FORWARD_REF(BDE_OTHER_TYPE) value,
                  typename bsl::enable_if<
                      bsl::is_convertible<BDE_OTHER_TYPE, TYPE>::value &&
                      !bsl::is_convertible<BDE_OTHER_TYPE,
                                           allocator_type>::value,
                      EnableType>::type = EnableType());            // IMPLICIT
        // Create a nullable object having the specified 'value' (of
        // 'BDE_OTHER_TYPE') converted to 'TYPE'.  If 'TYPE' takes an optional
        // allocator at construction, use the currently installed default
        // allocator to supply memory.  Note that this constructor will not
        // participate in overload resolution unless 'BDE_OTHER_TYPE' is
        // convertible to 'TYPE' and is not convertible to 'allocator_type'.

    template <class BDE_OTHER_TYPE>
    NullableValue(
             BSLS_COMPILERFEATURES_FORWARD_REF(BDE_OTHER_TYPE) value,
             const allocator_type&                             allocator,
             typename bsl::enable_if<
                 bsl::is_convertible<BDE_OTHER_TYPE, TYPE>::value,
                 EnableType>::type = EnableType());
        // Create a nullable object that has the specified 'value' (of
        // 'BDE_OTHER_TYPE') converted to 'TYPE' and that uses the specified
        // 'allocator' (e.g., the address of a 'bslma::Allocator' object) to
        // supply memory.  Note that this constructor will not participate in
        // overload resolution unless 'TYPE' is allocator aware and
        // 'BDE_OTHER_TYPE' is convertible to 'TYPE'.

    template <class BDE_OTHER_TYPE>
    explicit NullableValue(const NullableValue<BDE_OTHER_TYPE>& original);
        // Create a nullable object having the null value if the specified
        // 'original' object is null, and the value of 'original.value()' (of
        // 'BDE_OTHER_TYPE') converted to 'TYPE' otherwise.  If 'TYPE' takes an
        // optional allocator at construction, use the currently installed
        // default allocator to supply memory.  Note that this method will fail
        // to compile if 'TYPE and 'BDE_OTHER_TYPE' are not compatible.

    template <class BDE_OTHER_TYPE>
    NullableValue(const NullableValue<BDE_OTHER_TYPE>& original,
                  const allocator_type&                allocator);
        // Create a nullable object that has the null value if the specified
        // 'original' object is null, and the value of 'original.value()' (of
        // 'BDE_OTHER_TYPE') converted to 'TYPE' otherwise.  Use the specified
        // 'allocator' (e.g., the address of a 'bslma::Allocator' object) to
        // supply memory.  Note that this constructor will not partcipate in
        // overload resolution unless 'TYPE' is allocator aware.  Also note
        // that compilation will fail if this function is called with a
        // 'BDE_OTHER_TYPE' that is not convertible to 'TYPE'.

    NullableValue(const NullOptType&) BSLS_KEYWORD_NOEXCEPT;        // IMPLICIT
        // Create a nullable object having the null value.  If 'TYPE' takes an
        // optional allocator at construction, use the currently installed
        // default allocator to supply memory for subsequent values assigned to
        // this object.

    NullableValue(const NullOptType&    ,
                  const allocator_type& allocator) BSLS_KEYWORD_NOEXCEPT;
        // Create a nullable object that has the null value; use the specified
        // 'allocator' (e.g., the address of a 'bslma::Allocator' object) to
        // supply memory for subsequent values assigned to this object.  Note
        // that this constructor will not participate in overload resolution
        // unless 'TYPE' is allocator aware.

    // ~NullableValue();
        // Destroy this object.  Note that this destructor is generated by the
        // compiler.

    // MANIPULATORS
    NullableValue<TYPE>& operator=(const NullableValue& rhs);
        // Assign to this object the value of the specified 'rhs', and return a
        // reference providing modifiable access to this object.

    NullableValue<TYPE>& operator=(bslmf::MovableRef<NullableValue> rhs);
        // Assign to this object the value of the specified 'rhs', and return a
        // reference providing modifiable access to this object.  The contents
        // of 'rhs' are either move-inserted into or move-assigned to this
        // object.  'rhs' is left in a valid but unspecified state.

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

    NullableValue<TYPE>& operator=(bslmf::MovableRef<TYPE> rhs);
        // Assign to this object the value of the specified 'rhs', and return a
        // reference providing modifiable access to this object.  The contents
        // of 'rhs' are either move-inserted into or move-assigned to this
        // object.  'rhs' is left in a valid but unspecified state.

    template <class BDE_OTHER_TYPE>
    NullableValue<TYPE>& operator=(const BDE_OTHER_TYPE& rhs);
        // Assign to this object the value of the specified 'rhs' object (of
        // 'BDE_OTHER_TYPE') converted to 'TYPE', and return a reference
        // providing modifiable access to this object.  Note that this method
        // will fail to compile if 'TYPE and 'BDE_OTHER_TYPE' are not
        // compatible.

    NullableValue<TYPE>& operator=(const NullOptType&) BSLS_KEYWORD_NOEXCEPT;
        // Reset this object to the default constructed state (i.e., to have
        // the null value), and return a reference providing modifiable access
        // to this object.

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

    template <class BDE_OTHER_TYPE>
    TYPE& makeValue(BSLS_COMPILERFEATURES_FORWARD_REF(BDE_OTHER_TYPE) value);
        // Assign to this object the specified 'value' (of 'BDE_OTHER_TYPE')
        // converted to 'TYPE', and return a reference providing modifiable
        // access to the underlying 'TYPE' object.  Note that this method will
        // fail to compile if 'TYPE and 'BDE_OTHER_TYPE' are not compatible.

    TYPE& makeValue();
        // Assign to this object the default value for 'TYPE', and return a
        // reference providing modifiable access to the underlying 'TYPE'
        // object.

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=5
    template <class... ARGS>
    TYPE& makeValueInplace(ARGS&&... args);
        // Assign to this nullable object the value of the (template parameter)
        // 'TYPE' created in place using the specified 'args'.  Return a
        // reference providing modifiable access to the created (value) object.
        // The object is also accessible via the 'value' method.  If this
        // nullable object already contains an object ('false == isNull()'),
        // that object is destroyed before the new object is created.  If
        // 'TYPE' has the trait 'bslma::UsesBslmaAllocator' ('TYPE' is
        // allocator-enabled) the allocator specified at the construction of
        // this nullable object is used to supply memory to the value object.
        // Attempts to explicitly specify via 'args' another allocator to
        // supply memory to the created (value) object are disallowed by the
        // compiler.  Note that if the constructor of 'TYPE' throws an
        // exception this object is left in the null state.
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bdlb_nullablevalue.h
#ifndef BDLB_NULLABLEVALUE_VARIADIC_LIMIT
#define BDLB_NULLABLEVALUE_VARIADIC_LIMIT 5
#endif
#ifndef BDLB_NULLABLEVALUE_VARIADIC_LIMIT_A
#define BDLB_NULLABLEVALUE_VARIADIC_LIMIT_A BDLB_NULLABLEVALUE_VARIADIC_LIMIT
#endif
#if BDLB_NULLABLEVALUE_VARIADIC_LIMIT_A >= 0
    TYPE& makeValueInplace();
#endif  // BDLB_NULLABLEVALUE_VARIADIC_LIMIT_A >= 0

#if BDLB_NULLABLEVALUE_VARIADIC_LIMIT_A >= 1
    template <class ARGS_1>
    TYPE& makeValueInplace(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1);
#endif  // BDLB_NULLABLEVALUE_VARIADIC_LIMIT_A >= 1

#if BDLB_NULLABLEVALUE_VARIADIC_LIMIT_A >= 2
    template <class ARGS_1,
              class ARGS_2>
    TYPE& makeValueInplace(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_2) args_2);
#endif  // BDLB_NULLABLEVALUE_VARIADIC_LIMIT_A >= 2

#if BDLB_NULLABLEVALUE_VARIADIC_LIMIT_A >= 3
    template <class ARGS_1,
              class ARGS_2,
              class ARGS_3>
    TYPE& makeValueInplace(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_2) args_2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_3) args_3);
#endif  // BDLB_NULLABLEVALUE_VARIADIC_LIMIT_A >= 3

#if BDLB_NULLABLEVALUE_VARIADIC_LIMIT_A >= 4
    template <class ARGS_1,
              class ARGS_2,
              class ARGS_3,
              class ARGS_4>
    TYPE& makeValueInplace(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_2) args_2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_3) args_3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_4) args_4);
#endif  // BDLB_NULLABLEVALUE_VARIADIC_LIMIT_A >= 4

#if BDLB_NULLABLEVALUE_VARIADIC_LIMIT_A >= 5
    template <class ARGS_1,
              class ARGS_2,
              class ARGS_3,
              class ARGS_4,
              class ARGS_5>
    TYPE& makeValueInplace(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_2) args_2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_3) args_3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_4) args_4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_5) args_5);
#endif  // BDLB_NULLABLEVALUE_VARIADIC_LIMIT_A >= 5

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
    template <class... ARGS>
    TYPE& makeValueInplace(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args);
// }}} END GENERATED CODE
#endif

    void reset();
        // Reset this object to the default constructed state (i.e., to have
        // the null value).

    void swap(NullableValue& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee if the (template parameter) 'TYPE'
        // provides that guarantee and the result of the 'isNull' method for
        // the two objects being swapped is the same.  The behavior is
        // undefined unless this object was created with the same allocator as
        // 'other' (if applicable).

    TYPE& value();
        // Return a reference providing modifiable access to the underlying
        // 'TYPE' object.  The behavior is undefined unless this object is
        // non-null.

    // ACCESSORS
    const TYPE *addressOr(const TYPE *address) const;
        // Return an address providing non-modifiable access to the underlying
        // object of a (template parameter) 'TYPE' if this object is non-null,
        // and the specified 'address' otherwise.

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

    bool isNull() const BSLS_KEYWORD_NOEXCEPT;
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

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    int maxSupportedBdexVersion() const;
        // Return the most current BDEX streaming version number supported by
        // this class.  (See the 'bslx' package-level documentation for more
        // information on BDEX streaming of value-semantic types and
        // containers.)
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
        // Return a reference providing non-modifiable access to the underlying
        // object of a (template parameter) 'TYPE'.  The behavior is undefined
        // unless this object is non-null.

    TYPE valueOr(const TYPE& value) const;
        // Return the value of the underlying object of a (template parameter)
        // 'TYPE' if this object is non-null, and the specified 'value'
        // otherwise.  Note that this method returns *by* *value*, so may be
        // inefficient in some contexts.

    #if BSLS_DEPRECATE_IS_ACTIVE(BDL, 3, 5)
    BSLS_DEPRECATE
    #endif
    const TYPE *valueOr(const TYPE *value) const;
        // !DEPRECATED!: Use 'addressOr' instead.
        //
        // Return an address providing non-modifiable access to the underlying
        // object of a (template parameter) 'TYPE' if this object is non-null,
        // and the specified 'value' otherwise.

    const TYPE *valueOrNull() const;
        // Return an address providing non-modifiable access to the underlying
        // object of a (template parameter) 'TYPE' if this object is non-null,
        // and 0 otherwise.

    allocator_type get_allocator() const;
        // Return the allocator used by this object to supply memory.  Note
        // that compilation will fail if this method is called for a 'TYPE'
        // that is not allocator aware.
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

template <class LHS_TYPE, class RHS_TYPE>
bool operator!=(const NullableValue<LHS_TYPE>& lhs,
                const RHS_TYPE&                rhs);
template <class LHS_TYPE, class RHS_TYPE>
bool operator!=(const LHS_TYPE&                lhs,
                const NullableValue<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  A nullable object and a value of
    // some type do not have the same value if either the nullable object is
    // null, or its underlying value does not compare equal to the other value.
    // Note that this function will fail to compile if 'LHS_TYPE' and
    // 'RHS_TYPE' are not compatible.

template <class LHS_TYPE, class RHS_TYPE>
bool operator==(const NullableValue<LHS_TYPE>& lhs,
                const RHS_TYPE&                rhs);
template <class LHS_TYPE, class RHS_TYPE>
bool operator==(const LHS_TYPE&                lhs,
                const NullableValue<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  A nullable object and a value of some
    // type have the same value if the nullable object is non-null and its
    // underlying value compares equal to the other value.  Note that this
    // function will fail to compile if 'LHS_TYPE' and 'RHS_TYPE' are not
    // compatible.

template <class LHS_TYPE, class RHS_TYPE>
bool operator<(const NullableValue<LHS_TYPE>& lhs,
               const NullableValue<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' nullable object is ordered before
    // the specified 'rhs' nullable object, and 'false' otherwise.  'lhs' is
    // ordered before 'rhs' if 'lhs' is null and 'rhs' is non-null or if both
    // are non-null and 'lhs.value()' is ordered before 'rhs.value()'.  Note
    // that this function will fail to compile if 'LHS_TYPE' and 'RHS_TYPE' are
    // not compatible.

template <class LHS_TYPE, class RHS_TYPE>
bool operator<(const NullableValue<LHS_TYPE>& lhs,
               const RHS_TYPE&                rhs);
    // Return 'true' if the specified 'lhs' nullable object is ordered before
    // the specified 'rhs', and 'false' otherwise.  'lhs' is ordered before
    // 'rhs' if 'lhs' is null or 'lhs.value()' is ordered before 'rhs'.

template <class LHS_TYPE, class RHS_TYPE>
bool operator<(const LHS_TYPE&                lhs,
               const NullableValue<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' is ordered before the specified
    // 'rhs' nullable object, and 'false' otherwise.  'lhs' is ordered before
    // 'rhs' if 'rhs' is not null and 'lhs' is ordered before 'rhs.value()'.

template <class LHS_TYPE, class RHS_TYPE>
bool operator>(const NullableValue<LHS_TYPE>& lhs,
               const NullableValue<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' nullable object is ordered after
    // the specified 'rhs' nullable object, and 'false' otherwise.  'lhs' is
    // ordered after 'rhs' if 'lhs' is non-null and 'rhs' is null or if both
    // are non-null and 'lhs.value()' is ordered after 'rhs.value()'.  Note
    // that this operator returns 'rhs < lhs'.  Also note that this function
    // will fail to compile if 'LHS_TYPE' and 'RHS_TYPE' are not compatible.

template <class LHS_TYPE, class RHS_TYPE>
bool operator>(const NullableValue<LHS_TYPE>& lhs,
               const RHS_TYPE&                rhs);
    // Return 'true' if the specified 'lhs' nullable object is ordered after
    // the specified 'rhs', and 'false' otherwise.  'lhs' is ordered after
    // 'rhs' if 'lhs' is not null and 'lhs.value()' is ordered after 'rhs'.
    // Note that this operator returns 'rhs < lhs'.

template <class LHS_TYPE, class RHS_TYPE>
bool operator>(const LHS_TYPE&                lhs,
               const NullableValue<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' is ordered after the specified
    // 'rhs' nullable object, and 'false' otherwise.  'lhs' is ordered after
    // 'rhs' if 'rhs' is null or 'lhs' is ordered after 'rhs.value()'.  Note
    // that this operator returns 'rhs < lhs'.

template <class LHS_TYPE, class RHS_TYPE>
bool operator<=(const NullableValue<LHS_TYPE>& lhs,
                const NullableValue<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' nullable object is ordered before
    // the specified 'rhs' nullable object or 'lhs' and 'rhs' have the same
    // value, and 'false' otherwise.  (See 'operator<' and 'operator=='.)  Note
    // that this operator returns '!(rhs < lhs)'.  Also note that this function
    // will fail to compile if 'LHS_TYPE' and 'RHS_TYPE' are not compatible.

template <class LHS_TYPE, class RHS_TYPE>
bool operator<=(const NullableValue<LHS_TYPE>& lhs,
                const RHS_TYPE&                rhs);
    // Return 'true' if the specified 'lhs' nullable object is ordered before
    // the specified 'rhs' or 'lhs' and 'rhs' have the same value, and 'false'
    // otherwise.  (See 'operator<' and 'operator=='.)  Note that this operator
    // returns '!(rhs < lhs)'.

template <class LHS_TYPE, class RHS_TYPE>
bool operator<=(const LHS_TYPE&                lhs,
                const NullableValue<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' is ordered before the specified
    // 'rhs' nullable object or 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.  (See 'operator<' and 'operator=='.)  Note that this
    // operator returns '!(rhs < lhs)'.

template <class LHS_TYPE, class RHS_TYPE>
bool operator>=(const NullableValue<LHS_TYPE>& lhs,
                const NullableValue<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' nullable object is ordered after
    // the specified 'rhs' nullable object or 'lhs' and 'rhs' have the same
    // value, and 'false' otherwise.  (See 'operator>' and 'operator=='.)  Note
    // that this operator returns '!(lhs < rhs)'.  Also note that this function
    // will fail to compile if 'LHS_TYPE' and 'RHS_TYPE' are not compatible.

template <class LHS_TYPE, class RHS_TYPE>
bool operator>=(const NullableValue<LHS_TYPE>& lhs,
                const RHS_TYPE&                rhs);
    // Return 'true' if the specified 'lhs' nullable object is ordered after
    // the specified 'rhs' or 'lhs' and 'rhs' have the same value, and 'false'
    // otherwise.  (See 'operator>' and 'operator=='.)  Note that this operator
    // returns '!(lhs < rhs)'.

template <class LHS_TYPE, class RHS_TYPE>
bool operator>=(const LHS_TYPE&                lhs,
                const NullableValue<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' is ordered after the specified
    // 'rhs' nullable object or 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.  (See 'operator>' and 'operator=='.)  Note that this
    // operator returns '!(lhs < rhs)'.

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR
bool operator==(const NullableValue<TYPE>& value,
                const NullOptType&         ) BSLS_KEYWORD_NOEXCEPT;
template <class TYPE>
BSLS_KEYWORD_CONSTEXPR
bool operator==(const NullOptType&         ,
                const NullableValue<TYPE>& value) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if the specified 'value' is null, and 'false' otherwise.

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR
bool operator!=(const NullableValue<TYPE>& value,
                const NullOptType&         ) BSLS_KEYWORD_NOEXCEPT;
template <class TYPE>
BSLS_KEYWORD_CONSTEXPR
bool operator!=(const NullOptType&         ,
                const NullableValue<TYPE>& value) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if the specified 'value' is not null, and 'false'
    // otherwise.

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR
bool operator<(const NullableValue<TYPE>&,
               const NullOptType&        ) BSLS_KEYWORD_NOEXCEPT;
    // Return 'false'.  Note that 'bdlb::nullOpt' never orders before a
    // 'NullableValue'.

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR
bool operator<(const NullOptType&         ,
               const NullableValue<TYPE>& value) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if the specified 'value' is not null, and 'false'
    // otherwise.  Note that 'bdlb::nullOpt' sorts before any 'NullableValue'
    // that is not null.

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR
bool operator>(const NullableValue<TYPE>& value,
               const NullOptType&         ) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if the specified 'value' is not null, and 'false'
    // otherwise.

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR
bool operator>(const NullOptType&         ,
               const NullableValue<TYPE>& value) BSLS_KEYWORD_NOEXCEPT;
    // Return 'false'.  Note that 'bdlb::nullOpt' never orders after a
    // 'NullableValue'.

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR
bool operator<=(const NullableValue<TYPE>& value,
                const NullOptType&         ) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if the specified 'value' is null, and 'false'
    // otherwise.

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR
bool operator<=(const NullOptType&         ,
                const NullableValue<TYPE>& value) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true'.

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR
bool operator>=(const NullableValue<TYPE>& value,
                const NullOptType&         ) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true'.

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR
bool operator>=(const NullOptType&         ,
                const NullableValue<TYPE>& value) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if the specified 'value' is null, and 'false' otherwise.

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
template <class HASHALG, class TYPE>
void hashAppend(HASHALG& hashAlg, const NullableValue<TYPE>& input);
    // Pass the boolean value of whether the specified 'input' contains a value
    // to the specified 'hashAlg' hashing algorithm of (template parameter)
    // type 'HASHALG'.  If 'input' contains a value, additionally pass that
    // value to 'hashAlg'.

template <class TYPE>
void swap(NullableValue<TYPE>& a, NullableValue<TYPE>& b);
    // Exchange the values of the specified 'a' and 'b' objects.  This function
    // provides the no-throw exception-safety guarantee if the (template
    // parameter) 'TYPE' provides that guarantee, the two objects were created
    // with the same allocator (if applicable), and the result of the 'isNull'
    // method for the two objects is the same; otherwise this function provides
    // the basic guarantee.

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

    bsl::allocator<char>      d_allocator;

    // PRIVATE TYPES
    typedef bsl::allocator<char> AllocType;
        // This type is used as the allocator argument type in the
        // 'NullableValue' class.  The presence of this type and the fact that
        // it is publicly constructible enables constructor overloads taking
        // allocator arguments.

    typedef bslmf::MovableRefUtil MoveUtil;

    // FRIENDS
    friend class NullableValue<TYPE>;

    template <class FRIEND_TYPE>
    friend void swap(NullableValue_WithAllocator<FRIEND_TYPE>&,
                     NullableValue_WithAllocator<FRIEND_TYPE>&);

    // PRIVATE MANIPULATORS
    template <class BDE_OTHER_TYPE>
    void makeValueRaw(BSLS_COMPILERFEATURES_FORWARD_REF(BDE_OTHER_TYPE) value);
        // Assign to this object the specified 'value' (of 'BDE_OTHER_TYPE')
        // converted to 'TYPE'.  The behavior is undefined unless this object
        // is null.  Note that this method will fail to compile if 'TYPE and
        // 'BDE_OTHER_TYPE' are not compatible.

  public:
    // TYPES
    typedef AllocType allocator_type;

    // CREATORS
    explicit NullableValue_WithAllocator(
                                const allocator_type& allocator = AllocType());
        // Create a nullable object having the null value.  Optionally specify
        // an 'allocator' (e.g., the address of a 'bslma::Allocator' object) to
        // supply memory; otherwise, the default allocator is used.

    NullableValue_WithAllocator(
                   const NullableValue_WithAllocator& original,
                   const allocator_type&              allocator = AllocType());
        // Create a nullable object having the value of the specified
        // 'original' object.  Optionally specify an 'allocator' (e.g., the
        // address of a 'bslma::Allocator' object) to supply memory; otherwise,
        // the default allocator is used.

    NullableValue_WithAllocator(
                      bslmf::MovableRef<NullableValue_WithAllocator> original);
        // Create a nullable object having the same value as the specified
        // 'original' object by moving the contents of 'original' to the
        // newly-created object.  The allocator associated with 'original' is
        // propagated for use in the newly-created object.  'original' is left
        // in a valid but unspecified state.

    NullableValue_WithAllocator(
                     bslmf::MovableRef<NullableValue_WithAllocator> original,
                     const allocator_type&                          allocator);
        // Create a nullable object having the same value as the specified
        // 'original' object but using the specified 'allocator' (e.g., the
        // address of a 'bslma::Allocator' object) to supply memory.  The
        // contents of 'original' are moved to the newly-created object.
        // 'original' is left in a valid but unspecified state.

    ~NullableValue_WithAllocator();
        // Destroy this object.

    // MANIPULATORS
    NullableValue_WithAllocator& operator=(
                                       const NullableValue_WithAllocator& rhs);
        // Assign to this object the value of the specified 'rhs', and return a
        // reference providing modifiable access to this object.

    NullableValue_WithAllocator& operator=(
                           bslmf::MovableRef<NullableValue_WithAllocator> rhs);
        // Assign to this object the value of the specified 'rhs', and return a
        // reference providing modifiable access to this object.  The contents
        // of 'rhs' are either move-inserted into or move-assigned to this
        // object.  'rhs' is left in a valid but unspecified state.

    void swap(NullableValue_WithAllocator& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee if the (template parameter) 'TYPE'
        // provides that guarantee and the result of the 'isNull' method for
        // the two objects being swapped is the same.  The behavior is
        // undefined unless this object was created with the same allocator as
        // 'other'.

    template <class BDE_OTHER_TYPE>
    void makeValue(BSLS_COMPILERFEATURES_FORWARD_REF(BDE_OTHER_TYPE) value);
        // Assign to this object the specified 'value' (of 'BDE_OTHER_TYPE')
        // converted to 'TYPE'.  Note that this method will fail to compile if
        // 'TYPE and 'BDE_OTHER_TYPE' are not compatible.

    void makeValue();
        // Assign to this object the default value for 'TYPE'.

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... ARGS>
    void makeValueInplace(ARGS&&... args);
        // Assign to this nullable object the value of the (template parameter)
        // 'TYPE' (which has the 'bslma::UsesBslmaAllocator' trait) created in
        // place using the specified 'args'.  The object is also accessible via
        // the 'value' method.  If this nullable object already contains a
        // object ('false == isNull())', that object is destroyed before the
        // new object is created.  The allocator specified at the construction
        // of this nullable object is used to supply memory to the value
        // object.  Attempts to explicitly specify via 'args' another allocator
        // to supply memory to the created (value) object are disallowed by the
        // compiler.  Note that if the constructor of 'TYPE' throws an
        // exception this object is left in the null state.
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bdlb_nullablevalue.h
#ifndef BDLB_NULLABLEVALUE_VARIADIC_LIMIT
#define BDLB_NULLABLEVALUE_VARIADIC_LIMIT 5
#endif
#ifndef BDLB_NULLABLEVALUE_VARIADIC_LIMIT_B
#define BDLB_NULLABLEVALUE_VARIADIC_LIMIT_B BDLB_NULLABLEVALUE_VARIADIC_LIMIT
#endif
#if BDLB_NULLABLEVALUE_VARIADIC_LIMIT_B >= 0
    void makeValueInplace();
#endif  // BDLB_NULLABLEVALUE_VARIADIC_LIMIT_B >= 0

#if BDLB_NULLABLEVALUE_VARIADIC_LIMIT_B >= 1
    template <class ARGS_1>
    void makeValueInplace(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1);
#endif  // BDLB_NULLABLEVALUE_VARIADIC_LIMIT_B >= 1

#if BDLB_NULLABLEVALUE_VARIADIC_LIMIT_B >= 2
    template <class ARGS_1,
              class ARGS_2>
    void makeValueInplace(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1,
                          BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_2) args_2);
#endif  // BDLB_NULLABLEVALUE_VARIADIC_LIMIT_B >= 2

#if BDLB_NULLABLEVALUE_VARIADIC_LIMIT_B >= 3
    template <class ARGS_1,
              class ARGS_2,
              class ARGS_3>
    void makeValueInplace(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1,
                          BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_2) args_2,
                          BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_3) args_3);
#endif  // BDLB_NULLABLEVALUE_VARIADIC_LIMIT_B >= 3

#if BDLB_NULLABLEVALUE_VARIADIC_LIMIT_B >= 4
    template <class ARGS_1,
              class ARGS_2,
              class ARGS_3,
              class ARGS_4>
    void makeValueInplace(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1,
                          BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_2) args_2,
                          BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_3) args_3,
                          BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_4) args_4);
#endif  // BDLB_NULLABLEVALUE_VARIADIC_LIMIT_B >= 4

#if BDLB_NULLABLEVALUE_VARIADIC_LIMIT_B >= 5
    template <class ARGS_1,
              class ARGS_2,
              class ARGS_3,
              class ARGS_4,
              class ARGS_5>
    void makeValueInplace(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1,
                          BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_2) args_2,
                          BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_3) args_3,
                          BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_4) args_4,
                          BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_5) args_5);
#endif  // BDLB_NULLABLEVALUE_VARIADIC_LIMIT_B >= 5

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
    template <class... ARGS>
    void makeValueInplace(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args);
// }}} END GENERATED CODE
#endif

    void reset();
        // Reset this object to the default constructed state (i.e., to have
        // the null value).

    TYPE& value();
        // Return a reference providing modifiable access to the underlying
        // 'TYPE' object.  The behavior is undefined unless this object is
        // non-null.

    // ACCESSORS
    const TYPE *address() const;
        // Return the address of the  underlying 'TYPE' object.  The behavior
        // is undefined unless this object is non-null.

    bool isNull() const;
        // Return 'true' if this object is null, and 'false' otherwise.

    const TYPE& value() const;
        // Return a reference providing non-modifiable access to the underlying
        // 'TYPE' object.  The behavior is undefined unless this object is
        // non-null.

    allocator_type get_allocator() const;
        // Return the allocator used by this object to supply memory.
};

// FREE FUNCTIONS
template <class TYPE>
void swap(NullableValue_WithAllocator<TYPE>& a,
          NullableValue_WithAllocator<TYPE>& b);
    // Exchange the values of the specified 'a' and 'b' objects.  This function
    // provides the no-throw exception-safety guarantee if the (template
    // parameter) 'TYPE' provides that guarantee, the two objects were created
    // with the same allocator, and the result of the 'isNull' method for the
    // two objects is the same; otherwise this function provides the basic
    // guarantee.

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

    // PRIVATE TYPES
    typedef bslmf::MovableRefUtil MoveUtil;

    struct AllocType {
        // This empty 'struct' is used as an optional argument type in the
        // 'NullableValue' class.  Because it is not constructible by end
        // clients, constructor overloads using 'AllocType' are effectively
        // disabled (do not participate in overload resolution).
    };

    // FRIENDS
    friend class NullableValue<TYPE>;

    // PRIVATE MANIPULATORS
    template <class BDE_OTHER_TYPE>
    void makeValueRaw(BSLS_COMPILERFEATURES_FORWARD_REF(BDE_OTHER_TYPE) value);
        // Assign to this object the specified 'value' (of 'BDE_OTHER_TYPE')
        // converted to 'TYPE'.  The behavior is undefined unless this object
        // is null.  Note that this method will fail to compile if 'TYPE and
        // 'BDE_OTHER_TYPE' are not compatible.

  public:
    // CREATORS
    NullableValue_WithoutAllocator();
        // Create a nullable object having the null value.

    NullableValue_WithoutAllocator(
                               const NullableValue_WithoutAllocator& original);
        // Create a nullable object having the value of the specified
        // 'original' object.

    NullableValue_WithoutAllocator(
                   bslmf::MovableRef<NullableValue_WithoutAllocator> original);
        // Create a nullable object having the same value as the specified
        // 'original' object by moving the contents of 'original' to the
        // newly-created object.  'original' is left in a valid but unspecified
        // state.

    ~NullableValue_WithoutAllocator();
        // Destroy this object.

    // MANIPULATORS
    NullableValue_WithoutAllocator& operator=(
                                    const NullableValue_WithoutAllocator& rhs);
        // Assign to this object the value of the specified 'rhs', and return a
        // reference providing modifiable access to this object.

    NullableValue_WithoutAllocator& operator=(
                        bslmf::MovableRef<NullableValue_WithoutAllocator> rhs);
        // Assign to this object the value of the specified 'rhs', and return a
        // reference providing modifiable access to this object.  The contents
        // of 'rhs' are either move-inserted into or move-assigned to this
        // object.  'rhs' is left in a valid but unspecified state.

    void swap(NullableValue_WithoutAllocator& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee if the (template parameter) 'TYPE'
        // provides that guarantee and the result of the 'isNull' method for
        // the two objects being swapped is the same.

    template <class BDE_OTHER_TYPE>
    void makeValue(BSLS_COMPILERFEATURES_FORWARD_REF(BDE_OTHER_TYPE) value);
        // Assign to this object the specified 'value' (of 'BDE_OTHER_TYPE')
        // converted to 'TYPE'.  Note that this method will fail to compile if
        // 'TYPE and 'BDE_OTHER_TYPE' are not compatible.

    void makeValue();
        // Assign to this object the default value for 'TYPE'.

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... ARGS>
    void makeValueInplace(ARGS&&... args);
        // Assign to this nullable object the value of the (template parameter)
        // 'TYPE' (which does not have the 'bslma::UsesBslmaAllocator' trait)
        // created in place using the specified 'args'.  (The object is also
        // accessible via the 'value' method).  If this nullable object already
        // contains an object ('false == isNull())', that object is destroyed
        // before the new object is created.  Note that if the constructor of
        // 'TYPE' throws an exception this object is left in the null state.
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bdlb_nullablevalue.h
#ifndef BDLB_NULLABLEVALUE_VARIADIC_LIMIT
#define BDLB_NULLABLEVALUE_VARIADIC_LIMIT 5
#endif
#ifndef BDLB_NULLABLEVALUE_VARIADIC_LIMIT_C
#define BDLB_NULLABLEVALUE_VARIADIC_LIMIT_C BDLB_NULLABLEVALUE_VARIADIC_LIMIT
#endif
#if BDLB_NULLABLEVALUE_VARIADIC_LIMIT_C >= 0
    void makeValueInplace();
#endif  // BDLB_NULLABLEVALUE_VARIADIC_LIMIT_C >= 0

#if BDLB_NULLABLEVALUE_VARIADIC_LIMIT_C >= 1
    template <class ARGS_1>
    void makeValueInplace(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1);
#endif  // BDLB_NULLABLEVALUE_VARIADIC_LIMIT_C >= 1

#if BDLB_NULLABLEVALUE_VARIADIC_LIMIT_C >= 2
    template <class ARGS_1,
              class ARGS_2>
    void makeValueInplace(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1,
                          BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_2) args_2);
#endif  // BDLB_NULLABLEVALUE_VARIADIC_LIMIT_C >= 2

#if BDLB_NULLABLEVALUE_VARIADIC_LIMIT_C >= 3
    template <class ARGS_1,
              class ARGS_2,
              class ARGS_3>
    void makeValueInplace(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1,
                          BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_2) args_2,
                          BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_3) args_3);
#endif  // BDLB_NULLABLEVALUE_VARIADIC_LIMIT_C >= 3

#if BDLB_NULLABLEVALUE_VARIADIC_LIMIT_C >= 4
    template <class ARGS_1,
              class ARGS_2,
              class ARGS_3,
              class ARGS_4>
    void makeValueInplace(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1,
                          BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_2) args_2,
                          BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_3) args_3,
                          BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_4) args_4);
#endif  // BDLB_NULLABLEVALUE_VARIADIC_LIMIT_C >= 4

#if BDLB_NULLABLEVALUE_VARIADIC_LIMIT_C >= 5
    template <class ARGS_1,
              class ARGS_2,
              class ARGS_3,
              class ARGS_4,
              class ARGS_5>
    void makeValueInplace(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1,
                          BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_2) args_2,
                          BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_3) args_3,
                          BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_4) args_4,
                          BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_5) args_5);
#endif  // BDLB_NULLABLEVALUE_VARIADIC_LIMIT_C >= 5

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
    template <class... ARGS>
    void makeValueInplace(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args);
// }}} END GENERATED CODE
#endif

    void reset();
        // Reset this object to the default constructed state (i.e., to have
        // the null value).

    TYPE& value();
        // Return a reference providing modifiable access to the underlying
        // 'TYPE' object.  The behavior is undefined unless this object is
        // non-null.

    // ACCESSORS
    const TYPE *address() const;
        // Return the address of the  underlying 'TYPE' object.  The behavior
        // is undefined unless this object is non-null.

    bool isNull() const;
        // Return 'true' if this object is null, and 'false' otherwise.

    const TYPE& value() const;
        // Return a reference providing non-modifiable access to the underlying
        // 'TYPE' object.  The behavior is undefined unless this object is
        // non-null.
};

// FREE FUNCTIONS
template <class TYPE>
void swap(NullableValue_WithoutAllocator<TYPE>& a,
          NullableValue_WithoutAllocator<TYPE>& b);
    // Exchange the values of the specified 'a' and 'b' objects.  This function
    // provides the no-throw exception-safety guarantee if the (template
    // parameter) 'TYPE' provides that guarantee and the result of the 'isNull'
    // method for the two objects is the same; otherwise this function provides
    // the basic guarantee.

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                      // -------------------------
                      // class NullableValue<TYPE>
                      // -------------------------

// CREATORS
template <class TYPE>
inline
NullableValue<TYPE>::NullableValue() BSLS_KEYWORD_NOEXCEPT
{
}

template <class TYPE>
inline
NullableValue<TYPE>::NullableValue(const allocator_type& allocator)
                                                          BSLS_KEYWORD_NOEXCEPT
: d_imp(allocator)
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
                                   const allocator_type& allocator)
: d_imp(original.d_imp, allocator)
{
}

template <class TYPE>
inline
NullableValue<TYPE>::NullableValue(bslmf::MovableRef<NullableValue> original)
: d_imp(MoveUtil::move(MoveUtil::access(original).d_imp))
{
}

template <class TYPE>
inline
NullableValue<TYPE>::NullableValue(bslmf::MovableRef<NullableValue> original,
                                   const allocator_type&            allocator)
: d_imp(MoveUtil::move(MoveUtil::access(original).d_imp), allocator)
{
}

template <class TYPE>
template <class BDE_OTHER_TYPE>
inline
NullableValue<TYPE>::NullableValue(
       BSLS_COMPILERFEATURES_FORWARD_REF(BDE_OTHER_TYPE)  value,
       typename bsl::enable_if<bsl::is_convertible<BDE_OTHER_TYPE, TYPE>::value
                               &&
                               !bsl::is_convertible<BDE_OTHER_TYPE,
                                                    allocator_type>::value,
                               EnableType>::type)
{
    d_imp.makeValueRaw(BSLS_COMPILERFEATURES_FORWARD(BDE_OTHER_TYPE, value));
}

template <class TYPE>
template <class BDE_OTHER_TYPE>
inline
NullableValue<TYPE>::NullableValue(
      BSLS_COMPILERFEATURES_FORWARD_REF(BDE_OTHER_TYPE)  value,
      const allocator_type&                              allocator,
      typename bsl::enable_if<bsl::is_convertible<BDE_OTHER_TYPE, TYPE>::value,
                              EnableType>::type)
: d_imp(allocator)
{
    d_imp.makeValueRaw(BSLS_COMPILERFEATURES_FORWARD(BDE_OTHER_TYPE, value));
}

template <class TYPE>
template <class BDE_OTHER_TYPE>
inline
NullableValue<TYPE>::NullableValue(
                                 const NullableValue<BDE_OTHER_TYPE>& original)
{
    if (!original.isNull()) {
        d_imp.makeValueRaw(original.value());
    }
}

template <class TYPE>
template <class BDE_OTHER_TYPE>
inline
NullableValue<TYPE>::NullableValue(
                                const NullableValue<BDE_OTHER_TYPE>& original,
                                const allocator_type&                allocator)
: d_imp(allocator)
{
    if (!original.isNull()) {
        d_imp.makeValueRaw(original.value());
    }
}

template <class TYPE>
inline
NullableValue<TYPE>::NullableValue(const NullOptType&) BSLS_KEYWORD_NOEXCEPT
: d_imp()
{
}

template <class TYPE>
inline
NullableValue<TYPE>::NullableValue(const NullOptType&    ,
                                   const allocator_type& allocator)
                                                          BSLS_KEYWORD_NOEXCEPT
: d_imp(allocator)
{
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
inline
NullableValue<TYPE>&
NullableValue<TYPE>::operator=(bslmf::MovableRef<NullableValue> rhs)
{
    NullableValue& lvalue = rhs;
    d_imp = MoveUtil::move(lvalue.d_imp);

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
template <class STREAM>
STREAM& NullableValue<TYPE>::bdexStreamIn(STREAM& stream, int version)
{
    using bslx::InStreamFunctions::bdexStreamIn;

    char isNull = 0; // Redundant initialization to suppress -Werror.

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
NullableValue<TYPE>&
NullableValue<TYPE>::operator=(bslmf::MovableRef<TYPE> rhs)
{
    d_imp.makeValue(MoveUtil::move(rhs));

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
NullableValue<TYPE>& NullableValue<TYPE>::operator=(const NullOptType&)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    d_imp.reset();

    return *this;
}

template <class TYPE>
template <class BDE_OTHER_TYPE>
inline
TYPE& NullableValue<TYPE>::makeValue(
                       BSLS_COMPILERFEATURES_FORWARD_REF(BDE_OTHER_TYPE) value)
{
    d_imp.makeValue(BSLS_COMPILERFEATURES_FORWARD(BDE_OTHER_TYPE, value));
    return d_imp.value();
}

template <class TYPE>
inline
TYPE& NullableValue<TYPE>::makeValue()
{
    d_imp.makeValue();

    return d_imp.value();
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class TYPE>
template <class... ARGS>
inline
TYPE& NullableValue<TYPE>::makeValueInplace(ARGS&&... args)
{
    d_imp.makeValueInplace(BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
    return d_imp.value();
}
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bdlb_nullablevalue.h
#ifndef BDLB_NULLABLEVALUE_VARIADIC_LIMIT
#define BDLB_NULLABLEVALUE_VARIADIC_LIMIT 5
#endif
#ifndef BDLB_NULLABLEVALUE_VARIADIC_LIMIT_D
#define BDLB_NULLABLEVALUE_VARIADIC_LIMIT_D BDLB_NULLABLEVALUE_VARIADIC_LIMIT
#endif
#if BDLB_NULLABLEVALUE_VARIADIC_LIMIT_D >= 0
template <class TYPE>
inline
TYPE& NullableValue<TYPE>::makeValueInplace(
                               )
{
    d_imp.makeValueInplace();
    return d_imp.value();
}
#endif  // BDLB_NULLABLEVALUE_VARIADIC_LIMIT_D >= 0

#if BDLB_NULLABLEVALUE_VARIADIC_LIMIT_D >= 1
template <class TYPE>
template <class ARGS_1>
inline
TYPE& NullableValue<TYPE>::makeValueInplace(
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1)
{
    d_imp.makeValueInplace(BSLS_COMPILERFEATURES_FORWARD(ARGS_1, args_1));
    return d_imp.value();
}
#endif  // BDLB_NULLABLEVALUE_VARIADIC_LIMIT_D >= 1

#if BDLB_NULLABLEVALUE_VARIADIC_LIMIT_D >= 2
template <class TYPE>
template <class ARGS_1,
          class ARGS_2>
inline
TYPE& NullableValue<TYPE>::makeValueInplace(
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_2) args_2)
{
    d_imp.makeValueInplace(BSLS_COMPILERFEATURES_FORWARD(ARGS_1, args_1),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_2, args_2));
    return d_imp.value();
}
#endif  // BDLB_NULLABLEVALUE_VARIADIC_LIMIT_D >= 2

#if BDLB_NULLABLEVALUE_VARIADIC_LIMIT_D >= 3
template <class TYPE>
template <class ARGS_1,
          class ARGS_2,
          class ARGS_3>
inline
TYPE& NullableValue<TYPE>::makeValueInplace(
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_2) args_2,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_3) args_3)
{
    d_imp.makeValueInplace(BSLS_COMPILERFEATURES_FORWARD(ARGS_1, args_1),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_2, args_2),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_3, args_3));
    return d_imp.value();
}
#endif  // BDLB_NULLABLEVALUE_VARIADIC_LIMIT_D >= 3

#if BDLB_NULLABLEVALUE_VARIADIC_LIMIT_D >= 4
template <class TYPE>
template <class ARGS_1,
          class ARGS_2,
          class ARGS_3,
          class ARGS_4>
inline
TYPE& NullableValue<TYPE>::makeValueInplace(
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_2) args_2,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_3) args_3,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_4) args_4)
{
    d_imp.makeValueInplace(BSLS_COMPILERFEATURES_FORWARD(ARGS_1, args_1),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_2, args_2),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_3, args_3),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_4, args_4));
    return d_imp.value();
}
#endif  // BDLB_NULLABLEVALUE_VARIADIC_LIMIT_D >= 4

#if BDLB_NULLABLEVALUE_VARIADIC_LIMIT_D >= 5
template <class TYPE>
template <class ARGS_1,
          class ARGS_2,
          class ARGS_3,
          class ARGS_4,
          class ARGS_5>
inline
TYPE& NullableValue<TYPE>::makeValueInplace(
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_2) args_2,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_3) args_3,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_4) args_4,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_5) args_5)
{
    d_imp.makeValueInplace(BSLS_COMPILERFEATURES_FORWARD(ARGS_1, args_1),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_2, args_2),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_3, args_3),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_4, args_4),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_5, args_5));
    return d_imp.value();
}
#endif  // BDLB_NULLABLEVALUE_VARIADIC_LIMIT_D >= 5

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
template <class TYPE>
template <class... ARGS>
inline
TYPE& NullableValue<TYPE>::makeValueInplace(
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args)
{
    d_imp.makeValueInplace(BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
    return d_imp.value();
}
// }}} END GENERATED CODE
#endif

template <class TYPE>
inline
void NullableValue<TYPE>::reset()
{
    d_imp.reset();
}

template <class TYPE>
inline
void NullableValue<TYPE>::swap(NullableValue<TYPE>& other)
{
    d_imp.swap(other.d_imp);
}

template <class TYPE>
inline
TYPE& NullableValue<TYPE>::value()
{
    return d_imp.value();
}

// ACCESSORS
template <class TYPE>
inline
const TYPE *NullableValue<TYPE>::addressOr(const TYPE *address) const
{
    return d_imp.isNull() ? address : d_imp.address();
}

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
bool NullableValue<TYPE>::isNull() const BSLS_KEYWORD_NOEXCEPT
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

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
template <class TYPE>
inline
int NullableValue<TYPE>::maxSupportedBdexVersion() const
{
    using bslx::VersionFunctions::maxSupportedBdexVersion;

    // We need to call the 'bslx::VersionFunctions' helper function, because we
    // cannot guarantee that 'TYPE' implements 'maxSupportedBdexVersion' as a
    // class method.

    return maxSupportedBdexVersion(reinterpret_cast<TYPE *>(0));
}
#endif  // BDE_OMIT_INTERNAL_DEPRECATED

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
TYPE NullableValue<TYPE>::valueOr(const TYPE& value) const
{
    return d_imp.isNull() ? value : d_imp.value();
}

template <class TYPE>
inline
const TYPE *NullableValue<TYPE>::valueOr(const TYPE *value) const
{
    return d_imp.isNull() ? value : d_imp.address();
}

template <class TYPE>
inline
const TYPE *NullableValue<TYPE>::valueOrNull() const
{
    return d_imp.isNull() ? 0 : d_imp.address();
}

template <class TYPE>
inline
typename NullableValue<TYPE>::allocator_type
NullableValue<TYPE>::get_allocator() const
{
    // Will not compile if 'd_imp' does not have a 'get_allocator' method.
    return d_imp.get_allocator();
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

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator==(const NullableValue<LHS_TYPE>& lhs,
                      const RHS_TYPE&                rhs)
{
    return !lhs.isNull() && lhs.value() == rhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator==(const LHS_TYPE&                lhs,
                      const NullableValue<RHS_TYPE>& rhs)
{
    return !rhs.isNull() && rhs.value() == lhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator!=(const NullableValue<LHS_TYPE>& lhs,
                      const RHS_TYPE&                rhs)
{
    return lhs.isNull() || lhs.value() != rhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator!=(const LHS_TYPE&                lhs,
                      const NullableValue<RHS_TYPE>& rhs)
{
    return rhs.isNull() || rhs.value() != lhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator<(const NullableValue<LHS_TYPE>& lhs,
                     const NullableValue<RHS_TYPE>& rhs)
{
    if (rhs.isNull()) {
        return false;                                                 // RETURN
    }

    return lhs.isNull() || lhs.value() < rhs.value();
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator<(const NullableValue<LHS_TYPE>& lhs,
                     const RHS_TYPE&                rhs)
{
    return lhs.isNull() || lhs.value() < rhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator<(const LHS_TYPE&                lhs,
                     const NullableValue<RHS_TYPE>& rhs)
{
    return !rhs.isNull() && lhs < rhs.value();
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator>(const NullableValue<LHS_TYPE>& lhs,
                     const NullableValue<RHS_TYPE>& rhs)
{
    return rhs < lhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator>(const NullableValue<LHS_TYPE>& lhs,
                     const RHS_TYPE&                rhs)
{
    return rhs < lhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator>(const LHS_TYPE&                lhs,
                     const NullableValue<RHS_TYPE>& rhs)
{
    return rhs < lhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator<=(const NullableValue<LHS_TYPE>& lhs,
                      const NullableValue<RHS_TYPE>& rhs)
{
    return !(rhs < lhs);
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator<=(const NullableValue<LHS_TYPE>& lhs,
                      const RHS_TYPE&                rhs)
{
    return !(rhs < lhs);
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator<=(const LHS_TYPE&                lhs,
                      const NullableValue<RHS_TYPE>& rhs)
{
    return !(rhs < lhs);
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator>=(const NullableValue<LHS_TYPE>& lhs,
                      const NullableValue<RHS_TYPE>& rhs)
{
    return !(lhs < rhs);
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator>=(const NullableValue<LHS_TYPE>& lhs,
                      const RHS_TYPE&                rhs)
{
    return !(lhs < rhs);
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator>=(const LHS_TYPE&                lhs,
                      const NullableValue<RHS_TYPE>& rhs)
{
    return !(lhs < rhs);
}

template <class TYPE>
inline BSLS_KEYWORD_CONSTEXPR
bool bdlb::operator==(const NullableValue<TYPE>& value,
                      const NullOptType&         ) BSLS_KEYWORD_NOEXCEPT
{
    return value.isNull();
}

template <class TYPE>
inline BSLS_KEYWORD_CONSTEXPR
bool bdlb::operator==(const NullOptType&         ,
                      const NullableValue<TYPE>& value) BSLS_KEYWORD_NOEXCEPT
{
    return value.isNull();
}

template <class TYPE>
inline BSLS_KEYWORD_CONSTEXPR
bool bdlb::operator!=(const NullableValue<TYPE>& value,
                      const NullOptType&         ) BSLS_KEYWORD_NOEXCEPT
{
    return !value.isNull();
}

template <class TYPE>
inline BSLS_KEYWORD_CONSTEXPR
bool bdlb::operator!=(const NullOptType&         ,
                      const NullableValue<TYPE>& value) BSLS_KEYWORD_NOEXCEPT
{
    return !value.isNull();
}

template <class TYPE>
inline BSLS_KEYWORD_CONSTEXPR
bool bdlb::operator<(const NullableValue<TYPE>&,
                     const NullOptType&        ) BSLS_KEYWORD_NOEXCEPT
{
    return false;
}

template <class TYPE>
inline BSLS_KEYWORD_CONSTEXPR
bool bdlb::operator<(const NullOptType&         ,
                     const NullableValue<TYPE>& value) BSLS_KEYWORD_NOEXCEPT
{
    return !value.isNull();
}

template <class TYPE>
inline BSLS_KEYWORD_CONSTEXPR
bool bdlb::operator>(const NullableValue<TYPE>& value,
                     const NullOptType&         ) BSLS_KEYWORD_NOEXCEPT
{
    return !value.isNull();
}

template <class TYPE>
inline BSLS_KEYWORD_CONSTEXPR
bool bdlb::operator>(const NullOptType&         ,
                     const NullableValue<TYPE>& ) BSLS_KEYWORD_NOEXCEPT
{
    return false;
}

template <class TYPE>
inline BSLS_KEYWORD_CONSTEXPR
bool bdlb::operator<=(const NullableValue<TYPE>& value,
                      const NullOptType&         ) BSLS_KEYWORD_NOEXCEPT
{
    return value.isNull();
}

template <class TYPE>
inline BSLS_KEYWORD_CONSTEXPR
bool bdlb::operator<=(const NullOptType&         ,
                      const NullableValue<TYPE>& ) BSLS_KEYWORD_NOEXCEPT
{
    return true;
}

template <class TYPE>
inline BSLS_KEYWORD_CONSTEXPR
bool bdlb::operator>=(const NullableValue<TYPE>& ,
                      const NullOptType&         ) BSLS_KEYWORD_NOEXCEPT
{
    return true;
}

template <class TYPE>
inline BSLS_KEYWORD_CONSTEXPR
bool bdlb::operator>=(const NullOptType&  ,
                      const NullableValue<TYPE>& value) BSLS_KEYWORD_NOEXCEPT
{
    return value.isNull();
}

template <class TYPE>
inline
bsl::ostream& bdlb::operator<<(bsl::ostream&              stream,
                               const NullableValue<TYPE>& object)
{
    return object.print(stream, 0, -1);
}

// FREE FUNCTIONS
template <class HASHALG, class TYPE>
void bdlb::hashAppend(HASHALG& hashAlg, const NullableValue<TYPE>& input)
{
    if (!input.isNull()) {
        hashAppend(hashAlg, true);
        hashAppend(hashAlg, input.value());
    }
    else {
        hashAppend(hashAlg, false);
    }
}

template <class TYPE>
inline
void bdlb::swap(NullableValue<TYPE>& a, NullableValue<TYPE>& b)
{
    bslalg::SwapUtil::swap(&a.d_imp, &b.d_imp);
}

namespace bdlb {

               // ---------------------------------------
               // class NullableValue_WithAllocator<TYPE>
               // ---------------------------------------

// PRIVATE MANIPULATORS
template <class TYPE>
template <class BDE_OTHER_TYPE>
void NullableValue_WithAllocator<TYPE>::makeValueRaw(
                       BSLS_COMPILERFEATURES_FORWARD_REF(BDE_OTHER_TYPE) value)
{
    BSLS_ASSERT_SAFE(d_isNull);

    bslma::ConstructionUtil::construct(
                         d_buffer.address(),
                         d_allocator.mechanism(),
                         BSLS_COMPILERFEATURES_FORWARD(BDE_OTHER_TYPE, value));
    d_isNull = false;
}

// CREATORS
template <class TYPE>
inline
NullableValue_WithAllocator<TYPE>::NullableValue_WithAllocator(
                                                    const AllocType& allocator)
: d_isNull(true)
, d_allocator(allocator)
{
}

template <class TYPE>
inline
NullableValue_WithAllocator<TYPE>::NullableValue_WithAllocator(
                                  const NullableValue_WithAllocator& original,
                                  const AllocType&                   allocator)
: d_isNull(true)
, d_allocator(allocator)
{
    if (!original.isNull()) {
        makeValueRaw(original.value());
    }
}

template <class TYPE>
inline
NullableValue_WithAllocator<TYPE>::NullableValue_WithAllocator(
                       bslmf::MovableRef<NullableValue_WithAllocator> original)
: d_isNull(true)
, d_allocator(MoveUtil::access(original).d_allocator)
{
    NullableValue_WithAllocator& lvalue = original;

    if (!lvalue.isNull()) {
        makeValueRaw(MoveUtil::move(lvalue.value()));
    }
}

template <class TYPE>
inline
NullableValue_WithAllocator<TYPE>::NullableValue_WithAllocator(
                      bslmf::MovableRef<NullableValue_WithAllocator> original,
                      const AllocType&                               allocator)
: d_isNull(true)
, d_allocator(allocator)
{
    NullableValue_WithAllocator& lvalue = original;

    if (!lvalue.isNull()) {
        makeValueRaw(MoveUtil::move(lvalue.value()));
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
NullableValue_WithAllocator<TYPE>&
NullableValue_WithAllocator<TYPE>::operator=(
                            bslmf::MovableRef<NullableValue_WithAllocator> rhs)
{
    NullableValue_WithAllocator& lvalue = rhs;

    if (!lvalue.isNull()) {
        makeValue(MoveUtil::move(lvalue.value()));
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

    BSLS_ASSERT(d_allocator == other.d_allocator);

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
template <class BDE_OTHER_TYPE>
void NullableValue_WithAllocator<TYPE>::makeValue(
                       BSLS_COMPILERFEATURES_FORWARD_REF(BDE_OTHER_TYPE) value)
{
    if (d_isNull) {
        makeValueRaw(BSLS_COMPILERFEATURES_FORWARD(BDE_OTHER_TYPE, value));
    }
    else {
        d_buffer.object() =
            BSLS_COMPILERFEATURES_FORWARD(BDE_OTHER_TYPE, value);
    }
}

template <class TYPE>
inline
void NullableValue_WithAllocator<TYPE>::makeValue()
{
    reset();

    bslma::ConstructionUtil::construct(d_buffer.address(),
                                       d_allocator.mechanism());
    d_isNull = false;
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class TYPE>
template <class... ARGS>
inline
void NullableValue_WithAllocator<TYPE>::makeValueInplace(ARGS&&... args)
{
    reset();
    bslma::ConstructionUtil::construct(
        d_buffer.address(),
        d_allocator.mechanism(),
        BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
    d_isNull = false;
}
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bdlb_nullablevalue.h
#ifndef BDLB_NULLABLEVALUE_VARIADIC_LIMIT
#define BDLB_NULLABLEVALUE_VARIADIC_LIMIT 5
#endif
#ifndef BDLB_NULLABLEVALUE_VARIADIC_LIMIT_E
#define BDLB_NULLABLEVALUE_VARIADIC_LIMIT_E BDLB_NULLABLEVALUE_VARIADIC_LIMIT
#endif
#if BDLB_NULLABLEVALUE_VARIADIC_LIMIT_E >= 0
template <class TYPE>
inline
void NullableValue_WithAllocator<TYPE>::makeValueInplace(
                               )
{
    reset();
    bslma::ConstructionUtil::construct(
        d_buffer.address(),
        d_allocator.mechanism());
    d_isNull = false;
}
#endif  // BDLB_NULLABLEVALUE_VARIADIC_LIMIT_E >= 0

#if BDLB_NULLABLEVALUE_VARIADIC_LIMIT_E >= 1
template <class TYPE>
template <class ARGS_1>
inline
void NullableValue_WithAllocator<TYPE>::makeValueInplace(
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1)
{
    reset();
    bslma::ConstructionUtil::construct(
        d_buffer.address(),
        d_allocator.mechanism(),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_1, args_1));
    d_isNull = false;
}
#endif  // BDLB_NULLABLEVALUE_VARIADIC_LIMIT_E >= 1

#if BDLB_NULLABLEVALUE_VARIADIC_LIMIT_E >= 2
template <class TYPE>
template <class ARGS_1,
          class ARGS_2>
inline
void NullableValue_WithAllocator<TYPE>::makeValueInplace(
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_2) args_2)
{
    reset();
    bslma::ConstructionUtil::construct(
        d_buffer.address(),
        d_allocator.mechanism(),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_1, args_1),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_2, args_2));
    d_isNull = false;
}
#endif  // BDLB_NULLABLEVALUE_VARIADIC_LIMIT_E >= 2

#if BDLB_NULLABLEVALUE_VARIADIC_LIMIT_E >= 3
template <class TYPE>
template <class ARGS_1,
          class ARGS_2,
          class ARGS_3>
inline
void NullableValue_WithAllocator<TYPE>::makeValueInplace(
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_2) args_2,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_3) args_3)
{
    reset();
    bslma::ConstructionUtil::construct(
        d_buffer.address(),
        d_allocator.mechanism(),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_1, args_1),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_2, args_2),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_3, args_3));
    d_isNull = false;
}
#endif  // BDLB_NULLABLEVALUE_VARIADIC_LIMIT_E >= 3

#if BDLB_NULLABLEVALUE_VARIADIC_LIMIT_E >= 4
template <class TYPE>
template <class ARGS_1,
          class ARGS_2,
          class ARGS_3,
          class ARGS_4>
inline
void NullableValue_WithAllocator<TYPE>::makeValueInplace(
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_2) args_2,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_3) args_3,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_4) args_4)
{
    reset();
    bslma::ConstructionUtil::construct(
        d_buffer.address(),
        d_allocator.mechanism(),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_1, args_1),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_2, args_2),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_3, args_3),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_4, args_4));
    d_isNull = false;
}
#endif  // BDLB_NULLABLEVALUE_VARIADIC_LIMIT_E >= 4

#if BDLB_NULLABLEVALUE_VARIADIC_LIMIT_E >= 5
template <class TYPE>
template <class ARGS_1,
          class ARGS_2,
          class ARGS_3,
          class ARGS_4,
          class ARGS_5>
inline
void NullableValue_WithAllocator<TYPE>::makeValueInplace(
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_2) args_2,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_3) args_3,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_4) args_4,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_5) args_5)
{
    reset();
    bslma::ConstructionUtil::construct(
        d_buffer.address(),
        d_allocator.mechanism(),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_1, args_1),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_2, args_2),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_3, args_3),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_4, args_4),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_5, args_5));
    d_isNull = false;
}
#endif  // BDLB_NULLABLEVALUE_VARIADIC_LIMIT_E >= 5

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
template <class TYPE>
template <class... ARGS>
inline
void NullableValue_WithAllocator<TYPE>::makeValueInplace(
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args)
{
    reset();
    bslma::ConstructionUtil::construct(
        d_buffer.address(),
        d_allocator.mechanism(),
        BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
    d_isNull = false;
}
// }}} END GENERATED CODE
#endif

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
const TYPE *NullableValue_WithAllocator<TYPE>::address() const
{
    BSLS_ASSERT_SAFE(!d_isNull);

    return d_buffer.address();
}

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

template <class TYPE>
inline
typename NullableValue_WithAllocator<TYPE>::AllocType
NullableValue_WithAllocator<TYPE>::get_allocator() const
{
    return d_allocator;
}

}  // close package namespace

// FREE FUNCTIONS
template <class TYPE>
void bdlb::swap(NullableValue_WithAllocator<TYPE>& a,
                NullableValue_WithAllocator<TYPE>& b)
{
    if (a.d_allocator == b.d_allocator) {
        a.swap(b);

        return;                                                       // RETURN
    }

    NullableValue_WithAllocator<TYPE> futureA(b, a.d_allocator);
    NullableValue_WithAllocator<TYPE> futureB(a, b.d_allocator);

    futureA.swap(a);
    futureB.swap(b);
}

namespace bdlb {

              // ------------------------------------------
              // class NullableValue_WithoutAllocator<TYPE>
              // ------------------------------------------

// PRIVATE MANIPULATORS
template <class TYPE>
template <class BDE_OTHER_TYPE>
void NullableValue_WithoutAllocator<TYPE>::makeValueRaw(
                       BSLS_COMPILERFEATURES_FORWARD_REF(BDE_OTHER_TYPE) value)
{
    BSLS_ASSERT_SAFE(d_isNull);

    bslma::ConstructionUtil::construct(
                         d_buffer.address(),
                         (void *)0,
                         BSLS_COMPILERFEATURES_FORWARD(BDE_OTHER_TYPE, value));
    d_isNull = false;
}

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
        makeValueRaw(original.value());
    }
}

template <class TYPE>
inline
NullableValue_WithoutAllocator<TYPE>::NullableValue_WithoutAllocator(
                    bslmf::MovableRef<NullableValue_WithoutAllocator> original)
: d_isNull(true)
{
    NullableValue_WithoutAllocator& lvalue = original;

    if (!lvalue.isNull()) {
        makeValueRaw(MoveUtil::move(lvalue.value()));
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
NullableValue_WithoutAllocator<TYPE>&
NullableValue_WithoutAllocator<TYPE>::operator=(
                         bslmf::MovableRef<NullableValue_WithoutAllocator> rhs)
{
    NullableValue_WithoutAllocator& lvalue = rhs;

    if (!lvalue.isNull()) {
        makeValue(MoveUtil::move(lvalue.value()));
    }
    else {
        reset();
    }

    return *this;
}

template <class TYPE>
template <class BDE_OTHER_TYPE>
void NullableValue_WithoutAllocator<TYPE>::makeValue(
                       BSLS_COMPILERFEATURES_FORWARD_REF(BDE_OTHER_TYPE) value)
{
    if (d_isNull) {
        makeValueRaw(BSLS_COMPILERFEATURES_FORWARD(BDE_OTHER_TYPE, value));
    }
    else {
        d_buffer.object() =
            BSLS_COMPILERFEATURES_FORWARD(BDE_OTHER_TYPE, value);
    }
}

template <class TYPE>
inline
void NullableValue_WithoutAllocator<TYPE>::makeValue()
{
    reset();

    bslma::ConstructionUtil::construct(d_buffer.address(), (void *)0);
    d_isNull = false;
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class TYPE>
template <class... ARGS>
inline
void NullableValue_WithoutAllocator<TYPE>::makeValueInplace(ARGS&&... args)
{
    reset();
    bslma::ConstructionUtil::construct(
        d_buffer.address(),
        (void *)0,
        BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
    d_isNull = false;
}
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bdlb_nullablevalue.h
#ifndef BDLB_NULLABLEVALUE_VARIADIC_LIMIT
#define BDLB_NULLABLEVALUE_VARIADIC_LIMIT 5
#endif
#ifndef BDLB_NULLABLEVALUE_VARIADIC_LIMIT_F
#define BDLB_NULLABLEVALUE_VARIADIC_LIMIT_F BDLB_NULLABLEVALUE_VARIADIC_LIMIT
#endif
#if BDLB_NULLABLEVALUE_VARIADIC_LIMIT_F >= 0
template <class TYPE>
inline
void NullableValue_WithoutAllocator<TYPE>::makeValueInplace(
                               )
{
    reset();
    bslma::ConstructionUtil::construct(
        d_buffer.address(),
        (void *)0);
    d_isNull = false;
}
#endif  // BDLB_NULLABLEVALUE_VARIADIC_LIMIT_F >= 0

#if BDLB_NULLABLEVALUE_VARIADIC_LIMIT_F >= 1
template <class TYPE>
template <class ARGS_1>
inline
void NullableValue_WithoutAllocator<TYPE>::makeValueInplace(
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1)
{
    reset();
    bslma::ConstructionUtil::construct(
        d_buffer.address(),
        (void *)0,
        BSLS_COMPILERFEATURES_FORWARD(ARGS_1, args_1));
    d_isNull = false;
}
#endif  // BDLB_NULLABLEVALUE_VARIADIC_LIMIT_F >= 1

#if BDLB_NULLABLEVALUE_VARIADIC_LIMIT_F >= 2
template <class TYPE>
template <class ARGS_1,
          class ARGS_2>
inline
void NullableValue_WithoutAllocator<TYPE>::makeValueInplace(
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_2) args_2)
{
    reset();
    bslma::ConstructionUtil::construct(
        d_buffer.address(),
        (void *)0,
        BSLS_COMPILERFEATURES_FORWARD(ARGS_1, args_1),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_2, args_2));
    d_isNull = false;
}
#endif  // BDLB_NULLABLEVALUE_VARIADIC_LIMIT_F >= 2

#if BDLB_NULLABLEVALUE_VARIADIC_LIMIT_F >= 3
template <class TYPE>
template <class ARGS_1,
          class ARGS_2,
          class ARGS_3>
inline
void NullableValue_WithoutAllocator<TYPE>::makeValueInplace(
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_2) args_2,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_3) args_3)
{
    reset();
    bslma::ConstructionUtil::construct(
        d_buffer.address(),
        (void *)0,
        BSLS_COMPILERFEATURES_FORWARD(ARGS_1, args_1),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_2, args_2),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_3, args_3));
    d_isNull = false;
}
#endif  // BDLB_NULLABLEVALUE_VARIADIC_LIMIT_F >= 3

#if BDLB_NULLABLEVALUE_VARIADIC_LIMIT_F >= 4
template <class TYPE>
template <class ARGS_1,
          class ARGS_2,
          class ARGS_3,
          class ARGS_4>
inline
void NullableValue_WithoutAllocator<TYPE>::makeValueInplace(
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_2) args_2,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_3) args_3,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_4) args_4)
{
    reset();
    bslma::ConstructionUtil::construct(
        d_buffer.address(),
        (void *)0,
        BSLS_COMPILERFEATURES_FORWARD(ARGS_1, args_1),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_2, args_2),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_3, args_3),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_4, args_4));
    d_isNull = false;
}
#endif  // BDLB_NULLABLEVALUE_VARIADIC_LIMIT_F >= 4

#if BDLB_NULLABLEVALUE_VARIADIC_LIMIT_F >= 5
template <class TYPE>
template <class ARGS_1,
          class ARGS_2,
          class ARGS_3,
          class ARGS_4,
          class ARGS_5>
inline
void NullableValue_WithoutAllocator<TYPE>::makeValueInplace(
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_2) args_2,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_3) args_3,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_4) args_4,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_5) args_5)
{
    reset();
    bslma::ConstructionUtil::construct(
        d_buffer.address(),
        (void *)0,
        BSLS_COMPILERFEATURES_FORWARD(ARGS_1, args_1),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_2, args_2),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_3, args_3),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_4, args_4),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_5, args_5));
    d_isNull = false;
}
#endif  // BDLB_NULLABLEVALUE_VARIADIC_LIMIT_F >= 5

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
template <class TYPE>
template <class... ARGS>
inline
void NullableValue_WithoutAllocator<TYPE>::makeValueInplace(
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args)
{
    reset();
    bslma::ConstructionUtil::construct(
        d_buffer.address(),
        (void *)0,
        BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
    d_isNull = false;
}
// }}} END GENERATED CODE
#endif

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
inline
TYPE& NullableValue_WithoutAllocator<TYPE>::value()
{
    BSLS_ASSERT_SAFE(!d_isNull);

    return d_buffer.object();
}

// ACCESSORS
template <class TYPE>
inline
const TYPE *NullableValue_WithoutAllocator<TYPE>::address() const
{
    BSLS_ASSERT_SAFE(!d_isNull);

    return d_buffer.address();
}

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

// FREE FUNCTIONS
template <class TYPE>
inline
void bdlb::swap(NullableValue_WithoutAllocator<TYPE>& a,
                NullableValue_WithoutAllocator<TYPE>& b)
{
    a.swap(b);
}

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
