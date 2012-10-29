// baea_serializableobjectproxy.t.cpp                                 -*-C++-*-
#include <baea_serializableobjectproxy.h>

#include <baexml_encoder.h>
#include <baexml_decoder.h>
#include <baexml_minireader.h>
#include <bdesb_fixedmeminstreambuf.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bslma_sequentialallocator.h>

#include <bdeut_nullablevalue.h>

#include <cstdlib>

#include <bslalg_typetraits.h>
#include <bdeat_attributeinfo.h>
#include <bdeat_selectioninfo.h>
#include <bdeat_typetraits.h>
#include <bsls_objectbuffer.h>
#include <bdex_instreamfunctions.h>
#include <bdex_outstreamfunctions.h>
#include <bsls_assert.h>

#include <bcem_aggregate.h>
#include <bcem_aggregateutil.h>

#include <bdeat_formattingmode.h>
#include <bdeat_valuetypefunctions.h>
#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_string.h>

#include <bsl_vector.h>
#include <bsls_assert.h>

#include <iomanip>
#include <limits>

using namespace BloombergLP;
using namespace BloombergLP::bdeat_TypeCategoryFunctions;

using bsl::cout;
using bsl::endl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// CREATORS
// [ 1] baea_SerializableObjectProxy;
//
// NO-OP FUNCTIONS FOR INTEGRATION
// [  ] void reset();
//
// MANIPULATORS
// [ 4] void resize(size_t newSize);
// [ 5] int choiceMakeSelection(int selectionId);
// [ 5] int choiceMakeSelection(const char *name, int length);
// [ 5] int choiceManipulateSelection(MANIPULATOR& manipulator);
// [ 4] int arrayManipulateElement(MANIPULATOR& manipulator, int index);
// [ 6] int sequenceManipulateAttribute(MANIPULATOR& manipulator, int id);
// [ 6] int sequenceManipulateAttribute(MANIPULATOR& m, const char*  n, int l);
// [ 2] int manipulateSimple(MANIPULATOR& manipulator);
// [ 7] int manipulateNullable(MANIPULATOR& manipulator);
// [ 3] int enumFromInt(int value);
// [ 3] int enumFromString(const char *stringValue, int stringLength);
// [ 7] void makeValue();
// [ 2] void loadSimple(char               *value);
// [ 2] void loadSimple(unsigned char      *value);
// [ 2] void loadSimple(short              *value);
// [ 2] void loadSimple(int                *value);
// [ 2] void loadSimple(bsls_Types::Int64  *value);
// [ 2] void loadSimple(unsigned short     *value);
// [ 2] void loadSimple(unsigned int       *value);
// [ 2] void loadSimple(bsls_Types::Uint64 *value);
// [ 2] void loadSimple(float              *value);
// [ 2] void loadSimple(double             *value);
// [ 2] void loadSimple(bsl::string        *value);
// [ 2] void loadSimple(bdet_Datetime      *value);
// [ 2] void loadSimple(bdet_Date          *value);
// [ 2] void loadSimple(bdet_Time          *value);
// [ 2] void loadSimple(bool               *value);
// [ 2] void loadSimple(bdet_DatetimeTz    *value);
// [ 2] void loadSimple(bdet_DateTz        *value);
// [ 2] void loadSimple(bdet_TimeTz        *value);
// [ 5] loadChoice(int, void *, const SInfo *, const char *, Accessor);
// [ 5] loadChoice(int, void *, const SInfo *, AccssrNExtrctr, Chooser);
// [ 6] loadSequence(int, void *, const AInfo *, const char *, ElmtAccssr);
// [ 4] loadArray(int length, int elementSize, void *begin, Accessor a);
// [ 4] loadArray(void *, int, int, void *, Resizer, Accessor);
// [ 7] loadNullable(void *object, Accessor accessor);
// [ 7] loadNullable(void *obj, Accessor, Manipulator, ObjectFetcher);
// [ 3] loadEnumeration(int v, const EnumeratorInfo *a, int s);
// [ 3] loadEnumeration(void *, IntSetter, StrSetter, const EInfo *, int);
//
// ACCESSORS
// [ 9] const char *className() const;
// [ 9] bdeat_TypeCategory::Value category() const;
// [ 5] int selectionId() const;
// [ 4] bsl::size_t size() const;
// [ 9] const void* object() const;
// [ 4] bool isByteArrayValue() const;
// [ 7] bool isNull() const;
// [ 5] bool choiceHasSelection(const char *name, int len) const;
// [ 5] bool choiceHasSelection(int selectionId) const;
// [ 6] bool sequenceHasAttribute(const char *name, int len) const;
// [ 6] bool sequenceHasAttribute(int attributeId) const;
// [ 5] int choiceAccessSelection(ACCESSOR& accessor) const;
// [ 6] int sequenceAccessAttribute(ACCESSOR& accessor, int attrId) const;
// [ 6] int sequenceAccessAttributes(ACCESSOR& accessor) const;
// [ 7] int accessNullable(ACCESSOR& accessor) const;
// [ 2] int accessSimple(ACCESSOR& accessor) const;
// [ 4] int arrayAccessElement(ACCESSOR& accessor, int index) const;
// [ 3] int enumToInt() const;
// [ 3] const char* enumToString() const;

// 'bdeat_typecategory' OVERLOADS
// [ 9] Value bdeat_typeCategorySelect(const ObjectProxy& obj);
// [ 2] int bdeat_typeCategoryAccessSimple(const ObjProxy&, ACCESSOR& a);
// [ 2] int bdeat_typeCategoryManipulateSimple(ObjProxy *, MANIPULATOR&);
// [ 4] int bdeat_typeCategoryManipulateArray(ObjProxy *, MANIPULATOR&);
// [ 4] int bdeat_typeCategoryAccessArray(const ObjectProxy&, ACCESSOR&);
// [ 7] int bdeat_typeCategoryAccessNullableValue(const ObjProxy&, ACC&);
// [ 7] int bdeat_typeCategoryManipulateNullableValue(ObjProxy*, MANIP&);
//
// 'bdeat_EnumFunctions' OVERLOADS
//
// [ 3] void bdeat_enumToInt(int *, const ObjectProxy& object)
// [ 3] int bdeat_enumFromInt(ObjectProxy *result, int value)
// [ 3] void bdeat_enumToString(bsl::string *, const ObjectProxy& object)
// [ 3] int bdeat_enumFromString(ObjectProxy *, const char *str, int len)
//
// 'bdeat_arrayfunctions' OVERLOADS AND SPECIALIZATIONS
// [ 4] bsl::size_t bdeat_arraySize(const ObjectProxy& object)
// [ 4] void bdeat_arrayResize(ObjectProxy* object, int newSize)
// [ 4] int bdeat_arrayAccessElement(const ObjectProxy&, ACCESSOR&, int)
// [ 4] int bdeat_arrayManipulateElement(ObjectProxy*, MANIPULATOR&, int)
//
// 'bdeat_sequencefunctions' OVERLOADS AND SPECIALIZATIONS
// [ 6] int bdeat_sequenceAccessAttributes(const ObjectProxy&, ACCESSOR&)
// [ 6] int bdeat_sequenceAccessAttribute(const ObjProxy&, ACCESSOR&, int)
// [ 6] bdeat_sequenceAccessAttribute(const Proxy&, AC&, const char*, int)
// [ 6] int bdeat_sequenceManipulateAttributes(ObjProxy *, MANIPULATOR&)
// [ 6] bdeat_sequenceManipulateAttribute(ObjProxy *, MANIPULATOR&, int)
// [ 6] bdeat_sequenceManipulateAttribute(Proxy*, MAN&, const char*, int)
// [ 6] bdeat_sequenceHasAttribute(const ObjectProxy&, int)
// [ 6] bdeat_sequenceHasAttribute(const ObjProxy&, const char *, int)
//
// 'bdeat_nullablevaluefunctions' OVERLOADS AND SPECIALIZATIONS
// [ 8] bool bdeat_nullableValueIsNull(const NullableAdapter& object)
// [ 8] void bdeat_nullableValueMakeValue(NullableAdapter *object)
// [ 8] int bdeat_nullableValueManipulateValue(Nullable*, MANIPULATOR&)
// [ 8] int bdeat_nullableValueAccessValue(const Nullable&, ACCESSOR&)
//
// 'bdeat_typename' overloads
// [ 9] const char *bdeat_TypeName_className(const ObjectProxy& object)
//
// 'bdeat_choicefunctions' overloads and specializations
// [ 5] int bdeat_choiceSelectionId(const ObjectProxy& object)
// [ 5] int bdeat_choiceAccessSelection(const ObjectProxy&, ACCESSOR&)
// [ 5] bool bdeat_choiceHasSelection(const ObjProxy&, const char *, int)
// [ 5] bool bdeat_choiceHasSelection(const ObjectProxy&, int)
// [ 5] int bdeat_choiceMakeSelection(ObjectProxy *obj, int selectionId)
// [ 5] int bdeat_choiceMakeSelection(ObjectProxy  *, const char *, int)
// [ 5] int bdeat_choiceManipulateSelection(ObjectProxy *, MANIPULATOR&)

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

