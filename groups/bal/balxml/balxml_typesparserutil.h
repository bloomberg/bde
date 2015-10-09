// balxml_typesparserutil.h                                           -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALXML_TYPESPARSERUTIL
#define INCLUDED_BALXML_TYPESPARSERUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a utility for parsing types using XML formatting.
//
//@CLASSES:
//   balxml::TypesParserUtil: utility for parsing using XML formatting
//
//@SEE_ALSO: http://www.w3.org/TR/xmlschema-2/
//
//@DESCRIPTION: The 'balxml::TypesParserUtil' struct provided by this component
// contains the following functions:
//..
//  o 'parse':        Parse a string using the supplied formatting mode.
//  o 'parseBase64':  Parse a string using
//                    'bdlat_FormattingMode::e_BASE64'.
//  o 'parseDecimal': Parse a string using 'bdlat_FormattingMode::e_DEC'.
//  o 'parseDefault': Parse a string using
//                    'bdlat_FormattingMode::e_DEFAULT'.
//  o 'parseHex':     Parse a string using 'bdlat_FormattingMode::e_HEX'.
//  o 'parseList':    Parse a string using
//                    'bdlat_FormattingMode::e_IS_LIST'.
//  o 'parseText':    Parse a string using 'bdlat_FormattingMode::e_TEXT'.
//..
// The input strings are parsed according to each type's lexical representation
// as described in the XML Schema Specification, which is available at
// 'http://www.w3.org/TR/xmlschema-2/'.
//
// The following C++ Type / Formatting Mode combinations are supported by this
// component:
//..
//      C++ Type                            Formatting Mode
//      --------                            ---------------
//      bool                                DEFAULT, DEC, TEXT
//      char                                DEFAULT, DEC, TEXT
//      unsigned char                       DEFAULT, DEC
//      [unsigned] short                    DEFAULT, DEC
//      [unsigned] int                      DEFAULT, DEC
//      [unsigned] long                     DEFAULT, DEC
//      bsls::Types::[Uint64|Int64]         DEFAULT, DEC
//      float                               DEFAULT, DEC
//      double                              DEFAULT, DEC
//      bsl::string                         DEFAULT, TEXT, BASE64, HEX
//      bdlt::Date                           DEFAULT
//      bdlt::DateTz                         DEFAULT
//      bdlt::Datetime                       DEFAULT
//      bdlt::DateTimeTz                     DEFAULT
//      bdlt::Time                           DEFAULT
//      bdlt::TimeTz                         DEFAULT
//      bsl::vector<char>                   DEFAULT, BASE64, HEX, TEXT, IS_LIST
//..
// In addition to the types listed above, this component also recognizes the
// following 'bdeat' type categories:
//..
//      'bdeat' Type Category               Formatting Mode
//      ---------------------               ---------------
//      Array                               IS_LIST
//      CustomizedType                      Base type's formatting modes
//      DynamicType                         Runtime type's formatting modes
//      Enumeration                         DEFAULT, TEXT, DECIMAL
//..
// When 'bdlat_FormattingMode::e_DEFAULT' is used, the actual formatting mode
// selected is based on the following mapping:
//..
//      C++ Type                            Default Formatting Mode
//      --------                            -----------------------
//      bool                                DEC or TEXT
//      [unsigned] char                     DEC
//      [unsigned] short                    DEC
//      [unsigned] int                      DEC
//      [unsigned] long                     DEC
//      bsls::Types::[Uint64|Int64]         DEC
//      bsl::string                         TEXT
//      bsl::vector<char>                   BASE64
//
//      'bdeat' Type Category               Default Formatting Mode
//      ---------------------               -----------------------
//      Enumeration                         TEXT
//..
//
///Usage
///-----
// The following snippets of code illustrate how to parse a Base64 string into
// an 'bsl::vector<char>':
//..
//  #include <balxml_typesparserutil.h>
//
//  #include <cassert>
//  #include <vector>
//
//  using namespace BloombergLP;
//
//  void usageExample()
//  {
//      const char INPUT[]      = "YWJjZA==";  // "abcd" in Base64
//      const int  INPUT_LENGTH = sizeof(INPUT) - 1;
//
//      bsl::vector<char> vec;
//
//      int retCode = balxml::TypesParserUtil::parseBase64(&vec,
//                                                         INPUT,
//                                                         INPUT_LENGTH);
//
//      assert(0   == retCode);
//      assert(4   == vec.size());
//      assert('a' == vec[0]);
//      assert('b' == vec[1]);
//      assert('c' == vec[2]);
//      assert('d' == vec[3]);
//  }
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALXML_LISTPARSER
#include <balxml_listparser.h>
#endif

#ifndef INCLUDED_BDLAT_CUSTOMIZEDTYPEFUNCTIONS
#include <bdlat_customizedtypefunctions.h>
#endif

#ifndef INCLUDED_BDLAT_ENUMFUNCTIONS
#include <bdlat_enumfunctions.h>
#endif

#ifndef INCLUDED_BDLAT_FORMATTINGMODE
#include <bdlat_formattingmode.h>
#endif

