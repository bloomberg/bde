// bcem_aggregateutil.t.cpp   -*-C++-*-
#include <bcem_aggregateutil.h>

#include <bcem_aggregate.h>
#include <bcema_sharedptr.h>

#include <bdem_schema.h>
#include <bdetu_unset.h>
#include <bdeut_nullablevalue.h>
#include <bslma_allocator.h>
#include <bslma_default.h>

#include <bsl_algorithm.h>
#include <bsl_cctype.h>
#include <bsl_iostream.h>
#include <bsl_iterator.h>
#include <bsl_limits.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

using namespace BloombergLP;

using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

//=============================================================================
//                                   TEST PLAN
//-----------------------------------------------------------------------------
//                                   Overview
//                                   --------
//
// The component under test provides a set of static functions that translate
// between 'bcem_Aggregate' and "primitive" C++ types; 'bdeut_NullableValue' of
// primitive types; and 'bsl::vector's of primitive types.  In the description
// of each test case, the phrase "primitive types" is understood as the
// following types: 'bool', 'char', 'short', 'int', 'float', 'double',
// 'unsigned char', 'unsigned short', 'unsigned int', 'bsl::string',
// 'bsls_PlatformUtil::Int64', 'bsls_PlatformUtil::Uint64', 'bdet_DateTz',
// 'bdet_DatetimeTz', 'bdet_TimeTz', 'bdet_Date', 'bdet_Datetime', and
// 'bdet_Time'.
//-----------------------------------------------------------------------------
// PUBLIC MANIPULATORS
// [ 2] fromAggregate(bool)
// [ 2] fromAggregate(char)
// [ 2] fromAggregate(short)
// [ 2] fromAggregate(int)
// [ 2] fromAggregate(float)
// [ 2] fromAggregate(double)
// [ 2] fromAggregate(unsigned char)
// [ 2] fromAggregate(unsigned short)
// [ 2] fromAggregate(unsigned int)
// [ 2] fromAggregate(bsl::string)
// [ 2] fromAggregate(bsls_PlatformUtil::Int64)
// [ 2] fromAggregate(bsls_PlatformUtil::Uint64)
// [ 2] fromAggregate(bdet_DateTz)
// [ 2] fromAggregate(bdet_DatetimeTz)
// [ 2] fromAggregate(bdet_TimeTz)
// [ 2] fromAggregate(bdet_Date)
// [ 2] fromAggregate(bdet_Datetime)
// [ 2] fromAggregate(bdet_Time)
// [ 2] toAggregate(bool)
// [ 2] toAggregate(char)
// [ 2] toAggregate(short)
// [ 2] toAggregate(int)
// [ 2] toAggregate(float)
// [ 2] toAggregate(double)
// [ 2] toAggregate(unsigned char)
// [ 2] toAggregate(unsigned short)
// [ 2] toAggregate(unsigned int)
// [ 2] toAggregate(bsl::string)
// [ 2] toAggregate(bsls_PlatformUtil::Int64)
// [ 2] toAggregate(bsls_PlatformUtil::Uint64)
// [ 2] toAggregate(bdet_DateTz)
// [ 2] toAggregate(bdet_DatetimeTz)
// [ 2] toAggregate(bdet_TimeTz)
// [ 2] toAggregate(bdet_Date)
// [ 2] toAggregate(bdet_Datetime)
// [ 2] toAggregate(bdet_Time)
// [ 3] fromAggregate(bdeut_NullableValue<bool>)
// [ 3] fromAggregate(bdeut_NullableValue<char>)
// [ 3] fromAggregate(bdeut_NullableValue<short>)
// [ 3] fromAggregate(bdeut_NullableValue<int>)
// [ 3] fromAggregate(bdeut_NullableValue<float>)
// [ 3] fromAggregate(bdeut_NullableValue<double>)
// [ 3] fromAggregate(bdeut_NullableValue<unsigned char>)
// [ 3] fromAggregate(bdeut_NullableValue<unsigned short>)
// [ 3] fromAggregate(bdeut_NullableValue<unsigned int>)
// [ 3] fromAggregate(bdeut_NullableValue<bsl::string>)
// [ 3] fromAggregate(bdeut_NullableValue<bsls_PlatformUtil::Int64>)
// [ 3] fromAggregate(bdeut_NullableValue<bsls_PlatformUtil::Uint64>)
// [ 3] fromAggregate(bdeut_NullableValue<bdet_DateTz>)
// [ 3] fromAggregate(bdeut_NullableValue<bdet_DatetimeTz>)
// [ 3] fromAggregate(bdeut_NullableValue<bdet_TimeTz>)
// [ 3] fromAggregate(bdeut_NullableValue<bdet_Date>)
// [ 3] fromAggregate(bdeut_NullableValue<bdet_Datetime>)
// [ 3] fromAggregate(bdeut_NullableValue<bdet_Time>)
// [ 3] toAggregate(bdeut_NullableValue<bool>)
// [ 3] toAggregate(bdeut_NullableValue<char>)
// [ 3] toAggregate(bdeut_NullableValue<short>)
// [ 3] toAggregate(bdeut_NullableValue<int>)
// [ 3] toAggregate(bdeut_NullableValue<float>)
// [ 3] toAggregate(bdeut_NullableValue<double>)
// [ 3] toAggregate(bdeut_NullableValue<unsigned char>)
// [ 3] toAggregate(bdeut_NullableValue<unsigned short>)
// [ 3] toAggregate(bdeut_NullableValue<unsigned int>)
// [ 3] toAggregate(bdeut_NullableValue<bsl::string>)
// [ 3] toAggregate(bdeut_NullableValue<bsls_PlatformUtil::Int64>)
// [ 3] toAggregate(bdeut_NullableValue<bsls_PlatformUtil::Uint64>)
// [ 3] toAggregate(bdeut_NullableValue<bdet_DateTz>)
// [ 3] toAggregate(bdeut_NullableValue<bdet_DatetimeTz>)
// [ 3] toAggregate(bdeut_NullableValue<bdet_TimeTz>)
// [ 3] toAggregate(bdeut_NullableValue<bdet_Date>)
// [ 3] toAggregate(bdeut_NullableValue<bdet_Datetime>)
// [ 3] toAggregate(bdeut_NullableValue<bdet_Time>)
// [ 4] fromAggregate(bsl::vector<bool>)
// [ 4] fromAggregate(bsl::vector<char>)
// [ 4] fromAggregate(bsl::vector<short>)
// [ 4] fromAggregate(bsl::vector<int>)
// [ 4] fromAggregate(bsl::vector<float>)
// [ 4] fromAggregate(bsl::vector<double>)
// [ 4] fromAggregate(bsl::vector<unsigned char>)
// [ 4] fromAggregate(bsl::vector<unsigned short>)
// [ 4] fromAggregate(bsl::vector<unsigned int>)
// [ 4] fromAggregate(bsl::vector<bsl::string>)
// [ 4] fromAggregate(bsl::vector<bsls_PlatformUtil::Int64>)
// [ 4] fromAggregate(bsl::vector<bsls_PlatformUtil::Uint64>)
// [ 4] fromAggregate(bsl::vector<bdet_DateTz>)
// [ 4] fromAggregate(bsl::vector<bdet_DatetimeTz>)
// [ 4] fromAggregate(bsl::vector<bdet_TimeTz>)
// [ 4] fromAggregate(bsl::vector<bdet_Date>)
// [ 4] fromAggregate(bsl::vector<bdet_Datetime>)
// [ 4] fromAggregate(bsl::vector<bdet_Time>)
// [ 4] toAggregate(bsl::vector<bool>)
// [ 4] toAggregate(bsl::vector<char>)
// [ 4] toAggregate(bsl::vector<short>)
// [ 4] toAggregate(bsl::vector<int>)
// [ 4] toAggregate(bsl::vector<float>)
// [ 4] toAggregate(bsl::vector<double>)
// [ 4] toAggregate(bsl::vector<unsigned char>)
// [ 4] toAggregate(bsl::vector<unsigned short>)
// [ 4] toAggregate(bsl::vector<unsigned int>)
// [ 4] toAggregate(bsl::vector<bsl::string>)
// [ 4] toAggregate(bsl::vector<bsls_PlatformUtil::Int64>)
// [ 4] toAggregate(bsl::vector<bsls_PlatformUtil::Uint64>)
// [ 4] toAggregate(bsl::vector<bdet_DateTz>)
// [ 4] toAggregate(bsl::vector<bdet_DatetimeTz>)
// [ 4] toAggregate(bsl::vector<bdet_TimeTz>)
// [ 4] toAggregate(bsl::vector<bdet_Date>)
// [ 4] toAggregate(bsl::vector<bdet_Datetime>)
// [ 4] toAggregate(bsl::vector<bdet_Time>)
//-----------------------------------------------------------------------------
// [ 0] USAGE EXAMPLE
// [ 1] BREATHING TEST
// [ 2] TESTING PRIMITIVE TYPES
// [ 3] TESTING NULLABLE PRIMITIVE TYPES
// [ 4] TESTING ARRAYS OF PRIMITIVE TYPES
// [ 5] TESTING CONVERSIONS
// [ 6] TESTING SEQUENCES
// [ 7] TESTING CONCERN: NULLABLE PRIMITIVE TYPES ARE CONVERTIBLE TO
//                       NON-NULLABLE AGGREGATE FIELDS
//-----------------------------------------------------------------------------

//=============================================================================
//                        STANDARD BDE ASSERT TEST MACROS
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100)  ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\n";\
               aSsErT(1, #X, __LINE__); }}

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
                    << #K << ": " << K << "\n";                           \
               aSsErT(1, #X, __LINE__); }}

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
                    << #K << ": " << K << "\t" << #L << ": " << L << "\n";\
               aSsErT(1, #X, __LINE__); }}

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
                    << #K << ": " << K << "\t" << #L << ": " << L << "\t" \
                    << #M << ": " << M << "\n";                           \
               aSsErT(1, #X, __LINE__); }}

//=============================================================================
//                       SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number.
#define T_()  cout << '\t' << flush;          // Print tab w/o newline.

//=============================================================================
//              GLOBAL TYPES, CONSTANTS, AND VARIABLES FOR TESTING
//-----------------------------------------------------------------------------
namespace {

typedef bcem_AggregateUtil Util;

typedef bsl::numeric_limits<char>               NLC;
typedef bsl::numeric_limits<short>              NLS;
typedef bsl::numeric_limits<int>                NLI;
typedef bsl::numeric_limits<float>              NLF;
typedef bsl::numeric_limits<double>             NLD;
typedef bsl::numeric_limits<unsigned char>      NLUC;
typedef bsl::numeric_limits<unsigned short>     NLUS;
typedef bsl::numeric_limits<unsigned int>       NLUI;
typedef bsl::numeric_limits<long long>          NLI64;
typedef bsl::numeric_limits<unsigned long long> NLUI64;

static const char              NLC_MIN = NLC::min();
static const char              NLC_MAX = NLC::max();
static const short             NLS_MIN = NLS::min();
static const short             NLS_MAX = NLS::max();
static const int               NLI_MIN = NLI::min();
static const int               NLI_MAX = NLI::max();
static const float             NLF_MIN = NLF::min();
static const float             NLF_MAX = NLF::max();
static const double            NLD_MIN = NLD::min();
static const double            NLD_MAX = NLD::max();

static const unsigned char     NLUC_MIN = NLUC::min();
static const unsigned char     NLUC_MAX = NLUC::max();
static const unsigned short    NLUS_MIN = NLUS::min();
static const unsigned short    NLUS_MAX = NLUS::max();
static const unsigned int      NLUI_MIN = NLUI::min();
static const unsigned int      NLUI_MAX = NLUI::max();

static const bsls_PlatformUtil::Int64 NLI64_MIN =
    bsl::numeric_limits<bsls_PlatformUtil::Int64>::min();

static const bsls_PlatformUtil::Int64 NLI64_MAX =
    bsl::numeric_limits<bsls_PlatformUtil::Int64>::max();

static const bsls_PlatformUtil::Uint64 NLUI64_MIN =
    bsl::numeric_limits<bsls_PlatformUtil::Uint64>::min();

static const bsls_PlatformUtil::Uint64 NLUI64_MAX =
    bsl::numeric_limits<bsls_PlatformUtil::Uint64>::max();

template <typename TYPE>
struct NullTraits {
    static TYPE notUnsetValue() { return 1 + bdetu_Unset<TYPE>::unsetValue(); }
};

enum {
    // The following values are valid 'fieldId' arguments to the 'toAggregate'
    // and 'fromAggregate' methods of 'bcem_AggregateUtil'.

    ID_VAL1   // bool
  , ID_VAL2   // char
  , ID_VAL3   // short
  , ID_VAL4   // int
  , ID_VAL5   // float
  , ID_VAL6   // double
  , ID_VAL7   // unsigned char
  , ID_VAL8   // unsigned short
  , ID_VAL9   // unsigned int
  , ID_VAL10  // string
  , ID_VAL11  // bsls_PlatformUtil::Int64
  , ID_VAL12  // bsls_PlatformUtil::Uint64
  , ID_VAL13  // bdet_DateTz
  , ID_VAL14  // bdet_DatetimeTz
  , ID_VAL15  // bdet_TimeTz
  , ID_VAL16  // bdet_Date
  , ID_VAL17  // bdet_Datetime
  , ID_VAL18  // bdet_Time

  , ID_INVALID    // invalid id
};

int verbose = 0;
int veryVerbose = 0;
int veryVeryVerbose = 0;
int veryVeryVeryVerbose = 0;

}  // close unnamed namespace

// BEGIN GENERATED CODE

// test_messages.h   -*-C++-*-   GENERATED FILE -- DO NOT EDIT
#ifndef INCLUDED_TEST_MESSAGES
#define INCLUDED_TEST_MESSAGES

//@PURPOSE: Provide value-semantic attribute classes
//
//@CLASSES:
// test::Person:
// test::Company:
// test::Entity:
//
//@AUTHOR: BAS
//
//@DESCRIPTION:

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDEAT_ATTRIBUTEINFO
#include <bdeat_attributeinfo.h>
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

#ifndef INCLUDED_BDET_DATETZ
#include <bdet_datetz.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

class bcem_Aggregate;

namespace test { class Person; }
namespace test { class Company; }
namespace test { class Entity; }
namespace test {

                                // ============
                                // class Person
                                // ============

class Person {

    // DATA MEMBERS
    bsl::string  d_lastName;
    bsl::string  d_firstName;
    int          d_age;
    bdet_DateTz  d_birthDate;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_LAST_NAME  = 0
      , ATTRIBUTE_ID_FIRST_NAME = 1
      , ATTRIBUTE_ID_AGE        = 2
      , ATTRIBUTE_ID_BIRTH_DATE = 3
    };

    enum {
        NUM_ATTRIBUTES = 4
    };

    enum {
        ATTRIBUTE_INDEX_LAST_NAME  = 0
      , ATTRIBUTE_INDEX_FIRST_NAME = 1
      , ATTRIBUTE_INDEX_AGE        = 2
      , ATTRIBUTE_INDEX_BIRTH_DATE = 3
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

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
    explicit Person(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'Person' having the default value.  Use the
        // optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Person(const Person& original,
           bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'Person' having the value of the specified
        // 'original' object.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~Person();
        // Destroy this object.

    // MANIPULATORS
    Person& operator=(const Person& rhs);
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

    int fromAggregate(const bcem_Aggregate& aggregate);
        // Store the value of the specified 'aggregate' into this object.
        // Return 0 on success, and a non-zero value otherwise.

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

    bsl::string& lastName();
        // Return a reference to the modifiable "LastName" attribute of this
        // object.

    bsl::string& firstName();
        // Return a reference to the modifiable "FirstName" attribute of this
        // object.

    int& age();
        // Return a reference to the modifiable "Age" attribute of this object.

    bdet_DateTz& birthDate();
        // Return a reference to the modifiable "BirthDate" attribute of this
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

    int toAggregate(bcem_Aggregate *result) const;
        // Load the specified 'result' with the value of this object.  Return
        // 0 on success, and a non-zero value otherwise.

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

    const bsl::string& lastName() const;
        // Return a reference to the non-modifiable "LastName" attribute of
        // this object.

    const bsl::string& firstName() const;
        // Return a reference to the non-modifiable "FirstName" attribute of
        // this object.

    const int& age() const;
        // Return a reference to the non-modifiable "Age" attribute of this
        // object.

    const bdet_DateTz& birthDate() const;
        // Return a reference to the non-modifiable "BirthDate" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const Person& lhs, const Person& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const Person& lhs, const Person& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const Person& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace test

// TRAITS

BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::Person)

namespace test {

                               // =============
                               // class Company
                               // =============

class Company {

    // DATA MEMBERS
    bsl::string  d_name;
    int          d_accountNum;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_NAME        = 0
      , ATTRIBUTE_ID_ACCOUNT_NUM = 1
    };

    enum {
        NUM_ATTRIBUTES = 2
    };

    enum {
        ATTRIBUTE_INDEX_NAME        = 0
      , ATTRIBUTE_INDEX_ACCOUNT_NUM = 1
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

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
    explicit Company(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'Company' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Company(const Company& original,
            bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'Company' having the value of the specified
        // 'original' object.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~Company();
        // Destroy this object.

    // MANIPULATORS
    Company& operator=(const Company& rhs);
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

    int fromAggregate(const bcem_Aggregate& aggregate);
        // Store the value of the specified 'aggregate' into this object.
        // Return 0 on success, and a non-zero value otherwise.

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

    int& accountNum();
        // Return a reference to the modifiable "AccountNum" attribute of this
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

    int toAggregate(bcem_Aggregate *result) const;
        // Load the specified 'result' with the value of this object.  Return
        // 0 on success, and a non-zero value otherwise.

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

    const int& accountNum() const;
        // Return a reference to the non-modifiable "AccountNum" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const Company& lhs, const Company& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const Company& lhs, const Company& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const Company& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace test

// TRAITS

BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::Company)

namespace test {

                                // ============
                                // class Entity
                                // ============

class Entity {

    // DATA MEMBERS
    union {
        bsls_ObjectBuffer< Company > d_corp;
        bsls_ObjectBuffer< Person >  d_human;
    };

    int              d_selectionId;
    bslma_Allocator *d_allocator_p;

  public:
    // TYPES
    enum {
        SELECTION_ID_UNDEFINED = -1

      , SELECTION_ID_CORP  = 0
      , SELECTION_ID_HUMAN = 1
    };

    enum {
        NUM_SELECTIONS = 2
    };

    enum {
        SELECTION_INDEX_CORP  = 0
      , SELECTION_INDEX_HUMAN = 1
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdeat_SelectionInfo SELECTION_INFO_ARRAY[];

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
    explicit Entity(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'Entity' having the default value.  Use the
        // optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Entity(const Entity& original,
          bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'Entity' having the value of the specified
        // 'original' object.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~Entity();
        // Destroy this object.

    // MANIPULATORS
    Entity& operator=(const Entity& rhs);
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

    int fromAggregate(const bcem_Aggregate& aggregate);
        // Store the value of the specified 'aggregate' into this object.
        // Return 0 on success, and a non-zero value otherwise.

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

    Company& makeCorp();
    Company& makeCorp(const Company& value);
        // Set the value of this object to be a "Corp" value.  Optionally
        // specify the 'value' of the "Corp".  If 'value' is not specified, the
        // default "Corp" value is used.

    Person& makeHuman();
    Person& makeHuman(const Person& value);
        // Set the value of this object to be a "Human" value.  Optionally
        // specify the 'value' of the "Human".  If 'value' is not specified,
        // the default "Human" value is used.

    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.

    Company& corp();
        // Return a reference to the modifiable "Corp" selection of this object
        // if "Corp" is the current selection.  The behavior is undefined
        // unless "Corp" is the selection of this object.

    Person& human();
        // Return a reference to the modifiable "Human" selection of this
        // object if "Human" is the current selection.  The behavior is
        // undefined unless "Human" is the selection of this object.

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

    int toAggregate(bcem_Aggregate *result) const;
        // Load the specified 'result' with the value of this object.  Return
        // 0 on success, and a non-zero value otherwise.

    int selectionId() const;
        // Return the id of the current selection if the selection is defined,
        // and -1 otherwise.

    template<class ACCESSOR>
    int accessSelection(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' on the non-modifiable selection,
        // supplying 'accessor' with the corresponding selection information
        // structure.  Return the value returned from the invocation of
        // 'accessor' if this object has a defined selection, and -1 otherwise.

    const Company& corp() const;
        // Return a reference to the non-modifiable "Corp" selection of this
        // object if "Corp" is the current selection.  The behavior is
        // undefined unless "Corp" is the selection of this object.

    const Person& human() const;
        // Return a reference to the non-modifiable "Human" selection of this
        // object if "Human" is the current selection.  The behavior is
        // undefined unless "Human" is the selection of this object.

    bool isCorpValue() const;
        // Return 'true' if the value of this object is a "Corp" value, and
        // return 'false' otherwise.

    bool isHumanValue() const;
        // Return 'true' if the value of this object is a "Human" value, and
        // return 'false' otherwise.

    bool isUndefinedValue() const;
        // Return 'true' if the value of this object is undefined, and 'false'
        // otherwise.
};

// FREE OPERATORS
inline
bool operator==(const Entity& lhs, const Entity& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'Entity' objects have the same
    // value if either the selections in both objects have the same ids and
    // the same values, or both selections are undefined.

inline
bool operator!=(const Entity& lhs, const Entity& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const Entity& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace test

// TRAITS

BDEAT_DECL_CHOICE_WITH_ALLOCATOR_TRAITS(test::Entity)

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
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace test {

                                // ------------
                                // class Person
                                // ------------

// CLASS METHODS
inline
int Person::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// MANIPULATORS
template <class STREAM>
STREAM& Person::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_lastName, 1);
            bdex_InStreamFunctions::streamIn(stream, d_firstName, 1);
            bdex_InStreamFunctions::streamIn(stream, d_age, 1);
            bdex_InStreamFunctions::streamIn(stream, d_birthDate, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

template <class MANIPULATOR>
int Person::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_lastName,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_LAST_NAME]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_firstName,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FIRST_NAME]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_age, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_birthDate,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BIRTH_DATE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int Person::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_LAST_NAME: {
        return manipulator(&d_lastName,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_LAST_NAME]);
      } break;
      case ATTRIBUTE_ID_FIRST_NAME: {
        return manipulator(&d_firstName,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FIRST_NAME]);
      } break;
      case ATTRIBUTE_ID_AGE: {
        return manipulator(&d_age, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE]);
      } break;
      case ATTRIBUTE_ID_BIRTH_DATE: {
        return manipulator(&d_birthDate,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BIRTH_DATE]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int Person::manipulateAttribute(
        MANIPULATOR&  manipulator,
        const char   *name,
        int           nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
bsl::string& Person::lastName()
{
    return d_lastName;
}

inline
bsl::string& Person::firstName()
{
    return d_firstName;
}

inline
int& Person::age()
{
    return d_age;
}

inline
bdet_DateTz& Person::birthDate()
{
    return d_birthDate;
}

// ACCESSORS
template <class STREAM>
STREAM& Person::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_lastName, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_firstName, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_age, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_birthDate, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
int Person::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_lastName,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_LAST_NAME]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_firstName,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FIRST_NAME]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_age, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_birthDate,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BIRTH_DATE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int Person::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_LAST_NAME: {
        return accessor(d_lastName,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_LAST_NAME]);
      } break;
      case ATTRIBUTE_ID_FIRST_NAME: {
        return accessor(d_firstName,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FIRST_NAME]);
      } break;
      case ATTRIBUTE_ID_AGE: {
        return accessor(d_age, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE]);
      } break;
      case ATTRIBUTE_ID_BIRTH_DATE: {
        return accessor(d_birthDate,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BIRTH_DATE]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int Person::accessAttribute(
        ACCESSOR&   accessor,
        const char *name,
        int         nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

inline
const bsl::string& Person::lastName() const
{
    return d_lastName;
}

inline
const bsl::string& Person::firstName() const
{
    return d_firstName;
}

inline
const int& Person::age() const
{
    return d_age;
}

inline
const bdet_DateTz& Person::birthDate() const
{
    return d_birthDate;
}

                               // -------------
                               // class Company
                               // -------------

// CLASS METHODS
inline
int Company::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// MANIPULATORS
template <class STREAM>
STREAM& Company::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_name, 1);
            bdex_InStreamFunctions::streamIn(stream, d_accountNum, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

template <class MANIPULATOR>
int Company::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_accountNum,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCOUNT_NUM]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int Company::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_NAME: {
        return manipulator(&d_name,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
      } break;
      case ATTRIBUTE_ID_ACCOUNT_NUM: {
        return manipulator(&d_accountNum,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCOUNT_NUM]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int Company::manipulateAttribute(
        MANIPULATOR&  manipulator,
        const char   *name,
        int           nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
bsl::string& Company::name()
{
    return d_name;
}

inline
int& Company::accountNum()
{
    return d_accountNum;
}

// ACCESSORS
template <class STREAM>
STREAM& Company::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_name, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_accountNum, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
int Company::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_accountNum,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCOUNT_NUM]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int Company::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_NAME: {
        return accessor(d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
      } break;
      case ATTRIBUTE_ID_ACCOUNT_NUM: {
        return accessor(d_accountNum,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCOUNT_NUM]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int Company::accessAttribute(
        ACCESSOR&   accessor,
        const char *name,
        int         nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

inline
const bsl::string& Company::name() const
{
    return d_name;
}

inline
const int& Company::accountNum() const
{
    return d_accountNum;
}

                                // ------------
                                // class Entity
                                // ------------

// CLASS METHODS
inline
int Entity::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// CREATORS
inline
Entity::Entity(bslma_Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

inline
Entity::~Entity()
{
    reset();
}

// MANIPULATORS
template <class STREAM>
STREAM& Entity::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            short selectionId;
            stream.getInt16(selectionId);
            if (!stream) {
                return stream;
            }
            switch (selectionId) {
              case SELECTION_ID_CORP: {
                makeCorp();
                bdex_InStreamFunctions::streamIn(
                    stream, d_corp.object(), 1);
              } break;
              case SELECTION_ID_HUMAN: {
                makeHuman();
                bdex_InStreamFunctions::streamIn(
                    stream, d_human.object(), 1);
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

template <class MANIPULATOR>
int Entity::manipulateSelection(MANIPULATOR& manipulator)
{
    enum { FAILURE = -1, SUCCESS = 0 };

    switch (d_selectionId) {
      case Entity::SELECTION_ID_CORP:
        return manipulator(&d_corp.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_CORP]);
      case Entity::SELECTION_ID_HUMAN:
        return manipulator(&d_human.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_HUMAN]);
      default:
        BSLS_ASSERT_SAFE(Entity::SELECTION_ID_UNDEFINED ==
                     d_selectionId);
        return FAILURE;
    }
}

inline
Company& Entity::corp()
{
    BSLS_ASSERT_SAFE(SELECTION_ID_CORP == d_selectionId);
    return d_corp.object();
}

inline
Person& Entity::human()
{
    BSLS_ASSERT_SAFE(SELECTION_ID_HUMAN == d_selectionId);
    return d_human.object();
}

// ACCESSORS
template <class STREAM>
STREAM& Entity::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
            stream.putInt16(d_selectionId);
            switch (d_selectionId) {
              case SELECTION_ID_CORP: {
                bdex_OutStreamFunctions::streamOut(
                    stream, d_corp.object(), 1);
              } break;
              case SELECTION_ID_HUMAN: {
                bdex_OutStreamFunctions::streamOut(
                    stream, d_human.object(), 1);
              } break;
              default:
                BSLS_ASSERT_SAFE(SELECTION_ID_UNDEFINED == d_selectionId);
            }
      } break;
    }
    return stream;
}

inline
int Entity::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int Entity::accessSelection(ACCESSOR& accessor) const
{
    enum { FAILURE = -1, SUCCESS = 0 };

    switch (d_selectionId) {
      case SELECTION_ID_CORP:
        return accessor(d_corp.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_CORP]);
      case SELECTION_ID_HUMAN:
        return accessor(d_human.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_HUMAN]);
      default:
        BSLS_ASSERT_SAFE(SELECTION_ID_UNDEFINED == d_selectionId);
        return FAILURE;
    }
}

inline
const Company& Entity::corp() const
{
    BSLS_ASSERT_SAFE(SELECTION_ID_CORP == d_selectionId);
    return d_corp.object();
}

inline
const Person& Entity::human() const
{
    BSLS_ASSERT_SAFE(SELECTION_ID_HUMAN == d_selectionId);
    return d_human.object();
}

inline
bool Entity::isCorpValue() const
{
    return SELECTION_ID_CORP == d_selectionId;
}

inline
bool Entity::isHumanValue() const
{
    return SELECTION_ID_HUMAN == d_selectionId;
}

inline
bool Entity::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}

}  // close namespace test

// FREE FUNCTIONS

inline
bool test::operator==(
        const test::Person& lhs,
        const test::Person& rhs)
{
    return  lhs.lastName() == rhs.lastName()
         && lhs.firstName() == rhs.firstName()
         && lhs.age() == rhs.age()
         && lhs.birthDate() == rhs.birthDate();
}

inline
bool test::operator!=(
        const test::Person& lhs,
        const test::Person& rhs)
{
    return  lhs.lastName() != rhs.lastName()
         || lhs.firstName() != rhs.firstName()
         || lhs.age() != rhs.age()
         || lhs.birthDate() != rhs.birthDate();
}

inline
bsl::ostream& test::operator<<(
        bsl::ostream& stream,
        const test::Person& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline
bool test::operator==(
        const test::Company& lhs,
        const test::Company& rhs)
{
    return  lhs.name() == rhs.name()
         && lhs.accountNum() == rhs.accountNum();
}

inline
bool test::operator!=(
        const test::Company& lhs,
        const test::Company& rhs)
{
    return  lhs.name() != rhs.name()
         || lhs.accountNum() != rhs.accountNum();
}

inline
bsl::ostream& test::operator<<(
        bsl::ostream& stream,
        const test::Company& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline
bool test::operator==(
        const test::Entity& lhs,
        const test::Entity& rhs)
{
    typedef test::Entity Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::SELECTION_ID_CORP:
            return lhs.corp() == rhs.corp();
          case Class::SELECTION_ID_HUMAN:
            return lhs.human() == rhs.human();
          default:
            BSLS_ASSERT_SAFE(Class::SELECTION_ID_UNDEFINED
                            == rhs.selectionId());
            return true;
        }
    }
    else {
        return false;
   }
}

inline
bool test::operator!=(
        const test::Entity& lhs,
        const test::Entity& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& test::operator<<(
        bsl::ostream& stream,
        const test::Entity& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP
#endif

// GENERATED BY BLP_BAS_CODEGEN_3.0.9 Tue Jun 24 15:47:19 2008
// ----------------------------------------------------------------------------
// *Generated end-of-file block removed.*
// ----------------------------------------------------------------------------

// test_messages.cpp   -*-C++-*-   GENERATED FILE -- DO NOT EDIT

//#include <test_messages.h>

#include <bcem_aggregate.h>
#include <bcem_aggregateutil.h>

#include <bdet_datetz.h>
#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_string.h>
#include <bsls_assert.h>

#include <bsl_cctype.h>
#include <bsl_iomanip.h>
#include <bsl_ostream.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace test {

                                // ------------
                                // class Person
                                // ------------

// CONSTANTS

const char Person::CLASS_NAME[] = "Person";
    // the name of this class

const bdeat_AttributeInfo Person::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_LAST_NAME,
        "LastName",             // name
        sizeof("LastName") - 1, // name length
        "",  // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_FIRST_NAME,
        "FirstName",             // name
        sizeof("FirstName") - 1, // name length
        "",  // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_AGE,
        "Age",             // name
        sizeof("Age") - 1, // name length
        "",  // annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_BIRTH_DATE,
        "BirthDate",             // name
        sizeof("BirthDate") - 1, // name length
        "",  // annotation
        bdeat_FormattingMode::BDEAT_DEFAULT // formatting mode
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *Person::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 3: {
            if ((name[0]|0x20)=='a'
             && (name[1]|0x20)=='g'
             && (name[2]|0x20)=='e')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE];
            }
        } break;
        case 8: {
            if ((name[0]|0x20)=='l'
             && (name[1]|0x20)=='a'
             && (name[2]|0x20)=='s'
             && (name[3]|0x20)=='t'
             && (name[4]|0x20)=='n'
             && (name[5]|0x20)=='a'
             && (name[6]|0x20)=='m'
             && (name[7]|0x20)=='e')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_LAST_NAME];
            }
        } break;
        case 9: {
            switch(bsl::toupper(name[0])) {
                case 'B': {
                    if ((name[1]|0x20)=='i'
                     && (name[2]|0x20)=='r'
                     && (name[3]|0x20)=='t'
                     && (name[4]|0x20)=='h'
                     && (name[5]|0x20)=='d'
                     && (name[6]|0x20)=='a'
                     && (name[7]|0x20)=='t'
                     && (name[8]|0x20)=='e')
                    {
                        return
                             &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BIRTH_DATE];
                    }
                } break;
                case 'F': {
                    if ((name[1]|0x20)=='i'
                     && (name[2]|0x20)=='r'
                     && (name[3]|0x20)=='s'
                     && (name[4]|0x20)=='t'
                     && (name[5]|0x20)=='n'
                     && (name[6]|0x20)=='a'
                     && (name[7]|0x20)=='m'
                     && (name[8]|0x20)=='e')
                    {
                        return
                             &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FIRST_NAME];
                    }
                } break;
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *Person::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_LAST_NAME:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_LAST_NAME];
      case ATTRIBUTE_ID_FIRST_NAME:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FIRST_NAME];
      case ATTRIBUTE_ID_AGE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE];
      case ATTRIBUTE_ID_BIRTH_DATE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BIRTH_DATE];
      default:
        return 0;
    }
}

