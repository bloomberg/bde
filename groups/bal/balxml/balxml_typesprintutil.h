// balxml_typesprintutil.h                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALXML_TYPESPRINTUTIL
#define INCLUDED_BALXML_TYPESPRINTUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a utility for printing types using XML formatting.
//
//@CLASSES:
//   balxml::TypesPrintUtil: utility for printing using XML formatting
//
//@SEE_ALSO: http://www.w3.org/TR/xmlschema-2/
//
//@DESCRIPTION: The 'balxml::TypesPrintUtil' 'struct' provided by this
// component contains the following functions:
//
//: 'print':
//:   Print an object using the supplied formatting mode.
//:
//: 'printBase64':
//:   Print an object using 'bdlat_FormattingMode::e_BASE64'.
//:
//: 'printDecimal':
//:   Print an object using 'bdlat_FormattingMode::e_DEC'.
//:
//: 'printDefault':
//:   Print an object using 'bdlat_FormattingMode::e_DEFAULT'.
//:
//: 'printHex':
//:   Print an object using 'bdlat_FormattingMode::e_HEX'.
//:
//: 'printList':
//:   Print an object using 'bdlat_FormattingMode::e_IS_LIST'.
//:
//: 'printText':
//:   Print an object using 'bdlat_FormattingMode::e_TEXT'.
//
// The output is generated according to each type's lexical representation as
// described in the XML Schema Specification, which is available at
// 'http://www.w3.org/TR/xmlschema-2/'.  The text input is parsed and output
// according to the XML-1.0 with UTF-8 encoding, which prevents control
// characters (accepted by UTF-8) but otherwise accepts valid characters as
// described in the Unicode Standard 4.0, which is available at
// 'http://www.unicode.org/versions/Unicode4.0.0/' (well-formed UTF-8 byte
// sequences are described in Chapter 3, Section 3.9 and Table 3.5).
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
//      float                               DEFAULT
//      double                              DEFAULT
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
//      Enumeration                         DEFAULT, TEXT, DECIMAL
//      CustomizedType                      Base type's formatting modes
//      Array                               IS_LIST
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
///Behavior of 'printText' on Non-Valid Strings
///--------------------------------------------
// The output of 'printText' will always be valid XML 1.0 with UTF-8 encoding.
// When attempting to print text data that contains non-valid UTF-8 characters
// or non-printable control characters using 'printText', this component prints
// the valid characters up to and excluding the first invalid character.  See
// the second example in the 'Usage' section for an illustration.
//
///Usage
///-----
// The following snippets of code illustrates how to print an
// 'bsl::vector<char>' object in Base64 format:
//..
//  #include <balxml_typesprintutil.h>
//
//  #include <cassert>
//  #include <sstream>
//  #include <vector>
//
//  using namespace BloombergLP;
//
//  void usageExample1()
//  {
//      bsl::ostringstream ss;
//
//      bsl::vector<char> vec;
//      vec.push_back('a');
//      vec.push_back('b');
//      vec.push_back('c');
//      vec.push_back('d');
//
//      const char EXPECTED_RESULT[] = "YWJjZA==";
//
//      balxml::TypesPrintUtil::printBase64(ss, vec);
//      assert(EXPECTED_RESULT == ss.str());
//  }
//..
// The following snippet of shows what can be expected when printing valid or
// invalid data via 'printText':
//..
//  void usageExample2()
//  {
//      bsl::ostringstream ss;
//
//      const char VALID_STR[] = "Hello \t 'World'";
//..
// Note that all characters in the range '0x01' to '0x7F' are valid first bytes
// (including printable ASCII, TAB '0x09', or LF '0x0a', but excluding control
// characters other than TAB and LF) and that ampersand ('&' or '0x26'),
// less-than ('<' or '0x3c'), greater-than ('>' or '0x3e'), apostrophe
// ('0x27'), and quote ('"') will be printed as '&amp;', '&lt;', '&gt',
// '&apos;' and '&quot;' respectively.  Hence the expected output for the above
// string 'VALID_STR' is:
//..
//      const char EXPECTED_RESULT[] = "Hello \t &apos;World&apos;";
//..
// We can test that 'printText' will successfully print the string:
//..
//      balxml::TypesPrintUtil::printText(ss, VALID_STR);
//      assert(ss.good());
//      assert(EXPECTED_RESULT == ss.str());
//..
// In addition, when invalid data is printed, the stream is set to a bad state
// which is the proper means for the user to detect an error, as shown in the
// following code snippet:
//..
//      ss.str("");
//      const char INVALID_STR[]  = "Hello \300\t 'World'";
//      balxml::TypesPrintUtil::printText(ss, INVALID_STR);
//      assert(ss.fail());
//      assert("Hello " == ss.str());
//  }
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALXML_ENCODEROPTIONS
#include <balxml_encoderoptions.h>
#endif

#ifndef INCLUDED_BDLAT_ARRAYFUNCTIONS
#include <bdlat_arrayfunctions.h>
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

#ifndef INCLUDED_BDLB_FLOAT
#include <bdlb_float.h>
#endif

#ifndef INCLUDED_BSL_IOMANIP
#include <bsl_iomanip.h>
#endif

#ifndef INCLUDED_BSL_ISTREAM
#include <bsl_istream.h>
#endif

#ifndef INCLUDED_BSL_IOS
#include <bsl_ios.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

namespace balxml {
                           // ============================
                           // struct TypesPrintUtil
                           // ============================

struct TypesPrintUtil {
    // This 'struct' contains functions for printing objects to output streams
    // using various formatting modes.

    template <class TYPE>
    static bsl::ostream& print(bsl::ostream&         stream,
                               const TYPE&           object,
                               int                   formattingMode,
                               const EncoderOptions *encoderOptions = 0);
        // Format the specified 'object' to the specified output 'stream' using
        // the specified 'formattingMode' and the optionally specified
        // 'encoderOptions' and return a reference to 'stream'.  The behavior
        // is undefined unless the parameterized 'TYPE' and the
        // 'formattingMode' combination is supported (supported combinations
        // are listed in the component-level documentation).

    template <class TYPE>
    static bsl::ostream& printBase64(bsl::ostream&         stream,
                                     const TYPE&           object,
                                     const EncoderOptions *encoderOptions = 0);
        // Format the specified 'object' to the specified output 'stream' using
        // the 'bdlat_FormattingMode::e_BASE64' formatting mode and the
        // optionally specified 'encoderOptions'.  Return a reference to
        // 'stream'.

