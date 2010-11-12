// baexml_typesparserutil.h   -*-C++-*-
#ifndef INCLUDED_BAEXML_TYPESPARSERUTIL
#define INCLUDED_BAEXML_TYPESPARSERUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a utility for parsing types using XML formatting.
//
//@CLASSES: baexml_TypesParserUtil: utility for parsing using XML formatting.
//
//@SEE_ALSO: http://www.w3.org/TR/xmlschema-2/
//
//@AUTHOR: Shezan Baig (sbaig)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION: The 'baexml_TypesParserUtil' struct provided by this component
// contains the following functions:
//..
//  o 'parse':        Parse a string using the supplied formatting mode.
//  o 'parseBase64':  Parse a string using
//                    'bdeat_FormattingMode::BDEAT_BASE64'.
//  o 'parseDecimal': Parse a string using 'bdeat_FormattingMode::BDEAT_DEC'.
//  o 'parseDefault': Parse a string using
//                    'bdeat_FormattingMode::BDEAT_DEFAULT'.
//  o 'parseHex':     Parse a string using 'bdeat_FormattingMode::BDEAT_HEX'.
//  o 'parseList':    Parse a string using
//                    'bdeat_FormattingMode::BDEAT_IS_LIST'.
//  o 'parseText':    Parse a string using 'bdeat_FormattingMode::BDEAT_TEXT'.
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
//      bsls_PlatformUtil::[Uint64|Int64]   DEFAULT, DEC
//      float                               DEFAULT, DEC
//      double                              DEFAULT, DEC
//      bsl::string                         DEFAULT, TEXT, BASE64, HEX
//      bdet_Date                           DEFAULT
//      bdet_DateTz                         DEFAULT
//      bdet_Datetime                       DEFAULT
//      bdet_DateTimeTz                     DEFAULT
//      bdet_Time                           DEFAULT
//      bdet_TimeTz                         DEFAULT
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
// When 'bdeat_FormattingMode::BDEAT_DEFAULT' is used, the actual formatting
// mode selected is based on the following mapping:
//..
//      C++ Type                            Default Formatting Mode
//      --------                            -----------------------
//      bool                                DEC or TEXT
//      [unsigned] char                     DEC
//      [unsigned] short                    DEC
//      [unsigned] int                      DEC
//      [unsigned] long                     DEC
//      bsls_PlatformUtil::[Uint64|Int64]   DEC
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
//  #include <baexml_typesparserutil.h>
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
//      int retCode = baexml_TypesParserUtil::parseBase64(&vec,
//                                                        INPUT,
//                                                        INPUT_LENGTH);
//
//      assert(0   == retCode);
//      assert(4   == vec.size());
//      assert('a' == vec[0]);
//      assert('b' == vec[1]);
//      assert('c' == vec[2]);
//      assert('d' == vec[3]);
//  }
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEXML_LISTPARSER
#include <baexml_listparser.h>
#endif

#ifndef INCLUDED_BDEAT_CUSTOMIZEDTYPEFUNCTIONS
#include <bdeat_customizedtypefunctions.h>
#endif

#ifndef INCLUDED_BDEAT_ENUMFUNCTIONS
#include <bdeat_enumfunctions.h>
#endif

#ifndef INCLUDED_BDEAT_FORMATTINGMODE
#include <bdeat_formattingmode.h>
#endif

#ifndef INCLUDED_BDEAT_TYPECATEGORY
#include <bdeat_typecategory.h>
#endif

#ifndef INCLUDED_BDEPU_ISO8601
#include <bdepu_iso8601.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

class bdet_Date;
class bdet_DateTz;
class bdet_Datetime;
class bdet_DatetimeTz;
class bdet_Time;
class bdet_TimeTz;

                           // =============================
                           // struct baexml_TypesParserUtil
                           // =============================

struct baexml_TypesParserUtil {
    // This 'struct' contains functions for parsing input strings using various
    // XML formatting modes.

    template <typename TYPE>
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

    template <typename TYPE>
    static int parseBase64(TYPE       *result,
                           const char *input,
                           int         inputLength);
        // Parse the specified 'input' of the specified 'inputLength' using
        // 'bdeat_FormattingMode::BDEAT_BASE64' and load the result into the
        // specified 'result'.  Return 0 on success, and a non-zero value
        // otherwise.

    template <typename TYPE>
    static int parseDecimal(TYPE       *result,
                            const char *input,
                            int         inputLength);
        // Parse the specified 'input' of the specified 'inputLength' using
        // 'bdeat_FormattingMode::BDEAT_DEC' and load the result into the
        // specified 'result'.  Return 0 on success, and a non-zero value
        // otherwise.

    template <typename TYPE>
    static int parseDefault(TYPE       *result,
                            const char *input,
                            int         inputLength);
        // Parse the specified 'input' of the specified 'inputLength' using
        // 'bdeat_FormattingMode::BDEAT_DEFAULT' and load the result into the
        // specified 'result'.  Return 0 on success, and a non-zero value
        // otherwise.

