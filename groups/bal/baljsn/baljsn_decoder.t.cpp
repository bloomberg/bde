// baljsn_decoder.t.cpp                                               -*-C++-*-
#include <baljsn_decoder.h>

#include <baljsn_decoderoptions.h>
#include <baljsn_encoder.h>
#include <baljsn_encoderoptions.h>
#include <baljsn_printutil.h>

// These header are for testing only and the hierarchy level of `baljsn` was
// increase because of them.  They should be remove when possible.
#include <balb_testmessages.h>

#include <balxml_decoder.h>
#include <balxml_decoderoptions.h>
#include <balxml_minireader.h>
#include <balxml_errorinfo.h>

// For round-trip testing
#include <baljsn_encoder.h>
#include <baljsn_encoderoptions.h>
#include <baljsn_encodingstyle.h>

#include <s_baltst_address.h>
#include <s_baltst_employee.h>
#include <s_baltst_mysequencewithprecisiondecimalattribute.h> // TC19 Decimal64
#include <s_baltst_sqrt.h>
#include <s_baltst_sqrtf.h>

#include <bdlat_attributeinfo.h>
#include <bdlat_choicefunctions.h>
#include <bdlat_enumeratorinfo.h>
#include <bdlat_enumfunctions.h>
#include <bdlat_selectioninfo.h>
#include <bdlat_sequencefunctions.h>
#include <bdlat_typetraits.h>
#include <bdlat_valuetypefunctions.h>

#include <bdlb_chartype.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>  // for printing vector

#include <bdldfp_decimal.h>
#include <bdldfp_decimalutil.h>

#include <bdlde_utf8util.h>

#include <bdlt_datetimetz.h>

#include <bdlsb_fixedmeminstreambuf.h>
#include <bdlsb_fixedmemoutstreambuf.h>

#include <bdlpcre_regex.h>

#include <bslmt_testutil.h>

#include <bslim_printer.h>
#include <bslmt_threadutil.h>

#include <s_baltst_address.h>
#include <s_baltst_basicrecord.h>
#include <s_baltst_bigrecord.h>
#include <s_baltst_depthtestmessageutil.h>
#include <s_baltst_employee.h>
#include <s_baltst_generatetestarray.h>
#include <s_baltst_generatetestchoice.h>
#include <s_baltst_generatetestcustomizedtype.h>
#include <s_baltst_generatetestdynamictype.h>
#include <s_baltst_generatetestenumeration.h>
#include <s_baltst_generatetestnullablevalue.h>
#include <s_baltst_generatetestsequence.h>
#include <s_baltst_generatetesttaggedvalue.h>
#include <s_baltst_myenumerationwithfallback.h>
#include <s_baltst_myintenumeration.h>
#include <s_baltst_mysequencewitharray.h>
#include <s_baltst_mysequencewithchoice.h>
#include <s_baltst_mysequencewithdefault.h>
#include <s_baltst_mysequencewithnillable.h>
#include <s_baltst_testchoice.h>
#include <s_baltst_testcustomizedtype.h>
#include <s_baltst_testdynamictype.h>
#include <s_baltst_testenumeration.h>
#include <s_baltst_testnilvalue.h>
#include <s_baltst_testplaceholder.h>
#include <s_baltst_testselection.h>
#include <s_baltst_testsequence.h>
#include <s_baltst_testtaggedvalue.h>

#include <bsla_maybeunused.h>

#include <bsls_libraryfeatures.h>

#include <bsl_climits.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_iterator.h>
#include <bsl_limits.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_vector.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
namespace test = BloombergLP::s_baltst;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test implements an decoder for decoding into
// `bdlat`-compatible objects in the JSON format.  The object types that can be
// decoded include `bdlat` sequence, choice, array, enumeration, customized,
// simple, and dynamic types.  In addition, the decoder supports options to
// specify the max depth and whether unknown elements should be skipped.
//
// We use standard table-based approach to testing where we put both input and
// expected output in the same table row and verify that the actual result
// matches the expected value.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] baljsn::Decoder(bslma::Allocator *basicAllocator = 0);
// [ 2] ~baljsn::Decoder();
//
// MANIPULATORS
// [ 4] int decode(bsl::streambuf *streamBuf, TYPE *v, options);
// [ 4] int decode(bsl::istream& stream, TYPE *v, options);
// [ 4] int decode(bsl::streambuf *streamBuf, TYPE *v, &options);
// [ 4] int decode(bsl::istream& stream, TYPE *v, &options);
//
// ACCESSORS
// [ 4] bsl::string loggedMessages() const;
// [ 3] int numUnknownElementsSkipped() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] TESTING SKIPPING UNKNOWN ELEMENTS
// [ 4] TESTING INVALID JSON RETURNS AN ERROR
// [ 5] MULTI-THREADING TEST CASE                           {DRQS 41660550<GO>}
// [ 6] TESTING DECODING OF `hexBinary` CUSTOMIZED TYPE     {DRQS 43702912<GO>}
// [ 7] TESTING DECODING OF ENUM TYPES WITH ESCAPED CHARS
// [ 8] TESTING CLEARING OF LOGGED MESSAGES ON DECODE CALLS
// [ 9] TESTING UTF-8 DETECTION
// [10] TESTING DECODING VECTORS OF VECTORS
// [11] FLOATING-POINT VALUES ROUND-TRIP
// [12] REPRODUCE SCENARIO FROM DRQS 169438741
// [13] FALLBACK ENUMERATORS
// [14] DECODING INTS AS ENUMS AND VICE VERSA              {DRQS 166048981<GO>}
// [15] ARRAY HAVING NULLABLE COMPLEX ELEMENTS             {DRQS 167908706<GO>}
// [16] `DecoderOptions` CAN BE CONFIGURED FOR STRICT CONFORMANCE
// [17] MAXDEPTH IS RESPECTED
// [18] `allowMissingRequiredAttributes` OPTION
// [19] TESTING `Decimal64`
// [20] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLMT_TESTUTIL_ASSERT
#define ASSERTV      BSLMT_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLMT_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLMT_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLMT_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLMT_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLMT_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLMT_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLMT_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLMT_TESTUTIL_LOOP6_ASSERT

#define Q            BSLMT_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLMT_TESTUTIL_P   // Print identifier and value.
#define P_           BSLMT_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLMT_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLMT_TESTUTIL_L_  // current Line number

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

#if defined(BSLS_PLATFORM_CMP_IBM)
#define U_SKIP_DUE_TO_COMPILER_RESOURCE_LIMITATIONS
#endif

typedef baljsn::Decoder Obj;

typedef s_baltst::DepthTestMessage     DepthTestMessage;
typedef s_baltst::DepthTestMessageUtil DTMU;

namespace BloombergLP {
namespace s_baltst {

                        // ============================
                        // class template RoundTripData
                        // ============================

                                // ============
                                // class Colors
                                // ============

struct Colors {

  public:
    // TYPES
    enum Value {
        BLUE_QUOTE_YELLOW                                               = 0
      , GREY_BACKSLASH_BLUE                                             = 1
      , RED_SLASH_GREEN                                                 = 2
      , RED_BACKSLASH_B_BACKSLASH_R_BACKSLASH_T_BACKSLASH_F_BACKSLASH_N = 3
      , WHITE_TICK_BLACK                                                = 4
    };

    enum {
        NUM_ENUMERATORS = 5
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_EnumeratorInfo ENUMERATOR_INFO_ARRAY[];

    // CLASS METHODS

    /// Return the string representation exactly matching the enumerator
    /// name corresponding to the specified enumeration `value`.
    static const char *toString(Value value);

    /// Load into the specified `result` the enumerator matching the
    /// specified `string` of the specified `stringLength`.  Return 0 on
    /// success, and a non-zero value with no effect on `result` otherwise
    /// (i.e., `string` does not match any enumerator).
    static int fromString(Value        *result,
                          const char   *string,
                          int           stringLength);

    /// Load into the specified `result` the enumerator matching the
    /// specified `string`.  Return 0 on success, and a non-zero value with
    /// no effect on `result` otherwise (i.e., `string` does not match any
    /// enumerator).
    static int fromString(Value              *result,
                          const bsl::string&  string);

    /// Load into the specified `result` the enumerator matching the
    /// specified `number`.  Return 0 on success, and a non-zero value with
    /// no effect on `result` otherwise (i.e., `number` does not match any
    /// enumerator).
    static int fromInt(Value *result, int number);

    /// Write to the specified `stream` the string representation of
    /// the specified enumeration `value`.  Return a reference to
    /// the modifiable `stream`.
    static bsl::ostream& print(bsl::ostream& stream, Value value);
};

// FREE OPERATORS

/// Format the specified `rhs` to the specified output `stream` and
/// return a reference to the modifiable `stream`.
inline
bsl::ostream& operator<<(bsl::ostream& stream, Colors::Value rhs);

}  // close namespace s_baltst

// TRAITS

BDLAT_DECL_ENUMERATION_TRAITS(test::Colors)

namespace s_baltst {

                               // =============
                               // class Palette
                               // =============

class Palette {

    // INSTANCE DATA
    bsl::vector<Colors::Value>  d_colors;
    Colors::Value               d_color;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_COLOR  = 0
      , ATTRIBUTE_ID_COLORS = 1
    };

    enum {
        NUM_ATTRIBUTES = 2
    };

    enum {
        ATTRIBUTE_INDEX_COLOR  = 0
      , ATTRIBUTE_INDEX_COLORS = 1
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

  public:
    // CLASS METHODS

    /// Return attribute information for the attribute indicated by the
    /// specified `id` if the attribute exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(int id);

    /// Return attribute information for the attribute indicated by the
    /// specified `name` of the specified `nameLength` if the attribute
    /// exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(
                                                    const char *name,
                                                    int         nameLength);

    // CREATORS

    /// Create an object of type `Palette` having the default value.
    /// Optionally specify a `basicAllocator` used to supply memory.  If
    /// `basicAllocator` is 0, the currently installed default allocator is
    /// used.
    explicit Palette(bslma::Allocator *basicAllocator = 0);

    /// Create an object of type `Palette` having the value of the specified
    /// `original` object.  Optionally specify a `basicAllocator` used to
    /// supply memory.  If `basicAllocator` is 0, the currently installed
    /// default allocator is used.
    Palette(const Palette& original,
            bslma::Allocator *basicAllocator = 0);

    /// Destroy this object.
    ~Palette();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object.
    Palette& operator=(const Palette& rhs);

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Invoke the specified `manipulator` sequentially on the address of
    /// each (modifiable) attribute of this object, supplying `manipulator`
    /// with the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the
    /// last invocation of `manipulator` (i.e., the invocation that
    /// terminated the sequence).
    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);

    /// Invoke the specified `manipulator` on the address of
    /// the (modifiable) attribute indicated by the specified `id`,
    /// supplying `manipulator` with the corresponding attribute
    /// information structure.  Return the value returned from the
    /// invocation of `manipulator` if `id` identifies an attribute of this
    /// class, and -1 otherwise.
    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);

    /// Invoke the specified `manipulator` on the address of
    /// the (modifiable) attribute indicated by the specified `name` of the
    /// specified `nameLength`, supplying `manipulator` with the
    /// corresponding attribute information structure.  Return the value
    /// returned from the invocation of `manipulator` if `name` identifies
    /// an attribute of this class, and -1 otherwise.
    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);

    /// Return a reference to the modifiable "Color" attribute of this
    /// object.
    Colors::Value& color();

    /// Return a reference to the modifiable "Colors" attribute of this
    /// object.
    bsl::vector<Colors::Value>& colors();

    // ACCESSORS

    /// Format this object to the specified output `stream` at the
    /// optionally specified indentation `level` and return a reference to
    /// the modifiable `stream`.  If `level` is specified, optionally
    /// specify `spacesPerLevel`, the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of `level * spacesPerLevel`.  If `level` is
    /// negative, suppress indentation of the first line.  If
    /// `spacesPerLevel` is negative, suppress line breaks and format the
    /// entire output on one line.  If `stream` is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;

    /// Invoke the specified `accessor` sequentially on each
    /// (non-modifiable) attribute of this object, supplying `accessor`
    /// with the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the
    /// last invocation of `accessor` (i.e., the invocation that terminated
    /// the sequence).
    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute
    /// of this object indicated by the specified `id`, supplying `accessor`
    /// with the corresponding attribute information structure.  Return the
    /// value returned from the invocation of `accessor` if `id` identifies
    /// an attribute of this class, and -1 otherwise.
    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute
    /// of this object indicated by the specified `name` of the specified
    /// `nameLength`, supplying `accessor` with the corresponding attribute
    /// information structure.  Return the value returned from the
    /// invocation of `accessor` if `name` identifies an attribute of this
    /// class, and -1 otherwise.
    template<class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;

    /// Return a reference to the non-modifiable "Color" attribute of this
    /// object.
    Colors::Value color() const;

    /// Return a reference to the non-modifiable "Colors" attribute of this
    /// object.
    const bsl::vector<Colors::Value>& colors() const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` attribute objects have
/// the same value, and `false` otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
inline
bool operator==(const Palette& lhs, const Palette& rhs);

/// Return `true` if the specified `lhs` and `rhs` attribute objects do not
/// have the same value, and `false` otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
inline
bool operator!=(const Palette& lhs, const Palette& rhs);

/// Format the specified `rhs` to the specified output `stream` and
/// return a reference to the modifiable `stream`.
inline
bsl::ostream& operator<<(bsl::ostream& stream, const Palette& rhs);

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(test::Palette)

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace s_baltst {

                                // ------------
                                // class Colors
                                // ------------

// CLASS METHODS
inline
int Colors::fromString(Value *result, const bsl::string& string)
{
    return fromString(result,
                      string.c_str(),
                      static_cast<int>(string.length()));
}

inline
bsl::ostream& Colors::print(bsl::ostream&      stream,
                                 Colors::Value value)
{
    return stream << toString(value);
}

                               // -------------
                               // class Palette
                               // -------------

// CLASS METHODS
template <class MANIPULATOR>
int Palette::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_color, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COLOR]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_colors, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COLORS]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int Palette::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_COLOR: {
        return manipulator(&d_color,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COLOR]);
      } break;
      case ATTRIBUTE_ID_COLORS: {
        return manipulator(&d_colors,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COLORS]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int Palette::manipulateAttribute(
        MANIPULATOR&  manipulator,
        const char   *name,
        int           nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
Colors::Value& Palette::color()
{
    return d_color;
}

inline
bsl::vector<Colors::Value>& Palette::colors()
{
    return d_colors;
}

// ACCESSORS
template <class ACCESSOR>
int Palette::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_color, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COLOR]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_colors, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COLORS]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int Palette::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_COLOR: {
        return accessor(d_color, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COLOR]);
      } break;
      case ATTRIBUTE_ID_COLORS: {
      return accessor(d_colors, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COLORS]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int Palette::accessAttribute(
        ACCESSOR&   accessor,
        const char *name,
        int         nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

inline
Colors::Value Palette::color() const
{
    return d_color;
}

inline
const bsl::vector<Colors::Value>& Palette::colors() const
{
    return d_colors;
}

}  // close package namespace

// FREE FUNCTIONS

inline
bsl::ostream& test::operator<<(
        bsl::ostream& stream,
        test::Colors::Value rhs)
{
    return test::Colors::print(stream, rhs);
}

inline
bool test::operator==(
        const test::Palette& lhs,
        const test::Palette& rhs)
{
    return  lhs.color()  == rhs.color()
         && lhs.colors() == rhs.colors();
}

inline
bool test::operator!=(
        const test::Palette& lhs,
        const test::Palette& rhs)
{
    return  lhs.color()  != rhs.color()
         || lhs.colors() != rhs.colors();
}

inline
bsl::ostream& test::operator<<(
        bsl::ostream& stream,
        const test::Palette& rhs)
{
    return rhs.print(stream, 0, -1);
}

namespace s_baltst {

                                // ------------
                                // class Colors
                                // ------------

// CONSTANTS

const char Colors::CLASS_NAME[] = "Colors";

const bdlat_EnumeratorInfo Colors::ENUMERATOR_INFO_ARRAY[] = {
    {
        Colors::BLUE_QUOTE_YELLOW,
        "BLUE\"YELLOW",
        sizeof("BLUE\"YELLOW") - 1,
        ""
    },
    {
        Colors::GREY_BACKSLASH_BLUE,
        "GREY\\BLUE",
        sizeof("GREY\\BLUE") - 1,
        ""
    },
    {
        Colors::RED_SLASH_GREEN,
        "RED/GREEN",
        sizeof("RED/GREEN") - 1,
        ""
    },
    {
       Colors::RED_BACKSLASH_B_BACKSLASH_R_BACKSLASH_T_BACKSLASH_F_BACKSLASH_N,
       "RED\b\r\t\f\n",
       sizeof("RED\b\r\t\f\n") - 1,
       ""
    },
    {
        Colors::WHITE_TICK_BLACK,
        "WHITE'BLACK",
        sizeof("WHITE'BLACK") - 1,
        ""
    }
};

// CLASS METHODS

int Colors::fromInt(Colors::Value *result, int number)
{
    switch (number) {
  case Colors::BLUE_QUOTE_YELLOW:
  case Colors::GREY_BACKSLASH_BLUE:
  case Colors::RED_SLASH_GREEN:
  case Colors::RED_BACKSLASH_B_BACKSLASH_R_BACKSLASH_T_BACKSLASH_F_BACKSLASH_N:
  case Colors::WHITE_TICK_BLACK:
        *result = (Colors::Value)number;
        return 0;
      default:
        return -1;
    }
}

int Colors::fromString(
        Colors::Value *result,
        const char         *string,
        int                 stringLength)
{
    for (int i = 0; i < 5; ++i) {
        const bdlat_EnumeratorInfo& enumeratorInfo =
                    Colors::ENUMERATOR_INFO_ARRAY[i];

        if (stringLength == enumeratorInfo.d_nameLength
        &&  0 == bsl::memcmp(enumeratorInfo.d_name_p, string, stringLength))
        {
            *result = (Colors::Value)enumeratorInfo.d_value;
            return 0;
        }
    }

    return -1;
}

const char *Colors::toString(Colors::Value value)
{
    switch (value) {
      case BLUE_QUOTE_YELLOW: {
        return "BLUE\"YELLOW";
      } break;
      case GREY_BACKSLASH_BLUE: {
        return "GREY\\BLUE";
      } break;
      case RED_SLASH_GREEN: {
        return "RED/GREEN";
      } break;
      case RED_BACKSLASH_B_BACKSLASH_R_BACKSLASH_T_BACKSLASH_F_BACKSLASH_N: {
        return "RED\b\r\t\f\n";
      } break;
      case WHITE_TICK_BLACK: {
        return "WHITE'BLACK";
      } break;
    }

    BSLS_ASSERT(0 == "invalid enumerator");
    return 0;
}

                               // -------------
                               // class Palette
                               // -------------

// CONSTANTS

const char Palette::CLASS_NAME[] = "Palette";

const bdlat_AttributeInfo Palette::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_COLOR,
        "color",
        sizeof("color") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        ATTRIBUTE_ID_COLORS,
        "colors",
        sizeof("colors") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *Palette::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 2; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    Palette::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *Palette::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_COLOR:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COLOR];
      case ATTRIBUTE_ID_COLORS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COLORS];
      default:
        return 0;
    }
}

// CREATORS

Palette::Palette(bslma::Allocator *basicAllocator)
: d_colors(basicAllocator)
, d_color(static_cast<Colors::Value>(0))
{
}

Palette::Palette(const Palette& original,
                 bslma::Allocator *basicAllocator)
: d_colors(original.d_colors, basicAllocator)
, d_color(original.d_color)
{
}

Palette::~Palette()
{
}

// MANIPULATORS

Palette&
Palette::operator=(const Palette& rhs)
{
    if (this != &rhs) {
        d_color = rhs.d_color;
        d_colors = rhs.d_colors;
    }

    return *this;
}

void Palette::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_color);
    bdlat_ValueTypeFunctions::reset(&d_colors);
}

// ACCESSORS

bsl::ostream& Palette::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("color", d_color);
    printer.printAttribute("colors", d_colors);
    printer.end();
    return stream;
}

}  // close namespace s_baltst

}  // close enterprise namespace

void constructFeatureTestMessage(
                                bsl::vector<balb::FeatureTestMessage> *objects)
{
    balxml::MiniReader     reader;
    balxml::DecoderOptions options; options.setSkipUnknownElements(true);
    balxml::ErrorInfo      e;
    balxml::Decoder        decoder(&options, &reader, &e);

    for (int i = 0; i < DTMU::k_NUM_MESSAGES; ++i) {
        balb::FeatureTestMessage object;
        bsl::istringstream ss(DTMU::s_TEST_MESSAGES[i].d_XML_text_p);

        int rc = decoder.decode(ss.rdbuf(), &object);
        if (0 != rc) {
            cout << "Failed to decode from initialization data (i="
                 << i << "): "
                 << decoder.loggedMessages() << endl;
        }
        if (balb::FeatureTestMessage::SELECTION_ID_UNDEFINED ==
                                                        object.selectionId()) {
            cout << "Decoded unselected choice from initialization data"
                 << " (LINE =" << i << "):"
                 << endl;
            rc = 9;
        }
        ASSERT(0 == rc); // test invariant
        objects->push_back(object);
    }
}

namespace CASE5 {

struct ThreadData {
    const bsl::vector<balb::FeatureTestMessage> *d_testObjects_p;
    bool                                         d_veryVerbose;
    bool                                         d_checkUtf8;
};

extern "C" void *threadFunction(void *data)
{
    ThreadData& threadData = *(ThreadData *) data;
    const bsl::vector<balb::FeatureTestMessage>& testObjects =
                                                   *threadData.d_testObjects_p;
    bool veryVerbose = threadData.d_veryVerbose;

    for (int ti = 0; ti < DTMU::k_NUM_MESSAGES; ++ti) {
        const int          LINE = ti;
        const bsl::string& PRETTY = DTMU::s_TEST_MESSAGES[ti].d_prettyJSON_p;
        const balb::FeatureTestMessage& EXP = testObjects[ti];

        if (veryVerbose) {
            P(ti);    P(LINE);    P(PRETTY);
            EXP.print(cout, 1, 4);
        }

        {
            balb::FeatureTestMessage  value;
            baljsn::DecoderOptions     options;
            options.setValidateInputIsUtf8(threadData.d_checkUtf8);
            baljsn::Decoder            decoder;
            bdlsb::FixedMemInStreamBuf isb(PRETTY.data(), PRETTY.length());

            const int rc = decoder.decode(&isb, &value, options);
            ASSERTV(LINE, decoder.loggedMessages(), rc, 0 == rc);
            ASSERTV(LINE, isb.length(), 1 == isb.length()); // trailing newline
            ASSERTV(LINE, decoder.loggedMessages(), EXP, value, EXP == value);
        }

        {
            balb::FeatureTestMessage  value;
            baljsn::DecoderOptions     options;
            options.setValidateInputIsUtf8(threadData.d_checkUtf8);
            baljsn::Decoder            decoder;
            bdlsb::FixedMemInStreamBuf isb(PRETTY.data(), PRETTY.length());
            bsl::istream              iss(&isb);

            const int rc = decoder.decode(iss, &value, options);
            ASSERTV(LINE, decoder.loggedMessages(), rc, 0 == rc);
            ASSERTV(LINE, isb.length(), 1 == isb.length()); // trailing newline
            ASSERTV(LINE, decoder.loggedMessages(), EXP, value, EXP == value);
        }
    }

    for (int ti = 0; ti < DTMU::k_NUM_MESSAGES; ++ti) {
        const int          LINE    = ti;
        const bsl::string& COMPACT = DTMU::s_TEST_MESSAGES[ti].d_compactJSON_p;
        const balb::FeatureTestMessage& EXP = testObjects[ti];

        if (veryVerbose) {
            P(ti);    P(LINE);    P(COMPACT);
            EXP.print(cout, 1, 4);
        }

        {
            balb::FeatureTestMessage value;

            baljsn::DecoderOptions     options;
            options.setValidateInputIsUtf8(threadData.d_checkUtf8);
            baljsn::Decoder            decoder;
            bdlsb::FixedMemInStreamBuf isb(COMPACT.data(), COMPACT.length());
            bsl::istream              iss(&isb);

            const int rc = decoder.decode(&isb, &value, options);
            ASSERTV(LINE, decoder.loggedMessages(), rc, 0 == rc);
            ASSERTV(LINE, isb.length(), 0 == isb.length());
            ASSERTV(LINE, decoder.loggedMessages(), EXP, value, EXP == value);
        }
    }
    return (void *) 0;
}

}  // close namespace CASE5

namespace BloombergLP {
namespace bslma { class Allocator; }
namespace case4 { class FullName; }
namespace case4 { class Employee; }
namespace case4 {

                                // ===========
                                // class Color
                                // ===========

struct Color {

  public:
    // TYPES
    enum Value {
        e_RED   = 0
      , e_GREEN = 1
      , e_BLUE  = 2
    };

    enum {
        k_NUM_ENUMERATORS = 3
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_EnumeratorInfo ENUMERATOR_INFO_ARRAY[];

    // CLASS METHODS

    /// Return the string representation exactly matching the enumerator
    /// name corresponding to the specified enumeration `value`.
    static const char *toString(Value value);

    /// Load into the specified `result` the enumerator matching the
    /// specified `string` of the specified `stringLength`.  Return 0 on
    /// success, and a non-zero value with no effect on `result` otherwise
    /// (i.e., `string` does not match any enumerator).
    static int fromString(Value        *result,
                          const char   *string,
                          int           stringLength);

    /// Load into the specified `result` the enumerator matching the
    /// specified `string`.  Return 0 on success, and a non-zero value with
    /// no effect on `result` otherwise (i.e., `string` does not match any
    /// enumerator).
    static int fromString(Value              *result,
                          const bsl::string&  string);

    /// Load into the specified `result` the enumerator matching the
    /// specified `number`.  Return 0 on success, and a non-zero value with
    /// no effect on `result` otherwise (i.e., `number` does not match any
    /// enumerator).
    static int fromInt(Value *result, int number);

    /// Write to the specified `stream` the string representation of the
    /// specified enumeration `value`.  Return a reference to the modifiable
    /// `stream`.
    static bsl::ostream& print(bsl::ostream& stream, Value value);
};

// FREE OPERATORS

/// Format the specified `rhs` to the specified output `stream` and return a
/// reference to the modifiable `stream`.
inline
bsl::ostream& operator<<(bsl::ostream& stream, Color::Value rhs);

}  // close namespace case4

// TRAITS

BDLAT_DECL_ENUMERATION_TRAITS(case4::Color)

namespace case4 {

                               // ==============
                               // class FullName
                               // ==============

class FullName {

    // INSTANCE DATA
    bsl::vector<int>  d_ids;
    bsl::string       d_name;

  public:
    // TYPES
    enum {
        k_ATTRIBUTE_ID_NAME = 0
      , k_ATTRIBUTE_ID_IDS  = 1
    };

    enum {
        k_NUM_ATTRIBUTES = 2
    };

    enum {
        k_ATTRIBUTE_INDEX_NAME = 0
      , k_ATTRIBUTE_INDEX_IDS  = 1
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

  public:
    // CLASS METHODS

    /// Return attribute information for the attribute indicated by the
    /// specified `id` if the attribute exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(int id);

    /// Return attribute information for the attribute indicated by the
    /// specified `name` of the specified `nameLength` if the attribute
    /// exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(
                                                       const char *name,
                                                       int         nameLength);

    // CREATORS

    /// Create an object of type `FullName` having the default value.
    /// Optionally specify a `basicAllocator` used to supply memory.  If
    /// `basicAllocator` is 0, the currently installed default allocator is
    /// used.
    explicit FullName(bslma::Allocator *basicAllocator = 0);

    /// Create an object of type `FullName` having the value of the
    /// specified `original` object.  Optionally specify a `basicAllocator`
    /// used to supply memory.  If `basicAllocator` is 0, the currently
    /// installed default allocator is used.
    FullName(const FullName& original, bslma::Allocator *basicAllocator = 0);

    /// Destroy this object.
    ~FullName();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object.
    FullName& operator=(const FullName& rhs);

    /// Reset this object to the default value (i.e., its value upon default
    /// construction).
    void reset();

    /// Invoke the specified `manipulator` sequentially on the address of
    /// each (modifiable) attribute of this object, supplying `manipulator`
    /// with the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the last
    /// invocation of `manipulator` (i.e., the invocation that terminated
    /// the sequence).
    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);

    /// Invoke the specified `manipulator` on the address of the
    /// (modifiable) attribute indicated by the specified `id`, supplying
    /// `manipulator` with the corresponding attribute information
    /// structure.  Return the value returned from the invocation of
    /// `manipulator` if `id` identifies an attribute of this class, and -1
    /// otherwise.
    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);

    /// Invoke the specified `manipulator` on the address of the
    /// (modifiable) attribute indicated by the specified `name` of the
    /// specified `nameLength`, supplying `manipulator` with the
    /// corresponding attribute information structure.  Return the value
    /// returned from the invocation of `manipulator` if `name` identifies
    /// an attribute of this class, and -1 otherwise.
    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);

    /// Return a reference to the modifiable "Name" attribute of this
    /// object.
    bsl::string& name();

    /// Return a reference to the modifiable "Ids" attribute of this object.
    bsl::vector<int>& ids();

    // ACCESSORS

    /// Invoke the specified `accessor` sequentially on each
    /// (non-modifiable) attribute of this object, supplying `accessor` with
    /// the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the last
    /// invocation of `accessor` (i.e., the invocation that terminated the
    /// sequence).
    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute of
    /// this object indicated by the specified `id`, supplying `accessor`
    /// with the corresponding attribute information structure.  Return the
    /// value returned from the invocation of `accessor` if `id` identifies
    /// an attribute of this class, and -1 otherwise.
    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute of
    /// this object indicated by the specified `name` of the specified
    /// `nameLength`, supplying `accessor` with the corresponding attribute
    /// information structure.  Return the value returned from the
    /// invocation of `accessor` if `name` identifies an attribute of this
    /// class, and -1 otherwise.
    template<class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;

    /// Return a reference to the non-modifiable "Name" attribute of this
    /// object.
    const bsl::string& name() const;

    /// Return a reference to the non-modifiable "Ids" attribute of this
    /// object.
    const bsl::vector<int>& ids() const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` attribute objects have
/// the same value, and `false` otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
inline
bool operator==(const FullName& lhs, const FullName& rhs);

/// Return `true` if the specified `lhs` and `rhs` attribute objects do not
/// have the same value, and `false` otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
inline
bool operator!=(const FullName& lhs, const FullName& rhs);

}  // close namespace case4

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(case4::FullName)

namespace case4 {

                               // ==============
                               // class Employee
                               // ==============

class Employee {

    // INSTANCE DATA
    bsl::vector<int>       d_ids;
    bsl::vector<FullName>  d_friends;
    bsl::string            d_name;
    FullName               d_fullname;
    int                    d_age;
    Color::Value           d_carColor;

  public:
    // TYPES
    enum {
        k_ATTRIBUTE_ID_NAME      = 0
      , k_ATTRIBUTE_ID_AGE       = 1
      , k_ATTRIBUTE_ID_IDS       = 2
      , k_ATTRIBUTE_ID_FULLNAME  = 3
      , k_ATTRIBUTE_ID_CAR_COLOR = 4
      , k_ATTRIBUTE_ID_FRIENDS   = 5
    };

    enum {
        k_NUM_ATTRIBUTES = 6
    };

    enum {
        k_ATTRIBUTE_INDEX_NAME      = 0
      , k_ATTRIBUTE_INDEX_AGE       = 1
      , k_ATTRIBUTE_INDEX_IDS       = 2
      , k_ATTRIBUTE_INDEX_FULLNAME  = 3
      , k_ATTRIBUTE_INDEX_CAR_COLOR = 4
      , k_ATTRIBUTE_INDEX_FRIENDS   = 5
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

  public:
    // CLASS METHODS

    /// Return attribute information for the attribute indicated by the
    /// specified `id` if the attribute exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(int id);

    /// Return attribute information for the attribute indicated by the
    /// specified `name` of the specified `nameLength` if the attribute
    /// exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(
                                                       const char *name,
                                                       int         nameLength);

    // CREATORS

    /// Create an object of type `Employee` having the default value.
    /// Optionally specify a `basicAllocator` used to supply memory.  If
    /// `basicAllocator` is 0, the currently installed default allocator is
    /// used.
    explicit Employee(bslma::Allocator *basicAllocator = 0);

    /// Create an object of type `Employee` having the value of the
    /// specified `original` object.  Optionally specify a `basicAllocator`
    /// used to supply memory.  If `basicAllocator` is 0, the currently
    /// installed default allocator is used.
    Employee(const Employee& original, bslma::Allocator *basicAllocator = 0);

    /// Destroy this object.
    ~Employee();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object.
    Employee& operator=(const Employee& rhs);

    /// Reset this object to the default value (i.e., its value upon default
    /// construction).
    void reset();

    /// Invoke the specified `manipulator` sequentially on the address of
    /// each (modifiable) attribute of this object, supplying `manipulator`
    /// with the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the last
    /// invocation of `manipulator` (i.e., the invocation that terminated
    /// the sequence).
    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);

