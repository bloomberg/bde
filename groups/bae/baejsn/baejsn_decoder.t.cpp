#include <baejsn_decoder.h>
#include <baea_testmessages.h>

#include <bdem_schema.h>
#include <bdem_recorddef.h>

#include <bsl_string.h>
#include <bdeat_attributeinfo.h>
#include <bdeat_choicefunctions.h>
#include <bdeat_enumeratorinfo.h>
#include <bdeat_selectioninfo.h>
#include <bdeat_sequencefunctions.h>
#include <bdeat_valuetypefunctions.h>
#include <bdesb_fixedmeminstreambuf.h>
#include <bdex_instream.h>
#include <bdex_outstream.h>
#include <bsl_sstream.h>

#include <bdede_utf8util.h>
#include <bdem_row.h>
#include <bdem_schema.h>
#include <bdem_schemaaggregateutil.h>
#include <bdem_table.h>
#include <bdesb_fixedmeminstreambuf.h>
#include <bdeu_printmethods.h>  // for printing vector
#include <bdeu_chartype.h>

#include <iostream>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

#define LOOP0_ASSERT ASSERT

// ============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) {                                                    \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP1_ASSERT LOOP_ASSERT

#define LOOP2_ASSERT(I,J,X) {                                                 \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": "                 \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) {                                               \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"     \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) {                                             \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\n";                    \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) {                                           \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" <<                  \
       #M << ": " << M << "\n";                                               \
       aSsErT(1, #X, __LINE__); } }

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // 'P(X)' without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline.
#define L_ __LINE__                           // current Line number

// The 'BSLS_BSLTESTUTIL_EXPAND' macro is required to workaround a
// pre-proccessor issue on windows that prevents __VA_ARGS__ to be expanded in
// the definition of 'BSLS_BSLTESTUTIL_NUM_ARGS'
#define EXPAND(X)                                            \
    X

#define NUM_ARGS_IMPL(X5, X4, X3, X2, X1, X0, N, ...)        \
    N

#define NUM_ARGS(...)                                        \
    EXPAND(NUM_ARGS_IMPL( __VA_ARGS__, 5, 4, 3, 2, 1, 0, ""))

#define LOOPN_ASSERT_IMPL(N, ...)                            \
    EXPAND(LOOP ## N ## _ASSERT(__VA_ARGS__))

#define LOOPN_ASSERT(N, ...)                                 \
    LOOPN_ASSERT_IMPL(N, __VA_ARGS__)

#define ASSERTV(...)                                         \
    LOOPN_ASSERT(NUM_ARGS(__VA_ARGS__), __VA_ARGS__)

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

static const char* TEST_MESSAGES[] = {

"{"
"    \"selection1\" : {"
"        \"element3\" : {"
"            \"selection1\" : true"
"        }"
"    }"
"}",


"{"
"    \"selection1\" : {"
"        \"element1\" : {"
"            \"selection1\" : {"
"                \"element1\"  : 0,"
"                \"element2\"  : \"custom\","
"                \"element3\"  : 999,"
"                \"element4\"  : 0,"
"                \"element5\"  : [ 999 ] ,"
"                \"element6\"  : \"custom\","
"                \"element7\"  : 999,"
"                \"element8\"  : [ 0 ],"
"                \"element9\"  : [ \"custom\" ],"
"                \"element10\" : [ 0 ],"
"                \"element11\" : [ 999 ]"
"            }"
"        },"
"        \"element2\" : ["
"            { \"selection2\" : 1.5 },"
"            { \"selection2\" : 1.5 }"
"        ],"
"        \"element3\" : {"
"            \"selection2\" : \"\""
"        },"
"       \"element4\" : ["
"            { \"selection1\" : {"
"                \"element1\"  : 0,"
"                \"element2\"  : \"custom\","
"                \"element3\"  : 999,"
"                \"element4\"  : 0,"
"                \"element5\"  : [ 999 ] ,"
"                \"element6\"  : \"custom\","
"                \"element7\"  : 999,"
"                \"element8\"  : [ 0 ],"
"                \"element9\"  : [ \"custom\" ],"
"                \"element10\" : [ 0 ],"
"                \"element11\" : [ 999 ]"
"            }},"
"            { \"selection1\" : {"
"                \"element1\"  : 0,"
"                \"element2\"  : \"custom\","
"                \"element3\"  : 999,"
"                \"element4\"  : 0,"
"                \"element5\"  : [ 999 ] ,"
"                \"element6\"  : \"custom\","
"                \"element7\"  : 999,"
"                \"element8\"  : [ 0 ],"
"                \"element9\"  : [ \"custom\" ],"
"                \"element10\" : [ 0 ],"
"                \"element11\" : [ 999 ]"
"            }}"
"       ]"
"    }"
"}",


"{"
"    \"selection3\" : {"
"        \"element1\" : \"custom\","
"        \"element2\" : 255,"
"        \"element3\" : \"2012-08-18T13:25:00\","
"        \"element4\" : {"
"            \"selection3\" : {"
"                \"element8\" : true,"
"                \"element9\" : \"\","
"                \"element10\" : 1.5,"
"                \"element11\" : \"VGhlIHF1aWNrIGJyb3duIGZveA==\","
"                \"element12\" : 2,"
"                \"element13\" : \"LONDON\""
"            }"
"        }"
"    }"
"}"

};

static const int NUM_TEST_MESSAGES =
                              (sizeof(TEST_MESSAGES) / sizeof(*TEST_MESSAGES));


namespace BloombergLP {

namespace test {

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

}  // close namespace test

// TRAITS

BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::Address)

namespace test {

                               // -------------
                               // class Address
                               // -------------

// CONSTANTS

const char Address::CLASS_NAME[] = "Address";
    // the name of this class

const bdeat_AttributeInfo Address::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_STREET,
        "street",                 // name
        sizeof("street") - 1,     // name length
        "street: TBD: Provide annotation",  // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_CITY,
        "city",                 // name
        sizeof("city") - 1,     // name length
        "city: TBD: Provide annotation",  // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_STATE,
        "state",                 // name
        sizeof("state") - 1,     // name length
        "state: TBD: Provide annotation",  // annotation
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
        return accessor(d_street,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STREET]);
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

}  // close namespace test

namespace test {

                               // ==============
                               // class Employee
                               // ==============

class Employee {
    // Employee: TBD: Provide annotation

  private:
    bsl::string d_name;
        // name: TBD: Provide annotation
    Address d_homeAddress;
        // homeAddress: TBD: Provide annotation
    int d_age;
        // age: TBD: Provide annotation

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

}  // close namespace test

// TRAITS

BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::Employee)


namespace test {

                               // --------------
                               // class Employee
                               // --------------

// CONSTANTS

const char Employee::CLASS_NAME[] = "Employee";
    // the name of this class

const bdeat_AttributeInfo Employee::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_NAME,
        "name",                 // name
        sizeof("name") - 1,     // name length
        "name: TBD: Provide annotation",  // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_HOME_ADDRESS,
        "homeAddress",                 // name
        sizeof("homeAddress") - 1,     // name length
        "homeAddress: TBD: Provide annotation",  // annotation
        bdeat_FormattingMode::BDEAT_DEFAULT // formatting mode
    },
    {
        ATTRIBUTE_ID_AGE,
        "age",                 // name
        sizeof("age") - 1,     // name length
        "age: TBD: Provide annotation",  // annotation
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

}  // close namespace test

} // close namespace BloombergLP

