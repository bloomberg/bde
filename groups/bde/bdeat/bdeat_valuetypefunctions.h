// bdeat_valuetypefunctions.h                                         -*-C++-*-
#ifndef INCLUDED_BDEAT_VALUETYPEFUNCTIONS
#define INCLUDED_BDEAT_VALUETYPEFUNCTIONS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a namespace for functions on value types.
//
//@CLASSES:
//  bdeat_ValueTypeFunctions: namespace for functions on value types
//
//@SEE_ALSO:
//
//@AUTHOR: Clay Wilson (cwilson9), Shezan Baig (sbaig)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
// TBD: update this documentation to reflect the new overloaded functions
//
//@DESCRIPTION: This component provides a 'namespace',
// 'bdeat_ValueTypeFunctions', defining functions that may be called on "value
// types".  This namespace contains two functions: 'assign' that performs a
// value assignment from 'rhs' to an object pointed to by 'lhs' (i.e., as if
// by using the assignment operator '*lhs = rhs';) and 'reset' that puts an
// object's black-box state to its default state (i.e., as if the object was
// just constructed using its default constructor).
//
// This component implements 'bdeat_ValueTypeFunctions' for:
//..
//      o any type with a default constructor and assignment operator
//      o any type with the 'bdeat_TypeTraitBasicSequence' trait
//      o any type with the 'bdeat_TypeTraitBasicChoice' trait
//      o any type with the 'bdeat_TypeTraitBasicCustomizedType' trait
//      o types instantiated from the 'bdeut_NullableValue' template
//      o types instantiated from the 'bsl::vector' template
//      o types instantiated from the 'bsl::basic_string' template
//..
// Third-party types may overload the 'bdeat_valueTypeReset' function to
// perform something more optimal than simply assigning the default value.
//
///Usage
///-----
// The following snippets of code illustrate the usage of this component.
// First, we construct a few objects of different types, assigning each of them
// a non-default value.  After calling 'reset' for all these object, we will
// assert that the object have the default value:
//..
//  #include <bdeat_valuetypefunctions.h>
//  #include <string>
//  #include <vector>
//
//  void usageExample()
//  {
//      using namespace BloombergLP;
//
//      int               intVal    = 123;
//      float             floatVal  = 34.56;
//      bsl::string       stringVal = "Hello";
//      bsl::vector<char> vecVal, vecVal2, vecVal3;
//
//      vecVal.push_back('T');
//      vecVal.push_back('e');
//      vecVal.push_back('s');
//      vecVal.push_back('t');
//
//      vecVal2 = vecVal;
//
//      // assert(0 == bdeat_ValueTypeFunctions::assign(&vecVal3, vecVal2));
//
//      bdeat_ValueTypeFunctions::reset(&intVal);
//      bdeat_ValueTypeFunctions::reset(&floatVal);
//      bdeat_ValueTypeFunctions::reset(&stringVal);
//      bdeat_ValueTypeFunctions::reset(&vecVal);
//
//      assert(0 == intVal);
//      assert(0 == floatVal);
//      assert(stringVal.empty());
//      assert(vecVal.empty());
//
//      // assert(vecVal2 == vecVal3);
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEAT_ENUMFUNCTIONS
#include <bdeat_enumfunctions.h>
#endif

#ifndef INCLUDED_BDEAT_TYPECATEGORY
#include <bdeat_typecategory.h>
#endif

#ifndef INCLUDED_BDEAT_TYPETRAITS
#include <bdeat_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDEUT_NULLABLEVALUE
#include <bdeut_nullablevalue.h>
#endif

#ifndef INCLUDED_BSLMF_IF
#include <bslmf_if.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#include <bslmf_isconvertible.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

                      // ==================================
                      // namespace bdeat_ValueTypeFunctions
                      // ==================================

namespace bdeat_ValueTypeFunctions {
    // The functions provided in this 'namespace' may be applied to value
    // types.  See the component-level documentation for what is meant by
    // "value type".

    template <typename LHS_TYPE, typename RHS_TYPE>
    int assign(LHS_TYPE *lhs, const RHS_TYPE& rhs);
        // Assign the value of the specified 'rhs' to the object specified its
        // address 'lhs'.  Return 0 if successful, and a non-zero value
        // otherwise.

    template <typename TYPE>
    void reset(TYPE *object);
        // Reset the value of the specified 'object' to its default value.

    // OVERLOADABLE FUNCTIONS
    // The following functions should be overloaded for other types (in their
    // respective namespaces).  The following functions are the default
    // implementations (for 'bas_codegen.pl'-generated types).  Do *not* call
    // these functions directly.  Use the functions above instead.

#if ! defined(BSLS_PLATFORM__CMP_IBM)
    template <typename LHS_TYPE, typename RHS_TYPE>
    int bdeat_valueTypeAssign(LHS_TYPE *lhs, const RHS_TYPE& rhs);

