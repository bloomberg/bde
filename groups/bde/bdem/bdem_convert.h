// bdem_convert.h                                                     -*-C++-*-
#ifndef INCLUDED_BDEM_CONVERT
#define INCLUDED_BDEM_CONVERT

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide functions that convert values to and from 'bdem' types.
//
//@CLASSES:
//  bdem_Convert: namespace for 'bdem'-based conversion functions
//
//@SEE_ALSO: bdem_elemtype
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: The functions in this component take a data value and convert
// that value to or from a 'bdem' type, where the actual conversion is selected
// at run-time based on the value of a 'bdem_ElemType::Type' argument.  Since
// the type conversion is selected at run-time, special care is taken to ensure
// that potentially illegal conversions do not cause a compile-time error.
//
///Extended Type Conversions
///-------------------------
// This component implements "extended type conversions" as follows:
//..
// 1. All normal C++ conversions apply except that converting from 'char *'
//    to 'bool' is performed by treating the pointer as a string and parsing
//    it, rather than by checking the pointer against null.  (See 3 below.)
//
// 2. Converting a date, time, or datetime value (with or without a timezone
//    offset) to an 'bsl::string' yields a text string in ISO 8601 format (the
//    format used by XML).  Any other type that can be printed using
//    'bdeu_PrintMethods::print' is convertible to 'bsl::string' and yields
//    the same result as printing that value to an 'bsl::ostream' using
//    'bdeu_PrintMethods::print'.  Vectors are similarly formatted using
//    'bdeu_PrintMethods::print'.  Other conversions to string will fail.
//
// 3. A string ('char *' or 'bsl::string') is convertible to any 'bdem'
//    *scalar* type.  Converting from a string is accomplished by parsing the
//    contents of the string.  The string is not considered convertible to a
//    specified type unless the type is one of the 'bdem' scalar types and the
//    string contains a canonical textual representation of a value of the
//    specified type.  Converting a value to string then back to the original
//    type will yield the original value, except for round-off errors in
//    floating-point values.  A value of '(char*) 0' is treated as an empty
//    string.
//
// 4. A 'bdem_List' is convertible to a 'bdem_Row' and vice versa.  A
//    'bdem_Choice' is convertible to a 'bdem_ChoiceArrayItem' and vice-versa.
//    A 'bdem_Row' is convertible to another 'bdem_Row', even though
//    'bdem_Row' lacks a public assignment operator.  A 'bdem_ChoiceArrayItem'
//    is convertible to another 'bdem_ChoiceArrayItem' even though
//    'bdem_ChoiceArrayItem' lacks a public assignment operator.  When the
//    target type is 'bdem_Row' or 'bdem_ChoiceArrayItem', the respective type
//    lists of the source and target must match for the conversion to be
//    successful.
//
// 5. If the source and target types both support unset values (as defined by
//    'bdetu_Unset<T>::isUnset') and if the value being converted is unset,
//    then the target object is set to its corresponding unset value.  Thus,
//    for example, an empty string will be successfully converted to an unset
//    value of any 'bdem' scalar type even though, in the absence of this rule,
//    it would fail to parse.  Exceptions: 'bool' and 'char' are never
//    considered to have an unset value, even if 'bdetu_Unset<bool>::isUnset'
//    returns 'true'.
//
// 6. Conversions involving a 'bdem_ElemRef' or 'bdem_ConstElemRef' are
//    performed by applying the previous rules to the referenced values.
//..
// A failed conversion will not result in a compile-time error but will simply
// return a non-zero status from the conversion functions.  The caller may
// choose to abort, throw an exception, ignore the error, or whatever.  The
// conversion functions modify existing objects, rather than constructing new
// objects.  This makes the conversion operations exception-safe, as it does
// not leave any objects whose construction status is indeterminate.
//
///Thread Safety
///-------------
// All of the functions in this component may safely be called from more than
// one thread simultaneously provided that the 'dstAddr' is distinct in each
// thread.
//
///Usage
///-----
// Assume we want to write a template function that takes a value and inserts
// it into a 'bdem_ElemRef', without knowing in advance what type the
// 'bdem_ElemRef' refers to and we want to honor conversions such as 'short'
// to 'int'.  The direct approach might start with a 'switch' statement:
//..
//  template <typename VALTYPE>
//  void setValue(const bdem_ElemRef& elem, const VALTYPE& value) {
//      switch (elem.elemType()) {
//        case bdem_ElemType::BDEM_SHORT:
//          elem.theModifiableShort()  = value; break;
//        case bdem_ElemType::BDEM_INT:
//          elem.theModifiableInt()    = value; break;
//        case bdem_ElemType::BDEM_FLOAT:
//          elem.theModifiableFloat()  = value; break;
//        case bdem_ElemType::BDEM_STRING:
//          elem.theModifiableString() = value; break;
//        // etc.
//      }
//  }
//..
// The above code is not only tedious to write, but will not compile under most
// circumstances.  For example:
//..
//  setValue(elem, "hello");
//..
// will fail to compile with a type mismatch in the first three cases because
// 'const char *' is not convertible to 'short', 'int', or 'float'.  Similarly,
//..
//  setValue(elem, 5.5);
//..
// Will fail to compile with a type mismatch on 'case
// bdem_ElemType::BDEM_STRING' because 'float' is not is not convertible to
// 'bsl::string'.  Note that we do not even know what type 'elem' might hold
// -- even the branches that will not be taken at run-time must be valid at
// compile-time.
//
// The functions in this component solve this problem through sophisticated
// meta-programming.  Using these functions, we can re-write 'setValue':
//..
//  template <typename VALTYPE>
//  void setValue(const bdem_ElemRef& elem, const VALTYPE& value) {
//      bdem_ElemType::Type elemType = elem.elemType();
//      void *elemData = elem.data();
//      if (bdem_Convert::toBdemType(elemData, elemType, value)) {
//          throw bsl::string("Invalid conversion");
//      }
//  }
//..
// We can test this function with a simple 'main' program:
//..
//  int main()
//  {
//      int i = -1;
//      float f = -2.2;
//      bsl::string s = "junk";
//
//      bdem_ElemRef iref(&i, &bdem_Properties::d_intAttr);
//      bdem_ElemRef fref(&f, &bdem_Properties::d_floatAttr);
//      bdem_ElemRef sref(&s, &bdem_Properties::d_stringAttr);
//
//      setValue(iref, 9.8);
//      assert(9 == i);
//
//      setValue(fref, 6);
//      assert(6.0 == f);
//
//      setValue(sref, "Hello");
//      assert("Hello" == s);
//..
// This implementation can also take advantage of the automatic conversions to
// and from string types:
//..
//      setValue(sref, 9.8);
//      assert("9.8" == s);
//
//      setValue(iref, "123");
//      assert(123   == i);
//..
// To complete the picture, we should handle errors.  The following code shows
// that a failed conversion does not modify the target value:
//..
//      try {
//          setValue(sref, 4.6);    // successful
//          setValue(iref, "xyz");  // "xyz" cannot be converted to 'int'
//          assert("Shouldn't get here" == 0);
//      }
//      catch (const bsl::string& error) {
//          assert("Invalid conversion" == error);
//          assert("4.6" == s);     // test that 'sref' was changed
//          assert(123   == i);     // test that 'iref' was *not* changed
//      }
//      return 0;
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEM_ELEMREF
#include <bdem_elemref.h>
#endif

#ifndef INCLUDED_BDEM_ELEMTYPE
#include <bdem_elemtype.h>
#endif

#ifndef INCLUDED_BDEU_PRINTMETHODS
#include <bdeu_printmethods.h>
#endif

#ifndef INCLUDED_BDETU_UNSET
#include <bdetu_unset.h>
#endif

#ifndef INCLUDED_BSLMF_MATCHANYTYPE
#include <bslmf_matchanytype.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#include <bslmf_isconvertible.h>
#endif

#ifndef INCLUDED_BSLMF_ISSAME
#include <bslmf_issame.h>
#endif

#ifdef BSLS_PLATFORM__CMP_SUN
#ifndef INCLUDED_BSLMF_REMOVECVQ
#include <bslmf_removecvq.h>
#endif
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

#ifndef INCLUDED_BSL_SSTREAM
#include <bsl_sstream.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>
#endif

namespace BloombergLP {

class bdem_Choice;
class bdem_ChoiceArray;
class bdem_ChoiceArrayItem;
class bdem_Row;
class bdem_List;
class bdem_Table;

class bdet_Datetime;
class bdet_DatetimeTz;
class bdet_Date;
class bdet_DateTz;
class bdet_Time;
class bdet_TimeTz;