    template <typename TYPE>
    static int parseHex(TYPE       *result,
                        const char *input,
                        int         inputLength);
        // Parse the specified 'input' of the specified 'inputLength' using
        // 'bdeat_FormattingMode::BDEAT_HEX' and load the result into the
        // specified 'result'.  Return 0 on success, and a non-zero value
        // otherwise.

    template <typename TYPE>
    static int parseList(TYPE       *result,
                         const char *input,
                         int         inputLength);
        // Parse the specified 'input' of the specified 'inputLength' using
        // 'bdeat_FormattingMode::BDEAT_LIST' and load the result into the
        // specified 'result'.  Return 0 on success, and a non-zero value
        // otherwise.

    template <typename TYPE>
    static int parseText(TYPE       *result,
                         const char *input,
                         int         inputLength);
        // Parse the specified 'input' of the specified 'inputLength' using
        // 'bdeat_FormattingMode::BDEAT_TEXT' and load the result into the
        // specified 'result'.  Return 0 on success, and a non-zero value
        // otherwise.
};

                         // =================================
                         // struct baexml_TypesParserUtil_Imp
                         // =================================

struct baexml_TypesParserUtil_Imp {
    // This 'struct' contains functions that are used in the implementation of
    // this component.

    // BASE64 FUNCTIONS
    template <typename TYPE>
    static int parseBase64(TYPE       *result,
                           const char *input,
                           int         inputLength,
                           bdeat_TypeCategory::DynamicType);

    template <typename TYPE, typename ANY_CATEGORY>
    static int parseBase64(TYPE       *result,
                           const char *input,
                           int         inputLength,
                           ANY_CATEGORY);

    static int parseBase64(bsl::string *result,
                           const char  *input,
                           int          inputLength,
                           bdeat_TypeCategory::Simple);
    static int parseBase64(bsl::vector<char> *result,
                           const char        *input,
                           int                inputLength,
                           bdeat_TypeCategory::Array);

    // DECIMAL FUNCTIONS
    template <typename TYPE>
    static int parseDecimal(TYPE       *result,
                            const char *input,
                            int         inputLength,
                            bdeat_TypeCategory::Enumeration);

    template <typename TYPE>
    static int parseDecimal(TYPE       *result,
                            const char *input,
                            int         inputLength,
                            bdeat_TypeCategory::CustomizedType);

    template <typename TYPE>
    static int parseDecimal(TYPE       *result,
                            const char *input,
                            int         inputLength,
                            bdeat_TypeCategory::DynamicType);

    template <typename TYPE, typename ANY_CATEGORY>
    static int parseDecimal(TYPE       *result,
                            const char *input,
                            int         inputLength,
                            ANY_CATEGORY);

    static int parseDecimal(bool       *result,
                            const char *input,
                            int         inputLength,
                            bdeat_TypeCategory::Simple);
    static int parseDecimal(char       *result,
                            const char *input,
                            int         inputLength,
                            bdeat_TypeCategory::Simple);
    static int parseDecimal(short      *result,
                            const char *input,
                            int         inputLength,
                            bdeat_TypeCategory::Simple);
    static int parseDecimal(int        *result,
                            const char *input,
                            int         inputLength,
                            bdeat_TypeCategory::Simple);
    static int parseDecimal(bsls_PlatformUtil::Int64 *result,
                            const char               *input,
                            int                       inputLength,
                            bdeat_TypeCategory::Simple);
    static int parseDecimal(unsigned char *result,
                            const char    *input,
                            int            inputLength,
                            bdeat_TypeCategory::Simple);
    static int parseDecimal(unsigned short *result,
                            const char     *input,
                            int             inputLength,
                            bdeat_TypeCategory::Simple);
    static int parseDecimal(unsigned int *result,
                            const char   *input,
                            int           inputLength,
                            bdeat_TypeCategory::Simple);
    static int parseDecimal(bsls_PlatformUtil::Uint64 *result,
                            const char                *input,
                            int                        inputLength,
                            bdeat_TypeCategory::Simple);

    static int parseDecimal(float       *result,
                            const char  *input,
                            int          inputLength,
                            bdeat_TypeCategory::Simple);
    static int parseDecimal(double      *result,
                            const char  *input,
                            int          inputLength,
                            bdeat_TypeCategory::Simple);

    // DEFAULT FUNCTIONS
    template <typename TYPE>
    static int parseDefault(TYPE       *result,
                            const char *input,
                            int         inputLength,
                            bdeat_TypeCategory::Enumeration);

    template <typename TYPE>
    static int parseDefault(TYPE       *result,
                            const char *input,
                            int         inputLength,
                            bdeat_TypeCategory::CustomizedType);

    template <typename TYPE>
    static int parseDefault(TYPE       *result,
                            const char *input,
                            int         inputLength,
                            bdeat_TypeCategory::DynamicType);