#ifndef INCLUDED_BDLAT_TYPECATEGORY
#include <bdlat_typecategory.h>
#endif

#ifndef INCLUDED_BDLT_DATE
#include <bdlt_date.h>
#endif

#ifndef INCLUDED_BDLT_DATETIME
#include <bdlt_datetime.h>
#endif

#ifndef INCLUDED_BDLT_DATETIMETZ
#include <bdlt_datetimetz.h>
#endif

#ifndef INCLUDED_BDLT_DATETZ
#include <bdlt_datetz.h>
#endif

#ifndef INCLUDED_BDLT_ISO8601UTIL
#include <bdlt_iso8601util.h>
#endif

#ifndef INCLUDED_BDLT_TIME
#include <bdlt_time.h>
#endif

#ifndef INCLUDED_BDLT_TIMETZ
#include <bdlt_timetz.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
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

namespace balxml {
                           // ======================
                           // struct TypesParserUtil
                           // ======================

struct TypesParserUtil {
    // This 'struct' contains functions for parsing input strings using various
    // XML formatting modes.

    template <class TYPE>
    static int parse(TYPE       *result,
                     const char *input,
                     int         inputLength,
                     int         formattingMode);
        // Parse the specified 'input' of the specified 'inputLength' using the
        // specified 'formattingMode' and load the result into the specified
        // 'result'.  Return 0 on success, and a non-zero value otherwise.  The
        // behavior is undefined unless the parameterized 'TYPE' and the
        // 'formattingMode' combination is supported (supported combinations
        // are listed in the component-level documentation).

    template <class TYPE>
    static int parseBase64(TYPE       *result,
                           const char *input,
                           int         inputLength);
        // Parse the specified 'input' of the specified 'inputLength' using
        // 'bdlat_FormattingMode::e_BASE64' and load the result into the
        // specified 'result'.  Return 0 on success, and a non-zero value
        // otherwise.

    template <class TYPE>
    static int parseDecimal(TYPE       *result,
                            const char *input,
                            int         inputLength);
        // Parse the specified 'input' of the specified 'inputLength' using
        // 'bdlat_FormattingMode::e_DEC' and load the result into the
        // specified 'result'.  Return 0 on success, and a non-zero value
        // otherwise.

    template <class TYPE>
    static int parseDefault(TYPE       *result,
                            const char *input,
                            int         inputLength);
        // Parse the specified 'input' of the specified 'inputLength' using
        // 'bdlat_FormattingMode::e_DEFAULT' and load the result into the
        // specified 'result'.  Return 0 on success, and a non-zero value
        // otherwise.

    template <class TYPE>
    static int parseHex(TYPE       *result,
                        const char *input,
                        int         inputLength);
        // Parse the specified 'input' of the specified 'inputLength' using
        // 'bdlat_FormattingMode::e_HEX' and load the result into the
        // specified 'result'.  Return 0 on success, and a non-zero value
        // otherwise.

    template <class TYPE>
    static int parseList(TYPE       *result,
                         const char *input,
                         int         inputLength);
        // Parse the specified 'input' of the specified 'inputLength' using
        // 'bdlat_FormattingMode::e_LIST' and load the result into the
        // specified 'result'.  Return 0 on success, and a non-zero value
        // otherwise.

    template <class TYPE>
    static int parseText(TYPE       *result,
                         const char *input,
                         int         inputLength);
        // Parse the specified 'input' of the specified 'inputLength' using
        // 'bdlat_FormattingMode::e_TEXT' and load the result into the
        // specified 'result'.  Return 0 on success, and a non-zero value
        // otherwise.
};

                         // ==========================
                         // struct TypesParserUtil_Imp
                         // ==========================

struct TypesParserUtil_Imp {
    // This 'struct' contains functions that are used in the implementation of
    // this component.

    // BASE64 FUNCTIONS
    template <class TYPE>
    static int parseBase64(TYPE                            *result,
                           const char                      *input,
                           int                              inputLength,
                           bdlat_TypeCategory::DynamicType);

    template <class TYPE, class ANY_CATEGORY>
    static int parseBase64(TYPE         *result,
                           const char   *input,
                           int           inputLength,
                           ANY_CATEGORY);

    static int parseBase64(bsl::string                *result,
                           const char                 *input,
                           int                         inputLength,
                           bdlat_TypeCategory::Simple);
    static int parseBase64(bsl::vector<char>         *result,
                           const char                *input,
                           int                        inputLength,
                           bdlat_TypeCategory::Array);

    // DECIMAL FUNCTIONS
    template <class TYPE>
    static int parseDecimal(TYPE                            *result,
                            const char                      *input,
                            int                              inputLength,
                            bdlat_TypeCategory::Enumeration);

    template <class TYPE>
    static int parseDecimal(TYPE                               *result,
                            const char                         *input,
                            int                                 inputLength,
                            bdlat_TypeCategory::CustomizedType);

    template <class TYPE>
    static int parseDecimal(TYPE                            *result,
                            const char                      *input,
                            int                              inputLength,
                            bdlat_TypeCategory::DynamicType);

