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

#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>
#include <bsl_iostream.h>


using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// TBD: Update
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
// [  ] int maxSupportedBdexVersion();
// [  ] bdem_ElemType::Type getBdemType(const TYPE& value);
// [  ] bool areEquivalent(const Obj& lhs, const Obj& rhs);
// [  ] bool areIdentical(const Obj& lhs, const Obj& rhs);
//
// CREATORS
// [  ] bcem_AggregateRaw();
// [  ] bcem_AggregateRaw(const bcem_AggregateRaw& original);
// [  ] ~bcem_AggregateRaw();
//
// MANIPULATORS
// [  ] bcem_AggregateRaw& operator=(const bcem_AggregateRaw& rhs);
// [  ] void setDataType(bdem_ElemType::Type dataType);
// [  ] void setDataPointer(void *data);
// [  ] void setSchemaPointer(const bdem_Schema *schema);
// [  ] void setRecordDefPointer(const bdem_RecordDef *recordDef);
// [  ] void setFieldDefPointer(const bdem_FieldDef *fieldDef);
// [  ] void setTopLevelAggregateNullnessPointer(int *nullnessFlag);
// [  ] void clearParent();
// [  ] void reset();
//
// REFERENCED-VALUE ACCESSORS
// [  ] int reserveRaw(Error *error, bsl::size_t numItems) const;
// [  ] int capacityRaw(Error *error, bsl::size_t *capacity) const;
// [  ] bool isError() const;
// [  ] bool isVoid() const;
// [  ] bool isNull() const;
// [  ] bool isNullable() const;
// [  ] int errorCode() const;
// [  ] bsl::string errorMessage() const;
// [  ] bsl::string asString() const;
// [  ] void loadAsString(bsl::string *result) const;
// [  ] bool asBool() const;
// [  ] char asChar() const;
// [  ] short asShort() const;
// [  ] int asInt() const;
// [  ] bsls_Types::Int64 asInt64() const;
// [  ] float asFloat() const;
// [  ] double asDouble() const;
// [  ] bdet_Datetime asDatetime() const;
// [  ] bdet_DatetimeTz asDatetimeTz() const;
// [  ] bdet_Date asDate() const;
// [  ] bdet_DateTz asDateTz() const;
// [  ] bdet_Time asTime() const;
// [  ] bdet_TimeTz asTimeTz() const;
// [  ] const bdem_ElemRef asElemRef() const;
// [ 4] bool hasField(const char *fieldName) const;
// [  ] bool hasFieldById(int fieldId) const;
// [  ] bool hasFieldByIndex(int fieldIndex) const;
// [  ] int anonymousField(Obj *object, Error *error, int index) const;
// [  ] int anonymousField(Obj *object, Error *error) const;
// [ 4] int getField(Obj *o, Error *e, bool null, f1, f2, . ., f10) const;
// [  ] int findUnambiguousChoice(Obj *obj, Error *error, caller) const;
// [  ] int fieldByIndex(Obj *obj, Error *error, int index) const;
// [  ] int fieldById(Obj *obj, Error *error, int id) const;
// [  ] int arrayItem(Obj *item, Error *error, int index) const;
// [  ] int length() const;
// [  ] int size() const;
// [  ] int numSelections() const;
// [  ] const char *selector() const;
// [  ] int selectorId() const;
// [  ] int selectorIndex() const;
// [  ] int selection(Obj *obj, Error *error) const;
// [ 4] bdem_ElemType::Type dataType() const;
// [  ] const bdem_RecordDef& recordDef() const;
// [  ] const bdem_RecordDef *recordConstraint() const;
// [  ] const bdem_EnumerationDef *enumerationConstraint() const;
// [ 4] const bdem_FieldDef *fieldDef() const;
// [  ] const bdem_RecordDef *recordDefPtr() const;
// [  ] const void *data() const;
// [  ] const bdem_Schema *schema() const;
// [  ] void swap(bcem_AggregateRaw& other);
// [  ] STREAM& bdexStreamIn(STREAM& stream, int version) const;
// [  ] STREAM& bdexStreamOut(STREAM& stream, int version) const;
// [  ] bsl::ostream& print(stream, int level, int spl) const;
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
// [  ] int insertItem(Obj *item, Error *error, int index, value) const;
// [  ] int insertItemRaw(Obj *item, Error *error, int index) const;
// [  ] int insertItems(Error *error, int index, int numItems) const;
// [  ] int insertNullItems(Error *error, int index, int numItems) const;
// [  ] int removeItems(Error *error, int index, int numItems) const;
// [  ] int makeSelectionByIndex(Obj *obj, Error *error, int index) const;
// [  ] int makeSelectionByIndex(Obj *obj, Error *error, int idx, value) const;
// [  ] int makeSelection(Obj *obj, Error *error, newSelector) const;
// [  ] int makeSelection(Obj *obj, Error *error, newSelector, value) const;
// [  ] int makeSelectionById(Obj *obj, Error *error, id) const;
// [  ] int makeSelectionById(Obj *obj, Error *error, id, value) const;
// [  ] void makeNull() const;
// [  ] void makeValue() const;
// [ 2] int setValue(Error *error, const TYPE& value) const;
// [  ] int resize(Error *error, bsl::size_t newSize) const;
//
// FREE OPERATORS
// [  ] bsl::ostream& operator<<(bsl::ostream& stream, const Obj& obj);
//
// BDEAT FRAMEWORK
// [  ] namespace bdeat_TypeCategoryFunctions
// [  ] namespace bdeat_SequenceFunctions
// [  ] namespace bdeat_ChoiceFunctions
// [  ] namespace bdeat_ArrayFunctions
// [  ] namespace bdeat_EnumFunctions
// [  ] namespace bdeat_NullableValueFunctions
// [  ] namespace bdeat_ValueTypeFunctions
//
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] ggSchema, ggAggData, destroyAggData
// [  ] USAGE EXAMPLE
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
#define T_() cout << '\t' << flush;
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

typedef bdem_ConstElemRef     CERef;
typedef bdem_ElemRef          ERef;
typedef RecDef::RecordType    RecType;

