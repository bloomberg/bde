// bcem_aggregate.t.cpp                                               -*-C++-*-

#include <bcem_aggregate.h>

#include <bcema_sharedptr.h>

#include <bcem_errorattributes.h>
#include <bcem_fieldselector.h>

#include <bdem_berdecoder.h>
#include <bdem_berencoder.h>
#include <bdem_choice.h>
#include <bdem_choicearray.h>
#include <bdem_elemattrlookup.h>
#include <bdem_elemref.h>
#include <bdem_elemtype.h>
#include <bdem_list.h>
#include <bdem_properties.h>
#include <bdem_row.h>
#include <bdem_schema.h>
#include <bdem_schemaaggregateutil.h>
#include <bdem_schemautil.h>
#include <bdem_table.h>

#include <bdeat_arrayfunctions.h>
#include <bdeat_choicefunctions.h>
#include <bdeat_enumfunctions.h>
#include <bdeat_nullablevaluefunctions.h>
#include <bdeat_sequencefunctions.h>
#include <bdeat_typecategory.h>
#include <bdeat_typename.h>
#include <bdeat_valuetypefunctions.h>

#include <bdesb_fixedmemoutstreambuf.h>

#include <bdex_byteinstream.h>
#include <bdex_byteoutstream.h>
#include <bdex_instreamfunctions.h>
#include <bdex_outstreamfunctions.h>
#include <bdex_testinstream.h>
#include <bdex_testoutstream.h>
#include <bdex_testinstreamexception.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>

#include <bslmf_assert.h>

#include <bsls_objectbuffer.h>
#include <bsls_platform.h>
#include <bsls_stopwatch.h>

#include <bsl_iostream.h>
#include <bsl_list.h>
#include <bsl_sstream.h>
#include <bsl_stack.h>
#include <bsl_cstdlib.h>

#ifdef BSLS_PLATFORM__OS_WINDOWS
#include <crtdbg.h>  // _CrtSetReportMode, to suppress popups
#endif

using namespace BloombergLP;
// using bsl::cout;  // Replaced by tst::cout, below
using bsl::endl;
using bsl::cerr;
using bsl::flush;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// This component provides an easy to use interface to the bdem
// package.  It allows users to store complex data, bind it to some meta-data
// and provides access to the stored data by field names or ids as specified in
// the bound meta-data.
//
// Most of the functionality provided by this component is performed by lower
// level bdem components.  For example, the type conversions supported by this
// component is implemented in the 'bdem_Convert' component.  Similarly, all
// the meta-data functionality resides in the 'bdem_Schema' component, and
// this component uses the bdem aggregate components to store data and
// retrieve data.  Thus for a lot of the tests it will suffice to test that
// this component forwards data to the appropriate bdem components.
// Additionally, we will need to test that 1) this component correctly
// navigates to the appropriate sub elements, 2) maintains the scripting
// language semantics of by-value access for scalar and by-reference access
// for non-scalars, and 3) does sufficient error checking and allows
// operations only on the applicable types.
//
// The test plan for this component follows the general pattern used for
// value-semantic components.  Additionally, as this component
// contains many private functions, we will try to test them each (as much as
// possible) through the available interface.  The test plan for each test case
// will mention the private functions that will be tested in it.  Finally,
// given the complex interface provided by this component, we will test that
// error reporting works correctly.

//-----------------------------------------------------------------------------

// CLASS METHODS
// [ 9] bool areEquivalent(const bcem_Aggregate& lhs,
//                         const bcem_Aggregate& rhs);
// [ 9] bool areIdentical(const bcem_Aggregate& lhs,
//                        const bcem_Aggregate& rhs);
//
// CREATORS
// [10] bcem_Aggregate(bslma_Allocator *basicAllocator = 0);
// [10] template <typename VALTYPE>
//      bcem_Aggregate(const ET::Type   dataType,
//                     const VALTYPE&   value,
//                     bslma_Allocator *basicAllocator);
// [10] explicit bcem_Aggregate(const ConstRecDefShdPtr&  recordDefPtr,
//                              bslma_Allocator          *basicAllocator);
// [10] explicit bcem_Aggregate(const RecDefShdPtr&  recordDefPtr,
//                              bslma_Allocator     *basicAllocator);
// [10] bcem_Aggregate(const ConstRecDefShdPtr&  recordDefPtr,
//                     ET::Type                  elemType,
//                     bslma_Allocator          *basicAllocator);
// [10] bcem_Aggregate(const RecDefShdPtr&  recordDefPtr,
//                     ET::Type             elemType,
//                     bslma_Allocator     *basicAllocator);
// [10] bcem_Aggregate(const ConstSchemaShdPtr&  schemaPtr,
//                     const bsl::string&        recordName,
//                     ET::Type                  elemType,
//                     bslma_Allocator          *basicAllocator);
// [10] bcem_Aggregate(const SchemaShdPtr&  schemaPtr,
//                     const bsl::string&   recordName,
//                     ET::Type             elemType,
//                     bslma_Allocator     *basicAllocator);
// [21] bcem_Aggregate(const bcem_Aggregate&  other,
//                     bslma_Allocator       *basicAllocator);
// [10] ~bcem_Aggregate();

// MANIPULATORS
// [12] bcem_Aggregate& operator=(const bcem_Aggregate& rhs);
// [14] void makeNull();
// [14] void reset();
// [ 7] bdem_ElemRef asElemRef();
// [ 3] template <typename VALTYPE>
//      void setField(NameOrIndex    fieldOrIdx1,
//                    const VALTYPE& value) const;
// [ 4] template <typename VALTYPE>
//      void setField(NameOrIndex    fieldOrIdx1,
//                    NameOrIndex    fieldOrIdx2,
//                    const VALTYPE& value) const;
// [ 4] template <typename VALTYPE>
//      void setField(NameOrIndex    fieldOrIdx1,
//                    NameOrIndex    fieldOrIdx2,
//                    NameOrIndex    fieldOrIdx3,
//                    const VALTYPE& value) const;
// [ 4] template <typename VALTYPE>
//      void setField(NameOrIndex    fieldOrIdx1,
//                    NameOrIndex    fieldOrIdx2,
//                    NameOrIndex    fieldOrIdx3,
//                    NameOrIndex    fieldOrIdx4,
//                    const VALTYPE& value) const;
// [ 4] template <typename VALTYPE>
//      void setField(NameOrIndex    fieldOrIdx1,
//                    NameOrIndex    fieldOrIdx2,
//                    NameOrIndex    fieldOrIdx3,
//                    NameOrIndex    fieldOrIdx4,
//                    NameOrIndex    fieldOrIdx5,
//                    const VALTYPE& value) const;
// [ 4] template <typename VALTYPE>
//      void setField(NameOrIndex    fieldOrIdx1,
//                    NameOrIndex    fieldOrIdx2,
//                    NameOrIndex    fieldOrIdx3,
//                    NameOrIndex    fieldOrIdx4,
//                    NameOrIndex    fieldOrIdx5,
//                    NameOrIndex    fieldOrIdx6,
//                    const VALTYPE& value) const;
// [ 4] template <typename VALTYPE>
//      void setField(NameOrIndex    fieldOrIdx1,
//                    NameOrIndex    fieldOrIdx2,
//                    NameOrIndex    fieldOrIdx3,
//                    NameOrIndex    fieldOrIdx4,
//                    NameOrIndex    fieldOrIdx5,
//                    NameOrIndex    fieldOrIdx6,
//                    NameOrIndex    fieldOrIdx7,
//                    const VALTYPE& value) const;
// [ 4] template <typename VALTYPE>
//      void setField(NameOrIndex    fieldOrIdx1,
//                    NameOrIndex    fieldOrIdx2,
//                    NameOrIndex    fieldOrIdx3,
//                    NameOrIndex    fieldOrIdx4,
//                    NameOrIndex    fieldOrIdx5,
//                    NameOrIndex    fieldOrIdx6,
//                    NameOrIndex    fieldOrIdx7,
//                    NameOrIndex    fieldOrIdx8,
//                    const VALTYPE& value) const;
// [ 4] template <typename VALTYPE>
//      void setField(NameOrIndex    fieldOrIdx1,
//                    NameOrIndex    fieldOrIdx2,
//                    NameOrIndex    fieldOrIdx3,
//                    NameOrIndex    fieldOrIdx4,
//                    NameOrIndex    fieldOrIdx5,
//                    NameOrIndex    fieldOrIdx6,
//                    NameOrIndex    fieldOrIdx7,
//                    NameOrIndex    fieldOrIdx8,
//                    NameOrIndex    fieldOrIdx9,
//                    const VALTYPE& value) const;
// [ 4] template <typename VALTYPE>
//      void setField(NameOrIndex    fieldOrIdx1,
//                    NameOrIndex    fieldOrIdx2,
//                    NameOrIndex    fieldOrIdx3,
//                    NameOrIndex    fieldOrIdx4,
//                    NameOrIndex    fieldOrIdx5,
//                    NameOrIndex    fieldOrIdx6,
//                    NameOrIndex    fieldOrIdx7,
//                    NameOrIndex    fieldOrIdx8,
//                    NameOrIndex    fieldOrIdx9,
//                    NameOrIndex    fieldOrIdx10,
//                    const VALTYPE& value) const;
// [ 3] template <typename VALTYPE>
//      void setFieldNull(NameOrIndex fieldOrIdx1) const;
// [ 8] template <typename VALTYPE>
//      void setFieldById(int fieldId, const VALTYPE& value) const;
// [ 3] template <typename VALTYPE>
//      void setItem(int index, const VALTYPE& value) const;
// [16] template <typename VALTYPE>
//      void append(const VALTYPE& newItem) const;
// [29] const bcem_Aggregate reserveRaw(size_t numItems) const;
// [16] template <typename VALTYPE>
//      void insert(int pos, const VALTYPE& newItem) const;
// [15] void resize(int newSize) const;
// [17] void appendItems(int numItems) const;
// [17] void insertItems(int pos, int numItems) const;
// [17] void removeItems(int pos, int numItems) const;
// [18] void removeAllItems() const;
// [19] bcem_Aggregate makeSelection(const char         *newSelector) const;
// [19] bcem_Aggregate makeSelection(const bsl::string&  newSelector) const;
// [19] template <typename VALTYPE>
//      bcem_Aggregate makeSelection(const char        *newSelector,
//                                   const VALTYPE&     value) const;
// [19] template <typename VALTYPE>
//      bcem_Aggregate makeSelection(const bsl::string& newSelector,
//                                   const VALTYPE&     value) const;
// [20] bcem_Aggregate makeSelectionById(int newSelectorId) const;
// [20] template <typename VALTYPE>
//      bcem_Aggregate makeSelectionById(int            newSelectorId,
//                                       const VALTYPE& value) const;
// [23] bcem_Aggregate makeValue() const;
// [25] enumeration manipulators
// [26] STREAM& bdexStreamIn(STREAM& stream, int version);

// ACCESSORS
// [ 7] bsl::string asString() const;
// [ 6] bool asBool() const;
// [ 6] char asChar() const;
// [ 6] short asShort() const;
// [ 6] int asInt() const;
// [ 6] bsls_PlatformUtil::Int64 asInt64() const;
// [ 6] float asFloat() const;
// [ 6] double asDouble() const;
// [ 6] bdet_Datetime asDatetime() const;
// [ 6] bdet_DatetimeTz asDatetimeTz() const;
// [ 6] bdet_Date asDate() const;
// [ 6] bdet_DateTz asDateTz() const;
// [ 6] bdet_Time asTime() const;
// [ 6] bdet_TimeTz asTimeTz() const;
// [ 7] bdem_ConstElemRef asElemRef() const;
// [30] bsl::size_t capacityRaw() const;
// [ 5] bool hasField(const char *fieldName) const;
// [ 5] bcem_Aggregate field(NameOrIndex fieldOrIdx) const;
// [ 5] bcem_Aggregate field(NameOrIndex fieldOrIdx1,
//                           NameOrIndex fieldOrIdx2,
//                           NameOrIndex fieldOrIdx3 = NameOrIndex(),
//                           NameOrIndex fieldOrIdx4 = NameOrIndex(),
//                           NameOrIndex fieldOrIdx5 = NameOrIndex(),
//                           NameOrIndex fieldOrIdx6 = NameOrIndex(),
//                           NameOrIndex fieldOrIdx7 = NameOrIndex(),
//                           NameOrIndex fieldOrIdx8 = NameOrIndex(),
//                           NameOrIndex fieldOrIdx9 = NameOrIndex(),
//                           NameOrIndex fieldOrIdx10 = NameOrIndex()) const;
// [ 8] bcem_Aggregate fieldById(int fieldId) const;
// [ 5] ET::Type
//      fieldType(NameOrIndex fieldOrIdx1,
//                NameOrIndex fieldOrIdx2 = NameOrIndex(),
//                NameOrIndex fieldOrIdx3 = NameOrIndex(),
//                NameOrIndex fieldOrIdx4 = NameOrIndex(),
//                NameOrIndex fieldOrIdx5 = NameOrIndex(),
//                NameOrIndex fieldOrIdx6 = NameOrIndex(),
//                NameOrIndex fieldOrIdx7 = NameOrIndex(),
//                NameOrIndex fieldOrIdx8 = NameOrIndex(),
//                NameOrIndex fieldOrIdx9 = NameOrIndex(),
//                NameOrIndex fieldOrIdx10 = NameOrIndex()) const;
// [ 8] ET::Type fieldTypeById(int fieldId) const;
// [ 5] bdem_ElemRef fieldRef(NameOrIndex fieldOrIdx1,
//                            NameOrIndex fieldOrIdx2 = NameOrIndex(),
//                            NameOrIndex fieldOrIdx3 = NameOrIndex(),
//                            NameOrIndex fieldOrIdx4 = NameOrIndex(),
//                            NameOrIndex fieldOrIdx5 = NameOrIndex(),
//                            NameOrIndex fieldOrIdx6 = NameOrIndex(),
//                            NameOrIndex fieldOrIdx7 = NameOrIndex(),
//                            NameOrIndex fieldOrIdx8 = NameOrIndex(),
//                            NameOrIndex fieldOrIdx9 = NameOrIndex(),
//                            NameOrIndex fieldOrIdx10 = NameOrIndex()) const;
// [ 8] bdem_ElemRef fieldRefById(int fieldId) const;
// [ 5] const bdem_FieldDef *fieldDef() const;
// [13] bcem_Aggregate operator[](int index) const;
// [13] int length() const;
// [13] int size() const;
// [14] bool isNul2() const;
// [19] bcem_Aggregate selection() const;
// [14] int numSelections() const;
// [19] const char *selector() const;
// [20] int selectorId() const;
// [22] bcem_Aggregate clone(bslma_Allocator *basicAllocator) const;
// [22] bcem_Aggregate cloneData(bslma_Allocator *basicAllocator) const;
// [ 7] ET::Type dataType() const;
// [10] const bdem_RecordDef& recordDef() const;
// [10] const void *data() const;
// [10] ConstRecDefShdPtr recordDefPtr() const;
// [10] bcema_SharedPtr<void> dataPtr() const;
// [10] bsl::ostream& print(bsl::ostream& stream,
//                          int           level,
//                          int           spacesPerLevel) const;
// [25] enumeration accessors
// [26] int maxSupportedBdexVersion() const;
// [26] STREAM& bdexStreamOut(STREAM& stream, int version) const;
//
// FREE OPERATORS
// [10] bsl::ostream& operator<<(bsl::ostream& stream,
//                               const bcem_Aggregate& obj);
//
// BDEAT FRAMEWORK
// [24] namespace bdeat_TypeCategoryFunctions
// [24] namespace bdeat_SequenceFunctions
// [24] namespace bdeat_ChoiceFunctions
// [24] namespace bdeat_ArrayFunctions
// [24] namespace bdeat_EnumFunctions
// [24] namespace bdeat_NullableValueFunctions
// [24] namespace bdeat_ValueTypeFunctions
//
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] ggSchema, ggList, ggTable, ggChoice, ggChoiceArray
// [31] USAGE EXAMPLE
//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
namespace {

int testStatus = 0;
const int LINE = -1;  // Dummy line if no 'LINE' is defined.

void aSsErT(int c, const char *s, int srcLine, int dataLine) {
    if (c) {
        if (dataLine >= 0) bsl::cout << "LINE: " << dataLine << bsl::endl;
        bsl::cout << "Error " << __FILE__ << "(" << srcLine << "): " << s
                  << "    (failed)" << bsl::endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

class TestOut {
    // Works like bsl::cout except it handles null pointers gracefully.

    bsl::ostream& d_os;
  public:
    TestOut(bsl::ostream& os) : d_os(os) { }
    bsl::ostream& os() const { return d_os; }

    template <typename T>
    TestOut& operator<<(const T& v);
        // Print the specified 'v' value and return this stream.

    template <typename T>
    TestOut& operator<<(T *const& v);
        // Specialization for pointers.  Print the specified 'v' pointer and
        // return this stream.  Print "<NULL>" for null pointers.

    TestOut& operator<<(bsl::ostream& (*v)(bsl::ostream&));
        // Print the specified 'v' value and return this stream.

};

template <typename T>
inline
TestOut& TestOut::operator<<(const T& v)
{
    d_os << v;
    return *this;
}

template <typename T>
inline
TestOut& TestOut::operator<<(T *const& v)
{
    if (0 == v) {
        d_os << "<NULL>";
    }
    else {
        d_os << v;
    }
    return *this;
}

inline
TestOut& TestOut::operator<<(bsl::ostream& (*v)(bsl::ostream&))
{
    d_os << v;
    return *this;
}

} // close unnamed namespace

namespace tst {
    // Use tst::cout instead of bsl::cout to get null-tolerant behavior
    TestOut cout(bsl::cout);
}

using tst::cout;

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__, LINE); }
//--------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { tst::cout << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__, LINE); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { tst::cout << #I << ": " << I << "\t" << #J << ": " \
                          << J << "\n"; aSsErT(1, #X, __LINE__, LINE); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { tst::cout << #I << ": " << I << "\t" << #J << ": " << J \
                         << "\t" << #K << ": " << K << "\n";           \
                aSsErT(1, #X, __LINE__, LINE); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { tst::cout << #I << ": " << I << "\t" << #J << ": " << J \
                         << "\t" << #K << ": " << K << "\t" << #L << ": " \
                         << L << "\n"; aSsErT(1, #X, __LINE__, LINE); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { tst::cout << #I << ": " << I << "\t" << #J << ": " << J    \
                         << "\t" << #K << ": " << K << "\t" << #L << ": " \
                         << L << "\t" << #M << ": " << M << "\n";         \
               aSsErT(1, #X, __LINE__, LINE); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { tst::cout << #I << ": " << I << "\t" << #J << ": " << J     \
                         << "\t" << #K << ": " << K << "\t" << #L << ": "  \
                         << L << "\t" << #M << ": " << M << "\t" << #N     \
                         << ": " << N << "\n";                             \
               aSsErT(1, #X, __LINE__, LINE); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) tst::cout << #X " = " << (X) << bsl::endl; // Print ID and value.
#define Q(X) tst::cout << "<| " #X " |>" << bsl::endl;  // Quote ID literally.
#define P_(X) tst::cout << #X " = " << (X) << ", " << flush; // P(X) w/o '\n'
#define L_ __LINE__                                // current Line number
#define T_ tst::cout << "\t" << flush;             // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;

const int BCEM_ERR_TBD = -1;
const int BDEM_NULL_FIELD_ID = bdem_RecordDef::BDEM_NULL_FIELD_ID;

typedef bcem_Aggregate        Obj;
typedef bcem_ErrorAttributes  Err;
typedef bcem_ErrorCode        ErrorCode;

const char *errorNm(int errorCode) {

    static const char *ERROR_NAMES[] = {
        "UNKNOWN_ERROR",
        "NOT_A_RECORD",
        "NOT_A_SEQUENCE",
        "NOT_A_CHOICE",
        "NOT_AN_ARRAY",
        "BAD_FIELDNAME",
        "BAD_FIELDID",
        "BAD_FIELDINDEX",
        "BAD_ARRAYINDEX",
        "NOT_SELECTED",
        "BAD_CONVERSION",
        "BAD_ENUMVALUE",
        "NON_CONFORMANT",
        "AMBIGUOUS_ANON"
    };

    if (0 == errorCode) {
        return "NO_ERROR";
    }
    else if (BCEM_ERR_TBD == errorCode) {
        return "BCEM_ERR_TBD";
    }
    else if (ErrorCode::BCEM_UNKNOWN_ERROR <= errorCode &&
             errorCode <= ErrorCode::BCEM_AMBIGUOUS_ANON) {
        return ERROR_NAMES[errorCode - ErrorCode::BCEM_UNKNOWN_ERROR];
    }
    else {
        return "<unexpected error code>";
    }
}

void aSsErTAggError(const bcem_Aggregate& agg, const char *aggNm,
                    int errCode, const char* errCodeNm,
                    int srcLine, int dataLine)
{
    if (BCEM_ERR_TBD == errCode ?
        !agg.isError() : errCode != agg.errorCode()) {
        bsl::string s(errCodeNm);
        // Assertion failed.
        tst::cout << errCodeNm << ": " << errorNm(errCode) << '\t'
                  << aggNm << ".errorCode(): " << errorNm(agg.errorCode())
                  << '\n';
        s += " == ";
        s += aggNm;
        s += ".errorCode()";
        aSsErT(1, s.c_str(), srcLine, dataLine);
    }
    else if (veryVerbose) {
        bsl::cerr << "line " << srcLine;
        if (dataLine >= 0) bsl::cerr << " w/LINE=" << dataLine;
        bsl::cerr << ": " << aggNm << " = "
                  << '[' << errorNm(agg.errorCode()) << "] "
                  << agg.errorMessage() << bsl::endl;
    }
}

#define ASSERT_AGG_ERROR(a, e) \
    aSsErTAggError((a), #a, (e), #e, __LINE__, LINE)
    // Assert that the specified aggregate 'a' has the specified error code
    // 'e'.  If 'e' is 'BCEM_ERR_TBD', then any failure code is considered a
    // match.  If 'e' is zero, then 'a' must be in a non-error state.  If
    // 'veryVerbose' is non-zero, then a verbose message is printed even when
    // the error codes match.

typedef bdem_ElemType         ET;
typedef bdem_ElemType         EType;
typedef bdem_ConstElemRef     CERef;
typedef bdem_ElemRef          ERef;
typedef bdem_Properties       Prop;
typedef bdem_ElemAttrLookup   EAL;

typedef bdem_List             List;
typedef bdem_Row              Row;
typedef bdem_Table            Table;
typedef bdem_Choice           Choice;
typedef bdem_ChoiceArrayItem  ChoiceItem;
typedef bdem_ChoiceArray      ChoiceArray;

typedef bdem_FieldDef           FldDef;
typedef bdem_FieldSpec          FldSpec;
typedef bdem_FieldDefAttributes FldAttr;
typedef bdem_RecordDef          RecDef;
typedef RecDef::RecordType      RecType;
typedef bdem_EnumerationDef     EnumDef;
typedef bdem_Schema             Schema;

typedef bdem_SchemaUtil          SchemaUtil;
typedef bdem_SchemaAggregateUtil SchemaAggUtil;
typedef bdeat_FormattingMode     Format;

typedef bcema_SharedPtr<const RecDef>        ConstRecDefShdPtr;
typedef bcema_SharedPtr<RecDef>              RecDefShdPtr;
typedef bcema_SharedPtr<const Schema>        ConstSchemaShdPtr;
typedef bcema_SharedPtr<Schema>              SchemaShdPtr;
typedef bcema_SharedPtr<const List>          ConstListShdPtr;
typedef bcema_SharedPtr<List>                ListShdPtr;
typedef bcema_SharedPtr<const Row>           ConstRowShdPtr;
typedef bcema_SharedPtr<Row>                 RowShdPtr;
typedef bcema_SharedPtr<const Table>         ConstTableShdPtr;
typedef bcema_SharedPtr<Table>               TableShdPtr;
typedef bcema_SharedPtr<const Choice>        ConstChoiceShdPtr;
typedef bcema_SharedPtr<Choice>              ChoiceShdPtr;
typedef bcema_SharedPtr<const ChoiceItem>    ConstChoiceItemShdPtr;
typedef bcema_SharedPtr<ChoiceItem>          ChoiceItemShdPtr;
typedef bcema_SharedPtr<const ChoiceArray>   ConstChoiceArrayShdPtr;
typedef bcema_SharedPtr<ChoiceArray>         ChoiceArrayShdPtr;
typedef bcema_SharedPtr<void>                VoidDataShdPtr;

typedef bcema_SharedPtrNilDeleter NilDeleter;

typedef bdet_Time             Time;
typedef bdet_Date             Date;
typedef bdet_Datetime         Datetime;
typedef bdet_DatetimeTz       DatetimeTz;
typedef bdet_DateTz           DateTz;
typedef bdet_TimeTz           TimeTz;

typedef bsls_PlatformUtil::Int64         Int64;

typedef bdeat_TypeName     TN;
typedef bdeat_TypeCategory TC;

namespace TCF = bdeat_TypeCategoryFunctions;
namespace SF  = bdeat_SequenceFunctions;
namespace CF  = bdeat_ChoiceFunctions;
namespace AF  = bdeat_ArrayFunctions;
namespace EF  = bdeat_EnumFunctions;
namespace NVF = bdeat_NullableValueFunctions;

//=============================================================================
//                           GLOBAL DATA FOR TESTING
//-----------------------------------------------------------------------------

        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // Create Three Distinct Exemplars For Each Element Type
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool  mA22 ;        const bool&   A22 = mA22;
bool  mB22 ;        const bool&   B22 = mB22;
bool  mN22 ;        const bool&   N22 = mN22;

char  mA00 ;        const char&   A00 = mA00;
char  mB00 ;        const char&   B00 = mB00;
char  mN00 ;        const char&   N00 = mN00;

short mA01 ;        const short&  A01 = mA01;
short mB01 ;        const short&  B01 = mB01;
short mN01 ;        const short&  N01 = mN01;

int   mA02 ;        const int&    A02 = mA02;
int   mB02 ;        const int&    B02 = mB02;
int   mN02 ;        const int&    N02 = mN02;

bsls_PlatformUtil::Int64 mA03;
bsls_PlatformUtil::Int64 mB03;
bsls_PlatformUtil::Int64 mN03;
const bsls_PlatformUtil::Int64& A03 = mA03;
const bsls_PlatformUtil::Int64& B03 = mB03;
const bsls_PlatformUtil::Int64& N03 = mN03;

float            mA04;           const float&            A04 = mA04;
float            mB04;           const float&            B04 = mB04;
float            mN04;           const float&            N04 = mN04;

double           mA05;           const double&           A05 = mA05;
double           mB05;           const double&           B05 = mB05;
double           mN05;           const double&           N05 = mN05;

bsl::string      mA06;           const bsl::string&      A06 = mA06;
bsl::string      mB06;           const bsl::string&      B06 = mB06;
bsl::string      mN06;           const bsl::string&      N06 = mN06;

// Note: bdet_Datetime  X07 implemented in terms of X08 and X09.

bdet_Date        mA08;           const bdet_Date&        A08 = mA08;
bdet_Date        mB08;           const bdet_Date&        B08 = mB08;
bdet_Date        mN08;           const bdet_Date&        N08 = mN08;

bdet_Time        mA09;           const bdet_Time&        A09 = mA09;
bdet_Time        mB09;           const bdet_Time&        B09 = mB09;
bdet_Time        mN09;           const bdet_Time&        N09 = mN09;

bdet_Datetime    mA07;           const bdet_Datetime&    A07 = mA07;
bdet_Datetime    mB07;           const bdet_Datetime&    B07 = mB07;
bdet_Datetime    mN07;           const bdet_Datetime&    N07 = mN07;

bdet_DateTz      mA24;           const bdet_DateTz&      A24 = mA24;
bdet_DateTz      mB24;           const bdet_DateTz&      B24 = mB24;
bdet_DateTz      mN24;           const bdet_DateTz&      N24 = mN24;

bdet_TimeTz      mA25;           const bdet_TimeTz&      A25 = mA25;
bdet_TimeTz      mB25;           const bdet_TimeTz&      B25 = mB25;
bdet_TimeTz      mN25;           const bdet_TimeTz&      N25 = mN25;

bdet_DatetimeTz  mA23;           const bdet_DatetimeTz&  A23 = mA23;
bdet_DatetimeTz  mB23;           const bdet_DatetimeTz&  B23 = mB23;
bdet_DatetimeTz  mN23;           const bdet_DatetimeTz&  N23 = mN23;

bsl::vector<bool>  mA26;         const bsl::vector<bool>&  A26 = mA26;
bsl::vector<bool>  mB26;         const bsl::vector<bool>&  B26 = mB26;
bsl::vector<bool>  mN26;         const bsl::vector<bool>&  N26 = mN26;

bsl::vector<char>  mA10;         const bsl::vector<char>&  A10 = mA10;
bsl::vector<char>  mB10;         const bsl::vector<char>&  B10 = mB10;
bsl::vector<char>  mN10;         const bsl::vector<char>&  N10 = mN10;

bsl::vector<short> mA11;         const bsl::vector<short>& A11 = mA11;
bsl::vector<short> mB11;         const bsl::vector<short>& B11 = mB11;
bsl::vector<short> mN11;         const bsl::vector<short>& N11 = mN11;

bsl::vector<int>   mA12;         const bsl::vector<int>&   A12 = mA12;
bsl::vector<int>   mB12;         const bsl::vector<int>&   B12 = mB12;
bsl::vector<int>   mN12;         const bsl::vector<int>&   N12 = mN12;

bsl::vector<bsls_PlatformUtil::Int64> mA13;
bsl::vector<bsls_PlatformUtil::Int64> mB13;
bsl::vector<bsls_PlatformUtil::Int64> mN13;
const bsl::vector<bsls_PlatformUtil::Int64>&  A13 = mA13;
const bsl::vector<bsls_PlatformUtil::Int64>&  B13 = mB13;
const bsl::vector<bsls_PlatformUtil::Int64>&  N13 = mN13;

bsl::vector<float> mA14;
bsl::vector<float> mB14;
bsl::vector<float> mN14;
const bsl::vector<float>& A14 = mA14;
const bsl::vector<float>& B14 = mB14;
const bsl::vector<float>& N14 = mN14;

bsl::vector<double> mA15;
bsl::vector<double> mB15;
bsl::vector<double> mN15;
const bsl::vector<double>& A15 = mA15;
const bsl::vector<double>& B15 = mB15;
const bsl::vector<double>& N15 = mN15;

bsl::vector<bsl::string> mA16;
bsl::vector<bsl::string> mB16;
bsl::vector<bsl::string> mN16;
const bsl::vector<bsl::string>& A16 = mA16;
const bsl::vector<bsl::string>& B16 = mB16;
const bsl::vector<bsl::string>& N16 = mN16;

bsl::vector<bdet_Datetime> mA17;
bsl::vector<bdet_Datetime> mB17;
bsl::vector<bdet_Datetime> mN17;
const bsl::vector<bdet_Datetime>& A17 = mA17;
const bsl::vector<bdet_Datetime>& B17 = mB17;
const bsl::vector<bdet_Datetime>& N17 = mN17;

bsl::vector<bdet_Date> mA18;
bsl::vector<bdet_Date> mB18;
bsl::vector<bdet_Date> mN18;
const bsl::vector<bdet_Date>& A18 = mA18;
const bsl::vector<bdet_Date>& B18 = mB18;
const bsl::vector<bdet_Date>& N18 = mN18;

bsl::vector<bdet_Time> mA19;
bsl::vector<bdet_Time> mB19;
bsl::vector<bdet_Time> mN19;
const bsl::vector<bdet_Time>& A19 = mA19;
const bsl::vector<bdet_Time>& B19 = mB19;
const bsl::vector<bdet_Time>& N19 = mN19;

bsl::vector<bdet_DatetimeTz> mA27;
bsl::vector<bdet_DatetimeTz> mB27;
bsl::vector<bdet_DatetimeTz> mN27;
const bsl::vector<bdet_DatetimeTz>& A27 = mA27;
const bsl::vector<bdet_DatetimeTz>& B27 = mB27;
const bsl::vector<bdet_DatetimeTz>& N27 = mN27;

bsl::vector<bdet_DateTz> mA28;
bsl::vector<bdet_DateTz> mB28;
bsl::vector<bdet_DateTz> mN28;
const bsl::vector<bdet_DateTz>& A28 = mA28;
const bsl::vector<bdet_DateTz>& B28 = mB28;
const bsl::vector<bdet_DateTz>& N28 = mN28;

bsl::vector<bdet_TimeTz> mA29;
bsl::vector<bdet_TimeTz> mB29;
bsl::vector<bdet_TimeTz> mN29;
const bsl::vector<bdet_TimeTz>& A29 = mA29;
const bsl::vector<bdet_TimeTz>& B29 = mB29;
const bsl::vector<bdet_TimeTz>& N29 = mN29;

bdem_List mA20;
bdem_List mB20;
bdem_List mN20;
const bdem_List& A20 = mA20;
const bdem_List& B20 = mB20;
const bdem_List& N20 = mN20;

bdem_Table mA21;
bdem_Table mB21;
bdem_Table mN21;
const bdem_Table& A21 = mA21;
const bdem_Table& B21 = mB21;
const bdem_Table& N21 = mN21;

bdem_Choice mA30;
bdem_Choice mB30;
bdem_Choice mN30;
const bdem_Choice& A30 = mA30;
const bdem_Choice& B30 = mB30;
const bdem_Choice& N30 = mN30;

bdem_ChoiceArray mA31;
bdem_ChoiceArray mB31;
bdem_ChoiceArray mN31;
const bdem_ChoiceArray& A31 = mA31;
const bdem_ChoiceArray& B31 = mB31;
const bdem_ChoiceArray& N31 = mN31;

        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        //  The following function populates static test objects with
        //  test data.  Since we do not have a control over sequence of
        //  static initializers in C++, we can not populate static test
        //  objects at the moment of construction.  The workaround is
        //  to create special function that performs required
        //  initialization.  This function 'initStaticData()' must be
        //  called from the 'main' function before any other actions.
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void initStaticData ()
{
    mA22 = true;
    mB22 = false;
    mN22 = bdetu_Unset<bool>::unsetValue();

    mA00 = 'A';
    mB00 = 'B';
    mN00 = bdetu_Unset<char>::unsetValue();

    mA01 = -1;
    mB01 = -2;
    mN01 = bdetu_Unset<short>::unsetValue();

    mA02 = 5;
    mB02 = 6;
    mN02 = bdetu_Unset<int>::unsetValue();

    mA03 = -100;
    mB03 = -200;
    mN03 = bdetu_Unset<bsls_PlatformUtil::Int64>::unsetValue();

    mA04 = -1.5;
    mB04 = -2.5;
    mN04 = bdetu_Unset<float>::unsetValue();

    mA05 = 10.5;
    mB05 = 20.5;
    mN05 = bdetu_Unset<double>::unsetValue();

    mA06 = "you";
    mB06 = "vee";
    mN06 = bdetu_Unset<bsl::string>::unsetValue();

    // Note: bdet_Datetime X07 implemented in terms of X08 and X09.

    mA08 = bdet_Date(2000,  1, 1);
    mB08 = bdet_Date(9999, 12,31);
    mN08 = bdetu_Unset<bdet_Date>::unsetValue();

    mA09 = bdet_Time(0, 1, 2, 3);
    mB09 = bdet_Time(4, 5, 6, 789);
    mN09 = bdetu_Unset<bdet_Time>::unsetValue();

    mA07 = bdet_Datetime(mA08, mA09);
    mB07 = bdet_Datetime(mB08, mB09);
    mN07 = bdetu_Unset<bdet_Datetime>::unsetValue();

    mA24 = bdet_DateTz(mA08, -5);
    mB24 = bdet_DateTz(mB08, -4);
    mN24 = bdetu_Unset<bdet_DateTz>::unsetValue();

    mA25 = bdet_TimeTz(mA09, -5);
    mB25 = bdet_TimeTz(mB09, -5);
    mN25 = bdetu_Unset<bdet_TimeTz>::unsetValue();

    mA23 = bdet_DatetimeTz(mA07, -5);
    mB23 = bdet_DatetimeTz(mB07, -5);
    mN23 = bdetu_Unset<bdet_DatetimeTz>::unsetValue();

    mA26.push_back(mA22);
    mB26.push_back(mB22);

    mA10.push_back(mA00);
    mB10.push_back(mB00);

    mA11.push_back(mA01);
    mB11.push_back(mB01);

    mA12.push_back(mA02);
    mB12.push_back(mB02);

    mA13.push_back(mA03);
    mB13.push_back(mB03);

    mA14.push_back(mA04);
    mB14.push_back(mB04);

    mA15.push_back(mA05);
    mB15.push_back(mB05);

    mA16.push_back(mA06);
    mB16.push_back(mB06);

    mA17.push_back(mA07);
    mB17.push_back(mB07);

    mA18.push_back(mA08);
    mB18.push_back(mB08);

    mA19.push_back(mA09);
    mB19.push_back(mB09);

    mA27.push_back(mA23);
    mB27.push_back(mB23);

    mA28.push_back(mA24);
    mB28.push_back(mB24);

    mA29.push_back(mA25);
    mB29.push_back(mB25);

    mA20.appendInt(mA02);
    mA20.appendDouble(mA05);
    mA20.appendString(mA06);
    mA20.appendStringArray(mA16);

    mB20.appendInt(mB02);
    mB20.appendDouble(mB05);
    mB20.appendString(mB06);
    mB20.appendStringArray(mB16);

    const ET::Type types[] =
    {
       ET::BDEM_INT,
       ET::BDEM_DOUBLE,
       ET::BDEM_STRING,
       ET::BDEM_STRING_ARRAY
    };
    const int NUM_TYPES = sizeof types / sizeof *types;

    bdem_Table t(types, NUM_TYPES);

    mA21 = t;
    mA21.appendNullRow();
    mA21.theModifiableRow(0)[0].theModifiableInt()         = mA02;
    mA21.theModifiableRow(0)[1].theModifiableDouble()      = mA05;
    mA21.theModifiableRow(0)[2].theModifiableString()      = mA06;
    mA21.theModifiableRow(0)[3].theModifiableStringArray() = mA16;

    mB21 = t;
    mB21.appendNullRow();
    mB21.theModifiableRow(0)[0].theModifiableInt()         = mB02;
    mB21.theModifiableRow(0)[1].theModifiableDouble()      = mB05;
    mB21.theModifiableRow(0)[2].theModifiableString()      = mB06;
    mB21.theModifiableRow(0)[3].theModifiableStringArray() = mB16;

    bdem_Choice c(types, NUM_TYPES);

    mA30 = c;
    mA30.makeSelection(2).theModifiableString() = mA06;

    mB30 = c;
    mB30.makeSelection(3).theModifiableStringArray() = mA16;

    bdem_ChoiceArray x(types, NUM_TYPES);

    mA31 = x;
    mA31.appendNullItems(4);
    mA31.theModifiableItem(0).makeSelection(0).theModifiableInt()       = mA02;
    mA31.theModifiableItem(1).makeSelection(1).theModifiableDouble()    = mA05;
    mA31.theModifiableItem(2).makeSelection(2).theModifiableString()    = mA06;
    mA31.theModifiableItem(3).makeSelection(3).theModifiableStringArray()=mA16;

    mB31 = x;
    mB31.appendNullItems(4);
    mB31.theModifiableItem(0).makeSelection(0).theModifiableInt()       = mB02;
    mB31.theModifiableItem(1).makeSelection(1).theModifiableDouble()    = mB05;
    mB31.theModifiableItem(2).makeSelection(2).theModifiableString()    = mB06;
    mB31.theModifiableItem(3).makeSelection(3).theModifiableStringArray()=mB16;
}

// The following code creates a 'bdem_Schema' object given a specification.
// It was copied verbatim from bdem_binding.t.cpp.  The only difference is
// that the gg() function was renamed to ggSchema().

//=============================================================================
//                        GENERATOR LANGUAGE FOR ggSchema
//=============================================================================
//
// The ggSchema function interprets a given 'spec' in order from left to right
// to build up a schema according to a custom language.  Lowercase letters [a
// .. x] specify arbitrary (but unique) names to be used for records or
// fields.  Records and fields may be unnamed; this is specified by using the
// '*' character for the name.  Field types are indicated with uppercase and
// lowercase letters [A .. Z, a .. f] for unconstrained fields, '+' for a
// constrained list, '#' for a constrained table, '%' for a constrained
// choice, and '@' for a constrained choice array.
//
// 'spec' consists of 0 or more record definitions.  (An empty spec will result
// in a valid empty schema.)  Records are specified by a leading ':' and name
// character followed by 0 or more field definitions.  A field definition
// is either an unconstrained type specifier followed by a name and 0 or more
// field attribute specifications, or a constrained field specifier followed by
// a name, a constraint specification, and 0 or more field attribute
// specifications.  A constraint specification is either the name of a
// (previously defined) record within this schema, or the 0-based index of a
// record within this schema, less than or equal to the 0-based index of the
// current record.  A field attribute specification is a 3-character sequence
// of the form "&av" where 'a' denotes a field attribute and 'v' denotes a
// value for that attribute.
//
// As may be obvious from this description, encountering a name specification
// character where a field type would have been expected indicates the start of
// a new record.  A '~' indicates that 'removeAll' is performed on the target
// schema.  The '~' is legal anywhere that a record definition could begin.
// Spaces, tabs, and newlines are allowed in the specification, but are
// ignored; they are purely cosmetic.
//
// Annotated EBNF grammar:
//  (For those unfamiliar with Extended-BNF, parentheses indicate grouping,
//  and "*" means "0 or more of the preceding".  Although this grammar does
//  not use it, EBNF also defines "+" as meaning "1 or more of the preceding"
//  and "?" as "0 or 1 of the preceding".  As in BNF, "|" separate
//  alternatives.)
//
//  Note that items following %% comment markers are not part of the grammar,
//  but are comments about semantics.
//
//  Spaces, tabs, and newlines are ignored by the parser, and are not specified
//  in this grammar although they are legal anywhere in the spec.
//
//  Spec      := ('~' | SeqRecordDef | ChoiceRecordDef | EnumerationDef)*
//
//  SeqRecordDef := ':' Name FieldDef*
//
//  ChoiceRecordDef := ':' Name '?' FieldDef*
//
//  EnumerationDef := ':' Name '=' Enumerator*
//
//  FieldDef  := (UnconstrainedFieldType Name AttrDef*)
//             | (ConstrainedFieldType Name ConstraintRef AttrDef*)
//
//  Enumerator := Name [ Index ]
//
//  Name      :=
//            '*'                   %% No name, the 0 char *
//           |'a'|'b'|'c'|'d'       %% "a".."d"
//           |'e'                   %% ""  <- An empty string, not "e"
//           |'f'|'g'|'h'|'i'|'j'   %% "f".."j"
//           |'k'|'l'|'m'|'n'|'o'   %% "k".."o"
//           |'p'|'q'|'r'|'s'|'t'   %% "p".."t"
//           |'u'|'v'|'w'|'x'       %% "you", "vee", "doubleU" "ex"
//
//  UnconstrainedFieldType :=
//            'A'|'B'|'C'|'D'|'E'   %% CHAR, SHORT, INT, INT64, FLOAT
//           |'F'|'G'|'H'|'I'|'J'   %% DOUBLE, STRING, DATETIME, DATE, TIME
//           |'K'|'L'|'M'|'N'|'O'   %% (CHAR|SHORT|INT|INT64|FLOAT)_ARRAY
//           |'P'|'Q'|'R'|'S'|'T'   %% (DOUBLE|STRING|DATETIME|DATE|TIME)_ARRAY
//           |'U'|'V'               %% LIST, TABLE
//           |'W'|'X'|'Y'|'Z'       %% BOOL, DATETIMETZ, DATETZ, TIMETZ
//           |'a'|'b'|'c'|'d'       %% (BOOL|DATETIMETZ|DATETZ|TIMETZ)_ARRAY
//           |'e'|'f'               %% CHOICE, CHOICE_ARRAY
//
//  ConstrainedFieldType :=
//            '+'|'#'|'%'|'@'       %% LIST, TABLE, CHOICE, CHOICE_ARRAY
//           |'$'|'^'               %% INT enumeration, STRING enumeration
//           |'!'|'/'               %% INT_ARRAY enum, STRING_ARRAY enum
//
//  ConstraintRef := Name | Index
//
//  %% If a constraint is referenced by index, the index is the 0-based number
//  %% of the constraint record in the schema.  Since we're limiting this to
//  %% one digit, we can't constrain by index on any record past the 10th one.
//  %% We also allow constraining by name, so this is not a big problem.
//  Index     :=
//            '0'|'1'|'2'|'3'|'4'|'5'|'6'|'7'|'8'|'9'
//
//  AttrDef   := '&' AttrKind AttrValue
//
//  AttrKind  :=
//            'D'                   %% Field default value
//           |'F'                   %% Field formatting mode
//           |'I'                   %% Field id
//           |'N'                   %% Field nullability
//
//  AttrValue :=
//             '0'|'1'                   %% AttrKind == 'D'
//            |'0'|'B'|'D'|'L'|'T'|'X'   %% AttrKind == 'F'
//            |Index                     %% AttrKind == 'I'
//            |'D'|'F'|'T'               %% AttrKind == 'N'
//
// 'gg' syntax usage examples:
//  Here are some examples, and the schema which would result when applied to
//  an empty schema.  Since our 'gg' language defines '~' for 'removeAll', all
//  'spec' strings should begin with '~' to insure expected behavior.
//
// Spec String      Result schema          Description
// -----------      -------------          -----------
// ""               (empty)                An empty string is a legal spec,
//                                         but has no effect.
//
// " \t\n  "        (empty)                Whitespace characters are ignored;
//                                         this is equivalent to "".
//
// ":a"             SCHEMA {               A schema containing a single empty
//                      RECORD "a" {       record named "a".
//                      }
//                  }
//
// ":a~"            SCHEMA {               An empty schema, since ~ performs
//                  }                      a removeAll().
//
// ":j"             SCHEMA {               A schema containing a single empty
//                      RECORD "j" {       record named "j".
//                      }
//                  }
//
// ":*"             SCHEMA {               A schema containing a single empty
//                      RECORD {           unnamed record.
//                      }
//                  }
//
// ":a:b"           SCHEMA {               A schema containing two empty
//                      RECORD "a" {       records named "a" and "b".
//                      }
//                      RECORD "b" {
//                      }
//                  }
//
// ":aFc"           SCHEMA {               A schema containing one record named
//                      RECORD "a" {       "a" with one DOUBLE field named "c".
//                          DOUBLE "c";
//                      }
//                  }
//
// ":aCa"           SCHEMA {               A schema containing one record named
//                      RECORD "a" {       "a" with one INT field named "a".
//                          INT "a";       Note that fields and records have
//                      }                  separate namespaces.
//                  }
//
// ":aG*"           SCHEMA {               A schema containing one record named
//                      RECORD "a" {       "a" with one STRING field which is
//                          STRING;        unnamed.
//                      }
//                  }
//
// ":aGbHc"         SCHEMA {               A schema containing one record named
//                      RECORD "a" {       "a" with two fields, one a STRING
//                          STRING "b";    named "b" and the other a DATETIME
//                          DATETIME "c";  named "c".
//                      }
//                  }
//
// ":aGb:cHc"       SCHEMA {               A schema containing two records.
//                      RECORD "a" {       Record "a" has one STRING field
//                          STRING "b";    named "b".
//                      }
//                      RECORD "c" {       Record "c" has one DATETIME field
//                          DATETIME "c";  named "c".
//                      }
//                  }
//
// ":aGb :cHc"      SCHEMA {               Exactly the same as the previous
//                      RECORD "a" {       example ":aGb:cHc", but more
//                          STRING "b";    readable with a space inserted
//                      }                  between the record definitions
//                      RECORD "c" {
//                          DATETIME "c";
//                      }
//                  }
//
// ":*C*F* :*G*H* :*:*"
//                  SCHEMA {               Any number of records or fields can
//                      RECORD {           be unnamed.
//                          INT;
//                          DOUBLE;
//                      }
//                      RECORD {
//                          STRING;
//                          DATETIME;
//                      }
//                      RECORD {           Empty records can be defined,
//                      }                  although this is of limited
//                      RECORD {           practical use.
//                      }
//                  }
//
// ":aFc :d+ea"     SCHEMA {               A schema containing 2 records.
//                      RECORD "a" {       Record "a" has one DOUBLE field
//                          DOUBLE "c";    named "c".
//                      }
//                      RECORD "d" {       Record "d" has one constrained LIST
//                          LIST<"a"> "";  field named "" constrained on record
//                      }                  "a".
//                  }
//
// ":aFc :d#ea"     SCHEMA {               A schema containing 2 records.
//                      RECORD "a" {       Record "a" has one DOUBLE field
//                          DOUBLE "c";    named "c".
//                      }
//                      RECORD "d" {       Record "d" has one constrained TABLE
//                          TABLE<"a"> ""; field named "" constrained on record
//                      }                  "a".
//                  }
//
// ":aFc :d#e0"     SCHEMA {               A schema containing 2 records.
//                      RECORD "a" {       Record "a" has one DOUBLE field
//                          DOUBLE "c";    named "c".
//                      }
//                      RECORD "d" {       Record "d" has one constrained TABLE
//                          TABLE<"a"> ""; field named "" constrained on record
//                      }                  "a", referring to "a" by it's index,
//                  }                      0.
//
// ":a~:a~:a~:a~"   SCHEMA {               Equivalent to "" except for
//                  }                      white-box implications.
//
// ":a=uv3 :k$fa/g0"
//     SCHEMA {                            A schema containing one enumeration
//         ENUMERATION "a" {               and one record.  Enumeration "a"
//             "you" => 0;                 has two enumerators "you" and "vee",
//             "vee" => 3;                 where "vee" has an explicit ID of 3.
//         }
//         RECORD "k" {                    Record "k" has one INT enumeration
//             INT ENUM<"a"> "f";          field named "f" and one STRING_ARRAY
//             STRING_ARRAY ENUM<"a"> "g"; enumeration field named "g", both
//         }                               of which are constrained by
//     }                                   enumeration "a".
//
//=============================================================================

const EType::Type ggElemTypes[] = {
    EType::BDEM_CHAR,             //  0
    EType::BDEM_SHORT,            //  1
    EType::BDEM_INT,              //  2
    EType::BDEM_INT64,            //  3
    EType::BDEM_FLOAT,            //  4
    EType::BDEM_DOUBLE,           //  5
    EType::BDEM_STRING,           //  6
    EType::BDEM_DATETIME,         //  7
    EType::BDEM_DATE,             //  8
    EType::BDEM_TIME,             //  9
    EType::BDEM_CHAR_ARRAY,       // 10
    EType::BDEM_SHORT_ARRAY,      // 11
    EType::BDEM_INT_ARRAY,        // 12
    EType::BDEM_INT64_ARRAY,      // 13
    EType::BDEM_FLOAT_ARRAY,      // 14
    EType::BDEM_DOUBLE_ARRAY,     // 15
    EType::BDEM_STRING_ARRAY,     // 16
    EType::BDEM_DATETIME_ARRAY,   // 17
    EType::BDEM_DATE_ARRAY,       // 18
    EType::BDEM_TIME_ARRAY,       // 19
    EType::BDEM_LIST,             // 20
    EType::BDEM_TABLE,            // 21
    EType::BDEM_BOOL,             // 22
    EType::BDEM_DATETIMETZ,       // 23
    EType::BDEM_DATETZ,           // 24
    EType::BDEM_TIMETZ,           // 25
    EType::BDEM_BOOL_ARRAY,       // 26
    EType::BDEM_DATETIMETZ_ARRAY, // 27
    EType::BDEM_DATETZ_ARRAY,     // 28
    EType::BDEM_TIMETZ_ARRAY,     // 29
    EType::BDEM_CHOICE,           // 30
    EType::BDEM_CHOICE_ARRAY,     // 31
    EType::BDEM_LIST,             // 32 (constrained, but type still 'LIST')
    EType::BDEM_TABLE,            // 33 (constrained, but type still 'TABLE')
    EType::BDEM_CHOICE,           // 34 (constrained, but type still 'CHOICE')
    EType::BDEM_CHOICE_ARRAY,     // 35 (constrained, but type still
                                  //     'CHOICE_ARRAY')
    EType::BDEM_INT,              // 36 (enumeration, but type still 'INT')
    EType::BDEM_STRING,           // 37 (enumeration, but type still 'STRING')
    EType::BDEM_INT_ARRAY,        // 38 (enumeration, but type still
                                  //     'INT_ARRAY')
    EType::BDEM_STRING_ARRAY      // 39 (enumeration, but type still
                                  //     'STRING_ARRAY')
};

const int NUM_GG_TYPES = sizeof ggElemTypes / sizeof *ggElemTypes;

static const char *ggElemTypeNames[] = {
    "bdem_ElemType::BDEM_CHAR",                       //  0
    "bdem_ElemType::BDEM_SHORT",                      //  1
    "bdem_ElemType::BDEM_INT",                        //  2
    "bdem_ElemType::BDEM_INT64",                      //  3
    "bdem_ElemType::BDEM_FLOAT",                      //  4
    "bdem_ElemType::BDEM_DOUBLE",                     //  5
    "bdem_ElemType::BDEM_STRING",                     //  6
    "bdem_ElemType::BDEM_DATETIME",                   //  7
    "bdem_ElemType::BDEM_DATE",                       //  8
    "bdem_ElemType::BDEM_TIME",                       //  9
    "bdem_ElemType::BDEM_CHAR_ARRAY",                 // 10
    "bdem_ElemType::BDEM_SHORT_ARRAY",                // 11
    "bdem_ElemType::BDEM_INT_ARRAY",                  // 12
    "bdem_ElemType::BDEM_INT64_ARRAY",                // 13
    "bdem_ElemType::BDEM_FLOAT_ARRAY",                // 14
    "bdem_ElemType::BDEM_DOUBLE_ARRAY",               // 15
    "bdem_ElemType::BDEM_STRING_ARRAY",               // 16
    "bdem_ElemType::BDEM_DATETIME_ARRAY",             // 17
    "bdem_ElemType::BDEM_DATE_ARRAY",                 // 18
    "bdem_ElemType::BDEM_TIME_ARRAY",                 // 19
    "bdem_ElemType::BDEM_LIST",                       // 20
    "bdem_ElemType::BDEM_TABLE",                      // 21
    "bdem_ElemType::BDEM_BOOL",                       // 22
    "bdem_ElemType::BDEM_DATETIMETZ",                 // 23
    "bdem_ElemType::BDEM_DATETZ",                     // 24
    "bdem_ElemType::BDEM_TIMETZ",                     // 25
    "bdem_ElemType::BDEM_BOOL_ARRAY",                 // 26
    "bdem_ElemType::BDEM_DATETIMETZ_ARRAY",           // 27
    "bdem_ElemType::BDEM_DATETZ_ARRAY",               // 28
    "bdem_ElemType::BDEM_TIMETZ_ARRAY",               // 29
    "bdem_ElemType::BDEM_CHOICE",                     // 30
    "bdem_ElemType::BDEM_CHOICE_ARRAY",               // 31
    "bdem_ElemType::BDEM_LIST (constrained)",         // 32
    "bdem_ElemType::BDEM_TABLE (constrainted)",       // 33
    "bdem_ElemType::BDEM_CHOICE (constrained)",       // 34
    "bdem_ElemType::BDEM_CHOICE_ARRAY (constrainted)",// 35
    "bdem_ElemType::BDEM_INT (enumeration)",          // 36
    "bdem_ElemType::BDEM_STRING (enumeration)",       // 37
    "bdem_ElemType::BDEM_INT_ARRAY (enumeration)",    // 38
    "bdem_ElemType::BDEM_STRING_ARRAY (enumeration)"  // 39
};

BSLMF_ASSERT(NUM_GG_TYPES == sizeof ggElemTypeNames / sizeof *ggElemTypeNames);

//=============================================================================
//                ggSchema HELPER DATA AND FUNCTIONS
//=============================================================================

// NOTE: 'index' is used in string.h on AIX so switched to indexStr
const char removeTilde[]    = "~";
const char name[]           = "*abcdefghijklmnopqrstuvwx";
const char indexStr[]       = "0123456789";
const char bdemType[]       = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef";
const char constrBdemType[] = "+#%@$^!/";  // Note: Includes terminal '\0'

const int NUM_GG_NAMES     = sizeof name           / sizeof(char) - 1;
const int NUM_CONSTR_TYPES = sizeof constrBdemType / sizeof(char) - 1;

int isName(char token)
{
    return '\0' != token && 0 != bsl::strchr(name, token);
}

int isConstrainedFieldType(char token)
{
    return '\0' != token && 0 != bsl::strchr(constrBdemType, token);
}

int isUnconstrainedFieldType(char token)
{
    return '\0' != token && 0 != bsl::strchr(bdemType, token);
}

int isFieldType(char token)
{
    return isUnconstrainedFieldType(token) || isConstrainedFieldType(token);
}

const char *getName(char nameCode)
    // Return the name corresponding to the specified 'nameCode'.
    //..
    //  '*'      => 0        (a null name)
    //  'a'-'d'  => "a"-"d"
    //  'e'      => ""       (an empty name)
    //  'f'-'t'  => "f"-"t"
    //  'u'      => "you"
    //  'v'      => "vee"
    //  'w'      => "doubleU"
    //  'x'      => "ex"
    //..
{
    switch (nameCode) {
      case '*': return 0;    // Note that name code '*' is the null string.
      case 'a': return "a";
      case 'b': return "b";
      case 'c': return "c";
      case 'd': return "d";
      case 'e': return "";   // Note that name code 'e' is the empty string.
      case 'f': return "f";
      case 'g': return "g";
      case 'h': return "h";
      case 'i': return "i";
      case 'j': return "j";
      case 'k': return "k";
      case 'l': return "l";
      case 'm': return "m";
      case 'n': return "n";
      case 'o': return "o";
      case 'p': return "p";
      case 'q': return "q";
      case 'r': return "r";
      case 's': return "s";
      case 't': return "t";
      case 'u': return "you";
      case 'v': return "vee";
      case 'w': return "doubleU";
      case 'x': return "ex";
      default: {
        P(nameCode);  ASSERT("Invalid name used in gg script" && 0);
      } break;
    }

    return 0;
}

EType::Type getType(char typeCode)
    // Return the element type corresponding to the specified 'typeCode'.
    //..
    //  'A' => CHAR        'K' => CHAR_ARRAY
    //  'B' => SHORT       'L' => SHORT_ARRAY
    //  'C' => INT         'M' => INT_ARRAY
    //  'D' => INT64       'N' => INT64_ARRAY
    //  'E' => FLOAT       'O' => FLOAT_ARRAY
    //  'F' => DOUBLE      'P' => DOUBLE_ARRAY
    //  'G' => STRING      'Q' => STRING_ARRAY
    //  'H' => DATETIME    'R' => DATETIME_ARRAY
    //  'I' => DATE        'S' => DATE_ARRAY
    //  'J' => TIME        'T' => TIME_ARRAY
    //
    //  'U' => LIST        'V' => TABLE         (used for unconstrained fields)
    //
    //  'W' => BOOL        'a' => BOOL_ARRAY
    //  'X' => DATETIMETZ  'b' => DATETIMETZ_ARRAY
    //  'Y' => DATETZ      'c' => DATETZ_ARRAY
    //  'Z' => TIMETZ      'd' => TIMETZ_ARRAY
    //
    //  'e' => CHOICE      'f' => CHOICE_ARRAY  (used for unconstrained fields)
    //
    //  '+' => LIST        '#' => TABLE         (used for constrained fields)
    //  '%' => CHOICE      '@' => CHOICE_ARRAY  (used for constrained fields)
    //  '$' => INT         '^' => STRING        (used for enumeration fields)
    //  '!' => INT_ARRAY   '/' => STRING_ARRAY  (used for enumeration fields)
    //..
{
    switch (typeCode) {
      case 'A': return EType::BDEM_CHAR;
      case 'B': return EType::BDEM_SHORT;
      case 'C': return EType::BDEM_INT;
      case 'D': return EType::BDEM_INT64;
      case 'E': return EType::BDEM_FLOAT;
      case 'F': return EType::BDEM_DOUBLE;
      case 'G': return EType::BDEM_STRING;
      case 'H': return EType::BDEM_DATETIME;
      case 'I': return EType::BDEM_DATE;
      case 'J': return EType::BDEM_TIME;
      case 'K': return EType::BDEM_CHAR_ARRAY;
      case 'L': return EType::BDEM_SHORT_ARRAY;
      case 'M': return EType::BDEM_INT_ARRAY;
      case 'N': return EType::BDEM_INT64_ARRAY;
      case 'O': return EType::BDEM_FLOAT_ARRAY;
      case 'P': return EType::BDEM_DOUBLE_ARRAY;
      case 'Q': return EType::BDEM_STRING_ARRAY;
      case 'R': return EType::BDEM_DATETIME_ARRAY;
      case 'S': return EType::BDEM_DATE_ARRAY;
      case 'T': return EType::BDEM_TIME_ARRAY;
      case 'U': return EType::BDEM_LIST;
      case 'V': return EType::BDEM_TABLE;
      case 'W': return EType::BDEM_BOOL;
      case 'X': return EType::BDEM_DATETIMETZ;
      case 'Y': return EType::BDEM_DATETZ;
      case 'Z': return EType::BDEM_TIMETZ;
      case 'a': return EType::BDEM_BOOL_ARRAY;
      case 'b': return EType::BDEM_DATETIMETZ_ARRAY;
      case 'c': return EType::BDEM_DATETZ_ARRAY;
      case 'd': return EType::BDEM_TIMETZ_ARRAY;
      case 'e': return EType::BDEM_CHOICE;
      case 'f': return EType::BDEM_CHOICE_ARRAY;

      case '+': return EType::BDEM_LIST;
      case '#': return EType::BDEM_TABLE;
      case '%': return EType::BDEM_CHOICE;
      case '@': return EType::BDEM_CHOICE_ARRAY;
      case '$': return EType::BDEM_INT;
      case '^': return EType::BDEM_STRING;
      case '!': return EType::BDEM_INT_ARRAY;
      case '/': return EType::BDEM_STRING_ARRAY;
      default: {
        P(typeCode);  ASSERT("Invalid element type used in gg script" && 0);
      } break;
    }

    return EType::BDEM_INT;
}

const RecDef *getRecordConstraint(Schema *object, char token)
{
    if (bsl::strchr(indexStr, token)) {
        // constrained by index
        return &object->record(token - '0');
    }

    // else constrained by name
    return object->lookupRecord(getName(token));
}

const EnumDef *getEnumConstraint(Schema *object, char token)
{
    if (bsl::strchr(indexStr, token)) {
        // constrained by index
        return &object->enumeration(token - '0');
    }

    // else constrained by name
    return object->lookupEnumeration(getName(token));
}

int getFormattingMode(char fmtCode)
    // Return the formatting mode corresponding to the specified 'fmtCode'.
    //..
    //  '0' => DEFAULT (zero, not "oh")
    //  'B' => BASE64
    //  'D' => DEC
    //  'T' => TEXT
    //  'X' => HEX
    //  'L' => IS_LIST
    //  'N' => IS_NILLABLE
    //  'S' => IS_SIMPLE_CONTENT;
    //  'U' => IS_UNTAGGED
    //..
{
    switch (fmtCode) {
      case '0': return Format::BDEAT_DEFAULT;
      case 'B': return Format::BDEAT_BASE64;
      case 'D': return Format::BDEAT_DEC;
      case 'T': return Format::BDEAT_TEXT;
      case 'X': return Format::BDEAT_HEX;
      case 'A': return Format::BDEAT_ATTRIBUTE;
      case 'L': return Format::BDEAT_LIST;
      case 'N':
      case 'n': return Format::BDEAT_NILLABLE;
      case 'S': return Format::BDEAT_SIMPLE_CONTENT;
      case 'U': return Format::BDEAT_UNTAGGED;
      default: {
        P(fmtCode);  ASSERT("Invalid formatting mode used in gg script" && 0);
      } break;
    }

    return 0;
}

bool getNullability(char nullCode, EType::Type type)
    // Return the nullability value corresponding to the specified 'nullCode'
    // and 'bdem' element 'type'.  Note that 'type' is ignored unless the
    // default nullability is specified.
    //..
    //  'D' => default for 'type'
    //  'F' => false
    //  'T' => true
    //..
{
    switch (nullCode) {
      case 'D': return EType::BDEM_LIST == type ? false : true;
      case 'F': return false;
      case 'T': return true;
      default: {
        P(nullCode);  ASSERT("Invalid nullability used in gg script" && 0);
      } break;
    }

    return true;
}

void setDefaultValueForType(ERef ref, bool other = false)
    // Set the value of the element referenced by the specified 'ref' to a
    // non-null value appropriate for its type.  If the optionally-specified
    // 'other' flag is 'true', set the value of the element referenced by 'ref'
    // to an alternate non-null value.  The behavior is undefined unless the
    // type of the element referenced by 'ref' is a scalar 'bdem' type.
{
    switch (ref.type()) {
      case EType::BDEM_CHAR: {
        ref.theModifiableChar()       = other ? B00 : A00;
      } break;
      case EType::BDEM_SHORT: {
        ref.theModifiableShort()      = other ? B01 : A01;
      } break;
      case EType::BDEM_INT: {
        ref.theModifiableInt()        = other ? B02 : A02;
      } break;
      case EType::BDEM_INT64: {
        ref.theModifiableInt64()      = other ? B03 : A03;
      } break;
      case EType::BDEM_FLOAT: {
        ref.theModifiableFloat()      = other ? B04 : A04;
      } break;
      case EType::BDEM_DOUBLE: {
        ref.theModifiableDouble()     = other ? B05 : A05;
      } break;
      case EType::BDEM_STRING: {
        ref.theModifiableString()     = other ? B06 : A06;
      } break;
      case EType::BDEM_DATETIME: {
        ref.theModifiableDatetime()   = other ? B07 : A07;
      } break;
      case EType::BDEM_DATE: {
        ref.theModifiableDate()       = other ? B08 : A08;
      } break;
      case EType::BDEM_TIME: {
        ref.theModifiableTime()       = other ? B09 : A09;
      } break;
      case EType::BDEM_BOOL: {
        ref.theModifiableBool()       = other ? B22 : A22;
      } break;
      case EType::BDEM_DATETIMETZ: {
        ref.theModifiableDatetimeTz() = other ? B23 : A23;
      } break;
      case EType::BDEM_DATETZ: {
        ref.theModifiableDateTz()     = other ? B24 : A24;
      } break;
      case EType::BDEM_TIMETZ: {
        ref.theModifiableTimeTz()     = other ? B25 : A25;
      } break;
      default: {
        ASSERT("Invalid element passed to 'setDefaultValueForType'" && 0);
      } break;
    }
}

bool verifyDefaultValueForType(CERef ref, bool other = false)
    // Return 'true' if the value of the non-modifiable element referenced by
    // the specified 'ref' has the first non-null exemplar value, or the
    // optionally-specified 'other' flag is 'true' and the value of the element
    // referenced by 'ref' is of the alternate non-null exemplar value; return
    // 'false' otherwise.  The behavior is undefined unless the type of the
    // element referenced by 'ref' is a scalar 'bdem' type.
{
    switch (ref.type()) {
      case EType::BDEM_CHAR: {
        return (other ? B00 : A00) == ref.theChar();
      } break;
      case EType::BDEM_SHORT: {
        return (other ? B01 : A01) == ref.theShort();
      } break;
      case EType::BDEM_INT: {
        return (other ? B02 : A02) == ref.theInt();
      } break;
      case EType::BDEM_INT64: {
        return (other ? B03 : A03) == ref.theInt64();
      } break;
      case EType::BDEM_FLOAT: {
        return (other ? B04 : A04) == ref.theFloat();
      } break;
      case EType::BDEM_DOUBLE: {
        return (other ? B05 : A05) == ref.theDouble();
      } break;
      case EType::BDEM_STRING: {
        return (other ? B06 : A06) == ref.theString();
      } break;
      case EType::BDEM_DATETIME: {
        return (other ? B07 : A07) == ref.theDatetime();
      } break;
      case EType::BDEM_DATE: {
        return (other ? B08 : A08) == ref.theDate();
      } break;
      case EType::BDEM_TIME: {
        return (other ? B09 : A09) == ref.theTime();
      } break;
      case EType::BDEM_BOOL: {
        return (other ? B22 : A22) == ref.theBool();
      } break;
      case EType::BDEM_DATETIMETZ: {
        return (other ? B23 : A23) == ref.theDatetimeTz();
      } break;
      case EType::BDEM_DATETZ: {
        return (other ? B24 : A24) == ref.theDateTz();
      } break;
      case EType::BDEM_TIMETZ: {
        return (other ? B25 : A25) == ref.theTimeTz();
      } break;
      default: {
        ASSERT("Invalid element passed to 'verifyDefaultValueForType'" && 0);
      } break;
    }

    return false;
}

const char *addRecord(Schema     *object,
                      const char *recordName,
                      RecType     recordType,
                      const char *p)
{
    RecDef *record = object->createRecord(recordName, recordType);
    ASSERT(record);

    while (isFieldType(*p)) {       // add a field
        const char         fieldTypeChar = *p++;
        EType::Type        fieldType     = getType(fieldTypeChar);
        ASSERT(isName(*p));
        const char        *fieldName = getName(*p++);

        const RecDef  *recordConstraint = 0;
        const EnumDef *enumConstraint = 0;
        if (isConstrainedFieldType(fieldTypeChar)) {
            if (EType::isAggregateType(fieldType)) {
                recordConstraint = getRecordConstraint(object, *p++);
                ASSERT(recordConstraint);
            }
            else {
                ASSERT(EnumDef::canHaveEnumConstraint(fieldType));
                enumConstraint = getEnumConstraint(object, *p++);
                ASSERT(enumConstraint);
            }
        }

        // Parse field attributes, if any.

        int  fmt       = Format::BDEAT_DEFAULT;
        bool nullable  = false;
        int  dfltIndex = -1;
        int  id        = BDEM_NULL_FIELD_ID;

        while ('&' == *p) {
            const char attrCode  = *++p;
            const char attrValue = *++p;
            switch (attrCode) {
              case 'D': {
                  ASSERT('0' <= attrValue && attrValue <= '1');
                  dfltIndex = attrValue - '0';
              } break;
              case 'F': {
                  fmt |= getFormattingMode(attrValue);
              } break;
              case 'I': {
                  ASSERT('0' <= attrValue && attrValue <= '9');
                  id = attrValue - '0';
              } break;
              case 'N': {
                  nullable = getNullability(attrValue, fieldType);
              } break;
              default: {
                  P(attrCode);
                  ASSERT("Invalid attribute code used in gg script");
              } break;
            }
            ++p;
        }

        if (-1 != dfltIndex && nullable) {
            nullable = false;
        }

        bool isNSA = false;
        if (bdeat_FormattingMode::BDEAT_NILLABLE & fmt
         && EType::isArrayType(fieldType)
         && !EType::isAggregateType(fieldType)
         && -1 == dfltIndex) {
            bdem_RecordDef *tmpRecord = object->createRecord(
                                         0,
                                         bdem_RecordDef::BDEM_SEQUENCE_RECORD);
            bdem_ElemType::Type baseType = EType::fromArrayType(fieldType);
            bdem_FieldDefAttributes baseAttr(
                                       baseType,
                                       false,
                                       bdeat_FormattingMode::BDEAT_NILLABLE);
            tmpRecord->appendField(baseAttr, enumConstraint);
            recordConstraint = tmpRecord;
            fieldType = EType::BDEM_TABLE;
            isNSA = true;
        }

        FldAttr attr(fieldType, nullable, fmt);

        if (-1 != dfltIndex) {
            ASSERT(! EType::isAggregateType(fieldType));
            setDefaultValueForType(attr.defaultValue(), dfltIndex);
        }

        if (enumConstraint && !isNSA) {
            const FldDef *field = record->appendField(attr,
                                                      enumConstraint,
                                                      fieldName,
                                                      id);
            ASSERT(field);
        }
        else {
            const FldDef *field = record->appendField(attr,
                                                      recordConstraint,
                                                      fieldName,
                                                      id);
            ASSERT(field);
        }
    }

    return p;
}

const char *addEnumeration(Schema *object, const char* enumName, const char *p)
{
    EnumDef *enumdef = object->createEnumeration(enumName);
    ASSERT(enumdef);

    while (isName(*p)) {
        const char* enumeratorName = getName(*p++);
        ASSERT(enumeratorName);
        if ('0' <= *p && *p <= '9') {
            int enumeratorId = *p++ - '0';
            enumdef->addEnumerator(enumeratorName, enumeratorId);
        }
        else {
            enumdef->addEnumerator(enumeratorName);
        }
    }

    return p;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

Schema& ggSchema(Schema *object, const char *spec)
    // Configure the specified schema 'object' according to the specified
    // 'spec' using the primary manipulators and accessors of this component
    // and return a reference to 'object.  Note that normal usage is that
    // 'object' is an empty schema, but the manipulators will be applied to
    // 'object' in whatever state it is in.
{
    enum { MAX_SPEC_LEN = 20000 };
    ASSERT(bsl::strlen(spec) < MAX_SPEC_LEN);

    char cleanSpec[MAX_SPEC_LEN];  // spec after removing all white spaces

    int i = 0;
    for (int j = 0; spec[j]; ++j) {
        if (!isspace(spec[j])) {
            cleanSpec[i++] = spec[j];
        }
    }
    cleanSpec[i] = '\0';

    const char *p = cleanSpec;
    while (*p) {
        if ('~' == *p) {                    // removeAll
            object->removeAll();
            ++p;
        }
        else {
            ASSERT(':' == *p);              // create a record
            const char N = *++p;            // record name index
            ASSERT(isName(N));
            ++p;

            if ('?' == *p) {
                ++p;
                p = addRecord(object, getName(N),
                              RecDef::BDEM_CHOICE_RECORD, p);
            }
            else if ('=' == *p) {
                ++p;
                p = addEnumeration(object, getName(N), p);
            }
            else {
                p = addRecord(object, getName(N),
                              RecDef::BDEM_SEQUENCE_RECORD, p);
            }
        }
    }

    return *object;
}

//=============================================================================
//            One Last Helper That Uses a 'ggSchema' Helper
//-----------------------------------------------------------------------------

static
const char *csName(int index)
    // Return the 'ggSchema' test name corresponding to the specified
    // 'index' if index < NUM_GG_NAMES, and 0 otherwise.
{
    return index < NUM_GG_NAMES ? getName(name[index]) : 0;
}

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

static bool streq(const char *a, const char *b)
    // Return true if null-terminated strings 'a' and 'b' are equal.  Returns
    // true if both 'a' and 'b' are null and false if either 'a' or 'b', but
    // not both, are null.
{
    if (a == b) {
        return true;
    }
    else if (0 == a || 0 == b) {
        return false;
    }
    else {
        return 0 == bsl::strcmp(a, b);
    }
}

static
const char *recName(const bdem_RecordDef& record)
    // Return the name of the specified 'record'.  Note that 'record' does not
    // know its name.  Rather, 'record.schema()' accesses its parent schema,
    // which in turn has a 'recordName' method.  This helper just implements
    // less-convenient 'bdem_schema' functionality.
{
    return record.schema().recordName(record.recordIndex());
}

const char *enumName(const EnumDef& en)
    // Return the name of the specified 'en'.  Note that 'en' does not know
    // its name.  Rather, 'en.schema()' accesses its parent schema, which in
    // turn has an 'enumerationName' method.  This helper just implements
    // less-convenient 'bdem_schema' functionality.
{
    return en.schema().enumerationName(en.enumerationIndex());
}

static const CERef VALUES_A[] =
    // This array stores a set of values corresponding to each element
    // type specified in the spec string.
{
    CERef((const void *) &A00, EAL::lookupTable()[0]),
    CERef((const void *) &A01, EAL::lookupTable()[1]),
    CERef((const void *) &A02, EAL::lookupTable()[2]),
    CERef((const void *) &A03, EAL::lookupTable()[3]),
    CERef((const void *) &A04, EAL::lookupTable()[4]),
    CERef((const void *) &A05, EAL::lookupTable()[5]),
    CERef((const void *) &A06, EAL::lookupTable()[6]),
    CERef((const void *) &A07, EAL::lookupTable()[7]),
    CERef((const void *) &A08, EAL::lookupTable()[8]),
    CERef((const void *) &A09, EAL::lookupTable()[9]),
    CERef((const void *) &A10, EAL::lookupTable()[10]),
    CERef((const void *) &A11, EAL::lookupTable()[11]),
    CERef((const void *) &A12, EAL::lookupTable()[12]),
    CERef((const void *) &A13, EAL::lookupTable()[13]),
    CERef((const void *) &A14, EAL::lookupTable()[14]),
    CERef((const void *) &A15, EAL::lookupTable()[15]),
    CERef((const void *) &A16, EAL::lookupTable()[16]),
    CERef((const void *) &A17, EAL::lookupTable()[17]),
    CERef((const void *) &A18, EAL::lookupTable()[18]),
    CERef((const void *) &A19, EAL::lookupTable()[19]),
    CERef((const void *) &A20, EAL::lookupTable()[20]),
    CERef((const void *) &A21, EAL::lookupTable()[21]),
    CERef((const void *) &A22, EAL::lookupTable()[22]),
    CERef((const void *) &A23, EAL::lookupTable()[23]),
    CERef((const void *) &A24, EAL::lookupTable()[24]),
    CERef((const void *) &A25, EAL::lookupTable()[25]),
    CERef((const void *) &A26, EAL::lookupTable()[26]),
    CERef((const void *) &A27, EAL::lookupTable()[27]),
    CERef((const void *) &A28, EAL::lookupTable()[28]),
    CERef((const void *) &A29, EAL::lookupTable()[29]),
    CERef((const void *) &A30, EAL::lookupTable()[30]),
    CERef((const void *) &A31, EAL::lookupTable()[31]),
};
const int NUM_VALUESA = sizeof VALUES_A / sizeof *VALUES_A;

static const CERef VALUES_B[] =
    // This array stores a set of values corresponding to each element
    // type specified in the spec string.
{
    CERef((const void *) &B00, EAL::lookupTable()[0]),
    CERef((const void *) &B01, EAL::lookupTable()[1]),
    CERef((const void *) &B02, EAL::lookupTable()[2]),
    CERef((const void *) &B03, EAL::lookupTable()[3]),
    CERef((const void *) &B04, EAL::lookupTable()[4]),
    CERef((const void *) &B05, EAL::lookupTable()[5]),
    CERef((const void *) &B06, EAL::lookupTable()[6]),
    CERef((const void *) &B07, EAL::lookupTable()[7]),
    CERef((const void *) &B08, EAL::lookupTable()[8]),
    CERef((const void *) &B09, EAL::lookupTable()[9]),
    CERef((const void *) &B10, EAL::lookupTable()[10]),
    CERef((const void *) &B11, EAL::lookupTable()[11]),
    CERef((const void *) &B12, EAL::lookupTable()[12]),
    CERef((const void *) &B13, EAL::lookupTable()[13]),
    CERef((const void *) &B14, EAL::lookupTable()[14]),
    CERef((const void *) &B15, EAL::lookupTable()[15]),
    CERef((const void *) &B16, EAL::lookupTable()[16]),
    CERef((const void *) &B17, EAL::lookupTable()[17]),
    CERef((const void *) &B18, EAL::lookupTable()[18]),
    CERef((const void *) &B19, EAL::lookupTable()[19]),
    CERef((const void *) &B20, EAL::lookupTable()[20]),
    CERef((const void *) &B21, EAL::lookupTable()[21]),
    CERef((const void *) &B22, EAL::lookupTable()[22]),
    CERef((const void *) &B23, EAL::lookupTable()[23]),
    CERef((const void *) &B24, EAL::lookupTable()[24]),
    CERef((const void *) &B25, EAL::lookupTable()[25]),
    CERef((const void *) &B26, EAL::lookupTable()[26]),
    CERef((const void *) &B27, EAL::lookupTable()[27]),
    CERef((const void *) &B28, EAL::lookupTable()[28]),
    CERef((const void *) &B29, EAL::lookupTable()[29]),
    CERef((const void *) &B30, EAL::lookupTable()[30]),
    CERef((const void *) &B31, EAL::lookupTable()[31]),
};
const int NUM_VALUESB = sizeof VALUES_B / sizeof *VALUES_B;

static const CERef VALUES_N[] =
    // This array stores a set of values corresponding to each element
    // type specified in the spec string.
{
    CERef((const void *) &N00, EAL::lookupTable()[0]),
    CERef((const void *) &N01, EAL::lookupTable()[1]),
    CERef((const void *) &N02, EAL::lookupTable()[2]),
    CERef((const void *) &N03, EAL::lookupTable()[3]),
    CERef((const void *) &N04, EAL::lookupTable()[4]),
    CERef((const void *) &N05, EAL::lookupTable()[5]),
    CERef((const void *) &N06, EAL::lookupTable()[6]),
    CERef((const void *) &N07, EAL::lookupTable()[7]),
    CERef((const void *) &N08, EAL::lookupTable()[8]),
    CERef((const void *) &N09, EAL::lookupTable()[9]),
    CERef((const void *) &N10, EAL::lookupTable()[10]),
    CERef((const void *) &N11, EAL::lookupTable()[11]),
    CERef((const void *) &N12, EAL::lookupTable()[12]),
    CERef((const void *) &N13, EAL::lookupTable()[13]),
    CERef((const void *) &N14, EAL::lookupTable()[14]),
    CERef((const void *) &N15, EAL::lookupTable()[15]),
    CERef((const void *) &N16, EAL::lookupTable()[16]),
    CERef((const void *) &N17, EAL::lookupTable()[17]),
    CERef((const void *) &N18, EAL::lookupTable()[18]),
    CERef((const void *) &N19, EAL::lookupTable()[19]),
    CERef((const void *) &N20, EAL::lookupTable()[20]),
    CERef((const void *) &N21, EAL::lookupTable()[21]),
    CERef((const void *) &N22, EAL::lookupTable()[22]),
    CERef((const void *) &N23, EAL::lookupTable()[23]),
    CERef((const void *) &N24, EAL::lookupTable()[24]),
    CERef((const void *) &N25, EAL::lookupTable()[25]),
    CERef((const void *) &N26, EAL::lookupTable()[26]),
    CERef((const void *) &N27, EAL::lookupTable()[27]),
    CERef((const void *) &N28, EAL::lookupTable()[28]),
    CERef((const void *) &N29, EAL::lookupTable()[29]),
    CERef((const void *) &N30, EAL::lookupTable()[30]),
    CERef((const void *) &N31, EAL::lookupTable()[31]),
};
const int NUM_VALUESN = sizeof VALUES_N / sizeof *VALUES_N;

struct  tempStruct {
    static int assertion2[sizeof bdemType - 1 == NUM_VALUESA];
    static int assertion3[sizeof bdemType - 1 == NUM_VALUESB];
    static int assertion4[sizeof bdemType - 1 == NUM_VALUESN];
};

CERef getCERef(ET::Type type, int value)
    // Return the const elem ref value corresponding to the specified 'value'
    // (one of 'A', 'B', 'N') of the specified 'type'.
{
    switch (value) {
      case 0: return VALUES_N[(int) type];
      case 1: return VALUES_A[(int) type];
      case 2: return VALUES_B[(int) type];
      default: ASSERT(0);
    }
    return CERef(0, 0);
}

ERef getERef(ET::Type type, int value)
    // Return the elem ref value corresponding to the specified 'value'
    // (one of 'A', 'B', 'N') of the specified 'type'.
{
    CERef T = getCERef(type, value);
    return ERef((void *) T.data(), T.descriptor());
}

bool isUnset(const CERef& ref)
    // Return 'true' if the specified element 'ref' references an element that
    // has the "unset" value for its type, and 'false' otherwise.
{
    return ref.descriptor()->isUnset(ref.data());
}

//=============================================================================
//       HELPER FUNCTIONS THAT FACILITATE CREATION OF A LIST OR TABLE
//=============================================================================

void addElementToList(bdem_List *list, ET::Type type)
    // Append a data element of the specified 'type' to the specified 'list'.
{
    const int MIN_ENTRIES = 3;
    int randNum = bsl::rand() % MIN_ENTRIES;
    CERef elemRef = getCERef(type, randNum);

    switch(type) {
      case ET::BDEM_CHAR: {
        list->appendChar(elemRef.theChar());
      } break;
      case ET::BDEM_SHORT: {
        list->appendShort(elemRef.theShort());
      } break;
      case ET::BDEM_INT: {
        list->appendInt(elemRef.theInt());
      } break;
      case ET::BDEM_INT64: {
        list->appendInt64(elemRef.theInt64());
      } break;
      case ET::BDEM_FLOAT: {
        list->appendFloat(elemRef.theFloat());
      } break;
      case ET::BDEM_DOUBLE: {
        list->appendDouble(elemRef.theDouble());
      } break;
      case ET::BDEM_STRING: {
        list->appendString(elemRef.theString());
      } break;
      case ET::BDEM_DATETIME: {
        list->appendDatetime(elemRef.theDatetime());
      } break;
      case ET::BDEM_DATE: {
        list->appendDate(elemRef.theDate());
      } break;
      case ET::BDEM_TIME: {
        list->appendTime(elemRef.theTime());
      } break;
      case ET::BDEM_CHAR_ARRAY: {
        list->appendCharArray(elemRef.theCharArray());
      } break;
      case ET::BDEM_SHORT_ARRAY: {
        list->appendShortArray(elemRef.theShortArray());
      } break;
      case ET::BDEM_INT_ARRAY: {
        list->appendIntArray(elemRef.theIntArray());
      } break;
      case ET::BDEM_INT64_ARRAY: {
        list->appendInt64Array(elemRef.theInt64Array());
      } break;
      case ET::BDEM_FLOAT_ARRAY: {
        list->appendFloatArray(elemRef.theFloatArray());
      } break;
      case ET::BDEM_DOUBLE_ARRAY: {
        list->appendDoubleArray(elemRef.theDoubleArray());
      } break;
      case ET::BDEM_STRING_ARRAY: {
        list->appendStringArray(elemRef.theStringArray());
      } break;
      case ET::BDEM_TIME_ARRAY: {
        list->appendTimeArray(elemRef.theTimeArray());
      } break;
      case ET::BDEM_DATE_ARRAY: {
        list->appendDateArray(elemRef.theDateArray());
      } break;
      case ET::BDEM_DATETIME_ARRAY: {
        list->appendDatetimeArray(elemRef.theDatetimeArray());
      } break;
      case ET::BDEM_BOOL: {
        list->appendBool(elemRef.theBool());
      } break;
      case ET::BDEM_DATETIMETZ: {
        list->appendDatetimeTz(elemRef.theDatetimeTz());
      } break;
      case ET::BDEM_DATETZ: {
        list->appendDateTz(elemRef.theDateTz());
      } break;
      case ET::BDEM_TIMETZ: {
        list->appendTimeTz(elemRef.theTimeTz());
      } break;
      case ET::BDEM_BOOL_ARRAY: {
        list->appendBoolArray(elemRef.theBoolArray());
      } break;
      case ET::BDEM_DATETIMETZ_ARRAY: {
        list->appendDatetimeTzArray(elemRef.theDatetimeTzArray());
      } break;
      case ET::BDEM_DATETZ_ARRAY: {
        list->appendDateTzArray(elemRef.theDateTzArray());
      } break;
      case ET::BDEM_TIMETZ_ARRAY: {
        list->appendTimeTzArray(elemRef.theTimeTzArray());
      } break;
      default: {
        cout << "unidentified element type: " << type << endl;
        ASSERT(0);
      } break;
    }
}

void assignElement(bdem_ElemRef elemRef, ET::Type type)
    // Assign a random valid value to the specified 'elemRef' of the specified
    // 'type'.
{
    LOOP2_ASSERT(type, elemRef.type(), type == elemRef.type());

    const int MIN_ENTRIES = 3;
    int randNum = bsl::rand() % MIN_ENTRIES;
    CERef cer = getCERef(type, randNum);

    elemRef.replaceValue(cer);
    if (0 == randNum) {
        elemRef.makeNull();
    }
}

void ggTable(bdem_Table           *table,
             const bdem_RecordDef *record,
             bool                  replaceElement = false);
void ggChoice(bdem_Choice          *choice,
              const bdem_RecordDef *constraint,
              bool                  replaceElement = false);
void ggChoiceArray(bdem_ChoiceArray     *choiceArray,
                   const bdem_RecordDef *constraint,
                   bool                  replaceElement = false);

void ggList(bdem_List            *list,
            const bdem_RecordDef *record,
            bool                  replaceElement = false)
    // Populate the specified 'list' with data that satisfies the specified
    // 'record'.  Recursively walks the record creating sub containers &
    // values.  works with ggTable, ggChoice, ggChoiceArray
{
    if (!record) {  // 'record' will be 0 in case of UNCONSTRAINED LIST
        return;
    }

    for (int i = 0; i < record->numFields(); ++i) {
        ET::Type type = record->field(i).elemType();

        if (ET::BDEM_LIST == type) {
            List listElement;
            ggList(&listElement,
                   record->field(i).recordConstraint(),
                   replaceElement);
            list->appendList(listElement);
        }
        else if (ET::BDEM_TABLE == type) {
            Table table;
            ggTable(&table,
                    record->field(i).recordConstraint(),
                    replaceElement);
            list->appendTable(table);
        }
        else if (ET::BDEM_CHOICE== type) {
            Choice choice;
            ggChoice(&choice,
                     record->field(i).recordConstraint(),
                     replaceElement);
            list->appendChoice(choice);
        }
        else if (ET::BDEM_CHOICE_ARRAY==type) {
            ChoiceArray choiceArray;
            ggChoiceArray(&choiceArray,
                          record->field(i).recordConstraint(),
                          replaceElement);
            list->appendChoiceArray(choiceArray);
        }
        else {
            if (replaceElement) {
                assignElement((*list)[i], type);
            }
            else {
                addElementToList(list, type);
            }
        }
    }
}

void ggTable(bdem_Table           *table,
             const bdem_RecordDef *constraint,
             bool                  replaceElement)
    // Populate the specified 'table' with rows that satisfy the specified
    // 'record'.Recursively walks the record creating values and sub-containers
    // works with ggList, ggCHoice,ggChoiceArray
{
    if (!constraint) {  // UNCONSTRAINED TABLE
        return;
    }

    bsl::vector<ET::Type> existingTypes;
    table->columnTypes(&existingTypes);
    // create the catalog of possible choice types
    bsl::vector<ET::Type> typesCatalog;
    for (int i=0;i<constraint->numFields();++i)
    {
        typesCatalog.push_back(constraint->field(i).elemType());
    }
    if (existingTypes != typesCatalog)
    {
        // the new types catalog is not the same
        // as the old one, we need to reset the table to the new catalog.
        // Otherwise do nothing and just append rows to the existing table
        table->reset(typesCatalog);
    }

    int numRows = 2;  // TBD perhaps make this a default (third) argument

    for (int i = 0; i < numRows; ++i) {
        bdem_List list;
        ggList(&list, constraint);
        table->appendRow(list);
    }

}

void ggSimpleTypeList(bdem_List            *list,
                      ET::Type              type,
                      const bdem_RecordDef *constraint)
// creates a value of the type and adds it to the list.  For aggregate types
// it creates an empty aggregate
{

   // re-use code to create an object of the specified type
    switch (type)   {
        case ET::BDEM_LIST: {
          bdem_List nlist;
          ggList(&nlist, constraint);
          list->appendList(nlist);
        } break;
        case ET::BDEM_TABLE: {
          Table table;
          ggTable(&table, constraint);
          list->appendTable(table);
        } break;
        case ET::BDEM_CHOICE: {
          bdem_Choice choice;
          ggChoice(&choice,constraint);
          list->appendChoice(choice);
        } break;
        case ET::BDEM_CHOICE_ARRAY: {
          bdem_ChoiceArray choiceArray;
          ggChoiceArray(&choiceArray,constraint);
          list->appendChoiceArray(choiceArray);
        } break;
        default: {
          addElementToList(list,type);
        } break;
    };
}

void ggChoice(bdem_Choice          *choice,
              const bdem_RecordDef *constraint,
              bool                  replaceElement)
    // Populates the specified 'choice' with a value that satisfies the
    // specified record.  Recursively walks the record creating values &
    // subcontainers
    // works with ggList, ggChoiceArray, ggTable
{
    if (!constraint) {
        return;

    }

    bsl::vector<ET::Type> existingTypes;
    choice->selectionTypes(&existingTypes);
    // create the catalog of possible choice types
    bsl::vector<ET::Type> typesCatalog;
    for (int i=0;i<constraint->numFields();++i)
    {
        typesCatalog.push_back(constraint->field(i).elemType());
    }
    if (existingTypes != typesCatalog)
    {
      // the new types catalog is not the same
      // as the old one, we need to reset the choice to the new catalog.
      choice->reset(typesCatalog);
    }

    // add a value to the selection
    if (typesCatalog.size() > 0)
    {
        int randNum = bsl::rand() % typesCatalog.size();

        ET::Type type = constraint->field(randNum).elemType();

        // use a utility method to add an element of the type to a list
        bdem_List list;

        ggSimpleTypeList(&list,
                         type,
                         constraint->field(randNum).recordConstraint());
        // make the selection and add the value to the choice
        choice->makeSelection(randNum).replaceValue(list[0]);
    }
}

void ggChoiceArray(bdem_ChoiceArray     *choiceArray,
                   const bdem_RecordDef *constraint,
                   bool                  replaceElement)
    // Populates the specified 'choice array' with values that satisfies the
    // specified record.  Recursively walks the record creating values &
    // subcontainers works with ggList, ggChoice, ggTable
{
    if (!constraint) {

        return;
    }

    bsl::vector<ET::Type> existingTypes;
    choiceArray->selectionTypes(&existingTypes);

    // create the catalog of possible choice types
    bsl::vector<ET::Type> typesCatalog;
    for (int i=0;i<constraint->numFields();++i)
    {
        typesCatalog.push_back(constraint->field(i).elemType());
    }
    if (existingTypes != typesCatalog)
    {
      // the new types catalog is not the same
      // as the old one, we need to reset the choice array to the new catalog.
      // Otherwise do nothing and just append choices to the existing array
      choiceArray->reset(typesCatalog);
    }

    // select only 2 of the choice types to keep the recursive definition
    // manageable
    for (int i=0; (i<4) && (i<(int) typesCatalog.size()); ++i) {
        choiceArray->appendNullItems(1);
        int randNum = bsl::rand() % typesCatalog.size();
        ET::Type type = constraint->field(randNum).elemType();

        // use the utility method to add an element of the type to a list
        bdem_List list;
        ggSimpleTypeList(&list,
                         type,
                         constraint->field(randNum).recordConstraint());

        // make the selection and add the value to the choice
        choiceArray->theModifiableItem(i).makeSelection(randNum).replaceValue(
                                                                      list[0]);
    }
}

void initAggregate(const Obj *obj, const RecDef& record)
{
    if (obj->isNul2()) {
        return;
    }

    if (EType::BDEM_TABLE == obj->dataType()
     || EType::BDEM_CHOICE_ARRAY == obj->dataType()) {
        const int arrayLen = obj->length();
        for (int i = 0; i < arrayLen; ++i) {
            const Obj item = obj->field(i);
            initAggregate(&item, record);
        }
    }

    if (bdem_RecordDef::BDEM_CHOICE_RECORD == record.recordType()) {
        const int recLen = record.numFields();
        if (recLen > 0) {
            ASSERT(EType::BDEM_CHOICE            == obj->dataType()
                || EType::BDEM_CHOICE_ARRAY_ITEM == obj->dataType());
            if (BDEM_NULL_FIELD_ID == obj->selectorId()) {
                obj->makeSelectionByIndex(0);
            }
        }
    }
    else {
        ASSERT(bdem_RecordDef::BDEM_SEQUENCE_RECORD == record.recordType());
        ASSERT(EType::BDEM_LIST == obj->dataType()
            || EType::BDEM_ROW == obj->dataType());

        const int numFields = record.numFields();
        for (int i = 0; i < numFields; ++i) {
            const FldDef   *fieldDef = record.lookupField(i);
            EType::Type     type     = fieldDef->elemType();
            ERef            elemRef  = obj->fieldRefByIndex(i);

//             if (elemRef.isNull()) {
//                 continue;
//             }

            switch (type) {
              case ET::BDEM_LIST: {
                ggList(&elemRef.theModifiableList(),
                       fieldDef->recordConstraint(),
                       true);
              } break;
              case ET::BDEM_TABLE: {
                ggTable(&elemRef.theModifiableTable(),
                        fieldDef->recordConstraint(),
                        true);
              } break;
              case ET::BDEM_CHOICE: {
                ggChoice(&elemRef.theModifiableChoice(),
                         fieldDef->recordConstraint());
              } break;
              case ET::BDEM_CHOICE_ARRAY: {
                ggChoiceArray(&elemRef.theModifiableChoiceArray(),
                              fieldDef->recordConstraint());
              } break;
              default: {
                assignElement(elemRef, type);
              } break;
            }
        }
    }
}

template <typename VISITOR>
void funcVisitor(VISITOR *vPtr, const CERef& VALUE)
{
    VISITOR& visitor = *vPtr;

    switch (VALUE.type()) {
      case bdem_ElemType::BDEM_CHAR: {
        visitor(VALUE.theChar());
      } break;
      case bdem_ElemType::BDEM_SHORT: {
        visitor(VALUE.theShort());
      } break;
      case bdem_ElemType::BDEM_INT: {
        visitor(VALUE.theInt());
      } break;
      case bdem_ElemType::BDEM_INT64: {
        visitor(VALUE.theInt64());
      } break;
      case bdem_ElemType::BDEM_FLOAT: {
        visitor(VALUE.theFloat());
      } break;
      case bdem_ElemType::BDEM_DOUBLE: {
        visitor(VALUE.theDouble());
      } break;
      case bdem_ElemType::BDEM_STRING: {
        visitor(VALUE.theString());
      } break;
      case bdem_ElemType::BDEM_DATETIME: {
        visitor(VALUE.theDatetime());
      } break;
      case bdem_ElemType::BDEM_DATE: {
        visitor(VALUE.theDate());
      } break;
      case bdem_ElemType::BDEM_TIME: {
        visitor(VALUE.theTime());
      } break;
      case bdem_ElemType::BDEM_BOOL: {
        visitor(VALUE.theBool());
      } break;
      case bdem_ElemType::BDEM_DATETIMETZ: {
        visitor(VALUE.theDatetimeTz());
      } break;
      case bdem_ElemType::BDEM_DATETZ: {
        visitor(VALUE.theDateTz());
      } break;
      case bdem_ElemType::BDEM_TIMETZ: {
        visitor(VALUE.theTimeTz());
      } break;

      case bdem_ElemType::BDEM_CHAR_ARRAY: {
        visitor(VALUE.theCharArray());
      } break;
      case bdem_ElemType::BDEM_SHORT_ARRAY: {
        visitor(VALUE.theShortArray());
      } break;
      case bdem_ElemType::BDEM_INT_ARRAY: {
        visitor(VALUE.theIntArray());
      } break;
      case bdem_ElemType::BDEM_INT64_ARRAY: {
        visitor(VALUE.theInt64Array());
      } break;
      case bdem_ElemType::BDEM_FLOAT_ARRAY: {
        visitor(VALUE.theFloatArray());
      } break;
      case bdem_ElemType::BDEM_DOUBLE_ARRAY: {
        visitor(VALUE.theDoubleArray());
      } break;
      case bdem_ElemType::BDEM_STRING_ARRAY: {
        visitor(VALUE.theStringArray());
      } break;
      case bdem_ElemType::BDEM_DATETIME_ARRAY: {
        visitor(VALUE.theDatetimeArray());
      } break;
      case bdem_ElemType::BDEM_DATE_ARRAY: {
        visitor(VALUE.theDateArray());
      } break;
      case bdem_ElemType::BDEM_TIME_ARRAY: {
        visitor(VALUE.theTimeArray());
      } break;
      case bdem_ElemType::BDEM_BOOL_ARRAY: {
        visitor(VALUE.theBoolArray());
      } break;
      case bdem_ElemType::BDEM_DATETIMETZ_ARRAY: {
        visitor(VALUE.theDatetimeTzArray());
      } break;
      case bdem_ElemType::BDEM_DATETZ_ARRAY: {
        visitor(VALUE.theDateTzArray());
      } break;
      case bdem_ElemType::BDEM_TIMETZ_ARRAY: {
        visitor(VALUE.theTimeTzArray());
      } break;

      case bdem_ElemType::BDEM_LIST: {
        visitor(VALUE.theList());
      } break;
      case bdem_ElemType::BDEM_TABLE: {
        visitor(VALUE.theTable());
      } break;
      case bdem_ElemType::BDEM_CHOICE: {
        visitor(VALUE.theChoice());
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY: {
        visitor(VALUE.theChoiceArray());
      } break;

      default: {
        ASSERT("Shouldn't get here" && 0);
      } break;
    }
}

static int getLength(const CERef& value)
    // Return the length of the specified 'value'
{
    switch (value.type()) {
      case bdem_ElemType::BDEM_CHAR_ARRAY:
        return value.theCharArray().size();
      case bdem_ElemType::BDEM_SHORT_ARRAY:
        return value.theShortArray().size();
      case bdem_ElemType::BDEM_INT_ARRAY:
        return value.theIntArray().size();
      case bdem_ElemType::BDEM_INT64_ARRAY:
        return value.theInt64Array().size();
      case bdem_ElemType::BDEM_FLOAT_ARRAY:
        return value.theFloatArray().size();
      case bdem_ElemType::BDEM_DOUBLE_ARRAY:
        return value.theDoubleArray().size();
      case bdem_ElemType::BDEM_STRING_ARRAY:
        return value.theStringArray().size();
      case bdem_ElemType::BDEM_DATETIME_ARRAY:
        return value.theDatetimeArray().size();
      case bdem_ElemType::BDEM_DATE_ARRAY:
        return value.theDateArray().size();
      case bdem_ElemType::BDEM_TIME_ARRAY:
        return value.theTimeArray().size();
      case bdem_ElemType::BDEM_BOOL_ARRAY:
        return value.theBoolArray().size();
      case bdem_ElemType::BDEM_DATETIMETZ_ARRAY:
        return value.theDatetimeTzArray().size();
      case bdem_ElemType::BDEM_DATETZ_ARRAY:
        return value.theDateTzArray().size();
      case bdem_ElemType::BDEM_TIMETZ_ARRAY:
        return value.theTimeTzArray().size();

      case bdem_ElemType::BDEM_LIST:
        return value.theList().length();
      case bdem_ElemType::BDEM_TABLE:
        return value.theTable().numRows();
      case bdem_ElemType::BDEM_CHOICE:
        return value.theChoice().numSelections();
      case bdem_ElemType::BDEM_CHOICE_ARRAY:
        return value.theChoiceArray().length();

      default:
        ASSERT(0);
    }
    return -1;
}

static bool compareNillableElement(bcem_Aggregate agg, const CERef& elemRef)
{
    bdem_ElemType::Type type = elemRef.type();
    switch (type) {
      case bdem_ElemType::BDEM_CHAR:
        return agg.asChar() == elemRef.theChar();
      case bdem_ElemType::BDEM_SHORT:
        return agg.asShort() == elemRef.theShort();
      case bdem_ElemType::BDEM_INT:
        return agg.asInt() == elemRef.theInt();
      case bdem_ElemType::BDEM_INT64:
        return agg.asInt64() == elemRef.theInt64();
      case bdem_ElemType::BDEM_FLOAT:
        return agg.asFloat() == elemRef.theFloat();
      case bdem_ElemType::BDEM_DOUBLE:
        return agg.asDouble() == elemRef.theDouble();
      case bdem_ElemType::BDEM_STRING:
        return agg.asString() == elemRef.theString();
      case bdem_ElemType::BDEM_DATETIME:
        return agg.asDatetime() == elemRef.theDatetime();
      case bdem_ElemType::BDEM_DATE:
        return agg.asDate() == elemRef.theDate();
      case bdem_ElemType::BDEM_TIME:
        return agg.asTime() == elemRef.theTime();
      case bdem_ElemType::BDEM_BOOL:
        return agg.asBool() == elemRef.theBool();
      case bdem_ElemType::BDEM_DATETIMETZ:
        return agg.asDatetimeTz() == elemRef.theDatetimeTz();
      case bdem_ElemType::BDEM_DATETZ:
        return agg.asDateTz() == elemRef.theDateTz();
      case bdem_ElemType::BDEM_TIMETZ:
        return agg.asTimeTz() == elemRef.theTimeTz();

      default:
        ASSERT(0);
    }
    return -1;
}

static void assignToNillableTable(bcem_Aggregate *agg, const CERef& arrayRef)
{
    bdem_ElemType::Type arrayType = arrayRef.type();
    switch (arrayType) {
      case bdem_ElemType::BDEM_CHAR_ARRAY:
        agg->setValue(arrayRef.theCharArray());
        return;
      case bdem_ElemType::BDEM_SHORT_ARRAY:
        agg->setValue(arrayRef.theShortArray());
        return;
      case bdem_ElemType::BDEM_INT_ARRAY:
        agg->setValue(arrayRef.theIntArray());
        return;
      case bdem_ElemType::BDEM_INT64_ARRAY:
        agg->setValue(arrayRef.theInt64Array());
        return;
      case bdem_ElemType::BDEM_FLOAT_ARRAY:
        agg->setValue(arrayRef.theFloatArray());
        return;
      case bdem_ElemType::BDEM_DOUBLE_ARRAY:
        agg->setValue(arrayRef.theDoubleArray());
        return;
      case bdem_ElemType::BDEM_STRING_ARRAY:
        agg->setValue(arrayRef.theStringArray());
        return;
      case bdem_ElemType::BDEM_DATETIME_ARRAY:
        agg->setValue(arrayRef.theDatetimeArray());
        return;
      case bdem_ElemType::BDEM_DATE_ARRAY:
        agg->setValue(arrayRef.theDateArray());
        return;
      case bdem_ElemType::BDEM_TIME_ARRAY:
        agg->setValue(arrayRef.theTimeArray());
        return;
      case bdem_ElemType::BDEM_BOOL_ARRAY:
        agg->setValue(arrayRef.theBoolArray());
        return;
      case bdem_ElemType::BDEM_DATETIMETZ_ARRAY:
        agg->setValue(arrayRef.theDatetimeTzArray());
        return;
      case bdem_ElemType::BDEM_DATETZ_ARRAY:
        agg->setValue(arrayRef.theDateTzArray());
        return;
      case bdem_ElemType::BDEM_TIMETZ_ARRAY:
        agg->setValue(arrayRef.theTimeTzArray());
        return;
      default:
        ASSERT(0);
    }
}

static bool compareNillableElement(bcem_Aggregate agg,
                                   const CERef&   elemRef,
                                   int            index)
{
    bdem_ElemType::Type arrayType = elemRef.type();
    bdem_ElemType::Type subType   = bdem_ElemType::fromArrayType(arrayType);
    switch (arrayType) {
      case bdem_ElemType::BDEM_CHAR_ARRAY:
        return agg.asChar() == elemRef.theCharArray()[index];
      case bdem_ElemType::BDEM_SHORT_ARRAY:
        return agg.asShort() == elemRef.theShortArray()[index];
      case bdem_ElemType::BDEM_INT_ARRAY:
        return agg.asInt() == elemRef.theIntArray()[index];
      case bdem_ElemType::BDEM_INT64_ARRAY:
        return agg.asInt64() == elemRef.theInt64Array()[index];
      case bdem_ElemType::BDEM_FLOAT_ARRAY:
        return agg.asFloat() == elemRef.theFloatArray()[index];
      case bdem_ElemType::BDEM_DOUBLE_ARRAY:
        return agg.asDouble() == elemRef.theDoubleArray()[index];
      case bdem_ElemType::BDEM_STRING_ARRAY:
        return agg.asString() == elemRef.theStringArray()[index];
      case bdem_ElemType::BDEM_DATETIME_ARRAY:
        return agg.asDatetime() == elemRef.theDatetimeArray()[index];
      case bdem_ElemType::BDEM_DATE_ARRAY:
        return agg.asDate() == elemRef.theDateArray()[index];
      case bdem_ElemType::BDEM_TIME_ARRAY:
        return agg.asTime() == elemRef.theTimeArray()[index];
      case bdem_ElemType::BDEM_BOOL_ARRAY:
        return agg.asBool() == elemRef.theBoolArray()[index];
      case bdem_ElemType::BDEM_DATETIMETZ_ARRAY:
        return agg.asDatetimeTz() == elemRef.theDatetimeTzArray()[index];
      case bdem_ElemType::BDEM_DATETZ_ARRAY:
        return agg.asDateTz() == elemRef.theDateTzArray()[index];
      case bdem_ElemType::BDEM_TIMETZ_ARRAY:
        return agg.asTimeTz() == elemRef.theTimeTzArray()[index];

      default:
        ASSERT(0);
    }
    return -1;
}

static bool compareNillableTable(bcem_Aggregate agg, const CERef& elemRef)
{
    ASSERT(bdem_ElemType::BDEM_TABLE == agg.dataType());
    const int LEN = getLength(elemRef);
    if (LEN != agg.length()) {
        return false;
    }
    for (int i = 0; i < LEN; ++i) {
        if (!compareNillableElement(agg[i], elemRef, i)) {
            return false;
        }
    }
    return true;
}

static bool compareCERefs(const CERef& lhs, const CERef& rhs)
{
    // Special Handling of list/row and choice/choice-array-item combinations:
    if (ET::BDEM_ROW == lhs.type() && ET::BDEM_LIST == rhs.type()) {
        return *(Row *) lhs.data() == rhs.theList().row();
    }
    else if (ET::BDEM_LIST == lhs.type() && ET::BDEM_ROW == rhs.type()) {
        return lhs.theList().row() == *(Row *) rhs.data();
    }
    else if (ET::BDEM_CHOICE_ARRAY_ITEM == lhs.type()
          && ET::BDEM_CHOICE            == rhs.type()) {
        return *(ChoiceItem *) lhs.data() == rhs.theChoice().item();
    }
    else if (ET::BDEM_CHOICE            == lhs.type()
          && ET::BDEM_CHOICE_ARRAY_ITEM == rhs.type()) {
        return lhs.theChoice().item() == *(ChoiceItem *) rhs.data();
    }
    else {
        return lhs == rhs;
    }
}

// Simple function objects for testing templated functions
struct SetFieldFunctor
{
    // Function object to call setField function for a templated type on a
    // specified name or index
    Obj        *d_obj;      // Object to call setField function on
    const char *d_fldName;  // FieldName of the field to set or 0
    int         d_index;    // Index of field to set

  public:
    SetFieldFunctor(Obj *obj, const char *fldName)
    : d_obj(obj)
    , d_fldName(fldName)
    , d_index(-1)
    { }

    SetFieldFunctor(Obj *obj, int index)
    : d_obj(obj)
    , d_fldName(0)
    , d_index(index)
    { }

    template <typename VALTYPE>
    void operator()(const VALTYPE& value)
    {
        if (d_fldName) {
            d_obj->setField(d_fldName, value);
        }
        else {
            d_obj->setField(d_index, value);
        }
    }
};

struct SetValueFunctor
{
    // Function object to call setValue function for a templated type.
    Obj        *d_obj;      // Object to call setField function on

  public:
    SetValueFunctor(Obj *obj)
        : d_obj(obj)
        { }

    template <typename VALTYPE>
    void operator()(const VALTYPE& value) {
        d_obj->setValue(value);
    }
};

//=============================================================================
//                  HELPER FUNCTION FOR TESTING ENUMERATIONS
//-----------------------------------------------------------------------------

template <typename VALTYPE>
void enumTest(const bcem_Aggregate&  AGG,
              const char            *fldcodes,
              const VALTYPE&         value,
              int                    expInt,
              const char            *expString,
              int                    expErrCode,
              const int              LINE = -1)
    // Clone AGG.  Then, for each field code in 'fldcodes', set the
    // enumeration field specified by the field code to the specified
    // 'value'.  Compare the resulting value against the expected values
    // specified in 'expInt', 'expString', and 'expErrcode'.  Perform this
    // test using both 'setValue' and 'setField' to set the field.  If a
    // specific field code refers to an array, then perform these tests on
    // element 0 of the array.
{
    for (const char *fldcode = fldcodes; *fldcode; ++fldcode) {
        const char *const fldname = getName(*fldcode);

        bool isNSA = 'b' == fldcode[0] || 'c' == fldcode[0] ? true : false;

        bool isArray = ET::isArrayType(AGG.fieldType(fldname));

        Obj mAgg1 = AGG.cloneData();
        Obj fld;
        if (isArray)
            fld = mAgg1.field(fldname, 0);
        else
            fld = mAgg1.field(fldname);
        Obj ret = fld.setValue(value);
        LOOP2_ASSERT(expErrCode, ret, expErrCode == ret.errorCode());
        if (veryVerbose) { P_(value) P_(fld) P(ret) }
        if (ret.isError()) {
            // Assert that field is unchanged.
            if (!isNSA) {
                LOOP5_ASSERT(fld,
                             AGG.field(fldname),
                             mAgg1.field(fldname),
                             fldname, value,
                             Obj::areEquivalent(AGG.field(fldname),
                                                mAgg1.field(fldname)));
            }
        }
        else {
            LOOP_ASSERT(fld, fld.asInt()    == expInt);
            LOOP_ASSERT(fld, fld.asString() == expString);
        }

        mAgg1 = AGG.cloneData();
        if (isArray)
            ret = mAgg1.setField(fldname, 0, value);
        else
            ret = mAgg1.setField(fldname, value);
        LOOP2_ASSERT(expErrCode, ret, expErrCode == ret.errorCode());
        if (isArray)
            fld = mAgg1.field(fldname, 0);
        else
            fld = mAgg1.field(fldname);
        if (veryVerbose) { P_(value) P_(fld) P(ret) }
        if (ret.isError()) {
            if (!isNSA) {
                // Assert that field is unchanged.
                LOOP5_ASSERT(fld, AGG.field(fldname), mAgg1.field(fldname),
                             fldname, value,
                             Obj::areEquivalent(AGG.field(fldname),
                                                mAgg1.field(fldname)));
            }
        }
        else {
            LOOP_ASSERT(fld, fld.asInt()    == expInt);
            LOOP3_ASSERT(fld, expString, fldname, fld.asString() == expString);
        }
    } // End for each field code
}

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

class AggAccessor {
    // Aggregate accessor class.  Keeps track of the values of every aggregate
    // that it visits.  The list of values is reset automatically using the
    // 'matchValue' or 'matchValues' functions.

    bsl::vector<int> d_values;
    bool             d_isNullable; // true if current node is nullable

  public:
    AggAccessor() : d_isNullable(false) { }
        // Construct an accessor object

    // MANIPULATORS
    template <typename T>
    int operator()(const T& value);
    template <typename T, typename INFO>
    int operator()(const T& value, const INFO& info);
        // Convert 'value' to a series of integers and push onto value stack.

    template <typename T>
    int operator()(const T& value, const TC::Array&);
    template <typename T>
    int operator()(const T& value, const TC::Choice&);
    template <typename T>
    int operator()(const T& value, const TC::CustomizedType&);
    template <typename T>
    int operator()(const T& value, const TC::Enumeration&);
    template <typename T>
    int operator()(const T& value, const TC::NullableValue&);
    template <typename T>
    int operator()(const T& value, const TC::Sequence&);
    template <typename T>
    int operator()(const T& value, const TC::Simple&);

    template <typename T>
    int operator()(const T& value, const bslmf_Nil&);
        // Illegal call: prevent recursive calls to the 'INFO' version of
        // 'operator()' in case of incorrect overload selection by the
        // compiler.

    bool matchValue(int v);
        // Return true if vector of 'numValues()' is 1 and 'v == value(0)'.
        // Call 'reset()' before returning.

    bool matchValues(int v0, int v1);
        // Return true if vector of 'numValues()' is 2 and 'v0 == value(0)'
        // and 'v1 == value(1)'.  Call 'reset()' before returning.

    bool matchValues(int v0, int v1, int v2);
        // Return true if vector of 'numValues()' is 2 and 'v0 == value(0)'
        // and 'v1 == value(1)' and 'v2 == value(2)'.  Call 'reset()' before
        // returning.

    void reset() { d_values.clear(); }
        // Clear accumulated values.

    // ACCESSORS
    int numValues() const { return d_values.size(); }
        // Return the number of values since last call to 'reset'.

    int value(int n) const;
        // Return the 'n'th value received since the last call to 'reset' or
        // the string -1 if 'n < 0 || numValues() < n'.
};

template <typename T>
inline
int AggAccessor::operator()(const T& value, const TC::Array&)
{
    for (int i = 0; i < (int) AF::size(value); ++i) {
        AF::accessElement(value, *this, i);
    }
    return 0;
}

template <typename T>
inline
int AggAccessor::operator()(const T& value, const TC::Choice&)
{
    d_isNullable = false;
    CF::accessSelection(value, *this);
    return 0;
}

template <typename T>
inline
int AggAccessor::operator()(const T& value, const TC::CustomizedType&)
{
    ASSERT(! "Custom types are NOT HANDLED");
    return -1;
}

template <typename T>
inline
int AggAccessor::operator()(const T& value, const TC::Enumeration&)
{
    int intValue;
    bdeat_EnumFunctions::toInt(&intValue, value);
    if (d_isNullable) {
        intValue = -intValue;
    }
    d_values.push_back(intValue);
    return 0;
}

template <typename T>
inline
int AggAccessor::operator()(const T& value, const TC::NullableValue&)
{
    d_isNullable = true;

    if (NVF::isNull(value)) {
        d_values.push_back(-5000);
    }
    else {
        NVF::accessValue(value, *this);
    }
    return 0;
}

template <typename T>
inline
int AggAccessor::operator()(const T& value, const TC::Sequence&)
{
    d_isNullable = false;
    SF::accessAttributes(value, *this);
    return 0;
}

template <typename T>
inline
int AggAccessor::operator()(const T& value, const TC::Simple&)
{
    int intValue = -2;
    bdem_Convert::convert(&intValue, value);
    if (d_isNullable) {
        intValue = -intValue;
    }
    d_values.push_back(intValue);
    return 0;
}

template <typename T>
inline
int AggAccessor::operator()(const T& value, const bslmf_Nil&)
{
    // This method should never be instantiated
    ASSERT("Shouldn't call this function" && 0);
    return 0;
}

template <typename T>
inline
int AggAccessor::operator()(const T& value) {
    bdeat_TypeCategoryUtil::accessByCategory(value, *this);
    d_isNullable = false;
    return 0;
}

template <typename T, typename INFO>
int AggAccessor::operator()(const T& value, const INFO& info) {
    return operator()(value);
}

bool AggAccessor::matchValue(int v)
{
    bool ret = (1 == d_values.size() && v == d_values[0]);
//     P(d_values[0]);
    reset();
    return ret;
}

bool AggAccessor::matchValues(int v0, int v1)
{
    bool ret = (2 == d_values.size() &&
                v0 == d_values[0] && v1 == d_values[1]);
    reset();
    return ret;
}

bool AggAccessor::matchValues(int v0, int v1, int v2)
{
    bool ret = (3 == d_values.size() &&
                v0 == d_values[0] && v1 == d_values[1] && v2 == d_values[2]);
    reset();
    return ret;
}

int AggAccessor::value(int n) const
{
    if (0 <= n && (unsigned) n < d_values.size()) {
        return d_values[n];
    }
    else {
        return -1;
    }
}

class NewAggAccessor {
    // Aggregate accessor class.  Keeps track of the values of every aggregate
    // that it visits.  The list of values is reset automatically using the
    // 'matchValue' or 'matchValues' functions.

    bsl::ostringstream      d_output;
    bsl::stack<bsl::string> d_nodes;
    bool                    d_isNillable; // true if current node is nillable

  public:
    NewAggAccessor() : d_isNillable(false) { }
        // Construct an accessor object

    // MANIPULATORS
    template <typename T>
    int operator()(const T& value);
    template <typename T, typename INFO>
    int operator()(const T& value, const INFO& info);
        // Convert 'value' to a series of integers and push onto value stack.

    template <typename T>
    int operator()(const T& value, const TC::Array&);
    template <typename T>
    int operator()(const T& value, const TC::Choice&);
    template <typename T>
    int operator()(const T& value, const TC::CustomizedType&);
    template <typename T>
    int operator()(const T& value, const TC::Enumeration&);
    template <typename T>
    int operator()(const T& value, const TC::NullableValue&);
    template <typename T>
    int operator()(const T& value, const TC::Sequence&);
    template <typename T>
    int operator()(const T& value, const TC::Simple&);

    template <typename T>
    int operator()(const T& value, const bslmf_Nil&);
        // Illegal call: prevent recursive calls to the 'INFO' version of
        // 'operator()' in case of incorrect overload selection by the
        // compiler.

    void reset() { d_output.str(""); }
        // Clear accumulated values.

    // ACCESSORS

    bsl::string value() const { return d_output.str(); }
};

template <typename T>
inline
int NewAggAccessor::operator()(const T& value, const TC::Array&)
{
    for (int i = 0; i < (int) AF::size(value); ++i) {
        AF::accessElement(value, *this, i);
    }
    return 0;
}

template <typename T>
inline
int NewAggAccessor::operator()(const T& value, const TC::Choice&)
{
    const bsl::string& name = d_nodes.top();
    d_output << '<' << name << '>';
    CF::accessSelection(value, *this);
    d_output << "</" << name << ">";
    return 0;
}

template <typename T>
inline
int NewAggAccessor::operator()(const T& value, const TC::CustomizedType&)
{
    ASSERT(! "Custom types are NOT HANDLED");
    return -1;
}

template <typename T>
inline
int NewAggAccessor::operator()(const T& value, const TC::Enumeration&)
{
    int intValue;
    bdeat_EnumFunctions::toInt(&intValue, value);
    const bsl::string& name = d_nodes.top();
    d_output << '<' << name << '>'
             << intValue
             << "</" << name << ">";
    return 0;
}

template <typename T>
inline
int NewAggAccessor::operator()(const T& value, const TC::NullableValue&)
{
    if (NVF::isNull(value)) {
        if (d_isNillable) {
            d_output << '<' << d_nodes.top() << " xsi:nil='true'/>";
        }
        else {
            d_output << '<' << d_nodes.top() << "/>";
        }
    }
    else {
        NVF::accessValue(value, *this);
    }
    return 0;
}

template <typename T>
inline
int NewAggAccessor::operator()(const T& value, const TC::Sequence&)
{
    const bsl::string& name = d_nodes.top();
    d_output << '<' << name << '>';
    SF::accessAttributes(value, *this);
    d_output << "</" << name << ">";
    return 0;
}

template <typename T>
inline
int NewAggAccessor::operator()(const T& value, const TC::Simple&)
{
    int intValue = -2;
    bdem_Convert::convert(&intValue, value);
    const bsl::string& name = d_nodes.top();
    d_output << '<' << name << '>'
             << intValue
             << "</" << name << ">";
    return 0;
}

template <typename T>
inline
int NewAggAccessor::operator()(const T& value, const bslmf_Nil&)
{
    // This method should never be instantiated
    ASSERT("Shouldn't call this function" && 0);
    return 0;
}

template <typename T>
inline
int NewAggAccessor::operator()(const T& value) {
    bdeat_TypeCategoryUtil::accessByCategory(value, *this);
    return 0;
}

template <typename T, typename INFO>
int NewAggAccessor::operator()(const T& value, const INFO& info) {
    if (Format::BDEAT_NILLABLE & info.formattingMode()) {
        d_isNillable = true;
    }
    d_nodes.push(info.name());
    int rc = operator()(value);
    d_nodes.pop();
    return rc;
}

class NewAggManipulator {
    // Manipulate an aggregate by setting it to the next counter value.

   bsl::list<CERef> d_elements;
   int              d_isNillable;

  public:
    NewAggManipulator() : d_isNillable(false) { }
        // Construct an manipulator object

    // MANIPULATORS
    template <typename T>
    int operator()(T* value);
    template <typename T, typename INFO>
    int operator()(T* value, const INFO& info);
        // Set the specified 'value' to the current 'counter()' and increment
        // the counter.

    template <typename T>
    int operator()(T* value, const TC::Array&);
    template <typename T>
    int operator()(T* value, const TC::Choice&);
    template <typename T>
    int operator()(T* value, const TC::CustomizedType&);
    template <typename T>
    int operator()(T* value, const TC::Enumeration&);
    template <typename T>
    int operator()(T* value, const TC::NullableValue&);
    template <typename T>
    int operator()(T* value, const TC::Sequence&);
    template <typename T>
    int operator()(T* value, const TC::Simple&);

    void reset() { d_elements.clear(); }

    bsl::list<CERef>& elements() { return d_elements; }
};

template <typename T>
inline
int NewAggManipulator::operator()(T* value, const TC::Array&)
{
    const int numItems = d_elements.front().theInt();
    d_elements.pop_front();
    AF::resize(value, numItems);
    for (int i = 0; i < (int) AF::size(*value); ++i) {
        AF::manipulateElement(value, *this, i);
    }
    return 0;
}

template <typename T>
inline
int NewAggManipulator::operator()(T* value, const TC::Choice&)
{
    // Calculate the number of selections supported by this choice
    int numSelections = 0;
    for ( ; CF::hasSelection(*value, numSelections); ++numSelections);

    if (0 == numSelections) {
        return 0;
    }

    const CERef& selectorRef = d_elements.front();
    int selector = selectorRef.theInt();
    CF::makeSelection(value, selector);
    d_elements.pop_front();
    CF::manipulateSelection(value, *this);
    return 0;
}

template <typename T>
inline
int NewAggManipulator::operator()(T* value, const TC::CustomizedType&)
{
    ASSERT(! "Custom types are NOT HANDLED");
    return -1;
}

template <typename T>
inline
int NewAggManipulator::operator()(T* value, const TC::Enumeration&)
{
    int intValue;
    const CERef& ref = d_elements.front();
    bdem_Convert::convert(&intValue, ref);
    bdeat_EnumFunctions::fromInt(value, intValue);
    d_elements.pop_front();
    return 0;
}

template <typename T>
inline
int NewAggManipulator::operator()(T* value, const TC::NullableValue&)
{
    const CERef& ref = d_elements.front();
    if (!ref.isBound()) {
        bdeat_ValueTypeFunctions::reset(value);
        d_elements.pop_front();
        return 0;
    }

    if (NVF::isNull(*value)) {
        NVF::makeValue(value);
    }

    NVF::manipulateValue(value, *this);
    return 0;
}

template <typename T>
inline
int NewAggManipulator::operator()(T* value, const TC::Sequence&)
{
    SF::manipulateAttributes(value, *this);
    return 0;
}

template <typename T>
inline
int NewAggManipulator::operator()(T* value, const TC::Simple&)
{
    const CERef& ref = d_elements.front();
    int rc = bdem_Convert::convert(value, ref);
    d_elements.pop_front();
    return rc;
}

template <typename T>
inline
int NewAggManipulator::operator()(T* value) {
    bdeat_TypeCategoryUtil::manipulateByCategory(value, *this);
    return 0;
}

template <typename T, typename INFO>
int NewAggManipulator::operator()(T* value, const INFO& info)
{
    return operator()(value);
}

class AggManipulator {
    // Manipulate an aggregate by setting it to the next counter value.

    int d_counter;
    int d_isNullable;

  public:
    AggManipulator() : d_counter(100), d_isNullable(false) { }
        // Construct an manipulator object

    // MANIPULATORS
    template <typename T>
    int operator()(T* value);
    template <typename T, typename INFO>
    int operator()(T* value, const INFO& info);
        // Set the specified 'value' to the current 'counter()' and increment
        // the counter.

    template <typename T>
    int operator()(T* value, const TC::Array&);
    template <typename T>
    int operator()(T* value, const TC::Choice&);
    template <typename T>
    int operator()(T* value, const TC::CustomizedType&);
    template <typename T>
    int operator()(T* value, const TC::Enumeration&);
    template <typename T>
    int operator()(T* value, const TC::NullableValue&);
    template <typename T>
    int operator()(T* value, const TC::Sequence&);
    template <typename T>
    int operator()(T* value, const TC::Simple&);

    void reset(int counter = 0) { d_counter = counter; }
        // Reset the counter to the specified 'counter' value.

    int counterNext() { return d_counter++; }
        // Return the counter and increment count to the next value.

    // ACCESSORS
    int counter() const { return d_counter; }
        // Return the current counter value.
};

template <typename T>
inline
int AggManipulator::operator()(T* value, const TC::Array&)
{
    for (int i = 0; i < (int) AF::size(*value); ++i) {
        AF::manipulateElement(value, *this, i);
    }
    return 0;
}

template <typename T>
inline
int AggManipulator::operator()(T* value, const TC::Choice&)
{
    d_isNullable = false;

    // Calculate the number of selections supported by this choice
    int numSelections = 0;
    for ( ; CF::hasSelection(*value, numSelections); ++numSelections)
        ;

    if (0 == numSelections)
        return 0;

    int selector = counter() % numSelections;
    CF::makeSelection(value, selector);
    CF::manipulateSelection(value, *this);
    return 0;
}

template <typename T>
inline
int AggManipulator::operator()(T* value, const TC::CustomizedType&)
{
    ASSERT(! "Custom types are NOT HANDLED");
    return -1;
}

template <typename T>
inline
int AggManipulator::operator()(T* value, const TC::Enumeration&)
{
    bdeat_EnumFunctions::fromInt(value, counterNext());
    return 0;
}

template <typename T>
inline
int AggManipulator::operator()(T* value, const TC::NullableValue&)
{
    d_isNullable = true;

    if (NVF::isNull(*value)) {
        NVF::makeValue(value);
    }

    NVF::manipulateValue(value, *this);
    return 0;
}

template <typename T>
inline
int AggManipulator::operator()(T* value, const TC::Sequence&)
{
    d_isNullable = false;
    SF::manipulateAttributes(value, *this);
    return 0;
}

template <typename T>
inline
int AggManipulator::operator()(T* value, const TC::Simple&)
{
    int intValue = counterNext();
    if (d_isNullable) {
        intValue = -intValue;
    }
    bdem_Convert::convert(value, intValue);
    return 0;
}

template <typename T>
inline
int AggManipulator::operator()(T* value) {
    bdeat_TypeCategoryUtil::manipulateByCategory(value, *this);
    d_isNullable = false;
    return 0;
}

template <typename T, typename INFO>
int AggManipulator::operator()(T* value, const INFO& info) {
    return operator()(value);
}

//=============================================================================
//                    SUPPORT TYPES USED IN TEST CASES
//-----------------------------------------------------------------------------

enum {
    NOT_CHOICE = ErrorCode::BCEM_NOT_A_CHOICE,
    BAD_FLDNM  = ErrorCode::BCEM_BAD_FIELDNAME,
    BAD_FLDIDX = ErrorCode::BCEM_BAD_FIELDINDEX,
    NOT_SELECT = ErrorCode::BCEM_NOT_SELECTED,
    NON_RECORD = ErrorCode::BCEM_NOT_A_RECORD,
    AMBIGUOUS  = ErrorCode::BCEM_AMBIGUOUS_ANON
};


struct Accumulator {
    int d_count;
    
    int operator()(const bcem_AggregateRaw& value) {
        d_count += value.asInt();
        return 0;
    }

    int operator()(const bcem_Aggregate& value) {
        d_count += value.asInt();
        return 0;
//         return (*this)(value.aggregateRaw());
    }
 
    template<typename TYPE>
    int operator()(const TYPE& value) {
        return -1;
    }
   
    Accumulator() : d_count(0) {}
};

//=============================================================================
//                              TEST CASES
//-----------------------------------------------------------------------------

static void runBerBenchmark(bool verbose, bool veryVerbose, 
                            bool veryVeryVerbose) {

    const char SPEC[] = 
        ":s Ca&NF Cb&NT Cf&NT Cg&NT Ch&NT Gc&NF Gd&NT"
        ":d? Ga&NF Bb&NF"
        ":r %fd Fa&NT Cb&NT Gc&NF #js "
        ":f Cx&NF #js"
        ":g Cx&NF #jr"
        ":c? +ff +gg";

    SchemaShdPtr schema;
    schema.createInplace(0);
    
    ggSchema(schema.ptr(), SPEC);
    if (veryVerbose) {
        bsl::cout << "Schema=";
        schema->print(bsl::cout) << bsl::endl;
    }

    // create a test Aggregate
    enum {
        NUM_REQUEST_ENTRIES = 20,
        NUM_ENTRY_FIELDS = 80,
        STR_LENGTH = 400
    };

    bcem_Aggregate testMessage(schema, "c");
    bcem_Aggregate gRecord = testMessage.makeSelection("g");
    gRecord.setField("ex", 4);

    bsl::string theString(STR_LENGTH, 'X');

    bcem_Aggregate requestEntries = gRecord.field("j");
    requestEntries.appendItems(NUM_REQUEST_ENTRIES);
    for (int i = 0; i < NUM_REQUEST_ENTRIES; ++i) {
        requestEntries[i].field("f").makeSelection("a").setValue(theString);
        requestEntries[i].setField("a", 1.5);
        requestEntries[i].setField("b", 5);
        requestEntries[i].setField("c", theString);
        bcem_Aggregate entryFields = requestEntries[i].field("j");
        entryFields.appendItems(NUM_ENTRY_FIELDS);
        for (int j = 0; j < NUM_ENTRY_FIELDS; ++j) {
            entryFields[j].setField("a", 6);
            entryFields[j].setField("c", theString);
            entryFields[j].setField("d", theString);
        }
    }
    if (veryVerbose) {
        bsl::cout << "Message=" << testMessage
                  << bsl::endl;
    }

    enum {
        NUM_ITER = 500,
        ENCODE_BUFFER_SIZE=512*1024*1024
    };
    
    char* ENCODE_BUFFER = new char[ENCODE_BUFFER_SIZE];

    bsls_Stopwatch timer;
    bdem_BerEncoderOptions options;
    bdem_BerEncoder encoder(&options);
    
    timer.start(true);
    int length;
    for (int i = 0; i < NUM_ITER; ++i) {
        bdesb_FixedMemOutStreamBuf osb(ENCODE_BUFFER, ENCODE_BUFFER_SIZE);
        ASSERT(0 == encoder.encode(&osb, testMessage));
        length = osb.length();
    }
    timer.stop();

    bsl::cout << "Encode time: " << timer.accumulatedWallTime()
              << " user: " << timer.accumulatedUserTime() << bsl::endl;

    timer.reset();
    bdem_BerDecoderOptions berDecoderOptions;
    bdem_BerDecoder berDecoder(&berDecoderOptions);
    
    timer.start(true);
    bcem_Aggregate testResponse(schema, "c");

    for (int i = 0; i < NUM_ITER; ++i) {
        bdesb_FixedMemInStreamBuf isb(ENCODE_BUFFER, length);
        ASSERT(0 == berDecoder.decode(&isb, &testResponse));
    }

    timer.stop();

    bsl::cout << "Decode time: " << timer.accumulatedWallTime()
              << " user: " << timer.accumulatedUserTime() << bsl::endl;

    delete[] ENCODE_BUFFER;
}
    

static void testCase35(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // TESTING 'bdeat' FUNCTIONS
        //
        // Concerns:
        //   - 'bdeat_TypeName::className(bcem_Aggregate)' returns the
        //     record name for aggregates that are constrained by a record
        //     definition, and a zero pointer for aggregates that are not
        //     constrained by a record definition.
        //   - 'bdeat_TypeCategory::Select<bcem_Aggregate>::
        //      BDEAT_SELECTION' is '0'
        //   - 'bdeat_TypeCategoryFunctions::select(bdem_AggregateRawPtr)' 
        //     returns the category value appropriate to the value of the 
        //     aggregate.
        //   - The following methods in 'bdeat_SequenceFunctions' produce the
        //     expected results on an AggregateRawPtr holding a list or row:
        //     - 'manipulateAttribute'
        //     - 'manipulateAttributes'
        //     - 'accessAttribute'
        //     - 'accessAttributes'
        //     - 'hasAttribute'
        //   - The following methods in 'bdeat_ChoiceFunctions' produce the
        //     expected result on an aggregate holding a choice or choice array
        //     item:
        //     - 'makeSelection'
        //     - 'manipulateSelection'
        //     - 'accessSelection'
        //     - 'hasSelection'
        //     - 'selectionId'
        //   - The following methods in 'bdeat_ArrayFunctions' produce the
        //     expected result on an aggregate holding an array, table, or
        //     choice array:
        //     - 'size'
        //     - 'resize'
        //     - 'manipulateElement'
        //     - 'accessElement'
        //   - The following methods in 'bdeat_NullableValueFunctions'
        //     return the expected results when invoked on a nullable field
        //     within an aggregate:
        //     - 'makeValue'
        //     - 'manipulateValue'
        //     - 'accessValue'
        //     - 'isNull'
        //   - The following methods in 'bdeat_EnumFunctions' set and get the
        //     correct values for an enumeration field within an aggregate:
        //     - 'fromInt'
        //     - 'fromString'
        //     - 'toInt'
        //     - 'toString'
        //   - The following methods in 'bdeat_ValueTypeFunctions' assign the
        //     correct values for a scalar aggregate and for a list aggregate:
        //     - 'assign'
        //     - 'reset'
        //   - A complex operation that uses bdeat functions works as expected.
        //
        // Plan:
        // - Construct a schema with a root sequence record containing at
        //   least one of each of the categories to be tested.
        // - Define a manipulator class that performs a standard set of
        //   manipulations on each simple type and simple recursive operations
        //   on each sequence, choice, and nullable type.
        // - Define an accessor that records the sequence of calls.
        // - Create an 'bcem_Aggregate' object that conforms to the root
        //   record of the schema.
        // - Call 'bdeat_TypeName::className(bcem_Aggregate)' on each
        //   field and check for expected result.
        // - Check that
        //   'bdeat_TypeCategory::Select<bcem_Aggregate>::
        //    BDEAT_SELECTION' is '0'.
        // - Call 'bdeat_TypeCategoryFunctions::select(bdem_AggregateRawPtr)' 
        //   on each field and check for expected result.
        // - Apply the manipulator to the aggregate and confirm that the final
        //   state of the aggregate is as expected.
        // - Apply the accessor to the aggregate and confirm that the values
        //   and sequence of accesses are as expected.
        // - To exercise the bdeat functions in a real-world scenario, encode
        //   a complex aggregate using 'bdem_berencoder' and decode
        //   it using 'bdem_berdecoderutil'.  Confirm that the decoded
        //   aggregate is equivalent to the original.
        //
        // Testing:
        //     namespace bdeat_TypeCategoryFunctions
        //     namespace bdeat_SequenceFunctions
        //     namespace bdeat_ChoiceFunctions
        //     namespace bdeat_ArrayFunctions
        //     namespace bdeat_EnumFunctions
        //     namespace bdeat_NullableValueFunctions
        //     namespace bdeat_ValueTypeFunctions
        // --------------------------------------------------------------------

        if (verbose) tst::cout << "\nTESTING 'bdeat' FUNCTIONS"
                               << "\n========================="
                               << bsl::endl;

        const char SPEC[] =
            ":f= uvx"         // ENUM "f" { "you" => 0, "vee" => 1, "ex" => 2}
            ":c? Aa&NF Bb&NF" // CHOICE "c" { CHAR "a"; SHORT "b"; }
            ":s Cc&NF Dd"     // SEQUENCE "s" { INT "c"; INT64 "d"; }
            ":r"              // SEQUENCE "r" {
            "  Fa &NF"        //     DOUBLE "a" !nullable;
            "  Fb &NT"        //     DOUBLE "b"  nullable;
            "  Gc &NF"        //     STRING "c" !nullable;
            "  Gd &NT &D1"    //     STRING "d"  nullable default="vee";
            "  +fs &NF"       //     LIST<"s"> "f" !nullable;
            "  +gs &NT"       //     LIST<"s"> "g"  nullable;
            "  %hc"           //     CHOICE<"c"> "h";
            "  Mi"            //     INT_ARRAY "i";
            "  #js"           //     TABLE<"s"> "j"
            "  @kc"           //     CHOICE_ARRAY<"c"> "k";
            "  $mf &NF"       //     INT ENUM<"f"> "m" !nullable;
            "  ^nf &NT"       //     STRING ENUM<"f"> "n" nullable;
            "  Mo &FN"        //     INT_ARRAY     "o" !nullable nillable
            "  #ps &FN"       //     TABLE<"s"> "p" nillable
            "  /qf &FN"       //     STRING_ARRAY ENUM<"f"> "q" nillable
            ;                 // }

        SchemaShdPtr schema;
        schema.createInplace(0);

        ggSchema(schema.ptr(), SPEC);
        ASSERT(5 == schema->numRecords());
        ASSERT(1 == schema->numEnumerations());
        if (veryVerbose) P(*schema);

        ASSERT(0 == TC::Select<bcem_Aggregate>::BDEAT_SELECTION);

        Obj mA1(schema, "r");  const Obj& A1 = mA1;
        mA1.setField("a", 4.0);
        mA1.setField("c", "22");
        mA1.setField("d", "33");
        mA1.setField("f", "c", 99);
        mA1.setField("f", "d", 999);
        mA1.field("g").makeValue();
        mA1.setField("g", "c", 88);
        mA1.setField("g", "d", 888);
        mA1.field("h").makeSelection("a", 77);
        mA1.field("i").append(6);
        mA1.field("i").append(5);
        mA1.field("i").append(4);
        mA1.field("j").resize(2);
        mA1.setField("j", 0, "c", 66);
        mA1.setField("j", 0, "d", 666);
        mA1.setField("j", 1, "c", 55);
        mA1.setField("j", 1, "d", 555);
        mA1.field("k").resize(2);
        mA1.field("k", 0).makeSelection("a", 44);
        mA1.field("k", 1).makeSelection("b", 333);
        mA1.setField("m", 1);    // == enumerator string "vee"
        mA1.setField("n", "ex"); // == enumerator ID 2
        ASSERT(mA1.field("i").enumerationConstraint() == 0);
        ASSERT(mA1.field("m").enumerationConstraint() ==
               schema->lookupEnumeration("f"));
        mA1.field("o").resize(2);
        mA1.setField("o", 0, "33");
        mA1.field("p").resize(2);
        mA1.field("q").resize(2);
        mA1.setField("p", 0, mA1.field("j", 0));
        mA1.setField("q", 1, "ex");

        if (veryVerbose) P(A1);

        Obj mA2(schema, "r");  const Obj& A2 = mA2;
        if (veryVerbose) P(A2);

        if (verbose) tst::cout << "Testing type name" << bsl::endl;
        ASSERT(streq("r", TN::className(A1))           );
        ASSERT(      0 == TN::className(mA1.field("a").aggregateRaw()) );
        ASSERT(      0 == TN::className(mA1.field("b").aggregateRaw()) );
        ASSERT(      0 == TN::className(mA1.field("c").aggregateRaw()) );
        ASSERT(      0 == TN::className(mA1.field("d").aggregateRaw()) );
        ASSERT(streq("s", TN::className(mA1.field("f").aggregateRaw())));
        ASSERT(streq("s", TN::className(mA1.field("g").aggregateRaw())));
        ASSERT(streq("c", TN::className(mA1.field("h").aggregateRaw())));
        ASSERT(      0 == TN::className(mA1.field("i").aggregateRaw()) );
        ASSERT(streq("s", TN::className(mA1.field("j").aggregateRaw())));
        ASSERT(streq("c", TN::className(mA1.field("k").aggregateRaw())));
        ASSERT(streq("f", TN::className(mA1.field("m").aggregateRaw())));
        ASSERT(streq("f", TN::className(mA1.field("n").aggregateRaw())));
        ASSERT(      0 == TN::className(mA1.field("o").aggregateRaw()));
        ASSERT(streq("s", TN::className(mA1.field("p").aggregateRaw())));
        ASSERT(      0 == TN::className(mA1.field("q").aggregateRaw()));

        if (verbose) tst::cout << "Testing category selection" << bsl::endl;
        ASSERT(0 ==
                  bdeat_TypeCategory::Select<bcem_Aggregate>::BDEAT_SELECTION);
        ASSERT(TC::BDEAT_SEQUENCE_CATEGORY    == TCF::select(A1));
        ASSERT(TC::BDEAT_SIMPLE_CATEGORY      == 
               TCF::select(mA1.field("a").aggregateRaw()));
        ASSERT(TC::BDEAT_SIMPLE_CATEGORY      == 
               TCF::select(mA1.field("b").aggregateRaw()));
        ASSERT(TC::BDEAT_SIMPLE_CATEGORY      == 
               TCF::select(mA1.field("c").aggregateRaw()));
        ASSERT(TC::BDEAT_SIMPLE_CATEGORY      == 
               TCF::select(mA1.field("d").aggregateRaw()));
        ASSERT(TC::BDEAT_SEQUENCE_CATEGORY    == 
               TCF::select(mA1.field("f").aggregateRaw()));
        ASSERT(TC::BDEAT_SEQUENCE_CATEGORY    == 
               TCF::select(mA1.field("g").aggregateRaw()));
        ASSERT(TC::BDEAT_CHOICE_CATEGORY      == 
               TCF::select(mA1.field("h").aggregateRaw()));
        ASSERT(TC::BDEAT_ARRAY_CATEGORY       == 
               TCF::select(mA1.field("i").aggregateRaw()));
        ASSERT(TC::BDEAT_ARRAY_CATEGORY       == 
               TCF::select(mA1.field("j").aggregateRaw()));
        ASSERT(TC::BDEAT_ARRAY_CATEGORY       == 
               TCF::select(mA1.field("k").aggregateRaw()));
        ASSERT(TC::BDEAT_ENUMERATION_CATEGORY == 
               TCF::select(mA1.field("m").aggregateRaw()));
        ASSERT(TC::BDEAT_ENUMERATION_CATEGORY == 
               TCF::select(mA1.field("n").aggregateRaw()));
        ASSERT(TC::BDEAT_ARRAY_CATEGORY       == 
               TCF::select(mA1.field("o").aggregateRaw()));
        ASSERT(TC::BDEAT_ARRAY_CATEGORY       == 
               TCF::select(mA1.field("p").aggregateRaw()));
        ASSERT(TC::BDEAT_ARRAY_CATEGORY       == 
               TCF::select(mA1.field("q").aggregateRaw()));

        AggAccessor       theAccessor;
        AggManipulator    theManipulator;
        NewAggAccessor    newAccessor;
        NewAggManipulator newManipulator;

        const bsl::string RA = "<a>4</a>";
        const bsl::string RB = "<b/>";
        const bsl::string RC = "<c>22</c>";
        const bsl::string RD = "<d>33</d>";
        const bsl::string RF = "<f><c>99</c><d>999</d></f>";
        const bsl::string RG = "<g><c>88</c><d>888</d></g>";
        const bsl::string RH = "<h><a>77</a></h>";
        const bsl::string RI = "<i>6</i><i>5</i><i>4</i>";
        const bsl::string RJ =
                        "<j><c>66</c><d>666</d></j><j><c>55</c><d>555</d></j>";
        const bsl::string RK = "<k><a>44</a></k><k><b>333</b></k>";
        const bsl::string RM = "<m>1</m>";
        const bsl::string RN = "<n>2</n>";
        const bsl::string RO = "<o>33</o><o xsi:nil='true'/>";
        const bsl::string RP = "<p><c>66</c><d>666</d></p><p xsi:nil='true'/>";
        const bsl::string RQ = "<q xsi:nil='true'/><q>2</q>";

        bsl::string res;
        if (verbose) tst::cout << "Testing bdeat_SequenceFunctions"<<bsl::endl;
        ASSERT(SF::IsSequence<bcem_Aggregate>::VALUE);

        newAccessor.reset();
        SF::accessAttribute(A1, newAccessor, "a", 1);
        res = newAccessor.value();
        LOOP2_ASSERT(RA, res, RA == res);

        newAccessor.reset();
        SF::accessAttribute(A1, newAccessor, "b", 1);
        res = newAccessor.value();
        LOOP2_ASSERT(RB, res, RB == res);

        newAccessor.reset();
        SF::accessAttribute(A1, newAccessor, "c", 1);
        res = newAccessor.value();
        LOOP2_ASSERT(RC, res, RC == res);

        newAccessor.reset();
        SF::accessAttribute(A1, newAccessor, "d", 1);
        res = newAccessor.value();
        LOOP2_ASSERT(RD, res, RD == res);

        newAccessor.reset();
        SF::accessAttribute(A1, newAccessor, "f", 1);
        res = newAccessor.value();
        LOOP2_ASSERT(RF, res, RF == res);

        newAccessor.reset();
        SF::accessAttribute(A1, newAccessor, "g", 1);
        res = newAccessor.value();
        LOOP2_ASSERT(RG, res, RG == res);

        newAccessor.reset();
        SF::accessAttribute(A1, newAccessor, "h", 1);
        res = newAccessor.value();
        LOOP2_ASSERT(RH, res, RH == res);

        newAccessor.reset();
        SF::accessAttribute(A1, newAccessor, "i", 1);
        res = newAccessor.value();
        LOOP2_ASSERT(RI, res, RI == res);

        newAccessor.reset();
        SF::accessAttribute(A1, newAccessor, "j", 1);
        res = newAccessor.value();
        LOOP2_ASSERT(RJ, res, RJ == res);

        newAccessor.reset();
        SF::accessAttribute(A1, newAccessor, "k", 1);
        res = newAccessor.value();
        LOOP2_ASSERT(RK, res, RK == res);

        newAccessor.reset();
        SF::accessAttribute(A1, newAccessor, "m", 1);
        res = newAccessor.value();
        LOOP2_ASSERT(RM, res, RM == res);

        newAccessor.reset();
        SF::accessAttribute(A1, newAccessor, "n", 1);
        res = newAccessor.value();
        LOOP2_ASSERT(RN, res, RN == res);

        newAccessor.reset();
        SF::accessAttribute(A1, newAccessor, "o", 1);
        res = newAccessor.value();
        LOOP2_ASSERT(RO, res, RO == res);

        newAccessor.reset();
        SF::accessAttribute(A1, newAccessor, "p", 1);
        res = newAccessor.value();
        LOOP2_ASSERT(RP, res, RP == res);

        newAccessor.reset();
        SF::accessAttribute(A1, newAccessor, "q", 1);
        res = newAccessor.value();
        LOOP2_ASSERT(RQ, res, RQ == res);

        SF::accessAttribute(A1, theAccessor, "b", 1);
        ASSERT(theAccessor.matchValue(-5000));
        SF::accessAttribute(mA1.field("f").aggregateRaw(), theAccessor, "c", 1);
        ASSERT(theAccessor.matchValue(99));
        SF::accessAttribute(mA1.field("f").aggregateRaw(), theAccessor, 1);
        ASSERT(theAccessor.matchValue(999));
        SF::accessAttributes(mA1.field("f").aggregateRaw(), theAccessor);
        ASSERT(theAccessor.matchValues(99, 999));

        Obj mB(schema, "r"); const bcem_Aggregate& B = mB;

        bsl::list<CERef>& values = newManipulator.elements();
        const CERef& RefA = getCERef(ET::BDEM_DOUBLE, 1);
        values.push_back(RefA);
        SF::manipulateAttribute(&mB, newManipulator, "a", 1);
        ASSERT(compareCERefs(RefA, B.field("a").asElemRef()));

        newManipulator.reset();
        const CERef& RefB = getCERef(ET::BDEM_DOUBLE, 2);
        values.push_back(RefB);
        SF::manipulateAttribute(&mB, newManipulator, "b", 1);
        ASSERT(compareCERefs(RefB, B.field("b").asElemRef()));

        newManipulator.reset();
        const CERef& RefC = getCERef(ET::BDEM_STRING, 2);
        values.push_back(RefC);
        SF::manipulateAttribute(&mB, newManipulator, "c", 1);
        LOOP2_ASSERT(RefC,
                     B.field("c").asElemRef(),
                     compareCERefs(RefC, B.field("c").asElemRef()));

        newManipulator.reset();
        const CERef& RefD = getCERef(ET::BDEM_STRING, 0);
        values.push_back(RefD);
        SF::manipulateAttribute(&mB, newManipulator, "d", 1);
        ASSERT(compareCERefs(RefD, B.field("d").asElemRef()));

        newManipulator.reset();
        const CERef& RefFC = getCERef(ET::BDEM_INT, 2);
        const CERef& RefFD = getCERef(ET::BDEM_INT64, 1);
        values.push_back(RefFC);
        values.push_back(RefFD);
        SF::manipulateAttribute(&mB, newManipulator, "f", 1);
        LOOP2_ASSERT(RefFC,
                     B.field("f", "c").asElemRef(),
                     compareCERefs(RefFC, B.field("f", "c").asElemRef()));
        ASSERT(compareCERefs(RefFD, B.field("f", "d").asElemRef()));

        newManipulator.reset();
        const CERef& RefGC = getCERef(ET::BDEM_INT, 0);
        const CERef& RefGD = getCERef(ET::BDEM_INT64, 0);
        values.push_back(RefGC);
        values.push_back(RefGD);
        SF::manipulateAttribute(&mB, newManipulator, "g", 1);
        ASSERT(compareCERefs(RefGC, B.field("g", "c").asElemRef()));
        ASSERT(compareCERefs(RefGD, B.field("g", "d").asElemRef()));

        newManipulator.reset();
        int selector = 0;
        const CERef  RefSelector(&selector, &bdem_Properties::d_intAttr);
        values.push_back(RefSelector);
        const CERef& RefH = getCERef(ET::BDEM_CHAR, 1);
        values.push_back(RefH);
        SF::manipulateAttribute(&mB, newManipulator, "h", 1);
        ASSERT(selector == B.field("h").selectorId());
        ASSERT(bsl::string("a") == B.field("h").selector());
        ASSERT(compareCERefs(RefH, B.field("h", "a").asElemRef()));

        newManipulator.reset();
        int numItems = 1;
        const CERef  RefNI(&numItems, &bdem_Properties::d_intAttr);
        const CERef& RefI = getCERef(ET::BDEM_INT, 1);
        values.push_back(RefNI);
        values.push_back(RefI);
        SF::manipulateAttribute(&mB, newManipulator, "i", 1);
        Obj resAgg = B.field("i");  const Obj& RES = resAgg;
        ASSERT(1 == RES.length());
        LOOP2_ASSERT(RefI, resAgg.asElemRef(),
                     compareCERefs(RefI, resAgg[0].asElemRef()));

        newManipulator.reset();
        numItems = 2;
        const CERef& RefJA1 = getCERef(ET::BDEM_INT, 1);
        const CERef& RefJA2 = getCERef(ET::BDEM_INT64, 1);
        const CERef& RefJB1 = getCERef(ET::BDEM_INT, 2);
        const CERef& RefJB2 = getCERef(ET::BDEM_INT64, 2);
        values.push_back(RefNI);
        values.push_back(RefJA1);
        values.push_back(RefJA2);
        values.push_back(RefJB1);
        values.push_back(RefJB2);
        SF::manipulateAttribute(&mB, newManipulator, "j", 1);
        ASSERT(2 == B.field("j").length());
        ASSERT(compareCERefs(RefJA1, B.field("j", 0, "c").asElemRef()));
        ASSERT(compareCERefs(RefJA2, B.field("j", 0, "d").asElemRef()));
        ASSERT(compareCERefs(RefJB1, B.field("j", 1, "c").asElemRef()));
        ASSERT(compareCERefs(RefJB2, B.field("j", 1, "d").asElemRef()));

        newManipulator.reset();
        int s1 = 0, s2 = 1;
        const CERef  RefS1(&s1, &bdem_Properties::d_intAttr);
        const CERef  RefS2(&s2, &bdem_Properties::d_intAttr);
        const CERef& RefKA = getCERef(ET::BDEM_CHAR, 1);
        const CERef& RefKB = getCERef(ET::BDEM_SHORT, 1);
        values.push_back(RefNI);
        values.push_back(RefS1);
        values.push_back(RefKA);
        values.push_back(RefS2);
        values.push_back(RefKB);
        SF::manipulateAttribute(&mB, newManipulator, "k", 1);
        ASSERT(numItems == B.field("k").length());

        Obj resaAgg = B.field("k", 0);  const Obj& RESA = resaAgg;
        Obj resbAgg = B.field("k", 1);  const Obj& RESB = resbAgg;

        ASSERT(s1 == RESA.selectorId());
        ASSERT(bsl::string("a") == RESA.selector());
        LOOP_ASSERT(RefKA, compareCERefs(RefKA, 
                                         resaAgg.selection().asElemRef()));
        ASSERT(s2 == RESB.selectorId());
        ASSERT(bsl::string("b") == RESB.selector());
        LOOP2_ASSERT(RefKB, resbAgg.selection().asElemRef(),
                     compareCERefs(RefKB, 
                                   resbAgg.selection().asElemRef()));

        newManipulator.reset();
        int enumId = 1;
        const CERef enumRef1(&enumId, &bdem_Properties::d_intAttr);
        values.push_back(enumRef1);
        SF::manipulateAttribute(&mB, newManipulator, "m", 1);
        LOOP2_ASSERT(enumRef1, B.field("m").asElemRef(),
                     compareCERefs(enumRef1, B.field("m").asElemRef()));
        enumId = 2;
        values.push_back(enumRef1);
        SF::manipulateAttribute(&mB, newManipulator, "m", 1);
        LOOP2_ASSERT(enumRef1, B.field("m").asElemRef(),
                     compareCERefs(enumRef1, B.field("m").asElemRef()));

        newManipulator.reset();
        enumId = 1;
        bsl::string enumString = "1";
        const CERef enumRef2(&enumString, &bdem_Properties::d_stringAttr);
        values.push_back(enumRef2);
        SF::manipulateAttribute(&mB, newManipulator, "m", 1);
        LOOP2_ASSERT(enumRef1, B.field("m").asElemRef(),
                     compareCERefs(enumRef1, B.field("m").asElemRef()));
        enumId = 2;
        enumString = "2";
        values.push_back(enumRef2);
        SF::manipulateAttribute(&mB, newManipulator, "m", 1);
        LOOP2_ASSERT(enumRef1, B.field("m").asElemRef(),
                     compareCERefs(enumRef1, B.field("m").asElemRef()));

        newManipulator.reset();
        enumId = 1;
        values.push_back(enumRef1);
        SF::manipulateAttribute(&mB, newManipulator, "n", 1);
        ASSERT(bsl::string("vee") == B.field("n").asElemRef().theString());

        newManipulator.reset();
        enumId = 2;
        values.push_back(enumRef1);
        SF::manipulateAttribute(&mB, newManipulator, "n", 1);
        LOOP_ASSERT(B.field("n").asElemRef().theString(),
                    bsl::string("ex") == B.field("n").asElemRef().theString());

        newManipulator.reset();
        enumString = "1";
        values.push_back(enumRef2);
        SF::manipulateAttribute(&mB, newManipulator, "n", 1);
        ASSERT(bsl::string("vee") == B.field("n").asElemRef().theString());

        newManipulator.reset();
        enumString = "2";
        values.push_back(enumRef2);
        SF::manipulateAttribute(&mB, newManipulator, "n", 1);
        LOOP_ASSERT(B.field("n").asElemRef().theString(),
                    bsl::string("ex") == B.field("n").asElemRef().theString());

        newManipulator.reset();
        numItems = 2;
        const CERef& RefO = getCERef(ET::BDEM_INT, 0);
        values.push_back(RefNI);
        values.push_back(RefI);
        values.push_back(RefO);
        SF::manipulateAttribute(&mB, newManipulator, "o", 1);
        ASSERT(2 == B.field("o").length());
        LOOP_ASSERT(B.field("o", 0).asElemRef(),
                    compareCERefs(RefI, B.field("o", 0).asElemRef()));
        LOOP_ASSERT(B.field("o", 1).asElemRef(),
                    compareCERefs(RefO, B.field("o", 1).asElemRef()));

        newManipulator.reset();
        numItems = 2;
        values.push_back(RefNI);
        values.push_back(RefJA1);
        values.push_back(RefJA2);
        values.push_back(RefJB1);
        values.push_back(RefJB2);
        SF::manipulateAttribute(&mB, newManipulator, "p", 1);
        ASSERT(2 == B.field("p").length());
        ASSERT(compareCERefs(RefJA1, B.field("p", 0, "c").asElemRef()));
        ASSERT(compareCERefs(RefJA2, B.field("p", 0, "d").asElemRef()));
        ASSERT(compareCERefs(RefJB1, B.field("p", 1, "c").asElemRef()));
        ASSERT(compareCERefs(RefJB2, B.field("p", 1, "d").asElemRef()));

        newManipulator.reset();
        numItems = 1;
        enumString = "1";
        values.push_back(RefNI);
        values.push_back(enumRef2);
        SF::manipulateAttribute(&mB, newManipulator, "q", 1);
        ASSERT(1 == B.field("q").length());
        ASSERT(bsl::string("vee") == B.field("q", 0).asElemRef().theString());

        bcem_Aggregate mA2f = mA2.field("f");
        SF::manipulateAttribute(&mA2f, theManipulator,  0);
        ASSERT(100 == mA2.field("f", "c").asInt());
        SF::manipulateAttribute(&mA2f, theManipulator,  "d", 1);
        ASSERT(101 == mA2.field("f", "d").asInt64());
        SF::manipulateAttributes(&mA2f, theManipulator);
        ASSERT(102 == mA2.field("f", "c").asInt());
        ASSERT(103 == mA2.field("f", "d").asInt64());
        if (veryVerbose) P(A2);

        if (verbose) tst::cout << "Testing bdeat_ChoiceFunctions" << bsl::endl;
        ASSERT(CF::IsChoice<bcem_Aggregate>::VALUE);
        int id = 99;
        id = CF::selectionId(mA1.field("h").aggregateRaw());
        ASSERT(0 == id);
        ASSERT(CF::hasSelection(mA1.field("h").aggregateRaw(), 0));
        ASSERT(CF::hasSelection(mA1.field("h").aggregateRaw(), 1));
        ASSERT(! CF::hasSelection(mA1.field("h").aggregateRaw(), 2));
        ASSERT(CF::hasSelection(mA1.field("h").aggregateRaw(), "a", 1));
        ASSERT(CF::hasSelection(mA1.field("h").aggregateRaw(), "b", 1));
        ASSERT(! CF::hasSelection(mA1.field("h").aggregateRaw(), "ab", 2));
        CF::accessSelection(mA1.field("h").aggregateRaw(), theAccessor);
        ASSERT(theAccessor.matchValue(77));

        bcem_Aggregate mA2h(mA2.field("h"));
        id = CF::selectionId(mA2h);
        ASSERT(bdeat_ChoiceFunctions::BDEAT_UNDEFINED_SELECTION_ID == id);
        CF::makeSelection(&mA2h, 0);
        ASSERT(0 == CF::selectionId(mA2h));
        CF::manipulateSelection(&mA2h, theManipulator);
        ASSERT(104 == mA2.field("h", "a").asChar());
        CF::makeSelection(&mA2h, "b", 1);
        ASSERT(1 == CF::selectionId(mA2h));
        CF::manipulateSelection(&mA2h, theManipulator);
        ASSERT(105 == mA2.field("h", "").asShort());
        if (veryVerbose) P(A2);

        if (verbose) tst::cout << "Testing bdeat_ArrayFunctions" << bsl::endl;
        ASSERT(AF::IsArray<bcem_Aggregate>::VALUE);
        ASSERT(3 == AF::size(mA1.field("i").aggregateRaw()));
        AF::accessElement(mA1.field("i").aggregateRaw(), theAccessor, 0);
        ASSERT(theAccessor.matchValue(6));
        AF::accessElement(mA1.field("i").aggregateRaw(), theAccessor, 2);
        ASSERT(theAccessor.matchValue(4));
        AF::accessElement(mA1.field("i").aggregateRaw(), theAccessor, 1);
        ASSERT(theAccessor.matchValue(5));

        mA2.setField("i", mA1.field("i"));
        bcem_Aggregate mA2i(mA2.field("i"));
        AF::manipulateElement(&mA2i, theManipulator, 1);
        ASSERT(6   == mA2.field("i", 0).asInt());
        ASSERT(106 == mA2.field("i", 1).asInt());
        ASSERT(4   == mA2.field("i", 2).asInt());
        AF::resize(&mA2i, 2);
        ASSERT(2   == mA2.field("i").size());
        ASSERT(6   == mA2.field("i", 0).asInt());
        ASSERT(106 == mA2.field("i", 1).asInt());
        if (veryVerbose) P(A2);

        if (verbose) tst::cout << "Testing bdeat_ArrayFunctions on TABLEs"
                               << bsl::endl;
        ASSERT(2 == AF::size(mA1.field("j").aggregateRaw()));
        AF::accessElement(mA1.field("j").aggregateRaw(), theAccessor, 0);
        ASSERT(theAccessor.matchValues(66, 666));
        AF::accessElement(mA1.field("j").aggregateRaw(), theAccessor, 1);
        ASSERT(theAccessor.matchValues(55, 555));

        mA2.setField("j", mA1.field("j"));
        bcem_Aggregate mA2j(mA2.field("j"));
        AF::manipulateElement(&mA2j, theManipulator, 0);
        ASSERT(107 == mA2.field("j", 0, "c").asInt());
        ASSERT(108 == mA2.field("j", 0, "d").asInt64());
        ASSERT(55  == mA2.field("j", 1, "c").asInt());
        ASSERT(555 == mA2.field("j", 1, "d").asInt64());
        AF::resize(&mA2j, 3);
        ASSERT(3   == mA2.field("j").size());
        AF::manipulateElement(&mA2j, theManipulator, 2);
        ASSERT(107 == mA2.field("j", 0, "c").asInt());
        ASSERT(108 == mA2.field("j", 0, "d").asInt64());
        ASSERT(55  == mA2.field("j", 1, "c").asInt());
        ASSERT(555 == mA2.field("j", 1, "d").asInt64());
        ASSERT(109 == mA2.field("j", 2, "c").asInt());
        ASSERT(110 == mA2.field("j", 2, "d").asInt64());
        if (veryVerbose) P(A2);

        if (verbose) tst::cout << "Testing bdeat_ArrayFunctions on "
                               << "CHOICE_ARRAYs" << bsl::endl;
        ASSERT(2 == AF::size(mA1.field("k").aggregateRaw()));
        ASSERT(0 == CF::selectionId(mA1.field("k", 0).aggregateRaw()));
        ASSERT(1 == CF::selectionId(mA1.field("k", 1).aggregateRaw()));
        AF::accessElement(mA1.field("k").aggregateRaw(), theAccessor, 0);
        ASSERT(theAccessor.matchValue(44));
        AF::accessElement(mA1.field("k").aggregateRaw(), theAccessor, 1);
        ASSERT(theAccessor.matchValue(333));

        mA2.setField("k", mA1.field("k"));
        bcem_Aggregate mA2k(mA2.field("k"));
        AF::manipulateElement(&mA2k, theManipulator, 0);
        ASSERT(1   == mA2.field("k", 0).selectorId());
        ASSERT(111 == mA2.field("k", 0, "b").asShort());
        ASSERT(1   == mA2.field("k", 1).selectorId());
        ASSERT(333 == mA2.field("k", 1, "b").asShort());
        if (veryVerbose) P(A2);
        AF::manipulateElement(&mA2k, theManipulator, 1);
        ASSERT(1   == mA2.field("k", 0).selectorId());
        ASSERT(111 == mA2.field("k", 0, "b").asShort());
        ASSERT(0   == mA2.field("k", 1).selectorId());
        ASSERT(112 == mA2.field("k", 1, "a").asChar());
        AF::resize(&mA2k, 3);
        ASSERT(3   == mA2.field("k").size());
        AF::manipulateElement(&mA2k, theManipulator, 2);
        ASSERT(1   == mA2.field("k", 0).selectorId());
        ASSERT(111 == mA2.field("k", 0, "b").asShort());
        ASSERT(0   == mA2.field("k", 1).selectorId());
        ASSERT(112 == mA2.field("k", 1, "a").asChar());
        ASSERT(1   == mA2.field("k", 2).selectorId());
        ASSERT(113 == mA2.field("k", 2, "b").asShort());
        if (veryVerbose) P(A2);

        // WHITE-BOX test of bcem_Aggregate_BdeatUtil::NullableAdapter
        if (verbose) tst::cout << "Testing bdeat_NullableValueFunctions:"
                               << bsl::endl;
        typedef bcem_Aggregate_NullableAdapter NullableAdapter;
        ASSERT(! NVF::IsNullableValue<bcem_Aggregate>::VALUE);
        ASSERT(NVF::IsNullableValue<NullableAdapter>::VALUE);
        bcem_Aggregate mA1b(mA1.field("b"));
        bcem_Aggregate mA1br(mA1.field("b"));
        NullableAdapter nmA1b  = { &mA1br };
        ASSERT(NVF::isNull(nmA1b));
        bcem_Aggregate mA1g(mA1.field("g"));
        bcem_Aggregate mA1gr = mA1g;
        NullableAdapter nmA1g  = { &mA1gr };
        ASSERT(!NVF::isNull(nmA1g));
        bcem_Aggregate mA1gd(mA1.field("g", "d"));
        NullableAdapter nmA1gd = { &mA1gd };
        ASSERT(! NVF::isNull(nmA1gd));
        NVF::accessValue(nmA1gd, theAccessor);
        ASSERT(theAccessor.matchValue(888));
        NVF::accessValue(nmA1g, theAccessor);
        ASSERT(theAccessor.matchValues(88, 888));

        bcem_Aggregate mA2b(mA2.field("b"));
        NullableAdapter nmA2b  = { &mA2b };
        ASSERT(NVF::isNull(nmA2b));
        NVF::manipulateValue(&nmA2b, theManipulator);
        ASSERT(! NVF::isNull(nmA2b));
        ASSERT(114 == mA2b.asDouble());
        // Note: mA2b is a COPY.  A2.field("b") is still null.
        bcem_Aggregate mA2g(mA2.field("g"));
        NullableAdapter nmA2g  = { &mA2g };
        ASSERT(NVF::isNull(nmA2g));
        mA2g.makeNull();
        NVF::makeValue(&nmA2g);
        ASSERT(! NVF::isNull(nmA2g));
        NVF::manipulateValue(&nmA2g, theManipulator);
        ASSERT(!mA2.field("g", "d").isNul2());
        ASSERT(115 == mA2.field("g", "c").asInt());
        ASSERT(116 == mA2.field("g", "d").asInt64());

        // Black-box test of null value manipulator through parent sequence
        mA2g.makeNull();
        ASSERT(NVF::isNull(nmA2g));
        mA2.field("g").makeValue();

        SF::manipulateAttribute(&mA2, theManipulator, "g", 1);
        ASSERT(! mA2.field("g").isNul2());
        ASSERT(! mA2.field("g", "d").isNul2());
        ASSERT(117 == mA2.field("g", "c").asInt());
        ASSERT(118 == mA2.field("g", "d").asInt64());
        if (veryVerbose) P(A2);

        if (verbose) tst::cout << "Testing bdeat_EnumFunctions:" << bsl::endl;
        ASSERT(EF::IsEnumeration<bcem_Aggregate>::VALUE);
        int intValue;
        bsl::string stringValue;
        EF::toInt(&intValue, mA1.field("m").aggregateRaw());
        ASSERT(1 == intValue);
        EF::toString(&stringValue, mA1.field("m").aggregateRaw());
        ASSERT("vee" == stringValue);
        EF::toInt(&intValue, mA1.field("n").aggregateRaw());
        ASSERT(2 == intValue);
        EF::toString(&stringValue, mA1.field("n").aggregateRaw());
        ASSERT("ex" == stringValue);

        bcem_Aggregate mA2magg(mA2.field("m")); 
        const bcem_Aggregate &A2m = mA2magg;
        bcem_Aggregate mA2mr = mA2magg;
        int status;
        status = EF::fromInt(&mA2mr, 0);
        ASSERT(0 == status);
        ASSERT(0 == A2m.asInt());
        status = EF::fromString(&mA2mr, "ex", 2);
        ASSERT(0 == status);
        ASSERT(2 == A2m.asInt());
        status = EF::fromInt(&mA2mr, 3);
        ASSERT(0 != status);
        ASSERT(2 == A2m.asInt());
        status = EF::fromString(&mA2mr, "doubleU", 7);
        ASSERT(0 != status);
        ASSERT(2 == A2m.asInt());

        bcem_Aggregate mA2n(mA2.field("n")); 
        const bcem_Aggregate &A2n = mA2n;
        bcem_Aggregate mA2nr = mA2n;
        status = EF::fromInt(&mA2nr, 1);
        ASSERT(0 == status);
        ASSERT("vee" == A2n.asString());
        status = EF::fromString(&mA2nr, "you", 3);
        ASSERT(0 == status);
        ASSERT("you" == A2n.asString());
        status = EF::fromInt(&mA2nr, 3);
        ASSERT(0 != status);
        ASSERT("you" == A2n.asString());
        status = EF::fromString(&mA2nr, "doubleU", 7);
        ASSERT(0 != status);
        ASSERT("you" == A2n.asString());

        // Test enumeration operations using accessors from parent
        SF::accessAttribute(A1, theAccessor, "m", 1);
        ASSERT(theAccessor.matchValue(1));
        SF::accessAttribute(A1, theAccessor, "n", 1);
        ASSERT(theAccessor.matchValue(-2));

        // Test enumeration operations using manipulators from parent
        theManipulator.reset(2);
        SF::manipulateAttribute(&mA2, theManipulator, "m", 1);
        ASSERT(2 == mA2.field("m").asInt())
        theManipulator.reset(0);
        SF::manipulateAttribute(&mA2, theManipulator, "n", 1);
        ASSERT("you" == mA2.field("n").asString())

        if (veryVerbose) P(A2);

        if (verbose) tst::cout << "Testing bdeat_ValueTypeFunctions on STRING"
                               << bsl::endl;
        bcem_Aggregate mA3d(mA1["d"].clone());
        const bcem_Aggregate& A3d = mA3d;
        ASSERT("33" == A3d.asString());
        if (veryVerbose) {
            bsl::cout << "A3d.recordDef()=";
            A3d.recordDef().print(bsl::cout) << bsl::endl;
        }
        
        bdeat_ValueTypeFunctions::reset(&mA3d);
        ASSERT(ET::BDEM_STRING == A3d.dataType());
        LOOP_ASSERT(A3d.asString(), "vee" == A3d.asString());
        bdeat_ValueTypeFunctions::assign(&mA3d, mA1["c"]);
        ASSERT("22" == A3d.asString());
        ASSERT(A3d.data() != mA1["c"].data());  // replace with areIdentical.

        if (verbose) tst::cout << "Testing BER encoding/decoding" << bsl::endl;
        bsl::stringstream strm;
        bdem_BerEncoderOptions berEncoderOptions;
        berEncoderOptions.setTraceLevel(veryVeryVerbose);
        bdem_BerEncoder berEncoder(&berEncoderOptions);
        status = berEncoder.encode(strm, A1);
        bsl::cerr << berEncoder.loggedMessages();
        ASSERT(0 == status);
        // Assert that enumerator "ex" is NOT stored in alpha form in stream.
        ASSERT(bsl::string::npos == strm.str().find("ex"));

        bcem_Aggregate mA4(schema, "r"); const bcem_Aggregate& A4 = mA4;
        ASSERT(! bcem_Aggregate::areEquivalent(mA1, A4));
        bcem_Aggregate a4Ptr = A4;

        bdem_BerDecoderOptions berDecoderOptions;
        berDecoderOptions.setTraceLevel(veryVeryVerbose);
        bdem_BerDecoder berDecoder(&berDecoderOptions);
        status = berDecoder.decode(strm, &a4Ptr);
        bsl::cerr << berDecoder.loggedMessages();
        ASSERT(0 == status);

        bsl::ostringstream A1str, A4str;
        A1str << A1;
        A4str << a4Ptr;
        
        LOOP2_ASSERT(A1, a4Ptr, 
                     A1str.str() == A4str.str());

        if (veryVerbose) P(a4Ptr);
}

static void testCase34(bool verbose, bool veryVerbose, bool veryVeryVerbose)
{
    // --------------------------------------------------------------------
    // TESTING 'accessArray(RawPtr)'
    //
    // Concerns:
    //   - bdeat_arrayAccessElement() works properly when the argument is 
    //     bcem_AggregateRaw.
    //     
    //
    // Plan:
    //
    // Testing:
    //
    //  bdeat_arrayAccessElement(bcem_AggregateRaw, ...)
    // --------------------------------------------------------------------

    if (verbose) tst::cout << "\nTESTING 'accessArray(RawPtr)'"
                           << "\n=============================" << bsl::endl;
    
    bslma_TestAllocator da("da", veryVeryVerbose);
    bslma_DefaultAllocatorGuard dag(&da);
    bslma_TestAllocator ta("ta", veryVeryVerbose);

    for (int numElements = 1; numElements <= 3; ++numElements) {

        bsl::vector<int> scalarArray;
        int expectedSum = 0;
        for (int i = 0; i < numElements; ++i) {
            scalarArray.push_back(i);
            expectedSum += i;
        }
        bcem_Aggregate scalarArrayAggregate(bdem_ElemType::BDEM_INT_ARRAY, 
                                            scalarArray, &ta);

        bcem_AggregateRaw data = scalarArrayAggregate.aggregateRaw();
        
        Accumulator accumulator;

        ASSERT(TC::BDEAT_ARRAY_CATEGORY == TCF::select(data));
        ASSERT(AF::size(data) == numElements);

        for (int i = 0; i < numElements; ++i) {
            ASSERT(0 == AF::accessElement(data, accumulator, i));
        }
        ASSERT(expectedSum == accumulator.d_count);
    }
}

static void testCase33(bool verbose, bool veryVerbose, bool veryVeryVerbose)
{
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
    // --------------------------------------------------------------------
    // TESTING 'isUnset':
    //
    // Concerns:
    //   - isUnset returns the correct value
    //
    // Plan:
    //
    // Testing:
    //   bool isUnset() const;
    // --------------------------------------------------------------------

    if (verbose) tst::cout << "\nTESTING 'isUnset'"
                           << "\n=================" << bsl::endl;

    // For a unset aggregate
    {
        Obj mX; const Obj& X = mX;
        Obj mY; const Obj& Y = mY;
        ASSERT(X.isUnset());
        ASSERT(Y.isUnset());
        mX.reset();
        ASSERT(ET::BDEM_VOID == X.dataType());
        ASSERT(ConstRecDefShdPtr() == X.recordDefPtr());
        ASSERT(VoidDataShdPtr() == X.dataPtr());
        ASSERT(X.isUnset());

        mY.reset();
        ASSERT(ET::BDEM_VOID == Y.dataType());
        ASSERT(ConstRecDefShdPtr() == Y.recordDefPtr());
        ASSERT(VoidDataShdPtr() == Y.dataPtr());
        ASSERT(Y.isUnset());
    }

    const struct {
            int         d_line;
            const char *d_spec;
    } DATA[] = {
        // Line     Spec
        // ----     ----
        {   L_,   ":aCa" },
        {   L_,   ":aGa" },
        {   L_,   ":aHa" },
        {   L_,   ":aNa" },
        {   L_,   ":aPa" },
        {   L_,   ":aQa" },
        {   L_,   ":aRa" },
        {   L_,   ":aUa" },
        {   L_,   ":aVa" },
        {   L_,   ":aWa" },
        {   L_,   ":aaa" },
        {   L_,   ":aea" },
        {   L_,   ":afa" },
        {   L_,   ":aCbFcGdQf :g+ha" },
        {   L_,   ":aCbFcGdQf :g#ha" },
        {   L_,   ":a?CbFcGdQf :g%ha" },
        {   L_,   ":a?CbFcGdQf :g@ha" },
    };
    const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

    for (int i = 0; i < NUM_DATA; ++i) {
        const int   LINE = DATA[i].d_line;
        const char *SPEC = DATA[i].d_spec;

        Schema schema; const Schema& SCHEMA = schema;
        ggSchema(&schema, SPEC);
        const RecDef *RECORD = &SCHEMA.record(SCHEMA.length() - 1);

        ConstRecDefShdPtr crp(RECORD, NilDeleter(), 0);
        const ConstRecDefShdPtr& CRP = crp;

        ET::Type TYPE = RECORD->field(0).elemType();
        const char *fldName = RECORD->fieldName(0);

        const CERef VN = getCERef(TYPE, 0);
        const CERef VA = getCERef(TYPE, 1);

        if (veryVerbose) { T_ P_(SPEC) P(SCHEMA) };

        bslma_TestAllocator t(veryVeryVerbose);
        Obj mX(CRP, &t); const Obj& X = mX;
        Obj mY(CRP, &t); const Obj& Y = mY;
        ASSERT(!X.isUnset());
        ASSERT(!Y.isUnset());

        if (veryVerbose) { T_ P_(X) P(Y) };

        if (!ET::isAggregateType(TYPE)) {
            ASSERT(X.field(fldName).isUnset());
            mX.setField(fldName, VN);
            ASSERT(X.field(fldName).isUnset());
        }
        else if (ET::isArrayType(TYPE)) {
            X.field(fldName).resize(1);
            Y.field(fldName).resize(1);
            ASSERT(X.field(fldName, 0).isUnset());
            ASSERT(Y.field(fldName, 0).isUnset());
            if (veryVerbose) { T_ P_(X) P(Y) };
        }

        mX.setField(fldName, VA);
        ASSERT(!X.field(fldName).isUnset());

        if (ET::isAggregateType(TYPE) && ET::isArrayType(TYPE)) {
            ASSERT(!X.field(fldName, 0).isUnset());
            ASSERT(Y.field(fldName, 0).isUnset());
        }

        if (veryVerbose) { T_ P_(X) P(Y) };

        mY.setField(fldName, VA);
        Obj mA = Y.field(fldName); const Obj& A = mA;

        if (ET::isAggregateType(TYPE) && ET::isArrayType(TYPE)) {
            ASSERT(!X.field(fldName, 0).isUnset());
            ASSERT(!Y.field(fldName, 0).isUnset());
        }
        if (veryVerbose) { T_ P_(X) P(Y) };

        ASSERT(!A.isUnset());
        ASSERT(compareCERefs(VA, A.asElemRef()));

        if (!ET::isAggregateType(TYPE)) {
            mX.setField(fldName, VN);
            LOOP_ASSERT(X, X.field(fldName).isUnset());

            mY.setField(fldName, VN);
            mA = Y.field(fldName);
            LOOP_ASSERT(A, A.isUnset());
            ASSERT(compareCERefs(VN, A.asElemRef()));
        }
    }
#else 
    if (verbose) tst::cout << "\n(nil test case)"
                           << "\n===============" << bsl::endl;
#endif
}

static void testCase32(bool verbose, bool veryVerbose, bool veryVeryVerbose)
{
    // --------------------------------------------------------------------
    // TESTING 'makeError' error message:
    //
    // Concerns:
    //: 1 'makeError' prints the expected error message.
    //
    // Plan:
    //
    // Testing:
    // --------------------------------------------------------------------

    if (verbose) tst::cout << "\nTESTING 'makeError'"
                           << "\n===================" << bsl::endl;

    {
        const char        AA = 'X';
              bdet_Date   BB(1, 1, 1);
        const bsl::string S1("Invalid conversion to CHAR from DATE");
        const bsl::string S2("Invalid conversion when setting "
                             "CHAR value from DATE value");

        {
            Obj mX(ET::BDEM_CHAR, BB);  const Obj& X = mX;

            ASSERT(X.isError());
            ASSERT(ErrorCode::BCEM_BAD_CONVERSION == X.errorCode());
            ASSERT(S1 == X.errorMessage());

            Obj mY(ET::BDEM_CHAR, AA);  const Obj& Y = mY;
            const Obj ERR = Y.setValue(BB);

            ASSERT(!Y.isError());
            ASSERT(ERR.isError());
            ASSERT(ErrorCode::BCEM_BAD_CONVERSION == ERR.errorCode());
            ASSERT(S2 == ERR.errorMessage());
        }

        {
            CERef CER(&BB, EAL::lookupTable()[ET::BDEM_DATE]);
            Obj mX(ET::BDEM_CHAR, CER);  const Obj& X = mX;

            ASSERT(X.isError());
            ASSERT(ErrorCode::BCEM_BAD_CONVERSION == X.errorCode());
            ASSERT(S1 == X.errorMessage());

            Obj mY(ET::BDEM_CHAR, AA);  const Obj& Y = mY;
            const Obj ERR = Y.setValue(BB);

            ASSERT(!Y.isError());
            ASSERT(ERR.isError());
            ASSERT(ErrorCode::BCEM_BAD_CONVERSION == ERR.errorCode());
            ASSERT(S2 == ERR.errorMessage());
        }

        {
            ERef ER(&BB, EAL::lookupTable()[ET::BDEM_DATE]);
            Obj mX(ET::BDEM_CHAR, ER);  const Obj& X = mX;

            ASSERT(X.isError());
            ASSERT(ErrorCode::BCEM_BAD_CONVERSION == X.errorCode());
            LOOP_ASSERT(X.errorMessage(), S1 == X.errorMessage());

            Obj mY(ET::BDEM_CHAR, AA);  const Obj& Y = mY;
            const Obj ERR = Y.setValue(BB);

            ASSERT(!Y.isError());
            ASSERT(ERR.isError());
            ASSERT(ErrorCode::BCEM_BAD_CONVERSION == ERR.errorCode());
            ASSERT(S2 == ERR.errorMessage());
        }

        {
            Obj mZ(ET::BDEM_DATE, BB);
            Obj mX(ET::BDEM_CHAR, mZ);  const Obj& X = mX;

            ASSERT(X.isError());
            ASSERT(ErrorCode::BCEM_BAD_CONVERSION == X.errorCode());
            LOOP2_ASSERT(S1, X.errorMessage(), 
                         S1 == X.errorMessage());

            Obj mY(ET::BDEM_CHAR, AA);  const Obj& Y = mY;
            const Obj ERR = Y.setValue(BB);

            ASSERT(!Y.isError());
            ASSERT(ERR.isError());
            ASSERT(ErrorCode::BCEM_BAD_CONVERSION == ERR.errorCode());
            ASSERT(S2 == ERR.errorMessage());
        }
    }
}

static void testCase31(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
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

        if (verbose) tst::cout << "\nUSAGE EXAMPLE"
                               << "\n=============" << bsl::endl;

// In this example, we will create a schema for a postal-service address
// record, then create a 'bcem_Aggregate' object that conforms to that schema
// and demonstrate accessing and modifying the aggregate.  The address record
// contains a street address, a city, state or province, country, postal code
// and an array of entities at that address.  Each entity can be either a
// company or a person.  A company has a name and postal account number.  A
// person has a name and birth date.  The XML schema description looks like
// this:
//..
//  <?xml version="1.0" encoding="UTF-8"?>
//  <schema xmlns="http://www.w3.org/2001/XMLSchema"
//          xmlns:bb="http://bloomberg.com/schemas/xyz"
//          targetNamespace="http://bloomberg.com/schemas/xyz"
//          elementFormDefault="qualified">
//
//      <complexType name="Company">
//          <sequence>
//              <element name="Name" type="string"/>
//              <element name="AccountNum" type="int"/>
//          </sequence>
//      </complexType>
//
//      <complexType name="Person">
//          <sequence>
//              <element name="LastName" type="string"/>
//              <element name="FirstName" type="string"/>
//              <element name="BirthDate" type="date"/>
//          </sequence>
//      </complexType>
//
//      <complexType name="Entity">
//          <choice>
//              <element name="Corp" type="bb:Company"/>
//              <element name="Human" type="bb:Person"/>
//          </choice>
//      </complexType>
//
//      <complexType name="Address">
//          <sequence>
//              <element name="StreetAddress" type="string"/>
//              <element name="City" type="string"/>
//              <element name="Province" type="string"/>
//              <element name="Country" type="string"/>
//              <element name="PostalCode" type="int"/>
//              <element name="Entities" type="bb:Entity"
//                       maxOccurs="unbounded"/>
//          </sequence>
//      </complexType>
//
//      <element name="Address" type="bb:Address"/>
//
//  </schema>
//..
// We could create the bdem_Schema object from this XML schema, but we'll do
// it manually for this example:
//..
    bcema_SharedPtr<bdem_Schema> schema(new bdem_Schema);

    bdem_RecordDef *company = schema->createRecord("Company");
    company->appendField(bdem_ElemType::BDEM_STRING, "Name");
    company->appendField(bdem_ElemType::BDEM_INT,    "AccountNum");

    bdem_RecordDef *person = schema->createRecord("Person");
    person->appendField(bdem_ElemType::BDEM_STRING, "LastName");
    person->appendField(bdem_ElemType::BDEM_STRING, "FirstName");
    person->appendField(bdem_ElemType::BDEM_DATE,   "BirthDate");

    // The "entity" recordDef describes a choice
    bdem_RecordDef *entity =
        schema->createRecord("Entity", bdem_RecordDef::BDEM_CHOICE_RECORD);
    entity->appendField(bdem_ElemType::BDEM_LIST, company, "Corp");
    entity->appendField(bdem_ElemType::BDEM_LIST, person,  "Human");

    bdem_RecordDef *address = schema->createRecord("Address");
    address->appendField(bdem_ElemType::BDEM_STRING,       "StreetAddress");
    address->appendField(bdem_ElemType::BDEM_STRING,       "City");
    address->appendField(bdem_ElemType::BDEM_STRING,       "Province");
    address->appendField(bdem_ElemType::BDEM_STRING,       "Country");
    address->appendField(bdem_ElemType::BDEM_INT,          "PostalCode");
    address->appendField(bdem_ElemType::BDEM_CHOICE_ARRAY, entity, "Entities");

//..
// Now we create and populate a simple aggregate object representing a person.
// The fields are accessed by name using the square-bracket ('[]') operator.
// 'setValue' is used to modify an aggregate's value and 'asString' is used to
// retrieve the value as a string (converting to string if necessary).
//..
    bcem_Aggregate michael(schema, "Person");
    ASSERT(michael.dataType() == bdem_ElemType::BDEM_LIST);
    ASSERT(michael.length() == 3);
    michael["LastName"].setValue("Bloomberg");
    michael["FirstName"].setValue("Michael");
    ASSERT("Bloomberg" == michael["LastName"].asString());
    ASSERT("Michael"   == michael["FirstName"].asString());
//..
// The 'field' method can be used instead of one or more '[]' operations.
// Similarly, the 'setField' method combines the effect of one or more '[]'
// operations and the 'setValue' method.  Both methods are slightly more
// efficient than using 'operator[]', especially when navigating down several
// levels.
//..
    michael.setField("BirthDate", bdet_Date(1942, 2, 14));
    ASSERT(bdet_Date(1942, 2, 14) == michael.field("BirthDate").asDate());
//..
// Getting more sophisticated, we create and populate a schema representing an
// address.  We begin with the simple scalar fields:
//..
    bcem_Aggregate addr1(schema, "Address");
    addr1.setField("StreetAddress", "499 Park Ave.");  // Before the move
    addr1.setField("City", "New York");
    addr1.setField("Province", "NY");
    addr1.setField("Country", "USA");
    addr1.setField("PostalCode", 10022);
//..
// A 'bcem_Aggregate' object does not actually hold data.  Rather it holds a
// smart reference to some other anonymous data, similar to the way a pointer
// holds the address of anonymous data, but without the need for dereferencing
// operators ('*' or '->').  A 'bcem_Aggregate' can also hold a reference to a
// sub-part of the data referenced by another 'bcem_Aggregate'.  To simplify
// access to the "Entities" choice array, retrieve the empty array from
// 'addr1' and keep a reference to this sub-aggregate in another
// 'bcem_Aggregate' object.  'bcem_Aggregate' uses reference counting so that
// the sub-aggregate will remain valid even if the original aggregate goes out
// of scope:
//..
    bcem_Aggregate entities = addr1.field("Entities");
    entities.appendItems(2);
    ASSERT(2 == addr1.field("Entities").length());
//..
// We set the first entity to be a company, and we set its name:
//..
    entities[0].makeSelection("Corp").setField("Name", "Bloomberg LP");
    ASSERT(0 == bsl::strcmp("Corp", entities[0].selector()));
//..
// We set the second entity to be a person and set its value from the
// 'michael' aggregate.  Note that the copy of 'michael' stored in the choice
// item is independent of the data referred to by 'michael'; 'setField',
// 'setValue', 'makeSelection', and other methods have value semantics and
// their operation should not be confused with assignment.
//..
    entities[1].makeSelection("Human", michael);
    ASSERT(0 == bsl::strcmp("Human", entities[1].selector()));
//..
// Array items or table rows can be accessed by using the '[]' operator or by
// supplying an integer argument to the 'field' function.  The current
// selection in a choice can be accessed using the 'selection()' method, by
// supplying the selector name to the 'field' method or the '[]', or by
// supplying the empty string ("") to the 'field' method or '[]' operator.
// The 'field' method provides convenient access to data up to 10 levels deep
// in nested aggregates.  All data types can be accessed as strings using the
// 'asString' function:
//..
    bsl::string birthday = addr1.field("Entities")[1].selection().
                                 field("BirthDate").asString();
    const bcem_Aggregate bbAgg = addr1.field("Entities", 0, "Corp", "Name");
    bsl::string bb = bbAgg.asString();
    ASSERT("Bloomberg LP" == bb);
    ASSERT("1942-02-14"   == birthday);
//..
// The 'setValue' and 'setField' methods as well the 'asString', 'asInt',
// 'asDate', and similar methods automatically convert between the source and
// target types.  This is especially useful for converting between string and
// non-string types, although numeric and other conversions are also
// performed.  The behavior is undefined if an impossible conversion is
// attempted.
//..
    ASSERT(10022 == addr1["PostalCode"].asInt());      // Retrieve int
    ASSERT("10022" == addr1["PostalCode"].asString()); // Convert to string
    addr1["PostalCode"].setValue("10023");             // Convert from string
    ASSERT(10023 == addr1["PostalCode"].asInt());      // Retrieve int
    addr1["City"].setValue(101);                       // Convert from int
    ASSERT("101" == addr1["City"].asString());         // Retrieve string
    ASSERT(101 == addr1["City"].asInt());              // Convert to int
    addr1["City"].setValue("New York");                // Set string
//..
// Copying an aggregate using assignment or copy construction results in the
// copy referring to the same data as the original.  'areIdentical' returns
// true if and only if one aggregate references the same data as the other.
//..
    bcem_Aggregate addr2 = addr1;
    ASSERT(bcem_Aggregate::areIdentical(addr1, addr2));
//..
// To create a new aggregate with an independent copy of the data instead of a
// second reference to the same data, use the 'clone' function.  Just as two
// pointers that point to different but equal objects will not compare equal,
// the original 'bcem_Aggregate' and its clone are equivalent, but return
// false when compared with 'areIdentical'.  To compare the referenced data,
// use the 'areEquivalent' class methods:
//..
    addr2 = addr1.clone();
    ASSERT(!bcem_Aggregate::areIdentical(addr1, addr2));  // Not identical...
    ASSERT(bcem_Aggregate::areEquivalent(addr1, addr2));  // but equivalent
    ASSERT(&addr1.recordDef() != &addr2.recordDef()); // Different schema addr
//..
// The clone can be shown to be independent by changing a field, thus breaking
// the equivalence relationship.
//..
    addr2.setField("StreetAddress", "731 Lexington Ave.");  // After the move
    ASSERT(! bcem_Aggregate::areEquivalent(addr1, addr2));
    ASSERT("499 Park Ave."      == addr1.field("StreetAddress").asString());
    ASSERT("731 Lexington Ave." == addr2.field("StreetAddress").asString());
//..
// Changing the schema after creating an aggregate using that schema produces
// undefined behavior.  The schema can thus be considered constant and it is
// rarely necessary to clone the schema.  It is thus more efficient to clone
// the data only, using the 'cloneData' method:
//..
    addr2 = addr1.cloneData();
    ASSERT(!bcem_Aggregate::areIdentical(addr1, addr2));  // Not identical...
    ASSERT(bcem_Aggregate::areEquivalent(addr1, addr2));  // but equivalent
    ASSERT(&addr1.recordDef() == &addr2.recordDef());     // Exact same schema
//..
// It is important to note that the assignment operator always replaces both
// the schema and data of a 'bcem_Aggregate' so that both its structure and
// value may change.  It is thus not possible to modify a sub-aggregate via
// assignment:
//..
    addr1.setField("Entities", 1, "", "FirstName", "Mike");
    bcem_Aggregate mike = addr1.field("Entities", 1).selection();
    // 'mike' is a copy of 'addr1["Entities"][1]["Human"].
    // It's structure is defined by the 'person' record definition.
    ASSERT("Mike" == mike.field("FirstName").asString());

    // Assign 'mike' to refer to different data:
    mike = michael["FirstName"];
    // 'mike' is a copy of 'michael["FirstName"]' and contains a scalar string.
    ASSERT(mike.asString() == "Michael");

    // "FirstName" field within the (former) parent aggregate is unchanged.
    ASSERT("Mike" == addr1.field("Entities", 1, "", "FirstName").asString());
//..
// Similarly, you cannot modify a sub-aggregate through an rvalue expression:
//..
//  bcem_Aggregate x(bdem_ElemType::BDEM_STRING, "Michael");
//  addr1["Entities"][1][""]["FirstName"] = x;  // Won't work
//..
// In fact, methods that return sub-aggregates return them as 'const' to
// ensure that the previous example will not compile.  To modify a
// sub-aggregate, use 'setValue' (or one of the other manipulators):
//..
    mike = addr1.field("Entities", 1).selection();
    // 'mike' is another reference to 'addr1["Entities"][1]["Human"]
    ASSERT("Mike" == mike.field("FirstName").asString());
//..
// The following would be an error because 'michael["FirstName"]' does not
// have the same structure (dynamic type) as 'mike':
//..
    bcem_Aggregate result = mike.setValue(michael["FirstName"]);
    ASSERT(result.isError());
    ASSERT(result.errorCode() == ErrorCode::BCEM_NON_CONFORMANT);
    if (verbose) tst::cout << result.errorMessage();
//..
// Modify the data that 'mike' references using 'setValue' with data having
// compatible structure.
//..
    result = mike.setValue(michael);
    ASSERT(! result.isError());
    ASSERT(bcem_Aggregate::areEquivalent(mike, michael));

    // "FirstName" field within the parent aggregate has been changed through
    // the sub-aggregate:
    ASSERT("Michael"==addr1.field("Entities", 1, "", "FirstName").asString());
//..
// Error handling is designed so that a chain of operations will record the
// first failure in an error aggregate.  For example, if we attempt to access
// an table field using an invalid index, then go on to try to access a field
// within the table using an invalid field name, the array-index error is
// preserved:
//..
    result = addr1["Entities"][40]["uHman"]["FirstName"];
    ASSERT(result.isError());
    ASSERT(result.errorCode() == ErrorCode::BCEM_BAD_ARRAYINDEX);
    if (verbose) tst::cout << result.errorMessage();
//..
}

static void testCase30(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
    // --------------------------------------------------------------------
    // ACCESSOR 'capacityRaw'
    //
    // Concerns:
    //: 1 'capacityRaw' correctly forwards to the appropriate
    //:   lower level function, when the type underlying the aggregate is
    //:   an array type.
    //: 2 The method loads the correct value in its output parameter.
    //: 3 The method returns an error-aggregate if invoked on a
    //:   'bcem_Aggregate' whose type is not an array-type.
    //
    // Plan:
    //: 1 Instantiate one aggregate for each array type, and verify that
    //:   invoking 'capacityRaw' on the aggregate loads the same value
    //:   as the one returned by invoking the corresponding 'capacity' method
    //:   on the underlying array type.  [C-1,2]
    //: 2 Instantiate a 'bcem_Aggregate' with an underlying non-array
    //:   'BDE_ELEM' type and verify that an error-aggregate is returned.
    //:   [C-3]
    //
    // Testing the following functions:
    //   const bcem_Aggregate capacityRaw() const;
    // --------------------------------------------------------------------

    if (verbose) cout << "Testing with a table type aggregate" << endl;

    for (int i = 1; i <= 4096; i <<= 1)
    {
        bdem_Table table;
        bcem_Aggregate tableAggregate(bdem_ElemType::BDEM_TABLE,
                                      table);

        table.reserveRaw(i);
        tableAggregate.reserveRaw(i);

        size_t aggregateCapacity = 1492;  // arbitrary flag value
        size_t tableCapacity = table.capacityRaw();
        const bcem_Aggregate RESULT =
                             tableAggregate.capacityRaw(&aggregateCapacity);

        LOOP2_ASSERT(i,
                     tableCapacity,
                     i == tableCapacity);
        LOOP2_ASSERT(i,
                     aggregateCapacity,
                     i == aggregateCapacity);
        LOOP_ASSERT(i, bcem_Aggregate::areEquivalent(RESULT, tableAggregate));
    }

    if (verbose) cout << "Testing with a choice array type aggregate" << endl;

    for (int i = 1; i <= 4096; i <<= 1)
    {
        bdem_ChoiceArray choiceArray;
        bcem_Aggregate choiceArrayAggregate(bdem_ElemType::BDEM_CHOICE_ARRAY,
                                            choiceArray);

        choiceArray.reserveRaw(i);
        choiceArrayAggregate.reserveRaw(i);

        size_t aggregateCapacity = 1492;  // arbitrary flag value
        size_t choiceArrayCapacity = choiceArray.capacityRaw();
        const bcem_Aggregate RESULT =
                   choiceArrayAggregate.capacityRaw(&aggregateCapacity);

        LOOP2_ASSERT(i,
                     choiceArrayCapacity,
                     i == choiceArrayCapacity);
        LOOP2_ASSERT(i,
                     aggregateCapacity,
                     i == aggregateCapacity);
        LOOP_ASSERT(
                  i,
                  bcem_Aggregate::areEquivalent(RESULT, choiceArrayAggregate));
    }

    if (verbose) cout << "Testing with a scalar array type aggregate" << endl;

    for (int i = 1; i <= 4096; i <<= 1)
    {
        bsl::vector<int> scalarArray;
        bcem_Aggregate scalarArrayAggregate(bdem_ElemType::BDEM_INT_ARRAY,
                                            scalarArray);

        scalarArray.reserve(i);
        scalarArrayAggregate.reserveRaw(i);

        size_t aggregateCapacity = 1492;  // arbitrary flag value
        size_t scalarArrayCapacity = scalarArray.capacity();
        const bcem_Aggregate RESULT =
                       scalarArrayAggregate.capacityRaw(&aggregateCapacity);

        LOOP2_ASSERT(i,
                     scalarArrayCapacity,
                     i == scalarArrayCapacity);

        LOOP2_ASSERT(i,
                     aggregateCapacity,
                     1492 != aggregateCapacity);

        LOOP2_ASSERT(i,
                     aggregateCapacity,
                     i == aggregateCapacity);
        LOOP_ASSERT(
                  i,
                  bcem_Aggregate::areEquivalent(RESULT, scalarArrayAggregate));
    }

    if (verbose) cout << "Testing with a non-array type aggregate" << endl;
    {
        bdem_List list;
        bcem_Aggregate listAggregate(bdem_ElemType::BDEM_LIST, list);

        size_t capacity = 1492;  // arbitrary flag value
        const bcem_Aggregate RESULT = listAggregate.capacityRaw(&capacity);
        ASSERT(!bcem_Aggregate::areEquivalent(RESULT, listAggregate));
        ASSERT(RESULT.isError());
        ASSERT(1492 == capacity);
    }
}

static void testCase29(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
    // --------------------------------------------------------------------
    // MANIPULATOR 'reserveRaw'
    //
    // Concerns:
    //: 1 'reserveRaw' correctly forwards to the appropriate
    //:   lower level function, when the type underlying the aggregate is
    //:   an array type.
    //: 2 'reserveRaw' does not allocate memory and returns an
    //:   error aggregate when the type underlying the aggregate is not an
    //:   array type.
    //
    // Plan:
    //: 1 Instantiate one aggregate for each array type, and verify that
    //:   invoking 'reserveRaw' on the aggregate allocates the same
    //:   amount of memory as invoking the correspoding 'reserve' method on
    //:   the underlying array type.  [C-1]
    //: 2 Instantiate a 'bcem_Aggregate' with an underlying non-array
    //:   'BDE_ELEM' type and verify that an error-aggregate is returned.
    //:   [C-3]
    //
    // Testing the following functions:
    //     const bcem_Aggregate reserveRaw(size_t numItems) const;
    // --------------------------------------------------------------------

    if (verbose) cout << "Testing with a table-type aggregate" << endl;

    for (int i = 1; i <= 4096; i <<= 1)
    {
        bslma_TestAllocator ta("TableTestAllocator", false);
        bslma_TestAllocator aa("AggregateTestAllocator", false);

        bdem_Table table(&ta);
        bcem_Aggregate tableAggregate(bdem_ElemType::BDEM_TABLE,
                                      table,
                                      &aa);

        const size_t NUM_BYTES_TABLE     = ta.numBytesTotal();
        const size_t NUM_BYTES_AGGREGATE = aa.numBytesTotal();

        table.reserveRaw(i);
        const bcem_Aggregate RESULT = tableAggregate.reserveRaw(i);
        LOOP_ASSERT(i, bcem_Aggregate::areEquivalent(RESULT, tableAggregate));

        const size_t DELTA_TABLE = NUM_BYTES_TABLE - ta.numBytesTotal();
        const size_t DELTA_AGGREGATE =
                                      NUM_BYTES_AGGREGATE - aa.numBytesTotal();
        LOOP3_ASSERT(i,
                     DELTA_TABLE,
                     DELTA_AGGREGATE,
                     DELTA_TABLE == DELTA_AGGREGATE);
    }

    if (verbose) cout << "Testing with a choice-array-type aggregate" << endl;

    for (int i = 1; i <= 4096; i <<= 1)
    {
        bslma_TestAllocator ca("ChoiceArrayTestAllocator", false);
        bslma_TestAllocator aa("AggregateTestAllocator", false);

        bdem_ChoiceArray choiceArray(&ca);
        bcem_Aggregate choiceArrayAggregate(bdem_ElemType::BDEM_CHOICE_ARRAY,
                                            choiceArray,
                                            &aa);

        const size_t NUM_BYTES_CHOICEARRAY = ca.numBytesTotal();
        const size_t NUM_BYTES_AGGREGATE   = aa.numBytesTotal();

        choiceArray.reserveRaw(i);
        const bcem_Aggregate RESULT = choiceArrayAggregate.reserveRaw(i);
        LOOP_ASSERT(
                  i,
                  bcem_Aggregate::areEquivalent(RESULT, choiceArrayAggregate));

        const size_t DELTA_CHOICEARRAY =
                                    NUM_BYTES_CHOICEARRAY - ca.numBytesTotal();
        const size_t DELTA_AGGREGATE =
                                      NUM_BYTES_AGGREGATE - aa.numBytesTotal();
        LOOP3_ASSERT(i,
                     DELTA_CHOICEARRAY,
                     DELTA_AGGREGATE,
                     DELTA_CHOICEARRAY == DELTA_AGGREGATE);
    }

    if (verbose) cout << "Testing with a scalar-array-type aggregate" << endl;

    for (int i = 1; i <= 4096; i <<= 1)
    {
        bslma_TestAllocator sa("ScalarArrayTestAllocator", false);
        bslma_TestAllocator aa("AggregateTestAllocator", false);

        bsl::vector<int> scalarArray(&sa);
        bcem_Aggregate scalarArrayAggregate(bdem_ElemType::BDEM_INT_ARRAY,
                                            scalarArray,
                                            &aa);

        const size_t NUM_BYTES_SCALARARRAY = sa.numBytesTotal();
        const size_t NUM_BYTES_AGGREGATE = aa.numBytesTotal();

        scalarArray.reserve(i);
        const bcem_Aggregate RESULT = scalarArrayAggregate.reserveRaw(i);
        LOOP_ASSERT(
                  i,
                  bcem_Aggregate::areEquivalent(RESULT, scalarArrayAggregate));

        const size_t DELTA_SCALARARRAY =
                                    NUM_BYTES_SCALARARRAY - sa.numBytesTotal();
        const size_t DELTA_AGGREGATE =
                                      NUM_BYTES_AGGREGATE - aa.numBytesTotal();
        LOOP3_ASSERT(i,
                     DELTA_SCALARARRAY,
                     DELTA_AGGREGATE,
                     DELTA_SCALARARRAY == DELTA_AGGREGATE);
    }

    if (verbose) cout << "Testing with a non-array type aggregate" << endl;
    {
        bslma_TestAllocator la("ListArrayTestAllocator", false);
        bslma_TestAllocator aa("AggregateTestAllocator", false);

        bdem_List list(&la);
        bcem_Aggregate listAggregate(bdem_ElemType::BDEM_LIST, list, &aa);

        const size_t NUM_BYTES_SCALARARRAY = la.numBytesTotal();
        const size_t NUM_BYTES_AGGREGATE = aa.numBytesTotal();

        const bcem_Aggregate RESULT = listAggregate.reserveRaw(1);

        ASSERT(NUM_BYTES_SCALARARRAY == la.numBytesTotal());
        ASSERT(NUM_BYTES_AGGREGATE   == aa.numBytesTotal());

        ASSERT(!bcem_Aggregate::areEquivalent(RESULT, listAggregate));
        ASSERT(RESULT.isError());
    }
}

static void testCase28(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // TESTING EXTENDED FIELD LOOKUP
        //
        // Concerns:
        //   1 The 'hasField', 'field', 'setField', 'makeSelection',
        //     'selection', 'selector', 'selectorIndex', 'selectorId', and
        //     'numSelections' methods work transparently when accessing an
        //     unambiguously named child of an anonymous field within a 'LIST'
        //     or 'CHOICE' aggregate.
        //   2 'hasField' returns false and the other methods fail 'if the
        //     aggregate contains one or more unnamed fields but not one with
        //     the specified name.
        //   3 The aggregate returned by 'field' refers to the expected data
        //     (at the expected address) for the named child of an anonymous
        //     field (as determined by the address of the data item itself).
        //   4 If the record definition contains an unnamed field containing
        //     a nested unnamed field containing the named field, then
        //     both levels of nesting are traversed.
        //   5 Anonymous fields of non-aggregate type are ignored during
        //     field lookup.
        //   6 All of the above apply when there exist multiple unnamed fields
        //     at any level and when the field is in the first, middle, or last
        //     unnamed field, provided the lookup is unambiguous.
        //   7 All of the above apply when the unnamed field is a LIST, TABLE,
        //     CHOICE, or CHOICE_ARRAY.
        //
        // Plan:
        //   - Construct a set of test vectors where each vector consists of a
        //     schema script and the indexes where the named child of the
        //     unnamed field should be found.
        //   - Each schema will have a top-level record definition named "a"
        //     and the program will look for a field named "b" in "a".
        //   - For each test vector, construct a 'bdem_Schema' from the script
        //     and a bcem_Aggregate from record "a" in the schema.
        //   - If the aggregate is a choice, call 'makeSelection("b")'.
        //   - Insert data into the aggregate using primitive bdem calls.
        //   - Set the value of field "b" using 'setField("b", 5)'.
        //   - 'hasField', 'field', 'setField', 'makeSelection',
        //     'selection', 'selector', 'selectorIndex', 'selectorId', and
        //     'numSelections' all return the expected results.
        //
        // Testing the following functions with anonymous fields:
        //     const bcem_Aggregate makeSelection(const char *newSel) const;
        //     const char *selector() const;
        //     int selectorIndex() const;
        //     int selectorId() const;
        //     int numSelections() const;
        //     const bcem_Aggregate selection() const;
        //     bool hasField(const char *fieldName) const;
        //     template <typename VALTYPE>
        //     const bcem_Aggregate setField(NameOrIndex    fieldOrIdx,
        //                                   const VALTYPE& value) const;
        //     const bcem_Aggregate field(NameOrIndex fieldOrIdx) const;
        // --------------------------------------------------------------------

        if (verbose) tst::cout << "\nTESTING EXTENDED FIELD LOOKUP"
                               << "\n=============================\n"
                               << bsl::endl;

        const int MAX_DEPTH = 3;
        static const struct {
            int         d_line;
            const char *d_spec;

            const char  d_expectedIndexes[MAX_DEPTH + 1];
                // String of 0-3 ascii digits, one per nested index, used to
                // traverse the LIST structure.  The root LIST is always
                // defined by record "a" in the schema and the target field is
                // always named "b".  For example "21" means that record "a"
                // has an anonymous field at index 2 that is a record with a
                // field named "b" at index 1.  An empty string indicates that
                // there is no anonymous path to a field "b" from record "a".
        } DATA[] = {

            //line Spec                                     Indexes  Concerns
            //---- ---------------------------------------  -------  --------
            { L_, ":a",                                      ""    },
            { L_, ":aGc",                                    ""    },
            { L_, ":aCb",                                    "0"   },
            { L_, ":aGcCb",                                  "1"   },

            { L_, ":*Gc :a+*0&FU",                           ""    }, // 2
            { L_, ":*FaGcMd :a+*0&FU",                       ""    }, // 2
            { L_, ":*Gc :aFa+*0&FUMd",                       ""    }, // 2
            { L_, ":*GcXe :aFa+*0&FUMd",                     ""    }, // 2

            { L_, ":*Cb :a+*0&FU",                           "00"  }, // 3
            { L_, ":*Cb :a+*0&FUFaMd",                       "00"  }, // 3
            { L_, ":*Cb :aFa+*0&FUMd",                       "10"  }, // 3
            { L_, ":*Cb :aFaMd+*0&FU",                       "20"  }, // 3
            { L_, ":*FaCbMd :a+*0&FU",                       "01"  }, // 3
            { L_, ":*CbXe :aFa+*0&FUMd",                     "10"  }, // 3
            { L_, ":*XeCb :aFa+*0&FUMd",                     "11"  }, // 3

            { L_, ":*Cb :*+*0&FU :a+*1&FU",                  "000" }, // 4
            { L_, ":*FaCbMd :*+*0&FU :a+*1&FU",              "001" }, // 4
            { L_, ":*Cb :*+*0&FU :aFa+*1&FUMd",              "100" }, // 4
            { L_, ":*Cb :*Fa+*0&FUMd :a+*1&FU",              "010" }, // 4
            { L_, ":*Gc :*Fa+*0&FUCb :a+*1&FU",              "02"  }, // 4

            { L_, ":aCb&FUD*",                               "0"   }, // 5
            { L_, ":*Cb :aG*&FU+*0&FU",                      "10"  }, // 5
            { L_, ":*Cb :aG*&FU+*0&FUDb",                    "2"   }, // 5
            { L_, ":*Cb :aCb&FU+*0&FU",                      "0"   }, // 5

            { L_, ":*Cg :*?Gc :*?Ad :a+*0&FUBf%*1&FU%*2&FU", ""    }, // 2,6

            { L_, ":*Cb :*?Gc :*?Ad :a+*0&FUBf%*1&FU%*2&FU", "00"  }, // 3,6
            { L_, ":*Gc :*?Cb :*?Ad :a+*0&FUBf%*1&FU%*2&FU", "20"  }, // 3,6
            { L_, ":*Ad :*?Gc :*?Cb :a+*0&FUBf%*1&FU%*2&FU", "30"  }, // 3,6

            { L_, ":*Cb :*?Gc :*?Ad :*+*0&FUBf%*1&FU%*2&FU"
                  ":a+*3&FU",                                "000" }, // 4,6
            { L_, ":*Gc :*?Cb :*?Ad :*+*0&FUBf%*1&FU%*2&FU"
                  ":a+*3&FU",                                "020" }, // 4,6
            { L_, ":*Ad :*?Gc :*?Cb :*+*0&FUBf%*1&FU%*2&FU"
                  ":a+*3&FU",                                "030" }, // 4,6
            { L_, ":*Cg :*?Gc :*?Ad :*+*0&FUBf%*1&FU%*2&FU"
                  ":a+*3&FU",                                ""    }, // 4,6

            { L_, ":*?Gc :a%*0&FU",                          ""    }, // 2,7
            { L_, ":*?Cb :aFa%*0&FUMd",                      "10"  }, // 3,7
            { L_, ":*Cb :*+*0&FU :a+*1&FU",                  "000" }, // 4,7
            { L_, ":*?Cb :*%*0&FU :a+*1&FU",                 "000" }, // 4,7
            { L_, ":*Cb :*?+*0&FU :a%*1&FU",                 "000" }, // 4,7
            { L_, ":*?Cb :aG*&FU%*0&FU",                     "10"  }, // 5,7

        };
        const int DATA_SIZE = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < DATA_SIZE; ++i) {

            ///////////////// SET UP TESTS HERE /////////////////////////
            //
            // This is a rather elaborate setup procedure to compute
            // expected values for each tested function.

            const int         LINE          = DATA[i].d_line;
            const char *const SPEC          = DATA[i].d_spec;
            const char *const EXPECTED      = DATA[i].d_expectedIndexes;

            int expectedIndexes[MAX_DEPTH];
            int expectedDepth = 0;
            for (expectedDepth = 0; EXPECTED[expectedDepth]; ++expectedDepth) {
                expectedIndexes[expectedDepth] = EXPECTED[expectedDepth] - '0';
            }
            const int        EXPECTED_DEPTH   = expectedDepth;
            const int *const EXPECTED_INDEXES = expectedIndexes;

            Schema schema;
            ggSchema(&schema, SPEC);

            if (veryVerbose) { P_(LINE) P(schema) }

            const RecDef *recA = schema.lookupRecord("a");
            ASSERT(0 != recA);

            Obj theAgg(ConstRecDefShdPtr(recA, bcema_SharedPtrNilDeleter(),0));
            CERef topRef = theAgg.asElemRef();

            // Expected values if field does not exist and agg is not a choice.
            EType::Type expMkSelectionType = EType::BDEM_VOID;
            int         expMkSelectionErr  = BAD_FLDNM;
            int         expSelectorIndex   = NOT_CHOICE;
            int         expSelectorId      = BDEM_NULL_FIELD_ID;
            const char* expSelectorStr     = "";
            int         expNumSelections   = NOT_CHOICE;
            EType::Type expSelectionType   = EType::BDEM_VOID;
            int         expSelectionErr    = NOT_CHOICE;
            const void *expSelectionData   = 0;
            bool        expHasField        = false;
            EType::Type expFieldType       = EType::BDEM_VOID;
            int         expFieldErr        = BAD_FLDNM;

            if (EXPECTED_DEPTH > 0) {
                // Field exists.
                expMkSelectionErr = NOT_CHOICE;
                expHasField       = true;
                expFieldErr       = 0;
            }

            // Descend through the anonymous fields in the schema, looking for
            // any choice records or ambiguous references in the
            // expected path.  If found, set expected function values
            // accordingly.
            bool hasChoice   = false;
            bool isAmbiguous = false;
            const RecDef *constraint = recA;
            for (int depth = 0; constraint && depth < MAX_DEPTH; ++depth) {

                int index = (expHasField ? EXPECTED_INDEXES[depth] : -1);

                if (RecDef::BDEM_CHOICE_RECORD == constraint->recordType())
                {
                    hasChoice = true;
                    expSelectionErr  = 0;
                    expNumSelections = constraint->numFields();

                    if (expHasField) {
                        // Operations are expected to succeed.
                        expMkSelectionType = EType::BDEM_INT;
                        expMkSelectionErr  = 0;
                        expSelectorIndex   = index;
                        expSelectorId      = index;
                        expSelectorStr     =
                            constraint->fieldName(expSelectorIndex);
                        expSelectionType   =
                            constraint->field(expSelectorIndex).elemType();
                    }
                    else {
                        // Operations are expected to fail.
                        expMkSelectionType = EType::BDEM_VOID;
                        expMkSelectionErr  = BAD_FLDNM;
                        expSelectorIndex   = -1;
                        expSelectorId      = BDEM_NULL_FIELD_ID;
                        expSelectorStr     = "";
                        expSelectionType   = EType::BDEM_VOID;
                    }

                    break;
                }

                if (constraint->numAnonymousFields() > 1) {
                    isAmbiguous = true;
                }
                else if (constraint->numAnonymousFields() < 1) {
                    break;
                }

                if (index < 0) {
                    // Find anonymous field
                    for (index = 0; 0 != constraint->fieldName(index); ++index)
                        ;
                }

                // Descend to the next level
                constraint = constraint->field(index).recordConstraint();
            }

            if (isAmbiguous) {
                expSelectorIndex   = AMBIGUOUS;
                expSelectorId      = BDEM_NULL_FIELD_ID;
                expSelectorStr     = "";
                expNumSelections   = AMBIGUOUS;
                expSelectionType   = EType::BDEM_VOID;
                expSelectionErr    = AMBIGUOUS;
            }

            ///////////////// BEGIN TESTING HERE /////////////////////////
            //
            // Expected values have been computed.  Test each function and
            // compare against expected results.

            theAgg.makeValue();
            theAgg.fieldByIndex(0).makeValue();
            Obj result = theAgg.makeSelection("b");
            result.makeValue();
            ASSERT_AGG_ERROR(result, expMkSelectionErr);
            LOOP2_ASSERT(expMkSelectionType, result.dataType(),
                         expMkSelectionType == result.dataType());

            if (0 == expMkSelectionErr && result.isError()) {
                // Prevent cascade errors if makeSelection fails unexpectedly.
                continue;
            }

            bsls_ObjectBuffer<CERef> refs[MAX_DEPTH];
            CERef *expRef = &topRef;
            for (int depth = 0; depth < EXPECTED_DEPTH; ++depth) {
                // Using 'bdem' primitives (not 'bcem_Aggregate' methods),
                // find the field referenced by "b", using the array of
                // expected indexes to descend down the data structure.
                int index = EXPECTED_INDEXES[depth];

                void *refp = &refs[depth];
                if (ET::BDEM_CHOICE == expRef->type()) {
                    LOOP_ASSERT(expRef->theChoice().selector(),
                                index == expRef->theChoice().selector());
                    expRef = new(refp) CERef(expRef->theChoice().selection());
                    if (0 == expSelectionData) {
                        // Keep track of first choice
                        expSelectionData = expRef->data();
                    }
                }
                else { // if LIST
                    expRef = new(refp) CERef(expRef->theList()[index]);
                }

                expFieldType = expRef->type();
            }

            if (hasChoice) {
                // Test result of 'makeSelection'
                ASSERT(expMkSelectionErr ||
                       expRef->data() == result.asElemRef().data());
            }

            // selectorIndex(), selectorId(), selector(), numSelections,
            // selection():
            int selectorIndex = theAgg.selectorIndex();
            LOOP3_ASSERT(LINE, expSelectorIndex, selectorIndex,
                         expSelectorIndex == selectorIndex);
            if (veryVerbose && expSelectorIndex != selectorIndex) {
                tst::cout << "theAgg=" << theAgg << bsl::endl;
                if (theAgg.schemaPtr()) {
                    tst::cout << "schema=" << *theAgg.schemaPtr() 
                              << bsl::endl;
                }
            }

            int selectorId = theAgg.selectorId();
            LOOP2_ASSERT(expSelectorId, selectorId,
                         expSelectorId == selectorId);

            const char* selectorStr = theAgg.selector();
            LOOP2_ASSERT(expSelectorStr, selectorStr,
                         streq(expSelectorStr, selectorStr));

            int numSelections = theAgg.numSelections();
            LOOP2_ASSERT(expNumSelections, numSelections,
                         expNumSelections == numSelections);

            result = theAgg.selection();
            LOOP2_ASSERT(expSelectionType, result.dataType(),
                         expSelectionType == result.dataType());
            ASSERT_AGG_ERROR(result, expSelectionErr);
            ASSERT(expSelectionErr ||
                   expSelectionData == result.data());

            // Test hasField(), setField(), field()
            LOOP2_ASSERT(expHasField, theAgg.hasField("b"),
                         expHasField == theAgg.hasField("b"));

            result = theAgg.setField("b", 5);
            LOOP2_ASSERT(expFieldType, result.dataType(),
                         expFieldType == result.dataType());
            ASSERT_AGG_ERROR(result, expFieldErr);
            ASSERT(result.isError() ||
                   expRef->data() == result.asElemRef().data());

            result = theAgg.field("b");
            LOOP2_ASSERT(expFieldType, result.dataType(),
                         expFieldType == result.dataType());
            ASSERT_AGG_ERROR(result, expFieldErr);
            ASSERT(result.isError() ||
                   expRef->data() == result.asElemRef().data());
        }
}

static void testCase27(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // TESTING anonymousField
        //
        // Concerns:
        //   - Calling 'anonymousField(n)' on an aggregate that has anonymous
        //     fields will return the nth anonymous field.
        //   - If the aggregate is a choice and the nth anonymous field is not
        //     selected, then return the usual error of accessing an
        //     unselected member of a choice.
        //   - The appropriate error is returned for calling 'anonymousField'
        //     on a scalar aggregate.
        //   - The appropriate error is returned for calling 'anonymousField'
        //     on an aggregate with no anonymous fields.
        //   - The appropriate error is returned for calling 'anonymousField'
        //     on an aggregate with anonymous fields, but no nth anonymous
        //     field.
        //   - Calling 'anonymousField' with no arguments returns the same as
        //     'anonymousField(0)' unless the number of anonymous fields is
        //     not one.
        //   - Calling 'anonymousField' with no arguments returns an ambiguous
        //     anonymous field error if the number of anonymous fields is not
        //     one.
        //
        // Plan:
        //   - Create an aggregate containing lists and choices with zero,
        //     one, and two anonymous fields.
        //   - Call 'anonymousField' for each anonymous field and verify that
        //     it is identical to accessing that field by index.
        //   - Call 'anonymousField' for each error condition listed in the
        //     concerns and verify the expected error code and error message.
        //   - Verify that the result of 'anonymousField' is the same with an
        //     argument of zero and with no arguments when the number of
        //     anonymous fields is one.
        //   - Verify the expected error for calling 'anonymousField' with no
        //     arguments when the number of anonymous fields is one.
        //
        // Testing:
        //     const bcem_Aggregate anonymousField(int n) const;
        //     const bcem_Aggregate anonymousField() const;
        // --------------------------------------------------------------------

        if (verbose) tst::cout << "\nTESTING anonymousField"
                               << "\n======================" << bsl::endl;

        if (verbose) tst::cout << "Testing valid calls" << bsl::endl;
        {
            static const struct {
                int         d_line;
                const char *d_spec;
                int         d_anonIndex;
                int         d_fieldIndex;
            } DATA[] = {
                //                                              Anon    Field
                // Ln  Schema spec                              index   index
                // -- ---------------------------------------   -----   -----
                { L_, ":rA*BbCc",                               0,      0 },
                { L_, ":rAaB*Cc",                               0,      1 },
                { L_, ":rAaBbC*",                               0,      2 },
                { L_, ":rA*B*Cc",                               0,      0 },
                { L_, ":rA*BbC*",                               0,      0 },
                { L_, ":rAaB*C*",                               0,      1 },
                { L_, ":rA*B*C*",                               0,      0 },

                { L_, ":rA*B*Cc",                               1,      1 },
                { L_, ":rA*BbC*",                               1,      2 },
                { L_, ":rAaB*C*",                               1,      2 },
                { L_, ":rA*B*C*",                               1,      1 },

                { L_, ":rA*B*C*",                               2,      2 },

                { L_, ":r?A*BbCc",                              0,      0 },
                { L_, ":r?AaB*Cc",                              0,      1 },
                { L_, ":r?AaBbC*",                              0,      2 },
                { L_, ":r?A*B*Cc",                              0,      0 },
                { L_, ":r?A*BbC*",                              0,      0 },
                { L_, ":r?AaB*C*",                              0,      1 },
                { L_, ":r?A*B*C*",                              0,      0 },

                { L_, ":r?A*B*Cc",                              1,      1 },
                { L_, ":r?A*BbC*",                              1,      2 },
                { L_, ":r?AaB*C*",                              1,      2 },
                { L_, ":r?A*B*C*",                              1,      1 },

                { L_, ":r?A*B*C*",                              2,      2 },

                { L_, ":i?DdEe :rAa%*iCc",                      0,      1 },
                { L_, ":i?DdEe :rAa@*iCc",                      0,      1 },
                { L_, ":i?DdEe :j?FfGg :r%*i@*jCc",             1,      1 },
                { L_, ":i?DdEe :j?FfGg :kHh :r@*i%*j+*k",       2,      2 },
                { L_, ":iDdEe :r?AaBb#*i",                      0,      2 },
                { L_, ":i?DdEe :j?FfGg :r?%*iBb%*j",            1,      2 },
                { L_, ":i?DdEe :jFfGg :kHh :r?@*i+*j#*k",       2,      2 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE        = DATA[i].d_line;
                const char *const SPEC        = DATA[i].d_spec;
                const int         ANON_INDEX  = DATA[i].d_anonIndex;
                const int         FIELD_INDEX = DATA[i].d_fieldIndex;

                bcema_SharedPtr<bdem_Schema> schema;
                schema.createInplace();
                ggSchema(schema.ptr(), SPEC);

                Obj agg(schema, "r");
                if (EType::BDEM_CHOICE == agg.dataType()) {
                    agg.makeSelectionByIndex(FIELD_INDEX);
                }
                LOOP_ASSERT(agg, ! agg.isError());

                if (veryVeryVerbose) { P(agg); }

                bcem_Aggregate expected = agg.fieldByIndex(FIELD_INDEX);
                bcem_Aggregate result = agg.anonymousField(ANON_INDEX);
                LOOP2_ASSERT(expected, result,
                             Obj::areIdentical(expected, result));

                if (veryVeryVerbose) { P(expected); P(result); }

                if (schema->lookupRecord("r")->numAnonymousFields() == 1) {
                    bcem_Aggregate result2 = agg.anonymousField();
                    LOOP2_ASSERT(expected, result2,
                                 Obj::areIdentical(expected, result2));
                }
            }
        }

        if (verbose) tst::cout << "Testing error conditions" << bsl::endl;

        {
            static const struct {
                int         d_line;
                const char *d_spec;
                int         d_selectorIndex;
                int         d_anonIndex;
                int         d_errorCode;
                const char *d_errorMessage;
            } DATA[] = {
                //      Schema     sel anon
                // Ln    Spec      idx idx error code  error message
                // -- -----------  --- --- ----------  ---------------------
                { L_, ":rAaBbCc",   0,  0, BAD_FLDIDX, "no anonymous fields" },
                { L_, ":rAaBbCc",   0,  1, BAD_FLDIDX, "no anonymous fields" },
                { L_, ":rA*BbCc",   0,  1, BAD_FLDIDX, "Invalid index"       },
                { L_, ":rAaB*Cc",   0,  1, BAD_FLDIDX, "Invalid index"       },
                { L_, ":rAaBbC*",   0,  1, BAD_FLDIDX, "Invalid index"       },
                { L_, ":rA*B*Cc",   0,  2, BAD_FLDIDX, "Invalid index"       },
                { L_, ":rA*BbC*",   0,  3, BAD_FLDIDX, "Invalid index"       },
                { L_, ":rAaB*C*",   0, -1, BAD_FLDIDX, "Invalid index"       },
                { L_, ":rA*B*C*",   0,  4, BAD_FLDIDX, "Invalid index"       },

                { L_, ":r?AaBbCc",  0,  0, BAD_FLDIDX, "no anonymous fields" },
                { L_, ":r?AaBbCc",  0,  1, BAD_FLDIDX, "no anonymous fields" },
                { L_, ":r?A*BbCc",  0,  1, BAD_FLDIDX, "Invalid index"       },
                { L_, ":r?AaB*Cc",  1,  1, BAD_FLDIDX, "Invalid index"       },
                { L_, ":r?AaBbC*",  2,  1, BAD_FLDIDX, "Invalid index"       },
                { L_, ":r?A*B*Cc",  1,  2, BAD_FLDIDX, "Invalid index"       },
                { L_, ":r?A*BbC*",  2,  3, BAD_FLDIDX, "Invalid index"       },
                { L_, ":r?AaB*C*",  2, -1, BAD_FLDIDX, "Invalid index"       },
                { L_, ":r?A*B*C*",  2,  4, BAD_FLDIDX, "Invalid index"       },

                { L_, ":r?A*BbCc",  1,  0, NOT_SELECT, "currently selected"  },
                { L_, ":r?AaB*Cc",  0,  0, NOT_SELECT, "currently selected"  },
                { L_, ":r?AaBbC*",  1,  0, NOT_SELECT, "currently selected"  },
                { L_, ":r?A*B*Cc",  0,  1, NOT_SELECT, "currently selected"  },
                { L_, ":r?A*BbC*",  2,  0, NOT_SELECT, "currently selected"  },
                { L_, ":r?AaB*C*",  1,  1, NOT_SELECT, "currently selected"  },
                { L_, ":r?A*B*C*",  0,  2, NOT_SELECT, "currently selected"  },

                { L_, ":r?A*BbCc", -1,  0, NOT_SELECT, "currently selected"  },
                { L_, ":r?A*B*Cc", -1,  1, NOT_SELECT, "currently selected"  },
                { L_, ":r?A*B*C*", -1,  2, NOT_SELECT, "currently selected"  },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE           = DATA[i].d_line;
                const char *const SPEC           = DATA[i].d_spec;
                const int         SELECTOR_INDEX = DATA[i].d_selectorIndex;
                const int         ANON_INDEX     = DATA[i].d_anonIndex;
                const int         ERROR_CODE     = DATA[i].d_errorCode;
                const char *const ERROR_MESSAGE  = DATA[i].d_errorMessage;

                bcema_SharedPtr<bdem_Schema> schema;
                schema.createInplace();
                ggSchema(schema.ptr(), SPEC);

                Obj agg(schema, "r");
                if (EType::BDEM_CHOICE == agg.dataType()) {
                    agg.makeSelectionByIndex(SELECTOR_INDEX);
                }
                LOOP_ASSERT(agg, ! agg.isError());

                if (veryVeryVerbose) { P(agg); }

                bcem_Aggregate result = agg.anonymousField(ANON_INDEX);
                LOOP_ASSERT(result, result.isError());
                ASSERT_AGG_ERROR(result, ERROR_CODE);
                LOOP_ASSERT(result.errorMessage(),
                            bsl::string::npos !=
                            result.errorMessage().find(ERROR_MESSAGE));

                if (veryVeryVerbose) { P(result); }

                int numAnon = schema->lookupRecord("r")->numAnonymousFields();
                bcem_Aggregate result2 = agg.anonymousField();
                if (0 == numAnon) {
                    LOOP_ASSERT(result2, result2.isError());
                    ASSERT_AGG_ERROR(result2, BAD_FLDIDX);
                    LOOP_ASSERT(result2.errorMessage(),
                                bsl::string::npos !=
                                result2.errorMessage().find("no anonymous "
                                                            "fields"));
                } else if (numAnon > 1) {
                    LOOP_ASSERT(result2, result2.isError());
                    ASSERT_AGG_ERROR(result2, AMBIGUOUS);
                    LOOP_ASSERT(result2.errorMessage(),
                                bsl::string::npos !=
                                result2.errorMessage().find("multiple "
                                                            "anonymous"));
                } else if (0 == ANON_INDEX) {
                    ASSERT_AGG_ERROR(result2, result.errorCode());
                    LOOP2_ASSERT(result, result2, result.errorMessage() ==
                                 result2.errorMessage());
                }
            }
        }

        {
            // Check error condition for non-aggregate.
            Obj agg(EType::BDEM_INT, 5);
            bcem_Aggregate result = agg.anonymousField();
            LOOP_ASSERT(result, result.isError());
            ASSERT_AGG_ERROR(result, NON_RECORD);
            LOOP_ASSERT(result.errorMessage(),
                        bsl::string::npos !=
                        result.errorMessage().find("unconstrained INT"));

            if (veryVeryVerbose) { P(result); }
        }
}

static void testCase26(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // TESTING BDEX STREAMING
        //
        // Concerns:
        //   - For each supported data type, data can be streamed out to a
        //     bdex stream, then streamed back into a non-aggregate object of
        //     the corresponding data type.
        //   - For each supported data type, data can be streamed out of an
        //     object of that type, then streamed back into a bcem_aggregate
        //     holding that type.
        //   - Streaming in a list, table, choice, or choice-array that does
        //     not conform to the schema causes an error and sets the
        //     aggregate to null.
        //   - Since the streaming operations are pass-through's to the
        //     underlying data types' streaming operations, it is not
        //     necessary to test corrupt schemas, etc. once it is determined
        //     that the calls are properly passed through.
        //
        // Plan:
        //   - For each supported data type, stream an object of that type
        //     to a memory stream.
        //   - Read from the memory stream into a properly initialized
        //     bcem_Aggregate and compare with the original.
        //   - Stream the bcem_Aggregate back out to another memory stream.
        //   - Compare the original and second memory stream.
        //   - Test that data that does not conform to the schema will put the
        //     stream in the invalid state set the bcem_Aggregate to null.
        //
        // Testing:
        //     int maxSupportedBdexVersion() const;
        //     STREAM& bdexStreamIn(STREAM& stream, int version);
        //     STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // --------------------------------------------------------------------

        if (verbose) tst::cout << "\nBDEX STREAMING"
                               << "\n==============" << bsl::endl;

        const int MAX_VERSION = 3;
        {
            LOOP_ASSERT(Obj::maxSupportedBdexVersion(),
                        MAX_VERSION == Obj::maxSupportedBdexVersion());
        }

        {
            const struct {
                int         d_line;
                const char *d_spec;
            } DATA[] = {
                // Line     Spec
                // ----     ----
                {   L_,   ":aCa"    },
                {   L_,   ":aCa&NT" },
                {   L_,   ":aCa&NF" },
                {   L_,   ":aCa&D0" },
                {   L_,   ":aGa"    },
                {   L_,   ":aGa&NT" },
                {   L_,   ":aGa&NF" },
                {   L_,   ":aGa&D0" },
                {   L_,   ":aHa"    },
                {   L_,   ":aHa&NT" },
                {   L_,   ":aHa&NF" },
                {   L_,   ":aHa&D0" },
                {   L_,   ":aNa"    },
                {   L_,   ":aNa&NT" },
                {   L_,   ":aNa&NF" },
                {   L_,   ":aNa&D0" },
                {   L_,   ":aNa&FN" },
                {   L_,   ":aPa"    },
                {   L_,   ":aPa&NT" },
                {   L_,   ":aPa&NF" },
                {   L_,   ":aPa&D0" },
                {   L_,   ":aQa"    },
                {   L_,   ":aQa&NT" },
                {   L_,   ":aQa&NF" },
                {   L_,   ":aQa&D0" },
                {   L_,   ":aRa"    },
                {   L_,   ":aRa&NT" },
                {   L_,   ":aRa&NF" },
                {   L_,   ":aRa&D0" },
// TBD failed on little-endian only
//                {   L_,   ":aWa"    },
                {   L_,   ":aWa&NT" },
// TBD
//                 {   L_,   ":aWa&NF" },
//                 {   L_,   ":aWa&D0" },
                {   L_,   ":aaa"    },
                {   L_,   ":aaa&NT" },
                {   L_,   ":aaa&NF" },
                {   L_,   ":aaa&D0" },
                {   L_,   ":aYa"    },
                {   L_,   ":aYa&NT" },
                {   L_,   ":aYa&NF" },
                {   L_,   ":aYa&D0" },
                {   L_,   ":ada"    },
                {   L_,   ":ada&NT" },
                {   L_,   ":ada&NF" },
                {   L_,   ":ada&D0" },

// TBD
//                 {   L_,   ":aCbFcGdQf :g+ha"                },
//                 {   L_,   ":aCbFcGdQf :g+ha&NT"             },
// TBD
//                 {   L_,   ":aCbFcGdQf :g+ha&NF"             },
                {   L_,   ":aCb&NTFcGd&NTQf :g+ha"          },
                {   L_,   ":aCb&NFFc&NTGd&NFQf&NT :g+ha"    },

                {   L_,   ":aCbFcGdQf :g#ha"    },
                {   L_,   ":aCbFcGdQf :g#ha&NT" },
                {   L_,   ":aCbFcGdQf :g#ha&NF" },
                {   L_,   ":aCb&NTFcGd&NTQf :g#ha"          },
                {   L_,   ":aCb&NFFc&NTGd&NFQf&NT :g#ha"    },

                {   L_,   ":a?CbFcGdQf :g%ha"    },
                {   L_,   ":a?CbFcGdQf :g%ha&NT" },
                {   L_,   ":a?CbFcGdQf :g%ha&NF" },

                {   L_,   ":a?CbFcGdQf :g@ha"    },
                {   L_,   ":a?CbFcGdQf :g@ha&NT" },
                {   L_,   ":a?CbFcGdQf :g@ha&NF" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const char *SPEC = DATA[i].d_spec;
                const bool  NSA  = (bool) bsl::strstr(SPEC, "&FN");

                Schema schema; const Schema& SCHEMA = schema;
                ggSchema(&schema, SPEC);
                const RecDef *RECORD = NSA
                                     ? &SCHEMA.record(0)
                                     : &SCHEMA.record(SCHEMA.numRecords() - 1);

                ConstRecDefShdPtr crp(RECORD, NilDeleter(), 0);
                const ConstRecDefShdPtr& CRP = crp;

                ET::Type TYPE = NSA
                        ? ET::toArrayType(SCHEMA.record(1).field(0).elemType())
                        : RECORD->field(0).elemType();
                const char *fldName = RECORD->fieldName(0);

                const CERef A1 = getCERef(TYPE, 1);
                const CERef A2 = getCERef(TYPE, 2);

                int version = bsl::strstr(SPEC, "NT") ? 3 : 1;
                for (; version <= MAX_VERSION; ++version) {
                    bslma_TestAllocator t(veryVeryVerbose);
                    Obj mU(CRP, &t); const Obj& U = mU;
                    Obj mV(CRP, &t); const Obj& V = mV;
                    Obj mX(CRP, &t); const Obj& X = mX;
                    Obj mY(CRP, &t); const Obj& Y = mY;

                    initAggregate(&mU, *RECORD);
                    initAggregate(&mX, *RECORD);

                    if (veryVerbose) {
                        P(U);
                    }

                    bdex_TestOutStream out1, out2;
                    U.bdexStreamOut(out1, version);
                    bdex_OutStreamFunctions::streamOut(out2, X, version);

                    LOOP_ASSERT(LINE, out1);
                    LOOP_ASSERT(LINE, out2);

                    bdex_TestInStream in1(out1.data(), out1.length());

                    LOOP_ASSERT(LINE, in1);

                    in1.setSuppressVersionCheck(1);

                    V.bdexStreamIn(in1, version);

                    if (veryVerbose) {
                        P(V);
                    }

                    if (version > 1) {
                        LOOP4_ASSERT(LINE, version, U, V,
                                     Obj::areEquivalent(U, V));
                    }

                    bdex_TestInStream testInStream(out2.data(), out2.length());
                    testInStream.setSuppressVersionCheck(1);
                    LOOP_ASSERT(LINE, testInStream);

                   BEGIN_BDEX_EXCEPTION_TEST {
                     testInStream.reset();

                     bdex_InStreamFunctions::streamIn(testInStream,
                                                      Y,
                                                      version);

                     // TBD: Fix
                     if (version > 1) {
                         LOOP4_ASSERT(LINE, version, X, Y,
                                      Obj::areEquivalent(X, Y));
                     }
                   } END_BDEX_EXCEPTION_TEST
                }
            }
        }
}

static void testCase25(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // TESTING ENUMERATIONS
        //
        // Concerns:
        //   - Passing a valid integral enumerator to 'setValue' or 'setField'
        //     for an 'INT' enumeration field sets the field to the integral
        //     value.
        //   - Passing a valid string enumerator to 'setValue' or 'setField'
        //     for a 'STRING' enumeration field sets the field to the string
        //     value.
        //   - Passing a valid integral enumerator to 'setValue' or 'setField'
        //     for a 'STRING' enumeration field sets the field to the
        //     corresponding string enumerator name.
        //   - Passing a valid string enumerator to 'setValue' or 'setField'
        //     for an 'INT' enumeration field sets the field to the
        //     corresponding int enumerator ID.
        //   - Passing a null integral or string value to 'setValue' or
        //     'setField' for either an 'INT' or 'STRING' enumeration field
        //     will set the field to a null value.
        //   - Setting an enumerated field to a floating-point value is the
        //     same as setting it to the corresponding (truncated) integral
        //     value.  No compiler warnings are produced.  Null floating-point
        //     values are translated into null integral values.
        //   - Setting an enumerated field to value that is convertible to
        //     an integer or string will set the field accordingly after
        //     conversion.
        //   - Passing an invalid numeric or string enumerator to 'setValue' or
        //     'setField' for an enumeration field does not modify the field
        //     and returns a 'BAD_ENUMVALUE' error.
        //   - Passing a type that is not convertible to 'int' or 'string' to
        //     'setValue' or 'setField' for an enumeration field does not
        //     modify the field and returns a 'BAD_CONVERSION' error.
        //   - Retrieving a valid enumeration field using 'asInt', 'asShort',
        //     or other integral accessor returns the integral enumerator id
        //     for the field value, or the null value if the field is null.n
        //   - Retrieving a valid enumeration field using 'asString' returns
        //     the string enumerator name for the field value, or the null
        //     (empty) string if the field is null.
        //   - Retrieving a valid enumeration field using a non-string,
        //     non-numeric 'as' method returns a best-possible conversion or,
        //     if no conversion is possible, the null value.
        //   - Retrieving an enumeration field that was corrupted using
        //     low-level operations such that it does not hold a valid
        //     enumerator will return a best-possible conversion or, if no
        //     conversion is possible, the null value.
        //   - A single item in an array of enumerations behaves like a an
        //     enumerated scalar field.
        //
        // Plan:
        //   Use a combination of loops and test vectors to test the
        //   cross-product of the following variables:
        //   - Field type ('INT', 'STRING', 'INT_ARRAY', 'STRING_ARRAY'
        //   - Value to set (valid, invalid, null)
        //   - Type of value (int, short, double, convertibleToInt, string,
        //     char*, const char*, char[], const char[], convertibleToString,
        //     bdet_Date).
        //   - Retrieval function (asString, asInt, asDouble, asDate).
        //   Also test setting an invalid value through 'asElemRef' and verify
        //   best-possible conversion on retrieval.
        //
        // Testing:
        //     const bcem_Aggregate setValue(const VALTYPE& value) const;
        //     const bcem_Aggregate setField(NameOrIndex    fieldOrIdx1,
        //                                   const VALTYPE& value) const;
        //     int asInt() const;
        //     bsl::string asString() const;
        // --------------------------------------------------------------------

        if (verbose) tst::cout << "\nTESTING ENUMERATIONS"
                               << "\n====================" << bsl::endl;

        // NI is a compile-time constant.  Assert that it is the same as
        // bdetu_Unset<int>::unsetValue().
        const int NI = INT_MIN;
        ASSERT(NI == bdetu_Unset<int>::unsetValue());

        // Construct a schema like this:
        //
        // ENUMERATION "f" {
        //    "a" => 0;
        //    "you" => 1;
        //    "vee" => 2;
        // }
        // RECORD "r" {
        //     INT          ENUM<"f"> "i";
        //     STRING       ENUM<"f"> "s";
        //     INT_ARRAY    ENUM<"f"> "j";
        //     STRING_ARRAY ENUM<"f"> "t";
        // }
        bdem_Schema theSchema;
        ggSchema(&theSchema,
                 ":f=auv  :r $if ^sf !jf /tf !bf&FN /cf&FN");
        SchemaShdPtr theSchemaPtr(&theSchema, NilDeleter(), 0);

        const Obj AGG(theSchemaPtr, "r");
        AGG["j"].resize(1);
        AGG["t"].resize(1);
        AGG["b"].resize(1);
        AGG["c"].resize(1);

        if (veryVerbose) { P(theSchema) P(AGG) };

        static const struct {
            int  d_line;
            int  d_intValue;
            char d_strValue;
            int  d_errorCode;
        } DATA[] = {
            //         int    string  error
            // Line   value    value   code
            // ====   =====    =====  =====
            { L_,       NI,     'e',    0 },
            { L_,        0,     'a',    0 },
            { L_,        1,     'u',    0 },
            { L_,        2,     'v',    0 },
            { L_,        3,     'w',    ErrorCode::BCEM_BAD_ENUMVALUE }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE            = DATA[i].d_line;
            const int INT_VAL         = DATA[i].d_intValue;
            const char *const STR_VAL = getName(DATA[i].d_strValue);
            const int ERROR_CODE      = DATA[i].d_errorCode;

            short  SHORT_VAL  = (INT_VAL == NI ?
                                 bdetu_Unset<short>::unsetValue() : INT_VAL);
            double DOUBLE_VAL = (INT_VAL == NI ?
                               bdetu_Unset<double>::unsetValue() : INT_VAL+.1);
            const bsl::string STRING_VAL = STR_VAL;
            const char *const CONSTCHARSTAR_VAL = STR_VAL;
            char CHARARRAY_VAL[100];
            bsl::strcpy(CHARARRAY_VAL, STR_VAL);
            char *const CHARSTAR_VAL = CHARARRAY_VAL;
            const char (&CONSTCHARARRAY_VAL)[100] = CHARARRAY_VAL;

#           define SIMPLE_ENUM_TEST(value) \
               enumTest(AGG, "isjtbc", value, INT_VAL, \
                        STR_VAL, ERROR_CODE, LINE)

            SIMPLE_ENUM_TEST(INT_VAL);
            SIMPLE_ENUM_TEST(SHORT_VAL);
            SIMPLE_ENUM_TEST(DOUBLE_VAL);
            SIMPLE_ENUM_TEST(STRING_VAL);
            SIMPLE_ENUM_TEST(CHARSTAR_VAL);
            SIMPLE_ENUM_TEST(CONSTCHARSTAR_VAL);
            SIMPLE_ENUM_TEST(CHARARRAY_VAL);
            SIMPLE_ENUM_TEST(CONSTCHARARRAY_VAL);

#           undef SIMPLE_ENUM_TEST
        }

        if (verbose) tst::cout << "Testing bad conversions" << bsl::endl;
        const bdet_Date DATE_VAL(2006, 9, 20);
        enumTest(AGG, "isjtbc", DATE_VAL, 0,
                 "", ErrorCode::BCEM_BAD_CONVERSION);

        // Conversion to date fails
        bdet_Date dateRet = AGG["i"].asDate();
        ASSERT(bdetu_Unset<bdet_Date>::isUnset(dateRet));
        dateRet = AGG["s"].asDate();
        ASSERT(bdetu_Unset<bdet_Date>::isUnset(dateRet));

        if (verbose) tst::cout << "Testing corrupt values" << bsl::endl;

        bcem_Aggregate mAgg2 = AGG.cloneData();
        mAgg2.fieldRef("i").theModifiableInt() = 98;
                                                // Invalid enumerator ID
        ASSERT(98 == mAgg2["i"].asInt());       // exact return
        ASSERT("98" == mAgg2["i"].asString());  // best conversion
        dateRet = mAgg2["i"].asDate();
        ASSERT(bdetu_Unset<bdet_Date>::isUnset(dateRet)); // Invalid conversion

        mAgg2.fieldRef("s").theModifiableString() = "2006-09-20";
                                                        // Invalid enumerator
        ASSERT(2006 == mAgg2["s"].asInt());             // best conversion
        ASSERT("2006-09-20" == mAgg2["s"].asString());  // exact return
        dateRet = mAgg2["s"].asDate();
        ASSERT(DATE_VAL == dateRet);                    // best conversion
}

static void testCase24(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // TESTING 'bdeat' FUNCTIONS
        //
        // Concerns:
        //   - 'bdeat_TypeName::className(bcem_Aggregate)' returns the record
        //     name for aggregates that are constrained by a record
        //     definition, and a zero pointer for aggregates that are not
        //     constrained by a record definition.
        //   - 'bdeat_TypeCategory::Select<bcem_Aggregate>::BDEAT_SELECTION' is
        //     '0'
        //   - 'bdeat_TypeCategoryFunctions::select(bdem_Aggregate)' returns
        //     the category value appropriate to the value of the aggregate.
        //   - The following methods in 'bdeat_SequenceFunctions' produce the
        //     expected results on an aggregate holding a list or row:
        //     - 'manipulateAttribute'
        //     - 'manipulateAttributes'
        //     - 'accessAttribute'
        //     - 'accessAttributes'
        //     - 'hasAttribute'
        //   - The following methods in 'bdeat_ChoiceFunctions' produce the
        //     expected result on an aggregate holding a choice or choice array
        //     item:
        //     - 'makeSelection'
        //     - 'manipulateSelection'
        //     - 'accessSelection'
        //     - 'hasSelection'
        //     - 'selectionId'
        //   - The following methods in 'bdeat_ArrayFunctions' produce the
        //     expected result on an aggregate holding an array, table, or
        //     choice array:
        //     - 'size'
        //     - 'resize'
        //     - 'manipulateElement'
        //     - 'accessElement'
        //   - The following methods in 'bdeat_NullableValueFunctions'
        //     return the expected results when invoked on a nullable field
        //     within an aggregate:
        //     - 'makeValue'
        //     - 'manipulateValue'
        //     - 'accessValue'
        //     - 'isNull'
        //   - The following methods in 'bdeat_EnumFunctions' set and get the
        //     correct values for an enumeration field within an aggregate:
        //     - 'fromInt'
        //     - 'fromString'
        //     - 'toInt'
        //     - 'toString'
        //   - The following methods in 'bdeat_ValueTypeFunctions' assign the
        //     correct values for a scalar aggregate and for a list aggregate:
        //     - 'assign'
        //     - 'reset'
        //   - A complex operation that uses bdeat functions works as expected.
        //
        // Plan:
        // - Construct a schema with a root sequence record containing at
        //   least one of each of the categories to be tested.
        // - Define a manipulator class that performs a standard set of
        //   manipulations on each simple type and simple recursive operations
        //   on each sequence, choice, and nullable type.
        // - Define an accessor that records the sequence of calls.
        // - Create an 'bcem_Aggregate' object that conforms to the root
        //   record of the schema.
        // - Call 'bdeat_TypeName::className(bcem_Aggregate)' on each field
        //   and check for expected result.
        // - Check that
        //   'bdeat_TypeCategory::Select<bcem_Aggregate>::BDEAT_SELECTION' is
        //   '0'.
        // - Call 'bdeat_TypeCategoryFunctions::select(bdem_Aggregate)' on
        //   each field and check for expected result.
        // - Apply the manipulator to the aggregate and confirm that the final
        //   state of the aggregate is as expected.
        // - Apply the accessor to the aggregate and confirm that the values
        //   and sequence of accesses are as expected.
        // - To exercise the bdeat functions in a real-world scenario, encode
        //   a complex aggregate using 'bdem_berencoder' and decode
        //   it using 'bdem_berdecoderutil'.  Confirm that the decoded
        //   aggregate is equivalent to the original.
        //
        // Testing:
        //     namespace bdeat_TypeCategoryFunctions
        //     namespace bdeat_SequenceFunctions
        //     namespace bdeat_ChoiceFunctions
        //     namespace bdeat_ArrayFunctions
        //     namespace bdeat_EnumFunctions
        //     namespace bdeat_NullableValueFunctions
        //     namespace bdeat_ValueTypeFunctions
        // --------------------------------------------------------------------

        if (verbose) tst::cout << "\nTESTING 'bdeat' FUNCTIONS"
                               << "\n========================="
                               << bsl::endl;

        const char SPEC[] =
            ":f= uvx"         // ENUM "f" { "you" => 0, "vee" => 1, "ex" => 2}
            ":c? Aa&NF Bb&NF" // CHOICE "c" { CHAR "a"; SHORT "b"; }
            ":s Cc&NF Dd"     // SEQUENCE "s" { INT "c"; INT64 "d"; }
            ":r"              // SEQUENCE "r" {
            "  Fa &NF"        //     DOUBLE "a" !nullable;
            "  Fb &NT"        //     DOUBLE "b"  nullable;
            "  Gc &NF"        //     STRING "c" !nullable;
            "  Gd &NT &D1"    //     STRING "d"  nullable default="vee";
            "  +fs &NF"       //     LIST<"s"> "f" !nullable;
            "  +gs &NT"       //     LIST<"s"> "g"  nullable;
            "  %hc"           //     CHOICE<"c"> "h";
            "  Mi"            //     INT_ARRAY "i";
            "  #js"           //     TABLE<"s"> "j"
            "  @kc"           //     CHOICE_ARRAY<"c"> "k";
            "  $mf &NF"       //     INT ENUM<"f"> "m" !nullable;
            "  ^nf &NT"       //     STRING ENUM<"f"> "n" nullable;
            "  Mo &FN"        //     INT_ARRAY     "o" !nullable nillable
            "  #ps &FN"       //     TABLE<"s"> "p" nillable
            "  /qf &FN"       //     STRING_ARRAY ENUM<"f"> "q" nillable
            ;                 // }

        SchemaShdPtr schema;
        schema.createInplace(0);

        ggSchema(schema.ptr(), SPEC);
        ASSERT(5 == schema->numRecords());
        ASSERT(1 == schema->numEnumerations());
        if (veryVerbose) P(*schema);

        ASSERT(0 == TC::Select<bcem_Aggregate>::BDEAT_SELECTION);

        bcem_Aggregate mA1(schema, "r"), &A1 = mA1;
        mA1.setField("a", 4.0);
        mA1.setField("c", "22");
        mA1.setField("d", "33");
        mA1.setField("f", "c", 99);
        mA1.setField("f", "d", 999);
        mA1.field("g").makeValue();
        mA1.setField("g", "c", 88);
        mA1.setField("g", "d", 888);
        mA1.field("h").makeSelection("a", 77);
        mA1.field("i").append(6);
        mA1.field("i").append(5);
        mA1.field("i").append(4);
        mA1.field("j").resize(2);
        mA1.setField("j", 0, "c", 66);
        mA1.setField("j", 0, "d", 666);
        mA1.setField("j", 1, "c", 55);
        mA1.setField("j", 1, "d", 555);
        mA1.field("k").resize(2);
        mA1.field("k", 0).makeSelection("a", 44);
        mA1.field("k", 1).makeSelection("b", 333);
        mA1.setField("m", 1);    // == enumerator string "vee"
        mA1.setField("n", "ex"); // == enumerator ID 2
        ASSERT(A1.field("i").enumerationConstraint() == 0);
        ASSERT(A1.field("m").enumerationConstraint() ==
               schema->lookupEnumeration("f"));
        mA1.field("o").resize(2);
        mA1.setField("o", 0, "33");
        mA1.field("p").resize(2);
        mA1.field("q").resize(2);
        mA1.setField("p", 0, mA1.field("j", 0));
        mA1.setField("q", 1, "ex");

        if (veryVerbose) P(A1);

        bcem_Aggregate mA2(schema, "r"), &A2 = mA2;
        if (veryVerbose) P(A2);

        if (verbose) tst::cout << "Testing type name" << bsl::endl;
        ASSERT(streq("r", TN::className(A1))           );
        ASSERT(      0 == TN::className(A1.field("a")) );
        ASSERT(      0 == TN::className(A1.field("b")) );
        ASSERT(      0 == TN::className(A1.field("c")) );
        ASSERT(      0 == TN::className(A1.field("d")) );
        ASSERT(streq("s", TN::className(A1.field("f"))));
        ASSERT(streq("s", TN::className(A1.field("g"))));
        ASSERT(streq("c", TN::className(A1.field("h"))));
        ASSERT(      0 == TN::className(A1.field("i")) );
        ASSERT(streq("s", TN::className(A1.field("j"))));
        ASSERT(streq("c", TN::className(A1.field("k"))));
        ASSERT(streq("f", TN::className(A1.field("m"))));
        ASSERT(streq("f", TN::className(A1.field("n"))));
        ASSERT(      0 == TN::className(A1.field("o")));
        ASSERT(streq("s", TN::className(A1.field("p"))));
        ASSERT(      0 == TN::className(A1.field("q")));

        if (verbose) tst::cout << "Testing category selection" << bsl::endl;
        ASSERT(0 ==
                  bdeat_TypeCategory::Select<bcem_Aggregate>::BDEAT_SELECTION);
        ASSERT(TC::BDEAT_SEQUENCE_CATEGORY    == TCF::select(A1));
        ASSERT(TC::BDEAT_SIMPLE_CATEGORY      == TCF::select(A1.field("a")));
        ASSERT(TC::BDEAT_SIMPLE_CATEGORY      == TCF::select(A1.field("b")));
        ASSERT(TC::BDEAT_SIMPLE_CATEGORY      == TCF::select(A1.field("c")));
        ASSERT(TC::BDEAT_SIMPLE_CATEGORY      == TCF::select(A1.field("d")));
        ASSERT(TC::BDEAT_SEQUENCE_CATEGORY    == TCF::select(A1.field("f")));
        ASSERT(TC::BDEAT_SEQUENCE_CATEGORY    == TCF::select(A1.field("g")));
        ASSERT(TC::BDEAT_CHOICE_CATEGORY      == TCF::select(A1.field("h")));
        ASSERT(TC::BDEAT_ARRAY_CATEGORY       == TCF::select(A1.field("i")));
        ASSERT(TC::BDEAT_ARRAY_CATEGORY       == TCF::select(A1.field("j")));
        ASSERT(TC::BDEAT_ARRAY_CATEGORY       == TCF::select(A1.field("k")));
        ASSERT(TC::BDEAT_ENUMERATION_CATEGORY == TCF::select(A1.field("m")));
        ASSERT(TC::BDEAT_ENUMERATION_CATEGORY == TCF::select(A1.field("n")));
        ASSERT(TC::BDEAT_ARRAY_CATEGORY       == TCF::select(A1.field("o")));
        ASSERT(TC::BDEAT_ARRAY_CATEGORY       == TCF::select(A1.field("p")));
        ASSERT(TC::BDEAT_ARRAY_CATEGORY       == TCF::select(A1.field("q")));

        AggAccessor    theAccessor;
        AggManipulator theManipulator;
        NewAggAccessor    newAccessor;
        NewAggManipulator newManipulator;

        const bsl::string RA = "<a>4</a>";
        const bsl::string RB = "<b/>";
        const bsl::string RC = "<c>22</c>";
        const bsl::string RD = "<d>33</d>";
        const bsl::string RF = "<f><c>99</c><d>999</d></f>";
        const bsl::string RG = "<g><c>88</c><d>888</d></g>";
        const bsl::string RH = "<h><a>77</a></h>";
        const bsl::string RI = "<i>6</i><i>5</i><i>4</i>";
        const bsl::string RJ =
                        "<j><c>66</c><d>666</d></j><j><c>55</c><d>555</d></j>";
        const bsl::string RK = "<k><a>44</a></k><k><b>333</b></k>";
        const bsl::string RM = "<m>1</m>";
        const bsl::string RN = "<n>2</n>";
        const bsl::string RO = "<o>33</o><o xsi:nil='true'/>";
        const bsl::string RP = "<p><c>66</c><d>666</d></p><p xsi:nil='true'/>";
        const bsl::string RQ = "<q xsi:nil='true'/><q>2</q>";

        bsl::string res;
        if (verbose) tst::cout << "Testing bdeat_SequenceFunctions"<<bsl::endl;
        ASSERT(SF::IsSequence<bcem_Aggregate>::VALUE);

        newAccessor.reset();
        SF::accessAttribute(A1, newAccessor, "a", 1);
        res = newAccessor.value();
        LOOP2_ASSERT(RA, res, RA == res);

        newAccessor.reset();
        SF::accessAttribute(A1, newAccessor, "b", 1);
        res = newAccessor.value();
        LOOP2_ASSERT(RB, res, RB == res);

        newAccessor.reset();
        SF::accessAttribute(A1, newAccessor, "c", 1);
        res = newAccessor.value();
        LOOP2_ASSERT(RC, res, RC == res);

        newAccessor.reset();
        SF::accessAttribute(A1, newAccessor, "d", 1);
        res = newAccessor.value();
        LOOP2_ASSERT(RD, res, RD == res);

        newAccessor.reset();
        SF::accessAttribute(A1, newAccessor, "f", 1);
        res = newAccessor.value();
        LOOP2_ASSERT(RF, res, RF == res);

        newAccessor.reset();
        SF::accessAttribute(A1, newAccessor, "g", 1);
        res = newAccessor.value();
        LOOP2_ASSERT(RG, res, RG == res);

        newAccessor.reset();
        SF::accessAttribute(A1, newAccessor, "h", 1);
        res = newAccessor.value();
        LOOP2_ASSERT(RH, res, RH == res);

        newAccessor.reset();
        SF::accessAttribute(A1, newAccessor, "i", 1);
        res = newAccessor.value();
        LOOP2_ASSERT(RI, res, RI == res);

        newAccessor.reset();
        SF::accessAttribute(A1, newAccessor, "j", 1);
        res = newAccessor.value();
        LOOP2_ASSERT(RJ, res, RJ == res);

        newAccessor.reset();
        SF::accessAttribute(A1, newAccessor, "k", 1);
        res = newAccessor.value();
        LOOP2_ASSERT(RK, res, RK == res);

        newAccessor.reset();
        SF::accessAttribute(A1, newAccessor, "m", 1);
        res = newAccessor.value();
        LOOP2_ASSERT(RM, res, RM == res);

        newAccessor.reset();
        SF::accessAttribute(A1, newAccessor, "n", 1);
        res = newAccessor.value();
        LOOP2_ASSERT(RN, res, RN == res);

        newAccessor.reset();
        SF::accessAttribute(A1, newAccessor, "o", 1);
        res = newAccessor.value();
        LOOP2_ASSERT(RO, res, RO == res);

        newAccessor.reset();
        SF::accessAttribute(A1, newAccessor, "p", 1);
        res = newAccessor.value();
        LOOP2_ASSERT(RP, res, RP == res);

        newAccessor.reset();
        SF::accessAttribute(A1, newAccessor, "q", 1);
        res = newAccessor.value();
        LOOP2_ASSERT(RQ, res, RQ == res);

        SF::accessAttribute(A1, theAccessor, "b", 1);
        ASSERT(theAccessor.matchValue(-5000));
        SF::accessAttribute(A1.field("f"), theAccessor, "c", 1);
        ASSERT(theAccessor.matchValue(99));
        SF::accessAttribute(A1.field("f"), theAccessor, 1);
        ASSERT(theAccessor.matchValue(999));
        SF::accessAttributes(A1.field("f"), theAccessor);
        ASSERT(theAccessor.matchValues(99, 999));

        bcem_Aggregate mB(schema, "r"); const bcem_Aggregate& B = mB;

        bsl::list<CERef>& values = newManipulator.elements();
        const CERef& RefA = getCERef(ET::BDEM_DOUBLE, 1);
        values.push_back(RefA);
        SF::manipulateAttribute(&mB, newManipulator, "a", 1);
        ASSERT(compareCERefs(RefA, B.field("a").asElemRef()));

        newManipulator.reset();
        const CERef& RefB = getCERef(ET::BDEM_DOUBLE, 2);
        values.push_back(RefB);
        SF::manipulateAttribute(&mB, newManipulator, "b", 1);
        ASSERT(compareCERefs(RefB, B.field("b").asElemRef()));

        newManipulator.reset();
        const CERef& RefC = getCERef(ET::BDEM_STRING, 2);
        values.push_back(RefC);
        SF::manipulateAttribute(&mB, newManipulator, "c", 1);
        LOOP2_ASSERT(RefC,
                     B.field("c").asElemRef(),
                     compareCERefs(RefC, B.field("c").asElemRef()));

        newManipulator.reset();
        const CERef& RefD = getCERef(ET::BDEM_STRING, 0);
        values.push_back(RefD);
        SF::manipulateAttribute(&mB, newManipulator, "d", 1);
        ASSERT(compareCERefs(RefD, B.field("d").asElemRef()));

        newManipulator.reset();
        const CERef& RefFC = getCERef(ET::BDEM_INT, 2);
        const CERef& RefFD = getCERef(ET::BDEM_INT64, 1);
        values.push_back(RefFC);
        values.push_back(RefFD);
        SF::manipulateAttribute(&mB, newManipulator, "f", 1);
        LOOP2_ASSERT(RefFC,
                     B.field("f", "c").asElemRef(),
                     compareCERefs(RefFC, B.field("f", "c").asElemRef()));
        ASSERT(compareCERefs(RefFD, B.field("f", "d").asElemRef()));

        newManipulator.reset();
        const CERef& RefGC = getCERef(ET::BDEM_INT, 0);
        const CERef& RefGD = getCERef(ET::BDEM_INT64, 0);
        values.push_back(RefGC);
        values.push_back(RefGD);
        SF::manipulateAttribute(&mB, newManipulator, "g", 1);
        ASSERT(compareCERefs(RefGC, B.field("g", "c").asElemRef()));
        ASSERT(compareCERefs(RefGD, B.field("g", "d").asElemRef()));

        newManipulator.reset();
        int selector = 0;
        const CERef  RefSelector(&selector, &bdem_Properties::d_intAttr);
        values.push_back(RefSelector);
        const CERef& RefH = getCERef(ET::BDEM_CHAR, 1);
        values.push_back(RefH);
        SF::manipulateAttribute(&mB, newManipulator, "h", 1);
        ASSERT(selector == B.field("h").selectorId());
        ASSERT(bsl::string("a") == B.field("h").selector());
        ASSERT(compareCERefs(RefH, B.field("h", "a").asElemRef()));

        newManipulator.reset();
        int numItems = 1;
        const CERef  RefNI(&numItems, &bdem_Properties::d_intAttr);
        const CERef& RefI = getCERef(ET::BDEM_INT, 1);
        values.push_back(RefNI);
        values.push_back(RefI);
        SF::manipulateAttribute(&mB, newManipulator, "i", 1);
        Obj RES = B.field("i");
        ASSERT(1 == RES.length());
        LOOP2_ASSERT(RefI, RES.asElemRef(),
                     compareCERefs(RefI, RES[0].asElemRef()));

        newManipulator.reset();
        numItems = 2;
        const CERef& RefJA1 = getCERef(ET::BDEM_INT, 1);
        const CERef& RefJA2 = getCERef(ET::BDEM_INT64, 1);
        const CERef& RefJB1 = getCERef(ET::BDEM_INT, 2);
        const CERef& RefJB2 = getCERef(ET::BDEM_INT64, 2);
        values.push_back(RefNI);
        values.push_back(RefJA1);
        values.push_back(RefJA2);
        values.push_back(RefJB1);
        values.push_back(RefJB2);
        SF::manipulateAttribute(&mB, newManipulator, "j", 1);
        ASSERT(2 == B.field("j").length());
        ASSERT(compareCERefs(RefJA1, B.field("j", 0, "c").asElemRef()));
        ASSERT(compareCERefs(RefJA2, B.field("j", 0, "d").asElemRef()));
        ASSERT(compareCERefs(RefJB1, B.field("j", 1, "c").asElemRef()));
        ASSERT(compareCERefs(RefJB2, B.field("j", 1, "d").asElemRef()));

        newManipulator.reset();
        int s1 = 0, s2 = 1;
        const CERef  RefS1(&s1, &bdem_Properties::d_intAttr);
        const CERef  RefS2(&s2, &bdem_Properties::d_intAttr);
        const CERef& RefKA = getCERef(ET::BDEM_CHAR, 1);
        const CERef& RefKB = getCERef(ET::BDEM_SHORT, 1);
        values.push_back(RefNI);
        values.push_back(RefS1);
        values.push_back(RefKA);
        values.push_back(RefS2);
        values.push_back(RefKB);
        SF::manipulateAttribute(&mB, newManipulator, "k", 1);
        ASSERT(numItems == B.field("k").length());

        Obj RESA = B.field("k", 0);
        Obj RESB = B.field("k", 1);

        ASSERT(s1 == RESA.selectorId());
        ASSERT(bsl::string("a") == RESA.selector());
        LOOP_ASSERT(RefKA, compareCERefs(RefKA, RESA.selection().asElemRef()));
        ASSERT(s2 == RESB.selectorId());
        ASSERT(bsl::string("b") == RESB.selector());
        LOOP2_ASSERT(RefKB, RESB.selection().asElemRef(),
                     compareCERefs(RefKB, RESB.selection().asElemRef()));

        newManipulator.reset();
        int enumId = 1;
        const CERef enumRef1(&enumId, &bdem_Properties::d_intAttr);
        values.push_back(enumRef1);
        SF::manipulateAttribute(&mB, newManipulator, "m", 1);
        LOOP2_ASSERT(enumRef1, B.field("m").asElemRef(),
                     compareCERefs(enumRef1, B.field("m").asElemRef()));
        enumId = 2;
        values.push_back(enumRef1);
        SF::manipulateAttribute(&mB, newManipulator, "m", 1);
        LOOP2_ASSERT(enumRef1, B.field("m").asElemRef(),
                     compareCERefs(enumRef1, B.field("m").asElemRef()));

        newManipulator.reset();
        enumId = 1;
        bsl::string enumString = "1";
        const CERef enumRef2(&enumString, &bdem_Properties::d_stringAttr);
        values.push_back(enumRef2);
        SF::manipulateAttribute(&mB, newManipulator, "m", 1);
        LOOP2_ASSERT(enumRef1, B.field("m").asElemRef(),
                     compareCERefs(enumRef1, B.field("m").asElemRef()));
        enumId = 2;
        enumString = "2";
        values.push_back(enumRef2);
        SF::manipulateAttribute(&mB, newManipulator, "m", 1);
        LOOP2_ASSERT(enumRef1, B.field("m").asElemRef(),
                     compareCERefs(enumRef1, B.field("m").asElemRef()));

        newManipulator.reset();
        enumId = 1;
        values.push_back(enumRef1);
        SF::manipulateAttribute(&mB, newManipulator, "n", 1);
        ASSERT(bsl::string("vee") == B.field("n").asElemRef().theString());

        newManipulator.reset();
        enumId = 2;
        values.push_back(enumRef1);
        SF::manipulateAttribute(&mB, newManipulator, "n", 1);
        LOOP_ASSERT(B.field("n").asElemRef().theString(),
                    bsl::string("ex") == B.field("n").asElemRef().theString());

        newManipulator.reset();
        enumString = "1";
        values.push_back(enumRef2);
        SF::manipulateAttribute(&mB, newManipulator, "n", 1);
        ASSERT(bsl::string("vee") == B.field("n").asElemRef().theString());

        newManipulator.reset();
        enumString = "2";
        values.push_back(enumRef2);
        SF::manipulateAttribute(&mB, newManipulator, "n", 1);
        LOOP_ASSERT(B.field("n").asElemRef().theString(),
                    bsl::string("ex") == B.field("n").asElemRef().theString());

        newManipulator.reset();
        numItems = 2;
        const CERef& RefO = getCERef(ET::BDEM_INT, 0);
        values.push_back(RefNI);
        values.push_back(RefI);
        values.push_back(RefO);
        SF::manipulateAttribute(&mB, newManipulator, "o", 1);
        ASSERT(2 == B.field("o").length());
        LOOP_ASSERT(B.field("o", 0).asElemRef(),
                    compareCERefs(RefI, B.field("o", 0).asElemRef()));
        LOOP_ASSERT(B.field("o", 1).asElemRef(),
                    compareCERefs(RefO, B.field("o", 1).asElemRef()));

        newManipulator.reset();
        numItems = 2;
        values.push_back(RefNI);
        values.push_back(RefJA1);
        values.push_back(RefJA2);
        values.push_back(RefJB1);
        values.push_back(RefJB2);
        SF::manipulateAttribute(&mB, newManipulator, "p", 1);
        ASSERT(2 == B.field("p").length());
        ASSERT(compareCERefs(RefJA1, B.field("p", 0, "c").asElemRef()));
        ASSERT(compareCERefs(RefJA2, B.field("p", 0, "d").asElemRef()));
        ASSERT(compareCERefs(RefJB1, B.field("p", 1, "c").asElemRef()));
        ASSERT(compareCERefs(RefJB2, B.field("p", 1, "d").asElemRef()));

        newManipulator.reset();
        numItems = 1;
        enumString = "1";
        values.push_back(RefNI);
        values.push_back(enumRef2);
        SF::manipulateAttribute(&mB, newManipulator, "q", 1);
        ASSERT(1 == B.field("q").length());
        ASSERT(bsl::string("vee") == B.field("q", 0).asElemRef().theString());

        bcem_Aggregate mA2f = mA2.field("f");
        SF::manipulateAttribute(&mA2f, theManipulator,  0);
        ASSERT(100 == A2.field("f", "c").asInt());
        SF::manipulateAttribute(&mA2f, theManipulator,  "d", 1);
        ASSERT(101 == A2.field("f", "d").asInt64());
        SF::manipulateAttributes(&mA2f, theManipulator);
        ASSERT(102 == A2.field("f", "c").asInt());
        ASSERT(103 == A2.field("f", "d").asInt64());
        if (veryVerbose) P(A2);

        if (verbose) tst::cout << "Testing bdeat_ChoiceFunctions" << bsl::endl;
        ASSERT(CF::IsChoice<bcem_Aggregate>::VALUE);
        int id = 99;
        id = CF::selectionId(A1.field("h"));
        ASSERT(0 == id);
        ASSERT(CF::hasSelection(A1.field("h"), 0));
        ASSERT(CF::hasSelection(A1.field("h"), 1));
        ASSERT(! CF::hasSelection(A1.field("h"), 2));
        ASSERT(CF::hasSelection(A1.field("h"), "a", 1));
        ASSERT(CF::hasSelection(A1.field("h"), "b", 1));
        ASSERT(! CF::hasSelection(A1.field("h"), "ab", 2));
        CF::accessSelection(A1.field("h"), theAccessor);
        ASSERT(theAccessor.matchValue(77));

        bcem_Aggregate mA2h(mA2.field("h"));
        id = CF::selectionId(mA2h);
        ASSERT(bdeat_ChoiceFunctions::BDEAT_UNDEFINED_SELECTION_ID == id);
        CF::makeSelection(&mA2h, 0);
        ASSERT(0 == CF::selectionId(mA2h));
        CF::manipulateSelection(&mA2h, theManipulator);
        ASSERT(104 == A2.field("h", "a").asChar());
        CF::makeSelection(&mA2h, "b", 1);
        ASSERT(1 == CF::selectionId(mA2h));
        CF::manipulateSelection(&mA2h, theManipulator);
        ASSERT(105 == A2.field("h", "").asShort());
        if (veryVerbose) P(A2);

        if (verbose) tst::cout << "Testing bdeat_ArrayFunctions" << bsl::endl;
        ASSERT(AF::IsArray<bcem_Aggregate>::VALUE);
        ASSERT(3 == AF::size(A1.field("i")));
        AF::accessElement(A1.field("i"), theAccessor, 0);
        ASSERT(theAccessor.matchValue(6));
        AF::accessElement(A1.field("i"), theAccessor, 2);
        ASSERT(theAccessor.matchValue(4));
        AF::accessElement(A1.field("i"), theAccessor, 1);
        ASSERT(theAccessor.matchValue(5));

        A2.setField("i", A1.field("i"));
        bcem_Aggregate mA2i(A2.field("i"));
        AF::manipulateElement(&mA2i, theManipulator, 1);
        ASSERT(6   == A2.field("i", 0).asInt());
        ASSERT(106 == A2.field("i", 1).asInt());
        ASSERT(4   == A2.field("i", 2).asInt());
        AF::resize(&mA2i, 2);
        ASSERT(2   == A2.field("i").size());
        ASSERT(6   == A2.field("i", 0).asInt());
        ASSERT(106 == A2.field("i", 1).asInt());
        if (veryVerbose) P(A2);

        if (verbose) tst::cout << "Testing bdeat_ArrayFunctions on TABLEs"
                               << bsl::endl;
        ASSERT(2 == AF::size(A1.field("j")));
        AF::accessElement(A1.field("j"), theAccessor, 0);
        ASSERT(theAccessor.matchValues(66, 666));
        AF::accessElement(A1.field("j"), theAccessor, 1);
        ASSERT(theAccessor.matchValues(55, 555));

        A2.setField("j", A1.field("j"));
        bcem_Aggregate mA2j(A2.field("j"));
        AF::manipulateElement(&mA2j, theManipulator, 0);
        ASSERT(107 == A2.field("j", 0, "c").asInt());
        ASSERT(108 == A2.field("j", 0, "d").asInt64());
        ASSERT(55  == A2.field("j", 1, "c").asInt());
        ASSERT(555 == A2.field("j", 1, "d").asInt64());
        AF::resize(&mA2j, 3);
        ASSERT(3   == A2.field("j").size());
        AF::manipulateElement(&mA2j, theManipulator, 2);
        ASSERT(107 == A2.field("j", 0, "c").asInt());
        ASSERT(108 == A2.field("j", 0, "d").asInt64());
        ASSERT(55  == A2.field("j", 1, "c").asInt());
        ASSERT(555 == A2.field("j", 1, "d").asInt64());
        ASSERT(109 == A2.field("j", 2, "c").asInt());
        ASSERT(110 == A2.field("j", 2, "d").asInt64());
        if (veryVerbose) P(A2);

        if (verbose) tst::cout << "Testing bdeat_ArrayFunctions on "
                               << "CHOICE_ARRAYs" << bsl::endl;
        ASSERT(2 == AF::size(A1.field("k")));
        ASSERT(0 == CF::selectionId(A1.field("k", 0)));
        ASSERT(1 == CF::selectionId(A1.field("k", 1)));
        AF::accessElement(A1.field("k"), theAccessor, 0);
        ASSERT(theAccessor.matchValue(44));
        AF::accessElement(A1.field("k"), theAccessor, 1);
        ASSERT(theAccessor.matchValue(333));

        A2.setField("k", A1.field("k"));
        bcem_Aggregate mA2k(A2.field("k"));
        AF::manipulateElement(&mA2k, theManipulator, 0);
        ASSERT(1   == A2.field("k", 0).selectorId());
        ASSERT(111 == A2.field("k", 0, "b").asShort());
        ASSERT(1   == A2.field("k", 1).selectorId());
        ASSERT(333 == A2.field("k", 1, "b").asShort());
        if (veryVerbose) P(A2);
        AF::manipulateElement(&mA2k, theManipulator, 1);
        ASSERT(1   == A2.field("k", 0).selectorId());
        ASSERT(111 == A2.field("k", 0, "b").asShort());
        ASSERT(0   == A2.field("k", 1).selectorId());
        ASSERT(112 == A2.field("k", 1, "a").asChar());
        AF::resize(&mA2k, 3);
        ASSERT(3   == A2.field("k").size());
        AF::manipulateElement(&mA2k, theManipulator, 2);
        ASSERT(1   == A2.field("k", 0).selectorId());
        ASSERT(111 == A2.field("k", 0, "b").asShort());
        ASSERT(0   == A2.field("k", 1).selectorId());
        ASSERT(112 == A2.field("k", 1, "a").asChar());
        ASSERT(1   == A2.field("k", 2).selectorId());
        ASSERT(113 == A2.field("k", 2, "b").asShort());
        if (veryVerbose) P(A2);

        // WHITE-BOX test of bcem_Aggregate_BdeatUtil::NullableAdapter
        if (verbose) tst::cout << "Testing bdeat_NullableValueFunctions:"
                               << bsl::endl;
        typedef bcem_Aggregate_NullableAdapter NullableAdapter;
        ASSERT(! NVF::IsNullableValue<bcem_Aggregate>::VALUE);
        ASSERT(NVF::IsNullableValue<NullableAdapter>::VALUE);
        bcem_Aggregate mA1b(A1.field("b"));
        NullableAdapter nmA1b  = { &mA1b };
        ASSERT(NVF::isNull(nmA1b));
        bcem_Aggregate mA1g(A1.field("g"));
        NullableAdapter nmA1g  = { &mA1g };
        ASSERT(!NVF::isNull(nmA1g));
        bcem_Aggregate mA1gd(A1.field("g", "d"));
        NullableAdapter nmA1gd = { &mA1gd };
        ASSERT(! NVF::isNull(nmA1gd));
        NVF::accessValue(nmA1gd, theAccessor);
        ASSERT(theAccessor.matchValue(888));
        NVF::accessValue(nmA1g, theAccessor);
        ASSERT(theAccessor.matchValues(88, 888));

        bcem_Aggregate mA2b(A2.field("b"));
        NullableAdapter nmA2b  = { &mA2b };
        ASSERT(NVF::isNull(nmA2b));
        NVF::manipulateValue(&nmA2b, theManipulator);
        ASSERT(! NVF::isNull(nmA2b));
        ASSERT(114 == mA2b.asDouble());
        // Note: mA2b is a COPY.  A2.field("b") is still null.
        bcem_Aggregate mA2g(A2.field("g"));
        NullableAdapter nmA2g  = { &mA2g };
        ASSERT(NVF::isNull(nmA2g));
        mA2g.makeNull();
        NVF::makeValue(&nmA2g);
        ASSERT(! NVF::isNull(nmA2g));
        NVF::manipulateValue(&nmA2g, theManipulator);
        ASSERT(!A2.field("g", "d").isNul2());
        ASSERT(115 == A2.field("g", "c").asInt());
        ASSERT(116 == A2.field("g", "d").asInt64());

        // Black-box test of null value manipulator through parent sequence
        mA2g.makeNull();
        ASSERT(NVF::isNull(nmA2g));
        mA2.field("g").makeValue();
        SF::manipulateAttribute(&A2, theManipulator, "g", 1);
        ASSERT(! A2.field("g").isNul2());
        ASSERT(! A2.field("g", "d").isNul2());
        ASSERT(117 == A2.field("g", "c").asInt());
        ASSERT(118 == A2.field("g", "d").asInt64());
        if (veryVerbose) P(A2);

        if (verbose) tst::cout << "Testing bdeat_EnumFunctions:" << bsl::endl;
        ASSERT(EF::IsEnumeration<bcem_Aggregate>::VALUE);
        int intValue;
        bsl::string stringValue;
        EF::toInt(&intValue, A1.field("m"));
        ASSERT(1 == intValue);
        EF::toString(&stringValue, A1.field("m"));
        ASSERT("vee" == stringValue);
        EF::toInt(&intValue, A1.field("n"));
        ASSERT(2 == intValue);
        EF::toString(&stringValue, A1.field("n"));
        ASSERT("ex" == stringValue);

        bcem_Aggregate mA2m(A2.field("m")); const bcem_Aggregate &A2m = mA2m;
        int status;
        status = EF::fromInt(&mA2m, 0);
        ASSERT(0 == status);
        ASSERT(0 == A2m.asInt());
        status = EF::fromString(&mA2m, "ex", 2);
        ASSERT(0 == status);
        ASSERT(2 == A2m.asInt());
        status = EF::fromInt(&mA2m, 3);
        ASSERT(0 != status);
        ASSERT(2 == A2m.asInt());
        status = EF::fromString(&mA2m, "doubleU", 7);
        ASSERT(0 != status);
        ASSERT(2 == A2m.asInt());

        bcem_Aggregate mA2n(A2.field("n")); const bcem_Aggregate &A2n = mA2n;
        status = EF::fromInt(&mA2n, 1);
        ASSERT(0 == status);
        ASSERT("vee" == A2n.asString());
        status = EF::fromString(&mA2n, "you", 3);
        ASSERT(0 == status);
        ASSERT("you" == A2n.asString());
        status = EF::fromInt(&mA2n, 3);
        ASSERT(0 != status);
        ASSERT("you" == A2n.asString());
        status = EF::fromString(&mA2n, "doubleU", 7);
        ASSERT(0 != status);
        ASSERT("you" == A2n.asString());

        // Test enumeration operations using accessors from parent
        SF::accessAttribute(A1, theAccessor, "m", 1);
        ASSERT(theAccessor.matchValue(1));
        SF::accessAttribute(A1, theAccessor, "n", 1);
        ASSERT(theAccessor.matchValue(-2));

        // Test enumeration operations using manipulators from parent
        theManipulator.reset(2);
        SF::manipulateAttribute(&mA2, theManipulator, "m", 1);
        ASSERT(2 == A2.field("m").asInt())
        theManipulator.reset(0);
        SF::manipulateAttribute(&mA2, theManipulator, "n", 1);
        ASSERT("you" == A2.field("n").asString())

        if (veryVerbose) P(A2);

        if (verbose) tst::cout << "Testing bdeat_ValueTypeFunctions on STRING"
                               << bsl::endl;
        bcem_Aggregate mA3d(A1["d"].clone());
        const bcem_Aggregate& A3d = mA3d;
        ASSERT("33" == A3d.asString());
        bdeat_ValueTypeFunctions::reset(&mA3d);
        ASSERT(ET::BDEM_STRING == A3d.dataType());
        LOOP_ASSERT(A3d.asString(), "vee" == A3d.asString());
        bdeat_ValueTypeFunctions::assign(&mA3d, A1["c"]);
        ASSERT("22" == A3d.asString());
        ASSERT(A3d.data() != A1["c"].data());  // replace with areIdentical.

        if (verbose) tst::cout << "Testing BER encoding/decoding" << bsl::endl;
        bsl::stringstream strm;
        bdem_BerEncoderOptions berEncoderOptions;
        berEncoderOptions.setTraceLevel(veryVeryVerbose);
        bdem_BerEncoder berEncoder(&berEncoderOptions);
        status = berEncoder.encode(strm, A1);
        bsl::cerr << berEncoder.loggedMessages();
        ASSERT(0 == status);
        // Assert that enumerator "ex" is NOT stored in alpha form in stream.
        ASSERT(bsl::string::npos == strm.str().find("ex"));

        bcem_Aggregate mA4(schema, "r"); const bcem_Aggregate& A4 = mA4;
        ASSERT(! bcem_Aggregate::areEquivalent(A1, A4));

        bdem_BerDecoderOptions berDecoderOptions;
        berDecoderOptions.setTraceLevel(veryVeryVerbose);
        bdem_BerDecoder berDecoder(&berDecoderOptions);
        status = berDecoder.decode(strm, &mA4);
        bsl::cerr << berDecoder.loggedMessages();
        ASSERT(0 == status);


        bsl::ostringstream A1str, A4str;
        A1str << A1;
        A4str << A4;
        
        LOOP2_ASSERT(A1, A4, 
                     A1str.str() == A4str.str());

        if (veryVerbose) P(A4);
}

static void testCase23(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // TESTING 'makeValue' FUNCTION
        //
        // Concerns:
        //   - makeValue deeply initializes a null list with fields according
        //     to its record definition
        //   - Non empty lists and non list aggregates are not modified
        //   - Unconstrained
        //   - The allocator used for allocating memory is the same allocator
        //     with which the object was constructed
        //
        // Plan:
        //   - This function just forwards all the processing to
        //     'bdem_SchemaAggregateUtil::initListDeep' function after checking
        //     the input parameters.  So we basically need to confirm that it
        //     passes the arguments to that function correctly and that the
        //     boundary conditions are handled correctly.
        //
        // Testing:
        //   bcem_Aggregate makeValue() const;
        // --------------------------------------------------------------------

        if (verbose) tst::cout << "\nTESTING 'makeValue' FUNCTION"
                               << "\n============================"
                               << bsl::endl;
        {
            // Testing setting the value on a VOID aggregate
            Obj mX; const Obj& X = mX;
            Obj mY; const Obj& Y = mY;
            Obj mZ; const Obj& Z = mZ;
            ASSERT(Obj::areEquivalent(X, Y));
            ASSERT(Obj::areEquivalent(X, Z));

            const Obj R1 = mX.makeValue();
            ASSERT(!R1.isError());
            ASSERT(!X.isError());
            ASSERT(!Obj::areIdentical(X, R1));
            ASSERT(Obj::areEquivalent(X, Y));
        }

        const struct {
            int         d_line;
            const char *d_spec;
        } DATA[] = {
            // Line     Spec
            // ----     ----
            {   L_,   ":aCa" },
            {   L_,   ":aCa&NT" },
            {   L_,   ":aCa&NF" },
            {   L_,   ":aCa&D0" },
            {   L_,   ":aGa" },
            {   L_,   ":aGa&NT" },
            {   L_,   ":aGa&NF" },
            {   L_,   ":aGa&D0" },
            {   L_,   ":aHa" },
            {   L_,   ":aHa&NT" },
            {   L_,   ":aHa&NF" },
            {   L_,   ":aHa&D0" },
            {   L_,   ":aNa" },
            {   L_,   ":aNa&NT" },
            {   L_,   ":aNa&NF" },
            {   L_,   ":aNa&D0" },
            {   L_,   ":aNa&FN" },
            {   L_,   ":aPa" },
            {   L_,   ":aPa&NT" },
            {   L_,   ":aPa&NF" },
            {   L_,   ":aPa&D0" },
            {   L_,   ":aPa&FN" },
            {   L_,   ":aQa" },
            {   L_,   ":aQa&NT" },
            {   L_,   ":aQa&NF" },
            {   L_,   ":aQa&D0" },
            {   L_,   ":aQa&FN" },
            {   L_,   ":aRa" },
            {   L_,   ":aRa&NT" },
            {   L_,   ":aRa&NF" },
            {   L_,   ":aRa&D0" },
            {   L_,   ":aRa&FN" },
            {   L_,   ":aUa" },
            {   L_,   ":aUa&NT" },
            {   L_,   ":aUa&NF" },
            {   L_,   ":aVa" },
            {   L_,   ":aVa&NT" },
            {   L_,   ":aVa&NF" },
            {   L_,   ":aWa" },
            {   L_,   ":aWa&NT" },
            {   L_,   ":aWa&NF" },
            {   L_,   ":aWa&D0" },
            {   L_,   ":aXa" },
            {   L_,   ":aXa&NT" },
            {   L_,   ":aXa&NF" },
            {   L_,   ":aXa&D0" },
            {   L_,   ":aaa" },
            {   L_,   ":aaa&NT" },
            {   L_,   ":aaa&NF" },
            {   L_,   ":aaa&D0" },
            {   L_,   ":aaa&FN" },
            {   L_,   ":ada"    },
            {   L_,   ":ada&NT" },
            {   L_,   ":ada&NF" },
            {   L_,   ":ada&D0" },
            {   L_,   ":ada&FN" },

            {   L_,   ":aCbFcGdQf :g+ha"    },
            {   L_,   ":aCbFcGdQf :g+ha&FN" },
            {   L_,   ":aCbFcGdQf :g+ha&NT" },
            {   L_,   ":aCbFcGdQf :g+ha&NF" },

            {   L_,   ":aCbFcGdQf :g#ha"    },
            {   L_,   ":aCbFcGdQf :g#ha&FN" },
            {   L_,   ":aCbFcGdQf :g#ha&NT" },
            {   L_,   ":aCbFcGdQf :g#ha&NF" },

            {   L_,   ":a?Ca" },
            {   L_,   ":a?Ca&NT" },
            {   L_,   ":a?Ca&NF" },
            {   L_,   ":a?Ca&D0" },
            {   L_,   ":a?Gb" },
            {   L_,   ":a?Gb&NT" },
            {   L_,   ":a?Gb&NF" },
            {   L_,   ":a?Gb&D0" },
            {   L_,   ":a?Hc" },
            {   L_,   ":a?Hc&NT" },
            {   L_,   ":a?Hc&NF" },
            {   L_,   ":a?Hc&D0" },
            {   L_,   ":a?Nd" },
            {   L_,   ":a?Nd&NT" },
            {   L_,   ":a?Nd&NF" },
            {   L_,   ":a?Nd&D0" },
            {   L_,   ":a?Pf" },
            {   L_,   ":a?Pf&NT" },
            {   L_,   ":a?Pf&NF" },
            {   L_,   ":a?Pf&D0" },
            {   L_,   ":a?Qg" },
            {   L_,   ":a?Qg&NT" },
            {   L_,   ":a?Qg&NF" },
            {   L_,   ":a?Qg&D0" },
            {   L_,   ":a?Rh" },
            {   L_,   ":a?Rh&NT" },
            {   L_,   ":a?Rh&NF" },
            {   L_,   ":a?Rh&D0" },
            {   L_,   ":a?Ui" },
            {   L_,   ":a?Ui&NT" },
            {   L_,   ":a?Ui&NF" },
            {   L_,   ":a?Vj" },
            {   L_,   ":a?Vj&NT" },
            {   L_,   ":a?Vj&NF" },
            {   L_,   ":a?Wk" },
            {   L_,   ":a?Wk&NT" },
            {   L_,   ":a?Wk&NF" },
            {   L_,   ":a?Wk&D0" },
            {   L_,   ":a?al" },
            {   L_,   ":a?al&NT" },
            {   L_,   ":a?al&NF" },
            {   L_,   ":a?al&D0" },
            {   L_,   ":a?em" },
            {   L_,   ":a?em&NT" },
            {   L_,   ":a?em&NF" },
            {   L_,   ":a?fn"    },
            {   L_,   ":a?fn&NT" },
            {   L_,   ":a?fn&NF" },

            {   L_,   ":a?CbFcGdQf :g%ha"    },
            {   L_,   ":a?CbFcGdQf :g%ha&FN" },
            {   L_,   ":a?CbFcGdQf :g%ha&NT" },
            {   L_,   ":a?CbFcGdQf :g%ha&NF" },

            {   L_,   ":a?CbFcGdQf :g@ha"    },
            {   L_,   ":a?CbFcGdQf :g@ha&FN" },
            {   L_,   ":a?CbFcGdQf :g@ha&NT" },
            {   L_,   ":a?CbFcGdQf :g@ha&NF" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE = DATA[i].d_line;
            const char *SPEC = DATA[i].d_spec;
            const bool  NSA  = (bool) bsl::strstr(SPEC, "&FN")
                               && !bsl::strchr(SPEC, ' ');

            Schema schema; const Schema& SCHEMA = schema;

            ggSchema(&schema, SPEC);
            const RecDef *RECORD = NSA
                                 ? &SCHEMA.record(0)
                                 : &SCHEMA.record(SCHEMA.numRecords() - 1);

            ConstRecDefShdPtr crp(RECORD, NilDeleter(), 0);
            const ConstRecDefShdPtr& CRP = crp;

            const bdem_FieldDef& FIELD_DEF = RECORD->field(0);
            ET::Type TYPE = NSA
                        ? ET::toArrayType(SCHEMA.record(1).field(0).elemType())
                        : RECORD->field(0).elemType();
            const char *fldName     = RECORD->fieldName(0);
            const bool  HAS_DEFAULT = FIELD_DEF.hasDefaultValue();
            if (ET::BDEM_LIST == TYPE) {
                if (SCHEMA.numRecords() > 1) {
                    ASSERT(SCHEMA.record(1).field(0).recordConstraint());
                }
            }

            bslma_TestAllocator testAllocator(veryVeryVerbose);
#if !defined(BSLS_PLATFORM__CMP_MSVC) || defined(BDE_BUILD_TARGET_OPT)
         BEGIN_BSLMA_EXCEPTION_TEST {
#endif
            bslma_TestAllocator alloc(veryVeryVerbose);
            bslma_DefaultAllocatorGuard dag(&alloc);

            Obj mX(CRP, &testAllocator); const Obj& X = mX;
            Obj mA(CRP, &testAllocator); const Obj& A = mA;

            if (RecDef::BDEM_CHOICE_RECORD == RECORD->recordType()) {
                mX.makeSelection(fldName);
                mA.makeSelection(fldName);
            }

            const CERef A1 = getCERef(TYPE, 1);

            Obj mY = X.field(fldName); const Obj& Y = mY;
            Obj mB = A.setField(fldName, A1); const Obj& B = mB;
            Obj C  = B.clone(&testAllocator);

            if (veryVerbose) { P(SCHEMA) P(X) P(Y) }

            mY.makeValue();
            mB.makeValue();
            ASSERT(Obj::areEquivalent(B, C));
            LOOP_ASSERT(LINE, 0 == alloc.numBytesInUse());
            LOOP2_ASSERT(LINE, Y, !Y.asElemRef().isNull());
            LOOP2_ASSERT(LINE, B, !B.asElemRef().isNull());

            if (ET::isScalarType(TYPE) && HAS_DEFAULT) {
                LOOP_ASSERT(LINE, compareCERefs(Y.asElemRef(),
                                             RECORD->field(0).defaultValue()));
            }
            else if (ET::isAggregateType(TYPE) && SCHEMA.numRecords() > 1) {
                const bdem_ElemRef&   ELEM_REF   = Y.asElemRef();
                const bdem_RecordDef& CONSTRAINT = SCHEMA.record(0);

                switch (TYPE) {
                  case ET::BDEM_LIST: {
                    LOOP_ASSERT(LINE,
                           SchemaAggUtil::canSatisfyRecord(ELEM_REF.theList(),
                                                           CONSTRAINT));
                  } break;
                  case ET::BDEM_TABLE: {
                    LOOP_ASSERT(LINE,
                          SchemaAggUtil::canSatisfyRecord(ELEM_REF.theTable(),
                                                          CONSTRAINT));
                    Y.resize(1);
                    Y[0].makeValue();
                    LOOP_ASSERT(LINE,
                    SchemaAggUtil::canSatisfyRecord(
                                               Y.field(0).asElemRef().theRow(),
                                               CONSTRAINT));
                  } break;
                  case ET::BDEM_CHOICE: {
                    LOOP_ASSERT(LINE,
                         SchemaAggUtil::canSatisfyRecord(ELEM_REF.theChoice(),
                                                         CONSTRAINT));
                  } break;
                  case ET::BDEM_CHOICE_ARRAY: {
                    LOOP_ASSERT(LINE,
                    SchemaAggUtil::canSatisfyRecord(ELEM_REF.theChoiceArray(),
                                                    CONSTRAINT));
                    Y.resize(1);
                    Y[0].makeValue();
                    LOOP_ASSERT(LINE,
                    SchemaAggUtil::canSatisfyRecord(
                                   Y.field(0).asElemRef().theChoiceArrayItem(),
                                   CONSTRAINT));
                  } break;
                  default: {
                    ; // suppress warnings about incomplete case statement
                  }
                }
            }
            else if (NSA) {
                const bdem_Table& T = Y.asElemRef().theTable();
                LOOP_ASSERT(LINE, 0 == T.numRows());
                LOOP_ASSERT(LINE, 1 == T.numColumns());
                LOOP_ASSERT(LINE, ET::fromArrayType(TYPE) == T.columnType(0));
            }
            else {
                LOOP_ASSERT(LINE, isUnset(Y.asElemRef()));
            }
#if !defined(BSLS_PLATFORM__CMP_MSVC) || defined(BDE_BUILD_TARGET_OPT)
         } END_BSLMA_EXCEPTION_TEST
#endif
        }
}

static void testCase22(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // TESTING 'clone' FUNCTIONS
        //
        // Concerns:
        //   - 'clone' function creates a copy of both the data and meta-data
        //   - 'cloneData' function creates a copy of only of the data
        //   - The passed allocator (or the default allocator) is used for
        //     allocating memory required by the new aggregate.
        //   - No memory is leaked in the case of exceptions
        //
        // Plan:
        //   - To test the 'cloneData' function we will construct an aggregate
        //     object mX of different data types from a set S of varied
        //     aggregates.  Then we will construct a new aggregate mZ using
        //     the 'cloneData' function.  We will ensure that the data and
        //     meta-data of the mX and mZ compares equal.  We will confirm
        //     that changes to mZ do not affect mX.  We will also confirm that
        //     the allocator of mX is not used for the memory allocation in
        //     the construction of mY.
        //   - We will test the 'clone' function using the same plan as used
        //     for testing 'cloneData'.  The difference will be that we will
        //     compare that the meta-data will also be duped.
        //   - For both functions we will check that no memory is leaked in
        //     the presence of exceptions.
        //
        // Testing:
        //   bcem_Aggregate clone(bslma_Allocator *basicAllocator) const;
        //   bcem_Aggregate cloneData(bslma_Allocator *basicAllocator) const;
        // --------------------------------------------------------------------

        if (verbose) tst::cout << "\nTESTING 'clone' FUNCTIONS"
                               << "\n=========================" << bsl::endl;
        {
            bslma_TestAllocator ta1, ta2;
            Obj mX; const Obj& X = mX;
            ASSERT(X.isNul2());

            Obj mA = X.clone(); const Obj& A = mA;
            Obj mB = X.clone(&ta1); const Obj& B = mB;
            Obj mC = X.cloneData(); const Obj& C = mC;
            Obj mD = X.cloneData(&ta2); const Obj& D = mD;

            ASSERT(!Obj::areIdentical(X, A));
            ASSERT(!Obj::areIdentical(X, B));
            ASSERT(!Obj::areIdentical(X, C));
            ASSERT(!Obj::areIdentical(X, D));

            ASSERT( Obj::areEquivalent(X, A));
            ASSERT( Obj::areEquivalent(X, B));
            ASSERT( Obj::areEquivalent(X, C));
            ASSERT( Obj::areEquivalent(X, D));

            ASSERT(A.isNul2());
            ASSERT(B.isNul2());
            ASSERT(C.isNul2());
            ASSERT(D.isNul2());
        }

        const struct {
            int         d_line;
            const char *d_spec;
        } DATA[] = {
            // Line     Spec
            // ----     ----
            {   L_,   ":aCa" },
            {   L_,   ":aCa&NT" },
            {   L_,   ":aCa&NF" },
            {   L_,   ":aCa&D0" },
            {   L_,   ":aGa" },
            {   L_,   ":aGa&NT" },
            {   L_,   ":aGa&NF" },
            {   L_,   ":aGa&D0" },
            {   L_,   ":aHa" },
            {   L_,   ":aHa&NT" },
            {   L_,   ":aHa&NF" },
            {   L_,   ":aHa&D0" },
            {   L_,   ":aNa" },
            {   L_,   ":aNa&NT" },
            {   L_,   ":aNa&NF" },
            {   L_,   ":aNa&D0" },
            {   L_,   ":aNa&FN" },
            {   L_,   ":aPa" },
            {   L_,   ":aPa&NT" },
            {   L_,   ":aPa&NF" },
            {   L_,   ":aPa&D0" },
            {   L_,   ":aPa&FN" },
            {   L_,   ":aQa" },
            {   L_,   ":aQa&NT" },
            {   L_,   ":aQa&NF" },
            {   L_,   ":aQa&D0" },
            {   L_,   ":aQa&FN" },
            {   L_,   ":aRa" },
            {   L_,   ":aRa&NT" },
            {   L_,   ":aRa&NF" },
            {   L_,   ":aRa&D0" },
            {   L_,   ":aRa&FN" },
            {   L_,   ":aWa" },
            {   L_,   ":aWa&NT" },
            {   L_,   ":aWa&NF" },
            {   L_,   ":aWa&D0" },
            {   L_,   ":aaa" },
            {   L_,   ":aaa&NT" },
            {   L_,   ":aaa&NF" },
            {   L_,   ":aaa&D0" },
            {   L_,   ":aaa&FN" },

            {   L_,   ":aCbFcGdQf :g+ha" },
            {   L_,   ":aCbFcGdQf :g+ha&NT" },
            {   L_,   ":aCbFcGdQf :g+ha&NF" },

            {   L_,   ":aCbFcGdQf :g#ha" },
            {   L_,   ":aCbFcGdQf :g#ha&NT" },
            {   L_,   ":aCbFcGdQf :g#ha&NF" },

            {   L_,   ":a?CbFcGdQf :g%ha" },
            {   L_,   ":a?CbFcGdQf :g%ha&NT" },
            {   L_,   ":a?CbFcGdQf :g%ha&NF" },

            {   L_,   ":a?CbFcGdQf :g@ha" },
            {   L_,   ":a?CbFcGdQf :g@ha&NT" },
            {   L_,   ":a?CbFcGdQf :g@ha&NF" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE = DATA[i].d_line;
            const char *SPEC = DATA[i].d_spec;
            const bool  NSA  = (bool) bsl::strstr(SPEC, "&FN");

            Schema schema; const Schema& SCHEMA = schema;
            ggSchema(&schema, SPEC);
            const RecDef *RECORD = NSA
                                 ? &SCHEMA.record(0)
                                 : &SCHEMA.record(SCHEMA.numRecords() - 1);

            ConstRecDefShdPtr crp(RECORD, NilDeleter(), 0);
            const ConstRecDefShdPtr& CRP = crp;

            ET::Type TYPE = NSA
                        ? ET::toArrayType(SCHEMA.record(1).field(0).elemType())
                        : RECORD->field(0).elemType();
            const char *fldName = RECORD->fieldName(0);

            const CERef A1 = getCERef(TYPE, 1);
            const CERef A2 = getCERef(TYPE, 2);

            bslma_TestAllocator t(veryVeryVerbose);
            Obj mU(CRP, &t); const Obj& U = mU;
            Obj mV(CRP, &t); const Obj& V = mV;

            mU.setField(fldName, A1);
            Obj mX = U.field(fldName); const Obj& X = mX;

            mV.setFieldNull(fldName);
            Obj mY = V.field(fldName); const Obj& Y = mY;

            if (veryVerbose) { T_ P_(SPEC) P(SCHEMA) P(X) P(Y) };

            bslma_TestAllocator testAllocator(veryVeryVerbose);
          BEGIN_BSLMA_EXCEPTION_TEST {
            const int BYTES = t.numBytesInUse();

            // Testing cloneData
            {
                Obj mZ = X.cloneData(&testAllocator); const Obj& Z = mZ;
                ASSERT(!Obj::areIdentical(X, Z));
                ASSERT(Obj::areEquivalent(X, Z));
                ASSERT(&X.recordDef() == &Z.recordDef());
                ASSERT(BYTES == t.numBytesInUse());

                mZ.setValue(A2);
                ASSERT(!Obj::areIdentical(X, Z));
                ASSERT(!Obj::areEquivalent(X, Z));
                if (NSA) {
                    ASSERT(compareNillableTable(X, A1));
                    ASSERT(compareNillableTable(Z, A2));
                }
                else {
                    ASSERT(A1 == X.asElemRef());
                    ASSERT(A2 == Z.asElemRef());
                }

                mZ.setValue(A1);
                ASSERT(!Obj::areIdentical(X, Z));
                ASSERT( Obj::areEquivalent(X, Z));
                if (NSA) {
                    ASSERT(compareNillableTable(X, A1));
                    ASSERT(compareNillableTable(Z, A1));
                }
                else {
                    ASSERT(A1 == X.asElemRef());
                    ASSERT(A1 == Z.asElemRef());
                }

                mZ.makeNull();
                ASSERT(!Obj::areIdentical(X, Z));
                ASSERT(!Obj::areEquivalent(X, Z));
                if (NSA) {
                    ASSERT(compareNillableTable(X, A1));
                }
                else {
                    ASSERT(A1 == X.asElemRef());
                }
                ASSERT(Z.isNul2());

                Obj mW = Y.cloneData(&testAllocator); const Obj& W = mW;
                ASSERT(!Obj::areIdentical(W, Y));
                ASSERT(Obj::areEquivalent(W, Y));
                ASSERT(&W.recordDef() == &Y.recordDef());
                ASSERT(BYTES == t.numBytesInUse());

                mW.setValue(A1);
                ASSERT(!Obj::areIdentical(W, Y));
                ASSERT(!Obj::areEquivalent(W, Y));
                ASSERT(!W.isNul2());
                ASSERT( Y.isNul2());

                mW.makeNull();
                ASSERT(!Obj::areIdentical(W, Y));
                ASSERT( Obj::areEquivalent(W, Y));
            }

            // Testing clone
            {
                Obj mZ = X.clone(&testAllocator); const Obj& Z = mZ;
                ASSERT(!Obj::areIdentical(X, Z));
                ASSERT(Obj::areEquivalent(X, Z));
                ASSERT(BYTES == t.numBytesInUse());

                if (ET::isAggregateType(TYPE)) {
                    ASSERT(&X.recordDef() != &Z.recordDef());
                    ASSERT(SchemaUtil::areEquivalent(X.recordDef(),
                                                     Z.recordDef()));
                }
                mZ.setValue(A2);
                ASSERT(!Obj::areIdentical(X, Z));
                ASSERT(!Obj::areEquivalent(X, Z));
                if (NSA) {
                    ASSERT(compareNillableTable(X, A1));
                    ASSERT(compareNillableTable(Z, A2));
                }
                else {
                    ASSERT(A1 == X.asElemRef());
                    ASSERT(A2 == Z.asElemRef());
                }

                mZ.setValue(A1);
                ASSERT(!Obj::areIdentical(X, Z));
                ASSERT( Obj::areEquivalent(X, Z));
                if (NSA) {
                    ASSERT(compareNillableTable(X, A1));
                    ASSERT(compareNillableTable(Z, A1));
                }
                else {
                    ASSERT(A1 == X.asElemRef());
                    ASSERT(A1 == Z.asElemRef());
                }

                mZ.makeNull();
                ASSERT(!Obj::areIdentical(X, Z));
                ASSERT(!Obj::areEquivalent(X, Z));
                if (NSA) {
                    ASSERT(compareNillableTable(X, A1));
                }
                else {
                    ASSERT(A1 == X.asElemRef());
                }
                ASSERT(Z.isNul2());

                Obj mW = Y.clone(&testAllocator); const Obj& W = mW;
                ASSERT(!Obj::areIdentical(W, Y));
                ASSERT(Obj::areEquivalent(W, Y));
                ASSERT(BYTES == t.numBytesInUse());

                if (ET::isAggregateType(TYPE)) {
                    ASSERT(&X.recordDef() != &Z.recordDef());
                    ASSERT(SchemaUtil::areEquivalent(X.recordDef(),
                                                     Z.recordDef()));
                }

                mW.setValue(A1);
                ASSERT(!Obj::areIdentical(W, Y));
                ASSERT(!Obj::areEquivalent(W, Y));
                ASSERT(!W.isNul2());
                ASSERT( Y.isNul2());

                mW.makeNull();
                ASSERT(!Obj::areIdentical(W, Y));
                ASSERT( Obj::areEquivalent(W, Y));
            }
          } END_BSLMA_EXCEPTION_TEST
        }
}

static void testCase21(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR:
        //
        // Concerns:
        //   - Any value must be able to be copy constructed without affecting
        //     its argument.  Changes to the original and the new object
        //     should affect the other object unless the original object
        //     stores a scalar type.
        //   - If the original object stores a non-scalar type then the
        //     new object should use the allocator of the original object for
        //     subsequent memory allocations, otherwise it should use the
        //     specified allocator.
        //
        // Plan:
        //   Specify a set S whose elements have substantial and varied
        //   differences in value.  For each element in S, construct and
        //   initialize identically valued objects mY and mZ, where mY is used
        //   for copy construction and mZ serves as the control object used to
        //   verify equality with original value.  Then copy construct an
        //   object mA from mX.  Use the equality operator to confirm that both
        //   mX, mY and mA have the same value.  Modify mA and confirm that mX
        //   is still equal to mY.  If mX stores a non-scalar type then
        //   confirm that mA equals mX.  Using the allocators specified
        //   to construct the two objects ensure that each object uses the
        //   correct allocator for memory allocations and leaks no memory.
        //   Finally, confirm that the operation does not leak any memory in
        //   the face of exceptions.
        //
        // Testing:
        //   bcem_Aggregate(const bcem_Aggregate&  other,
        //                  bslma_Allocator       *basicAllocator);
        // --------------------------------------------------------------------

        if (verbose) tst::cout << "\nTESTING COPY CONSTRUCTOR"
                               << "\n========================" << bsl::endl;

        const struct {
            int         d_line;
            const char *d_spec;
        } DATA[] = {
            // Line     Spec
            // ----     ----
            {   L_,   ":aCa" },
            {   L_,   ":aCa&NT" },
            {   L_,   ":aCa&NF" },
            {   L_,   ":aCa&D0" },
            {   L_,   ":aGa" },
            {   L_,   ":aGa&NT" },
            {   L_,   ":aGa&NF" },
            {   L_,   ":aGa&D0" },
            {   L_,   ":aHa" },
            {   L_,   ":aHa&NT" },
            {   L_,   ":aHa&NF" },
            {   L_,   ":aHa&D0" },
            {   L_,   ":aNa" },
            {   L_,   ":aNa&NT" },
            {   L_,   ":aNa&NF" },
            {   L_,   ":aNa&D0" },
            {   L_,   ":aNa&FN" },
            {   L_,   ":aPa" },
            {   L_,   ":aPa&NT" },
            {   L_,   ":aPa&NF" },
            {   L_,   ":aPa&D0" },
            {   L_,   ":aPa&FN" },
            {   L_,   ":aQa" },
            {   L_,   ":aQa&NT" },
            {   L_,   ":aQa&NF" },
            {   L_,   ":aQa&D0" },
            {   L_,   ":aQa&FN" },
            {   L_,   ":aRa" },
            {   L_,   ":aRa&NT" },
            {   L_,   ":aRa&NF" },
            {   L_,   ":aRa&D0" },
            {   L_,   ":aRa&FN" },
            {   L_,   ":aUa" },
            {   L_,   ":aUa&NT" },
            {   L_,   ":aUa&NF" },
            {   L_,   ":aVa" },
            {   L_,   ":aVa&NT" },
            {   L_,   ":aVa&NF" },
            {   L_,   ":aWa" },
            {   L_,   ":aWa&NT" },
            {   L_,   ":aWa&NF" },
            {   L_,   ":aWa&D0" },
            {   L_,   ":aaa" },
            {   L_,   ":aaa&NT" },
            {   L_,   ":aaa&NF" },
            {   L_,   ":aaa&D0" },
            {   L_,   ":aaa&FN" },
            {   L_,   ":aea" },
            {   L_,   ":aea&NT" },
            {   L_,   ":aea&NF" },
            {   L_,   ":afa" },
            {   L_,   ":afa&NT" },
            {   L_,   ":afa&NF" },

            {   L_,   ":aCbFcGdQf :g+ha" },
            {   L_,   ":aCbFcGdQf :g+ha&NT" },
            {   L_,   ":aCbFcGdQf :g+ha&NF" },

            {   L_,   ":aCbFcGdQf :g#ha" },
            {   L_,   ":aCbFcGdQf :g#ha&NT" },
            {   L_,   ":aCbFcGdQf :g#ha&NF" },

            {   L_,   ":a?CbFcGdQf :g%ha" },
            {   L_,   ":a?CbFcGdQf :g%ha&NT" },
            {   L_,   ":a?CbFcGdQf :g%ha&NF" },

            {   L_,   ":a?CbFcGdQf :g@ha" },
            {   L_,   ":a?CbFcGdQf :g@ha&NT" },
            {   L_,   ":a?CbFcGdQf :g@ha&NF" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE = DATA[i].d_line;
            const char *SPEC = DATA[i].d_spec;
            const bool  NSA  = (bool) bsl::strstr(SPEC, "&FN");

            Schema schema; const Schema& SCHEMA = schema;
            ggSchema(&schema, SPEC);
            const RecDef *RECORD = NSA
                                 ? &SCHEMA.record(0)
                                 : &SCHEMA.record(SCHEMA.numRecords() - 1);

            ConstRecDefShdPtr crp(RECORD, NilDeleter(), 0);
            const ConstRecDefShdPtr& CRP = crp;

            ET::Type TYPE = NSA
                        ? ET::toArrayType(SCHEMA.record(1).field(0).elemType())
                        : RECORD->field(0).elemType();
            const char *fldName = RECORD->fieldName(0);

            const CERef A1 = getCERef(TYPE, 1);
            const CERef A2 = getCERef(TYPE, 2);

            if (veryVerbose) { T_ P_(SPEC) P(SCHEMA) };

            bslma_TestAllocator testAllocator(veryVeryVerbose);
            Obj mX(CRP, &testAllocator); const Obj& X = mX;
            Obj mY(CRP, &testAllocator); const Obj& Y = mY;

            mX.setField(fldName, A1);
            mY.setFieldNull(fldName);

            Obj mU = X.field(fldName); const Obj& U = mU;
            Obj mV = Y.field(fldName); const Obj& V = mV;

          BEGIN_BSLMA_EXCEPTION_TEST {
            bslma_TestAllocator alloc1(veryVeryVerbose);
            bslma_DefaultAllocatorGuard allocGuard(&alloc1);

            {
                Obj mA(U); const Obj& A = mA;
                ASSERT(Obj::areIdentical(A, U));
                ASSERT(0 == alloc1.numBytesInUse());
                mA.setValue(A2);
                ASSERT(Obj::areIdentical(A, U));
                mA.makeNull();
                ASSERT(Obj::areIdentical(A, U));

                mY.setFieldNull(fldName);
                Obj mB(V); const Obj& B = mB;
                ASSERT(Obj::areIdentical(B, V));
                ASSERT(0 == alloc1.numBytesInUse());
                mB.setValue(A2);
                ASSERT(Obj::areIdentical(B, V));
                mA.makeNull();
                ASSERT(Obj::areIdentical(B, V));
            }
          } END_BSLMA_EXCEPTION_TEST
        }
}

static void testCase20(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // TESTING 'makeSelectionById' MANIPULATORS & 'selectorId':
        //
        // Concerns:
        //   - Confirm that the single argument function sets the selector to
        //     the specified id.
        //   - Confirm that the two argument function sets the selector to
        //     the specified id and the value at that id to the specified
        //     value.
        //   - Both functions return the selected aggregated by reference.
        //   - If the aggregate does not store a choice or choice array then
        //     an error object is returned.
        //   - 'selectorId' returns the correct selected id
        //
        // Plan:
        //   - These functions forward their processing to two private
        //     functions that have already been tested in a previous test
        //     case.  So in this test we will just perform white box testing
        //     to confirm that the functions pass their arguments correctly to
        //     the private helper functions.  Note that we test the two types
        //     of choice aggregates (one storing a choice and the other a
        //     choice array item).
        //
        // Testing:
        //   bcem_Aggregate makeSelectionById(int newSelectorId) const;
        //   template <typename VALTYPE>
        //   bcem_Aggregate makeSelectionById(int            newSelectorId,
        //                                    const VALTYPE& value) const;
        //   int selectorId() const;
        // --------------------------------------------------------------------

        if (verbose) tst::cout << "\nTESTING 'makeSelectionById' MANIPULATORS"
                               << "\n========================================"
                               << bsl::endl;

        if (verbose) tst::cout << "\nTesting choice aggregates"
                               << "\n=========================" << bsl::endl;
        {
            const struct {
                int         d_line;
                const char *d_spec;
            } DATA[] = {
                // Line    Spec
                // ----    ----
                {   L_,    ":a?CcFfGgHhNnPpQqRrWw :b%ba" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const char *SPEC = DATA[i].d_spec;

                Schema schema; const Schema& SCHEMA = schema;
                ggSchema(&schema, SPEC);
                const RecDef& REC = SCHEMA.record(SCHEMA.numRecords() - 1);
                const RecDef& SUB_REC = SCHEMA.record(0);
                ConstRecDefShdPtr RP(&REC, NilDeleter(), 0);

                if (veryVerbose) { T_ P(SPEC) P(SCHEMA) };

                bslma_TestAllocator t(veryVeryVerbose);
                Obj mW(RP, &t); const Obj& W = mW;
                Obj mX(RP, &t); const Obj& X = mX;
                Obj mY(RP, &t); const Obj& Y = mY;

                if (veryVerbose) { T_ P(X) };

                for (int j = 0; j < SUB_REC.numFields(); ++j) {
                    ET::Type    TYPE = SUB_REC.field(j).elemType();
                    const int   ID   = SUB_REC.fieldId(j);
                    const char *NAME = SUB_REC.fieldName(j);

                    const CERef VA = getCERef(TYPE, 1);

                    Obj mE = W.field(REC.fieldName(0)).makeSelection(NAME);
                    const Obj& E = mE;
                    Obj mZ = Y.field(REC.fieldName(0)).makeSelection(NAME, VA);
                    const Obj& Z = mZ;

                    Obj mC = X.field(REC.fieldName(0)); const Obj& C = mC;

                    Obj mA = mC.makeSelectionById(ID); const Obj& A = mA;
                    ASSERT(A.asElemRef() == E.asElemRef());
                    ASSERT(ID == C.selectorId());
                    ASSERT(C.selection().asElemRef() == E.asElemRef());

                    Obj mB = mC.makeSelectionById(ID, Z);
                    const Obj& B = mB;
                    ASSERT(B.asElemRef() == VA);
                    ASSERT(ID == C.selectorId());
                    ASSERT(C.selection().asElemRef() == VA);
                }
            }
        }

        if (verbose) tst::cout << "\nTesting choice array item aggregates"
                               << "\n===================================="
                               << bsl::endl;
        {
            const struct {
                int         d_line;
                const char *d_spec;
            } DATA[] = {
                // Line    Spec
                // ----    ----
                {   L_,    ":a?CcFfGgHhNnPpQqRrWw :b@ba" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const char *SPEC = DATA[i].d_spec;

                Schema schema; const Schema& SCHEMA = schema;
                ggSchema(&schema, SPEC);
                const RecDef& REC = SCHEMA.record(SCHEMA.numRecords() - 1);
                const RecDef& SUB_REC = SCHEMA.record(0);
                ConstRecDefShdPtr RP(&REC, NilDeleter(), 0);

                if (veryVerbose) { T_ P(SPEC) P(SCHEMA) };

                bslma_TestAllocator t(veryVeryVerbose);
                Obj mW(RP, &t); const Obj& W = mW;
                Obj mX(RP, &t); const Obj& X = mX;
                Obj mY(RP, &t); const Obj& Y = mY;

                if (veryVerbose) { T_ P(X) };

                for (int j = 0; j < SUB_REC.numFields(); ++j) {
                    ET::Type    TYPE = SUB_REC.field(j).elemType();
                    const int   ID   = SUB_REC.fieldId(j);
                    const char *NAME = SUB_REC.fieldName(j);

                    const CERef VA = getCERef(TYPE, 1);

                    W.field(REC.fieldName(0)).appendItems(1);
                    X.field(REC.fieldName(0)).appendItems(1);
                    Y.field(REC.fieldName(0)).appendItems(1);
                    Obj mE = W.field(REC.fieldName(0), 0).makeSelection(NAME);
                    const Obj& E = mE;
                    Obj mZ = Y.field(REC.fieldName(0), 0).makeSelection(NAME,
                                                                        VA);
                    const Obj& Z = mZ;

                    Obj mC = X.field(REC.fieldName(0), 0); const Obj& C = mC;

                    Obj mA = mC.makeSelectionById(ID); const Obj& A = mA;
                    ASSERT(A.asElemRef() == E.asElemRef());
                    ASSERT(ID == C.selectorId());
                    ASSERT(C.selection().asElemRef() == E.asElemRef());

                    Obj mB = mC.makeSelectionById(ID, Z);
                    const Obj& B = mB;
                    ASSERT(B.asElemRef() == VA);
                    ASSERT(ID == C.selectorId());
                    ASSERT(C.selection().asElemRef() == VA);
                }
            }
        }
}

static void testCase19(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // TESTING 'makeSelection' MANIPULATORS:
        //
        // Concerns:
        //   - Confirm that the single argument function sets the selector to
        //     the specified name.
        //   - Confirm that the single argument function initializes the
        //     selection to its default value for scalar values.
        //   - Confirm that the single argument function constructs the value
        //     for aggregate selections.
        //   - Confirm that the two argument function sets the selector to
        //     the specified name and the value at that name to the specified
        //     value.
        //   - Both functions return the selected aggregated by reference.
        //   - If the aggregate does not store a choice or choice array then
        //     an error object is returned.
        //
        // Plan:
        //   - These functions forward their processing to two private
        //     functions.  So our endeavor in this test is to confirm that
        //     those functions are tested.  Firstly we divide our test into
        //     two parts to test the two kinds of choice aggregates.  In the
        //     first part we will test the makeSelection function on
        //     aggregates that store a choice and in the second test we will
        //     test aggregates that store a choice array item.  For both tests
        //     we will construct an aggregate mX from a set S of aggregate
        //     specifications.  We will call the makeSelection functions for a
        //     field value and confirm that in each case selection is modified
        //     as expected.  We will test the value of the returned aggregate
        //     is correct.  Finally, we will test that calling the function on
        //     a non-choice aggregate prints an error object is returned.
        //
        // Testing:
        //  Aggregate makeSelection(const char         *newSelector) const;
        //  Aggregate makeSelection(const bsl::string&  newSelector) const;
        //  template <typename VALTYPE>
        //  bcem_Aggregate makeSelection(const char        *newSelector,
        //                               const VALTYPE&     value) const;
        //  template <typename VALTYPE>
        //  bcem_Aggregate makeSelection(const bsl::string& newSelector,
        //                               const VALTYPE&     value) const;
        //  bcem_Aggregate selection() const;
        //  const char *selector() const;
        //
        //  Private functions:
        //    bcem_Aggregate makeSelectionByIndex(int selectorIndex) const;
        //    template <typename VALTYPE>
        //    bcem_Aggregate makeSelectionByIndex(int            selectorIndex,
        //                                        const VALTYPE& value) const;
        //    void initField(void *data, const bdem_FieldDef& fieldDef);
        // --------------------------------------------------------------------

        if (verbose) tst::cout << "\nTESTING CHOICE MANIPULATORS"
                               << "\n===========================" << bsl::endl;

        if (verbose) tst::cout << "\nTesting choice aggregates"
                               << "\n=========================" << bsl::endl;
        {
            const struct {
                int         d_line;
                const char *d_spec;
            } DATA[] = {
                // Line     Spec
                // ----     ----
                {   L_,   ":a?Ca" },
                {   L_,   ":a?Ca&NT" },
                {   L_,   ":a?Ca&NF" },
                {   L_,   ":a?Ca&D0" },
                {   L_,   ":a?Gb" },
                {   L_,   ":a?Gb&NT" },
                {   L_,   ":a?Gb&NF" },
                {   L_,   ":a?Gb&D0" },
                {   L_,   ":a?Hc" },
                {   L_,   ":a?Hc&NT" },
                {   L_,   ":a?Hc&NF" },
                {   L_,   ":a?Hc&D0" },
                {   L_,   ":a?Nd" },
                {   L_,   ":a?Nd&NT" },
                {   L_,   ":a?Nd&NF" },
                {   L_,   ":a?Nd&D0" },
                {   L_,   ":a?Pf" },
                {   L_,   ":a?Pf&NT" },
                {   L_,   ":a?Pf&NF" },
                {   L_,   ":a?Pf&D0" },
                {   L_,   ":a?Qg" },
                {   L_,   ":a?Qg&NT" },
                {   L_,   ":a?Qg&NF" },
                {   L_,   ":a?Qg&D0" },
                {   L_,   ":a?Rh" },
                {   L_,   ":a?Rh&NT" },
                {   L_,   ":a?Rh&NF" },
                {   L_,   ":a?Rh&D0" },
                {   L_,   ":a?Ui" },
                {   L_,   ":a?Ui&NT" },
                {   L_,   ":a?Ui&NF" },
                {   L_,   ":a?Vj" },
                {   L_,   ":a?Vj&NT" },
                {   L_,   ":a?Vj&NF" },
                {   L_,   ":a?Wk" },
                {   L_,   ":a?Wk&NT" },
                {   L_,   ":a?Wk&NF" },
                {   L_,   ":a?Wk&D0" },
                {   L_,   ":a?al" },
                {   L_,   ":a?al&NT" },
                {   L_,   ":a?al&NF" },
                {   L_,   ":a?al&D0" },
                {   L_,   ":a?em" },
                {   L_,   ":a?em&NT" },
                {   L_,   ":a?em&NF" },
                {   L_,   ":a?fn"    },
                {   L_,   ":a?fn&NT" },
                {   L_,   ":a?fn&NF" },

                {   L_,   ":aCbFcGdQf :g?+oa"    },
                {   L_,   ":aCbFcGdQf :g?+oa&NT" },
                {   L_,   ":aCbFcGdQf :g?+oa&NF" },

                {   L_,   ":aCbFcGdQf :g?#pa"    },
                {   L_,   ":aCbFcGdQf :g?#pa&NT" },
                {   L_,   ":aCbFcGdQf :g?#pa&NF" },

                {   L_,   ":a?CbFcGdQf :g?%qa"    },
                {   L_,   ":a?CbFcGdQf :g?%qa&NT" },
                {   L_,   ":a?CbFcGdQf :g?%qa&NF" },

                {   L_,   ":a?CbFcGdQf :g?@ra"    },
                {   L_,   ":a?CbFcGdQf :g?@ra&NT" },
                {   L_,   ":a?CbFcGdQf :g?@ra&NF" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const char *SPEC = DATA[i].d_spec;

                Schema schema; const Schema& SCHEMA = schema;
                ggSchema(&schema, SPEC);
                const RecDef *RECORD = &SCHEMA.record(SCHEMA.numRecords() - 1);

                ConstRecDefShdPtr crp(RECORD, NilDeleter(), 0);
                const ConstRecDefShdPtr& CRP = crp;

                ET::Type TYPE = RECORD->field(0).elemType();
                const bdem_FieldDef& FIELD_DEF = RECORD->field(0);
                const bool HAS_DEFAULT = FIELD_DEF.hasDefaultValue();
                bool IS_NULL;
                if (HAS_DEFAULT
                 || (ET::isArrayType(TYPE) && bsl::strstr(SPEC, "NF"))) {
                    IS_NULL = false;
                }
                else {
                    IS_NULL = true;
                }

                if (ET::BDEM_LIST == TYPE) {
                    if (SCHEMA.numRecords() > 1) {
                        ASSERT(SCHEMA.record(1).field(0).recordConstraint());
                        IS_NULL = false;
                    }
                }
                bsl::string fldName = RECORD->fieldName(0);

                const CERef VA = getCERef(TYPE, 1);

                bslma_TestAllocator t(veryVeryVerbose);
                Obj mX(CRP, &t); const Obj& X = mX;
                Obj mY(CRP, &t); const Obj& Y = mY;

                if (veryVerbose) { T_ P_(SPEC) P(SCHEMA) P(X) P(Y) };

                mY.makeSelection(fldName.c_str());

                LOOP2_ASSERT(LINE, Y, !Y.isNul2());
                if (ET::isScalarType(TYPE)) {
                    if (FIELD_DEF.defaultValue().isNull()) {
                        ASSERT(isUnset(mY.fieldRef(fldName.c_str())));
                    }
                    else {
                        LOOP3_ASSERT(LINE,
                                     FIELD_DEF.defaultValue(),
                                     mY.fieldRef(fldName.c_str()),
                                     compareCERefs(
                                                FIELD_DEF.defaultValue(),
                                                mY.fieldRef(fldName.c_str())));
                    }
                }

                mY.setField(fldName, VA);
                Obj mZ = Y.field(fldName); const Obj& Z = mZ;

                Obj mW(CRP, &t); const Obj& W = mW;
                mW.makeSelection(fldName.c_str());
                Obj mE = W.field(fldName.c_str()); const Obj& E = mE;

                Obj mA = mX.makeSelection(fldName.c_str()); const Obj& A = mA;
                ASSERT(A.asElemRef() == E.asElemRef());
                ASSERT(streq(fldName.c_str(), X.selector()));
                ASSERT(X.selection().asElemRef() == E.asElemRef());

                Obj mN = mX.makeSelection("");  const Obj& N = mN;
                ASSERT(bdem_ElemType::BDEM_VOID == N.dataType());
                ASSERT('\0' == X.selector()[0]);
                ASSERT(BDEM_NULL_FIELD_ID == X.selectorId());
                ASSERT(-1 == X.selectorIndex());
                ASSERT(ET::BDEM_VOID == X.selection().dataType());

                Obj mB = mX.makeSelection(fldName.c_str(), Z);
                const Obj& B = mB;
                LOOP2_ASSERT(B, VA, B.asElemRef() == VA);
                ASSERT(streq(fldName.c_str(), X.selector()));
                ASSERT(X.selection().asElemRef() == VA);

                Obj mC = mX.makeSelection(fldName); const Obj& C = mC;
                ASSERT(C.asElemRef() == E.asElemRef());
                ASSERT(streq(fldName.c_str(), X.selector()));
                ASSERT(X.selection().asElemRef() == E.asElemRef());

                Obj mD = mX.makeSelection(fldName, Z); const Obj& D = mD;
                ASSERT(D.asElemRef() == VA);
                ASSERT(streq(fldName.c_str(), X.selector()));
                ASSERT(X.selection().asElemRef() == VA);
            }
        }

        if (verbose) tst::cout << "\nTesting choice array item aggregates"
                               << "\n===================================="
                               << bsl::endl;
        {
            const struct {
                int         d_line;
                const char *d_spec;
            } DATA[] = {
                // Line     Spec
                // ----     ----
                {   L_,   ":a?Ca :b@ba" },
                {   L_,   ":a?Gb :b@ba" },
                {   L_,   ":a?Hc :b@ba" },
                {   L_,   ":a?Nd :b@ba" },
                {   L_,   ":a?Pf :b@ba" },
                {   L_,   ":a?Qg :b@ba" },
                {   L_,   ":a?Rh :b@ba" },
                {   L_,   ":a?Ui :b@ba" },
                {   L_,   ":a?Vj :b@ba" },
                {   L_,   ":a?Wk :b@ba" },
                {   L_,   ":a?al :b@ba" },
                {   L_,   ":a?em :b@ba" },
                {   L_,   ":a?fn :b@ba" },
                {   L_,   ":aCbFcGdQf :g?+oa :b@bg" },
                {   L_,   ":aCbFcGdQf :g?#pa :b@bg" },
                {   L_,   ":a?CbFcGdQf :g?%qa :b@bg" },
                {   L_,   ":a?CbFcGdQf :g?@ra :b@bg" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const char *SPEC = DATA[i].d_spec;

                Schema schema; const Schema& SCHEMA = schema;
                ggSchema(&schema, SPEC);
                const RecDef *RECORD = &SCHEMA.record(SCHEMA.numRecords() - 1);
                const RecDef *SUB_REC =
                                       &SCHEMA.record(SCHEMA.numRecords() - 2);
                ConstRecDefShdPtr crp(RECORD, NilDeleter(), 0);
                const ConstRecDefShdPtr& CRP = crp;

                bsl::string fldName = RECORD->fieldName(0);
                bsl::string selName = SUB_REC->fieldName(0);
                ET::Type SEL_TYPE   = SUB_REC->field(0).elemType();

                const CERef VA = getCERef(SEL_TYPE, 1);

                if (veryVerbose) { T_ P_(SPEC) P(SCHEMA) };

                bslma_TestAllocator t(veryVeryVerbose);
                Obj mW(CRP, &t); const Obj& W = mW;
                Obj mX(CRP, &t); const Obj& X = mX;
                Obj mY(CRP, &t); const Obj& Y = mY;

                if (veryVerbose) { T_ P(X) P(Y) P(W) };

                mW.field(fldName.c_str()).appendItems(1);
                mX.field(fldName.c_str()).appendItems(1);
                mY.field(fldName.c_str()).appendItems(1);

                W.field(fldName.c_str(), 0).makeSelection(selName.c_str());
                Y.field(fldName.c_str(), 0).makeSelection(selName.c_str());
                Y.setField(fldName.c_str(), 0, selName.c_str(), VA);

                if (veryVerbose) { T_ P(X) P(Y) P(W) };
                Obj mE = W.field(fldName.c_str(), 0, selName.c_str());
                const Obj& E = mE;
                Obj mZ = Y.field(fldName.c_str(), 0, selName.c_str());
                const Obj& Z = mZ;

                Obj mF = X.field(fldName.c_str(), 0); const Obj& F = mF;

                Obj mA = mF.makeSelection(selName.c_str()); const Obj& A = mA;
                ASSERT(A.asElemRef() == E.asElemRef());
                ASSERT(streq(selName.c_str(), F.selector()));
                ASSERT(F.selection().asElemRef() == E.asElemRef());

                Obj mB = mF.makeSelection(selName.c_str(), Z);
                const Obj& B = mB;
                ASSERT(B.asElemRef() == VA);
                ASSERT(streq(selName.c_str(), F.selector()));
                ASSERT(F.selection().asElemRef() == VA);

                Obj mN = mF.makeSelection("");  const Obj& N = mN;
                ASSERT(bdem_ElemType::BDEM_VOID == N.dataType());
                ASSERT('\0' == F.selector()[0]);
                ASSERT(BDEM_NULL_FIELD_ID == F.selectorId());
                ASSERT(-1 == F.selectorIndex());
                ASSERT(ET::BDEM_VOID == F.selection().dataType());

                Obj mC = mF.makeSelection(selName); const Obj& C = mC;
                ASSERT(C.asElemRef() == E.asElemRef());
                ASSERT(streq(selName.c_str(), F.selector()));
                ASSERT(F.selection().asElemRef() == E.asElemRef());

                Obj mD = mF.makeSelection(selName, Z);
                const Obj& D = mD;
                ASSERT(D.asElemRef() == VA);
                ASSERT(streq(selName.c_str(), F.selector()));
                ASSERT(F.selection().asElemRef() == VA);
            }
        }

        // Test that error object is returned if data type does not match
        {
            Schema schema; const Schema& SCHEMA = schema;
            ggSchema(&schema, ":aAa");
            const RecDef *RECORD = &SCHEMA.record(0);
            ConstRecDefShdPtr crp(RECORD, NilDeleter(), 0);
            const ConstRecDefShdPtr& CRP = crp;

            bsl::string errFld = RECORD->fieldName(0);
            ET::Type TYPE      = RECORD->field(0).elemType();

            const CERef VA = getCERef(TYPE, 1);

            Obj mX(CRP);

            ASSERT_AGG_ERROR(mX.makeSelection(errFld), BCEM_ERR_TBD);

            ASSERT_AGG_ERROR(mX.makeSelection(errFld, VA), BCEM_ERR_TBD);
        }
}

static void testCase18(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // TESTING 'appendItems' and 'removeAllItems' FUNCTION:
        //
        // Concerns:
        //   - That appendItems appends the correct number of elements
        //     at the end
        //   - That removeAllItems removes all elements stored in the aggregate
        //
        // Plan:
        //   - Both appendItems and removeAllItems call underlying functions
        //     for their processing.  Hence we will only test them minimally.
        //
        // Testing:
        //   void appendItems(int numItems) const;
        //   void removeAllItems() const;
        // --------------------------------------------------------------------

        if (verbose) tst::cout
                << "\nTESTING 'appendItems' and 'removeAllItems'FUNCTION"
                << "\n=================================================="
                << bsl::endl;

        const struct {
            int         d_line;
            const char *d_spec;
        } DATA[] = {
                // Line     Spec
                // ----     ----
                {   L_,   ":aKa" },
                {   L_,   ":aKa&D0" },
                {   L_,   ":aKa&NT" },
                {   L_,   ":aKa&NF" },
                {   L_,   ":aKa&FN" },
                {   L_,   ":aLa" },
                {   L_,   ":aLa&D0" },
                {   L_,   ":aLa&NT" },
                {   L_,   ":aLa&NF" },
                {   L_,   ":aLa&FN" },
                {   L_,   ":aMa" },
                {   L_,   ":aMa&D0" },
                {   L_,   ":aMa&NT" },
                {   L_,   ":aMa&NF" },
                {   L_,   ":aMa&FN" },
                {   L_,   ":aNa" },
                {   L_,   ":aNa&D0" },
                {   L_,   ":aNa&NT" },
                {   L_,   ":aNa&NF" },
                {   L_,   ":aNa&FN" },
                {   L_,   ":aOa" },
                {   L_,   ":aOa&D0" },
                {   L_,   ":aOa&NT" },
                {   L_,   ":aOa&NF" },
                {   L_,   ":aOa&FN" },
                {   L_,   ":aPa" },
                {   L_,   ":aPa&D0" },
                {   L_,   ":aPa&NT" },
                {   L_,   ":aPa&NF" },
                {   L_,   ":aPa&FN" },
                {   L_,   ":aQa" },
                {   L_,   ":aQa&D0" },
                {   L_,   ":aQa&NT" },
                {   L_,   ":aQa&NF" },
                {   L_,   ":aQa&FN" },
                {   L_,   ":aRa" },
                {   L_,   ":aRa&D0" },
                {   L_,   ":aRa&NT" },
                {   L_,   ":aRa&NF" },
                {   L_,   ":aRa&FN" },
                {   L_,   ":aSa" },
                {   L_,   ":aSa&D0" },
                {   L_,   ":aSa&NT" },
                {   L_,   ":aSa&NF" },
                {   L_,   ":aSa&FN" },
                {   L_,   ":aTa" },
                {   L_,   ":aTa&D0" },
                {   L_,   ":aTa&NT" },
                {   L_,   ":aTa&NF" },
                {   L_,   ":aTa&FN" },
                {   L_,   ":aaa" },
                {   L_,   ":aaa&D0" },
                {   L_,   ":aaa&NT" },
                {   L_,   ":aaa&NF" },
                {   L_,   ":aaa&FN" },
                {   L_,   ":aba" },
                {   L_,   ":aba&D0" },
                {   L_,   ":aba&NT" },
                {   L_,   ":aba&NF" },
                {   L_,   ":aba&FN" },
                {   L_,   ":aca" },
                {   L_,   ":aca&D0" },
                {   L_,   ":aca&NT" },
                {   L_,   ":aca&NF" },
                {   L_,   ":aca&FN" },
                {   L_,   ":ada" },
                {   L_,   ":ada&D0" },
                {   L_,   ":ada&NT" },
                {   L_,   ":ada&NF" },
                {   L_,   ":ada&FN" },

                {   L_,   ":aCbFcGdQf :g#ha" },
                {   L_,   ":aCbFcGdQf :g#ha&NT" },
                {   L_,   ":aCbFcGdQf :g#ha&NF" },
                {   L_,   ":a?CbFcGdQf :g@ha" },
                {   L_,   ":a?CbFcGdQf :g@ha&NT" },
                {   L_,   ":a?CbFcGdQf :g@ha&NF" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE = DATA[i].d_line;
            const char *SPEC = DATA[i].d_spec;
            const bool  NSA  = (bool) bsl::strstr(SPEC, "&FN");

            Schema schema; const Schema& SCHEMA = schema;
            ggSchema(&schema, SPEC);
            const RecDef *RECORD = NSA
                                 ? &SCHEMA.record(0)
                                 : &SCHEMA.record(SCHEMA.numRecords() - 1);

            ConstRecDefShdPtr crp(RECORD, NilDeleter(), 0);
            const ConstRecDefShdPtr& CRP = crp;

            ET::Type TYPE, ARRAY_TYPE;
            bool IS_NULL;
            if (NSA) {
                TYPE       = SCHEMA.record(1).field(0).elemType();
                ARRAY_TYPE = ET::toArrayType(TYPE);
                IS_NULL    = true;
            }
            else {
                ARRAY_TYPE = RECORD->field(0).elemType();
                TYPE       = ET::fromArrayType(ARRAY_TYPE);
                IS_NULL    = false;
            }
            if (RecDef::BDEM_CHOICE_RECORD == SCHEMA.record(0).recordType()) {
                IS_NULL = true;
            }

            const char *fldName         = RECORD->fieldName(0);
            const bool  hasDefaultValue = RECORD->field(0).hasDefaultValue();

            const CERef VA = getCERef(TYPE, 1);
            const CERef VB = getCERef(TYPE, 2);

            if (veryVerbose) { T_ P_(SPEC) P(SCHEMA) };

            bslma_TestAllocator testAllocator(veryVeryVerbose);

#ifndef BSLS_PLATFORM__CMP_MSVC
          BEGIN_BSLMA_EXCEPTION_TEST {
#endif
            Obj mX(CRP, &testAllocator); const Obj& X = mX;

            Obj mA = X.field(fldName); const Obj& A = mA;

            mA.appendItems(0);
            ASSERT(0 == A.length());

            mA.appendNullItems(0);
            ASSERT(0 == A.length());

            mA.appendItems(1);
            ASSERT(1 == A.length());
            if (hasDefaultValue) {
                ASSERT(compareCERefs(RECORD->field(0).defaultValue(),
                                     A[0].asElemRef()));
            }
            else {
                ASSERT(isUnset(A[0].asElemRef()));
            }

            mA.setField(0, VA);
            ASSERT(compareCERefs(VA, A[0].asElemRef()));

            mA.appendNullItems(1);
            ASSERT(2 == A.length());
            ASSERT(compareCERefs(VA, A[0].asElemRef()));
            ASSERT(IS_NULL == A[1].isNul2());

            mA.appendItems(1);
            ASSERT(3 == A.length());
            ASSERT(compareCERefs(VA, A[0].asElemRef()));
            ASSERT(IS_NULL == A[1].isNul2());
            if (hasDefaultValue) {
                ASSERT(compareCERefs(RECORD->field(0).defaultValue(),
                                     A[2].asElemRef()));
            }
            else {
                ASSERT(isUnset(A[2].asElemRef()));
            }

            mA.setField(2, VB);
            ASSERT(compareCERefs(VB, A[2].asElemRef()));

            mA.appendNullItems(1);
            ASSERT(4 == A.length());
            ASSERT(compareCERefs(VA, A[0].asElemRef()));
            ASSERT(compareCERefs(VB, A[2].asElemRef()));
            ASSERT(IS_NULL == A[1].isNul2());
            ASSERT(IS_NULL == A[3].isNul2());

            mA.appendItems(2);
            ASSERT(6 == A.length());
            ASSERT(compareCERefs(VA, A[0].asElemRef()));
            ASSERT(compareCERefs(VB, A[2].asElemRef()));
            ASSERT(IS_NULL == A[1].isNul2());
            ASSERT(IS_NULL == A[3].isNul2());

            if (hasDefaultValue) {
                ASSERT(compareCERefs(RECORD->field(0).defaultValue(),
                                     A[4].asElemRef()));
                ASSERT(compareCERefs(RECORD->field(0).defaultValue(),
                                     A[5].asElemRef()));
            }
            else {
                ASSERT(isUnset(A[4].asElemRef()));
                ASSERT(isUnset(A[5].asElemRef()));
            }
            mA.setField(4, VA);
            mA.setField(5, VB);
            ASSERT(compareCERefs(VA, A[4].asElemRef()));
            ASSERT(compareCERefs(VB, A[5].asElemRef()));

            mA.appendNullItems(2);
            ASSERT(8 == A.length());
            ASSERT(compareCERefs(VA, A[0].asElemRef()));
            ASSERT(compareCERefs(VB, A[2].asElemRef()));
            ASSERT(compareCERefs(VA, A[4].asElemRef()));
            ASSERT(compareCERefs(VB, A[5].asElemRef()));
            ASSERT(IS_NULL == A[1].isNul2());
            ASSERT(IS_NULL == A[3].isNul2());
            ASSERT(IS_NULL == A[6].isNul2());
            ASSERT(IS_NULL == A[7].isNul2());

            mA.removeAllItems();
            ASSERT(0 == A.length());

#ifndef BSLS_PLATFORM__CMP_MSVC
          } END_BSLMA_EXCEPTION_TEST
#endif
        }
}

static void testCase17(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // TESTING 'insertItems' and 'removeItems' FUNCTION:
        //
        // Concerns:
        //   - Insertion and removal inserts or removes the correct number of
        //     elements at the correct index
        //   - No memory is leaked, even in the presence of exceptions
        //
        // Plan:
        //   - Both insertItems and removeItems call insert and remove
        //     functions if the type is a table or choice array and call the
        //     insert or remove function on the vector for the other array
        //     types.  Thus we just need to confirm that these functions pass
        //     the arguments correctly to the underlying functions and handle
        //     the boundary conditions correctly.
        //
        // Testing:
        //   void insertItems(int pos, int numItems) const;
        //   void insertNullItems(int pos, int numItems) const;
        //   void removeItems(int pos, int numItems) const;
        // --------------------------------------------------------------------

        if (verbose) tst::cout << "\nTESTING 'insertItems' and 'removeItems'"
                               << " FUNCTION"
                               << "\n======================================="
                               << "========="
                               << bsl::endl;

        if (veryVerbose) { tst::cout << "\n\tTesting scalar arrays and "
                                     << "constrained table and choice array"
                                     << bsl::endl; }
        {
            const struct {
                int         d_line;
                const char *d_spec;
            } DATA[] = {
                // Line     Spec
                // ----     ----
                {   L_,   ":aKa" },
                {   L_,   ":aKa&D0" },
                {   L_,   ":aKa&NT" },
                {   L_,   ":aKa&NF" },
                {   L_,   ":aKa&FN" },
                {   L_,   ":aLa" },
                {   L_,   ":aLa&D0" },
                {   L_,   ":aLa&NT" },
                {   L_,   ":aLa&NF" },
                {   L_,   ":aLa&FN" },
                {   L_,   ":aMa" },
                {   L_,   ":aMa&D0" },
                {   L_,   ":aMa&NT" },
                {   L_,   ":aMa&NF" },
                {   L_,   ":aMa&FN" },
                {   L_,   ":aNa" },
                {   L_,   ":aNa&D0" },
                {   L_,   ":aNa&NT" },
                {   L_,   ":aNa&NF" },
                {   L_,   ":aNa&FN" },
                {   L_,   ":aOa" },
                {   L_,   ":aOa&D0" },
                {   L_,   ":aOa&NT" },
                {   L_,   ":aOa&NF" },
                {   L_,   ":aOa&FN" },
                {   L_,   ":aPa" },
                {   L_,   ":aPa&D0" },
                {   L_,   ":aPa&NT" },
                {   L_,   ":aPa&NF" },
                {   L_,   ":aPa&FN" },
                {   L_,   ":aQa" },
                {   L_,   ":aQa&D0" },
                {   L_,   ":aQa&NT" },
                {   L_,   ":aQa&NF" },
                {   L_,   ":aQa&FN" },
                {   L_,   ":aRa" },
                {   L_,   ":aRa&D0" },
                {   L_,   ":aRa&NT" },
                {   L_,   ":aRa&NF" },
                {   L_,   ":aRa&FN" },
                {   L_,   ":aSa" },
                {   L_,   ":aSa&D0" },
                {   L_,   ":aSa&NT" },
                {   L_,   ":aSa&NF" },
                {   L_,   ":aSa&FN" },
                {   L_,   ":aTa" },
                {   L_,   ":aTa&D0" },
                {   L_,   ":aTa&NT" },
                {   L_,   ":aTa&NF" },
                {   L_,   ":aTa&FN" },
                {   L_,   ":aaa" },
                {   L_,   ":aaa&D0" },
                {   L_,   ":aaa&NT" },
                {   L_,   ":aaa&NF" },
                {   L_,   ":aaa&FN" },
                {   L_,   ":aba" },
                {   L_,   ":aba&D0" },
                {   L_,   ":aba&NT" },
                {   L_,   ":aba&NF" },
                {   L_,   ":aba&FN" },
                {   L_,   ":aca" },
                {   L_,   ":aca&D0" },
                {   L_,   ":aca&NT" },
                {   L_,   ":aca&NF" },
                {   L_,   ":aca&FN" },
                {   L_,   ":ada" },
                {   L_,   ":ada&D0" },
                {   L_,   ":ada&NT" },
                {   L_,   ":ada&NF" },
                {   L_,   ":ada&FN" },

                {   L_,   ":aCbFcGdQf :g#ha" },
                {   L_,   ":aCbFcGdQf :g#ha&NT" },
                {   L_,   ":aCbFcGdQf :g#ha&NF" },
                {   L_,   ":a?CbFcGdQf :g@ha" },
                {   L_,   ":a?CbFcGdQf :g@ha&NT" },
                {   L_,   ":a?CbFcGdQf :g@ha&NF" },

                // TBD: Add enumerations
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const char *SPEC = DATA[i].d_spec;
                const bool  NSA  = (bool) bsl::strstr(SPEC, "&FN");

                Schema schema; const Schema& SCHEMA = schema;
                ggSchema(&schema, SPEC);
                const RecDef *RECORD = NSA
                                     ? &SCHEMA.record(0)
                                     : &SCHEMA.record(SCHEMA.numRecords() - 1);

                ConstRecDefShdPtr crp(RECORD, NilDeleter(), 0);
                const ConstRecDefShdPtr& CRP = crp;

                const char *fldName       = RECORD->fieldName(0);
                const bool  hasDefaultValue =
                                            RECORD->field(0).hasDefaultValue();

                ET::Type ITEM_TYPE, ARRAY_TYPE;
                bool IS_NULL;
                if (NSA) {
                    ITEM_TYPE  = SCHEMA.record(1).field(0).elemType();
                    ARRAY_TYPE = ET::toArrayType(ITEM_TYPE);
                    IS_NULL    = true;
                }
                else {
                    ARRAY_TYPE = RECORD->field(0).elemType();
                    ITEM_TYPE  = ET::fromArrayType(ARRAY_TYPE);
                    IS_NULL    = false;
                }

                if (RecDef::BDEM_CHOICE_RECORD ==
                                               SCHEMA.record(0).recordType()) {
                    IS_NULL = true;
                }

                bdem_List theList;  const bdem_List& THE_LIST = theList;
                if (ET::isAggregateType(ITEM_TYPE)) {
                    // Create a special record containing a non-array field
                    // of the item type.  'theList[0]' will contain a
                    // deep-initialized object of type 'ITEM_TYPE'.
                    RecDef *flatRecord = schema.createRecord();
                    flatRecord->appendField(ITEM_TYPE,
                                          RECORD->field(0).recordConstraint());
                    SchemaAggUtil::initListDeep(&theList, *flatRecord);
                }

                const CERef VA = getCERef(ITEM_TYPE, 1);
                const CERef VB = getCERef(ITEM_TYPE, 2);

                if (veryVerbose) { T_ P_(SPEC) P(SCHEMA) };

                bslma_TestAllocator testAllocator(veryVeryVerbose);

#ifndef BSLS_PLATFORM__CMP_MSVC
              BEGIN_BSLMA_EXCEPTION_TEST {
#endif
                Obj mX(CRP, &testAllocator); const Obj& X = mX;

                Obj mA = X.field(fldName); const Obj& A = mA;

                mA.insertItems(0, 0);
                ASSERT(0 == A.length());

                mA.insertNullItems(0, 0);
                ASSERT(0 == A.length());

                mA.insertItems(0, 1);
                ASSERT(1 == A.length());
                if (hasDefaultValue) {
                    ASSERT(compareCERefs(RECORD->field(0).defaultValue(),
                                         A[0].asElemRef()));
                }
                else {
                    ASSERT(isUnset(A[0].asElemRef()));
                }

                mA.setItem(0, VA);
                ASSERT(compareCERefs(VA, A[0].asElemRef()));

                mA.insertNullItems(0, 1);
                ASSERT(2 == A.length());
                LOOP2_ASSERT(LINE, IS_NULL, IS_NULL == A[0].isNul2());

                ASSERT(compareCERefs(VA, A[1].asElemRef()));
                mA.setItem(0, VB);
                mA.setItem(1, VA);
                ASSERT(compareCERefs(VB, A[0].asElemRef()));
                ASSERT(compareCERefs(VA, A[1].asElemRef()));
                mA.setItemNull(0);
                ASSERT(IS_NULL == A[0].isNul2());

                mA.insertItems(1, 2);
                ASSERT(4 == A.length());
                ASSERT(IS_NULL == A[0].isNul2());
                if (hasDefaultValue) {
                    ASSERT(compareCERefs(RECORD->field(0).defaultValue(),
                                         A[1].asElemRef()));
                    ASSERT(compareCERefs(RECORD->field(0).defaultValue(),
                                         A[2].asElemRef()));
                }
                else {
                    ASSERT(isUnset(A[1].asElemRef()));
                    ASSERT(isUnset(A[2].asElemRef()));
                }

                ASSERT(compareCERefs(VA, A[3].asElemRef()));
                mA.setItem(1, VA);
                mA.setItem(2, VB);

                mA.insertNullItems(1, 2);
                ASSERT(6 == A.length());
                ASSERT(IS_NULL == A[0].isNul2());
                ASSERT(IS_NULL == A[1].isNul2());
                ASSERT(IS_NULL == A[2].isNul2());
                ASSERT(compareCERefs(VA, A[3].asElemRef()));
                ASSERT(compareCERefs(VB, A[4].asElemRef()));
                ASSERT(compareCERefs(VA, A[5].asElemRef()));

                mA.removeItems(3, 1);
                ASSERT(5 == A.length());
                ASSERT(IS_NULL == A[0].isNul2());
                ASSERT(IS_NULL == A[1].isNul2());
                ASSERT(IS_NULL == A[2].isNul2());

                ASSERT(compareCERefs(VB, A[3].asElemRef()));
                ASSERT(compareCERefs(VA, A[4].asElemRef()));
                mA.setItem(0, VB);

                mA.removeItems(1, 4);
                ASSERT(1 == A.length());
                ASSERT(compareCERefs(VB, A[0].asElemRef()));

                mA.removeItems(0, 0);
                ASSERT(1 == A.length());
                ASSERT(compareCERefs(VB, A[0].asElemRef()));

                mA.removeItems(0, 1);
                ASSERT(0 == A.length());

#ifndef BSLS_PLATFORM__CMP_MSVC
              } END_BSLMA_EXCEPTION_TEST
#endif
          }
        }

        if (veryVerbose) { tst::cout << "\n\tTesting unconstrained table "
                                     << "and choice array" << bsl::endl; }
        {
            const struct {
                int         d_line;
                const char *d_spec;
            } DATA[] = {
                // Line     Spec
                // ----     ----
                {   L_,   ":aVa" },
                {   L_,   ":aVa&NT" },
                {   L_,   ":aVa&NF" },
                {   L_,   ":afa" },
                {   L_,   ":afa&NT" },
                {   L_,   ":afa&NF" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const char *SPEC = DATA[i].d_spec;

                Schema schema; const Schema& SCHEMA = schema;
                ggSchema(&schema, SPEC);
                const RecDef *RECORD = &SCHEMA.record(SCHEMA.numRecords() - 1);

                ConstRecDefShdPtr crp(RECORD, NilDeleter(), 0);
                const ConstRecDefShdPtr& CRP = crp;

                ET::Type ARRAY_TYPE = RECORD->field(0).elemType();
                const char *fldName = RECORD->fieldName(0);

                const CERef ARR_VA = getCERef(ARRAY_TYPE, 1);
                const int   LEN    = getLength(ARR_VA);

                const CERef VA = getCERef(ET::fromArrayType(ARRAY_TYPE), 1);
                const CERef VB = getCERef(ET::fromArrayType(ARRAY_TYPE), 2);

                if (veryVerbose) { T_ P_(SPEC) P(SCHEMA) };

                bslma_TestAllocator testAllocator(veryVeryVerbose);

#ifndef BSLS_PLATFORM__CMP_MSVC
              BEGIN_BSLMA_EXCEPTION_TEST {
#endif
                Obj mX(CRP, &testAllocator); const Obj& X = mX;

                mX.setField(fldName, ARR_VA);
                Obj mA = X.field(fldName); const Obj& A = mA;

                mA.insertItems(0, 0);
                ASSERT(LEN == A.length());

                mA.insertNullItems(0, 0);
                ASSERT(LEN == A.length());

                mA.insertItems(0, 1);
                ASSERT(LEN + 1 == A.length());

                mA.insertNullItems(0, 1);
                ASSERT(LEN + 2 == A.length());

                mA.insertItems(A.length(), 1);
                ASSERT(LEN + 3 == A.length());

                mA.insertNullItems(A.length(), 1);
                ASSERT(LEN + 4 == A.length());

                mA.insertItems(1, 2);
                ASSERT(LEN + 6 == A.length());

                mA.insertNullItems(1, 2);
                ASSERT(LEN + 8 == A.length());

                mA.removeItems(3, 1);
                ASSERT(LEN + 7 == A.length());

                mA.removeItems(1, 6);
                ASSERT(LEN + 1 == A.length());

                mA.removeItems(0, 0);
                ASSERT(LEN + 1 == A.length());

                mA.removeItems(0, 1);
                ASSERT(LEN + 0 == A.length());
#ifndef BSLS_PLATFORM__CMP_MSVC
            } END_BSLMA_EXCEPTION_TEST
#endif
          }
        }
}

static void testCase16(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // TESTING 'insert' and 'append' FUNCTIONS:
        //
        // Concerns:
        //   - That insert properly inserts a new element in the array at the
        //     correct index and that the new element is set to the correct
        //     value.
        //   - append inserts a new value to the end of the array
        //
        // Plan:
        //   - We will construct an aggregate mA from a set S of aggregates
        //     that store all types of array (scalar arrays, tables and choice
        //     arrays).  We will construct two element reference value VA and
        //     VB that will store two distinct values corresponding to the the
        //     base type of the type of mA.  We will then call insert on mA at
        //     various indices and confirm that the new value is inserted at
        //     the correct location and that it contains the expected value.
        //     We will use the 'length' function to confirm the length of the
        //     aggregate and the indexing operator to confirm the value.  We
        //     will then construct another object mB that is identical to mA.
        //     We will test that append works for that object.  Note that
        //     we will not test insert and append with values of different
        //     data types (and just use a bdem_ConstElemRef to store the
        //     value) as the insert function internally calls the private
        //     function setValue which has already been tested in a previous
        //     test case.  We just need to confirm that insert passes the
        //     arguments correctly to that function.
        //
        // Testing:
        //   template <typename VALTYPE>
        //   const bcem_Aggregate append(const VALTYPE& newItem) const;
        //   const bcem_Aggregate appendNull(const VALTYPE& newItem) const;
        //   template <typename VALTYPE>
        //   const bcem_Aggregate insert(int            pos,
        //                               const VALTYPE& newItem) const;
        //   const bcem_Aggregate insertNull(int pos) const;
        // --------------------------------------------------------------------

        if (verbose) tst::cout << "\nTESTING 'insert' and 'append' FUNCTIONS"
                               << "\n======================================="
                               << bsl::endl;

        if (veryVerbose) { tst::cout << "\n\tTesting scalar arrays and "
                                     << "constrained table and choice array"
                                     << bsl::endl; }
        {
            const struct {
                int         d_line;
                const char *d_spec;
            } DATA[] = {
                // Line     Spec
                // ----     ----
                {   L_,   ":aKa" },
                {   L_,   ":aKa&D0" },
                {   L_,   ":aKa&NT" },
                {   L_,   ":aKa&NF" },
                {   L_,   ":aKa&FN" },
                {   L_,   ":aLa" },
                {   L_,   ":aLa&D0" },
                {   L_,   ":aLa&NT" },
                {   L_,   ":aLa&NF" },
                {   L_,   ":aLa&FN" },
                {   L_,   ":aMa" },
                {   L_,   ":aMa&D0" },
                {   L_,   ":aMa&NT" },
                {   L_,   ":aMa&NF" },
                {   L_,   ":aMa&FN" },
                {   L_,   ":aNa" },
                {   L_,   ":aNa&D0" },
                {   L_,   ":aNa&NT" },
                {   L_,   ":aNa&NF" },
                {   L_,   ":aNa&FN" },
                {   L_,   ":aOa" },
                {   L_,   ":aOa&D0" },
                {   L_,   ":aOa&NT" },
                {   L_,   ":aOa&NF" },
                {   L_,   ":aOa&FN" },
                {   L_,   ":aPa" },
                {   L_,   ":aPa&D0" },
                {   L_,   ":aPa&NT" },
                {   L_,   ":aPa&NF" },
                {   L_,   ":aPa&FN" },
                {   L_,   ":aQa" },
                {   L_,   ":aQa&D0" },
                {   L_,   ":aQa&NT" },
                {   L_,   ":aQa&NF" },
                {   L_,   ":aQa&FN" },
                {   L_,   ":aRa" },
                {   L_,   ":aRa&D0" },
                {   L_,   ":aRa&NT" },
                {   L_,   ":aRa&NF" },
                {   L_,   ":aRa&FN" },
                {   L_,   ":aSa" },
                {   L_,   ":aSa&D0" },
                {   L_,   ":aSa&NT" },
                {   L_,   ":aSa&NF" },
                {   L_,   ":aSa&FN" },
                {   L_,   ":aTa" },
                {   L_,   ":aTa&D0" },
                {   L_,   ":aTa&NT" },
                {   L_,   ":aTa&NF" },
                {   L_,   ":aTa&FN" },
                {   L_,   ":aaa" },
                {   L_,   ":aaa&D0" },
                {   L_,   ":aaa&NT" },
                {   L_,   ":aaa&NF" },
                {   L_,   ":aaa&FN" },
                {   L_,   ":aba" },
                {   L_,   ":aba&D0" },
                {   L_,   ":aba&NT" },
                {   L_,   ":aba&NF" },
                {   L_,   ":aba&FN" },
                {   L_,   ":aca" },
                {   L_,   ":aca&D0" },
                {   L_,   ":aca&NT" },
                {   L_,   ":aca&NF" },
                {   L_,   ":aca&FN" },
                {   L_,   ":ada" },
                {   L_,   ":ada&D0" },
                {   L_,   ":ada&NT" },
                {   L_,   ":ada&NF" },
                {   L_,   ":ada&FN" },

                {   L_,   ":aCbFcGdQf :g#ha" },
                {   L_,   ":aCbFcGdQf :g#ha&NT" },
                {   L_,   ":aCbFcGdQf :g#ha&NF" },
                {   L_,   ":a?CbFcGdQf :g@ha" },
                {   L_,   ":a?CbFcGdQf :g@ha&NT" },
                {   L_,   ":a?CbFcGdQf :g@ha&NF" },

                // TBD: Add enumerations
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const char *SPEC = DATA[i].d_spec;
                const bool  NSA  = (bool) bsl::strstr(SPEC, "&FN");

                Schema schema; const Schema& SCHEMA = schema;
                ggSchema(&schema, SPEC);
                const RecDef *RECORD = NSA
                                ? &SCHEMA.record(0)
                                : &SCHEMA.record(SCHEMA.numRecords() - 1);

                ConstRecDefShdPtr crp(RECORD, NilDeleter(), 0);
                const ConstRecDefShdPtr& CRP = crp;
                bool IS_NULL;
                ET::Type TYPE, ARRAY_TYPE;
                if (NSA) {
                    TYPE       = SCHEMA.record(1).field(0).elemType();
                    ARRAY_TYPE = ET::toArrayType(TYPE);
                    IS_NULL    = true;
                }
                else {
                    ARRAY_TYPE = RECORD->field(0).elemType();
                    TYPE       = ET::fromArrayType(ARRAY_TYPE);
                    IS_NULL    = false;
                }
                if (RecDef::BDEM_CHOICE_RECORD ==
                                               SCHEMA.record(0).recordType()) {
                    IS_NULL = true;
                }

                const char *fldName         = RECORD->fieldName(0);
                const bool  hasDefaultValue =
                                            RECORD->field(0).hasDefaultValue();

                const CERef VA = getCERef(TYPE, 1);
                const CERef VB = getCERef(TYPE, 2);

                if (veryVerbose) { T_ P_(SPEC) P(SCHEMA) };

                bslma_TestAllocator testAllocator;

#ifndef BSLS_PLATFORM__CMP_MSVC
              BEGIN_BSLMA_EXCEPTION_TEST {
#endif
                Obj mX(CRP, &testAllocator); const Obj& X = mX;
                Obj mY(CRP, &testAllocator); const Obj& Y = mY;

                Obj mA = X.field(fldName); const Obj& A = mA;
                Obj mB = Y.field(fldName); const Obj& B = mB;

                Obj ret = mA.insert(0, VA);
                LOOP2_ASSERT(A, A.length(), 1 == A.length());
                ASSERT(compareCERefs(VA, A[0].asElemRef()));
                ASSERT(Obj::areEquivalent(ret, A[0]));

                ret = mA.insertNull(0);
                ASSERT(2 == A.length());
                ASSERT(IS_NULL == A[0].isNul2());
                ASSERT(compareCERefs(VA, A[1].asElemRef()));
                ASSERT(Obj::areEquivalent(ret, A[0]));

                ret = mA.insert(0, VB);
                ASSERT(3 == A.length());
                ASSERT(compareCERefs(VB, A[0].asElemRef()));
                ASSERT(IS_NULL == A[1].isNul2());
                ASSERT(compareCERefs(VA, A[2].asElemRef()));
                ASSERT(Obj::areEquivalent(ret, A[0]));

                ret = mA.insert(3, VB);
                ASSERT(4 == A.length());
                ASSERT(compareCERefs(VB, A[0].asElemRef()));
                ASSERT(IS_NULL == A[1].isNul2());
                ASSERT(compareCERefs(VA, A[2].asElemRef()));
                ASSERT(compareCERefs(VB, A[3].asElemRef()));
                ASSERT(Obj::areEquivalent(ret, A[3]));

                ret = mA.insertNull(4);
                ASSERT(5 == A.length());
                ASSERT(compareCERefs(VB, A[0].asElemRef()));
                ASSERT(IS_NULL == A[1].isNul2());
                ASSERT(compareCERefs(VA, A[2].asElemRef()));
                ASSERT(compareCERefs(VB, A[3].asElemRef()));
                ASSERT(IS_NULL == A[4].isNul2());
                ASSERT(Obj::areEquivalent(ret, A[4]));

                ret = mA.insert(1, VB);
                ASSERT(6 == A.length());
                ASSERT(compareCERefs(VB, A[0].asElemRef()));
                ASSERT(compareCERefs(VB, A[1].asElemRef()));
                ASSERT(IS_NULL == A[2].isNul2());
                ASSERT(compareCERefs(VA, A[3].asElemRef()));
                ASSERT(compareCERefs(VB, A[4].asElemRef()));
                ASSERT(IS_NULL == A[5].isNul2());
                ASSERT(Obj::areEquivalent(ret, A[4]));

                ret = mA.insertNull(5);
                ASSERT(7 == A.length());
                ASSERT(compareCERefs(VB, A[0].asElemRef()));
                ASSERT(compareCERefs(VB, A[1].asElemRef()));
                ASSERT(IS_NULL == A[2].isNul2());
                ASSERT(compareCERefs(VA, A[3].asElemRef()));
                ASSERT(compareCERefs(VB, A[4].asElemRef()));
                ASSERT(IS_NULL == A[5].isNul2());
                ASSERT(IS_NULL == A[6].isNul2());
                ASSERT(Obj::areEquivalent(ret, A[5]));

                ret = mB.append(VA);
                ASSERT(1 == B.length());
                ASSERT(compareCERefs(VA, B[0].asElemRef()));
                ASSERT(Obj::areEquivalent(ret, B[0]));

                ret = mB.appendNull();
                ASSERT(2 == B.length());
                ASSERT(compareCERefs(VA, B[0].asElemRef()));
                ASSERT(IS_NULL == B[1].isNul2());
                ASSERT(Obj::areEquivalent(ret, B[1]));

                ret = mB.append(VA);
                ASSERT(3 == B.length());
                ASSERT(compareCERefs(VA, B[0].asElemRef()));
                ASSERT(IS_NULL == B[1].isNul2());
                ASSERT(compareCERefs(VA, B[2].asElemRef()));
                ASSERT(Obj::areEquivalent(ret, B[2]));

                ret = mB.appendNull();
                ASSERT(4 == B.length());
                ASSERT(compareCERefs(VA, B[0].asElemRef()));
                ASSERT(IS_NULL == B[1].isNul2());
                ASSERT(compareCERefs(VA, B[2].asElemRef()));
                ASSERT(IS_NULL == B[3].isNul2());
                ASSERT(Obj::areEquivalent(ret, B[3]));
#ifndef BSLS_PLATFORM__CMP_MSVC
              } END_BSLMA_EXCEPTION_TEST
#endif
            }
        }

        if (veryVerbose) { tst::cout << "\n\tTesting unconstrained table "
                                     << "and choice array" << bsl::endl; }
        {
            const struct {
                int         d_line;
                const char *d_spec;
            } DATA[] = {
                // Line     Spec
                // ----     ----
                {   L_,   ":aVa" },
                {   L_,   ":aVa&NT" },
                {   L_,   ":aVa&NF" },
                {   L_,   ":afa" },
                {   L_,   ":afa&NT" },
                {   L_,   ":afa&NF" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const char *SPEC = DATA[i].d_spec;

                Schema schema; const Schema& SCHEMA = schema;
                ggSchema(&schema, SPEC);
                const RecDef *RECORD = &SCHEMA.record(SCHEMA.numRecords() - 1);

                ConstRecDefShdPtr crp(RECORD, NilDeleter(), 0);
                const ConstRecDefShdPtr& CRP = crp;

                ET::Type ARRAY_TYPE = RECORD->field(0).elemType();
                const char *fldName = RECORD->fieldName(0);

                const CERef ARR_VA = getCERef(ARRAY_TYPE, 1);
                const int   LEN    = getLength(ARR_VA);

                const CERef VA = getCERef(ET::fromArrayType(ARRAY_TYPE), 1);
                const CERef VB = getCERef(ET::fromArrayType(ARRAY_TYPE), 2);

                // TBD: why ?
                const bool IS_NULL = ET::isChoiceType(ARRAY_TYPE)
                                   ? true : false;
                if (veryVerbose) { T_ P_(SPEC) P(SCHEMA) };

                bslma_TestAllocator testAllocator(veryVeryVerbose);

#ifndef BSLS_PLATFORM__CMP_MSVC
              BEGIN_BSLMA_EXCEPTION_TEST {
#endif
                Obj mX(CRP, &testAllocator); const Obj& X = mX;
                Obj mY(CRP, &testAllocator); const Obj& Y = mY;

                mX.setField(fldName, ARR_VA);
                mY.setField(fldName, ARR_VA);
                Obj mA = X.field(fldName); const Obj& A = mA;
                Obj mB = Y.field(fldName); const Obj& B = mB;

                Obj ret = mA.insert(0, VA);
                ASSERT(LEN + 1 == A.length());
                ASSERT(compareCERefs(VA, A[0].asElemRef()));
                ASSERT(Obj::areEquivalent(ret, A[0]));

                ret = mA.insertNull(0);
                ASSERT(LEN + 2 == A.length());
                LOOP_ASSERT(IS_NULL, IS_NULL == A[0].isNul2());
                ASSERT(compareCERefs(VA, A[1].asElemRef()));
                ASSERT(Obj::areEquivalent(ret, A[0]));

                ret = mA.insert(0, VB);
                ASSERT(LEN + 3 == A.length());
                ASSERT(compareCERefs(VB, A[0].asElemRef()));
                ASSERT(IS_NULL == A[1].isNul2());
                ASSERT(compareCERefs(VA, A[2].asElemRef()));
                ASSERT(Obj::areEquivalent(ret, A[0]));

                ret = mA.insert(1, VB);
                ASSERT(LEN + 4 == A.length());
                ASSERT(compareCERefs(VB, A[1].asElemRef()));
                ASSERT(Obj::areEquivalent(ret, A[1]));

                ret = mA.insertNull(1);
                ASSERT(LEN + 5 == A.length());
                ASSERT(IS_NULL == A[1].isNul2());
                ASSERT(compareCERefs(VB, A[2].asElemRef()));
                ASSERT(Obj::areEquivalent(ret, A[1]));

                ret = mB.append(VA);
                ASSERT(LEN + 1 == B.length());
                ASSERT(compareCERefs(VA, B[LEN].asElemRef()));
                ASSERT(Obj::areEquivalent(ret, B[LEN]));

                ret = mB.appendNull();
                ASSERT(LEN + 2 == B.length());
                ASSERT(compareCERefs(VA, B[LEN].asElemRef()));
                ASSERT(IS_NULL == B[LEN + 1].isNul2());
                ASSERT(Obj::areEquivalent(ret, B[LEN + 1]));

                ret = mB.append(VA);
                ASSERT(LEN + 3 == B.length());
                ASSERT(compareCERefs(VA, B[LEN].asElemRef()));
                ASSERT(IS_NULL == B[LEN + 1].isNul2());
                ASSERT(compareCERefs(VA, B[LEN + 2].asElemRef()));
                ASSERT(Obj::areEquivalent(ret, B[LEN + 2]));

                ret = mB.appendNull();
                ASSERT(LEN + 4 == B.length());
                ASSERT(compareCERefs(VA, B[LEN].asElemRef()));
                ASSERT(IS_NULL == B[LEN + 1].isNul2());
                ASSERT(compareCERefs(VA, B[LEN + 2].asElemRef()));
                ASSERT(IS_NULL == B[LEN + 3].isNul2());
                ASSERT(Obj::areEquivalent(ret, B[LEN + 3]));

#ifndef BSLS_PLATFORM__CMP_MSVC
              } END_BSLMA_EXCEPTION_TEST
#endif
            }
        }
}

static void testCase15(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // TESTING 'resize' FUNCTION:
        //
        // Concerns:
        //  - Resizing increases/decreases the size of the array to the
        //    specified size.
        //
        // Plan:
        //  - Resize just calls the resize function on the corresponding lower
        //    level component (table for table aggregates, choice array for
        //    choice array aggregates and on vector for the scalar array
        //    types).  So we just need to test that resize forwards the
        //    arguments correctly and handles the boundary conditions.
        //
        // Testing:
        //   void resize(int newSize) const;
        // --------------------------------------------------------------------

        if (verbose) tst::cout << "\nTESTING 'resize' FUNCTION"
                               << "\n========================="
                               << bsl::endl;
        const struct {
            int         d_line;
            const char *d_spec;
        } DATA[] = {
            // Line     Spec
            // ----     ----
            {   L_,   ":aKa" },
            {   L_,   ":aKa&D0" },
            {   L_,   ":aKa&NT" },
            {   L_,   ":aKa&NF" },
            {   L_,   ":aKa&FN" },
            {   L_,   ":aLa" },
            {   L_,   ":aLa&D0" },
            {   L_,   ":aLa&NT" },
            {   L_,   ":aLa&NF" },
            {   L_,   ":aLa&FN" },
            {   L_,   ":aMa" },
            {   L_,   ":aMa&D0" },
            {   L_,   ":aMa&NT" },
            {   L_,   ":aMa&NF" },
            {   L_,   ":aMa&FN" },
            {   L_,   ":aNa" },
            {   L_,   ":aNa&D0" },
            {   L_,   ":aNa&NT" },
            {   L_,   ":aNa&NF" },
            {   L_,   ":aNa&FN" },
            {   L_,   ":aOa" },
            {   L_,   ":aOa&D0" },
            {   L_,   ":aOa&NT" },
            {   L_,   ":aOa&NF" },
            {   L_,   ":aOa&FN" },
            {   L_,   ":aPa" },
            {   L_,   ":aPa&D0" },
            {   L_,   ":aPa&NT" },
            {   L_,   ":aPa&NF" },
            {   L_,   ":aPa&FN" },
            {   L_,   ":aQa" },
            {   L_,   ":aQa&D0" },
            {   L_,   ":aQa&NT" },
            {   L_,   ":aQa&NF" },
            {   L_,   ":aQa&FN" },
            {   L_,   ":aRa" },
            {   L_,   ":aRa&D0" },
            {   L_,   ":aRa&NT" },
            {   L_,   ":aRa&NF" },
            {   L_,   ":aRa&FN" },
            {   L_,   ":aSa" },
            {   L_,   ":aSa&D0" },
            {   L_,   ":aSa&NT" },
            {   L_,   ":aSa&NF" },
            {   L_,   ":aSa&FN" },
            {   L_,   ":aTa" },
            {   L_,   ":aTa&D0" },
            {   L_,   ":aTa&NT" },
            {   L_,   ":aTa&NF" },
            {   L_,   ":aTa&FN" },
            {   L_,   ":aVa" },
            {   L_,   ":aVa&NT" },
            {   L_,   ":aVa&NF" },
            {   L_,   ":afa" },
            {   L_,   ":afa&NT" },
            {   L_,   ":afa&NF" },
            {   L_,   ":aaa" },
            {   L_,   ":aaa&D0" },
            {   L_,   ":aaa&NT" },
            {   L_,   ":aaa&NF" },
            {   L_,   ":aaa&FN" },
            {   L_,   ":aba" },
            {   L_,   ":aba&D0" },
            {   L_,   ":aba&NT" },
            {   L_,   ":aba&NF" },
            {   L_,   ":aba&FN" },
            {   L_,   ":aca" },
            {   L_,   ":aca&D0" },
            {   L_,   ":aca&NT" },
            {   L_,   ":aca&NF" },
            {   L_,   ":aca&FN" },
            {   L_,   ":ada" },
            {   L_,   ":ada&D0" },
            {   L_,   ":ada&NT" },
            {   L_,   ":ada&NF" },
            {   L_,   ":ada&FN" },

            {   L_,   ":aCbFcGdQf :g#ha" },
            {   L_,   ":aCbFcGdQf :g#ha&NT" },
            {   L_,   ":aCbFcGdQf :g#ha&NF" },
            {   L_,   ":a?CbFcGdQf :g@ha" },
            {   L_,   ":a?CbFcGdQf :g@ha&NT" },
            {   L_,   ":a?CbFcGdQf :g@ha&NF" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE = DATA[i].d_line;
            const char *SPEC = DATA[i].d_spec;
            const bool  NSA  = (bool) bsl::strstr(SPEC, "&FN");

            Schema schema; const Schema& SCHEMA = schema;
            ggSchema(&schema, SPEC);
            const RecDef *RECORD = NSA
                                 ? &SCHEMA.record(0)
                                 : &SCHEMA.record(SCHEMA.numRecords() - 1);

            ConstRecDefShdPtr crp(RECORD, NilDeleter(), 0);
            const ConstRecDefShdPtr& CRP = crp;

            const char *fldName         = RECORD->fieldName(0);
            const bool  hasDefaultValue = RECORD->field(0).hasDefaultValue();

            if (veryVerbose) { T_ P_(SPEC) P(SCHEMA) };

            bslma_TestAllocator testAllocator(veryVeryVerbose);

          BEGIN_BSLMA_EXCEPTION_TEST {
            Obj mX(CRP, &testAllocator); const Obj& X = mX;

            Obj mA = X.field(fldName); const Obj& A = mA;

            mA.resize(0);
            ASSERT(0 == A.length());

            mA.resize(1);
            ASSERT(1 == A.length());
            if (hasDefaultValue) {
                LOOP_ASSERT(LINE,
                            compareCERefs(A[0].asElemRef(),
                                          RECORD->field(0).defaultValue()));
            }

            mA.resize(1);
            ASSERT(1 == A.length());
            if (hasDefaultValue) {
                LOOP_ASSERT(LINE,
                            compareCERefs(A[0].asElemRef(),
                                          RECORD->field(0).defaultValue()));
            }

            mA.resize(2);
            ASSERT(2 == A.length());
            if (hasDefaultValue) {
                LOOP_ASSERT(LINE,
                            compareCERefs(A[0].asElemRef(),
                                          RECORD->field(0).defaultValue()));
                LOOP_ASSERT(LINE,
                            compareCERefs(A[1].asElemRef(),
                                          RECORD->field(0).defaultValue()));
            }

            mA.resize(7);
            ASSERT(7 == A.length());

            mA.resize(3);
            ASSERT(3 == A.length());

            mA.resize(1);
            ASSERT(1 == A.length());
            if (hasDefaultValue) {
                LOOP_ASSERT(LINE,
                            compareCERefs(A[0].asElemRef(),
                                          RECORD->field(0).defaultValue()));
            }

            mA.resize(0);
            ASSERT(0 == A.length());

          } END_BSLMA_EXCEPTION_TEST
        }
}

static void testCase14(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // TESTING REMAINING ACCESSORS:
        //
        // Concerns:
        //   - That reset resets the aggregate to its default constructed
        //     state and releases all resources.
        //   - makeNull sets the aggregate to its null value
        //   - isNull returns the correct value
        //   - numSelections returns the correct number of selections
        //
        // Plan:
        //   - For reset function and null we will construct an aggregate
        //     object mX from a set S of aggregates.  We will confirm that an
        //     unassigned field in mX returns true for isNull.  We will then
        //     assign to that field and confirm that isNull will return false.
        //     We will then call reset on mX and confirm that it resembles an
        //     default constructed aggregate.  Similarly, construct an object
        //     mY equal to mX and confirm that calling makeNull sets the data
        //     to its null value.
        //   - Construct a aggregate mX (storing either a choice or choice
        //     array item aggregate).  Confirm that calling numSelections on
        //     mX returns the correct value.
        //
        // Testing:
        //   void reset();
        //   void makeNull();
        //   bool isNul2() const;
        //   int numSelections() const;
        // --------------------------------------------------------------------

        if (verbose) tst::cout << "\nTESTING REMAINING ACCESSORS"
                               << "\n===========================" << bsl::endl;

        if (veryVerbose) { tst::cout << "\n\tTesting reset & isNul2"
                                     << "\n\t======================"
                                     << bsl::endl; }
        {
            // For a null aggregate
            {
                Obj mX; const Obj& X = mX;
                Obj mY; const Obj& Y = mY;
                ASSERT(X.isNul2());
                ASSERT(Y.isNul2());
                mX.reset();
                ASSERT(ET::BDEM_VOID == X.dataType());
                ASSERT(ConstRecDefShdPtr() == X.recordDefPtr());
                ASSERT(VoidDataShdPtr() == X.dataPtr());
                ASSERT(X.isNul2());

                mY.reset();
                ASSERT(ET::BDEM_VOID == Y.dataType());
                ASSERT(ConstRecDefShdPtr() == Y.recordDefPtr());
                ASSERT(VoidDataShdPtr() == Y.dataPtr());
                ASSERT(Y.isNul2());
            }

            const struct {
                int         d_line;
                const char *d_spec;
            } DATA[] = {
                // Line         Spec
                // ----         ----
                // For List Aggregates
                {   L_,         ":aCa"    },
                {   L_,         ":aCa&NT" },
                {   L_,         ":aCa&NF" },
                {   L_,         ":aCa&D0" },
                {   L_,         ":aEa"    },
                {   L_,         ":aEa&NT" },
                {   L_,         ":aEa&NF" },
                {   L_,         ":aEa&D0" },
                {   L_,         ":aGa"    },
                {   L_,         ":aGa&NT" },
                {   L_,         ":aGa&NF" },
                {   L_,         ":aGa&D0" },
                {   L_,         ":aHa"    },
                {   L_,         ":aHa&NT" },
                {   L_,         ":aHa&NF" },
                {   L_,         ":aHa&D0" },
                {   L_,         ":aNa"    },
                {   L_,         ":aNa&NT" },
                {   L_,         ":aNa&NF" },
                {   L_,         ":aNa&D0" },
                {   L_,         ":aNa&FN" },
                {   L_,         ":aPa"    },
                {   L_,         ":aPa&NT" },
                {   L_,         ":aPa&NF" },
                {   L_,         ":aPa&D0" },
                {   L_,         ":aPa&FN" },
                {   L_,         ":aWa"    },
                {   L_,         ":aWa&NT" },
                {   L_,         ":aWa&NF" },
                {   L_,         ":aWa&D0" },
                {   L_,         ":aaa"    },
                {   L_,         ":aaa&NT" },
                {   L_,         ":aaa&NF" },
                {   L_,         ":aaa&D0" },
                {   L_,         ":aaa&FN" },
                {   L_,         ":aba"    },
                {   L_,         ":aba&NT" },
                {   L_,         ":aba&NF" },
                {   L_,         ":aba&D0" },
                {   L_,         ":aba&FN" },
                {   L_,         ":aUa" },
                {   L_,         ":aUa&NT" },
                {   L_,         ":aUa&NF" },
                {   L_,         ":aVa" },
                {   L_,         ":aVa&NT" },
                {   L_,         ":aVa&NF" },
                {   L_,         ":aea" },
                {   L_,         ":aea&NT" },
                {   L_,         ":aea&NF" },
                {   L_,         ":afa" },
                {   L_,         ":afa&NT" },
                {   L_,         ":afa&NF" },

                {   L_,         ":aAaFb" },
                {   L_,         ":aAaFb&NT" },
                {   L_,         ":aAaFb&NF" },

                {   L_,         ":aBaGbHc" },
                {   L_,         ":aBaGbHc&NT" },
                {   L_,         ":aBaGbHc&NF" },

                {   L_,         ":aNaPbQcRd" },
                {   L_,         ":aNaPbQcRd&NT" },
                {   L_,         ":aNaPbQcRd&NF" },

                {   L_,         ":aCbFcGdQf :g+ha" },
                {   L_,         ":aCbFcGdQf :g+ha&NT" },
                {   L_,         ":aCbFcGdQf :g+ha&NF" },

                {   L_,         ":aCbFcGdQf :g#ha" },
                {   L_,         ":aCbFcGdQf :g#ha&NT" },
                {   L_,         ":aCbFcGdQf :g#ha&NF" },

                {   L_,         ":a?CbFcGdQf :g%ha" },
                {   L_,         ":a?CbFcGdQf :g%ha&NT" },
                {   L_,         ":a?CbFcGdQf :g%ha&NF" },

                {   L_,         ":a?CbFcGdQf :g@ha" },
                {   L_,         ":a?CbFcGdQf :g@ha&NT" },
                {   L_,         ":a?CbFcGdQf :g@ha&NF" },

                // For Choice Aggregates
                {   L_,         ":a?Ca" },
                {   L_,         ":a?Ca&NT" },
                {   L_,         ":a?Ca&NF" },
                {   L_,         ":a?Ca&D0" },
                {   L_,         ":a?Ua" },
                {   L_,         ":a?Ua&NT" },
                {   L_,         ":a?Ua&NF" },
                {   L_,         ":a?Va" },
                {   L_,         ":a?Va&NT" },
                {   L_,         ":a?Va&NF" },
                {   L_,         ":a?ea" },
                {   L_,         ":a?ea&NT" },
                {   L_,         ":a?ea&NF" },
                {   L_,         ":a?fa" },
                {   L_,         ":a?fa&NT" },
                {   L_,         ":a?fa&NF" },

                {   L_,         ":a?AaFb" },
                {   L_,         ":a?AaFb&NT" },
                {   L_,         ":a?AaFb&NF" },

                {   L_,         ":a?BaGbHc" },
                {   L_,         ":a?BaGbHc&NT" },
                {   L_,         ":a?BaGbHc&NF" },

                {   L_,         ":a?NaPbQcRd" },
                {   L_,         ":a?NaPbQcRd&NT" },
                {   L_,         ":a?NaPbQcRd&NF" },

                {   L_,         ":aCbFcGdQf :g?+ha" },
                {   L_,         ":aCbFcGdQf :g?+ha&NT" },
                {   L_,         ":aCbFcGdQf :g?+ha&NF" },

                {   L_,         ":aCbFcGdQf :g?#ha" },
                {   L_,         ":aCbFcGdQf :g?#ha&NT" },
                {   L_,         ":aCbFcGdQf :g?#ha&NF" },

                {   L_,         ":a?CbFcGdQf :g?%ha" },
                {   L_,         ":a?CbFcGdQf :g?%ha&NT" },
                {   L_,         ":a?CbFcGdQf :g?%ha&NF" },

                {   L_,         ":a?CbFcGdQf :g?@ha" },
                {   L_,         ":a?CbFcGdQf :g?@ha&NT" },
                {   L_,         ":a?CbFcGdQf :g?@ha&NF" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const char *SPEC = DATA[i].d_spec;
                const bool  NSA  = (bool) bsl::strstr(SPEC, "&FN");

                Schema schema; const Schema& SCHEMA = schema;
                ggSchema(&schema, SPEC);
                const RecDef *RECORD = NSA
                                ? &SCHEMA.record(0)
                                : &SCHEMA.record(SCHEMA.numRecords() - 1);

                ConstRecDefShdPtr crp(RECORD, NilDeleter(), 0);
                const ConstRecDefShdPtr& CRP = crp;

                ET::Type TYPE = NSA
                        ? ET::toArrayType(SCHEMA.record(1).field(0).elemType())
                        : RECORD->field(0).elemType();
                const bool  DEF_VAL = RECORD->field(0).hasDefaultValue();
                bool        IS_NULL;
                if (DEF_VAL) {
                    IS_NULL = ET::isArrayType(TYPE)
                          || RecDef::BDEM_CHOICE_RECORD == RECORD->recordType()
                            ? true : false;
                }
                else if (ET::isAggregateType(TYPE)) {
                    if (1 == SCHEMA.numRecords()) {
                        IS_NULL = true;
                    }
                    else if (ET::isArrayType(TYPE)) {
                        IS_NULL =
                            RecDef::BDEM_CHOICE_RECORD == RECORD->recordType()
                            ? true : false;
                    }
                    else {
                        IS_NULL = std::strstr(SPEC, "&NT")
                          || RecDef::BDEM_CHOICE_RECORD == RECORD->recordType()
                                ? true : false;
                    }
                }
                else if (ET::isArrayType(TYPE)) {
                    IS_NULL = NSA ? false : true;
                }
                else {
                    IS_NULL = true;
                }

                const char *fldName = RECORD->fieldName(0);

                const CERef VA = getCERef(TYPE, 1);
                const CERef VN = getCERef(TYPE, 0);

                bslma_TestAllocator t(veryVeryVerbose);
                Obj mX(CRP, &t); const Obj& X = mX;
                Obj mY(CRP, &t); const Obj& Y = mY;

                if (veryVerbose) { T_ P_(SPEC) P(SCHEMA) P(X) };

// TBD a new failure following merge from bsl_hybrid_stl
#if 0
                LOOP2_ASSERT(LINE, IS_NULL,
                             IS_NULL != X.field(fldName).isNul2());
#endif

                if (RecDef::BDEM_CHOICE_RECORD == RECORD->recordType()) {
                    mX.makeSelection(fldName);
                    mY.makeSelection(fldName);
                }

//                 LOOP2_ASSERT(LINE, IS_NULL,
//                              IS_NULL == X.field(fldName).isNul2());

                if (!ET::isAggregateType(TYPE)
                 && RecDef::BDEM_CHOICE_RECORD != RECORD->recordType()) {
                    LOOP3_ASSERT(LINE, X, IS_NULL,
                                 IS_NULL == X.field(fldName).isNul2());
                    LOOP_ASSERT(LINE, IS_NULL == Y.field(fldName).isNul2());
                }

                if (!ET::isAggregateType(TYPE)) {
                    mX.setField(fldName, VN);
                    LOOP2_ASSERT(LINE, X, !X.field(fldName).isNul2());
                }

                mX.setField(fldName, VA);
                ASSERT(!X.field(fldName).isNul2());

                mY.setFieldNull(fldName);
                ASSERT(Y.field(fldName).isNul2());

                mX.reset();
                ASSERT(ET::BDEM_VOID == X.dataType());
                ASSERT(ConstRecDefShdPtr() == X.recordDefPtr());
                ASSERT(VoidDataShdPtr() == X.dataPtr());
                ASSERT(X.isNul2());

                mY.reset();
                ASSERT(ET::BDEM_VOID == Y.dataType());
                ASSERT(ConstRecDefShdPtr() == Y.recordDefPtr());
                ASSERT(VoidDataShdPtr() == Y.dataPtr());
                ASSERT(Y.isNul2());
            }
        }

        if (veryVerbose) { tst::cout << "\n\tTesting numSelections"
                                     << "\n\t====================="
                                     << bsl::endl; }
        {
            const struct {
                int         d_line;
                const char *d_spec;
            } DATA[] = {
                // Line            Spec
                // ----            ----
                // For choice aggregates
                {   L_,         ":a?" },
                {   L_,         ":a?Ca" },
                {   L_,         ":a?AaFb" },
                {   L_,         ":a?BaGbHc" },
                {   L_,         ":a?NaPbQcRd" },

                // For choice array item aggregates
                {   L_,         ":a?Ca :b@ba" },
                {   L_,         ":a?AaFb :b@ba" },
                {   L_,         ":a?BaGbHc :b@ba" },
                {   L_,         ":a?NaPbQcRd :b@ba" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const char *SPEC = DATA[i].d_spec;

                Schema schema; const Schema& SCHEMA = schema;
                ggSchema(&schema, SPEC);
                const RecDef *REC = &SCHEMA.record(SCHEMA.numRecords() - 1);

                ConstRecDefShdPtr crp(REC, NilDeleter(), 0);
                const ConstRecDefShdPtr& CRP = crp;

                if (veryVerbose) { T_ P_(SPEC) P(SCHEMA) };

                bslma_TestAllocator t(veryVeryVerbose);
                Obj mX(CRP, &t); const Obj& X = mX;
                if (2 == SCHEMA.numRecords()) {
                    X.field(REC->fieldName(0)).appendItems(1);
                    ASSERT(SCHEMA.record(0).numFields() ==
                                X.field(REC->fieldName(0), 0).numSelections());
                }
                else {
                    ASSERT(REC->numFields() == X.numSelections());
                }
            }

            // Test that error message is printed if data type does not match
            {
                const CERef CEA = getCERef(ET::BDEM_DOUBLE, 1);
                bsl::string sa; const bsl::string& SA = sa;

                Obj mX(ET::BDEM_DOUBLE, CEA);
                ASSERT(ErrorCode::BCEM_NOT_A_CHOICE == mX.numSelections());
            }
        }

        if (veryVerbose) { tst::cout << "\n\tTesting makeNull"
                                     << "\n\t================" << bsl::endl; }
        {
            const struct {
                int         d_line;
                const char *d_spec;
            } DATA[] = {
                // Line         Spec
                // ----         ----
                // For List Aggregates
//                 {   L_,         ":aCa"    },
//                 {   L_,         ":aCa&NT" },
//                 {   L_,         ":aCa&NF" },
//                 {   L_,         ":aCa&D0" },
//                 {   L_,         ":aEa"    },
//                 {   L_,         ":aEa&NT" },
//                 {   L_,         ":aEa&NF" },
//                 {   L_,         ":aEa&D0" },
//                 {   L_,         ":aGa"    },
//                 {   L_,         ":aGa&NT" },
//                 {   L_,         ":aGa&NF" },
//                 {   L_,         ":aGa&D0" },
//                 {   L_,         ":aHa"    },
//                 {   L_,         ":aHa&NT" },
//                 {   L_,         ":aHa&NF" },
//                 {   L_,         ":aHa&D0" },
//                 {   L_,         ":aNa"    },
//                 {   L_,         ":aNa&NT" },
//                 {   L_,         ":aNa&NF" },
//                 {   L_,         ":aNa&D0" },
                {   L_,         ":aNa&FN" },
                {   L_,         ":aPa"    },
                {   L_,         ":aPa&NT" },
                {   L_,         ":aPa&NF" },
                {   L_,         ":aPa&D0" },
                {   L_,         ":aPa&FN" },
                {   L_,         ":aWa"    },
                {   L_,         ":aWa&NT" },
                {   L_,         ":aWa&NF" },
                {   L_,         ":aWa&D0" },
                {   L_,         ":aaa"    },
                {   L_,         ":aaa&NT" },
                {   L_,         ":aaa&NF" },
                {   L_,         ":aaa&D0" },
                {   L_,         ":aaa&FN" },
                {   L_,         ":aba"    },
                {   L_,         ":aba&NT" },
                {   L_,         ":aba&NF" },
                {   L_,         ":aba&D0" },
                {   L_,         ":aba&FN" },
                {   L_,         ":aUa" },
                {   L_,         ":aUa&NT" },
                {   L_,         ":aUa&NF" },
                {   L_,         ":aVa" },
                {   L_,         ":aVa&NT" },
                {   L_,         ":aVa&NF" },
                {   L_,         ":aea" },
                {   L_,         ":aea&NT" },
                {   L_,         ":aea&NF" },
                {   L_,         ":afa" },
                {   L_,         ":afa&NT" },
                {   L_,         ":afa&NF" },

                {   L_,         ":aAaFb" },
                {   L_,         ":aAaFb&NT" },
                {   L_,         ":aAaFb&NF" },

                {   L_,         ":aBaGbHc" },
                {   L_,         ":aBaGbHc&NT" },
                {   L_,         ":aBaGbHc&NF" },

                {   L_,         ":aNaPbQcRd" },
                {   L_,         ":aNaPbQcRd&NT" },
                {   L_,         ":aNaPbQcRd&NF" },

                {   L_,         ":aCbFcGdQf :g+ha" },
                {   L_,         ":aCbFcGdQf :g+ha&NT" },
                {   L_,         ":aCbFcGdQf :g+ha&NF" },

                {   L_,         ":aCbFcGdQf :g#ha" },
                {   L_,         ":aCbFcGdQf :g#ha&NT" },
                {   L_,         ":aCbFcGdQf :g#ha&NF" },

                {   L_,         ":a?CbFcGdQf :g%ha" },
                {   L_,         ":a?CbFcGdQf :g%ha&NT" },
                {   L_,         ":a?CbFcGdQf :g%ha&NF" },

                {   L_,         ":a?CbFcGdQf :g@ha" },
                {   L_,         ":a?CbFcGdQf :g@ha&NT" },
                {   L_,         ":a?CbFcGdQf :g@ha&NF" },

                // For Choice Aggregates
                {   L_,         ":a?Ca" },
                {   L_,         ":a?Ca&NT" },
                {   L_,         ":a?Ca&NF" },
                {   L_,         ":a?Ca&D0" },
                {   L_,         ":a?Ua" },
                {   L_,         ":a?Ua&NT" },
                {   L_,         ":a?Ua&NF" },
                {   L_,         ":a?Va" },
                {   L_,         ":a?Va&NT" },
                {   L_,         ":a?Va&NF" },
                {   L_,         ":a?ea" },
                {   L_,         ":a?ea&NT" },
                {   L_,         ":a?ea&NF" },
                {   L_,         ":a?fa" },
                {   L_,         ":a?fa&NT" },
                {   L_,         ":a?fa&NF" },

                {   L_,         ":a?AaFb" },
                {   L_,         ":a?AaFb&NT" },
                {   L_,         ":a?AaFb&NF" },

                {   L_,         ":a?BaGbHc" },
                {   L_,         ":a?BaGbHc&NT" },
                {   L_,         ":a?BaGbHc&NF" },

                {   L_,         ":a?NaPbQcRd" },
                {   L_,         ":a?NaPbQcRd&NT" },
                {   L_,         ":a?NaPbQcRd&NF" },

                {   L_,         ":aCbFcGdQf :g?+ha" },
                {   L_,         ":aCbFcGdQf :g?+ha&NT" },
                {   L_,         ":aCbFcGdQf :g?+ha&NF" },

                {   L_,         ":aCbFcGdQf :g?#ha" },
                {   L_,         ":aCbFcGdQf :g?#ha&NT" },
                {   L_,         ":aCbFcGdQf :g?#ha&NF" },

                {   L_,         ":a?CbFcGdQf :g?%ha" },
                {   L_,         ":a?CbFcGdQf :g?%ha&NT" },
                {   L_,         ":a?CbFcGdQf :g?%ha&NF" },

                {   L_,         ":a?CbFcGdQf :g?@ha" },
                {   L_,         ":a?CbFcGdQf :g?@ha&NT" },
                {   L_,         ":a?CbFcGdQf :g?@ha&NF" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const char *SPEC = DATA[i].d_spec;
                const bool  NSA  = (bool) bsl::strstr(SPEC, "&FN");

                Schema schema; const Schema& SCHEMA = schema;
                ggSchema(&schema, SPEC);
                const RecDef *RECORD = NSA
                                ? &SCHEMA.record(0)
                                : &SCHEMA.record(SCHEMA.numRecords() - 1);

                ConstRecDefShdPtr crp(RECORD, NilDeleter(), 0);
                const ConstRecDefShdPtr& CRP = crp;

                ET::Type    TYPE = NSA
                        ? ET::toArrayType(SCHEMA.record(1).field(0).elemType())
                        : RECORD->field(0).elemType();
                const char *fldName = RECORD->fieldName(0);

                const CERef VA = getCERef(TYPE, 1);
                const CERef VB = getCERef(TYPE, 2);

                bslma_TestAllocator t(veryVeryVerbose);
                Obj mX(CRP, &t); const Obj& X = mX;
                Obj mY(CRP, &t); const Obj& Y = mY;
                if (RecDef::BDEM_CHOICE_RECORD == RECORD->recordType()) {
                    mX.makeSelection(fldName, VA);
                    mY.makeSelection(fldName, VA);
                } else {
                    mX.setField(fldName, VA);
                    mY.setField(fldName, VA);
                }

                if (veryVerbose) { T_ P_(SPEC) P(X) P(SCHEMA) };

                Obj mA = X.field(fldName); const Obj& A = mA;
                ASSERT(!A.isNul2());
                if (NSA) {
                    ASSERT(compareNillableTable(A, VA));
                }
                else {
                    ASSERT(VA == A.asElemRef());
                }

                mA.makeNull();
                ASSERT(A.isNul2());
                ASSERT(A.asElemRef().isNull());
                ASSERT(isUnset(A.asElemRef()));

                mX.setField(fldName, VB);
                ASSERT(!A.isNul2());

                if (NSA) {
                    ASSERT(compareNillableTable(A, VB));
                }
                else {
                    ASSERT(VB == A.asElemRef());
                }

                mX.makeNull();
                ASSERT(X.isNul2());
                ASSERT(X.asElemRef().isNull());
                ASSERT(isUnset(X.asElemRef()));

                mX = Y;
                ASSERT(!X.isNul2());
                ASSERT(Obj::areIdentical(X, Y));
            }
        }
}

static void testCase13(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // TESTING INDEXING OPERATOR and 'length' ACCESSORS:
        //
        // Concerns:
        //   - That the indexing operator returns the element at the specified
        //     index
        //   - length returns the length of the array aggregate
        //   - size returns the length of the array aggregate
        //
        // Plan:
        //   - Construct object mY from a set S of aggregates of all array
        //     types.  Assert that the initial length of mY is correct.  For
        //     each index confirm that the value at that index is as
        //     expected.
        //
        // Testing:
        //   bcem_Aggregate operator[](int index) const;
        //   int length() const;
        //   int size() const;
        // --------------------------------------------------------------------

        if (verbose) tst::cout << "\nTESTING INDEXING OPERATOR "
                               << "and 'length' ACCESSORS"
                               << "\n========================="
                               << "======================"
                               << bsl::endl;

        const struct {
            int         d_line;
            const char *d_spec;
        } DATA[] = {
            // Line     Spec
            // ----     ----
            {   L_,   ":aKa" },
            {   L_,   ":aKa&NT" },
            {   L_,   ":aKa&NF" },
            {   L_,   ":aKa&D0" },
            {   L_,   ":aKa&FN" },
            {   L_,   ":aLa" },
            {   L_,   ":aLa&NT" },
            {   L_,   ":aLa&NF" },
            {   L_,   ":aLa&D0" },
            {   L_,   ":aLa&FN" },
            {   L_,   ":aMa" },
            {   L_,   ":aMa&NT" },
            {   L_,   ":aMa&NF" },
            {   L_,   ":aMa&D0" },
            {   L_,   ":aMa&FN" },
            {   L_,   ":aNa" },
            {   L_,   ":aNa&NT" },
            {   L_,   ":aNa&NF" },
            {   L_,   ":aNa&D0" },
            {   L_,   ":aNa&FN" },
            {   L_,   ":aOa" },
            {   L_,   ":aOa&NT" },
            {   L_,   ":aOa&NF" },
            {   L_,   ":aOa&D0" },
            {   L_,   ":aOa&FN" },
            {   L_,   ":aPa" },
            {   L_,   ":aPa&NT" },
            {   L_,   ":aPa&NF" },
            {   L_,   ":aPa&D0" },
            {   L_,   ":aPa&FN" },
            {   L_,   ":aQa" },
            {   L_,   ":aQa&NT" },
            {   L_,   ":aQa&NF" },
            {   L_,   ":aQa&D0" },
            {   L_,   ":aQa&FN" },
            {   L_,   ":aRa" },
            {   L_,   ":aRa&NT" },
            {   L_,   ":aRa&NF" },
            {   L_,   ":aRa&D0" },
            {   L_,   ":aRa&FN" },
            {   L_,   ":aSa" },
            {   L_,   ":aSa&NT" },
            {   L_,   ":aSa&NF" },
            {   L_,   ":aSa&D0" },
            {   L_,   ":aSa&FN" },
            {   L_,   ":aTa" },
            {   L_,   ":aTa&NT" },
            {   L_,   ":aTa&NF" },
            {   L_,   ":aTa&D0" },
            {   L_,   ":aTa&FN" },
            // TBD:
//             {   L_,   ":aVa" },
//             {   L_,   ":aVa&NT" },
//             {   L_,   ":aVa&NF" },
            {   L_,   ":aaa" },
            {   L_,   ":aaa&NT" },
            {   L_,   ":aaa&NF" },
            {   L_,   ":aaa&D0" },
            {   L_,   ":aaa&FN" },
            {   L_,   ":aba" },
            {   L_,   ":aba&NT" },
            {   L_,   ":aba&NF" },
            {   L_,   ":aba&D0" },
            {   L_,   ":aba&FN" },
            {   L_,   ":aca" },
            {   L_,   ":aca&NT" },
            {   L_,   ":aca&NF" },
            {   L_,   ":aca&D0" },
            {   L_,   ":aca&FN" },
            {   L_,   ":ada" },
            {   L_,   ":ada&NT" },
            {   L_,   ":ada&NF" },
            {   L_,   ":ada&D0" },
            {   L_,   ":ada&FN" },
            // TBD:
//             {   L_,   ":afa" },
//             {   L_,   ":afa&NT" },
//             {   L_,   ":afa&NF" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE = DATA[i].d_line;
            const char *SPEC = DATA[i].d_spec;
            const bool  NSA  = (bool) bsl::strstr(SPEC, "&FN");

            Schema schema; const Schema& SCHEMA = schema;
            ggSchema(&schema, SPEC);
            const RecDef *RECORD = NSA
                                 ? &SCHEMA.record(0)
                                 : &SCHEMA.record(SCHEMA.numRecords() - 1);

            ConstRecDefShdPtr crp(RECORD, NilDeleter(), 0);
            const ConstRecDefShdPtr& CRP = crp;

            ET::Type TYPE, ARRAY_TYPE;
            if (NSA) {
                TYPE       = SCHEMA.record(1).field(0).elemType();
                ARRAY_TYPE = ET::toArrayType(TYPE);
            }
            else {
                ARRAY_TYPE = RECORD->field(0).elemType();
                TYPE       = ET::fromArrayType(ARRAY_TYPE);
            }

            const char *fldName = RECORD->fieldName(0);

            const CERef VA = getCERef(ARRAY_TYPE, 1);
            const CERef VB = getCERef(TYPE, 1);

            if (ET::BDEM_LIST == TYPE) {
                TYPE = ET::BDEM_ROW;
            }
            else if (ET::BDEM_CHOICE == TYPE) {
                TYPE = ET::BDEM_CHOICE_ARRAY_ITEM;
            }

            if (veryVerbose) { T_ P_(SPEC) P(SCHEMA) };

            Obj mX(CRP); const Obj& X = mX;

            mX.setField(fldName, VA);
            const int LEN = getLength(VA);

            Obj mY = X.field(fldName); const Obj& Y = mY;
            LOOP_ASSERT(ARRAY_TYPE, Y.length() == LEN);
            LOOP_ASSERT(ARRAY_TYPE, Y.size() == LEN);
            if (NSA) {
                LOOP_ASSERT(Y, ET::BDEM_TABLE == Y.asElemRef().type());
                LOOP_ASSERT(Y, ET::BDEM_TABLE == X.fieldType(fldName));
            }
            else {
                LOOP_ASSERT(Y, ARRAY_TYPE == Y.asElemRef().type());
                LOOP_ASSERT(Y, ARRAY_TYPE == X.fieldType(fldName));
            }

            for (int j = 0; j < LEN; ++j) {
                Obj mA = Y.field(j); const Obj& A = mA;
                Obj mB = Y[j]; const Obj& B = mB;

                if (veryVerbose) { P(X) P(Y) P(A) P(B) };

                LOOP2_ASSERT(ARRAY_TYPE, j, Obj::areEquivalent(A, B));
                LOOP2_ASSERT(ARRAY_TYPE, j, !Y[j].isNul2());
                LOOP2_ASSERT(ARRAY_TYPE, j, !A.isNul2());
                LOOP2_ASSERT(ARRAY_TYPE, j, !B.isNul2());
                LOOP4_ASSERT(A, j, TYPE, A.asElemRef().type(),
                             TYPE == A.asElemRef().type());
                LOOP2_ASSERT(B, j, TYPE == B.asElemRef().type());
                LOOP3_ASSERT(TYPE, j, Y.fieldType(j), TYPE == Y.fieldType(j));

                Y[j].makeNull();
                bool isNull = NSA ? true : false;
                LOOP3_ASSERT(ARRAY_TYPE, j, Y, isNull == Y[j].isNul2());
                LOOP2_ASSERT(ARRAY_TYPE, j, isNull == A.isNul2());
                LOOP2_ASSERT(ARRAY_TYPE, j, isNull == B.isNul2());
                LOOP2_ASSERT(ARRAY_TYPE, j, isUnset(Y[j].asElemRef()));
                LOOP2_ASSERT(ARRAY_TYPE, j, isUnset(A.asElemRef()));
                LOOP2_ASSERT(ARRAY_TYPE, j, isUnset(B.asElemRef()));
                LOOP2_ASSERT(A, j, TYPE == A.asElemRef().type());
                LOOP2_ASSERT(B, j, TYPE == B.asElemRef().type());
                LOOP2_ASSERT(TYPE, j, TYPE == Y.fieldType(j));

                Y[j].setValue(VB);
                LOOP2_ASSERT(ARRAY_TYPE, j, !Y[j].isNul2());
                LOOP2_ASSERT(ARRAY_TYPE, j, !A.isNul2());
                LOOP2_ASSERT(ARRAY_TYPE, j, !B.isNul2());
                LOOP2_ASSERT(ARRAY_TYPE, j, VB == Y[j].asElemRef());
                LOOP2_ASSERT(ARRAY_TYPE, j, VB == A.asElemRef());
                LOOP2_ASSERT(ARRAY_TYPE, j, VB == B.asElemRef());
                LOOP2_ASSERT(A, j, TYPE == A.asElemRef().type());
                LOOP2_ASSERT(B, j, TYPE == B.asElemRef().type());
                LOOP2_ASSERT(TYPE, j, TYPE == Y.fieldType(j));

                A.makeNull();
                LOOP2_ASSERT(ARRAY_TYPE, j, isNull == Y[j].isNul2());
                LOOP2_ASSERT(ARRAY_TYPE, j, isNull == A.isNul2());
                LOOP2_ASSERT(ARRAY_TYPE, j, isNull == B.isNul2());
                LOOP2_ASSERT(ARRAY_TYPE, j, isUnset(Y[j].asElemRef()));
                LOOP2_ASSERT(ARRAY_TYPE, j, isUnset(A.asElemRef()));
                LOOP2_ASSERT(ARRAY_TYPE, j, isUnset(B.asElemRef()));

                B.setValue(VB);
                LOOP2_ASSERT(ARRAY_TYPE, j, !Y[j].isNul2());
                LOOP2_ASSERT(ARRAY_TYPE, j, !A.isNul2());
                LOOP2_ASSERT(ARRAY_TYPE, j, !B.isNul2());
                LOOP2_ASSERT(ARRAY_TYPE, j, VB == Y[j].asElemRef());
                LOOP2_ASSERT(ARRAY_TYPE, j, VB == A.asElemRef());
                LOOP2_ASSERT(ARRAY_TYPE, j, VB == B.asElemRef());

                B.makeNull();
                LOOP2_ASSERT(ARRAY_TYPE, j, isNull == Y[j].isNul2());
                LOOP2_ASSERT(ARRAY_TYPE, j, isNull == A.isNul2());
                LOOP2_ASSERT(ARRAY_TYPE, j, isNull == B.isNul2());
                LOOP2_ASSERT(ARRAY_TYPE, j, isUnset(Y[j].asElemRef()));
                LOOP2_ASSERT(ARRAY_TYPE, j, isUnset(A.asElemRef()));
                LOOP2_ASSERT(ARRAY_TYPE, j, isUnset(B.asElemRef()));

                A.setValue(VB);
                LOOP2_ASSERT(ARRAY_TYPE, j, !Y[j].isNul2());
                LOOP2_ASSERT(ARRAY_TYPE, j, !A.isNul2());
                LOOP2_ASSERT(ARRAY_TYPE, j, !B.isNul2());
                LOOP2_ASSERT(ARRAY_TYPE, j, VB == Y[j].asElemRef());
                LOOP2_ASSERT(ARRAY_TYPE, j, VB == A.asElemRef());
                LOOP2_ASSERT(ARRAY_TYPE, j, VB == B.asElemRef());
            }
        }
}

static void testCase12(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR:
        //
        // Concerns:
        //   Any value must be assignable to an object having any initial value
        //   without affecting the rhs operand value.  Also, any object must be
        //   assignable to itself.  Assigning a value to an object with the
        //   same value type should use assignment, whereas assigning a value
        //   to an object with a different value type should use destruction
        //   and copy construction.
        //
        // Plan:
        //   Specify a set S of (unique) objects with substantial and varied
        //   differences in value.  Construct and initialize all combinations
        //   (mX, mZ) in the cross product S x S, copy construct a control mY
        //   from mX, assign mX to mZ and assert that mX == mZ and mY == mZ.
        //   Ensure that assignment does not copy the allocator to assigned
        //   object.  Use the bdema exception macros to ensure that the
        //   operation is exception safe and there are no memory leaks when
        //   the assigned object is constructed with various allocation
        //   strategies.  Additionally, test that assignment of a null object
        //   to a constructed object works.
        //
        // Testing:
        //   bdem_Aggregate& operator=(const bdem_Aggregate& rhs);
        // --------------------------------------------------------------------

        if (verbose) tst::cout << "\nTESTING ASSIGNMENT OPERATOR"
                               << "\n===========================" << bsl::endl;

        const struct {
            int         d_line;
            const char *d_spec;
        } DATA[] = {
            // Line         Spec
            // ----         ----
            // For List Aggregates
            {   L_,         ":aCa" },
            {   L_,         ":aCa&NF" },
            {   L_,         ":aCa&D0" },
            {   L_,         ":aFa" },
            {   L_,         ":aFa&NT" },
            {   L_,         ":aFa&NF" },
            {   L_,         ":aFa&D0" },
            {   L_,         ":aGa" },
            {   L_,         ":aGa&NT" },
            {   L_,         ":aGa&NF" },
            {   L_,         ":aGa&D0" },
            {   L_,         ":aHa" },
            {   L_,         ":aHa&NT" },
            {   L_,         ":aHa&NF" },
            {   L_,         ":aHa&D0" },
            {   L_,         ":aNa" },
            {   L_,         ":aNa&NT" },
            {   L_,         ":aNa&NF" },
            {   L_,         ":aNa&D0" },
            {   L_,         ":aNa&FN" },
            {   L_,         ":aPa" },
            {   L_,         ":aPa&NT" },
            {   L_,         ":aPa&NF" },
            {   L_,         ":aPa&D0" },
            {   L_,         ":aPa&FN" },
            {   L_,         ":aQa" },
            {   L_,         ":aQa&NT" },
            {   L_,         ":aQa&NF" },
            {   L_,         ":aQa&D0" },
            {   L_,         ":aQa&FN" },
            {   L_,         ":aRa" },
            {   L_,         ":aRa&NT" },
            {   L_,         ":aRa&NF" },
            {   L_,         ":aRa&D0" },
            {   L_,         ":aRa&FN" },
            {   L_,         ":aWa"    },
            {   L_,         ":aWa&NT" },
            {   L_,         ":aWa&NF" },
            {   L_,         ":aWa&D0" },
            {   L_,         ":aaa&FN" },
            {   L_,         ":aVa"    },
            {   L_,         ":aVa&NT" },
            {   L_,         ":aVa&NF" },
            {   L_,         ":afa"    },
            {   L_,         ":afa&NT" },
            {   L_,         ":afa&NF" },

            {   L_,         ":b=tu5v :a$ab" },
            {   L_,         ":b=tu5v :a$ab&NT" },
            {   L_,         ":b=tu5v :a$ab&NF" },

            {   L_,         ":b=tu5v :a^ab" },
            {   L_,         ":b=tu5v :a^ab&NT" },
            {   L_,         ":b=tu5v :a^ab&NF" },

            {   L_,         ":b=tu5v :a!ab" },
            {   L_,         ":b=tu5v :a!ab&NT" },
            {   L_,         ":b=tu5v :a!ab&NF" },
            {   L_,         ":b=tu5v :a!ab&FN" },

            {   L_,         ":b=tu5v :a/ab" },
            {   L_,         ":b=tu5v :a/ab&NT" },
            {   L_,         ":b=tu5v :a/ab&NF" },
            {   L_,         ":b=tu5v :a/ab&FN" },

            {   L_,         ":aCbFcGdQf :g+ha" },
            {   L_,         ":aCbFcGdQf :g+ha&NT" },
            {   L_,         ":aCbFcGdQf :g+ha&NF" },

            {   L_,         ":aCbFcGdQf :g#ha" },
            {   L_,         ":aCbFcGdQf :g#ha&NT" },
            {   L_,         ":aCbFcGdQf :g#ha&NF" },

            {   L_,         ":a?CbFcGdQf :g%ha" },
            {   L_,         ":a?CbFcGdQf :g%ha&NT" },
            {   L_,         ":a?CbFcGdQf :g%ha&NF" },

            {   L_,         ":a?CbFcGdQf :g@ha" },
            {   L_,         ":a?CbFcGdQf :g@ha&NT" },
            {   L_,         ":a?CbFcGdQf :g@ha&NF" },

            // For Choice Aggregates
            {   L_,         ":a?Ca" },
            {   L_,         ":a?Ca&NT" },
            {   L_,         ":a?Ca&NF" },
            {   L_,         ":a?Ca&D0" },
            {   L_,         ":a?Fa" },
            {   L_,         ":a?Fa&NT" },
            {   L_,         ":a?Fa&NF" },
            {   L_,         ":a?Fa&D0" },
            {   L_,         ":a?Ga" },
            {   L_,         ":a?Ga&NT" },
            {   L_,         ":a?Ga&NF" },
            {   L_,         ":a?Ga&D0" },
            {   L_,         ":a?Ha" },
            {   L_,         ":a?Ha&NT" },
            {   L_,         ":a?Ha&NF" },
            {   L_,         ":a?Ha&D0" },
            {   L_,         ":a?Na" },
            {   L_,         ":a?Na&NT" },
            {   L_,         ":a?Na&NF" },
            {   L_,         ":a?Na&D0" },
            {   L_,         ":a?Pa" },
            {   L_,         ":a?Pa&NT" },
            {   L_,         ":a?Pa&NF" },
            {   L_,         ":a?Pa&D0" },
            {   L_,         ":a?Qa" },
            {   L_,         ":a?Qa&NT" },
            {   L_,         ":a?Qa&NF" },
            {   L_,         ":a?Qa&D0" },
            {   L_,         ":a?Ra" },
            {   L_,         ":a?Ra&NT" },
            {   L_,         ":a?Ra&NF" },
            {   L_,         ":a?Ra&D0" },
            {   L_,         ":a?Wa" },
            {   L_,         ":a?Wa&NT" },
            {   L_,         ":a?Wa&NF" },
            {   L_,         ":a?Wa&D0" },
            {   L_,         ":a?Va" },
            {   L_,         ":a?Va&NT" },
            {   L_,         ":a?Va&NF" },
            {   L_,         ":a?fa" },
            {   L_,         ":a?fa&NT" },
            {   L_,         ":a?fa&NF" },

            {   L_,         ":aCbFcGdQf :g?+ha" },
            {   L_,         ":aCbFcGdQf :g?+ha&NT" },
            {   L_,         ":aCbFcGdQf :g?+ha&NF" },

            {   L_,         ":aCbFcGdQf :g?#ha" },
            {   L_,         ":aCbFcGdQf :g?#ha&NT" },
            {   L_,         ":aCbFcGdQf :g?#ha&NF" },

            {   L_,         ":a?CbFcGdQf :g?%ha" },
            {   L_,         ":a?CbFcGdQf :g?%ha&NT" },
            {   L_,         ":a?CbFcGdQf :g?%ha&NF" },

            {   L_,         ":a?CbFcGdQf :g?@ha" },
            {   L_,         ":a?CbFcGdQf :g?@ha&NT" },
            {   L_,         ":a?CbFcGdQf :g?@ha&NF" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE1 = DATA[i].d_line;
            const char *SPEC1 = DATA[i].d_spec;
            const bool  NSA1  = (bool) bsl::strstr(SPEC1, "&FN");

            Schema schema1; const Schema& SCHEMA1 = schema1;
            ggSchema(&schema1, SPEC1);
            const RecDef *RECORD1 = NSA1
                                 ? &SCHEMA1.record(0)
                                 : &SCHEMA1.record(SCHEMA1.numRecords() - 1);

            ConstRecDefShdPtr crp1(RECORD1, NilDeleter(), 0);
            const ConstRecDefShdPtr& CRP1 = crp1;

            ET::Type TYPE1 = NSA1
                       ? ET::toArrayType(SCHEMA1.record(1).field(0).elemType())
                       : RECORD1->field(0).elemType();
            const char *fldName1 = RECORD1->fieldName(0);

            const CERef A1 = getCERef(TYPE1, 1);
            const CERef A2 = getCERef(TYPE1, 2);

            if (veryVerbose) { T_ P_(SPEC1) P(SCHEMA1) };

            bslma_TestAllocator t1(veryVeryVerbose);
            Obj mX(CRP1, &t1); const Obj& X = mX;

            if (RecDef::BDEM_CHOICE_RECORD == RECORD1->recordType()) {
                mX.makeSelection(fldName1);
            }

            mX.setField(fldName1, A1);

            Obj mA = X.field(fldName1); const Obj& A = mA;

            for (int j = 0; j < NUM_DATA; ++j) {
                const int   LINE2 = DATA[j].d_line;
                const char *SPEC2 = DATA[j].d_spec;
                const bool  NSA2  = (bool) bsl::strstr(SPEC2, "&FN");

                Schema schema2; const Schema& SCHEMA2 = schema2;
                ggSchema(&schema2, SPEC2);
                const RecDef *RECORD2 = NSA2
                                   ? &SCHEMA2.record(0)
                                   : &SCHEMA2.record(SCHEMA2.numRecords() - 1);

                ConstRecDefShdPtr crp2(RECORD2, NilDeleter(), 0);
                const ConstRecDefShdPtr& CRP2 = crp2;

                ET::Type TYPE2 = NSA2
                       ? ET::toArrayType(SCHEMA2.record(1).field(0).elemType())
                       : RECORD2->field(0).elemType();
                const char *fldName2 = RECORD2->fieldName(0);

                const CERef B1 = getCERef(TYPE2, 1);
                const CERef B2 = getCERef(TYPE2, 2);

                if (veryVerbose) { T_ P_(SPEC2) P(SCHEMA2) };

                bslma_TestAllocator testAllocator(veryVeryVerbose);

                Obj mY(CRP2, &testAllocator); const Obj& Y = mY;

                if (RecDef::BDEM_CHOICE_RECORD == RECORD2->recordType()) {
                    mY.makeSelection(fldName2);
                }

                mY.setField(fldName2, B2);

                Obj mB = Y.field(fldName2); const Obj& B = mB;
                LOOP2_ASSERT(LINE1, LINE2, !Obj::areIdentical(A, B));

                if (veryVerbose) {
                    P(A) P(B)
                }

              BEGIN_BSLMA_EXCEPTION_TEST {
                mB = A;
                LOOP2_ASSERT(LINE1, LINE2, Obj::areIdentical(A, B));

                mB.setValue(A2);
                LOOP2_ASSERT(LINE1, LINE2, Obj::areIdentical(A, B));

                mB.makeNull();
                LOOP2_ASSERT(LINE1, LINE2, Obj::areIdentical(A, B));
              } END_BSLMA_EXCEPTION_TEST
            }
        }
}

static void testCase11(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // TESTING print() FUNCTION AND OUTPUT (<<) OPERATOR:
        //
        // Concerns:
        //   The print() function is correctly forwarded to the appropriate
        //   print function for the type stored in the aggregate object.  The
        //   '<<' operator prints the output on one line.
        //
        // Plan:
        //   This function just forwards to the
        //   'bdem_SchemaAggregateUtil::print' function.  So we can just test
        //   that this function forwards parameters correctly to that function
        //   with a small set of values and avoid exhaustive testing.
        //
        // Testing:
        //   bsl::ostream& print(bsl::ostream& stream,
        //                       int           level = 0,
        //                       int           spacesPerLevel = 4) const;
        //   ostream& operator<<(ostream&, const bcem_Aggregate&);
        // --------------------------------------------------------------------

        if (verbose) tst::cout << bsl::endl
                          << "\nTESTING OUTPUT FUNCTIONS" << bsl::endl
                          << "\n========================" << bsl::endl;

        if (veryVerbose) tst::cout << bsl::endl
                                   << "\n\tTest null aggregate" << bsl::endl;
        {
            Obj mX; const Obj& X = mX;
            bsl::ostringstream os1, os2;
            X.print(os1, 2, 4);
            os2 << X;
            ASSERT("        <null aggregate>\n" == os1.str());
            ASSERT("<null aggregate>" == os2.str());
        }

        if (veryVerbose) tst::cout << bsl::endl
                                   << "\n\tTest scalar values" << bsl::endl;
        {
            const struct {
                int      d_line;
                ET::Type d_type;
            } DATA[] = {
                // Line   Type
                // ----   ----
                {   L_,   ET::BDEM_INT },
                {   L_,   ET::BDEM_DOUBLE },
                {   L_,   ET::BDEM_STRING },
                {   L_,   ET::BDEM_DATETIME },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE = DATA[i].d_line;
                ET::Type  TYPE = DATA[i].d_type;

                const CERef CEA = getCERef(TYPE, 1);
                const CERef CEB = getCERef(TYPE, 2);

                bslma_TestAllocator t(veryVeryVerbose);
                Obj mA(TYPE, CEA, &t); const Obj& A = mA;
                Obj mB(TYPE, CEB, &t); const Obj& B = mB;

                bsl::ostringstream exp1, exp2, exp3, exp4, exp5;
                bsl::ostringstream exp6, exp7, exp8, exp9, exp10;
                SchemaAggUtil::print(exp1, CEA, 0, 2, 4);
                SchemaAggUtil::print(exp2, CEA, 0, 2, -4);
                SchemaAggUtil::print(exp3, CEA, 0, -2, 4);
                SchemaAggUtil::print(exp4, CEA, 0, -2, -4);
                SchemaAggUtil::print(exp5, CEA, 0, 0, -1);
                SchemaAggUtil::print(exp6, CEB, 0, 2, 4);
                SchemaAggUtil::print(exp7, CEB, 0, 2, -4);
                SchemaAggUtil::print(exp8, CEB, 0, -2, 4);
                SchemaAggUtil::print(exp9, CEB, 0, -2, -4);
                SchemaAggUtil::print(exp10, CEB, 0, 0, -1);

                bsl::ostringstream os1, os2, os3, os4, os5;
                bsl::ostringstream os6, os7, os8, os9, os10;
                A.print(os1, 2, 4);
                A.print(os2, 2, -4);
                A.print(os3, -2, 4);
                A.print(os4, -2, -4);
                os5 << A;
                B.print(os6, 2, 4);
                B.print(os7, 2, -4);
                B.print(os8, -2, 4);
                B.print(os9, -2, -4);
                os10 << B;

                LOOP3_ASSERT(TYPE, exp1.str(), os1.str(),
                             exp1.str() == os1.str());
                LOOP3_ASSERT(TYPE, exp2.str(), os2.str(),
                             exp2.str() == os2.str());
                LOOP3_ASSERT(TYPE, exp3.str(), os3.str(),
                             exp3.str() == os3.str());
                LOOP3_ASSERT(TYPE, exp4.str(), os4.str(),
                             exp4.str() == os4.str());
                LOOP3_ASSERT(TYPE, exp5.str(), os5.str(),
                             exp5.str() == os5.str());
                LOOP3_ASSERT(TYPE, exp6.str(), os6.str(),
                             exp6.str() == os6.str());
                LOOP3_ASSERT(TYPE, exp7.str(), os7.str(),
                             exp7.str() == os7.str());
                LOOP3_ASSERT(TYPE, exp8.str(), os8.str(),
                             exp8.str() == os8.str());
                LOOP3_ASSERT(TYPE, exp9.str(), os9.str(),
                             exp9.str() == os9.str());
                LOOP3_ASSERT(TYPE, exp10.str(), os10.str(),
                             exp10.str() == os10.str());
            }
        }

        if (veryVerbose) tst::cout << bsl::endl
                                   << "\n\tTest array values" << bsl::endl;
        {
            const struct {
                int         d_line;
                const char *d_spec;
            } DATA[] = {
                // Line  Spec
                // ----  ----
                {   L_,  ":aPa" },
                {   L_,  ":aQa" },
                {   L_,  ":aRa" },
                {   L_,  ":aWa" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const char *SPEC = DATA[i].d_spec;

                Schema s; ggSchema(&s, SPEC);
                const RecDef& r = s.record(s.numRecords() - 1);
                ConstRecDefShdPtr rp(&r, NilDeleter(), 0);

                if (veryVerbose) { T_ P(s) };

                bslma_TestAllocator t(veryVeryVerbose);
                Obj mX(rp, &t); const Obj& X = mX;
                Obj mY(rp, &t); const Obj& Y = mY;

                const char *fldName = r.fieldName(0);
                ET::Type    TYPE    = r.field(0).elemType();
                const CERef CEA     = getCERef(TYPE, 1);
                const CERef CEB     = getCERef(TYPE, 2);

                X.setField(fldName, CEA);
                Y.setField(fldName, CEB);

                if (veryVerbose) { T_ P(X) };
                Obj mA = X.field(fldName); const Obj& A = mA;
                Obj mB = Y.field(fldName); const Obj& B = mB;

                bsl::ostringstream exp1, exp2, exp3, exp4, exp5;
                bsl::ostringstream exp6, exp7, exp8, exp9, exp10;
                SchemaAggUtil::print(exp1, CEA, 0, 2, 4);
                SchemaAggUtil::print(exp2, CEA, 0, 2, -4);
                SchemaAggUtil::print(exp3, CEA, 0, -2, 4);
                SchemaAggUtil::print(exp4, CEA, 0, -2, -4);
                SchemaAggUtil::print(exp5, CEA, 0, 0, -1);
                SchemaAggUtil::print(exp6, CEB, 0, 2, 4);
                SchemaAggUtil::print(exp7, CEB, 0, 2, -4);
                SchemaAggUtil::print(exp8, CEB, 0, -2, 4);
                SchemaAggUtil::print(exp9, CEB, 0, -2, -4);
                SchemaAggUtil::print(exp10, CEB, 0, 0, -1);

                bsl::ostringstream os1, os2, os3, os4, os5;
                bsl::ostringstream os6, os7, os8, os9, os10;
                A.print(os1, 2, 4);
                A.print(os2, 2, -4);
                A.print(os3, -2, 4);
                A.print(os4, -2, -4);
                os5 << A;
                B.print(os6, 2, 4);
                B.print(os7, 2, -4);
                B.print(os8, -2, 4);
                B.print(os9, -2, -4);
                os10 << B;

                LOOP3_ASSERT(TYPE, exp1.str(), os1.str(),
                             exp1.str() == os1.str());
                LOOP3_ASSERT(TYPE, exp2.str(), os2.str(),
                             exp2.str() == os2.str());
                LOOP3_ASSERT(TYPE, exp3.str(), os3.str(),
                             exp3.str() == os3.str());
                LOOP3_ASSERT(TYPE, exp4.str(), os4.str(),
                             exp4.str() == os4.str());
                LOOP3_ASSERT(TYPE, exp5.str(), os5.str(),
                             exp5.str() == os5.str());
                LOOP3_ASSERT(TYPE, exp6.str(), os6.str(),
                             exp6.str() == os6.str());
                LOOP3_ASSERT(TYPE, exp7.str(), os7.str(),
                             exp7.str() == os7.str());
                LOOP3_ASSERT(TYPE, exp8.str(), os8.str(),
                             exp8.str() == os8.str());
                LOOP3_ASSERT(TYPE, exp9.str(), os9.str(),
                             exp9.str() == os9.str());
                LOOP3_ASSERT(TYPE, exp10.str(), os10.str(),
                             exp10.str() == os10.str());
            }
        }

        if (veryVerbose) tst::cout << bsl::endl
                                   << "\n\tTest aggregate values" << bsl::endl;
        {
            const struct {
                int         d_line;
                const char *d_spec;
            } DATA[] = {
                // Line  Spec
                // ----  ----
                // For List Aggregates
                {   L_,         ":aCa" },
                {   L_,         ":aFa" },
                {   L_,         ":aGa" },
                {   L_,         ":aHa" },
                {   L_,         ":aNa" },
                {   L_,         ":aPa" },
                {   L_,         ":aQa" },
                {   L_,         ":aRa" },
                {   L_,         ":aWa" },
                {   L_,         ":aCbFcGdQf :g+ha" },
                {   L_,         ":aCbFcGdQf :g#ha" },
                {   L_,         ":a?CbFcGdQf :g%ha" },
                {   L_,         ":a?CbFcGdQf :g@ha" },

                // For Choice Aggregates
                {   L_,         ":a?Ca" },
                {   L_,         ":a?Fa" },
                {   L_,         ":a?Ga" },
                {   L_,         ":a?Ha" },
                {   L_,         ":a?Na" },
                {   L_,         ":a?Pa" },
                {   L_,         ":a?Qa" },
                {   L_,         ":a?Ra" },
                {   L_,         ":a?Wa" },
                {   L_,         ":aCbFcGdQf  :g?+ha" },
                {   L_,         ":aCbFcGdQf  :g?#ha" },
                {   L_,         ":a?CbFcGdQf :g?%ha" },
                {   L_,         ":a?CbFcGdQf :g?@ha" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const char *SPEC = DATA[i].d_spec;

                Schema s; ggSchema(&s, SPEC);
                const RecDef& r = s.record(s.numRecords() - 1);
                ConstRecDefShdPtr rp(&r, NilDeleter(), 0);

                ET::Type TYPE =
                    RecDef::BDEM_SEQUENCE_RECORD == r.recordType()
                    ? ET::BDEM_LIST : ET::BDEM_CHOICE;

                if (veryVerbose) { T_ P(s) };

                if (ET::BDEM_LIST == TYPE) {
                    List list;
                    Table table;
                    ggList(&list, &r);
                    ggTable(&table, &r);

                    ListShdPtr lp(&list, NilDeleter(), 0);
                    TableShdPtr tp(&table, NilDeleter(), 0);

                    bslma_TestAllocator t(veryVeryVerbose);
                    Obj mX(rp, ET::BDEM_LIST);  const Obj& X = mX;
                    mX.setValue(*lp);
                    Obj mY(rp, ET::BDEM_TABLE);  const Obj& Y = mY;
                    mY.setValue(*tp);

                    if (veryVerbose) { T_ P(X) P(Y) };

                    const CERef CL(&list, &bdem_ListImp::d_listAttr);
                    const CERef CT(&table, &bdem_TableImp::d_tableAttr);

                    bsl::ostringstream exp1, exp2, exp3, exp4, exp5;
                    bsl::ostringstream exp6, exp7, exp8, exp9, exp10;
                    SchemaAggUtil::print(exp1, CL, &r, 2, 4);
                    SchemaAggUtil::print(exp2, CL, &r, 2, -4);
                    SchemaAggUtil::print(exp3, CL, &r, -2, 4);
                    SchemaAggUtil::print(exp4, CL, &r, -2, -4);
                    SchemaAggUtil::print(exp5, CL, &r, 0, -1);
                    SchemaAggUtil::print(exp6, CT, &r, 2, 4);
                    SchemaAggUtil::print(exp7, CT, &r, 2, -4);
                    SchemaAggUtil::print(exp8, CT, &r, -2, 4);
                    SchemaAggUtil::print(exp9, CT, &r, -2, -4);
                    SchemaAggUtil::print(exp10, CT, &r, 0, -1);

                    bsl::ostringstream os1, os2, os3, os4, os5;
                    bsl::ostringstream os6, os7, os8, os9, os10;
                    X.print(os1, 2, 4);
                    X.print(os2, 2, -4);
                    X.print(os3, -2, 4);
                    X.print(os4, -2, -4);
                    os5 << X;
                    Y.print(os6, 2, 4);
                    Y.print(os7, 2, -4);
                    Y.print(os8, -2, 4);
                    Y.print(os9, -2, -4);
                    os10 << Y;

                    LOOP3_ASSERT(TYPE, exp1.str(), os1.str(),
                                 exp1.str() == os1.str());
                    LOOP3_ASSERT(TYPE, exp2.str(), os2.str(),
                                 exp2.str() == os2.str());
                    LOOP3_ASSERT(TYPE, exp3.str(), os3.str(),
                                 exp3.str() == os3.str());
                    LOOP3_ASSERT(TYPE, exp4.str(), os4.str(),
                                 exp4.str() == os4.str());
                    LOOP3_ASSERT(TYPE, exp5.str(), os5.str(),
                                 exp5.str() == os5.str());
                    LOOP3_ASSERT(TYPE, exp6.str(), os6.str(),
                                 exp6.str() == os6.str());
                    LOOP3_ASSERT(TYPE, exp7.str(), os7.str(),
                                 exp7.str() == os7.str());
                    LOOP3_ASSERT(TYPE, exp8.str(), os8.str(),
                                 exp8.str() == os8.str());
                    LOOP3_ASSERT(TYPE, exp9.str(), os9.str(),
                                 exp9.str() == os9.str());
                    LOOP3_ASSERT(TYPE, exp10.str(), os10.str(),
                                 exp10.str() == os10.str());
                }
                else if (ET::BDEM_CHOICE == TYPE) {
                    Choice choice;
                    ChoiceArray choiceArray;
                    ggChoice(&choice, &r);
                    ggChoiceArray(&choiceArray, &r);
                    ChoiceShdPtr cp(&choice, NilDeleter(), 0);
                    ChoiceArrayShdPtr ccp(&choiceArray, NilDeleter(), 0);

                    bslma_TestAllocator t(veryVeryVerbose);
                    Obj mX(rp, ET::BDEM_CHOICE); const Obj& X = mX;
                    mX.setValue(*cp);
                    Obj mY(rp, ET::BDEM_CHOICE_ARRAY); const Obj& Y = mY;
                    mY.setValue(*ccp);

                    if (veryVerbose) { T_ P(X) };

                    const CERef CC(&choice, &bdem_ChoiceImp::d_choiceAttr);
                    const CERef CCA(&choiceArray,
                                    &bdem_ChoiceArrayImp::d_choiceArrayAttr);

                    bsl::ostringstream exp1, exp2, exp3, exp4, exp5;
                    bsl::ostringstream exp6, exp7, exp8, exp9, exp10;
                    SchemaAggUtil::print(exp1, CC, &r, 2, 4);
                    SchemaAggUtil::print(exp2, CC, &r, 2, -4);
                    SchemaAggUtil::print(exp3, CC, &r, -2, 4);
                    SchemaAggUtil::print(exp4, CC, &r, -2, -4);
                    SchemaAggUtil::print(exp5, CC, &r, 0, -1);
                    SchemaAggUtil::print(exp6, CCA, &r, 2, 4);
                    SchemaAggUtil::print(exp7, CCA, &r, 2, -4);
                    SchemaAggUtil::print(exp8, CCA, &r, -2, 4);
                    SchemaAggUtil::print(exp9, CCA, &r, -2, -4);
                    SchemaAggUtil::print(exp10, CCA, &r, 0, -1);

                    bsl::ostringstream os1, os2, os3, os4, os5;
                    bsl::ostringstream os6, os7, os8, os9, os10;
                    X.print(os1, 2, 4);
                    X.print(os2, 2, -4);
                    X.print(os3, -2, 4);
                    X.print(os4, -2, -4);
                    os5 << X;
                    Y.print(os6, 2, 4);
                    Y.print(os7, 2, -4);
                    Y.print(os8, -2, 4);
                    Y.print(os9, -2, -4);
                    os10 << Y;

                    LOOP3_ASSERT(TYPE, exp1.str(), os1.str(),
                                 exp1.str() == os1.str());
                    LOOP3_ASSERT(TYPE, exp2.str(), os2.str(),
                                 exp2.str() == os2.str());
                    LOOP3_ASSERT(TYPE, exp3.str(), os3.str(),
                                 exp3.str() == os3.str());
                    LOOP3_ASSERT(TYPE, exp4.str(), os4.str(),
                                 exp4.str() == os4.str());
                    LOOP3_ASSERT(TYPE, exp5.str(), os5.str(),
                                 exp5.str() == os5.str());
                    LOOP3_ASSERT(TYPE, exp6.str(), os6.str(),
                                 exp6.str() == os6.str());
                    LOOP3_ASSERT(TYPE, exp7.str(), os7.str(),
                                 exp7.str() == os7.str());
                    LOOP3_ASSERT(TYPE, exp8.str(), os8.str(),
                                 exp8.str() == os8.str());
                    LOOP3_ASSERT(TYPE, exp9.str(), os9.str(),
                                 exp9.str() == os9.str());
                    LOOP3_ASSERT(TYPE, exp10.str(), os10.str(),
                                 exp10.str() == os10.str());
               }
           }
        }
}

static void testCase10(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
    // Execute the test case for creating instances of 'bcem_Aggregate'.  Note
    // that this is a work-around to avoid unhandled exceptions on Windows
    // (DRQS 13364916).  The root cause of those errors is unknown, but it
    // *appears* to be a VC compiler error (probably related to the test-driver
    // 'main' function being 7000 lines and containing many try/catch blocks).
    // This function was cut & paste directly from the original test case.
    // --------------------------------------------------------------------
    // TESTING CREATORS
    //
    // Concerns:
    //   - The constructor can be used to create an object containing any
    //     unset value.
    //   - The constructor taking an element type and value can be used to
    //     construct all types of scalar aggregates.
    //   - The constructors that take the meta-data and data information
    //     properly construct the aggregate objects with them.
    //
    // Plan:
    //   - Test creation of an empty aggregate.  Confirm its data type and
    //     meta-data and data information is empty.
    //   - Construct scalar aggregates by calling the constructor taking a
    //     type and value.  Confirm that the data type of the constructed
    //     object is as expected and that it stores the expected value.
    //   - Construct aggregates by specifying various combinations of
    //     meta-data and data values and confirm that in each case the
    //     constructed object stores the expected meta-data and data.
    //   - Confirm that the aggregates are constructed with the correct
    //     allocator
    //   - Confirm that none of the constructors leak any memory, even in
    //     the presence of exceptions.
    //
    // Testing:
    //   bcem_Aggregate(bslma_Allocator *basicAllocator);
    //   template <typename VALTYPE>
    //       bcem_Aggregate(const ET::Type   dataType,
    //                      const VALTYPE&   value,
    //                      bslma_Allocator *basicAllocator);
    //   explicit bcem_Aggregate(const ConstRecDefShdPtr&  recordDefPtr,
    //                           bslma_Allocator          *basicAllocator);
    //   explicit bcem_Aggregate(const RecDefShdPtr&  recordDefPtr,
    //                           bslma_Allocator     *basicAllocator);
    //   bcem_Aggregate(const ConstRecDefShdPtr&  recordDefPtr,
    //                  ET::Type                  elemType,
    //                  bslma_Allocator          *basicAllocator);
    //   bcem_Aggregate(const RecDefShdPtr&  recordDefPtr,
    //                  ET::Type             elemType,
    //                  bslma_Allocator     *basicAllocator);
    //   bcem_Aggregate(const ConstSchemaShdPtr&  schemaPtr,
    //                  const bsl::string&        recordName,
    //                  ET::Type                  elemType,
    //                  bslma_Allocator          *basicAllocator);
    //   bcem_Aggregate(const SchemaShdPtr&  schemaPtr,
    //                  const bsl::string&   recordName,
    //                  ET::Type             elemType,
    //                  bslma_Allocator     *basicAllocator);
    //   ~bcem_Aggregate();
    // --------------------------------------------------------------------
    if (verbose) tst::cout << "\nTESTING CREATORS"
                           << "\n================" << bsl::endl;

    if (verbose) tst::cout << "\nTest Default Constructor"
                           << "\n========================"
                           << bsl::endl;
    {
        bslma_TestAllocator da;
        bslma_DefaultAllocatorGuard allocGuard(&da);

        Obj mX; const Obj& X = mX;
        ASSERT(ET::BDEM_VOID            == X.dataType());
        ASSERT(ConstRecDefShdPtr() == X.recordDefPtr());
        ASSERT(VoidDataShdPtr()    == X.dataPtr());
        ASSERT(0                   == X.data());
        ASSERT(true                == X.isNul2());
    }

    if (verbose) tst::cout << "\nTest Constructor taking type and value"
                           << "\n======================================"
                           << bsl::endl;
    {
        for (int i = 0; i < ET::BDEM_NUM_TYPES; ++i) {
            ET::Type TYPE = (ET::Type) i;

            if (!ET::isScalarType(TYPE)) {
                continue;
            }

            if (veryVerbose) { P(TYPE) };

            const CERef CEA = getCERef(TYPE, 1);
            bsl::string sa; const bsl::string& SA = sa;

            bdem_Convert::convert(&sa, CEA);

            bslma_TestAllocator da("da");
            bslma_TestAllocator testAllocator("ta", veryVeryVerbose);

            BEGIN_BSLMA_EXCEPTION_TEST {
                bslma_DefaultAllocatorGuard allocGuard(&da);

                Obj mA(TYPE, CEA, &testAllocator); const Obj& A = mA;
                LOOP_ASSERT(TYPE, TYPE == A.dataType());
                LOOP_ASSERT(TYPE, ConstRecDefShdPtr() == A.recordDefPtr());
                LOOP_ASSERT(TYPE, CEA  == A.asElemRef());

                Obj mB(TYPE, SA.c_str(), &testAllocator); const Obj& B = mB;
                LOOP_ASSERT(TYPE, TYPE == B.dataType());
                LOOP_ASSERT(TYPE, ConstRecDefShdPtr() == B.recordDefPtr());
                LOOP_ASSERT(TYPE, CEA  == B.asElemRef());

                ASSERT(0 == da.numBlocksInUse());

                Obj mC(TYPE, CEA); const Obj& C = mC;
                LOOP_ASSERT(TYPE, TYPE == C.dataType());
                LOOP_ASSERT(TYPE, ConstRecDefShdPtr() == C.recordDefPtr());
                LOOP_ASSERT(TYPE, CEA  == C.asElemRef());

                Obj mD(TYPE, SA.c_str()); const Obj& D = mD;
                LOOP_ASSERT(TYPE, TYPE == D.dataType());
                LOOP_ASSERT(TYPE, ConstRecDefShdPtr() == D.recordDefPtr());
                LOOP_ASSERT(TYPE, CEA  == D.asElemRef());
            } END_BSLMA_EXCEPTION_TEST
                  }

        // Test error messages (Invalid conversion)
        {
            if (verbose) tst::cout << "\nTest invalid conversion errors"
                                   << "\n=============================="
                                   << bsl::endl;
            const CERef CEA = getCERef(ET::BDEM_DOUBLE, 1);
            bsl::string sa; const bsl::string& SA = sa;
            bdem_Convert::convert(&sa, CEA);

            bslma_TestAllocator ta1(veryVeryVerbose);
            ASSERT_AGG_ERROR(Obj(ET::BDEM_DATETIME, SA.c_str(), &ta1),
                             BCEM_ERR_TBD);

            bslma_TestAllocator ta2(veryVeryVerbose);
            ASSERT_AGG_ERROR(Obj(ET::BDEM_TIMETZ, SA.c_str(), &ta2),
                             BCEM_ERR_TBD);

            bslma_TestAllocator ta3(veryVeryVerbose);
            ASSERT_AGG_ERROR(Obj(ET::BDEM_CHAR_ARRAY, SA.c_str(), &ta2),
                             BCEM_ERR_TBD);
        }
    }

    if (verbose) tst::cout << "\nTest Constructor taking only meta-data"
                           << "\n======================================"
                           << bsl::endl;
    {
        const struct {
                int         d_line;
                const char *d_spec;
        } DATA[] = {
            // Line         Spec
            // ----         ----
            // For List Aggregates
            {   L_,         ":aCa" },
            {   L_,         ":aUa" },
            {   L_,         ":aVa" },
            {   L_,         ":aea" },
            {   L_,         ":afa" },
            {   L_,         ":aAaFb" },
            {   L_,         ":aBaGbHc" },
            {   L_,         ":aNaPbQcRd" },
            {   L_,         ":aCbFcGdQf :g+ha" },
            {   L_,         ":aCbFcGdQf :g#ha" },
            {   L_,         ":a?CbFcGdQf :g%ha" },
            {   L_,         ":a?CbFcGdQf :g@ha" },

            // For Choice Aggregates
            {   L_,         ":a?Ca" },
            {   L_,         ":a?Ua" },
            {   L_,         ":a?Va" },
            {   L_,         ":a?ea" },
            {   L_,         ":a?fa" },
            {   L_,         ":a?AaFb" },
            {   L_,         ":a?BaGbHc" },
            {   L_,         ":a?NaPbQcRd" },
            {   L_,         ":aCbFcGdQf  :g?+ha" },
            {   L_,         ":aCbFcGdQf  :g?#ha" },
            {   L_,         ":a?CbFcGdQf :g?%ha" },
            {   L_,         ":a?CbFcGdQf :g?@ha" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE = DATA[i].d_line;
            const char *SPEC = DATA[i].d_spec;

            Schema schema; const Schema& SCHEMA = schema;
            ggSchema(&schema, SPEC);
            const RecDef *RECORD = &SCHEMA.record(SCHEMA.numRecords() - 1);
            RecDef *record = const_cast<RecDef *>(RECORD);

            ConstSchemaShdPtr csp(&SCHEMA, NilDeleter(), 0);
            const ConstSchemaShdPtr& CSP = csp;
            SchemaShdPtr sp(&schema, NilDeleter(), 0);
            const SchemaShdPtr& SP = sp;

            ConstRecDefShdPtr crp(RECORD, NilDeleter(), 0);
            const ConstRecDefShdPtr& CRP = crp;
            RecDefShdPtr rp(record, NilDeleter(), 0);
            const RecDefShdPtr& RP = rp;

            const bsl::string REC_NAME =
                SCHEMA.recordName(SCHEMA.numRecords() - 1);
            ET::Type TYPE =
                RecDef::BDEM_SEQUENCE_RECORD == RECORD->recordType()
                ? ET::BDEM_LIST : ET::BDEM_CHOICE;

            if (veryVerbose) { T_ P_(SPEC) P(SCHEMA) };

            bslma_TestAllocator da("da");
            bslma_TestAllocator testAllocator("ta", veryVeryVerbose);

            BEGIN_BSLMA_EXCEPTION_TEST {
                // Test constructor that are explicitly supplied a ctor
                {
                    bslma_DefaultAllocatorGuard allocGuard(&da);

                    Obj mA(CRP, &testAllocator); const Obj& A = mA;
                    Obj mB(RP, &testAllocator); const Obj& B = mB;
                    Obj mC(CRP, TYPE, &testAllocator); const Obj& C = mC;
                    Obj mD(RP, TYPE, &testAllocator); const Obj& D = mD;

                    Obj mE(CSP, REC_NAME, TYPE, &testAllocator);
                    const Obj& E = mE;
                    Obj mF(SP, REC_NAME, TYPE, &testAllocator);
                    const Obj& F = mF;

                    ASSERT(CRP  == A.recordDefPtr());
                    ASSERT(CRP  == B.recordDefPtr());
                    ASSERT(CRP  == C.recordDefPtr());
                    ASSERT(CRP  == D.recordDefPtr());
                    ASSERT(CRP  == E.recordDefPtr());
                    ASSERT(CRP  == F.recordDefPtr());

                    ASSERT(RECORD  == &A.recordDef());
                    ASSERT(RECORD  == &B.recordDef());
                    ASSERT(RECORD  == &C.recordDef());
                    ASSERT(RECORD  == &D.recordDef());
                    ASSERT(RECORD  == &E.recordDef());
                    ASSERT(RECORD  == &F.recordDef());

                    ASSERT(TYPE == A.dataType());
                    ASSERT(TYPE == B.dataType());
                    ASSERT(TYPE == C.dataType());
                    ASSERT(TYPE == D.dataType());
                    ASSERT(TYPE == E.dataType());
                    ASSERT(TYPE == F.dataType());

                    if (ET::BDEM_LIST == TYPE) {
                        Obj mW(CRP, ET::BDEM_TABLE, &testAllocator);
                        const Obj& W = mW;
                        Obj mX(RP, ET::BDEM_TABLE, &testAllocator);
                        const Obj& X = mX;
                        Obj mY(CSP, REC_NAME, ET::BDEM_TABLE, &testAllocator);
                        const Obj& Y = mY;
                        Obj mZ(SP, REC_NAME, ET::BDEM_TABLE, &testAllocator);
                        const Obj& Z = mZ;

                        ASSERT(0 == da.numBlocksInUse());

                        ASSERT(CRP == W.recordDefPtr());
                        ASSERT(CRP == X.recordDefPtr());
                        ASSERT(CRP == Y.recordDefPtr());
                        ASSERT(CRP == Z.recordDefPtr());

                        ASSERT(RECORD  == &W.recordDef());
                        ASSERT(RECORD  == &X.recordDef());
                        ASSERT(RECORD  == &Y.recordDef());
                        ASSERT(RECORD  == &Z.recordDef());

                        ASSERT(ET::BDEM_TABLE == W.dataType());
                        ASSERT(ET::BDEM_TABLE == X.dataType());
                        ASSERT(ET::BDEM_TABLE == Y.dataType());
                        ASSERT(ET::BDEM_TABLE == Z.dataType());

                        List list; const List& LIST = list;
                        Table table; const Table& TABLE = table;
                        bdem_SchemaAggregateUtil::initListDeep(&list, *RECORD);
                        bdem_SchemaAggregateUtil::initTable(&table, *RECORD);

                        ASSERT(LIST == A.asElemRef().theList());
                        ASSERT(LIST == B.asElemRef().theList());
                        ASSERT(LIST == C.asElemRef().theList());
                        ASSERT(LIST == D.asElemRef().theList());
                        ASSERT(LIST == E.asElemRef().theList());
                        ASSERT(LIST == F.asElemRef().theList());

                        ASSERT(TABLE == W.asElemRef().theTable());
                        ASSERT(TABLE == X.asElemRef().theTable());
                        ASSERT(TABLE == Y.asElemRef().theTable());
                        ASSERT(TABLE == Z.asElemRef().theTable());
                    }
                    else {
                        Obj mW(CRP, ET::BDEM_CHOICE_ARRAY, &testAllocator);
                        const Obj& W = mW;
                        Obj mX(RP, ET::BDEM_CHOICE_ARRAY, &testAllocator);
                        const Obj& X = mX;
                        Obj mY(CSP,
                               REC_NAME,
                               ET::BDEM_CHOICE_ARRAY,
                               &testAllocator);
                        const Obj& Y = mY;
                        Obj mZ(SP,
                               REC_NAME,
                               ET::BDEM_CHOICE_ARRAY,
                               &testAllocator);
                        const Obj& Z = mZ;

                        ASSERT(0 == da.numBlocksInUse());

                        ASSERT(CRP == W.recordDefPtr());
                        ASSERT(CRP == X.recordDefPtr());
                        ASSERT(CRP == Y.recordDefPtr());
                        ASSERT(CRP == Z.recordDefPtr());

                        ASSERT(RECORD  == &W.recordDef());
                        ASSERT(RECORD  == &X.recordDef());
                        ASSERT(RECORD  == &Y.recordDef());
                        ASSERT(RECORD  == &Z.recordDef());

                        ASSERT(ET::BDEM_CHOICE_ARRAY == W.dataType());
                        ASSERT(ET::BDEM_CHOICE_ARRAY == X.dataType());
                        ASSERT(ET::BDEM_CHOICE_ARRAY == Y.dataType());
                        ASSERT(ET::BDEM_CHOICE_ARRAY == Z.dataType());

                        Choice choice; const Choice& CHOICE = choice;
                        ChoiceArray choiceArray;
                        const ChoiceArray& CHOICE_ARRAY = choiceArray;
                        bdem_SchemaAggregateUtil::initChoice(&choice, *RECORD);
                        bdem_SchemaAggregateUtil::initChoiceArray(&choiceArray,
                                                                  *RECORD);

                        ASSERT(CHOICE == A.asElemRef().theChoice());
                        ASSERT(CHOICE == B.asElemRef().theChoice());
                        ASSERT(CHOICE == C.asElemRef().theChoice());
                        ASSERT(CHOICE == D.asElemRef().theChoice());
                        ASSERT(CHOICE == E.asElemRef().theChoice());
                        ASSERT(CHOICE == F.asElemRef().theChoice());

                        ASSERT(CHOICE_ARRAY == W.asElemRef().theChoiceArray());
                        ASSERT(CHOICE_ARRAY == X.asElemRef().theChoiceArray());
                        ASSERT(CHOICE_ARRAY == Y.asElemRef().theChoiceArray());
                        ASSERT(CHOICE_ARRAY == Z.asElemRef().theChoiceArray());
                    }
                }

                // Test constructor where allocator is not explicitly supplied
                {
                    bslma_DefaultAllocatorGuard allocGuard(&testAllocator);
                    int numBytes = testAllocator.numBytesInUse();

                    Obj mA(CRP); const Obj& A = mA;
                    ASSERT(numBytes < testAllocator.numBytesInUse());
                    numBytes = testAllocator.numBytesInUse();

                    Obj mB(RP); const Obj& B = mB;
                    ASSERT(numBytes < testAllocator.numBytesInUse());
                    numBytes = testAllocator.numBytesInUse();

                    Obj mC(CRP, TYPE); const Obj& C = mC;
                    ASSERT(numBytes < testAllocator.numBytesInUse());
                    numBytes = testAllocator.numBytesInUse();

                    Obj mD(RP, TYPE); const Obj& D = mD;
                    ASSERT(numBytes < testAllocator.numBytesInUse());
                    numBytes = testAllocator.numBytesInUse();

                    Obj mE(CSP, REC_NAME); const Obj& E = mE;
                    ASSERT(numBytes < testAllocator.numBytesInUse());
                    numBytes = testAllocator.numBytesInUse();

                    Obj mF(SP, REC_NAME); const Obj& F = mF;
                    ASSERT(numBytes < testAllocator.numBytesInUse());
                    numBytes = testAllocator.numBytesInUse();

                    Obj mG(CSP, REC_NAME, TYPE); const Obj& G = mG;
                    ASSERT(numBytes < testAllocator.numBytesInUse());
                    numBytes = testAllocator.numBytesInUse();

                    Obj mH(SP, REC_NAME, TYPE); const Obj& H = mH;
                    ASSERT(numBytes < testAllocator.numBytesInUse());
                    numBytes = testAllocator.numBytesInUse();

                    ASSERT(CRP  == A.recordDefPtr());
                    ASSERT(CRP  == B.recordDefPtr());
                    ASSERT(CRP  == C.recordDefPtr());
                    ASSERT(CRP  == D.recordDefPtr());
                    ASSERT(CRP  == E.recordDefPtr());
                    ASSERT(CRP  == F.recordDefPtr());
                    ASSERT(CRP  == G.recordDefPtr());
                    ASSERT(CRP  == H.recordDefPtr());

                    ASSERT(RECORD  == &A.recordDef());
                    ASSERT(RECORD  == &B.recordDef());
                    ASSERT(RECORD  == &C.recordDef());
                    ASSERT(RECORD  == &D.recordDef());
                    ASSERT(RECORD  == &E.recordDef());
                    ASSERT(RECORD  == &F.recordDef());
                    ASSERT(RECORD  == &G.recordDef());
                    ASSERT(RECORD  == &H.recordDef());

                    ASSERT(TYPE == A.dataType());
                    ASSERT(TYPE == B.dataType());
                    ASSERT(TYPE == C.dataType());
                    ASSERT(TYPE == D.dataType());
                    ASSERT(TYPE == E.dataType());
                    ASSERT(TYPE == F.dataType());
                    ASSERT(TYPE == G.dataType());
                    ASSERT(TYPE == H.dataType());

                    if (ET::BDEM_LIST == TYPE) {
                        Obj mW(CRP, ET::BDEM_TABLE); const Obj& W = mW;
                        ASSERT(numBytes < testAllocator.numBytesInUse());
                        numBytes = testAllocator.numBytesInUse();

                        Obj mX(RP, ET::BDEM_TABLE); const Obj& X = mX;
                        ASSERT(numBytes < testAllocator.numBytesInUse());
                        numBytes = testAllocator.numBytesInUse();

                        Obj mY(CSP, REC_NAME, ET::BDEM_TABLE);
                        const Obj& Y = mY;
                        ASSERT(numBytes < testAllocator.numBytesInUse());
                        numBytes = testAllocator.numBytesInUse();

                        Obj mZ(SP, REC_NAME, ET::BDEM_TABLE);
                        const Obj& Z = mZ;
                        ASSERT(numBytes < testAllocator.numBytesInUse());
                        numBytes = testAllocator.numBytesInUse();

                        ASSERT(ET::BDEM_TABLE == W.dataType());
                        ASSERT(ET::BDEM_TABLE == X.dataType());
                        ASSERT(ET::BDEM_TABLE == Y.dataType());
                        ASSERT(ET::BDEM_TABLE == Z.dataType());

                        ASSERT(CRP       == W.recordDefPtr());
                        ASSERT(CRP       == X.recordDefPtr());
                        ASSERT(CRP       == Y.recordDefPtr());
                        ASSERT(CRP       == Z.recordDefPtr());

                        ASSERT(RECORD  == &W.recordDef());
                        ASSERT(RECORD  == &X.recordDef());
                        ASSERT(RECORD  == &Y.recordDef());
                        ASSERT(RECORD  == &Z.recordDef());

                        List list; const List& LIST = list;
                        Table table; const Table& TABLE = table;
                        bdem_SchemaAggregateUtil::initListDeep(&list, *RECORD);
                        bdem_SchemaAggregateUtil::initTable(&table, *RECORD);

                        ASSERT(LIST == A.asElemRef().theList());
                        ASSERT(LIST == B.asElemRef().theList());
                        ASSERT(LIST == C.asElemRef().theList());
                        ASSERT(LIST == D.asElemRef().theList());
                        ASSERT(LIST == E.asElemRef().theList());
                        ASSERT(LIST == F.asElemRef().theList());
                        ASSERT(LIST == G.asElemRef().theList());
                        ASSERT(LIST == H.asElemRef().theList());

                        ASSERT(TABLE == W.asElemRef().theTable());
                        ASSERT(TABLE == X.asElemRef().theTable());
                        ASSERT(TABLE == Y.asElemRef().theTable());
                        ASSERT(TABLE == Z.asElemRef().theTable());
                    }
                    else {
                        Obj mW(CRP, ET::BDEM_CHOICE_ARRAY); const Obj& W = mW;
                        ASSERT(numBytes < testAllocator.numBytesInUse());
                        numBytes = testAllocator.numBytesInUse();

                        Obj mX(RP, ET::BDEM_CHOICE_ARRAY); const Obj& X = mX;
                        ASSERT(numBytes < testAllocator.numBytesInUse());
                        numBytes = testAllocator.numBytesInUse();

                        Obj mY(CSP, REC_NAME, ET::BDEM_CHOICE_ARRAY);
                        const Obj& Y = mY;
                        ASSERT(numBytes < testAllocator.numBytesInUse());
                        numBytes = testAllocator.numBytesInUse();

                        Obj mZ(SP, REC_NAME, ET::BDEM_CHOICE_ARRAY);
                        const Obj& Z = mZ;
                        ASSERT(numBytes < testAllocator.numBytesInUse());
                        numBytes = testAllocator.numBytesInUse();

                        ASSERT(ET::BDEM_CHOICE_ARRAY == W.dataType());
                        ASSERT(ET::BDEM_CHOICE_ARRAY == X.dataType());
                        ASSERT(ET::BDEM_CHOICE_ARRAY == Y.dataType());
                        ASSERT(ET::BDEM_CHOICE_ARRAY == Z.dataType());

                        ASSERT(CRP == W.recordDefPtr());
                        ASSERT(CRP == X.recordDefPtr());
                        ASSERT(CRP == Y.recordDefPtr());
                        ASSERT(CRP == Z.recordDefPtr());

                        ASSERT(RECORD  == &W.recordDef());
                        ASSERT(RECORD  == &X.recordDef());
                        ASSERT(RECORD  == &Y.recordDef());
                        ASSERT(RECORD  == &Z.recordDef());

                        Choice choice; const Choice& CHOICE = choice;
                        ChoiceArray choiceArray;
                        const ChoiceArray& CHOICE_ARRAY = choiceArray;
                        bdem_SchemaAggregateUtil::initChoice(&choice, *RECORD);
                        bdem_SchemaAggregateUtil::initChoiceArray(&choiceArray,
                                                                  *RECORD);

                        ASSERT(CHOICE == A.asElemRef().theChoice());
                        ASSERT(CHOICE == B.asElemRef().theChoice());
                        ASSERT(CHOICE == C.asElemRef().theChoice());
                        ASSERT(CHOICE == D.asElemRef().theChoice());
                        ASSERT(CHOICE == E.asElemRef().theChoice());
                        ASSERT(CHOICE == F.asElemRef().theChoice());
                        ASSERT(CHOICE == G.asElemRef().theChoice());
                        ASSERT(CHOICE == H.asElemRef().theChoice());

                        ASSERT(CHOICE_ARRAY == W.asElemRef().theChoiceArray());
                        ASSERT(CHOICE_ARRAY == X.asElemRef().theChoiceArray());
                        ASSERT(CHOICE_ARRAY == Y.asElemRef().theChoiceArray());
                        ASSERT(CHOICE_ARRAY == Z.asElemRef().theChoiceArray());
                    }
                }
            } END_BSLMA_EXCEPTION_TEST
                  }
    }
    if (verbose) tst::cout << "\nTest Constructor taking meta-data & data"
                           << "\n========================================"
                           << bsl::endl;
    {
        const struct {
                int         d_line;
                const char *d_spec;
        } DATA[] = {
            // Line         Spec
            // ----         ----
            // For List Aggregates
            {   L_,         ":aCa" },
            {   L_,         ":aUa" },
            {   L_,         ":aVa" },
            {   L_,         ":aea" },
            {   L_,         ":afa" },
            {   L_,         ":aAaFb" },
            {   L_,         ":aBaGbHc" },
            {   L_,         ":aNaPbQcRd" },
            {   L_,         ":aCbFcGdQf :g+ha" },
            {   L_,         ":aCbFcGdQf :g#ha" },
            {   L_,         ":a?CbFcGdQf :g%ha" },
            {   L_,         ":a?CbFcGdQf :g@ha" },

            // For Choice Aggregates
            {   L_,         ":a?Ca" },
            {   L_,         ":a?Ua" },
            {   L_,         ":a?Va" },
            {   L_,         ":a?ea" },
            {   L_,         ":a?fa" },
            {   L_,         ":a?AaFb" },
            {   L_,         ":a?BaGbHc" },
            {   L_,         ":a?NaPbQcRd" },
            {   L_,         ":aCbFcGdQf  :g?+ha" },
            {   L_,         ":aCbFcGdQf  :g?#ha" },
            {   L_,         ":a?CbFcGdQf :g?%ha" },
            {   L_,         ":a?CbFcGdQf :g?@ha" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE = DATA[i].d_line;
            const char *SPEC = DATA[i].d_spec;

            Schema schema; const Schema& SCHEMA = schema;
            ggSchema(&schema, SPEC);
            const RecDef *RECORD = &SCHEMA.record(SCHEMA.numRecords() - 1);
            RecDef *record = const_cast<RecDef *>(RECORD);

            ConstSchemaShdPtr csp(&SCHEMA, NilDeleter(), 0);
            const ConstSchemaShdPtr& CSP = csp;
            SchemaShdPtr sp(&schema, NilDeleter(), 0);
            const SchemaShdPtr& SP = sp;

            ConstRecDefShdPtr crp(RECORD, NilDeleter(), 0);
            const ConstRecDefShdPtr& CRP = crp;
            RecDefShdPtr rp(record, NilDeleter(), 0);
            const RecDefShdPtr& RP = rp;

            const bsl::string REC_NAME =
                SCHEMA.recordName(SCHEMA.numRecords() - 1);
            ET::Type TYPE =
                RecDef::BDEM_SEQUENCE_RECORD == RECORD->recordType()
                ? ET::BDEM_LIST : ET::BDEM_CHOICE;

            if (veryVerbose) { T_ P_(SPEC) P(SCHEMA) };

            if (ET::BDEM_LIST == TYPE) {
                List list; const List& LIST = list;
                Table table; const Table& TABLE = table;
                ggList(&list, RECORD);
                ggTable(&table, RECORD);

                ListShdPtr lp(&list, NilDeleter(), 0);
                const ListShdPtr& LP = lp;
                RowShdPtr row(&list.row(), NilDeleter(), 0);
                const RowShdPtr& ROW = row;
                TableShdPtr tp(&table, NilDeleter(), 0);
                const TableShdPtr& TP = tp;

                bslma_TestAllocator testAllocator(veryVeryVerbose);

                BEGIN_BSLMA_EXCEPTION_TEST {
                    bslma_DefaultAllocatorGuard allocGuard(&testAllocator);

                    Obj mA(CRP, ET::BDEM_LIST);             const Obj& A = mA;
                    mA.setValue(*LP);
                    Obj mB(RP, ET::BDEM_LIST);              const Obj& B = mB;
                    mB.setValue(*LP);
                    Obj mC(CRP, ET::BDEM_LIST);             const Obj& C = mC;
                    mC.setValue(*ROW);
                    Obj mD(RP, ET::BDEM_LIST);              const Obj& D = mD;
                    mD.setValue(*ROW);
                    Obj mE(CRP, ET::BDEM_TABLE);            const Obj& E = mE;
                    mE.setValue(*TP);
                    Obj mF(RP, ET::BDEM_TABLE);             const Obj& F = mF;
                    mF.setValue(*TP);

                    Obj mG(CSP, REC_NAME, ET::BDEM_LIST);   const Obj& G = mG;
                    mG.setValue(*LP);
                    Obj mH(SP, REC_NAME, ET::BDEM_LIST);    const Obj& H = mH;
                    mH.setValue(*LP);
                    Obj mI(CSP, REC_NAME, ET::BDEM_LIST);   const Obj& I = mI;
                    mI.setValue(*ROW);
                    Obj mJ(SP, REC_NAME, ET::BDEM_LIST);    const Obj& J = mJ;
                    mJ.setValue(*ROW);
                    Obj mK(CSP, REC_NAME, ET::BDEM_TABLE);  const Obj& K = mK;
                    mK.setValue(*TP);
                    Obj mL(SP, REC_NAME, ET::BDEM_TABLE);   const Obj& L = mL;
                    mL.setValue(*TP);

                    ASSERT(CRP  == A.recordDefPtr());
                    ASSERT(CRP  == B.recordDefPtr());
                    ASSERT(CRP  == C.recordDefPtr());
                    ASSERT(CRP  == D.recordDefPtr());
                    ASSERT(CRP  == E.recordDefPtr());
                    ASSERT(CRP  == F.recordDefPtr());
                    ASSERT(CRP  == G.recordDefPtr());
                    ASSERT(CRP  == H.recordDefPtr());
                    ASSERT(CRP  == I.recordDefPtr());
                    ASSERT(CRP  == J.recordDefPtr());
                    ASSERT(CRP  == K.recordDefPtr());
                    ASSERT(CRP  == L.recordDefPtr());

                    ASSERT(RECORD  == &A.recordDef());
                    ASSERT(RECORD  == &B.recordDef());
                    ASSERT(RECORD  == &C.recordDef());
                    ASSERT(RECORD  == &D.recordDef());
                    ASSERT(RECORD  == &E.recordDef());
                    ASSERT(RECORD  == &F.recordDef());
                    ASSERT(RECORD  == &G.recordDef());
                    ASSERT(RECORD  == &H.recordDef());
                    ASSERT(RECORD  == &I.recordDef());
                    ASSERT(RECORD  == &J.recordDef());
                    ASSERT(RECORD  == &K.recordDef());
                    ASSERT(RECORD  == &L.recordDef());

                    ASSERT(ET::BDEM_LIST  == A.dataType());
                    ASSERT(ET::BDEM_LIST  == B.dataType());
                    ASSERT(ET::BDEM_LIST  == C.dataType());
                    ASSERT(ET::BDEM_LIST  == D.dataType());
                    ASSERT(ET::BDEM_TABLE == E.dataType());
                    ASSERT(ET::BDEM_TABLE == F.dataType());
                    ASSERT(ET::BDEM_LIST  == G.dataType());
                    ASSERT(ET::BDEM_LIST  == H.dataType());
                    ASSERT(ET::BDEM_LIST  == I.dataType());
                    ASSERT(ET::BDEM_LIST  == J.dataType());
                    ASSERT(ET::BDEM_TABLE == K.dataType());
                    ASSERT(ET::BDEM_TABLE == L.dataType());

                    ASSERT(LIST       == A.asElemRef().theList());
                    ASSERT(LIST       == B.asElemRef().theList());
                    ASSERT(*ROW.ptr() == C.asElemRef().theList().row());
                    ASSERT(*ROW.ptr() == D.asElemRef().theList().row());
                    ASSERT(TABLE      == E.asElemRef().theTable());
                    ASSERT(TABLE      == F.asElemRef().theTable());
                } END_BSLMA_EXCEPTION_TEST
            }
            else {
                Choice choice; const Choice& CHOICE = choice;
                ChoiceArray choiceArray;
                const ChoiceArray& CHOICE_ARRAY = choiceArray;
                ggChoice(&choice, RECORD);
                ggChoiceArray(&choiceArray, RECORD);

                ChoiceShdPtr cp(&choice, NilDeleter(), 0);
                const ChoiceShdPtr& CP = cp;
                ChoiceItemShdPtr item(&choice.item(), NilDeleter(), 0);
                const ChoiceItemShdPtr& ITEM = item;
                ChoiceArrayShdPtr cap(&choiceArray, NilDeleter(), 0);
                const ChoiceArrayShdPtr& CAP = cap;

                bslma_TestAllocator testAllocator(veryVeryVerbose);

                BEGIN_BSLMA_EXCEPTION_TEST {
                    bslma_DefaultAllocatorGuard allocGuard(&testAllocator);

                    Obj mA(CRP, ET::BDEM_CHOICE);
                    const Obj& A = mA;
                    mA.setValue(*CP);
                    Obj mB(RP, ET::BDEM_CHOICE);
                    const Obj& B = mB;
                    mB.setValue(*CP);
                    Obj mC(CRP, ET::BDEM_CHOICE);
                    const Obj& C = mC;
                    mC.setValue(*ITEM);
                    Obj mD(RP, ET::BDEM_CHOICE);
                    const Obj& D = mD;
                    mD.setValue(*ITEM);
                    Obj mE(CRP, ET::BDEM_CHOICE_ARRAY);
                    const Obj& E = mE;
                    mE.setValue(*CAP);
                    Obj mF(RP, ET::BDEM_CHOICE_ARRAY);
                    const Obj& F = mF;
                    mF.setValue(*CAP);

                    Obj mG(CSP, REC_NAME, ET::BDEM_CHOICE);
                    const Obj& G = mG;
                    mG.setValue(*CP);
                    Obj mH(SP, REC_NAME, ET::BDEM_CHOICE);
                    const Obj& H = mH;
                    mH.setValue(*CP);
                    Obj mI(CSP, REC_NAME, ET::BDEM_CHOICE);
                    const Obj& I = mI;
                    mI.setValue(*ITEM);
                    Obj mJ(SP, REC_NAME, ET::BDEM_CHOICE);
                    const Obj& J = mJ;
                    mJ.setValue(*ITEM);
                    Obj mK(CSP, REC_NAME, ET::BDEM_CHOICE_ARRAY);
                    const Obj& K = mK;
                    mK.setValue(*CAP);
                    Obj mL(SP, REC_NAME, ET::BDEM_CHOICE_ARRAY);
                    const Obj& L = mL;
                    mL.setValue(*CAP);

                    ASSERT(CRP  == A.recordDefPtr());
                    ASSERT(CRP  == B.recordDefPtr());
                    ASSERT(CRP  == C.recordDefPtr());
                    ASSERT(CRP  == D.recordDefPtr());
                    ASSERT(CRP  == E.recordDefPtr());
                    ASSERT(CRP  == F.recordDefPtr());
                    ASSERT(CRP  == G.recordDefPtr());
                    ASSERT(CRP  == H.recordDefPtr());
                    ASSERT(CRP  == I.recordDefPtr());
                    ASSERT(CRP  == J.recordDefPtr());
                    ASSERT(CRP  == K.recordDefPtr());
                    ASSERT(CRP  == L.recordDefPtr());

                    ASSERT(RECORD  == &A.recordDef());
                    ASSERT(RECORD  == &B.recordDef());
                    ASSERT(RECORD  == &C.recordDef());
                    ASSERT(RECORD  == &D.recordDef());
                    ASSERT(RECORD  == &E.recordDef());
                    ASSERT(RECORD  == &F.recordDef());
                    ASSERT(RECORD  == &G.recordDef());
                    ASSERT(RECORD  == &H.recordDef());
                    ASSERT(RECORD  == &I.recordDef());
                    ASSERT(RECORD  == &J.recordDef());
                    ASSERT(RECORD  == &K.recordDef());
                    ASSERT(RECORD  == &L.recordDef());

                    ASSERT(ET::BDEM_CHOICE       == A.dataType());
                    ASSERT(ET::BDEM_CHOICE       == B.dataType());
                    ASSERT(ET::BDEM_CHOICE       == C.dataType());
                    ASSERT(ET::BDEM_CHOICE       == D.dataType());
                    ASSERT(ET::BDEM_CHOICE_ARRAY == E.dataType());
                    ASSERT(ET::BDEM_CHOICE_ARRAY == F.dataType());
                    ASSERT(ET::BDEM_CHOICE       == G.dataType());
                    ASSERT(ET::BDEM_CHOICE       == H.dataType());
                    ASSERT(ET::BDEM_CHOICE       == I.dataType());
                    ASSERT(ET::BDEM_CHOICE       == J.dataType());
                    ASSERT(ET::BDEM_CHOICE_ARRAY == K.dataType());
                    ASSERT(ET::BDEM_CHOICE_ARRAY == L.dataType());

                    ASSERT(CHOICE        == A.asElemRef().theChoice());
                    ASSERT(CHOICE        == B.asElemRef().theChoice());
                    ASSERT(CHOICE.item() == C.asElemRef().theChoice().item());
                    ASSERT(CHOICE.item() == D.asElemRef().theChoice().item());
                    ASSERT(CHOICE_ARRAY  == E.asElemRef().theChoiceArray());
                    ASSERT(CHOICE_ARRAY  == F.asElemRef().theChoiceArray());
                } END_BSLMA_EXCEPTION_TEST
            }
        }
    }
}

static void testCase9(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // TESTING IDENTITY AND EQUIVALENCE:
        //
        // Concerns:
        //   - Confirm that empty aggregates compare identical.
        //   - Aggregates compare identical only if they point to the same
        //     object.
        //   - Aggregates compare equivalent only if they have equivalent
        //     structure and identical values.
        //   - If two aggregates compare identical, they will also compare
        //     equivalent.
        //   - If two aggregates compare identical, then they will continue to
        //     be identical after changing a field in either one.
        //   - If two aggregates compare equivalent but not identical, then
        //     they will no longer be equivalent after changing a field in
        //     either one.
        //
        // Plan:
        //   - Create empty aggregates and confirm that they compare identical
        //     and equivalent.
        //   - For each data type, separately create a pair of aggregates.
        //     - Confirm that they not identical.
        //     - Confirm that they are equivalent only if they were created
        //       the same way.
        //     - For equivalent aggregates, modify the value of one of them.
        //     - Confirm that they are no longer equivalent nor identical.
        //   - For each of a set of different schemas, create
        //     two identical objects and a third that is equivalent but not
        //     identical to the other two.
        //     - Confirm the initial identity and equivalence attributes.
        //     - Confirm that identical fields from identical aggregates are
        //       also identical and that equivalent fields from equivalent
        //       aggregates are also equivalent.
        //     - Confirm that after modifying a field of the first or second
        //       aggregate, it is no longer equivalent to the third aggregate,
        //       but the identity relationships do not change.
        //
        // Testing:
        //   bool areIdentical(const bcem_Aggregate&, const bcem_Aggregate&);
        //   bool areEquivalent(const bcem_Aggregate&, const bcem_Aggregate&);
        // --------------------------------------------------------------------

        if (verbose) tst::cout << "\nTESTING IDENTITY AND EQUIVALENCE"
                               << "\n================================"
                               << bsl::endl;

        if (veryVerbose) { T_ cout << "Testing for empty aggregates" << endl; }
        {
            Obj mX; const Obj& X = mX;
            Obj mY; const Obj& Y = mY;
            ASSERT(!Obj::areIdentical(X, Y));
            ASSERT(Obj::areEquivalent(X, Y));
        }

        if (veryVerbose) { T_ cout << "Testing for independent aggregates"
                                   << endl; }
        {
            const struct {
                int      d_line;
                ET::Type d_type;
            } DATA[] = {
                // Line   Type
                // ----   ----
                {   L_,   ET::BDEM_BOOL },
                {   L_,   ET::BDEM_INT },
                {   L_,   ET::BDEM_DOUBLE },
                {   L_,   ET::BDEM_STRING },
                {   L_,   ET::BDEM_DATETIME },
                {   L_,   ET::BDEM_CHAR_ARRAY },
                {   L_,   ET::BDEM_STRING_ARRAY },
                {   L_,   ET::BDEM_DATETIMETZ_ARRAY },
                {   L_,   ET::BDEM_BOOL_ARRAY },
                {   L_,   ET::BDEM_LIST },
                {   L_,   ET::BDEM_TABLE },
                {   L_,   ET::BDEM_CHOICE },
                {   L_,   ET::BDEM_CHOICE_ARRAY },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE1 = DATA[i].d_line;
                ET::Type  TYPE1 = DATA[i].d_type;

                for (int j = 0; j < NUM_DATA; ++j) {
                    const int LINE2 = DATA[j].d_line;
                    ET::Type  TYPE2 = DATA[j].d_type;

                    bslma_TestAllocator t1(veryVeryVerbose);
                    bslma_TestAllocator t2(veryVeryVerbose);

                    const CERef A1 = getCERef(TYPE1, 1);
                    const CERef A2 = getCERef(TYPE1, 2);

                    const CERef B1 = getCERef(TYPE2, 1);
                    const CERef B2 = getCERef(TYPE2, 2);

                    Obj mX(TYPE1, A2, &t1); const Obj& X = mX;
                    Obj mY(TYPE2, B2, &t2); const Obj& Y = mY;
                    Obj mZ(X); const Obj& Z = mZ;

                    LOOP3_ASSERT(LINE1, X, Z, Obj::areIdentical(X, Z));

                    if (i == j) {
                        LOOP4_ASSERT(LINE1, LINE2, X, Y,
                                     ! Obj::areIdentical(X, Y));
                        LOOP4_ASSERT(LINE1, LINE2, X, Y,
                                     Obj::areEquivalent(X, Y));

                        mX.setValue(A1);
                        LOOP3_ASSERT(LINE1, X, Z, Obj::areIdentical(X, Z));
                        LOOP4_ASSERT(LINE1, LINE2, X, Y,
                                     ! Obj::areIdentical(X, Y));
                        LOOP4_ASSERT(LINE1, LINE2, X, Y,
                                     ! Obj::areEquivalent(X, Y));

                        mY.setValue(A1);
                        LOOP4_ASSERT(LINE1, LINE2, X, Y,
                                     ! Obj::areIdentical(X, Y));
                        LOOP4_ASSERT(LINE1, LINE2, X, Y,
                                       Obj::areEquivalent(X, Y));

                        mX.makeNull();
                        LOOP3_ASSERT(LINE1, X, Z, Obj::areIdentical(X, Z));
                        LOOP4_ASSERT(LINE1, LINE2, X, Y,
                                     ! Obj::areIdentical(X, Y));
                        LOOP4_ASSERT(LINE1, LINE2, X, Y,
                                     ! Obj::areEquivalent(X, Y));

                        mY.makeNull();
                        LOOP4_ASSERT(LINE1, LINE2, X, Y,
                                     ! Obj::areIdentical(X, Y));
                        LOOP4_ASSERT(LINE1, LINE2, X, Y,
                                       Obj::areEquivalent(X, Y));
                    }
                    else {
                        LOOP4_ASSERT(LINE1, LINE2, X, Y,
                                     ! Obj::areIdentical(X, Y));
                        LOOP4_ASSERT(LINE1, LINE2, X, Y,
                                     ! Obj::areEquivalent(X, Y));
                    }
                }
            }
        }

        if (veryVerbose) { tst::cout << "\tTesting array and aggregate types"
                                     << bsl::endl; }
        {
            const struct {
                int         d_line;
                const char *d_spec;
            } DATA[] = {
                // Line         Spec
                // ----         ----
                // For List Aggregates
                {   L_,         ":aNa" },
                {   L_,         ":aNa&NT" },
                {   L_,         ":aNa&NF" },
                {   L_,         ":aNa&D0" },
                {   L_,         ":aNa&FN" },
                {   L_,         ":aPa" },
                {   L_,         ":aPa&NT" },
                {   L_,         ":aPa&NF" },
                {   L_,         ":aPa&D0" },
                {   L_,         ":aPa&FN" },
                {   L_,         ":aQa" },
                {   L_,         ":aQa&NT" },
                {   L_,         ":aQa&NF" },
                {   L_,         ":aQa&D0" },
                {   L_,         ":aQa&FN" },
                {   L_,         ":aRa" },
                {   L_,         ":aRa&NT" },
                {   L_,         ":aRa&NF" },
                {   L_,         ":aRa&D0" },
                {   L_,         ":aRa&FN" },
                {   L_,         ":aWa" },
                {   L_,         ":aWa&NT" },
                {   L_,         ":aWa&NF" },
                {   L_,         ":aaa&FN" },
                {   L_,         ":aCbFcGdQf :g+ha" },
                {   L_,         ":aCbFcGdQf :g#ha" },
                {   L_,         ":a?CbFcGdQf :g%ha" },
                {   L_,         ":a?CbFcGdQf :g@ha" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const char *SPEC = DATA[i].d_spec;
                const bool  NSA  = (bool) bsl::strstr(SPEC, "&FN");

                Schema s; ggSchema(&s, SPEC);
                const RecDef& r = NSA
                                ? s.record(0)
                                : s.record(s.numRecords() - 1);
                ConstRecDefShdPtr rp(&r, NilDeleter(), 0);

                if (veryVerbose) { T_ P(s) };

                // Create three objects:
                //   X and Y are identical
                //   Z is equivalent but not identical to X and Y
                bslma_TestAllocator t(veryVeryVerbose);
                Obj mX(rp, &t); const Obj& X = mX;
                Obj mY(X);      const Obj& Y = mY;
                Obj mZ(rp, &t); const Obj& Z = mZ;
                LOOP3_ASSERT(LINE, X, Y, Obj::areIdentical(X, Y));
                LOOP3_ASSERT(LINE, X, Y, Obj::areEquivalent(X, Y));
                LOOP3_ASSERT(LINE, X, Z, ! Obj::areIdentical(X, Z));
                LOOP3_ASSERT(LINE, X, Z, Obj::areEquivalent(X, Z));
                LOOP3_ASSERT(LINE, Z, Y, ! Obj::areIdentical(Y, Z));
                LOOP3_ASSERT(LINE, Z, Y, Obj::areEquivalent(Y, Z));

                if (veryVerbose) { T_ P(X) };

                const char *fldName = r.fieldName(0);
                ET::Type    fldType = NSA
                             ? ET::toArrayType(s.record(1).field(0).elemType())
                             : r.field(0).elemType();
                const CERef A0 = getCERef(fldType, 0);
                const CERef A1 = getCERef(fldType, 1);
                const CERef A2 = getCERef(fldType, 2);

                // Same field from identical objects are identical.
                // Same field from equivalent objects are equivalent.
                Obj mA = X.field(fldName); const Obj& A = mA;
                Obj mB = X.field(fldName); const Obj& B = mB;
                Obj mC = Y.field(fldName); const Obj& C = mC;
                Obj mD = Z.field(fldName); const Obj& D = mD;
                LOOP3_ASSERT(LINE, A, B, Obj::areIdentical(A, B));
                LOOP3_ASSERT(LINE, A, B, Obj::areEquivalent(A, B));
                LOOP3_ASSERT(LINE, A, C, Obj::areIdentical(A, C));
                LOOP3_ASSERT(LINE, A, C, Obj::areEquivalent(A, C));
                LOOP3_ASSERT(LINE, A, D, ! Obj::areIdentical(A, D));
                LOOP3_ASSERT(LINE, A, D, Obj::areEquivalent(A, D));
                LOOP3_ASSERT(LINE, C, D, ! Obj::areIdentical(C, D));
                LOOP3_ASSERT(LINE, C, D, Obj::areEquivalent(C, D));

                // Modifying field in identical objects preserves identity.
                // Modifying field in equivalent objects does not preserve
                // equivalence.
                X.setField(fldName, A1);
                LOOP3_ASSERT(LINE, X, Y, Obj::areIdentical(X, Y));
                LOOP3_ASSERT(LINE, X, Y, Obj::areEquivalent(X, Y));
                LOOP3_ASSERT(LINE, X, Z, ! Obj::areEquivalent(X, Z));
                LOOP3_ASSERT(LINE, X, Z, ! Obj::areIdentical(X, Z));
                LOOP3_ASSERT(LINE, Z, Y, ! Obj::areEquivalent(Y, Z));
                LOOP3_ASSERT(LINE, Z, Y, ! Obj::areIdentical(Y, Z));

                Y.setField(fldName, A2);
                LOOP3_ASSERT(LINE, X, Y, Obj::areIdentical(X, Y));
                LOOP3_ASSERT(LINE, X, Y, Obj::areEquivalent(X, Y));
                if (A2 != A0) {
                    LOOP3_ASSERT(LINE, X, Z, ! Obj::areEquivalent(X, Z));
                    LOOP3_ASSERT(LINE, X, Z, ! Obj::areIdentical(X, Z));
                    LOOP3_ASSERT(LINE, Z, Y, ! Obj::areEquivalent(Y, Z));
                    LOOP3_ASSERT(LINE, Z, Y, ! Obj::areIdentical(Y, Z));
               }

                // Restore equivalence
                Z.setField(fldName, A2);
                LOOP3_ASSERT(LINE, X, Y, Obj::areIdentical(X, Y));
                LOOP3_ASSERT(LINE, X, Y, Obj::areEquivalent(X, Y));
                LOOP3_ASSERT(LINE, X, Z,   Obj::areEquivalent(X, Z));
                LOOP3_ASSERT(LINE, X, Z, ! Obj::areIdentical(X, Z));
                LOOP3_ASSERT(LINE, Z, Y,   Obj::areEquivalent(Y, Z));
                LOOP3_ASSERT(LINE, Z, Y, ! Obj::areIdentical(Y, Z));

                // Check nullness equivalence
                X.setFieldNull(fldName);
                LOOP3_ASSERT(LINE, X, Y, Obj::areIdentical(X, Y));
                LOOP3_ASSERT(LINE, X, Y, Obj::areEquivalent(X, Y));
                LOOP3_ASSERT(LINE, X, Z, ! Obj::areEquivalent(X, Z));
                LOOP3_ASSERT(LINE, X, Z, ! Obj::areIdentical(X, Z));
                LOOP3_ASSERT(LINE, Z, Y, ! Obj::areEquivalent(Y, Z));
                LOOP3_ASSERT(LINE, Z, Y, ! Obj::areIdentical(Y, Z));

                Y.setFieldNull(fldName);
                LOOP3_ASSERT(LINE, X, Y, Obj::areIdentical(X, Y));
                LOOP3_ASSERT(LINE, X, Y, Obj::areEquivalent(X, Y));
                LOOP3_ASSERT(LINE, X, Z, ! Obj::areEquivalent(X, Z));
                LOOP3_ASSERT(LINE, X, Z, ! Obj::areIdentical(X, Z));
                LOOP3_ASSERT(LINE, Z, Y, ! Obj::areEquivalent(Y, Z));
                LOOP3_ASSERT(LINE, Z, Y, ! Obj::areIdentical(Y, Z));

                Z.setFieldNull(fldName);
                LOOP3_ASSERT(LINE, X, Y, Obj::areIdentical(X, Y));
                LOOP3_ASSERT(LINE, X, Y, Obj::areEquivalent(X, Y));
                LOOP3_ASSERT(LINE, X, Z, Obj::areEquivalent(X, Z));
                LOOP3_ASSERT(LINE, X, Z, ! Obj::areIdentical(X, Z));
                LOOP3_ASSERT(LINE, Z, Y, Obj::areEquivalent(Y, Z));
                LOOP3_ASSERT(LINE, Z, Y, ! Obj::areIdentical(Y, Z));
            }
        }

        if (veryVerbose) { tst::cout << "\tChecking equality of a nillable "
                                 << "scalar array and a non-nillable scalar"
                                 << " array" << bsl::endl; }
        {
            const struct {
                int         d_line;
                const char *d_spec;
            } DATA[] = {
                // Line          Spec
                // ----          ----
                {   L_,         ":aKa :bKa&FN" },
                {   L_,         ":aLa :bLa&FN" },
                {   L_,         ":aMa :bMa&FN" },
                {   L_,         ":aNa :bNa&FN" },
                {   L_,         ":aOa :bOa&FN" },
                {   L_,         ":aPa :bPa&FN" },
                {   L_,         ":aQa :bQa&FN" },
                {   L_,         ":aRa :bRa&FN" },
                {   L_,         ":aSa :bSa&FN" },
                {   L_,         ":aTa :bTa&FN" },

                {   L_,         ":aaa :baa&FN" },
                {   L_,         ":aba :bba&FN" },
                {   L_,         ":aca :bca&FN" },
                {   L_,         ":ada :bda&FN" },
                {   L_,         ":b=tu5v :a!ab :c!ab&FN" },
                {   L_,         ":b=tu5v :a/ab :c/ab&FN" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const char *SPEC = DATA[i].d_spec;

                Schema schema; const Schema& SCHEMA = schema;
                ggSchema(&schema, SPEC);
                const RecDef *R1 = &SCHEMA.record(0);
                const RecDef *R2 = &SCHEMA.record(1);

                if (veryVerbose) { T_ P(SPEC) P(SCHEMA) };

                ConstRecDefShdPtr crp1(R1, NilDeleter(), 0);
                const ConstRecDefShdPtr& CRP1 = crp1;
                ConstRecDefShdPtr crp2(R2, NilDeleter(), 0);
                const ConstRecDefShdPtr& CRP2 = crp2;

                ET::Type    SUB_TYPE   = SCHEMA.record(2).field(0).elemType();
                ET::Type    ARRAY_TYPE = EType::toArrayType(SUB_TYPE);
                const char *name1 = R1->fieldName(0);
                const char *name2 = R2->fieldName(0);

                const CERef ARRAY_CEA = getCERef(ARRAY_TYPE, 1);

                if (veryVerbose) { T_ P(SPEC) P(SCHEMA) P(ARRAY_CEA) };

                bslma_TestAllocator testAllocator(veryVeryVerbose);

                Obj mX(CRP1, &testAllocator); const Obj& X = mX;
                Obj mY(CRP2, &testAllocator); const Obj& Y = mY;

                mX.setField(name1, ARRAY_CEA);
                mY.setField(name2, ARRAY_CEA);

                if (veryVerbose) { T_ P(X) P(Y) };

                ASSERT(! Obj::areIdentical(X, Y));
                ASSERT(! Obj::areEquivalent(X, Y));

                Obj A = X.field(name1);
                Obj B = Y.field(name2);
                ASSERT(! Obj::areIdentical(A, B));
                ASSERT(! Obj::areEquivalent(A, B));

                Obj C = A.field(0);
                Obj D = B.field(0);
                ASSERT(! Obj::areIdentical(A, B));
                ASSERT(! Obj::areEquivalent(A, B));
            }
        }
}

static void testCase8(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // TESTING FIELD ID RELATED MANIPULATORS AND ACCESSORS:
        //
        // Concerns:
        //   - That the setFieldById sets the value of the corresponding field
        //   - That the fieldById returns the sub aggregate corresponding to
        //     the specified id
        //   - fieldTypeById returns the correct field type
        //   - fieldRefById returns a reference to the element specified by id
        //
        // Plan:
        //   - All the functions under test call the private function
        //     descendIntoFieldById and then return the data returned by that
        //     function (the setField function additionally calls the setValue
        //     function which was tested previously).  So our main aim in this
        //     test case is to test descendIntoFieldById well and then confirm
        //     that all the functions pass their arguments to that function
        //     correctly.  So to test we construct an aggregate mX from a set
        //     S of aggregates storing different list and choice values.  We
        //     will then call the various functions on mX and verify that
        //     setFieldById sets the values of the specified field and that
        //     fieldById return the value of the request field.  We will also
        //     call the fieldTypeById function to confirm that the data type
        //     of the requested id is as expected.  Finally, we will call
        //     fieldRefById to confirm that a reference to the requested field
        //     is returned.  Note that we will also test that error conditions
        //     are output onto stderr.
        //
        // Testing:
        //   template <typename VALTYPE>
        //   void setFieldById(int fieldId, const VALTYPE& value) const;
        //   void setFieldNullById(int fieldId) const;
        //   bcem_Aggregate fieldById(int fieldId) const;
        //   ET::Type fieldTypeById(int fieldId) const;
        //   bdem_ElemRef fieldRefById(int fieldId) const;
        //
        // Private functions:
        //   bool FieldCursor::descendIntoFieldById(int fieldId);
        // --------------------------------------------------------------------

        if (verbose) tst::cout << "\nTESTING FIELD ID RELATED "
                               << "MANIPULATORS AND ACCESSORS"
                               << "\n========================="
                               << "=========================="
                               << bsl::endl;
        const struct {
            int         d_line;
            const char *d_spec;
        } DATA[] = {
            // Line    Spec
            // ----    ----
            // For List Aggregates
            {   L_,    ":aCcFfGgHhNnPpQqRrWw" },

            // For Choice Aggregates
            {   L_,    ":a?CcFfGgHhNnPpQqRrWw" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE = DATA[i].d_line;
            const char *SPEC = DATA[i].d_spec;

            Schema s; ggSchema(&s, SPEC);
            const RecDef& r = s.record(s.numRecords() - 1);
            ConstRecDefShdPtr rp(&r, NilDeleter(), 0);

            if (veryVerbose) { T_ P(s) };

            Obj mX(rp); const Obj& X = mX;

            if (veryVerbose) { T_ P(X) };

            for (int j = 0; j < r.numFields(); ++j) {
                ET::Type  TYPE = r.field(j).elemType();
                const int ID   = r.fieldId(j);

                const CERef CEA = getCERef(TYPE, 1);
                const CERef CEB = getCERef(TYPE, 2);

                if (RecDef::BDEM_CHOICE_RECORD == r.recordType()) {
                    mX.makeSelectionById(ID);
                }

                LOOP_ASSERT(ID, TYPE == X.fieldTypeById(ID));

                mX.setFieldById(ID, CEA);
                LOOP_ASSERT(ID, !X.fieldById(ID).isNul2());
                LOOP_ASSERT(ID, CEA == X.fieldById(ID).asElemRef());

                Obj mA = X.fieldById(ID); const Obj& A = mA;
                LOOP_ASSERT(ID, !A.isNul2());
                LOOP_ASSERT(ID, CEA == A.asElemRef());
                LOOP_ASSERT(ID, CEA == X.fieldRefById(ID));

                mX.setFieldNullById(ID);
                LOOP_ASSERT(ID, X.fieldById(ID).isNul2());
                LOOP_ASSERT(ID, isUnset(X.fieldById(ID).asElemRef()));
                LOOP_ASSERT(ID, A.isNul2());
                LOOP_ASSERT(ID, isUnset(A.asElemRef()));

                mX.setFieldById(ID, CEB);
                LOOP_ASSERT(ID, !X.fieldById(ID).isNul2());
                LOOP_ASSERT(ID, CEB == X.fieldById(ID).asElemRef());

                Obj mB = X.fieldById(ID); const Obj& B = mB;
                LOOP_ASSERT(ID, !B.isNul2());
                LOOP_ASSERT(ID, CEB == B.asElemRef());
                LOOP_ASSERT(ID, CEB == X.fieldRefById(ID));

                mX.setFieldNullById(ID);
                LOOP_ASSERT(ID, X.fieldById(ID).isNul2());
                LOOP_ASSERT(ID, isUnset(X.fieldById(ID).asElemRef()));
                LOOP_ASSERT(ID, A.isNul2());
                LOOP_ASSERT(ID, isUnset(A.asElemRef()));
                LOOP_ASSERT(ID, B.isNul2());
                LOOP_ASSERT(ID, isUnset(B.asElemRef()));

                if (RecDef::BDEM_CHOICE_RECORD == r.recordType()) {
                    const int NULL_ID = BDEM_NULL_FIELD_ID;
                    mX.setFieldById(NULL_ID, CEA);
                    LOOP_ASSERT(ID, !X.fieldById(NULL_ID).isNul2());
                    LOOP_ASSERT(ID, CEA == X.fieldById(NULL_ID).asElemRef());

                    Obj mC = X.fieldById(NULL_ID); const Obj& C = mC;
                    LOOP_ASSERT(ID, !C.isNul2());
                    LOOP_ASSERT(ID, CEA == C.asElemRef());
                    LOOP_ASSERT(ID, CEA == X.fieldRefById(NULL_ID));

                    mX.setFieldNullById(NULL_ID);
                    LOOP_ASSERT(ID, X.fieldById(NULL_ID).isNul2());
                    LOOP_ASSERT(ID,
                                isUnset(X.fieldById(NULL_ID).asElemRef()));
                    LOOP_ASSERT(ID, C.isNul2());
                    LOOP_ASSERT(ID, isUnset(C.asElemRef()));
                }
            }

            // Test error conditions
            {
                const CERef CEA = getCERef(ET::BDEM_BOOL, 1);
                const CERef CEB = getCERef(ET::BDEM_BOOL, 2);

                ASSERT(ET::BDEM_VOID == X.fieldTypeById(r.numFields()));
                ASSERT(ET::BDEM_VOID == X.fieldRefById(r.numFields()).type());

                // Test condition when the fieldId is not found or is not
                // the selector
                ASSERT_AGG_ERROR(mX.setFieldById(r.numFields(), CEA),
                                 BCEM_ERR_TBD);

                ASSERT_AGG_ERROR(X.fieldById(r.numFields()), BCEM_ERR_TBD);

                // Test condition when aggregate is empty
                mX.reset();
                ASSERT_AGG_ERROR(mX.setFieldById(0, CEA), BCEM_ERR_TBD);

                ASSERT_AGG_ERROR(X.fieldById(0), BCEM_ERR_TBD);
            }
        }
}

static void testCase7(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // TESTING 'asElemRef' and related ACCESSORS:
        //
        // Concerns:
        //   - That the asElemRef accessors return the correct data
        //   - The const and non-const versions of 'asElemRef' return the
        //     correct type (bdem_ConstElemRef and bdem_ElemRef).
        //   - 'dataType' returns the correct data type
        //   - 'asString' returns a string representation of the value
        //
        // Plan:
        //   For each data type create two aggregates of that data type and
        //   different values.  Call the 'asElemRef' functions and verify that
        //   they return the correct data.  Additionally, we also call the
        //   const and non-const versions of 'asElemRef' and verify that
        //   const-correctness is maintained.  Call 'dataType' and 'asString'
        //   while testing 'asElemRef' and check that the work correctly.
        //
        // Testing:
        //   bdem_ElemRef asElemRef();
        //   bsl::string asString() const;
        //   bdem_ConstElemRef asElemRef() const;
        //   ET::Type dataType() const;
        // --------------------------------------------------------------------

        if (verbose) tst::cout << "\nTESTING 'asElemRef' and related ACCESSORS"
                               << "\n========================================="
                               << bsl::endl;

        if (veryVerbose) { T_ cout << "Testing for empty aggregates" << endl; }
        {
            Obj mX; const Obj& X = mX;
            ASSERT(X.isNul2());

            ASSERT(ET::BDEM_VOID == X.dataType());
            ASSERT(bsl::string() == X.asString());
        }

        if (veryVerbose) { T_ cout << "Testing for scalar aggregates"
                                   << endl; }
        {
            for (int i = 0; i < ET::BDEM_NUM_TYPES; ++i) {
                ET::Type    TYPE = (ET::Type) i;
                const CERef CER  = getCERef(TYPE, 1);

                Obj mX(TYPE, CER); const Obj& X = mX;
                LOOP_ASSERT(TYPE, !X.isNul2());
                LOOP_ASSERT(TYPE, TYPE == X.dataType());

                const bdem_ElemRef ER = X.asElemRef();
                LOOP_ASSERT(TYPE, CER == ER);

                X.makeNull();
                LOOP_ASSERT(TYPE, X.isNul2());
                LOOP_ASSERT(TYPE, ER.isNull());
                LOOP_ASSERT(TYPE, isUnset(ER));
            }
        }

        if (veryVerbose) { T_ cout << "Testing for aggregates" << endl; }
        {
            const struct {
                int         d_line;
                const char *d_spec;
            } DATA[] = {
                // Line         Spec
                // ----         ----
                // For List Aggregates
                {   L_,         ":aCa" },
                {   L_,         ":aCa&NT" },
                {   L_,         ":aCa&NF" },
                {   L_,         ":aCa&D0" },
                {   L_,         ":aFa" },
                {   L_,         ":aFa&NT" },
                {   L_,         ":aFa&NF" },
                {   L_,         ":aFa&D0" },
                {   L_,         ":aGa" },
                {   L_,         ":aGa&NT" },
                {   L_,         ":aGa&NF" },
                {   L_,         ":aGa&D0" },
                {   L_,         ":aHa"    },
                {   L_,         ":aHa&NT" },
                {   L_,         ":aHa&NF" },
                {   L_,         ":aHa&D0" },
                {   L_,         ":aNa"    },
                {   L_,         ":aNa&NT" },
                {   L_,         ":aNa&NF" },
                {   L_,         ":aNa&D0" },
                {   L_,         ":aNa&FN" },
                {   L_,         ":aPa" },
                {   L_,         ":aPa&NT" },
                {   L_,         ":aPa&NF" },
                {   L_,         ":aPa&D0" },
                {   L_,         ":aPa&FN" },
                {   L_,         ":aQa" },
                {   L_,         ":aQa&NT" },
                {   L_,         ":aQa&NF" },
                {   L_,         ":aQa&D0" },
                {   L_,         ":aQa&FN" },
                {   L_,         ":aRa" },
                {   L_,         ":aRa&NT" },
                {   L_,         ":aRa&NF" },
                {   L_,         ":aRa&D0" },
                {   L_,         ":aRa&FN" },
                {   L_,         ":aWa" },
                {   L_,         ":aWa&NT" },
                {   L_,         ":aWa&NF" },
                {   L_,         ":aWa&D0" },
                {   L_,         ":aaa&FN" },
                {   L_,         ":aVa" },
                {   L_,         ":aVa&NT" },
                {   L_,         ":aVa&NF" },
                {   L_,         ":afa" },
                {   L_,         ":afa&NT" },
                {   L_,         ":afa&NF" },

                {   L_,         ":b=tu5v :a$ab" },
                {   L_,         ":b=tu5v :a$ab&NT" },
                {   L_,         ":b=tu5v :a$ab&NF" },

                {   L_,         ":b=tu5v :a^ab" },
                {   L_,         ":b=tu5v :a^ab&NT" },
                {   L_,         ":b=tu5v :a^ab&NF" },

                {   L_,         ":b=tu5v :a!ab" },
                {   L_,         ":b=tu5v :a!ab&NT" },
                {   L_,         ":b=tu5v :a!ab&NF" },
                {   L_,         ":b=tu5v :a!ab&FN" },

                {   L_,         ":b=tu5v :a/ab" },
                {   L_,         ":b=tu5v :a/ab&NT" },
                {   L_,         ":b=tu5v :a/ab&NF" },
                {   L_,         ":b=tu5v :a/ab&FN" },

                {   L_,         ":aCbFcGdQf :g+ha" },
                {   L_,         ":aCbFcGdQf :g+ha&NT" },
                {   L_,         ":aCbFcGdQf :g+ha&NF" },

                {   L_,         ":aCbFcGdQf :g#ha" },
                {   L_,         ":aCbFcGdQf :g#ha&NT" },
                {   L_,         ":aCbFcGdQf :g#ha&NF" },

                {   L_,         ":a?CbFcGdQf :g%ha" },
                {   L_,         ":a?CbFcGdQf :g%ha&NT" },
                {   L_,         ":a?CbFcGdQf :g%ha&NF" },

                {   L_,         ":a?CbFcGdQf :g@ha" },
                {   L_,         ":a?CbFcGdQf :g@ha&NT" },
                {   L_,         ":a?CbFcGdQf :g@ha&NF" },

                // For Choice Aggregates
                {   L_,         ":a?Ca" },
                {   L_,         ":a?Ca&NT" },
                {   L_,         ":a?Ca&NF" },
                {   L_,         ":a?Ca&D0" },
                {   L_,         ":a?Fa" },
                {   L_,         ":a?Fa&NT" },
                {   L_,         ":a?Fa&NF" },
                {   L_,         ":a?Fa&D0" },
                {   L_,         ":a?Ga" },
                {   L_,         ":a?Ga&NT" },
                {   L_,         ":a?Ga&NF" },
                {   L_,         ":a?Ga&D0" },
                {   L_,         ":a?Ha" },
                {   L_,         ":a?Ha&NT" },
                {   L_,         ":a?Ha&NF" },
                {   L_,         ":a?Ha&D0" },
                {   L_,         ":a?Na" },
                {   L_,         ":a?Na&NT" },
                {   L_,         ":a?Na&NF" },
                {   L_,         ":a?Na&D0" },
                {   L_,         ":a?Pa" },
                {   L_,         ":a?Pa&NT" },
                {   L_,         ":a?Pa&NF" },
                {   L_,         ":a?Pa&D0" },
                {   L_,         ":a?Qa" },
                {   L_,         ":a?Qa&NT" },
                {   L_,         ":a?Qa&NF" },
                {   L_,         ":a?Qa&D0" },
                {   L_,         ":a?Ra" },
                {   L_,         ":a?Ra&NT" },
                {   L_,         ":a?Ra&NF" },
                {   L_,         ":a?Ra&D0" },
                {   L_,         ":a?Wa" },
                {   L_,         ":a?Wa&NT" },
                {   L_,         ":a?Wa&NF" },
                {   L_,         ":a?Wa&D0" },
                {   L_,         ":a?Va" },
                {   L_,         ":a?Va&NT" },
                {   L_,         ":a?Va&NF" },
                {   L_,         ":a?fa" },
                {   L_,         ":a?fa&NT" },
                {   L_,         ":a?fa&NF" },

                {   L_,         ":aCbFcGdQf :g?+ha" },
                {   L_,         ":aCbFcGdQf :g?+ha&NT" },
                {   L_,         ":aCbFcGdQf :g?+ha&NF" },

                {   L_,         ":aCbFcGdQf :g?#ha" },
                {   L_,         ":aCbFcGdQf :g?#ha&NT" },
                {   L_,         ":aCbFcGdQf :g?#ha&NF" },

                {   L_,         ":a?CbFcGdQf :g?%ha"    },
                {   L_,         ":a?CbFcGdQf :g?%ha&NT" },
                {   L_,         ":a?CbFcGdQf :g?%ha&NF" },

                {   L_,         ":a?CbFcGdQf :g?@ha" },
                {   L_,         ":a?CbFcGdQf :g?@ha&NT" },
                {   L_,         ":a?CbFcGdQf :g?@ha&NF" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const char *SPEC = DATA[i].d_spec;
                const bool  NSA  = (bool) bsl::strstr(SPEC, "&FN");

                Schema s; ggSchema(&s, SPEC);
                const RecDef *r = NSA
                                ? &s.record(0)
                                : &s.record(s.numRecords() - 1);
                ConstRecDefShdPtr rp(r, NilDeleter(), 0);

                if (veryVerbose) { T_ P(s) };

                bslma_TestAllocator t(veryVeryVerbose);
                Obj mX(rp, &t); const Obj& X = mX;
                ASSERT(rp == X.recordDefPtr());

                if (veryVerbose) { T_ P(X) };

                const char *fldName = r->fieldName(0);
                ET::Type    TYPE    = NSA
                             ? ET::toArrayType(s.record(1).field(0).elemType())
                             : r->field(0).elemType();

                const CERef CER     = getCERef(TYPE, 1);

                if (veryVerbose) { T_ T_ P(s) P(CER) P(X) }

                if (RecDef::BDEM_CHOICE_RECORD == r->recordType()) {
                    mX.makeSelection(fldName);
                }

                LOOP_ASSERT(LINE, !X.isNul2());
                const bdem_ElemRef ERX = X.asElemRef();
                LOOP_ASSERT(LINE, !ERX.isNull());

                bcem_Aggregate mA = X.field(fldName);
                const bcem_Aggregate& A = mA;

                const bdem_ElemRef ERA = A.asElemRef();

                X.setField(fldName, CER);
                LOOP_ASSERT(LINE, !X.isNul2());
                LOOP_ASSERT(LINE, !A.isNul2());
                LOOP_ASSERT(LINE, !ERX.isNull());
                LOOP_ASSERT(LINE, !ERA.isNull());
                if (NSA) {
                    ASSERT(ET::BDEM_TABLE == ERA.type());
                    ASSERT(compareNillableTable(X.field(fldName), CER));
                }
                else {
                    LOOP_ASSERT(LINE, CER == ERA);
                }

                X.setFieldNull(fldName);
                LOOP_ASSERT(LINE, !X.isNul2());
                LOOP_ASSERT(LINE,  A.isNul2());
                LOOP_ASSERT(LINE, !ERX.isNull());
                LOOP_ASSERT(LINE,  ERA.isNull());
                LOOP_ASSERT(LINE,  isUnset(ERA));

                X.makeNull();
                LOOP_ASSERT(LINE, X.isNul2());
                LOOP_ASSERT(LINE, ERX.isNull());
                LOOP_ASSERT(LINE, isUnset(ERX));
            }
        }

        if (veryVerbose) { T_ cout << "Testing const-correctness of asElemRef"
                                   << " functions" << endl; }
        {
            const CERef VA = getCERef(ET::BDEM_BOOL, 1);
            Obj mX(ET::BDEM_BOOL, VA); const Obj& X = mX;

            ERef  EA  = mX.asElemRef();
            CERef CEA = X.asElemRef();
            ASSERT(VA == EA);
            ASSERT(VA == CEA);
        }
}

static void testCase6(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // TESTING 'asXXX' ACCESSORS:
        //
        // Concerns:
        //   - That the asXXX accessors return the correct data
        //   - If the type of the aggregate is not the same as the
        //     corresponding asXXX function then an error is printed on stderr
        //
        // Plan:
        //   Each 'asXXX' function internally calls the private function
        //   'convertScalar' with the appropriate template parameter and
        //   'convertScalar' itself just calls 'bdem_Convert::fromBdemType'.
        //   So our aim in this test is to confirm that the function calls
        //   'convertScalar' function with the correct template parameter and
        //   that 'convertScalar' itself passes the corrects arguments to
        //   'bdem_Convert::fromBdemType'.
        //   For that we construct two Aggregate objects of each type, one set
        //   to some value and the other set to the null value for that type.
        //   Then we call the corresponding 'asXXX' function on each object
        //   and verify that it returns the correct data.  Finally, we verify
        //   that invalid conversions output the error message.  Note that we
        //   will test the error message only for a couple of cases.
        //
        // Testing:
        //   bool asBool() const;
        //   char asChar() const;
        //   short asShort() const;
        //   int asInt() const;
        //   bsls_PlatformUtil::Int64 asInt64() const;
        //   float asFloat() const;
        //   double asDouble() const;
        //   bdet_Datetime asDatetime() const;
        //   bdet_DatetimeTz asDatetimeTz() const;
        //   bdet_Date asDate() const;
        //   bdet_DateTz asDateTz() const;
        //   bdet_Time asTime() const;
        //   bdet_TimeTz asTimeTz() const;
        //
        // Private functions:
        // template <typename TOTYPE>
        // inline
        // TOTYPE convertScalar() const
        // --------------------------------------------------------------------

        if (verbose) tst::cout << "\nTESTING 'asXXX' ACCESSORS"
                               << "\n=========================" << bsl::endl;

        if (veryVerbose) { T_ cout << "Testing for BOOL" << endl; }
        {
            const CERef VN = getCERef(ET::BDEM_BOOL, 0);
            const CERef VA = getCERef(ET::BDEM_BOOL, 1);
            const CERef VB = getCERef(ET::BDEM_BOOL, 2);

            Obj mX(ET::BDEM_BOOL, VA); const Obj& X = mX;
            Obj mY(ET::BDEM_BOOL, VB); const Obj& Y = mY;

            ASSERT(VA.theBool() == X.asBool());
            ASSERT(VA           == X.asElemRef());
            ASSERT(!X.isNul2());

            X.makeNull();
            ASSERT(X.isNul2());
            ASSERT(X.asElemRef().isNull());
            ASSERT(VN.theBool() == X.asBool());

            ASSERT(VB.theBool() == Y.asBool());
            ASSERT(VB           == Y.asElemRef());
            ASSERT(!Y.isNul2());

            Y.makeNull();
            ASSERT(Y.isNul2());
            ASSERT(Y.asElemRef().isNull());
            ASSERT(VN.theBool() == Y.asBool());

            // nillable bool array
            const char *SPEC = ":aaa&FN";
            Schema schema; const Schema& SCHEMA = schema;
            ggSchema(&schema, SPEC);

            const RecDef *RECORD  = &SCHEMA.record(0);
            const char   *fldName = RECORD->fieldName(0);

            ConstRecDefShdPtr crp(&SCHEMA.record(0), NilDeleter(), 0);
            const ConstRecDefShdPtr& CRP = crp;

            Obj mA(CRP); const Obj& A = mA;
            Obj mB = mA.field(fldName); const Obj& B = mB;
            const int NE = 3;
            mB.resize(NE);
            for (int i = 0; i < NE; ++i) {
                LOOP_ASSERT(i, B.field(i).isNul2());
                LOOP_ASSERT(i, VN.theBool() == B.field(i).asBool());
                LOOP_ASSERT(i, VN.theBool() ==
                                             B.field(i).asElemRef().theBool());
                LOOP3_ASSERT(i, B.field(i), B.field(i).asElemRef(),
                             B.field(i).asElemRef().isNull());
                LOOP3_ASSERT(i, B.field(i), B.field(i).asElemRef(),
                             isUnset(B.field(i).asElemRef()));
            }

            mB.setField(0, VA);
            ASSERT(VA.theBool() == B.field(0).asBool());
            ASSERT(VA           == B.field(0).asElemRef());
            ASSERT(!B.field(0).isNul2());

            mB.setFieldNull(0);
            ASSERT(VN.theBool() == B.field(0).asBool());
            ASSERT(B.field(0).asElemRef().isNull());
            ASSERT(B.field(0).isNul2());

            mA.setField(fldName, 1, VB);
            ASSERT(VB.theBool() == A.field(fldName, 1).asBool());
            LOOP2_ASSERT(VB, A.field(fldName, 1).asElemRef(),
                         VB           == A.field(fldName, 1).asElemRef());
            ASSERT(!A.field(fldName, 1).isNul2());

            mA.setFieldNull(fldName, 1);
            ASSERT(VN.theBool() == A.field(fldName, 1).asBool());
            ASSERT(A.field(fldName, 1).asElemRef().isNull());
            ASSERT(A.field(fldName, 1).isNul2());

            Obj mC = B.field(2); const Obj& C = mC;
            mC.setValue(VA);
            ASSERT(VA.theBool() == C.asBool());
            ASSERT(VA           == C.asElemRef());
            ASSERT(!C.isNul2());

            mC.makeNull();
            ASSERT(VN.theBool() == C.asBool());
            ASSERT(C.asElemRef().isNull());
            ASSERT(C.isNul2());
        }

        if (veryVerbose) { T_ cout << "Testing for CHAR" << endl; }
        {
            const CERef VN = getCERef(ET::BDEM_CHAR, 0);
            const CERef VA = getCERef(ET::BDEM_CHAR, 1);
            const CERef VB = getCERef(ET::BDEM_CHAR, 2);

            Obj mX(ET::BDEM_CHAR, VA); const Obj& X = mX;
            Obj mY(ET::BDEM_CHAR, VB); const Obj& Y = mY;

            ASSERT(VA.theChar() == X.asChar());
            ASSERT(VA           == X.asElemRef());
            ASSERT(!X.isNul2());

            X.makeNull();
            ASSERT(X.isNul2());
            ASSERT(X.asElemRef().isNull());
            ASSERT(VN.theChar() == X.asChar());

            ASSERT(VB.theChar() == Y.asChar());
            ASSERT(VB           == Y.asElemRef());
            ASSERT(!Y.isNul2());

            Y.makeNull();
            ASSERT(Y.isNul2());
            ASSERT(Y.asElemRef().isNull());
            ASSERT(VN.theChar() == Y.asChar());

            // nillable char array
            const char *SPEC = ":aKa&FN";
            Schema schema; const Schema& SCHEMA = schema;
            ggSchema(&schema, SPEC);

            const RecDef *RECORD  = &SCHEMA.record(0);
            const char   *fldName = RECORD->fieldName(0);

            ConstRecDefShdPtr crp(&SCHEMA.record(0), NilDeleter(), 0);
            const ConstRecDefShdPtr& CRP = crp;

            Obj mA(CRP); const Obj& A = mA;
            Obj mB = mA.field(fldName); const Obj& B = mB;
            const int NE = 3;
            mB.resize(NE);
            for (int i = 0; i < NE; ++i) {
                LOOP_ASSERT(i, B.field(i).isNul2());
                LOOP_ASSERT(i, VN.theChar() == B.field(i).asChar());
                LOOP_ASSERT(i, VN.theChar() ==
                                             B.field(i).asElemRef().theChar());
                LOOP_ASSERT(i, B.field(i).asElemRef().isNull());
                LOOP_ASSERT(i, isUnset(B.field(i).asElemRef()));
            }

            mB.setField(0, VA);
            ASSERT(VA.theChar() == B.field(0).asChar());
            ASSERT(VA           == B.field(0).asElemRef());
            ASSERT(!B.field(0).isNul2());

            mB.setFieldNull(0);
            ASSERT(VN.theChar() == B.field(0).asChar());
            ASSERT(B.field(0).asElemRef().isNull());
            ASSERT(B.field(0).isNul2());

            mA.setField(fldName, 1, VB);
            LOOP2_ASSERT(VB.theChar(), A.field(fldName, 1).asChar(),
                         VB.theChar() == A.field(fldName, 1).asChar());
            ASSERT(VB           == A.field(fldName, 1).asElemRef());
            ASSERT(!A.field(fldName, 1).isNul2());

            mA.setFieldNull(fldName, 1);
            ASSERT(VN.theChar() == A.field(fldName, 1).asChar());
            ASSERT(A.field(fldName, 1).asElemRef().isNull());
            ASSERT(A.field(fldName, 1).isNul2());

            Obj mC = B.field(2); const Obj& C = mC;
            mC.setValue(VA);
            ASSERT(VA.theChar() == C.asChar());
            ASSERT(VA           == C.asElemRef());
            ASSERT(!C.isNul2());

            mC.makeNull();
            ASSERT(VN.theChar() == C.asChar());
            ASSERT(C.asElemRef().isNull());
            ASSERT(C.isNul2());
        }

        if (veryVerbose) { T_ cout << "Testing for SHORT" << endl; }
        {
            const CERef VN = getCERef(ET::BDEM_SHORT, 0);
            const CERef VA = getCERef(ET::BDEM_SHORT, 1);
            const CERef VB = getCERef(ET::BDEM_SHORT, 2);

            Obj mX(ET::BDEM_SHORT, VA); const Obj& X = mX;
            Obj mY(ET::BDEM_SHORT, VB); const Obj& Y = mY;

            ASSERT(VA.theShort() == X.asShort());
            ASSERT(VA            == X.asElemRef());
            ASSERT(!X.isNul2());

            X.makeNull();
            ASSERT(X.isNul2());
            ASSERT(X.asElemRef().isNull());
            ASSERT(VN.theShort() == X.asShort());

            ASSERT(VB.theShort() == Y.asShort());
            ASSERT(VB            == Y.asElemRef());
            ASSERT(!Y.isNul2());

            Y.makeNull();
            ASSERT(Y.isNul2());
            ASSERT(Y.asElemRef().isNull());
            ASSERT(VN.theShort() == Y.asShort());

            // nillable short array
            const char *SPEC = ":aLa&FN";
            Schema schema; const Schema& SCHEMA = schema;
            ggSchema(&schema, SPEC);

            const RecDef *RECORD  = &SCHEMA.record(0);
            const char   *fldName = RECORD->fieldName(0);

            ConstRecDefShdPtr crp(&SCHEMA.record(0), NilDeleter(), 0);
            const ConstRecDefShdPtr& CRP = crp;

            Obj mA(CRP); const Obj& A = mA;
            Obj mB = mA.field(fldName); const Obj& B = mB;
            const int NE = 3;
            mB.resize(NE);
            for (int i = 0; i < NE; ++i) {
                LOOP_ASSERT(i, B.field(i).isNul2());
                LOOP_ASSERT(i, VN.theShort() == B.field(i).asShort());
                LOOP_ASSERT(i, VN.theShort() ==
                                            B.field(i).asElemRef().theShort());
                LOOP_ASSERT(i, B.field(i).asElemRef().isNull());
                LOOP_ASSERT(i, isUnset(B.field(i).asElemRef()));
            }

            mB.setField(0, VA);
            ASSERT(VA.theShort() == B.field(0).asShort());
            ASSERT(VA            == B.field(0).asElemRef());
            ASSERT(!B.field(0).isNul2());

            mB.setFieldNull(0);
            ASSERT(VN.theShort() == B.field(0).asShort());
            ASSERT(B.field(0).asElemRef().isNull());
            ASSERT(B.field(0).isNul2());

            mA.setField(fldName, 1, VB);
            ASSERT(VB.theShort() == A.field(fldName, 1).asShort());
            ASSERT(VB            == A.field(fldName, 1).asElemRef());
            ASSERT(!A.field(fldName, 1).isNul2());

            mA.setFieldNull(fldName, 1);
            ASSERT(VN.theShort() == A.field(fldName, 1).asShort());
            ASSERT(A.field(fldName, 1).asElemRef().isNull());
            ASSERT(A.field(fldName, 1).isNul2());

            Obj mC = B.field(2); const Obj& C = mC;
            mC.setValue(VA);
            ASSERT(VA.theShort() == C.asShort());
            ASSERT(VA            == C.asElemRef());
            ASSERT(!C.isNul2());

            mC.makeNull();
            ASSERT(VN.theShort() == C.asShort());
            ASSERT(C.asElemRef().isNull());
            ASSERT(C.isNul2());
        }

        if (veryVerbose) { T_ cout << "Testing for INT" << endl; }
        {
            const CERef VN = getCERef(ET::BDEM_INT, 0);
            const CERef VA = getCERef(ET::BDEM_INT, 1);
            const CERef VB = getCERef(ET::BDEM_INT, 2);

            Obj mX(ET::BDEM_INT, VA); const Obj& X = mX;
            Obj mY(ET::BDEM_INT, VB); const Obj& Y = mY;

            ASSERT(VA.theInt() == X.asInt());
            ASSERT(VA          == X.asElemRef());
            ASSERT(!X.isNul2());

            X.makeNull();
            ASSERT(X.isNul2());
            ASSERT(X.asElemRef().isNull());
            ASSERT(VN.theInt() == X.asInt());

            ASSERT(VB.theInt() == Y.asInt());
            ASSERT(VB          == Y.asElemRef());
            ASSERT(!Y.isNul2());

            Y.makeNull();
            ASSERT(Y.isNul2());
            ASSERT(Y.asElemRef().isNull());
            ASSERT(VN.theInt() == Y.asInt());

            // nillable int array
            const char *SPEC = ":aMa&FN";
            Schema schema; const Schema& SCHEMA = schema;
            ggSchema(&schema, SPEC);

            const RecDef *RECORD  = &SCHEMA.record(0);
            const char   *fldName = RECORD->fieldName(0);

            ConstRecDefShdPtr crp(&SCHEMA.record(0), NilDeleter(), 0);
            const ConstRecDefShdPtr& CRP = crp;

            Obj mA(CRP); const Obj& A = mA;
            Obj mB = mA.field(fldName); const Obj& B = mB;
            const int NE = 3;
            mB.resize(NE);
            for (int i = 0; i < NE; ++i) {
                LOOP_ASSERT(i, B.field(i).isNul2());
                LOOP_ASSERT(i, VN.theInt() == B.field(i).asInt());
                LOOP_ASSERT(i, VN.theInt() == B.field(i).asElemRef().theInt());
                LOOP_ASSERT(i, B.field(i).asElemRef().isNull());
                LOOP_ASSERT(i, isUnset(B.field(i).asElemRef()));
            }

            mB.setField(0, VA);
            ASSERT(VA.theInt() == B.field(0).asInt());
            ASSERT(VA          == B.field(0).asElemRef());
            ASSERT(!B.field(0).isNul2());

            mB.setFieldNull(0);
            ASSERT(VN.theInt() == B.field(0).asInt());
            ASSERT(B.field(0).asElemRef().isNull());
            ASSERT(B.field(0).isNul2());

            mA.setField(fldName, 1, VB);
            ASSERT(VB.theInt() == A.field(fldName, 1).asInt());
            ASSERT(VB          == A.field(fldName, 1).asElemRef());
            ASSERT(!A.field(fldName, 1).isNul2());

            mA.setFieldNull(fldName, 1);
            ASSERT(VN.theInt() == A.field(fldName, 1).asInt());
            ASSERT(A.field(fldName, 1).asElemRef().isNull());
            ASSERT(A.field(fldName, 1).isNul2());

            Obj mC = B.field(2); const Obj& C = mC;
            mC.setValue(VA);
            ASSERT(VA.theInt() == C.asInt());
            ASSERT(VA          == C.asElemRef());
            ASSERT(!C.isNul2());

            mC.makeNull();
            ASSERT(VN.theInt() == C.asInt());
            ASSERT(C.asElemRef().isNull());
            ASSERT(C.isNul2());
        }

        if (veryVerbose) { T_ cout << "Testing for INT64" << endl; }
        {
            const CERef VN = getCERef(ET::BDEM_INT64, 0);
            const CERef VA = getCERef(ET::BDEM_INT64, 1);
            const CERef VB = getCERef(ET::BDEM_INT64, 2);

            Obj mX(ET::BDEM_INT64, VA); const Obj& X = mX;
            Obj mY(ET::BDEM_INT64, VB); const Obj& Y = mY;

            ASSERT(VA.theInt64() == X.asInt64());
            ASSERT(VA            == X.asElemRef());
            ASSERT(!X.isNul2());

            X.makeNull();
            ASSERT(X.isNul2());
            ASSERT(X.asElemRef().isNull());
            ASSERT(VN.theInt64() == X.asInt64());

            ASSERT(VB.theInt64() == Y.asInt64());
            ASSERT(VB            == Y.asElemRef());
            ASSERT(!Y.isNul2());

            Y.makeNull();
            ASSERT(Y.isNul2());
            ASSERT(Y.asElemRef().isNull());
            ASSERT(VN.theInt64() == Y.asInt64());

            // nillable int64 array
            const char *SPEC = ":aNa&FN";
            Schema schema; const Schema& SCHEMA = schema;
            ggSchema(&schema, SPEC);

            const RecDef *RECORD  = &SCHEMA.record(0);
            const char   *fldName = RECORD->fieldName(0);

            ConstRecDefShdPtr crp(&SCHEMA.record(0), NilDeleter(), 0);
            const ConstRecDefShdPtr& CRP = crp;

            Obj mA(CRP); const Obj& A = mA;
            Obj mB = mA.field(fldName); const Obj& B = mB;
            const int NE = 3;
            mB.resize(NE);
            for (int i = 0; i < NE; ++i) {
                LOOP_ASSERT(i, B.field(i).isNul2());
                LOOP_ASSERT(i, VN.theInt64() == B.field(i).asInt64());
                LOOP_ASSERT(i, VN.theInt64() ==
                                            B.field(i).asElemRef().theInt64());
                LOOP_ASSERT(i, B.field(i).asElemRef().isNull());
                LOOP_ASSERT(i, isUnset(B.field(i).asElemRef()));
            }

            mB.setField(0, VA);
            ASSERT(VA.theInt64() == B.field(0).asInt64());
            ASSERT(VA            == B.field(0).asElemRef());
            ASSERT(!B.field(0).isNul2());

            mB.setFieldNull(0);
            ASSERT(VN.theInt64() == B.field(0).asInt64());
            ASSERT(B.field(0).asElemRef().isNull());
            ASSERT(B.field(0).isNul2());

            mA.setField(fldName, 1, VB);
            ASSERT(VB.theInt64() == A.field(fldName, 1).asInt64());
            ASSERT(VB            == A.field(fldName, 1).asElemRef());
            ASSERT(!A.field(fldName, 1).isNul2());

            mA.setFieldNull(fldName, 1);
            ASSERT(VN.theInt64() == A.field(fldName, 1).asInt64());
            ASSERT(A.field(fldName, 1).asElemRef().isNull());
            ASSERT(A.field(fldName, 1).isNul2());

            Obj mC = B.field(2); const Obj& C = mC;
            mC.setValue(VA);
            ASSERT(VA.theInt64() == C.asInt64());
            ASSERT(VA            == C.asElemRef());
            ASSERT(!C.isNul2());

            mC.makeNull();
            ASSERT(VN.theInt64() == C.asInt64());
            ASSERT(C.asElemRef().isNull());
            ASSERT(C.isNul2());
        }

        if (veryVerbose) { T_ cout << "Testing for FLOAT" << endl; }
        {
            const CERef VN = getCERef(ET::BDEM_FLOAT, 0);
            const CERef VA = getCERef(ET::BDEM_FLOAT, 1);
            const CERef VB = getCERef(ET::BDEM_FLOAT, 2);

            Obj mX(ET::BDEM_FLOAT, VA); const Obj& X = mX;
            Obj mY(ET::BDEM_FLOAT, VB); const Obj& Y = mY;

            ASSERT(VA.theFloat() == X.asFloat());
            ASSERT(VA            == X.asElemRef());
            ASSERT(!X.isNul2());

            X.makeNull();
            ASSERT(X.isNul2());
            ASSERT(X.asElemRef().isNull());
            ASSERT(VN.theFloat() == X.asFloat());

            ASSERT(VB.theFloat() == Y.asFloat());
            ASSERT(VB            == Y.asElemRef());
            ASSERT(!Y.isNul2());

            Y.makeNull();
            ASSERT(Y.isNul2());
            ASSERT(Y.asElemRef().isNull());
            ASSERT(VN.theFloat() == Y.asFloat());

            // nillable float array
            const char *SPEC = ":aOa&FN";
            Schema schema; const Schema& SCHEMA = schema;
            ggSchema(&schema, SPEC);

            const RecDef *RECORD  = &SCHEMA.record(0);
            const char   *fldName = RECORD->fieldName(0);

            ConstRecDefShdPtr crp(&SCHEMA.record(0), NilDeleter(), 0);
            const ConstRecDefShdPtr& CRP = crp;

            Obj mA(CRP); const Obj& A = mA;
            Obj mB = mA.field(fldName); const Obj& B = mB;
            const int NE = 3;
            mB.resize(NE);
            for (int i = 0; i < NE; ++i) {
                LOOP_ASSERT(i, B.field(i).isNul2());
                LOOP_ASSERT(i, VN.theFloat() == B.field(i).asFloat());
                LOOP_ASSERT(i, VN.theFloat() ==
                                            B.field(i).asElemRef().theFloat());
                LOOP_ASSERT(i, B.field(i).asElemRef().isNull());
                LOOP_ASSERT(i, isUnset(B.field(i).asElemRef()));
            }

            mB.setField(0, VA);
            ASSERT(VA.theFloat() == B.field(0).asFloat());
            ASSERT(VA            == B.field(0).asElemRef());
            ASSERT(!B.field(0).isNul2());

            mB.setFieldNull(0);
            ASSERT(VN.theFloat() == B.field(0).asFloat());
            ASSERT(B.field(0).asElemRef().isNull());
            ASSERT(B.field(0).isNul2());

            mA.setField(fldName, 1, VB);
            ASSERT(VB.theFloat() == A.field(fldName, 1).asFloat());
            ASSERT(VB            == A.field(fldName, 1).asElemRef());
            ASSERT(!A.field(fldName, 1).isNul2());

            mA.setFieldNull(fldName, 1);
            ASSERT(VN.theFloat() == A.field(fldName, 1).asFloat());
            ASSERT(A.field(fldName, 1).asElemRef().isNull());
            ASSERT(A.field(fldName, 1).isNul2());

            Obj mC = B.field(2); const Obj& C = mC;
            mC.setValue(VA);
            ASSERT(VA.theFloat() == C.asFloat());
            ASSERT(VA            == C.asElemRef());
            ASSERT(!C.isNul2());

            mC.makeNull();
            ASSERT(VN.theFloat() == C.asFloat());
            ASSERT(C.asElemRef().isNull());
            ASSERT(C.isNul2());
        }

        if (veryVerbose) { T_ cout << "Testing for DOUBLE" << endl; }
        {
            const CERef VN = getCERef(ET::BDEM_DOUBLE, 0);
            const CERef VA = getCERef(ET::BDEM_DOUBLE, 1);
            const CERef VB = getCERef(ET::BDEM_DOUBLE, 2);

            Obj mX(ET::BDEM_DOUBLE, VA); const Obj& X = mX;
            Obj mY(ET::BDEM_DOUBLE, VB); const Obj& Y = mY;

            ASSERT(VA.theDouble() == X.asDouble());
            ASSERT(VA           == X.asElemRef());
            ASSERT(!X.isNul2());

            X.makeNull();
            ASSERT(X.isNul2());
            ASSERT(X.asElemRef().isNull());
            ASSERT(VN.theDouble() == X.asDouble());

            ASSERT(VB.theDouble() == Y.asDouble());
            ASSERT(VB             == Y.asElemRef());
            ASSERT(!Y.isNul2());

            Y.makeNull();
            ASSERT(Y.isNul2());
            ASSERT(Y.asElemRef().isNull());
            ASSERT(VN.theDouble() == Y.asDouble());

            // nillable double array
            const char *SPEC = ":aPa&FN";
            Schema schema; const Schema& SCHEMA = schema;
            ggSchema(&schema, SPEC);

            const RecDef *RECORD  = &SCHEMA.record(0);
            const char   *fldName = RECORD->fieldName(0);

            ConstRecDefShdPtr crp(&SCHEMA.record(0), NilDeleter(), 0);
            const ConstRecDefShdPtr& CRP = crp;

            Obj mA(CRP); const Obj& A = mA;
            Obj mB = mA.field(fldName); const Obj& B = mB;
            const int NE = 3;
            mB.resize(NE);
            for (int i = 0; i < NE; ++i) {
                LOOP_ASSERT(i, B.field(i).isNul2());
                LOOP_ASSERT(i, VN.theDouble() == B.field(i).asDouble());
                LOOP_ASSERT(i, VN.theDouble() ==
                                           B.field(i).asElemRef().theDouble());
                LOOP_ASSERT(i, B.field(i).asElemRef().isNull());
                LOOP_ASSERT(i, isUnset(B.field(i).asElemRef()));
            }

            mB.setField(0, VA);
            ASSERT(VA.theDouble() == B.field(0).asDouble());
            ASSERT(VA             == B.field(0).asElemRef());
            ASSERT(!B.field(0).isNul2());

            mB.setFieldNull(0);
            ASSERT(VN.theDouble() == B.field(0).asDouble());
            ASSERT(B.field(0).asElemRef().isNull());
            ASSERT(B.field(0).isNul2());

            mA.setField(fldName, 1, VB);
            ASSERT(VB.theDouble() == A.field(fldName, 1).asDouble());
            ASSERT(VB             == A.field(fldName, 1).asElemRef());
            ASSERT(!A.field(fldName, 1).isNul2());

            mA.setFieldNull(fldName, 1);
            ASSERT(VN.theDouble() == A.field(fldName, 1).asDouble());
            ASSERT(A.field(fldName, 1).asElemRef().isNull());
            ASSERT(A.field(fldName, 1).isNul2());

            Obj mC = B.field(2); const Obj& C = mC;
            mC.setValue(VA);
            ASSERT(VA.theDouble() == C.asDouble());
            ASSERT(VA             == C.asElemRef());
            ASSERT(!C.isNul2());

            mC.makeNull();
            ASSERT(VN.theDouble() == C.asDouble());
            ASSERT(C.asElemRef().isNull());
            ASSERT(C.isNul2());
        }

        if (veryVerbose) { T_ cout << "Testing for DATETIME" << endl; }
        {
            const CERef VN = getCERef(ET::BDEM_DATETIME, 0);
            const CERef VA = getCERef(ET::BDEM_DATETIME, 1);
            const CERef VB = getCERef(ET::BDEM_DATETIME, 2);

            Obj mX(ET::BDEM_DATETIME, VA); const Obj& X = mX;
            Obj mY(ET::BDEM_DATETIME, VB); const Obj& Y = mY;

            ASSERT(VA.theDatetime() == X.asDatetime());
            ASSERT(VA               == X.asElemRef());
            ASSERT(!X.isNul2());

            X.makeNull();
            ASSERT(X.isNul2());
            ASSERT(X.asElemRef().isNull());
            ASSERT(VN.theDatetime() == X.asDatetime());

            ASSERT(VB.theDatetime() == Y.asDatetime());
            ASSERT(VB               == Y.asElemRef());
            ASSERT(!Y.isNul2());

            Y.makeNull();
            ASSERT(Y.isNul2());
            ASSERT(Y.asElemRef().isNull());
            ASSERT(VN.theDatetime() == Y.asDatetime());

            // nillable datetime array
            const char *SPEC = ":aRa&FN";
            Schema schema; const Schema& SCHEMA = schema;
            ggSchema(&schema, SPEC);

            const RecDef *RECORD  = &SCHEMA.record(0);
            const char   *fldName = RECORD->fieldName(0);

            ConstRecDefShdPtr crp(&SCHEMA.record(0), NilDeleter(), 0);
            const ConstRecDefShdPtr& CRP = crp;

            Obj mA(CRP); const Obj& A = mA;
            Obj mB = mA.field(fldName); const Obj& B = mB;
            const int NE = 3;
            mB.resize(NE);
            for (int i = 0; i < NE; ++i) {
                LOOP_ASSERT(i, B.field(i).isNul2());
                LOOP_ASSERT(i, VN.theDatetime() == B.field(i).asDatetime());
                LOOP_ASSERT(i, VN.theDatetime() ==
                                         B.field(i).asElemRef().theDatetime());
                LOOP_ASSERT(i, B.field(i).asElemRef().isNull());
                LOOP_ASSERT(i, isUnset(B.field(i).asElemRef()));
            }

            mB.setField(0, VA);
            ASSERT(VA.theDatetime() == B.field(0).asDatetime());
            ASSERT(VA               == B.field(0).asElemRef());
            ASSERT(!B.field(0).isNul2());

            mB.setFieldNull(0);
            ASSERT(VN.theDatetime() == B.field(0).asDatetime());
            ASSERT(B.field(0).asElemRef().isNull());
            ASSERT(B.field(0).isNul2());

            mA.setField(fldName, 1, VB);
            ASSERT(VB.theDatetime() == A.field(fldName, 1).asDatetime());
            ASSERT(VB               == A.field(fldName, 1).asElemRef());
            ASSERT(!A.field(fldName, 1).isNul2());

            mA.setFieldNull(fldName, 1);
            ASSERT(VN.theDatetime() == A.field(fldName, 1).asDatetime());
            ASSERT(A.field(fldName, 1).asElemRef().isNull());
            ASSERT(A.field(fldName, 1).isNul2());

            Obj mC = B.field(2); const Obj& C = mC;
            mC.setValue(VA);
            ASSERT(VA.theDatetime() == C.asDatetime());
            ASSERT(VA               == C.asElemRef());
            ASSERT(!C.isNul2());

            mC.makeNull();
            ASSERT(VN.theDatetime() == C.asDatetime());
            ASSERT(C.asElemRef().isNull());
            ASSERT(C.isNul2());
        }

        if (veryVerbose) { T_ cout << "Testing for DATE" << endl; }
        {
            const CERef VN = getCERef(ET::BDEM_DATE, 0);
            const CERef VA = getCERef(ET::BDEM_DATE, 1);
            const CERef VB = getCERef(ET::BDEM_DATE, 2);

            Obj mX(ET::BDEM_DATE, VA); const Obj& X = mX;
            Obj mY(ET::BDEM_DATE, VB); const Obj& Y = mY;

            ASSERT(VA.theDate() == X.asDate());
            ASSERT(VA           == X.asElemRef());
            ASSERT(!X.isNul2());

            X.makeNull();
            ASSERT(X.isNul2());
            ASSERT(X.asElemRef().isNull());
            ASSERT(VN.theDate() == X.asDate());

            ASSERT(VB.theDate() == Y.asDate());
            ASSERT(VB           == Y.asElemRef());
            ASSERT(!Y.isNul2());

            Y.makeNull();
            ASSERT(Y.isNul2());
            ASSERT(Y.asElemRef().isNull());
            ASSERT(VN.theDate() == Y.asDate());

            // nillable date array
            const char *SPEC = ":aSa&FN";
            Schema schema; const Schema& SCHEMA = schema;
            ggSchema(&schema, SPEC);

            const RecDef *RECORD  = &SCHEMA.record(0);
            const char   *fldName = RECORD->fieldName(0);

            ConstRecDefShdPtr crp(&SCHEMA.record(0), NilDeleter(), 0);
            const ConstRecDefShdPtr& CRP = crp;

            Obj mA(CRP); const Obj& A = mA;
            Obj mB = mA.field(fldName); const Obj& B = mB;
            const int NE = 3;
            mB.resize(NE);
            for (int i = 0; i < NE; ++i) {
                LOOP_ASSERT(i, B.field(i).isNul2());
                LOOP_ASSERT(i, VN.theDate() == B.field(i).asDate());
                LOOP_ASSERT(i, VN.theDate() ==
                                             B.field(i).asElemRef().theDate());
                LOOP_ASSERT(i, B.field(i).asElemRef().isNull());
                LOOP_ASSERT(i, isUnset(B.field(i).asElemRef()));
            }

            mB.setField(0, VA);
            ASSERT(VA.theDate() == B.field(0).asDate());
            ASSERT(VA           == B.field(0).asElemRef());
            ASSERT(!B.field(0).isNul2());

            mB.setFieldNull(0);
            ASSERT(VN.theDate() == B.field(0).asDate());
            ASSERT(B.field(0).asElemRef().isNull());
            ASSERT(B.field(0).isNul2());

            mA.setField(fldName, 1, VB);
            ASSERT(VB.theDate() == A.field(fldName, 1).asDate());
            ASSERT(VB           == A.field(fldName, 1).asElemRef());
            ASSERT(!A.field(fldName, 1).isNul2());

            mA.setFieldNull(fldName, 1);
            ASSERT(VN.theDate() == A.field(fldName, 1).asDate());
            ASSERT(A.field(fldName, 1).asElemRef().isNull());
            ASSERT(A.field(fldName, 1).isNul2());

            Obj mC = B.field(2); const Obj& C = mC;
            mC.setValue(VA);
            ASSERT(VA.theDate() == C.asDate());
            ASSERT(VA           == C.asElemRef());
            ASSERT(!C.isNul2());

            mC.makeNull();
            ASSERT(VN.theDate() == C.asDate());
            ASSERT(C.asElemRef().isNull());
            ASSERT(C.isNul2());
        }

        if (veryVerbose) { T_ cout << "Testing for TIME" << endl; }
        {
            const CERef VN = getCERef(ET::BDEM_TIME, 0);
            const CERef VA = getCERef(ET::BDEM_TIME, 1);
            const CERef VB = getCERef(ET::BDEM_TIME, 2);

            Obj mX(ET::BDEM_TIME, VA); const Obj& X = mX;
            Obj mY(ET::BDEM_TIME, VB); const Obj& Y = mY;

            ASSERT(VA.theTime() == X.asTime());
            ASSERT(VA           == X.asElemRef());
            ASSERT(!X.isNul2());

            X.makeNull();
            ASSERT(X.isNul2());
            ASSERT(X.asElemRef().isNull());
            ASSERT(VN.theTime() == X.asTime());

            ASSERT(VB.theTime() == Y.asTime());
            ASSERT(VB           == Y.asElemRef());
            ASSERT(!Y.isNul2());

            Y.makeNull();
            ASSERT(Y.isNul2());
            ASSERT(Y.asElemRef().isNull());
            ASSERT(VN.theTime() == Y.asTime());

            // nillable char array
            const char *SPEC = ":aTa&FN";
            Schema schema; const Schema& SCHEMA = schema;
            ggSchema(&schema, SPEC);

            const RecDef *RECORD  = &SCHEMA.record(0);
            const char   *fldName = RECORD->fieldName(0);

            ConstRecDefShdPtr crp(&SCHEMA.record(0), NilDeleter(), 0);
            const ConstRecDefShdPtr& CRP = crp;

            Obj mA(CRP); const Obj& A = mA;
            Obj mB = mA.field(fldName); const Obj& B = mB;
            const int NE = 3;
            mB.resize(NE);
            for (int i = 0; i < NE; ++i) {
                LOOP_ASSERT(i, B.field(i).isNul2());
                LOOP_ASSERT(i, VN.theTime() == B.field(i).asTime());
                LOOP_ASSERT(i, VN.theTime() ==
                                             B.field(i).asElemRef().theTime());
                LOOP_ASSERT(i, B.field(i).asElemRef().isNull());
                LOOP_ASSERT(i, isUnset(B.field(i).asElemRef()));
            }

            mB.setField(0, VA);
            ASSERT(VA.theTime() == B.field(0).asTime());
            ASSERT(VA           == B.field(0).asElemRef());
            ASSERT(!B.field(0).isNul2());

            mB.setFieldNull(0);
            ASSERT(VN.theTime() == B.field(0).asTime());
            ASSERT(B.field(0).asElemRef().isNull());
            ASSERT(B.field(0).isNul2());

            mA.setField(fldName, 1, VB);
            ASSERT(VB.theTime() == A.field(fldName, 1).asTime());
            ASSERT(VB           == A.field(fldName, 1).asElemRef());
            ASSERT(!A.field(fldName, 1).isNul2());

            mA.setFieldNull(fldName, 1);
            ASSERT(VN.theTime() == A.field(fldName, 1).asTime());
            ASSERT(A.field(fldName, 1).asElemRef().isNull());
            ASSERT(A.field(fldName, 1).isNul2());

            Obj mC = B.field(2); const Obj& C = mC;
            mC.setValue(VA);
            ASSERT(VA.theTime() == C.asTime());
            ASSERT(VA           == C.asElemRef());
            ASSERT(!C.isNul2());

            mC.makeNull();
            ASSERT(VN.theTime() == C.asTime());
            ASSERT(C.asElemRef().isNull());
            ASSERT(C.isNul2());
        }

        if (veryVerbose) { T_ cout << "Testing for DATETIMETZ" << endl; }
        {
            const CERef VN = getCERef(ET::BDEM_DATETIMETZ, 0);
            const CERef VA = getCERef(ET::BDEM_DATETIMETZ, 1);
            const CERef VB = getCERef(ET::BDEM_DATETIMETZ, 2);

            Obj mX(ET::BDEM_DATETIMETZ, VA); const Obj& X = mX;
            Obj mY(ET::BDEM_DATETIMETZ, VB); const Obj& Y = mY;

            ASSERT(VA.theDatetimeTz() == X.asDatetimeTz());
            ASSERT(VA                 == X.asElemRef());
            ASSERT(!X.isNul2());

            X.makeNull();
            ASSERT(X.isNul2());
            ASSERT(X.asElemRef().isNull());
            ASSERT(VN.theDatetimeTz() == X.asDatetimeTz());

            ASSERT(VB.theDatetimeTz() == Y.asDatetimeTz());
            ASSERT(VB                 == Y.asElemRef());
            ASSERT(!Y.isNul2());

            Y.makeNull();
            ASSERT(Y.isNul2());
            ASSERT(Y.asElemRef().isNull());
            ASSERT(VN.theDatetimeTz() == Y.asDatetimeTz());

            // nillable datetimeTz array
            const char *SPEC = ":aba&FN";
            Schema schema; const Schema& SCHEMA = schema;
            ggSchema(&schema, SPEC);

            const RecDef *RECORD  = &SCHEMA.record(0);
            const char   *fldName = RECORD->fieldName(0);

            ConstRecDefShdPtr crp(&SCHEMA.record(0), NilDeleter(), 0);
            const ConstRecDefShdPtr& CRP = crp;

            Obj mA(CRP); const Obj& A = mA;
            Obj mB = mA.field(fldName); const Obj& B = mB;
            const int NE = 3;
            mB.resize(NE);
            for (int i = 0; i < NE; ++i) {
                LOOP_ASSERT(i, B.field(i).isNul2());
                LOOP_ASSERT(i, VN.theDatetimeTz() ==
                                                    B.field(i).asDatetimeTz());
                LOOP_ASSERT(i, VN.theDatetimeTz() ==
                                       B.field(i).asElemRef().theDatetimeTz());
                LOOP_ASSERT(i, B.field(i).asElemRef().isNull());
                LOOP_ASSERT(i, isUnset(B.field(i).asElemRef()));
            }

            mB.setField(0, VA);
            ASSERT(VA.theDatetimeTz() == B.field(0).asDatetimeTz());
            ASSERT(VA                 == B.field(0).asElemRef());
            ASSERT(!B.field(0).isNul2());

            mB.setFieldNull(0);
            ASSERT(VN.theDatetimeTz() == B.field(0).asDatetimeTz());
            ASSERT(B.field(0).asElemRef().isNull());
            ASSERT(B.field(0).isNul2());

            mA.setField(fldName, 1, VB);
            ASSERT(VB.theDatetimeTz() == A.field(fldName, 1).asDatetimeTz());
            ASSERT(VB                 == A.field(fldName, 1).asElemRef());
            ASSERT(!A.field(fldName, 1).isNul2());

            mA.setFieldNull(fldName, 1);
            ASSERT(VN.theDatetimeTz() == A.field(fldName, 1).asDatetimeTz());
            ASSERT(A.field(fldName, 1).asElemRef().isNull());
            ASSERT(A.field(fldName, 1).isNul2());

            Obj mC = B.field(2); const Obj& C = mC;
            mC.setValue(VA);
            ASSERT(VA.theDatetimeTz() == C.asDatetimeTz());
            ASSERT(VA                 == C.asElemRef());
            ASSERT(!C.isNul2());

            mC.makeNull();
            ASSERT(VN.theDatetimeTz() == C.asDatetimeTz());
            ASSERT(C.asElemRef().isNull());
            ASSERT(C.isNul2());
        }

        if (veryVerbose) { T_ cout << "Testing for DATETZ" << endl; }
        {
            const CERef VN = getCERef(ET::BDEM_DATETZ, 0);
            const CERef VA = getCERef(ET::BDEM_DATETZ, 1);
            const CERef VB = getCERef(ET::BDEM_DATETZ, 2);

            Obj mX(ET::BDEM_DATETZ, VA); const Obj& X = mX;
            Obj mY(ET::BDEM_DATETZ, VB); const Obj& Y = mY;

            ASSERT(VA.theDateTz() == X.asDateTz());
            ASSERT(VA             == X.asElemRef());
            ASSERT(!X.isNul2());

            X.makeNull();
            ASSERT(X.isNul2());
            ASSERT(X.asElemRef().isNull());
            ASSERT(VN.theDateTz() == X.asDateTz());

            ASSERT(VB.theDateTz() == Y.asDateTz());
            ASSERT(VB             == Y.asElemRef());
            ASSERT(!Y.isNul2());

            Y.makeNull();
            ASSERT(Y.isNul2());
            ASSERT(Y.asElemRef().isNull());
            ASSERT(VN.theDateTz() == Y.asDateTz());

            // nillable dateTz array
            const char *SPEC = ":aca&FN";
            Schema schema; const Schema& SCHEMA = schema;
            ggSchema(&schema, SPEC);

            const RecDef *RECORD  = &SCHEMA.record(0);
            const char   *fldName = RECORD->fieldName(0);

            ConstRecDefShdPtr crp(&SCHEMA.record(0), NilDeleter(), 0);
            const ConstRecDefShdPtr& CRP = crp;

            Obj mA(CRP); const Obj& A = mA;
            Obj mB = mA.field(fldName); const Obj& B = mB;
            const int NE = 3;
            mB.resize(NE);
            for (int i = 0; i < NE; ++i) {
                LOOP_ASSERT(i, B.field(i).isNul2());
                LOOP_ASSERT(i, VN.theDateTz() == B.field(i).asDateTz());
                LOOP_ASSERT(i, VN.theDateTz() ==
                                           B.field(i).asElemRef().theDateTz());
                LOOP_ASSERT(i, B.field(i).asElemRef().isNull());
                LOOP_ASSERT(i, isUnset(B.field(i).asElemRef()));
            }

            mB.setField(0, VA);
            ASSERT(VA.theDateTz() == B.field(0).asDateTz());
            ASSERT(VA             == B.field(0).asElemRef());
            ASSERT(!B.field(0).isNul2());

            mB.setFieldNull(0);
            ASSERT(VN.theDateTz() == B.field(0).asDateTz());
            ASSERT(B.field(0).asElemRef().isNull());
            ASSERT(B.field(0).isNul2());

            mA.setField(fldName, 1, VB);
            ASSERT(VB.theDateTz() == A.field(fldName, 1).asDateTz());
            ASSERT(VB             == A.field(fldName, 1).asElemRef());
            ASSERT(!A.field(fldName, 1).isNul2());

            mA.setFieldNull(fldName, 1);
            ASSERT(VN.theDateTz() == A.field(fldName, 1).asDateTz());
            ASSERT(A.field(fldName, 1).asElemRef().isNull());
            ASSERT(A.field(fldName, 1).isNul2());

            Obj mC = B.field(2); const Obj& C = mC;
            mC.setValue(VA);
            ASSERT(VA.theDateTz() == C.asDateTz());
            ASSERT(VA             == C.asElemRef());
            ASSERT(!C.isNul2());

            mC.makeNull();
            ASSERT(VN.theDateTz() == C.asDateTz());
            ASSERT(C.asElemRef().isNull());
            ASSERT(C.isNul2());
        }

        if (veryVerbose) { T_ cout << "Testing for TIMETZ" << endl; }
        {
            const CERef VN = getCERef(ET::BDEM_TIMETZ, 0);
            const CERef VA = getCERef(ET::BDEM_TIMETZ, 1);
            const CERef VB = getCERef(ET::BDEM_TIMETZ, 2);

            Obj mX(ET::BDEM_TIMETZ, VA); const Obj& X = mX;
            Obj mY(ET::BDEM_TIMETZ, VB); const Obj& Y = mY;

            ASSERT(VA.theTimeTz() == X.asTimeTz());
            ASSERT(VA             == X.asElemRef());
            ASSERT(!X.isNul2());

            X.makeNull();
            ASSERT(X.isNul2());
            ASSERT(X.asElemRef().isNull());
            ASSERT(VN.theTimeTz() == X.asTimeTz());

            ASSERT(VB.theTimeTz() == Y.asTimeTz());
            ASSERT(VB             == Y.asElemRef());
            ASSERT(!Y.isNul2());

            Y.makeNull();
            ASSERT(Y.isNul2());
            ASSERT(Y.asElemRef().isNull());
            ASSERT(VN.theTimeTz() == Y.asTimeTz());

            // nillable timeTz array
            const char *SPEC = ":ada&FN";
            Schema schema; const Schema& SCHEMA = schema;
            ggSchema(&schema, SPEC);

            const RecDef *RECORD  = &SCHEMA.record(0);
            const char   *fldName = RECORD->fieldName(0);

            ConstRecDefShdPtr crp(&SCHEMA.record(0), NilDeleter(), 0);
            const ConstRecDefShdPtr& CRP = crp;

            Obj mA(CRP); const Obj& A = mA;
            Obj mB = mA.field(fldName); const Obj& B = mB;
            const int NE = 3;
            mB.resize(NE);
            for (int i = 0; i < NE; ++i) {
                LOOP_ASSERT(i, B.field(i).isNul2());
                LOOP_ASSERT(i, VN.theTimeTz() == B.field(i).asTimeTz());
                LOOP_ASSERT(i, VN.theTimeTz() ==
                                           B.field(i).asElemRef().theTimeTz());
                LOOP_ASSERT(i, B.field(i).asElemRef().isNull());
                LOOP_ASSERT(i, isUnset(B.field(i).asElemRef()));
            }

            mB.setField(0, VA);
            ASSERT(VA.theTimeTz() == B.field(0).asTimeTz());
            ASSERT(VA             == B.field(0).asElemRef());
            ASSERT(!B.field(0).isNul2());

            mB.setFieldNull(0);
            ASSERT(VN.theTimeTz() == B.field(0).asTimeTz());
            ASSERT(B.field(0).asElemRef().isNull());
            ASSERT(B.field(0).isNul2());

            mA.setField(fldName, 1, VB);
            ASSERT(VB.theTimeTz() == A.field(fldName, 1).asTimeTz());
            ASSERT(VB             == A.field(fldName, 1).asElemRef());
            ASSERT(!A.field(fldName, 1).isNul2());

            mA.setFieldNull(fldName, 1);
            ASSERT(VN.theTimeTz() == A.field(fldName, 1).asTimeTz());
            ASSERT(A.field(fldName, 1).asElemRef().isNull());
            ASSERT(A.field(fldName, 1).isNul2());

            Obj mC = B.field(2); const Obj& C = mC;
            mC.setValue(VA);
            ASSERT(VA.theTimeTz() == C.asTimeTz());
            ASSERT(VA             == C.asElemRef());
            ASSERT(!C.isNul2());

            mC.makeNull();
            ASSERT(VN.theTimeTz() == C.asTimeTz());
            ASSERT(C.asElemRef().isNull());
            ASSERT(C.isNul2());
        }

        if (veryVerbose) { T_ cout << "Test error message output" << endl; }
        {
            const CERef VA = getCERef(ET::BDEM_TIMETZ, 1);
            const CERef VB = getCERef(ET::BDEM_DOUBLE, 1);

            Obj mX(ET::BDEM_TIMETZ, VA); const Obj& X = mX;
            Obj mY(ET::BDEM_DOUBLE, VB); const Obj& Y = mY;

            ASSERT(0 == X.asChar());
            ASSERT(bdetu_Unset<bdet_Datetime>::isUnset(Y.asDatetime()));
        }
}

static void testCase5(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // TESTING FIELD ACCESSORS:
        //
        // Concerns:
        //   - Each 'field' function returns the an aggregate of the correct
        //     field value containing a reference to the sub-aggregate.
        //   - If a field is not found then an error aggregate is returned
        //   - 'fieldType' function returns the type of the specified field
        //   - 'fieldSpec' function returns a pointer to the specified field's
        //     'bdem_FieldSpec'.
        //   - 'fieldRef' returns a reference to the specified field
        //   - 'hasField' function returns the correct information
        //   - A chain of calls to 'operator[]' has the same effect as calling
        //     'field'
        //
        // Plan:
        //   Note that the functions under test follow the methodology of the
        //   'setField' set of functions in that they use the 'FindField'
        //   private function to reach the specified field and then perform
        //   the requested operation.  Because we have indirectly tested the
        //   'FindField' function while testing 'setField' we will only
        //   concentrate on testing that each function passes its arguments
        //   correctly to 'FindField' and only rigorously test the additional
        //   functionality that they provide.
        //
        //   We will construct a set of Schemas S that contain constrained
        //   record definitions of different depths (from 1 - 10).  Then we
        //   will construct an object mX from the top level record
        //   definition.  Then for each depth we will set the field at that
        //   level by calling the setField function and specifying the chain
        //   of field names from the topmost level to that field.  Then we
        //   will call the field and other accessor function using the same
        //   chain of field names and confirm that they return expected
        //   results.
        //
        //   To test that the returned field contains a reference into the
        //   original, we modify the field through the returned aggregate,
        //   then test that retrieving that field again through the original
        //   aggregate produces the modified value.  The second retrieval is
        //   done using 'operator[]', and the data pointer is compared for
        //   equality.
        //
        //   Finally, we will confirm that incorrect field accesses return an
        //   error aggregate.
        //
        // Testing:
        //   bool hasField(const char *fieldName) const;
        //   bcem_Aggregate field(NameOrIndex fieldOrIdx) const;
        //   bcem_Aggregate field(NameOrIndex fieldOrIdx1,
        //                        NameOrIndex fieldOrIdx2,
        //                        NameOrIndex fieldOrIdx3 = NameOrIndex(),
        //                        NameOrIndex fieldOrIdx4 = NameOrIndex(),
        //                        NameOrIndex fieldOrIdx5 = NameOrIndex(),
        //                        NameOrIndex fieldOrIdx6 = NameOrIndex(),
        //                        NameOrIndex fieldOrIdx7 = NameOrIndex(),
        //                        NameOrIndex fieldOrIdx8 = NameOrIndex(),
        //                        NameOrIndex fieldOrIdx9 = NameOrIndex(),
        //                        NameOrIndex fieldOrIdx10 = NameOrIndex())
        //                                                               const;
        //   ET::Type
        //   fieldType(NameOrIndex fieldOrIdx1,
        //             NameOrIndex fieldOrIdx2 = NameOrIndex(),
        //             NameOrIndex fieldOrIdx3 = NameOrIndex(),
        //             NameOrIndex fieldOrIdx4 = NameOrIndex(),
        //             NameOrIndex fieldOrIdx5 = NameOrIndex(),
        //             NameOrIndex fieldOrIdx6 = NameOrIndex(),
        //             NameOrIndex fieldOrIdx7 = NameOrIndex(),
        //             NameOrIndex fieldOrIdx8 = NameOrIndex(),
        //             NameOrIndex fieldOrIdx9 = NameOrIndex(),
        //             NameOrIndex fieldOrIdx10 = NameOrIndex()) const;
        //   bdem_ElemRef fieldRef(NameOrIndex fieldOrIdx1,
        //                         NameOrIndex fieldOrIdx2 = NameOrIndex(),
        //                         NameOrIndex fieldOrIdx3 = NameOrIndex(),
        //                         NameOrIndex fieldOrIdx4 = NameOrIndex(),
        //                         NameOrIndex fieldOrIdx5 = NameOrIndex(),
        //                         NameOrIndex fieldOrIdx6 = NameOrIndex(),
        //                         NameOrIndex fieldOrIdx7 = NameOrIndex(),
        //                         NameOrIndex fieldOrIdx8 = NameOrIndex(),
        //                         NameOrIndex fieldOrIdx9 = NameOrIndex(),
        //                         NameOrIndex fieldOrIdx10 = NameOrIndex())
        //                                                               const;
        //   const bdem_FieldDef *fieldDef() const;
        //   bcem_Aggregate operator[](const char *fieldOrIdx) const;
        //   bcem_Aggregate operator[](const bsl::string &fieldOrIdx) const;
        // --------------------------------------------------------------------

        if (verbose) tst::cout << "\nTESTING FIELD ACCESSORS"
                               << "\n=======================" << bsl::endl;

        const struct {
            int         d_line;
            const char *d_spec;  // Schema specification
        } DATA[] = {
            // Line  Schema Spec
            // ----  -----------
            {   L_,  ":aAa" },
            {   L_,  ":aAa&NT" },
            {   L_,  ":aAa&NF" },
            {   L_,  ":aAa&D0" },

            {   L_,  ":aCa :e+ba" },
            {   L_,  ":aCa&NT :e+ba" },
            {   L_,  ":aCa&NF :e+ba" },
            {   L_,  ":aCa&D0 :e+ba" },

            {   L_,  ":aDa :b+ba :e+cb" },
            {   L_,  ":aDa&NT :b+ba :e+cb" },
            {   L_,  ":aDa&NF :b+ba :e+cb" },
            {   L_,  ":aDa&D0 :b+ba :e+cb" },

            {   L_,  ":aFa :b+ba :c+cb :e+dc" },
            {   L_,  ":aFa&NT :b+ba :c+cb :e+dc" },
            {   L_,  ":aFa&NF :b+ba :c+cb :e+dc" },
            {   L_,  ":aFa&D0 :b+ba :c+cb :e+dc" },

            {   L_,  ":aGa :b+ba :c+cb :d+dc :f+fd" },
            {   L_,  ":aGa&NT :b+ba :c+cb :d+dc :f+fd" },
            {   L_,  ":aGa&NF :b+ba :c+cb :d+dc :f+fd" },
            {   L_,  ":aGa&D0 :b+ba :c+cb :d+dc :f+fd" },

            {   L_,  ":aHa :b+ba :c+cb :d+dc :f+fd :g+gf" },
            {   L_,  ":aHa&NT :b+ba :c+cb :d+dc :f+fd :g+gf" },
            {   L_,  ":aHa&NF :b+ba :c+cb :d+dc :f+fd :g+gf" },
            {   L_,  ":aHa&D0 :b+ba :c+cb :d+dc :f+fd :g+gf" },

            {   L_,  ":aMa :b+ba :c+cb :d+dc :f+fd :g+gf :h+hg" },
            {   L_,  ":aMa&NT :b+ba :c+cb :d+dc :f+fd :g+gf :h+hg" },
            {   L_,  ":aMa&NF :b+ba :c+cb :d+dc :f+fd :g+gf :h+hg" },
            {   L_,  ":aMa&D0 :b+ba :c+cb :d+dc :f+fd :g+gf :h+hg" },

            {   L_,  ":aNa :b+ba :c+cb :d+dc :f+fd :g+gf :h+hg :e+ih" },
            {   L_,  ":aNa&NT :b+ba :c+cb :d+dc :f+fd :g+gf :h+hg :e+ih" },
            {   L_,  ":aNa&NF :b+ba :c+cb :d+dc :f+fd :g+gf :h+hg :e+ih" },
            {   L_,  ":aNa&D0 :b+ba :c+cb :d+dc :f+fd :g+gf :h+hg :e+ih" },

            {   L_,  ":aPa :b+ba :c+cb :d+dc :f+fd :g+gf :h+hg :i+ih :e+ji" },
            {   L_,  ":aPa&NT :b+ba :c+cb :d+dc :f+fd :g+gf :h+hg :i+ih "
                     ":e+ji" },
            {   L_,  ":aPa&NF :b+ba :c+cb :d+dc :f+fd :g+gf :h+hg :i+ih "
                     ":e+ji" },
            {   L_,  ":aPa&D0 :b+ba :c+cb :d+dc :f+fd :g+gf :h+hg :i+ih "
                     ":e+ji" },

            {   L_,  ":aQa :b+ba :c+cb :d+dc :f+fd :g+gf :h+hg :i+ih :j+ji "
                     ":e+kj" },
            {   L_,  ":aQa&NT :b+ba :c+cb :d+dc :f+fd :g+gf :h+hg :i+ih :j+ji "
                     ":e+kj" },
            {   L_,  ":aQa&NF :b+ba :c+cb :d+dc :f+fd :g+gf :h+hg :i+ih :j+ji "
                     ":e+kj" },
            {   L_,  ":aQa&D0 :b+ba :c+cb :d+dc :f+fd :g+gf :h+hg :i+ih :j+ji "
                     ":e+kj" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE        = DATA[i].d_line;
            const char *ACTUAL_SPEC = DATA[i].d_spec;
            const int   SPEC_LEN    = bsl::strlen(ACTUAL_SPEC);
            ET::Type    FLD_TYPE    = getType(ACTUAL_SPEC[2]);

            const int   MAX_SPEC_SIZE = 128;
            char        SPEC[MAX_SPEC_SIZE];

            LOOP_ASSERT(LINE, MAX_SPEC_SIZE > SPEC_LEN);
            bsl::memset(SPEC, 0, sizeof SPEC);
            bsl::memcpy(SPEC, ACTUAL_SPEC, SPEC_LEN + 1);

            Schema s; ggSchema(&s, SPEC);
            const RecDef& r = s.record(s.numRecords() - 1);
            const FldDef& fd = s.record(0).field(0);

            if (veryVerbose) { P(s) };

            ConstRecDefShdPtr rp(&r, NilDeleter(), 0);
            bslma_TestAllocator t(veryVeryVerbose);

            const CERef VN = getCERef(FLD_TYPE, 0);
            const CERef VA = getCERef(FLD_TYPE, 1);
            const CERef VB = getCERef(FLD_TYPE, 2);

            if (veryVerbose) { P(VA) };

            Obj mX(rp, &t); const Obj& X = mX;
            Obj mY(rp, &t); const Obj& Y = mY;

            if (veryVerbose) { P(X) P(Y) };

            ASSERT(0 == X.fieldDef());

            const char *errorField = "ErrorField";
            switch (s.numRecords()) {
              case 1: {
                const char f1[] = { (*bsl::strtok(SPEC, ":")), 0 };

                ASSERT(FLD_TYPE == X.fieldType(f1));

                ASSERT(X.hasField(f1));
                ASSERT(!X.hasField(errorField));
                mX.setField(f1, VA);
                Obj mA = X.field(f1); const Obj& A = mA;
                ASSERT(FLD_TYPE == A.dataType());
                ASSERT(&fd      == A.fieldDef());
                ASSERT(VA       == A.asElemRef());
                ASSERT(VA       == X.fieldRef(f1));
                ASSERT(!A.isNul2());

                mX.setFieldNull(f1);
                ASSERT(FLD_TYPE == A.dataType());
                ASSERT(&fd      == A.fieldDef());
                ASSERT(isUnset(A.asElemRef()));
                ASSERT(A.isNul2());

                // Test that scalar types are returned by reference
                mA.setValue(VB);
                ASSERT(VB == A.asElemRef());
                ASSERT(VB == X.field(f1).asElemRef());
                ASSERT(!A.isNul2());

                mX.setField(f1, VA);
                Obj mB = X[f1]; const Obj& B = mB;
                ASSERT(FLD_TYPE == B.dataType());
                ASSERT(VA       == B.asElemRef());
                ASSERT(VA       == X.fieldRef(f1));
                ASSERT(B.dataPtr() == A.dataPtr());
                ASSERT(!A.isNul2());
                ASSERT(!B.isNul2());

                mX.setFieldNull(f1);
                ASSERT(FLD_TYPE == A.dataType());
                ASSERT(&fd      == A.fieldDef());
                ASSERT(isUnset(A.asElemRef()));
                ASSERT(isUnset(B.asElemRef()));
                ASSERT(A.isNul2());
                ASSERT(B.isNul2());

                mB.setValue(VA);
                ASSERT(VA == B.asElemRef());
                ASSERT(VA == X.field(f1).asElemRef());
                ASSERT(!B.isNul2());

                // Test bsl::string version of 'field'
                Obj mC = X.field(bsl::string(f1)); const Obj& C = mB;
                ASSERT(C.dataPtr() == A.dataPtr());
                ASSERT(!C.isNul2());

                // Test bsl::string version of 'operator[]'
                Obj mD = X[bsl::string(f1)]; const Obj& D = mB;
                ASSERT(D.dataPtr() == A.dataPtr());
                ASSERT(!D.isNul2());

              } break;
              case 2: {
                const char f1[] = { (*bsl::strtok(SPEC, ":")), 0 };
                const char f2[] = { *(bsl::strtok(0, ":") + 2), 0 };

                ASSERT(ET::BDEM_LIST == X.fieldType(f2));
                ASSERT(FLD_TYPE == X.fieldType(f2, f1));

                ASSERT(X.hasField(f2));
                ASSERT(!X.hasField(errorField));
                mX.setField(f2, f1, VA);
                Obj mA = X.field(f2, f1); const Obj& A = mA;
                ASSERT(FLD_TYPE == A.dataType());
                ASSERT(&fd      == A.fieldDef());
                ASSERT(VA       == A.asElemRef());
                ASSERT(VA       == X.fieldRef(f2, f1));
                ASSERT(!A.isNul2());

                mX.setFieldNull(f2, f1);
                ASSERT(FLD_TYPE == A.dataType());
                ASSERT(&fd      == A.fieldDef());
                ASSERT(isUnset(A.asElemRef()));
                ASSERT(A.isNul2());

                mX.setField(f2, f1, VB);
                Obj mB = X[f2][f1]; const Obj& B = mB;
                ASSERT(FLD_TYPE == B.dataType());
                ASSERT(VB       == B.asElemRef());
                ASSERT(VB       == X.fieldRef(f2, f1));
                ASSERT(B.dataPtr() == A.dataPtr());
                ASSERT(!A.isNul2());
                ASSERT(!B.isNul2());

                mX.setFieldNull(f2, f1);
                ASSERT(FLD_TYPE == A.dataType());
                ASSERT(&fd      == A.fieldDef());
                ASSERT(isUnset(A.asElemRef()));
                ASSERT(isUnset(B.asElemRef()));
                ASSERT(A.isNul2());
                ASSERT(B.isNul2());

                // Test that aggregate types are returned by reference
                Obj mC = X.field(f2);
                mC.setField(f1, VA);
                ASSERT(!X.field(f2, f1).isNul2());
                ASSERT(VA == X.field(f2, f1).asElemRef());

                mC.setField(f1, VB);
                ASSERT(!X.field(f2, f1).isNul2());
                ASSERT(VB == X.field(f2, f1).asElemRef());
              } break;
              case 3: {
                const char f1[] = { (*bsl::strtok(SPEC, ":")), 0 };
                const char f2[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f3[] = { *(bsl::strtok(0, ":") + 2), 0 };

                ASSERT(FLD_TYPE == X.fieldType(f3, f2, f1));

                ASSERT(X.hasField(f3));
                ASSERT(!X.hasField(errorField));
                mX.setField(f3, f2, f1, VA);
                Obj mA = X.field(f3, f2, f1); const Obj& A = mA;
                ASSERT(FLD_TYPE == A.dataType());
                ASSERT(&fd      == A.fieldDef());
                ASSERT(VA       == A.asElemRef());
                ASSERT(VA       == X.fieldRef(f3, f2, f1));
                ASSERT(!A.isNul2());

                mX.setFieldNull(f3, f2, f1);
                ASSERT(FLD_TYPE == A.dataType());
                ASSERT(&fd      == A.fieldDef());
                ASSERT(isUnset(A.asElemRef()));
                ASSERT(A.isNul2());

                mX.setField(f3, f2, f1, VB);
                Obj mB = X[f3][f2][f1]; const Obj& B = mB;
                ASSERT(FLD_TYPE == B.dataType());
                ASSERT(VB       == B.asElemRef());
                ASSERT(VB       == X.fieldRef(f3, f2, f1));
                ASSERT(B.dataPtr() == A.dataPtr());
                ASSERT(!A.isNul2());
                ASSERT(!B.isNul2());

                mX.setFieldNull(f3, f2, f1);
                ASSERT(FLD_TYPE == A.dataType());
                ASSERT(&fd      == A.fieldDef());
                ASSERT(isUnset(A.asElemRef()));
                ASSERT(isUnset(B.asElemRef()));
                ASSERT(A.isNul2());
                ASSERT(B.isNul2());
              } break;
              case 4: {
                const char f1[] = { (*bsl::strtok(SPEC, ":")), 0 };
                const char f2[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f3[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f4[] = { *(bsl::strtok(0, ":") + 2), 0 };

                ASSERT(FLD_TYPE == X.fieldType(f4, f3, f2, f1));

                ASSERT(X.hasField(f4));
                ASSERT(!X.hasField(errorField));
                mX.setField(f4, f3, f2, f1, VA);
                Obj mA = X.field(f4, f3, f2, f1); const Obj& A = mA;
                ASSERT(FLD_TYPE == A.dataType());
                ASSERT(&fd      == A.fieldDef());
                ASSERT(VA       == A.asElemRef());
                ASSERT(VA       == X.fieldRef(f4, f3, f2, f1));
                ASSERT(!A.isNul2());

                mX.setFieldNull(f4, f3, f2, f1);
                ASSERT(FLD_TYPE == A.dataType());
                ASSERT(&fd      == A.fieldDef());
                ASSERT(isUnset(A.asElemRef()));
                ASSERT(A.isNul2());

                mX.setField(f4, f3, f2, f1, VB);
                Obj mB = X[f4][f3][f2][f1]; const Obj& B = mB;
                ASSERT(FLD_TYPE == B.dataType());
                ASSERT(VB       == B.asElemRef());
                ASSERT(VB       == X.fieldRef(f4, f3, f2, f1));
                ASSERT(B.dataPtr() == A.dataPtr());
                ASSERT(!A.isNul2());
                ASSERT(!B.isNul2());

                mX.setFieldNull(f4, f3, f2, f1);
                ASSERT(FLD_TYPE == A.dataType());
                ASSERT(&fd      == A.fieldDef());
                ASSERT(isUnset(A.asElemRef()));
                ASSERT(isUnset(B.asElemRef()));
                ASSERT(A.isNul2());
                ASSERT(B.isNul2());
              } break;
              case 5: {
                const char f1[] = { (*bsl::strtok(SPEC, ":")), 0 };
                const char f2[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f3[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f4[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f5[] = { *(bsl::strtok(0, ":") + 2), 0 };

                ASSERT(FLD_TYPE == X.fieldType(f5, f4, f3, f2, f1));

                ASSERT(X.hasField(f5));
                ASSERT(!X.hasField(errorField));
                mX.setField(f5, f4, f3, f2, f1, VA);
                Obj mA = X.field(f5, f4, f3, f2, f1); const Obj& A = mA;
                ASSERT(FLD_TYPE == A.dataType());
                ASSERT(&fd      == A.fieldDef());
                ASSERT(VA       == A.asElemRef());
                ASSERT(VA       == X.fieldRef(f5, f4, f3, f2, f1));
                ASSERT(!A.isNul2());

                mX.setFieldNull(f5, f4, f3, f2, f1);
                ASSERT(FLD_TYPE == A.dataType());
                ASSERT(&fd      == A.fieldDef());
                ASSERT(isUnset(A.asElemRef()));
                ASSERT(A.isNul2());

                mX.setField(f5, f4, f3, f2, f1, VB);
                Obj mB = X[f5][f4][f3][f2][f1]; const Obj& B = mB;
                ASSERT(FLD_TYPE == B.dataType());
                ASSERT(VB       == B.asElemRef());
                ASSERT(VB       == X.fieldRef(f5, f4, f3, f2, f1));
                ASSERT(B.dataPtr() == A.dataPtr());
                ASSERT(!A.isNul2());
                ASSERT(!B.isNul2());

                mX.setFieldNull(f5, f4, f3, f2, f1);
                ASSERT(FLD_TYPE == A.dataType());
                ASSERT(&fd      == A.fieldDef());
                ASSERT(isUnset(A.asElemRef()));
                ASSERT(isUnset(B.asElemRef()));
                ASSERT(A.isNul2());
                ASSERT(B.isNul2());
              } break;
              case 6: {
                const char f1[] = { (*bsl::strtok(SPEC, ":")), 0 };
                const char f2[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f3[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f4[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f5[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f6[] = { *(bsl::strtok(0, ":") + 2), 0 };

                ASSERT(FLD_TYPE == X.fieldType(f6, f5, f4, f3, f2,
                                                          f1));

                ASSERT(X.hasField(f6));
                ASSERT(!X.hasField(errorField));
                mX.setField(f6, f5, f4, f3, f2, f1, VA);
                Obj mA = X.field(f6, f5, f4, f3, f2, f1); const Obj& A = mA;
                ASSERT(FLD_TYPE == A.dataType());
                ASSERT(&fd      == A.fieldDef());
                ASSERT(VA       == A.asElemRef());
                ASSERT(VA       == X.fieldRef(f6, f5, f4, f3, f2, f1));
                ASSERT(!A.isNul2());

                mX.setFieldNull(f6, f5, f4, f3, f2, f1);
                ASSERT(FLD_TYPE == A.dataType());
                ASSERT(&fd      == A.fieldDef());
                ASSERT(isUnset(A.asElemRef()));
                ASSERT(A.isNul2());

                mX.setField(f6, f5, f4, f3, f2, f1, VB);
                Obj mB = X[f6][f5][f4][f3][f2][f1]; const Obj& B = mB;
                ASSERT(FLD_TYPE == B.dataType());
                ASSERT(VB       == B.asElemRef());
                ASSERT(VB       == X.fieldRef(f6, f5, f4, f3, f2, f1));
                ASSERT(B.dataPtr() == A.dataPtr());
                ASSERT(!A.isNul2());
                ASSERT(!B.isNul2());

                mX.setFieldNull(f6, f5, f4, f3, f2, f1);
                ASSERT(FLD_TYPE == A.dataType());
                ASSERT(&fd      == A.fieldDef());
                ASSERT(isUnset(A.asElemRef()));
                ASSERT(isUnset(B.asElemRef()));
                ASSERT(A.isNul2());
                ASSERT(B.isNul2());
              } break;
              case 7: {
                const char f1[] = { (*bsl::strtok(SPEC, ":")), 0 };
                const char f2[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f3[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f4[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f5[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f6[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f7[] = { *(bsl::strtok(0, ":") + 2), 0 };

                ASSERT(FLD_TYPE == X.fieldType(f7, f6, f5, f4, f3, f2, f1));

                ASSERT(X.hasField(f7));
                ASSERT(!X.hasField(errorField));
                mX.setField(f7, f6, f5, f4, f3, f2, f1, VA);
                Obj mA = X.field(f7, f6, f5, f4, f3, f2, f1);
                const Obj& A = mA;
                ASSERT(FLD_TYPE == A.dataType());
                ASSERT(&fd      == A.fieldDef());
                ASSERT(VA       == A.asElemRef());
                ASSERT(VA       == X.fieldRef(f7, f6, f5, f4, f3, f2, f1));
                ASSERT(!A.isNul2());

                mX.setFieldNull(f7, f6, f5, f4, f3, f2, f1);
                ASSERT(FLD_TYPE == A.dataType());
                ASSERT(&fd      == A.fieldDef());
                ASSERT(isUnset(A.asElemRef()));
                ASSERT(A.isNul2());

                mX.setField(f7, f6, f5, f4, f3, f2, f1, VB);
                Obj mB = X[f7][f6][f5][f4][f3][f2][f1];
                const Obj& B = mB;
                ASSERT(FLD_TYPE == B.dataType());
                ASSERT(VB       == B.asElemRef());
                ASSERT(VB       == X.fieldRef(f7, f6, f5, f4, f3, f2, f1));
                ASSERT(B.dataPtr() == A.dataPtr());
                ASSERT(!A.isNul2());
                ASSERT(!B.isNul2());

                mX.setFieldNull(f7, f6, f5, f4, f3, f2, f1);
                ASSERT(FLD_TYPE == A.dataType());
                ASSERT(&fd      == A.fieldDef());
                ASSERT(isUnset(A.asElemRef()));
                ASSERT(isUnset(B.asElemRef()));
                ASSERT(A.isNul2());
                ASSERT(B.isNul2());
              } break;
              case 8: {
                const char f1[] = { (*bsl::strtok(SPEC, ":")), 0 };
                const char f2[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f3[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f4[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f5[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f6[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f7[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f8[] = { *(bsl::strtok(0, ":") + 2), 0 };

                ASSERT(FLD_TYPE == X.fieldType(f8, f7, f6, f5,
                                               f4, f3, f2, f1));

                ASSERT(X.hasField(f8));
                ASSERT(!X.hasField(errorField));
                mX.setField(f8, f7, f6, f5, f4, f3, f2, f1, VA);
                Obj mA = X.field(f8, f7, f6, f5, f4, f3, f2, f1);
                const Obj& A = mA;
                ASSERT(FLD_TYPE == A.dataType());
                ASSERT(&fd      == A.fieldDef());
                ASSERT(VA       == A.asElemRef());
                ASSERT(VA       == X.fieldRef(f8, f7, f6, f5, f4, f3, f2, f1));
                ASSERT(!A.isNul2());

                mX.setFieldNull(f8, f7, f6, f5, f4, f3, f2, f1);
                ASSERT(FLD_TYPE == A.dataType());
                ASSERT(&fd      == A.fieldDef());
                ASSERT(isUnset(A.asElemRef()));
                ASSERT(A.isNul2());

                mX.setField(f8, f7, f6, f5, f4, f3, f2, f1, VB);
                Obj mB = X[f8][f7][f6][f5][f4][f3][f2][f1];
                const Obj& B = mB;
                ASSERT(FLD_TYPE == B.dataType());
                ASSERT(VB       == B.asElemRef());
                ASSERT(VB       == X.fieldRef(f8, f7, f6, f5, f4, f3, f2, f1));
                ASSERT(B.dataPtr() == A.dataPtr());
                ASSERT(!A.isNul2());
                ASSERT(!B.isNul2());

                mX.setFieldNull(f8, f7, f6, f5, f4, f3, f2, f1);
                ASSERT(FLD_TYPE == A.dataType());
                ASSERT(&fd      == A.fieldDef());
                ASSERT(isUnset(A.asElemRef()));
                ASSERT(isUnset(B.asElemRef()));
                ASSERT(A.isNul2());
                ASSERT(B.isNul2());
              } break;
              case 9: {
                const char f1[] = { (*bsl::strtok(SPEC, ":")), 0 };
                const char f2[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f3[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f4[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f5[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f6[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f7[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f8[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f9[] = { *(bsl::strtok(0, ":") + 2), 0 };

                ASSERT(FLD_TYPE == X.fieldType(f9, f8, f7, f6, f5,
                                               f4, f3, f2, f1));

                ASSERT(X.hasField(f9));
                ASSERT(!X.hasField(errorField));
                mX.setField(f9, f8, f7, f6, f5, f4, f3, f2, f1, VA);
                Obj mA = X.field(f9, f8, f7, f6, f5, f4, f3, f2, f1);
                const Obj& A = mA;
                ASSERT(FLD_TYPE == A.dataType());
                ASSERT(&fd      == A.fieldDef());
                ASSERT(VA       == A.asElemRef());
                ASSERT(VA       == X.fieldRef(f9, f8, f7, f6, f5,
                                              f4, f3, f2, f1));
                ASSERT(!A.isNul2());

                mX.setFieldNull(f9, f8, f7, f6, f5, f4, f3, f2, f1);
                ASSERT(FLD_TYPE == A.dataType());
                ASSERT(&fd      == A.fieldDef());
                ASSERT(isUnset(A.asElemRef()));
                ASSERT(A.isNul2());

                mX.setField(f9, f8, f7, f6, f5, f4, f3, f2, f1, VB);
                Obj mB = X[f9][f8][f7][f6][f5][f4][f3][f2][f1];
                const Obj& B = mB;
                ASSERT(FLD_TYPE == B.dataType());
                ASSERT(VB       == B.asElemRef());
                ASSERT(VB       == X.fieldRef(f9, f8, f7, f6, f5,
                                                         f4, f3, f2, f1));
                ASSERT(B.dataPtr() == A.dataPtr());
                ASSERT(!A.isNul2());
                ASSERT(!B.isNul2());

                mX.setFieldNull(f9, f8, f7, f6, f5, f4, f3, f2, f1);
                ASSERT(FLD_TYPE == A.dataType());
                ASSERT(&fd      == A.fieldDef());
                ASSERT(isUnset(A.asElemRef()));
                ASSERT(isUnset(B.asElemRef()));
                ASSERT(A.isNul2());
                ASSERT(B.isNul2());
              } break;
              case 10: {
                const char f1[] = { (*bsl::strtok(SPEC, ":")), 0 };
                const char f2[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f3[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f4[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f5[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f6[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f7[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f8[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f9[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f10[] = { *(bsl::strtok(0, ":") + 2), 0 };

                ASSERT(FLD_TYPE == X.fieldType(f10, f9, f8, f7, f6,
                                                          f5, f4, f3, f2, f1));

                ASSERT(X.hasField(f10));
                ASSERT(!X.hasField(errorField));
                mX.setField(f10, f9, f8, f7, f6, f5, f4, f3, f2, f1, VA);
                Obj mA = X.field(f10, f9, f8, f7, f6, f5, f4, f3, f2, f1);
                const Obj& A = mA;
                ASSERT(FLD_TYPE == A.dataType());
                ASSERT(&fd      == A.fieldDef());
                ASSERT(VA       == A.asElemRef());
                ASSERT(VA       == X.fieldRef(f10, f9, f8, f7, f6,
                                              f5, f4, f3, f2, f1));
                ASSERT(!A.isNul2());

                mX.setFieldNull(f10, f9, f8, f7, f6, f5, f4, f3, f2, f1);
                ASSERT(FLD_TYPE == A.dataType());
                ASSERT(&fd      == A.fieldDef());
                ASSERT(isUnset(A.asElemRef()));
                ASSERT(A.isNul2());

                mX.setField(f10, f9, f8, f7, f6, f5, f4, f3, f2, f1, VB);
                Obj mB = X[f10][f9][f8][f7][f6][f5][f4][f3][f2][f1];
                const Obj& B = mB;
                ASSERT(FLD_TYPE == B.dataType());
                ASSERT(VB       == B.asElemRef());
                ASSERT(VB       == X.fieldRef(f10, f9, f8, f7, f6,
                                                         f5, f4, f3, f2, f1));
                ASSERT(B.dataPtr() == A.dataPtr());
                ASSERT(!A.isNul2());
                ASSERT(!B.isNul2());

                mX.setFieldNull(f10, f9, f8, f7, f6, f5, f4, f3, f2, f1);
                ASSERT(FLD_TYPE == A.dataType());
                ASSERT(&fd      == A.fieldDef());
                ASSERT(isUnset(A.asElemRef()));
                ASSERT(isUnset(B.asElemRef()));
                ASSERT(A.isNul2());
                ASSERT(B.isNul2());

                const char errFld[] = "ErrorField";

                // Test error output
                {
                    Obj EMPTY; // Empty object

                    // Test that fieldType prints does not print an error when
                    // field is not found.
                    ASSERT(ET::BDEM_VOID ==
                      X.fieldType(errFld, f9, f8, f7, f6, f5, f4, f3, f2, f1));

                    ASSERT(ET::BDEM_VOID ==
                     X.fieldType(f10, errFld, f8, f7, f6, f5, f4, f3, f2, f1));

                    ASSERT(ET::BDEM_VOID ==
                     X.fieldType(f10, f9, errFld, f7, f6, f5, f4, f3, f2, f1));

                    ASSERT(ET::BDEM_VOID ==
                     X.fieldType(f10, f9, f8, errFld, f6, f5, f4, f3, f2, f1));

                    ASSERT(ET::BDEM_VOID ==
                     X.fieldType(f10, f9, f8, f7, errFld, f5, f4, f3, f2, f1));

                    ASSERT(ET::BDEM_VOID ==
                     X.fieldType(f10, f9, f8, f7, f6, errFld, f4, f3, f2, f1));

                    ASSERT(ET::BDEM_VOID ==
                     X.fieldType(f10, f9, f8, f7, f6, f5, errFld, f3, f2, f1));

                    ASSERT(ET::BDEM_VOID ==
                     X.fieldType(f10, f9, f8, f7, f6, f5, f4, errFld, f2, f1));

                    ASSERT(ET::BDEM_VOID ==
                     X.fieldType(f10, f9, f8, f7, f6, f5, f4, f3, errFld, f1));

                    ASSERT(ET::BDEM_VOID ==
                     X.fieldType(f10, f9, f8, f7, f6, f5, f4, f3, f2, errFld));

                    // Test that field prints the appropriate message on error
                    // on each field

                    ASSERT_AGG_ERROR(mX.field(errFld, f9, f8, f7, f6, f5,
                                      f4, f3, f2, f1), BCEM_ERR_TBD);

                    ASSERT_AGG_ERROR(mX.field(f10, errFld, f8, f7, f6, f5,
                                      f4, f3, f2, f1), BCEM_ERR_TBD);

                    ASSERT_AGG_ERROR(mX.field(f10, f9, errFld, f7, f6, f5,
                                      f4, f3, f2, f1), BCEM_ERR_TBD);

                    ASSERT_AGG_ERROR(mX.field(f10, f9, f8, errFld, f6, f5,
                                      f4, f3, f2, f1), BCEM_ERR_TBD);

                    ASSERT_AGG_ERROR(mX.field(f10, f9, f8, f7, errFld, f5,
                                      f4, f3, f2, f1), BCEM_ERR_TBD);

                    ASSERT_AGG_ERROR(mX.field(f10, f9, f8, f7, f6, errFld,
                                      f4, f3, f2, f1), BCEM_ERR_TBD);

                    ASSERT_AGG_ERROR(mX.field(f10, f9, f8, f7, f6, f5,
                                      errFld, f3, f2, f1), BCEM_ERR_TBD);

                    ASSERT_AGG_ERROR(mX.field(f10, f9, f8, f7, f6, f5,
                                      f4, errFld, f2, f1), BCEM_ERR_TBD);

                    ASSERT_AGG_ERROR(mX.field(f10, f9, f8, f7, f6, f5,
                                      f4, f3, errFld, f1), BCEM_ERR_TBD);

                    ASSERT_AGG_ERROR(mX.field(f10, f9, f8, f7, f6, f5,
                                      f4, f3, f2, errFld), BCEM_ERR_TBD);

                    // Test that fieldRef prints does not print an error
                    // message when 'fieldRef' can't find the field.
                    ASSERT(ET::BDEM_VOID ==
                        X.fieldRef(errFld, f9, f8, f7, f6, f5, f4, f3, f2, f1)
                                                                      .type());

                    ASSERT(ET::BDEM_VOID ==
                        X.fieldRef(f10, errFld, f8, f7, f6, f5, f4, f3, f2, f1)
                                                                      .type());

                    ASSERT(ET::BDEM_VOID ==
                        X.fieldRef(f10, f9, errFld, f7, f6, f5, f4, f3, f2, f1)
                                                                      .type());

                    ASSERT(ET::BDEM_VOID ==
                        X.fieldRef(f10, f9, f8, errFld, f6, f5, f4, f3, f2, f1)
                                                                      .type());

                    ASSERT(ET::BDEM_VOID ==
                        X.fieldRef(f10, f9, f8, f7, errFld, f5, f4, f3, f2, f1)
                                                                      .type());

                    ASSERT(ET::BDEM_VOID ==
                        X.fieldRef(f10, f9, f8, f7, f6, errFld, f4, f3, f2, f1)
                                                                      .type());

                    ASSERT(ET::BDEM_VOID ==
                        X.fieldRef(f10, f9, f8, f7, f6, f5, errFld, f3, f2, f1)
                                                                      .type());

                    ASSERT(ET::BDEM_VOID ==
                        X.fieldRef(f10, f9, f8, f7, f6, f5, f4, errFld, f2, f1)
                                                                      .type());

                    ASSERT(ET::BDEM_VOID ==
                        X.fieldRef(f10, f9, f8, f7, f6, f5, f4, f3, errFld, f1)
                                                                      .type());

                    ASSERT(ET::BDEM_VOID ==
                        X.fieldRef(f10, f9, f8, f7, f6, f5, f4, f3, f2, errFld)
                                                                      .type());
                }
              } break;
              default:
                ASSERT(0);
            }
        }
}

static void testCase4(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // TESTING 'setField' MANIPULATOR FUNCTIONS:
        //
        // Concerns:
        //   Each setField function sets the specified field value correctly.
        //
        // Plan:
        //   Note that each of the setField functions calls the 'field'
        //   function with the variable number of arguments to
        //   navigate to the specified field and then calls 'setValue'
        //   function to set the field's value.  We have already tested
        //   setting the value of the field in the one-arg setField function.
        //   Hence here we are only concerned about checking that the
        //   'field' function selects the correct field.  Also note that
        //   we have tested the 'descendIntoField' private function in the one
        //   arg setField function and will assume that it works if the passed
        //   argument is a name or index.  Hence our testing is quite
        //   simplified as all we need to test is that the setField function
        //   passes the correct number of functions to 'FindField' and that
        //   'FindField' correctly finds the requested field.
        //
        //   We will construct a set of Schemas S that contain constrained
        //   record definitions of different depths (from 1 - 10).  Then we
        //   will construct an object mX from the top level record
        //   definition.  Then for each depth we will set the field at that
        //   level by calling the setField function and specifying the chain
        //   of field names from the topmost level to that field.  We will
        //   check the correctness of our result by calling the corresponding
        //   field function and relying on its correctness.
        //
        //   Finally, we will confirm that incorrect field accesses print out
        //   an error message on stderr.
        //
        // Testing:
        //   template <typename VALTYPE>
        //   void setField(NameOrIndex    fieldOrIdx1,
        //                 NameOrIndex    fieldOrIdx2,
        //                 const VALTYPE& value) const;
        //   template <typename VALTYPE>
        //   void setField(NameOrIndex    fieldOrIdx1,
        //                 NameOrIndex    fieldOrIdx2,
        //                 NameOrIndex    fieldOrIdx3,
        //                 const VALTYPE& value) const;
        //   template <typename VALTYPE>
        //   void setField(NameOrIndex    fieldOrIdx1,
        //                 NameOrIndex    fieldOrIdx2,
        //                 NameOrIndex    fieldOrIdx3,
        //                 NameOrIndex    fieldOrIdx4,
        //                 const VALTYPE& value) const;
        //   template <typename VALTYPE>
        //   void setField(NameOrIndex    fieldOrIdx1,
        //                 NameOrIndex    fieldOrIdx2,
        //                 NameOrIndex    fieldOrIdx3,
        //                 NameOrIndex    fieldOrIdx4,
        //                 NameOrIndex    fieldOrIdx5,
        //                 const VALTYPE& value) const;
        //   template <typename VALTYPE>
        //   void setField(NameOrIndex    fieldOrIdx1,
        //                 NameOrIndex    fieldOrIdx2,
        //                 NameOrIndex    fieldOrIdx3,
        //                 NameOrIndex    fieldOrIdx4,
        //                 NameOrIndex    fieldOrIdx5,
        //                 NameOrIndex    fieldOrIdx6,
        //                 const VALTYPE& value) const;
        //   template <typename VALTYPE>
        //   void setField(NameOrIndex    fieldOrIdx1,
        //                 NameOrIndex    fieldOrIdx2,
        //                 NameOrIndex    fieldOrIdx3,
        //                 NameOrIndex    fieldOrIdx4,
        //                 NameOrIndex    fieldOrIdx5,
        //                 NameOrIndex    fieldOrIdx6,
        //                 NameOrIndex    fieldOrIdx7,
        //                 const VALTYPE& value) const;
        //   template <typename VALTYPE>
        //   void setField(NameOrIndex    fieldOrIdx1,
        //                 NameOrIndex    fieldOrIdx2,
        //                 NameOrIndex    fieldOrIdx3,
        //                 NameOrIndex    fieldOrIdx4,
        //                 NameOrIndex    fieldOrIdx5,
        //                 NameOrIndex    fieldOrIdx6,
        //                 NameOrIndex    fieldOrIdx7,
        //                 NameOrIndex    fieldOrIdx8,
        //                 const VALTYPE& value) const;
        //   template <typename VALTYPE>
        //   void setField(NameOrIndex    fieldOrIdx1,
        //                 NameOrIndex    fieldOrIdx2,
        //                 NameOrIndex    fieldOrIdx3,
        //                 NameOrIndex    fieldOrIdx4,
        //                 NameOrIndex    fieldOrIdx5,
        //                 NameOrIndex    fieldOrIdx6,
        //                 NameOrIndex    fieldOrIdx7,
        //                 NameOrIndex    fieldOrIdx8,
        //                 NameOrIndex    fieldOrIdx9,
        //                 const VALTYPE& value) const;
        //   template <typename VALTYPE>
        //   void setField(NameOrIndex    fieldOrIdx1,
        //                 NameOrIndex    fieldOrIdx2,
        //                 NameOrIndex    fieldOrIdx3,
        //                 NameOrIndex    fieldOrIdx4,
        //                 NameOrIndex    fieldOrIdx5,
        //                 NameOrIndex    fieldOrIdx6,
        //                 NameOrIndex    fieldOrIdx7,
        //                 NameOrIndex    fieldOrIdx8,
        //                 NameOrIndex    fieldOrIdx9,
        //                 NameOrIndex    fieldOrIdx10,
        //                 const VALTYPE& value) const;
        //   void setFieldNull(NameOrIndex    fieldOrIdx1,
        //                     NameOrIndex    fieldOrIdx2) const;
        //   void setFieldNull(NameOrIndex    fieldOrIdx1,
        //                     NameOrIndex    fieldOrIdx2,
        //                     NameOrIndex    fieldOrIdx3) const;
        //   void setFieldNull(NameOrIndex    fieldOrIdx1,
        //                     NameOrIndex    fieldOrIdx2,
        //                     NameOrIndex    fieldOrIdx3,
        //                     NameOrIndex    fieldOrIdx4) const;
        //   void setFieldNull(NameOrIndex    fieldOrIdx1,
        //                     NameOrIndex    fieldOrIdx2,
        //                     NameOrIndex    fieldOrIdx3,
        //                     NameOrIndex    fieldOrIdx4,
        //                     NameOrIndex    fieldOrIdx5) const;
        //   void setFieldNull(NameOrIndex    fieldOrIdx1,
        //                     NameOrIndex    fieldOrIdx2,
        //                     NameOrIndex    fieldOrIdx3,
        //                     NameOrIndex    fieldOrIdx4,
        //                     NameOrIndex    fieldOrIdx5,
        //                     NameOrIndex    fieldOrIdx6) const;
        //   void setFieldNull(NameOrIndex    fieldOrIdx1,
        //                     NameOrIndex    fieldOrIdx2,
        //                     NameOrIndex    fieldOrIdx3,
        //                     NameOrIndex    fieldOrIdx4,
        //                     NameOrIndex    fieldOrIdx5,
        //                     NameOrIndex    fieldOrIdx6,
        //                     NameOrIndex    fieldOrIdx7) const;
        //   void setFieldNull(NameOrIndex    fieldOrIdx1,
        //                     NameOrIndex    fieldOrIdx2,
        //                     NameOrIndex    fieldOrIdx3,
        //                     NameOrIndex    fieldOrIdx4,
        //                     NameOrIndex    fieldOrIdx5,
        //                     NameOrIndex    fieldOrIdx6,
        //                     NameOrIndex    fieldOrIdx7,
        //                     NameOrIndex    fieldOrIdx8) const;
        //   void setFieldNull(NameOrIndex    fieldOrIdx1,
        //                     NameOrIndex    fieldOrIdx2,
        //                     NameOrIndex    fieldOrIdx3,
        //                     NameOrIndex    fieldOrIdx4,
        //                     NameOrIndex    fieldOrIdx5,
        //                     NameOrIndex    fieldOrIdx6,
        //                     NameOrIndex    fieldOrIdx7,
        //                     NameOrIndex    fieldOrIdx8,
        //                     NameOrIndex    fieldOrIdx9) const;
        //   void setFieldNull(NameOrIndex    fieldOrIdx1,
        //                     NameOrIndex    fieldOrIdx2,
        //                     NameOrIndex    fieldOrIdx3,
        //                     NameOrIndex    fieldOrIdx4,
        //                     NameOrIndex    fieldOrIdx5,
        //                     NameOrIndex    fieldOrIdx6,
        //                     NameOrIndex    fieldOrIdx7,
        //                     NameOrIndex    fieldOrIdx8,
        //                     NameOrIndex    fieldOrIdx9,
        //                     NameOrIndex    fieldOrIdx10) const;
        //
        // Private functions:
        //   NavStatus descendIntoField(NameOrIndex fieldOrIdx1);
        // --------------------------------------------------------------------

        if (verbose) tst::cout << "\nTESTING 'setField' MANIPULATOR"
                               << "\n=============================="
                               << bsl::endl;

        const struct {
            int         d_line;
            const char *d_spec;
        } DATA[] = {
            // Line  Spec
            // ----  ----
            {   L_,  ":aAa" },
            {   L_,  ":aAa&NT" },
            {   L_,  ":aAa&NF" },
            {   L_,  ":aAa&D0" },

            {   L_,  ":aCa :e+ba" },
            {   L_,  ":aCa&NT :e+ba" },
            {   L_,  ":aCa&NF :e+ba" },
            {   L_,  ":aCa&D0 :e+ba" },

            {   L_,  ":aDa :b+ba :e+cb" },
            {   L_,  ":aDa&NT :b+ba :e+cb" },
            {   L_,  ":aDa&NF :b+ba :e+cb" },
            {   L_,  ":aDa&D0 :b+ba :e+cb" },

            {   L_,  ":aFa :b+ba :c+cb :e+dc" },
            {   L_,  ":aFa&NT :b+ba :c+cb :e+dc" },
            {   L_,  ":aFa&NF :b+ba :c+cb :e+dc" },
            {   L_,  ":aFa&D0 :b+ba :c+cb :e+dc" },

            {   L_,  ":aGa :b+ba :c+cb :d+dc :f+fd" },
            {   L_,  ":aGa&NT :b+ba :c+cb :d+dc :f+fd" },
            {   L_,  ":aGa&NF :b+ba :c+cb :d+dc :f+fd" },
            {   L_,  ":aGa&D0 :b+ba :c+cb :d+dc :f+fd" },

            {   L_,  ":aHa :b+ba :c+cb :d+dc :f+fd :g+gf" },
            {   L_,  ":aHa&NT :b+ba :c+cb :d+dc :f+fd :g+gf" },
            {   L_,  ":aHa&NF :b+ba :c+cb :d+dc :f+fd :g+gf" },
            {   L_,  ":aHa&D0 :b+ba :c+cb :d+dc :f+fd :g+gf" },

            {   L_,  ":aMa :b+ba :c+cb :d+dc :f+fd :g+gf :h+hg" },
            {   L_,  ":aMa&NT :b+ba :c+cb :d+dc :f+fd :g+gf :h+hg" },
            {   L_,  ":aMa&NF :b+ba :c+cb :d+dc :f+fd :g+gf :h+hg" },
            {   L_,  ":aMa&D0 :b+ba :c+cb :d+dc :f+fd :g+gf :h+hg" },

            {   L_,  ":aNa :b+ba :c+cb :d+dc :f+fd :g+gf :h+hg :e+ih" },
            {   L_,  ":aNa&NT :b+ba :c+cb :d+dc :f+fd :g+gf :h+hg :e+ih" },
            {   L_,  ":aNa&NF :b+ba :c+cb :d+dc :f+fd :g+gf :h+hg :e+ih" },
            {   L_,  ":aNa&D0 :b+ba :c+cb :d+dc :f+fd :g+gf :h+hg :e+ih" },

            {   L_,  ":aPa :b+ba :c+cb :d+dc :f+fd :g+gf :h+hg :i+ih :e+ji" },
            {   L_,  ":aPa&NT :b+ba :c+cb :d+dc :f+fd :g+gf :h+hg :i+ih "
                     ":e+ji" },
            {   L_,  ":aPa&NF :b+ba :c+cb :d+dc :f+fd :g+gf :h+hg :i+ih "
                     ":e+ji" },
            {   L_,  ":aPa&D0 :b+ba :c+cb :d+dc :f+fd :g+gf :h+hg :i+ih "
                     ":e+ji" },

            {   L_,  ":aQa :b+ba :c+cb :d+dc :f+fd :g+gf :h+hg :i+ih :j+ji "
                     ":e+kj" },
            {   L_,  ":aQa&NT :b+ba :c+cb :d+dc :f+fd :g+gf :h+hg :i+ih :j+ji "
                     ":e+kj" },
            {   L_,  ":aQa&NF :b+ba :c+cb :d+dc :f+fd :g+gf :h+hg :i+ih :j+ji "
                     ":e+kj" },
            {   L_,  ":aQa&D0 :b+ba :c+cb :d+dc :f+fd :g+gf :h+hg :i+ih :j+ji "
                     ":e+kj" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE          = DATA[i].d_line;
            const char *ACTUAL_SPEC   = DATA[i].d_spec;
            const int   SPEC_LEN      = bsl::strlen(ACTUAL_SPEC);
            ET::Type    FLD_TYPE      = getType(ACTUAL_SPEC[2]);

            const int   MAX_SPEC_SIZE = 128;
            char        SPEC[MAX_SPEC_SIZE];

            LOOP_ASSERT(LINE, MAX_SPEC_SIZE > SPEC_LEN);
            bsl::memset(SPEC, 0, sizeof SPEC);
            bsl::memcpy(SPEC, ACTUAL_SPEC, SPEC_LEN + 1);

            Schema s; ggSchema(&s, SPEC);
            const RecDef& r = s.record(s.numRecords() - 1);

            if (veryVerbose) { P(s) };

            ConstRecDefShdPtr rp(&r, NilDeleter(), 0);
            bslma_TestAllocator t(veryVeryVerbose);

            const CERef VN = getCERef(FLD_TYPE, 0);
            const CERef VA = getCERef(FLD_TYPE, 1);
            const CERef VB = getCERef(FLD_TYPE, 2);

            if (veryVerbose) { P(VA) };

            Obj mX(rp, &t); const Obj& X = mX;
            Obj mY(rp, &t); const Obj& Y = mY;

            if (veryVerbose) { P(X) };

            switch (s.numRecords()) {
              case 1: {
                const char f1[] = { (*bsl::strtok(SPEC, ":")), 0 };

                mX.setField(f1, VA);
                ASSERT(VA == X.field(f1).asElemRef());
                ASSERT(!X.field(f1).isNul2());

                mX.setFieldNull(f1);
                ASSERT(isUnset(X.field(f1).asElemRef()));
                ASSERT(X.field(f1).isNul2());

                mX.setField(f1, VB);
                ASSERT(VB == X.field(f1).asElemRef());
                ASSERT(!X.field(f1).isNul2());

                mX.setFieldNull(f1);
                ASSERT(isUnset(X.field(f1).asElemRef()));
                ASSERT(X.field(f1).isNul2());
              } break;
              case 2: {
                const char f1[] = { *(bsl::strtok(SPEC, ":")), 0 };
                const char f2[] = { *(bsl::strtok(0, ":") + 2), 0 };

                mX.setField(f2, f1, VA);
                ASSERT(VA == X.field(f2, f1).asElemRef());
                LOOP_ASSERT(X, !X.field(f2, f1).isNul2());

                mX.setFieldNull(f2, f1);
                ASSERT(isUnset(X.field(f2, f1).asElemRef()));
                ASSERT(X.field(f2, f1).isNul2());

                mX.setField(f2, f1, VB);
                ASSERT(VB == X.field(f2, f1).asElemRef());
                ASSERT(!X.field(f2, f1).isNul2());

                mX.setFieldNull(f2, f1);
                ASSERT(isUnset(X.field(f2, f1).asElemRef()));
                ASSERT(X.field(f2, f1).isNul2());
              } break;
              case 3: {
                const char f1[] = { *(bsl::strtok(SPEC, ":")), 0 };
                const char f2[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f3[] = { *(bsl::strtok(0, ":") + 2), 0 };

                mX.setField(f3, f2, f1, VA);
                ASSERT(VA == X.field(f3, f2, f1).asElemRef());
                ASSERT(!X.field(f3, f2, f1).isNul2());

                mX.setFieldNull(f3, f2, f1);
                ASSERT(isUnset(X.field(f3, f2, f1).asElemRef()));
                ASSERT(X.field(f3, f2, f1).isNul2());

                mX.setField(f3, f2, f1, VB);
                ASSERT(VB == X.field(f3, f2, f1).asElemRef());
                ASSERT(!X.field(f3, f2, f1).isNul2());

                mX.setFieldNull(f3, f2, f1);
                ASSERT(isUnset(X.field(f3, f2, f1).asElemRef()));
                ASSERT(X.field(f3, f2, f1).isNul2());
              } break;
              case 4: {
                const char f1[] = { *(bsl::strtok(SPEC, ":")), 0 };
                const char f2[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f3[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f4[] = { *(bsl::strtok(0, ":") + 2), 0 };

                mX.setField(f4, f3, f2, f1, VA);
                ASSERT(VA == X.field(f4, f3, f2, f1).asElemRef());
                ASSERT(!X.field(f4, f3, f2, f1).isNul2());

                mX.setFieldNull(f4, f3, f2, f1);
                ASSERT(isUnset(X.field(f4, f3, f2, f1).asElemRef()));
                ASSERT(X.field(f4, f3, f2, f1).isNul2());

                mX.setField(f4, f3, f2, f1, VB);
                ASSERT(VB == X.field(f4, f3, f2, f1).asElemRef());
                ASSERT(!X.field(f4, f3, f2, f1).isNul2());

                mX.setFieldNull(f4, f3, f2, f1);
                ASSERT(isUnset(X.field(f4, f3, f2, f1).asElemRef()));
                ASSERT(X.field(f4, f3, f2, f1).isNul2());
              } break;
              case 5: {
                const char f1[] = { *(bsl::strtok(SPEC, ":")), 0 };
                const char f2[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f3[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f4[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f5[] = { *(bsl::strtok(0, ":") + 2), 0 };

                mX.setField(f5, f4, f3, f2, f1, VA);
                ASSERT(VA == X.field(f5, f4, f3, f2, f1).asElemRef());
                ASSERT(!X.field(f5, f4, f3, f2, f1).isNul2());

                mX.setFieldNull(f5, f4, f3, f2, f1);
                ASSERT(isUnset(X.field(f5, f4, f3, f2, f1).asElemRef()));
                ASSERT(X.field(f5, f4, f3, f2, f1).isNul2());

                mX.setField(f5, f4, f3, f2, f1, VB);
                ASSERT(VB == X.field(f5, f4, f3, f2, f1).asElemRef());
                ASSERT(!X.field(f5, f4, f3, f2, f1).isNul2());

                mX.setFieldNull(f5, f4, f3, f2, f1);
                ASSERT(isUnset(X.field(f5, f4, f3, f2, f1).asElemRef()));
                ASSERT(X.field(f5, f4, f3, f2, f1).isNul2());
              } break;
              case 6: {
                const char f1[] = { *(bsl::strtok(SPEC, ":")), 0 };
                const char f2[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f3[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f4[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f5[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f6[] = { *(bsl::strtok(0, ":") + 2), 0 };

                mX.setField(f6, f5, f4, f3, f2, f1, VA);
                ASSERT(VA == X.field(f6, f5, f4, f3, f2, f1).asElemRef());
                ASSERT(!X.field(f6, f5, f4, f3, f2, f1).isNul2());

                mX.setFieldNull(f6, f5, f4, f3, f2, f1);
                ASSERT(isUnset(X.field(f6, f5, f4, f3, f2, f1).asElemRef()));
                ASSERT(X.field(f6, f5, f4, f3, f2, f1).isNul2());

                mX.setField(f6, f5, f4, f3, f2, f1, VB);
                ASSERT(VB == X.field(f6, f5, f4, f3, f2, f1).asElemRef());
                ASSERT(!X.field(f6, f5, f4, f3, f2, f1).isNul2());

                mX.setFieldNull(f6, f5, f4, f3, f2, f1);
                ASSERT(isUnset(X.field(f6, f5, f4, f3, f2, f1).asElemRef()));
                ASSERT(X.field(f6, f5, f4, f3, f2, f1).isNul2());
              } break;
              case 7: {
                const char f1[] = { *(bsl::strtok(SPEC, ":")), 0 };
                const char f2[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f3[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f4[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f5[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f6[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f7[] = { *(bsl::strtok(0, ":") + 2), 0 };

                mX.setField(f7, f6, f5, f4, f3, f2, f1, VA);
                ASSERT(VA == X.field(f7, f6, f5, f4, f3, f2, f1).asElemRef());
                ASSERT(!X.field(f7, f6, f5, f4, f3, f2, f1).isNul2());

                mX.setFieldNull(f7, f6, f5, f4, f3, f2, f1);
                ASSERT(isUnset(X.field(f7, f6, f5, f4,
                                        f3, f2, f1).asElemRef()));
                ASSERT(X.field(f7, f6, f5, f4, f3, f2, f1).isNul2());

                mX.setField(f7, f6, f5, f4, f3, f2, f1, VB);
                ASSERT(VB == X.field(f7, f6, f5, f4, f3, f2, f1).asElemRef());
                ASSERT(!X.field(f7, f6, f5, f4, f3, f2, f1).isNul2());

                mX.setFieldNull(f7, f6, f5, f4, f3, f2, f1);
                ASSERT(isUnset(X.field(f7, f6, f5, f4,
                                        f3, f2, f1).asElemRef()));
                ASSERT(X.field(f7, f6, f5, f4, f3, f2, f1).isNul2());
              } break;
              case 8: {
                const char f1[] = { *(bsl::strtok(SPEC, ":")), 0 };
                const char f2[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f3[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f4[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f5[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f6[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f7[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f8[] = { *(bsl::strtok(0, ":") + 2), 0 };

                mX.setField(f8, f7, f6, f5, f4, f3, f2, f1, VA);
                ASSERT(VA == X.field(f8, f7, f6, f5,
                                     f4, f3, f2, f1).asElemRef());
                ASSERT(!X.field(f8, f7, f6, f5, f4, f3, f2, f1).isNul2());

                mX.setFieldNull(f8, f7, f6, f5, f4, f3, f2, f1);
                ASSERT(isUnset(X.field(f8, f7, f6, f5, f4,
                                        f3, f2, f1).asElemRef()));
                ASSERT(X.field(f8, f7, f6, f5, f4, f3, f2, f1).isNul2());

                mX.setField(f8, f7, f6, f5, f4, f3, f2, f1, VB);
                ASSERT(VB == X.field(f8, f7, f6, f5,
                                     f4, f3, f2, f1).asElemRef());
                ASSERT(!X.field(f8, f7, f6, f5, f4, f3, f2, f1).isNul2());

                mX.setFieldNull(f8, f7, f6, f5, f4, f3, f2, f1);
                ASSERT(isUnset(X.field(f8, f7, f6, f5, f4,
                                        f3, f2, f1).asElemRef()));
                ASSERT(X.field(f8, f7, f6, f5, f4, f3, f2, f1).isNul2());
              } break;
              case 9: {
                const char f1[] = { *(bsl::strtok(SPEC, ":")), 0 };
                const char f2[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f3[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f4[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f5[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f6[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f7[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f8[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f9[] = { *(bsl::strtok(0, ":") + 2), 0 };

                mX.setField(f9, f8, f7, f6, f5, f4, f3, f2, f1, VA);
                ASSERT(VA == X.field(f9, f8, f7, f6, f5,
                                     f4, f3, f2, f1).asElemRef());
                ASSERT(!X.field(f9, f8, f7, f6, f5, f4, f3, f2, f1).isNul2());

                mX.setFieldNull(f9, f8, f7, f6, f5, f4, f3, f2, f1);
                ASSERT(isUnset(X.field(f9, f8, f7, f6, f5, f4,
                                        f3, f2, f1).asElemRef()));
                ASSERT(X.field(f9, f8, f7, f6, f5,
                               f4, f3, f2, f1).isNul2());

                mX.setField(f9, f8, f7, f6, f5, f4, f3, f2, f1, VB);
                ASSERT(VB == X.field(f9, f8, f7, f6, f5,
                                     f4, f3, f2, f1).asElemRef());
                ASSERT(!X.field(f9, f8, f7, f6, f5, f4, f3, f2, f1).isNul2());

                mX.setFieldNull(f9, f8, f7, f6, f5, f4, f3, f2, f1);
                ASSERT(isUnset(X.field(f9, f8, f7, f6, f5, f4,
                                        f3, f2, f1).asElemRef()));
                ASSERT(X.field(f9, f8, f7, f6, f5,
                               f4, f3, f2, f1).isNul2());
              } break;
              case 10: {
                const char f1 [] = { *(bsl::strtok(SPEC, ":")), 0 };
                const char f2 [] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f3 [] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f4 [] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f5 [] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f6 [] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f7 [] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f8 [] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f9 [] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f10[] = { *(bsl::strtok(0, ":") + 2), 0 };

                mX.setField(f10, f9, f8, f7, f6, f5, f4, f3, f2, f1, VA);
                ASSERT(VA == X.field(f10, f9, f8, f7, f6,
                                     f5, f4, f3, f2, f1).asElemRef());
                ASSERT(!X.field(f10, f9, f8, f7, f6,
                                f5, f4, f3, f2, f1).isNul2());

                mX.setFieldNull(f10, f9, f8, f7, f6, f5, f4, f3, f2, f1);
                ASSERT(isUnset(X.field(f10, f9, f8, f7, f6, f5, f4,
                                        f3, f2, f1).asElemRef()));
                ASSERT(X.field(f10, f9, f8, f7, f6, f5,
                               f4, f3, f2, f1).isNul2());

                mX.setField(f10, f9, f8, f7, f6, f5, f4, f3, f2, f1, VB);
                ASSERT(VB == X.field(f10, f9, f8, f7, f6,
                                     f5, f4, f3, f2, f1).asElemRef());
                ASSERT(!X.field(f10, f9, f8, f7, f6,
                                f5, f4, f3, f2, f1).isNul2());

                mX.setFieldNull(f10, f9, f8, f7, f6, f5, f4, f3, f2, f1);
                ASSERT(isUnset(X.field(f10, f9, f8, f7, f6, f5, f4,
                                        f3, f2, f1).asElemRef()));
                ASSERT(X.field(f10, f9, f8, f7, f6, f5,
                               f4, f3, f2, f1).isNul2());

                const char errFld[] = "ErrorField";

                // Test error output
                {
                    ASSERT_AGG_ERROR(mX.setField(errFld, f9, f8, f7, f6, f5,
                                f4, f3, f2, f1, VA), BCEM_ERR_TBD);

                    ASSERT_AGG_ERROR(mX.setField(f10, errFld, f8, f7, f6, f5,
                                f4, f3, f2, f1, VA), BCEM_ERR_TBD);

                    ASSERT_AGG_ERROR(mX.setField(f10, f9, errFld, f7, f6, f5,
                                f4, f3, f2, f1, VA), BCEM_ERR_TBD);

                    ASSERT_AGG_ERROR(mX.setField(f10, f9, f8, errFld, f6, f5,
                                f4, f3, f2, f1, VA), BCEM_ERR_TBD);

                    ASSERT_AGG_ERROR(mX.setField(f10, f9, f8, f7, errFld, f5,
                                f4, f3, f2, f1, VA), BCEM_ERR_TBD);

                    ASSERT_AGG_ERROR(mX.setField(f10, f9, f8, f7, f6, errFld,
                                f4, f3, f2, f1, VA), BCEM_ERR_TBD);

                    ASSERT_AGG_ERROR(mX.setField(f10, f9, f8, f7, f6, f5,
                                errFld, f3, f2, f1, VA), BCEM_ERR_TBD);

                    ASSERT_AGG_ERROR(mX.setField(f10, f9, f8, f7, f6, f5,
                                f4, errFld, f2, f1, VA), BCEM_ERR_TBD);

                    ASSERT_AGG_ERROR(mX.setField(f10, f9, f8, f7, f6, f5,
                                f4, f3, errFld, f1, VA), BCEM_ERR_TBD);

                    ASSERT_AGG_ERROR(mX.setField(f10, f9, f8, f7, f6, f5,
                                f4, f3, f2, errFld, VA), BCEM_ERR_TBD);
                }
              } break;
              default:
                ASSERT(0);
            }
        }
}

static void testCase3(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
        //
        // Concerns:
        //   1. The primary manipulator can be used to set the value of any
        //      field in the aggregate, either by name or index and by
        //      specifying the value as either a regular data type, a
        //      bdem_ConstElemRef, a bdem_ElemRef or as an aggregate.
        //
        // Plan:
        //   Note that this function after performing error checking calls the
        //   'bdem_Convert::toBdemType' function to set the value.  Thus we
        //   need to ensure that this function passes the arguments correctly
        //   to that function.  We don't need to test the primary manipulator
        //   for each conceivable value/type.  However, for integer and string
        //   fields that have enumeration constraints, we must explicitly test
        //   that the enumerators are correctly translated between string and
        //   integral representations, in both directions.  Additionally, we
        //   select the data so that the private functions are also indirectly
        //   tested.  Construct a set S of schemas (containing one record
        //   definition each) from a set of selected specifications.
        //   Construct an aggregate mX using the constructor that takes a
        //   record definition.  Then test the following:
        //   1. Call the primary manipulator with a field name for aggregates
        //      storing lists and choices.
        //   2. Call the primary manipulator with an index for aggregates
        //      storing scalar arrays, tables, and choice arrays.
        //   3. Call 'setValue' on the entire aggregate and on each field or
        //      item within the aggregate.
        //   Call the setField function with the value specified as a
        //   bdem_ConstElemRef, bdem_ElemRef, bcem_Aggregate and a value of
        //   that type.
        //
        //   Note that we will tentatively rely on the correctness of the
        //   basic accessors and creators.  We are also not concerned about
        //   the absolute correctness of the FindField private function (that
        //   will be done in a later test).
        //
        // Testing:
        //   template <typename VALTYPE>
        //   void setField(NameOrIndex    fieldOrIdx1,
        //                 const VALTYPE& value) const;
        //   template <typename VALTYPE>
        //   void setFieldNull(NameOrIndex fieldOrIdx1) const;
        //   template <typename VALTYPE>
        //   void setItem(int index, const VALTYPE& value) const;
        //   template <typename VALTYPE>
        //   void setValue(const VALTYPE& value) const;
        //
        // Private Functions:
        //   template <typename VALUETYPE>
        //   static void putValue(void                  *object,
        //                        bdem_ElemType::Type    dataType,
        //                        const bdem_RecordDef  *recordDef,
        //                        const VALUETYPE&       value);
        //   bool bcem_Aggregate::descendIntoField(
        //                             bcem_Aggregate_NameOrIndex fieldOrIdx);
        // --------------------------------------------------------------------

        if (verbose) tst::cout << "\nTESTING PRIMARY MANIPULATOR"
                               << "\n==========================="
                               << bsl::endl;

        {
            // Testing setting the value on a VOID aggregate
            Obj mX; const Obj& X = mX;
            Obj mY; const Obj& Y = mY;
            Obj mZ; const Obj& Z = mZ;
            ASSERT(Obj::areEquivalent(X, Y));
            ASSERT(Obj::areEquivalent(X, Z));

            const Obj R1 = mX.setValue(1);
            ASSERT(R1.isError());
            ASSERT(!X.isError());
            ASSERT(Obj::areEquivalent(X, Y));

            const Obj R2 = mX.setValue(Z);
            // This succeeds as conversion from VOID to VOID is valid
            ASSERT(!R2.isError());
            ASSERT(!X.isError());
            ASSERT(Obj::areEquivalent(X, Y));

            const Obj R3 = mX.setValue(Z.asElemRef());
            // This succeeds as conversion from VOID to VOID is valid
            ASSERT(!R3.isError());
            ASSERT(!X.isError());
            ASSERT(Obj::areEquivalent(X, Y));

            const Obj R4 = mX.setValue(R1);
            ASSERT(!R4.isError());
            ASSERT(!X.isError());
            ASSERT(Obj::areEquivalent(X, Y));
        }

        if (veryVerbose) { tst::cout << "\tTest setting field by name"
                                     << bsl::endl; }
        {

            const struct {
                int         d_line;
                const char *d_spec;
            } DATA[] = {
                // Line         Spec
                // ----         ----
                // For List Aggregates
                {   L_,         ":aCa" },
                {   L_,         ":aCa&NT" },
                {   L_,         ":aCa&NF" },
                {   L_,         ":aCa&D0" },
                {   L_,         ":aFa" },
                {   L_,         ":aFa&NT" },
                {   L_,         ":aFa&NF" },
                {   L_,         ":aFa&D0" },
                {   L_,         ":aGa" },
                {   L_,         ":aGa&NT" },
                {   L_,         ":aGa&NF" },
                {   L_,         ":aGa&D0" },
                {   L_,         ":aHa" },
                {   L_,         ":aHa&NT" },
                {   L_,         ":aHa&NF" },
                {   L_,         ":aHa&D0" },
                {   L_,         ":aNa" },
                {   L_,         ":aNa&NT" },
                {   L_,         ":aNa&NF" },
                {   L_,         ":aNa&D0" },
                {   L_,         ":aPa" },
                {   L_,         ":aPa&NT" },
                {   L_,         ":aPa&NF" },
                {   L_,         ":aPa&D0" },
                {   L_,         ":aQa" },
                {   L_,         ":aQa&NT" },
                {   L_,         ":aQa&NF" },
                {   L_,         ":aQa&D0" },
                {   L_,         ":aRa" },
                {   L_,         ":aRa&NT" },
                {   L_,         ":aRa&NF" },
                {   L_,         ":aRa&D0" },
                {   L_,         ":aWa" },
                {   L_,         ":aWa&NT" },
                {   L_,         ":aWa&NF" },
                {   L_,         ":aWa&D0" },
                {   L_,         ":aVa" },
                {   L_,         ":aVa&NT" },
                {   L_,         ":aVa&NF" },
                {   L_,         ":afa" },
                {   L_,         ":afa&NT" },
                {   L_,         ":afa&NF" },

                {   L_,         ":b=tu5v :a$ab" },
                {   L_,         ":b=tu5v :a$ab&NT" },
                {   L_,         ":b=tu5v :a$ab&NF" },

                {   L_,         ":b=tu5v :a^ab" },
                {   L_,         ":b=tu5v :a^ab&NT" },
                {   L_,         ":b=tu5v :a^ab&NF" },

                {   L_,         ":b=tu5v :a!ab" },
                {   L_,         ":b=tu5v :a!ab&NT" },
                {   L_,         ":b=tu5v :a!ab&NF" },

                {   L_,         ":b=tu5v :a/ab" },
                {   L_,         ":b=tu5v :a/ab&NT" },
                {   L_,         ":b=tu5v :a/ab&NF" },

                {   L_,         ":aCbFcGdQf :g+ha" },
                {   L_,         ":aCbFcGdQf :g+ha&NT" },
                {   L_,         ":aCbFcGdQf :g+ha&NF" },

                {   L_,         ":aCbFcGdQf :g#ha" },
                {   L_,         ":aCbFcGdQf :g#ha&NT" },
                {   L_,         ":aCbFcGdQf :g#ha&NF" },

                {   L_,         ":a?CbFcGdQf :g%ha" },
                {   L_,         ":a?CbFcGdQf :g%ha&NT" },
                {   L_,         ":a?CbFcGdQf :g%ha&NF" },

                {   L_,         ":a?CbFcGdQf :g@ha" },
                {   L_,         ":a?CbFcGdQf :g@ha&NT" },
                {   L_,         ":a?CbFcGdQf :g@ha&NF" },

                // For Choice Aggregates
                {   L_,         ":a?Ca" },
                {   L_,         ":a?Ca&NT" },
                {   L_,         ":a?Ca&NF" },
                {   L_,         ":a?Ca&D0" },
                {   L_,         ":a?Fa" },
                {   L_,         ":a?Fa&NT" },
                {   L_,         ":a?Fa&NF" },
                {   L_,         ":a?Fa&D0" },
                {   L_,         ":a?Ga" },
                {   L_,         ":a?Ga&NT" },
                {   L_,         ":a?Ga&NF" },
                {   L_,         ":a?Ga&D0" },
                {   L_,         ":a?Ha" },
                {   L_,         ":a?Ha&NT" },
                {   L_,         ":a?Ha&NF" },
                {   L_,         ":a?Ha&D0" },
                {   L_,         ":a?Na" },
                {   L_,         ":a?Na&NT" },
                {   L_,         ":a?Na&NF" },
                {   L_,         ":a?Na&D0" },
                {   L_,         ":a?Pa" },
                {   L_,         ":a?Pa&NT" },
                {   L_,         ":a?Pa&NF" },
                {   L_,         ":a?Pa&D0" },
                {   L_,         ":a?Qa" },
                {   L_,         ":a?Qa&NT" },
                {   L_,         ":a?Qa&NF" },
                {   L_,         ":a?Qa&D0" },
                {   L_,         ":a?Ra" },
                {   L_,         ":a?Ra&NT" },
                {   L_,         ":a?Ra&NF" },
                {   L_,         ":a?Ra&D0" },
                {   L_,         ":a?Wa" },
                {   L_,         ":a?Wa&NT" },
                {   L_,         ":a?Wa&NF" },
                {   L_,         ":a?Wa&D0" },
                {   L_,         ":a?Va" },
                {   L_,         ":a?Va&NT" },
                {   L_,         ":a?Va&NF" },
                {   L_,         ":a?fa" },
                {   L_,         ":a?fa&NT" },
                {   L_,         ":a?fa&NF" },

                {   L_,         ":b=tu5v :a?$ab" },
                {   L_,         ":b=tu5v :a?$ab&NT" },
                {   L_,         ":b=tu5v :a?$ab&NF" },
                {   L_,         ":b=tu5v :a?$ab&D0" },
                {   L_,         ":b=tu5v :a?^ab" },
                {   L_,         ":b=tu5v :a?^ab&NT" },
                {   L_,         ":b=tu5v :a?^ab&NF" },
                {   L_,         ":b=tu5v :a?^ab&D0" },
                {   L_,         ":b=tu5v :a?!ab" },
                {   L_,         ":b=tu5v :a?!ab&NT" },
                {   L_,         ":b=tu5v :a?!ab&NF" },
                {   L_,         ":b=tu5v :a?!ab&D0" },
                {   L_,         ":b=tu5v :a?/ab" },
                {   L_,         ":b=tu5v :a?/ab&NT" },
                {   L_,         ":b=tu5v :a?/ab&NF" },
                {   L_,         ":b=tu5v :a?/ab&D0" },
                {   L_,         ":aCbFcGdQf  :g?+ha" },
                {   L_,         ":aCbFcGdQf  :g?+ha&NT" },
                {   L_,         ":aCbFcGdQf  :g?+ha&NF" },
                {   L_,         ":aCbFcGdQf  :g?#ha" },
                {   L_,         ":aCbFcGdQf  :g?#ha&NT" },
                {   L_,         ":aCbFcGdQf  :g?#ha&NF" },
                {   L_,         ":a?CbFcGdQf :g?%ha" },
                {   L_,         ":a?CbFcGdQf :g?%ha&NT" },
                {   L_,         ":a?CbFcGdQf :g?%ha&NF" },
                {   L_,         ":a?CbFcGdQf :g?@ha" },
                {   L_,         ":a?CbFcGdQf :g?@ha&NT" },
                {   L_,         ":a?CbFcGdQf :g?@ha&NF" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const char *SPEC = DATA[i].d_spec;
                const bool  NSA  = (bool) bsl::strstr(SPEC, "&FN");

                Schema s; ggSchema(&s, SPEC);
                const RecDef& r = s.record(s.numRecords() - 1);
                ConstRecDefShdPtr rp(&r, NilDeleter(), 0);

                const char           *fldName  = r.fieldName(0);
                ET::Type              TYPE     = r.field(0).elemType();
                const bool            DEF_VAL  = r.field(0).hasDefaultValue();
                const bool            isNull   = !DEF_VAL
                                               || ET::isArrayType(TYPE);
                const EnumDef *const  ENUM_DEF =
                                            r.field(0).enumerationConstraint();

                const CERef CEN = getCERef(TYPE, 0);
                const CERef CEA = getCERef(TYPE, 1);
                const CERef CEB = getCERef(TYPE, 2);

                      int NULLNESS_FLAGS   = 1;
                const int NULLNESS_BIT_IDX = 0;

                const CERef NULL_CER(CEN.data(),
                                     CEN.descriptor(),
                                     &NULLNESS_FLAGS,
                                     NULLNESS_BIT_IDX);

                const ERef  EA = getERef(TYPE, 1);
                const ERef  EB = getERef(TYPE, 2);
                const ERef NULL_ER((void *) CEN.data(),
                                   CEN.descriptor(),
                                   &NULLNESS_FLAGS,
                                   NULLNESS_BIT_IDX);

                if (veryVerbose) { T_ T_ P(s) P(CEA) P(EA) }

                bslma_TestAllocator t(veryVeryVerbose);
                Obj mX(rp, &t); const Obj& X = mX;
                ASSERT(rp == X.recordDefPtr());
                Obj mN(TYPE, CEA); const Obj& N = mN;
                mN.makeNull();

                if (veryVerbose) { T_ P(X) };

                if (RecDef::BDEM_CHOICE_RECORD == r.recordType()) {
                    mX.makeSelection(fldName);
                }

                if (!ET::BDEM_LIST == TYPE) {
                    LOOP2_ASSERT(LINE, X, X.field(fldName).isNul2() == isNull);
                }

                // Test setField with bdem_ConstElemRef and bdem_ElemRef
                mX.setField(fldName, CEA);
                ASSERT(CEA == X.field(fldName).asElemRef());
                ASSERT(!X.field(fldName).isNul2());

                mX.setFieldNull(fldName);
                ASSERT(isUnset(X.field(fldName).asElemRef()));
                ASSERT(X.field(fldName).isNul2());

                mX.setField(fldName, CEB);
                ASSERT(CEB == X.field(fldName).asElemRef());
                ASSERT(!X.field(fldName).isNul2());

                mX.setField(fldName, NULL_CER);
                ASSERT(isUnset(X.field(fldName).asElemRef()));
                ASSERT(X.field(fldName).isNul2());

                mX.setField(fldName, EA);
                ASSERT(EA == X.field(fldName).asElemRef());
                ASSERT(!X.field(fldName).isNul2());

                mX.setFieldNull(fldName);
                ASSERT(isUnset(X.field(fldName).asElemRef()));
                ASSERT(X.field(fldName).isNul2());

                mX.setField(fldName, EB);
                ASSERT(EB == X.field(fldName).asElemRef());
                ASSERT(!X.field(fldName).isNul2());

                mX.setField(fldName, NULL_ER);
                ASSERT(isUnset(X.field(fldName).asElemRef()));
                ASSERT(X.field(fldName).isNul2());

                // Test setField with values of the field types
                SetFieldFunctor f1(&mX, fldName);
                funcVisitor(&f1, CEA);
                ASSERT(CEA == X.field(fldName).asElemRef());
                ASSERT(!X.field(fldName).isNul2());

                mX.setFieldNull(fldName);
                ASSERT(isUnset(X.field(fldName).asElemRef()));
                ASSERT(X.field(fldName).isNul2());

                funcVisitor(&f1, CEB);
                ASSERT(CEB == X.field(fldName).asElemRef());
                ASSERT(!X.field(fldName).isNul2());

                mX.setFieldNull(fldName);
                ASSERT(isUnset(X.field(fldName).asElemRef()));
                ASSERT(X.field(fldName).isNul2());

                if (ENUM_DEF) {
                    if (TYPE == ET::BDEM_INT) {
                        // Test setField: INT enumeration as STRING
                        mX.setField(fldName, N06);
                        ASSERT(CEN == X.field(fldName).asElemRef());
                        ASSERT(!X.field(fldName).isNul2());

                        mX.setFieldNull(fldName);
                        ASSERT(isUnset(X.field(fldName).asElemRef()));
                        ASSERT(X.field(fldName).isNul2());

                        mX.setField(fldName, A06);
                        ASSERT(CEA == X.field(fldName).asElemRef());
                        ASSERT(!X.field(fldName).isNul2());

                        mX.setField(fldName, N);
                        ASSERT(isUnset(X.field(fldName).asElemRef()));
                        ASSERT(X.field(fldName).isNul2());

                        mX.setField(fldName, B06);
                        ASSERT(CEB == X.field(fldName).asElemRef());
                        ASSERT(!X.field(fldName).isNul2());

                        mX.setField(fldName, N.asElemRef());
                        ASSERT(isUnset(X.field(fldName).asElemRef()));
                        ASSERT(X.field(fldName).isNul2());
                    }
                    else if (TYPE == ET::BDEM_STRING) {
                        // Test setField: STRING enumeration as INT
                        mX.setField(fldName, N02);
                        ASSERT(CEN == X.field(fldName).asElemRef());
                        ASSERT(!X.field(fldName).isNul2());

                        mX.setFieldNull(fldName);
                        ASSERT(isUnset(X.field(fldName).asElemRef()));
                        ASSERT(X.field(fldName).isNul2());

                        mX.setField(fldName, A02);
                        ASSERT(CEA == X.field(fldName).asElemRef());
                        ASSERT(!X.field(fldName).isNul2());

                        mX.setField(fldName, N);
                        ASSERT(isUnset(X.field(fldName).asElemRef()));
                        ASSERT(X.field(fldName).isNul2());

                        mX.setField(fldName, B02);
                        ASSERT(CEB == X.field(fldName).asElemRef());
                        ASSERT(!X.field(fldName).isNul2());

                        mX.setField(fldName, N.asElemRef());
                        ASSERT(isUnset(X.field(fldName).asElemRef()));
                        ASSERT(X.field(fldName).isNul2());
                    }
                }

                // Test setField with an aggregate value
                Obj mA(TYPE, CEA, &t); const Obj& A = mA;
                mX.setField(fldName, A);
                ASSERT(CEA == X.field(fldName).asElemRef());
                ASSERT(!X.field(fldName).isNul2());

                mX.setFieldNull(fldName);
                ASSERT(isUnset(X.field(fldName).asElemRef()));
                ASSERT(X.field(fldName).isNul2());

                Obj mB(TYPE, CEB, &t); const Obj& B = mB;
                mX.setField(fldName, B);
                ASSERT(CEB == X.field(fldName).asElemRef());
                ASSERT(!X.field(fldName).isNul2());

                mX.setField(fldName, N);
                ASSERT(isUnset(X.field(fldName).asElemRef()));
                ASSERT(X.field(fldName).isNul2());

                // Test empty string if aggregate stores a choice
                if (RecDef::BDEM_CHOICE_RECORD == r.recordType()) {
                    mX.setField("", CEA);
                    ASSERT(CEA == X.field(fldName).asElemRef());
                    ASSERT(!X.field(fldName).isNul2());

                    mX.setFieldNull(fldName);
                    ASSERT(isUnset(X.field(fldName).asElemRef()));
                    ASSERT(X.field(fldName).isNul2());
                }

                // Test setValue with bdem_ConstElemRef and bdem_ElemRef
                bcem_Aggregate mXfld(X.field(fldName));
                const bcem_Aggregate& Xfld = mXfld;
                Xfld.setValue(CEA);
                ASSERT(CEA == Xfld.asElemRef());
                ASSERT(!Xfld.isNul2());

                Xfld.setValue(NULL_CER);
                ASSERT(isUnset(Xfld.asElemRef()));
                ASSERT(Xfld.isNul2());

                Xfld.setValue(CEB);
                ASSERT(CEB == Xfld.asElemRef());
                ASSERT(!Xfld.isNul2());

                Xfld.setValue(EA);
                ASSERT(EA == Xfld.asElemRef());
                ASSERT(!Xfld.isNul2());

                Xfld.setValue(NULL_ER);
                ASSERT(isUnset(Xfld.asElemRef()));
                ASSERT(Xfld.isNul2());

                Xfld.setValue(EB);
                ASSERT(EB == Xfld.asElemRef());
                ASSERT(!Xfld.isNul2());

                // Test setValue with values of the field types
                SetValueFunctor v1(&mXfld);
                funcVisitor(&v1, CEA);
                ASSERT(CEA == Xfld.asElemRef());
                ASSERT(!Xfld.isNul2());

                funcVisitor(&v1, CEB);
                ASSERT(CEB == Xfld.asElemRef());
                ASSERT(!Xfld.isNul2());

                // Test setValue with an aggregate value
                Xfld.setValue(A);
                ASSERT(CEA == Xfld.asElemRef());
                ASSERT(!Xfld.isNul2());

                Xfld.setValue(N);
                ASSERT(isUnset(Xfld.asElemRef()));
                ASSERT(Xfld.isNul2());

                Xfld.setValue(B);
                ASSERT(CEB == Xfld.asElemRef());
                ASSERT(!Xfld.isNul2());

                Xfld.makeNull();
                ASSERT(isUnset(Xfld.asElemRef()));
                ASSERT(Xfld.isNul2());

                if (ENUM_DEF) {
                    if (TYPE == ET::BDEM_INT) {
                        // Test setValue: INT enumeration as STRING
                        Xfld.setValue(N06);
                        ASSERT(CEN == Xfld.asElemRef());
                        ASSERT(!Xfld.isNul2());

                        Xfld.makeNull();
                        ASSERT(isUnset(Xfld.asElemRef()));
                        ASSERT(Xfld.isNul2());

                        Xfld.setValue(A06);
                        ASSERT(CEA == Xfld.asElemRef());
                        ASSERT(!Xfld.isNul2());

                        Xfld.makeNull();
                        ASSERT(isUnset(Xfld.asElemRef()));
                        ASSERT(Xfld.isNul2());

                        Xfld.setValue(B06);
                        ASSERT(CEB == Xfld.asElemRef());
                        ASSERT(!Xfld.isNul2());

                        Xfld.makeNull();
                        ASSERT(isUnset(Xfld.asElemRef()));
                        ASSERT(Xfld.isNul2());
                    }
                    else if (TYPE == ET::BDEM_STRING) {
                        // Test setValue: STRING enumeration as INT
                        Xfld.setValue(N02);
                        ASSERT(CEN == Xfld.asElemRef());
                        ASSERT(!Xfld.isNul2());

                        Xfld.makeNull();
                        ASSERT(isUnset(Xfld.asElemRef()));
                        ASSERT(Xfld.isNul2());

                        Xfld.setValue(A02);
                        ASSERT(CEA == Xfld.asElemRef());
                        ASSERT(!Xfld.isNul2());

                        Xfld.makeNull();
                        ASSERT(isUnset(Xfld.asElemRef()));
                        ASSERT(Xfld.isNul2());

                        Xfld.setValue(B02);
                        ASSERT(CEB == Xfld.asElemRef());
                        ASSERT(!Xfld.isNul2());

                        Xfld.makeNull();
                        ASSERT(isUnset(Xfld.asElemRef()));
                        ASSERT(Xfld.isNul2());
                    }
                }

                // Test error conditions and output

                // Test with an index value
                ASSERT_AGG_ERROR(mX.setField(0, CEA), BCEM_ERR_TBD);

                // Test with invalid field name
                const char *errFldName = "ErrorField";
                ASSERT_AGG_ERROR(mX.setField(errFldName, CEA), BCEM_ERR_TBD);

                // Test that calling a field on an empty aggregate fails
                mX.reset();
                ASSERT_AGG_ERROR(mX.setField(fldName, CEA), BCEM_ERR_TBD);
            }
        }

        if (veryVerbose) { tst::cout << "\tTest setting field by index"
                                     << bsl::endl; }
        {
            const struct {
                int         d_line;
                const char *d_spec;
            } DATA[] = {
                // Line          Spec
                // ----          ----
                {   L_,         ":aKa" },
                {   L_,         ":aKa&NT" },
                {   L_,         ":aKa&NF" },
                {   L_,         ":aKa&D0" },
                {   L_,         ":aKa&FN&D0" },
                {   L_,         ":aLa" },
                {   L_,         ":aLa&NT" },
                {   L_,         ":aLa&NF" },
                {   L_,         ":aLa&D0" },
                {   L_,         ":aLa&FN&D0" },
                {   L_,         ":aMa" },
                {   L_,         ":aMa&NT" },
                {   L_,         ":aMa&NF" },
                {   L_,         ":aMa&D0" },
                {   L_,         ":aMa&FN&D0" },
                {   L_,         ":aNa" },
                {   L_,         ":aNa&NT" },
                {   L_,         ":aNa&NF" },
                {   L_,         ":aNa&D0" },
                {   L_,         ":aNa&FN&D0" },
                {   L_,         ":aOa" },
                {   L_,         ":aOa&NT" },
                {   L_,         ":aOa&NF" },
                {   L_,         ":aOa&D0" },
                {   L_,         ":aOa&FN&D0" },
                {   L_,         ":aPa" },
                {   L_,         ":aPa&NT" },
                {   L_,         ":aPa&NF" },
                {   L_,         ":aPa&D0" },
                {   L_,         ":aPa&FN&D0" },
                {   L_,         ":aQa" },
                {   L_,         ":aQa&NT" },
                {   L_,         ":aQa&NF" },
                {   L_,         ":aQa&D0" },
                {   L_,         ":aQa&FN&D0" },
                {   L_,         ":aRa" },
                {   L_,         ":aRa&NT" },
                {   L_,         ":aRa&NF" },
                {   L_,         ":aRa&D0" },
                {   L_,         ":aRa&FN&D0" },
                {   L_,         ":aSa" },
                {   L_,         ":aSa&NT" },
                {   L_,         ":aSa&NF" },
                {   L_,         ":aSa&D0" },
                {   L_,         ":aSa&FN&D0" },
                {   L_,         ":aTa" },
                {   L_,         ":aTa&NT" },
                {   L_,         ":aTa&NF" },
                {   L_,         ":aTa&D0" },
                {   L_,         ":aTa&FN&D0" },

                {   L_,         ":aVa" },
                {   L_,         ":aVa&NT" },
                {   L_,         ":aVa&NF" },
                {   L_,         ":aaa" },
                {   L_,         ":aaa&NT" },
                {   L_,         ":aaa&NF" },
                {   L_,         ":aaa&D0" },
                {   L_,         ":aaa&FN&D0" },
                {   L_,         ":aba" },
                {   L_,         ":aba&NT" },
                {   L_,         ":aba&NF" },
                {   L_,         ":aba&D0" },
                {   L_,         ":aba&FN&D0" },
                {   L_,         ":aca" },
                {   L_,         ":aca&NT" },
                {   L_,         ":aca&NF" },
                {   L_,         ":aca&D0" },
                {   L_,         ":aca&FN&D0" },
                {   L_,         ":ada" },
                {   L_,         ":ada&NT" },
                {   L_,         ":ada&NF" },
                {   L_,         ":ada&D0" },
                {   L_,         ":ada&FN&D0" },
                {   L_,         ":afa" },
                {   L_,         ":afa&NT" },
                {   L_,         ":afa&NF" },
                {   L_,         ":aCbFcGdQf :g#ha" },
                {   L_,         ":aCbFcGdQf :g#ha&NT" },
                {   L_,         ":aCbFcGdQf :g#ha&NF" },
                {   L_,         ":aCbFcGdQf :g#ha&FN" },

                {   L_,         ":a?CbFcGdQf :g@ha" },
                {   L_,         ":a?CbFcGdQf :g@ha&NT" },
                {   L_,         ":a?CbFcGdQf :g@ha&NF" },
                {   L_,         ":a?CbFcGdQf :g@ha&FN" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const char *SPEC = DATA[i].d_spec;

                Schema schema; const Schema& SCHEMA = schema;
                ggSchema(&schema, SPEC);
                const RecDef *RECORD = &SCHEMA.record(SCHEMA.numRecords() - 1);

                ConstRecDefShdPtr crp(RECORD, NilDeleter(), 0);
                const ConstRecDefShdPtr& CRP = crp;

                ET::Type    ARRAY_TYPE = RECORD->field(0).elemType();
                ET::Type    SUB_TYPE   = ET::fromArrayType(ARRAY_TYPE);
                const char *fldName    = RECORD->fieldName(0);

                const CERef ARRAY_CEA = getCERef(ARRAY_TYPE, 1);
                const CERef ARRAY_CEB = getCERef(ARRAY_TYPE, 2);
                const int   LEN       = getLength(ARRAY_CEA);

                const CERef CEA = getCERef(SUB_TYPE, 1);
                const CERef CEB = getCERef(SUB_TYPE, 2);
                const ERef  EA  = getERef(SUB_TYPE, 1);
                const ERef  EB  = getERef(SUB_TYPE, 2);

                if (veryVerbose) { T_ P(SPEC) P(SCHEMA) };

                bslma_TestAllocator testAllocator(veryVeryVerbose);

                Obj mW(CRP, &testAllocator); const Obj& W = mW;
                mW.setField(fldName, ARRAY_CEA);
                ASSERT(!W.field(fldName).isNul2());

                Obj mX(CRP, &testAllocator); const Obj& X = mX;
                mX.setField(fldName, ARRAY_CEA);
                ASSERT(!X.field(fldName).isNul2());

                Obj mY = X.field(fldName); const Obj& Y = mY;
                ASSERT(LEN == Y.length());
                ASSERT(!Y.isNul2());

                Obj mZ = W.field(fldName); const Obj& Z = mZ;
                ASSERT(LEN == Z.length());
                ASSERT(!Z.isNul2());

                if (veryVerbose) { T_ P(X) P(Y) };
                for (int j = 0; j < LEN; ++j) {
                    mY.setField(j, CEA);
                    mZ.setItem(j, CEA);
                    LOOP_ASSERT(j, compareCERefs(CEA, Y.field(j).asElemRef()));
                    LOOP_ASSERT(j, compareCERefs(CEA, Z.field(j).asElemRef()));
                    LOOP_ASSERT(Y.field(j).asElemRef(),
                                !isUnset(Y.field(j).asElemRef()));
                    LOOP_ASSERT(Z.field(j).asElemRef(),
                                !isUnset(Z.field(j).asElemRef()));

                    mY.setFieldNull(j);
                    mZ.setItemNull(j);
                    ASSERT(isUnset(Y.field(j).asElemRef()));
                    ASSERT(isUnset(Z.field(j).asElemRef()));

                    mY.setField(j, CEB);
                    mZ.setItem(j, CEB);
                    LOOP_ASSERT(j, compareCERefs(CEB, Y.field(j).asElemRef()));
                    LOOP_ASSERT(j, compareCERefs(CEB, Z.field(j).asElemRef()));
                    if (ET::BDEM_BOOL != SUB_TYPE) {
                        ASSERT(!isUnset(Y.field(j).asElemRef()));
                        ASSERT(!isUnset(Z.field(j).asElemRef()));
                    }

                    mY.setFieldNull(j);
                    mZ.setItemNull(j);
                    ASSERT(isUnset(Y.field(j).asElemRef()));
                    ASSERT(isUnset(Z.field(j).asElemRef()));

                    mY.setField(j, EA);
                    mZ.setItem(j, EA);
                    LOOP_ASSERT(j, compareCERefs(EA, Y.field(j).asElemRef()));
                    LOOP_ASSERT(j, compareCERefs(EA, Z.field(j).asElemRef()));
                    if (ET::BDEM_BOOL != SUB_TYPE) {
                        ASSERT(!isUnset(Y.field(j).asElemRef()));
                        ASSERT(!isUnset(Z.field(j).asElemRef()));
                    }

                    mY.setFieldNull(j);
                    mZ.setItemNull(j);
                    ASSERT(isUnset(Y.field(j).asElemRef()));
                    ASSERT(isUnset(Z.field(j).asElemRef()));

                    mY.setField(j, EB);
                    mZ.setItem(j, EB);
                    LOOP_ASSERT(j, compareCERefs(EB, Y.field(j).asElemRef()));
                    LOOP_ASSERT(j, compareCERefs(EB, Z.field(j).asElemRef()));
                    if (ET::BDEM_BOOL != SUB_TYPE) {
                        ASSERT(!isUnset(Y.field(j).asElemRef()));
                        ASSERT(!isUnset(Z.field(j).asElemRef()));
                    }

                    mY.setFieldNull(j);
                    mZ.setItemNull(j);
                    ASSERT(isUnset(Y.field(j).asElemRef()));
                    ASSERT(isUnset(Z.field(j).asElemRef()));

                    // Test setField with values of the field types
                    SetFieldFunctor f(&mY, j);
                    funcVisitor(&f, CEA);
                    LOOP_ASSERT(j, compareCERefs(CEA, Y.field(j).asElemRef()));
                    ASSERT(!isUnset(Y.field(j).asElemRef()));

                    mY.setFieldNull(j);
                    ASSERT(isUnset(Y.field(j).asElemRef()));

                    funcVisitor(&f, CEB);
                    LOOP_ASSERT(j, compareCERefs(CEB, Y.field(j).asElemRef()));
                    if (ET::BDEM_BOOL != SUB_TYPE) {
                        ASSERT(!isUnset(Y.field(j).asElemRef()));
                    }

                    mY.setFieldNull(j);
                    ASSERT(isUnset(Y.field(j).asElemRef()));

                    // Test setField with an aggregate value
                    Obj mU(CRP, &testAllocator); const Obj& U = mU;
                    mU.setField(fldName, ARRAY_CEA);

                    Obj mV(CRP, &testAllocator); const Obj& V = mV;
                    mV.setField(fldName, ARRAY_CEB);

                    Obj mA = U.field(fldName).field(0); const Obj& A = mA;
                    Obj mB = V.field(fldName).field(0); const Obj& B = mB;

                    mY.setField(j, A);
                    LOOP_ASSERT(j, compareCERefs(A.asElemRef(),
                                                 Y.field(j).asElemRef()));
                    ASSERT(!isUnset(Y.field(j).asElemRef()));

                    mY.setFieldNull(j);
                    ASSERT(isUnset(Y.field(j).asElemRef()));

                    mY.setField(j, B);
                    LOOP_ASSERT(j, compareCERefs(B.asElemRef(),
                                                 Y.field(j).asElemRef()));
                    if (ET::BDEM_BOOL != SUB_TYPE) {
                        ASSERT(!isUnset(Y.field(j).asElemRef()));
                    }

                    mY.setFieldNull(j);
                    ASSERT(isUnset(Y.field(j).asElemRef()));

                    mZ.setItem(j, A);
                    LOOP_ASSERT(j, compareCERefs(A.asElemRef(),
                                                 Z.field(j).asElemRef()));
                    ASSERT(!isUnset(Z.field(j).asElemRef()));

                    mZ.setFieldNull(j);
                    ASSERT(isUnset(Z.field(j).asElemRef()));

                    mZ.setItem(j, B);
                    LOOP_ASSERT(j, compareCERefs(B.asElemRef(),
                                                 Z.field(j).asElemRef()));
                    if (ET::BDEM_BOOL != SUB_TYPE) {
                        ASSERT(!isUnset(Z.field(j).asElemRef()));
                    }

                    mZ.setFieldNull(j);
                    ASSERT(isUnset(Z.field(j).asElemRef()));
                }

                // Test error output
                const char *errFldName = "ErrorField";

                ASSERT_AGG_ERROR(mY.setField(errFldName, CEA), BCEM_ERR_TBD);

                ASSERT_AGG_ERROR(mY.setField(LEN, CEA), BCEM_ERR_TBD);

                ASSERT_AGG_ERROR(mY.setField(-1, CEA), BCEM_ERR_TBD);
            }
        }

        if (veryVerbose) { tst::cout << "\tTest setting field for"
                                     << " nillable arrays" << bsl::endl; }
        {
            const struct {
                int         d_line;
                const char *d_spec;
            } DATA[] = {
                // Line          Spec
                // ----          ----
                {   L_,         ":aKa&FN" },
                {   L_,         ":aLa&FN" },
                {   L_,         ":aMa&FN" },
                {   L_,         ":aNa&FN" },
                {   L_,         ":aOa&FN" },
                {   L_,         ":aPa&FN" },
                {   L_,         ":aQa&FN" },
                {   L_,         ":aRa&FN" },
                {   L_,         ":aSa&FN" },
                {   L_,         ":aTa&FN" },

                {   L_,         ":aaa&FN" },
                {   L_,         ":aba&FN" },
                {   L_,         ":aca&FN" },
                {   L_,         ":ada&FN" },
                {   L_,         ":b=tu5v :a!ab&FN" },
                {   L_,         ":b=tu5v :a/ab&FN" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const char *SPEC = DATA[i].d_spec;

                Schema schema; const Schema& SCHEMA = schema;
                ggSchema(&schema, SPEC);
                const RecDef *RECORD = &SCHEMA.record(0);

                ConstRecDefShdPtr crp(RECORD, NilDeleter(), 0);
                const ConstRecDefShdPtr& CRP = crp;

                ET::Type    SUB_TYPE   = SCHEMA.record(1).field(0).elemType();
                ET::Type    ARRAY_TYPE = EType::toArrayType(SUB_TYPE);
                const char *fldName    = RECORD->fieldName(0);
                const EnumDef *const  ENUM_DEF =
                             SCHEMA.record(1).field(0).enumerationConstraint();

                const CERef ARRAY_CEA = getCERef(ARRAY_TYPE, 1);
                const CERef ARRAY_CEB = getCERef(ARRAY_TYPE, 2);
                const int   LEN       = getLength(ARRAY_CEA);

                const CERef CEA = getCERef(SUB_TYPE, 1);
                const CERef CEB = getCERef(SUB_TYPE, 2);
                const CERef CEN = getCERef(SUB_TYPE, 0);
                const ERef  EA  = getERef(SUB_TYPE, 1);
                const ERef  EB  = getERef(SUB_TYPE, 2);

                if (veryVerbose) { T_ P(SPEC) P(SCHEMA) P(ARRAY_CEA) P(CEA) };

                bslma_TestAllocator testAllocator(veryVeryVerbose);

                Obj mW(CRP, &testAllocator); const Obj& W = mW;

                mW.setField(fldName, ARRAY_CEA);
                ASSERT(!W.field(fldName).isNul2());
                ASSERT(compareNillableTable(W.field(fldName), ARRAY_CEA));

                Obj mD(CRP, &testAllocator); const Obj& D = mD;
                Obj mE(CRP, &testAllocator); const Obj& E = mE;
                Obj mS = mD.field(fldName); const Obj& S = mS;

                assignToNillableTable(&mS, ARRAY_CEA);
                ASSERT(!D.field(fldName).isNul2());
                ASSERT(compareNillableTable(D.field(fldName), ARRAY_CEA));

                mE.setField(fldName, S);
                ASSERT(compareNillableTable(E.field(fldName), ARRAY_CEA));

                assignToNillableTable(&mS, ARRAY_CEB);
                ASSERT(!S.isNul2());
                ASSERT(compareNillableTable(S, ARRAY_CEB));

                mE.setField(fldName, S);
                ASSERT(compareNillableTable(E.field(fldName), ARRAY_CEB));

                Obj mX(CRP, &testAllocator); const Obj& X = mX;
                if (veryVerbose) { P(X) }

                mX.setField(fldName, ARRAY_CEA);
                ASSERT(!X.field(fldName).isNul2());
                ASSERT(compareNillableTable(X.field(fldName), ARRAY_CEA));

                if (veryVerbose) { P(X) }

                Obj mY = X.field(fldName); const Obj& Y = mY;

                if (veryVerbose) { P(Y) }

                ASSERT(LEN == Y.length());
                ASSERT(!Y.isNul2());

                Obj mZ = W.field(fldName); const Obj& Z = mZ;
                ASSERT(LEN == Z.length());
                ASSERT(!Z.isNul2());

                if (veryVerbose) { T_ P(X) P(Y) };
                for (int j = 0; j < LEN; ++j) {
                    mY.setField(j, CEA);
                    LOOP_ASSERT(j, compareCERefs(CEA ,
                                      Y.field(j).asElemRef()));
                    LOOP_ASSERT(j, compareNillableElement(Y.field(j), CEA));
                    LOOP_ASSERT(j, compareNillableElement(Y.field(j),
                                                          ARRAY_CEA, j));
                    mZ.setItem(j, Y.field(j));
                    LOOP_ASSERT(j, compareCERefs(CEA,
                                      Z.field(j).asElemRef()));
                    LOOP_ASSERT(j, compareNillableElement(Z.field(j), CEA));
                    LOOP_ASSERT(j, compareNillableElement(Z.field(j),
                                                          ARRAY_CEA, j));
                    LOOP_ASSERT(Y.field(j).asElemRef(),
                                !isUnset(Y.field(j).asElemRef()));
                    LOOP_ASSERT(Y.field(j).asElemRef(),
                                !Y.field(j).asElemRef().isNull());
                    LOOP_ASSERT(Z.field(j).asElemRef(),
                                !isUnset(Z.field(j).asElemRef()));
                    LOOP_ASSERT(Z.field(j).asElemRef(),
                                !Z.field(j).asElemRef().isNull());

                    LOOP_ASSERT(Y.field(j), !Y.field(j).isNul2());
                    LOOP_ASSERT(Z.field(j), !Z.field(j).isNul2());

                    mY.setFieldNull(j);
                    mZ.setItemNull(j);
                    ASSERT(isUnset(Y.field(j).asElemRef()));
                    ASSERT(Y.field(j).asElemRef().isNull());
                    ASSERT(isUnset(Z.field(j).asElemRef()));
                    ASSERT(Z.field(j).asElemRef().isNull());

                    LOOP_ASSERT(Y.field(j), Y.field(j).isNul2());
                    LOOP_ASSERT(Z.field(j), Z.field(j).isNul2());

                    mY.setField(j, CEB);
                    LOOP_ASSERT(j, compareCERefs(CEB,
                                      Y.field(j).asElemRef()));
                    LOOP_ASSERT(j, compareNillableElement(Y.field(j), CEB));
                    LOOP_ASSERT(j, compareNillableElement(Y.field(j),
                                                          ARRAY_CEB, j));
                    mZ.setItem(j, Y.field(j));
                    LOOP_ASSERT(j, compareCERefs(CEB,
                                      Z.field(j).asElemRef()));
                    LOOP_ASSERT(j, compareNillableElement(Z.field(j), CEB));
                    LOOP_ASSERT(j, compareNillableElement(Z.field(j),
                                                          ARRAY_CEB, j));

                    if (ET::BDEM_BOOL != SUB_TYPE) {
                        LOOP_ASSERT(Y.field(j).asElemRef(),
                                    !isUnset(Y.field(j).asElemRef()));
                        LOOP_ASSERT(Y.field(j).asElemRef(),
                                    !Y.field(j).asElemRef().isNull());
                        LOOP_ASSERT(Z.field(j).asElemRef(),
                                    !isUnset(Z.field(j).asElemRef()));
                        LOOP_ASSERT(Z.field(j).asElemRef(),
                                    !Z.field(j).asElemRef().isNull());

                        LOOP_ASSERT(Y.field(j), !Y.field(j).isNul2());
                        LOOP_ASSERT(Z.field(j), !Z.field(j).isNul2());
                    }

                    mY.setFieldNull(j);
                    mZ.setItemNull(j);
                    ASSERT(isUnset(Y.field(j).asElemRef()));
                    ASSERT(Y.field(j).asElemRef().isNull());
                    ASSERT(isUnset(Z.field(j).asElemRef()));
                    ASSERT(Z.field(j).asElemRef().isNull());

                    LOOP_ASSERT(Y.field(j), Y.field(j).isNul2());
                    LOOP_ASSERT(Z.field(j), Z.field(j).isNul2());

                    mY.setField(j, EA);
                    LOOP_ASSERT(j, compareCERefs(EA,
                                      Y.field(j).asElemRef()));
                    LOOP_ASSERT(j, compareNillableElement(Y.field(j), EA));
                    mZ.setItem(j, Y.field(j));
                    LOOP_ASSERT(j, compareCERefs(EA,
                                      Z.field(j).asElemRef()));
                    LOOP_ASSERT(j, compareNillableElement(Z.field(j), EA));
                    ASSERT(!isUnset(Y.field(j).asElemRef()));
                    ASSERT(!isUnset(Z.field(j).asElemRef()));

                    ASSERT(!Y.field(j).isNul2());
                    ASSERT(!Z.field(j).isNul2());

                    mY.setFieldNull(j);
                    mZ.setItemNull(j);
                    ASSERT(isUnset(Y.field(j).asElemRef()));
                    ASSERT(isUnset(Z.field(j).asElemRef()));

                    ASSERT(Y.field(j).isNul2());
                    ASSERT(Z.field(j).isNul2());

                    mY.setField(j, EB);
                    LOOP_ASSERT(j, compareCERefs(EB,
                                      Y.field(j).asElemRef()));
                    LOOP_ASSERT(j, compareNillableElement(Y.field(j), EB));
                    mZ.setItem(j, Y.field(j));
                    LOOP_ASSERT(j, compareCERefs(EB,
                                      Z.field(j).asElemRef()));
                    LOOP_ASSERT(j, compareNillableElement(Z.field(j), EB));
                    if (ET::BDEM_BOOL != SUB_TYPE) {
                        ASSERT(!isUnset(Y.field(j).asElemRef()));
                        ASSERT(!isUnset(Z.field(j).asElemRef()));

                        ASSERT(!Y.field(j).isNul2());
                        ASSERT(!Z.field(j).isNul2());
                    }

                    mY.setFieldNull(j);
                    mZ.setItemNull(j);
                    ASSERT(isUnset(Y.field(j).asElemRef()));
                    ASSERT(isUnset(Z.field(j).asElemRef()));

                    ASSERT(Y.field(j).isNul2());
                    ASSERT(Z.field(j).isNul2());

                    // Test setField with values of the field types
                    SetFieldFunctor f(&mY, j);
                    funcVisitor(&f, CEA);
                    LOOP_ASSERT(j, compareCERefs(
                                      CEA,
                                      Y.field(j).asElemRef()));
                    LOOP_ASSERT(j, compareNillableElement(Y.field(j), CEA));
                    ASSERT(!isUnset(Y.field(j).asElemRef()));
                    ASSERT(!Y.field(j).isNul2());

                    mY.setFieldNull(j);
                    ASSERT(isUnset(Y.field(j).asElemRef()));
                    ASSERT(Y.field(j).isNul2());

                    funcVisitor(&f, CEB);
                    LOOP_ASSERT(j, compareCERefs(
                                      CEB,
                                      Y.field(j).asElemRef()));
                    LOOP_ASSERT(j, compareNillableElement(Y.field(j), CEB));
                    if (ET::BDEM_BOOL != SUB_TYPE) {
                        ASSERT(!isUnset(Y.field(j).asElemRef()));
                        ASSERT(!Y.field(j).isNul2());
                    }

                    mY.setFieldNull(j);
                    ASSERT(isUnset(Y.field(j).asElemRef()));
                    ASSERT(Y.field(j).isNul2());

                    mZ.setItem(j, CEB);

                    // Test setField with an aggregate value
                    Obj mU(CRP, &testAllocator); const Obj& U = mU;
                    mU.setField(fldName, ARRAY_CEA);

                    Obj mV(CRP, &testAllocator); const Obj& V = mV;
                    mV.setField(fldName, Z);

                    Obj mA = U.field(fldName).field(0); const Obj& A = mA;
                    Obj mB = V.field(fldName).field(0); const Obj& B = mB;

                    mY.setField(j, A);
                    LOOP_ASSERT(j, compareCERefs(A.asElemRef(),
                                                 Y.field(j).asElemRef()));
                    ASSERT(!isUnset(Y.field(j).asElemRef()));
                    ASSERT(!Y.field(j).isNul2());

                    mY.setFieldNull(j);
                    ASSERT(isUnset(Y.field(j).asElemRef()));
                    ASSERT(Y.field(j).isNul2());

                    mY.setField(j, B);
                    LOOP_ASSERT(j, compareCERefs(B.asElemRef(),
                                                 Y.field(j).asElemRef()));
                    if (ET::BDEM_BOOL != SUB_TYPE) {
                        ASSERT(!isUnset(Y.field(j).asElemRef()));
                        ASSERT(!Y.field(j).isNul2());
                    }

                    mY.setFieldNull(j);
                    ASSERT(isUnset(Y.field(j).asElemRef()));
                    ASSERT(Y.field(j).isNul2());

                    mZ.setItem(j, A);
                    LOOP_ASSERT(j, compareCERefs(A.asElemRef(),
                                                 Z.field(j).asElemRef()));
                    ASSERT(!isUnset(Z.field(j).asElemRef()));
                    ASSERT(!Z.field(j).isNul2());

                    mZ.setFieldNull(j);
                    ASSERT(isUnset(Z.field(j).asElemRef()));
                    ASSERT(Z.field(j).isNul2());

                    mZ.setItem(j, B);
                    LOOP_ASSERT(j, compareCERefs(B.asElemRef(),
                                                 Z.field(j).asElemRef()));
                    if (ET::BDEM_BOOL != SUB_TYPE) {
                        ASSERT(!isUnset(Z.field(j).asElemRef()));
                        ASSERT(!Z.field(j).isNul2());
                    }

                    mZ.setFieldNull(j);
                    ASSERT(isUnset(Z.field(j).asElemRef()));
                    ASSERT(Z.field(j).isNul2());

                    if (ENUM_DEF) {
                        if (SUB_TYPE == ET::BDEM_INT) {
                            // Test setField: INT enumeration as STRING
                            mY.setField(j, N06);
                            ASSERT(CEN == Y.field(j).asElemRef());
                            ASSERT(!Y.field(j).isNul2());

                            mY.setFieldNull(j);
                            ASSERT(Y.field(j).asElemRef().isNull());
                            ASSERT(Y.field(j).isNul2());

                            mY.setItem(j, A06);
                            LOOP2_ASSERT(A06,
                                         Y.field(j).asString(),
                                         A06 ==
                                        Y.field(j).asString());
                            ASSERT(!Y.field(j).isNul2());

                            mY.setFieldNull(j);
                            ASSERT(Y.field(j).asElemRef().isNull());
                            ASSERT(Y.field(j).isNul2());

                            mY.setField(j, B06);
                            ASSERT(B06 ==
                                        Y.field(j).asString());
                            ASSERT(!Y.field(j).isNul2());
                        }
                        else if (SUB_TYPE == ET::BDEM_STRING) {
                            // Test setField: STRING enumeration as INT
                            mY.setField(j, N02);
                            ASSERT(CEN == Y.field(j).asElemRef());
                            ASSERT(!Y.field(j).isNul2());

                            mY.setFieldNull(j);
                            ASSERT(Y.field(j).asElemRef().isNull());
                            ASSERT(Y.field(j).isNul2());

                            mY.setItem(j, A02);
                            ASSERT(A02 == Y.field(j).asInt());
                            ASSERT(!Y.field(j).isNul2());

                            mY.setFieldNull(j);
                            ASSERT(Y.field(j).asElemRef().isNull());
                            ASSERT(Y.field(j).isNul2());

                            mY.setField(j, B02);
                            ASSERT(B02 == Y.field(j).asInt());
                            ASSERT(!Y.field(j).isNul2());
                        }
                    }
                }
            }
        }

        if (veryVerbose) { tst::cout << "\tChecking that assigning a nillable "
                                 << "scalar array to a non-nillable scalar"
                                 << " array resuls in an error" << bsl::endl; }
        {
            const struct {
                int         d_line;
                const char *d_spec;
            } DATA[] = {
                // Line          Spec
                // ----          ----
                {   L_,         ":aKa :bKa&FN" },
                {   L_,         ":aLa :bLa&FN" },
                {   L_,         ":aMa :bMa&FN" },
                {   L_,         ":aNa :bNa&FN" },
                {   L_,         ":aOa :bOa&FN" },
                {   L_,         ":aPa :bPa&FN" },
                {   L_,         ":aQa :bQa&FN" },
                {   L_,         ":aRa :bRa&FN" },
                {   L_,         ":aSa :bSa&FN" },
                {   L_,         ":aTa :bTa&FN" },

                {   L_,         ":aaa :baa&FN" },
                {   L_,         ":aba :bba&FN" },
                {   L_,         ":aca :bca&FN" },
                {   L_,         ":ada :bda&FN" },
                {   L_,         ":b=tu5v :a!ab :c!ab&FN" },
                {   L_,         ":b=tu5v :a/ab :c/ab&FN" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const char *SPEC = DATA[i].d_spec;

                Schema schema; const Schema& SCHEMA = schema;
                ggSchema(&schema, SPEC);
                const RecDef *R1 = &SCHEMA.record(0);
                const RecDef *R2 = &SCHEMA.record(1);

                if (veryVerbose) { T_ P(SPEC) P(SCHEMA) };

                ConstRecDefShdPtr crp1(R1, NilDeleter(), 0);
                const ConstRecDefShdPtr& CRP1 = crp1;
                ConstRecDefShdPtr crp2(R2, NilDeleter(), 0);
                const ConstRecDefShdPtr& CRP2 = crp2;

                ET::Type    SUB_TYPE   = SCHEMA.record(2).field(0).elemType();
                ET::Type    ARRAY_TYPE = EType::toArrayType(SUB_TYPE);
                const char *name1 = R1->fieldName(0);
                const char *name2 = R2->fieldName(0);

                const CERef ARRAY_CEA = getCERef(ARRAY_TYPE, 1);
                const CERef ARRAY_CEB = getCERef(ARRAY_TYPE, 2);

                if (veryVerbose) { T_ P(SPEC) P(SCHEMA) P(ARRAY_CEA) };

                bslma_TestAllocator testAllocator(veryVeryVerbose);

                Obj mX(CRP1, &testAllocator); const Obj& X = mX;
                Obj mY(CRP2, &testAllocator); const Obj& Y = mY;

                mX.setField(name1, ARRAY_CEA);
                mY.setField(name2, ARRAY_CEB);

                if (veryVerbose) { T_ P(X) P(Y) };

                const Obj RET = mX.setField(name1, Y.field(name2));
                ASSERT(RET.isError());

                if (veryVerbose) { P(RET); P(Y); P(name1); P(name2) }
            }
        }
}


static void testCase2(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        //   Since the helper functions were copied verbatim from
        //   bdem_binding.t.cpp, the test cases for them were also copied from
        //   that test driver.
        // --------------------------------------------------------------------
        // TESTING HELPER FUNCTIONS
        //   Verify that the helper functions used to create instances of
        //   'bdem_Schema', 'bdem_List', and 'bdem_Table' work correctly.
        //
        // Concerns:
        //   - Given valid input, the generator functions create correct
        //     objects.
        //   - The 'ggSchema' function is implemented as a finite state
        //     machine (FSM).  All symbols must be recognized successfully, and
        //     all possible state transitions must be verified.
        //
        // Plan:
        //   This test case was copied from the test case for the generator
        //   function 'gg' in 'bdem_schema'.  First, the test case verifies
        //   that, given a spec, the 'ggSchema' function creates a valid
        //   'bdem_Schema'.  Then we check that the 'ggList' and
        //   'ggTable' methods create correct 'bdem_List' and 'bdem_Table'
        //   instances, respectively, from records in that 'bdem_Schema'.
        //
        //   For testing 'ggSchema', evaluate a series of test strings of
        //   increasing complexity to validate every major state and transition
        //   in the FSM underlying 'ggSchema'.  Add extra tests to validate
        //   less-likely transitions until they are all covered.
        //
        //   After 'bdem_Schema' creation, use records in that schema to create
        //   instances of 'bdem_List' and 'bdem_Table' using the 'ggList'
        //   and 'ggTable' helper functions, respectively.  Verify that the
        //   element types in the lists and tables match the element types in
        //   the corresponding 'bdem_Schema' records.
        //
        // Testing:
        //   void ggSchema(bdem_Schema *schema, const char *spec);
        //   void ggList(bdem_List *list, const bdem_RecordDef *record);
        //   void ggTable(bdem_Table *table, const bdem_RecordDef *record);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING HELPER FUNCTIONS" << endl
                          << "========================" << endl;

//        const int MAX_TYPES = 100;
//        ET::Type  tabColTypes[MAX_TYPES];
        {
            if (veryVerbose) cout << "Testing with empty string." << endl;

            Schema mS;  const Schema& S = mS;  ggSchema(&mS, "");
            ASSERT(0 == S.numRecords());

            if (veryVeryVerbose) cout << S << endl;
        }

        if (veryVerbose) cout << "Testing all unconstrained types." << endl;
        {
            Schema mS;  const Schema& S = mS;

            for (int i = 0; i < ET::BDEM_NUM_TYPES; ++i) {
                static char spec[] = "~ :* %*";
                spec[5] = bdemType[i];  // replace '%' in 'spec'

                if (veryVeryVerbose) {
                    T_ cout << "Testing using " << spec << endl;
                }

                ggSchema(&mS, spec);
                const RecDef *rec = &S.record(0);

                ASSERT(1              == S.numRecords());
                ASSERT(1              == rec->numFields());
                ASSERT(0              == rec->fieldName(0));
                ASSERT(ggElemTypes[i] == rec->field(0).elemType());
                ASSERT(0              == rec->field(0).recordConstraint());
                ASSERT(!rec->field(0).isNullable());

                List mL;  const List& L = mL;  ggList(&mL, rec);
                Table mT;  const Table& T = mT;
                ggTable(&mT, rec);

                ASSERT(1 == L.length());
                ASSERT(1 == T.numColumns());

                if (veryVerbose) {
                    T_ cout << "Ensure data elements match." << endl;
                }

                for (int len = 0; len < rec->numFields(); ++len) {
                    ASSERT(L[len].type()     == rec->field(len).elemType());
                    ASSERT(T.columnType(len) == rec->field(len).elemType());
                }

                if (veryVeryVerbose) {
                    cout << S << endl << L << endl << T << endl << endl;
                }
            }
        }

        {
            if (veryVerbose) cout << "All unconstrained types, with name c"
                                     " (\"c\")." << endl;

            Schema mS;  const Schema& S = mS;

            for (int i = 0; i < ET::BDEM_NUM_TYPES; ++i) {
                static char spec[] = "~ :* %c";
                spec[5] = bdemType[i];  // replace '%' in 'spec'

                if (veryVeryVerbose) {
                    T_ cout << "Testing using " << spec << endl;
                }

                ggSchema(&mS, spec);
                const RecDef *rec = &S.record(0);

                LOOP_ASSERT(i, 1              == S.numRecords());
                LOOP_ASSERT(i, 0              == recName(*rec));
                LOOP_ASSERT(i, 1              == rec->numFields());
                LOOP_ASSERT(i, streq(rec->fieldName(0), "c"));
                LOOP_ASSERT(i, ggElemTypes[i] == rec->field(0).elemType());
                LOOP_ASSERT(i, 0              ==
                                             rec->field(0).recordConstraint());
                LOOP_ASSERT(i, !rec->field(0).isNullable());

                List mL;  const List& L = mL;  ggList(&mL, rec);

                Table mT;  const Table& T = mT;
                ggTable(&mT, rec);

                LOOP_ASSERT(i, 1 == L.length());
                LOOP_ASSERT(i, 1 == T.numColumns());

                if (veryVerbose) {
                    T_ cout << "Ensure data elements match." << endl;
                }

                for (int len = 0; len < rec->numFields(); ++len) {
                    ASSERT(L[len].type()     == rec->field(len).elemType());
                    ASSERT(T.columnType(len) == rec->field(len).elemType());
                }

                if (veryVeryVerbose) {
                    cout << S << endl << L << endl << T << endl << endl;
                }
            }
        }

        if (veryVerbose) cout << "Testing with all constrained types." << endl;
        {
            Schema mS;  const Schema& S = mS;

            for (int i = 0; i < NUM_CONSTR_TYPES; ++i) {
                // Construct a different schema spec for different constrained
                // types.
                const char  bdemType = constrBdemType[i];
                const char *spec = 0;
                switch (bdemType) {
                  case '+': spec = "~ :c  Ad :* +dc"; break; // LIST
                  case '#': spec = "~ :c  Ad :* #dc"; break; // TABLE
                  case '%': spec = "~ :c? Ad :* %dc"; break; // CHOICE
                  case '@': spec = "~ :c? Ad :* @dc"; break; // CHOICE_ARRAY
                  case '$': spec = "~ :c= a2 :* $dc"; break; // INT
                  case '^': spec = "~ :c= a2 :* ^dc"; break; // STRING
                  case '!': spec = "~ :c= a2 :* !dc"; break; // INT_ARRAY
                  case '/': spec = "~ :c= a2 :* /dc"; break; // STRING_ARRAY
                }

                if (veryVeryVerbose) cout << "Testing using " << spec << endl;

                ggSchema(&mS, spec);

                if (veryVerbose) {
                    P(S)
                }

                const RecDef *rec = 0;
                if ('$' == bdemType || '^' == bdemType ||
                    '!' == bdemType || '/' == bdemType) {

                    // Testing enumeration constraint
                    LOOP_ASSERT(i, 1 == S.numRecords());
                    LOOP_ASSERT(i, 1 == S.numEnumerations());

                    const EnumDef *en = &S.enumeration(0);

                    LOOP_ASSERT(i, streq(enumName(*en), "c"));
                    LOOP_ASSERT(i, 1 == en->numEnumerators());
                    LOOP_ASSERT(i, streq(en->lookupName(2), "a"));

                    rec = &S.record(0);
                    LOOP_ASSERT(i, &S.enumeration(0) ==
                                rec->field(0).enumerationConstraint());
                }
                else {
                    // Testing record constraint
                    LOOP_ASSERT(i, 2 == S.numRecords());
                    LOOP_ASSERT(i, 0 == S.numEnumerations());

                    rec = &S.record(0);

                    LOOP_ASSERT(i, streq(recName(*rec), "c"));
                    LOOP_ASSERT(i, 1 == rec->numFields());
                    LOOP_ASSERT(i, streq(rec->fieldName(0), "d"));
                    LOOP_ASSERT(i, ggElemTypes[0] == rec->field(0).elemType());
                    LOOP_ASSERT(i, !rec->field(0).isNullable());

                    rec = &S.record(1);
                    LOOP_ASSERT(i, &S.record(0) ==
                                rec->field(0).recordConstraint());
                    LOOP_ASSERT(i, !rec->field(0).isNullable());
                }
                LOOP_ASSERT(i, 0 == recName(*rec));
                LOOP_ASSERT(i, 1 == rec->numFields());
                LOOP_ASSERT(i, streq(rec->fieldName(0), "d"));
                ASSERT(ggElemTypes[32 + i]   == rec->field(0).elemType());

                if (bdemType != '%' && bdemType != '@')
                {
                    List mL;  const List& L = mL;  ggList(&mL, rec);

                    Table mT;  const Table& T = mT;
                    ggTable(&mT, rec);

                    LOOP_ASSERT(i, 1 == L.length());
                    LOOP_ASSERT(i, 1 == T.numColumns());

                    if (veryVerbose) {
                        T_ cout << "Ensure data elements match." << endl;
                    }

                    for (int len = 0; len < rec->numFields(); ++len) {
                        ASSERT(L[len].type()     ==
                               rec->field(len).elemType());
                        ASSERT(T.columnType(len) ==
                               rec->field(len).elemType());
                    }

                    if (veryVeryVerbose) {
                        cout << S << endl << L << endl << T << endl << endl;
                    }
                }
            }
        }

        if (veryVerbose) cout << "Testing with a large invocation"
                                 " touching every token." << endl;
        {
            Schema mS;  const Schema& S = mS;
            bsl::string spec = "~";

            int i, j;  // loop variables declared here to keep MSVC++ happy
            for (i = 0; i < NUM_GG_NAMES; ++i) {
                spec += "\n:";
                spec += name[i];
                if (i % 2) {
                   spec += '?';  // odd 'i' are CHOICE records
                }
                for (j = 0; j < ET::BDEM_NUM_TYPES; ++j) {
                    spec += ' ';
                    spec += bdemType[j];
                    if (j < NUM_GG_NAMES) {
                        spec += name[j];
                    }
                    else {
                        // test space transition in state FLD_NAME.
                        spec += " *";
                    }
                }

                // Add constrained fields by name on previous record.
                if (i > 1) {
                       if (i % 2) {
                            spec += " +*";
                            spec += name[i - 1];

                            spec += " #*";
                            spec += name[i - 1];
                        }
                        else {
                            spec += " %*";
                            spec += name[i - 1];

                            spec += " @*";
                            spec += name[i - 1];
                        }
                }

                // Add constrained fields by index on all previous records,
                // and on current record.  Note that this can only
                // reference the first 10 records, since we use a single
                // digit for reference by index.
                for (j = 0; j < i && j <= 9; ++j) {
                    if (j % 2) {
                         // Trailing space to exercise space transition in
                         // state CFLDCONSTR.
                         spec += " %* ";
                         spec += char(j + '0');

                         // Inner space to exercise space transition in
                         // state CFLDNAME.
                         spec += " @ * ";
                         spec += char(j + '0');
                    }
                    else {
                         spec += " +* ";
                         spec += char(j + '0');

                         spec += " # *";
                         spec += char(j + '0');
                     }
                 }
            }

            if (veryVerbose) { T_ cout << "Testing using " << spec << endl; }

            ggSchema(&mS, spec.data());
            ASSERT(NUM_GG_NAMES == S.numRecords());

            // Note: reducing the number of iterations of the following loop
            //       speeds up this case *enormously*.

            if (veryVeryVerbose) {T_ cout << "Schema created" << endl; }

            for (i = 0; i < 5;  ++i) {
                const RecDef *R = &S.record(i);

                LOOP_ASSERT(i,0 != R);
                LOOP_ASSERT(i,streq(recName(*R), csName(i)));

                for (j = 0; j < ET::BDEM_NUM_TYPES; ++j) {
                    LOOP2_ASSERT(i,j,ggElemTypes[j] == R->field(j).elemType());
                    LOOP2_ASSERT(i,j,0 == R->field(j).recordConstraint());
                    LOOP2_ASSERT(i,j,!R->field(j).isNullable());
                }

                if (i > 1) {
                    // Two fields constrained by name on previous record.
                    int k = i % 2 ? 0 : 2;  // "+#" or "%@"
                    for (j = ET::BDEM_NUM_TYPES;
                         j < ET::BDEM_NUM_TYPES + 1; ++j, ++k) {
                         LOOP2_ASSERT(i,j,getType(constrBdemType[k]) ==
                                                       R->field(j).elemType());
                         LOOP2_ASSERT(i,j,0 != R->field(j).recordConstraint());
                         LOOP2_ASSERT(i,j,!R->field(j).isNullable());
                         LOOP2_ASSERT(i,j,
                                streq(recName(*R->field(j).recordConstraint()),
                                            csName(i - 1)));
                    }
                }

                List mL;  const List& L = mL;  ggList(&mL, R);

                Table mT;  const Table& T = mT;
                ggTable(&mT, R);

                ASSERT(R->numFields() == L.length());
                ASSERT(R->numFields() == T.numColumns());

                if (veryVerbose) {
                    T_ cout << "Ensure data elements match." << endl;
                }

                for (int len = 0; len < R->numFields(); ++len) {
                    ASSERT(L.elemType(len)   == R->field(len).elemType());
                    ASSERT(T.columnType(len) == R->field(len).elemType());
                }
                if (veryVeryVerbose) {
                    cout << L << endl << T << endl << endl;
                }
            }
        }

        {
            Schema mS; const Schema& S = mS; ggSchema(&mS, ":aAa~:a");

            if (veryVerbose) cout << "Force a test of remove transition in"
                                     " state FLD_END." << endl;

            const RecDef *rec = &S.record(S.numRecords() - 1);

            ASSERT(1 == S.numRecords());
            ASSERT(streq(recName(S.record(0)), "a"));

            List mL;  const List& L = mL;  ggList(&mL, rec);

            Table mT;  const Table& T = mT;
            ggTable(&mT, rec);

            if (veryVeryVerbose) {
                cout << S << endl << L << endl << T << endl << endl;
            }
        }

        {
            if (veryVerbose) cout << "Test schema for nullability" << endl;

            for (int i = 0; i < ET::BDEM_NUM_TYPES; ++i) {
                 Schema mS1; const Schema& S1 = mS1;
                 char spec[] = ":a%a &NT";  // Replace % with bdem type
                 spec[2] = bdemType[i];

                 ggSchema(&mS1, spec);

                 if (veryVerbose) {
                     P_(i) P(spec);
                     P(S1);
                 }

                 ASSERT(1 == S1.numRecords());
                 const RecDef *rec = &S1.record(0);

                 ASSERT(streq(recName(*rec), "a"));
                 ASSERT(1 == rec->numFields());
                 const FldDef& fld1 = rec->field(0);

                 ASSERT(fld1.isNullable());

                 if (veryVeryVerbose) {
                     cout << S1 << endl;
                 }

                 spec[bsl::strlen(spec) - 1] = 'F';

                 Schema mS2; const Schema& S2 = mS2;
                 ggSchema(&mS2, spec);

                 if (veryVerbose) {
                     P_(i) P(spec);
                     P(S2);
                 }

                 ASSERT(1 == S2.numRecords());
                 rec = &S2.record(0);

                 ASSERT(streq(recName(*rec), "a"));
                 ASSERT(1 == rec->numFields());
                 const FldDef& fld2 = rec->field(0);

                 ASSERT(!fld2.isNullable());

                 if (veryVeryVerbose) {
                     cout << S2 << endl;
                 }
            }

            const int MAX_SPEC_SIZE = 64;
            char spec[MAX_SPEC_SIZE];
            bsl::memset(spec, 0, sizeof spec);

            for (int i = 0; i < 4; ++i) {
                 Schema mS1; const Schema& S1 = mS1;
                 const char *specValue;
                 int         index = 13;
                 if (i < 2) {
                     specValue = ":aCbFcGdQf :g%ha&NT";    // Replace % with
                                                           // bdem type
                 }
                 else {
                     specValue = ":a?CbFcGdQf :g%ha&NT";   // Replace % with
                                                           // bdem type
                     ++index;
                 }
                 bsl::memcpy(spec, specValue, bsl::strlen(specValue));
                 spec[index] = constrBdemType[i];

                 ggSchema(&mS1, spec);

                 if (veryVerbose) {
                     P_(i) P(spec);
                     P(S1);
                 }

                 ASSERT(2 == S1.numRecords());
                 const RecDef *constraintRec = &S1.record(0);
                 const RecDef *rec           = &S1.record(1);

                 ASSERT(streq(recName(*constraintRec), "a"));
                 ASSERT(streq(recName(*rec), "g"));

                 ASSERT(1 == rec->numFields());
                 const FldDef& fld1 = rec->field(0);

                 ASSERT(fld1.isNullable());

                 if (veryVeryVerbose) {
                     cout << S1 << endl;
                 }

                 spec[bsl::strlen(spec) - 1] = 'F';

                 Schema mS2; const Schema& S2 = mS2;
                 ggSchema(&mS2, spec);

                 if (veryVerbose) {
                     P_(i) P(spec);
                     P(S2);
                 }

                 ASSERT(2 == S2.numRecords());
                 constraintRec = &S2.record(0);
                 rec           = &S2.record(1);

                 ASSERT(streq(recName(*constraintRec), "a"));
                 ASSERT(streq(recName(*rec), "g"));

                 ASSERT(1 == rec->numFields());
                 const FldDef& fld2 = rec->field(0);

                 ASSERT(!fld2.isNullable());

                 if (veryVeryVerbose) {
                     cout << S2 << endl;
                 }
            }
        }

        {
            if (veryVerbose) cout << "Test schema for default value" << endl;

            for (int i = 0; i < ET::BDEM_NUM_TYPES; ++i) {
                ET::Type TYPE = (ET::Type) i;

                if (ET::isAggregateType(TYPE)) {
                    continue;
                }

                Schema mS1; const Schema& S1 = mS1;
                char spec[] = ":a%a &D0";   // Replace % with bdem type
                spec[2] = bdemType[i];

                ggSchema(&mS1, spec);

                if (veryVerbose) {
                    P_(i) P(spec);
                    P(S1);
                }

                ASSERT(1 == S1.numRecords());
                const RecDef *rec = &S1.record(0);

                ASSERT(streq(recName(*rec), "a"));
                ASSERT(1 == rec->numFields());
                const FldDef& fld1 = rec->field(0);

                ASSERT(verifyDefaultValueForType(fld1.defaultValue()));

                if (veryVeryVerbose) {
                    cout << S1 << endl;
                }

                spec[bsl::strlen(spec) - 1] = '1';

                Schema mS2; const Schema& S2 = mS2;
                ggSchema(&mS2, spec);

                if (veryVerbose) {
                    P_(i) P(spec);
                    P(S2);
                }

                ASSERT(1 == S2.numRecords());
                rec = &S2.record(0);

                ASSERT(streq(recName(*rec), "a"));
                ASSERT(1 == rec->numFields());
                const FldDef& fld2 = rec->field(0);

                ASSERT(verifyDefaultValueForType(fld2.defaultValue(), true));

                if (veryVeryVerbose) {
                    cout << S2 << endl;
                }
            }
        }
}

static void testCase1(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   Exercise a broad cross-section of value-semantic functionality
        //   before beginning testing in earnest.  Probe that functionality
        //   systematically and incrementally to discover basic errors in
        //   isolation.
        //
        // Plan:
        //   Confirm that all the functions are called at least once
        //
        // Testing:
        //   BREATHING TEST
        //
        // --------------------------------------------------------------------

        if (verbose) tst::cout << "\nBREATHING TEST"
                               << "\n==============" << bsl::endl;

        if (verbose) tst::cout << "Testing scalar constructors" << bsl::endl;
        {
            bslma_TestAllocator ta (veryVeryVerbose);

            bcem_Aggregate agg1;
            ASSERT(agg1.dataType() == ET::BDEM_VOID);
            ASSERT(agg1.isNul2());
            if (verbose) P(agg1);

            bcem_Aggregate agg2(ET::BDEM_INT, 55, &ta);
            ASSERT(agg2.dataType() == ET::BDEM_INT);
            ASSERT(!agg2.isNul2());
            ASSERT(agg2.asInt() == 55);
            ASSERT(agg2.asString() == "55");
            if (verbose) P(agg2);

            bcem_Aggregate agg3(ET::BDEM_STRING, "", &ta);
            ASSERT(agg3.dataType() == ET::BDEM_STRING);
            ASSERT(agg3.asString() == "");
            if (verbose) P(agg3);
        }

        bslma_TestAllocator sa;
        bcema_SharedPtr<bdem_Schema> schema(new(sa) bdem_Schema(&sa), &sa);

        bdem_RecordDef *level2 = schema->createRecord("Level2");
        level2->appendField(ET::BDEM_STRING_ARRAY, "StringArray2.1");
        bdem_FieldDefAttributes doubleArrayAttr(ET::BDEM_DOUBLE_ARRAY);
        doubleArrayAttr.defaultValue().theModifiableDouble() = 0.125;
        level2->appendField(doubleArrayAttr,  "DoubleArray2.2");
        level2->appendField(ET::BDEM_DATE,         "Date2.3");

        bdem_RecordDef *level1 = schema->createRecord("Level1");
        level1->appendField(ET::BDEM_STRING,       "String1.1");
        level1->appendField(ET::BDEM_DOUBLE,       "Double1.2");
        level1->appendField(ET::BDEM_LIST, level2, "List1.3");
        level1->appendField(ET::BDEM_LIST, level1, "List1.4"); // Recursive

        {
            bslma_TestAllocator ta;

            if (verbose) tst::cout << "Testing record def constructor"
                                   << bsl::endl;
            bcem_Aggregate agg1(schema, "Level1", ET::BDEM_VOID, &ta);
            ASSERT(! agg1.isNul2());

            if (verbose) tst::cout << "Testing list operations" << bsl::endl;
            agg1.setField("Double1.2", 3.4);
            double dbl1 = agg1.field("Double1.2").asDouble();
            ASSERT(3.4 == dbl1);
            double dbl2 = agg1.fieldById(1).asDouble();
            ASSERT(3.4 == dbl2);

            agg1.setField("List1.3", "Date2.3", "2006-02-16");
            bdet_Date date1 = agg1.field("List1.3", "Date2.3").asDate();
            ASSERT(2006 == date1.year());

            bcem_Aggregate agg2(bdem_ElemType::BDEM_DATE, "1996-04-05");
            bcem_Aggregate agg3 = agg1.field("List1.3");
            agg3.setField("Date2.3", agg2);
            bdet_Date date2 = agg1.field("List1.3", "Date2.3").asDate();
            ASSERT(1996 == date2.year());
            if (veryVerbose) P(agg1);

            ASSERT(agg1.field("List1.4").isNul2());
            agg1.field("List1.4").makeValue();
            ASSERT(!agg1.field("List1.4").isNul2());
            ASSERT(agg1.field("List1.4", "List1.4").isNul2());
            agg1.setField("List1.4", "String1.1", "List in list");
            ASSERT("List in list" ==
                   agg1.field("List1.4", "String1.1").asString());

            if (verbose) tst::cout << "Testing array operations" << bsl::endl;
            bcem_Aggregate agg4 = agg1.field("List1.3", "StringArray2.1");
            agg4.resize(2);
            ASSERT(2 == agg4.length());
            agg4.setItem(0, 99.5);
            agg4.setField(1, "Hello");
            ASSERT(99.5 ==
                   agg1.field("List1.3", "StringArray2.1", 0).asDouble());
            ASSERT("Hello" == agg4[1].asString());

            bcem_Aggregate dblArray = agg1.field("List1.3", "DoubleArray2.2");
            agg1.field("List1.3", "DoubleArray2.2").append(3.14159);
            ASSERT(1 == agg1.field("List1.3", "DoubleArray2.2").length());
            ASSERT(3.14159 ==
                   agg1.field("List1.3", "DoubleArray2.2", 0).asDouble());
            dblArray.insert(0, -2e10);
            ASSERT(2 == dblArray.length());
            ASSERT(-2e10 == dblArray[0].asDouble());
            ASSERT(3.14159 ==
                   agg1.field("List1.3", "DoubleArray2.2", 1).asDouble());
            dblArray.resize(4);
            ASSERT(4 == dblArray.length());
            ASSERT(-2e10   == dblArray[0].asDouble());
            ASSERT(3.14159 == dblArray[1].asDouble());
            ASSERT(0.125   == dblArray[2].asDouble());
            ASSERT(0.125   == dblArray[3].asDouble());
            if (veryVerbose) P(agg1);
            dblArray.removeItems(1, 2);
            ASSERT(2 == dblArray.length());
            ASSERT(-2e10 == dblArray[0].asDouble());
            ASSERT(0.125 == dblArray[1].asDouble());

            if (verbose) tst::cout << "Testing by-id access" << bsl::endl;
            agg1.setFieldById(1, 2.4);
            ASSERT(agg1.field("Double1.2").asDouble() == 2.4);
            if (veryVerbose) P(agg1);
        }

        bdem_RecordDef *table1Row = schema->createRecord("Table1Row");
        table1Row->appendField(ET::BDEM_STRING, "StringField");
        table1Row->appendField(ET::BDEM_DOUBLE, "DoubleField");

        {
            bslma_TestAllocator ta;

            if (verbose) tst::cout << "Testing table operations" << bsl::endl;

            bcem_Aggregate agg1(schema, "Table1Row", ET::BDEM_TABLE, &ta);
            ASSERT(ET::BDEM_TABLE == agg1.dataType());
            ASSERT(0 == agg1.length());

            static const double nullDouble = bdetu_Unset<double>::unsetValue();

            agg1.appendItems(2);
            ASSERT(2 == agg1.length());
            agg1.setField(0, "StringField", "Hello");
            agg1.setField(0, "DoubleField", 1.2);
            agg1[1].setField("StringField", "Goodbye");
            ASSERT("Hello" == agg1.field(0, "StringField").asString());
            ASSERT("1.2"   == agg1[0].field("DoubleField").asString());
            bcem_Aggregate agg1Row;
            ASSERT(bdem_ElemType::BDEM_VOID == agg1Row.dataType());
            agg1Row = agg1[1];
            ASSERT(!agg1Row.isNul2());
            ASSERT(bdem_ElemType::BDEM_ROW == agg1Row.dataType());
            ASSERT("Goodbye" == agg1Row.field("StringField").asString());
            ASSERT(nullDouble == agg1Row.field("DoubleField").asDouble());
            if (veryVerbose) P(agg1);

            agg1Row = bcem_Aggregate(schema, "Table1Row");
            agg1Row.setField("StringField", "New row");
            agg1Row.setField("DoubleField", 99.99);
            agg1.insert(1, agg1Row);
            ASSERT(3 == agg1.length());
            ASSERT("Hello"    == agg1.field(0, "StringField").asString());
            ASSERT(1.2        == agg1.field(0, "DoubleField").asDouble());
            LOOP_ASSERT(agg1.field(1, "StringField").asString(), 
                        "New row" == agg1.field(1, "StringField").asString());
            ASSERT(99.99      == agg1.field(1, "DoubleField").asDouble());
            ASSERT("Goodbye"  == agg1.field(2, "StringField").asString());
            ASSERT(nullDouble == agg1.field(2, "DoubleField").asDouble());
            agg1.insertItems(1, 2);
            ASSERT(5 == agg1.length());
            ASSERT("Hello"    == agg1.field(0, "StringField").asString());
            ASSERT(1.2        == agg1.field(0, "DoubleField").asDouble());
            ASSERT("New row"  == agg1.field(3, "StringField").asString());
            ASSERT(99.99      == agg1.field(3, "DoubleField").asDouble());
            ASSERT("Goodbye"  == agg1.field(4, "StringField").asString());
            ASSERT(nullDouble == agg1.field(4, "DoubleField").asDouble());
            if (veryVerbose) P(agg1);
        }

        bdem_RecordDef *choice1 = schema->createRecord(
            "Choice1", bdem_RecordDef::BDEM_CHOICE_RECORD);
        choice1->appendField(ET::BDEM_STRING, "NameSelection");
        choice1->appendField(ET::BDEM_INT,    "IdSelection");

        {
            bslma_TestAllocator ta;

            if (verbose) tst::cout << "Testing choice operations" << bsl::endl;
            bcem_Aggregate agg1(schema, "Choice1", ET::BDEM_CHOICE, &ta);
            ASSERT(2 == agg1.numSelections());
            ASSERT(streq("", agg1.selector()));
            ASSERT(BDEM_NULL_FIELD_ID == agg1.selectorId());
            if (veryVerbose) P(agg1);

            agg1.makeSelection("IdSelection", 64);
            ASSERT(streq("IdSelection", agg1.selector()));
            ASSERT(1 == agg1.selectorId());
            ASSERT(64 == agg1.selection().asShort());
            ASSERT(64 == agg1.field("IdSelection").asInt());
            ASSERT(64 == agg1.field("").asInt());
            if (veryVerbose) P(agg1);

            agg1.makeSelectionById(0);
            ASSERT(streq("NameSelection", agg1.selector()));
            ASSERT(0 == agg1.selectorId());
            agg1.setField("", "pizza");
            ASSERT("pizza" == agg1.selection().asString());
            ASSERT("pizza" == agg1.field("NameSelection").asString());
            ASSERT("pizza" == agg1.field("").asString());
            if (veryVerbose) P(agg1);
        }

        if (verbose) tst::cout << "Testing other constructors" << bsl::endl;

        {
            bslma_TestAllocator ta;
            bslma_DefaultAllocatorGuard allocGuard(&ta);

            // Create shared pointers to some record definitions
            bcema_SharedPtr<bdem_RecordDef> lev1(schema, level1);
            bcema_SharedPtr<const bdem_RecordDef> lev1c(schema, level1);
            bcema_SharedPtr<bdem_RecordDef> ch1(schema, choice1);
            bcema_SharedPtr<const bdem_RecordDef> ch1c(schema, choice1);

            if (veryVerbose)
                tst::cout << "Testing bcem_Aggregate(recordDefPtr)"
                          << bsl::endl;

            // list record (const)
            bcem_Aggregate aggLev1c(lev1c);
            ASSERT(aggLev1c.dataType() == ET::BDEM_LIST);
            ASSERT(! aggLev1c.isNul2());
            ASSERT(ET::BDEM_STRING == aggLev1c.fieldById(0).dataType());
            ASSERT(aggLev1c.recordDefPtr().ptr() == level1);
            if (veryVerbose) P(aggLev1c);

            // list record - non-const (same as const)
            bcem_Aggregate aggLev1(lev1);
            ASSERT(aggLev1.dataType() == ET::BDEM_LIST);
            ASSERT(! aggLev1.isNul2());
            ASSERT(aggLev1.recordDefPtr().ptr() == level1);
            ASSERT(! Obj::areIdentical(aggLev1, aggLev1c));
            ASSERT(  Obj::areEquivalent(aggLev1, aggLev1c));

            // choice record (const)
            bcem_Aggregate aggCh1c(ch1c);
            ASSERT(aggCh1c.dataType() == ET::BDEM_CHOICE);
            aggCh1c.makeSelection(bsl::string("IdSelection"), 3);
            ASSERT(3 == aggCh1c.selection().asInt());
            ASSERT(aggCh1c.recordDefPtr().ptr() == choice1);
            if (veryVerbose) P(aggCh1c);

            // choice record non-const (same as const)
            bcem_Aggregate aggCh1(ch1);
            ASSERT(aggCh1.dataType() == ET::BDEM_CHOICE);
            ASSERT(aggCh1.recordDefPtr().ptr() == choice1);
            aggCh1.makeSelection(bsl::string("IdSelection"), 3);
            ASSERT(! Obj::areIdentical(aggCh1, aggCh1c));
            ASSERT(  Obj::areEquivalent(aggCh1, aggCh1c));

            if (veryVerbose)
                tst::cout << "Testing bcem_Aggregate(recordDefPtr, elemType)"
                          << bsl::endl;

            // table record (const)
            bcem_Aggregate tblLev1c(lev1c, ET::BDEM_TABLE);
            ASSERT(tblLev1c.dataType() == ET::BDEM_TABLE);
            ASSERT(tblLev1c.recordDefPtr().ptr() == level1);
            ASSERT(tblLev1c.length() == 0);
            tblLev1c.appendItems(1);
            ASSERT(tblLev1c.length() == 1);
            ASSERT(tblLev1c.field(0,
                                  "Double1.2").dataType() == ET::BDEM_DOUBLE);
            if (veryVerbose) P(tblLev1c);

            // table record non-const (same as const)
            bcem_Aggregate tblLev1(lev1, ET::BDEM_TABLE);
            ASSERT(tblLev1.recordDefPtr().ptr() == level1);
            ASSERT(tblLev1.length() == 0);
            tblLev1.appendItems(1);
            ASSERT(! Obj::areIdentical(tblLev1, tblLev1c));
            ASSERT(  Obj::areEquivalent(tblLev1, tblLev1c));

            // choice array record (const)
            bcem_Aggregate arrCh1c(ch1c, ET::BDEM_CHOICE_ARRAY);
            ASSERT(arrCh1c.dataType() == ET::BDEM_CHOICE_ARRAY);
            ASSERT(arrCh1c.recordDefPtr().ptr() == choice1);
            ASSERT(arrCh1c.length() == 0);
            arrCh1c.append(aggCh1c);
            ASSERT(arrCh1c.length() == 1);
            ASSERT(arrCh1c[0].selection().dataType() == ET::BDEM_INT);
            ASSERT(arrCh1c[0].selection().asInt() == 3);
            if (veryVerbose) P(arrCh1c);

            // choice array record - non-const (same as const)
            bcem_Aggregate arrCh1(ch1, ET::BDEM_CHOICE_ARRAY);
            ASSERT(arrCh1.recordDefPtr().ptr() == choice1);
            ASSERT(arrCh1.length() == 0);
            arrCh1.append(aggCh1);
            ASSERT(! Obj::areIdentical(arrCh1, arrCh1c));
            ASSERT(  Obj::areEquivalent(arrCh1, arrCh1c));
        }

        if (verbose) tst::cout << "Testing clone" << bsl::endl;
        {
            bslma_TestAllocator da("da", veryVeryVerbose);
            bslma_TestAllocator ta1("ta1", veryVeryVerbose);
            bslma_TestAllocator ta2("ta2", veryVeryVerbose);
            const bsl::string recName = "Level1";

            bslma_DefaultAllocatorGuard allocGuard(&da);
            bcem_Aggregate agg1(schema, recName, ET::BDEM_VOID, &ta1);
            agg1.setField("Double1.2", 3.4);
            agg1.setField("List1.3", "Date2.3", "2006-02-16");
            agg1.field("List1.4").makeValue();
            agg1.setField("List1.4", "String1.1", "List in list");

            const int ta1BlocksUsed = ta1.numBlocksInUse();
            ASSERT(0 == da.numBlocksInUse());
            {
                bcem_Aggregate agg2(agg1.clone(&ta2));

                // New memory came only from ta2
                ASSERT(0 != ta2.numBlocksInUse());
                ASSERT(ta1BlocksUsed == ta1.numBlocksInUse());
                ASSERT(0 == da.numBlocksInUse());

                ASSERT(! Obj::areIdentical(agg1, agg2));
                ASSERT(  Obj::areEquivalent(agg1, agg2));
                ASSERT(agg1.data() != agg2.data());
                ASSERT(&agg1.recordDef() != &agg2.recordDef());
                LOOP_ASSERT(agg2, Obj::areEquivalent(
                                agg1.field("Double1.2"),
                                agg2.field("Double1.2")));
                LOOP_ASSERT(agg2, Obj::areEquivalent(
                                agg1.field("List1.3","Date2.3"),
                                agg2.field("List1.3", "Date2.3")));
                LOOP_ASSERT(agg2, "List in list" ==
                            agg2.field("List1.4", "String1.1").asString());
            }
            ASSERT(0 == ta2.numBlocksInUse());

            {
                bcem_Aggregate agg2(agg1.cloneData(&ta2));

                // New memory came only from ta2
                ASSERT(0 != ta2.numBlocksInUse());
                ASSERT(ta1BlocksUsed == ta1.numBlocksInUse());
                ASSERT(0 == da.numBlocksInUse());

                ASSERT(! Obj::areIdentical(agg1, agg2));
                ASSERT(  Obj::areEquivalent(agg1, agg2));
                ASSERT(agg1.data() != agg2.data());
                ASSERT(&agg1.recordDef() == &agg2.recordDef());
                LOOP_ASSERT(agg2, Obj::areEquivalent(
                                agg1.field("Double1.2"),
                                agg2.field("Double1.2")));
                LOOP_ASSERT(agg2, Obj::areEquivalent(
                                agg1.field("List1.3", "Date2.3"),
                                agg2.field("List1.3", "Date2.3")));
                LOOP_ASSERT(agg2, "List in list" ==
                            agg2.field("List1.4", "String1.1").asString())
            }
            ASSERT(0 == ta2.numBlocksInUse());
            ASSERT(0 == da.numBlocksInUse());
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

    initStaticData ();

    tst::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

#ifdef BSLS_PLATFORM__OS_WINDOWS
    // Suppress all windows debugging popups
    _CrtSetReportMode(_CRT_ASSERT, 0);
    _CrtSetReportMode(_CRT_ERROR,  0);
    _CrtSetReportMode(_CRT_WARN,   0);
#endif

    switch (test) { case 0:  // Zero is always the leading case.
#define CASE(NUMBER) \
    case NUMBER: testCase##NUMBER(verbose, veryVerbose, veryVeryVerbose); break
        CASE(35);
        CASE(34);
        CASE(33);
        CASE(32);
        CASE(31);
        CASE(30);
        CASE(29);
        CASE(28);
        CASE(27);
        CASE(26);
        CASE(25);
        CASE(24);
        CASE(23);
        CASE(22);
        CASE(21);
        CASE(20);
        CASE(19);
        CASE(18);
        CASE(17);
        CASE(16);
        CASE(15);
        CASE(14);
        CASE(13);
        CASE(12);
        CASE(11);
        CASE(10);
        CASE(9);
        CASE(8);
        CASE(7);
        CASE(6);
        CASE(5);
        CASE(4);
        CASE(3);
        CASE(2);
        CASE(1);
#undef CASE
        case -1:
          runBerBenchmark(verbose, veryVerbose, veryVeryVerbose); break;
        default: {
          bsl::cerr << "WARNING: CASE `" << test 
                    << "' NOT FOUND." << bsl::endl;
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
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ---------------------------- END-OF-FILE ---------------------------------
