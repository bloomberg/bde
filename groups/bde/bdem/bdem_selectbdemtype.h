// bdem_selectbdemtype.h                                              -*-C++-*-
#ifndef INCLUDED_BDEM_SELECTBDEMTYPE
#define INCLUDED_BDEM_SELECTBDEMTYPE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for selecting 'bdem' and C++ types.
//
//@CLASSES:
//   bdem_SelectBdemType: meta-function mapping 'TYPE' to 'bdem' and C++ types
//
//@SEE_ALSO: bdem_elemtype
//
//@AUTHOR: Clay Wilson (cwilson9), Shezan Baig (sbaig)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION: The 'bdem_SelectBdemType' meta-function supplied by this
// component maps a parameterized 'TYPE' to its corresponding 'bdem_ElemType'
// enumerator and its corresponding C++ type.  The resultant 'bdem_ElemType'
// enumerator and C++ types are made available via the
// 'bdem_SelectBdemType<TYPE>::VALUE' compile-time constant and the
// 'bdem_SelectBdemType<TYPE>::Type' 'typedef', respectively.
//
// This component recognizes 'bdeat' types.  In particular, types that fall
// into one of the following categories (as defined in 'bdeat_typecategory')
// will be mapped appropriately (as described in the subsequent sections):
//..
//    Category           Reference
//    --------           ---------
//    Array              bdeat_arrayfunctions
//    CustomizedType     bdeat_customizedtypefunctions
//    Enumeration        bdeat_enumeration
//    NullableValue      bdeat_nullablevaluefunctions
//..
// Types that do not fall in any of these categories are mapped as follows:
//..
//    Type                        bdem_ElemType     C++ Type
//    ----                        -------------     --------
//    bool                        BDEM_BOOL         bool
//    char                        BDEM_CHAR         char
//    signed char                 BDEM_CHAR         char
//    unsigned char               BDEM_CHAR         char
//    short                       BDEM_SHORT        short
//    unsigned short              BDEM_SHORT        short
//    wchar_t                     BDEM_SHORT        short
//    int                         BDEM_INT          int
//    unsigned int                BDEM_INT          int
//    bsls_Types::Int64           BDEM_INT64        bsls_Types::Int64
//    bsls_Types::Uint64          BDEM_INT64        bsls_Types::Int64
//    float                       BDEM_FLOAT        float
//    double                      BDEM_DOUBLE       double
//    bsl::string                 BDEM_STRING       bsl::string
//    bdet_Date                   BDEM_DATE         bdet_Date
//    bdet_Datetime               BDEM_DATETIME     bdet_Datetime
//    bdet_Time                   BDEM_TIME         bdet_Time
//    'enum' types                BDEM_INT          int
//    other types                 BDEM_LIST         bdem_List
//    bdet_DatetimeTz             BDEM_DATETIMETZ   bdet_DatetimeTz
//    bdet_DateTz                 BDEM_DATETZ       bdet_DateTz
//    bdet_TimeTz                 BDEM_TIMETZ       bdet_TimeTz
//    bdem_Choice                 BDEM_CHOICE       bdem_Choice
//    bdem_Row                    BDEM_ROW          bdem_Row
//..
///Behavior for 'Array' Category
///-----------------------------
// When the parameterized 'TYPE' is an 'Array', the corresponding
// 'bdem_ElemType' of the array's element type is used to determine the array's
// 'bdem_ElemType' and C++ type.  The array's element type is obtained using
// 'bdeat_ArrayFunctions::ElementType<TYPE>::Type'.  The mapping is then
// performed based on the following table:
//..
//  -- array element --     ---------------------- array ----------------------
//  bdem_ElemType           bdem_ElemType          C++ Type
//  ----------------------  ---------------------  ----------------------------
//  BDEM_BOOL               BDEM_BOOL_ARRAY        bsl::vector<bool>
//  BDEM_CHAR               BDEM_CHAR_ARRAY        bsl::vector<char>
//  BDEM_SHORT              BDEM_SHORT_ARRAY       bsl::vector<short>
//  BDEM_INT                BDEM_INT_ARRAY         bsl::vector<int>
//  BDEM_INT64              BDEM_INT64_ARRAY       bsl::vector<
//                                                           bsls_Types::Int64>
//  BDEM_FLOAT              BDEM_FLOAT_ARRAY       bsl::vector<float>
//  BDEM_DOUBLE             BDEM_DOUBLE_ARRAY      bsl::vector<double>
//  BDEM_STRING             BDEM_STRING_ARRAY      bsl::vector<bsl::string>
//  BDEM_DATE               BDEM_DATE_ARRAY        bsl::vector<bdet_Date>
//  BDEM_DATETIME           BDEM_DATETIME_ARRAY    bsl::vector<bdet_Datetime>
//  BDEM_TIME               BDEM_TIME_ARRAY        bsl::vector<bdet_Time>
//  BDEM_LIST               BDEM_TABLE             bdem_Table
//  BDEM_DATETIMETZ         BDEM_DATETIMETZ_ARRAY  bsl::vector<bdet_DatetimeTz>
//  BDEM_DATETZ             BDEM_DATETZ_ARRAY      bsl::vector<bdet_DateTz>
//  BDEM_TIMETZ             BDEM_TIMETZ_ARRAY      bsl::vector<bdet_TimeTz>
//  BDEM_CHOICE             BDEM_CHOICE_ARRAY      bdem_ChoiceArray
//  BDEM_ROW                BDEM_TABLE             bdem_Row
//  BDEM_CHOICE_ARRAY_ITEM  BDEM_CHOICE_ARRAY      bdem_ChoiceArray
//..
// Note that 'const' and 'volatile' qualifiers are removed from the array's
// element type before evaluating its 'bdem_ElemType'.
//
///Behavior for 'CustomizedType' Category
///--------------------------------------
// The 'bdem_ElemType' and C++ type of the base type are used for the
// customized type's 'bdem_ElemType' and C++ type.  The base type is obtained
// using 'bdeat_CustomizedTypeFunctions::BaseType<TYPE>::Type'.
//
// Note that 'const' and 'volatile' qualifiers are removed from the base type
// before evaluating the result.
//
///Behavior for 'Enumeration' Category
///-----------------------------------
// The 'bdem_ElemType' is 'INT' and the C++ type is 'int'.
//
///Behavior for 'NullableValue' Category
///-------------------------------------
// The 'bdem_ElemType' and C++ type of the value type are used for the nullable
// value's 'bdem_ElemType' and C++ type.  The value type is obtained using
// 'bdeat_NullableValueFunctions::ValueType<TYPE>::Type'.
//
// Note that 'const' and 'volatile' qualifiers are removed from the value type
// before evaluating the result.
//
///Usage
///-----
// The following snippets of code illustrate basic usage of this component.
// Note that the 'bslmf_IsSame' meta-function is employed only for the
// purposes of illustration:
//..
//  class Class {};
//  enum Enum {};
//
//  // 'bool' -> 'bool'
//  assert(bdem_ElemType::BDEM_BOOL == bdem_SelectBdemType<bool>::VALUE);
//  typedef bdem_SelectBdemType<bool>::Type boolProxyType;
//  assert(1 == (bslmf_IsSame<bool, boolProxyType>::VALUE));
//
//  // 'unsigned' -> 'int'
//  assert(bdem_ElemType::BDEM_INT == bdem_SelectBdemType<unsigned>::VALUE);
//  typedef bdem_SelectBdemType<unsigned>::Type unsignedProxyType;
//  assert(1 == (bslmf_IsSame<int, unsignedProxyType>::VALUE));
//
//  // 'Enum' -> 'int'
//  assert(bdem_ElemType::BDEM_INT == bdem_SelectBdemType<Enum>::VALUE);
//  typedef bdem_SelectBdemType<Enum>::Type enumProxyType;
//  assert(1 == (bslmf_IsSame<int, enumProxyType>::VALUE));
//
//  // 'bsl::vector<Enum>' -> 'bsl::vector<int>'
//  assert(bdem_ElemType::BDEM_INT_ARRAY ==
//                             bdem_SelectBdemType<bsl::vector<Enum> >::VALUE);
//  typedef bdem_SelectBdemType<bsl::vector<Enum> >::Type enumVectorProxyType;
//  assert(1 == (bslmf_IsSame<bsl::vector<int>, enumVectorProxyType>::VALUE));
//
//  // 'Class' -> 'bdem_List'
//  assert(bdem_ElemType::BDEM_LIST == bdem_SelectBdemType<Class>::VALUE);
//  typedef bdem_SelectBdemType<Class>::Type classProxyType;
//  assert(1 == (bslmf_IsSame<bdem_List, classProxyType>::VALUE));
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEAT_ARRAYFUNCTIONS
#include <bdeat_arrayfunctions.h>
#endif

