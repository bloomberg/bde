// balber_berdecoder.t.cpp                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <balber_berdecoder.h>

#include <balber_berencoder.h>        // for testing only

#include <bdlat_attributeinfo.h>
#include <bdlat_selectioninfo.h>
#include <bdlat_valuetypefunctions.h>

#include <bslim_testutil.h>

#include <bdlsb_memoutstreambuf.h>      // for testing only
#include <bdlsb_fixedmeminstreambuf.h>  // for testing only

#include <bslma_allocator.h>

#include <bsls_objectbuffer.h>
#include <bsls_stopwatch.h>
#include <bsls_types.h>

#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_serialdateimputil.h>
#include <bdlt_time.h>

#include <bdlb_chartype.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>

#include <bslim_testutil.h>

#include <bsl_iostream.h>
#include <bsl_iomanip.h>

#include <bsl_cstdlib.h>

#include <bsl_fstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//
//

// ----------------------------------------------------------------------------

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

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

static bool         verbose = false;
static bool     veryVerbose = false;
static bool veryVeryVerbose = false;

// ============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

bsl::vector<char> loadFromHex(const char *hexData)
{
    char firstDigit = 0;

    bsl::vector<char> result;

    char hexValueTable[128] =
    {
        0 , //   0   0
        0 , //   1   1
        0 , //   2   2
        0 , //   3   3
        0 , //   4   4
        0 , //   5   5
        0 , //   6   6
        0 , //   7   7
        0 , //   8   8 - BACKSPACE
        0 , //   9   9 - TAB
        0 , //  10   a - LF
        0 , //  11   b
        0 , //  12   c
        0 , //  13   d - CR
        0 , //  14   e
        0 , //  15   f
        0 , //  16  10
        0 , //  17  11
        0 , //  18  12
        0 , //  19  13
        0 , //  20  14
        0 , //  21  15
        0 , //  22  16
        0 , //  23  17
        0 , //  24  18
        0 , //  25  19
        0 , //  26  1a
        0 , //  27  1b
        0 , //  28  1c
        0 , //  29  1d
        0 , //  30  1e
        0 , //  31  1f
        0 , //  32  20 - SPACE
        0 , //  33  21 - !
        0 , //  34  22 - "
        0 , //  35  23 - #
        0 , //  36  24 - $
        0 , //  37  25 - %
        0 , //  38  26 - &
        0 , //  39  27 - '
        0 , //  40  28 - (
        0 , //  41  29 - )
        0 , //  42  2a - *
        0 , //  43  2b - +
        0 , //  44  2c - ,
        0 , //  45  2d - -
        0 , //  46  2e - .
        0 , //  47  2f - /
        0 , //  48  30 - 0
        1 , //  49  31 - 1
        2 , //  50  32 - 2
        3 , //  51  33 - 3
        4 , //  52  34 - 4
        5 , //  53  35 - 5
        6 , //  54  36 - 6
        7 , //  55  37 - 7
        8 , //  56  38 - 8
        9 , //  57  39 - 9
        0 , //  58  3a - :
        0 , //  59  3b - ;
        0 , //  60  3c - <
        0 , //  61  3d - =
        0 , //  62  3e - >
        0 , //  63  3f - ?
        0 , //  64  40 - @
        10 , //  65  41 - A
        11 , //  66  42 - B
        12 , //  67  43 - C
        13 , //  68  44 - D
        14 , //  69  45 - E
        15 , //  70  46 - F
        0 , //  71  47 - G
        0 , //  72  48 - H
        0 , //  73  49 - I
        0 , //  74  4a - J
        0 , //  75  4b - K
        0 , //  76  4c - L
        0 , //  77  4d - M
        0 , //  78  4e - N
        0 , //  79  4f - O
        0 , //  80  50 - P
        0 , //  81  51 - Q
        0 , //  82  52 - R
        0 , //  83  53 - S
        0 , //  84  54 - T
        0 , //  85  55 - U
        0 , //  86  56 - V
        0 , //  87  57 - W
        0 , //  88  58 - X
        0 , //  89  59 - Y
        0 , //  90  5a - Z
        0 , //  91  5b - [
        0 , //  92  5c - '\'
        0 , //  93  5d - ]
        0 , //  94  5e - ^
        0 , //  95  5f - _
        0 , //  96  60 - `
        10 , //  97  61 - a
        11 , //  98  62 - b
        12 , //  99  63 - c
        13 , // 100  64 - d
        14 , // 101  65 - e
        15 , // 102  66 - f
        0 , // 103  67 - g
        0 , // 104  68 - h
        0 , // 105  69 - i
        0 , // 106  6a - j
        0 , // 107  6b - k
        0 , // 108  6c - l
        0 , // 109  6d - m
        0 , // 110  6e - n
        0 , // 111  6f - o
        0 , // 112  70 - p
        0 , // 113  71 - q
        0 , // 114  72 - r
        0 , // 115  73 - s
        0 , // 116  74 - t
        0 , // 117  75 - u
        0 , // 118  76 - v
        0 , // 119  77 - w
        0 , // 120  78 - x
        0 , // 121  79 - y
        0 , // 122  7a - z
        0 , // 123  7b - {
        0 , // 124  7c - |
        0 , // 125  7d - }
        0 , // 126  7e - ~
        0   // 127  7f - DEL
    };

    while (*hexData) {
        if (' ' == *hexData) {
            ++hexData;
            continue;
        }

        if (0 == firstDigit) {
            firstDigit = *hexData;
        }
        else {
            char value = (hexValueTable[firstDigit] << 4)
                       | (hexValueTable[*hexData]);

            result.push_back(value);
            firstDigit = 0;
        }

        ++hexData;
    }

    ASSERT(0 == firstDigit);

    return result;
}

void printBuffer(const char *buffer, int length)
    // Print the specified 'buffer' of the specified 'length' in hex form
{
    cout << hex;
    int numOutput = 0;
    for (int i = 0; i < length; ++i) {
        if ((unsigned char) buffer[i] < 16) {
            cout << '0';
        }
        cout << (int) (unsigned char) buffer[i];
        numOutput += 2;
        if (0 == numOutput % 8) {
            cout << " ";
        }
    }
    cout << dec << endl;
}

void printDiagnostic(balber::BerDecoder & decoder)
{
    if (veryVerbose) {
        bsl::cout << decoder.loggedMessages();
    }
}

// ============================================================================
//                     GLOBAL HELPER CLASSES FOR TESTING
// ----------------------------------------------------------------------------

// The code below was generated using the following command:
//..
// bas_codegen.pl --m msg --noTestDrivers --noTimestamps -p test test_codec.xsd
//..
// The contents of test_codec.xsd follows:
//..
//  <?xml version="1.0" encoding="UTF-8"?>
//  <xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema"
//             xmlns:bdem="http://bloomberg.com/schemas/bdem"
//             xmlns="http://bloomberg.com/schemas/test_codec"
//             targetNamespace="http://bloomberg.com/schemas/test_codec"
//             elementFormDefault="qualified">
//      <!-- "$Id: $  $CSID:  $  $SCMId:  $" -->
//
//      <xs:annotation>
//          <xs:documentation>
//              This component provides classes for testing codecs.
//          </xs:documentation>
//      </xs:annotation>
//
//      <xs:simpleType name="CustomizedString">
//          <xs:restriction base="xs:string">
//              <xs:maxLength value="25"/>
//          </xs:restriction>
//      </xs:simpleType>
//
//      <xs:complexType name="MyChoice">
//          <xs:choice>
//              <xs:element name="selection1" type="xs:int"/>
//              <xs:element name="selection2" type="xs:string"/>
//          </xs:choice>
//      </xs:complexType>
//
//      <xs:simpleType name="MyEnumeration">
//          <xs:restriction base="xs:string">
//              <xs:enumeration value="VALUE1"/>
//              <xs:enumeration value="VALUE2"/>
//          </xs:restriction>
//      </xs:simpleType>
//
//      <xs:complexType name="MySequence">
//          <xs:sequence>
//              <xs:element name="attribute1" type="xs:int"/>
//              <xs:element name="attribute2" type="xs:string"/>
//          </xs:sequence>
//      </xs:complexType>
//
//      <xs:complexType name="MySequenceWithArray">
//          <xs:sequence>
//              <xs:element name="attribute1" type="xs:int"/>
//              <xs:element name="attribute2" type="xs:string"
//                          minOccurs="0" maxOccurs="unbounded"/>
//          </xs:sequence>
//      </xs:complexType>
//
//      <xs:complexType name="MySequenceWithNullable">
//          <xs:sequence>
//              <xs:element name="attribute1" type="xs:int"/>
//              <xs:element name="attribute2" type="xs:string"
//                          minOccurs="0" maxOccurs="1"/>
//          </xs:sequence>
//      </xs:complexType>
//
//      <xs:complexType name="MySequenceWithAnonymousChoice">
//          <xs:sequence>
//              <xs:element name="attribute1" type="xs:int"/>
//              <xs:choice>
//                  <xs:element name="myChoice1" type="xs:int"/>
//                  <xs:element name="myChoice2" type="xs:string"/>
//              </xs:choice>
//              <xs:element name="attribute2" type="xs:string"/>
//          </xs:sequence>
//      </xs:complexType>
//
//      <xs:complexType name="MySequenceWithNillable">
//          <xs:sequence>
//              <xs:element name="attribute1" type="xs:int"/>
//              <xs:element name="myNillable" type="xs:string"
//                          nillable="true"/>
//              <xs:element name="attribute2" type="xs:string"/>
//          </xs:sequence>
//      </xs:complexType>
//
//      <xs:complexType name="Address">
//          <xs:sequence>
//              <xs:element name="street" type="xs:string"/>
//              <xs:element name="city"   type="xs:string"/>
//              <xs:element name="state"  type="xs:string"/>
//          </xs:sequence>
//      </xs:complexType>
//
//      <xs:complexType name="Employee">
//          <xs:sequence>
//              <xs:element name="name"        type="xs:string"/>
//              <xs:element name="homeAddress" type="Address"/>
//              <xs:element name="age"         type="xs:int"/>
//          </xs:sequence>
//      </xs:complexType>
//
//      <xs:complexType name="sqrt">
//          <xs:annotation>
//              <xs:documentation>
//                  Simulate a square root request/response
//              </xs:documentation>
//          </xs:annotation>
//          <xs:sequence>
//              <xs:element name="value" type="xs:double"/>
//          </xs:sequence>
//      </xs:complexType>
//
//      <xs:complexType name="basicRecord">
//          <xs:annotation>
//              <xs:documentation>
//                  A representative small record type
//              </xs:documentation>
//          </xs:annotation>
//          <xs:sequence>
//              <xs:element name="i1" type="xs:int"/>
//              <xs:element name="i2" type="xs:int"/>
//              <xs:element name="dt" type="xs:dateTime"/>
//              <xs:element name="s"  type="xs:string"/>
//          </xs:sequence>
//      </xs:complexType>
//
//      <xs:complexType name="bigRecord">
//          <xs:annotation>
//              <xs:documentation>
//                  A bigger record containing an array of smaller records
//              </xs:documentation>
//          </xs:annotation>
//          <xs:sequence>
//              <xs:element name="name" type="xs:string" minOccur="0"/>
//              <xs:element name="array" type="basicRecord"
//                          minOccurs="0" maxOccurs="unbounded"/>
//          </xs:sequence>
//      </xs:complexType>
//
//      <xs:complexType name="timingRequest">
//          <xs:annotation>
//              <xs:documentation>
//                  A choice record representative of a typical request object.
//              </xs:documentation>
//          </xs:annotation>
//          <xs:choice>
//              <xs:element name="sqrt" type="sqrt"/>
//              <xs:element name="basic" type="basicRecord"/>
//              <xs:element name="big" type="bigRecord"/>
//          </xs:choice>
//      </xs:complexType>
//
//      <xs:element name="timingRequest" type="timingRequest"/>
//
//  </xs:schema>

// ************************ START OF GENERATED CODE **************************
// NOTE: Please update schema above and regenerate to make changes.

// test_messages.h   -*-C++-*-
#ifndef INCLUDED_TEST_MESSAGES
#define INCLUDED_TEST_MESSAGES

//@PURPOSE: TODO: Provide purpose
//
//@CLASSES:
// test::MyChoice: TODO: Provide purpose
// test::MySequenceWithNullable: TODO: Provide purpose
// test::Address: TODO: Provide purpose
// test::MySequence: TODO: Provide purpose
// test::MySequenceWithNillable: TODO: Provide purpose
// test::MyEnumeration: TODO: Provide purpose
// test::Sqrt: TODO: Provide purpose
// test::CustomizedString: TODO: Provide purpose
// test::BasicRecord: TODO: Provide purpose
// test::MySequenceWithArray: TODO: Provide purpose
// test::MySequenceWithAnonymousChoiceChoice: TODO: Provide purpose
// test::Employee: TODO: Provide purpose
// test::BigRecord: TODO: Provide purpose
// test::MySequenceWithAnonymousChoice: TODO: Provide purpose
// test::TimingRequest: TODO: Provide purpose
//
//@DESCRIPTION:
// This component provides classes for testing codecs.

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDLAT_ATTRIBUTEINFO
#include <bdlat_attributeinfo.h>
#endif

#ifndef INCLUDED_BDLAT_ENUMERATORINFO
#include <bdlat_enumeratorinfo.h>
#endif

#ifndef INCLUDED_BDLAT_TYPETRAITS
#include <bdlat_typetraits.h>
#endif

#ifndef INCLUDED_BDLAT_VALUETYPEFUNCTIONS
#include <bdlat_valuetypefunctions.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BDLB_PRINTMETHODS
#include <bdlb_printmethods.h>
#endif

#ifndef INCLUDED_BDLT_DATETIMETZ
#include <bdlt_datetimetz.h>
#endif

#ifndef INCLUDED_BDLB_NULLABLEVALUE
#include <bdlb_nullablevalue.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

namespace BloombergLP {

namespace test { class MyChoice; }
namespace test { class MySequenceWithNullable; }
namespace test { class Address; }
namespace test { class MySequence; }
namespace test { class MySequenceWithNillable; }
namespace test { class Sqrt; }
namespace test { class CustomizedString; }
namespace test { class BasicRecord; }
namespace test { class BasicRecordWithVariant; }
namespace test { class MySequenceWithArray; }
namespace test { class MySequenceWithAnonymousChoiceChoice; }
namespace test { class Employee; }
namespace test { class BigRecord; }
namespace test { class MySequenceWithAnonymousChoice; }
namespace test { class TimingRequest; }
namespace test {

                               // ==============
                               // class MyChoice
                               // ==============

class MyChoice {
    // TODO: Provide annotation

  private:
    union {
        bsls::ObjectBuffer< int >         d_selection1;
        bsls::ObjectBuffer< bsl::string > d_selection2;
    };

    int               d_selectionId;
    bslma::Allocator *d_allocator_p;

  public:
    // TYPES
    enum {
        NUM_SELECTIONS = 2 // the number of selections in this class
    };

    enum {
        SELECTION_INDEX_SELECTION1 = 0,
            // index for "Selection1" selection
        SELECTION_INDEX_SELECTION2 = 1
            // index for "Selection2" selection
    };

    enum {
        SELECTION_ID_UNDEFINED = -1,

        SELECTION_ID_SELECTION1 = 0,
            // id for "Selection1" selection
        SELECTION_ID_SELECTION2 = 1
            // id for "Selection2" selection
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "MyChoice")

    static const bdlat_SelectionInfo SELECTION_INFO_ARRAY[];
        // selection information for each selection

  public:
    // CLASS METHODS
    static const bdlat_SelectionInfo *lookupSelectionInfo(int id);
        // Return selection information for the selection indicated by the
        // specified 'id' if the selection exists, and 0 otherwise.

    static const bdlat_SelectionInfo *lookupSelectionInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return selection information for the selection indicated by the
        // specified 'name' of the specified 'nameLength' if the selection
        // exists, and 0 otherwise.

