// bdlmxxx_selectbdemtype.h                                           -*-C++-*-
#ifndef INCLUDED_BDLMXXX_SELECTBDEMTYPE
#define INCLUDED_BDLMXXX_SELECTBDEMTYPE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for selecting 'bdem' and C++ types.
//
//@CLASSES:
//   bdlmxxx::SelectBdemType: meta-function mapping 'TYPE' to 'bdem' and C++ types
//
//@SEE_ALSO: bdlmxxx_elemtype
//
//@AUTHOR: Clay Wilson (cwilson9), Shezan Baig (sbaig)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION: The 'bdlmxxx::SelectBdemType' meta-function supplied by this
// component maps a parameterized 'TYPE' to its corresponding 'bdlmxxx::ElemType'
// enumerator and its corresponding C++ type.  The resultant 'bdlmxxx::ElemType'
// enumerator and C++ types are made available via the
// 'bdlmxxx::SelectBdemType<TYPE>::VALUE' compile-time constant and the
// 'bdlmxxx::SelectBdemType<TYPE>::Type' 'typedef', respectively.
//
// This component recognizes 'bdeat' types.  In particular, types that fall
// into one of the following categories (as defined in 'bdlat_typecategory')
// will be mapped appropriately (as described in the subsequent sections):
//..
//    Category           Reference
//    --------           ---------
//    Array              bdlat_arrayfunctions
//    CustomizedType     bdlat_customizedtypefunctions
//    Enumeration        bdeat_enumeration
//    NullableValue      bdlat_nullablevaluefunctions
//..
// Types that do not fall in any of these categories are mapped as follows:
//..
//    Type                        bdlmxxx::ElemType     C++ Type
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
//    bsls::Types::Int64          BDEM_INT64        bsls::Types::Int64
//    bsls::Types::Uint64         BDEM_INT64        bsls::Types::Int64
//    float                       BDEM_FLOAT        float
//    double                      BDEM_DOUBLE       double
//    bsl::string                 BDEM_STRING       bsl::string
//    bdlt::Date                   BDEM_DATE         bdlt::Date
//    bdlt::Datetime               BDEM_DATETIME     bdlt::Datetime
//    bdlt::Time                   BDEM_TIME         bdlt::Time
//    'enum' types                BDEM_INT          int
//    other types                 BDEM_LIST         bdlmxxx::List
//    bdlt::DatetimeTz             BDEM_DATETIMETZ   bdlt::DatetimeTz
//    bdlt::DateTz                 BDEM_DATETZ       bdlt::DateTz
//    bdlt::TimeTz                 BDEM_TIMETZ       bdlt::TimeTz
//    bdlmxxx::Choice                 BDEM_CHOICE       bdlmxxx::Choice
//    bdlmxxx::Row                    BDEM_ROW          bdlmxxx::Row
//..
///Behavior for 'Array' Category
///-----------------------------
// When the parameterized 'TYPE' is an 'Array', the corresponding
// 'bdlmxxx::ElemType' of the array's element type is used to determine the array's
// 'bdlmxxx::ElemType' and C++ type.  The array's element type is obtained using
// 'bdeat_ArrayFunctions::ElementType<TYPE>::Type'.  The mapping is then
// performed based on the following table:
//..
//  -- array element --     ---------------------- array ----------------------
//  bdlmxxx::ElemType           bdlmxxx::ElemType          C++ Type
//  ----------------------  ---------------------  ----------------------------
//  BDEM_BOOL               BDEM_BOOL_ARRAY        bsl::vector<bool>
//  BDEM_CHAR               BDEM_CHAR_ARRAY        bsl::vector<char>
//  BDEM_SHORT              BDEM_SHORT_ARRAY       bsl::vector<short>
//  BDEM_INT                BDEM_INT_ARRAY         bsl::vector<int>
//  BDEM_INT64              BDEM_INT64_ARRAY       bsl::vector<
//                                                          bsls::Types::Int64>
//  BDEM_FLOAT              BDEM_FLOAT_ARRAY       bsl::vector<float>
//  BDEM_DOUBLE             BDEM_DOUBLE_ARRAY      bsl::vector<double>
//  BDEM_STRING             BDEM_STRING_ARRAY      bsl::vector<bsl::string>
//  BDEM_DATE               BDEM_DATE_ARRAY        bsl::vector<bdlt::Date>
//  BDEM_DATETIME           BDEM_DATETIME_ARRAY    bsl::vector<bdlt::Datetime>
//  BDEM_TIME               BDEM_TIME_ARRAY        bsl::vector<bdlt::Time>
//  BDEM_LIST               BDEM_TABLE             bdlmxxx::Table
//  BDEM_DATETIMETZ         BDEM_DATETIMETZ_ARRAY  bsl::vector<bdlt::DatetimeTz>
//  BDEM_DATETZ             BDEM_DATETZ_ARRAY      bsl::vector<bdlt::DateTz>
//  BDEM_TIMETZ             BDEM_TIMETZ_ARRAY      bsl::vector<bdlt::TimeTz>
//  BDEM_CHOICE             BDEM_CHOICE_ARRAY      bdlmxxx::ChoiceArray
//  BDEM_ROW                BDEM_TABLE             bdlmxxx::Row
//  BDEM_CHOICE_ARRAY_ITEM  BDEM_CHOICE_ARRAY      bdlmxxx::ChoiceArray
//..
// Note that 'const' and 'volatile' qualifiers are removed from the array's
// element type before evaluating its 'bdlmxxx::ElemType'.
//
///Behavior for 'CustomizedType' Category
///--------------------------------------
// The 'bdlmxxx::ElemType' and C++ type of the base type are used for the
// customized type's 'bdlmxxx::ElemType' and C++ type.  The base type is obtained
// using 'bdeat_CustomizedTypeFunctions::BaseType<TYPE>::Type'.
//
// Note that 'const' and 'volatile' qualifiers are removed from the base type
// before evaluating the result.
//
///Behavior for 'Enumeration' Category
///-----------------------------------
// The 'bdlmxxx::ElemType' is 'INT' and the C++ type is 'int'.
//
///Behavior for 'NullableValue' Category
///-------------------------------------
// The 'bdlmxxx::ElemType' and C++ type of the value type are used for the nullable
// value's 'bdlmxxx::ElemType' and C++ type.  The value type is obtained using
// 'bdeat_NullableValueFunctions::ValueType<TYPE>::Type'.
//
// Note that 'const' and 'volatile' qualifiers are removed from the value type
// before evaluating the result.
//
///Usage
///-----
// The following snippets of code illustrate basic usage of this component.
// Note that the 'bslmf::IsSame' meta-function is employed only for the
// purposes of illustration:
//..
//  class Class {};
//  enum Enum {};
//
//  // 'bool' -> 'bool'
//  assert(bdlmxxx::ElemType::BDEM_BOOL == bdlmxxx::SelectBdemType<bool>::VALUE);
//  typedef bdlmxxx::SelectBdemType<bool>::Type boolProxyType;
//  assert(1 == (bslmf::IsSame<bool, boolProxyType>::VALUE));
//
//  // 'unsigned' -> 'int'
//  assert(bdlmxxx::ElemType::BDEM_INT == bdlmxxx::SelectBdemType<unsigned>::VALUE);
//  typedef bdlmxxx::SelectBdemType<unsigned>::Type unsignedProxyType;
//  assert(1 == (bslmf::IsSame<int, unsignedProxyType>::VALUE));
//
//  // 'Enum' -> 'int'
//  assert(bdlmxxx::ElemType::BDEM_INT == bdlmxxx::SelectBdemType<Enum>::VALUE);
//  typedef bdlmxxx::SelectBdemType<Enum>::Type enumProxyType;
//  assert(1 == (bslmf::IsSame<int, enumProxyType>::VALUE));
//
//  // 'bsl::vector<Enum>' -> 'bsl::vector<int>'
//  assert(bdlmxxx::ElemType::BDEM_INT_ARRAY ==
//                             bdlmxxx::SelectBdemType<bsl::vector<Enum> >::VALUE);
//  typedef bdlmxxx::SelectBdemType<bsl::vector<Enum> >::Type enumVectorProxyType;
//  assert(1 == (bslmf::IsSame<bsl::vector<int>, enumVectorProxyType>::VALUE));
//
//  // 'Class' -> 'bdlmxxx::List'
//  assert(bdlmxxx::ElemType::BDEM_LIST == bdlmxxx::SelectBdemType<Class>::VALUE);
//  typedef bdlmxxx::SelectBdemType<Class>::Type classProxyType;
//  assert(1 == (bslmf::IsSame<bdlmxxx::List, classProxyType>::VALUE));
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLAT_ARRAYFUNCTIONS
#include <bdlat_arrayfunctions.h>
#endif

