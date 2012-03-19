// bdem_berencoder.t.cpp                                              -*-C++-*-

#include <bdem_berencoder.h>

#include <bdeat_attributeinfo.h>
#include <bdeat_selectioninfo.h>
#include <bdeat_valuetypefunctions.h>

#include <bdesb_memoutstreambuf.h>

#include <bdet_date.h>
#include <bdet_datetime.h>
#include <bdet_time.h>

#include <bdeu_chartype.h>

#include <bdeu_chartype.h>
#include <bdeu_print.h>
#include <bdeu_printmethods.h>

#include <bdex_byteoutstreamformatter.h>

#include <bsls_objectbuffer.h>
#include <bsls_stopwatch.h>
#include <bsls_types.h>

#include <bsl_iostream.h>
#include <bsl_iomanip.h>

#include <bsl_cstdlib.h>
#include <bsl_cctype.h>
#include <bsl_bitset.h>

#include <bsl_climits.h>
#include <bsl_fstream.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::dec;
using bsl::hex;
using bsl::flush;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//
//

//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//--------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " \
                          << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J \
                         << "\t" << #K << ": " << K << "\n";           \
                aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J \
                         << "\t" << #K << ": " << K << "\t" << #L << ": " \
                         << L << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J    \
                         << "\t" << #K << ": " << K << "\t" << #L << ": " \
                         << L << "\t" << #M << ": " << M << "\n";         \
               aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J     \
                         << "\t" << #K << ": " << K << "\t" << #L << ": "  \
                         << L << "\t" << #M << ": " << M << "\t" << #N     \
                         << ": " << N << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl; // Print ID and value.
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;  // Quote ID literally.
#define P_(X) bsl::cout << #X " = " << (X) << ", " << flush; // P(X) w/o '\n'
#define L_ __LINE__                                // current Line number
#define T_ bsl::cout << "\t" << flush;             // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

//typedef bdem_BerEncoder Obj;

//struct Obj
//{
//    template <typename TYPE>
//    static int encode(bsl::streambuf *streamBuf, const TYPE& value)
//    {
//        bdem_BerEncoder encoder(0, &bsl::cerr, &bsl::cerr);
//        return encoder.encode(streamBuf, value);
//    }
//
//    template <typename TYPE>
//    static bsl::ostream& encode(bsl::ostream& stream, const TYPE& value)
//    {
//        bdem_BerEncoder encoder(0, &bsl::cerr, &bsl::cerr);
//        return encoder.encode(stream, value);
//    }
//
//};

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };
enum { SUCCESS = 0, FAILURE = -1 };
static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

int getIntValue(char c)
{
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    c = toupper(c);
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    ASSERT(0);
    return -1;
}

int compareBuffers(const char *stream, const char *buffer)
    // Compare the data written to the  specified 'stream' with the data in the
    // specified 'buffer'.  Return 0 on success, and -1 otherwise.
{
    while (*buffer) {
        if (' ' == *buffer) {
            ++buffer;
            continue;
        }
        char temp = (char) getIntValue(*buffer) << 4;
        ++buffer;
        temp |= (char) getIntValue(*buffer);
        if (*stream != temp) {
           return -1;
        }
        ++stream;
        ++buffer;
    }
    return 0;
}

void printBuffer(const char *buffer, int length)
    // Print the specified 'buffer' of the specified 'length' in hex form
{
    bsl::cout << bsl::hex;
    int numOutput = 0;
    for (int i = 0; i < length; ++i) {
        if ((unsigned char) buffer[i] < 16) {
            bsl::cout << '0';
        }
        bsl::cout << (int) (unsigned char) buffer[i];
        numOutput += 2;
        if (0 == numOutput % 8) {
            cout << " ";
        }
    }
    bsl::cout << bsl::dec << bsl::endl;
}

#define DOUBLE_MANTISSA_MASK   0xfffffffffffffLL
#define DOUBLE_SIGN_MASK       ((long long) ((long long) 1                   \
                                               << (sizeof(long long) * 8 - 1)))

void assembleDouble(double *value, int sign, int exponent, long long mantissa)
{
    enum {
        DOUBLE_EXPONENT_SHIFT  = 52,
        DOUBLE_BIAS            = 1023
    };

    unsigned long long *longLongValue
                               = reinterpret_cast<unsigned long long *>(value);
    *longLongValue = 0;

    exponent += DOUBLE_BIAS;

    *longLongValue  = (unsigned long long) exponent << DOUBLE_EXPONENT_SHIFT;
    *longLongValue |= mantissa & DOUBLE_MANTISSA_MASK;

    if (sign) {
        *longLongValue |= DOUBLE_SIGN_MASK;
    }
}

//=============================================================================
//                  GLOBAL HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

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
//@AUTHOR: Pablo Halpern (phalpern@bloomberg.net)
//
//@DESCRIPTION:
// This component provides classes for testing codecs.

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDEAT_ATTRIBUTEINFO
#include <bdeat_attributeinfo.h>
#endif

#ifndef INCLUDED_BDEAT_ENUMERATORINFO
#include <bdeat_enumeratorinfo.h>
#endif

#ifndef INCLUDED_BDEAT_TYPETRAITS
#include <bdeat_typetraits.h>
#endif

#ifndef INCLUDED_BDEAT_VALUETYPEFUNCTIONS
#include <bdeat_valuetypefunctions.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BDEX_INSTREAMFUNCTIONS
#include <bdex_instreamfunctions.h>
#endif

#ifndef INCLUDED_BDEX_OUTSTREAMFUNCTIONS
#include <bdex_outstreamfunctions.h>
#endif

#ifndef INCLUDED_BDEU_PRINTMETHODS
#include <bdeu_printmethods.h>
#endif

#ifndef INCLUDED_BDET_DATETIMETZ
#include <bdet_datetimetz.h>
#endif

#ifndef INCLUDED_BDEUT_NULLABLEVALUE
#include <bdeut_nullablevalue.h>
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

class bslma_Allocator;

