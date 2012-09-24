// bdem_beruniversaltagnumber.h                                       -*-C++-*-
#ifndef INCLUDED_BDEM_BERUNIVERSALTAGNUMBER
#define INCLUDED_BDEM_BERUNIVERSALTAGNUMBER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Enumerate the set of 'BER' universal tag numbers.
//
//@CLASSES:
// bdem_BerUniversalTagNumber: namespace for enumerating universal tag numbers
//
//@SEE_ALSO:
// bdem_berencoder, bdem_berdecoder
// http://www.itu.int/ITU-T/studygroups/com17/languages/X.680-0207.pdf
//
//@AUTHOR: Shezan Baig (sbaig)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION: This component provides a namespace for the 'enum' type
// 'bdem_BerUniversalTagNumber::Value'.  'Value' enumerates the set of 'BER'
// universal tag numbers used by the 'BER' encoder and decoder.  The universal
// tag numbers are defined in the X.680 standard, in section 8.
//
// Templatized functions are provided to support the streaming in and out of
// the enumerated values.  Input streams must be compatible with the
// 'bdex_InStream' protocol, and output streams must be compatible with the
// 'bdex_OutStream' protocol or else convertible to a standard 'ostream'; in
// the latter case the value is written as its corresponding string
// representation.  In addition, this component supports functions that
// convert the 'Value' enumerations to a well-defined ASCII representation.
//
// This component also provides a function that returns the universal tag
// number for an object with formatting mode, according to the following table:
//..
//  C++ Type                           Formatting Mode  Universal Tag Number
//  --------                           ---------------  --------------------
//  bool                               DEFAULT          BDEM_BER_BOOL
//                                     DEC              BDEM_BER_BOOL
//                                     TEXT             BDEM_BER_BOOL
//  char                               DEFAULT          BDEM_BER_INT
//                                     DEC              BDEM_BER_INT
//                                     TEXT             BDEM_BER_UTF8_STRING
//  unsigned char                      DEFAULT          BDEM_BER_INT
//                                     DEC              BDEM_BER_INT
//  [unsigned] short                   DEFAULT          BDEM_BER_INT
//                                     DEC              BDEM_BER_INT
//  [unsigned] int                     DEFAULT          BDEM_BER_INT
//                                     DEC              BDEM_BER_INT
//  [unsigned] long                    DEFAULT          BDEM_BER_INT
//                                     DEC              BDEM_BER_INT
//  bsls_Types::[Uint64|Int64]         DEFAULT          BDEM_BER_INT
//                                     DEC              BDEM_BER_INT
//  float                              DEFAULT          BDEM_BER_REAL
//  double                             DEFAULT          BDEM_BER_REAL
//  bsl::string                        DEFAULT          BDEM_BER_UTF8_STRING
//                                     TEXT             BDEM_BER_UTF8_STRING
//                                     BASE64           BDEM_BER_OCTET_STRING
//                                     HEX              BDEM_BER_OCTET_STRING
//  bdet_Date                          DEFAULT          BDEM_BER_VISIBLE_STRING
//  bdet_DateTz                        DEFAULT          BDEM_BER_VISIBLE_STRING
//  bdet_Datetime                      DEFAULT          BDEM_BER_VISIBLE_STRING
//  bdet_DateTimeTz                    DEFAULT          BDEM_BER_VISIBLE_STRING
//  bdet_Time                          DEFAULT          BDEM_BER_VISIBLE_STRING
//  bdet_TimeTz                        DEFAULT          BDEM_BER_VISIBLE_STRING
//  bsl::vector<char>                  DEFAULT          BDEM_BER_OCTET_STRING
//                                     BASE64           BDEM_BER_OCTET_STRING
//                                     HEX              BDEM_BER_OCTET_STRING
//                                     TEXT             BDEM_BER_UTF8_STRING
//..
// If the object is not one of these types, then the universal tag number is
// selected based on the object's type category, as follows:
//..
//  Category             Universal Tag Number
//  --------             --------------------
//  CustomizedType       Use universal tag number of the base type.
//  Enumeration          'BDEM_BER_ENUMERATION' when formatting mode is either
//                       'DEFAULT', 'DEC', or 'TEXT'.
//  Sequence             'BDEM_BER_SEQUENCE' when formatting mode is 'DEFAULT'.
//  Choice               'BDEM_BER_SEQUENCE' when formatting mode is 'DEFAULT'.
//  Array                'BDEM_BER_SEQUENCE' when formatting mode is 'DEFAULT'.
//..
// The behavior is undefined if the object does not fall into one the above
// categories or formatting modes.
//
///Usage 1
///-------
// The following snippets of code provide a simple illustration of
// 'bdem_BerUniversalTagNumber' operation.
//
// First, create a variable 'tagNumber' of type
// 'bdem_BerUniversalTagNumber::Value' and initialize it to the value
// 'bdem_BerUniversalTagNumber::BDEM_BER_INT':
//..
//  bdem_BerUniversalTagNumber::Value tagNumber
//                                  = bdem_BerUniversalTagNumber::BDEM_BER_INT;
//..
// Next, store its representation in a variable 'rep' of type 'const char*':
//..
//  const char *rep = bdem_BerUniversalTagNumber::toString(tagNumber);
//  assert(0 == strcmp(rep, "INT"));
//..
// Finally, print the value of 'tagNumber' to 'bsl::cout':
//..
//  bsl::cout << tagNumber << bsl::endl;
//..
// This statement produces the following output on 'bsl::cout':
//..
//  INT
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEM_BERENCODEROPTIONS
#include <bdem_berencoderoptions.h>
#endif