namespace {

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

}  // close anonymous namespace

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t"  \
                          << #J << ": " << J << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" \
                         << #J << ": " << J << "\t" \
                         << #K << ": " << K << "\n";\
               aSsErT(1, #X, __LINE__); } }

#define LOOP0_ASSERT ASSERT
#define LOOP1_ASSERT LOOP_ASSERT

//=============================================================================
//                  STANDARD BDE VARIADIC ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define NUM_ARGS_IMPL(X5, X4, X3, X2, X1, X0, N, ...)   N
#define NUM_ARGS(...) NUM_ARGS_IMPL(__VA_ARGS__, 5, 4, 3, 2, 1, 0, "")

#define LOOPN_ASSERT_IMPL(N, ...) LOOP ## N ## _ASSERT(__VA_ARGS__)
#define LOOPN_ASSERT(N, ...)      LOOPN_ASSERT_IMPL(N, __VA_ARGS__)

#define ASSERTV(...) LOOPN_ASSERT(NUM_ARGS(__VA_ARGS__), __VA_ARGS__)

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl;
                                              // Print identifier and value.
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;
                                              // Quote identifier literally.
#define P_(X) bsl::cout << #X " = " << (X) << ", " << bsl::flush;
                                              // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define NL "\n"

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;
static int veryVeryVeryVerbose = 0;

const char LOG_CATEGORY[] = "BAEA_SERIALIZABLEOBJECTPROXY.TEST";

typedef baea_SerializableObjectProxy Obj;
typedef bdeat_TypeCategory           Category;

//=============================================================================
//                  GENERATED TEST CLASS
//-----------------------------------------------------------------------------
namespace BloombergLP {

class bcem_Aggregate;

namespace test { class Simple; }
namespace test {

                                // ============
                                // class Simple
                                // ============

class Simple {

    // INSTANCE DATA
    int  d_status;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_STATUS = 0
    };

    enum {
        NUM_ATTRIBUTES = 1
    };

    enum {
        ATTRIBUTE_INDEX_STATUS = 0
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
    Simple();
        // Create an object of type 'Simple' having the default value.

    Simple(const Simple& original);
        // Create an object of type 'Simple' having the value of the specified
        // 'original' object.

    ~Simple();
        // Destroy this object.

    // MANIPULATORS
    Simple& operator=(const Simple& rhs);
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

    int& status();
        // Return a reference to the modifiable "Status" attribute of this
        // object.

    // ACCESSORS
    std::ostream& print(std::ostream& stream,
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

    int status() const;
        // Return a reference to the non-modifiable "Status" attribute of this
        // object.
};

// FREE OPERATORS
inline
bool operator==(const Simple& lhs, const Simple& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const Simple& lhs, const Simple& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
std::ostream& operator<<(std::ostream& stream, const Simple& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace test

// TRAITS

BDEAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(test::Simple)

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace test {

                                // ------------
                                // class Simple
                                // ------------

// CLASS METHODS
inline
int Simple::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// MANIPULATORS
template <class STREAM>
STREAM& Simple::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_status, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

template <class MANIPULATOR>
int Simple::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_status, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATUS]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int Simple::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_STATUS: {
        return manipulator(&d_status, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATUS]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int Simple::manipulateAttribute(
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
int& Simple::status()
{
    return d_status;
}

// ACCESSORS
template <class STREAM>
STREAM& Simple::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_status, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
int Simple::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_status, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATUS]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int Simple::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_STATUS: {
        return accessor(d_status, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATUS]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int Simple::accessAttribute(
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
int Simple::status() const
{
    return d_status;
}

}  // close namespace test

// FREE FUNCTIONS

inline
bool test::operator==(
        const test::Simple& lhs,
        const test::Simple& rhs)
{
    return  lhs.status() == rhs.status();
}

inline
bool test::operator!=(
        const test::Simple& lhs,
        const test::Simple& rhs)
{
    return  lhs.status() != rhs.status();
}

inline
std::ostream& test::operator<<(
        std::ostream& stream,
        const test::Simple& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP


namespace BloombergLP {
namespace test {

                                // ------------
                                // class Simple
                                // ------------

// CONSTANTS

const char Simple::CLASS_NAME[] = "Simple";

const bdeat_AttributeInfo Simple::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_STATUS,
        "status",
        sizeof("status") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *Simple::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 6: {
            if (name[0]=='s'
             && name[1]=='t'
             && name[2]=='a'
             && name[3]=='t'
             && name[4]=='u'
             && name[5]=='s')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATUS];
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *Simple::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_STATUS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATUS];
      default:
        return 0;
    }
}

// CREATORS

Simple::Simple()
: d_status()
{
}

Simple::Simple(const Simple& original)
: d_status(original.d_status)
{
}

Simple::~Simple()
{
}

// MANIPULATORS

Simple&
Simple::operator=(const Simple& rhs)
{
    if (this != &rhs) {
        d_status = rhs.d_status;
    }
    return *this;
}

int Simple::fromAggregate(const bcem_Aggregate& aggregate)
{
    int rc;
    if ((rc = bcem_AggregateUtil::fromAggregate(
                       &d_status,
                       aggregate,
                       ATTRIBUTE_ID_STATUS)))
    {
        return rc;
    }
    return 0;
}

void Simple::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_status);
}

// ACCESSORS
std::ostream& Simple::print(
    std::ostream& stream,
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
        stream << "Status = ";
        bdeu_PrintMethods::print(stream, d_status,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Status = ";
        bdeu_PrintMethods::print(stream, d_status,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << std::flush;
}

int Simple::toAggregate(bcem_Aggregate *result) const
{
    int rc;

    rc = bcem_AggregateUtil::toAggregate(
                       result,
                       ATTRIBUTE_ID_STATUS,
                       d_status);
    if (rc != 0 && rc != bcem_Aggregate::BCEM_ERR_BAD_FIELDID) {
        return rc;
    }

    return 0;
}

}  // close namespace test
}  // close namespace BloombergLP

//=============================================================================
//                         TEST CALLBACK FUNCTIONS
//-----------------------------------------------------------------------------

namespace {

struct ExtractAddressAccessor {
    const void *d_address;

    ExtractAddressAccessor() : d_address(0) {};

    int operator()(const baea_SerializableObjectProxy& object)
    {
        d_address = object.object();
        return 0;
    }

    template <class TYPE>
    int operator() (const TYPE&)
    {
        // needed to compile due to nullable adapter, but should not be called
        ASSERTV(!"Should be unreachable");
        return -1;
    }
};

struct ExtractAddressManipulator {
    const void *d_address;

    ExtractAddressManipulator() : d_address(0) {};

    int operator()(baea_SerializableObjectProxy *object)
    {
        d_address = object->object();
        return 0;
    }

    template <class TYPE>
    int operator() (TYPE *)
    {
        // needed to compile due to nullable adapter, but should not be called
        ASSERTV(!"Should be unreachable");
        return -1;
    }
};

template<typename TYPE>
struct SimpleAccessor {
    const void *d_address;
    int         d_rc;

    // CREATORS
    SimpleAccessor() : d_address(0), d_rc(0) {};

    // MANIPULATORS
    void reset()
    {
        d_address = 0;
        d_rc = 0;
    }

    int operator()(const TYPE& object, bdeat_TypeCategory::Simple)
    {
        d_address = &object;
        return d_rc;
    }

    template<typename OTHER_TYPE, typename OTHER_CATEGORY>
    int operator()(const OTHER_TYPE&, const OTHER_CATEGORY&)
    {
        // This is needed to compile because there are many Simple types, but
        // should not be called.

        ASSERTV(!"Should be unreachable");
        return -1;
    }
};

template <class TYPE>
struct SimpleManipulator {
    void *d_address;
    int  d_rc;

    // CREATORS
    SimpleManipulator() : d_address(0), d_rc(0) {};

    // MANIPULATORS
    void reset()
    {
        d_address = 0;
        d_rc = 0;
    }

    int operator()(TYPE *value, bdeat_TypeCategory::Simple)
    {
        d_address = value;
        return d_rc;
    }

    template <class OTHER_TYPE, class OTHER_CATEGORY>
    int operator()(OTHER_TYPE *, const OTHER_CATEGORY&)
    {
        // needed to compile due to nullable adapter, but should not be called
        ASSERTV(!"Should be unreachable");
        return -1;
    }
};

template <class TYPE>
struct ArrayElementAccessor {
    const baea_SerializableObjectProxy *d_proxy;
    const void                         *d_address;
    int                                 d_rc;

    // CREATORS
    ArrayElementAccessor() : d_address(0), d_rc(0) {}

    // MANIPULATORS
    void reset()
    {
        d_proxy = 0;
        d_address = 0;
        d_rc = 0;
    }

    int operator() (const baea_SerializableObjectProxy& object)
    {
        d_proxy = &object;

        SimpleAccessor<TYPE> accessor;
        object.accessSimple(accessor);
        d_address = accessor.d_address;

        return d_rc;
    }

    int operator() (const baea_SerializableObjectProxy_NullableAdapter&)
    {
        // needed to compile due to nullable adapter, but should not be called
        ASSERTV(!"Should be unreachable");
        return -1;
    }
};

struct ArrayAccessor {
    const void                    *d_address;
    int                            d_rc;

    ArrayAccessor() : d_address(0), d_rc(0) {}

    int operator() (const baea_SerializableObjectProxy& object,
                    const bdeat_TypeCategory::Array&)
    {
        d_address = object.object();
        return d_rc;
    }

    int operator() (const bsl::vector<char>& object,
                    const bdeat_TypeCategory::Array&)
    {
        d_address = &object;
        return d_rc;
    }
};

template <class TYPE>
struct ArrayElementManipulator {
    const baea_SerializableObjectProxy *d_proxy;
    void                               *d_address;
    int                                 d_rc;

    // CREATORS
    ArrayElementManipulator() : d_address(0), d_rc(0) {}

    // MANIPULATORS
    void reset()
    {
        d_proxy = 0;
        d_address = 0;
        d_rc = 0;
    }

    int operator() (baea_SerializableObjectProxy *object)
    {
        d_proxy = object;

        SimpleManipulator<TYPE> manipulator;
        object->manipulateSimple(manipulator);
        d_address = manipulator.d_address;
        return d_rc;
    }

    int operator() (baea_SerializableObjectProxy_NullableAdapter *)
    {
        // needed to compile due to nullable adapter, but should not be called
        ASSERTV(!"Should be unreachable");
        return -1;
    }
};

struct ArrayManipulator {
    const void *d_address;
    int         d_rc;

    ArrayManipulator() : d_address(0), d_rc(0) {}

    int operator() (baea_SerializableObjectProxy *object,
                    const bdeat_TypeCategory::Array&)
    {
        d_address = object->object();
        return d_rc;
    }

    int operator() (bsl::vector<char> *object,
                    const bdeat_TypeCategory::Array&)
    {
        d_address = object;
        return d_rc;
    }
};

struct SequenceAccessor {
    const baea_SerializableObjectProxy *d_proxy;
    const void                         *d_address;
    bdeat_AttributeInfo                 d_info;
    int                                 d_rc;

    // CREATORS
    SequenceAccessor() : d_proxy(0), d_address(0), d_rc(0) {}

    // MANIPULATORS
    void reset()
    {
        d_proxy = 0;
        d_address = 0;
        d_rc = 0;
    }

    int operator() (const baea_SerializableObjectProxy& object,
                    const bdeat_AttributeInfo&     info)
    {
        d_proxy = &object;
        SimpleAccessor<int> accessor;
        object.accessSimple(accessor);

        d_address = accessor.d_address;
        d_info = info;
        return d_rc;
    }

    template <typename TYPE>
    int operator() (const TYPE&, const bdeat_AttributeInfo&)
    {
        // needed to compile due to nullable adapter, but should not be called

        ASSERTV(!"Should be unreachable");
        return -1;
    }
};

struct SequenceManipulator {
    const baea_SerializableObjectProxy *d_proxy;
    const void                         *d_address;
    bdeat_AttributeInfo                 d_info;
    int                                 d_rc;

    // CREATORS
    SequenceManipulator() : d_proxy(0), d_address(0), d_rc(0) {}

    // MANIPULATORS
    void reset()
    {
        d_proxy = 0;
        d_address = 0;
        d_rc = 0;
    }

    int operator() (baea_SerializableObjectProxy *object,
                    const bdeat_AttributeInfo&    info)
    {
        d_proxy = object;
        SimpleAccessor<int> extractor;
        object->accessSimple(extractor);

        d_address = extractor.d_address;
        d_info = info;
        return d_rc;
    }

    template <typename TYPE>
    int operator() (TYPE *, const bdeat_AttributeInfo&)
    {
        // needed to compile due to nullable adapter, but should not be called

        ASSERTV(!"Should be unreachable");
        return -1;
    }
};

struct ChoiceAccessor {
    const baea_SerializableObjectProxy *d_proxy;
    const void                    *d_address;
    bdeat_SelectionInfo            d_info;
    int                            d_rc;

    ChoiceAccessor() : d_proxy(0), d_address(0), d_rc(0) {}

    int operator() (const baea_SerializableObjectProxy& object,
                    const bdeat_SelectionInfo&          info)
    {
        d_proxy = &object;
        d_address = object.object();
        d_info = info;
        return d_rc;
    }

    template <typename TYPE>
    int operator() (const TYPE&, const bdeat_SelectionInfo&)
    {
        // needed to compile due to nullable adapter, but should not be called

        ASSERTV(!"Should be unreachable");
        return -1;
    }
};

struct ChoiceManipulator
{
    baea_SerializableObjectProxy *d_proxy;
    const void                   *d_address;
    bdeat_SelectionInfo           d_info;
    int                           d_rc;

    ChoiceManipulator() : d_proxy(0), d_address(0), d_rc(0) {}

    int operator() (baea_SerializableObjectProxy *object,
                    const bdeat_SelectionInfo&    info)
    {
        d_proxy = object;
        d_address = object->object();
        d_info = info;
        return d_rc;
    }

    template <typename TYPE>
    int operator() (TYPE*, const bdeat_SelectionInfo&)
    {
        // needed to compile due to nullable adapter, but should not be called
        ASSERTV(!"Should be unreachable");
        return -1;
    }
};

baea_SerializableObjectProxy *s_elementLoaderFn_proxy;
const void                   *s_elementLoaderFn_object;
int                           s_elementLoaderFn_int;
int                           s_elementLoaderFn_index;
bsl::vector<int>              s_elementLoaderFn_indexes;
void elementLoaderFn(baea_SerializableObjectProxy        *proxy,
                     const baea_SerializableObjectProxy&  object,
                     int                             index)
{
    s_elementLoaderFn_proxy = proxy;
    s_elementLoaderFn_object = &object;
    s_elementLoaderFn_index = index;
    s_elementLoaderFn_indexes.push_back(index);
    proxy->loadSimple(&s_elementLoaderFn_int);
}

baea_SerializableObjectProxy *s_loaderFn_proxy;
void                    *s_loaderFn_object;
int                      s_loaderFn_int;
template<class TYPE>
void loaderFn(baea_SerializableObjectProxy *proxy, void* object)
{
    s_loaderFn_proxy = proxy;
    s_loaderFn_object = object;
    proxy->loadSimple(&s_loaderFn_int);
}

baea_SerializableObjectProxy   *s_selectionLoaderFn_proxy;
void                           *s_selectionLoaderFn_object;
const bdeat_SelectionInfo      *s_selectionLoaderFn_selectInfoPtr;
int                             s_selectionLoaderFn_int;
void selectionLoaderFn(baea_SerializableObjectProxy  *proxy,
                       void                          *object,
                       const bdeat_SelectionInfo    **selectInfoPtr)
{
    s_selectionLoaderFn_proxy = proxy;
    s_selectionLoaderFn_object = object;
    *selectInfoPtr = s_selectionLoaderFn_selectInfoPtr;
    proxy->loadSimple(&s_selectionLoaderFn_int);
}

int   s_chooserFn_rc = 0;
void *s_chooserFn_object;
int   s_chooserFn_id;
int chooserFn(void *object, int selectionId)
{
    s_chooserFn_object = object;
    s_chooserFn_id = selectionId;
    return s_chooserFn_rc;
}


template <typename NULLABLE>
void nullableValueMaker(void *object)
{
    ((NULLABLE*)object)->makeValue();
}

template<typename NULLABLE>
void* nullableValueFetcher(void *object)
{
    NULLABLE* nullableValue = (NULLABLE*)object;
    if (nullableValue->isNull()) {
        // We also use this function for testing for null.

        return 0;                                                     // RETURN
    }
    return &nullableValue->value();
}

void         *s_resizerFn_object  = 0;
void        **s_resizerFn_begin   = 0;
bsl::size_t   s_resizerFn_newSize = 0;
template <class TYPE>
void resizerFn(void* object, void** newBegin, bsl::size_t newSize)
{
    s_resizerFn_object  = object;
    s_resizerFn_begin   = newBegin;
    s_resizerFn_newSize = newSize;
}

int   s_intSetterFn_rc = 0;
void *s_intSetterFn_object = 0;
int   s_intSetterFn_value = 0;
int intSetterFn(void *object, int value)
{
    s_intSetterFn_object = object;
    s_intSetterFn_value  = value;
    return s_intSetterFn_rc;
}

int         s_stringSetterFn_rc     = 0;
void       *s_stringSetterFn_object = 0;
const char *s_stringSetterFn_value  = 0;
int         s_stringSetterFn_length = 0;
int stringSetterFn(void *object, const char *value, int length)
{
    s_stringSetterFn_object = object;
    s_stringSetterFn_value  = value;
    s_stringSetterFn_length = length;

    return s_stringSetterFn_rc;
}

}  // close unamed namespace

// ============================================================================
//                      TEST DRIVER
// ----------------------------------------------------------------------------

template <class TYPE>
void executeSimpleCategoryTest(const char *typeName)
{
    Obj mX; const Obj& X = mX;
    const TYPE DEFAULT = TYPE();
    TYPE obj = DEFAULT;
    mX.loadSimple(&obj);

    if (verbose) cout << "\nTesting properties" << endl;
    {
        ASSERTV(typeName, Category::BDEAT_SIMPLE_CATEGORY == X.category());
        ASSERTV(typeName, Category::BDEAT_SIMPLE_CATEGORY ==
                                                  bdeat_typeCategorySelect(X));

        ASSERTV(&obj == (TYPE*) X.object());
        ASSERTV(false == X.isByteArrayValue());
        ASSERTV(0     == X.className());
    }

    if (verbose) cout << "\naccessSimple" << endl;
    {
        SimpleAccessor<TYPE> accessor;
        ASSERTV(typeName, 0 == X.accessSimple(accessor));
        ASSERTV(typeName, &obj == accessor.d_address);

        accessor.reset();
        accessor.d_rc = 1;
        ASSERTV(typeName, 1 == X.accessSimple(accessor));
        ASSERTV(typeName, &obj == accessor.d_address);
    }

    if (verbose) cout << "\nbdeat_typeCategoryAccessSimple" << endl;
    {
        SimpleAccessor<TYPE> accessor;
        ASSERTV(typeName, 0 == bdeat_typeCategoryAccessSimple(X, accessor));
        ASSERTV(typeName, &obj == accessor.d_address);

        accessor.reset();
        accessor.d_rc = 1;
        ASSERTV(typeName, 1 == bdeat_typeCategoryAccessSimple(X, accessor));
        ASSERTV(typeName, &obj == accessor.d_address);
    }

    if (verbose) cout << "\nmanipulateSimple" << endl;
    {
        SimpleManipulator<TYPE> manipulator;

        ASSERTV(typeName, 0 == mX.manipulateSimple(manipulator));
        ASSERTV(typeName, &obj == manipulator.d_address);

        manipulator.reset();
        manipulator.d_rc = 1;
        ASSERTV(typeName, 1 == mX.manipulateSimple(manipulator));
        ASSERTV(typeName, &obj == manipulator.d_address);
    }

    if (verbose) cout << "\nbdeat_typeCategoryManipulateSimple" << endl;
    {
        SimpleManipulator<TYPE> manipulator;

        ASSERTV(typeName,
                0 == bdeat_typeCategoryManipulateSimple(&mX, manipulator));
        ASSERTV(typeName, &obj == manipulator.d_address);

        manipulator.reset();
        manipulator.d_rc = 1;
        ASSERTV(typeName,
                1 == bdeat_typeCategoryManipulateSimple(&mX, manipulator));
        ASSERTV(typeName, &obj == manipulator.d_address);
    }
}

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating a Proxy Object for a Simple Request
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to decode some XML data.  The data conforms to this schema:
//..
//  <?xml version='1.0' encoding='UTF-8'?>
//  <xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'
//             xmlns:test='http://bloomberg.com/schemas/test'
//             targetNamespace='http://bloomberg.com/schemas/test'
//             elementFormDefault='unqualified'>
//
//      <xs:complexType name='Simple'>
//          <xs:sequence>
//              <xs:element name='status' type='xs:int'/>
//          </xs:sequence>
//      </xs:complexType>
//
//      <xs:element name='Simple' type='test:Simple'/>
//  </xs:schema>
//..
// Using the 'bas_codegen.pl' tool, we can generate C++ classes for this
// schema:
//..
//  $ bas_codegen.pl -m msg -p test -E simple.xsd
//..
// This tool will generate the header and implementation files for the
// 'test_simple' component, which contains a 'Test::Simple' class.
//
// 'Test::Simple' is a Sequence type.  To create a
// 'baea_SerializableObjectProxy' for this type, we first need to create a
// function that can create the proxy object for its element.  Note that in
// this case, 'Test::Simple' only have one element, 'status':
//..
void elementAccessor(baea_SerializableObjectProxy        *proxy,
                     const baea_SerializableObjectProxy&  object,
                     int                                  index)
{
    (void) index;  // remove warning
    test::Simple *simpleObject = (test::Simple *)object.object();
    proxy->loadSimple(&simpleObject->status());
}
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? std::atoi(argv[1]) : 0;

    verbose = (argc > 2);
    veryVerbose = (argc >  3);
    veryVeryVerbose = (argc > 4);
    veryVeryVeryVerbose = (argc > 5);

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    switch (test) { case 0: // Zero is always the leading case.
      case 9: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
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

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;
// Then, we define the XML string that we would like to decode.  This XML
// string defines a 'Simple' object to have a 'status' of 42:
//..
    const char data[] =
        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
        "<Simple xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"
        "    <status>42</status>"
        "</Simple>";
//..
// Next, we create a 'result' object we want to decode the XML string into:
//..
    test::Simple result;
//..
// Then, instead of decoding the XML string directly, we can create a proxy
// object for 'result'.  Since 'test::Simple' is a Sequence object, we use the
// 'loadSequence' method to configure the proxy object.  Note that this is done
// for demonstration purpose.  Users should use functions in the
// 'baea_serializableobjectproxyutil' component to create the proxy object:
//..
    baea_SerializableObjectProxy decodeProxy;
    decodeProxy.loadSequence(test::Simple::NUM_ATTRIBUTES,
                             &result,
                             test::Simple::ATTRIBUTE_INFO_ARRAY,
                             test::Simple::CLASS_NAME,
                             &elementAccessor);
//..
// Now, we create a decoder and decode the XML string into 'result' through the
// 'decodeProxy':
//..
    baexml_DecoderOptions dOptions;
    baexml_MiniReader reader;
    baexml_Decoder decoder(&dOptions, &reader);
//
    bdesb_FixedMemInStreamBuf isb(data, sizeof(data) - 1);
    decoder.decode(&isb, &decodeProxy);
//..
// Finally, we assert that 'result' has the expected value.
//..
    ASSERT(42 == result.status());
//..
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'baea_SerializableObjectProxy_NullableAdapter'
        //
        // Concerns:
        //: 1 The 'bdeat' methods overload correctly fowards the invocation to
        //:   the proxy object held by the nullable adaptor.
        //
        // Plan:
        //: 1 Load a Nullable value to a proxy and create a Nullable adapter
        //:   for that proxy.
        //:
        //: 2 Invoke the 'bdeat' methods and verify the results.
        //
        // Testing:
        //   bool bdeat_nullableValueIsNull(const NullableAdapter& object)
        //   void bdeat_nullableValueMakeValue(NullableAdapter *object)
        //   int bdeat_nullableValueManipulateValue(Nullable*, MANIPULATOR&)
        //   int bdeat_nullableValueAccessValue(const Nullable&, ACCESSOR&)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING Nullable Adapter" << endl
                          << "========================" << endl;

        if (verbose) cout << "\nTesting Adapter for Nullable for Encoding"
                          << endl;
        {
            Obj mX;
            int value = 1;

            mX.loadNullableForEncoding(0, &loaderFn<int>);
            baea_SerializableObjectProxy_NullableAdapter adapter = { &mX };
            ASSERTV(true == bdeat_nullableValueIsNull(adapter));

            mX.loadNullableForEncoding(&value, &loaderFn<int>);
            ASSERTV(false == bdeat_nullableValueIsNull(adapter));

            ExtractAddressAccessor accessor;
            ASSERTV(0 == bdeat_nullableValueAccessValue(adapter, accessor));
            ASSERTV(&value == accessor.d_address);
        }

        if (verbose) cout << "\nTesting Adapter for Nullable for Decoding"
                          << endl;
        {
            Obj mX;

            bdeut_NullableValue<int> value;

            mX.loadNullableForDecoding(
                            &value,
                            &loaderFn<int>,
                            &nullableValueMaker<bdeut_NullableValue<int> >,
                            &nullableValueFetcher<bdeut_NullableValue<int> >);

            baea_SerializableObjectProxy_NullableAdapter adapter = { &mX };

            ASSERTV(true == bdeat_nullableValueIsNull(adapter));

            bdeat_nullableValueMakeValue(&adapter);
            ASSERTV(false == bdeat_nullableValueIsNull(adapter));
            ASSERTV(false == value.isNull());
            ASSERTV(0     == value.value());

            ExtractAddressManipulator manipulator;
            ASSERTV(0 == bdeat_nullableValueManipulateValue(&adapter,
                                                            manipulator));
            ASSERTV(&value.value() == manipulator.d_address);
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING Nullable
        //
        // Concerns:
        //: 1 'loadNullable' creates a proxy object for Nullable type.
        //:
        //: 2 Return 'true' if the proxy object represents a Nullable value
        //:   that is currently null, and 'false' otherwise.
        //:
        //: 3 'accessNullable' and 'manipulateNullable'
        //:   returns the value returned by the suppliced functor.
        //:
        //: 4 The 'accessNullable' and 'manipulateNullable' methods passes the
        //:   correct reference to the function pointers supplied on load.
        //:
        //: 5 'manipulateNullable' invoke the 'ObjectFetcher' supplied at the
        //:   'loadNullable' and pass the return value to the loader.
        //:
        //: 6 'makeValue' invokes the 'ValueMaker' funciton supplied at the
        //:   'loadNullable' method.
        //:
        //: 7 'object' returns ths expected result.
        //:
        //: 8 'bdeat' functions are correctly overloaded and can be found
        //:   through ADL.
        //
        // Plan:
        //: 1 Load a Nullable object into a proxy object.
        //:
        //: 2 Verify properties such as 'object', 'isByteArrayValue', etc.
        //:
        //: 3 Invoke 'access*' and 'manipulate*' method with functors that
        //:   extract the address ofthe object passed to the function.  Verify
        //:   the extracted address and the return value.
        //:
        //: 4 Invoke all the 'bdeat' functions related to for Simple type and
        //:   verify the results.
        //
        // Testing:
        //   int manipulateNullable(MANIPULATOR& manipulator);
        //   void makeValue();
        //   loadNullable(void *object, Accessor accessor);
        //   loadNullable(void *obj, Accessor, Manipulator, ObjectFetcher);
        //   bool isNull() const;
        //   int accessNullable(ACCESSOR& accessor) const;
        //   int bdeat_typeCategoryAccessNullableValue(const ObjProxy&, ACC&);
        //   int bdeat_typeCategoryManipulateNullableValue(ObjProxy*, MANIP&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING Nullable" << endl
                          << "================" << endl;

        if (verbose) cout << "\nTesting 'isNull'" << endl;
        {
            Obj mX; const Obj& X = mX;

            mX.loadNullableForEncoding(0, 0);

            ASSERTV(Category::BDEAT_NULLABLE_VALUE_CATEGORY == X.category());
            ASSERTV(Category::BDEAT_NULLABLE_VALUE_CATEGORY ==
                                                  bdeat_typeCategorySelect(X));
            ASSERTV(0 == X.object());
            ASSERTV(0 == X.className());
            ASSERTV(0 == bdeat_TypeName_Overloadable
                                                ::bdeat_TypeName_className(X));
            ASSERTV(false == X.isByteArrayValue());

            ASSERTV(true == X.isNull());
        }

        if (verbose) cout << "\nTesting Nullable for Encoding" << endl;
        {
            Obj mX; const Obj& X = mX;

            int obj = 1;

            mX.loadNullableForEncoding(&obj, &loaderFn<int>);

            ASSERTV(Category::BDEAT_NULLABLE_VALUE_CATEGORY == X.category());
            ASSERTV(Category::BDEAT_NULLABLE_VALUE_CATEGORY ==
                                                  bdeat_typeCategorySelect(X));
            ASSERTV(&obj == X.object());
            ASSERTV(0 == X.className());
            ASSERTV(0 == bdeat_TypeName_Overloadable
                                                ::bdeat_TypeName_className(X));
            ASSERTV(false == X.isByteArrayValue());

            ASSERTV(false == X.isNull());

            SimpleAccessor<int> accessor;
            s_loaderFn_proxy = 0;
            s_loaderFn_object = 0;

            ASSERTV(0 == X.accessNullable(accessor));
            ASSERTV(s_loaderFn_object == &obj);
            ASSERTV(&s_loaderFn_int == accessor.d_address);

            accessor.reset();
            accessor.d_rc = 1;
            s_loaderFn_proxy = 0;
            s_loaderFn_object = 0;

            ASSERTV(1 == bdeat_typeCategoryAccessNullableValue(X, accessor));
            ASSERTV(s_loaderFn_object == &obj);
            ASSERTV(&s_loaderFn_int == accessor.d_address);
        }

        if (verbose) cout << "\nTesting Nullable for Decoding" << endl;
        {
            Obj mX; const Obj& X = mX;

            bdeut_NullableValue<int> obj;

            mX.loadNullableForDecoding(
                             &obj,
                             &loaderFn<int>,
                             &nullableValueMaker<bdeut_NullableValue<int> >,
                             &nullableValueFetcher<bdeut_NullableValue<int> >);

            ASSERTV(true == X.isNull());

            mX.makeValue();
            ASSERTV(false == X.isNull());
            ASSERTV(false == obj.isNull());
            ASSERTV(0     == obj.value());

            SimpleManipulator<int> manipulator;
            s_loaderFn_proxy = 0;
            s_loaderFn_object = 0;

            ASSERTV(0 == mX.manipulateNullable(manipulator));
            ASSERTV(&s_loaderFn_int == manipulator.d_address);

            manipulator.reset();
            manipulator.d_rc = 1;
            s_loaderFn_proxy = 0;
            s_loaderFn_object = 0;

            ASSERTV(1 == bdeat_typeCategoryManipulateNullableValue(
                                                                 &mX,
                                                                 manipulator));
            ASSERTV(&s_loaderFn_int == manipulator.d_address);
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING Sequence
        //
        // Concerns
        //: 1 'loadSequence' creates a proxy object for Sequence type.
        //:
        //: 2 The 'sequenceAccessAttribute' and 'sequenceManipulateAttribute'
        //:   methods passes the correct reference to the function pointers
        //:   supplied on load.
        //:
        //: 3 'sequenceAccessAttribute' and 'sequenceManipulateAttribute'
        //:   returns the value returned by the suppliced functor.
        //:
        //: 4 The 'sequenceAccessAttributes' and 'sequenceManipulateAttributes'
        //:   methods passes the correct reference to the function pointers
        //:   supplied on load.
        //:
        //: 5 'sequenceAccessAttributes' and 'sequenceManipulateAttributes'
        //:   returns the value returned by the suppliced functor.
        //:
        //: 6 'sequenceHasAttribute' correctly returns whether the supplied
        //:   selection is valid.
        //:
        //: 8 'object' and 'isNull' returns ths expected result.
        //:
        //: 9 'bdeat' functions are correctly overloaded and can be found
        //:   through ADL.
        //
        // Plan:
        //: 1 Load a Sequence object into a proxy object.
        //:
        //: 2 Verify properties such as 'object', 'isByteArrayValue', etc.
        //:
        //: 3 Invoke 'access*' and 'manipulate*' method with functors that
        //:   extract the address ofthe object passed to the function.  Verify
        //:   the extracted address and the return value.
        //:
        //: 4 Invoke all the 'bdeat' functions related to for Simple type and
        //:   verify the results.
        //
        // Testing:
        //   int sequenceManipulateAttribute(MANIPULATOR& manipulator, int id);
        //   int sequenceManipulateAttribute(MANIPULATOR&, const char*, int;
        //   loadSequence(int, void *, const AInfo *, const char *, ElmtAccssr)
        //   bool sequenceHasAttribute(const char *name, int len) const;
        //   bool sequenceHasAttribute(int attributeId) const;
        //   int sequenceAccessAttribute(ACCESSOR& accessor, int attrId) const;
        //   int sequenceAccessAttributes(ACCESSOR& accessor) const;
        //   int bdeat_sequenceAccessAttributes(const ObjectProxy&, ACCESSOR&)
        //   int bdeat_sequenceAccessAttribute(const ObjProxy&, ACCESSOR&, int)
        //   bdeat_sequenceAccessAttribute(const Proxy&, AC&, const char*, int)
        //   int bdeat_sequenceManipulateAttributes(ObjProxy *, MANIPULATOR&)
        //   bdeat_sequenceManipulateAttribute(ObjProxy *, MANIPULATOR&, int)
        //   bdeat_sequenceManipulateAttribute(Proxy*, MAN&, const char*, int)
        //   bdeat_sequenceHasAttribute(const ObjectProxy&, int)
        //   bdeat_sequenceHasAttribute(const ObjProxy&, const char *, int)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING Sequence" << endl
                          << "================" << endl;

        bdeat_AttributeInfo INFO[] = {
            { 0,   "",         0,  "",         0 },
            { 1,   "A",        1,  "a",        1 },
            { 10,  "AB",       2,  "ab",       0 },
            { 3,   "ABC",      3,  "abc",      2 },
            { 6,   "ABCD",     4,  "abcd",     2 },
            { 5,   "ABCDE",    5,  "abcde",    4 },
            { 4,   "ABCDEF",   6,  "abcdef",   4 },
            { 8,   "ABCDEFG",  7,  "abcdefg",  4 }
        };
        const int NUM_INFO = sizeof INFO / sizeof *INFO;
        const char *CLASSNAME = "foo";

        int obj;

        if (verbose) cout << "\nTesting Sequence for encoding" << endl;
        {
            Obj mX;  const Obj& X = mX;

            mX.loadSequence(NUM_INFO, &obj, INFO, CLASSNAME, &elementLoaderFn);

            ASSERTV(Category::BDEAT_SEQUENCE_CATEGORY == X.category());
            ASSERTV(Category::BDEAT_SEQUENCE_CATEGORY ==
                                                  bdeat_typeCategorySelect(X));
            ASSERTV(&obj == X.object());
            ASSERTV(0 == strcmp(CLASSNAME, X.className()));
            ASSERTV(0 == strcmp(CLASSNAME, bdeat_TypeName_Overloadable
                                               ::bdeat_TypeName_className(X)));
            ASSERTV(false == X.isByteArrayValue());

            for (int ti = 0; ti < NUM_INFO; ++ti) {
                const int   ID     = INFO[ti].d_id;
                const char *NAME   = INFO[ti].d_name_p;
                const int   LENGTH = INFO[ti].d_nameLength;

                SequenceAccessor accessor;
                accessor.d_rc = ti;
                s_elementLoaderFn_proxy = 0;
                s_elementLoaderFn_object = 0;
                s_elementLoaderFn_index = 0;

                ASSERTV(ti == X.sequenceAccessAttribute(accessor, ID));

                ASSERTV(s_elementLoaderFn_proxy == accessor.d_proxy);
                ASSERTV(s_elementLoaderFn_object == &X);
                ASSERTV(s_elementLoaderFn_index == ti);
                ASSERTV(accessor.d_address == &s_elementLoaderFn_int);
                ASSERTV(accessor.d_info == INFO[ti]);

                accessor.reset();
                accessor.d_rc = ti;
                s_elementLoaderFn_proxy = 0;
                s_elementLoaderFn_object = 0;
                s_elementLoaderFn_index = 0;

                ASSERTV(ti == X.sequenceAccessAttribute(accessor,
                                                        NAME,
                                                        LENGTH));

                ASSERTV(s_elementLoaderFn_proxy == accessor.d_proxy);
                ASSERTV(s_elementLoaderFn_object == &X);
                ASSERTV(s_elementLoaderFn_index == ti);
                ASSERTV(accessor.d_address == &s_elementLoaderFn_int);
                ASSERTV(accessor.d_info == INFO[ti]);

                accessor.reset();
                accessor.d_rc = ti;
                s_elementLoaderFn_proxy = 0;
                s_elementLoaderFn_object = 0;
                s_elementLoaderFn_index = 0;

                ASSERTV(ti == bdeat_sequenceAccessAttribute(X, accessor, ID));

                ASSERTV(s_elementLoaderFn_proxy == accessor.d_proxy);
                ASSERTV(s_elementLoaderFn_object == &X);
                ASSERTV(s_elementLoaderFn_index == ti);
                ASSERTV(accessor.d_address == &s_elementLoaderFn_int);
                ASSERTV(accessor.d_info == INFO[ti]);

                accessor.reset();
                accessor.d_rc = ti;
                s_elementLoaderFn_proxy = 0;
                s_elementLoaderFn_object = 0;
                s_elementLoaderFn_index = 0;

                ASSERTV(ti == bdeat_sequenceAccessAttribute(X,
                                                            accessor,
                                                            NAME,
                                                            LENGTH));

                ASSERTV(s_elementLoaderFn_proxy == accessor.d_proxy);
                ASSERTV(s_elementLoaderFn_object == &X);
                ASSERTV(s_elementLoaderFn_index == ti);
                ASSERTV(accessor.d_address == &s_elementLoaderFn_int);
                ASSERTV(accessor.d_info == INFO[ti]);
            }

            SequenceAccessor accessor;
            s_elementLoaderFn_indexes.clear();
            ASSERT(0 == bdeat_sequenceAccessAttributes(X, accessor));
            ASSERT(NUM_INFO == s_elementLoaderFn_indexes.size());
            for (int i = 0; i < s_elementLoaderFn_indexes.size(); ++i) {
                LOOP_ASSERT(i, s_elementLoaderFn_indexes[i] == INFO[i].d_id);
            }

        }

        if (verbose) cout << "\nTesting Sequence for decoding" << endl;
        {
            Obj mX;  const Obj& X = mX;
            mX.loadSequence(NUM_INFO, &obj, INFO, CLASSNAME, &elementLoaderFn);

            ASSERTV(false == X.sequenceHasAttribute(-1));
            ASSERTV(false == X.sequenceHasAttribute(2));
            ASSERTV(false == bdeat_sequenceHasAttribute(X, -1));
            ASSERTV(false == bdeat_sequenceHasAttribute(X, 2));

            ASSERTV(false == X.sequenceHasAttribute("foo", 3));
            ASSERTV(false == bdeat_sequenceHasAttribute(X, "foo", 3));

            for (int ti = 0; ti < NUM_INFO; ++ti) {
                const int   ID     = INFO[ti].d_id;
                const char *NAME   = INFO[ti].d_name_p;
                const int   LENGTH = INFO[ti].d_nameLength;

                ASSERTV(true == X.sequenceHasAttribute(ID));
                ASSERTV(true == X.sequenceHasAttribute(NAME, LENGTH));
                ASSERTV(true == bdeat_sequenceHasAttribute(X, ID));
                ASSERTV(true == bdeat_sequenceHasAttribute(X, NAME, LENGTH));

                SequenceManipulator manipulator;
                manipulator.d_rc = ti;
                s_elementLoaderFn_proxy = 0;
                s_elementLoaderFn_object = 0;
                s_elementLoaderFn_index = 0;

                ASSERTV(ti == mX.sequenceManipulateAttribute(manipulator, ID));

                ASSERTV(s_elementLoaderFn_proxy == manipulator.d_proxy);
                ASSERTV(s_elementLoaderFn_object == &X);
                ASSERTV(s_elementLoaderFn_index == ti);
                ASSERTV(manipulator.d_address == &s_elementLoaderFn_int);
                ASSERTV(manipulator.d_info == INFO[ti]);

                manipulator.reset();
                manipulator.d_rc = ti;
                s_elementLoaderFn_proxy = 0;
                s_elementLoaderFn_object = 0;
                s_elementLoaderFn_index = 0;

                ASSERTV(ti == mX.sequenceManipulateAttribute(manipulator,
                                                             NAME,
                                                             LENGTH));

                ASSERTV(s_elementLoaderFn_proxy == manipulator.d_proxy);
                ASSERTV(s_elementLoaderFn_object == &X);
                ASSERTV(s_elementLoaderFn_index == ti);
                ASSERTV(manipulator.d_address == &s_elementLoaderFn_int);
                ASSERTV(manipulator.d_info == INFO[ti]);

                manipulator.reset();
                manipulator.d_rc = ti;
                s_elementLoaderFn_proxy = 0;
                s_elementLoaderFn_object = 0;
                s_elementLoaderFn_index = 0;

                ASSERTV(ti == bdeat_sequenceManipulateAttribute(&mX,
                                                                manipulator,
                                                                ID));

                ASSERTV(s_elementLoaderFn_proxy == manipulator.d_proxy);
                ASSERTV(s_elementLoaderFn_object == &X);
                ASSERTV(s_elementLoaderFn_index == ti);
                ASSERTV(manipulator.d_address == &s_elementLoaderFn_int);
                ASSERTV(manipulator.d_info == INFO[ti]);

                manipulator.reset();
                manipulator.d_rc = ti;
                s_elementLoaderFn_proxy = 0;
                s_elementLoaderFn_object = 0;
                s_elementLoaderFn_index = 0;

                ASSERTV(ti == bdeat_sequenceManipulateAttribute(&mX,
                                                                manipulator,
                                                                NAME,
                                                                LENGTH));

                ASSERTV(s_elementLoaderFn_proxy == manipulator.d_proxy);
                ASSERTV(s_elementLoaderFn_object == &X);
                ASSERTV(s_elementLoaderFn_index == ti);
                ASSERTV(manipulator.d_address == &s_elementLoaderFn_int);
                ASSERTV(manipulator.d_info == INFO[ti]);
            }

            SequenceManipulator manipulator;
            s_elementLoaderFn_indexes.clear();
            ASSERT(0 == bdeat_sequenceManipulateAttributes(&mX, manipulator));
            ASSERT(NUM_INFO == s_elementLoaderFn_indexes.size());
            for (int i = 0; i < s_elementLoaderFn_indexes.size(); ++i) {
                LOOP_ASSERT(i, s_elementLoaderFn_indexes[i] == INFO[i].d_id);
            }

        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING Choice
        //
        // Concerns:
        //: 1 'loadChoice' creates a proxy object for Choice type.
        //:
        //: 2 The 'choiceAccessSelection' and 'choiceManipulateSelection'
        //:   methods passes the correct reference to the function pointers
        //:   supplied on load.
        //:
        //: 3 'choiceAccessSelection' and 'choiceManipulateSelection'
        //:   returns the value returned by the suppliced functor.
        //:
        //: 4 'choiceHasSelection' correctly returns whether the supplied
        //:   selection is valid.
        //:
        //: 5 'choiceMakeSelection' invokes the chooser function supplied at
        //: 'loadChoice' and supply the correct argument.
        //:
        //: 6 'object' and 'isNull' returns ths expected result.
        //:
        //: 7 'bdeat' functions are correctly overloaded and can be found
        //:   through ADL.
        //
        // Plan:
        //: 1 Load a Choice object into a proxy object.
        //:
        //: 2 Verify properties such as 'object', 'isByteArrayValue', etc.
        //:
        //: 3 Invoke 'access*' and 'manipulate*' method with functors that
        //:   extract the address ofthe object passed to the function.  Verify
        //:   the extracted address and the return value.
        //:
        //: 4 Invoke all the 'bdeat' functions related to for Simple type and
        //:   verify the results.
        //
        // Testing:
        //   int choiceMakeSelection(int selectionId);
        //   int choiceMakeSelection(const char *name, int length);
        //   int choiceManipulateSelection(MANIPULATOR& manipulator);
        //   loadChoice(int, void *, const SInfo *, const char *, Accessor);
        //   loadChoice(int, void *, const SInfo *, AccssrNExtrctr, Chooser);
        //   int selectionId() const;
        //   bool choiceHasSelection(const char *name, int len) const;
        //   bool choiceHasSelection(int selectionId) const;
        //   int choiceAccessSelection(ACCESSOR& accessor) const;
        //   int bdeat_choiceSelectionId(const ObjectProxy& object)
        //   int bdeat_choiceAccessSelection(const ObjectProxy&, ACCESSOR&)
        //   bool bdeat_choiceHasSelection(const ObjProxy&, const char *, int)
        //   bool bdeat_choiceHasSelection(const ObjectProxy&, int)
        //   int bdeat_choiceMakeSelection(ObjectProxy *obj, int selectionId)
        //   int bdeat_choiceMakeSelection(ObjectProxy  *, const char *, int)
        //   int bdeat_choiceManipulateSelection(ObjectProxy *, MANIPULATOR&)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING Choice" << endl
                          << "==============" << endl;

        bdeat_SelectionInfo INFO[] = {
            { 0,   "",         0,  "",         0 },
            { 1,   "A",        1,  "a",        1 },
            { 10,  "AB",       2,  "ab",       0 },
            { 3,   "ABC",      3,  "abc",      2 },
            { 6,   "ABCD",     4,  "abcd",     2 },
            { 5,   "ABCDE",    5,  "abcde",    4 },
            { 4,   "ABCDEF",   6,  "abcdef",   4 },
            { 8,   "ABCDEFG",  7,  "abcdefg",  4 }
        };
        const int NUM_INFO = sizeof INFO / sizeof *INFO;

        if (verbose) cout << "\nTesting Choice for encoding." << endl;
        for (int ti = 0; ti < NUM_INFO; ++ti) {
            const int ID = INFO[ti].d_id;
            const char CLASSNAME[] = "foo";

            Obj mX;  const Obj& X = mX;

            int obj;

            mX.loadChoiceForEncoding(&obj,
                                     &INFO[ti],
                                     CLASSNAME,
                                     &loaderFn<int>);

            ASSERTV(Category::BDEAT_CHOICE_CATEGORY == X.category());
            ASSERTV(Category::BDEAT_CHOICE_CATEGORY ==
                                                  bdeat_typeCategorySelect(X));
            ASSERTV(&obj == X.object());
            ASSERTV(0 == strcmp(CLASSNAME, X.className()));
            ASSERTV(0 == strcmp(CLASSNAME,
                                bdeat_TypeName_Overloadable
                                               ::bdeat_TypeName_className(X)));
            ASSERTV(false == X.isByteArrayValue());

            ASSERTV(ID == X.selectionId());
            ASSERTV(ID == bdeat_choiceSelectionId(X));

            ChoiceAccessor accessor;
            accessor.d_rc = ti;
            s_loaderFn_proxy = 0;
            s_loaderFn_object = 0;

            ASSERTV(ti == X.choiceAccessSelection(accessor));
            ASSERTV(s_loaderFn_proxy == accessor.d_proxy);
            ASSERTV(s_loaderFn_object == &obj);
            ASSERTV(accessor.d_address == &s_loaderFn_int);
            ASSERTV(accessor.d_info == INFO[ti]);

            s_loaderFn_proxy = 0;
            s_loaderFn_object = 0;

            ASSERTV(ti == bdeat_choiceAccessSelection(X, accessor));
            ASSERTV(s_loaderFn_proxy == accessor.d_proxy);
            ASSERTV(s_loaderFn_object == &obj);
            ASSERTV(accessor.d_address == &s_loaderFn_int);
            ASSERTV(accessor.d_info == INFO[ti]);
        }

        if (verbose) cout << "\nTesting Choice for decoding." << endl;
        {
            Obj mX;  const Obj& X = mX;

            int obj;
            mX.loadChoiceForDecoding(NUM_INFO,
                                     &obj,
                                     INFO,
                                     &selectionLoaderFn,
                                     &chooserFn);

            ASSERTV(Category::BDEAT_CHOICE_CATEGORY == X.category());
            ASSERTV(Category::BDEAT_CHOICE_CATEGORY ==
                                                  bdeat_typeCategorySelect(X));
            ASSERTV(&obj == X.object());
            ASSERTV(0 == X.className());
            ASSERTV(0 ==
                    bdeat_TypeName_Overloadable::bdeat_TypeName_className(X));
            ASSERTV(false == X.isByteArrayValue());

            ASSERTV(false == X.choiceHasSelection(-1));
            ASSERTV(false == X.choiceHasSelection(2));
            ASSERTV(false == bdeat_choiceHasSelection(X, -1));
            ASSERTV(false == bdeat_choiceHasSelection(X, 2));

            ASSERTV(false == X.choiceHasSelection("foo", 3));
            ASSERTV(false == bdeat_choiceHasSelection(X, "foo", 3));

            for (int ti = 0; ti < NUM_INFO; ++ti) {
                const int   ID     = INFO[ti].d_id;
                const char *NAME   = INFO[ti].d_name_p;
                const int   LENGTH = INFO[ti].d_nameLength;

                s_selectionLoaderFn_selectInfoPtr = &INFO[ti];

                ASSERTV(true == X.choiceHasSelection(ID));
                ASSERTV(true == X.choiceHasSelection(NAME, LENGTH));
                ASSERTV(true == bdeat_choiceHasSelection(X, ID));
                ASSERTV(true == bdeat_choiceHasSelection(X, NAME, LENGTH));

                s_chooserFn_rc = ti;
                ASSERTV(ti   == mX.choiceMakeSelection(ID));
                ASSERTV(ID   == s_chooserFn_id);
                ASSERTV(&obj == s_chooserFn_object);
                ASSERTV(ti   == bdeat_choiceMakeSelection(&mX, ID));
                ASSERTV(ID   == s_chooserFn_id);
                ASSERTV(&obj == s_chooserFn_object);

                ChoiceManipulator manipulator;
                ASSERTV(0 == mX.choiceManipulateSelection(manipulator));

                ASSERTV(s_selectionLoaderFn_proxy == manipulator.d_proxy);
                ASSERTV(s_selectionLoaderFn_object == &obj);

                ASSERTV(manipulator.d_address = &s_selectionLoaderFn_int);
                ASSERTV(manipulator.d_info == INFO[ti]);

                manipulator.d_rc = ti;
                ASSERTV(ti == mX.choiceManipulateSelection(manipulator));

                ASSERTV(ID == X.selectionId());
                ASSERTV(ID == bdeat_choiceSelectionId(X));
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING Array
        //
        // Concerns:
        //: 1 'loadArray' creates a proxy object for array type.
        //:
        //: 2 The 'arrayAccessElement' and 'arrayManipulateElement' methods
        //:   passes the correct reference to the function pointers supplied on
        //:   load.
        //:
        //: 3 'resize' invoke the resizer funciton supplied on load and pass in
        //:   the correct argument.
        //:
        //: 4 Invoking 'loadArray' without an accessor function creates a byte
        //:   array.
        //:
        //: 5 'bdeat_typeCategoryAccessArray' and
        //:   'bdeat_typeCategoryManipulateArray' passed the correct reference
        //:   for both byte array and normal array.
        //:
        //: 6 'object' and 'isNull' returns ths expected result.
        //:
        //: 7 'bdeat' functions are correctly overloaded and can be found
        //:   through ADL.
        //
        // Plan:
        //: 1 Load an Array object into a proxy object.
        //:
        //: 2 Verify properties such as 'object', 'isByteArrayValue', etc.
        //:
        //: 3 Invoke 'access*' and 'manipulate*' method with functors that
        //:   extract the address ofthe object passed to the function.  Verify
        //:   the extracted address and the return value.
        //:
        //: 4 Invoke all the 'bdeat' functions related to for Simple type and
        //:   verify the results.
        //
        // Testing:
        //   void resize(size_t newSize);
        //   int arrayManipulateElement(MANIPULATOR& manipulator, int index);
        //   loadArray(int length, int elementSize, void *begin, Accessor a);
        //   loadArray(void *, int, int, void *, Resizer, Accessor);
        //   bsl::size_t size() const;
        //   bool isByteArrayValue() const;
        //   int arrayAccessElement(ACCESSOR& accessor, int index) const;
        //   bsl::size_t bdeat_arraySize(const ObjectProxy& object)
        //   void bdeat_arrayResize(ObjectProxy* object, int newSize)
        //   int bdeat_arrayAccessElement(const ObjectProxy&, ACCESSOR&, int)
        //   int bdeat_arrayManipulateElement(ObjectProxy*, MANIPULATOR&, int)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING Array" << endl
                          << "=============" << endl;

        bsl::vector<int> obj;
        const int SIZE = 5;
        obj.resize(SIZE);
        for (int i = 0; i < SIZE; ++i) {
            obj[i] = i;
        }

        if (verbose) cout << "\nTesting Array for encoding." << endl;
        {
            Obj mX; const Obj& X = mX;

            mX.loadArrayForEncoding(obj.size(),
                                    sizeof(int),
                                    obj.data(),
                                    &loaderFn<int>);

            ASSERTV(Category::BDEAT_ARRAY_CATEGORY == X.category());
            ASSERTV(Category::BDEAT_ARRAY_CATEGORY ==
                                                  bdeat_typeCategorySelect(X));
            ASSERTV(obj.data() == X.object());
            ASSERTV(0 == X.className());
            ASSERTV(0 ==
                    bdeat_TypeName_Overloadable::bdeat_TypeName_className(X));
            ASSERTV(false == X.isByteArrayValue());

            ASSERTV(SIZE  == (int)X.size());
            ASSERTV(SIZE  == (int)bdeat_arraySize(X));

            for (int i = 0; i < (int)obj.size(); ++i) {
                s_loaderFn_object = 0;
                s_loaderFn_proxy  = 0;

                ArrayElementAccessor<int> accessor;
                ASSERTV(0 == mX.arrayAccessElement(accessor, i));
                ASSERTV(&obj[i] == s_loaderFn_object);
                ASSERTV(s_loaderFn_proxy == accessor.d_proxy);
                ASSERTV(&s_loaderFn_int  == accessor.d_address);

                accessor.reset();
                s_loaderFn_object = 0;
                s_loaderFn_proxy  = 0;

                ASSERTV(0 == bdeat_arrayAccessElement(X, accessor, i));
                ASSERTV(&obj[i] == s_loaderFn_object);
                ASSERTV(s_loaderFn_proxy == accessor.d_proxy);
                ASSERTV(&s_loaderFn_int  == accessor.d_address);
            }

            ArrayAccessor accessor;
            ASSERTV(0          == bdeat_typeCategoryAccessArray(X, accessor));
            ASSERTV(obj.data() == accessor.d_address);
        }

        if (verbose) cout << "\nTesting Array for decoding." << endl;
        {
            Obj mX; const Obj& X = mX;

            mX.loadArrayForDecoding(&obj,
                                    obj.size(),
                                    sizeof(int),
                                    obj.data(),
                                    &resizerFn<int>,
                                    &loaderFn<int>);

            ASSERTV(Category::BDEAT_ARRAY_CATEGORY == X.category());
            ASSERTV(Category::BDEAT_ARRAY_CATEGORY ==
                                                  bdeat_typeCategorySelect(X));
            ASSERTV(&obj == X.object());
            ASSERTV(0 == X.className());
            ASSERTV(0 ==
                    bdeat_TypeName_Overloadable::bdeat_TypeName_className(X));
            ASSERTV(false == X.isByteArrayValue());

            ASSERTV(SIZE == (int)X.size());
            ASSERTV(SIZE == (int)bdeat_arraySize(X));

            for (int i = 0; i < (int)obj.size(); ++i) {
                ArrayElementManipulator<int> manipulator;
                s_loaderFn_object = 0;
                s_loaderFn_proxy  = 0;

                ASSERTV(0       == mX.arrayManipulateElement(manipulator, i));
                ASSERTV(&obj[i] == s_loaderFn_object);
                ASSERTV(s_loaderFn_proxy == manipulator.d_proxy);
                ASSERTV(&s_loaderFn_int  == manipulator.d_address);

                manipulator.reset();
                s_loaderFn_object = 0;
                s_loaderFn_proxy  = 0;

                ASSERTV(0 ==
                            bdeat_arrayManipulateElement(&mX, manipulator, i));
                ASSERTV(&obj[i] == s_loaderFn_object);
                ASSERTV(s_loaderFn_proxy == manipulator.d_proxy);
                ASSERTV(&s_loaderFn_int  == manipulator.d_address);
            }

            ArrayManipulator manipulator;
            ASSERTV(0  == bdeat_typeCategoryManipulateArray(&mX, manipulator));
            ASSERTV(&obj == manipulator.d_address);

            s_resizerFn_object = 0;
            s_resizerFn_newSize = -1;
            s_resizerFn_begin = 0;

            mX.resize(0);
            ASSERTV(&obj       ==  s_resizerFn_object);
            ASSERTV(0          ==  s_resizerFn_newSize);
            ASSERTV(obj.data() == *s_resizerFn_begin);

            s_resizerFn_object = 0;
            s_resizerFn_newSize = -1;
            s_resizerFn_begin = 0;

            mX.resize(1);
            ASSERTV(&obj       ==  s_resizerFn_object);
            ASSERTV(1          ==  s_resizerFn_newSize);
            ASSERTV(obj.data() == *s_resizerFn_begin);

            s_resizerFn_object = 0;
            s_resizerFn_newSize = -1;
            s_resizerFn_begin = 0;

            bdeat_arrayResize(&mX, 0);
            ASSERTV(&obj       ==  s_resizerFn_object);
            ASSERTV(0          ==  s_resizerFn_newSize);
            ASSERTV(obj.data() == *s_resizerFn_begin);

            s_resizerFn_object = 0;
            s_resizerFn_newSize = -1;
            s_resizerFn_begin = 0;

            bdeat_arrayResize(&mX, 1);
            ASSERTV(&obj       ==  s_resizerFn_object);
            ASSERTV(1          ==  s_resizerFn_newSize);
            ASSERTV(obj.data() == *s_resizerFn_begin);
        }

        if (verbose) cout << "Testing byte array" << endl;
        {
            Obj mX; const Obj& X = mX;

            bsl::vector<char> obj;
            obj.push_back(0);
            obj.push_back(1);
            obj.push_back(2);
            obj.push_back(3);
            obj.push_back(4);

            // Load 'char' vector.  A byte array is the same for both encoding
            // and decoding.

            mX.loadByteArray(&obj);

            ASSERTV(Category::BDEAT_ARRAY_CATEGORY == X.category());
            ASSERTV(Category::BDEAT_ARRAY_CATEGORY ==
                                                  bdeat_typeCategorySelect(X));
            ASSERTV(&obj == X.object());
            ASSERTV(0 == X.className());
            ASSERTV(0 ==
                    bdeat_TypeName_Overloadable::bdeat_TypeName_className(X));

            ASSERTV(true == X.isByteArrayValue());

            ArrayAccessor accessor;
            ASSERTV(0 == bdeat_typeCategoryAccessArray(X, accessor));
            ASSERTV(&obj == accessor.d_address);

            ArrayManipulator manipulator;
            ASSERTV(0 == bdeat_typeCategoryManipulateArray(&mX, manipulator));
            ASSERTV(&obj == manipulator.d_address);
        }

      }  break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING Enum
        //
        // Concerns:
        //: 1 'loadEnum' creates a proxy object for enumerated type.
        //:
        //: 2 'enumToInt' and 'enumToString' returns the enumerated value.
        //:
        //: 3 The 'enumFromInt' and 'enumFromString' methods passes the correct
        //:   reference to the function pointers supplied on load.
        //:
        //: 4 'enumFromInt' and 'enumFromString' returns the value returned
        //:   by the suppliced functor.
        //:
        //: 5 'object' and 'isNull' returns ths expected result.
        //:
        //: 6 'bdeat' functions are correctly overloaded and can be found
        //:   through ADL.
        //
        // Plan:
        //: 1 Load an Enumerated object into a proxy object.
        //:
        //: 2 Verify properties such as 'object', 'isByteArrayValue', etc.
        //:
        //: 3 Invoke 'access*' and 'manipulate*' method with functors that
        //:   extract the address ofthe object passed to the function.  Verify
        //:   the extracted address and the return value.
        //:
        //: 4 Invoke all the 'bdeat' functions related to for Simple type and
        //:   verify the results.
        //
        // Testing:
        //   int enumFromInt(int value);
        //   int enumFromString(const char *stringValue, int stringLength);
        //   loadEnumeration(int v, const EnumeratorInfo *a, int s);
        //   loadEnumeration(void *, IntSetter, StrSetter, const EInfo *, int);
        //   int enumToInt() const;
        //   const char* enumToString() const;
        //   void bdeat_enumToInt(int *, const ObjectProxy& object)
        //   int bdeat_enumFromInt(ObjectProxy *result, int value)
        //   void bdeat_enumToString(bsl::string *, const ObjectProxy& object)
        //   int bdeat_enumFromString(ObjectProxy *, const char *str, int len)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING Enumeration" << endl
                          << "===================" << endl;

        const bdeat_EnumeratorInfo INFO[] = {
            { 0, "", 0, "" },
            { 1, "ENUM1", 5, "annotation for enum1" },
            { 2, "ENUM2", 5, "annotation for enum2" },
            { 5, "ENUMERATION5", 12, "" }
        };
        const int INFO_SIZE = sizeof INFO / sizeof *INFO;

        if (verbose) cout << "Enumeration accessor" << endl;
        for (int ti = 0; ti < INFO_SIZE; ++ti) {
            const int   VALUE = INFO[ti].d_value;
            const char *NAME  = INFO[ti].d_name_p;

            Obj mX; const Obj& X = mX;

            mX.loadEnumerationForEncoding(VALUE, INFO, INFO_SIZE);

            ASSERTV(Category::BDEAT_ENUMERATION_CATEGORY == X.category());
            ASSERTV(Category::BDEAT_ENUMERATION_CATEGORY ==
                                                  bdeat_typeCategorySelect(X));
            ASSERTV(0 == X.object());
            ASSERTV(0 == X.className());
            ASSERTV(0 ==
                    bdeat_TypeName_Overloadable::bdeat_TypeName_className(X));
            ASSERTV(false == X.isByteArrayValue());

            ASSERTV(VALUE == X.enumToInt());
            ASSERTV(0     == strcmp(NAME, X.enumToString()));

            int intResult;
            bdeat_enumToInt(&intResult, X);
            ASSERTV(VALUE == intResult);

            bsl::string strResult;
            bdeat_enumToString(&strResult, X);
            ASSERTV(NAME == strResult);
        }

        if (verbose) cout << "Enumeration manipulator" << endl;
        {
            Obj mX; const Obj& X = mX;

            int obj;
            mX.loadEnumerationForDecoding(&obj,
                                          &intSetterFn,
                                          &stringSetterFn,
                                          INFO,
                                          INFO_SIZE);

            ASSERTV(Category::BDEAT_ENUMERATION_CATEGORY == X.category());
            ASSERTV(Category::BDEAT_ENUMERATION_CATEGORY ==
                                                  bdeat_typeCategorySelect(X));
            ASSERTV(&obj == X.object());
            ASSERTV(0 == X.className());
            ASSERTV(0 ==
                    bdeat_TypeName_Overloadable::bdeat_TypeName_className(X));
            ASSERTV(false == X.isByteArrayValue());

            for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                for (int ti = 0; ti < INFO_SIZE; ++ti) {
                    const int         VALUE  = INFO[ti].d_value;
                    const char *const NAME   = INFO[ti].d_name_p;
                    const int         LENGTH = INFO[ti].d_nameLength;

                    s_intSetterFn_object    = 0;
                    s_intSetterFn_value     = 0;
                    s_intSetterFn_rc        = ti;
                    s_stringSetterFn_object = 0;
                    s_stringSetterFn_value  = 0;
                    s_stringSetterFn_rc     = ti;

                    switch (cfg) {
                      case 'a': {
                        ASSERTV(ti == mX.enumFromInt(VALUE));
                        ASSERTV(&obj  == s_intSetterFn_object);
                        ASSERTV(VALUE == s_intSetterFn_value);
                      } break;
                      case 'b': {
                        ASSERTV(ti == mX.enumFromString(NAME, LENGTH));
                        ASSERTV(&obj   == s_stringSetterFn_object);
                        ASSERTV(NAME   == s_stringSetterFn_value);
                        ASSERTV(LENGTH == s_stringSetterFn_length);
                      } break;
                      case 'c': {
                        ASSERTV(ti == bdeat_enumFromInt(&mX, VALUE));
                        ASSERTV(&obj  == s_intSetterFn_object);
                        ASSERTV(VALUE == s_intSetterFn_value);
                      } break;
                      case 'd': {
                        ASSERTV(ti == bdeat_enumFromString(&mX, NAME, LENGTH));
                        ASSERTV(&obj   == s_stringSetterFn_object);
                        ASSERTV(NAME   == s_stringSetterFn_value);
                        ASSERTV(LENGTH == s_stringSetterFn_length);
                      } break;
                    }
                }
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING Simple Pointer
        //
        // Concerns:
        //: 1 'loadSimple' works for all Simple type.
        //:
        //: 2 'accessSimple' and 'manipulateSimple' passes the correct
        //:   reference to the supplied functor.
        //:
        //: 3 'accessSimple' and 'manipulateSimple' returns the value returned
        //:   by the suppliced functor.
        //:
        //: 4 'object', 'isByteArrayValue', and 'className' returns ths
        //:   expected result.
        //:
        //: 5 'bdeat' functions are correctly overloaded and can be found
        //:   through ADL.
        //
        // Plan:
        //: 1 Load a Simple object into a proxy object.
        //:
        //: 2 Verify properties such as 'object', 'isByteArrayValue', etc.
        //:
        //: 3 Invoke 'access*' and 'manipulate*' method with functors that
        //:   extract the address ofthe object passed to the function.  Verify
        //:   the extracted address and the return value.
        //:
        //: 4 Invoke all the 'bdeat' functions related to for Simple type and
        //:   verify the results.
        //
        // Testing:
        //   int manipulateSimple(MANIPULATOR& manipulator);
        //   void loadSimple(char               *value);
        //   void loadSimple(unsigned char      *value);
        //   void loadSimple(short              *value);
        //   void loadSimple(int                *value);
        //   void loadSimple(bsls_Types::Int64  *value);
        //   void loadSimple(unsigned short     *value);
        //   void loadSimple(unsigned int       *value);
        //   void loadSimple(bsls_Types::Uint64 *value);
        //   void loadSimple(float              *value);
        //   void loadSimple(double             *value);
        //   void loadSimple(bsl::string        *value);
        //   void loadSimple(bdet_Datetime      *value);
        //   void loadSimple(bdet_Date          *value);
        //   void loadSimple(bdet_Time          *value);
        //   void loadSimple(bool               *value);
        //   void loadSimple(bdet_DatetimeTz    *value);
        //   void loadSimple(bdet_DateTz        *value);
        //   void loadSimple(bdet_TimeTz        *value);
        //   int accessSimple(ACCESSOR& accessor) const;
        //   int bdeat_typeCategoryAccessSimple(const ObjProxy&, ACCESSOR& a);
        //   int bdeat_typeCategoryManipulateSimple(ObjProxy *, MANIPULATOR&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING Simple Pointer" << endl
                          << "======================" << endl;

        executeSimpleCategoryTest<char>("char");
        executeSimpleCategoryTest<unsigned char>("uchar");
        executeSimpleCategoryTest<short>("short");
        executeSimpleCategoryTest<int>("int");
        executeSimpleCategoryTest<bsls_Types::Uint64>("uint64");
        executeSimpleCategoryTest<float>("float");
        executeSimpleCategoryTest<double>("double");
        executeSimpleCategoryTest<bsl::string>("string");
        executeSimpleCategoryTest<bdet_Datetime>("datetime");
        executeSimpleCategoryTest<bdet_Date>("date");
        executeSimpleCategoryTest<bdet_Time>("time");
        executeSimpleCategoryTest<bool>("bool");
        executeSimpleCategoryTest<bdet_DatetimeTz>("datetimetz");
        executeSimpleCategoryTest<bdet_DateTz>("datetz");
        executeSimpleCategoryTest<bdet_TimeTz>("timetz");

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING Constructor
        //
        // Test that baea_SerializableObjectProxy is initialized to a valid
        // empty state.
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing constructor" << endl
                 << "===================" << endl;
        }

        Obj mX; const Obj& X = mX;

        ASSERTV(0 == X.className());
        ASSERTV(Category::BDEAT_DYNAMIC_CATEGORY == X.category());
        ASSERTV(Category::BDEAT_DYNAMIC_CATEGORY ==
                bdeat_TypeCategoryFunctions::bdeat_typeCategorySelect(X));

      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
