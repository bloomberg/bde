// bsls_compilerfeatures.cpp                                          -*-C++-*-
#include <bsls_compilerfeatures.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_bsltestutil.h>   // for testing only
#include <bsls_buildtarget.h>   // for testing only

#include <bsls_platform.h>  // for 'BSLS_PLATFORM_COMPILER_ERROR'

#if defined(BSLS_COMPILERFEATURES_INTIIALIZER_LIST_LEAKS_ON_EXCEPTIONS)

# ifndef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#   error "'BSLS_COMPILERFEATURES_INITIALIZER_LIST_LEAKS_ON_EXCEPTIONS'       \
            requires 'BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS'"
  BSLS_PLATFORM_COMPILER_ERROR;
# endif

#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14) &&                 \
    !defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR)
  #error Relaxed 'constexpr' support should imply 'constexpr' support!
  BSLS_PLATFORM_COMPILER_ERROR;
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP17) &&                 \
    !defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14)
  #error Relaxed (C++17) 'constexpr' support should imply relaxed (C++14)     \
                                                          'constexpr' support!
  BSLS_PLATFORM_COMPILER_ERROR;
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS) &&                  \
    !defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
  #error Ref qualifiers support should imply rvalue reference support!
  BSLS_PLATFORM_COMPILER_ERROR;
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_IN_FNC_TYPE) !=            \
                         defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES)
  #error Deprecated alias must be defined!
  BSLS_PLATFORM_COMPILER_ERROR;
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_COROUTINE)
  #include <coroutine>
  #if !defined(__cpp_lib_coroutine) || __cpp_lib_coroutine < 201902L
    #error <coroutine> implementation discrepancy!
    BSLS_PLATFORM_COMPILER_ERROR;
  #endif
#endif

// Testing full feature support macros.
// If a compiler supports a `FULL` C++ version macro, then there are guarantees
// that no feature macro for that C++ version is enabled unless building for
// that standard or later.  Hence, each `FULL` macro should have all of the
// listed feature macros when defined, otherwise if the corresponding `NO`
// macro is defined then none of the listed feature macros should be defined.

#if defined(BSLS_COMPILERFEATURES_FULL_CPP11)
# if !defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES)
#   error Alias templates feature missing from full C++11 support
# endif

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_ALIGNAS)
#   error Alignas feature missing from full C++11 support
# endif

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN)
#   error [[noreturn]] attribute missing from full C++11 support
# endif

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR)
#   error Constexpr missing from full C++11 support
# endif

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE)
#   error Decltype missing from full C++11 support
# endif

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_DEFAULT_TEMPLATE_ARGS)
#   error Default template arguments missing from full C++11 support
# endif

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS)
#   error Defaulted functions feature missing from full C++11 support
# endif

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS)
#   error Deleted functions feature missing from full C++11 support
# endif

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_ENUM_CLASS)
#   error Enum class feature missing from full C++11 support
# endif

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE)
#   error Extern template feature missing from full C++11 support
# endif

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_FINAL)
#   error Final overrider feature missing from full C++11 support
# endif

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
#   error Braced initialization feature missing from full C++11 support
# endif

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE)
#   error Inline namespaces feature missing from full C++11 support
# endif

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
#   error Noexcept feature missing from full C++11 support
# endif

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_NULLPTR)
#   error Nullptr feature missing from full C++11 support
# endif

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT)
#   error Explicit operators feature missing from full C++11 support
# endif

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE)
#   error Override feature missing from full C++11 support
# endif

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_RAW_STRINGS)
#   error Raw string literals feature missing from full C++11 support
# endif

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
#   error Ref qualifiers feature missing from full C++11 support
# endif

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
#   error Rvalue references feature missing from full C++11 support
# endif

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT)
#   error Static assert feature missing from full C++11 support
# endif

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER)
#   error Type traits header (intrinsics) missing from full C++11 support
# endif

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
#   error Unicode character types feature missing from full C++11 support
# endif

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_USER_DEFINED_LITERALS)
#   error User defined literals feature missing from full C++11 support
# endif

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)
#   error Variadic templates feature missing from full C++11 support
# endif
#endif

#if defined(BSLS_COMPILERFEATURES_FULL_CPP14)
# if !defined(BSLS_COMPILERFEATURES_FULL_CPP11)
#  error cannot support full C++14 without full C++11
# endif

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14)
#   error Upgraded constexpr support missing from full C++14 support
# endif

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES)
#   error Variable templates feature missing from full C++14 support
# endif
#endif

