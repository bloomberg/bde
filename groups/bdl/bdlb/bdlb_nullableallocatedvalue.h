// bdlb_nullableallocatedvalue.h                                      -*-C++-*-
#ifndef INCLUDED_BDLB_NULLABLEALLOCATEDVALUE
#define INCLUDED_BDLB_NULLABLEALLOCATEDVALUE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a template for nullable allocated (out-of-place) objects.
//
//@CLASSES:
//  bdlb::NullableAllocatedValue: template for nullable allocated objects
//
//@SEE_ALSO: bdlb_nullablevalue
//
//@DESCRIPTION: This component provides a template class,
// `bdlb::NullableAllocatedValue<TYPE>`, that has nearly the same interface as
// `bdlb::NullableValue` (see `bdlb_nullablevalue`), but, in contrast with that
// template class, the implementation of `bdlb::NullableAllocatedValue` does
// not require that the `TYPE` parameter be a complete type when the *class* is
// instantiated.  However, the template parameter `TYPE` must be complete when
// *methods* of the class (and free operators) are instantiated.
//
// For small types (no larger than a pointer) with simple alignment needs, the
// object is embedded into the NullableAllocatedValue object.  For types that
// do not fit these requirements, the object of template parameter `TYPE` that
// is managed by a `bdlb::NullableAllocatedValue<TYPE>` object is allocated
// out-of-place.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// Suppose we want to create a linked list of nodes that contain integers:
// ```
// struct LinkedListNode {
//     int                                          d_value;
//     bdlb::NullableAllocatedValue<LinkedListNode> d_next;
// };
// ```
// Note that `bdlb::NullableValue<LinkedListNode>` cannot be used for `d_next`
// because `bdlb::NullableValue` requires that the template parameter `TYPE` be
// a complete type when the class is instantiated.
//
// We can now traverse a linked list and add a new value at the end using the
// following code:
// ```
// /// Add the specified `value` to the end of the list that contains the
// /// specified `node`.
// void addValueAtEnd(LinkedListNode *node, int value)
// {
//     while (!node->d_next.isNull()) {
//         node = &node->d_next.value();
//     }
//
//     node->d_next.makeValue();
//     node = &node->d_next.value();
//     node->d_value = value;
// }
// ```

#include <bdlscm_version.h>

#include <bdlb_nullableallocatedvalue_pointerbitspair.h>
#include <bdlb_printmethods.h>

#include <bslalg_scalarprimitives.h>
#include <bslalg_swaputil.h>

#include <bslma_allocator.h>
#include <bslma_deallocatorproctor.h>
#include <bslma_default.h>

#include <bslmf_isbitwisemoveable.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_deprecate.h>
#include <bsls_deprecatefeature.h>
#include <bsls_keyword.h>
#include <bsls_review.h>

#include <bslstl_optional.h>     // bsl::nullopt

#include <bslx_instreamfunctions.h>
#include <bslx_outstreamfunctions.h>
#include <bslx_versionfunctions.h>

#include <bsl_algorithm.h>
#include <bsl_cstddef.h>    // bsl::size_t
#include <bsl_cstdint.h>    // uintptr_t
#include <bsl_iosfwd.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslalg_typetraits.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#include <stddef.h>   // NULL

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// clang-format off
// Include version that can be compiled with C++03
// Generated on Mon Jan 13 08:32:07 2025
// Command line: sim_cpp11_features.pl bdlb_nullableallocatedvalue.h

# define COMPILING_BDLB_NULLABLEALLOCATEDVALUE_H
# include <bdlb_nullableallocatedvalue_cpp03.h>
# undef COMPILING_BDLB_NULLABLEALLOCATEDVALUE_H

// clang-format on
#else

namespace BloombergLP {
namespace bdlb {

                      // ==================================
                      // class NullableAllocatedValue<TYPE>
                      // ==================================

/// This template class extends the set of values of its value-semantic
/// `TYPE` parameter to include the notion of a "null" value.  If `TYPE` is
/// fully value-semantic, then the augmented type
/// `NullableAllocatedValue<TYPE>` will be as well.  In addition to
/// supporting all homogeneous value-semantic operations, conversions
/// between comparable underlying value types is also supported.  Two
/// nullable objects with different underlying types compare equal if their
/// underlying types are comparable and either (1) both objects are null or
/// (2) the non-null values compare equal.  Attempts to copy construct, copy
/// assign, or compare incompatible values types will fail to compile.  The
/// `NullableAllocatedValue` template can be instantiated on an incomplete
/// type, but it cannot be instantiated on a type that overloads
/// `operator&`.
template <class TYPE>
class NullableAllocatedValue {

    enum { k_HAS_VALUE = 0 };
    // flag for checking if the value is present

    // DATA
    NullableAllocatedValue_PointerBitsPair<bslma::Allocator, 1>  d_allocator;
    union {
        TYPE                               *d_pointer_p;
        char                                d_buffer[sizeof(TYPE *)];
        } d_storage;

    // PRIVATE TYPES
#  ifndef BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
    // UNSPECIFIED BOOL

    /// This type is needed only in C++03 mode, where `explicit` conversion
    /// operators are not supported.  A `NullableAllocatedValue` is implicitly
    /// converted to `UnspecifiedBool` when used in `if` statements, but is not
    /// implicitly convertible to `bool`.
    typedef BloombergLP::bsls::UnspecifiedBool<NullableAllocatedValue>
                                                           UnspecifiedBoolUtil;
    typedef typename UnspecifiedBoolUtil::BoolType         UnspecifiedBool;
#  endif

    // PRIVATE CLASS METHODS

    /// Returns `true` if an object of the template parameter `TYPE` can be
    /// stored locally, instead of being allocated on the heap.
    static bool isLocal() BSLS_KEYWORD_NOEXCEPT;

    // PRIVATE ACCESSORS

    /// return a pointer to the held value.  If the value does not exist and
    /// the storage is local, then return a pointer to storage suitable for
    /// constructing the held value.  If the value does not exist and the
    /// storage is not local, return NULL.
    TYPE *getAddress();
    const TYPE *getAddress() const;

    // PRIVATE MANIPULATORS

    /// Clear the flag in the `d_allocator` field that indicates that this
    /// object does not hold a value.
    void clearHasValueFlag() BSLS_KEYWORD_NOEXCEPT;

    /// Set the flag in the `d_allocator` field that indicates that this
    /// object holds a value.
    void setHasValueFlag() BSLS_KEYWORD_NOEXCEPT;

    /// Set the value of the pointer to the held value to the specified
    /// `newPtr`.  The behavior is undefined if the storage is local.
    void setRemoteAddress(TYPE *newPtr);

    /// Efficiently exchange the value of this object with the value of the
    /// specified `other` object.  At least one of `this` or `other` is not
    /// empty, and the values are stored locally.  This method provides the
    /// no-throw exception-safety guarantee.  The behavior is undefined
    /// unless this object was created with the same allocator as `other`.
    void swapLocal(NullableAllocatedValue& other);

    /// Efficiently exchange the value of this object with the value of the
    /// specified `other` object.  At least one of `this` or `other` is not
    /// empty, and the values are stored remotely.  This method provides the
    /// no-throw exception-safety guarantee.  The behavior is undefined
    /// unless this object was created with the same allocator as `other`.
    void swapRemote(NullableAllocatedValue& other);


  public:
    // TYPES

    /// `ValueType` is an alias for the underlying `TYPE` upon which this
    /// template class is instantiated, and represents the type of the
    /// managed object.
    typedef TYPE ValueType;

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(NullableAllocatedValue,
                                   bslma::UsesBslmaAllocator);
    BSLMF_NESTED_TRAIT_DECLARATION(NullableAllocatedValue,
                                   bslmf::IsBitwiseMoveable);
    BSLMF_NESTED_TRAIT_DECLARATION(NullableAllocatedValue,
                                   bdlb::HasPrintMethod);

    // CREATORS

    /// Create a nullable object having the null value.  Use the currently
    /// installed default allocator to supply memory.
    NullableAllocatedValue();

