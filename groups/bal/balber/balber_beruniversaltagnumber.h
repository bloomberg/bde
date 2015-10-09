// balber_beruniversaltagnumber.h                                     -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALBER_BERUNIVERSALTAGNUMBER
#define INCLUDED_BALBER_BERUNIVERSALTAGNUMBER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Enumerate the set of BER universal tag numbers.
//
//@CLASSES:
//  balber::BerUniversalTagNumber: namespace universal tag number enumeration
//
//@SEE_ALSO: balber_berencoder, balber_berdecoder
//          http://www.itu.int/ITU-T/studygroups/com17/languages/X.680-0207.pdf
//
//@DESCRIPTION: This component provides a namespace for the 'enum' type
// 'balber::BerUniversalTagNumber::Value'.  'Value' enumerates the set of BER
// universal tag numbers used by the BER encoder and decoder.  The universal
// tag numbers are defined in the X.680 standard, in section 8.
//
// In addition, this component supports functions that convert the 'Value'
// enumerations to a well-defined ASCII representation.
//
// This component also provides a function that returns the universal tag
// number for an object with formatting mode, according to the following table:
//..
//  C++ Type                           Formatting Mode  Universal Tag Number
//  --------                           ---------------  --------------------
//  bool                               DEFAULT          e_BER_BOOL
//                                     DEC              e_BER_BOOL
//                                     TEXT             e_BER_BOOL
//  char                               DEFAULT          e_BER_INT
//                                     DEC              e_BER_INT
//                                     TEXT             e_BER_UTF8_STRING
//  unsigned char                      DEFAULT          e_BER_INT
//                                     DEC              e_BER_INT
//  [unsigned] short                   DEFAULT          e_BER_INT
//                                     DEC              e_BER_INT
//  [unsigned] int                     DEFAULT          e_BER_INT
//                                     DEC              e_BER_INT
//  [unsigned] long                    DEFAULT          e_BER_INT
//                                     DEC              e_BER_INT
//  bsls::Types::[Uint64|Int64]        DEFAULT          e_BER_INT
//                                     DEC              e_BER_INT
//  float                              DEFAULT          e_BER_REAL
//  double                             DEFAULT          e_BER_REAL
//  bsl::string                        DEFAULT          e_BER_UTF8_STRING
//                                     TEXT             e_BER_UTF8_STRING
//                                     BASE64           e_BER_OCTET_STRING
//                                     HEX              e_BER_OCTET_STRING
//  bdlt::Date                         DEFAULT          e_BER_VISIBLE_STRING
//  bdlt::DateTz                       DEFAULT          e_BER_VISIBLE_STRING
//  bdlt::Datetime                     DEFAULT          e_BER_VISIBLE_STRING
//  bdlt::DateTimeTz                   DEFAULT          e_BER_VISIBLE_STRING
//  bdlt::Time                         DEFAULT          e_BER_VISIBLE_STRING
//  bdlt::TimeTz                       DEFAULT          e_BER_VISIBLE_STRING
//  bsl::vector<char>                  DEFAULT          e_BER_OCTET_STRING
//                                     BASE64           e_BER_OCTET_STRING
//                                     HEX              e_BER_OCTET_STRING
//                                     TEXT             e_BER_UTF8_STRING
//..
// If the object is not one of these types, then the universal tag number is
// selected based on the object's type category, as follows:
//..
//  Category             Universal Tag Number
//  --------             --------------------
//  CustomizedType       Use universal tag number of the base type.
//  Enumeration          'e_BER_ENUMERATION' when formatting mode is either
//                       'DEFAULT', 'DEC', or 'TEXT'.
//  Sequence             'e_BER_SEQUENCE' when formatting mode is 'DEFAULT'.
//  Choice               'e_BER_SEQUENCE' when formatting mode is 'DEFAULT'.
//  Array                'e_BER_SEQUENCE' when formatting mode is 'DEFAULT'.
//..
// The behavior is undefined if the object does not fall into one the above
// categories or formatting modes.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Exercise 1: Basic Syntax
/// - - - - - - - - - - - -
// The following snippets of code provide a simple illustration of
// 'balber::BerUniversalTagNumber' operation.
//
// First, create a variable 'tagNumber' of type
// 'balber::BerUniversalTagNumber::Value' and initialize it to the value
// 'balber::BerUniversalTagNumber::e_BER_INT':
//..
//  balber::BerUniversalTagNumber::Value tagNumber
//                                  = balber::BerUniversalTagNumber::e_BER_INT;
//..
// Next, store its representation in a variable 'rep' of type 'const char *':
//..
//  const char *rep = balber::BerUniversalTagNumber::toString(tagNumber);
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

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALBER_BERENCODEROPTIONS
#include <balber_berencoderoptions.h>
#endif

