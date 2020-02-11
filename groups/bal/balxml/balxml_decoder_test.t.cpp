// balxml_decoder_test.t.cpp                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balxml_decoder_test.h>

#include <balxml_decoderoptions.h>
#include <balxml_errorinfo.h>
#include <balxml_minireader.h>

#include <bdlat_attributeinfo.h>
#include <bdlat_choicefunctions.h>
#include <bdlat_enumeratorinfo.h>
#include <bdlat_formattingmode.h>
#include <bdlat_selectioninfo.h>
#include <bdlat_sequencefunctions.h>
#include <bdlat_typetraits.h>
#include <bdlat_valuetypefunctions.h>

#include <bdlb_chartype.h>
#include <bdlb_nullableallocatedvalue.h>
#include <bdlb_nullablevalue.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>
#include <bdlb_string.h>
#include <bdlb_variant.h>

#include <bdldfp_decimal.h>

#include <bsla_fallthrough.h>

#include <bslim_testutil.h>

#include <bslalg_typetraits.h>

#include <bslim_testutil.h>

#include <bslma_allocator.h>
#include <bslma_default.h>

#include <bsls_assert.h>
#include <bsls_review.h>

#include <bsl_cstddef.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test is a decoder that decodes XML input to produce a
// value for a parameterized 'TYPE'.  Each 'TYPE' is represented by a context
// class that implements the 'Decoder_ElementContext' protocol.  This
// component provides context classes for the following types:
//
//     - 'bsl::string' (parsed as UTF8).
//     - 'bsl::vector<char>' (parsed as Base64).
//     - other 'bsl::vector's.
//     - types that support 'bdlat_SequenceFunctions'.
//     - types that support 'bdlat_ChoiceFunctions'.
//     - simple types that can be parsed by 'balxml_typesparserutil'.
//
// After breathing the component [1], we will first test the internal 'Parser'
// class through the 'Decoder' interface [2].  The purpose of this test will be
// to establish that the parser uses the 'Decoder_ElementContext' protocol
// correctly.  A customized 'TestContext' will be used for this purpose.  Then
// we will test the 'Decoder_SelectContext' meta-function [3] to check that,
// given a particular 'TYPE', the meta-function returns an appropriate context
// type that can be used by the parser to parse that 'TYPE'.
//
// Once we have established that the parser is working correctly, we can start
// testing the context types defined in this component.  The
// 'Decoder_UTF8Context' [4] and 'Decoder_Base64Context' [5] context types are
// tested first, because they are simple and non-templated.  Next, the
// 'Decoder_SimpleContext<TYPE>' [6] template is tested.
//
// The 'Decoder_SequenceContext<TYPE>' [8] and the
// 'Decoder_ChoiceContext<TYPE>' [9] templates make use of the
// 'Decoder_PrepareSubContext' [7] function class.  Therefore, this function
// class must be tested prior to these two class templates.
//
// The 'Decoder_VectorContext<TYPE>' [10] test makes use of the
// 'Decoder_SequenceContext<TYPE>' template, so it must be tested after the
// sequence test.
//
// At this point, the main functionality of this component has been thoroughly
// tested.  Now we need to test the 4 'decode' functions [11] in the 'Decoder'
// namespace.  These tests are trivial and only involve testing that the
// arguments and return values are passed correctly and that the input streams
// are invalidated if there is an error.
//
// Finally, we will test the usage example from the component-level
// documentation to check that it compiles and runs as expected.
//
// Note that the 'Decoder_ErrorReporter' and 'Decoder_ElementContext' protocol
// classes are tested implicitly in all test cases.
// ----------------------------------------------------------------------------
// [11] int balxml::Decoder::decode(sbuf*, TYPE, b_A*);
// [11] int balxml::Decoder::decode(sbuf*, TYPE, ostrm&, ostrm&, b_A*);
// [11] int balxml::Decoder::decode(istrm&, TYPE, b_A*);
// [11] int balxml::Decoder::decode(istrm&, TYPE, ostrm&, ostrm&, b_A*);
// [15] void setNumUnknownElementsSkipped(int value);
// [15] int numUnknownElementsSkipped() const;
// [ 3] balxml::Decoder_SelectContext
// [ 2] baexml_Decoder_ParserUtil
// [ 5] baexml_Decoder_Base64Context
// [ 9] balxml::Decoder_ChoiceContext<TYPE>
// [ 8] balxml::Decoder_SequenceContext<TYPE>
// [ 6] balxml::Decoder_SimpleContext<TYPE>
// [ 4] balxml::Decoder_UTF8Context
// [10] baexml_Decoder_VectorContext<TYPE>
// [ 7] baexml_Decoder_PrepareSubContext
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [16] USAGE EXAMPLES
// ----------------------------------------------------------------------------

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

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

