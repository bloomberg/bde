// baea_serializableobjectproxy.t.cpp   -*-C++-*-
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

#include <bsls_assert.h>

#include <iomanip>
#include <limits>

using namespace BloombergLP;
using namespace BloombergLP::baea;
using namespace BloombergLP::bdeat_TypeCategoryFunctions;

using bsl::cout;
using bsl::endl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// CREATORS
// [ 1] SerializableObjectProxy;
//
// NO-OP FUNCTIONS FOR INTEGRATION
// [  ] void reset();
// [  ] int maxSupportedBdexVersion() const;
// [  ] STREAM& bdexStreamIn(STREAM& stream, int);
// [  ] STREAM& bdexStreamOut(STREAM& stream, int) const;
//
// MANIPULATORS
// [ 6] void resize(size_t newSize);
// [ 7] int choiceMakeSelection(int selectionId);
// [ 7] int choiceMakeSelection(const char *name, int length);
// [ 7] int choiceManipulateSelection(MANIPULATOR& manipulator);
// [ 6] int arrayManipulateElement(MANIPULATOR& manipulator, int index);
// [ 8] int sequenceManipulateAttribute(MANIPULATOR& manipulator, int id);
// [ 8] int sequenceManipulateAttribute(MANIPULATOR& m, const char*  n, int l);
// [ 4] int manipulateSimple(MANIPULATOR& manipulator);
// [ 9] int manipulateNullable(MANIPULATOR& manipulator);
// [ 5] int enumFromInt(int value);
// [ 5] int enumFromString(const char *stringValue, int stringLength);
// [ 9] void makeValue();
// [ 4] void loadSimple(char               *value);
// [ 4] void loadSimple(unsigned char      *value);
// [ 4] void loadSimple(short              *value);
// [ 4] void loadSimple(int                *value);
// [ 4] void loadSimple(bsls_Types::Int64  *value);
// [ 4] void loadSimple(unsigned short     *value);
// [ 4] void loadSimple(unsigned int       *value);
// [ 4] void loadSimple(bsls_Types::Uint64 *value);
// [ 4] void loadSimple(float              *value);
// [ 4] void loadSimple(double             *value);
// [ 4] void loadSimple(bsl::string        *value);
// [ 4] void loadSimple(bdet_Datetime      *value);
// [ 4] void loadSimple(bdet_Date          *value);
// [ 4] void loadSimple(bdet_Time          *value);
// [ 4] void loadSimple(bool               *value);
// [ 4] void loadSimple(bdet_DatetimeTz    *value);
// [ 4] void loadSimple(bdet_DateTz        *value);
// [ 4] void loadSimple(bdet_TimeTz        *value);
// [ 7] loadChoice(int, void *, const SInfo *, const char *, Accessor);
// [ 7] loadChoice(int, void *, const SInfo *, AccssrNExtrctr, Chooser);
// [ 8] loadSequence(int, void *, const AInfo *, const char *, ElmtAccssr);
// [ 6] loadArray(int length, int elementSize, void *begin, Accessor a);
// [ 6] loadArray(void *, int, int, void *, Resizer, Accessor);
// [ 9] loadNullable(void *object, Accessor accessor);
// [ 9] loadNullable(void *obj, Accessor, Manipulator, ObjectFetcher);
// [ 5] loadEnumeration(int v, const EnumeratorInfo *a, int s);
// [ 5] loadEnumeration(void *, IntSetter, StrSetter, const EInfo *, int);
//
// ACCESSORS
// [11] const char *className() const;
// [11] bdeat_TypeCategory::Value category() const;
// [ 7] int selectionId() const;
// [ 6] bsl::size_t size() const;
// [11] const void* object() const;
// [ 6] bool isByteArrayValue() const;
// [ 9] bool isNull() const;
// [11] bool isValidForEncoding() const;
// [11] bool isValidForDecoding() const;
// [ 7] bool choiceHasSelection(const char *name, int len) const;
// [ 7] bool choiceHasSelection(int selectionId) const;
// [ 8] bool sequenceHasAttribute(const char *name, int len) const;
// [ 8] bool sequenceHasAttribute(int attributeId) const;
// [ 7] int choiceAccessSelection(ACCESSOR& accessor) const;
// [ 8] int sequenceAccessAttribute(ACCESSOR& accessor, int attrId) const;
// [ 8] int sequenceAccessAttributes(ACCESSOR& accessor) const;
// [ 9] int accessNullable(ACCESSOR& accessor) const;
// [ 4] int accessSimple(ACCESSOR& accessor) const;
// [ 6] int arrayAccessElement(ACCESSOR& accessor, int index) const;
// [ 5] int enumToInt() const;
// [ 5] const char* enumToString() const;