    template <typename TYPE>
    void bdeat_valueTypeReset(TYPE *object);
#endif

}  // close namespace bdeat_ValueTypeFunctions

// ---  Anything below this line is implementation specific.  Do not use.  ----

                    // ===================================
                    // struct bdeat_ValueTypeFunctions_Imp
                    // ===================================

struct bdeat_ValueTypeFunctions_Imp {
    // This 'struct' contains functions used by the implementation of this
    // component.

    // TAGS
    struct IsConvertible    { };
    struct IsNotConvertible { };
    struct UseResetMethod   { };
    struct UseDefaultCtor   { };

    // FUNCTIONS
    template <typename LHS_TYPE>
    static int assign(LHS_TYPE    *lhs,
                      bdeat_TypeCategory::Enumeration,
                      const char&  rhs,
                      bdeat_TypeCategory::Simple);

    template <typename LHS_TYPE>
    static int assign(LHS_TYPE     *lhs,
                      bdeat_TypeCategory::Enumeration,
                      const short&  rhs,
                      bdeat_TypeCategory::Simple);

    template <typename LHS_TYPE>
    static int assign(LHS_TYPE   *lhs,
                      bdeat_TypeCategory::Enumeration,
                      const int&  rhs,
                      bdeat_TypeCategory::Simple);

    template <typename LHS_TYPE>
    static int assign(LHS_TYPE           *lhs,
                      bdeat_TypeCategory::Enumeration,
                      const bsl::string&  rhs,
                      bdeat_TypeCategory::Simple);

    template <typename RHS_TYPE>
    static int assign(char            *lhs,
                      bdeat_TypeCategory::Simple,
                      const RHS_TYPE&  rhs,
                      bdeat_TypeCategory::Enumeration);

    template <typename RHS_TYPE>
    static int assign(short           *lhs,
                      bdeat_TypeCategory::Simple,
                      const RHS_TYPE&  rhs,
                      bdeat_TypeCategory::Enumeration);

    template <typename RHS_TYPE>
    static int assign(int             *lhs,
                      bdeat_TypeCategory::Simple,
                      const RHS_TYPE&  rhs,
                      bdeat_TypeCategory::Enumeration);

    template <typename RHS_TYPE>
    static int assign(bsl::string     *lhs,
                      bdeat_TypeCategory::Simple,
                      const RHS_TYPE&  rhs,
                      bdeat_TypeCategory::Enumeration);

    template <typename LHS_TYPE, typename RHS_TYPE>
    static int assign(LHS_TYPE        *lhs,
                      bdeat_TypeCategory::Simple,
                      const RHS_TYPE&  rhs,
                      bdeat_TypeCategory::Simple);

    template <typename LHS_TYPE, typename LHS_CATEGORY,
              typename RHS_TYPE, typename RHS_CATEGORY>
    static int assign(LHS_TYPE        *lhs,
                      LHS_CATEGORY,
                      const RHS_TYPE&  rhs,
                      RHS_CATEGORY);

    template <typename LHS_TYPE, typename RHS_TYPE>
    static int assignSimpleTypes(LHS_TYPE *lhs, const RHS_TYPE& rhs,
                                 IsConvertible);

    template <typename LHS_TYPE, typename RHS_TYPE>
    static int assignSimpleTypes(LHS_TYPE *lhs, const RHS_TYPE& rhs,
                                 IsNotConvertible);

    template <typename TYPE>
    static void reset(TYPE *object);

    template <typename TYPE>
    static void reset(bdeut_NullableValue<TYPE> *object);

    template <typename TYPE, typename ALLOC>
    static void reset(bsl::vector<TYPE, ALLOC> *object);

    template <typename CHAR_T, typename CHAR_TRAITS, typename ALLOC>
    static void reset(bsl::basic_string<CHAR_T, CHAR_TRAITS, ALLOC> *object);

    template <typename TYPE>
    static void reset(TYPE *object, UseResetMethod);

    template <typename TYPE>
    static void reset(TYPE *object, UseDefaultCtor);

};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                      // ----------------------------------
                      // namespace bdeat_ValueTypeFunctions
                      // ----------------------------------

template <typename LHS_TYPE, typename RHS_TYPE>
inline
int bdeat_ValueTypeFunctions::assign(LHS_TYPE *lhs, const RHS_TYPE& rhs)
{
    return bdeat_valueTypeAssign(lhs, rhs);
}

template <typename TYPE>
inline
void bdeat_ValueTypeFunctions::reset(TYPE *object)
{
    return bdeat_valueTypeReset(object);
}

        // -----------------------------------------------------------
        // namespace bdeat_ValueTypeFunctions (OVERLOADABLE FUNCTIONS)
        // -----------------------------------------------------------