    /// Create a nullable object that has the null value and that uses the
    /// mechanism of the specified `allocator` to supply memory.
    explicit NullableAllocatedValue(const bsl::allocator<char>& allocator);

    /// Create a nullable object having the null value.  Use the currently
    /// installed default allocator to supply memory.
    NullableAllocatedValue(const bsl::nullopt_t&);                  // IMPLICIT

    /// Create a nullable object that has the null value and that uses the
    /// mechanism of the specified `allocator` to supply memory.
    NullableAllocatedValue(const bsl::nullopt_t&,
                           const bsl::allocator<char>& allocator);

    /// Create a nullable object having the value of the specified
    /// `original` object.  Use the currently installed default allocator
    /// to supply memory.
    NullableAllocatedValue(const NullableAllocatedValue&  original);

    /// Create a nullable object having the value of the specified
    /// `original` object and that uses the mechanism of the specified
    /// `allocator` to supply memory.
    NullableAllocatedValue(const NullableAllocatedValue&  original,
                           const bsl::allocator<char>&    allocator);

    /// Create a nullable object having the specified `value`.  Use the
    /// currently installed default allocator to supply memory.
    NullableAllocatedValue(const TYPE& value);                      // IMPLICIT

    /// Create a nullable object having the specified `value` and that uses
    /// the mechanism of specified `allocator` to supply memory.
    NullableAllocatedValue(const TYPE&                    value,
                           const bsl::allocator<char>&    allocator);

    /// Destroy this object.
    ~NullableAllocatedValue();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs`, and return a
    /// reference providing modifiable access to this object.
    NullableAllocatedValue<TYPE>& operator=(const NullableAllocatedValue& rhs);

    /// Reset this object to the default constructed state (i.e., to have
    /// the null value).
    NullableAllocatedValue<TYPE>& operator=(const bsl::nullopt_t&);

    /// Assign to this object the value of the specified `rhs`, and return a
    /// reference providing modifiable access to the underlying `TYPE`
    /// object.
    NullableAllocatedValue<TYPE>& operator=(const TYPE& rhs);

    /// Return a pointer providing modifiable access to the underlying
    /// `TYPE` object.  The behavior is undefined if the object has no
    /// value.
    TYPE *operator->();

    /// Return a reference providing modifiable access to the underlying
    /// `TYPE` object.  The behavior is undefined if the object has no
    /// value.
    TYPE& operator*();

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    /// Assign to this object the value of the (template parameter) `TYPE`
    /// created in place using the specified `args` and return a reference
    /// providing modifiable access to the underlying `TYPE` object.  If
    /// this `optional` object already contains an object (`true ==
    /// hasValue()`), that object is destroyed before the new object is
    /// created.  Note that if the constructor of `TYPE` throws an exception
    /// this object is left in a disengaged state.
    template <class... ARGS>
    TYPE& emplace(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...);

#   if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    /// Assign to this object the value of the (template parameter) `TYPE`
    /// created in place using the specified `il` and specified `args` and
    /// return a reference providing modifiable access to the underlying
    /// `TYPE` object.  If this object already contains an object (`true ==
    /// hasValue()`), that object is destroyed before the new object is
    /// created.  Note that if the constructor of `TYPE` throws an exception
    /// this object is left in a disengaged state.
    template <class INIT_LIST_TYPE, class... ARGS>
    TYPE& emplace(std::initializer_list<INIT_LIST_TYPE>      il,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...);

#   endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#endif

    /// Assign to this object the value read from the specified input
    /// `stream` using the specified `version` format, and return a
    /// reference to `stream`.  If `stream` is initially invalid, this
    /// operation has no effect.  If `version` is not supported, this object
    /// is unaltered and `stream` is invalidated, but otherwise unmodified.
    /// If `version` is supported but `stream` becomes invalid during this
    /// operation, this object has an undefined, but valid, state.  Note
    /// that no version is read from `stream`.  See the `bslx` package-level
    /// documentation for more information on BDEX streaming of
    /// value-semantic types and containers.
    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);

    /// Assign to this object the specified `val`, and return a reference
    /// providing modifiable access to the underlying `TYPE` object.
    TYPE& makeValue(const TYPE& val);

    /// Assign to this object the default value for `TYPE`, and return a
    /// reference providing modifiable access to the underlying `TYPE`
    /// object.
    TYPE& makeValue();

    /// Reset this object to the default constructed state (i.e., to have
    /// the null value).
    void reset();

    /// Efficiently exchange the value of this object with the value of the
    /// specified `other` object.  This method provides the no-throw
    /// exception-safety guarantee.  The behavior is undefined unless this
    /// object was created with the same allocator as `other`.
    void swap(NullableAllocatedValue& other);

    /// Return a reference providing modifiable access to the underlying
    /// `TYPE` object.  The behavior is undefined unless this object is
    /// non-null.
    TYPE& value();

    // ACCESSORS

    /// Write the value of this object, using the specified `version`
    /// format, to the specified output `stream`, and return a reference to
    /// `stream`.  If `stream` is initially invalid, this operation has no
    /// effect.  If `version` is not supported, `stream` is invalidated, but
    /// otherwise unmodified.  Note that `version` is not written to
    /// `stream`.  See the `bslx` package-level documentation for more
    /// information on BDEX streaming of value-semantic types and
    /// containers.
    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;

    /// Return a `bsl::allocator` constructed from the `bslma::Allocator`
    /// used by this object to supply memory.  Note that if no allocator was
    /// supplied at construction the default allocator in effect at
    /// construction is used.
    bsl::allocator<char> get_allocator() const;

    /// Return `true` if this object contains a value, and `false`
    /// otherwise.
    bool has_value() const BSLS_KEYWORD_NOEXCEPT;

    /// Return `false` if this object contains a value, and `true`
    /// otherwise.  Note that this is the opposite of `has_value`.
    bool isNull() const BSLS_KEYWORD_NOEXCEPT;

    /// Return the maximum valid BDEX format version, as indicated by the
    /// specified `versionSelector`, to be passed to the `bdexStreamOut`
    /// method.  Note that it is highly recommended that `versionSelector`
    /// be formatted as "YYYYMMDD", a date representation.  Also note that
    /// `versionSelector` should be a *compile*-time-chosen value that
    /// selects a format version supported by both externalizer and
    /// unexternalizer.  See the `bslx` package-level documentation for more
    /// information on BDEX streaming of value-semantic types and
    /// containers.
    int maxSupportedBdexVersion(int versionSelector) const;

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    /// Return the most current BDEX streaming version number supported by
    /// this class.  (See the package-group-level documentation for more
    /// information on BDEX streaming of container types.)
    int maxSupportedBdexVersion() const;
#endif  // BDE_OMIT_INTERNAL_DEPRECATED

    /// Return the value of the underlying object of a (template parameter)
    /// `TYPE` if this object is non-null, and the specified `default_value`
    /// otherwise.  Note that this method returns *by* *value*, so may be
    /// inefficient in some contexts.
    template <class ANY_TYPE>
    TYPE value_or(const ANY_TYPE& default_value) const;

    /// Return a pointer providing non-modifiable access to the underlying
    /// `TYPE` object.  The behavior is undefined if the object has no
    /// value.
    const TYPE *operator->() const;

    /// Return a reference providing non-modifiable access to the underlying
    /// `TYPE` object.  The behavior is undefined if the object has no
    /// value.
    const TYPE& operator*() const;


#  ifdef BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
    /// Return `true` if this object is contains a value, and `true`
    /// otherwise.
    BSLS_KEYWORD_EXPLICIT operator bool() const BSLS_KEYWORD_NOEXCEPT;
#  else  // BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
    /// Simulation of explicit conversion to bool.  Inlined to work around xlC
    /// bug when out-of-line.
    operator UnspecifiedBool() const BSLS_NOTHROW_SPEC
    {
        return UnspecifiedBoolUtil::makeValue(has_value());
    }
#  endif  // BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT else

                              // Aspects

    /// Return the allocator used by this object to supply memory.
    bslma::Allocator *allocator() const;