#define XSI "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""

int verbose;
int veryVerbose;
int veryVeryVerbose;
int veryVeryVeryVerbose;

// ============================================================================
//                        GLOBAL CLASSES FOR TESTING
// ----------------------------------------------------------------------------

bool compareEqual(const bsl::string& lhs, const bsl::string& rhs)
{
    LOOP2_ASSERT(lhs.size(), rhs.size(), lhs.size() == rhs.size());
    const bsl::size_t LEN  = lhs.size();
    int               line = 1;
    for (bsl::size_t k = 0; k < LEN; ++k) {
        if (lhs[k] != rhs[k]) {
            LOOP4_ASSERT(line, k, lhs[k], rhs[k], lhs[k] == rhs[k]);
            return false;                                             // RETURN
        }
        if ('\n' == lhs[k]) {
            ++line;
        }
    }
    return true;
}

namespace test {

}  // close namespace test

namespace DECIMALTEST {

using namespace BloombergLP;

                               // =============
                               // class Address
                               // =============

class Address {
    // Address: TBD: Provide annotation

  private:
    bsl::string d_street;
        // street: TBD: Provide annotation
    bsl::string d_city;
        // city: TBD: Provide annotation
    bsl::string d_state;
        // state: TBD: Provide annotation

  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 3 // the number of attributes in this class
    };

    enum {
        ATTRIBUTE_INDEX_STREET = 0,
            // index for "Street" attribute
        ATTRIBUTE_INDEX_CITY = 1,
            // index for "City" attribute
        ATTRIBUTE_INDEX_STATE = 2
            // index for "State" attribute
    };

    enum {
        ATTRIBUTE_ID_STREET = 0,
            // id for "Street" attribute
        ATTRIBUTE_ID_CITY = 1,
            // id for "City" attribute
        ATTRIBUTE_ID_STATE = 2
            // id for "State" attribute
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "Address")

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute

  public:
    // CLASS METHODS
    static const bdlat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.