    template <class TYPE, class ANY_CATEGORY>
    static int parseDecimal(TYPE         *result,
                            const char   *input,
                            int           inputLength,
                            ANY_CATEGORY);

    static int parseDecimal(bool                       *result,
                            const char                 *input,
                            int                         inputLength,
                            bdlat_TypeCategory::Simple);
    static int parseDecimal(char                       *result,
                            const char                 *input,
                            int                         inputLength,
                            bdlat_TypeCategory::Simple);
    static int parseDecimal(short                      *result,
                            const char                 *input,
                            int                         inputLength,
                            bdlat_TypeCategory::Simple);
    static int parseDecimal(int                        *result,
                            const char                 *input,
                            int                         inputLength,
                            bdlat_TypeCategory::Simple);
    static int parseDecimal(bsls::Types::Int64         *result,
                            const char                 *input,
                            int                         inputLength,
                            bdlat_TypeCategory::Simple);
    static int parseDecimal(unsigned char              *result,
                            const char                 *input,
                            int                         inputLength,
                            bdlat_TypeCategory::Simple);
    static int parseDecimal(unsigned short             *result,
                            const char                 *input,
                            int                         inputLength,
                            bdlat_TypeCategory::Simple);
    static int parseDecimal(unsigned int               *result,
                            const char                 *input,
                            int                         inputLength,
                            bdlat_TypeCategory::Simple);
    static int parseDecimal(bsls::Types::Uint64        *result,
                            const char                 *input,
                            int                         inputLength,
                            bdlat_TypeCategory::Simple);

    static int parseDecimal(float                      *result,
                            const char                 *input,
                            int                         inputLength,
                            bdlat_TypeCategory::Simple);
    static int parseDecimal(double                     *result,
                            const char                 *input,
                            int                         inputLength,
                            bdlat_TypeCategory::Simple);

    // DEFAULT FUNCTIONS
    template <class TYPE>
    static int parseDefault(TYPE                            *result,
                            const char                      *input,
                            int                              inputLength,
                            bdlat_TypeCategory::Enumeration);

    template <class TYPE>
    static int parseDefault(TYPE                               *result,
                            const char                         *input,
                            int                                 inputLength,
                            bdlat_TypeCategory::CustomizedType);

    template <class TYPE>
    static int parseDefault(TYPE                            *result,
                            const char                      *input,
                            int                              inputLength,
                            bdlat_TypeCategory::DynamicType);

    template <class TYPE, class ANY_CATEGORY>
    static int parseDefault(TYPE         *result,
                            const char   *input,
                            int           inputLength,
                            ANY_CATEGORY);

    static int parseDefault(bool                       *result,
                            const char                 *input,
                            int                         inputLength,
                            bdlat_TypeCategory::Simple);
    static int parseDefault(char                       *result,
                            const char                 *input,
                            int                         inputLength,
                            bdlat_TypeCategory::Simple);
    static int parseDefault(short                      *result,
                            const char                 *input,
                            int                         inputLength,
                            bdlat_TypeCategory::Simple);
    static int parseDefault(int                        *result,
                            const char                 *input,
                            int                         inputLength,
                            bdlat_TypeCategory::Simple);
    static int parseDefault(bsls::Types::Int64         *result,
                            const char                 *input,
                            int                         inputLength,
                            bdlat_TypeCategory::Simple);
    static int parseDefault(unsigned char              *result,
                            const char                 *input,
                            int                         inputLength,
                            bdlat_TypeCategory::Simple);
    static int parseDefault(unsigned short             *result,
                            const char                 *input,
                            int                         inputLength,
                            bdlat_TypeCategory::Simple);
    static int parseDefault(unsigned int               *result,
                            const char                 *input,
                            int                         inputLength,
                            bdlat_TypeCategory::Simple);
    static int parseDefault(bsls::Types::Uint64        *result,
                            const char                 *input,
                            int                         inputLength,
                            bdlat_TypeCategory::Simple);
    static int parseDefault(float                      *result,
                            const char                 *input,
                            int                         inputLength,
                            bdlat_TypeCategory::Simple);
    static int parseDefault(double                     *result,
                            const char                 *input,
                            int                         inputLength,
                            bdlat_TypeCategory::Simple);
    static int parseDefault(bsl::string                *result,
                            const char                 *input,
                            int                         inputLength,
                            bdlat_TypeCategory::Simple);
    static int parseDefault(bdlt::Date                 *result,
                            const char                 *input,
                            int                         inputLength,
                            bdlat_TypeCategory::Simple);
    static int parseDefault(bdlt::DateTz               *result,
                            const char                 *input,
                            int                         inputLength,
                            bdlat_TypeCategory::Simple);
    static int parseDefault(bdlt::Datetime             *result,
                            const char                 *input,
                            int                         inputLength,
                            bdlat_TypeCategory::Simple);
    static int parseDefault(bdlt::DatetimeTz           *result,
                            const char                 *input,
                            int                         inputLength,
                            bdlat_TypeCategory::Simple);
    static int parseDefault(bdlt::Time                 *result,
                            const char                 *input,
                            int                         inputLength,
                            bdlat_TypeCategory::Simple);
    static int parseDefault(bdlt::TimeTz               *result,
                            const char                 *input,
                            int                         inputLength,
                            bdlat_TypeCategory::Simple);
    static int parseDefault(bsl::vector<char>         *result,
                            const char                *input,
                            int                        inputLength,
                            bdlat_TypeCategory::Array);

