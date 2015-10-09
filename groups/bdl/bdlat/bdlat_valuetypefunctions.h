// bdlat_valuetypefunctions.h                                         -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLAT_VALUETYPEFUNCTIONS
#define INCLUDED_BDLAT_VALUETYPEFUNCTIONS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a namespace for functions on value types.
//
//@CLASSES:
//  bdlat_ValueTypeFunctions: namespace for functions on value types
//
//@SEE_ALSO:
//
// TBD: update this documentation to reflect the new overloaded functions
//
//@DESCRIPTION: This component provides a 'namespace',
// 'bdlat_ValueTypeFunctions', defining functions that may be called on "value
// types".  This namespace contains two functions: 'assign' that performs a
// value assignment from 'rhs' to an object pointed to by 'lhs' (i.e., as if by
// using the assignment operator '*lhs = rhs';) and 'reset' that puts an
// object's black-box state to its default state (i.e., as if the object was
// just constructed using its default constructor).
//
// This component implements 'bdlat_ValueTypeFunctions' for:
//..
//      o any type with a default constructor and assignment operator
//      o any type with the 'bdlat_TypeTraitBasicSequence' trait
//      o any type with the 'bdlat_TypeTraitBasicChoice' trait
//      o any type with the 'bdlat_TypeTraitBasicCustomizedType' trait
//      o types instantiated from the 'bdlb::NullableValue' template
//      o types instantiated from the 'bsl::vector' template
//      o types instantiated from the 'bsl::basic_string' template
//..
// Third-party types may overload the 'bdlat_valueTypeReset' function to
// perform something more optimal than simply assigning the default value.
//
///Usage
///-----
// The following snippets of code illustrate the usage of this component.
// First, we construct a few objects of different types, assigning each of them
// a non-default value.  After calling 'reset' for all these object, we will
// assert that the object have the default value:
//..
//  #include <bdlat_valuetypefunctions.h>
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
//      // assert(0 == bdlat_ValueTypeFunctions::assign(&vecVal3, vecVal2));
//
//      bdlat_ValueTypeFunctions::reset(&intVal);
//      bdlat_ValueTypeFunctions::reset(&floatVal);
//      bdlat_ValueTypeFunctions::reset(&stringVal);
//      bdlat_ValueTypeFunctions::reset(&vecVal);
//
//      assert(0 == intVal);
//      assert(0 == floatVal);
//      assert(stringVal.empty());
//      assert(vecVal.empty());
//
//      // assert(vecVal2 == vecVal3);
//  }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLAT_BDEATOVERRIDES
#include <bdlat_bdeatoverrides.h>
#endif

#ifndef INCLUDED_BDLAT_ENUMFUNCTIONS
#include <bdlat_enumfunctions.h>
#endif

#ifndef INCLUDED_BDLAT_TYPECATEGORY
#include <bdlat_typecategory.h>
#endif

#ifndef INCLUDED_BDLAT_TYPETRAITS
#include <bdlat_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDLB_NULLABLEVALUE
#include <bdlb_nullablevalue.h>
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
                      // namespace bdlat_ValueTypeFunctions
                      // ==================================

namespace bdlat_ValueTypeFunctions {
    // The functions provided in this 'namespace' may be applied to value
    // types.  See the component-level documentation for what is meant by
    // "value type".

    template <class LHS_TYPE, class RHS_TYPE>
    int assign(LHS_TYPE *lhs, const RHS_TYPE& rhs);
        // Assign the value of the specified 'rhs' to the object specified its
        // address 'lhs'.  Return 0 if successful, and a non-zero value
        // otherwise.

    template <class TYPE>
    void reset(TYPE *object);
        // Reset the value of the specified 'object' to its default value.

    // OVERLOADABLE FUNCTIONS

    // The following functions should be overloaded for other types (in their
    // respective namespaces).  The following functions are the default
    // implementations (for 'bas_codegen.pl'-generated types).  Do *not* call
    // these functions directly.  Use the functions above instead.

#if ! defined(BSLS_PLATFORM_CMP_IBM)
    template <class LHS_TYPE, class RHS_TYPE>
    int bdlat_valueTypeAssign(LHS_TYPE *lhs, const RHS_TYPE& rhs);

    template <class TYPE>
    void bdlat_valueTypeReset(TYPE *object);
#endif

}  // close namespace bdlat_ValueTypeFunctions