    /// Format this object to the specified output `stream` at the (absolute
    /// value of) the optionally specified indentation `level` and return a
    /// reference to `stream`.  If `level` is specified, optionally specify
    /// `spacesPerLevel`, the number of spaces per indentation level for
    /// this and all of its nested objects.  If `level` is negative,
    /// suppress indentation of the first line.  If `spacesPerLevel` is
    /// negative, format the entire output on one line, suppressing all but
    /// the initial indentation (as governed by `level`).  If `stream` is
    /// not valid on entry, this operation has no effect.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// Return a reference providing non-modifiable access to the underlying
    /// `TYPE` object.  The behavior is undefined unless this object is
    /// non-null.
    const TYPE& value() const;

    // DEPRECATED FUNCTIONS
    //  provided for compatibility with NullableValue

    /// Return an address providing non-modifiable access to the underlying
    /// object of a (template parameter) `TYPE` if this object is non-null,
    /// and the specified `address` otherwise.
    BSLS_DEPRECATE_FEATURE("bdl", "NullableAllocatedValue::addressOr",
                              "Use 'has_value() ? &value() : address' instead")
    const TYPE *addressOr(const TYPE *address) const;

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=5

    /// Assign to this nullable object the value of the (template parameter)
    /// `TYPE` created in place using the specified `args`.  Return a
    /// reference providing modifiable access to the created (value) object.
    /// The object is also accessible via the `value` method.  If this
    /// nullable object already contains an object (`false == isNull()`),
    /// that object is destroyed before the new object is created.  If
    /// `TYPE` has the trait `bslma::UsesBslmaAllocator` (`TYPE` is
    /// allocator-enabled) the allocator specified at the construction of
    /// this nullable object is used to supply memory to the value object.
    /// Attempts to explicitly specify via `args` another allocator to
    /// supply memory to the created (value) object are disallowed by the
    /// compiler.  Note that if the constructor of `TYPE` throws an
    /// exception this object is left in the null state.
    template <class... ARGS>
    BSLS_DEPRECATE_FEATURE("bdl", "NullableAllocatedValue::makeValueInplace",
                                                       "Use 'emplace' instead")
    TYPE& makeValueInplace(ARGS&&... args);
#endif

    /// Return the value of the underlying object of a (template parameter)
    /// `TYPE` if this object is non-null, and the specified `otherValue`
    /// otherwise.  Note that this method returns *by* *value*, so may be
    /// inefficient in some contexts.
    BSLS_DEPRECATE_FEATURE("bdl", "NullableAllocatedValue::valueOr",
                                                      "Use 'value_or' instead")
    TYPE valueOr(const TYPE& otherValue) const;

    /// Return an address providing non-modifiable access to the underlying
    /// object of a (template parameter) `TYPE` if this object is non-null,
    /// and 0 otherwise.
    BSLS_DEPRECATE_FEATURE("bdl", "NullableAllocatedValue::valueOrNull",
                                 "Use 'has_value() ? &value() : NULL' instead")
    const TYPE *valueOrNull() const;

};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` nullable objects have the
/// same value, and `false` otherwise.  Two nullable objects have the same
/// value if both are null, or if both are non-null and the values of their
/// underlying objects compare equal.  Note that this function will fail to
/// compile if `LHS_TYPE` and `RHS_TYPE` are not compatible.
template <class LHS_TYPE, class RHS_TYPE>
bool operator==(const NullableAllocatedValue<LHS_TYPE>& lhs,
                const NullableAllocatedValue<RHS_TYPE>& rhs);

/// Return `true` if the specified `lhs` and `rhs` objects have the same
/// value, and `false` otherwise.  A nullable object and a value of some
/// type have the same value if the nullable object is non-null and its
/// underlying value compares equal to the other value.  Note that this
/// function will fail to compile if `LHS_TYPE` and `RHS_TYPE` are not
/// compatible.
template <class LHS_TYPE, class RHS_TYPE>
bool operator==(const NullableAllocatedValue<LHS_TYPE>& lhs,
                const RHS_TYPE&                         rhs);
template <class LHS_TYPE, class RHS_TYPE>
bool operator==(const LHS_TYPE&                         lhs,
                const NullableAllocatedValue<RHS_TYPE>& rhs);

/// Return `true` if the specified `lhs` and `rhs` nullable objects do not
/// have the same value, and `false` otherwise.  Two nullable objects do not
/// have the same value if one is null and the other is non-null, or if both
/// are non-null and the values of their underlying objects do not compare
/// equal.  Note that this function will fail to compile if `LHS_TYPE` and
/// `RHS_TYPE` are not compatible.
template <class LHS_TYPE, class RHS_TYPE>
bool operator!=(const NullableAllocatedValue<LHS_TYPE>& lhs,
                const NullableAllocatedValue<RHS_TYPE>& rhs);

/// Return `true` if the specified `lhs` and `rhs` objects do not have the
/// same value, and `false` otherwise.  A nullable object and a value of
/// some type do not have the same value if either the nullable object is
/// null, or its underlying value does not compare equal to the other value.
/// Note that this function will fail to compile if `LHS_TYPE` and
/// `RHS_TYPE` are not compatible.
template <class LHS_TYPE, class RHS_TYPE>
bool operator!=(const NullableAllocatedValue<LHS_TYPE>& lhs,
                const RHS_TYPE&                         rhs);
template <class LHS_TYPE, class RHS_TYPE>
bool operator!=(const LHS_TYPE&                         lhs,
                const NullableAllocatedValue<RHS_TYPE>& rhs);

/// Return `true` if the specified `lhs` nullable object is ordered before
/// the specified `rhs` nullable object, and `false` otherwise.  `lhs` is
/// ordered before `rhs` if `lhs` is null and `rhs` is non-null or if both
/// are non-null and `lhs.value()` is ordered before `rhs.value()`.  Note
/// that this function will fail to compile if `LHS_TYPE` and `RHS_TYPE` are
/// not compatible.
template <class LHS_TYPE, class RHS_TYPE>
bool operator<(const NullableAllocatedValue<LHS_TYPE>& lhs,
               const NullableAllocatedValue<RHS_TYPE>& rhs);

/// Return `true` if the specified `lhs` nullable object is ordered before
/// the specified `rhs`, and `false` otherwise.  `lhs` is ordered before
/// `rhs` if `lhs` is null or `lhs.value()` is ordered before `rhs`.
template <class LHS_TYPE, class RHS_TYPE>
bool operator<(const NullableAllocatedValue<LHS_TYPE>& lhs,
               const RHS_TYPE&                         rhs);

/// Return `true` if the specified `lhs` is ordered before the specified
/// `rhs` nullable object, and `false` otherwise.  `lhs` is ordered before
/// `rhs` if `rhs` is not null and `lhs` is ordered before `rhs.value()`.
template <class LHS_TYPE, class RHS_TYPE>
bool operator<(const LHS_TYPE&                         lhs,
               const NullableAllocatedValue<RHS_TYPE>& rhs);


/// Return `true` if the specified `lhs` nullable object is ordered before
/// the specified `rhs` nullable object or `lhs` and `rhs` have the same
/// value, and `false` otherwise.  (See `operator<` and `operator==`.)  Note
/// that this operator returns `!(rhs < lhs)` when both operands are of
/// `NullableValue` type.  Also note that this function will fail to compile
/// if `LHS_TYPE` and `RHS_TYPE` are not compatible.
template <class LHS_TYPE, class RHS_TYPE>
bool operator<=(const NullableAllocatedValue<LHS_TYPE>& lhs,
                const NullableAllocatedValue<RHS_TYPE>& rhs);

/// Return `true` if the specified `lhs` nullable object is ordered before
/// the specified `rhs` or `lhs` and `rhs` have the same value, and `false`
/// otherwise.  (See `operator<` and `operator==`.)  Note that this operator
/// returns `!(rhs < lhs)`.
template <class LHS_TYPE, class RHS_TYPE>
bool operator<=(const NullableAllocatedValue<LHS_TYPE>& lhs,
                const RHS_TYPE&                         rhs);

/// Return `true` if the specified `lhs` is ordered before the specified
/// `rhs` nullable object or `lhs` and `rhs` have the same value, and
/// `false` otherwise.  (See `operator<` and `operator==`.)  Note that this
/// operator returns `!(rhs < lhs)`.
template <class LHS_TYPE, class RHS_TYPE>
bool operator<=(const LHS_TYPE&                         lhs,
                const NullableAllocatedValue<RHS_TYPE>& rhs);

/// Return `true` if the specified `lhs` nullable object is ordered after
/// the specified `rhs` nullable object, and `false` otherwise.  `lhs` is
/// ordered after `rhs` if `lhs` is non-null and `rhs` is null or if both
/// are non-null and `lhs.value()` is ordered after `rhs.value()`.  Note
/// that this operator returns `rhs < lhs` when both operands are of
/// `NullableValue` type.  Also note that this function will fail to compile
/// if `LHS_TYPE` and `RHS_TYPE` are not compatible.
template <class LHS_TYPE, class RHS_TYPE>
bool operator>(const NullableAllocatedValue<LHS_TYPE>& lhs,
               const NullableAllocatedValue<RHS_TYPE>& rhs);

/// Return `true` if the specified `lhs` nullable object is ordered after
/// the specified `rhs`, and `false` otherwise.  `lhs` is ordered after
/// `rhs` if `lhs` is not null and `lhs.value()` is ordered after `rhs`.
/// Note that this operator returns `rhs < lhs`.
template <class LHS_TYPE, class RHS_TYPE>
bool operator>(const NullableAllocatedValue<LHS_TYPE>& lhs,
               const RHS_TYPE&                         rhs);

/// Return `true` if the specified `lhs` is ordered after the specified
/// `rhs` nullable object, and `false` otherwise.  `lhs` is ordered after
/// `rhs` if `rhs` is null or `lhs` is ordered after `rhs.value()`.  Note
/// that this operator returns `rhs < lhs`.
template <class LHS_TYPE, class RHS_TYPE>
bool operator>(const LHS_TYPE&                         lhs,
               const NullableAllocatedValue<RHS_TYPE>& rhs);

/// Return `true` if the specified `lhs` nullable object is ordered after
/// the specified `rhs` nullable object or `lhs` and `rhs` have the same
/// value, and `false` otherwise.  (See `operator>` and `operator==`.)  Note
/// that this operator returns `!(lhs < rhs)` when both operands are of
/// `NullableValue` type.  Also note that this function will fail to compile
/// if `LHS_TYPE` and `RHS_TYPE` are not compatible.
template <class LHS_TYPE, class RHS_TYPE>
bool operator>=(const NullableAllocatedValue<LHS_TYPE>& lhs,
                const NullableAllocatedValue<RHS_TYPE>& rhs);

/// Return `true` if the specified `lhs` nullable object is ordered after
/// the specified `rhs` or `lhs` and `rhs` have the same value, and `false`
/// otherwise.  (See `operator>` and `operator==`.)  Note that this operator
/// returns `!(lhs < rhs)`.
template <class LHS_TYPE, class RHS_TYPE>
bool operator>=(const NullableAllocatedValue<LHS_TYPE>& lhs,
                const RHS_TYPE&                         rhs);

/// Return `true` if the specified `lhs` is ordered after the specified
/// `rhs` nullable object or `lhs` and `rhs` have the same value, and
/// `false` otherwise.  (See `operator>` and `operator==`.)  Note that this
/// operator returns `!(lhs < rhs)`.
template <class LHS_TYPE, class RHS_TYPE>
bool operator>=(const LHS_TYPE&                         lhs,
                const NullableAllocatedValue<RHS_TYPE>& rhs);

/// Write the value of the specified `object` to the specified output
/// `stream` in a single-line format, and return a reference to `stream`.
/// If `stream` is not valid on entry, this operation has no effect.  Note
/// that this human-readable format is not fully specified, can change
/// without notice, and is logically equivalent to:
/// ```
/// print(stream, 0, -1);
/// ```
template <class TYPE>
bsl::ostream& operator<<(bsl::ostream&                       stream,
                         const NullableAllocatedValue<TYPE>& object);

