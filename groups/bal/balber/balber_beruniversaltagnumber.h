// balber_beruniversaltagnumber.h                                     -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALBER_BERUNIVERSALTAGNUMBER
#define INCLUDED_BALBER_BERUNIVERSALTAGNUMBER

#include <bsls_ident.h>
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
//  bdldfp::Decimal64                  DEFAULT          e_BER_OCTET_STRING
//  bsl::string[_view]                 DEFAULT          e_BER_UTF8_STRING
//                                     TEXT             e_BER_UTF8_STRING
//                                     BASE64           e_BER_OCTET_STRING
//                                     HEX              e_BER_OCTET_STRING
//  bslstl::StringRef                  DEFAULT          e_BER_UTF8_STRING
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

#include <balscm_version.h>

#include <balber_berencoderoptions.h>

#include <bdlat_customizedtypefunctions.h>
#include <bdlat_formattingmode.h>
#include <bdlat_nullablevaluefunctions.h>
#include <bdlat_typecategory.h>

#include <bdldfp_decimal.h>

#include <bdlb_variant.h>

#include <bdlt_date.h>
#include <bdlt_datetz.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimetz.h>
#include <bdlt_time.h>
#include <bdlt_timetz.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>

#include <bsls_assert.h>
#include <bsls_types.h>