#ifndef INCLUDED_BDEAT_CUSTOMIZEDTYPEFUNCTIONS
#include <bdeat_customizedtypefunctions.h>
#endif

#ifndef INCLUDED_BDEAT_NULLABLEVALUEFUNCTIONS
#include <bdeat_nullablevaluefunctions.h>
#endif

#ifndef INCLUDED_BDEAT_TYPECATEGORY
#include <bdeat_typecategory.h>
#endif

#ifndef INCLUDED_BDEM_ELEMTYPE
#include <bdem_elemtype.h>
#endif

#ifndef INCLUDED_BSLMF_IF
#include <bslmf_if.h>
#endif

#ifndef INCLUDED_BSLMF_ISENUM
#include <bslmf_isenum.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECVQ
#include <bslmf_removecvq.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVEREFERENCE
#include <bslmf_removereference.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

class bdem_List;
class bdem_Table;
class bdet_Date;
class bdet_Datetime;
class bdet_Time;
class bdet_DatetimeTz;
class bdet_DateTz;
class bdet_TimeTz;
class bdem_Choice;
class bdem_ChoiceArray;
class bdem_Row;
class bdem_ChoiceArrayItem;

template <typename TYPE, typename TYPE_CATEGORY>
struct bdem_SelectBdemType_ByCategory;

