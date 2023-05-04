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
//@SEE_ALSO: bdlb_nullableallocatedvalue, bslstl_optional
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
///Conversion to 'bool': Explicit with C++11 but Implicit with C++03
///-----------------------------------------------------------------
// 'bdlb::NullableValue<TYPE>' provides a standard-compliant allocator-aware
// implementation of 'std::optional<TYPE>'.  Hence, 'bdlb::NullableValue<TYPE>'
// converts to 'bool', where the resulting Boolean value indicates whether the
// 'bdlb::NullableValue<TYPE>' object is "engaged" (see {'bslstl_optional'}).
// With C++11 and later, this conversion is explicit (per the C++ Standard) but
// the conversion is *implicit* with C++03 because 'explicit' conversion
// operators were not available until C++11.  Note that this implicit
// conversion on C++03 platforms is implemented using the "unspecified Boolean
// type" idiom.
//
// For example, consider the following code snippet where we assert behavior
// that holds with C++11 (and later), i.e., that there is not an *implicit*
// conversion from 'bdlb::NullableValue<double>' to 'bool':
//..
//  typedef bdlb::NullableValue<double> AnyNullableValue;
//
//  assert(!(bsl::is_convertible<AnyNullableValue, bool>::value));
//..
// However, as explained above, the assertion fails with C++03.  The result is
// the same when 'double' is substituted with any other type.
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

#include <bslmf_allocatorargt.h>
#include <bslmf_conditional.h>
#include <bslmf_enableif.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_isconvertible.h>
#include <bslmf_isnothrowmoveconstructible.h>
#include <bslmf_istriviallycopyable.h>
#include <bslmf_movableref.h>
#include <bslmf_nestedtraitdeclaration.h>
#include <bslmf_util.h>    // 'forward(V)'

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_deprecate.h>
#include <bsls_keyword.h>
#include <bsls_objectbuffer.h>
#include <bsls_review.h>
#include <bsls_util.h>     // 'forward<T>(V)'

#include <bslstl_optional.h>

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

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Include version that can be compiled with C++03
// Generated on Thu Oct 21 10:11:37 2021
// Command line: sim_cpp11_features.pl bdlb_nullablevalue.h
# define COMPILING_BDLB_NULLABLEVALUE_H
# include <bdlb_nullablevalue_cpp03.h>
# undef COMPILING_BDLB_NULLABLEVALUE_H
#else

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
class NullableValue : public bsl::optional<TYPE> {
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
    typedef bslmf::MovableRefUtil MoveUtil;

    struct EnableType {
    };

    struct NoAlloc {
        // This trivial tag type is used as a dummy when 'NullableValue' wraps
        // a non-allocator-aware type.
    };

    typedef typename bsl::conditional<bslma::UsesBslmaAllocator<TYPE>::value,
                                      bsl::allocator<char>,
                                      NoAlloc>::type AllocType;
        // Type alias to the allocator type used by this 'NullableValue'.  Note
        // that we can't refer to 'optional::allocator_type' because the
        // conditional needs the type to exist even for non allocator aware
        // types.

    // FRIENDS
    template <class ANY_TYPE>
    friend class NullableValue;

  public:
    // TYPES
    typedef bsl::optional<TYPE>   Base;
        // Base class of this type.

    typedef TYPE ValueType;
        // 'ValueType' is an alias for the underlying 'TYPE' upon which this
        // template class is instantiated, and represents the type of the
        // managed object.

    typedef AllocType allocator_type;
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