#if defined(BSLS_COMPILERFEATURES_FULL_CPP17)
# if !defined(BSLS_COMPILERFEATURES_FULL_CPP14)
#  error cannot support full C++17 without full C++14
# endif

# if !defined(BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION)
#   error Guaranteed copy elision missing from full C++17 support
# endif

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_FALLTHROUGH)
#   error [[fallthrough]] attribute missing from full C++17 support
# endif

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_MAYBE_UNUSED)
#   error [[maybe_unused]] attribute missing from full C++17 support
# endif

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NODISCARD)
#   error [[nodiscard]] attribute missing from full C++17 support
# endif

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP17)
#   error Upgraded constexpr support missing from full C++17 support
# endif

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_HEXFLOAT_LITERALS)
#   error Hexfloat literals feature missing from full C++17 support
# endif

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES)
#   error Inline variables feature missing from full C++17 support
# endif

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_IN_FNC_TYPE) \
  || !defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES)
#   error Noexcept function types feature missing from full C++17 support
# endif
#endif

// Note that BSLS_COMPILERFEATURES_FULL_CPP20  is not yet defined for
// any compiler that BDE supports.

#if defined(BSLS_COMPILERFEATURES_FULL_CPP20)
# if !defined(BSLS_COMPILERFEATURES_FULL_CPP17)
#  error cannot support full C++20 without full C++17
# endif

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_CONCEPTS)
#   error Concepts feature missing from full C++20 support
# endif

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEVAL_CPP20)
#   error Consteval feature missing from full C++20 support
# endif

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP20)
#   error Upgraded constexpr support missing from full C++20 support
# endif

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_COROUTINE)
#   error Coroutines feature missing from full C++20 support
# endif

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON)
#   error Spaceship operator missing from full C++20 support
# endif

# if !defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
#   error char8_t character type missing from full C++20 support
# endif
#endif

// Confirm that all the C++ Standard feature test macros are defined and
// have at least the expected value for the given standard.  Note that
// these macros were specified by a WG21 document SD-6 prior to becoming
// part of the Standard document in C++20, so might not be defined on all
// compilers for standards prior to C++20.  All are tested and confirmed,
// except as noted, on the toolchains where we claim full C++17 support.