                     // =======================================
                     // local struct bdem_Convert_IsOstreamable
                     // =======================================

struct bdem_Convert_IsOstreamableImp {
    // This *local* 'struct' provides implementation details for the *local*
    // 'bdem_Convert_IsOstreamable<TESTTYPE>'.  Do *not* use.

    struct NonStream {
        // A 'NonStream' is *not* an 'ostream'.
    };

    struct StreamProbe : bsl::ostream {
        // A 'StreamProbe' IS-A 'ostream'.  It is also convertible to a
        // 'NonStream'.  Given the choice of converting to an 'ostream' or
        // converting to a 'NonStream', overload resolution will prefer the
        // base-class conversion to 'ostream' over the user-defined conversion
        // to 'NonStream'.

        StreamProbe();

        operator NonStream&(); // Compile-time only.  No implementation needed.
    };

    static StreamProbe s_probe;  // Compile-time only.  No definition needed.
};

bdem_Convert_IsOstreamableImp::NonStream&
operator<<(bdem_Convert_IsOstreamableImp::NonStream&, const bslmf_AnyType&);
    // Any type is streamable to a 'NonStream'.  This operator will be
    // selected when no better match is found during overload resolution for
    // 'operator<<'.  Compile-time only.  No implementation needed.

                     // =======================================
                     // local struct bdem_Convert_IsOstreamable
                     // =======================================

template <typename TESTTYPE>
struct bdem_Convert_IsOstreamable {
    // Meta-function to determine if a value, 'x', of parameterized 'TESTTYPE'
    // can be printed using the expression 'bsl::ostream << x'.  The
    // constant 'VALUE' (below) will be non-zero if 'TESTTYPE' can be printed
    // in this way and 0 otherwise.

  private:
    typedef bdem_Convert_IsOstreamableImp Imp;

    static TESTTYPE *s_ptr;  // Compile-time only.  No definition needed.

  public:
    enum {
        VALUE = (sizeof(Imp::s_probe << *s_ptr) == sizeof(bsl::ostream))
    };
        // "Invoke" 'operator<<' and test the size of its return value.  Since
        // it is invoked within a constant expression (and, in particular,
        // as the argument to 'sizeof'), no runtime call results.  If:
        //..
        //  bsl::ostream& operator<<(bsl::ostream&, const TESTTYPE&);
        //..
        // exists, then it will be preferred over:
        //..
        //  NonStream& operator<<(NonStream&, const bslmf_AnyType&);
        //..
        // because the latter requires two user-defined conversions whereas the
        // former requires *no* user-defined conversions.  If the size of the
        // returned value is 'sizeof(bsl::ostream)', then a 'TESTTYPE' value is
        // printable to 'bsl::ostream'.

    typedef bslmf_MetaInt<VALUE> Type;
};

                            // ===================
                            // struct bdem_Convert
                            // ===================

struct bdem_Convert {
    // This 'struct' provides a namespace for 'static' utility functions that
    // convert to and from 'bdem' types using "extended conversion" semantics.
    // See the component-level documentation for a description of "extended
    // conversion".

  private:
    // PRIVATE TYPES
    typedef bsls_Types::Int64 Int64;

    typedef bslmf_MetaInt<0> FalseType;
    typedef bslmf_MetaInt<1> TrueType;

    // Predicates, in priority order.

    template <typename SRCTYPE, typename DSTTYPE>
    struct IsToElemRef : FalseType {
    };

    template <typename SRCTYPE, typename DSTTYPE>
    struct IsFloatingToIntegral : FalseType {
    };

    template <typename SRCTYPE, typename DSTTYPE>
    struct IsBdetNonTzToBdetTz : FalseType {
    };

    template <typename SRCTYPE, typename DSTTYPE>
    struct IsFromString : FalseType {
    };

    template <typename SRCTYPE, typename DSTTYPE>
    struct IsFromElemRef : FalseType {
    };

    template <typename SRCTYPE, typename DSTTYPE>
    struct IsPrintableToString : FalseType {
    };

    enum ConversionType {
        BDEM_NO_CONVERSION,
        BDEM_TO_ELEMREF,
        BDEM_FLOAT_TO_INT,
        BDEM_BDETNONTZ_TO_BDETTZ,
        BDEM_CONVERTIBLE,
        BDEM_FROM_STRING,
        BDEM_FROM_ELEMREF,
        BDEM_TO_STRING
    };

    typedef bslmf_MetaInt<BDEM_NO_CONVERSION>       NoConversion;
    typedef bslmf_MetaInt<BDEM_TO_ELEMREF>          ElemRefFromAny;
    typedef bslmf_MetaInt<BDEM_FLOAT_TO_INT>        IntegralFromFloating;
    typedef bslmf_MetaInt<BDEM_BDETNONTZ_TO_BDETTZ> BdetTzFromBdetNonTz;
    typedef bslmf_MetaInt<BDEM_CONVERTIBLE>         IsConvertible;
    typedef bslmf_MetaInt<BDEM_FROM_STRING>         AnyFromString;
    typedef bslmf_MetaInt<BDEM_FROM_ELEMREF>        AnyFromElemRef;
    typedef bslmf_MetaInt<BDEM_TO_STRING>           StringFromOstreamable;

    template <typename SRCTYPE, typename DSTTYPE>
    struct ConversionCategory;
    template <typename SRCTYPE, typename DSTTYPE>
    friend struct ConversionCategory;

    // PRIVATE CLASS METHODS
    template <typename TESTTYPE>
    static bool isUnset(const TESTTYPE& value, const TrueType&);
        // Return 'true' if the specified 'value' of parameterized 'TESTTYPE'
        // is unset as defined by:
        //..
        //  bdetu_Unset<TESTTYPE>::isUnset(value)
        //..
        // and 'false' otherwise.  Note that this method is specialized for
        // 'bool' and 'char'.

    static bool isUnset(const bool& value, const TrueType&);
    static bool isUnset(const char& value, const TrueType&);
        // Return 'false'.  Note that 'bool' and 'char' values are never
        // considered to be unset, contrary to 'bdetu_unset'.

    template <typename TESTTYPE>
    static bool isUnset(const TESTTYPE& value, const FalseType&);
        // Return 'false' indicating that parameterized 'TESTTYPE' does not
        // have an unset value as defined by:
        //..
        //  bdetu_Unset<TESTTYPE>::isUnset(value)
        //..

    template <typename TESTTYPE>
    static void setUnset(TESTTYPE *value, const TrueType&);
        // Set the specified '*value' of parameterized 'TESTTYPE' to its unset
        // value as defined by:
        //..
        //  bdetu_Unset<TESTTYPE>::isUnset(value)
        //..

    template <typename TESTTYPE>
    static void setUnset(TESTTYPE *value, const FalseType&);
        // This method has no effect.

    static int fromString(bool            *dstAddr, const char *srcValue);
    static int fromString(char            *dstAddr, const char *srcValue);
    static int fromString(short           *dstAddr, const char *srcValue);
    static int fromString(int             *dstAddr, const char *srcValue);
    static int fromString(bsls_Types::Int64
                                          *dstAddr, const char *srcValue);
    static int fromString(float           *dstAddr, const char *srcValue);
    static int fromString(double          *dstAddr, const char *srcValue);
    static int fromString(bdet_Datetime   *dstAddr, const char *srcValue);
    static int fromString(bdet_Date       *dstAddr, const char *srcValue);
    static int fromString(bdet_Time       *dstAddr, const char *srcValue);
    static int fromString(bdet_DatetimeTz *dstAddr, const char *srcValue);
    static int fromString(bdet_DateTz     *dstAddr, const char *srcValue);
    static int fromString(bdet_TimeTz     *dstAddr, const char *srcValue);
        // Return 0, with no effect on the specified '*dstAddr', if the
        // specified 'srcValue' is null or is the empty string.  Otherwise,
        // parse the contents of 'srcValue' according to the type of '*dstAddr'
        // and load the resulting value into '*dstAddr'.  Return 0 on success,
        // and a non-zero value with no effect on '*dstAddr' otherwise.  Note
        // that initial white-space in 'srcValue' is skipped.

    static int fromString(void *dstAddr, const char *srcValue);
        // Return a non-zero value with no effect on the specified '*dstAddr'.
        // Note that this version of 'fromString' is called for any type that
        // is not convertible from a string.

    template <typename SRCTYPE, typename DSTTYPE>
    static int doConvert(DSTTYPE        *dstAddr,
                         const SRCTYPE&  srcValue,
                         const NoConversion&);
        // Return a non-zero value indicating that there is no conversion from
        // parameterized 'SRCTYPE' to parameterized 'DSTTYPE' with no effect on
        // the specified '*dstAddr'.

