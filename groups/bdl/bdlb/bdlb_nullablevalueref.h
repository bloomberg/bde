// bdlb_nullablevalueref.h                                            -*-C++-*-

#ifndef INCLUDED_BDLB_NULLABLEVALUEREF
#define INCLUDED_BDLB_NULLABLEVALUEREF

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: *Tech* *preview* for non-owning reference to nullable objects.
//
//@CLASSES:
//  NullableValueRef: template for modifiable reference to nullable object
//  ConstNullableValueRef: template for const reference to nullable object
//
//@SEE_ALSO: NullableAllocatedValue, bsl::optional, NullableValue
//
//@DESCRIPTION: *This* *component* *is* *a* *technology* *preview* and subject
// to change.  It is provided for evaluation purposes only and should not be
// used in a production setting.  It provides two template classes,
// `bdlb::NullableValueRef<TYPE>` and `bdlb::ConstNullableValueRef<TYPE>`, that
// hold a reference to a either a `bdlb::NullableValue` or a
// `bdlb::NullableAllocatedValue`, and provide a common interface for dealing
// with the referenced object.  Once constructed, the "ref" object is not
// `re-targetable`, i. e, it cannot be changed to refer to a different nullable
// object (the `target`).  However, in the case of `NullableValueRef`, the
// value that is contained by the target can be changed, cleared, created and
// so on.
//
// Neither a `NullableValueRef` nor a `ConstNullableValueRef` owns the target
// that it refers to.  They do not allocate the value upon construction; they
// store a pointer to an already existing nullable value.
//
///Ownership and Lifetime
///----------------------
// Since the NullableValueRef does not own the value that it refers to, the
// user must ensure that the value's lifetime does not end before the
// NullableValueRef.
//
///Usage
///-----
// The following snippets of code illustrate use of this component.
//
///Example 1: Migrating from NullableValue to NullableAllocatedValue
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have a data structure that contains optional data members that
// are implemented with `NullableValue`, and client code that uses them.  Now
// we want to change the data structure to use `NullableAllocatedValue`, but
// without requiring simultaneous changes to the client code.  If the client
// code uses `NullableValueRef` to access the optional values, the change will
// not require any source changes in the clients; a recompile is sufficient.
//
// Given the following functions:
// ```
// bsl::optional<int> &getOpt()
//     // Return a reference to a NullableValue for processing
// {
//     static bdlb::NullableValue<int> nv(23);
//     return nv;
// }
//
// bdlb::NullableAllocatedValue<int> &getNAV()
//     // Return a reference to a NullableAllocatedValue for processing
// {
//     static bdlb::NullableAllocatedValue<int> nav(34);
//     return nav;
// }
// ```
//  We can wrap both of these types into a wrapper, and then treat them
//  identically.
// ```
// bdlb::NullableValueRef<int> r1 = getOpt();
// bdlb::NullableValueRef<int> r2 = getNAV();
//
// assert(23 == r1.value());
// assert(34 == r2.value());
// ```

#include <bdlscm_version.h>

#include <bdlb_nullableallocatedvalue.h>  // NullableAllocatedValue

#include <bslmf_assert.h>
#include <bslmf_issame.h>
#include <bslmf_removecv.h>

#include <bsls_deprecate.h>
#include <bsls_deprecatefeature.h>

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_unspecifiedbool.h>

#include <bslstl_optional.h>              // bsl::optional, bsl::nullopt_t

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Include version that can be compiled with C++03
// Generated on Fri Sep 15 15:38:58 2023
// Command line: sim_cpp11_features.pl bdlb_nullablevalueref.h
# define COMPILING_BDLB_NULLABLEVALUEREF_H
# include <bdlb_nullablevalueref_cpp03.h>
# undef COMPILING_BDLB_NULLABLEVALUEREF_H
#else

namespace BloombergLP {
namespace bdlb {

template <class TYPE>
class ConstNullableValueRef;

                        // ============================
                        // class NullableValueRef<TYPE>
                        // ============================

/// This class is a wrapper for either a `bsl::optional` or
/// `NullableAllocatedValue`, and provides modifiable access to the wrapped
/// object.
template <class TYPE>
class NullableValueRef {

    // PRIVATE TYPES
#ifndef BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
    // UNSPECIFIED BOOL

    // This type is needed only in C++03 mode, where 'explicit' conversion
    // operators are not supported.  A 'NullableAllocatedValue' is implicitly
    // converted to 'UnspecifiedBool' when used in 'if' statements, but is not
    // implicitly convertible to 'bool'.
    typedef BloombergLP::bsls::UnspecifiedBool<NullableValueRef>
                                                           UnspecifiedBoolUtil;
    typedef typename UnspecifiedBoolUtil::BoolType         UnspecifiedBool;
#endif

    // DATA

    // the address of the target
    void *d_target_p;

    // `true` if the target is a specialization of `bsl::optional`, and
    // `false` it's a specialization of `NullableAllocatedValue`
    bool  d_isTargetOptional;

    // PRIVATE ACCESSORS

    /// Return a reference providing non-const access to the held
    /// `NullableAllocatedValue` object.  The behavior is undefined if the
    /// target is not a `NullableAllocatedValue`.
    NullableAllocatedValue<TYPE>& getNAV() const;

    /// Return a reference providing non-const access to the held
    /// `bsl::optional` object.  The behavior is undefined if the target is
    /// not a `bdlb::bsl::optional`.
    bsl::optional<TYPE>& getOpt() const;

    /// Return `true` if the target of this object is a `bsl::optional`, and
    /// `false` otherwise.
    bool hasOpt() const;

    // FRIENDS
    friend class ConstNullableValueRef<TYPE>;

    // TRAITS
    BSLMF_ASSERT((
                 bsl::is_same<TYPE,
                              typename bsl::remove_cvref<TYPE>::type>::value));
        // This class requires that 'TYPE' is not 'const'- or 'volatile'-
        // qualified, nor a reference.

  public:
    // TYPES

    /// `value_type` is an alias for the template parameter `TYPE`, and
    /// represents the type of the object managed by the wrapped nullable
    /// object.
    typedef TYPE value_type;

    // CREATORS

    /// Create a nullable object wrapper that refers to the specified `opt`
    /// object.  Note that the created wrapper does not own a copy of the
    /// underlying nullable value, but instead refers to it, and so the
    /// lifetime of `opt` must exceed the lifetime of the created wrapper.
    NullableValueRef(bsl::optional<TYPE>& opt);                     // IMPLICIT

    /// Create a nullable object wrapper that refers to the specified `opt`
    /// object.  Note that the created wrapper does not own a copy of the
    /// underlying nullable value, but instead refers to it, and so the
    /// lifetime of `opt` must exceed the lifetime of the created wrapper.
    NullableValueRef(NullableAllocatedValue<TYPE>& opt);            // IMPLICIT

    /// Create a nullable object wrapper that refers to the target object of
    /// the specified `original` wrapper.  Note that the created wrapper
    /// does not own a copy of the underlying nullable value, but instead
    /// refers to it, and so the lifetime of the target must exceed the
    /// lifetime of the created wrapper.
    NullableValueRef(const NullableValueRef& original);

    // ~NullableValueRef();
        // Destroy this object.  Note that this destructor is generated by the
        // compiler, and does not destroy the target.

    // ACCESSORS

    /// Return `true` if the target contains a value, and `false` otherwise.
    bool has_value() const BSLS_KEYWORD_NOEXCEPT;

    /// Return `false` if the target contains a value, and `true` otherwise.
    /// Note that this accessor is provided purely for compatibility with
    /// `NullableValue` and `NullableAllocatedValue`, and its use is
    /// discouraged in favor of `has_value`.
    bool isNull() const BSLS_KEYWORD_NOEXCEPT;

    /// Return a reference providing non-modifiable access to the underlying
    /// object of a (template parameter) `TYPE`.  The behavior is undefined
    /// if the target has no value.
    const value_type& value() const;

    /// Return a pointer providing non-modifiable access to the underlying
    /// `TYPE` object.  The behavior is undefined if the target has no
    /// value.
    const value_type *operator->() const;

    /// Return a reference providing non-modifiable access to the underlying
    /// `TYPE` object.  The behavior is undefined if the target has no
    /// value.
    const value_type& operator*() const;

#ifdef BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
    /// Return `true` if the target holds a value, and `false` otherwise.
    BSLS_KEYWORD_EXPLICIT operator bool() const BSLS_KEYWORD_NOEXCEPT;
#else
    // Simulation of explicit conversion to bool.  Inlined to work around xlC
    // but when out-of-line.
    operator UnspecifiedBool() const BSLS_NOTHROW_SPEC
    {
        return UnspecifiedBoolUtil::makeValue(has_value());
    }
#endif

    /// Return the value of the underlying object of a (template parameter)
    /// `TYPE` if the target is non-null, and the specified `default_value`
    /// otherwise.  Note that this method returns *by* *value*, so may be
    /// inefficient in some contexts.
    template <class ANY_TYPE>
    TYPE value_or(const ANY_TYPE& default_value) const;

    // MANIPULATORS
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    /// Assign to the target the value of the (template parameter) `TYPE`
    /// created in place using the specified `args` and return a reference
    /// providing modifiable access to the underlying `TYPE` object.  If
    /// this `optional` object already contains an object ('true ==
    /// hasValue()'), that object is destroyed before the new object is
    /// created.  Note that if the constructor of `TYPE` throws an exception
    /// this object is left in a disengaged state.
    template <class... ARGS>
    TYPE& emplace(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...);
#endif

    /// Return a pointer providing modifiable access to the underlying
    /// `TYPE` object.  The behavior is undefined if the target has no
    /// value.
    value_type *operator->();