#if defined(BSLS_PLATFORM__CMP_IBM)
namespace bdeat_ValueTypeFunctions {
    // xlC 6 will not do Koenig (argument-dependent) lookup if the function
    // being called has already been declared in some scope at the point of
    // the template function *definition* (not instantiation).  We work around
    // this bug by not declaring these functions until *after* the template
    // definitions that call them.

    template <typename LHS_TYPE, typename RHS_TYPE>
    int bdeat_valueTypeAssign(LHS_TYPE *lhs, const RHS_TYPE& rhs);

    template <typename TYPE>
    void bdeat_valueTypeReset(TYPE *object);

}  // close namespace bdeat_ValueTypeFunctions
#endif

template <typename LHS_TYPE, typename RHS_TYPE>
inline
int bdeat_ValueTypeFunctions::bdeat_valueTypeAssign(LHS_TYPE        *lhs,
                                                    const RHS_TYPE&  rhs)
{
    typedef typename bdeat_TypeCategory::Select<LHS_TYPE>::Type LhsCategory;
    typedef typename bdeat_TypeCategory::Select<RHS_TYPE>::Type RhsCategory;

    return bdeat_ValueTypeFunctions_Imp::assign(lhs,
                                                LhsCategory(),
                                                rhs,
                                                RhsCategory());
}

template <typename TYPE>
inline
void bdeat_ValueTypeFunctions::bdeat_valueTypeReset(TYPE *object)
{
    bdeat_ValueTypeFunctions_Imp::reset(object);
}

                    // -----------------------------------
                    // struct bdeat_ValueTypeFunctions_Imp
                    // -----------------------------------

template <typename LHS_TYPE>
int bdeat_ValueTypeFunctions_Imp::assign(LHS_TYPE                         *lhs,
                                         bdeat_TypeCategory::Enumeration,
                                         const char&                       rhs,
                                         bdeat_TypeCategory::Simple)
{
    return bdeat_EnumFunctions::fromInt(lhs, rhs);
}

template <typename LHS_TYPE>
int bdeat_ValueTypeFunctions_Imp::assign(LHS_TYPE                         *lhs,
                                         bdeat_TypeCategory::Enumeration,
                                         const short&                      rhs,
                                         bdeat_TypeCategory::Simple)
{
    return bdeat_EnumFunctions::fromInt(lhs, rhs);
}

template <typename LHS_TYPE>
int bdeat_ValueTypeFunctions_Imp::assign(LHS_TYPE                         *lhs,
                                         bdeat_TypeCategory::Enumeration,
                                         const int&                        rhs,
                                         bdeat_TypeCategory::Simple)
{
    return bdeat_EnumFunctions::fromInt(lhs, rhs);
}

template <typename LHS_TYPE>
int bdeat_ValueTypeFunctions_Imp::assign(LHS_TYPE                         *lhs,
                                         bdeat_TypeCategory::Enumeration,
                                         const bsl::string&                rhs,
                                         bdeat_TypeCategory::Simple)
{
    return bdeat_EnumFunctions::fromString(lhs, rhs.data(), (int)rhs.length());
}

template <typename RHS_TYPE>
int bdeat_ValueTypeFunctions_Imp::assign(char                             *lhs,
                                         bdeat_TypeCategory::Simple,
                                         const RHS_TYPE&                   rhs,
                                         bdeat_TypeCategory::Enumeration)
{
    enum { BDEAT_SUCCESS = 0, BDEAT_FAILURE = -1 };

    const int MIN_CHAR = -128;
    const int MAX_CHAR = 127;

    int proxy;

    bdeat_EnumFunctions::toInt(&proxy, rhs);

    if (proxy < MIN_CHAR || proxy > MAX_CHAR) {
        return BDEAT_FAILURE;
    }

    *lhs = static_cast<char>(proxy);

    return BDEAT_SUCCESS;
}

template <typename RHS_TYPE>
int bdeat_ValueTypeFunctions_Imp::assign(short                            *lhs,
                                         bdeat_TypeCategory::Simple,
                                         const RHS_TYPE&                   rhs,
                                         bdeat_TypeCategory::Enumeration)
{
    enum { BDEAT_SUCCESS = 0, BDEAT_FAILURE = -1 };

    const int MIN_SHORT = -32768;
    const int MAX_SHORT = 32767;

    int proxy;

    bdeat_EnumFunctions::toInt(&proxy, rhs);

    if (proxy < MIN_SHORT || proxy > MAX_SHORT) {
        return BDEAT_FAILURE;
    }

    *lhs = static_cast<short>(proxy);

    return BDEAT_SUCCESS;
}

