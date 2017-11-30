// baljsn_formatter.t.cpp                                             -*-C++-*-
#include <baljsn_formatter.h>

#include <balb_testmessages.h>

#include <bslim_testutil.h>

#include <bdlat_attributeinfo.h>
#include <bdlat_choicefunctions.h>
#include <bdlat_enumeratorinfo.h>
#include <bdlat_selectioninfo.h>
#include <bdlat_sequencefunctions.h>
#include <bdlat_valuetypefunctions.h>

#include <bdlb_printmethods.h>  // for printing vector
#include <bdlb_chartype.h>

#include <bdlde_utf8util.h>

#include <bdlsb_fixedmeminstreambuf.h>
#include <bdlsb_memoutstreambuf.h>

// These header are for testing only and the hierarchy level of 'baljsn' was
// increase because of them.  They should be remove when possible.
#include <balxml_decoder.h>
#include <balxml_decoderoptions.h>
#include <balxml_encoder.h>
#include <balxml_encoderoptions.h>
#include <balxml_minireader.h>
#include <balxml_errorinfo.h>

#include <bdlat_typetraits.h>

#include <bdlb_nullablevalue.h>
#include <bdlb_nullableallocatedvalue.h>

#include <bdlt_date.h>
#include <bdlt_datetz.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimetz.h>
#include <bdlt_time.h>
#include <bdlt_timetz.h>

#include <bslma_testallocator.h>

#include <bslmf_assert.h>

#include <bsl_climits.h>
#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_limits.h>
#include <bsl_memory.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_utility.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test implements a formatter for outputting
// 'bdeat'-compatible objects in the JSON format.
//
// We use standard table-based approach to testing where we put both input and
// expected output in the same table row and verify that the actual result
// matches the expected value.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] baljsn::Formatter(bsl::ostream& stream, style, indent, spl);
// [ 2] ~baljsn::Formatter();
//
// MANIPULATORS
// [ 3] void openObject();
// [ 4] void closeObject();
// [ 5] void openArray();
// [ 6] void closeArray();
// [ 7] int openElement();
// [ 8] int putValue(const TYPE& value, const EncoderOptions *options);
// [ 9] void closeElement();
// [10] void indent();
// [11] void setIsArrayElement(bool isArrayElement);
//
// ACCESSORS
// [12] bool isArrayElement() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [13] USAGE EXAMPLE

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

typedef baljsn::Formatter      Obj;
typedef baljsn::EncoderOptions Options;

namespace BloombergLP {
namespace test {

                               // =============
                               // class Address
                               // =============

class Address {
    // Address

  private:
    bsl::string d_street;
        // street
    bsl::string d_city;
        // city
    bsl::string d_state;
        // state

  public:
    // TYPES
    enum {
        k_NUM_ATTRIBUTES = 3 // the number of attributes in this class
    };

    enum {
        k_ATTRIBUTE_INDEX_STREET = 0,
            // index for "Street" attribute
        k_ATTRIBUTE_INDEX_CITY = 1,
            // index for "City" attribute
        k_ATTRIBUTE_INDEX_STATE = 2
            // index for "State" attribute
    };

    enum {
        k_ATTRIBUTE_ID_STREET = 0,
            // id for "Street" attribute
        k_ATTRIBUTE_ID_CITY = 1,
            // id for "City" attribute
        k_ATTRIBUTE_ID_STATE = 2
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
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

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

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    void reset();
        // Reset this object to the default value (i.e., its value upon default
        // construction).

    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the last
        // invocation of 'manipulator' (i.e., the invocation that terminated
        // the sequence).

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

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.  See
        // the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor' with
        // the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the last
        // invocation of 'accessor' (i.e., the invocation that terminated the
        // sequence).

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute of
        // this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute of
        // this object indicated by the specified 'name' of the specified
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

}  // close namespace test

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::Address)

namespace test {

                               // -------------
                               // class Address
                               // -------------

// CONSTANTS

const char Address::CLASS_NAME[] = "Address";
    // the name of this class

const bdlat_AttributeInfo Address::ATTRIBUTE_INFO_ARRAY[] = {
    {
        k_ATTRIBUTE_ID_STREET,
        "street",                 // name
        sizeof("street") - 1,     // name length
        "street",  // annotation
        bdlat_FormattingMode::e_TEXT // formatting mode
    },
    {
        k_ATTRIBUTE_ID_CITY,
        "city",                 // name
        sizeof("city") - 1,     // name length
        "city",  // annotation
        bdlat_FormattingMode::e_TEXT // formatting mode
    },
    {
        k_ATTRIBUTE_ID_STATE,
        "state",                 // name
        sizeof("state") - 1,     // name length
        "state",  // annotation
        bdlat_FormattingMode::e_TEXT // formatting mode
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *Address::lookupAttributeInfo(const char *name,
                                                        int         nameLength)
{
    switch(nameLength) {
        case 4: {
            if (bdlb::CharType::toUpper(name[0])=='C'
             && bdlb::CharType::toUpper(name[1])=='I'
             && bdlb::CharType::toUpper(name[2])=='T'
             && bdlb::CharType::toUpper(name[3])=='Y')
            {
                return &ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_CITY]; // RETURN
            }
        } break;
        case 5: {
            if (bdlb::CharType::toUpper(name[0])=='S'
             && bdlb::CharType::toUpper(name[1])=='T'
             && bdlb::CharType::toUpper(name[2])=='A'
             && bdlb::CharType::toUpper(name[3])=='T'
             && bdlb::CharType::toUpper(name[4])=='E')
            {
                return &ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_STATE];
                                                                      // RETURN
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
                return &ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_STREET];
                                                                      // RETURN
            }
        } break;
    }
    return 0;
}

const bdlat_AttributeInfo *Address::lookupAttributeInfo(int id)
{
    switch (id) {
      case k_ATTRIBUTE_ID_STREET:
        return &ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_STREET];
      case k_ATTRIBUTE_ID_CITY:
        return &ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_CITY];
      case k_ATTRIBUTE_ID_STATE:
        return &ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_STATE];
      default:
        return 0;
    }
}