#include <bsl_ostream.h>
#include <bsl_string.h>
#include <bsl_string_view.h>
#include <bsl_vector.h>

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

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , BDEM_BER_INVALID        = e_BER_INVALID
      , BDEM_BER_BOOL           = e_BER_BOOL
      , BDEM_BER_INT            = e_BER_INT
      , BDEM_BER_OCTET_STRING   = e_BER_OCTET_STRING
      , BDEM_BER_REAL           = e_BER_REAL
      , BDEM_BER_ENUMERATION    = e_BER_ENUMERATION
      , BDEM_BER_UTF8_STRING    = e_BER_UTF8_STRING
      , BDEM_BER_SEQUENCE       = e_BER_SEQUENCE
      , BDEM_BER_VISIBLE_STRING = e_BER_VISIBLE_STRING
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };

    // CONSTANTS
    enum {
        k_LENGTH = 8  // the number of enumerations in the 'Value' enumeration

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , BDEM_LENGTH = k_LENGTH
      , NUM_ENUMERATORS = k_LENGTH
#endif // BDE_OMIT_INTERNAL_DEPRECATED
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

                       // ===============================
                       // class BerUniversalTagNumber_Sel
                       // ===============================

template <class TYPE, class CATEGORY>
class BerUniversalTagNumber_Sel {
    // Component-private class.  Do not use.  This class contains
    // implementation details of this component.
    //
    ///Implementation Note
    ///-------------------
    // The suffix of this class, "Sel", is a contraction of "Selector", which
    // is meant to associate with the action of selecting a particular overload
    // from an overload set.
    //
    // This component uses specializations of this class to guide overload
    // resolution for functions that need to select between one of several
    // overloads based on the specified 'TYPE' and 'CATEGORY' information.
    // Note that 'CATEGORY' is expected to be one of the "category" types that
    // are members of 'bdlat_TypeCategory'.

  public:
    // CREATORS
    BerUniversalTagNumber_Sel()
        // Create a 'BerUniversalTagNumber_Sel' object.  Note that objects of
        // this type are stateless.
    {
    }

    //! BerUniversalTagNumber_Sel(
    //!                   const BerUniversalTagNumber_Sel& original) = default;
        // Create a new 'BerUniversalTagNumber_Sel' object having a copy of the
        // value of the specified 'original' object.  Note that objects of this
        // type are stateless and their value is a notional concept only.

    // MANIPULATORS
    //! BerUniversalTagNumber_Sel& operator=(
    //!                   const BerUniversalTagNumber_Sel& original) = default;
        // Assign the value of the specified 'original' object to this object.
        // Note that objects of this type are stateless and their value is a
        // notional concept only.
};

                      // ================================
                      // struct BerUniversalTagNumber_Imp
                      // ================================

class BerUniversalTagNumber_Imp {
    // Component-private class.  Do not use.  This class contains
    // implementation details for this component.

    // PRIVATE TYPES
    typedef bdlat_FormattingMode         FMode;
    typedef BerUniversalTagNumber::Value TagVal;
        // These type definitions are shorthand used throughout the members of
        // this class.

    typedef bdlat_TypeCategory::Array          ArrayCat;
    typedef bdlat_TypeCategory::Choice         ChoiceCat;
    typedef bdlat_TypeCategory::CustomizedType CustomizedTypeCat;
    typedef bdlat_TypeCategory::DynamicType    DynamicTypeCat;
    typedef bdlat_TypeCategory::Enumeration    EnumerationCat;
    typedef bdlat_TypeCategory::NullableValue  NullableValueCat;
    typedef bdlat_TypeCategory::Sequence       SequenceCat;
    typedef bdlat_TypeCategory::Simple         SimpleCat;
        // These type definitions are shorthand aliases for 'bdlat' category
        // types.  The "Cat" suffix stands for "Category".  The shorthand is
        // useful, essentially, for permitting rows in tables of function
        // overloads and dependent type definitions to fit on one line.  Doing
        // so improves the legibility of this class immensely.

    typedef bsl::string         String;
    typedef bsl::string_view    StringView;
    typedef bslstl::StringRef   StringRef;
    typedef bsls::Types::Int64  Int64;
    typedef bsls::Types::Uint64 Uint64;
    typedef bdldfp::Decimal64   Decimal64;
    typedef bdlt::Date          Date;
    typedef bdlt::DateTz        DateTz;
    typedef bdlt::Datetime      Datetime;
    typedef bdlt::DatetimeTz    DatetimeTz;
    typedef bdlt::Time          Time;
    typedef bdlt::TimeTz        TimeTz;
        // Similar to the aliases for the 'bdlat' category types above, these
        // type definitions are shorthand aliases for object types in the
        // 'Simple' 'bdlat' type category, used to improve the legibility of
        // this class.

    typedef bsl::vector<char>   CharVector;
        // 'CharVector' is the one such alias in the 'Array' 'bdlat' category,
        // not the 'Simple' category.

    typedef BerUniversalTagNumber_Sel<bool          , SimpleCat> BoolSel;
    typedef BerUniversalTagNumber_Sel<char          , SimpleCat> CharSel;
    typedef BerUniversalTagNumber_Sel<signed char   , SimpleCat> ScharSel;
    typedef BerUniversalTagNumber_Sel<unsigned char , SimpleCat> UcharSel;
    typedef BerUniversalTagNumber_Sel<short         , SimpleCat> ShortSel;
    typedef BerUniversalTagNumber_Sel<unsigned short, SimpleCat> UshortSel;
    typedef BerUniversalTagNumber_Sel<int           , SimpleCat> IntSel;
    typedef BerUniversalTagNumber_Sel<unsigned int  , SimpleCat> UintSel;
    typedef BerUniversalTagNumber_Sel<long          , SimpleCat> LongSel;
    typedef BerUniversalTagNumber_Sel<unsigned long , SimpleCat> UlongSel;
    typedef BerUniversalTagNumber_Sel<Int64         , SimpleCat> Int64Sel;
    typedef BerUniversalTagNumber_Sel<Uint64        , SimpleCat> Uint64Sel;
    typedef BerUniversalTagNumber_Sel<float         , SimpleCat> FloatSel;
    typedef BerUniversalTagNumber_Sel<double        , SimpleCat> DoubleSel;
    typedef BerUniversalTagNumber_Sel<Decimal64     , SimpleCat> Decimal64Sel;
    typedef BerUniversalTagNumber_Sel<String        , SimpleCat> StringSel;
    typedef BerUniversalTagNumber_Sel<StringView    , SimpleCat> StringViewSel;
    typedef BerUniversalTagNumber_Sel<StringRef     , SimpleCat> StringRefSel;
    typedef BerUniversalTagNumber_Sel<Date          , SimpleCat> DateSel;
    typedef BerUniversalTagNumber_Sel<DateTz        , SimpleCat> DateTzSel;
    typedef BerUniversalTagNumber_Sel<Datetime      , SimpleCat> DatetimeSel;
    typedef BerUniversalTagNumber_Sel<DatetimeTz    , SimpleCat> DatetimeTzSel;
    typedef BerUniversalTagNumber_Sel<Time          , SimpleCat> TimeSel;
    typedef BerUniversalTagNumber_Sel<TimeTz        , SimpleCat> TimeTzSel;
        // These type definitions are shorthand aliases for types having the
        // 'bdlat' 'Simple' category, whose objects can be created and used as
        // arguments to 'select' in order to select a particular overload.
        // Note that these "tag" types denote both an underlying type and its
        // 'bdlat' category.

    typedef BerUniversalTagNumber_Sel<CharVector    , ArrayCat > CharVectorSel;
        // 'CharVectorSel' is the one such alias that has a 'CATEGORY' that
        // denotes the 'bdlat' 'Array' category, not the 'Simple' category.

    // DATA
    int                      d_formattingMode;
    const BerEncoderOptions *d_options_p;         // options (held, not owned)
    int                      d_alternateTag;      // alternate tag

    // PRIVATE MANIPULATORS
    TagVal selectForDateAndTimeTypes();
        // Return the universal tag number for date and time types and load
        // into the internal 'alternateTag' data member the any alternative tag
        // numbers corresponding to those types.

    TagVal selectForStringTypes();
        // Return the universal tag number for string-like types.

  public:
    BerUniversalTagNumber_Imp(int fm, const BerEncoderOptions *options = 0)
    : d_formattingMode(fm)
    , d_options_p(options)
    , d_alternateTag(-1)
    {
    }

                               //  ** By Type **

    TagVal select(const BoolSel&                    selector);
    TagVal select(const CharSel&                    selector);
    TagVal select(const ScharSel&                   selector);
    TagVal select(const UcharSel&                   selector);
    TagVal select(const ShortSel&                   selector);
    TagVal select(const UshortSel&                  selector);
    TagVal select(const IntSel&                     selector);
    TagVal select(const UintSel&                    selector);
    TagVal select(const LongSel&                    selector);
    TagVal select(const UlongSel&                   selector);
    TagVal select(const Int64Sel&                   selector);
    TagVal select(const Uint64Sel&                  selector);
    TagVal select(const FloatSel&                   selector);
    TagVal select(const DoubleSel&                  selector);
    TagVal select(const Decimal64Sel&               selector);
    TagVal select(const StringSel&                  selector);
    TagVal select(const StringViewSel&              selector);
    TagVal select(const StringRefSel&               selector);
    TagVal select(const DateSel&                    selector);
    TagVal select(const DateTzSel&                  selector);
    TagVal select(const DatetimeSel&                selector);
    TagVal select(const DatetimeTzSel&              selector);
    TagVal select(const TimeSel&                    selector);
    TagVal select(const TimeTzSel&                  selector);
    template <typename TYPE, typename TYPETZ>
    TagVal select(
        const BerUniversalTagNumber_Sel<bdlb::Variant2<TYPE, TYPETZ>,
                                        SimpleCat>& selector);
    TagVal select(const CharVectorSel&              selector);
        // Return the universal tag number for an object having the 'TYPE' and
        // 'bdlat' 'CATEGORY' of the type of the specified 'selector', and load
        // into the 'alternateTag' attribute of this object any alternate tag
        // numbers corresponding to 'TYPE' and 'CATEGORY'.  Note that if an
        // alternate tag number for the 'TYPE' and 'CATEGORY' does not exist,
        // then the 'alternateTag' attribute of this object is not modified.

                             //  ** By Category **

    template <typename TYPE>
    TagVal select(
           const BerUniversalTagNumber_Sel<TYPE, ArrayCat         >& selector);
    template <typename TYPE>
    TagVal select(
           const BerUniversalTagNumber_Sel<TYPE, ChoiceCat        >& selector);
    template <typename TYPE>
    TagVal select(
           const BerUniversalTagNumber_Sel<TYPE, CustomizedTypeCat>& selector);
    template <typename TYPE>
    TagVal select(
           const BerUniversalTagNumber_Sel<TYPE, DynamicTypeCat   >& selector);
    template <typename TYPE>
    TagVal select(
           const BerUniversalTagNumber_Sel<TYPE, EnumerationCat   >& selector);
    template <typename TYPE>
    TagVal select(
           const BerUniversalTagNumber_Sel<TYPE, NullableValueCat >& selector);
    template <typename TYPE>
    TagVal select(
           const BerUniversalTagNumber_Sel<TYPE, SequenceCat      >& selector);
    template <typename TYPE, typename ANY_CATEGORY>
    TagVal select(
           const BerUniversalTagNumber_Sel<TYPE, ANY_CATEGORY     >& selector);
        // Return the universal tag number for an object having the 'TYPE' and
        // 'bdlat' 'CATEGORY' of the type of the specified 'selector', and load
        // into the 'alternateTag' attribute of this object any alternate tag
        // numbers corresponding to 'TYPE' and 'CATEGORY'.  Note that if an
        // alternate tag number for the 'TYPE' and 'CATEGORY' does not exist,
        // then the 'alternateTag' attribute of this object is not modified.

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
    TagVal operator()(const TYPE&, ANY_CATEGORY)
    {
        typedef BerUniversalTagNumber_Sel<TYPE, ANY_CATEGORY> Selector;
        return BerUniversalTagNumber_Imp::select(Selector());
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

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::selectForStringTypes()
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

                               //  ** By Type **

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const BoolSel&)
{
    BSLS_ASSERT_SAFE(
          FMode::e_DEFAULT == (d_formattingMode & FMode::e_TYPE_MASK)
       || FMode::e_DEC     == (d_formattingMode & FMode::e_TYPE_MASK)
       || FMode::e_TEXT    == (d_formattingMode & FMode::e_TYPE_MASK));

    return BerUniversalTagNumber::e_BER_BOOL;
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const CharSel&)
{
    if (FMode::e_TEXT == (d_formattingMode & FMode::e_TYPE_MASK)) {
        return BerUniversalTagNumber::e_BER_UTF8_STRING;
    }

    BSLS_ASSERT_SAFE(
          FMode::e_DEFAULT == (d_formattingMode & FMode::e_TYPE_MASK)
       || FMode::e_DEC     == (d_formattingMode & FMode::e_TYPE_MASK));

    // For 'unsigned short' and 'int', we also accept the enumeration
    // identifier (DRQS 166048981).  For all other integer types besides
    // 'bool', we also accept the enumeration identifier for consistency.
    d_alternateTag = BerUniversalTagNumber::e_BER_ENUMERATION;

    return BerUniversalTagNumber::e_BER_INT;
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const ScharSel&)
{
    if (FMode::e_TEXT == (d_formattingMode & FMode::e_TYPE_MASK)) {
        return BerUniversalTagNumber::e_BER_UTF8_STRING;
    }

    BSLS_ASSERT_SAFE(
          FMode::e_DEFAULT == (d_formattingMode & FMode::e_TYPE_MASK)
       || FMode::e_DEC     == (d_formattingMode & FMode::e_TYPE_MASK));

    // For 'unsigned short' and 'int', we also accept the enumeration
    // identifier (DRQS 166048981).  For all other integer types besides
    // 'bool', we also accept the enumeration identifier for consistency.
    d_alternateTag = BerUniversalTagNumber::e_BER_ENUMERATION;

    return BerUniversalTagNumber::e_BER_INT;
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const UcharSel&)
{
    BSLS_ASSERT_SAFE(
          FMode::e_DEFAULT == (d_formattingMode & FMode::e_TYPE_MASK)
       || FMode::e_DEC     == (d_formattingMode & FMode::e_TYPE_MASK));

    // For 'unsigned short' and 'int', we also accept the enumeration
    // identifier (DRQS 166048981).  For all other integer types besides
    // 'bool', we also accept the enumeration identifier for consistency.
    d_alternateTag = BerUniversalTagNumber::e_BER_ENUMERATION;

    return BerUniversalTagNumber::e_BER_INT;
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const ShortSel&)
{
    BSLS_ASSERT_SAFE(
          FMode::e_DEFAULT == (d_formattingMode & FMode::e_TYPE_MASK)
       || FMode::e_DEC     == (d_formattingMode & FMode::e_TYPE_MASK));

    // For 'unsigned short' and 'int', we also accept the enumeration
    // identifier (DRQS 166048981).  For all other integer types besides
    // 'bool', we also accept the enumeration identifier for consistency.
    d_alternateTag = BerUniversalTagNumber::e_BER_ENUMERATION;

    return BerUniversalTagNumber::e_BER_INT;
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const UshortSel&)
{
    BSLS_ASSERT_SAFE(
          FMode::e_DEFAULT == (d_formattingMode & FMode::e_TYPE_MASK)
       || FMode::e_DEC     == (d_formattingMode & FMode::e_TYPE_MASK));

    // For 'unsigned short' and 'int', we also accept the enumeration
    // identifier (DRQS 166048981).  For all other integer types besides
    // 'bool', we also accept the enumeration identifier for consistency.
    d_alternateTag = BerUniversalTagNumber::e_BER_ENUMERATION;

    return BerUniversalTagNumber::e_BER_INT;
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const IntSel&)
{
    BSLS_ASSERT_SAFE(
          FMode::e_DEFAULT == (d_formattingMode & FMode::e_TYPE_MASK)
       || FMode::e_DEC     == (d_formattingMode & FMode::e_TYPE_MASK));

    // For 'unsigned short' and 'int', we also accept the enumeration
    // identifier (DRQS 166048981).  For all other integer types besides
    // 'bool', we also accept the enumeration identifier for consistency.
    d_alternateTag = BerUniversalTagNumber::e_BER_ENUMERATION;

    return BerUniversalTagNumber::e_BER_INT;
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const UintSel&)
{
    BSLS_ASSERT_SAFE(
          FMode::e_DEFAULT == (d_formattingMode & FMode::e_TYPE_MASK)
       || FMode::e_DEC     == (d_formattingMode & FMode::e_TYPE_MASK));

    // For 'unsigned short' and 'int', we also accept the enumeration
    // identifier (DRQS 166048981).  For all other integer types besides
    // 'bool', we also accept the enumeration identifier for consistency.
    d_alternateTag = BerUniversalTagNumber::e_BER_ENUMERATION;

    return BerUniversalTagNumber::e_BER_INT;
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const LongSel&)
{
    BSLS_ASSERT_SAFE(
          FMode::e_DEFAULT == (d_formattingMode & FMode::e_TYPE_MASK)
       || FMode::e_DEC     == (d_formattingMode & FMode::e_TYPE_MASK));

    // For 'unsigned short' and 'int', we also accept the enumeration
    // identifier (DRQS 166048981).  For all other integer types besides
    // 'bool', we also accept the enumeration identifier for consistency.
    d_alternateTag = BerUniversalTagNumber::e_BER_ENUMERATION;

    return BerUniversalTagNumber::e_BER_INT;
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const UlongSel&)
{
    BSLS_ASSERT_SAFE(
          FMode::e_DEFAULT == (d_formattingMode & FMode::e_TYPE_MASK)
       || FMode::e_DEC     == (d_formattingMode & FMode::e_TYPE_MASK));

    // For 'unsigned short' and 'int', we also accept the enumeration
    // identifier (DRQS 166048981).  For all other integer types besides
    // 'bool', we also accept the enumeration identifier for consistency.
    d_alternateTag = BerUniversalTagNumber::e_BER_ENUMERATION;

    return BerUniversalTagNumber::e_BER_INT;
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const Int64Sel&)
{
    BSLS_ASSERT_SAFE(
          FMode::e_DEFAULT == (d_formattingMode & FMode::e_TYPE_MASK)
       || FMode::e_DEC     == (d_formattingMode & FMode::e_TYPE_MASK));

    // For 'unsigned short' and 'int', we also accept the enumeration
    // identifier (DRQS 166048981).  For all other integer types besides
    // 'bool', we also accept the enumeration identifier for consistency.
    d_alternateTag = BerUniversalTagNumber::e_BER_ENUMERATION;

    return BerUniversalTagNumber::e_BER_INT;
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const Uint64Sel&)
{
    BSLS_ASSERT_SAFE(
          FMode::e_DEFAULT == (d_formattingMode & FMode::e_TYPE_MASK)
       || FMode::e_DEC     == (d_formattingMode & FMode::e_TYPE_MASK));

    // For 'unsigned short' and 'int', we also accept the enumeration
    // identifier (DRQS 166048981).  For all other integer types besides
    // 'bool', we also accept the enumeration identifier for consistency.
    d_alternateTag = BerUniversalTagNumber::e_BER_ENUMERATION;

    return BerUniversalTagNumber::e_BER_INT;
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const FloatSel&)
{
    BSLS_ASSERT_SAFE(
          FMode::e_DEFAULT == (d_formattingMode & FMode::e_TYPE_MASK)
       || FMode::e_DEC     == (d_formattingMode & FMode::e_TYPE_MASK));

    return BerUniversalTagNumber::e_BER_REAL;
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const DoubleSel&)
{
    BSLS_ASSERT_SAFE(
          FMode::e_DEFAULT == (d_formattingMode & FMode::e_TYPE_MASK)
       || FMode::e_DEC     == (d_formattingMode & FMode::e_TYPE_MASK));

    return BerUniversalTagNumber::e_BER_REAL;
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const Decimal64Sel&)
{
    BSLS_ASSERT_SAFE(
          FMode::e_DEFAULT == (d_formattingMode & FMode::e_TYPE_MASK)
       || FMode::e_DEC     == (d_formattingMode & FMode::e_TYPE_MASK));

    return BerUniversalTagNumber::e_BER_OCTET_STRING;
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const StringSel&)
{
    return selectForStringTypes();
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const StringViewSel&)
{
    return selectForStringTypes();
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const StringRefSel&)
{
    return selectForStringTypes();
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const DateSel&)
{
    return selectForDateAndTimeTypes();
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const DateTzSel&)
{
    return selectForDateAndTimeTypes();
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const DatetimeSel&)
{
    return selectForDateAndTimeTypes();
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const DatetimeTzSel&)
{
    return selectForDateAndTimeTypes();
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const TimeSel&)
{
    return selectForDateAndTimeTypes();
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const TimeTzSel&)
{
    return selectForDateAndTimeTypes();
}

template <typename TYPE, typename TYPETZ>
inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(
     const BerUniversalTagNumber_Sel<bdlb::Variant2<TYPE, TYPETZ>, SimpleCat>&)
{
    BSLMF_ASSERT((bslmf::IsSame<bdlt::Date,       TYPE  >::value
               && bslmf::IsSame<bdlt::DateTz,     TYPETZ>::value)
              || (bslmf::IsSame<bdlt::Time,       TYPE  >::value
               && bslmf::IsSame<bdlt::TimeTz,     TYPETZ>::value)
              || (bslmf::IsSame<bdlt::Datetime,   TYPE  >::value
               && bslmf::IsSame<bdlt::DatetimeTz, TYPETZ>::value));

    return selectForDateAndTimeTypes();
}

inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(const CharVectorSel&)
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

                             //  ** By Category **

template <typename TYPE>
inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(
                     const BerUniversalTagNumber_Sel<TYPE, CustomizedTypeCat>&)
    ///Implementation Note
    ///-------------------
    // The BER universal tag number of the specified customized type 'TYPE' is
    // defined to be the BER universal tag number of the base type of 'TYPE'.
{
    typedef typename
    bdlat_CustomizedTypeFunctions::BaseType<TYPE>::Type BaseType;
        // To compute the universal tag number of the specified 'TYPE', first
        // compute the 'BaseType' of 'TYPE'.

    typedef typename
    bdlat_TypeCategory::Select<BaseType>::Type          BaseTypeCategory;
        // Then, determine the 'bdlat' type category of the 'BaseType',
        // 'BaseTypeCategory'.

    typedef BerUniversalTagNumber_Sel<BaseType,
                                      BaseTypeCategory> BaseTypeSelector;
        // Next, construct a 'BerUniversalTagNumber_Sel' type specialized with
        // the computed 'BaseType' and 'BaseTypeCategory'.

    return this->select(BaseTypeSelector());
        // Finally, return the result of computing the universal tag number of
        // the base type of 'TYPE' by using the 'BaseTypeSelector' to pick an
        // overload of 'select' that performs said computation.
}

template <typename TYPE>
inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(
                        const BerUniversalTagNumber_Sel<TYPE, DynamicTypeCat>&)
    ///Implementation Note
    ///-------------------
    // Universal tag numbers are used only in the encoding of top-level
    // objects.  Objects having the 'DynamicType' 'bdlat' type category
    // at any point other than their top level do not have a well defined
    // universal tag number.  For the purpose of this component, an object
    // is a top-level object if it is not a member of any other object.
{
    BSLS_ASSERT_OPT(0 && "Sub-objects having the 'DynamicType' category are "
                         "unsupported.  Only top-level objects may have "
                         "a dynamic category.");

    return BerUniversalTagNumber::e_BER_INVALID;
}

template <typename TYPE>
inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(
                        const BerUniversalTagNumber_Sel<TYPE, EnumerationCat>&)
{
    BSLS_ASSERT_SAFE(
          FMode::e_DEFAULT == (d_formattingMode & FMode::e_TYPE_MASK)
       || FMode::e_DEC     == (d_formattingMode & FMode::e_TYPE_MASK)
       || FMode::e_TEXT    == (d_formattingMode & FMode::e_TYPE_MASK));

    // We also allow decoding integer values into enums (DRQS 166048981).
    d_alternateTag = BerUniversalTagNumber::e_BER_INT;

    return BerUniversalTagNumber::e_BER_ENUMERATION;
}

template <typename TYPE>
inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(
                              const BerUniversalTagNumber_Sel<TYPE, ArrayCat>&)
{
    BSLS_ASSERT_SAFE(
          FMode::e_DEFAULT == (d_formattingMode & FMode::e_TYPE_MASK));

    return BerUniversalTagNumber::e_BER_SEQUENCE;
}

template <typename TYPE>
inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(
                           const BerUniversalTagNumber_Sel<TYPE, SequenceCat>&)
{
    BSLS_ASSERT_SAFE(
          FMode::e_DEFAULT == (d_formattingMode & FMode::e_TYPE_MASK));

    return BerUniversalTagNumber::e_BER_SEQUENCE;
}

template <typename TYPE>
inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(
                             const BerUniversalTagNumber_Sel<TYPE, ChoiceCat>&)
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
BerUniversalTagNumber_Imp::select(
                      const BerUniversalTagNumber_Sel<TYPE, NullableValueCat>&)
    ///Implementation Note
    ///-------------------
    // The BER universal tag number of the specified nullable type 'TYPE' is
    // defined to be the BER universal tag number of the value type of 'TYPE'
    // unless 'TYPE' has a "nillable" formatting mode flag set, in which case
    // the tag number is the tag number for sequences.
{
    if (d_formattingMode & FMode::e_NILLABLE) {
        return BerUniversalTagNumber::e_BER_SEQUENCE;
    }

    // If control flow gets here, then the 'TYPE' is nullable, but not
    // nillable.

    typedef typename
    bdlat_NullableValueFunctions::ValueType<TYPE>::Type  ValueType;
        // To compute the universal tag number of the specified 'TYPE', first
        // compute the 'ValueType' of 'TYPE'.

    typedef typename
    bdlat_TypeCategory::Select<ValueType>::Type          ValueTypeCategory;
        // Then, determine the 'bdlat' type category of 'ValueType',
        // 'ValueTypeCategory'.

    typedef BerUniversalTagNumber_Sel<ValueType,
                                      ValueTypeCategory> ValueTypeSelector;
        // Next, construct a 'BerUniversalTagNumber_Sel' type specialized with
        // the computed 'ValueType' and 'ValueTypeCategory'.

    return this->select(ValueTypeSelector());
        // Finally, return the result of computing the universal tag number of
        // the value of 'TYPE' by using the 'ValueTypeSelector' to pick an
        // overload of 'select' that performs said computation.
}

template <typename TYPE, typename ANY_CATEGORY>
inline
BerUniversalTagNumber::Value
BerUniversalTagNumber_Imp::select(
                          const BerUniversalTagNumber_Sel<TYPE, ANY_CATEGORY>&)
{
    BSLS_ASSERT(0 && "invalid type category");
    return BerUniversalTagNumber::e_BER_INVALID;
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