#ifndef INCLUDED_BDLAT_CUSTOMIZEDTYPEFUNCTIONS
#include <bdlat_customizedtypefunctions.h>
#endif

#ifndef INCLUDED_BDLAT_FORMATTINGMODE
#include <bdlat_formattingmode.h>
#endif

#ifndef INCLUDED_BDLAT_NULLABLEVALUEFUNCTIONS
#include <bdlat_nullablevaluefunctions.h>
#endif

#ifndef INCLUDED_BDLAT_TYPECATEGORY
#include <bdlat_typecategory.h>
#endif

#ifndef INCLUDED_BDLB_VARIANT
#include <bdlb_variant.h>
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
namespace balber {
                        // ============================
                        // struct BerUniversalTagNumber
                        // ============================

struct BerUniversalTagNumber {
    // This 'struct' contains an enumeration of the universal tag numbers for
    // BER encoding.

    // TYPES
    enum Value {
        e_BER_INVALID        = -1

        // Universal tag numbers as per X.680 section 8

      , e_BER_BOOL           = 0x01  // ( 1) Boolean type
      , e_BER_INT            = 0x02  // ( 2) Integer type
      , e_BER_OCTET_STRING   = 0x04  // ( 4) Octet string type
      , e_BER_REAL           = 0x09  // ( 9) Real type
      , e_BER_ENUMERATION    = 0x0A  // (10) Enumerated type
      , e_BER_UTF8_STRING    = 0x0C  // (12) UTF8 string type
      , e_BER_SEQUENCE       = 0x10  // (16) Sequence and Sequence-of types
      , e_BER_VISIBLE_STRING = 0x1A  // (26) VisibleString type (7-bit ASCII)

    };

    // CONSTANTS
    enum {
        k_LENGTH = 8  // the number of enumerations in the 'Value' enumeration

    };

    // CLASS METHODS
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

    static bsl::ostream& print(bsl::ostream& stream, Value value);
        // Write to the specified 'stream' the string representation of the
        // specified enumeration 'value'.  Return a reference to the modifiable
        // 'stream'.

    template <typename TYPE>
    static Value select(const TYPE&  object,
                        int          formattingMode,
                        int         *alternateTag);
        // Return the universal tag number for the specified 'object' using the
        // specified 'formattingMode', and load into the specified
        // 'alternateTag' any alternative tag numbers corresponding to
        // 'object'.  The behavior is undefined if the type category of
        // 'object' and the 'formattingMode' does not permit a universal tag
        // number (see {DESCRIPTION} for allowed type categories and formatting
        // modes).  Note that if an alternate tag number does not exist for
        // 'object' then 'alternateTag' is not modified.

    template <typename TYPE>
    static Value select(const TYPE&              object,
                        int                      formattingMode,
                        const BerEncoderOptions *options);
        // Return the universal tag number for the specified 'object' with the
        // specified 'formattingMode' using the specified 'options'.  The
        // behavior is undefined if the type category of 'object' and the
        // 'formattingMode' do not permit a universal tag number (see
        // {DESCRIPTION} for allowed type categories and formatting modes).
};

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream&                stream,
                         BerUniversalTagNumber::Value rhs);
    // Format the specified 'rhs' to the specified output 'stream' and return a
    // reference providing modifiable access to 'stream'.


                    // ====================================
                    // struct BerUniversalTagNumber_ImpUtil
                    // ====================================

class BerUniversalTagNumber_Imp {
    // Component-private class.  Do not use.  This class contains
    // implementation details for this component.