template <int ELEM_TYPE>
struct bdem_SelectBdemType_GetCppType;

template <typename TYPE>
struct bdem_SelectBdemType_Imp;

template <int ELEM_TYPE>
struct bdem_SelectBdemType_ToArrayElemType;

                         // ==========================
                         // struct bdem_SelectBdemType
                         // ==========================

template <typename TYPE>
struct bdem_SelectBdemType {
    // This meta-function contains a 'VALUE' compile-time constant that
    // indicates the corresponding 'bdem_ElemType::Type' value for the
    // parameterized 'TYPE'.  It also contains the 'Type' typedef that
    // indicates the corresponding C++ type used for the parameterized 'TYPE'.

  private:
    typedef bdem_SelectBdemType_Imp<TYPE> Imp;

  public:
    enum { VALUE = Imp::VALUE };  // value indicating the corresponding
                                  // 'bdem_ElemType::Type' for the
                                  // parameterized 'TYPE'

    typedef typename
    bdem_SelectBdemType_GetCppType<VALUE>::Type Type;
                                       // type indicating the corresponding C++
                                       // type for the parameterized 'TYPE'
};

// ---  Anything below this line is implementation specific.  Do not use.  ----

         // ==========================================================
         // struct bdem_SelectBdemType_ByCategory<TYPE, TYPE_CATEGORY>
         // ==========================================================

template <typename TYPE, typename TYPE_CATEGORY>
struct bdem_SelectBdemType_ByCategory {

  private:
    enum { IS_ENUM = bslmf_IsEnum<TYPE>::VALUE };

  public:
    enum {
        VALUE = IS_ENUM ? bdem_ElemType::BDEM_INT : bdem_ElemType::BDEM_LIST
    };
};

             // ==================================================
             // struct bdem_SelectBdemType_ByCategory<TYPE, Array>
             // ==================================================

template <typename TYPE>
struct bdem_SelectBdemType_ByCategory<TYPE, bdeat_TypeCategory::Array> {

  private:
    typedef typename
    bdeat_ArrayFunctions::ElementType<TYPE>::Type ElementType;

    typedef typename
    bslmf_RemoveCvq<ElementType>::Type                    ElementTypeNonCvq;
    typedef typename
    bslmf_RemoveReference<ElementTypeNonCvq>::Type        ElementTypeNonRefCvq;

