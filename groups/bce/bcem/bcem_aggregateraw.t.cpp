// bcem_aggregateraw.t.cpp                                            -*-C++-*-

#include <bcem_aggregateraw.h>

#include <bcema_sharedptr.h>

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
#include <bdem_berdecoder.h>
#include <bdem_berencoder.h>

#include <bdex_byteinstream.h>
#include <bdex_byteoutstream.h>
#include <bdex_instreamfunctions.h>
#include <bdex_outstreamfunctions.h>
#include <bdex_testinstream.h>
#include <bdex_testoutstream.h>
#include <bdex_testinstreamexception.h>

#include <bdeat_arrayfunctions.h>
#include <bdeat_choicefunctions.h>
#include <bdeat_enumfunctions.h>
#include <bdeat_nullablevaluefunctions.h>
#include <bdeat_sequencefunctions.h>
#include <bdeat_typecategory.h>
#include <bdeat_typename.h>
#include <bdeat_valuetypefunctions.h>

#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>
#include <bsl_iostream.h>
#include <bsl_stack.h>
#include <bsl_list.h>


using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// This component provides an easy to use interface to the bdem package.  It
// allows users to store complex data, bind it to some meta-data and provides
// access to the stored data by field names or ids as specified in the bound
// meta-data.
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
// [23] int maxSupportedBdexVersion();
// [  ] bdem_ElemType::Type getBdemType(const TYPE& value);
// [ 7] bool areEquivalent(const Obj& lhs, const Obj& rhs);
// [ 7] bool areIdentical(const Obj& lhs, const Obj& rhs);
//
// CREATORS
// [ 8] bcem_AggregateRaw();
// [18] bcem_AggregateRaw(const bcem_AggregateRaw& original);
// [ 8] ~bcem_AggregateRaw();
//
// MANIPULATORS
// [10] bcem_AggregateRaw& operator=(const bcem_AggregateRaw& rhs);
// [11] void setDataType(bdem_ElemType::Type dataType);
// [11] void setDataPointer(void *data);
// [11] void setSchemaPointer(const bdem_Schema *schema);
// [11] void setRecordDefPointer(const bdem_RecordDef *recordDef);
// [11] void setFieldDefPointer(const bdem_FieldDef *fieldDef);
// [11] void setTopLevelAggregateNullnessPointer(int *nullnessFlag);
// [  ] void clearParent();
// [20] void reset();
//
// REFERENCED-VALUE ACCESSORS
// [25] int reserveRaw(Error *error, bsl::size_t numItems) const;
// [26] int capacityRaw(Error *error, bsl::size_t *capacity) const;
// [27] bool isError() const;
// [27] bool isVoid() const;
// [20] bool isNull() const;
// [20] bool isNullable() const;
// [27] int errorCode() const;
// [27] bsl::string errorMessage() const;
// [ 6] bsl::string asString() const;
// [ 6] void loadAsString(bsl::string *result) const;
// [ 5] bool asBool() const;
// [ 5] char asChar() const;
// [ 5] short asShort() const;
// [ 5] int asInt() const;
// [ 5] bsls_Types::Int64 asInt64() const;
// [ 5] float asFloat() const;
// [ 5] double asDouble() const;
// [ 5] bdet_Datetime asDatetime() const;
// [ 5] bdet_DatetimeTz asDatetimeTz() const;
// [ 5] bdet_Date asDate() const;
// [ 5] bdet_DateTz asDateTz() const;
// [ 5] bdet_Time asTime() const;
// [ 5] bdet_TimeTz asTimeTz() const;
// [ 6] const bdem_ElemRef asElemRef() const;
// [ 4] bool hasField(const char *fieldName) const;
// [ 4] bool hasFieldById(int fieldId) const;
// [ 4] bool hasFieldByIndex(int fieldIndex) const;
// [24] int anonymousField(Obj *object, Error *error, int index) const;
// [24] int anonymousField(Obj *object, Error *error) const;
// [ 4] int getField(Obj *o, Error *e, bool null, f1, f2, . ., f10) const;
// [29] int findUnambiguousChoice(Obj *obj, Error *error, caller) const;
// [ 4] int fieldByIndex(Obj *obj, Error *error, int index) const;
// [ 4] int fieldById(Obj *obj, Error *error, int id) const;
// [13] int arrayItem(Obj *item, Error *error, int index) const;
// [19] int length() const;
// [20] int numSelections() const;
// [15] const char *selector() const;
// [16] int selectorId() const;
// [17] int selectorIndex() const;
// [15] int selection(Obj *obj, Error *error) const;
// [ 4] bdem_ElemType::Type dataType() const;
// [11] const bdem_RecordDef& recordDef() const;
// [11] const bdem_RecordDef *recordConstraint() const;
// [  ] const bdem_EnumerationDef *enumerationConstraint() const;
// [ 4] const bdem_FieldDef *fieldDef() const;
// [11] const bdem_RecordDef *recordDefPtr() const;
// [11] const void *data() const;
// [11] const bdem_Schema *schema() const;
// [27] void swap(bcem_AggregateRaw& other);
// [23] STREAM& bdexStreamIn(STREAM& stream, int version) const;
// [23] STREAM& bdexStreamOut(STREAM& stream, int version) const;
// [ 9] bsl::ostream& print(stream, int level, int spl) const;
// [ 3] int setField(Obj *o, Error *e, f1, value) const;
// [ 4] int setField(Obj *o, Error *e, f1, f2, value) const;
// [ 4] int setField(Obj *o, Error *e, f1, f2, f3, value) const;
// [ 4] int setField(Obj *o, Error *e, f1, f2, . ., f4, value) const;
// [ 4] int setField(Obj *o, Error *e, f1, f2, . ., f5, value) const;
// [ 4] int setField(Obj *o, Error *e, f1, f2, . ., f6, value) const;
// [ 4] int setField(Obj *o, Error *e, f1, f2, . ., f7, value) const;
// [ 4] int setField(Obj *o, Error *e, f1, f2, . ., f8, value) const;
// [ 4] int setField(Obj *o, Error *e, f1, f2, . ., f9, value) const;
// [ 4] int setField(Obj *o, Error *e, f1, f2, . ., f10, value) const;
// [13] int insertItem(Obj *item, Error *error, int index, value) const;
// [14] int insertItems(Error *error, int index, int numItems) const;
// [14] int insertNullItems(Error *error, int index, int numItems) const;
// [14] int removeItems(Error *error, int index, int numItems) const;
// [17] int makeSelectionByIndex(Obj *obj, Error *error, int index) const;
// [17] int makeSelectionByIndex(Obj *obj, Error *error, int idx, value) const;
// [15] int makeSelection(Obj *obj, Error *error, newSelector) const;
// [15] int makeSelection(Obj *obj, Error *error, newSelector, value) const;
// [16] int makeSelectionById(Obj *obj, Error *error, id) const;
// [16] int makeSelectionById(Obj *obj, Error *error, id, value) const;
// [20] void makeNull() const;
// [21] void makeValue() const;
// [ 2] int setValue(Error *error, const TYPE& value) const;
// [12] int resize(Error *error, bsl::size_t newSize) const;
//
// FREE OPERATORS
// [ 9] bsl::ostream& operator<<(bsl::ostream& stream, const Obj& obj);
//
// BDEAT FRAMEWORK
// [22] namespace bdeat_TypeCategoryFunctions
// [22] namespace bdeat_SequenceFunctions
// [22] namespace bdeat_ChoiceFunctions
// [22] namespace bdeat_ArrayFunctions
// [22] namespace bdeat_EnumFunctions
// [22] namespace bdeat_NullableValueFunctions
// [22] namespace bdeat_ValueTypeFunctions
//
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] ggSchema, ggAggData, destroyAggData
// [30] USAGE EXAMPLE
//-----------------------------------------------------------------------------

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
#define T_ cout << "\t" << flush;             // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bcem_AggregateRaw   Obj;
typedef bcem_AggregateError Error;
typedef bdem_ElemType       ET;
typedef bdem_ElemType       EType;
typedef bdem_ElemAttrLookup  EAL;
typedef bdeat_FormattingMode Format;

typedef bdem_FieldDef           FldDef;
typedef bdem_FieldSpec          FldSpec;
typedef bdem_FieldDefAttributes FldAttr;
typedef bdem_RecordDef          RecDef;
typedef RecDef::RecordType      RecType;
typedef bdem_EnumerationDef     EnumDef;
typedef bdem_Schema             Schema;

typedef bdem_SchemaAggregateUtil SchemaAggUtil;

typedef bdem_ConstElemRef     CERef;
typedef bdem_ElemRef          ERef;
typedef RecDef::RecordType    RecType;

typedef bdem_List             List;
typedef bdem_Row              Row;
typedef bdem_Table            Table;
typedef bdem_Choice           Choice;
typedef bdem_ChoiceArrayItem  ChoiceItem;
typedef bdem_ChoiceArray      ChoiceArray;

typedef bdet_Time             Time;
typedef bdet_Date             Date;
typedef bdet_Datetime         Datetime;
typedef bdet_DatetimeTz       DatetimeTz;
typedef bdet_DateTz           DateTz;
typedef bdet_TimeTz           TimeTz;

typedef bsls_PlatformUtil::Int64         Int64;
typedef bslma_TestAllocator   TestAllocator;

typedef bdeat_TypeName     TN;
typedef bdeat_TypeCategory TC;

namespace TCF = bdeat_TypeCategoryFunctions;
namespace SF  = bdeat_SequenceFunctions;
namespace CF  = bdeat_ChoiceFunctions;
namespace AF  = bdeat_ArrayFunctions;
namespace EF  = bdeat_EnumFunctions;
namespace NVF = bdeat_NullableValueFunctions;

const int BCEM_ERR_TBD = -1;

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
    else if (Error::BCEM_ERR_UNKNOWN_ERROR <= errorCode &&
             errorCode <= Error::BCEM_ERR_AMBIGUOUS_ANON) {
        return ERROR_NAMES[errorCode - Error::BCEM_ERR_UNKNOWN_ERROR];
    }
    else {
        return "<unexpected error code>";
    }
}