    template <typename SRCTYPE>
    static int doConvert(bsl::string    *dstAddr,
                         const SRCTYPE&  srcValue,
                         const StringFromOstreamable&);
        // Convert the specified 'srcValue' of parameterized 'SRCTYPE' to a
        // string that is formatted according to 'bdeu_PrintMethods::print' and
        // load the resulting value into the specified '*dstAddr'.  Return 0 on
        // success, and a non-zero value with no effect on '*dstAddr'
        // otherwise.  Note that non-zero is returned if 'SRCTYPE' does not
        // satisfy the requirements of 'bdeu_PrintMethods::print'.

    template <typename DSTTYPE>
    static int doConvert(DSTTYPE            *dstAddr,
                         const bsl::string&  srcValue,
                         const AnyFromString&);
    template <typename DSTTYPE>
    static int doConvert(DSTTYPE            *dstAddr,
                         const char         *srcValue,
                         const AnyFromString&);
        // Return 0, with no effect on the specified '*dstAddr', if the
        // specified 'srcValue' is null or is the empty string.  Otherwise,
        // parse the contents of 'srcValue' according to parameterized
        // 'DSTTYPE' and load the resulting value into '*dstAddr'.  Return 0 on
        // success, and a non-zero value with no effect on '*dstAddr'
        // otherwise.  Note that initial white-space in 'srcValue' is skipped.

    template <typename DSTTYPE>
    static int doConvert(DSTTYPE                  *dstAddr,
                         const bdem_ConstElemRef&  srcValue,
                         const AnyFromElemRef&);
        // Convert the 'bdem' element referenced by the specified 'srcValue' to
        // a value of parameterized 'DSTTYPE' and load the resulting value into
        // the specified '*dstAddr'.  Return 0 on success, and a non-zero value
        // otherwise.

    template <typename SRCTYPE, typename DSTTYPE>
    static int doConvert(DSTTYPE        *dstAddr,
                         const SRCTYPE&  srcValue,
                         const IsConvertible&);
        // Convert the specified 'srcValue' of parameterized 'SRCTYPE' to a
        // value of parameterized 'DSTTYPE', and load the resulting value into
        // the specified '*dstAddr'.  Return 0 on success, and a non-zero value
        // otherwise.

    template <typename INTEGRALTYPE, typename FLOATINGTYPE>
    static int doConvert(INTEGRALTYPE *dstAddr,
                         FLOATINGTYPE  srcValue,
                         const IntegralFromFloating&);
        // Convert the specified 'srcValue' of parameterized 'FLOATINGTYPE'
        // into a value of parameterized 'INTEGRALTYPE', load the resulting
        // value into the specified '*dstAddr', and return 0 (to indicate
        // success).

    template <typename BDETNONTZTYPE, typename BDETTZTYPE>
    static int doConvert(BDETTZTYPE           *dstAddr,
                         const BDETNONTZTYPE&  srcValue,
                         const BdetTzFromBdetNonTz&);
        // Convert the specified 'srcValue' of parameterized 'BDETNONTZTYPE' to
        // a value of parameterized 'BDETTZTYPE', load the resulting value into
        // the specified '*dstAddr', and return 0 (to indicate success).  Note
        // that the timezone offset of '*dstAddr' following a call to this
        // method will be 0.

    template <typename SRCTYPE>
    static int doConvert(bdem_ElemRef      *dstAddr,
                         const SRCTYPE&     srcValue,
                         const ElemRefFromAny&);
    template <typename SRCTYPE>
    static int doConvert(bdem_ConstElemRef *dstAddr,
                         const SRCTYPE&     srcValue,
                         const ElemRefFromAny&);
        // Convert the specified 'srcValue' of parameterized 'SRCTYPE' to a
        // value having the type of the 'bdem' element referenced by the
        // specified '*dstAddr, and load the resulting value into '*dstAddr'.
        // Return 0 on success, and a non-zero value otherwise.

    template <typename SRCTYPE, typename DSTTYPE>
    static int constConvert(DSTTYPE *dstAddr, SRCTYPE& srcValue);
        // Convert the specified 'srcValue' of parameterized 'SRCTYPE' to a
        // value of parameterized 'DSTTYPE', and load the resulting value into
        // the specified '*dstAddr'.  Return 0 on success, and a non-zero value
        // otherwise.

#ifdef BSLS_PLATFORM__CMP_SUN

    template <typename SRCTYPE, bsl::size_t SRCARRAYSIZE, typename DSTTYPE>
    static int constConvert(DSTTYPE *dstAddr,
                            SRCTYPE  (&srcValue)[SRCARRAYSIZE]);
        // Convert the first element of the specified 'srcValue' array, whose
        // elements are of parameterized 'SRCTYPE', to a value of parameterized
        // 'DSTTYPE', and load the resulting value into the specified
        // '*dstAddr'.  Return 0 on success, and a non-zero value otherwise.
        // The behavior is undefined unless 'SRCARRAYSIZE > 0'.

    template <typename SRCTYPE, typename DSTTYPE>
    static int nonconstConvert(DSTTYPE *dstAddr, SRCTYPE& srcValue);
        // Convert the specified 'srcValue' of parameterized 'SRCTYPE' to a
        // value of parameterized 'DSTTYPE', and load the resulting value into
        // the specified '*dstAddr'.  Return 0 on success, and a non-zero value
        // otherwise.

    template <typename SRCTYPE, bsl::size_t SRCARRAYSIZE, typename DSTTYPE>
    static int nonconstConvert(DSTTYPE *dstAddr,
                               SRCTYPE  (&srcValue)[SRCARRAYSIZE]);
        // Convert the first element of the specified 'srcValue' array, whose
        // elements are of parameterized 'SRCTYPE', to a value of parameterized
        // 'DSTTYPE', and load the resulting value into the specified
        // '*dstAddr'.  Return 0 on success, and a non-zero value otherwise.
        // The behavior is undefined unless 'SRCARRAYSIZE > 0'.

    template <typename SRCTYPE, bsl::size_t SRCARRAYSIZE>
    static int doToBdemType(void                *dstAddr,
                            bdem_ElemType::Type  dstType,
                            SRCTYPE              (&srcValue)[SRCARRAYSIZE]);
        // Convert the first element of the specified 'srcValue' array, whose
        // elements are of parameterized 'SRCTYPE', to a value of the specified
        // 'dstType', and load the resulting value into the specified
        // '*dstAddr'.  Return 0 on success, and a non-zero value otherwise.
        // The behavior is undefined unless the type of the 'bdem' element at
        // 'dstAddr' is of 'dstType' and 'SRCARRAYSIZE > 0'.
#endif

    template <typename SRCTYPE>
    static int doToBdemType(void                *dstAddr,
                            bdem_ElemType::Type  dstType,
                            SRCTYPE&             srcValue);
        // Convert the specified 'srcValue' of parameterized 'SRCTYPE' to a
        // value of the specified 'dstType' and load the resulting value into
        // the specified '*dstAddr'.  Return 0 on success, and a non-zero value
        // otherwise.  The behavior is undefined unless the type of the 'bdem'
        // element at 'dstAddr' is of 'dstType'.

  public:
    // CLASS METHODS
    template <typename SRCTYPE, typename DSTTYPE>
    static int convert(DSTTYPE *dstAddr, const SRCTYPE& srcValue);
#ifdef BSLS_PLATFORM__CMP_SUN
    template <typename SRCTYPE, typename DSTTYPE>
    static int convert(DSTTYPE *dstAddr, SRCTYPE& srcValue);
#endif
       // Convert the specified 'srcValue' of parameterized 'SRCTYPE' to a
       // value of parameterized 'DSTTYPE' and load the resulting value into
       // the specified '*dstAddr'.  Return 0 on success, and a non-zero value
       // with no effect on '*dstAddr' otherwise.  Extended conversion is
       // performed as described in the component-level documentation.  Note
       // that this function will fail to compile if converting to
       // 'bsl::string' from a type that cannot be printed to an 'ostream'
       // (via 'operator<<' or 'bdeu_PrintMethods::print()').

    template <typename SRCTYPE, typename DSTTYPE>
    static int convert(const DSTTYPE            *dstAddr,
                       const SRCTYPE&            srcValue);
    static int convert(bdem_ConstElemRef        *dstAddr,
                       const bdem_ConstElemRef&  srcValue);
    static int convert(bdem_ConstElemRef        *dstAddr,
                       const bdem_ElemRef&       srcValue);
        // Return a non-zero value.  Note that conversion to 'const' always
        // fails.