    /// Invoke the specified `manipulator` on the address of the
    /// (modifiable) attribute indicated by the specified `id`, supplying
    /// `manipulator` with the corresponding attribute information
    /// structure.  Return the value returned from the invocation of
    /// `manipulator` if `id` identifies an attribute of this class, and -1
    /// otherwise.
    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);

    /// Invoke the specified `manipulator` on the address of the
    /// (modifiable) attribute indicated by the specified `name` of the
    /// specified `nameLength`, supplying `manipulator` with the
    /// corresponding attribute information structure.  Return the value
    /// returned from the invocation of `manipulator` if `name` identifies
    /// an attribute of this class, and -1 otherwise.
    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);

    /// Return a reference to the modifiable "Name" attribute of this
    /// object.
    bsl::string& name();

    /// Return a reference to the modifiable "Age" attribute of this object.
    int& age();

    /// Return a reference to the modifiable "Ids" attribute of this object.
    bsl::vector<int>& ids();

    /// Return a reference to the modifiable "Fullname" attribute of this
    /// object.
    FullName& fullname();

    /// Return a reference to the modifiable "CarColor" attribute of this
    /// object.
    Color::Value& carColor();

    /// Return a reference to the modifiable "Friends" attribute of this
    /// object.
    bsl::vector<FullName>& friends();

    // ACCESSORS

    /// Invoke the specified `accessor` sequentially on each
    /// (non-modifiable) attribute of this object, supplying `accessor` with
    /// the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the last
    /// invocation of `accessor` (i.e., the invocation that terminated the
    /// sequence).
    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute of
    /// this object indicated by the specified `id`, supplying `accessor`
    /// with the corresponding attribute information structure.  Return the
    /// value returned from the invocation of `accessor` if `id` identifies
    /// an attribute of this class, and -1 otherwise.
    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute of
    /// this object indicated by the specified `name` of the specified
    /// `nameLength`, supplying `accessor` with the corresponding attribute
    /// information structure.  Return the value returned from the
    /// invocation of `accessor` if `name` identifies an attribute of this
    /// class, and -1 otherwise.
    template<class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;

    /// Return a reference to the non-modifiable "Name" attribute of this
    /// object.
    const bsl::string& name() const;

    /// Return a reference to the non-modifiable "Age" attribute of this
    /// object.
    int age() const;

    /// Return a reference to the non-modifiable "Ids" attribute of this
    /// object.
    const bsl::vector<int>& ids() const;

    /// Return a reference to the non-modifiable "Fullname" attribute of
    /// this object.
    const FullName& fullname() const;

    /// Return a reference to the non-modifiable "CarColor" attribute of
    /// this object.
    Color::Value carColor() const;

    /// Return a reference to the non-modifiable "Friends" attribute of this
    /// object.
    const bsl::vector<FullName>& friends() const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` attribute objects have
/// the same value, and `false` otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
inline
bool operator==(const Employee& lhs, const Employee& rhs);

/// Return `true` if the specified `lhs` and `rhs` attribute objects do not
/// have the same value, and `false` otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
inline
bool operator!=(const Employee& lhs, const Employee& rhs);

}  // close namespace case4

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(case4::Employee)

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

namespace case4 {

                                // -----------
                                // class Color
                                // -----------

// CLASS METHODS
inline
int Color::fromString(Value *result, const bsl::string& string)
{
    return fromString(result,
                      string.c_str(),
                      static_cast<int>(string.length()));
}

                               // --------------
                               // class FullName
                               // --------------

// MANIPULATORS
template <class MANIPULATOR>
int FullName::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_name, ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_NAME]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_ids, ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_IDS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
int FullName::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { e_NOT_FOUND = -1 };

    switch (id) {
      case k_ATTRIBUTE_ID_NAME: {
        return manipulator(&d_name,
                           ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_NAME]);
                                                                      // RETURN
      } break;
      case k_ATTRIBUTE_ID_IDS: {
        return manipulator(&d_ids,
                           ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_IDS]);
                                                                      // RETURN
      } break;
      default:
        return e_NOT_FOUND;                                           // RETURN
    }
}

template <class MANIPULATOR>
int FullName::manipulateAttribute(MANIPULATOR&  manipulator,
                                  const char   *name,
                                  int           nameLength)
{
    enum { e_NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return e_NOT_FOUND;                                           // RETURN
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
bsl::string& FullName::name()
{
    return d_name;
}

inline
bsl::vector<int>& FullName::ids()
{
    return d_ids;
}

// ACCESSORS
template <class ACCESSOR>
int FullName::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_name, ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_NAME]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_ids, ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_IDS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
int FullName::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { e_NOT_FOUND = -1 };

    switch (id) {
      case k_ATTRIBUTE_ID_NAME: {
        return accessor(d_name, ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_NAME]);
                                                                      // RETURN
      } break;
      case k_ATTRIBUTE_ID_IDS: {
        return accessor(d_ids, ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_IDS]);
                                                                      // RETURN
      } break;
      default:
        return e_NOT_FOUND;                                           // RETURN
    }
}

template <class ACCESSOR>
int FullName::accessAttribute(ACCESSOR&   accessor,
                              const char *name,
                              int         nameLength) const
{
    enum { e_NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return e_NOT_FOUND;                                            // RETURN
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

inline
const bsl::string& FullName::name() const
{
    return d_name;
}

inline
const bsl::vector<int>& FullName::ids() const
{
    return d_ids;
}

                               // --------------
                               // class Employee
                               // --------------

// MANIPULATORS
template <class MANIPULATOR>
int Employee::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_name,
                      ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_NAME]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_age,
                      ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_AGE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_ids,
                      ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_IDS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_fullname,
                      ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_FULLNAME]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_carColor,
                      ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_CAR_COLOR]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_friends,
                      ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_FRIENDS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
int Employee::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { e_NOT_FOUND = -1 };

    switch (id) {
      case k_ATTRIBUTE_ID_NAME: {
        return manipulator(&d_name,
                           ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_NAME]);
                                                                      // RETURN
      } break;
      case k_ATTRIBUTE_ID_AGE: {
        return manipulator(&d_age,
                           ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_AGE]);
                                                                      // RETURN
      } break;
      case k_ATTRIBUTE_ID_IDS: {
        return manipulator(&d_ids,
                           ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_IDS]);
                                                                      // RETURN
      } break;
      case k_ATTRIBUTE_ID_FULLNAME: {
        return manipulator(&d_fullname,
                           ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_FULLNAME]);
                                                                      // RETURN
      } break;
      case k_ATTRIBUTE_ID_CAR_COLOR: {
        return manipulator(&d_carColor,
                           ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_CAR_COLOR]);
                                                                      // RETURN
      } break;
      case k_ATTRIBUTE_ID_FRIENDS: {
        return manipulator(&d_friends,
                           ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_FRIENDS]);
                                                                      // RETURN
      } break;
      default:
        return e_NOT_FOUND;                                           // RETURN
    }
}

template <class MANIPULATOR>
int Employee::manipulateAttribute(MANIPULATOR&  manipulator,
                                  const char   *name,
                                  int           nameLength)
{
    enum { e_NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return e_NOT_FOUND;                                           // RETURN
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
bsl::string& Employee::name()
{
    return d_name;
}

inline
int& Employee::age()
{
    return d_age;
}

inline
bsl::vector<int>& Employee::ids()
{
    return d_ids;
}

inline
FullName& Employee::fullname()
{
    return d_fullname;
}

inline
Color::Value& Employee::carColor()
{
    return d_carColor;
}

inline
bsl::vector<FullName>& Employee::friends()
{
    return d_friends;
}

// ACCESSORS
template <class ACCESSOR>
int Employee::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_name, ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_NAME]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_age, ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_AGE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_ids, ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_IDS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_fullname,
                   ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_FULLNAME]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_carColor,
                   ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_CAR_COLOR]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_friends, ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_FRIENDS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
int Employee::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { e_NOT_FOUND = -1 };

    switch (id) {
      case k_ATTRIBUTE_ID_NAME: {
        return accessor(d_name, ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_NAME]);
                                                                      // RETURN
      } break;
      case k_ATTRIBUTE_ID_AGE: {
        return accessor(d_age,
                        ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_AGE]);
                                                                      // RETURN
      } break;
      case k_ATTRIBUTE_ID_IDS: {
        return accessor(d_ids,
                        ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_IDS]);
                                                                      // RETURN
      } break;
      case k_ATTRIBUTE_ID_FULLNAME: {
        return accessor(d_fullname,
                        ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_FULLNAME]);
                                                                      // RETURN
      } break;
      case k_ATTRIBUTE_ID_CAR_COLOR: {
        return accessor(d_carColor,
                        ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_CAR_COLOR]);
                                                                      // RETURN
      } break;
      case k_ATTRIBUTE_ID_FRIENDS: {
        return accessor(d_friends,
                        ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_FRIENDS]);
                                                                      // RETURN
      } break;
      default:
        return e_NOT_FOUND;                                           // RETURN
    }
}

template <class ACCESSOR>
int Employee::accessAttribute(ACCESSOR&   accessor,
                              const char *name,
                              int         nameLength) const
{
    enum { e_NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return e_NOT_FOUND;                                            // RETURN
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

inline
const bsl::string& Employee::name() const
{
    return d_name;
}

inline
int Employee::age() const
{
    return d_age;
}

inline
const bsl::vector<int>& Employee::ids() const
{
    return d_ids;
}

inline
const FullName& Employee::fullname() const
{
    return d_fullname;
}

inline
Color::Value Employee::carColor() const
{
    return d_carColor;
}

inline
const bsl::vector<FullName>& Employee::friends() const
{
    return d_friends;
}

}  // close namespace case4

// FREE FUNCTIONS

inline
bool case4::operator==(const case4::FullName& lhs, const case4::FullName& rhs)
{
    return  lhs.name() == rhs.name()
         && lhs.ids() == rhs.ids();
}

inline
bool case4::operator!=(const case4::FullName& lhs, const case4::FullName& rhs)
{
    return  lhs.name() != rhs.name()
         || lhs.ids() != rhs.ids();
}

inline
bool case4::operator==(const case4::Employee& lhs, const case4::Employee& rhs)
{
    return  lhs.name() == rhs.name()
         && lhs.age() == rhs.age()
         && lhs.ids() == rhs.ids()
         && lhs.fullname() == rhs.fullname()
         && lhs.carColor() == rhs.carColor()
         && lhs.friends() == rhs.friends();
}

inline
bool case4::operator!=(const case4::Employee& lhs, const case4::Employee& rhs)
{
    return  lhs.name() != rhs.name()
         || lhs.age() != rhs.age()
         || lhs.ids() != rhs.ids()
         || lhs.fullname() != rhs.fullname()
         || lhs.carColor() != rhs.carColor()
         || lhs.friends() != rhs.friends();
}

namespace case4 {

                                // -----------
                                // class Color
                                // -----------

// CONSTANTS

const char Color::CLASS_NAME[] = "Color";

const bdlat_EnumeratorInfo Color::ENUMERATOR_INFO_ARRAY[] = {
    {
        Color::e_RED,
        "RED",
        sizeof("RED") - 1,
        ""
    },
    {
        Color::e_GREEN,
        "GREEN",
        sizeof("GREEN") - 1,
        ""
    },
    {
        Color::e_BLUE,
        "BLUE",
        sizeof("BLUE") - 1,
        ""
    }
};

// CLASS METHODS

int Color::fromInt(Color::Value *result, int number)
{
    switch (number) {
      case Color::e_RED:
      case Color::e_GREEN:
      case Color::e_BLUE:
        *result = (Color::Value)number;
        return 0;                                                     // RETURN
      default:
        return -1;                                                    // RETURN
    }
}

int Color::fromString(Color::Value *result,
                      const char   *string,
                      int           stringLength)
{
    for (int i = 0; i < 3; ++i) {
        const bdlat_EnumeratorInfo& enumeratorInfo =
                    Color::ENUMERATOR_INFO_ARRAY[i];

        if (stringLength == enumeratorInfo.d_nameLength
        &&  0 == bsl::memcmp(enumeratorInfo.d_name_p, string, stringLength))
        {
            *result = (Color::Value)enumeratorInfo.d_value;
            return 0;                                                 // RETURN
        }
    }

    return -1;
}

const char *Color::toString(Color::Value value)
{
    switch (value) {
      case e_RED: {
        return "RED";                                                 // RETURN
      } break;
      case e_GREEN: {
        return "GREEN";                                               // RETURN
      } break;
      case e_BLUE: {
        return "BLUE";                                                // RETURN
      } break;
    }

    BSLS_ASSERT(0 == "invalid enumerator");
    return 0;
}

                               // --------------
                               // class FullName
                               // --------------

// CONSTANTS

const char FullName::CLASS_NAME[] = "FullName";

const bdlat_AttributeInfo FullName::ATTRIBUTE_INFO_ARRAY[] = {
    {
        k_ATTRIBUTE_ID_NAME,
        "name",
        sizeof("name") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        k_ATTRIBUTE_ID_IDS,
        "ids",
        sizeof("ids") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *FullName::lookupAttributeInfo(
                                                        const char *name,
                                                        int         nameLength)
{
    for (int i = 0; i < 2; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    FullName::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;                                    // RETURN
        }
    }

    return 0;
}

const bdlat_AttributeInfo *FullName::lookupAttributeInfo(int id)
{
    switch (id) {
      case k_ATTRIBUTE_ID_NAME:
        return &ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_NAME];
      case k_ATTRIBUTE_ID_IDS:
        return &ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_IDS];
      default:
        return 0;
    }
}

// CREATORS

FullName::FullName(bslma::Allocator *basicAllocator)
: d_ids(basicAllocator)
, d_name(basicAllocator)
{
}

FullName::FullName(const FullName&   original,
                   bslma::Allocator *basicAllocator)
: d_ids(original.d_ids, basicAllocator)
, d_name(original.d_name, basicAllocator)
{
}

FullName::~FullName()
{
}

// MANIPULATORS

FullName&
FullName::operator=(const FullName& rhs)
{
    if (this != &rhs) {
        d_name = rhs.d_name;
        d_ids = rhs.d_ids;
    }

    return *this;
}

void FullName::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_name);
    bdlat_ValueTypeFunctions::reset(&d_ids);
}

                               // --------------
                               // class Employee
                               // --------------

// CONSTANTS

const char Employee::CLASS_NAME[] = "Employee";

const bdlat_AttributeInfo Employee::ATTRIBUTE_INFO_ARRAY[] = {
    {
        k_ATTRIBUTE_ID_NAME,
        "name",
        sizeof("name") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        k_ATTRIBUTE_ID_AGE,
        "age",
        sizeof("age") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        k_ATTRIBUTE_ID_IDS,
        "ids",
        sizeof("ids") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        k_ATTRIBUTE_ID_FULLNAME,
        "fullname",
        sizeof("fullname") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        k_ATTRIBUTE_ID_CAR_COLOR,
        "car_color",
        sizeof("car_color") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        k_ATTRIBUTE_ID_FRIENDS,
        "friends",
        sizeof("friends") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *Employee::lookupAttributeInfo(
                                                        const char *name,
                                                        int         nameLength)
{
    for (int i = 0; i < 6; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    Employee::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;                                    // RETURN
        }
    }

    return 0;
}

const bdlat_AttributeInfo *Employee::lookupAttributeInfo(int id)
{
    switch (id) {
      case k_ATTRIBUTE_ID_NAME:
        return &ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_NAME];
      case k_ATTRIBUTE_ID_AGE:
        return &ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_AGE];
      case k_ATTRIBUTE_ID_IDS:
        return &ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_IDS];
      case k_ATTRIBUTE_ID_FULLNAME:
        return &ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_FULLNAME];
      case k_ATTRIBUTE_ID_CAR_COLOR:
        return &ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_CAR_COLOR];
      case k_ATTRIBUTE_ID_FRIENDS:
        return &ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_FRIENDS];
      default:
        return 0;
    }
}

// CREATORS

Employee::Employee(bslma::Allocator *basicAllocator)
: d_ids(basicAllocator)
, d_friends(basicAllocator)
, d_name(basicAllocator)
, d_fullname(basicAllocator)
, d_age()
, d_carColor(static_cast<Color::Value>(0))
{
}

Employee::Employee(const Employee&   original,
                   bslma::Allocator *basicAllocator)
: d_ids(original.d_ids, basicAllocator)
, d_friends(original.d_friends, basicAllocator)
, d_name(original.d_name, basicAllocator)
, d_fullname(original.d_fullname, basicAllocator)
, d_age(original.d_age)
, d_carColor(original.d_carColor)
{
}

Employee::~Employee()
{
}

// MANIPULATORS

Employee&
Employee::operator=(const Employee& rhs)
{
    if (this != &rhs) {
        d_name = rhs.d_name;
        d_age = rhs.d_age;
        d_ids = rhs.d_ids;
        d_fullname = rhs.d_fullname;
        d_carColor = rhs.d_carColor;
        d_friends = rhs.d_friends;
    }

    return *this;
}

void Employee::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_name);
    bdlat_ValueTypeFunctions::reset(&d_age);
    bdlat_ValueTypeFunctions::reset(&d_ids);
    bdlat_ValueTypeFunctions::reset(&d_fullname);
    bdlat_ValueTypeFunctions::reset(&d_carColor);
    bdlat_ValueTypeFunctions::reset(&d_friends);
}

}  // close namespace case4

}  // close enterprise namespace

namespace BloombergLP {
namespace s_baltst {

                       // =============================
                       // class HexBinaryCustomizedType
                       // =============================

class HexBinaryCustomizedType {

    // INSTANCE DATA
    bsl::vector<char> d_value;

    // PRIVATE CLASS METHODS

    /// Check if the specified `value` having the specified `size` satisfies
    /// the restrictions of this class.  Return 0 if successful (i.e., the
    /// restrictions are satisfied) and non-zero otherwise.
    static int checkRestrictions(const char *value, int size);

    /// Check if the specified `value` satisfies the restrictions of this
    /// class.  Return 0 if successful (i.e., the restrictions are
    /// satisfied) and non-zero otherwise.
    static int checkRestrictions(const bsl::vector<char>& value);

  public:
    // TYPES
    typedef bsl::vector<char> BaseType;

    // CONSTANTS
    static const char CLASS_NAME[];

    // CREATORS

    /// Create an object of type `HexBinaryCustomizedType` having the
    /// default value.  Optionally specify a `basicAllocator` used to
    /// supply memory.  If `basicAllocator` is 0, the currently installed
    /// default allocator is used.
    explicit HexBinaryCustomizedType(bslma::Allocator *basicAllocator = 0);

    /// Create an object of type `HexBinaryCustomizedType` having the value
    /// of the specified `original` object.  Optionally specify a
    /// `basicAllocator` used to supply memory.  If `basicAllocator` is 0,
    /// the currently installed default allocator is used.
    HexBinaryCustomizedType(
                           const HexBinaryCustomizedType&  original,
                           bslma::Allocator               *basicAllocator = 0);

    /// Destroy this object.
    ~HexBinaryCustomizedType();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object.
    HexBinaryCustomizedType& operator=(const HexBinaryCustomizedType& rhs);

    /// Convert from the specified `value` to this type.  Return 0 if
    /// successful and non-zero otherwise.
    int fromVector(const bsl::vector<char>& value);

    /// Reset this object to the default value (i.e., its value upon default
    /// construction).
    void reset();

    /// Return the array encapsulated by this object.
    bsl::vector<char>& array();

    // ACCESSORS

    /// Format this object to the specified output `stream` at the
    /// optionally specified indentation `level` and return a reference to
    /// the modifiable `stream`.  If `level` is specified, optionally
    /// specify `spacesPerLevel`, the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of `level * spacesPerLevel`.  If `level` is
    /// negative, suppress indentation of the first line.  If
    /// `spacesPerLevel` is negative, suppress line breaks and format the
    /// entire output on one line.  If `stream` is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;

    /// Return the array encapsulated by this object.
    const bsl::vector<char>& array() const;

    /// Return the array encapsulated by this object.
    const bsl::vector<char>& toVector() const { return array(); };
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` attribute objects have
/// the same value, and `false` otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
inline
bool operator==(const HexBinaryCustomizedType& lhs,
                const HexBinaryCustomizedType& rhs);

/// Return `true` if the specified `lhs` and `rhs` attribute objects do not
/// have the same value, and `false` otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
inline
bool operator!=(const HexBinaryCustomizedType& lhs,
                const HexBinaryCustomizedType& rhs);

/// Format the specified `rhs` to the specified output `stream` and return a
/// reference to the modifiable `stream`.
inline
bsl::ostream& operator<<(bsl::ostream&                  stream,
                         const HexBinaryCustomizedType& rhs);

}  // close namespace s_baltst

// TRAITS

BDLAT_DECL_CUSTOMIZEDTYPE_TRAITS(test::HexBinaryCustomizedType)

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

namespace s_baltst {

                       // -----------------------------
                       // class HexBinaryCustomizedType
                       // -----------------------------

// PRIVATE CLASS METHODS
int HexBinaryCustomizedType::checkRestrictions(const char *, int)
{
    return 0;
}

int HexBinaryCustomizedType::checkRestrictions(const bsl::vector<char>&)
{
    return 0;
}

// CREATORS
inline
HexBinaryCustomizedType::HexBinaryCustomizedType(
    bslma::Allocator *basicAllocator)
: d_value(basicAllocator)
{
}

inline
HexBinaryCustomizedType::HexBinaryCustomizedType(
    const HexBinaryCustomizedType& original, bslma::Allocator *basicAllocator)
: d_value(original.d_value, basicAllocator)
{
}

inline
HexBinaryCustomizedType::~HexBinaryCustomizedType()
{
}

// MANIPULATORS
inline
HexBinaryCustomizedType& HexBinaryCustomizedType::operator=(
                                            const HexBinaryCustomizedType& rhs)
{
    d_value = rhs.d_value;
    return *this;
}

inline
int HexBinaryCustomizedType::fromVector(const bsl::vector<char>& value)
{
    int ret = checkRestrictions(value);
    if (0 == ret) {
        d_value = value;
    }

    return ret;
}

inline
void HexBinaryCustomizedType::reset()
{
    d_value.clear();
}

inline
bsl::vector<char>& HexBinaryCustomizedType::array()
{
    return d_value;
}

// ACCESSORS
bsl::ostream& HexBinaryCustomizedType::print(
                                            bsl::ostream& stream,
                                            int,
                                            int) const
{
    if (d_value.empty()) {
        stream << "";
    }
    else {
        stream << 'x'
               << '\''
               << bdlb::PrintStringSingleLineHexDumper(
                                 &d_value[0], static_cast<int>(d_value.size()))
               << '\'';
    }
    return stream;
}

inline
const bsl::vector<char>& HexBinaryCustomizedType::array() const
{
    return d_value;
}

// FREE FUNCTIONS
inline
bool operator==(const HexBinaryCustomizedType& lhs,
                const HexBinaryCustomizedType& rhs)
{
    const bsl::vector<char>& lhsArray = lhs.array();
    const bsl::vector<char>& rhsArray = rhs.array();

    if (lhsArray.size() != rhsArray.size()) {
        return false;                                                 // RETURN
    }

    for (size_t i = 0; i < lhsArray.size(); ++i) {
        if (lhsArray[i] != rhsArray[i]) {
            return false;                                             // RETURN
        }
    }

    return true;
}

inline
bool operator!=(const HexBinaryCustomizedType& lhs,
                const HexBinaryCustomizedType& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& operator<<(bsl::ostream&                  stream,
                         const HexBinaryCustomizedType& rhs)
{
    return rhs.print(stream, 0, -1);
}

// CONSTANTS

const char HexBinaryCustomizedType::CLASS_NAME[] = "HexBinaryCT";

                          // =======================
                          // class HexBinarySequence
                          // =======================

class HexBinarySequence {

    // INSTANCE DATA
    HexBinaryCustomizedType d_element1;

  public:
    // TYPES
    enum {
        k_ATTRIBUTE_ID_ELEMENT1 = 0
    };

    enum {
        k_NUM_ATTRIBUTES = 1
    };

    enum {
        k_ATTRIBUTE_INDEX_ELEMENT1 = 0
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

  public:
    // CLASS METHODS

    /// Return attribute information for the attribute indicated by the
    /// specified `id` if the attribute exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(int id);

    /// Return attribute information for the attribute indicated by the
    /// specified `name` of the specified `nameLength` if the attribute
    /// exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(
                                                       const char *name,
                                                       int         nameLength);

    // CREATORS

    /// Create an object of type `HexBinarySequence` having the default
    /// value.
    explicit HexBinarySequence(bslma::Allocator *basicAllocator = 0);

    /// Create an object of type `HexBinarySequence` having the value of the
    /// specified `original` object.
    HexBinarySequence(const HexBinarySequence&  original,
                      bslma::Allocator         *basicAllocator = 0);

    /// Destroy this object.
    ~HexBinarySequence();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object.
    HexBinarySequence& operator=(const HexBinarySequence& rhs);

    /// Reset this object to the default value (i.e., its value upon default
    /// construction).
    void reset();

    /// Invoke the specified `manipulator` sequentially on the address of
    /// each (modifiable) attribute of this object, supplying `manipulator`
    /// with the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the last
    /// invocation of `manipulator` (i.e., the invocation that terminated
    /// the sequence).
    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);

    /// Invoke the specified `manipulator` on the address of the
    /// (modifiable) attribute indicated by the specified `id`, supplying
    /// `manipulator` with the corresponding attribute information
    /// structure.  Return the value returned from the invocation of
    /// `manipulator` if `id` identifies an attribute of this class, and -1
    /// otherwise.
    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);