// CREATORS

Person::Person(bslma_Allocator *basicAllocator)
: d_lastName(basicAllocator)
, d_firstName(basicAllocator)
, d_age()
, d_birthDate()
{
}

Person::Person(
        const Person& original,
        bslma_Allocator *basicAllocator)
: d_lastName(original.d_lastName, basicAllocator)
, d_firstName(original.d_firstName, basicAllocator)
, d_age(original.d_age)
, d_birthDate(original.d_birthDate)
{
}

Person::~Person()
{
}

// MANIPULATORS

Person&
Person::operator=(const Person& rhs)
{
    if (this != &rhs) {
        d_lastName = rhs.d_lastName;
        d_firstName = rhs.d_firstName;
        d_age = rhs.d_age;
        d_birthDate = rhs.d_birthDate;
    }
    return *this;
}

int Person::fromAggregate(const bcem_Aggregate& aggregate)
{
    int rc;

    if ((rc = bcem_AggregateUtil::fromAggregate(
                       &d_lastName,
                       aggregate,
                       ATTRIBUTE_ID_LAST_NAME)) ||
        (rc = bcem_AggregateUtil::fromAggregate(
                       &d_firstName,
                       aggregate,
                       ATTRIBUTE_ID_FIRST_NAME)) ||
        (rc = bcem_AggregateUtil::fromAggregate(
                       &d_age,
                       aggregate,
                       ATTRIBUTE_ID_AGE)) ||
        (rc = bcem_AggregateUtil::fromAggregate(
                       &d_birthDate,
                       aggregate,
                       ATTRIBUTE_ID_BIRTH_DATE)))
    {
        return rc;
    }

    return 0;
}

void Person::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_lastName);
    bdeat_ValueTypeFunctions::reset(&d_firstName);
    bdeat_ValueTypeFunctions::reset(&d_age);
    bdeat_ValueTypeFunctions::reset(&d_birthDate);
}

// ACCESSORS

bsl::ostream& Person::print(
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
        stream << "LastName = ";
        bdeu_PrintMethods::print(stream, d_lastName,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "FirstName = ";
        bdeu_PrintMethods::print(stream, d_firstName,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Age = ";
        bdeu_PrintMethods::print(stream, d_age,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "BirthDate = ";
        bdeu_PrintMethods::print(stream, d_birthDate,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "LastName = ";
        bdeu_PrintMethods::print(stream, d_lastName,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "FirstName = ";
        bdeu_PrintMethods::print(stream, d_firstName,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Age = ";
        bdeu_PrintMethods::print(stream, d_age,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "BirthDate = ";
        bdeu_PrintMethods::print(stream, d_birthDate,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

int Person::toAggregate(bcem_Aggregate *result) const
{
    int rc;

    if ((rc = bcem_AggregateUtil::toAggregate(
                       result,
                       ATTRIBUTE_ID_LAST_NAME,
                       d_lastName)) ||
        (rc = bcem_AggregateUtil::toAggregate(
                       result,
                       ATTRIBUTE_ID_FIRST_NAME,
                       d_firstName)) ||
        (rc = bcem_AggregateUtil::toAggregate(
                       result,
                       ATTRIBUTE_ID_AGE,
                       d_age)) ||
        (rc = bcem_AggregateUtil::toAggregate(
                       result,
                       ATTRIBUTE_ID_BIRTH_DATE,
                       d_birthDate)))
    {
        return rc;
    }

    return 0;
}

                               // -------------
                               // class Company
                               // -------------

// CONSTANTS

const char Company::CLASS_NAME[] = "Company";
    // the name of this class

const bdeat_AttributeInfo Company::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_NAME,
        "Name",             // name
        sizeof("Name") - 1, // name length
        "",  // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_ACCOUNT_NUM,
        "AccountNum",             // name
        sizeof("AccountNum") - 1, // name length
        "",  // annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *Company::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 4: {
            if ((name[0]|0x20)=='n'
             && (name[1]|0x20)=='a'
             && (name[2]|0x20)=='m'
             && (name[3]|0x20)=='e')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME];
            }
        } break;
        case 10: {
            if ((name[0]|0x20)=='a'
             && (name[1]|0x20)=='c'
             && (name[2]|0x20)=='c'
             && (name[3]|0x20)=='o'
             && (name[4]|0x20)=='u'
             && (name[5]|0x20)=='n'
             && (name[6]|0x20)=='t'
             && (name[7]|0x20)=='n'
             && (name[8]|0x20)=='u'
             && (name[9]|0x20)=='m')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCOUNT_NUM];
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *Company::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_NAME:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME];
      case ATTRIBUTE_ID_ACCOUNT_NUM:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCOUNT_NUM];
      default:
        return 0;
    }
}

// CREATORS

Company::Company(bslma_Allocator *basicAllocator)
: d_name(basicAllocator)
, d_accountNum()
{
}

Company::Company(
        const Company& original,
        bslma_Allocator *basicAllocator)
: d_name(original.d_name, basicAllocator)
, d_accountNum(original.d_accountNum)
{
}

Company::~Company()
{
}

// MANIPULATORS

Company&
Company::operator=(const Company& rhs)
{
    if (this != &rhs) {
        d_name = rhs.d_name;
        d_accountNum = rhs.d_accountNum;
    }
    return *this;
}

int Company::fromAggregate(const bcem_Aggregate& aggregate)
{
    int rc;

    if ((rc = bcem_AggregateUtil::fromAggregate(
                       &d_name,
                       aggregate,
                       ATTRIBUTE_ID_NAME)) ||
        (rc = bcem_AggregateUtil::fromAggregate(
                       &d_accountNum,
                       aggregate,
                       ATTRIBUTE_ID_ACCOUNT_NUM)))
    {
        return rc;
    }

    return 0;
}

void Company::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_name);
    bdeat_ValueTypeFunctions::reset(&d_accountNum);
}

// ACCESSORS

bsl::ostream& Company::print(
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
        stream << "AccountNum = ";
        bdeu_PrintMethods::print(stream, d_accountNum,
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
        stream << "AccountNum = ";
        bdeu_PrintMethods::print(stream, d_accountNum,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

int Company::toAggregate(bcem_Aggregate *result) const
{
    int rc;

    if ((rc = bcem_AggregateUtil::toAggregate(
                       result,
                       ATTRIBUTE_ID_NAME,
                       d_name)) ||
        (rc = bcem_AggregateUtil::toAggregate(
                       result,
                       ATTRIBUTE_ID_ACCOUNT_NUM,
                       d_accountNum)))
    {
        return rc;
    }

    return 0;
}

                                // ------------
                                // class Entity
                                // ------------

// CONSTANTS

const char Entity::CLASS_NAME[] = "Entity";
    // the name of this class

const bdeat_SelectionInfo Entity::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_CORP,
        "Corp",               // name
        sizeof("Corp") - 1,   // name length
        "",  // annotation
        bdeat_FormattingMode::BDEAT_DEFAULT // formatting mode
    },
    {
        SELECTION_ID_HUMAN,
        "Human",               // name
        sizeof("Human") - 1,   // name length
        "",  // annotation
        bdeat_FormattingMode::BDEAT_DEFAULT // formatting mode
    }
};

// CLASS METHODS

const bdeat_SelectionInfo *Entity::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 4: {
            if ((name[0]|0x20)=='c'
             && (name[1]|0x20)=='o'
             && (name[2]|0x20)=='r'
             && (name[3]|0x20)=='p')
            {
                return &SELECTION_INFO_ARRAY[SELECTION_INDEX_CORP];
            }
        } break;
        case 5: {
            if ((name[0]|0x20)=='h'
             && (name[1]|0x20)=='u'
             && (name[2]|0x20)=='m'
             && (name[3]|0x20)=='a'
             && (name[4]|0x20)=='n')
            {
                return &SELECTION_INFO_ARRAY[SELECTION_INDEX_HUMAN];
            }
        } break;
    }
    return 0;
}

const bdeat_SelectionInfo *Entity::lookupSelectionInfo(int id)
{
    switch (id) {
      case SELECTION_ID_CORP:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_CORP];
      case SELECTION_ID_HUMAN:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_HUMAN];
      default:
        return 0;
    }
}

// CREATORS