namespace test { class MyChoice; }
namespace test { class MySequenceWithNullable; }
namespace test { class Address; }
namespace test { class MySequence; }
namespace test { class MySequenceWithNillable; }
namespace test { class Sqrt; }
namespace test { class CustomizedString; }
namespace test { class BasicRecord; }
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
        bsls_ObjectBuffer< int >         d_selection1;
        bsls_ObjectBuffer< bsl::string > d_selection2;
    };

    int              d_selectionId;
    bslma_Allocator *d_allocator_p;

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

    static const bdeat_SelectionInfo SELECTION_INFO_ARRAY[];
        // selection information for each selection

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static const bdeat_SelectionInfo *lookupSelectionInfo(int id);
        // Return selection information for the selection indicated by the
        // specified 'id' if the selection exists, and 0 otherwise.

    static const bdeat_SelectionInfo *lookupSelectionInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return selection information for the selection indicated by the
        // specified 'name' of the specified 'nameLength' if the selection
        // exists, and 0 otherwise.

    // CREATORS
    explicit MyChoice(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'MyChoice' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    MyChoice(const MyChoice& original,
            bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'MyChoice' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~MyChoice();
        // Destroy this object.

    // MANIPULATORS
    MyChoice& operator=(const MyChoice& rhs);
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

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

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

}  // close package namespace

// TRAITS

BDEAT_DECL_CHOICE_WITH_ALLOCATOR_TRAITS(test::MyChoice)

namespace test {

                        // ============================
                        // class MySequenceWithNullable
                        // ============================

class MySequenceWithNullable {
    // TODO: Provide annotation

  private:
    int                               d_attribute1;
    bdeut_NullableValue<bsl::string>  d_attribute2;

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

    static const bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static const bdeat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.

    static const bdeat_AttributeInfo *lookupAttributeInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.

    // CREATORS
    explicit MySequenceWithNullable(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'MySequenceWithNullable' having the default
        // value.  Use the optionally specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    MySequenceWithNullable(const MySequenceWithNullable& original,
                           bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'MySequenceWithNullable' having the value
        // of the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~MySequenceWithNullable();
        // Destroy this object.

    // MANIPULATORS
    MySequenceWithNullable& operator=(const MySequenceWithNullable& rhs);
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

    bdeut_NullableValue<bsl::string>& attribute2();
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

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

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

    const bdeut_NullableValue<bsl::string>& attribute2() const;
        // Return a reference to the non-modifiable "Attribute2" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const MySequenceWithNullable& lhs, const MySequenceWithNullable& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const MySequenceWithNullable& lhs, const MySequenceWithNullable& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const MySequenceWithNullable& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::MySequenceWithNullable)

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

    static const bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static const bdeat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.

    static const bdeat_AttributeInfo *lookupAttributeInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.

    // CREATORS
    explicit Address(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'Address' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Address(const Address& original,
            bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'Address' having the value of the specified
        // 'original' object.  Use the optionally specified 'basicAllocator' to
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

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

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

}  // close package namespace

// TRAITS

BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::Address)

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

    static const bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static const bdeat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.

    static const bdeat_AttributeInfo *lookupAttributeInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.

    // CREATORS
    explicit MySequence(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'MySequence' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    MySequence(const MySequence& original,
               bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'MySequence' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~MySequence();
        // Destroy this object.

    // MANIPULATORS
    MySequence& operator=(const MySequence& rhs);
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

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

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

}  // close package namespace

// TRAITS

BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::MySequence)

namespace test {

                        // ============================
                        // class MySequenceWithNillable
                        // ============================

class MySequenceWithNillable {
    // TODO: Provide annotation

  private:
    int                               d_attribute1;
    bdeut_NullableValue<bsl::string>  d_myNillable;
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

    static const bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static const bdeat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.

    static const bdeat_AttributeInfo *lookupAttributeInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.

    // CREATORS
    explicit MySequenceWithNillable(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'MySequenceWithNillable' having the default
        // value.  Use the optionally specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    MySequenceWithNillable(const MySequenceWithNillable& original,
                           bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'MySequenceWithNillable' having the value
        // of the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~MySequenceWithNillable();
        // Destroy this object.

    // MANIPULATORS
    MySequenceWithNillable& operator=(const MySequenceWithNillable& rhs);
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

    bdeut_NullableValue<bsl::string>& myNillable();
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

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

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

    const bdeut_NullableValue<bsl::string>& myNillable() const;
        // Return a reference to the non-modifiable "MyNillable" attribute of
        // this object.

    const bsl::string& attribute2() const;
        // Return a reference to the non-modifiable "Attribute2" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const MySequenceWithNillable& lhs, const MySequenceWithNillable& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const MySequenceWithNillable& lhs, const MySequenceWithNillable& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const MySequenceWithNillable& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::MySequenceWithNillable)

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

    static const bdeat_EnumeratorInfo ENUMERATOR_INFO_ARRAY[];
        // enumerator information for each enumerator

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
bsl::ostream& operator<<(bsl::ostream& stream, MyEnumeration::Value rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDEAT_DECL_ENUMERATION_TRAITS(test::MyEnumeration)

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

    static const bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static const bdeat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.

    static const bdeat_AttributeInfo *lookupAttributeInfo(
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

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

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

}  // close package namespace

// TRAITS

BDEAT_DECL_SEQUENCE_TRAITS(test::Sqrt)

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
    friend bool operator==(const CustomizedString& lhs, const CustomizedString& rhs);
    friend bool operator!=(const CustomizedString& lhs, const CustomizedString& rhs);

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
    explicit CustomizedString(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'CustomizedString' having the default
        // value.  Use the optionally specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    CustomizedString(const CustomizedString& original,
                    bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'CustomizedString' having the value
        // of the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    explicit CustomizedString(const bsl::string& value,
                             bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'CustomizedString' having the specified
        // 'value'.  Use the optionally specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    ~CustomizedString();
        // Destroy this object.

    // MANIPULATORS
    CustomizedString& operator=(const CustomizedString& rhs);
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
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    int fromString(const bsl::string& value);
        // Convert from the specified 'value' to this type.  Return 0 if
        // successful and non-zero otherwise.

    // ACCESSORS
    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

    int maxSupportedBdexVersion() const;
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

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

}  // close package namespace

// TRAITS

BDEAT_DECL_CUSTOMIZEDTYPE_WITH_ALLOCATOR_TRAITS(test::CustomizedString)

namespace test {

                             // =================
                             // class BasicRecord
                             // =================

class BasicRecord {
    // A representative small record type

  private:
    int              d_i1;
    int              d_i2;
    bdet_DatetimeTz  d_dt;
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

    static const bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static const bdeat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.

    static const bdeat_AttributeInfo *lookupAttributeInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.

    // CREATORS
    explicit BasicRecord(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'BasicRecord' having the default value.
        // Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    BasicRecord(const BasicRecord& original,
                bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'BasicRecord' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~BasicRecord();
        // Destroy this object.

    // MANIPULATORS
    BasicRecord& operator=(const BasicRecord& rhs);
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

    bdet_DatetimeTz& dt();
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

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

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

    const bdet_DatetimeTz& dt() const;
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

}  // close package namespace

// TRAITS

BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::BasicRecord)

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

    static const bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static const bdeat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.

    static const bdeat_AttributeInfo *lookupAttributeInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.

    // CREATORS
    explicit MySequenceWithArray(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'MySequenceWithArray' having the default
        // value.  Use the optionally specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    MySequenceWithArray(const MySequenceWithArray& original,
                        bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'MySequenceWithArray' having the value of
        // the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~MySequenceWithArray();
        // Destroy this object.

    // MANIPULATORS
    MySequenceWithArray& operator=(const MySequenceWithArray& rhs);
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

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

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
bool operator==(const MySequenceWithArray& lhs, const MySequenceWithArray& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const MySequenceWithArray& lhs, const MySequenceWithArray& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const MySequenceWithArray& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::MySequenceWithArray)

namespace test {

                 // =========================================
                 // class MySequenceWithAnonymousChoiceChoice
                 // =========================================

class MySequenceWithAnonymousChoiceChoice {
    // TODO: Provide annotation

  private:
    union {
        bsls_ObjectBuffer< int >         d_myChoice1;
        bsls_ObjectBuffer< bsl::string > d_myChoice2;
    };

    int              d_selectionId;
    bslma_Allocator *d_allocator_p;

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

    static const bdeat_SelectionInfo SELECTION_INFO_ARRAY[];
        // selection information for each selection

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static const bdeat_SelectionInfo *lookupSelectionInfo(int id);
        // Return selection information for the selection indicated by the
        // specified 'id' if the selection exists, and 0 otherwise.

    static const bdeat_SelectionInfo *lookupSelectionInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return selection information for the selection indicated by the
        // specified 'name' of the specified 'nameLength' if the selection
        // exists, and 0 otherwise.

    // CREATORS
    explicit MySequenceWithAnonymousChoiceChoice(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'MySequenceWithAnonymousChoiceChoice'
        // having the default value.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    MySequenceWithAnonymousChoiceChoice(const MySequenceWithAnonymousChoiceChoice& original,
                                       bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'MySequenceWithAnonymousChoiceChoice'
        // having the value of the specified 'original' object.  Use the
        // optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    ~MySequenceWithAnonymousChoiceChoice();
        // Destroy this object.

    // MANIPULATORS
    MySequenceWithAnonymousChoiceChoice& operator=(const MySequenceWithAnonymousChoiceChoice& rhs);
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

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

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
bool operator==(const MySequenceWithAnonymousChoiceChoice& lhs, const MySequenceWithAnonymousChoiceChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'MySequenceWithAnonymousChoiceChoice' objects have the same
    // value if either the selections in both objects have the same ids and
    // the same values, or both selections are undefined.

inline
bool operator!=(const MySequenceWithAnonymousChoiceChoice& lhs, const MySequenceWithAnonymousChoiceChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const MySequenceWithAnonymousChoiceChoice& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDEAT_DECL_CHOICE_WITH_ALLOCATOR_TRAITS(test::MySequenceWithAnonymousChoiceChoice)

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

    static const bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static const bdeat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.

    static const bdeat_AttributeInfo *lookupAttributeInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.

    // CREATORS
    explicit Employee(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'Employee' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Employee(const Employee& original,
             bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'Employee' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

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

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

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

}  // close package namespace

// TRAITS

BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::Employee)

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

    static const bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static const bdeat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.

    static const bdeat_AttributeInfo *lookupAttributeInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.

    // CREATORS
    explicit BigRecord(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'BigRecord' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    BigRecord(const BigRecord& original,
              bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'BigRecord' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~BigRecord();
        // Destroy this object.

    // MANIPULATORS
    BigRecord& operator=(const BigRecord& rhs);
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

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

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

}  // close package namespace

// TRAITS

BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::BigRecord)

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

    static const bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static const bdeat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.

    static const bdeat_AttributeInfo *lookupAttributeInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.

    // CREATORS
    explicit MySequenceWithAnonymousChoice(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'MySequenceWithAnonymousChoice' having the
        // default value.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    MySequenceWithAnonymousChoice(const MySequenceWithAnonymousChoice& original,
                                  bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'MySequenceWithAnonymousChoice' having the
        // value of the specified 'original' object.  Use the optionally
        // specified 'basicAllocator' to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.

    ~MySequenceWithAnonymousChoice();
        // Destroy this object.

    // MANIPULATORS
    MySequenceWithAnonymousChoice& operator=(const MySequenceWithAnonymousChoice& rhs);
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

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

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
bool operator==(const MySequenceWithAnonymousChoice& lhs, const MySequenceWithAnonymousChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const MySequenceWithAnonymousChoice& lhs, const MySequenceWithAnonymousChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const MySequenceWithAnonymousChoice& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::MySequenceWithAnonymousChoice)

namespace test {

                            // ===================
                            // class TimingRequest
                            // ===================

class TimingRequest {
    // A choice record representative of a typical request object.

  private:
    union {
        bsls_ObjectBuffer< Sqrt >        d_sqrt;
        bsls_ObjectBuffer< BasicRecord > d_basic;
        bsls_ObjectBuffer< BigRecord >   d_big;
    };

    int              d_selectionId;
    bslma_Allocator *d_allocator_p;

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

    static const bdeat_SelectionInfo SELECTION_INFO_ARRAY[];
        // selection information for each selection

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static const bdeat_SelectionInfo *lookupSelectionInfo(int id);
        // Return selection information for the selection indicated by the
        // specified 'id' if the selection exists, and 0 otherwise.

    static const bdeat_SelectionInfo *lookupSelectionInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return selection information for the selection indicated by the
        // specified 'name' of the specified 'nameLength' if the selection
        // exists, and 0 otherwise.

    // CREATORS
    explicit TimingRequest(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'TimingRequest' having the default value.
        // Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    TimingRequest(const TimingRequest& original,
                 bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'TimingRequest' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~TimingRequest();
        // Destroy this object.

    // MANIPULATORS
    TimingRequest& operator=(const TimingRequest& rhs);
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

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

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

}  // close package namespace

// TRAITS

BDEAT_DECL_CHOICE_WITH_ALLOCATOR_TRAITS(test::TimingRequest)

namespace test {

                               // ==============
                               // class Messages
                               // ==============

struct Messages {
    // This class serves as a place holder to reserve a type having the same
    // name as this component.  Doing so ensures that such a type cannot be
    // defined outside of this component in the current namespace.
};

}  // close package namespace

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace test {

                               // --------------
                               // class MyChoice
                               // --------------

// CLASS METHODS
inline
int MyChoice::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// CREATORS
inline
MyChoice::MyChoice(bslma_Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

inline
MyChoice::MyChoice(
    const MyChoice& original,
    bslma_Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
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

template <class STREAM>
inline
STREAM& MyChoice::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            short selectionId;
            stream.getInt16(selectionId);
            if (!stream) {
                return stream;                                        // RETURN
            }
            switch (selectionId) {
              case SELECTION_ID_SELECTION1: {
                makeSelection1();
                bdex_InStreamFunctions::streamIn(
                    stream, d_selection1.object(), 1);
              } break;
              case SELECTION_ID_SELECTION2: {
                makeSelection2();
                bdex_InStreamFunctions::streamIn(
                    stream, d_selection2.object(), 1);
              } break;
              case SELECTION_ID_UNDEFINED: {
                reset();
              } break;
              default:
                stream.invalidate();
            }
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
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

    const bdeat_SelectionInfo *selectionInfo =
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
        bdeat_ValueTypeFunctions::reset(&d_selection1.object());
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
        bdeat_ValueTypeFunctions::reset(&d_selection2.object());
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
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1]);        // RETURN
      case MyChoice::SELECTION_ID_SELECTION2:
        return manipulator(&d_selection2.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2]);        // RETURN
      default:
        BSLS_ASSERT_SAFE(MyChoice::SELECTION_ID_UNDEFINED ==
                     d_selectionId);
        return FAILURE;
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
template <class STREAM>
inline
STREAM& MyChoice::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
            stream.putInt16(d_selectionId);
            switch (d_selectionId) {
              case SELECTION_ID_SELECTION1: {
                bdex_OutStreamFunctions::streamOut(
                    stream, d_selection1.object(), 1);
              } break;
              case SELECTION_ID_SELECTION2: {
                bdex_OutStreamFunctions::streamOut(
                    stream, d_selection2.object(), 1);
              } break;
              default:
                BSLS_ASSERT_SAFE(SELECTION_ID_UNDEFINED == d_selectionId);
            }
      } break;
    }
    return stream;
}

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
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1]);        // RETURN
      case SELECTION_ID_SELECTION2:
        return accessor(d_selection2.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2]);        // RETURN
      default:
        BSLS_ASSERT_SAFE(SELECTION_ID_UNDEFINED == d_selectionId);
        return FAILURE;
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

// CLASS METHODS
inline
int MySequenceWithNullable::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// CREATORS
inline
MySequenceWithNullable::MySequenceWithNullable(bslma_Allocator *basicAllocator)
: d_attribute1()
, d_attribute2(basicAllocator)
{
}

inline
MySequenceWithNullable::MySequenceWithNullable(
        const MySequenceWithNullable& original,
        bslma_Allocator *basicAllocator)
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

template <class STREAM>
inline
STREAM& MySequenceWithNullable::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_attribute1, 1);
            bdex_InStreamFunctions::streamIn(stream, d_attribute2, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

inline
void MySequenceWithNullable::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_attribute1);
    bdeat_ValueTypeFunctions::reset(&d_attribute2);
}

template <class MANIPULATOR>
inline
int MySequenceWithNullable::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_attribute2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
inline
int MySequenceWithNullable::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return manipulator(&d_attribute2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;
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

    const bdeat_AttributeInfo *attributeInfo =
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
bdeut_NullableValue<bsl::string>& MySequenceWithNullable::attribute2()
{
    return d_attribute2;
}

// ACCESSORS
template <class STREAM>
inline
STREAM& MySequenceWithNullable::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_attribute1, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_attribute2, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
inline
int MySequenceWithNullable::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_attribute2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
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
        return accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return accessor(d_attribute2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;
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

    const bdeat_AttributeInfo *attributeInfo =
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
const bdeut_NullableValue<bsl::string>& MySequenceWithNullable::attribute2() const
{
    return d_attribute2;
}

                               // -------------
                               // class Address
                               // -------------

// CLASS METHODS
inline
int Address::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// CREATORS
inline
Address::Address(bslma_Allocator *basicAllocator)
: d_street(basicAllocator)
, d_city(basicAllocator)
, d_state(basicAllocator)
{
}

inline
Address::Address(
        const Address& original,
        bslma_Allocator *basicAllocator)
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
            bdex_InStreamFunctions::streamIn(stream, d_street, 1);
            bdex_InStreamFunctions::streamIn(stream, d_city, 1);
            bdex_InStreamFunctions::streamIn(stream, d_state, 1);
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
    bdeat_ValueTypeFunctions::reset(&d_street);
    bdeat_ValueTypeFunctions::reset(&d_city);
    bdeat_ValueTypeFunctions::reset(&d_state);
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
        return manipulator(&d_street, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STREET]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_CITY: {
        return manipulator(&d_city, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CITY]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_STATE: {
        return manipulator(&d_state, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATE]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;
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

    const bdeat_AttributeInfo *attributeInfo =
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
template <class STREAM>
inline
STREAM& Address::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_street, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_city, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_state, 1);
      } break;
    }
    return stream;
}

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
        return accessor(d_street, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STREET]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_CITY: {
        return accessor(d_city, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CITY]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_STATE: {
        return accessor(d_state, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATE]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;
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

    const bdeat_AttributeInfo *attributeInfo =
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

// CLASS METHODS
inline
int MySequence::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// CREATORS
inline
MySequence::MySequence(bslma_Allocator *basicAllocator)
: d_attribute1()
, d_attribute2(basicAllocator)
{
}

inline
MySequence::MySequence(
        const MySequence& original,
        bslma_Allocator *basicAllocator)
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

template <class STREAM>
inline
STREAM& MySequence::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_attribute1, 1);
            bdex_InStreamFunctions::streamIn(stream, d_attribute2, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

inline
void MySequence::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_attribute1);
    bdeat_ValueTypeFunctions::reset(&d_attribute2);
}

template <class MANIPULATOR>
inline
int MySequence::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_attribute2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
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
        return manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return manipulator(&d_attribute2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;
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

    const bdeat_AttributeInfo *attributeInfo =
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
template <class STREAM>
inline
STREAM& MySequence::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_attribute1, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_attribute2, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
inline
int MySequence::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_attribute2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
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
        return accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return accessor(d_attribute2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;
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

    const bdeat_AttributeInfo *attributeInfo =
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

// CLASS METHODS
inline
int MySequenceWithNillable::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// CREATORS
inline
MySequenceWithNillable::MySequenceWithNillable(bslma_Allocator *basicAllocator)
: d_attribute1()
, d_myNillable(basicAllocator)
, d_attribute2(basicAllocator)
{
}

inline
MySequenceWithNillable::MySequenceWithNillable(
        const MySequenceWithNillable& original,
        bslma_Allocator *basicAllocator)
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

template <class STREAM>
inline
STREAM& MySequenceWithNillable::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_attribute1, 1);
            bdex_InStreamFunctions::streamIn(stream, d_myNillable, 1);
            bdex_InStreamFunctions::streamIn(stream, d_attribute2, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

inline
void MySequenceWithNillable::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_attribute1);
    bdeat_ValueTypeFunctions::reset(&d_myNillable);
    bdeat_ValueTypeFunctions::reset(&d_attribute2);
}

template <class MANIPULATOR>
inline
int MySequenceWithNillable::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_myNillable, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MY_NILLABLE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_attribute2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
inline
int MySequenceWithNillable::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_MY_NILLABLE: {
        return manipulator(&d_myNillable, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MY_NILLABLE]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return manipulator(&d_attribute2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;
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

    const bdeat_AttributeInfo *attributeInfo =
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
bdeut_NullableValue<bsl::string>& MySequenceWithNillable::myNillable()
{
    return d_myNillable;
}

inline
bsl::string& MySequenceWithNillable::attribute2()
{
    return d_attribute2;
}

// ACCESSORS
template <class STREAM>
inline
STREAM& MySequenceWithNillable::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_attribute1, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_myNillable, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_attribute2, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
inline
int MySequenceWithNillable::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_myNillable, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MY_NILLABLE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_attribute2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
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
        return accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_MY_NILLABLE: {
        return accessor(d_myNillable, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MY_NILLABLE]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return accessor(d_attribute2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;
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

    const bdeat_AttributeInfo *attributeInfo =
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
const bdeut_NullableValue<bsl::string>& MySequenceWithNillable::myNillable() const
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
int MyEnumeration::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1
}

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
        return NOT_FOUND;
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
        return "VALUE1";
      } break;
      case VALUE2: {
        return "VALUE2";
      } break;
      default:
        BSLS_ASSERT_SAFE(!"encountered out-of-bound enumerated value");
    }

    return 0;
}

template <class STREAM>
inline
STREAM& MyEnumeration::bdexStreamIn(STREAM&             stream,
                                   MyEnumeration::Value& value,
                                   int                 version)
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

template <class STREAM>
inline
STREAM& MyEnumeration::bdexStreamOut(STREAM&              stream,
                                    MyEnumeration::Value value,
                                    int                version)
{
    switch (version) {
      case 1: {
        stream.putInt32(value);  // Write the value as an int
      } break;
    }
    return stream;
}

                                 // ----------
                                 // class Sqrt
                                 // ----------

// CLASS METHODS
inline
int Sqrt::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

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

template <class STREAM>
inline
STREAM& Sqrt::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_value, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

inline
void Sqrt::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_value);
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
        return manipulator(&d_value, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALUE]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;
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

    const bdeat_AttributeInfo *attributeInfo =
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
template <class STREAM>
inline
STREAM& Sqrt::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_value, 1);
      } break;
    }
    return stream;
}

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
        return NOT_FOUND;
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

    const bdeat_AttributeInfo *attributeInfo =
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
        return FAILURE;
    }

    return SUCCESS;
}

inline
CustomizedString::CustomizedString(bslma_Allocator *basicAllocator)
: d_value(basicAllocator)
{
}

inline
CustomizedString::CustomizedString(const CustomizedString& original, bslma_Allocator *basicAllocator)
: d_value(original.d_value, basicAllocator)
{
}

inline
CustomizedString::CustomizedString(const bsl::string& value, bslma_Allocator *basicAllocator)
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

template <class STREAM>
STREAM& CustomizedString::bdexStreamIn(STREAM& stream, int version)
{
    bsl::string temp;

    bdex_InStreamFunctions::streamIn(stream, temp, version);

    if (!stream) {
        return stream;
    }

    if (fromString(temp)!=0) {
        stream.invalidate();
    }

    return stream;
}

inline
void CustomizedString::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_value);
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
template <class STREAM>
STREAM& CustomizedString::bdexStreamOut(STREAM& stream, int version) const
{
    return bdex_OutStreamFunctions::streamOut(stream, d_value, version);
}

inline
int CustomizedString::maxSupportedBdexVersion() const
{
    return bdex_VersionFunctions::maxSupportedVersion(d_value);
}

inline
bsl::ostream& CustomizedString::print(bsl::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    return bdeu_PrintMethods::print(stream, d_value, level, spacesPerLevel);
}

inline
const bsl::string& CustomizedString::toString() const
{
    return d_value;
}

                             // -----------------
                             // class BasicRecord
                             // -----------------

// CLASS METHODS
inline
int BasicRecord::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// CREATORS
inline
BasicRecord::BasicRecord(bslma_Allocator *basicAllocator)
: d_i1()
, d_i2()
, d_dt()
, d_s(basicAllocator)
{
}

inline
BasicRecord::BasicRecord(
        const BasicRecord& original,
        bslma_Allocator *basicAllocator)
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

template <class STREAM>
inline
STREAM& BasicRecord::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_i1, 1);
            bdex_InStreamFunctions::streamIn(stream, d_i2, 1);
            bdex_InStreamFunctions::streamIn(stream, d_dt, 1);
            bdex_InStreamFunctions::streamIn(stream, d_s, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

inline
void BasicRecord::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_i1);
    bdeat_ValueTypeFunctions::reset(&d_i2);
    bdeat_ValueTypeFunctions::reset(&d_dt);
    bdeat_ValueTypeFunctions::reset(&d_s);
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
        return NOT_FOUND;
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

    const bdeat_AttributeInfo *attributeInfo =
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
bdet_DatetimeTz& BasicRecord::dt()
{
    return d_dt;
}

inline
bsl::string& BasicRecord::s()
{
    return d_s;
}

// ACCESSORS
template <class STREAM>
inline
STREAM& BasicRecord::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_i1, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_i2, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_dt, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_s, 1);
      } break;
    }
    return stream;
}

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
        return NOT_FOUND;
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

    const bdeat_AttributeInfo *attributeInfo =
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
const bdet_DatetimeTz& BasicRecord::dt() const
{
    return d_dt;
}