    /// Invoke the specified `manipulator` on the address of the
    /// (modifiable) attribute indicated by the specified `name` of the
    /// specified `nameLength`, supplying `manipulator` with the
    /// corresponding attribute information structure.  Return the value
    /// returned from the invocation of `manipulator` if `name` identifies
    /// an attribute of this class, and -1 otherwise.
    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);

    /// Return a reference to the modifiable "Element1" attribute of this
    /// object.
    HexBinaryCustomizedType& element1();

    // ACCESSORS

    /// Format this object to the specified output `stream` at the
    /// optionally specified indentation `level` and return a reference to
    /// the modifiable `stream`.  If `level` is specified, optionally
    /// specify `spacesPerLevel`, the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of `level * spacesPerLevel`.  If `level` is
    /// negative, suppress indentation of the first line.  If
    /// `spacesPerLevel` is negative, suppress line breaks and format the
    /// entire output on one line.  If `stream` is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;

    /// Invoke the specified `accessor` sequentially on each
    /// (non-modifiable) attribute of this object, supplying `accessor` with
    /// the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the last
    /// invocation of `accessor` (i.e., the invocation that terminated the
    /// sequence).
    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute of
    /// this object indicated by the specified `id`, supplying `accessor`
    /// with the corresponding attribute information structure.  Return the
    /// value returned from the invocation of `accessor` if `id` identifies
    /// an attribute of this class, and -1 otherwise.
    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute of
    /// this object indicated by the specified `name` of the specified
    /// `nameLength`, supplying `accessor` with the corresponding attribute
    /// information structure.  Return the value returned from the
    /// invocation of `accessor` if `name` identifies an attribute of this
    /// class, and -1 otherwise.
    template<class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;

    /// Return a reference to the modifiable "Element1" attribute of this
    /// object.
    const HexBinaryCustomizedType& element1() const;

};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` attribute objects have
/// the same value, and `false` otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
inline
bool operator==(const HexBinarySequence& lhs, const HexBinarySequence& rhs);

/// Return `true` if the specified `lhs` and `rhs` attribute objects do not
/// have the same value, and `false` otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
inline
bool operator!=(const HexBinarySequence& lhs, const HexBinarySequence& rhs);

/// Format the specified `rhs` to the specified output `stream` and return a
/// reference to the modifiable `stream`.
inline
bsl::ostream& operator<<(bsl::ostream& stream, const HexBinarySequence& rhs);

}  // close namespace s_baltst

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(test::HexBinarySequence)

namespace s_baltst {

                          // -----------------------
                          // class HexBinarySequence
                          // -----------------------

// MANIPULATORS
template <class MANIPULATOR>
int HexBinarySequence::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_element1,
                      ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
int HexBinarySequence::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { e_NOT_FOUND = -1 };

    switch (id) {
      case k_ATTRIBUTE_ID_ELEMENT1: {
        return manipulator(&d_element1,
                           ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_ELEMENT1]);
                                                                      // RETURN
      } break;
      default:
        return e_NOT_FOUND;                                           // RETURN
    }
}

template <class MANIPULATOR>
int HexBinarySequence::manipulateAttribute(MANIPULATOR&  manipulator,
                                           const char   *name,
                                           int           nameLength)
{
    enum { e_NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return e_NOT_FOUND;                                           // RETURN
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
HexBinaryCustomizedType& HexBinarySequence::element1()
{
    return d_element1;
}

// ACCESSORS
template <class ACCESSOR>
int HexBinarySequence::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_element1,
                   ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
int HexBinarySequence::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { e_NOT_FOUND = -1 };

    switch (id) {
      case k_ATTRIBUTE_ID_ELEMENT1: {
        return accessor(d_element1,
                        ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_ELEMENT1]);
                                                                      // RETURN
      } break;
      default:
        return e_NOT_FOUND;                                           // RETURN
    }
}

template <class ACCESSOR>
int HexBinarySequence::accessAttribute(ACCESSOR&   accessor,
                                       const char *name,
                                       int         nameLength) const
{
    enum { e_NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return e_NOT_FOUND;                                            // RETURN
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

inline
const HexBinaryCustomizedType& HexBinarySequence::element1() const
{
    return d_element1;
}

// FREE FUNCTIONS

inline
bool operator==(const HexBinarySequence& lhs, const HexBinarySequence& rhs)
{
    return lhs.element1() == rhs.element1();
}

inline
bool operator!=(const HexBinarySequence& lhs, const HexBinarySequence& rhs)
{
    return !(lhs == rhs);
}

                          // -----------------------
                          // class HexBinarySequence
                          // -----------------------

// CONSTANTS

const char HexBinarySequence::CLASS_NAME[] = "HexBinarySequence";

const bdlat_AttributeInfo HexBinarySequence::ATTRIBUTE_INFO_ARRAY[] = {
    {
        k_ATTRIBUTE_ID_ELEMENT1,
        "element1",
        sizeof("element1") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *HexBinarySequence::lookupAttributeInfo(
                                                        const char *name,
                                                        int         nameLength)
{
    if (nameLength < 8) {
        return 0;                                                     // RETURN
    }
    if (name[0]=='e'
     && name[1]=='l'
     && name[2]=='e'
     && name[3]=='m'
     && name[4]=='e'
     && name[5]=='n'
     && name[6]=='t'
     && name[7]=='1') {
        return &ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_ELEMENT1];     // RETURN
    }
    return 0;
}

const bdlat_AttributeInfo *HexBinarySequence::lookupAttributeInfo(int id)
{
    switch (id) {
      case k_ATTRIBUTE_ID_ELEMENT1:
        return &ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_ELEMENT1];
      default:
        return 0;
    }
}

// CREATORS

HexBinarySequence::HexBinarySequence(bslma::Allocator *basicAllocator)
: d_element1(basicAllocator)
{
}

HexBinarySequence::HexBinarySequence(const HexBinarySequence&  original,
                                     bslma::Allocator         *basicAllocator)
: d_element1(original.d_element1, basicAllocator)
{
}

HexBinarySequence::~HexBinarySequence()
{
}

// MANIPULATORS

HexBinarySequence&
HexBinarySequence::operator=(const HexBinarySequence& rhs)
{
    if (this != &rhs) {
        d_element1 = rhs.d_element1;
    }
    return *this;
}

void HexBinarySequence::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_element1);
}

// ACCESSORS

bsl::ostream& HexBinarySequence::print(bsl::ostream& stream,
                                       int           level,
                                       int           spacesPerLevel) const
{
    if (level < 0) {
        level = -level;
    }
    else {
        bdlb::Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;
    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element1 = ";
        bdlb::PrintMethods::print(stream, d_element1,
                                 -levelPlus1, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Element1 = ";
        bdlb::PrintMethods::print(stream, d_element1,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

bsl::ostream& operator<<(bsl::ostream&            stream,
                         const HexBinarySequence& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace s_baltst
}  // close enterprise namespace

// ============================================================================
//                              BDLAT TEST TYPES
// ----------------------------------------------------------------------------

namespace BloombergLP {
namespace s_baltst {

                             // ==================
                             // class Enumeration0
                             // ==================

class Enumeration0 {

  public:
    Enumeration0()
    {
    }
};

int bdlat_enumFromInt(Enumeration0 *, int number)
{
    if (0 == number) {
        return 0;                                                     // RETURN
    }

    return -1;
}

int bdlat_enumFromString(Enumeration0 *, const char *string,
                         int stringLength)
{
    const bsl::string_view stringRef(string, stringLength);

    if ("zero" == stringRef) {
        return 0;                                                     // RETURN
    }

    return -1;
}

void bdlat_enumToInt(int *result, const Enumeration0&)
{
    *result = 0;
}

void bdlat_enumToString(bsl::string *result, const Enumeration0&)
{
    *result = "zero";
}

}  // close namespace s_baltst

// TRAITS
template <>
struct bdlat_IsBasicEnumeration<test::Enumeration0> : bsl::true_type {};

namespace bdlat_EnumFunctions {

template <>
struct IsEnumeration<test::Enumeration0> : public bsl::true_type {
};

}  // close bdlat_EnumFunctions namespace
namespace s_baltst {

                             // ==================
                             // class Enumeration1
                             // ==================

class Enumeration1 {
  private:
    int d_value;

    friend int bdlat_enumFromInt(Enumeration1 *dest, int val)
    {
        if (val == 0 || val == 1) {
            dest->d_value = val;
            return 0;                                                 // RETURN
        }
        return -1;
    }

    friend int bdlat_enumFromString(Enumeration1 *, const char *, int)
    {
        bsl::cout << "should not be called\n";
        ASSERT(false);
        return -1;
    }

    friend void bdlat_enumToInt(int *result, const Enumeration1& src)
    {
        *result = src.d_value;
    }

    friend void bdlat_enumToString(bsl::string         *result,
                                   const Enumeration1&  src)
    {
        if (src.d_value == 0) {
            *result = "0";
        } else {
            ASSERT(1 == src.d_value);
            *result = "1";
        }
    }
};
}  // close namespace s_baltst
namespace bdlat_EnumFunctions {
template <>
struct IsEnumeration<test::Enumeration1> : public bsl::true_type {
};
}  // close namespace bdlat_EnumFunctions
}  // close enterprise namespace

namespace BloombergLP {
namespace baljsn {
namespace decoder {
namespace u {

                      // ===============================
                      // NAMES FOR BLDAT TYPE GENERATION
                      // ===============================

extern const char k_ATTRIBUTE_1_NAME[]  = "a1";
extern const char k_ATTRIBUTE_2_NAME[]  = "a2";
extern const char k_ATTRIBUTE_3_NAME[]  = "a3";
extern const char k_ENUMERATOR_1_NAME[] = "e1";
extern const char k_ENUMERATOR_2_NAME[] = "e2";
extern const char k_SELECTION_1_NAME[]  = "s1";
extern const char k_SELECTION_2_NAME[]  = "s2";

extern const char k_EMPTY_STRING[] = "";

}  // close namespace u
}  // close namespace encoder
}  // close package namespace
}  // close enterprise namespace

namespace u {

                              // ===============
                              // struct TestUtil
                              // ===============

struct TestUtil {
    // CLASS METHODS

    /// Assert that decoding the specified `JSON` using `baljsn::Decoder`
    /// with a default set of options yields the specified `VALUE` if
    /// `IS_VALID` is `true`, and fails otherwise.
    template <class VALUE_TYPE>
    static void assertDecodedValueIsEqual(int                     LINE,
                                          const bsl::string_view& JSON,
                                          bool                    IS_VALID,
                                          const VALUE_TYPE&       VALUE)
    {
        typedef baljsn::Decoder Obj;
        Obj mX;

        baljsn::DecoderOptions options;

        bdlsb::FixedMemInStreamBuf streamBuffer(JSON.data(), JSON.size());

        VALUE_TYPE result;
        int rc = mX.decode(&streamBuffer, &result, options);
        if (IS_VALID) {
            ASSERTV(LINE, rc, 0 == rc);
            ASSERTV(LINE, rc, result == VALUE);
        }
        else {
            ASSERTV(LINE, rc, 0 != rc);
        }
    }
};

                               // =============
                               // class Replace
                               // =============

class Replace {
  public:
    // CREATORS
    Replace() { }

    // ACCESSORS

    /// Return the result of replacing each occurrence of the specified
    /// `subject` with the specified `replacement` in the specified
    /// `original` string.
    bsl::string operator()(const bsl::string_view& original,
                           const bsl::string_view& subject,
                           const bsl::string_view& replacement) const
    {
        bdlpcre::RegEx regex;
        bsl::string    prepareErrorMessage;
        bsl::size_t    prepareErrorOffset = 0;
        bsl::string    subjectCopy(subject);
        int rc = regex.prepare(
            &prepareErrorMessage, &prepareErrorOffset, subjectCopy.c_str());
        if (0 != rc) {
            P(prepareErrorMessage);
        }
        BSLS_ASSERT(0 == rc);
        BSLS_ASSERT(0 == prepareErrorOffset);

        bsl::string result;
        int         errorOffset = 0;

        rc = regex.replace(&result,
                           &errorOffset,
                           original,
                           replacement,
                           bdlpcre::RegEx::k_REPLACE_LITERAL |
                               bdlpcre::RegEx::k_REPLACE_GLOBAL);
        if (0 > rc) {
            P(prepareErrorMessage);
        }
        BSLS_ASSERT(0 <= rc);
        BSLS_ASSERT(0 == errorOffset);
        return result;
    }
};

                  // =======================================
                  // class AssertDecodedValueIsEqualFunction
                  // =======================================

class AssertDecodedValueIsEqualFunction {
  public:
    // CREATORS
    AssertDecodedValueIsEqualFunction()
    {
    }

    // ACCESSORS

    /// Assert that decoding the specified `JSON` using `baljsn::Decoder`
    /// with a default set of options yields the specified `VALUE`.
    template <class VALUE_TYPE>
    void operator()(int                     LINE,
                    const bsl::string_view& JSON,
                    const VALUE_TYPE&       VALUE) const
    {
        TestUtil::assertDecodedValueIsEqual(LINE, JSON, true, VALUE);
    }

    /// Assert that decoding the specified `JSON` using `baljsn::Decoder`
    /// with a default set of options yields the specified `VALUE` if
    /// `IS_VALID` is `true`, and fails otherwise.
    template <class VALUE_TYPE>
    void operator()(int                     LINE,
                    const bsl::string_view& JSON,
                    bool                    IS_VALID,
                    const VALUE_TYPE&       VALUE) const
    {
        TestUtil::assertDecodedValueIsEqual(LINE, JSON, IS_VALID, VALUE);
    }
};

class AssertDecodedArrayOfValuesIsEqualFunction {

  public:
    // CREATORS
    AssertDecodedArrayOfValuesIsEqualFunction()
    {
    }

    // ACCESSORS

    /// Assert that decoding various nested and non-nested JSON arrays
    /// having the specified `ELEMENT_JSON` as terminal element(s) using
    /// `baljsn::Decoder` with a default set of options yields an
    /// equivalently-structured `bsl::vector` specialization having the
    /// zero or more copies of the specified terminal `EXPECTED_ELEMENT`.
    template <class ELEMENT_TYPE>
    void operator()(int                    LINE,
                    const bsl::string_view ELEMENT_JSON,
                    const ELEMENT_TYPE&    EXPECTED_ELEMENT) const
    {
        const AssertDecodedValueIsEqualFunction       TEST;
        const s_baltst::TestPlaceHolder<ELEMENT_TYPE> e;
        const s_baltst::GenerateTestArray             a;
        const Replace                                 fmt;

        const int               L = LINE;
        const ELEMENT_TYPE&     E = EXPECTED_ELEMENT;
        const bsl::string_view  S = ELEMENT_JSON;

        TEST(L, fmt("[]"                 , "S",S), a(e)                   );
        TEST(L, fmt("[S]"                , "S",S), a(E)                   );
        TEST(L, fmt("[S,S]"              , "S",S), a(E,E)                 );
        TEST(L, fmt("[S,S,S]"            , "S",S), a(E,E,E)               );
        TEST(L, fmt("[[]]"               , "S",S), a(a(e))                );
        TEST(L, fmt("[[S]]"              , "S",S), a(a(E))                );
        TEST(L, fmt("[[S,S]]"            , "S",S), a(a(E,E))              );
        TEST(L, fmt("[[S,S,S]]"          , "S",S), a(a(E,E,E))            );
        TEST(L, fmt("[[],[]]"            , "S",S), a(a(e),a(e))           );
        TEST(L, fmt("[[],[S]]"           , "S",S), a(a(e),a(E))           );
        TEST(L, fmt("[[],[S,S]]"         , "S",S), a(a(e),a(E,E))         );
        TEST(L, fmt("[[],[S,S,S]]"       , "S",S), a(a(e),a(E,E,E))       );
#ifndef U_SKIP_DUE_TO_COMPILER_RESOURCE_LIMITATIONS
        TEST(L, fmt("[[S],[]]"           , "S",S), a(a(E),a(e))           );
        TEST(L, fmt("[[S],[S]]"          , "S",S), a(a(E),a(E))           );
        TEST(L, fmt("[[S],[S,S]]"        , "S",S), a(a(E),a(E,E))         );
        TEST(L, fmt("[[S],[S,S,S]]"      , "S",S), a(a(E),a(E,E,E))       );
        TEST(L, fmt("[[S,S],[]]"         , "S",S), a(a(E,E),a(e))         );
        TEST(L, fmt("[[S,S],[S]]"        , "S",S), a(a(E,E),a(E))         );
        TEST(L, fmt("[[S,S],[S,S]]"      , "S",S), a(a(E,E),a(E,E))       );
        TEST(L, fmt("[[S,S],[S,S,S]]"    , "S",S), a(a(E,E),a(E,E,E))     );
        TEST(L, fmt("[[S,S,S],[]]"       , "S",S), a(a(E,E,E),a(e))       );
        TEST(L, fmt("[[S,S,S],[S]]"      , "S",S), a(a(E,E,E),a(E))       );
        TEST(L, fmt("[[S,S,S],[S,S]]"    , "S",S), a(a(E,E,E),a(E,E))     );
        TEST(L, fmt("[[S,S,S],[S,S,S]]"  , "S",S), a(a(E,E,E),a(E,E,E))   );
        TEST(L, fmt("[[],[],[]]"         , "S",S), a(a(e),a(e),a(e))      );
        TEST(L, fmt("[[],[],[S]]"        , "S",S), a(a(e),a(e),a(E))      );
        TEST(L, fmt("[[],[],[S,S]]"      , "S",S), a(a(e),a(e),a(E,E))    );
        TEST(L, fmt("[[],[S],[]]"        , "S",S), a(a(e),a(E),a(e))      );
        TEST(L, fmt("[[],[S],[S]]"       , "S",S), a(a(e),a(E),a(E))      );
        TEST(L, fmt("[[],[S],[S,S]]"     , "S",S), a(a(e),a(E),a(E,E))    );
        TEST(L, fmt("[[],[S,S],[]]"      , "S",S), a(a(e),a(E,E),a(e))    );
        TEST(L, fmt("[[],[S,S],[S]]"     , "S",S), a(a(e),a(E,E),a(E))    );
        TEST(L, fmt("[[],[S,S],[S,S]]"   , "S",S), a(a(e),a(E,E),a(E,E))  );
        TEST(L, fmt("[[S],[],[]]"        , "S",S), a(a(E),a(e),a(e))      );
        TEST(L, fmt("[[S],[],[S]]"       , "S",S), a(a(E),a(e),a(E))      );
        TEST(L, fmt("[[S],[],[S,S]]"     , "S",S), a(a(E),a(e),a(E,E))    );
        TEST(L, fmt("[[S],[S],[]]"       , "S",S), a(a(E),a(E),a(e))      );
        TEST(L, fmt("[[S],[S],[S]]"      , "S",S), a(a(E),a(E),a(E))      );
        TEST(L, fmt("[[S],[S],[S,S]]"    , "S",S), a(a(E),a(E),a(E,E))    );
        TEST(L, fmt("[[S],[S,S],[]]"     , "S",S), a(a(E),a(E,E),a(e))    );
        TEST(L, fmt("[[S],[S,S],[S]]"    , "S",S), a(a(E),a(E,E),a(E))    );
        TEST(L, fmt("[[S],[S,S],[S,S]]"  , "S",S), a(a(E),a(E,E),a(E,E))  );
        TEST(L, fmt("[[S,S],[],[]]"      , "S",S), a(a(E,E),a(e),a(e))    );
        TEST(L, fmt("[[S,S],[],[S]]"     , "S",S), a(a(E,E),a(e),a(E))    );
        TEST(L, fmt("[[S,S],[],[S,S]]"   , "S",S), a(a(E,E),a(e),a(E,E))  );
        TEST(L, fmt("[[S,S],[S],[]]"     , "S",S), a(a(E,E),a(E),a(e))    );
        TEST(L, fmt("[[S,S],[S],[S]]"    , "S",S), a(a(E,E),a(E),a(E))    );
        TEST(L, fmt("[[S,S],[S],[S,S]]"  , "S",S), a(a(E,E),a(E),a(E,E))  );
#endif
        TEST(L, fmt("[[S,S],[S,S],[]]"   , "S",S), a(a(E,E),a(E,E),a(e))  );
        TEST(L, fmt("[[S,S],[S,S],[S]]"  , "S",S), a(a(E,E),a(E,E),a(E))  );
        TEST(L, fmt("[[S,S],[S,S],[S,S]]", "S",S), a(a(E,E),a(E,E),a(E,E)));
    }
};

}  // close u namespace

// ============================================================================
//                               TEST MACHINERY
// ----------------------------------------------------------------------------

template <class TESTED_TYPE> struct SelectEncodeableType;

template <> struct SelectEncodeableType<float> {
    typedef s_baltst::SqrtF Type;
};

template <> struct SelectEncodeableType<double> {
    typedef s_baltst::Sqrt Type;
};

template <class FLOAT_TYPE>
void roundTripTestNonNumericValues()
{
    typedef          FLOAT_TYPE                             Type;
    typedef typename SelectEncodeableType<FLOAT_TYPE>::Type TestType;
    typedef          bsl::numeric_limits<FLOAT_TYPE>        Limits;

    const Type NAN_P = Limits::quiet_NaN();
    // Negative NaN does not print for any floating point type, so we
    // don't test it for round-trip (on purpose).
    //const Type NAN_N = -NAN_P;
    const Type INF_P = Limits::infinity();
    const Type INF_N = -INF_P;

    const struct {
        int         d_line;
        Type        d_value;
    } DATA[] = {
        //---------------
        // LINE | VALUE |
        //---------------
        { L_,    NAN_P },
        // Negative NaN does not print for any floating point type,
        // so we don't test it for round-trip (on purpose).
        //{ L_,    NAN_N },
        { L_,    INF_P },
        { L_,    INF_N },
    };
    const int NUM_DATA = sizeof DATA / sizeof DATA[0];

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int  LINE  = DATA[ti].d_line;
        const Type VALUE = DATA[ti].d_value;

        baljsn::EncoderOptions encoderOptions;
        encoderOptions.setEncodeInfAndNaNAsStrings(true);

        TestType toEncode;
        toEncode.value() = VALUE;

        // No options (NULL)
        bsl::stringstream ss;
        baljsn::Encoder   encoder;
        ASSERTV(LINE, encoder.loggedMessages(),
                0 == encoder.encode(ss, toEncode, encoderOptions));

        TestType  decoded;
        decoded.value() = Limits::quiet_NaN(); // A value we don't use
        baljsn::Decoder decoder;
        ASSERTV(LINE, decoder.loggedMessages(),
                0 == decoder.decode(ss, &decoded, 0));
        Type DECODED = decoded.value();
        bsl::string encoded(ss.str());

        if (VALUE != VALUE) { // A NaN
            ASSERTV(LINE, encoded, VALUE, DECODED, DECODED != DECODED);
        }
        else {
            ASSERTV(LINE, encoded, VALUE, DECODED, VALUE == DECODED);
        }
        // We also use `memcmp` to ensure that we really get back the
        // same binary IEEE-754.
        ASSERTV(LINE, encoded, VALUE, DECODED,
                0 == bsl::memcmp(&VALUE, &DECODED, sizeof VALUE));

    }
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int             test = argc > 1 ? atoi(argv[1]) : 0;
    bool         verbose = argc > 2;
    bool     veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 20: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, remove
        //    leading comment characters, and replace `assert` with `ASSERT`.
        //    (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nUSAGE EXAMPLE"
                          << "\n=============" << endl;

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Decoding into a `bas_codegen.pl`-generated from data in JSON
///-----------------------------------------------------------------------
// Consider that we want to exchange an employee's information between two
// processes.  To allow this information exchange we will define the XML schema
// representation for that class, use `bas_codegen.pl` to create the `Employee`
// `class` for storing that information, and decode into that object using the
// `baljsn` decoder.
//
// First, we will define the XML schema inside a file called `employee.xsd`:
// ```
//  <?xml version='1.0' encoding='UTF-8'?>
//  <xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'
//             xmlns:test='http://bloomberg.com/schemas/test'
//             targetNamespace='http://bloomberg.com/schemas/test'
//             elementFormDefault='unqualified'>
//
//      <xs:complexType name='Address'>
//          <xs:sequence>
//              <xs:element name='street' type='xs:string'/>
//              <xs:element name='city'   type='xs:string'/>
//              <xs:element name='state'  type='xs:string'/>
//          </xs:sequence>
//      </xs:complexType>
//
//      <xs:complexType name='Employee'>
//          <xs:sequence>
//              <xs:element name='name'        type='xs:string'/>
//              <xs:element name='homeAddress' type='test:Address'/>
//              <xs:element name='age'         type='xs:int'/>
//          </xs:sequence>
//      </xs:complexType>
//
//      <xs:element name='Employee' type='test:Employee'/>
//
//  </xs:schema>
// ```
// Then, we will use the `bas_codegen.pl` tool, to generate the C++ classes for
// this schema.  The following command will generate the header and
// implementation files for the all the classes in the `test_messages`
// components in the current directory:
// ```
//  $ bas_codegen.pl -m msg -p test xsdfile.xsd
// ```
// Next, we will create a `test::Employee` object:
// ```
    test::Employee employee;
// ```
// Then, we will create a `baljsn::Decoder` object:
// ```
    baljsn::Decoder decoder;
// ```
// Next, we will specify the input data provided to the decoder:
// ```
    const char INPUT[] = "{\"name\":\"Bob\",\"homeAddress\":{\"street\":"
                         "\"Lexington Ave\",\"city\":\"New York City\","
                         "\"state\":\"New York\"},\"age\":21}";

    bsl::istringstream is(INPUT);
// ```
// Now, we will decode this object using the `decode` function of the baljsn
// decoder by providing it a `baljsn::DecoderOptions` object.  The decoder
// options allow us to specify that unknown elements should *not* be skipped.
// Setting this option to `false` will result in the decoder returning an error
// on encountering an unknown element:
// ```
    baljsn::DecoderOptions options;
    options.setSkipUnknownElements(false);

    const int rc = decoder.decode(is, &employee, options);
    ASSERT(!rc);
    ASSERT(is);
// ```
// Finally, we will verify that the decoded object is as expected:
// ```
    ASSERT("Bob"           == employee.name());
    ASSERT("Lexington Ave" == employee.homeAddress().street());
    ASSERT("New York City" == employee.homeAddress().city());
    ASSERT("New York"      == employee.homeAddress().state());
    ASSERT(21              == employee.age());
// ```
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING `Decimal64`
        //   The decoder can load values from a JSON document into a field of a
        //   `bdlat`-compatible object having type `bdldfp::Decimal64`.
        //
        // Concerns:
        // 1. The loaded values can be any valid `bdldfp::Decimal64` value,
        //    including all limit values of that type.
        //
        // 2. The loaded values can be any acceptable representation of
        //    positive infinity, negative infinity, and not-a-number.
        //
        // 3. Valid input can be either quoted (i.e., a JSON string) or not
        //    (i.e., a JSON number).
        //
        // 4. The decoder returns zero for successful conversions and a
        //    non-zero value for invalid input.
        //
        // 5. A non-zero return value implies that an immediate call to the
        //    `loggedMessages()` method returns a non-empty string.
        //
        // Plan:
        // 1. This test case uses a series of table-driven tests where the
        //    tables are replicated from the `baljsn_parserutil.t.cpp` test
        //    cases of the `bdldfp::Decimal64` overload of
        //    `ParserUtil::getValue`, a function that converts text to a
        //    numeric value.
        //
        // 2. Each of the table input values are used to synthesize a JSON
        //    document representing
        //    `s_baltst::MySequenceWithPrecisionDecimalAttribute`.  The single
        //    attribute of that class holds a `bdldfp::Decimal64` value.
        //
        //    * Depending on the table, the input value is either used directly
        //      or converted to a textual representation using lower-level
        //      components.
        //
        //    * Note that two table entries were considered invalid in their
        //      original table but classified as valid below.  Those entries
        //      had trailing characters that cause failure if one tries to
        //      convert them Decimal64 but when used in a JSON document, they
        //      cause no error -- the normal decoder tokenization avoids them.
        //
        // 3. Each synthesized JSON document is passed to the `decode` method.
        //    The return value, the result of `loggedMessages`, and the
        //    value of `attribute1()` of the target `bdlat` class are tested
        //    for their expected values.
        //
        // Testing:
        //   TESTING `Decimal64`
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING `Decimal64`" <<
                             "\n===================" << endl;

        typedef s_baltst::MySequenceWithPrecisionDecimalAttribute SeqDec64;
        typedef baljsn::PrintUtil                                 Print;
        typedef baljsn::DecoderOptions                            DOptions;
        typedef baljsn::EncoderOptions                            EOptions;
        typedef bdldfp::Decimal64                                 Decimal64;
        typedef bdldfp::DecimalUtil                               DecUtil;

        if (verbose) cout << endl << "Regular values and limits" << endl;
        {
#define DEC(X) BDLDFP_DECIMAL_DD(X)

            typedef bsl::numeric_limits<Decimal64> Limits;

            const struct {
                int       d_line;
                Decimal64 d_value;
            } DATA[] = {
                //LINE  VALUE
                //----  -----------------------------
                {L_,    DEC( 0.0),                    },
                {L_,    DEC(-0.0),                    },
                {L_,    DEC( 1.13),                   },
                {L_,    DEC(-9.876543210987654e307)   },
                {L_,    DEC(-9.8765432109876548e307)  },
                {L_,    DEC(-9.87654321098765482e307) },

                // Boundary values
                { L_,    Limits::min()        },
                { L_,    Limits::denorm_min() },
                { L_,    Limits::max()        },
                { L_,   -Limits::min()        },
                { L_,   -Limits::denorm_min() },
                { L_,   -Limits::max()        },
            };
            const int NUM_DATA = sizeof DATA / sizeof DATA[0];

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int       LINE  = DATA[ti].d_line;
                const Decimal64 VALUE = DATA[ti].d_value;

                if (veryVerbose) {
                    P_(LINE); P(VALUE);
                }

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == Print::printValue(oss, VALUE));

                bsl::string result(oss.str());

                if (veryVeryVerbose) {
                    P(result);
                }

                bsl::string jsonDoc;
                jsonDoc.append("{");
                jsonDoc.append("\"attribute1\"");
                jsonDoc.append(":");
                jsonDoc.append(result);
                jsonDoc.append("}");

                if (veryVeryVerbose) {
                    P(jsonDoc);
                }

                bsl::istringstream iss(jsonDoc);

                Obj      decoder;
                SeqDec64 seqDec64;
                DOptions dOptions;

                int rc = decoder.decode(iss, &seqDec64, &dOptions);     // TEST
                ASSERTV(LINE,          rc,
                              0     == rc);
                ASSERTV(LINE,          decoder.loggedMessages(),
                              ""    == decoder.loggedMessages());
                ASSERTV(LINE, VALUE,   seqDec64.attribute1(),
                              VALUE == seqDec64.attribute1());

                if (veryVeryVerbose) {
                    P(seqDec64);
                }
            }
#undef DEC
        }

        if (verbose) cout << endl << "+Inf, -Inf, and NaN" << endl;
        {
            typedef  bsl::numeric_limits<Decimal64> Limits;

            const Decimal64 NAN_P = Limits::quiet_NaN();
            // Negative NaN does not print for any floating point type, so we
            // don't test it for round-trip (on purpose).
            //const Type NAN_N = -NAN_P;
            const Decimal64 INF_P = Limits::infinity();
            const Decimal64 INF_N = -INF_P;

            const struct {
                int       d_line;
                Decimal64 d_value;
            } DATA[] = {
                // LINE   VALUE |
                // ----   ------
                {  L_,    NAN_P },
                // Negative NaN does not print for any floating point type,
                // so we don't test it for round-trip (on purpose).
              //{  L_,    NAN_N },
                {  L_,    INF_P },
                {  L_,    INF_N },
            };
            const int NUM_DATA = sizeof DATA / sizeof DATA[0];

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int       LINE  = DATA[ti].d_line;
                const Decimal64 VALUE = DATA[ti].d_value;

                if (veryVerbose) {
                    P_(LINE); P(VALUE);
                }

                bsl::ostringstream oss;
                EOptions           eOptions;
                eOptions.setEncodeInfAndNaNAsStrings(true);
                int                rc = Print::printValue(oss,
                                                          VALUE,
                                                          &eOptions);
                ASSERTV(LINE, rc, 0 == rc);

                bsl::string result(oss.str());

                if (veryVeryVerbose) {
                    P(result);
                }

                bsl::string jsonDoc;
                jsonDoc.append("{ ");
                jsonDoc.append("\"attribute1\"");
                jsonDoc.append(":");
                jsonDoc.append(result);
                jsonDoc.append("}");

                if (veryVeryVerbose) {
                    P(jsonDoc);
                }

                bsl::istringstream iss(jsonDoc);

                Obj      decoder;
                SeqDec64 seqDec64;
                DOptions dOptions;

                rc = decoder.decode(iss, &seqDec64, &dOptions);         // TEST
                ASSERTV(LINE,          rc,
                              0     == rc);
                ASSERTV(LINE,          decoder.loggedMessages(),
                              ""    == decoder.loggedMessages());

                if (DecUtil::isNan(VALUE)) {
                    ASSERTV(LINE,                seqDec64.attribute1().value(),
                                  DecUtil::isNan(seqDec64.attribute1().value())
                           );
                } else {
                    ASSERTV(LINE, VALUE,   seqDec64.attribute1(),
                                  VALUE == seqDec64.attribute1());
                }

                if (veryVeryVerbose) {
                    P(seqDec64);
                }
            }
        }

        if (verbose) cout << endl << "Quoted input and invalid input" << endl;
        {
#define DEC(X) BDLDFP_DECIMAL_DD(X)

            typedef Decimal64 Type;

            const Type NAN_P = bsl::numeric_limits<Type>::quiet_NaN();
            const Type NAN_N = -NAN_P;
            const Type INF_P = bsl::numeric_limits<Type>::infinity();
            const Type INF_N = -INF_P;

            const Type ERROR_VALUE = BDLDFP_DECIMAL_DD(999.0);

            static const struct {
                int         d_line;    // line number
                const char *d_input_p; // input on the stream
                Type        d_exp;     // exp unsigned value
                bool        d_isValid; // isValid flag
            } DATA[] = {
     //---------v

     // line  input                       exp                         isValid
     // ----  -----                       ---                         -------
     {  L_,    "0",                       DEC(0.0),                    true  },
     {  L_,   "-0",                       DEC(0.0),                    true  },
     {  L_,    "0.0",                     DEC(0.0),                    true  },
     {  L_,   "-0.0",                     DEC(0.0),                    true  },
     {  L_,    "1",                       DEC(1.0),                    true  },
     {  L_,   "-1",                       DEC(-1.0),                   true  },
     {  L_,    "1.2",                     DEC(1.2),                    true  },
     {  L_,    "1.23",                    DEC(1.23),                   true  },
     {  L_,    "1.234",                   DEC(1.234),                  true  },
     {  L_,   "12.34",                    DEC(12.34),                  true  },
     {  L_,  "123.4",                     DEC(123.4),                  true  },
     {  L_,   "-1.2",                     DEC(-1.2),                   true  },
     {  L_,   "-1.23",                    DEC(-1.23),                  true  },
     {  L_,   "-1.234",                   DEC(-1.234),                 true  },
     {  L_,  "-12.34",                    DEC(-12.34),                 true  },
     {  L_, "-123.4",                     DEC(-123.4),                 true  },
     {  L_,   "+1.2",                     DEC(1.2),                    true  },
     {  L_,   "+1.23",                    DEC(1.23),                   true  },
     {  L_,   "+1.234",                   DEC(1.234),                  true  },
     {  L_,  "+12.34",                    DEC(12.34),                  true  },
     {  L_, "+123.4",                     DEC(123.4),                  true  },
     {  L_,   "-9.876543210987654e307",   DEC(-9.876543210987654e307), true  },
     {  L_, "\"-0.1\"",                   DEC(-0.1),                   true  },
     {  L_,  "\"0\"",                     DEC(0.0),                    true  },
     {  L_, "\"-0\"",                     DEC(0.0),                    true  },
     {  L_,  "\"0.0\"",                   DEC(0.0),                    true  },
     {  L_, "\"-0.0\"",                   DEC(0.0),                    true  },
     {  L_,  "\"1\"",                     DEC(1.0),                    true  },
     {  L_, "\"-1\"",                     DEC(-1.0),                   true  },
     {  L_,  "\"1.2\"",                   DEC(1.2),                    true  },
     {  L_,  "\"1.23\"",                  DEC(1.23),                   true  },
     {  L_,  "\"1.234\"",                 DEC(1.234),                  true  },
     {  L_, "\"12.34\"",                  DEC(12.34),                  true  },
     {  L_, "\"123.4\"",                  DEC(123.4),                  true  },
     {  L_, "\"-1.2\"",                   DEC(-1.2),                   true  },
     {  L_, "\"-1.23\"",                  DEC(-1.23),                  true  },
     {  L_, "\"-1.234\"",                 DEC(-1.234),                 true  },
     {  L_, "\"-12.34\"",                 DEC(-12.34),                 true  },
     {  L_, "\"-123.4\"",                 DEC(-123.4),                 true  },
     {  L_, "\"+1.2\"",                   DEC(1.2),                    true  },
     {  L_, "\"+1.23\"",                  DEC(1.23),                   true  },
     {  L_, "\"+1.234\"",                 DEC(1.234),                  true  },
     {  L_, "\"+12.34\"",                 DEC(12.34),                  true  },
     {  L_, "\"+123.4\"",                 DEC(123.4),                  true  },
     {  L_, "\"-9.876543210987654e307\"", DEC(-9.876543210987654e307), true  },
     {  L_,   "-0.1",                     DEC(-0.1),                   true  },
     {  L_,  "\"NaN\"",                   NAN_P,                       true  },
     {  L_,  "\"nan\"",                   NAN_P,                       true  },
     {  L_,  "\"NAN\"",                   NAN_P,                       true  },
     {  L_, "\"+NaN\"",                   NAN_P,                       true  },
     {  L_, "\"+nan\"",                   NAN_P,                       true  },
     {  L_, "\"+NAN\"",                   NAN_P,                       true  },
     {  L_, "\"-NaN\"",                   NAN_N,                       true  },
     {  L_, "\"-nan\"",                   NAN_N,                       true  },
     {  L_, "\"-NAN\"",                   NAN_N,                       true  },
     {  L_,  "\"INF\"",                   INF_P,                       true  },
     {  L_,  "\"inf\"",                   INF_P,                       true  },
     {  L_,  "\"infinity\"",              INF_P,                       true  },
     {  L_, "\"+INF\"",                   INF_P,                       true  },
     {  L_, "\"+inf\"",                   INF_P,                       true  },
     {  L_, "\"+infinity\"",              INF_P,                       true  },
     {  L_, "\"-INF\"",                   INF_N,                       true  },
     {  L_, "\"-inf\"",                   INF_N,                       true  },
     {  L_, "\"-infinity\"",              INF_N,                       true  },
     {  L_,         "-",                  ERROR_VALUE,                 false },
     {  L_,       "E-1",                  ERROR_VALUE,                 false },
     {  L_,  "Z34.56e1",                  ERROR_VALUE,                 false },
     {  L_,  "3Z4.56e1",                  ERROR_VALUE,                 false },
#if 0
     {  L_,      "1.1}",                  ERROR_VALUE,                 false },
     {  L_,     "1.1\n",                  ERROR_VALUE,                 false },
#else
     {  L_,      "1.1}",                  DEC(1.1),                    true  },
     {  L_,     "1.1\n",                  DEC(1.1),                    true  },
#endif // 0
     {  L_,   "1.10xFF",                  ERROR_VALUE,                 false },
     {  L_,  "DEADBEEF",                  ERROR_VALUE,                 false },
     {  L_,      "JUNK",                  ERROR_VALUE,                 false },
     {  L_,     "\"0\"",                  DEC(0.0),                    true  },
     {  L_,       "0\"",                  ERROR_VALUE,                 false },
     {  L_,       "\"0",                  ERROR_VALUE,                 false },
     {  L_,        "\"",                  ERROR_VALUE,                 false },
     {  L_,      "\"\"",                  ERROR_VALUE,                 false },
     {  L_,     "\"X\"",                  ERROR_VALUE,                 false },
     {  L_,  "\" NaN\"",                  ERROR_VALUE,                 false },

     //---------v
            };
            const int NUM_DATA = sizeof DATA / sizeof DATA[0];

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE     = DATA[i].d_line;
                const bsl::string INPUT    = DATA[i].d_input_p;
                const Type        EXP      = DATA[i].d_exp;
                const bool        IS_VALID = DATA[i].d_isValid;

                if (veryVerbose) {
                    P_(LINE); P(IS_VALID);
                    P(INPUT);
                    P(EXP);
                }

                bsl::string jsonDoc;
                jsonDoc.append("{");
                jsonDoc.append("\"attribute1\"");
                jsonDoc.append(":");
                jsonDoc.append(INPUT);
                jsonDoc.append("}");

                if (veryVeryVerbose) {
                    P(jsonDoc);
                }

                bsl::istringstream iss(jsonDoc);

                Obj      decoder;
                SeqDec64 seqDec64;
                DOptions dOptions;

                int rc = decoder.decode(iss, &seqDec64, &dOptions);     // TEST

                if (IS_VALID) {
                    ASSERTV(      rc, IS_VALID,
                            0  == rc);
                    ASSERTV(      decoder.loggedMessages(), IS_VALID,
                            "" == decoder.loggedMessages());

                    if (veryVeryVerbose) {
                        P(seqDec64);
                    }

                    if (DecUtil::isNan(EXP)) {
                        ASSERTV(LINE,                seqDec64.attribute1()
                                                                      .value(),
                                      DecUtil::isNan(seqDec64.attribute1()
                                                                      .value())
                               );
                    } else {
                        ASSERTV(LINE, EXP,   seqDec64.attribute1(),
                                      EXP == seqDec64.attribute1());
                    }

                }
                else {
                    ASSERTV(      rc, IS_VALID,
                            0  != rc);
                    ASSERTV(      decoder.loggedMessages(), IS_VALID,
                            "" != decoder.loggedMessages());

                    if (veryVeryVerbose) {
                        P(seqDec64);
                    }
                }
            }