#ifndef INCLUDED_BDLAT_CUSTOMIZEDTYPEFUNCTIONS
#include <bdlat_customizedtypefunctions.h>
#endif

#ifndef INCLUDED_BDLAT_NULLABLEVALUEFUNCTIONS
#include <bdlat_nullablevaluefunctions.h>
#endif

#ifndef INCLUDED_BDLAT_TYPECATEGORY
#include <bdlat_typecategory.h>
#endif

#ifndef INCLUDED_BDLMXXX_ELEMTYPE
#include <bdlmxxx_elemtype.h>
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


namespace bdlmxxx {class List;
class Table;
}  // close package namespace


// Updated by 'bde-replace-bdet-forward-declares.py -m bdlt': 2015-02-03
// Updated declarations tagged with '// bdet -> bdlt'.

namespace bdlt { class Date; }                                  // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::Date Date;                    // bdet -> bdlt
}  // close namespace bdet

namespace bdlt { class Datetime; }                              // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::Datetime Datetime;            // bdet -> bdlt
}  // close namespace bdet

namespace bdlt { class Time; }                                  // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::Time Time;                    // bdet -> bdlt
}  // close namespace bdet

namespace bdlt { class DatetimeTz; }                            // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::DatetimeTz DatetimeTz;        // bdet -> bdlt
}  // close namespace bdet