    static int convert(bool *dstAddr, const char *srcValue);
        // Return 0, with no effect on the specified '*dstAddr', if the
        // specified 'srcValue' is null or is the empty string.  Otherwise,
        // load 'false' into '*dstAddr' if the initial contents of 'srcValue',
        // after skipping whitespace, is either "0" or "false" (compared
        // case-insensitively), and return 0.  Otherwise, load 'true' into
        // '*dstAddr' if the initial contents of 'srcValue', after skipping
        // whitespace, is either "1" or "true" (compared case-insensitively),
        // and return 0.  Otherwise, return a non-zero value with no effect on
        // '*dstAddr'.

    static int convert(bdem_List *dstAddr, const bdem_Row&  srcValue);
    static int convert(bdem_Row  *dstAddr, const bdem_List& srcValue);
    static int convert(bdem_Row  *dstAddr, const bdem_Row&  srcValue);
        // Convert the specified 'srcValue' (row or list) to a value of type
        // indicated by the specified '*dstAddr', and load the resulting value
        // into '*dstAddr'.  Return 0 on success, and a non-zero value
        // otherwise.  Conversion of a 'srcValue' that is a 'bdem_Row' to a
        // 'bdem_List' is performed as if by assignment and always succeeds.
        // Conversion to a 'bdem_Row' succeeds only if
        // 'dstAddr->length() == srcValue.length()' and the types of the
        // respective 'bdem' elements in the source and destination are the
        // same.

    static int convert(bdem_ChoiceArrayItem        *dstAddr,
                       const bdem_Choice&           srcValue);
    static int convert(bdem_Choice                 *dstAddr,
                       const bdem_ChoiceArrayItem&  srcValue);
    static int convert(bdem_ChoiceArrayItem        *dstAddr,
                       const bdem_ChoiceArrayItem&  srcValue);
        // Convert the specified 'srcValue' (choice or choice array item) to a
        // value of type indicated by the specified '*dstAddr', and load the
        // resulting value into '*dstAddr'.  Return 0 on success, and a
        // non-zero value otherwise.  Conversion of a 'srcValue' that is a
        // 'bdem_ChoiceArrayItem' to a 'bdem_Choice' is performed as if by
        // assignment and always succeeds.  Conversion to a
        // 'bdem_ChoiceArrayItem' succeeds only if the following holds:
        //..
        //  1) srcSelector < dstAddr->numSelections()
        //  2) srcValue.selectionType() == dstAddr->selectionType(srcSelector)
        //..
        // where 'srcSelector == srcValue.selector()'.

    static int convert(bdem_ElemRef             *dstAddr,
                       const bdem_ConstElemRef&  srcValue);
    static int convert(bdem_ElemRef             *dstAddr,
                       const bdem_ElemRef&       srcValue);
        // Convert the 'bdem' element referenced by the specified 'srcValue' to
        // a value having the type of the 'bdem' element referenced by the
        // specified '*dstAddr, and load the resulting value into '*dstAddr'.
        // Return 0 on success, and a non-zero value otherwise.

    static int convert(bdet_DateTz     *dstAddr, const bdet_Date& srcValue);
    static int convert(bdet_TimeTz     *dstAddr, const bdet_Time& srcValue);
    static int convert(bdet_DatetimeTz *dstAddr, const bdet_Datetime&
                                                                  srcValue);
        // Convert the specified 'srcValue' to a value of type of the
        // specified '*dstAddr, load the resulting value into '*dstAddr',
        // and return 0 (to indicate success).  Note that the timezone offset
        // of '*dstAddr' following a call to this method will be 0.

    static int convert(bsl::string *dstAddr, bool        srcValue);
        // Load "0" into the specified '*dstAddr' if the specified 'srcValue'
        // is 'false', and load "1" otherwise.  Return 0 in any case.

    static int convert(bsl::string *dstAddr, char        srcValue);
    static int convert(bsl::string *dstAddr, short       srcValue);
    static int convert(bsl::string *dstAddr, int         srcValue);
    static int convert(bsl::string *dstAddr, bsls_Types::Int64
                                                         srcValue);
    static int convert(bsl::string *dstAddr, float       srcValue);
    static int convert(bsl::string *dstAddr, double      srcValue);
    static int convert(bsl::string *dstAddr, long double srcValue);
        // If the specified 'srcValue' is null according to:
        //..
        //  bdetu_Unset<TYPE>::isUnset(srcValue)
        //..
        // then clear the specified '*dstAddr' string and return 0.  Otherwise,
        // format 'srcValue' in a manner equivalent to 'operator<<' and load
        // the resulting string into '*dstAddr'.  Return 0 in any case.

    static int convert(bsl::string *dstAddr, const bdet_Datetime&   srcValue);
    static int convert(bsl::string *dstAddr, const bdet_Date&       srcValue);
    static int convert(bsl::string *dstAddr, const bdet_Time&       srcValue);
    static int convert(bsl::string *dstAddr, const bdet_DatetimeTz& srcValue);
    static int convert(bsl::string *dstAddr, const bdet_DateTz&     srcValue);
    static int convert(bsl::string *dstAddr, const bdet_TimeTz&     srcValue);
        // If the specified 'srcValue' is null according to:
        //..
        //  bdetu_Unset<TYPE>::isUnset(srcValue)
        //..
        // then clear the specified '*dstAddr' string and return 0.  Otherwise,
        // format 'srcValue' according to ISO 8601 and load the resulting
        // string into '*dstAddr'.  Return 0 in any case.

#ifndef BSLS_PLATFORM__CMP_SUN

    template <typename SRCTYPE, bsl::size_t SRCARRAYSIZE, typename DSTTYPE>
    static int convert(DSTTYPE *dstAddr, SRCTYPE (&srcValue)[SRCARRAYSIZE])
        // Convert the first element of the specified 'srcValue' array, whose
        // elements are of parameterized 'SRCTYPE', to a value of parameterized
        // 'DSTTYPE', and load the resulting value into the specified
        // '*dstAddr'.  Return 0 on success, and a non-zero value otherwise.
        // The behavior is undefined unless 'SRCARRAYSIZE > 0'.  Note that this
        // method is defined 'inline' within this class to work around a bug in
        // the AIX 'xlC' compiler.
    {
        return bdem_Convert::convert(dstAddr, &srcValue[0]);
    }
    template <typename SRCTYPE, bsl::size_t SRCARRAYSIZE, typename DSTTYPE>
    static int convert(DSTTYPE       *dstAddr,
                       const SRCTYPE  (&srcValue)[SRCARRAYSIZE])
        // Convert the first element of the specified 'srcValue' array, whose
        // elements are of parameterized 'SRCTYPE', to a value of parameterized
        // 'DSTTYPE', and load the resulting value into the specified
        // '*dstAddr'.  Return 0 on success, and a non-zero value otherwise.
        // The behavior is undefined unless 'SRCARRAYSIZE > 0'.  Note that this
        // method is defined 'inline' within this class to work around a bug in
        // the AIX 'xlC' compiler.
    {
        return bdem_Convert::convert(dstAddr, &srcValue[0]);
    }

#endif

    // *** 'bdem' type of destination is indicated explicitly ***

    template <typename SRCTYPE>
    static int toBdemType(void                *dstAddr,
                          bdem_ElemType::Type  dstType,
                          const SRCTYPE&       srcValue);
        // Convert the specified 'srcValue' to a value of the specified
        // 'dstType' and load the resulting value into the specified
        // '*dstAddr'.  Return 0 on success, and a non-zero value otherwise.
        // The behavior is undefined unless the type of the 'bdem' element at
        // 'dstAddr' is of 'dstType'.

#ifndef BSLS_PLATFORM__CMP_SUN

    template <typename SRCTYPE, bsl::size_t SRCARRAYSIZE>
    static int toBdemType(void                *dstAddr,
                          bdem_ElemType::Type  dstType,
                          const SRCTYPE        (&srcValue)[SRCARRAYSIZE])
        // Convert the first element of the specified 'srcValue' array, whose
        // elements are of parameterized 'SRCTYPE', to a value of the specified
        // 'dstType', and load the resulting value into the specified
        // '*dstAddr'.  Return 0 on success, and a non-zero value otherwise.
        // The behavior is undefined unless the type of the 'bdem' element at
        // 'dstAddr' is of 'dstType' and 'SRCARRAYSIZE > 0'.  Note that this
        // method is defined 'inline' within this class to work around a bug in
        // the AIX 'xlC' compiler.
    {
        return bdem_Convert::toBdemType(dstAddr, dstType, &srcValue[0]);
    }

#endif

    static int toBdemType(void                     *dstAddr,
                          bdem_ElemType::Type       dstType,
                          const bdem_ConstElemRef&  srcValue);
    static int toBdemType(void                     *dstAddr,
                          bdem_ElemType::Type       dstType,
                          const bdem_ElemRef&       srcValue);
        // Convert the 'bdem' element referenced by the specified 'srcValue' to
        // a value of the specified 'dstType' and load the resulting value into
        // the specified '*dstAddr'.  Return 0 on success, and a non-zero value
        // otherwise.  The behavior is undefined unless the type of the 'bdem'
        // element at 'dstAddr' is of 'dstType'.