    typedef bdlat_FormattingMode         FMode;   // shorthand
    typedef BerUniversalTagNumber::Value TagVal;  // shorthand

    int                      d_formattingMode;
    const BerEncoderOptions *d_options_p;         // options (held, not owned)
    int                      d_alternateTag;      // alternate tag

    // PRIVATE MANIPULATORS
    TagVal selectForDateAndTimeTypes();
        // Return the universal tag number for date and time types and load
        // into the internal 'alternateTag' data member the any alternative tag
        // numbers corresponding to those types.

  public:
    BerUniversalTagNumber_Imp(int                      fm,
                              const BerEncoderOptions *options = 0)
    : d_formattingMode(fm)
    , d_options_p(options)
    , d_alternateTag(-1)
    {
    }

                        //  ** By Type **

    TagVal select(const bool&              object, bdlat_TypeCategory::Simple);
    TagVal select(const char&              object, bdlat_TypeCategory::Simple);
    TagVal select(const signed char&       object, bdlat_TypeCategory::Simple);
    TagVal select(const unsigned char&     object, bdlat_TypeCategory::Simple);
    TagVal select(const short&             object, bdlat_TypeCategory::Simple);
    TagVal select(const unsigned short&    object, bdlat_TypeCategory::Simple);
    TagVal select(const int&               object, bdlat_TypeCategory::Simple);
    TagVal select(const unsigned int&      object, bdlat_TypeCategory::Simple);
    TagVal select(const long&              object, bdlat_TypeCategory::Simple);
    TagVal select(const unsigned long&     object, bdlat_TypeCategory::Simple);
    TagVal select(const bsls::Types::Int64&
                                           object, bdlat_TypeCategory::Simple);
    TagVal select(const bsls::Types::Uint64&
                                           object, bdlat_TypeCategory::Simple);
    TagVal select(const float&             object, bdlat_TypeCategory::Simple);
    TagVal select(const double&            object, bdlat_TypeCategory::Simple);
    TagVal select(const bsl::string&       object, bdlat_TypeCategory::Simple);
    TagVal select(const bdlt::Date&        object, bdlat_TypeCategory::Simple);
    TagVal select(const bdlt::DateTz&      object, bdlat_TypeCategory::Simple);
    TagVal select(const bdlt::Datetime&    object, bdlat_TypeCategory::Simple);
    TagVal select(const bdlt::DatetimeTz&  object, bdlat_TypeCategory::Simple);
    TagVal select(const bdlt::Time&        object, bdlat_TypeCategory::Simple);
    TagVal select(const bdlt::TimeTz&      object, bdlat_TypeCategory::Simple);
    template <typename TYPE, typename TYPETZ>
    TagVal select(const bdlb::Variant2<TYPE, TYPETZ>&
                                           object, bdlat_TypeCategory::Simple);

    TagVal select(const bsl::vector<char>& object, bdlat_TypeCategory::Array);

                        //  ** By Catetory **

    template <typename TYPE>
    TagVal select(const TYPE& object, bdlat_TypeCategory::CustomizedType);

    template <typename TYPE>
    TagVal select(const TYPE& object, bdlat_TypeCategory::Enumeration);

    template <typename TYPE>
    TagVal select(const TYPE& object, bdlat_TypeCategory::Array);

    template <typename TYPE>
    TagVal select(const TYPE& object, bdlat_TypeCategory::Sequence);

    template <typename TYPE>
    TagVal select(const TYPE& object, bdlat_TypeCategory::Choice);

    template <typename TYPE>
    TagVal select(const TYPE& object, bdlat_TypeCategory::NullableValue);

    template <typename TYPE, typename ANY_CATEGORY>
    TagVal select(const TYPE& object, ANY_CATEGORY);

    // FUNCTOR OPERATORS
    template <typename TYPE>
    inline
    TagVal operator()(const TYPE&, bslmf::Nil)
    {
        BSLS_ASSERT(0 && "Invalid type category");
        return BerUniversalTagNumber::e_BER_INVALID;
    }

    template <typename TYPE, typename ANY_CATEGORY>
    inline
    TagVal operator()(const TYPE& object, ANY_CATEGORY category)
    {
        return BerUniversalTagNumber_Imp::select(object, category);
    }