    static const bdlat_AttributeInfo *lookupAttributeInfo(
                                                       const char *name,
                                                       int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.

    // CREATORS
    explicit Address(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Address' having the default value.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Address(const Address& original, bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Address' having the value of the specified
        // 'original' object.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~Address();
        // Destroy this object.

    // MANIPULATORS
    Address& operator=(const Address& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template <class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template <class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of the
        // (modifiable) attribute indicated by the specified 'id', supplying
        // 'manipulator' with the corresponding attribute information
        // structure.  Return the value returned from the invocation of
        // 'manipulator' if 'id' identifies an attribute of this class, and -1
        // otherwise.

    template <class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of the
        // (modifiable) attribute indicated by the specified 'name' of the
        // specified 'nameLength', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return the value
        // returned from the invocation of 'manipulator' if 'name' identifies
        // an attribute of this class, and -1 otherwise.

    bsl::string& street();
        // Return a reference to the modifiable "Street" attribute of this
        // object.

    bsl::string& city();
        // Return a reference to the modifiable "City" attribute of this
        // object.

    bsl::string& state();
        // Return a reference to the modifiable "State" attribute of this
        // object.

    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  Each line is indented by
        // the absolute value of 'level * spacesPerLevel'.  If 'level' is
        // negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.  Note that a trailing newline is provided
        // in multiline mode only.

    template <class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template <class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template <class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    const bsl::string& street() const;
        // Return a reference to the non-modifiable "Street" attribute of this
        // object.

    const bsl::string& city() const;
        // Return a reference to the non-modifiable "City" attribute of this
        // object.

    const bsl::string& state() const;
        // Return a reference to the non-modifiable "State" attribute of this
        // object.
};

// FREE OPERATORS
inline
bool operator==(const Address& lhs, const Address& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const Address& lhs, const Address& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const Address& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and return a
    // reference to the modifiable 'stream'.

                               // -------------
                               // class Address
                               // -------------

// CONSTANTS
const char Address::CLASS_NAME[] = "Address";
    // the name of this class

const bdlat_AttributeInfo Address::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_STREET,
        "street",                 // name
        sizeof("street") - 1,     // name length
        "street: TBD: Provide annotation",  // annotation
        bdlat_FormattingMode::e_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_CITY,
        "city",                 // name
        sizeof("city") - 1,     // name length
        "city: TBD: Provide annotation",  // annotation
        bdlat_FormattingMode::e_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_STATE,
        "state",                 // name
        sizeof("state") - 1,     // name length
        "state: TBD: Provide annotation",  // annotation
        bdlat_FormattingMode::e_TEXT // formatting mode
    }
};

// CLASS METHODS
const bdlat_AttributeInfo *Address::lookupAttributeInfo(const char *name,
                                                        int         nameLength)
{
    switch (nameLength) {
      case 4: {
        if (bdlb::CharType::toUpper(name[0])=='C'
         && bdlb::CharType::toUpper(name[1])=='I'
         && bdlb::CharType::toUpper(name[2])=='T'
         && bdlb::CharType::toUpper(name[3])=='Y')
        {
            return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CITY];       // RETURN
        }
      } break;
      case 5: {
        if (bdlb::CharType::toUpper(name[0])=='S'
         && bdlb::CharType::toUpper(name[1])=='T'
         && bdlb::CharType::toUpper(name[2])=='A'
         && bdlb::CharType::toUpper(name[3])=='T'
         && bdlb::CharType::toUpper(name[4])=='E')
        {
            return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATE];      // RETURN
        }
      } break;
      case 6: {
        if (bdlb::CharType::toUpper(name[0])=='S'
         && bdlb::CharType::toUpper(name[1])=='T'
         && bdlb::CharType::toUpper(name[2])=='R'
         && bdlb::CharType::toUpper(name[3])=='E'
         && bdlb::CharType::toUpper(name[4])=='E'
         && bdlb::CharType::toUpper(name[5])=='T')
        {
            return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STREET];     // RETURN
        }
      } break;
    }
    return 0;
}

const bdlat_AttributeInfo *Address::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_STREET:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STREET];
      case ATTRIBUTE_ID_CITY:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CITY];
      case ATTRIBUTE_ID_STATE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATE];
      default:
        return 0;
    }
}

// CREATORS
inline
Address::Address(bslma::Allocator *basicAllocator)
: d_street(basicAllocator)
, d_city(basicAllocator)
, d_state(basicAllocator)
{
}

inline
Address::Address(const Address& original, bslma::Allocator *basicAllocator)
: d_street(original.d_street, basicAllocator)
, d_city(original.d_city, basicAllocator)
, d_state(original.d_state, basicAllocator)
{
}

inline
Address::~Address()
{
}

// MANIPULATORS
inline
Address& Address::operator=(const Address& rhs)
{
    if (this != &rhs) {
        d_street = rhs.d_street;
        d_city = rhs.d_city;
        d_state = rhs.d_state;
    }
    return *this;
}

inline
void Address::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_street);
    bdlat_ValueTypeFunctions::reset(&d_city);
    bdlat_ValueTypeFunctions::reset(&d_state);
}