    template <typename SRCTYPE>
    static int toBdemType(const void          *dstAddr,
                          bdem_ElemType::Type  dstType,
                          const SRCTYPE&       srcValue);
        // Return a non-zero value.  The behavior is undefined unless the type
        // of the 'bdem' element at the specified 'dstAddr' is of the specified
        // 'dstType'.  Note that conversion to 'const' always fails.

    // *** 'bdem' type of source is indicated explicitly ***

    template <typename DSTTYPE>
    static int fromBdemType(DSTTYPE             *dstAddr,
                            const void          *srcAddr,
                            bdem_ElemType::Type  srcType);
        // Convert the 'bdem' element at the specified 'srcAddr' having the
        // specified 'srcType' to a value of parameterized 'DSTTYPE' and load
        // the resulting value into the specified '*dstAddr'.  Return 0 on
        // success, and a non-zero value otherwise.  The behavior is undefined
        // unless the type of the 'bdem' element at 'srcAddr' is of 'srcType'.

    static int fromBdemType(bdem_ElemRef        *dstAddr,
                            const void          *srcAddr,
                            bdem_ElemType::Type  srcType);
        // Convert the 'bdem' element at the specified 'srcAddr' having the
        // specified 'srcType' to a value having the type of the 'bdem' element
        // referenced by the specified '*dstAddr, and load the resulting value
        // into '*dstAddr'.  Return 0 on success, and a non-zero value
        // otherwise.  The behavior is undefined unless the type of the 'bdem'
        // element at 'srcAddr' is of 'srcType'.

    template <typename DSTTYPE>
    static int fromBdemType(const DSTTYPE       *dstAddr,
                            const void          *srcAddr,
                            bdem_ElemType::Type  srcType);
    static int fromBdemType(bdem_ConstElemRef   *dstAddr,
                            const void          *srcAddr,
                            bdem_ElemType::Type  srcType);
        // Return a non-zero value.  The behavior is undefined unless the type
        // of the 'bdem' element at 'srcAddr' is of 'srcType'.  Note that
        // conversion to 'const' always fails.

    // *** 'bdem' types of destination and source are indicated explicitly ***

    static int convertBdemTypes(void                *dstAddr,
                                bdem_ElemType::Type  dstType,
                                const void          *srcAddr,
                                bdem_ElemType::Type  srcType);
        // Convert the 'bdem' element at the specified 'srcAddr' having the
        // specified 'srcType' to a value of the specified 'dstType' and load
        // the resulting value into the specified '*dstAddr'.  Return 0 on
        // success, and a non-zero value otherwise.  The behavior is undefined
        // unless the type of the 'bdem' element at 'dstAddr' is of 'dstType'
        // and the type of the 'bdem' element at 'srcAddr' is of 'srcType'.

    static int convertBdemTypes(const void          *dstAddr,
                                bdem_ElemType::Type  dstType,
                                const void          *srcAddr,
                                bdem_ElemType::Type  srcType);
        // Return a non-zero value.  The behavior is undefined unless the type
        // of the 'bdem' element at 'dstAddr' is of 'dstType' and the type of
        // the 'bdem' element at 'srcAddr' is of 'srcType'.  Note that
        // conversion to 'const' always fails.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                            // -------------------
                            // struct bdem_Convert
                            // -------------------

template <typename SRCTYPE>
struct bdem_Convert::IsToElemRef<SRCTYPE, bdem_ElemRef>
                : bdem_Convert::TrueType {
};

template <typename DSTTYPE>
struct bdem_Convert::IsFloatingToIntegral<float, DSTTYPE>
                : bslmf_IsConvertible<DSTTYPE, int> {
};

template <typename DSTTYPE>
struct bdem_Convert::IsFloatingToIntegral<double, DSTTYPE>
                : bslmf_IsConvertible<DSTTYPE, int> {
};

template <typename DSTTYPE>
struct bdem_Convert::IsFloatingToIntegral<long double, DSTTYPE>
                : bslmf_IsConvertible<DSTTYPE, int> {
};

template <typename DSTTYPE>
struct bdem_Convert::IsBdetNonTzToBdetTz<bdet_Date, DSTTYPE>
                : bslmf_IsSame<DSTTYPE, bdet_DateTz> {
};

template <typename DSTTYPE>
struct bdem_Convert::IsBdetNonTzToBdetTz<bdet_Time, DSTTYPE>
                : bslmf_IsSame<DSTTYPE, bdet_TimeTz> {
};

template <typename DSTTYPE>
struct bdem_Convert::IsBdetNonTzToBdetTz<bdet_Datetime, DSTTYPE>
                : bslmf_IsSame<DSTTYPE, bdet_DatetimeTz> {
};

template <typename DSTTYPE>
struct bdem_Convert::IsFromString<bsl::string, DSTTYPE>
                : bdem_Convert::TrueType {
};

template <typename DSTTYPE>
struct bdem_Convert::IsFromString<const char*, DSTTYPE>
                : bdem_Convert::TrueType {
};

template <typename DSTTYPE>
struct bdem_Convert::IsFromString<char*, DSTTYPE>
                : bdem_Convert::TrueType {
};

template <typename DSTTYPE>
struct bdem_Convert::IsFromElemRef<bdem_ElemRef, DSTTYPE>
                : bdem_Convert::TrueType {
};

template <typename DSTTYPE>
struct bdem_Convert::IsFromElemRef<bdem_ConstElemRef, DSTTYPE>
                : bdem_Convert::TrueType {
};

template <typename SRCTYPE>
struct bdem_Convert::IsPrintableToString<SRCTYPE, bsl::string>
                : bdem_Convert_IsOstreamable<SRCTYPE> {
};

template <typename VECTOR_ITEM>
struct bdem_Convert::IsPrintableToString<bsl::vector<VECTOR_ITEM>, bsl::string>
                : bdem_Convert::IsPrintableToString<VECTOR_ITEM, bsl::string> {
};

template <typename SRCTYPE, typename DSTTYPE>
struct bdem_Convert::ConversionCategory {
    enum {
        VALUE =
          IsToElemRef<SRCTYPE, DSTTYPE>::VALUE          ? BDEM_TO_ELEMREF     :
          IsFloatingToIntegral<SRCTYPE, DSTTYPE>::VALUE ? BDEM_FLOAT_TO_INT   :
          IsBdetNonTzToBdetTz<SRCTYPE, DSTTYPE>::VALUE  ?
                                                     BDEM_BDETNONTZ_TO_BDETTZ :
          bslmf_IsConvertible<SRCTYPE, DSTTYPE>::VALUE  ? BDEM_CONVERTIBLE    :
          IsFromString<SRCTYPE, DSTTYPE>::VALUE         ? BDEM_FROM_STRING    :
          IsFromElemRef<SRCTYPE, DSTTYPE>::VALUE        ? BDEM_FROM_ELEMREF   :
          IsPrintableToString<SRCTYPE, DSTTYPE>::VALUE  ? BDEM_TO_STRING      :
          BDEM_NO_CONVERSION
    };