inline
const bsl::string& BasicRecord::s() const
{
    return d_s;
}

                         // -------------------------
                         // class MySequenceWithArray
                         // -------------------------

// CLASS METHODS
inline
int MySequenceWithArray::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// CREATORS
inline
MySequenceWithArray::MySequenceWithArray(bslma_Allocator *basicAllocator)
: d_attribute1()
, d_attribute2(basicAllocator)
{
}

inline
MySequenceWithArray::MySequenceWithArray(
        const MySequenceWithArray& original,
        bslma_Allocator *basicAllocator)
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

template <class STREAM>
inline
STREAM& MySequenceWithArray::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_attribute1, 1);
            bdex_InStreamFunctions::streamIn(stream, d_attribute2, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

inline
void MySequenceWithArray::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_attribute1);
    bdeat_ValueTypeFunctions::reset(&d_attribute2);
}

template <class MANIPULATOR>
inline
int MySequenceWithArray::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_attribute2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
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
        return manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return manipulator(&d_attribute2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;
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

    const bdeat_AttributeInfo *attributeInfo =
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
template <class STREAM>
inline
STREAM& MySequenceWithArray::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_attribute1, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_attribute2, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
inline
int MySequenceWithArray::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_attribute2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
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
        return accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return accessor(d_attribute2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;
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

    const bdeat_AttributeInfo *attributeInfo =
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

// CLASS METHODS
inline
int MySequenceWithAnonymousChoiceChoice::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// CREATORS
inline
MySequenceWithAnonymousChoiceChoice::MySequenceWithAnonymousChoiceChoice(bslma_Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

inline
MySequenceWithAnonymousChoiceChoice::MySequenceWithAnonymousChoiceChoice(
    const MySequenceWithAnonymousChoiceChoice& original,
    bslma_Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
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
MySequenceWithAnonymousChoiceChoice::operator=(const MySequenceWithAnonymousChoiceChoice& rhs)
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

template <class STREAM>
inline
STREAM& MySequenceWithAnonymousChoiceChoice::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            short selectionId;
            stream.getInt16(selectionId);
            if (!stream) {
                return stream;                                        // RETURN
            }
            switch (selectionId) {
              case SELECTION_ID_MY_CHOICE1: {
                makeMyChoice1();
                bdex_InStreamFunctions::streamIn(
                    stream, d_myChoice1.object(), 1);
              } break;
              case SELECTION_ID_MY_CHOICE2: {
                makeMyChoice2();
                bdex_InStreamFunctions::streamIn(
                    stream, d_myChoice2.object(), 1);
              } break;
              case SELECTION_ID_UNDEFINED: {
                reset();
              } break;
              default:
                stream.invalidate();
            }
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
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
int MySequenceWithAnonymousChoiceChoice::makeSelection(const char *name, int nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdeat_SelectionInfo *selectionInfo =
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
        bdeat_ValueTypeFunctions::reset(&d_myChoice1.object());
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
        bdeat_ValueTypeFunctions::reset(&d_myChoice2.object());
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
bsl::string& MySequenceWithAnonymousChoiceChoice::makeMyChoice2(const bsl::string& value)
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
int MySequenceWithAnonymousChoiceChoice::manipulateSelection(MANIPULATOR& manipulator)
{
    enum { FAILURE = -1, SUCCESS = 0 };

    switch (d_selectionId) {
      case MySequenceWithAnonymousChoiceChoice::SELECTION_ID_MY_CHOICE1:
        return manipulator(&d_myChoice1.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_MY_CHOICE1]);        // RETURN
      case MySequenceWithAnonymousChoiceChoice::SELECTION_ID_MY_CHOICE2:
        return manipulator(&d_myChoice2.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_MY_CHOICE2]);        // RETURN
      default:
        BSLS_ASSERT_SAFE(MySequenceWithAnonymousChoiceChoice::SELECTION_ID_UNDEFINED ==
                     d_selectionId);
        return FAILURE;
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
template <class STREAM>
inline
STREAM& MySequenceWithAnonymousChoiceChoice::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
            stream.putInt16(d_selectionId);
            switch (d_selectionId) {
              case SELECTION_ID_MY_CHOICE1: {
                bdex_OutStreamFunctions::streamOut(
                    stream, d_myChoice1.object(), 1);
              } break;
              case SELECTION_ID_MY_CHOICE2: {
                bdex_OutStreamFunctions::streamOut(
                    stream, d_myChoice2.object(), 1);
              } break;
              default:
                BSLS_ASSERT_SAFE(SELECTION_ID_UNDEFINED == d_selectionId);
            }
      } break;
    }
    return stream;
}

inline
int MySequenceWithAnonymousChoiceChoice::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
inline
int MySequenceWithAnonymousChoiceChoice::accessSelection(ACCESSOR& accessor) const
{
    enum { FAILURE = -1, SUCCESS = 0 };

    switch (d_selectionId) {
      case SELECTION_ID_MY_CHOICE1:
        return accessor(d_myChoice1.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_MY_CHOICE1]);        // RETURN
      case SELECTION_ID_MY_CHOICE2:
        return accessor(d_myChoice2.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_MY_CHOICE2]);        // RETURN
      default:
        BSLS_ASSERT_SAFE(SELECTION_ID_UNDEFINED == d_selectionId);
        return FAILURE;
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

// CLASS METHODS
inline
int Employee::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// CREATORS
inline
Employee::Employee(bslma_Allocator *basicAllocator)
: d_name(basicAllocator)
, d_homeAddress(basicAllocator)
, d_age()
{
}

inline
Employee::Employee(
        const Employee& original,
        bslma_Allocator *basicAllocator)
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
            bdex_InStreamFunctions::streamIn(stream, d_name, 1);
            bdex_InStreamFunctions::streamIn(stream, d_homeAddress, 1);
            bdex_InStreamFunctions::streamIn(stream, d_age, 1);
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
    bdeat_ValueTypeFunctions::reset(&d_name);
    bdeat_ValueTypeFunctions::reset(&d_homeAddress);
    bdeat_ValueTypeFunctions::reset(&d_age);
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

    ret = manipulator(&d_homeAddress, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS]);
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
        return manipulator(&d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_HOME_ADDRESS: {
        return manipulator(&d_homeAddress, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_AGE: {
        return manipulator(&d_age, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;
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

    const bdeat_AttributeInfo *attributeInfo =
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
template <class STREAM>
inline
STREAM& Employee::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_name, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_homeAddress, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_age, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
inline
int Employee::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_homeAddress, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS]);
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
        return accessor(d_homeAddress, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_AGE: {
        return accessor(d_age, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;
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

    const bdeat_AttributeInfo *attributeInfo =
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

// CLASS METHODS
inline
int BigRecord::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// CREATORS
inline
BigRecord::BigRecord(bslma_Allocator *basicAllocator)
: d_name(basicAllocator)
, d_array(basicAllocator)
{
}

inline
BigRecord::BigRecord(
        const BigRecord& original,
        bslma_Allocator *basicAllocator)
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

template <class STREAM>
inline
STREAM& BigRecord::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_name, 1);
            bdex_InStreamFunctions::streamIn(stream, d_array, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

inline
void BigRecord::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_name);
    bdeat_ValueTypeFunctions::reset(&d_array);
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
        return manipulator(&d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ARRAY: {
        return manipulator(&d_array, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ARRAY]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;
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

    const bdeat_AttributeInfo *attributeInfo =
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
template <class STREAM>
inline
STREAM& BigRecord::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_name, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_array, 1);
      } break;
    }
    return stream;
}

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
        return NOT_FOUND;
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

    const bdeat_AttributeInfo *attributeInfo =
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

// CLASS METHODS
inline
int MySequenceWithAnonymousChoice::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// CREATORS
inline
MySequenceWithAnonymousChoice::MySequenceWithAnonymousChoice(bslma_Allocator *basicAllocator)
: d_attribute1()
, d_choice(basicAllocator)
, d_attribute2(basicAllocator)
{
}

inline
MySequenceWithAnonymousChoice::MySequenceWithAnonymousChoice(
        const MySequenceWithAnonymousChoice& original,
        bslma_Allocator *basicAllocator)
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
MySequenceWithAnonymousChoice::operator=(const MySequenceWithAnonymousChoice& rhs)
{
    if (this != &rhs) {
        d_attribute1 = rhs.d_attribute1;
        d_choice = rhs.d_choice;
        d_attribute2 = rhs.d_attribute2;
    }
    return *this;
}

template <class STREAM>
inline
STREAM& MySequenceWithAnonymousChoice::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_attribute1, 1);
            bdex_InStreamFunctions::streamIn(stream, d_choice, 1);
            bdex_InStreamFunctions::streamIn(stream, d_attribute2, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

inline
void MySequenceWithAnonymousChoice::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_attribute1);
    bdeat_ValueTypeFunctions::reset(&d_choice);
    bdeat_ValueTypeFunctions::reset(&d_attribute2);
}

template <class MANIPULATOR>
inline
int MySequenceWithAnonymousChoice::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_attribute2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
inline
int MySequenceWithAnonymousChoice::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_CHOICE: {
        return manipulator(&d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return manipulator(&d_attribute2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;
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

    const bdeat_AttributeInfo *attributeInfo =
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
template <class STREAM>
inline
STREAM& MySequenceWithAnonymousChoice::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_attribute1, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_choice, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_attribute2, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
inline
int MySequenceWithAnonymousChoice::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_attribute2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
inline
int MySequenceWithAnonymousChoice::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_CHOICE: {
        return accessor(d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return accessor(d_attribute2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;
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

    const bdeat_AttributeInfo *attributeInfo =
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
const MySequenceWithAnonymousChoiceChoice& MySequenceWithAnonymousChoice::choice() const
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

// CLASS METHODS
inline
int TimingRequest::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// CREATORS
inline
TimingRequest::TimingRequest(bslma_Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

inline
TimingRequest::TimingRequest(
    const TimingRequest& original,
    bslma_Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
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

template <class STREAM>
inline
STREAM& TimingRequest::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            short selectionId;
            stream.getInt16(selectionId);
            if (!stream) {
                return stream;                                        // RETURN
            }
            switch (selectionId) {
              case SELECTION_ID_SQRT: {
                makeSqrt();
                bdex_InStreamFunctions::streamIn(
                    stream, d_sqrt.object(), 1);
              } break;
              case SELECTION_ID_BASIC: {
                makeBasic();
                bdex_InStreamFunctions::streamIn(
                    stream, d_basic.object(), 1);
              } break;
              case SELECTION_ID_BIG: {
                makeBig();
                bdex_InStreamFunctions::streamIn(
                    stream, d_big.object(), 1);
              } break;
              case SELECTION_ID_UNDEFINED: {
                reset();
              } break;
              default:
                stream.invalidate();
            }
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
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

    const bdeat_SelectionInfo *selectionInfo =
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
        bdeat_ValueTypeFunctions::reset(&d_sqrt.object());
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
        bdeat_ValueTypeFunctions::reset(&d_basic.object());
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
        bdeat_ValueTypeFunctions::reset(&d_big.object());
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
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SQRT]);        // RETURN
      case TimingRequest::SELECTION_ID_BASIC:
        return manipulator(&d_basic.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_BASIC]);        // RETURN
      case TimingRequest::SELECTION_ID_BIG:
        return manipulator(&d_big.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_BIG]);        // RETURN
      default:
        BSLS_ASSERT_SAFE(TimingRequest::SELECTION_ID_UNDEFINED ==
                     d_selectionId);
        return FAILURE;
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
template <class STREAM>
inline
STREAM& TimingRequest::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
            stream.putInt16(d_selectionId);
            switch (d_selectionId) {
              case SELECTION_ID_SQRT: {
                bdex_OutStreamFunctions::streamOut(
                    stream, d_sqrt.object(), 1);
              } break;
              case SELECTION_ID_BASIC: {
                bdex_OutStreamFunctions::streamOut(
                    stream, d_basic.object(), 1);
              } break;
              case SELECTION_ID_BIG: {
                bdex_OutStreamFunctions::streamOut(
                    stream, d_big.object(), 1);
              } break;
              default:
                BSLS_ASSERT_SAFE(SELECTION_ID_UNDEFINED == d_selectionId);
            }
      } break;
    }
    return stream;
}

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
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SQRT]);        // RETURN
      case SELECTION_ID_BASIC:
        return accessor(d_basic.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_BASIC]);        // RETURN
      case SELECTION_ID_BIG:
        return accessor(d_big.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_BIG]);        // RETURN
      default:
        BSLS_ASSERT_SAFE(SELECTION_ID_UNDEFINED == d_selectionId);
        return FAILURE;
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
}  // close package namespace

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
            return true;                                            // RETURN
        }
    }
    else {
        return false;
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
            return true;                                            // RETURN
        }
    }
    else {
        return false;
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
            return true;                                            // RETURN
        }
    }
    else {
        return false;
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

}  // close namespace BloombergLP
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
#include <bdet_datetimetz.h>
#include <bdeut_nullablevalue.h>
#include <bsl_string.h>
#include <bsl_vector.h>