    // HEX FUNCTIONS
    template <class TYPE>
    static int parseHex(TYPE                            *result,
                        const char                      *input,
                        int                              inputLength,
                        bdlat_TypeCategory::DynamicType);

    template <class TYPE, class ANY_CATEGORY>
    static int parseHex(TYPE         *result,
                        const char   *input,
                        int           inputLength,
                        ANY_CATEGORY);

    static int parseHex(bsl::string                *result,
                        const char                 *input,
                        int                         inputLength,
                        bdlat_TypeCategory::Simple);
    static int parseHex(bsl::vector<char>         *result,
                        const char                *input,
                        int                        inputLength,
                        bdlat_TypeCategory::Array);

    // LIST FUNCTIONS
    template <class TYPE>
    static int parseList(TYPE                      *result,
                         const char                *input,
                         int                        inputLength,
                         bdlat_TypeCategory::Array);

    template <class TYPE>
    static int parseList(TYPE                            *result,
                         const char                      *input,
                         int                              inputLength,
                         bdlat_TypeCategory::DynamicType);

    template <class TYPE, class ANY_CATEGORY>
    static int parseList(TYPE         *result,
                         const char   *input,
                         int           inputLength,
                         ANY_CATEGORY);

    // TEXT FUNCTIONS
    template <class TYPE>
    static int parseText(TYPE                            *result,
                         const char                      *input,
                         int                              inputLength,
                         bdlat_TypeCategory::Enumeration);

    template <class TYPE>
    static int parseText(TYPE                               *result,
                         const char                         *input,
                         int                                 inputLength,
                         bdlat_TypeCategory::CustomizedType);

    template <class TYPE>
    static int parseText(TYPE                            *result,
                         const char                      *input,
                         int                              inputLength,
                         bdlat_TypeCategory::DynamicType);

    template <class TYPE, class ANY_CATEGORY>
    static int parseText(TYPE         *result,
                         const char   *input,
                         int           inputLength,
                         ANY_CATEGORY);

    static int parseText(bool                       *result,
                         const char                 *input,
                         int                         inputLength,
                         bdlat_TypeCategory::Simple);
    static int parseText(char                       *result,
                         const char                 *input,
                         int                         inputLength,
                         bdlat_TypeCategory::Simple);
    static int parseText(bsl::string                *result,
                         const char                 *input,
                         int                         inputLength,
                         bdlat_TypeCategory::Simple);
    static int parseText(bsl::vector<char>         *result,
                         const char                *input,
                         int                        inputLength,
                         bdlat_TypeCategory::Array);

};

// ============================================================================
//                               PROXY CLASSES
// ============================================================================

                // ===========================================
                // struct TypesParserUtil_Imp_parseBase64Proxy
                // ===========================================

struct TypesParserUtil_Imp_parseBase64Proxy {
    // Component-private struct.  Do not use.

    // DATA MEMBERS
    const char *d_input_p;
    int         d_inputLength;

    // CREATORS

    // Creators have been omitted to allow simple static initialization of this
    // struct.

    // FUNCTIONS
    template <class TYPE>
    inline
    int operator()(TYPE *, bslmf::Nil)
    {
        BSLS_ASSERT_SAFE(0);
        return -1;
    }

    template <class TYPE, class ANY_CATEGORY>
    inline
    int operator()(TYPE *object, ANY_CATEGORY category)
    {
        return TypesParserUtil_Imp::parseBase64(object,
                                                d_input_p,
                                                d_inputLength,
                                                category);
    }
};

                // ============================================
                // struct TypesParserUtil_Imp_parseDecimalProxy
                // ============================================

struct TypesParserUtil_Imp_parseDecimalProxy {
    // Component-private struct.  Do not use.

    // DATA MEMBERS
    const char *d_input_p;
    int         d_inputLength;

    // CREATORS

    // Creators have been omitted to allow simple static initialization of this
    // struct.

    // FUNCTIONS
    template <class TYPE>
    inline
    int operator()(TYPE *, bslmf::Nil)
    {
        BSLS_ASSERT_SAFE(0);
        return -1;
    }

    template <class TYPE, class ANY_CATEGORY>
    inline
    int operator()(TYPE *object, ANY_CATEGORY category)
    {
        return TypesParserUtil_Imp::parseDecimal(object,
                                                 d_input_p,
                                                 d_inputLength,
                                                 category);
    }
};

                // ============================================
                // struct TypesParserUtil_Imp_parseDefaultProxy
                // ============================================

struct TypesParserUtil_Imp_parseDefaultProxy {
    // Component-private struct.  Do not use.

    // DATA MEMBERS
    const char *d_input_p;
    int         d_inputLength;

    // CREATORS

    // Creators have been omitted to allow simple static initialization of this
    // struct.