// 'bdeat_typecategory' OVERLOADS
// [11] Value bdeat_typeCategorySelect(const ObjectProxy& obj);
// [ 4] int bdeat_typeCategoryAccessSimple(const ObjProxy&, ACCESSOR& a);
// [ 4] int bdeat_typeCategoryManipulateSimple(ObjProxy *, MANIPULATOR&);
// [ 6] int bdeat_typeCategoryManipulateArray(ObjProxy *, MANIPULATOR&);
// [ 6] int bdeat_typeCategoryAccessArray(const ObjectProxy&, ACCESSOR&);
// [ 9] int bdeat_typeCategoryAccessNullableValue(const ObjProxy&, ACC&);
// [ 9] int bdeat_typeCategoryManipulateNullableValue(ObjProxy*, MANIP&);
//
// 'bdeat_EnumFunctions' OVERLOADS
//
// [ 5] void bdeat_enumToInt(int *, const ObjectProxy& object)
// [ 5] int bdeat_enumFromInt(ObjectProxy *result, int value)
// [ 5] void bdeat_enumToString(bsl::string *, const ObjectProxy& object)
// [ 5] int bdeat_enumFromString(ObjectProxy *, const char *str, int len)
//
// 'bdeat_arrayfunctions' OVERLOADS AND SPECIALIZATIONS
// [ 6] bsl::size_t bdeat_arraySize(const ObjectProxy& object)
// [ 6] void bdeat_arrayResize(ObjectProxy* object, int newSize)
// [ 6] int bdeat_arrayAccessElement(const ObjectProxy&, ACCESSOR&, int)
// [ 6] int bdeat_arrayManipulateElement(ObjectProxy*, MANIPULATOR&, int)
//
// 'bdeat_sequencefunctions' OVERLOADS AND SPECIALIZATIONS
// [  ] int bdeat_sequenceAccessAttributes(const ObjectProxy&, ACCESSOR&)
// [  ] int bdeat_sequenceAccessAttribute(const ObjProxy&, ACCESSOR&, int)
// [  ] bdeat_sequenceManipulateAttribute(ObjProxy *, MANIPULATOR&, int)
// [  ] bdeat_sequenceManipulateAttribute(ObjProxy *, MANIP&, const char*, int)
// [  ] bdeat_sequenceHasAttribute(const ObjectProxy&, int)
// [  ] bdeat_sequenceHasAttribute(const ObjProxy&, const char *, int)
//
// 'bdeat_nullablevaluefunctions' OVERLOADS AND SPECIALIZATIONS
// [10] bool bdeat_nullableValueIsNull(const NullableAdapter& object)
// [10] void bdeat_nullableValueMakeValue(NullableAdapter *object)
// [10] int bdeat_nullableValueManipulateValue(Nullable*, MANIPULATOR&)
// [10] int bdeat_nullableValueAccessValue(const Nullable&, ACCESSOR&)
//
// 'bdeat_typename' overloads
// [11] const char *bdeat_TypeName_className(const ObjectProxy& object)
//
// 'bdeat_choicefunctions' overloads and specializations
// [  ] int bdeat_choiceSelectionId(const ObjectProxy& object)
// [  ] int bdeat_choiceAccessSelection(const ObjectProxy&, ACCESSOR&)
// [  ] bool bdeat_choiceHasSelection(const ObjProxy&, const char *, int)
// [  ] bool bdeat_choiceHasSelection(const ObjectProxy&, int)
// [  ] int bdeat_choiceMakeSelection(ObjectProxy *obj, int selectionId)
// [  ] int bdeat_choiceMakeSelection(ObjectProxy  *, const char *, int)
// [  ] int bdeat_choiceManipulateSelection(ObjectProxy *, MANIPULATOR&)

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

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

typedef baea::SerializableObjectProxy Obj;
typedef bdeat_TypeCategory            Category;

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

    int operator()(const SerializableObjectProxy& object)
    {
        d_address = object.object();
        return 0;
    }

    template <class TYPE>
    int operator() (const TYPE&)
    {
        // needed to compile due to nullable adapter, but should not be called
        ASSERT(!"Should be unreachable");
        return -1;
    }
};

struct ExtractAddressManipulator {
    const void *d_address;

    ExtractAddressManipulator() : d_address(0) {};

    int operator()(SerializableObjectProxy *object)
    {
        d_address = object->object();
        return 0;
    }

    template <class TYPE>
    int operator() (TYPE *)
    {
        // needed to compile due to nullable adapter, but should not be called
        ASSERT(!"Should be unreachable");
        return -1;
    }
};

struct SimpleAccessor {
    const void *d_address;
    int         d_rc;

    SimpleAccessor() : d_address(0), d_rc(0) {};

    template<typename TYPE>
    int operator()(const TYPE& object, bdeat_TypeCategory::Simple)
    {
        d_address = &object;
        return d_rc;
    }

    template<typename TYPE, typename CATEGORY>
    int operator()(const TYPE& object, const CATEGORY&)
    {
        // This is needed to compile for use with nullable proxy, but should
        // not be called.

        ASSERT(!"Should be unreachable");
        return -1;
    }
};

template <class TYPE>
struct SimpleManipulator {
    void *d_address;
    int  d_rc;

    SimpleManipulator() : d_address(0), d_rc(0) {};

    int operator()(TYPE *value, bdeat_TypeCategory::Simple)
    {
        d_address = value;
        return d_rc;
    }

    template <class OTHER_TYPE, class OTHER_CATEGORY>
    int operator()(OTHER_TYPE *, const OTHER_CATEGORY&)
    {
        // needed to compile due to nullable adapter, but should not be called
        ASSERT(!"Should be unreachable");
        return -1;
    }
};

template <class TYPE>
struct ArrayElementAccessor {
    const SerializableObjectProxy *d_proxy;
    const void                    *d_address;
    int                            d_rc;

    ArrayElementAccessor() : d_address(0), d_rc(0) {}

    int operator() (const SerializableObjectProxy& object)
    {
        d_proxy = &object;

        SimpleAccessor accessor;
        object.accessSimple(accessor);
        d_address = accessor.d_address;

        return d_rc;
    }

    int operator() (const SerializableObjectProxy_NullableAdapter&)
    {
        // needed to compile due to nullable adapter, but should not be called
        ASSERT(!"Should be unreachable");
        return -1;
    }
};

template <class TYPE>
struct ArrayAccessor {
    const void                    *d_address;
    int                            d_rc;

    ArrayAccessor() : d_address(0), d_rc(0) {}