#if defined(BSLS_COMPILERFEATURES_FULL_CPP17)
// First test the SD-6 macros back-ported to C++14 and earlier
# ifndef __cpp_aggregate_nsdmi
#   error __cpp_aggregate_nsdmi is not defined
# elif __cpp_aggregate_nsdmi < 201304L
#   error __cpp_aggregate_nsdmi does not suport C++17
# endif
# ifndef __cpp_alias_templates
#   error __cpp_alias_templates is not defined
# elif __cpp_alias_templates < 200704L
#   error __cpp_alias_templates does not suport C++17
# endif
# ifndef __cpp_attributes
#   error __cpp_attributes is not defined
# elif __cpp_attributes < 200809L
#   error __cpp_attributes does not suport C++17
# endif
# ifndef __cpp_binary_literals
#   error __cpp_binary_literals is not defined
# elif __cpp_binary_literals < 201304L
#   error __cpp_binary_literals does not suport C++17
# endif
# ifndef __cpp_decltype
#   error __cpp_decltype is not defined
# elif __cpp_decltype < 200707L
#   error __cpp_decltype does not suport C++17
# endif
# ifndef __cpp_decltype_auto
#   error __cpp_decltype_auto is not defined
# elif __cpp_decltype_auto < 201304L
#   error __cpp_decltype_auto does not suport C++17
# endif
# ifndef __cpp_delegating_constructors
#   error __cpp_delegating_constructors is not defined
# elif __cpp_delegating_constructors < 200604L
#   error __cpp_delegating_constructors does not suport C++17
# endif
# ifndef __cpp_initializer_lists
#   error __cpp_initializer_lists is not defined
# elif __cpp_initializer_lists < 200806L
#   error __cpp_initializer_lists does not suport C++17
# endif
# ifndef __cpp_lambdas
#   error __cpp_lambdas is not defined
# elif __cpp_lambdas < 200907L
#   error __cpp_lambdas does not suport C++17
# endif
# ifndef __cpp_namespace_attributes
#   error __cpp_namespace_attributes is not defined
# elif __cpp_namespace_attributes < 201411L
#   error __cpp_namespace_attributes does not suport C++17
# endif
# ifndef __cpp_nsdmi
#   error __cpp_nsdmi is not defined
# elif __cpp_nsdmi < 200809L
#   error __cpp_nsdmi does not suport C++17
# endif
# ifndef __cpp_range_based_for
#   error __cpp_range_based_for is not defined
# elif __cpp_range_based_for < 201603L
#   error __cpp_range_based_for does not suport C++17
# endif
# ifndef __cpp_raw_strings
#   error __cpp_raw_strings is not defined
# elif __cpp_raw_strings < 200710L
#   error __cpp_raw_strings does not suport C++17
# endif
# ifndef __cpp_ref_qualifiers
#   error __cpp_ref_qualifiers is not defined
# elif __cpp_ref_qualifiers < 200710L
#   error __cpp_ref_qualifiers does not suport C++17
# endif
# ifndef __cpp_return_type_deduction
#   error __cpp_return_type_deduction is not defined
# elif __cpp_return_type_deduction < 201304L
#   error __cpp_return_type_deduction does not suport C++17
# endif
# ifndef __cpp_rvalue_references
#   error __cpp_rvalue_references is not defined
# elif __cpp_rvalue_references < 200610L
#   error __cpp_rvalue_references does not suport C++17
# endif
# ifndef __cpp_threadsafe_static_init
#   error __cpp_threadsafe_static_init is not defined
# elif __cpp_threadsafe_static_init < 200806L
#   error __cpp_threadsafe_static_init does not suport C++17
# endif
# ifndef __cpp_unicode_characters
#   error __cpp_unicode_characters is not defined
# elif __cpp_unicode_characters < 200704L
#   error __cpp_unicode_characters does not suport C++17
# endif
# ifndef __cpp_unicode_literals
#   error __cpp_unicode_literals is not defined
# elif __cpp_unicode_literals < 200710L
#   error __cpp_unicode_literals does not suport C++17
# endif
# ifndef __cpp_user_defined_literals
#   error __cpp_user_defined_literals is not defined
# elif __cpp_user_defined_literals < 200809L
#   error __cpp_user_defined_literals does not suport C++17
# endif
# ifndef __cpp_variable_templates
#   error __cpp_variable_templates is not defined
# elif __cpp_variable_templates < 201304L
#   error __cpp_variable_templates does not suport C++17
# endif
# ifndef __cpp_variadic_templates
#   error __cpp_variadic_templates is not defined
# elif __cpp_variadic_templates < 200704L
#   error __cpp_variadic_templates does not suport C++17
# endif

// True C++17 feature macros
# ifndef __cpp_aggregate_bases
#   error __cpp_aggregate_bases is not defined
# elif __cpp_aggregate_bases < 201603L
#   error __cpp_aggregate_bases does not suport C++17
# endif
# ifndef __cpp_aligned_new
#   error __cpp_aligned_new is not defined
# elif __cpp_aligned_new < 201606L
#   error __cpp_aligned_new does not suport C++17
# endif
# ifndef __cpp_capture_star_this
#   error __cpp_capture_star_this is not defined
# elif __cpp_capture_star_this < 201603L
#   error __cpp_capture_star_this does not suport C++17
# endif
# ifndef __cpp_enumerator_attributes
#   error __cpp_enumerator_attributes is not defined
# elif __cpp_enumerator_attributes < 201411L
#   error __cpp_enumerator_attributes does not suport C++17
# endif
# ifndef __cpp_fold_expressions
#   error __cpp_fold_expressions is not defined
# elif __cpp_fold_expressions < 201603L
#   error __cpp_fold_expressions does not suport C++17
# endif
# ifndef __cpp_guaranteed_copy_elision
#   error __cpp_guaranteed_copy_elision is not defined
# elif __cpp_guaranteed_copy_elision < 201606L
#   error __cpp_guaranteed_copy_elision does not suport C++17
# endif
# ifndef __cpp_hex_float
#   error __cpp_hex_float is not defined
# elif __cpp_hex_float < 201603L
#   error __cpp_hex_float does not suport C++17
# endif
# ifndef __cpp_if_constexpr
#   error __cpp_if_constexpr is not defined
# elif __cpp_if_constexpr < 201606L
#   error __cpp_if_constexpr does not suport C++17
# endif
# ifndef __cpp_inheriting_constructors
#   error __cpp_inheriting_constructors is not defined
# elif __cpp_inheriting_constructors < 201511L
#   error __cpp_inheriting_constructors does not suport C++17
# endif
# ifndef __cpp_inline_variables
#   error __cpp_inline_variables is not defined
# elif __cpp_inline_variables < 201606L
#   error __cpp_inline_variables does not suport C++17
# endif
# ifndef __cpp_noexcept_function_type
#   error __cpp_noexcept_function_type is not defined
# elif __cpp_noexcept_function_type < 201510L
#   error __cpp_noexcept_function_type does not suport C++17
# endif
# ifndef __cpp_nontype_template_parameter_auto
#   error __cpp_nontype_template_parameter_auto is not defined
# elif __cpp_nontype_template_parameter_auto < 201606L
#   error __cpp_nontype_template_parameter_auto does not suport C++17
# endif
# ifndef __cpp_static_assert
#   error __cpp_static_assert is not defined
# elif __cpp_static_assert < 201411L
#   error __cpp_static_assert does not suport C++17
# endif
# ifndef __cpp_structured_bindings
#   error __cpp_structured_bindings is not defined
# elif __cpp_structured_bindings < 201606L
#   error __cpp_structured_bindings does not suport C++17
# endif
# ifndef __cpp_variadic_using
#   error __cpp_variadic_using is not defined
# elif __cpp_variadic_using < 201611L
#   error __cpp_variadic_using does not suport C++17
# endif