inline
Entity::Entity(
    const Entity& original,
    bslma_Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_CORP: {
        new (d_corp.buffer())
            Company(
                original.d_corp.object(), d_allocator_p);
      } break;
      case SELECTION_ID_HUMAN: {
        new (d_human.buffer())
            Person(
                original.d_human.object(), d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

// MANIPULATORS

Entity&
Entity::operator=(const Entity& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_CORP: {
            makeCorp(rhs.d_corp.object());
          } break;
          case SELECTION_ID_HUMAN: {
            makeHuman(rhs.d_human.object());
          } break;
          default:
            BSLS_ASSERT(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }
    return *this;
}

int Entity::fromAggregate(const bcem_Aggregate& aggregate)
{
    int rc = 0;

    int selectorId = aggregate.selectorId();
    switch (selectorId) {
      case SELECTION_ID_CORP: {
        rc = bcem_AggregateUtil::fromAggregate(&makeCorp(),
                                               aggregate,
                                               selectorId);
      } break;
      case SELECTION_ID_HUMAN: {
        rc = bcem_AggregateUtil::fromAggregate(&makeHuman(),
                                               aggregate,
                                               selectorId);
      } break;
      default: {
        rc = bcem_Aggregate::BCEM_ERR_BAD_CONVERSION;
      }
    }

    return rc;
}

void Entity::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_CORP: {
        d_corp.object().~Company();
      } break;
      case SELECTION_ID_HUMAN: {
        d_human.object().~Person();
      } break;
      default:
        BSLS_ASSERT_SAFE(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

int Entity::makeSelection(int selectionId)
{
    enum { NOT_FOUND = -1, SUCCESS = 0 };

    switch (selectionId) {
      case SELECTION_ID_CORP: {
        makeCorp();
      } break;
      case SELECTION_ID_HUMAN: {
        makeHuman();
      } break;
      case SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return NOT_FOUND;
    }
    return SUCCESS;
}

int Entity::makeSelection(const char *name, int nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdeat_SelectionInfo *selectionInfo =
           lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return NOT_FOUND;
    }

    return makeSelection(selectionInfo->d_id);
}

Company& Entity::makeCorp()
{
    if (SELECTION_ID_CORP == d_selectionId) {
        bdeat_ValueTypeFunctions::reset(&d_corp.object());
    }
    else {
        reset();
        new (d_corp.buffer())
                Company(d_allocator_p);

        d_selectionId = SELECTION_ID_CORP;
    }

    return d_corp.object();
}

Company& Entity::makeCorp(const Company& value)
{
    if (SELECTION_ID_CORP == d_selectionId) {
        d_corp.object() = value;
    }
    else {
        reset();
        new (d_corp.buffer())
                Company(value, d_allocator_p);
        d_selectionId = SELECTION_ID_CORP;
    }

    return d_corp.object();
}

Person& Entity::makeHuman()
{
    if (SELECTION_ID_HUMAN == d_selectionId) {
        bdeat_ValueTypeFunctions::reset(&d_human.object());
    }
    else {
        reset();
        new (d_human.buffer())
                Person(d_allocator_p);

        d_selectionId = SELECTION_ID_HUMAN;
    }

    return d_human.object();
}

Person& Entity::makeHuman(const Person& value)
{
    if (SELECTION_ID_HUMAN == d_selectionId) {
        d_human.object() = value;
    }
    else {
        reset();
        new (d_human.buffer())
                Person(value, d_allocator_p);
        d_selectionId = SELECTION_ID_HUMAN;
    }

    return d_human.object();
}

// ACCESSORS

bsl::ostream& Entity::print(
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
          case SELECTION_ID_CORP: {
            stream << "Corp = ";
            bdeu_PrintMethods::print(stream, d_corp.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_HUMAN: {
            stream << "Human = ";
            bdeu_PrintMethods::print(stream, d_human.object(),
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
          case SELECTION_ID_CORP: {
            stream << "Corp = ";
            bdeu_PrintMethods::print(stream, d_corp.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_HUMAN: {
            stream << "Human = ";
            bdeu_PrintMethods::print(stream, d_human.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED";
        }

        stream << " ]";
    }

    return stream << bsl::flush;
}

int Entity::toAggregate(bcem_Aggregate *result) const
{
    bcem_Aggregate selection = result->makeSelectionById(d_selectionId);
    if (selection.isError()) {
        return selection.errorCode();
    }

    int rc = 0;

    switch (d_selectionId) {
      case SELECTION_ID_CORP: {
        const Company& source = corp();
        rc = bcem_AggregateUtil::toAggregate(result,
                                             d_selectionId,
                                             source);
      } break;
      case SELECTION_ID_HUMAN: {
        const Person& source = human();
        rc = bcem_AggregateUtil::toAggregate(result,
                                             d_selectionId,
                                             source);
      } break;
      default:
        BSLS_ASSERT_SAFE(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    return rc;
}

}  // close namespace test
}  // close namespace BloombergLP

// GENERATED BY BLP_BAS_CODEGEN_3.0.9 Tue Jun 24 15:47:19 2008
// ----------------------------------------------------------------------------
// *Generated end-of-file block removed.*
// ----------------------------------------------------------------------------

// END GENERATED CODE

//=============================================================================
//                      GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------
namespace {

int loadPrimitives(bcem_Aggregate *result)
{
    // Load into the specified 'result' a 'bcem_Aggregate' constructed
    // according to the following XSD.  Return 0 on success, and a non-zero
    // value otherwise.
    //..
    //  <?xml version='1.0' encoding='UTF-8'?>
    //  <xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'
    //             xmlns:bdem='http://bloomberg.com/schemas/bdem'
    //             elementFormDefault='qualified'>
    //
    //    <xs:complexType name='Primitives'>
    //      <xs:sequence>
    //        <xs:element name='value1'  type='xs:boolean'/>
    //        <xs:element name='value2'  type='xs:byte'/>
    //        <xs:element name='value3'  type='xs:short'/>
    //        <xs:element name='value4'  type='xs:int'/>
    //        <xs:element name='value5'  type='xs:float'/>
    //        <xs:element name='value6'  type='xs:double'/>
    //        <xs:element name='value7'  type='xs:unsignedByte'/>
    //        <xs:element name='value8'  type='xs:unsignedShort'/>
    //        <xs:element name='value9'  type='xs:unsignedInt'/>
    //        <xs:element name='value10' type='xs:string'/>
    //        <xs:element name='value11' type='xs:long'/>
    //        <xs:element name='value12' type='xs:unsignedLong'/>
    //        <xs:element name='value13' type='xs:date'/>
    //        <xs:element name='value14' type='xs:dateTime'/>
    //        <xs:element name='value15' type='xs:time'/>
    //
    //        <xs:element name='value16' type='xs:date'
    //                                   bdem:cpptype='bdet_Date'/>
    //        <xs:element name='value17' type='xs:dateTime'
    //                                   bdem:cpptype='bdet_Datetime'/>
    //        <xs:element name='value18' type='xs:time'
    //                                   bdem:cpptype='bdet_Time'/>
    //      </xs:sequence>
    //    </xs:complexType>
    //
    //    <xs:element name='primitives' type='Primitives'/>
    //
    //  </xs:schema>
    //..

    bcema_SharedPtr<bdem_Schema> schema_sp;
    schema_sp.createInplace();

    bdem_RecordDef *rd = schema_sp->createRecord("primitives");
    bdem_FieldDefAttributes  fa;
#define RFA(T) fa.reset(T); fa.setIsNullable(false);
    RFA(bdem_ElemType::BDEM_BOOL);   rd->appendField(fa, "value1");
    RFA(bdem_ElemType::BDEM_CHAR);   rd->appendField(fa, "value2");
    RFA(bdem_ElemType::BDEM_SHORT);  rd->appendField(fa, "value3");
    RFA(bdem_ElemType::BDEM_INT);    rd->appendField(fa, "value4");
    RFA(bdem_ElemType::BDEM_FLOAT);  rd->appendField(fa, "value5");
    RFA(bdem_ElemType::BDEM_DOUBLE); rd->appendField(fa, "value6");
    // unsigned char
    RFA(bdem_ElemType::BDEM_CHAR);   rd->appendField(fa, "value7");
    // unsigned short
    RFA(bdem_ElemType::BDEM_SHORT);  rd->appendField(fa, "value8");
    // unsigned int
    RFA(bdem_ElemType::BDEM_INT);    rd->appendField(fa, "value9");
    RFA(bdem_ElemType::BDEM_STRING); rd->appendField(fa, "value10");
    RFA(bdem_ElemType::BDEM_INT64);  rd->appendField(fa, "value11");
    RFA(bdem_ElemType::BDEM_INT64);  rd->appendField(fa, "value12");// Uint64
    RFA(bdem_ElemType::BDEM_DATETZ);     rd->appendField(fa, "value13");
    RFA(bdem_ElemType::BDEM_DATETIMETZ); rd->appendField(fa, "value14");
    RFA(bdem_ElemType::BDEM_TIMETZ);     rd->appendField(fa, "value15");
    RFA(bdem_ElemType::BDEM_DATE);       rd->appendField(fa, "value16");
    RFA(bdem_ElemType::BDEM_DATETIME);   rd->appendField(fa, "value17");
    RFA(bdem_ElemType::BDEM_TIME);       rd->appendField(fa, "value18");
#undef RFA

    bcem_Aggregate aggregate(schema_sp, "primitives");
    *result = aggregate;

    bdet_Date       myDate(2006, 2, 10);
    bdet_Time       myTime(18, 5);
    bdet_Datetime   myDatetime(myDate, myTime);
    bdet_DateTz     myDateTz(myDate, -5 * 60);
    bdet_TimeTz     myTimeTz(myTime, -5 * 60);
    bdet_DatetimeTz myDatetimeTz(myDatetime, -5 * 60);

    return result->setFieldById(ID_VAL1,  false).errorCode()
        || result->setFieldById(ID_VAL2,  NLC::max()).errorCode()
        || result->setFieldById(ID_VAL3,  NLS::max()).errorCode()
        || result->setFieldById(ID_VAL4,  NLI::max()).errorCode()
        || result->setFieldById(ID_VAL5,  NLF::max()).errorCode()
        || result->setFieldById(ID_VAL6,  NLD::max()).errorCode()
        || result->setFieldById(ID_VAL7,  NLUC::max()).errorCode()
        || result->setFieldById(ID_VAL8,  NLUS::max()).errorCode()
        || result->setFieldById(ID_VAL9,  NLUI::max()).errorCode()
        || result->setFieldById(ID_VAL10, "This is a string").errorCode()
        || result->setFieldById(ID_VAL11, NLI64::max()).errorCode()
        || result->setFieldById(ID_VAL12, NLUI64::max()).errorCode()
        || result->setFieldById(ID_VAL13, myDateTz).errorCode()
        || result->setFieldById(ID_VAL14, myDatetimeTz).errorCode()
        || result->setFieldById(ID_VAL15, myTimeTz).errorCode()
        || result->setFieldById(ID_VAL16, myDate).errorCode()
        || result->setFieldById(ID_VAL17, myDatetime).errorCode()
        || result->setFieldById(ID_VAL18, myTime).errorCode();
}

int loadNullables(bcem_Aggregate *result)
{
    // Load into the specified 'result' a 'bcem_Aggregate' constructed
    // according to the following XSD.  Return 0 on success, and a non-zero
    // value otherwise.
    //..
    //  <?xml version='1.0' encoding='UTF-8'?>
    //  <xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'
    //             xmlns:bdem='http://bloomberg.com/schemas/bdem'
    //             elementFormDefault='qualified'>
    //
    //    <xs:complexType name='Nullables'>
    //      <xs:sequence>
    //        <xs:element name='value1'  type='xs:boolean'
    //                                   minOccurs='0'/>
    //
    //        <xs:element name='value2'  type='xs:byte'
    //                                   minOccurs='0'/>
    //
    //        <xs:element name='value3'  type='xs:short'
    //                                   minOccurs='0'/>
    //
    //        <xs:element name='value4'  type='xs:int'
    //                                   minOccurs='0'/>
    //
    //        <xs:element name='value5'  type='xs:float'
    //                                   minOccurs='0'/>
    //
    //        <xs:element name='value6'  type='xs:double'
    //                                   minOccurs='0'/>
    //
    //        <xs:element name='value7'  type='xs:unsignedByte'
    //                                   minOccurs='0'/>
    //
    //        <xs:element name='value8'  type='xs:unsignedShort'
    //                                   minOccurs='0'/>
    //
    //        <xs:element name='value9'  type='xs:unsignedInt'
    //                                   minOccurs='0'/>
    //
    //        <xs:element name='value10' type='xs:string'
    //                                   minOccurs='0'/>
    //
    //        <xs:element name='value11' type='xs:long'
    //                                   minOccurs='0'/>
    //
    //        <xs:element name='value12' type='xs:unsignedLong'
    //                                   minOccurs='0'/>
    //
    //        <xs:element name='value13' type='xs:date'
    //                                   minOccurs='0'/>
    //
    //        <xs:element name='value14' type='xs:dateTime'
    //                                   minOccurs='0'/>
    //
    //        <xs:element name='value15' type='xs:time'
    //                                   minOccurs='0'/>
    //
    //
    //        <xs:element name='value16' type='xs:date'
    //                                   minOccurs='0'
    //                                   bdem:cpptype='bdet_Date'/>
    //
    //        <xs:element name='value17' type='xs:dateTime'
    //                                   minOccurs='0'
    //                                   bdem:cpptype='bdet_Datetime'/>
    //
    //        <xs:element name='value18' type='xs:time'
    //                                   minOccurs='0'
    //                                   bdem:cpptype='bdet_Time'/>
    //
    //      </xs:sequence>
    //    </xs:complexType>
    //
    //    <xs:element name='nullables' type='Nullables'/>
    //
    //  </xs:schema>
    //..

    bcema_SharedPtr<bdem_Schema> schema_sp;
    schema_sp.createInplace();

    bdem_RecordDef *rd = schema_sp->createRecord("nullables");
    bdem_FieldDefAttributes  fa;
#define RFA(T) fa.reset(T); fa.setIsNullable(true);
    RFA(bdem_ElemType::BDEM_BOOL);   rd->appendField(fa, "value1");
    RFA(bdem_ElemType::BDEM_CHAR);   rd->appendField(fa, "value2");
    RFA(bdem_ElemType::BDEM_SHORT);  rd->appendField(fa, "value3");
    RFA(bdem_ElemType::BDEM_INT);    rd->appendField(fa, "value4");
    RFA(bdem_ElemType::BDEM_FLOAT);  rd->appendField(fa, "value5");
    RFA(bdem_ElemType::BDEM_DOUBLE); rd->appendField(fa, "value6");
    // unsigned char
    RFA(bdem_ElemType::BDEM_CHAR);   rd->appendField(fa, "value7");
    // unsigned short
    RFA(bdem_ElemType::BDEM_SHORT);  rd->appendField(fa, "value8");
    // unsigned int
    RFA(bdem_ElemType::BDEM_INT);    rd->appendField(fa, "value9");
    RFA(bdem_ElemType::BDEM_STRING); rd->appendField(fa, "value10");
    RFA(bdem_ElemType::BDEM_INT64);  rd->appendField(fa, "value11");
    RFA(bdem_ElemType::BDEM_INT64);  rd->appendField(fa, "value12");// Uint64
    RFA(bdem_ElemType::BDEM_DATETZ);     rd->appendField(fa, "value13");
    RFA(bdem_ElemType::BDEM_DATETIMETZ); rd->appendField(fa, "value14");
    RFA(bdem_ElemType::BDEM_TIMETZ);     rd->appendField(fa, "value15");
    RFA(bdem_ElemType::BDEM_DATE);       rd->appendField(fa, "value16");
    RFA(bdem_ElemType::BDEM_DATETIME);   rd->appendField(fa, "value17");
    RFA(bdem_ElemType::BDEM_TIME);       rd->appendField(fa, "value18");
#undef RFA

    bcem_Aggregate aggregate(schema_sp, "nullables");
    *result = aggregate;

    return result->fieldById(ID_VAL1).makeNull().errorCode()
        || result->fieldById(ID_VAL2).makeNull().errorCode()
        || result->fieldById(ID_VAL3).makeNull().errorCode()
        || result->fieldById(ID_VAL4).makeNull().errorCode()
        || result->fieldById(ID_VAL5).makeNull().errorCode()
        || result->fieldById(ID_VAL6).makeNull().errorCode()
        || result->fieldById(ID_VAL7).makeNull().errorCode()
        || result->fieldById(ID_VAL8).makeNull().errorCode()
        || result->fieldById(ID_VAL9).makeNull().errorCode()
        || result->fieldById(ID_VAL10).makeNull().errorCode()
        || result->fieldById(ID_VAL11).makeNull().errorCode()
        || result->fieldById(ID_VAL12).makeNull().errorCode()
        || result->fieldById(ID_VAL13).makeNull().errorCode()
        || result->fieldById(ID_VAL14).makeNull().errorCode()
        || result->fieldById(ID_VAL15).makeNull().errorCode()
        || result->fieldById(ID_VAL16).makeNull().errorCode()
        || result->fieldById(ID_VAL17).makeNull().errorCode()
        || result->fieldById(ID_VAL18).makeNull().errorCode();
}

int loadArrays(bcem_Aggregate *result)
{
    // Load into the specified 'result' a 'bcem_Aggregate' constructed
    // according to the following XSD.  Return 0 on success, and a non-zero
    // value otherwise.
    //..
    //  <?xml version='1.0' encoding='UTF-8'?>
    //  <xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'
    //             elementFormDefault='qualified'>
    //
    //    <xs:complexType name='Nullables'>
    //      <xs:sequence>
    //        <xs:element name='value1' type='xs:boolean'
    //                                  minOccurs='0'
    //                                  maxOccurs='unbounded'/>
    //
    //        <xs:element name='value2' type='xs:byte'
    //                                  minOccurs='0'
    //                                  maxOccurs='unbounded'/>
    //
    //        <xs:element name='value3' type='xs:short'
    //                                  minOccurs='0'
    //                                  maxOccurs='unbounded'/>
    //
    //        <xs:element name='value4' type='xs:int'
    //                                  minOccurs='0'
    //                                  maxOccurs='unbounded'/>
    //
    //        <xs:element name='value5' type='xs:float'
    //                                  minOccurs='0'
    //                                  maxOccurs='unbounded'/>
    //
    //        <xs:element name='value6' type='xs:double'
    //                                  minOccurs='0'
    //                                  maxOccurs='unbounded'/>
    //
    //        <xs:element name='value7' type='xs:unsignedByte'
    //                                  minOccurs='0'
    //                                  maxOccurs='unbounded'/>
    //
    //        <xs:element name='value8' type='xs:unsignedShort'
    //                                  minOccurs='0'
    //                                  maxOccurs='unbounded'/>
    //
    //        <xs:element name='value9' type='xs:unsignedInt'
    //                                  minOccurs='0'
    //                                  maxOccurs='unbounded'/>
    //
    //        <xs:element name='value0' type='xs:string'
    //                                  minOccurs='0'
    //                                  maxOccurs='unbounded'/>
    //
    //      </xs:sequence>
    //    </xs:complexType>
    //
    //    <xs:element name='nullables' type='Nullables'/>
    //
    //  </xs:schema>
    //..

    bcema_SharedPtr<bdem_Schema> schema_sp;
    schema_sp.createInplace();

    bdem_RecordDef *rd = schema_sp->createRecord("arrays");
    rd->appendField(bdem_ElemType::BDEM_BOOL_ARRAY,   "value1");
    rd->appendField(bdem_ElemType::BDEM_CHAR_ARRAY,   "value2");
    rd->appendField(bdem_ElemType::BDEM_SHORT_ARRAY,  "value3");
    rd->appendField(bdem_ElemType::BDEM_INT_ARRAY,    "value4");
    rd->appendField(bdem_ElemType::BDEM_FLOAT_ARRAY,  "value5");
    rd->appendField(bdem_ElemType::BDEM_DOUBLE_ARRAY, "value6");
    // unsigned char
    rd->appendField(bdem_ElemType::BDEM_CHAR_ARRAY,   "value7");
    // unsigned short
    rd->appendField(bdem_ElemType::BDEM_SHORT_ARRAY,  "value8");
    // unsigned int
    rd->appendField(bdem_ElemType::BDEM_INT_ARRAY,    "value9");
    rd->appendField(bdem_ElemType::BDEM_STRING_ARRAY, "value10");
    rd->appendField(bdem_ElemType::BDEM_INT64_ARRAY,  "value11");
    rd->appendField(bdem_ElemType::BDEM_INT64_ARRAY,  "value12"); // Uint64
    rd->appendField(bdem_ElemType::BDEM_DATETZ_ARRAY,     "value13");
    rd->appendField(bdem_ElemType::BDEM_DATETIMETZ_ARRAY, "value14");
    rd->appendField(bdem_ElemType::BDEM_TIMETZ_ARRAY,     "value15");
    rd->appendField(bdem_ElemType::BDEM_DATE_ARRAY,       "value16");
    rd->appendField(bdem_ElemType::BDEM_DATETIME_ARRAY,   "value17");
    rd->appendField(bdem_ElemType::BDEM_TIME_ARRAY,       "value18");

    bcem_Aggregate aggregate(schema_sp, "arrays");
    *result = aggregate;

    typedef bsls_PlatformUtil::Int64  Int64;
    typedef bsls_PlatformUtil::Uint64 Uint64;

    typedef bsl::vector<bool>            VB;
    typedef bsl::vector<char>            VC;
    typedef bsl::vector<short>           VS;
    typedef bsl::vector<int>             VI;
    typedef bsl::vector<float>           VF;
    typedef bsl::vector<double>          VD;
//  typedef bsl::vector<unsigned char>   VUC;
//  typedef bsl::vector<unsigned short>  VUS;
//  typedef bsl::vector<unsigned int>    VUI;
    typedef bsl::vector<bsl::string>     VSS;
    typedef bsl::vector<Int64>           VI64;
//  typedef bsl::vector<Uint64>          VUI64;
    typedef bsl::vector<bdet_DateTz>     VXDTZ;
    typedef bsl::vector<bdet_DatetimeTz> VXDTTZ;
    typedef bsl::vector<bdet_TimeTz>     VXTTZ;
    typedef bsl::vector<bdet_Date>       VXD;
    typedef bsl::vector<bdet_Datetime>   VXDT;
    typedef bsl::vector<bdet_Time>       VXT;

    return result->setFieldById(ID_VAL1,  VB()).errorCode()
        || result->setFieldById(ID_VAL2,  VC()).errorCode()
        || result->setFieldById(ID_VAL3,  VS()).errorCode()
        || result->setFieldById(ID_VAL4,  VI()).errorCode()
        || result->setFieldById(ID_VAL5,  VF()).errorCode()
        || result->setFieldById(ID_VAL6,  VD()).errorCode()
        || result->setFieldById(ID_VAL7,  VC()).errorCode()   // unsigned char
        || result->setFieldById(ID_VAL8,  VS()).errorCode()   // unsigned short
        || result->setFieldById(ID_VAL9,  VI()).errorCode()   // unsigned int
        || result->setFieldById(ID_VAL10, VSS()).errorCode()
        || result->setFieldById(ID_VAL11, VI64()).errorCode()
        || result->setFieldById(ID_VAL12, VI64()).errorCode() // Uint64
        || result->setFieldById(ID_VAL13, VXDTZ()).errorCode()
        || result->setFieldById(ID_VAL14, VXDTTZ()).errorCode()
        || result->setFieldById(ID_VAL15, VXTTZ()).errorCode()
        || result->setFieldById(ID_VAL16, VXD()).errorCode()
        || result->setFieldById(ID_VAL17, VXDT()).errorCode()
        || result->setFieldById(ID_VAL18, VXT()).errorCode();
}

int loadArraysOfNullableValues(bcem_Aggregate *result)
{
    // Load into the specified 'result' a 'bcem_Aggregate' constructed
    // according to the following XSD.  Return 0 on success, and a non-zero
    // value otherwise.
    //..
    //  <?xml version='1.0' encoding='UTF-8'?>
    //  <xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'
    //             elementFormDefault='qualified'>
    //
    //    <xs:complexType name='Nullables'>
    //      <xs:sequence>
    //        <xs:element name='value1' type='xs:boolean'
    //                                  nillable='true'
    //                                  minOccurs='0'
    //                                  maxOccurs='unbounded'/>
    //
    //        <xs:element name='value2' type='xs:byte'
    //                                  nillable='true'
    //                                  minOccurs='0'
    //                                  maxOccurs='unbounded'/>
    //
    //        <xs:element name='value3' type='xs:short'
    //                                  nillable='true'
    //                                  minOccurs='0'
    //                                  maxOccurs='unbounded'/>
    //
    //        <xs:element name='value4' type='xs:int'
    //                                  nillable='true'
    //                                  minOccurs='0'
    //                                  maxOccurs='unbounded'/>
    //
    //        <xs:element name='value5' type='xs:float'
    //                                  nillable='true'
    //                                  minOccurs='0'
    //                                  maxOccurs='unbounded'/>
    //
    //        <xs:element name='value6' type='xs:double'
    //                                  nillable='true'
    //                                  minOccurs='0'
    //                                  maxOccurs='unbounded'/>
    //
    //        <xs:element name='value7' type='xs:unsignedByte'
    //                                  nillable='true'
    //                                  minOccurs='0'
    //                                  maxOccurs='unbounded'/>
    //
    //        <xs:element name='value8' type='xs:unsignedShort'
    //                                  nillable='true'
    //                                  minOccurs='0'
    //                                  maxOccurs='unbounded'/>
    //
    //        <xs:element name='value9' type='xs:unsignedInt'
    //                                  nillable='true'
    //                                  minOccurs='0'
    //                                  maxOccurs='unbounded'/>
    //
    //        <xs:element name='value0' type='xs:string'
    //                                  nillable='true'
    //                                  minOccurs='0'
    //                                  maxOccurs='unbounded'/>
    //
    //      </xs:sequence>
    //    </xs:complexType>
    //
    //    <xs:element name='nullables' type='Nullables'/>
    //
    //  </xs:schema>
    //..

    bcema_SharedPtr<bdem_Schema> schema_sp;
    schema_sp.createInplace();

    bdem_RecordDef *rd = schema_sp->createRecord("arrays");
    rd->appendField(bdem_ElemType::BDEM_BOOL_ARRAY,   "value1");
    rd->appendField(bdem_ElemType::BDEM_CHAR_ARRAY,   "value2");
    rd->appendField(bdem_ElemType::BDEM_SHORT_ARRAY,  "value3");
    rd->appendField(bdem_ElemType::BDEM_INT_ARRAY,    "value4");
    rd->appendField(bdem_ElemType::BDEM_FLOAT_ARRAY,  "value5");
    rd->appendField(bdem_ElemType::BDEM_DOUBLE_ARRAY, "value6");
    // unsigned char
    rd->appendField(bdem_ElemType::BDEM_CHAR_ARRAY,   "value7");
    // unsigned short
    rd->appendField(bdem_ElemType::BDEM_SHORT_ARRAY,  "value8");
    // unsigned int
    rd->appendField(bdem_ElemType::BDEM_INT_ARRAY,    "value9");
    rd->appendField(bdem_ElemType::BDEM_STRING_ARRAY, "value10");
    rd->appendField(bdem_ElemType::BDEM_INT64_ARRAY,  "value11");
    rd->appendField(bdem_ElemType::BDEM_INT64_ARRAY,  "value12"); // Uint64
    rd->appendField(bdem_ElemType::BDEM_DATETZ_ARRAY,     "value13");
    rd->appendField(bdem_ElemType::BDEM_DATETIMETZ_ARRAY, "value14");
    rd->appendField(bdem_ElemType::BDEM_TIMETZ_ARRAY,     "value15");
    rd->appendField(bdem_ElemType::BDEM_DATE_ARRAY,       "value16");
    rd->appendField(bdem_ElemType::BDEM_DATETIME_ARRAY,   "value17");
    rd->appendField(bdem_ElemType::BDEM_TIME_ARRAY,       "value18");

    bcem_Aggregate aggregate(schema_sp, "arrays");
    *result = aggregate;

    typedef bsls_PlatformUtil::Int64  Int64;
    typedef bsls_PlatformUtil::Uint64 Uint64;

    typedef bsl::vector<bool>            VB;
    typedef bsl::vector<char>            VC;
    typedef bsl::vector<short>           VS;
    typedef bsl::vector<int>             VI;
    typedef bsl::vector<float>           VF;
    typedef bsl::vector<double>          VD;
//  typedef bsl::vector<unsigned char>   VUC;
//  typedef bsl::vector<unsigned short>  VUS;
//  typedef bsl::vector<unsigned int>    VUI;
    typedef bsl::vector<bsl::string>     VSS;
    typedef bsl::vector<Int64>           VI64;
//  typedef bsl::vector<Uint64>          VUI64;
    typedef bsl::vector<bdet_DateTz>     VXDTZ;
    typedef bsl::vector<bdet_DatetimeTz> VXDTTZ;
    typedef bsl::vector<bdet_TimeTz>     VXTTZ;
    typedef bsl::vector<bdet_Date>       VXD;
    typedef bsl::vector<bdet_Datetime>   VXDT;
    typedef bsl::vector<bdet_Time>       VXT;

    return result->setFieldById(ID_VAL1,  VB()).errorCode()
        || result->setFieldById(ID_VAL2,  VC()).errorCode()
        || result->setFieldById(ID_VAL3,  VS()).errorCode()
        || result->setFieldById(ID_VAL4,  VI()).errorCode()
        || result->setFieldById(ID_VAL5,  VF()).errorCode()
        || result->setFieldById(ID_VAL6,  VD()).errorCode()
        || result->setFieldById(ID_VAL7,  VC()).errorCode()   // unsigned char
        || result->setFieldById(ID_VAL8,  VS()).errorCode()   // unsigned short
        || result->setFieldById(ID_VAL9,  VI()).errorCode()   // unsigned int
        || result->setFieldById(ID_VAL10, VSS()).errorCode()
        || result->setFieldById(ID_VAL11, VI64()).errorCode()
        || result->setFieldById(ID_VAL12, VI64()).errorCode() // Uint64
        || result->setFieldById(ID_VAL13, VXDTZ()).errorCode()
        || result->setFieldById(ID_VAL14, VXDTTZ()).errorCode()
        || result->setFieldById(ID_VAL15, VXTTZ()).errorCode()
        || result->setFieldById(ID_VAL16, VXD()).errorCode()
        || result->setFieldById(ID_VAL17, VXDT()).errorCode()
        || result->setFieldById(ID_VAL18, VXT()).errorCode();
}

template <typename TYPE>
bsl::string vectorToString(const bsl::vector<TYPE>& v)
{
    bsl::ostringstream ss;
    bsl::copy(v.begin(),
              v.end(),
              bsl::ostream_iterator<TYPE>(ss, " "));
    return ss.str();
}

}  // close unnamed namespace

//=============================================================================
//                    FUNCTIONS FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = (argc > 1) ? bsl::atoi(argv[1]) : 0;

    verbose = (argc > 2);
    veryVerbose = (argc > 3);
    veryVeryVerbose = (argc > 4);
    veryVeryVeryVerbose = (argc > 5);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 8: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Simple example illustrating how one might use an aggregate.
        //
        // Concerns: The usage example documented in the header file will
        //   compile and run as expected.
        //
        // Plan: Copy the usage example verbatim from the header file.  Change
        //   each occurrence of "assert" to "ASSERT".
        //
        // Tactics:
        //   - Add-Hoc Data Selection Method
        //   - Brute-Force implementation technique
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nUSAGE EXAMPLE"
                               << "\n=============" << bsl::endl;

        // Define the structure of the aggregate in XSD.
        const char SCHEMA[] =
            "<?xml version='1.0' encoding='UTF-8'?>"
            "<schema xmlns='http://www.w3.org/2001/XMLSchema'"
            "        xmlns:bb='http://bloomberg.com/schemas/xyz'"
            "        targetNamespace='http://bloomberg.com/schemas/xyz'"
            "        elementFormDefault='qualified'>"
            ""
            "  <complexType name='Company'>"
            "    <sequence>"
            "      <element name='Name' type='string'/>"
            "      <element name='AccountNum' type='int'/>"
            "    </sequence>"
            "  </complexType>"
            ""
            "  <complexType name='Person'>"
            "    <sequence>"
            "      <element name='LastName' type='string'/>"
            "      <element name='FirstName' type='string'/>"
            "      <element name='Age' type='int'/>"
            "      <element name='BirthDate' type='date'/>"
            "    </sequence>"
            "  </complexType>"
            ""
            "  <complexType name='Entity'>"
            "    <choice>"
            "      <element name='Corp' type='bb:Company'/>"
            "      <element name='Human' type='bb:Person'/>"
            "    </choice>"
            "  </complexType>"
            ""
            "  <element name='Entity' type='bb:Entity'/>"
            ""
            "</schema>";

        // Manually create the bdem schema.

        bslma_Allocator *allocator = bslma_Default::defaultAllocator();

        bcema_SharedPtr<bdem_Schema> schema(new (*allocator) bdem_Schema());

        bdem_RecordDef *companyRecordDef = schema->createRecord("Company");
        companyRecordDef->appendField(bdem_ElemType::BDEM_STRING, "Name");
        companyRecordDef->appendField(bdem_ElemType::BDEM_INT,   "AccountNum");

        bdem_RecordDef *personRecordDef = schema->createRecord("Person");
        personRecordDef->appendField(bdem_ElemType::BDEM_STRING,  "LastName");
        personRecordDef->appendField(bdem_ElemType::BDEM_STRING,  "FirstName");
        personRecordDef->appendField(bdem_ElemType::BDEM_INT,     "Age");
        personRecordDef->appendField(bdem_ElemType::BDEM_DATETZ,  "BirthDate");

        bdem_RecordDef *entityRecordDef =
            schema->createRecord("Entity", bdem_RecordDef::BDEM_CHOICE_RECORD);
        entityRecordDef->appendField(
                               bdem_ElemType::BDEM_LIST,
                               companyRecordDef,
                               "Corp");
        entityRecordDef->appendField(
                               bdem_ElemType::BDEM_LIST,
                               personRecordDef,
                               "Human");

        bcem_Aggregate entityAgg(schema, "Entity");

        bcem_Aggregate entityAggSelection = entityAgg.makeSelection("Human");
        entityAggSelection.setField("FirstName", "John");
        entityAggSelection.setField("LastName", "Doe");
        entityAggSelection.setField("Age", 27);
        entityAggSelection.setField("BirthDate",
                                    bdet_DateTz(bdet_Date(1980, 4, 4), -5));

        test::Entity entity;
        int rc = 0;

        rc = bcem_AggregateUtil::fromAggregate(
                                            &entity.makeHuman(),
                                            entityAgg,
                                            test::Entity::SELECTION_ID_HUMAN);
        if (0 != rc) {
            bsl::cout << bcem_AggregateUtil::errorString(rc) << bsl::endl;
            ASSERT(0);
        }

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: NULLABLE PRIMITIVE TYPES ARE CONVERTIBLE TO
        //                  NON-NULLABLE AGGREGATE FIELDS
        //
        // Concerns:
        //   - That generated fields of type 'bdeut_NullableValue<T>' (where
        //     'T' is a primitive type) can be converted to an
        //     equivalently-typed aggregate with the except that the aggregate
        //     does have the 'isNullable' flag set in its field spec.  This
        //     case simulates the situation where elements in an XSD are
        //     declared to have 'minOccurs="1"' but also 'nillable="true"'.
        //     Such elements are considered nullable by bas_codegen.pl but not
        //     considered nullable by baexml_SchemaParser.  Such elements
        //     should be convertible, despite this disparity.
        //
        //   - That 'toAggregate' returns a non-zero value when called with an
        //     invalid field id.
        //
        //   - That 'fromAggregate' returns a non-zero value when called with
        //     an invalid field id.
        //
        //   - That calling 'toAggregate' followed by 'fromAggregate' on the
        //     same aggregate with the same field id yields the initial value.
        //
        // Plan:
        //   Generate a 'bcem_Aggregate' object corresponding to a schema
        //   containing a sequence of *nullable* primitive values.  Convert
        //   each field calling the overloaded 'toAggregate' and
        //   'fromAggregate' methods using an invalid field id, and verify that
        //   an error result is returned.  Convert each field from its
        //   associated primitive type, and back again, and verify that the
        //   initial value is obtained.
        //
        // Testing:
        //   fromAggregate(bdeut_NullableValue<bool>)
        //   fromAggregate(bdeut_NullableValue<char>)
        //   fromAggregate(bdeut_NullableValue<short>)
        //   fromAggregate(bdeut_NullableValue<int>)
        //   fromAggregate(bdeut_NullableValue<float>)
        //   fromAggregate(bdeut_NullableValue<double>)
        //   fromAggregate(bdeut_NullableValue<unsigned char>)
        //   fromAggregate(bdeut_NullableValue<unsigned short>)
        //   fromAggregate(bdeut_NullableValue<unsigned int>)
        //   fromAggregate(bdeut_NullableValue<bsl::string>)
        //   fromAggregate(bdeut_NullableValue<bsls_PlatformUtil::Int64>)
        //   fromAggregate(bdeut_NullableValue<bsls_PlatformUtil::Uint64>)
        //   fromAggregate(bdeut_NullableValue<bdet_DateTz>)
        //   fromAggregate(bdeut_NullableValue<bdet_DatetimeTz>)
        //   fromAggregate(bdeut_NullableValue<bdet_TimeTz>)
        //   fromAggregate(bdeut_NullableValue<bdet_Date>)
        //   fromAggregate(bdeut_NullableValue<bdet_Datetime>)
        //   fromAggregate(bdeut_NullableValue<bdet_Time>)
        //   toAggregate(bdeut_NullableValue<bool>)
        //   toAggregate(bdeut_NullableValue<char>)
        //   toAggregate(bdeut_NullableValue<short>)
        //   toAggregate(bdeut_NullableValue<int>)
        //   toAggregate(bdeut_NullableValue<float>)
        //   toAggregate(bdeut_NullableValue<double>)
        //   toAggregate(bdeut_NullableValue<unsigned char>)
        //   toAggregate(bdeut_NullableValue<unsigned short>)
        //   toAggregate(bdeut_NullableValue<unsigned int>)
        //   toAggregate(bdeut_NullableValue<bsl::string>)
        //   toAggregate(bdeut_NullableValue<bsls_PlatformUtil::Int64>)
        //   toAggregate(bdeut_NullableValue<bsls_PlatformUtil::Uint64>)
        //   toAggregate(bdeut_NullableValue<bdet_DateTz>)
        //   toAggregate(bdeut_NullableValue<bdet_DatetimeTz>)
        //   toAggregate(bdeut_NullableValue<bdet_TimeTz>)
        //   toAggregate(bdeut_NullableValue<bdet_Date>)
        //   toAggregate(bdeut_NullableValue<bdet_Datetime>)
        //   toAggregate(bdeut_NullableValue<bdet_Time>)
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing Nullable Primitives (Non-nullable Aggregates)"
                 << endl
                 << "====================================================="
                 << endl;
        }

        if (verbose) cout << "\tPrinting test aggregate.\n" << endl;
        {
            bcem_Aggregate agg;
            ASSERT(0 == loadNullables(&agg));

            bcem_Aggregate       AGG = agg.clone();
            if (verbose) { P(AGG); }

            ASSERT(AGG.fieldById(ID_VAL1).fieldDef()->isNullable());
            ASSERT(AGG.fieldById(ID_VAL2).fieldDef()->isNullable());
            ASSERT(AGG.fieldById(ID_VAL3).fieldDef()->isNullable());
            ASSERT(AGG.fieldById(ID_VAL4).fieldDef()->isNullable());
            ASSERT(AGG.fieldById(ID_VAL5).fieldDef()->isNullable());
            ASSERT(AGG.fieldById(ID_VAL6).fieldDef()->isNullable());
            ASSERT(AGG.fieldById(ID_VAL7).fieldDef()->isNullable());
            ASSERT(AGG.fieldById(ID_VAL8).fieldDef()->isNullable());
            ASSERT(AGG.fieldById(ID_VAL9).fieldDef()->isNullable());
            ASSERT(AGG.fieldById(ID_VAL10).fieldDef()->isNullable());
            ASSERT(AGG.fieldById(ID_VAL11).fieldDef()->isNullable());
            ASSERT(AGG.fieldById(ID_VAL12).fieldDef()->isNullable());
            ASSERT(AGG.fieldById(ID_VAL13).fieldDef()->isNullable());
            ASSERT(AGG.fieldById(ID_VAL14).fieldDef()->isNullable());
            ASSERT(AGG.fieldById(ID_VAL15).fieldDef()->isNullable());
            ASSERT(AGG.fieldById(ID_VAL16).fieldDef()->isNullable());
            ASSERT(AGG.fieldById(ID_VAL17).fieldDef()->isNullable());
            ASSERT(AGG.fieldById(ID_VAL18).fieldDef()->isNullable());
        }

        if (verbose) cout << "\n\t...with 'bool'.\n" << endl;
        {
            enum { ID = ID_VAL1 };

            bcem_Aggregate agg;
            ASSERT(0 == loadNullables(&agg));

            bcem_Aggregate                      AGG = agg.clone();
            bdeut_NullableValue<bool>           VAL(true);
            bdeut_NullableValue<bool>           val(VAL);
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.isNull());
            ASSERT(VAL.value() != bdetu_Unset<bool>::unsetValue());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.reset();
            ASSERT(VAL != val);
            ASSERT(true == agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(true != agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.value() == agg.fieldById(ID).asBool());
        }

        if (verbose) cout << "\t...with 'char'.\n" << endl;
        {
            // NLC::min() cannot be used to specify a value guaranteed to be
            // not null because numeric_limits<char>::min() yields different
            // values using different compilers.

            enum { ID = ID_VAL2 };

            bcem_Aggregate agg;
            ASSERT(0 == loadNullables(&agg));

            bcem_Aggregate              AGG = agg.clone();
            bdeut_NullableValue<char>   VAL(NullTraits<char>::notUnsetValue());
            bdeut_NullableValue<char>   val(VAL);
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.isNull());

            ASSERT(VAL.value() != bdetu_Unset<char>::unsetValue());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.reset();
            ASSERT(VAL != val);
            ASSERT(true == agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(true != agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.value() == agg.fieldById(ID).asChar());
        }

        if (verbose) cout << "\t...with 'short'.\n" << endl;
        {
            enum { ID = ID_VAL3 };

            bcem_Aggregate agg;
            ASSERT(0 == loadNullables(&agg));

            bcem_Aggregate                      AGG = agg.clone();
            bdeut_NullableValue<short>          VAL(NLS::min() + 1);
            bdeut_NullableValue<short>          val(VAL);
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.isNull());
            ASSERT(VAL.value() != bdetu_Unset<short>::unsetValue());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.reset();
            ASSERT(VAL != val);
            ASSERT(true == agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(true != agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.value() == agg.fieldById(ID).asShort());
        }

        if (verbose) cout << "\t...with 'int'.\n" << endl;
        {
            enum { ID = ID_VAL4 };

            bcem_Aggregate agg;
            ASSERT(0 == loadNullables(&agg));

            bcem_Aggregate                      AGG = agg.clone();
            bdeut_NullableValue<int>            VAL(NLI::min() + 1);
            bdeut_NullableValue<int>            val(VAL);
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.isNull());
            ASSERT(VAL.value() != bdetu_Unset<int>::unsetValue());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.reset();
            ASSERT(VAL != val);
            ASSERT(true == agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(true != agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.value() == agg.fieldById(ID).asInt());
        }

        if (verbose) cout << "\t...with 'float'.\n" << endl;
        {
            enum { ID = ID_VAL5 };

            bcem_Aggregate agg;
            ASSERT(0 == loadNullables(&agg));

            bcem_Aggregate                      AGG = agg.clone();
            bdeut_NullableValue<float>          VAL(NLF::min());
            bdeut_NullableValue<float>          val(VAL);
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.isNull());
            ASSERT(VAL.value() != bdetu_Unset<float>::unsetValue());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.reset();
            ASSERT(VAL != val);
            ASSERT(true == agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(true != agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.value() == agg.fieldById(ID).asFloat());
        }

        if (verbose) cout << "\t...with 'double'.\n" << endl;
        {
            enum { ID = ID_VAL6 };

            bcem_Aggregate agg;
            ASSERT(0 == loadNullables(&agg));

            bcem_Aggregate                      AGG = agg.clone();
            bdeut_NullableValue<double>         VAL(NLD::min());
            bdeut_NullableValue<double>         val(VAL);
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.isNull());
            ASSERT(VAL.value() != bdetu_Unset<double>::unsetValue());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.reset();
            ASSERT(VAL != val);
            ASSERT(true == agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(true != agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.value() == agg.fieldById(ID).asDouble());
        }

        if (verbose) cout << "\t...with 'unsigned char'.\n" << endl;
        {
            enum { ID = ID_VAL7 };

            bcem_Aggregate agg;
            ASSERT(0 == loadNullables(&agg));

            bcem_Aggregate                      AGG = agg.clone();
            bdeut_NullableValue<unsigned char>  VAL(NLUC::min() + 1);
            bdeut_NullableValue<unsigned char>  val(VAL);
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.isNull());
            ASSERT(VAL.value() != bdetu_Unset<char>::unsetValue());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.reset();
            ASSERT(VAL != val);
            ASSERT(true == agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(true != agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.value() == (unsigned char)agg.fieldById(ID)
                                                    .asChar());
        }

        if (verbose) cout << "\t...with 'unsigned short'.\n" << endl;
        {
            enum { ID = ID_VAL8 };

            bcem_Aggregate agg;
            ASSERT(0 == loadNullables(&agg));

            bcem_Aggregate                      AGG = agg.clone();
            bdeut_NullableValue<unsigned short> VAL(NLUC::min());
            bdeut_NullableValue<unsigned short> val(VAL);
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.isNull());
            ASSERT(VAL.value() != bdetu_Unset<short>::unsetValue());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.reset();
            ASSERT(VAL != val);
            ASSERT(true == agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(true != agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.value() == (unsigned short)agg.fieldById(ID)
                                                     .asShort());
        }

        if (verbose) cout << "\t...with 'unsigned int'.\n" << endl;
        {
            enum { ID = ID_VAL9 };

            bcem_Aggregate agg;
            ASSERT(0 == loadNullables(&agg));

            bcem_Aggregate                      AGG = agg.clone();
            bdeut_NullableValue<unsigned int>   VAL(NLUC::min());
            bdeut_NullableValue<unsigned int>   val(VAL);
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.isNull());
            ASSERT(VAL.value()
                   != (unsigned int)bdetu_Unset<int>::unsetValue());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.reset();
            ASSERT(VAL != val);
            ASSERT(true == agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(true != agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.value() == (unsigned int)agg.fieldById(ID)
                                                   .asInt());
        }

        if (verbose) cout << "\t...with 'bsl::string'.\n" << endl;
        {
            enum { ID = ID_VAL10 };

            bcem_Aggregate agg;
            ASSERT(0 == loadNullables(&agg));

            bcem_Aggregate                      AGG = agg.clone();
            bdeut_NullableValue<bsl::string>    VAL("not empty");
            bdeut_NullableValue<bsl::string>    val(VAL);
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.isNull());
            ASSERT(VAL.value() != bdetu_Unset<bsl::string>::unsetValue());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.reset();
            ASSERT(VAL != val);
            ASSERT(true == agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(true != agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.value() == agg.fieldById(ID).asString());
        }

        if (verbose) cout << "\t...with 'Int64'.\n" << endl;
        {
            enum { ID = ID_VAL11 };

            bcem_Aggregate agg;
            ASSERT(0 == loadNullables(&agg));

            typedef bsls_PlatformUtil::Int64 Int64;

            bcem_Aggregate                       AGG = agg.clone();
            bdeut_NullableValue<Int64>           VAL(NLI64::min() + 1);
            bdeut_NullableValue<Int64>           val(VAL);
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.isNull());
            ASSERT(VAL.value() != bdetu_Unset<Int64>::unsetValue());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.reset();
            ASSERT(VAL != val);
            ASSERT(true == agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(true != agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.value() == agg.fieldById(ID).asInt64());
        }

        if (verbose) cout << "\t...with 'Uint64'.\n" << endl;
        {
            enum { ID = ID_VAL12 };

            bcem_Aggregate agg;
            ASSERT(0 == loadNullables(&agg));

            typedef bsls_PlatformUtil::Int64  Int64;
            typedef bsls_PlatformUtil::Uint64 Uint64;

            bcem_Aggregate                       AGG = agg.clone();
            bdeut_NullableValue<Uint64>          VAL(NLUI64::min());
            bdeut_NullableValue<Uint64>          val(VAL);
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.isNull());
            ASSERT(VAL.value() != (Uint64)bdetu_Unset<Int64>::unsetValue());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.reset();
            ASSERT(VAL != val);
            ASSERT(true == agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(true != agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.value() == (Uint64)agg.fieldById(ID).asInt64());
        }

        if (verbose) cout << "\t...with 'bdet_DateTz'.\n" << endl;
        {
            enum { ID = ID_VAL13 };

            bcem_Aggregate agg;
            ASSERT(0 == loadNullables(&agg));

            bdet_DateTz myDateTz(bdet_Date(2008, 7, 21), -5 * 60);

            bcem_Aggregate                       AGG = agg.clone();
            bdeut_NullableValue<bdet_DateTz>     VAL(myDateTz);
            bdeut_NullableValue<bdet_DateTz>     val(VAL);
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.isNull());
            ASSERT(VAL.value() != bdetu_Unset<bdet_DateTz>::unsetValue());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.reset();
            ASSERT(VAL != val);
            ASSERT(true == agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(true != agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.value() == agg.fieldById(ID).asDateTz());
        }

        if (verbose) cout << "\t...with 'bdet_DatetimeTz'.\n" << endl;
        {
            enum { ID = ID_VAL14 };

            bcem_Aggregate agg;
            ASSERT(0 == loadNullables(&agg));

            bdet_DatetimeTz myDatetimeTz(bdet_Datetime(2008, 7, 21, 16, 53),
                                         -5 * 60);

            bcem_Aggregate                       AGG = agg.clone();
            bdeut_NullableValue<bdet_DatetimeTz> VAL(myDatetimeTz);
            bdeut_NullableValue<bdet_DatetimeTz> val(VAL);
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.isNull());
            ASSERT(VAL.value() != bdetu_Unset<bdet_DatetimeTz>::unsetValue());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.reset();
            ASSERT(VAL != val);
            ASSERT(true == agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(true != agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.value() == agg.fieldById(ID).asDatetimeTz());
        }

        if (verbose) cout << "\t...with 'bdet_TimeTz'.\n" << endl;
        {
            enum { ID = ID_VAL15 };

            bcem_Aggregate agg;
            ASSERT(0 == loadNullables(&agg));

            bdet_TimeTz myTimeTz(bdet_Time(16, 53), -5 * 60);

            bcem_Aggregate                       AGG = agg.clone();
            bdeut_NullableValue<bdet_TimeTz>     VAL(myTimeTz);
            bdeut_NullableValue<bdet_TimeTz>     val(VAL);
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.isNull());
            ASSERT(VAL.value() != bdetu_Unset<bdet_TimeTz>::unsetValue());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.reset();
            ASSERT(VAL != val);
            ASSERT(true == agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(true != agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.value() == agg.fieldById(ID).asTimeTz());
        }

        if (verbose) cout << "\t...with 'bdet_Date'.\n" << endl;
        {
            enum { ID = ID_VAL16 };

            bcem_Aggregate agg;
            ASSERT(0 == loadNullables(&agg));

            bdet_Date myDate(2008, 7, 21);

            bcem_Aggregate                       AGG = agg.clone();
            bdeut_NullableValue<bdet_Date>       VAL(myDate);
            bdeut_NullableValue<bdet_Date>       val(VAL);
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.isNull());
            ASSERT(VAL.value() != bdetu_Unset<bdet_Date>::unsetValue());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.reset();
            ASSERT(VAL != val);
            ASSERT(true == agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(true != agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.value() == agg.fieldById(ID).asDate());
        }

        if (verbose) cout << "\t...with 'bdet_Datetime'.\n" << endl;
        {
            enum { ID = ID_VAL17 };

            bcem_Aggregate agg;
            ASSERT(0 == loadNullables(&agg));

            bdet_Datetime myDatetime(2008, 7, 21, 16, 53);

            bcem_Aggregate                       AGG = agg.clone();
            bdeut_NullableValue<bdet_Datetime>   VAL(myDatetime);
            bdeut_NullableValue<bdet_Datetime>   val(VAL);
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.isNull());
            ASSERT(VAL.value() != bdetu_Unset<bdet_Datetime>::unsetValue());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.reset();
            ASSERT(VAL != val);
            ASSERT(true == agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(true != agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.value() == agg.fieldById(ID).asDatetime());
        }

        if (verbose) cout << "\t...with 'bdet_Time'.\n" << endl;
        {
            enum { ID = ID_VAL18 };

            bcem_Aggregate agg;
            ASSERT(0 == loadNullables(&agg));

            bdet_Time myTime(16, 53);

            bcem_Aggregate                       AGG = agg.clone();
            bdeut_NullableValue<bdet_Time>       VAL(myTime);
            bdeut_NullableValue<bdet_Time>       val(VAL);
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.isNull());
            ASSERT(VAL.value() != bdetu_Unset<bdet_Time>::unsetValue());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.reset();
            ASSERT(VAL != val);
            ASSERT(true == agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(true != agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.value() == agg.fieldById(ID).asTime());
        }
      }  break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING SEQUENCE TYPES
        //
        // Concerns:
        //   - That 'toAggregate' returns a non-zero value when called with an
        //     invalid field id.
        //
        //   - That 'fromAggregate' returns a non-zero value when called with
        //     an invalid field id.
        //
        //   - That calling 'toAggregate' followed by 'fromAggregate' on the
        //     same aggregate with the same field id yields the initial value.
        //
        // Plan:
        //   Generate a 'bcem_Aggregate' object corresponding to a schema
        //   containing a sequence of sequences.  Convert each field
        //   calling the overloaded 'toAggregate' and 'fromAggregate' methods
        //   using an invalid field id, and verify that an error result is
        //   returned.  Convert each field from its associated primitive type,
        //   and back again, and verify that the initial value is obtained.
        //
        // Testing:
        //   fromAggregateImp(bslmf_MetaValue<SEQUENCE>)
        //   toAggregateImp(bslmf_MetaValue<SEQUENCE>)
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing Sequence Types" << endl
                 << "======================" << endl;
        }

        {
            /*
            bcem_Aggregate agg;
            ASSERT(0 == loadPrimitives(&agg));

            bcem_Aggregate       AGG = agg.clone();
            const bool           VAL = true;
            bool                 val = VAL;
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val = agg.fieldById(ID).asBool();
            ASSERT(VAL != val);
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val == agg.fieldById(ID).asBool());
            */
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING ARRAYS OF NULLABLE PRIMITIVE TYPES
        //
        // Concerns:
        //   - That 'toAggregate' returns a non-zero value when called with an
        //     invalid field id.
        //
        //   - That 'fromAggregate' returns a non-zero value when called with
        //     an invalid field id.
        //
        //   - That calling 'toAggregate' followed by 'fromAggregate' on the
        //     same aggregate with the same field id yields the initial value.
        //
        // Plan:
        //   Generate a 'bcem_Aggregate' object corresponding to a schema
        //   containing a sequence of arrays of *nullable* primitive values.
        //   Convert each field calling the overloaded 'toAggregate' and
        //   'fromAggregate' methods using an invalid field id, and verify that
        //   an error result is returned.  Convert each field from its
        //   associated primitive type, and back again, and verify that the
        //   initial value is obtained.
        //
        // Testing:
        //   fromAggregate(bsl::vector<bool>)
        //   fromAggregate(bsl::vector<char>)
        //   fromAggregate(bsl::vector<short>)
        //   fromAggregate(bsl::vector<int>)
        //   fromAggregate(bsl::vector<float>)
        //   fromAggregate(bsl::vector<double>)
        //   fromAggregate(bsl::vector<unsigned char>)
        //   fromAggregate(bsl::vector<unsigned short>)
        //   fromAggregate(bsl::vector<unsigned int>)
        //   fromAggregate(bsl::vector<bsl::string>)
        //   fromAggregate(bsl::vector<bsls_PlatformUtil::Int64>)
        //   fromAggregate(bsl::vector<bsls_PlatformUtil::Uint64>)
        //   fromAggregate(bsl::vector<bdet_DateTz>)
        //   fromAggregate(bsl::vector<bdet_DatetimeTz>)
        //   fromAggregate(bsl::vector<bdet_TimeTz>)
        //   fromAggregate(bsl::vector<bdet_Date>)
        //   fromAggregate(bsl::vector<bdet_Datetime>)
        //   fromAggregate(bsl::vector<bdet_Time>)
        //   toAggregate(bsl::vector<bool>)
        //   toAggregate(bsl::vector<char>)
        //   toAggregate(bsl::vector<short>)
        //   toAggregate(bsl::vector<int>)
        //   toAggregate(bsl::vector<float>)
        //   toAggregate(bsl::vector<double>)
        //   toAggregate(bsl::vector<unsigned char>)
        //   toAggregate(bsl::vector<unsigned short>)
        //   toAggregate(bsl::vector<unsigned int>)
        //   toAggregate(bsl::vector<bsl::string>)
        //   toAggregate(bsl::vector<bsls_PlatformUtil::Int64>)
        //   toAggregate(bsl::vector<bsls_PlatformUtil::Uint64>)
        //   toAggregate(bsl::vector<bdet_DateTz>)
        //   toAggregate(bsl::vector<bdet_DatetimeTz>)
        //   toAggregate(bsl::vector<bdet_TimeTz>)
        //   toAggregate(bsl::vector<bdet_Date>)
        //   toAggregate(bsl::vector<bdet_Datetime>)
        //   toAggregate(bsl::vector<bdet_Time>)
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing Arrays of Nullable Primitive Types" << endl
                 << "==========================================" << endl;
        }

        if (verbose) cout << "\tPrinting test aggregate.\n" << endl;
        {
            bcem_Aggregate agg;
            ASSERT(0 == loadArrays(&agg));

            bcem_Aggregate       AGG = agg.clone();
            if (verbose) { P(AGG); }
        }

        if (verbose) cout << "\n\t...with sequences of 'char'.\n" << endl;
        {
            enum { ID = ID_VAL2 };

            char   data[] = { NLC_MIN+1, NLC_MAX, NLC_MIN+1, NLC_MAX };
            enum { DATA_SIZE = sizeof(data) / sizeof(*data) };

            bcem_Aggregate agg;
            ASSERT(0 == loadArrays(&agg));

            bcem_Aggregate             AGG = agg.clone();
            bsl::vector< bdeut_NullableValue<char> > VAL(data,
                                                         data + DATA_SIZE);
            bsl::vector< bdeut_NullableValue<char> > val(VAL);

            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.empty());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.clear();
            ASSERT(VAL != val);
            ASSERT(0 == agg.fieldById(ID).size());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 <  agg.fieldById(ID).size());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.size() == (unsigned int)agg.fieldById(ID).size());
        }

        if (verbose) cout << "\t...with sequences of 'short'.\n" << endl;
        {
            enum { ID = ID_VAL3 };

            short  data[] = { NLS_MIN+1, NLS_MAX, NLS_MIN+1, NLS_MAX };
            enum { DATA_SIZE = sizeof(data) / sizeof(*data) };

            bcem_Aggregate agg;
            ASSERT(0 == loadArrays(&agg));

            bcem_Aggregate             AGG = agg.clone();
            bsl::vector< bdeut_NullableValue<short> > VAL(data,
                                                          data + DATA_SIZE);
            bsl::vector< bdeut_NullableValue<short> > val(VAL);

            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.empty());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.clear();
            ASSERT(VAL != val);
            ASSERT(0 == agg.fieldById(ID).size());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 <  agg.fieldById(ID).size());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.size() == (unsigned int)agg.fieldById(ID).size());
        }

        if (verbose) cout << "\t...with sequences of 'int'.\n" << endl;
        {
            enum { ID = ID_VAL4 };

            int    data[] = { NLI_MIN+1, NLI_MAX, NLI_MIN+1, NLI_MAX };
            enum { DATA_SIZE = sizeof(data) / sizeof(*data) };

            bcem_Aggregate agg;
            ASSERT(0 == loadArrays(&agg));

            bcem_Aggregate             AGG = agg.clone();
            bsl::vector< bdeut_NullableValue<int> > VAL(data,
                                                        data + DATA_SIZE);
            bsl::vector< bdeut_NullableValue<int> > val(VAL);

            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.empty());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.clear();
            ASSERT(VAL != val);
            ASSERT(0 == agg.fieldById(ID).size());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 <  agg.fieldById(ID).size());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.size() == (unsigned int)agg.fieldById(ID).size());
        }

        if (verbose) cout << "\t...with sequences of 'float'.\n" << endl;
        {
            enum { ID = ID_VAL5 };

            float  data[] = { NLF_MIN, NLF_MAX, 0.0f, NLF_MIN, NLF_MAX };
            enum { DATA_SIZE = sizeof(data) / sizeof(*data) };

            bcem_Aggregate agg;
            ASSERT(0 == loadArrays(&agg));

            bcem_Aggregate             AGG = agg.clone();
            bsl::vector< bdeut_NullableValue<float> > VAL(data,
                                                          data + DATA_SIZE);
            bsl::vector< bdeut_NullableValue<float> > val(VAL);

            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.empty());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.clear();
            ASSERT(VAL != val);
            ASSERT(0 == agg.fieldById(ID).size());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 <  agg.fieldById(ID).size());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.size() == (unsigned int)agg.fieldById(ID).size());
        }

        if (verbose) cout << "\t...with sequences of 'double'.\n" << endl;
        {
            enum { ID = ID_VAL6 };

            double   data[] = { NLD_MIN, NLD_MAX, 0.0, NLD_MIN, NLD_MAX };
            enum   { DATA_SIZE = sizeof(data) / sizeof(*data) };

            bcem_Aggregate agg;
            ASSERT(0 == loadArrays(&agg));

            bcem_Aggregate             AGG = agg.clone();
            bsl::vector< bdeut_NullableValue<double> > VAL(data,
                                                           data + DATA_SIZE);
            bsl::vector< bdeut_NullableValue<double> > val(VAL);

            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.empty());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.clear();
            ASSERT(VAL != val);
            ASSERT(0 == agg.fieldById(ID).size());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 <  agg.fieldById(ID).size());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.size() == (unsigned int)agg.fieldById(ID).size());
        }

        if (verbose)
            cout << "\t...with sequences of 'unsigned char'.\n" << endl;
        {
            enum { ID = ID_VAL7 };

            unsigned char data[] = { NLUC_MIN+1, NLUC_MAX,
                                     NLUC_MIN+1, NLUC_MAX };
            enum        { DATA_SIZE = sizeof(data) / sizeof(*data) };

            bcem_Aggregate agg;
            ASSERT(0 == loadArrays(&agg));

            bcem_Aggregate             AGG = agg.clone();
            bsl::vector< bdeut_NullableValue<unsigned char> > VAL(
                                                             data,
                                                             data + DATA_SIZE);
            bsl::vector< bdeut_NullableValue<unsigned char> > val(VAL);

            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.empty());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.clear();
            ASSERT(VAL != val);
            ASSERT(0 == agg.fieldById(ID).size());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 <  agg.fieldById(ID).size());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.size() == (unsigned int)agg.fieldById(ID).size());
        }

        if (verbose)
            cout << "\t...with sequences of 'unsigned short'.\n" << endl;
        {
            enum { ID = ID_VAL8 };

            unsigned short data[] = { NLUS_MIN, NLUS_MAX, NLUS_MIN, NLUS_MAX };
            enum { DATA_SIZE = sizeof(data) / sizeof(*data) };

            bcem_Aggregate agg;
            ASSERT(0 == loadArrays(&agg));

            bcem_Aggregate              AGG = agg.clone();
            bsl::vector< bdeut_NullableValue<unsigned short> > VAL(
                                                             data,
                                                             data + DATA_SIZE);
            bsl::vector< bdeut_NullableValue<unsigned short> > val(VAL);

            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.empty());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.clear();
            ASSERT(VAL != val);
            ASSERT(0 == agg.fieldById(ID).size());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 <  agg.fieldById(ID).size());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.size() == (unsigned int)agg.fieldById(ID).size());
        }

        if (verbose)
            cout << "\t...with sequences of 'unsigned int'.\n" << endl;
        {
            enum { ID = ID_VAL9 };

            unsigned int data[] = { NLUI_MIN, NLUI_MAX, NLUI_MIN, NLUI_MAX };
            enum { DATA_SIZE = sizeof(data) / sizeof(*data) };

            bcem_Aggregate agg;
            ASSERT(0 == loadArrays(&agg));

            bcem_Aggregate             AGG = agg.clone();
            bsl::vector< bdeut_NullableValue<unsigned int> > VAL(
                                                             data,
                                                             data + DATA_SIZE);
            bsl::vector< bdeut_NullableValue<unsigned int> > val(VAL);

            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.empty());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.clear();
            ASSERT(VAL != val);
            ASSERT(0 == agg.fieldById(ID).size());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 <  agg.fieldById(ID).size());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.size() == (unsigned int)agg.fieldById(ID).size());
        }

        if (verbose)
            cout << "\t...with sequences of 'bsl::string'.\n" << endl;
        {
            enum { ID = ID_VAL10 };

            bcem_Aggregate agg;
            ASSERT(0 == loadArrays(&agg));

            bcem_Aggregate             AGG = agg.clone();
            bsl::vector< bdeut_NullableValue<bsl::string> > VAL;
            VAL.push_back(bsl::string("zero"));
            VAL.push_back(bsl::string("one"));
            VAL.push_back(bsl::string("two"));
            VAL.push_back(bsl::string("three"));
            VAL.push_back(bsl::string("four"));

            bsl::vector< bdeut_NullableValue<bsl::string> > val(VAL);

            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.empty());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.clear();
            ASSERT(VAL != val);
            ASSERT(0 == agg.fieldById(ID).size());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 <  agg.fieldById(ID).size());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.size() == (unsigned int)agg.fieldById(ID).size());
        }

        if (verbose) cout << "\t...with sequences of 'Int64'.\n" << endl;
        {
            enum { ID = ID_VAL11 };

            bcem_Aggregate agg;
            ASSERT(0 == loadArrays(&agg));

            bcem_Aggregate             AGG = agg.clone();
            bsl::vector< bdeut_NullableValue<bsls_PlatformUtil::Int64> > VAL;
            VAL.push_back(NLI64_MIN+1);
            VAL.push_back(NLI64_MAX);
            VAL.push_back(0);
            VAL.push_back(NLI64_MIN+1);
            VAL.push_back(NLI64_MAX);

            bsl::vector< bdeut_NullableValue<bsls_PlatformUtil::Int64> > val(
                                                                         VAL);

            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.empty());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.clear();
            ASSERT(VAL != val);
            ASSERT(0 == agg.fieldById(ID).size());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 <  agg.fieldById(ID).size());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.size() == (unsigned int)agg.fieldById(ID).size());
        }

        if (verbose) cout << "\t...with sequences of 'Uint64'.\n" << endl;
        {
            enum { ID = ID_VAL12 };

            bcem_Aggregate agg;
            ASSERT(0 == loadArrays(&agg));

            bcem_Aggregate             AGG = agg.clone();
            bsl::vector< bdeut_NullableValue<bsls_PlatformUtil::Uint64> > VAL;
            VAL.push_back(NLUI64_MIN);
            VAL.push_back(NLUI64_MAX);
            VAL.push_back(0);
            VAL.push_back(NLUI64_MIN);
            VAL.push_back(NLUI64_MAX);

            bsl::vector< bdeut_NullableValue<bsls_PlatformUtil::Uint64> > val(
                                                                          VAL);

            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.empty());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.clear();
            ASSERT(VAL != val);
            ASSERT(0 == agg.fieldById(ID).size());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 <  agg.fieldById(ID).size());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.size() == (unsigned int)agg.fieldById(ID).size());
        }

        if (verbose)
            cout << "\t...with sequences of 'bdet_DateTz'.\n" << endl;
        {
            enum { ID = ID_VAL13 };

            bdet_DateTz myDateTz(bdet_Date(1980, 4, 4), -5 * 60);

            bcem_Aggregate agg;
            ASSERT(0 == loadArrays(&agg));

            bcem_Aggregate             AGG = agg.clone();
            bsl::vector< bdeut_NullableValue<bdet_DateTz> > VAL(3, myDateTz);
            bsl::vector< bdeut_NullableValue<bdet_DateTz> > val(VAL);

            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.empty());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.clear();
            ASSERT(VAL != val);
            ASSERT(0 == agg.fieldById(ID).size());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 <  agg.fieldById(ID).size());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.size() == (unsigned int)agg.fieldById(ID).size());
        }

        if (verbose)
            cout << "\t...with sequences of 'bdet_DatetimeTz'.\n" << endl;
        {
            enum { ID = ID_VAL14 };

            bdet_DatetimeTz myDatetimeTz(bdet_Datetime(2008, 7, 21, 16, 53),
                                         -5 * 60);

            bcem_Aggregate agg;
            ASSERT(0 == loadArrays(&agg));

            bcem_Aggregate                 AGG = agg.clone();
            bsl::vector< bdeut_NullableValue<bdet_DatetimeTz> >
                                           VAL(3, myDatetimeTz);

            bsl::vector< bdeut_NullableValue<bdet_DatetimeTz> > val(VAL);

            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.empty());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.clear();
            ASSERT(VAL != val);
            ASSERT(0 == agg.fieldById(ID).size());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 <  agg.fieldById(ID).size());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.size() == (unsigned int)agg.fieldById(ID).size());
        }

        if (verbose)
            cout << "\t...with sequences of 'bdet_TimeTz'.\n" << endl;
        {
            enum { ID = ID_VAL15 };

            bdet_TimeTz myTimeTz(bdet_Time(16, 53), -5 * 60);

            bcem_Aggregate agg;
            ASSERT(0 == loadArrays(&agg));

            bcem_Aggregate                 AGG = agg.clone();
            bsl::vector< bdeut_NullableValue<bdet_TimeTz> > VAL(3, myTimeTz);

            bsl::vector< bdeut_NullableValue<bdet_TimeTz> > val(VAL);

            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.empty());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.clear();
            ASSERT(VAL != val);
            ASSERT(0 == agg.fieldById(ID).size());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 <  agg.fieldById(ID).size());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.size() == (unsigned int)agg.fieldById(ID).size());
        }

        if (verbose)
            cout << "\t...with sequences of 'bdet_Date'.\n" << endl;
        {
            enum { ID = ID_VAL16 };

            bdet_Date myDate(1980, 4, 4);

            bcem_Aggregate agg;
            ASSERT(0 == loadArrays(&agg));

            bcem_Aggregate           AGG = agg.clone();
            bsl::vector< bdeut_NullableValue<bdet_Date> > VAL(3, myDate);

            bsl::vector< bdeut_NullableValue<bdet_Date> > val(VAL);

            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.empty());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.clear();
            ASSERT(VAL != val);
            ASSERT(0 == agg.fieldById(ID).size());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 <  agg.fieldById(ID).size());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.size() == (unsigned int)agg.fieldById(ID).size());
        }

        if (verbose)
            cout << "\t...with sequences of 'bdet_Datetime'.\n" << endl;
        {
            enum { ID = ID_VAL17 };

            bdet_Datetime myDatetime(bdet_Date(1980, 4, 4), bdet_Time());

            bcem_Aggregate agg;
            ASSERT(0 == loadArrays(&agg));

            bcem_Aggregate               AGG = agg.clone();
            bsl::vector< bdeut_NullableValue<bdet_Datetime> >
                                         VAL(3, myDatetime);

            bsl::vector< bdeut_NullableValue<bdet_Datetime> > val(VAL);

            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.empty());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.clear();
            ASSERT(VAL != val);
            ASSERT(0 == agg.fieldById(ID).size());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 <  agg.fieldById(ID).size());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.size() == (unsigned int)agg.fieldById(ID).size());
        }

        if (verbose)
            cout << "\t...with sequences of 'bdet_Time'.\n" << endl;
        {
            enum { ID = ID_VAL18 };

            bdet_Time myTime(bdet_Time(1, 2, 3, 4));

            bcem_Aggregate agg;
            ASSERT(0 == loadArrays(&agg));

            bcem_Aggregate           AGG = agg.clone();
            bsl::vector< bdeut_NullableValue<bdet_Time> > VAL(3, myTime);

            bsl::vector< bdeut_NullableValue<bdet_Time> > val(VAL);

            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.empty());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.clear();
            ASSERT(VAL != val);
            ASSERT(0 == agg.fieldById(ID).size());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 <  agg.fieldById(ID).size());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.size() == (unsigned int)agg.fieldById(ID).size());
        }

      }  break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING ARRAYS OF PRIMITIVE TYPES
        //
        // Concerns:
        //   - That 'toAggregate' returns a non-zero value when called with an
        //     invalid field id
        //
        //   - That 'fromAggregate' returns a non-zero value when called with
        //     an invalid field id
        //
        //   - That calling 'toAggregate' followed by 'fromAggregate' on the
        //     same aggregate with the same field id yields the initial value
        //
        // Plan:
        //   Generate a 'bcem_Aggregate' object corresponding to a schema
        //   containing a sequence of arrays of primitive values.  Convert each
        //   field calling the overloaded 'toAggregate' and 'fromAggregate'
        //   methods using an invalid field id, and verify that an error result
        //   is returned.  Convert each field from its associated primitive
        //   type, and back again, and verify that the initial value is
        //   obtained.
        //
        // Testing:
        //   fromAggregate(bsl::vector<bool>)
        //   fromAggregate(bsl::vector<char>)
        //   fromAggregate(bsl::vector<short>)
        //   fromAggregate(bsl::vector<int>)
        //   fromAggregate(bsl::vector<float>)
        //   fromAggregate(bsl::vector<double>)
        //   fromAggregate(bsl::vector<unsigned char>)
        //   fromAggregate(bsl::vector<unsigned short>)
        //   fromAggregate(bsl::vector<unsigned int>)
        //   fromAggregate(bsl::vector<bsl::string>)
        //   fromAggregate(bsl::vector<bsls_PlatformUtil::Int64>)
        //   fromAggregate(bsl::vector<bsls_PlatformUtil::Uint64>)
        //   fromAggregate(bsl::vector<bdet_DateTz>)
        //   fromAggregate(bsl::vector<bdet_DatetimeTz>)
        //   fromAggregate(bsl::vector<bdet_TimeTz>)
        //   fromAggregate(bsl::vector<bdet_Date>)
        //   fromAggregate(bsl::vector<bdet_Datetime>)
        //   fromAggregate(bsl::vector<bdet_Time>)
        //   toAggregate(bsl::vector<bool>)
        //   toAggregate(bsl::vector<char>)
        //   toAggregate(bsl::vector<short>)
        //   toAggregate(bsl::vector<int>)
        //   toAggregate(bsl::vector<float>)
        //   toAggregate(bsl::vector<double>)
        //   toAggregate(bsl::vector<unsigned char>)
        //   toAggregate(bsl::vector<unsigned short>)
        //   toAggregate(bsl::vector<unsigned int>)
        //   toAggregate(bsl::vector<bsl::string>)
        //   toAggregate(bsl::vector<bsls_PlatformUtil::Int64>)
        //   toAggregate(bsl::vector<bsls_PlatformUtil::Uint64>)
        //   toAggregate(bsl::vector<bdet_DateTz>)
        //   toAggregate(bsl::vector<bdet_DatetimeTz>)
        //   toAggregate(bsl::vector<bdet_TimeTz>)
        //   toAggregate(bsl::vector<bdet_Date>)
        //   toAggregate(bsl::vector<bdet_Datetime>)
        //   toAggregate(bsl::vector<bdet_Time>)
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing Arrays of Primitive Types" << endl
                 << "=================================" << endl;
        }

        if (verbose) cout << "\tPrinting test aggregate.\n" << endl;
        {
            bcem_Aggregate agg;
            ASSERT(0 == loadArrays(&agg));

            bcem_Aggregate       AGG = agg.clone();
            if (verbose) { P(AGG); }
        }

        if (verbose) cout << "\n\t...with sequences of 'char'.\n" << endl;
        {
            enum { ID = ID_VAL2 };

            char   data[] = { NLC_MIN, NLC_MAX, NLC_MIN, NLC_MAX };
            enum { DATA_SIZE = sizeof(data) / sizeof(*data) };

            bcem_Aggregate agg;
            ASSERT(0 == loadArrays(&agg));

            bcem_Aggregate             AGG = agg.clone();
            bsl::vector<char>          VAL(data, data + DATA_SIZE);
            bsl::vector<char>          val(VAL);

            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.empty());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.clear();
            ASSERT(VAL != val);
            ASSERT(0 == agg.fieldById(ID).size());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 <  agg.fieldById(ID).size());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.size() == (unsigned int)agg.fieldById(ID).size());
        }

        if (verbose) cout << "\t...with sequences of 'short'.\n" << endl;
        {
            enum { ID = ID_VAL3 };

            short  data[] = { NLS_MIN, NLS_MAX, NLS_MIN, NLS_MAX };
            enum { DATA_SIZE = sizeof(data) / sizeof(*data) };

            bcem_Aggregate agg;
            ASSERT(0 == loadArrays(&agg));

            bcem_Aggregate             AGG = agg.clone();
            bsl::vector<short>         VAL(data, data + DATA_SIZE);
            bsl::vector<short>         val(VAL);

            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.empty());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.clear();
            ASSERT(VAL != val);
            ASSERT(0 == agg.fieldById(ID).size());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 <  agg.fieldById(ID).size());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.size() == (unsigned int)agg.fieldById(ID).size());
        }

        if (verbose) cout << "\t...with sequences of 'int'.\n" << endl;
        {
            enum { ID = ID_VAL4 };

            int    data[] = { NLI_MIN, NLI_MAX, NLI_MIN, NLI_MAX };
            enum { DATA_SIZE = sizeof(data) / sizeof(*data) };

            bcem_Aggregate agg;
            ASSERT(0 == loadArrays(&agg));

            bcem_Aggregate             AGG = agg.clone();
            bsl::vector<int>           VAL(data, data + DATA_SIZE);
            bsl::vector<int>           val(VAL);

            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.empty());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.clear();
            ASSERT(VAL != val);
            ASSERT(0 == agg.fieldById(ID).size());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 <  agg.fieldById(ID).size());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.size() == (unsigned int)agg.fieldById(ID).size());
        }

        if (verbose) cout << "\t...with sequences of 'float'.\n" << endl;
        {
            enum { ID = ID_VAL5 };

            float  data[] = { NLF_MIN, NLF_MAX, 0.0f, NLF_MIN, NLF_MAX };
            enum { DATA_SIZE = sizeof(data) / sizeof(*data) };

            bcem_Aggregate agg;
            ASSERT(0 == loadArrays(&agg));

            bcem_Aggregate             AGG = agg.clone();
            bsl::vector<float>         VAL(data, data + DATA_SIZE);
            bsl::vector<float>         val(VAL);

            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.empty());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.clear();
            ASSERT(VAL != val);
            ASSERT(0 == agg.fieldById(ID).size());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 <  agg.fieldById(ID).size());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.size() == (unsigned int)agg.fieldById(ID).size());
        }

        if (verbose) cout << "\t...with sequences of 'double'.\n" << endl;
        {
            enum { ID = ID_VAL6 };

            double   data[] = { NLD_MIN, NLD_MAX, 0.0, NLD_MIN, NLD_MAX };
            enum   { DATA_SIZE = sizeof(data) / sizeof(*data) };

            bcem_Aggregate agg;
            ASSERT(0 == loadArrays(&agg));

            bcem_Aggregate             AGG = agg.clone();
            bsl::vector<double>        VAL(data, data + DATA_SIZE);
            bsl::vector<double>        val(VAL);

            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.empty());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.clear();
            ASSERT(VAL != val);
            ASSERT(0 == agg.fieldById(ID).size());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 <  agg.fieldById(ID).size());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.size() == (unsigned int)agg.fieldById(ID).size());
        }

        if (verbose)
            cout << "\t...with sequences of 'unsigned char'.\n" << endl;
        {
            enum { ID = ID_VAL7 };

            unsigned char data[] = { NLUC_MIN, NLUC_MAX, NLUC_MIN, NLUC_MAX };
            enum        { DATA_SIZE = sizeof(data) / sizeof(*data) };

            bcem_Aggregate agg;
            ASSERT(0 == loadArrays(&agg));

            bcem_Aggregate             AGG = agg.clone();
            bsl::vector<unsigned char> VAL(data, data + DATA_SIZE);
            bsl::vector<unsigned char> val(VAL);

            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.empty());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.clear();
            ASSERT(VAL != val);
            ASSERT(0 == agg.fieldById(ID).size());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 <  agg.fieldById(ID).size());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.size() == (unsigned int)agg.fieldById(ID).size());
        }

        if (verbose)
            cout << "\t...with sequences of 'unsigned short'.\n" << endl;
        {
            enum { ID = ID_VAL8 };

            unsigned short data[] = { NLUS_MIN, NLUS_MAX, NLUS_MIN, NLUS_MAX };
            enum { DATA_SIZE = sizeof(data) / sizeof(*data) };

            bcem_Aggregate agg;
            ASSERT(0 == loadArrays(&agg));

            bcem_Aggregate              AGG = agg.clone();
            bsl::vector<unsigned short> VAL(data, data + DATA_SIZE);
            bsl::vector<unsigned short> val(VAL);

            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.empty());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.clear();
            ASSERT(VAL != val);
            ASSERT(0 == agg.fieldById(ID).size());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 <  agg.fieldById(ID).size());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.size() == (unsigned int)agg.fieldById(ID).size());
        }

        if (verbose)
            cout << "\t...with sequences of 'unsigned int'.\n" << endl;
        {
            enum { ID = ID_VAL9 };

            unsigned int data[] = { NLUI_MIN, NLUI_MAX, NLUI_MIN, NLUI_MAX };
            enum { DATA_SIZE = sizeof(data) / sizeof(*data) };

            bcem_Aggregate agg;
            ASSERT(0 == loadArrays(&agg));

            bcem_Aggregate             AGG = agg.clone();
            bsl::vector<unsigned int>  VAL(data, data + DATA_SIZE);
            bsl::vector<unsigned int>  val(VAL);

            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.empty());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.clear();
            ASSERT(VAL != val);
            ASSERT(0 == agg.fieldById(ID).size());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 <  agg.fieldById(ID).size());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.size() == (unsigned int)agg.fieldById(ID).size());
        }

        if (verbose)
            cout << "\t...with sequences of 'bsl::string'.\n" << endl;
        {
            enum { ID = ID_VAL10 };

            bcem_Aggregate agg;
            ASSERT(0 == loadArrays(&agg));

            bcem_Aggregate             AGG = agg.clone();
            bsl::vector<bsl::string>   VAL;
            VAL.push_back("zero");
            VAL.push_back("one");
            VAL.push_back("two");
            VAL.push_back("three");
            VAL.push_back("four");

            bsl::vector<bsl::string>   val(VAL);

            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.empty());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.clear();
            ASSERT(VAL != val);
            ASSERT(0 == agg.fieldById(ID).size());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 <  agg.fieldById(ID).size());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.size() == (unsigned int)agg.fieldById(ID).size());
        }

        if (verbose) cout << "\t...with sequences of 'Int64'.\n" << endl;
        {
            enum { ID = ID_VAL11 };

            bcem_Aggregate agg;
            ASSERT(0 == loadArrays(&agg));

            bcem_Aggregate             AGG = agg.clone();
            bsl::vector<bsls_PlatformUtil::Int64>   VAL;
            VAL.push_back(NLI64_MIN);
            VAL.push_back(NLI64_MAX);
            VAL.push_back(0);
            VAL.push_back(NLI64_MIN);
            VAL.push_back(NLI64_MAX);

            bsl::vector<bsls_PlatformUtil::Int64>   val(VAL);

            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.empty());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.clear();
            ASSERT(VAL != val);
            ASSERT(0 == agg.fieldById(ID).size());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 <  agg.fieldById(ID).size());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.size() == (unsigned int)agg.fieldById(ID).size());
        }

        if (verbose) cout << "\t...with sequences of 'Uint64'.\n" << endl;
        {
            enum { ID = ID_VAL12 };

            bcem_Aggregate agg;
            ASSERT(0 == loadArrays(&agg));

            bcem_Aggregate             AGG = agg.clone();
            bsl::vector<bsls_PlatformUtil::Uint64>   VAL;
            VAL.push_back(NLUI64_MIN);
            VAL.push_back(NLUI64_MAX);
            VAL.push_back(0);
            VAL.push_back(NLUI64_MIN);
            VAL.push_back(NLUI64_MAX);

            bsl::vector<bsls_PlatformUtil::Uint64>   val(VAL);

            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.empty());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.clear();
            ASSERT(VAL != val);
            ASSERT(0 == agg.fieldById(ID).size());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 <  agg.fieldById(ID).size());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.size() == (unsigned int)agg.fieldById(ID).size());
        }

        if (verbose)
            cout << "\t...with sequences of 'bdet_DateTz'.\n" << endl;
        {
            enum { ID = ID_VAL13 };

            bdet_DateTz myDateTz(bdet_Date(1980, 4, 4), -5 * 60);

            bcem_Aggregate agg;
            ASSERT(0 == loadArrays(&agg));

            bcem_Aggregate             AGG = agg.clone();
            bsl::vector<bdet_DateTz>   VAL(3, myDateTz);
            bsl::vector<bdet_DateTz>   val(VAL);

            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.empty());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.clear();
            ASSERT(VAL != val);
            ASSERT(0 == agg.fieldById(ID).size());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 <  agg.fieldById(ID).size());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.size() == (unsigned int)agg.fieldById(ID).size());
        }

        if (verbose)
            cout << "\t...with sequences of 'bdet_DatetimeTz'.\n" << endl;
        {
            enum { ID = ID_VAL14 };

            bdet_DatetimeTz myDatetimeTz(bdet_Datetime(2008, 7, 21, 16, 53),
                                         -5 * 60);

            bcem_Aggregate agg;
            ASSERT(0 == loadArrays(&agg));

            bcem_Aggregate                 AGG = agg.clone();
            bsl::vector<bdet_DatetimeTz>   VAL(3, myDatetimeTz);

            bsl::vector<bdet_DatetimeTz>   val(VAL);

            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.empty());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.clear();
            ASSERT(VAL != val);
            ASSERT(0 == agg.fieldById(ID).size());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 <  agg.fieldById(ID).size());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.size() == (unsigned int)agg.fieldById(ID).size());
        }

        if (verbose)
            cout << "\t...with sequences of 'bdet_TimeTz'.\n" << endl;
        {
            enum { ID = ID_VAL15 };

            bdet_TimeTz myTimeTz(bdet_Time(16, 53), -5 * 60);

            bcem_Aggregate agg;
            ASSERT(0 == loadArrays(&agg));

            bcem_Aggregate                 AGG = agg.clone();
            bsl::vector<bdet_TimeTz>   VAL(3, myTimeTz);

            bsl::vector<bdet_TimeTz>   val(VAL);

            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.empty());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.clear();
            ASSERT(VAL != val);
            ASSERT(0 == agg.fieldById(ID).size());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 <  agg.fieldById(ID).size());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.size() == (unsigned int)agg.fieldById(ID).size());
        }

        if (verbose)
            cout << "\t...with sequences of 'bdet_Date'.\n" << endl;
        {
            enum { ID = ID_VAL16 };

            bdet_Date myDate(1980, 4, 4);

            bcem_Aggregate agg;
            ASSERT(0 == loadArrays(&agg));

            bcem_Aggregate           AGG = agg.clone();
            bsl::vector<bdet_Date>   VAL(3, myDate);

            bsl::vector<bdet_Date>   val(VAL);

            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.empty());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.clear();
            ASSERT(VAL != val);
            ASSERT(0 == agg.fieldById(ID).size());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 <  agg.fieldById(ID).size());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.size() == (unsigned int)agg.fieldById(ID).size());
        }

        if (verbose)
            cout << "\t...with sequences of 'bdet_Datetime'.\n" << endl;
        {
            enum { ID = ID_VAL17 };

            bdet_Datetime myDatetime(bdet_Date(1980, 4, 4), bdet_Time());

            bcem_Aggregate agg;
            ASSERT(0 == loadArrays(&agg));

            bcem_Aggregate               AGG = agg.clone();
            bsl::vector<bdet_Datetime>   VAL(3, myDatetime);

            bsl::vector<bdet_Datetime>   val(VAL);

            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.empty());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.clear();
            ASSERT(VAL != val);
            ASSERT(0 == agg.fieldById(ID).size());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 <  agg.fieldById(ID).size());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.size() == (unsigned int)agg.fieldById(ID).size());
        }

        if (verbose)
            cout << "\t...with sequences of 'bdet_Time'.\n" << endl;
        {
            enum { ID = ID_VAL18 };

            bdet_Time myTime(bdet_Time(1, 2, 3, 4));

            bcem_Aggregate agg;
            ASSERT(0 == loadArrays(&agg));

            bcem_Aggregate           AGG = agg.clone();
            bsl::vector<bdet_Time>   VAL(3, myTime);

            bsl::vector<bdet_Time>   val(VAL);

            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.empty());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.clear();
            ASSERT(VAL != val);
            ASSERT(0 == agg.fieldById(ID).size());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 <  agg.fieldById(ID).size());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.size() == (unsigned int)agg.fieldById(ID).size());
        }

      }  break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING NULLABLE PRIMITIVE TYPES
        //
        // Concerns:
        //   - That 'toAggregate' returns a non-zero value when called with an
        //     invalid field id
        //
        //   - That 'fromAggregate' returns a non-zero value when called with
        //     an invalid field id
        //
        //   - That calling 'toAggregate' followed by 'fromAggregate' on the
        //     same aggregate with the same field id yields the initial value
        //
        // Plan:
        //   Generate a 'bcem_Aggregate' object corresponding to a schema
        //   containing a sequence of nullable primitive values.  Convert each
        //   field calling the overloaded 'toAggregate' and 'fromAggregate'
        //   methods using an invalid field id, and verify that an error result
        //   is returned.  Convert each field from its associated primitive
        //   type, and back again, and verify that the initial value is
        //   obtained.
        //
        // Testing:
        //   fromAggregate(bdeut_NullableValue<bool>)
        //   fromAggregate(bdeut_NullableValue<char>)
        //   fromAggregate(bdeut_NullableValue<short>)
        //   fromAggregate(bdeut_NullableValue<int>)
        //   fromAggregate(bdeut_NullableValue<float>)
        //   fromAggregate(bdeut_NullableValue<double>)
        //   fromAggregate(bdeut_NullableValue<unsigned char>)
        //   fromAggregate(bdeut_NullableValue<unsigned short>)
        //   fromAggregate(bdeut_NullableValue<unsigned int>)
        //   fromAggregate(bdeut_NullableValue<bsl::string>)
        //   fromAggregate(bdeut_NullableValue<bsls_PlatformUtil::Int64>)
        //   fromAggregate(bdeut_NullableValue<bsls_PlatformUtil::Uint64>)
        //   fromAggregate(bdeut_NullableValue<bdet_DateTz>)
        //   fromAggregate(bdeut_NullableValue<bdet_DatetimeTz>)
        //   fromAggregate(bdeut_NullableValue<bdet_TimeTz>)
        //   fromAggregate(bdeut_NullableValue<bdet_Date>)
        //   fromAggregate(bdeut_NullableValue<bdet_Datetime>)
        //   fromAggregate(bdeut_NullableValue<bdet_Time>)
        //   toAggregate(bdeut_NullableValue<bool>)
        //   toAggregate(bdeut_NullableValue<char>)
        //   toAggregate(bdeut_NullableValue<short>)
        //   toAggregate(bdeut_NullableValue<int>)
        //   toAggregate(bdeut_NullableValue<float>)
        //   toAggregate(bdeut_NullableValue<double>)
        //   toAggregate(bdeut_NullableValue<unsigned char>)
        //   toAggregate(bdeut_NullableValue<unsigned short>)
        //   toAggregate(bdeut_NullableValue<unsigned int>)
        //   toAggregate(bdeut_NullableValue<bsl::string>)
        //   toAggregate(bdeut_NullableValue<bsls_PlatformUtil::Int64>)
        //   toAggregate(bdeut_NullableValue<bsls_PlatformUtil::Uint64>)
        //   toAggregate(bdeut_NullableValue<bdet_DateTz>)
        //   toAggregate(bdeut_NullableValue<bdet_DatetimeTz>)
        //   toAggregate(bdeut_NullableValue<bdet_TimeTz>)
        //   toAggregate(bdeut_NullableValue<bdet_Date>)
        //   toAggregate(bdeut_NullableValue<bdet_Datetime>)
        //   toAggregate(bdeut_NullableValue<bdet_Time>)
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing Nullable Primitives" << endl
                 << "===========================" << endl;
        }

        if (verbose) cout << "\tPrinting test aggregate.\n" << endl;
        {
            bcem_Aggregate agg;
            ASSERT(0 == loadNullables(&agg));

            bcem_Aggregate       AGG = agg.clone();
            if (verbose) { P(AGG); }

            ASSERT(AGG.fieldById(ID_VAL1).fieldDef()->isNullable());
            ASSERT(AGG.fieldById(ID_VAL2).fieldDef()->isNullable());
            ASSERT(AGG.fieldById(ID_VAL3).fieldDef()->isNullable());
            ASSERT(AGG.fieldById(ID_VAL4).fieldDef()->isNullable());
            ASSERT(AGG.fieldById(ID_VAL5).fieldDef()->isNullable());
            ASSERT(AGG.fieldById(ID_VAL6).fieldDef()->isNullable());
            ASSERT(AGG.fieldById(ID_VAL7).fieldDef()->isNullable());
            ASSERT(AGG.fieldById(ID_VAL8).fieldDef()->isNullable());
            ASSERT(AGG.fieldById(ID_VAL9).fieldDef()->isNullable());
            ASSERT(AGG.fieldById(ID_VAL10).fieldDef()->isNullable());
            ASSERT(AGG.fieldById(ID_VAL11).fieldDef()->isNullable());
            ASSERT(AGG.fieldById(ID_VAL12).fieldDef()->isNullable());
            ASSERT(AGG.fieldById(ID_VAL13).fieldDef()->isNullable());
            ASSERT(AGG.fieldById(ID_VAL14).fieldDef()->isNullable());
            ASSERT(AGG.fieldById(ID_VAL15).fieldDef()->isNullable());
            ASSERT(AGG.fieldById(ID_VAL16).fieldDef()->isNullable());
            ASSERT(AGG.fieldById(ID_VAL17).fieldDef()->isNullable());
            ASSERT(AGG.fieldById(ID_VAL18).fieldDef()->isNullable());
        }

        if (verbose) cout << "\n\t...with 'bool'.\n" << endl;
        {
            enum { ID = ID_VAL1 };

            bcem_Aggregate agg;
            ASSERT(0 == loadNullables(&agg));

            bcem_Aggregate                      AGG = agg.clone();
            bdeut_NullableValue<bool>           VAL(true);
            bdeut_NullableValue<bool>           val(VAL);
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.isNull());
            ASSERT(VAL.value() != bdetu_Unset<bool>::unsetValue());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.reset();
            ASSERT(VAL != val);
            ASSERT(true == agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(true != agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.value() == agg.fieldById(ID).asBool());
        }

        if (verbose) cout << "\t...with 'char'.\n" << endl;
        {
            // NLC::min() cannot be used to specify a value guaranteed to be
            // not null because numeric_limits<char>::min() yields different
            // values using different compilers.

            enum { ID = ID_VAL2 };

            bcem_Aggregate agg;
            ASSERT(0 == loadNullables(&agg));

            bcem_Aggregate              AGG = agg.clone();
            bdeut_NullableValue<char>   VAL(NullTraits<char>::notUnsetValue());
            bdeut_NullableValue<char>   val(VAL);
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.isNull());

            ASSERT(VAL.value() != bdetu_Unset<char>::unsetValue());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.reset();
            ASSERT(VAL != val);
            ASSERT(true == agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(true != agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.value() == agg.fieldById(ID).asChar());
        }

        if (verbose) cout << "\t...with 'short'.\n" << endl;
        {
            enum { ID = ID_VAL3 };

            bcem_Aggregate agg;
            ASSERT(0 == loadNullables(&agg));

            bcem_Aggregate                      AGG = agg.clone();
            bdeut_NullableValue<short>          VAL(NLS::min() + 1);
            bdeut_NullableValue<short>          val(VAL);
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.isNull());
            ASSERT(VAL.value() != bdetu_Unset<short>::unsetValue());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.reset();
            ASSERT(VAL != val);
            ASSERT(true == agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(true != agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.value() == agg.fieldById(ID).asShort());
        }

        if (verbose) cout << "\t...with 'int'.\n" << endl;
        {
            enum { ID = ID_VAL4 };

            bcem_Aggregate agg;
            ASSERT(0 == loadNullables(&agg));

            bcem_Aggregate                      AGG = agg.clone();
            bdeut_NullableValue<int>            VAL(NLI::min() + 1);
            bdeut_NullableValue<int>            val(VAL);
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.isNull());
            ASSERT(VAL.value() != bdetu_Unset<int>::unsetValue());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.reset();
            ASSERT(VAL != val);
            ASSERT(true == agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(true != agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.value() == agg.fieldById(ID).asInt());
        }

        if (verbose) cout << "\t...with 'float'.\n" << endl;
        {
            enum { ID = ID_VAL5 };

            bcem_Aggregate agg;
            ASSERT(0 == loadNullables(&agg));

            bcem_Aggregate                      AGG = agg.clone();
            bdeut_NullableValue<float>          VAL(NLF::min());
            bdeut_NullableValue<float>          val(VAL);
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.isNull());
            ASSERT(VAL.value() != bdetu_Unset<float>::unsetValue());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.reset();
            ASSERT(VAL != val);
            ASSERT(true == agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(true != agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.value() == agg.fieldById(ID).asFloat());
        }

        if (verbose) cout << "\t...with 'double'.\n" << endl;
        {
            enum { ID = ID_VAL6 };

            bcem_Aggregate agg;
            ASSERT(0 == loadNullables(&agg));

            bcem_Aggregate                      AGG = agg.clone();
            bdeut_NullableValue<double>         VAL(NLD::min());
            bdeut_NullableValue<double>         val(VAL);
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.isNull());
            ASSERT(VAL.value() != bdetu_Unset<double>::unsetValue());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.reset();
            ASSERT(VAL != val);
            ASSERT(true == agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(true != agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.value() == agg.fieldById(ID).asDouble());
        }

        if (verbose) cout << "\t...with 'unsigned char'.\n" << endl;
        {
            enum { ID = ID_VAL7 };

            bcem_Aggregate agg;
            ASSERT(0 == loadNullables(&agg));

            bcem_Aggregate                      AGG = agg.clone();
            bdeut_NullableValue<unsigned char>  VAL(NLUC::min() + 1);
            bdeut_NullableValue<unsigned char>  val(VAL);
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.isNull());
            ASSERT(VAL.value() != bdetu_Unset<char>::unsetValue());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.reset();
            ASSERT(VAL != val);
            ASSERT(true == agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(true != agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.value() == (unsigned char)agg.fieldById(ID)
                                                    .asChar());
        }

        if (verbose) cout << "\t...with 'unsigned short'.\n" << endl;
        {
            enum { ID = ID_VAL8 };

            bcem_Aggregate agg;
            ASSERT(0 == loadNullables(&agg));

            bcem_Aggregate                      AGG = agg.clone();
            bdeut_NullableValue<unsigned short> VAL(NLUC::min());
            bdeut_NullableValue<unsigned short> val(VAL);
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.isNull());
            ASSERT(VAL.value() != bdetu_Unset<short>::unsetValue());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.reset();
            ASSERT(VAL != val);
            ASSERT(true == agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(true != agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.value() == (unsigned short)agg.fieldById(ID)
                                                     .asShort());
        }

        if (verbose) cout << "\t...with 'unsigned int'.\n" << endl;
        {
            enum { ID = ID_VAL9 };

            bcem_Aggregate agg;
            ASSERT(0 == loadNullables(&agg));

            bcem_Aggregate                      AGG = agg.clone();
            bdeut_NullableValue<unsigned int>   VAL(NLUC::min());
            bdeut_NullableValue<unsigned int>   val(VAL);
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.isNull());
            ASSERT(VAL.value()
                   != (unsigned int)bdetu_Unset<int>::unsetValue());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.reset();
            ASSERT(VAL != val);
            ASSERT(true == agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(true != agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.value() == (unsigned int)agg.fieldById(ID)
                                                   .asInt());
        }

        if (verbose) cout << "\t...with 'bsl::string'.\n" << endl;
        {
            enum { ID = ID_VAL10 };

            bcem_Aggregate agg;
            ASSERT(0 == loadNullables(&agg));

            bcem_Aggregate                      AGG = agg.clone();
            bdeut_NullableValue<bsl::string>    VAL("not empty");
            bdeut_NullableValue<bsl::string>    val(VAL);
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.isNull());
            ASSERT(VAL.value() != bdetu_Unset<bsl::string>::unsetValue());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.reset();
            ASSERT(VAL != val);
            ASSERT(true == agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(true != agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.value() == agg.fieldById(ID).asString());
        }

        if (verbose) cout << "\t...with 'Int64'.\n" << endl;
        {
            enum { ID = ID_VAL11 };

            bcem_Aggregate agg;
            ASSERT(0 == loadNullables(&agg));

            typedef bsls_PlatformUtil::Int64 Int64;

            bcem_Aggregate                       AGG = agg.clone();
            bdeut_NullableValue<Int64>           VAL(NLI64::min() + 1);
            bdeut_NullableValue<Int64>           val(VAL);
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.isNull());
            ASSERT(VAL.value() != bdetu_Unset<Int64>::unsetValue());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.reset();
            ASSERT(VAL != val);
            ASSERT(true == agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(true != agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.value() == agg.fieldById(ID).asInt64());
        }

        if (verbose) cout << "\t...with 'Uint64'.\n" << endl;
        {
            enum { ID = ID_VAL12 };

            bcem_Aggregate agg;
            ASSERT(0 == loadNullables(&agg));

            typedef bsls_PlatformUtil::Int64  Int64;
            typedef bsls_PlatformUtil::Uint64 Uint64;

            bcem_Aggregate                       AGG = agg.clone();
            bdeut_NullableValue<Uint64>          VAL(NLUI64::min());
            bdeut_NullableValue<Uint64>          val(VAL);
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.isNull());
            ASSERT(VAL.value() != (Uint64)bdetu_Unset<Int64>::unsetValue());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.reset();
            ASSERT(VAL != val);
            ASSERT(true == agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(true != agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.value() == (Uint64)agg.fieldById(ID).asInt64());
        }

        if (verbose) cout << "\t...with 'bdet_DateTz'.\n" << endl;
        {
            enum { ID = ID_VAL13 };

            bcem_Aggregate agg;
            ASSERT(0 == loadNullables(&agg));

            bdet_DateTz myDateTz(bdet_Date(2008, 7, 21), -5 * 60);

            bcem_Aggregate                       AGG = agg.clone();
            bdeut_NullableValue<bdet_DateTz>     VAL(myDateTz);
            bdeut_NullableValue<bdet_DateTz>     val(VAL);
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.isNull());
            ASSERT(VAL.value() != bdetu_Unset<bdet_DateTz>::unsetValue());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.reset();
            ASSERT(VAL != val);
            ASSERT(true == agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(true != agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.value() == agg.fieldById(ID).asDateTz());
        }

        if (verbose) cout << "\t...with 'bdet_DatetimeTz'.\n" << endl;
        {
            enum { ID = ID_VAL14 };

            bcem_Aggregate agg;
            ASSERT(0 == loadNullables(&agg));

            bdet_DatetimeTz myDatetimeTz(bdet_Datetime(2008, 7, 21, 16, 53),
                                         -5 * 60);

            bcem_Aggregate                       AGG = agg.clone();
            bdeut_NullableValue<bdet_DatetimeTz> VAL(myDatetimeTz);
            bdeut_NullableValue<bdet_DatetimeTz> val(VAL);
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.isNull());
            ASSERT(VAL.value() != bdetu_Unset<bdet_DatetimeTz>::unsetValue());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.reset();
            ASSERT(VAL != val);
            ASSERT(true == agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(true != agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.value() == agg.fieldById(ID).asDatetimeTz());
        }

        if (verbose) cout << "\t...with 'bdet_TimeTz'.\n" << endl;
        {
            enum { ID = ID_VAL15 };

            bcem_Aggregate agg;
            ASSERT(0 == loadNullables(&agg));

            bdet_TimeTz myTimeTz(bdet_Time(16, 53), -5 * 60);

            bcem_Aggregate                       AGG = agg.clone();
            bdeut_NullableValue<bdet_TimeTz>     VAL(myTimeTz);
            bdeut_NullableValue<bdet_TimeTz>     val(VAL);
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.isNull());
            ASSERT(VAL.value() != bdetu_Unset<bdet_TimeTz>::unsetValue());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.reset();
            ASSERT(VAL != val);
            ASSERT(true == agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(true != agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.value() == agg.fieldById(ID).asTimeTz());
        }

        if (verbose) cout << "\t...with 'bdet_Date'.\n" << endl;
        {
            enum { ID = ID_VAL16 };

            bcem_Aggregate agg;
            ASSERT(0 == loadNullables(&agg));

            bdet_Date myDate(2008, 7, 21);

            bcem_Aggregate                       AGG = agg.clone();
            bdeut_NullableValue<bdet_Date>       VAL(myDate);
            bdeut_NullableValue<bdet_Date>       val(VAL);
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.isNull());
            ASSERT(VAL.value() != bdetu_Unset<bdet_Date>::unsetValue());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.reset();
            ASSERT(VAL != val);
            ASSERT(true == agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(true != agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.value() == agg.fieldById(ID).asDate());
        }

        if (verbose) cout << "\t...with 'bdet_Datetime'.\n" << endl;
        {
            enum { ID = ID_VAL17 };

            bcem_Aggregate agg;
            ASSERT(0 == loadNullables(&agg));

            bdet_Datetime myDatetime(2008, 7, 21, 16, 53);

            bcem_Aggregate                       AGG = agg.clone();
            bdeut_NullableValue<bdet_Datetime>   VAL(myDatetime);
            bdeut_NullableValue<bdet_Datetime>   val(VAL);
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.isNull());
            ASSERT(VAL.value() != bdetu_Unset<bdet_Datetime>::unsetValue());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.reset();
            ASSERT(VAL != val);
            ASSERT(true == agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(true != agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.value() == agg.fieldById(ID).asDatetime());
        }

        if (verbose) cout << "\t...with 'bdet_Time'.\n" << endl;
        {
            enum { ID = ID_VAL18 };

            bcem_Aggregate agg;
            ASSERT(0 == loadNullables(&agg));

            bdet_Time myTime(16, 53);

            bcem_Aggregate                       AGG = agg.clone();
            bdeut_NullableValue<bdet_Time>       VAL(myTime);
            bdeut_NullableValue<bdet_Time>       val(VAL);
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(true != VAL.isNull());
            ASSERT(VAL.value() != bdetu_Unset<bdet_Time>::unsetValue());
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val.reset();
            ASSERT(VAL != val);
            ASSERT(true == agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(true != agg.fieldById(ID).isNul2());
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val.value() == agg.fieldById(ID).asTime());
        }

      }  break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMITIVE TYPES
        //
        // Concerns:
        //   - That 'toAggregate' returns a non-zero value when called with an
        //     invalid field id
        //
        //   - That 'fromAggregate' returns a non-zero value when called with
        //     an invalid field id
        //
        //   - That calling 'toAggregate' followed by 'fromAggregate' on the
        //     same aggregate with the same field id yields the initial value
        //
        // Plan:
        //   Generate a 'bcem_Aggregate' object corresponding to a schema
        //   containing a sequence of primitive values.  Convert each field
        //   calling the overloaded 'toAggregate' and 'fromAggregate' methods
        //   using an invalid field id, and verify that an error result is
        //   returned.  Convert each field from its associated primitive type,
        //   and back again, and verify that the initial value is obtained.
        //
        // Testing:
        //   fromAggregate(bool)
        //   fromAggregate(char)
        //   fromAggregate(short)
        //   fromAggregate(int)
        //   fromAggregate(float)
        //   fromAggregate(double)
        //   fromAggregate(unsigned char)
        //   fromAggregate(unsigned short)
        //   fromAggregate(unsigned int)
        //   fromAggregate(bsl::string)
        //   fromAggregate(bsls_PlatformUtil::Int64)
        //   fromAggregate(bsls_PlatformUtil::Uint64)
        //   fromAggregate(bdet_DateTz)
        //   fromAggregate(bdet_DatetimeTz)
        //   fromAggregate(bdet_TimeTz)
        //   fromAggregate(bdet_Date)
        //   fromAggregate(bdet_Datetime)
        //   fromAggregate(bdet_Time)
        //   toAggregate(bool)
        //   toAggregate(char)
        //   toAggregate(short)
        //   toAggregate(int)
        //   toAggregate(float)
        //   toAggregate(double)
        //   toAggregate(unsigned char)
        //   toAggregate(unsigned short)
        //   toAggregate(unsigned int)
        //   toAggregate(bsl::string)
        //   toAggregate(bsls_PlatformUtil::Int64)
        //   toAggregate(bsls_PlatformUtil::Uint64)
        //   toAggregate(bdet_DateTz)
        //   toAggregate(bdet_DatetimeTz)
        //   toAggregate(bdet_TimeTz)
        //   toAggregate(bdet_Date)
        //   toAggregate(bdet_Datetime)
        //   toAggregate(bdet_Time)
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing Primitive Types" << endl
                          << "=======================" << endl;

        if (verbose) cout << "\tPrinting test aggregate.\n" << endl;
        {
            bcem_Aggregate agg;
            ASSERT(0 == loadPrimitives(&agg));

            bcem_Aggregate       AGG = agg.clone();
            if (verbose) { P(AGG); }

            ASSERT(!AGG.fieldById(ID_VAL1).fieldDef()->isNullable());
            ASSERT(!AGG.fieldById(ID_VAL2).fieldDef()->isNullable());
            ASSERT(!AGG.fieldById(ID_VAL3).fieldDef()->isNullable());
            ASSERT(!AGG.fieldById(ID_VAL4).fieldDef()->isNullable());
            ASSERT(!AGG.fieldById(ID_VAL5).fieldDef()->isNullable());
            ASSERT(!AGG.fieldById(ID_VAL6).fieldDef()->isNullable());
            ASSERT(!AGG.fieldById(ID_VAL7).fieldDef()->isNullable());
            ASSERT(!AGG.fieldById(ID_VAL8).fieldDef()->isNullable());
            ASSERT(!AGG.fieldById(ID_VAL9).fieldDef()->isNullable());
            ASSERT(!AGG.fieldById(ID_VAL10).fieldDef()->isNullable());
            ASSERT(!AGG.fieldById(ID_VAL11).fieldDef()->isNullable());
            ASSERT(!AGG.fieldById(ID_VAL12).fieldDef()->isNullable());
            ASSERT(!AGG.fieldById(ID_VAL13).fieldDef()->isNullable());
            ASSERT(!AGG.fieldById(ID_VAL14).fieldDef()->isNullable());
            ASSERT(!AGG.fieldById(ID_VAL15).fieldDef()->isNullable());
            ASSERT(!AGG.fieldById(ID_VAL16).fieldDef()->isNullable());
            ASSERT(!AGG.fieldById(ID_VAL17).fieldDef()->isNullable());
            ASSERT(!AGG.fieldById(ID_VAL18).fieldDef()->isNullable());
        }

        if (verbose) cout << "\n\t...with 'bool'.\n" << endl;
        {
            enum { ID = ID_VAL1 };

            bcem_Aggregate agg;
            ASSERT(0 == loadPrimitives(&agg));

            bcem_Aggregate       AGG = agg.clone();
            const bool           VAL = true;
            bool                 val = VAL;
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val = agg.fieldById(ID).asBool();
            ASSERT(VAL != val);
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val == agg.fieldById(ID).asBool());
        }

        if (verbose) cout << "\t...with 'char'.\n" << endl;
        {
            enum { ID = ID_VAL2 };

            bcem_Aggregate agg;
            ASSERT(0 == loadPrimitives(&agg));

            bcem_Aggregate       AGG = agg.clone();
            const char           VAL = NLC::min();
            char                 val = VAL;
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val = agg.fieldById(ID).asChar();
            ASSERT(VAL != val);
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val == agg.fieldById(ID).asChar());
        }

        if (verbose) cout << "\t...with 'short'.\n" << endl;
        {
            enum { ID = ID_VAL3 };

            bcem_Aggregate agg;
            ASSERT(0 == loadPrimitives(&agg));

            bcem_Aggregate       AGG = agg.clone();
            const short          VAL = NLS::min();
            short                val = VAL;
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val = agg.fieldById(ID).asShort();
            ASSERT(VAL != val);
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val == agg.fieldById(ID).asShort());
        }

        if (verbose) cout << "\t...with 'int'.\n" << endl;
        {
            enum { ID = ID_VAL4 };

            bcem_Aggregate agg;
            ASSERT(0 == loadPrimitives(&agg));

            bcem_Aggregate       AGG = agg.clone();
            const int            VAL = NLI::min();
            int                  val = VAL;
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val = agg.fieldById(ID).asInt();
            ASSERT(VAL != val);
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val == agg.fieldById(ID).asInt());
        }

        if (verbose) cout << "\t...with 'float'.\n" << endl;
        {
            enum { ID = ID_VAL5 };

            bcem_Aggregate agg;
            ASSERT(0 == loadPrimitives(&agg));

            bcem_Aggregate       AGG = agg.clone();
            const float          VAL = NLF::min();
            float                val = VAL;
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val = agg.fieldById(ID).asFloat();
            ASSERT(VAL != val);
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val == agg.fieldById(ID).asFloat());
        }

        if (verbose) cout << "\t...with 'double'.\n" << endl;
        {
            enum { ID = ID_VAL6 };

            bcem_Aggregate agg;
            ASSERT(0 == loadPrimitives(&agg));

            bcem_Aggregate       AGG = agg.clone();
            const double         VAL = NLD::min();
            double               val = VAL;
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val = agg.fieldById(ID).asDouble();
            ASSERT(VAL != val);
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val == agg.fieldById(ID).asDouble());
        }

        if (verbose) cout << "\t...with 'unsigned char'.\n" << endl;
        {
            enum { ID = ID_VAL7 };

            bcem_Aggregate agg;
            ASSERT(0 == loadPrimitives(&agg));

            bcem_Aggregate       AGG = agg.clone();
            const unsigned char  VAL = NLUC::min();
            unsigned char        val = VAL;
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val = (unsigned char)agg.fieldById(ID).asChar();
            ASSERT(VAL != val);
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val == (unsigned char)agg.fieldById(ID).asChar());
        }

        if (verbose) cout << "\t...with 'unsigned short'.\n" << endl;
        {
            enum { ID = ID_VAL8 };

            bcem_Aggregate agg;
            ASSERT(0 == loadPrimitives(&agg));

            bcem_Aggregate       AGG = agg.clone();
            const unsigned short VAL = NLUC::min();
            unsigned short       val = VAL;
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val = (unsigned short)agg.fieldById(ID).asChar();
            ASSERT(VAL != val);
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val == (unsigned short)agg.fieldById(ID).asShort());
        }

        if (verbose) cout << "\t...with 'unsigned int'.\n" << endl;
        {
            enum { ID = ID_VAL9 };

            bcem_Aggregate agg;
            ASSERT(0 == loadPrimitives(&agg));

            bcem_Aggregate       AGG = agg.clone();
            const unsigned int   VAL = NLUC::min();
            unsigned int         val = VAL;
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val = (unsigned int)agg.fieldById(ID).asChar();
            ASSERT(VAL != val);
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val == (unsigned int)agg.fieldById(ID).asInt());
        }

        if (verbose) cout << "\t...with 'bsl::string'.\n" << endl;
        {
            enum { ID = ID_VAL10 };

            bcem_Aggregate agg;
            ASSERT(0 == loadPrimitives(&agg));

            bcem_Aggregate       AGG = agg.clone();
            const bsl::string    VAL = "not empty";
            bsl::string          val = VAL;
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val = agg.fieldById(ID).asString();
            ASSERT(VAL != val);
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val == agg.fieldById(ID).asString());
        }

        if (verbose) cout << "\t...with 'Int64'.\n" << endl;
        {
            enum { ID = ID_VAL11 };

            bcem_Aggregate agg;
            ASSERT(0 == loadPrimitives(&agg));

            typedef bsls_PlatformUtil::Int64 Int64;

            bcem_Aggregate        AGG = agg.clone();
            const Int64           VAL = NLI64::min();
            Int64                 val = VAL;
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val = agg.fieldById(ID).asInt64();
            ASSERT(VAL != val);
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val == agg.fieldById(ID).asInt64());
        }

        if (verbose) cout << "\t...with 'Uint64'.\n" << endl;
        {
            enum { ID = ID_VAL12 };

            bcem_Aggregate agg;
            ASSERT(0 == loadPrimitives(&agg));

            typedef bsls_PlatformUtil::Uint64 Uint64;

            bcem_Aggregate        AGG = agg.clone();
            const Uint64          VAL = NLUI64::min();
            Uint64                val = VAL;
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val = (Uint64)agg.fieldById(ID).asInt64();
            ASSERT(VAL != val);
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val == (Uint64)agg.fieldById(ID).asInt64());
        }

        if (verbose) cout << "\t...with 'bdet_DateTz'.\n" << endl;
        {
            enum { ID = ID_VAL13 };

            bcem_Aggregate agg;
            ASSERT(0 == loadPrimitives(&agg));

            bcem_Aggregate        AGG = agg.clone();
            const bdet_DateTz     VAL(bdet_Date(2008, 7, 12), 0);
            bdet_DateTz           val = VAL;
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val = agg.fieldById(ID).asDateTz();
            ASSERT(VAL != val);
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val == agg.fieldById(ID).asDateTz());
        }

        if (verbose) cout << "\t...with 'bdet_DatetimeTz'.\n" << endl;
        {
            enum { ID = ID_VAL14 };

            bcem_Aggregate agg;
            ASSERT(0 == loadPrimitives(&agg));

            bcem_Aggregate        AGG = agg.clone();
            const bdet_DatetimeTz VAL(bdet_Datetime(2008, 7, 12, 16, 53), 0);
            bdet_DatetimeTz       val = VAL;
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val = agg.fieldById(ID).asDatetimeTz();
            ASSERT(VAL != val);
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val == agg.fieldById(ID).asDatetimeTz());
        }

        if (verbose) cout << "\t...with 'bdet_TimeTz'.\n" << endl;
        {
            enum { ID = ID_VAL15 };

            bcem_Aggregate agg;
            ASSERT(0 == loadPrimitives(&agg));

            bcem_Aggregate        AGG = agg.clone();
            const bdet_TimeTz     VAL(bdet_Time(16, 53), 0);
            bdet_TimeTz           val = VAL;
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val = agg.fieldById(ID).asTimeTz();
            ASSERT(VAL != val);
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val == agg.fieldById(ID).asTimeTz());
        }

        if (verbose) cout << "\t...with 'bdet_Date'.\n" << endl;
        {
            enum { ID = ID_VAL16 };

            bcem_Aggregate agg;
            ASSERT(0 == loadPrimitives(&agg));

            bcem_Aggregate        AGG = agg.clone();
            const bdet_Date       VAL(2008, 7, 12);
            bdet_Date             val = VAL;
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val = agg.fieldById(ID).asDate();
            ASSERT(VAL != val);
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val == agg.fieldById(ID).asDate());
        }

        if (verbose) cout << "\t...with 'bdet_Datetime'.\n" << endl;
        {
            enum { ID = ID_VAL17 };

            bcem_Aggregate agg;
            ASSERT(0 == loadPrimitives(&agg));

            bcem_Aggregate        AGG = agg.clone();
            const bdet_Datetime   VAL(2008, 7, 12, 16, 53);
            bdet_Datetime         val = VAL;
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val = agg.fieldById(ID).asDatetime();
            ASSERT(VAL != val);
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val == agg.fieldById(ID).asDatetime());
        }

        if (verbose) cout << "\t...with 'bdet_Time'.\n" << endl;
        {
            enum { ID = ID_VAL18 };

            bcem_Aggregate agg;
            ASSERT(0 == loadPrimitives(&agg));

            bcem_Aggregate        AGG = agg.clone();
            const bdet_Time       VAL(16, 53);
            bdet_Time             val = VAL;
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(0 != Util::fromAggregate(&val, agg, ID_INVALID));
            ASSERT(0 != Util::toAggregate(&agg, ID_INVALID, val));
            ASSERT(true == bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);

            val = agg.fieldById(ID).asTime();
            ASSERT(VAL != val);
            ASSERT(0 == Util::toAggregate(&agg, ID, VAL));
            ASSERT(0 == Util::fromAggregate(&val, agg, ID));
            ASSERT(true != bcem_Aggregate::areEquivalent(AGG, agg));
            ASSERT(VAL == val);
            ASSERT(val == agg.fieldById(ID).asTime());
        }
      }  break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   Exercise the basic functionality of the 'bcem_AggregateUtil'
        //   class.  Ensure that each method is called.
        //
        // Plan:
        //   Generate three 'bcem_Aggregate' objects corresponding to a schema
        //   containing a sequence of primitive values, a schema containing a
        //   sequence of nullable primitive values, and a schema containing a
        //   sequence of primitive value arrays.  For each aggregate, convert
        //   each member to its associated primitive type by calling the
        //   appropriate 'fromAggregate' method, and then convert each
        //   primitive to its associated aggregate type by calling the
        //   appropriate 'toAggregate' method.
        //
        // Testing:
        //   Exercise basic functionality
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "BREATHING TEST" << endl
                 << "==============" << endl;
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) {
            cout << "\na. Primitive Types"
                 << "\n------------------"
                 << endl;
        }
        {
            bcem_Aggregate agg;
            ASSERT(0 == loadPrimitives(&agg));

            bool v1;
            ASSERT(0 == Util::fromAggregate(&v1, agg, ID_VAL1));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL1, v1));

            char v2;
            ASSERT(0 == Util::fromAggregate(&v2, agg, ID_VAL2));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL2, v2));

            short v3;
            ASSERT(0 == Util::fromAggregate(&v3, agg, ID_VAL3));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL3, v3));

            int v4;
            ASSERT(0 == Util::fromAggregate(&v4, agg, ID_VAL4));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL4, v4));

            float v5;
            ASSERT(0 == Util::fromAggregate(&v5, agg, ID_VAL5));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL5, v5));

            double v6;
            ASSERT(0 == Util::fromAggregate(&v6, agg, ID_VAL6));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL6, v6));

            unsigned char v7;
            ASSERT(0 == Util::fromAggregate(&v7, agg, ID_VAL7));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL7, v7));

            unsigned short v8;
            ASSERT(0 == Util::fromAggregate(&v8, agg, ID_VAL8));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL8, v8));

            unsigned int v9;
            ASSERT(0 == Util::fromAggregate(&v9, agg, ID_VAL9));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL9, v9));

            bsl::string v10;
            ASSERT(0 == Util::fromAggregate(&v10, agg, ID_VAL10));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL10, v10));

            bsls_PlatformUtil::Int64 v11;
            ASSERT(0 == Util::fromAggregate(&v11, agg, ID_VAL11));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL11, v11));

            bsls_PlatformUtil::Uint64 v12;
            ASSERT(0 == Util::fromAggregate(&v12, agg, ID_VAL12));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL12, v12));

            bdet_DateTz v13;
            ASSERT(0 == Util::fromAggregate(&v13, agg, ID_VAL13));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL13, v13));

            bdet_DatetimeTz v14;
            ASSERT(0 == Util::fromAggregate(&v14, agg, ID_VAL14));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL14, v14));

            bdet_TimeTz v15;
            ASSERT(0 == Util::fromAggregate(&v15, agg, ID_VAL15));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL15, v15));

            bdet_Date v16;
            ASSERT(0 == Util::fromAggregate(&v16, agg, ID_VAL16));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL16, v16));

            bdet_Datetime v17;
            ASSERT(0 == Util::fromAggregate(&v17, agg, ID_VAL17));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL17, v17));

            bdet_Time v18;
            ASSERT(0 == Util::fromAggregate(&v18, agg, ID_VAL18));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL18, v18));
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) {
            cout << "\nb. Nullable Types"
                 << "\n-----------------"
                 << endl;
        }
        {
            bcem_Aggregate agg;
            ASSERT(0 == loadNullables(&agg));

            bdeut_NullableValue<bool> v1;
            ASSERT(0 == Util::fromAggregate(&v1, agg, ID_VAL1));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL1, v1));

            bdeut_NullableValue<char> v2;
            ASSERT(0 == Util::fromAggregate(&v2, agg, ID_VAL2));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL2, v2));

            bdeut_NullableValue<short> v3;
            ASSERT(0 == Util::fromAggregate(&v3, agg, ID_VAL3));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL3, v3));

            bdeut_NullableValue<int> v4;
            ASSERT(0 == Util::fromAggregate(&v4, agg, ID_VAL4));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL4, v4));

            bdeut_NullableValue<float> v5;
            ASSERT(0 == Util::fromAggregate(&v5, agg, ID_VAL5));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL5, v5));

            bdeut_NullableValue<double> v6;
            ASSERT(0 == Util::fromAggregate(&v6, agg, ID_VAL6));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL6, v6));

            bdeut_NullableValue<unsigned char> v7;
            ASSERT(0 == Util::fromAggregate(&v7, agg, ID_VAL7));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL7, v7));

            bdeut_NullableValue<unsigned short> v8;
            ASSERT(0 == Util::fromAggregate(&v8, agg, ID_VAL8));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL8, v8));

            bdeut_NullableValue<unsigned int> v9;
            ASSERT(0 == Util::fromAggregate(&v9, agg, ID_VAL9));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL9, v9));

            bdeut_NullableValue<bsl::string> v10;
            ASSERT(0 == Util::fromAggregate(&v10, agg, ID_VAL10));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL10, v10));

            bdeut_NullableValue<bsls_PlatformUtil::Int64> v11;
            ASSERT(0 == Util::fromAggregate(&v11, agg, ID_VAL11));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL11, v11));

            bdeut_NullableValue<bsls_PlatformUtil::Uint64> v12;
            ASSERT(0 == Util::fromAggregate(&v12, agg, ID_VAL12));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL12, v12));

            bdeut_NullableValue<bdet_DateTz> v13;
            ASSERT(0 == Util::fromAggregate(&v13, agg, ID_VAL13));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL13, v13));

            bdeut_NullableValue<bdet_DatetimeTz> v14;
            ASSERT(0 == Util::fromAggregate(&v14, agg, ID_VAL14));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL14, v14));

            bdeut_NullableValue<bdet_TimeTz> v15;
            ASSERT(0 == Util::fromAggregate(&v15, agg, ID_VAL15));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL15, v15));

            bdeut_NullableValue<bdet_Date> v16;
            ASSERT(0 == Util::fromAggregate(&v16, agg, ID_VAL16));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL16, v16));

            bdeut_NullableValue<bdet_Datetime> v17;
            ASSERT(0 == Util::fromAggregate(&v17, agg, ID_VAL17));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL17, v17));

            bdeut_NullableValue<bdet_Time> v18;
            ASSERT(0 == Util::fromAggregate(&v18, agg, ID_VAL18));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL18, v18));
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) {
            cout << "\nc. Array Types"
                 << "\n--------------"
                 << endl;
        }
        {
            bcem_Aggregate agg;
            ASSERT(0 == loadArrays(&agg));

            bsl::vector<bool> v1;
            ASSERT(0 == Util::fromAggregate(&v1, agg, ID_VAL1));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL1, v1));

            bsl::vector<char> v2;
            ASSERT(0 == Util::fromAggregate(&v2, agg, ID_VAL2));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL2, v2));

            bsl::vector<short> v3;
            ASSERT(0 == Util::fromAggregate(&v3, agg, ID_VAL3));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL3, v3));

            bsl::vector<int> v4;
            ASSERT(0 == Util::fromAggregate(&v4, agg, ID_VAL4));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL4, v4));

            bsl::vector<float> v5;
            ASSERT(0 == Util::fromAggregate(&v5, agg, ID_VAL5));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL5, v5));

            bsl::vector<double> v6;
            ASSERT(0 == Util::fromAggregate(&v6, agg, ID_VAL6));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL6, v6));

            bsl::vector<unsigned char> v7;
            ASSERT(0 == Util::fromAggregate(&v7, agg, ID_VAL7));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL7, v7));

            bsl::vector<unsigned short> v8;
            ASSERT(0 == Util::fromAggregate(&v8, agg, ID_VAL8));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL8, v8));

            bsl::vector<unsigned int> v9;
            ASSERT(0 == Util::fromAggregate(&v9, agg, ID_VAL9));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL9, v9));

            bsl::vector<bsl::string> v10;
            ASSERT(0 == Util::fromAggregate(&v10, agg, ID_VAL10));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL10, v10));

            bsl::vector<bsls_PlatformUtil::Int64> v11;
            ASSERT(0 == Util::fromAggregate(&v11, agg, ID_VAL11));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL11, v11));

            bsl::vector<bsls_PlatformUtil::Uint64> v12;
            ASSERT(0 == Util::fromAggregate(&v12, agg, ID_VAL12));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL12, v12));

            bsl::vector<bdet_DateTz> v13;
            ASSERT(0 == Util::fromAggregate(&v13, agg, ID_VAL13));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL13, v13));

            bsl::vector<bdet_DatetimeTz> v14;
            ASSERT(0 == Util::fromAggregate(&v14, agg, ID_VAL14));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL14, v14));

            bsl::vector<bdet_TimeTz> v15;
            ASSERT(0 == Util::fromAggregate(&v15, agg, ID_VAL15));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL15, v15));

            bsl::vector<bdet_Date> v16;
            ASSERT(0 == Util::fromAggregate(&v16, agg, ID_VAL16));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL16, v16));

            bsl::vector<bdet_Datetime> v17;
            ASSERT(0 == Util::fromAggregate(&v17, agg, ID_VAL17));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL17, v17));

            bsl::vector<bdet_Time> v18;
            ASSERT(0 == Util::fromAggregate(&v18, agg, ID_VAL18));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL18, v18));
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) {
            cout << "\nc. Array of Nullable Values"
                 << "\n---------------------------"
                 << endl;
        }
        {
            bcem_Aggregate agg;
            ASSERT(0 == loadArrays(&agg));

            bsl::vector< bdeut_NullableValue<bool> > v1;
            ASSERT(0 == Util::fromAggregate(&v1, agg, ID_VAL1));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL1, v1));

            bsl::vector< bdeut_NullableValue<char> > v2;
            ASSERT(0 == Util::fromAggregate(&v2, agg, ID_VAL2));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL2, v2));

            bsl::vector< bdeut_NullableValue<short> > v3;
            ASSERT(0 == Util::fromAggregate(&v3, agg, ID_VAL3));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL3, v3));

            bsl::vector< bdeut_NullableValue<int> > v4;
            ASSERT(0 == Util::fromAggregate(&v4, agg, ID_VAL4));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL4, v4));

            bsl::vector< bdeut_NullableValue<float> > v5;
            ASSERT(0 == Util::fromAggregate(&v5, agg, ID_VAL5));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL5, v5));

            bsl::vector< bdeut_NullableValue<double> > v6;
            ASSERT(0 == Util::fromAggregate(&v6, agg, ID_VAL6));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL6, v6));

            bsl::vector< bdeut_NullableValue<unsigned char> > v7;
            ASSERT(0 == Util::fromAggregate(&v7, agg, ID_VAL7));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL7, v7));

            bsl::vector< bdeut_NullableValue<unsigned short> > v8;
            ASSERT(0 == Util::fromAggregate(&v8, agg, ID_VAL8));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL8, v8));

            bsl::vector< bdeut_NullableValue<unsigned int> > v9;
            ASSERT(0 == Util::fromAggregate(&v9, agg, ID_VAL9));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL9, v9));

            bsl::vector< bdeut_NullableValue<bsl::string> > v10;
            ASSERT(0 == Util::fromAggregate(&v10, agg, ID_VAL10));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL10, v10));

            bsl::vector< bdeut_NullableValue<bsls_PlatformUtil::Int64> > v11;
            ASSERT(0 == Util::fromAggregate(&v11, agg, ID_VAL11));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL11, v11));

            bsl::vector< bdeut_NullableValue<bsls_PlatformUtil::Uint64> > v12;
            ASSERT(0 == Util::fromAggregate(&v12, agg, ID_VAL12));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL12, v12));

            bsl::vector< bdeut_NullableValue<bdet_DateTz> > v13;
            ASSERT(0 == Util::fromAggregate(&v13, agg, ID_VAL13));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL13, v13));

            bsl::vector< bdeut_NullableValue<bdet_DatetimeTz> > v14;
            ASSERT(0 == Util::fromAggregate(&v14, agg, ID_VAL14));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL14, v14));

            bsl::vector< bdeut_NullableValue<bdet_TimeTz> > v15;
            ASSERT(0 == Util::fromAggregate(&v15, agg, ID_VAL15));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL15, v15));

            bsl::vector< bdeut_NullableValue<bdet_Date> > v16;
            ASSERT(0 == Util::fromAggregate(&v16, agg, ID_VAL16));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL16, v16));

            bsl::vector< bdeut_NullableValue<bdet_Datetime> > v17;
            ASSERT(0 == Util::fromAggregate(&v17, agg, ID_VAL17));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL17, v17));

            bsl::vector< bdeut_NullableValue<bdet_Time> > v18;
            ASSERT(0 == Util::fromAggregate(&v18, agg, ID_VAL18));
            ASSERT(0 == Util::toAggregate(&agg, ID_VAL18, v18));
        }
      }  break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "."
             << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