// ---  Anything below this line is implementation specific.  Do not use.  ----

                    // ===================================
                    // struct bdlat_ValueTypeFunctions_Imp
                    // ===================================

struct bdlat_ValueTypeFunctions_Imp {
    // This 'struct' contains functions used by the implementation of this
    // component.

    // TAGS
    struct IsConvertible    { };
    struct IsNotConvertible { };
    struct UseResetMethod   { };
    struct UseDefaultCtor   { };

    // FUNCTIONS
    template <class LHS_TYPE>
    static int assign(LHS_TYPE                        *lhs,
                      bdlat_TypeCategory::Enumeration,
                      const char&                      rhs,
                      bdlat_TypeCategory::Simple);

    template <class LHS_TYPE>
    static int assign(LHS_TYPE                        *lhs,
                      bdlat_TypeCategory::Enumeration,
                      const short&                     rhs,
                      bdlat_TypeCategory::Simple);

    template <class LHS_TYPE>
    static int assign(LHS_TYPE                        *lhs,
                      bdlat_TypeCategory::Enumeration,
                      const int&                       rhs,
                      bdlat_TypeCategory::Simple);

    template <class LHS_TYPE>
    static int assign(LHS_TYPE                        *lhs,
                      bdlat_TypeCategory::Enumeration,
                      const bsl::string&               rhs,
                      bdlat_TypeCategory::Simple);

    template <class RHS_TYPE>
    static int assign(char                            *lhs,
                      bdlat_TypeCategory::Simple,
                      const RHS_TYPE&                  rhs,
                      bdlat_TypeCategory::Enumeration);

    template <class RHS_TYPE>
    static int assign(short                           *lhs,
                      bdlat_TypeCategory::Simple,
                      const RHS_TYPE&                  rhs,
                      bdlat_TypeCategory::Enumeration);

    template <class RHS_TYPE>
    static int assign(int                             *lhs,
                      bdlat_TypeCategory::Simple,
                      const RHS_TYPE&                  rhs,
                      bdlat_TypeCategory::Enumeration);

    template <class RHS_TYPE>
    static int assign(bsl::string                     *lhs,
                      bdlat_TypeCategory::Simple,
                      const RHS_TYPE&                  rhs,
                      bdlat_TypeCategory::Enumeration);

    template <class LHS_TYPE, class RHS_TYPE>
    static int assign(LHS_TYPE                   *lhs,
                      bdlat_TypeCategory::Simple,
                      const RHS_TYPE&             rhs,
                      bdlat_TypeCategory::Simple);

    template <class LHS_TYPE,
              class LHS_CATEGORY,
              class RHS_TYPE,
              class RHS_CATEGORY>
    static int assign(LHS_TYPE        *lhs,
                      LHS_CATEGORY,
                      const RHS_TYPE&  rhs,
                      RHS_CATEGORY);

    template <class LHS_TYPE, class RHS_TYPE>
    static int assignSimpleTypes(LHS_TYPE        *lhs,
                                 const RHS_TYPE&  rhs,
                                 IsConvertible);

    template <class LHS_TYPE, class RHS_TYPE>
    static int assignSimpleTypes(LHS_TYPE         *lhs,
                                 const RHS_TYPE&   rhs,
                                 IsNotConvertible);

    template <class TYPE>
    static void reset(TYPE *object);

    template <class TYPE>
    static void reset(bdlb::NullableValue<TYPE> *object);

    template <class TYPE, class ALLOC>
    static void reset(bsl::vector<TYPE, ALLOC> *object);

    template <class CHAR_T, class CHAR_TRAITS, class ALLOC>
    static void reset(bsl::basic_string<CHAR_T, CHAR_TRAITS, ALLOC> *object);

    template <class TYPE>
    static void reset(TYPE *object, UseResetMethod);

    template <class TYPE>
    static void reset(TYPE *object, UseDefaultCtor);

};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                      // ----------------------------------
                      // namespace bdlat_ValueTypeFunctions
                      // ----------------------------------

template <class LHS_TYPE, class RHS_TYPE>
inline
int bdlat_ValueTypeFunctions::assign(LHS_TYPE *lhs, const RHS_TYPE& rhs)
{
    return bdlat_valueTypeAssign(lhs, rhs);
}