    template <typename TYPE, typename ANY_CATEGORY>
    static int parseDefault(TYPE       *result,
                            const char *input,
                            int         inputLength,
                            ANY_CATEGORY);

    static int parseDefault(bool       *result,
                            const char *input,
                            int         inputLength,
                            bdeat_TypeCategory::Simple);
    static int parseDefault(char       *result,
                            const char *input,
                            int         inputLength,
                            bdeat_TypeCategory::Simple);
    static int parseDefault(short      *result,
                            const char *input,
                            int         inputLength,
                            bdeat_TypeCategory::Simple);
    static int parseDefault(int        *result,
                            const char *input,
                            int         inputLength,
                            bdeat_TypeCategory::Simple);
    static int parseDefault(bsls_PlatformUtil::Int64 *result,
                            const char               *input,
                            int                       inputLength,
                            bdeat_TypeCategory::Simple);
    static int parseDefault(unsigned char *result,
                            const char    *input,
                            int            inputLength,
                            bdeat_TypeCategory::Simple);
    static int parseDefault(unsigned short *result,
                            const char     *input,
                            int             inputLength,
                            bdeat_TypeCategory::Simple);
    static int parseDefault(unsigned int *result,
                            const char   *input,
                            int           inputLength,
                            bdeat_TypeCategory::Simple);
    static int parseDefault(bsls_PlatformUtil::Uint64 *result,
                            const char                *input,
                            int                        inputLength,
                            bdeat_TypeCategory::Simple);
    static int parseDefault(float      *result,
                            const char *input,
                            int         inputLength,
                            bdeat_TypeCategory::Simple);
    static int parseDefault(double     *result,
                            const char *input,
                            int         inputLength,
                            bdeat_TypeCategory::Simple);
    static int parseDefault(bsl::string *result,
                            const char  *input,
                            int          inputLength,
                            bdeat_TypeCategory::Simple);
    static int parseDefault(bdet_Date  *result,
                            const char *input,
                            int         inputLength,
                            bdeat_TypeCategory::Simple);
    static int parseDefault(bdet_DateTz *result,
                            const char  *input,
                            int          inputLength,
                            bdeat_TypeCategory::Simple);
    static int parseDefault(bdet_Datetime *result,
                            const char    *input,
                            int            inputLength,
                            bdeat_TypeCategory::Simple);
    static int parseDefault(bdet_DatetimeTz *result,
                            const char      *input,
                            int              inputLength,
                            bdeat_TypeCategory::Simple);
    static int parseDefault(bdet_Time  *result,
                            const char *input,
                            int         inputLength,
                            bdeat_TypeCategory::Simple);
    static int parseDefault(bdet_TimeTz *result,
                            const char  *input,
                            int          inputLength,
                            bdeat_TypeCategory::Simple);
    static int parseDefault(bsl::vector<char> *result,
                            const char        *input,
                            int                inputLength,
                            bdeat_TypeCategory::Array);

    // HEX FUNCTIONS
    template <typename TYPE>
    static int parseHex(TYPE       *result,
                        const char *input,
                        int         inputLength,
                        bdeat_TypeCategory::DynamicType);

    template <typename TYPE, typename ANY_CATEGORY>
    static int parseHex(TYPE       *result,
                        const char *input,
                        int         inputLength,
                        ANY_CATEGORY);

    static int parseHex(bsl::string *result,
                        const char  *input,
                        int          inputLength,
                        bdeat_TypeCategory::Simple);
    static int parseHex(bsl::vector<char> *result,
                        const char        *input,
                        int                inputLength,
                        bdeat_TypeCategory::Array);

    // LIST FUNCTIONS
    template <typename TYPE>
    static int parseList(TYPE       *result,
                         const char *input,
                         int         inputLength,
                         bdeat_TypeCategory::Array);

    template <typename TYPE>
    static int parseList(TYPE       *result,
                         const char *input,
                         int         inputLength,
                         bdeat_TypeCategory::DynamicType);

    template <typename TYPE, typename ANY_CATEGORY>
    static int parseList(TYPE       *result,
                         const char *input,
                         int         inputLength,
                         ANY_CATEGORY);

    // TEXT FUNCTIONS
    template <typename TYPE>
    static int parseText(TYPE       *result,
                         const char *input,
                         int         inputLength,
                         bdeat_TypeCategory::Enumeration);

    template <typename TYPE>
    static int parseText(TYPE       *result,
                         const char *input,
                         int         inputLength,
                         bdeat_TypeCategory::CustomizedType);

    template <typename TYPE>
    static int parseText(TYPE       *result,
                         const char *input,
                         int         inputLength,
                         bdeat_TypeCategory::DynamicType);

    template <typename TYPE, typename ANY_CATEGORY>
    static int parseText(TYPE       *result,
                         const char *input,
                         int         inputLength,
                         ANY_CATEGORY);