#undef DEC
        }

      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING `allowMissingRequiredAttributes` OPTION
        //
        // Concerns:
        // 1. The `allowMissingRequiredAttributes` configuration option set to
        //    false makes the decoder fail unless all non-optional attributes
        //    are presented in the input message.
        //
        // 2. Sequence elements with a default value are optional.
        //
        // Plan:
        // 1. The `s_baltst::MySequenceWithArray` sequence has 2 non-optional
        //    attributes (`attribute1` and `attribute2`).  Create a JSON that
        //    contains `attribute1`, but lacks `attribute2`.
        //
        // 2. Try to decode this JSON using default options.  Verify that
        //    decoding was successful.  (C-1)
        //
        // 3. Set the `allowMissingRequiredAttributes` option to false and try
        //    to decode the same again.  Verify that decoding failed.  (C-1)
        //
        // 4. The `s_baltst::MySequenceWithDefault` sequence has 2 non-optional
        //    attributes (`attribute1` and `attribute2`), but `attribute2` has
        //    a default value.  Try to decode the same JSON with
        //    `allowMissingRequiredAttributes == false`.  Verify that decoding
        //    was successful.  (C-2)
        //
        // Testing:
        //   `allowMissingRequiredAttributes` OPTION
        // --------------------------------------------------------------------
        if (verbose) cout <<
                   "\nTESTING `allowMissingRequiredAttributes` OPTION" <<
                   "\n===============================================" << endl;

        static const char JSON[] = "{ \"attribute1\": 1 }";
                                     // attribute2 is missing!

        baljsn::DecoderOptions options;
        baljsn::Decoder        decoder;

        {
            bsl::istringstream ss(JSON);
            test::MySequenceWithArray seq;

            ASSERT(options.allowMissingRequiredAttributes() == true);
            ASSERT(decoder.decode(ss, &seq, options) == 0);
        }

        options.setAllowMissingRequiredAttributes(false);
        {
            bsl::istringstream ss(JSON);
            test::MySequenceWithArray seq;

            ASSERT(options.allowMissingRequiredAttributes() == false);
            ASSERT(decoder.decode(ss, &seq, options) != 0);
        }
        {
            bsl::istringstream ss(JSON);
            test::MySequenceWithDefault seq;

            ASSERT(options.allowMissingRequiredAttributes() == false);
            ASSERT(decoder.decode(ss, &seq, options) == 0);
        }
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // MAXDEPTH IS RESPECTED
        //
        // Concerns:
        // 1. The `maxDepth` configuration option is repected.
        //
        // Plan:
        // 1. Manually determine the `maxDepth` value required for various
        //    messages in a table-driven technique.
        //
        // 2. Using those messages, ensure that decoding fails if the
        //    `maxDepth` configuration option is too low, and succeeds when it
        //    is correct.
        //
        // Testing:
        //   `option.setMaxDepth()`
        // --------------------------------------------------------------------

        if (verbose) cout
             << "\nMAXDEPTH IS RESPECTED"
             << "\n====================="
             << endl;

        balxml::MiniReader     xml_reader;
        balxml::ErrorInfo      e;

        for (int i = 0; i < DTMU::k_NUM_MESSAGES; ++i) {
            balb::FeatureTestMessage object;

            const DepthTestMessage& TEST_MESSAGE = DTMU::s_TEST_MESSAGES[i];

            const char *XML   = TEST_MESSAGE.d_XML_text_p;
            const int   LINE  = i;
            const int   DEPTH = TEST_MESSAGE.d_depthJSON;

            bsl::istringstream ss(XML);
            balxml::DecoderOptions xml_options;
            xml_options.setSkipUnknownElements(false);
            balxml::Decoder xml_decoder(&xml_options, &xml_reader, &e);

            int rc = xml_decoder.decode(ss.rdbuf(), &object);
            if (0 != rc) {
                cout << "Failed to decode from initialization data (i="
                     << i << ", LINE=" << LINE
                     << "): " << xml_decoder.loggedMessages() << endl;
            }
            if (balb::FeatureTestMessage::SELECTION_ID_UNDEFINED ==
                object.selectionId()) {
                cout
                    << "Decoded unselected choice from initialization data"
                    << " (LINE =" << LINE
                    << "):" << endl;
                rc = 9;
            }

            bsl::stringstream json_stream;
            baljsn::EncoderOptions encoder_options;
            baljsn::Encoder   encoder;

            encoder_options.setEncodingStyle(baljsn::EncoderOptions::e_PRETTY);
            encoder_options.setSpacesPerLevel(2);
            ASSERTV(
                   LINE,
                   encoder.loggedMessages(),
                   0 == encoder.encode(json_stream, object, &encoder_options));
            bsl::string json_payload = json_stream.str();

            // Set depth too low, expect failure
            {
                int depth = DEPTH - 1;

                json_stream.str(json_payload);
                baljsn::DecoderOptions options;

                options.setMaxDepth(depth);
                options.setSkipUnknownElements(false);

                baljsn::Decoder decoder;

                int rc = decoder.decode(json_stream.rdbuf(),
                                        &object,
                                        &options);
                if (0 == rc) {
                    int delta = DEPTH - depth;

                    cout << "Unexpected success for ";
                    P_(LINE);
                    P_(DEPTH);
                    P_(depth);
                    P(delta);
                }
            }

            // Set depth correctly, expect success
            {
                json_stream.str(json_payload);
                baljsn::DecoderOptions options;

                options.setMaxDepth(DEPTH);

                baljsn::Decoder decoder;

                int rc = decoder.decode(json_stream.rdbuf(),
                                        &object,
                                        &options);
                if (0 != rc) {
                    cout << "Unexpectedly failed to decode from\n"
                         << json_payload
                         << "initialization data (i="
                         << i << ", LINE=" << LINE
                         << "): " << decoder.loggedMessages() << endl;
                }
                if (balb::FeatureTestMessage::SELECTION_ID_UNDEFINED ==
                    object.selectionId()) {
                    cout
                        << "Decoded unselected choice from initialization data"
                        << " (LINE =" << LINE
                        << "):" << endl;
                    rc = 9;
                }

                ASSERTV(LINE, DEPTH, options.maxDepth(), rc, 0 == rc);
            }

            // Set depth too low, expect failure, then set depth correctly
            // on same decoder and expect success.
            {
                int depth = DEPTH - 1;

                json_stream.str(json_payload);
                baljsn::DecoderOptions options;

                options.setMaxDepth(depth);
                options.setSkipUnknownElements(false);

                baljsn::Decoder decoder;

                int rc = decoder.decode(json_stream.rdbuf(),
                                        &object,
                                        &options);

                ASSERTV(LINE, DEPTH, depth, DEPTH - depth, 0 != rc);

                options.setMaxDepth(DEPTH);

                json_stream.str(json_payload);
                rc = decoder.decode(json_stream.rdbuf(),
                                        &object,
                                        &options);
                if (0 != rc) {
                    cout << "Unexpectedly failed to decode from\n"
                         << json_payload
                         << "initialization data (i="
                         << i << ", LINE=" << LINE
                         << "): " << decoder.loggedMessages() << endl;
                }

                ASSERTV(LINE, DEPTH, options.maxDepth(), rc, 0 == rc);
            }
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // STRICT CONFORMANCE
        //
        // Concerns:
        // 1. Invoking `decode` with `DecoderOptions` configured for strict
        //    compliance detects violations fails when any of the strict
        //    requirements is violated.  Specifically:
        //
        //    - `setValidateInputIsUtf8(true)`
        //    - `setAllowConsecutiveSeparators(false)`
        //    - `setAllowFormFeedAsWhitespace(false)`
        //    - `setAllowUnescapedControlCharacters(false)`
        //
        // 1. On failure, the log message correctly describes the nature of the
        //    failure.
        //
        // 2. Given either a default configured or a strictly configured
        //    `DecoderOptions` object, `decode` ignores arbitrary text
        //    following a valid JSON document.
        //
        // 3. Given either a default configured or a strictly configured
        //    `DecoderOptions` object, two valid JSON documents in a stream can
        //    correctly parsed, by successive calls to `decode`, if and only if
        //    the two documents are separated by zero or more whitespace
        //    characters.
        //
        // Plan:
        // 1. Attempt to initialize a `test::Employee` object from a series of
        //    JSON documents in which each document has a single violation of
        //    the requirements of strictness.  `decode` calls using the default
        //    `DecoderOptions` are expected to succeed but those using a
        //    strictly configured `DecoderOptions` options are expected to
        //    fail.  (C-1)
        //
        //   1. Compare the log message to the expected value.  (C-1.1)
        //
        //   2. Note that TC 9 (TESTING UTF-8 DETECTION) provides a more
        //      comprehensive test of the `setValidateInputIsUtf8` option.
        //
        // 2. Append to a valid JSON document additional text consisting of
        //    different combinations of whitespace and non-whitespace
        //    characters.  Confirm that the all-whitespace suffices do not
        //    interfere with the `decode` of the JSON document for either
        //    default or strictly-configures `DecoderOptions`; however, if any
        //    non-whitespace characters are appended, `decode` fails.  (C-2)
        //
        // 3. Given a valid JSON document describing a `test::Employee` object,
        //    append a second version of that document having different values.
        //    Confirm that `decode` of the combined text fails when using a
        //    strictly-configured `DecoderOptions` but the second JSON document
        //    is ignored when a default `DecoderOptions` is used.  (C-3)
        //
        //   1. Repeat with various combinations of whitespace (including no
        //      whitespace separating the two JSON documents.
        //
        //   2. Confirm that when `decode` is successful, the values in the
        //      resulting `test::Employee` object correspond to the first JSON
        //      document in the text.
        //
        // Testing:
        //   `DecoderOptions` CAN BE CONFIGURED FOR STRICT CONFORMANCE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "STRICT CONFORMANCE" << endl
                          << "==================" << endl;

        if (veryVerbose) {
            cout << "Check violations of strictness" << endl;
        }

        baljsn::DecoderOptions mDO;
        mDO.setValidateInputIsUtf8(true);
        mDO.setAllowConsecutiveSeparators(false);
        mDO.setAllowFormFeedAsWhitespace(false);
        mDO.setAllowUnescapedControlCharacters(false);

        const baljsn::DecoderOptions& OPTIONS_STRICT = mDO;
        const baljsn::DecoderOptions  OPTIONS_DEFAULT;

        const char BASELINE[] =
                               "{\"name\":\"Bob\",\"homeAddress\":{\"street\":"
                               "\"Lexington Ave\",\"city\":\"New York City\","
                               "\"state\":\"New York\"},\"age\":21}";
                                                             // ^^

        const char BASELINE2[] =
                               "{\"name\":\"Bob\",\"homeAddress\":{\"street\":"
                               "\"Lexington Ave\",\"city\":\"New York City\","
                               "\"state\":\"New York\"},\"age\":22}";
                                                             // ^^

        const char DOUBLE_COLON[] =
                              "{\"name\"::\"Bob\",\"homeAddress\":{\"street\":"
                                      // ^^
                               "\"Lexington Ave\",\"city\":\"New York City\","
                               "\"state\":\"New York\"},\"age\":21}";

        const char MSG_DOUBLE_COLON[] =
                                 "Error reading value for attribute 'name' \n";

        const char DOUBLE_COMMA[] =
                              "{\"name\":\"Bob\",,\"homeAddress\":{\"street\":"
                                             // ^^
                               "\"Lexington Ave\",\"city\":\"New York City\","
                               "\"state\":\"New York\"},\"age\":21}";

        const char MSG_DOUBLE_COMMA[] =
                     "Could not decode sequence, "
                     "error reading token after value for attribute 'name' \n";

        const char FORMFEED_AS_WHITESPACE[] =
                               "{\"name\":\"Bob\",\"homeAddress\":{\"street\":"
                               "\f"
                             // ^^
                               "\"Lexington Ave\",\"city\":\"New York City\","
                               "\"state\":\"New York\"},\"age\":21}";

        const char MSG_FORMFEED_AS_WHITESPACE[] =
                      "Could not decode sequence, "
                      "error decoding element or bad element name 'street' \n"
                      "Could not decode sequence, "
                      "error decoding element or bad element name 'street' \n";
                      // Yes, message is generated twice.

        const char INVALID_UTF8[] =
                               "{\"name\":\"Bob\",\"homeAddress\":{\"street\":"
                               "\"Lexington Ave\",\"city\":\"New York City\","
                         "\"state\xf4\xa0\x80\x80\":\"New York\"},\"age\":21}";
                              // ^^^^^^^^^^^^^^^^ VALUE_LARGER_THAN_0X10FFFF

        const char MSG_INVALID_UTF8[] =
             "Could not decode sequence, "
             "UTF-8 error VALUE_LARGER_THAN_0X10FFFF "
             "at offset 155 reading token after value for attribute 'city' \n"
             "Could not decode sequence, "
             "error decoding element or bad element name 'city' \n";

        const char UNESCAPED_CONTROL_CHARACTER[] =
                             "{\"name\":\"Bob\n\",\"homeAddress\":{\"street\":"
                                          // ^^
                             "\"Lexington Ave\",\"city\":\"New York City\","
                             "\"state\":\"New York\"},\"age\":21}";

        const char MSG_UNESCAPED_CONTROL_CHARACTER[] =
                                 "Error reading value for attribute 'name' \n";

        const struct  {
            int         d_line;
            const char *d_input;
            const char *d_errorMsg;

        } DATA[] = {
          { L_,  DOUBLE_COLON               , MSG_DOUBLE_COLON                }
        , { L_,  DOUBLE_COMMA               , MSG_DOUBLE_COMMA                }
        , { L_,  FORMFEED_AS_WHITESPACE     , MSG_FORMFEED_AS_WHITESPACE      }
        , { L_,  INVALID_UTF8               , MSG_INVALID_UTF8                }
        , { L_,  UNESCAPED_CONTROL_CHARACTER, MSG_UNESCAPED_CONTROL_CHARACTER }
        };

        const bsl::size_t NUM_DATA = sizeof DATA / sizeof DATA[0];

        for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {

            const int         LINE = DATA[ti].d_line;
            const char *const JSON = DATA[ti].d_input;
            const char *const MSG  = DATA[ti].d_errorMsg;

            if (veryVerbose) {
                T_; P_(LINE); P(JSON);
            }

            if (veryVeryVerbose) {
                T_; T_; P_(LINE); P(MSG);
            }

            test::Employee      employee;
            int                 rc;
            bsl::istringstream  is(JSON);
            Obj                 mX; const Obj& X = mX;

            if (veryVerbose) {
                Q(Test Default);
            }

            rc = mX.decode(is, &employee, OPTIONS_DEFAULT);             // TEST
            ASSERTV(LINE, rc, JSON, 0 == rc);

            if (veryVeryVerbose && 0 != rc) {
                cout << "|" << X.loggedMessages() << "|" << endl;
            }

            is.clear(); is.str(JSON);

            if (veryVerbose) {
                Q(Test Strict);
            }

            rc = mX.decode(is, &employee, OPTIONS_STRICT);              // TEST
            ASSERTV(LINE, rc, JSON, 0 != rc);

            if (veryVeryVerbose && 0 != rc) {
                cout << "|" << X.loggedMessages() << "|" << endl;
            }

            if (0 != rc) {
                ASSERTV(MSG,   X.loggedMessages(),
                        MSG == X.loggedMessages());
            }

            if (veryVeryVerbose && 0 != rc) {
                Q(DEBUG: EXPECTED);
                P(LINE);
                cout << "|" << MSG                << "|" << endl;
                Q(DEBUG: ACTUAL);
                cout << "|" << X.loggedMessages() << "|" << endl;
            }

            is.clear(); is.str(JSON);

            if (veryVerbose) {
                Q(Test Default Again);
            }

            rc = mX.decode(is, &employee, OPTIONS_DEFAULT);             // TEST
            ASSERTV(LINE, rc, JSON, 0 == rc);

            if (veryVeryVerbose && 0 != rc) {
                cout << "|" << X.loggedMessages() << "|" << endl;
            }
        }

        if (veryVerbose) {
            cout << "Check characters trailing JSON document" << endl;
        }

        const struct  {
            int         d_line;
            const char *d_suffix;
            bool        d_expDefault;
            bool        d_expStrict;

        } DATA2[] = {
        //    LINE SUFFIX      EXP_DFT EXP_SRT
        //    ---- ------      ------- -------

            // Just Trailing whitespace
            { L_,  "",         true,   true     }
          , { L_,  " ",        true,   true     }
          , { L_,  "   ",      true,   true     }
          , { L_,  " \t\n\r",  true,   true     }

            // Comma, then trailing whitespace
          , { L_,  ",",        true,   true     }
          , { L_,  ", ",       true,   true     }
          , { L_,  ",   ",     true,   true     }
          , { L_,  ", \t\n\r", true,   true     }

            // Trailing whitespace, then comma
          , { L_,  ",",        true,   true     }
          , { L_,  " ,",       true,   true     }
          , { L_,  "   ,",     true,   true     }
          , { L_,  " \t\n\r,", true,   true     }

            // Other character, then trailing whitespace
          , { L_,  "W",        true,   true     }
          , { L_,  "X ",       true,   true     }
          , { L_,  "Y   ",     true,   true     }
          , { L_,  "Z \t\n\r", true,   true     }

            // Trailing whitespace, then other character
          , { L_,  "A",        true,   true     }
          , { L_,  " B",       true,   true     }
          , { L_,  "   C",     true,   true     }
          , { L_,  " \t\n\rD", true,   true     }
        };

        const bsl::size_t NUM_DATA2 = sizeof DATA2 / sizeof DATA2[0];

        for (bsl::size_t ti = 0; ti < NUM_DATA2; ++ti) {

            const int         LINE    = DATA2[ti].d_line;
            const char *const SUFFIX  = DATA2[ti].d_suffix;
            const bool        EXP_DFT = DATA2[ti].d_expDefault;
            const bool        EXP_SRT = DATA2[ti].d_expStrict;

            if (veryVerbose) {
                T_; P_(LINE); P_(EXP_DFT); P_(EXP_SRT); P(SUFFIX);
            }

            bsl::string input(BASELINE); const bsl::string& INPUT = input;
            input.append(SUFFIX);

            if (veryVeryVerbose) {
                T_; T_; P(INPUT);
            }

            test::Employee        employee;
            const test::Employee& EMPLOYEE = employee;
            int                 rc;
            bsl::istringstream  is(INPUT.c_str());
            Obj                 mX; const Obj& X = mX;

            if (veryVerbose) {
                Q(Test Default);
            }

            rc = mX.decode(is, &employee, OPTIONS_DEFAULT);             // TEST
            ASSERTV(LINE, rc, SUFFIX, EXP_DFT,
                    (true == EXP_DFT) == (0 == rc));

            if (0 == rc) {
                ASSERT(21 == EMPLOYEE.age());
            }

            is.clear(); is.str(INPUT.c_str());

            if (veryVerbose) {
                Q(Test Strict);
            }

            rc = mX.decode(is, &employee, OPTIONS_STRICT);              // TEST
            ASSERTV(LINE, rc, SUFFIX, EXP_SRT,
                    (true == EXP_SRT) == (0 == rc));

            if (veryVeryVerbose && 0 != rc) {
                cout << "|" << X.loggedMessages() << "|" << endl;
            }

            if (0 == rc) {
                ASSERTV(EMPLOYEE.age(), 21 == EMPLOYEE.age());
            }
        }

        const struct  {
            int         d_line;
            const char *d_whitespace;
            bool        d_expDefault;
            bool        d_expStrict;

        } DATA3[] = {
        //    LINE WSPACE        EXP_DFT2 EXP_SRT2
        //    ---- ------        -------  -------
            { L_,  "",           true,    true     }

          , { L_,  " ",          true,    true     }
          , { L_,  "   ",        true,    true     }
          , { L_,  " \t\n\r",    true,    true     }

          , { L_,  ",",          false,   false    }
          , { L_,  " , ",        false,   false    }
          , { L_,  " \t,\n\r",   false,   false    }

          , { L_,  ",",          false,   false    }
          , { L_,  " , ",        false,   false    }
          , { L_,  "X\tY\nZ\r",  false,   false    }
        };

        const bsl::size_t NUM_DATA3 = sizeof DATA3 / sizeof DATA3[0];

        for (bsl::size_t ti = 0; ti < NUM_DATA3; ++ti) {

            const int         LINE     = DATA3[ti].d_line;
            const char *const WSPACE   = DATA3[ti].d_whitespace;;
            const bool        EXP_DFT2 = DATA3[ti].d_expDefault;
            const bool        EXP_SRT2 = DATA3[ti].d_expStrict;

            const bool        EXP_DFT1 = true;
            const bool        EXP_SRT1 = true;

            if (veryVerbose) {
                T_; P_(LINE);
                    P_(EXP_DFT1);
                    P_(EXP_SRT1);
                    P_(EXP_DFT2);
                    P_(EXP_SRT2);
                    P(WSPACE);
            }

            ASSERTV(LINE, EXP_DFT1,   EXP_SRT1,
                          EXP_DFT1 == EXP_SRT1);
            ASSERTV(LINE, EXP_DFT2,   EXP_SRT2,
                          EXP_DFT2 == EXP_SRT2);

            bsl::string input; const bsl::string& INPUT = input;
            input.append(BASELINE);
            input.append(WSPACE);
            input.append(BASELINE2);

            if (veryVeryVerbose) {
                T_; T_; P(INPUT);
            }

            test::Employee        employee1;
            const test::Employee& EMPLOYEE1 = employee1;
            test::Employee        employee2;
            const test::Employee& EMPLOYEE2 = employee2;

            int                 rc;
            bsl::istringstream  is(INPUT.c_str());
            Obj                 mX; const Obj& X = mX;

            if (veryVerbose) {
                Q(Test Default);
            }

            rc = mX.decode(is, &employee1, OPTIONS_DEFAULT);            // TEST
            ASSERTV(LINE, rc, WSPACE, EXP_DFT1,
                    (true == EXP_DFT1) == (0 == rc));

            if (0 == rc) {
                ASSERTV(EMPLOYEE1.age(), 21 == EMPLOYEE1.age());
            }

            rc = mX.decode(is, &employee2, OPTIONS_DEFAULT);            // TEST
            ASSERTV(LINE, rc, WSPACE, EXP_DFT2,
                    (true == EXP_DFT2) == (0 == rc));

            if (0 == rc) {
                ASSERTV(EMPLOYEE2.age(), 22 == EMPLOYEE2.age());
            }

            // Reset stream
            is.clear(); is.str(INPUT.c_str());

            if (veryVerbose) {
                Q(Test Strict);
            }

            rc = mX.decode(is, &employee1, OPTIONS_STRICT);             // TEST
            ASSERTV(LINE, rc, WSPACE, EXP_SRT1,
                    (true == EXP_SRT1) == (0 == rc));

            if (veryVeryVerbose && 0 != rc) {
                cout << "|" << X.loggedMessages() << "|" << endl;
            }

            if (0 == rc) {
                ASSERTV(EMPLOYEE1.age(), 21 == EMPLOYEE1.age());
            }

            rc = mX.decode(is, &employee2, OPTIONS_STRICT);             // TEST
            ASSERTV(LINE, rc, WSPACE, EXP_SRT2,
                    (true == EXP_SRT2) == (0 == rc));

            if (veryVeryVerbose && 0 != rc) {
                cout << "|" << X.loggedMessages() << "|" << endl;
            }

            if (0 == rc) {
                ASSERTV(EMPLOYEE2.age(), 22 == EMPLOYEE2.age());
            }

        };
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // ARRAY HAVING NULLABLE COMPLEX ELEMENTS
        //   Ticket {DRQS 167908706} reports a failure to decode a JSON
        //   document to an array of nillable types when `null` appears in the
        //   document.  This test case reproduces that problem and tests the
        //   fix.  Note that investigation showed that the failure did occured
        //   when the nillable type was a `ComplexType` but not a `SimpleType`.
        //
        // Concerns:
        // 1. The fix enables the successful decoding of `null` for arrays
        //    nillable `ComplexType`s.
        //
        // 2. The successful decoding of `null` for arrays nillable
        //    `SimpleType`s continues to work correctly.
        //
        // 3. The (incorrect) use of `null` in an array of non-nillalble types
        //    continues to fail provide a meaningful error message.
        //
        // 4. The JSON `null` token is decoded irrespective of its position in
        //    the array.
        //
        // Plan:
        // 1. Use three test types:
        //
        //   1. `s_baltst::MySequenceWithNillableIntArray`, a
        //       nillable array of a simple type
        //
        //   2. `s_baltst::MySequenceWithNillableIntSequenceArray`, a
        //      nillable array of a complex type
        //
        //   3. `s_baltst::BigRecord TestObj`: features a non-nillable array
        //      of a complex type (and a few other non-relevant fields).
        //
        // 2. For the first two test types, P1-1 and P1-2:
        //   1. Create a test object
        //   2. Call `reset` on one of the elements of the array to generate
        //      a `null` in the corresponding position in the JSON document.
        //   3. Encode the object into a JSON document.
        //   4. Decode the JSON document into a second object.
        //   5. Compare the first and second objects for equality.
        //   6. Repeat 1-5 for each position in the array.
        //
        // 3. The third test type, P1-3, that provides no `reset` method (it is
        //    *not* nillable), create a series of JSON documents with a `null`
        //    successively in each array position.  Check that each of these
        //    data points fail and provides an error message that reports the
        //    position of the `null`.
        //
        // Testing:
        //   DRQS 167908706
        // --------------------------------------------------------------------

        if (verbose) cout << "ARRAY HAVING NULLABLE COMPLEX ELEMENTS" << endl
                          << "======================================" << endl;

        if (verbose) { Q(Nillable array of simple type);}
        {
            typedef s_baltst::MySequenceWithNillableIntArray TestObj;

            for (int resetIndex = 0; resetIndex < 4; ++resetIndex) {

                if (veryVerbose) {
                    P(resetIndex);
                }

                TestObj mX; const TestObj& X = mX;

                mX.attribute1().push_back(  7);
                mX.attribute1().push_back( 13);
                mX.attribute1().push_back( 42);
                mX.attribute1().push_back(666);

                mX.attribute1()[resetIndex].reset();

                bsl::ostringstream os;
                baljsn::Encoder    encoder;

                int rcEn = encoder.encode(os, X);
                ASSERTV(rcEn, 0 == rcEn);

                bsl::string encoding = os.str();

                const char *const INPUT = encoding.c_str();
                if (veryVerbose) {
                    P(INPUT);
                }
                bsl::istringstream is(INPUT);

                TestObj mY; const TestObj& Y = mY;

                Obj decoder;
                int rcDe = decoder.decode(is, &mY);                     // TEST

                ASSERTV(rcDe, 0 == rcDe);
                ASSERTV(decoder.loggedMessages(),
                        decoder.loggedMessages() == "");

                ASSERTV(X, Y, X == Y);
            }
        }

        if (verbose) { Q(Nillable array of complex type);}
        {
            typedef s_baltst::MySequenceWithNillableIntSequenceArray
                                                                 TestObj;
            typedef s_baltst::MySequenceWithNillableIntSequenceArraySequence
                                                                 TestObjValue;

            TestObjValue v007; v007.attribute1() =   7;
            TestObjValue v013; v013.attribute1() =  13;
            TestObjValue v042; v042.attribute1() =  42;
            TestObjValue v666; v666.attribute1() = 666;

            for (int resetIndex = 0; resetIndex < 4; ++resetIndex) {

                if (veryVerbose) {
                    P(resetIndex);
                }

                TestObj mX; const TestObj& X = mX;
                mX.attribute1().push_back(v007);
                mX.attribute1().push_back(v013);
                mX.attribute1().push_back(v042);
                mX.attribute1().push_back(v666);

                mX.attribute1()[resetIndex].reset();

                bsl::ostringstream os;
                baljsn::Encoder    encoder;

                int rcEn = encoder.encode(os, X);
                ASSERTV(rcEn, 0 == rcEn);

                bsl::string encoding = os.str();

                const char *const INPUT = encoding.c_str();

                if (veryVerbose) {
                    P(INPUT);
                }

                bsl::istringstream is(INPUT);

                TestObj mY; const TestObj& Y = mY;

                Obj decoder;
                int rcDe = decoder.decode(is, &mY);                     // TEST
                ASSERTV(rcDe, 0 == rcDe);
                ASSERTV(decoder.loggedMessages(),
                        decoder.loggedMessages() == "");
                ASSERTV(X, Y, X == Y);
            }
        }

        if (verbose) { Q(non-nillable array of complex type);}
        {
            typedef s_baltst::  BigRecord TestObj;
            typedef s_baltst::BasicRecord TestObjValue;

            TestObjValue mV1; const TestObjValue& V1 = mV1;
            mV1.i1() = 11;
            mV1.i2() = 12;
            mV1.dt() = bdlt::DatetimeTz();
            mV1. s() = "I am 'BasicRecord' value1.";

            TestObjValue mV2; const TestObjValue& V2 = mV2;
            mV2.i1() = 21;
            mV2.i2() = 22;
            mV2.dt() = bdlt::DatetimeTz();
            mV2. s() = "I am 'BasicRecord' value2.";

            TestObjValue mV3; const TestObjValue& V3 = mV3;
            mV3.i1() = 31;
            mV3.i2() = 32;
            mV3.dt() = bdlt::DatetimeTz();
            mV3. s() = "I am 'BasicRecord' value3.";

            if (veryVerbose) {
                P(V1);
                P(V2);
                P(V3);
            }

            TestObj mX; const TestObj& X = mX;
            mX.name() = "I am an array of 'BasicRecord's.";
            mX.array().push_back(V1);
            mX.array().push_back(V2);
            mX.array().push_back(V3);

            bsl::ostringstream     os;
            baljsn::Encoder        encoder;
            baljsn::EncoderOptions encoderOptions;

            encoderOptions.setEncodingStyle(baljsn::EncodingStyle::e_PRETTY);
            encoderOptions.setSpacesPerLevel(4);

            int rcEn = encoder.encode(os, X, encoderOptions);

            ASSERTV(rcEn, 0 == rcEn);

            bsl::string encoding = os.str();
            if (veryVerbose && 0 != rcEn) {
                P(encoding);
            }

#define NL "\n"
            const char *const INPUT_OK =
            "{"                                                              NL
            "    \"name\": \"I am an array of 'BasicRecord's.\","           NL
            "    \"array\": ["                                              NL
            "        {"                                                      NL
            "            \"i1\": 11,"                                       NL
            "            \"i2\": 12,"                                       NL
            "            \"dt\": \"0001-01-01T24:00:00.000+00:00\","        NL
            "            \"s\": \"I am 'BasicRecord' value1.\""             NL
            "        },"                                                     NL
            "        {"                                                      NL
            "            \"i1\": 21,"                                       NL
            "            \"i2\": 22,"                                       NL
            "            \"dt\": \"0001-01-01T24:00:00.000+00:00\","        NL
            "            \"s\": \"I am 'BasicRecord' value2.\""             NL
            "        },"                                                     NL
            "        {"                                                      NL
            "            \"i1\": 31,"                                       NL
            "            \"i2\": 32,"                                       NL
            "            \"dt\": \"0001-01-01T24:00:00.000+00:00\","        NL
            "            \"s\": \"I am 'BasicRecord' value3.\""             NL
            "        }"                                                      NL
            "    ]"                                                          NL
            "}"                                                              NL
            ;

            const char *LOG_OK = "";

            ASSERTV(encoding.size(),   bsl::strlen(INPUT_OK),
                    encoding.size() == bsl::strlen(INPUT_OK));

            ASSERTV(encoding, INPUT_OK, encoding == INPUT_OK);

            const char *const INPUT_NG0 =
            "{"                                                              NL
            "    \"name\": \"I am an array of 'BasicRecord's.\","           NL
            "    \"array\": ["                                              NL

            "        null,"                                                  NL

            "        {"                                                      NL
            "            \"i1\": 21,"                                       NL
            "            \"i2\": 22,"                                       NL
            "            \"dt\": \"0001-01-01T24:00:00.000+00:00\","        NL
            "            \"s\": \"I am 'BasicRecord' value2.\""             NL
            "        },"                                                     NL
            "        {"                                                      NL
            "            \"i1\": 31,"                                       NL
            "            \"i2\": 32,"                                       NL
            "            \"dt\": \"0001-01-01T24:00:00.000+00:00\","        NL
            "            \"s\": \"I am 'BasicRecord' value3.\""             NL
            "        }"                                                      NL
            "    ]"                                                          NL
            "}"                                                              NL
            ;

            const char *LOG_NG0 =
"Could not decode sequence, missing starting '{'"                            NL
"Error adding element '0'"                                                   NL
"Could not decode sequence, error decoding element or bad element name "
                                                                  "'array' " NL
;
            const char *const INPUT_NG1 =
            "{"                                                              NL
            "    \"name\": \"I am an array of 'BasicRecord's.\","           NL
            "    \"array\": ["                                              NL
            "        {"                                                      NL
            "            \"i1\": 11,"                                       NL
            "            \"i2\": 12,"                                       NL
            "            \"dt\": \"0001-01-01T24:00:00.000+00:00\","        NL
            "            \"s\": \"I am 'BasicRecord' value1.\""             NL
            "        },"                                                     NL

            "        null,"                                                  NL

            "        {"                                                      NL
            "            \"i1\": 31,"                                       NL
            "            \"i2\": 32,"                                       NL
            "            \"dt\": \"0001-01-01T24:00:00.000+00:00\","        NL
            "            \"s\": \"I am 'BasicRecord' value3.\""             NL
            "        }"                                                      NL
            "    ]"                                                          NL
            "}"                                                              NL
            ;

            const char *LOG_NG1 =
"Could not decode sequence, missing starting '{'"                            NL
"Error adding element '1'"                                                   NL
"Could not decode sequence, error decoding element or bad element name 's' " NL
;

            const char *const INPUT_NG2 =
            "{"                                                              NL
            "    \"name\": \"I am an array of 'BasicRecord's.\","           NL
            "    \"array\": ["                                              NL
            "        {"                                                      NL
            "            \"i1\": 11,"                                       NL
            "            \"i2\": 12,"                                       NL
            "            \"dt\": \"0001-01-01T24:00:00.000+00:00\","        NL
            "            \"s\": \"I am 'BasicRecord' value1.\""             NL
            "        },"                                                     NL
            "        {"                                                      NL
            "            \"i1\": 21,"                                       NL
            "            \"i2\": 22,"                                       NL
            "            \"dt\": \"0001-01-01T24:00:00.000+00:00\","        NL
            "            \"s\": \"I am 'BasicRecord' value2.\""             NL
            "        },"                                                     NL

            "        null"                                                   NL

            "    ]"                                                          NL
            "}"                                                              NL
            ;

            const char *LOG_NG2 =
"Could not decode sequence, missing starting '{'"                            NL
"Error adding element '2'"                                                   NL
"Could not decode sequence, error decoding element or bad element name 's' " NL
;

            const struct {
                int         d_line;
                const char *d_input;
                bool        d_isValid;
                const char *d_logMessage;
            } DATA[] = {
              { L_, INPUT_OK,  true,  LOG_OK  }
            , { L_, INPUT_NG0, false, LOG_NG0 }
            , { L_, INPUT_NG1, false, LOG_NG1 }
            , { L_, INPUT_NG2, false, LOG_NG2 }
            };

            const bsl::size_t NUM_DATA = sizeof DATA / sizeof DATA[0];

            for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE     = DATA[ti].d_line;
                const char *const INPUT    = DATA[ti].d_input;
                const bool        IS_VALID = DATA[ti].d_isValid;
                const char *const LOG      = DATA[ti].d_logMessage;

                if (veryVerbose) {
                    T_; P_(LINE); P(IS_VALID);
                    P(INPUT);
                    P(LOG);
                }

                Obj                decoder;
                bsl::istringstream is(INPUT);

                TestObj mY; const TestObj& Y = mY;

                int rcDe = decoder.decode(is, &mY);                     // TEST

                ASSERTV(IS_VALID, rcDe, IS_VALID == (0 == rcDe));

                if (0 == rcDe) {
                    ASSERTV(X == Y);
                } else {
                    ASSERTV(LOG,   decoder.loggedMessages(),
                            LOG == decoder.loggedMessages());
                }
            }
        }