    int alternateTag() {
        return d_alternateTag;
    }
};

}  // close package namespace

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                    // ------------------------------------
                    // struct balber::BerUniversalTagNumber
                    // ------------------------------------

// FORWARD DECLARATIONS

// Updated by 'bde-replace-bdet-forward-declares.py -m bdlt': 2015-02-03
// Updated declarations tagged with '// bdet -> bdlt'.

namespace bdlt { class Date; }                                  // bdet -> bdlt
namespace bdet { typedef ::BloombergLP::bdlt::Date Date; }      // bdet -> bdlt

namespace bdlt { class DateTz; }                                // bdet -> bdlt
namespace bdet { typedef ::BloombergLP::bdlt::DateTz DateTz; }  // bdet -> bdlt

namespace bdlt { class Datetime; }                              // bdet -> bdlt
namespace bdet { typedef ::BloombergLP::bdlt::Datetime Datetime; }
                                                                // bdet -> bdlt

namespace bdlt { class DatetimeTz; }                            // bdet -> bdlt
namespace bdet { typedef ::BloombergLP::bdlt::DatetimeTz DatetimeTz; }
                                                                // bdet -> bdlt

namespace bdlt { class Time; }                                  // bdet -> bdlt
namespace bdet { typedef ::BloombergLP::bdlt::Time Time; }      // bdet -> bdlt

namespace bdlt { class TimeTz; }                                // bdet -> bdlt
namespace bdet { typedef ::BloombergLP::bdlt::TimeTz TimeTz; }  // bdet -> bdlt

namespace balber {

// CLASS METHODS
inline
int BerUniversalTagNumber::fromInt(BerUniversalTagNumber::Value *result,
                                   int                           number)
{
    enum { k_SUCCESS = 0, k_NOT_FOUND = 1 };

    switch (number) {
      case e_BER_BOOL:
      case e_BER_INT:
      case e_BER_OCTET_STRING:
      case e_BER_REAL:
      case e_BER_ENUMERATION:
      case e_BER_UTF8_STRING:
      case e_BER_SEQUENCE:
      case e_BER_VISIBLE_STRING:
        *result = static_cast<BerUniversalTagNumber::Value>(number);
        return k_SUCCESS;                                             // RETURN
      default:
        return k_NOT_FOUND;                                           // RETURN
    }
}

inline
bsl::ostream& BerUniversalTagNumber::print(bsl::ostream&                stream,
                                           BerUniversalTagNumber::Value value)
{
    return stream << toString(value);
}

template <typename TYPE>
inline
BerUniversalTagNumber::Value
BerUniversalTagNumber::select(const TYPE&              object,
                              int                      formattingMode,
                              const BerEncoderOptions *options)
{
    BerUniversalTagNumber_Imp imp(formattingMode, options);
    int retVal = bdlat_TypeCategoryUtil::accessByCategory(object, imp);
    return (BerUniversalTagNumber::Value) retVal;
}

template <typename TYPE>
inline
BerUniversalTagNumber::Value
BerUniversalTagNumber::select(const TYPE&  object,
                              int          formattingMode,
                              int         *alternateTag)
{
    BerUniversalTagNumber_Imp imp(formattingMode);
    int retVal = bdlat_TypeCategoryUtil::accessByCategory(object, imp);

    const int tag = imp.alternateTag();
    if (-1 != tag) {
        *alternateTag = tag;
    }

    return (BerUniversalTagNumber::Value) retVal;
}

}  // close package namespace

// FREE OPERATORS
inline
bsl::ostream& balber::operator<<(bsl::ostream&                stream,
                                 BerUniversalTagNumber::Value rhs)
{
    return BerUniversalTagNumber::print(stream, rhs);
}

namespace balber {