                  //================================
                  // Comparisons with bsl::nullopt_t
                  //================================

/// Return `true` if the specified `lhs` is null, and `false` otherwise.
template <class TYPE>
bool operator==(const NullableAllocatedValue<TYPE>& lhs, const bsl::nullopt_t&)
                                                         BSLS_KEYWORD_NOEXCEPT;

/// Return `true` if the specified `rhs` is null, and `false` otherwise.
template <class TYPE>
bool operator==(const bsl::nullopt_t&, const NullableAllocatedValue<TYPE>& rhs)
                                                         BSLS_KEYWORD_NOEXCEPT;

/// Return `true` if the specified `lhs` is not null, and `false` otherwise.
template <class TYPE>
bool operator!=(const NullableAllocatedValue<TYPE>& lhs, const bsl::nullopt_t&)
                                                         BSLS_KEYWORD_NOEXCEPT;

/// Return `true` if the specified `rhs` is not null, and `false`
/// otherwise.
template <class TYPE>
bool operator!=(const bsl::nullopt_t&, const NullableAllocatedValue<TYPE>& rhs)
                                                         BSLS_KEYWORD_NOEXCEPT;

/// Return `false`.  Note that `bsl::nullopt` never orders after a
/// `NullableAllocatedValue`.
template <class TYPE>
bool operator<(const NullableAllocatedValue<TYPE>&, const bsl::nullopt_t&)
                                                         BSLS_KEYWORD_NOEXCEPT;

/// Return `true` if the specified `rhs` is not null, and `false` otherwise.
/// Note that `bsl::nullopt` is ordered before any `NullableAllocatedValue`
/// that is not null.
template <class TYPE>
bool operator<(const bsl::nullopt_t&, const NullableAllocatedValue<TYPE>& rhs)
                                                         BSLS_KEYWORD_NOEXCEPT;

/// Return `true` if the specified `lhs` is not null, and `false`
/// otherwise.
template <class TYPE>
bool operator>(const NullableAllocatedValue<TYPE>& lhs, const bsl::nullopt_t&)
                                                         BSLS_KEYWORD_NOEXCEPT;

/// Return `false`.  Note that `bsl::nullopt` never orders after a
/// `NullableAllocatedValue`.
template <class TYPE>
bool operator>(const bsl::nullopt_t&, const NullableAllocatedValue<TYPE>&)
                                                         BSLS_KEYWORD_NOEXCEPT;

/// Return `true` if the specified `lhs` is null, and `false` otherwise.
template <class TYPE>
bool operator<=(const NullableAllocatedValue<TYPE>& lhs, const bsl::nullopt_t&)
                                                         BSLS_KEYWORD_NOEXCEPT;

/// Return `true`.
template <class TYPE>
bool operator<=(const bsl::nullopt_t&, const NullableAllocatedValue<TYPE>&)
                                                         BSLS_KEYWORD_NOEXCEPT;

/// Return `true`.
template <class TYPE>
bool operator>=(const NullableAllocatedValue<TYPE>&, const bsl::nullopt_t&)
                                                         BSLS_KEYWORD_NOEXCEPT;

/// Return `true` if the specified `rhs` is null, and `false` otherwise.
template <class TYPE>
bool operator>=(const bsl::nullopt_t&, const NullableAllocatedValue<TYPE>& rhs)
                                                         BSLS_KEYWORD_NOEXCEPT;