    /// Return a reference providing modifiable access to the underlying
    /// `TYPE` object.  The behavior is undefined if the target has no
    /// value.
    value_type& operator*();

    /// Reset the target to the default constructed state (i.e., to have the
    /// null value).
    NullableValueRef<TYPE>& operator=(const bsl::nullopt_t&);

    /// Assign to the target the value of the specified `rhs`, and return a
    /// reference providing modifiable access to this object.
    NullableValueRef<TYPE>& operator=(const TYPE& rhs);

    /// Assign to the target the value of the specified `rhs`, and return a
    /// reference providing modifiable access to this object.
    NullableValueRef<TYPE>& operator=(const bsl::optional<TYPE>& rhs);

    /// Assign to the target the value of the specified `rhs`, and return a
    /// reference providing modifiable access to this object.
    NullableValueRef<TYPE>& operator=(
                                      const NullableAllocatedValue<TYPE>& rhs);

    /// Assign to the target the value of the specified `rhs`, and return a
    /// reference providing modifiable access to this object.
    NullableValueRef<TYPE>& operator=(const NullableValueRef& rhs);

    /// Reset the target to the default constructed state (i.e., to have the
    /// null value).
    void reset();

    /// Return a reference providing modifiable access to the underlying
    /// `TYPE` object.  The behavior is undefined unless this object is
    /// non-null.
    value_type& value();

    // DEPRECATED FUNCTIONS
    //  provided for compatibility with NullableValue
    BSLS_DEPRECATE_FEATURE("bdl", "NullableValueRef::addressOr",
                              "Use 'has_value() ? &value() : address' instead")
    /// Return an address providing non-modifiable access to the underlying
    /// object of a (template parameter) `TYPE` if this object is non-null,
    /// and the specified `address` otherwise.
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
    BSLS_DEPRECATE_FEATURE("bdl", "NullableValueRef::makeValueInplace",
                                                       "Use 'emplace' instead")
    TYPE& makeValueInplace(ARGS&&... args);
#endif

    BSLS_DEPRECATE_FEATURE("bdl", "NullableValueRef::valueOr",
                                                      "Use 'value_or' instead")
    /// Return the value of the underlying object of a (template parameter)
    /// `TYPE` if this object is non-null, and the specified `otherValue`
    /// otherwise.  Note that this method returns *by* *value*, so may be
    /// inefficient in some contexts.
    TYPE valueOr(const TYPE& otherValue) const;