#include <bdeat_formattingmode.h>

#include <bsls_assert.h>
#include <bdeu_chartype.h>
#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_string.h>

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

const bdeat_SelectionInfo MyChoice::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_SELECTION1,
        "selection1",               // name
        sizeof("selection1") - 1,   // name length
        "",  // annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        SELECTION_ID_SELECTION2,
        "selection2",               // name
        sizeof("selection2") - 1,   // name length
        "",  // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    }
};

// CLASS METHODS

const bdeat_SelectionInfo *MyChoice::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 10: {
            if (bdeu_CharType::toUpper(name[0])=='S'
             && bdeu_CharType::toUpper(name[1])=='E'
             && bdeu_CharType::toUpper(name[2])=='L'
             && bdeu_CharType::toUpper(name[3])=='E'
             && bdeu_CharType::toUpper(name[4])=='C'
             && bdeu_CharType::toUpper(name[5])=='T'
             && bdeu_CharType::toUpper(name[6])=='I'
             && bdeu_CharType::toUpper(name[7])=='O'
             && bdeu_CharType::toUpper(name[8])=='N')
            {
                switch(bdeu_CharType::toUpper(name[9])) {
                    case '1': {
                        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1];
                    } break;
                    case '2': {
                        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2];
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdeat_SelectionInfo *MyChoice::lookupSelectionInfo(int id)
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
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);

        switch (d_selectionId) {
          case SELECTION_ID_SELECTION1: {
            stream << "Selection1 = ";
            bdeu_PrintMethods::print(stream, d_selection1.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION2: {
            stream << "Selection2 = ";
            bdeu_PrintMethods::print(stream, d_selection2.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED\n";
        }
        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << "[ ";

        switch (d_selectionId) {
          case SELECTION_ID_SELECTION1: {
            stream << "Selection1 = ";
            bdeu_PrintMethods::print(stream, d_selection1.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION2: {
            stream << "Selection2 = ";
            bdeu_PrintMethods::print(stream, d_selection2.object(),
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

const bdeat_AttributeInfo MySequenceWithNullable::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "attribute1",             // name
        sizeof("attribute1") - 1, // name length
        "",  // annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE2,
        "attribute2",             // name
        sizeof("attribute2") - 1, // name length
        "",  // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *MySequenceWithNullable::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 10: {
            if (bdeu_CharType::toUpper(name[0])=='A'
             && bdeu_CharType::toUpper(name[1])=='T'
             && bdeu_CharType::toUpper(name[2])=='T'
             && bdeu_CharType::toUpper(name[3])=='R'
             && bdeu_CharType::toUpper(name[4])=='I'
             && bdeu_CharType::toUpper(name[5])=='B'
             && bdeu_CharType::toUpper(name[6])=='U'
             && bdeu_CharType::toUpper(name[7])=='T'
             && bdeu_CharType::toUpper(name[8])=='E')
            {
                switch(bdeu_CharType::toUpper(name[9])) {
                    case '1': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
                    } break;
                    case '2': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2];
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *MySequenceWithNullable::lookupAttributeInfo(int id)
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
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute1 = ";
        bdeu_PrintMethods::print(stream, d_attribute1,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute2 = ";
        bdeu_PrintMethods::print(stream, d_attribute2,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Attribute1 = ";
        bdeu_PrintMethods::print(stream, d_attribute1,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Attribute2 = ";
        bdeu_PrintMethods::print(stream, d_attribute2,
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

const bdeat_AttributeInfo Address::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_STREET,
        "street",             // name
        sizeof("street") - 1, // name length
        "",  // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_CITY,
        "city",             // name
        sizeof("city") - 1, // name length
        "",  // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_STATE,
        "state",             // name
        sizeof("state") - 1, // name length
        "",  // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *Address::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 4: {
            if (bdeu_CharType::toUpper(name[0])=='C'
             && bdeu_CharType::toUpper(name[1])=='I'
             && bdeu_CharType::toUpper(name[2])=='T'
             && bdeu_CharType::toUpper(name[3])=='Y')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CITY];
            }
        } break;
        case 5: {
            if (bdeu_CharType::toUpper(name[0])=='S'
             && bdeu_CharType::toUpper(name[1])=='T'
             && bdeu_CharType::toUpper(name[2])=='A'
             && bdeu_CharType::toUpper(name[3])=='T'
             && bdeu_CharType::toUpper(name[4])=='E')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATE];
            }
        } break;
        case 6: {
            if (bdeu_CharType::toUpper(name[0])=='S'
             && bdeu_CharType::toUpper(name[1])=='T'
             && bdeu_CharType::toUpper(name[2])=='R'
             && bdeu_CharType::toUpper(name[3])=='E'
             && bdeu_CharType::toUpper(name[4])=='E'
             && bdeu_CharType::toUpper(name[5])=='T')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STREET];
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *Address::lookupAttributeInfo(int id)
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
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Street = ";
        bdeu_PrintMethods::print(stream, d_street,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "City = ";
        bdeu_PrintMethods::print(stream, d_city,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "State = ";
        bdeu_PrintMethods::print(stream, d_state,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Street = ";
        bdeu_PrintMethods::print(stream, d_street,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "City = ";
        bdeu_PrintMethods::print(stream, d_city,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "State = ";
        bdeu_PrintMethods::print(stream, d_state,
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

const bdeat_AttributeInfo MySequence::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "attribute1",             // name
        sizeof("attribute1") - 1, // name length
        "",  // annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE2,
        "attribute2",             // name
        sizeof("attribute2") - 1, // name length
        "",  // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *MySequence::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 10: {
            if (bdeu_CharType::toUpper(name[0])=='A'
             && bdeu_CharType::toUpper(name[1])=='T'
             && bdeu_CharType::toUpper(name[2])=='T'
             && bdeu_CharType::toUpper(name[3])=='R'
             && bdeu_CharType::toUpper(name[4])=='I'
             && bdeu_CharType::toUpper(name[5])=='B'
             && bdeu_CharType::toUpper(name[6])=='U'
             && bdeu_CharType::toUpper(name[7])=='T'
             && bdeu_CharType::toUpper(name[8])=='E')
            {
                switch(bdeu_CharType::toUpper(name[9])) {
                    case '1': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
                    } break;
                    case '2': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2];
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *MySequence::lookupAttributeInfo(int id)
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
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute1 = ";
        bdeu_PrintMethods::print(stream, d_attribute1,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute2 = ";
        bdeu_PrintMethods::print(stream, d_attribute2,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Attribute1 = ";
        bdeu_PrintMethods::print(stream, d_attribute1,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Attribute2 = ";
        bdeu_PrintMethods::print(stream, d_attribute2,
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

const bdeat_AttributeInfo MySequenceWithNillable::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "attribute1",             // name
        sizeof("attribute1") - 1, // name length
        "",  // annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_MY_NILLABLE,
        "myNillable",             // name
        sizeof("myNillable") - 1, // name length
        "",  // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
      | bdeat_FormattingMode::BDEAT_NILLABLE
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE2,
        "attribute2",             // name
        sizeof("attribute2") - 1, // name length
        "",  // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *MySequenceWithNillable::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 10: {
            switch(bdeu_CharType::toUpper(name[0])) {
                case 'A': {
                    if (bdeu_CharType::toUpper(name[1])=='T'
                     && bdeu_CharType::toUpper(name[2])=='T'
                     && bdeu_CharType::toUpper(name[3])=='R'
                     && bdeu_CharType::toUpper(name[4])=='I'
                     && bdeu_CharType::toUpper(name[5])=='B'
                     && bdeu_CharType::toUpper(name[6])=='U'
                     && bdeu_CharType::toUpper(name[7])=='T'
                     && bdeu_CharType::toUpper(name[8])=='E')
                    {
                        switch(bdeu_CharType::toUpper(name[9])) {
                            case '1': {
                                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
                            } break;
                            case '2': {
                                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2];
                            } break;
                        }
                    }
                } break;
                case 'M': {
                    if (bdeu_CharType::toUpper(name[1])=='Y'
                     && bdeu_CharType::toUpper(name[2])=='N'
                     && bdeu_CharType::toUpper(name[3])=='I'
                     && bdeu_CharType::toUpper(name[4])=='L'
                     && bdeu_CharType::toUpper(name[5])=='L'
                     && bdeu_CharType::toUpper(name[6])=='A'
                     && bdeu_CharType::toUpper(name[7])=='B'
                     && bdeu_CharType::toUpper(name[8])=='L'
                     && bdeu_CharType::toUpper(name[9])=='E')
                    {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MY_NILLABLE];
                    }
                } break;
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *MySequenceWithNillable::lookupAttributeInfo(int id)
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
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute1 = ";
        bdeu_PrintMethods::print(stream, d_attribute1,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "MyNillable = ";
        bdeu_PrintMethods::print(stream, d_myNillable,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute2 = ";
        bdeu_PrintMethods::print(stream, d_attribute2,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Attribute1 = ";
        bdeu_PrintMethods::print(stream, d_attribute1,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "MyNillable = ";
        bdeu_PrintMethods::print(stream, d_myNillable,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Attribute2 = ";
        bdeu_PrintMethods::print(stream, d_attribute2,
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

const bdeat_EnumeratorInfo MyEnumeration::ENUMERATOR_INFO_ARRAY[] = {
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
            if (bdeu_CharType::toUpper(string[0])=='V'
             && bdeu_CharType::toUpper(string[1])=='A'
             && bdeu_CharType::toUpper(string[2])=='L'
             && bdeu_CharType::toUpper(string[3])=='U'
             && bdeu_CharType::toUpper(string[4])=='E') {
                switch(bdeu_CharType::toUpper(string[5])) {
                    case '1': {
                        *result = MyEnumeration::VALUE1;
                        return SUCCESS;                                                       // RETURN
                    } break;
                    case '2': {
                        *result = MyEnumeration::VALUE2;
                        return SUCCESS;                                                       // RETURN
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

const bdeat_AttributeInfo Sqrt::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_VALUE,
        "value",             // name
        sizeof("value") - 1, // name length
        "",  // annotation
        bdeat_FormattingMode::BDEAT_DEFAULT // formatting mode
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *Sqrt::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 5: {
            if (bdeu_CharType::toUpper(name[0])=='V'
             && bdeu_CharType::toUpper(name[1])=='A'
             && bdeu_CharType::toUpper(name[2])=='L'
             && bdeu_CharType::toUpper(name[3])=='U'
             && bdeu_CharType::toUpper(name[4])=='E')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALUE];
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *Sqrt::lookupAttributeInfo(int id)
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
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Value = ";
        bdeu_PrintMethods::print(stream, d_value,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Value = ";
        bdeu_PrintMethods::print(stream, d_value,
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

const bdeat_AttributeInfo BasicRecord::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_I1,
        "i1",             // name
        sizeof("i1") - 1, // name length
        "",  // annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_I2,
        "i2",             // name
        sizeof("i2") - 1, // name length
        "",  // annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_DT,
        "dt",             // name
        sizeof("dt") - 1, // name length
        "",  // annotation
        bdeat_FormattingMode::BDEAT_DEFAULT // formatting mode
    },
    {
        ATTRIBUTE_ID_S,
        "s",             // name
        sizeof("s") - 1, // name length
        "",  // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *BasicRecord::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 1: {
            if (bdeu_CharType::toUpper(name[0])=='S')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_S];
            }
        } break;
        case 2: {
            switch(bdeu_CharType::toUpper(name[0])) {
                case 'D': {
                    if (bdeu_CharType::toUpper(name[1])=='T')
                    {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DT];
                    }
                } break;
                case 'I': {
                    switch(bdeu_CharType::toUpper(name[1])) {
                        case '1': {
                            return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_I1];
                        } break;
                        case '2': {
                            return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_I2];
                        } break;
                    }
                } break;
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *BasicRecord::lookupAttributeInfo(int id)
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
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "I1 = ";
        bdeu_PrintMethods::print(stream, d_i1,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "I2 = ";
        bdeu_PrintMethods::print(stream, d_i2,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Dt = ";
        bdeu_PrintMethods::print(stream, d_dt,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "S = ";
        bdeu_PrintMethods::print(stream, d_s,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "I1 = ";
        bdeu_PrintMethods::print(stream, d_i1,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "I2 = ";
        bdeu_PrintMethods::print(stream, d_i2,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Dt = ";
        bdeu_PrintMethods::print(stream, d_dt,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "S = ";
        bdeu_PrintMethods::print(stream, d_s,
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

const bdeat_AttributeInfo MySequenceWithArray::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "attribute1",             // name
        sizeof("attribute1") - 1, // name length
        "",  // annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE2,
        "attribute2",             // name
        sizeof("attribute2") - 1, // name length
        "",  // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *MySequenceWithArray::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 10: {
            if (bdeu_CharType::toUpper(name[0])=='A'
             && bdeu_CharType::toUpper(name[1])=='T'
             && bdeu_CharType::toUpper(name[2])=='T'
             && bdeu_CharType::toUpper(name[3])=='R'
             && bdeu_CharType::toUpper(name[4])=='I'
             && bdeu_CharType::toUpper(name[5])=='B'
             && bdeu_CharType::toUpper(name[6])=='U'
             && bdeu_CharType::toUpper(name[7])=='T'
             && bdeu_CharType::toUpper(name[8])=='E')
            {
                switch(bdeu_CharType::toUpper(name[9])) {
                    case '1': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
                    } break;
                    case '2': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2];
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *MySequenceWithArray::lookupAttributeInfo(int id)
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
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute1 = ";
        bdeu_PrintMethods::print(stream, d_attribute1,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute2 = ";
        bdeu_PrintMethods::print(stream, d_attribute2,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Attribute1 = ";
        bdeu_PrintMethods::print(stream, d_attribute1,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Attribute2 = ";
        bdeu_PrintMethods::print(stream, d_attribute2,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

                 // -----------------------------------------
                 // class MySequenceWithAnonymousChoiceChoice
                 // -----------------------------------------

// CONSTANTS

const char MySequenceWithAnonymousChoiceChoice::CLASS_NAME[] = "MySequenceWithAnonymousChoiceChoice";
    // the name of this class

const bdeat_SelectionInfo MySequenceWithAnonymousChoiceChoice::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_MY_CHOICE1,
        "myChoice1",               // name
        sizeof("myChoice1") - 1,   // name length
        "",  // annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        SELECTION_ID_MY_CHOICE2,
        "myChoice2",               // name
        sizeof("myChoice2") - 1,   // name length
        "",  // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    }
};

// CLASS METHODS

const bdeat_SelectionInfo *MySequenceWithAnonymousChoiceChoice::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 9: {
            if (bdeu_CharType::toUpper(name[0])=='M'
             && bdeu_CharType::toUpper(name[1])=='Y'
             && bdeu_CharType::toUpper(name[2])=='C'
             && bdeu_CharType::toUpper(name[3])=='H'
             && bdeu_CharType::toUpper(name[4])=='O'
             && bdeu_CharType::toUpper(name[5])=='I'
             && bdeu_CharType::toUpper(name[6])=='C'
             && bdeu_CharType::toUpper(name[7])=='E')
            {
                switch(bdeu_CharType::toUpper(name[8])) {
                    case '1': {
                        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_MY_CHOICE1];
                    } break;
                    case '2': {
                        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_MY_CHOICE2];
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdeat_SelectionInfo *MySequenceWithAnonymousChoiceChoice::lookupSelectionInfo(int id)
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
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);

        switch (d_selectionId) {
          case SELECTION_ID_MY_CHOICE1: {
            stream << "MyChoice1 = ";
            bdeu_PrintMethods::print(stream, d_myChoice1.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_MY_CHOICE2: {
            stream << "MyChoice2 = ";
            bdeu_PrintMethods::print(stream, d_myChoice2.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED\n";
        }
        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << "[ ";

        switch (d_selectionId) {
          case SELECTION_ID_MY_CHOICE1: {
            stream << "MyChoice1 = ";
            bdeu_PrintMethods::print(stream, d_myChoice1.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_MY_CHOICE2: {
            stream << "MyChoice2 = ";
            bdeu_PrintMethods::print(stream, d_myChoice2.object(),
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

const bdeat_AttributeInfo Employee::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_NAME,
        "name",             // name
        sizeof("name") - 1, // name length
        "",  // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_HOME_ADDRESS,
        "homeAddress",             // name
        sizeof("homeAddress") - 1, // name length
        "",  // annotation
        bdeat_FormattingMode::BDEAT_DEFAULT // formatting mode
    },
    {
        ATTRIBUTE_ID_AGE,
        "age",             // name
        sizeof("age") - 1, // name length
        "",  // annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *Employee::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 3: {
            if (bdeu_CharType::toUpper(name[0])=='A'
             && bdeu_CharType::toUpper(name[1])=='G'
             && bdeu_CharType::toUpper(name[2])=='E')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE];
            }
        } break;
        case 4: {
            if (bdeu_CharType::toUpper(name[0])=='N'
             && bdeu_CharType::toUpper(name[1])=='A'
             && bdeu_CharType::toUpper(name[2])=='M'
             && bdeu_CharType::toUpper(name[3])=='E')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME];
            }
        } break;
        case 11: {
            if (bdeu_CharType::toUpper(name[0])=='H'
             && bdeu_CharType::toUpper(name[1])=='O'
             && bdeu_CharType::toUpper(name[2])=='M'
             && bdeu_CharType::toUpper(name[3])=='E'
             && bdeu_CharType::toUpper(name[4])=='A'
             && bdeu_CharType::toUpper(name[5])=='D'
             && bdeu_CharType::toUpper(name[6])=='D'
             && bdeu_CharType::toUpper(name[7])=='R'
             && bdeu_CharType::toUpper(name[8])=='E'
             && bdeu_CharType::toUpper(name[9])=='S'
             && bdeu_CharType::toUpper(name[10])=='S')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS];
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *Employee::lookupAttributeInfo(int id)
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
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Name = ";
        bdeu_PrintMethods::print(stream, d_name,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "HomeAddress = ";
        bdeu_PrintMethods::print(stream, d_homeAddress,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Age = ";
        bdeu_PrintMethods::print(stream, d_age,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Name = ";
        bdeu_PrintMethods::print(stream, d_name,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "HomeAddress = ";
        bdeu_PrintMethods::print(stream, d_homeAddress,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Age = ";
        bdeu_PrintMethods::print(stream, d_age,
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

const bdeat_AttributeInfo BigRecord::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_NAME,
        "name",             // name
        sizeof("name") - 1, // name length
        "",  // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_ARRAY,
        "array",             // name
        sizeof("array") - 1, // name length
        "",  // annotation
        bdeat_FormattingMode::BDEAT_DEFAULT // formatting mode
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *BigRecord::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 4: {
            if (bdeu_CharType::toUpper(name[0])=='N'
             && bdeu_CharType::toUpper(name[1])=='A'
             && bdeu_CharType::toUpper(name[2])=='M'
             && bdeu_CharType::toUpper(name[3])=='E')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME];
            }
        } break;
        case 5: {
            if (bdeu_CharType::toUpper(name[0])=='A'
             && bdeu_CharType::toUpper(name[1])=='R'
             && bdeu_CharType::toUpper(name[2])=='R'
             && bdeu_CharType::toUpper(name[3])=='A'
             && bdeu_CharType::toUpper(name[4])=='Y')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ARRAY];
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *BigRecord::lookupAttributeInfo(int id)
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
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Name = ";
        bdeu_PrintMethods::print(stream, d_name,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Array = ";
        bdeu_PrintMethods::print(stream, d_array,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Name = ";
        bdeu_PrintMethods::print(stream, d_name,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Array = ";
        bdeu_PrintMethods::print(stream, d_array,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

                    // -----------------------------------
                    // class MySequenceWithAnonymousChoice
                    // -----------------------------------

// CONSTANTS

const char MySequenceWithAnonymousChoice::CLASS_NAME[] = "MySequenceWithAnonymousChoice";
    // the name of this class

const bdeat_AttributeInfo MySequenceWithAnonymousChoice::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "attribute1",             // name
        sizeof("attribute1") - 1, // name length
        "",  // annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_CHOICE,
        "Choice",             // name
        sizeof("Choice") - 1, // name length
        "",  // annotation
        bdeat_FormattingMode::BDEAT_DEFAULT // formatting mode
      | bdeat_FormattingMode::BDEAT_UNTAGGED
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE2,
        "attribute2",             // name
        sizeof("attribute2") - 1, // name length
        "",  // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *MySequenceWithAnonymousChoice::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    if (bdeu_String::areEqualCaseless("myChoice1", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
    }

    if (bdeu_String::areEqualCaseless("myChoice2", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
    }

    switch(nameLength) {
        case 6: {
            if (bdeu_CharType::toUpper(name[0])=='C'
             && bdeu_CharType::toUpper(name[1])=='H'
             && bdeu_CharType::toUpper(name[2])=='O'
             && bdeu_CharType::toUpper(name[3])=='I'
             && bdeu_CharType::toUpper(name[4])=='C'
             && bdeu_CharType::toUpper(name[5])=='E')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
            }
        } break;
        case 10: {
            if (bdeu_CharType::toUpper(name[0])=='A'
             && bdeu_CharType::toUpper(name[1])=='T'
             && bdeu_CharType::toUpper(name[2])=='T'
             && bdeu_CharType::toUpper(name[3])=='R'
             && bdeu_CharType::toUpper(name[4])=='I'
             && bdeu_CharType::toUpper(name[5])=='B'
             && bdeu_CharType::toUpper(name[6])=='U'
             && bdeu_CharType::toUpper(name[7])=='T'
             && bdeu_CharType::toUpper(name[8])=='E')
            {
                switch(bdeu_CharType::toUpper(name[9])) {
                    case '1': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
                    } break;
                    case '2': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2];
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *MySequenceWithAnonymousChoice::lookupAttributeInfo(int id)
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
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute1 = ";
        bdeu_PrintMethods::print(stream, d_attribute1,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Choice = ";
        bdeu_PrintMethods::print(stream, d_choice,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute2 = ";
        bdeu_PrintMethods::print(stream, d_attribute2,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Attribute1 = ";
        bdeu_PrintMethods::print(stream, d_attribute1,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Choice = ";
        bdeu_PrintMethods::print(stream, d_choice,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Attribute2 = ";
        bdeu_PrintMethods::print(stream, d_attribute2,
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

const bdeat_SelectionInfo TimingRequest::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_SQRT,
        "sqrt",               // name
        sizeof("sqrt") - 1,   // name length
        "",  // annotation
        bdeat_FormattingMode::BDEAT_DEFAULT // formatting mode
    },
    {
        SELECTION_ID_BASIC,
        "basic",               // name
        sizeof("basic") - 1,   // name length
        "",  // annotation
        bdeat_FormattingMode::BDEAT_DEFAULT // formatting mode
    },
    {
        SELECTION_ID_BIG,
        "big",               // name
        sizeof("big") - 1,   // name length
        "",  // annotation
        bdeat_FormattingMode::BDEAT_DEFAULT // formatting mode
    }
};

// CLASS METHODS

const bdeat_SelectionInfo *TimingRequest::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 3: {
            if (bdeu_CharType::toUpper(name[0])=='B'
             && bdeu_CharType::toUpper(name[1])=='I'
             && bdeu_CharType::toUpper(name[2])=='G')
            {
                return &SELECTION_INFO_ARRAY[SELECTION_INDEX_BIG];
            }
        } break;
        case 4: {
            if (bdeu_CharType::toUpper(name[0])=='S'
             && bdeu_CharType::toUpper(name[1])=='Q'
             && bdeu_CharType::toUpper(name[2])=='R'
             && bdeu_CharType::toUpper(name[3])=='T')
            {
                return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SQRT];
            }
        } break;
        case 5: {
            if (bdeu_CharType::toUpper(name[0])=='B'
             && bdeu_CharType::toUpper(name[1])=='A'
             && bdeu_CharType::toUpper(name[2])=='S'
             && bdeu_CharType::toUpper(name[3])=='I'
             && bdeu_CharType::toUpper(name[4])=='C')
            {
                return &SELECTION_INFO_ARRAY[SELECTION_INDEX_BASIC];
            }
        } break;
    }
    return 0;
}

const bdeat_SelectionInfo *TimingRequest::lookupSelectionInfo(int id)
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
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);

        switch (d_selectionId) {
          case SELECTION_ID_SQRT: {
            stream << "Sqrt = ";
            bdeu_PrintMethods::print(stream, d_sqrt.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_BASIC: {
            stream << "Basic = ";
            bdeu_PrintMethods::print(stream, d_basic.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_BIG: {
            stream << "Big = ";
            bdeu_PrintMethods::print(stream, d_big.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED\n";
        }
        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << "[ ";

        switch (d_selectionId) {
          case SELECTION_ID_SQRT: {
            stream << "Sqrt = ";
            bdeu_PrintMethods::print(stream, d_sqrt.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_BASIC: {
            stream << "Basic = ";
            bdeu_PrintMethods::print(stream, d_basic.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_BIG: {
            stream << "Big = ";
            bdeu_PrintMethods::print(stream, d_big.object(),
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
}  // close namespace BloombergLP

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

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------

// The following snippets of code illustrate the usage of this component.
// Suppose we have the following XML schema inside a file called 'xsdfile.xsd':
//..
//  <?xml version='1.0' encoding='UTF-8'?>
//  <xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'
//             xmlns:bdem='http://bloomberg.com/schemas/bdem'
//             elementFormDefault='unqualified'>
//
//      <xs:complexType name='Address'>
//          <xs:sequence>
//              <xs:element name='street' type='string'/>
//              <xs:element name='city'   type='string'/>
//              <xs:element name='state'  type='string'/>
//          </xs:sequence>
//      </xs:complexType>
//
//      <xs:complexType name='Employee'>
//          <xs:sequence>
//              <xs:element name='name'        type='string'/>
//              <xs:element name='homeAddress' type='Address'/>
//              <xs:element name='age'         type='int'/>
//          </xs:sequence>
//      </xs:complexType>
//
//  </xs:schema>
//..
// Using the 'bde_xsdcc.pl' tool, we can generate C++ classes for this schema:
//..
//  $ bde_xsdcc.pl -g h -g cpp -p test xsdfile.xsd
//..
// This tool will generate the header and implementation files for the
// 'test_address' and 'test_employee' components in the current directory.
//
// Now suppose we wanted to encode information about a particular employee
// using BER encoding.  The following function will do this:
//..
void usageExample()
{
    bdesb_MemOutStreamBuf osb;

    test::Employee bob;

    bob.name()                 = "Bob";
    bob.homeAddress().street() = "Some Street";
    bob.homeAddress().city()   = "Some City";
    bob.homeAddress().state()  = "Some State";
    bob.age()                  = 21;

    bdem_BerEncoder encoder(0);
    int retCode = encoder.encode(&osb, bob);

    ASSERT(0 == retCode);
//..
// Now we will verify the contents of 'osb' using the 'bdem_berdecoderutil'
// component:
//..
//      bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
//      test::Employee            obj;
//
//      retCode = bdem_BerDecoder::decode(&isb, &obj);
//
//      ASSERT(0                          == retCode);
//      ASSERT(bob.name()                 == obj.name());
//      ASSERT(bob.homeAddress().street() == obj.homeAddress().street());
//      ASSERT(bob.homeAddress().city()   == obj.homeAddress().city());
//      ASSERT(bob.homeAddress().state()  == obj.homeAddress().state());
//      ASSERT(bob.age()                  == obj.age());
}
//..

void printDiagnostic(bdem_BerEncoder & encoder)
{
    if (veryVerbose) {
        bsl::cout << encoder.loggedMessages();
    }
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    bdem_BerEncoder encoder(0);

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 15: {
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

        if (verbose) bsl::cout << "\nTesting bdet_Date"
                               << "\n=================" << bsl::endl;

        {
            const int YEAR = 2020, MONTH = 3, DAY = 1;

            bdet_Date d(YEAR, MONTH, DAY);

            bdem_BerEncoderOptions options;
            options.setEncodeDateAndTimeTypesAsBinary(true);

            bdesb_MemOutStreamBuf osb;
            bdem_BerEncoder encoder(&options);
            ASSERT(0 == encoder.encode(&osb, d));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }

        if (verbose) bsl::cout << "\nTesting bdet_Date"
                               << "\n=================" << bsl::endl;

        {
            const int YEAR = 1990, MONTH = 12, DAY = 31;

            bdet_Date d(YEAR, MONTH, DAY);

            bdem_BerEncoderOptions options;
            options.setEncodeDateAndTimeTypesAsBinary(true);

            bdesb_MemOutStreamBuf osb;
            bdem_BerEncoder encoder(&options);
            ASSERT(0 == encoder.encode(&osb, d));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }

        if (verbose) bsl::cout << "\nTesting bdet_Date"
                               << "\n=================" << bsl::endl;

        {
            const int YEAR = 1, MONTH = 1, DAY = 1;

            bdet_Date d(YEAR, MONTH, DAY);

            bdem_BerEncoderOptions options;
            options.setEncodeDateAndTimeTypesAsBinary(true);

            bdesb_MemOutStreamBuf osb;
            bdem_BerEncoder encoder(&options);
            ASSERT(0 == encoder.encode(&osb, d));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }

        if (verbose) bsl::cout << "\nTesting bdet_DateTz"
                               << "\n=================" << bsl::endl;

        {
            const int YEAR = 2020, MONTH = 3, DAY = 1;

            bdet_DateTz dt(bdet_Date(YEAR, MONTH, DAY), 0);
 
            bdem_BerEncoderOptions options;
            options.setEncodeDateAndTimeTypesAsBinary(true);

            bdesb_MemOutStreamBuf osb;
            bdem_BerEncoder encoder(&options);
            ASSERT(0 == encoder.encode(&osb, dt));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }

        if (verbose) bsl::cout << "\nTesting bdet_DateTz"
                               << "\n===================" << bsl::endl;

        {
            const int YEAR = 1950, MONTH = 12, DAY = 31;

            bdet_DateTz dt(bdet_Date(YEAR, MONTH, DAY), 0);
 
            bdem_BerEncoderOptions options;
            options.setEncodeDateAndTimeTypesAsBinary(true);

            bdesb_MemOutStreamBuf osb;
            bdem_BerEncoder encoder(&options);
            ASSERT(0 == encoder.encode(&osb, dt));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }

        if (verbose) bsl::cout << "\nTesting bdet_DateTz"
                               << "\n=================" << bsl::endl;

        {
            const int YEAR = 1800, MONTH = 12, DAY = 31;

            bdet_DateTz dt(bdet_Date(YEAR, MONTH, DAY), 0);
 
            bdem_BerEncoderOptions options;
            options.setEncodeDateAndTimeTypesAsBinary(true);

            bdesb_MemOutStreamBuf osb;
            bdem_BerEncoder encoder(&options);
            ASSERT(0 == encoder.encode(&osb, dt));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Usage Example"
                               << "\n=====================" << bsl::endl;

        usageExample();

        if (verbose) bsl::cout << "\nEnd of test." << bsl::endl;
      } break;
      case 13: {
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

        if (verbose) bsl::cout << "\nTesting bdet_Date"
                               << "\n=================" << bsl::endl;

        {
            const int YEAR = 2005, MONTH = 12, DAY = 15;

            bdet_Date d(YEAR, MONTH, DAY);

            bdem_BerEncoderOptions options;
            options.setEncodeDateAndTimeTypesAsBinary(true);

            bdesb_MemOutStreamBuf osb;
            bdem_BerEncoder encoder(&options);
            ASSERT(0 == encoder.encode(&osb, d));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }

        if (verbose) bsl::cout << "\nTesting bdet_Date"
                               << "\n=================" << bsl::endl;

        {
            const int YEAR = 2020, MONTH = 1, DAY = 1;

            bdet_Date d(YEAR, MONTH, DAY);

            bdem_BerEncoderOptions options;
            options.setEncodeDateAndTimeTypesAsBinary(true);

            bdesb_MemOutStreamBuf osb;
            bdem_BerEncoder encoder(&options);
            ASSERT(0 == encoder.encode(&osb, d));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }

        if (verbose) bsl::cout << "\nTesting bdet_DateTz"
                               << "\n===================" << bsl::endl;

        {
            const int YEAR = 2005, MONTH = 12, DAY = 15, OFFSET = 45;

            bdet_DateTz dt(bdet_Date(YEAR, MONTH, DAY), OFFSET);

            bdem_BerEncoderOptions options;
            options.setEncodeDateAndTimeTypesAsBinary(true);

            bdesb_MemOutStreamBuf osb;
            bdem_BerEncoder encoder(&options);
            ASSERT(0 == encoder.encode(&osb, dt));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }

        if (verbose) bsl::cout << "\nTesting bdet_DateTz"
                               << "\n===================" << bsl::endl;

        {
            const int YEAR = 2020, MONTH = 1, DAY = 1, OFFSET = 0;

            bdet_DateTz dt(bdet_Date(YEAR, MONTH, DAY), OFFSET);

            bdem_BerEncoderOptions options;
            options.setEncodeDateAndTimeTypesAsBinary(true);

            bdesb_MemOutStreamBuf osb;
            bdem_BerEncoder encoder(&options);
            ASSERT(0 == encoder.encode(&osb, dt));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }

        if (verbose) bsl::cout << "\nTesting bdet_Time"
                               << "\n=================" << bsl::endl;

        {
            const int HOUR = 12, MIN = 56, SECS = 9, MILLISECS = 134;

            bdet_Time t(HOUR, MIN, SECS, MILLISECS);

            bdem_BerEncoderOptions options;
            options.setEncodeDateAndTimeTypesAsBinary(true);

            bdesb_MemOutStreamBuf osb;
            bdem_BerEncoder encoder(&options);
            ASSERT(0 == encoder.encode(&osb, t));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }

        if (verbose) bsl::cout << "\nTesting bdet_TimeTz"
                               << "\n===================" << bsl::endl;

        {
            const int HOUR   = 12, MIN = 56, SECS = 9, MILLISECS = 134,
                      OFFSET = 45;

            bdet_TimeTz t(bdet_Time(HOUR, MIN, SECS, MILLISECS), OFFSET);

            bdem_BerEncoderOptions options;
            options.setEncodeDateAndTimeTypesAsBinary(true);

            bdesb_MemOutStreamBuf osb;
            bdem_BerEncoder encoder(&options);

            ASSERT(0 == encoder.encode(&osb, t));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }

        if (verbose) bsl::cout << "\nTesting bdet_Datetime"
                               << "\n=====================" << bsl::endl;

        {
            const int YEAR = 2005, MONTH = 12, DAY = 15;
            const int HOUR = 12, MIN = 56, SECS = 9, MILLISECS = 134;

            bdet_Datetime dt(YEAR, MONTH, DAY, HOUR, MIN, SECS, MILLISECS);

            bdem_BerEncoderOptions options;
            options.setEncodeDateAndTimeTypesAsBinary(true);

            bdesb_MemOutStreamBuf osb;
            bdem_BerEncoder encoder(&options);

            ASSERT(0 == encoder.encode(&osb, dt));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }

        if (verbose) bsl::cout << "\nTesting bdet_DatetimeTz"
                               << "\n=======================" << bsl::endl;

        {
            const int YEAR   = 2005, MONTH = 12, DAY = 15;
            const int HOUR   = 12, MIN = 56, SECS = 9, MILLISECS = 134;
            const int OFFSET = 45;

            bdet_DatetimeTz dt(bdet_Datetime(YEAR, MONTH, DAY,
                                             HOUR, MIN, SECS, MILLISECS),
                               OFFSET);

            bdem_BerEncoderOptions options;
            options.setEncodeDateAndTimeTypesAsBinary(true);

            bdesb_MemOutStreamBuf osb;
            bdem_BerEncoder encoder(&options);

            ASSERT(0 == encoder.encode(&osb, dt));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }

        if (verbose) bsl::cout << "\nTesting bdet_DatetimeTz"
                               << "\n=======================" << bsl::endl;

        {
            const int YEAR   = 2012, MONTH = 3, DAY = 1;
            const int HOUR   = 9, MIN = 0, SECS = 0, MILLISECS = 0;
            const int OFFSET = -300;

            bdet_DatetimeTz dt(bdet_Datetime(YEAR, MONTH, DAY,
                                             HOUR, MIN, SECS, MILLISECS),
                               OFFSET);

            bdem_BerEncoderOptions options;
            options.setEncodeDateAndTimeTypesAsBinary(true);

            bdesb_MemOutStreamBuf osb;
            bdem_BerEncoder encoder(&options);

            ASSERT(0 == encoder.encode(&osb, dt));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING ARRAYS WITH the 'encodeEmptyArrays' option (DRQS 29114951)
        //
        // Concerns:
        //: 1 If 'bdem_BerEncoderOptions' is not specified then empty arrays
        //:   are encoded.
        //:
        //: 2 If 'bdem_BerEncoderOptions' is specified but the
        //:   'encodeEmptyArrays' is set to 'false' then empty arrays
        //:   are not encoded.
        //:
        //: 3 If 'bdem_BerEncoderOptions' is specified and the
        //:   'encodeEmptyArrays' option is not set or set to 'true' then
        //:   empty arrays are encoded.
        //:
        //: 4 Non-empty arrays are always encoded.
        //
        // Plan:
        //: 1 Create three 'bdem_BerEncoderOptions' objects.  Set the
        //:   'encodeEmptyArrays' option in one encoder options object to
        //:   'true' and to 'false' in the another object.  Leave the third
        //:   encoder options object unmodified.
        //:
        //: 2 Create four 'bdem_BerEncoder' objects passing the three
        //:   'bdem_BerEncoderOptions' objects created in step 1 to the first
        //:   three encoder objects.  The fourth encoder object is not passed
        //:   any encoder options.
        //:
        //: 3 Create four 'bdesb_MemOutStreamBuf' objects.
        //:
        //: 4 Populate a 'MySequenceWithArray' object ensuring that its
        //:   underlying vector data member is empty.
        //:
        //: 5 Encode the 'MySequenceWithArray' object onto a
        //:   'bdesb_MemOutStreamBuf' using one of the created
        //:   'bdem_BerEncoder' objects.
        //:
        //: 6 Ensure that the empty vector is encoded in all cases except when
        //:   the encoder options are explicitly provided and the
        //:   'encodeEmptyArrays' option on that object is set to 'false'.
        //:
        //: 7 Repeat steps 1 - 6 for a 'MySequenceWithArray' object that has a
        //:   non-empty vector.
        //:
        //: 8 Ensure that the non-empty vector is encoded in all cases.
        //
        // Testing:
        //  Encoding of vectors
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Arrays with 'encodeEmptyArrays'"
                               << "\n======================================="
                               << bsl::endl;

        if (verbose) bsl::cout << "\nTesting with empty array." << bsl::endl;
        {
            bdem_BerEncoderOptions options1, options2, options3;
            options1.setEncodeEmptyArrays(true);
            options2.setEncodeEmptyArrays(false);

            bdem_BerEncoder encoder1(&options1), encoder2(&options2),
                            encoder3(&options3), encoder4;

            bdesb_MemOutStreamBuf osb1, osb2, osb3, osb4;

            test::MySequenceWithArray value;
            value.attribute1() = 34;

            ASSERT(0 == encoder1.encode(&osb1, value));
            ASSERT(0 == encoder2.encode(&osb2, value));
            ASSERT(0 == encoder3.encode(&osb3, value));
            ASSERT(0 == encoder4.encode(&osb4, value));

            ASSERT(osb1.length()  > osb2.length());
            ASSERT(osb1.length() == osb3.length());
            ASSERT(osb1.length() == osb4.length());
            ASSERT(0 == memcmp(osb1.data(), osb3.data(), osb1.length()));
            ASSERT(0 == memcmp(osb1.data(), osb4.data(), osb1.length()));

            printDiagnostic(encoder1);
            printDiagnostic(encoder2);
            printDiagnostic(encoder3);
            printDiagnostic(encoder4);

            if (veryVerbose) {
                P(osb1.length())
                printBuffer(osb1.data(), osb1.length());

                P(osb2.length())
                printBuffer(osb2.data(), osb2.length());

                P(osb3.length())
                printBuffer(osb3.data(), osb3.length());

                P(osb4.length())
                printBuffer(osb4.data(), osb4.length());
            }
        }

        if (verbose) bsl::cout << "\nTesting with a non-empty array."
                               << bsl::endl;
        {
            bdem_BerEncoderOptions options1, options2, options3;
            options1.setEncodeEmptyArrays(true);
            options2.setEncodeEmptyArrays(false);

            bdem_BerEncoder encoder1(&options1), encoder2(&options2),
                            encoder3(&options3), encoder4;

            bdesb_MemOutStreamBuf osb1, osb2, osb3, osb4;

            test::MySequenceWithArray value;
            value.attribute1() = 34;
            value.attribute2().push_back("Hello");
            value.attribute2().push_back("World!");

            ASSERT(0 == encoder1.encode(&osb1, value));
            ASSERT(0 == encoder2.encode(&osb2, value));
            ASSERT(0 == encoder3.encode(&osb3, value));
            ASSERT(0 == encoder4.encode(&osb4, value));

            ASSERT(osb1.length() == osb2.length())
            ASSERT(osb1.length() == osb3.length())
            ASSERT(osb1.length() == osb4.length())
            ASSERT(0 == memcmp(osb1.data(), osb2.data(), osb1.length()));
            ASSERT(0 == memcmp(osb1.data(), osb3.data(), osb1.length()));
            ASSERT(0 == memcmp(osb1.data(), osb4.data(), osb1.length()));

            printDiagnostic(encoder1);
            printDiagnostic(encoder2);
            printDiagnostic(encoder3);
            printDiagnostic(encoder4);

            if (veryVerbose) {
                P(osb1.length())
                printBuffer(osb1.data(), osb1.length());

                P(osb2.length())
                printBuffer(osb2.data(), osb2.length());

                P(osb3.length())
                printBuffer(osb3.data(), osb3.length());

                P(osb4.length())
                printBuffer(osb4.data(), osb4.length());
            }
        }

        if (verbose) bsl::cout << "\nEnd of test." << bsl::endl;
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
            bdesb_MemOutStreamBuf osb;

            test::MySequenceWithNillable value;
            value.attribute1() = 34;
            value.attribute2() = "Hello";

            ASSERT(0 == encoder.encode(&osb, value));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }

        if (verbose) bsl::cout << "\nTesting with non-null value."
                               << bsl::endl;
        {
            bdesb_MemOutStreamBuf osb;

            test::MySequenceWithNillable value;
            value.attribute1() = 34;
            value.myNillable() = "World!";
            value.attribute2() = "Hello";

            ASSERT(0 == encoder.encode(&osb, value));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
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

        if (verbose) bsl::cout << "\nTesting Anonymous Choice"
                               << "\n========================" << bsl::endl;

        if (verbose) bsl::cout << "\nTesting with no selection." << bsl::endl;
        {
            bdesb_MemOutStreamBuf osb;

            test::MySequenceWithAnonymousChoice value;
            value.attribute1() = 34;
            value.attribute2() = "Hello";

            ASSERT(0 == encoder.encode(&osb, value));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }

        if (verbose) bsl::cout << "\nTesting with selection." << bsl::endl;
        {
            bdesb_MemOutStreamBuf osb;

            test::MySequenceWithAnonymousChoice value;
            value.attribute1() = 34;
            value.choice().makeMyChoice1(58);
            value.attribute2() = "Hello";

            ASSERT(0 == encoder.encode(&osb, value));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }
        {
            bdesb_MemOutStreamBuf osb;

            test::MySequenceWithAnonymousChoice value;
            value.attribute1() = 34;
            value.choice().makeMyChoice2("World!");
            value.attribute2() = "Hello";

            ASSERT(0 == encoder.encode(&osb, value));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
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
            bdesb_MemOutStreamBuf osb;

            test::MySequenceWithArray value;
            value.attribute1() = 34;

            ASSERT(0 == encoder.encode(&osb, value));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }

        if (verbose) bsl::cout << "\nTesting with non-empty array."
                               << bsl::endl;
        {
            bdesb_MemOutStreamBuf osb;

            test::MySequenceWithArray value;
            value.attribute1() = 34;
            value.attribute2().push_back("Hello");
            value.attribute2().push_back("World!");

            ASSERT(0 == encoder.encode(&osb, value));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
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
            bdesb_MemOutStreamBuf osb;

            test::MySequenceWithNullable value;
            value.attribute1() = 34;

            ASSERT(0 == encoder.encode(&osb, value));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }

        if (verbose) bsl::cout << "\nTesting with non-null value."
                               << bsl::endl;
        {
            bdesb_MemOutStreamBuf osb;

            test::MySequenceWithNullable value;
            value.attribute1() = 34;
            value.attribute2().makeValue("Hello");

            ASSERT(0 == encoder.encode(&osb, value));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
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
            bdesb_MemOutStreamBuf osb;

            test::MyChoice value;

            ASSERT(0 == encoder.encode(&osb, value));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }

        if (verbose) bsl::cout << "\nTesting with selection." << bsl::endl;
        {
            bdesb_MemOutStreamBuf osb;

            test::MyChoice value;
            value.makeSelection1();
            value.selection1() = 34;

            ASSERT(0 == encoder.encode(&osb, value));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
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

        bdesb_MemOutStreamBuf osb;

        test::MySequence value;
        value.attribute1() = 34;
        value.attribute2() = "Hello";

        ASSERT(0 == encoder.encode(&osb, value));
        printDiagnostic(encoder);

        if (veryVerbose) {
            P(osb.length())
            printBuffer(osb.data(), osb.length());
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

        bdesb_MemOutStreamBuf osb;

        test::MyEnumeration::Value value = test::MyEnumeration::VALUE1;

        ASSERT(0 == encoder.encode(&osb, value));
        printDiagnostic(encoder);

        if (veryVerbose) {
            P(osb.length())
            printBuffer(osb.data(), osb.length());
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

        bdesb_MemOutStreamBuf osb1, osb2;

        const bsl::string VALUE = "Hello";

        if (verbose) bsl::cout << "\nEncoding customized string." << bsl::endl;
        {
            test::CustomizedString value;
            value.fromString(VALUE);

            ASSERT(0 == encoder.encode(&osb1, value));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb1.length())
                printBuffer(osb1.data(), osb1.length());
            }
        }

        if (verbose) bsl::cout << "\nEncoding bsl::string (control)."
                               << bsl::endl;
        {
            bsl::string value = VALUE;

            ASSERT(0 == encoder.encode(&osb2, value));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb2.length())
                printBuffer(osb2.data(), osb2.length());
            }
        }

        LOOP2_ASSERT(osb1.length(),   osb2.length(),
                     osb1.length() == osb2.length());
        ASSERT(0 == bsl::memcmp(osb1.data(), osb2.data(), osb1.length()));

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

        if (verbose) bsl::cout << "\nTesting bdet_Date"
                               << "\n=================" << bsl::endl;

        {
            const int YEAR = 2005, MONTH = 12, DAY = 15;

            bdet_Date d(YEAR, MONTH, DAY);

            bdesb_MemOutStreamBuf osb;
            ASSERT(0 == encoder.encode(&osb, d));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }

        if (verbose) bsl::cout << "\nTesting bdet_DateTz"
                               << "\n===================" << bsl::endl;

        {
            const int YEAR = 2005, MONTH = 12, DAY = 15, OFFSET = 45;

            bdet_DateTz d(bdet_Date(YEAR, MONTH, DAY), OFFSET);

            bdesb_MemOutStreamBuf osb;
            ASSERT(0 == encoder.encode(&osb, d));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }

        if (verbose) bsl::cout << "\nTesting bdet_Time"
                               << "\n=================" << bsl::endl;

        {
            const int HOUR = 12, MIN = 56, SECS = 9, MILLISECS = 134;

            bdet_Time t(HOUR, MIN, SECS, MILLISECS);

            bdesb_MemOutStreamBuf osb;
            ASSERT(0 == encoder.encode(&osb, t));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }

        if (verbose) bsl::cout << "\nTesting bdet_TimeTz"
                               << "\n===================" << bsl::endl;

        {
            const int HOUR   = 12, MIN = 56, SECS = 9, MILLISECS = 134,
                      OFFSET = 45;

            bdet_TimeTz t(bdet_Time(HOUR, MIN, SECS, MILLISECS), OFFSET);

            bdesb_MemOutStreamBuf osb;
            ASSERT(0 == encoder.encode(&osb, t));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }

        if (verbose) bsl::cout << "\nTesting bdet_Datetime"
                               << "\n=====================" << bsl::endl;

        {
            const int YEAR = 2005, MONTH = 12, DAY = 15;
            const int HOUR = 12, MIN = 56, SECS = 9, MILLISECS = 134;

            bdet_Datetime dt(YEAR, MONTH, DAY, HOUR, MIN, SECS, MILLISECS);

            bdesb_MemOutStreamBuf osb;
            ASSERT(0 == encoder.encode(&osb, dt));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
        }

        if (verbose) bsl::cout << "\nTesting bdet_DatetimeTz"
                               << "\n=======================" << bsl::endl;

        {
            const int YEAR   = 2005, MONTH = 12, DAY = 15;
            const int HOUR   = 12, MIN = 56, SECS = 9, MILLISECS = 134;
            const int OFFSET = 45;

            bdet_DatetimeTz dt(bdet_Datetime(YEAR, MONTH, DAY,
                                             HOUR, MIN, SECS, MILLISECS),
                               OFFSET);

            bdesb_MemOutStreamBuf osb;
            ASSERT(0 == encoder.encode(&osb, dt));
            printDiagnostic(encoder);

            if (veryVerbose) {
                P(osb.length())
                printBuffer(osb.data(), osb.length());
            }
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
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nREAL TEST"
                               << "\n=========" << bsl::endl;

        {
            static const struct {
                int         d_lineNum; // source line number
                double      d_value;   // real value
                const char *d_exp;     // encoded result
            } REAL[] = {
                //line  value               buffer
                //----  -----               ------
                { L_,   0,             "09 00" },
                { L_,   1.25,          "09 03 80 FE 05" },
                { L_,   1.1,           "09 09 80 CD 08 CC CC CC CC CC CD" },
                { L_,   0.0176,        "09 09 80 C6 12 05 BC 01 A3 6E 2F" },
                { L_,   -7.8752345,    "09 09 C0 CE 1F 80 3D 79 07 52 DB" },
                { L_,   99.234,
                                       "09 09 80 D2 18 CE F9 DB 22 D0 E5" },
                { L_,   -100.987,
                                       "09 09 C0 D3 0C 9F 95 81 06 24 DD" },
                { L_,   -77723.875,    "09 05 C0 FD 09 7C DF"             },
                { L_,   19998989.1234, "09 09 80 E4 13 12 90 D1 F9 72 47"    },
                { L_,   79879879249686698E-100,
                                       "09 0A 81 FE B7 1F 09 39 59 03 93 B3" },
                { L_,   -9999999999999999E25,
                                       "09 09 C0 55 09 2E FD 1B 8D 0C F3"    },

                // Single precision denormalized numbers
                { L_,   1.4E-45,       "09 0A 81 FF 37 0F FC 34 5F A6 CA B5" },
                { L_,   -1.4E-45,      "09 0A C1 FF 37 0F FC 34 5F A6 CA B5" },

                // Double precision denormalized numbers
                { L_,   1E-314,        "09 07 81 FB CE 78 A4 22 05" },
                { L_,   -1E-314,       "09 07 C1 FB CE 78 A4 22 05" },

                // FLT_MAX & FLT_MIN
                { L_,   1.175494351E-38,
                                       "09 0A 81 FF 4E 10 00 00 00 0A 63 9B" },
                { L_,   3.402823466E+38,
                                       "09 09 80 4C 0F FF FF EF F8 38 1B" },

            };

            const int NUM_DATA = sizeof REAL / sizeof *REAL;

            if (verbose) { cout << "\nTesting normal real values" << endl; }

            for (int di = 0; di < NUM_DATA; ++di) {
                const int     LINE  = REAL[di].d_lineNum;
                const double  VALUE = REAL[di].d_value;
                const char   *EXP   = REAL[di].d_exp;

                bdesb_MemOutStreamBuf osb;

                LOOP_ASSERT(LINE, 0 == encoder.encode(&osb, VALUE));
                printDiagnostic(encoder);
                LOOP_ASSERT(LINE, 0 == compareBuffers(osb.data(), EXP));

                if (veryVerbose) {
                    T_ T_ P(osb.length())
                    T_ T_ P(EXP)
                    T_ T_ cout << "ACTUAL: ";
                    printBuffer(osb.data(), osb.length());
                }
            }

            if (verbose) { cout << "\nTesting for special values" << endl; }
            {
                static const struct {
                    int         d_lineNum;  // source line number
                    int         d_sign;     // sign value (0 - +ve, 1 - -ve)
                    int         d_exponent; // unbiased exponent value
                    long long   d_mantissa; // mantissa value
                    const char *d_buffer;   // encoded result
                    int         d_result;   // SUCCESS (0) / FAILURE (-1)
                } REAL[] = {
                    //line  sign  exp      man            buffer       res
                    //----  ----  ---      ---            ------       ---
                    // +ve & -ve infinity
                    { L_,      0, 1024,     0,         "09 01 40",   SUCCESS },
                    { L_,      1, 1024,     0,         "09 01 41",   SUCCESS },

                    // +ve & -ve NaN
                    { L_,      0, 1024,     1,         "09 01 42",   SUCCESS },
                    { L_,      1, 1024,     1,         "09 01 42",   SUCCESS },
                    { L_,      0, 1024, 0x000fffffffffffffLL,
                                                       "09 01 42",   SUCCESS },
                    { L_,      1, 1024, 0x000fffffffffffffLL,
                                                       "09 01 42",   SUCCESS },
                    { L_,      0, 1024, 0x000123456789ABCDLL,
                                                       "09 01 42",   SUCCESS },
                    { L_,      0, 1024, 0x000123456789ABCDLL,
                                                       "09 01 42",   SUCCESS },
                };

                bdesb_MemOutStreamBuf osb;

                const int NUM_DATA = sizeof REAL / sizeof *REAL;

                for (int di = 0; di < NUM_DATA; ++di) {
                    const int        LINE     = REAL[di].d_lineNum;
                    const int        SIGN     = REAL[di].d_sign;
                    const int        EXPONENT = REAL[di].d_exponent;
                    const long long  MANTISSA = REAL[di].d_mantissa;
                    const char      *BUFFER   = REAL[di].d_buffer;
                    const int        RESULT   = REAL[di].d_result;
                          double     value;

                    bdesb_MemOutStreamBuf osb;

                    assembleDouble(&value, SIGN, EXPONENT, MANTISSA);

                    ASSERT(RESULT == encoder.encode(&osb, value));
                    printDiagnostic(encoder);

                    if (0 == RESULT) {
                        LOOP_ASSERT(LINE,
                                    0 == compareBuffers(osb.data(), BUFFER));
                    }
                }
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
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nFUNDAMENTALS TEST"
                               << "\n=================" << bsl::endl;

        {
            const unsigned char   XA1 = UCHAR_MAX;
            const signed   char   XB1 = SCHAR_MIN;
            const          char   XC1 = SCHAR_MAX;

            const unsigned short  XD1 = USHRT_MAX;
            const signed   short  XE1 = SHRT_MIN;
            const          short  XF1 = SHRT_MAX;

            const unsigned int    XG1 = UINT_MAX;
            const signed   int    XH1 = INT_MIN;
            const          int    XI1 = INT_MAX;

            const unsigned long   XJ1 = ULONG_MAX;
            const signed   long   XK1 = LONG_MIN;
            const          long   XL1 = LONG_MAX;

            const bsls_Types::Int64  XM1 = 0xff34567890123456LL;
            const bsls_Types::Uint64 XN1 = 0x1234567890123456LL;

            const          bool   XO1 = true;

            const bsl::string     XP1("This is a really long line");
            bsl::string           XP2;

            const float        XQ1 = 99.234;
            const float        XR1 = -100.987;
            const float        XS1 = -77723.875;

            const double       XT1 = 19998989.1234;
            const double       XU1 = 100;

            {
                bdesb_MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XA1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
            {
                bdesb_MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XB1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
            {
                bdesb_MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XC1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
            {
                bdesb_MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XD1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
            {
                bdesb_MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XE1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
            {
                bdesb_MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XF1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
            {
                bdesb_MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XG1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
            {
                bdesb_MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XH1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
            {
                bdesb_MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XI1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
            {
                bdesb_MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XJ1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
            {
                bdesb_MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XK1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
            {
                bdesb_MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XL1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
            {
                bdesb_MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XM1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
            {
                bdesb_MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XN1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
            {
                bdesb_MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XO1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
            {
                bdesb_MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XP1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
            {
                bdesb_MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XQ1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
            {
                bdesb_MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XR1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
            {
                bdesb_MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XS1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
            {
                bdesb_MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XT1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
            {
                bdesb_MemOutStreamBuf osb;

                ASSERT(0 == encoder.encode(&osb, XU1));
                printDiagnostic(encoder);

                if (veryVerbose) {
                      P(osb.length())
                      printBuffer(osb.data(), osb.length());
                }
            }
        }
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST
        // --------------------------------------------------------------------

        static const int MAX_BUF_SIZE = 100000;

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
        int minOutputSize;
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
              basicRec.dt() = bdet_DatetimeTz(
                  bdet_Datetime(bdet_Date(2007, 9, 3),
                                bdet_Time(16, 30)), 0);
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
              basicRec.dt() = bdet_DatetimeTz(
                  bdet_Datetime(bdet_Date(2007, 9, 3),
                                bdet_Time(16, 30)), 0);
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
            return 1;
        }

        bsl::cout << "  " << reps << " repetitions..." << bsl::endl;

        // Measure old ber encoding times:
        bdesb_MemOutStreamBuf osb;
        osb.reserveCapacity(MAX_BUF_SIZE);

        bsls_Stopwatch stopwatch;
        double elapsed;

        // Measure ber encoding times:
        stopwatch.reset();
        stopwatch.start();
        for (int i = 0; i < reps; ++i) {
            osb.pubseekpos(0);
            bdem_BerEncoder encoder;  // Typical usage: single-use object
            encoder.encode(&osb, request);
        }
        stopwatch.stop();

        ASSERT(minOutputSize <= osb.length());
        ASSERT(osb.length() <= MAX_BUF_SIZE);
        elapsed = stopwatch.elapsedTime();
        ASSERT(elapsed > 0);

        double berEncodeSpeed = reps / elapsed;
        bsl::cout << "    bdem_BerEncoder: " << elapsed << " seconds, "
                  << (reps / elapsed) << " reps/sec, "
                  << osb.length() << " bytes" << bsl::endl;

        // Measure bdex encoding time:
        osb.reserveCapacity(MAX_BUF_SIZE);
        bdex_ByteOutStreamFormatter bdexOutStream(&osb);

        stopwatch.reset();
        stopwatch.start();
        const int VERSION = request.maxSupportedBdexVersion();
        for (int i = 0; i < reps; ++i) {
            osb.pubseekpos(0);
            bdex_OutStreamFunctions::streamOut(bdexOutStream,
                                               request,
                                               VERSION);
        }
        stopwatch.stop();

        ASSERT(minOutputSize <= osb.length());
        ASSERT(osb.length() <= MAX_BUF_SIZE);
        elapsed = stopwatch.elapsedTime();
        ASSERT(elapsed > 0);

        double bdexEncodeSpeed = reps / elapsed;
        bsl::cout << "      bdex encoding: " << elapsed << " seconds, "
                  << (reps / elapsed) << " reps/sec, "
                  << osb.length() << " bytes" << bsl::endl;

        double slowDown = 1.0 - berEncodeSpeed / bdexEncodeSpeed;
        bsl::cout << "BerEncoder is " << (slowDown * 100.0)
                  << "% slower than bdex" << bsl::endl;

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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