template <class TYPE>
inline
void bdlat_ValueTypeFunctions::reset(TYPE *object)
{
    return bdlat_valueTypeReset(object);
}

        // -----------------------------------------------------------
        // namespace bdlat_ValueTypeFunctions (OVERLOADABLE FUNCTIONS)
        // -----------------------------------------------------------

#if defined(BSLS_PLATFORM_CMP_IBM)
namespace bdlat_ValueTypeFunctions {
    // xlC 6 will not do Koenig (argument-dependent) lookup if the function
    // being called has already been declared in some scope at the point of the
    // template function *definition* (not instantiation).  We work around this
    // bug by not declaring these functions until *after* the template
    // definitions that call them.

    template <typename LHS_TYPE, typename RHS_TYPE>
    int bdlat_valueTypeAssign(LHS_TYPE *lhs, const RHS_TYPE& rhs);

    template <typename TYPE>
    void bdlat_valueTypeReset(TYPE *object);

}  // close namespace bdlat_ValueTypeFunctions
#endif

template <class LHS_TYPE, class RHS_TYPE>
inline
int bdlat_ValueTypeFunctions::bdlat_valueTypeAssign(LHS_TYPE        *lhs,
                                                    const RHS_TYPE&  rhs)
{
    typedef typename bdlat_TypeCategory::Select<LHS_TYPE>::Type LhsCategory;
    typedef typename bdlat_TypeCategory::Select<RHS_TYPE>::Type RhsCategory;

    return bdlat_ValueTypeFunctions_Imp::assign(lhs,
                                                LhsCategory(),
                                                rhs,
                                                RhsCategory());
}

template <class TYPE>
inline
void bdlat_ValueTypeFunctions::bdlat_valueTypeReset(TYPE *object)
{
    bdlat_ValueTypeFunctions_Imp::reset(object);
}

                    // -----------------------------------
                    // struct bdlat_ValueTypeFunctions_Imp
                    // -----------------------------------

template <class LHS_TYPE>
int bdlat_ValueTypeFunctions_Imp::assign(LHS_TYPE                        *lhs,
                                         bdlat_TypeCategory::Enumeration,
                                         const char&                      rhs,
                                         bdlat_TypeCategory::Simple)
{
    return bdlat_EnumFunctions::fromInt(lhs, rhs);
}

template <class LHS_TYPE>
int bdlat_ValueTypeFunctions_Imp::assign(LHS_TYPE                        *lhs,
                                         bdlat_TypeCategory::Enumeration,
                                         const short&                     rhs,
                                         bdlat_TypeCategory::Simple)
{
    return bdlat_EnumFunctions::fromInt(lhs, rhs);
}

template <class LHS_TYPE>
int bdlat_ValueTypeFunctions_Imp::assign(LHS_TYPE                        *lhs,
                                         bdlat_TypeCategory::Enumeration,
                                         const int&                       rhs,
                                         bdlat_TypeCategory::Simple)
{
    return bdlat_EnumFunctions::fromInt(lhs, rhs);
}

template <class LHS_TYPE>
int bdlat_ValueTypeFunctions_Imp::assign(LHS_TYPE                        *lhs,
                                         bdlat_TypeCategory::Enumeration,
                                         const bsl::string&               rhs,
                                         bdlat_TypeCategory::Simple)
{
    return bdlat_EnumFunctions::fromString(lhs, rhs.data(), (int)rhs.length());
}

template <class RHS_TYPE>
int bdlat_ValueTypeFunctions_Imp::assign(char                            *lhs,
                                         bdlat_TypeCategory::Simple,
                                         const RHS_TYPE&                  rhs,
                                         bdlat_TypeCategory::Enumeration)
{
    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    const int MIN_CHAR = -128;
    const int MAX_CHAR = 127;

    int proxy;

    bdlat_EnumFunctions::toInt(&proxy, rhs);

    if (proxy < MIN_CHAR || proxy > MAX_CHAR) {
        return k_FAILURE;                                             // RETURN
    }

    *lhs = static_cast<char>(proxy);

    return k_SUCCESS;
}

template <class RHS_TYPE>
int bdlat_ValueTypeFunctions_Imp::assign(short                           *lhs,
                                         bdlat_TypeCategory::Simple,
                                         const RHS_TYPE&                  rhs,
                                         bdlat_TypeCategory::Enumeration)
{
    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    const int MIN_SHORT = -32768;
    const int MAX_SHORT = 32767;

    int proxy;

    bdlat_EnumFunctions::toInt(&proxy, rhs);

    if (proxy < MIN_SHORT || proxy > MAX_SHORT) {
        return k_FAILURE;                                             // RETURN
    }

    *lhs = static_cast<short>(proxy);

    return k_SUCCESS;
}