    typedef bslmf_MetaInt<VALUE> Type;
};

// PRIVATE CLASS METHODS
template <typename TESTTYPE>
inline
bool bdem_Convert::isUnset(const TESTTYPE& value, const TrueType&)
{
    return bdetu_Unset<TESTTYPE>::isUnset(value);
}

inline
bool bdem_Convert::isUnset(const bool&, const TrueType&)
{
    return false;  // 'bool' is never considered to be unset.
}

inline
bool bdem_Convert::isUnset(const char&, const TrueType&)
{
    return false;  // 'char' is never considered to be unset.
}

template <typename TESTTYPE>
inline
bool bdem_Convert::isUnset(const TESTTYPE&, const FalseType&)
{
    return false;
}

template <typename TESTTYPE>
inline
void bdem_Convert::setUnset(TESTTYPE *value, const TrueType&)
{
    BSLS_ASSERT_SAFE(value);

    bdetu_Unset<TESTTYPE>::makeUnset(value);
}

template <typename TESTTYPE>
inline
void bdem_Convert::setUnset(TESTTYPE *, const FalseType&)
{
}

inline
int bdem_Convert::fromString(void *, const char *)
{
    return -1;
}

template <typename SRCTYPE, typename DSTTYPE>
inline
int bdem_Convert::doConvert(DSTTYPE *, const SRCTYPE&, const NoConversion&)
{
    return -1;
}

template <typename SRCTYPE>
int bdem_Convert::doConvert(bsl::string    *dstAddr,
                            const SRCTYPE&  srcValue,
                            const StringFromOstreamable&)
{
    BSLS_ASSERT(dstAddr);

    // Format 'srcValue' using 'ostream'.
    bsl::ostringstream stream;
    bdeu_PrintMethods::print(stream, srcValue, 0, -1);
    *dstAddr = stream.str();
    return 0;
}

template <typename DSTTYPE>
inline
int bdem_Convert::doConvert(DSTTYPE            *dstAddr,
                            const bsl::string&  srcValue,
                            const AnyFromString&)
{
    BSLS_ASSERT_SAFE(dstAddr);

    return bdem_Convert::fromString(dstAddr, srcValue.c_str());
}

template <typename DSTTYPE>
inline
int bdem_Convert::doConvert(DSTTYPE    *dstAddr,
                            const char *srcValue,
                            const AnyFromString&)
{
    BSLS_ASSERT_SAFE(dstAddr);

    return bdem_Convert::fromString(dstAddr, srcValue);
}

template <typename DSTTYPE>
inline
int bdem_Convert::doConvert(DSTTYPE                  *dstAddr,
                            const bdem_ConstElemRef&  srcValue,
                            const AnyFromElemRef&)
{
    BSLS_ASSERT_SAFE(dstAddr || bdem_ElemType::BDEM_VOID == srcValue.type());

    return bdem_Convert::fromBdemType(dstAddr,
                                      srcValue.data(), srcValue.type());
}

template <typename SRCTYPE, typename DSTTYPE>
inline
int bdem_Convert::doConvert(DSTTYPE        *dstAddr,
                            const SRCTYPE&  srcValue,
                            const IsConvertible&)
{
    BSLS_ASSERT_SAFE(dstAddr);

    // Cast to avoid warnings about (e.g.) 'float' -> 'int' and
    // 'int' -> 'float' conversions.
    *dstAddr = static_cast<DSTTYPE>(srcValue);
    return 0;
}

template <typename INTEGRALTYPE, typename FLOATINGTYPE>
inline
int bdem_Convert::doConvert(INTEGRALTYPE *dstAddr,
                            FLOATINGTYPE  srcValue,
                            const IntegralFromFloating&)
{
    BSLS_ASSERT_SAFE(dstAddr);

    // Cast to avoid warnings about (e.g.) 'float' -> 'int' conversions.
    *dstAddr = static_cast<INTEGRALTYPE>(srcValue);
    return 0;
}

template <typename BDETNONTZTYPE, typename BDETTZTYPE>
inline
int bdem_Convert::doConvert(BDETTZTYPE           *dstAddr,
                            const BDETNONTZTYPE&  srcValue,
                            const BdetTzFromBdetNonTz&)
{
    BSLS_ASSERT_SAFE(dstAddr);

    return bdem_Convert::convert(dstAddr, srcValue);
}

template <typename SRCTYPE>
int bdem_Convert::doConvert(bdem_ElemRef   *dstAddr,
                            const SRCTYPE&  srcValue,
                            const ElemRefFromAny&)
{
    BSLS_ASSERT(dstAddr);

    const bool                isDstNull = dstAddr->isBound()
                                       && dstAddr->isNull();
    const bdem_ElemType::Type dstType   = dstAddr->type();

    // 'dstType' is 'bdem_ElemType::BDEM_VOID' if '*dstAddr' is unbound.

    void *dstData;

    switch (dstType) {
      case bdem_ElemType::BDEM_CHAR: {
        dstData = &dstAddr->theModifiableChar();
      } break;
      case bdem_ElemType::BDEM_SHORT: {
        dstData = &dstAddr->theModifiableShort();
      } break;
      case bdem_ElemType::BDEM_INT: {
        dstData = &dstAddr->theModifiableInt();
      } break;
      case bdem_ElemType::BDEM_INT64: {
        dstData = &dstAddr->theModifiableInt64();
      } break;
      case bdem_ElemType::BDEM_FLOAT: {
        dstData = &dstAddr->theModifiableFloat();
      } break;
      case bdem_ElemType::BDEM_DOUBLE: {
        dstData = &dstAddr->theModifiableDouble();
      } break;
      case bdem_ElemType::BDEM_STRING: {
        dstData = &dstAddr->theModifiableString();
      } break;
      case bdem_ElemType::BDEM_DATETIME: {
        dstData = &dstAddr->theModifiableDatetime();
      } break;
      case bdem_ElemType::BDEM_DATE: {
        dstData = &dstAddr->theModifiableDate();
      } break;
      case bdem_ElemType::BDEM_TIME: {
        dstData = &dstAddr->theModifiableTime();
      } break;
      case bdem_ElemType::BDEM_CHAR_ARRAY: {
        dstData = &dstAddr->theModifiableCharArray();
      } break;
      case bdem_ElemType::BDEM_SHORT_ARRAY: {
        dstData = &dstAddr->theModifiableShortArray();
      } break;
      case bdem_ElemType::BDEM_INT_ARRAY: {
        dstData = &dstAddr->theModifiableIntArray();
      } break;
      case bdem_ElemType::BDEM_INT64_ARRAY: {
        dstData = &dstAddr->theModifiableInt64Array();
      } break;
      case bdem_ElemType::BDEM_FLOAT_ARRAY: {
        dstData = &dstAddr->theModifiableFloatArray();
      } break;
      case bdem_ElemType::BDEM_DOUBLE_ARRAY: {
        dstData = &dstAddr->theModifiableDoubleArray();
      } break;
      case bdem_ElemType::BDEM_STRING_ARRAY: {
        dstData = &dstAddr->theModifiableStringArray();
      } break;
      case bdem_ElemType::BDEM_DATETIME_ARRAY: {
        dstData = &dstAddr->theModifiableDatetimeArray();
      } break;
      case bdem_ElemType::BDEM_DATE_ARRAY: {
        dstData = &dstAddr->theModifiableDateArray();
      } break;
      case bdem_ElemType::BDEM_TIME_ARRAY: {
        dstData = &dstAddr->theModifiableTimeArray();
      } break;
      case bdem_ElemType::BDEM_LIST: {
        dstData = &dstAddr->theModifiableList();
      } break;
      case bdem_ElemType::BDEM_TABLE: {
        dstData = &dstAddr->theModifiableTable();
      } break;
      case bdem_ElemType::BDEM_BOOL: {
        dstData = &dstAddr->theModifiableBool();
      } break;
      case bdem_ElemType::BDEM_DATETIMETZ: {
        dstData = &dstAddr->theModifiableDatetimeTz();
      } break;
      case bdem_ElemType::BDEM_DATETZ: {
        dstData = &dstAddr->theModifiableDateTz();
      } break;
      case bdem_ElemType::BDEM_TIMETZ: {
        dstData = &dstAddr->theModifiableTimeTz();
      } break;
      case bdem_ElemType::BDEM_BOOL_ARRAY: {
        dstData = &dstAddr->theModifiableBoolArray();
      } break;
      case bdem_ElemType::BDEM_DATETIMETZ_ARRAY: {
        dstData = &dstAddr->theModifiableDatetimeTzArray();
      } break;
      case bdem_ElemType::BDEM_DATETZ_ARRAY: {
        dstData = &dstAddr->theModifiableDateTzArray();
      } break;
      case bdem_ElemType::BDEM_TIMETZ_ARRAY: {
        dstData = &dstAddr->theModifiableTimeTzArray();
      } break;
      case bdem_ElemType::BDEM_CHOICE: {
        dstData = &dstAddr->theModifiableChoice();
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY: {
        dstData = &dstAddr->theModifiableChoiceArray();
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY_ITEM: {
        dstData = &dstAddr->theModifiableChoiceArrayItem();
      } break;
      case bdem_ElemType::BDEM_ROW: {
        dstData = &dstAddr->theModifiableRow();
      } break;
      case bdem_ElemType::BDEM_VOID:
      default: {
        return -1;                                                    // RETURN
      } break;
    }

    const int rc = bdem_Convert::toBdemType(dstData, dstType, srcValue);

    if (rc && isDstNull) {
        // If the destination was originally null and the conversion failed
        // then make it null again.
        dstAddr->makeNull();
    }

    return rc;
}

template <typename SRCTYPE>
inline
int bdem_Convert::doConvert(bdem_ConstElemRef *,
                            const SRCTYPE&     ,
                            const ElemRefFromAny&)
{
    return -1;  // destination is 'const'-qualified
}

#ifdef BSLS_PLATFORM__CMP_SUN

template <typename SRCTYPE, bsl::size_t SRCARRAYSIZE, typename DSTTYPE>
inline
int bdem_Convert::constConvert(DSTTYPE *dstAddr,
                               SRCTYPE  (&srcValue)[SRCARRAYSIZE])
{
    BSLS_ASSERT_SAFE(dstAddr);

    SRCTYPE *const srcPointer = &srcValue[0];
    return bdem_Convert::constConvert(dstAddr, srcPointer);
}

template <typename SRCTYPE, typename DSTTYPE>
inline
int bdem_Convert::nonconstConvert(DSTTYPE *dstAddr, SRCTYPE& srcValue)
{
    BSLS_ASSERT_SAFE(dstAddr);

    // Call 'convert' (not 'constConvert') to ensure that all of the overloads
    // are honored.

    return bdem_Convert::convert(dstAddr,
                                 const_cast<const SRCTYPE&>(srcValue));
}

template <typename SRCTYPE, bsl::size_t SRCARRAYSIZE, typename DSTTYPE>
inline
int bdem_Convert::nonconstConvert(DSTTYPE *dstAddr,
                                  SRCTYPE  (&srcValue)[SRCARRAYSIZE])
{
    BSLS_ASSERT_SAFE(dstAddr);

    // Call 'convert' (not 'constConvert') to ensure that all of the overloads
    // are honored.

    SRCTYPE *const srcPointer = &srcValue[0];
    return bdem_Convert::convert(dstAddr, srcPointer);
}

#endif

// CLASS METHODS

template <typename SRCTYPE, typename DSTTYPE>
int bdem_Convert::constConvert(DSTTYPE *dstAddr, SRCTYPE& srcValue)
{
    BSLS_ASSERT_SAFE(dstAddr);

    typedef typename bslmf_RemoveCvq<SRCTYPE>::Type ncvqFromType;

    typedef typename ConversionCategory<ncvqFromType, DSTTYPE>::Type Category;
    const int result = bdem_Convert::doConvert(dstAddr, srcValue, Category());

    if (result) {
        return result;                                                // RETURN
    }

    // If 'srcValue' is null, make '*dstAddr' null, also.  Do nothing if either
    // 'SRCTYPE' or 'DSTTYPE' does not have a null value as determined by
    // 'bdetu_UnsetValueIsDefined<T>'.  Note that we do this test only AFTER a
    // successful conversion to ensure that non-convertible types do not report
    // success.  (E.g., conversion from a null 'int' to a 'bdem_List' should
    // fail.)  Also note that the nullness bit of 'dstAddr', if any (i.e., in
    // the case where 'DSTTYPE' is an ElemRef), has already been accounted for;
    // furthermore, 'bdetu_UnsetValueIsDefined<T>' is 'false' for ElemRefs.

    bslmf_MetaInt<bdetu_UnsetValueIsDefined<ncvqFromType>::VALUE>
                                                               srcTypeHasUnset;
    bslmf_MetaInt<bdetu_UnsetValueIsDefined<DSTTYPE>::VALUE>   dstTypeHasUnset;

    if (bdem_Convert::isUnset(srcValue, srcTypeHasUnset)) {
        bdem_Convert::setUnset(dstAddr, dstTypeHasUnset);
    }

    return 0;
}

template <typename SRCTYPE, typename DSTTYPE>
inline
int bdem_Convert::convert(DSTTYPE *dstAddr, const SRCTYPE& srcValue)
{
    BSLS_ASSERT_SAFE(dstAddr);

    return bdem_Convert::constConvert(dstAddr, srcValue);
}

#ifdef BSLS_PLATFORM__CMP_SUN

template <typename SRCTYPE, typename DSTTYPE>
inline
int bdem_Convert::convert(DSTTYPE *dstAddr, SRCTYPE& srcValue)
{
    BSLS_ASSERT_SAFE(dstAddr);

    return bdem_Convert::nonconstConvert(dstAddr, srcValue);
}

#endif

template <typename SRCTYPE, typename DSTTYPE>
inline
int bdem_Convert::convert(const DSTTYPE *, const SRCTYPE&)
{
    return -1;  // destination is 'const'-qualified
}

inline
int bdem_Convert::convert(bool *dstAddr, const char *srcValue)
{
    BSLS_ASSERT_SAFE(dstAddr);

    return bdem_Convert::fromString(dstAddr, srcValue);
}

inline
int bdem_Convert::convert(bdem_ConstElemRef *, const bdem_ConstElemRef&)
{
    return -1;  // destination is 'const'-qualified
}

inline
int bdem_Convert::convert(bdem_ConstElemRef *, const bdem_ElemRef&)
{
    return -1;  // destination is 'const'-qualified
}

inline
int bdem_Convert::convert(bsl::string *dstAddr, bool srcValue)
{
    BSLS_ASSERT_SAFE(dstAddr);

    return bdem_Convert::convert(dstAddr, int(srcValue));
}

template <typename SRCTYPE>
int bdem_Convert::doToBdemType(void                *dstAddr,
                               bdem_ElemType::Type  dstType,
                               SRCTYPE&             srcValue)
{
    BSLS_ASSERT(dstAddr || bdem_ElemType::BDEM_VOID == dstType);

    int result;

    switch (dstType) {
      case bdem_ElemType::BDEM_CHAR: {
        result = bdem_Convert::convert((char*) dstAddr, srcValue);
      } break;
      case bdem_ElemType::BDEM_SHORT: {
        result = bdem_Convert::convert((short*) dstAddr, srcValue);
      } break;
      case bdem_ElemType::BDEM_INT: {
        result = bdem_Convert::convert((int*) dstAddr, srcValue);
      } break;
      case bdem_ElemType::BDEM_INT64: {
        result = bdem_Convert::convert((Int64*) dstAddr, srcValue);
      } break;
      case bdem_ElemType::BDEM_FLOAT: {
        result = bdem_Convert::convert((float*) dstAddr, srcValue);
      } break;
      case bdem_ElemType::BDEM_DOUBLE: {
        result = bdem_Convert::convert((double*) dstAddr, srcValue);
      } break;
      case bdem_ElemType::BDEM_STRING: {
        result = bdem_Convert::convert((bsl::string*) dstAddr, srcValue);
      } break;
      case bdem_ElemType::BDEM_DATETIME: {
        result = bdem_Convert::convert((bdet_Datetime*) dstAddr, srcValue);
      } break;
      case bdem_ElemType::BDEM_DATE: {
        result = bdem_Convert::convert((bdet_Date*) dstAddr, srcValue);
      } break;
      case bdem_ElemType::BDEM_TIME: {
        result = bdem_Convert::convert((bdet_Time*) dstAddr, srcValue);
      } break;
      case bdem_ElemType::BDEM_CHAR_ARRAY: {
        result = bdem_Convert::convert((bsl::vector<char>*) dstAddr, srcValue);
      } break;
      case bdem_ElemType::BDEM_SHORT_ARRAY: {
        result = bdem_Convert::convert((bsl::vector<short>*) dstAddr,
                                       srcValue);
      } break;
      case bdem_ElemType::BDEM_INT_ARRAY: {
        result = bdem_Convert::convert((bsl::vector<int>*) dstAddr, srcValue);
      } break;
      case bdem_ElemType::BDEM_INT64_ARRAY: {
        result = bdem_Convert::convert((bsl::vector<Int64>*) dstAddr,
                                       srcValue);
      } break;
      case bdem_ElemType::BDEM_FLOAT_ARRAY: {
        result = bdem_Convert::convert((bsl::vector<float>*) dstAddr,
                                       srcValue);
      } break;
      case bdem_ElemType::BDEM_DOUBLE_ARRAY: {
        result = bdem_Convert::convert((bsl::vector<double>*) dstAddr,
                                       srcValue);
      } break;
      case bdem_ElemType::BDEM_STRING_ARRAY: {
        result = bdem_Convert::convert((bsl::vector<bsl::string>*) dstAddr,
                                       srcValue);
      } break;
      case bdem_ElemType::BDEM_DATETIME_ARRAY: {
        result = bdem_Convert::convert((bsl::vector<bdet_Datetime>*) dstAddr,
                                       srcValue);
      } break;
      case bdem_ElemType::BDEM_DATE_ARRAY: {
        result = bdem_Convert::convert((bsl::vector<bdet_Date>*) dstAddr,
                                       srcValue);
      } break;
      case bdem_ElemType::BDEM_TIME_ARRAY: {
        result = bdem_Convert::convert((bsl::vector<bdet_Time>*) dstAddr,
                                       srcValue);
      } break;
      case bdem_ElemType::BDEM_LIST: {
        result = bdem_Convert::convert((bdem_List*) dstAddr, srcValue);
      } break;
      case bdem_ElemType::BDEM_TABLE: {
        result = bdem_Convert::convert((bdem_Table*) dstAddr, srcValue);
      } break;
      case bdem_ElemType::BDEM_BOOL: {
        result = bdem_Convert::convert((bool*) dstAddr, srcValue);
      } break;
      case bdem_ElemType::BDEM_DATETIMETZ: {
        result = bdem_Convert::convert((bdet_DatetimeTz*) dstAddr, srcValue);
      } break;
      case bdem_ElemType::BDEM_DATETZ: {
        result = bdem_Convert::convert((bdet_DateTz*) dstAddr, srcValue);
      } break;
      case bdem_ElemType::BDEM_TIMETZ: {
        result = bdem_Convert::convert((bdet_TimeTz*) dstAddr, srcValue);
      } break;
      case bdem_ElemType::BDEM_BOOL_ARRAY: {
        result = bdem_Convert::convert((bsl::vector<bool>*) dstAddr, srcValue);
      } break;
      case bdem_ElemType::BDEM_DATETIMETZ_ARRAY: {
        result = bdem_Convert::convert((bsl::vector<bdet_DatetimeTz>*) dstAddr,
                                       srcValue);
      } break;
      case bdem_ElemType::BDEM_DATETZ_ARRAY: {
        result = bdem_Convert::convert((bsl::vector<bdet_DateTz>*) dstAddr,
                                       srcValue);
      } break;
      case bdem_ElemType::BDEM_TIMETZ_ARRAY: {
        result = bdem_Convert::convert((bsl::vector<bdet_TimeTz>*) dstAddr,
                                       srcValue);
      } break;
      case bdem_ElemType::BDEM_CHOICE: {
        result = bdem_Convert::convert((bdem_Choice*) dstAddr, srcValue);
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY: {
        result = bdem_Convert::convert((bdem_ChoiceArray*) dstAddr, srcValue);
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY_ITEM: {
        result = bdem_Convert::convert((bdem_ChoiceArrayItem*) dstAddr,
                                       srcValue);
      } break;
      case bdem_ElemType::BDEM_ROW: {
        result = bdem_Convert::convert((bdem_Row*) dstAddr, srcValue);
      } break;
      case bdem_ElemType::BDEM_VOID:
      default: {
        result = -1;
      } break;
    }

    return result;
}

#ifdef BSLS_PLATFORM__CMP_SUN

template <typename SRCTYPE, bsl::size_t SRCARRAYSIZE>
inline
int bdem_Convert::doToBdemType(void                *dstAddr,
                               bdem_ElemType::Type  dstType,
                               SRCTYPE              (&srcValue)[SRCARRAYSIZE])
{
    BSLS_ASSERT_SAFE(dstAddr || bdem_ElemType::BDEM_VOID == dstType);

    SRCTYPE *const srcPointer = &srcValue[0];
    return bdem_Convert::doToBdemType(dstAddr, dstType, srcPointer);
}

#endif

template <typename SRCTYPE>
inline
int bdem_Convert::toBdemType(void                *dstAddr,
                             bdem_ElemType::Type  dstType,
                             const SRCTYPE&       srcValue)
{
    BSLS_ASSERT_SAFE(dstAddr || bdem_ElemType::BDEM_VOID == dstType);

    return bdem_Convert::doToBdemType(dstAddr, dstType, srcValue);
}

inline
int bdem_Convert::toBdemType(void                     *dstAddr,
                             bdem_ElemType::Type       dstType,
                             const bdem_ConstElemRef&  srcValue)
{
    BSLS_ASSERT_SAFE(dstAddr || bdem_ElemType::BDEM_VOID == dstType);

    return bdem_Convert::convertBdemTypes(dstAddr, dstType,
                                          srcValue.data(), srcValue.type());
}

template <typename SRCTYPE>
inline
int bdem_Convert::toBdemType(const void *, bdem_ElemType::Type, const SRCTYPE&)
{
    return -1;  // destination is 'const'-qualified
}

template <typename DSTTYPE>
int bdem_Convert::fromBdemType(DSTTYPE             *dstAddr,
                               const void          *srcAddr,
                               bdem_ElemType::Type  srcType)
{
    BSLS_ASSERT(srcAddr || bdem_ElemType::BDEM_VOID == srcType);

    int result;

    switch (srcType) {
      case bdem_ElemType::BDEM_CHAR: {
        result = bdem_Convert::convert(dstAddr, *(char*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_SHORT: {
        result = bdem_Convert::convert(dstAddr, *(short*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_INT: {
        result = bdem_Convert::convert(dstAddr, *(int*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_INT64: {
        result = bdem_Convert::convert(dstAddr, *(Int64*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_FLOAT: {
        result = bdem_Convert::convert(dstAddr, *(float*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_DOUBLE: {
        result = bdem_Convert::convert(dstAddr, *(double*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_STRING: {
        result = bdem_Convert::convert(dstAddr, *(bsl::string*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_DATETIME: {
        result = bdem_Convert::convert(dstAddr, *(bdet_Datetime*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_DATE: {
        result = bdem_Convert::convert(dstAddr, *(bdet_Date*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_TIME: {
        result = bdem_Convert::convert(dstAddr, *(bdet_Time*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_CHAR_ARRAY: {
        result = bdem_Convert::convert(dstAddr, *(bsl::vector<char>*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_SHORT_ARRAY: {
        result = bdem_Convert::convert(dstAddr,
                                       *(bsl::vector<short>*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_INT_ARRAY: {
        result = bdem_Convert::convert(dstAddr, *(bsl::vector<int>*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_INT64_ARRAY: {
        result = bdem_Convert::convert(dstAddr,
                                       *(bsl::vector<Int64>*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_FLOAT_ARRAY: {
        result = bdem_Convert::convert(dstAddr,
                                       *(bsl::vector<float>*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_DOUBLE_ARRAY: {
        result = bdem_Convert::convert(dstAddr,
                                       *(bsl::vector<double>*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_STRING_ARRAY: {
        result = bdem_Convert::convert(dstAddr,
                                       *(bsl::vector<bsl::string>*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_DATETIME_ARRAY: {
        result = bdem_Convert::convert(
                                      dstAddr,
                                      *(bsl::vector<bdet_Datetime>*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_DATE_ARRAY: {
        result = bdem_Convert::convert(dstAddr,
                                       *(bsl::vector<bdet_Date>*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_TIME_ARRAY: {
        result = bdem_Convert::convert(dstAddr,
                                       *(bsl::vector<bdet_Time>*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_LIST: {
        result = bdem_Convert::convert(dstAddr, *(bdem_List*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_TABLE: {
        result = bdem_Convert::convert(dstAddr, *(bdem_Table*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_BOOL: {
        result = bdem_Convert::convert(dstAddr, *(bool*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_DATETIMETZ: {
        result = bdem_Convert::convert(dstAddr, *(bdet_DatetimeTz*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_DATETZ: {
        result = bdem_Convert::convert(dstAddr, *(bdet_DateTz*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_TIMETZ: {
        result = bdem_Convert::convert(dstAddr, *(bdet_TimeTz*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_BOOL_ARRAY: {
        result = bdem_Convert::convert(dstAddr, *(bsl::vector<bool>*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_DATETIMETZ_ARRAY: {
        result = bdem_Convert::convert(
                                     dstAddr,
                                     *(bsl::vector<bdet_DatetimeTz>*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_DATETZ_ARRAY: {
        result = bdem_Convert::convert(dstAddr,
                                       *(bsl::vector<bdet_DateTz>*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_TIMETZ_ARRAY: {
        result = bdem_Convert::convert(dstAddr,
                                       *(bsl::vector<bdet_TimeTz>*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_CHOICE: {
        result = bdem_Convert::convert(dstAddr, *(bdem_Choice*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY: {
        result = bdem_Convert::convert(dstAddr, *(bdem_ChoiceArray*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY_ITEM: {
        result = bdem_Convert::convert(dstAddr,
                                       *(bdem_ChoiceArrayItem*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_ROW: {
        result = bdem_Convert::convert(dstAddr, *(bdem_Row*) srcAddr);
      } break;
      case bdem_ElemType::BDEM_VOID:
      default: {
        result = -1;
      } break;
    }

    return result;
}

template <typename DSTTYPE>
inline
int bdem_Convert::fromBdemType(const DSTTYPE       *,
                               const void          *,
                               bdem_ElemType::Type)
{
    return -1;  // destination is 'const'-qualified
}

inline
int bdem_Convert::fromBdemType(bdem_ConstElemRef   *,
                               const void          *,
                               bdem_ElemType::Type)
{
    return -1;  // destination is 'const'-qualified
}

inline
int bdem_Convert::convertBdemTypes(const void          *,
                                   bdem_ElemType::Type  ,
                                   const void          *,
                                   bdem_ElemType::Type)
{
    return -1;  // destination is 'const'-qualified
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