    typedef bdem_SelectBdemType_Imp<ElementTypeNonRefCvq> ElementImp;
    typedef bdem_SelectBdemType_ToArrayElemType<ElementImp::VALUE>
                                                          ToArrayElemType;

  public:
    enum {
        VALUE = ToArrayElemType::VALUE
    };
};

        // ===========================================================
        // struct bdem_SelectBdemType_ByCategory<TYPE, CustomizedType>
        // ===========================================================

template <typename TYPE>
struct bdem_SelectBdemType_ByCategory<TYPE,
                                      bdeat_TypeCategory::CustomizedType> {
  private:
    typedef typename
    bdeat_CustomizedTypeFunctions::BaseType<TYPE>::Type BaseType;

    typedef typename
    bslmf_RemoveCvq<BaseType>::Type                    BaseTypeNonCvq;
    typedef typename
    bslmf_RemoveReference<BaseTypeNonCvq>::Type        BaseTypeNonRefCvq;

    typedef bdem_SelectBdemType_Imp<BaseTypeNonRefCvq> Imp;

  public:
    enum { VALUE = Imp::VALUE };
};

          // ========================================================
          // struct bdem_SelectBdemType_ByCategory<TYPE, Enumeration>
          // ========================================================

template <typename TYPE>
struct bdem_SelectBdemType_ByCategory<TYPE, bdeat_TypeCategory::Enumeration> {
    enum { VALUE = bdem_ElemType::BDEM_INT };
};

         // ==========================================================
         // struct bdem_SelectBdemType_ByCategory<TYPE, NullableValue>
         // ==========================================================

template <typename TYPE>
struct bdem_SelectBdemType_ByCategory<TYPE,
                                      bdeat_TypeCategory::NullableValue> {
  private:
    typedef typename
    bdeat_NullableValueFunctions::ValueType<TYPE>::Type ValueType;

    typedef typename
    bslmf_RemoveCvq<ValueType>::Type                    ValueTypeNonCvq;
    typedef typename
    bslmf_RemoveReference<ValueTypeNonCvq>::Type        ValueTypeNonRefCvq;

    typedef bdem_SelectBdemType_Imp<ValueTypeNonRefCvq> Imp;

  public:
    enum { VALUE = Imp::VALUE };
};

              // ================================================
              // struct bdem_SelectBdemType_GetCppType<ELEM_TYPE>
              // ================================================

template <>
struct bdem_SelectBdemType_GetCppType<bdem_ElemType::BDEM_CHAR> {
    typedef char Type;
};

template <>
struct bdem_SelectBdemType_GetCppType<bdem_ElemType::BDEM_SHORT> {
    typedef short Type;
};

template <>
struct bdem_SelectBdemType_GetCppType<bdem_ElemType::BDEM_INT> {
    typedef int Type;
};

template <>
struct bdem_SelectBdemType_GetCppType<bdem_ElemType::BDEM_INT64> {
    typedef bsls_Types::Int64 Type;
};

template <>
struct bdem_SelectBdemType_GetCppType<bdem_ElemType::BDEM_FLOAT> {
    typedef float Type;
};

template <>
struct bdem_SelectBdemType_GetCppType<bdem_ElemType::BDEM_DOUBLE> {
    typedef double Type;
};

template <>
struct bdem_SelectBdemType_GetCppType<bdem_ElemType::BDEM_STRING> {
    typedef bsl::string Type;
};

template <>
struct bdem_SelectBdemType_GetCppType<bdem_ElemType::BDEM_DATETIME> {
    typedef bdet_Datetime Type;
};

template <>
struct bdem_SelectBdemType_GetCppType<bdem_ElemType::BDEM_DATE> {
    typedef bdet_Date Type;
};

template <>
struct bdem_SelectBdemType_GetCppType<bdem_ElemType::BDEM_TIME> {
    typedef bdet_Time Type;
};

template <>
struct bdem_SelectBdemType_GetCppType<bdem_ElemType::BDEM_CHAR_ARRAY> {
    typedef bsl::vector<char> Type;
};