    // FUNCTIONS
    template <class TYPE>
    inline
    int operator()(TYPE *, bslmf::Nil)
    {
        BSLS_ASSERT_SAFE(0);
        return -1;
    }

    template <class TYPE, class ANY_CATEGORY>
    inline
    int operator()(TYPE *object, ANY_CATEGORY category)
    {
        return TypesParserUtil_Imp::parseDefault(object,
                                                 d_input_p,
                                                 d_inputLength,
                                                 category);
    }
};

                  // ========================================
                  // struct TypesParserUtil_Imp_parseHexProxy
                  // ========================================

struct TypesParserUtil_Imp_parseHexProxy {
    // Component-private struct.  Do not use.

    // DATA MEMBERS
    const char *d_input_p;
    int         d_inputLength;

    // CREATORS

    // Creators have been omitted to allow simple static initialization of this
    // struct.

    // FUNCTIONS
    template <class TYPE>
    inline
    int operator()(TYPE *, bslmf::Nil)
    {
        BSLS_ASSERT_SAFE(0);
        return -1;
    }

    template <class TYPE, class ANY_CATEGORY>
    inline
    int operator()(TYPE *object, ANY_CATEGORY category)
    {
        return TypesParserUtil_Imp::parseHex(object,
                                             d_input_p,
                                             d_inputLength,
                                             category);
    }
};

                 // =========================================
                 // struct TypesParserUtil_Imp_parseListProxy
                 // =========================================

struct TypesParserUtil_Imp_parseListProxy {
    // Component-private struct.  Do not use.

    // DATA MEMBERS
    const char *d_input_p;
    int         d_inputLength;

    // CREATORS

    // Creators have been omitted to allow simple static initialization of this
    // struct.

    // FUNCTIONS
    template <class TYPE>
    inline
    int operator()(TYPE *, bslmf::Nil)
    {
        BSLS_ASSERT_SAFE(0);
        return -1;
    }

    template <class TYPE, class ANY_CATEGORY>
    inline
    int operator()(TYPE *object, ANY_CATEGORY category)
    {
        return TypesParserUtil_Imp::parseList(object,
                                              d_input_p,
                                              d_inputLength,
                                              category);
    }
};

                 // =========================================
                 // struct TypesParserUtil_Imp_parseTextProxy
                 // =========================================

struct TypesParserUtil_Imp_parseTextProxy {
    // Component-private struct.  Do not use.

    // DATA MEMBERS
    const char *d_input_p;
    int         d_inputLength;

    // CREATORS

    // Creators have been omitted to allow simple static initialization of this
    // struct.

    // FUNCTIONS
    template <class TYPE>
    inline
    int operator()(TYPE *, bslmf::Nil)
    {
        BSLS_ASSERT_SAFE(0);
        return -1;
    }

    template <class TYPE, class ANY_CATEGORY>
    inline
    int operator()(TYPE *object, ANY_CATEGORY category)
    {
        return TypesParserUtil_Imp::parseText(object,
                                              d_input_p,
                                              d_inputLength,
                                              category);
    }
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           // ----------------------
                           // struct TypesParserUtil
                           // ----------------------

template <class TYPE>
int TypesParserUtil::parse(TYPE       *result,
                           const char *input,
                           int         inputLength,
                           int         formattingMode)
{
    enum { k_FAILURE = - 1 };

    typedef typename bdlat_TypeCategory::Select<TYPE>::Type Tag;

    if (formattingMode & bdlat_FormattingMode::e_LIST) {
        return TypesParserUtil_Imp::parseList(result,
                                              input,
                                              inputLength,
                                              Tag());                 // RETURN
    }

    switch (formattingMode & bdlat_FormattingMode::e_TYPE_MASK) {
      case bdlat_FormattingMode::e_BASE64: {
        return TypesParserUtil_Imp::parseBase64(result,
                                                input,
                                                inputLength,
                                                Tag());               // RETURN
      }
      case bdlat_FormattingMode::e_DEC: {
        return TypesParserUtil_Imp::parseDecimal(result,
                                                 input,
                                                 inputLength,
                                                 Tag());              // RETURN
      }
      case bdlat_FormattingMode::e_DEFAULT: {
        return TypesParserUtil_Imp::parseDefault(result,
                                                 input,
                                                 inputLength,
                                                 Tag());              // RETURN
      }
      case bdlat_FormattingMode::e_HEX: {
        return TypesParserUtil_Imp::parseHex(result,
                                             input,
                                             inputLength,
                                             Tag());                  // RETURN
      }
      case bdlat_FormattingMode::e_TEXT: {
        return TypesParserUtil_Imp::parseText(result,
                                              input,
                                              inputLength,
                                              Tag());                 // RETURN
      }
      default: {
        BSLS_ASSERT_SAFE(!"Unsupported operation!");

        return k_FAILURE;                                             // RETURN
      }
    }
}

template <class TYPE>
inline
int TypesParserUtil::parseBase64(TYPE       *result,
                                 const char *input,
                                 int         inputLength)
{
    typedef typename bdlat_TypeCategory::Select<TYPE>::Type Tag;

    return TypesParserUtil_Imp::parseBase64(result, input, inputLength, Tag());
}

template <class TYPE>
inline
int TypesParserUtil::parseDecimal(TYPE       *result,
                                  const char *input,
                                  int         inputLength)
{
    typedef typename bdlat_TypeCategory::Select<TYPE>::Type Tag;

    return TypesParserUtil_Imp::parseDecimal(result,
                                             input,
                                             inputLength,
                                             Tag());
}

template <class TYPE>
inline
int TypesParserUtil::parseDefault(TYPE       *result,
                                  const char *input,
                                  int         inputLength)
{
    typedef typename bdlat_TypeCategory::Select<TYPE>::Type Tag;

    return TypesParserUtil_Imp::parseDefault(result,
                                             input,
                                             inputLength,
                                             Tag());
}

template <class TYPE>
inline
int TypesParserUtil::parseHex(TYPE       *result,
                              const char *input,
                              int         inputLength)
{
    typedef typename bdlat_TypeCategory::Select<TYPE>::Type Tag;

    return TypesParserUtil_Imp::parseHex(result, input, inputLength, Tag());
}

template <class TYPE>
inline
int TypesParserUtil::parseList(TYPE       *result,
                               const char *input,
                               int         inputLength)
{
    typedef typename bdlat_TypeCategory::Select<TYPE>::Type Tag;

    return TypesParserUtil_Imp::parseList(result, input, inputLength, Tag());
}

template <class TYPE>
inline
int TypesParserUtil::parseText(TYPE       *result,
                               const char *input,
                               int         inputLength)
{
    typedef typename bdlat_TypeCategory::Select<TYPE>::Type Tag;

    return TypesParserUtil_Imp::parseText(result, input, inputLength, Tag());
}