namespace bdlt { class DateTz; }                                // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::DateTz DateTz;                // bdet -> bdlt
}  // close namespace bdet

namespace bdlt { class TimeTz; }                                // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::TimeTz TimeTz;                // bdet -> bdlt
}  // close namespace bdet

namespace bdlmxxx {class Choice;
class ChoiceArray;
class Row;
class ChoiceArrayItem;

template <class TYPE, class TYPE_CATEGORY>
struct SelectBdemType_ByCategory;

template <int ELEM_TYPE>
struct SelectBdemType_GetCppType;

template <class TYPE>
struct SelectBdemType_Imp;

template <int ELEM_TYPE>
struct SelectBdemType_ToArrayElemType;

                         // ==========================
                         // struct SelectBdemType
                         // ==========================

template <class TYPE>
struct SelectBdemType {
    // This meta-function contains a 'VALUE' compile-time constant that
    // indicates the corresponding 'ElemType::Type' value for the
    // parameterized 'TYPE'.  It also contains the 'Type' typedef that
    // indicates the corresponding C++ type used for the parameterized 'TYPE'.

  private:
    typedef SelectBdemType_Imp<TYPE> Imp;

  public:
    enum { VALUE = Imp::VALUE };  // value indicating the corresponding
                                  // 'ElemType::Type' for the
                                  // parameterized 'TYPE'

    typedef typename
    SelectBdemType_GetCppType<VALUE>::Type Type;
                                       // type indicating the corresponding C++
                                       // type for the parameterized 'TYPE'
};

// ---  Anything below this line is implementation specific.  Do not use.  ----