template <>
struct bdem_SelectBdemType_GetCppType<bdem_ElemType::BDEM_SHORT_ARRAY> {
    typedef bsl::vector<short> Type;
};

template <>
struct bdem_SelectBdemType_GetCppType<bdem_ElemType::BDEM_INT_ARRAY> {
    typedef bsl::vector<int> Type;
};

template <>
struct bdem_SelectBdemType_GetCppType<bdem_ElemType::BDEM_INT64_ARRAY> {
    typedef bsl::vector<bsls_Types::Int64> Type;
};

template <>
struct bdem_SelectBdemType_GetCppType<bdem_ElemType::BDEM_FLOAT_ARRAY> {
    typedef bsl::vector<float> Type;
};

template <>
struct bdem_SelectBdemType_GetCppType<bdem_ElemType::BDEM_DOUBLE_ARRAY> {
    typedef bsl::vector<double> Type;
};

template <>
struct bdem_SelectBdemType_GetCppType<bdem_ElemType::BDEM_STRING_ARRAY> {
    typedef bsl::vector<bsl::string> Type;
};

template <>
struct bdem_SelectBdemType_GetCppType<bdem_ElemType::BDEM_DATETIME_ARRAY> {
    typedef bsl::vector<bdet_Datetime> Type;
};

template <>
struct bdem_SelectBdemType_GetCppType<bdem_ElemType::BDEM_DATE_ARRAY> {
    typedef bsl::vector<bdet_Date> Type;
};

template <>
struct bdem_SelectBdemType_GetCppType<bdem_ElemType::BDEM_TIME_ARRAY> {
    typedef bsl::vector<bdet_Time> Type;
};

template <>
struct bdem_SelectBdemType_GetCppType<bdem_ElemType::BDEM_LIST> {
    typedef bdem_List Type;
};

template <>
struct bdem_SelectBdemType_GetCppType<bdem_ElemType::BDEM_TABLE> {
    typedef bdem_Table Type;
};

template <>
struct bdem_SelectBdemType_GetCppType<bdem_ElemType::BDEM_BOOL> {
    typedef bool Type;
};

template <>
struct bdem_SelectBdemType_GetCppType<bdem_ElemType::BDEM_DATETIMETZ> {
    typedef bdet_DatetimeTz Type;
};

template <>
struct bdem_SelectBdemType_GetCppType<bdem_ElemType::BDEM_DATETZ> {
    typedef bdet_DateTz Type;
};

template <>
struct bdem_SelectBdemType_GetCppType<bdem_ElemType::BDEM_TIMETZ> {
    typedef bdet_TimeTz Type;
};

template <>
struct bdem_SelectBdemType_GetCppType<bdem_ElemType::BDEM_BOOL_ARRAY> {
    typedef bsl::vector<bool> Type;
};

template <>
struct bdem_SelectBdemType_GetCppType<bdem_ElemType::BDEM_DATETIMETZ_ARRAY> {
    typedef bsl::vector<bdet_DatetimeTz> Type;
};

template <>
struct bdem_SelectBdemType_GetCppType<bdem_ElemType::BDEM_DATETZ_ARRAY> {
    typedef bsl::vector<bdet_DateTz> Type;
};

template <>
struct bdem_SelectBdemType_GetCppType<bdem_ElemType::BDEM_TIMETZ_ARRAY> {
    typedef bsl::vector<bdet_TimeTz> Type;
};

template <>
struct bdem_SelectBdemType_GetCppType<bdem_ElemType::BDEM_CHOICE> {
    typedef bdem_Choice Type;
};

template <>
struct bdem_SelectBdemType_GetCppType<bdem_ElemType::BDEM_CHOICE_ARRAY> {
    typedef bdem_ChoiceArray Type;
};

template <>
struct bdem_SelectBdemType_GetCppType<bdem_ElemType::BDEM_ROW> {
    typedef bdem_Row Type;
};

template <>
struct bdem_SelectBdemType_GetCppType<bdem_ElemType::BDEM_CHOICE_ARRAY_ITEM> {
    typedef bdem_ChoiceArrayItem Type;
};

            // ====================================
            // struct bdem_SelectBdemType_Imp<TYPE>
            // ====================================