    static int parseText(bool       *result,
                         const char *input,
                         int         inputLength,
                         bdeat_TypeCategory::Simple);
    static int parseText(char       *result,
                         const char *input,
                         int         inputLength,
                         bdeat_TypeCategory::Simple);
    static int parseText(bsl::string *result,
                         const char  *input,
                         int          inputLength,
                         bdeat_TypeCategory::Simple);
    static int parseText(bsl::vector<char> *result,
                         const char        *input,
                         int                inputLength,
                         bdeat_TypeCategory::Array);

};

// ============================================================================
//                               PROXY CLASSES
// ============================================================================

                // ==================================================
                // struct baexml_TypesParserUtil_Imp_parseBase64Proxy
                // ==================================================

struct baexml_TypesParserUtil_Imp_parseBase64Proxy {
    // Component-private struct.  Do not use.

    // DATA MEMBERS
    const char *d_input_p;
    int         d_inputLength;

    // CREATORS
    // Creators have been omitted to allow simple static initialization of
    // this struct.

    // FUNCTIONS
    template <typename TYPE>
    inline
    int operator()(TYPE *object, bslmf_Nil)
    {
        BSLS_ASSERT_SAFE(0);
        (void) object;
        return -1;
    }

    template <typename TYPE, typename ANY_CATEGORY>
    inline
    int operator()(TYPE *object, ANY_CATEGORY category)
    {
        return baexml_TypesParserUtil_Imp::parseBase64(object,
                                                d_input_p,
                                                d_inputLength,
                                                category);
    }
};

                // ===================================================
                // struct baexml_TypesParserUtil_Imp_parseDecimalProxy
                // ===================================================

struct baexml_TypesParserUtil_Imp_parseDecimalProxy {
    // Component-private struct.  Do not use.

    // DATA MEMBERS
    const char *d_input_p;
    int         d_inputLength;

    // CREATORS
    // Creators have been omitted to allow simple static initialization of
    // this struct.

    // FUNCTIONS
    template <typename TYPE>
    inline
    int operator()(TYPE *object, bslmf_Nil)
    {
        BSLS_ASSERT_SAFE(0);
        (void) object;
        return -1;
    }

    template <typename TYPE, typename ANY_CATEGORY>
    inline
    int operator()(TYPE *object, ANY_CATEGORY category)
    {
        return baexml_TypesParserUtil_Imp::parseDecimal(object,
                                                        d_input_p,
                                                        d_inputLength,
                                                        category);
    }
};

                // ===================================================
                // struct baexml_TypesParserUtil_Imp_parseDefaultProxy
                // ===================================================

struct baexml_TypesParserUtil_Imp_parseDefaultProxy {
    // Component-private struct.  Do not use.

    // DATA MEMBERS
    const char *d_input_p;
    int         d_inputLength;

    // CREATORS
    // Creators have been omitted to allow simple static initialization of
    // this struct.

    // FUNCTIONS
    template <typename TYPE>
    inline
    int operator()(TYPE *object, bslmf_Nil)
    {
        BSLS_ASSERT_SAFE(0);
        (void) object;
        return -1;
    }

    template <typename TYPE, typename ANY_CATEGORY>
    inline
    int operator()(TYPE *object, ANY_CATEGORY category)
    {
        return baexml_TypesParserUtil_Imp::parseDefault(object,
                                                        d_input_p,
                                                        d_inputLength,
                                                        category);
    }
};

                  // ===============================================
                  // struct baexml_TypesParserUtil_Imp_parseHexProxy
                  // ===============================================

struct baexml_TypesParserUtil_Imp_parseHexProxy {
    // Component-private struct.  Do not use.

    // DATA MEMBERS
    const char *d_input_p;
    int         d_inputLength;

    // CREATORS
    // Creators have been omitted to allow simple static initialization of
    // this struct.

    // FUNCTIONS
    template <typename TYPE>
    inline
    int operator()(TYPE *object, bslmf_Nil)
    {
        BSLS_ASSERT_SAFE(0);
        (void) object;
        return -1;
    }

    template <typename TYPE, typename ANY_CATEGORY>
    inline
    int operator()(TYPE *object, ANY_CATEGORY category)
    {
        return baexml_TypesParserUtil_Imp::parseHex(object,
                                                    d_input_p,
                                                    d_inputLength,
                                                    category);
    }
};

                 // ================================================
                 // struct baexml_TypesParserUtil_Imp_parseListProxy
                 // ================================================

struct baexml_TypesParserUtil_Imp_parseListProxy {
    // Component-private struct.  Do not use.

    // DATA MEMBERS
    const char *d_input_p;
    int         d_inputLength;

    // CREATORS
    // Creators have been omitted to allow simple static initialization of
    // this struct.

    // FUNCTIONS
    template <typename TYPE>
    inline
    int operator()(TYPE *object, bslmf_Nil)
    {
        BSLS_ASSERT_SAFE(0);
        (void) object;
        return -1;
    }