    NullableValue(bslmf::MovableRef<NullableValue> original)
                      BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                              bsl::is_nothrow_move_constructible<TYPE>::value);
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
    NullableValue(const bsl::optional<BDE_OTHER_TYPE>& value,
                  typename bsl::enable_if<
                     bsl::is_convertible<BDE_OTHER_TYPE, TYPE>::value &&
                     !bsl::is_same<bsl::optional<BDE_OTHER_TYPE>, TYPE>::value,
                     EnableType>::type = EnableType());             // IMPLICIT

    template <class BDE_OTHER_TYPE>
    NullableValue(const bsl::optional<BDE_OTHER_TYPE>& value,
                  const allocator_type&                allocator,
                  typename bsl::enable_if<
                     bsl::is_convertible<BDE_OTHER_TYPE, TYPE>::value &&
                     !bsl::is_same<bsl::optional<BDE_OTHER_TYPE>, TYPE>::value,
                     EnableType>::type = EnableType());             // IMPLICIT

    template <class BDE_OTHER_TYPE>
    NullableValue(BSLMF_MOVABLEREF_DEDUCE(
                                          NullableValue<BDE_OTHER_TYPE>) value,
                  typename bsl::enable_if<
                     bsl::is_convertible<BDE_OTHER_TYPE, TYPE>::value &&
                     !bsl::is_same<bsl::optional<BDE_OTHER_TYPE>, TYPE>::value,
                     EnableType>::type = EnableType());             // IMPLICIT

    template <class BDE_OTHER_TYPE>
    NullableValue(BSLMF_MOVABLEREF_DEDUCE(
                                      NullableValue<BDE_OTHER_TYPE>) value,
                  const allocator_type&                              allocator,
                  typename bsl::enable_if<
                     bsl::is_convertible<BDE_OTHER_TYPE, TYPE>::value &&
                     !bsl::is_same<bsl::optional<BDE_OTHER_TYPE>, TYPE>::value,
                     EnableType>::type = EnableType());             // IMPLICIT

    template <class BDE_OTHER_TYPE>
    NullableValue(BSLMF_MOVABLEREF_DEDUCE(bsl::optional<BDE_OTHER_TYPE>) value,
                  typename bsl::enable_if<
                     bsl::is_convertible<BDE_OTHER_TYPE, TYPE>::value &&
                     !bsl::is_same<bsl::optional<BDE_OTHER_TYPE>, TYPE>::value,
                     EnableType>::type = EnableType());             // IMPLICIT

    template <class BDE_OTHER_TYPE>
    NullableValue(BSLMF_MOVABLEREF_DEDUCE(
                                      bsl::optional<BDE_OTHER_TYPE>) value,
                  const allocator_type&                              allocator,
                  typename bsl::enable_if<
                     bsl::is_convertible<BDE_OTHER_TYPE, TYPE>::value &&
                     !bsl::is_same<bsl::optional<BDE_OTHER_TYPE>, TYPE>::value,
                     EnableType>::type = EnableType());             // IMPLICIT

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
        // supply memory.  Note that this constructor will not participate in
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

    template <class BDE_OTHER_TYPE>
    NullableValue<TYPE>& operator=(
                            BSLMF_MOVABLEREF_DEDUCE(
                                           NullableValue<BDE_OTHER_TYPE>) rhs);
        // Assign to this object the null value if the specified 'rhs' object
        // is null, and the value of 'rhs.value()' (of 'BDE_OTHER_TYPE')
        // converted to 'TYPE' otherwise.  Return a reference providing
        // modifiable access to this object.  Note that this method will fail
        // to compile if 'TYPE and 'BDE_OTHER_TYPE' are not compatible.

    template <class BDE_OTHER_TYPE>
    typename bsl::enable_if<bsl::is_convertible<BDE_OTHER_TYPE, TYPE>::value,
                            NullableValue<TYPE>&>::type
    operator=(const bsl::optional<BDE_OTHER_TYPE>& rhs);
        // Assign to this object the null value if the specified 'rhs' object
        // is null, and the value of 'rhs.value()' (of 'BDE_OTHER_TYPE')
        // converted to 'TYPE' otherwise.  Return a reference providing
        // modifiable access to this object.  Note that this method will fail
        // to compile if 'TYPE and 'BDE_OTHER_TYPE' are not compatible.

    template <class BDE_OTHER_TYPE>
    typename bsl::enable_if<bsl::is_convertible<BDE_OTHER_TYPE, TYPE>::value,
                            NullableValue<TYPE>&>::type
    operator=(BSLMF_MOVABLEREF_DEDUCE(bsl::optional<BDE_OTHER_TYPE>) rhs);
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
        // will fail to compile if 'TYPE' and 'BDE_OTHER_TYPE' are not
        // compatible.  Note that on C++03 but not in C++11 and beyond, if
        // 'BDE_OTHER_TYPE' is 'bslmf::MovableRef<TYPE3>' and 'TYPE' supports
        // moves and/or assigns from that type, a move rather than a copy may
        // take place.

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
#endif

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

};

// FREE OPERATORS
template <class LHS_TYPE, class RHS_TYPE>
bool operator==(const NullableValue<LHS_TYPE>& lhs,
                const NullableValue<RHS_TYPE>& rhs);
template <class LHS_TYPE, class RHS_TYPE>
bool operator==(const NullableValue<LHS_TYPE>& lhs,
                const bsl::optional<RHS_TYPE>& rhs);