void aSsErTAggError(const bcem_AggregateRaw& agg,
                    const char *aggNm,
                    int errCode, const char* errCodeNm,
                    int srcLine, int dataLine)
{
    if (BCEM_ERR_TBD == errCode ?
        !agg.isError() : errCode != agg.errorCode()) {
        bsl::string s(errCodeNm);
        // Assertion failed.
        cout << errCodeNm << ": " << errorNm(errCode) << '\t'
             << aggNm << ".errorCode(): " << errorNm(agg.errorCode())
             << '\n';
        s += " == ";
        s += aggNm;
        s += ".errorCode()";
        aSsErT(1, s.c_str(), srcLine);
    }
    else {
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
        int  id        = RecDef::BDEM_NULL_FIELD_ID;

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

void *makeValuePtr(bdem_ElemType::Type type, TestAllocator *ta)
{
    void *data;
    switch (type) {
      case bdem_ElemType::BDEM_CHAR: {
        data = (void *) new (*ta) char;
      } break;
      case bdem_ElemType::BDEM_SHORT: {
        data = (void *) new (*ta) short;
      } break;
      case bdem_ElemType::BDEM_INT: {
        data = (void *) new (*ta) int;
      } break;
      case bdem_ElemType::BDEM_INT64: {
        data = (void *) new (*ta) Int64;
      } break;
      case bdem_ElemType::BDEM_FLOAT: {
        data = (void *) new (*ta) float;
      } break;
      case bdem_ElemType::BDEM_DOUBLE: {
        data = (void *) new (*ta) double;
      } break;
      case bdem_ElemType::BDEM_STRING: {
        data = (void *) new (*ta) string;
      } break;
      case bdem_ElemType::BDEM_DATETIME: {
        data = (void *) new (*ta) Datetime;
      } break;
      case bdem_ElemType::BDEM_DATE: {
        data = (void *) new (*ta) Date;
      } break;
      case bdem_ElemType::BDEM_TIME: {
        data = (void *) new (*ta) Time;
      } break;
      case bdem_ElemType::BDEM_BOOL: {
        data = (void *) new (*ta) bool;
      } break;
      case bdem_ElemType::BDEM_DATETIMETZ: {
        data = (void *) new (*ta) DatetimeTz;
      } break;
      case bdem_ElemType::BDEM_DATETZ: {
        data = (void *) new (*ta) DateTz;
      } break;
      case bdem_ElemType::BDEM_TIMETZ: {
        data = (void *) new (*ta) TimeTz;
      } break;
      case bdem_ElemType::BDEM_CHAR_ARRAY: {
        data = (void *) new (*ta) vector<char>;
      } break;
      case bdem_ElemType::BDEM_SHORT_ARRAY: {
        data = (void *) new (*ta) vector<short>;
      } break;
      case bdem_ElemType::BDEM_INT_ARRAY: {
        data = (void *) new (*ta) vector<int>;
      } break;
      case bdem_ElemType::BDEM_INT64_ARRAY: {
        data = (void *) new (*ta) vector<Int64>;
      } break;
      case bdem_ElemType::BDEM_FLOAT_ARRAY: {
        data = (void *) new (*ta) vector<float>;
      } break;
      case bdem_ElemType::BDEM_DOUBLE_ARRAY: {
        data = (void *) new (*ta) vector<double>;
      } break;
      case bdem_ElemType::BDEM_STRING_ARRAY: {
        data = (void *) new (*ta) vector<string>;
      } break;
      case bdem_ElemType::BDEM_DATETIME_ARRAY: {
        data = (void *) new (*ta) vector<Datetime>;
      } break;
      case bdem_ElemType::BDEM_DATE_ARRAY: {
        data = (void *) new (*ta) vector<Date>;
      } break;
      case bdem_ElemType::BDEM_TIME_ARRAY: {
        data = (void *) new (*ta) vector<Time>;
      } break;
      case bdem_ElemType::BDEM_BOOL_ARRAY: {
        data = (void *) new (*ta) vector<bool>;
      } break;
      case bdem_ElemType::BDEM_DATETIMETZ_ARRAY: {
        data = (void *) new (*ta) vector<DatetimeTz>;
      } break;
      case bdem_ElemType::BDEM_DATETZ_ARRAY: {
        data = (void *) new (*ta) vector<DateTz>;
      } break;
      case bdem_ElemType::BDEM_TIMETZ_ARRAY: {
        data = (void *) new (*ta) vector<TimeTz>;
      } break;
      case bdem_ElemType::BDEM_LIST: {
        data = (void *) new (*ta) List;
      } break;
      case bdem_ElemType::BDEM_TABLE: {
        data = (void *) new (*ta) Table;
      } break;
      case bdem_ElemType::BDEM_CHOICE: {
        data = (void *) new (*ta) Choice;
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY: {
        data = (void *) new (*ta) ChoiceArray;
      } break;
      default: {
        ASSERT(0);
      } break;
    }
    return data;
}

void destroyValuePtr(void *value, bdem_ElemType::Type type, TestAllocator *ta)
{
    switch (type) {
      case bdem_ElemType::BDEM_CHAR: {
        ta->deleteObject((char *) value);
      } break;
      case bdem_ElemType::BDEM_SHORT: {
        ta->deleteObject((short *) value);
      } break;
      case bdem_ElemType::BDEM_INT: {
        ta->deleteObject((int *) value);
      } break;
      case bdem_ElemType::BDEM_INT64: {
        ta->deleteObject((Int64 *) value);
      } break;
      case bdem_ElemType::BDEM_FLOAT: {
        ta->deleteObject((float *) value);
      } break;
      case bdem_ElemType::BDEM_DOUBLE: {
        ta->deleteObject((double *) value);
      } break;
      case bdem_ElemType::BDEM_STRING: {
        ta->deleteObject((string *) value);
      } break;
      case bdem_ElemType::BDEM_DATETIME: {
        ta->deleteObject((Datetime *) value);
      } break;
      case bdem_ElemType::BDEM_DATE: {
        ta->deleteObject((Date *) value);
      } break;
      case bdem_ElemType::BDEM_TIME: {
        ta->deleteObject((Time *) value);
      } break;
      case bdem_ElemType::BDEM_BOOL: {
        ta->deleteObject((bool *) value);
      } break;
      case bdem_ElemType::BDEM_DATETIMETZ: {
        ta->deleteObject((DatetimeTz *) value);
      } break;
      case bdem_ElemType::BDEM_DATETZ: {
        ta->deleteObject((DateTz *) value);
      } break;
      case bdem_ElemType::BDEM_TIMETZ: {
        ta->deleteObject((TimeTz *) value);
      } break;
      case bdem_ElemType::BDEM_CHAR_ARRAY: {
        ta->deleteObject((vector<char> *) value);
      } break;
      case bdem_ElemType::BDEM_SHORT_ARRAY: {
        ta->deleteObject((vector<short> *) value);
      } break;
      case bdem_ElemType::BDEM_INT_ARRAY: {
        ta->deleteObject((vector<int> *) value);
      } break;
      case bdem_ElemType::BDEM_INT64_ARRAY: {
        ta->deleteObject((vector<Int64> *) value);
      } break;
      case bdem_ElemType::BDEM_FLOAT_ARRAY: {
        ta->deleteObject((vector<float> *) value);
      } break;
      case bdem_ElemType::BDEM_DOUBLE_ARRAY: {
        ta->deleteObject((vector<double> *) value);
      } break;
      case bdem_ElemType::BDEM_STRING_ARRAY: {
        ta->deleteObject((vector<string> *) value);
      } break;
      case bdem_ElemType::BDEM_DATETIME_ARRAY: {
        ta->deleteObject((vector<Datetime> *) value);
      } break;
      case bdem_ElemType::BDEM_DATE_ARRAY: {
        ta->deleteObject((vector<Date> *) value);
      } break;
      case bdem_ElemType::BDEM_TIME_ARRAY: {
        ta->deleteObject((vector<Time> *) value);
      } break;
      case bdem_ElemType::BDEM_BOOL_ARRAY: {
        ta->deleteObject((vector<bool> *) value);
      } break;
      case bdem_ElemType::BDEM_DATETIMETZ_ARRAY: {
        ta->deleteObject((vector<DatetimeTz> *) value);
      } break;
      case bdem_ElemType::BDEM_DATETZ_ARRAY: {
        ta->deleteObject((vector<DateTz> *) value);
      } break;
      case bdem_ElemType::BDEM_TIMETZ_ARRAY: {
        ta->deleteObject((vector<TimeTz> *) value);
      } break;
      case bdem_ElemType::BDEM_LIST: {
        ta->deleteObject((List *) value);
      } break;
      case bdem_ElemType::BDEM_TABLE: {
        ta->deleteObject((Table *) value);
      } break;
      case bdem_ElemType::BDEM_CHOICE: {
        ta->deleteObject((Choice *) value);
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY: {
        ta->deleteObject((ChoiceArray *) value);
      } break;
      default: {
        ASSERT(0);
      } break;
    }
}

int ggAggData(Obj *agg, const RecDef& record, bslma_Allocator *basicAllocator)
{
    bslma_Allocator *allocator = bslma_Default::allocator(basicAllocator);

    EType::Type elemType = bdem_RecordDef::BDEM_CHOICE_RECORD ==
                                                            record.recordType()
                         ? bdem_ElemType::BDEM_CHOICE
                         : bdem_ElemType::BDEM_LIST;
    void *data;
    switch (elemType) {
      case bdem_ElemType::BDEM_LIST: {
        if (record.recordType() != bdem_RecordDef::BDEM_SEQUENCE_RECORD) {
            ASSERT(0);
            return -1;                                                // RETURN
        }
        bdem_List *list = new (*allocator) bdem_List(allocator);
        bdem_SchemaAggregateUtil::initListDeep(list, record);
        data = (void *) list;
      } break;
      case bdem_ElemType::BDEM_TABLE: {
        if (record.recordType() != bdem_RecordDef::BDEM_SEQUENCE_RECORD) {
            ASSERT(0);
            return -1;                                                // RETURN
        }
        bdem_Table *table = new (*allocator) bdem_Table(allocator);
        bdem_SchemaAggregateUtil::initTable(table, record);
        data = (void *) table;
      } break;
      case bdem_ElemType::BDEM_CHOICE: {
        if (record.recordType() != bdem_RecordDef::BDEM_CHOICE_RECORD) {
            ASSERT(0);
            return -1;                                                // RETURN
        }
        bdem_Choice *choice = new (*allocator) bdem_Choice(allocator);
        bdem_SchemaAggregateUtil::initChoice(choice, record);
        data = (void *) choice;
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY: {
        if (record.recordType() != bdem_RecordDef::BDEM_CHOICE_RECORD) {
            ASSERT(0);
            return -1;                                                // RETURN
        }
        bdem_ChoiceArray *ca = new (*allocator) bdem_ChoiceArray(allocator);
        bdem_SchemaAggregateUtil::initChoiceArray(ca, record);
        data = (void *) ca;
      } break;
      default: {
        return -1;                                                    // RETURN
      }
    }

    agg->setDataType(elemType);
    agg->setSchemaPointer(&record.schema());
    agg->setRecordDefPointer(&record);
    agg->setDataPointer(data);
    return 0;
}

int destroyAggData(Obj *agg, bslma_Allocator *allocator)
{
    EType::Type  elemType = agg->dataType();
    void        *aggData  = (void *) agg->data();
    switch (elemType) {
      case bdem_ElemType::BDEM_LIST: {
        bdem_List *list = (bdem_List *) aggData;
        allocator->deleteObject(list);
      } break;
      case bdem_ElemType::BDEM_TABLE: {
        bdem_Table *table = (bdem_Table *) aggData;
        allocator->deleteObject(table);
      } break;
      case bdem_ElemType::BDEM_CHOICE: {
        bdem_Choice *choice = (bdem_Choice *) aggData;
        allocator->deleteObject(choice);
      } break;
      case bdem_ElemType::BDEM_CHOICE_ARRAY: {
        bdem_ChoiceArray *choiceArray = (bdem_ChoiceArray *) aggData;
        allocator->deleteObject(choiceArray);
      } break;
      default: {
        return -1;                                                    // RETURN
      }
    }
    return 0;
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
        Obj   subAgg;
        Error error;
        if (d_fldName) {
            int rc = d_obj->setField(&subAgg, &error, d_fldName, value);
            ASSERT(!rc);
        }
        else {
            int rc = d_obj->setField(&subAgg, &error, d_index, value);
            ASSERT(!rc);
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
    void operator()(const VALTYPE& value)
    {
        Error error;
        int rc = d_obj->setValue(&error, value);
        ASSERT(!rc);
    }
};

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

static bool compareNillableElement(bcem_AggregateRaw agg, const CERef& elemRef)
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

static void assignToNillableTable(bcem_AggregateRaw *agg,
                                  const CERef&       arrayRef)
{
    bdem_ElemType::Type arrayType = arrayRef.type();
    Error err;
    int   rc;
    switch (arrayType) {
      case bdem_ElemType::BDEM_CHAR_ARRAY:
        rc = agg->setValue(&err, arrayRef.theCharArray());
        ASSERT(!rc);
        return;
      case bdem_ElemType::BDEM_SHORT_ARRAY:
        rc = agg->setValue(&err, arrayRef.theShortArray());
        ASSERT(!rc);
        return;
      case bdem_ElemType::BDEM_INT_ARRAY:
        rc = agg->setValue(&err, arrayRef.theIntArray());
        ASSERT(!rc);
        return;
      case bdem_ElemType::BDEM_INT64_ARRAY:
        rc = agg->setValue(&err, arrayRef.theInt64Array());
        ASSERT(!rc);
        return;
      case bdem_ElemType::BDEM_FLOAT_ARRAY:
        rc = agg->setValue(&err, arrayRef.theFloatArray());
        return;
      case bdem_ElemType::BDEM_DOUBLE_ARRAY:
        rc = agg->setValue(&err, arrayRef.theDoubleArray());
        ASSERT(!rc);
        return;
      case bdem_ElemType::BDEM_STRING_ARRAY:
        rc = agg->setValue(&err, arrayRef.theStringArray());
        ASSERT(!rc);
        return;
      case bdem_ElemType::BDEM_DATETIME_ARRAY:
        rc = agg->setValue(&err, arrayRef.theDatetimeArray());
        ASSERT(!rc);
        return;
      case bdem_ElemType::BDEM_DATE_ARRAY:
        rc = agg->setValue(&err, arrayRef.theDateArray());
        ASSERT(!rc);
        return;
      case bdem_ElemType::BDEM_TIME_ARRAY:
        rc = agg->setValue(&err, arrayRef.theTimeArray());
        ASSERT(!rc);
        return;
      case bdem_ElemType::BDEM_BOOL_ARRAY:
        rc = agg->setValue(&err, arrayRef.theBoolArray());
        ASSERT(!rc);
        return;
      case bdem_ElemType::BDEM_DATETIMETZ_ARRAY:
        rc = agg->setValue(&err, arrayRef.theDatetimeTzArray());
        ASSERT(!rc);
        return;
      case bdem_ElemType::BDEM_DATETZ_ARRAY:
        rc = agg->setValue(&err, arrayRef.theDateTzArray());
        ASSERT(!rc);
        return;
      case bdem_ElemType::BDEM_TIMETZ_ARRAY:
        rc = agg->setValue(&err, arrayRef.theTimeTzArray());
        ASSERT(!rc);
        return;
      default:
        ASSERT(0);
    }
}

static bool compareNillableElement(bcem_AggregateRaw agg,
                                   const CERef&      elemRef,
                                   int               index)
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

static bool compareNillableTable(bcem_AggregateRaw agg, const CERef& elemRef)
{
    ASSERT(bdem_ElemType::BDEM_TABLE == agg.dataType());
    const int LEN = getLength(elemRef);
    if (LEN != agg.length()) {
        return false;
    }
    for (int i = 0; i < LEN; ++i) {
        Obj   tmp;
        Error err;
        int rc = agg.arrayItem(&tmp, &err, i);
        ASSERT(!rc);
        if (!compareNillableElement(tmp, elemRef, i)) {
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

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

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
    NOT_CHOICE = Error::BCEM_ERR_NOT_A_CHOICE,
    BAD_FLDNM  = Error::BCEM_ERR_BAD_FIELDNAME,
    BAD_FLDIDX = Error::BCEM_ERR_BAD_FIELDINDEX,
    NOT_SELECT = Error::BCEM_ERR_NOT_SELECTED,
    NON_RECORD = Error::BCEM_ERR_NOT_A_RECORD,
    AMBIGUOUS  = Error::BCEM_ERR_AMBIGUOUS_ANON
};


struct Accumulator {
    int d_count;
    
    int operator()(const bcem_AggregateRaw& value) {
        d_count += value.asInt();
        return 0;
    }

    template<typename TYPE>
    int operator()(const TYPE& value) {
        return -1;
    }
   
    Accumulator() : d_count(0) {}
};


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? std::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    initStaticData();

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 30: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Test that the usage example compiles
        // --------------------------------------------------------------------

          if (verbose) bsl::cout << bsl::endl << "Usage Example"
                                 << bsl::endl << "============="
                                 << bsl::endl;

          Obj object;

          for (int i = 0; i < object.length(); ++i) {
              bcem_AggregateRaw field;
              bcem_AggregateError error;
              if (0 == object.fieldByIndex(&field, &error, i)) {
                  field.print(bsl::cout, 0, -1);
              }
          }
      } break;
      case 29: {
        // --------------------------------------------------------------------
        // TESTING 'findUnambiguousChoice'
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   int findUnambiguousChoice(Obj *obj, Error *error, caller) const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'findUnambiguousChoice'"
                          << "\n===============================" << bsl::endl;
  
        Schema schema;
        ggSchema(&schema, ":*?Gc :a%*0&FU");

        bslma_TestAllocator t;
        const RecDef* RECORD = schema.lookupRecord("a");

        Obj mX;  const Obj& X = mX;
        int rc = ggAggData(&mX, *RECORD, &t);
        ASSERT(!rc);

        if (veryVerbose) {
            P(X);
        }

        Error err;
        Obj mA;  const Obj& A = mA;
        rc = mX.anonymousField(&mA, &err);
        ASSERT(!rc);

        rc = mX.anonymousField(&mA, &err, 0);
        ASSERT(!rc);

        LOOP_ASSERT(A.dataType(), bdem_ElemType::BDEM_CHOICE == A.dataType());

        if (veryVerbose) {
            P(A);
        }

        Obj choice;
        rc = mX.findUnambiguousChoice(&choice, &err);
        ASSERT(!rc);
        ASSERT(!choice.isNull());
        ASSERT(TC::BDEAT_CHOICE_CATEGORY == TCF::select(choice));

        if (veryVerbose) {
            P(choice);
        }

        LOOP_ASSERT(X.selectorIndex(), -1 == X.selectorIndex());

        destroyAggData(&mX, &t);
      } break;
      case 28: {
        // --------------------------------------------------------------------
        // TESTING 'swap'
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   void swap(bcem_AggregateRaw& other);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'swap'"
                          << "\n=============="
                          << bsl::endl;

        if (veryVerbose) { T_ cout << "Testing for empty aggregates" << endl; }
        {
            Obj mX;  const Obj& X = mX;
            Obj mY;  const Obj& Y = mY;
            Obj mZ;  const Obj& Z = mZ;
            ASSERT(Obj::areEquivalent(X, Y));

            mX.swap(mY);
            ASSERT(Obj::areEquivalent(X, Y));

            ET::Type    TYPE = ET::BDEM_STRING;
            const CERef CER  = getCERef(TYPE, 1);

            bslma_TestAllocator ta;
            void *data = makeValuePtr(TYPE, &ta);
            CER.descriptor()->assign(data, CER.data());

            mX.setDataType(TYPE);
            mX.setDataPointer(data);
            ASSERT(!Obj::areEquivalent(X, Y));
            ASSERT(!Obj::areEquivalent(X, Z));
            ASSERT( Obj::areEquivalent(Y, Z));

            mX.swap(mY);
            ASSERT(!Obj::areEquivalent(X, Y));
            ASSERT( Obj::areEquivalent(X, Z));
            ASSERT(!Obj::areEquivalent(Y, Z));

            destroyValuePtr(data, TYPE, &ta);
        }

        if (veryVerbose) { T_ cout << "Testing for scalar aggregates"
                                   << endl; }
        {
            TestAllocator ta;
            for (int i = 0; i < ET::BDEM_NUM_TYPES; ++i) {

                ET::Type    TYPE1 = (ET::Type) i;
                const CERef CER1  = getCERef(TYPE1, 1);

                void *data1 = makeValuePtr(TYPE1, &ta);
                CER1.descriptor()->assign(data1, CER1.data());

                int nf1 = 0;
                Obj mX; const Obj& X = mX;
                mX.setDataType(TYPE1);
                mX.setDataPointer(data1);
                mX.setTopLevelAggregateNullnessPointer(&nf1);

                for (int j = 0; j < ET::BDEM_NUM_TYPES; ++j) {
                    ET::Type    TYPE2 = (ET::Type) j;
                    const CERef CER2  = getCERef(TYPE2, 1);
                    bool        IS_EQUAL = (i == j) ? true : false;

                    void *data2 = makeValuePtr(TYPE2, &ta);
                    CER2.descriptor()->assign(data2, CER2.data());

                    int nf2 = 0;
                    Obj mY; const Obj& Y = mY;
                    mY.setDataType(TYPE2);
                    mY.setDataPointer(data2);
                    mY.setTopLevelAggregateNullnessPointer(&nf2);

                    if (veryVerbose) { P(TYPE1) P(TYPE2) P(X) P(Y) }

                    Obj mA(X);  const Obj& A = mA;
                    Obj mB(Y);  const Obj& B = mB;

                    ASSERT(IS_EQUAL == Obj::areEquivalent(X, Y));
                    ASSERT(            Obj::areEquivalent(X, A));
                    ASSERT(IS_EQUAL == Obj::areEquivalent(X, B));
                    ASSERT(IS_EQUAL == Obj::areEquivalent(Y, A));
                    ASSERT(            Obj::areEquivalent(Y, B));

                    mX.swap(mY);

                    ASSERT(IS_EQUAL == Obj::areEquivalent(X, Y));
                    ASSERT(IS_EQUAL == Obj::areEquivalent(X, A));
                    ASSERT(            Obj::areEquivalent(X, B));
                    ASSERT(            Obj::areEquivalent(Y, A));
                    ASSERT(IS_EQUAL == Obj::areEquivalent(Y, B));

                    mY.swap(mX);

                    ASSERT(IS_EQUAL == Obj::areEquivalent(X, Y));
                    ASSERT(            Obj::areEquivalent(X, A));
                    ASSERT(IS_EQUAL == Obj::areEquivalent(X, B));
                    ASSERT(IS_EQUAL == Obj::areEquivalent(Y, A));
                    ASSERT(            Obj::areEquivalent(Y, B));

                    destroyValuePtr(data2, TYPE2, &ta);
                }

                destroyValuePtr(data1, TYPE1, &ta);
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
                {   L_,         ":aCa&D0" },
                {   L_,         ":aFa" },
                {   L_,         ":aFa&NT" },
                {   L_,         ":aFa&D0" },
                {   L_,         ":aGa" },
                {   L_,         ":aGa&NT" },
                {   L_,         ":aGa&D0" },
                {   L_,         ":aHa"    },
                {   L_,         ":aHa&NT" },
                {   L_,         ":aHa&D0" },
                {   L_,         ":aNa"    },
                {   L_,         ":aNa&NT" },
                {   L_,         ":aNa&D0" },
                {   L_,         ":aNa&FN" },
                {   L_,         ":aPa" },
                {   L_,         ":aPa&NT" },
                {   L_,         ":aPa&D0" },
                {   L_,         ":aPa&FN" },
                {   L_,         ":aQa" },
                {   L_,         ":aQa&NT" },
                {   L_,         ":aQa&D0" },
                {   L_,         ":aQa&FN" },
                {   L_,         ":aRa" },
                {   L_,         ":aRa&NT" },
                {   L_,         ":aRa&D0" },
                {   L_,         ":aRa&FN" },
                {   L_,         ":aWa" },
                {   L_,         ":aWa&NT" },
                {   L_,         ":aWa&D0" },
                {   L_,         ":aaa&FN" },
                {   L_,         ":aVa" },
                {   L_,         ":aVa&NT" },
                {   L_,         ":afa" },
                {   L_,         ":afa&NT" },

                {   L_,         ":b=tu5v :a$ab" },
                {   L_,         ":b=tu5v :a$ab&NT" },

                {   L_,         ":b=tu5v :a^ab" },
                {   L_,         ":b=tu5v :a^ab&NT" },

                {   L_,         ":b=tu5v :a!ab" },
                {   L_,         ":b=tu5v :a!ab&NT" },
                {   L_,         ":b=tu5v :a!ab&FN" },

                {   L_,         ":b=tu5v :a/ab" },
                {   L_,         ":b=tu5v :a/ab&NT" },
                {   L_,         ":b=tu5v :a/ab&FN" },

                {   L_,         ":aCbFcGdQf :g+ha" },
                {   L_,         ":aCbFcGdQf :g+ha&NT" },

                {   L_,         ":aCbFcGdQf :g#ha" },
                {   L_,         ":aCbFcGdQf :g#ha&NT" },

                {   L_,         ":a?CbFcGdQf :g%ha" },
                {   L_,         ":a?CbFcGdQf :g%ha&NT" },

                {   L_,         ":a?CbFcGdQf :g@ha" },
                {   L_,         ":a?CbFcGdQf :g@ha&NT" },

                // For Choice Aggregates
                {   L_,         ":a?Ca" },
                {   L_,         ":a?Ca&NT" },
                {   L_,         ":a?Ca&D0" },
                {   L_,         ":a?Fa" },
                {   L_,         ":a?Fa&NT" },
                {   L_,         ":a?Fa&D0" },
                {   L_,         ":a?Ga" },
                {   L_,         ":a?Ga&NT" },
                {   L_,         ":a?Ga&D0" },
                {   L_,         ":a?Ha" },
                {   L_,         ":a?Ha&NT" },
                {   L_,         ":a?Ha&D0" },
                {   L_,         ":a?Na" },
                {   L_,         ":a?Na&NT" },
                {   L_,         ":a?Na&D0" },
                {   L_,         ":a?Pa" },
                {   L_,         ":a?Pa&NT" },
                {   L_,         ":a?Pa&D0" },
                {   L_,         ":a?Qa" },
                {   L_,         ":a?Qa&NT" },
                {   L_,         ":a?Qa&D0" },
                {   L_,         ":a?Ra" },
                {   L_,         ":a?Ra&NT" },
                {   L_,         ":a?Ra&D0" },
                {   L_,         ":a?Wa" },
                {   L_,         ":a?Wa&NT" },
                {   L_,         ":a?Wa&D0" },
                {   L_,         ":a?Va" },
                {   L_,         ":a?Va&NT" },
                {   L_,         ":a?fa" },
                {   L_,         ":a?fa&NT" },

                {   L_,         ":aCbFcGdQf :g?+ha" },
                {   L_,         ":aCbFcGdQf :g?+ha&NT" },

                {   L_,         ":aCbFcGdQf :g?#ha" },
                {   L_,         ":aCbFcGdQf :g?#ha&NT" },

                {   L_,         ":a?CbFcGdQf :g?%ha"    },
                {   L_,         ":a?CbFcGdQf :g?%ha&NT" },

                {   L_,         ":a?CbFcGdQf :g?@ha" },
                {   L_,         ":a?CbFcGdQf :g?@ha&NT" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            bslma_TestAllocator ta(veryVeryVerbose);
            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE1 = DATA[i].d_line;
                const char *SPEC1 = DATA[i].d_spec;
                const bool  NSA1  = (bool) bsl::strstr(SPEC1, "&FN");

                Schema s1; ggSchema(&s1, SPEC1);
                const RecDef *r1 = NSA1
                                 ? &s1.record(0)
                                 : &s1.record(s1.numRecords() - 1);

                const char *fldName1 = r1->fieldName(0);

                Obj mX; const Obj& X = mX;
                int rc = ggAggData(&mX, *r1, &ta);
                ASSERT(!rc);

                Obj   mA; const Obj& A = mA;
                Error err;
                if (RecDef::BDEM_CHOICE_RECORD == r1->recordType()) {
                    rc = mX.makeSelection(&mA, &err, fldName1);
                    ASSERT(!rc);
                }

                for (int j = 0; j < NUM_DATA; ++j) {
                    const int   LINE2 = DATA[j].d_line;
                    const char *SPEC2 = DATA[j].d_spec;
                    const bool  NSA2  = (bool) bsl::strstr(SPEC2, "&FN");
                    bool        IS_EQUAL = (i == j) ? true : false;

                    Schema s2; ggSchema(&s2, SPEC2);
                    const RecDef *r2 = NSA2
                                     ? &s2.record(0)
                                     : &s2.record(s2.numRecords() - 1);

                    const char *fldName2 = r2->fieldName(0);

                    Obj mY; const Obj& Y = mY;
                    rc = ggAggData(&mY, *r2, &ta);
                    ASSERT(!rc);

                    if (RecDef::BDEM_CHOICE_RECORD == r2->recordType()) {
                        rc = mY.makeSelection(&mA, &err, fldName2);
                        ASSERT(!rc);
                    }

                    if (veryVerbose) { P(LINE1) P(LINE2) P(X) P(Y) }

                    Obj mA(X);  const Obj& A = mA;
                    Obj mB(Y);  const Obj& B = mB;

                    LOOP3_ASSERT(X, Y, IS_EQUAL,
                                 IS_EQUAL == Obj::areEquivalent(X, Y));
                    ASSERT(            Obj::areEquivalent(X, A));
                    ASSERT(IS_EQUAL == Obj::areEquivalent(X, B));
                    ASSERT(IS_EQUAL == Obj::areEquivalent(Y, A));
                    ASSERT(            Obj::areEquivalent(Y, B));

                    mX.swap(mY);

                    ASSERT(IS_EQUAL == Obj::areEquivalent(X, Y));
                    ASSERT(IS_EQUAL == Obj::areEquivalent(X, A));
                    ASSERT(            Obj::areEquivalent(X, B));
                    ASSERT(            Obj::areEquivalent(Y, A));
                    ASSERT(IS_EQUAL == Obj::areEquivalent(Y, B));

                    mY.swap(mX);

                    ASSERT(IS_EQUAL == Obj::areEquivalent(X, Y));
                    ASSERT(            Obj::areEquivalent(X, A));
                    ASSERT(IS_EQUAL == Obj::areEquivalent(X, B));
                    ASSERT(IS_EQUAL == Obj::areEquivalent(Y, A));
                    ASSERT(            Obj::areEquivalent(Y, B));

                    destroyAggData(&mY, &ta);
                }

                destroyAggData(&mX, &ta);
            }
        }
      } break;
      case 27: {
        // --------------------------------------------------------------------
        // TESTING error functions:
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   bool isError() const;
        //   bool isVoid() const;
        //   int errorCode() const;
        //   bsl::string errorMessage() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING error functions"
                          << "\n=======================" << bsl::endl;

        {
                  char        AA = 'X';
                  bdet_Date   BB(1, 1, 1);
            const bsl::string S1("Invalid conversion when setting "
                                 "CHAR value from DATE value");

            {
                Obj mX; const Obj& X = mX;
                mX.setDataType(ET::BDEM_CHAR);
                mX.setDataPointer(&AA);

                ASSERT(!X.isError());
                ASSERT(!X.isVoid());

                Error err;
                X.setValue(&err, BB);
                ASSERT(Error::BCEM_ERR_BAD_CONVERSION == err.code());
                LOOP_ASSERT(err.description(), S1 == err.description());
            }
        }
      } break;
      case 26: {
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
        //:   as the one returned by invoking the corresponding 'capacity'
        //:   method on the underlying array type.  [C-1,2]
        //: 2 Instantiate a 'bcem_Aggregate' with an underlying non-array
        //:   'BDEM_ELEMTYPE' type and verify that an error-aggregate is
        //:   returned. [C-3]
        //
        // Testing the following functions:
        //   int capacityRaw(Error *error, bsl::size_t *capacity) const;
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing with a table type aggregate" << endl;

        for (int i = 1; i <= 4096; i <<= 1) {
            bdem_Table table1, table2;
            Obj mX;
            mX.setDataType(bdem_ElemType::BDEM_TABLE);
            mX.setDataPointer(&table1);

            Error err;
            mX.reserveRaw(&err, i);
            table2.reserveRaw(i);

            size_t aggCapacity = 1492;  // arbitrary flag value
            size_t tableCapacity = table1.capacityRaw();
            int rc = mX.capacityRaw(&err, &aggCapacity);
            ASSERT(!rc);

            LOOP2_ASSERT(i, tableCapacity, i == tableCapacity);
            LOOP2_ASSERT(i, aggCapacity, i == aggCapacity);
        }

        if (verbose) cout << "Testing with a choice array type aggregate"
                          << endl;

        for (int i = 1; i <= 4096; i <<= 1) {
            bdem_ChoiceArray ca1, ca2;
            Obj mX;
            mX.setDataType(bdem_ElemType::BDEM_CHOICE_ARRAY);
            mX.setDataPointer(&ca1);

            Error err;
            mX.reserveRaw(&err, i);
            ca2.reserveRaw(i);

            size_t aggCapacity = 1492;  // arbitrary flag value
            size_t choiceArrayCapacity = ca2.capacityRaw();
            int rc = mX.capacityRaw(&err, &aggCapacity);
            ASSERT(!rc);

            LOOP2_ASSERT(i, choiceArrayCapacity, i == choiceArrayCapacity);
            LOOP2_ASSERT(i, aggCapacity, i == aggCapacity);
        }

        if (verbose) cout << "Testing with a scalar array type aggregate"
                          << endl;

        for (int i = 1; i <= 4096; i <<= 1) {
            bsl::vector<int> sa1, sa2;
            Obj mX;
            mX.setDataType(bdem_ElemType::BDEM_INT_ARRAY);
            mX.setDataPointer(&sa1);

            Error err;
            mX.reserveRaw(&err, i);
            sa2.reserve(i);

            size_t aggCapacity = 1492;  // arbitrary flag value
            size_t scalarCapacity = sa2.capacity();

            int rc = mX.capacityRaw(&err, &aggCapacity);
            ASSERT(!rc);

            LOOP2_ASSERT(i, scalarCapacity, i == scalarCapacity);
            LOOP2_ASSERT(i, aggCapacity, i == aggCapacity);
        }

        if (verbose) cout << "Testing with a non-array type aggregate" << endl;
        {
            bdem_List list;
            Obj mX;
            mX.setDataType(bdem_ElemType::BDEM_LIST);
            mX.setDataPointer(&list);

            Error err;
            size_t capacity = 1492;  // arbitrary flag value
            int rc = mX.capacityRaw(&err, &capacity);
            ASSERT(rc);
            ASSERT(1492 == capacity);
        }
      } break;
      case 25: {
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
        //   int reserveRaw(Error *error, bsl::size_t numItems) const;
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing with a table-type aggregate" << endl;
        {
            bslma_TestAllocator ta("TableTestAllocator", false);
            bslma_TestAllocator aa("AggregateTestAllocator", false);
            for (int i = 1; i <= 4096; i <<= 1) {
                bdem_Table table1(&ta), table2(&aa);
                Obj mX, mY;
                mX.setDataType(bdem_ElemType::BDEM_TABLE);
                mX.setDataPointer(&table1);
                mY.setDataType(bdem_ElemType::BDEM_TABLE);
                mY.setDataPointer(&table2);

                LOOP_ASSERT(i, Obj::areEquivalent(mX, mY));

                const size_t NUM_BYTES_TABLE = ta.numBytesTotal();
                const size_t NUM_BYTES_AGGREGATE = aa.numBytesTotal();

                table1.reserveRaw(i);

                Error err;
                int rc = mY.reserveRaw(&err, i);
                LOOP_ASSERT(rc, !rc);

                LOOP_ASSERT(i, Obj::areEquivalent(mX, mY));

                const size_t DELTA_TABLE =
                                          NUM_BYTES_TABLE - ta.numBytesTotal();
                const size_t DELTA_AGGREGATE =
                                      NUM_BYTES_AGGREGATE - aa.numBytesTotal();
                LOOP3_ASSERT(i,
                             DELTA_TABLE,
                             DELTA_AGGREGATE,
                             DELTA_TABLE == DELTA_AGGREGATE);
            }
        }

        if (verbose) cout << "Testing with a choice-array-type aggregate"
                          << endl;
        {
            bslma_TestAllocator ta("ChoiceArrayTestAllocator", false);
            bslma_TestAllocator aa("AggregateTestAllocator", false);
            for (int i = 1; i <= 4096; i <<= 1) {
                bdem_ChoiceArray ca1(&ta), ca2(&aa);
                Obj mX, mY;
                mX.setDataType(bdem_ElemType::BDEM_CHOICE_ARRAY);
                mX.setDataPointer(&ca1);
                mY.setDataType(bdem_ElemType::BDEM_CHOICE_ARRAY);
                mY.setDataPointer(&ca2);

                LOOP_ASSERT(i, Obj::areEquivalent(mX, mY));

                const size_t NUM_BYTES_CA        = ta.numBytesTotal();
                const size_t NUM_BYTES_AGGREGATE = aa.numBytesTotal();

                ca1.reserveRaw(i);

                Error err;
                int rc = mY.reserveRaw(&err, i);
                ASSERT(!rc);

                LOOP_ASSERT(i, Obj::areEquivalent(mX, mY));

                const size_t DELTA_CA = NUM_BYTES_CA - ta.numBytesTotal();
                const size_t DELTA_AGGREGATE =
                                      NUM_BYTES_AGGREGATE - aa.numBytesTotal();
                LOOP3_ASSERT(i,
                             DELTA_CA,
                             DELTA_AGGREGATE,
                             DELTA_CA == DELTA_AGGREGATE);
            }
        }

        if (verbose) cout << "Testing with a scalar-array-type aggregate"
                          << endl;
        {
            bslma_TestAllocator ta("ScalarArrayTestAllocator", false);
            bslma_TestAllocator aa("AggregateTestAllocator", false);
            for (int i = 1; i <= 4096; i <<= 1) {
                bsl::vector<int> sa1(&ta), sa2(&aa);
                Obj mX, mY;
                mX.setDataType(bdem_ElemType::BDEM_INT_ARRAY);
                mX.setDataPointer(&sa1);
                mY.setDataType(bdem_ElemType::BDEM_INT_ARRAY);
                mY.setDataPointer(&sa2);

                LOOP_ASSERT(i, Obj::areEquivalent(mX, mY));

                const size_t NUM_BYTES_SA        = ta.numBytesTotal();
                const size_t NUM_BYTES_AGGREGATE = aa.numBytesTotal();

                sa1.reserve(i);

                Error err;
                int rc = mY.reserveRaw(&err, i);
                ASSERT(!rc);

                LOOP_ASSERT(i, Obj::areEquivalent(mX, mY));

                const size_t DELTA_SA = NUM_BYTES_SA - ta.numBytesTotal();
                const size_t DELTA_AGGREGATE =
                                      NUM_BYTES_AGGREGATE - aa.numBytesTotal();
                LOOP3_ASSERT(i,
                             DELTA_SA,
                             DELTA_AGGREGATE,
                             DELTA_SA == DELTA_AGGREGATE);
            }
        }

        if (verbose) cout << "Testing with a non-array type aggregate" << endl;
        {
            bslma_TestAllocator la("ListArrayTestAllocator", false);
            bslma_TestAllocator aa("AggregateTestAllocator", false);

            bdem_List list1(&la), list2(&aa);
            Obj mX, mY;
            mX.setDataType(bdem_ElemType::BDEM_LIST);
            mX.setDataPointer(&list1);
            mY.setDataType(bdem_ElemType::BDEM_LIST);
            mY.setDataPointer(&list2);

            const size_t NUM_BYTES_LIST      = la.numBytesTotal();
            const size_t NUM_BYTES_AGGREGATE = aa.numBytesTotal();

            Error err;
            int rc = mY.reserveRaw(&err, 1);
            ASSERT(rc);

            ASSERT(NUM_BYTES_LIST      == la.numBytesTotal());
            ASSERT(NUM_BYTES_AGGREGATE == aa.numBytesTotal());
        }
      } break;
      case 24: {
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
        //   int anonymousField(Obj *object, Error *error, int index) const;
        //   int anonymousField(Obj *object, Error *error) const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING anonymousField"
                          << "\n======================" << bsl::endl;

        if (verbose) cout << "Testing valid calls" << bsl::endl;
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

                bdem_Schema schema;
                ggSchema(&schema, SPEC);

                bslma_TestAllocator t;
                Error err;
                Obj agg, field;
                ggAggData(&agg, *schema.lookupRecord("r"), &t);

                if (EType::BDEM_CHOICE == agg.dataType()) {
                    agg.makeSelectionByIndex(&field, &err, FIELD_INDEX);
                }
                LOOP_ASSERT(agg, ! agg.isError());

                if (veryVeryVerbose) { P(agg); }

                Obj expected;
                agg.fieldByIndex(&expected, &err, FIELD_INDEX);
                Obj result;
                int rc = agg.anonymousField(&result, &err, ANON_INDEX);
                ASSERT(!rc);
                LOOP2_ASSERT(expected, result,
                             Obj::areIdentical(expected, result));

                if (veryVeryVerbose) { P(expected); P(result); }

                if (schema.lookupRecord("r")->numAnonymousFields() == 1) {
                    Obj result2;
                    rc = agg.anonymousField(&result2, &err);
                    ASSERT(!rc);
                    LOOP2_ASSERT(expected, result2,
                                 Obj::areIdentical(expected, result2));
                }

                destroyAggData(&agg, &t);
            }
        }

        if (verbose) cout << "Testing error conditions" << bsl::endl;

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

                bdem_Schema schema;
                ggSchema(&schema, SPEC);

                bslma_TestAllocator t;
                Error err;
                Obj agg, field;
                ggAggData(&agg, *schema.lookupRecord("r"), &t);

                if (EType::BDEM_CHOICE == agg.dataType()) {
                    agg.makeSelectionByIndex(&field, &err, SELECTOR_INDEX);
                }
                LOOP_ASSERT(agg, ! agg.isError());

                if (veryVeryVerbose) { P(agg); }

                Obj result;
                int rc = agg.anonymousField(&result, &err, ANON_INDEX);
                ASSERT(rc);
                ASSERT(ERROR_CODE == err.code());
                LOOP_ASSERT(err.description(),
                            bsl::string::npos !=
                            err.description().find(ERROR_MESSAGE));

                if (veryVeryVerbose) { P(result); }

                int numAnon = schema.lookupRecord("r")->numAnonymousFields();
                Obj result2;
                rc = agg.anonymousField(&result2, &err);
                if (0 == numAnon) {
                    ASSERT(BAD_FLDIDX == err.code());
                    LOOP_ASSERT(err.description(),
                                bsl::string::npos !=
                                err.description().find("no anonymous "
                                                       "fields"));
                } else if (numAnon > 1) {
                    ASSERT(AMBIGUOUS == err.code());
                    LOOP_ASSERT(err.description(),
                                bsl::string::npos !=
                                err.description().find("ambiguous "
                                                       "anonymous"));
                } else if (0 == ANON_INDEX) {
                    ASSERT(ERROR_CODE == err.code());
                }

                destroyAggData(&agg, &t);
            }
        }

        {
            // Check error condition for non-aggregate.
            int value = 5;
            Obj mX;
            mX.setDataType(ET::BDEM_INT);
            mX.setDataPointer(&value);
            Error err;
            Obj result;
            int rc = mX.anonymousField(&result, &err);
            LOOP_ASSERT(rc, rc);
            ASSERT(NON_RECORD == err.code());
            LOOP_ASSERT(err.description(),
                        bsl::string::npos !=
                        err.description().find("unconstrained INT"));

            if (veryVeryVerbose) { P(mX); }
        }
      } break;
      case 23: {
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

        if (verbose) cout << "\nBDEX STREAMING"
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

                ET::Type TYPE = NSA
                        ? ET::toArrayType(SCHEMA.record(1).field(0).elemType())
                        : RECORD->field(0).elemType();
                const char *fldName = RECORD->fieldName(0);

                const CERef A1 = getCERef(TYPE, 1);
                const CERef A2 = getCERef(TYPE, 2);

                int version = bsl::strstr(SPEC, "NT") ? 3 : 1;
                for (; version <= MAX_VERSION; ++version) {
                    bslma_TestAllocator t(veryVeryVerbose);
                    Obj mU; const Obj& U = mU;
                    Obj mV; const Obj& V = mV;
                    Obj mX; const Obj& X = mX;
                    Obj mY; const Obj& Y = mY;

                    ggAggData(&mU, *RECORD, &t);
                    ggAggData(&mV, *RECORD, &t);
                    ggAggData(&mX, *RECORD, &t);
                    ggAggData(&mY, *RECORD, &t);

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
                        LOOP2_ASSERT(LINE, version, Obj::areEquivalent(U, V));
                    }

                    bdex_TestInStream testInStream(out2.data(), out2.length());
                    testInStream.setSuppressVersionCheck(1);
                    LOOP_ASSERT(LINE, testInStream);

                   BEGIN_BDEX_EXCEPTION_TEST {
                     testInStream.reset();

                     bdex_InStreamFunctions::streamIn(testInStream,
                                                      Y,
                                                      version);

                     if (version > 1) {
                         LOOP2_ASSERT(LINE, version, Obj::areEquivalent(X, Y));
                     }
                   } END_BDEX_EXCEPTION_TEST

                   destroyAggData(&mU, &t);
                   destroyAggData(&mV, &t);
                   destroyAggData(&mX, &t);
                   destroyAggData(&mY, &t);
                }
            }
        }
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING 'bdeat' FUNCTIONS
        //
        // Concerns:
        //   - 'bdeat_TypeName::className(bcem_AggregateRaw)' returns the
        //     record name for aggregates that are constrained by a record
        //     definition, and a zero pointer for aggregates that are not
        //     constrained by a record definition.
        //   - 'bdeat_TypeCategory::Select<bcem_AggregateRaw>::BDEAT_SELECTION'
        //     is '0'
        //   - 'bdeat_TypeCategoryFunctions::select(bdem_AggregateRaw)' returns
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
        // - Create an 'bcem_AggregateRaw' object that conforms to the root
        //   record of the schema.
        // - Call 'bdeat_TypeName::className(bcem_AggregateRaw)' on each field
        //   and check for expected result.
        // - Check that
        //   'bdeat_TypeCategory::Select<bcem_AggregateRaw>::BDEAT_SELECTION'
        //   is '0'.
        // - Call 'bdeat_TypeCategoryFunctions::select(bdem_AggregateRaw)' on
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

        if (verbose) cout << "\nTESTING 'bdeat' FUNCTIONS"
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

        Schema schema;

        ggSchema(&schema, SPEC);
        ASSERT(5 == schema.numRecords());
        ASSERT(1 == schema.numEnumerations());
        if (veryVerbose) P(schema);

        ASSERT(0 == TC::Select<bcem_AggregateRaw>::BDEAT_SELECTION);

        bslma_TestAllocator t(veryVeryVerbose);
        bcem_AggregateRaw mA1, &A1 = mA1;
        int rc = ggAggData(&mA1, *schema.lookupRecord("r"), &t);
        ASSERT(!rc);

        Error err;
        Obj mX;  const Obj& X = mX;
        Obj mY;  const Obj& Y = mY;

        mA1.setField(&mX, &err, "a", 4.0);
        mA1.setField(&mX, &err, "c", "22");
        mA1.setField(&mX, &err, "d", "33");
        mA1.setField(&mX, &err, "f", "c", 99);
        mA1.setField(&mX, &err, "f", "d", 999);

        mA1.getField(&mX, &err, false, "g");
        mX.makeValue();

        mA1.setField(&mX, &err, "g", "c", 88);
        mA1.setField(&mX, &err, "g", "d", 888);

        mA1.getField(&mX, &err, false, "h");
        mX.makeSelection(&mY, &err, "a", 77);

        mA1.getField(&mX, &err, false, "i");
        mX.insertItem(&mY, &err, 0, 6);
        mX.insertItem(&mY, &err, 1, 5);
        mX.insertItem(&mY, &err, 2, 4);

        mA1.getField(&mX, &err, false, "j");
        mX.resize(&err, 2);

        mA1.setField(&mX, &err, "j", 0, "c", 66);
        mA1.setField(&mX, &err, "j", 0, "d", 666);
        mA1.setField(&mX, &err, "j", 1, "c", 55);
        mA1.setField(&mX, &err, "j", 1, "d", 555);

        mA1.getField(&mX, &err, false, "k");
        mX.resize(&err, 2);

        mA1.getField(&mX, &err, false, "k", 0);
        mX.makeSelection(&mY, &err, "a", 44);
        mA1.getField(&mX, &err, false, "k", 1);
        mX.makeSelection(&mY, &err, "b", 333);

        mA1.setField(&mX, &err, "m", 1);    // == enumerator string "vee"
        mA1.setField(&mX, &err, "n", "ex"); // == enumerator ID 2
        A1.getField(&mX, &err, false, "i");
        ASSERT(X.enumerationConstraint() == 0);
        A1.getField(&mX, &err, false, "m");
        ASSERT(X.enumerationConstraint() == schema.lookupEnumeration("f"));

        mA1.getField(&mX, &err, false, "o");
        mX.resize(&err, 2);

        mA1.setField(&mX, &err, "o", 0, "33");

        mA1.getField(&mX, &err, false, "p");
        mX.resize(&err, 2);

        mA1.getField(&mX, &err, false, "q");
        mX.resize(&err, 2);

        mA1.getField(&mY, &err, false, "j", 0);

        mA1.setField(&mX, &err, "p", 0, Y);
        mA1.setField(&mX, &err, "q", 1, "ex");

        if (veryVerbose) P(A1);

        bcem_AggregateRaw mA2, &A2 = mA2;
        rc = ggAggData(&mA2, *schema.lookupRecord("r"), &t);
        ASSERT(!rc);

        if (verbose) cout << "Testing type name" << bsl::endl;
        ASSERT(streq("r", TN::className(A1)));
        ASSERT(TC::BDEAT_SEQUENCE_CATEGORY == TCF::select(A1));

        A1.getField(&mX, &err, false, "a");
        ASSERT(      0 == TN::className(X) );
        ASSERT(TC::BDEAT_SIMPLE_CATEGORY == TCF::select(X));

        A1.getField(&mX, &err, false, "b");
        ASSERT(      0 == TN::className(X) );
        ASSERT(TC::BDEAT_SIMPLE_CATEGORY == TCF::select(X));

        A1.getField(&mX, &err, false, "c");
        ASSERT(      0 == TN::className(X) );
        ASSERT(TC::BDEAT_SIMPLE_CATEGORY == TCF::select(X));

        A1.getField(&mX, &err, false, "d");
        ASSERT(      0 == TN::className(X) );
        ASSERT(TC::BDEAT_SIMPLE_CATEGORY == TCF::select(X));

        A1.getField(&mX, &err, false, "f");
        ASSERT(streq("s", TN::className(X)));
        ASSERT(TC::BDEAT_SEQUENCE_CATEGORY == TCF::select(X));

        A1.getField(&mX, &err, false, "g");
        ASSERT(streq("s", TN::className(X)));
        ASSERT(TC::BDEAT_SEQUENCE_CATEGORY == TCF::select(X));

        A1.getField(&mX, &err, false, "h");
        ASSERT(streq("c", TN::className(X)));
        ASSERT(TC::BDEAT_CHOICE_CATEGORY == TCF::select(X));

        A1.getField(&mX, &err, false, "i");
        ASSERT(      0 == TN::className(X) );
        ASSERT(TC::BDEAT_ARRAY_CATEGORY == TCF::select(X));

        A1.getField(&mX, &err, false, "j");
        ASSERT(streq("s", TN::className(X)));
        ASSERT(TC::BDEAT_ARRAY_CATEGORY == TCF::select(X));

        A1.getField(&mX, &err, false, "k");
        ASSERT(streq("c", TN::className(X)));
        ASSERT(TC::BDEAT_ARRAY_CATEGORY == TCF::select(X));

        A1.getField(&mX, &err, false, "m");
        ASSERT(streq("f", TN::className(X)));
        ASSERT(TC::BDEAT_ENUMERATION_CATEGORY == TCF::select(X));

        A1.getField(&mX, &err, false, "n");
        ASSERT(streq("f", TN::className(X)));
        ASSERT(TC::BDEAT_ENUMERATION_CATEGORY == TCF::select(X));

        A1.getField(&mX, &err, false, "o");
        ASSERT(      0 == TN::className(X) );
        ASSERT(TC::BDEAT_ARRAY_CATEGORY == TCF::select(X));

        A1.getField(&mX, &err, false, "q");
        ASSERT(      0 == TN::className(X) );
        ASSERT(TC::BDEAT_ARRAY_CATEGORY == TCF::select(X));

        A1.getField(&mX, &err, false, "p");
        ASSERT(streq("s", TN::className(X)));
        ASSERT(TC::BDEAT_ARRAY_CATEGORY == TCF::select(X));

        if (verbose) cout << "Testing category selection" << bsl::endl;
        ASSERT(0 ==
               bdeat_TypeCategory::Select<bcem_AggregateRaw>::BDEAT_SELECTION);

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
        if (verbose) cout << "Testing bdeat_SequenceFunctions"<< bsl::endl;
        ASSERT(SF::IsSequence<bcem_AggregateRaw>::VALUE);

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
        mA1.getField(&mX, &err, false, "f");
        SF::accessAttribute(X, theAccessor, "c", 1);
        ASSERT(theAccessor.matchValue(99));
        SF::accessAttribute(X, theAccessor, 1);
        ASSERT(theAccessor.matchValue(999));
        SF::accessAttributes(X, theAccessor);
        ASSERT(theAccessor.matchValues(99, 999));

        Obj mB; const Obj& B = mB;
        ggAggData(&mB, *schema.lookupRecord("r"), &t);

        bsl::list<CERef>& values = newManipulator.elements();
        const CERef& RefA = getCERef(ET::BDEM_DOUBLE, 1);
        values.push_back(RefA);
        SF::manipulateAttribute(&mB, newManipulator, "a", 1);
        mB.getField(&mX, &err, false, "a");
        ASSERT(compareCERefs(RefA, X.asElemRef()));

        newManipulator.reset();
        const CERef& RefB = getCERef(ET::BDEM_DOUBLE, 2);
        values.push_back(RefB);
        SF::manipulateAttribute(&mB, newManipulator, "b", 1);
        mB.getField(&mX, &err, false, "b");
        ASSERT(compareCERefs(RefB, X.asElemRef()));

        newManipulator.reset();
        const CERef& RefC = getCERef(ET::BDEM_STRING, 2);
        values.push_back(RefC);
        SF::manipulateAttribute(&mB, newManipulator, "c", 1);
        mB.getField(&mX, &err, false, "c");
        LOOP2_ASSERT(RefC, X.asElemRef(), compareCERefs(RefC, X.asElemRef()));

        newManipulator.reset();
        const CERef& RefD = getCERef(ET::BDEM_STRING, 0);
        values.push_back(RefD);
        SF::manipulateAttribute(&mB, newManipulator, "d", 1);
        mB.getField(&mX, &err, false, "d");
        ASSERT(compareCERefs(RefD, X.asElemRef()));

        newManipulator.reset();
        const CERef& RefFC = getCERef(ET::BDEM_INT, 2);
        const CERef& RefFD = getCERef(ET::BDEM_INT64, 1);
        values.push_back(RefFC);
        values.push_back(RefFD);
        SF::manipulateAttribute(&mB, newManipulator, "f", 1);
        mB.getField(&mX, &err, false, "f");
        mX.getField(&mY, &err, false, "c");
        LOOP2_ASSERT(RefFC, Y.asElemRef(),
                     compareCERefs(RefFC, Y.asElemRef()));
        mX.getField(&mY, &err, false, "d");
        ASSERT(compareCERefs(RefFD, Y.asElemRef()));

        newManipulator.reset();
        const CERef& RefGC = getCERef(ET::BDEM_INT, 0);
        const CERef& RefGD = getCERef(ET::BDEM_INT64, 0);
        values.push_back(RefGC);
        values.push_back(RefGD);
        SF::manipulateAttribute(&mB, newManipulator, "g", 1);
        mB.getField(&mX, &err, false, "g");
        mX.getField(&mY, &err, false, "c");
        ASSERT(compareCERefs(RefGC, Y.asElemRef()));
        mX.getField(&mY, &err, false, "d");
        ASSERT(compareCERefs(RefGD, Y.asElemRef()));

        newManipulator.reset();
        int selector = 0;
        const CERef  RefSelector(&selector, &bdem_Properties::d_intAttr);
        values.push_back(RefSelector);
        const CERef& RefH = getCERef(ET::BDEM_CHAR, 1);
        values.push_back(RefH);
        SF::manipulateAttribute(&mB, newManipulator, "h", 1);
        mB.getField(&mX, &err, false, "h");
        ASSERT(selector == X.selectorId());
        ASSERT(bsl::string("a") == X.selector());
        mX.getField(&mY, &err, false, "a");
        ASSERT(compareCERefs(RefH, Y.asElemRef()));

        newManipulator.reset();
        int numItems = 1;
        const CERef  RefNI(&numItems, &bdem_Properties::d_intAttr);
        const CERef& RefI = getCERef(ET::BDEM_INT, 1);
        values.push_back(RefNI);
        values.push_back(RefI);
        SF::manipulateAttribute(&mB, newManipulator, "i", 1);
        mB.getField(&mX, &err, false, "i");
        ASSERT(1 == X.length());
        mX.getField(&mY, &err, false, 0);
        LOOP2_ASSERT(RefI, Y.asElemRef(), compareCERefs(RefI, Y.asElemRef()));

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
        mB.getField(&mX, &err, false, "j");
        ASSERT(2 == X.length());
        X.getField(&mY, &err, false, 0);
        Y.getField(&mX, &err, false, "c");
        ASSERT(compareCERefs(RefJA1, X.asElemRef()));
        Y.getField(&mX, &err, false, "d");
        ASSERT(compareCERefs(RefJA2, X.asElemRef()));
        B.getField(&mX, &err, false, "j");
        X.getField(&mY, &err, false, 1);
        Y.getField(&mX, &err, false, "c");
        ASSERT(compareCERefs(RefJB1, X.asElemRef()));
        Y.getField(&mX, &err, false, "d");
        ASSERT(compareCERefs(RefJB2, X.asElemRef()));

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
        mB.getField(&mX, &err, false, "k");
        ASSERT(numItems == X.length());

        Obj RESA, RESB;
        B.getField(&RESA, &err, false, "k", 0);
        B.getField(&RESB, &err, false, "k", 1);

        ASSERT(s1 == RESA.selectorId());
        ASSERT(bsl::string("a") == RESA.selector());
        RESA.selection(&mX, &err);
        LOOP_ASSERT(RefKA, compareCERefs(RefKA, X.asElemRef()));
        ASSERT(s2 == RESB.selectorId());
        ASSERT(bsl::string("b") == RESB.selector());
        RESB.selection(&mX, &err);
        LOOP2_ASSERT(RefKB, X.asElemRef(),
                     compareCERefs(RefKB, X.asElemRef()));

        newManipulator.reset();
        int enumId = 1;
        const CERef enumRef1(&enumId, &bdem_Properties::d_intAttr);
        values.push_back(enumRef1);
        SF::manipulateAttribute(&mB, newManipulator, "m", 1);
        B.getField(&mX, &err, false, "m");
        LOOP2_ASSERT(enumRef1, X.asElemRef(),
                     compareCERefs(enumRef1, X.asElemRef()));
        enumId = 2;
        values.push_back(enumRef1);
        SF::manipulateAttribute(&mB, newManipulator, "m", 1);
        B.getField(&mX, &err, false, "m");
        LOOP2_ASSERT(enumRef1, X.asElemRef(),
                     compareCERefs(enumRef1, X.asElemRef()));

        newManipulator.reset();
        enumId = 1;
        bsl::string enumString = "1";
        const CERef enumRef2(&enumString, &bdem_Properties::d_stringAttr);
        values.push_back(enumRef2);
        SF::manipulateAttribute(&mB, newManipulator, "m", 1);
        B.getField(&mX, &err, false, "m");
        LOOP2_ASSERT(enumRef1, X.asElemRef(),
                     compareCERefs(enumRef1, X.asElemRef()));
        enumId = 2;
        enumString = "2";
        values.push_back(enumRef2);
        SF::manipulateAttribute(&mB, newManipulator, "m", 1);
        B.getField(&mX, &err, false, "m");
        LOOP2_ASSERT(enumRef1, X.asElemRef(),
                     compareCERefs(enumRef1, X.asElemRef()));

        newManipulator.reset();
        enumId = 1;
        values.push_back(enumRef1);
        SF::manipulateAttribute(&mB, newManipulator, "n", 1);
        B.getField(&mX, &err, false, "n");
        ASSERT(bsl::string("vee") == X.asElemRef().theString());

        newManipulator.reset();
        enumId = 2;
        values.push_back(enumRef1);
        SF::manipulateAttribute(&mB, newManipulator, "n", 1);
        B.getField(&mX, &err, false, "n");
        LOOP_ASSERT(X.asElemRef().theString(),
                    bsl::string("ex") == X.asElemRef().theString());

        newManipulator.reset();
        enumString = "1";
        values.push_back(enumRef2);
        SF::manipulateAttribute(&mB, newManipulator, "n", 1);
        B.getField(&mX, &err, false, "n");
        ASSERT(bsl::string("vee") == X.asElemRef().theString());

        newManipulator.reset();
        enumString = "2";
        values.push_back(enumRef2);
        SF::manipulateAttribute(&mB, newManipulator, "n", 1);
        B.getField(&mX, &err, false, "n");
        LOOP_ASSERT(X.asElemRef().theString(),
                    bsl::string("ex") == X.asElemRef().theString());

        newManipulator.reset();
        numItems = 2;
        const CERef& RefO = getCERef(ET::BDEM_INT, 0);
        values.push_back(RefNI);
        values.push_back(RefI);
        values.push_back(RefO);
        SF::manipulateAttribute(&mB, newManipulator, "o", 1);
        B.getField(&mX, &err, false, "o");
        ASSERT(2 == X.length());
        X.getField(&mY, &err, false, 0);
        LOOP_ASSERT(Y.asElemRef(), compareCERefs(RefI, Y.asElemRef()));
        X.getField(&mY, &err, false, 1);
        LOOP_ASSERT(Y.asElemRef(), compareCERefs(RefO, Y.asElemRef()));

        newManipulator.reset();
        numItems = 2;
        values.push_back(RefNI);
        values.push_back(RefJA1);
        values.push_back(RefJA2);
        values.push_back(RefJB1);
        values.push_back(RefJB2);
        SF::manipulateAttribute(&mB, newManipulator, "p", 1);
        B.getField(&mX, &err, false, "p");
        ASSERT(2 == X.length());
        X.getField(&mY, &err, false, 0);
        Y.getField(&mX, &err, false, "c");
        ASSERT(compareCERefs(RefJA1, X.asElemRef()));
        Y.getField(&mX, &err, false, "d");
        ASSERT(compareCERefs(RefJA2, X.asElemRef()));
        B.getField(&mX, &err, false, "p");
        X.getField(&mY, &err, false, 1);
        Y.getField(&mX, &err, false, "c");
        ASSERT(compareCERefs(RefJB1, X.asElemRef()));
        Y.getField(&mX, &err, false, "d");
        ASSERT(compareCERefs(RefJB2, X.asElemRef()));

        newManipulator.reset();
        numItems = 1;
        enumString = "1";
        values.push_back(RefNI);
        values.push_back(enumRef2);
        SF::manipulateAttribute(&mB, newManipulator, "q", 1);
        B.getField(&mX, &err, false, "q");
        ASSERT(1 == X.length());
        X.getField(&mY, &err, false, 0);
        ASSERT(bsl::string("vee") == Y.asElemRef().theString());

        Obj mA2f;
        mA2.getField(&mA2f, &err, false, "f");
        SF::manipulateAttribute(&mA2f, theManipulator,  0);
        A2.getField(&mX, &err, false, "f", "c");
        ASSERT(100 == X.asInt());
        SF::manipulateAttribute(&mA2f, theManipulator,  "d", 1);
        A2.getField(&mX, &err, false, "f", "d");
        ASSERT(101 == X.asInt64());
        SF::manipulateAttributes(&mA2f, theManipulator);
        A2.getField(&mX, &err, false, "f", "c");
        ASSERT(102 == X.asInt());
        A2.getField(&mX, &err, false, "f", "d");
        ASSERT(103 == X.asInt64());
        if (veryVerbose) P(A2);

        if (verbose) cout << "Testing bdeat_ChoiceFunctions" << bsl::endl;
        ASSERT(CF::IsChoice<bcem_AggregateRaw>::VALUE);
        int id = 99;
        A1.getField(&mX, &err, false, "h");
        id = CF::selectionId(X);
        ASSERT(0 == id);
        ASSERT(CF::hasSelection(X, 0));
        ASSERT(CF::hasSelection(X, 1));
        ASSERT(! CF::hasSelection(X, 2));
        ASSERT(CF::hasSelection(X, "a", 1));
        ASSERT(CF::hasSelection(X, "b", 1));
        ASSERT(! CF::hasSelection(X, "ab", 2));
        CF::accessSelection(X, theAccessor);
        ASSERT(theAccessor.matchValue(77));

        Obj mA2h;
        A2.getField(&mA2h, &err, false, "h");
        id = CF::selectionId(mA2h);
        ASSERT(bdeat_ChoiceFunctions::BDEAT_UNDEFINED_SELECTION_ID == id);
        CF::makeSelection(&mA2h, 0);
        ASSERT(0 == CF::selectionId(mA2h));
        CF::manipulateSelection(&mA2h, theManipulator);
        A2.getField(&mX, &err, false, "h", "a");
        ASSERT(104 == X.asChar());
        CF::makeSelection(&mA2h, "b", 1);
        ASSERT(1 == CF::selectionId(mA2h));
        CF::manipulateSelection(&mA2h, theManipulator);
        A2.getField(&mX, &err, false, "h", "");
        ASSERT(105 == X.asShort());
        if (veryVerbose) P(A2);

        if (verbose) cout << "Testing bdeat_ArrayFunctions" << bsl::endl;
        ASSERT(AF::IsArray<bcem_AggregateRaw>::VALUE);
        A1.getField(&mX, &err, false, "i");
        ASSERT(3 == AF::size(X));
        AF::accessElement(X, theAccessor, 0);
        ASSERT(theAccessor.matchValue(6));
        AF::accessElement(X, theAccessor, 2);
        ASSERT(theAccessor.matchValue(4));
        AF::accessElement(X, theAccessor, 1);
        ASSERT(theAccessor.matchValue(5));

        A2.setField(&mY, &err, "i", X);
        Obj mA2i(Y);
        AF::manipulateElement(&mA2i, theManipulator, 1);
        Y.getField(&mX, &err, false, 0);
        ASSERT(6   == X.asInt());
        Y.getField(&mX, &err, false, 1);
        ASSERT(106 == X.asInt());
        Y.getField(&mX, &err, false, 2);
        ASSERT(4   == X.asInt());
        AF::resize(&mA2i, 2);
        ASSERT(2   == Y.length());
        Y.getField(&mX, &err, false, 0);
        ASSERT(6   == X.asInt());
        Y.getField(&mX, &err, false, 1);
        ASSERT(106 == X.asInt());
        if (veryVerbose) P(A2);

        if (verbose) cout << "Testing bdeat_ArrayFunctions on TABLEs"
                          << bsl::endl;
        A1.getField(&mX, &err, false, "j");
        ASSERT(2 == AF::size(X));
        AF::accessElement(X, theAccessor, 0);
        ASSERT(theAccessor.matchValues(66, 666));
        AF::accessElement(X, theAccessor, 1);
        ASSERT(theAccessor.matchValues(55, 555));

        A2.setField(&mY, &err, "j", X);
        Obj mA2j(Y);
        AF::manipulateElement(&mA2j, theManipulator, 0);
        A2.getField(&mX, &err, false, "j", 0);
        X.getField(&mY, &err, false, "c");
        ASSERT(107 == Y.asInt());
        X.getField(&mY, &err, false, "d");
        ASSERT(108 == Y.asInt64());
        A2.getField(&mX, &err, false, "j", 1);
        X.getField(&mY, &err, false, "c");
        ASSERT(55  == Y.asInt());
        X.getField(&mY, &err, false, "d");
        ASSERT(555 == Y.asInt64());
        AF::resize(&mA2j, 3);
        A2.getField(&mX, &err, false, "j");
        ASSERT(3   == X.length());
        AF::manipulateElement(&mA2j, theManipulator, 2);
        A2.getField(&mX, &err, false, "j", 0);
        X.getField(&mY, &err, false, "c");
        ASSERT(107 == Y.asInt());
        X.getField(&mY, &err, false, "d");
        ASSERT(108 == Y.asInt64());
        A2.getField(&mX, &err, false, "j", 1);
        X.getField(&mY, &err, false, "c");
        ASSERT(55  == Y.asInt());
        X.getField(&mY, &err, false, "d");
        ASSERT(555 == Y.asInt64());
        A2.getField(&mX, &err, false, "j", 2);
        X.getField(&mY, &err, false, "c");
        ASSERT(109 == Y.asInt());
        X.getField(&mY, &err, false, "d");
        ASSERT(110 == Y.asInt64());
        if (veryVerbose) P(A2);

        if (verbose) cout << "Testing bdeat_ArrayFunctions on "
                          << "CHOICE_ARRAYs" << bsl::endl;
        A1.getField(&mX, &err, false, "k");
        ASSERT(2 == AF::size(X));
        X.getField(&mY, &err, false, 0);
        ASSERT(0 == CF::selectionId(Y));
        X.getField(&mY, &err, false, 1);
        ASSERT(1 == CF::selectionId(Y));
        AF::accessElement(X, theAccessor, 0);
        ASSERT(theAccessor.matchValue(44));
        AF::accessElement(X, theAccessor, 1);
        ASSERT(theAccessor.matchValue(333));

        A2.setField(&mY, &err, "k", X);
        Obj mA2k(Y);
        AF::manipulateElement(&mA2k, theManipulator, 0);
        Y.getField(&mX, &err, false, 0);
        ASSERT(1   == X.selectorId());
        Y.getField(&mX, &err, false, 0, "b");
        ASSERT(111 == X.asShort());
        Y.getField(&mX, &err, false, 1);
        ASSERT(1   == X.selectorId());
        Y.getField(&mX, &err, false, 1, "b");
        ASSERT(333 == X.asShort());
        if (veryVerbose) P(A2);
        AF::manipulateElement(&mA2k, theManipulator, 1);
        A2.getField(&mX, &err, false, "k", 0);
        ASSERT(1   == X.selectorId());
        A2.getField(&mX, &err, false, "k", 0, "b");
        ASSERT(111 == X.asShort());
        A2.getField(&mX, &err, false, "k", 1);
        ASSERT(0   == X.selectorId());
        A2.getField(&mX, &err, false, "k", 1, "a");
        ASSERT(112 == X.asChar());
        AF::resize(&mA2k, 3);
        A2.getField(&mX, &err, false, "k");
        ASSERT(3   == X.length());
        AF::manipulateElement(&mA2k, theManipulator, 2);
        A2.getField(&mX, &err, false, "k", 0);
        ASSERT(1   == X.selectorId());
        A2.getField(&mX, &err, false, "k", 0, "b");
        ASSERT(111 == X.asShort());
        A2.getField(&mX, &err, false, "k", 1);
        ASSERT(0   == X.selectorId());
        A2.getField(&mX, &err, false, "k", 1, "a");
        ASSERT(112 == X.asChar());
        A2.getField(&mX, &err, false, "k", 2);
        ASSERT(1   == X.selectorId());
        A2.getField(&mX, &err, false, "k", 2, "b");
        ASSERT(113 == X.asShort());
        if (veryVerbose) P(A2);

        // WHITE-BOX test of bcem_Aggregate_BdeatUtil::NullableAdapter
        if (verbose) cout << "Testing bdeat_NullableValueFunctions:"
                          << bsl::endl;
        typedef bcem_AggregateRaw_BdeatUtil::NullableAdapter NullableAdapter;
        ASSERT(! NVF::IsNullableValue<bcem_AggregateRaw>::VALUE);
        ASSERT(NVF::IsNullableValue<NullableAdapter>::VALUE);
        A1.getField(&mX, &err, false, "b");
        Obj mA1b(X);
        NullableAdapter nmA1b  = { &mA1b };
        ASSERT(NVF::isNull(nmA1b));
        A1.getField(&mX, &err, false, "g");
        Obj mA1g(X);
        NullableAdapter nmA1g  = { &mA1g };
        ASSERT(!NVF::isNull(nmA1g));
        A1.getField(&mX, &err, false, "g", "d");
        Obj mA1gd(X);
        NullableAdapter nmA1gd = { &mA1gd };
        ASSERT(! NVF::isNull(nmA1gd));
        NVF::accessValue(nmA1gd, theAccessor);
        ASSERT(theAccessor.matchValue(888));
        NVF::accessValue(nmA1g, theAccessor);
        ASSERT(theAccessor.matchValues(88, 888));

        A2.getField(&mX, &err, false, "b");
        Obj mA2b(X);
        NullableAdapter nmA2b  = { &mA2b };
        ASSERT(NVF::isNull(nmA2b));
        NVF::manipulateValue(&nmA2b, theManipulator);
        ASSERT(! NVF::isNull(nmA2b));
        ASSERT(114 == mA2b.asDouble());
        // Note: mA2b is a COPY.  A2.field("b") is still null.
        A2.getField(&mX, &err, false, "g");
        Obj mA2g(X);
        NullableAdapter nmA2g  = { &mA2g };
        ASSERT(NVF::isNull(nmA2g));
        mA2g.makeNull();
        NVF::makeValue(&nmA2g);
        ASSERT(! NVF::isNull(nmA2g));
        NVF::manipulateValue(&nmA2g, theManipulator);
        A2.getField(&mX, &err, false, "g", "d");
        ASSERT(!X.isNull());
        ASSERT(116 == X.asInt64());
        A2.getField(&mX, &err, false, "g", "c");
        ASSERT(115 == X.asInt());

        // Black-box test of null value manipulator through parent sequence
        mA2g.makeNull();
        ASSERT(NVF::isNull(nmA2g));
        A2.getField(&mX, &err, false, "g");
        X.makeValue();
        SF::manipulateAttribute(&A2, theManipulator, "g", 1);
        ASSERT(! X.isNull());
        A2.getField(&mX, &err, false, "g", "d");
        ASSERT(! X.isNull());
        ASSERT(118 == X.asInt64());
        A2.getField(&mX, &err, false, "g", "c");
        ASSERT(117 == X.asInt());
        if (veryVerbose) P(A2);

        if (verbose) cout << "Testing bdeat_EnumFunctions:" << bsl::endl;
        ASSERT(EF::IsEnumeration<bcem_AggregateRaw>::VALUE);
        int intValue;
        bsl::string stringValue;
        A1.getField(&mX, &err, false, "m");
        EF::toInt(&intValue, X);
        ASSERT(1 == intValue);
        EF::toString(&stringValue, X);
        ASSERT("vee" == stringValue);
        A1.getField(&mX, &err, false, "n");
        EF::toInt(&intValue, X);
        ASSERT(2 == intValue);
        EF::toString(&stringValue, X);
        ASSERT("ex" == stringValue);

        A2.getField(&mX, &err, false, "m");
        Obj mA2m(X); const Obj &A2m = mA2m;
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

        A2.getField(&mX, &err, false, "n");
        Obj mA2n(X); const Obj &A2n = mA2n;
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
        A2.getField(&mX, &err, false, "m");
        ASSERT(2 == X.asInt())
        theManipulator.reset(0);
        SF::manipulateAttribute(&mA2, theManipulator, "n", 1);
        A2.getField(&mX, &err, false, "n");
        ASSERT("you" == X.asString())

        if (veryVerbose) P(A2);

        if (verbose) cout << "Testing BER encoding/decoding" << bsl::endl;
        bsl::stringstream strm;
        bdem_BerEncoderOptions berEncoderOptions;
        berEncoderOptions.setTraceLevel(veryVeryVerbose);
        bdem_BerEncoder berEncoder(&berEncoderOptions);
        status = berEncoder.encode(strm, A1);
        bsl::cerr << berEncoder.loggedMessages();
        ASSERT(0 == status);
        // Assert that enumerator "ex" is NOT stored in alpha form in stream.
        ASSERT(bsl::string::npos == strm.str().find("ex"));

        Obj mA4; const Obj& A4 = mA4;
        ggAggData(&mA4, *schema.lookupRecord("r"), &t);
        ASSERT(! Obj::areEquivalent(A1, A4));

        bdem_BerDecoderOptions berDecoderOptions;
        berDecoderOptions.setTraceLevel(veryVeryVerbose);
        bdem_BerDecoder berDecoder(&berDecoderOptions);
        status = berDecoder.decode(strm, &mA4);
        bsl::cerr << berDecoder.loggedMessages();

        bsl::ostringstream A1str, A4str;
        A1str << A1;
        A4str << A4;

        if (veryVerbose) P(A4);

        destroyAggData(&mA1, &t);
        destroyAggData(&mA2, &t);
        destroyAggData(&mB, &t);
        destroyAggData(&mA4, &t);

      } break;
      case 21: {
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
        //   void makeValue() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'makeValue' FUNCTION"
                          << "\n============================"
                          << bsl::endl;
        {
            // Testing setting the value on a VOID aggregate
            Obj mX; const Obj& X = mX;
            Obj mY; const Obj& Y = mY;
            Obj mZ; const Obj& Z = mZ;
            ASSERT(Obj::areEquivalent(X, Y));
            ASSERT(Obj::areEquivalent(X, Z));

            mX.makeValue();
            ASSERT(!X.isError());
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

            bslma_TestAllocator t(veryVeryVerbose);

            Obj mX;  const Obj& X = mX;
            int rc = ggAggData(&mX, *RECORD, &t);
            ASSERT(!rc);

            Obj mY;  const Obj& Y = mY;
            rc = ggAggData(&mY, *RECORD, &t);
            ASSERT(!rc);

            if (veryVerbose) { T_ P_(SPEC) P(SCHEMA) P(X) };

            Error err;
            Obj mA;  const Obj& A = mA;
            Obj mB;  const Obj& B = mB;
            if (RecDef::BDEM_CHOICE_RECORD == RECORD->recordType()) {
                mX.makeSelection(&mA, &err, fldName);
                mY.makeSelection(&mB, &err, fldName);
            }

            const CERef A1 = getCERef(TYPE, 1);

            mY.setField(&mB, &err, fldName, A1);

            mX.getField(&mA, &err, false, fldName);
            mY.getField(&mB, &err, false, fldName);

            if (veryVerbose) { P(LINE) P(SCHEMA) P(A) P(B) }

            mA.makeValue();
            mB.makeValue();
            LOOP2_ASSERT(LINE, Y, !A.asElemRef().isNull());
            LOOP2_ASSERT(LINE, B, !B.asElemRef().isNull());

            if (ET::isScalarType(TYPE) && HAS_DEFAULT) {
                LOOP_ASSERT(LINE,
                            compareCERefs(A.asElemRef(),
                                          RECORD->field(0).defaultValue()));
            }
            else if (ET::isAggregateType(TYPE) && SCHEMA.numRecords() > 1) {
                const bdem_ElemRef&   ELEM_REF   = A.asElemRef();
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
                    mA.resize(&err, 1);
                    Obj mC;  const Obj& C = mC;
                    mA.arrayItem(&mC, &err, 0);
                    mC.makeValue();
                    LOOP_ASSERT(LINE,
                    SchemaAggUtil::canSatisfyRecord(C.asElemRef().theRow(),
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
                    mA.resize(&err, 1);
                    Obj mC;  const Obj& C = mC;
                    mA.arrayItem(&mC, &err, 0);
                    mC.makeValue();
                    LOOP_ASSERT(LINE,
                                SchemaAggUtil::canSatisfyRecord(
                                            C.asElemRef().theChoiceArrayItem(),
                                            CONSTRAINT));
                  } break;
                  default: {
                    ; // suppress warnings about incomplete case statement
                  }
                }
            }
            else if (NSA) {
                const bdem_Table& T = A.asElemRef().theTable();
                LOOP_ASSERT(LINE, 0 == T.numRows());
                LOOP_ASSERT(LINE, 1 == T.numColumns());
                LOOP_ASSERT(LINE, ET::fromArrayType(TYPE) == T.columnType(0));
            }
            else {
                LOOP_ASSERT(LINE, isUnset(A.asElemRef()));
            }

            destroyAggData(&mX, &t);
            destroyAggData(&mY, &t);
        }
      } break;
      case 20: {
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
        //   bool isNull() const;
        //   bool isNullable() const;
        //   int numSelections() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING REMAINING ACCESSORS"
                          << "\n===========================" << bsl::endl;

        if (veryVerbose) { cout << "\n\tTesting reset & isNull"
                                << "\n\t======================"
                                << bsl::endl; }
        {
            // For a null aggregate
            {
                Obj mX; const Obj& X = mX;
                Obj mY; const Obj& Y = mY;
                ASSERT(X.isNull());
                ASSERT(Y.isNull());
                mX.reset();
                ASSERT(ET::BDEM_VOID == X.dataType());
                ASSERT(0 == X.data());
                ASSERT(X.isNull());

                mY.reset();
                ASSERT(ET::BDEM_VOID == Y.dataType());
                ASSERT(0 == Y.data());
                ASSERT(Y.isNull());
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

                Obj mX;  const Obj& X = mX;
                int rc = ggAggData(&mX, *RECORD, &t);
                ASSERT(!rc);

                Obj mY;  const Obj& Y = mY;
                rc = ggAggData(&mY, *RECORD, &t);
                ASSERT(!rc);

                if (veryVerbose) { T_ P_(SPEC) P(SCHEMA) P(X) };

                Error err;
                Obj mA;  const Obj& A = mA;
                Obj mB;  const Obj& B = mB;
                mX.getField(&mA, &err, false, fldName);

// TBD: Uncomment
//                 LOOP2_ASSERT(LINE, IS_NULL, IS_NULL != A.isNull());

                if (RecDef::BDEM_CHOICE_RECORD == RECORD->recordType()) {
                    mX.makeSelection(&mA, &err, fldName);
                    mY.makeSelection(&mB, &err, fldName);
                }

                if (!ET::isAggregateType(TYPE)
                 && RecDef::BDEM_CHOICE_RECORD != RECORD->recordType()) {
                    LOOP3_ASSERT(LINE, X, IS_NULL,
                                 IS_NULL == A.isNull());
                }

                if (!ET::isAggregateType(TYPE)) {
                    mX.setField(&mA, &err, fldName, VN);
                    LOOP2_ASSERT(LINE, X, !A.isNull());
                }

                mX.setField(&mA, &err, fldName, VA);
                ASSERT(!A.isNull());

                mY.getField(&mB, &err, false, fldName);
                mB.makeNull();

                Obj mC(X);  const Obj& C = mC;
                Obj mD(Y);  const Obj& D = mD;

                mX.reset();
                ASSERT(ET::BDEM_VOID == X.dataType());
                ASSERT(0 == X.data());
                ASSERT(X.isNull());

                mY.reset();
                ASSERT(ET::BDEM_VOID == Y.dataType());
                ASSERT(0 == Y.data());
                ASSERT(Y.isNull());

                destroyAggData(&mC, &t);
                destroyAggData(&mD, &t);
            }
        }

        if (veryVerbose) { cout << "\n\tTesting numSelections"
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

                if (veryVerbose) { T_ P_(SPEC) P(SCHEMA) };

                bslma_TestAllocator t(veryVeryVerbose);

                Obj mX;  const Obj& X = mX;
                int rc = ggAggData(&mX, *REC, &t);
                ASSERT(!rc);

                Error err;
                Obj mA;  const Obj& A = mA;
                Obj mB;  const Obj& B = mB;
                if (2 == SCHEMA.numRecords()) {
                    X.getField(&mA, &err, false, REC->fieldName(0));
                    mA.insertItem(&mB, &err, 0, 1);
                    ASSERT(SCHEMA.record(0).numFields() ==
                           B.numSelections());
                }
                else {
                    ASSERT(REC->numFields() == X.numSelections());
                }

                destroyAggData(&mX, &t);
            }

            // Test that error message is printed if data type does not match
            {
                const CERef CEA = getCERef(ET::BDEM_DOUBLE, 1);
                bsl::string sa; const bsl::string& SA = sa;

                Obj mX;
                mX.setDataType(ET::BDEM_DOUBLE);
                mX.setDataPointer((void *) CEA.data());
                int rc = mX.numSelections();
                ASSERT(rc);
            }
        }

        if (veryVerbose) { cout << "\n\tTesting makeNull"
                                << "\n\t================" << bsl::endl; }
        {
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

                ET::Type    TYPE = NSA
                        ? ET::toArrayType(SCHEMA.record(1).field(0).elemType())
                        : RECORD->field(0).elemType();
                const char *fldName = RECORD->fieldName(0);

                const CERef VA = getCERef(TYPE, 1);
                const CERef VB = getCERef(TYPE, 2);

                bslma_TestAllocator t(veryVeryVerbose);

                int nf1 = 0, nf2 = 0;

                Obj mX;  const Obj& X = mX;
                int rc = ggAggData(&mX, *RECORD, &t);
                ASSERT(!rc);
                mX.setTopLevelAggregateNullnessPointer(&nf1);

                Obj mY;  const Obj& Y = mY;
                rc = ggAggData(&mY, *RECORD, &t);
                ASSERT(!rc);
                mY.setTopLevelAggregateNullnessPointer(&nf2);

                if (veryVerbose) { T_ P_(SPEC) P(SCHEMA) P(X) };

                Error err;
                Obj mA;  const Obj& A = mA;
                Obj mB;  const Obj& B = mB;
                if (RecDef::BDEM_CHOICE_RECORD == RECORD->recordType()) {
                    mX.makeSelection(&mA, &err, fldName, VA);
                    mY.makeSelection(&mB, &err, fldName, VA);
                } else {
                    mX.setField(&mA, &err, fldName, VA);
                    mY.setField(&mA, &err, fldName, VA);
                }

                if (veryVerbose) { T_ P_(SPEC) P(X) P(SCHEMA) };

                ASSERT(!A.isNull());
                if (NSA) {
                    ASSERT(compareNillableTable(A, VA));
                }
                else {
                    ASSERT(VA == A.asElemRef());
                }

                mA.makeNull();
                ASSERT(A.isNull());
                ASSERT(A.asElemRef().isNull());
                ASSERT(isUnset(A.asElemRef()));

                mX.setField(&mA, &err, fldName, VB);
                ASSERT(!A.isNull());

                if (NSA) {
                    ASSERT(compareNillableTable(A, VB));
                }
                else {
                    ASSERT(VB == A.asElemRef());
                }

                mX.makeNull();
                LOOP_ASSERT(X, X.isNull());
                ASSERT(X.asElemRef().isNull());
                ASSERT(isUnset(X.asElemRef()));

                Obj mC(X);

                mX = Y;
                ASSERT(!X.isNull());
                ASSERT(Obj::areIdentical(X, Y));

                destroyAggData(&mC, &t);
                destroyAggData(&mY, &t);
            }
        }
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING 'length' ACCESSORS:
        //
        // Concerns:
        //   - length returns the length of the array aggregate
        //   - size returns the length of the array aggregate
        //
        // Plan:
        //   - Construct object mY from a set S of aggregates of all array
        //     types.  Assert that the initial length of mY is correct.
        //
        // Testing:
        //   int length() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'length' ACCESSORS"
                          << "\n=========================="
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

            bslma_TestAllocator t;
            Obj mX;  const Obj& X = mX;
            int rc = ggAggData(&mX, *RECORD, &t);
            ASSERT(!rc);

            Error err;
            Obj mY;  const Obj& Y = mY;
            rc = mX.setField(&mY, &err, fldName, VA);
            ASSERT(!rc);

            const int LEN = getLength(VA);

            LOOP_ASSERT(ARRAY_TYPE, Y.length() == LEN);
            if (NSA) {
                LOOP_ASSERT(Y, ET::BDEM_TABLE == Y.asElemRef().type());
            }
            else {
                LOOP_ASSERT(Y, ARRAY_TYPE == Y.asElemRef().type());
            }

            for (int j = 0; j < LEN; ++j) {
                Obj mA; const Obj& A = mA;
                mY.getField(&mA, &err, false, j);
                Obj mB; const Obj& B = mB;
                mY.arrayItem(&mB, &err, j);

                if (veryVerbose) { P(X) P(Y) P(A) P(B) };

                LOOP2_ASSERT(ARRAY_TYPE, j, Obj::areEquivalent(A, B));
                LOOP2_ASSERT(ARRAY_TYPE, j, !A.isNull());
                LOOP2_ASSERT(ARRAY_TYPE, j, !B.isNull());
                LOOP2_ASSERT(A, j, TYPE == A.asElemRef().type());
                LOOP2_ASSERT(B, j, TYPE == B.asElemRef().type());

                B.makeNull();
                bool isNull = NSA ? true : false;
                LOOP3_ASSERT(ARRAY_TYPE, j, Y, isNull == B.isNull());
                LOOP2_ASSERT(ARRAY_TYPE, j, isNull == A.isNull());
                LOOP2_ASSERT(ARRAY_TYPE, j, isNull == B.isNull());
                LOOP2_ASSERT(ARRAY_TYPE, j, isUnset(A.asElemRef()));
                LOOP2_ASSERT(ARRAY_TYPE, j, isUnset(B.asElemRef()));
                LOOP2_ASSERT(A, j, TYPE == A.asElemRef().type());
                LOOP2_ASSERT(B, j, TYPE == B.asElemRef().type());

                B.setValue(&err, VB);
                LOOP2_ASSERT(ARRAY_TYPE, j, !A.isNull());
                LOOP2_ASSERT(ARRAY_TYPE, j, !B.isNull());
                LOOP2_ASSERT(ARRAY_TYPE, j, VB == A.asElemRef());
                LOOP2_ASSERT(ARRAY_TYPE, j, VB == B.asElemRef());
                LOOP2_ASSERT(A, j, TYPE == A.asElemRef().type());
                LOOP2_ASSERT(B, j, TYPE == B.asElemRef().type());

                A.makeNull();
                LOOP2_ASSERT(ARRAY_TYPE, j, isNull == A.isNull());
                LOOP2_ASSERT(ARRAY_TYPE, j, isNull == B.isNull());
                LOOP2_ASSERT(ARRAY_TYPE, j, isUnset(A.asElemRef()));
                LOOP2_ASSERT(ARRAY_TYPE, j, isUnset(B.asElemRef()));

                B.setValue(&err, VB);
                LOOP2_ASSERT(ARRAY_TYPE, j, !A.isNull());
                LOOP2_ASSERT(ARRAY_TYPE, j, !B.isNull());
                LOOP2_ASSERT(ARRAY_TYPE, j, VB == A.asElemRef());
                LOOP2_ASSERT(ARRAY_TYPE, j, VB == B.asElemRef());
                LOOP2_ASSERT(A, j, TYPE == A.asElemRef().type());
                LOOP2_ASSERT(B, j, TYPE == B.asElemRef().type());

                B.makeNull();
                LOOP2_ASSERT(ARRAY_TYPE, j, isNull == A.isNull());
                LOOP2_ASSERT(ARRAY_TYPE, j, isNull == B.isNull());
                LOOP2_ASSERT(ARRAY_TYPE, j, isUnset(A.asElemRef()));
                LOOP2_ASSERT(ARRAY_TYPE, j, isUnset(B.asElemRef()));

                A.setValue(&err, VB);
                LOOP2_ASSERT(ARRAY_TYPE, j, !A.isNull());
                LOOP2_ASSERT(ARRAY_TYPE, j, !B.isNull());
                LOOP2_ASSERT(ARRAY_TYPE, j, VB == A.asElemRef());
                LOOP2_ASSERT(ARRAY_TYPE, j, VB == B.asElemRef());
            }

            destroyAggData(&mX, &t);
        }
      } break;
      case 18: {
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
        //   bcem_AggregateRaw(const bcem_AggregateRaw&  other);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING COPY CONSTRUCTOR"
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

            ET::Type TYPE = NSA
                        ? ET::toArrayType(SCHEMA.record(1).field(0).elemType())
                        : RECORD->field(0).elemType();
            const char *fldName = RECORD->fieldName(0);

            const CERef A1 = getCERef(TYPE, 1);
            const CERef A2 = getCERef(TYPE, 2);

            if (veryVerbose) { T_ P_(SPEC) P(SCHEMA) };

            bslma_TestAllocator t(veryVeryVerbose);

            Obj mX;  const Obj& X = mX;
            int rc = ggAggData(&mX, *RECORD, &t);
            ASSERT(!rc);

            Obj mY;  const Obj& Y = mY;
            rc = ggAggData(&mY, *RECORD, &t);
            ASSERT(!rc);

            if (veryVerbose) { T_ P(X) };

            Error err;
            Obj mU;  const Obj& U = mU;
            Obj mV;  const Obj& V = mV;

            mX.setField(&mU, &err, fldName, A1);
            mY.getField(&mV, &err, false, fldName);
            mV.makeNull();

            {
                Obj mA(U);  const Obj& A = mA;
                ASSERT(Obj::areIdentical(A, U));

                mA.setValue(&err, A2);
                ASSERT(Obj::areIdentical(A, U));
                mA.makeNull();
                ASSERT(Obj::areIdentical(A, U));

                Obj mB(V); const Obj& B = mB;
                ASSERT(Obj::areIdentical(B, V));
                mB.setValue(&err, A2);
                ASSERT(Obj::areIdentical(B, V));
            }

            destroyAggData(&mX, &t);
            destroyAggData(&mY, &t);
        }
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING 'makeSelectionByIndex' MANIPULATORS & 'selectorIndex':
        //
        // Concerns:
        //   - Confirm that the single argument function sets the selector to
        //     the specified index.
        //   - Confirm that the two argument function sets the selector to
        //     the specified index and the value at that index to the specified
        //     value.
        //   - Both functions return the selected aggregated by reference.
        //   - If the aggregate does not store a choice or choice array then
        //     an error object is returned.
        //   - 'selectorIndex' returns the correct selected index
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
        //   int makeSelectionByIndex(Obj *obj, Error *err, idx) const;
        //   int makeSelectionByIndex(Obj *obj, Error *err, idx, value) const;
        //   int selectorIndex() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'makeSelectionByIndex' MANIPULATORS"
                          << "\n==========================================="
                          << bsl::endl;

        if (verbose) cout << "\nTesting choice aggregates"
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

                if (veryVerbose) { T_ P(SPEC) P(SCHEMA) };

                bslma_TestAllocator t(veryVeryVerbose);

                Obj mW;  const Obj& W = mW;
                int rc = ggAggData(&mW, REC, &t);
                ASSERT(!rc);

                Obj mX;  const Obj& X = mX;
                rc = ggAggData(&mX, REC, &t);
                ASSERT(!rc);

                Obj mY;  const Obj& Y = mY;
                rc = ggAggData(&mY, REC, &t);
                ASSERT(!rc);

                if (veryVerbose) { T_ P(X) };

                Error err;
                const char *fldName = REC.fieldName(0);

                Obj mA;  const Obj& A = mA;
                Obj mB;  const Obj& B = mB;
                Obj mC;  const Obj& C = mC;

                W.getField(&mA, &err, true, fldName);
                Y.getField(&mB, &err, true, fldName);
                X.getField(&mC, &err, true, fldName);

                if (veryVerbose) { T_ P(A) };

                for (int j = 0; j < SUB_REC.numFields(); ++j) {
                    ET::Type     TYPE = SUB_REC.field(j).elemType();
                    const char  *NAME = SUB_REC.fieldName(j);
                    const CERef  VA   = getCERef(TYPE, 1);

                    Obj mD;  const Obj& D = mD;
                    Obj mE;  const Obj& E = mE;
                    Obj mF;  const Obj& F = mF;
                    Obj mG;  const Obj& G = mG;

                    rc = mA.makeSelection(&mD, &err, NAME);
                    ASSERT(!rc);

                    rc = mB.makeSelection(&mE, &err, NAME, VA);
                    ASSERT(!rc);

                    rc = mC.makeSelectionByIndex(&mF, &err, j);
                    ASSERT(!rc);
                    ASSERT(D.asElemRef() == F.asElemRef());
                    ASSERT(j == C.selectorIndex());

                    rc = mC.makeSelectionByIndex(&mG, &err, j, VA);
                    ASSERT(!rc);
                    ASSERT(G.asElemRef() == E.asElemRef());
                    ASSERT(j == C.selectorIndex());
                }

                destroyAggData(&mX, &t);
                destroyAggData(&mY, &t);
                destroyAggData(&mW, &t);
            }
        }

        if (verbose) cout << "\nTesting choice array item aggregates"
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
                const RecDef& REC     = SCHEMA.record(SCHEMA.numRecords() - 1);
                const RecDef& SUB_REC = SCHEMA.record(0);

                if (veryVerbose) { T_ P(SPEC) P(SCHEMA) };

                bslma_TestAllocator t(veryVeryVerbose);

                Obj mW;  const Obj& W = mW;
                Obj mX;  const Obj& X = mX;
                Obj mY;  const Obj& Y = mY;

                ggAggData(&mW, REC, &t);
                ggAggData(&mX, REC, &t);
                ggAggData(&mY, REC, &t);

                if (veryVerbose) { T_ P(X) };

                Error err;
                const char *fldName = REC.fieldName(0);

                Obj mA;  const Obj& A = mA;
                Obj mB;  const Obj& B = mB;
                Obj mC;  const Obj& C = mC;

                W.getField(&mA, &err, true, fldName);
                Y.getField(&mB, &err, true, fldName);
                X.getField(&mC, &err, true, fldName);

                for (int j = 0; j < SUB_REC.numFields(); ++j) {
                    ET::Type    TYPE    = SUB_REC.field(j).elemType();
                    const char *selName = SUB_REC.fieldName(j);

                    const CERef VA = getCERef(TYPE, 1);
                    const CERef VB = getCERef(TYPE, 2);

                    mA.insertItems(&err, 0, 1);
                    mB.insertItems(&err, 0, 1);
                    mC.insertItems(&err, 0, 1);

                    Obj mD;  const Obj& D = mD;
                    Obj mE;  const Obj& E = mE;
                    Obj mF;  const Obj& F = mF;
                    mA.getField(&mD, &err, true, 0);
                    mB.getField(&mE, &err, true, 0);
                    mC.getField(&mF, &err, true, 0);

                    Obj mG;  const Obj& G = mG;
                    Obj mH;  const Obj& H = mH;
                    Obj mI;  const Obj& I = mI;
                    Obj mJ;  const Obj& J = mJ;
                    int rc = mD.makeSelection(&mG, &err, selName);
                    ASSERT(!rc);
                    rc = mE.makeSelection(&mH, &err, selName, VA);
                    ASSERT(!rc);

                    rc = mF.makeSelectionByIndex(&mI, &err, j);
                    ASSERT(!rc);
                    ASSERT(I.asElemRef() == G.asElemRef());
                    ASSERT(j == F.selectorIndex());

                    rc = mF.makeSelectionById(&mJ, &err, j, VA);
                    ASSERT(!rc);
                    ASSERT(J.asElemRef() == VA);
                    ASSERT(j == F.selectorIndex());
                }

                destroyAggData(&mW, &t);
                destroyAggData(&mX, &t);
                destroyAggData(&mY, &t);
            }
        }
      } break;
      case 16: {
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
        //   int makeSelectionById(Obj *obj, Error *err, int id) const;
        //   int makeSelectionById(Obj *obj, Error *err, int id, value) const;
        //   int selectorId() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'makeSelectionById' MANIPULATORS"
                          << "\n========================================"
                          << bsl::endl;

        if (verbose) cout << "\nTesting choice aggregates"
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

                if (veryVerbose) { T_ P(SPEC) P(SCHEMA) };

                bslma_TestAllocator t(veryVeryVerbose);

                Obj mW;  const Obj& W = mW;
                int rc = ggAggData(&mW, REC, &t);
                ASSERT(!rc);

                Obj mX;  const Obj& X = mX;
                rc = ggAggData(&mX, REC, &t);
                ASSERT(!rc);

                Obj mY;  const Obj& Y = mY;
                rc = ggAggData(&mY, REC, &t);
                ASSERT(!rc);

                if (veryVerbose) { T_ P(X) };

                Error err;
                const char *fldName = REC.fieldName(0);

                Obj mA;  const Obj& A = mA;
                Obj mB;  const Obj& B = mB;
                Obj mC;  const Obj& C = mC;

                W.getField(&mA, &err, true, fldName);
                Y.getField(&mB, &err, true, fldName);
                X.getField(&mC, &err, true, fldName);

                if (veryVerbose) { T_ P(A) };

                for (int j = 0; j < SUB_REC.numFields(); ++j) {
                    ET::Type     TYPE = SUB_REC.field(j).elemType();
                    const int    ID   = SUB_REC.fieldId(j);
                    const char  *NAME = SUB_REC.fieldName(j);
                    const CERef  VA   = getCERef(TYPE, 1);

                    Obj mD;  const Obj& D = mD;
                    Obj mE;  const Obj& E = mE;
                    Obj mF;  const Obj& F = mF;
                    Obj mG;  const Obj& G = mG;

                    rc = mA.makeSelection(&mD, &err, NAME);
                    ASSERT(!rc);

                    rc = mB.makeSelection(&mE, &err, NAME, VA);
                    ASSERT(!rc);

                    rc = mC.makeSelectionById(&mF, &err, ID);
                    ASSERT(!rc);
                    ASSERT(D.asElemRef() == F.asElemRef());
                    ASSERT(ID == C.selectorId());

                    rc = mC.makeSelectionById(&mG, &err, ID, VA);
                    ASSERT(!rc);
                    ASSERT(G.asElemRef() == E.asElemRef());
                    ASSERT(ID == C.selectorId());
                }

                destroyAggData(&mX, &t);
                destroyAggData(&mY, &t);
                destroyAggData(&mW, &t);
            }
        }

        if (verbose) cout << "\nTesting choice array item aggregates"
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
                const RecDef& REC     = SCHEMA.record(SCHEMA.numRecords() - 1);
                const RecDef& SUB_REC = SCHEMA.record(0);

                if (veryVerbose) { T_ P(SPEC) P(SCHEMA) };

                bslma_TestAllocator t(veryVeryVerbose);

                Obj mW;  const Obj& W = mW;
                Obj mX;  const Obj& X = mX;
                Obj mY;  const Obj& Y = mY;

                ggAggData(&mW, REC, &t);
                ggAggData(&mX, REC, &t);
                ggAggData(&mY, REC, &t);

                if (veryVerbose) { T_ P(X) };

                Error err;
                const char *fldName = REC.fieldName(0);

                Obj mA;  const Obj& A = mA;
                Obj mB;  const Obj& B = mB;
                Obj mC;  const Obj& C = mC;

                W.getField(&mA, &err, true, fldName);
                Y.getField(&mB, &err, true, fldName);
                X.getField(&mC, &err, true, fldName);

                for (int j = 0; j < SUB_REC.numFields(); ++j) {
                    ET::Type    TYPE    = SUB_REC.field(j).elemType();
                    const int   ID      = SUB_REC.fieldId(j);
                    const char *selName = SUB_REC.fieldName(j);

                    const CERef VA = getCERef(TYPE, 1);
                    const CERef VB = getCERef(TYPE, 2);

                    mA.insertItems(&err, 0, 1);
                    mB.insertItems(&err, 0, 1);
                    mC.insertItems(&err, 0, 1);

                    Obj mD;  const Obj& D = mD;
                    Obj mE;  const Obj& E = mE;
                    Obj mF;  const Obj& F = mF;
                    mA.getField(&mD, &err, true, 0);
                    mB.getField(&mE, &err, true, 0);
                    mC.getField(&mF, &err, true, 0);

                    Obj mG;  const Obj& G = mG;
                    Obj mH;  const Obj& H = mH;
                    Obj mI;  const Obj& I = mI;
                    Obj mJ;  const Obj& J = mJ;
                    int rc = mD.makeSelection(&mG, &err, selName);
                    ASSERT(!rc);
                    rc = mE.makeSelection(&mH, &err, selName, VA);
                    ASSERT(!rc);

                    rc = mF.makeSelectionById(&mI, &err, ID);
                    ASSERT(!rc);
                    ASSERT(I.asElemRef() == G.asElemRef());
                    ASSERT(ID == F.selectorId());

                    rc = mF.makeSelectionById(&mJ, &err, ID, VA);
                    ASSERT(!rc);
                    ASSERT(J.asElemRef() == VA);
                    ASSERT(ID == F.selectorId());
                }

                destroyAggData(&mW, &t);
                destroyAggData(&mX, &t);
                destroyAggData(&mY, &t);
            }
        }
      } break;
      case 15: {
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
        //  int makeSelection(Obj *obj, Error *error, newSelector) const;
        //  int makeSelection(Obj *obj, Error *error, newSelector, val) const;
        //  int selection(Obj *obj, Error *error) const;
        //  const char *selector() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING CHOICE MANIPULATORS"
                          << "\n===========================" << bsl::endl;

        if (verbose) cout << "\nTesting choice aggregates"
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

                const char *fldName = RECORD->fieldName(0);

                const CERef VA = getCERef(TYPE, 1);

                bslma_TestAllocator t(veryVeryVerbose);
                Obj mX;  const Obj& X = mX;
                int rc = ggAggData(&mX, *RECORD, &t);
                ASSERT(!rc);

                Obj mY;  const Obj& Y = mY;
                rc = ggAggData(&mY, *RECORD, &t);
                ASSERT(!rc);

                if (veryVerbose) { T_ P_(SPEC) P(SCHEMA) P(X) P(Y) };

                Error err;
                Obj mA;  const Obj& A = mA;
                rc = mY.makeSelection(&mA, &err, fldName);
                ASSERT(!rc);

                LOOP2_ASSERT(LINE, Y, !Y.isNull());
                // TBD: Uncomment
//                 LOOP4_ASSERT(LINE, IS_NULL, Y, Y.field(fldName),
//                              IS_NULL == Y.field(fldName).isNul2());
                if (ET::isScalarType(TYPE)) {
                    if (FIELD_DEF.defaultValue().isNull()) {
                        ASSERT(isUnset(mA.asElemRef()));
                    }
                    else {
                        LOOP3_ASSERT(LINE,
                                     FIELD_DEF.defaultValue(),
                                     mA.asElemRef(),
                                     compareCERefs(
                                                 FIELD_DEF.defaultValue(),
                                                 mA.asElemRef()));
                    }
                }

                rc = mY.setField(&mA, &err, fldName, VA);
                ASSERT(!rc);

                Obj mZ; const Obj& Z = mZ;
                rc = Y.getField(&mZ, &err, false, fldName);
                ASSERT(!rc);

                Obj mW;  const Obj& W = mW;
                rc = ggAggData(&mW, *RECORD, &t);
                ASSERT(!rc);

                rc = mW.makeSelection(&mA, &err, fldName);
                ASSERT(!rc);

                Obj mB; const Obj& B = mB;
                rc = W.getField(&mB, &err, false, fldName);
                ASSERT(!rc);

                rc = mX.makeSelection(&mA, &err, fldName);
                ASSERT(!rc);
                ASSERT(A.asElemRef() == B.asElemRef());
                ASSERT(streq(fldName, X.selector()));

                Obj mC; const Obj& C = mC;
                rc = X.selection(&mC, &err);
                ASSERT(!rc);
                ASSERT(C.asElemRef() == B.asElemRef());

                Obj mN;  const Obj& N = mN;
                rc = mX.makeSelection(&mN, &err, "");
                ASSERT(!rc);
                ASSERT(bdem_ElemType::BDEM_VOID == N.dataType());
                ASSERT('\0' == X.selector()[0]);
                ASSERT(RecDef::BDEM_NULL_FIELD_ID == X.selectorId());
                ASSERT(-1 == X.selectorIndex());
                ASSERT(ET::BDEM_VOID == N.dataType());

                Obj mD;  const Obj& D = mD;
                rc = mX.makeSelection(&mD, &err, fldName, Z);
                ASSERT(!rc);
                ASSERT(D.asElemRef() == VA);
                ASSERT(streq(fldName, X.selector()));

                Obj mE; const Obj& E = mE;
                rc = mX.makeSelection(&mE, &err, fldName);
                ASSERT(!rc);
                ASSERT(C.asElemRef() == E.asElemRef());
                ASSERT(streq(fldName, X.selector()));

                Obj mF; const Obj& F = mF;
                rc = mX.makeSelection(&mF, &err, fldName, Z);
                ASSERT(F.asElemRef() == VA);
                ASSERT(streq(fldName, X.selector()));

                destroyAggData(&mX, &t);
                destroyAggData(&mY, &t);
                destroyAggData(&mW, &t);
            }
        }

        if (verbose) cout << "\nTesting choice array item aggregates"
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

                const char *fldName = RECORD->fieldName(0);
                const char *selName = SUB_REC->fieldName(0);
                ET::Type SEL_TYPE   = SUB_REC->field(0).elemType();

                const CERef VA = getCERef(SEL_TYPE, 1);
                const CERef VB = getCERef(SEL_TYPE, 2);

                if (veryVerbose) { T_ P_(SPEC) P(SCHEMA) };

                bslma_TestAllocator t(veryVeryVerbose);

                Obj mW; const Obj& W = mW;
                Obj mX; const Obj& X = mX;
                Obj mY; const Obj& Y = mY;

                ggAggData(&mW, *RECORD, &t);
                ggAggData(&mX, *RECORD, &t);
                ggAggData(&mY, *RECORD, &t);

                if (veryVerbose) { T_ P(X) P(Y) P(W) };

                Error err;
                Obj mA;  const Obj& A = mA;
                Obj mB;  const Obj& B = mB;
                Obj mC;  const Obj& C = mC;
                mW.getField(&mA, &err, false, fldName);
                mX.getField(&mB, &err, false, fldName);
                mY.getField(&mC, &err, false, fldName);

                mA.insertItems(&err, 0, 1);
                mB.insertItems(&err, 0, 1);
                mC.insertItems(&err, 0, 1);

                Obj mD;  const Obj& D = mD;
                Obj mE;  const Obj& E = mE;
                Obj mF;  const Obj& F = mF;
                mA.getField(&mD, &err, true, 0);
                mB.getField(&mE, &err, true, 0);
                mC.getField(&mF, &err, true, 0);

                Obj mG;  const Obj& G = mG;
                Obj mH;  const Obj& H = mH;
                Obj mI;  const Obj& I = mI;
                int rc = mD.makeSelection(&mG, &err, selName);
                ASSERT(!rc);
                rc = mE.makeSelection(&mH, &err, selName, VA);
                ASSERT(!rc);
                rc = mF.makeSelection(&mI, &err, selName, VB);
                ASSERT(!rc);

                Obj mJ;  const Obj& J = mJ;
                Obj mK;  const Obj& K = mK;
                Obj mL;  const Obj& L = mL;
                rc = mD.getField(&mJ, &err, false, selName);
                ASSERT(!rc);
                rc = mE.getField(&mK, &err, false, selName);
                ASSERT(!rc);
                rc = mF.getField(&mL, &err, false, selName);
                ASSERT(!rc);

                if (veryVerbose) { T_ P(X) P(Y) P(W) };
                ASSERT(L.asElemRef() == I.asElemRef());
                ASSERT(L.asElemRef() == VB);
                ASSERT(streq(selName, F.selector()));

                rc = mF.makeSelection(&mL, &err, selName, K);
                ASSERT(!rc);
                ASSERT(L.asElemRef() == VA);
                ASSERT(streq(selName, F.selector()));

                rc = mF.makeSelection(&mL, &err, "");
                ASSERT(!rc);
                ASSERT(bdem_ElemType::BDEM_VOID == L.dataType());
                ASSERT('\0' == F.selector()[0]);
                ASSERT(RecDef::BDEM_NULL_FIELD_ID == F.selectorId());
                ASSERT(-1 == F.selectorIndex());

                rc = mF.makeSelection(&mL, &err, selName);
                ASSERT(!rc);
                ASSERT(streq(selName, F.selector()));

                rc = mF.makeSelection(&mL, &err, selName, K);
                ASSERT(!rc);
                ASSERT(L.asElemRef() == VA);
                ASSERT(streq(selName, F.selector()));

                destroyAggData(&mW, &t);
                destroyAggData(&mX, &t);
                destroyAggData(&mY, &t);
            }
        }

        // Test that error object is returned if data type does not match
        {
            Schema schema; const Schema& SCHEMA = schema;
            ggSchema(&schema, ":aAa");
            const RecDef *RECORD = &SCHEMA.record(0);

            const char *errFld = RECORD->fieldName(0);
            ET::Type    TYPE   = RECORD->field(0).elemType();

            const CERef VA = getCERef(TYPE, 1);

            bslma_TestAllocator t(veryVeryVerbose);
            Error err;
            Obj mX;  const Obj& X = mX;
            ggAggData(&mX, *RECORD, &t);

            Obj mA;  const Obj& A = mA;

            int rc = mX.makeSelection(&mA, &err, errFld);
            ASSERT(rc);

            rc = mX.makeSelection(&mA, &err, errFld, VA);
            ASSERT(rc);

            destroyAggData(&mX, &t);
        }
      } break;
      case 14: {
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
        //   void insertItems(Error *error, int pos, int numItems) const;
        //   void insertNullItems(Error *error, int pos, int numItems) const;
        //   void removeItems(Error *error, int pos, int numItems) const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'insertItems' and 'removeItems'"
                          << " FUNCTION"
                          << "\n======================================="
                          << "========="
                          << bsl::endl;

        if (veryVerbose) { cout << "\n\tTesting scalar arrays and "
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

                Obj mX;  const Obj& X = mX;
                int rc = ggAggData(&mX, *RECORD, &testAllocator);
                ASSERT(!rc);

                Error err;
                Obj mA;  const Obj& A = mA;
                rc = X.getField(&mA, &err, false, fldName);
                ASSERT(!rc);

                rc = mA.insertItems(&err, 0, 0);
                ASSERT(!rc);
                ASSERT(0 == A.length());

                rc = mA.insertNullItems(&err, 0, 0);
                ASSERT(!rc);
                ASSERT(0 == A.length());

                rc = mA.insertItems(&err, 0, 1);
                ASSERT(!rc);
                ASSERT(1 == A.length());

                Obj mB;  const Obj& B = mB;
                rc = A.arrayItem(&mB, &err, 0);
                ASSERT(!rc);
                if (hasDefaultValue) {
                    ASSERT(compareCERefs(RECORD->field(0).defaultValue(),
                                         B.asElemRef()));
                }
                else {
                    ASSERT(isUnset(B.asElemRef()));
                }

                rc = mA.setField(&mB, &err, 0, VA);
                ASSERT(!rc);
                ASSERT(compareCERefs(VA, B.asElemRef()));

                rc = mA.insertNullItems(&err, 0, 1);
                ASSERT(!rc);
                ASSERT(2 == A.length());

                rc = mA.getField(&mB, &err, false, 0);
                ASSERT(!rc);
                LOOP2_ASSERT(LINE, IS_NULL, IS_NULL == B.isNull());

                rc = mA.setField(&mB, &err, 0, VB);
                ASSERT(!rc);
                Obj mC;  const Obj& C = mC;
                rc = mA.setField(&mC, &err, 1, VA);
                ASSERT(!rc);

                ASSERT(compareCERefs(VB, B.asElemRef()));
                ASSERT(compareCERefs(VA, C.asElemRef()));
                mB.makeNull();

                rc = mA.insertItems(&err, 1, 2);
                ASSERT(!rc);
                ASSERT(4 == A.length());
                mA.getField(&mB, &err, false, 1);
                mA.getField(&mC, &err, false, 2);
                if (hasDefaultValue) {
                    ASSERT(compareCERefs(RECORD->field(0).defaultValue(),
                                         B.asElemRef()));
                    ASSERT(compareCERefs(RECORD->field(0).defaultValue(),
                                         C.asElemRef()));
                }
                else {
                    ASSERT(isUnset(B.asElemRef()));
                    ASSERT(isUnset(C.asElemRef()));
                }

                Obj mD;  const Obj& D = mD;
                mA.getField(&mD, &err, false, 3);
                ASSERT(compareCERefs(VA, D.asElemRef()));

                mA.setField(&mB, &err, 1, VA);
                mA.setField(&mB, &err, 2, VB);

                rc = mA.insertNullItems(&err, 1, 2);
                ASSERT(!rc);
                ASSERT(6 == A.length());

                mA.getField(&mB, &err, false, 3);
                mA.getField(&mC, &err, false, 4);
                mA.getField(&mD, &err, false, 5);
                ASSERT(compareCERefs(VA, B.asElemRef()));
                ASSERT(compareCERefs(VB, C.asElemRef()));
                ASSERT(compareCERefs(VA, D.asElemRef()));

                mA.getField(&mB, &err, false, 0);
                mA.getField(&mC, &err, false, 1);
                mA.getField(&mD, &err, false, 2);
                ASSERT(IS_NULL == B.isNull());
                ASSERT(IS_NULL == C.isNull());
                ASSERT(IS_NULL == D.isNull());

                rc = mA.removeItems(&err, 3, 1);
                ASSERT(!rc);
                ASSERT(5 == A.length());
                ASSERT(IS_NULL == B.isNull());
                ASSERT(IS_NULL == C.isNull());
                ASSERT(IS_NULL == D.isNull());

                mA.getField(&mB, &err, false, 3);
                mA.getField(&mC, &err, false, 4);
                ASSERT(compareCERefs(VB, B.asElemRef()));
                ASSERT(compareCERefs(VA, C.asElemRef()));

                mA.setField(&mB, &err, 0, VB);

                rc = mA.removeItems(&err, 1, 4);
                ASSERT(1 == A.length());

                mA.getField(&mB, &err, 0, false);
                ASSERT(compareCERefs(VB, B.asElemRef()));

                rc = mA.removeItems(&err, 0, 0);
                ASSERT(!rc);
                ASSERT(1 == A.length());

                mA.getField(&mB, &err, 0, false);
                ASSERT(compareCERefs(VB, B.asElemRef()));

                mA.removeItems(&err, 0, 1);
                ASSERT(!rc);
                ASSERT(0 == A.length());

                destroyAggData(&mX, &testAllocator);

// #ifndef BSLS_PLATFORM__CMP_MSVC
//               } END_BSLMA_EXCEPTION_TEST
// #endif
          }
        }