// ACCESSORS

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
        bdlb::PrintMethods::print(stream, d_street,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "City = ";
        bdlb::PrintMethods::print(stream, d_city,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "State = ";
        bdlb::PrintMethods::print(stream, d_state,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Street = ";
        bdlb::PrintMethods::print(stream, d_street,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "City = ";
        bdlb::PrintMethods::print(stream, d_city,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "State = ";
        bdlb::PrintMethods::print(stream, d_state,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

// CLASS METHODS
inline
int Address::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
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
Address::Address(
        const Address&    original,
        bslma::Allocator *basicAllocator)
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
Address&
Address::operator=(const Address& rhs)
{
    if (this != &rhs) {
        d_street = rhs.d_street;
        d_city = rhs.d_city;
        d_state = rhs.d_state;
    }
    return *this;
}

template <class STREAM>
inline
STREAM& Address::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            stream.getString(d_street);
            stream.getString(d_city);
            stream.getString(d_state);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
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

    ret = manipulator(&d_street,
                      ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_STREET]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_city, ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_CITY]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_state, ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_STATE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
inline
int Address::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { e_NOT_FOUND = -1 };

    switch (id) {
      case k_ATTRIBUTE_ID_STREET: {
        return manipulator(&d_street,
                           ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_STREET]);
                                                                      // RETURN
      } break;
      case k_ATTRIBUTE_ID_CITY: {
        return manipulator(&d_city,
                           ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_CITY]);
                                                                      // RETURN
      } break;
      case k_ATTRIBUTE_ID_STATE: {
        return manipulator(&d_state,
                           ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_STATE]);
                                                                      // RETURN
      } break;
      default:
        return e_NOT_FOUND;                                           // RETURN
    }
}

template <class MANIPULATOR>
inline
int Address::manipulateAttribute(MANIPULATOR&  manipulator,
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
template <class STREAM>
inline
STREAM& Address::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        stream.putString(d_street);
        stream.putString(d_city);
        stream.putString(d_state);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
inline
int Address::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_street, ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_STREET]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_city, ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_CITY]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_state, ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_STATE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
inline
int Address::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { e_NOT_FOUND = -1 };

    switch (id) {
      case k_ATTRIBUTE_ID_STREET: {
        return accessor(d_street,
                        ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_STREET]);
                                                                      // RETURN
      } break;
      case k_ATTRIBUTE_ID_CITY: {
        return accessor(d_city, ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_CITY]);
                                                                      // RETURN
      } break;
      case k_ATTRIBUTE_ID_STATE: {
        return accessor(d_state,
                        ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_STATE]);
                                                                      // RETURN
      } break;
      default:
        return e_NOT_FOUND;                                           // RETURN
    }
}

template <class ACCESSOR>
inline
int Address::accessAttribute(ACCESSOR&   accessor,
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

}  // close namespace test

namespace test {

                               // ==============
                               // class Employee
                               // ==============

class Employee {
    // Employee

  private:
    bsl::string d_name;
        // name
    Address d_homeAddress;
        // homeAddress
    int d_age;
        // age

  public:
    // TYPES
    enum {
        k_NUM_ATTRIBUTES = 3 // the number of attributes in this class
    };

    enum {
        k_ATTRIBUTE_INDEX_NAME = 0,
            // index for "Name" attribute
        k_ATTRIBUTE_INDEX_HOME_ADDRESS = 1,
            // index for "HomeAddress" attribute
        k_ATTRIBUTE_INDEX_AGE = 2
            // index for "Age" attribute
    };

    enum {
        k_ATTRIBUTE_ID_NAME = 0,
            // id for "Name" attribute
        k_ATTRIBUTE_ID_HOME_ADDRESS = 1,
            // id for "HomeAddress" attribute
        k_ATTRIBUTE_ID_AGE = 2
            // id for "Age" attribute
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "Employee")

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

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
    explicit Employee(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Employee' having the default value.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Employee(const Employee& original, bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Employee' having the value of the
        // specified 'original' object.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    ~Employee();
        // Destroy this object.

    // MANIPULATORS
    Employee& operator=(const Employee& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    void reset();
        // Reset this object to the default value (i.e., its value upon default
        // construction).

    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the last
        // invocation of 'manipulator' (i.e., the invocation that terminated
        // the sequence).

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
        // Return a reference to the modifiable "HomeAddress" attribute of this
        // object.

    int& age();
        // Return a reference to the modifiable "Age" attribute of this object.

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

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.  See
        // the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor' with
        // the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the last
        // invocation of 'accessor' (i.e., the invocation that terminated the
        // sequence).

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute of
        // this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute of
        // this object indicated by the specified 'name' of the specified
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

    const int& age() const;
        // Return a reference to the non-modifiable "Age" attribute of this
        // object.
};

// FREE OPERATORS
inline
bool operator==(const Employee& lhs, const Employee& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const Employee& lhs, const Employee& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const Employee& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and return a
    // reference to the modifiable 'stream'.

}  // close namespace test

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::Employee)

namespace test {