                  //===============================
                  // Comparisons with bsl::optional
                  //===============================

/// If neither of the specified `lhs` and `rhs` contain a value, return
/// `true`.  If one contains a value, and the other does not, return
/// `false`.  Otherwise, return `lhs.value == rhs.value()`.
template <class LHS_TYPE, class RHS_TYPE>
bool operator==(const NullableAllocatedValue<LHS_TYPE>& lhs,
                const bsl::optional<RHS_TYPE>&          rhs);
template <class LHS_TYPE, class RHS_TYPE>
bool operator==(const bsl::optional<LHS_TYPE>&          lhs,
                const NullableAllocatedValue<RHS_TYPE>& rhs);

/// If neither of the specified `lhs` and `rhs` contain a value, return
/// `false`.  If one contains a value, and the other does not, return
/// `true`.  Otherwise, return `lhs.value != rhs.value()`.
template <class LHS_TYPE, class RHS_TYPE>
bool operator!=(const NullableAllocatedValue<LHS_TYPE>& lhs,
                const bsl::optional<RHS_TYPE>&          rhs);
template <class LHS_TYPE, class RHS_TYPE>
bool operator!=(const bsl::optional<LHS_TYPE>&          lhs,
                const NullableAllocatedValue<RHS_TYPE>& rhs);

/// If neither of the specified `lhs` and `rhs` contain a value, return
/// `false`.  If `lhs` contains a value, and `rhs` does not, return `false`.
/// If `lhs` does not contains a value, `rhs` does, return `true`.
/// Otherwise, return `lhs.value < rhs.value()`.
template <class LHS_TYPE, class RHS_TYPE>
bool operator<(const NullableAllocatedValue<LHS_TYPE>& lhs,
               const bsl::optional<RHS_TYPE>&          rhs);

/// If neither of the specified `lhs` and `rhs` contain a value, return
/// `false`.  If `lhs` contains a value, and `rhs` does not, return `false`.
/// If `lhs` does not contains a value, `rhs` does, return `true`.
/// Otherwise, return `lhs.value < rhs.value()`.
template <class LHS_TYPE, class RHS_TYPE>
bool operator<(const bsl::optional<LHS_TYPE>&          lhs,
               const NullableAllocatedValue<RHS_TYPE>& rhs);

/// If neither of the specified `lhs` and `rhs` contain a value, return
/// `false`.  If `lhs` contains a value, and `rhs` does not, return `true`.
/// If `lhs` does not contains a value, `rhs` does, return `false`.
/// Otherwise, return `lhs.value > rhs.value()`.
template <class LHS_TYPE, class RHS_TYPE>
bool operator>(const NullableAllocatedValue<LHS_TYPE>& lhs,
               const bsl::optional<RHS_TYPE>&          rhs);

/// If neither of the specified `lhs` and `rhs` contain a value, return
/// `false`.  If `lhs` contains a value, and `rhs` does not, return `true`.
/// If `lhs` does not contains a value, `rhs` does, return `false`.
/// Otherwise, return `lhs.value > rhs.value()`.
template <class LHS_TYPE, class RHS_TYPE>
bool operator>(const bsl::optional<LHS_TYPE>&          lhs,
               const NullableAllocatedValue<RHS_TYPE>& rhs);

/// If neither of the specified `lhs` and `rhs` contain a value, return
/// `true`.  If `lhs` contains a value, and `rhs` does not, return `false`.
/// If `lhs` does not contains a value, `rhs` does, return `true`.
/// Otherwise, return `lhs.value <= rhs.value()`.
template <class LHS_TYPE, class RHS_TYPE>
bool operator<=(const NullableAllocatedValue<LHS_TYPE>& lhs,
                const bsl::optional<RHS_TYPE>&          rhs);

/// If neither of the specified `lhs` and `rhs` contain a value, return
/// `true`.  If `lhs` contains a value, and `rhs` does not, return `false`.
/// If `lhs` does not contains a value, `rhs` does, return `true`.
/// Otherwise, return `lhs.value <= rhs.value()`.
template <class LHS_TYPE, class RHS_TYPE>
bool operator<=(const bsl::optional<LHS_TYPE>&          lhs,
                const NullableAllocatedValue<RHS_TYPE>& rhs);

/// If neither of the specified `lhs` and `rhs` contain a value, return
/// `true`.  If `lhs` contains a value, and `rhs` does not, return `true`.
/// If `lhs` does not contains a value, `rhs` does, return `false`.
/// Otherwise, return `lhs.value >= rhs.value()`.
template <class LHS_TYPE, class RHS_TYPE>
bool operator>=(const NullableAllocatedValue<LHS_TYPE>& lhs,
                const bsl::optional<RHS_TYPE>&          rhs);

/// If neither of the specified `lhs` and `rhs` contain a value, return
/// `true`.  If `lhs` contains a value, and `rhs` does not, return `true`.
/// If `lhs` does not contains a value, `rhs` does, return `false`.
/// Otherwise, return `lhs.value >= rhs.value()`.
template <class LHS_TYPE, class RHS_TYPE>
bool operator>=(const bsl::optional<LHS_TYPE>&          lhs,
                const NullableAllocatedValue<RHS_TYPE>& rhs);

// FREE FUNCTIONS

/// Pass the boolean value of whether the specified `input` contains a value
/// to the specified `hashAlg` hashing algorithm of (template parameter)
/// type `HASHALG`.  If `input` contains a value, additionally pass that
/// value to `hashAlg`.
template <class HASHALG, class TYPE>
void hashAppend(HASHALG& hashAlg, const NullableAllocatedValue<TYPE>& input);

/// Exchange the values of the specified `a` and `b` objects.  This function
/// provides the no-throw exception-safety guarantee if the two objects were
/// created with the same allocator and the basic guarantee otherwise.
template <class TYPE>
void swap(NullableAllocatedValue<TYPE>& a,
          NullableAllocatedValue<TYPE>& b);

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                  // ----------------------------------
                  // class NullableAllocatedValue<TYPE>
                  // ----------------------------------

// CREATORS
template <class TYPE>
inline
NullableAllocatedValue<TYPE>::NullableAllocatedValue()
: d_allocator(bslma::Default::defaultAllocator())
{
    d_storage.d_pointer_p = 0;
}

template <class TYPE>
inline
NullableAllocatedValue<TYPE>
::NullableAllocatedValue(const bsl::allocator<char>& allocator)
: d_allocator(allocator.mechanism())
{
    d_storage.d_pointer_p = 0;
}

template <class TYPE>
inline
NullableAllocatedValue<TYPE>::NullableAllocatedValue(const bsl::nullopt_t&)
: d_allocator(bslma::Default::defaultAllocator())
{
    d_storage.d_pointer_p = 0;
}

template <class TYPE>
inline
NullableAllocatedValue<TYPE>
::NullableAllocatedValue(const bsl::nullopt_t&,
                         const bsl::allocator<char>& allocator)
: d_allocator(allocator.mechanism())
{
    d_storage.d_pointer_p = 0;
}

template <class TYPE>
inline
NullableAllocatedValue<TYPE>
::NullableAllocatedValue(const NullableAllocatedValue<TYPE>&  original)
: d_allocator(bslma::Default::defaultAllocator())
{
    if (original.has_value()) {
        makeValue(original.value());
    }
    else {
        d_storage.d_pointer_p = 0;
    }
}

template <class TYPE>
inline
NullableAllocatedValue<TYPE>
::NullableAllocatedValue(const NullableAllocatedValue<TYPE>&  original,
                         const bsl::allocator<char>&          allocator)
: d_allocator(allocator.mechanism())
{
    if (original.has_value()) {
        makeValue(original.value());
    }
    else {
        d_storage.d_pointer_p = 0;
    }
}

template <class TYPE>
inline
NullableAllocatedValue<TYPE>::NullableAllocatedValue(const TYPE& value)
: d_allocator(bslma::Default::defaultAllocator())
{
    makeValue(value);
}

template <class TYPE>
inline
NullableAllocatedValue<TYPE>::NullableAllocatedValue(
                                         const TYPE&                 value,
                                         const bsl::allocator<char>& allocator)
: d_allocator(allocator.mechanism())
{
    makeValue(value);
}

template <class TYPE>
inline
NullableAllocatedValue<TYPE>::~NullableAllocatedValue()
{
    reset();
}

// PRIVATE CLASS METHODS
template <class TYPE>
bool NullableAllocatedValue<TYPE>::isLocal() BSLS_KEYWORD_NOEXCEPT
{
//  we can store it locally if it will fit into a pointer, and it doesn't have
//  an exceptionally large alignment requirement.
    return (sizeof(TYPE) <= sizeof(TYPE *)) &&
           bslmf::IsBitwiseMoveable<TYPE>::value &&
           (static_cast<bsl::size_t>(bsls::AlignmentFromType<TYPE  >::VALUE) <=
            static_cast<bsl::size_t>(bsls::AlignmentFromType<TYPE *>::VALUE));
}

// MANIPULATORS
template <class TYPE>
inline
NullableAllocatedValue<TYPE>&
NullableAllocatedValue<TYPE>::operator=(
                                       const NullableAllocatedValue<TYPE>& rhs)
{
    if (rhs.has_value()) {
        makeValue(rhs.value());
    }
    else {
        reset();
    }

    return *this;
}

template <class TYPE>
inline
NullableAllocatedValue<TYPE>&
NullableAllocatedValue<TYPE>::operator=(const bsl::nullopt_t&)
{
    reset();
    return *this;
}

template <class TYPE>
inline
NullableAllocatedValue<TYPE>&
NullableAllocatedValue<TYPE>::operator=(const TYPE& rhs)
{
    makeValue(rhs);
    return *this;
}

template <class TYPE>
template <class STREAM>
STREAM& NullableAllocatedValue<TYPE>::bdexStreamIn(STREAM& stream, int version)
{
    using bslx::InStreamFunctions::bdexStreamIn;

    char isNullFlag = 0; // Redundant initialization to suppress -Werror.

    stream.getInt8(isNullFlag);

    if (stream) {
        if (!isNullFlag) {
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
TYPE& NullableAllocatedValue<TYPE>::makeValue(const TYPE& val)
{
    if (has_value()) {
        TYPE &v = value();
        v = val;
        return v;                                                     // RETURN
    }

    bslma::Allocator *alloc = allocator();
    if (isLocal()) {
        bslalg::ScalarPrimitives::copyConstruct(getAddress(), val, alloc);
    }
    else {
        TYPE *tmpPtr = reinterpret_cast<TYPE *>(alloc->allocate(sizeof(TYPE)));

        bslma::DeallocatorProctor<bslma::Allocator> proctor(tmpPtr, alloc);
        bslalg::ScalarPrimitives::copyConstruct(tmpPtr, val, alloc);
        proctor.release();
        setRemoteAddress(tmpPtr);
    }

    setHasValueFlag();
    return value();
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
    //      *d_value_p = TYPE(allocator());
    //      return *d_value_p;                                        // RETURN
    //  }
    //..

    bslma::Allocator *alloc = allocator();
    if (isLocal()) {
        bslalg::ScalarPrimitives::defaultConstruct(getAddress(), alloc);
        }
    else {
        TYPE *tmpPtr = reinterpret_cast<TYPE *>(alloc->allocate(sizeof(TYPE)));

        bslma::DeallocatorProctor<bslma::Allocator> proctor(tmpPtr, alloc);
        bslalg::ScalarPrimitives::defaultConstruct(tmpPtr, alloc);
        proctor.release();
        setRemoteAddress(tmpPtr);
    }

    setHasValueFlag();

    return value();
}

template <class TYPE>
inline
void NullableAllocatedValue<TYPE>::reset()
{
    if (has_value()) {
        TYPE *p = getAddress();
        BSLS_ASSERT(p);
        p->~TYPE();
        if (!isLocal()) {
            allocator()->deallocate(p);
        }
        clearHasValueFlag();
    }
}

template <class TYPE>
void NullableAllocatedValue<TYPE>::swap(NullableAllocatedValue& other)
{
    // Member 'swap' is undefined for non-equal allocators.

    BSLS_ASSERT(allocator() == other.allocator());

    // Nothing to do if both objects are null.
    if (isNull() && other.isNull()) {
        return;                                                       // RETURN
    }

    if (isLocal()) {
        swapLocal(other);
    }
    else {
        swapRemote(other);
    }
}

template <class TYPE>
inline
TYPE& NullableAllocatedValue<TYPE>::value()
{
    BSLS_ASSERT(has_value());
    return *getAddress();
}

template <class TYPE>
inline
TYPE *NullableAllocatedValue<TYPE>::operator->()
{
    BSLS_ASSERT(has_value());
    return getAddress();
}

template <class TYPE>
inline
TYPE& NullableAllocatedValue<TYPE>::operator*()
{
    BSLS_ASSERT(has_value());
    return value();
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class TYPE>
template <class... ARGS>
inline
TYPE& NullableAllocatedValue<TYPE>::emplace(
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args)
{
    bslma::Allocator *alloc = allocator();
    TYPE             *ptr = getAddress();
    // First, we get destruct the existing value - w/o releasing the storage
    if (has_value()) {
        BSLS_ASSERT(ptr);
        ptr->~TYPE();
        clearHasValueFlag();
    }
    else if (!isLocal()) {
        // Allocate some space for the object that we're creating. If this
        // throws, we have no cleanup to do; because the object is already
        // empty.
        ptr = reinterpret_cast<TYPE *>(alloc->allocate(sizeof(TYPE)));
        setRemoteAddress(ptr);
    }

    if (isLocal()) {
        BloombergLP::bslma::ConstructionUtil::construct(
                                 ptr,
                                 alloc,
                                 BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
    }
    else {
        bslma::DeallocatorProctor<bslma::Allocator> proctor(ptr, alloc);
        BloombergLP::bslma::ConstructionUtil::construct(
                                 ptr,
                                 alloc,
                                 BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
        proctor.release();
    }
    setHasValueFlag();
    return value();
}

#  if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class TYPE>
template <class INIT_LIST_TYPE, class... ARGS>
TYPE& NullableAllocatedValue<TYPE>::emplace(
                               std::initializer_list<INIT_LIST_TYPE>      il,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args)
{
    bslma::Allocator *alloc = allocator();
    TYPE             *ptr = getAddress();
    // First, we get destruct the existing value - w/o releasing the storage
    if (has_value()) {
        BSLS_ASSERT(ptr);
        ptr->~TYPE();
        clearHasValueFlag();
    }
    else if (!isLocal()) {
        // Allocate some space for the object that we're creating. If this
        // throws, we have no cleanup to do; because the object is already
        // empty.
        ptr = reinterpret_cast<TYPE *>(alloc->allocate(sizeof(TYPE)));
        setRemoteAddress(ptr);
    }

    if (isLocal()) {
        BloombergLP::bslma::ConstructionUtil::construct(
                                 ptr,
                                 alloc,
                                 il,
                                 BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
    }
    else {
        bslma::DeallocatorProctor<bslma::Allocator> proctor(ptr, alloc);
        BloombergLP::bslma::ConstructionUtil::construct(
                                 ptr,
                                 alloc,
                                 il,
                                 BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
        proctor.release();
    }
    setHasValueFlag();
    return value();
}
#  endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#endif

// PRIVATE ACCESSORS
template <class TYPE>
inline
TYPE *NullableAllocatedValue<TYPE>::getAddress() {
    if (isLocal()) {
        return reinterpret_cast<TYPE *>(
                           static_cast<void *>(d_storage.d_buffer));  // RETURN
        }
    else {
        return d_storage.d_pointer_p;                                 // RETURN
    }
}

template <class TYPE>
inline
const TYPE *NullableAllocatedValue<TYPE>::getAddress() const {
    if (isLocal()) {
        return reinterpret_cast<const TYPE *>(
                     static_cast<const void *>(d_storage.d_buffer));  // RETURN
        }
    else {
        return d_storage.d_pointer_p;                                 // RETURN
    }
}

// PRIVATE MANIPULATORS
template <class TYPE>
inline
void NullableAllocatedValue<TYPE>::clearHasValueFlag() BSLS_KEYWORD_NOEXCEPT
{
    d_allocator.clearFlag(k_HAS_VALUE);
}

template <class TYPE>
inline
void NullableAllocatedValue<TYPE>::setHasValueFlag() BSLS_KEYWORD_NOEXCEPT
{
    d_allocator.setFlag(k_HAS_VALUE);
}

template <class TYPE>
inline
void NullableAllocatedValue<TYPE>::setRemoteAddress(TYPE *newPtr) {
    BSLS_ASSERT(!isLocal());
    d_storage.d_pointer_p = newPtr;
}

template <class TYPE>
inline
void NullableAllocatedValue<TYPE>::swapLocal(NullableAllocatedValue& other)
{
//  At most one of 'isNull()' and 'other.isNull()' is true
//  Swapping the 'd_allocator's would be wrong here.
    if (isNull()) {
        makeValue(other.value());
        other.reset();
    }
    else if (other.isNull()) {
        other.makeValue(value());
        reset();
    }
    else {
        bslalg::SwapUtil::swap(getAddress(), other.getAddress());
    }
}

template <class TYPE>
inline
void NullableAllocatedValue<TYPE>::swapRemote(NullableAllocatedValue& other)
{
    bslalg::SwapUtil::swap(&d_allocator, &other.d_allocator);
    bslalg::SwapUtil::swap(&      d_storage.d_pointer_p,
                           &other.d_storage.d_pointer_p);
}

// ACCESSORS
template <class TYPE>
template <class STREAM>
STREAM& NullableAllocatedValue<TYPE>::bdexStreamOut(STREAM& stream,
                                                    int     version) const
{
    using bslx::OutStreamFunctions::bdexStreamOut;

    stream.putInt8(has_value() ? 0 : 1);

    if (has_value()) {
        bdexStreamOut(stream, value(), version);
    }

    return stream;
}

template <class TYPE>
inline
bsl::allocator<char> NullableAllocatedValue<TYPE>::get_allocator() const
{
    return allocator();
}


template <class TYPE>
inline
bool NullableAllocatedValue<TYPE>::has_value() const BSLS_KEYWORD_NOEXCEPT
{
    return d_allocator.readFlag(k_HAS_VALUE);
}

template <class TYPE>
inline
bool NullableAllocatedValue<TYPE>::isNull() const BSLS_KEYWORD_NOEXCEPT
{
    return !has_value();
}

# ifdef BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
template <class TYPE>
NullableAllocatedValue<TYPE>::operator bool() const BSLS_KEYWORD_NOEXCEPT
{
    return has_value();
}
# endif  // BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT

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

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
template <class TYPE>
inline
int NullableAllocatedValue<TYPE>::maxSupportedBdexVersion() const
{
    return maxSupportedBdexVersion(0);
}
#endif  // BDE_OMIT_INTERNAL_DEPRECATED

template <class TYPE>
template <class ANY_TYPE>
inline TYPE
NullableAllocatedValue<TYPE>::value_or(const ANY_TYPE& default_value) const
{
    return has_value() ? value() : static_cast<TYPE>(default_value);
}

template <class TYPE>
inline
const TYPE *NullableAllocatedValue<TYPE>::operator->() const
{
    BSLS_ASSERT(has_value());
    return &value();
}

template <class TYPE>
inline
const TYPE& NullableAllocatedValue<TYPE>::operator*() const
{
    BSLS_ASSERT(has_value());
    return value();
}

                                  // Aspects

template <class TYPE>
inline
bslma::Allocator *NullableAllocatedValue<TYPE>::allocator() const
{
    return d_allocator.getPointer();
}

template <class TYPE>
inline
bsl::ostream& NullableAllocatedValue<TYPE>::print(
                                            bsl::ostream& stream,
                                            int           level,
                                            int           spacesPerLevel) const
{
    if (!has_value()) {
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
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    // TBD: The assert below was commented out because a call to this function
    // is sometimes used as an argument to a template function that only looks
    // at the value type (and does not access the value).

    // BSLS_REVIEW(has_value());
#else
    BSLS_ASSERT(has_value());
#endif

    return *getAddress();
}

// DEPRECATED FUNCTIONS

template <class TYPE>
inline
const TYPE *NullableAllocatedValue<TYPE>::addressOr(const TYPE *address) const
{
    return has_value() ? &value() : address;
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=5
template <class TYPE>
template <class... ARGS>
inline
TYPE& NullableAllocatedValue<TYPE>::makeValueInplace(ARGS&&... args)
{
    return emplace(BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
}
#endif

template <class TYPE>
inline
TYPE NullableAllocatedValue<TYPE>::valueOr(const TYPE& otherValue) const
{
    return has_value() ? value() : otherValue;
}

template <class TYPE>
inline
const TYPE *NullableAllocatedValue<TYPE>::valueOrNull() const {
    return has_value() ? &value() : NULL;
}

}  // close package namespace

// FREE OPERATORS
template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator==(const NullableAllocatedValue<LHS_TYPE>& lhs,
                      const NullableAllocatedValue<RHS_TYPE>& rhs)
{
    if (lhs.isNull()) {
        return rhs.isNull();                                          // RETURN
    }

    return rhs.isNull() ? false : lhs.value() == rhs.value();
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator==(const NullableAllocatedValue<LHS_TYPE>& lhs,
                      const RHS_TYPE&                         rhs)
{
    return lhs.isNull() ? false : lhs.value() == rhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator==(const LHS_TYPE&                         lhs,
                      const NullableAllocatedValue<RHS_TYPE>& rhs)
{
    return rhs.isNull() ? false : lhs == rhs.value();
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator!=(const NullableAllocatedValue<LHS_TYPE>& lhs,
                      const NullableAllocatedValue<RHS_TYPE>& rhs)
{
    if (lhs.isNull()) {
        return !rhs.isNull();                                         // RETURN
    }

    return rhs.isNull() ? true : lhs.value() != rhs.value();
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator!=(const NullableAllocatedValue<LHS_TYPE>& lhs,
                      const RHS_TYPE&                         rhs)
{
    return lhs.isNull() ? true : lhs.value() != rhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator!=(const LHS_TYPE&                         lhs,
                      const NullableAllocatedValue<RHS_TYPE>& rhs)
{
    return rhs.isNull() ? true : lhs != rhs.value();
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator<(const NullableAllocatedValue<LHS_TYPE>& lhs,
                     const NullableAllocatedValue<RHS_TYPE>& rhs)
{
    if (rhs.isNull()) {
        return false;                                                 // RETURN
    }

    return lhs.isNull() ? true : lhs.value() < rhs.value();
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator<(const NullableAllocatedValue<LHS_TYPE>& lhs,
                     const RHS_TYPE&                         rhs)
{
    return lhs.isNull() ? true : lhs.value() < rhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator<(const LHS_TYPE&                         lhs,
                     const NullableAllocatedValue<RHS_TYPE>& rhs)
{
    return rhs.isNull() ? false : lhs < rhs.value();
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator<=(const NullableAllocatedValue<LHS_TYPE>& lhs,
                      const NullableAllocatedValue<RHS_TYPE>& rhs)
{
    if (lhs.isNull()) {
        return true;                                                  // RETURN
    }

    return rhs.isNull() ? false : lhs.value() <= rhs.value();
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator<=(const NullableAllocatedValue<LHS_TYPE>& lhs,
                      const RHS_TYPE&                         rhs)
{
    return lhs.isNull() ? true : lhs.value() <= rhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator<=(const LHS_TYPE&                         lhs,
                      const NullableAllocatedValue<RHS_TYPE>& rhs)
{
    return rhs.isNull() ? false : lhs <= rhs.value();
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator>(const NullableAllocatedValue<LHS_TYPE>& lhs,
                     const NullableAllocatedValue<RHS_TYPE>& rhs)
{
    if (lhs.isNull()) {
        return false;                                                 // RETURN
    }

    return rhs.isNull() ? true : lhs.value() > rhs.value();
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator>(const NullableAllocatedValue<LHS_TYPE>& lhs,
                     const RHS_TYPE&                         rhs)
{
    return lhs.isNull() ? false : lhs.value() > rhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator>(const LHS_TYPE&                         lhs,
                     const NullableAllocatedValue<RHS_TYPE>& rhs)
{
    return rhs.isNull() ? true : lhs > rhs.value();
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator>=(const NullableAllocatedValue<LHS_TYPE>& lhs,
                      const NullableAllocatedValue<RHS_TYPE>& rhs)
{
    if (rhs.isNull()) {
        return true;                                                  // RETURN
    }

    return lhs.isNull() ? false : lhs.value() >= rhs.value();
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator>=(const NullableAllocatedValue<LHS_TYPE>& lhs,
                      const RHS_TYPE&                         rhs)
{
    return lhs.isNull() ? false : lhs.value() >= rhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator>=(const LHS_TYPE&                         lhs,
                      const NullableAllocatedValue<RHS_TYPE>& rhs)
{
    return rhs.isNull() ? true : lhs >= rhs.value();
}

template <class TYPE>
inline
bsl::ostream& bdlb::operator<<(bsl::ostream&                       stream,
                               const NullableAllocatedValue<TYPE>& object)
{
    return object.print(stream, 0, -1);
}

                  //--------------------------------
                  // Comparisons with bsl::nullopt_t
                  //--------------------------------

template <class TYPE>
inline
bool bdlb::operator==(const NullableAllocatedValue<TYPE>& lhs,
                      const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT
{
    return !lhs.has_value();
}

template <class TYPE>
inline
bool bdlb::operator==(
                 const bsl::nullopt_t&,
                 const NullableAllocatedValue<TYPE>& rhs) BSLS_KEYWORD_NOEXCEPT
{
    return !rhs.has_value();
}

template <class TYPE>
inline bool bdlb::operator!=(const NullableAllocatedValue<TYPE>& lhs,
                             const bsl::nullopt_t& ) BSLS_KEYWORD_NOEXCEPT
{
    return lhs.has_value();
}

template <class TYPE>
inline
bool bdlb::operator!=(
                 const bsl::nullopt_t&,
                 const NullableAllocatedValue<TYPE>& rhs) BSLS_KEYWORD_NOEXCEPT
{
    return rhs.has_value();
}

template <class TYPE>
inline
bool bdlb::operator<(const NullableAllocatedValue<TYPE>&,
                     const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT
{
    return false;
}

template <class TYPE>
inline
bool bdlb::operator<(
                 const bsl::nullopt_t&,
                 const NullableAllocatedValue<TYPE>& rhs) BSLS_KEYWORD_NOEXCEPT
{
    return rhs.has_value();
}

template <class TYPE>
inline
bool bdlb::operator>(const NullableAllocatedValue<TYPE>& lhs,
                     const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT
{
    return lhs.has_value();
}

template <class TYPE>
inline
bool bdlb::operator>(const bsl::nullopt_t&,
                     const NullableAllocatedValue<TYPE>&) BSLS_KEYWORD_NOEXCEPT
{
    return false;
}

template <class TYPE>
inline
bool bdlb::operator<=(const NullableAllocatedValue<TYPE>& lhs,
                      const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT
{
    return !lhs.has_value();
}

template <class TYPE>
inline
bool bdlb::operator<=(
                     const bsl::nullopt_t&,
                     const NullableAllocatedValue<TYPE>&) BSLS_KEYWORD_NOEXCEPT
{
    return true;
}

template <class TYPE>
inline
bool bdlb::operator>=(const NullableAllocatedValue<TYPE>&,
                      const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT
{
    return true;
}

template <class TYPE>
inline
bool bdlb::operator>=(
                 const bsl::nullopt_t&,
                 const NullableAllocatedValue<TYPE>& rhs) BSLS_KEYWORD_NOEXCEPT
{
    return !rhs.has_value();
}

                  //-------------------------------
                  // Comparisons with bsl::optional
                  //-------------------------------

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator==(const NullableAllocatedValue<LHS_TYPE>& lhs,
                      const bsl::optional<RHS_TYPE>&          rhs)
{
    if (lhs.has_value() != rhs.has_value())
        return false;                                                 // RETURN
    return lhs.has_value () ? lhs.value() == rhs.value () : true;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator==(const bsl::optional<LHS_TYPE>&          lhs,
                      const NullableAllocatedValue<RHS_TYPE>& rhs)
{
    if (lhs.has_value() != rhs.has_value())
        return false;                                                 // RETURN
    return lhs.has_value () ? lhs.value() == rhs.value () : true;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator!=(const NullableAllocatedValue<LHS_TYPE>& lhs,
                      const bsl::optional<RHS_TYPE>&          rhs)
{
    if (lhs.has_value() != rhs.has_value())
        return true;                                                  // RETURN
    return lhs.has_value () ? lhs.value() != rhs.value () : false;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator!=(const bsl::optional<LHS_TYPE>&          lhs,
                      const NullableAllocatedValue<RHS_TYPE>& rhs)
{
    if (lhs.has_value() != rhs.has_value())
        return true;                                                  // RETURN
    return lhs.has_value () ? lhs.value() != rhs.value () : false;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator<(const NullableAllocatedValue<LHS_TYPE>& lhs,
                     const bsl::optional<RHS_TYPE>&          rhs)
{
    if (lhs.has_value() != rhs.has_value())
        return rhs.has_value();                                       // RETURN
    return lhs.has_value () ? lhs.value() < rhs.value () : false;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator<(const bsl::optional<LHS_TYPE>&          lhs,
                     const NullableAllocatedValue<RHS_TYPE>& rhs)
{
    if (lhs.has_value() != rhs.has_value())
        return rhs.has_value();                                       // RETURN
    return lhs.has_value () ? lhs.value() < rhs.value () : false;
}


template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator>(const NullableAllocatedValue<LHS_TYPE>& lhs,
                     const bsl::optional<RHS_TYPE>&          rhs)
{
    if (lhs.has_value() != rhs.has_value())
        return lhs.has_value();                                       // RETURN
    return lhs.has_value () ? lhs.value() > rhs.value () : false;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator>(const bsl::optional<LHS_TYPE>&          lhs,
                     const NullableAllocatedValue<RHS_TYPE>& rhs)
{
    if (lhs.has_value() != rhs.has_value())
        return lhs.has_value();                                       // RETURN
    return lhs.has_value () ? lhs.value() > rhs.value () : false;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator<=(const NullableAllocatedValue<LHS_TYPE>& lhs,
                      const bsl::optional<RHS_TYPE>&          rhs)
{
    if (lhs.has_value() != rhs.has_value())
        return rhs.has_value();                                       // RETURN
    return lhs.has_value () ? lhs.value() <= rhs.value () : true;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator<=(const bsl::optional<LHS_TYPE>&          lhs,
                      const NullableAllocatedValue<RHS_TYPE>& rhs)
{
    if (lhs.has_value() != rhs.has_value())
        return rhs.has_value();                                       // RETURN
    return lhs.has_value () ? lhs.value() <= rhs.value () : true;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator>=(const NullableAllocatedValue<LHS_TYPE>& lhs,
                      const bsl::optional<RHS_TYPE>&          rhs)
{
    if (lhs.has_value() != rhs.has_value())
        return lhs.has_value();                                       // RETURN
    return lhs.has_value () ? lhs.value() >= rhs.value () : true;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator>=(const bsl::optional<LHS_TYPE>&          lhs,
                      const NullableAllocatedValue<RHS_TYPE>& rhs)
{
    if (lhs.has_value() != rhs.has_value())
        return lhs.has_value();                                       // RETURN
    return lhs.has_value () ? lhs.value() >= rhs.value () : true;
}


// FREE FUNCTIONS
template <class HASHALG, class TYPE>
void bdlb::hashAppend(HASHALG&                            hashAlg,
                      const NullableAllocatedValue<TYPE>& input)
{
    using ::BloombergLP::bslh::hashAppend;

    if (!input.isNull()) {
        hashAppend(hashAlg, true);
        hashAppend(hashAlg, input.value());
    }
    else {
        hashAppend(hashAlg, false);
    }
}

template <class TYPE>
void bdlb::swap(NullableAllocatedValue<TYPE>& a,
                NullableAllocatedValue<TYPE>& b)
{
    if (a.allocator() == b.allocator()) {
        a.swap(b);

        return;                                                       // RETURN
    }

    NullableAllocatedValue<TYPE> futureA(b, a.allocator());
    NullableAllocatedValue<TYPE> futureB(a, b.allocator());

    futureA.swap(a);
    futureB.swap(b);
}

}  // close enterprise namespace

#endif // End C++11 code

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