                         // --------------------------
                         // struct TypesParserUtil_Imp
                         // --------------------------

// BASE64 FUNCTIONS

template <class TYPE>
inline
int TypesParserUtil_Imp::parseBase64(
                                  TYPE                            *result,
                                  const char                      *input,
                                  int                              inputLength,
                                  bdlat_TypeCategory::DynamicType)
{
    TypesParserUtil_Imp_parseBase64Proxy proxy = { input, inputLength };

    return bdlat_TypeCategoryUtil::manipulateByCategory(result, proxy);
}

template <class TYPE, class ANY_CATEGORY>
inline
int TypesParserUtil_Imp::parseBase64(TYPE *, const char *, int, ANY_CATEGORY)
{
    enum { k_FAILURE = -1 };

    BSLS_ASSERT_SAFE(!"Unsupported operation!");

    // Note: 'parseBase64' for 'bsl::string' and 'bsl::vector<char>' is inside
    //       the CPP file.

    return k_FAILURE;
}

// DECIMAL FUNCTIONS

template <class TYPE>
inline
int TypesParserUtil_Imp::parseDecimal(
                                  TYPE                            *result,
                                  const char                      *input,
                                  int                              inputLength,
                                  bdlat_TypeCategory::Enumeration)
{
    enum { k_FAILURE = -1 };

    int intValue;

    if (0 != TypesParserUtil::parseDecimal(&intValue, input, inputLength)) {
        return k_FAILURE;                                             // RETURN
    }

    return bdlat_EnumFunctions::fromInt(result, intValue);
}

template <class TYPE>
inline
int TypesParserUtil_Imp::parseDecimal(
                               TYPE                               *result,
                               const char                         *input,
                               int                                 inputLength,
                               bdlat_TypeCategory::CustomizedType)
{
    enum { k_FAILURE = -1 };

    typedef typename
    bdlat_CustomizedTypeFunctions::BaseType<TYPE>::Type BaseType;

    BaseType base;

    if (0 != TypesParserUtil::parseDecimal(&base, input, inputLength)) {
        return k_FAILURE;                                             // RETURN
    }

    return bdlat_CustomizedTypeFunctions::convertFromBaseType(result, base);
}

template <class TYPE>
inline
int TypesParserUtil_Imp::parseDecimal(
                                  TYPE                            *result,
                                  const char                      *input,
                                  int                              inputLength,
                                  bdlat_TypeCategory::DynamicType)
{
    TypesParserUtil_Imp_parseDecimalProxy proxy = { input, inputLength };

    return bdlat_TypeCategoryUtil::manipulateByCategory(result, proxy);
}

template <class TYPE, class ANY_CATEGORY>
inline
int TypesParserUtil_Imp::parseDecimal(TYPE *, const char *, int, ANY_CATEGORY)
{
    enum { k_FAILURE = -1 };

    BSLS_ASSERT_SAFE(!"Unsupported operation!");

    return k_FAILURE;
}

// DEFAULT FUNCTIONS

template <class TYPE>
inline
int TypesParserUtil_Imp::parseDefault(
                                  TYPE                            *result,
                                  const char                      *input,
                                  int                              inputLength,
                                  bdlat_TypeCategory::Enumeration)
{
    return bdlat_EnumFunctions::fromString(result, input, inputLength);
}

template <class TYPE>
inline
int TypesParserUtil_Imp::parseDefault(
                               TYPE                               *result,
                               const char                         *input,
                               int                                 inputLength,
                               bdlat_TypeCategory::CustomizedType)
{
    enum { k_FAILURE = -1 };

    typedef typename
    bdlat_CustomizedTypeFunctions::BaseType<TYPE>::Type BaseType;

    BaseType base;

    if (0 != TypesParserUtil::parseDefault(&base, input, inputLength)) {
        return k_FAILURE;                                             // RETURN
    }

    return bdlat_CustomizedTypeFunctions::convertFromBaseType(result, base);
}

template <class TYPE>
inline
int TypesParserUtil_Imp::parseDefault(
                                  TYPE                            *result,
                                  const char                      *input,
                                  int                              inputLength,
                                  bdlat_TypeCategory::DynamicType)
{
    TypesParserUtil_Imp_parseDefaultProxy proxy = { input, inputLength };

    return bdlat_TypeCategoryUtil::manipulateByCategory(result, proxy);
}

template <class TYPE, class ANY_CATEGORY>
inline
int TypesParserUtil_Imp::parseDefault(TYPE *, const char *, int, ANY_CATEGORY)
{
    enum { k_FAILURE = -1 };

    BSLS_ASSERT_SAFE(!"Unsupported operation!");

    return k_FAILURE;
}

inline
int TypesParserUtil_Imp::parseDefault(char                       *result,
                                      const char                 *input,
                                      int                         inputLength,
                                      bdlat_TypeCategory::Simple)
{
    return parseDecimal(result,
                        input,
                        inputLength,
                        bdlat_TypeCategory::Simple());
}

inline
int TypesParserUtil_Imp::parseDefault(short                      *result,
                                      const char                 *input,
                                      int                         inputLength,
                                      bdlat_TypeCategory::Simple)
{
    return parseDecimal(result,
                        input,
                        inputLength,
                        bdlat_TypeCategory::Simple());
}

inline
int TypesParserUtil_Imp::parseDefault(int                        *result,
                                      const char                 *input,
                                      int                         inputLength,
                                      bdlat_TypeCategory::Simple)
{
    return parseDecimal(result,
                        input,
                        inputLength,
                        bdlat_TypeCategory::Simple());
}

inline
int TypesParserUtil_Imp::parseDefault(bsls::Types::Int64         *result,
                                      const char                 *input,
                                      int                         inputLength,
                                      bdlat_TypeCategory::Simple)
{
    return parseDecimal(result,
                        input,
                        inputLength,
                        bdlat_TypeCategory::Simple());
}

inline
int TypesParserUtil_Imp::parseDefault(unsigned char              *result,
                                      const char                 *input,
                                      int                         inputLength,
                                      bdlat_TypeCategory::Simple)
{
    return parseDecimal(result,
                        input,
                        inputLength,
                        bdlat_TypeCategory::Simple());
}

inline
int TypesParserUtil_Imp::parseDefault(unsigned short             *result,
                                      const char                 *input,
                                      int                         inputLength,
                                      bdlat_TypeCategory::Simple)
{
    return parseDecimal(result,
                        input,
                        inputLength,
                        bdlat_TypeCategory::Simple());
}

inline
int TypesParserUtil_Imp::parseDefault(unsigned int               *result,
                                      const char                 *input,
                                      int                         inputLength,
                                      bdlat_TypeCategory::Simple)
{
    return parseDecimal(result,
                        input,
                        inputLength,
                        bdlat_TypeCategory::Simple());
}

inline
int TypesParserUtil_Imp::parseDefault(bsls::Types::Uint64        *result,
                                      const char                 *input,
                                      int                         inputLength,
                                      bdlat_TypeCategory::Simple)
{
    return parseDecimal(result,
                        input,
                        inputLength,
                        bdlat_TypeCategory::Simple());
}

inline
int TypesParserUtil_Imp::parseDefault(bsl::string                *result,
                                      const char                 *input,
                                      int                         inputLength,
                                      bdlat_TypeCategory::Simple)
{
    return parseText(result,
                     input,
                     inputLength,
                     bdlat_TypeCategory::Simple());
}

inline
int TypesParserUtil_Imp::parseDefault(bdlt::Date                 *result,
                                      const char                 *input,
                                      int                         inputLength,
                                      bdlat_TypeCategory::Simple)
{
    return bdlt::Iso8601Util::parse(result, input, inputLength);
}

inline
int TypesParserUtil_Imp::parseDefault(bdlt::DateTz               *result,
                                      const char                 *input,
                                      int                         inputLength,
                                      bdlat_TypeCategory::Simple)
{
    return bdlt::Iso8601Util::parse(result, input, inputLength);
}

inline
int TypesParserUtil_Imp::parseDefault(bdlt::Datetime             *result,
                                      const char                 *input,
                                      int                         inputLength,
                                      bdlat_TypeCategory::Simple)
{
    return bdlt::Iso8601Util::parse(result, input, inputLength);
}

inline
int TypesParserUtil_Imp::parseDefault(bdlt::DatetimeTz           *result,
                                      const char                 *input,
                                      int                         inputLength,
                                      bdlat_TypeCategory::Simple)
{
    return bdlt::Iso8601Util::parse(result, input, inputLength);
}

inline
int TypesParserUtil_Imp::parseDefault(bdlt::Time                 *result,
                                      const char                 *input,
                                      int                         inputLength,
                                      bdlat_TypeCategory::Simple)
{
    return bdlt::Iso8601Util::parse(result, input, inputLength);
}

inline
int TypesParserUtil_Imp::parseDefault(bdlt::TimeTz               *result,
                                      const char                 *input,
                                      int                         inputLength,
                                      bdlat_TypeCategory::Simple)
{
    return bdlt::Iso8601Util::parse(result, input, inputLength);
}

inline
int TypesParserUtil_Imp::parseDefault(bsl::vector<char>         *result,
                                      const char                *input,
                                      int                        inputLength,
                                      bdlat_TypeCategory::Array)
{
    return parseBase64(result,
                       input,
                       inputLength,
                       bdlat_TypeCategory::Array());
}

// HEX FUNCTIONS

template <class TYPE>
inline
int TypesParserUtil_Imp::parseHex(TYPE                            *result,
                                  const char                      *input,
                                  int                              inputLength,
                                  bdlat_TypeCategory::DynamicType)
{
    TypesParserUtil_Imp_parseHexProxy proxy = { input, inputLength };

    return bdlat_TypeCategoryUtil::manipulateByCategory(result, proxy);
}

template <class TYPE, class ANY_CATEGORY>
inline
int TypesParserUtil_Imp::parseHex(TYPE *, const char *, int, ANY_CATEGORY)
{
    enum { k_FAILURE = -1 };

    BSLS_ASSERT_SAFE(!"Unsupported operation!");

    // Note: 'parseHex' for 'bsl::string' and 'bsl::vector<char>' is inside the
    //       CPP file.

    return k_FAILURE;
}

// LIST FUNCTIONS

template <class TYPE>
int TypesParserUtil_Imp::parseList(TYPE                      *result,
                                   const char                *input,
                                   int                        inputLength,
                                   bdlat_TypeCategory::Array)
{
    enum { k_FAILURE = -1 };

    typedef balxml::ListParser<TYPE> ListParser;

    typename ListParser::ParseElementFunction fn =
        &TypesParserUtil::parseDefault;

    ListParser listParser(fn);

    if (0 != listParser.beginParse(result)) {
        return k_FAILURE;                                             // RETURN
    }

    if (0 != listParser.pushCharacters(input, input + inputLength)) {
        return k_FAILURE;                                             // RETURN
    }

    return listParser.endParse();
}

template <class TYPE>
inline
int TypesParserUtil_Imp::parseList(
                                  TYPE                            *result,
                                  const char                      *input,
                                  int                              inputLength,
                                  bdlat_TypeCategory::DynamicType)
{
    TypesParserUtil_Imp_parseListProxy proxy = { input, inputLength };

    return bdlat_TypeCategoryUtil::manipulateByCategory(result, proxy);
}

template <class TYPE, class ANY_CATEGORY>
inline
int TypesParserUtil_Imp::parseList(TYPE *, const char *, int, ANY_CATEGORY)
{
    enum { k_FAILURE = -1 };

    BSLS_ASSERT_SAFE(!"Unsupported operation!");

    return k_FAILURE;
}

// TEXT FUNCTIONS

template <class TYPE>
inline
int TypesParserUtil_Imp::parseText(
                                  TYPE                            *result,
                                  const char                      *input,
                                  int                              inputLength,
                                  bdlat_TypeCategory::Enumeration)
{
    return bdlat_EnumFunctions::fromString(result, input, inputLength);
}

template <class TYPE>
inline
int TypesParserUtil_Imp::parseText(
                               TYPE                               *result,
                               const char                         *input,
                               int                                 inputLength,
                               bdlat_TypeCategory::CustomizedType)
{
    enum { k_FAILURE = -1 };

    typedef typename
    bdlat_CustomizedTypeFunctions::BaseType<TYPE>::Type BaseType;

    BaseType base;

    if (0 != TypesParserUtil::parseText(&base, input, inputLength)) {
        return k_FAILURE;                                             // RETURN
    }

    return bdlat_CustomizedTypeFunctions::convertFromBaseType(result, base);
}

template <class TYPE>
inline
int TypesParserUtil_Imp::parseText(
                                  TYPE                            *result,
                                  const char                      *input,
                                  int                              inputLength,
                                  bdlat_TypeCategory::DynamicType)
{
    TypesParserUtil_Imp_parseTextProxy proxy = { input, inputLength };

    return bdlat_TypeCategoryUtil::manipulateByCategory(result, proxy);
}

template <class TYPE, class ANY_CATEGORY>
inline
int TypesParserUtil_Imp::parseText(TYPE *, const char *, int, ANY_CATEGORY)
{
    enum { k_FAILURE = -1 };

    BSLS_ASSERT_SAFE(!"Unsupported operation!");

    return k_FAILURE;
}

}  // close package namespace
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