                               // --------------
                               // class Employee
                               // --------------

// CONSTANTS

const char Employee::CLASS_NAME[] = "Employee";
    // the name of this class

const bdlat_AttributeInfo Employee::ATTRIBUTE_INFO_ARRAY[] = {
    {
        k_ATTRIBUTE_ID_NAME,
        "name",                 // name
        sizeof("name") - 1,     // name length
        "name",  // annotation
        bdlat_FormattingMode::e_TEXT // formatting mode
    },
    {
        k_ATTRIBUTE_ID_HOME_ADDRESS,
        "homeAddress",                 // name
        sizeof("homeAddress") - 1,     // name length
        "homeAddress",  // annotation
        bdlat_FormattingMode::e_DEFAULT // formatting mode
    },
    {
        k_ATTRIBUTE_ID_AGE,
        "age",                 // name
        sizeof("age") - 1,     // name length
        "age",  // annotation
        bdlat_FormattingMode::e_DEC // formatting mode
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *Employee::lookupAttributeInfo(
                                                        const char *name,
                                                        int         nameLength)
{
    switch(nameLength) {
        case 3: {
            if (bdlb::CharType::toUpper(name[0])=='A'
             && bdlb::CharType::toUpper(name[1])=='G'
             && bdlb::CharType::toUpper(name[2])=='E')
            {
                return &ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_AGE];  // RETURN
            }
        } break;
        case 4: {
            if (bdlb::CharType::toUpper(name[0])=='N'
             && bdlb::CharType::toUpper(name[1])=='A'
             && bdlb::CharType::toUpper(name[2])=='M'
             && bdlb::CharType::toUpper(name[3])=='E')
            {
                return &ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_NAME]; // RETURN
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
             && bdlb::CharType::toUpper(name[10])=='S')
            {
                return &ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_HOME_ADDRESS];
                                                                      // RETURN
            }
        } break;
    }
    return 0;
}

const bdlat_AttributeInfo *Employee::lookupAttributeInfo(int id)
{
    switch (id) {
      case k_ATTRIBUTE_ID_NAME:
        return &ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_NAME];
      case k_ATTRIBUTE_ID_HOME_ADDRESS:
        return &ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_HOME_ADDRESS];
      case k_ATTRIBUTE_ID_AGE:
        return &ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_AGE];
      default:
        return 0;
    }
}

// ACCESSORS

bsl::ostream& Employee::print(bsl::ostream& stream,
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
        bdlb::PrintMethods::print(stream, d_age,
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
        stream << "Age = ";
        bdlb::PrintMethods::print(stream, d_age,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}
// CLASS METHODS
inline
int Employee::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// CREATORS
inline
Employee::Employee(bslma::Allocator *basicAllocator)
: d_name(basicAllocator)
, d_homeAddress(basicAllocator)
, d_age()
{
}

inline
Employee::Employee(
        const Employee&   original,
        bslma::Allocator *basicAllocator)
: d_name(original.d_name, basicAllocator)
, d_homeAddress(original.d_homeAddress, basicAllocator)
, d_age(original.d_age)
{
}

inline
Employee::~Employee()
{
}

// MANIPULATORS
inline
Employee&
Employee::operator=(const Employee& rhs)
{
    if (this != &rhs) {
        d_name = rhs.d_name;
        d_homeAddress = rhs.d_homeAddress;
        d_age = rhs.d_age;
    }
    return *this;
}

template <class STREAM>
inline
STREAM& Employee::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            stream.getString(d_name);
            d_homeAddress.bdexStreamIn(stream, 1);
            stream.getInt32(d_age);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

inline
void Employee::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_name);
    bdlat_ValueTypeFunctions::reset(&d_homeAddress);
    bdlat_ValueTypeFunctions::reset(&d_age);
}

template <class MANIPULATOR>
inline
int Employee::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_name, ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_NAME]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_homeAddress,
                      ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_HOME_ADDRESS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_age, ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_AGE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
inline
int Employee::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { e_NOT_FOUND = -1 };

    switch (id) {
      case k_ATTRIBUTE_ID_NAME: {
        return manipulator(&d_name,
                           ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_NAME]);
                                                                      // RETURN
      } break;
      case k_ATTRIBUTE_ID_HOME_ADDRESS: {
        return manipulator(
                         &d_homeAddress,
                         ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_HOME_ADDRESS]);
                                                                      // RETURN
      } break;
      case k_ATTRIBUTE_ID_AGE: {
        return manipulator(&d_age,
                           ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_AGE]);
                                                                      // RETURN
      } break;
      default:
        return e_NOT_FOUND;                                           // RETURN
    }
}

template <class MANIPULATOR>
inline
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
Address& Employee::homeAddress()
{
    return d_homeAddress;
}

inline
int& Employee::age()
{
    return d_age;
}

// ACCESSORS
template <class STREAM>
inline
STREAM& Employee::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        stream.putString(d_name);
        d_homeAddress.bdexStreamOut(stream, 1);
        stream.putInt32(d_age);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
inline
int Employee::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_name, ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_NAME]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_homeAddress,
                   ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_HOME_ADDRESS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_age, ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_AGE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
inline
int Employee::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { e_NOT_FOUND = -1 };

    switch (id) {
      case k_ATTRIBUTE_ID_NAME: {
        return accessor(d_name, ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_NAME]);
                                                                      // RETURN
      } break;
      case k_ATTRIBUTE_ID_HOME_ADDRESS: {
        return accessor(d_homeAddress,
                        ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_HOME_ADDRESS]);
                                                                      // RETURN
      } break;
      case k_ATTRIBUTE_ID_AGE: {
        return accessor(d_age, ATTRIBUTE_INFO_ARRAY[k_ATTRIBUTE_INDEX_AGE]);
                                                                      // RETURN
      } break;
      default:
        return e_NOT_FOUND;                                           // RETURN
    }
}

template <class ACCESSOR>
inline
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
const Address& Employee::homeAddress() const
{
    return d_homeAddress;
}

inline
const int& Employee::age() const
{
    return d_age;
}

}  // close namespace test

}  // close enterprise namespace