#ifndef INCLUDED_BDEAT_CUSTOMIZEDTYPEFUNCTIONS
#include <bdeat_customizedtypefunctions.h>
#endif

#ifndef INCLUDED_BDEAT_FORMATTINGMODE
#include <bdeat_formattingmode.h>
#endif

#ifndef INCLUDED_BDEAT_NULLABLEVALUEFUNCTIONS
#include <bdeat_nullablevaluefunctions.h>
#endif

#ifndef INCLUDED_BDEAT_TYPECATEGORY
#include <bdeat_typecategory.h>
#endif

#ifndef INCLUDED_BDEX_INSTREAMFUNCTIONS
#include <bdex_instreamfunctions.h>
#endif

#ifndef INCLUDED_BDEX_VERSIONFUNCTIONS
#include <bdex_versionfunctions.h>
#endif

#ifndef INCLUDED_BDEX_OUTSTREAMFUNCTIONS
#include <bdex_outstreamfunctions.h>
#endif

#ifndef INCLUDED_BDEUT_VARIANT
#include <bdeut_variant.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

                     // =================================
                     // struct bdem_BerUniversalTagNumber
                     // =================================

struct bdem_BerUniversalTagNumber {
    // This 'struct' contains an enumeration of the universal tag numbers for
    // 'BER' encoding.

    // TYPES
    enum Value {
        // Universal tag numbers as per X.680, section 8

        BDEM_BER_BOOL           = 0x01,  // ( 1) Boolean type
        BDEM_BER_INT            = 0x02,  // ( 2) Integer type
        BDEM_BER_OCTET_STRING   = 0x04,  // ( 4) Octet string type
        BDEM_BER_REAL           = 0x09,  // ( 9) Real type
        BDEM_BER_ENUMERATION    = 0x0A,  // (10) Enumerated type
        BDEM_BER_UTF8_STRING    = 0x0C,  // (12) UTF8 string type
        BDEM_BER_SEQUENCE       = 0x10,  // (16) Sequence and Sequence-of types
        BDEM_BER_VISIBLE_STRING = 0x1A   // (26) VisibleString type (7-bit
                                         //      ASCII)
    };

    // CONSTANTS
    enum {
        BDEM_LENGTH = 8  // the number of enumerations in the 'Value'
                         // enumeration

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , NUM_ENUMERATORS = BDEM_LENGTH
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static const char *toString(Value value);
        // Return the string representation exactly matching the enumerator
        // name corresponding to the specified enumeration 'value'.

    static int fromString(Value        *result,
                          const char   *string,
                          int           stringLength);
        // Load into the specified 'result' the enumerator matching the
        // specified 'string' of the specified 'stringLength'.  Return 0 on
        // success, and a non-zero value with no effect on 'result' otherwise
        // (i.e., 'string' does not match any enumerator).

    static int fromInt(Value *result, int number);
        // Load into the specified 'result' the enumerator matching the
        // specified 'number'.  Return 0 on success, and a non-zero value with
        // no effect on 'result' otherwise (i.e., 'number' does not match any
        // enumerator).

    template <class STREAM>
    static STREAM& bdexStreamIn(STREAM&  stream,
                                Value&   value,
                                int      version);
        // Assign to the specified 'value' the value read from the specified
        // input 'stream' using the specified 'version' format and return a
        // reference to the modifiable 'stream'.  If 'stream' is initially
        // invalid, this operation has no effect.  If 'stream' becomes invalid
        // during this operation, the 'value' is valid, but its value is
        // undefined.  If the specified 'version' is not supported, 'stream' is
        // marked invalid, but 'value' is unaltered.  Note that no version is
        // read from 'stream'.  (See the package-group-level documentation for
        // more information on 'bdex' streaming of container types.)

    static bsl::ostream& print(bsl::ostream& stream, Value value);
        // Write to the specified 'stream' the string representation of
        // the specified enumeration 'value'.  Return a reference to
        // the modifiable 'stream'.