template <class MANIPULATOR>
inline
int Address::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_street, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STREET]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_city, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CITY]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_state, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
inline
int Address::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_STREET:
        return manipulator(&d_street,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STREET]);
      case ATTRIBUTE_ID_CITY:
        return manipulator(&d_city,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CITY]);
      case ATTRIBUTE_ID_STATE:
        return manipulator(&d_state,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATE]);
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
inline
int Address::manipulateAttribute(MANIPULATOR&  manipulator,
                                 const char   *name,
                                 int           nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;                                             // RETURN
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
bsl::string& Address::street()
{
    return d_street;
}

inline
bsl::string& Address::city()
{
    return d_city;
}

inline
bsl::string& Address::state()
{
    return d_state;
}

// ACCESSORS
template <class ACCESSOR>
inline
int Address::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_street, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STREET]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_city, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CITY]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_state, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
inline
int Address::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_STREET:
        return accessor(d_street,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STREET]);
      case ATTRIBUTE_ID_CITY:
        return accessor(d_city, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CITY]);
      case ATTRIBUTE_ID_STATE:
        return accessor(d_state, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATE]);
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
inline
int Address::accessAttribute(ACCESSOR&   accessor,
                             const char *name,
                             int         nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;                                              // RETURN
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

inline
const bsl::string& Address::street() const
{
    return d_street;
}

inline
const bsl::string& Address::city() const
{
    return d_city;
}

inline
const bsl::string& Address::state() const
{
    return d_state;
}

bsl::ostream& Address::print(bsl::ostream& stream,
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
        stream << "Street = ";
        bdlb::PrintMethods::print(stream,
                                  d_street,
                                  -levelPlus1,
                                  spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "City = ";
        bdlb::PrintMethods::print(stream,
                                  d_city,
                                  -levelPlus1,
                                  spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "State = ";
        bdlb::PrintMethods::print(stream,
                                  d_state,
                                  -levelPlus1,
                                  spacesPerLevel);

        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Street = ";
        bdlb::PrintMethods::print(stream,
                                  d_street,
                                  -levelPlus1,
                                  spacesPerLevel);

        stream << ' ';
        stream << "City = ";
        bdlb::PrintMethods::print(stream,
                                  d_city,
                                  -levelPlus1,
                                  spacesPerLevel);

        stream << ' ';
        stream << "State = ";
        bdlb::PrintMethods::print(stream,
                                  d_state,
                                  -levelPlus1,
                                  spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

// FREE OPERATORS
inline
bool operator==(const Address& lhs, const Address& rhs)
{
    return  lhs.street() == rhs.street()
         && lhs.city() == rhs.city()
         && lhs.state() == rhs.state();
}

inline
bool operator!=(const Address& lhs, const Address& rhs)
{
    return  lhs.street() != rhs.street()
         || lhs.city() != rhs.city()
         || lhs.state() != rhs.state();
}

inline
bsl::ostream& operator<<(bsl::ostream& stream, const Address& rhs)
{
    return rhs.print(stream, 0, -1);
}

                              // ================
                              // class Contractor
                              // ================

class Contractor {

  private:
    bsl::string d_name; // todo: provide annotation
    Address d_homeAddress; // todo: provide annotation
    bdldfp::Decimal64 d_hourlyRate; // todo: provide annotation

  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 3 // the number of attributes in this class
    };

    enum {
        ATTRIBUTE_INDEX_NAME = 0,
            // index for "Name" attribute
        ATTRIBUTE_INDEX_HOME_ADDRESS = 1,
            // index for "HomeAddress" attribute
        ATTRIBUTE_INDEX_HOURLY_RATE = 2
            // index for "HourlyRate" attribute
    };

    enum {
        ATTRIBUTE_ID_NAME = 0,
            // id for "Name" attribute
        ATTRIBUTE_ID_HOME_ADDRESS = 1,
            // id for "HomeAddress" attribute
        ATTRIBUTE_ID_HOURLY_RATE = 2
            // id for "HourlyRate" attribute
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "Contractor")

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute

  public:
    // CLASS METHODS
    static const bdlat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.

    static const bdlat_AttributeInfo *lookupAttributeInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.

    // CREATORS
    explicit Contractor(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Contractor' having the default value.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Contractor(const Contractor&  original,
               bslma::Allocator  *basicAllocator = 0);
        // Create an object of type 'Contractor' having the value of the
        // specified 'original' object.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    ~Contractor();
        // Destroy this object.

    // MANIPULATORS
    Contractor& operator=(const Contractor& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of the
        // (modifiable) attribute indicated by the specified 'id', supplying
        // 'manipulator' with the corresponding attribute information
        // structure.  Return the value returned from the invocation of
        // 'manipulator' if 'id' identifies an attribute of this class, and -1
        // otherwise.

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of the
        // (modifiable) attribute indicated by the specified 'name' of the
        // specified 'nameLength', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return the value
        // returned from the invocation of 'manipulator' if 'name' identifies
        // an attribute of this class, and -1 otherwise.

    bsl::string& name();
        // Return a reference to the modifiable "Name" attribute of this
        // object.

    Address& homeAddress();
        // Return a reference to the modifiable "HomeAddress" attribute of
        // this object.

    bdldfp::Decimal64& hourlyRate();
        // Return a reference to the modifiable "HourlyRate" attribute of this
        // object.

    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  Each line is indented by
        // the absolute value of 'level * spacesPerLevel'.  If 'level' is
        // negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.  Note that a trailing newline is provided
        // in multiline mode only.

    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    const bsl::string& name() const;
        // Return a reference to the non-modifiable "Name" attribute of this
        // object.

    const Address& homeAddress() const;
        // Return a reference to the non-modifiable "HomeAddress" attribute of
        // this object.

    const bdldfp::Decimal64& hourlyRate() const;
        // Return a reference to the non-modifiable "HourlyRate" attribute of
        // this object.

};

// FREE OPERATORS
inline
bool operator==(const Contractor& lhs, const Contractor& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const Contractor& lhs, const Contractor& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const Contractor& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and return a
    // reference to the modifiable 'stream'.

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================


                                // ----------------
                                // class Contractor
                                // ----------------

// CONSTANTS
const char Contractor::CLASS_NAME[] = "Contractor";
    // the name of this class

const bdlat_AttributeInfo Contractor::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_NAME,
        "name",                     // name
        sizeof("name") - 1,         // name length
        "todo: provide annotation",// annotation
        bdlat_FormattingMode::e_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_HOME_ADDRESS,
        "homeAddress",                     // name
        sizeof("homeAddress") - 1,         // name length
        "todo: provide annotation",// annotation
        bdlat_FormattingMode::e_DEFAULT // formatting mode
    },
    {
        ATTRIBUTE_ID_HOURLY_RATE,
        "hourlyRate",                     // name
        sizeof("hourlyRate") - 1,         // name length
        "todo: provide annotation",// annotation
        bdlat_FormattingMode::e_DEC // formatting mode
    }
};

// CLASS METHODS
const bdlat_AttributeInfo *Contractor::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 4: {
            if (bdlb::CharType::toUpper(name[0])=='N'
             && bdlb::CharType::toUpper(name[1])=='A'
             && bdlb::CharType::toUpper(name[2])=='M'
             && bdlb::CharType::toUpper(name[3])=='E') {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME];   // RETURN
            }
        } break;
        case 10: {
            if (bdlb::CharType::toUpper(name[0])=='H'
             && bdlb::CharType::toUpper(name[1])=='O'
             && bdlb::CharType::toUpper(name[2])=='U'
             && bdlb::CharType::toUpper(name[3])=='R'
             && bdlb::CharType::toUpper(name[4])=='L'
             && bdlb::CharType::toUpper(name[5])=='Y'
             && bdlb::CharType::toUpper(name[6])=='R'
             && bdlb::CharType::toUpper(name[7])=='A'
             && bdlb::CharType::toUpper(name[8])=='T'
             && bdlb::CharType::toUpper(name[9])=='E') {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOURLY_RATE];
                                                                      // RETURN
            }
        } break;
        case 11: {
            if (bdlb::CharType::toUpper(name[0])=='H'
             && bdlb::CharType::toUpper(name[1])=='O'
             && bdlb::CharType::toUpper(name[2])=='M'
             && bdlb::CharType::toUpper(name[3])=='E'
             && bdlb::CharType::toUpper(name[4])=='A'
             && bdlb::CharType::toUpper(name[5])=='D'
             && bdlb::CharType::toUpper(name[6])=='D'
             && bdlb::CharType::toUpper(name[7])=='R'
             && bdlb::CharType::toUpper(name[8])=='E'
             && bdlb::CharType::toUpper(name[9])=='S'
             && bdlb::CharType::toUpper(name[10])=='S') {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS];
                                                                      // RETURN
            }
        } break;
    }
    return 0;
}