// The Clang compiler did not define a few standard macros, even
// though the features were implemented, until release 19.0.  We
// will verify these cases in the regular test driver, to justify
// removing the enforcement from here.
# if !defined(BSLS_PLATFORM_CMP_CLANG) \
           || BSLS_PLATFORM_CMP_VERSION >= 190000
#   ifndef __cpp_sized_deallocation
#     error __cpp_sized_deallocation is not defined
#   elif __cpp_sized_deallocation < 201309L
#     error __cpp_sized_deallocation does not suport C++17
#   endif
#   ifndef __cpp_template_template_args
#     error __cpp_template_template_args is not defined
#   elif __cpp_template_template_args < 201611L
#     error __cpp_template_template_args does not suport C++17
#   endif
# endif
#endif // BSLS_COMPILERFEATURES_FULL_CPP17

// Note that BSLS_COMPILERFEATURES_FULL_CPP20 is not yet defined for
// any compiler that BDE supports.

#if defined(BSLS_COMPILERFEATURES_FULL_CPP20)
// __cpp_modules is *NOT* tested as part of this conformance feature
# ifndef __cpp_aggregate_paren_init
#   error __cpp_aggregate_paren_init is not defined
# elif __cpp_aggregate_paren_init < 201902L
#   error __cpp_aggregate_paren_init does not suport C++20
# endif
# ifndef __cpp_char8_t
#   error __cpp_char8_t is not defined
# elif __cpp_char8_t < 201811L
#   error __cpp_char8_t does not suport C++20
# endif
# ifndef __cpp_concepts
#   error __cpp_concepts is not defined
# elif __cpp_concepts < 201907L
#   error __cpp_concepts does not suport C++20
# endif
# ifndef __cpp_conditional_explicit
#   error __cpp_conditional_explicit is not defined
# elif __cpp_conditional_explicit < 201806L
#   error __cpp_conditional_explicit does not suport C++20
# endif
# ifndef __cpp_constexpr
#   error __cpp_constexpr is not defined
# elif __cpp_constexpr < 201907L
#   error __cpp_constexpr does not suport C++20
# endif
# ifndef __cpp_constexpr_dynamic_alloc
#   error __cpp_constexpr_dynamic_alloc is not defined
# elif __cpp_constexpr_dynamic_alloc < 201907L
#   error __cpp_constexpr_dynamic_alloc does not suport C++20
# endif
# ifndef __cpp_constexpr_in_decltype
#   error __cpp_constexpr_in_decltype is not defined
# elif __cpp_constexpr_in_decltype < 201711L
#   error __cpp_constexpr_in_decltype does not suport C++20
# endif
# ifndef __cpp_consteval
#   error __cpp_consteval is not defined
# elif __cpp_consteval < 201811L
#   error __cpp_consteval does not suport C++20
# endif
# ifndef __cpp_constinit
#   error __cpp_constinit is not defined
# elif __cpp_constinit < 201907L
#   error __cpp_constinit does not suport C++20
# endif
# ifndef __cpp_deduction_guides
#   error __cpp_deduction_guides is not defined
# elif __cpp_deduction_guides < 201907L
#   error __cpp_deduction_guides does not suport C++20
# endif
# ifndef __cpp_designated_initializers
#   error __cpp_designated_initializers is not defined
# elif __cpp_designated_initializers < 201707L
#   error __cpp_designated_initializers does not suport C++20
# endif
# ifndef __cpp_generic_lambdas
#   error __cpp_generic_lambdas is not defined
# elif __cpp_generic_lambdas < 201707L
#   error __cpp_generic_lambdas does not suport C++20
# endif
# ifndef __cpp_impl_coroutine
#   error __cpp_impl_coroutine is not defined
# elif __cpp_impl_coroutine < 201902L
#   error __cpp_impl_coroutine does not suport C++20
# endif
# ifndef __cpp_impl_destroying_delete
#   error __cpp_impl_destroying_delete is not defined
# elif __cpp_impl_destroying_delete < 201806L
#   error __cpp_impl_destroying_delete does not suport C++20
# endif
# ifndef __cpp_impl_three_way_comparison
#   error __cpp_impl_three_way_comparison is not defined
# elif __cpp_impl_three_way_comparison < 201907L
#   error __cpp_impl_three_way_comparison does not suport C++20
# endif
# ifndef __cpp_init_captures
#   error __cpp_init_captures is not defined
# elif __cpp_init_captures < 201803L
#   error __cpp_init_captures does not suport C++20
# endif
# ifndef __cpp_nontype_template_args
#   error __cpp_nontype_template_args is not defined
# elif __cpp_nontype_template_args < 201911L
#   error __cpp_nontype_template_args does not suport C++20
# endif
# ifndef __cpp_using_enum
#   error __cpp_using_enum is not defined
# elif __cpp_using_enum < 201907L
#   error __cpp_using_enum does not suport C++20
# endif
#endif // BSLS_COMPILERFEATURES_FULL_CPP20