    BSLS_DEPRECATE_FEATURE("bdl", "NullableValueRef::valueOrNull",
                                 "Use 'has_value() ? &value() : NULL' instead")
    /// Return an address providing non-modifiable access to the underlying
    /// object of a (template parameter) `TYPE` if this object is non-null,
    /// and 0 otherwise.
    const TYPE *valueOrNull() const;
};

// FREE FUNCTIONS

/// Pass the boolean value of whether the specified `input` references a
/// non-empty nullable value to the specified `hashAlg` hashing algorithm of
/// (template parameter) type `HASHALG`.  If `input.has_value` is true,
/// additionally pass the value to `hashAlg`.
template <class HASHALG, class TYPE>
void hashAppend(HASHALG& hashAlg, const NullableValueRef<TYPE>& input);

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` nullable wrappers have
/// the same value, and `false` otherwise.  Two nullable wrappers have the
/// same value if both targets are null, or if both are non-null and the
/// values of their underlying objects compare equal.  Note that this
/// function will fail to compile if `LHS_TYPE` and `RHS_TYPE` are not
/// compatible.
template <class LHS_TYPE, class RHS_TYPE>
bool operator==(const NullableValueRef<LHS_TYPE>& lhs,
                const NullableValueRef<RHS_TYPE>& rhs);

/// Return `true` if the specified `lhs` and `rhs` objects have the same
/// value, and `false` otherwise.  A nullable wrapper and a value of some
/// type have the same value if the nullable wrapper is non-null and the
/// underlying value of it's target compares equal to the other value.  Note
/// that this function will fail to compile if `LHS_TYPE` and `RHS_TYPE` are
/// not compatible.
template <class LHS_TYPE, class RHS_TYPE>
bool operator==(const NullableValueRef<LHS_TYPE>& lhs,
                const RHS_TYPE&                   rhs);
template <class LHS_TYPE, class RHS_TYPE>
bool operator==(const LHS_TYPE&                   lhs,
                const NullableValueRef<RHS_TYPE>& rhs);

/// Return `true` if the specified `lhs` and `rhs` nullable objects do not
/// have the same value, and `false` otherwise.  Two nullable wrappers do
/// not have the same value if one is null and the other is non-null, or if
/// both are non-null and the values of their underlying objects do not
/// compare equal.  Note that this function will fail to compile if
/// `LHS_TYPE` and `RHS_TYPE` are not compatible.
template <class LHS_TYPE, class RHS_TYPE>
bool operator!=(const NullableValueRef<LHS_TYPE>& lhs,
                const NullableValueRef<RHS_TYPE>& rhs);

/// Return `true` if the specified `lhs` and `rhs` objects do not have the
/// same value, and `false` otherwise.  A nullable wrappers and a value of
/// some type do not have the same value if either the nullable wrappers is
/// null, or its underlying value does not compare equal to the other value.
/// Note that this function will fail to compile if `LHS_TYPE` and
/// `RHS_TYPE` are not compatible.
template <class LHS_TYPE, class RHS_TYPE>
bool operator!=(const NullableValueRef<LHS_TYPE>& lhs,
                const RHS_TYPE&                   rhs);
template <class LHS_TYPE, class RHS_TYPE>
bool operator!=(const LHS_TYPE&                   lhs,
                const NullableValueRef<RHS_TYPE>& rhs);

/// Return `true` if the specified `lhs` nullable wrapper is ordered before
/// the specified `rhs` nullable object, and `false` otherwise.  `lhs` is
/// ordered before `rhs` if `lhs` is null and `rhs` is non-null or if both
/// are non-null and `lhs.value()` is ordered before `rhs.value()`.  Note
/// that this function will fail to compile if `LHS_TYPE` and `RHS_TYPE` are
/// not compatible.
template <class LHS_TYPE, class RHS_TYPE>
bool operator<(const NullableValueRef<LHS_TYPE>& lhs,
               const NullableValueRef<RHS_TYPE>& rhs);

/// Return `true` if the specified `lhs` nullable wrapper is ordered before
/// the specified `rhs`, and `false` otherwise.  `lhs` is ordered before
/// `rhs` if `lhs` is null or `lhs.value()` is ordered before `rhs`.
template <class LHS_TYPE, class RHS_TYPE>
bool operator<(const NullableValueRef<LHS_TYPE>& lhs,
               const RHS_TYPE&                   rhs);

/// Return `true` if the specified `lhs` is ordered before the specified
/// `rhs` nullable wrappers, and `false` otherwise.  `lhs` is ordered before
/// `rhs` if `rhs` is not null and `lhs` is ordered before `rhs.value()`.
template <class LHS_TYPE, class RHS_TYPE>
bool operator<(const LHS_TYPE&                   lhs,
               const NullableValueRef<RHS_TYPE>& rhs);


/// Return `true` if the specified `lhs` nullable wrapper is ordered before
/// the specified `rhs` nullable object or `lhs` and `rhs` have the same
/// value, and `false` otherwise.  (See `operator<` and `operator==`.)  Note
/// that this function will fail to compile if `LHS_TYPE` and `RHS_TYPE`
/// are not compatible.
template <class LHS_TYPE, class RHS_TYPE>
bool operator<=(const NullableValueRef<LHS_TYPE>& lhs,
                const NullableValueRef<RHS_TYPE>& rhs);

/// Return `true` if the specified `lhs` nullable wrapper is ordered before
/// the specified `rhs` or `lhs` and `rhs` have the same value, and `false`
/// otherwise.  (See `operator<` and `operator==`.)  Note that this operator
/// returns `!(rhs < lhs)`.
template <class LHS_TYPE, class RHS_TYPE>
bool operator<=(const NullableValueRef<LHS_TYPE>& lhs,
                const RHS_TYPE&                   rhs);

/// Return `true` if the specified `lhs` is ordered before the specified
/// `rhs` nullable wrapper or `lhs` and `rhs` have the same value, and
/// `false` otherwise.  (See `operator<` and `operator==`.)  Note that this
/// operator returns `!(rhs < lhs)`.
template <class LHS_TYPE, class RHS_TYPE>
bool operator<=(const LHS_TYPE&                   lhs,
                const NullableValueRef<RHS_TYPE>& rhs);

/// Return `true` if the specified `lhs` nullable wrapper is ordered after
/// the specified `rhs` nullable object, and `false` otherwise.  `lhs` is
/// ordered after `rhs` if `lhs` is non-null and `rhs` is null or if both
/// are non-null and `lhs.value()` is ordered after `rhs.value()`.  Note
/// that this operator returns `rhs < lhs` when both operands are of
/// `bsl::optional` type.  Also note that this function will fail to compile
/// if `LHS_TYPE` and `RHS_TYPE` are not compatible.
template <class LHS_TYPE, class RHS_TYPE>
bool operator>(const NullableValueRef<LHS_TYPE>& lhs,
               const NullableValueRef<RHS_TYPE>& rhs);

/// Return `true` if the specified `lhs` nullable wrapper is ordered after
/// the specified `rhs`, and `false` otherwise.  `lhs` is ordered after
/// `rhs` if `lhs` is not null and `lhs.value()` is ordered after `rhs`.
/// Note that this operator returns `rhs < lhs`.
template <class LHS_TYPE, class RHS_TYPE>
bool operator>(const NullableValueRef<LHS_TYPE>& lhs,
               const RHS_TYPE&                   rhs);

/// Return `true` if the specified `lhs` is ordered after the specified
/// `rhs` nullable wrapper, and `false` otherwise.  `lhs` is ordered after
/// `rhs` if `rhs` is null or `lhs` is ordered after `rhs.value()`.  Note
/// that this operator returns `rhs < lhs`.
template <class LHS_TYPE, class RHS_TYPE>
bool operator>(const LHS_TYPE&                   lhs,
               const NullableValueRef<RHS_TYPE>& rhs);

/// Return `true` if the specified `lhs` nullable object is ordered after
/// the specified `rhs` nullable wrapper or `lhs` and `rhs` have the same
/// value, and `false` otherwise.  (See `operator>` and `operator==`.)  Note
/// that this operator returns `!(lhs < rhs)` when both operands are of
/// `bsl::optional` type.  Also note that this function will fail to compile
/// if `LHS_TYPE` and `RHS_TYPE` are not compatible.
template <class LHS_TYPE, class RHS_TYPE>
bool operator>=(const NullableValueRef<LHS_TYPE>& lhs,
                const NullableValueRef<RHS_TYPE>& rhs);

/// Return `true` if the specified `lhs` nullable wrapper is ordered after
/// the specified `rhs` or `lhs` and `rhs` have the same value, and `false`
/// otherwise.  (See `operator>` and `operator==`.)  Note that this operator
/// returns `!(lhs < rhs)`.
template <class LHS_TYPE, class RHS_TYPE>
bool operator>=(const NullableValueRef<LHS_TYPE>& lhs,
                const RHS_TYPE&                   rhs);

/// Return `true` if the specified `lhs` is wrapper after the specified
/// `rhs` nullable object or `lhs` and `rhs` have the same value, and
/// `false` otherwise.  (See `operator>` and `operator==`.)  Note that this
/// operator returns `!(lhs < rhs)`.
template <class LHS_TYPE, class RHS_TYPE>
bool operator>=(const LHS_TYPE&                   lhs,
                const NullableValueRef<RHS_TYPE>& rhs);


                  //================================
                  // Comparisons with bsl::nullopt_t
                  //================================

/// Return `true` if the specified `lhs` is null, and `false` otherwise.
template <class TYPE>
bool operator==(const NullableValueRef<TYPE>& lhs, const bsl::nullopt_t&)
                                                         BSLS_KEYWORD_NOEXCEPT;

/// Return `true` if the specified `rhs` is null, and `false` otherwise.
template <class TYPE>
bool operator==(const bsl::nullopt_t&, const NullableValueRef<TYPE>& rhs)
                                                         BSLS_KEYWORD_NOEXCEPT;

/// Return `true` if the specified `lhs` is not null, and `false` otherwise.
template <class TYPE>
bool operator!=(const NullableValueRef<TYPE>& lhs, const bsl::nullopt_t&)
                                                         BSLS_KEYWORD_NOEXCEPT;

/// Return `true` if the specified `rhs` is not null, and `false`
/// otherwise.
template <class TYPE>
bool operator!=(const bsl::nullopt_t&, const NullableValueRef<TYPE>& rhs)
                                                         BSLS_KEYWORD_NOEXCEPT;

/// Return `false`.  `bsl::nullopt` never orders after a
/// `NullableValueRef`.
template <class TYPE>
bool operator<(const NullableValueRef<TYPE>&, const bsl::nullopt_t&)
                                                         BSLS_KEYWORD_NOEXCEPT;

/// Return `true` if the specified `rhs` is not null, and `false` otherwise.
/// Note that `bsl::nullopt` is ordered before any `NullableValueRef` that
/// is not null.
template <class TYPE>
bool operator<(const bsl::nullopt_t&, const NullableValueRef<TYPE>& rhs)
                                                         BSLS_KEYWORD_NOEXCEPT;

/// Return `true` if the specified `lhs` is not null, and `false`
/// otherwise.
template <class TYPE>
bool operator>(const NullableValueRef<TYPE>& lhs, const bsl::nullopt_t&)
                                                         BSLS_KEYWORD_NOEXCEPT;

/// Return `false`.  `bsl::nullopt` never orders after a
/// `NullableValueRef`.
template <class TYPE>
bool operator>(const bsl::nullopt_t&, const NullableValueRef<TYPE>&)
                                                         BSLS_KEYWORD_NOEXCEPT;

/// Return `true` if the specified `lhs` is null, and `false` otherwise.
template <class TYPE>
bool operator<=(const NullableValueRef<TYPE>& lhs, const bsl::nullopt_t&)
                                                         BSLS_KEYWORD_NOEXCEPT;

/// Return `true`.
template <class TYPE>
bool operator<=(const bsl::nullopt_t&, const NullableValueRef<TYPE>&)
                                                         BSLS_KEYWORD_NOEXCEPT;

/// Return `true`.
template <class TYPE>
bool operator>=(const NullableValueRef<TYPE>&, const bsl::nullopt_t&)
                                                         BSLS_KEYWORD_NOEXCEPT;

/// Return `true` if the specified `rhs` is null, and `false` otherwise.
template <class TYPE>
bool operator>=(const bsl::nullopt_t&, const NullableValueRef<TYPE>& rhs)
                                                         BSLS_KEYWORD_NOEXCEPT;

                      // =================================
                      // class ConstNullableValueRef<TYPE>
                      // =================================

/// This class is a wrapper for either a `bsl::optional` or
/// `NullableAllocatedValue`, and provides non-modifiable access to the
/// wrapped object.
template <class TYPE>
class ConstNullableValueRef {

    // PRIVATE TYPES
#ifndef BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
    // UNSPECIFIED BOOL

    // This type is needed only in C++03 mode, where 'explicit' conversion
    // operators are not supported.  A 'NullableAllocatedValue' is implicitly
    // converted to 'UnspecifiedBool' when used in 'if' statements, but is not
    // implicitly convertible to 'bool'.
    typedef BloombergLP::bsls::UnspecifiedBool<ConstNullableValueRef>
                                                           UnspecifiedBoolUtil;
    typedef typename UnspecifiedBoolUtil::BoolType         UnspecifiedBool;
#endif

    // DATA

    // the address of the target
    const void *d_target_p;

    // `true` if the referent is a specialization of `bsl::optional`, and
    // `false` if it's a specialization of `NullableAllocatedValue`.
    bool        d_isTargetOptional;

    // PRIVATE ACCESSORS

    /// Return a pointer to the held `bdlb::NullableAllocatedValue`.  The
    /// behavior is undefined if the target is not a
    /// `bdlb::NullableAllocatedValue`.
    const NullableAllocatedValue<TYPE>& getNAV() const;

    /// Return a pointer to the held `bdlb::bsl::optional`. The behavior is
    /// undefined if the target is not a `bdlb::bsl::optional`.
    const bsl::optional<TYPE>& getOpt() const;

    /// Return `true` if this object currently holds a pointer to a
    /// bdlb::bsl::optional, and `false` otherwise.
    bool hasOpt() const;

    // TRAITS
    BSLMF_ASSERT((
                 bsl::is_same<TYPE,
                              typename bsl::remove_cvref<TYPE>::type>::value));
        // This class requires that 'TYPE' is not 'const'- or 'volatile'-
        // qualified, nor a reference.

  public:
    // TYPES

    /// `value_type` is an alias for the template parameter `TYPE`, and
    /// represents the type of the object managed by the wrapped nullable
    /// object.
    typedef TYPE value_type;

    // CREATORS

    /// Create a nullable object wrapper that refers to the specified `opt`
    /// object.  Note that the created wrapper does not own a copy of the
    /// underlying nullable value, but instead refers to it, and so the
    /// lifetime of `opt` must exceed the lifetime of the created wrapper.
    ConstNullableValueRef(const bsl::optional<TYPE>& opt);          // IMPLICIT

    ConstNullableValueRef(const NullableAllocatedValue<TYPE>& opt);
                                                                    // IMPLICIT
        // Create a nullable object wrapper that refers to the specified 'opt'
        // object.  Note that the created wrapper does not own a copy of the
        // underlying nullable value, but instead refers to it, and so the
        // lifetime of 'opt' must exceed the lifetime of the created wrapper.

    /// Create a nullable object wrapper that refers to the target object of
    /// the specified `ref` wrapper.  Note that the created wrapper does not
    /// own a copy of the underlying nullable value, but instead refers to
    /// to it, and so the lifetime of the target must exceed the lifetime of
    /// the created wrapper.
    ConstNullableValueRef(const NullableValueRef<TYPE>& ref);       // IMPLICIT

    /// Create a nullable object wrapper that refers to the target object of
    /// the specified `original` wrapper.  Note that the created wrapper
    /// does not own a copy of the underlying nullable value, but instead
    /// refers to it, and so the lifetime of the target must exceed the
    /// lifetime of the created wrapper.
    ConstNullableValueRef(const ConstNullableValueRef& original);

    // ~ConstNullableValueRef();
        // Destroy this object.  Note that this destructor is generated by the
        // compiler, and does not destruct the target.

    // ACCESSORS

    /// Return `true` if the target contains a value, and `false` otherwise.
    bool has_value() const BSLS_KEYWORD_NOEXCEPT;

    /// Return `false` if the target contains a value, and `true` otherwise.
    /// Note that this accessor is provided purely for compatibility with
    /// `NullableValue` and `NullableAllocatedValue`, and its use is
    /// discouraged in favor of `has_value`.
    bool isNull() const BSLS_KEYWORD_NOEXCEPT;

    /// Return a reference providing non-modifiable access to the underlying
    /// object of a (template parameter) `TYPE`.  The behavior is undefined
    /// if the target has no value.
    const value_type& value() const;

    /// Return a pointer providing non-modifiable access to the underlying
    /// `TYPE` object.  The behavior is undefined if the target has no
    /// value.
    const value_type *operator->() const;

    /// Return a reference providing non-modifiable access to the underlying
    /// `TYPE` object.  The behavior is undefined if the target has no
    /// value.
    const value_type& operator*() const;

#ifdef BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
    /// Return `true` if the target holds a value, and `false` otherwise.
    BSLS_KEYWORD_EXPLICIT operator bool() const BSLS_KEYWORD_NOEXCEPT;
#else
    // Simulation of explicit conversion to bool.  Inlined to work around xlC
    // but when out-of-line.
    operator UnspecifiedBool() const BSLS_NOTHROW_SPEC
    {
        return UnspecifiedBoolUtil::makeValue(has_value());
    }
#endif

    /// Return the value of the underlying object of a (template parameter)
    /// `TYPE` if the target is non-null, and the specified `default_value`
    /// otherwise.  Note that this method returns *by* *value*, so may be
    /// inefficient in some contexts.
    template <class ANY_TYPE>
    TYPE value_or(const ANY_TYPE& default_value) const;

    // DEPRECATED FUNCTIONS
    //  provided for compatibility with NullableValue
    BSLS_DEPRECATE_FEATURE("bdl", "ConstNullableValueRef::addressOr",
                              "Use 'has_value() ? &value() : address' instead")
    /// Return an address providing non-modifiable access to the underlying
    /// object of a (template parameter) `TYPE` if this object is non-null,
    /// and the specified `address` otherwise.
    const TYPE *addressOr(const TYPE *address) const;

    BSLS_DEPRECATE_FEATURE("bdl", "ConstNullableValueRef::valueOr",
                                                      "Use 'value_or' instead")
    /// Return the value of the underlying object of a (template parameter)
    /// `TYPE` if this object is non-null, and the specified `otherValue`
    /// otherwise.  Note that this method returns *by* *value*, so may be
    /// inefficient in some contexts.
    TYPE valueOr(const TYPE& otherValue) const;

    BSLS_DEPRECATE_FEATURE("bdl", "ConstNullableValueRef::valueOrNull",
                                 "Use 'has_value() ? &value() : NULL' instead")
    /// Return an address providing non-modifiable access to the underlying
    /// object of a (template parameter) `TYPE` if this object is non-null,
    /// and 0 otherwise.
    const TYPE *valueOrNull() const;

};

// FREE FUNCTIONS

/// Pass the boolean value of whether the specified `input` references a
/// non-empty nullable value to the specified `hashAlg` hashing algorithm of
/// (template parameter) type `HASHALG`.  If `input.has_value` is true,
/// additionally pass the value to `hashAlg`.
template <class HASHALG, class TYPE>
void hashAppend(HASHALG& hashAlg, const ConstNullableValueRef<TYPE>& input);

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` nullable wrappers have
/// the same value, and `false` otherwise.  Two nullable wrappers have the
/// same value if both targets are null, or if both are non-null and the
/// values of their underlying objects compare equal.  Note that this
/// function will fail to compile if `LHS_TYPE` and `RHS_TYPE` are not
/// compatible.
template <class LHS_TYPE, class RHS_TYPE>
bool operator==(const ConstNullableValueRef<LHS_TYPE>& lhs,
                const ConstNullableValueRef<RHS_TYPE>& rhs);

/// Return `true` if the specified `lhs` and `rhs` objects have the same
/// value, and `false` otherwise.  A nullable wrapper and a value of some
/// type have the same value if the nullable wrapper is non-null and the
/// underlying value of it's target compares equal to the other value.  Note
/// that this function will fail to compile if `LHS_TYPE` and `RHS_TYPE` are
/// not compatible.
template <class LHS_TYPE, class RHS_TYPE>
bool operator==(const ConstNullableValueRef<LHS_TYPE>& lhs,
                const RHS_TYPE&                        rhs);
template <class LHS_TYPE, class RHS_TYPE>
bool operator==(const LHS_TYPE&                        lhs,
                const ConstNullableValueRef<RHS_TYPE>& rhs);

/// Return `true` if the specified `lhs` and `rhs` nullable objects do not
/// have the same value, and `false` otherwise.  Two nullable wrappers do
/// not have the same value if one is null and the other is non-null, or if
/// bost are non-null and the values of their underlying objects do not
/// compare equal.  Note that this function will fail to compile if
/// `LHS_TYPE` and `RHS_TYPE` are not compatible.
template <class LHS_TYPE, class RHS_TYPE>
bool operator!=(const ConstNullableValueRef<LHS_TYPE>& lhs,
                const ConstNullableValueRef<RHS_TYPE>& rhs);

/// Return `true` if the specified `lhs` and `rhs` objects do not have the
/// same value, and `false` otherwise.  A nullable wrappers and a value of
/// some type do not have the same value if either the nullable wrappers is
/// null, or its underlying value does not compare equal to the other value.
/// Note that this function will fail to compile if `LHS_TYPE` and
/// `RHS_TYPE` are not compatible.
template <class LHS_TYPE, class RHS_TYPE>
bool operator!=(const ConstNullableValueRef<LHS_TYPE>& lhs,
                const RHS_TYPE&                        rhs);
template <class LHS_TYPE, class RHS_TYPE>
bool operator!=(const LHS_TYPE&                        lhs,
                const ConstNullableValueRef<RHS_TYPE>& rhs);

/// Return `true` if the specified `lhs` nullable wrapper is ordered before
/// the specified `rhs` nullable object, and `false` otherwise.  `lhs` is
/// ordered before `rhs` if `lhs` is null and `rhs` is non-null or if both
/// are non-null and `lhs.value()` is ordered before `rhs.value()`.  Note
/// that this function will fail to compile if `LHS_TYPE` and `RHS_TYPE` are
/// not compatible.
template <class LHS_TYPE, class RHS_TYPE>
bool operator<(const ConstNullableValueRef<LHS_TYPE>& lhs,
               const ConstNullableValueRef<RHS_TYPE>& rhs);

/// Return `true` if the specified `lhs` nullable wrapper is ordered before
/// the specified `rhs`, and `false` otherwise.  `lhs` is ordered before
/// `rhs` if `lhs` is null or `lhs.value()` is ordered before `rhs`.
template <class LHS_TYPE, class RHS_TYPE>
bool operator<(const ConstNullableValueRef<LHS_TYPE>& lhs,
               const RHS_TYPE&                        rhs);

/// Return `true` if the specified `lhs` is ordered before the specified
/// `rhs` nullable wrappers, and `false` otherwise.  `lhs` is ordered before
/// `rhs` if `rhs` is not null and `lhs` is ordered before `rhs.value()`.
template <class LHS_TYPE, class RHS_TYPE>
bool operator<(const LHS_TYPE&                        lhs,
               const ConstNullableValueRef<RHS_TYPE>& rhs);

/// Return `true` if the specified `lhs` nullable wrapper is ordered before
/// the specified `rhs` nullable object or `lhs` and `rhs` have the same
/// value, and `false` otherwise.  (See `operator<` and `operator==`.)  Note
/// that this function will fail to compile if `LHS_TYPE` and `RHS_TYPE`
/// are not compatible.
template <class LHS_TYPE, class RHS_TYPE>
bool operator<=(const ConstNullableValueRef<LHS_TYPE>& lhs,
                const ConstNullableValueRef<RHS_TYPE>& rhs);

/// Return `true` if the specified `lhs` nullable wrapper is ordered before
/// the specified `rhs` or `lhs` and `rhs` have the same value, and `false`
/// otherwise.  (See `operator<` and `operator==`.)  Note that this operator
/// returns `!(rhs < lhs)`.
template <class LHS_TYPE, class RHS_TYPE>
bool operator<=(const ConstNullableValueRef<LHS_TYPE>& lhs,
                const RHS_TYPE&                        rhs);

/// Return `true` if the specified `lhs` is ordered before the specified
/// `rhs` nullable wrapper or `lhs` and `rhs` have the same value, and
/// `false` otherwise.  (See `operator<` and `operator==`.)  Note that this
/// operator returns `!(rhs < lhs)`.
template <class LHS_TYPE, class RHS_TYPE>
bool operator<=(const LHS_TYPE&                        lhs,
                const ConstNullableValueRef<RHS_TYPE>& rhs);

/// Return `true` if the specified `lhs` nullable wrapper is ordered after
/// the specified `rhs` nullable object, and `false` otherwise.  `lhs` is
/// ordered after `rhs` if `lhs` is non-null and `rhs` is null or if both
/// are non-null and `lhs.value()` is ordered after `rhs.value()`.  Note
/// that this operator returns `rhs < lhs` when both operands are of
/// `NullableValue` type.  Also note that this function will fail to compile
/// if `LHS_TYPE` and `RHS_TYPE` are not compatible.
template <class LHS_TYPE, class RHS_TYPE>
bool operator>(const ConstNullableValueRef<LHS_TYPE>& lhs,
               const ConstNullableValueRef<RHS_TYPE>& rhs);

/// Return `true` if the specified `lhs` nullable wrapper is ordered after
/// the specified `rhs`, and `false` otherwise.  `lhs` is ordered after
/// `rhs` if `lhs` is not null and `lhs.value()` is ordered after `rhs`.
/// Note that this operator returns `rhs < lhs`.
template <class LHS_TYPE, class RHS_TYPE>
bool operator>(const ConstNullableValueRef<LHS_TYPE>& lhs,
               const RHS_TYPE&                        rhs);

/// Return `true` if the specified `lhs` is ordered after the specified
/// `rhs` nullable wrapper, and `false` otherwise.  `lhs` is ordered after
/// `rhs` if `rhs` is null or `lhs` is ordered after `rhs.value()`.  Note
/// that this operator returns `rhs < lhs`.
template <class LHS_TYPE, class RHS_TYPE>
bool operator>(const LHS_TYPE&                        lhs,
               const ConstNullableValueRef<RHS_TYPE>& rhs);

/// Return `true` if the specified `lhs` nullable object is ordered after
/// the specified `rhs` nullable wrapper or `lhs` and `rhs` have the same
/// value, and `false` otherwise.  (See `operator>` and `operator==`.)  Note
/// that this operator returns `!(lhs < rhs)` when both operands are of
/// `NullableValue` type.  Also note that this function will fail to compile
/// if `LHS_TYPE` and `RHS_TYPE` are not compatible.
template <class LHS_TYPE, class RHS_TYPE>
bool operator>=(const ConstNullableValueRef<LHS_TYPE>& lhs,
                const ConstNullableValueRef<RHS_TYPE>& rhs);

/// Return `true` if the specified `lhs` nullable wrapper is ordered after
/// the specified `rhs` or `lhs` and `rhs` have the same value, and `false`
/// otherwise.  (See `operator>` and `operator==`.)  Note that this operator
/// returns `!(lhs < rhs)`.
template <class LHS_TYPE, class RHS_TYPE>
bool operator>=(const ConstNullableValueRef<LHS_TYPE>& lhs,
                const RHS_TYPE&                        rhs);

/// Return `true` if the specified `lhs` is wrapper after the specified
/// `rhs` nullable object or `lhs` and `rhs` have the same value, and
/// `false` otherwise.  (See `operator>` and `operator==`.)  Note that this
/// operator returns `!(lhs < rhs)`.
template <class LHS_TYPE, class RHS_TYPE>
bool operator>=(const LHS_TYPE&                        lhs,
                const ConstNullableValueRef<RHS_TYPE>& rhs);