const bdlat_AttributeInfo *Contractor::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_NAME:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME];
      case ATTRIBUTE_ID_HOME_ADDRESS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS];
      case ATTRIBUTE_ID_HOURLY_RATE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOURLY_RATE];
      default:
        return 0;
    }
}

// CREATORS
inline
Contractor::Contractor(bslma::Allocator *basicAllocator)
: d_name(bslma::Default::allocator(basicAllocator))
, d_homeAddress(bslma::Default::allocator(basicAllocator))
, d_hourlyRate()
{
}

inline
Contractor::Contractor(
    const Contractor&   original,
    bslma::Allocator *basicAllocator)
: d_name(original.d_name, bslma::Default::allocator(basicAllocator))
, d_homeAddress(original.d_homeAddress,
                bslma::Default::allocator(basicAllocator))
, d_hourlyRate(original.d_hourlyRate)
{
}

inline
Contractor::~Contractor()
{
}

// MANIPULATORS
inline
Contractor&
Contractor::operator=(const Contractor& rhs)
{
    if (this != &rhs) {
        d_name = rhs.d_name;
        d_homeAddress = rhs.d_homeAddress;
        d_hourlyRate = rhs.d_hourlyRate;
    }
    return *this;
}

inline
void Contractor::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_name);
    bdlat_ValueTypeFunctions::reset(&d_homeAddress);
    bdlat_ValueTypeFunctions::reset(&d_hourlyRate);
}