#undef NL
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // DECODING INTS AS ENUMS AND VICE VERSA
        //
        // Concerns:
        // 1. The encoding produced by `baljsn_encoder` when encoding an
        //    integer enumeration can be decoded into either a plain integral
        //    type or a customized type (as produced by bas_codegen) whose base
        //    type is integral.
        //
        // 2. The encoding produced by `baljsn_encoder` when encoding either a
        //    plain integral type or a customized type (as produced by
        //    bas_codegen) whose base type is integral can be decoded into an
        //    integer enumeration type.
        //
        // Plan:
        // 1. Define a type, `Enumeration1`, that is a `bdlat` enumeration, can
        //    hold an integer value of either 0 or 1, and whose string
        //    representation is just the decimal form of its value (as with
        //    `bcem_Aggregate`).
        //
        // 2. Using `baljsn_encoder`, encode objects of type `Enumeration1`
        //    having values of 0 and 1, and decode them into plain `int`s.
        //    Verify that the resulting values are the same as the original
        //    values.  Then, repeat using the generated type
        //    `test::MyIntEnumeration`.  (C-1)
        //
        // 3. Using `baljsn_encoder`, encode plain `int`s having values of 0
        //    and 1, decode them into `Enumeration1`, and verify that the
        //    resulting values are the same as the original values.  Then,
        //    repeat using the generated type `test::MyIntEnumeration`.  (C-2)
        //
        // Testing:
        //   DECODING INTS AS ENUMS AND VICE VERSA
        // --------------------------------------------------------------------

        if (verbose) cout << "\nDECODING INTS AS ENUMS AND VICE VERSA"
                          << "\n=====================================" << endl;

        baljsn::Encoder   encoder;
        baljsn::Decoder   decoder;
        bsl::stringstream ss;

        bsl::vector<test::Enumeration1>     ve(2);
        bsl::vector<int>                    vi(2);
        bsl::vector<test::MyIntEnumeration> vc(2);

        // P-1
        ASSERT(0 == bdlat_EnumFunctions::fromInt(&ve[0], 0));
        ASSERT(0 == bdlat_EnumFunctions::fromInt(&ve[1], 1));
        ASSERT(0 == encoder.encode(ss, ve, baljsn::EncoderOptions()));
        ASSERT(0 == decoder.decode(ss, &vi, baljsn::DecoderOptions()));
        ASSERT(0 == vi[0]);
        ASSERT(1 == vi[1]);
        ss.seekg(0);
        ASSERT(0 == decoder.decode(ss, &vc, baljsn::DecoderOptions()));
        ASSERT(test::MyIntEnumeration(0) == vc[0]);
        ASSERT(test::MyIntEnumeration(1) == vc[1]);

        // P-2
        int value;
        ss.str("");
        ASSERT(0 == encoder.encode(ss, vi, baljsn::EncoderOptions()));
        ASSERT(0 == decoder.decode(ss, &ve, baljsn::DecoderOptions()));
        bdlat_EnumFunctions::toInt(&value, ve[0]);
        ASSERT(0 == value);
        bdlat_EnumFunctions::toInt(&value, ve[1]);
        ASSERT(1 == value);
        ss.str("");
        ASSERT(0 == encoder.encode(ss, vc, baljsn::EncoderOptions()));
        ASSERT(0 == decoder.decode(ss, &ve, baljsn::DecoderOptions()));
        bdlat_EnumFunctions::toInt(&value, ve[0]);
        ASSERT(0 == value);
        bdlat_EnumFunctions::toInt(&value, ve[1]);
        ASSERT(1 == value);
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // FALLBACK ENUMERATORS
        //
        // Concerns:
        // 1. When decoding into an enumeration type with a fallback
        //    enumerator, if the encoded value corresponds to a known
        //    enumerator, that enumerator is the result of the decoding.
        // 2. When decoding into an enumeration type with a fallback
        //    enumerator, if the encoded value does not correspond to any known
        //    enumerator, the decoding still succeeds and produces the fallback
        //    enumerator value.
        //
        // Plan:
        // 1. Use the table-driven technique, specify a set of valid encoded
        //    values of the enumeration `test:MyEnumerationWithFallback`,
        //    including ones that correspond to both known and unknown
        //    enumerators, together with the expected result of decoding that
        //    value (either the corresponding enumerator value or the fallback
        //    enumerator value, respectively).
        // 2. Generate a JSON input string representing an array of encoded
        //    values by concatenating the encoded values specified in P-1,
        //    separated by commas, and delimited by '[' and ']'.
        //    Simultaneously, generate a vector of expected values, which
        //    consists of the enumerator values specified in P-1.
        // 3. Verify that the result of decoding the JSON input string created
        //    in P-2 equals the vector of expected values created in P-2
        //    (C-1..2).
        //
        // Note: The reason why this is a separate test case is to avoid
        // further increasing the size of {`balb_testmessages`}, which would
        // affect every component that depends on it.
        //
        // Testing:
        //   FALLBACK ENUMERATORS
        // --------------------------------------------------------------------

        if (verbose) cout << "\nFALLBACK ENUMERATORS"
                          << "\n====================" << endl;

        typedef test::MyEnumerationWithFallback Enum;

        static const struct {
            const char *d_json_p;
            Enum::Value d_expected;
        } DATA[] = {
            { "\"VALUE1\"",  Enum::VALUE1  },
            { "\"VALUE2\"",  Enum::VALUE2  },
            { "\"UNKNOWN\"", Enum::UNKNOWN },
            { "\"VALUE3\"",  Enum::UNKNOWN },
        };
        static const int DATA_LEN = sizeof DATA / sizeof DATA[0];

        bsl::string input = "[";
        bsl::vector<Enum::Value> expected;
        for (int i = 0; i < DATA_LEN; i++) {
            if (i > 0) {
                input.push_back(',');
            }
            input.append(DATA[i].d_json_p);
            expected.push_back(DATA[i].d_expected);
        }
        input.push_back(']');

        for (int UTF8 = 0; UTF8 < 2; UTF8++) {
            if (veryVerbose) { T_; P(UTF8); }

            bsl::vector<Enum::Value>   value;
            baljsn::DecoderOptions     options;
            bdlsb::FixedMemInStreamBuf isb(input.data(), input.length());
            Obj                        decoder;
            if (UTF8) options.setValidateInputIsUtf8(true);

            const int rc = decoder.decode(&isb, &value, options);

            ASSERTV(decoder.loggedMessages(), rc, 0 == rc);
            ASSERTV(isb.length(), 0 == isb.length());
            ASSERTV(decoder.loggedMessages(),
                    expected.size(),   value.size(),
                    expected.size() == value.size());
            if (expected.size() == value.size()) {
                for (bsl::size_t i = 0; i < expected.size(); i++) {
                    ASSERTV(expected[i], value[i], expected[i] == value[i]);
                }
            }
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // REPRODUCE SCENARIO FROM DRQS 169438741
        //
        // Concerns:
        // 1. Encoded `bdlt::Date` and `bdlt::DateTz` values can be decoded to
        //    `bdlb::Variant2<bdlt::Date, bdlt::DateTz>` object.
        //
        // 2. Encoded `bdlt::Time` and `bdlt::TimeTz` values can be decoded to
        //    `bdlb::Variant2<bdlt::Time, bdlt::TimeTz>` object.
        //
        // 3. Encoded `bdlt::Datetime` and `bdlt::DatetimeTz` values can be
        //    decoded to `bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz>`
        //    object.
        //
        // Plan:
        // 1. For a number of different JSON date-and-time representations,
        //    enumerate JSON arrays of such elements and verify that the JSON
        //    array decodes into the corresponding `bsl::vector` value.
        //    (C-1..3)
        //
        // Testing:
        //   DRQS 169438741
        // --------------------------------------------------------------------

        if (verbose) cout << "\nREPRODUCE SCENARIO FROM DRQS 169438741"
                          << "\n======================================"
                          << endl;
        {
            typedef bdlb::Variant2<bdlt::Date, bdlt::DateTz> DateOrDateTz;
            typedef bdlb::Variant2<bdlt::Time, bdlt::TimeTz> TimeOrTimeTz;
            typedef bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz>
                                                          DatetimeOrDatetimeTz;

            baljsn::Decoder        mX;
            baljsn::DecoderOptions options;

            // Testing `bdlt::Date` and `bdltDateTz`.
            {
                const char *ENCODED_DATES = "[\"0002-02-02\","
                                            " \"9999-12-31\","
                                            " \"0002-02-02Z\","
                                            " \"9999-12-31-01:30\"]";

                bdlsb::FixedMemInStreamBuf streamBuffer(
                                                   ENCODED_DATES,
                                                   bsl::strlen(ENCODED_DATES));

                const bdlt::Date   EXP_DATE0(2,     2,  2);
                const bdlt::Date   EXP_DATE1(9999, 12, 31);
                const bdlt::DateTz EXP_DATE2(EXP_DATE0,   0);
                const bdlt::DateTz EXP_DATE3(EXP_DATE1, -90);

                bsl::vector<DateOrDateTz> dateVector(4);

                int rc = mX.decode(&streamBuffer, &dateVector, options);
                ASSERTV(rc, 0 == rc);

                ASSERTV(dateVector[0].is<bdlt::Date>());
                ASSERTV(EXP_DATE0,   dateVector[0].the<bdlt::Date>(),
                        EXP_DATE0 == dateVector[0].the<bdlt::Date>());

                ASSERTV(dateVector[1].is<bdlt::Date>());
                ASSERTV(EXP_DATE1,   dateVector[1].the<bdlt::Date>(),
                        EXP_DATE1 == dateVector[1].the<bdlt::Date>());

                ASSERTV(dateVector[2].is<bdlt::DateTz>());
                ASSERTV(EXP_DATE2,   dateVector[2].the<bdlt::DateTz>(),
                        EXP_DATE2 == dateVector[2].the<bdlt::DateTz>());

                ASSERTV(dateVector[3].is<bdlt::DateTz>());
                ASSERTV(EXP_DATE3,   dateVector[3].the<bdlt::DateTz>(),
                        EXP_DATE3 == dateVector[3].the<bdlt::DateTz>());
            }

            // Testing `bdlt::Time` and `bdltTimeTz`.
            {
                const char *ENCODED_TIMES = "[\"01:01:01\","
                                            " \"23:59:59.999999\","
                                            " \"01:01:01Z\","
                                            " \"23:59:59.999999-01:30\"]";

                bdlsb::FixedMemInStreamBuf streamBuffer(
                                                   ENCODED_TIMES,
                                                   bsl::strlen(ENCODED_TIMES));

                const bdlt::Time   EXP_TIME0(1,   1,  1);
                const bdlt::Time   EXP_TIME1(23, 59, 59, 999, 999);
                const bdlt::TimeTz EXP_TIME2(EXP_TIME0,   0);
                const bdlt::TimeTz EXP_TIME3(EXP_TIME1, -90);

                bsl::vector<TimeOrTimeTz> timeVector(4);

                int rc = mX.decode(&streamBuffer, &timeVector, options);
                ASSERTV(rc, 0 == rc);

                ASSERTV(timeVector[0].is<bdlt::Time>());
                ASSERTV(EXP_TIME0,   timeVector[0].the<bdlt::Time>(),
                        EXP_TIME0 == timeVector[0].the<bdlt::Time>());

                ASSERTV(timeVector[1].is<bdlt::Time>());
                ASSERTV(EXP_TIME1,   timeVector[1].the<bdlt::Time>(),
                        EXP_TIME1 == timeVector[1].the<bdlt::Time>());

                ASSERTV(timeVector[2].is<bdlt::TimeTz>());
                ASSERTV(EXP_TIME2,   timeVector[2].the<bdlt::TimeTz>(),
                        EXP_TIME2 == timeVector[2].the<bdlt::TimeTz>());

                ASSERTV(timeVector[3].is<bdlt::TimeTz>());
                ASSERTV(EXP_TIME3,   timeVector[3].the<bdlt::TimeTz>(),
                        EXP_TIME3 == timeVector[3].the<bdlt::TimeTz>());
            }

            // Testing `bdlt::Datetime` and `bdltDatetimeTz`.
            {
                const char *ENCODED_DATETIMES =
                                     "[\"0001-01-01T00:00:00.000000\","
                                     " \"9998-12-31T23:59:60.9999999\","
                                     " \"0001-01-01T00:00:00.0000001Z\","
                                     " \"9998-12-31T23:59:60.9999999+00:30\"]";

                bdlsb::FixedMemInStreamBuf streamBuffer(
                                               ENCODED_DATETIMES,
                                               bsl::strlen(ENCODED_DATETIMES));

               const bdlt::Datetime   EXP_DATETIME0(   1, 1, 1, 0, 0, 0, 0, 0);
               const bdlt::Datetime   EXP_DATETIME1(9999, 1, 1, 0, 0, 1, 0, 0);
               const bdlt::DatetimeTz EXP_DATETIME2(EXP_DATETIME0,   0);
               const bdlt::DatetimeTz EXP_DATETIME3(EXP_DATETIME1,  30);

                bsl::vector<DatetimeOrDatetimeTz> dtVector(4);

                int rc = mX.decode(&streamBuffer, &dtVector, options);
                ASSERTV(rc, 0 == rc);

                ASSERTV(dtVector[0].is<bdlt::Datetime>());
                ASSERTV(EXP_DATETIME0,   dtVector[0].the<bdlt::Datetime>(),
                        EXP_DATETIME0 == dtVector[0].the<bdlt::Datetime>());

                ASSERTV(dtVector[1].is<bdlt::Datetime>());
                ASSERTV(EXP_DATETIME1,   dtVector[1].the<bdlt::Datetime>(),
                        EXP_DATETIME1 == dtVector[1].the<bdlt::Datetime>());

                ASSERTV(dtVector[2].is<bdlt::DatetimeTz>());
                ASSERTV(EXP_DATETIME2,   dtVector[2].the<bdlt::DatetimeTz>(),
                        EXP_DATETIME2 == dtVector[2].the<bdlt::DatetimeTz>());

                ASSERTV(dtVector[3].is<bdlt::DatetimeTz>());
                ASSERTV(EXP_DATETIME3,   dtVector[3].the<bdlt::DatetimeTz>(),
                        EXP_DATETIME3 == dtVector[3].the<bdlt::DatetimeTz>());
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // FLOATING-POINT VALUES ROUND-TRIP
        //
        // Concerns:
        // 1. Numbers encoded without precision restrictions decode back to the
        //    same number (round-trip).
        //
        // 2. Providing no options or default options does not change behavior.
        //
        // Plan:
        // 1. Use the table-driven technique:
        //
        //   1. Specify a set of valid values, including those that will test
        //      the precision of the output.
        //
        //   2. Encode, then decode each value and verify that the decoded
        //      value is as expected.
        //
        // 2. Do those for `float` as well as `double` values.
        //
        // Testing:
        //  FLOATING-POINT VALUES ROUND-TRIP
        // --------------------------------------------------------------------

        if (verbose) cout << "\nFLOATING-POINT VALUES ROUND-TRIP"
                          << "\n================================" << endl;

        if (verbose) cout << "Round-trip `float`" << endl;
        {
            typedef bsl::numeric_limits<float> Limits;

            const float neg0 = copysignf(0.0f, -1.0f);

            const struct {
                int   d_line;
                float d_value;
            } DATA[] = {
                //LINE        VALUE
                //----  -------------
                { L_,           0.0f },
                { L_,         0.125f },
                { L_,        1.0e-1f },
                { L_,      0.123456f },
                { L_,           1.0f },
                { L_,           1.5f },
                { L_,          10.0f },
                { L_,         1.5e1f },
                { L_,   1.23456e-20f },
                { L_,   0.123456789f },
                { L_,  0.1234567891f },

                { L_,           neg0 },
                { L_,        -0.125f },
                { L_,       -1.0e-1f },
                { L_,     -0.123456f },
                { L_,          -1.0f },
                { L_,          -1.5f },
                { L_,         -10.0f },
                { L_,        -1.5e1f },
                { L_,  -1.23456e-20f },
                { L_,  -0.123456789f },
                { L_, -0.1234567891f },

                // {DRQS 165162213} regression, 2^24 loses precision as float
                { L_, 1.0f * 0xFFFFFF },

                // Full Mantissa Integers
                { L_, 1.0f * 0xFFFFFF },
                { L_, 1.0f * 0xFFFFFF     // this happens to be also
                       * (1ull << 63)     // `NumLimits::max()`
                       * (1ull << 41) },

                // Boundary Values
                { L_,  Limits::min()        },
                { L_,  Limits::denorm_min() },
                { L_,  Limits::max()        },
                { L_, -Limits::min()        },
                { L_, -Limits::denorm_min() },
                { L_, -Limits::max()        },
            };
            const int NUM_DATA = sizeof DATA / sizeof DATA[0];

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE  = DATA[ti].d_line;
                const float VALUE = DATA[ti].d_value;

                s_baltst::SqrtF toEncode;
                toEncode.value() = VALUE;

                // No options (NULL)
                bsl::stringstream ss;
                baljsn::Encoder   encoder;
                ASSERTV(LINE, encoder.loggedMessages(),
                        0 == encoder.encode(ss, toEncode));

                s_baltst::SqrtF  decoded;
                decoded.value() = Limits::quiet_NaN(); // A value we don't use
                baljsn::Decoder decoder;
                ASSERTV(LINE, decoder.loggedMessages(),
                        0 == decoder.decode(ss, &decoded, 0));
                float DECODED = decoded.value();
                bsl::string encoded(ss.str());
                ASSERTV(LINE, encoded, VALUE, DECODED, VALUE == DECODED);
                // We also use `memcmp` to ensure that we really get back the
                // exact same binary IEEE-754, bit-by-bit.
                ASSERTV(LINE, encoded, VALUE, DECODED,
                        0 == bsl::memcmp(&VALUE, &DECODED, sizeof VALUE));

                // Default encoder options, no decoder options (NULL)
                ss.str(""); ss.clear();
                baljsn::EncoderOptions encoderOptions;
                ASSERTV(LINE, encoder.loggedMessages(),
                        0 == encoder.encode(ss, toEncode, &encoderOptions));

                decoded.value() = Limits::quiet_NaN(); // A value we don't use
                ASSERTV(LINE, decoder.loggedMessages(),
                        0 == decoder.decode(ss, &decoded, 0));
                DECODED = decoded.value();
                encoded = ss.str();
                ASSERTV(LINE, encoded, VALUE, DECODED, VALUE == DECODED);
                // We also use `memcmp` to ensure that we really get back the
                // exact same binary IEEE-754, bit-by-bit.
                ASSERTV(LINE, encoded, VALUE, DECODED,
                        0 == bsl::memcmp(&VALUE, &DECODED, sizeof VALUE));

                // Default encoder options, default decoder options
                ss.str(""); ss.clear();
                ASSERTV(LINE, encoder.loggedMessages(),
                        0 == encoder.encode(ss, toEncode, &encoderOptions));

                decoded.value() = Limits::quiet_NaN(); // A value we don't use
                baljsn::DecoderOptions decoderOptions;
                ASSERTV(LINE, decoder.loggedMessages(),
                        0 == decoder.decode(ss, &decoded, &decoderOptions));
                DECODED = decoded.value();
                encoded = ss.str();
                ASSERTV(LINE, encoded, VALUE, DECODED, VALUE == DECODED);
                // We also use `memcmp` to ensure that we really get back the
                // exact same binary IEEE-754, bit-by-bit.
                ASSERTV(LINE, encoded, VALUE, DECODED,
                        0 == bsl::memcmp(&VALUE, &DECODED, sizeof VALUE));

                // Encoder options enable NaN & Inf, default decoder options
                // Should not cause change as we don't test those values here
                ss.str(""); ss.clear();
                encoderOptions.setEncodeInfAndNaNAsStrings(true);
                ASSERTV(LINE, encoder.loggedMessages(),
                        0 == encoder.encode(ss, toEncode, &encoderOptions));

                decoded.value() = Limits::quiet_NaN(); // A value we don't use
                ASSERTV(LINE, decoder.loggedMessages(),
                        0 == decoder.decode(ss, &decoded, &decoderOptions));
                DECODED = decoded.value();
                encoded = ss.str();
                ASSERTV(LINE, encoded, VALUE, DECODED, VALUE == DECODED);
                // We also use `memcmp` to ensure that we really get back the
                // exact same binary IEEE-754, bit-by-bit.
                ASSERTV(LINE, encoded, VALUE, DECODED,
                        0 == bsl::memcmp(&VALUE, &DECODED, sizeof VALUE));

                // Encoder options enable NaN & Inf, no decoder options (NULL)
                // Should not cause change as we don't test those values here
                ss.str(""); ss.clear();
                encoderOptions.setEncodeInfAndNaNAsStrings(true);
                ASSERTV(LINE, encoder.loggedMessages(),
                        0 == encoder.encode(ss, toEncode, &encoderOptions));

                decoded.value() = Limits::quiet_NaN(); // A value we don't use
                ASSERTV(LINE, decoder.loggedMessages(),
                        0 == decoder.decode(ss, &decoded, 0));
                DECODED = decoded.value();
                encoded = ss.str();
                ASSERTV(LINE, encoded, VALUE, DECODED, VALUE == DECODED);
                // We also use `memcmp` to ensure that we really get back the
                // exact same binary IEEE-754, bit-by-bit.
                ASSERTV(LINE, encoded, VALUE, DECODED,
                        0 == bsl::memcmp(&VALUE, &DECODED, sizeof VALUE));

                // Encoder options set `maxDoublePrecision`, no decoder options
                // (NULL).  Since we are encoding `float`, there should be no
                // effect.
                ss.str(""); ss.clear();
                encoderOptions.reset();
                encoderOptions.setMaxDoublePrecision(2);
                ASSERTV(LINE, encoder.loggedMessages(),
                        0 == encoder.encode(ss, toEncode, &encoderOptions));

                decoded.value() = Limits::quiet_NaN(); // A value we don't use
                ASSERTV(LINE, decoder.loggedMessages(),
                        0 == decoder.decode(ss, &decoded, 0));
                DECODED = decoded.value();
                encoded = ss.str();
                ASSERTV(LINE, encoded, VALUE, DECODED, VALUE == DECODED);
                // We also use `memcmp` to ensure that we really get back the
                // exact same binary IEEE-754, bit-by-bit.
                ASSERTV(LINE, encoded, VALUE, DECODED,
                        0 == bsl::memcmp(&VALUE, &DECODED, sizeof VALUE));

                // Encoder options set `maxFloatPrecision`, no decoder options
                // (NULL).  We should get the same number back.
                ss.str(""); ss.clear();
                encoderOptions.reset();
                encoderOptions.setMaxFloatPrecision(0);
                ASSERTV(LINE, encoder.loggedMessages(),
                        0 == encoder.encode(ss, toEncode, &encoderOptions));

                decoded.value() = Limits::quiet_NaN(); // A value we don't use
                ASSERTV(LINE, decoder.loggedMessages(),
                        0 == decoder.decode(ss, &decoded, 0));
                DECODED = decoded.value();
                encoded = ss.str();
                ASSERTV(LINE, encoded, VALUE, DECODED, VALUE == DECODED);
                // We also use `memcmp` to ensure that we really get back the
                // exact same binary IEEE-754, bit-by-bit.
                ASSERTV(LINE, encoded, VALUE, DECODED,
                        0 == bsl::memcmp(&VALUE, &DECODED, sizeof VALUE));

                // Encoder options set `maxFloatPrecision` to maximum necessary
                // for round trip, no decoder options (NULL).  We should get
                // the same number back.
                ss.str(""); ss.clear();
                encoderOptions.reset();
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
                encoderOptions.setMaxFloatPrecision(Limits::max_digits10);
#elif defined(FLT_DECIMAL_DIG)
                encoderOptions.setMaxFloatPrecision(FLT_DECIMAL_DIG);
#else
                encoderOptions.setMaxFloatPrecision(9);
#endif
                ASSERTV(LINE, encoder.loggedMessages(),
                        0 == encoder.encode(ss, toEncode, &encoderOptions));

                decoded.value() = Limits::quiet_NaN(); // A value we don't use
                ASSERTV(LINE, decoder.loggedMessages(),
                        0 == decoder.decode(ss, &decoded, 0));
                DECODED = decoded.value();
                encoded = ss.str();
                ASSERTV(LINE, encoded, VALUE, DECODED, VALUE == DECODED);
                // We also use `memcmp` to ensure that we really get back the
                // exact same binary IEEE-754, bit-by-bit.
                ASSERTV(LINE, encoded, VALUE, DECODED,
                        0 == bsl::memcmp(&VALUE, &DECODED, sizeof VALUE));
            }
        }

        if (verbose)
            cout << "Round-trip `float` with `maxFloatPrecision` option"
                 << endl;
        {
            float neg0 = copysignf(0.0f, -1.0f);

            typedef bsl::numeric_limits<float> Limits;

            // We use infinity to indicate that with the specified encoder
            // options the exact same binary numeric value must be parsed
            // back, as `printValue`/`getValue` do not normally allow it as
            // input.
            const float ROUND_TRIPS = Limits::infinity();

            const struct {
                int   d_line;
                float d_value;
                int   d_maxFloatPrecision;
                float d_expected;
            } DATA[] = {
                //LINE   VALUE             PRECISION     EXPECTED
                //----   -----             ---------   ----------------

                { L_,     0.0,             1,            ROUND_TRIPS    },
                { L_,     0.0,             2,            ROUND_TRIPS    },
                { L_,    neg0,             1,            ROUND_TRIPS    },
                { L_,    neg0,             7,            ROUND_TRIPS    },
                { L_,     1.0,             1,            ROUND_TRIPS    },
                { L_,     1.0,             3,            ROUND_TRIPS    },
                { L_,    10.0,             2,            ROUND_TRIPS    },
                { L_,    10.0,             3,            ROUND_TRIPS    },
                { L_,    -1.5,             1,          -2.0f            },
                { L_,    -1.5,             2,            ROUND_TRIPS    },
                { L_,     1.0e-1f,         1,            ROUND_TRIPS    },
                { L_,     0.1234567891f,   1,           0.1f            },
                { L_,     0.1234567891f,   4,           0.1235f         },
                { L_,     0.1234567891f,   9,           0.123456791f    },

                { L_,    10.0f,            1,            ROUND_TRIPS    },
                { L_,    -1.5e1f,          1,          -20.0f           },
                { L_,    -1.23456789e-20f, 1,          -1e-20f          },
                { L_,    -1.23456789e-20f, 2,          -1.2e-20f        },
                { L_,    -1.23456789e-20f, 8,          -1.2345679e-20f  },
                { L_,    -1.23456789e-20f, 9,            ROUND_TRIPS    },
                { L_,     1.23456789e-20f, 1,           1e-20f          },
                { L_,     1.23456789e-20f, 9,           1.23456787e-20f },
            };
            const int NUM_DATA = sizeof DATA / sizeof DATA[0];

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE      = DATA[ti].d_line;
                const float VALUE     = DATA[ti].d_value;
                const int   PRECISION = DATA[ti].d_maxFloatPrecision;
                const float EXPECTED  = DATA[ti].d_expected;

                baljsn::EncoderOptions encoderOptions;
                encoderOptions.setMaxFloatPrecision(PRECISION);

                bsl::stringstream ss;

                s_baltst::SqrtF toEncode;
                toEncode.value() = VALUE;
                baljsn::Encoder encoder;
                ASSERTV(LINE, encoder.loggedMessages(),
                        0 == encoder.encode(ss, toEncode, &encoderOptions));

                s_baltst::SqrtF  decoded;
                decoded.value() = Limits::quiet_NaN(); // A value we don't use
                baljsn::Decoder decoder;
                ASSERTV(LINE, decoder.loggedMessages(),
                        0 == decoder.decode(ss, &decoded, 0));
                float DECODED = decoded.value();
                bsl::string encoded(ss.str());
                if (EXPECTED == ROUND_TRIPS) {
                    ASSERTV(LINE, encoded, VALUE, DECODED, VALUE == DECODED);
                    // We also use `memcmp` to ensure that we really get back
                    // the same binary IEEE-754.
                    ASSERTV(LINE, encoded, VALUE, DECODED,
                            0 == bsl::memcmp(&VALUE, &DECODED, sizeof VALUE));
                }
                else {
                    ASSERTV(LINE, encoded, EXPECTED, DECODED,
                            EXPECTED == DECODED);
                }
            }
        }

        if (verbose) cout << "Round-trip `float` Inf and NaN" << endl;
        {
            roundTripTestNonNumericValues<float>();
        }

        if (verbose) cout << "Round-trip `double`" << endl;
        {
            typedef bsl::numeric_limits<double> Limits;

            double neg0 = copysign(0.0, -1.0);

            const struct {
                int    d_line;
                double d_value;
            } DATA[] = {
                //LINE            VALUE
                //----   ----------------------
                { L_,      0.0                  },
                { L_,      1.0e-1               },
                { L_,      0.125                },
                { L_,      1.0                  },
                { L_,      1.5                  },
                { L_,     10.0                  },
                { L_,      1.5e1                },
                { L_,      9.9e100              },
                { L_,      3.14e300             },
                { L_,      2.23e-308            },
                { L_,      0.12345678912345     },
                { L_,      0.12345678901234567  },
                { L_,      0.123456789012345678 },

                { L_,   neg0                    },
                { L_,     -1.0e-1               },
                { L_,     -0.125                },
                { L_,     -1.0                  },
                { L_,     -1.5                  },
                { L_,    -10.0                  },
                { L_,     -1.5e1                },
                { L_,     -9.9e100              },
                { L_,     -3.14e300             },
                { L_,     -2.23e-308            },
                { L_,     -0.12345678912345     },
                { L_,     -0.12345678901234567  },
                { L_,     -0.123456789012345678 },

                // Small Integers
                { L_,      123456789012345.     },
                { L_,      1234567890123456.    },

                // Full Mantissa Integers
                { L_, 1.0 * 0x1FFFFFFFFFFFFFull },
                { L_, 1.0 * 0x1FFFFFFFFFFFFFull  // This is also limits::max()
                      * (1ull << 63) * (1ull << 63) * (1ull << 63)
                      * (1ull << 63) * (1ull << 63) * (1ull << 63)
                      * (1ull << 63) * (1ull << 63) * (1ull << 63)
                      * (1ull << 63) * (1ull << 63) * (1ull << 63)
                      * (1ull << 63) * (1ull << 63) * (1ull << 63)
                      * (1ull << 26)            },

                // Boundary Values
                { L_,  Limits::min()        },
                { L_,  Limits::denorm_min() },
                { L_,  Limits::max()        },
                { L_, -Limits::min()        },
                { L_, -Limits::denorm_min() },
                { L_, -Limits::max()        },
            };
            const int NUM_DATA = sizeof DATA / sizeof DATA[0];

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE  = DATA[ti].d_line;
                const double VALUE = DATA[ti].d_value;

                s_baltst::Sqrt toEncode;
                toEncode.value() = VALUE;

                // No options (NULL)
                bsl::stringstream ss;
                baljsn::Encoder   encoder;
                ASSERTV(LINE, encoder.loggedMessages(),
                        0 == encoder.encode(ss, toEncode));

                s_baltst::Sqrt  decoded;
                decoded.value() = Limits::quiet_NaN(); // A value we don't use
                baljsn::Decoder decoder;
                ASSERTV(LINE, decoder.loggedMessages(),
                        0 == decoder.decode(ss, &decoded, 0));
                double DECODED = decoded.value();
                bsl::string encoded(ss.str());
                ASSERTV(LINE, encoded, VALUE, DECODED, VALUE == DECODED);
                // We also use `memcmp` to ensure that we really get back the
                // exact same binary IEEE-754, bit-by-bit.
                ASSERTV(LINE, encoded, VALUE, DECODED,
                        0 == bsl::memcmp(&VALUE, &DECODED, sizeof VALUE));

                // Default encoder options, no decoder options (NULL)
                ss.str(""); ss.clear();
                baljsn::EncoderOptions encoderOptions;
                ASSERTV(LINE, encoder.loggedMessages(),
                        0 == encoder.encode(ss, toEncode, &encoderOptions));

                decoded.value() = Limits::quiet_NaN(); // A value we don't use
                ASSERTV(LINE, decoder.loggedMessages(),
                        0 == decoder.decode(ss, &decoded, 0));
                DECODED = decoded.value();
                encoded = ss.str();
                ASSERTV(LINE, encoded, VALUE, DECODED, VALUE == DECODED);
                // We also use `memcmp` to ensure that we really get back the
                // exact same binary IEEE-754, bit-by-bit.
                ASSERTV(LINE, encoded, VALUE, DECODED,
                        0 == bsl::memcmp(&VALUE, &DECODED, sizeof VALUE));

                // Default encoder options, default decoder options
                ss.str(""); ss.clear();
                ASSERTV(LINE, encoder.loggedMessages(),
                        0 == encoder.encode(ss, toEncode, &encoderOptions));

                decoded.value() = Limits::quiet_NaN(); // A value we don't use
                baljsn::DecoderOptions decoderOptions;
                ASSERTV(LINE, decoder.loggedMessages(),
                        0 == decoder.decode(ss, &decoded, &decoderOptions));
                DECODED = decoded.value();
                encoded = ss.str();
                ASSERTV(LINE, encoded, VALUE, DECODED, VALUE == DECODED);
                // We also use `memcmp` to ensure that we really get back the
                // exact same binary IEEE-754, bit-by-bit.
                ASSERTV(LINE, encoded, VALUE, DECODED,
                        0 == bsl::memcmp(&VALUE, &DECODED, sizeof VALUE));

                // Encoder options enable NaN & Inf, default decoder options
                // Should not cause change as we don't test those values here
                ss.str(""); ss.clear();
                encoderOptions.setEncodeInfAndNaNAsStrings(true);
                ASSERTV(LINE, encoder.loggedMessages(),
                        0 == encoder.encode(ss, toEncode, &encoderOptions));

                decoded.value() = Limits::quiet_NaN(); // A value we don't use
                ASSERTV(LINE, decoder.loggedMessages(),
                        0 == decoder.decode(ss, &decoded, &decoderOptions));
                DECODED = decoded.value();
                encoded = ss.str();
                ASSERTV(LINE, encoded, VALUE, DECODED, VALUE == DECODED);
                // We also use `memcmp` to ensure that we really get back the
                // exact same binary IEEE-754, bit-by-bit.
                ASSERTV(LINE, encoded, VALUE, DECODED,
                        0 == bsl::memcmp(&VALUE, &DECODED, sizeof VALUE));

                // Encoder options enable NaN & Inf, no decoder options (NULL)
                // Should not cause change as we don't test those values here
                ss.str(""); ss.clear();
                encoderOptions.setEncodeInfAndNaNAsStrings(true);
                ASSERTV(LINE, encoder.loggedMessages(),
                        0 == encoder.encode(ss, toEncode, &encoderOptions));

                decoded.value() = Limits::quiet_NaN(); // A value we don't use
                ASSERTV(LINE, decoder.loggedMessages(),
                        0 == decoder.decode(ss, &decoded, 0));
                DECODED = decoded.value();
                encoded = ss.str();
                ASSERTV(LINE, encoded, VALUE, DECODED, VALUE == DECODED);
                // We also use `memcmp` to ensure that we really get back the
                // exact same binary IEEE-754, bit-by-bit.
                ASSERTV(LINE, encoded, VALUE, DECODED,
                        0 == bsl::memcmp(&VALUE, &DECODED, sizeof VALUE));

                // Encoder options set `maxFloatPrecision`, no decoder options
                // (NULL).  Since we are encoding `double`, there should be no
                // effect.
                ss.str(""); ss.clear();
                encoderOptions.reset();
                encoderOptions.setMaxFloatPrecision(2);
                ASSERTV(LINE, encoder.loggedMessages(),
                        0 == encoder.encode(ss, toEncode, &encoderOptions));

                decoded.value() = Limits::quiet_NaN(); // A value we don't use
                ASSERTV(LINE, decoder.loggedMessages(),
                        0 == decoder.decode(ss, &decoded, 0));
                DECODED = decoded.value();
                encoded = ss.str();
                ASSERTV(LINE, encoded, VALUE, DECODED, VALUE == DECODED);
                // We also use `memcmp` to ensure that we really get back the
                // exact same binary IEEE-754, bit-by-bit.
                ASSERTV(LINE, encoded, VALUE, DECODED,
                        0 == bsl::memcmp(&VALUE, &DECODED, sizeof VALUE));

                // Encoder options set `maxDoublePrecision`, no decoder options
                // (NULL).  We should get the same number back.
                ss.str(""); ss.clear();
                encoderOptions.reset();
                encoderOptions.setMaxDoublePrecision(0);
                ASSERTV(LINE, encoder.loggedMessages(),
                        0 == encoder.encode(ss, toEncode, &encoderOptions));

                decoded.value() = Limits::quiet_NaN(); // A value we don't use
                ASSERTV(LINE, decoder.loggedMessages(),
                        0 == decoder.decode(ss, &decoded, 0));
                DECODED = decoded.value();
                encoded = ss.str();
                ASSERTV(LINE, encoded, VALUE, DECODED, VALUE == DECODED);
                // We also use `memcmp` to ensure that we really get back the
                // exact same binary IEEE-754, bit-by-bit.
                ASSERTV(LINE, encoded, VALUE, DECODED,
                        0 == bsl::memcmp(&VALUE, &DECODED, sizeof VALUE));

                // Encoder options set `maxDoublePrecision` to maximum
                // necessary for round trip, no decoder options (NULL).  We
                // should get the same number back.
                ss.str(""); ss.clear();
                encoderOptions.reset();
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
                encoderOptions.setMaxDoublePrecision(Limits::max_digits10);
#elif defined(DBL_DECIMAL_DIG)
                encoderOptions.setMaxDoublePrecision(DBL_DECIMAL_DIG);
#else
                encoderOptions.setMaxDoublePrecision(17);
#endif
                ASSERTV(LINE, encoder.loggedMessages(),
                        0 == encoder.encode(ss, toEncode, &encoderOptions));

                decoded.value() = Limits::quiet_NaN(); // A value we don't use
                ASSERTV(LINE, decoder.loggedMessages(),
                        0 == decoder.decode(ss, &decoded, 0));
                DECODED = decoded.value();
                encoded = ss.str();
                ASSERTV(LINE, encoded, VALUE, DECODED, VALUE == DECODED);
                // We also use `memcmp` to ensure that we really get back the
                // exact same binary IEEE-754, bit-by-bit.
                ASSERTV(LINE, encoded, VALUE, DECODED,
                        0 == bsl::memcmp(&VALUE, &DECODED, sizeof VALUE));
            }
        }

        if (verbose)
            cout << "Round-trip `double` with `maxDoublePrecision` option"
                 << endl;
        {
            typedef bsl::numeric_limits<double> Limits;

            double neg0 = copysign(0.0, -1.0);

            // We use infinity to indicate that with the specified encoder
            // options the exact same binary numeric value must be parsed
            // back, as `printValue`/`getValue` do not normally allow it as
            // input.
            const double ROUND_TRIPS = Limits::infinity();

            const struct {
                int    d_line;
                double d_value;
                int    d_maxDoublePrecision;
                double d_expected;
            } DATA[] = {
                //                            PRECISION
                //LINE               VALUE        |         EXPECTED
                //---  -----------------------   -v-  ---------------------

                {L_,                       0.0,   1,             ROUND_TRIPS },
                {L_,                       0.0,   2,             ROUND_TRIPS },
                {L_,                      neg0,   1,             ROUND_TRIPS },
                {L_,                      neg0,  17,             ROUND_TRIPS },
                {L_,                       1.0,   1,             ROUND_TRIPS },
                {L_,                       1.0,   3,             ROUND_TRIPS },
                {L_,                      10.0,   2,             ROUND_TRIPS },
                {L_,                      10.0,   3,             ROUND_TRIPS },
                {L_,                      -1.5,   1,  -2.0                   },
                {L_,                      -1.5,   2,             ROUND_TRIPS },
                {L_,                  -9.9e100,   2,             ROUND_TRIPS },
                {L_,                  -9.9e100,  15,             ROUND_TRIPS },
                {L_,                  -9.9e100,  17,             ROUND_TRIPS },
                {L_,                 -3.14e300,  15,             ROUND_TRIPS },
                {L_,                 -3.14e300,  17,             ROUND_TRIPS },
                {L_,                  3.14e300,   2,   3.1e+300              },
                {L_,                  3.14e300,  17,             ROUND_TRIPS },
                {L_,                    1.0e-1,   1,             ROUND_TRIPS },
                {L_,                 2.23e-308,   2,   2.2e-308              },
                {L_,                 2.23e-308,  17,             ROUND_TRIPS },
                {L_,      0.123456789012345678,   1,   0.1                   },
                {L_,      0.123456789012345678,   2,   0.12                  },
                {L_,      0.123456789012345678,  15,   0.123456789012346     },
                {L_,      0.123456789012345678,  16,   0.1234567890123457    },
                {L_,      0.123456789012345678,  17,             ROUND_TRIPS },

                {L_,                      10.0,   1,   1e+01                 },
                {L_,                    -1.5e1,   1,  -2e+01                 },
                {L_,   -1.2345678901234567e-20,   1,  -1e-20                 },
                {L_,   -1.2345678901234567e-20,   2,  -1.2e-20               },
                {L_,   -1.2345678901234567e-20,  15,  -1.23456789012346e-20  },
                {L_,   -1.2345678901234567e-20,  16,  -1.234567890123457e-20 },
                {L_,   -1.2345678901234567e-20,  17,             ROUND_TRIPS },
            };
            const int NUM_DATA = sizeof DATA / sizeof DATA[0];

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE      = DATA[ti].d_line;
                const double VALUE     = DATA[ti].d_value;
                const int    PRECISION = DATA[ti].d_maxDoublePrecision;
                const double EXPECTED  = DATA[ti].d_expected;

                baljsn::EncoderOptions encoderOptions;
                encoderOptions.setMaxDoublePrecision(PRECISION);

                bsl::stringstream ss;

                s_baltst::Sqrt toEncode;
                toEncode.value() = VALUE;
                baljsn::Encoder encoder;
                ASSERTV(LINE, encoder.loggedMessages(),
                        0 == encoder.encode(ss, toEncode, &encoderOptions));

                s_baltst::Sqrt  decoded;
                decoded.value() = Limits::quiet_NaN(); // A value we don't use
                baljsn::Decoder decoder;
                ASSERTV(LINE, decoder.loggedMessages(),
                        0 == decoder.decode(ss, &decoded, 0));
                double DECODED = decoded.value();
                bsl::string encoded(ss.str());
                if (EXPECTED == ROUND_TRIPS) {
                    ASSERTV(LINE, encoded, VALUE, DECODED, VALUE == DECODED);
                    // We also use `memcmp` to ensure that we really get back
                    // the same binary IEEE-754.
                    ASSERTV(LINE, encoded, VALUE, DECODED,
                            0 == bsl::memcmp(&VALUE, &DECODED, sizeof VALUE));
                }
                else {
                    ASSERTV(LINE, encoded, EXPECTED, DECODED,
                            EXPECTED == DECODED);
                }
            }
        }

        if (verbose) cout << "Round-trip `double` Inf and NaN" << endl;
        {
            roundTripTestNonNumericValues<double>();
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING DECODING VECTORS OF VECTORS
        //   This case tests that arrays of arrays can be decoded into vectors
        //   of vectors
        //
        // Concerns:
        // 1. The decoder can decode nested JSON arrays into nested
        //    `bsl::vector` objects having arbitrary `bdlat`-compatible
        //    elements.
        //
        // Plan:
        // 1. Perform depth-ordered enumeration of all JSON representation of
        //    an array of arrays of integers up to depth 2 and length 2, and
        //    verify that such a string successfully decodes into the
        //    corresponding vector of vector of integers.  Longer arrays
        //    are tested in the next step of the plan.
        //
        // 2. For a large number of different JSON value representations and
        //    compatible `bdlat` element types, enumerate a large number of
        //    different nested and non-nested JSON arrays of these elements and
        //    verify that the JSON array decodes into the corresponding nested
        //    or non-nested `bsl::vector` value.
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING DECODING VECTORS OF VECTORS" << endl
                 << "===================================" << endl;

        if (veryVerbose)
            cout << endl
                 << "Testing decoding vectors of vectors of integers" << endl
                 << "-----------------------------------------------" << endl;

        static const int MAX_SIZE = 4;

        static const bool NO  = false;
        static const bool YES = true;

        const struct {
            int         d_line;
            const char *d_json_p;
            bool        d_isValid;
            int         d_topLevelSize;
            int         d_nestedSizes[MAX_SIZE];
        } DATA[] = {
            //    LINE       IS VALID        TOP LEVEL SIZE
            //   .----       --------.      .--------------
            //  /       JSON          \.   /   NESTED SIZES
            // -- ------------------ ---- -- ----------------
            // Note that this test table stops at a (nested) array length of 2.
            // Longer (nested) arrays are tested below.
            { L_, ""                ,  NO, 0, {            } },
            { L_, "[]"              , YES, 0, {            } },
            { L_, "[[]]"            , YES, 1, { 0          } },
            { L_, "[[0]]"           , YES, 1, { 1          } },
            { L_, "[[0,0]]"         , YES, 1, { 2          } },
            { L_, "[[],[]]"         , YES, 2, { 0, 0       } },
            { L_, "[[],[0]]"        , YES, 2, { 0, 1       } },
            { L_, "[[],[0,0]]"      , YES, 2, { 0, 2       } },
            { L_, "[[0],[]]"        , YES, 2, { 1, 0       } },
            { L_, "[[0],[0]]"       , YES, 2, { 1, 1       } },
            { L_, "[[0],[0,0]]"     , YES, 2, { 1, 2       } },
            { L_, "[[0,0],[]]"      , YES, 2, { 2, 0       } },
            { L_, "[[0,0],[0]]"     , YES, 2, { 2, 1       } },
            { L_, "[[0,0],[0,0]]"   , YES, 2, { 2, 2       } },

            // Test missing brackets.
            { L_, "["               ,  NO, 0, {            } },
            { L_, "[["              ,  NO, 0, {            } },
            { L_, "[[]"             ,  NO, 0, {            } },
            { L_, "]"               ,  NO, 0, {            } },
            { L_, "]]"              ,  NO, 0, {            } },
            { L_, "[]]"             , YES, 0, {            } }, // *
            // * This test vector is accepted because the decoder does not read
            // any tokens after a document is complete.

            // Test incomplete arrays-of-arrays.
            { L_, "["               ,  NO, 0, {            } },
            { L_, "[["              ,  NO, 0, {            } },
            { L_, "[[0"             ,  NO, 0, {            } },
            { L_, "[[0,"            ,  NO, 0, {            } },
            { L_, "[[0,0"           ,  NO, 0, {            } },
            { L_, "[[0,0]"          ,  NO, 0, {            } },
            { L_, "[[0,0]]"         , YES, 1, { 2          } },
            { L_, "[[0,0],]"        ,  NO, 0, {            } },
            { L_, "[[0,0],[]"       ,  NO, 0, {            } },
            { L_, "[[0,0],[0]"      ,  NO, 0, {            } },
            { L_, "[[0,0],[0,]"     ,  NO, 0, {            } },
            { L_, "[[0,0],[0,0]"    ,  NO, 0, {            } },
            { L_, "[[0,0],[0,0]]"   , YES, 2, { 2, 2       } },

            // Test a misplaced commas.
            { L_, "[[0]]"           , YES, 1, { 1          } },
            { L_, "[,[0]]"          ,  NO, 0, {            } },
            { L_, "[[,0]]"          ,  NO, 0, {            } },
            { L_, "[[0,]]"          ,  NO, 0, {            } },
            { L_, "[[0],]"          ,  NO, 0, {            } },
            { L_, "[[0]],"          , YES, 1, { 1          } }, // *
            // * This test vector is accepted because the decoder does not read
            // any tokens after a document is complete.

            // Test misplaced commas in documents with other commas.
            { L_, "[[0,0]]"         , YES, 1, { 2          } },
            { L_, "[,[0,0]]"        ,  NO, 0, {            } },
            { L_, "[[,0,0]]"        ,  NO, 0, {            } },
            { L_, "[[0,,0]]"        , YES, 1, { 2          } }, // *
            { L_, "[[0,,,0]]"       , YES, 1, { 2          } }, // *
            { L_, "[[0,0,]]"        ,  NO, 0, {            } },
            { L_, "[[0,0],]"        ,  NO, 0, {            } },
            { L_, "[[0,0]],"        , YES, 1, { 2          } },
            { L_, "[[0],,[0]]"      , YES, 2, { 1, 1       } }, // *
            { L_, "[[0],,,[0]]"     , YES, 2, { 1, 1       } }, // *
            // * These test vectors illustrate a known issue with the decoder,
            // which is that it accepts consecutive commas between array
            // elements.

            // Test nested arrays where the inner arrays have mis-matched
            // types.
            { L_, "[[0,0]]"         , YES, 1, { 2          } },
            { L_, "[[[],0]]"        ,  NO, 0, {            } },
            { L_, "[[{},0]]"        ,  NO, 0, {            } },
            { L_, "[[\"\",0]]"      ,  NO, 0, {            } },
            { L_, "[[true,0]]"      ,  NO, 0, {            } },
            { L_, "[[false,0]]"     ,  NO, 0, {            } },
            { L_, "[[null,0]]"      ,  NO, 0, {            } },
            { L_, "[[0,[]]]"        ,  NO, 0, {            } },
            { L_, "[[0,{}]]"        ,  NO, 0, {            } },
            { L_, "[[0,\"\"]]"      ,  NO, 0, {            } },
            { L_, "[[0,true]]"      ,  NO, 0, {            } },
            { L_, "[[0,false]]"     ,  NO, 0, {            } },
            { L_, "[[0,null]]"      ,  NO, 0, {            } },
            { L_, "[[0,0],[]]"      , YES, 2, { 2, 0       } },
            { L_, "[[0,0],[[]]]"    ,  NO, 0, {            } },
            { L_, "[[0,0],[[]],[]]" ,  NO, 0, {            } },
        };

        const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        for (int i = 0; i != NUM_DATA; ++i) {
            const int         LINE             = DATA[i].d_line;
            const char *const JSON             = DATA[i].d_json_p;
            const bool        IS_VALID         = DATA[i].d_isValid;
            const int         TOP_LEVEL_SIZE   = DATA[i].d_topLevelSize;

            const int(&NESTED_SIZES)[MAX_SIZE] = DATA[i].d_nestedSizes;

            if (veryVerbose) {
                T_;
                P_(LINE);
                P_(JSON);
                P_(IS_VALID);
                P(TOP_LEVEL_SIZE);
            }

            typedef baljsn::Decoder Obj;
            Obj mX;

            baljsn::DecoderOptions options;

            bdlsb::FixedMemInStreamBuf streamBuffer(JSON, bsl::strlen(JSON));

            typedef bsl::vector<int>       IntVector;
            typedef bsl::vector<IntVector> IntMatrix;

            IntMatrix MATRIX(TOP_LEVEL_SIZE);
            for (int j = 0; j != TOP_LEVEL_SIZE; ++j) {
                MATRIX[j].resize(NESTED_SIZES[j]);
            }

            IntMatrix matrix;

            int rc = mX.decode(&streamBuffer, &matrix, options);
            if (IS_VALID) {
                ASSERTV(LINE, rc, 0 == rc);
                ASSERTV(LINE, MATRIX == matrix);
            }
            else {
                ASSERTV(LINE, rc, 0 != rc);
            }
        }

        if (veryVerbose)
            cout << endl
                 << "Testing decoding vectors of vectors of objects" << endl
                 << "----------------------------------------------" << endl;

        const u::AssertDecodedValueIsEqualFunction TEST;

        namespace ux = baljsn::decoder::u;

        typedef s_baltst::TestAttribute<1, ux::k_ATTRIBUTE_1_NAME>   At1;
        typedef s_baltst::TestAttribute<2, ux::k_ATTRIBUTE_2_NAME>   At2;
        typedef s_baltst::TestAttribute<3, ux::k_ATTRIBUTE_3_NAME>   At3;
        typedef s_baltst::TestEnumerator<1, ux::k_ENUMERATOR_1_NAME> En1;
        typedef s_baltst::TestSelection<1, ux::k_SELECTION_1_NAME>   Se1;

        const At1 at1;
        const At2 at2;
        const At3 at3;
        const En1 en1;
        const Se1 se1;

        const s_baltst::TestPlaceHolder<int>                  ip;
        const s_baltst::GenerateTestArray                     a_;
        const s_baltst::GenerateTestArrayPlaceHolder          ap;
        const s_baltst::GenerateTestChoice                    c_;
        const s_baltst::GenerateTestChoicePlaceHolder         cp;
        const s_baltst::GenerateTestCustomizedType            t_;
        const s_baltst::GenerateTestEnumeration               e_;
        const s_baltst::GenerateTestEnumerationPlaceHolder    ep;
        const s_baltst::GenerateTestNullableValue             n_;
        const s_baltst::GenerateTestSequence                  s_;
        const s_baltst::GenerateTestSequencePlaceHolder       sp;

        static const bool T = true;

#define STR(...) #__VA_ARGS__
        // This macro stringifies its arguments, as a rough approximation to
        // C++11 raw string literals.

        //      LINE                      JSON IS VALID
        //     .----                     .-------------
        //    /         JSON            /     EXPECTED DECODED OBJECT
        //   -- ---------------------- -- ----------------------------------
        TEST(L_, STR({"a1":0})        , T, s_(at1,0)                        );
        TEST(L_, STR({})              , T, s_(at1,a_(ip))                   );
        TEST(L_, STR({"a1":[0]})      , T, s_(at1,a_(0))                    );
        TEST(L_, STR({})              , T, s_(at1,a_(ap(ip)))               );
        TEST(L_, STR({"a1":[]})       , T, s_(at1,a_(ap(ip)))               );
        TEST(L_, STR({"a1":[[]]})     , T, s_(at1,a_(a_(ip)))               );
        TEST(L_, STR({"a1":[[0]]})    , T, s_(at1,a_(a_(0)))                );
        TEST(L_, STR({"a1":[[],[]]})  , T, s_(at1,a_(a_(ip),a_(ip)))        );
#ifndef U_SKIP_DUE_TO_COMPILER_RESOURCE_LIMITATIONS
        TEST(L_, STR({"a1":[[],[0]]}) , T, s_(at1,a_(a_(ip),a_(0)))         );
        TEST(L_, STR({"a1":[[0],[]]}) , T, s_(at1,a_(a_(0),a_(ip)))         );
        TEST(L_, STR({"a1":[[0],[0]]}), T, s_(at1,a_(a_(0),a_(0)))          );
        TEST(L_, STR([])              , T, a_(sp(at1,ip))                   );
        TEST(L_, STR([])              , T, a_(ap(sp(at1,ip)))               );
        TEST(L_, STR([[]])            , T, a_(a_(sp(at1,ip)))               );
        TEST(L_, STR([[],[]])         , T, a_(a_(sp(at1,ip)),a_(sp(at1,ip))));
#endif

        const u::AssertDecodedArrayOfValuesIsEqualFunction TEST2;

#ifndef U_SKIP_DUE_TO_COMPILER_RESOURCE_LIMITATIONS
        TEST2(L_, STR(1)              , 1                 );
        TEST2(L_, STR([1])            , a_(1)             );
        TEST2(L_, STR({"s1":1})       , c_(se1, 1)        );
        TEST2(L_, STR(1)              , t_(1, ip)         );
        TEST2(L_, STR("e1")           , e_(en1, 1)        );
        TEST2(L_, STR(null)           , n_(ip)            );
        TEST2(L_, STR(1)              , n_(1)             );
        TEST2(L_, STR({"a1":1})       , s_(at1, 1)        );
        TEST2(L_, STR({"a1":1,"a2":2}), s_(at1, at2, 1, 2));

        TEST2(L_, STR({"s1":[]})              , c_(se1,a_(ip))              );
        TEST2(L_, STR({"s1":[1]})             , c_(se1,a_(1))               );
        TEST2(L_, STR({"s1":[1,1]})           , c_(se1,a_(1,1))             );
        TEST2(L_, STR({"s1":[1,1,1]})         , c_(se1,a_(1,1,1))           );
        TEST2(L_, STR({})                     , s_(at1,a_(ip))              );
        TEST2(L_, STR({"a1":[1]})             , s_(at1,a_(1))               );
        TEST2(L_, STR({"a1":[1,1]})           , s_(at1,a_(1,1))             );
        TEST2(L_, STR({"a1":[1,1,1]})         , s_(at1,a_(1,1,1))           );
        TEST2(L_, STR({})                     , s_(at1,at2,a_(ip),a_(ip))   );
        TEST2(L_, STR({"a1":[1]})             , s_(at1,at2,a_(1),a_(ip))    );
        TEST2(L_, STR({"a1":[1,1]})           , s_(at1,at2,a_(1,1),a_(ip))  );
        TEST2(L_, STR({"a1":[1,1,1]})         , s_(at1,at2,a_(1,1,1),a_(ip)));
        TEST2(L_, STR({"a2":[1]})             , s_(at1,at2,a_(ip),a_(1))    );
        TEST2(L_, STR({"a1":[1],"a2":[1]})    , s_(at1,at2,a_(1),a_(1))     );
        TEST2(L_, STR({"a1":[1,1],"a2":[1]})  , s_(at1,at2,a_(1,1),a_(1))   );
        TEST2(L_, STR({"a1":[1,1,1],"a2":[1]}), s_(at1,at2,a_(1,1,1),a_(1)) );

        TEST2(L_, STR({"a1":[1,1,1],"a2":[1,1],"a3":[1]}),
                  s_(at1,at2,at3,a_(1,1,1),a_(1,1),a_(1)) );
#endif
#undef STR

      } break;
      case 9: {
        // ------------------------------------------------------------------
        // TESTING UTF-8 DETECTION
        //
        // The `Decoder` type is able to do UTF-8 checking of input, which is
        // enabled or disabled by a field in the `DecoderOptions` object.
        //
        // Concerns:
        // 1. When UTF-8 validation is enabled, the decoder can detect invalid
        //    UTF-8.
        //
        // 2. The message logged when UTF-8 occurs correctly describes the
        //    nature of the UTF-8 error.
        //
        // 3. The message logged contains the offset of the UTF-8 error in the
        //    `streambuf`.
        //
        // 4. The message accurately describes the context in which the
        //    offending invalid UTF-8 sequences occurred (this was especially a
        //    worry, because the tokenizer reads data 8K at a time, and we
        //    didn't want to be giving decoder messages describing a context at
        //    the start of the buffer when in fact the invalid UTF-8 was
        //    several kilobytes later).
        //
        // Plan:
        // 1. Start out with `pattern`, a valid UTF-8 string of JSON input for
        //    an object of type 'test::Employee".
        //
        // 2. Create a table `UTF8_DATA`, where every row of the table has
        //    either `Tokenizer::k_EOF`, or an `enum` from
        //    `bdlde::Utf8Util::ErrorStatus` indicating a type of invalid
        //    UTF-8, and a string with a sequence of that type of bad UTF-8.
        //
        // 3. Create a table `FIND_DATA` consisting of strings to search for,
        //    in sequence, through `pattern`, and for each string, some
        //    context-appropriate strings (separated by '\t') that we expect to
        //    be logged if an error were to be encountered at that point in
        //    `pattern`.
        //
        // 4. Iterate through `FIND_DATA` forwarding to each point in `pattern`
        //    identified by the string in `FIND_DATA`.  Each field in the
        //    `FIND_DATA` table is visited twice in a row, the first time with
        //    the boolean `AFTER` `false`, meaning that the good UTF-8 is
        //    truncated before the string from `FIND_DATA`, and then the next
        //    iteration with `AFTER` `true`, in which case the good UTF-8 is
        //    trucated immediately after the found string.  Note that we only
        //    search for the tab-separated strings in the case where `AFTER`
        //    was `false`.
        //
        // 5. In a nested loop, iterate through `UTF-8` data
        //    - splice the invalid UTF-8 sequence right at the point indicated
        //      by `FIND_DATA` into the string
        //
        //    - attempt to decode the string
        //
        //    - observe that this failed, and that the message logged by the
        //      decoder includes
        //     1. the appropriate message describing the type of UTF-8 error or
        //        end of file
        //
        //     2. the offset at which the problem occurs
        //
        //     3. a description of the context appropriate to the location of
        //        the problematic input in `pattern`.
        //
        // 6. In the last iteration, the bool `FINAL` is set, and the
        //    problematic input (or EOF) is after the closing '}' of input, and
        //    we observe that there was no error, and no error was logged.
        // ------------------------------------------------------------------

        const char *pattern =
                "{\n"
                "       \"name\": \"Bob \xca\xbf\",\n"
                "       \"homeAddress\": {\n"
                "           \"street\": \"Some Street \xf1\x80\x80\x80\",\n"
                "           \"city\": \"Some City \xe0\xa0\xbf\",\n"
                "           \"state\": \"Some State \xcf\x80\"\n"
                "       },\n"
                "       \"age\": 21\n"
                "}";

        typedef bdlde::Utf8Util Util;

        const bsl::size_t npos = bsl::string::npos;

        static const struct Utf8Data {
            int                d_line;
            int                d_status;
            const char        *d_sequence;
        } UTF8_DATA[] = {
            { L_, baljsn::Tokenizer::k_EOF,              ""                 },
            { L_, Util::k_END_OF_INPUT_TRUNCATION,       "\xc7"             },
            { L_, Util::k_UNEXPECTED_CONTINUATION_OCTET, "\xa4"             },
            { L_, Util::k_NON_CONTINUATION_OCTET,        "\xc7?"            },
            { L_, Util::k_OVERLONG_ENCODING,             "\xc0\x83"         },
            { L_, Util::k_INVALID_INITIAL_OCTET,         "\xfa\0x80"        },
            { L_, Util::k_VALUE_LARGER_THAN_0X10FFFF,    "\xf4\xa0\x80\x80" },
            { L_, Util::k_SURROGATE,                     "\xed\xb0\x85"     }
        };
        enum { k_NUM_UTF8_DATA = sizeof UTF8_DATA /sizeof UTF8_DATA[0] };

        static const struct FindData {
            int         d_line;
            const char *d_findStr;
            const char *d_expStrings;
        } FIND_DATA[] = {
            { L_, "{"   , "advancing to the first token\t"
                                                    "Expecting a '{' or '['" },
            { L_, " "   , "reading token after '{'" },
            { L_, "\""  , "reading token after '{'" },
            { L_, "name", "reading token after '{'" },
            { L_, ":"   , "reading value for attribute 'name'" },
            { L_, "o"   , "reading value for attribute 'name'" },
            { L_, ","   , "reading token after value for attribute 'name'" },
            { L_, "A"   , "reading token after value for attribute 'name'" },
            { L_, "{"   , "reading value for attribute 'homeAddress'" },
            { L_, "ee"  , "reading token after '{'\t"
                         "Could not decode sequence\terror decoding element" },
            { L_, "Str" , "reading value for attribute 'street'\t"
                         "Could not decode sequence\terror decoding element" },
            { L_, ":"   , "reading value for attribute 'city'\t"
                         "Could not decode sequence\terror decoding element" },
            { L_, " "   , "reading value for attribute 'city'\t"
                         "Could not decode sequence\terror decoding element" },
            { L_, "C"   , "reading value for attribute 'city'\t"
                         "Could not decode sequence\terror decoding element" },
            { L_, "}"   , "reading token after value for attribute 'state'\t"
                         "Could not decode sequence\terror decoding element" },
            { L_, ","   , "reading token after value for attribute 'state'"},
            { L_, "2"   , "reading value for attribute 'age'"},
            { L_, "}"   , "reading token after value for attribute 'age'" } };
        enum { k_NUM_FIND_DATA = sizeof FIND_DATA /sizeof FIND_DATA[0] };

        bool done = false;
        const char *pf = pattern;
        for (int tf = 0; tf < 2 * k_NUM_FIND_DATA; ++tf) {
            const FindData&   findData = FIND_DATA[tf / 2];
            const int         FLINE    = findData.d_line;
            const char       *FIND_STR = findData.d_findStr;
            const char       *EXP_STRS = findData.d_expStrings;
            const bool        AFTER    = tf & 1;
            const bool        FINAL    = 2 * k_NUM_FIND_DATA-1 == tf;

            done |= FINAL;

            pf = AFTER ? pf + bsl::strlen(FIND_STR)
                       : bsl::strstr(pf, FIND_STR);
            ASSERT(pf);
            ASSERT(!FINAL == !!*pf);

            for (int tu = 0; tu < k_NUM_UTF8_DATA; ++tu) {
                const Utf8Data& utf8Data = UTF8_DATA[tu];
                const int       ULINE    = utf8Data.d_line;
                const int       UERR     = utf8Data.d_status;
                const char     *USEQ     = utf8Data.d_sequence;
                const char     *UMSG     = baljsn::Tokenizer::k_EOF == UERR
                                         ? "Error: unexpected end of file"
                                         : Util::toAscii(UERR);

                bsl::string str(pattern, pf - pattern);
                str += USEQ;
                if (baljsn::Tokenizer::k_EOF != UERR &&
                                     Util::k_END_OF_INPUT_TRUNCATION != UERR) {
                    str += pf;
                }

                bsl::istringstream iss(str);

                test::Employee bob;

                baljsn::DecoderOptions options;
                options.setValidateInputIsUtf8(true);
                baljsn::Decoder        decoder;
                ASSERTV(FIND_STR, USEQ, str, FINAL ==
                                    (0 == decoder.decode(iss, &bob, options)));
                const bsl::string& logMsg = decoder.loggedMessages();

                if (FINAL) {
                    ASSERT(logMsg.empty());
                }
                else {
                    ASSERTV(ULINE, UMSG, logMsg, npos != logMsg.find(UMSG));
                }

                if (veryVerbose) {
                    P_(str);    P(logMsg);
                }

                if (!AFTER) {
                    const char *pEnd = "";
                    for (const char *pe = EXP_STRS; pe; pe = *pEnd ? pEnd+1 :
                                                                           0) {
                        pEnd = bsl::strstr(pe, "\t");
                        pEnd = pEnd ? pEnd : pe + bsl::strlen(pe);
                        bsl::string exp(pe, pEnd - pe);

                        ASSERTV(FLINE, FIND_STR, str, logMsg, exp,
                                                     npos != logMsg.find(exp));
                    }
                }
            }
        }
        ASSERT(done);
      } break;
      case 8: {
        // ------------------------------------------------------------------
        // TESTING CLEARING OF LOGGED MESSAGES ON DECODE CALLS
        //   This case tests that the log buffer is reset on each call to
        //   `decode`.
        //
        // Concerns:
        // 1. The string returned from `loggedMessages` resets each time
        //    `decode` is invoked, such that the contents of the logged
        //    messages refer to only the most recent invocation of `decode`.
        //
        // 2. That when the input contains no invalid UTF-8, enabling or
        //    disabling UTF-8 checking has no influence on behavior.
        //
        // Plan:
        // 1. Define a type, `SOType`, that `baljsn::Decoder` is able to
        //    successfully decode into given well-formed input.
        //
        // 2. Define a type, `FOType`, that `baljsn::Decoder` is unable to
        //    successfully decode into given any input.
        //
        // 3. Create a string that successfully decodes into a `SOType`.
        //
        // 4. Create a string that does not decode into a `SOType`.
        //
        // 5. Create a string that does not decode into a `FOType`.  Note that
        //    this can be any string, because `FOType` can never be
        //    successfully decoded-into.
        //
        // 6. Verify that, after performing various sequences of decoding
        //    operations using combinations of the above types and strings,
        //    where some/all succeed/fail, the `loggedMessages` are empty if
        //    the last operation succeeds, and contain an expected message
        //    if and only if the last operation fails.
        //
        // 7. Have a boolean `UTF8` variable that enables / disables UTF-8
        //    checking, and run ALL of the tests in the case with it `true` and
        //    `false` and observe that this has no influence on test outcomes.
        //
        // Testing:
        //   int decode(bsl::streambuf *streamBuf, TYPE *y, options);
        // ------------------------------------------------------------------

        if (verbose) cout
            << "\nTESTING CLEARING OF LOGGED MESSAGES ON DECODE CALLS"
            << "\n===================================================" << endl;

        /// `SOType` is the type of an object for which
        /// `baljsn::Decoder::decode` will succeed for some input.
        typedef test::Address SOType;

        /// `FOType` is the type of an object or which
        /// `baljsn::Decoder::deocde` will fail for all input.
        typedef test::Enumeration0 FOType;

        // `SOSStr` is a string that will successfully decode into an
        // object of `SOType`.
        static const char SOSStr[] =
                "{"
                "    \"street\": \"1st\","
                "    \"city\": \"New York\","
                "    \"state\": \"New York\""
                "}";

        // `SOFStr` is a string that will fail to decode into an object of
        // `SOType`.
        static const char SOFStr[] =
                "\"Address\": {}";

        // `FOFStr` is a string that will fail to decode into an object of
        // `FOType`.
        static const char FOFStr[] =
                "\"zero\"";

        // `SMsg` is a string that is equivalent to the `loggedMessages` of
        // a `baljsn::Decoder` after a successful decoding operation.
        static const char SMsg[] = "";

        // `FMsg1` is a string that is equivalent to the `loggedMessages`
        // of a `baljsn::Decoder` after an decoding operation that fails
        // due to the type of the target object having an unsupported
        // `bdlat` category.
        static const char FMsg1[] =
                "The object being decoded must be a Sequence, Choice,"
                " or Array type\n";

        // `FMsg2` is a string that is equivalent to the `loggedMessages`
        // of the `baljsn::Decoder` after a decoding operation that fails
        // due to the contents of the JSON string not being a valid
        // representation of an object of the target type.
        static const char FMsg2[] =
                "Error advancing to the first token. Expecting a '{' or '['"
                " as the first character\n";

        enum ObjId {
            // This enumeration provides a set of integer constants that serve
            // as identifiers for different target objects for decoding.

            NONE = 0, // indicates to decode into no object (to not decode)
            SOId,     // indicates to decode into an object of `SOType`
            FOId      // indicates to decode into an object of `FOType`
        };

        /// This `struct` provides a pair that defines the input to a
        /// decoding operation: the object to decode into, and the string
        /// containing the encoded JSON.
        struct Instruction {

            /// Id of an object to decode into
            ObjId       d_objId;

            /// the JSON to decode from
            const char *d_string;
        };

        enum {
            k_MAX_INSTRUCTIONS = 3  // maximum number of instructions that may
                                    // be performed in one row of the table
                                    // that drives the testing apparatus
        };

        enum SuccessStatus {
            // This enumeration provides a set of enumerators used to identity
            // whether or not a decoding operation completed successfully.

            failure,
            success
        };

        /// line number
        static const struct {
            int           d_line;

            /// instructions for test apparatus
            Instruction   d_instructions[k_MAX_INSTRUCTIONS];

            /// whether all operations succeed
            SuccessStatus d_decodeSuccessStatus;

            /// messages from final operation
            const char   *d_loggedMessages;

        } DATA[] = {
            //                                         LOGGED MESSAGES
            //                                         ---------------.
            //                           DECODE SUCCESS STATUS        |
            //                           ---------------------.       |
            //LINE             INSTRUCTIONS                   |       |
            //---- --------------------------------------- -------- ------
            {   L_, {                                    }, success, SMsg  },
                // Verify that the `loggedMessages` are empty if no decoding
                // operations are performed.

            {   L_, { {SOId, SOSStr}                     }, success, SMsg  },
                // Verify that the `loggedMessages` are empty if one encoding
                // operation is performed, and that operation succeeds.

            {   L_, { {SOId, SOFStr}                     }, failure, FMsg2 },
            {   L_, { {FOId, FOFStr}                     }, failure, FMsg1 },
                // Verify that the `loggedMessages` have an expected error
                // message if one encoding operation is performed, and that
                // operation fails.

            {   L_, { {SOId, SOSStr}, {SOId, SOSStr}     }, success, SMsg  },
            {   L_, { {SOId, SOSStr}, {SOId, SOFStr}     }, failure, FMsg2 },
            {   L_, { {SOId, SOSStr}, {FOId, FOFStr}     }, failure, FMsg1 },
            {   L_, { {SOId, SOFStr}, {SOId, SOSStr}     }, failure, SMsg  },
            {   L_, { {SOId, SOFStr}, {SOId, SOFStr}     }, failure, FMsg2 },
            {   L_, { {SOId, SOFStr}, {FOId, FOFStr}     }, failure, FMsg1 },
            {   L_, { {FOId, FOFStr}, {SOId, SOSStr}     }, failure, SMsg  },
            {   L_, { {FOId, FOFStr}, {SOId, SOFStr}     }, failure, FMsg2 },
            {   L_, { {FOId, FOFStr}, {FOId, FOFStr}     }, failure, FMsg1 },
                // Verify that the `loggedMessages` have an expected message
                // when, after performing 2 decoding operations, the second
                // operation fails, and otherwise are empty.
        };

        const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        for (int ti = 0; ti < 2 * NUM_DATA; ++ti) {
            const int                tj           = ti / 2;
            const bool               UTF8         = ti & 1;
            const int                LINE         = DATA[tj].d_line;
            const Instruction *const INSTRUCTIONS = DATA[tj].d_instructions;
            const SuccessStatus      DECODE_SUCCESS_STATUS =
                                                    DATA[tj].
                                                         d_decodeSuccessStatus;
            const bsl::string_view   LOGGED_MESSAGES =
                                                    DATA[tj].d_loggedMessages;

            typedef baljsn::Decoder Obj;
            baljsn::DecoderOptions options;
            options.setValidateInputIsUtf8(UTF8);
            Obj mX;

            int decodeStatus = 0;

            for (const Instruction *instructionPtr = INSTRUCTIONS;
                 instructionPtr != INSTRUCTIONS + k_MAX_INSTRUCTIONS;
                 ++instructionPtr) {
                switch (instructionPtr->d_objId) {
                  case NONE: {
                      // do nothing, in just the right way
                  } break;
                  case SOId: {
                      const bsl::string_view string(instructionPtr->d_string);
                      bdlsb::FixedMemInStreamBuf streamBuf(string.data(),
                                                           string.length());

                      SOType output;
                      decodeStatus |= mX.decode(&streamBuf, &output, options);
                  } break;
                  case FOId: {
                      const bsl::string_view string(instructionPtr->d_string);
                      bdlsb::FixedMemInStreamBuf streamBuf(string.data(),
                                                           string.length());

                      FOType output;
                      decodeStatus |= mX.decode(&streamBuf, &output, options);
                  } break;
                }
            }

            ASSERTV(LINE,
                    decodeStatus,
                    DECODE_SUCCESS_STATUS ? decodeStatus == 0
                                          : decodeStatus != 0);

            ASSERTV(LINE,
                    LOGGED_MESSAGES,
                    mX.loggedMessages(),
                    LOGGED_MESSAGES == mX.loggedMessages());
        }
      } break;
      case 7: {
        // ------------------------------------------------------------------
        // TESTING DECODING OF ENUM TYPES WITH ESCAPED CHARS
        //
        // Concerns:
        // 1. Decoding of an enum type with escaped characters works as
        //    expected.
        //
        // Plan:
        // 1. Using the table-driven technique, specify a table with JSON text
        //    that contains an enum string with escaped chars.
        //
        // 2. For each row in the tables of P-1:
        //
        //   1. Create a `baljsn_Decoder` object.
        //
        //   2. Create a `bsl::istringstream` object with the JSON text.
        //
        //   3. Decode that JSON into a `test::Palette` object.
        //
        //   4. Verify that the return code from `decode` and the decoded
        //      object is as expected.
        //
        // Testing:
        // ------------------------------------------------------------------

        if (verbose) cout
            << "\nTESTING DECODING OF ENUM TYPES WITH ESCAPED CHARS"
            << "\n=================================================" << endl;

        using bsl::string;

        const char *S1 = "RED/GREEN";
        const char *S2 = "GREY\\BLUE";
        const char *S3 = "WHITE'BLACK";
        const char *S4 = "BLUE\"YELLOW";
        const char *S5 = "RED\b\r\t\f\n";

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_text_p;   // text string
            const char *d_output_p; // json output
        } DATA[] = {
            // line  text   output
            // ----  ----   ----
            {   L_,   S1,   "{ \"color\": \"RED\\/GREEN\" }"    },
                            // validate decoding unescaped solidus
            {   L_,   S1,   "{ \"color\": \"RED/GREEN\" }"      },
            {   L_,   S2,   "{ \"color\": \"GREY\\\\BLUE\" }"   },
            {   L_,   S3,   "{ \"color\": \"WHITE\'BLACK\" }"   },
            {   L_,   S4,   "{ \"color\": \"BLUE\\\"YELLOW\" }" },
            {   L_,   S5,   "{ \"color\": \"RED\b\r\t\f\n\" }"  }
        };
        const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        for (int ti = 0; ti < 2 * NUM_DATA; ++ti) {
            const int          tj     = ti / 2;
            const bool         UTF8   = ti & 1;
            const int          LINE   = DATA[tj].d_lineNum;
            const bsl::string& TEXT   = DATA[tj].d_text_p;
            const bsl::string& OUTPUT = DATA[tj].d_output_p;

            test::Palette obj;
            bsl::istringstream iss(OUTPUT);

            baljsn::DecoderOptions options;
            options.setValidateInputIsUtf8(UTF8);
            baljsn::Decoder        decoder;
            const int rc = decoder.decode(iss, &obj, options);
            ASSERTV(LINE, rc, 0 == rc);
            ASSERTV(LINE, TEXT,
                    TEXT == bsl::string(test::Colors::toString(obj.color())));
            if (veryVerbose) {
                P(decoder.loggedMessages());
                P(obj);
            }
        }

        for (int tu = 0; tu < 2; ++tu) {
            const bool UTF8 = tu;

            const bsl::string& ALL_OUTPUT   =
                "{ \"colors\": [\n"
                                   "\"RED\\/GREEN\",    \n"
                                   // validate decoding unescaped solidus
                                   "\"RED/GREEN\",      \n"
                                   "\"GREY\\\\BLUE\",   \n"
                                   "\"WHITE\'BLACK\",   \n"
                                   "\"BLUE\\\"YELLOW\", \n"
                                   "\"RED\b\r\t\f\n\"   \n"
                               "]\n"
                 "}";

            test::Palette obj;

            bsl::istringstream iss(ALL_OUTPUT);

            baljsn::DecoderOptions options;
            options.setValidateInputIsUtf8(UTF8);
            baljsn::Decoder        decoder;
            const int rc = decoder.decode(iss, &obj, options);
            ASSERTV(rc, 0 == rc);
            const bsl::vector<test::Colors::Value>& V = obj.colors();

            ASSERTV(6  == V.size());
            ASSERTV(string(S1) == string(test::Colors::toString(V[0])));
            ASSERTV(string(S1) == string(test::Colors::toString(V[1])));
            ASSERTV(string(S2) == string(test::Colors::toString(V[2])));
            ASSERTV(string(S3) == string(test::Colors::toString(V[3])));
            ASSERTV(string(S4) == string(test::Colors::toString(V[4])));
            ASSERTV(string(S5) == string(test::Colors::toString(V[5])));

            if (veryVerbose) {
                P(decoder.loggedMessages());
                P(obj);
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING DECODING OF `hexBinary` CUSTOMIZED TYPE
        //
        // Concerns:
        // 1. Decoding of a customized type of type `hexBinary` works as
        //    expected.
        //
        // Plan:
        // 1. Using the table-driven technique, specify a table with JSON text
        //    that contains a `hexBinary` customized type element.
        //
        // 2. For each row in the tables of P-1:
        //
        //   1. Construct a `HexBinarySequence` object.
        //
        //   2. Create a `baljsn::Decoder` object.
        //
        //   3. Create a `bsl::istringstream` object with the JSON text.
        //
        //   4. Decode that JSON into a `HexBinarySequence` object.
        //
        //   5. Verify that the return code from `decode` is as expected.
        //
        // Testing:
        //   DRQS 43702912
        // --------------------------------------------------------------------

        if (verbose) cout
               << "\nTESTING DECODING OF `hexBinary` CUSTOMIZED TYPE"
               << "\n===============================================" << endl;

        static const struct {
            int         d_line;      // source line number
            const char *d_input_p;   // input
            const char *d_output_p;  // output
            int         d_outputLen; // output length
            bool        d_isValid;   // isValid flag
            } DATA[] = {
          {  L_,  "\"\"",            "",                   0, true  },

          {  L_,  "\"Ug==\"",        "R",                  1, true  },
          {  L_,  "\"QVY=\"",        "AV",                 2, true  },

          {  L_,  "\"AA==\"",        "\x00",               1, true  },
          {  L_,  "\"AQ==\"",        "\x01",               1, true  },
          {  L_,  "\"\\/w==\"",      "\xFF",               1, true  },

          {  L_,  "\"UVE=\"",        "QQ",                 2, true  },

          {  L_,  "\"YQ==\"",        "a",                  1, true  },
          {  L_,  "\"YWI=\"",        "ab",                 2, true  },
          {  L_,  "\"YWJj\"",        "abc",                3, true  },
          {  L_,  "\"YWJjZA==\"",    "abcd",               4, true  },

          {  L_,  "\"Qmxvb21iZXJnTFA=\"", "BloombergLP",  11, true  },

          {  L_,     "",               "",                   0, false },
          {  L_,     "\"Q\"",          "",                   0, false },
          {  L_,     "\"QV\"",         "",                   0, false },
          {  L_,     "\"QVE\"",        "",                   0, false },
          {  L_,     "\"QVE==\"",      "",                   0, false },
            };
        const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        for (int ti = 0; ti < 2 * NUM_DATA; ++ti) {
            const int   tj          = ti / 2;
            const bool  UTF8        = ti & 1;
            const int   LINE        = DATA[tj].d_line;
            const char *INPUT       = DATA[tj].d_input_p;
            const char *OUTPUT      = DATA[tj].d_output_p;
            const int   LEN         = DATA[tj].d_outputLen;
            const bool  IS_VALID    = DATA[tj].d_isValid;

            bsl::vector<char> vc(OUTPUT, OUTPUT + LEN);
            const bsl::vector<char>& VC = vc;

            test::HexBinarySequence exp;
            const test::HexBinarySequence& EXP = exp;
            exp.element1().array() = VC;

            test::HexBinarySequence value;

            bsl::ostringstream os;
            os << "{ \"element1\": " << INPUT << '}';

            bsl::istringstream is(os.str());

            baljsn::DecoderOptions options;
            options.setValidateInputIsUtf8(UTF8);
            baljsn::Decoder        decoder;
            const int rc = decoder.decode(is, &value, options);
            if (veryVerbose) {
                P(decoder.loggedMessages());
            }

            if (IS_VALID) {
                LOOP2_ASSERT(LINE, rc, 0 == rc);

                bool result = EXP == value;
                LOOP3_ASSERT(LINE, EXP, value, result);
                if (!result) {
                    cout << "EXP: ";
                    bdlb::PrintMethods::print(cout, EXP, 0, -1);
                    cout << endl << "VALUE: ";
                    bdlb::PrintMethods::print(cout, value, 0, -1);
                    cout << endl;
                }

                {
                    bsl::istringstream is(os.str());
                    test::HexBinarySequence value2;
                    ASSERT(decoder.decodeAny(is, &value2, options) == 0);
                }
            }
            else {
                LOOP2_ASSERT(LINE, rc, rc);
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING DECODING FROM MULTIPLE DECODERS IN SEPARATE THREADS
        //
        // Concerns:
        // 1. Multiple decoders in separate threads decode correctly.
        //
        // Plan:
        // 1. Create multiple threads.
        //
        // 2. Using the table-driven technique, specify three tables: one with
        //    a set of distinct rows of XML string value corresponding to a
        //    `balb::FeatureTestMessage` object, the second with JSON in pretty
        //    format, and third with the JSON in the compact format.
        //
        // 3. For each thread created, for each row in the tables of P-2:
        //
        //   1. Construct a `balb::FeatureTestMessage` object from the XML
        //      string using the XML decoder.
        //
        //   2. Create a `baljsn::Decoder` object.
        //
        //   3. Create a `bdlsb::FixedMemInStreamBuf` object with the pretty
        //      JSON text.
        //
        //   4. Decode that JSON into a `balb::FeatureTestMessage` object.
        //
        //   5. Verify that the decoded object matches the original object
        //      from step 1.
        //
        //   6. Repeat steps 1 - 5 using JSON in the compact format.
        //
        // Testing:
        //   DRQS 41660550
        // --------------------------------------------------------------------

        if (verbose) cout
             << "\nTESTING DECODING FROM MULTIPLE DECODERS IN SEPARATE THREADS"
             << "\n==========================================================="
             << endl;

        using namespace CASE5;

        for (int tu = 0; tu < 2; ++tu) {
            bool checkUtf8 = tu;

            bsl::vector<balb::FeatureTestMessage> testObjects;
            constructFeatureTestMessage(&testObjects);

            ThreadData threadData;
            threadData.d_testObjects_p = &testObjects;
            threadData.d_veryVerbose   = veryVerbose;
            threadData.d_checkUtf8     = checkUtf8;

            const int NUM_THREADS = 20;
            bslmt::ThreadUtil::Handle handles[NUM_THREADS];

            for (int i = 0; i < NUM_THREADS; ++i) {
                ASSERT(0 == bslmt::ThreadUtil::create(&handles[i],
                                                     &threadFunction,
                                                     &threadData));
            }

            for (int i = 0; i < NUM_THREADS; ++i) {
                ASSERT(0 == bslmt::ThreadUtil::join(handles[i]));
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING INVALID JSON RETURNS AN ERROR
        //
        // Concerns:
        // 1. The decoder returns an error on encountering invalid JSON text.
        //
        // Plan:
        // 1. Using the table-driven technique, specify a table with JSON text.
        //
        // 2. For each row in the tables of P-1:
        //
        //   1. Construct a test object.
        //
        //   2. Create a `baljsn::Decoder` object.
        //
        //   3. Create a `bsl::istringstream` object with the JSON text.
        //
        //   4. Decode that JSON into a test object.
        //
        //   5. Verify that the return code from `decode` is *not* 0.
        //
        // Testing:
        //   int decode(bsl::streambuf *streamBuf, TYPE *v, options);
        //   int decode(bsl::istream& stream, TYPE *v, options);
        //   int decode(bsl::streambuf *streamBuf, TYPE *v, &options);
        //   int decode(bsl::istream& stream, TYPE *v, &options);
        //   bsl::string loggedMessages() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\n" "TESTING INVALID JSON RETURNS AN ERROR"
                             "\n" "=====================================" "\n";

        // Testing first character
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_text_p;   // json text
            } DATA[] = {
                //line   input
                //----   -----
                {   L_,   "]   {}"    },
                {   L_,   "}   {}"    },
                {   L_,   "\"  {}"    },
                {   L_,   ",   {}"    },
                {   L_,   ":   {}"    },
                {   L_,   "1   {}"    },
                {   L_,   "*   {}"    },
                {   L_,   "A   {}"    },
                {   L_,   "{\"elem\":[\\}}}}\n" },  // `{DRQS 162368278}`
            };
            const int NUM_DATA = sizeof DATA / sizeof DATA[0];

            for (int ti = 0; ti < 2 * NUM_DATA; ++ti) {
                const int           tj    = ti / 2;
                const bool          UTF8  = ti & 1;
                const int           LINE  = DATA[tj].d_lineNum;
                const bsl::string&  INPUT = DATA[tj].d_text_p;
                balb::SimpleRequest value;

                bsl::istringstream iss(INPUT);

                baljsn::DecoderOptions options;
                if (UTF8) {
                    options.setValidateInputIsUtf8(true);
                }
                baljsn::Decoder        decoder;

                const baljsn::DecoderOptions& mO = options;
                const int RC = decoder.decode(iss, &value, mO);
                ASSERTV(LINE, RC, 0 != RC);
                if (veryVerbose) {
                    P(decoder.loggedMessages());
                }

                {
                    bsl::istringstream iss(INPUT);
                    ASSERT(decoder.decodeAny(iss, &value, mO) != 0);
                }
            }
        }

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_text_p;   // json text
        } DATA[] = {
        //line    input
        //-----   -----

        // invalid token after first character
        {    L_,  "{{   }"   },
        {    L_,  "{[   }"   },
        {    L_,  "{]   }"   },
        {    L_,  "{,   }"   },
        {    L_,  "{:   }"   },
        {    L_,  "{1   }"   },
        {    L_,  "{*   }"   },
        {    L_,  "{A   }"   },

        // invalid name name
        {    L_,  "{\"                 }"              },
        {    L_,  "{\"\"               }"              },
        {    L_,  "{ \" { \"           }"              },
        {    L_,  "{ \" } \"           }"              },
        {    L_,  "{ \" [ \"           }"              },
        {    L_,  "{ \" ] \"           }"              },
        {    L_,  "{ \" , \"           }"              },
        {    L_,  "{ \": \"           }"              },
        {    L_,  "{" "\"id_num\"      }"              },
        {    L_,  "{" "\"id\"          }"              },
        {    L_,  "{" "  12345         }"              },

        // invalid token after object's element name
        {
            L_,
            "{"
              "\"name\""
                       "{"
            "}"
        },
        {
            L_,
            "{"
              "\"name\""
                       "}"
            "}"
        },
        {
            L_,
            "{"
              "\"name\""
                       "["
            "}"
        },
        {
            L_,
            "{"
              "\"name\""
                       "]"
            "}"
        },
        {
            L_,
            "{"
              "\"name\""
                       ","
            "}"
        },
        {
            L_,
            "{"
              "\"name\""
                       "       "
                               "\"another string\""
        },

        // invalid element value
        {
            L_,
            "{"
              "\"name\""
                       ":"
                         ":"
            "}"
        },
        {
            L_,
            "{"
              "\"name\""
                       ":"
                         ","
            "}"
        },
        {
            L_,
            "{"
              "\"name\""
                       ":"
                         "{"
            "}"
        },
        {
            L_,
            "{"
              "\"name\""
                       ":"
                         "}"
            "}"
        },
        {
            L_,
            "{"
              "\"name\""
                       ":"
                         "["
            "}"
        },
        {
            L_,
            "{"
              "\"name\""
                       ":"
                         "]"
            "}"
        },
        {
            L_,
            "{"
              "\"name\""
                       ":"
                         "12345"
            "}"
        },
        {
            L_,
            "{"
              "\"name\""
                       ":"
                         "12.345"
            "}"
        },
        {
            L_,
            "{"
              "\"age\""
                       ":"
                         "\"Bob\""
            "}"
        },
        {
            L_,
            "{"
              "\"age\""
                       ":"
                         "\"Bob\""
            "}"
        },
        {
            L_,
            "{"
              "\"fullname\""
                           ":"
                             "\"Bob\""
            "}"
        },
        {
            L_,
            "{"
              "\"fullname\""
                           ":"
                             "12345"
            "}"
        },
        {
            L_,
            "{"
              "\"fullname\""
                           ":"
                             "["
                             "]"
            "}"
        },
        {
            L_,
            "{"
              "\"fullname\""
                           ":"
                             "["
                               "1"
                             "]"
            "}"
        },
        {
            L_,
            "{"
              "\"ids\""
                      ":"
                        "\"Bob\""
            "}"
        },
        {
            L_,
            "{"
              "\"ids\""
                      ":"
                        "12345"
            "}"
        },
        {
            L_,
            "{"
              "\"ids\""
                      ":"
                        "{"
                        "}"
            "}"
        },
        {
            L_,
            "{"
              "\"ids\""
                      ":"
                        "{"
                          "1"
                        "}"
            "}"
        },

        // invalid token after element value
        {
            L_,
            "{"
              "\"name\""
                       ":"
                         "\"Pete\""
                                  "{"
            "}"
        },
        {
            L_,
            "{"
              "\"name\""
                       ":"
                         "\"Pete\""
                                  ":"
            "}"
        },
        {
            L_,
            "{"
              "\"name\""
                       ":"
                         "\"Pete\""
                                  "["
            "}"
        },
        {
            L_,
            "{"
              "\"name\""
                       ":"
                         "\"Pete\""
                                  "]"
            "}"
        },
        {
            L_,
            "{"
              "\"name\""
                       ":"
                         "\"Pete\""
                                  "   "
                                      "\"invalid string\""
            "}"
        },
        {
            L_,
            "{"
             "\"name\""
                     ":"
                      "\"Pete\","
                                "{"
            "}"
        },
        {
            L_,
            "{"
             "\"name\""
                     ":"
                      "\"Pete\","
                                "}"
            "}"
        },
        {
            L_,
            "{"
             "\"name\""
                     ":"
                      "\"Pete\","
                                "["
            "}"
        },
        {
            L_,
            "{"
             "\"name\""
                     ":"
                      "\"Pete\","
                                "]"
            "}"
        },
        {
            L_,
            "{"
             "\"name\""
                     ":"
                      "\"Pete\","
                                ":"
            "}"
        },
        {
            L_,
            "{"
             "\"name\""
                     ":"
                      "\"Pete\","
                                ","
        },

        // invalid nested object element name
        {
            L_,
            "{"
              "\"fullname\""
                           ":"
                             "{"
                               "{"
            "}"
        },
        {
            L_,
            "{"
              "\"fullname\""
                           ":"
                             "{"
                               "["
            "}"
        },
        {
            L_,
            "{"
              "\"fullname\""
                           ":"
                             "{"
                               "]"
            "}"
        },
        {
            L_,
            "{"
              "\"fullname\""
                           ":"
                             "{"
                               ","
            "}"
        },
        {
            L_,
            "{"
              "\"fullname\""
                           ":"
                             "{"
                               ":"
            "}"
        },
        {
            L_,
            "{"
              "\"fullname\""
                           ":"
                             "{"
                               "12345"
        },

        // invalid token after nested element
        {
            L_,
            "{"
              "\"fullname\""
                           ":"
                             "{"
                             "}"
                               "12345"
            "}"
        },
        {
            L_,
            "{"
              "\"fullname\""
                           ":"
                             "{"
                             "}"
                               "{"
            "}"
        },
        {
            L_,
            "{"
              "\"fullname\""
                           ":"
                             "{"
                             "}"
                               "["
            "}"
        },
        {
            L_,
            "{"
              "\"fullname\""
                           ":"
                             "{"
                             "}"
                               "]"
            "}"
        },
        {
            L_,
            "{"
              "\"fullname\""
                           ":"
                             "{"
                             "}"
                               ":"
            "}"
        },
        {
            L_,
            "{"
              "\"fullname\""
                           ":"
                             "{"
                             "}"
                               "ABC"
            "}"
        },
        {
            L_,
            "{"
              "\"fullname\""
                           ":"
                             "{"
                             "}"
                               ","
        },

        // nested invalid array element
        {
            L_,
            "{"
              "\"ids\""
                      ":"
                        "["
                          "Hello"
            "}"
        },
        {
            L_,
            "{"
              "\"friends\""
                          ":"
                            "["
                              "Hello"
            "}"
        },
        {
            L_,
            "{"
              "\"ids\""
                      ":"
                        "["
                          ":"
            "}"
        },
        {
            L_,
            "{"
              "\"friends\""
                          ":"
                            "["
                              ":"
            "}"
        },
        {
            L_,
            "{"
              "\"friends\""
                          ":"
                            "["
                              "["
            "}"
        },
        {
            L_,
            "{"
              "\"friends\""
                          ":"
                            "["
                              "1"
                            "]"
            "}"
        },
        {
            L_,
            "{"
              "\"ids\""
                      ":"
                        "["
                          "{"
            "}"
        },
        {
            L_,
            "{"
              "\"ids\""
                      ":"
                        "["
                          ","
            "}"
        },
        {
            L_,
            "{"
              "\"ids\""
                      ":"
                        "["
                          "1"
                            "["
            "}"
        },
        {
            L_,
            "{"
              "\"ids\""
                      ":"
                        "["
                          "1"
                            "{"
            "}"
        },
        {
            L_,
            "{"
              "\"ids\""
                      ":"
                        "["
                          "1"
                            "}"
            "}"
        },
        {
            L_,
            "{"
              "\"ids\""
                      ":"
                        "["
                          "1"
                            ":"
            "}"
        },
        {
            L_,
            "{"
              "\"ids\""
                      ":"
                        "["
                          "1"
                            " \"ABC\""
            "}"
        },
        {
            L_,
            "{"
              "\"ids\""
                      ":"
                        "["
                          "1"
                          " "
                          "2"
                        "]"
            "}"
        },
        {
            L_,
            "{"
              "\"ids\""
                      ":"
                        "["
                          "1"
                          ":"
                          "2"
                        "]"
            "}"
        },
        {
            L_,
            "{"
              "\"ids\""
                      ":"
                        "["
                          "1"
                          "'"
                          "2"
                        "]"
            "}"
        },
        {
            L_,
            "{"
              "\"ids\""
                      ":"
                        "["
                          "1,"
                          "\"Bob\","
                          "2"
                        "]"
            "}"
        },
        {
            L_,
            "{"
              "\"ids\""
                       ":"
                         "["
                           ","
                         "]"
            "}"
        },
        {
            L_,
            "{"
              "\"ids\""
                       ":"
                         "["
                           "1,"
                         "]"
            "}"
        },
        {
            L_,
            "{"
              "\"ids\""
                       ":"
                         "["
                           "{"
                           "}"
                         "]"
            "}"
        },
        {
            L_,
            "{"
              "\"friends\""
                          ":"
                            "["
                              "{"
                                "["
            "}"
        },
        {
            L_,
            "{"
              "\"friends\""
                          ":"
                            "["
                              "{"
                                ":"
            "}"
        },
        {
            L_,
            "{"
              "\"friends\""
                          ":"
                            "["
                              "{"
                                ":"
            "}"
        },
        {
            L_,
            "{"
              "\"friends\""
                          ":"
                            "["
                              "{"
                                "\"lastname\""
            "}"
        },
        {
            L_,
            "{"
              "\"friends\""
                          ":"
                            "["
                              "{"
                                "\"name\","
            "}"
        },
        {
            L_,
            "{"
              "\"friends\""
                          ":"
                            "["
                              "{"
                                "\"name\" 12345"
            "}"
        },
        {
            L_,
            "{"
              "\"friends\""
                          ":"
                            "["
                              "{"
                                "\"name\" 12345"
            "}"
        },
        {
            L_,
            "{"
              "\"friends\""
                          ":"
                            "["
                              "{"
                                "\"ids\""
                                        ":"
                                          "{"
            "}"
        },
        {
            L_,
            "{"
              "\"friends\""
                          ":"
                            "["
                              "{"
                                "\"ids\""
                                        ":"
                                          "]"
            "}"
        },
        {
            L_,
            "{"
              "\"friends\""
                          ":"
                            "["
                              "{"
                                "\"ids\""
                                        ":"
                                          "}"
        },

        // invalid token after array element
        {
            L_,
            "{"
              "\"ids\""
                      ":"
                        "["
                        "]"
                          "\"value\""
            "}"
        },
        {
            L_,
            "{"
              "\"ids\""
                      ":"
                        "["
                        "]"
                          "12345"
            "}"
        },
        {
            L_,
            "{"
              "\"ids\""
                      ":"
                        "["
                        "]"
                      ":"
            "}"
        },
        {
            L_,
            "{"
              "\"friends\""
                          ":"
                            "["
                              "{"
                              "},"
                            "]"
            "}"
        },
        {
            L_,
            "{"
              "\"ids\""
                      ":"
                        "["
                        "]"
                      "{"
            "}"
        },
        {
            L_,
            "{"
              "\"ids\""
                      ":"
                        "["
                        "]"
                      "["
            "}"
        },
        {
            L_,
            "{"
              "\"ids\""
                      ":"
                        "["
                        "]"
                      "]"
            "}"
        },
        {
            L_,
            "{"
              "\"ids\""
                      ":"
                        "["
                        "]"
                      "*"
            "}"
        },
        {
            L_,
            "{"
              "\"ids\""
                       ":"
                         "["
                           "{"
                           "},"
                         "]"
            "}"
        },
        {
            L_,
            "{"
              "\"ids\""
                       ":"
                         "["
                           "["
                           "],"
                         "]"
        },

        // invalid enumeration element value
        {
            L_,
            "{"
              "\"car_color\""
                           ":"
                             "123"
            "}"
        },
        {
            L_,
            "{"
              "\"car_color\""
                           ":"
                             "BLACK"
            "}"
        },
        {
            L_,
            "{"
              "\"car_color\""
                           ":"
                             "\"BLACK\""
            "}"
        },
        };
        const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        // Testing sequences
        {
            for (int ti = 0; ti < 2 * NUM_DATA; ++ti) {
                const int          tj    = ti / 2;
                const bool         UTF8  = ti & 1;
                const int          LINE  = DATA[tj].d_lineNum;
                const bsl::string& INPUT = DATA[tj].d_text_p;
                case4::Employee bob;

                bsl::istringstream iss(INPUT);

                baljsn::DecoderOptions options;
                if (UTF8) {
                    options.setValidateInputIsUtf8(true);
                }
                baljsn::Decoder        decoder;
                const int RC = decoder.decode(iss, &bob, options);
                ASSERTV(LINE, RC, 0 != RC);
                if (veryVerbose) {
                    P(decoder.loggedMessages());
                }

                {
                    bsl::istringstream iss(INPUT);
                    ASSERT(decoder.decodeAny(iss, &bob, options) != 0);
                }
            }
        }

        // Testing choices
        {
            for (int ti = 0; ti < 2 * NUM_DATA; ++ti) {
                const int          tj    = ti / 2;
                const bool         UTF8  = ti & 1;
                const int          LINE  = DATA[tj].d_lineNum;
                const bsl::string& INPUT = DATA[tj].d_text_p;

                case4::Employee bob;

                bsl::istringstream iss(INPUT);

                baljsn::DecoderOptions options;
                if (UTF8) {
                    options.setValidateInputIsUtf8(true);
                }
                baljsn::Decoder        decoder;
                const int RC = decoder.decode(iss, &bob, options);
                ASSERTV(LINE, RC, 0 != RC);
                if (veryVerbose) {
                    P(decoder.loggedMessages());
                }

                {
                    bsl::istringstream iss(INPUT);
                    ASSERT(decoder.decodeAny(iss, &bob, options) != 0);
                }
            }
        }

        // Testing arrays
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_text_p;   // json text
            } DATA[] = {
                //line  input
                //----  -----
                {
                    L_,
                    "["
                      "}"
                    "]"
                },
                {
                    L_,
                    "["
                      "\"Bob\""
                    "]"
                },
                {
                    L_,
                    "["
                      ","
                    "]"
                },
                {
                    L_,
                    "["
                      "["
                    "]"
                },
                {
                    L_,
                    "["
                      "1"
                        ","
                    "]"
                },
                {
                    L_,
                    "["
                      "1"
                        "{"
                    "]"
                },
                {
                    L_,
                    "["
                      "1"
                        "}"
                    "]"
                },
                {
                    L_,
                    "["
                      "{"
                        "{"
                    "]"
                },
                {
                    L_,
                    "["
                      "{"
                        ":"
                    "]"
                },
                {
                    L_,
                    "["
                      "{"
                        ","
                    "]"
                },
                {
                    L_,
                    "["
                      "{"
                        "12345"
                    "]"
                },
                {
                    L_,
                    "["
                      "{"
                        "\"Bob\""
                    "]"
                },
                {
                    L_,
                    "["
                      "{"
                        "\"name\""
                      "}"
                    "]"
                },
                {
                    L_,
                    "["
                      "{"
                        "\"name\": \"Bob\""
                      "},"
                    "]"
                },
                {
                    L_,
                    "["
                      "{"
                        "\"name\": \"Bob\""
                      "},"
                      "\"name\": \"John\""
                    "]"
                },
                {
                    L_,
                    "["
                },
            };
            const int NUM_DATA = sizeof DATA / sizeof DATA[0];

            for (int ti = 0; ti < 2 * NUM_DATA; ++ti) {
                const int          tj    = ti / 2;
                const bool         UTF8  = ti & 1;
                const int          LINE  = DATA[tj].d_lineNum;
                const bsl::string& INPUT = DATA[tj].d_text_p;

                case4::Employee bob;

                bsl::istringstream iss(INPUT);

                baljsn::DecoderOptions options;
                if (UTF8) {
                    options.setValidateInputIsUtf8(true);
                }
                baljsn::Decoder        decoder;
                const int RC = decoder.decode(iss, &bob, options);
                ASSERTV(LINE, RC, 0 != RC);
                if (veryVerbose) {
                    P(decoder.loggedMessages());
                }

                {
                    bsl::istringstream iss(INPUT);
                    ASSERT(decoder.decodeAny(iss, &bob, options) != 0);
                }
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING SKIPPING UNKNOWN ELEMENTS
        //
        // Concerns:
        // 1. The decoder correctly skips unknown elements if the
        //    `skipUnknownElement` decoder option is specified.
        //
        // 2. The decoder returns an error on encountering unknown elements if
        //    the `skipUnknownElement` decoder option is *not* specified.
        //
        // 3. The decoder returns an error on encountering malformed unknown
        //    element regardless of the `skipUnknownElement` decoder option
        //    value.
        //
        // 4. The decoder correctly counts the skipped elements if the
        //    `skipUnknownElement` decoder option is specified.
        //
        // Plan:
        // 1. Using the table-driven technique, specify a table with JSON text.
        //
        // 2. For each row in the tables of P-1:
        //
        //   1. Construct a test object.
        //
        //   2. Create a `baljsn::Decoder` object.
        //
        //   3. Create a `bsl::istringstream` object with the JSON text.
        //
        //   4. Decode that JSON into a test object specifying
        //      that unknown elements be skipped.
        //
        //   5. Verify that the decoded object has the expected data.
        //
        //   6. Verify that the return code from `decode` is 0.  (C-1)
        //
        //   7. Verify that `numUnknownElementsSkipped` has the value specified
        //      in the table.  (C-4)
        //
        //   8. Repeat steps 1 - 7 with the `skipUnknownElements` option set
        //      to `false`.  Verify that an error code is returned by `decode`.
        //      (C-2)
        //
        // 3. Using the table-driven technique, specify a table with malformed
        //    JSON texts.
        //
        // 4. For each row in the tables of P-3:
        //
        //   1. Construct a test object.
        //
        //   2. Create a `baljsn::Decoder` object.
        //
        //   3. Create a `bsl::istringstream` object with the JSON text.
        //
        //   4. Decode that JSON into a test object specifying
        //      that unknown elements be skipped.
        //
        //   5. Verify that an error code is returned by `decode`.
        //
        //   6. Repeat steps 1 - 5 with the `skipUnknownElements` option set
        //      to `false`.  (C-3)
        //
        // Testing:
        //   int decode(bsl::streambuf *streamBuf, TYPE *v, options);
        //   int decode(bsl::istream& stream, TYPE *v, options);
        //   bsl::string loggedMessages() const;
        //   int numUnknownElementsSkipped() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING SKIPPING UNKNOWN ELEMENTS"
                          << "\n=================================" << endl;

        if (verbose) cout << "Testing valid JSON strings" << endl;

        static const struct {
            int         d_lineNum;             // source line number
            const char *d_text_p;              // json text
            int         d_numUnknownElements;  // number of unknown elements
        } DATA[] = {
            {
                L_,
                "{\n"
                "       \"name\": \"Bob\",\n"
                "       \"homeAddress\": {\n"
                "           \"street\": \"Some Street\",\n"
                "           \"city\": \"Some City\",\n"
                "           \"state\": \"Some State\"\n"
                "       },\n"
                "       \"age\": 21,\n"
                "       \"id\": 21\n"                 // <--- unknown element
                "}",
                1
            },
            {
                L_,
                "{\n"
                "       \"name\": \"Bob\",\n"
                "       \"homeAddress\": {\n"
                "           \"street\": \"Some Street\",\n"
                "           \"city\": \"Some City\",\n"
                "           \"state\": \"Some State\"\n"
                "       },\n"
                "       \"id\": 21,\n"                // <--- unknown element
                "       \"age\": 21\n"
                "}",
                1
            },
            {
                L_,
                "{\n"
                "       \"name\": \"Bob\",\n"
                "       \"homeAddress\": {\n"
                "           \"street\": \"Some Street\",\n"
                "           \"city\": \"Some City\",\n"
                "           \"state\": \"Some State\"\n"
                "       },\n"
                "       \"age\": 21,\n"
                "       \"nickname\": \"Robert\"\n"    // <--- unknown element
                "}",
                1
            },
            {
                L_,
                "{\n"
                "       \"name\": \"Bob\",\n"
                "       \"homeAddress\": {\n"
                "           \"street\": \"Some Street\",\n"
                "           \"city\": \"Some City\",\n"
                "           \"state\": \"Some State\"\n"
                "       },\n"
                "       \"nickname\": \"Robert\",\n"   // <--- unknown element
                "       \"age\": 21\n"
                "}",
                1
            },
            {
                L_,
                "{\n"
                "       \"name\": \"Bob\",\n"
                "       \"homeAddress\": {\n"
                "           \"street\": \"Some Street\",\n"
                "           \"city\": \"Some City\",\n"
                "           \"state\": \"Some State\"\n"
                "       },\n"
                "       \"age\": 21,\n"
                "       \"id\": 21,\n"                 // <--- unknown element
                "       \"nickname\": \"Robert\"\n"    // <--- unknown element
                "}",
                2
            },
            {
                L_,
                "{\n"
                "       \"name\": \"Bob\",\n"
                "       \"homeAddress\": {\n"
                "           \"street\": \"Some Street\",\n"
                "           \"city\": \"Some City\",\n"
                "           \"state\": \"Some State\"\n"
                "       },\n"
                "       \"id\": 21,\n"                 // <--- unknown element
                "       \"nickname\": \"Robert\",\n"   // <--- unknown element
                "       \"age\": 21\n"
                "}",
                2
            },
            {
                L_,
                "{\n"
                "       \"name\": \"Bob\",\n"
                "       \"homeAddress\": {\n"
                "           \"street\": \"Some Street\",\n"
                "           \"city\": \"Some City\",\n"
                "           \"state\": \"Some State\"\n"
                "       },\n"
                "       \"age\": 21,\n"
                "       \"ids\": [ 1, 2 ]\n"           // <--- unknown element
                "}",
                1
            },
            {
                L_,
                "{\n"
                "       \"name\": \"Bob\",\n"
                "       \"homeAddress\": {\n"
                "           \"street\": \"Some Street\",\n"
                "           \"city\": \"Some City\",\n"
                "           \"state\": \"Some State\"\n"
                "       },\n"
                "       \"ids\": [ 1, 2 ],\n"          // <--- unknown element
                "       \"age\": 21\n"
                "}",
                1
            },
            {
                L_,
                "{\n"
                "       \"name\": \"Bob\",\n"
                "       \"homeAddress\": {\n"
                "           \"street\": \"Some Street\",\n"
                "           \"city\": \"Some City\",\n"
                "           \"state\": \"Some State\"\n"
                "       },\n"
                "       \"age\": 21,\n"
                "       \"aliases\": [ \"Foo\", \"Bar\" ]\n"   // <--- unknown
                                                                //      element
                "}",
                1
            },
            {
                L_,
                "{\n"
                "       \"name\": \"Bob\",\n"
                "       \"homeAddress\": {\n"
                "           \"street\": \"Some Street\",\n"
                "           \"city\": \"Some City\",\n"
                "           \"state\": \"Some State\"\n"
                "       },\n"
                "       \"aliases\": [ \"Foo\", \"Bar\" ],\n"  // <--- unknown
                                                                //      element
                "       \"age\": 21\n"
                "}",
                1
            },
            {
                L_,
                "{\n"
                "       \"name\": \"Bob\",\n"
                "       \"homeAddress\": {\n"
                "           \"street\": \"Some Street\",\n"
                "           \"city\": \"Some City\",\n"
                "           \"state\": \"Some State\"\n"
                "       },\n"
                "       \"age\": 21,\n"
                "       \"officeAddress\": {\n"        // <--- unknown element
                "           \"street\": \"Some Street\",\n"
                "           \"city\": \"Some City\",\n"
                "           \"state\": \"Some State\"\n"
                "       }\n"
                "}",
                1
            },
            {
                L_,
                "{\n"
                "       \"name\": \"Bob\",\n"
                "       \"homeAddress\": {\n"
                "           \"street\": \"Some Street\",\n"
                "           \"city\": \"Some City\",\n"
                "           \"state\": \"Some State\"\n"
                "       },\n"
                "       \"officeAddress\": {\n"        // <--- unknown element
                "           \"street\": \"Some Street\",\n"
                "           \"city\": \"Some City\",\n"
                "           \"state\": \"Some State\"\n"
                "       },\n"
                "       \"age\": 21\n"
                "}",
                1
            },
            {
                L_,
                "{\n"
                "       \"name\": \"Bob\",\n"
                "       \"homeAddress\": {\n"
                "           \"street\": \"Some Street\",\n"
                "           \"city\": \"Some City\",\n"
                "           \"state\": \"Some State\"\n"
                "       },\n"
                "       \"age\": 21,\n"
                "       \"officeAddress\": {\n"        // <--- unknown element
                "           \"street\": \"Some Street\",\n"
                "           \"city\": \"Some City\",\n"
                "           \"state\": \"Some State\",\n"
                "           \"ids\": [ 1, 2, 3]\n"
                "       }\n"
                "}",
                1
            },
            {
                L_,
                "{\n"
                "       \"name\": \"Bob\",\n"
                "       \"homeAddress\": {\n"
                "           \"street\": \"Some Street\",\n"
                "           \"city\": \"Some City\",\n"
                "           \"state\": \"Some State\"\n"
                "       },\n"
                "       \"officeAddress\": {\n"        // <--- unknown element
                "           \"street\": \"Some Street\",\n"
                "           \"city\": \"Some City\",\n"
                "           \"state\": \"Some State\",\n"
                "           \"ids\": [ 1, 2, 3]\n"
                "       },\n"
                "       \"age\": 21\n"
                "}",
                1
            },
            {
                L_,
                "{\n"
                "       \"name\": \"Bob\",\n"
                "       \"homeAddress\": {\n"
                "           \"street\": \"Some Street\",\n"
                "           \"city\": \"Some City\",\n"
                "           \"state\": \"Some State\"\n"
                "       },\n"
                "       \"officeAddress\": {\n"        // <--- unknown element
                "           \"street\": \"Some Street\",\n"
                "           \"city\": \"Some City\",\n"
                "           \"state\": \"Some State\",\n"
                "           \"misc\": {\n"
                "               \"country\": \"USA\",\n"
                "               \"timezone\": \"EST\"\n"
                "           }\n"
                "       },\n"
                "       \"age\": 21\n"
                "}",
                1
            },
            {
                L_,
                "{\n"
                "       \"name\": \"Bob\",\n"
                "       \"homeAddress\": {\n"
                "           \"street\": \"Some Street\",\n"
                "           \"city\": \"Some City\",\n"
                "           \"state\": \"Some State\"\n"
                "       },\n"
                "       \"age\": 21,\n"
                "       \"officeAddress\": {\n"        // <--- unknown element
                "           \"street\": \"Some Street\",\n"
                "           \"city\": \"Some City\",\n"
                "           \"state\": \"Some State\",\n"
                "           \"misc\": {\n"
                "               \"country\": \"USA\",\n"
                "               \"timezone\": \"EST\"\n"
                "           }\n"
                "       }\n"
                "}",
                1
            },
            {
                L_,
                "{\n"
                "       \"name\": \"Bob\",\n"
                "       \"homeAddress\": {\n"
                "           \"street\": \"Some Street\",\n"
                "           \"city\": \"Some City\",\n"
                "           \"state\": \"Some State\"\n"
                "       },\n"
                "       \"addrs\": [\n"                // <--- unknown element
                "           {\n"
                "               \"officeAddress\": {\n"
                "                   \"street\": \"Some Street\",\n"
                "                   \"city\": \"Some City\",\n"
                "                   \"state\": \"Some State\",\n"
                "                   \"times\": [ 1, 2, 3 ],\n"
                "                   \"misc\": {\n"
                "                       \"country\": \"USA\",\n"
                "                       \"timezone\": \"EST\"\n"
                "                   }\n"
                "               }\n"
                "           }\n"
                "       ],\n"
                "       \"age\": 21\n"
                "}",
                1
            },
            {
                L_,
                "{\n"
                "       \"name\": \"Bob\",\n"
                "       \"homeAddress\": {\n"
                "           \"street\": \"Some Street\",\n"
                "           \"city\": \"Some City\",\n"
                "           \"state\": \"Some State\"\n"
                "       },\n"
                "       \"age\": 21,\n"
                "       \"addrs\": [\n"                // <--- unknown element
                "           {\n"
                "               \"officeAddress\": {\n"
                "                   \"street\": \"Some Street\",\n"
                "                   \"city\": \"Some City\",\n"
                "                   \"state\": \"Some State\",\n"
                "                   \"times\": [ 1, 2, 3 ],\n"
                "                   \"misc\": {\n"
                "                       \"country\": \"USA\",\n"
                "                       \"timezone\": \"EST\"\n"
                "                   }\n"
                "               }\n"
                "           }\n"
                "       ]\n"
                "}",
                1
            },
            {
                L_,
                "{\n"
                "       \"name\": \"Bob\",\n"
                "       \"homeAddress\": {\n"
                "           \"street\": \"Some Street\",\n"
                "           \"city\": \"Some City\",\n"
                "           \"state\": \"Some State\"\n"
                "       },\n"
                "       \"addrs\": [\n"                // <--- unknown element
                "           {\n"
                "               \"officeAddress\": {\n"
                "                   \"street\": \"Some Street\",\n"
                "                   \"city\": \"Some City\",\n"
                "                   \"state\": \"Some State\",\n"
                "                   \"times\": [ 1, 2, 3 ],\n"
                "                   \"misc\": {\n"
                "                       \"country\": \"USA\",\n"
                "                       \"timezone\": \"EST\"\n"
                "                   }\n"
                "               }\n"
                "           }\n"
                "       ],\n"
                "       \"age\": 21\n"
                "}",
                1
            },
            {
                L_,
                "{\n"
                "       \"name\": \"Bob\",\n"
                "       \"homeAddress\": {\n"
                "           \"street\": \"Some Street\",\n"
                "           \"city\": \"Some City\",\n"
                "           \"state\": \"Some State\"\n"
                "       },\n"
                "       \"age\": 21,\n"
                "       \"addrs\": [\n"                // <--- unknown element
                "           {\n"
                "               \"officeAddress\": {\n"
                "                   \"street\": \"Some Street\",\n"
                "                   \"city\": \"Some City\",\n"
                "                   \"state\": \"Some State\",\n"
                "                   \"times\": [ 1, 2, 3 ],\n"
                "                   \"misc\": {\n"
                "                       \"country\": \"USA\",\n"
                "                       \"timezone\": \"EST\"\n"
                "                   }\n"
                "               }\n"
                "           },\n"
                "           {\n"
                "               \"shippingAddress\": {\n"
                "                   \"street\": \"Some Street\",\n"
                "                   \"city\": \"Some City\",\n"
                "                   \"state\": \"Some State\",\n"
                "                   \"times\": [ 1, 2, 3 ],\n"
                "                   \"misc\": {\n"
                "                       \"country\": \"USA\",\n"
                "                       \"timezone\": \"EST\"\n"
                "                   }\n"
                "               }\n"
                "           }\n"
                "       ]\n"
                "}",
                1
            },
            {
                L_,
                "{\n"
                "       \"name\": \"Bob\",\n"
                "       \"homeAddress\": {\n"
                "           \"street\": \"Some Street\",\n"
                "           \"city\": \"Some City\",\n"
                "           \"state\": \"Some State\"\n"
                "       },\n"
                "       \"addrs\": [\n"                // <--- unknown element
                "           {\n"
                "               \"officeAddress\": {\n"
                "                   \"street\": \"Some Street\",\n"
                "                   \"city\": \"Some City\",\n"
                "                   \"state\": \"Some State\",\n"
                "                   \"times\": [ 1, 2, 3 ],\n"
                "                   \"misc\": {\n"
                "                       \"country\": \"USA\",\n"
                "                       \"timezone\": \"EST\"\n"
                "                   }\n"
                "               }\n"
                "           },\n"
                "           {\n"
                "               \"shippingAddress\": {\n"
                "                   \"street\": \"Some Street\",\n"
                "                   \"city\": \"Some City\",\n"
                "                   \"state\": \"Some State\",\n"
                "                   \"times\": [ 1, 2, 3 ],\n"
                "                   \"misc\": {\n"
                "                       \"country\": \"USA\",\n"
                "                       \"timezone\": \"EST\"\n"
                "                   }\n"
                "               }\n"
                "           }\n"
                "       ],\n"
                "       \"age\": 21\n"
                "}",
                1
            },
            {
                L_,
                "{\n"
                "       \"name\": \"Bob\",\n"
                "       \"homeAddress\": {\n"
                "           \"street\": \"Some Street\",\n"
                "           \"city\": \"Some City\",\n"
                "           \"state\": \"Some State\"\n"
                "       },\n"
                "       \"age\": 21,\n"
                "       \"misc\": {\n"                 // <--- unknown element
                "           \"name\": \"Bob\",\n"
                "           \"homeAddress\": {\n"
                "               \"street\": \"Some Street\",\n"
                "               \"city\": \"Some City\",\n"
                "               \"state\": \"Some State\"\n"
                "           },\n"
                "           \"age\": 21,\n"
                "           \"addrs\": [\n"
                "               {\n"
                "                   \"officeAddress\": {\n"
                "                       \"street\": \"Some Street\",\n"
                "                       \"city\": \"Some City\",\n"
                "                       \"state\": \"Some State\",\n"
                "                       \"times\": [ 1, 2, 3 ],\n"
                "                       \"misc\": {\n"
                "                           \"country\": \"USA\",\n"
                "                           \"timezone\": \"EST\"\n"
                "                       }\n"
                "                   }\n"
                "               },\n"
                "               {\n"
                "                   \"shippingAddress\": {\n"
                "                       \"street\": \"Some Street\",\n"
                "                       \"city\": \"Some City\",\n"
                "                       \"state\": \"Some State\",\n"
                "                       \"times\": [ 1, 2, 3 ],\n"
                "                       \"misc\": {\n"
                "                           \"country\": \"USA\",\n"
                "                           \"timezone\": \"EST\"\n"
                "                       }\n"
                "                   }\n"
                "               }\n"
                "           ]\n"
                "       }\n"
                "}",
                1
            },
            {
                L_,
                "{\n"
                "       \"name\": \"Bob\",\n"
                "       \"homeAddress\": {\n"
                "           \"street\": \"Some Street\",\n"
                "           \"city\": \"Some City\",\n"
                "           \"state\": \"Some State\"\n"
                "       },\n"
                "       \"misc\": {\n"                 // <--- unknown element
                "           \"name\": \"Bob\",\n"
                "           \"homeAddress\": {\n"
                "               \"street\": \"Some Street\",\n"
                "               \"city\": \"Some City\",\n"
                "               \"state\": \"Some State\"\n"
                "           },\n"
                "           \"age\": 21,\n"
                "           \"addrs\": [\n"
                "               {\n"
                "                   \"officeAddress\": {\n"
                "                       \"street\": \"Some Street\",\n"
                "                       \"city\": \"Some City\",\n"
                "                       \"state\": \"Some State\",\n"
                "                       \"times\": [ 1, 2, 3 ],\n"
                "                       \"misc\": {\n"
                "                           \"country\": \"USA\",\n"
                "                           \"timezone\": \"EST\"\n"
                "                       }\n"
                "                   }\n"
                "               },\n"
                "               {\n"
                "                   \"shippingAddress\": {\n"
                "                       \"street\": \"Some Street\",\n"
                "                       \"city\": \"Some City\",\n"
                "                       \"state\": \"Some State\",\n"
                "                       \"times\": [ 1, 2, 3 ],\n"
                "                       \"misc\": {\n"
                "                           \"country\": \"USA\",\n"
                "                           \"timezone\": \"EST\"\n"
                "                       }\n"
                "                   }\n"
                "               }\n"
                "           ]\n"
                "       },\n"
                "       \"age\": 21\n"
                "}",
                1
            },
            // `baljsn::Decoder` fails to decode heterogeneous arrays, but
            // since we skip an unknown element it is allowed to contain such
            // arrays.  See {DRQS 171296111} for more details.
            {
                L_,
                "{\n"
                "       \"name\": \"Bob\",\n"
                "       \"homeAddress\": {\n"
                "           \"street\": \"Some Street\",\n"
                "           \"city\": \"Some City\",\n"
                "           \"state\": \"Some State\"\n"
                "       },\n"
                "       \"build-depends\": [\n"         // <--- unknown element
                "           \"cmake-breg-generate-code\",\n"
                "           \"cmake-configure-bb-target\",\n"
                "           \"cmake-upside-down-shared-objects\",\n"
                "           {\n"
                "               \"name\": \"liba-basfs-dev\",\n"
                "               \"architectures\":\n"
                "               [\n"
                "                   \"aix6-powerpc\",\n"
                "                   \"solaris10-sparc\"\n"
                "               ]\n"
                "           },\n"
                "           \"liba-iaabass-dev\"\n"
                "       ],\n"
                "       \"age\": 21\n"
                "}",
                1
            },
        };
        const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        for (int ti = 0; ti < 2 * NUM_DATA; ++ti) {
            const int          tj       = ti / 2;
            const bool         UTF8     = ti & 1;
            const int          LINE     = DATA[tj].d_lineNum;
            const bsl::string& jsonText = DATA[tj].d_text_p;
            const int          NUM_UNKNOWN_ELEMENTS
                                        = DATA[tj].d_numUnknownElements;

            // Without skipping option
            baljsn::Decoder        decoder;
            baljsn::DecoderOptions options;
            if (UTF8) {
                options.setValidateInputIsUtf8(true);
            }
            const baljsn::DecoderOptions& mO = options;

            options.setSkipUnknownElements(false);
            {
                test::Employee     bob;
                bsl::istringstream iss(jsonText);
                ASSERTV(LINE, 0 != decoder.decode(iss, &bob, mO));
                ASSERTV(LINE, 0 == decoder.numUnknownElementsSkipped());
            }
            {
                test::Employee     bob;
                bsl::istringstream iss(jsonText);
                ASSERTV(LINE, 0 != decoder.decode(iss, &bob, &mO));
                ASSERTV(LINE, 0 == decoder.numUnknownElementsSkipped());
            }
            {
                test::Employee     bob;
                bsl::istringstream iss(jsonText);
                ASSERTV(LINE, 0 != decoder.decodeAny(iss, &bob, mO));
                ASSERTV(LINE, 0 == decoder.numUnknownElementsSkipped());
            }

            // With skipping option
            options.setSkipUnknownElements(true);
            {
                test::Employee     bob;
                bsl::istringstream iss(jsonText);
                ASSERTV(LINE, 0 == decoder.decode(iss, &bob, mO));
                ASSERTV(bob.name(), "Bob" == bob.name());
                ASSERT("Some Street" == bob.homeAddress().street());
                ASSERT("Some City"   == bob.homeAddress().city());
                ASSERT("Some State"  == bob.homeAddress().state());
                ASSERTV(LINE, 21     == bob.age());
                ASSERTV(LINE, NUM_UNKNOWN_ELEMENTS ==
                                          decoder.numUnknownElementsSkipped());
            }
            test::Employee     bob;
            {
                bsl::istringstream iss(jsonText);
                ASSERTV(LINE, 0 == decoder.decode(iss, &bob, &mO));
                ASSERTV(bob.name(), "Bob" == bob.name());
                ASSERT("Some Street" == bob.homeAddress().street());
                ASSERT("Some City"   == bob.homeAddress().city());
                ASSERT("Some State"  == bob.homeAddress().state());
                ASSERTV(LINE, 21     == bob.age());
                ASSERTV(LINE, NUM_UNKNOWN_ELEMENTS ==
                                          decoder.numUnknownElementsSkipped());
            }
            {
                bsl::istringstream iss(jsonText);
                test::Employee     bob2;
                ASSERT(decoder.decodeAny(iss, &bob2, mO) == 0);
                ASSERT(bob2 == bob);
                ASSERT(NUM_UNKNOWN_ELEMENTS ==
                                          decoder.numUnknownElementsSkipped());
            }
        }

        if (verbose) cout << "Testing malformed JSON strings" << endl;

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_text_p;   // json text
        } MALFORMED_DATA[] = {
            {
                L_,
                "{\n"
                "       \"name\": \"Bob\",\n"
                "       \"homeAddress\": {\n"
                "           \"street\": \"Some Street\",\n"
                "           \"city\": \"Some City\",\n"
                "           \"state\": \"Some State\"\n"
                "       },\n"
                "       \"age\": 21,\n"
                "       \"empty value\"  :    \n"       // <--- unknown element
                "}"
            },
            {
                L_,
                "{\n"
                "       \"name\": \"Bob\",\n"
                "       \"object\": {\n"               // <--- unknown element
                "           \"empty property value\": \n"
                "       },\n"
                "       \"age\": 21\n"
                "}"
            },
            {
                L_,
                "{\n"
                "       \"name\": \"Bob\",\n"
                "       \"build-depends\": [\n"         // <--- unknown element
                "           \"key-value pair\": \" in array\"\n"
                "       ],\n"
                "       \"age\": 21\n"
                "}"
            },
        };
        const int NUM_MALFORMED_DATA = sizeof MALFORMED_DATA /
                                       sizeof MALFORMED_DATA[0];

        for (int ti = 0; ti < 2 * NUM_MALFORMED_DATA; ++ti) {
            const int          tj       = ti / 2;
            const bool         UTF8     = ti & 1;
            const int          LINE     = MALFORMED_DATA[tj].d_lineNum;
            const bsl::string& jsonText = MALFORMED_DATA[tj].d_text_p;

            // Without skipping option
            baljsn::Decoder        decoder;
            baljsn::DecoderOptions options;
            if (UTF8) {
                options.setValidateInputIsUtf8(true);
            }
            const baljsn::DecoderOptions& mO = options;

            options.setSkipUnknownElements(false);
            {
                test::Employee     bob;
                bsl::istringstream iss(jsonText);
                ASSERTV(LINE, 0 != decoder.decode(iss, &bob, mO));
            }
            {
                test::Employee     bob;
                bsl::istringstream iss(jsonText);
                ASSERTV(LINE, 0 != decoder.decodeAny(iss, &bob, mO));
            }

            // With skipping option
            options.setSkipUnknownElements(true);
            {
                test::Employee     bob;
                bsl::istringstream iss(jsonText);
                ASSERTV(LINE, 0 != decoder.decode(iss, &bob, mO));
            }
            {
                test::Employee     bob;
                bsl::istringstream iss(jsonText);
                ASSERTV(LINE, 0 != decoder.decodeAny(iss, &bob, mO));
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING COMPLEX MESSAGES
        //
        // Concerns:
        // 1. The decoder correctly decodes a variety of complex JSON data into
        //    `bas-codegen` generated objects.
        //
        // 2. The decoder correctly parses the input in both pretty and compact
        //    style.
        //
        // Plan:
        // 1. Using the table-driven technique, specify three tables: one with
        //    a set of distinct rows of XML string value corresponding to a
        //    `balb::FeatureTestMessage` object, the second with JSON in pretty
        //    format, and third with the JSON in the compact format.
        //
        // 2. For each row in the tables of P-1:
        //
        //   1. Construct a `balb::FeatureTestMessage` object from the XML
        //      string using the XML decoder.
        //
        //   2. Create a `baljsn::Decoder` object.
        //
        //   3. Create a `bdlsb::FixedMemInStreamBuf` object with the pretty
        //      JSON text.
        //
        //   4. Decode that JSON into a `balb::FeatureTestMessage` object.
        //
        //   5. Verify that the decoded object matches the original object
        //      from step 1.
        //
        //   6. Repeat steps 1 - 5 using JSON in the compact format.
        //
        // Testing:
        //   baljsn::Decoder(bslma::Allocator *basicAllocator = 0);
        //   ~baljsn::Decoder();
        //   int decode(bsl::streambuf *streamBuf, TYPE *v, options);
        //   int decode(bsl::istream& stream, TYPE *v, options);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING COMPLEX MESSAGES"
                          << "\n========================" << endl;

        bsl::vector<balb::FeatureTestMessage> testObjects;
        constructFeatureTestMessage(&testObjects);

        for (int ti = 0; ti < 2 * DTMU::k_NUM_MESSAGES; ++ti) {
            const int          tj   = ti / 2;
            const bool         UTF8 = ti & 1;
            const int          LINE = tj;
            const bsl::string& PRETTY =
                                      DTMU::s_TEST_MESSAGES[tj].d_prettyJSON_p;
            const balb::FeatureTestMessage& EXP = testObjects[tj];

            if (veryVerbose) {
                P(ti);    P(LINE);    P(PRETTY);
                EXP.print(cout, 1, 4);
            }

            {
                balb::FeatureTestMessage   value;
                baljsn::DecoderOptions     options;
                if (UTF8) {
                    options.setValidateInputIsUtf8(true);
                }
                baljsn::Decoder            decoder;
                bdlsb::FixedMemInStreamBuf isb(PRETTY.data(), PRETTY.length());

                const int rc = decoder.decode(&isb, &value, options);
                ASSERTV(LINE, decoder.loggedMessages(), rc, 0 == rc);
                ASSERTV(LINE, 1 == isb.length()); // trailing newline
                ASSERTV(LINE, decoder.loggedMessages(), EXP, value,
                        EXP == value);

                {
                    bdlsb::FixedMemInStreamBuf isb(PRETTY.data(),
                                                   PRETTY.length());
                    balb::FeatureTestMessage value2;
                    ASSERT(decoder.decodeAny(&isb, &value2, options) == 0);
                    ASSERT(value2 == value);
                }
            }

            {
                balb::FeatureTestMessage  value;
                baljsn::DecoderOptions     options;
                if (UTF8) {
                    options.setValidateInputIsUtf8(true);
                }
                baljsn::Decoder            decoder;
                bdlsb::FixedMemInStreamBuf isb(PRETTY.data(), PRETTY.length());
                bsl::istream              iss(&isb);

                const int rc = decoder.decode(iss, &value, options);
                ASSERTV(LINE, decoder.loggedMessages(), rc, 0 == rc);
                ASSERTV(LINE, 1 == isb.length()); // trailing newline
                ASSERTV(LINE, decoder.loggedMessages(), EXP, value,
                        EXP == value);

                {
                    bdlsb::FixedMemInStreamBuf isb(PRETTY.data(),
                                                   PRETTY.length());
                    bsl::istream               iss(&isb);
                    balb::FeatureTestMessage value2;
                    ASSERT(decoder.decodeAny(iss, &value2, options) == 0);
                    ASSERT(value2 == value);
                }
            }
        }

        for (int ti = 0; ti < 2 * DTMU::k_NUM_MESSAGES; ++ti) {
            const int          tj      = ti / 2;
            const bool         UTF8    = ti & 1;
            const int          LINE    = tj;
            const bsl::string& COMPACT =
                                     DTMU::s_TEST_MESSAGES[tj].d_compactJSON_p;
            const balb::FeatureTestMessage& EXP = testObjects[tj];

            if (veryVerbose) {
                P(ti);    P(LINE);    P(COMPACT);
                EXP.print(cout, 1, 4);
            }

            {
                balb::FeatureTestMessage value;

                baljsn::DecoderOptions     options;
                if (UTF8) {
                    options.setValidateInputIsUtf8(true);
                }
                baljsn::Decoder            decoder;
                bdlsb::FixedMemInStreamBuf isb(COMPACT.data(),
                                              COMPACT.length());
                bsl::istream              iss(&isb);

                const int rc = decoder.decode(&isb, &value, options);
                ASSERTV(LINE, decoder.loggedMessages(), rc, 0 == rc);
                ASSERTV(LINE, isb.length(), 0 == isb.length());
                ASSERTV(LINE, decoder.loggedMessages(), EXP, value,
                        EXP == value);

                {
                    bdlsb::FixedMemInStreamBuf isb(COMPACT.data(),
                                                   COMPACT.length());
                    balb::FeatureTestMessage value2;
                    ASSERT(decoder.decodeAny(&isb, &value2, options) == 0);
                    ASSERT(value2 == value);
                }
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        // 1. The class is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

        char jsonText[] =
            "{\n"
            "       \"name\": \"Bob\",\n"
            "       \"homeAddress\": {\n"
            "           \"street\": \"Some Street\",\n"
            "           \"city\": \"Some City\",\n"
            "           \"state\": \"Some State\"\n"
            "       },\n"
            "       \"age\": 21\n"
            "}";

        for (int tu = 0; tu < 2; ++tu) {
            test::Employee bob;

            bsl::istringstream iss(jsonText);

            baljsn::DecoderOptions options;
            baljsn::Decoder        decoder;
            if (tu) {
                options.setValidateInputIsUtf8(true);
            }
            ASSERTV(0 == decoder.decode(iss, &bob, options));

            ASSERTV(bob.name(), "Bob"         == bob.name());
            ASSERT("Some Street" == bob.homeAddress().street());
            ASSERT("Some City"   == bob.homeAddress().city());
            ASSERT("Some State"  == bob.homeAddress().state());
            ASSERT(21            == bob.age());
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

#ifdef U_SKIP_DUE_TO_COMPILER_RESOURCE_LIMITATIONS
#undef U_SKIP_DUE_TO_COMPILER_RESOURCE_LIMITATIONS
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