    template <class TYPE>
    static bsl::ostream& printDecimal(
                                     bsl::ostream&         stream,
                                     const TYPE&           object,
                                     const EncoderOptions *encoderOptions = 0);
        // Format the specified 'object' to the specified output 'stream' using
        // the 'bdlat_FormattingMode::e_DEC' formatting mode and the
        // optionally specified 'encoderOptions'.  Return a reference to
        // 'stream'.

    template <class TYPE>
    static bsl::ostream& printDefault(
                                     bsl::ostream&         stream,
                                     const TYPE&           object,
                                     const EncoderOptions *encoderOptions = 0);
        // Format the specified 'object' to the specified output 'stream' using
        // the 'bdlat_FormattingMode::e_DEFAULT' formatting mode and the
        // optionally specified 'encoderOptions'.  Return a reference to
        // 'stream'.

    template <class TYPE>
    static bsl::ostream& printHex(bsl::ostream&         stream,
                                  const TYPE&           object,
                                  const EncoderOptions *encoderOptions = 0);
        // Format the specified 'object' to the specified output 'stream' using
        // the 'bdlat_FormattingMode::e_HEX' formatting mode and the
        // optionally specified 'encoderOptions'.  Return a reference to
        // 'stream'.

    template <class TYPE>
    static bsl::ostream& printList(bsl::ostream&         stream,
                                   const TYPE&           object,
                                   const EncoderOptions *encoderOptions = 0);
        // Format the specified 'object' to the specified output 'stream' using
        // the 'bdlat_FormattingMode::e_LIST' formatting mode and the
        // optionally specified 'encoderOptions'.  Return a reference to
        // 'stream'.

    template <class TYPE>
    static bsl::ostream& printText(bsl::ostream&         stream,
                                   const TYPE&           object,
                                   const EncoderOptions *encoderOptions = 0);
        // Format the specified 'object' to the specified output 'stream' using
        // the 'bdlat_FormattingMode::e_TEXT' formatting mode and the
        // optionally specified 'encoderOptions'.  Return a reference to
        // 'stream'.  The string representation of 'object' must be a valid
        // UTF-8 string and may not contain any control characters other than
        // TAB, NL, and CR (i.e., no binary data) unless
        // 'encoderOptions->allowControlCharacters()' is 'true', in which
        // control characters will be encoded as is.  Upon detecting an invalid
        // byte, the output stops and the 'failbit' is be set on the output
        // 'stream'.  In the case of an invalid byte in a multi-byte
        // character, the output stops after the previous character and no
        // byte in this character is output.
};

                         // =========================
                         // struct TypesPrintUtil_Imp
                         // =========================

struct TypesPrintUtil_Imp {
    // This 'struct' contains functions that are used in the implementation of
    // this component.

    // BASE64 FUNCTIONS
    template <class TYPE>
    static bsl::ostream& printBase64(
                               bsl::ostream&                    stream,
                               const TYPE&                      object,
                               const EncoderOptions            *encoderOptions,
                               bdlat_TypeCategory::DynamicType);

    template <class TYPE, class ANY_CATEGORY>
    static bsl::ostream& printBase64(bsl::ostream&         stream,
                                     const TYPE&           object,
                                     const EncoderOptions *encoderOptions,
                                     ANY_CATEGORY);

    static bsl::ostream& printBase64(
                                    bsl::ostream&               stream,
                                    const bsl::string&          object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple);
    static bsl::ostream& printBase64(
                                   bsl::ostream&               stream,
                                   const bslstl::StringRef&    object,
                                   const EncoderOptions       *encoderOptions,
                                   bdlat_TypeCategory::Simple);
    static bsl::ostream& printBase64(bsl::ostream&              stream,
                                     const bsl::vector<char>&   object,
                                     const EncoderOptions      *encoderOptions,
                                     bdlat_TypeCategory::Array);

    // DECIMAL FUNCTIONS
    template <class TYPE>
    static bsl::ostream& printDecimal(
                               bsl::ostream&                    stream,
                               const TYPE&                      object,
                               const EncoderOptions            *encoderOptions,
                               bdlat_TypeCategory::Enumeration);

    template <class TYPE>
    static bsl::ostream& printDecimal(
                            bsl::ostream&                       stream,
                            const TYPE&                         object,
                            const EncoderOptions               *encoderOptions,
                            bdlat_TypeCategory::CustomizedType);

    template <class TYPE>
    static bsl::ostream& printDecimal(
                               bsl::ostream&                    stream,
                               const TYPE&                      object,
                               const EncoderOptions            *encoderOptions,
                               bdlat_TypeCategory::DynamicType);

    template <class TYPE, class ANY_CATEGORY>
    static bsl::ostream& printDecimal(bsl::ostream&         stream,
                                      const TYPE&           object,
                                      const EncoderOptions *encoderOptions,
                                      ANY_CATEGORY);

    static bsl::ostream& printDecimal(
                                    bsl::ostream&               stream,
                                    const bool&                 object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple);
    static bsl::ostream& printDecimal(
                                    bsl::ostream&               stream,
                                    const char&                 object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple);
    static bsl::ostream& printDecimal(
                                    bsl::ostream&               stream,
                                    const short&                object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple);
    static bsl::ostream& printDecimal(
                                    bsl::ostream&               stream,
                                    const int&                  object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple);
    static bsl::ostream& printDecimal(
                                    bsl::ostream&               stream,
                                    const long&                 object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple);
    static bsl::ostream& printDecimal(
                                    bsl::ostream&               stream,
                                    const bsls::Types::Int64&   object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple);
    static bsl::ostream& printDecimal(
                                    bsl::ostream&               stream,
                                    const unsigned char&        object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple);
    static bsl::ostream& printDecimal(
                                    bsl::ostream&               stream,
                                    const unsigned short&       object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple);
    static bsl::ostream& printDecimal(
                                    bsl::ostream&               stream,
                                    const unsigned int&         object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple);
    static bsl::ostream& printDecimal(
                                    bsl::ostream&               stream,
                                    const unsigned long&        object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple);
    static bsl::ostream& printDecimal(
                                    bsl::ostream&               stream,
                                    const bsls::Types::Uint64&  object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple);