// BaseReaderHandler

//! Default implementation of Handler.
/*! This can be used as base class of any reader handler.
    \implements Handler
*/
int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    //verbose = argc > 2;
    //veryVerbose = argc > 3;
    //veryVeryVerbose = argc > 4;
    //veryVeryVeryVerbose = argc > 5;
    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) {
      case 3: {
        for (int ti = 0; ti < NUM_TEST_MESSAGES; ++ti) {
            cout << "> START MESSAGE " << ti << endl;
            const char *jsonText = TEST_MESSAGES[ti];

            baea::FeatureTestMessage message;

            baejsn_Decoder decoder;
            bsl::istringstream iss(jsonText);
            ASSERTV(0 == decoder.decode(iss, &message));

            P(message);
            cout << "> END MESSAGE " << ti << endl;
        }

      } break;
      case 2: {
        bcema_SharedPtr<bdem_Schema> schema(new bdem_Schema);

        bdem_RecordDef *address = schema->createRecord("Address");
        address->appendField(bdem_ElemType::BDEM_STRING,       "street");
        address->appendField(bdem_ElemType::BDEM_STRING,       "city");
        address->appendField(bdem_ElemType::BDEM_STRING,       "state");

        bdem_RecordDef *employee = schema->createRecord("Employee");
        employee->appendField(bdem_ElemType::BDEM_STRING, "name");
        employee->appendField(bdem_ElemType::BDEM_LIST,   address, "homeAddress");
        employee->appendField(bdem_ElemType::BDEM_INT, "age");

        bcem_Aggregate bob(schema, "Employee");

        char jsonText[] =
            "{\n"
            //"   \"Employee\": {\n"
            "       \"name\" : \"Bob\",\n"
            "       \"homeAddress\" : {\n"
            "           \"street\" : \"Some Street\",\n"
            "           \"city\" : \"Some City\",\n"
            "           \"state\" : \"Some State\"\n"
            "       },\n"
            "       \"age\" : 21\n"
            //"   }\n"
            "}";

        baejsn_Decoder decoder;
        bsl::istringstream iss(jsonText);

        ASSERTV(0 == decoder.decode(iss, &bob));

        //rapidjson::GenericReader<char, char> reader;
        //BaseReaderHandler handler;
        //TestReaderHandler<test::Employee> handler(&bob);
        //reader.Parse<rapidjson::kParseInsituFlag>(iss, handler);

        P(bob);

        //ASSERTV(bob.name(), "Bob"         == bob.name());
        //ASSERT("Some Street" == bob.homeAddress().street());
        //ASSERT("Some City"   == bob.homeAddress().city());
        //ASSERT("Some State"  == bob.homeAddress().state());
        //ASSERT(21            == bob.age());
      } break;
      case 1: {
        test::Employee bob;

        char jsonText[] =
            "{\n"
            //"   \"Employee\": {\n"
            "       \"name\" : \"Bob\",\n"
            "       \"homeAddress\" : {\n"
            "           \"street\" : \"Some Street\",\n"
            "           \"city\" : \"Some City\",\n"
            "           \"state\" : \"Some State\"\n"
            "       },\n"
            "       \"age\" : 21\n"
            //"   }\n"
            "}";

        bsl::istringstream iss(jsonText);

        baejsn_Decoder decoder;
        ASSERTV(0 == decoder.decode(iss, &bob));

        //rapidjson::GenericReader<char, char> reader;
        //BaseReaderHandler handler;
        //TestReaderHandler<test::Employee> handler(&bob);
        //reader.Parse<rapidjson::kParseInsituFlag>(iss, handler);


        ASSERTV(bob.name(), "Bob"         == bob.name());
        ASSERT("Some Street" == bob.homeAddress().street());
        ASSERT("Some City"   == bob.homeAddress().city());
        ASSERT("Some State"  == bob.homeAddress().state());
        ASSERT(21            == bob.age());
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