         // ==========================================================
         // struct SelectBdemType_ByCategory<TYPE, TYPE_CATEGORY>
         // ==========================================================

template <class TYPE, class TYPE_CATEGORY>
struct SelectBdemType_ByCategory {

  private:
    enum { IS_ENUM = bslmf::IsEnum<TYPE>::VALUE };

  public:
    enum {
        VALUE = IS_ENUM ? ElemType::BDEM_INT : ElemType::BDEM_LIST
    };
};

             // ==================================================
             // struct SelectBdemType_ByCategory<TYPE, Array>
             // ==================================================

template <class TYPE>
struct SelectBdemType_ByCategory<TYPE, bdeat_TypeCategory::Array> {

  private:
    typedef typename
    bdeat_ArrayFunctions::ElementType<TYPE>::Type ElementType;

    typedef typename
    bslmf::RemoveCvq<ElementType>::Type                   ElementTypeNonCvq;
    typedef typename
    bslmf::RemoveReference<ElementTypeNonCvq>::Type       ElementTypeNonRefCvq;

    typedef SelectBdemType_Imp<ElementTypeNonRefCvq> ElementImp;
    typedef SelectBdemType_ToArrayElemType<ElementImp::VALUE>
                                                          ToArrayElemType;

  public:
    enum {
        VALUE = ToArrayElemType::VALUE
    };
};

        // ===========================================================
        // struct SelectBdemType_ByCategory<TYPE, CustomizedType>
        // ===========================================================

template <class TYPE>
struct SelectBdemType_ByCategory<TYPE,
                                      bdeat_TypeCategory::CustomizedType> {
  private:
    typedef typename
    bdeat_CustomizedTypeFunctions::BaseType<TYPE>::Type BaseType;

    typedef typename
    bslmf::RemoveCvq<BaseType>::Type                    BaseTypeNonCvq;
    typedef typename
    bslmf::RemoveReference<BaseTypeNonCvq>::Type        BaseTypeNonRefCvq;

    typedef SelectBdemType_Imp<BaseTypeNonRefCvq>  Imp;

  public:
    enum { VALUE = Imp::VALUE };
};

          // ========================================================
          // struct SelectBdemType_ByCategory<TYPE, Enumeration>
          // ========================================================

template <class TYPE>
struct SelectBdemType_ByCategory<TYPE, bdeat_TypeCategory::Enumeration> {
    enum { VALUE = ElemType::BDEM_INT };
};

         // ==========================================================
         // struct SelectBdemType_ByCategory<TYPE, NullableValue>
         // ==========================================================

template <class TYPE>
struct SelectBdemType_ByCategory<TYPE,
                                      bdeat_TypeCategory::NullableValue> {
  private:
    typedef typename
    bdeat_NullableValueFunctions::ValueType<TYPE>::Type ValueType;

    typedef typename
    bslmf::RemoveCvq<ValueType>::Type                    ValueTypeNonCvq;
    typedef typename
    bslmf::RemoveReference<ValueTypeNonCvq>::Type        ValueTypeNonRefCvq;

    typedef SelectBdemType_Imp<ValueTypeNonRefCvq>  Imp;

  public:
    enum { VALUE = Imp::VALUE };
};