    static bsl::ostream& printDecimal(
                                    bsl::ostream&               stream,
                                    const float&                object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple);

    static bsl::ostream& printDecimal(
                                    bsl::ostream&               stream,
                                    const double&               object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple);

    // DEFAULT FUNCTIONS
    template <class TYPE>
    static bsl::ostream& printDefault(
                               bsl::ostream&                    stream,
                               const TYPE&                      object,
                               const EncoderOptions            *encoderOptions,
                               bdlat_TypeCategory::Enumeration);

    template <class TYPE>
    static bsl::ostream& printDefault(
                            bsl::ostream&                       stream,
                            const TYPE&                         object,
                            const EncoderOptions               *encoderOptions,
                            bdlat_TypeCategory::CustomizedType);

    template <class TYPE>
    static bsl::ostream& printDefault(
                               bsl::ostream&                    stream,
                               const TYPE&                      object,
                               const EncoderOptions            *encoderOptions,
                               bdlat_TypeCategory::DynamicType);

    template <class TYPE, class ANY_CATEGORY>
    static bsl::ostream& printDefault(bsl::ostream&         stream,
                                      const TYPE&           object,
                                      const EncoderOptions *encoderOptions,
                                      ANY_CATEGORY);

    static bsl::ostream& printDefault(
                                    bsl::ostream&               stream,
                                    const bool&                 object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple);
    static bsl::ostream& printDefault(
                                    bsl::ostream&               stream,
                                    const char&                 object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple);
    static bsl::ostream& printDefault(
                                    bsl::ostream&               stream,
                                    const short&                object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple);
    static bsl::ostream& printDefault(
                                    bsl::ostream&               stream,
                                    const int&                  object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple);
    static bsl::ostream& printDefault(
                                    bsl::ostream&               stream,
                                    const long&                 object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple);
    static bsl::ostream& printDefault(
                                    bsl::ostream&               stream,
                                    const bsls::Types::Int64&   object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple);
    static bsl::ostream& printDefault(
                                    bsl::ostream&               stream,
                                    const unsigned char&        object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple);
    static bsl::ostream& printDefault(
                                    bsl::ostream&               stream,
                                    const unsigned short&       object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple);
    static bsl::ostream& printDefault(
                                    bsl::ostream&               stream,
                                    const unsigned int&         object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple);
    static bsl::ostream& printDefault(
                                    bsl::ostream&               stream,
                                    const unsigned long&        object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple);
    static bsl::ostream& printDefault(
                                    bsl::ostream&               stream,
                                    const bsls::Types::Uint64&  object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple);
    static bsl::ostream& printDefault(
                                    bsl::ostream&               stream,
                                    const float&                object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple);
    static bsl::ostream& printDefault(
                                    bsl::ostream&               stream,
                                    const double&               object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple);
    static bsl::ostream& printDefault(
                                    bsl::ostream&               stream,
                                    const char                 *object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple);
    static bsl::ostream& printDefault(
                                    bsl::ostream&               stream,
                                    const bsl::string&          object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple);
    static bsl::ostream& printDefault(
                                    bsl::ostream&               stream,
                                    const bslstl::StringRef&    object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple);
    static bsl::ostream& printDefault(
                                    bsl::ostream&               stream,
                                    const bdlt::Date&           object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple);
    static bsl::ostream& printDefault(
                                    bsl::ostream&               stream,
                                    const bdlt::DateTz&         object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple);
    static bsl::ostream& printDefault(
                                    bsl::ostream&               stream,
                                    const bdlt::Datetime&       object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple);
    static bsl::ostream& printDefault(
                                    bsl::ostream&               stream,
                                    const bdlt::DatetimeTz&     object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple);
    static bsl::ostream& printDefault(
                                    bsl::ostream&               stream,
                                    const bdlt::Time&           object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple);
    static bsl::ostream& printDefault(
                                    bsl::ostream&               stream,
                                    const bdlt::TimeTz&         object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple);
    static bsl::ostream& printDefault(
                                     bsl::ostream&              stream,
                                     const bsl::vector<char>&   object,
                                     const EncoderOptions      *encoderOptions,
                                     bdlat_TypeCategory::Array);

    // HEX FUNCTIONS
    template <class TYPE>
    static bsl::ostream& printHex(
                               bsl::ostream&                    stream,
                               const TYPE&                      object,
                               const EncoderOptions            *encoderOptions,
                               bdlat_TypeCategory::DynamicType);

    template <class TYPE, class ANY_CATEGORY>
    static bsl::ostream& printHex(bsl::ostream&         stream,
                                  const TYPE&           object,
                                  const EncoderOptions *encoderOptions,
                                  ANY_CATEGORY);

    static bsl::ostream& printHex(bsl::ostream&               stream,
                                  const bsl::string&          object,
                                  const EncoderOptions       *encoderOptions,
                                  bdlat_TypeCategory::Simple);
    static bsl::ostream& printHex(bsl::ostream&               stream,
                                  const bslstl::StringRef&    object,
                                  const EncoderOptions       *encoderOptions,
                                  bdlat_TypeCategory::Simple);
    static bsl::ostream& printHex(bsl::ostream&              stream,
                                  const bsl::vector<char>&   object,
                                  const EncoderOptions      *encoderOptions,
                                  bdlat_TypeCategory::Array);

    // LIST FUNCTIONS
    template <class TYPE>
    static bsl::ostream& printList(bsl::ostream&              stream,
                                   const TYPE&                object,
                                   const EncoderOptions      *encoderOptions,
                                   bdlat_TypeCategory::Array);

    template <class TYPE>
    static bsl::ostream& printList(
                               bsl::ostream&                    stream,
                               const TYPE&                      object,
                               const EncoderOptions            *encoderOptions,
                               bdlat_TypeCategory::DynamicType);

    template <class TYPE, class ANY_CATEGORY>
    static bsl::ostream& printList(bsl::ostream&         stream,
                                   const TYPE&           object,
                                   const EncoderOptions *encoderOptions,
                                   ANY_CATEGORY);

    // TEXT FUNCTIONS
    template <class TYPE>
    static bsl::ostream& printText(
                               bsl::ostream&                    stream,
                               const TYPE&                      object,
                               const EncoderOptions            *encoderOptions,
                               bdlat_TypeCategory::Enumeration);