template <class RHS_TYPE>
int bdlat_ValueTypeFunctions_Imp::assign(int                             *lhs,
                                         bdlat_TypeCategory::Simple,
                                         const RHS_TYPE&                  rhs,
                                         bdlat_TypeCategory::Enumeration)
{
    enum { k_SUCCESS = 0 };

    bdlat_EnumFunctions::toInt(lhs, rhs);

    return k_SUCCESS;
}

template <class RHS_TYPE>
int bdlat_ValueTypeFunctions_Imp::assign(bsl::string                     *lhs,
                                         bdlat_TypeCategory::Simple,
                                         const RHS_TYPE&                  rhs,
                                         bdlat_TypeCategory::Enumeration)
{
    enum { k_SUCCESS = 0 };

    bdlat_EnumFunctions::toString(lhs, rhs);

    return k_SUCCESS;
}

template <class LHS_TYPE, class RHS_TYPE>
int bdlat_ValueTypeFunctions_Imp::assign(LHS_TYPE                   *lhs,
                                         bdlat_TypeCategory::Simple,
                                         const RHS_TYPE&             rhs,
                                         bdlat_TypeCategory::Simple)
{
    enum {
        IS_CONVERTIBLE = bslmf::IsConvertible<RHS_TYPE, LHS_TYPE>::VALUE
    };

    typedef typename
    bslmf::If<IS_CONVERTIBLE, IsConvertible, IsNotConvertible>::Type Selector;

    return assignSimpleTypes(lhs, rhs, Selector());
}

template <class LHS_TYPE,
          class LHS_CATEGORY,
          class RHS_TYPE,
          class RHS_CATEGORY>
int bdlat_ValueTypeFunctions_Imp::assign(LHS_TYPE        *,
                                         LHS_CATEGORY,
                                         const RHS_TYPE&,
                                         RHS_CATEGORY)
{
    enum { k_FAILURE = -1 };

    return k_FAILURE;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
int bdlat_ValueTypeFunctions_Imp::assignSimpleTypes(LHS_TYPE        *lhs,
                                                    const RHS_TYPE&  rhs,
                                                    IsConvertible)
{
    enum { k_SUCCESS = 0 };

    *lhs = static_cast<LHS_TYPE>(rhs);

    return k_SUCCESS;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
int bdlat_ValueTypeFunctions_Imp::assignSimpleTypes(LHS_TYPE        *,
                                                    const RHS_TYPE&,
                                                    IsNotConvertible)
{
    enum { k_FAILURE = -1 };

    return k_FAILURE;
}

template <class TYPE>
inline
void bdlat_ValueTypeFunctions_Imp::reset(TYPE *object)
{
    enum {
        HAS_TRAIT = bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicChoice>::VALUE
                 || bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicSequence>::VALUE
                 || bslalg::HasTrait<TYPE,
                                    bdlat_TypeTraitBasicCustomizedType>::VALUE
    };

    typedef typename
    bslmf::If<HAS_TRAIT,
             bdlat_ValueTypeFunctions_Imp::UseResetMethod,
             bdlat_ValueTypeFunctions_Imp::UseDefaultCtor>::Type Selector;

    bdlat_ValueTypeFunctions_Imp::reset(object, Selector());
}

template <class TYPE>
inline
void bdlat_ValueTypeFunctions_Imp::reset(bdlb::NullableValue<TYPE> *object)
{
    object->reset();
}

template <class TYPE, class ALLOC>
inline
void bdlat_ValueTypeFunctions_Imp::reset(bsl::vector<TYPE, ALLOC> *object)
{
    object->clear();
}

template <class CHAR_T, class CHAR_TRAITS, class ALLOC>
inline
void bdlat_ValueTypeFunctions_Imp::reset(
                         bsl::basic_string<CHAR_T, CHAR_TRAITS, ALLOC> *object)
{
    object->erase();
}

template <class TYPE>
inline
void bdlat_ValueTypeFunctions_Imp::reset(TYPE *object, UseResetMethod)
{
    object->reset();
}

template <class TYPE>
inline
void bdlat_ValueTypeFunctions_Imp::reset(TYPE *object, UseDefaultCtor)
{
    *object = TYPE();
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