typedef bdem_List             List;
typedef bdem_Row              Row;
typedef bdem_Table            Table;
typedef bdem_Choice           Choice;
typedef bdem_ChoiceArrayItem  ChoiceItem;
typedef bdem_ChoiceArray      ChoiceArray;

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
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? std::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    initStaticData ();

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 20: {
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

        if (verbose) tst::cout << "\nTESTING 'asXXX' ACCESSORS"
                               << "\n=========================" << bsl::endl;

        if (veryVerbose) { T_ cout << "Testing for BOOL" << endl; }
        {
            typedef bool TYPE;

            const CERef VN = getCERef(ET::BDEM_BOOL, 0);
            const CERef VA = getCERef(ET::BDEM_BOOL, 1);
            const CERef VB = getCERef(ET::BDEM_BOOL, 2);

            Obj mX; const Obj& X = mX;
            mX.setDataType(ET::BDEM_BOOL);
            mX.setDataPointer(VA.data());
            int nf1 = 0;
            mX.setTopLevelAggregateNullnessPointer(&nf1);

            ASSERT(VA.theBool() == X.asBool());
            ASSERT(VA           == X.asElemRef());
            ASSERT(!X.isNull());

            X.makeNull();
            ASSERT(X.isNull());
            ASSERT(X.asElemRef().isNull());
            ASSERT(VN.theBool() == X.asBool());

            Obj mY; const Obj& Y = mY;
            mY.setDataType(ET::BDEM_BOOL);
            mY.setDataPointer(VB.data());
            int nf2 = 0;
            mY.setTopLevelAggregateNullnessPointer(&nf2);

            ASSERT(VB.theBool() == Y.asBool());
            ASSERT(VB           == Y.asElemRef());
            ASSERT(!Y.isNull());

            Y.makeNull();
            ASSERT(Y.isNull());
            ASSERT(Y.asElemRef().isNull());
            ASSERT(VN.theBool() == Y.asBool());

            // nillable bool array
            const char *SPEC = ":aaa&FN";
            Schema schema; const Schema& SCHEMA = schema;
            ggSchema(&schema, SPEC);

            const RecDef *RECORD  = &SCHEMA.record(0);
            const char   *fldName = RECORD->fieldName(0);

            Obj mZ; const Obj& Z = mZ;
            int rc = ggAggData(&mZ, *r, &t);
            ASSERT(!rc);

            Obj mB = mA.field(fldName); const Obj& B = mB;
            const int NE = 3;
            mB.resize(NE);
            for (int i = 0; i < NE; ++i) {
                LOOP_ASSERT(i, B.field(i).isNull());
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
            ASSERT(!B.field(0).isNull());

            mB.setFieldNull(0);
            ASSERT(VN.theBool() == B.field(0).asBool());
            ASSERT(B.field(0).asElemRef().isNull());
            ASSERT(B.field(0).isNull());

            mA.setField(fldName, 1, VB);
            ASSERT(VB.theBool() == A.field(fldName, 1).asBool());
            LOOP2_ASSERT(VB, A.field(fldName, 1).asElemRef(),
                         VB           == A.field(fldName, 1).asElemRef());
            ASSERT(!A.field(fldName, 1).isNull());

            mA.setFieldNull(fldName, 1);
            ASSERT(VN.theBool() == A.field(fldName, 1).asBool());
            ASSERT(A.field(fldName, 1).asElemRef().isNull());
            ASSERT(A.field(fldName, 1).isNull());

            Obj mC = B.field(2); const Obj& C = mC;
            mC.setValue(VA);
            ASSERT(VA.theBool() == C.asBool());
            ASSERT(VA           == C.asElemRef());
            ASSERT(!C.isNull());

            mC.makeNull();
            ASSERT(VN.theBool() == C.asBool());
            ASSERT(C.asElemRef().isNull());
            ASSERT(C.isNull());
        }

#if 0
        if (veryVerbose) { T_ cout << "Testing for CHAR" << endl; }
        {
            const CERef VN = getCERef(ET::BDEM_CHAR, 0);
            const CERef VA = getCERef(ET::BDEM_CHAR, 1);
            const CERef VB = getCERef(ET::BDEM_CHAR, 2);

            Obj mX(ET::BDEM_CHAR, VA); const Obj& X = mX;
            Obj mY(ET::BDEM_CHAR, VB); const Obj& Y = mY;

            ASSERT(VA.theChar() == X.asChar());
            ASSERT(VA           == X.asElemRef());
            ASSERT(!X.isNull());

            X.makeNull();
            ASSERT(X.isNull());
            ASSERT(X.asElemRef().isNull());
            ASSERT(VN.theChar() == X.asChar());

            ASSERT(VB.theChar() == Y.asChar());
            ASSERT(VB           == Y.asElemRef());
            ASSERT(!Y.isNull());

            Y.makeNull();
            ASSERT(Y.isNull());
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
                LOOP_ASSERT(i, B.field(i).isNull());
                LOOP_ASSERT(i, VN.theChar() == B.field(i).asChar());
                LOOP_ASSERT(i, VN.theChar() ==
                                             B.field(i).asElemRef().theChar());
                LOOP_ASSERT(i, B.field(i).asElemRef().isNull());
                LOOP_ASSERT(i, isUnset(B.field(i).asElemRef()));
            }

            mB.setField(0, VA);
            ASSERT(VA.theChar() == B.field(0).asChar());
            ASSERT(VA           == B.field(0).asElemRef());
            ASSERT(!B.field(0).isNull());

            mB.setFieldNull(0);
            ASSERT(VN.theChar() == B.field(0).asChar());
            ASSERT(B.field(0).asElemRef().isNull());
            ASSERT(B.field(0).isNull());

            mA.setField(fldName, 1, VB);
            LOOP2_ASSERT(VB.theChar(), A.field(fldName, 1).asChar(),
                         VB.theChar() == A.field(fldName, 1).asChar());
            ASSERT(VB           == A.field(fldName, 1).asElemRef());
            ASSERT(!A.field(fldName, 1).isNull());

            mA.setFieldNull(fldName, 1);
            ASSERT(VN.theChar() == A.field(fldName, 1).asChar());
            ASSERT(A.field(fldName, 1).asElemRef().isNull());
            ASSERT(A.field(fldName, 1).isNull());

            Obj mC = B.field(2); const Obj& C = mC;
            mC.setValue(VA);
            ASSERT(VA.theChar() == C.asChar());
            ASSERT(VA           == C.asElemRef());
            ASSERT(!C.isNull());

            mC.makeNull();
            ASSERT(VN.theChar() == C.asChar());
            ASSERT(C.asElemRef().isNull());
            ASSERT(C.isNull());
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
            ASSERT(!X.isNull());

            X.makeNull();
            ASSERT(X.isNull());
            ASSERT(X.asElemRef().isNull());
            ASSERT(VN.theShort() == X.asShort());

            ASSERT(VB.theShort() == Y.asShort());
            ASSERT(VB            == Y.asElemRef());
            ASSERT(!Y.isNull());

            Y.makeNull();
            ASSERT(Y.isNull());
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
                LOOP_ASSERT(i, B.field(i).isNull());
                LOOP_ASSERT(i, VN.theShort() == B.field(i).asShort());
                LOOP_ASSERT(i, VN.theShort() ==
                                            B.field(i).asElemRef().theShort());
                LOOP_ASSERT(i, B.field(i).asElemRef().isNull());
                LOOP_ASSERT(i, isUnset(B.field(i).asElemRef()));
            }

            mB.setField(0, VA);
            ASSERT(VA.theShort() == B.field(0).asShort());
            ASSERT(VA            == B.field(0).asElemRef());
            ASSERT(!B.field(0).isNull());

            mB.setFieldNull(0);
            ASSERT(VN.theShort() == B.field(0).asShort());
            ASSERT(B.field(0).asElemRef().isNull());
            ASSERT(B.field(0).isNull());

            mA.setField(fldName, 1, VB);
            ASSERT(VB.theShort() == A.field(fldName, 1).asShort());
            ASSERT(VB            == A.field(fldName, 1).asElemRef());
            ASSERT(!A.field(fldName, 1).isNull());

            mA.setFieldNull(fldName, 1);
            ASSERT(VN.theShort() == A.field(fldName, 1).asShort());
            ASSERT(A.field(fldName, 1).asElemRef().isNull());
            ASSERT(A.field(fldName, 1).isNull());

            Obj mC = B.field(2); const Obj& C = mC;
            mC.setValue(VA);
            ASSERT(VA.theShort() == C.asShort());
            ASSERT(VA            == C.asElemRef());
            ASSERT(!C.isNull());

            mC.makeNull();
            ASSERT(VN.theShort() == C.asShort());
            ASSERT(C.asElemRef().isNull());
            ASSERT(C.isNull());
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
            ASSERT(!X.isNull());

            X.makeNull();
            ASSERT(X.isNull());
            ASSERT(X.asElemRef().isNull());
            ASSERT(VN.theInt() == X.asInt());

            ASSERT(VB.theInt() == Y.asInt());
            ASSERT(VB          == Y.asElemRef());
            ASSERT(!Y.isNull());

            Y.makeNull();
            ASSERT(Y.isNull());
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
                LOOP_ASSERT(i, B.field(i).isNull());
                LOOP_ASSERT(i, VN.theInt() == B.field(i).asInt());
                LOOP_ASSERT(i, VN.theInt() == B.field(i).asElemRef().theInt());
                LOOP_ASSERT(i, B.field(i).asElemRef().isNull());
                LOOP_ASSERT(i, isUnset(B.field(i).asElemRef()));
            }

            mB.setField(0, VA);
            ASSERT(VA.theInt() == B.field(0).asInt());
            ASSERT(VA          == B.field(0).asElemRef());
            ASSERT(!B.field(0).isNull());

            mB.setFieldNull(0);
            ASSERT(VN.theInt() == B.field(0).asInt());
            ASSERT(B.field(0).asElemRef().isNull());
            ASSERT(B.field(0).isNull());

            mA.setField(fldName, 1, VB);
            ASSERT(VB.theInt() == A.field(fldName, 1).asInt());
            ASSERT(VB          == A.field(fldName, 1).asElemRef());
            ASSERT(!A.field(fldName, 1).isNull());

            mA.setFieldNull(fldName, 1);
            ASSERT(VN.theInt() == A.field(fldName, 1).asInt());
            ASSERT(A.field(fldName, 1).asElemRef().isNull());
            ASSERT(A.field(fldName, 1).isNull());

            Obj mC = B.field(2); const Obj& C = mC;
            mC.setValue(VA);
            ASSERT(VA.theInt() == C.asInt());
            ASSERT(VA          == C.asElemRef());
            ASSERT(!C.isNull());

            mC.makeNull();
            ASSERT(VN.theInt() == C.asInt());
            ASSERT(C.asElemRef().isNull());
            ASSERT(C.isNull());
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
            ASSERT(!X.isNull());

            X.makeNull();
            ASSERT(X.isNull());
            ASSERT(X.asElemRef().isNull());
            ASSERT(VN.theInt64() == X.asInt64());

            ASSERT(VB.theInt64() == Y.asInt64());
            ASSERT(VB            == Y.asElemRef());
            ASSERT(!Y.isNull());

            Y.makeNull();
            ASSERT(Y.isNull());
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
                LOOP_ASSERT(i, B.field(i).isNull());
                LOOP_ASSERT(i, VN.theInt64() == B.field(i).asInt64());
                LOOP_ASSERT(i, VN.theInt64() ==
                                            B.field(i).asElemRef().theInt64());
                LOOP_ASSERT(i, B.field(i).asElemRef().isNull());
                LOOP_ASSERT(i, isUnset(B.field(i).asElemRef()));
            }

            mB.setField(0, VA);
            ASSERT(VA.theInt64() == B.field(0).asInt64());
            ASSERT(VA            == B.field(0).asElemRef());
            ASSERT(!B.field(0).isNull());

            mB.setFieldNull(0);
            ASSERT(VN.theInt64() == B.field(0).asInt64());
            ASSERT(B.field(0).asElemRef().isNull());
            ASSERT(B.field(0).isNull());

            mA.setField(fldName, 1, VB);
            ASSERT(VB.theInt64() == A.field(fldName, 1).asInt64());
            ASSERT(VB            == A.field(fldName, 1).asElemRef());
            ASSERT(!A.field(fldName, 1).isNull());

            mA.setFieldNull(fldName, 1);
            ASSERT(VN.theInt64() == A.field(fldName, 1).asInt64());
            ASSERT(A.field(fldName, 1).asElemRef().isNull());
            ASSERT(A.field(fldName, 1).isNull());

            Obj mC = B.field(2); const Obj& C = mC;
            mC.setValue(VA);
            ASSERT(VA.theInt64() == C.asInt64());
            ASSERT(VA            == C.asElemRef());
            ASSERT(!C.isNull());

            mC.makeNull();
            ASSERT(VN.theInt64() == C.asInt64());
            ASSERT(C.asElemRef().isNull());
            ASSERT(C.isNull());
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
            ASSERT(!X.isNull());

            X.makeNull();
            ASSERT(X.isNull());
            ASSERT(X.asElemRef().isNull());
            ASSERT(VN.theFloat() == X.asFloat());

            ASSERT(VB.theFloat() == Y.asFloat());
            ASSERT(VB            == Y.asElemRef());
            ASSERT(!Y.isNull());

            Y.makeNull();
            ASSERT(Y.isNull());
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
                LOOP_ASSERT(i, B.field(i).isNull());
                LOOP_ASSERT(i, VN.theFloat() == B.field(i).asFloat());
                LOOP_ASSERT(i, VN.theFloat() ==
                                            B.field(i).asElemRef().theFloat());
                LOOP_ASSERT(i, B.field(i).asElemRef().isNull());
                LOOP_ASSERT(i, isUnset(B.field(i).asElemRef()));
            }

            mB.setField(0, VA);
            ASSERT(VA.theFloat() == B.field(0).asFloat());
            ASSERT(VA            == B.field(0).asElemRef());
            ASSERT(!B.field(0).isNull());

            mB.setFieldNull(0);
            ASSERT(VN.theFloat() == B.field(0).asFloat());
            ASSERT(B.field(0).asElemRef().isNull());
            ASSERT(B.field(0).isNull());

            mA.setField(fldName, 1, VB);
            ASSERT(VB.theFloat() == A.field(fldName, 1).asFloat());
            ASSERT(VB            == A.field(fldName, 1).asElemRef());
            ASSERT(!A.field(fldName, 1).isNull());

            mA.setFieldNull(fldName, 1);
            ASSERT(VN.theFloat() == A.field(fldName, 1).asFloat());
            ASSERT(A.field(fldName, 1).asElemRef().isNull());
            ASSERT(A.field(fldName, 1).isNull());

            Obj mC = B.field(2); const Obj& C = mC;
            mC.setValue(VA);
            ASSERT(VA.theFloat() == C.asFloat());
            ASSERT(VA            == C.asElemRef());
            ASSERT(!C.isNull());

            mC.makeNull();
            ASSERT(VN.theFloat() == C.asFloat());
            ASSERT(C.asElemRef().isNull());
            ASSERT(C.isNull());
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
            ASSERT(!X.isNull());

            X.makeNull();
            ASSERT(X.isNull());
            ASSERT(X.asElemRef().isNull());
            ASSERT(VN.theDouble() == X.asDouble());

            ASSERT(VB.theDouble() == Y.asDouble());
            ASSERT(VB             == Y.asElemRef());
            ASSERT(!Y.isNull());

            Y.makeNull();
            ASSERT(Y.isNull());
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
                LOOP_ASSERT(i, B.field(i).isNull());
                LOOP_ASSERT(i, VN.theDouble() == B.field(i).asDouble());
                LOOP_ASSERT(i, VN.theDouble() ==
                                           B.field(i).asElemRef().theDouble());
                LOOP_ASSERT(i, B.field(i).asElemRef().isNull());
                LOOP_ASSERT(i, isUnset(B.field(i).asElemRef()));
            }

            mB.setField(0, VA);
            ASSERT(VA.theDouble() == B.field(0).asDouble());
            ASSERT(VA             == B.field(0).asElemRef());
            ASSERT(!B.field(0).isNull());

            mB.setFieldNull(0);
            ASSERT(VN.theDouble() == B.field(0).asDouble());
            ASSERT(B.field(0).asElemRef().isNull());
            ASSERT(B.field(0).isNull());

            mA.setField(fldName, 1, VB);
            ASSERT(VB.theDouble() == A.field(fldName, 1).asDouble());
            ASSERT(VB             == A.field(fldName, 1).asElemRef());
            ASSERT(!A.field(fldName, 1).isNull());

            mA.setFieldNull(fldName, 1);
            ASSERT(VN.theDouble() == A.field(fldName, 1).asDouble());
            ASSERT(A.field(fldName, 1).asElemRef().isNull());
            ASSERT(A.field(fldName, 1).isNull());

            Obj mC = B.field(2); const Obj& C = mC;
            mC.setValue(VA);
            ASSERT(VA.theDouble() == C.asDouble());
            ASSERT(VA             == C.asElemRef());
            ASSERT(!C.isNull());

            mC.makeNull();
            ASSERT(VN.theDouble() == C.asDouble());
            ASSERT(C.asElemRef().isNull());
            ASSERT(C.isNull());
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
            ASSERT(!X.isNull());

            X.makeNull();
            ASSERT(X.isNull());
            ASSERT(X.asElemRef().isNull());
            ASSERT(VN.theDatetime() == X.asDatetime());

            ASSERT(VB.theDatetime() == Y.asDatetime());
            ASSERT(VB               == Y.asElemRef());
            ASSERT(!Y.isNull());

            Y.makeNull();
            ASSERT(Y.isNull());
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
                LOOP_ASSERT(i, B.field(i).isNull());
                LOOP_ASSERT(i, VN.theDatetime() == B.field(i).asDatetime());
                LOOP_ASSERT(i, VN.theDatetime() ==
                                         B.field(i).asElemRef().theDatetime());
                LOOP_ASSERT(i, B.field(i).asElemRef().isNull());
                LOOP_ASSERT(i, isUnset(B.field(i).asElemRef()));
            }

            mB.setField(0, VA);
            ASSERT(VA.theDatetime() == B.field(0).asDatetime());
            ASSERT(VA               == B.field(0).asElemRef());
            ASSERT(!B.field(0).isNull());

            mB.setFieldNull(0);
            ASSERT(VN.theDatetime() == B.field(0).asDatetime());
            ASSERT(B.field(0).asElemRef().isNull());
            ASSERT(B.field(0).isNull());

            mA.setField(fldName, 1, VB);
            ASSERT(VB.theDatetime() == A.field(fldName, 1).asDatetime());
            ASSERT(VB               == A.field(fldName, 1).asElemRef());
            ASSERT(!A.field(fldName, 1).isNull());

            mA.setFieldNull(fldName, 1);
            ASSERT(VN.theDatetime() == A.field(fldName, 1).asDatetime());
            ASSERT(A.field(fldName, 1).asElemRef().isNull());
            ASSERT(A.field(fldName, 1).isNull());

            Obj mC = B.field(2); const Obj& C = mC;
            mC.setValue(VA);
            ASSERT(VA.theDatetime() == C.asDatetime());
            ASSERT(VA               == C.asElemRef());
            ASSERT(!C.isNull());

            mC.makeNull();
            ASSERT(VN.theDatetime() == C.asDatetime());
            ASSERT(C.asElemRef().isNull());
            ASSERT(C.isNull());
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
            ASSERT(!X.isNull());

            X.makeNull();
            ASSERT(X.isNull());
            ASSERT(X.asElemRef().isNull());
            ASSERT(VN.theDate() == X.asDate());

            ASSERT(VB.theDate() == Y.asDate());
            ASSERT(VB           == Y.asElemRef());
            ASSERT(!Y.isNull());

            Y.makeNull();
            ASSERT(Y.isNull());
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
                LOOP_ASSERT(i, B.field(i).isNull());
                LOOP_ASSERT(i, VN.theDate() == B.field(i).asDate());
                LOOP_ASSERT(i, VN.theDate() ==
                                             B.field(i).asElemRef().theDate());
                LOOP_ASSERT(i, B.field(i).asElemRef().isNull());
                LOOP_ASSERT(i, isUnset(B.field(i).asElemRef()));
            }

            mB.setField(0, VA);
            ASSERT(VA.theDate() == B.field(0).asDate());
            ASSERT(VA           == B.field(0).asElemRef());
            ASSERT(!B.field(0).isNull());

            mB.setFieldNull(0);
            ASSERT(VN.theDate() == B.field(0).asDate());
            ASSERT(B.field(0).asElemRef().isNull());
            ASSERT(B.field(0).isNull());

            mA.setField(fldName, 1, VB);
            ASSERT(VB.theDate() == A.field(fldName, 1).asDate());
            ASSERT(VB           == A.field(fldName, 1).asElemRef());
            ASSERT(!A.field(fldName, 1).isNull());

            mA.setFieldNull(fldName, 1);
            ASSERT(VN.theDate() == A.field(fldName, 1).asDate());
            ASSERT(A.field(fldName, 1).asElemRef().isNull());
            ASSERT(A.field(fldName, 1).isNull());

            Obj mC = B.field(2); const Obj& C = mC;
            mC.setValue(VA);
            ASSERT(VA.theDate() == C.asDate());
            ASSERT(VA           == C.asElemRef());
            ASSERT(!C.isNull());

            mC.makeNull();
            ASSERT(VN.theDate() == C.asDate());
            ASSERT(C.asElemRef().isNull());
            ASSERT(C.isNull());
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
            ASSERT(!X.isNull());

            X.makeNull();
            ASSERT(X.isNull());
            ASSERT(X.asElemRef().isNull());
            ASSERT(VN.theTime() == X.asTime());

            ASSERT(VB.theTime() == Y.asTime());
            ASSERT(VB           == Y.asElemRef());
            ASSERT(!Y.isNull());

            Y.makeNull();
            ASSERT(Y.isNull());
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
                LOOP_ASSERT(i, B.field(i).isNull());
                LOOP_ASSERT(i, VN.theTime() == B.field(i).asTime());
                LOOP_ASSERT(i, VN.theTime() ==
                                             B.field(i).asElemRef().theTime());
                LOOP_ASSERT(i, B.field(i).asElemRef().isNull());
                LOOP_ASSERT(i, isUnset(B.field(i).asElemRef()));
            }

            mB.setField(0, VA);
            ASSERT(VA.theTime() == B.field(0).asTime());
            ASSERT(VA           == B.field(0).asElemRef());
            ASSERT(!B.field(0).isNull());

            mB.setFieldNull(0);
            ASSERT(VN.theTime() == B.field(0).asTime());
            ASSERT(B.field(0).asElemRef().isNull());
            ASSERT(B.field(0).isNull());

            mA.setField(fldName, 1, VB);
            ASSERT(VB.theTime() == A.field(fldName, 1).asTime());
            ASSERT(VB           == A.field(fldName, 1).asElemRef());
            ASSERT(!A.field(fldName, 1).isNull());

            mA.setFieldNull(fldName, 1);
            ASSERT(VN.theTime() == A.field(fldName, 1).asTime());
            ASSERT(A.field(fldName, 1).asElemRef().isNull());
            ASSERT(A.field(fldName, 1).isNull());

            Obj mC = B.field(2); const Obj& C = mC;
            mC.setValue(VA);
            ASSERT(VA.theTime() == C.asTime());
            ASSERT(VA           == C.asElemRef());
            ASSERT(!C.isNull());

            mC.makeNull();
            ASSERT(VN.theTime() == C.asTime());
            ASSERT(C.asElemRef().isNull());
            ASSERT(C.isNull());
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
            ASSERT(!X.isNull());

            X.makeNull();
            ASSERT(X.isNull());
            ASSERT(X.asElemRef().isNull());
            ASSERT(VN.theDatetimeTz() == X.asDatetimeTz());

            ASSERT(VB.theDatetimeTz() == Y.asDatetimeTz());
            ASSERT(VB                 == Y.asElemRef());
            ASSERT(!Y.isNull());

            Y.makeNull();
            ASSERT(Y.isNull());
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
                LOOP_ASSERT(i, B.field(i).isNull());
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
            ASSERT(!B.field(0).isNull());

            mB.setFieldNull(0);
            ASSERT(VN.theDatetimeTz() == B.field(0).asDatetimeTz());
            ASSERT(B.field(0).asElemRef().isNull());
            ASSERT(B.field(0).isNull());

            mA.setField(fldName, 1, VB);
            ASSERT(VB.theDatetimeTz() == A.field(fldName, 1).asDatetimeTz());
            ASSERT(VB                 == A.field(fldName, 1).asElemRef());
            ASSERT(!A.field(fldName, 1).isNull());

            mA.setFieldNull(fldName, 1);
            ASSERT(VN.theDatetimeTz() == A.field(fldName, 1).asDatetimeTz());
            ASSERT(A.field(fldName, 1).asElemRef().isNull());
            ASSERT(A.field(fldName, 1).isNull());

            Obj mC = B.field(2); const Obj& C = mC;
            mC.setValue(VA);
            ASSERT(VA.theDatetimeTz() == C.asDatetimeTz());
            ASSERT(VA                 == C.asElemRef());
            ASSERT(!C.isNull());

            mC.makeNull();
            ASSERT(VN.theDatetimeTz() == C.asDatetimeTz());
            ASSERT(C.asElemRef().isNull());
            ASSERT(C.isNull());
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
            ASSERT(!X.isNull());

            X.makeNull();
            ASSERT(X.isNull());
            ASSERT(X.asElemRef().isNull());
            ASSERT(VN.theDateTz() == X.asDateTz());

            ASSERT(VB.theDateTz() == Y.asDateTz());
            ASSERT(VB             == Y.asElemRef());
            ASSERT(!Y.isNull());

            Y.makeNull();
            ASSERT(Y.isNull());
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
                LOOP_ASSERT(i, B.field(i).isNull());
                LOOP_ASSERT(i, VN.theDateTz() == B.field(i).asDateTz());
                LOOP_ASSERT(i, VN.theDateTz() ==
                                           B.field(i).asElemRef().theDateTz());
                LOOP_ASSERT(i, B.field(i).asElemRef().isNull());
                LOOP_ASSERT(i, isUnset(B.field(i).asElemRef()));
            }

            mB.setField(0, VA);
            ASSERT(VA.theDateTz() == B.field(0).asDateTz());
            ASSERT(VA             == B.field(0).asElemRef());
            ASSERT(!B.field(0).isNull());

            mB.setFieldNull(0);
            ASSERT(VN.theDateTz() == B.field(0).asDateTz());
            ASSERT(B.field(0).asElemRef().isNull());
            ASSERT(B.field(0).isNull());

            mA.setField(fldName, 1, VB);
            ASSERT(VB.theDateTz() == A.field(fldName, 1).asDateTz());
            ASSERT(VB             == A.field(fldName, 1).asElemRef());
            ASSERT(!A.field(fldName, 1).isNull());

            mA.setFieldNull(fldName, 1);
            ASSERT(VN.theDateTz() == A.field(fldName, 1).asDateTz());
            ASSERT(A.field(fldName, 1).asElemRef().isNull());
            ASSERT(A.field(fldName, 1).isNull());

            Obj mC = B.field(2); const Obj& C = mC;
            mC.setValue(VA);
            ASSERT(VA.theDateTz() == C.asDateTz());
            ASSERT(VA             == C.asElemRef());
            ASSERT(!C.isNull());

            mC.makeNull();
            ASSERT(VN.theDateTz() == C.asDateTz());
            ASSERT(C.asElemRef().isNull());
            ASSERT(C.isNull());
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
            ASSERT(!X.isNull());

            X.makeNull();
            ASSERT(X.isNull());
            ASSERT(X.asElemRef().isNull());
            ASSERT(VN.theTimeTz() == X.asTimeTz());

            ASSERT(VB.theTimeTz() == Y.asTimeTz());
            ASSERT(VB             == Y.asElemRef());
            ASSERT(!Y.isNull());

            Y.makeNull();
            ASSERT(Y.isNull());
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
                LOOP_ASSERT(i, B.field(i).isNull());
                LOOP_ASSERT(i, VN.theTimeTz() == B.field(i).asTimeTz());
                LOOP_ASSERT(i, VN.theTimeTz() ==
                                           B.field(i).asElemRef().theTimeTz());
                LOOP_ASSERT(i, B.field(i).asElemRef().isNull());
                LOOP_ASSERT(i, isUnset(B.field(i).asElemRef()));
            }

            mB.setField(0, VA);
            ASSERT(VA.theTimeTz() == B.field(0).asTimeTz());
            ASSERT(VA             == B.field(0).asElemRef());
            ASSERT(!B.field(0).isNull());

            mB.setFieldNull(0);
            ASSERT(VN.theTimeTz() == B.field(0).asTimeTz());
            ASSERT(B.field(0).asElemRef().isNull());
            ASSERT(B.field(0).isNull());

            mA.setField(fldName, 1, VB);
            ASSERT(VB.theTimeTz() == A.field(fldName, 1).asTimeTz());
            ASSERT(VB             == A.field(fldName, 1).asElemRef());
            ASSERT(!A.field(fldName, 1).isNull());

            mA.setFieldNull(fldName, 1);
            ASSERT(VN.theTimeTz() == A.field(fldName, 1).asTimeTz());
            ASSERT(A.field(fldName, 1).asElemRef().isNull());
            ASSERT(A.field(fldName, 1).isNull());

            Obj mC = B.field(2); const Obj& C = mC;
            mC.setValue(VA);
            ASSERT(VA.theTimeTz() == C.asTimeTz());
            ASSERT(VA             == C.asElemRef());
            ASSERT(!C.isNull());

            mC.makeNull();
            ASSERT(VN.theTimeTz() == C.asTimeTz());
            ASSERT(C.asElemRef().isNull());
            ASSERT(C.isNull());
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
#endif
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
            switch (s.numRecords()) {
              case 1: {
                const char f1[] = { (*bsl::strtok(SPEC, ":")), 0 };

                ASSERT(X.hasField(f1));
                ASSERT(!X.hasField(errorField));

                Error error;
                Obj   mS; const Obj& S = mS;
                rc = mX.setField(&mS, &error, f1, VA);
                ASSERT(!rc);
                ASSERT(FLD_TYPE == S.dataType());
                ASSERT(&fd      == S.fieldDef());
                ASSERT(0        == S.recordConstraint());
                ASSERT(VA       == S.asElemRef());
                ASSERT(!S.isNull());

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
                ASSERT(!X.hasField(errorField));

                Error error;
                Obj   mS; const Obj& S = mS;
                rc = mX.setField(&mS, &error, f2, f1, VA);
                ASSERT(!rc);
                ASSERT(FLD_TYPE == S.dataType());
                ASSERT(&fd      == S.fieldDef());
                ASSERT(0        == S.recordConstraint());
                ASSERT(VA       == S.asElemRef());
                ASSERT(!S.isNull());

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
                ASSERT(!X.hasField(errorField));

                Error error;
                Obj   mS; const Obj& S = mS;
                rc = mX.setField(&mS, &error, f3, f2, f1, VA);
                ASSERT(!rc);
                ASSERT(FLD_TYPE == S.dataType());
                ASSERT(&fd      == S.fieldDef());
                ASSERT(0        == S.recordConstraint());
                ASSERT(VA       == S.asElemRef());
                ASSERT(!S.isNull());

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
                ASSERT(!X.hasField(errorField));

                Error error;
                Obj   mS; const Obj& S = mS;
                rc = mX.setField(&mS, &error, f4, f3, f2, f1, VA);
                ASSERT(!rc);
                ASSERT(FLD_TYPE == S.dataType());
                ASSERT(&fd      == S.fieldDef());
                LOOP3_ASSERT(LINE, VA, S.asElemRef(), VA == S.asElemRef());
                ASSERT(!S.isNull());

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
                ASSERT(!X.hasField(errorField));

                Error error;
                Obj   mS; const Obj& S = mS;
                rc = mX.setField(&mS, &error, f5, f4, f3, f2, f1, VA);
                ASSERT(!rc);
                ASSERT(FLD_TYPE == S.dataType());
                ASSERT(&fd      == S.fieldDef());
                LOOP3_ASSERT(LINE, VA, S.asElemRef(), VA == S.asElemRef());
                ASSERT(!S.isNull());

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
                ASSERT(!X.hasField(errorField));

                Error error;
                Obj   mS; const Obj& S = mS;
                rc = mX.setField(&mS, &error, f6, f5, f4, f3, f2, f1, VA);
                ASSERT(!rc);
                ASSERT(FLD_TYPE == S.dataType());
                ASSERT(&fd      == S.fieldDef());
                LOOP3_ASSERT(LINE, VA, S.asElemRef(), VA == S.asElemRef());
                ASSERT(!S.isNull());

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
                ASSERT(!X.hasField(errorField));

                Error error;
                Obj   mS; const Obj& S = mS;
                rc = mX.setField(&mS, &error, f7, f6, f5, f4, f3, f2, f1, VA);
                ASSERT(!rc);
                ASSERT(FLD_TYPE == S.dataType());
                ASSERT(&fd      == S.fieldDef());
                LOOP3_ASSERT(LINE, VA, S.asElemRef(), VA == S.asElemRef());
                ASSERT(!S.isNull());

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
                ASSERT(!X.hasField(errorField));

                Error error;
                Obj   mS; const Obj& S = mS;
                rc = mX.setField(&mS, &error, f8, f7, f6, f5, f4,
                                 f3, f2, f1, VA);
                ASSERT(!rc);
                ASSERT(FLD_TYPE == S.dataType());
                ASSERT(&fd      == S.fieldDef());
                LOOP3_ASSERT(LINE, VA, S.asElemRef(), VA == S.asElemRef());
                ASSERT(!S.isNull());

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
                ASSERT(!X.hasField(errorField));

                Error error;
                Obj   mS; const Obj& S = mS;
                rc = mX.setField(&mS, &error, f9, f8, f7, f6, f5, f4,
                                 f3, f2, f1, VA);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, VA, S.asElemRef(), VA == S.asElemRef());
                ASSERT(!S.isNull());

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
                ASSERT(!X.hasField(errorField));

                Error error;
                Obj   mS; const Obj& S = mS;
                rc = mX.setField(&mS, &error, f10, f9, f8, f7, f6, f5, f4,
                                 f3, f2, f1, VA);
                ASSERT(!rc);
                ASSERT(FLD_TYPE == S.dataType());
                ASSERT(&fd      == S.fieldDef());
                LOOP3_ASSERT(LINE, VA, S.asElemRef(), VA == S.asElemRef());
                ASSERT(!S.isNull());

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

                // TBD: Uncomment
#if 0
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
#endif
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

                if (RecDef::BDEM_CHOICE_RECORD == r.recordType()) {
                    Error error;
                    Obj mS; const Obj& S = mS;
                    int rc = mX.makeSelection(&mS, &error, fldName);
                    ASSERT(!rc);
                }

                if (!ET::BDEM_LIST == TYPE) {
                    Error error;
                    Obj   mS; const Obj& S = mS;
                    int rc = X.getField(&mS, &error, false, fldName);
                    ASSERT(!rc);
                    if (veryVerbose) { T_ P(S) };
                    LOOP2_ASSERT(LINE, S, S.isNull() == isNull);
                }

                // Test setField with bdem_ConstElemRef and bdem_ElemRef
                Error error;
                Obj   mS; const Obj& S = mS;

                rc = mX.setField(&mS, &error, fldName, CEA);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, CEA, S.asElemRef(),
                             CEA == S.asElemRef());
                LOOP_ASSERT(S, !S.isNull());

                mS.reset();
                rc = X.getField(&mS, &error, false, fldName);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, CEA, S.asElemRef(),
                             CEA == S.asElemRef());
                LOOP_ASSERT(S, !S.isNull());

                mS.makeNull();

                mS.reset();
                rc = X.getField(&mS, &error, false, fldName);
                ASSERT(!rc);
                LOOP_ASSERT(S, S.isNull());

                mS.reset();
                rc = mX.setField(&mS, &error, fldName, CEB);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, CEB, S.asElemRef(),
                             CEB == S.asElemRef());
                LOOP_ASSERT(S, !S.isNull());

                mS.reset();
                rc = X.getField(&mS, &error, false, fldName);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, CEB, S.asElemRef(),
                             CEB == S.asElemRef());
                LOOP_ASSERT(S, !S.isNull());

                mS.reset();
                rc = mX.setField(&mS, &error, fldName, NULL_CER);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, NULL_CER, S.asElemRef(),
                             NULL_CER == S.asElemRef());
                LOOP_ASSERT(S, S.isNull());

                mS.reset();
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

                mS.reset();
                rc = X.getField(&mS, &error, false, fldName);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, EA, S.asElemRef(),
                             EA == S.asElemRef());
                LOOP_ASSERT(S, !S.isNull());

                mS.makeNull();

                mS.reset();
                rc = X.getField(&mS, &error, false, fldName);
                ASSERT(!rc);
                LOOP_ASSERT(S, S.isNull());

                rc = mX.setField(&mS, &error, fldName, EB);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, EB, S.asElemRef(),
                             EB == S.asElemRef());
                LOOP_ASSERT(S, !S.isNull());

                mS.reset();
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

                mS.reset();
                rc = X.getField(&mS, &error, false, fldName);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, NULL_ER, S.asElemRef(),
                             NULL_ER == S.asElemRef());
                LOOP_ASSERT(S, S.isNull());

                // Test setField with values of the field types
                SetFieldFunctor f1(&mX, fldName);

                funcVisitor(&f1, CEA);

                mS.reset();
                rc = X.getField(&mS, &error, false, fldName);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, CEA, S.asElemRef(),
                             CEA == S.asElemRef());
                LOOP_ASSERT(S, !S.isNull());

                mS.makeNull();

                mS.reset();
                rc = X.getField(&mS, &error, false, fldName);
                ASSERT(!rc);
                LOOP_ASSERT(S, S.isNull());

                funcVisitor(&f1, CEB);

                mS.reset();
                rc = X.getField(&mS, &error, false, fldName);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, CEB, S.asElemRef(),
                             CEB == S.asElemRef());
                LOOP_ASSERT(S, !S.isNull());

                mS.makeNull();

                mS.reset();
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

                        mS.reset();
                        rc = X.getField(&mS, &error, false, fldName);
                        ASSERT(!rc);
                        LOOP3_ASSERT(LINE, CEN, S.asElemRef(),
                                     CEN == S.asElemRef());
                        LOOP_ASSERT(S, !S.isNull());

                        mS.makeNull();

                        mS.reset();
                        rc = X.getField(&mS, &error, false, fldName);
                        ASSERT(!rc);
                        LOOP_ASSERT(S, S.isNull());

                        rc = mX.setField(&mS, &error, fldName, A06);
                        ASSERT(!rc);
                        LOOP3_ASSERT(LINE, CEA, S.asElemRef(),
                                     CEA == S.asElemRef());
                        LOOP_ASSERT(S, !S.isNull());

                        mS.reset();
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

                        mS.reset();
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

                        mS.reset();
                        rc = X.getField(&mS, &error, false, fldName);
                        ASSERT(!rc);
                        LOOP3_ASSERT(LINE, CEN, S.asElemRef(),
                                     CEN == S.asElemRef());
                        LOOP_ASSERT(S, !S.isNull());

                        mS.makeNull();

                        mS.reset();
                        rc = X.getField(&mS, &error, false, fldName);
                        ASSERT(!rc);
                        LOOP_ASSERT(S, S.isNull());

                        rc = mX.setField(&mS, &error, fldName, A02);
                        ASSERT(!rc);
                        LOOP3_ASSERT(LINE, CEA, S.asElemRef(),
                                     CEA == S.asElemRef());
                        LOOP_ASSERT(S, !S.isNull());

                        mS.reset();
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

                        mS.reset();
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
                    Error error;
                    Obj mS; const Obj& S = mS;
                    int rc = mY.makeSelection(&mS, &error, fldName);
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

                mS.reset();
                rc = X.getField(&mS, &error, false, fldName);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, CEA, S.asElemRef(),
                             CEA == S.asElemRef());
                LOOP_ASSERT(S, !S.isNull());

                mS.makeNull();

                mS.reset();
                rc = X.getField(&mS, &error, false, fldName);
                ASSERT(!rc);
                LOOP_ASSERT(S, S.isNull());

                Obj mZ; const Obj& Z = mZ;
                rc = ggAggData(&mZ, r, &t);
                ASSERT(!rc);

                if (RecDef::BDEM_CHOICE_RECORD == r.recordType()) {
                    Error error;
                    Obj mS; const Obj& S = mS;
                    int rc = mZ.makeSelection(&mS, &error, fldName);
                    ASSERT(!rc);
                }

                mT.reset();
                rc = mZ.setField(&mT, &error, fldName, CEB);
                ASSERT(!rc);

                mS.reset();
                rc = mX.setField(&mS, &error, fldName, T);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, CEB, S.asElemRef(),
                             CEB == S.asElemRef());
                LOOP_ASSERT(S, !S.isNull());

                mS.reset();
                rc = X.getField(&mS, &error, false, fldName);
                ASSERT(!rc);
                LOOP3_ASSERT(LINE, CEB, S.asElemRef(),
                             CEB == S.asElemRef());
                LOOP_ASSERT(S, !S.isNull());

                mS.makeNull();

                mS.reset();
                rc = X.getField(&mS, &error, false, fldName);
                ASSERT(!rc);
                LOOP_ASSERT(S, S.isNull());

                // Test empty string if aggregate stores a choice
                if (RecDef::BDEM_CHOICE_RECORD == r.recordType()) {
                    mS.reset();
                    rc = mX.setField(&mS, &error, "", CEA);
                    ASSERT(!rc);
                    LOOP3_ASSERT(LINE, CEA, S.asElemRef(),
                                 CEA == S.asElemRef());
                    LOOP_ASSERT(S, !S.isNull());

                    mS.makeNull();

                    mS.reset();
                    rc = X.getField(&mS, &error, false, fldName);
                    ASSERT(!rc);
                    LOOP_ASSERT(S, S.isNull());
                }

                // Test setValue with bdem_ConstElemRef and bdem_ElemRef

                mS.reset();
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
// TBD: Uncomment to remove mem leak
#if 0
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
                mX.reset();
                rc = mX.setField(&err3, &error, fldName, CEA);
                ASSERT(rc);
                LOOP_ASSERT(error.code(),
                            Error::BCEM_ERR_NOT_A_RECORD == error.code());
#endif

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
                P(agg1)

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
                Obj agg1;
                agg1.setSchemaPointer(schema.ptr());
                agg1.setRecordDefPointer(schema->lookupRecord("Level2"));
                agg1.setDataType(ET::BDEM_LIST);
                agg1.setDataPointer(&list2);

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
                agg5.setDataPointer(&list1);

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