              // ================================================
              // struct SelectBdemType_GetCppType<ELEM_TYPE>
              // ================================================

template <>
struct SelectBdemType_GetCppType<ElemType::BDEM_CHAR> {
    typedef char Type;
};

template <>
struct SelectBdemType_GetCppType<ElemType::BDEM_SHORT> {
    typedef short Type;
};

template <>
struct SelectBdemType_GetCppType<ElemType::BDEM_INT> {
    typedef int Type;
};

template <>
struct SelectBdemType_GetCppType<ElemType::BDEM_INT64> {
    typedef bsls::Types::Int64 Type;
};

template <>
struct SelectBdemType_GetCppType<ElemType::BDEM_FLOAT> {
    typedef float Type;
};

template <>
struct SelectBdemType_GetCppType<ElemType::BDEM_DOUBLE> {
    typedef double Type;
};

template <>
struct SelectBdemType_GetCppType<ElemType::BDEM_STRING> {
    typedef bsl::string Type;
};

template <>
struct SelectBdemType_GetCppType<ElemType::BDEM_DATETIME> {
    typedef bdlt::Datetime Type;
};

template <>
struct SelectBdemType_GetCppType<ElemType::BDEM_DATE> {
    typedef bdlt::Date Type;
};

template <>
struct SelectBdemType_GetCppType<ElemType::BDEM_TIME> {
    typedef bdlt::Time Type;
};

template <>
struct SelectBdemType_GetCppType<ElemType::BDEM_CHAR_ARRAY> {
    typedef bsl::vector<char> Type;
};

template <>
struct SelectBdemType_GetCppType<ElemType::BDEM_SHORT_ARRAY> {
    typedef bsl::vector<short> Type;
};

template <>
struct SelectBdemType_GetCppType<ElemType::BDEM_INT_ARRAY> {
    typedef bsl::vector<int> Type;
};

template <>
struct SelectBdemType_GetCppType<ElemType::BDEM_INT64_ARRAY> {
    typedef bsl::vector<bsls::Types::Int64> Type;
};

template <>
struct SelectBdemType_GetCppType<ElemType::BDEM_FLOAT_ARRAY> {
    typedef bsl::vector<float> Type;
};

template <>
struct SelectBdemType_GetCppType<ElemType::BDEM_DOUBLE_ARRAY> {
    typedef bsl::vector<double> Type;
};

template <>
struct SelectBdemType_GetCppType<ElemType::BDEM_STRING_ARRAY> {
    typedef bsl::vector<bsl::string> Type;
};

template <>
struct SelectBdemType_GetCppType<ElemType::BDEM_DATETIME_ARRAY> {
    typedef bsl::vector<bdlt::Datetime> Type;
};

template <>
struct SelectBdemType_GetCppType<ElemType::BDEM_DATE_ARRAY> {
    typedef bsl::vector<bdlt::Date> Type;
};

template <>
struct SelectBdemType_GetCppType<ElemType::BDEM_TIME_ARRAY> {
    typedef bsl::vector<bdlt::Time> Type;
};

template <>
struct SelectBdemType_GetCppType<ElemType::BDEM_LIST> {
    typedef List Type;
};

template <>
struct SelectBdemType_GetCppType<ElemType::BDEM_TABLE> {
    typedef Table Type;
};

template <>
struct SelectBdemType_GetCppType<ElemType::BDEM_BOOL> {
    typedef bool Type;
};

template <>
struct SelectBdemType_GetCppType<ElemType::BDEM_DATETIMETZ> {
    typedef bdlt::DatetimeTz Type;
};

template <>
struct SelectBdemType_GetCppType<ElemType::BDEM_DATETZ> {
    typedef bdlt::DateTz Type;
};

template <>
struct SelectBdemType_GetCppType<ElemType::BDEM_TIMETZ> {
    typedef bdlt::TimeTz Type;
};

template <>
struct SelectBdemType_GetCppType<ElemType::BDEM_BOOL_ARRAY> {
    typedef bsl::vector<bool> Type;
};

template <>
struct SelectBdemType_GetCppType<ElemType::BDEM_DATETIMETZ_ARRAY> {
    typedef bsl::vector<bdlt::DatetimeTz> Type;
};

template <>
struct SelectBdemType_GetCppType<ElemType::BDEM_DATETZ_ARRAY> {
    typedef bsl::vector<bdlt::DateTz> Type;
};

template <>
struct SelectBdemType_GetCppType<ElemType::BDEM_TIMETZ_ARRAY> {
    typedef bsl::vector<bdlt::TimeTz> Type;
};

template <>
struct SelectBdemType_GetCppType<ElemType::BDEM_CHOICE> {
    typedef Choice Type;
};

template <>
struct SelectBdemType_GetCppType<ElemType::BDEM_CHOICE_ARRAY> {
    typedef ChoiceArray Type;
};

template <>
struct SelectBdemType_GetCppType<ElemType::BDEM_ROW> {
    typedef Row Type;
};

template <>
struct SelectBdemType_GetCppType<ElemType::BDEM_CHOICE_ARRAY_ITEM> {
    typedef ChoiceArrayItem Type;
};