template <typename TYPE>
struct bdem_SelectBdemType_Imp {

  private:
    typedef typename bdeat_TypeCategory::Select<TYPE>::Type TypeCategory;

    typedef bdem_SelectBdemType_ByCategory<TYPE, TypeCategory> ByCategory;

  public:
    enum { VALUE = ByCategory::VALUE };
};

template <>
struct bdem_SelectBdemType_Imp<bool>
{
    enum { VALUE = bdem_ElemType::BDEM_BOOL };
};

template <>
struct bdem_SelectBdemType_Imp<char>
{
    enum { VALUE = bdem_ElemType::BDEM_CHAR };
};

template <>
struct bdem_SelectBdemType_Imp<signed char>
{
    enum { VALUE = bdem_ElemType::BDEM_CHAR };
};

template <>
struct bdem_SelectBdemType_Imp<unsigned char>
{
    enum { VALUE = bdem_ElemType::BDEM_CHAR };
};

template <>
struct bdem_SelectBdemType_Imp<short>
{
    enum { VALUE = bdem_ElemType::BDEM_SHORT };
};

template <>
struct bdem_SelectBdemType_Imp<unsigned short>
{
    enum { VALUE = bdem_ElemType::BDEM_SHORT };
};

template <>
struct bdem_SelectBdemType_Imp<wchar_t>
{
    enum { VALUE = bdem_ElemType::BDEM_SHORT };
};

template <>
struct bdem_SelectBdemType_Imp<int>
{
    enum { VALUE = bdem_ElemType::BDEM_INT };
};

template <>
struct bdem_SelectBdemType_Imp<unsigned int>
{
    enum { VALUE = bdem_ElemType::BDEM_INT };
};

template <>
struct bdem_SelectBdemType_Imp<bsls_Types::Int64>
{
    enum { VALUE = bdem_ElemType::BDEM_INT64 };
};

template <>
struct bdem_SelectBdemType_Imp<bsls_Types::Uint64>
{
    enum { VALUE = bdem_ElemType::BDEM_INT64 };
};

template <>
struct bdem_SelectBdemType_Imp<float>
{
    enum { VALUE = bdem_ElemType::BDEM_FLOAT };
};

template <>
struct bdem_SelectBdemType_Imp<double>
{
    enum { VALUE = bdem_ElemType::BDEM_DOUBLE };
};

template <>
struct bdem_SelectBdemType_Imp<bsl::string>
{
    enum { VALUE = bdem_ElemType::BDEM_STRING };
};

template <>
struct bdem_SelectBdemType_Imp<bdet_Date>
{
    enum { VALUE = bdem_ElemType::BDEM_DATE };
};

template <>
struct bdem_SelectBdemType_Imp<bdet_Datetime>
{
    enum { VALUE = bdem_ElemType::BDEM_DATETIME };
};

template <>
struct bdem_SelectBdemType_Imp<bdet_Time>
{
    enum { VALUE = bdem_ElemType::BDEM_TIME };
};

template <>
struct bdem_SelectBdemType_Imp<bdet_DatetimeTz>
{
    enum { VALUE = bdem_ElemType::BDEM_DATETIMETZ };
};

template <>
struct bdem_SelectBdemType_Imp<bdet_DateTz>
{
    enum { VALUE = bdem_ElemType::BDEM_DATETZ };
};

template <>
struct bdem_SelectBdemType_Imp<bdet_TimeTz>
{
    enum { VALUE = bdem_ElemType::BDEM_TIMETZ };
};

template <>
struct bdem_SelectBdemType_Imp<bdem_Choice>
{
    enum { VALUE = bdem_ElemType::BDEM_CHOICE };
};

template <>
struct bdem_SelectBdemType_Imp<bdem_ChoiceArray>
{
    enum { VALUE = bdem_ElemType::BDEM_CHOICE_ARRAY };
};

template <>
struct bdem_SelectBdemType_Imp<bdem_Row>
{
    enum { VALUE = bdem_ElemType::BDEM_ROW };
};