template <class LHS_TYPE, class RHS_TYPE>
bool operator==(const bsl::optional<LHS_TYPE>& lhs,
                const NullableValue<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' nullable objects have the
    // same value, and 'false' otherwise.  Two nullable objects have the same
    // value if both are null, or if both are non-null and the values of their
    // underlying objects compare equal.  Note that this function will fail to
    // compile if 'LHS_TYPE' and 'RHS_TYPE' are not compatible.

template <class LHS_TYPE, class RHS_TYPE>
bool operator!=(const NullableValue<LHS_TYPE>& lhs,
                const NullableValue<RHS_TYPE>& rhs);
template <class LHS_TYPE, class RHS_TYPE>
bool operator!=(const bsl::optional<LHS_TYPE>& lhs,
                const NullableValue<RHS_TYPE>& rhs);
template <class LHS_TYPE, class RHS_TYPE>
bool operator!=(const NullableValue<LHS_TYPE>& lhs,
                const bsl::optional<RHS_TYPE>& rhs);
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
template <class LHS_TYPE, class RHS_TYPE>
bool operator<(const bsl::optional<LHS_TYPE>& lhs,
               const NullableValue<RHS_TYPE>& rhs);
template <class LHS_TYPE, class RHS_TYPE>
bool operator<(const NullableValue<LHS_TYPE>& lhs,
               const bsl::optional<RHS_TYPE>& rhs);
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
template <class LHS_TYPE, class RHS_TYPE>
bool operator>(const bsl::optional<LHS_TYPE>& lhs,
               const NullableValue<RHS_TYPE>& rhs);
template <class LHS_TYPE, class RHS_TYPE>
bool operator>(const NullableValue<LHS_TYPE>& lhs,
               const bsl::optional<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' nullable object is ordered after
    // the specified 'rhs' nullable object, and 'false' otherwise.  'lhs' is
    // ordered after 'rhs' if 'lhs' is non-null and 'rhs' is null or if both
    // are non-null and 'lhs.value()' is ordered after 'rhs.value()'.  Note
    // that this operator returns 'rhs < lhs' when both operands are of
    // 'NullableValue' type.  Also note that this function will fail to compile
    // if 'LHS_TYPE' and 'RHS_TYPE' are not compatible.

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
template <class LHS_TYPE, class RHS_TYPE>
bool operator<=(const bsl::optional<LHS_TYPE>& lhs,
                const NullableValue<RHS_TYPE>& rhs);
template <class LHS_TYPE, class RHS_TYPE>
bool operator<=(const NullableValue<LHS_TYPE>& lhs,
                const bsl::optional<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' nullable object is ordered before
    // the specified 'rhs' nullable object or 'lhs' and 'rhs' have the same
    // value, and 'false' otherwise.  (See 'operator<' and 'operator=='.)  Note
    // that this operator returns '!(rhs < lhs)' when both operands are of
    // 'NullableValue' type.  Also note that this function will fail to compile
    // if 'LHS_TYPE' and 'RHS_TYPE' are not compatible.

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
template <class LHS_TYPE, class RHS_TYPE>
bool operator>=(const bsl::optional<LHS_TYPE>& lhs,
                const NullableValue<RHS_TYPE>& rhs);
template <class LHS_TYPE, class RHS_TYPE>
bool operator>=(const NullableValue<LHS_TYPE>& lhs,
                const bsl::optional<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' nullable object is ordered after
    // the specified 'rhs' nullable object or 'lhs' and 'rhs' have the same
    // value, and 'false' otherwise.  (See 'operator>' and 'operator=='.)  Note
    // that this operator returns '!(lhs < rhs)' when both operands are of
    // 'NullableValue' type.  Also note that this function will fail to compile
    // if 'LHS_TYPE' and 'RHS_TYPE' are not compatible.

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
    // Return 'false'.  Note that 'bdlb::nullOpt' never orders after a
    // 'NullableValue'.

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR
bool operator<(const NullOptType&         ,
               const NullableValue<TYPE>& value) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if the specified 'value' is not null, and 'false'
    // otherwise.  Note that 'bdlb::nullOpt' is ordered before any
    // 'NullableValue' that is not null.

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
typename bsl::enable_if<BloombergLP::bslma::UsesBslmaAllocator<TYPE>::value,
                        void>::type
swap(NullableValue<TYPE>& lhs, NullableValue<TYPE>& rhs);
template <class TYPE>
typename bsl::enable_if<!BloombergLP::bslma::UsesBslmaAllocator<TYPE>::value,
                        void>::type
swap(NullableValue<TYPE>& lhs, NullableValue<TYPE>& rhs);
    // Exchange the values of the specified 'rhs' and 'lhs' objects.  This
    // function provides the no-throw exception-safety guarantee if the
    // (template parameter) 'TYPE' provides that guarantee, the two objects
    // were created with the same allocator (if applicable), and the result of
    // the 'isNull' method for the two objects is the same; otherwise this
    // function provides the basic guarantee.

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
{}

template <class TYPE>
inline
NullableValue<TYPE>::NullableValue(
                         const allocator_type& allocator) BSLS_KEYWORD_NOEXCEPT
: Base(bsl::allocator_arg, allocator)
{}

template <class TYPE>
inline
NullableValue<TYPE>::NullableValue(const NullableValue& original)
: Base(static_cast<const bsl::optional<TYPE>&>(original))
{}

template <class TYPE>
inline
NullableValue<TYPE>::NullableValue(const NullableValue&  original,
                                   const allocator_type& allocator)
: Base(bsl::allocator_arg,
       allocator,
       static_cast<const bsl::optional<TYPE>&>(original))
{}

template <class TYPE>
inline
NullableValue<TYPE>::NullableValue(bslmf::MovableRef<NullableValue> original)
                       BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                               bsl::is_nothrow_move_constructible<TYPE>::value)
: Base(MoveUtil::move(
       static_cast<bsl::optional<TYPE>&>(MoveUtil::access(original))))
{}

template <class TYPE>
inline
NullableValue<TYPE>::NullableValue(bslmf::MovableRef<NullableValue> original,
                                   const allocator_type&            allocator)
: Base(bsl::allocator_arg,
       allocator,
       MoveUtil::move(static_cast<bsl::optional<TYPE>&>(
                                                  MoveUtil::access(original))))
{}

template <class TYPE>
template <class BDE_OTHER_TYPE>
inline
NullableValue<TYPE>::NullableValue(
    BSLS_COMPILERFEATURES_FORWARD_REF(BDE_OTHER_TYPE) value,
    typename bsl::enable_if<
        bsl::is_convertible<BDE_OTHER_TYPE, TYPE>::value &&
            !bsl::is_convertible<BDE_OTHER_TYPE, allocator_type>::value,
        EnableType>::type)
: Base(BSLS_COMPILERFEATURES_FORWARD(BDE_OTHER_TYPE, value))
{}

template <class TYPE>
template <class BDE_OTHER_TYPE>
inline
NullableValue<TYPE>::NullableValue(
    BSLS_COMPILERFEATURES_FORWARD_REF(BDE_OTHER_TYPE) value,
    const allocator_type&                             allocator,
    typename bsl::enable_if<bsl::is_convertible<BDE_OTHER_TYPE, TYPE>::value,
                            EnableType>::type)
: Base(bsl::allocator_arg,
       allocator,
       BSLS_COMPILERFEATURES_FORWARD(BDE_OTHER_TYPE, value))
{}

template <class TYPE>
template <class BDE_OTHER_TYPE>
inline
NullableValue<TYPE>::NullableValue(
                  const bsl::optional<BDE_OTHER_TYPE>& value,
                  typename bsl::enable_if<
                     bsl::is_convertible<BDE_OTHER_TYPE, TYPE>::value &&
                     !bsl::is_same<bsl::optional<BDE_OTHER_TYPE>, TYPE>::value,
                     EnableType>::type)
: Base(value)
{}

template <class TYPE>
template <class BDE_OTHER_TYPE>
inline
NullableValue<TYPE>::NullableValue(
                  const bsl::optional<BDE_OTHER_TYPE>& value,
                  const allocator_type&                allocator,
                  typename bsl::enable_if<
                     bsl::is_convertible<BDE_OTHER_TYPE, TYPE>::value &&
                     !bsl::is_same<bsl::optional<BDE_OTHER_TYPE>, TYPE>::value,
                     EnableType>::type)
: Base(bsl::allocator_arg, allocator, value)
{}

template <class TYPE>
template <class BDE_OTHER_TYPE>
inline
NullableValue<TYPE>::NullableValue(BSLMF_MOVABLEREF_DEDUCE(
                                          NullableValue<BDE_OTHER_TYPE>) value,
                  typename bsl::enable_if<
                     bsl::is_convertible<BDE_OTHER_TYPE, TYPE>::value &&
                     !bsl::is_same<bsl::optional<BDE_OTHER_TYPE>, TYPE>::value,
                     EnableType>::type)
: Base(MoveUtil::move(static_cast<bsl::optional<BDE_OTHER_TYPE>&>(
                                                     MoveUtil::access(value))))
{}

template <class TYPE>
template <class BDE_OTHER_TYPE>
inline
NullableValue<TYPE>::NullableValue(BSLMF_MOVABLEREF_DEDUCE(
                                      NullableValue<BDE_OTHER_TYPE>) value,
                  const allocator_type&                              allocator,
                  typename bsl::enable_if<
                     bsl::is_convertible<BDE_OTHER_TYPE, TYPE>::value &&
                     !bsl::is_same<bsl::optional<BDE_OTHER_TYPE>, TYPE>::value,
                     EnableType>::type)
: Base(bsl::allocator_arg,
       allocator,
       MoveUtil::move(static_cast<bsl::optional<BDE_OTHER_TYPE>&>(
                                                     MoveUtil::access(value))))
{}

template <class TYPE>
template <class BDE_OTHER_TYPE>
inline
NullableValue<TYPE>::NullableValue(
                  BSLMF_MOVABLEREF_DEDUCE(bsl::optional<BDE_OTHER_TYPE>) value,
                  typename bsl::enable_if<
                     bsl::is_convertible<BDE_OTHER_TYPE, TYPE>::value &&
                     !bsl::is_same<bsl::optional<BDE_OTHER_TYPE>, TYPE>::value,
                     EnableType>::type)
: Base(MoveUtil::move(value))
{}

template <class TYPE>
template <class BDE_OTHER_TYPE>
inline
NullableValue<TYPE>::NullableValue(
                  BSLMF_MOVABLEREF_DEDUCE(
                                      bsl::optional<BDE_OTHER_TYPE>) value,
                  const allocator_type&                              allocator,
                  typename bsl::enable_if<
                     bsl::is_convertible<BDE_OTHER_TYPE, TYPE>::value &&
                     !bsl::is_same<bsl::optional<BDE_OTHER_TYPE>, TYPE>::value,
                     EnableType>::type)
: Base(bsl::allocator_arg, allocator, MoveUtil::move(value))
{}

template <class TYPE>
template <class BDE_OTHER_TYPE>
inline
NullableValue<TYPE>::NullableValue(
    const NullableValue<BDE_OTHER_TYPE>& original)
: Base(static_cast<const bsl::optional<BDE_OTHER_TYPE>&>(original))
{}

template <class TYPE>
template <class BDE_OTHER_TYPE>
inline
NullableValue<TYPE>::NullableValue(
    const NullableValue<BDE_OTHER_TYPE>& original,
    const allocator_type&                allocator)
: Base(bsl::allocator_arg,
       allocator,
       static_cast<const bsl::optional<BDE_OTHER_TYPE>&>(original))
{}

template <class TYPE>
inline
NullableValue<TYPE>::NullableValue(const NullOptType&) BSLS_KEYWORD_NOEXCEPT
: Base()
{}

template <class TYPE>
inline
NullableValue<TYPE>::NullableValue(
                         const NullOptType&,
                         const allocator_type& allocator) BSLS_KEYWORD_NOEXCEPT
: Base(bsl::allocator_arg, allocator)
{}

// MANIPULATORS
template <class TYPE>
inline
NullableValue<TYPE>& NullableValue<TYPE>::operator=(const NullableValue& rhs)
{
    // Constraints on 'bsl::optional' assignment operator may affect the
    // assignment.  In order to avoid changes to behavior, we implement the
    // assignment or conversion directly.

    if (rhs.has_value()) {
        if (this->has_value()) {
            this->value() = rhs.value();
        }
        else {
            this->emplace(rhs.value());
        }
    }
    else {
        this->reset();
    }

    return *this;
}

template <class TYPE>
inline
NullableValue<TYPE>& NullableValue<TYPE>::operator=(
                                          bslmf::MovableRef<NullableValue> rhs)
{
    // Constraints on 'bsl::optional' assignment operator may affect the
    // assignment.  In order to avoid changes to behavior, we implement the
    // assignment or conversion directly.

    NullableValue& localRhs = rhs;

    if (localRhs.has_value()) {
        if (this->has_value()) {
            this->value() = MoveUtil::move(localRhs.value());
        }
        else {
            this->emplace(MoveUtil::move(localRhs.value()));
        }
    }
    else {
        this->reset();
    }

    return *this;
}

template <class TYPE>
template <class BDE_OTHER_TYPE>
NullableValue<TYPE>& NullableValue<TYPE>::operator=(
                                      const NullableValue<BDE_OTHER_TYPE>& rhs)
{
    // Constraints on 'bsl::optional' assignment operator may affect the
    // assignment.  In order to avoid changes to behavior, we implement the
    // assignment or conversion directly.

    if (rhs.has_value()) {
        if (this->has_value()) {
            this->value() = rhs.value();
        }
        else {
            this->emplace(rhs.value());
        }
    }
    else {
        this->reset();
    }

    return *this;
}

template <class TYPE>
template <class BDE_OTHER_TYPE>
NullableValue<TYPE>& NullableValue<TYPE>::operator=(
      BSLMF_MOVABLEREF_DEDUCE(NullableValue<BDE_OTHER_TYPE>) rhs)
{
    // Constraints on 'bsl::optional' assignment operator may affect the
    // assignment.  In order to avoid changes to behavior, we implement the
    // assignment or conversion directly.

    NullableValue<BDE_OTHER_TYPE>& rhsLocal = rhs;

    if (rhsLocal.has_value()) {
        if (this->has_value()) {
            this->value() = MoveUtil::move(rhsLocal.value());
        }
        else {
            this->emplace(MoveUtil::move(rhsLocal.value()));
        }
    }
    else {
        this->reset();
    }

    return *this;
}

template <class TYPE>
template <class BDE_OTHER_TYPE>
typename bsl::enable_if<bsl::is_convertible<BDE_OTHER_TYPE, TYPE>::value,
                        NullableValue<TYPE>&>::type
NullableValue<TYPE>::operator=(const bsl::optional<BDE_OTHER_TYPE>& rhs)
{
    Base& base = *this;

    base = rhs;

    return *this;
}

template <class TYPE>
template <class BDE_OTHER_TYPE>
typename bsl::enable_if<bsl::is_convertible<BDE_OTHER_TYPE, TYPE>::value,
                        NullableValue<TYPE>&>::type
NullableValue<TYPE>::operator=(
      BSLMF_MOVABLEREF_DEDUCE(bsl::optional<BDE_OTHER_TYPE>) rhs)
{
    Base& base = *this;

    base = MoveUtil::move(rhs);

    return *this;
}

template <class TYPE>
inline
NullableValue<TYPE>& NullableValue<TYPE>::operator=(const TYPE& rhs)
{
    // Constraints on 'bsl::optional' assignment operator may affect the
    // assignment.  In order to avoid changes to behavior, we implement the
    // assignment or conversion directly.

    if (this->has_value()) {
        this->value() = rhs;
    }
    else {
        this->emplace(rhs);
    }

    return *this;
}

template <class TYPE>
inline
NullableValue<TYPE>& NullableValue<TYPE>::operator=(
                                                   bslmf::MovableRef<TYPE> rhs)
{
    // Constraints on 'bsl::optional' assignment operator may affect the
    // assignment.  In order to avoid changes to behavior, we implement the
    // assignment or conversion directly.

    if (this->has_value()) {
        this->value() = MoveUtil::move(rhs);
    }
    else {
        this->emplace(MoveUtil::move(rhs));
    }

    return *this;
}

template <class TYPE>
template <class BDE_OTHER_TYPE>
inline
NullableValue<TYPE>& NullableValue<TYPE>::operator=(const BDE_OTHER_TYPE& rhs)
{
    // Constraints on 'bsl::optional' assignment operator may affect the
    // assignment.  In order to avoid changes to behavior, we implement the
    // assignment or conversion directly.

    if (this->has_value()) {
        this->value() = rhs;
    }
    else {
        this->emplace(rhs);
    }

    return *this;
}

template <class TYPE>
inline
NullableValue<TYPE>& NullableValue<TYPE>::operator=(
                                      const NullOptType&) BSLS_KEYWORD_NOEXCEPT
{
    this->reset();

    return *this;
}

template <class TYPE>
template <class STREAM>
STREAM& NullableValue<TYPE>::bdexStreamIn(STREAM& stream, int version)
{
    using bslx::InStreamFunctions::bdexStreamIn;

    char isNull = 0;  // Redundant initialization to suppress -Werror.

    stream.getInt8(isNull);

    if (stream) {
        if (!isNull) {
            bdexStreamIn(stream, this->emplace(), version);
        }
        else {
            this->reset();
        }
    }

    return stream;
}

template <class TYPE>
template <class BDE_OTHER_TYPE>
inline
TYPE& NullableValue<TYPE>::makeValue(
                       BSLS_COMPILERFEATURES_FORWARD_REF(BDE_OTHER_TYPE) value)
{
    return this->emplace(BSLS_COMPILERFEATURES_FORWARD(BDE_OTHER_TYPE, value));
}

template <class TYPE>
inline
TYPE& NullableValue<TYPE>::makeValue()
{
    return this->emplace();
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class TYPE>
template <class... ARGS>
inline
TYPE& NullableValue<TYPE>::makeValueInplace(ARGS&&... args)
{
    return this->emplace(BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
}
#endif

template <class TYPE>
inline
TYPE& NullableValue<TYPE>::value()
{
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    BSLS_REVIEW_OPT(this->has_value());

    return this->dereferenceRaw();
#else
    return **this;
#endif
}
// ACCESSORS
template <class TYPE>
inline
const TYPE *NullableValue<TYPE>::addressOr(const TYPE *address) const
{
    return this->has_value() ? this-> operator->() : address;
}

template <class TYPE>
template <class STREAM>
STREAM& NullableValue<TYPE>::bdexStreamOut(STREAM& stream, int version) const
{
    using bslx::OutStreamFunctions::bdexStreamOut;

    const bool isNull = !this->has_value();

    stream.putInt8(isNull ? 1 : 0);

    if (!isNull) {
        bdexStreamOut(stream, this->value(), version);
    }

    return stream;
}

template <class TYPE>
inline
bool NullableValue<TYPE>::isNull() const BSLS_KEYWORD_NOEXCEPT
{
    return !this->has_value();
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
    if (!this->has_value()) {
        return bdlb::PrintMethods::print(stream,
                                         "NULL",
                                         level,
                                         spacesPerLevel);             // RETURN
    }

    return bdlb::PrintMethods::print(
        stream, this->value(), level, spacesPerLevel);
}

template <class TYPE>
inline
const TYPE& NullableValue<TYPE>::value() const
{
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    BSLS_REVIEW_OPT(this->has_value());

    return this->dereferenceRaw();
#else
    return **this;
#endif
}

template <class TYPE>
inline
TYPE NullableValue<TYPE>::valueOr(const TYPE& value) const
{
    return this->value_or(value);
}

template <class TYPE>
inline
const TYPE *NullableValue<TYPE>::valueOr(const TYPE *value) const
{
    return this->has_value() ? this-> operator->() : value;
}

template <class TYPE>
inline
const TYPE *NullableValue<TYPE>::valueOrNull() const
{
    return this->has_value() ? this-> operator->() : 0;
}

}  // close package namespace

// FREE OPERATORS
template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator==(const NullableValue<LHS_TYPE>& lhs,
                      const NullableValue<RHS_TYPE>& rhs)
{
    return static_cast<const bsl::optional<LHS_TYPE>&>(lhs) ==
           static_cast<const bsl::optional<RHS_TYPE>&>(rhs);
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator==(const bsl::optional<LHS_TYPE>& lhs,
                      const NullableValue<RHS_TYPE>& rhs)
{
    return lhs == static_cast<const bsl::optional<RHS_TYPE>&>(rhs);
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator==(const NullableValue<LHS_TYPE>& lhs,
                      const bsl::optional<RHS_TYPE>& rhs)
{
    return static_cast<const bsl::optional<LHS_TYPE>&>(lhs) == rhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator!=(const NullableValue<LHS_TYPE>& lhs,
                      const NullableValue<RHS_TYPE>& rhs)
{
    return static_cast<const bsl::optional<LHS_TYPE>&>(lhs) !=
           static_cast<const bsl::optional<RHS_TYPE>&>(rhs);
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator!=(const bsl::optional<LHS_TYPE>& lhs,
                      const NullableValue<RHS_TYPE>& rhs)
{
    return lhs != static_cast<const bsl::optional<RHS_TYPE>&>(rhs);
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator!=(const NullableValue<LHS_TYPE>& lhs,
                      const bsl::optional<RHS_TYPE>& rhs)
{
    return static_cast<const bsl::optional<LHS_TYPE>&>(lhs) != rhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator==(const NullableValue<LHS_TYPE>& lhs, const RHS_TYPE& rhs)
{
    return static_cast<const bsl::optional<LHS_TYPE>&>(lhs) == rhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator==(const LHS_TYPE& lhs, const NullableValue<RHS_TYPE>& rhs)
{
    return lhs == static_cast<const bsl::optional<RHS_TYPE>&>(rhs);
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator!=(const NullableValue<LHS_TYPE>& lhs, const RHS_TYPE& rhs)
{
    return static_cast<const bsl::optional<LHS_TYPE>&>(lhs) != rhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator!=(const LHS_TYPE& lhs, const NullableValue<RHS_TYPE>& rhs)
{
    return lhs != static_cast<const bsl::optional<RHS_TYPE>&>(rhs);
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator<(const NullableValue<LHS_TYPE>& lhs,
                     const NullableValue<RHS_TYPE>& rhs)
{
    return static_cast<const bsl::optional<LHS_TYPE>&>(lhs) <
           static_cast<const bsl::optional<RHS_TYPE>&>(rhs);
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator<(const bsl::optional<LHS_TYPE>& lhs,
                     const NullableValue<RHS_TYPE>& rhs)
{
    return lhs < static_cast<const bsl::optional<RHS_TYPE>&>(rhs);
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator<(const NullableValue<LHS_TYPE>& lhs,
                     const bsl::optional<RHS_TYPE>& rhs)
{
    return static_cast<const bsl::optional<LHS_TYPE>&>(lhs) < rhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator<(const NullableValue<LHS_TYPE>& lhs, const RHS_TYPE& rhs)
{
    return static_cast<const bsl::optional<LHS_TYPE>&>(lhs) < rhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator<(const LHS_TYPE& lhs, const NullableValue<RHS_TYPE>& rhs)
{
    return lhs < static_cast<const bsl::optional<RHS_TYPE>&>(rhs);
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
bool bdlb::operator>(const bsl::optional<LHS_TYPE>& lhs,
                     const NullableValue<RHS_TYPE>& rhs)
{
    return lhs > static_cast<const bsl::optional<RHS_TYPE>&>(rhs);
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator>(const NullableValue<LHS_TYPE>& lhs,
                     const bsl::optional<RHS_TYPE>& rhs)
{
    return static_cast<const bsl::optional<LHS_TYPE>&>(lhs) > rhs;
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
bool bdlb::operator<=(const bsl::optional<LHS_TYPE>& lhs,
                      const NullableValue<RHS_TYPE>& rhs)
{
    return lhs <= static_cast<const bsl::optional<RHS_TYPE>&>(rhs);
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator<=(const NullableValue<LHS_TYPE>& lhs,
                      const bsl::optional<RHS_TYPE>& rhs)
{
    return static_cast<const bsl::optional<LHS_TYPE>&>(lhs) <= rhs;
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
bool bdlb::operator>=(const bsl::optional<LHS_TYPE>& lhs,
                      const NullableValue<RHS_TYPE>& rhs)
{
    return lhs >= static_cast<const bsl::optional<RHS_TYPE>&>(rhs);
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bdlb::operator>=(const NullableValue<LHS_TYPE>& lhs,
                      const bsl::optional<RHS_TYPE>& rhs)
{
    return static_cast<const bsl::optional<LHS_TYPE>&>(lhs) >= rhs;
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
inline
BSLS_KEYWORD_CONSTEXPR bool bdlb::operator==(
                             const NullableValue<TYPE>& value,
                             const NullOptType&) BSLS_KEYWORD_NOEXCEPT
{
    return value.isNull();
}

template <class TYPE>
inline
BSLS_KEYWORD_CONSTEXPR bool bdlb::operator==(
                        const NullOptType&,
                        const NullableValue<TYPE>& value) BSLS_KEYWORD_NOEXCEPT
{
    return value.isNull();
}

template <class TYPE>
inline
BSLS_KEYWORD_CONSTEXPR bool bdlb::operator!=(
                             const NullableValue<TYPE>& value,
                             const NullOptType&) BSLS_KEYWORD_NOEXCEPT
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
typename bsl::enable_if<BloombergLP::bslma::UsesBslmaAllocator<TYPE>::value,
                        void>::type
bdlb::swap(NullableValue<TYPE>& lhs, NullableValue<TYPE>& rhs)
{
    swap(static_cast<bsl::optional<TYPE>&>(lhs),
         static_cast<bsl::optional<TYPE>&>(rhs));
}

template <class TYPE>
inline
typename bsl::enable_if<!BloombergLP::bslma::UsesBslmaAllocator<TYPE>::value,
                        void>::type
bdlb::swap(NullableValue<TYPE>& lhs, NullableValue<TYPE>& rhs)
{
    lhs.swap(rhs);
}

}  // close enterprise namespace

#ifdef BSLSTL_OPTIONAL_CPP20_IS_OPTIONAL_GNU_WORKAROUND_NEEDED
// This hack works around a bug in gcc's defintion for is-optional.  See
// bslstl_optional.h for more information.

namespace std {
template <typename _Tp>
inline constexpr bool __is_optional_v<BloombergLP::bdlb::NullableValue<_Tp>> =
    true;
}
#endif // BSLSTL_OPTIONAL_CPP20_IS_OPTIONAL_GNU_WORKAROUND_NEEDED

#ifdef BSLSTL_OPTIONAL_CPP20_IS_OPTIONAL_MSVC_WORKAROUND_NEEDED
// This hack works around a bug in MSVC's C++20 defintion for is-optional. See
// bslstl_optional.h for more information.

namespace std {
template <typename _Tp>
inline
constexpr bool _Is_specialization_v<BloombergLP::bdlb::NullableValue<_Tp>,
                                    std::optional> = true;
}
#endif  // BSLSTL_OPTIONAL_CPP20_IS_OPTIONAL_MSVC_WORKAROUND_NEEDED

#endif // End C++11 code

#endif  // INCLUDED_BDLB_NULLABLEVALUE

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