template <typename RHS_TYPE>
int bdeat_ValueTypeFunctions_Imp::assign(int                              *lhs,
                                         bdeat_TypeCategory::Simple,
                                         const RHS_TYPE&                   rhs,
                                         bdeat_TypeCategory::Enumeration)
{
    enum { BDEAT_SUCCESS = 0 };

    bdeat_EnumFunctions::toInt(lhs, rhs);

    return BDEAT_SUCCESS;
}

template <typename RHS_TYPE>
int bdeat_ValueTypeFunctions_Imp::assign(bsl::string                      *lhs,
                                         bdeat_TypeCategory::Simple,
                                         const RHS_TYPE&                   rhs,
                                         bdeat_TypeCategory::Enumeration)
{
    enum { BDEAT_SUCCESS = 0 };

    bdeat_EnumFunctions::toString(lhs, rhs);

    return BDEAT_SUCCESS;
}

template <typename LHS_TYPE, typename RHS_TYPE>
int bdeat_ValueTypeFunctions_Imp::assign(LHS_TYPE                    *lhs,
                                         bdeat_TypeCategory::Simple,
                                         const RHS_TYPE&              rhs,
                                         bdeat_TypeCategory::Simple)
{
    enum {
        IS_CONVERTIBLE = bslmf_IsConvertible<RHS_TYPE, LHS_TYPE>::VALUE
    };

    typedef typename
    bslmf_If<IS_CONVERTIBLE, IsConvertible, IsNotConvertible>::Type Selector;

    return assignSimpleTypes(lhs, rhs, Selector());
}

template <typename LHS_TYPE, typename LHS_CATEGORY,
          typename RHS_TYPE, typename RHS_CATEGORY>
int bdeat_ValueTypeFunctions_Imp::assign(LHS_TYPE        *lhs,
                                         LHS_CATEGORY,
                                         const RHS_TYPE&  rhs,
                                         RHS_CATEGORY)
{
    enum { BDEAT_FAILURE = -1 };

    (void)lhs;  // quell warning
    (void)rhs;  // quell warning
    return BDEAT_FAILURE;
}

template <typename LHS_TYPE, typename RHS_TYPE>
inline
int bdeat_ValueTypeFunctions_Imp::assignSimpleTypes(LHS_TYPE        *lhs,
                                                    const RHS_TYPE&  rhs,
                                                    IsConvertible)
{
    enum { BDEAT_SUCCESS = 0 };

    *lhs = static_cast<LHS_TYPE>(rhs);

    return BDEAT_SUCCESS;
}

template <typename LHS_TYPE, typename RHS_TYPE>
inline
int bdeat_ValueTypeFunctions_Imp::assignSimpleTypes(LHS_TYPE        *lhs,
                                                    const RHS_TYPE&  rhs,
                                                    IsNotConvertible)
{
    enum { BDEAT_FAILURE = -1 };

    (void)lhs;  // quell warning
    (void)rhs;  // quell warning
    return BDEAT_FAILURE;
}

template <typename TYPE>
inline
void bdeat_ValueTypeFunctions_Imp::reset(TYPE *object)
{
    enum {
        HAS_TRAIT = bslalg_HasTrait<TYPE, bdeat_TypeTraitBasicChoice>::VALUE
                 || bslalg_HasTrait<TYPE, bdeat_TypeTraitBasicSequence>::VALUE
                 || bslalg_HasTrait<TYPE,
                                    bdeat_TypeTraitBasicCustomizedType>::VALUE
    };

    typedef typename
    bslmf_If<HAS_TRAIT,
             bdeat_ValueTypeFunctions_Imp::UseResetMethod,
             bdeat_ValueTypeFunctions_Imp::UseDefaultCtor>::Type Selector;

    bdeat_ValueTypeFunctions_Imp::reset(object, Selector());
}

template <typename TYPE>
inline
void bdeat_ValueTypeFunctions_Imp::reset(bdeut_NullableValue<TYPE> *object)
{
    object->reset();
}

template <typename TYPE, typename ALLOC>
inline
void bdeat_ValueTypeFunctions_Imp::reset(bsl::vector<TYPE, ALLOC> *object)
{
    object->clear();
}

template <typename CHAR_T, typename CHAR_TRAITS, typename ALLOC>
inline
void bdeat_ValueTypeFunctions_Imp::reset(
                         bsl::basic_string<CHAR_T, CHAR_TRAITS, ALLOC> *object)
{
    object->erase();
}

template <typename TYPE>
inline
void bdeat_ValueTypeFunctions_Imp::reset(TYPE            *object,
                                         UseResetMethod)
{
    object->reset();
}

template <typename TYPE>
inline
void bdeat_ValueTypeFunctions_Imp::reset(TYPE            *object,
                                         UseDefaultCtor)
{
    *object = TYPE();
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