    template <typename TYPE>
    static Value select(const TYPE&  object,
                        int          formattingMode,
                        int         *alternateTag);
        // Return the universal tag number for the specified 'object' with the
        // specified 'formattingMode' and load into the specified
        // 'alternateTag' any alternative tag numbers corresponding to
        // 'object'.  The behavior is undefined if the type category of
        // 'object' and the 'formattingMode' do not permit a universal tag
        // number (see component-level documentation for allowed type
        // categories and formatting modes).  Note that if an alternate tag
        // number does not exist for 'object' then 'alternateTag' is not
        // modified.

    template <typename TYPE>
    static Value select(const TYPE&                   object,
                        int                           formattingMode,
                        const bdem_BerEncoderOptions *options);
        // Return the universal tag number for the specified 'object' with the
        // specified 'formattingMode' and using the specified 'options'.  The
        // behavior is undefined if the type category of 'object' and the
        // 'formattingMode' do not permit a universal tag number (see
        // component-level documentation for allowed type categories and
        // formatting modes).

    template <class STREAM>
    static STREAM& bdexStreamOut(STREAM&  stream,
                                 Value    value,
                                 int      version);
        // Write the specified 'value' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.  Optionally specify
        // an explicit 'version' format; by default, the maximum supported
        // version is written to 'stream' and used as the format.  If 'version'
        // is specified, that format is used but *not* written to 'stream'.  If
        // 'version' is not supported, 'stream' is left unmodified.  (See the
        // package-group-level documentation for more information on 'bdex'
        // streaming of container types).
};

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream&                     stream,
                         bdem_BerUniversalTagNumber::Value rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

// ---  Anything below this line is implementation specific.  Do not use.  ----

                 // =========================================
                 // struct bdem_BerUniversalTagNumber_ImpUtil
                 // =========================================

class bdem_BerUniversalTagNumber_Imp {
    // Component-private class.  Do not use.
    // This struct contains implementation details for this component.

    typedef bdeat_FormattingMode              FMode;   // shorthand
    typedef bdem_BerUniversalTagNumber::Value TagVal;  // shorthand

    int                           d_formattingMode;
    const bdem_BerEncoderOptions *d_options_p;         // options
                                                       // (held, not owned)
    int                           d_alternateTag;      // alternate tag

    // PRIVATE MANIPULATORS
    TagVal selectForDateAndTimeTypes();
        // Return the universal tag number for date and time types and load
        // into the internal 'alternateTag' data member the any alternative
        // tag numbers corresponding to those types.

  public:
    bdem_BerUniversalTagNumber_Imp(int                           fm,
                                   const bdem_BerEncoderOptions *options = 0)
    : d_formattingMode(fm)
    , d_options_p(options)
    , d_alternateTag(-1)
    {
    }

    // --- By Type ------------------------------------------------------------

    TagVal select(const bool& object, bdeat_TypeCategory::Simple);

    TagVal select(const char& object, bdeat_TypeCategory::Simple);

    TagVal select(const signed char& object, bdeat_TypeCategory::Simple);

    TagVal select(const unsigned char& object, bdeat_TypeCategory::Simple);

    TagVal select(const short& object, bdeat_TypeCategory::Simple);

    TagVal select(const unsigned short& object, bdeat_TypeCategory::Simple);

    TagVal select(const int& object, bdeat_TypeCategory::Simple);

    TagVal select(const unsigned int& object, bdeat_TypeCategory::Simple);

    TagVal select(const long& object, bdeat_TypeCategory::Simple);

    TagVal select(const unsigned long& object, bdeat_TypeCategory::Simple);

    TagVal select(const bsls_Types::Int64& object, bdeat_TypeCategory::Simple);

    TagVal select(const bsls_Types::Uint64& object,
                  bdeat_TypeCategory::Simple);

    TagVal select(const float& object, bdeat_TypeCategory::Simple);

    TagVal select(const double& object, bdeat_TypeCategory::Simple);

    TagVal select(const bsl::string& object, bdeat_TypeCategory::Simple);

    TagVal select(const bdet_Date& object, bdeat_TypeCategory::Simple);

    TagVal select(const bdet_DateTz& object, bdeat_TypeCategory::Simple);

    TagVal select(const bdet_Datetime& object, bdeat_TypeCategory::Simple);

    TagVal select(const bdet_DatetimeTz& object, bdeat_TypeCategory::Simple);

    TagVal select(const bdet_Time& object, bdeat_TypeCategory::Simple);

    TagVal select(const bdet_TimeTz& object, bdeat_TypeCategory::Simple);

    template <typename TYPE, typename TYPETZ>
    TagVal select(const bdeut_Variant2<TYPE, TYPETZ>& object,
                  bdeat_TypeCategory::Simple);

    TagVal select(const bsl::vector<char>& object, bdeat_TypeCategory::Array);