    // CREATORS
    explicit MyChoice(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'MyChoice' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    MyChoice(const MyChoice& original,
            bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'MyChoice' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~MyChoice();
        // Destroy this object.

    // MANIPULATORS
    MyChoice& operator=(const MyChoice& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    void reset();
        // Reset this object to the default value (i.e., its value upon default
        // construction).

    int makeSelection(int selectionId);
        // Set the value of this object to be the default for the selection
        // indicated by the specified 'selectionId'.  Return 0 on success, and
        // non-zero value otherwise (i.e., the selection is not found).

    int makeSelection(const char *name, int nameLength);
        // Set the value of this object to be the default for the selection
        // indicated by the specified 'name' of the specified 'nameLength'.
        // Return 0 on success, and non-zero value otherwise (i.e., the
        // selection is not found).

    int& makeSelection1();
    int& makeSelection1(int value);
        // Set the value of this object to be a "Selection1" value.  Optionally
        // specify the 'value' of the "Selection1".  If 'value' is not
        // specified, the default "Selection1" value is used.

    bsl::string& makeSelection2();
    bsl::string& makeSelection2(const bsl::string& value);
        // Set the value of this object to be a "Selection2" value.  Optionally
        // specify the 'value' of the "Selection2".  If 'value' is not
        // specified, the default "Selection2" value is used.

    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.

    int& selection1();
        // Return a reference to the modifiable "Selection1" selection of this
        // object if "Selection1" is the current selection.  The behavior is
        // undefined unless "Selection1" is the selection of this object.

    bsl::string& selection2();
        // Return a reference to the modifiable "Selection2" selection of this
        // object if "Selection2" is the current selection.  The behavior is
        // undefined unless "Selection2" is the selection of this object.

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

    int selectionId() const;
        // Return the id of the current selection if the selection is defined,
        // and -1 otherwise.

    template<class ACCESSOR>
    int accessSelection(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' on the non-modifiable selection,
        // supplying 'accessor' with the corresponding selection information
        // structure.  Return the value returned from the invocation of
        // 'accessor' if this object has a defined selection, and -1 otherwise.

    const int& selection1() const;
        // Return a reference to the non-modifiable "Selection1" selection of
        // this object if "Selection1" is the current selection.  The behavior
        // is undefined unless "Selection1" is the selection of this object.

    const bsl::string& selection2() const;
        // Return a reference to the non-modifiable "Selection2" selection of
        // this object if "Selection2" is the current selection.  The behavior
        // is undefined unless "Selection2" is the selection of this object.

    bool isSelection1Value() const;
        // Return 'true' if the value of this object is a "Selection1" value,
        // and return 'false' otherwise.

    bool isSelection2Value() const;
        // Return 'true' if the value of this object is a "Selection2" value,
        // and return 'false' otherwise.

    bool isUndefinedValue() const;
        // Return 'true' if the value of this object is undefined, and 'false'
        // otherwise.
};

// FREE OPERATORS
inline
bool operator==(const MyChoice& lhs, const MyChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'MyChoice' objects have the same
    // value if either the selections in both objects have the same ids and
    // the same values, or both selections are undefined.

inline
bool operator!=(const MyChoice& lhs, const MyChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const MyChoice& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace test

// TRAITS

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_TRAITS(test::MyChoice)

namespace test {

                        // ============================
                        // class MySequenceWithNullable
                        // ============================

class MySequenceWithNullable {
    // TODO: Provide annotation

  private:
    int                               d_attribute1;
    bdlb::NullableValue<bsl::string>  d_attribute2;

  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 2 // the number of attributes in this class
    };

    enum {
        ATTRIBUTE_INDEX_ATTRIBUTE1 = 0,
            // index for "Attribute1" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE2 = 1
            // index for "Attribute2" attribute
    };

    enum {
        ATTRIBUTE_ID_ATTRIBUTE1 = 0,
            // id for "Attribute1" attribute
        ATTRIBUTE_ID_ATTRIBUTE2 = 1
            // id for "Attribute2" attribute
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "MySequenceWithNullable")

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
    explicit MySequenceWithNullable(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'MySequenceWithNullable' having the default
        // value.  Use the optionally specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    MySequenceWithNullable(const MySequenceWithNullable& original,
                           bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'MySequenceWithNullable' having the value
        // of the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~MySequenceWithNullable();
        // Destroy this object.

    // MANIPULATORS
    MySequenceWithNullable& operator=(const MySequenceWithNullable& rhs);
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
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'name' of the
        // specified 'nameLength', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return the value
        // returned from the invocation of 'manipulator' if 'name' identifies
        // an attribute of this class, and -1 otherwise.

    int& attribute1();
        // Return a reference to the modifiable "Attribute1" attribute of this
        // object.

    bdlb::NullableValue<bsl::string>& attribute2();
        // Return a reference to the modifiable "Attribute2" attribute of this
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

    const int& attribute1() const;
        // Return a reference to the non-modifiable "Attribute1" attribute of
        // this object.

    const bdlb::NullableValue<bsl::string>& attribute2() const;
        // Return a reference to the non-modifiable "Attribute2" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const MySequenceWithNullable& lhs,
                const MySequenceWithNullable& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const MySequenceWithNullable& lhs,
                const MySequenceWithNullable& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream&
operator<<(bsl::ostream& stream, const MySequenceWithNullable& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace test

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::MySequenceWithNullable)

namespace test {

                               // =============
                               // class Address
                               // =============

class Address {
    // TODO: Provide annotation

  private:
    bsl::string  d_street;
    bsl::string  d_city;
    bsl::string  d_state;

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
        // Create an object of type 'Address' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Address(const Address& original,
            bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Address' having the value of the specified
        // 'original' object.  Use the optionally specified 'basicAllocator' to
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
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'name' of the
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
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace test

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::Address)

namespace test {

                              // ================
                              // class MySequence
                              // ================

class MySequence {
    // TODO: Provide annotation

  private:
    int          d_attribute1;
    bsl::string  d_attribute2;

  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 2 // the number of attributes in this class
    };

    enum {
        ATTRIBUTE_INDEX_ATTRIBUTE1 = 0,
            // index for "Attribute1" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE2 = 1
            // index for "Attribute2" attribute
    };

    enum {
        ATTRIBUTE_ID_ATTRIBUTE1 = 0,
            // id for "Attribute1" attribute
        ATTRIBUTE_ID_ATTRIBUTE2 = 1
            // id for "Attribute2" attribute
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "MySequence")

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
    explicit MySequence(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'MySequence' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    MySequence(const MySequence& original,
               bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'MySequence' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~MySequence();
        // Destroy this object.

    // MANIPULATORS
    MySequence& operator=(const MySequence& rhs);
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
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'name' of the
        // specified 'nameLength', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return the value
        // returned from the invocation of 'manipulator' if 'name' identifies
        // an attribute of this class, and -1 otherwise.

    int& attribute1();
        // Return a reference to the modifiable "Attribute1" attribute of this
        // object.

    bsl::string& attribute2();
        // Return a reference to the modifiable "Attribute2" attribute of this
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

    const int& attribute1() const;
        // Return a reference to the non-modifiable "Attribute1" attribute of
        // this object.

    const bsl::string& attribute2() const;
        // Return a reference to the non-modifiable "Attribute2" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const MySequence& lhs, const MySequence& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const MySequence& lhs, const MySequence& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const MySequence& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace test

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::MySequence)

namespace test {

                        // ============================
                        // class MySequenceWithNillable
                        // ============================

class MySequenceWithNillable {
    // TODO: Provide annotation

  private:
    int                               d_attribute1;
    bdlb::NullableValue<bsl::string>  d_myNillable;
    bsl::string                       d_attribute2;

  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 3 // the number of attributes in this class
    };

    enum {
        ATTRIBUTE_INDEX_ATTRIBUTE1 = 0,
            // index for "Attribute1" attribute
        ATTRIBUTE_INDEX_MY_NILLABLE = 1,
            // index for "MyNillable" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE2 = 2
            // index for "Attribute2" attribute
    };

    enum {
        ATTRIBUTE_ID_ATTRIBUTE1 = 0,
            // id for "Attribute1" attribute
        ATTRIBUTE_ID_MY_NILLABLE = 1,
            // id for "MyNillable" attribute
        ATTRIBUTE_ID_ATTRIBUTE2 = 2
            // id for "Attribute2" attribute
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "MySequenceWithNillable")

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
    explicit MySequenceWithNillable(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'MySequenceWithNillable' having the default
        // value.  Use the optionally specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    MySequenceWithNillable(const MySequenceWithNillable& original,
                           bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'MySequenceWithNillable' having the value
        // of the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~MySequenceWithNillable();
        // Destroy this object.

    // MANIPULATORS
    MySequenceWithNillable& operator=(const MySequenceWithNillable& rhs);
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
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'name' of the
        // specified 'nameLength', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return the value
        // returned from the invocation of 'manipulator' if 'name' identifies
        // an attribute of this class, and -1 otherwise.

    int& attribute1();
        // Return a reference to the modifiable "Attribute1" attribute of this
        // object.

    bdlb::NullableValue<bsl::string>& myNillable();
        // Return a reference to the modifiable "MyNillable" attribute of this
        // object.

    bsl::string& attribute2();
        // Return a reference to the modifiable "Attribute2" attribute of this
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

    const int& attribute1() const;
        // Return a reference to the non-modifiable "Attribute1" attribute of
        // this object.

    const bdlb::NullableValue<bsl::string>& myNillable() const;
        // Return a reference to the non-modifiable "MyNillable" attribute of
        // this object.

    const bsl::string& attribute2() const;
        // Return a reference to the non-modifiable "Attribute2" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const MySequenceWithNillable& lhs,
                const MySequenceWithNillable& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const MySequenceWithNillable& lhs,
                const MySequenceWithNillable& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream&
operator<<(bsl::ostream& stream, const MySequenceWithNillable& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace test

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::MySequenceWithNillable)

namespace test {

                            // ===================
                            // class MyEnumeration
                            // ===================

struct MyEnumeration {
    // TODO: Provide annotation

  public:
    // TYPES
    enum Value {
        VALUE1 = 0,
        VALUE2 = 1
    };

    enum {
        NUM_ENUMERATORS = 2
            // the number of enumerators in the 'Value' enumeration
    };

    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "MyEnumeration")

    static const bdlat_EnumeratorInfo ENUMERATOR_INFO_ARRAY[];
        // enumerator information for each enumerator

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
        // Write to the specified 'stream' the string representation of
        // the specified enumeration 'value'.  Return a reference to
        // the modifiable 'stream'.
};

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream& stream, MyEnumeration::Value rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace test

// TRAITS

BDLAT_DECL_ENUMERATION_TRAITS(test::MyEnumeration)

namespace test {

                                 // ==========
                                 // class Sqrt
                                 // ==========

class Sqrt {
    // Simulate a square root request/response

  private:
    double  d_value;

  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 1 // the number of attributes in this class
    };

    enum {
        ATTRIBUTE_INDEX_VALUE = 0
            // index for "Value" attribute
    };

    enum {
        ATTRIBUTE_ID_VALUE = 0
            // id for "Value" attribute
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "Sqrt")

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
    Sqrt();
        // Create an object of type 'Sqrt' having the default value.

    Sqrt(const Sqrt& original);
        // Create an object of type 'Sqrt' having the value of the specified
        // 'original' object.

    ~Sqrt();
        // Destroy this object.

    // MANIPULATORS
    Sqrt& operator=(const Sqrt& rhs);
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
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'name' of the
        // specified 'nameLength', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return the value
        // returned from the invocation of 'manipulator' if 'name' identifies
        // an attribute of this class, and -1 otherwise.

    double& value();
        // Return a reference to the modifiable "Value" attribute of this
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

    const double& value() const;
        // Return a reference to the non-modifiable "Value" attribute of this
        // object.
};

// FREE OPERATORS
inline
bool operator==(const Sqrt& lhs, const Sqrt& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const Sqrt& lhs, const Sqrt& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const Sqrt& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace test

// TRAITS

BDLAT_DECL_SEQUENCE_TRAITS(test::Sqrt)

namespace test {

                           // ======================
                           // class CustomizedString
                           // ======================

class CustomizedString {
    // TODO: Provide annotation

  private:
    // PRIVATE DATA MEMBERS
    bsl::string d_value;  // stored value

    // FRIENDS
    friend bool operator==(const CustomizedString& lhs,
                           const CustomizedString& rhs);
    friend bool operator!=(const CustomizedString& lhs,
                           const CustomizedString& rhs);

    // PRIVATE CLASS METHODS
    static int checkRestrictions(const bsl::string& value);
        // Check if the specified 'value' satisfies the restrictions of this
        // class (i.e., "CustomizedString").  Return 0 if successful (i.e., the
        // restrictions are satisfied) and non-zero otherwise.

  public:
    // TYPES
    typedef bsl::string BaseType;

    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "CustomizedString")

    // CREATORS
    explicit CustomizedString(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'CustomizedString' having the default
        // value.  Use the optionally specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    CustomizedString(const CustomizedString& original,
                    bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'CustomizedString' having the value
        // of the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    explicit CustomizedString(const bsl::string& value,
                             bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'CustomizedString' having the specified
        // 'value'.  Use the optionally specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    ~CustomizedString();
        // Destroy this object.

    // MANIPULATORS
    CustomizedString& operator=(const CustomizedString& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    int fromString(const bsl::string& value);
        // Convert from the specified 'value' to this type.  Return 0 if
        // successful and non-zero otherwise.

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

    const bsl::string& toString() const;
        // Convert this value to 'bsl::string'.
};

// FREE OPERATORS
inline
bool operator==(const CustomizedString& lhs, const CustomizedString& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const CustomizedString& lhs, const CustomizedString& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const CustomizedString& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace test

// TRAITS

BDLAT_DECL_CUSTOMIZEDTYPE_WITH_ALLOCATOR_TRAITS(test::CustomizedString)

namespace test {

                             // =================
                             // class BasicRecord
                             // =================

class BasicRecord {
    // A representative small record type

  private:
    int              d_i1;
    int              d_i2;
    bdlt::DatetimeTz  d_dt;
    bsl::string      d_s;

  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 4 // the number of attributes in this class
    };

    enum {
        ATTRIBUTE_INDEX_I1 = 0,
            // index for "I1" attribute
        ATTRIBUTE_INDEX_I2 = 1,
            // index for "I2" attribute
        ATTRIBUTE_INDEX_DT = 2,
            // index for "Dt" attribute
        ATTRIBUTE_INDEX_S = 3
            // index for "S" attribute
    };

    enum {
        ATTRIBUTE_ID_I1 = 0,
            // id for "I1" attribute
        ATTRIBUTE_ID_I2 = 1,
            // id for "I2" attribute
        ATTRIBUTE_ID_DT = 2,
            // id for "Dt" attribute
        ATTRIBUTE_ID_S = 3
            // id for "S" attribute
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "BasicRecord")

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
    explicit BasicRecord(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BasicRecord' having the default value.
        // Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    BasicRecord(const BasicRecord& original,
                bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BasicRecord' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~BasicRecord();
        // Destroy this object.

    // MANIPULATORS
    BasicRecord& operator=(const BasicRecord& rhs);
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
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'name' of the
        // specified 'nameLength', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return the value
        // returned from the invocation of 'manipulator' if 'name' identifies
        // an attribute of this class, and -1 otherwise.

    int& i1();
        // Return a reference to the modifiable "I1" attribute of this object.

    int& i2();
        // Return a reference to the modifiable "I2" attribute of this object.

    bdlt::DatetimeTz& dt();
        // Return a reference to the modifiable "Dt" attribute of this object.

    bsl::string& s();
        // Return a reference to the modifiable "S" attribute of this object.

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

    const int& i1() const;
        // Return a reference to the non-modifiable "I1" attribute of this
        // object.

    const int& i2() const;
        // Return a reference to the non-modifiable "I2" attribute of this
        // object.

    const bdlt::DatetimeTz& dt() const;
        // Return a reference to the non-modifiable "Dt" attribute of this
        // object.

    const bsl::string& s() const;
        // Return a reference to the non-modifiable "S" attribute of this
        // object.
};

// FREE OPERATORS
inline
bool operator==(const BasicRecord& lhs, const BasicRecord& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const BasicRecord& lhs, const BasicRecord& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const BasicRecord& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace test

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::BasicRecord)

namespace test {

                        // ============================
                        // class BasicRecordWithVariant
                        // ============================

class BasicRecordWithVariant {
    // A representative small record type

  private:
    int                                            d_i1;
    int                                            d_i2;
    bdlb::Variant<bdlt::Datetime, bdlt::DatetimeTz>  d_dt;
    bsl::string                                    d_s;

  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 4 // the number of attributes in this class
    };

    enum {
        ATTRIBUTE_INDEX_I1 = 0,
            // index for "I1" attribute
        ATTRIBUTE_INDEX_I2 = 1,
            // index for "I2" attribute
        ATTRIBUTE_INDEX_DT = 2,
            // index for "Dt" attribute
        ATTRIBUTE_INDEX_S = 3
            // index for "S" attribute
    };

    enum {
        ATTRIBUTE_ID_I1 = 0,
            // id for "I1" attribute
        ATTRIBUTE_ID_I2 = 1,
            // id for "I2" attribute
        ATTRIBUTE_ID_DT = 2,
            // id for "Dt" attribute
        ATTRIBUTE_ID_S = 3
            // id for "S" attribute
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "BasicRecord")

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
    explicit BasicRecordWithVariant(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BasicRecord' having the default value.
        // Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    BasicRecordWithVariant(const BasicRecordWithVariant& original,
                bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BasicRecord' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~BasicRecordWithVariant();
        // Destroy this object.

    // MANIPULATORS
    BasicRecordWithVariant& operator=(const BasicRecordWithVariant& rhs);
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
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'name' of the
        // specified 'nameLength', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return the value
        // returned from the invocation of 'manipulator' if 'name' identifies
        // an attribute of this class, and -1 otherwise.

    int& i1();
        // Return a reference to the modifiable "I1" attribute of this object.

    int& i2();
        // Return a reference to the modifiable "I2" attribute of this object.

    bdlb::Variant<bdlt::Datetime, bdlt::DatetimeTz>& dt();
        // Return a reference to the modifiable "Dt" attribute of this object.

    bsl::string& s();
        // Return a reference to the modifiable "S" attribute of this object.

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

    const int& i1() const;
        // Return a reference to the non-modifiable "I1" attribute of this
        // object.

    const int& i2() const;
        // Return a reference to the non-modifiable "I2" attribute of this
        // object.

    const bdlb::Variant<bdlt::Datetime, bdlt::DatetimeTz>& dt() const;
        // Return a reference to the non-modifiable "Dt" attribute of this
        // object.

    const bsl::string& s() const;
        // Return a reference to the non-modifiable "S" attribute of this
        // object.
};

// FREE OPERATORS
inline
bool operator==(const BasicRecordWithVariant& lhs,
                const BasicRecordWithVariant& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const BasicRecordWithVariant& lhs,
                const BasicRecordWithVariant& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream,
                         const BasicRecordWithVariant& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace test

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::BasicRecordWithVariant)

namespace test {

                         // =========================
                         // class MySequenceWithArray
                         // =========================

class MySequenceWithArray {
    // TODO: Provide annotation

  private:
    int                       d_attribute1;
    bsl::vector<bsl::string>  d_attribute2;

  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 2 // the number of attributes in this class
    };

    enum {
        ATTRIBUTE_INDEX_ATTRIBUTE1 = 0,
            // index for "Attribute1" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE2 = 1
            // index for "Attribute2" attribute
    };

    enum {
        ATTRIBUTE_ID_ATTRIBUTE1 = 0,
            // id for "Attribute1" attribute
        ATTRIBUTE_ID_ATTRIBUTE2 = 1
            // id for "Attribute2" attribute
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "MySequenceWithArray")

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
    explicit MySequenceWithArray(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'MySequenceWithArray' having the default
        // value.  Use the optionally specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    MySequenceWithArray(const MySequenceWithArray& original,
                        bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'MySequenceWithArray' having the value of
        // the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~MySequenceWithArray();
        // Destroy this object.

    // MANIPULATORS
    MySequenceWithArray& operator=(const MySequenceWithArray& rhs);
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
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'name' of the
        // specified 'nameLength', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return the value
        // returned from the invocation of 'manipulator' if 'name' identifies
        // an attribute of this class, and -1 otherwise.

    int& attribute1();
        // Return a reference to the modifiable "Attribute1" attribute of this
        // object.

    bsl::vector<bsl::string>& attribute2();
        // Return a reference to the modifiable "Attribute2" attribute of this
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

    const int& attribute1() const;
        // Return a reference to the non-modifiable "Attribute1" attribute of
        // this object.

    const bsl::vector<bsl::string>& attribute2() const;
        // Return a reference to the non-modifiable "Attribute2" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const MySequenceWithArray& lhs,
                const MySequenceWithArray& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const MySequenceWithArray& lhs,
                const MySequenceWithArray& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const MySequenceWithArray& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace test

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::MySequenceWithArray)

namespace test {

                 // =========================================
                 // class MySequenceWithAnonymousChoiceChoice
                 // =========================================

class MySequenceWithAnonymousChoiceChoice {
    // TODO: Provide annotation

  private:
    union {
        bsls::ObjectBuffer< int >         d_myChoice1;
        bsls::ObjectBuffer< bsl::string > d_myChoice2;
    };

    int               d_selectionId;
    bslma::Allocator *d_allocator_p;

  public:
    // TYPES
    enum {
        NUM_SELECTIONS = 2 // the number of selections in this class
    };

    enum {
        SELECTION_INDEX_MY_CHOICE1 = 0,
            // index for "MyChoice1" selection
        SELECTION_INDEX_MY_CHOICE2 = 1
            // index for "MyChoice2" selection
    };

    enum {
        SELECTION_ID_UNDEFINED = -1,

        SELECTION_ID_MY_CHOICE1 = 0,
            // id for "MyChoice1" selection
        SELECTION_ID_MY_CHOICE2 = 1
            // id for "MyChoice2" selection
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "MySequenceWithAnonymousChoiceChoice")

    static const bdlat_SelectionInfo SELECTION_INFO_ARRAY[];
        // selection information for each selection

  public:
    // CLASS METHODS
    static const bdlat_SelectionInfo *lookupSelectionInfo(int id);
        // Return selection information for the selection indicated by the
        // specified 'id' if the selection exists, and 0 otherwise.

    static const bdlat_SelectionInfo *lookupSelectionInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return selection information for the selection indicated by the
        // specified 'name' of the specified 'nameLength' if the selection
        // exists, and 0 otherwise.

    // CREATORS
    explicit
    MySequenceWithAnonymousChoiceChoice(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'MySequenceWithAnonymousChoiceChoice'
        // having the default value.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    MySequenceWithAnonymousChoiceChoice(
               const MySequenceWithAnonymousChoiceChoice&  original,
               bslma::Allocator                           *basicAllocator = 0);
        // Create an object of type 'MySequenceWithAnonymousChoiceChoice'
        // having the value of the specified 'original' object.  Use the
        // optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    ~MySequenceWithAnonymousChoiceChoice();
        // Destroy this object.

    // MANIPULATORS
    MySequenceWithAnonymousChoiceChoice& operator=(
                               const MySequenceWithAnonymousChoiceChoice& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    void reset();
        // Reset this object to the default value (i.e., its value upon default
        // construction).

    int makeSelection(int selectionId);
        // Set the value of this object to be the default for the selection
        // indicated by the specified 'selectionId'.  Return 0 on success, and
        // non-zero value otherwise (i.e., the selection is not found).

    int makeSelection(const char *name, int nameLength);
        // Set the value of this object to be the default for the selection
        // indicated by the specified 'name' of the specified 'nameLength'.
        // Return 0 on success, and non-zero value otherwise (i.e., the
        // selection is not found).

    int& makeMyChoice1();
    int& makeMyChoice1(int value);
        // Set the value of this object to be a "MyChoice1" value.  Optionally
        // specify the 'value' of the "MyChoice1".  If 'value' is not
        // specified, the default "MyChoice1" value is used.

    bsl::string& makeMyChoice2();
    bsl::string& makeMyChoice2(const bsl::string& value);
        // Set the value of this object to be a "MyChoice2" value.  Optionally
        // specify the 'value' of the "MyChoice2".  If 'value' is not
        // specified, the default "MyChoice2" value is used.

    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.

    int& myChoice1();
        // Return a reference to the modifiable "MyChoice1" selection of this
        // object if "MyChoice1" is the current selection.  The behavior is
        // undefined unless "MyChoice1" is the selection of this object.

    bsl::string& myChoice2();
        // Return a reference to the modifiable "MyChoice2" selection of this
        // object if "MyChoice2" is the current selection.  The behavior is
        // undefined unless "MyChoice2" is the selection of this object.

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

    int selectionId() const;
        // Return the id of the current selection if the selection is defined,
        // and -1 otherwise.

    template<class ACCESSOR>
    int accessSelection(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' on the non-modifiable selection,
        // supplying 'accessor' with the corresponding selection information
        // structure.  Return the value returned from the invocation of
        // 'accessor' if this object has a defined selection, and -1 otherwise.

    const int& myChoice1() const;
        // Return a reference to the non-modifiable "MyChoice1" selection of
        // this object if "MyChoice1" is the current selection.  The behavior
        // is undefined unless "MyChoice1" is the selection of this object.

    const bsl::string& myChoice2() const;
        // Return a reference to the non-modifiable "MyChoice2" selection of
        // this object if "MyChoice2" is the current selection.  The behavior
        // is undefined unless "MyChoice2" is the selection of this object.

    bool isMyChoice1Value() const;
        // Return 'true' if the value of this object is a "MyChoice1" value,
        // and return 'false' otherwise.

    bool isMyChoice2Value() const;
        // Return 'true' if the value of this object is a "MyChoice2" value,
        // and return 'false' otherwise.

    bool isUndefinedValue() const;
        // Return 'true' if the value of this object is undefined, and 'false'
        // otherwise.
};

// FREE OPERATORS
inline
bool operator==(const MySequenceWithAnonymousChoiceChoice& lhs,
                const MySequenceWithAnonymousChoiceChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'MySequenceWithAnonymousChoiceChoice'
    // objects have the same value if either the selections in both objects
    // have the same ids and the same values, or both selections are undefined.

inline
bool operator!=(const MySequenceWithAnonymousChoiceChoice& lhs,
                const MySequenceWithAnonymousChoiceChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
bsl::ostream& operator<<(bsl::ostream&                              stream,
                         const MySequenceWithAnonymousChoiceChoice& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace test

// TRAITS

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_TRAITS(
                                     test::MySequenceWithAnonymousChoiceChoice)

namespace test {

                               // ==============
                               // class Employee
                               // ==============

class Employee {
    // TODO: Provide annotation

  private:
    bsl::string  d_name;
    Address      d_homeAddress;
    int          d_age;

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
        ATTRIBUTE_INDEX_AGE = 2
            // index for "Age" attribute
    };

    enum {
        ATTRIBUTE_ID_NAME = 0,
            // id for "Name" attribute
        ATTRIBUTE_ID_HOME_ADDRESS = 1,
            // id for "HomeAddress" attribute
        ATTRIBUTE_ID_AGE = 2
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
        // Create an object of type 'Employee' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Employee(const Employee& original,
             bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Employee' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~Employee();
        // Destroy this object.

    // MANIPULATORS
    Employee& operator=(const Employee& rhs);
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
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'name' of the
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
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace test

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::Employee)

namespace test {

                              // ===============
                              // class BigRecord
                              // ===============

class BigRecord {
    // A bigger record containing an array of smaller records

  private:
    bsl::string               d_name;
    bsl::vector<BasicRecord>  d_array;

  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 2 // the number of attributes in this class
    };

    enum {
        ATTRIBUTE_INDEX_NAME = 0,
            // index for "Name" attribute
        ATTRIBUTE_INDEX_ARRAY = 1
            // index for "Array" attribute
    };

    enum {
        ATTRIBUTE_ID_NAME = 0,
            // id for "Name" attribute
        ATTRIBUTE_ID_ARRAY = 1
            // id for "Array" attribute
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "BigRecord")

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
    explicit BigRecord(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BigRecord' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    BigRecord(const BigRecord& original,
              bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BigRecord' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~BigRecord();
        // Destroy this object.

    // MANIPULATORS
    BigRecord& operator=(const BigRecord& rhs);
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
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'name' of the
        // specified 'nameLength', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return the value
        // returned from the invocation of 'manipulator' if 'name' identifies
        // an attribute of this class, and -1 otherwise.

    bsl::string& name();
        // Return a reference to the modifiable "Name" attribute of this
        // object.

    bsl::vector<BasicRecord>& array();
        // Return a reference to the modifiable "Array" attribute of this
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

    const bsl::vector<BasicRecord>& array() const;
        // Return a reference to the non-modifiable "Array" attribute of this
        // object.
};

// FREE OPERATORS
inline
bool operator==(const BigRecord& lhs, const BigRecord& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const BigRecord& lhs, const BigRecord& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const BigRecord& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace test

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::BigRecord)

namespace test {

                    // ===================================
                    // class MySequenceWithAnonymousChoice
                    // ===================================

class MySequenceWithAnonymousChoice {
    // TODO: Provide annotation

  private:
    int                                  d_attribute1;
    MySequenceWithAnonymousChoiceChoice  d_choice;
    bsl::string                          d_attribute2;

  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 3 // the number of attributes in this class
    };

    enum {
        ATTRIBUTE_INDEX_ATTRIBUTE1 = 0,
            // index for "Attribute1" attribute
        ATTRIBUTE_INDEX_CHOICE = 1,
            // index for "Choice" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE2 = 2
            // index for "Attribute2" attribute
    };

    enum {
        ATTRIBUTE_ID_ATTRIBUTE1 = 0,
            // id for "Attribute1" attribute
        ATTRIBUTE_ID_CHOICE = 1,
            // id for "Choice" attribute
        ATTRIBUTE_ID_ATTRIBUTE2 = 2
            // id for "Attribute2" attribute
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "MySequenceWithAnonymousChoice")

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
    explicit
    MySequenceWithAnonymousChoice(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'MySequenceWithAnonymousChoice' having the
        // default value.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    MySequenceWithAnonymousChoice(
                     const MySequenceWithAnonymousChoice&  original,
                     bslma::Allocator                     *basicAllocator = 0);
        // Create an object of type 'MySequenceWithAnonymousChoice' having the
        // value of the specified 'original' object.  Use the optionally
        // specified 'basicAllocator' to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.

    ~MySequenceWithAnonymousChoice();
        // Destroy this object.

    // MANIPULATORS
    MySequenceWithAnonymousChoice& operator=(
                                     const MySequenceWithAnonymousChoice& rhs);
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
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'name' of the
        // specified 'nameLength', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return the value
        // returned from the invocation of 'manipulator' if 'name' identifies
        // an attribute of this class, and -1 otherwise.

    int& attribute1();
        // Return a reference to the modifiable "Attribute1" attribute of this
        // object.

    MySequenceWithAnonymousChoiceChoice& choice();
        // Return a reference to the modifiable "Choice" attribute of this
        // object.

    bsl::string& attribute2();
        // Return a reference to the modifiable "Attribute2" attribute of this
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

    const int& attribute1() const;
        // Return a reference to the non-modifiable "Attribute1" attribute of
        // this object.

    const MySequenceWithAnonymousChoiceChoice& choice() const;
        // Return a reference to the non-modifiable "Choice" attribute of this
        // object.

    const bsl::string& attribute2() const;
        // Return a reference to the non-modifiable "Attribute2" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const MySequenceWithAnonymousChoice& lhs,
                const MySequenceWithAnonymousChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const MySequenceWithAnonymousChoice& lhs,
                const MySequenceWithAnonymousChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream&
operator<<(bsl::ostream& stream, const MySequenceWithAnonymousChoice& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace test

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::MySequenceWithAnonymousChoice)

namespace test {

                            // ===================
                            // class TimingRequest
                            // ===================

class TimingRequest {
    // A choice record representative of a typical request object.

  private:
    union {
        bsls::ObjectBuffer< Sqrt >        d_sqrt;
        bsls::ObjectBuffer< BasicRecord > d_basic;
        bsls::ObjectBuffer< BigRecord >   d_big;
    };

    int               d_selectionId;
    bslma::Allocator *d_allocator_p;

  public:
    // TYPES
    enum {
        NUM_SELECTIONS = 3 // the number of selections in this class
    };

    enum {
        SELECTION_INDEX_SQRT = 0,
            // index for "Sqrt" selection
        SELECTION_INDEX_BASIC = 1,
            // index for "Basic" selection
        SELECTION_INDEX_BIG = 2
            // index for "Big" selection
    };

    enum {
        SELECTION_ID_UNDEFINED = -1,

        SELECTION_ID_SQRT = 0,
            // id for "Sqrt" selection
        SELECTION_ID_BASIC = 1,
            // id for "Basic" selection
        SELECTION_ID_BIG = 2
            // id for "Big" selection
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "TimingRequest")

    static const bdlat_SelectionInfo SELECTION_INFO_ARRAY[];
        // selection information for each selection

  public:
    // CLASS METHODS
    static const bdlat_SelectionInfo *lookupSelectionInfo(int id);
        // Return selection information for the selection indicated by the
        // specified 'id' if the selection exists, and 0 otherwise.

    static const bdlat_SelectionInfo *lookupSelectionInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return selection information for the selection indicated by the
        // specified 'name' of the specified 'nameLength' if the selection
        // exists, and 0 otherwise.

    // CREATORS
    explicit TimingRequest(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'TimingRequest' having the default value.
        // Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    TimingRequest(const TimingRequest& original,
                 bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'TimingRequest' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~TimingRequest();
        // Destroy this object.

    // MANIPULATORS
    TimingRequest& operator=(const TimingRequest& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    void reset();
        // Reset this object to the default value (i.e., its value upon default
        // construction).

    int makeSelection(int selectionId);
        // Set the value of this object to be the default for the selection
        // indicated by the specified 'selectionId'.  Return 0 on success, and
        // non-zero value otherwise (i.e., the selection is not found).

    int makeSelection(const char *name, int nameLength);
        // Set the value of this object to be the default for the selection
        // indicated by the specified 'name' of the specified 'nameLength'.
        // Return 0 on success, and non-zero value otherwise (i.e., the
        // selection is not found).

    Sqrt& makeSqrt();
    Sqrt& makeSqrt(const Sqrt& value);
        // Set the value of this object to be a "Sqrt" value.  Optionally
        // specify the 'value' of the "Sqrt".  If 'value' is not specified, the
        // default "Sqrt" value is used.

    BasicRecord& makeBasic();
    BasicRecord& makeBasic(const BasicRecord& value);
        // Set the value of this object to be a "Basic" value.  Optionally
        // specify the 'value' of the "Basic".  If 'value' is not specified,
        // the default "Basic" value is used.

    BigRecord& makeBig();
    BigRecord& makeBig(const BigRecord& value);
        // Set the value of this object to be a "Big" value.  Optionally
        // specify the 'value' of the "Big".  If 'value' is not specified, the
        // default "Big" value is used.

    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.

    Sqrt& sqrt();
        // Return a reference to the modifiable "Sqrt" selection of this object
        // if "Sqrt" is the current selection.  The behavior is undefined
        // unless "Sqrt" is the selection of this object.

    BasicRecord& basic();
        // Return a reference to the modifiable "Basic" selection of this
        // object if "Basic" is the current selection.  The behavior is
        // undefined unless "Basic" is the selection of this object.

    BigRecord& big();
        // Return a reference to the modifiable "Big" selection of this object
        // if "Big" is the current selection.  The behavior is undefined unless
        // "Big" is the selection of this object.

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

    int selectionId() const;
        // Return the id of the current selection if the selection is defined,
        // and -1 otherwise.

    template<class ACCESSOR>
    int accessSelection(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' on the non-modifiable selection,
        // supplying 'accessor' with the corresponding selection information
        // structure.  Return the value returned from the invocation of
        // 'accessor' if this object has a defined selection, and -1 otherwise.

    const Sqrt& sqrt() const;
        // Return a reference to the non-modifiable "Sqrt" selection of this
        // object if "Sqrt" is the current selection.  The behavior is
        // undefined unless "Sqrt" is the selection of this object.

    const BasicRecord& basic() const;
        // Return a reference to the non-modifiable "Basic" selection of this
        // object if "Basic" is the current selection.  The behavior is
        // undefined unless "Basic" is the selection of this object.

    const BigRecord& big() const;
        // Return a reference to the non-modifiable "Big" selection of this
        // object if "Big" is the current selection.  The behavior is undefined
        // unless "Big" is the selection of this object.

    bool isSqrtValue() const;
        // Return 'true' if the value of this object is a "Sqrt" value, and
        // return 'false' otherwise.

    bool isBasicValue() const;
        // Return 'true' if the value of this object is a "Basic" value, and
        // return 'false' otherwise.

    bool isBigValue() const;
        // Return 'true' if the value of this object is a "Big" value, and
        // return 'false' otherwise.

    bool isUndefinedValue() const;
        // Return 'true' if the value of this object is undefined, and 'false'
        // otherwise.
};

// FREE OPERATORS
inline
bool operator==(const TimingRequest& lhs, const TimingRequest& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'TimingRequest' objects have the same
    // value if either the selections in both objects have the same ids and
    // the same values, or both selections are undefined.

inline
bool operator!=(const TimingRequest& lhs, const TimingRequest& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const TimingRequest& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace test

// TRAITS

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_TRAITS(test::TimingRequest)

namespace test {

                               // ==============
                               // class Messages
                               // ==============

struct Messages {
    // This class serves as a place holder to reserve a type having the same
    // name as this component.  Doing so ensures that such a type cannot be
    // defined outside of this component in the current namespace.
};

}  // close namespace test

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace test {

                               // --------------
                               // class MyChoice
                               // --------------

// CREATORS
inline
MyChoice::MyChoice(bslma::Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
MyChoice::MyChoice(
    const MyChoice& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1: {
        new (d_selection1.buffer())
            int(original.d_selection1.object());
      } break;
      case SELECTION_ID_SELECTION2: {
        new (d_selection2.buffer())
            bsl::string(
                original.d_selection2.object(), d_allocator_p);
      } break;
      default:
        BSLS_ASSERT_SAFE(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

inline
MyChoice::~MyChoice()
{
    reset();
}

// MANIPULATORS
inline
MyChoice&
MyChoice::operator=(const MyChoice& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_SELECTION1: {
            makeSelection1(rhs.d_selection1.object());
          } break;
          case SELECTION_ID_SELECTION2: {
            makeSelection2(rhs.d_selection2.object());
          } break;
          default:
            BSLS_ASSERT_SAFE(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }
    return *this;
}

inline
void MyChoice::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1: {
        // no destruction required
      } break;
      case SELECTION_ID_SELECTION2: {
        typedef bsl::string Type;
        d_selection2.object().~Type();
      } break;
      default:
        BSLS_ASSERT_SAFE(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

inline
int MyChoice::makeSelection(int selectionId)
{
    enum { NOT_FOUND = -1, SUCCESS = 0 };

    switch (selectionId) {
      case SELECTION_ID_SELECTION1: {
        makeSelection1();
      } break;
      case SELECTION_ID_SELECTION2: {
        makeSelection2();
      } break;
      case SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
    return SUCCESS;
}

inline
int MyChoice::makeSelection(const char *name, int nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdlat_SelectionInfo *selectionInfo =
           lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return NOT_FOUND;                                              // RETURN
    }

    return makeSelection(selectionInfo->d_id);
}

inline
int& MyChoice::makeSelection1()
{
    if (SELECTION_ID_SELECTION1 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection1.object());
    }
    else {
        reset();
        new (d_selection1.buffer())
            int();

        d_selectionId = SELECTION_ID_SELECTION1;
    }

    return d_selection1.object();
}

inline
int& MyChoice::makeSelection1(int value)
{
    if (SELECTION_ID_SELECTION1 == d_selectionId) {
        d_selection1.object() = value;
    }
    else {
        reset();
        new (d_selection1.buffer())
                int(value);
        d_selectionId = SELECTION_ID_SELECTION1;
    }

    return d_selection1.object();
}

inline
bsl::string& MyChoice::makeSelection2()
{
    if (SELECTION_ID_SELECTION2 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection2.object());
    }
    else {
        reset();
        new (d_selection2.buffer())
                bsl::string(d_allocator_p);

        d_selectionId = SELECTION_ID_SELECTION2;
    }

    return d_selection2.object();
}

inline
bsl::string& MyChoice::makeSelection2(const bsl::string& value)
{
    if (SELECTION_ID_SELECTION2 == d_selectionId) {
        d_selection2.object() = value;
    }
    else {
        reset();
        new (d_selection2.buffer())
                bsl::string(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION2;
    }

    return d_selection2.object();
}

template <class MANIPULATOR>
inline
int MyChoice::manipulateSelection(MANIPULATOR& manipulator)
{
    enum { FAILURE = -1, SUCCESS = 0 };

    switch (d_selectionId) {
      case MyChoice::SELECTION_ID_SELECTION1:
        return manipulator(&d_selection1.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1]);    // RETURN
      case MyChoice::SELECTION_ID_SELECTION2:
        return manipulator(&d_selection2.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2]);    // RETURN
      default:
        BSLS_ASSERT_SAFE(MyChoice::SELECTION_ID_UNDEFINED == d_selectionId);
        return FAILURE;                                               // RETURN
    }
}

inline
int& MyChoice::selection1()
{
    BSLS_ASSERT_SAFE(SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

inline
bsl::string& MyChoice::selection2()
{
    BSLS_ASSERT_SAFE(SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}

// ACCESSORS
inline
int MyChoice::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
inline
int MyChoice::accessSelection(ACCESSOR& accessor) const
{
    enum { FAILURE = -1, SUCCESS = 0 };

    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1:
        return accessor(d_selection1.object(),
                        SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1]);
                                                                      // RETURN
      case SELECTION_ID_SELECTION2:
        return accessor(d_selection2.object(),
                        SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2]);
                                                                      // RETURN
      default:
        BSLS_ASSERT_SAFE(SELECTION_ID_UNDEFINED == d_selectionId);
        return FAILURE;                                               // RETURN
    }
}

inline
const int& MyChoice::selection1() const
{
    BSLS_ASSERT_SAFE(SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

inline
const bsl::string& MyChoice::selection2() const
{
    BSLS_ASSERT_SAFE(SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}

inline
bool MyChoice::isSelection1Value() const
{
    return SELECTION_ID_SELECTION1 == d_selectionId;
}

inline
bool MyChoice::isSelection2Value() const
{
    return SELECTION_ID_SELECTION2 == d_selectionId;
}

inline
bool MyChoice::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}

                        // ----------------------------
                        // class MySequenceWithNullable
                        // ----------------------------

// CREATORS
inline
MySequenceWithNullable::MySequenceWithNullable(
                                              bslma::Allocator *basicAllocator)
: d_attribute1()
, d_attribute2(basicAllocator)
{
}

inline
MySequenceWithNullable::MySequenceWithNullable(
        const MySequenceWithNullable& original,
        bslma::Allocator *basicAllocator)
: d_attribute1(original.d_attribute1)
, d_attribute2(original.d_attribute2, basicAllocator)
{
}

inline
MySequenceWithNullable::~MySequenceWithNullable()
{
}

// MANIPULATORS
inline
MySequenceWithNullable&
MySequenceWithNullable::operator=(const MySequenceWithNullable& rhs)
{
    if (this != &rhs) {
        d_attribute1 = rhs.d_attribute1;
        d_attribute2 = rhs.d_attribute2;
    }
    return *this;
}

inline
void MySequenceWithNullable::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_attribute1);
    bdlat_ValueTypeFunctions::reset(&d_attribute2);
}

template <class MANIPULATOR>
inline
int MySequenceWithNullable::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute1,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_attribute2,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
inline
int
MySequenceWithNullable::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return manipulator(&d_attribute1,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return manipulator(&d_attribute2,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

template <class MANIPULATOR>
inline
int MySequenceWithNullable::manipulateAttribute(
        MANIPULATOR&  manipulator,
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
int& MySequenceWithNullable::attribute1()
{
    return d_attribute1;
}

inline
bdlb::NullableValue<bsl::string>& MySequenceWithNullable::attribute2()
{
    return d_attribute2;
}

// ACCESSORS
template <class ACCESSOR>
inline
int MySequenceWithNullable::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute1,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_attribute2,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
inline
int MySequenceWithNullable::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return accessor(d_attribute1,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return accessor(d_attribute2,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

template <class ACCESSOR>
inline
int MySequenceWithNullable::accessAttribute(
        ACCESSOR&   accessor,
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
const int& MySequenceWithNullable::attribute1() const
{
    return d_attribute1;
}

inline
const bdlb::NullableValue<bsl::string>&
MySequenceWithNullable::attribute2() const
{
    return d_attribute2;
}

                               // -------------
                               // class Address
                               // -------------

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
        const Address& original,
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
      case ATTRIBUTE_ID_STREET: {
        return manipulator(&d_street,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STREET]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_CITY: {
        return manipulator(&d_city,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CITY]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_STATE: {
        return manipulator(&d_state,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATE]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

template <class MANIPULATOR>
inline
int Address::manipulateAttribute(
        MANIPULATOR&  manipulator,
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
      case ATTRIBUTE_ID_STREET: {
        return accessor(d_street,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STREET]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_CITY: {
        return accessor(d_city,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CITY]);  // RETURN
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_STATE: {
        return accessor(d_state, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATE]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

template <class ACCESSOR>
inline
int Address::accessAttribute(
        ACCESSOR&   accessor,
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

                              // ----------------
                              // class MySequence
                              // ----------------

// CREATORS
inline
MySequence::MySequence(bslma::Allocator *basicAllocator)
: d_attribute1()
, d_attribute2(basicAllocator)
{
}

inline
MySequence::MySequence(
        const MySequence& original,
        bslma::Allocator *basicAllocator)
: d_attribute1(original.d_attribute1)
, d_attribute2(original.d_attribute2, basicAllocator)
{
}

inline
MySequence::~MySequence()
{
}

// MANIPULATORS
inline
MySequence&
MySequence::operator=(const MySequence& rhs)
{
    if (this != &rhs) {
        d_attribute1 = rhs.d_attribute1;
        d_attribute2 = rhs.d_attribute2;
    }
    return *this;
}

inline
void MySequence::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_attribute1);
    bdlat_ValueTypeFunctions::reset(&d_attribute2);
}

template <class MANIPULATOR>
inline
int MySequence::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute1,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_attribute2,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
inline
int MySequence::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return manipulator(&d_attribute1,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return manipulator(&d_attribute2,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

template <class MANIPULATOR>
inline
int MySequence::manipulateAttribute(
        MANIPULATOR&  manipulator,
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
int& MySequence::attribute1()
{
    return d_attribute1;
}

inline
bsl::string& MySequence::attribute2()
{
    return d_attribute2;
}

// ACCESSORS
template <class ACCESSOR>
inline
int MySequence::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute1,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_attribute2,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
inline
int MySequence::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return accessor(d_attribute1,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return accessor(d_attribute2,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

template <class ACCESSOR>
inline
int MySequence::accessAttribute(
        ACCESSOR&   accessor,
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
const int& MySequence::attribute1() const
{
    return d_attribute1;
}

inline
const bsl::string& MySequence::attribute2() const
{
    return d_attribute2;
}

                        // ----------------------------
                        // class MySequenceWithNillable
                        // ----------------------------

// CREATORS
inline
MySequenceWithNillable::MySequenceWithNillable(
                                              bslma::Allocator *basicAllocator)
: d_attribute1()
, d_myNillable(basicAllocator)
, d_attribute2(basicAllocator)
{
}

inline
MySequenceWithNillable::MySequenceWithNillable(
        const MySequenceWithNillable& original,
        bslma::Allocator *basicAllocator)
: d_attribute1(original.d_attribute1)
, d_myNillable(original.d_myNillable, basicAllocator)
, d_attribute2(original.d_attribute2, basicAllocator)
{
}

inline
MySequenceWithNillable::~MySequenceWithNillable()
{
}

// MANIPULATORS
inline
MySequenceWithNillable&
MySequenceWithNillable::operator=(const MySequenceWithNillable& rhs)
{
    if (this != &rhs) {
        d_attribute1 = rhs.d_attribute1;
        d_myNillable = rhs.d_myNillable;
        d_attribute2 = rhs.d_attribute2;
    }
    return *this;
}

inline
void MySequenceWithNillable::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_attribute1);
    bdlat_ValueTypeFunctions::reset(&d_myNillable);
    bdlat_ValueTypeFunctions::reset(&d_attribute2);
}

template <class MANIPULATOR>
inline
int MySequenceWithNillable::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute1,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_myNillable,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MY_NILLABLE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_attribute2,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
inline
int MySequenceWithNillable::manipulateAttribute(MANIPULATOR& manipulator,
                                                int          id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return manipulator(&d_attribute1,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_MY_NILLABLE: {
        return manipulator(&d_myNillable,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MY_NILLABLE]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return manipulator(&d_attribute2,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

template <class MANIPULATOR>
inline
int MySequenceWithNillable::manipulateAttribute(
        MANIPULATOR&  manipulator,
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
int& MySequenceWithNillable::attribute1()
{
    return d_attribute1;
}

inline
bdlb::NullableValue<bsl::string>& MySequenceWithNillable::myNillable()
{
    return d_myNillable;
}

inline
bsl::string& MySequenceWithNillable::attribute2()
{
    return d_attribute2;
}

// ACCESSORS
template <class ACCESSOR>
inline
int MySequenceWithNillable::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute1,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_myNillable,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MY_NILLABLE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_attribute2,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
inline
int MySequenceWithNillable::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return accessor(d_attribute1,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_MY_NILLABLE: {
        return accessor(d_myNillable,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MY_NILLABLE]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return accessor(d_attribute2,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

template <class ACCESSOR>
inline
int MySequenceWithNillable::accessAttribute(
        ACCESSOR&   accessor,
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
const int& MySequenceWithNillable::attribute1() const
{
    return d_attribute1;
}

inline
const bdlb::NullableValue<bsl::string>&
MySequenceWithNillable::myNillable() const
{
    return d_myNillable;
}

inline
const bsl::string& MySequenceWithNillable::attribute2() const
{
    return d_attribute2;
}

                            // -------------------
                            // class MyEnumeration
                            // -------------------

// CLASS METHODS
inline
int MyEnumeration::fromInt(MyEnumeration::Value *result, int number)
{
    enum { SUCCESS = 0, NOT_FOUND = 1 };

    switch (number) {
      case MyEnumeration::VALUE1:
      case MyEnumeration::VALUE2:
        *result = (MyEnumeration::Value)number;
        return SUCCESS;                                               // RETURN
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

inline
bsl::ostream& MyEnumeration::print(bsl::ostream&      stream,
                                 MyEnumeration::Value value)
{
    return stream << toString(value);
}

inline
const char *MyEnumeration::toString(MyEnumeration::Value value)
{
    switch (value) {
      case VALUE1: {
        return "VALUE1";                                              // RETURN
      } break;
      case VALUE2: {
        return "VALUE2";                                              // RETURN
      } break;
      default:
        BSLS_ASSERT_SAFE(!"encountered out-of-bound enumerated value");
    }

    return 0;
}

                                 // ----------
                                 // class Sqrt
                                 // ----------

// CREATORS
inline
Sqrt::Sqrt()
: d_value()
{
}

inline
Sqrt::Sqrt(const Sqrt& original)
: d_value(original.d_value)
{
}

inline
Sqrt::~Sqrt()
{
}

// MANIPULATORS
inline
Sqrt&
Sqrt::operator=(const Sqrt& rhs)
{
    if (this != &rhs) {
        d_value = rhs.d_value;
    }
    return *this;
}

inline
void Sqrt::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_value);
}

template <class MANIPULATOR>
inline
int Sqrt::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_value, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALUE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
inline
int Sqrt::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_VALUE: {
        return manipulator(&d_value,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALUE]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

template <class MANIPULATOR>
inline
int Sqrt::manipulateAttribute(
        MANIPULATOR&  manipulator,
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
double& Sqrt::value()
{
    return d_value;
}

// ACCESSORS
template <class ACCESSOR>
inline
int Sqrt::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_value, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALUE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
inline
int Sqrt::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_VALUE: {
        return accessor(d_value, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALUE]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

template <class ACCESSOR>
inline
int Sqrt::accessAttribute(
        ACCESSOR&   accessor,
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
const double& Sqrt::value() const
{
    return d_value;
}

                           // ----------------------
                           // class CustomizedString
                           // ----------------------

// CREATORS
inline
int CustomizedString::checkRestrictions(const bsl::string& value)
{
    enum { SUCCESS = 0, FAILURE = -1 };

    if (25 < value.size()) {
        return FAILURE;                                               // RETURN
    }

    return SUCCESS;
}

inline
CustomizedString::CustomizedString(bslma::Allocator *basicAllocator)
: d_value(basicAllocator)
{
}

inline
CustomizedString::CustomizedString(const CustomizedString&  original,
                                   bslma::Allocator        *basicAllocator)
: d_value(original.d_value, basicAllocator)
{
}

inline
CustomizedString::CustomizedString(const bsl::string&  value,
                                   bslma::Allocator   *basicAllocator)
: d_value(value, basicAllocator)
{
    BSLS_ASSERT_SAFE(checkRestrictions(value) == 0);
}

inline
CustomizedString::~CustomizedString()
{
}

// MANIPULATORS
inline
CustomizedString& CustomizedString::operator=(const CustomizedString& rhs)
{
    d_value = rhs.d_value;
    return *this;
}

inline
void CustomizedString::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_value);
}

inline
int CustomizedString::fromString(const bsl::string& value)
{
    int ret = checkRestrictions(value);
    if (0 == ret) {
        d_value = value;
    }

    return ret;
}

// ACCESSORS
inline
bsl::ostream& CustomizedString::print(bsl::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    return bdlb::PrintMethods::print(stream, d_value, level, spacesPerLevel);
}

inline
const bsl::string& CustomizedString::toString() const
{
    return d_value;
}

                             // -----------------
                             // class BasicRecord
                             // -----------------

// CREATORS
inline
BasicRecord::BasicRecord(bslma::Allocator *basicAllocator)
: d_i1()
, d_i2()
, d_dt()
, d_s(basicAllocator)
{
}

inline
BasicRecord::BasicRecord(
        const BasicRecord& original,
        bslma::Allocator *basicAllocator)
: d_i1(original.d_i1)
, d_i2(original.d_i2)
, d_dt(original.d_dt)
, d_s(original.d_s, basicAllocator)
{
}

inline
BasicRecord::~BasicRecord()
{
}

// MANIPULATORS
inline
BasicRecord&
BasicRecord::operator=(const BasicRecord& rhs)
{
    if (this != &rhs) {
        d_i1 = rhs.d_i1;
        d_i2 = rhs.d_i2;
        d_dt = rhs.d_dt;
        d_s = rhs.d_s;
    }
    return *this;
}

inline
void BasicRecord::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_i1);
    bdlat_ValueTypeFunctions::reset(&d_i2);
    bdlat_ValueTypeFunctions::reset(&d_dt);
    bdlat_ValueTypeFunctions::reset(&d_s);
}

template <class MANIPULATOR>
inline
int BasicRecord::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_i1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_I1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_i2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_I2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_dt, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DT]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_s, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_S]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
inline
int BasicRecord::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_I1: {
        return manipulator(&d_i1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_I1]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_I2: {
        return manipulator(&d_i2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_I2]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_DT: {
        return manipulator(&d_dt, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DT]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_S: {
        return manipulator(&d_s, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_S]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

template <class MANIPULATOR>
inline
int BasicRecord::manipulateAttribute(
        MANIPULATOR&  manipulator,
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
int& BasicRecord::i1()
{
    return d_i1;
}

inline
int& BasicRecord::i2()
{
    return d_i2;
}

inline
bdlt::DatetimeTz& BasicRecord::dt()
{
    return d_dt;
}

inline
bsl::string& BasicRecord::s()
{
    return d_s;
}

// ACCESSORS
template <class ACCESSOR>
inline
int BasicRecord::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_i1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_I1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_i2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_I2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_dt, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DT]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_s, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_S]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
inline
int BasicRecord::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_I1: {
        return accessor(d_i1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_I1]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_I2: {
        return accessor(d_i2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_I2]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_DT: {
        return accessor(d_dt, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DT]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_S: {
        return accessor(d_s, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_S]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

template <class ACCESSOR>
inline
int BasicRecord::accessAttribute(
        ACCESSOR&   accessor,
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
const int& BasicRecord::i1() const
{
    return d_i1;
}

inline
const int& BasicRecord::i2() const
{
    return d_i2;
}

inline
const bdlt::DatetimeTz& BasicRecord::dt() const
{
    return d_dt;
}

inline
const bsl::string& BasicRecord::s() const
{
    return d_s;
}

                        // ----------------------------
                        // class BasicRecordWithVariant
                        // ----------------------------

// CREATORS
inline
BasicRecordWithVariant::BasicRecordWithVariant(
                                              bslma::Allocator *basicAllocator)
: d_i1()
, d_i2()
, d_dt()
, d_s(basicAllocator)
{
}

inline
BasicRecordWithVariant::BasicRecordWithVariant(
        const BasicRecordWithVariant& original,
        bslma::Allocator *basicAllocator)
: d_i1(original.d_i1)
, d_i2(original.d_i2)
, d_dt(original.d_dt)
, d_s(original.d_s, basicAllocator)
{
}

inline
BasicRecordWithVariant::~BasicRecordWithVariant()
{
}

// MANIPULATORS
inline
BasicRecordWithVariant&
BasicRecordWithVariant::operator=(const BasicRecordWithVariant& rhs)
{
    if (this != &rhs) {
        d_i1 = rhs.d_i1;
        d_i2 = rhs.d_i2;
        d_dt = rhs.d_dt;
        d_s = rhs.d_s;
    }
    return *this;
}

inline
void BasicRecordWithVariant::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_i1);
    bdlat_ValueTypeFunctions::reset(&d_i2);
    bdlat_ValueTypeFunctions::reset(&d_dt);
    bdlat_ValueTypeFunctions::reset(&d_s);
}

template <class MANIPULATOR>
inline
int BasicRecordWithVariant::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_i1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_I1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_i2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_I2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_dt, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DT]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_s, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_S]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
inline
int BasicRecordWithVariant::manipulateAttribute(MANIPULATOR& manipulator,
                                                int          id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_I1: {
        return manipulator(&d_i1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_I1]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_I2: {
        return manipulator(&d_i2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_I2]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_DT: {
        return manipulator(&d_dt, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DT]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_S: {
        return manipulator(&d_s, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_S]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

template <class MANIPULATOR>
inline
int BasicRecordWithVariant::manipulateAttribute(
        MANIPULATOR&  manipulator,
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
int& BasicRecordWithVariant::i1()
{
    return d_i1;
}

inline
int& BasicRecordWithVariant::i2()
{
    return d_i2;
}

inline
bdlb::Variant<bdlt::Datetime, bdlt::DatetimeTz>& BasicRecordWithVariant::dt()
{
    return d_dt;
}

inline
bsl::string& BasicRecordWithVariant::s()
{
    return d_s;
}

// ACCESSORS
template <class ACCESSOR>
inline
int BasicRecordWithVariant::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_i1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_I1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_i2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_I2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    if (d_dt.is<bdlt::Datetime>()) {
        ret = accessor(d_dt.the<bdlt::Datetime>(),
                       ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DT]);
        if (ret) {
            return ret;                                               // RETURN
        }
    }
    else {
        ret = accessor(d_dt.the<bdlt::DatetimeTz>(),
                       ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DT]);
        if (ret) {
            return ret;                                               // RETURN
        }
    }

    ret = accessor(d_s, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_S]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
inline
int BasicRecordWithVariant::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_I1: {
        return accessor(d_i1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_I1]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_I2: {
        return accessor(d_i2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_I2]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_DT: {
        if (d_dt.is<bdlt::Datetime>()) {
            return accessor(d_dt.the<bdlt::Datetime>(),
                            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DT]);// RETURN
        }
        else {
            return accessor(d_dt.the<bdlt::DatetimeTz>(),
                            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DT]);// RETURN
        }
      } break;
      case ATTRIBUTE_ID_S: {
        return accessor(d_s, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_S]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

template <class ACCESSOR>
inline
int BasicRecordWithVariant::accessAttribute(
        ACCESSOR&   accessor,
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
const int& BasicRecordWithVariant::i1() const
{
    return d_i1;
}

inline
const int& BasicRecordWithVariant::i2() const
{
    return d_i2;
}

inline
const bdlb::Variant<bdlt::Datetime, bdlt::DatetimeTz>&
BasicRecordWithVariant::dt() const
{
    return d_dt;
}

inline
const bsl::string& BasicRecordWithVariant::s() const
{
    return d_s;
}

                         // -------------------------
                         // class MySequenceWithArray
                         // -------------------------

// CREATORS
inline
MySequenceWithArray::MySequenceWithArray(bslma::Allocator *basicAllocator)
: d_attribute1()
, d_attribute2(basicAllocator)
{
}

inline
MySequenceWithArray::MySequenceWithArray(
        const MySequenceWithArray& original,
        bslma::Allocator *basicAllocator)
: d_attribute1(original.d_attribute1)
, d_attribute2(original.d_attribute2, basicAllocator)
{
}

inline
MySequenceWithArray::~MySequenceWithArray()
{
}

// MANIPULATORS
inline
MySequenceWithArray&
MySequenceWithArray::operator=(const MySequenceWithArray& rhs)
{
    if (this != &rhs) {
        d_attribute1 = rhs.d_attribute1;
        d_attribute2 = rhs.d_attribute2;
    }
    return *this;
}

inline
void MySequenceWithArray::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_attribute1);
    bdlat_ValueTypeFunctions::reset(&d_attribute2);
}

template <class MANIPULATOR>
inline
int MySequenceWithArray::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute1,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_attribute2,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
inline
int MySequenceWithArray::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return manipulator(&d_attribute1,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return manipulator(&d_attribute2,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

template <class MANIPULATOR>
inline
int MySequenceWithArray::manipulateAttribute(
        MANIPULATOR&  manipulator,
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
int& MySequenceWithArray::attribute1()
{
    return d_attribute1;
}

inline
bsl::vector<bsl::string>& MySequenceWithArray::attribute2()
{
    return d_attribute2;
}

// ACCESSORS
template <class ACCESSOR>
inline
int MySequenceWithArray::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute1,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_attribute2,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
inline
int MySequenceWithArray::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return accessor(d_attribute1,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return accessor(d_attribute2,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

template <class ACCESSOR>
inline
int MySequenceWithArray::accessAttribute(
        ACCESSOR&   accessor,
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
const int& MySequenceWithArray::attribute1() const
{
    return d_attribute1;
}

inline
const bsl::vector<bsl::string>& MySequenceWithArray::attribute2() const
{
    return d_attribute2;
}

                 // -----------------------------------------
                 // class MySequenceWithAnonymousChoiceChoice
                 // -----------------------------------------

// CREATORS
inline
MySequenceWithAnonymousChoiceChoice::MySequenceWithAnonymousChoiceChoice(
                                              bslma::Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
MySequenceWithAnonymousChoiceChoice::MySequenceWithAnonymousChoiceChoice(
    const MySequenceWithAnonymousChoiceChoice& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_MY_CHOICE1: {
        new (d_myChoice1.buffer())
            int(original.d_myChoice1.object());
      } break;
      case SELECTION_ID_MY_CHOICE2: {
        new (d_myChoice2.buffer())
            bsl::string(
                original.d_myChoice2.object(), d_allocator_p);
      } break;
      default:
        BSLS_ASSERT_SAFE(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

inline
MySequenceWithAnonymousChoiceChoice::~MySequenceWithAnonymousChoiceChoice()
{
    reset();
}

// MANIPULATORS
inline
MySequenceWithAnonymousChoiceChoice&
MySequenceWithAnonymousChoiceChoice::operator=(
                                const MySequenceWithAnonymousChoiceChoice& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_MY_CHOICE1: {
            makeMyChoice1(rhs.d_myChoice1.object());
          } break;
          case SELECTION_ID_MY_CHOICE2: {
            makeMyChoice2(rhs.d_myChoice2.object());
          } break;
          default:
            BSLS_ASSERT_SAFE(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }
    return *this;
}

inline
void MySequenceWithAnonymousChoiceChoice::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_MY_CHOICE1: {
        // no destruction required
      } break;
      case SELECTION_ID_MY_CHOICE2: {
        typedef bsl::string Type;
        d_myChoice2.object().~Type();
      } break;
      default:
        BSLS_ASSERT_SAFE(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

inline
int MySequenceWithAnonymousChoiceChoice::makeSelection(int selectionId)
{
    enum { NOT_FOUND = -1, SUCCESS = 0 };

    switch (selectionId) {
      case SELECTION_ID_MY_CHOICE1: {
        makeMyChoice1();
      } break;
      case SELECTION_ID_MY_CHOICE2: {
        makeMyChoice2();
      } break;
      case SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
    return SUCCESS;
}

inline
int MySequenceWithAnonymousChoiceChoice::makeSelection(const char *name,
                                                       int         nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdlat_SelectionInfo *selectionInfo =
           lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return NOT_FOUND;                                              // RETURN
    }

    return makeSelection(selectionInfo->d_id);
}

inline
int& MySequenceWithAnonymousChoiceChoice::makeMyChoice1()
{
    if (SELECTION_ID_MY_CHOICE1 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_myChoice1.object());
    }
    else {
        reset();
        new (d_myChoice1.buffer())
            int();

        d_selectionId = SELECTION_ID_MY_CHOICE1;
    }

    return d_myChoice1.object();
}

inline
int& MySequenceWithAnonymousChoiceChoice::makeMyChoice1(int value)
{
    if (SELECTION_ID_MY_CHOICE1 == d_selectionId) {
        d_myChoice1.object() = value;
    }
    else {
        reset();
        new (d_myChoice1.buffer())
                int(value);
        d_selectionId = SELECTION_ID_MY_CHOICE1;
    }

    return d_myChoice1.object();
}

inline
bsl::string& MySequenceWithAnonymousChoiceChoice::makeMyChoice2()
{
    if (SELECTION_ID_MY_CHOICE2 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_myChoice2.object());
    }
    else {
        reset();
        new (d_myChoice2.buffer())
                bsl::string(d_allocator_p);

        d_selectionId = SELECTION_ID_MY_CHOICE2;
    }

    return d_myChoice2.object();
}

inline
bsl::string&
MySequenceWithAnonymousChoiceChoice::makeMyChoice2(const bsl::string& value)
{
    if (SELECTION_ID_MY_CHOICE2 == d_selectionId) {
        d_myChoice2.object() = value;
    }
    else {
        reset();
        new (d_myChoice2.buffer())
                bsl::string(value, d_allocator_p);
        d_selectionId = SELECTION_ID_MY_CHOICE2;
    }

    return d_myChoice2.object();
}

template <class MANIPULATOR>
inline
int MySequenceWithAnonymousChoiceChoice::manipulateSelection(
                                                      MANIPULATOR& manipulator)
{
    enum { FAILURE = -1, SUCCESS = 0 };

    switch (d_selectionId) {
      case MySequenceWithAnonymousChoiceChoice::SELECTION_ID_MY_CHOICE1:
        return manipulator(&d_myChoice1.object(),
                           SELECTION_INFO_ARRAY[SELECTION_INDEX_MY_CHOICE1]);
                                                                      // RETURN
      case MySequenceWithAnonymousChoiceChoice::SELECTION_ID_MY_CHOICE2:
        return manipulator(&d_myChoice2.object(),
                           SELECTION_INFO_ARRAY[SELECTION_INDEX_MY_CHOICE2]);
                                                                      // RETURN
      default:
        BSLS_ASSERT_SAFE(
 MySequenceWithAnonymousChoiceChoice::SELECTION_ID_UNDEFINED == d_selectionId);
        return FAILURE;                                               // RETURN
    }
}

inline
int& MySequenceWithAnonymousChoiceChoice::myChoice1()
{
    BSLS_ASSERT_SAFE(SELECTION_ID_MY_CHOICE1 == d_selectionId);
    return d_myChoice1.object();
}

inline
bsl::string& MySequenceWithAnonymousChoiceChoice::myChoice2()
{
    BSLS_ASSERT_SAFE(SELECTION_ID_MY_CHOICE2 == d_selectionId);
    return d_myChoice2.object();
}

// ACCESSORS
inline
int MySequenceWithAnonymousChoiceChoice::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
inline
int
MySequenceWithAnonymousChoiceChoice::accessSelection(ACCESSOR& accessor) const
{
    enum { FAILURE = -1, SUCCESS = 0 };

    switch (d_selectionId) {
      case SELECTION_ID_MY_CHOICE1:
        return accessor(d_myChoice1.object(),
                        SELECTION_INFO_ARRAY[SELECTION_INDEX_MY_CHOICE1]);
                                                                      // RETURN
      case SELECTION_ID_MY_CHOICE2:
        return accessor(d_myChoice2.object(),
                        SELECTION_INFO_ARRAY[SELECTION_INDEX_MY_CHOICE2]);
                                                                      // RETURN
      default:
        BSLS_ASSERT_SAFE(SELECTION_ID_UNDEFINED == d_selectionId);
        return FAILURE;                                               // RETURN
    }
}

inline
const int& MySequenceWithAnonymousChoiceChoice::myChoice1() const
{
    BSLS_ASSERT_SAFE(SELECTION_ID_MY_CHOICE1 == d_selectionId);
    return d_myChoice1.object();
}

inline
const bsl::string& MySequenceWithAnonymousChoiceChoice::myChoice2() const
{
    BSLS_ASSERT_SAFE(SELECTION_ID_MY_CHOICE2 == d_selectionId);
    return d_myChoice2.object();
}

inline
bool MySequenceWithAnonymousChoiceChoice::isMyChoice1Value() const
{
    return SELECTION_ID_MY_CHOICE1 == d_selectionId;
}

inline
bool MySequenceWithAnonymousChoiceChoice::isMyChoice2Value() const
{
    return SELECTION_ID_MY_CHOICE2 == d_selectionId;
}

inline
bool MySequenceWithAnonymousChoiceChoice::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}

                               // --------------
                               // class Employee
                               // --------------

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
        const Employee& original,
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

    ret = manipulator(&d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_homeAddress,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_age, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
inline
int Employee::manipulateAttribute(MANIPULATOR& manipulator, int id)
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
      case ATTRIBUTE_ID_AGE: {
        return manipulator(&d_age, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

template <class MANIPULATOR>
inline
int Employee::manipulateAttribute(
        MANIPULATOR&  manipulator,
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
template <class ACCESSOR>
inline
int Employee::accessAttributes(ACCESSOR& accessor) const
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

    ret = accessor(d_age, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
inline
int Employee::accessAttribute(ACCESSOR& accessor, int id) const
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
      case ATTRIBUTE_ID_AGE: {
        return accessor(d_age, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

template <class ACCESSOR>
inline
int Employee::accessAttribute(
        ACCESSOR&   accessor,
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

                              // ---------------
                              // class BigRecord
                              // ---------------

// CREATORS
inline
BigRecord::BigRecord(bslma::Allocator *basicAllocator)
: d_name(basicAllocator)
, d_array(basicAllocator)
{
}

inline
BigRecord::BigRecord(
        const BigRecord& original,
        bslma::Allocator *basicAllocator)
: d_name(original.d_name, basicAllocator)
, d_array(original.d_array, basicAllocator)
{
}

inline
BigRecord::~BigRecord()
{
}

// MANIPULATORS
inline
BigRecord&
BigRecord::operator=(const BigRecord& rhs)
{
    if (this != &rhs) {
        d_name = rhs.d_name;
        d_array = rhs.d_array;
    }
    return *this;
}

inline
void BigRecord::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_name);
    bdlat_ValueTypeFunctions::reset(&d_array);
}

template <class MANIPULATOR>
inline
int BigRecord::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_array, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ARRAY]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
inline
int BigRecord::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_NAME: {
        return manipulator(&d_name,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ARRAY: {
        return manipulator(&d_array,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ARRAY]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

template <class MANIPULATOR>
inline
int BigRecord::manipulateAttribute(
        MANIPULATOR&  manipulator,
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
bsl::string& BigRecord::name()
{
    return d_name;
}

inline
bsl::vector<BasicRecord>& BigRecord::array()
{
    return d_array;
}

// ACCESSORS
template <class ACCESSOR>
inline
int BigRecord::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_array, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ARRAY]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
inline
int BigRecord::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_NAME: {
        return accessor(d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ARRAY: {
        return accessor(d_array, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ARRAY]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

template <class ACCESSOR>
inline
int BigRecord::accessAttribute(
        ACCESSOR&   accessor,
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
const bsl::string& BigRecord::name() const
{
    return d_name;
}

inline
const bsl::vector<BasicRecord>& BigRecord::array() const
{
    return d_array;
}

                    // -----------------------------------
                    // class MySequenceWithAnonymousChoice
                    // -----------------------------------

// CREATORS
inline
MySequenceWithAnonymousChoice::MySequenceWithAnonymousChoice(
                                              bslma::Allocator *basicAllocator)
: d_attribute1()
, d_choice(basicAllocator)
, d_attribute2(basicAllocator)
{
}

inline
MySequenceWithAnonymousChoice::MySequenceWithAnonymousChoice(
        const MySequenceWithAnonymousChoice& original,
        bslma::Allocator *basicAllocator)
: d_attribute1(original.d_attribute1)
, d_choice(original.d_choice, basicAllocator)
, d_attribute2(original.d_attribute2, basicAllocator)
{
}

inline
MySequenceWithAnonymousChoice::~MySequenceWithAnonymousChoice()
{
}

// MANIPULATORS
inline
MySequenceWithAnonymousChoice&
MySequenceWithAnonymousChoice::operator=(
                                      const MySequenceWithAnonymousChoice& rhs)
{
    if (this != &rhs) {
        d_attribute1 = rhs.d_attribute1;
        d_choice = rhs.d_choice;
        d_attribute2 = rhs.d_attribute2;
    }
    return *this;
}

inline
void MySequenceWithAnonymousChoice::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_attribute1);
    bdlat_ValueTypeFunctions::reset(&d_choice);
    bdlat_ValueTypeFunctions::reset(&d_attribute2);
}

template <class MANIPULATOR>
inline
int
MySequenceWithAnonymousChoice::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute1,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_attribute2,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
inline
int
MySequenceWithAnonymousChoice::manipulateAttribute(MANIPULATOR& manipulator,
                                                   int          id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return
  manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_CHOICE: {
        return
          manipulator(&d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return
  manipulator(&d_attribute2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

template <class MANIPULATOR>
inline
int MySequenceWithAnonymousChoice::manipulateAttribute(
        MANIPULATOR&  manipulator,
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
int& MySequenceWithAnonymousChoice::attribute1()
{
    return d_attribute1;
}

inline
MySequenceWithAnonymousChoiceChoice& MySequenceWithAnonymousChoice::choice()
{
    return d_choice;
}

inline
bsl::string& MySequenceWithAnonymousChoice::attribute2()
{
    return d_attribute2;
}

// ACCESSORS
template <class ACCESSOR>
inline
int MySequenceWithAnonymousChoice::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret =
      accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret =
      accessor(d_attribute2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
inline
int MySequenceWithAnonymousChoice::accessAttribute(ACCESSOR& accessor,
                                                   int       id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return
      accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_CHOICE: {
        return
              accessor(d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return
      accessor(d_attribute2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

template <class ACCESSOR>
inline
int MySequenceWithAnonymousChoice::accessAttribute(
        ACCESSOR&   accessor,
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
const int& MySequenceWithAnonymousChoice::attribute1() const
{
    return d_attribute1;
}

inline
const MySequenceWithAnonymousChoiceChoice&
MySequenceWithAnonymousChoice::choice() const
{
    return d_choice;
}

inline
const bsl::string& MySequenceWithAnonymousChoice::attribute2() const
{
    return d_attribute2;
}

                            // -------------------
                            // class TimingRequest
                            // -------------------

// CREATORS
inline
TimingRequest::TimingRequest(bslma::Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
TimingRequest::TimingRequest(
    const TimingRequest& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_SQRT: {
        new (d_sqrt.buffer())
            Sqrt(original.d_sqrt.object());
      } break;
      case SELECTION_ID_BASIC: {
        new (d_basic.buffer())
            BasicRecord(
                original.d_basic.object(), d_allocator_p);
      } break;
      case SELECTION_ID_BIG: {
        new (d_big.buffer())
            BigRecord(
                original.d_big.object(), d_allocator_p);
      } break;
      default:
        BSLS_ASSERT_SAFE(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

inline
TimingRequest::~TimingRequest()
{
    reset();
}

// MANIPULATORS
inline
TimingRequest&
TimingRequest::operator=(const TimingRequest& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_SQRT: {
            makeSqrt(rhs.d_sqrt.object());
          } break;
          case SELECTION_ID_BASIC: {
            makeBasic(rhs.d_basic.object());
          } break;
          case SELECTION_ID_BIG: {
            makeBig(rhs.d_big.object());
          } break;
          default:
            BSLS_ASSERT_SAFE(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }
    return *this;
}

inline
void TimingRequest::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_SQRT: {
        d_sqrt.object().~Sqrt();
      } break;
      case SELECTION_ID_BASIC: {
        d_basic.object().~BasicRecord();
      } break;
      case SELECTION_ID_BIG: {
        d_big.object().~BigRecord();
      } break;
      default:
        BSLS_ASSERT_SAFE(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

inline
int TimingRequest::makeSelection(int selectionId)
{
    enum { NOT_FOUND = -1, SUCCESS = 0 };

    switch (selectionId) {
      case SELECTION_ID_SQRT: {
        makeSqrt();
      } break;
      case SELECTION_ID_BASIC: {
        makeBasic();
      } break;
      case SELECTION_ID_BIG: {
        makeBig();
      } break;
      case SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
    return SUCCESS;
}

inline
int TimingRequest::makeSelection(const char *name, int nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdlat_SelectionInfo *selectionInfo =
           lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return NOT_FOUND;                                              // RETURN
    }

    return makeSelection(selectionInfo->d_id);
}

inline
Sqrt& TimingRequest::makeSqrt()
{
    if (SELECTION_ID_SQRT == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_sqrt.object());
    }
    else {
        reset();
        new (d_sqrt.buffer())
            Sqrt();

        d_selectionId = SELECTION_ID_SQRT;
    }

    return d_sqrt.object();
}

inline
Sqrt& TimingRequest::makeSqrt(const Sqrt& value)
{
    if (SELECTION_ID_SQRT == d_selectionId) {
        d_sqrt.object() = value;
    }
    else {
        reset();
        new (d_sqrt.buffer())
                Sqrt(value);
        d_selectionId = SELECTION_ID_SQRT;
    }

    return d_sqrt.object();
}

inline
BasicRecord& TimingRequest::makeBasic()
{
    if (SELECTION_ID_BASIC == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_basic.object());
    }
    else {
        reset();
        new (d_basic.buffer())
                BasicRecord(d_allocator_p);

        d_selectionId = SELECTION_ID_BASIC;
    }

    return d_basic.object();
}

inline
BasicRecord& TimingRequest::makeBasic(const BasicRecord& value)
{
    if (SELECTION_ID_BASIC == d_selectionId) {
        d_basic.object() = value;
    }
    else {
        reset();
        new (d_basic.buffer())
                BasicRecord(value, d_allocator_p);
        d_selectionId = SELECTION_ID_BASIC;
    }

    return d_basic.object();
}

inline
BigRecord& TimingRequest::makeBig()
{
    if (SELECTION_ID_BIG == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_big.object());
    }
    else {
        reset();
        new (d_big.buffer())
                BigRecord(d_allocator_p);

        d_selectionId = SELECTION_ID_BIG;
    }

    return d_big.object();
}

inline
BigRecord& TimingRequest::makeBig(const BigRecord& value)
{
    if (SELECTION_ID_BIG == d_selectionId) {
        d_big.object() = value;
    }
    else {
        reset();
        new (d_big.buffer())
                BigRecord(value, d_allocator_p);
        d_selectionId = SELECTION_ID_BIG;
    }

    return d_big.object();
}

template <class MANIPULATOR>
inline
int TimingRequest::manipulateSelection(MANIPULATOR& manipulator)
{
    enum { FAILURE = -1, SUCCESS = 0 };

    switch (d_selectionId) {
      case TimingRequest::SELECTION_ID_SQRT:
        return manipulator(&d_sqrt.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SQRT]);          // RETURN
      case TimingRequest::SELECTION_ID_BASIC:
        return manipulator(&d_basic.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_BASIC]);         // RETURN
      case TimingRequest::SELECTION_ID_BIG:
        return manipulator(&d_big.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_BIG]);           // RETURN
      default:
        BSLS_ASSERT_SAFE(TimingRequest::SELECTION_ID_UNDEFINED ==
                     d_selectionId);
        return FAILURE;                                               // RETURN
    }
}

inline
Sqrt& TimingRequest::sqrt()
{
    BSLS_ASSERT_SAFE(SELECTION_ID_SQRT == d_selectionId);
    return d_sqrt.object();
}

inline
BasicRecord& TimingRequest::basic()
{
    BSLS_ASSERT_SAFE(SELECTION_ID_BASIC == d_selectionId);
    return d_basic.object();
}

inline
BigRecord& TimingRequest::big()
{
    BSLS_ASSERT_SAFE(SELECTION_ID_BIG == d_selectionId);
    return d_big.object();
}

// ACCESSORS
inline
int TimingRequest::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
inline
int TimingRequest::accessSelection(ACCESSOR& accessor) const
{
    enum { FAILURE = -1, SUCCESS = 0 };

    switch (d_selectionId) {
      case SELECTION_ID_SQRT:
        return accessor(d_sqrt.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SQRT]);          // RETURN
      case SELECTION_ID_BASIC:
        return accessor(d_basic.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_BASIC]);         // RETURN
      case SELECTION_ID_BIG:
        return accessor(d_big.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_BIG]);           // RETURN
      default:
        BSLS_ASSERT_SAFE(SELECTION_ID_UNDEFINED == d_selectionId);
        return FAILURE;                                               // RETURN
    }
}

inline
const Sqrt& TimingRequest::sqrt() const
{
    BSLS_ASSERT_SAFE(SELECTION_ID_SQRT == d_selectionId);
    return d_sqrt.object();
}

inline
const BasicRecord& TimingRequest::basic() const
{
    BSLS_ASSERT_SAFE(SELECTION_ID_BASIC == d_selectionId);
    return d_basic.object();
}

inline
const BigRecord& TimingRequest::big() const
{
    BSLS_ASSERT_SAFE(SELECTION_ID_BIG == d_selectionId);
    return d_big.object();
}

inline
bool TimingRequest::isSqrtValue() const
{
    return SELECTION_ID_SQRT == d_selectionId;
}

inline
bool TimingRequest::isBasicValue() const
{
    return SELECTION_ID_BASIC == d_selectionId;
}

inline
bool TimingRequest::isBigValue() const
{
    return SELECTION_ID_BIG == d_selectionId;
}

inline
bool TimingRequest::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}
}  // close namespace test

// FREE FUNCTIONS

inline
bool test::operator==(
        const test::MyChoice& lhs,
        const test::MyChoice& rhs)
{
    typedef test::MyChoice Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::SELECTION_ID_SELECTION1:
            return lhs.selection1() == rhs.selection1();
                                                                    // RETURN
          case Class::SELECTION_ID_SELECTION2:
            return lhs.selection2() == rhs.selection2();
                                                                    // RETURN
          default:
            BSLS_ASSERT_SAFE(Class::SELECTION_ID_UNDEFINED
                            == rhs.selectionId());
            return true;                                              // RETURN
        }
    }
    else {
        return false;                                                 // RETURN
   }
}

inline
bool test::operator!=(
        const test::MyChoice& lhs,
        const test::MyChoice& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& test::operator<<(
        bsl::ostream& stream,
        const test::MyChoice& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline
bool test::operator==(
        const test::MySequenceWithNullable& lhs,
        const test::MySequenceWithNullable& rhs)
{
    return  lhs.attribute1() == rhs.attribute1()
         && lhs.attribute2() == rhs.attribute2();
}

inline
bool test::operator!=(
        const test::MySequenceWithNullable& lhs,
        const test::MySequenceWithNullable& rhs)
{
    return  lhs.attribute1() != rhs.attribute1()
         || lhs.attribute2() != rhs.attribute2();
}

inline
bsl::ostream& test::operator<<(
        bsl::ostream& stream,
        const test::MySequenceWithNullable& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline
bool test::operator==(
        const test::Address& lhs,
        const test::Address& rhs)
{
    return  lhs.street() == rhs.street()
         && lhs.city() == rhs.city()
         && lhs.state() == rhs.state();
}

inline
bool test::operator!=(
        const test::Address& lhs,
        const test::Address& rhs)
{
    return  lhs.street() != rhs.street()
         || lhs.city() != rhs.city()
         || lhs.state() != rhs.state();
}

inline
bsl::ostream& test::operator<<(
        bsl::ostream& stream,
        const test::Address& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline
bool test::operator==(
        const test::MySequence& lhs,
        const test::MySequence& rhs)
{
    return  lhs.attribute1() == rhs.attribute1()
         && lhs.attribute2() == rhs.attribute2();
}

inline
bool test::operator!=(
        const test::MySequence& lhs,
        const test::MySequence& rhs)
{
    return  lhs.attribute1() != rhs.attribute1()
         || lhs.attribute2() != rhs.attribute2();
}

inline
bsl::ostream& test::operator<<(
        bsl::ostream& stream,
        const test::MySequence& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline
bool test::operator==(
        const test::MySequenceWithNillable& lhs,
        const test::MySequenceWithNillable& rhs)
{
    return  lhs.attribute1() == rhs.attribute1()
         && lhs.myNillable() == rhs.myNillable()
         && lhs.attribute2() == rhs.attribute2();
}

inline
bool test::operator!=(
        const test::MySequenceWithNillable& lhs,
        const test::MySequenceWithNillable& rhs)
{
    return  lhs.attribute1() != rhs.attribute1()
         || lhs.myNillable() != rhs.myNillable()
         || lhs.attribute2() != rhs.attribute2();
}

inline
bsl::ostream& test::operator<<(
        bsl::ostream& stream,
        const test::MySequenceWithNillable& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline
bsl::ostream& test::operator<<(
        bsl::ostream& stream,
        test::MyEnumeration::Value rhs)
{
    return test::MyEnumeration::print(stream, rhs);
}

inline
bool test::operator==(
        const test::Sqrt& lhs,
        const test::Sqrt& rhs)
{
    return  lhs.value() == rhs.value();
}

inline
bool test::operator!=(
        const test::Sqrt& lhs,
        const test::Sqrt& rhs)
{
    return  lhs.value() != rhs.value();
}

inline
bsl::ostream& test::operator<<(
        bsl::ostream& stream,
        const test::Sqrt& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline
bool test::operator==(
        const test::CustomizedString& lhs,
        const test::CustomizedString& rhs)
{
    return lhs.d_value == rhs.d_value;
}

inline
bool test::operator!=(
        const test::CustomizedString& lhs,
        const test::CustomizedString& rhs)
{
    return lhs.d_value != rhs.d_value;
}

inline
bsl::ostream& test::operator<<(
        bsl::ostream& stream,
        const test::CustomizedString& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline
bool test::operator==(
        const test::BasicRecord& lhs,
        const test::BasicRecord& rhs)
{
    return  lhs.i1() == rhs.i1()
         && lhs.i2() == rhs.i2()
         && lhs.dt() == rhs.dt()
         && lhs.s() == rhs.s();
}

inline
bool test::operator!=(
        const test::BasicRecord& lhs,
        const test::BasicRecord& rhs)
{
    return  lhs.i1() != rhs.i1()
         || lhs.i2() != rhs.i2()
         || lhs.dt() != rhs.dt()
         || lhs.s() != rhs.s();
}

inline
bsl::ostream& test::operator<<(
        bsl::ostream& stream,
        const test::BasicRecord& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline
bool test::operator==(
        const test::BasicRecordWithVariant& lhs,
        const test::BasicRecordWithVariant& rhs)
{
    return  lhs.i1() == rhs.i1()
         && lhs.i2() == rhs.i2()
         && lhs.dt() == rhs.dt()
         && lhs.s() == rhs.s();
}

inline
bool test::operator!=(
        const test::BasicRecordWithVariant& lhs,
        const test::BasicRecordWithVariant& rhs)
{
    return  lhs.i1() != rhs.i1()
         || lhs.i2() != rhs.i2()
         || lhs.dt() != rhs.dt()
         || lhs.s() != rhs.s();
}

inline
bsl::ostream& test::operator<<(
        bsl::ostream& stream,
        const test::BasicRecordWithVariant& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline
bool test::operator==(
        const test::MySequenceWithArray& lhs,
        const test::MySequenceWithArray& rhs)
{
    return  lhs.attribute1() == rhs.attribute1()
         && lhs.attribute2() == rhs.attribute2();
}

inline
bool test::operator!=(
        const test::MySequenceWithArray& lhs,
        const test::MySequenceWithArray& rhs)
{
    return  lhs.attribute1() != rhs.attribute1()
         || lhs.attribute2() != rhs.attribute2();
}

inline
bsl::ostream& test::operator<<(
        bsl::ostream& stream,
        const test::MySequenceWithArray& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline
bool test::operator==(
        const test::MySequenceWithAnonymousChoiceChoice& lhs,
        const test::MySequenceWithAnonymousChoiceChoice& rhs)
{
    typedef test::MySequenceWithAnonymousChoiceChoice Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::SELECTION_ID_MY_CHOICE1:
            return lhs.myChoice1() == rhs.myChoice1();
                                                                    // RETURN
          case Class::SELECTION_ID_MY_CHOICE2:
            return lhs.myChoice2() == rhs.myChoice2();
                                                                    // RETURN
          default:
            BSLS_ASSERT_SAFE(Class::SELECTION_ID_UNDEFINED
                            == rhs.selectionId());
            return true;                                              // RETURN
        }
    }
    else {
        return false;                                                 // RETURN
   }
}

inline
bool test::operator!=(
        const test::MySequenceWithAnonymousChoiceChoice& lhs,
        const test::MySequenceWithAnonymousChoiceChoice& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& test::operator<<(
        bsl::ostream& stream,
        const test::MySequenceWithAnonymousChoiceChoice& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline
bool test::operator==(
        const test::Employee& lhs,
        const test::Employee& rhs)
{
    return  lhs.name() == rhs.name()
         && lhs.homeAddress() == rhs.homeAddress()
         && lhs.age() == rhs.age();
}

inline
bool test::operator!=(
        const test::Employee& lhs,
        const test::Employee& rhs)
{
    return  lhs.name() != rhs.name()
         || lhs.homeAddress() != rhs.homeAddress()
         || lhs.age() != rhs.age();
}

inline
bsl::ostream& test::operator<<(
        bsl::ostream& stream,
        const test::Employee& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline
bool test::operator==(
        const test::BigRecord& lhs,
        const test::BigRecord& rhs)
{
    return  lhs.name() == rhs.name()
         && lhs.array() == rhs.array();
}

inline
bool test::operator!=(
        const test::BigRecord& lhs,
        const test::BigRecord& rhs)
{
    return  lhs.name() != rhs.name()
         || lhs.array() != rhs.array();
}

inline
bsl::ostream& test::operator<<(
        bsl::ostream& stream,
        const test::BigRecord& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline
bool test::operator==(
        const test::MySequenceWithAnonymousChoice& lhs,
        const test::MySequenceWithAnonymousChoice& rhs)
{
    return  lhs.attribute1() == rhs.attribute1()
         && lhs.choice() == rhs.choice()
         && lhs.attribute2() == rhs.attribute2();
}

inline
bool test::operator!=(
        const test::MySequenceWithAnonymousChoice& lhs,
        const test::MySequenceWithAnonymousChoice& rhs)
{
    return  lhs.attribute1() != rhs.attribute1()
         || lhs.choice() != rhs.choice()
         || lhs.attribute2() != rhs.attribute2();
}

inline
bsl::ostream& test::operator<<(
        bsl::ostream& stream,
        const test::MySequenceWithAnonymousChoice& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline
bool test::operator==(
        const test::TimingRequest& lhs,
        const test::TimingRequest& rhs)
{
    typedef test::TimingRequest Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::SELECTION_ID_SQRT:
            return lhs.sqrt() == rhs.sqrt();
                                                                    // RETURN
          case Class::SELECTION_ID_BASIC:
            return lhs.basic() == rhs.basic();
                                                                    // RETURN
          case Class::SELECTION_ID_BIG:
            return lhs.big() == rhs.big();
                                                                    // RETURN
          default:
            BSLS_ASSERT_SAFE(Class::SELECTION_ID_UNDEFINED
                            == rhs.selectionId());
            return true;                                              // RETURN
        }
    }
    else {
        return false;                                                 // RETURN
   }
}

inline
bool test::operator!=(
        const test::TimingRequest& lhs,
        const test::TimingRequest& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& test::operator<<(
        bsl::ostream& stream,
        const test::TimingRequest& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace
#endif

// GENERATED BY BLP_BAS_CODEGEN_2.1.8
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------

// test_messages.cpp   -*-C++-*-

// #include <test_messages.h>
#include <bdlt_datetimetz.h>
#include <bdlb_nullablevalue.h>
#include <bsl_string.h>
#include <bsl_vector.h>

#include <bdlat_formattingmode.h>

#include <bsls_assert.h>
#include <bdlb_chartype.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>
#include <bdlb_string.h>

#include <bsl_ostream.h>
#include <bsl_iomanip.h>

namespace BloombergLP {
namespace test {

                               // --------------
                               // class MyChoice
                               // --------------

// CONSTANTS

const char MyChoice::CLASS_NAME[] = "MyChoice";
    // the name of this class

const bdlat_SelectionInfo MyChoice::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_SELECTION1,
        "selection1",               // name
        sizeof("selection1") - 1,   // name length
        "",  // annotation
        bdlat_FormattingMode::e_DEC // formatting mode
    },
    {
        SELECTION_ID_SELECTION2,
        "selection2",               // name
        sizeof("selection2") - 1,   // name length
        "",  // annotation
        bdlat_FormattingMode::e_TEXT // formatting mode
    }
};

// CLASS METHODS

const bdlat_SelectionInfo *MyChoice::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 10: {
            if (bdlb::CharType::toUpper(name[0])=='S'
             && bdlb::CharType::toUpper(name[1])=='E'
             && bdlb::CharType::toUpper(name[2])=='L'
             && bdlb::CharType::toUpper(name[3])=='E'
             && bdlb::CharType::toUpper(name[4])=='C'
             && bdlb::CharType::toUpper(name[5])=='T'
             && bdlb::CharType::toUpper(name[6])=='I'
             && bdlb::CharType::toUpper(name[7])=='O'
             && bdlb::CharType::toUpper(name[8])=='N')
            {
                switch(bdlb::CharType::toUpper(name[9])) {
                    case '1': {
                      return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1];
                                                                      // RETURN
                    } break;
                    case '2': {
                      return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2];
                                                                      // RETURN
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdlat_SelectionInfo *MyChoice::lookupSelectionInfo(int id)
{
    switch (id) {
      case SELECTION_ID_SELECTION1:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1];
      case SELECTION_ID_SELECTION2:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2];
      default:
        return 0;
    }
}

// ACCESSORS

bsl::ostream& MyChoice::print(
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

        switch (d_selectionId) {
          case SELECTION_ID_SELECTION1: {
            stream << "Selection1 = ";
            bdlb::PrintMethods::print(stream, d_selection1.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION2: {
            stream << "Selection2 = ";
            bdlb::PrintMethods::print(stream, d_selection2.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED\n";
        }
        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << "[ ";

        switch (d_selectionId) {
          case SELECTION_ID_SELECTION1: {
            stream << "Selection1 = ";
            bdlb::PrintMethods::print(stream, d_selection1.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION2: {
            stream << "Selection2 = ";
            bdlb::PrintMethods::print(stream, d_selection2.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED";
        }

        stream << " ]";
    }

    return stream << bsl::flush;
}

                        // ----------------------------
                        // class MySequenceWithNullable
                        // ----------------------------

// CONSTANTS

const char MySequenceWithNullable::CLASS_NAME[] = "MySequenceWithNullable";
    // the name of this class

const bdlat_AttributeInfo MySequenceWithNullable::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "attribute1",             // name
        sizeof("attribute1") - 1, // name length
        "",  // annotation
        bdlat_FormattingMode::e_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE2,
        "attribute2",             // name
        sizeof("attribute2") - 1, // name length
        "",  // annotation
        bdlat_FormattingMode::e_TEXT // formatting mode
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *MySequenceWithNullable::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 10: {
            if (bdlb::CharType::toUpper(name[0])=='A'
             && bdlb::CharType::toUpper(name[1])=='T'
             && bdlb::CharType::toUpper(name[2])=='T'
             && bdlb::CharType::toUpper(name[3])=='R'
             && bdlb::CharType::toUpper(name[4])=='I'
             && bdlb::CharType::toUpper(name[5])=='B'
             && bdlb::CharType::toUpper(name[6])=='U'
             && bdlb::CharType::toUpper(name[7])=='T'
             && bdlb::CharType::toUpper(name[8])=='E')
            {
                switch(bdlb::CharType::toUpper(name[9])) {
                    case '1': {
                      return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
                                                                      // RETURN
                    } break;
                    case '2': {
                      return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2];
                                                                      // RETURN
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdlat_AttributeInfo *MySequenceWithNullable::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
      case ATTRIBUTE_ID_ATTRIBUTE2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2];
      default:
        return 0;
    }
}

// ACCESSORS

bsl::ostream& MySequenceWithNullable::print(
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
        stream << "Attribute1 = ";
        bdlb::PrintMethods::print(stream, d_attribute1,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute2 = ";
        bdlb::PrintMethods::print(stream, d_attribute2,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Attribute1 = ";
        bdlb::PrintMethods::print(stream, d_attribute1,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Attribute2 = ";
        bdlb::PrintMethods::print(stream, d_attribute2,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

                               // -------------
                               // class Address
                               // -------------

// CONSTANTS

const char Address::CLASS_NAME[] = "Address";
    // the name of this class

const bdlat_AttributeInfo Address::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_STREET,
        "street",             // name
        sizeof("street") - 1, // name length
        "",  // annotation
        bdlat_FormattingMode::e_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_CITY,
        "city",             // name
        sizeof("city") - 1, // name length
        "",  // annotation
        bdlat_FormattingMode::e_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_STATE,
        "state",             // name
        sizeof("state") - 1, // name length
        "",  // annotation
        bdlat_FormattingMode::e_TEXT // formatting mode
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *Address::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 4: {
            if (bdlb::CharType::toUpper(name[0])=='C'
             && bdlb::CharType::toUpper(name[1])=='I'
             && bdlb::CharType::toUpper(name[2])=='T'
             && bdlb::CharType::toUpper(name[3])=='Y')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CITY];   // RETURN
            }
        } break;
        case 5: {
            if (bdlb::CharType::toUpper(name[0])=='S'
             && bdlb::CharType::toUpper(name[1])=='T'
             && bdlb::CharType::toUpper(name[2])=='A'
             && bdlb::CharType::toUpper(name[3])=='T'
             && bdlb::CharType::toUpper(name[4])=='E')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATE];  // RETURN
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
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STREET]; // RETURN
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

// ACCESSORS

bsl::ostream& Address::print(
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

                              // ----------------
                              // class MySequence
                              // ----------------

// CONSTANTS

const char MySequence::CLASS_NAME[] = "MySequence";
    // the name of this class

const bdlat_AttributeInfo MySequence::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "attribute1",             // name
        sizeof("attribute1") - 1, // name length
        "",  // annotation
        bdlat_FormattingMode::e_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE2,
        "attribute2",             // name
        sizeof("attribute2") - 1, // name length
        "",  // annotation
        bdlat_FormattingMode::e_TEXT // formatting mode
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *MySequence::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 10: {
            if (bdlb::CharType::toUpper(name[0])=='A'
             && bdlb::CharType::toUpper(name[1])=='T'
             && bdlb::CharType::toUpper(name[2])=='T'
             && bdlb::CharType::toUpper(name[3])=='R'
             && bdlb::CharType::toUpper(name[4])=='I'
             && bdlb::CharType::toUpper(name[5])=='B'
             && bdlb::CharType::toUpper(name[6])=='U'
             && bdlb::CharType::toUpper(name[7])=='T'
             && bdlb::CharType::toUpper(name[8])=='E')
            {
                switch(bdlb::CharType::toUpper(name[9])) {
                    case '1': {
                      return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
                                                                      // RETURN
                    } break;
                    case '2': {
                      return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2];
                                                                      // RETURN
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdlat_AttributeInfo *MySequence::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
      case ATTRIBUTE_ID_ATTRIBUTE2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2];
      default:
        return 0;
    }
}

// ACCESSORS

bsl::ostream& MySequence::print(
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
        stream << "Attribute1 = ";
        bdlb::PrintMethods::print(stream, d_attribute1,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute2 = ";
        bdlb::PrintMethods::print(stream, d_attribute2,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Attribute1 = ";
        bdlb::PrintMethods::print(stream, d_attribute1,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Attribute2 = ";
        bdlb::PrintMethods::print(stream, d_attribute2,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

                        // ----------------------------
                        // class MySequenceWithNillable
                        // ----------------------------

// CONSTANTS

const char MySequenceWithNillable::CLASS_NAME[] = "MySequenceWithNillable";
    // the name of this class

const bdlat_AttributeInfo MySequenceWithNillable::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "attribute1",             // name
        sizeof("attribute1") - 1, // name length
        "",  // annotation
        bdlat_FormattingMode::e_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_MY_NILLABLE,
        "myNillable",             // name
        sizeof("myNillable") - 1, // name length
        "",  // annotation
        bdlat_FormattingMode::e_TEXT // formatting mode
      | bdlat_FormattingMode::e_NILLABLE
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE2,
        "attribute2",             // name
        sizeof("attribute2") - 1, // name length
        "",  // annotation
        bdlat_FormattingMode::e_TEXT // formatting mode
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *MySequenceWithNillable::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 10: {
            switch(bdlb::CharType::toUpper(name[0])) {
                case 'A': {
                    if (bdlb::CharType::toUpper(name[1])=='T'
                     && bdlb::CharType::toUpper(name[2])=='T'
                     && bdlb::CharType::toUpper(name[3])=='R'
                     && bdlb::CharType::toUpper(name[4])=='I'
                     && bdlb::CharType::toUpper(name[5])=='B'
                     && bdlb::CharType::toUpper(name[6])=='U'
                     && bdlb::CharType::toUpper(name[7])=='T'
                     && bdlb::CharType::toUpper(name[8])=='E')
                    {
                        switch(bdlb::CharType::toUpper(name[9])) {
                          case '1': {
                            return
                             &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
                                                                      // RETURN
                          } break;
                          case '2': {
                            return
                             &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2];
                                                                      // RETURN
                          } break;
                        }
                    }
                } break;
                case 'M': {
                    if (bdlb::CharType::toUpper(name[1])=='Y'
                     && bdlb::CharType::toUpper(name[2])=='N'
                     && bdlb::CharType::toUpper(name[3])=='I'
                     && bdlb::CharType::toUpper(name[4])=='L'
                     && bdlb::CharType::toUpper(name[5])=='L'
                     && bdlb::CharType::toUpper(name[6])=='A'
                     && bdlb::CharType::toUpper(name[7])=='B'
                     && bdlb::CharType::toUpper(name[8])=='L'
                     && bdlb::CharType::toUpper(name[9])=='E')
                    {
                        return
                            &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MY_NILLABLE];
                                                                      // RETURN
                    }
                } break;
            }
        } break;
    }
    return 0;
}

const bdlat_AttributeInfo *MySequenceWithNillable::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
      case ATTRIBUTE_ID_MY_NILLABLE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MY_NILLABLE];
      case ATTRIBUTE_ID_ATTRIBUTE2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2];
      default:
        return 0;
    }
}

// ACCESSORS

bsl::ostream& MySequenceWithNillable::print(
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
        stream << "Attribute1 = ";
        bdlb::PrintMethods::print(stream, d_attribute1,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "MyNillable = ";
        bdlb::PrintMethods::print(stream, d_myNillable,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute2 = ";
        bdlb::PrintMethods::print(stream, d_attribute2,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Attribute1 = ";
        bdlb::PrintMethods::print(stream, d_attribute1,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "MyNillable = ";
        bdlb::PrintMethods::print(stream, d_myNillable,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Attribute2 = ";
        bdlb::PrintMethods::print(stream, d_attribute2,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

                            // -------------------
                            // class MyEnumeration
                            // -------------------

// CONSTANTS

const char MyEnumeration::CLASS_NAME[] = "MyEnumeration";
    // the name of this class

const bdlat_EnumeratorInfo MyEnumeration::ENUMERATOR_INFO_ARRAY[] = {
    {
        MyEnumeration::VALUE1,
        "VALUE1",                 // name
        sizeof("VALUE1") - 1,     // name length
        ""  // annotation
    },
    {
        MyEnumeration::VALUE2,
        "VALUE2",                 // name
        sizeof("VALUE2") - 1,     // name length
        ""  // annotation
    }
};

// CLASS METHODS

int MyEnumeration::fromString(MyEnumeration::Value *result,
                            const char         *string,
                            int                 stringLength)
{

    enum { SUCCESS = 0, NOT_FOUND = 1 };

    switch(stringLength) {
        case 6: {
            if (bdlb::CharType::toUpper(string[0])=='V'
             && bdlb::CharType::toUpper(string[1])=='A'
             && bdlb::CharType::toUpper(string[2])=='L'
             && bdlb::CharType::toUpper(string[3])=='U'
             && bdlb::CharType::toUpper(string[4])=='E') {
                switch(bdlb::CharType::toUpper(string[5])) {
                    case '1': {
                        *result = MyEnumeration::VALUE1;
                        return SUCCESS;                               // RETURN
                    } break;
                    case '2': {
                        *result = MyEnumeration::VALUE2;
                        return SUCCESS;                               // RETURN
                    } break;
                }
            }
        } break;
    }

    return NOT_FOUND;

}

                                 // ----------
                                 // class Sqrt
                                 // ----------

// CONSTANTS

const char Sqrt::CLASS_NAME[] = "Sqrt";
    // the name of this class

const bdlat_AttributeInfo Sqrt::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_VALUE,
        "value",             // name
        sizeof("value") - 1, // name length
        "",  // annotation
        bdlat_FormattingMode::e_DEFAULT // formatting mode
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *Sqrt::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 5: {
            if (bdlb::CharType::toUpper(name[0])=='V'
             && bdlb::CharType::toUpper(name[1])=='A'
             && bdlb::CharType::toUpper(name[2])=='L'
             && bdlb::CharType::toUpper(name[3])=='U'
             && bdlb::CharType::toUpper(name[4])=='E')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALUE];  // RETURN
            }
        } break;
    }
    return 0;
}

const bdlat_AttributeInfo *Sqrt::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_VALUE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALUE];
      default:
        return 0;
    }
}

// ACCESSORS

bsl::ostream& Sqrt::print(
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
        stream << "Value = ";
        bdlb::PrintMethods::print(stream, d_value,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Value = ";
        bdlb::PrintMethods::print(stream, d_value,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

                           // ----------------------
                           // class CustomizedString
                           // ----------------------

// CONSTANTS

const char CustomizedString::CLASS_NAME[] = "CustomizedString";
    // the name of this class

                             // -----------------
                             // class BasicRecord
                             // -----------------

// CONSTANTS

const char BasicRecord::CLASS_NAME[] = "BasicRecord";
    // the name of this class

const bdlat_AttributeInfo BasicRecord::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_I1,
        "i1",             // name
        sizeof("i1") - 1, // name length
        "",  // annotation
        bdlat_FormattingMode::e_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_I2,
        "i2",             // name
        sizeof("i2") - 1, // name length
        "",  // annotation
        bdlat_FormattingMode::e_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_DT,
        "dt",             // name
        sizeof("dt") - 1, // name length
        "",  // annotation
        bdlat_FormattingMode::e_DEFAULT // formatting mode
    },
    {
        ATTRIBUTE_ID_S,
        "s",             // name
        sizeof("s") - 1, // name length
        "",  // annotation
        bdlat_FormattingMode::e_TEXT // formatting mode
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *BasicRecord::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 1: {
            if (bdlb::CharType::toUpper(name[0])=='S')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_S];      // RETURN
            }
        } break;
        case 2: {
            switch(bdlb::CharType::toUpper(name[0])) {
                case 'D': {
                    if (bdlb::CharType::toUpper(name[1])=='T')
                    {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DT];
                                                                      // RETURN
                    }
                } break;
                case 'I': {
                    switch(bdlb::CharType::toUpper(name[1])) {
                        case '1': {
                            return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_I1];
                                                                      // RETURN
                        } break;
                        case '2': {
                            return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_I2];
                                                                      // RETURN
                        } break;
                    }
                } break;
            }
        } break;
    }
    return 0;
}

const bdlat_AttributeInfo *BasicRecord::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_I1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_I1];
      case ATTRIBUTE_ID_I2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_I2];
      case ATTRIBUTE_ID_DT:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DT];
      case ATTRIBUTE_ID_S:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_S];
      default:
        return 0;
    }
}

// ACCESSORS

bsl::ostream& BasicRecord::print(
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
        stream << "I1 = ";
        bdlb::PrintMethods::print(stream, d_i1,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "I2 = ";
        bdlb::PrintMethods::print(stream, d_i2,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Dt = ";
        bdlb::PrintMethods::print(stream, d_dt,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "S = ";
        bdlb::PrintMethods::print(stream, d_s,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "I1 = ";
        bdlb::PrintMethods::print(stream, d_i1,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "I2 = ";
        bdlb::PrintMethods::print(stream, d_i2,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Dt = ";
        bdlb::PrintMethods::print(stream, d_dt,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "S = ";
        bdlb::PrintMethods::print(stream, d_s,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

                        // ----------------------------
                        // class BasicRecordWithVariant
                        // ----------------------------

// CONSTANTS

const char BasicRecordWithVariant::CLASS_NAME[] = "BasicRecordWithVariant";
    // the name of this class

const bdlat_AttributeInfo BasicRecordWithVariant::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_I1,
        "i1",             // name
        sizeof("i1") - 1, // name length
        "",  // annotation
        bdlat_FormattingMode::e_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_I2,
        "i2",             // name
        sizeof("i2") - 1, // name length
        "",  // annotation
        bdlat_FormattingMode::e_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_DT,
        "dt",             // name
        sizeof("dt") - 1, // name length
        "",  // annotation
        bdlat_FormattingMode::e_DEFAULT // formatting mode
    },
    {
        ATTRIBUTE_ID_S,
        "s",             // name
        sizeof("s") - 1, // name length
        "",  // annotation
        bdlat_FormattingMode::e_TEXT // formatting mode
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *BasicRecordWithVariant::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 1: {
            if (bdlb::CharType::toUpper(name[0])=='S')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_S];      // RETURN
            }
        } break;
        case 2: {
            switch(bdlb::CharType::toUpper(name[0])) {
                case 'D': {
                    if (bdlb::CharType::toUpper(name[1])=='T')
                    {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DT];
                                                                      // RETURN
                    }
                } break;
                case 'I': {
                    switch(bdlb::CharType::toUpper(name[1])) {
                        case '1': {
                            return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_I1];
                                                                      // RETURN
                        } break;
                        case '2': {
                            return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_I2];
                                                                      // RETURN
                        } break;
                    }
                } break;
            }
        } break;
    }
    return 0;
}

const bdlat_AttributeInfo *BasicRecordWithVariant::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_I1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_I1];
      case ATTRIBUTE_ID_I2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_I2];
      case ATTRIBUTE_ID_DT:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DT];
      case ATTRIBUTE_ID_S:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_S];
      default:
        return 0;
    }
}

// ACCESSORS

bsl::ostream& BasicRecordWithVariant::print(
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
        stream << "I1 = ";
        bdlb::PrintMethods::print(stream, d_i1,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "I2 = ";
        bdlb::PrintMethods::print(stream, d_i2,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Dt = ";
        if (d_dt.is<bdlt::Datetime>()) {
            bdlb::PrintMethods::print(stream, d_dt.the<bdlt::Datetime>(),
                                     -levelPlus1, spacesPerLevel);
        }
        else {
            bdlb::PrintMethods::print(stream, d_dt.the<bdlt::DatetimeTz>(),
                                     -levelPlus1, spacesPerLevel);
        }

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "S = ";
        bdlb::PrintMethods::print(stream, d_s,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "I1 = ";
        bdlb::PrintMethods::print(stream, d_i1,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "I2 = ";
        bdlb::PrintMethods::print(stream, d_i2,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Dt = ";
        if (d_dt.is<bdlt::Datetime>()) {
            bdlb::PrintMethods::print(stream, d_dt.the<bdlt::Datetime>(),
                                     -levelPlus1, spacesPerLevel);
        }
        else {
            bdlb::PrintMethods::print(stream, d_dt.the<bdlt::DatetimeTz>(),
                                     -levelPlus1, spacesPerLevel);
        }

        stream << ' ';
        stream << "S = ";
        bdlb::PrintMethods::print(stream, d_s,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

                         // -------------------------
                         // class MySequenceWithArray
                         // -------------------------

// CONSTANTS

const char MySequenceWithArray::CLASS_NAME[] = "MySequenceWithArray";
    // the name of this class

const bdlat_AttributeInfo MySequenceWithArray::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "attribute1",             // name
        sizeof("attribute1") - 1, // name length
        "",  // annotation
        bdlat_FormattingMode::e_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE2,
        "attribute2",             // name
        sizeof("attribute2") - 1, // name length
        "",  // annotation
        bdlat_FormattingMode::e_TEXT // formatting mode
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *MySequenceWithArray::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 10: {
            if (bdlb::CharType::toUpper(name[0])=='A'
             && bdlb::CharType::toUpper(name[1])=='T'
             && bdlb::CharType::toUpper(name[2])=='T'
             && bdlb::CharType::toUpper(name[3])=='R'
             && bdlb::CharType::toUpper(name[4])=='I'
             && bdlb::CharType::toUpper(name[5])=='B'
             && bdlb::CharType::toUpper(name[6])=='U'
             && bdlb::CharType::toUpper(name[7])=='T'
             && bdlb::CharType::toUpper(name[8])=='E')
            {
                switch(bdlb::CharType::toUpper(name[9])) {
                    case '1': {
                      return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
                                                                      // RETURN
                    } break;
                    case '2': {
                      return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2];
                                                                      // RETURN
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdlat_AttributeInfo *MySequenceWithArray::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
      case ATTRIBUTE_ID_ATTRIBUTE2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2];
      default:
        return 0;
    }
}

// ACCESSORS

bsl::ostream& MySequenceWithArray::print(
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
        stream << "Attribute1 = ";
        bdlb::PrintMethods::print(stream, d_attribute1,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute2 = ";
        bdlb::PrintMethods::print(stream, d_attribute2,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Attribute1 = ";
        bdlb::PrintMethods::print(stream, d_attribute1,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Attribute2 = ";
        bdlb::PrintMethods::print(stream, d_attribute2,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

                 // -----------------------------------------
                 // class MySequenceWithAnonymousChoiceChoice
                 // -----------------------------------------

// CONSTANTS

const char MySequenceWithAnonymousChoiceChoice::CLASS_NAME[] =
                                         "MySequenceWithAnonymousChoiceChoice";
    // the name of this class

const bdlat_SelectionInfo
MySequenceWithAnonymousChoiceChoice::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_MY_CHOICE1,
        "myChoice1",               // name
        sizeof("myChoice1") - 1,   // name length
        "",  // annotation
        bdlat_FormattingMode::e_DEC // formatting mode
    },
    {
        SELECTION_ID_MY_CHOICE2,
        "myChoice2",               // name
        sizeof("myChoice2") - 1,   // name length
        "",  // annotation
        bdlat_FormattingMode::e_TEXT // formatting mode
    }
};

// CLASS METHODS

const bdlat_SelectionInfo *
MySequenceWithAnonymousChoiceChoice::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 9: {
            if (bdlb::CharType::toUpper(name[0])=='M'
             && bdlb::CharType::toUpper(name[1])=='Y'
             && bdlb::CharType::toUpper(name[2])=='C'
             && bdlb::CharType::toUpper(name[3])=='H'
             && bdlb::CharType::toUpper(name[4])=='O'
             && bdlb::CharType::toUpper(name[5])=='I'
             && bdlb::CharType::toUpper(name[6])=='C'
             && bdlb::CharType::toUpper(name[7])=='E')
            {
                switch(bdlb::CharType::toUpper(name[8])) {
                    case '1': {
                      return &SELECTION_INFO_ARRAY[SELECTION_INDEX_MY_CHOICE1];
                                                                      // RETURN
                    } break;
                    case '2': {
                      return &SELECTION_INFO_ARRAY[SELECTION_INDEX_MY_CHOICE2];
                                                                      // RETURN
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdlat_SelectionInfo *
MySequenceWithAnonymousChoiceChoice::lookupSelectionInfo(int id)
{
    switch (id) {
      case SELECTION_ID_MY_CHOICE1:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_MY_CHOICE1];
      case SELECTION_ID_MY_CHOICE2:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_MY_CHOICE2];
      default:
        return 0;
    }
}

// ACCESSORS

bsl::ostream& MySequenceWithAnonymousChoiceChoice::print(
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

        switch (d_selectionId) {
          case SELECTION_ID_MY_CHOICE1: {
            stream << "MyChoice1 = ";
            bdlb::PrintMethods::print(stream, d_myChoice1.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_MY_CHOICE2: {
            stream << "MyChoice2 = ";
            bdlb::PrintMethods::print(stream, d_myChoice2.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED\n";
        }
        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << "[ ";

        switch (d_selectionId) {
          case SELECTION_ID_MY_CHOICE1: {
            stream << "MyChoice1 = ";
            bdlb::PrintMethods::print(stream, d_myChoice1.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_MY_CHOICE2: {
            stream << "MyChoice2 = ";
            bdlb::PrintMethods::print(stream, d_myChoice2.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED";
        }

        stream << " ]";
    }

    return stream << bsl::flush;
}

                               // --------------
                               // class Employee
                               // --------------

// CONSTANTS

const char Employee::CLASS_NAME[] = "Employee";
    // the name of this class

const bdlat_AttributeInfo Employee::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_NAME,
        "name",             // name
        sizeof("name") - 1, // name length
        "",  // annotation
        bdlat_FormattingMode::e_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_HOME_ADDRESS,
        "homeAddress",             // name
        sizeof("homeAddress") - 1, // name length
        "",  // annotation
        bdlat_FormattingMode::e_DEFAULT // formatting mode
    },
    {
        ATTRIBUTE_ID_AGE,
        "age",             // name
        sizeof("age") - 1, // name length
        "",  // annotation
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
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE];    // RETURN
            }
        } break;
        case 4: {
            if (bdlb::CharType::toUpper(name[0])=='N'
             && bdlb::CharType::toUpper(name[1])=='A'
             && bdlb::CharType::toUpper(name[2])=='M'
             && bdlb::CharType::toUpper(name[3])=='E')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME];   // RETURN
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
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS];
                                                                      // RETURN
            }
        } break;
    }
    return 0;
}

const bdlat_AttributeInfo *Employee::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_NAME:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME];
      case ATTRIBUTE_ID_HOME_ADDRESS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS];
      case ATTRIBUTE_ID_AGE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE];
      default:
        return 0;
    }
}

// ACCESSORS

bsl::ostream& Employee::print(
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

                              // ---------------
                              // class BigRecord
                              // ---------------

// CONSTANTS

const char BigRecord::CLASS_NAME[] = "BigRecord";
    // the name of this class

const bdlat_AttributeInfo BigRecord::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_NAME,
        "name",             // name
        sizeof("name") - 1, // name length
        "",  // annotation
        bdlat_FormattingMode::e_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_ARRAY,
        "array",             // name
        sizeof("array") - 1, // name length
        "",  // annotation
        bdlat_FormattingMode::e_DEFAULT // formatting mode
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *BigRecord::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 4: {
            if (bdlb::CharType::toUpper(name[0])=='N'
             && bdlb::CharType::toUpper(name[1])=='A'
             && bdlb::CharType::toUpper(name[2])=='M'
             && bdlb::CharType::toUpper(name[3])=='E')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME];   // RETURN
            }
        } break;
        case 5: {
            if (bdlb::CharType::toUpper(name[0])=='A'
             && bdlb::CharType::toUpper(name[1])=='R'
             && bdlb::CharType::toUpper(name[2])=='R'
             && bdlb::CharType::toUpper(name[3])=='A'
             && bdlb::CharType::toUpper(name[4])=='Y')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ARRAY];  // RETURN
            }
        } break;
    }
    return 0;
}

const bdlat_AttributeInfo *BigRecord::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_NAME:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME];
      case ATTRIBUTE_ID_ARRAY:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ARRAY];
      default:
        return 0;
    }
}

// ACCESSORS

bsl::ostream& BigRecord::print(
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
        stream << "Array = ";
        bdlb::PrintMethods::print(stream, d_array,
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
        stream << "Array = ";
        bdlb::PrintMethods::print(stream, d_array,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

                    // -----------------------------------
                    // class MySequenceWithAnonymousChoice
                    // -----------------------------------

// CONSTANTS

const char MySequenceWithAnonymousChoice::CLASS_NAME[] =
                                               "MySequenceWithAnonymousChoice";
    // the name of this class

const bdlat_AttributeInfo
MySequenceWithAnonymousChoice::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "attribute1",             // name
        sizeof("attribute1") - 1, // name length
        "",  // annotation
        bdlat_FormattingMode::e_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_CHOICE,
        "Choice",             // name
        sizeof("Choice") - 1, // name length
        "",  // annotation
        bdlat_FormattingMode::e_DEFAULT // formatting mode
      | bdlat_FormattingMode::e_UNTAGGED
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE2,
        "attribute2",             // name
        sizeof("attribute2") - 1, // name length
        "",  // annotation
        bdlat_FormattingMode::e_TEXT // formatting mode
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *MySequenceWithAnonymousChoice::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    if (bdlb::String::areEqualCaseless("myChoice1", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];         // RETURN
    }

    if (bdlb::String::areEqualCaseless("myChoice2", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];         // RETURN
    }

    switch(nameLength) {
        case 6: {
            if (bdlb::CharType::toUpper(name[0])=='C'
             && bdlb::CharType::toUpper(name[1])=='H'
             && bdlb::CharType::toUpper(name[2])=='O'
             && bdlb::CharType::toUpper(name[3])=='I'
             && bdlb::CharType::toUpper(name[4])=='C'
             && bdlb::CharType::toUpper(name[5])=='E')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]; // RETURN
            }
        } break;
        case 10: {
            if (bdlb::CharType::toUpper(name[0])=='A'
             && bdlb::CharType::toUpper(name[1])=='T'
             && bdlb::CharType::toUpper(name[2])=='T'
             && bdlb::CharType::toUpper(name[3])=='R'
             && bdlb::CharType::toUpper(name[4])=='I'
             && bdlb::CharType::toUpper(name[5])=='B'
             && bdlb::CharType::toUpper(name[6])=='U'
             && bdlb::CharType::toUpper(name[7])=='T'
             && bdlb::CharType::toUpper(name[8])=='E')
            {
                switch(bdlb::CharType::toUpper(name[9])) {
                    case '1': {
                      return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
                                                                      // RETURN
                    } break;
                    case '2': {
                      return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2];
                                                                      // RETURN
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdlat_AttributeInfo *
MySequenceWithAnonymousChoice::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
      case ATTRIBUTE_ID_CHOICE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
      case ATTRIBUTE_ID_ATTRIBUTE2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2];
      default:
        return 0;
    }
}

// ACCESSORS

bsl::ostream& MySequenceWithAnonymousChoice::print(
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
        stream << "Attribute1 = ";
        bdlb::PrintMethods::print(stream, d_attribute1,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Choice = ";
        bdlb::PrintMethods::print(stream, d_choice,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute2 = ";
        bdlb::PrintMethods::print(stream, d_attribute2,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Attribute1 = ";
        bdlb::PrintMethods::print(stream, d_attribute1,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Choice = ";
        bdlb::PrintMethods::print(stream, d_choice,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Attribute2 = ";
        bdlb::PrintMethods::print(stream, d_attribute2,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

                            // -------------------
                            // class TimingRequest
                            // -------------------

// CONSTANTS

const char TimingRequest::CLASS_NAME[] = "TimingRequest";
    // the name of this class

const bdlat_SelectionInfo TimingRequest::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_SQRT,
        "sqrt",               // name
        sizeof("sqrt") - 1,   // name length
        "",  // annotation
        bdlat_FormattingMode::e_DEFAULT // formatting mode
    },
    {
        SELECTION_ID_BASIC,
        "basic",               // name
        sizeof("basic") - 1,   // name length
        "",  // annotation
        bdlat_FormattingMode::e_DEFAULT // formatting mode
    },
    {
        SELECTION_ID_BIG,
        "big",               // name
        sizeof("big") - 1,   // name length
        "",  // annotation
        bdlat_FormattingMode::e_DEFAULT // formatting mode
    }
};

// CLASS METHODS

const bdlat_SelectionInfo *TimingRequest::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 3: {
            if (bdlb::CharType::toUpper(name[0])=='B'
             && bdlb::CharType::toUpper(name[1])=='I'
             && bdlb::CharType::toUpper(name[2])=='G')
            {
                return &SELECTION_INFO_ARRAY[SELECTION_INDEX_BIG];    // RETURN
            }
        } break;
        case 4: {
            if (bdlb::CharType::toUpper(name[0])=='S'
             && bdlb::CharType::toUpper(name[1])=='Q'
             && bdlb::CharType::toUpper(name[2])=='R'
             && bdlb::CharType::toUpper(name[3])=='T')
            {
                return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SQRT];   // RETURN
            }
        } break;
        case 5: {
            if (bdlb::CharType::toUpper(name[0])=='B'
             && bdlb::CharType::toUpper(name[1])=='A'
             && bdlb::CharType::toUpper(name[2])=='S'
             && bdlb::CharType::toUpper(name[3])=='I'
             && bdlb::CharType::toUpper(name[4])=='C')
            {
                return &SELECTION_INFO_ARRAY[SELECTION_INDEX_BASIC];  // RETURN
            }
        } break;
    }
    return 0;
}

const bdlat_SelectionInfo *TimingRequest::lookupSelectionInfo(int id)
{
    switch (id) {
      case SELECTION_ID_SQRT:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SQRT];
      case SELECTION_ID_BASIC:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_BASIC];
      case SELECTION_ID_BIG:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_BIG];
      default:
        return 0;
    }
}

// ACCESSORS

bsl::ostream& TimingRequest::print(
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

        switch (d_selectionId) {
          case SELECTION_ID_SQRT: {
            stream << "Sqrt = ";
            bdlb::PrintMethods::print(stream, d_sqrt.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_BASIC: {
            stream << "Basic = ";
            bdlb::PrintMethods::print(stream, d_basic.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_BIG: {
            stream << "Big = ";
            bdlb::PrintMethods::print(stream, d_big.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED\n";
        }
        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << "[ ";

        switch (d_selectionId) {
          case SELECTION_ID_SQRT: {
            stream << "Sqrt = ";
            bdlb::PrintMethods::print(stream, d_sqrt.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_BASIC: {
            stream << "Basic = ";
            bdlb::PrintMethods::print(stream, d_basic.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_BIG: {
            stream << "Big = ";
            bdlb::PrintMethods::print(stream, d_big.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED";
        }

        stream << " ]";
    }

    return stream << bsl::flush;
}

}  // close package namespace
}  // close enterprise namespace

// GENERATED BY BLP_BAS_CODEGEN_2.1.8
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------

// ************************ END OF GENERATED CODE **************************

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------
//..
    namespace BloombergLP {
    namespace usage {

    struct EmployeeRecord {
        // This struct represents a sequence containing a 'string' member, an
        // 'int' member, and a 'float' member.

        // CONSTANTS
        enum {
            NAME_ATTRIBUTE_ID   = 1,
            AGE_ATTRIBUTE_ID    = 2,
            SALARY_ATTRIBUTE_ID = 3
        };

        // DATA
        bsl::string d_name;
        int         d_age;
        float       d_salary;

        // CREATORS
        EmployeeRecord();
            // Create an 'EmployeeRecord' having the attributes:
            //..
            //  d_name   == ""
            //  d_age    == 0
            //  d_salary = 0.0
            //..
        EmployeeRecord(const bsl::string& name, int age, float salary);
            // Create an 'EmployeeRecord' object having the specified
            // 'name', 'age', and 'salary' attributes.

        // ACCESSORS
        const bsl::string& name()   const;
        int                age()    const;
        float              salary() const;
    };

    // CREATORS
    EmployeeRecord::EmployeeRecord()
    : d_name()
    , d_age()
    , d_salary()
    {
    }

    EmployeeRecord::EmployeeRecord(const bsl::string& name,
                                   int               age,
                                   float             salary)
    : d_name(name)
    , d_age(age)
    , d_salary(salary)
    {
    }

    // ACCESSORS
    const bsl::string& EmployeeRecord::name() const
    {
        return d_name;
    }

    int EmployeeRecord::age() const
    {
        return d_age;
    }

    float EmployeeRecord::salary() const
    {
        return d_salary;
    }

    }  // close namespace 'usage'

    namespace usage {

    template <typename MANIPULATOR>
    int bdlat_sequenceManipulateAttribute(EmployeeRecord *object,
                                          MANIPULATOR&    manipulator,
                                          const char     *attributeName,
                                          int             attributeNameLength);
    template <typename MANIPULATOR>
    int bdlat_sequenceManipulateAttribute(EmployeeRecord *object,
                                          MANIPULATOR&    manipulator,
                                          int             attributeId);
    template <typename MANIPULATOR>
    int bdlat_sequenceManipulateAttributes(EmployeeRecord *object,
                                           MANIPULATOR&    manipulator);
    template <typename ACCESSOR>
    int bdlat_sequenceAccessAttribute(
                                   const EmployeeRecord&  object,
                                   ACCESSOR&              accessor,
                                   const char            *attributeName,
                                   int                    attributeNameLength);
    template <typename ACCESSOR>
    int bdlat_sequenceAccessAttribute(const EmployeeRecord& object,
                                      ACCESSOR&             accessor,
                                      int                   attributeId);
    template <typename ACCESSOR>
    int bdlat_sequenceAccessAttributes(const EmployeeRecord& object,
                                       ACCESSOR&             accessor);
    bool bdlat_sequenceHasAttribute(
                                   const EmployeeRecord&  object,
                                   const char            *attributeName,
                                   int                    attributeNameLength);
    bool bdlat_sequenceHasAttribute(const EmployeeRecord& object,
                                    int                   attributeId);

    }  // close namespace 'usage'

    template <typename MANIPULATOR>
    int usage::bdlat_sequenceManipulateAttribute(
                                           EmployeeRecord *object,
                                           MANIPULATOR&    manipulator,
                                           const char     *attributeName,
                                           int             attributeNameLength)
    {
        enum { k_NOT_FOUND = -1 };

        if (bdlb::String::areEqualCaseless("name",
                                           attributeName,
                                           attributeNameLength)) {

            return bdlat_sequenceManipulateAttribute(
                                            object,
                                            manipulator,
                                            EmployeeRecord::NAME_ATTRIBUTE_ID);
        }

        if (bdlb::String::areEqualCaseless("age",
                                           attributeName,
                                           attributeNameLength)) {

            return bdlat_sequenceManipulateAttribute(
                                             object,
                                             manipulator,
                                             EmployeeRecord::AGE_ATTRIBUTE_ID);
        }

        if (bdlb::String::areEqualCaseless("salary",
                                           attributeName,
                                           attributeNameLength)) {

            return bdlat_sequenceManipulateAttribute(
                                          object,
                                          manipulator,
                                          EmployeeRecord::SALARY_ATTRIBUTE_ID);
        }

        return k_NOT_FOUND;
    }

    template <typename MANIPULATOR>
    int usage::bdlat_sequenceManipulateAttribute(EmployeeRecord  *object,
                                                 MANIPULATOR&     manipulator,
                                                 int              attributeId)
    {
        enum { k_NOT_FOUND = -1 };

        switch (attributeId) {
          case EmployeeRecord::NAME_ATTRIBUTE_ID: {
            bdlat_AttributeInfo info;

            info.annotation()     = "Name of employee";
            info.formattingMode() = bdlat_FormattingMode::e_DEFAULT;
            info.id()             = EmployeeRecord::NAME_ATTRIBUTE_ID;
            info.name()           = "name";
            info.nameLength()     = 4;

            return manipulator(&object->d_name, info);
          }
          case EmployeeRecord::AGE_ATTRIBUTE_ID: {
            bdlat_AttributeInfo info;

            info.annotation()     = "Age of employee";
            info.formattingMode() = bdlat_FormattingMode::e_DEFAULT;
            info.id()             = EmployeeRecord::AGE_ATTRIBUTE_ID;
            info.name()           = "age";
            info.nameLength()     = 3;

            return manipulator(&object->d_age, info);
          }
          case EmployeeRecord::SALARY_ATTRIBUTE_ID: {
            bdlat_AttributeInfo info;

            info.annotation()     = "Salary of employee";
            info.formattingMode() = bdlat_FormattingMode::e_DEFAULT;
            info.id()             = EmployeeRecord::SALARY_ATTRIBUTE_ID;
            info.name()           = "salary";
            info.nameLength()     = 6;

            return manipulator(&object->d_salary, info);
          }
          default: {
              return k_NOT_FOUND;
          }
        }
    }

    template <typename MANIPULATOR>
    int usage::bdlat_sequenceManipulateAttributes(
                                                 EmployeeRecord   *object,
                                                 MANIPULATOR&      manipulator)
    {
        int retVal;

        retVal = bdlat_sequenceManipulateAttribute(
                                            object,
                                            manipulator,
                                            EmployeeRecord::NAME_ATTRIBUTE_ID);

        if (0 != retVal) {
            return retVal;
        }

        retVal = bdlat_sequenceManipulateAttribute(
                                             object,
                                             manipulator,
                                             EmployeeRecord::AGE_ATTRIBUTE_ID);

        if (0 != retVal) {
            return retVal;
        }

        retVal = bdlat_sequenceManipulateAttribute(
                                          object,
                                          manipulator,
                                          EmployeeRecord::SALARY_ATTRIBUTE_ID);

        return retVal;
    }

    // ACCESSORS
    template <typename ACCESSOR>
    int usage::bdlat_sequenceAccessAttribute(
                                    const EmployeeRecord&  object,
                                    ACCESSOR&              accessor,
                                    const char            *attributeName,
                                    int                    attributeNameLength)
    {
        enum { k_NOT_FOUND = -1 };

        if (bdlb::String::areEqualCaseless("name",
                                           attributeName,
                                           attributeNameLength)) {
            return bdlat_sequenceAccessAttribute(
                                            object,
                                            accessor,
                                            EmployeeRecord::NAME_ATTRIBUTE_ID);
        }

        if (bdlb::String::areEqualCaseless("age",
                                           attributeName,
                                           attributeNameLength)) {
            return bdlat_sequenceAccessAttribute(
                                             object,
                                             accessor,
                                             EmployeeRecord::AGE_ATTRIBUTE_ID);
        }

        if (bdlb::String::areEqualCaseless("salary",
                                           attributeName,
                                           attributeNameLength)) {

            return bdlat_sequenceAccessAttribute(
                                          object,
                                          accessor,
                                          EmployeeRecord::SALARY_ATTRIBUTE_ID);
        }

        return k_NOT_FOUND;
    }

    template <typename ACCESSOR>
    int usage::bdlat_sequenceAccessAttribute(const EmployeeRecord& object,
                                             ACCESSOR&             accessor,
                                             int                   attributeId)
    {
        enum { k_NOT_FOUND = -1 };

        switch (attributeId) {
          case EmployeeRecord::NAME_ATTRIBUTE_ID: {
            bdlat_AttributeInfo info;

            info.annotation()     = "Name of employee";
            info.formattingMode() = bdlat_FormattingMode::e_DEFAULT;
            info.id()             = EmployeeRecord::NAME_ATTRIBUTE_ID;
            info.name()           = "name";
            info.nameLength()     = 4;

            return accessor(object.d_name, info);
          }
          case EmployeeRecord::AGE_ATTRIBUTE_ID: {
            bdlat_AttributeInfo info;

            info.annotation()     = "Age of employee";
            info.formattingMode() = bdlat_FormattingMode::e_DEFAULT;
            info.id()             = EmployeeRecord::AGE_ATTRIBUTE_ID;
            info.name()           = "age";
            info.nameLength()     = 3;

            return accessor(object.d_age, info);
          }
          case EmployeeRecord::SALARY_ATTRIBUTE_ID: {
            bdlat_AttributeInfo info;

            info.annotation()     = "Salary of employee";
            info.formattingMode() = bdlat_FormattingMode::e_DEFAULT;
            info.id()             = EmployeeRecord::SALARY_ATTRIBUTE_ID;
            info.name()           = "salary";
            info.nameLength()     = 6;

            return accessor(object.d_salary, info);
          }
          default: {
              return k_NOT_FOUND;
          }
        }
    }

    template <typename ACCESSOR>
    int usage::bdlat_sequenceAccessAttributes(const EmployeeRecord& object,
                                              ACCESSOR&             accessor)
    {
        int retVal;

        retVal = bdlat_sequenceAccessAttribute(
                                            object,
                                            accessor,
                                            EmployeeRecord::NAME_ATTRIBUTE_ID);

        if (0 != retVal) {
            return retVal;
        }

        retVal = bdlat_sequenceAccessAttribute(
                                             object,
                                             accessor,
                                             EmployeeRecord::AGE_ATTRIBUTE_ID);

        if (0 != retVal) {
            return retVal;
        }

        retVal = bdlat_sequenceAccessAttribute(
                                          object,
                                          accessor,
                                          EmployeeRecord::SALARY_ATTRIBUTE_ID);

        return retVal;
    }

    bool usage::bdlat_sequenceHasAttribute(
                                    const EmployeeRecord&  ,
                                    const char            *attributeName,
                                    int                    attributeNameLength)
    {
        return bdlb::String::areEqualCaseless("name",
                                              attributeName,
                                              attributeNameLength)
            || bdlb::String::areEqualCaseless("age",
                                              attributeName,
                                              attributeNameLength)
            || bdlb::String::areEqualCaseless("salary",
                                              attributeName,
                                              attributeNameLength);
    }

    bool usage::bdlat_sequenceHasAttribute(const EmployeeRecord& ,
                                           int                   attributeId)
    {
        return EmployeeRecord::NAME_ATTRIBUTE_ID   == attributeId
            || EmployeeRecord::AGE_ATTRIBUTE_ID    == attributeId
            || EmployeeRecord::SALARY_ATTRIBUTE_ID == attributeId;
    }

    namespace bdlat_SequenceFunctions {

        template <>
        struct IsSequence<usage::EmployeeRecord> {
            enum { VALUE = 1 };
        };

    }  // close namespace 'bdlat_SequenceFunctions'
    }  // close enterprise namespace

static void usageExample()
{
    using namespace BloombergLP;
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Decoding an Employee Record
/// - - - - - - - - - - - - - - - - - - -
// Suppose that an "employee record" consists of a sequence of attributes --
// 'name', 'age', and 'salary' -- that are of types 'bsl::string', 'int', and
// 'float', respectively.  Furthermore, we have a need to BER encode employee
// records as a sequence of values (for out-of-process consumption).
//
// Assume that we have defined a 'usage::EmployeeRecord' class to represent
// employee record values, and assume that we have provided the 'bdlat'
// specializations that allow the 'balber' codec components to represent class
// values as a sequence of BER primitive values.  See
// {'bdlat_sequencefunctions'|Usage} for details of creating specializations
// for a sequence type.
//
// First, we create an employee record object having typical values:
//..
    usage::EmployeeRecord bob("Bob", 56, 1234.00);
    ASSERT("Bob"   == bob.name());
    ASSERT(  56    == bob.age());
    ASSERT(1234.00 == bob.salary());
//..
// Next, we create a 'balber::Encoder' object and use it to encode our 'bob'
// object.  Here, to facilitate the examination of our results, the BER
// encoding data is delivered to a 'bslsb::MemOutStreamBuf' object:
//..
    bdlsb::MemOutStreamBuf osb;
    balber::BerEncoder     encoder;
    int                    rc = encoder.encode(&osb, bob);
    ASSERT( 0 == rc);
    ASSERT(18 == osb.length());
//..
// Now, we create a 'bdlsb::FixedMemInStreamBuf' object to manage our access
// to the data portion of the 'bdlsb::MemOutStreamBuf' (where our BER encoding
// resides), decode the values found there, and use them to set the value
// of an 'usage::EmployeeRecord' object.
//..
    balber::BerDecoderOptions  options;
    balber::BerDecoder         decoder(&options);
    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
    usage::EmployeeRecord      obj;

    rc = decoder.decode(&isb, &obj);
    ASSERT(0 == rc);
//..
// Finally, we confirm that the object defined by the BER encoding has the
// same value as the original object.
//..
    ASSERT(bob.name()   == obj.name());
    ASSERT(bob.age()    == obj.age());
    ASSERT(bob.salary() == obj.salary());
//..
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int        test = argc > 1 ? bsl::atoi(argv[1]) : 0;
            verbose = argc > 2;
        veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    balber::BerDecoderOptions  options;

    if (veryVeryVerbose) {
       options.setTraceLevel(1);
    }

    balber::BerEncoder         encoder(0);
    balber::BerDecoder         decoder(&options);

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 18: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Usage Example"
                               << "\n=====================" << bsl::endl;

        usageExample();

        if (verbose) bsl::cout << "\nEnd of test." << bsl::endl;
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING decoding for date/time components using a variant
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout
                            << "\nTESTING decoding for date/time using variant"
                            << "\n============================================"
                            << bsl::endl;

        balber::BerEncoderOptions options;
        options.setEncodeDateAndTimeTypesAsBinary(true);
        const balber::BerEncoderOptions DEFOPTS;

        if (verbose) bsl::cout << "\nDefine data" << bsl::endl;

        static const struct {
            int d_lineNum;   // source line number
            int d_year;      // year under test
            int d_month;     // month under test
            int d_day;       // day under test
            int d_hour;      // hour under test
            int d_minutes;   // minutes under test
            int d_seconds;   // seconds under test
            int d_milliSecs; // milli seconds under test
            int d_tzoffset;  // time zone offset
        } DATA[] = {
   //line no.  year   month   day   hour    min   sec    ms  offset
   //-------   -----  -----   ---   ----    ---   ---    --  ------
    {      L_,      1,     1,    1,     0,     0,    0,    0,      0     },
    {      L_,      1,     1,    1,     0,     0,    0,    0,     45     },
    {      L_,      1,     1,    1,     0,     0,    0,    0,  -1439     },

    {      L_,      1,     1,    1,     1,     1,    1,    1,      0     },
    {      L_,      1,     1,    1,     1,     1,    1,    1,    500     },
    {      L_,      1,     1,    1,     0,     0,    0,    0,  -1439     },

    {      L_,      1,     1,    1,     1,    23,   59,   59,      0     },
    {      L_,      1,     1,    1,     1,    23,   59,   59,   1439     },
    {      L_,      1,     1,    1,     1,    23,   59,   59,  -1439     },

    {      L_,      1,     1,    2,     0,     0,    0,    0,      0     },
    {      L_,      1,     1,    2,     0,     0,    0,    0,   1439     },
    {      L_,      1,     1,    2,     0,     0,    0,    0,  -1439     },

    {      L_,      1,     1,    2,     1,     1,    1,    1,      0     },
    {      L_,      1,     1,    2,     1,     1,    1,    1,    500     },

    {      L_,      1,     1,    2,     1,    23,   59,   59,      0     },
    {      L_,      1,     1,    2,     1,    23,   59,   59,    500     },
    {      L_,      1,     1,    2,     1,    23,   59,   59,   -500     },

    {      L_,      1,     1,   10,     0,     0,    0,    0,      0     },
    {      L_,      1,     1,   10,     1,     1,    1,    1,     99     },

    {      L_,      1,     1,   30,     0,     0,    0,    0,      0     },
    {      L_,      1,     1,   31,     0,     0,    0,    0,   1439     },
    {      L_,      1,     1,   31,     0,     0,    0,    0,  -1439     },

    {      L_,      1,     2,    1,     0,     0,    0,    0,      0     },
    {      L_,      1,     2,    1,    23,    59,   59,    0,   1439     },

    {      L_,      1,    12,   31,     0,     0,    0,    0,      0     },
    {      L_,      1,    12,   31,    23,    59,   59,    0,   1439     },

    {      L_,      2,     1,    1,     0,     0,    0,    0,      0     },
    {      L_,      2,     1,    1,    23,    59,   59,    0,   1439     },

    {      L_,      4,     1,    1,     0,     0,    0,    0,      0     },
    {      L_,      4,     1,    1,    23,    59,   59,    0,   1439     },

    {      L_,      4,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,      4,     2,   28,    23,    59,   59,    0,   1439     },
    {      L_,      4,     2,   28,    23,    59,   59,    0,  -1439     },

    {      L_,      4,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,      4,     2,   29,    23,    59,   59,    0,   1439     },
    {      L_,      4,     2,   29,    23,    59,   59,    0,  -1439     },

    {      L_,      4,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,      4,     3,    1,    23,    59,   59,    0,   1439     },
    {      L_,      4,     3,    1,    23,    59,   59,    0,  -1439     },

    {      L_,      8,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,      8,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,      8,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,      8,     2,   29,    23,    59,   59,    0,   1439     },

    {      L_,      8,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,      8,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,    100,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,    100,     2,   28,    23,    59,   59,    0,   1439     },
    {      L_,    100,     2,   28,    23,    59,   59,    0,  -1439     },

    {      L_,    100,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,    100,     3,    1,    23,    59,   59,    0,   1439     },
    {      L_,    100,     3,    1,    23,    59,   59,    0,  -1439     },

    {      L_,    400,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,    400,     2,   28,    23,    59,   59,    0,   1439     },
    {      L_,    400,     2,   28,    23,    59,   59,    0,  -1439     },

    {      L_,    400,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,    400,     2,   29,    23,    59,   59,    0,   1439     },
    {      L_,    400,     2,   29,    23,    59,   59,    0,  -1439     },

    {      L_,    400,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,    400,     3,    1,    23,    59,   59,    0,   1439     },
    {      L_,    400,     3,    1,    23,    59,   59,    0,  -1439     },

    {      L_,    500,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,    500,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,    500,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,    500,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,    800,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,    800,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,    800,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,    800,     2,   29,    23,    59,   59,    0,   1439     },

    {      L_,    800,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,    800,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,   1000,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,   1000,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,   1000,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,   1000,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,   2000,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,   2000,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,   2000,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,   2000,     2,   29,    23,    59,   59,    0,   1439     },

    {      L_,   2000,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,   2000,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,   2016,    12,   31,     0,     0,    0,    0,      0     },
    {      L_,   2017,    12,   31,     0,     0,    0,    0,      0     },
    {      L_,   2018,    12,   31,     0,     0,    0,    0,      0     },
    {      L_,   2019,    12,   31,     0,     0,    0,    0,      0     },

    {      L_,   2020,     1,    1,     0,     0,    0,    0,      0     },
    {      L_,   2020,     1,    1,     0,     0,    0,    0,   1439     },
    {      L_,   2020,     1,    1,     0,     0,    0,    0,  -1439     },

    {      L_,   2020,     1,    1,    23,    59,   59,  999,      0     },
    {      L_,   2020,     1,    1,    23,    59,   59,  999,   1439     },
    {      L_,   2020,     1,    1,    23,    59,   59,  999,  -1439     },

    {      L_,   2020,     1,    2,     0,     0,    0,    0,      0     },
    {      L_,   2020,     1,    2,     0,     0,    0,    0,   1439     },
    {      L_,   2020,     1,    2,     0,     0,    0,    0,  -1439     },

    {      L_,   2020,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,   2020,     2,   28,    23,    59,   59,    0,   1439     },
    {      L_,   2020,     2,   28,    23,    59,   59,    0,  -1439     },

    {      L_,   2020,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,   2020,     2,   29,    23,    59,   59,    0,   1439     },
    {      L_,   2020,     2,   29,    23,    59,   59,    0,  -1439     },

    {      L_,   2020,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,   2020,     3,    1,    23,    59,   59,    0,   1439     },
    {      L_,   2020,     3,    1,    23,    59,   59,    0,  -1439     },

    {      L_,   2021,     1,    2,     0,     0,    0,    0,      0     },
    {      L_,   2022,     1,    2,     0,     0,    0,    0,      0     },

    {      L_,   9999,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,   9999,     2,   28,    23,    59,   59,    0,   1439     },
    {      L_,   9999,     2,   28,    23,    59,   59,    0,  -1439     },

    {      L_,   9999,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,   9999,     3,    1,    23,    59,   59,    0,   1439     },
    {      L_,   9999,     3,    1,    23,    59,   59,    0,  -1439     },

    {      L_,   9999,    12,   30,     0,     0,    0,    0,      0     },
    {      L_,   9999,    12,   30,    23,    59,   59,    0,   1439     },

    {      L_,   9999,    12,   31,     0,     0,    0,    0,      0     },
    {      L_,   9999,    12,   31,    23,    59,   59,    0,   1439     },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) bsl::cout << "\nTesting 'bdlt::Date'." << bsl::endl;
        {
            typedef bdlb::Variant2<bdlt::Date, bdlt::DateTz> Variant;

            typedef bdlt::Date Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int Y    = DATA[i].d_year;
                const int M    = DATA[i].d_month;
                const int D    = DATA[i].d_day;

                if (veryVerbose) { P_(Y) P_(M) P(D) }

                const Type VALUE(Y, M, D);
                Variant    value1, value2;

                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder encoder(&options);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value1));
                    printDiagnostic(decoder);

                    LOOP_ASSERT(LINE, value1.is<bdlt::Date>());
                    LOOP3_ASSERT(LINE, VALUE, value1,
                                 VALUE == value1.the<bdlt::Date>());
                    if (veryVerbose) {
                        P(VALUE);
                        P(value1);
                    }
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder encoder(&DEFOPTS);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value2));
                    printDiagnostic(decoder);

                    LOOP_ASSERT(LINE, value2.is<bdlt::Date>());
                    LOOP3_ASSERT(LINE, VALUE, value2,
                                 VALUE == value2.the<bdlt::Date>());
                    if (veryVerbose) {
                        P(VALUE);
                        P(value2);
                    }
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting 'bdlt::DateTz'." << bsl::endl;
        {
            typedef bdlb::Variant2<bdlt::Date, bdlt::DateTz> Variant;

            typedef bdlt::DateTz Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int Y    = DATA[i].d_year;
                const int M    = DATA[i].d_month;
                const int D    = DATA[i].d_day;
                const int OFF  = DATA[i].d_tzoffset;

                const Type VALUE(bdlt::Date(Y, M, D), OFF);
                Variant    value1, value2;

                if (veryVerbose) { P_(Y) P_(M) P_(D) P(OFF) }

                if (OFF) {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder encoder(&options);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value1));
                    printDiagnostic(decoder);

                    LOOP_ASSERT(LINE, value1.is<bdlt::DateTz>());
                    LOOP3_ASSERT(LINE, VALUE, value1,
                                 VALUE == value1.the<bdlt::DateTz>());
                    if (veryVerbose) {
                        P(VALUE);
                        P(value1);
                    }
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder encoder(&DEFOPTS);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value2));
                    printDiagnostic(decoder);

                    LOOP_ASSERT(LINE, value2.is<bdlt::DateTz>());
                    LOOP3_ASSERT(LINE, VALUE, value2,
                                 VALUE == value2.the<bdlt::DateTz>());
                    if (veryVerbose) {
                        P(VALUE);
                        P(value1);
                    }
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting 'bdlt::Time'." << bsl::endl;
        {
            typedef bdlb::Variant2<bdlt::Time, bdlt::TimeTz> Variant;

            typedef bdlt::Time Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int H    = DATA[i].d_hour;
                const int MM   = DATA[i].d_minutes;
                const int S    = DATA[i].d_seconds;
                const int MS   = DATA[i].d_milliSecs;

                if (veryVerbose) { P_(H) P_(MM) P_(S) P(MS) }

                const Type VALUE(H, MM, S, MS);
                Variant    value1, value2;

                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder encoder(&options);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value1));
                    printDiagnostic(decoder);

                    LOOP_ASSERT(LINE, value1.is<bdlt::Time>());
                    LOOP3_ASSERT(LINE, VALUE, value1,
                                 VALUE == value1.the<bdlt::Time>());
                    if (veryVerbose) {
                        P(VALUE);
                        P(value1);
                    }
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder encoder(&DEFOPTS);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value2));
                    printDiagnostic(decoder);

                    LOOP_ASSERT(LINE, value2.is<bdlt::Time>());
                    LOOP3_ASSERT(LINE, VALUE, value2,
                                 VALUE == value2.the<bdlt::Time>());
                    if (veryVerbose) {
                        P(VALUE);
                        P(value2);
                    }
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting 'bdlt::TimeTz'." << bsl::endl;
        {
            typedef bdlb::Variant2<bdlt::Time, bdlt::TimeTz> Variant;

            typedef bdlt::TimeTz Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int H    = DATA[i].d_hour;
                const int MM   = DATA[i].d_minutes;
                const int S    = DATA[i].d_seconds;
                const int MS   = DATA[i].d_milliSecs;
                const int OFF  = DATA[i].d_tzoffset;

                const Type VALUE(bdlt::Time(H, MM, S, MS), OFF);
                Variant    value1, value2;

                if (veryVerbose) { P_(H) P_(MM) P_(S) P_(MS) P(OFF) }

                if (OFF) {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder encoder(&options);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value1));
                    printDiagnostic(decoder);

                    LOOP_ASSERT(LINE, value1.is<bdlt::TimeTz>());
                    LOOP3_ASSERT(LINE, VALUE, value1,
                                 VALUE == value1.the<bdlt::TimeTz>());
                    if (veryVerbose) {
                        P(VALUE);
                        P(value1);
                    }
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder encoder(&DEFOPTS);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value2));
                    printDiagnostic(decoder);

                    LOOP_ASSERT(LINE, value2.is<bdlt::TimeTz>());
                    LOOP3_ASSERT(LINE, VALUE, value2,
                                 VALUE == value2.the<bdlt::TimeTz>());
                    if (veryVerbose) {
                        P(VALUE);
                        P(value1);
                    }
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting 'bdlt::Datetime'." << bsl::endl;
        {
            typedef bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz> Variant;

            typedef bdlt::Datetime Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int Y    = DATA[i].d_year;
                const int M    = DATA[i].d_month;
                const int D    = DATA[i].d_day;
                const int H    = DATA[i].d_hour;
                const int MM   = DATA[i].d_minutes;
                const int S    = DATA[i].d_seconds;
                const int MS   = DATA[i].d_milliSecs;

                const Type VALUE(Y, M, D, H, MM, S, MS);
                Variant    value1, value2;

                if (veryVerbose) { P_(Y) P_(M) P_(D) P_(H)
                                   P_(MM) P_(S) P(MS) }

                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder encoder(&options);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (osb.length() > 6) {
                        // Datetime objects having length greater that 6 bytes
                        // are always encoded with a time zone.

                        continue;
                    }

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value1));
                    printDiagnostic(decoder);

                    LOOP_ASSERT(LINE, value1.is<bdlt::Datetime>());
                    LOOP3_ASSERT(LINE, VALUE, value1,
                                 VALUE == value1.the<bdlt::Datetime>());
                    if (veryVerbose) {
                        P(VALUE);
                        P(value1);
                    }
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder encoder(&DEFOPTS);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value2));
                    printDiagnostic(decoder);

                    LOOP_ASSERT(LINE, value2.is<bdlt::Datetime>());
                    LOOP3_ASSERT(LINE, VALUE, value2,
                                 VALUE == value2.the<bdlt::Datetime>());
                    if (veryVerbose) {
                        P(VALUE);
                        P(value2);
                    }
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting 'bdlt::DatetimeTz'." << bsl::endl;
        {
            typedef bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz> Variant;

            typedef bdlt::DatetimeTz Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int Y    = DATA[i].d_year;
                const int M    = DATA[i].d_month;
                const int D    = DATA[i].d_day;
                const int H    = DATA[i].d_hour;
                const int MM   = DATA[i].d_minutes;
                const int S    = DATA[i].d_seconds;
                const int MS   = DATA[i].d_milliSecs;
                const int OFF  = DATA[i].d_tzoffset;

                const Type VALUE(bdlt::Datetime(Y, M, D, H, MM, S, MS), OFF);
                Variant    value1, value2;

                if (veryVerbose) { P_(Y) P_(M) P_(D) P_(H)
                                   P_(MM) P_(S) P_(MS) P(OFF) }

                if (OFF) {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder encoder(&options);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value1));
                    printDiagnostic(decoder);

                    LOOP_ASSERT(LINE, value1.is<bdlt::DatetimeTz>());
                    LOOP3_ASSERT(LINE, VALUE, value1,
                                 VALUE == value1.the<bdlt::DatetimeTz>());
                    if (veryVerbose) {
                        P(VALUE);
                        P(value1);
                    }
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder encoder(&DEFOPTS);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value2));
                    printDiagnostic(decoder);

                    LOOP_ASSERT(LINE, value2.is<bdlt::DatetimeTz>());
                    LOOP3_ASSERT(LINE, VALUE, value2,
                                 VALUE == value2.the<bdlt::DatetimeTz>());
                    if (veryVerbose) {
                        P(VALUE);
                        P(value2);
                    }
                }
            }
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING encoding & decoding for date/time components
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING encoding & decoding for date/time"
                               << "\n========================================="
                               << bsl::endl;

        balber::BerEncoderOptions options;
        options.setEncodeDateAndTimeTypesAsBinary(true);
        const balber::BerEncoderOptions DEFOPTS;

        if (verbose) bsl::cout << "\nTesting Date Brute force." << bsl::endl;
        {
            const int YEARS[] = { 1, 4, 96, 100, 400, 500, 800, 1000, 1600,
                                  1700, 1751, 1752, 1753, 1930, 2000, 2010,
                                  2012, 2019, 2020, 2021, 6478, 6479, 6480,
                                  9998, 9999 };
            const int NUM_YEARS = sizeof YEARS / sizeof *YEARS;

            const int MONTHS[] = { 1, 2, 5, 8, 9, 12 };
            const int NUM_MONTHS = sizeof MONTHS / sizeof *MONTHS;

            const int DAYS[] = { 1, 2, 5, 10, 15, 20, 28, 29, 30, 31 };
            const int NUM_DAYS = sizeof DAYS / sizeof *DAYS;

            for (int i = 0; i < NUM_YEARS; ++i) {
            for (int j = 0; j < NUM_MONTHS; ++j) {
            for (int k = 0; k < NUM_DAYS; ++k) {

                const int YEAR  = YEARS[i];
                const int MONTH = MONTHS[j];
                const int DAY   = DAYS[k];

                if (bdlt::Date::isValidYearMonthDay(YEAR, MONTH, DAY)
                 && bdlt::SerialDateImpUtil::isValidYearMonthDay(YEAR,
                                                                 MONTH,
                                                                 DAY)) {

                    if (veryVerbose) { P_(YEAR) P_(MONTH) P(DAY) }

                    const bdlt::Date VALUE(YEAR, MONTH, DAY); bdlt::Date value;
                    const int OFF1 = 0, OFF2 =-1439, OFF3 =1439;
                    const bdlt::DateTz VALUE1(bdlt::Date(YEAR, MONTH, DAY),
                                             OFF1);
                    const bdlt::DateTz VALUE2(bdlt::Date(YEAR, MONTH, DAY),
                                             OFF2);
                    const bdlt::DateTz VALUE3(bdlt::Date(YEAR, MONTH, DAY),
                                             OFF3);
                    bdlt::DateTz value1, value2, value3;

                    {
                        bdlsb::MemOutStreamBuf osb;
                        balber::BerEncoder encoder(&DEFOPTS);
                        ASSERT(0 == encoder.encode(&osb, VALUE));

                        if (veryVerbose) {
                            P(osb.length());
                            printBuffer(osb.data(), osb.length());
                        }

                        bdlsb::FixedMemInStreamBuf isb(osb.data(),
                                                       osb.length());
                        ASSERT(0 == decoder.decode(&isb, &value));
                        printDiagnostic(decoder);

                        LOOP2_ASSERT(VALUE, value, VALUE == value);
                        if (veryVerbose) {
                            P(VALUE);
                            P(value);
                        }
                    }

                    {
                        bdlsb::MemOutStreamBuf osb1, osb2, osb3;
                        balber::BerEncoder encoder(&DEFOPTS);
                        ASSERT(0 == encoder.encode(&osb1, VALUE1));
                        ASSERT(0 == encoder.encode(&osb2, VALUE2));
                        ASSERT(0 == encoder.encode(&osb3, VALUE3));

                        if (veryVerbose) {
                            P(osb1.length());
                            P(osb2.length());
                            P(osb3.length());
                            printBuffer(osb1.data(), osb1.length());
                            printBuffer(osb2.data(), osb2.length());
                            printBuffer(osb3.data(), osb3.length());
                        }

                        bdlsb::FixedMemInStreamBuf isb1(osb1.data(),
                                                       osb1.length());
                        bdlsb::FixedMemInStreamBuf isb2(osb2.data(),
                                                       osb2.length());
                        bdlsb::FixedMemInStreamBuf isb3(osb3.data(),
                                                       osb3.length());

                        ASSERT(0 == decoder.decode(&isb1, &value1));
                        ASSERT(0 == decoder.decode(&isb2, &value2));
                        ASSERT(0 == decoder.decode(&isb3, &value3));
                        printDiagnostic(decoder);

                        LOOP2_ASSERT(VALUE1, value1, VALUE1 == value1);
                        LOOP2_ASSERT(VALUE2, value2, VALUE2 == value2);
                        LOOP2_ASSERT(VALUE3, value3, VALUE3 == value3);
                        if (veryVerbose) {
                            P(VALUE1); P(value1);
                            P(VALUE2); P(value2);
                            P(VALUE3); P(value3);
                        }
                    }

                    {
                        bdlsb::MemOutStreamBuf osb;
                        balber::BerEncoder encoder(&DEFOPTS);
                        ASSERT(0 == encoder.encode(&osb, VALUE));

                        if (veryVerbose) {
                            P(osb.length());
                            printBuffer(osb.data(), osb.length());
                        }

                        bdlsb::FixedMemInStreamBuf isb(osb.data(),
                                                      osb.length());
                        ASSERT(0 == decoder.decode(&isb, &value));
                        printDiagnostic(decoder);

                        LOOP2_ASSERT(VALUE, value, VALUE == value);
                        if (veryVerbose) {
                            P(VALUE);
                            P(value);
                        }
                    }

                    {
                        bdlsb::MemOutStreamBuf osb;
                        balber::BerEncoder encoder(&options);
                        ASSERT(0 == encoder.encode(&osb, VALUE));

                        if (veryVerbose) {
                            P(osb.length());
                            printBuffer(osb.data(), osb.length());
                        }

                        bdlsb::FixedMemInStreamBuf isb(osb.data(),
                                                      osb.length());
                        ASSERT(0 == decoder.decode(&isb, &value));
                        printDiagnostic(decoder);

                        LOOP2_ASSERT(VALUE, value, VALUE == value);
                        if (veryVerbose) {
                            P(VALUE);
                            P(value);
                        }
                    }

                    {
                        bdlsb::MemOutStreamBuf osb1, osb2, osb3;
                        balber::BerEncoder encoder(&options);
                        ASSERT(0 == encoder.encode(&osb1, VALUE1));
                        ASSERT(0 == encoder.encode(&osb2, VALUE2));
                        ASSERT(0 == encoder.encode(&osb3, VALUE3));

                        if (veryVerbose) {
                            P(osb1.length());
                            P(osb2.length());
                            P(osb3.length());
                            printBuffer(osb1.data(), osb1.length());
                            printBuffer(osb2.data(), osb2.length());
                            printBuffer(osb3.data(), osb3.length());
                        }

                        bdlsb::FixedMemInStreamBuf isb1(osb1.data(),
                                                       osb1.length());
                        bdlsb::FixedMemInStreamBuf isb2(osb2.data(),
                                                       osb2.length());
                        bdlsb::FixedMemInStreamBuf isb3(osb3.data(),
                                                       osb3.length());

                        ASSERT(0 == decoder.decode(&isb1, &value1));
                        ASSERT(0 == decoder.decode(&isb2, &value2));
                        ASSERT(0 == decoder.decode(&isb3, &value3));
                        printDiagnostic(decoder);

                        LOOP2_ASSERT(VALUE1, value1, VALUE1 == value1);
                        LOOP2_ASSERT(VALUE2, value2, VALUE2 == value2);
                        LOOP2_ASSERT(VALUE3, value3, VALUE3 == value3);
                        if (veryVerbose) {
                            P(VALUE1); P(value1);
                            P(VALUE2); P(value2);
                            P(VALUE3); P(value3);
                        }
                    }
                }
            }
            }
            }
        }

        if (verbose) bsl::cout << "\nTesting Time Brute force." << bsl::endl;
        {
            for (int hour = 0; hour <= 23; ++hour) {
                for (int min = 0; min < 60; ++min) {
                    for (int sec = 0; sec < 60; ++sec) {
                        if (veryVerbose) { P_(hour) P_(min) P(sec) }

                        const int MS = 0;
                        const bdlt::Time VALUE(hour, min, sec, MS);
                        bdlt::Time value;
                        const int MS1 = 0, MS2 = 500, MS3 = 999;
                        const int OFF1 = 0, OFF2 =-1439, OFF3 =1439;
                        const bdlt::TimeTz VALUE1(bdlt::Time(hour,
                                                           min,
                                                           sec,
                                                           MS1),
                                                 OFF1);
                        const bdlt::TimeTz VALUE2(bdlt::Time(hour,
                                                           min,
                                                           sec,
                                                           MS2),
                                                 OFF2);
                        const bdlt::TimeTz VALUE3(bdlt::Time(hour,
                                                           min,
                                                           sec,
                                                           MS3),
                                                 OFF3);
                        bdlt::TimeTz value1, value2, value3;

                        {
                            bdlsb::MemOutStreamBuf osb;
                            balber::BerEncoder encoder(&DEFOPTS);
                            ASSERT(0 == encoder.encode(&osb, VALUE));

                            if (veryVerbose) {
                                P(osb.length());
                                printBuffer(osb.data(), osb.length());
                            }

                            bdlsb::FixedMemInStreamBuf isb(osb.data(),
                                                          osb.length());
                            ASSERT(0 == decoder.decode(&isb, &value));
                            printDiagnostic(decoder);

                            LOOP2_ASSERT(VALUE, value, VALUE == value);
                            if (veryVerbose) {
                                P(VALUE);
                                P(value);
                            }
                        }

                        {
                            bdlsb::MemOutStreamBuf osb1, osb2, osb3;
                            balber::BerEncoder encoder(&DEFOPTS);
                            ASSERT(0 == encoder.encode(&osb1, VALUE1));
                            ASSERT(0 == encoder.encode(&osb2, VALUE2));
                            ASSERT(0 == encoder.encode(&osb3, VALUE3));

                            if (veryVerbose) {
                                P(osb1.length());
                                P(osb2.length());
                                P(osb3.length());
                                printBuffer(osb1.data(), osb1.length());
                                printBuffer(osb2.data(), osb2.length());
                                printBuffer(osb3.data(), osb3.length());
                            }

                            bdlsb::FixedMemInStreamBuf isb1(osb1.data(),
                                                           osb1.length());
                            bdlsb::FixedMemInStreamBuf isb2(osb2.data(),
                                                           osb2.length());
                            bdlsb::FixedMemInStreamBuf isb3(osb3.data(),
                                                           osb3.length());

                            ASSERT(0 == decoder.decode(&isb1, &value1));
                            ASSERT(0 == decoder.decode(&isb2, &value2));
                            ASSERT(0 == decoder.decode(&isb3, &value3));
                            printDiagnostic(decoder);

                            LOOP2_ASSERT(VALUE1, value1, VALUE1 == value1);
                            LOOP2_ASSERT(VALUE2, value2, VALUE2 == value2);
                            LOOP2_ASSERT(VALUE3, value3, VALUE3 == value3);
                            if (veryVerbose) {
                                P(VALUE1); P(value1);
                                P(VALUE2); P(value2);
                                P(VALUE3); P(value3);
                            }
                        }

                        {
                            bdlsb::MemOutStreamBuf osb;
                            balber::BerEncoder encoder(&options);
                            ASSERT(0 == encoder.encode(&osb, VALUE));

                            if (veryVerbose) {
                                P(osb.length());
                                printBuffer(osb.data(), osb.length());
                            }

                            bdlsb::FixedMemInStreamBuf isb(osb.data(),
                                                          osb.length());
                            ASSERT(0 == decoder.decode(&isb, &value));
                            printDiagnostic(decoder);

                            LOOP2_ASSERT(VALUE, value, VALUE == value);
                            if (veryVerbose) {
                                P(VALUE);
                                P(value);
                            }
                        }

                        {
                            bdlsb::MemOutStreamBuf osb1, osb2, osb3;
                            balber::BerEncoder encoder(&options);
                            ASSERT(0 == encoder.encode(&osb1, VALUE1));
                            ASSERT(0 == encoder.encode(&osb2, VALUE2));
                            ASSERT(0 == encoder.encode(&osb3, VALUE3));

                            if (veryVerbose) {
                                P(osb1.length());
                                P(osb2.length());
                                P(osb3.length());
                                printBuffer(osb1.data(), osb1.length());
                                printBuffer(osb2.data(), osb2.length());
                                printBuffer(osb3.data(), osb3.length());
                            }

                            bdlsb::FixedMemInStreamBuf isb1(osb1.data(),
                                                           osb1.length());
                            bdlsb::FixedMemInStreamBuf isb2(osb2.data(),
                                                           osb2.length());
                            bdlsb::FixedMemInStreamBuf isb3(osb3.data(),
                                                           osb3.length());

                            ASSERT(0 == decoder.decode(&isb1, &value1));
                            ASSERT(0 == decoder.decode(&isb2, &value2));
                            ASSERT(0 == decoder.decode(&isb3, &value3));
                            printDiagnostic(decoder);

                            LOOP2_ASSERT(VALUE1, value1, VALUE1 == value1);
                            LOOP2_ASSERT(VALUE2, value2, VALUE2 == value2);
                            LOOP2_ASSERT(VALUE3, value3, VALUE3 == value3);
                            if (veryVerbose) {
                                P(VALUE1); P(value1);
                                P(VALUE2); P(value2);
                                P(VALUE3); P(value3);
                            }
                        }
                    }
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting Datetime Brute force."
                               << bsl::endl;
        {
            const int YEARS[] = { 1, 4, 96, 100, 400, 500, 800, 1000, 1600,
                                  1700, 1751, 1752, 1753, 1930, 2000, 2010,
                                  2012, 2019, 2020, 2021, 6478, 6479, 6480,
                                  9998, 9999 };
            const int NUM_YEARS = sizeof YEARS / sizeof *YEARS;

            const int MONTHS[] = { 1, 2, 5, 8, 9, 12 };
            const int NUM_MONTHS = sizeof MONTHS / sizeof *MONTHS;

            const int DAYS[] = { 1, 2, 5, 10, 15, 20, 28, 29, 30, 31 };
            const int NUM_DAYS = sizeof DAYS / sizeof *DAYS;

            for (int di = 0; di < NUM_YEARS; ++di) {
            for (int dj = 0; dj < NUM_MONTHS; ++dj) {
            for (int dk = 0; dk < NUM_DAYS; ++dk) {

                const int YEAR  = YEARS[di];
                const int MONTH = MONTHS[dj];
                const int DAY   = DAYS[dk];

                if (bdlt::Date::isValidYearMonthDay(YEAR, MONTH, DAY)
                 && bdlt::SerialDateImpUtil::isValidYearMonthDay(YEAR,
                                                                 MONTH,
                                                                 DAY)) {

                    const int HOURS[] = { 0, 12, 23 };
                    const int NUM_HOURS = sizeof HOURS / sizeof *HOURS;

                    const int MINS[] = { 0, 30, 59 };
                    const int NUM_MINS = sizeof MINS / sizeof *MINS;

                    const int SECONDS[] = { 0, 30, 59 };
                    const int NUM_SECS = sizeof SECONDS / sizeof *SECONDS;

                    for (int ti = 0; ti < NUM_HOURS; ++ti) {
                    for (int tj = 0; tj < NUM_MINS; ++tj) {
                    for (int tk = 0; tk < NUM_SECS; ++tk) {

                        const int HOUR = HOURS[ti];
                        const int MIN  = MINS[tj];
                        const int SECS = SECONDS[tk];

                        if (veryVerbose) { P_(YEAR) P_(MONTH) P(DAY) }
                        if (veryVerbose) { P_(HOUR) P_(MIN) P(SECS) }

                        const int MS = 0;
                        const bdlt::Date DATE(YEAR, MONTH, DAY);
                        const bdlt::Time TIME(HOUR, MIN, SECS, MS);
                        const bdlt::Datetime VALUE(DATE, TIME);
                        bdlt::Datetime value;
                        const int MS1 = 0, MS2 = 500, MS3 = 999;
                        const int OFF1 = 0, OFF2 =-1439, OFF3 =1439;
                        const bdlt::Date DATE1(YEAR, MONTH, DAY);
                        const bdlt::Time TIME1(HOUR, MIN, SECS, MS1);

                        const bdlt::Date DATE2(YEAR, MONTH, DAY);
                        const bdlt::Time TIME2(HOUR, MIN, SECS, MS2);

                        const bdlt::Date DATE3(YEAR, MONTH, DAY);
                        const bdlt::Time TIME3(HOUR, MIN, SECS, MS3);

                        const bdlt::Datetime DT1(DATE1, TIME1);
                        const bdlt::Datetime DT2(DATE2, TIME2);
                        const bdlt::Datetime DT3(DATE3, TIME3);

                        const bdlt::DatetimeTz VALUE1(DT1, OFF1);
                        const bdlt::DatetimeTz VALUE2(DT2, OFF2);
                        const bdlt::DatetimeTz VALUE3(DT3, OFF3);

                        bdlt::DatetimeTz value1, value2, value3;

                        test::BasicRecord VALUE4;
                        VALUE4.i1() = 100;
                        VALUE4.i2() = 200;
                        VALUE4.dt() = VALUE1;
                        VALUE4.s()  = "Hello World";

                        test::BasicRecord VALUE5(VALUE4), VALUE6(VALUE4);
                        VALUE5.dt() = VALUE2;
                        VALUE6.dt() = VALUE3;

                        test::BasicRecord value4, value5, value6;

                        {
                            bdlsb::MemOutStreamBuf osb;
                            balber::BerEncoder encoder(&DEFOPTS);
                            ASSERT(0 == encoder.encode(&osb, VALUE));

                            if (veryVerbose) {
                                P(osb.length());
                                printBuffer(osb.data(), osb.length());
                            }

                            bdlsb::FixedMemInStreamBuf isb(osb.data(),
                                                          osb.length());
                            ASSERT(0 == decoder.decode(&isb, &value));
                            printDiagnostic(decoder);

                            LOOP2_ASSERT(VALUE, value, VALUE == value);
                            if (veryVerbose) {
                                P(VALUE);
                                P(value);
                            }
                        }

                        {
                            bdlsb::MemOutStreamBuf osb1, osb2, osb3;
                            balber::BerEncoder encoder(&DEFOPTS);
                            ASSERT(0 == encoder.encode(&osb1, VALUE1));
                            ASSERT(0 == encoder.encode(&osb2, VALUE2));
                            ASSERT(0 == encoder.encode(&osb3, VALUE3));

                            if (veryVerbose) {
                                P(osb1.length());
                                P(osb2.length());
                                P(osb3.length());
                                printBuffer(osb1.data(), osb1.length());
                                printBuffer(osb2.data(), osb2.length());
                                printBuffer(osb3.data(), osb3.length());
                            }

                            bdlsb::FixedMemInStreamBuf isb1(osb1.data(),
                                                           osb1.length());
                            bdlsb::FixedMemInStreamBuf isb2(osb2.data(),
                                                           osb2.length());
                            bdlsb::FixedMemInStreamBuf isb3(osb3.data(),
                                                           osb3.length());

                            ASSERT(0 == decoder.decode(&isb1, &value1));
                            ASSERT(0 == decoder.decode(&isb2, &value2));
                            ASSERT(0 == decoder.decode(&isb3, &value3));
                            printDiagnostic(decoder);

                            LOOP2_ASSERT(VALUE1, value1, VALUE1 == value1);
                            LOOP2_ASSERT(VALUE2, value2, VALUE2 == value2);
                            LOOP2_ASSERT(VALUE3, value3, VALUE3 == value3);
                            if (veryVerbose) {
                                P(VALUE1); P(value1);
                                P(VALUE2); P(value2);
                                P(VALUE3); P(value3);
                            }
                        }

                        {
                            bdlsb::MemOutStreamBuf osb1, osb2, osb3;
                            balber::BerEncoder encoder(&DEFOPTS);
                            ASSERT(0 == encoder.encode(&osb1, VALUE4));
                            ASSERT(0 == encoder.encode(&osb2, VALUE5));
                            ASSERT(0 == encoder.encode(&osb3, VALUE6));

                            if (veryVerbose) {
                                P(osb1.length());
                                P(osb2.length());
                                P(osb3.length());
                                printBuffer(osb1.data(), osb1.length());
                                printBuffer(osb2.data(), osb2.length());
                                printBuffer(osb3.data(), osb3.length());
                            }

                            bdlsb::FixedMemInStreamBuf isb1(osb1.data(),
                                                           osb1.length());
                            bdlsb::FixedMemInStreamBuf isb2(osb2.data(),
                                                           osb2.length());
                            bdlsb::FixedMemInStreamBuf isb3(osb3.data(),
                                                           osb3.length());

                            ASSERT(0 == decoder.decode(&isb1, &value4));
                            ASSERT(0 == decoder.decode(&isb2, &value5));
                            ASSERT(0 == decoder.decode(&isb3, &value6));
                            printDiagnostic(decoder);

                            LOOP2_ASSERT(VALUE4, value4, VALUE4 == value4);
                            LOOP2_ASSERT(VALUE5, value5, VALUE5 == value5);
                            LOOP2_ASSERT(VALUE6, value6, VALUE6 == value6);
                            if (veryVerbose) {
                                P(VALUE4); P(value4);
                                P(VALUE5); P(value5);
                                P(VALUE6); P(value6);
                            }
                        }

                        {
                            bdlsb::MemOutStreamBuf osb;
                            balber::BerEncoder encoder(&options);
                            ASSERT(0 == encoder.encode(&osb, VALUE));

                            if (veryVerbose) {
                                P(osb.length());
                                printBuffer(osb.data(), osb.length());
                            }

                            bdlsb::FixedMemInStreamBuf isb(osb.data(),
                                                          osb.length());
                            ASSERT(0 == decoder.decode(&isb, &value));
                            printDiagnostic(decoder);

                            LOOP2_ASSERT(VALUE, value, VALUE == value);
                            if (veryVerbose) {
                                P(VALUE);
                                P(value);
                            }
                        }

                        {
                            bdlsb::MemOutStreamBuf osb1, osb2, osb3;
                            balber::BerEncoder encoder(&options);
                            ASSERT(0 == encoder.encode(&osb1, VALUE1));
                            ASSERT(0 == encoder.encode(&osb2, VALUE2));
                            ASSERT(0 == encoder.encode(&osb3, VALUE3));

                            if (veryVerbose) {
                                P(osb1.length());
                                P(osb2.length());
                                P(osb3.length());
                                printBuffer(osb1.data(), osb1.length());
                                printBuffer(osb2.data(), osb2.length());
                                printBuffer(osb3.data(), osb3.length());
                            }

                            bdlsb::FixedMemInStreamBuf isb1(osb1.data(),
                                                           osb1.length());
                            bdlsb::FixedMemInStreamBuf isb2(osb2.data(),
                                                           osb2.length());
                            bdlsb::FixedMemInStreamBuf isb3(osb3.data(),
                                                           osb3.length());

                            ASSERT(0 == decoder.decode(&isb1, &value1));
                            ASSERT(0 == decoder.decode(&isb2, &value2));
                            ASSERT(0 == decoder.decode(&isb3, &value3));
                            printDiagnostic(decoder);

                            LOOP2_ASSERT(VALUE1, value1, VALUE1 == value1);
                            LOOP2_ASSERT(VALUE2, value2, VALUE2 == value2);
                            LOOP2_ASSERT(VALUE3, value3, VALUE3 == value3);
                            if (veryVerbose) {
                                P(VALUE1); P(value1);
                                P(VALUE2); P(value2);
                                P(VALUE3); P(value3);
                            }
                        }

                        {
                            bdlsb::MemOutStreamBuf osb1, osb2, osb3;
                            balber::BerEncoder encoder(&options);
                            ASSERT(0 == encoder.encode(&osb1, VALUE4));
                            ASSERT(0 == encoder.encode(&osb2, VALUE5));
                            ASSERT(0 == encoder.encode(&osb3, VALUE6));

                            if (veryVerbose) {
                                P(osb1.length());
                                P(osb2.length());
                                P(osb3.length());
                                printBuffer(osb1.data(), osb1.length());
                                printBuffer(osb2.data(), osb2.length());
                                printBuffer(osb3.data(), osb3.length());
                            }

                            bdlsb::FixedMemInStreamBuf isb1(osb1.data(),
                                                           osb1.length());
                            bdlsb::FixedMemInStreamBuf isb2(osb2.data(),
                                                           osb2.length());
                            bdlsb::FixedMemInStreamBuf isb3(osb3.data(),
                                                           osb3.length());

                            ASSERT(0 == decoder.decode(&isb1, &value4));
                            ASSERT(0 == decoder.decode(&isb2, &value5));
                            ASSERT(0 == decoder.decode(&isb3, &value6));
                            printDiagnostic(decoder);

                            LOOP2_ASSERT(VALUE4, value4, VALUE4 == value4);
                            LOOP2_ASSERT(VALUE5, value5, VALUE5 == value5);
                            LOOP2_ASSERT(VALUE6, value6, VALUE6 == value6);
                            if (veryVerbose) {
                                P(VALUE4); P(value4);
                                P(VALUE5); P(value5);
                                P(VALUE6); P(value6);
                            }
                        }
                    }
                    }
                    }
                }
            }
            }
            }
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING encoding & decoding for date/time components
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING encoding & decoding for date/time"
                               << "\n========================================="
                               << bsl::endl;

        balber::BerEncoderOptions options;
        options.setEncodeDateAndTimeTypesAsBinary(true);
        const balber::BerEncoderOptions DEFOPTS;

        if (verbose) bsl::cout << "\nDefine data" << bsl::endl;

        static const struct {
            int d_lineNum;   // source line number
            int d_year;      // year under test
            int d_month;     // month under test
            int d_day;       // day under test
            int d_hour;      // hour under test
            int d_minutes;   // minutes under test
            int d_seconds;   // seconds under test
            int d_milliSecs; // milli seconds under test
            int d_tzoffset;  // time zone offset
        } DATA[] = {
   //line no.  year   month   day   hour    min   sec    ms  offset
   //-------   -----  -----   ---   ----    ---   ---    --  ------
    {      L_,      1,     1,    1,     0,     0,    0,    0,      0     },
    {      L_,      1,     1,    1,     0,     0,    0,    0,     45     },
    {      L_,      1,     1,    1,     0,     0,    0,    0,  -1439     },

    {      L_,      1,     1,    1,     1,     1,    1,    1,      0     },
    {      L_,      1,     1,    1,     1,     1,    1,    1,    500     },
    {      L_,      1,     1,    1,     0,     0,    0,    0,  -1439     },

    {      L_,      1,     1,    1,     1,    23,   59,   59,      0     },
    {      L_,      1,     1,    1,     1,    23,   59,   59,   1439     },
    {      L_,      1,     1,    1,     1,    23,   59,   59,  -1439     },

    {      L_,      1,     1,    2,     0,     0,    0,    0,      0     },
    {      L_,      1,     1,    2,     0,     0,    0,    0,   1439     },
    {      L_,      1,     1,    2,     0,     0,    0,    0,  -1439     },

    {      L_,      1,     1,    2,     1,     1,    1,    1,      0     },
    {      L_,      1,     1,    2,     1,     1,    1,    1,    500     },

    {      L_,      1,     1,    2,     1,    23,   59,   59,      0     },
    {      L_,      1,     1,    2,     1,    23,   59,   59,    500     },
    {      L_,      1,     1,    2,     1,    23,   59,   59,   -500     },

    {      L_,      1,     1,   10,     0,     0,    0,    0,      0     },
    {      L_,      1,     1,   10,     1,     1,    1,    1,     99     },

    {      L_,      1,     1,   30,     0,     0,    0,    0,      0     },
    {      L_,      1,     1,   31,     0,     0,    0,    0,   1439     },
    {      L_,      1,     1,   31,     0,     0,    0,    0,  -1439     },

    {      L_,      1,     2,    1,     0,     0,    0,    0,      0     },
    {      L_,      1,     2,    1,    23,    59,   59,    0,   1439     },

    {      L_,      1,    12,   31,     0,     0,    0,    0,      0     },
    {      L_,      1,    12,   31,    23,    59,   59,    0,   1439     },

    {      L_,      2,     1,    1,     0,     0,    0,    0,      0     },
    {      L_,      2,     1,    1,    23,    59,   59,    0,   1439     },

    {      L_,      4,     1,    1,     0,     0,    0,    0,      0     },
    {      L_,      4,     1,    1,    23,    59,   59,    0,   1439     },

    {      L_,      4,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,      4,     2,   28,    23,    59,   59,    0,   1439     },
    {      L_,      4,     2,   28,    23,    59,   59,    0,  -1439     },

    {      L_,      4,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,      4,     2,   29,    23,    59,   59,    0,   1439     },
    {      L_,      4,     2,   29,    23,    59,   59,    0,  -1439     },

    {      L_,      4,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,      4,     3,    1,    23,    59,   59,    0,   1439     },
    {      L_,      4,     3,    1,    23,    59,   59,    0,  -1439     },

    {      L_,      8,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,      8,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,      8,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,      8,     2,   29,    23,    59,   59,    0,   1439     },

    {      L_,      8,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,      8,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,    100,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,    100,     2,   28,    23,    59,   59,    0,   1439     },
    {      L_,    100,     2,   28,    23,    59,   59,    0,  -1439     },

    {      L_,    100,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,    100,     3,    1,    23,    59,   59,    0,   1439     },
    {      L_,    100,     3,    1,    23,    59,   59,    0,  -1439     },

    {      L_,    400,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,    400,     2,   28,    23,    59,   59,    0,   1439     },
    {      L_,    400,     2,   28,    23,    59,   59,    0,  -1439     },

    {      L_,    400,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,    400,     2,   29,    23,    59,   59,    0,   1439     },
    {      L_,    400,     2,   29,    23,    59,   59,    0,  -1439     },

    {      L_,    400,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,    400,     3,    1,    23,    59,   59,    0,   1439     },
    {      L_,    400,     3,    1,    23,    59,   59,    0,  -1439     },

    {      L_,    500,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,    500,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,    500,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,    500,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,    800,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,    800,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,    800,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,    800,     2,   29,    23,    59,   59,    0,   1439     },

    {      L_,    800,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,    800,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,   1000,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,   1000,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,   1000,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,   1000,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,   2000,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,   2000,     2,   28,    23,    59,   59,    0,   1439     },

    {      L_,   2000,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,   2000,     2,   29,    23,    59,   59,    0,   1439     },

    {      L_,   2000,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,   2000,     3,    1,    23,    59,   59,    0,   1439     },

    {      L_,   2016,    12,   31,     0,     0,    0,    0,      0     },
    {      L_,   2017,    12,   31,     0,     0,    0,    0,      0     },
    {      L_,   2018,    12,   31,     0,     0,    0,    0,      0     },
    {      L_,   2019,    12,   31,     0,     0,    0,    0,      0     },

    {      L_,   2020,     1,    1,     0,     0,    0,    0,      0     },
    {      L_,   2020,     1,    1,     0,     0,    0,    0,   1439     },
    {      L_,   2020,     1,    1,     0,     0,    0,    0,  -1439     },

    {      L_,   2020,     1,    1,    23,    59,   59,  999,      0     },
    {      L_,   2020,     1,    1,    23,    59,   59,  999,   1439     },
    {      L_,   2020,     1,    1,    23,    59,   59,  999,  -1439     },

    {      L_,   2020,     1,    2,     0,     0,    0,    0,      0     },
    {      L_,   2020,     1,    2,     0,     0,    0,    0,   1439     },
    {      L_,   2020,     1,    2,     0,     0,    0,    0,  -1439     },

    {      L_,   2020,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,   2020,     2,   28,    23,    59,   59,    0,   1439     },
    {      L_,   2020,     2,   28,    23,    59,   59,    0,  -1439     },

    {      L_,   2020,     2,   29,     0,     0,    0,    0,      0     },
    {      L_,   2020,     2,   29,    23,    59,   59,    0,   1439     },
    {      L_,   2020,     2,   29,    23,    59,   59,    0,  -1439     },

    {      L_,   2020,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,   2020,     3,    1,    23,    59,   59,    0,   1439     },
    {      L_,   2020,     3,    1,    23,    59,   59,    0,  -1439     },

    {      L_,   2021,     1,    2,     0,     0,    0,    0,      0     },
    {      L_,   2022,     1,    2,     0,     0,    0,    0,      0     },

    {      L_,   9999,     2,   28,     0,     0,    0,    0,      0     },
    {      L_,   9999,     2,   28,    23,    59,   59,    0,   1439     },
    {      L_,   9999,     2,   28,    23,    59,   59,    0,  -1439     },

    {      L_,   9999,     3,    1,     0,     0,    0,    0,      0     },
    {      L_,   9999,     3,    1,    23,    59,   59,    0,   1439     },
    {      L_,   9999,     3,    1,    23,    59,   59,    0,  -1439     },

    {      L_,   9999,    12,   30,     0,     0,    0,    0,      0     },
    {      L_,   9999,    12,   30,    23,    59,   59,    0,   1439     },

    {      L_,   9999,    12,   31,     0,     0,    0,    0,      0     },
    {      L_,   9999,    12,   31,    23,    59,   59,    0,   1439     },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) bsl::cout << "\nTesting 'bdlt::Date'." << bsl::endl;
        {
            typedef bdlt::Date Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int Y    = DATA[i].d_year;
                const int M    = DATA[i].d_month;
                const int D    = DATA[i].d_day;

                if (veryVerbose) { P_(Y) P_(M) P(D) }

                const Type VALUE(Y, M, D); Type value;

                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder encoder(&options);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value));
                    printDiagnostic(decoder);

                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                    if (veryVerbose) {
                        P(VALUE);
                        P(value);
                    }
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder encoder(&DEFOPTS);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value));
                    printDiagnostic(decoder);

                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                    if (veryVerbose) {
                        P(VALUE);
                        P(value);
                    }
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting 'bdlt::DateTz'." << bsl::endl;
        {
            typedef bdlt::DateTz Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int Y    = DATA[i].d_year;
                const int M    = DATA[i].d_month;
                const int D    = DATA[i].d_day;
                const int OFF  = DATA[i].d_tzoffset;

                const Type VALUE(bdlt::Date(Y, M, D), OFF); Type value;

                if (veryVerbose) { P_(Y) P_(M) P_(D) P(OFF) }

                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder encoder(&options);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value));
                    printDiagnostic(decoder);

                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                    if (veryVerbose) {
                        P(VALUE);
                        P(value);
                    }
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder encoder(&DEFOPTS);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value));
                    printDiagnostic(decoder);

                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                    if (veryVerbose) {
                        P(VALUE);
                        P(value);
                    }
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting 'bdlt::Time'." << bsl::endl;
        {
            typedef bdlt::Time Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int H    = DATA[i].d_hour;
                const int MM   = DATA[i].d_minutes;
                const int S    = DATA[i].d_seconds;
                const int MS   = DATA[i].d_milliSecs;

                if (veryVerbose) { P_(H) P_(MM) P_(S) P(MS) }

                const Type VALUE(H, MM, S, MS); Type value;
                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder encoder(&options);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value));
                    printDiagnostic(decoder);

                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                    if (veryVerbose) {
                        P(VALUE);
                        P(value);
                    }
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder encoder(&DEFOPTS);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value));
                    printDiagnostic(decoder);

                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                    if (veryVerbose) {
                        P(VALUE);
                        P(value);
                    }
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting 'bdlt::TimeTz'." << bsl::endl;
        {
            typedef bdlt::TimeTz Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int H    = DATA[i].d_hour;
                const int MM   = DATA[i].d_minutes;
                const int S    = DATA[i].d_seconds;
                const int MS   = DATA[i].d_milliSecs;
                const int OFF  = DATA[i].d_tzoffset;

                const Type VALUE(bdlt::Time(H, MM, S, MS), OFF); Type value;

                if (veryVerbose) { P_(H) P_(MM) P_(S) P_(MS) P(OFF) }

                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder encoder(&options);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value));
                    printDiagnostic(decoder);

                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                    if (veryVerbose) {
                        P(VALUE);
                        P(value);
                    }
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder encoder(&DEFOPTS);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value));
                    printDiagnostic(decoder);

                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                    if (veryVerbose) {
                        P(VALUE);
                        P(value);
                    }
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting 'bdlt::Datetime'." << bsl::endl;
        {
            typedef bdlt::Datetime Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int Y    = DATA[i].d_year;
                const int M    = DATA[i].d_month;
                const int D    = DATA[i].d_day;
                const int H    = DATA[i].d_hour;
                const int MM   = DATA[i].d_minutes;
                const int S    = DATA[i].d_seconds;
                const int MS   = DATA[i].d_milliSecs;

                const Type VALUE(Y, M, D, H, MM, S, MS); Type value;

                if (veryVerbose) { P_(Y) P_(M) P_(D) P_(H)
                                   P_(MM) P_(S) P(MS) }

                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder encoder(&options);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value));
                    printDiagnostic(decoder);

                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                    if (veryVerbose) {
                        P(VALUE);
                        P(value);
                    }
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder encoder(&DEFOPTS);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value));
                    printDiagnostic(decoder);

                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                    if (veryVerbose) {
                        P(VALUE);
                        P(value);
                    }
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting 'bdlt::DatetimeTz'." << bsl::endl;
        {
            typedef bdlt::DatetimeTz Type;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const int Y    = DATA[i].d_year;
                const int M    = DATA[i].d_month;
                const int D    = DATA[i].d_day;
                const int H    = DATA[i].d_hour;
                const int MM   = DATA[i].d_minutes;
                const int S    = DATA[i].d_seconds;
                const int MS   = DATA[i].d_milliSecs;
                const int OFF  = DATA[i].d_tzoffset;

                const Type VALUE(bdlt::Datetime(Y, M, D, H, MM, S, MS), OFF);
                Type value;

                if (veryVerbose) { P_(Y) P_(M) P_(D) P_(H)
                                   P_(MM) P_(S) P_(MS) P(OFF) }

                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder encoder(&options);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value));
                    printDiagnostic(decoder);

                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                    if (veryVerbose) {
                        P(VALUE);
                        P(value);
                    }
                }

                {
                    bdlsb::MemOutStreamBuf osb;
                    balber::BerEncoder encoder(&DEFOPTS);
                    ASSERT(0 == encoder.encode(&osb, VALUE));

                    if (veryVerbose) {
                        P(osb.length());
                        printBuffer(osb.data(), osb.length());
                    }

                    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                    ASSERT(0 == decoder.decode(&isb, &value));
                    printDiagnostic(decoder);

                    LOOP3_ASSERT(LINE, VALUE, value, VALUE == value);
                    if (veryVerbose) {
                        P(VALUE);
                        P(value);
                    }
                }
            }
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING functions related to skipped elements
        //   This test exercises functions that apply to skipped elements.
        //
        // Concerns:
        //   a. The setNumUnknownElementsSkipped sets the number of skipped
        //      elements correctly.
        //   b. The numUnknownElementsSkipped returns the number of skipped
        //      elements correctly.
        //
        // Plan:
        //
        // Testing:
        //   void setNumUnknownElementsSKipped(int value);
        //   int numUnknownElementsSkipped() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nFUNCTIONS related to skipped elements"
                          << "\n=====================================" << endl;

        if (verbose) cout << "\nTesting setting and getting num skipped elems."
                          << endl;
        {
            balber::BerDecoder mX; const balber::BerDecoder& X = mX;
            ASSERT(0 == X.numUnknownElementsSkipped());

            const int DATA[] = { 0, 1, 5, 100, 2000 };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int i = 0; i < NUM_DATA; ++i) {
                const int NUM_SKIPPED_ELEMS = DATA[i];
                mX.setNumUnknownElementsSkipped(NUM_SKIPPED_ELEMS);
                LOOP3_ASSERT(i, NUM_SKIPPED_ELEMS,
                           X.numUnknownElementsSkipped(),
                           NUM_SKIPPED_ELEMS == X.numUnknownElementsSkipped());
            }
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING SEQUENCE DECODED AS CHOICE
        //
        // Concerns:
        //   Decoder needs to be able to handle decoding a sequence as a
        //   choice without aborting, should simply log error messages.
        // --------------------------------------------------------------------

        {
            if (verbose) bsl::cout << "\nTesting Sequence Decoded as Choice\n"
                                        "==================================\n";

            bdlsb::MemOutStreamBuf osb;

            test::MySequence valueOut;
            valueOut.attribute1() = 34;
            valueOut.attribute2() = "Hello";

            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            test::MyChoice valueIn;

            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 != decoder.decode(&isb, &valueIn));
            ASSERT(decoder.loggedMessages().length() > 0);
            if (veryVerbose) {
                bsl::cout << "We expect the decoder to complain.  Here is\n"
                             "what it says:\n";
                printDiagnostic(decoder);
            }
        }

        {
            if (verbose) bsl::cout <<
                                "\nTesting Empty Sequence Decoded as Choice\n"
                                  "========================================\n";

            bdlsb::MemOutStreamBuf osb;

            test::MySequence valueOut;

            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            test::MyChoice valueIn;

            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 != decoder.decode(&isb, &valueIn));
            ASSERT(decoder.loggedMessages().length() > 0);
            if (veryVerbose) {
                bsl::cout << "We expect the decoder to complain.  Here is\n"
                             "what it says:\n";
                printDiagnostic(decoder);
            }
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING CHOICE DECODED AS SEQUENCE
        //
        // Concerns:
        //   Decoder needs to be able to handle decoding a choice as a
        //   sequence without aborting, should simply log error messages.
        // --------------------------------------------------------------------

        {
            if (verbose) bsl::cout << "\nTesting Choice Decoded as Sequence\n"
                                        "==================================\n";

            bdlsb::MemOutStreamBuf osb;

            test::MyChoice valueOut;
            valueOut.makeSelection1();
            valueOut.selection1() = 34;

            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            test::MySequence valueIn;

            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 != decoder.decode(&isb, &valueIn));
            ASSERT(decoder.loggedMessages().length() > 0);
            if (veryVerbose) {
                bsl::cout << "We expect the decoder to complain.  Here is\n"
                             "what it says:\n";
                printDiagnostic(decoder);
            }
        }

        {
            if (verbose) bsl::cout <<
                                 "\nTesting Null Choice Decoded as Sequence\n"
                                   "=======================================\n";

            bdlsb::MemOutStreamBuf osb;

            test::MyChoice valueOut;

            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            test::MySequence valueIn;

            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 != decoder.decode(&isb, &valueIn));
            ASSERT(decoder.loggedMessages().length() > 0);
            if (veryVerbose) {
                bsl::cout << "We expect the decoder to complain.  Here is\n"
                             "what it says:\n";
                printDiagnostic(decoder);
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING NILLABLE VALUES
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Nillable Values"
                               << "\n=======================" << bsl::endl;

        if (verbose) bsl::cout << "\nTesting with null value." << bsl::endl;
        {
            bdlsb::MemOutStreamBuf osb;

            test::MySequenceWithNillable valueOut;
            valueOut.attribute1() = 34;
            valueOut.attribute2() = "Hello";

            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            test::MySequenceWithNillable valueIn;
            valueIn.myNillable() = "Hello";

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);

            if (veryVerbose) bsl::cout << "\tDecoding from Nokalva "
                                          "(DEFINITE LENGTH)."
                                       << bsl::endl;
            {
                const char NOKALVA_DATA[] = "300C8001 22A10082 0548656C 6C6F";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::MySequenceWithNillable valueFromNokalva;
                valueFromNokalva.myNillable() = "Hello";

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                              nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }

            if (veryVerbose) bsl::cout << "\tDecoding from Nokalva "
                                          "(INDEFINITE LENGTH)."
                                       << bsl::endl;
            {
                const char NOKALVA_DATA[] = "30808001 22A18000 00820548 "
                                            "656C6C6F 0000";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::MySequenceWithNillable valueFromNokalva;
                valueFromNokalva.myNillable() = "Hello";

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                              nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }
        }

        if (verbose) bsl::cout << "\nTesting with non-null value."
                               << bsl::endl;
        {
            bdlsb::MemOutStreamBuf osb;

            test::MySequenceWithNillable valueOut;
            valueOut.attribute1() = 34;
            valueOut.myNillable() = "World!";
            valueOut.attribute2() = "Hello";

            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            test::MySequenceWithNillable valueIn;

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);

            if (veryVerbose) bsl::cout << "\tDecoding from Nokalva "
                                          "(DEFINITE LENGTH)."
                                       << bsl::endl;
            {
                const char NOKALVA_DATA[] = "30148001 22A10880 06576F72 "
                                            "6C642182 0548656C 6C6F";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::MySequenceWithNillable valueFromNokalva;

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                              nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }

            if (veryVerbose) bsl::cout << "\tDecoding from Nokalva "
                                          "(INDEFINITE LENGTH)."
                                       << bsl::endl;
            {
                const char NOKALVA_DATA[] = "30808001 22A18080 06576F72 "
                                            "6C642100 00820548 656C6C6F 0000";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::MySequenceWithNillable valueFromNokalva;

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                              nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }
        }

        if (verbose) bsl::cout << "\nEnd of test." << bsl::endl;
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING ANONYMOUS CHOICES
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Anonymous Choices"
                               << "\n=========================" << bsl::endl;

        if (verbose) bsl::cout << "\nTesting with no selection." << bsl::endl;
        {
            bdlsb::MemOutStreamBuf osb;

            test::MySequenceWithAnonymousChoice valueOut;
            valueOut.attribute1() = 34;
            valueOut.attribute2() = "Hello";

            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            test::MySequenceWithAnonymousChoice valueIn;
            valueIn.choice().makeMyChoice2("Hello");

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);
        }

        if (verbose) bsl::cout << "\nTesting with selection." << bsl::endl;
        {
            bdlsb::MemOutStreamBuf osb;

            test::MySequenceWithAnonymousChoice valueOut;
            valueOut.attribute1() = 34;
            valueOut.choice().makeMyChoice1(65);
            valueOut.attribute2() = "Hello";

            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            test::MySequenceWithAnonymousChoice valueIn;

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);

            if (veryVerbose) bsl::cout << "\tDecoding from Nokalva "
                                          "(DEFINITE LENGTH)."
                                       << bsl::endl;
            {
                const char NOKALVA_DATA[] = "300F8001 22A10380 01418205 "
                                            "48656C6C 6F";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::MySequenceWithAnonymousChoice valueFromNokalva;

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                              nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }

            if (veryVerbose) bsl::cout << "\tDecoding from Nokalva "
                                          "(INDEFINITE LENGTH)."
                                       << bsl::endl;
            {
                const char NOKALVA_DATA[] = "30808001 22A18080 01410000 "
                                            "82054865 6C6C6F00 00";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::MySequenceWithAnonymousChoice valueFromNokalva;

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                              nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }
        }
        {
            bdlsb::MemOutStreamBuf osb;

            test::MySequenceWithAnonymousChoice valueOut;
            valueOut.attribute1() = 34;
            valueOut.choice().makeMyChoice2("World!");
            valueOut.attribute2() = "Hello";

            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            test::MySequenceWithAnonymousChoice valueIn;

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);

            if (veryVerbose) bsl::cout << "\tDecoding from Nokalva "
                                          "(DEFINITE LENGTH)."
                                       << bsl::endl;
            {
                const char NOKALVA_DATA[] = "30148001 22A10881 06576F72 "
                                            "6C642182 0548656C 6C6F";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::MySequenceWithAnonymousChoice valueFromNokalva;

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                              nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }

            if (veryVerbose) bsl::cout << "\tDecoding from Nokalva "
                                          "(INDEFINITE LENGTH)."
                                       << bsl::endl;
            {
                const char NOKALVA_DATA[] = "30808001 22A18081 06576F72 "
                                            "6C642100 00820548 656C6C6F 0000";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::MySequenceWithAnonymousChoice valueFromNokalva;

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                              nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }
        }

        if (verbose) bsl::cout << "\nEnd of test." << bsl::endl;
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ARRAYS
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Arrays"
                               << "\n==============" << bsl::endl;

        if (verbose) bsl::cout << "\nTesting with empty array." << bsl::endl;
        {
            bdlsb::MemOutStreamBuf osb;

            test::MySequenceWithArray valueOut;
            valueOut.attribute1() = 34;

            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            test::MySequenceWithArray valueIn;
            valueIn.attribute2().push_back("This");
            valueIn.attribute2().push_back("is");
            valueIn.attribute2().push_back("a");
            valueIn.attribute2().push_back("test.");

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);

            if (veryVerbose) bsl::cout << "\tDecoding from Nokalva "
                                          "(DEFINITE LENGTH)."
                                       << bsl::endl;
            {
                const char NOKALVA_DATA[] = "30058001 22A100";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::MySequenceWithArray valueFromNokalva;
                valueFromNokalva.attribute2().push_back("This");
                valueFromNokalva.attribute2().push_back("is");
                valueFromNokalva.attribute2().push_back("a");
                valueFromNokalva.attribute2().push_back("test.");

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                              nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }

            if (veryVerbose) bsl::cout << "\tDecoding from Nokalva "
                                          "(INDEFINITE LENGTH)."
                                       << bsl::endl;
            {
                const char NOKALVA_DATA[] = "30808001 22A18000 000000";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::MySequenceWithArray valueFromNokalva;
                valueFromNokalva.attribute2().push_back("This");
                valueFromNokalva.attribute2().push_back("is");
                valueFromNokalva.attribute2().push_back("a");
                valueFromNokalva.attribute2().push_back("test.");

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                              nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }
        }

        if (verbose) bsl::cout << "\nTesting with empty array and"
                               << " 'encodeEmptyArrays' option set to 'false'"
                               << bsl::endl;
        {
            balber::BerEncoderOptions options;
            options.setEncodeEmptyArrays(false);

            bdlsb::MemOutStreamBuf osb;

            test::MySequenceWithArray valueOut;
            valueOut.attribute1() = 34;

            balber::BerEncoder encoder(&options);
            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            test::MySequenceWithArray valueIn;
            valueIn.attribute2().push_back("This");
            valueIn.attribute2().push_back("is");
            valueIn.attribute2().push_back("a");
            valueIn.attribute2().push_back("test.");

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);
        }

        if (verbose) bsl::cout << "\nTesting with non-empty array."
                               << bsl::endl;
        {
            bdlsb::MemOutStreamBuf osb;

            test::MySequenceWithArray valueOut;
            valueOut.attribute1() = 34;
            valueOut.attribute2().push_back("Hello");
            valueOut.attribute2().push_back("World!");

            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            test::MySequenceWithArray valueIn;

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);

            if (veryVerbose) bsl::cout << "\tDecoding from Nokalva "
                                          "(DEFINITE LENGTH)."
                                       << bsl::endl;
            {
                const char NOKALVA_DATA[] = "30148001 22A10F0C 0548656C "
                                            "6C6F0C06 576F726C 6421";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::MySequenceWithArray valueFromNokalva;

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                              nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }

            if (veryVerbose) bsl::cout << "\tDecoding from Nokalva "
                                          "(INDEFINITE LENGTH)."
                                       << bsl::endl;
            {
                const char NOKALVA_DATA[] = "30808001 22A1800C 0548656C "
                                            "6C6F0C06 576F726C 64210000 0000";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::MySequenceWithArray valueFromNokalva;

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                              nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }
        }

        if (verbose) bsl::cout << "\nEnd of test." << bsl::endl;
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING NULLABLE VALUES
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Nullable Values"
                               << "\n=======================" << bsl::endl;

        if (verbose) bsl::cout << "\nTesting with null value." << bsl::endl;
        {
            bdlsb::MemOutStreamBuf osb;

            test::MySequenceWithNullable valueOut;
            valueOut.attribute1() = 34;

            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            test::MySequenceWithNullable valueIn;
            valueIn.attribute1() = 356;
            valueIn.attribute2().makeValue("Hello");

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);

            if (veryVerbose) bsl::cout << "\tDecoding from Nokalva "
                                          "(DEFINITE LENGTH)."
                                       << bsl::endl;
            {
                const char NOKALVA_DATA[] = "30038001 22";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::MySequenceWithNullable valueFromNokalva;
                valueFromNokalva.attribute1() = 356;
                valueFromNokalva.attribute2().makeValue("Hello");

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                              nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }

            if (veryVerbose) bsl::cout << "\tDecoding from Nokalva "
                                          "(INDEFINITE LENGTH)."
                                       << bsl::endl;
            {
                const char NOKALVA_DATA[] = "30808001 220000";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::MySequenceWithNullable valueFromNokalva;
                valueFromNokalva.attribute1() = 356;
                valueFromNokalva.attribute2().makeValue("Hello");

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                              nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }
        }

        if (verbose) bsl::cout << "\nTesting with non-null value."
                               << bsl::endl;
        {
            bdlsb::MemOutStreamBuf osb;

            test::MySequenceWithNullable valueOut;
            valueOut.attribute1() = 34;
            valueOut.attribute2().makeValue("Hello");

            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            test::MySequenceWithNullable valueIn;

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);

            if (veryVerbose) bsl::cout << "\tDecoding from Nokalva "
                                          "(DEFINITE LENGTH)."
                                       << bsl::endl;
            {
                const char NOKALVA_DATA[] = "300A8001 22810548 656C6C6F";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::MySequenceWithNullable valueFromNokalva;

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                              nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }

            if (veryVerbose) bsl::cout << "\tDecoding from Nokalva "
                                          "(INDEFINITE LENGTH)."
                                       << bsl::endl;
            {
                const char NOKALVA_DATA[] = "30808001 22810548 656C6C6F 0000";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::MySequenceWithNullable valueFromNokalva;

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                              nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }
        }

        if (verbose) bsl::cout << "\nEnd of test." << bsl::endl;
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING CHOICES
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Choices"
                               << "\n===============" << bsl::endl;

        if (verbose) bsl::cout << "\nTesting with no selection." << bsl::endl;
        {
            bdlsb::MemOutStreamBuf osb;

            test::MyChoice valueOut;

            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            test::MyChoice valueIn;
            valueIn.makeSelection2();
            valueIn.selection2() = "Hello";

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);
        }

        if (verbose) bsl::cout << "\nTesting with selection." << bsl::endl;
        {
            bdlsb::MemOutStreamBuf osb;

            test::MyChoice valueOut;
            valueOut.makeSelection1();
            valueOut.selection1() = 34;

            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            test::MyChoice valueIn;

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);

            if (veryVerbose) bsl::cout << "\tDecoding from Nokalva "
                                          "(DEFINITE LENGTH)."
                                       << bsl::endl;
            {
                const char NOKALVA_DATA[] = "3005A003 800122";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::MyChoice valueFromNokalva;

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                              nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }

            if (veryVerbose) bsl::cout << "\tDecoding from Nokalva "
                                          "(INDEFINITE LENGTH)."
                                       << bsl::endl;
            {
                const char NOKALVA_DATA[] = "3080A080 80012200 000000";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::MyChoice valueFromNokalva;

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                              nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }
        }

        if (verbose) bsl::cout << "\nEnd of test." << bsl::endl;
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING SEQUENCES
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Sequences"
                               << "\n=================" << bsl::endl;

        bdlsb::MemOutStreamBuf osb;

        test::MySequence valueOut;
        valueOut.attribute1() = 34;
        valueOut.attribute2() = "Hello";

        ASSERT(0 == encoder.encode(&osb, valueOut));

        if (veryVerbose) {
            P(osb.length())
            printBuffer(osb.data(), osb.length());
        }

        test::MySequence valueIn;

        ASSERT(valueOut != valueIn);
        bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
        ASSERT(0 == decoder.decode(&isb, &valueIn));
        printDiagnostic(decoder);

        ASSERT(valueOut == valueIn);

        static const struct {
            int         d_line;
            const char *d_nokalvaData;
            int         d_numUnknownElements;
        } DATA[] = {
            //Line Nokalva Data                                     Num Unknown
            //==== ============                                     ===========

            // Definite length:
            { L_,  "300A 800122         810548656C6C6F"                  , 0 },
            { L_,  "300A 810548656C6C6F 800122"                          , 0 },

            { L_,  "300D 820199         800122         810548656C6C6F"   , 1 },
            { L_,  "300D 820199         810548656C6C6F 800122"           , 1 },
            { L_,  "300F A203820199     810548656C6C6F 800122"           , 1 },
            { L_,  "3011 A2808201990000 810548656C6C6F 800122"           , 1 },
            //           ^^^^^^: Extra (unknown) attr3

            { L_,  "300D 800122         820199         810548656C6C6F"   , 1 },
            { L_,  "300D 810548656C6C6F 820199         800122"           , 1 },
            { L_,  "300F 810548656C6C6F A203820199     800122"           , 1 },
            { L_,  "3011 810548656C6C6F A2808201990000 800122"           , 1 },
            //   Extra (unknown) attr3: ^^^^^^

            { L_,  "300D 800122         810548656C6C6F 820199"           , 1 },
            { L_,  "300D 810548656C6C6F 800122         820199"           , 1 },
            { L_,  "300F 810548656C6C6F 800122         A203820199"       , 1 },
            { L_,  "3011 810548656C6C6F 800122         A2808201990000"   , 1 },
            //                  Extra (unknown) attr3: ^^^^^^

            // Indefinite length:
            { L_,  "3080 800122         810548656C6C6F 0000"             , 0 },
            { L_,  "3080 810548656C6C6F 800122         0000"             , 0 },

            { L_,  "3080 820199         800122         810548656C6C6F 0000",1},
            { L_,  "3080 820199         810548656C6C6F 800122         0000",1},
            { L_,  "3080 A203820199     810548656C6C6F 800122         0000",1},
            { L_,  "3080 A2808201990000 810548656C6C6F 800122         0000",1},
            //           ^^^^^^: Extra (unknown) attr3

            { L_,  "3080 800122         820199         810548656C6C6F 0000",1},
            { L_,  "3080 810548656C6C6F 820199         800122         0000",1},
            { L_,  "3080 810548656C6C6F A203820199     800122         0000",1},
            { L_,  "3080 810548656C6C6F A2808201990000 800122         0000",1},
            //   Extra (unknown) attr3: ^^^^^^

            { L_,  "3080 800122         810548656C6C6F 820199         0000",1},
            { L_,  "3080 810548656C6C6F 800122         820199         0000",1},
            { L_,  "3080 810548656C6C6F 800122         A203820199     0000",1},
            { L_,  "3080 810548656C6C6F 800122         A2808201990000 0000",1},
            //                  Extra (unknown) attr3: ^^^^^^
        };

        static const int DATA_LEN = sizeof(DATA) / sizeof(DATA[0]);

        if (verbose) bsl::cout << "Testing with skipUnknownElements == true"
                               << bsl::endl;
        for (int i = 0; i < DATA_LEN; ++i)
        {
            const int   LINE         = DATA[i].d_line;
            const char* NOKALVA_DATA = DATA[i].d_nokalvaData;
            const int   NUM_UNKNOWN  = DATA[i].d_numUnknownElements;

            bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);
            const char *berData = &nokalvaData[0];
            const int berDataLen = nokalvaData.size();

            test::MySequence valueFromNokalva;

            ASSERT(valueOut != valueFromNokalva);
            bdlsb::FixedMemInStreamBuf isb(berData, berDataLen);
            decoder.setNumUnknownElementsSkipped(0);
            int ret = decoder.decode(&isb, &valueFromNokalva);
            printDiagnostic(decoder);
            LOOP3_ASSERT(LINE,
                         NUM_UNKNOWN,
                         decoder.numUnknownElementsSkipped(),
                         NUM_UNKNOWN == decoder.numUnknownElementsSkipped());

            LOOP_ASSERT(LINE, 0 == ret);
            LOOP_ASSERT(LINE, berDataLen == isb.pubseekoff(0, bsl::ios::cur));
            LOOP_ASSERT(LINE, valueOut == valueFromNokalva);
        }

        if (verbose) bsl::cout << "Testing with skipUnknownElements == false"
                               << bsl::endl;
        for (int i = 0; i < DATA_LEN; ++i)
        {
            const int   LINE         = DATA[i].d_line;
            const char* NOKALVA_DATA = DATA[i].d_nokalvaData;
            const int   NUM_UNKNOWN  = DATA[i].d_numUnknownElements;

            bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);
            const char *berData = &nokalvaData[0];
            const int berDataLen = nokalvaData.size();

            test::MySequence valueFromNokalva;

            ASSERT(valueOut != valueFromNokalva);
            bdlsb::FixedMemInStreamBuf isb(berData, berDataLen);

            balber::BerDecoderOptions opts;
            opts.setSkipUnknownElements(false);
            if (veryVeryVerbose) {
               opts.setTraceLevel(1);
            }

            balber::BerDecoder decoder2(&opts);
            int ret = decoder2.decode(&isb, &valueFromNokalva);
            printDiagnostic(decoder2);

            if (NUM_UNKNOWN) {
                LOOP_ASSERT(LINE, 0 != ret);
                LOOP_ASSERT(LINE,
                          NUM_UNKNOWN == decoder2.numUnknownElementsSkipped());
            }
            else {
                LOOP_ASSERT(LINE,
                            berDataLen == isb.pubseekoff(0, bsl::ios::cur));
                LOOP_ASSERT(LINE, valueOut == valueFromNokalva);
            }
        }

        if (verbose) bsl::cout << "\nEnd of test." << bsl::endl;

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING ENUMERATIONS
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Enumerations"
                               << "\n====================" << bsl::endl;

        if (verbose) bsl::cout << "\nTesting with VALUE1." << bsl::endl;
        {
            bdlsb::MemOutStreamBuf osb;

            test::MyEnumeration::Value valueOut = test::MyEnumeration::VALUE1;

            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            test::MyEnumeration::Value valueIn = test::MyEnumeration::VALUE2;

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);

            if (veryVerbose) bsl::cout << "\tDecoding from Nokalva."
                                       << bsl::endl;
            {
                const char NOKALVA_DATA[] = "0A0100";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::MyEnumeration::Value valueFromNokalva
                                                 = test::MyEnumeration::VALUE2;

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                              nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }
        }

        if (verbose) bsl::cout << "\nTesting with VALUE2." << bsl::endl;
        {
            bdlsb::MemOutStreamBuf osb;

            test::MyEnumeration::Value valueOut = test::MyEnumeration::VALUE2;

            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            test::MyEnumeration::Value valueIn = test::MyEnumeration::VALUE1;

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);

            if (veryVerbose) bsl::cout << "\tDecoding from Nokalva."
                                       << bsl::endl;
            {
                const char NOKALVA_DATA[] = "0A0101";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::MyEnumeration::Value valueFromNokalva
                                                 = test::MyEnumeration::VALUE1;

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                              nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }
        }

        if (verbose) bsl::cout << "\nEnd of test." << bsl::endl;
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING CUSTOMIZED TYPES
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Customized Types"
                               << "\n========================" << bsl::endl;

        const bsl::string VALUE = "Hello";

        if (verbose) bsl::cout << "\nEncoding customized string." << bsl::endl;
        {
            bdlsb::MemOutStreamBuf osb;

            test::CustomizedString valueOut;
            valueOut.fromString(VALUE);

            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            test::CustomizedString valueIn;

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);

            if (veryVerbose) bsl::cout << "\tDecoding from Nokalva."
                                       << bsl::endl;
            {
                const char NOKALVA_DATA[] = "0C054865 6C6C6F";

                bsl::vector<char> nokalvaData = loadFromHex(NOKALVA_DATA);

                test::CustomizedString valueFromNokalva;

                ASSERT(valueOut != valueFromNokalva);
                bdlsb::FixedMemInStreamBuf isb(&nokalvaData[0],
                                              nokalvaData.size());
                ASSERT(0 == decoder.decode(&isb, &valueFromNokalva));
                printDiagnostic(decoder);

                ASSERT(valueOut == valueFromNokalva);
            }
        }

        if (verbose) bsl::cout << "\nEncoding bsl::string (control)."
                               << bsl::endl;
        {
            bdlsb::MemOutStreamBuf osb;

            bsl::string valueOut = VALUE;

            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            test::CustomizedString valueIn;

            ASSERT(valueOut != valueIn.toString());
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn.toString());
        }

        if (verbose) bsl::cout << "\nEnd of test." << bsl::endl;
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // VOCABULARY TYPES TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nVOCABULARY TYPES TEST"
                               << "\n=====================" << bsl::endl;

        if (verbose) bsl::cout << "\nTesting bdlt::Date"
                               << "\n=================" << bsl::endl;

        {
            const int YEAR = 2005, MONTH = 12, DAY = 15;

            bdlt::Date valueOut(YEAR, MONTH, DAY);

            bdlsb::MemOutStreamBuf osb;
            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            bdlt::Date valueIn;

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);
        }

        if (verbose) bsl::cout << "\nTesting bdlt::DateTz"
                               << "\n===================" << bsl::endl;

        {
            const int YEAR = 2005, MONTH = 12, DAY = 15, OFFSET = 45;

            bdlt::DateTz valueOut(bdlt::Date(YEAR, MONTH, DAY), OFFSET);

            bdlsb::MemOutStreamBuf osb;
            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            bdlt::DateTz valueIn;

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);
        }

        if (verbose) bsl::cout << "\nTesting bdlt::Time"
                               << "\n=================" << bsl::endl;

        {
            const int HOUR = 12, MIN = 56, SECS = 9, MILLISECS = 134;

            bdlt::Time valueOut(HOUR, MIN, SECS, MILLISECS);

            bdlsb::MemOutStreamBuf osb;
            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            bdlt::Time valueIn;

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);
        }

        if (verbose) bsl::cout << "\nTesting bdlt::TimeTz"
                               << "\n===================" << bsl::endl;

        {
            const int HOUR   = 12, MIN = 56, SECS = 9, MILLISECS = 134,
                      OFFSET = 45;

            bdlt::TimeTz valueOut(bdlt::Time(HOUR, MIN, SECS, MILLISECS),
                                 OFFSET);

            bdlsb::MemOutStreamBuf osb;
            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            bdlt::TimeTz valueIn;

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);
        }

        if (verbose) bsl::cout << "\nTesting bdlt::Datetime"
                               << "\n=====================" << bsl::endl;

        {
            const int YEAR = 2005, MONTH = 12, DAY = 15;
            const int HOUR = 12, MIN = 56, SECS = 9, MILLISECS = 134;

            bdlt::Datetime valueOut(YEAR, MONTH, DAY,
                                   HOUR, MIN, SECS, MILLISECS);

            bdlsb::MemOutStreamBuf osb;
            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            bdlt::Datetime valueIn;

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);
        }

        if (verbose) bsl::cout << "\nTesting bdlt::DatetimeTz"
                               << "\n=======================" << bsl::endl;

        {
            const int YEAR   = 2005, MONTH = 12, DAY = 15;
            const int HOUR   = 12, MIN = 56, SECS = 9, MILLISECS = 134;
            const int OFFSET = 45;

            bdlt::DatetimeTz valueOut(bdlt::Datetime(YEAR, MONTH, DAY,
                                                   HOUR, MIN, SECS, MILLISECS),
                                      OFFSET);

            bdlsb::MemOutStreamBuf osb;
            ASSERT(0 == encoder.encode(&osb, valueOut));

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }

            bdlt::DatetimeTz valueIn;

            ASSERT(valueOut != valueIn);
            bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &valueIn));
            printDiagnostic(decoder);

            ASSERT(valueOut == valueIn);
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // REAL TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nREAL TEST"
                               << "\n=========" << bsl::endl;

        {
            const float  XA1 = 99.234;               float  XA2;
            const float  XB1 = -100.987;             float  XB2;
            const float  XC1 = -77723.875;           float  XC2;
            const float  XD1 = 1.4E-45;              float  XD2;
            const float  XE1 = -1.4E-45;             float  XE2;
            const float  XF1 = 3.402823466E+38;      float  XF2;
            const float  XG1 = 1.175494351E-38;      float  XG2;

            const double XH1 = 19998989.1234;        double  XH2;
            const double XI1 = -7.8752345;           double  XI2;
            const double XJ1 = 1.4E-45;              double  XJ2;
            const double XK1 = -1.4E-45;             double  XK2;
            const double XL1 = 3.402823466E+38;      double  XL2;
            const double XM1 = 1.175494351E-38;      double  XM2;
            const double XN1 = 1e-324;               double  XN2;

            const float  XO1 = 0;                     float XO2;
            const double XP1 = 0;                     double XP2;

            const double XQ1 = -1E-324;                     double XQ2;
            const double XR1 = -1.98347E-325;               double XR2;
            const double XS1 = 78979.23E-330;               double XS2;
            const double XT1 = 79879879249686698E-100;      double XT2;
            const double XU1 = -9999999999999999E+25;       double XU2;

            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XA1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XA2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XA1, XA2, XA1 == XA2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XB1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XB2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XB1, XB2, XB1 == XB2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XC1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XC2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XC1, XC2, XC1 == XC2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XD1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XD2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XD1, XD2, XD1 == XD2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XE1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XE2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XE1, XE2, XE1 == XE2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XF1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XF2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XF1, XF2, XF1 == XF2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XG1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XG2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XG1, XG2, XG1 == XG2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XH1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XH2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XH1, XH2, XH1 == XH2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XI1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XI2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XI1, XI2, XI1 == XI2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XJ1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XJ2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XJ1, XJ2, XJ1 == XJ2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XK1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XK2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XK1, XK2, XK1 == XK2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XL1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XL2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XL1, XL2, XL1 == XL2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XM1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XM2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XM1, XM2, XM1 == XM2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XN1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XN2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XN1, XN2, XN1 == XN2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XO1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XO2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XO1, XO2, XO1 == XO2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XP1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XP2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XP1, XP2, XP1 == XP2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XQ1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XQ2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XQ1, XQ2, XQ1 == XQ2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XR1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XR2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XR1, XR2, XR1 == XR2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XS1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XS2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XS1, XS2, XS1 == XS2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XT1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XT2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XT1, XT2, XT1 == XT2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XU1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XU2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XU1, XU2, XU1 == XU2);
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // FUNDAMENTALS TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nFUNDAMENTALS TEST"
                               << "\n=================" << bsl::endl;

        {
            const unsigned char   XA1 = UCHAR_MAX;       unsigned char     XA2;
            const signed   char   XB1 = SCHAR_MIN;       signed   char     XB2;
            const          char   XC1 = SCHAR_MAX;                char     XC2;

            const unsigned short  XD1 = USHRT_MAX;    unsigned short    XD2;
            const signed   short  XE1 = SHRT_MIN;     signed   short    XE2;
            const          short  XF1 = SHRT_MAX;              short    XF2;

            const unsigned int    XG1 = UINT_MAX; unsigned int      XG2;
            const signed   int    XH1 = INT_MIN;  signed   int      XH2;
            const          int    XI1 = INT_MAX;           int      XI2;

            const unsigned long   XJ1 = ULONG_MAX; unsigned long     XJ2;
            const signed   long   XK1 = LONG_MIN;  signed   long     XK2;
            const          long   XL1 = LONG_MAX;           long     XL2;

            const bsls::Types::Int64  XM1 = 0xff34567890123456LL;
            bsls::Types::Int64        XM2;
            const bsls::Types::Uint64 XN1 = 0x1234567890123456LL;
            bsls::Types::Uint64       XN2;

            const          bool   XO1 = true;             bool         XO2;

            const bsl::string     XP1("This is a really long line");
            bsl::string           XP2;

            const float        XQ1 = 99.234;           float        XQ2;
            const float        XR1 = -100.987;         float        XR2;
            const float        XS1 = -77723.875;       float        XS2;

            const double       XT1 = 19998989.1234;    double        XT2;
            const double       XU1 = -7.8752345;       double        XU2;

            const double       XV1 = 100.1;              double        XV2;
            const double       XW1 = 200.1;              double        XW2;
            const double       XX1 = 2500.1;             double        XX2;
            const double       XY1 = 1600.1;             double        XY2;
            const double       XZ1 = 100;                double        XZ2;

            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XA1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XA2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XA1, XA2, XA1 == XA2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XB1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XB2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XB1, XB2, XB1 == XB2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XC1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XC2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XC1, XC2, XC1 == XC2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XD1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XD2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XD1, XD2, XD1 == XD2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XE1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XE2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XE1, XE2, XE1 == XE2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XF1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XF2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XF1, XF2, XF1 == XF2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XG1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XG2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XG1, XG2, XG1 == XG2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XH1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XH2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XH1, XH2, XH1 == XH2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XI1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XI2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XI1, XI2, XI1 == XI2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XJ1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XJ2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XJ1, XJ2, XJ1 == XJ2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XK1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XK2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XK1, XK2, XK1 == XK2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XL1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XL2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XL1, XL2, XL1 == XL2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XM1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XM2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XM1, XM2, XM1 == XM2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XN1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XN2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XN1, XN2, XN1 == XN2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XO1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XO2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XO1, XO2, XO1 == XO2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XP1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XP2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XP1, XP2, XP1 == XP2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XQ1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XQ2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XQ1, XQ2, XQ1 == XQ2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XR1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XR2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XR1, XR2, XR1 == XR2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XS1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XS2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XS1, XS2, XS1 == XS2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XT1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XT2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XT1, XT2, XT1 == XT2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XU1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XU2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XU1, XU2, XU1 == XU2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XV1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XV2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XV1, XV2, XV1 == XV2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XW1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XW2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XW1, XW2, XW1 == XW2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XX1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XX2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XX1, XX2, XX1 == XX2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XY1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XY2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XY1, XY2, XY1 == XY2);
            }
            {
                bdlsb::MemOutStreamBuf osb;
                ASSERT(0 == encoder.encode(&osb, XZ1));

                if (veryVerbose) {
                    P(osb.length())
                    printBuffer(osb.data(), osb.length());
                }

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
                ASSERT(0 == decoder.decode(&isb, &XZ2));
                printDiagnostic(decoder);
                LOOP2_ASSERT(XZ1, XZ2, XZ1 == XZ2);
            }
        }
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST
        // --------------------------------------------------------------------

        static const bsl::size_t MAX_BUF_SIZE = 100000;

        int reps = 1000;
        int arraySize = 200;
        char requestType = 'b';

        if (argc > 2) {
            ASSERT('-' == argv[2][0]);

            // Request type:
            //  -s == sqrt request
            //  -r == basicRecord request
            //  -b == bigRecord request
            //  -2 == bigRecord2 request
            requestType = argv[2][1];
            if (('b' == requestType || '2' == requestType) && argc > 4) {
                // Get array size for types contain arrays
                arraySize = bsl::atoi(argv[4]);
                veryVeryVerbose = argc > 5;
            }
        }

        if (argc > 3) {
            // Get number of repetitions
            reps = bsl::atoi(argv[3]);
        }

        // Create request object:
        test::TimingRequest request;
        bsl::size_t         minOutputSize;
        switch (requestType) {
          case 's': {
              bsl::cout << "sqrt request" << bsl::endl;

              test::Sqrt sqrt;
              sqrt.value() = 3.1415927;
              request.makeSqrt(sqrt);
              minOutputSize = 2;
          } break;
          case 'r': {
              bsl::cout << "basicRecord request" << bsl::endl;

              test::BasicRecord basicRec;
              basicRec.i1() = 11;
              basicRec.i2() = 22;
              basicRec.dt() = bdlt::DatetimeTz(
                  bdlt::Datetime(bdlt::Date(2007, 9, 3),
                                bdlt::Time(16, 30)), 0);
              basicRec.s() = "The quick brown fox jumped over the lazy dog.";
              request.makeBasic(basicRec);
              minOutputSize = 3 * sizeof(int) + basicRec.s().length();
          } break;
          case 'b': {
              bsl::cout << "bigRecord request with array size of "
                        << arraySize << bsl::endl;

              test::BasicRecord basicRec;
              basicRec.i1() = 11;
              basicRec.i2() = 22;
              basicRec.dt() = bdlt::DatetimeTz(
                  bdlt::Datetime(bdlt::Date(2007, 9, 3),
                                bdlt::Time(16, 30)), 0);
              basicRec.s() = "The quick brown fox jumped over the lazy dog.";
              int minBasicRecSize =
                  minOutputSize = 3 * sizeof(int) + basicRec.s().length();

              test::BigRecord   bigRec;
              bigRec.name() = "This record is so big, it has its own gravity.";

              for (int i = 0; i < arraySize; ++i) {
                  bigRec.array().push_back(basicRec);
              }

              request.makeBig(bigRec);
              minOutputSize =
                  bigRec.name().length() + arraySize * minBasicRecSize;
          } break;
          default:
            bsl::cerr << "Unknown request type: " << '-' << requestType
                      << bsl::endl;
            return 1;                                                 // RETURN
        }

        bsl::cout << "  " << reps << " repetitions..." << bsl::endl;

        bdlsb::MemOutStreamBuf osb;
        osb.reserveCapacity(MAX_BUF_SIZE);

        bsls::Stopwatch stopwatch;
        double elapsed;

        // Measure ber encoding and decoding times:
        stopwatch.reset();
        stopwatch.start();
        for (int i = 0; i < reps; ++i) {
            osb.pubseekpos(0);
            balber::BerEncoder encoder;  // Typical usage: single-use object
            encoder.encode(&osb, request);
        }
        stopwatch.stop();

        ASSERT(minOutputSize <= osb.length());
        ASSERT(osb.length() <= MAX_BUF_SIZE);
        elapsed = stopwatch.elapsedTime();
        ASSERT(elapsed > 0);

        bsl::cout << "    balber::BerEncoder: "
                  << elapsed          << " seconds, "
                  << (reps / elapsed) << " reps/sec, "
                  << osb.length()     << " bytes" << bsl::endl;

        test::TimingRequest *inRequests = 0;
        bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());

        isb.pubsetbuf(osb.data(), osb.length());
        inRequests = new test::TimingRequest[reps];
        stopwatch.reset();
        stopwatch.start();
        for (int i = 0; i < reps; ++i) {
            isb.pubseekpos(0);
            balber::BerDecoder decoder;  // Typical usage: single-use object
            decoder.decode(&isb, &inRequests[i]);
        }
        stopwatch.stop();

        if (veryVeryVerbose) { P(request); P(*inRequests); }
        ASSERT(*inRequests == request);
        elapsed = stopwatch.elapsedTime();
        ASSERT(elapsed > 0);
        delete[] inRequests;

        bsl::cout << "    balber::BerDecoder: "
                  << elapsed          << " seconds, "
                  << (reps / elapsed) << " reps/sec" << bsl::endl;

        delete[] inRequests;
      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
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