                      // --------------------------------
                      // struct BerUniversalTagNumber_Imp
                      // --------------------------------

// PRIVATE MANIPULATORS
inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::selectForDateAndTimeTypes()
{
    BSLS_ASSERT_SAFE(
          FMode::e_DEFAULT == (d_formattingMode & FMode::e_TYPE_MASK));

    if (d_options_p) {
        return d_options_p->encodeDateAndTimeTypesAsBinary()
             ? BerUniversalTagNumber::e_BER_OCTET_STRING
             : BerUniversalTagNumber::e_BER_VISIBLE_STRING;
    }
    else {
        d_alternateTag = BerUniversalTagNumber::e_BER_OCTET_STRING;

        return BerUniversalTagNumber::e_BER_VISIBLE_STRING;
    }
}

                        //  ** By Type **

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const bool&, bdlat_TypeCategory::Simple)
{
    BSLS_ASSERT_SAFE(
          FMode::e_DEFAULT == (d_formattingMode & FMode::e_TYPE_MASK)
       || FMode::e_DEC     == (d_formattingMode & FMode::e_TYPE_MASK)
       || FMode::e_TEXT    == (d_formattingMode & FMode::e_TYPE_MASK));

    return BerUniversalTagNumber::e_BER_BOOL;
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const char&, bdlat_TypeCategory::Simple)
{
    if (FMode::e_TEXT == (d_formattingMode & FMode::e_TYPE_MASK)) {
        return BerUniversalTagNumber::e_BER_UTF8_STRING;
    }

    BSLS_ASSERT_SAFE(
          FMode::e_DEFAULT == (d_formattingMode & FMode::e_TYPE_MASK)
       || FMode::e_DEC     == (d_formattingMode & FMode::e_TYPE_MASK));

    return BerUniversalTagNumber::e_BER_INT;
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const signed char&         ,
                                  bdlat_TypeCategory::Simple )
{
    if (FMode::e_TEXT == (d_formattingMode & FMode::e_TYPE_MASK)) {
        return BerUniversalTagNumber::e_BER_UTF8_STRING;
    }

    BSLS_ASSERT_SAFE(
          FMode::e_DEFAULT == (d_formattingMode & FMode::e_TYPE_MASK)
       || FMode::e_DEC     == (d_formattingMode & FMode::e_TYPE_MASK));

    return BerUniversalTagNumber::e_BER_INT;
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const unsigned char&            ,
                                       bdlat_TypeCategory::Simple )
{
    BSLS_ASSERT_SAFE(
          FMode::e_DEFAULT == (d_formattingMode & FMode::e_TYPE_MASK)
       || FMode::e_DEC     == (d_formattingMode & FMode::e_TYPE_MASK));

    return BerUniversalTagNumber::e_BER_INT;
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const short&                    ,
                                       bdlat_TypeCategory::Simple )
{
    BSLS_ASSERT_SAFE(
          FMode::e_DEFAULT == (d_formattingMode & FMode::e_TYPE_MASK)
       || FMode::e_DEC     == (d_formattingMode & FMode::e_TYPE_MASK));

    return BerUniversalTagNumber::e_BER_INT;
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const unsigned short&           ,
                                       bdlat_TypeCategory::Simple )
{
    BSLS_ASSERT_SAFE(
          FMode::e_DEFAULT == (d_formattingMode & FMode::e_TYPE_MASK)
       || FMode::e_DEC     == (d_formattingMode & FMode::e_TYPE_MASK));

    return BerUniversalTagNumber::e_BER_INT;
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const int&, bdlat_TypeCategory::Simple)
{
    BSLS_ASSERT_SAFE(
          FMode::e_DEFAULT == (d_formattingMode & FMode::e_TYPE_MASK)
       || FMode::e_DEC     == (d_formattingMode & FMode::e_TYPE_MASK));

    return BerUniversalTagNumber::e_BER_INT;
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const unsigned int&        ,
                                  bdlat_TypeCategory::Simple )
{
    BSLS_ASSERT_SAFE(
          FMode::e_DEFAULT == (d_formattingMode & FMode::e_TYPE_MASK)
       || FMode::e_DEC     == (d_formattingMode & FMode::e_TYPE_MASK));

    return BerUniversalTagNumber::e_BER_INT;
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const long&                ,
                                  bdlat_TypeCategory::Simple )
{
    BSLS_ASSERT_SAFE(
          FMode::e_DEFAULT == (d_formattingMode & FMode::e_TYPE_MASK)
       || FMode::e_DEC     == (d_formattingMode & FMode::e_TYPE_MASK));

    return BerUniversalTagNumber::e_BER_INT;
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const unsigned long&       ,
                                  bdlat_TypeCategory::Simple )
{
    BSLS_ASSERT_SAFE(
          FMode::e_DEFAULT == (d_formattingMode & FMode::e_TYPE_MASK)
       || FMode::e_DEC     == (d_formattingMode & FMode::e_TYPE_MASK));

    return BerUniversalTagNumber::e_BER_INT;
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const bsls::Types::Int64&  ,
                                  bdlat_TypeCategory::Simple )
{
    BSLS_ASSERT_SAFE(
          FMode::e_DEFAULT == (d_formattingMode & FMode::e_TYPE_MASK)
       || FMode::e_DEC     == (d_formattingMode & FMode::e_TYPE_MASK));

    return BerUniversalTagNumber::e_BER_INT;
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const bsls::Types::Uint64& ,
                                  bdlat_TypeCategory::Simple )
{
    BSLS_ASSERT_SAFE(
          FMode::e_DEFAULT == (d_formattingMode & FMode::e_TYPE_MASK)
       || FMode::e_DEC     == (d_formattingMode & FMode::e_TYPE_MASK));

    return BerUniversalTagNumber::e_BER_INT;
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const float&               ,
                                  bdlat_TypeCategory::Simple )
{
    BSLS_ASSERT_SAFE(
          FMode::e_DEFAULT == (d_formattingMode & FMode::e_TYPE_MASK)
       || FMode::e_DEC     == (d_formattingMode & FMode::e_TYPE_MASK));

    return BerUniversalTagNumber::e_BER_REAL;
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const double&              ,
                                  bdlat_TypeCategory::Simple )
{
    BSLS_ASSERT_SAFE(
          FMode::e_DEFAULT == (d_formattingMode & FMode::e_TYPE_MASK)
       || FMode::e_DEC     == (d_formattingMode & FMode::e_TYPE_MASK));

    return BerUniversalTagNumber::e_BER_REAL;
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const bsl::string&        ,
                                  bdlat_TypeCategory::Simple)
{
    if (FMode::e_BASE64 == (d_formattingMode & FMode::e_TYPE_MASK)
     || FMode::e_HEX    == (d_formattingMode & FMode::e_TYPE_MASK)) {
        return BerUniversalTagNumber::e_BER_OCTET_STRING;
    }

    BSLS_ASSERT_SAFE(
          FMode::e_DEFAULT == (d_formattingMode & FMode::e_TYPE_MASK)
       || FMode::e_TEXT    == (d_formattingMode & FMode::e_TYPE_MASK));

    return BerUniversalTagNumber::e_BER_UTF8_STRING;
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const bdlt::Date&          ,
                                  bdlat_TypeCategory::Simple )
{
    return selectForDateAndTimeTypes();
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const bdlt::DateTz&        ,
                                  bdlat_TypeCategory::Simple )
{
    return selectForDateAndTimeTypes();
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const bdlt::Datetime&      ,
                                  bdlat_TypeCategory::Simple )
{
    return selectForDateAndTimeTypes();
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const bdlt::DatetimeTz&    ,
                                  bdlat_TypeCategory::Simple )
{
    return selectForDateAndTimeTypes();
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const bdlt::Time&          ,
                                  bdlat_TypeCategory::Simple )
{
    return selectForDateAndTimeTypes();
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const bdlt::TimeTz&        ,
                                  bdlat_TypeCategory::Simple )
{
    return selectForDateAndTimeTypes();
}