    template <typename TYPE, typename ANY_CATEGORY>
    inline
    int operator()(TYPE *object, ANY_CATEGORY category)
    {
        return baexml_TypesParserUtil_Imp::parseList(object,
                                                     d_input_p,
                                                     d_inputLength,
                                                     category);
    }
};

                 // ================================================
                 // struct baexml_TypesParserUtil_Imp_parseTextProxy
                 // ================================================

struct baexml_TypesParserUtil_Imp_parseTextProxy {
    // Component-private struct.  Do not use.

    // DATA MEMBERS
    const char *d_input_p;
    int         d_inputLength;

    // CREATORS
    // Creators have been omitted to allow simple static initialization of
    // this struct.

    // FUNCTIONS
    template <typename TYPE>
    inline
    int operator()(TYPE *object, bslmf_Nil)
    {
        BSLS_ASSERT_SAFE(0);
        (void) object;
        return -1;
    }

    template <typename TYPE, typename ANY_CATEGORY>
    inline
    int operator()(TYPE *object, ANY_CATEGORY category)
    {
        return baexml_TypesParserUtil_Imp::parseText(object,
                                                     d_input_p,
                                                     d_inputLength,
                                                     category);
    }
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                           // -----------------------------
                           // struct baexml_TypesParserUtil
                           // -----------------------------

template <typename TYPE>
int baexml_TypesParserUtil::parse(TYPE       *result,
                                  const char *input,
                                  int         inputLength,
                                  int         formattingMode)
{
    enum { BAEXML_FAILURE = - 1 };

    typedef typename bdeat_TypeCategory::Select<TYPE>::Type Tag;

    if (formattingMode & bdeat_FormattingMode::BDEAT_LIST) {
        return baexml_TypesParserUtil_Imp::parseList(result, input,
                                                     inputLength, Tag());
    }

    switch (formattingMode & bdeat_FormattingMode::BDEAT_TYPE_MASK) {
      case bdeat_FormattingMode::BDEAT_BASE64: {
        return baexml_TypesParserUtil_Imp::parseBase64(result,
                                                       input,
                                                       inputLength,
                                                       Tag());
      }
      case bdeat_FormattingMode::BDEAT_DEC: {
        return baexml_TypesParserUtil_Imp::parseDecimal(result,
                                                        input,
                                                        inputLength,
                                                        Tag());
      }
      case bdeat_FormattingMode::BDEAT_DEFAULT: {
        return baexml_TypesParserUtil_Imp::parseDefault(result,
                                                        input,
                                                        inputLength,
                                                        Tag());
      }
      case bdeat_FormattingMode::BDEAT_HEX: {
        return baexml_TypesParserUtil_Imp::parseHex(result,
                                                    input,
                                                    inputLength,
                                                    Tag());
      }
      case bdeat_FormattingMode::BDEAT_TEXT: {
        return baexml_TypesParserUtil_Imp::parseText(result,
                                                     input,
                                                     inputLength,
                                                     Tag());
      }
      default: {
        BSLS_ASSERT_SAFE(!"Unsupported operation!");

        return BAEXML_FAILURE;
      }
    }
}

template <typename TYPE>
inline
int baexml_TypesParserUtil::parseBase64(TYPE       *result,
                                        const char *input,
                                        int         inputLength)
{
    typedef typename bdeat_TypeCategory::Select<TYPE>::Type Tag;

    return baexml_TypesParserUtil_Imp::parseBase64(result, input,
                                                   inputLength, Tag());
}

template <typename TYPE>
inline
int baexml_TypesParserUtil::parseDecimal(TYPE       *result,
                                         const char *input,
                                         int         inputLength)
{
    typedef typename bdeat_TypeCategory::Select<TYPE>::Type Tag;

    return baexml_TypesParserUtil_Imp::parseDecimal(result, input,
                                                    inputLength, Tag());
}

template <typename TYPE>
inline
int baexml_TypesParserUtil::parseDefault(TYPE       *result,
                                         const char *input,
                                         int         inputLength)
{
    typedef typename bdeat_TypeCategory::Select<TYPE>::Type Tag;

    return baexml_TypesParserUtil_Imp::parseDefault(result, input,
                                                    inputLength, Tag());
}

template <typename TYPE>
inline
int baexml_TypesParserUtil::parseHex(TYPE       *result,
                                     const char *input,
                                     int         inputLength)
{
    typedef typename bdeat_TypeCategory::Select<TYPE>::Type Tag;

    return baexml_TypesParserUtil_Imp::parseHex(result, input,
                                                inputLength, Tag());
}

template <typename TYPE>
inline
int baexml_TypesParserUtil::parseList(TYPE       *result,
                                      const char *input,
                                      int         inputLength)
{
    typedef typename bdeat_TypeCategory::Select<TYPE>::Type Tag;

    return baexml_TypesParserUtil_Imp::parseList(result, input,
                                                 inputLength, Tag());
}

template <typename TYPE>
inline
int baexml_TypesParserUtil::parseText(TYPE       *result,
                                      const char *input,
                                      int         inputLength)
{
    typedef typename bdeat_TypeCategory::Select<TYPE>::Type Tag;

    return baexml_TypesParserUtil_Imp::parseText(result, input,
                                                 inputLength, Tag());
}