    template <class TYPE>
    static bsl::ostream& printText(
                            bsl::ostream&                       stream,
                            const TYPE&                         object,
                            const EncoderOptions               *encoderOptions,
                            bdlat_TypeCategory::CustomizedType);

    template <class TYPE>
    static bsl::ostream& printText(
                               bsl::ostream&                    stream,
                               const TYPE&                      object,
                               const EncoderOptions            *encoderOptions,
                               bdlat_TypeCategory::DynamicType);

    template <class TYPE, class ANY_CATEGORY>
    static bsl::ostream& printText(bsl::ostream&         stream,
                                   const TYPE&           object,
                                   const EncoderOptions *encoderOptions,
                                   ANY_CATEGORY);

    static bsl::ostream& printText(bsl::ostream&               stream,
                                   const bool&                 object,
                                   const EncoderOptions       *encoderOptions,
                                   bdlat_TypeCategory::Simple);
    static bsl::ostream& printText(bsl::ostream&               stream,
                                   const char&                 object,
                                   const EncoderOptions       *encoderOptions,
                                   bdlat_TypeCategory::Simple);
    static bsl::ostream& printText(bsl::ostream&               stream,
                                   const char                 *object,
                                   const EncoderOptions       *encoderOptions,
                                   bdlat_TypeCategory::Simple);
    static bsl::ostream& printText(bsl::ostream&               stream,
                                   const bsl::string&          object,
                                   const EncoderOptions       *encoderOptions,
                                   bdlat_TypeCategory::Simple);
    static bsl::ostream& printText(bsl::ostream&               stream,
                                   const bslstl::StringRef&    object,
                                   const EncoderOptions       *encoderOptions,
                                   bdlat_TypeCategory::Simple);
    static bsl::ostream& printText(bsl::ostream&              stream,
                                   const bsl::vector<char>&   object,
                                   const EncoderOptions      *encoderOptions,
                                   bdlat_TypeCategory::Array);
};

// ============================================================================
//                               PROXY CLASSES
// ============================================================================

                  // =======================================
                  // struct TypesPrintUtil_printDefaultProxy
                  // =======================================

struct TypesPrintUtil_printDefaultProxy {
    // Component-private struct.  Do not use.

    // DATA MEMBERS
    bsl::ostream                *d_stream_p;
    const EncoderOptions *d_encoderOptions_p;

    // CREATORS

    // Creators have been omitted to allow simple static initialization of
    // this struct.

    // FUNCTIONS
    template <class TYPE>
    inline
    int operator()(const TYPE& object)
    {
        TypesPrintUtil::printDefault(*d_stream_p, object, d_encoderOptions_p);
        return 0;  // returned value is ignored
    }
};

                 // ==========================================
                 // struct TypesPrintUtil_Imp_printBase64Proxy
                 // ==========================================

struct TypesPrintUtil_Imp_printBase64Proxy {
    // Component-private struct.  Do not use.

    // DATA MEMBERS
    bsl::ostream                *d_stream_p;
    const EncoderOptions *d_encoderOptions_p;

    // CREATORS

    // Creators have been omitted to allow simple static initialization of
    // this struct.

    // FUNCTIONS
    template <class TYPE>
    inline
    int operator()(const TYPE&, bslmf::Nil)
    {
        BSLS_ASSERT_SAFE(0);
        return -1;
    }

    template <class TYPE, class ANY_CATEGORY>
    inline
    int operator()(const TYPE& object, ANY_CATEGORY category)
    {
        TypesPrintUtil_Imp::printBase64(*d_stream_p,
                                        object,
                                        d_encoderOptions_p,
                                        category);
        return 0;  // returned value is ignored
    }
};

                // ===========================================
                // struct TypesPrintUtil_Imp_printDecimalProxy
                // ===========================================

struct TypesPrintUtil_Imp_printDecimalProxy {
    // Component-private struct.  Do not use.

    // DATA MEMBERS
    bsl::ostream                *d_stream_p;
    const EncoderOptions *d_encoderOptions_p;

    // CREATORS

    // Creators have been omitted to allow simple static initialization of
    // this struct.

    // FUNCTIONS
    template <class TYPE>
    inline
    int operator()(const TYPE&, bslmf::Nil)
    {
        BSLS_ASSERT_SAFE(0);
        return -1;
    }

    template <class TYPE, class ANY_CATEGORY>
    inline
    int operator()(const TYPE& object, ANY_CATEGORY category)
    {
        TypesPrintUtil_Imp::printDecimal(*d_stream_p,
                                         object,
                                         d_encoderOptions_p,
                                         category);
        return 0;  // returned value is ignored
    }
};

                // ===========================================
                // struct TypesPrintUtil_Imp_printDefaultProxy
                // ===========================================

struct TypesPrintUtil_Imp_printDefaultProxy {
    // Component-private struct.  Do not use.

    // DATA MEMBERS
    bsl::ostream                *d_stream_p;
    const EncoderOptions *d_encoderOptions_p;

    // CREATORS

    // Creators have been omitted to allow simple static initialization of
    // this struct.

    // FUNCTIONS
    template <class TYPE>
    inline
    int operator()(const TYPE&, bslmf::Nil)
    {
        BSLS_ASSERT_SAFE(0);
        return -1;
    }

    template <class TYPE, class ANY_CATEGORY>
    inline
    int operator()(const TYPE& object, ANY_CATEGORY category)
    {
        TypesPrintUtil_Imp::printDefault(*d_stream_p,
                                         object,
                                         d_encoderOptions_p,
                                         category);
        return 0;  // returned value is ignored
    }
};

                  // =======================================
                  // struct TypesPrintUtil_Imp_printHexProxy
                  // =======================================

struct TypesPrintUtil_Imp_printHexProxy {
    // Component-private struct.  Do not use.

    // DATA MEMBERS
    bsl::ostream                *d_stream_p;
    const EncoderOptions *d_encoderOptions_p;

    // CREATORS

    // Creators have been omitted to allow simple static initialization of
    // this struct.

    // FUNCTIONS
    template <class TYPE>
    inline
    int operator()(const TYPE&, bslmf::Nil)
    {
        BSLS_ASSERT_SAFE(0);
        return -1;
    }

    template <class TYPE, class ANY_CATEGORY>
    inline
    int operator()(const TYPE& object, ANY_CATEGORY category)
    {
        TypesPrintUtil_Imp::printHex(*d_stream_p,
                                     object,
                                     d_encoderOptions_p,
                                     category);
        return 0;  // returned value is ignored
    }
};