// Note that BSLS_COMPILERFEATURES_FULL_CPP23 is not yet defined for
// any compiler that BDE supports.

#if defined(BSLS_COMPILERFEATURES_FULL_CPP23)
# ifndef __cpp_char8_t
#   error __cpp_char8_t is not defined
# elif __cpp_char8_t < 202207L
#   error __cpp_char8_t does not suport C++23
# endif
# ifndef __cpp_concepts
#   error __cpp_concepts is not defined
# elif __cpp_concepts < 202002L
#   error __cpp_concepts does not suport C++23
# endif
# ifndef __cpp_constexpr
#   error __cpp_constexpr is not defined
# elif __cpp_constexpr < 202211L
#   error __cpp_constexpr does not suport C++23
# endif
# ifndef __cpp_consteval
#   error __cpp_consteval is not defined
# elif __cpp_consteval < 202211L
#   error __cpp_consteval does not suport C++23
# endif
# ifndef __cpp_explicit_this_parameter
#   error __cpp_explicit_this_parameter is not defined
# elif __cpp_explicit_this_parameter < 202110L
#   error __cpp_explicit_this_parameter does not suport C++23
# endif
# ifndef __cpp_if_consteval
#   error __cpp_if_consteval is not defined
# elif __cpp_if_consteval < 202106L
#   error __cpp_if_consteval does not suport C++23
# endif
# ifndef __cpp_implicit_move
#   error __cpp_implicit_move is not defined
# elif __cpp_implicit_move < 202207L
#   error __cpp_implicit_move does not suport C++23
# endif
# ifndef __cpp_multidimensional_subscript
#   error __cpp_multidimensional_subscript is not defined
# elif __cpp_multidimensional_subscript < 202211L
#   error __cpp_multidimensional_subscript does not suport C++23
# endif
# ifndef __cpp_named_character_escapes
#   error __cpp_named_character_escapes is not defined
# elif __cpp_named_character_escapes < 202207L
#   error __cpp_named_character_escapes does not suport C++23
# endif
# ifndef __cpp_range_based_for
#   error __cpp_range_based_for is not defined
# elif __cpp_range_based_for < 202211L
#   error __cpp_range_based_for does not suport C++23
# endif
# ifndef __cpp_size_t_suffix
#   error __cpp_size_t_suffix is not defined
# elif __cpp_size_t_suffix < 201806L
#   error __cpp_size_t_suffix does not suport C++23
# endif
# ifndef __cpp_static_call_operator
#   error __cpp_static_call_operator is not defined
# elif __cpp_static_call_operator < 202207L
#   error __cpp_static_call_operator does not suport C++23
# endif
#endif // BSLS_COMPILERFEATURES_FULL_CPP23