#if 0
        if (veryVerbose) { cout << "\n\tTesting unconstrained table "
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
#endif
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'insertItem' and 'arrayItem' FUNCTIONS:
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
        //   int insertItem(Obj *item, Error *error, int index, value) const;
        //   int arrayItem(Obj *item, Error *error, int index) const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'insertItem'"
                          << "\n====================" << bsl::endl;

        if (veryVerbose) { cout << "\n\tTesting scalar arrays and "
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

                Obj mX;  const Obj& X = mX;
                int rc = ggAggData(&mX, *RECORD, &testAllocator);
                ASSERT(!rc);

                Error err;
                Obj mA;  const Obj& A = mA;
                rc = X.getField(&mA, &err, false, fldName);
                ASSERT(!rc);

                Obj mC;  const Obj& C = mC;
                rc = mA.insertItem(&mC, &err, 0, VA);
                ASSERT(!rc);
                LOOP2_ASSERT(A, A.length(), 1 == A.length());
                ASSERT(compareCERefs(VA, C.asElemRef()));

                Obj mD;  const Obj& D = mD;
                rc = mA.insertItem(&mD, &err, 0, VB);
                ASSERT(!rc);
                ASSERT(2 == A.length());
                ASSERT(compareCERefs(VB, D.asElemRef()));

                rc = mA.arrayItem(&mC, &err, 0);
                ASSERT(!rc);
                rc = mA.arrayItem(&mD, &err, 1);
                ASSERT(!rc);
                ASSERT(compareCERefs(VB, C.asElemRef()));
                ASSERT(compareCERefs(VA, D.asElemRef()));

                Obj mE;  const Obj& E = mE;
                rc = mA.insertItem(&mE, &err, 2, VB);
                ASSERT(!rc);
                ASSERT(3 == A.length());
                ASSERT(compareCERefs(VB, E.asElemRef()));

                rc = mA.arrayItem(&mC, &err, 0);
                ASSERT(!rc);
                rc = mA.arrayItem(&mD, &err, 1);
                ASSERT(!rc);
                rc = mA.arrayItem(&mE, &err, 2);
                ASSERT(!rc);
                ASSERT(compareCERefs(VB, C.asElemRef()));
                ASSERT(compareCERefs(VA, D.asElemRef()));
                ASSERT(compareCERefs(VB, E.asElemRef()));

                Obj mF;  const Obj& F = mF;
                rc = mA.insertItem(&mF, &err, 1, VA);
                ASSERT(!rc);
                ASSERT(4 == A.length());
                ASSERT(compareCERefs(VA, F.asElemRef()));

                rc = mA.arrayItem(&mC, &err, 0);
                ASSERT(!rc);
                rc = mA.arrayItem(&mD, &err, 1);
                ASSERT(!rc);
                rc = mA.arrayItem(&mE, &err, 2);
                ASSERT(!rc);
                rc = mA.arrayItem(&mF, &err, 3);
                ASSERT(!rc);
                ASSERT(compareCERefs(VB, C.asElemRef()));
                ASSERT(compareCERefs(VA, D.asElemRef()));
                ASSERT(compareCERefs(VA, E.asElemRef()));
                ASSERT(compareCERefs(VB, F.asElemRef()));

                destroyAggData(&mX, &testAllocator);
            }
        }

        if (veryVerbose) { cout << "\n\tTesting unconstrained table "
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

                ET::Type ARRAY_TYPE = RECORD->field(0).elemType();
                const char *fldName = RECORD->fieldName(0);

                const CERef ARR_VA = getCERef(ARRAY_TYPE, 1);
                const int   LEN    = getLength(ARR_VA);

                const CERef VA = getCERef(ET::fromArrayType(ARRAY_TYPE), 1);
                const CERef VB = getCERef(ET::fromArrayType(ARRAY_TYPE), 2);

                const bool IS_NULL = ET::isChoiceType(ARRAY_TYPE)
                                   ? true : false;
                if (veryVerbose) { T_ P_(SPEC) P(SCHEMA) };

                bslma_TestAllocator testAllocator(veryVeryVerbose);

                Obj mX;  const Obj& X = mX;
                int rc = ggAggData(&mX, *RECORD, &testAllocator);
                ASSERT(!rc);

                Error err;
                Obj mA;  const Obj& A = mA;
                rc = X.setField(&mA, &err, fldName, ARR_VA);
                ASSERT(!rc);

                Obj mC;  const Obj& C = mC;
                rc = mA.insertItem(&mC, &err, 0, VA);
                ASSERT(LEN + 1 == A.length());
                ASSERT(compareCERefs(VA, C.asElemRef()));

                Obj mD;  const Obj& D = mD;
                rc = mA.insertItem(&mD, &err, 0, VB);
                ASSERT(LEN + 2 == A.length());
                ASSERT(compareCERefs(VB, D.asElemRef()));

                rc = mA.arrayItem(&mC, &err, 0);
                ASSERT(!rc);
                rc = mA.arrayItem(&mD, &err, 1);
                ASSERT(!rc);
                ASSERT(compareCERefs(VB, C.asElemRef()));
                ASSERT(compareCERefs(VA, D.asElemRef()));

                Obj mE;  const Obj& E = mE;
                rc = mA.insertItem(&mE, &err, 2, VB);
                ASSERT(LEN + 3 == A.length());
                ASSERT(compareCERefs(VB, E.asElemRef()));

                rc = mA.arrayItem(&mC, &err, 0);
                ASSERT(!rc);
                rc = mA.arrayItem(&mD, &err, 1);
                ASSERT(!rc);
                rc = mA.arrayItem(&mE, &err, 2);
                ASSERT(!rc);
                ASSERT(compareCERefs(VB, C.asElemRef()));
                ASSERT(compareCERefs(VA, D.asElemRef()));
                ASSERT(compareCERefs(VB, E.asElemRef()));

                Obj mF;  const Obj& F = mF;
                rc = mA.insertItem(&mF, &err, 1, VA);
                ASSERT(!rc);
                ASSERT(LEN + 4 == A.length());
                ASSERT(compareCERefs(VA, F.asElemRef()));

                rc = mA.arrayItem(&mC, &err, 0);
                ASSERT(!rc);
                rc = mA.arrayItem(&mD, &err, 1);
                ASSERT(!rc);
                rc = mA.arrayItem(&mE, &err, 2);
                ASSERT(!rc);
                rc = mA.arrayItem(&mF, &err, 3);
                ASSERT(!rc);
                ASSERT(compareCERefs(VB, C.asElemRef()));
                ASSERT(compareCERefs(VA, D.asElemRef()));
                ASSERT(compareCERefs(VA, E.asElemRef()));
                ASSERT(compareCERefs(VB, F.asElemRef()));

                destroyAggData(&mX, &testAllocator);
            }
        }
      } break;
      case 12: {
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

        if (verbose) cout << "\nTESTING 'resize' FUNCTION"
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

            // TBD: Uncomment
//             {   L_,   ":aCbFcGdQf :g#ha" },
//             {   L_,   ":aCbFcGdQf :g#ha&NT" },
//             {   L_,   ":aCbFcGdQf :g#ha&NF" },
//             {   L_,   ":a?CbFcGdQf :g@ha" },
//             {   L_,   ":a?CbFcGdQf :g@ha&NT" },
//             {   L_,   ":a?CbFcGdQf :g@ha&NF" },
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


            const char *fldName         = RECORD->fieldName(0);
            const bool  hasDefaultValue = RECORD->field(0).hasDefaultValue();

            if (veryVerbose) { T_ P_(SPEC) P(SCHEMA) };

            bslma_TestAllocator testAllocator(veryVeryVerbose);

            Obj mX;  const Obj& X = mX;
            int rc = ggAggData(&mX, *RECORD, &testAllocator);
            ASSERT(!rc);

            Error err;
            Obj mA;  const Obj& A = mA;

          BEGIN_BSLMA_EXCEPTION_TEST {
            rc = X.getField(&mA, &err, false, fldName);
            ASSERT(!rc);

            rc = mA.resize(&err, 0);
            ASSERT(!rc);
            ASSERT(0 == A.length());

            rc = mA.resize(&err, 1);
            ASSERT(!rc);
            ASSERT(1 == A.length());
            if (hasDefaultValue) {
                Obj mZ;  const Obj& Z = mZ;
                rc = A.arrayItem(&mZ, &err, 0);
                ASSERT(!rc);
                LOOP_ASSERT(LINE,
                            compareCERefs(Z.asElemRef(),
                                          RECORD->field(0).defaultValue()));
            }

            rc = mA.resize(&err, 1);
            ASSERT(!rc);
            ASSERT(1 == A.length());
            if (hasDefaultValue) {
                Obj mZ;  const Obj& Z = mZ;
                rc = A.arrayItem(&mZ, &err, 0);
                ASSERT(!rc);
                LOOP_ASSERT(LINE,
                            compareCERefs(Z.asElemRef(),
                                          RECORD->field(0).defaultValue()));
            }

            rc = mA.resize(&err, 2);
            ASSERT(!rc);
            ASSERT(2 == A.length());
            if (hasDefaultValue) {
                Obj mB;  const Obj& B = mB;
                Obj mC;  const Obj& C = mC;
                rc = A.arrayItem(&mB, &err, 0);
                ASSERT(!rc);
                rc = A.arrayItem(&mC, &err, 1);
                ASSERT(!rc);
                LOOP_ASSERT(LINE,
                            compareCERefs(B.asElemRef(),
                                          RECORD->field(0).defaultValue()));
                LOOP_ASSERT(LINE,
                            compareCERefs(C.asElemRef(),
                                          RECORD->field(0).defaultValue()));
            }

            rc = mA.resize(&err, 7);
            ASSERT(!rc);
            ASSERT(7 == A.length());

            rc = mA.resize(&err, 3);
            ASSERT(!rc);
            ASSERT(3 == A.length());

            rc = mA.resize(&err, 1);
            ASSERT(!rc);
            ASSERT(1 == A.length());
            if (hasDefaultValue) {
                Obj mZ;  const Obj& Z = mZ;
                rc = A.arrayItem(&mZ, &err, 0);
                ASSERT(!rc);
                LOOP_ASSERT(LINE,
                            compareCERefs(Z.asElemRef(),
                                          RECORD->field(0).defaultValue()));
            }

            rc = mA.resize(&err, 0);
            ASSERT(!rc);
            ASSERT(0 == A.length());

            destroyAggData(&mX, &testAllocator);

          } END_BSLMA_EXCEPTION_TEST
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING TRIVIAL MANIPULATORS AND ACCESSORS:
        //
        // Concerns:
        //   - The trivial manipulators set the appropriate data members.
        //   - The trivial accessors return the appropriate data members.
        //
        // Plan:
        //   - Set the appropriate data members.
        //
        // Testing:
        //   void setDataType(bdem_ElemType::Type dataType);
        //   void setDataPointer(void *data);
        //   void setSchemaPointer(const bdem_Schema *schema);
        //   void setRecordDefPointer(const bdem_RecordDef *recordDef);
        //   void setFieldDefPointer(const bdem_FieldDef *fieldDef);
        //   void setTopLevelAggregateNullnessPointer(int *nullnessFlag);
        //   bdem_ElemType::Type dataType() const;
        //   const bdem_RecordDef& recordDef() const;
        //   const bdem_RecordDef *recordConstraint() const;
        //   const bdem_EnumerationDef *enumerationConstraint() const;
        //   const bdem_FieldDef *fieldDef() const;
        //   const void *data() const;
        //   const bdem_Schema *schema() const;
        // --------------------------------------------------------------------

        if (veryVerbose) { cout << "\tTesting manipulators/accessors"
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
                const FldDef& f = r.field(0);

                const char           *fldName  = r.fieldName(0);
                ET::Type              TYPE     = f.elemType();
                const bool            DEF_VAL  = f.hasDefaultValue();
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

                bslma_TestAllocator t(veryVeryVerbose);
                int n = 0;
                void *d = makeValuePtr(TYPE, &t);

                Obj mX; const Obj& X = mX;
                mX.setDataType(TYPE);
                LOOP_ASSERT(LINE, TYPE == X.dataType());

                mX.setDataPointer(d);
                LOOP_ASSERT(LINE, d    == X.data());

                mX.setSchemaPointer(&s);
                LOOP_ASSERT(LINE, &s   == X.schema());

                mX.setTopLevelAggregateNullnessPointer(&n);

                mX.setRecordDefPointer(&r);
                LOOP_ASSERT(LINE, &r   == X.recordConstraint());

                mX.setFieldDefPointer(&f);
                LOOP_ASSERT(LINE, &f   == X.fieldDef());

                mX.reset();

                destroyValuePtr(d, TYPE, &t);
            }
        }
      } break;
      case 10: {
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

        if (verbose) cout << "\nTESTING ASSIGNMENT OPERATOR"
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

            ET::Type TYPE1 = NSA1
                       ? ET::toArrayType(SCHEMA1.record(1).field(0).elemType())
                       : RECORD1->field(0).elemType();
            const char *fldName1 = RECORD1->fieldName(0);

            const CERef A1 = getCERef(TYPE1, 1);
            const CERef A2 = getCERef(TYPE1, 2);

            if (veryVerbose) { T_ P_(SPEC1) P(SCHEMA1) };

            bslma_TestAllocator t1(veryVeryVerbose);
            Obj mX;  const Obj& X = mX;
            int rc = ggAggData(&mX, *RECORD1, &t1);
            ASSERT(!rc);

            Error err;
            Obj mA;  const Obj& A = mA;
            if (RecDef::BDEM_CHOICE_RECORD == RECORD1->recordType()) {
                int rc = mX.makeSelection(&mA, & err, fldName1);
                ASSERT(!rc);
            }

            rc = mX.setField(&mA, &err,fldName1, A1);
            ASSERT(!rc);

            for (int j = 0; j < NUM_DATA; ++j) {
                const int   LINE2 = DATA[j].d_line;
                const char *SPEC2 = DATA[j].d_spec;
                const bool  NSA2  = (bool) bsl::strstr(SPEC2, "&FN");

                Schema schema2; const Schema& SCHEMA2 = schema2;
                ggSchema(&schema2, SPEC2);
                const RecDef *RECORD2 = NSA2
                                   ? &SCHEMA2.record(0)
                                   : &SCHEMA2.record(SCHEMA2.numRecords() - 1);

                ET::Type TYPE2 = NSA2
                       ? ET::toArrayType(SCHEMA2.record(1).field(0).elemType())
                       : RECORD2->field(0).elemType();
                const char *fldName2 = RECORD2->fieldName(0);

                const CERef B1 = getCERef(TYPE2, 1);
                const CERef B2 = getCERef(TYPE2, 2);

                if (veryVerbose) { T_ P_(SPEC2) P(SCHEMA2) };

                bslma_TestAllocator testAllocator(veryVeryVerbose);
                Obj mY;  const Obj& Y = mY;
                int rc = ggAggData(&mY, *RECORD2, &testAllocator);
                ASSERT(!rc);

                Error err;
                Obj mB;  const Obj& B = mB;
                if (RecDef::BDEM_CHOICE_RECORD == RECORD2->recordType()) {
                    int rc = mY.makeSelection(&mB, &err, fldName2);
                    ASSERT(!rc);
                }

                rc = mY.setField(&mB, &err,fldName2, B1);
                ASSERT(!rc);
                LOOP2_ASSERT(LINE1, LINE2, !Obj::areIdentical(A, B));

                if (veryVerbose) {
                    P(A) P(B)
                }

              BEGIN_BSLMA_EXCEPTION_TEST {
                mB = A;
                LOOP2_ASSERT(LINE1, LINE2, Obj::areIdentical(A, B));

                mB.setValue(&err, A2);
                LOOP2_ASSERT(LINE1, LINE2, Obj::areIdentical(A, B));

                mB.makeNull();
                LOOP2_ASSERT(LINE1, LINE2, Obj::areIdentical(A, B));
              } END_BSLMA_EXCEPTION_TEST

              destroyAggData(&mY, &testAllocator);
            }
            destroyAggData(&mX, &t1);
        }
      } break;
      case 9: {
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
        //   ostream& operator<<(ostream&, const bcem_AggregateRaw&);
        // --------------------------------------------------------------------

        if (verbose) cout << bsl::endl
                          << "\nTESTING OUTPUT FUNCTIONS" << bsl::endl
                          << "\n========================" << bsl::endl;

        if (veryVerbose) cout << bsl::endl
                              << "\n\tTest null aggregate" << bsl::endl;
        {
            Obj mX; const Obj& X = mX;
            bsl::ostringstream os1, os2;
            X.print(os1, 2, 4);
            os2 << X;
            ASSERT("        <null aggregate>\n" == os1.str());
            ASSERT("<null aggregate>" == os2.str());
        }

        if (veryVerbose) cout << bsl::endl
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

            bslma_TestAllocator ta;
            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE = DATA[i].d_line;
                ET::Type  TYPE = DATA[i].d_type;

                const CERef CEA = getCERef(TYPE, 1);
                const CERef CEB = getCERef(TYPE, 2);

                void *data1 = makeValuePtr(TYPE, &ta);
                void *data2 = makeValuePtr(TYPE, &ta);

                CEA.descriptor()->assign(data1, CEA.data());
                CEB.descriptor()->assign(data2, CEB.data());

                bslma_TestAllocator t(veryVeryVerbose);
                int nf1 = 0, nf2 = 0;

                Obj mA; const Obj& A = mA;
                mA.setDataType(TYPE);
                mA.setDataPointer(data1);
                mA.setTopLevelAggregateNullnessPointer(&nf1);

                Obj mB; const Obj& B = mB;
                mB.setDataType(TYPE);
                mB.setDataPointer(data2);
                mB.setTopLevelAggregateNullnessPointer(&nf2);

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

                destroyValuePtr(data1, TYPE, &ta);
                destroyValuePtr(data2, TYPE, &ta);
            }
        }

        if (veryVerbose) cout << bsl::endl
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

                if (veryVerbose) { T_ P(s) };

                bslma_TestAllocator t(veryVeryVerbose);
                Obj mX;  const Obj& X = mX;
                int rc = ggAggData(&mX, r, &t);
                ASSERT(!rc);

                Obj mY;  const Obj& Y = mY;
                rc = ggAggData(&mY, r, &t);
                ASSERT(!rc);

                const char *fldName = r.fieldName(0);
                ET::Type    TYPE    = r.field(0).elemType();
                const CERef CEA     = getCERef(TYPE, 1);
                const CERef CEB     = getCERef(TYPE, 2);

                Error err;
                Obj mA;  const Obj& A = mA;
                Obj mB;  const Obj& B = mB;

                rc = X.setField(&mA, &err, fldName, CEA);
                ASSERT(!rc);
                rc = Y.setField(&mB, &err, fldName, CEB);
                ASSERT(!rc);

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

                destroyAggData(&mX, &t);
                destroyAggData(&mY, &t);
            }
        }