    // --- By Category --------------------------------------------------------

    template <typename TYPE>
    TagVal select(const TYPE& object, bdeat_TypeCategory::CustomizedType);

    template <typename TYPE>
    TagVal select(const TYPE& object, bdeat_TypeCategory::Enumeration);

    template <typename TYPE>
    TagVal select(const TYPE& object, bdeat_TypeCategory::Array);

    template <typename TYPE>
    TagVal select(const TYPE& object, bdeat_TypeCategory::Sequence);

    template <typename TYPE>
    TagVal select(const TYPE& object, bdeat_TypeCategory::Choice);

    template <typename TYPE>
    TagVal select(const TYPE& object, bdeat_TypeCategory::NullableValue);

    template <typename TYPE, typename ANY_CATEGORY>
    TagVal select(const TYPE& object, ANY_CATEGORY);

    // FUNCTOR OPERATORS
    template <typename TYPE>
    inline
    TagVal operator()(const TYPE&, bslmf_Nil)
    {
        BSLS_ASSERT(0 && "Invalid type category");
        return TagVal(-1);
    }

    template <typename TYPE, typename ANY_CATEGORY>
    inline
    TagVal operator()(const TYPE& object, ANY_CATEGORY category)
    {
        return bdem_BerUniversalTagNumber_Imp::select(object, category);
    }

    int alternateTag() {
        return d_alternateTag;
    }
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                     // ---------------------------------
                     // struct bdem_BerUniversalTagNumber
                     // ---------------------------------

// FORWARD DECLARATIONS

class bdet_Date;
class bdet_DateTz;
class bdet_Datetime;
class bdet_DatetimeTz;
class bdet_Time;
class bdet_TimeTz;

// CLASS METHODS

inline
int bdem_BerUniversalTagNumber::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

inline
int bdem_BerUniversalTagNumber::fromInt(
                                     bdem_BerUniversalTagNumber::Value *result,
                                     int                                number)
{
    enum { BDEM_SUCCESS = 0, BDEM_NOT_FOUND = 1 };

    switch (number) {
      case BDEM_BER_BOOL:
      case BDEM_BER_INT:
      case BDEM_BER_OCTET_STRING:
      case BDEM_BER_REAL:
      case BDEM_BER_ENUMERATION:
      case BDEM_BER_UTF8_STRING:
      case BDEM_BER_SEQUENCE:
      case BDEM_BER_VISIBLE_STRING:
        *result = static_cast<bdem_BerUniversalTagNumber::Value>(number);
        return BDEM_SUCCESS;                                         // RETURN
      default:
        return BDEM_NOT_FOUND;
    }
}

template <class STREAM>
inline
STREAM& bdem_BerUniversalTagNumber::bdexStreamIn(
                                    STREAM&                            stream,
                                    bdem_BerUniversalTagNumber::Value& value,
                                    int                                version)
{
    switch(version) {
      case 1: {
        int readValue;
        stream.getInt32(readValue);
        if (stream) {
            if (fromInt(&value, readValue)) {
               stream.invalidate();   // bad value in stream
            }
        }
      } break;
      default: {
        stream.invalidate();          // unrecognized version number
      } break;
    }
    return stream;
}

inline
bsl::ostream& bdem_BerUniversalTagNumber::print(
                                      bsl::ostream&                     stream,
                                      bdem_BerUniversalTagNumber::Value value)
{
    return stream << toString(value);
}

template <typename TYPE>
inline
bdem_BerUniversalTagNumber::Value
bdem_BerUniversalTagNumber::select(
                                  const TYPE&                   object,
                                  int                           formattingMode,
                                  const bdem_BerEncoderOptions *options)
{
    typedef typename bdeat_TypeCategory::Select<TYPE>::Type TypeCategory;

    bdem_BerUniversalTagNumber_Imp imp(formattingMode, options);
    int retVal = bdeat_TypeCategoryUtil::accessByCategory(object, imp);
    return (bdem_BerUniversalTagNumber::Value) retVal;
}

template <typename TYPE>
inline
bdem_BerUniversalTagNumber::Value
bdem_BerUniversalTagNumber::select(const TYPE&  object,
                                   int          formattingMode,
                                   int         *alternateTag)
{
    typedef typename bdeat_TypeCategory::Select<TYPE>::Type TypeCategory;

    bdem_BerUniversalTagNumber_Imp imp(formattingMode);
    int retVal = bdeat_TypeCategoryUtil::accessByCategory(object, imp);

    const int tag = imp.alternateTag();
    if (-1 != tag) {
        *alternateTag = tag;
    }

    return (bdem_BerUniversalTagNumber::Value) retVal;
}

template <class STREAM>
inline
STREAM& bdem_BerUniversalTagNumber::bdexStreamOut(
                                     STREAM&                           stream,
                                     bdem_BerUniversalTagNumber::Value value,
                                     int                               version)
{
    switch (version) {
      case 1: {
        stream.putInt32(value);  // Write the value as an int
      } break;
    }
    return stream;
}

// FREE OPERATORS

inline
bsl::ostream& operator<<(bsl::ostream&                     stream,
                         bdem_BerUniversalTagNumber::Value rhs)
{
    return bdem_BerUniversalTagNumber::print(stream, rhs);
}