// Note that BSLS_COMPILERFEATURES_FULL_CPP26 is not yet defined for
// any compiler that BDE supports.

#if defined(BSLS_COMPILERFEATURES_FULL_CPP26)
# ifndef __cpp_constexpr
#   error __cpp_constexpr is not defined
# elif __cpp_constexpr < 202406L
#   error __cpp_constexpr does not suport C++26
# endif
# ifndef __cpp_constexpr_exceptions
#   error __cpp_constexpr_exceptions is not defined
# elif __cpp_constexpr_exceptions < 202411L
#   error __cpp_constexpr_exceptions does not suport C++26
# endif
# ifndef __cpp_constexpr_virtual_inheritance
#   error __cpp_constexpr_virtual_inheritance is not defined
# elif __cpp_constexpr_virtual_inheritance < 202506L
#   error __cpp_constexpr_virtual_inheritance does not suport C++26
# endif
# ifndef __cpp_contracts
#   error __cpp_contracts is not defined
# elif __cpp_contracts < 202502L
#   error __cpp_contracts does not suport C++26
# endif
# ifndef __cpp_deleted_function
#   error __cpp_deleted_function is not defined
# elif __cpp_deleted_function < 202403L
#   error __cpp_deleted_function does not suport C++26
# endif
# ifndef __cpp_expansion_statements
#   error __cpp_expansion_statements is not defined
# elif __cpp_expansion_statements < 202506L
#   error __cpp_expansion_statements does not suport C++26
# endif
# ifndef __cpp_impl_reflection
#   error __cpp_impl_reflection is not defined
# elif __cpp_impl_reflection < 202506L
#   error __cpp_impl_reflection does not suport C++26
# endif
# ifndef __cpp_pack_indexing
#   error __cpp_pack_indexing is not defined
# elif __cpp_pack_indexing < 202311L
#   error __cpp_pack_indexing does not suport C++26
# endif
# ifndef __cpp_placeholder_variables
#   error __cpp_placeholder_variables is not defined
# elif __cpp_placeholder_variables < 202306L
#   error __cpp_placeholder_variables does not suport C++26
# endif
# ifndef __cpp_pp_embed
#   error __cpp_pp_embed is not defined
# elif __cpp_pp_embed < 202502L
#   error __cpp_pp_embed does not suport C++26
# endif
# ifndef __cpp_static_assert
#   error __cpp_static_assert is not defined
# elif __cpp_static_assert < 202306L
#   error __cpp_static_assert does not suport C++26
# endif
# ifndef __cpp_structured_bindings
#   error __cpp_structured_bindings is not defined
# elif __cpp_structured_bindings < 202411L
#   error __cpp_structured_bindings does not suport C++26
# endif
# ifndef __cpp_template_parameters
#   error __cpp_template_parameters is not defined
# elif __cpp_template_parameters < 202502L
#   error __cpp_template_parameters does not suport C++26
# endif
# ifndef __cpp_trivial_relocatability
#   error __cpp_trivial_relocatability is not defined
# elif __cpp_trivial_relocatability < 202502L
#   error __cpp_trivial_relocatability does not suport C++26
# endif
# ifndef __cpp_trivial_union
#   error __cpp_trivial_union is not defined
# elif __cpp_trivial_union < 202502L
#   error __cpp_trivial_union does not suport C++26
# endif
# ifndef __cpp_variadic_friend
#   error __cpp_variadic_friend is not defined
# elif __cpp_variadic_friend < 202403L
#   error __cpp_variadic_friend does not suport C++26
# endif
#endif // BSLS_COMPILERFEATURES_FULL_CPP26

// Set expectations for complete support when enabling earlier standards.

#if BSLS_COMPILERFEATURES_CPLUSPLUS >= 201103L
# if !defined(BSLS_COMPILERFEATURES_FULL_CPP11)
#   error Full C++11 support is expected when C++11 is enabled
# endif
#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