Obj g(bsl::ostream& os, int style, int indent, int spl)
{
    if (-1 != style) {
        if (-1 != indent) {
            if (-1 != spl) {
                return Obj(os, style, indent, spl);
            }
            else {
                return Obj(os, style, indent);
            }
        }
        else {
            return Obj(os, style);
        }
    }
    else {
        return Obj(os);
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

    (void)veryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 13: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
// Let us say that we have to encode a JSON document with the following
// information about stocks that we are interested in.  For brevity we just
// show and encode a part of the complete document.
//..
//  {
//      "Stocks" : [
//        {
//          "Name" : "International Business Machines Corp",
//          "Ticker" : "IBM US Equity",
//          "Last Price" : 149.3,
//          "Dividend Yield" : 3.95
//       },
//       ...
//    ]
//  }
//..
// To encode this JSON document we first create a 'baljsn::Formatter' object.
// Since we want the document to be written in a pretty, easy to understand
// format we will specify the 'true' for the 'usePrettyStyle' option and
// provide an appropriate initial indent level and spaces per level values:
//..
    bsl::ostringstream os;
    baljsn::Formatter formatter(os, true, 0, 2);
//..
// Next, we start calling the sequence of methods requires to produce this
// document.  We start with the top level object and add an element named
// 'Stocks' to it:
//..
    formatter.openObject();
    formatter.openElement("Stocks");
//..
// Then, we see that 'Stocks' is an array element so we specify the start of
// the array:
//..
    formatter.openArray();
//..
// Next, each element within 'Stocks' is an object that contains the
// information for an individual stock.  So we have to output an object here.
// However, there is an additional step that the 'Formatter' 'class' requires.
// When writing data in pretty style the 'isArrayElement' flag is required to
// be set before encoding elements.  Setting this flag allows the formatter to
// correctly distinguish between elements that are part of an object and those
// that are part of an array and encode them appropriately.
//..
    formatter.setIsArrayElement(true);
    formatter.openObject();
//..
// We now revert back the 'isArrayElement' flag as we start encoding elements
// in an object.  Note that the 'isArrayElement' flag is only relevant if the
// pretty encoding style is used.  Otherwise, users can safely ignore that
// flag.
//..
    formatter.setIsArrayElement(false);
//..
// We now encode the other elements in the stock object.  The 'closeElement'
// terminates the element by adding a ',' at the end.  For the last element in
// an object do not call the 'closeElement' method.
//..
    formatter.openElement("Name");
    formatter.putValue("International Business Machines Corp");
    formatter.closeElement();
//
    formatter.openElement("Ticker");
    formatter.putValue("IBM US Equity");
    formatter.closeElement();
//
    formatter.openElement("Last Price");
    formatter.putValue(149.3);
    formatter.closeElement();
//
    formatter.openElement("Divident Yield");
    formatter.putValue(3.95);
    // Note no call to 'closeElement' for the last element
//..
// Similarly, we can continue to format the rest of the document.  For the
// purpose of this usage we will complete this document.
//..
    formatter.closeObject();
    formatter.closeArray();
    formatter.closeObject();
//..
// Once the formatting is complete the written data can be viewed from the
//  stream passed to the formatter at construction.
//..
    if (verbose)
    bsl::cout << os.str() << bsl::endl;
//..
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'closeElement' METHOD
        //
        // Concerns:
        //: 1 The 'closeElement' method outputs a ','.
        //:
        //: 2 If pretty style is selected then 'closeElement' outputs a newline
        //:   after the ','.
        //:
        // Plan:
        //: 1 Using a table-based approach specify the encoding style,
        //:   indentation level, spaces per level, is array element, number of
        //:   times 'closeElement' must be called and the expected output
        //:   after calling 'closeElement'.  Create a formatter object using
        //:   the specified parameters and invoke 'closeElement' on it.  Verify
        //:   that the output written to the stream is as expected.
        //
        // Testing:
        //   void closeElement();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'closeElement' METHOD" << endl
                          << "=============================" << endl;
#define NL "\n"

        // Use a value of -1 to signify that that specific option should not be
        // set and that its default value should be used.

        const struct Data {
            int         d_line;                // source line number
            int         d_encodingStyle;
            int         d_initialIndentLevel;
            int         d_spacesPerLevel;
            bool        d_isArrayElement;
            int         d_numTimesCloseArrayCalled;
            bsl::string d_expected;
        } DATA[] = {

        // LINE  STYLE  INDENT   SPL    IAE    NT   EXPECTED
        // ----  -----  ------   ---    ---    ---  -------

        {   L_,    -1,     -1,   -1,  false,    0,       ","             },
        {   L_,    -1,     -1,   -1,  true,     0,       ","             },

        {   L_,     0,     -1,   -1,  false,    0,       ","             },
        {   L_,     0,     -1,   -1,  true,     0,       ","             },

        {   L_,     1,      2,    2,  false,    0,       ","        NL   },
        {   L_,     1,      2,    2,  true,     0,       ","        NL   },

        {   L_,    -1,     -1,   -1,  false,    3,       ",,,,"          },
        {   L_,    -1,     -1,   -1,  true,     3,       ",,,,"          },

        {   L_,     0,     -1,   -1,  false,    3,       ",,,,"          },
        {   L_,     0,     -1,   -1,  true,     3,       ",,,,"          },

        {   L_,     1,      5,    2,  false,    3,       ","        NL
                                                         ","        NL
                                                         ","        NL
                                                         ","        NL
                                                                         },
        {   L_,     1,      5,    2,  true,     3,       ","        NL
                                                         ","        NL
                                                         ","        NL
                                                         ","        NL
                                                                         },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int         LINE   = DATA[i].d_line;
            const int         STYLE  = DATA[i].d_encodingStyle;
            const int         INDENT = DATA[i].d_initialIndentLevel;
            const int         SPL    = DATA[i].d_spacesPerLevel;
            const bool        IAE    = DATA[i].d_isArrayElement;
            const int         NT     = DATA[i].d_numTimesCloseArrayCalled;
            const bsl::string EXP    = DATA[i].d_expected;

            bsl::ostringstream     os;

            Obj mX = g(os, STYLE, INDENT, SPL);  const Obj& X = mX;

            mX.setIsArrayElement(IAE);

            for (int k = 0; k < NT; ++k) mX.closeElement();

            mX.closeElement();

            os << bsl::flush;

            ASSERTV(LINE, EXP, os.str(), EXP == os.str());
        }
#undef NL
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'openElement' METHOD
        //
        // Concerns:
        //: 1 The 'openElement' method outputs a the name of the element
        //:   followed by ':'.
        //:
        //: 2 If pretty style is selected then 'openElement' indents before
        //:   printing element name and outputs the ':' character with spaces
        //:   around it.
        //:
        //: 3 Providing an invalid (non-UTF8) element name results in a
        //:   non-zero value being returned.  Otherwise 0 is returned.
        //:
        // Plan:
        //: 1 Using a table-based approach specify the encoding style,
        //:   indentation level, spaces per level, is array element, element
        //:   name, expected return value, and the expected output after
        //:   calling 'openElement'.  Create a formatter object using the
        //:   specified parameters and invoke 'openElement' on it.  Verify that
        //:   the output written to the stream is as expected.
        //
        // Testing:
        //   void openElement(const bsl::string& name);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'openElement' METHOD" << endl
                          << "============================" << endl;
#define NL "\n"

        // Use a value of -1 to signify that that specific option should not be
        // set and that its default value should be used.

        const struct Data {
            int         d_line;                // source line number
            int         d_encodingStyle;
            int         d_initialIndentLevel;
            int         d_spacesPerLevel;
            int         d_isArrayElement;
            bsl::string d_elementName;
            int         d_expRetCode;
            bsl::string d_expected;
        } DATA[] = {
     // LINE  STYLE  INDENT   SPL   IAE    NAME    EXP_RC   EXPECTED
     // ----  -----  ------   ---   ---    ----    ------   -------

     {   L_,    -1,     -1,   -1,    -1,   "",         0,   "\"\":"        },
     {   L_,    -1,     -1,   -1,    -1,   "name",     0,   "\"name\":"    },

     {   L_,     0,     -1,   -1, false,   "",         0,   "\"\":"        },
     {   L_,     0,     -1,   -1,  true,   "",         0,   "\"\":"        },

        {   L_,     0,     -1,   -1,  false,    0,  "["                 },
        {   L_,     0,     -1,   -1,  true,     0,  "["                 },

        // {   L_,     1,      1,    2,  false,    0,  "["             NL  },
        // {   L_,     1,      1,    2,  true,     0,  "["                 },

        // {   L_,    -1,     -1,   -1,  false,    3,  "[[[["              },
        // {   L_,    -1,     -1,   -1,  true,     3,  "[[[["              },

        // {   L_,     0,     -1,   -1,  false,    3,  "[[[["              },
        // {   L_,     0,     -1,   -1,  true,     3,  "[[[["              },

        // {   L_,     1,      1,    2,  false,    3,  "["             NL
        //                                             "["             NL
        //                                             "["             NL
        //                                             "["             NL
        //                                                                 },
        // {   L_,     1,      1,    2,  true,     3,  "[[[["              },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int         LINE   = DATA[i].d_line;
            const int         STYLE  = DATA[i].d_encodingStyle;
            const int         INDENT = DATA[i].d_initialIndentLevel;
            const int         SPL    = DATA[i].d_spacesPerLevel;
            const int         IAE    = DATA[i].d_isArrayElement;
            const bsl::string NAME   = DATA[i].d_elementName;
            const int         EXP_RC = DATA[i].d_expRetCode;
            const bsl::string EXP    = DATA[i].d_expected;

            bsl::ostringstream     os;

            Obj mX = g(os, STYLE, INDENT, SPL);  const Obj& X = mX;

            mX.setIsArrayElement(IAE);

            const int rc = mX.openElement(NAME);

            os << bsl::flush;

            ASSERTV(LINE, EXP_RC, rc, EXP_RC == rc);
            ASSERTV(LINE, EXP, os.str(), EXP == os.str());
        }
#undef NL
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'closeArray' METHOD
        //
        // Concerns:
        //: 1 The 'closeArray' method outputs a ']'.
        //:
        //: 2 If pretty style is selected then 'closeArray' indents before
        //:   printing ']'.
        //:
        //: 3 Each invocation of 'closeArray' decreases the indent level.
        //:
        // Plan:
        //: 1 Using a table-based approach specify the encoding style,
        //:   indentation level, spaces per level, is array element, number of
        //:   times 'closeArray' must be called and the expected output
        //:   after calling 'closeArray'.  Create a formatter object using the
        //:   specified parameters and invoke 'closeArray' on it.  Verify that
        //:   the output written to the stream is as expected.
        //
        // Testing:
        //   void closeArray();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'closeArray' METHOD" << endl
                          << "============================" << endl;
#define NL "\n"

        // Use a value of -1 to signify that that specific option should not be
        // set and that its default value should be used.

        const struct Data {
            int         d_line;                // source line number
            int         d_encodingStyle;
            int         d_initialIndentLevel;
            int         d_spacesPerLevel;
            bool        d_formatAsEmptyArray;
            int         d_numTimesCloseArrayCalled;
            bsl::string d_expected;
        } DATA[] = {

        // LINE  STYLE  INDENT   SPL    FAEA    NT   EXPECTED
        // ----  -----  ------   ---    ----    ---  -------

        {   L_,    -1,     -1,   -1,  false,    0,       "]"             },
        {   L_,    -1,     -1,   -1,  true,     0,       "]"             },

        {   L_,     0,     -1,   -1,  false,    0,       "]"             },
        {   L_,     0,     -1,   -1,  true,     0,       "]"             },

        {   L_,     1,      2,    2,  false,    0,  NL   "  ]"           },
        {   L_,     1,      2,    2,  true,     0,       "]"             },

        {   L_,    -1,     -1,   -1,  false,    3,       "]]]]"          },
        {   L_,    -1,     -1,   -1,  true,     3,       "]]]]"          },

        {   L_,     0,     -1,   -1,  false,    3,       "]]]]"          },
        {   L_,     0,     -1,   -1,  true,     3,       "]]]]"          },

        {   L_,     1,      5,    2,  false,    3,  NL   "        ]"
                                                    NL   "      ]"
                                                    NL   "    ]"
                                                    NL   "  ]"
                                                                        },
        {   L_,     1,      5,    2,  true,     3,       "]]]]"         },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int         LINE   = DATA[i].d_line;
            const int         STYLE  = DATA[i].d_encodingStyle;
            const int         INDENT = DATA[i].d_initialIndentLevel;
            const int         SPL    = DATA[i].d_spacesPerLevel;
            const bool        FAEA   = DATA[i].d_formatAsEmptyArray;
            const int         NT     = DATA[i].d_numTimesCloseArrayCalled;
            const bsl::string EXP    = DATA[i].d_expected;

            bsl::ostringstream     os;

            Obj mX = g(os, STYLE, INDENT, SPL);  const Obj& X = mX;

            for (int k = 0; k < NT; ++k) mX.closeArray(FAEA);

            mX.closeArray(FAEA);

            os << bsl::flush;

            ASSERTV(LINE, EXP, os.str(), EXP == os.str());
        }
#undef NL
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'openArray' METHOD
        //
        // Concerns:
        //: 1 The 'openArray' method outputs a '['.
        //:
        //: 2 If pretty style is selected then 'openArray' indents before
        //:   printing '[' only if the 'isArrayElement' flag is true.
        //:
        //: 3 Each invocation of 'openArray' increases the indent level.
        //:
        // Plan:
        //: 1 Using a table-based approach specify the encoding style,
        //:   indentation level, spaces per level, is array element, number of
        //:   times 'openArray' must be called and the expected output
        //:   after calling 'openArray'.  Create a formatter object using the
        //:   specified parameters and invoke 'openArray' on it.  Verify that
        //:   the output written to the stream is as expected.
        //
        // Testing:
        //   void openArray();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'openArray' METHOD" << endl
                          << "===========================" << endl;
#define NL "\n"

        // Use a value of -1 to signify that that specific option should not be
        // set and that its default value should be used.

        const struct Data {
            int         d_line;                // source line number
            int         d_encodingStyle;
            int         d_initialIndentLevel;
            int         d_spacesPerLevel;
            bool        d_formatAsEmptyArray;
            int         d_numTimesOpenArrayCalled;
            bsl::string d_expected;
        } DATA[] = {
        // LINE  STYLE  INDENT   SPL    FAEA    NT  EXPECTED
        // ----  -----  ------   ---    ----   ---  --------

        {   L_,    -1,     -1,   -1,  false,    0,  "["                 },
        {   L_,    -1,     -1,   -1,  true,     0,  "["                 },

        {   L_,     0,     -1,   -1,  false,    0,  "["                 },
        {   L_,     0,     -1,   -1,  true,     0,  "["                 },

        {   L_,     1,      1,    2,  false,    0,  "["             NL  },
        {   L_,     1,      1,    2,  true,     0,  "["                 },

        {   L_,    -1,     -1,   -1,  false,    3,  "[[[["              },
        {   L_,    -1,     -1,   -1,  true,     3,  "[[[["              },

        {   L_,     0,     -1,   -1,  false,    3,  "[[[["              },
        {   L_,     0,     -1,   -1,  true,     3,  "[[[["              },

        {   L_,     1,      1,    2,  false,    3,  "["             NL
                                                    "["             NL
                                                    "["             NL
                                                    "["             NL
                                                                        },
        {   L_,     1,      1,    2,  true,     3,  "[[[["              },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int         LINE   = DATA[i].d_line;
            const int         STYLE  = DATA[i].d_encodingStyle;
            const int         INDENT = DATA[i].d_initialIndentLevel;
            const int         SPL    = DATA[i].d_spacesPerLevel;
            const bool        FAEA   = DATA[i].d_formatAsEmptyArray;
            const int         NT     = DATA[i].d_numTimesOpenArrayCalled;
            const bsl::string EXP    = DATA[i].d_expected;

            bsl::ostringstream     os;

            Obj mX = g(os, STYLE, INDENT, SPL);  const Obj& X = mX;

            for (int k = 0; k < NT; ++k) mX.openArray(FAEA);

            mX.openArray(FAEA);

            os << bsl::flush;

            ASSERTV(LINE, EXP, os.str(), EXP == os.str());
        }
#undef NL
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'closeObject' METHOD
        //
        // Concerns:
        //: 1 The 'closeObject' method outputs a '}'.
        //:
        //: 2 If pretty style is selected then 'closeObject' indents before
        //:   printing '}'.
        //:
        //: 3 Each invocation of 'closeObject' decreases the indent level.
        //:
        // Plan:
        //: 1 Using a table-based approach specify the encoding style,
        //:   indentation level, spaces per level, is array element, number of
        //:   times 'closeObject' must be called and the expected output
        //:   after calling 'closeObject'.  Create a formatter object using the
        //:   specified parameters and invoke 'closeObject' on it.  Verify that
        //:   the output written to the stream is as expected.
        //
        // Testing:
        //   void closeObject();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'closeObject' METHOD" << endl
                          << "============================" << endl;
#define NL "\n"

        // Use a value of -1 to signify that that specific option should not be
        // set and that its default value should be used.

        const struct Data {
            int         d_line;                // source line number
            int         d_encodingStyle;
            int         d_initialIndentLevel;
            int         d_spacesPerLevel;
            bool        d_isArrayElement;
            int         d_numTimesCloseObjectCalled;
            bsl::string d_expected;
        } DATA[] = {

        // LINE  STYLE  INDENT   SPL    IAE    NT   EXPECTED
        // ----  -----  ------   ---    ---    ---  -------

        {   L_,    -1,     -1,   -1,  false,    0,       "}"             },
        {   L_,    -1,     -1,   -1,  true,     0,       "}"             },

        {   L_,     0,     -1,   -1,  false,    0,       "}"             },
        {   L_,     0,     -1,   -1,  true,     0,       "}"             },

        {   L_,     1,      2,    2,  false,    0,  NL   "  }"           },
        {   L_,     1,      2,    2,  true,     0,  NL   "  }"           },

        {   L_,    -1,     -1,   -1,  false,    3,       "}}}}"          },
        {   L_,    -1,     -1,   -1,  true,     3,       "}}}}"          },

        {   L_,     0,     -1,   -1,  false,    3,       "}}}}"          },
        {   L_,     0,     -1,   -1,  true,     3,       "}}}}"          },

        {   L_,     1,      5,    2,  false,    3,  NL   "        }"
                                                    NL   "      }"
                                                    NL   "    }"
                                                    NL   "  }"
                                                                        },
        {   L_,     1,      5,    2,  true,     3,  NL   "        }"
                                                    NL   "      }"
                                                    NL   "    }"
                                                    NL   "  }"
                                                                        },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int         LINE   = DATA[i].d_line;
            const int         STYLE  = DATA[i].d_encodingStyle;
            const int         INDENT = DATA[i].d_initialIndentLevel;
            const int         SPL    = DATA[i].d_spacesPerLevel;
            const bool        IAE    = DATA[i].d_isArrayElement;
            const int         NT     = DATA[i].d_numTimesCloseObjectCalled;
            const bsl::string EXP    = DATA[i].d_expected;

            bsl::ostringstream     os;

            Obj mX = g(os, STYLE, INDENT, SPL);  const Obj& X = mX;

            mX.setIsArrayElement(IAE);

            for (int k = 0; k < NT; ++k) mX.closeObject();

            mX.closeObject();

            os << bsl::flush;

            ASSERTV(LINE, EXP, os.str(), EXP == os.str());
        }
#undef NL
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'openObject' METHOD
        //
        // Concerns:
        //: 1 The 'openObject' method outputs a '{'.
        //:
        //: 2 If pretty style is selected then 'openObject' indents before
        //:   printing '{' only if the 'isArrayElement' flag is true.
        //:
        //: 3 Each invocation of 'openObject' increases the indent level.
        //:
        // Plan:
        //: 1 Using a table-based approach specify the encoding style,
        //:   indentation level, spaces per level, is array element, number of
        //:   times 'openObject' must be called and the expected output
        //:   after calling 'openObject'.  Create a formatter object using the
        //:   specified parameters and invoke 'openObject' on it.  Verify that
        //:   the output written to the stream is as expected.
        //
        // Testing:
        //   void openObject();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'openObject' METHOD" << endl
                          << "===========================" << endl;
#define NL "\n"

        // Use a value of -1 to signify that that specific option should not be
        // set and that its default value should be used.

        const struct Data {
            int         d_line;                // source line number
            int         d_encodingStyle;
            int         d_initialIndentLevel;
            int         d_spacesPerLevel;
            bool        d_isArrayElement;
            int         d_numTimesOpenObjectCalled;
            bsl::string d_expected;
        } DATA[] = {

        // LINE  STYLE  INDENT   SPL    IAE    NT   EXPECTED
        // ----  -----  ------   ---    ---    ---  -------

        {   L_,    -1,     -1,   -1,  false,    0,  "{"                 },
        {   L_,    -1,     -1,   -1,  true,     0,  "{"                 },

        {   L_,     0,     -1,   -1,  false,    0,  "{"                 },
        {   L_,     0,     -1,   -1,  true,     0,  "{"                 },

        {   L_,     1,      1,    2,  false,    0,  "{"             NL  },
        {   L_,     1,      1,    2,  true,     0,  "  {"           NL  },

        {   L_,    -1,     -1,   -1,  false,    3,  "{{{{"              },
        {   L_,    -1,     -1,   -1,  true,     3,  "{{{{"              },

        {   L_,     0,     -1,   -1,  false,    3,  "{{{{"              },
        {   L_,     0,     -1,   -1,  true,     3,  "{{{{"              },

        {   L_,     1,      1,    2,  false,    3,  "{"             NL
                                                    "{"             NL
                                                    "{"             NL
                                                    "{"             NL
                                                                        },
        {   L_,     1,      1,    2,  true,     3,  "  {"           NL
                                                    "    {"         NL
                                                    "      {"       NL
                                                    "        {"     NL
                                                                        },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int         LINE   = DATA[i].d_line;
            const int         STYLE  = DATA[i].d_encodingStyle;
            const int         INDENT = DATA[i].d_initialIndentLevel;
            const int         SPL    = DATA[i].d_spacesPerLevel;
            const bool        IAE    = DATA[i].d_isArrayElement;
            const int         NT     = DATA[i].d_numTimesOpenObjectCalled;
            const bsl::string EXP    = DATA[i].d_expected;

            bsl::ostringstream     os;

            Obj mX = g(os, STYLE, INDENT, SPL);  const Obj& X = mX;

            mX.setIsArrayElement(IAE);

            for (int k = 0; k < NT; ++k) mX.openObject();

            mX.openObject();

            os << bsl::flush;

            ASSERTV(LINE, EXP, os.str(), EXP == os.str());
        }
#undef NL
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CREATORS
        //
        // Concerns:
        //: 1 The specified stream is correctly initialized.
        //:
        //: 2 The specified encoder options are correctly initialized.
        //
        // Plan:
        //: 1 Create multiple objects passing them different arguments for
        //:   'stream' and the formatting options.  Invoke a manipulator method
        //:   on each object and confirm that the output is as expected.
        //
        // Testing:
        //   baljsn::Formatter(bsl::ostream& stream, style, indent, spl);
        //   ~baljsn::Formatter();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING CREATORS" << endl
                          << "================" << endl;
#define NL "\n"

        // Use a value of -1 to signify that that specific option should not be
        // set and that its default value should be used.

        const struct Data {
            int         d_line;                // source line number
            int         d_encodingStyle;
            int         d_initialIndentLevel;
            int         d_spacesPerLevel;
            bsl::string d_expected;
        } DATA[] = {

        // LINE  STYLE  INDENT   SPL   EXPECTED
        // ----  -----  ------   ---   --------

        // No options specified
        {   L_,    -1,     -1,   -1,   "{\"A\":1,\"B\":[2]}"                 },

        // Specify only encoding style
        {   L_,     0,     -1,   -1,   "{\"A\":1,\"B\":[2]}"                 },
        {   L_,     1,     -1,   -1,   "{"                               NL
                                       "\"A\" : 1,"                      NL
                                       "\"B\" : ["                       NL
                                       "2"                               NL
                                       "]"                               NL
                                       "}"                                   },

        // Specify encoding style and initialIndentLevel
        {   L_,     0,      1,   -1,   "{\"A\":1,\"B\":[2]}"                 },
        {   L_,     0,      5,   -1,   "{\"A\":1,\"B\":[2]}"                 },
        {   L_,     1,      1,   -1,   "{"                               NL
                                       "\"A\" : 1,"                      NL
                                       "\"B\" : ["                       NL
                                       "2"                               NL
                                       "]"                               NL
                                       "}"                                   },

        // Specify all options
        {   L_,     0,      1,    2,   "{\"A\":1,\"B\":[2]}"                 },
        {   L_,     0,      5,   10,   "{\"A\":1,\"B\":[2]}"                 },
        {   L_,     1,      1,    2,   "{"                               NL
                                       "    \"A\" : 1,"                  NL
                                       "    \"B\" : ["                   NL
                                       "2"                               NL
                                       "    ]"                           NL
                                       "  }"                                 },
        {   L_,     1,      3,    5,   "{"                               NL
                                       "                    \"A\" : 1,"  NL
                                       "                    \"B\" : ["   NL
                                       "2"                               NL
                                       "                    ]"           NL
                                       "               }"                    },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int         LINE   = DATA[i].d_line;
            const int         STYLE  = DATA[i].d_encodingStyle;
            const int         INDENT = DATA[i].d_initialIndentLevel;
            const int         SPL    = DATA[i].d_spacesPerLevel;
            const bsl::string EXP    = DATA[i].d_expected;

            bsl::ostringstream     os1;

            bdlsb::MemOutStreamBuf mosb;
            bsl::ostream           os2(&mosb);

            Obj mX = g(os1, STYLE, INDENT, SPL);  const Obj& X = mX;
            Obj mY = g(os2, STYLE, INDENT, SPL);  const Obj& Y = mY;

            mX.openObject();
            mX.openElement("A"); mX.putValue(1); mX.closeElement();
            mX.openElement("B");
            mX.openArray(); mX.putValue(2); mX.closeArray();
            mX.closeObject();

            mY.openObject();
            mY.openElement("A"); mY.putValue(1); mY.closeElement();
            mY.openElement("B");
            mY.openArray(); mY.putValue(2); mY.closeArray();
            mY.closeObject();

            os1 << bsl::flush;
            os2 << bsl::flush;

            bsl::string actual(mosb.data(), mosb.length());

            ASSERTV(LINE, EXP, os1.str(), EXP == os1.str());
            ASSERTV(LINE, EXP, actual,    EXP == actual);
        }
#undef NL

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        bsl::ostringstream os;

        Obj mX(os);  const Obj& X = mX;

        bsl::string exp;

        mX.openObject();

        exp += '{';
        ASSERTV(exp, os.str(), exp == os.str());

        mX.closeObject();

        exp += '}';
        ASSERTV(exp, os.str(), exp == os.str());

        mX.openArray();

        exp += '[';
        ASSERTV(exp, os.str(), exp == os.str());

        mX.closeArray();

        exp += ']';
        ASSERTV(exp, os.str(), exp == os.str());

        bsl::string name = "name";

        const int rc = mX.openElement(name);
        ASSERTV(rc, 0 == rc);

        exp += '"' + name + '"' + ':';
        ASSERTV(exp, os.str(), exp == os.str());

        mX.closeElement();

        exp += ',';
        ASSERTV(exp, os.str(), exp == os.str());

        mX.setIsArrayElement(true);
        ASSERTV(true == X.isArrayElement());
        ASSERTV(exp, os.str(), exp == os.str());

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
// Copyright 2017 Bloomberg Finance L.P.
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