template <class MANIPULATOR>
inline
int Contractor::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_name,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_homeAddress,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_hourlyRate,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOURLY_RATE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
inline
int Contractor::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_NAME: {
        return manipulator(&d_name,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_HOME_ADDRESS: {
        return manipulator(&d_homeAddress,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_HOURLY_RATE: {
        return manipulator(&d_hourlyRate,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOURLY_RATE]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

template <class MANIPULATOR>
inline
int Contractor::manipulateAttribute(MANIPULATOR&  manipulator,
                                  const char   *name,
                                  int           nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;                                             // RETURN
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
bsl::string& Contractor::name()
{
    return d_name;
}

inline
Address& Contractor::homeAddress()
{
    return d_homeAddress;
}

inline
bdldfp::Decimal64& Contractor::hourlyRate()
{
    return d_hourlyRate;
}

// ACCESSORS
template <class ACCESSOR>
inline
int Contractor::accessAttributes(ACCESSOR& accessor) const
{
   int ret;

    ret = accessor(d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_homeAddress,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_hourlyRate,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOURLY_RATE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
inline
int Contractor::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_NAME: {
        return accessor(d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_HOME_ADDRESS: {
        return accessor(d_homeAddress,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_HOURLY_RATE: {
        return accessor(d_hourlyRate,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOURLY_RATE]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

template <class ACCESSOR>
inline
int Contractor::accessAttribute(ACCESSOR&   accessor,
                              const char *name,
                              int         nameLength) const
{
    enum { NOT_FOUND = -1 };

     const bdlat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
     if (0 == attributeInfo) {
        return NOT_FOUND;                                             // RETURN
     }

     return accessAttribute(accessor, attributeInfo->d_id);
}

inline
const bsl::string& Contractor::name() const
{
    return d_name;
}

inline
const Address& Contractor::homeAddress() const
{
    return d_homeAddress;
}

inline
const bdldfp::Decimal64& Contractor::hourlyRate() const
{
    return d_hourlyRate;
}

bsl::ostream& Contractor::print(
    bsl::ostream& stream,
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
        stream << "Name = ";
        bdlb::PrintMethods::print(stream, d_name,
                             -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "HomeAddress = ";
        bdlb::PrintMethods::print(stream, d_homeAddress,
                             -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Age = ";
        bdlb::PrintMethods::print(stream, d_hourlyRate,
                             -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Name = ";
        bdlb::PrintMethods::print(stream, d_name,
                             -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "HomeAddress = ";
        bdlb::PrintMethods::print(stream, d_homeAddress,
                             -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "HourlyRate = ";
        bdlb::PrintMethods::print(stream, d_hourlyRate,
                             -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

// FREE OPERATORS
inline
bool operator==(const Contractor& lhs, const Contractor& rhs)
{
    return  lhs.name() == rhs.name()
         && lhs.homeAddress() == rhs.homeAddress()
         && lhs.hourlyRate() == rhs.hourlyRate();
}

inline
bool operator!=(const Contractor& lhs, const Contractor& rhs)
{
    return  lhs.name() != rhs.name()
         || lhs.homeAddress() != rhs.homeAddress()
         || lhs.hourlyRate() != rhs.hourlyRate();
}

inline
bsl::ostream& operator<<(bsl::ostream& stream, const Contractor& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace DECIMALTEST

namespace BloombergLP {

// TRAITS
BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(DECIMALTEST::Address)
BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(DECIMALTEST::Contractor)

}  // close enterprise namespace

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    switch (test) { case 0:  // Zero is always the leading case.
      case 21: {
        // --------------------------------------------------------------------
        // Testing Decimal64
        //
        // Concerns:
        //: 1 That the decoder can encoder a field of type 'Decimal64'.
        //
        // Plan:
        //: 1 Copy the 'Employee' type from the usage example, creating a
        //:   'Contractor' type where the integer 'age' field is replaced by
        //:   by a 'Decimal64' 'hourlyRate' field.
        //:
        //: 2 Create a table containing text strings and the expected decimal
        //:   output from parsing the text strings.
        //:
        //: 3 Run the decoder and verify the results.
        //
        // Testing:
        //   Type Decimal64
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing Decimal64\n"
                             "=================\n";

        using namespace BloombergLP;

        namespace TC = DECIMALTEST;

#define DFP(X) BDLDFP_DECIMAL_DD(X)

        static const struct Data {
            int                d_line;
            bdldfp::Decimal64  d_value;
            const char        *d_text;
        } DATA[] = {
            { L_, DFP(0.0), "0.0" },
            { L_, DFP(0.0), "-0.0" },
            { L_, DFP(15.13), "15.13" },
            { L_, DFP(-15.13), "-15.13" },
            { L_, DFP(15.13), "1513e-2" },
            { L_, DFP(-15.13), "-1513e-2" },
            { L_, DFP(892.0), "892.0" },
            { L_, DFP(892.0), "892.0" },
            { L_, DFP(-892.0), "-892.0" },
            { L_, DFP(892.0), "892" },
            { L_, DFP(-892.0), "-892" },
            { L_, DFP(4.73e35), "4.73e+35" },
            { L_, DFP(-4.73e35), "-4.73e+35" },
            { L_, DFP(4.73e35), "473e+33" },
            { L_, DFP(-4.73e35), "-473e+33" }
        };
        enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int ti = 0; ti < k_NUM_DATA; ++ti) {
            const Data&               data  = DATA[ti];
            const int                 LINE  = data.d_line;
            const bdldfp::Decimal64&  VALUE = data.d_value;
            const char               *TEXT  = data.d_text;

            bsl::stringstream inputSs;
            inputSs <<  "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                        "<Contractor>\n"
                        "    <name>Bob</name>\n"
                        "    <homeAddress>\n"
                        "        <street>Some Street</street>\n"
                        "        <city>Some City</city>\n"
                        "        <state>Some State</state>\n"
                        "    </homeAddress>\n"
                        "    <hourlyRate>" << TEXT << "</hourlyRate>\n"
                        "</Contractor>\n";

            TC::Contractor bob;

            balxml::DecoderOptions options;
            balxml::MiniReader     reader;
            balxml::ErrorInfo      errInfo;

            options.setSkipUnknownElements(false);
            balxml::Decoder decoder(&options,
                                    &reader,
                                    &errInfo,
                                    &bsl::cerr,
                                    &bsl::cerr);
            decoder.decode(inputSs, &bob, "contractor.xml");

            ASSERTV(LINE, VALUE, bob.hourlyRate(), VALUE == bob.hourlyRate());
        }
      } break;
      case 20:    BSLA_FALLTHROUGH;
      case 19:    BSLA_FALLTHROUGH;
      case 18:    BSLA_FALLTHROUGH;
      case 17:    BSLA_FALLTHROUGH;
      case 16:    BSLA_FALLTHROUGH;
      case 15:    BSLA_FALLTHROUGH;
      case 14:    BSLA_FALLTHROUGH;
      case 13:    BSLA_FALLTHROUGH;
      case 12:    BSLA_FALLTHROUGH;
      case 11:    BSLA_FALLTHROUGH;
      case 10:    BSLA_FALLTHROUGH;
      case 9:     BSLA_FALLTHROUGH;
      case 8:     BSLA_FALLTHROUGH;
      case 7:     BSLA_FALLTHROUGH;
      case 6:     BSLA_FALLTHROUGH;
      case 5:     BSLA_FALLTHROUGH;
      case 4:     BSLA_FALLTHROUGH;
      case 3:     BSLA_FALLTHROUGH;
      case 2:     BSLA_FALLTHROUGH;
      case 1: {
        ; // do nothing
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