                  // ========================================
                  // struct TypesPrintUtil_Imp_printListProxy
                  // ========================================

struct TypesPrintUtil_Imp_printListProxy {
    // Component-private struct.  Do not use.

    // DATA MEMBERS
    bsl::ostream                *d_stream_p;
    const EncoderOptions *d_encoderOptions_p;

    // CREATORS

    // Creators have been omitted to allow simple static initialization of
    // this struct.

    // FUNCTIONS
    template <class TYPE>
    inline
    int operator()(const TYPE&, bslmf::Nil)
    {
        BSLS_ASSERT_SAFE(0);
        return -1;
    }

    template <class TYPE, class ANY_CATEGORY>
    inline
    int operator()(const TYPE& object, ANY_CATEGORY category)
    {
        TypesPrintUtil_Imp::printList(*d_stream_p,
                                      object,
                                      d_encoderOptions_p,
                                      category);
        return 0;  // returned value is ignored
    }
};

                  // ========================================
                  // struct TypesPrintUtil_Imp_printTextProxy
                  // ========================================

struct TypesPrintUtil_Imp_printTextProxy {
    // Component-private struct.  Do not use.

    // DATA MEMBERS
    bsl::ostream                *d_stream_p;
    const EncoderOptions *d_encoderOptions_p;

    // CREATORS

    // Creators have been omitted to allow simple static initialization of
    // this struct.

    // FUNCTIONS
    template <class TYPE>
    inline
    int operator()(const TYPE&, bslmf::Nil)
    {
        BSLS_ASSERT_SAFE(0);
        return -1;
    }

    template <class TYPE, class ANY_CATEGORY>
    inline
    int operator()(const TYPE& object, ANY_CATEGORY category)
    {
        TypesPrintUtil_Imp::printText(*d_stream_p,
                                      object,
                                      d_encoderOptions_p,
                                      category);
        return 0;  // returned value is ignored
    }
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           // ---------------------
                           // struct TypesPrintUtil
                           // ---------------------

template <class TYPE>
bsl::ostream& TypesPrintUtil::print(bsl::ostream&         stream,
                                    const TYPE&           object,
                                    int                   formattingMode,
                                    const EncoderOptions *encoderOptions)
{
    typedef typename bdlat_TypeCategory::Select<TYPE>::Type Tag;

    if (formattingMode & bdlat_FormattingMode::e_LIST) {
        return TypesPrintUtil_Imp::printList(stream,
                                             object,
                                             encoderOptions,
                                             Tag());                  // RETURN
    }

    switch (formattingMode & bdlat_FormattingMode::e_TYPE_MASK) {
      case bdlat_FormattingMode::e_BASE64: {
        return TypesPrintUtil_Imp::printBase64(stream,
                                               object,
                                               encoderOptions,
                                               Tag());                // RETURN
      }
      case bdlat_FormattingMode::e_DEC: {
        return TypesPrintUtil_Imp::printDecimal(stream,
                                                object,
                                                encoderOptions,
                                                Tag());               // RETURN
      }
      case bdlat_FormattingMode::e_DEFAULT: {
        return TypesPrintUtil_Imp::printDefault(stream,
                                                object,
                                                encoderOptions,
                                                Tag());               // RETURN
      }
      case bdlat_FormattingMode::e_HEX: {
        return TypesPrintUtil_Imp::printHex(stream,
                                            object,
                                            encoderOptions,
                                            Tag());                   // RETURN
      }
      case bdlat_FormattingMode::e_TEXT: {
        return TypesPrintUtil_Imp::printText(stream,
                                             object,
                                             encoderOptions,
                                             Tag());                  // RETURN
      }
      default: {
        BSLS_ASSERT_SAFE(!"Unsupported operation!");

        stream.setstate(bsl::ios_base::failbit);

        return stream;                                                // RETURN
      }
    }
}

template <class TYPE>
inline
bsl::ostream& TypesPrintUtil::printBase64(bsl::ostream&         stream,
                                          const TYPE&           object,
                                          const EncoderOptions *encoderOptions)
{
    typedef typename bdlat_TypeCategory::Select<TYPE>::Type Tag;

    return TypesPrintUtil_Imp::printBase64(stream,
                                           object,
                                           encoderOptions,
                                           Tag());
}

template <class TYPE>
inline
bsl::ostream& TypesPrintUtil::printDecimal(
                                          bsl::ostream&         stream,
                                          const TYPE&           object,
                                          const EncoderOptions *encoderOptions)
{
    typedef typename bdlat_TypeCategory::Select<TYPE>::Type Tag;

    return TypesPrintUtil_Imp::printDecimal(stream,
                                            object,
                                            encoderOptions,
                                            Tag());
}

template <class TYPE>
inline
bsl::ostream& TypesPrintUtil::printDefault(
                                          bsl::ostream&         stream,
                                          const TYPE&           object,
                                          const EncoderOptions *encoderOptions)
{
    typedef typename bdlat_TypeCategory::Select<TYPE>::Type Tag;

    return TypesPrintUtil_Imp::printDefault(stream,
                                            object,
                                            encoderOptions,
                                            Tag());
}

template <class TYPE>
inline
bsl::ostream& TypesPrintUtil::printHex(bsl::ostream&         stream,
                                       const TYPE&           object,
                                       const EncoderOptions *encoderOptions)
{
    typedef typename bdlat_TypeCategory::Select<TYPE>::Type Tag;

    return TypesPrintUtil_Imp::printHex(stream,
                                        object,
                                        encoderOptions,
                                        Tag());
}

template <class TYPE>
inline
bsl::ostream& TypesPrintUtil::printList(bsl::ostream&         stream,
                                        const TYPE&           object,
                                        const EncoderOptions *encoderOptions)
{
    typedef typename bdlat_TypeCategory::Select<TYPE>::Type Tag;

    return TypesPrintUtil_Imp::printList(stream,
                                         object,
                                         encoderOptions,
                                         Tag());
}

template <class TYPE>
inline
bsl::ostream& TypesPrintUtil::printText(bsl::ostream&         stream,
                                        const TYPE&           object,
                                        const EncoderOptions *encoderOptions)
{
    typedef typename bdlat_TypeCategory::Select<TYPE>::Type Tag;

    return TypesPrintUtil_Imp::printText(stream,
                                         object,
                                         encoderOptions,
                                         Tag());
}