                       //================================
                       // Comparisons with bsl::nullopt_t
                       //================================

/// Return `true` if the specified `lhs` is null, and `false` otherwise.
template <class TYPE>
bool operator==(const ConstNullableValueRef<TYPE>& lhs,
                const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT;

/// Return `true` if the specified `rhs` is null, and `false` otherwise.
template <class TYPE>
bool operator==(const bsl::nullopt_t&,
                const ConstNullableValueRef<TYPE>& rhs) BSLS_KEYWORD_NOEXCEPT;

/// Return `true` if the specified `lhs` is not null, and `false` otherwise.
template <class TYPE>
bool operator!=(const ConstNullableValueRef<TYPE>& lhs,
                const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT;

/// Return `true` if the specified `rhs` is not null, and `false`
/// otherwise.
template <class TYPE>
bool operator!=(const bsl::nullopt_t&,
                const ConstNullableValueRef<TYPE>& rhs) BSLS_KEYWORD_NOEXCEPT;

/// Return `false`.  `bsl::nullopt` never orders after a
/// `ConstNullableValueRef`.
template <class TYPE>
bool operator<(const ConstNullableValueRef<TYPE>&, const bsl::nullopt_t&)
                                                         BSLS_KEYWORD_NOEXCEPT;

/// Return `true` if the specified `rhs` is not null, and `false` otherwise.
/// Note that `bsl::nullopt` is ordered before any `ConstNullableValueRef`
/// that is not null.
template <class TYPE>
bool operator<(const bsl::nullopt_t&,
               const ConstNullableValueRef<TYPE>& rhs) BSLS_KEYWORD_NOEXCEPT;

/// Return `true` if the specified `lhs` is not null, and `false`
/// otherwise.
template <class TYPE>
bool operator>(const ConstNullableValueRef<TYPE>& lhs,
               const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT;

/// Return `false`.  `bsl::nullopt` never orders after a
/// `NullableValueRef`.
template <class TYPE>
bool operator>(const bsl::nullopt_t&, const ConstNullableValueRef<TYPE>&)
                                                         BSLS_KEYWORD_NOEXCEPT;

/// Return `true` if the specified `lhs` is null, and `false` otherwise.
template <class TYPE>
bool operator<=(const ConstNullableValueRef<TYPE>& lhs,
                const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT;

/// Return `true`.
template <class TYPE>
bool operator<=(const bsl::nullopt_t&, const ConstNullableValueRef<TYPE>&)
                                                         BSLS_KEYWORD_NOEXCEPT;

/// Return `true`.
template <class TYPE>
bool operator>=(const ConstNullableValueRef<TYPE>&, const bsl::nullopt_t&)
                                                         BSLS_KEYWORD_NOEXCEPT;

/// Return `true` if the specified `rhs` is null, and `false` otherwise.
template <class TYPE>
bool operator>=(const bsl::nullopt_t&,
                const ConstNullableValueRef<TYPE>& rhs) BSLS_KEYWORD_NOEXCEPT;

     //===================================================================
     // Comparisons between 'ConstNullableValueRef' and 'NullableValueRef'
     //===================================================================

/// Return `true` if the specified `lhs` and `rhs` nullable wrappers have
/// the same value, and `false` otherwise.  Two nullable wrappers have the
/// same value if both targets are null, or if both are non-null and the
/// values of their underlying objects compare equal.  Note that this
/// function will fail to compile if `LHS_TYPE` and `RHS_TYPE` are not
/// compatible.
template <class LHS_TYPE, class RHS_TYPE>
bool operator==(const ConstNullableValueRef<LHS_TYPE>& lhs,
                const NullableValueRef<RHS_TYPE>&      rhs);
template <class LHS_TYPE, class RHS_TYPE>
bool operator==(const NullableValueRef<LHS_TYPE>&      lhs,
                const ConstNullableValueRef<RHS_TYPE>& rhs);

/// Return `true` if the specified `lhs` and `rhs` nullable objects do not
/// have the same value, and `false` otherwise.  Two nullable wrappers do
/// not have the same value if one is null and the other is non-null, or if
/// both are non-null and the values of their underlying objects do not
/// compare equal.  Note that this function will fail to compile if
/// `LHS_TYPE` and `RHS_TYPE` are not compatible.
template <class LHS_TYPE, class RHS_TYPE>
bool operator!=(const ConstNullableValueRef<LHS_TYPE>& lhs,
                const NullableValueRef<RHS_TYPE>&      rhs);
template <class LHS_TYPE, class RHS_TYPE>
bool operator!=(const NullableValueRef<LHS_TYPE>&      lhs,
                const ConstNullableValueRef<RHS_TYPE>& rhs);

/// Return `true` if the specified `lhs` nullable wrapper is ordered before
/// the specified `rhs` nullable object, and `false` otherwise.  `lhs` is
/// ordered before `rhs` if `lhs` is null and `rhs` is non-null or if both
/// are non-null and `lhs.value()` is ordered before `rhs.value()`.  Note
/// that this function will fail to compile if `LHS_TYPE` and `RHS_TYPE` are
/// not compatible.
template <class LHS_TYPE, class RHS_TYPE>
bool operator<(const ConstNullableValueRef<LHS_TYPE>& lhs,
               const NullableValueRef<RHS_TYPE>&      rhs);
template <class LHS_TYPE, class RHS_TYPE>
bool operator<(const NullableValueRef<LHS_TYPE>&      lhs,
               const ConstNullableValueRef<RHS_TYPE>& rhs);

/// Return `true` if the specified `lhs` nullable wrapper is ordered before
/// the specified `rhs` nullable object or `lhs` and `rhs` have the same
/// value, and `false` otherwise.  (See `operator<` and `operator==`.)  Note
/// that this function will fail to compile if `LHS_TYPE` and `RHS_TYPE`
/// are not compatible.
template <class LHS_TYPE, class RHS_TYPE>
bool operator<=(const ConstNullableValueRef<LHS_TYPE>& lhs,
                const NullableValueRef<RHS_TYPE>&      rhs);
template <class LHS_TYPE, class RHS_TYPE>
bool operator<=(const NullableValueRef<LHS_TYPE>&      lhs,
                const ConstNullableValueRef<RHS_TYPE>& rhs);

/// Return `true` if the specified `lhs` nullable wrapper is ordered after
/// the specified `rhs` nullable object, and `false` otherwise.  `lhs` is
/// ordered after `rhs` if `lhs` is non-null and `rhs` is null or if both
/// are non-null and `lhs.value()` is ordered after `rhs.value()`.  Note
/// that this operator returns `rhs < lhs` when both operands are of
/// `NullableValue` type.  Also note that this function will fail to compile
/// if `LHS_TYPE` and `RHS_TYPE` are not compatible.
template <class LHS_TYPE, class RHS_TYPE>
bool operator>(const ConstNullableValueRef<LHS_TYPE>& lhs,
               const NullableValueRef<RHS_TYPE>&      rhs);
template <class LHS_TYPE, class RHS_TYPE>
bool operator>(const NullableValueRef<LHS_TYPE>&      lhs,
               const ConstNullableValueRef<RHS_TYPE>& rhs);

/// Return `true` if the specified `lhs` nullable object is ordered after
/// the specified `rhs` nullable wrapper or `lhs` and `rhs` have the same
/// value, and `false` otherwise.  (See `operator>` and `operator==`.)  Note
/// that this operator returns `!(lhs < rhs)` when both operands are of
/// `NullableValue` type.  Also note that this function will fail to compile
/// if `LHS_TYPE` and `RHS_TYPE` are not compatible.
template <class LHS_TYPE, class RHS_TYPE>
bool operator>=(const ConstNullableValueRef<LHS_TYPE>& lhs,
                const NullableValueRef<RHS_TYPE>&      rhs);
template <class LHS_TYPE, class RHS_TYPE>
bool operator>=(const NullableValueRef<LHS_TYPE>&      lhs,
                const ConstNullableValueRef<RHS_TYPE>& rhs);

}  // close package namespace

                        // ----------------------------
                        // class NullableValueRef<TYPE>
                        // ----------------------------

// PRIVATE ACCESSORS
template <class TYPE>
inline
bdlb::NullableAllocatedValue<TYPE>&
bdlb::NullableValueRef<TYPE>::getNAV() const
{
    BSLS_ASSERT(!hasOpt());
    return *static_cast<bdlb::NullableAllocatedValue<TYPE> *>(d_target_p);
}

template <class TYPE>
inline
bsl::optional<TYPE>&
bdlb::NullableValueRef<TYPE>::getOpt() const
{
    BSLS_ASSERT(hasOpt());
    return *static_cast<bsl::optional<TYPE> *>(d_target_p);
}

template <class TYPE>
inline
bool bdlb::NullableValueRef<TYPE>::hasOpt() const
{
    return d_isTargetOptional;
}

// CREATORS
template <class TYPE>
inline
bdlb::NullableValueRef<TYPE>::NullableValueRef(bsl::optional<TYPE>& opt)
: d_target_p(&opt)
, d_isTargetOptional(true)
{
}

template <class TYPE>
inline
bdlb::NullableValueRef<TYPE>::NullableValueRef(
                                             NullableAllocatedValue<TYPE>& opt)
: d_target_p(&opt)
, d_isTargetOptional(false)
{
}

template <class TYPE>
inline
bdlb::NullableValueRef<TYPE>::NullableValueRef(
                                              const NullableValueRef& original)
: d_target_p(original.d_target_p)
, d_isTargetOptional(original.d_isTargetOptional)
{
}

// ACCESSORS
template <class TYPE>
inline
bool bdlb::NullableValueRef<TYPE>::has_value() const BSLS_KEYWORD_NOEXCEPT
{
    return hasOpt()
              ? getOpt().has_value()
              : getNAV().has_value();
}

template <class TYPE>
inline
bool bdlb::NullableValueRef<TYPE>::isNull() const BSLS_KEYWORD_NOEXCEPT
{
    return !has_value();
}

template <class TYPE>
inline
const typename bdlb::NullableValueRef<TYPE>::value_type&
bdlb::NullableValueRef<TYPE>::value() const
{
    return hasOpt()
              ? getOpt().value()
              : getNAV().value();
}


template <class TYPE>
inline
const typename bdlb::NullableValueRef<TYPE>::value_type *
bdlb::NullableValueRef<TYPE>::operator->() const
{
    return hasOpt()
              ? getOpt().operator->()
              : getNAV().operator->();
}

template <class TYPE>
inline
const typename bdlb::NullableValueRef<TYPE>::value_type&
bdlb::NullableValueRef<TYPE>::operator*() const
{
    return hasOpt()
              ? getOpt().operator*()
              : getNAV().operator*();
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
template <class TYPE>
bdlb::NullableValueRef<TYPE>::operator bool() const BSLS_KEYWORD_NOEXCEPT
{
    return has_value();
}
#endif

template <class TYPE>
template <class ANY_TYPE>
inline
TYPE bdlb::NullableValueRef<TYPE>::value_or(
                                           const ANY_TYPE& default_value) const
{
    if (has_value()) {
        return hasOpt()
                  ? getOpt().value()
                  : getNAV().value();                                 // RETURN
    }
    return default_value;
}


// MANIPULATORS
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class TYPE>
template <class... ARGS>
inline
TYPE& bdlb::NullableValueRef<TYPE>::emplace(
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args)
{
    return hasOpt()
              ? getOpt().emplace(BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...)
              : getNAV().emplace(BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
}
#endif

template <class TYPE>
inline
typename bdlb::NullableValueRef<TYPE>::value_type *
bdlb::NullableValueRef<TYPE>::operator->()
{
    return hasOpt()
              ? getOpt().operator->()
              : getNAV().operator->();
}

template <class TYPE>
inline
typename bdlb::NullableValueRef<TYPE>::value_type&
bdlb::NullableValueRef<TYPE>::operator*()
{
    return hasOpt()
              ? getOpt().operator*()
              : getNAV().operator*();
}

template <class TYPE>
inline
bdlb::NullableValueRef<TYPE>&
bdlb::NullableValueRef<TYPE>::operator=(const bsl::nullopt_t&)
{
    reset();
    return *this;
}

template <class TYPE>
inline
bdlb::NullableValueRef<TYPE>&
bdlb::NullableValueRef<TYPE>::operator=(const TYPE& rhs)
{
    if (hasOpt()) {
        getOpt() = rhs;
    }
    else {
        getNAV() = rhs;
    }

    return *this;
}

template <class TYPE>
inline
bdlb::NullableValueRef<TYPE>&
bdlb::NullableValueRef<TYPE>::operator=(const bsl::optional<TYPE>& rhs)
{
    if (rhs.has_value()) {
        *this = rhs.value();
    }
    else {
        reset();
    }
    return *this;
}

template <class TYPE>
inline
bdlb::NullableValueRef<TYPE>&
bdlb::NullableValueRef<TYPE>::operator=(
                                       const NullableAllocatedValue<TYPE>& rhs)
{
    if (rhs.has_value()) {
        *this = rhs.value();
    }
    else {
        reset();
    }

    return *this;
}

template <class TYPE>
inline
bdlb::NullableValueRef<TYPE>&
bdlb::NullableValueRef<TYPE>::operator=(const NullableValueRef& rhs)
{
    if (rhs.has_value()) {
        *this = rhs.value();
    }
    else {
        reset();
    }

    return *this;
}

template <class TYPE>
inline
void bdlb::NullableValueRef<TYPE>::reset()
{
    if (hasOpt()) {
        getOpt().reset();
    }
    else {
        getNAV().reset();
    }
}

template <class TYPE>
inline
typename bdlb::NullableValueRef<TYPE>::value_type&
bdlb::NullableValueRef<TYPE>::value()
{
    return hasOpt()
              ? getOpt().value()
              : getNAV().value();
}

// DEPRECATED FUNCTIONS
template <class TYPE>
inline
const TYPE *
bdlb::NullableValueRef<TYPE>::addressOr(const TYPE *address) const
{
    return has_value() ? &value() : address;
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=5
template <class TYPE>
template <class... ARGS>
inline
TYPE& bdlb::NullableValueRef<TYPE>::makeValueInplace(ARGS&&... args)
{
    return emplace(BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
}
#endif

template <class TYPE>
inline
TYPE bdlb::NullableValueRef<TYPE>::valueOr(const TYPE& otherValue) const
{
    return value_or(otherValue);
}

template <class TYPE>
inline
const TYPE *bdlb::NullableValueRef<TYPE>::valueOrNull() const
{
    return has_value() ? &value() : 0;
}

// FREE FUNCTIONS
template <class HASHALG, class TYPE>
void bdlb::hashAppend(HASHALG&                      hashAlg,
                      const NullableValueRef<TYPE>& input)
{
    using ::BloombergLP::bslh::hashAppend;

    if (!input.has_value()) {
        hashAppend(hashAlg, false);
    }
    else {
        hashAppend(hashAlg, true);
        hashAppend(hashAlg, input.value());
    }
}

// FREE OPERATORS
template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator==(const NullableValueRef<LHS_TYPE>& lhs,
                      const NullableValueRef<RHS_TYPE>& rhs)
{
    if (lhs.has_value()) {
        return rhs.has_value() ? lhs.value() == rhs.value() : false;  // RETURN
    }

    return !rhs.has_value();
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator==(const NullableValueRef<LHS_TYPE>& lhs,
                      const RHS_TYPE&                   rhs)
{
    return lhs.has_value() ? lhs.value() == rhs : false;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator==(const LHS_TYPE&                   lhs,
                      const NullableValueRef<RHS_TYPE>& rhs)
{
    return rhs.has_value() ? lhs == rhs.value(): false;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator!=(const NullableValueRef<LHS_TYPE>& lhs,
                      const NullableValueRef<RHS_TYPE>& rhs)
{
    if (lhs.has_value()) {
        return rhs.has_value() ? lhs.value() != rhs.value() : true;   // RETURN
    }

    return rhs.has_value();
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator!=(const NullableValueRef<LHS_TYPE>& lhs,
                      const RHS_TYPE&                   rhs)
{
    return lhs.has_value() ? lhs.value() != rhs : true;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator!=(const LHS_TYPE&                   lhs,
                      const NullableValueRef<RHS_TYPE>& rhs)
{
    return rhs.has_value() ? lhs != rhs.value() : true;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator<(const NullableValueRef<LHS_TYPE>& lhs,
                     const NullableValueRef<RHS_TYPE>& rhs)
{
    if (!rhs.has_value()) {
        return false;                                                 // RETURN
    }

    return lhs.has_value() ? lhs.value() < rhs.value() : true;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator<(const NullableValueRef<LHS_TYPE>& lhs,
                     const RHS_TYPE&                   rhs)
{
    return lhs.has_value() ? lhs.value() < rhs : true;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator<(const LHS_TYPE&                   lhs,
                     const NullableValueRef<RHS_TYPE>& rhs)
{
    return rhs.has_value() ? lhs < rhs.value() : false;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator<=(const NullableValueRef<LHS_TYPE>& lhs,
                      const NullableValueRef<RHS_TYPE>& rhs)
{
    if (!lhs.has_value()) {
        return true;                                                  // RETURN
    }

    return rhs.has_value() ? lhs.value() <= rhs.value() : false;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator<=(const NullableValueRef<LHS_TYPE>& lhs,
                      const RHS_TYPE&                   rhs)
{
    return lhs.has_value() ?  lhs.value() <= rhs : true;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator<=(const LHS_TYPE&                   lhs,
                      const NullableValueRef<RHS_TYPE>& rhs)
{
    return rhs.has_value() ? lhs <= rhs.value() : false;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator>(const NullableValueRef<LHS_TYPE>& lhs,
                     const NullableValueRef<RHS_TYPE>& rhs)
{
    if (!lhs.has_value()) {
        return false;                                                 // RETURN
    }

    return rhs.has_value() ? lhs.value() > rhs.value() : true;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator>(const NullableValueRef<LHS_TYPE>& lhs,
                     const RHS_TYPE&                   rhs)
{
    return lhs.has_value() ? lhs.value() > rhs : false;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator>(const LHS_TYPE&                   lhs,
                     const NullableValueRef<RHS_TYPE>& rhs)
{
    return rhs.has_value() ? lhs > rhs.value() : true;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator>=(const NullableValueRef<LHS_TYPE>& lhs,
                      const NullableValueRef<RHS_TYPE>& rhs)
{
    if (!rhs.has_value()) {
        return true;                                                  // RETURN
    }

    return lhs.has_value() ? lhs.value() >= rhs.value() : false;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator>=(const NullableValueRef<LHS_TYPE>& lhs,
                      const RHS_TYPE&                   rhs)
{
    return lhs.has_value() ? lhs.value() >= rhs : false;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator>=(const LHS_TYPE&                   lhs,
                      const NullableValueRef<RHS_TYPE>& rhs)
{
    return rhs.has_value() ? lhs >= rhs.value() : true;
}

                       //--------------------------------
                       // Comparisons with bsl::nullopt_t
                       //--------------------------------

template <class TYPE>
inline
bool bdlb::operator==(const NullableValueRef<TYPE>& lhs,
                      const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT
{
    return !lhs.has_value();
}

template <class TYPE>
inline
bool bdlb::operator==(
                   const bsl::nullopt_t&,
                   const NullableValueRef<TYPE>& rhs) BSLS_KEYWORD_NOEXCEPT
{
    return !rhs.has_value();
}

template <class TYPE>
inline bool bdlb::operator!=(const NullableValueRef<TYPE>& lhs,
                             const bsl::nullopt_t& ) BSLS_KEYWORD_NOEXCEPT
{
    return lhs.has_value();
}

template <class TYPE>
inline
bool bdlb::operator!=(
                   const bsl::nullopt_t&,
                   const NullableValueRef<TYPE>& rhs) BSLS_KEYWORD_NOEXCEPT
{
    return rhs.has_value();
}

template <class TYPE>
inline
bool bdlb::operator<(const NullableValueRef<TYPE>&,
                     const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT
{
    return false;
}

template <class TYPE>
inline
bool bdlb::operator<(
                   const bsl::nullopt_t&,
                   const NullableValueRef<TYPE>& rhs) BSLS_KEYWORD_NOEXCEPT
{
    return rhs.has_value();
}

template <class TYPE>
inline
bool bdlb::operator>(const NullableValueRef<TYPE>& lhs,
                     const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT
{
    return lhs.has_value();
}

template <class TYPE>
inline
bool bdlb::operator>(const bsl::nullopt_t&,
                     const NullableValueRef<TYPE>&) BSLS_KEYWORD_NOEXCEPT
{
    return false;
}

template <class TYPE>
inline
bool bdlb::operator<=(const NullableValueRef<TYPE>& lhs,
                      const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT
{
    return !lhs.has_value();
}

template <class TYPE>
inline
bool bdlb::operator<=(
                       const bsl::nullopt_t&,
                       const NullableValueRef<TYPE>&) BSLS_KEYWORD_NOEXCEPT
{
    return true;
}

template <class TYPE>
inline
bool bdlb::operator>=(const NullableValueRef<TYPE>&,
                      const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT
{
    return true;
}

template <class TYPE>
inline
bool bdlb::operator>=(
                   const bsl::nullopt_t&,
                   const NullableValueRef<TYPE>& rhs) BSLS_KEYWORD_NOEXCEPT
{
    return !rhs.has_value();
}

                      // ---------------------------------
                      // class ConstNullableValueRef<TYPE>
                      // ---------------------------------

// PRIVATE ACCESSORS
template <class TYPE>
inline
const bdlb::NullableAllocatedValue<TYPE>&
bdlb::ConstNullableValueRef<TYPE>::getNAV() const
{
    BSLS_ASSERT(!hasOpt());
    return *static_cast<const NullableAllocatedValue<value_type> *>(
                                                                   d_target_p);
}

template <class TYPE>
inline
const bsl::optional<TYPE>&
bdlb::ConstNullableValueRef<TYPE>::getOpt() const
{
    BSLS_ASSERT(hasOpt());
    return *static_cast<const bsl::optional<value_type> *>(d_target_p);
}

template <class TYPE>
inline
bool bdlb::ConstNullableValueRef<TYPE>::hasOpt() const
{
    return d_isTargetOptional;
}

// CREATORS
template <class TYPE>
inline
bdlb::ConstNullableValueRef<TYPE>::ConstNullableValueRef(
                                                const bsl::optional<TYPE>& opt)
: d_target_p(&opt)
, d_isTargetOptional(true)
{
}

template <class TYPE>
inline
bdlb::ConstNullableValueRef<TYPE>::ConstNullableValueRef(
                                       const NullableAllocatedValue<TYPE>& opt)
: d_target_p(&opt)
, d_isTargetOptional(false)
{
}

template <class TYPE>
inline
bdlb::ConstNullableValueRef<TYPE>::ConstNullableValueRef(
                                             const NullableValueRef<TYPE>& ref)
: d_target_p(ref.d_target_p)
, d_isTargetOptional(ref.d_isTargetOptional)
{
}

template <class TYPE>
inline
bdlb::ConstNullableValueRef<TYPE>::ConstNullableValueRef(
                                         const ConstNullableValueRef& original)
: d_target_p(original.d_target_p)
, d_isTargetOptional(original.d_isTargetOptional)
{
}

// ACCESSORS
template <class TYPE>
inline
bool
bdlb::ConstNullableValueRef<TYPE>::has_value() const BSLS_KEYWORD_NOEXCEPT
{
    return hasOpt()
              ? getOpt().has_value()
              : getNAV().has_value();
}

template <class TYPE>
inline
bool
bdlb::ConstNullableValueRef<TYPE>::isNull() const BSLS_KEYWORD_NOEXCEPT
{
    return !has_value();
}

template <class TYPE>
inline
const typename bdlb::ConstNullableValueRef<TYPE>::value_type&
bdlb::ConstNullableValueRef<TYPE>::value() const
{
    return hasOpt()
              ? getOpt().value()
              : getNAV().value();
}


template <class TYPE>
inline
const typename bdlb::ConstNullableValueRef<TYPE>::value_type *
bdlb::ConstNullableValueRef<TYPE>::operator->() const
{
    return hasOpt()
              ? getOpt().operator->()
              : getNAV().operator->();
}

template <class TYPE>
inline
const typename bdlb::ConstNullableValueRef<TYPE>::value_type&
bdlb::ConstNullableValueRef<TYPE>::operator*() const
{
    return hasOpt()
              ? getOpt().operator*()
              : getNAV().operator*();
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
template <class TYPE>
bdlb::ConstNullableValueRef<TYPE>::operator bool() const BSLS_KEYWORD_NOEXCEPT
{
    return has_value();
}
#endif

template <class TYPE>
template <class ANY_TYPE>
inline
TYPE bdlb::ConstNullableValueRef<TYPE>::value_or(
                                           const ANY_TYPE& default_value) const
{
    if (has_value()) {
        return hasOpt()
                  ? getOpt().value()
                  : getNAV().value();                                 // RETURN
    }
    return default_value;
}

// DEPRECATED FUNCTIONS
template <class TYPE>
inline
const TYPE *
bdlb::ConstNullableValueRef<TYPE>::addressOr(const TYPE *address) const
{
    return has_value() ? &value() : address;
}

template <class TYPE>
inline
TYPE
bdlb::ConstNullableValueRef<TYPE>::valueOr(const TYPE& otherValue) const
{
    return value_or(otherValue);
}

template <class TYPE>
inline
const TYPE *bdlb::ConstNullableValueRef<TYPE>::valueOrNull() const
{
    return has_value() ? &value() : 0;
}

// FREE FUNCTIONS
template <class HASHALG, class TYPE>
void bdlb::hashAppend(HASHALG&                           hashAlg,
                      const ConstNullableValueRef<TYPE>& input)
{
    using ::BloombergLP::bslh::hashAppend;

    if (!input.has_value()) {
        hashAppend(hashAlg, false);
    }
    else {
        hashAppend(hashAlg, true);
        hashAppend(hashAlg, input.value());
    }
}

// FREE OPERATORS
template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator==(const ConstNullableValueRef<LHS_TYPE>& lhs,
                      const ConstNullableValueRef<RHS_TYPE>& rhs)
{
    if (lhs.has_value()) {
        return rhs.has_value() ? lhs.value() == rhs.value() : false;  // RETURN
    }

    return !rhs.has_value();
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator==(const ConstNullableValueRef<LHS_TYPE>& lhs,
                      const RHS_TYPE&                        rhs)
{
    return lhs.has_value() ? lhs.value() == rhs : false;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator==(const LHS_TYPE&                        lhs,
                      const ConstNullableValueRef<RHS_TYPE>& rhs)
{
    return rhs.has_value() ? lhs == rhs.value(): false;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator!=(const ConstNullableValueRef<LHS_TYPE>& lhs,
                      const ConstNullableValueRef<RHS_TYPE>& rhs)
{
    if (lhs.has_value()) {
        return rhs.has_value() ? lhs.value() != rhs.value() : true;   // RETURN
    }

    return rhs.has_value();
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator!=(const ConstNullableValueRef<LHS_TYPE>& lhs,
                      const RHS_TYPE&                        rhs)
{
    return lhs.has_value() ? lhs.value() != rhs : true;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator!=(const LHS_TYPE&                        lhs,
                      const ConstNullableValueRef<RHS_TYPE>& rhs)
{
    return rhs.has_value() ? lhs != rhs.value() : true;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator<(const ConstNullableValueRef<LHS_TYPE>& lhs,
                     const ConstNullableValueRef<RHS_TYPE>& rhs)
{
    if (!rhs.has_value()) {
        return false;                                                 // RETURN
    }

    return lhs.has_value() ? lhs.value() < rhs.value() : true;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator<(const ConstNullableValueRef<LHS_TYPE>& lhs,
                     const RHS_TYPE&                        rhs)
{
    return lhs.has_value() ? lhs.value() < rhs : true;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator<(const LHS_TYPE&                        lhs,
                     const ConstNullableValueRef<RHS_TYPE>& rhs)
{
    return rhs.has_value() ? lhs < rhs.value() : false;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator<=(const ConstNullableValueRef<LHS_TYPE>& lhs,
                      const ConstNullableValueRef<RHS_TYPE>& rhs)
{
    if (!lhs.has_value()) {
        return true;                                                  // RETURN
    }

    return rhs.has_value() ? lhs.value() <= rhs.value() : false;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator<=(const ConstNullableValueRef<LHS_TYPE>& lhs,
                      const RHS_TYPE&                        rhs)
{
    return lhs.has_value() ?  lhs.value() <= rhs : true;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator<=(const LHS_TYPE&                        lhs,
                      const ConstNullableValueRef<RHS_TYPE>& rhs)
{
    return rhs.has_value() ? lhs <= rhs.value() : false;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator>(const ConstNullableValueRef<LHS_TYPE>& lhs,
                     const ConstNullableValueRef<RHS_TYPE>& rhs)
{
    if (!lhs.has_value()) {
        return false;                                                 // RETURN
    }

    return rhs.has_value() ? lhs.value() > rhs.value() : true;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator>(const ConstNullableValueRef<LHS_TYPE>& lhs,
                     const RHS_TYPE&                        rhs)
{
    return lhs.has_value() ? lhs.value() > rhs : false;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator>(const LHS_TYPE&                        lhs,
                     const ConstNullableValueRef<RHS_TYPE>& rhs)
{
    return rhs.has_value() ? lhs > rhs.value() : true;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator>=(const ConstNullableValueRef<LHS_TYPE>& lhs,
                      const ConstNullableValueRef<RHS_TYPE>& rhs)
{
    if (!rhs.has_value()) {
        return true;                                                  // RETURN
    }

    return lhs.has_value() ? lhs.value() >= rhs.value() : false;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator>=(const ConstNullableValueRef<LHS_TYPE>& lhs,
                      const RHS_TYPE&                        rhs)
{
    return lhs.has_value() ? lhs.value() >= rhs : false;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator>=(const LHS_TYPE&                        lhs,
                      const ConstNullableValueRef<RHS_TYPE>& rhs)
{
    return rhs.has_value() ? lhs >= rhs.value() : true;
}

                  //--------------------------------
                  // Comparisons with bsl::nullopt_t
                  //--------------------------------

template <class TYPE>
inline
bool bdlb::operator==(const ConstNullableValueRef<TYPE>& lhs,
                      const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT
{
    return !lhs.has_value();
}

template <class TYPE>
inline
bool bdlb::operator==(
              const bsl::nullopt_t&,
              const ConstNullableValueRef<TYPE>& rhs) BSLS_KEYWORD_NOEXCEPT
{
    return !rhs.has_value();
}

template <class TYPE>
inline bool bdlb::operator!=(const ConstNullableValueRef<TYPE>& lhs,
                             const bsl::nullopt_t& ) BSLS_KEYWORD_NOEXCEPT
{
    return lhs.has_value();
}

template <class TYPE>
inline
bool bdlb::operator!=(
              const bsl::nullopt_t&,
              const ConstNullableValueRef<TYPE>& rhs) BSLS_KEYWORD_NOEXCEPT
{
    return rhs.has_value();
}

template <class TYPE>
inline
bool bdlb::operator<(const ConstNullableValueRef<TYPE>&,
                     const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT
{
    return false;
}

template <class TYPE>
inline
bool bdlb::operator<(
              const bsl::nullopt_t&,
              const ConstNullableValueRef<TYPE>& rhs) BSLS_KEYWORD_NOEXCEPT
{
    return rhs.has_value();
}

template <class TYPE>
inline
bool bdlb::operator>(const ConstNullableValueRef<TYPE>& lhs,
                     const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT
{
    return lhs.has_value();
}

template <class TYPE>
inline
bool bdlb::operator>(
              const bsl::nullopt_t&,
              const ConstNullableValueRef<TYPE>&) BSLS_KEYWORD_NOEXCEPT
{
    return false;
}

template <class TYPE>
inline
bool bdlb::operator<=(const ConstNullableValueRef<TYPE>& lhs,
                      const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT
{
    return !lhs.has_value();
}

template <class TYPE>
inline
bool bdlb::operator<=(
              const bsl::nullopt_t&,
              const ConstNullableValueRef<TYPE>&) BSLS_KEYWORD_NOEXCEPT
{
    return true;
}

template <class TYPE>
inline
bool bdlb::operator>=(const ConstNullableValueRef<TYPE>&,
                      const bsl::nullopt_t&) BSLS_KEYWORD_NOEXCEPT
{
    return true;
}

template <class TYPE>
inline
bool bdlb::operator>=(
              const bsl::nullopt_t&,
              const ConstNullableValueRef<TYPE>& rhs) BSLS_KEYWORD_NOEXCEPT
{
    return !rhs.has_value();
}

     //-------------------------------------------------------------------
     // Comparisons between 'ConstNullableValueRef' and 'NullableValueRef'
     //-------------------------------------------------------------------

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator==(const ConstNullableValueRef<LHS_TYPE>& lhs,
                      const NullableValueRef<RHS_TYPE>&      rhs)
{
    if (lhs.has_value()) {
        return rhs.has_value() ? lhs.value() == rhs.value() : false;  // RETURN
    }

    return !rhs.has_value();
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator==(const NullableValueRef<LHS_TYPE>&      lhs,
                      const ConstNullableValueRef<RHS_TYPE>& rhs)
{
    if (lhs.has_value()) {
        return rhs.has_value() ? lhs.value() == rhs.value() : false;  // RETURN
    }

    return !rhs.has_value();
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator!=(const ConstNullableValueRef<LHS_TYPE>& lhs,
                      const NullableValueRef<RHS_TYPE>&      rhs)
{
    if (lhs.has_value()) {
        return rhs.has_value() ? lhs.value() != rhs.value() : true;   // RETURN
    }

    return rhs.has_value();
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator!=(const NullableValueRef<LHS_TYPE>&      lhs,
                      const ConstNullableValueRef<RHS_TYPE>& rhs)
{
    if (lhs.has_value()) {
        return rhs.has_value() ? lhs.value() != rhs.value() : true;   // RETURN
    }

    return rhs.has_value();
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator<(const ConstNullableValueRef<LHS_TYPE>& lhs,
                     const NullableValueRef<RHS_TYPE>&      rhs)
{
    if (!rhs.has_value()) {
        return false;                                                 // RETURN
    }

    return lhs.has_value() ? lhs.value() < rhs.value() : true;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator<(const NullableValueRef<LHS_TYPE>&      lhs,
                     const ConstNullableValueRef<RHS_TYPE>& rhs)
{
    if (!rhs.has_value()) {
        return false;                                                 // RETURN
    }

    return lhs.has_value() ? lhs.value() < rhs.value() : true;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator<=(const ConstNullableValueRef<LHS_TYPE>& lhs,
                      const NullableValueRef<RHS_TYPE>&      rhs)
{
    if (!lhs.has_value()) {
        return true;                                                  // RETURN
    }

    return rhs.has_value() ? lhs.value() <= rhs.value() : false;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator<=(const NullableValueRef<LHS_TYPE>&      lhs,
                      const ConstNullableValueRef<RHS_TYPE>& rhs)
{
    if (!lhs.has_value()) {
        return true;                                                  // RETURN
    }

    return rhs.has_value() ? lhs.value() <= rhs.value() : false;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator>(const ConstNullableValueRef<LHS_TYPE>& lhs,
                     const NullableValueRef<RHS_TYPE>&      rhs)
{
    if (!lhs.has_value()) {
        return false;                                                 // RETURN
    }

    return rhs.has_value() ? lhs.value() > rhs.value() : true;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator>(const NullableValueRef<LHS_TYPE>&      lhs,
                     const ConstNullableValueRef<RHS_TYPE>& rhs)
{
    if (!lhs.has_value()) {
        return false;                                                 // RETURN
    }

    return rhs.has_value() ? lhs.value() > rhs.value() : true;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator>=(const ConstNullableValueRef<LHS_TYPE>& lhs,
                      const NullableValueRef<RHS_TYPE>&      rhs)
{
    if (!rhs.has_value()) {
        return true;                                                  // RETURN
    }

    return lhs.has_value() ? lhs.value() >= rhs.value() : false;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator>=(const NullableValueRef<LHS_TYPE>&      lhs,
                      const ConstNullableValueRef<RHS_TYPE>& rhs)
{
    if (!rhs.has_value()) {
        return true;                                                  // RETURN
    }

    return lhs.has_value() ? lhs.value() >= rhs.value() : false;
}

}  // close enterprise namespace

#endif // End C++11 code

#endif  // INCLUDED_BDLB_NULLABLEVALUEREF

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