    int operator() (const SerializableObjectProxy& object,
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
    const SerializableObjectProxy *d_proxy;
    void                          *d_address;
    int                            d_rc;

    ArrayElementManipulator() : d_address(0), d_rc(0) {}

    int operator() (SerializableObjectProxy *object)
    {
        d_proxy = object;

        SimpleManipulator<TYPE> manipulator;
        object->manipulateSimple(manipulator);
        d_address = manipulator.d_address;
        return d_rc;
    }

    int operator() (SerializableObjectProxy_NullableAdapter *)
    {
        // needed to compile due to nullable adapter, but should not be called
        ASSERT(!"Should be unreachable");
        return -1;
    }
};

template <class TYPE>
struct ArrayManipulator {
    const void *d_address;
    int         d_rc;

    ArrayManipulator() : d_address(0), d_rc(0) {}

    int operator() (SerializableObjectProxy *object,
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
    const SerializableObjectProxy *d_proxy;
    const void                    *d_address;
    bdeat_AttributeInfo            d_info;
    int                            d_rc;

    SequenceAccessor() : d_proxy(0), d_address(0), d_rc(0) {}

    int operator() (const SerializableObjectProxy& object,
                    const bdeat_AttributeInfo&     info)
    {
        d_proxy = &object;
        SimpleAccessor extractor;
        object.accessSimple(extractor);

        d_address = extractor.d_address;
        d_info = info;
        return d_rc;
    }

    template <typename TYPE>
    int operator() (const TYPE&, const bdeat_AttributeInfo&)
    {
        // needed to compile due to nullable adapter, but should not be called

        ASSERT(!"Should be unreachable");
        return -1;
    }
};

struct ChoiceAccessor {
    const SerializableObjectProxy *d_proxy;
    const void                    *d_address;
    bdeat_SelectionInfo            d_info;
    int                            d_rc;

    ChoiceAccessor() : d_proxy(0), d_address(0), d_rc(0) {}

    int operator() (const SerializableObjectProxy& object,
                    const bdeat_SelectionInfo&     info)
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

        ASSERT(!"Should be unreachable");
        return -1;
    }
};

struct ChoiceManipulator
{
    SerializableObjectProxy *d_proxy;
    const void              *d_address;
    bdeat_SelectionInfo      d_info;
    int                      d_rc;

    ChoiceManipulator() : d_proxy(0), d_address(0), d_rc(0) {}

    int operator() (SerializableObjectProxy* object,
                    const bdeat_SelectionInfo& info)
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
        ASSERT(!"Should be unreachable");
        return -1;
    }
};

SerializableObjectProxy *elementAccessorFn_proxy;
const void              *elementAccessorFn_object;
int                      elementAccessorFn_int;
int                      elementAccessorFn_index;
void elementAccessorFn(SerializableObjectProxy       *proxy,
                       const SerializableObjectProxy &object,
                       int                            index)
{
    elementAccessorFn_proxy = proxy;
    elementAccessorFn_object = &object;
    elementAccessorFn_index = index;
    proxy->loadSimple(&elementAccessorFn_int);
}

SerializableObjectProxy *s_accessorFn_proxy;
void                    *s_accessorFn_object;
int                      s_accessorFn_int;
template<class TYPE>
void accessorFn(SerializableObjectProxy *proxy, void* object)
{
    s_accessorFn_proxy = proxy;
    s_accessorFn_object = object;
    proxy->loadSimple(&s_accessorFn_int);
}