                 // -----------------------------------------
                 // struct bdem_BerUniversalTagNumber_Imp
                 // -----------------------------------------

// PRIVATE MANIPULATORS
inline
bdem_BerUniversalTagNumber::Value
bdem_BerUniversalTagNumber_Imp::selectForDateAndTimeTypes()
{
    BSLS_ASSERT_SAFE(
          FMode::BDEAT_DEFAULT == (d_formattingMode & FMode::BDEAT_TYPE_MASK));

    if (d_options_p) {
        return d_options_p->encodeDateAndTimeTypesAsBinary()
             ? bdem_BerUniversalTagNumber::BDEM_BER_OCTET_STRING
             : bdem_BerUniversalTagNumber::BDEM_BER_VISIBLE_STRING;
    }
    else {
        d_alternateTag = bdem_BerUniversalTagNumber::BDEM_BER_OCTET_STRING;

        return bdem_BerUniversalTagNumber::BDEM_BER_VISIBLE_STRING;
    }
}

// --- By Type ----------------------------------------------------------------

inline
bdem_BerUniversalTagNumber::Value
bdem_BerUniversalTagNumber_Imp::select(const bool&, bdeat_TypeCategory::Simple)
{
    BSLS_ASSERT_SAFE(
          FMode::BDEAT_DEFAULT == (d_formattingMode & FMode::BDEAT_TYPE_MASK)
       || FMode::BDEAT_DEC     == (d_formattingMode & FMode::BDEAT_TYPE_MASK)
       || FMode::BDEAT_TEXT    == (d_formattingMode & FMode::BDEAT_TYPE_MASK));

    return bdem_BerUniversalTagNumber::BDEM_BER_BOOL;
}

inline
bdem_BerUniversalTagNumber::Value
bdem_BerUniversalTagNumber_Imp::select(const char&, bdeat_TypeCategory::Simple)
{
    if (FMode::BDEAT_TEXT == (d_formattingMode & FMode::BDEAT_TYPE_MASK)) {
        return bdem_BerUniversalTagNumber::BDEM_BER_UTF8_STRING;
    }

    BSLS_ASSERT_SAFE(
          FMode::BDEAT_DEFAULT == (d_formattingMode & FMode::BDEAT_TYPE_MASK)
       || FMode::BDEAT_DEC     == (d_formattingMode & FMode::BDEAT_TYPE_MASK));

    return bdem_BerUniversalTagNumber::BDEM_BER_INT;
}

inline
bdem_BerUniversalTagNumber::Value
bdem_BerUniversalTagNumber_Imp::select(const signed char&,
                                       bdeat_TypeCategory::Simple)
{
    if (FMode::BDEAT_TEXT == (d_formattingMode & FMode::BDEAT_TYPE_MASK)) {
        return bdem_BerUniversalTagNumber::BDEM_BER_UTF8_STRING;
    }

    BSLS_ASSERT_SAFE(
          FMode::BDEAT_DEFAULT == (d_formattingMode & FMode::BDEAT_TYPE_MASK)
       || FMode::BDEAT_DEC     == (d_formattingMode & FMode::BDEAT_TYPE_MASK));

    return bdem_BerUniversalTagNumber::BDEM_BER_INT;
}

inline
bdem_BerUniversalTagNumber::Value
bdem_BerUniversalTagNumber_Imp::select(const unsigned char&,
                                       bdeat_TypeCategory::Simple)
{
    BSLS_ASSERT_SAFE(
          FMode::BDEAT_DEFAULT == (d_formattingMode & FMode::BDEAT_TYPE_MASK)
       || FMode::BDEAT_DEC     == (d_formattingMode & FMode::BDEAT_TYPE_MASK));

    return bdem_BerUniversalTagNumber::BDEM_BER_INT;
}

inline
bdem_BerUniversalTagNumber::Value
bdem_BerUniversalTagNumber_Imp::select(const short&,
                                       bdeat_TypeCategory::Simple)
{
    BSLS_ASSERT_SAFE(
          FMode::BDEAT_DEFAULT == (d_formattingMode & FMode::BDEAT_TYPE_MASK)
       || FMode::BDEAT_DEC     == (d_formattingMode & FMode::BDEAT_TYPE_MASK));

    return bdem_BerUniversalTagNumber::BDEM_BER_INT;
}

inline
bdem_BerUniversalTagNumber::Value
bdem_BerUniversalTagNumber_Imp::select(const unsigned short&,
                                       bdeat_TypeCategory::Simple)
{
    BSLS_ASSERT_SAFE(
          FMode::BDEAT_DEFAULT == (d_formattingMode & FMode::BDEAT_TYPE_MASK)
       || FMode::BDEAT_DEC     == (d_formattingMode & FMode::BDEAT_TYPE_MASK));

    return bdem_BerUniversalTagNumber::BDEM_BER_INT;
}

inline
bdem_BerUniversalTagNumber::Value
bdem_BerUniversalTagNumber_Imp::select(const int&, bdeat_TypeCategory::Simple)
{
    BSLS_ASSERT_SAFE(
          FMode::BDEAT_DEFAULT == (d_formattingMode & FMode::BDEAT_TYPE_MASK)
       || FMode::BDEAT_DEC     == (d_formattingMode & FMode::BDEAT_TYPE_MASK));

    return bdem_BerUniversalTagNumber::BDEM_BER_INT;
}

inline
bdem_BerUniversalTagNumber::Value
bdem_BerUniversalTagNumber_Imp::select(const unsigned int&,
                                       bdeat_TypeCategory::Simple)
{
    BSLS_ASSERT_SAFE(
          FMode::BDEAT_DEFAULT == (d_formattingMode & FMode::BDEAT_TYPE_MASK)
       || FMode::BDEAT_DEC     == (d_formattingMode & FMode::BDEAT_TYPE_MASK));

    return bdem_BerUniversalTagNumber::BDEM_BER_INT;
}

inline
bdem_BerUniversalTagNumber::Value
bdem_BerUniversalTagNumber_Imp::select(const long&,
                                       bdeat_TypeCategory::Simple)
{
    BSLS_ASSERT_SAFE(
          FMode::BDEAT_DEFAULT == (d_formattingMode & FMode::BDEAT_TYPE_MASK)
       || FMode::BDEAT_DEC     == (d_formattingMode & FMode::BDEAT_TYPE_MASK));

    return bdem_BerUniversalTagNumber::BDEM_BER_INT;
}

inline
bdem_BerUniversalTagNumber::Value
bdem_BerUniversalTagNumber_Imp::select(const unsigned long&,
                                       bdeat_TypeCategory::Simple)
{
    BSLS_ASSERT_SAFE(
          FMode::BDEAT_DEFAULT == (d_formattingMode & FMode::BDEAT_TYPE_MASK)
       || FMode::BDEAT_DEC     == (d_formattingMode & FMode::BDEAT_TYPE_MASK));

    return bdem_BerUniversalTagNumber::BDEM_BER_INT;
}

inline
bdem_BerUniversalTagNumber::Value
bdem_BerUniversalTagNumber_Imp::select(const bsls_Types::Int64&,
                                       bdeat_TypeCategory::Simple)
{
    BSLS_ASSERT_SAFE(
          FMode::BDEAT_DEFAULT == (d_formattingMode & FMode::BDEAT_TYPE_MASK)
       || FMode::BDEAT_DEC     == (d_formattingMode & FMode::BDEAT_TYPE_MASK));

    return bdem_BerUniversalTagNumber::BDEM_BER_INT;
}

inline
bdem_BerUniversalTagNumber::Value
bdem_BerUniversalTagNumber_Imp::select(const bsls_Types::Uint64&,
                                       bdeat_TypeCategory::Simple)
{
    BSLS_ASSERT_SAFE(
          FMode::BDEAT_DEFAULT == (d_formattingMode & FMode::BDEAT_TYPE_MASK)
       || FMode::BDEAT_DEC     == (d_formattingMode & FMode::BDEAT_TYPE_MASK));

    return bdem_BerUniversalTagNumber::BDEM_BER_INT;
}

inline
bdem_BerUniversalTagNumber::Value
bdem_BerUniversalTagNumber_Imp::select(const float&,
                                       bdeat_TypeCategory::Simple)
{
    BSLS_ASSERT_SAFE(
          FMode::BDEAT_DEFAULT == (d_formattingMode & FMode::BDEAT_TYPE_MASK)
       || FMode::BDEAT_DEC     == (d_formattingMode & FMode::BDEAT_TYPE_MASK));

    return bdem_BerUniversalTagNumber::BDEM_BER_REAL;
}

inline
bdem_BerUniversalTagNumber::Value
bdem_BerUniversalTagNumber_Imp::select(const double&,
                                       bdeat_TypeCategory::Simple)
{
    BSLS_ASSERT_SAFE(
          FMode::BDEAT_DEFAULT == (d_formattingMode & FMode::BDEAT_TYPE_MASK)
       || FMode::BDEAT_DEC     == (d_formattingMode & FMode::BDEAT_TYPE_MASK));

    return bdem_BerUniversalTagNumber::BDEM_BER_REAL;
}

inline
bdem_BerUniversalTagNumber::Value
bdem_BerUniversalTagNumber_Imp::select(const bsl::string&,
                                       bdeat_TypeCategory::Simple)
{
    if (FMode::BDEAT_BASE64 == (d_formattingMode & FMode::BDEAT_TYPE_MASK)
     || FMode::BDEAT_HEX    == (d_formattingMode & FMode::BDEAT_TYPE_MASK)) {
        return bdem_BerUniversalTagNumber::BDEM_BER_OCTET_STRING;
    }

    BSLS_ASSERT_SAFE(
          FMode::BDEAT_DEFAULT == (d_formattingMode & FMode::BDEAT_TYPE_MASK)
       || FMode::BDEAT_TEXT    == (d_formattingMode & FMode::BDEAT_TYPE_MASK));

    return bdem_BerUniversalTagNumber::BDEM_BER_UTF8_STRING;
}

inline
bdem_BerUniversalTagNumber::Value
bdem_BerUniversalTagNumber_Imp::select(const bdet_Date&,
                                       bdeat_TypeCategory::Simple)
{
    return selectForDateAndTimeTypes();
}

inline
bdem_BerUniversalTagNumber::Value
bdem_BerUniversalTagNumber_Imp::select(const bdet_DateTz&,
                                       bdeat_TypeCategory::Simple)
{
    return selectForDateAndTimeTypes();
}

inline
bdem_BerUniversalTagNumber::Value
bdem_BerUniversalTagNumber_Imp::select(const bdet_Datetime&,
                                       bdeat_TypeCategory::Simple)
{
    return selectForDateAndTimeTypes();
}

inline
bdem_BerUniversalTagNumber::Value
bdem_BerUniversalTagNumber_Imp::select(const bdet_DatetimeTz&,
                                       bdeat_TypeCategory::Simple)
{
    return selectForDateAndTimeTypes();
}

inline
bdem_BerUniversalTagNumber::Value
bdem_BerUniversalTagNumber_Imp::select(const bdet_Time&,
                                       bdeat_TypeCategory::Simple)
{
    return selectForDateAndTimeTypes();
}

inline
bdem_BerUniversalTagNumber::Value
bdem_BerUniversalTagNumber_Imp::select(const bdet_TimeTz&,
                                       bdeat_TypeCategory::Simple)
{
    return selectForDateAndTimeTypes();
}

template <typename TYPE, typename TYPETZ>
inline
bdem_BerUniversalTagNumber::Value
bdem_BerUniversalTagNumber_Imp::select(const bdeut_Variant2<TYPE, TYPETZ>&,
                                       bdeat_TypeCategory::Simple)
{
    BSLMF_ASSERT((bslmf_IsSame<bdet_Date, TYPE>::VALUE
               && bslmf_IsSame<bdet_DateTz, TYPETZ>::VALUE)
              || (bslmf_IsSame<bdet_Time, TYPE>::VALUE
               && bslmf_IsSame<bdet_TimeTz, TYPETZ>::VALUE)
              || (bslmf_IsSame<bdet_Datetime, TYPE>::VALUE
               && bslmf_IsSame<bdet_DatetimeTz, TYPETZ>::VALUE));

    return selectForDateAndTimeTypes();
}

inline
bdem_BerUniversalTagNumber::Value
bdem_BerUniversalTagNumber_Imp::select(const bsl::vector<char>&,
                                       bdeat_TypeCategory::Array)
{
    if (FMode::BDEAT_TEXT == (d_formattingMode & FMode::BDEAT_TYPE_MASK)) {
        return bdem_BerUniversalTagNumber::BDEM_BER_UTF8_STRING;
    }

    BSLS_ASSERT_SAFE(
          FMode::BDEAT_DEFAULT == (d_formattingMode & FMode::BDEAT_TYPE_MASK)
       || FMode::BDEAT_BASE64  == (d_formattingMode & FMode::BDEAT_TYPE_MASK)
       || FMode::BDEAT_HEX     == (d_formattingMode & FMode::BDEAT_TYPE_MASK));

    return bdem_BerUniversalTagNumber::BDEM_BER_OCTET_STRING;
}

// --- By Category ------------------------------------------------------------

template <typename TYPE>
inline
bdem_BerUniversalTagNumber::Value
bdem_BerUniversalTagNumber_Imp::select(const TYPE&,
                                       bdeat_TypeCategory::CustomizedType)
{
    typedef typename
    bdeat_CustomizedTypeFunctions::BaseType<TYPE>::Type BaseType;

    // The object referenced by the following pointer is never used.  A null
    // pointer was chosen to force a SEGV in case of inadvertent use.
    // Dereferencing null is technically illegal, but will work in this case
    // because the value is never retrieved from memory.
    BaseType *tmp = 0;
    return (TagVal) bdeat_TypeCategoryUtil::accessByCategory(*tmp, *this);
}

template <typename TYPE>
inline
bdem_BerUniversalTagNumber::Value
bdem_BerUniversalTagNumber_Imp::select(const TYPE&,
                                       bdeat_TypeCategory::Enumeration)
{
    BSLS_ASSERT_SAFE(
          FMode::BDEAT_DEFAULT == (d_formattingMode & FMode::BDEAT_TYPE_MASK)
       || FMode::BDEAT_DEC     == (d_formattingMode & FMode::BDEAT_TYPE_MASK)
       || FMode::BDEAT_TEXT    == (d_formattingMode & FMode::BDEAT_TYPE_MASK));

    return bdem_BerUniversalTagNumber::BDEM_BER_ENUMERATION;
}

template <typename TYPE>
inline
bdem_BerUniversalTagNumber::Value
bdem_BerUniversalTagNumber_Imp::select(const TYPE&,
                                       bdeat_TypeCategory::Array)
{
    BSLS_ASSERT_SAFE(
          FMode::BDEAT_DEFAULT == (d_formattingMode & FMode::BDEAT_TYPE_MASK));

    return bdem_BerUniversalTagNumber::BDEM_BER_SEQUENCE;
}

template <typename TYPE>
inline
bdem_BerUniversalTagNumber::Value
bdem_BerUniversalTagNumber_Imp::select(const TYPE&,
                                       bdeat_TypeCategory::Sequence)
{
    BSLS_ASSERT_SAFE(
          FMode::BDEAT_DEFAULT == (d_formattingMode & FMode::BDEAT_TYPE_MASK));

    return bdem_BerUniversalTagNumber::BDEM_BER_SEQUENCE;
}

template <typename TYPE>
inline
bdem_BerUniversalTagNumber::Value
bdem_BerUniversalTagNumber_Imp::select(const TYPE&,
                                       bdeat_TypeCategory::Choice)
{
    BSLS_ASSERT_SAFE(
          FMode::BDEAT_DEFAULT == (d_formattingMode & FMode::BDEAT_TYPE_MASK));

    // According to X.694 (clause 20.4), an XML choice element is encoded
    // as a sequence with 1 element.

    return bdem_BerUniversalTagNumber::BDEM_BER_SEQUENCE;
}

template <typename TYPE>
inline
bdem_BerUniversalTagNumber::Value
bdem_BerUniversalTagNumber_Imp::select(const TYPE&,
                                       bdeat_TypeCategory::NullableValue)
{
    if (d_formattingMode & FMode::BDEAT_NILLABLE) {
        return bdem_BerUniversalTagNumber::BDEM_BER_SEQUENCE;
    }

    // If got here, then value is nullable, but not nillable.
    typedef typename
    bdeat_NullableValueFunctions::ValueType<TYPE>::Type ValueType;

    // The object referenced by the following pointer is never used.  A null
    // pointer was chosen to force a SEGV in case of inadvertent use.
    // Dereferencing null is technically illegal, but will work in this case
    // because the value is never retrieved from memory.
    ValueType *tmp = 0;
    return (TagVal) bdeat_TypeCategoryUtil::accessByCategory(*tmp, *this);
}

template <typename TYPE, typename ANY_CATEGORY>
inline
bdem_BerUniversalTagNumber::Value
bdem_BerUniversalTagNumber_Imp::select(const TYPE&, ANY_CATEGORY)
{
    BSLS_ASSERT(0 && "invalid type category");
    return static_cast<TagVal>(-1);
}

// ============================================================================
//                     namespace bdex_InStreamFunctions
// ============================================================================

namespace bdex_InStreamFunctions {

template <typename STREAM>
inline
STREAM& streamIn(STREAM&                            stream,
                 bdem_BerUniversalTagNumber::Value& value,
                 int                                version)
{
    return bdem_BerUniversalTagNumber::bdexStreamIn(stream, value, version);
}

}  // close namespace bdex_InStreamFunctions;

// ============================================================================
//                      namespace bdex_VersionFunctions
// ============================================================================

namespace bdex_VersionFunctions {

inline
int maxSupportedVersion(bdem_BerUniversalTagNumber::Value)
{
    return bdem_BerUniversalTagNumber::maxSupportedBdexVersion();
}

}  // close namespace bdex_VersionFunctions;

// ============================================================================
//                     namespace bdex_OutStreamFunctions
// ============================================================================

namespace bdex_OutStreamFunctions {

template <typename STREAM>
inline
STREAM& streamOut(STREAM&                                  stream,
                  const bdem_BerUniversalTagNumber::Value& value,
                  int                                      version)
{
    return bdem_BerUniversalTagNumber::bdexStreamOut(stream, value, version);
}

}  // close namespace bdex_OutStreamFunctions

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