                         // -------------------------
                         // struct TypesPrintUtil_Imp
                         // -------------------------

// BASE64 FUNCTIONS

template <class TYPE>
inline
bsl::ostream& TypesPrintUtil_Imp::printBase64(
                               bsl::ostream&                    stream,
                               const TYPE&                      object,
                               const EncoderOptions            *encoderOptions,
                               bdlat_TypeCategory::DynamicType)
{
    TypesPrintUtil_Imp_printBase64Proxy proxy = { &stream, encoderOptions };
    bdlat_TypeCategoryUtil::accessByCategory(object, proxy);
    return stream;
}

template <class TYPE, class ANY_CATEGORY>
inline
bsl::ostream& TypesPrintUtil_Imp::printBase64(bsl::ostream&         stream,
                                              const TYPE&,
                                              const EncoderOptions *,
                                              ANY_CATEGORY)
{
    BSLS_ASSERT_SAFE(!"Unsupported operation!");

    stream.setstate(bsl::ios_base::failbit);

    // Note: 'printBase64' for 'bsl::string' and 'bsl::vector<char>' is inside
    //       the CPP file.

    return stream;
}

// DECIMAL FUNCTIONS

template <class TYPE>
inline
bsl::ostream& TypesPrintUtil_Imp::printDecimal(
                               bsl::ostream&                    stream,
                               const TYPE&                      object,
                               const EncoderOptions            *encoderOptions,
                               bdlat_TypeCategory::Enumeration)
{
    int intValue;

    bdlat_EnumFunctions::toInt(&intValue, object);

    return TypesPrintUtil::printDecimal(stream,
                                        intValue,
                                        encoderOptions);
}

template <class TYPE>
inline
bsl::ostream& TypesPrintUtil_Imp::printDecimal(
                            bsl::ostream&                       stream,
                            const TYPE&                         object,
                            const EncoderOptions               *encoderOptions,
                            bdlat_TypeCategory::CustomizedType)
{
    return TypesPrintUtil::printDecimal(
                      stream,
                      bdlat_CustomizedTypeFunctions::convertToBaseType(object),
                      encoderOptions);
}

template <class TYPE>
inline
bsl::ostream& TypesPrintUtil_Imp::printDecimal(
                               bsl::ostream&                    stream,
                               const TYPE&                      object,
                               const EncoderOptions            *encoderOptions,
                               bdlat_TypeCategory::DynamicType)
{
    TypesPrintUtil_Imp_printDecimalProxy proxy = { &stream, encoderOptions };
    bdlat_TypeCategoryUtil::accessByCategory(object, proxy);
    return stream;
}

template <class TYPE, class ANY_CATEGORY>
inline
bsl::ostream& TypesPrintUtil_Imp::printDecimal(bsl::ostream&         stream,
                                               const TYPE&,
                                               const EncoderOptions *,
                                               ANY_CATEGORY)
{
    BSLS_ASSERT_SAFE(!"Unsupported operation!");

    stream.setstate(bsl::ios_base::failbit);

    return stream;
}

inline
bsl::ostream& TypesPrintUtil_Imp::printDecimal(
                                            bsl::ostream&               stream,
                                            const bool&                 object,
                                            const EncoderOptions       *,
                                            bdlat_TypeCategory::Simple)
{
    return stream << (object ? 1 : 0);
}

inline
bsl::ostream& TypesPrintUtil_Imp::printDecimal(
                                            bsl::ostream&               stream,
                                            const char&                 object,
                                            const EncoderOptions       *,
                                            bdlat_TypeCategory::Simple)
{
    signed char temp(object);  // Note that 'char' is unsigned on IBM.

    return stream << int(temp);
}

inline
bsl::ostream& TypesPrintUtil_Imp::printDecimal(
                                            bsl::ostream&               stream,
                                            const short&                object,
                                            const EncoderOptions       *,
                                            bdlat_TypeCategory::Simple)
{
    return stream << object;
}

inline
bsl::ostream& TypesPrintUtil_Imp::printDecimal(
                                            bsl::ostream&               stream,
                                            const int&                  object,
                                            const EncoderOptions       *,
                                            bdlat_TypeCategory::Simple)
{
    return stream << object;
}

inline
bsl::ostream& TypesPrintUtil_Imp::printDecimal(
                                            bsl::ostream&               stream,
                                            const long&                 object,
                                            const EncoderOptions       *,
                                            bdlat_TypeCategory::Simple)
{
    return stream << object;
}

inline
bsl::ostream& TypesPrintUtil_Imp::printDecimal(
                                            bsl::ostream&               stream,
                                            const bsls::Types::Int64&   object,
                                            const EncoderOptions       *,
                                            bdlat_TypeCategory::Simple)
{
    return stream << object;
}

inline
bsl::ostream& TypesPrintUtil_Imp::printDecimal(
                                            bsl::ostream&               stream,
                                            const unsigned char&        object,
                                            const EncoderOptions       *,
                                            bdlat_TypeCategory::Simple)
{
    unsigned short us = object;
    return stream << us;
}

inline
bsl::ostream& TypesPrintUtil_Imp::printDecimal(
                                            bsl::ostream&               stream,
                                            const unsigned short&       object,
                                            const EncoderOptions       *,
                                            bdlat_TypeCategory::Simple)
{
    return stream << object;
}

inline
bsl::ostream& TypesPrintUtil_Imp::printDecimal(
                                            bsl::ostream&               stream,
                                            const unsigned int&         object,
                                            const EncoderOptions       *,
                                            bdlat_TypeCategory::Simple)
{
    return stream << object;
}

inline
bsl::ostream& TypesPrintUtil_Imp::printDecimal(
                                            bsl::ostream&               stream,
                                            const unsigned long&        object,
                                            const EncoderOptions       *,
                                            bdlat_TypeCategory::Simple)
{
    return stream << object;
}

inline
bsl::ostream& TypesPrintUtil_Imp::printDecimal(
                                            bsl::ostream&               stream,
                                            const bsls::Types::Uint64&  object,
                                            const EncoderOptions       *,
                                            bdlat_TypeCategory::Simple)
{
    return stream << object;
}

// DEFAULT FUNCTIONS

template <class TYPE>
inline
bsl::ostream& TypesPrintUtil_Imp::printDefault(
                               bsl::ostream&                    stream,
                               const TYPE&                      object,
                               const EncoderOptions            *encoderOptions,
                               bdlat_TypeCategory::Enumeration)
{
    bsl::string stringVal;

    bdlat_EnumFunctions::toString(&stringVal, object);

    return TypesPrintUtil::printText(stream, stringVal, encoderOptions);
}

template <class TYPE>
inline
bsl::ostream& TypesPrintUtil_Imp::printDefault(
                            bsl::ostream&                       stream,
                            const TYPE&                         object,
                            const EncoderOptions               *encoderOptions,
                            bdlat_TypeCategory::CustomizedType)
{
    return TypesPrintUtil::printDefault(
                      stream,
                      bdlat_CustomizedTypeFunctions::convertToBaseType(object),
                      encoderOptions);
}

template <class TYPE>
inline
bsl::ostream& TypesPrintUtil_Imp::printDefault(
                               bsl::ostream&                    stream,
                               const TYPE&                      object,
                               const EncoderOptions            *encoderOptions,
                               bdlat_TypeCategory::DynamicType)
{
    TypesPrintUtil_Imp_printDefaultProxy proxy = { &stream, encoderOptions };
    bdlat_TypeCategoryUtil::accessByCategory(object, proxy);
    return stream;
}

template <class TYPE, class ANY_CATEGORY>
inline
bsl::ostream& TypesPrintUtil_Imp::printDefault(bsl::ostream&         stream,
                                               const TYPE&,
                                               const EncoderOptions *,
                                               ANY_CATEGORY)
{
    BSLS_ASSERT_SAFE(!"Unsupported operation!");

    stream.setstate(bsl::ios_base::failbit);

    return stream;
}

inline
bsl::ostream& TypesPrintUtil_Imp::printDefault(
                                    bsl::ostream&               stream,
                                    const bool&                 object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple)
{
    return printText(stream,
                     object,
                     encoderOptions,
                     bdlat_TypeCategory::Simple());
}

inline
bsl::ostream& TypesPrintUtil_Imp::printDefault(
                                    bsl::ostream&               stream,
                                    const char&                 object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple)
{
    return printDecimal(stream,
                        object,
                        encoderOptions,
                        bdlat_TypeCategory::Simple());
}

inline
bsl::ostream& TypesPrintUtil_Imp::printDefault(
                                    bsl::ostream&               stream,
                                    const short&                object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple)
{
    return printDecimal(stream,
                        object,
                        encoderOptions,
                        bdlat_TypeCategory::Simple());
}

inline
bsl::ostream& TypesPrintUtil_Imp::printDefault(
                                    bsl::ostream&               stream,
                                    const int&                  object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple)
{
    return printDecimal(stream,
                        object,
                        encoderOptions,
                        bdlat_TypeCategory::Simple());
}

inline
bsl::ostream& TypesPrintUtil_Imp::printDefault(
                                    bsl::ostream&               stream,
                                    const long&                 object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple)
{
    return printDecimal(stream,
                        object,
                        encoderOptions,
                        bdlat_TypeCategory::Simple());
}

inline
bsl::ostream& TypesPrintUtil_Imp::printDefault(
                                    bsl::ostream&               stream,
                                    const bsls::Types::Int64&   object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple)
{
    return printDecimal(stream,
                        object,
                        encoderOptions,
                        bdlat_TypeCategory::Simple());
}

inline
bsl::ostream& TypesPrintUtil_Imp::printDefault(
                                    bsl::ostream&               stream,
                                    const unsigned char&        object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple)
{
    return printDecimal(stream,
                        object,
                        encoderOptions,
                        bdlat_TypeCategory::Simple());
}

inline
bsl::ostream& TypesPrintUtil_Imp::printDefault(
                                    bsl::ostream&               stream,
                                    const unsigned short&       object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple)
{
    return printDecimal(stream,
                        object,
                        encoderOptions,
                        bdlat_TypeCategory::Simple());
}

inline
bsl::ostream& TypesPrintUtil_Imp::printDefault(
                                    bsl::ostream&               stream,
                                    const unsigned int&         object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple)
{
    return printDecimal(stream,
                        object,
                        encoderOptions,
                        bdlat_TypeCategory::Simple());
}

inline
bsl::ostream& TypesPrintUtil_Imp::printDefault(
                                    bsl::ostream&               stream,
                                    const unsigned long&        object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple)
{
    return printDecimal(stream,
                        object,
                        encoderOptions,
                        bdlat_TypeCategory::Simple());
}

inline
bsl::ostream& TypesPrintUtil_Imp::printDefault(
                                    bsl::ostream&               stream,
                                    const bsls::Types::Uint64&  object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple)
{
    return printDecimal(stream,
                        object,
                        encoderOptions,
                        bdlat_TypeCategory::Simple());
}

inline
bsl::ostream& TypesPrintUtil_Imp::printDefault(
                                    bsl::ostream&               stream,
                                    const char                 *object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple)
{
    return printText(stream,
                     object,
                     encoderOptions,
                     bdlat_TypeCategory::Simple());
}

inline
bsl::ostream& TypesPrintUtil_Imp::printDefault(
                                    bsl::ostream&               stream,
                                    const bsl::string&          object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple)
{
    return printText(stream,
                     object,
                     encoderOptions,
                     bdlat_TypeCategory::Simple());
}

inline
bsl::ostream& TypesPrintUtil_Imp::printDefault(
                                    bsl::ostream&               stream,
                                    const bslstl::StringRef&    object,
                                    const EncoderOptions       *encoderOptions,
                                    bdlat_TypeCategory::Simple)
{
    return printText(stream,
                     object,
                     encoderOptions,
                     bdlat_TypeCategory::Simple());
}

inline
bsl::ostream& TypesPrintUtil_Imp::printDefault(
                                            bsl::ostream&               stream,
                                            const bdlt::Date&           object,
                                            const EncoderOptions       *,
                                            bdlat_TypeCategory::Simple)
{
    return bdlt::Iso8601Util::generate(stream, object);
}

inline
bsl::ostream& TypesPrintUtil_Imp::printDefault(
                                            bsl::ostream&               stream,
                                            const bdlt::DateTz&         object,
                                            const EncoderOptions       *,
                                            bdlat_TypeCategory::Simple)
{
    return bdlt::Iso8601Util::generate(stream, object);
}

inline
bsl::ostream& TypesPrintUtil_Imp::printDefault(
                                            bsl::ostream&               stream,
                                            const bdlt::Datetime&       object,
                                            const EncoderOptions       *,
                                            bdlat_TypeCategory::Simple)
{
    return bdlt::Iso8601Util::generate(stream, object);
}

inline
bsl::ostream& TypesPrintUtil_Imp::printDefault(
                                            bsl::ostream&               stream,
                                            const bdlt::DatetimeTz&     object,
                                            const EncoderOptions       *,
                                            bdlat_TypeCategory::Simple)
{
    return bdlt::Iso8601Util::generate(stream, object);
}

inline
bsl::ostream& TypesPrintUtil_Imp::printDefault(
                                            bsl::ostream&               stream,
                                            const bdlt::Time&           object,
                                            const EncoderOptions       *,
                                            bdlat_TypeCategory::Simple)
{
    return bdlt::Iso8601Util::generate(stream, object);
}

inline
bsl::ostream& TypesPrintUtil_Imp::printDefault(
                                            bsl::ostream&               stream,
                                            const bdlt::TimeTz&         object,
                                            const EncoderOptions       *,
                                            bdlat_TypeCategory::Simple)
{
    return bdlt::Iso8601Util::generate(stream, object);
}

inline
bsl::ostream& TypesPrintUtil_Imp::printDefault(
                                     bsl::ostream&              stream,
                                     const bsl::vector<char>&   object,
                                     const EncoderOptions      *encoderOptions,
                                     bdlat_TypeCategory::Array)
{
    return printBase64(stream,
                       object,
                       encoderOptions,
                       bdlat_TypeCategory::Array());
}

// HEX FUNCTIONS

template <class TYPE>
inline
bsl::ostream& TypesPrintUtil_Imp::printHex(
                               bsl::ostream&                    stream,
                               const TYPE&                      object,
                               const EncoderOptions            *encoderOptions,
                               bdlat_TypeCategory::DynamicType)
{
    TypesPrintUtil_Imp_printHexProxy proxy = { &stream, encoderOptions };
    bdlat_TypeCategoryUtil::accessByCategory(object, proxy);
    return stream;
}

template <class TYPE, class ANY_CATEGORY>
inline
bsl::ostream& TypesPrintUtil_Imp::printHex(bsl::ostream&         stream,
                                           const TYPE&,
                                           const EncoderOptions *,
                                           ANY_CATEGORY)
{
    BSLS_ASSERT_SAFE(!"Unsupported operation!");

    stream.setstate(bsl::ios_base::failbit);

    // Note: 'printHex' for 'bsl::string' and 'bsl::vector<char>' is inside the
    //       CPP file.

    return stream;
}

// LIST FUNCTIONS

template <class TYPE>
bsl::ostream& TypesPrintUtil_Imp::printList(
                                     bsl::ostream&              stream,
                                     const TYPE&                object,
                                     const EncoderOptions      *encoderOptions,
                                     bdlat_TypeCategory::Array)
{
    int size = (int)bdlat_ArrayFunctions::size(object);

    if (0 == size) {
        return stream;                                                // RETURN
    }

    TypesPrintUtil_printDefaultProxy proxy = { &stream, encoderOptions };

    bdlat_ArrayFunctions::accessElement(object, proxy, 0);

    for (int i = 1; i < size; ++i) {
        stream << " ";
        bdlat_ArrayFunctions::accessElement(object, proxy, i);
    }

    return stream;
}

template <class TYPE>
inline
bsl::ostream& TypesPrintUtil_Imp::printList(
                               bsl::ostream&                    stream,
                               const TYPE&                      object,
                               const EncoderOptions            *encoderOptions,
                               bdlat_TypeCategory::DynamicType)
{
    TypesPrintUtil_Imp_printListProxy proxy = { &stream, encoderOptions };
    bdlat_TypeCategoryUtil::accessByCategory(object, proxy);
    return stream;
}

template <class TYPE, class ANY_CATEGORY>
inline
bsl::ostream& TypesPrintUtil_Imp::printList(bsl::ostream&         stream,
                                            const TYPE&,
                                            const EncoderOptions *,
                                            ANY_CATEGORY)
{
    BSLS_ASSERT_SAFE(!"Unsupported operation!");

    stream.setstate(bsl::ios_base::failbit);

    return stream;
}
// TEXT FUNCTIONS

template <class TYPE>
inline
bsl::ostream& TypesPrintUtil_Imp::printText(
                               bsl::ostream&                    stream,
                               const TYPE&                      object,
                               const EncoderOptions            *encoderOptions,
                               bdlat_TypeCategory::Enumeration)
{
    bsl::string stringVal;

    bdlat_EnumFunctions::toString(&stringVal, object);

    return TypesPrintUtil::printText(stream, stringVal, encoderOptions);
}

template <class TYPE>
inline
bsl::ostream& TypesPrintUtil_Imp::printText(
                            bsl::ostream&                       stream,
                            const TYPE&                         object,
                            const EncoderOptions               *encoderOptions,
                            bdlat_TypeCategory::CustomizedType)
{
    return TypesPrintUtil::printText(
                      stream,
                      bdlat_CustomizedTypeFunctions::convertToBaseType(object),
                      encoderOptions);
}

template <class TYPE>
inline
bsl::ostream& TypesPrintUtil_Imp::printText(
                               bsl::ostream&                    stream,
                               const TYPE&                      object,
                               const EncoderOptions            *encoderOptions,
                               bdlat_TypeCategory::DynamicType)
{
    TypesPrintUtil_Imp_printTextProxy proxy = { &stream, encoderOptions };
    bdlat_TypeCategoryUtil::accessByCategory(object, proxy);
    return stream;
}

template <class TYPE, class ANY_CATEGORY>
inline
bsl::ostream& TypesPrintUtil_Imp::printText(bsl::ostream&         stream,
                                            const TYPE&,
                                            const EncoderOptions *,
                                            ANY_CATEGORY)
{
    BSLS_ASSERT_SAFE(!"Unsupported operation!");

    stream.setstate(bsl::ios_base::failbit);

    return stream;
}

inline
bsl::ostream& TypesPrintUtil_Imp::printText(bsl::ostream&               stream,
                                            const bool&                 object,
                                            const EncoderOptions       *,
                                            bdlat_TypeCategory::Simple)
{
    return stream << (object ? "true" : "false");
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