template <typename TYPE, typename TYPETZ>
inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const bdlb::Variant2<TYPE, TYPETZ>& ,
                                  bdlat_TypeCategory::Simple          )
{
    BSLMF_ASSERT((bslmf::IsSame<bdlt::Date,       TYPE  >::VALUE
               && bslmf::IsSame<bdlt::DateTz,     TYPETZ>::VALUE)
              || (bslmf::IsSame<bdlt::Time,       TYPE  >::VALUE
               && bslmf::IsSame<bdlt::TimeTz,     TYPETZ>::VALUE)
              || (bslmf::IsSame<bdlt::Datetime,   TYPE  >::VALUE
               && bslmf::IsSame<bdlt::DatetimeTz, TYPETZ>::VALUE));

    return selectForDateAndTimeTypes();
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const bsl::vector<char>&  ,
                                  bdlat_TypeCategory::Array )
{
    if (FMode::e_TEXT == (d_formattingMode & FMode::e_TYPE_MASK)) {
        return BerUniversalTagNumber::e_BER_UTF8_STRING;
    }

    BSLS_ASSERT_SAFE(
          FMode::e_DEFAULT == (d_formattingMode & FMode::e_TYPE_MASK)
       || FMode::e_BASE64  == (d_formattingMode & FMode::e_TYPE_MASK)
       || FMode::e_HEX     == (d_formattingMode & FMode::e_TYPE_MASK));

    return BerUniversalTagNumber::e_BER_OCTET_STRING;
}

                        //  ** By Catetory **