            // ====================================
            // struct SelectBdemType_Imp<TYPE>
            // ====================================

template <class TYPE>
struct SelectBdemType_Imp {

  private:
    typedef typename bdeat_TypeCategory::Select<TYPE>::Type TypeCategory;

    typedef SelectBdemType_ByCategory<TYPE, TypeCategory> ByCategory;

  public:
    enum { VALUE = ByCategory::VALUE };
};

template <>
struct SelectBdemType_Imp<bool>
{
    enum { VALUE = ElemType::BDEM_BOOL };
};

template <>
struct SelectBdemType_Imp<char>
{
    enum { VALUE = ElemType::BDEM_CHAR };
};

template <>
struct SelectBdemType_Imp<signed char>
{
    enum { VALUE = ElemType::BDEM_CHAR };
};

template <>
struct SelectBdemType_Imp<unsigned char>
{
    enum { VALUE = ElemType::BDEM_CHAR };
};

template <>
struct SelectBdemType_Imp<short>
{
    enum { VALUE = ElemType::BDEM_SHORT };
};

template <>
struct SelectBdemType_Imp<unsigned short>
{
    enum { VALUE = ElemType::BDEM_SHORT };
};

template <>
struct SelectBdemType_Imp<wchar_t>
{
    enum { VALUE = ElemType::BDEM_SHORT };
};

template <>
struct SelectBdemType_Imp<int>
{
    enum { VALUE = ElemType::BDEM_INT };
};

template <>
struct SelectBdemType_Imp<unsigned int>
{
    enum { VALUE = ElemType::BDEM_INT };
};

template <>
struct SelectBdemType_Imp<bsls::Types::Int64>
{
    enum { VALUE = ElemType::BDEM_INT64 };
};

template <>
struct SelectBdemType_Imp<bsls::Types::Uint64>
{
    enum { VALUE = ElemType::BDEM_INT64 };
};

template <>
struct SelectBdemType_Imp<float>
{
    enum { VALUE = ElemType::BDEM_FLOAT };
};

template <>
struct SelectBdemType_Imp<double>
{
    enum { VALUE = ElemType::BDEM_DOUBLE };
};

template <>
struct SelectBdemType_Imp<bsl::string>
{
    enum { VALUE = ElemType::BDEM_STRING };
};

template <>
struct SelectBdemType_Imp<bdlt::Date>
{
    enum { VALUE = ElemType::BDEM_DATE };
};

template <>
struct SelectBdemType_Imp<bdlt::Datetime>
{
    enum { VALUE = ElemType::BDEM_DATETIME };
};

template <>
struct SelectBdemType_Imp<bdlt::Time>
{
    enum { VALUE = ElemType::BDEM_TIME };
};

template <>
struct SelectBdemType_Imp<bdlt::DatetimeTz>
{
    enum { VALUE = ElemType::BDEM_DATETIMETZ };
};

template <>
struct SelectBdemType_Imp<bdlt::DateTz>
{
    enum { VALUE = ElemType::BDEM_DATETZ };
};

template <>
struct SelectBdemType_Imp<bdlt::TimeTz>
{
    enum { VALUE = ElemType::BDEM_TIMETZ };
};

template <>
struct SelectBdemType_Imp<Choice>
{
    enum { VALUE = ElemType::BDEM_CHOICE };
};

template <>
struct SelectBdemType_Imp<ChoiceArray>
{
    enum { VALUE = ElemType::BDEM_CHOICE_ARRAY };
};

template <>
struct SelectBdemType_Imp<Row>
{
    enum { VALUE = ElemType::BDEM_ROW };
};

template <>
struct SelectBdemType_Imp<ChoiceArrayItem>
{
    enum { VALUE = ElemType::BDEM_CHOICE_ARRAY_ITEM };
};