SerializableObjectProxy   *s_accessorAndExtractorFn_proxy;
void                      *s_accessorAndExtractorFn_object;
const bdeat_SelectionInfo *s_accessorAndExtractorFn_selectInfoPtr;
int                        s_accessorAndExtractorFn_int;
void accessorAndExtractorFn(SerializableObjectProxy *proxy,
                                  void* object,
                                  const bdeat_SelectionInfo **selectInfoPtr)
{
    s_accessorAndExtractorFn_proxy = proxy;
    s_accessorAndExtractorFn_object = object;
    *selectInfoPtr = s_accessorAndExtractorFn_selectInfoPtr;
    proxy->loadSimple(&s_accessorAndExtractorFn_int);
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
void executeSimpleCategoryTest(const char *typeName, const TYPE& testValue)
{
    Obj mX; const Obj& X = mX;
    const TYPE DEFAULT = TYPE();
    TYPE obj = DEFAULT;
    mX.loadSimple(&obj);

    ASSERTV(typeName, Category::BDEAT_SIMPLE_CATEGORY == X.category());
    ASSERTV(typeName,
            Category::BDEAT_SIMPLE_CATEGORY == bdeat_typeCategorySelect(X));

    // TBD: fix this!! object() is 0 instead of the address of the object as
    // documented.

    //ASSERTV(typeName, (void*)&obj, X.object(), &obj == (TYPE*) X.object());
    //ASSERTV(typeName, false == X.isNull());
    ASSERTV(false == X.isByteArrayValue());
    ASSERTV(true  == X.isValidForEncoding());
    ASSERTV(true  == X.isValidForDecoding());

    //SimpleAccessor<TYPE> accessor1;
    //SimpleAccessor<TYPE> accessor2;
    {
        SimpleAccessor accessor;
        ASSERTV(typeName, 0 == X.accessSimple(accessor));
        ASSERTV(typeName, &obj == accessor.d_address);

        accessor.d_rc = 1;
        ASSERTV(typeName, 1 == X.accessSimple(accessor));
        ASSERTV(typeName, &obj == accessor.d_address);
    }

    {
        SimpleAccessor accessor;
        ASSERTV(typeName, 0 == bdeat_typeCategoryAccessSimple(X, accessor));
        ASSERTV(typeName, &obj == accessor.d_address);

        accessor.d_rc = 1;
        ASSERTV(typeName, 1 == bdeat_typeCategoryAccessSimple(X, accessor));
        ASSERTV(typeName, &obj == accessor.d_address);
    }

    {
        SimpleManipulator<TYPE> manipulator;

        ASSERTV(typeName, 0 == mX.manipulateSimple(manipulator));
        ASSERTV(typeName, &obj == manipulator.d_address);

        manipulator.d_rc = 1;
        ASSERTV(typeName, 1 == mX.manipulateSimple(manipulator));
        ASSERTV(typeName, &obj == manipulator.d_address);
    }

    {
        SimpleManipulator<TYPE> manipulator;

        ASSERTV(typeName,
                0 == bdeat_typeCategoryManipulateSimple(&mX, manipulator));
        ASSERTV(typeName, &obj == manipulator.d_address);

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
// 'Test::Simple' is a Sequence type.  To create a 'SerializableObjectProxy'
// for this type, we first need to create a function that can create the proxy
// object for its element.  Note that in this case, 'Test::Simple' only have
// one element, 'status':
//..
void elementAccessor(SerializableObjectProxy       *proxy,
                     const SerializableObjectProxy &object,
                     int                            index)
{
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
      case 12: {
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
// Then, we define the XML string that we would like to decode:
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
    baea::SerializableObjectProxy decodeProxy;
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
      case 11: {
        // --------------------------------------------------------------------
        // TESTING PROPERTIES
        // --------------------------------------------------------------------

        Obj mX;  const Obj& X = mX;

        if (verbose) cout << "Testing properties of simple type." << endl;
        {
            int value;
            mX.loadSimple(&value);

        }

        if (verbose) cout << "Testing properties of enumeration type." << endl;
        {
            int value;
            bdeat_EnumeratorInfo enumInfo;
            mX.loadEnumeration(0, &enumInfo, 0);

            ASSERTV(Category::BDEAT_ENUMERATION_CATEGORY == X.category());
            ASSERTV(Category::BDEAT_ENUMERATION_CATEGORY ==
                                                  bdeat_typeCategorySelect(X));
            //ASSERTV(&value, X.object(), &value == X.object());
            ASSERTV(0 == X.className());
            ASSERTV(0 ==
                    bdeat_TypeName_Overloadable::bdeat_TypeName_className(X));
            //ASSERTV(false == X.isNull());
            ASSERTV(false == X.isByteArrayValue());
            ASSERTV(true  == X.isValidForEncoding());
            ASSERTV(false == X.isValidForDecoding());

            mX.loadEnumeration(&value,
                               &intSetterFn,
                               &stringSetterFn,
                               &enumInfo,
                               0);

            ASSERTV(Category::BDEAT_ENUMERATION_CATEGORY == X.category());
            ASSERTV(Category::BDEAT_ENUMERATION_CATEGORY ==
                                                  bdeat_typeCategorySelect(X));
            ASSERTV(&value, X.object(), &value == X.object());
            ASSERTV(0 == X.className());
            ASSERTV(0 ==
                    bdeat_TypeName_Overloadable::bdeat_TypeName_className(X));
            ASSERTV(false == X.isNull());
            ASSERTV(false == X.isByteArrayValue());
            ASSERTV(false == X.isValidForEncoding());
            ASSERTV(true  == X.isValidForDecoding());
        }

        if (verbose) cout << "Testing properties of array type." << endl;
        {
            std::vector<int> value(1);

            mX.loadArray(1, sizeof(int), value.data(), &accessorFn<int>);

            ASSERTV(Category::BDEAT_ARRAY_CATEGORY == X.category());
            ASSERTV(Category::BDEAT_ARRAY_CATEGORY ==
                                                  bdeat_typeCategorySelect(X));
            ASSERTV(value.data(), X.object(), value.data() == X.object());
            ASSERTV(0 == X.className());
            ASSERTV(0 ==
                    bdeat_TypeName_Overloadable::bdeat_TypeName_className(X));
            ASSERTV(false == X.isNull());
            ASSERTV(false == X.isByteArrayValue());
            ASSERTV(true  == X.isValidForEncoding());
            ASSERTV(false == X.isValidForDecoding());

            mX.loadArray(&value,
                         value.size(),
                         sizeof(int),
                         value.data(),
                         &resizerFn<int>,
                         &accessorFn<int>);

            ASSERTV(Category::BDEAT_ARRAY_CATEGORY == X.category());
            ASSERTV(Category::BDEAT_ARRAY_CATEGORY ==
                                                  bdeat_typeCategorySelect(X));
            ASSERTV(&value, X.object(),
                    &value == (std::vector<int> *) X.object());
            ASSERTV(0 == X.className());
            ASSERTV(0 ==
                    bdeat_TypeName_Overloadable::bdeat_TypeName_className(X));
            ASSERTV(false == X.isNull());
            ASSERTV(false == X.isByteArrayValue());
            ASSERTV(false == X.isValidForEncoding());
            ASSERTV(true  == X.isValidForDecoding());
        }

        if (verbose) cout << "Testing properties of selection type." << endl;
        {
            int                 value;
            bdeat_SelectionInfo info;
            const char *const   className = "foo";

            mX.loadChoice(0, &value, &info, className, &accessorFn<int>);

            ASSERTV(Category::BDEAT_CHOICE_CATEGORY == X.category());
            ASSERTV(Category::BDEAT_CHOICE_CATEGORY ==
                                                  bdeat_typeCategorySelect(X));
            ASSERTV(&value, X.object(), &value == X.object());
            ASSERTV(className == X.className());
            ASSERTV(className ==
                    bdeat_TypeName_Overloadable::bdeat_TypeName_className(X));
            ASSERTV(false == X.isNull());
            ASSERTV(false == X.isByteArrayValue());
            ASSERTV(true  == X.isValidForEncoding());
            ASSERTV(false == X.isValidForDecoding());

            mX.loadChoice(1,
                          &value,
                          &info,
                          &accessorAndExtractorFn,
                          &chooserFn);

            ASSERTV(Category::BDEAT_CHOICE_CATEGORY == X.category());
            ASSERTV(Category::BDEAT_CHOICE_CATEGORY ==
                                                  bdeat_typeCategorySelect(X));
            ASSERTV(&value, X.object(), &value == X.object());
            ASSERTV(0 == X.className());
            ASSERTV(0 ==
                    bdeat_TypeName_Overloadable::bdeat_TypeName_className(X));
            ASSERTV(false == X.isNull());
            ASSERTV(false == X.isByteArrayValue());
            ASSERTV(false == X.isValidForEncoding());
            ASSERTV(true  == X.isValidForDecoding());
        }

        if (verbose) cout << "Testing properties of sequence type." << endl;
        {
            int                 value;
            bdeat_AttributeInfo info;
            const char *const   className = "foo";
            mX.loadSequence(1, &value, &info, className, &elementAccessorFn);

            ASSERTV(Category::BDEAT_SEQUENCE_CATEGORY == X.category());
            ASSERTV(Category::BDEAT_SEQUENCE_CATEGORY ==
                                                  bdeat_typeCategorySelect(X));
            ASSERTV(&value, X.object(), &value == X.object());
            ASSERTV(className == X.className());
            ASSERTV(className ==
                    bdeat_TypeName_Overloadable::bdeat_TypeName_className(X));
            ASSERTV(false == X.isNull());
            ASSERTV(false == X.isByteArrayValue());
            ASSERTV(true  == X.isValidForEncoding());
            ASSERTV(true  == X.isValidForDecoding());
        }

        if (verbose) cout << "Testing properties of nullable type." << endl;
        {
            mX.loadNullable(0, &accessorFn<int>);

            ASSERTV(Category::BDEAT_NULLABLE_VALUE_CATEGORY == X.category());
            ASSERTV(Category::BDEAT_NULLABLE_VALUE_CATEGORY ==
                                                  bdeat_typeCategorySelect(X));
            ASSERTV(X.object(), 0 == X.object());
            ASSERTV(0 == X.className());
            ASSERTV(0 ==
                    bdeat_TypeName_Overloadable::bdeat_TypeName_className(X));
            ASSERTV(true  == X.isNull());
            ASSERTV(false == X.isByteArrayValue());
            ASSERTV(true  == X.isValidForEncoding());
            ASSERTV(false == X.isValidForDecoding());

            int value;
            mX.loadNullable(&value, &accessorFn<int>);

            ASSERTV(Category::BDEAT_NULLABLE_VALUE_CATEGORY == X.category());
            ASSERTV(Category::BDEAT_NULLABLE_VALUE_CATEGORY ==
                                                  bdeat_typeCategorySelect(X));
            ASSERTV(&value, X.object(), &value == X.object());
            ASSERTV(0 == X.className());
            ASSERTV(0 ==
                    bdeat_TypeName_Overloadable::bdeat_TypeName_className(X));
            ASSERTV(false == X.isNull());
            ASSERTV(false == X.isByteArrayValue());
            ASSERTV(true  == X.isValidForEncoding());
            ASSERTV(false == X.isValidForDecoding());


            bdeut_NullableValue<int> nullable;
            mX.loadNullable(&nullable,
                            &accessorFn<int>,
                            &nullableValueMaker<bdeut_NullableValue<int> >,
                            &nullableValueFetcher<bdeut_NullableValue<int> >);

            ASSERTV(Category::BDEAT_NULLABLE_VALUE_CATEGORY == X.category());
            ASSERTV(Category::BDEAT_NULLABLE_VALUE_CATEGORY ==
                                                  bdeat_typeCategorySelect(X));
            ASSERTV(&nullable, X.object(), &nullable == X.object());
            ASSERTV(0 == X.className());
            ASSERTV(0 ==
                    bdeat_TypeName_Overloadable::bdeat_TypeName_className(X));
            ASSERTV(true == X.isNull());
            ASSERTV(false == X.isByteArrayValue());
            ASSERTV(false == X.isValidForEncoding());
            ASSERTV(true  == X.isValidForDecoding());
        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'SerializableObjectProxy_NullableAdapter'
        // --------------------------------------------------------------------
 
        {
            Obj mX; const Obj& X = mX;
            int value = 1;
 
            mX.loadNullable(0, &accessorFn<int>);
            SerializableObjectProxy_NullableAdapter adapter = { &mX };
            ASSERTV(true == bdeat_nullableValueIsNull(adapter));

            mX.loadNullable(&value, &accessorFn<int>);
            ASSERTV(false == bdeat_nullableValueIsNull(adapter));

            ExtractAddressAccessor accessor;
            ASSERTV(0 == bdeat_nullableValueAccessValue(adapter, accessor));
            ASSERTV(&value == accessor.d_address);
        }

        {
            Obj mX; const Obj& X = mX;

            bdeut_NullableValue<int> value;

            mX.loadNullable(&value,
                            &accessorFn<int>,
                            &nullableValueMaker<bdeut_NullableValue<int> >,
                            &nullableValueFetcher<bdeut_NullableValue<int> >);

            SerializableObjectProxy_NullableAdapter adapter = { &mX };

            ASSERTV(true == bdeat_nullableValueIsNull(adapter));

            bdeat_nullableValueMakeValue(&adapter);
            ASSERTV(false == bdeat_nullableValueIsNull(adapter));
            ASSERTV(false == value.isNull());
            ASSERTV(0     == value.value());

            ExtractAddressManipulator manipulator;
            ASSERTV(0 == bdeat_nullableValueManipulateValue(&adapter, manipulator));
            ASSERTV(&value.value() == manipulator.d_address);
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING Nullable
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

        {
            Obj mX; const Obj& X = mX;

            mX.loadNullable(0, &accessorFn<int>);
            ASSERTV(true == X.isNull());
        }
        {
            Obj mX; const Obj& X = mX;

            int value = 1;

            mX.loadNullable(&value, &accessorFn<int>);
            {
                SimpleAccessor accessor;
                ASSERTV(0 == X.accessNullable(accessor));
                ASSERTV(&s_accessorFn_int == accessor.d_address);
            }

            {
                SimpleAccessor accessor;
                ASSERTV(0 == bdeat_typeCategoryAccessNullableValue(X,
                                                                   accessor));
                ASSERTV(&s_accessorFn_int == accessor.d_address);
            }
        }

        {
            Obj mX; const Obj& X = mX;

            bdeut_NullableValue<int> value;

            mX.loadNullable(&value,
                            &accessorFn<int>,
                            &nullableValueMaker<bdeut_NullableValue<int> >,
                            &nullableValueFetcher<bdeut_NullableValue<int> >);

            ASSERTV(true == X.isNull());

            mX.makeValue();
            ASSERTV(false == X.isNull());
            ASSERTV(false == value.isNull());
            ASSERTV(0     == value.value());

            {
                SimpleManipulator<int> manipulator;
                ASSERTV(0 == mX.manipulateNullable(manipulator));
                ASSERTV(&s_accessorFn_int == manipulator.d_address);
            }

            {
                SimpleManipulator<int> manipulator;
                ASSERTV(0 == bdeat_typeCategoryManipulateNullableValue(
                                                                 &mX,
                                                                 manipulator));
                ASSERTV(&s_accessorFn_int == manipulator.d_address);
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING Sequence
        //
        // Testing:
        //   int sequenceManipulateAttribute(MANIPULATOR& manipulator, int id);
        //   int sequenceManipulateAttribute(MANIPULATOR&, const char*, int;
        //   loadSequence(int, void *, const AInfo *, const char *, ElmtAccssr)
        //   bool sequenceHasAttribute(const char *name, int len) const;
        //   bool sequenceHasAttribute(int attributeId) const;
        //   int sequenceAccessAttribute(ACCESSOR& accessor, int attrId) const;
        //   int sequenceAccessAttributes(ACCESSOR& accessor) const;
        // --------------------------------------------------------------------
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

        int someObject;

        Obj proxy;
        proxy.loadSequence(NUM_INFO,
                           &someObject,
                           INFO,
                           "foo",
                           &elementAccessorFn);

        for (int ti = 0; ti < NUM_INFO; ++ti) {
            const int ID = INFO[ti].d_id;

            SequenceAccessor accessor;
            accessor.d_rc = ti;
            ASSERT(ti == proxy.sequenceAccessAttribute(accessor, ID));

            ASSERT(elementAccessorFn_proxy == accessor.d_proxy);
            ASSERT(elementAccessorFn_object == &proxy);
            ASSERT(elementAccessorFn_index == ti);
            ASSERT(accessor.d_address == &elementAccessorFn_int);
            ASSERT(accessor.d_info == INFO[ti]);
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING Choice
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
        // --------------------------------------------------------------------

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

        // accessor
        for (int ti = 0; ti < NUM_INFO; ++ti) {
            const int ID = INFO[ti].d_id;
            const char NAME[] = "foo";

            Obj mX;  const Obj& X = mX;

            int obj;

            mX.loadChoice(ID, &obj, &INFO[ti], NAME, &accessorFn<int>);

            ChoiceAccessor accessor;
            accessor.d_rc = ti;

            ASSERT(ti == X.choiceAccessSelection(accessor));
            ASSERT(s_accessorFn_proxy == accessor.d_proxy);
            ASSERT(s_accessorFn_object == &obj);
            ASSERT(accessor.d_address == &s_accessorFn_int);
            ASSERT(accessor.d_info == INFO[ti]);

            ASSERT(ti == bdeat_choiceAccessSelection(X, accessor));
            ASSERT(s_accessorFn_proxy == accessor.d_proxy);
            ASSERT(s_accessorFn_object == &obj);
            ASSERT(accessor.d_address == &s_accessorFn_int);
            ASSERT(accessor.d_info == INFO[ti]);
        }

        // manipulator
        {
            Obj mX;  const Obj& X = mX;

            int obj;
            mX.loadChoice(NUM_INFO,
                          &obj,
                          INFO,
                          &accessorAndExtractorFn,
                          &chooserFn);
            s_accessorAndExtractorFn_selectInfoPtr = INFO;

            for (int ti = 0; ti < NUM_INFO; ++ti) {
                const int ID = INFO[ti].d_id;

                s_chooserFn_rc = ti;
                ASSERT(ti   == mX.choiceMakeSelection(ID));
                ASSERT(ID   == s_chooserFn_id);
                ASSERT(&obj == s_chooserFn_object);
                ASSERT(ti   == bdeat_choiceMakeSelection(&mX, ID));
                ASSERT(ID   == s_chooserFn_id);
                ASSERT(&obj == s_chooserFn_object);


                ChoiceManipulator manipulator;
                ASSERT(0 == mX.choiceManipulateSelection(manipulator));

                ASSERT(s_accessorAndExtractorFn_proxy == manipulator.d_proxy);
                ASSERT(s_accessorAndExtractorFn_object == &obj);
                ASSERT(manipulator.d_address = &s_accessorAndExtractorFn_int);
                ASSERT(manipulator.d_info == INFO[0]);

                manipulator.d_rc = ti;
                ASSERT(ti == mX.choiceManipulateSelection(manipulator));
            }
        }
      } break;
      case 6: {
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
        //: 6 'object', 'isNull', 'isValidForEncoding' and 'isValidForDecoding'
        //:   returns ths expected result.
        //:
        //: 7 'bdeat' functions are correctly overloaded and can be found
        //:   through ADL.
        //
        // Testing:
        //   void resize(size_t newSize);
        //   int arrayManipulateElement(MANIPULATOR& manipulator, int index);
        //   loadArray(int length, int elementSize, void *begin, Accessor a);
        //   loadArray(void *, int, int, void *, Resizer, Accessor);
        //   bsl::size_t size() const;
        //   bool isByteArrayValue() const;
        //   int arrayAccessElement(ACCESSOR& accessor, int index) const;
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing Array type" << endl;
        {
            Obj mX; const Obj& X = mX;

            bsl::vector<int> obj;
            const int SIZE = 5;
            obj.resize(SIZE);
            for (int i = 0; i < SIZE; ++i) {
                obj[i] = i;
            }

            mX.loadArray(obj.size(),
                         sizeof(int),
                         obj.data(),
                         &accessorFn<int>);

            ASSERTV(false == X.isByteArrayValue());
            ASSERTV(SIZE  == X.size());
            ASSERTV(SIZE  == bdeat_arraySize(X));

            for (int i = 0; i < obj.size(); ++i) {
                ArrayElementAccessor<int> accessor;
                ASSERTV(0 == mX.arrayAccessElement(accessor, i));
                ASSERTV(&obj[i] == s_accessorFn_object);
                ASSERTV(s_accessorFn_proxy == accessor.d_proxy);
                ASSERTV(&s_accessorFn_int == accessor.d_address);

                ASSERTV(0 == bdeat_arrayAccessElement(X, accessor, i));
                ASSERTV(&obj[i] == s_accessorFn_object);
                ASSERTV(s_accessorFn_proxy == accessor.d_proxy);
                ASSERTV(&s_accessorFn_int == accessor.d_address);
            }

            ArrayAccessor<int> accessor;
            ASSERTV(0          == bdeat_typeCategoryAccessArray(X, accessor));
            ASSERTV(obj.data() == accessor.d_address);

            mX.loadArray(&obj,
                         obj.size(),
                         sizeof(int),
                         obj.data(),
                         &resizerFn<int>,
                         &accessorFn<int>);

            ASSERTV(false == X.isByteArrayValue());
            ASSERTV(SIZE == X.size());
            ASSERTV(SIZE == bdeat_arraySize(X));

            for (int i = 0; i < obj.size(); ++i) {
                ArrayElementManipulator<int> manipulator;
                ASSERTV(0       == mX.arrayManipulateElement(manipulator, i));
                ASSERTV(&obj[i] == s_accessorFn_object);
                ASSERTV(s_accessorFn_proxy == manipulator.d_proxy);
                ASSERTV(&s_accessorFn_int == manipulator.d_address);

                ASSERTV(0 ==
                            bdeat_arrayManipulateElement(&mX, manipulator, i));
                ASSERTV(&obj[i] == s_accessorFn_object);
                ASSERTV(s_accessorFn_proxy == manipulator.d_proxy);
                ASSERTV(&s_accessorFn_int == manipulator.d_address);
            }

            ArrayManipulator<int> manipulator;
            ASSERTV(0  == bdeat_typeCategoryManipulateArray(&mX, manipulator));
            ASSERTV(&obj == manipulator.d_address);

            mX.resize(0);
            ASSERTV(&obj       ==  s_resizerFn_object);
            ASSERTV(0          ==  s_resizerFn_newSize);
            ASSERTV(obj.data() == *s_resizerFn_begin);

            mX.resize(1);
            ASSERTV(&obj       ==  s_resizerFn_object);
            ASSERTV(1          ==  s_resizerFn_newSize);
            ASSERTV(obj.data() == *s_resizerFn_begin);

            bdeat_arrayResize(&mX, 0);
            ASSERTV(&obj       ==  s_resizerFn_object);
            ASSERTV(0          ==  s_resizerFn_newSize);
            ASSERTV(obj.data() == *s_resizerFn_begin);

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

            mX.loadArray(obj.size(), sizeof(char), &obj, 0);

            ASSERTV(true == X.isByteArrayValue());

            ArrayAccessor<char> accessor;
            ASSERTV(0 == bdeat_typeCategoryAccessArray(X, accessor));
            ASSERTV(&obj == accessor.d_address);

            ArrayManipulator<char> manipulator;
            ASSERTV(0 == bdeat_typeCategoryManipulateArray(&mX, manipulator));
            ASSERTV(&obj == manipulator.d_address);
        }

      }  break;
      case 5: {
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
        //: 5 'object', 'isNull', 'isValidForEncoding' and 'isValidForDecoding'
        //:   returns ths expected result.
        //:
        //: 6 'bdeat' functions are correctly overloaded and can be found
        //:   through ADL.
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

        using namespace bdeat_EnumFunctions;


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

            mX.loadEnumeration(VALUE, INFO, INFO_SIZE);

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
            mX.loadEnumeration(&obj,
                               &intSetterFn,
                               &stringSetterFn,
                               INFO,
                               INFO_SIZE);

            for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                for (int i = 0; i < INFO_SIZE; ++i) {
                    const int         VALUE  = INFO[i].d_value;
                    const char *const NAME   = INFO[i].d_name_p;
                    const int         LENGTH = INFO[i].d_nameLength;

                    s_intSetterFn_rc    = i;
                    s_stringSetterFn_rc = i;

                    switch (cfg) {
                      case 'a': {
                        ASSERTV(i == mX.enumFromInt(VALUE));
                        ASSERTV(&obj  == s_intSetterFn_object);
                        ASSERTV(VALUE == s_intSetterFn_value);
                      } break;
                      case 'b': {
                        ASSERTV(i == mX.enumFromString(NAME, LENGTH));
                        ASSERTV(&obj   == s_stringSetterFn_object);
                        ASSERTV(NAME   == s_stringSetterFn_value);
                        ASSERTV(LENGTH == s_stringSetterFn_length);
                      } break;
                      case 'c': {
                        ASSERTV(i == bdeat_enumFromInt(&mX, VALUE));
                        ASSERTV(&obj  == s_intSetterFn_object);
                        ASSERTV(VALUE == s_intSetterFn_value);
                      } break;
                      case 'd': {
                        ASSERTV(i == bdeat_enumFromString(&mX, NAME, LENGTH));
                        ASSERTV(&obj   == s_stringSetterFn_object);
                        ASSERTV(NAME   == s_stringSetterFn_value);
                        ASSERTV(LENGTH == s_stringSetterFn_length);
                      } break;
                    }
                }
            }
        }
      } break;
      case 4: {
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
        //: 4 'object', 'isNull', 'isValidForEncoding' and 'isValidForDecoding'
        //:   returns ths expected result.
        //:
        //: 5 'bdeat' functions are correctly overloaded and can be found
        //:   through ADL.
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

        executeSimpleCategoryTest<char>("char", 1);
        executeSimpleCategoryTest<unsigned char>("uchar", 2);
        executeSimpleCategoryTest<short>("short", 3);
        executeSimpleCategoryTest<int>("int", 4);
        executeSimpleCategoryTest<bsls_Types::Uint64>("uint64", 5);
        executeSimpleCategoryTest<float>("float", 6.1);
        executeSimpleCategoryTest<double>("double", 7.2);
        executeSimpleCategoryTest<bsl::string>("string", "hello world");
        executeSimpleCategoryTest<bdet_Datetime>("datetime",
                                                 bdet_Datetime(2001, 9, 3));
        executeSimpleCategoryTest<bdet_Date>("date", bdet_Date(2002, 10, 4));
        executeSimpleCategoryTest<bdet_Time>("time", bdet_Time(3, 11, 5));
        executeSimpleCategoryTest<bool>("bool", true);
        executeSimpleCategoryTest<bdet_DatetimeTz>(
                               "datetimetz",
                               bdet_DatetimeTz(bdet_Datetime(2004, 12, 6), 1));
        executeSimpleCategoryTest<bdet_DateTz>(
                                        "datetz",
                                        bdet_DateTz(bdet_Date(2005, 1, 7), 2));
        executeSimpleCategoryTest<bdet_TimeTz>(
                                           "timetz",
                                           bdet_TimeTz(bdet_Time(6, 2, 8), 3));
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING Sequence callback
        //
        // Test that the callback function supplied to the 'loadSequence'
        // method is invoked properly.
        // --------------------------------------------------------------------

        /*

        bdeat_AttributeInfo someInfo;
        someInfo.id() = 10;
        someInfo.name() = "plugh";
        someInfo.nameLength() = 5;

        int someObject;

        Obj proxy;
        proxy.loadSequence(1, &someObject, &someInfo, "foo",
                           &elementAccessorFn);

        SequenceAccessor sequenceAccessor;
        ASSERT(0 == proxy.sequenceAccessAttribute(sequenceAccessor, 10));

        ASSERT(elementAccessorFn_proxy == sequenceAccessor.d_proxy);
        ASSERT(elementAccessorFn_object == &proxy);
        ASSERT(elementAccessorFn_index == 0);
        ASSERT(sequenceAccessor.d_address == &elementAccessorFn_int);
        ASSERT(sequenceAccessor.d_info == someInfo);

        sequenceAccessor.d_rc = 5;
        ASSERT(5 == proxy.sequenceAccessAttribute(sequenceAccessor, 10));
        */

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING Choice callbacks
        //
        // Test that the callback functions supplied to the 'loadChoice'
        // methods are invoked properly.
        // --------------------------------------------------------------------

          /*
        // accessor
        int someObject;
        bdeat_SelectionInfo someInfo;
        someInfo.id() = 10;
        someInfo.name() = "plugh";
        someInfo.nameLength() = 5;

        Obj proxy;

        proxy.loadChoice(10, &someObject, &someInfo, "foo", &accessorFn<int>);

        ChoiceAccessor choiceAccessor;

        ASSERT(0 == proxy.choiceAccessSelection(choiceAccessor));

        ASSERT(accessorFn_proxy == choiceAccessor.d_proxy);
        ASSERT(accessorFn_object == &someObject);
        ASSERT(choiceAccessor.d_address = &someObject);
        ASSERT(choiceAccessor.d_info == someInfo);

        choiceAccessor.d_rc = 5;
        ASSERT(5 == proxy.choiceAccessSelection(choiceAccessor));

        // manipulator
        int someChoice; // different than someObject
        proxy.loadChoice(1, &someChoice, &someInfo,
                         &accessorAndExtractorFn,
                         &chooserFn);
        accessorAndExtractorFn_selectInfoPtr = &someInfo;

        ASSERT(0 == proxy.choiceMakeSelection(2));
        ASSERT(2 == chooserFn_id);
        ASSERT(&someChoice == chooserFn_object);
        ASSERT(0 == proxy.choiceMakeSelection(10));


        ChoiceManipulator choiceManipulator;
        ASSERT(0 == proxy.choiceManipulateSelection(choiceManipulator));

        ASSERT(accessorAndExtractorFn_proxy == choiceManipulator.d_proxy);
        ASSERT(accessorAndExtractorFn_object == &someChoice);
        ASSERT(choiceManipulator.d_address = &accessorAndExtractorFn_int);
        ASSERT(choiceManipulator.d_info == someInfo);

        choiceManipulator.d_rc = 7;
        ASSERT(7 == proxy.choiceManipulateSelection(choiceManipulator));

        chooserFn_rc = 6;
        ASSERT(6 == proxy.choiceMakeSelection(2));
        */

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING Constructor
        //
        // Test that SerializableObjectProxy is initialized to a valid empty
        // state.
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing constructor" << endl
                 << "===================" << endl;
        }

        Obj mX; const Obj& X = mX;

        ASSERTV(X.isNull());
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