template <typename TYPE>
inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const TYPE&                        ,
                                  bdlat_TypeCategory::CustomizedType )
{
    typedef typename
    bdlat_CustomizedTypeFunctions::BaseType<TYPE>::Type BaseType;

    // The object referenced by the following pointer is never used.  A null
    // pointer was chosen to force a SEGV in case of inadvertent use.
    // Dereferencing null is technically illegal, but will work in this case
    // because the value is never retrieved from memory.
    BaseType *tmp = 0;
    return (TagVal) bdlat_TypeCategoryUtil::accessByCategory(*tmp, *this);
}

template <typename TYPE>
inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const TYPE&                     ,
                                  bdlat_TypeCategory::Enumeration )
{
    BSLS_ASSERT_SAFE(
          FMode::e_DEFAULT == (d_formattingMode & FMode::e_TYPE_MASK)
       || FMode::e_DEC     == (d_formattingMode & FMode::e_TYPE_MASK)
       || FMode::e_TEXT    == (d_formattingMode & FMode::e_TYPE_MASK));

    return BerUniversalTagNumber::e_BER_ENUMERATION;
}

template <typename TYPE>
inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const TYPE&               ,
                                  bdlat_TypeCategory::Array )
{
    BSLS_ASSERT_SAFE(
          FMode::e_DEFAULT == (d_formattingMode & FMode::e_TYPE_MASK));

    return BerUniversalTagNumber::e_BER_SEQUENCE;
}

template <typename TYPE>
inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const TYPE&                  ,
                                  bdlat_TypeCategory::Sequence )
{
    BSLS_ASSERT_SAFE(
          FMode::e_DEFAULT == (d_formattingMode & FMode::e_TYPE_MASK));

    return BerUniversalTagNumber::e_BER_SEQUENCE;
}

template <typename TYPE>
inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const TYPE&                ,
                                  bdlat_TypeCategory::Choice )
{
    BSLS_ASSERT_SAFE(
          FMode::e_DEFAULT == (d_formattingMode & FMode::e_TYPE_MASK));

    // According to X.694 (clause 20.4), an XML choice element is encoded as a
    // sequence with 1 element.

    return BerUniversalTagNumber::e_BER_SEQUENCE;
}

template <typename TYPE>
inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const TYPE&                       ,
                                  bdlat_TypeCategory::NullableValue )
{
    if (d_formattingMode & FMode::e_NILLABLE) {
        return BerUniversalTagNumber::e_BER_SEQUENCE;
    }

    // If got here, then value is nullable, but not nillable.
    typedef typename
    bdlat_NullableValueFunctions::ValueType<TYPE>::Type ValueType;

    // The object referenced by the following pointer is never used.  A null
    // pointer was chosen to force a SEGV in case of inadvertent use.
    // Dereferencing null is technically illegal, but will work in this case
    // because the value is never retrieved from memory.
    ValueType *tmp = 0;
    return (TagVal) bdlat_TypeCategoryUtil::accessByCategory(*tmp, *this);
}

template <typename TYPE, typename ANY_CATEGORY>
inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const TYPE&, ANY_CATEGORY)
{
    BSLS_ASSERT(0 && "invalid type category");
    return static_cast<TagVal>(-1);
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