                         // ---------------------------------
                         // struct baexml_TypesParserUtil_Imp
                         // ---------------------------------

// BASE64 FUNCTIONS

template <typename TYPE>
inline
int baexml_TypesParserUtil_Imp::parseBase64(TYPE       *result,
                                            const char *input,
                                            int         inputLength,
                                            bdeat_TypeCategory::DynamicType)
{
    baexml_TypesParserUtil_Imp_parseBase64Proxy proxy = { input, inputLength };

    return bdeat_TypeCategoryUtil::manipulateByCategory(result, proxy);
}

template <typename TYPE, typename ANY_CATEGORY>
inline
int baexml_TypesParserUtil_Imp::parseBase64(TYPE       *result,
                                            const char *input,
                                            int         inputLength,
                                            ANY_CATEGORY)
{
    enum { BAEXML_FAILURE = -1 };

    BSLS_ASSERT_SAFE(!"Unsupported operation!");

    (void) result;
    (void) input;
    (void) inputLength;

    // Note: 'parseBase64' for 'bsl::string' and 'bsl::vector<char>' is inside
    //       the CPP file.

    return BAEXML_FAILURE;
}

// DECIMAL FUNCTIONS

template <typename TYPE>
inline
int baexml_TypesParserUtil_Imp::parseDecimal(TYPE       *result,
                                             const char *input,
                                             int         inputLength,
                                             bdeat_TypeCategory::Enumeration)
{
    enum { BAEXML_FAILURE = -1 };

    int intValue;

    if (0 != baexml_TypesParserUtil::parseDecimal(&intValue, input,
                                                  inputLength)) {
        return BAEXML_FAILURE;
    }

    return bdeat_EnumFunctions::fromInt(result, intValue);
}

template <typename TYPE>
inline
int baexml_TypesParserUtil_Imp::parseDecimal(TYPE       *result,
                                             const char *input,
                                             int         inputLength,
                                             bdeat_TypeCategory::CustomizedType
                                            )
{
    enum { BAEXML_FAILURE = -1 };

    typedef typename
    bdeat_CustomizedTypeFunctions::BaseType<TYPE>::Type BaseType;

    BaseType base;

    if (0 != baexml_TypesParserUtil::parseDecimal(&base, input, inputLength)) {
        return BAEXML_FAILURE;
    }

    return bdeat_CustomizedTypeFunctions::convertFromBaseType(result, base);
}

template <typename TYPE>
inline
int baexml_TypesParserUtil_Imp::parseDecimal(TYPE       *result,
                                             const char *input,
                                             int         inputLength,
                                             bdeat_TypeCategory::DynamicType)
{
    baexml_TypesParserUtil_Imp_parseDecimalProxy proxy = {
        input,
        inputLength
    };

    return bdeat_TypeCategoryUtil::manipulateByCategory(result, proxy);
}

template <typename TYPE, typename ANY_CATEGORY>
inline
int baexml_TypesParserUtil_Imp::parseDecimal(TYPE       *result,
                                             const char *input,
                                             int         inputLength,
                                             ANY_CATEGORY)
{
    enum { BAEXML_FAILURE = -1 };

    BSLS_ASSERT_SAFE(!"Unsupported operation!");

    (void) result;
    (void) input;
    (void) inputLength;

    return BAEXML_FAILURE;
}

// DEFAULT FUNCTIONS

template <typename TYPE>
inline
int baexml_TypesParserUtil_Imp::parseDefault(TYPE       *result,
                                             const char *input,
                                             int         inputLength,
                                             bdeat_TypeCategory::Enumeration)
{
    return bdeat_EnumFunctions::fromString(result, input, inputLength);
}

template <typename TYPE>
inline
int baexml_TypesParserUtil_Imp::parseDefault(TYPE       *result,
                                             const char *input,
                                             int         inputLength,
                                             bdeat_TypeCategory::CustomizedType
                                            )
{
    enum { BAEXML_FAILURE = -1 };

    typedef typename
    bdeat_CustomizedTypeFunctions::BaseType<TYPE>::Type BaseType;

    BaseType base;

    if (0 != baexml_TypesParserUtil::parseDefault(&base, input, inputLength)) {
        return BAEXML_FAILURE;
    }

    return bdeat_CustomizedTypeFunctions::convertFromBaseType(result, base);
}

template <typename TYPE>
inline
int baexml_TypesParserUtil_Imp::parseDefault(TYPE       *result,
                                             const char *input,
                                             int         inputLength,
                                             bdeat_TypeCategory::DynamicType)
{
    baexml_TypesParserUtil_Imp_parseDefaultProxy proxy = {
        input,
        inputLength
    };

    return bdeat_TypeCategoryUtil::manipulateByCategory(result, proxy);
}

template <typename TYPE, typename ANY_CATEGORY>
inline
int baexml_TypesParserUtil_Imp::parseDefault(TYPE       *result,
                                             const char *input,
                                             int         inputLength,
                                             ANY_CATEGORY)
{
    enum { BAEXML_FAILURE = -1 };

    BSLS_ASSERT_SAFE(!"Unsupported operation!");

    (void) result;
    (void) input;
    (void) inputLength;

    return BAEXML_FAILURE;
}

inline
int baexml_TypesParserUtil_Imp::parseDefault(char       *result,
                                             const char *input,
                                             int         inputLength,
                                             bdeat_TypeCategory::Simple)
{
    return parseDecimal(result, input, inputLength,
                        bdeat_TypeCategory::Simple());
}

inline
int baexml_TypesParserUtil_Imp::parseDefault(short      *result,
                                             const char *input,
                                             int         inputLength,
                                             bdeat_TypeCategory::Simple)
{
    return parseDecimal(result, input, inputLength,
                        bdeat_TypeCategory::Simple());
}

inline
int baexml_TypesParserUtil_Imp::parseDefault(int        *result,
                                             const char *input,
                                             int         inputLength,
                                             bdeat_TypeCategory::Simple)
{
    return parseDecimal(result, input, inputLength,
                        bdeat_TypeCategory::Simple());
}

inline
int baexml_TypesParserUtil_Imp::parseDefault(
    bsls_PlatformUtil::Int64 *result,
    const char               *input,
    int                       inputLength,
    bdeat_TypeCategory::Simple)
{
    return parseDecimal(result, input, inputLength,
                        bdeat_TypeCategory::Simple());
}

inline
int baexml_TypesParserUtil_Imp::parseDefault(unsigned char *result,
                                             const char    *input,
                                             int            inputLength,
                                             bdeat_TypeCategory::Simple)
{
    return parseDecimal(result, input, inputLength,
                        bdeat_TypeCategory::Simple());
}

inline
int baexml_TypesParserUtil_Imp::parseDefault(unsigned short *result,
                                             const char     *input,
                                             int             inputLength,
                                             bdeat_TypeCategory::Simple)
{
    return parseDecimal(result, input, inputLength,
                        bdeat_TypeCategory::Simple());
}

inline
int baexml_TypesParserUtil_Imp::parseDefault(unsigned int *result,
                                             const char   *input,
                                             int           inputLength,
                                             bdeat_TypeCategory::Simple)
{
    return parseDecimal(result, input, inputLength,
                        bdeat_TypeCategory::Simple());
}

inline
int baexml_TypesParserUtil_Imp::parseDefault(
    bsls_PlatformUtil::Uint64 *result,
    const char                *input,
    int                        inputLength,
    bdeat_TypeCategory::Simple)
{
    return parseDecimal(result, input, inputLength,
                        bdeat_TypeCategory::Simple());
}

inline
int baexml_TypesParserUtil_Imp::parseDefault(bsl::string *result,
                                             const char  *input,
                                             int          inputLength,
                                             bdeat_TypeCategory::Simple)
{
    return parseText(result, input, inputLength,
                     bdeat_TypeCategory::Simple());
}

inline
int baexml_TypesParserUtil_Imp::parseDefault(bdet_Date  *result,
                                             const char *input,
                                             int         inputLength,
                                             bdeat_TypeCategory::Simple)
{
    return bdepu_Iso8601::parse(result, input, inputLength);
}

inline
int baexml_TypesParserUtil_Imp::parseDefault(bdet_DateTz *result,
                                             const char  *input,
                                             int          inputLength,
                                             bdeat_TypeCategory::Simple)
{
    return bdepu_Iso8601::parse(result, input, inputLength);
}

inline
int baexml_TypesParserUtil_Imp::parseDefault(bdet_Datetime *result,
                                             const char    *input,
                                             int            inputLength,
                                             bdeat_TypeCategory::Simple)
{
    return bdepu_Iso8601::parse(result, input, inputLength);
}

inline
int baexml_TypesParserUtil_Imp::parseDefault(bdet_DatetimeTz *result,
                                             const char      *input,
                                             int              inputLength,
                                             bdeat_TypeCategory::Simple)
{
    return bdepu_Iso8601::parse(result, input, inputLength);
}

inline
int baexml_TypesParserUtil_Imp::parseDefault(bdet_Time  *result,
                                             const char *input,
                                             int         inputLength,
                                             bdeat_TypeCategory::Simple)
{
    return bdepu_Iso8601::parse(result, input, inputLength);
}

inline
int baexml_TypesParserUtil_Imp::parseDefault(bdet_TimeTz *result,
                                             const char  *input,
                                             int          inputLength,
                                             bdeat_TypeCategory::Simple)
{
    return bdepu_Iso8601::parse(result, input, inputLength);
}

inline
int baexml_TypesParserUtil_Imp::parseDefault(bsl::vector<char> *result,
                                             const char        *input,
                                             int                inputLength,
                                             bdeat_TypeCategory::Array)
{
    return parseBase64(result, input, inputLength,
                       bdeat_TypeCategory::Array());
}

// HEX FUNCTIONS

template <typename TYPE>
inline
int baexml_TypesParserUtil_Imp::parseHex(TYPE       *result,
                                         const char *input,
                                         int         inputLength,
                                         bdeat_TypeCategory::DynamicType)
{
    baexml_TypesParserUtil_Imp_parseHexProxy proxy = { input, inputLength };

    return bdeat_TypeCategoryUtil::manipulateByCategory(result, proxy);
}

template <typename TYPE, typename ANY_CATEGORY>
inline
int baexml_TypesParserUtil_Imp::parseHex(TYPE       *result,
                                         const char *input,
                                         int         inputLength,
                                         ANY_CATEGORY)
{
    enum { BAEXML_FAILURE = -1 };

    BSLS_ASSERT_SAFE(!"Unsupported operation!");

    (void) result;
    (void) input;
    (void) inputLength;

    // Note: 'parseHex' for 'bsl::string' and 'bsl::vector<char>' is inside the
    //       CPP file.

    return BAEXML_FAILURE;
}

// LIST FUNCTIONS

template <typename TYPE>
int baexml_TypesParserUtil_Imp::parseList(TYPE       *result,
                                          const char *input,
                                          int         inputLength,
                                          bdeat_TypeCategory::Array)
{
    enum { BAEXML_FAILURE = -1 };

    typedef baexml_ListParser<TYPE> ListParser;

    typename ListParser::ParseElementFunction fn =
        &baexml_TypesParserUtil::parseDefault;

    ListParser listParser(fn);

    if (0 != listParser.beginParse(result)) {
        return BAEXML_FAILURE;
    }

    if (0 != listParser.pushCharacters(input, input + inputLength)) {
        return BAEXML_FAILURE;
    }

    return listParser.endParse();
}

template <typename TYPE>
inline
int baexml_TypesParserUtil_Imp::parseList(TYPE       *result,
                                          const char *input,
                                          int         inputLength,
                                          bdeat_TypeCategory::DynamicType)
{
    baexml_TypesParserUtil_Imp_parseListProxy proxy = { input, inputLength };

    return bdeat_TypeCategoryUtil::manipulateByCategory(result, proxy);
}

template <typename TYPE, typename ANY_CATEGORY>
inline
int baexml_TypesParserUtil_Imp::parseList(TYPE       *result,
                                          const char *input,
                                          int         inputLength,
                                          ANY_CATEGORY)
{
    enum { BAEXML_FAILURE = -1 };

    BSLS_ASSERT_SAFE(!"Unsupported operation!");

    (void) result;
    (void) input;
    (void) inputLength;

    return BAEXML_FAILURE;
}

// TEXT FUNCTIONS

template <typename TYPE>
inline
int baexml_TypesParserUtil_Imp::parseText(TYPE       *result,
                                          const char *input,
                                          int         inputLength,
                                          bdeat_TypeCategory::Enumeration)
{
    return bdeat_EnumFunctions::fromString(result, input, inputLength);
}

template <typename TYPE>
inline
int baexml_TypesParserUtil_Imp::parseText(TYPE       *result,
                                          const char *input,
                                          int         inputLength,
                                          bdeat_TypeCategory::CustomizedType)
{
    enum { BAEXML_FAILURE = -1 };

    typedef typename
    bdeat_CustomizedTypeFunctions::BaseType<TYPE>::Type BaseType;

    BaseType base;

    if (0 != baexml_TypesParserUtil::parseText(&base, input, inputLength)) {
        return BAEXML_FAILURE;
    }

    return bdeat_CustomizedTypeFunctions::convertFromBaseType(result, base);
}

template <typename TYPE>
inline
int baexml_TypesParserUtil_Imp::parseText(TYPE       *result,
                                          const char *input,
                                          int         inputLength,
                                          bdeat_TypeCategory::DynamicType)
{
    baexml_TypesParserUtil_Imp_parseTextProxy proxy = { input, inputLength };

    return bdeat_TypeCategoryUtil::manipulateByCategory(result, proxy);
}

template <typename TYPE, typename ANY_CATEGORY>
inline
int baexml_TypesParserUtil_Imp::parseText(TYPE       *result,
                                          const char *input,
                                          int         inputLength,
                                          ANY_CATEGORY)
{
    enum { BAEXML_FAILURE = -1 };

    BSLS_ASSERT_SAFE(!"Unsupported operation!");

    (void) result;
    (void) input;
    (void) inputLength;

    return BAEXML_FAILURE;
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