// TBD: Uncomment
#if 0
        if (veryVerbose) cout << bsl::endl
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

                ET::Type TYPE =
                    RecDef::BDEM_SEQUENCE_RECORD == r.recordType()
                    ? ET::BDEM_LIST : ET::BDEM_CHOICE;

                if (veryVerbose) { T_ P(s) };

                if (ET::BDEM_LIST == TYPE) {
                    List list;
                    Table table;
                    ggList(&list, &r);
                    ggTable(&table, &r);

                    int nf1 = 0, nf2 = 0;

                    bslma_TestAllocator t(veryVeryVerbose);
                    List aggList;
                    aggList.appendList(list);
                    Obj mX; const Obj& X = mX;
                    mX.setDataType(ET::BDEM_LIST);
                    mX.setDataPointer(&list);
                    mX.setTopLevelAggregateNullnessPointer(&nf1);
                    mX.setSchemaPointer(&s);

                    Obj mY; const Obj& Y = mY;
                    mY.setDataType(ET::BDEM_TABLE);
                    mY.setDataPointer(&table);
                    mY.setTopLevelAggregateNullnessPointer(&nf2);
                    mX.setSchemaPointer(&s);

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

                    int nf1 = 0, nf2 = 0;

                    bslma_TestAllocator t(veryVeryVerbose);
                    Obj mX; const Obj& X = mX;
                    mX.setDataType(ET::BDEM_CHOICE);
                    mX.setDataPointer(&choice);
                    mX.setTopLevelAggregateNullnessPointer(&nf1);
                    mX.setSchemaPointer(&s);

                    Obj mY; const Obj& Y = mY;
                    mY.setDataType(ET::BDEM_CHOICE_ARRAY);
                    mY.setDataPointer(&choiceArray);
                    mY.setTopLevelAggregateNullnessPointer(&nf2);
                    mX.setSchemaPointer(&s);

                    if (veryVerbose) { T_ P(X) P(Y) };

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
#endif
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING CREATORS
        //
        // Concerns:
        //   - The constructor can be used to create an object containing any
        //         unset value.
        //   - The constructor taking an element type and value can be used to
        //         construct all types of scalar aggregates.
        //   - The constructors that take the meta-data and data information
        //         properly construct the aggregate objects with them.
        //
        // Plan:
        //   - Test creation of an empty aggregate.  Confirm its data type and
        //         meta-data and data information is empty.
        //   - Construct scalar aggregates by calling the constructor taking a
        //     type and value.  Confirm that the data type of the constructed
        //     object is as expected and that it stores the expected value.
        //   - Construct aggregates by specifying various combinations of
        //     meta-data and data values and confirm that in each case the
        //     constructed object stores the expected meta-data and data.
        //   - Confirm that the aggregates are constructed with the correct
        //         allocator
        //   - Confirm that none of the constructors leak any memory, even in
        //     the presence of exceptions.
        //
        // Testing:
        //   bcem_AggregateRaw();
        //   ~bcem_AggregateRaw();
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING CREATORS"
                          << "\n================" << endl;

        {
            bslma_TestAllocator da;
            bslma_DefaultAllocatorGuard allocGuard(&da);

            Obj mX;  const Obj& X = mX;
            ASSERT(ET::BDEM_VOID       == X.dataType());
            ASSERT(0                   == X.data());
            ASSERT(0                   == X.schema());
            ASSERT(0                   == X.recordConstraint());
            ASSERT(0                   == X.fieldDef());
            ASSERT(true                == X.isNull());
        }
      } break;
      case 7: {
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
        //   bool areIdentical(const Raw&, const Raw&);
        //   bool areEquivalent(const Raw&, const Raw&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING IDENTITY AND EQUIVALENCE"
                          << "\n================================"
                          << endl;

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

            bslma_TestAllocator ta(veryVeryVerbose);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE1 = DATA[i].d_line;
                ET::Type  TYPE1 = DATA[i].d_type;

                for (int j = 0; j < NUM_DATA; ++j) {
                    const int LINE2 = DATA[j].d_line;
                    ET::Type  TYPE2 = DATA[j].d_type;

                    const CERef A1  = getCERef(TYPE1, 1);
                    const CERef A2  = getCERef(TYPE1, 2);

                    const CERef B1  = getCERef(TYPE2, 1);
                    const CERef B2  = getCERef(TYPE2, 2);

                    void *data1 = makeValuePtr(TYPE1, &ta);
                    void *data2 = makeValuePtr(TYPE2, &ta);

                    A1.descriptor()->assign(data1, A1.data());
                    B1.descriptor()->assign(data2, B1.data());

                    int nf1 = 0, nf2 = 0;

                    Obj mX; const Obj& X = mX;
                    mX.setDataType(TYPE1);
                    mX.setDataPointer(data1);
                    mX.setTopLevelAggregateNullnessPointer(&nf1);
                    LOOP_ASSERT(TYPE1, !X.isNull());
                    LOOP_ASSERT(TYPE1, TYPE1 == X.dataType());

                    Obj mY; const Obj& Y = mY;
                    mY.setDataType(TYPE2);
                    mY.setDataPointer(data2);
                    mY.setTopLevelAggregateNullnessPointer(&nf2);
                    LOOP_ASSERT(TYPE2, !Y.isNull());
                    LOOP_ASSERT(TYPE2, TYPE2 == Y.dataType());

                    if (veryVerbose) { P_(i) P(TYPE1) P(X) P(Y) }

                    Obj mZ(X); const Obj& Z = mZ;

                    LOOP3_ASSERT(LINE1, X, Z, Obj::areIdentical(X, Z));

                    if (i == j) {
                        LOOP2_ASSERT(X, Y, ! Obj::areIdentical(X, Y));
                        LOOP2_ASSERT(X, Y,   Obj::areEquivalent(X, Y));

                        A1.descriptor()->assign(data1, A2.data());

                        LOOP2_ASSERT(X, Z,   Obj::areIdentical(X, Z));
                        LOOP2_ASSERT(X, Y, ! Obj::areIdentical(X, Y));
                        LOOP2_ASSERT(X, Y, ! Obj::areEquivalent(X, Y));

                        A1.descriptor()->assign(data2, A2.data());

                        LOOP2_ASSERT(X, Y, ! Obj::areIdentical(X, Y));
                        LOOP2_ASSERT(X, Y,   Obj::areEquivalent(X, Y));

                        mX.makeNull();

                        LOOP2_ASSERT(X, Z,   Obj::areIdentical(X, Z));
                        LOOP2_ASSERT(X, Y, ! Obj::areIdentical(X, Y));
                        LOOP2_ASSERT(X, Y, ! Obj::areEquivalent(X, Y));

                        mY.makeNull();

                        LOOP2_ASSERT(X, Y, ! Obj::areIdentical(X, Y));
                        LOOP2_ASSERT(X, Y,   Obj::areEquivalent(X, Y));
                    }
                    else {
                        LOOP2_ASSERT(X, Y, ! Obj::areIdentical(X, Y));
                        LOOP2_ASSERT(X, Y, ! Obj::areEquivalent(X, Y));
                    }

                    destroyValuePtr(data1, TYPE1, &ta);
                    destroyValuePtr(data2, TYPE2, &ta);
                }
            }
        }

        if (veryVerbose) { cout << "\tTesting array and aggregate types"
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

                if (veryVerbose) { T_ P(s) };

                // Create three objects:
                //   X and Y are identical
                //   Z is equivalent but not identical to X and Y

                bslma_TestAllocator t(veryVeryVerbose);
                Obj mX;  const Obj& X = mX;
                int rc = ggAggData(&mX, r, &t);
                ASSERT(!rc);

                Obj mY(X);  const Obj& Y = mY;

                Obj mZ;  const Obj& Z = mZ;
                rc = ggAggData(&mZ, r, &t);
                ASSERT(!rc);

                LOOP2_ASSERT(X, Y,   Obj::areIdentical(X, Y));
                LOOP2_ASSERT(X, Y,   Obj::areEquivalent(X, Y));
                LOOP2_ASSERT(X, Z, ! Obj::areIdentical(X, Z));
                LOOP2_ASSERT(X, Z,   Obj::areEquivalent(X, Z));
                LOOP2_ASSERT(Z, Y, ! Obj::areIdentical(Y, Z));
                LOOP2_ASSERT(Z, Y,   Obj::areEquivalent(Y, Z));

                if (veryVerbose) { T_ P(X) P(Z) };

                const char *fldName = r.fieldName(0);
                ET::Type    fldType = NSA
                             ? ET::toArrayType(s.record(1).field(0).elemType())
                             : r.field(0).elemType();

                const CERef A0 = getCERef(fldType, 0);
                const CERef A1 = getCERef(fldType, 1);
                const CERef A2 = getCERef(fldType, 2);

                // Same field from identical objects are identical.
                // Same field from equivalent objects are equivalent.

                Error err;
                Obj mA; const Obj& A = mA;
                Obj mB; const Obj& B = mB;
                Obj mC; const Obj& C = mC;
                Obj mD; const Obj& D = mD;

                rc = mX.getField(&mA, &err, false, fldName);
                ASSERT(!rc);
                rc = mX.getField(&mB, &err, false, fldName);
                ASSERT(!rc);
                rc = mY.getField(&mC, &err, false, fldName);
                ASSERT(!rc);
                rc = mZ.getField(&mD, &err, false, fldName);
                ASSERT(!rc);

                LOOP2_ASSERT(A, B,   Obj::areIdentical(A, B));
                LOOP2_ASSERT(A, B,   Obj::areEquivalent(A, B));
                LOOP2_ASSERT(A, C,   Obj::areIdentical(A, C));
                LOOP2_ASSERT(A, C,   Obj::areEquivalent(A, C));
                LOOP2_ASSERT(A, D, ! Obj::areIdentical(A, D));
                LOOP2_ASSERT(A, D,   Obj::areEquivalent(A, D));
                LOOP2_ASSERT(C, D, ! Obj::areIdentical(C, D));
                LOOP2_ASSERT(C, D,   Obj::areEquivalent(C, D));

                // Modifying field in identical objects preserves identity.
                // Modifying field in equivalent objects does not preserve
                // equivalence.

                Obj mE; const Obj& E = mE;

                rc = X.setField(&mE, &err, fldName, A1);
                ASSERT(!rc);

                LOOP2_ASSERT(X, Y,   Obj::areIdentical(X, Y));
                LOOP2_ASSERT(X, Y,   Obj::areEquivalent(X, Y));
                LOOP2_ASSERT(X, Z, ! Obj::areEquivalent(X, Z));
                LOOP2_ASSERT(X, Z, ! Obj::areIdentical(X, Z));
                LOOP2_ASSERT(Z, Y, ! Obj::areEquivalent(Y, Z));
                LOOP2_ASSERT(Z, Y, ! Obj::areIdentical(Y, Z));

                rc = Y.setField(&mE, &err, fldName, A2);
                LOOP2_ASSERT(X, Y, Obj::areIdentical(X, Y));
                LOOP2_ASSERT(X, Y, Obj::areEquivalent(X, Y));
                if (A2 != A0) {
                    LOOP2_ASSERT(X, Z, ! Obj::areEquivalent(X, Z));
                    LOOP2_ASSERT(X, Z, ! Obj::areIdentical(X, Z));
                    LOOP2_ASSERT(Z, Y, ! Obj::areEquivalent(Y, Z));
                    LOOP2_ASSERT(Z, Y, ! Obj::areIdentical(Y, Z));
               }

                // Restore equivalence
                rc = Z.setField(&mE, &err, fldName, A2);
                LOOP2_ASSERT(X, Y,   Obj::areIdentical(X, Y));
                LOOP2_ASSERT(X, Y,   Obj::areEquivalent(X, Y));
                LOOP2_ASSERT(X, Z,   Obj::areEquivalent(X, Z));
                LOOP2_ASSERT(X, Z, ! Obj::areIdentical(X, Z));
                LOOP2_ASSERT(Z, Y,   Obj::areEquivalent(Y, Z));
                LOOP2_ASSERT(Z, Y, ! Obj::areIdentical(Y, Z));

                destroyAggData(&mX, &t);
                destroyAggData(&mZ, &t);
            }
        }

        if (veryVerbose) { cout << "\tChecking equality of a nillable "
                                << "scalar array and a non-nillable scalar"
                                << " array" << endl; }
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

                ET::Type    SUB_TYPE   = SCHEMA.record(2).field(0).elemType();
                ET::Type    ARRAY_TYPE = EType::toArrayType(SUB_TYPE);
                const char *name1 = R1->fieldName(0);
                const char *name2 = R2->fieldName(0);

                const CERef ARRAY_CEA = getCERef(ARRAY_TYPE, 1);

                if (veryVerbose) { T_ P(SPEC) P(SCHEMA) P(ARRAY_CEA) };

                bslma_TestAllocator t(veryVeryVerbose);

                Obj mX;  const Obj& X = mX;
                Obj mY;  const Obj& Y = mY;

                int rc = ggAggData(&mX, *R1, &t);
                ASSERT(!rc);
                rc = ggAggData(&mY, *R2, &t);
                ASSERT(!rc);

                Error err;
                Obj mA;  const Obj& A = mA;
                Obj mB;  const Obj& B = mB;

                rc = mX.setField(&mA, &err, name1, ARRAY_CEA);
                ASSERT(!rc);
                rc = mY.setField(&mB, &err, name2, ARRAY_CEA);
                ASSERT(!rc);

                if (veryVerbose) { T_ P(X) P(Y) };

                ASSERT(! Obj::areIdentical(X, Y));
                ASSERT(! Obj::areEquivalent(X, Y));

                ASSERT(! Obj::areIdentical(A, B));
                ASSERT(! Obj::areEquivalent(A, B));

                Obj mC;  const Obj& C = mC;
                Obj mD;  const Obj& D = mD;
                rc = A.getField(&mC, &err, false, 0);
                ASSERT(!rc);
                rc = B.getField(&mD, &err, false, 0);
                ASSERT(!rc);
                ASSERT(! Obj::areIdentical(A, B));
                ASSERT(! Obj::areEquivalent(A, B));

                destroyAggData(&mX, &t);
                destroyAggData(&mY, &t);
            }
        }
      } break;
      case 6: {
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
        //   void loadAsString(bsl::string  *string) const;
        //   bdem_ConstElemRef asElemRef() const;
        //   ET::Type dataType() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'asElemRef' and related ACCESSORS"
                          << "\n========================================="
                          << bsl::endl;

        if (veryVerbose) { T_ cout << "Testing for empty aggregates" << endl; }
        {
            Obj mX; const Obj& X = mX;
            ASSERT(X.isNull());

            ASSERT(ET::BDEM_VOID == X.dataType());
            ASSERT(bsl::string() == X.asString());
            string s;
            X.loadAsString(&s);
            ASSERT(bsl::string() == s);
        }

        if (veryVerbose) { T_ cout << "Testing for scalar aggregates"
                                   << endl; }
        {
            TestAllocator ta;
            for (int i = 0; i < ET::BDEM_NUM_TYPES; ++i) {

                ET::Type    TYPE = (ET::Type) i;
                const CERef CER  = getCERef(TYPE, 1);

                void *data = makeValuePtr(TYPE, &ta);
                CER.descriptor()->assign(data, CER.data());

                int nf = 0;
                Obj mX; const Obj& X = mX;
                mX.setDataType(TYPE);
                mX.setDataPointer(data);
                mX.setTopLevelAggregateNullnessPointer(&nf);
                LOOP_ASSERT(TYPE, !X.isNull());
                LOOP_ASSERT(TYPE, TYPE == X.dataType());

                if (veryVerbose) { P_(i) P(TYPE) P(X) }

                const bdem_ElemRef ER = X.asElemRef();
                LOOP_ASSERT(TYPE, CER == ER);

                X.makeNull();
                LOOP_ASSERT(TYPE, X.isNull());
                LOOP_ASSERT(TYPE, ER.isNull());
                LOOP_ASSERT(TYPE, isUnset(ER));

                destroyValuePtr(data, TYPE, &ta);
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

                if (veryVerbose) { T_ P(s) };

                bslma_TestAllocator t(veryVeryVerbose);
                Obj mX; const Obj& X = mX;
                int rc = ggAggData(&mX, *r, &t);
                ASSERT(!rc);

                if (veryVerbose) { T_ P(X) };


                const char *fldName = r->fieldName(0);
                ET::Type    TYPE    = NSA
                             ? ET::toArrayType(s.record(1).field(0).elemType())
                             : r->field(0).elemType();

                const CERef CER = getCERef(TYPE, 1);

                if (veryVerbose) { T_ T_ P(s) P(TYPE) P(CER) P(X) }

                Obj   mY; const Obj& Y = mY;
                Error err;
                if (RecDef::BDEM_CHOICE_RECORD == r->recordType()) {
                    rc = mX.makeSelection(&mY, &err, fldName);
                    ASSERT(!rc);
                }

                LOOP_ASSERT(LINE, !X.isNull());
                const bdem_ElemRef ERX = X.asElemRef();
                LOOP_ASSERT(LINE, !ERX.isNull());

                mY.reset();
                rc = X.getField(&mY, &err, false, fldName);
                ASSERT(!rc);

                mY.reset();
                rc = X.setField(&mY, &err, fldName, CER);
                ASSERT(!rc);

                const bdem_ElemRef ERY = Y.asElemRef();

                LOOP_ASSERT(LINE, !X.isNull());
                LOOP_ASSERT(LINE, !Y.isNull());
                LOOP_ASSERT(LINE, !ERX.isNull());
                LOOP_ASSERT(LINE, !ERY.isNull());
                if (NSA) {
                    ASSERT(ET::BDEM_TABLE == ERY.type());
                    ASSERT(compareNillableTable(Y, CER));
                }
                else {
                    LOOP3_ASSERT(LINE, CER, ERY, CER == ERY);
                }

                mY.makeNull();
                LOOP_ASSERT(LINE, Y.isNull());
                LOOP_ASSERT(LINE, isUnset(ERY));

                destroyAggData(&mX, &t);
            }
        }
      } break;
      case 5: {
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
        //   template <typename TOTYPE>
        //   TOTYPE convertScalar() const
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'asXXX' ACCESSORS"
                          << "\n=========================" << endl;

        bslma_TestAllocator ta;

        if (veryVerbose) { T_ cout << "Testing for BOOL" << endl; }
        {
            typedef bool TYPE;

            ET::Type type = ET::BDEM_BOOL;

            const CERef VN = getCERef(type, 0);
            const CERef VA = getCERef(type, 1);
            const CERef VB = getCERef(type, 2);

            void *data = makeValuePtr(type, &ta);
            {
                VA.descriptor()->assign(data, VA.data());

                Obj mX; const Obj& X = mX;
                mX.setDataType(type);
                mX.setDataPointer(data);
                int nf1 = 0;
                mX.setTopLevelAggregateNullnessPointer(&nf1);

                ASSERT(VA.theBool() == X.asBool());
                ASSERT(VA           == X.asElemRef());
                ASSERT(!X.isNull());

                X.makeNull();
                ASSERT(X.isNull());
                ASSERT(X.asElemRef().isNull());
                ASSERT(VN.theBool() == X.asBool());

                VA.descriptor()->assign(data, VB.data());

                Obj mY; const Obj& Y = mY;
                mY.setDataType(type);
                mY.setDataPointer(data);
                int nf2 = 0;
                mY.setTopLevelAggregateNullnessPointer(&nf2);

                ASSERT(VB.theBool() == Y.asBool());
                ASSERT(VB           == Y.asElemRef());
                ASSERT(!Y.isNull());

                Y.makeNull();
                ASSERT(Y.isNull());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(VN.theBool() == Y.asBool());
            }
            destroyValuePtr(data, type, &ta);

            // nillable bool array
            {
                const char *SPEC = ":aaa&FN";
                Schema schema; const Schema& SCHEMA = schema;
                ggSchema(&schema, SPEC);

                const RecDef *RECORD  = &SCHEMA.record(0);
                const char   *fldName = RECORD->fieldName(0);

                bslma_TestAllocator t;

                Obj mA; const Obj& A = mA;
                int rc = ggAggData(&mA, *RECORD, &t);
                ASSERT(!rc);

                Error err;
                Obj mX; const Obj& X = mX;
                rc = mA.getField(&mX, &err, false, fldName);
                ASSERT(!rc);

                const int NE = 3;
                rc = mX.resize(&err, NE);
                ASSERT(!rc);

                Obj mY; const Obj& Y = mY;
                for (int i = 0; i < NE; ++i) {
                    rc = X.getField(&mY, &err, false, i);
                    ASSERT(!rc);

                    LOOP_ASSERT(i, Y.isNull());
                    LOOP_ASSERT(i, VN.theBool() == Y.asBool());
                    LOOP_ASSERT(i, VN.theBool() == Y.asElemRef().theBool());
                    LOOP3_ASSERT(i, Y, Y.asElemRef(), Y.asElemRef().isNull());
                    LOOP3_ASSERT(i, Y, Y.asElemRef(), isUnset(Y.asElemRef()));
                }

                rc = mX.setField(&mY, &err, 0, VA);
                ASSERT(!rc);
                ASSERT(VA.theBool() == Y.asBool());
                ASSERT(VA           == Y.asElemRef());
                ASSERT(!Y.isNull());

                mY.makeNull();
                ASSERT(VN.theBool() == Y.asBool());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(Y.isNull());

                mY.reset();
                rc = mA.setField(&mY, &err, fldName, 1, VB);
                ASSERT(!rc);
                ASSERT(VB.theBool() == Y.asBool());
                LOOP2_ASSERT(VB, Y.asElemRef(), VB == Y.asElemRef());
                ASSERT(!Y.isNull());

                mY.makeNull();
                ASSERT(VN.theBool() == Y.asBool());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(Y.isNull());

                mY.reset();
                rc = mX.getField(&mY, &err, false, 2);
                ASSERT(!rc);
                rc = mY.setValue(&err, VA);
                ASSERT(!rc);
                ASSERT(VA.theBool() == Y.asBool());
                ASSERT(VA           == Y.asElemRef());
                ASSERT(!Y.isNull());

                mY.makeNull();
                ASSERT(VN.theBool() == Y.asBool());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(Y.isNull());

                destroyAggData(&mA, &t);
            }
        }

        if (veryVerbose) { T_ cout << "Testing for CHAR" << endl; }
        {
            typedef char TYPE;

            ET::Type type = ET::BDEM_CHAR;

            const CERef VN = getCERef(type, 0);
            const CERef VA = getCERef(type, 1);
            const CERef VB = getCERef(type, 2);

            void *data = makeValuePtr(type, &ta);
            {
                VA.descriptor()->assign(data, VA.data());

                Obj mX; const Obj& X = mX;
                mX.setDataType(type);
                mX.setDataPointer(data);
                int nf1 = 0;
                mX.setTopLevelAggregateNullnessPointer(&nf1);

                ASSERT(VA.theChar() == X.asChar());
                ASSERT(VA           == X.asElemRef());
                ASSERT(!X.isNull());

                X.makeNull();
                ASSERT(X.isNull());
                ASSERT(X.asElemRef().isNull());
                ASSERT(VN.theChar() == X.asChar());

                VA.descriptor()->assign(data, VB.data());

                Obj mY; const Obj& Y = mY;
                mY.setDataType(type);
                mY.setDataPointer(data);
                int nf2 = 0;
                mY.setTopLevelAggregateNullnessPointer(&nf2);

                ASSERT(VB.theChar() == Y.asChar());
                ASSERT(VB           == Y.asElemRef());
                ASSERT(!Y.isNull());

                Y.makeNull();
                ASSERT(Y.isNull());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(VN.theChar() == Y.asChar());
            }

            destroyValuePtr(data, type, &ta);

            // nillable char array
            {
                const char *SPEC = ":aKa&FN";
                Schema schema; const Schema& SCHEMA = schema;
                ggSchema(&schema, SPEC);

                const RecDef *RECORD  = &SCHEMA.record(0);
                const char   *fldName = RECORD->fieldName(0);

                bslma_TestAllocator t;

                Obj mA; const Obj& A = mA;
                int rc = ggAggData(&mA, *RECORD, &t);
                ASSERT(!rc);

                Error err;
                Obj mX; const Obj& X = mX;
                rc = mA.getField(&mX, &err, false, fldName);
                ASSERT(!rc);

                const int NE = 3;
                rc = mX.resize(&err, NE);
                ASSERT(!rc);

                Obj mY; const Obj& Y = mY;
                for (int i = 0; i < NE; ++i) {
                    rc = X.getField(&mY, &err, false, i);
                    ASSERT(!rc);

                    LOOP_ASSERT(i, Y.isNull());
                    LOOP_ASSERT(i, VN.theChar() == Y.asChar());
                    LOOP_ASSERT(i, VN.theChar() == Y.asElemRef().theChar());
                    LOOP3_ASSERT(i, Y, Y.asElemRef(), Y.asElemRef().isNull());
                    LOOP3_ASSERT(i, Y, Y.asElemRef(), isUnset(Y.asElemRef()));
                }

                rc = mX.setField(&mY, &err, 0, VA);
                ASSERT(!rc);
                ASSERT(VA.theChar() == Y.asChar());
                ASSERT(VA           == Y.asElemRef());
                ASSERT(!Y.isNull());

                mY.makeNull();
                ASSERT(VN.theChar() == Y.asChar());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(Y.isNull());

                mY.reset();
                rc = mA.setField(&mY, &err, fldName, 1, VB);
                ASSERT(!rc);
                ASSERT(VB.theChar() == Y.asChar());
                LOOP2_ASSERT(VB, Y.asElemRef(), VB == Y.asElemRef());
                ASSERT(!Y.isNull());

                mY.makeNull();
                ASSERT(VN.theChar() == Y.asChar());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(Y.isNull());

                mY.reset();
                rc = mX.getField(&mY, &err, false, 2);
                ASSERT(!rc);
                rc = mY.setValue(&err, VA);
                ASSERT(!rc);
                ASSERT(VA.theChar() == Y.asChar());
                ASSERT(VA           == Y.asElemRef());
                ASSERT(!Y.isNull());

                mY.makeNull();
                ASSERT(VN.theChar() == Y.asChar());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(Y.isNull());

                destroyAggData(&mA, &t);
            }
        }

        if (veryVerbose) { T_ cout << "Testing for SHORT" << endl; }
        {
            typedef short TYPE;

            ET::Type type = ET::BDEM_SHORT;

            const CERef VN = getCERef(type, 0);
            const CERef VA = getCERef(type, 1);
            const CERef VB = getCERef(type, 2);

            void *data = makeValuePtr(type, &ta);
            {
                VA.descriptor()->assign(data, VA.data());

                Obj mX; const Obj& X = mX;
                mX.setDataType(type);
                mX.setDataPointer(data);
                int nf1 = 0;
                mX.setTopLevelAggregateNullnessPointer(&nf1);

                ASSERT(VA.theShort() == X.asShort());
                ASSERT(VA           == X.asElemRef());
                ASSERT(!X.isNull());

                X.makeNull();
                ASSERT(X.isNull());
                ASSERT(X.asElemRef().isNull());
                ASSERT(VN.theShort() == X.asShort());

                VA.descriptor()->assign(data, VB.data());

                Obj mY; const Obj& Y = mY;
                mY.setDataType(type);
                mY.setDataPointer(data);
                int nf2 = 0;
                mY.setTopLevelAggregateNullnessPointer(&nf2);

                ASSERT(VB.theShort() == Y.asShort());
                ASSERT(VB           == Y.asElemRef());
                ASSERT(!Y.isNull());

                Y.makeNull();
                ASSERT(Y.isNull());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(VN.theShort() == Y.asShort());
            }

            destroyValuePtr(data, type, &ta);

            // nillable short array
            {
                const char *SPEC = ":aLa&FN";
                Schema schema; const Schema& SCHEMA = schema;
                ggSchema(&schema, SPEC);

                const RecDef *RECORD  = &SCHEMA.record(0);
                const char   *fldName = RECORD->fieldName(0);

                bslma_TestAllocator t;

                Obj mA; const Obj& A = mA;
                int rc = ggAggData(&mA, *RECORD, &t);
                ASSERT(!rc);

                Error err;
                Obj mX; const Obj& X = mX;
                rc = mA.getField(&mX, &err, false, fldName);
                ASSERT(!rc);

                const int NE = 3;
                rc = mX.resize(&err, NE);
                ASSERT(!rc);

                Obj mY; const Obj& Y = mY;
                for (int i = 0; i < NE; ++i) {
                    rc = X.getField(&mY, &err, false, i);
                    ASSERT(!rc);

                    LOOP_ASSERT(i, Y.isNull());
                    LOOP_ASSERT(i, VN.theShort() == Y.asShort());
                    LOOP_ASSERT(i, VN.theShort() == Y.asElemRef().theShort());
                    LOOP3_ASSERT(i, Y, Y.asElemRef(), Y.asElemRef().isNull());
                    LOOP3_ASSERT(i, Y, Y.asElemRef(), isUnset(Y.asElemRef()));
                }

                rc = mX.setField(&mY, &err, 0, VA);
                ASSERT(!rc);
                ASSERT(VA.theShort() == Y.asShort());
                ASSERT(VA           == Y.asElemRef());
                ASSERT(!Y.isNull());

                mY.makeNull();
                ASSERT(VN.theShort() == Y.asShort());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(Y.isNull());

                mY.reset();
                rc = mA.setField(&mY, &err, fldName, 1, VB);
                ASSERT(!rc);
                ASSERT(VB.theShort() == Y.asShort());
                LOOP2_ASSERT(VB, Y.asElemRef(), VB == Y.asElemRef());
                ASSERT(!Y.isNull());

                mY.makeNull();
                ASSERT(VN.theShort() == Y.asShort());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(Y.isNull());

                mY.reset();
                rc = mX.getField(&mY, &err, false, 2);
                ASSERT(!rc);
                rc = mY.setValue(&err, VA);
                ASSERT(!rc);
                ASSERT(VA.theShort() == Y.asShort());
                ASSERT(VA           == Y.asElemRef());
                ASSERT(!Y.isNull());

                mY.makeNull();
                ASSERT(VN.theShort() == Y.asShort());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(Y.isNull());

                destroyAggData(&mA, &t);
            }
        }

        if (veryVerbose) { T_ cout << "Testing for INT" << endl; }
        {
            typedef int TYPE;

            ET::Type type = ET::BDEM_INT;

            const CERef VN = getCERef(type, 0);
            const CERef VA = getCERef(type, 1);
            const CERef VB = getCERef(type, 2);

            void *data = makeValuePtr(type, &ta);
            {
                VA.descriptor()->assign(data, VA.data());

                Obj mX; const Obj& X = mX;
                mX.setDataType(type);
                mX.setDataPointer(data);
                int nf1 = 0;
                mX.setTopLevelAggregateNullnessPointer(&nf1);

                ASSERT(VA.theInt() == X.asInt());
                ASSERT(VA          == X.asElemRef());
                ASSERT(!X.isNull());

                X.makeNull();
                ASSERT(X.isNull());
                ASSERT(X.asElemRef().isNull());
                ASSERT(VN.theInt() == X.asInt());

                VA.descriptor()->assign(data, VB.data());

                Obj mY; const Obj& Y = mY;
                mY.setDataType(type);
                mY.setDataPointer(data);
                int nf2 = 0;
                mY.setTopLevelAggregateNullnessPointer(&nf2);

                ASSERT(VB.theInt() == Y.asInt());
                ASSERT(VB          == Y.asElemRef());
                ASSERT(!Y.isNull());

                Y.makeNull();
                ASSERT(Y.isNull());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(VN.theInt() == Y.asInt());
            }

            destroyValuePtr(data, type, &ta);

            // nillable int array
            {
                const char *SPEC = ":aMa&FN";
                Schema schema; const Schema& SCHEMA = schema;
                ggSchema(&schema, SPEC);

                const RecDef *RECORD  = &SCHEMA.record(0);
                const char   *fldName = RECORD->fieldName(0);

                bslma_TestAllocator t;

                Obj mA; const Obj& A = mA;
                int rc = ggAggData(&mA, *RECORD, &t);
                ASSERT(!rc);

                Error err;
                Obj mX; const Obj& X = mX;
                rc = mA.getField(&mX, &err, false, fldName);
                ASSERT(!rc);

                const int NE = 3;
                rc = mX.resize(&err, NE);
                ASSERT(!rc);

                Obj mY; const Obj& Y = mY;
                for (int i = 0; i < NE; ++i) {
                    rc = X.getField(&mY, &err, false, i);
                    ASSERT(!rc);

                    LOOP_ASSERT(i, Y.isNull());
                    LOOP_ASSERT(i, VN.theInt() == Y.asInt());
                    LOOP_ASSERT(i, VN.theInt() == Y.asElemRef().theInt());
                    LOOP3_ASSERT(i, Y, Y.asElemRef(), Y.asElemRef().isNull());
                    LOOP3_ASSERT(i, Y, Y.asElemRef(), isUnset(Y.asElemRef()));
                }

                rc = mX.setField(&mY, &err, 0, VA);
                ASSERT(!rc);
                ASSERT(VA.theInt() == Y.asInt());
                ASSERT(VA          == Y.asElemRef());
                ASSERT(!Y.isNull());

                mY.makeNull();
                ASSERT(VN.theInt() == Y.asInt());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(Y.isNull());

                mY.reset();
                rc = mA.setField(&mY, &err, fldName, 1, VB);
                ASSERT(!rc);
                ASSERT(VB.theInt() == Y.asInt());
                LOOP2_ASSERT(VB, Y.asElemRef(), VB == Y.asElemRef());
                ASSERT(!Y.isNull());

                mY.makeNull();
                ASSERT(VN.theInt() == Y.asInt());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(Y.isNull());

                mY.reset();
                rc = mX.getField(&mY, &err, false, 2);
                ASSERT(!rc);
                rc = mY.setValue(&err, VA);
                ASSERT(!rc);
                ASSERT(VA.theInt() == Y.asInt());
                ASSERT(VA          == Y.asElemRef());
                ASSERT(!Y.isNull());

                mY.makeNull();
                ASSERT(VN.theInt() == Y.asInt());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(Y.isNull());

                destroyAggData(&mA, &t);
            }
        }

        if (veryVerbose) { T_ cout << "Testing for INT64" << endl; }
        {
            typedef Int64 TYPE;

            ET::Type type = ET::BDEM_INT64;

            const CERef VN = getCERef(type, 0);
            const CERef VA = getCERef(type, 1);
            const CERef VB = getCERef(type, 2);

            void *data = makeValuePtr(type, &ta);
            {
                VA.descriptor()->assign(data, VA.data());

                Obj mX; const Obj& X = mX;
                mX.setDataType(type);
                mX.setDataPointer(data);
                int nf1 = 0;
                mX.setTopLevelAggregateNullnessPointer(&nf1);

                ASSERT(VA.theInt64() == X.asInt64());
                ASSERT(VA          == X.asElemRef());
                ASSERT(!X.isNull());

                X.makeNull();
                ASSERT(X.isNull());
                ASSERT(X.asElemRef().isNull());
                ASSERT(VN.theInt64() == X.asInt64());

                VA.descriptor()->assign(data, VB.data());

                Obj mY; const Obj& Y = mY;
                mY.setDataType(type);
                mY.setDataPointer(data);
                int nf2 = 0;
                mY.setTopLevelAggregateNullnessPointer(&nf2);

                ASSERT(VB.theInt64() == Y.asInt64());
                ASSERT(VB          == Y.asElemRef());
                ASSERT(!Y.isNull());

                Y.makeNull();
                ASSERT(Y.isNull());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(VN.theInt64() == Y.asInt64());
            }

            destroyValuePtr(data, type, &ta);

            // nillable int64 array
            {
                const char *SPEC = ":aNa&FN";
                Schema schema; const Schema& SCHEMA = schema;
                ggSchema(&schema, SPEC);

                const RecDef *RECORD  = &SCHEMA.record(0);
                const char   *fldName = RECORD->fieldName(0);

                bslma_TestAllocator t;

                Obj mA; const Obj& A = mA;
                int rc = ggAggData(&mA, *RECORD, &t);
                ASSERT(!rc);

                Error err;
                Obj mX; const Obj& X = mX;
                rc = mA.getField(&mX, &err, false, fldName);
                ASSERT(!rc);

                const int NE = 3;
                rc = mX.resize(&err, NE);
                ASSERT(!rc);

                Obj mY; const Obj& Y = mY;
                for (int i = 0; i < NE; ++i) {
                    rc = X.getField(&mY, &err, false, i);
                    ASSERT(!rc);

                    LOOP_ASSERT(i, Y.isNull());
                    LOOP_ASSERT(i, VN.theInt64() == Y.asInt64());
                    LOOP_ASSERT(i, VN.theInt64() == Y.asElemRef().theInt64());
                    LOOP3_ASSERT(i, Y, Y.asElemRef(), Y.asElemRef().isNull());
                    LOOP3_ASSERT(i, Y, Y.asElemRef(), isUnset(Y.asElemRef()));
                }

                rc = mX.setField(&mY, &err, 0, VA);
                ASSERT(!rc);
                ASSERT(VA.theInt64() == Y.asInt64());
                ASSERT(VA          == Y.asElemRef());
                ASSERT(!Y.isNull());

                mY.makeNull();
                ASSERT(VN.theInt64() == Y.asInt64());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(Y.isNull());

                mY.reset();
                rc = mA.setField(&mY, &err, fldName, 1, VB);
                ASSERT(!rc);
                ASSERT(VB.theInt64() == Y.asInt64());
                LOOP2_ASSERT(VB, Y.asElemRef(), VB == Y.asElemRef());
                ASSERT(!Y.isNull());

                mY.makeNull();
                ASSERT(VN.theInt64() == Y.asInt64());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(Y.isNull());

                mY.reset();
                rc = mX.getField(&mY, &err, false, 2);
                ASSERT(!rc);
                rc = mY.setValue(&err, VA);
                ASSERT(!rc);
                ASSERT(VA.theInt64() == Y.asInt64());
                ASSERT(VA          == Y.asElemRef());
                ASSERT(!Y.isNull());

                mY.makeNull();
                ASSERT(VN.theInt64() == Y.asInt64());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(Y.isNull());

                destroyAggData(&mA, &t);
            }
        }

        if (veryVerbose) { T_ cout << "Testing for FLOAT" << endl; }
        {
            typedef float TYPE;

            ET::Type type = ET::BDEM_FLOAT;

            const CERef VN = getCERef(type, 0);
            const CERef VA = getCERef(type, 1);
            const CERef VB = getCERef(type, 2);

            void *data = makeValuePtr(type, &ta);
            {
                VA.descriptor()->assign(data, VA.data());

                Obj mX; const Obj& X = mX;
                mX.setDataType(type);
                mX.setDataPointer(data);
                int nf1 = 0;
                mX.setTopLevelAggregateNullnessPointer(&nf1);

                ASSERT(VA.theFloat() == X.asFloat());
                ASSERT(VA          == X.asElemRef());
                ASSERT(!X.isNull());

                X.makeNull();
                ASSERT(X.isNull());
                ASSERT(X.asElemRef().isNull());
                ASSERT(VN.theFloat() == X.asFloat());

                VA.descriptor()->assign(data, VB.data());

                Obj mY; const Obj& Y = mY;
                mY.setDataType(type);
                mY.setDataPointer(data);
                int nf2 = 0;
                mY.setTopLevelAggregateNullnessPointer(&nf2);

                ASSERT(VB.theFloat() == Y.asFloat());
                ASSERT(VB          == Y.asElemRef());
                ASSERT(!Y.isNull());

                Y.makeNull();
                ASSERT(Y.isNull());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(VN.theFloat() == Y.asFloat());
            }

            destroyValuePtr(data, type, &ta);

            // nillable float array
            {
                const char *SPEC = ":aOa&FN";
                Schema schema; const Schema& SCHEMA = schema;
                ggSchema(&schema, SPEC);

                const RecDef *RECORD  = &SCHEMA.record(0);
                const char   *fldName = RECORD->fieldName(0);

                bslma_TestAllocator t;

                Obj mA; const Obj& A = mA;
                int rc = ggAggData(&mA, *RECORD, &t);
                ASSERT(!rc);

                Error err;
                Obj mX; const Obj& X = mX;
                rc = mA.getField(&mX, &err, false, fldName);
                ASSERT(!rc);

                const int NE = 3;
                rc = mX.resize(&err, NE);
                ASSERT(!rc);

                Obj mY; const Obj& Y = mY;
                for (int i = 0; i < NE; ++i) {
                    rc = X.getField(&mY, &err, false, i);
                    ASSERT(!rc);

                    LOOP_ASSERT(i, Y.isNull());
                    LOOP_ASSERT(i, VN.theFloat() == Y.asFloat());
                    LOOP_ASSERT(i, VN.theFloat() == Y.asElemRef().theFloat());
                    LOOP3_ASSERT(i, Y, Y.asElemRef(), Y.asElemRef().isNull());
                    LOOP3_ASSERT(i, Y, Y.asElemRef(), isUnset(Y.asElemRef()));
                }

                rc = mX.setField(&mY, &err, 0, VA);
                ASSERT(!rc);
                ASSERT(VA.theFloat() == Y.asFloat());
                ASSERT(VA          == Y.asElemRef());
                ASSERT(!Y.isNull());

                mY.makeNull();
                ASSERT(VN.theFloat() == Y.asFloat());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(Y.isNull());

                mY.reset();
                rc = mA.setField(&mY, &err, fldName, 1, VB);
                ASSERT(!rc);
                ASSERT(VB.theFloat() == Y.asFloat());
                LOOP2_ASSERT(VB, Y.asElemRef(), VB == Y.asElemRef());
                ASSERT(!Y.isNull());

                mY.makeNull();
                ASSERT(VN.theFloat() == Y.asFloat());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(Y.isNull());

                mY.reset();
                rc = mX.getField(&mY, &err, false, 2);
                ASSERT(!rc);
                rc = mY.setValue(&err, VA);
                ASSERT(!rc);
                ASSERT(VA.theFloat() == Y.asFloat());
                ASSERT(VA          == Y.asElemRef());
                ASSERT(!Y.isNull());

                mY.makeNull();
                ASSERT(VN.theFloat() == Y.asFloat());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(Y.isNull());

                destroyAggData(&mA, &t);
            }
        }

        if (veryVerbose) { T_ cout << "Testing for DOUBLE" << endl; }
        {
            typedef double TYPE;

            ET::Type type = ET::BDEM_DOUBLE;

            const CERef VN = getCERef(type, 0);
            const CERef VA = getCERef(type, 1);
            const CERef VB = getCERef(type, 2);

            void *data = makeValuePtr(type, &ta);
            {
                VA.descriptor()->assign(data, VA.data());

                Obj mX; const Obj& X = mX;
                mX.setDataType(type);
                mX.setDataPointer(data);
                int nf1 = 0;
                mX.setTopLevelAggregateNullnessPointer(&nf1);

                ASSERT(VA.theDouble() == X.asDouble());
                ASSERT(VA             == X.asElemRef());
                ASSERT(!X.isNull());

                X.makeNull();
                ASSERT(X.isNull());
                ASSERT(X.asElemRef().isNull());
                ASSERT(VN.theDouble() == X.asDouble());

                VA.descriptor()->assign(data, VB.data());

                Obj mY; const Obj& Y = mY;
                mY.setDataType(type);
                mY.setDataPointer(data);
                int nf2 = 0;
                mY.setTopLevelAggregateNullnessPointer(&nf2);

                ASSERT(VB.theDouble() == Y.asDouble());
                ASSERT(VB             == Y.asElemRef());
                ASSERT(!Y.isNull());

                Y.makeNull();
                ASSERT(Y.isNull());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(VN.theDouble() == Y.asDouble());
            }

            destroyValuePtr(data, type, &ta);

            // nillable double array
            {
                const char *SPEC = ":aPa&FN";
                Schema schema; const Schema& SCHEMA = schema;
                ggSchema(&schema, SPEC);

                const RecDef *RECORD  = &SCHEMA.record(0);
                const char   *fldName = RECORD->fieldName(0);

                bslma_TestAllocator t;

                Obj mA; const Obj& A = mA;
                int rc = ggAggData(&mA, *RECORD, &t);
                ASSERT(!rc);

                Error err;
                Obj mX; const Obj& X = mX;
                rc = mA.getField(&mX, &err, false, fldName);
                ASSERT(!rc);

                const int NE = 3;
                rc = mX.resize(&err, NE);
                ASSERT(!rc);

                Obj mY; const Obj& Y = mY;
                for (int i = 0; i < NE; ++i) {
                    rc = X.getField(&mY, &err, false, i);
                    ASSERT(!rc);

                    LOOP_ASSERT(i, Y.isNull());
                    LOOP_ASSERT(i, VN.theDouble() == Y.asDouble());
                    LOOP_ASSERT(i,
                                VN.theDouble() == Y.asElemRef().theDouble());
                    LOOP3_ASSERT(i, Y, Y.asElemRef(), Y.asElemRef().isNull());
                    LOOP3_ASSERT(i, Y, Y.asElemRef(), isUnset(Y.asElemRef()));
                }

                rc = mX.setField(&mY, &err, 0, VA);
                ASSERT(!rc);
                ASSERT(VA.theDouble() == Y.asDouble());
                ASSERT(VA             == Y.asElemRef());
                ASSERT(!Y.isNull());

                mY.makeNull();
                ASSERT(VN.theDouble() == Y.asDouble());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(Y.isNull());

                mY.reset();
                rc = mA.setField(&mY, &err, fldName, 1, VB);
                ASSERT(!rc);
                ASSERT(VB.theDouble() == Y.asDouble());
                LOOP2_ASSERT(VB, Y.asElemRef(), VB == Y.asElemRef());
                ASSERT(!Y.isNull());

                mY.makeNull();
                ASSERT(VN.theDouble() == Y.asDouble());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(Y.isNull());

                mY.reset();
                rc = mX.getField(&mY, &err, false, 2);
                ASSERT(!rc);
                rc = mY.setValue(&err, VA);
                ASSERT(!rc);
                ASSERT(VA.theDouble() == Y.asDouble());
                ASSERT(VA             == Y.asElemRef());
                ASSERT(!Y.isNull());

                mY.makeNull();
                ASSERT(VN.theDouble() == Y.asDouble());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(Y.isNull());

                destroyAggData(&mA, &t);
            }
        }

        if (veryVerbose) { T_ cout << "Testing for DATETIME" << endl; }
        {
            typedef Datetime TYPE;

            ET::Type type = ET::BDEM_DATETIME;

            const CERef VN = getCERef(type, 0);
            const CERef VA = getCERef(type, 1);
            const CERef VB = getCERef(type, 2);

            void *data = makeValuePtr(type, &ta);
            {
                VA.descriptor()->assign(data, VA.data());

                Obj mX; const Obj& X = mX;
                mX.setDataType(type);
                mX.setDataPointer(data);
                int nf1 = 0;
                mX.setTopLevelAggregateNullnessPointer(&nf1);

                ASSERT(VA.theDatetime() == X.asDatetime());
                ASSERT(VA               == X.asElemRef());
                ASSERT(!X.isNull());

                X.makeNull();
                ASSERT(X.isNull());
                ASSERT(X.asElemRef().isNull());
                ASSERT(VN.theDatetime() == X.asDatetime());

                VA.descriptor()->assign(data, VB.data());

                Obj mY; const Obj& Y = mY;
                mY.setDataType(type);
                mY.setDataPointer(data);
                int nf2 = 0;
                mY.setTopLevelAggregateNullnessPointer(&nf2);

                ASSERT(VB.theDatetime() == Y.asDatetime());
                ASSERT(VB               == Y.asElemRef());
                ASSERT(!Y.isNull());

                Y.makeNull();
                ASSERT(Y.isNull());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(VN.theDatetime() == Y.asDatetime());
            }

            destroyValuePtr(data, type, &ta);

            // nillable datetime array
            {
                const char *SPEC = ":aRa&FN";
                Schema schema; const Schema& SCHEMA = schema;
                ggSchema(&schema, SPEC);

                const RecDef *RECORD  = &SCHEMA.record(0);
                const char   *fldName = RECORD->fieldName(0);

                bslma_TestAllocator t;

                Obj mA; const Obj& A = mA;
                int rc = ggAggData(&mA, *RECORD, &t);
                ASSERT(!rc);

                Error err;
                Obj mX; const Obj& X = mX;
                rc = mA.getField(&mX, &err, false, fldName);
                ASSERT(!rc);

                const int NE = 3;
                rc = mX.resize(&err, NE);
                ASSERT(!rc);

                Obj mY; const Obj& Y = mY;
                for (int i = 0; i < NE; ++i) {
                    rc = X.getField(&mY, &err, false, i);
                    ASSERT(!rc);

                    LOOP_ASSERT(i, Y.isNull());
                    LOOP_ASSERT(i, VN.theDatetime() == Y.asDatetime());
                    LOOP_ASSERT(i, VN.theDatetime() ==
                                                  Y.asElemRef().theDatetime());
                    LOOP3_ASSERT(i, Y, Y.asElemRef(), Y.asElemRef().isNull());
                    LOOP3_ASSERT(i, Y, Y.asElemRef(), isUnset(Y.asElemRef()));
                }

                rc = mX.setField(&mY, &err, 0, VA);
                ASSERT(!rc);
                ASSERT(VA.theDatetime() == Y.asDatetime());
                ASSERT(VA             == Y.asElemRef());
                ASSERT(!Y.isNull());

                mY.makeNull();
                ASSERT(VN.theDatetime() == Y.asDatetime());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(Y.isNull());

                mY.reset();
                rc = mA.setField(&mY, &err, fldName, 1, VB);
                ASSERT(!rc);
                ASSERT(VB.theDatetime() == Y.asDatetime());
                LOOP2_ASSERT(VB, Y.asElemRef(), VB == Y.asElemRef());
                ASSERT(!Y.isNull());

                mY.makeNull();
                ASSERT(VN.theDatetime() == Y.asDatetime());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(Y.isNull());

                mY.reset();
                rc = mX.getField(&mY, &err, false, 2);
                ASSERT(!rc);
                rc = mY.setValue(&err, VA);
                ASSERT(!rc);
                ASSERT(VA.theDatetime() == Y.asDatetime());
                ASSERT(VA             == Y.asElemRef());
                ASSERT(!Y.isNull());

                mY.makeNull();
                ASSERT(VN.theDatetime() == Y.asDatetime());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(Y.isNull());

                destroyAggData(&mA, &t);
            }
        }

        if (veryVerbose) { T_ cout << "Testing for DATE" << endl; }
        {
            typedef Date TYPE;

            ET::Type type = ET::BDEM_DATE;

            const CERef VN = getCERef(type, 0);
            const CERef VA = getCERef(type, 1);
            const CERef VB = getCERef(type, 2);

            void *data = makeValuePtr(type, &ta);
            {
                VA.descriptor()->assign(data, VA.data());

                Obj mX; const Obj& X = mX;
                mX.setDataType(type);
                mX.setDataPointer(data);
                int nf1 = 0;
                mX.setTopLevelAggregateNullnessPointer(&nf1);

                ASSERT(VA.theDate() == X.asDate());
                ASSERT(VA           == X.asElemRef());
                ASSERT(!X.isNull());

                X.makeNull();
                ASSERT(X.isNull());
                ASSERT(X.asElemRef().isNull());
                ASSERT(VN.theDate() == X.asDate());

                VA.descriptor()->assign(data, VB.data());

                Obj mY; const Obj& Y = mY;
                mY.setDataType(type);
                mY.setDataPointer(data);
                int nf2 = 0;
                mY.setTopLevelAggregateNullnessPointer(&nf2);

                ASSERT(VB.theDate() == Y.asDate());
                ASSERT(VB           == Y.asElemRef());
                ASSERT(!Y.isNull());

                Y.makeNull();
                ASSERT(Y.isNull());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(VN.theDate() == Y.asDate());
            }

            destroyValuePtr(data, type, &ta);

            // nillable date array
            {
                const char *SPEC = ":aSa&FN";
                Schema schema; const Schema& SCHEMA = schema;
                ggSchema(&schema, SPEC);

                const RecDef *RECORD  = &SCHEMA.record(0);
                const char   *fldName = RECORD->fieldName(0);

                bslma_TestAllocator t;

                Obj mA; const Obj& A = mA;
                int rc = ggAggData(&mA, *RECORD, &t);
                ASSERT(!rc);

                Error err;
                Obj mX; const Obj& X = mX;
                rc = mA.getField(&mX, &err, false, fldName);
                ASSERT(!rc);

                const int NE = 3;
                rc = mX.resize(&err, NE);
                ASSERT(!rc);

                Obj mY; const Obj& Y = mY;
                for (int i = 0; i < NE; ++i) {
                    rc = X.getField(&mY, &err, false, i);
                    ASSERT(!rc);

                    LOOP_ASSERT(i, Y.isNull());
                    LOOP_ASSERT(i, VN.theDate() == Y.asDate());
                    LOOP_ASSERT(i, VN.theDate() == Y.asElemRef().theDate());
                    LOOP3_ASSERT(i, Y, Y.asElemRef(), Y.asElemRef().isNull());
                    LOOP3_ASSERT(i, Y, Y.asElemRef(), isUnset(Y.asElemRef()));
                }

                rc = mX.setField(&mY, &err, 0, VA);
                ASSERT(!rc);
                ASSERT(VA.theDate() == Y.asDate());
                ASSERT(VA           == Y.asElemRef());
                ASSERT(!Y.isNull());

                mY.makeNull();
                ASSERT(VN.theDate() == Y.asDate());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(Y.isNull());

                mY.reset();
                rc = mA.setField(&mY, &err, fldName, 1, VB);
                ASSERT(!rc);
                ASSERT(VB.theDate() == Y.asDate());
                LOOP2_ASSERT(VB, Y.asElemRef(), VB == Y.asElemRef());
                ASSERT(!Y.isNull());

                mY.makeNull();
                ASSERT(VN.theDate() == Y.asDate());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(Y.isNull());

                mY.reset();
                rc = mX.getField(&mY, &err, false, 2);
                ASSERT(!rc);
                rc = mY.setValue(&err, VA);
                ASSERT(!rc);
                ASSERT(VA.theDate() == Y.asDate());
                ASSERT(VA           == Y.asElemRef());
                ASSERT(!Y.isNull());

                mY.makeNull();
                ASSERT(VN.theDate() == Y.asDate());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(Y.isNull());

                destroyAggData(&mA, &t);
            }
        }

        if (veryVerbose) { T_ cout << "Testing for TIME" << endl; }
        {
            typedef Time TYPE;

            ET::Type type = ET::BDEM_TIME;

            const CERef VN = getCERef(type, 0);
            const CERef VA = getCERef(type, 1);
            const CERef VB = getCERef(type, 2);

            void *data = makeValuePtr(type, &ta);
            {
                VA.descriptor()->assign(data, VA.data());

                Obj mX; const Obj& X = mX;
                mX.setDataType(type);
                mX.setDataPointer(data);
                int nf1 = 0;
                mX.setTopLevelAggregateNullnessPointer(&nf1);

                ASSERT(VA.theTime() == X.asTime());
                ASSERT(VA           == X.asElemRef());
                ASSERT(!X.isNull());

                X.makeNull();
                ASSERT(X.isNull());
                ASSERT(X.asElemRef().isNull());
                ASSERT(VN.theTime() == X.asTime());

                VA.descriptor()->assign(data, VB.data());

                Obj mY; const Obj& Y = mY;
                mY.setDataType(type);
                mY.setDataPointer(data);
                int nf2 = 0;
                mY.setTopLevelAggregateNullnessPointer(&nf2);

                ASSERT(VB.theTime() == Y.asTime());
                ASSERT(VB           == Y.asElemRef());
                ASSERT(!Y.isNull());

                Y.makeNull();
                ASSERT(Y.isNull());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(VN.theTime() == Y.asTime());
            }

            destroyValuePtr(data, type, &ta);

            // nillable time array
            {
                const char *SPEC = ":aTa&FN";
                Schema schema; const Schema& SCHEMA = schema;
                ggSchema(&schema, SPEC);

                const RecDef *RECORD  = &SCHEMA.record(0);
                const char   *fldName = RECORD->fieldName(0);

                bslma_TestAllocator t;

                Obj mA; const Obj& A = mA;
                int rc = ggAggData(&mA, *RECORD, &t);
                ASSERT(!rc);

                Error err;
                Obj mX; const Obj& X = mX;
                rc = mA.getField(&mX, &err, false, fldName);
                ASSERT(!rc);

                const int NE = 3;
                rc = mX.resize(&err, NE);
                ASSERT(!rc);

                Obj mY; const Obj& Y = mY;
                for (int i = 0; i < NE; ++i) {
                    rc = X.getField(&mY, &err, false, i);
                    ASSERT(!rc);

                    LOOP_ASSERT(i, Y.isNull());
                    LOOP_ASSERT(i, VN.theTime() == Y.asTime());
                    LOOP_ASSERT(i, VN.theTime() == Y.asElemRef().theTime());
                    LOOP3_ASSERT(i, Y, Y.asElemRef(), Y.asElemRef().isNull());
                    LOOP3_ASSERT(i, Y, Y.asElemRef(), isUnset(Y.asElemRef()));
                }

                rc = mX.setField(&mY, &err, 0, VA);
                ASSERT(!rc);
                ASSERT(VA.theTime() == Y.asTime());
                ASSERT(VA           == Y.asElemRef());
                ASSERT(!Y.isNull());

                mY.makeNull();
                ASSERT(VN.theTime() == Y.asTime());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(Y.isNull());

                mY.reset();
                rc = mA.setField(&mY, &err, fldName, 1, VB);
                ASSERT(!rc);
                ASSERT(VB.theTime() == Y.asTime());
                LOOP2_ASSERT(VB, Y.asElemRef(), VB == Y.asElemRef());
                ASSERT(!Y.isNull());

                mY.makeNull();
                ASSERT(VN.theTime() == Y.asTime());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(Y.isNull());

                mY.reset();
                rc = mX.getField(&mY, &err, false, 2);
                ASSERT(!rc);
                rc = mY.setValue(&err, VA);
                ASSERT(!rc);
                ASSERT(VA.theTime() == Y.asTime());
                ASSERT(VA           == Y.asElemRef());
                ASSERT(!Y.isNull());

                mY.makeNull();
                ASSERT(VN.theTime() == Y.asTime());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(Y.isNull());

                destroyAggData(&mA, &t);
            }
        }

        if (veryVerbose) { T_ cout << "Testing for DATETIMETZ" << endl; }
        {
            typedef DatetimeTz TYPE;

            ET::Type type = ET::BDEM_DATETIMETZ;

            const CERef VN = getCERef(type, 0);
            const CERef VA = getCERef(type, 1);
            const CERef VB = getCERef(type, 2);

            void *data = makeValuePtr(type, &ta);
            {
                VA.descriptor()->assign(data, VA.data());

                Obj mX; const Obj& X = mX;
                mX.setDataType(type);
                mX.setDataPointer(data);
                int nf1 = 0;
                mX.setTopLevelAggregateNullnessPointer(&nf1);

                ASSERT(VA.theDatetimeTz() == X.asDatetimeTz());
                ASSERT(VA                 == X.asElemRef());
                ASSERT(!X.isNull());

                X.makeNull();
                ASSERT(X.isNull());
                ASSERT(X.asElemRef().isNull());
                ASSERT(VN.theDatetimeTz() == X.asDatetimeTz());

                VA.descriptor()->assign(data, VB.data());

                Obj mY; const Obj& Y = mY;
                mY.setDataType(type);
                mY.setDataPointer(data);
                int nf2 = 0;
                mY.setTopLevelAggregateNullnessPointer(&nf2);

                ASSERT(VB.theDatetimeTz() == Y.asDatetimeTz());
                ASSERT(VB                 == Y.asElemRef());
                ASSERT(!Y.isNull());

                Y.makeNull();
                ASSERT(Y.isNull());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(VN.theDatetimeTz() == Y.asDatetimeTz());
            }

            destroyValuePtr(data, type, &ta);

            // nillable datetimeTz array
            {
                const char *SPEC = ":aba&FN";
                Schema schema; const Schema& SCHEMA = schema;
                ggSchema(&schema, SPEC);

                const RecDef *RECORD  = &SCHEMA.record(0);
                const char   *fldName = RECORD->fieldName(0);

                bslma_TestAllocator t;

                Obj mA; const Obj& A = mA;
                int rc = ggAggData(&mA, *RECORD, &t);
                ASSERT(!rc);

                Error err;
                Obj mX; const Obj& X = mX;
                rc = mA.getField(&mX, &err, false, fldName);
                ASSERT(!rc);

                const int NE = 3;
                rc = mX.resize(&err, NE);
                ASSERT(!rc);

                Obj mY; const Obj& Y = mY;
                for (int i = 0; i < NE; ++i) {
                    rc = X.getField(&mY, &err, false, i);
                    ASSERT(!rc);

                    LOOP_ASSERT(i, Y.isNull());
                    LOOP_ASSERT(i, VN.theDatetimeTz() == Y.asDatetimeTz());
                    LOOP_ASSERT(i, VN.theDatetimeTz() ==
                                                Y.asElemRef().theDatetimeTz());
                    LOOP3_ASSERT(i, Y, Y.asElemRef(), Y.asElemRef().isNull());
                    LOOP3_ASSERT(i, Y, Y.asElemRef(), isUnset(Y.asElemRef()));
                }

                rc = mX.setField(&mY, &err, 0, VA);
                ASSERT(!rc);
                ASSERT(VA.theDatetimeTz() == Y.asDatetimeTz());
                ASSERT(VA                 == Y.asElemRef());
                ASSERT(!Y.isNull());

                mY.makeNull();
                ASSERT(VN.theDatetimeTz() == Y.asDatetimeTz());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(Y.isNull());

                mY.reset();
                rc = mA.setField(&mY, &err, fldName, 1, VB);
                ASSERT(!rc);
                ASSERT(VB.theDatetimeTz() == Y.asDatetimeTz());
                LOOP2_ASSERT(VB, Y.asElemRef(), VB == Y.asElemRef());
                ASSERT(!Y.isNull());

                mY.makeNull();
                ASSERT(VN.theDatetimeTz() == Y.asDatetimeTz());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(Y.isNull());

                mY.reset();
                rc = mX.getField(&mY, &err, false, 2);
                ASSERT(!rc);
                rc = mY.setValue(&err, VA);
                ASSERT(!rc);
                ASSERT(VA.theDatetimeTz() == Y.asDatetimeTz());
                ASSERT(VA                 == Y.asElemRef());
                ASSERT(!Y.isNull());

                mY.makeNull();
                ASSERT(VN.theDatetimeTz() == Y.asDatetimeTz());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(Y.isNull());

                destroyAggData(&mA, &t);
            }
        }

        if (veryVerbose) { T_ cout << "Testing for DATETZ" << endl; }
        {
            typedef DateTz TYPE;

            ET::Type type = ET::BDEM_DATETZ;

            const CERef VN = getCERef(type, 0);
            const CERef VA = getCERef(type, 1);
            const CERef VB = getCERef(type, 2);

            void *data = makeValuePtr(type, &ta);
            {
                VA.descriptor()->assign(data, VA.data());

                Obj mX; const Obj& X = mX;
                mX.setDataType(type);
                mX.setDataPointer(data);
                int nf1 = 0;
                mX.setTopLevelAggregateNullnessPointer(&nf1);

                ASSERT(VA.theDateTz() == X.asDateTz());
                ASSERT(VA             == X.asElemRef());
                ASSERT(!X.isNull());

                X.makeNull();
                ASSERT(X.isNull());
                ASSERT(X.asElemRef().isNull());
                ASSERT(VN.theDateTz() == X.asDateTz());

                VA.descriptor()->assign(data, VB.data());

                Obj mY; const Obj& Y = mY;
                mY.setDataType(type);
                mY.setDataPointer(data);
                int nf2 = 0;
                mY.setTopLevelAggregateNullnessPointer(&nf2);

                ASSERT(VB.theDateTz() == Y.asDateTz());
                ASSERT(VB           == Y.asElemRef());
                ASSERT(!Y.isNull());

                Y.makeNull();
                ASSERT(Y.isNull());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(VN.theDateTz() == Y.asDateTz());
            }
            destroyValuePtr(data, type, &ta);

            // nillable dateTz array
            {
                const char *SPEC = ":aca&FN";
                Schema schema; const Schema& SCHEMA = schema;
                ggSchema(&schema, SPEC);

                const RecDef *RECORD  = &SCHEMA.record(0);
                const char   *fldName = RECORD->fieldName(0);

                bslma_TestAllocator t;

                Obj mA; const Obj& A = mA;
                int rc = ggAggData(&mA, *RECORD, &t);
                ASSERT(!rc);

                Error err;
                Obj mX; const Obj& X = mX;
                rc = mA.getField(&mX, &err, false, fldName);
                ASSERT(!rc);

                const int NE = 3;
                rc = mX.resize(&err, NE);
                ASSERT(!rc);

                Obj mY; const Obj& Y = mY;
                for (int i = 0; i < NE; ++i) {
                    rc = X.getField(&mY, &err, false, i);
                    ASSERT(!rc);

                    LOOP_ASSERT(i, Y.isNull());
                    LOOP_ASSERT(i, VN.theDateTz() == Y.asDateTz());
                    LOOP_ASSERT(i,
                                VN.theDateTz() == Y.asElemRef().theDateTz());
                    LOOP3_ASSERT(i, Y, Y.asElemRef(), Y.asElemRef().isNull());
                    LOOP3_ASSERT(i, Y, Y.asElemRef(), isUnset(Y.asElemRef()));
                }

                rc = mX.setField(&mY, &err, 0, VA);
                ASSERT(!rc);
                ASSERT(VA.theDateTz() == Y.asDateTz());
                ASSERT(VA             == Y.asElemRef());
                ASSERT(!Y.isNull());

                mY.makeNull();
                ASSERT(VN.theDateTz() == Y.asDateTz());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(Y.isNull());

                mY.reset();
                rc = mA.setField(&mY, &err, fldName, 1, VB);
                ASSERT(!rc);
                ASSERT(VB.theDateTz() == Y.asDateTz());
                LOOP2_ASSERT(VB, Y.asElemRef(), VB == Y.asElemRef());
                ASSERT(!Y.isNull());

                mY.makeNull();
                ASSERT(VN.theDateTz() == Y.asDateTz());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(Y.isNull());

                mY.reset();
                rc = mX.getField(&mY, &err, false, 2);
                ASSERT(!rc);
                rc = mY.setValue(&err, VA);
                ASSERT(!rc);
                ASSERT(VA.theDateTz() == Y.asDateTz());
                ASSERT(VA             == Y.asElemRef());
                ASSERT(!Y.isNull());

                mY.makeNull();
                ASSERT(VN.theDateTz() == Y.asDateTz());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(Y.isNull());

                destroyAggData(&mA, &t);
            }
        }

        if (veryVerbose) { T_ cout << "Testing for TIMETZ" << endl; }
        {
            typedef TimeTz TYPE;

            ET::Type type = ET::BDEM_TIMETZ;

            const CERef VN = getCERef(type, 0);
            const CERef VA = getCERef(type, 1);
            const CERef VB = getCERef(type, 2);

            void *data = makeValuePtr(type, &ta);
            {
                VA.descriptor()->assign(data, VA.data());

                Obj mX; const Obj& X = mX;
                mX.setDataType(type);
                mX.setDataPointer(data);
                int nf1 = 0;
                mX.setTopLevelAggregateNullnessPointer(&nf1);

                ASSERT(VA.theTimeTz() == X.asTimeTz());
                ASSERT(VA             == X.asElemRef());
                ASSERT(!X.isNull());

                X.makeNull();
                ASSERT(X.isNull());
                ASSERT(X.asElemRef().isNull());
                ASSERT(VN.theTimeTz() == X.asTimeTz());

                VA.descriptor()->assign(data, VB.data());

                Obj mY; const Obj& Y = mY;
                mY.setDataType(type);
                mY.setDataPointer(data);
                int nf2 = 0;
                mY.setTopLevelAggregateNullnessPointer(&nf2);

                ASSERT(VB.theTimeTz() == Y.asTimeTz());
                ASSERT(VB             == Y.asElemRef());
                ASSERT(!Y.isNull());

                Y.makeNull();
                ASSERT(Y.isNull());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(VN.theTimeTz() == Y.asTimeTz());
            }
            destroyValuePtr(data, type, &ta);

            // nillable timeTz array
            {
                const char *SPEC = ":ada&FN";
                Schema schema; const Schema& SCHEMA = schema;
                ggSchema(&schema, SPEC);

                const RecDef *RECORD  = &SCHEMA.record(0);
                const char   *fldName = RECORD->fieldName(0);

                bslma_TestAllocator t;

                Obj mA; const Obj& A = mA;
                int rc = ggAggData(&mA, *RECORD, &t);
                ASSERT(!rc);

                Error err;
                Obj mX; const Obj& X = mX;
                rc = mA.getField(&mX, &err, false, fldName);
                ASSERT(!rc);

                const int NE = 3;
                rc = mX.resize(&err, NE);
                ASSERT(!rc);

                Obj mY; const Obj& Y = mY;
                for (int i = 0; i < NE; ++i) {
                    rc = X.getField(&mY, &err, false, i);
                    ASSERT(!rc);

                    LOOP_ASSERT(i, Y.isNull());
                    LOOP_ASSERT(i, VN.theTimeTz() == Y.asTimeTz());
                    LOOP_ASSERT(i,
                                VN.theTimeTz() == Y.asElemRef().theTimeTz());
                    LOOP3_ASSERT(i, Y, Y.asElemRef(), Y.asElemRef().isNull());
                    LOOP3_ASSERT(i, Y, Y.asElemRef(), isUnset(Y.asElemRef()));
                }

                rc = mX.setField(&mY, &err, 0, VA);
                ASSERT(!rc);
                ASSERT(VA.theTimeTz() == Y.asTimeTz());
                ASSERT(VA             == Y.asElemRef());
                ASSERT(!Y.isNull());

                mY.makeNull();
                ASSERT(VN.theTimeTz() == Y.asTimeTz());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(Y.isNull());

                mY.reset();
                rc = mA.setField(&mY, &err, fldName, 1, VB);
                ASSERT(!rc);
                ASSERT(VB.theTimeTz() == Y.asTimeTz());
                LOOP2_ASSERT(VB, Y.asElemRef(), VB == Y.asElemRef());
                ASSERT(!Y.isNull());

                mY.makeNull();
                ASSERT(VN.theTimeTz() == Y.asTimeTz());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(Y.isNull());

                mY.reset();
                rc = mX.getField(&mY, &err, false, 2);
                ASSERT(!rc);
                rc = mY.setValue(&err, VA);
                ASSERT(!rc);
                ASSERT(VA.theTimeTz() == Y.asTimeTz());
                ASSERT(VA             == Y.asElemRef());
                ASSERT(!Y.isNull());

                mY.makeNull();
                ASSERT(VN.theTimeTz() == Y.asTimeTz());
                ASSERT(Y.asElemRef().isNull());
                ASSERT(Y.isNull());

                destroyAggData(&mA, &t);
            }
        }
      } break;
      case 4: {
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
        //   int setField(Obj *o, Error *e, f1, f2, value) const;
        //   int setField(Obj *o, Error *e, f1, f2, f3, value) const;
        //   int setField(Obj *o, Error *e, f1, f2, . ., f4, value) const;
        //   int setField(Obj *o, Error *e, f1, f2, . ., f5, value) const;
        //   int setField(Obj *o, Error *e, f1, f2, . ., f6, value) const;
        //   int setField(Obj *o, Error *e, f1, f2, . ., f7, value) const;
        //   int setField(Obj *o, Error *e, f1, f2, . ., f8, value) const;
        //   int setField(Obj *o, Error *e, f1, f2, . ., f9, value) const;
        //   int setField(Obj *o, Error *e, f1, f2, . ., f10, value) const;
        //   int getField(Obj *o, Error *e, bool null, f1, f2, . ., f10) const;
        //   bool hasField(const char *fieldName) const;
        //   bool hasFieldById(int fieldId) const;
        //   bool hasFieldByIndex(int fieldIndex) const;
        //   int fieldByIndex(Obj *obj, Error *error, int index) const;
        //   int fieldById(Obj *obj, Error *error, int id) const;
        //   const bdem_FieldDef *fieldDef() const;
        //   bdem_ElemType::Type dataType() const;
        //   const bdem_Schema *schema() const;
        //
        // Private functions:
        //   NavStatus descendIntoField(NameOrIndex fieldOrIdx1);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'setField' MANIPULATOR"
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
            const RecDef  *r  = &s.record(s.numRecords() - 1);
            const FldDef&  fd = s.record(0).field(0);

            if (veryVerbose) { P(s) };

            bslma_TestAllocator t(veryVeryVerbose);

            const CERef VN = getCERef(FLD_TYPE, 0);
            const CERef VA = getCERef(FLD_TYPE, 1);
            const CERef VB = getCERef(FLD_TYPE, 2);

            if (veryVerbose) { P(VA) };

            Obj mX; const Obj& X = mX;
            int rc = ggAggData(&mX, *r, &t);
            ASSERT(!rc);

            Obj mY; const Obj& Y = mY;
            rc = ggAggData(&mY, *r, &t);
            ASSERT(!rc);

            if (veryVerbose) { P(X) P(Y) };

            ASSERT(0  == X.fieldDef());
            ASSERT(r  == X.recordConstraint());
            ASSERT(&s == X.schema());

            const char *errorField = "ErrorField";
            const int   NUM_RECS   = s.numRecords();
            switch (NUM_RECS) {
              case 1: {
                const char f1[] = { (*bsl::strtok(SPEC, ":")), 0 };

                ASSERT(X.hasField(f1));
                ASSERT(X.hasFieldById(0));
                ASSERT(X.hasFieldByIndex(0));
                ASSERT(!X.hasField(errorField));
                ASSERT(!X.hasFieldById(NUM_RECS));
                ASSERT(!X.hasFieldByIndex(NUM_RECS));

                Error error;
                Obj   mS; const Obj& S = mS;
                rc = mX.setField(&mS, &error, f1, VA);
                ASSERT(!rc);
                ASSERT(FLD_TYPE == S.dataType());
                ASSERT(&fd      == S.fieldDef());
                ASSERT(0        == S.recordConstraint());
                ASSERT(VA       == S.asElemRef());
                ASSERT(!S.isNull());

                Obj   mT; const Obj& T = mT;
                rc = mX.fieldById(&mT, &error, 0);
                ASSERT(!rc);
                ASSERT(FLD_TYPE == T.dataType());
                ASSERT(&fd      == T.fieldDef());
                ASSERT(0        == T.recordConstraint());
                ASSERT(VA       == T.asElemRef());
                ASSERT(!T.isNull());

                rc = mX.fieldById(&mT, &error, NUM_RECS);
                ASSERT(rc);

                Obj   mV; const Obj& V = mV;
                rc = mX.fieldByIndex(&mV, &error, 0);
                ASSERT(!rc);
                ASSERT(FLD_TYPE == V.dataType());
                ASSERT(&fd      == V.fieldDef());
                ASSERT(0        == V.recordConstraint());
                ASSERT(VA       == V.asElemRef());
                ASSERT(!V.isNull());

                rc = mX.fieldByIndex(&mV, &error, NUM_RECS);
                ASSERT(rc);

                mS.reset();
                rc = X.getField(&mS, &error, false, f1);
                ASSERT(!rc);
                ASSERT(FLD_TYPE == S.dataType());
                ASSERT(&fd      == S.fieldDef());
                ASSERT(0        == S.recordConstraint());
                ASSERT(VA       == S.asElemRef());
                ASSERT(!S.isNull());

                mS.makeNull();
                ASSERT(FLD_TYPE == S.dataType());
                ASSERT(&fd      == S.fieldDef());
                ASSERT(0        == S.recordConstraint());
                ASSERT(isUnset(S.asElemRef()));
                ASSERT(S.isNull());

                mS.reset();
                rc = mX.setField(&mS, &error, f1, VB);
                ASSERT(!rc);
                ASSERT(FLD_TYPE == S.dataType());
                ASSERT(&fd      == S.fieldDef());
                ASSERT(0        == S.recordConstraint());
                ASSERT(VB       == S.asElemRef());
                ASSERT(!S.isNull());

                mS.reset();
                rc = X.getField(&mS, &error, false, f1);
                ASSERT(!rc);
                ASSERT(FLD_TYPE == S.dataType());
                ASSERT(&fd      == S.fieldDef());
                ASSERT(0        == S.recordConstraint());
                ASSERT(VB       == S.asElemRef());
                ASSERT(!S.isNull());

                // Test that scalar types are returned by reference
                mS.setValue(&error, VA);
                ASSERT(VA == S.asElemRef());
                Obj   mB; const Obj& B = mB;
                rc = mX.getField(&mB, &error, false, f1);
                ASSERT(!rc);
                ASSERT(FLD_TYPE == S.dataType());
                ASSERT(&fd      == S.fieldDef());
                ASSERT(0        == S.recordConstraint());
                ASSERT(VA       == B.asElemRef());
                ASSERT(!S.isNull());
              } break;
              case 2: {
                const char f1[] = { *(bsl::strtok(SPEC, ":")), 0 };
                const char f2[] = { *(bsl::strtok(0, ":") + 2), 0 };

                ASSERT(X.hasField(f2));
                ASSERT(X.hasFieldById(0));
                ASSERT(X.hasFieldByIndex(0));
                ASSERT(!X.hasField(errorField));
                ASSERT(!X.hasFieldById(NUM_RECS));
                ASSERT(!X.hasFieldByIndex(NUM_RECS));

                Error error;
                Obj   mS; const Obj& S = mS;
                rc = mX.setField(&mS, &error, f2, f1, VA);
                ASSERT(!rc);
                ASSERT(FLD_TYPE == S.dataType());
                ASSERT(&fd      == S.fieldDef());
                ASSERT(0        == S.recordConstraint());
                ASSERT(VA       == S.asElemRef());
                ASSERT(!S.isNull());

                Obj   mT; const Obj& T = mT;
                rc = mX.fieldById(&mT, &error, 0);
                ASSERT(!rc);
                ASSERT(!T.isNull());

                rc = mX.fieldById(&mT, &error, NUM_RECS);
                ASSERT(rc);

                Obj   mV; const Obj& V = mV;
                rc = mX.fieldByIndex(&mV, &error, 0);
                ASSERT(!rc);
                ASSERT(!V.isNull());

                rc = mX.fieldByIndex(&mV, &error, NUM_RECS);
                ASSERT(rc);

                mS.reset();
                rc = X.getField(&mS, &error, false, f2, f1);
                ASSERT(!rc);
                ASSERT(FLD_TYPE == S.dataType());
                ASSERT(&fd      == S.fieldDef());
                ASSERT(0        == S.recordConstraint());
                ASSERT(VA       == S.asElemRef());
                ASSERT(!S.isNull());

                mS.makeNull();
                ASSERT(FLD_TYPE == S.dataType());
                ASSERT(&fd      == S.fieldDef());
                ASSERT(0        == S.recordConstraint());
                ASSERT(isUnset(S.asElemRef()));
                ASSERT(S.isNull());

                rc = mX.setField(&mS, &error, f2, f1, VB);
                ASSERT(!rc);
                ASSERT(FLD_TYPE == S.dataType());
                ASSERT(&fd      == S.fieldDef());
                ASSERT(0        == S.recordConstraint());
                ASSERT(VB       == S.asElemRef());
                ASSERT(!S.isNull());

                mS.reset();
                rc = X.getField(&mS, &error, false, f2, f1);
                ASSERT(!rc);
                ASSERT(FLD_TYPE == S.dataType());
                ASSERT(&fd      == S.fieldDef());
                ASSERT(0        == S.recordConstraint());
                ASSERT(VB       == S.asElemRef());
                ASSERT(!S.isNull());

                // Test that scalar types are returned by reference
                mS.setValue(&error, VA);
                ASSERT(VA == S.asElemRef());
                Obj   mB; const Obj& B = mB;
                rc = X.getField(&mB, &error, false, f2, f1);
                ASSERT(!rc);
                ASSERT(FLD_TYPE == S.dataType());
                ASSERT(&fd      == S.fieldDef());
                ASSERT(0        == S.recordConstraint());
                ASSERT(VA       == B.asElemRef());
                ASSERT(!S.isNull());
              } break;
              case 3: {
                const char f1[] = { *(bsl::strtok(SPEC, ":")), 0 };
                const char f2[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f3[] = { *(bsl::strtok(0, ":") + 2), 0 };

                ASSERT(X.hasField(f3));
                ASSERT(X.hasFieldById(0));
                ASSERT(X.hasFieldByIndex(0));
                ASSERT(!X.hasField(errorField));
                ASSERT(!X.hasFieldById(NUM_RECS));
                ASSERT(!X.hasFieldByIndex(NUM_RECS));

                Error error;
                Obj   mS; const Obj& S = mS;
                rc = mX.setField(&mS, &error, f3, f2, f1, VA);
                ASSERT(!rc);
                ASSERT(FLD_TYPE == S.dataType());
                ASSERT(&fd      == S.fieldDef());
                ASSERT(0        == S.recordConstraint());
                ASSERT(VA       == S.asElemRef());
                ASSERT(!S.isNull());

                Obj   mT; const Obj& T = mT;
                rc = mX.fieldById(&mT, &error, 0);
                ASSERT(!rc);
                ASSERT(!T.isNull());

                rc = mX.fieldById(&mT, &error, NUM_RECS);
                ASSERT(rc);

                Obj   mV; const Obj& V = mV;
                rc = mX.fieldByIndex(&mV, &error, 0);
                ASSERT(!rc);
                ASSERT(!V.isNull());

                rc = mX.fieldByIndex(&mV, &error, NUM_RECS);
                ASSERT(rc);

                mS.reset();
                rc = X.getField(&mS, &error, false, f3, f2, f1);
                ASSERT(!rc);
                ASSERT(FLD_TYPE == S.dataType());
                ASSERT(&fd      == S.fieldDef());
                ASSERT(0        == S.recordConstraint());
                ASSERT(VA       == S.asElemRef());
                ASSERT(!S.isNull());

                mS.makeNull();
                ASSERT(FLD_TYPE == S.dataType());
                ASSERT(&fd      == S.fieldDef());
                ASSERT(0        == S.recordConstraint());
                ASSERT(isUnset(S.asElemRef()));
                ASSERT(S.isNull());

                rc = mX.setField(&mS, &error, f3, f2, f1, VB);
                ASSERT(!rc);
                ASSERT(FLD_TYPE == S.dataType());
                ASSERT(&fd      == S.fieldDef());
                ASSERT(0        == S.recordConstraint());
                ASSERT(VB       == S.asElemRef());
                ASSERT(!S.isNull());

                mS.reset();
                rc = X.getField(&mS, &error, false, f3, f2, f1);
                ASSERT(!rc);
                ASSERT(FLD_TYPE == S.dataType());
                ASSERT(&fd      == S.fieldDef());
                ASSERT(0        == S.recordConstraint());
                ASSERT(VB       == S.asElemRef());
                ASSERT(!S.isNull());

                // Test that scalar types are returned by reference
                mS.setValue(&error, VA);
                ASSERT(VA == S.asElemRef());
                Obj   mB; const Obj& B = mB;
                rc = X.getField(&mB, &error, false, f3, f2, f1);
                ASSERT(!rc);
                ASSERT(FLD_TYPE == S.dataType());
                ASSERT(&fd      == S.fieldDef());
                ASSERT(0        == S.recordConstraint());
                ASSERT(VA       == B.asElemRef());
                ASSERT(!S.isNull());
              } break;
              case 4: {
                const char f1[] = { *(bsl::strtok(SPEC, ":")), 0 };
                const char f2[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f3[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f4[] = { *(bsl::strtok(0, ":") + 2), 0 };

                ASSERT(X.hasField(f4));
                ASSERT(X.hasFieldById(0));
                ASSERT(X.hasFieldByIndex(0));
                ASSERT(!X.hasField(errorField));
                ASSERT(!X.hasFieldById(NUM_RECS));
                ASSERT(!X.hasFieldByIndex(NUM_RECS));

                Error error;
                Obj   mS; const Obj& S = mS;
                rc = mX.setField(&mS, &error, f4, f3, f2, f1, VA);
                ASSERT(!rc);
                ASSERT(FLD_TYPE == S.dataType());
                ASSERT(&fd      == S.fieldDef());
                LOOP3_ASSERT(LINE, VA, S.asElemRef(), VA == S.asElemRef());
                ASSERT(!S.isNull());

                Obj   mT; const Obj& T = mT;
                rc = mX.fieldById(&mT, &error, 0);
                ASSERT(!rc);
                ASSERT(!T.isNull());

                rc = mX.fieldById(&mT, &error, NUM_RECS);
                ASSERT(rc);

                Obj   mV; const Obj& V = mV;
                rc = mX.fieldByIndex(&mV, &error, 0);
                ASSERT(!rc);
                ASSERT(!V.isNull());

                rc = mX.fieldByIndex(&mV, &error, NUM_RECS);
                ASSERT(rc);

                mS.reset();
                rc = X.getField(&mS, &error, false, f4, f3, f2, f1);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, VA, S.asElemRef(), VA == S.asElemRef());
                ASSERT(!S.isNull());

                mS.makeNull();
                ASSERT(FLD_TYPE == S.dataType());
                ASSERT(&fd      == S.fieldDef());
                ASSERT(isUnset(S.asElemRef()));
                ASSERT(S.isNull());

                rc = mX.setField(&mS, &error, f4, f3, f2, f1, VB);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, VB, S.asElemRef(), VB == S.asElemRef());
                ASSERT(!S.isNull());

                mS.reset();
                rc = X.getField(&mS, &error, false, f4, f3, f2, f1);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, VB, S.asElemRef(), VB == S.asElemRef());
                ASSERT(!S.isNull());

                // Test that scalar types are returned by reference
                mS.setValue(&error, VA);
                ASSERT(VA == S.asElemRef());
                Obj   mB; const Obj& B = mB;
                rc = X.getField(&mB, &error, false, f4, f3, f2, f1);
                ASSERT(!rc);
                ASSERT(VA == B.asElemRef());
                ASSERT(!S.isNull());
              } break;
              case 5: {
                const char f1[] = { *(bsl::strtok(SPEC, ":")), 0 };
                const char f2[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f3[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f4[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f5[] = { *(bsl::strtok(0, ":") + 2), 0 };

                ASSERT(X.hasField(f5));
                ASSERT(X.hasFieldById(0));
                ASSERT(X.hasFieldByIndex(0));
                ASSERT(!X.hasField(errorField));
                ASSERT(!X.hasFieldById(NUM_RECS));
                ASSERT(!X.hasFieldByIndex(NUM_RECS));

                Error error;
                Obj   mS; const Obj& S = mS;
                rc = mX.setField(&mS, &error, f5, f4, f3, f2, f1, VA);
                ASSERT(!rc);
                ASSERT(FLD_TYPE == S.dataType());
                ASSERT(&fd      == S.fieldDef());
                LOOP3_ASSERT(LINE, VA, S.asElemRef(), VA == S.asElemRef());
                ASSERT(!S.isNull());

                Obj   mT; const Obj& T = mT;
                rc = mX.fieldById(&mT, &error, 0);
                ASSERT(!rc);
                ASSERT(!T.isNull());

                rc = mX.fieldById(&mT, &error, NUM_RECS);
                ASSERT(rc);

                Obj   mV; const Obj& V = mV;
                rc = mX.fieldByIndex(&mV, &error, 0);
                ASSERT(!rc);
                ASSERT(!V.isNull());

                rc = mX.fieldByIndex(&mV, &error, NUM_RECS);
                ASSERT(rc);

                mS.reset();
                rc = X.getField(&mS, &error, false, f5, f4, f3, f2, f1);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, VA, S.asElemRef(), VA == S.asElemRef());
                ASSERT(!S.isNull());

                mS.makeNull();
                ASSERT(FLD_TYPE == S.dataType());
                ASSERT(&fd      == S.fieldDef());
                ASSERT(isUnset(S.asElemRef()));
                ASSERT(S.isNull());

                rc = mX.setField(&mS, &error, f5, f4, f3, f2, f1, VB);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, VB, S.asElemRef(), VB == S.asElemRef());
                ASSERT(!S.isNull());

                mS.reset();
                rc = X.getField(&mS, &error, false, f5, f4, f3, f2, f1);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, VB, S.asElemRef(), VB == S.asElemRef());
                ASSERT(!S.isNull());

                // Test that scalar types are returned by reference
                mS.setValue(&error, VA);
                ASSERT(VA == S.asElemRef());
                Obj   mB; const Obj& B = mB;
                rc = X.getField(&mB, &error, false, f5, f4, f3, f2, f1);
                ASSERT(!rc);
                ASSERT(VA == B.asElemRef());
                ASSERT(!S.isNull());
              } break;
              case 6: {
                const char f1[] = { *(bsl::strtok(SPEC, ":")), 0 };
                const char f2[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f3[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f4[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f5[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f6[] = { *(bsl::strtok(0, ":") + 2), 0 };

                ASSERT(X.hasField(f6));
                ASSERT(X.hasFieldById(0));
                ASSERT(X.hasFieldByIndex(0));
                ASSERT(!X.hasField(errorField));
                ASSERT(!X.hasFieldById(NUM_RECS));
                ASSERT(!X.hasFieldByIndex(NUM_RECS));

                Error error;
                Obj   mS; const Obj& S = mS;
                rc = mX.setField(&mS, &error, f6, f5, f4, f3, f2, f1, VA);
                ASSERT(!rc);
                ASSERT(FLD_TYPE == S.dataType());
                ASSERT(&fd      == S.fieldDef());
                LOOP3_ASSERT(LINE, VA, S.asElemRef(), VA == S.asElemRef());
                ASSERT(!S.isNull());

                Obj   mT; const Obj& T = mT;
                rc = mX.fieldById(&mT, &error, 0);
                ASSERT(!rc);
                ASSERT(!T.isNull());

                rc = mX.fieldById(&mT, &error, NUM_RECS);
                ASSERT(rc);

                Obj   mV; const Obj& V = mV;
                rc = mX.fieldByIndex(&mV, &error, 0);
                ASSERT(!rc);
                ASSERT(!V.isNull());

                rc = mX.fieldByIndex(&mV, &error, NUM_RECS);
                ASSERT(rc);

                mS.reset();
                rc = X.getField(&mS, &error, false, f6, f5, f4, f3, f2, f1);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, VA, S.asElemRef(), VA == S.asElemRef());
                ASSERT(!S.isNull());

                mS.makeNull();
                ASSERT(FLD_TYPE == S.dataType());
                ASSERT(&fd      == S.fieldDef());
                ASSERT(isUnset(S.asElemRef()));
                ASSERT(S.isNull());

                rc = mX.setField(&mS, &error, f6, f5, f4, f3, f2, f1, VB);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, VB, S.asElemRef(), VB == S.asElemRef());
                ASSERT(!S.isNull());

                mS.reset();
                rc = X.getField(&mS, &error, false, f6, f5, f4, f3, f2, f1);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, VB, S.asElemRef(), VB == S.asElemRef());
                ASSERT(!S.isNull());

                // Test that scalar types are returned by reference
                mS.setValue(&error, VA);
                ASSERT(VA == S.asElemRef());
                Obj   mB; const Obj& B = mB;
                rc = X.getField(&mB, &error, false, f6, f5, f4, f3, f2, f1);
                ASSERT(!rc);
                ASSERT(VA == B.asElemRef());
                ASSERT(!S.isNull());
              } break;
              case 7: {
                const char f1[] = { *(bsl::strtok(SPEC, ":")), 0 };
                const char f2[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f3[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f4[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f5[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f6[] = { *(bsl::strtok(0, ":") + 2), 0 };
                const char f7[] = { *(bsl::strtok(0, ":") + 2), 0 };

                ASSERT(X.hasField(f7));
                ASSERT(X.hasFieldById(0));
                ASSERT(X.hasFieldByIndex(0));
                ASSERT(!X.hasField(errorField));
                ASSERT(!X.hasFieldById(NUM_RECS));
                ASSERT(!X.hasFieldByIndex(NUM_RECS));

                Error error;
                Obj   mS; const Obj& S = mS;
                rc = mX.setField(&mS, &error, f7, f6, f5, f4, f3, f2, f1, VA);
                ASSERT(!rc);
                ASSERT(FLD_TYPE == S.dataType());
                ASSERT(&fd      == S.fieldDef());
                LOOP3_ASSERT(LINE, VA, S.asElemRef(), VA == S.asElemRef());
                ASSERT(!S.isNull());

                Obj   mT; const Obj& T = mT;
                rc = mX.fieldById(&mT, &error, 0);
                ASSERT(!rc);
                ASSERT(!T.isNull());

                rc = mX.fieldById(&mT, &error, NUM_RECS);
                ASSERT(rc);

                Obj   mV; const Obj& V = mV;
                rc = mX.fieldByIndex(&mV, &error, 0);
                ASSERT(!rc);
                ASSERT(!V.isNull());

                rc = mX.fieldByIndex(&mV, &error, NUM_RECS);
                ASSERT(rc);

                mS.reset();
                rc = X.getField(&mS, &error, false, f7, f6, f5,
                                f4, f3, f2, f1);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, VA, S.asElemRef(), VA == S.asElemRef());
                ASSERT(!S.isNull());

                mS.makeNull();
                ASSERT(FLD_TYPE == S.dataType());
                ASSERT(&fd      == S.fieldDef());
                ASSERT(isUnset(S.asElemRef()));
                ASSERT(S.isNull());

                rc = mX.setField(&mS, &error, f7, f6, f5, f4, f3, f2, f1, VB);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, VB, S.asElemRef(), VB == S.asElemRef());
                ASSERT(!S.isNull());

                mS.reset();
                rc = X.getField(&mS, &error, false, f7, f6, f5,
                                f4, f3, f2, f1);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, VB, S.asElemRef(), VB == S.asElemRef());
                ASSERT(!S.isNull());

                // Test that scalar types are returned by reference
                mS.setValue(&error, VA);
                ASSERT(VA == S.asElemRef());
                Obj   mB; const Obj& B = mB;
                rc = X.getField(&mB, &error, false, f7, f6, f5,
                                f4, f3, f2, f1);
                ASSERT(!rc);
                ASSERT(VA == B.asElemRef());
                ASSERT(!S.isNull());
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

                ASSERT(X.hasField(f8));
                ASSERT(X.hasFieldById(0));
                ASSERT(X.hasFieldByIndex(0));
                ASSERT(!X.hasField(errorField));
                ASSERT(!X.hasFieldById(NUM_RECS));
                ASSERT(!X.hasFieldByIndex(NUM_RECS));

                Error error;
                Obj   mS; const Obj& S = mS;
                rc = mX.setField(&mS, &error, f8, f7, f6, f5, f4,
                                 f3, f2, f1, VA);
                ASSERT(!rc);
                ASSERT(FLD_TYPE == S.dataType());
                ASSERT(&fd      == S.fieldDef());
                LOOP3_ASSERT(LINE, VA, S.asElemRef(), VA == S.asElemRef());
                ASSERT(!S.isNull());

                Obj   mT; const Obj& T = mT;
                rc = mX.fieldById(&mT, &error, 0);
                ASSERT(!rc);
                ASSERT(!T.isNull());

                rc = mX.fieldById(&mT, &error, NUM_RECS);
                ASSERT(rc);

                Obj   mV; const Obj& V = mV;
                rc = mX.fieldByIndex(&mV, &error, 0);
                ASSERT(!rc);
                ASSERT(!V.isNull());

                rc = mX.fieldByIndex(&mV, &error, NUM_RECS);
                ASSERT(rc);

                mS.reset();
                rc = X.getField(&mS, &error, false, f8, f7, f6, f5,
                                f4, f3, f2, f1);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, VA, S.asElemRef(), VA == S.asElemRef());
                ASSERT(!S.isNull());

                mS.makeNull();
                ASSERT(FLD_TYPE == S.dataType());
                ASSERT(&fd      == S.fieldDef());
                ASSERT(isUnset(S.asElemRef()));
                ASSERT(S.isNull());

                rc = mX.setField(&mS, &error, f8, f7, f6, f5, f4,
                                 f3, f2, f1, VB);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, VB, S.asElemRef(), VB == S.asElemRef());
                ASSERT(!S.isNull());

                mS.reset();
                rc = X.getField(&mS, &error, false, f8, f7, f6, f5,
                                f4, f3, f2, f1);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, VB, S.asElemRef(), VB == S.asElemRef());
                ASSERT(!S.isNull());

                // Test that scalar types are returned by reference
                mS.setValue(&error, VA);
                ASSERT(VA == S.asElemRef());
                Obj   mB; const Obj& B = mB;
                rc = X.getField(&mB, &error, false, f8, f7, f6, f5,
                                f4, f3, f2, f1);
                ASSERT(!rc);
                ASSERT(VA == B.asElemRef());
                ASSERT(!S.isNull());
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

                ASSERT(X.hasField(f9));
                ASSERT(X.hasFieldById(0));
                ASSERT(X.hasFieldByIndex(0));
                ASSERT(!X.hasField(errorField));
                ASSERT(!X.hasFieldById(NUM_RECS));
                ASSERT(!X.hasFieldByIndex(NUM_RECS));

                Error error;
                Obj   mS; const Obj& S = mS;
                rc = mX.setField(&mS, &error, f9, f8, f7, f6, f5, f4,
                                 f3, f2, f1, VA);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, VA, S.asElemRef(), VA == S.asElemRef());
                ASSERT(!S.isNull());

                Obj   mT; const Obj& T = mT;
                rc = mX.fieldById(&mT, &error, 0);
                ASSERT(!rc);
                ASSERT(!T.isNull());

                rc = mX.fieldById(&mT, &error, NUM_RECS);
                ASSERT(rc);

                Obj   mV; const Obj& V = mV;
                rc = mX.fieldByIndex(&mV, &error, 0);
                ASSERT(!rc);
                ASSERT(!V.isNull());

                rc = mX.fieldByIndex(&mV, &error, NUM_RECS);
                ASSERT(rc);

                mS.reset();
                rc = X.getField(&mS, &error, false, f9, f8, f7, f6, f5,
                                f4, f3, f2, f1);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, VA, S.asElemRef(), VA == S.asElemRef());
                ASSERT(!S.isNull());

                mS.makeNull();
                ASSERT(FLD_TYPE == S.dataType());
                ASSERT(&fd      == S.fieldDef());
                ASSERT(isUnset(S.asElemRef()));
                ASSERT(S.isNull());

                rc = mX.setField(&mS, &error, f9, f8, f7, f6, f5, f4,
                                 f3, f2, f1, VB);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, VB, S.asElemRef(), VB == S.asElemRef());
                ASSERT(!S.isNull());

                mS.reset();
                rc = X.getField(&mS, &error, false, f9, f8, f7, f6, f5,
                                f4, f3, f2, f1);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, VB, S.asElemRef(), VB == S.asElemRef());
                ASSERT(!S.isNull());

                // Test that scalar types are returned by reference
                mS.setValue(&error, VA);
                ASSERT(VA == S.asElemRef());
                Obj   mB; const Obj& B = mB;
                rc = X.getField(&mB, &error, false, f9, f8, f7, f6, f5,
                                f4, f3, f2, f1);
                ASSERT(!rc);
                ASSERT(VA == B.asElemRef());
                ASSERT(!S.isNull());
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

                ASSERT(X.hasField(f10));
                ASSERT(X.hasFieldById(0));
                ASSERT(X.hasFieldByIndex(0));
                ASSERT(!X.hasField(errorField));
                ASSERT(!X.hasFieldById(NUM_RECS));
                ASSERT(!X.hasFieldByIndex(NUM_RECS));

                Error error;
                Obj   mS; const Obj& S = mS;
                rc = mX.setField(&mS, &error, f10, f9, f8, f7, f6, f5, f4,
                                 f3, f2, f1, VA);
                ASSERT(!rc);
                ASSERT(FLD_TYPE == S.dataType());
                ASSERT(&fd      == S.fieldDef());
                LOOP3_ASSERT(LINE, VA, S.asElemRef(), VA == S.asElemRef());
                ASSERT(!S.isNull());

                Obj   mT; const Obj& T = mT;
                rc = mX.fieldById(&mT, &error, 0);
                ASSERT(!rc);
                ASSERT(!T.isNull());

                rc = mX.fieldById(&mT, &error, NUM_RECS);
                ASSERT(rc);

                Obj   mV; const Obj& V = mV;
                rc = mX.fieldByIndex(&mV, &error, 0);
                ASSERT(!rc);
                ASSERT(!V.isNull());

                rc = mX.fieldByIndex(&mV, &error, NUM_RECS);
                ASSERT(rc);

                mS.reset();
                rc = X.getField(&mS, &error, false, f10, f9, f8, f7, f6, f5,
                                f4, f3, f2, f1);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, VA, S.asElemRef(), VA == S.asElemRef());
                ASSERT(!S.isNull());

                mS.makeNull();
                ASSERT(FLD_TYPE == S.dataType());
                ASSERT(&fd      == S.fieldDef());
                ASSERT(isUnset(S.asElemRef()));
                ASSERT(S.isNull());

                rc = mX.setField(&mS, &error, f10, f9, f8, f7, f6, f5, f4,
                                 f3, f2, f1, VB);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, VB, S.asElemRef(), VB == S.asElemRef());
                ASSERT(!S.isNull());

                mS.reset();
                rc = X.getField(&mS, &error, false, f10, f9, f8, f7, f6, f5,
                                f4, f3, f2, f1);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, VB, S.asElemRef(), VB == S.asElemRef());
                ASSERT(!S.isNull());

                // Test that scalar types are returned by reference
                mS.setValue(&error, VA);
                ASSERT(VA == S.asElemRef());
                Obj   mB; const Obj& B = mB;
                rc = X.getField(&mB, &error, false, f10, f9, f8, f7, f6, f5,
                                f4, f3, f2, f1);
                ASSERT(!rc);
                ASSERT(VA == B.asElemRef());
                ASSERT(!S.isNull());

                const char errFld[] = "ErrorField";
                Error err;
                Obj mZ;

                // Test error output
                {
                    rc = mX.setField(&mZ, &err, errFld, f9, f8, f7, f6, f5,
                                     f4, f3, f2, f1, VA);
                    ASSERT(rc);
                    LOOP_ASSERT(err.code(),
                                Error::BCEM_ERR_BAD_FIELDNAME == err.code());

                    rc = mX.setField(&mZ, &err, f10, errFld, f8, f7, f6, f5,
                                     f4, f3, f2, f1, VA);
                    ASSERT(rc);
                    LOOP_ASSERT(err.code(),
                                Error::BCEM_ERR_BAD_FIELDNAME == err.code());

                    rc = mX.setField(&mZ, &err, f10, f9, errFld, f7, f6, f5,
                                     f4, f3, f2, f1, VA);
                    ASSERT(rc);
                    LOOP_ASSERT(err.code(),
                                Error::BCEM_ERR_BAD_FIELDNAME == err.code());

                    rc = mX.setField(&mZ, &err, f10, f9, f8, errFld, f6, f5,
                                     f4, f3, f2, f1, VA);
                    ASSERT(rc);
                    LOOP_ASSERT(err.code(),
                                Error::BCEM_ERR_BAD_FIELDNAME == err.code());

                    rc = mX.setField(&mZ, &err, f10, f9, f8, f7, errFld, f5,
                                     f4, f3, f2, f1, VA);
                    ASSERT(rc);
                    LOOP_ASSERT(err.code(),
                                Error::BCEM_ERR_BAD_FIELDNAME == err.code());

                    rc = mX.setField(&mZ, &err, f10, f9, f8, f7, f6, errFld,
                                     f4, f3, f2, f1, VA);
                    ASSERT(rc);
                    LOOP_ASSERT(err.code(),
                                Error::BCEM_ERR_BAD_FIELDNAME == err.code());

                    rc = mX.setField(&mZ, &err, f10, f9, f8, f7, f6, f5,
                                     errFld, f3, f2, f1, VA);
                    ASSERT(rc);
                    LOOP_ASSERT(err.code(),
                                Error::BCEM_ERR_BAD_FIELDNAME == err.code());

                    rc = mX.setField(&mZ, &err, f10, f9, f8, f7, f6, f5,
                                     f4, errFld, f2, f1, VA);
                    ASSERT(rc);
                    LOOP_ASSERT(err.code(),
                                Error::BCEM_ERR_BAD_FIELDNAME == err.code());

                    rc = mX.setField(&mZ, &err, f10, f9, f8, f7, f6, f5,
                                     f4, f3, errFld, f1, VA);
                    ASSERT(rc);
                    LOOP_ASSERT(err.code(),
                                Error::BCEM_ERR_BAD_FIELDNAME == err.code());

                    rc =  mX.setField(&mZ, &err, f10, f9, f8, f7, f6, f5,
                                      f4, f3, f2, errFld, VA);
                    ASSERT(rc);
                    LOOP_ASSERT(err.code(),
                                Error::BCEM_ERR_BAD_FIELDNAME == err.code());
                }
              } break;
              default:
                ASSERT(0);
            }

            destroyAggData(&mY, &t);
            destroyAggData(&mX, &t);
        }
      } break;
      case 3: {
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
        //   void setField(bcem_AggregateRaw   *field,
        //                 bcem_AggregateError *errorDescription,
        //                 NameOrIndex          fieldOrIdx1,
        //                 const VALTYPE&       value) const;
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
        //                       bcem_AggregateError        *errorDescription,
        //                       const NameOrIndex&          fieldOrIdx,
        //                       bcem_Aggregate_NameOrIndex  fieldOrIdx);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING PRIMARY MANIPULATORS"
                          << "\n============================"
                          << bsl::endl;

        {
            // Testing setting the value on a VOID aggregate
            Obj mX; const Obj& X = mX;
            Obj mY; const Obj& Y = mY;
            Obj mZ; const Obj& Z = mZ;
            ASSERT(Obj::areEquivalent(X, Y));
            ASSERT(Obj::areEquivalent(X, Z));

            Error error1 ,error2, error3;
            int rc = mX.setValue(&error1, 1);
            ASSERT(rc);
            LOOP_ASSERT(error1.code(), Error::BCEM_SUCCESS != error1.code());
            ASSERT(Obj::areEquivalent(X, Y));

            rc = mX.setValue(&error2, Z);
            // This succeeds as conversion from VOID to VOID is valid
            ASSERT(!rc);
            LOOP_ASSERT(error2.code(), Error::BCEM_SUCCESS == error2.code());
            ASSERT(Obj::areEquivalent(X, Y));

            rc = mX.setValue(&error3, Z.asElemRef());
            // This succeeds as conversion from VOID to VOID is valid
            ASSERT(!rc);
            LOOP_ASSERT(error3.code(), Error::BCEM_SUCCESS == error3.code());
            ASSERT(Obj::areEquivalent(X, Y));
        }

        if (veryVerbose) { cout << "\tTest setting field by name"
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

                bslma_TestAllocator t(veryVeryVerbose);
                Obj mX; const Obj& X = mX;
                int rc = ggAggData(&mX, r, &t);
                ASSERT(!rc);

                int nullnessWord = 0;
                Obj mN; const Obj& N = mN;
                rc = ggAggData(&mN, r, &t);
                ASSERT(!rc);
                mN.setTopLevelAggregateNullnessPointer(&nullnessWord);
                mN.makeNull();

                if (veryVerbose) { T_ P_(LINE) P(X) P(N)
                                   T_ T_ P(s) P(CEA) P(EA) };

                Error error;
                Obj mS; const Obj& S = mS;
                if (RecDef::BDEM_CHOICE_RECORD == r.recordType()) {
                    rc = mX.makeSelection(&mS, &error, fldName);
                    ASSERT(!rc);
                }

                if (!ET::BDEM_LIST == TYPE) {
                    rc = X.getField(&mS, &error, false, fldName);
                    ASSERT(!rc);
                    if (veryVerbose) { T_ P(S) };
                    LOOP2_ASSERT(LINE, S, S.isNull() == isNull);
                }

                // Test setField with bdem_ConstElemRef and bdem_ElemRef
                rc = mX.setField(&mS, &error, fldName, CEA);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, CEA, S.asElemRef(),
                             CEA == S.asElemRef());
                LOOP_ASSERT(S, !S.isNull());

                rc = X.getField(&mS, &error, false, fldName);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, CEA, S.asElemRef(),
                             CEA == S.asElemRef());
                LOOP_ASSERT(S, !S.isNull());

                mS.makeNull();

                rc = X.getField(&mS, &error, false, fldName);
                ASSERT(!rc);
                LOOP_ASSERT(S, S.isNull());

                rc = mX.setField(&mS, &error, fldName, CEB);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, CEB, S.asElemRef(),
                             CEB == S.asElemRef());
                LOOP_ASSERT(S, !S.isNull());

                rc = X.getField(&mS, &error, false, fldName);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, CEB, S.asElemRef(),
                             CEB == S.asElemRef());
                LOOP_ASSERT(S, !S.isNull());

                rc = mX.setField(&mS, &error, fldName, NULL_CER);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, NULL_CER, S.asElemRef(),
                             NULL_CER == S.asElemRef());
                LOOP_ASSERT(S, S.isNull());

                rc = X.getField(&mS, &error, false, fldName);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, NULL_CER, S.asElemRef(),
                             NULL_CER == S.asElemRef());
                LOOP_ASSERT(S, S.isNull());

                rc = mX.setField(&mS, &error, fldName, EA);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, EA, S.asElemRef(),
                             EA == S.asElemRef());
                LOOP_ASSERT(S, !S.isNull());

                rc = X.getField(&mS, &error, false, fldName);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, EA, S.asElemRef(),
                             EA == S.asElemRef());
                LOOP_ASSERT(S, !S.isNull());

                mS.makeNull();

                rc = X.getField(&mS, &error, false, fldName);
                ASSERT(!rc);
                LOOP_ASSERT(S, S.isNull());

                rc = mX.setField(&mS, &error, fldName, EB);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, EB, S.asElemRef(),
                             EB == S.asElemRef());
                LOOP_ASSERT(S, !S.isNull());

                rc = X.getField(&mS, &error, false, fldName);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, EB, S.asElemRef(),
                             EB == S.asElemRef());
                LOOP_ASSERT(S, !S.isNull());

                rc = mX.setField(&mS, &error, fldName, NULL_ER);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, NULL_ER, S.asElemRef(),
                             NULL_ER == S.asElemRef());
                LOOP_ASSERT(S, S.isNull());

                rc = X.getField(&mS, &error, false, fldName);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, NULL_ER, S.asElemRef(),
                             NULL_ER == S.asElemRef());
                LOOP_ASSERT(S, S.isNull());

                // Test setField with values of the field types
                SetFieldFunctor f1(&mX, fldName);

                funcVisitor(&f1, CEA);

                rc = X.getField(&mS, &error, false, fldName);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, CEA, S.asElemRef(),
                             CEA == S.asElemRef());
                LOOP_ASSERT(S, !S.isNull());

                mS.makeNull();

                rc = X.getField(&mS, &error, false, fldName);
                ASSERT(!rc);
                LOOP_ASSERT(S, S.isNull());

                funcVisitor(&f1, CEB);

                rc = X.getField(&mS, &error, false, fldName);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, CEB, S.asElemRef(),
                             CEB == S.asElemRef());
                LOOP_ASSERT(S, !S.isNull());

                mS.makeNull();

                rc = X.getField(&mS, &error, false, fldName);
                ASSERT(!rc);
                LOOP_ASSERT(S, S.isNull());

                if (ENUM_DEF) {
                    if (TYPE == ET::BDEM_INT) {
                        // Test setField: INT enumeration as STRING
                        rc = mX.setField(&mS, &error, fldName, N06);
                        ASSERT(!rc);
                        LOOP3_ASSERT(LINE, CEN, S.asElemRef(),
                                     CEN == S.asElemRef());
                        LOOP_ASSERT(S, !S.isNull());

                        rc = X.getField(&mS, &error, false, fldName);
                        ASSERT(!rc);
                        LOOP3_ASSERT(LINE, CEN, S.asElemRef(),
                                     CEN == S.asElemRef());
                        LOOP_ASSERT(S, !S.isNull());

                        mS.makeNull();

                        rc = X.getField(&mS, &error, false, fldName);
                        ASSERT(!rc);
                        LOOP_ASSERT(S, S.isNull());

                        rc = mX.setField(&mS, &error, fldName, A06);
                        ASSERT(!rc);
                        LOOP3_ASSERT(LINE, CEA, S.asElemRef(),
                                     CEA == S.asElemRef());
                        LOOP_ASSERT(S, !S.isNull());

                        rc = X.getField(&mS, &error, false, fldName);
                        ASSERT(!rc);
                        LOOP3_ASSERT(LINE, CEA, S.asElemRef(),
                                     CEA == S.asElemRef());
                        LOOP_ASSERT(S, !S.isNull());

                        rc = mX.setField(&mS, &error, fldName, B06);
                        ASSERT(!rc);
                        LOOP3_ASSERT(LINE, CEB, S.asElemRef(),
                                     CEB == S.asElemRef());
                        LOOP_ASSERT(S, !S.isNull());

                        rc = X.getField(&mS, &error, false, fldName);
                        ASSERT(!rc);
                        LOOP3_ASSERT(LINE, CEB, S.asElemRef(),
                                     CEB == S.asElemRef());
                        LOOP_ASSERT(S, !S.isNull());
                    }
                    else if (TYPE == ET::BDEM_STRING) {
                        // Test setField: STRING enumeration as INT
                        rc = mX.setField(&mS, &error, fldName, N02);
                        ASSERT(!rc);
                        LOOP3_ASSERT(LINE, CEN, S.asElemRef(),
                                     CEN == S.asElemRef());
                        LOOP_ASSERT(S, !S.isNull());

                        rc = X.getField(&mS, &error, false, fldName);
                        ASSERT(!rc);
                        LOOP3_ASSERT(LINE, CEN, S.asElemRef(),
                                     CEN == S.asElemRef());
                        LOOP_ASSERT(S, !S.isNull());

                        mS.makeNull();

                        rc = X.getField(&mS, &error, false, fldName);
                        ASSERT(!rc);
                        LOOP_ASSERT(S, S.isNull());

                        rc = mX.setField(&mS, &error, fldName, A02);
                        ASSERT(!rc);
                        LOOP3_ASSERT(LINE, CEA, S.asElemRef(),
                                     CEA == S.asElemRef());
                        LOOP_ASSERT(S, !S.isNull());

                        rc = X.getField(&mS, &error, false, fldName);
                        ASSERT(!rc);
                        LOOP3_ASSERT(LINE, CEA, S.asElemRef(),
                                     CEA == S.asElemRef());
                        LOOP_ASSERT(S, !S.isNull());

                        rc = mX.setField(&mS, &error, fldName, B02);
                        ASSERT(!rc);
                        LOOP3_ASSERT(LINE, CEB, S.asElemRef(),
                                     CEB == S.asElemRef());
                        LOOP_ASSERT(S, !S.isNull());

                        rc = X.getField(&mS, &error, false, fldName);
                        ASSERT(!rc);
                        LOOP3_ASSERT(LINE, CEB, S.asElemRef(),
                                     CEB == S.asElemRef());
                        LOOP_ASSERT(S, !S.isNull());
                    }
                }

                // Test setField with an aggregate value
                Obj mY; const Obj& Y = mY;
                rc = ggAggData(&mY, r, &t);
                ASSERT(!rc);

                if (RecDef::BDEM_CHOICE_RECORD == r.recordType()) {
                    rc = mY.makeSelection(&mS, &error, fldName);
                    ASSERT(!rc);
                }

                rc = mY.setField(&mS, &error, fldName, CEA);
                ASSERT(!rc);

                Obj mT; const Obj& T = mT;
                rc = Y.getField(&mT, &error, false, fldName);
                ASSERT(!rc);

                rc = mX.setField(&mS, &error, fldName, T);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, CEA, S.asElemRef(),
                             CEA == S.asElemRef());
                LOOP_ASSERT(S, !S.isNull());

                rc = X.getField(&mS, &error, false, fldName);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, CEA, S.asElemRef(),
                             CEA == S.asElemRef());
                LOOP_ASSERT(S, !S.isNull());

                mS.makeNull();

                rc = X.getField(&mS, &error, false, fldName);
                ASSERT(!rc);
                LOOP_ASSERT(S, S.isNull());

                Obj mZ; const Obj& Z = mZ;
                rc = ggAggData(&mZ, r, &t);
                ASSERT(!rc);

                if (RecDef::BDEM_CHOICE_RECORD == r.recordType()) {
                    rc = mZ.makeSelection(&mS, &error, fldName);
                    ASSERT(!rc);
                }

                rc = mZ.setField(&mT, &error, fldName, CEB);
                ASSERT(!rc);

                rc = mX.setField(&mS, &error, fldName, T);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, CEB, S.asElemRef(),
                             CEB == S.asElemRef());
                LOOP_ASSERT(S, !S.isNull());

                rc = X.getField(&mS, &error, false, fldName);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, CEB, S.asElemRef(),
                             CEB == S.asElemRef());
                LOOP_ASSERT(S, !S.isNull());

                mS.makeNull();

                rc = X.getField(&mS, &error, false, fldName);
                ASSERT(!rc);
                LOOP_ASSERT(S, S.isNull());

                // Test empty string if aggregate stores a choice
                if (RecDef::BDEM_CHOICE_RECORD == r.recordType()) {
                    rc = mX.setField(&mS, &error, "", CEA);
                    ASSERT(!rc);
                    LOOP3_ASSERT(LINE, CEA, S.asElemRef(),
                                 CEA == S.asElemRef());
                    LOOP_ASSERT(S, !S.isNull());

                    mS.makeNull();

                    rc = X.getField(&mS, &error, false, fldName);
                    ASSERT(!rc);
                    LOOP_ASSERT(S, S.isNull());
                }

                // Test setValue with bdem_ConstElemRef and bdem_ElemRef

                rc = X.getField(&mS, &error, false, fldName);
                ASSERT(!rc);

                Obj mXfld(S); const Obj& Xfld = mXfld;
                rc = Xfld.setValue(&error, CEA);
                ASSERT(!rc);
                ASSERT(CEA == Xfld.asElemRef());
                ASSERT(!Xfld.isNull());

                rc = Xfld.setValue(&error, NULL_CER);
                ASSERT(!rc);
                ASSERT(Xfld.isNull());

                rc = Xfld.setValue(&error, CEB);
                ASSERT(!rc);
                ASSERT(CEB == Xfld.asElemRef());
                ASSERT(!Xfld.isNull());

                rc = Xfld.setValue(&error, EA);
                ASSERT(!rc);
                ASSERT(EA == Xfld.asElemRef());
                ASSERT(!Xfld.isNull());

                rc = Xfld.setValue(&error, NULL_ER);
                ASSERT(!rc);
                ASSERT(Xfld.isNull());

                rc = Xfld.setValue(&error, EB);
                ASSERT(!rc);
                ASSERT(EB == Xfld.asElemRef());
                ASSERT(!Xfld.isNull());

                // Test setValue with values of the field types
                SetValueFunctor v1(&mXfld);
                funcVisitor(&v1, CEA);
                ASSERT(CEA == Xfld.asElemRef());
                ASSERT(!Xfld.isNull());

                funcVisitor(&v1, CEB);
                ASSERT(CEB == Xfld.asElemRef());
                ASSERT(!Xfld.isNull());

                // Test setValue with an aggregate value
                rc = mS.setValue(&error, CEA);
                ASSERT(!rc);

                rc = Xfld.setValue(&error, S);
                ASSERT(!rc);
                ASSERT(CEA == Xfld.asElemRef());
                ASSERT(!Xfld.isNull());

                Xfld.makeNull();
                ASSERT(Xfld.isNull());

                if (ENUM_DEF) {
                    if (TYPE == ET::BDEM_INT) {
                        // Test setValue: INT enumeration as STRING
                        rc = Xfld.setValue(&error, N06);
                        ASSERT(!rc);
                        ASSERT(CEN == Xfld.asElemRef());
                        ASSERT(!Xfld.isNull());

                        Xfld.makeNull();
                        ASSERT(Xfld.isNull());

                        rc = Xfld.setValue(&error, A06);
                        ASSERT(CEA == Xfld.asElemRef());
                        ASSERT(!Xfld.isNull());

                        Xfld.makeNull();
                        ASSERT(Xfld.isNull());

                        rc = Xfld.setValue(&error, B06);
                        ASSERT(CEB == Xfld.asElemRef());
                        ASSERT(!Xfld.isNull());

                        Xfld.makeNull();
                        ASSERT(Xfld.isNull());
                    }
                    else if (TYPE == ET::BDEM_STRING) {
                        // Test setValue: STRING enumeration as INT
                        rc = Xfld.setValue(&error, N02);
                        ASSERT(!rc);
                        ASSERT(CEN == Xfld.asElemRef());
                        ASSERT(!Xfld.isNull());

                        Xfld.makeNull();
                        ASSERT(Xfld.isNull());

                        rc = Xfld.setValue(&error, A02);
                        ASSERT(CEA == Xfld.asElemRef());
                        ASSERT(!Xfld.isNull());

                        Xfld.makeNull();
                        ASSERT(Xfld.isNull());

                        rc = Xfld.setValue(&error, B02);
                        ASSERT(CEB == Xfld.asElemRef());
                        ASSERT(!Xfld.isNull());

                        Xfld.makeNull();
                        ASSERT(Xfld.isNull());
                    }
                }

                // Test error conditions and output

                // Test with an index value
                Obj err1, err2, err3;
                rc = mX.setField(&err1, &error, 0, CEA);
                ASSERT(rc);
                LOOP_ASSERT(error.code(),
                            Error::BCEM_ERR_NOT_AN_ARRAY == error.code());

                // Test with invalid field name
                const char *errFldName = "ErrorField";
                rc = mX.setField(&err2, &error, errFldName, CEA);
                ASSERT(rc);
                LOOP_ASSERT(error.code(),
                            Error::BCEM_ERR_BAD_FIELDNAME == error.code());

                // Test that calling a field on an empty aggregate fails
                Obj mV(X);
                mV.reset();
                rc = mV.setField(&err3, &error, fldName, CEA);
                ASSERT(rc);
                LOOP_ASSERT(error.code(),
                            Error::BCEM_ERR_NOT_A_RECORD == error.code());

                destroyAggData(&mZ, &t);
                destroyAggData(&mY, &t);
                destroyAggData(&mX, &t);
                destroyAggData(&mN, &t);
            }
        }
      } break;
      case 2: {
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
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING HELPER FUNCTIONS" << endl
                          << "========================" << endl;

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
      } break;
      case 1: {
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
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

        if (verbose) cout << "Testing scalar constructors" << endl;
        {
            bslma_TestAllocator ta(veryVeryVerbose);

            Obj agg1;
            ASSERT(agg1.dataType() == ET::BDEM_VOID);
            ASSERT(agg1.isNull());
            if (verbose) P(agg1);

            int intData = 55;
            Obj agg2;
            agg2.setDataType(ET::BDEM_INT);
            agg2.setDataPointer(&intData);
            ASSERT(agg2.dataType() == ET::BDEM_INT);
            ASSERT(!agg2.isNull());
            ASSERT(agg2.asInt() == intData);
            ASSERT(agg2.asString() == "55");
            if (verbose) P(agg2);

            string stringData = "";
            Obj agg3;
            agg3.setDataType(ET::BDEM_STRING);
            agg3.setDataPointer(&stringData);
            ASSERT(agg3.dataType() == ET::BDEM_STRING);
            ASSERT(!agg3.isNull());
            ASSERT(agg3.asString() == stringData);
            if (verbose) P(agg3);
        }

        bslma_TestAllocator sa;
        bcema_SharedPtr<bdem_Schema> schema(new (sa) bdem_Schema(&sa), &sa);

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

        bdem_RecordDef *table1Row = schema->createRecord("Table1Row");
        table1Row->appendField(ET::BDEM_STRING, "StringField");
        table1Row->appendField(ET::BDEM_DOUBLE, "DoubleField");

        bdem_RecordDef *choice1 = schema->createRecord(
            "Choice1", bdem_RecordDef::BDEM_CHOICE_RECORD);
        choice1->appendField(ET::BDEM_STRING, "NameSelection");
        choice1->appendField(ET::BDEM_INT,    "IdSelection");

        string stringValue = "Hello";
        vector<string> stringArray;
        stringArray.push_back(stringValue);
        double doubleValue = 3.4;
        vector<double> doubleArray;
        doubleArray.push_back(doubleValue);
        bdet_Date      date(2000, 10, 10);

        bdem_List      list2;
        list2.appendStringArray(stringArray);
        list2.appendDoubleArray(doubleArray);
        list2.appendDate(date);

        bdem_List      list1;
        list1.appendString(stringValue);
        list1.appendDouble(doubleValue);
        list1.appendList(list2);
        list1.appendList(list1);

        bdem_List      list3;
        list3.appendString(stringValue);
        list3.appendDouble(doubleValue);

        vector<ET::Type> ct;
        ct.push_back(ET::BDEM_STRING);
        ct.push_back(ET::BDEM_DOUBLE);
        bdem_Table     table(ct);
        table.appendRow(list3);

        ct.clear();
        ct.push_back(ET::BDEM_STRING);
        ct.push_back(ET::BDEM_INT);
        bdem_Choice choice(ct);
        choice.makeSelection(0).theModifiableString() = stringValue;
        {
            bslma_TestAllocator ta;

            if (verbose) cout << "Testing record def constructor"
                              << bsl::endl;
            {
                Obj agg1;
                agg1.setSchemaPointer(schema.ptr());
                agg1.setRecordDefPointer(schema->lookupRecord("Level1"));
                agg1.setDataType(ET::BDEM_LIST);
                agg1.setDataPointer(&list1);
                ASSERT(!agg1.isNull());

                Obj   agg2, agg3, agg4, agg5, agg6;
                Error error;
                int rc = agg1.getField(&agg2, &error, false, "Double1.2");
                ASSERT(!rc);
                double dbl1 = agg2.asDouble();
                ASSERT(3.4 == dbl1);

                rc = agg1.setField(&agg3,
                                   &error,
                                   "Double1.2",
                                   1.1);
                ASSERT(!rc);
                ASSERT(agg2.asDouble() == agg3.asDouble());

                double dbl2 = agg1.fieldByIndex(&agg4, &error, 1);
                ASSERT(1.1 == agg4.asDouble());
                ASSERT(!rc);

                bdet_Date d(2010, 1, 1);
                rc = agg1.setField(&agg5,
                                   &error,
                                   "List1.3",
                                   "Date2.3",
                                   d);
                ASSERT(!rc);
                rc = agg1.getField(&agg6, &error, false, "List1.3", "Date2.3");
                ASSERT(!rc);
                bdet_Date date1 = agg6.asDate();
                ASSERT(d == date1);
            }

            if (verbose) cout << "Testing list operations" << bsl::endl;
            {
                Obj agg1;
                agg1.setSchemaPointer(schema.ptr());
                agg1.setRecordDefPointer(schema->lookupRecord("Level1"));
                agg1.setDataType(ET::BDEM_LIST);
                agg1.setDataPointer(&list1);

                Obj   agg2, agg3, agg4;
                Error error;
                int rc = agg1.getField(&agg2, &error, false, "List1.4");
                ASSERT(!rc);
                ASSERT(!agg2.isNull());
                rc = agg1.getField(&agg3,
                                   &error,
                                   false,
                                   "List1.4",
                                   "String1.1");
                ASSERT(!rc);
                ASSERT(!agg3.isNull());
                ASSERT(!agg3.setValue(&error, "List in list"));

                rc = agg1.getField(&agg4,
                                   &error,
                                   false,
                                   "List1.4",
                                   "String1.1");
                ASSERT(!rc);
                ASSERT("List in list" == agg4.asString());
            }

            if (verbose) cout << "Testing array operations" << bsl::endl;
            {
                bslma_TestAllocator sa;
                bdem_List data1(list1, &sa);
                bdem_List data2(list2, &sa);
                Obj agg1;
                agg1.setSchemaPointer(schema.ptr());
                agg1.setRecordDefPointer(schema->lookupRecord("Level2"));
                agg1.setDataType(ET::BDEM_LIST);
                agg1.setDataPointer(&data2);

                Obj   agg2, agg3, agg4;
                Error error;
                int rc = agg1.getField(&agg2, &error, false, "StringArray2.1");
                ASSERT(!rc);
                ASSERT(!agg2.isNull());
                rc = agg2.resize(&error, 2);
                ASSERT(!rc);
                ASSERT(2 == agg2.length());
                rc = agg2.arrayItem(&agg3, &error, 0);
                ASSERT(!rc);
                rc = agg3.setValue(&error, "Hello World");
                ASSERT(!rc);
                rc = agg2.arrayItem(&agg4, &error, 0);
                ASSERT(!rc);
                LOOP_ASSERT(agg4.asString(), "Hello World" == agg4.asString());

                Obj agg5;
                agg5.setSchemaPointer(schema.ptr());
                agg5.setRecordDefPointer(schema->lookupRecord("Level1"));
                agg5.setDataType(ET::BDEM_LIST);
                agg5.setDataPointer(&data1);

                Obj agg6, agg7, agg8;
                rc = agg5.getField(&agg6,
                                   &error,
                                   false,
                                   "List1.3",
                                   "DoubleArray2.2");
                ASSERT(!rc);
                ASSERT(1 == agg6.length());
                rc = agg6.insertItem(&agg7, &error, 0, 3.14);
                ASSERT(!rc);
                ASSERT(2 == agg6.length());
                rc = agg6.insertItem(&agg8, &error, 1, 1.2);
                ASSERT(!rc);

                agg7.setValue(&error, 100.23);
                ASSERT(100.23 == agg7.asDouble());
                ASSERT(1.2    == agg8.asDouble());

                agg6.resize(&error, 4);
                ASSERT(4      == agg6.length());
                ASSERT(100.23 == agg7.asDouble());
                ASSERT(1.2    == agg8.asDouble());

                if (veryVerbose) P(agg6);
                agg6.removeItems(&error, 1, 2);
                ASSERT(2 == agg6.length());
            }
        }

        {
            bslma_TestAllocator ta;

            if (verbose) cout << "Testing table operations" << bsl::endl;

            Obj agg1;
            agg1.setSchemaPointer(schema.ptr());
            agg1.setRecordDefPointer(schema->lookupRecord("Table1Row"));
            agg1.setDataType(ET::BDEM_TABLE);
            agg1.setDataPointer(&table);
            ASSERT(1 == agg1.length());

            Obj   agg2, agg3, agg4, agg5;
            Error error;

            int rc = agg1.insertItems(&error, 0, 2);
            ASSERT(!rc);
            ASSERT(3 == agg1.length());

            rc = agg1.setField(&agg2, &error, 0, "StringField", "Hello");
            ASSERT(!rc);
            ASSERT("Hello" == agg2.asString());
            rc = agg2.setValue(&error, "Hello World");
            ASSERT(!rc);

            rc = agg1.setField(&agg3, &error, 0, "DoubleField", 1.2);
            ASSERT(!rc);
            ASSERT(1.2 == agg3.asDouble());
            rc = agg3.setValue(&error, 3.4);
            ASSERT(!rc);

            Obj tmpAgg;
            tmpAgg.setSchemaPointer(schema.ptr());
            tmpAgg.setRecordDefPointer(schema->lookupRecord("Table1Row"));
            tmpAgg.setDataType(ET::BDEM_LIST);
            tmpAgg.setDataPointer(&list3);

            rc = agg1.insertItem(&agg5, &error, 0, tmpAgg);
            ASSERT(!rc);
            ASSERT(4 == agg1.length());

            Obj   agg6, agg7, agg8, agg9;
            rc = agg1.getField(&agg6, &error, false, 0, "StringField");
            ASSERT(!rc);
            ASSERT(stringValue == agg6.asString());
            rc = agg1.getField(&agg7, &error, false, 0, "DoubleField");
            ASSERT(!rc);
            ASSERT(doubleValue == agg7.asDouble());

            agg1.insertItems(&error, 1, 2);
            ASSERT(6 == agg1.length());
            agg1.removeItems(&error, 1, 4);
            ASSERT(2 == agg1.length());
            if (veryVerbose) P(agg1);
        }

        {
            bslma_TestAllocator ta;

            if (verbose) cout << "Testing choice operations" << bsl::endl;

            Obj agg1;
            agg1.setSchemaPointer(schema.ptr());
            agg1.setRecordDefPointer(schema->lookupRecord("Choice1"));
            agg1.setDataType(ET::BDEM_CHOICE);
            agg1.setDataPointer(&choice);

            Obj   agg2, agg3, agg4, agg5;
            Error error;

            ASSERT(2 == agg1.numSelections());
            ASSERT(streq("NameSelection", agg1.selector()));
            ASSERT(0 == agg1.selectorId());
            int rc = agg1.selection(&agg2, &error);
            ASSERT(!rc);
            if (veryVerbose) P(agg1);

            agg1.makeSelection(&agg2, &error, "IdSelection");
            ASSERT(streq("IdSelection", agg1.selector()));
            ASSERT(1 == agg1.selectorId());
            rc = agg1.selection(&agg2, &error);
            ASSERT(!rc);
            if (veryVerbose) P(agg1);
        }

        {
            bslma_TestAllocator da("da", veryVeryVerbose);
            bslma_TestAllocator ta1("ta1", veryVeryVerbose);
            bslma_TestAllocator ta2("ta2", veryVeryVerbose);
            const bsl::string recName = "Level1";

            bslma_DefaultAllocatorGuard allocGuard(&da);
            Obj agg1;
            agg1.setSchemaPointer(schema.ptr());
            agg1.setRecordDefPointer(schema->lookupRecord(recName.c_str()));
            agg1.setDataType(ET::BDEM_LIST);
            agg1.setDataPointer(&list1);

            Obj   agg2, agg3, agg4, agg5;
            Error error;
            int rc = agg1.setField(&agg2, &error, "Double1.2", 3.4);
            ASSERT(!rc);
            rc = agg1.setField(&agg2, &error, "List1.3", "Date2.3",
                               "2006-02-16");
            ASSERT(!rc);
            rc = agg1.getField(&agg2, &error, false, "List1.4");
            ASSERT(!rc);
            agg2.makeValue();
            rc = agg1.setField(&agg2, &error, "List1.4",
                               "String1.1", "List in list");
            ASSERT(!rc);

            const int ta1BlocksUsed = ta1.numBlocksInUse();
            ASSERT(0 == da.numBlocksInUse());
            ASSERT(0 == ta2.numBlocksInUse());
        }
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