           // =====================================================
           // struct SelectBdemType_ToArrayElemType<ELEM_TYPE>
           // =====================================================

template <>
struct SelectBdemType_ToArrayElemType<ElemType::BDEM_CHAR> {
    enum { VALUE = ElemType::BDEM_CHAR_ARRAY };
};

template <>
struct SelectBdemType_ToArrayElemType<ElemType::BDEM_SHORT> {
    enum { VALUE = ElemType::BDEM_SHORT_ARRAY };
};

template <>
struct SelectBdemType_ToArrayElemType<ElemType::BDEM_INT> {
    enum { VALUE = ElemType::BDEM_INT_ARRAY };
};

template <>
struct SelectBdemType_ToArrayElemType<ElemType::BDEM_INT64> {
    enum { VALUE = ElemType::BDEM_INT64_ARRAY };
};

template <>
struct SelectBdemType_ToArrayElemType<ElemType::BDEM_FLOAT> {
    enum { VALUE = ElemType::BDEM_FLOAT_ARRAY };
};

template <>
struct SelectBdemType_ToArrayElemType<ElemType::BDEM_DOUBLE> {
    enum { VALUE = ElemType::BDEM_DOUBLE_ARRAY };
};

template <>
struct SelectBdemType_ToArrayElemType<ElemType::BDEM_STRING> {
    enum { VALUE = ElemType::BDEM_STRING_ARRAY };
};

template <>
struct SelectBdemType_ToArrayElemType<ElemType::BDEM_DATETIME> {
    enum { VALUE = ElemType::BDEM_DATETIME_ARRAY };
};

template <>
struct SelectBdemType_ToArrayElemType<ElemType::BDEM_DATE> {
    enum { VALUE = ElemType::BDEM_DATE_ARRAY };
};

template <>
struct SelectBdemType_ToArrayElemType<ElemType::BDEM_TIME> {
    enum { VALUE = ElemType::BDEM_TIME_ARRAY };
};

template <>
struct SelectBdemType_ToArrayElemType<ElemType::BDEM_LIST> {
    enum { VALUE = ElemType::BDEM_TABLE };
};

template <>
struct SelectBdemType_ToArrayElemType<ElemType::BDEM_BOOL> {
    enum { VALUE = ElemType::BDEM_BOOL_ARRAY };
};

template <>
struct SelectBdemType_ToArrayElemType<ElemType::BDEM_DATETIMETZ> {
    enum { VALUE = ElemType::BDEM_DATETIMETZ_ARRAY };
};

template <>
struct SelectBdemType_ToArrayElemType<ElemType::BDEM_DATETZ> {
    enum { VALUE = ElemType::BDEM_DATETZ_ARRAY };
};

template <>
struct SelectBdemType_ToArrayElemType<ElemType::BDEM_TIMETZ> {
    enum { VALUE = ElemType::BDEM_TIMETZ_ARRAY };
};

template <>
struct SelectBdemType_ToArrayElemType<ElemType::BDEM_CHOICE> {
    enum { VALUE = ElemType::BDEM_CHOICE_ARRAY };
};

template <>
struct SelectBdemType_ToArrayElemType<ElemType::BDEM_ROW> {
    enum { VALUE = ElemType::BDEM_TABLE };
};

template <>
struct
SelectBdemType_ToArrayElemType<ElemType::BDEM_CHOICE_ARRAY_ITEM> {
    enum { VALUE = ElemType::BDEM_CHOICE_ARRAY };
};
}  // close package namespace

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