template <>
struct bdem_SelectBdemType_Imp<bdem_ChoiceArrayItem>
{
    enum { VALUE = bdem_ElemType::BDEM_CHOICE_ARRAY_ITEM };
};

           // =====================================================
           // struct bdem_SelectBdemType_ToArrayElemType<ELEM_TYPE>
           // =====================================================

template <>
struct bdem_SelectBdemType_ToArrayElemType<bdem_ElemType::BDEM_CHAR> {
    enum { VALUE = bdem_ElemType::BDEM_CHAR_ARRAY };
};

template <>
struct bdem_SelectBdemType_ToArrayElemType<bdem_ElemType::BDEM_SHORT> {
    enum { VALUE = bdem_ElemType::BDEM_SHORT_ARRAY };
};

template <>
struct bdem_SelectBdemType_ToArrayElemType<bdem_ElemType::BDEM_INT> {
    enum { VALUE = bdem_ElemType::BDEM_INT_ARRAY };
};

template <>
struct bdem_SelectBdemType_ToArrayElemType<bdem_ElemType::BDEM_INT64> {
    enum { VALUE = bdem_ElemType::BDEM_INT64_ARRAY };
};

template <>
struct bdem_SelectBdemType_ToArrayElemType<bdem_ElemType::BDEM_FLOAT> {
    enum { VALUE = bdem_ElemType::BDEM_FLOAT_ARRAY };
};

template <>
struct bdem_SelectBdemType_ToArrayElemType<bdem_ElemType::BDEM_DOUBLE> {
    enum { VALUE = bdem_ElemType::BDEM_DOUBLE_ARRAY };
};

template <>
struct bdem_SelectBdemType_ToArrayElemType<bdem_ElemType::BDEM_STRING> {
    enum { VALUE = bdem_ElemType::BDEM_STRING_ARRAY };
};

template <>
struct bdem_SelectBdemType_ToArrayElemType<bdem_ElemType::BDEM_DATETIME> {
    enum { VALUE = bdem_ElemType::BDEM_DATETIME_ARRAY };
};

template <>
struct bdem_SelectBdemType_ToArrayElemType<bdem_ElemType::BDEM_DATE> {
    enum { VALUE = bdem_ElemType::BDEM_DATE_ARRAY };
};

template <>
struct bdem_SelectBdemType_ToArrayElemType<bdem_ElemType::BDEM_TIME> {
    enum { VALUE = bdem_ElemType::BDEM_TIME_ARRAY };
};

template <>
struct bdem_SelectBdemType_ToArrayElemType<bdem_ElemType::BDEM_LIST> {
    enum { VALUE = bdem_ElemType::BDEM_TABLE };
};

template <>
struct bdem_SelectBdemType_ToArrayElemType<bdem_ElemType::BDEM_BOOL> {
    enum { VALUE = bdem_ElemType::BDEM_BOOL_ARRAY };
};

template <>
struct bdem_SelectBdemType_ToArrayElemType<bdem_ElemType::BDEM_DATETIMETZ> {
    enum { VALUE = bdem_ElemType::BDEM_DATETIMETZ_ARRAY };
};

template <>
struct bdem_SelectBdemType_ToArrayElemType<bdem_ElemType::BDEM_DATETZ> {
    enum { VALUE = bdem_ElemType::BDEM_DATETZ_ARRAY };
};

template <>
struct bdem_SelectBdemType_ToArrayElemType<bdem_ElemType::BDEM_TIMETZ> {
    enum { VALUE = bdem_ElemType::BDEM_TIMETZ_ARRAY };
};

template <>
struct bdem_SelectBdemType_ToArrayElemType<bdem_ElemType::BDEM_CHOICE> {
    enum { VALUE = bdem_ElemType::BDEM_CHOICE_ARRAY };
};

template <>
struct bdem_SelectBdemType_ToArrayElemType<bdem_ElemType::BDEM_ROW> {
    enum { VALUE = bdem_ElemType::BDEM_TABLE };
};

template <>
struct
bdem_SelectBdemType_ToArrayElemType<bdem_ElemType::BDEM_CHOICE_ARRAY_ITEM> {
    enum { VALUE = bdem_ElemType::BDEM_CHOICE_ARRAY };
};

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
