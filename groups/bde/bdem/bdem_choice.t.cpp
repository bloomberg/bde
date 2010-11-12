// bdem_choice.t.cpp                                                  -*-C++-*-

#include <bdem_choice.h>

#include <bdem_choicearrayimp.h>
#include <bdem_choicearrayitem.h>
#include <bdem_choiceheader.h>
#include <bdem_elemattrlookup.h>
#include <bdem_functiontemplates.h>
#include <bdem_listimp.h>
#include <bdem_properties.h>
#include <bdem_rowdata.h>
#include <bdem_tableimp.h>

#include <bdema_sequentialallocator.h>
#include <bdesb_memoutstreambuf.h>
#include <bdesb_fixedmeminstreambuf.h>
#include <bdetu_unset.h>
#include <bdeu_printmethods.h>
#include <bdex_byteoutstream.h>
#include <bdex_byteinstream.h>
#include <bdex_testoutstream.h>
#include <bdex_testinstream.h>

#include <bslalg_typetraits.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_newdeleteallocator.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>
#include <bslmf_isconvertible.h>
#include <bsls_types.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>

#include <bsl_cstdlib.h>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// Although 'bdem_Choice' is a fully value-semantic class, all of the
// substantive implementation resides in 'bdem_ChoiceImp', which in turn
// resides in a separate (lower-level) component.
//
// Our goal here is primarily to ensure correct function signatures and that
// function arguments and return values are being propagated properly.
// Important issues related to the various allocation strategies, aliasing,
// thread safety, and exception neutrality are also tested via this interface.
//-----------------------------------------------------------------------------
// CLASS METHODS
// [ 8] static int maxSupportedBdexVersion();
//
// CREATORS
// [10] bdem_Choice(bslma_Allocator *alloc = 0);
// [10] bdem_Choice(AggOptionAllocationStrategy  mode,
//                  bslma_Allocator             *alloc = 0);
// [10] bdem_Choice(const bdem_ElemType::Type    typesCatalog[],
//                  int                          typesCatalogLen,
//                  bslma_Allocator             *alloc = 0);
// [10] bdem_Choice(const bdem_ElemType::Type    typesCatalog[],
//                  int                          typesCatalogLen,
//                  AggOptionAllocationStrategy  allocMode,
//                  bslma_Allocator             *alloc = 0);
// [10] bdem_Choice(const TypesCatalog&          typesCatalog,
//                  bslma_Allocator             *alloc = 0);
// [10] bdem_Choice(const TypesCatalog&          typesCatalog,
//                  AggOptionAllocationStrategy  allocMode,
//                  bslma_Allocator             *alloc = 0);
// [ 6] bdem_Choice(const bdem_Choice&           original,
//                  bslma_Allocator             *alloc = 0);
// [ 6] bdem_Choice(const bdem_Choice&           original,
//                  AggOptionAllocationStrategy  allocMode,
//                  bslma_Allocator             *alloc = 0);
// [ 6] bdem_Choice(const bdem_ChoiceArrayItem&  original,
//                  bslma_Allocator             *alloc = 0);
// [ 6] bdem_Choice(const bdem_ChoiceArrayItem&  original,
//                  AggOptionAllocationStrategy  allocMode,
//                  bslma_Allocator             *alloc = 0);
// [10] ~bdem_Choice();
//
// MANIPULATORS
// [ 7] bdem_Choice& operator=(const bdem_ChoiceArrayItem& rhs);
// [ 7] bdem_Choice& operator=(const bdem_Choice& rhs);
// [ 3] bdem_ChoiceArrayItem& item();
// [ 3] bdem_ElemRef makeSelection(int index)
// [ 3] bdem_ElemRef selection()
// [12] bool& theBool();
// [12] char& theChar();
// [12] short& theShort();
// [12] int& theInt();
// [12] bsls_Types::Int64& theInt64();
// [12] float& theFloat();
// [12] double& theDouble();
// [12] bsl::string& theString();
// [12] bdet_Datetime& theDatetime();
// [12] bdet_DatetimeTz& theDatetimeTz();
// [12] bdet_Date& theDate();
// [12] bdet_DateTz& theDateTz();
// [12] bdet_Time& theTime();
// [12] bdet_TimeTz& theTimeTz();
// [12] bsl::vector<bool>& theBoolArray();
// [12] bsl::vector<char>& theCharArray();
// [12] bsl::vector<short>& theShortArray();
// [12] bsl::vector<int>& theIntArray();
// [12] bsl::vector<bsls_Types::Int64>& theInt64Array();
// [12] bsl::vector<float>& theFloatArray();
// [12] bsl::vector<double>& theDoubleArray();
// [12] bsl::vector<bsl::string>& theStringArray();
// [12] bsl::vector<bdet_Datetime>& theDatetimeArray();
// [12] bsl::vector<bdet_DatetimeTz>& theDatetimeTzArray();
// [12] bsl::vector<bdet_Date>& theDateArray();
// [12] bsl::vector<bdet_DateTz>& theDateTzArray();
// [12] bsl::vector<bdet_Time>& theTimeArray();
// [12] bsl::vector<bdet_TimeTz>& theTimeTzArray();
// [12] bdem_List& theList();
// [12] bdem_Table& theTable();
// [12] bdem_Choice& theChoice();
// [12] bdem_ChoiceArray& theChoiceArray();
// [13] theModifiableBool();
// [13] theModifiableChar();
// [13] theModifiableShort();
// [13] theModifiableInt();
// [13] theModifiableInt64();
// [13] theModifiableFloat();
// [13] theModifiableDouble();
// [13] theModifiableString();
// [13] theModifiableDatetime();
// [13] theModifiableDatetimeTz();
// [13] theModifiableDate();
// [13] theModifiableDateTz();
// [13] theModifiableTime();
// [13] theModifiableTimeTz();
// [13] theModifiableBoolArray();
// [13] theModifiableCharArray();
// [13] theModifiableShortArray();
// [13] theModifiableIntArray();
// [13] theModifiableInt64Array();
// [13] theModifiableFloatArray();
// [13] theModifiableDoubleArray();
// [13] theModifiableStringArray();
// [13] theModifiableDatetimeArray();
// [13] theModifiableDatetimeTzArray();
// [13] theModifiableDateArray();
// [13] theModifiableDateTzArray();
// [13] theModifiableTimeArray();
// [13] theModifiableTimeTzArray();
// [13] theModifiableList();
// [13] theModifiableTable();
// [13] theModifiableChoice();
// [13] theModifiableChoiceArray();
// [ 9] void addSelection(bdem_ElemType::Type elemType);
// [11] void reset();
// [11] void reset(const bdem_ElemType::Type typesCatalog[],
//                 int                       typesCatalogLen);
// [11] void reset(const bsl::vector<bdem_ElemType::Type>& typesCatalog);
// [ 8] STREAM& bdexStreamIn(STREAM& stream, int version)
//
// ACCESSORS
// [ 3] int numSelections() const
// [ 3] int selector() const
// [ 3] const bdem_ChoiceArrayItem& item() const;
// [ 3] bdem_ElemType::Type selectionType() const
// [ 3] bdem_ElemType::Type selectionType(int index) const
// [ 3] void selectionTypes(bsl::vector<bdem_ElemType::Type> *result) const
// [ 3] bdem_ConstElemRef selection() const
// [12]  const bool& theBool() const;
// [12]  const char& theChar() const;
// [12]  const short& theShort() const;
// [12]  const int& theInt() const;
// [12]  const bsls_Types::Int64& theInt64() const;
// [12]  const float& theFloat() const;
// [12]  const double& theDouble() const;
// [12]  const bsl::string& theString() const;
// [12]  const bdet_Datetime& theDatetime() const;
// [12]  const bdet_DatetimeTz& theDatetimeTz() const;
// [12]  const bdet_Date& theDate() const;
// [12]  const bdet_DateTz& theDateTz() const;
// [12]  const bdet_Time& theTime() const;
// [12]  const bdet_TimeTz& theTimeTz() const;
// [12]  const bsl::vector<bool>& theBoolArray() const;
// [12]  const bsl::vector<char>& theCharArray() const;
// [12]  const bsl::vector<short>& theShortArray() const;
// [12]  const bsl::vector<int>& theIntArray() const;
// [12]  const bsl::vector<bsls_Types::Int64>& theInt64Array() const;
// [12]  const bsl::vector<float>& theFloatArray() const;
// [12]  const bsl::vector<double>& theDoubleArray() const;
// [12]  const bsl::vector<bsl::string>& theStringArray() const;
// [12]  const bsl::vector<bdet_Datetime>& theDatetimeArray() const;
// [12]  const bsl::vector<bdet_DatetimeTz>& theDatetimeTzArray() const;
// [12]  const bsl::vector<bdet_Date>& theDateArray() const;
// [12]  const bsl::vector<bdet_DateTz>& theDateTzArray() const;
// [12]  const bsl::vector<bdet_Time>& theTimeArray() const;
// [12]  const bsl::vector<bdet_TimeTz>& theTimeTzArray() const;
// [12]  const bdem_List& theList() const;
// [12]  const bdem_Table& theTable() const;
// [12]  const bdem_Choice& theChoice() const;
// [12]  const bdem_ChoiceArray& theChoiceArray() const;
// [ 8] STREAM& bdexStreamOut(STREAM& stream, int version) const
// [ 4] bsl::ostream& print(bsl::ostream& stream,
//                          int           level = 0,
//                          int           spacesPerLevel = 4) const;
// FREE OPERATORS
// [ 5] bool operator==(const bdem_Choice& lhs, const bdem_Choice& rhs);
// [ 5] bool operator!=(const bdem_Choice& lhs, const bdem_Choice& rhs);
// [ 4] bsl::ostream& operator<<(bsl::ostream& stream, const bdem_Choice& rhs);
//
// [ 1] BREATHING TEST
// [ 2] TEST APPARATUS
// [15] USAGE EXAMPLE

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
#define P_(X) bsl::cout << #X " = " << (X) << ", " << bsl::flush;
                                                        // P(X) w/o '\n'
#define L_ __LINE__                              // current Line number
#define T_ bsl::cout << "\t" << bsl::flush;      // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdem_Choice              Obj;

typedef bdem_ChoiceArrayItem     ChoiceArrayItem;

typedef bdem_Properties          Prop;
typedef bdem_Descriptor          Desc;
typedef bdem_ElemType            EType;
typedef bdem_ElemRef             ERef;
typedef bdem_ConstElemRef        CERef;
typedef bdem_ElemType            EType;
typedef bdem_AggregateOption     AggOption;

typedef bsls_Types::Int64        Int64;

typedef bdet_Datetime            Datetime;
typedef bdet_Date                Date;
typedef bdet_Time                Time;
typedef bdet_DatetimeTz          DatetimeTz;
typedef bdet_DateTz              DateTz;
typedef bdet_TimeTz              TimeTz;

typedef bsl::vector<EType::Type> TypesCatalog;

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;

//=============================================================================
//                  GLOBAL STUB CLASSES FOR TESTING
//-----------------------------------------------------------------------------

namespace BloombergLP {

                        // ============================
                        // DUMMY class bdem_ChoiceArray
                        // ============================

class bdem_ChoiceArray {
    // This class is a dummy bdem_ChoiceArray class to facilitate testing.
    // Note that it contains only the functions called in this test driver
    // and not the entire interface of bdem_ChoiceArray.

    bdem_ChoiceArrayImp d_choiceArrayImp;

    friend bool operator==(const bdem_ChoiceArray& lhs,
                           const bdem_ChoiceArray& rhs);
  public:
    // CREATORS
    bdem_ChoiceArray();
    bdem_ChoiceArray(const EType::Type types[], int numTypes);

    // MANIPULATORS
    void appendNullItems(int elemCount);
    bdem_ChoiceArrayItem& operator[](int index);
};

bool operator==(const bdem_ChoiceArray& lhs, const bdem_ChoiceArray& rhs);
bool operator!=(const bdem_ChoiceArray& lhs, const bdem_ChoiceArray& rhs);

                        // =====================
                        // DUMMY class bdem_List
                        // =====================

class bdem_List {
    // This class is a dummy bdem_ChoiceArray class to facilitate testing.
    // Note that it contains only the functions called in this test driver
    // and not the entire interface of bdem_ChoiceArray.

    bdem_ListImp d_listImp;

    friend bool operator==(const bdem_List& lhs, const bdem_List& rhs);

  public:
    // CREATORS
    bdem_List();
    bdem_List(const bdem_List& original);

    // MANIPULATORS
    void appendInt(int value);
    void appendDouble(double value);
    void appendString(const bsl::string& value);
    void appendStringArray(const bsl::vector<bsl::string>& value);
};

bool operator==(const bdem_List& lhs, const bdem_List& rhs);
bool operator!=(const bdem_List& lhs, const bdem_List& rhs);

                        // =======================
                        // DUMMY class bdem_RowRef
                        // =======================

class bdem_RowRef {
    // This class is a dummy bdem_Row class to facilitate testing
    // Note that it contains only the functions called in this test driver
    // and not the entire interface of bdem_Table.

    bdem_RowData *d_rowImp;

  public:
    // CREATORS
    bdem_RowRef(bdem_RowData *row);
    bdem_RowRef(const bdem_RowRef& original);

    // MANIPULATORS
    bdem_ElemRef operator[](int index);
};

                        // ======================
                        // DUMMY class bdem_Table
                        // ======================

class bdem_Table {
    // This class is a dummy bdem_Table class to facilitate testing

    bdem_TableImp d_tableImp;

    friend bool operator==(const bdem_Table& lhs, const bdem_Table& rhs);

  public:
    // CREATORS
    bdem_Table();
    bdem_Table(const EType::Type types[], int numTypes);
    bdem_Table(const bdem_Table& original);

    // MANIPULATORS
    void appendNullRow();
    bdem_RowRef operator[](int index);
};

bool operator==(const bdem_Table& lhs, const bdem_Table& rhs);
bool operator!=(const bdem_Table& lhs, const bdem_Table& rhs);

                        // ============================
                        // DUMMY class bdem_ChoiceArray
                        // ============================

bdem_ChoiceArray::bdem_ChoiceArray()
{
}

bdem_ChoiceArray::bdem_ChoiceArray(const EType::Type types[], int numTypes)
: d_choiceArrayImp(types,
                   numTypes,
                   bdem_ElemAttrLookup::lookupTable(),
                   bdem_AggregateOption::BDEM_PASS_THROUGH)
{
}

void bdem_ChoiceArray::appendNullItems(int elemCount)
{
    d_choiceArrayImp.insertNullItems(d_choiceArrayImp.length(), elemCount);
}

bdem_ChoiceArrayItem& bdem_ChoiceArray::operator[](int index)
{
    return (bdem_ChoiceArrayItem&) d_choiceArrayImp.theModifiableItem(index);
}

bool operator==(const bdem_ChoiceArray& lhs, const bdem_ChoiceArray& rhs)
{
    return lhs.d_choiceArrayImp == rhs.d_choiceArrayImp;
}

bool operator!=(const bdem_ChoiceArray& lhs, const bdem_ChoiceArray& rhs)
{
    return !(lhs == rhs);
}

                        // =====================
                        // DUMMY class bdem_List
                        // =====================

bdem_List::bdem_List()
: d_listImp(bdem_AggregateOption::BDEM_PASS_THROUGH)
{
}

bdem_List::bdem_List(const bdem_List&  original)
: d_listImp((bdem_ListImp &) original,
            bdem_AggregateOption::BDEM_PASS_THROUGH)
{
}

void bdem_List::appendInt(int value)
{
    d_listImp.insertElement(d_listImp.length(),
                            &value,
                            &Prop::d_intAttr);
}

void bdem_List::appendDouble(double value)
{
    d_listImp.insertElement(d_listImp.length(),
                            &value,
                            &Prop::d_doubleAttr);
}

void bdem_List::appendString(const bsl::string& value)
{
    d_listImp.insertElement(d_listImp.length(),
                            &value,
                            &Prop::d_stringAttr);
}

void bdem_List::appendStringArray(const bsl::vector<bsl::string>& value)
{
    d_listImp.insertElement(d_listImp.length(),
                            &value,
                            &Prop::d_stringArrayAttr);
}

bool operator==(const bdem_List& lhs, const bdem_List& rhs)
{
    return lhs.d_listImp == rhs.d_listImp;
}

bool operator!=(const bdem_List& lhs, const bdem_List& rhs)
{
    return !(lhs == rhs);
}

                        // =======================
                        // DUMMY class bdem_RowRef
                        // =======================

bdem_RowRef::bdem_RowRef(bdem_RowData *row)
: d_rowImp(row)
{
}

bdem_RowRef::bdem_RowRef(const bdem_RowRef& original)
: d_rowImp(original.d_rowImp)
{
}

bdem_ElemRef bdem_RowRef::operator[](int index)
{
    return d_rowImp->elemRef(index);
}

                        // ======================
                        // DUMMY class bdem_Table
                        // ======================

bdem_Table::bdem_Table()
: d_tableImp(bdem_AggregateOption::BDEM_PASS_THROUGH)
{
}

bdem_Table::bdem_Table(const EType::Type types[], int numTypes)
: d_tableImp(types,
             numTypes,
             bdem_ElemAttrLookup::lookupTable(),
             bdem_AggregateOption::BDEM_PASS_THROUGH)
{
}

bdem_Table::bdem_Table(const bdem_Table& original)
: d_tableImp(original.d_tableImp, bdem_AggregateOption::BDEM_PASS_THROUGH)
{
}

void bdem_Table::appendNullRow()
{
    d_tableImp.insertNullRows(d_tableImp.numRows(), 1);
}

bdem_RowRef bdem_Table::operator[](int index)
{
    return bdem_RowRef(&d_tableImp.theModifiableRow(index));
}

bool operator==(const bdem_Table& lhs, const bdem_Table& rhs)
{
    return lhs.d_tableImp == rhs.d_tableImp;
}

bool operator!=(const bdem_Table& lhs, const bdem_Table& rhs)
{
    return !(lhs == rhs);
}

}

//=============================================================================
//                           GLOBAL DATA FOR TESTING
//-----------------------------------------------------------------------------

        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // Create Three Distinct Exemplars For Each Element Type
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const bool             A22 = true;
const bool             B22 = false;
const bool             N22 = bdetu_Unset<bool>::unsetValue();

const char             A00 = 'A';
const char             B00 = 'B';
const char             N00 = bdetu_Unset<char>::unsetValue();

const short            A01 = -1;
const short            B01 = -2;
const short            N01 = bdetu_Unset<short>::unsetValue();

const int              A02 = 10;
const int              B02 = 20;
const int              N02 = bdetu_Unset<int>::unsetValue();

const bsls_Types::Int64
                       A03 = -100;
const bsls_Types::Int64
                       B03 = -200;
const bsls_Types::Int64
                       N03 = bdetu_Unset<bsls_Types::Int64>::unsetValue();

const float            A04 = -1.5;
const float            B04 = -2.5;
const float            N04 = bdetu_Unset<float>::unsetValue();

const double           A05 = 10.5;
const double           B05 = 20.5;
const double           N05 = bdetu_Unset<double>::unsetValue();

const bsl::string      A06 = "one";
const bsl::string      B06 = "two";
const bsl::string      N06 = bdetu_Unset<bsl::string>::unsetValue();

// Note: bdet_Datetime X07 implemented in terms of X08 and X09.

const bdet_Date        A08(2000,  1, 1);
const bdet_Date        B08(9999, 12,31);
const bdet_Date        N08 = bdetu_Unset<bdet_Date>::unsetValue();

const bdet_Time        A09(0, 1, 2, 3);
const bdet_Time        B09(4, 5, 6, 789);
const bdet_Time        N09 = bdetu_Unset<bdet_Time>::unsetValue();

const bdet_Datetime    A07(A08, A09);
const bdet_Datetime    B07(B08, B09);
const bdet_Datetime    N07 = bdetu_Unset<bdet_Datetime>::unsetValue();

const bdet_DateTz        A24(A08, -5);
const bdet_DateTz        B24(B08, -4);
const bdet_DateTz        N24 = bdetu_Unset<bdet_DateTz>::unsetValue();

const bdet_TimeTz        A25(A09, -5);
const bdet_TimeTz        B25(B09, -5);
const bdet_TimeTz        N25 = bdetu_Unset<bdet_TimeTz>::unsetValue();

const bdet_DatetimeTz    A23(A07, -5);
const bdet_DatetimeTz    B23(B07, -5);
const bdet_DatetimeTz    N23 = bdetu_Unset<bdet_DatetimeTz>::unsetValue();

static bsl::vector<bool>                        fA26() {
       bsl::vector<bool> t;          t.push_back(A22); return t; }
static bsl::vector<bool>                        fB26() {
       bsl::vector<bool> t;          t.push_back(B22); return t; }
const  bsl::vector<bool>                         A26 = fA26();
const  bsl::vector<bool>                         B26 = fB26();
const  bsl::vector<bool>                         N26;

static bsl::vector<char>                        fA10() {
       bsl::vector<char> t;          t.push_back(A00); return t; }
static bsl::vector<char>                        fB10() {
       bsl::vector<char> t;          t.push_back(B00); return t; }
const  bsl::vector<char>                         A10 = fA10();
const  bsl::vector<char>                         B10 = fB10();
const  bsl::vector<char>                         N10;

static bsl::vector<short>                       fA11() {
       bsl::vector<short> t;         t.push_back(A01); return t; }
static bsl::vector<short>                       fB11() {
       bsl::vector<short> t;         t.push_back(B01); return t; }
const  bsl::vector<short>                        A11 = fA11();
const  bsl::vector<short>                        B11 = fB11();
const  bsl::vector<short>                        N11;

static bsl::vector<int>                         fA12() {
       bsl::vector<int> t;           t.push_back(A02); return t; }
static bsl::vector<int>                         fB12() {
       bsl::vector<int> t;           t.push_back(B02); return t; }
const  bsl::vector<int>                          A12 = fA12();
const  bsl::vector<int>                          B12 = fB12();
const  bsl::vector<int>                          N12;

static bsl::vector<bsls_Types::Int64>           fA13() {
       bsl::vector<bsls_Types::Int64> t;
                                     t.push_back(A03); return t; }
static bsl::vector<bsls_Types::Int64>           fB13() {
       bsl::vector<bsls_Types::Int64> t;
                                     t.push_back(B03); return t; }
const  bsl::vector<bsls_Types::Int64>            A13 = fA13();
const  bsl::vector<bsls_Types::Int64>            B13 = fB13();
const  bsl::vector<bsls_Types::Int64>            N13;

static bsl::vector<float>                       fA14() {
       bsl::vector<float> t;         t.push_back(A04); return t; }
static bsl::vector<float>                       fB14() {
       bsl::vector<float> t;         t.push_back(B04); return t; }
const  bsl::vector<float>                        A14 = fA14();
const  bsl::vector<float>                        B14 = fB14();
const  bsl::vector<float>                        N14;

static bsl::vector<double>                      fA15() {
       bsl::vector<double> t;        t.push_back(A05); return t; }
static bsl::vector<double>                      fB15() {
       bsl::vector<double> t;        t.push_back(B05); return t; }
const  bsl::vector<double>                       A15 = fA15();
const  bsl::vector<double>                       B15 = fB15();
const  bsl::vector<double>                       N15;

static bsl::vector<bsl::string>                 fA16() {
       bsl::vector<bsl::string> t;   t.push_back(A06); return t; }
static bsl::vector<bsl::string>                 fB16() {
       bsl::vector<bsl::string> t;   t.push_back(B06); return t; }
const  bsl::vector<bsl::string>                  A16 = fA16();
const  bsl::vector<bsl::string>                  B16 = fB16();
const  bsl::vector<bsl::string>                  N16;

static bsl::vector<bdet_Datetime>               fA17() {
       bsl::vector<bdet_Datetime> t; t.push_back(A07); return t; }
static bsl::vector<bdet_Datetime>               fB17() {
       bsl::vector<bdet_Datetime> t; t.push_back(B07); return t; }
const  bsl::vector<bdet_Datetime>                A17 = fA17();
const  bsl::vector<bdet_Datetime>                B17 = fB17();
const  bsl::vector<bdet_Datetime>                N17;

static bsl::vector<bdet_Date>                   fA18() {
       bsl::vector<bdet_Date> t;     t.push_back(A08); return t; }
static bsl::vector<bdet_Date>                   fB18() {
       bsl::vector<bdet_Date> t;     t.push_back(B08); return t; }
const  bsl::vector<bdet_Date>                    A18 = fA18();
const  bsl::vector<bdet_Date>                    B18 = fB18();
const  bsl::vector<bdet_Date>                    N18;

static bsl::vector<bdet_Time>                   fA19() {
       bsl::vector<bdet_Time> t;     t.push_back(A09); return t; }
static bsl::vector<bdet_Time>                   fB19() {
       bsl::vector<bdet_Time> t;     t.push_back(B09); return t; }
const  bsl::vector<bdet_Time>                    A19 = fA19();
const  bsl::vector<bdet_Time>                    B19 = fB19();
const  bsl::vector<bdet_Time>                    N19;

static bsl::vector<bdet_DatetimeTz>               fA27() {
       bsl::vector<bdet_DatetimeTz> t; t.push_back(A23); return t; }
static bsl::vector<bdet_DatetimeTz>               fB27() {
       bsl::vector<bdet_DatetimeTz> t; t.push_back(B23); return t; }
const  bsl::vector<bdet_DatetimeTz>                A27 = fA27();
const  bsl::vector<bdet_DatetimeTz>                B27 = fB27();
const  bsl::vector<bdet_DatetimeTz>                N27;

static bsl::vector<bdet_DateTz>                   fA28() {
       bsl::vector<bdet_DateTz> t;     t.push_back(A24); return t; }
static bsl::vector<bdet_DateTz>                   fB28() {
       bsl::vector<bdet_DateTz> t;     t.push_back(B24); return t; }
const  bsl::vector<bdet_DateTz>                    A28 = fA28();
const  bsl::vector<bdet_DateTz>                    B28 = fB28();
const  bsl::vector<bdet_DateTz>                    N28;

static bsl::vector<bdet_TimeTz>                   fA29() {
       bsl::vector<bdet_TimeTz> t;     t.push_back(A25); return t; }
static bsl::vector<bdet_TimeTz>                   fB29() {
       bsl::vector<bdet_TimeTz> t;     t.push_back(B25); return t; }
const  bsl::vector<bdet_TimeTz>                    A29 = fA29();
const  bsl::vector<bdet_TimeTz>                    B29 = fB29();
const  bsl::vector<bdet_TimeTz>                    N29;

static bdem_List                            fA20() {
       bdem_List t;              t.appendInt(A02);
                                 t.appendDouble(A05);
                                 t.appendString(A06);
                                 t.appendStringArray(A16); return t; }
static bdem_List                            fB20() {
       bdem_List t;              t.appendInt(B02);
                                 t.appendDouble(B05);
                                 t.appendString(B06);
                                 t.appendStringArray(B16); return t; }

// Note: on Windows, the function pointers in bdem_Properties::d_intAttr
// are 0x00000000 before entering main().  Consequently, the calls in
// fAxx() and fBxx() cause a runtime exception.  Our workaround is to
// initialize these constants at the start of main.

#ifdef BSLS_PLATFORM__CMP_MSVC
bdem_List                                    mA20; // Initialized in main
bdem_List                                    mB20; // Initialized in main
const  bdem_List&                            A20 = mA20;
const  bdem_List&                            B20 = mB20;
#else
const  bdem_List                             A20 = fA20();
const  bdem_List                             B20 = fB20();
#endif
const  bdem_List                             N20;

static bdem_Table                            fA21() {
       const bdem_ElemType::Type types[] =
       {
           EType::BDEM_INT,
           EType::BDEM_DOUBLE,
           EType::BDEM_STRING,
           EType::BDEM_STRING_ARRAY
       };
       const int NUM_TYPES = sizeof(types) / sizeof(*types);
       bdem_Table t(types, NUM_TYPES); t.appendNullRow();
                            t[0][0].theModifiableInt() = A02;
                            t[0][1].theModifiableDouble() = A05;
                            t[0][2].theModifiableString() = A06;
                            t[0][3].theModifiableStringArray() = A16; return t;
}

static bdem_Table                            fB21() {
       const bdem_ElemType::Type types[] =
       {
           EType::BDEM_INT,
           EType::BDEM_DOUBLE,
           EType::BDEM_STRING,
           EType::BDEM_STRING_ARRAY
       };
       const int NUM_TYPES = sizeof(types) / sizeof(*types);
       bdem_Table t(types, NUM_TYPES); t.appendNullRow();
                          t[0][0].theModifiableInt() = B02;
                          t[0][1].theModifiableDouble() = B05;
                          t[0][2].theModifiableString() = B06;
                          t[0][3].theModifiableStringArray() = B16; return t;
}

// Note: on Windows, the function pointers in bdem_Properties::d_intAttr
// are 0x00000000 before entering main().  Consequently, the calls in
// fAxx() and fBxx() cause a runtime exception.  Our workaround is to
// initialize these constants at the start of main.
#ifdef BSLS_PLATFORM__CMP_MSVC
bdem_Table                                    mA21;
bdem_Table                                    mB21;
const  bdem_Table&                            A21 = mA21;
const  bdem_Table&                            B21 = mB21;
#else
const  bdem_Table                             A21 = fA21();
const  bdem_Table                             B21 = fB21();
#endif
const  bdem_Table                             N21;

static bdem_Choice                            fA30() {
       const bdem_ElemType::Type types[] =
       {
           EType::BDEM_INT,
           EType::BDEM_DOUBLE,
           EType::BDEM_STRING,
           EType::BDEM_STRING_ARRAY
       };
       const int NUM_TYPES = sizeof(types) / sizeof(*types);
       bdem_Choice t(types, NUM_TYPES);
                              t.makeSelection(2).theModifiableString() = A06;
                              return t;
}

static bdem_Choice                            fB30() {
       const bdem_ElemType::Type types[] =
       {
           EType::BDEM_INT,
           EType::BDEM_DOUBLE,
           EType::BDEM_STRING,
           EType::BDEM_STRING_ARRAY
       };
       const int NUM_TYPES = sizeof(types) / sizeof(*types);
       bdem_Choice t(types, NUM_TYPES);
                          t.makeSelection(3).theModifiableStringArray() = A16;
                          return t;
}

// Note: on Windows, the function pointers in bdem_Properties::d_intAttr
// are 0x00000000 before entering main().  Consequently, the calls in
// fAxx() and fBxx() cause a runtime exception.  Our workaround is to
// initialize these constants at the start of main.
#ifdef BSLS_PLATFORM__CMP_MSVC
bdem_Choice                                    mA30;
bdem_Choice                                    mB30;
const  bdem_Choice&                            A30 = mA30;
const  bdem_Choice&                            B30 = mB30;
#else
const  bdem_Choice                             A30 = fA30();
const  bdem_Choice                             B30 = fB30();
#endif
const  bdem_Choice                             N30;

static bdem_ChoiceArray                            fA31() {
       const bdem_ElemType::Type types[] =
       {
           EType::BDEM_INT,
           EType::BDEM_DOUBLE,
           EType::BDEM_STRING,
           EType::BDEM_STRING_ARRAY
       };
       const int NUM_TYPES = sizeof(types) / sizeof(*types);
       bdem_ChoiceArray t(types, NUM_TYPES); t.appendNullItems(4);
                      t[0].makeSelection(0).theModifiableInt() = A02;
                      t[1].makeSelection(1).theModifiableDouble() = A05;
                      t[2].makeSelection(2).theModifiableString() = A06;
                      t[3].makeSelection(3).theModifiableStringArray() = A16;
                      return t;
}

static bdem_ChoiceArray                            fB31() {
       const bdem_ElemType::Type types[] =
       {
           EType::BDEM_INT,
           EType::BDEM_DOUBLE,
           EType::BDEM_STRING,
           EType::BDEM_STRING_ARRAY
       };
       const int NUM_TYPES = sizeof(types) / sizeof(*types);
       bdem_ChoiceArray t(types, NUM_TYPES); t.appendNullItems(4);
                      t[0].makeSelection(0).theModifiableInt() = B02;
                      t[1].makeSelection(1).theModifiableDouble() = B05;
                      t[2].makeSelection(2).theModifiableString() = B06;
                      t[3].makeSelection(3).theModifiableStringArray() = B16;
                      return t;
}

// Note: on Windows, the function pointers in bdem_Properties::d_intAttr
// are 0x00000000 before entering main().  Consequently, the calls in
// fAxx() and fBxx() cause a runtime exception.  Our workaround is to
// initialize these constants at the start of main.
#ifdef BSLS_PLATFORM__CMP_MSVC
bdem_ChoiceArray                                    mA31;
bdem_ChoiceArray                                    mB31;
const  bdem_ChoiceArray&                            A31 = mA31;
const  bdem_ChoiceArray&                            B31 = mB31;
#else
const  bdem_ChoiceArray                             A31 = fA31();
const  bdem_ChoiceArray                             B31 = fB31();
#endif
const  bdem_ChoiceArray                             N31;

static char SPECIFICATIONS[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef";
    // This string stores the valid specification values used for constructing
    // a TypesCatalog.  Each character specifies an element type and the order
    // for character to element type mapping is similar to that in
    // bdem_elemtype.h.  Thus, 'A' corresponds to 'CHAR', 'B'- 'SHORT',
    // ... 'Z' - 'TIMETZ', 'a' - 'BOOL_ARRAY', ... 'f' - 'CHOICE_ARRAY'.
    // Additionally for this test 'g' - 'SmallString', 'h' -
    // 'SmallStringAlloc', 'i' - 'LargeString' and 'j' - 'LargeStringAlloc'.
static const int SPEC_LEN = sizeof(SPECIFICATIONS) - 1;

static const Desc *DESCRIPTORS[] =
    // This array stores the descriptors corresponding to each element
    // type specified in the spec string.
{
    &Prop::d_charAttr,
    &Prop::d_shortAttr,
    &Prop::d_intAttr,
    &Prop::d_int64Attr,
    &Prop::d_floatAttr,
    &Prop::d_doubleAttr,
    &Prop::d_stringAttr,
    &Prop::d_datetimeAttr,
    &Prop::d_dateAttr,
    &Prop::d_timeAttr,
    &Prop::d_charArrayAttr,
    &Prop::d_shortArrayAttr,
    &Prop::d_intArrayAttr,
    &Prop::d_int64ArrayAttr,
    &Prop::d_floatArrayAttr,
    &Prop::d_doubleArrayAttr,
    &Prop::d_stringArrayAttr,
    &Prop::d_datetimeArrayAttr,
    &Prop::d_dateArrayAttr,
    &Prop::d_timeArrayAttr,
    &bdem_ListImp::d_listAttr,
    &bdem_TableImp::d_tableAttr,
    &Prop::d_boolAttr,
    &Prop::d_datetimeTzAttr,
    &Prop::d_dateTzAttr,
    &Prop::d_timeTzAttr,
    &Prop::d_boolArrayAttr,
    &Prop::d_datetimeTzArrayAttr,
    &Prop::d_dateTzArrayAttr,
    &Prop::d_timeTzArrayAttr,
    &bdem_ChoiceImp::d_choiceAttr,
    &bdem_ChoiceArrayImp::d_choiceArrayAttr,
};
static const int NUM_DESCS = sizeof(DESCRIPTORS) / sizeof(*DESCRIPTORS);

static const void *VALUES_A[] =
    // This array stores a set of values corresponding to each element
    // type specified in the spec string.
{
    &A00,
    &A01,
    &A02,
    &A03,
    &A04,
    &A05,
    &A06,
    &A07,
    &A08,
    &A09,
    &A10,
    &A11,
    &A12,
    &A13,
    &A14,
    &A15,
    &A16,
    &A17,
    &A18,
    &A19,
    &A20,
    &A21,
    &A22,
    &A23,
    &A24,
    &A25,
    &A26,
    &A27,
    &A28,
    &A29,
    &A30,
    &A31,
};
static const int NUM_VALUESA = sizeof(VALUES_A) / sizeof(*VALUES_A);

static const void *VALUES_B[] =
    // This array stores a second set of values corresponding to each element
    // type specified in the spec string.
{
    &B00,
    &B01,
    &B02,
    &B03,
    &B04,
    &B05,
    &B06,
    &B07,
    &B08,
    &B09,
    &B10,
    &B11,
    &B12,
    &B13,
    &B14,
    &B15,
    &B16,
    &B17,
    &B18,
    &B19,
    &B20,
    &B21,
    &B22,
    &B23,
    &B24,
    &B25,
    &B26,
    &B27,
    &B28,
    &B29,
    &B30,
    &B31,
};
static const int NUM_VALUESB = sizeof(VALUES_B) / sizeof(*VALUES_B);

static const void *VALUES_N[] =
    // This array stores unset values corresponding to each element
    // type specified in the spec string.
{
    &N00,
    &N01,
    &N02,
    &N03,
    &N04,
    &N05,
    &N06,
    &N07,
    &N08,
    &N09,
    &N10,
    &N11,
    &N12,
    &N13,
    &N14,
    &N15,
    &N16,
    &N17,
    &N18,
    &N19,
    &N20,
    &N21,
    &N22,
    &N23,
    &N24,
    &N25,
    &N26,
    &N27,
    &N28,
    &N29,
    &N30,
    &N31,
};
static const int NUM_VALUESN = sizeof(VALUES_N) / sizeof(*VALUES_N);

struct  tempStruct {
    static int assertion1[SPEC_LEN == NUM_DESCS];
    static int assertion2[SPEC_LEN == NUM_VALUESA];
    static int assertion3[SPEC_LEN == NUM_VALUESB];
    static int assertion4[SPEC_LEN == NUM_VALUESN];
};

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

template <class TYPE>
int compare(const TYPE& lhs, const CERef& rhs, char spec)
    // Assign to the specified 'lhs' the value of the specified 'rhs'
    // according to the specified 'spec'.
{
    switch (spec) {
      case 'A': return lhs.theChar() == rhs.theChar();
      case 'B': return lhs.theShort() == rhs.theShort();
      case 'C': return lhs.theInt() == rhs.theInt();
      case 'D': return lhs.theInt64() == rhs.theInt64();
      case 'E': return lhs.theFloat() == rhs.theFloat();
      case 'F': return lhs.theDouble() == rhs.theDouble();
      case 'G': return lhs.theString() == rhs.theString();
      case 'H': return lhs.theDatetime() == rhs.theDatetime();
      case 'I': return lhs.theDate() == rhs.theDate();
      case 'J': return lhs.theTime() == rhs.theTime();
      case 'K': return lhs.theCharArray() == rhs.theCharArray();
      case 'L': return lhs.theShortArray() == rhs.theShortArray();
      case 'M': return lhs.theIntArray() == rhs.theIntArray();
      case 'N': return lhs.theInt64Array() == rhs.theInt64Array();
      case 'O': return lhs.theFloatArray() == rhs.theFloatArray();
      case 'P': return lhs.theDoubleArray() == rhs.theDoubleArray();
      case 'Q': return lhs.theStringArray() == rhs.theStringArray();
      case 'R': return lhs.theDatetimeArray() == rhs.theDatetimeArray();
      case 'S': return lhs.theDateArray() == rhs.theDateArray();
      case 'T': return lhs.theTimeArray() == rhs.theTimeArray();
      case 'U': return lhs.theList() == rhs.theList();
      case 'V': return lhs.theTable() == rhs.theTable();
      case 'W': return lhs.theBool() == rhs.theBool();
      case 'X': return lhs.theDatetimeTz() == rhs.theDatetimeTz();
      case 'Y': return lhs.theDateTz() == rhs.theDateTz();
      case 'Z': return lhs.theTimeTz() == rhs.theTimeTz();
      case 'a': return lhs.theBoolArray() == rhs.theBoolArray();
      case 'b': return lhs.theDatetimeTzArray() == rhs.theDatetimeTzArray();
      case 'c': return lhs.theDateTzArray() == rhs.theDateTzArray();
      case 'd': return lhs.theTimeTzArray() == rhs.theTimeTzArray();
      case 'e': return lhs.theChoice() == rhs.theChoice();
      case 'f': return lhs.theChoiceArray() == rhs.theChoiceArray();
      default: return 0;
    }
    return 0;
}

template <class TYPE>
void assign(TYPE *lhs, const CERef &rhs, char spec)
    // Assign to the specified 'lhs' the value of the specified 'rhs'
    // according to the specified 'spec'.
{
    switch (spec) {
      case 'A': lhs->theModifiableChar() = rhs.theChar(); break;
      case 'B': lhs->theModifiableShort() = rhs.theShort(); break;
      case 'C': lhs->theModifiableInt() = rhs.theInt(); break;
      case 'D': lhs->theModifiableInt64() = rhs.theInt64(); break;
      case 'E': lhs->theModifiableFloat() = rhs.theFloat(); break;
      case 'F': lhs->theModifiableDouble() = rhs.theDouble(); break;
      case 'G': lhs->theModifiableString() = rhs.theString(); break;
      case 'H': lhs->theModifiableDatetime() = rhs.theDatetime(); break;
      case 'I': lhs->theModifiableDate() = rhs.theDate(); break;
      case 'J': lhs->theModifiableTime() = rhs.theTime(); break;
      case 'K': lhs->theModifiableCharArray() = rhs.theCharArray(); break;
      case 'L': lhs->theModifiableShortArray() = rhs.theShortArray(); break;
      case 'M': lhs->theModifiableIntArray() = rhs.theIntArray(); break;
      case 'N': lhs->theModifiableInt64Array() = rhs.theInt64Array(); break;
      case 'O': lhs->theModifiableFloatArray() = rhs.theFloatArray(); break;
      case 'P': lhs->theModifiableDoubleArray() = rhs.theDoubleArray(); break;
      case 'Q': lhs->theModifiableStringArray() = rhs.theStringArray(); break;
      case 'R': lhs->theModifiableDatetimeArray() = rhs.theDatetimeArray();
                                                                         break;
      case 'S': lhs->theModifiableDateArray() = rhs.theDateArray(); break;
      case 'T': lhs->theModifiableTimeArray() = rhs.theTimeArray(); break;
      case 'U': lhs->theModifiableList() = rhs.theList(); break;
      case 'V': lhs->theModifiableTable() = rhs.theTable(); break;
      case 'W': lhs->theModifiableBool() = rhs.theBool(); break;
      case 'X': lhs->theModifiableDatetimeTz() = rhs.theDatetimeTz(); break;
      case 'Y': lhs->theModifiableDateTz() = rhs.theDateTz(); break;
      case 'Z': lhs->theModifiableTimeTz() = rhs.theTimeTz(); break;
      case 'a': lhs->theModifiableBoolArray() = rhs.theBoolArray(); break;
      case 'b': lhs->theModifiableDatetimeTzArray() = rhs.theDatetimeTzArray();
                                                                         break;
      case 'c': lhs->theModifiableDateTzArray() = rhs.theDateTzArray(); break;
      case 'd': lhs->theModifiableTimeTzArray() = rhs.theTimeTzArray(); break;
      case 'e': lhs->theModifiableChoice() = rhs.theChoice(); break;
      case 'f': lhs->theModifiableChoiceArray() = rhs.theChoiceArray(); break;
      default: return;
    }
}

template <class TYPE>
void set(TYPE *lhs, const CERef &rhs, int i, char spec)
    // Assign to the specified 'lhs' the value of the specified 'rhs'
    // according to the specified 'spec'.
{
    lhs->makeSelection(i);

    switch (spec) {
      case 'A': lhs->theModifiableChar() = rhs.theChar(); break;
      case 'B': lhs->theModifiableShort() = rhs.theShort(); break;
      case 'C': lhs->theModifiableInt() = rhs.theInt(); break;
      case 'D': lhs->theModifiableInt64() = rhs.theInt64(); break;
      case 'E': lhs->theModifiableFloat() = rhs.theFloat(); break;
      case 'F': lhs->theModifiableDouble() = rhs.theDouble(); break;
      case 'G': lhs->theModifiableString() = rhs.theString(); break;
      case 'H': lhs->theModifiableDatetime() = rhs.theDatetime(); break;
      case 'I': lhs->theModifiableDate() = rhs.theDate(); break;
      case 'J': lhs->theModifiableTime() = rhs.theTime(); break;
      case 'K': lhs->theModifiableCharArray() = rhs.theCharArray(); break;
      case 'L': lhs->theModifiableShortArray() = rhs.theShortArray(); break;
      case 'M': lhs->theModifiableIntArray() = rhs.theIntArray(); break;
      case 'N': lhs->theModifiableInt64Array() = rhs.theInt64Array(); break;
      case 'O': lhs->theModifiableFloatArray() = rhs.theFloatArray(); break;
      case 'P': lhs->theModifiableDoubleArray() = rhs.theDoubleArray(); break;
      case 'Q': lhs->theModifiableStringArray() = rhs.theStringArray(); break;
      case 'R': lhs->theModifiableDatetimeArray() = rhs.theDatetimeArray();
                                                                         break;
      case 'S': lhs->theModifiableDateArray() = rhs.theDateArray(); break;
      case 'T': lhs->theModifiableTimeArray() = rhs.theTimeArray(); break;
      case 'U': lhs->theModifiableList() = rhs.theList(); break;
      case 'V': lhs->theModifiableTable() = rhs.theTable(); break;
      case 'W': lhs->theModifiableBool() = rhs.theBool(); break;
      case 'X': lhs->theModifiableDatetimeTz() = rhs.theDatetimeTz(); break;
      case 'Y': lhs->theModifiableDateTz() = rhs.theDateTz(); break;
      case 'Z': lhs->theModifiableTimeTz() = rhs.theTimeTz(); break;
      case 'a': lhs->theModifiableBoolArray() = rhs.theBoolArray(); break;
      case 'b': lhs->theModifiableDatetimeTzArray() = rhs.theDatetimeTzArray();
                                                                         break;
      case 'c': lhs->theModifiableDateTzArray() = rhs.theDateTzArray(); break;
      case 'd': lhs->theModifiableTimeTzArray() = rhs.theTimeTzArray(); break;
      case 'e': lhs->theModifiableChoice() = rhs.theChoice(); break;
      case 'f': lhs->theModifiableChoiceArray() = rhs.theChoiceArray(); break;
      default: return;
    }
}

static EType::Type getElemType(char spec)
    // Return the element type corresponding to the specified
    // 'spec' value.  Valid input consists of uppercase letters where the
    // index of each letter is in "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef".
{
    char *p = bsl::strchr(SPECIFICATIONS, spec);
    LOOP_ASSERT(spec, p);
    int index = p - SPECIFICATIONS;
    LOOP3_ASSERT(*p, index, SPEC_LEN, index < SPEC_LEN);
    return (EType::Type) index;
}

static const Desc *getDescriptor(char spec)
    // Return the non-modifiable descriptor corresponding to the specified
    // 'spec' value.  Valid input consists of uppercase letters where the
    // index of each letter is in "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef".
{
    char *p = bsl::strchr(SPECIFICATIONS, spec);
    LOOP_ASSERT(spec, p);
    int index = p - SPECIFICATIONS;
    LOOP3_ASSERT(*p, index, SPEC_LEN, index < SPEC_LEN);
    return DESCRIPTORS[index];
}

static TypesCatalog ggCatalog(const char *s)
    // Return a types catalog containing the types corresponding to the
    // specified 's' specification string.
{
    TypesCatalog catalog;
    const char *q = s + bsl::strlen(s);
    for (const char *p = s; p != q; ++p) {
        catalog.push_back(getElemType(*p));
    }
    return catalog;
}

static const void *getValueA(char spec)
    // Return the 'A' value corresponding to the specified 'spec'.  Valid
    // input consists of uppercase letters where the index of each letter is
    // in "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef".
{
    char *p = bsl::strchr(SPECIFICATIONS, spec);
    LOOP_ASSERT(spec, p);
    int index = p - SPECIFICATIONS;
    LOOP3_ASSERT(*p, index, SPEC_LEN, index < SPEC_LEN);
    return VALUES_A[index];
}

static const void *getValueB(char spec)
    // Return the 'B' value corresponding to the specified 'spec'.
{
    char *p = bsl::strchr(SPECIFICATIONS, spec);
    LOOP_ASSERT(spec, p);
    int index = p - SPECIFICATIONS;
    LOOP3_ASSERT(*p, index, SPEC_LEN, index < SPEC_LEN);
    return VALUES_B[index];
}

static const void *getValueN(char spec)
    // Return the 'N' value corresponding to the specified 'spec'.
{
    char *p = bsl::strchr(SPECIFICATIONS, spec);
    LOOP_ASSERT(spec, p);
    int index = p - SPECIFICATIONS;
    LOOP3_ASSERT(*p, index, SPEC_LEN, index < SPEC_LEN);
    return VALUES_N[index];
}

static CERef getConstERefA(char spec)
{
    return CERef(getValueA(spec), getDescriptor(spec));
}

static CERef getConstERefB(char spec)
{
    return CERef(getValueB(spec), getDescriptor(spec));
}

static CERef getConstERefN(char spec)
{
    return CERef(getValueN(spec), getDescriptor(spec));
}

static int NULLBIT = 1;
static CERef getConstERefNull(char spec)
{
    return CERef(getValueN(spec), getDescriptor(spec), &NULLBIT, 0);
}

static bool isUnset(const CERef& ref)
    // Return 'true' if the specified element 'ref' references an element that
    // has the "unset" value for its type, and 'false' otherwise.
{
    return ref.descriptor()->isUnset(ref.data());
}

//=============================================================================
//                  FUNCTION FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

///Usage
///-----
// The 'divide' function reads two 'double' values from an input stream,
// divides the first by the second, and streams the result to an output
// stream.
//..
 void divide(bdex_ByteInStream& is, bdex_ByteOutStream& os) {
//
     // Read input parameters
     double dividend, divisor;
     bdex_InStreamFunctions::streamIn(is, dividend, 0);
     bdex_InStreamFunctions::streamIn(is, divisor, 0);
//..
// The division normally will result in a double value, but will sometimes
// result in an error string.  The latter case can occur either because the
// input stream is corrupt or because the division itself failed because of
// the divisor was zero.  The result of the division is therefore packaged in
// a 'bdem_Choice' which can store either a 'double' or a 'string'.  The
// current selection is indexed by a value of 0 through 2 for the double
// result, string for input error, or string for division error.  Note that
// input errors and division errors are represented by two separate 'STRING'
// items in the array of types, so that the choice selector will indicate
// which of the two types of error strings is currently being held by the
// choice.
//..
     static const bdem_ElemType::Type CHOICE_TYPES[] = {
         bdem_ElemType::BDEM_DOUBLE,  // result value
         bdem_ElemType::BDEM_STRING,  // input error
         bdem_ElemType::BDEM_STRING   // division error
     };
     static const int NUM_CHOICES =
         sizeof CHOICE_TYPES / sizeof CHOICE_TYPES[0];
     enum { RESULT_VAL, INPUT_ERROR, DIVISION_ERROR };
//
     bdem_Choice outMessage(CHOICE_TYPES, NUM_CHOICES);
     ASSERT(-1 == outMessage.selector());
//
     if (! is) {
//..
// If the input fails, the choice is set to 'INPUT_ERROR' and the
// corresponding selection is set to an error string.
//..
         bsl::string inputErrString = "Failed to read arguments.";
         outMessage.makeSelection(INPUT_ERROR).theModifiableString() =
                                                                inputErrString;
         ASSERT(INPUT_ERROR == outMessage.selector());
         ASSERT(inputErrString == outMessage.theString());
     }
     else if (0 == divisor) {
//..
// If the division fails, the choice is set to 'DIVISION_ERROR' and the
// corresponding selection is set to an error string.  Note the alternate
// syntax for setting '(selection().theModifiableString())' and accessing
// '(selection().theString())' the current element:
//..
         bsl::string errString = "Divided by zero.";
         outMessage.makeSelection(DIVISION_ERROR);
         outMessage.selection().theModifiableString() = errString;
         ASSERT(DIVISION_ERROR == outMessage.selector());
         ASSERT(errString == outMessage.selection().theString());
     }
     else {
//..
// If there are no errors, compute the quotient and store it as a 'double'
// with selector, 'RESULT_VAL'.
//..
         const double quotient = dividend / divisor;
         outMessage.makeSelection(RESULT_VAL).theModifiableDouble() = quotient;
         ASSERT(RESULT_VAL == outMessage.selector());
         ASSERT(quotient == outMessage.theDouble());
     }
//..
// Once the result is calculated, it is streamed out to be sent back to the
//..
     bdex_OutStreamFunctions::streamOut(os,
                                        outMessage,
                                        outMessage.maxSupportedBdexVersion());
 }
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

// Note: on Windows, the function pointers in bdem_Properties::d_intAttr
// are 0x00000000 before entering main().  Consequently, the calls in
// fAxx() and fBxx() cause a runtime exception.  Our workaround is to
// initialize these constants at the start of main.

#ifdef BSLS_PLATFORM__CMP_MSVC
    mA20 = fA20();
    mB20 = fB20();
    mA21 = fA21();
    mB21 = fB21();
    mA30 = fA30();
    mB30 = fB30();
    mA31 = fA31();
    mB31 = fB31();
#endif

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 15: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Simple example illustrating how one might use a choice.
        //
        // Concerns:
        //
        // Plan:
        //
        // Tactics:
        //   - Add-Hoc Data Selection Method
        //   - Brute-Force implementation technique
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "USAGE EXAMPLE" << bsl::endl
                               << "=============" << bsl::endl;

         bsl::string inputErrString = "Failed to read arguments.";
         bsl::string errString      = "Divided by zero.";

         static const bdem_ElemType::Type CHOICE_TYPES[] = {
             bdem_ElemType::BDEM_DOUBLE,  // result value
             bdem_ElemType::BDEM_STRING,  // input error
             bdem_ElemType::BDEM_STRING   // division error
         };
         static const int NUM_CHOICES =
         sizeof CHOICE_TYPES / sizeof CHOICE_TYPES[0];
         enum { RESULT_VAL, INPUT_ERROR, DIVISION_ERROR };

        // Try all three scenarios
        // Scenario 1
        {
            bdex_ByteInStream  in;
            bdex_ByteOutStream out;

            in.invalidate();
            divide(in, out);

            Obj mX; const Obj& X = mX;
            bdex_ByteInStream input(out.data(), out.length());
            bdex_InStreamFunctions::streamIn(input,
                                             mX,
                                             X.maxSupportedBdexVersion());
            ASSERT(NUM_CHOICES == X.numSelections());
            ASSERT(INPUT_ERROR == X.selector());
            ASSERT(inputErrString == X.theString());
        }

        // Scenario 2
        {
            double VALUE1 = 10.25, VALUE2 = 0;
            bdex_ByteOutStream output;
            bdex_OutStreamFunctions::streamOut(output, VALUE1, 0);
            bdex_OutStreamFunctions::streamOut(output, VALUE2, 0);

            bdex_ByteInStream  in(output.data(), output.length());
            bdex_ByteOutStream out;
            divide(in, out);

            Obj mX; const Obj& X = mX;
            bdex_ByteInStream input(out.data(), out.length());
            bdex_InStreamFunctions::streamIn(input,
                                             mX,
                                             X.maxSupportedBdexVersion());
            ASSERT(NUM_CHOICES == X.numSelections());
            ASSERT(DIVISION_ERROR == X.selector());
            ASSERT(errString == X.theString());
        }

        // Scenario 3
        {
            double VALUE1 = 10.5, VALUE2 = 5.25, RESULT = VALUE1 / VALUE2;
            bdex_ByteOutStream output;
            bdex_OutStreamFunctions::streamOut(output, VALUE1, 0);
            bdex_OutStreamFunctions::streamOut(output, VALUE2, 0);

            bdex_ByteInStream  in(output.data(), output.length());
            bdex_ByteOutStream out;
            divide(in, out);

            Obj mX; const Obj& X = mX;
            bdex_ByteInStream input(out.data(), out.length());
            bdex_InStreamFunctions::streamIn(input,
                                             mX,
                                             X.maxSupportedBdexVersion());
            ASSERT(NUM_CHOICES == X.numSelections());
            ASSERT(RESULT_VAL  == X.selector());
            ASSERT(RESULT      == X.theDouble());
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING BSLMA ALLOCATOR MODEL AND ALLOCATOR TRAITS
        //
        // Concerns: That the type under testing, which uses an allocator, when
        //   it is placed into a container, is correctly propagated the
        //   allocator of the container and will use the allocator (not the
        //   default allocator) for its future memory needs.
        //
        // Plan: It suffices to assert that the traits is defined.  One way is
        //   by using 'BSLALG_DECLARE_NESTED_TRAITS' and another is by sniffing
        //   that there is an implicit conversion construction from
        //   'bslma_Allocator*'.  We also want to discourage the second way, as
        //   that constructor should be made explicit.
        //
        // Testing:
        //   bdema allocator model
        //   correct declaration of bslalg_TypeTraitUsesBslmaAllocator
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << "\nTesting allocator traits"
                               << "\n========================" << bsl::endl;

        typedef bdem_Choice Obj;

        ASSERT((0 == bslmf_IsConvertible<bslma_Allocator*, Obj>::VALUE));
        ASSERT((1 ==
             bslalg_HasTrait<Obj, bslalg_TypeTraitUsesBslmaAllocator>::VALUE));

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'theModifiableXXX' MANIPULATORS
        //
        // Concerns:
        //   - That the choice gets set to the correct selection and the value
        //     is set appropriately.
        //
        // Plan:
        //   - Create a modifiable one-selection instance of each type and use
        //     the appropriate setThe{Type} method to set the value.  Assert
        //     that 'the{Type}' method returns the correct value.
        //   - Using test vectors create multi-selection instance of different
        //     types.  For each type in the test vector use the appropriate
        //     setThe{Type} method to set the value.  Assert that
        //     the{Type} method returns the correct value.
        //
        // Tactics:
        //   - Ad-Hoc test data selection method
        //   - Brute Force implementation technique
        //
        // Testing:
        //                     MANIPULATORS
        //                     ~~~~~~~~~~~~
        //   theModifiableBool() const;
        //   theModifiableChar() const;
        //   theModifiableShort() const;
        //   theModifiableInt() const;
        //   theModifiableInt64() const;
        //   theModifiableFloat() const;
        //   theModifiableDouble() const;
        //   theModifiableString() const;
        //   theModifiableDatetime() const;
        //   theModifiableDate() const;
        //   theModifiableTime() const;
        //   theModifiableCharArray() const;
        //   theModifiableShortArray() const;
        //   theModifiableIntArray() const;
        //   theModifiableInt64Array() const;
        //   theModifiableFloatArray() const;
        //   theModifiableDoubleArray() const;
        //   theModifiableDatetimeArray() const;
        //   theModifiableDateArray() const;
        //   theModifiableTimeArray() const;
        //   theModifiableList() const;
        //   theModifiableTable() const;
        //   theModifiableChoice() const;
        //   theModifiableChoiceArray() const;
        // --------------------------------------------------------------------

        #define SET_THE_TYPE(TYPE, ETYPE, NUM) {                              \
          if (verbose) bsl::cout << "\ttheModifiable" << #TYPE << bsl::endl;  \
          {                                                                   \
            bslma_TestAllocator alloc(veryVeryVerbose);                       \
                                                                              \
            Obj mX(&alloc); const Obj& X = mX;                                \
            mX.addSelection(EType::ETYPE);                                    \
                                                                              \
            mX.selection().makeNull();  ASSERT(mX.selection().isNull());      \
            mX.makeSelection(0);                                              \
            mX.theModifiable ## TYPE() = A ## NUM;                            \
            ASSERT(A ## NUM == X.the ## TYPE());                              \
            ASSERT(mX.selection().isNonNull());                               \
            mX.theModifiable ## TYPE() = B ## NUM;                            \
            ASSERT(B ## NUM == X.the ## TYPE());                              \
            mX.theModifiable ## TYPE() = N ## NUM;                            \
            ASSERT(N ## NUM == X.the ## TYPE());                              \
          }                                                                   \
        }

        if (verbose) bsl::cout << "\nTesting theModifiableXXX functions"
                               << "\n=================================="
                               << bsl::endl;

        if (verbose) bsl::cout << "\nAll kinds of one-selection choice"
                               << bsl::endl;
        {
            SET_THE_TYPE(Bool,            BDEM_BOOL,             22);
            SET_THE_TYPE(Char,            BDEM_CHAR,             00);
            SET_THE_TYPE(Short,           BDEM_SHORT,            01);
            SET_THE_TYPE(Int,             BDEM_INT,              02);
            SET_THE_TYPE(Int64,           BDEM_INT64,            03);
            SET_THE_TYPE(Float,           BDEM_FLOAT,            04);
            SET_THE_TYPE(Double,          BDEM_DOUBLE,           05);
            SET_THE_TYPE(String,          BDEM_STRING,           06);
            SET_THE_TYPE(Datetime,        BDEM_DATETIME,         07);
            SET_THE_TYPE(DatetimeTz,      BDEM_DATETIMETZ,       23);
            SET_THE_TYPE(Date,            BDEM_DATE,             08);
            SET_THE_TYPE(DateTz,          BDEM_DATETZ,           24);
            SET_THE_TYPE(Time,            BDEM_TIME,             09);
            SET_THE_TYPE(TimeTz,          BDEM_TIMETZ,           25);
            SET_THE_TYPE(BoolArray,       BDEM_BOOL_ARRAY,       26);
            SET_THE_TYPE(CharArray,       BDEM_CHAR_ARRAY,       10);
            SET_THE_TYPE(ShortArray,      BDEM_SHORT_ARRAY,      11);
            SET_THE_TYPE(IntArray,        BDEM_INT_ARRAY,        12);
            SET_THE_TYPE(Int64Array,      BDEM_INT64_ARRAY,      13);
            SET_THE_TYPE(FloatArray,      BDEM_FLOAT_ARRAY,      14);
            SET_THE_TYPE(DoubleArray,     BDEM_DOUBLE_ARRAY,     15);
            SET_THE_TYPE(StringArray,     BDEM_STRING_ARRAY,     16);
            SET_THE_TYPE(DatetimeArray,   BDEM_DATETIME_ARRAY,   17);
            SET_THE_TYPE(DatetimeTzArray, BDEM_DATETIMETZ_ARRAY, 27);
            SET_THE_TYPE(DateArray,       BDEM_DATE_ARRAY,       18);
            SET_THE_TYPE(DateTzArray,     BDEM_DATETZ_ARRAY,     28);
            SET_THE_TYPE(TimeArray,       BDEM_TIME_ARRAY,       19);
            SET_THE_TYPE(TimeTzArray,     BDEM_TIMETZ_ARRAY,     29);
            SET_THE_TYPE(List,            BDEM_LIST,             20);
            SET_THE_TYPE(Table,           BDEM_TABLE,            21);
            SET_THE_TYPE(Choice,          BDEM_CHOICE,           30);
            SET_THE_TYPE(ChoiceArray,     BDEM_CHOICE_ARRAY,     31);
        }

        if (verbose) bsl::cout << "\nMulti-selection choice" << bsl::endl;
        {
            const struct {
                int         d_line;
                const char *d_catalogSpec;  // Specification to create catalog
            } DATA[] = {
            // Line     DescriptorSpec
            // ====     ==============
                { L_,       "AA" },
                { L_,       "AC" },
                { L_,       "CD" },
                { L_,       "CE" },
                { L_,       "FG" },
                { L_,       "GG" },
                { L_,       "GH" },
                { L_,       "MN" },
                { L_,       "OP" },
                { L_,       "PQ" },
                { L_,       "KQ" },
                { L_,       "ST" },
                { L_,       "WX" },
                { L_,       "YZ" },
                { L_,       "ab" },
                { L_,       "cd" },
                { L_,       "ef" },

                { L_,       "HIJ" },
                { L_,       "KLM" },
                { L_,       "RST" },
                { L_,       "YZa" },

                { L_,       "BLRW" },
                { L_,       "DGNQ" },
                { L_,       "QRST" },

                { L_,       "JHKHSK" },
                { L_,       "RISXLSW" },
                { L_,       "MXKZOLPR" },
                { L_,       "GGGGGGGGG" },
                { L_,       "QQQQQQQQQQ" },
                { L_,       "abcdABCDEFG" },
                { L_,       "FDLALAabADSF" },

                { L_,       "GSRWSASDEFDSA" },
                { L_,       "PKJHALKGabASDA" },
                { L_,       "XZSAEWRPOIJLKaAS" },
                { L_,       "GALKacKASJDKSWEIO" },

                { L_,       "ABCDEFGHIJKLMNOPQRSTWXYZabcd" },
            };
            static const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE  = DATA[i].d_line;
                const char *SPEC  = DATA[i].d_catalogSpec;
                const int   LEN   = bsl::strlen(SPEC);

                const TypesCatalog CATALOG = ggCatalog(SPEC);
                bslma_TestAllocator alloc(veryVeryVerbose);

                Obj mX(CATALOG, &alloc); const Obj& X = mX;
                for (int j = 0; j < LEN; ++j) {
                  const char  S     = SPEC[j];
                  const CERef VAL_A = getConstERefA(S);
                  const CERef VAL_B = getConstERefB(S);
                  const CERef VAL_N = getConstERefN(S);

                  mX.selection().makeNull();
                  LOOP_ASSERT(LINE, mX.selection().isNull());

                  set(&mX, VAL_A, j, S);
                  ASSERT(mX.selection().isNonNull());
                  LOOP2_ASSERT(LINE, S, compare(X, VAL_A, S));

                  set(&mX, VAL_B, j, S);
                  LOOP2_ASSERT(LINE, S, compare(X, VAL_B, S));

                  set(&mX, VAL_N, j, S);
                  LOOP2_ASSERT(LINE, S, compare(X, VAL_N, S));
                }
            }
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING 'theXXX' MANIPULATORS/ACCESSORS
        //
        // Concerns:
        //   - That a reference of appropriate type is returned.
        //   - That a reference of appropriate 'const'-ness is returned.
        //
        // Plan:
        //   - Create at least two modifiable instances of each type.
        //   - Create const references to these respective instances.
        //   - Verify the type, 'const'-ness, and value of each value returned.
        //
        // Tactics:
        //   - Ad-Hoc test data selection method
        //   - Brute Force implementation technique
        //
        // Testing:
        //                     ACCESSORS
        //                     ~~~~~~~~~
        //   const bool& theBool() const;
        //   const char& theChar() const;
        //   const short& theShort() const;
        //   const int& theInt() const;
        //   const bsls_Types::Int64& theInt64() const;
        //   const float& theFloat() const;
        //   const double& theDouble() const;
        //   const bsl::string& theString() const;
        //   const bdet_Datetime& theDatetime() const;
        //   const bdet_DatetimeTz& theDatetimeTz() const;
        //   const bdet_Date& theDate() const;
        //   const bdet_DateTz& theDateTz() const;
        //   const bdet_Time& theTime() const;
        //   const bdet_TimeTz& theTimeTz() const;
        //   const vector<bool>& theBoolArray() const;
        //   const vector<char>& theCharArray() const;
        //   const vector<short>& theShortArray() const;
        //   const vector<int>& theIntArray() const;
        //   const vector<bsls_Types::Int64>& theInt64Array() const;
        //   const vector<float>& theFloatArray() const;
        //   const vector<double>& theDoubleArray() const;
        //   const vector<bsl::string>& theStringArray() const;
        //   const vector<bdet_Datetime>& theDatetimeArray() const;
        //   const vector<bdet_DatetimeTz>& theDatetimeTzArray() const;
        //   const vector<bdet_Date>& theDateArray() const;
        //   const vector<bdet_DateTz>& theDateTzArray() const;
        //   const vector<bdet_Time>& theTimeArray() const;
        //   const vector<bdet_TimeTz>& theTimeTzArray() const;
        //   const bdem_List& theList() const;
        //   const bdem_Table& theTable() const;
        //   const bdem_Choice& theChoice() const;
        //   const bdem_ChoiceArray& theChoiceArray() const;
        //
        //                     MANIPULATORS
        //                     ~~~~~~~~~~~~
        //   bool& theBool() const;
        //   char& theChar() const;
        //   short& theShort() const;
        //   int& theInt() const;
        //   bsls_Types::Int64& theInt64() const;
        //   float& theFloat() const;
        //   double& theDouble() const;
        //   bsl::string& theString() const;
        //   bdet_Datetime& theDatetime() const;
        //   bdet_Date& theDate() const;
        //   bdet_Time& theTime() const;
        //   vector<char>& theCharArray() const;
        //   vector<short>& theShortArray() const;
        //   vector<int>& theIntArray() const;
        //   vector<bsls_Types::Int64>& theInt64Array() const;
        //   vector<float>& theFloatArray() const;
        //   vector<double>& theDoubleArray() const;
        //   vector<bsl::string>& theStringArray() const;
        //   vector<bdet_Datetime>& theDatetimeArray() const;
        //   vector<bdet_Date>& theDateArray() const;
        //   vector<bdet_Time>& theTimeArray() const;
        //   bdem_List& theList() const;
        //   bdem_Table& theTable() const;
        //   bdem_Choice& theChoice() const;
        //   bdem_ChoiceArray& theChoiceArray() const;
        // --------------------------------------------------------------------

        #define THE_TYPE(TYPE, ETYPE, NUM) {                                  \
          if (verbose) bsl::cout << "\tthe" << #TYPE << bsl::endl;            \
          {                                                                   \
            bslma_TestAllocator alloc(veryVeryVerbose);                       \
                                                                              \
            Obj mX(&alloc); const Obj& X = mX;                                \
            mX.addSelection(EType::ETYPE);                                    \
            mX.makeSelection(0);                                              \
                                                                              \
            mX.theModifiable ## TYPE() = A ## NUM;                            \
            ASSERT(A ## NUM == X.the ## TYPE());                              \
            ASSERT(mX.selection().isNonNull());                               \
            mX.theModifiable ## TYPE() = B ## NUM;                            \
            ASSERT(B ## NUM == X.the ## TYPE());                              \
            mX.theModifiable ## TYPE() = N ## NUM;                            \
            ASSERT(N ## NUM == X.the ## TYPE());                              \
          }                                                                   \
        }

        if (verbose) bsl::cout << "\nTesting theXXX functions"
                               << "\n========================"
                               << bsl::endl;

        if (verbose) bsl::cout << "\nAll kinds of one-selection choice"
                               << bsl::endl;
        {
            THE_TYPE(Bool,            BDEM_BOOL,             22);
            THE_TYPE(Char,            BDEM_CHAR,             00);
            THE_TYPE(Short,           BDEM_SHORT,            01);
            THE_TYPE(Int,             BDEM_INT,              02);
            THE_TYPE(Int64,           BDEM_INT64,            03);
            THE_TYPE(Float,           BDEM_FLOAT,            04);
            THE_TYPE(Double,          BDEM_DOUBLE,           05);
            THE_TYPE(String,          BDEM_STRING,           06);
            THE_TYPE(Datetime,        BDEM_DATETIME,         07);
            THE_TYPE(DatetimeTz,      BDEM_DATETIMETZ,       23);
            THE_TYPE(Date,            BDEM_DATE,             08);
            THE_TYPE(DateTz,          BDEM_DATETZ,           24);
            THE_TYPE(Time,            BDEM_TIME,             09);
            THE_TYPE(TimeTz,          BDEM_TIMETZ,           25);
            THE_TYPE(BoolArray,       BDEM_BOOL_ARRAY,       26);
            THE_TYPE(CharArray,       BDEM_CHAR_ARRAY,       10);
            THE_TYPE(ShortArray,      BDEM_SHORT_ARRAY,      11);
            THE_TYPE(IntArray,        BDEM_INT_ARRAY,        12);
            THE_TYPE(Int64Array,      BDEM_INT64_ARRAY,      13);
            THE_TYPE(FloatArray,      BDEM_FLOAT_ARRAY,      14);
            THE_TYPE(DoubleArray,     BDEM_DOUBLE_ARRAY,     15);
            THE_TYPE(StringArray,     BDEM_STRING_ARRAY,     16);
            THE_TYPE(DatetimeArray,   BDEM_DATETIME_ARRAY,   17);
            THE_TYPE(DatetimeTzArray, BDEM_DATETIMETZ_ARRAY, 27);
            THE_TYPE(DateArray,       BDEM_DATE_ARRAY,       18);
            THE_TYPE(DateTzArray,     BDEM_DATETZ_ARRAY,     28);
            THE_TYPE(TimeArray,       BDEM_TIME_ARRAY,       19);
            THE_TYPE(TimeTzArray,     BDEM_TIMETZ_ARRAY,     29);
            THE_TYPE(List,            BDEM_LIST,             20);
            THE_TYPE(Table,           BDEM_TABLE,            21);
            THE_TYPE(Choice,          BDEM_CHOICE,           30);
            THE_TYPE(ChoiceArray,     BDEM_CHOICE_ARRAY,     31);
        }

        if (verbose) bsl::cout << "\nMulti-selection choice" << bsl::endl;
        {
            const struct {
                int         d_line;
                const char *d_catalogSpec;  // Specification to create catalog
            } DATA[] = {
            // Line     DescriptorSpec
            // ====     ==============
                { L_,       "AA" },
                { L_,       "AC" },
                { L_,       "CD" },
                { L_,       "CE" },
                { L_,       "FG" },
                { L_,       "GG" },
                { L_,       "GH" },
                { L_,       "MN" },
                { L_,       "OP" },
                { L_,       "PQ" },
                { L_,       "KQ" },
                { L_,       "ST" },
                { L_,       "WX" },
                { L_,       "YZ" },
                { L_,       "ab" },
                { L_,       "cd" },
                { L_,       "ef" },

                { L_,       "HIJ" },
                { L_,       "KLM" },
                { L_,       "RST" },
                { L_,       "YZa" },

                { L_,       "BLRW" },
                { L_,       "DGNQ" },
                { L_,       "QRST" },

                { L_,       "JHKHSK" },
                { L_,       "RISXLSW" },
                { L_,       "MXKZOLPR" },
                { L_,       "GGGGGGGGG" },
                { L_,       "QQQQQQQQQQ" },
                { L_,       "abcdABCDEFG" },
                { L_,       "FDLALAabADSF" },

                { L_,       "GSRWSASDEFDSA" },
                { L_,       "PKJHALKGabASDA" },
                { L_,       "XZSAEWRPOIJLKaAS" },
                { L_,       "GALKacKASJDKSWEIO" },

                { L_,       "ABCDEFGHIJKLMNOPQRSTWXYZabcd" },
            };
            static const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE  = DATA[i].d_line;
                const char *SPEC  = DATA[i].d_catalogSpec;
                const int   LEN   = bsl::strlen(SPEC);

                const TypesCatalog CATALOG = ggCatalog(SPEC);
                bslma_TestAllocator alloc(veryVeryVerbose);

                Obj mX(CATALOG, &alloc); const Obj& X = mX;
                for (int j = 0; j < LEN; ++j) {
                    const char  S     = SPEC[j];
                    const CERef VAL_A = getConstERefA(S);
                    const CERef VAL_B = getConstERefB(S);
                    const CERef VAL_N = getConstERefN(S);

                    mX.makeSelection(j);
                    assign(&mX, VAL_A, S);
                    LOOP2_ASSERT(LINE, S, compare(X, VAL_A, S));

                    assign(&mX, VAL_B, S);
                    LOOP2_ASSERT(LINE, S, compare(X, VAL_B, S));

                    assign(&mX, VAL_N, S);
                    LOOP2_ASSERT(LINE, S, compare(X, VAL_N, S));
                }
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'reset' FUNCTION:
        //
        // Concerns:
        //   A object that has been reset has state identical to its state
        //   immediately after default construction.
        //
        // Plan:
        //   Specify a set S of (unique) objects with substantial and varied
        //   differences in value.  Construct and initialize all objects, mX,
        //   in the set S.  Default-construct an object, mY.  Call mX.reset().
        //   Assert that mX == mY.
        //
        // Testing:
        //   void reset();
        //   void reset(const bdem_ElemType::Type typesCatalog[],
        //              int                       typesCatalogLen);
        //   void reset(const bsl::vector<EType::Type>& typesCatalog);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting reset Function"
                               << "\n======================" << bsl::endl;
        const struct {
            int         d_line;
            const char *d_catalogSpec;  // Specification to create the catalog
        } DATA[] = {
            // Line     DescriptorSpec
            // ====     ==============
            { L_,       "" },
            { L_,       "A" },
            { L_,       "B" },
            { L_,       "C" },
            { L_,       "D" },
            { L_,       "E" },
            { L_,       "F" },
            { L_,       "G" },
            { L_,       "H" },
            { L_,       "I" },
            { L_,       "J" },
            { L_,       "K" },
            { L_,       "L" },
            { L_,       "M" },
            { L_,       "N" },
            { L_,       "O" },
            { L_,       "P" },
            { L_,       "Q" },
            { L_,       "R" },
            { L_,       "S" },
            { L_,       "T" },
            { L_,       "U" },
            { L_,       "V" },
            { L_,       "W" },
            { L_,       "X" },
            { L_,       "Y" },
            { L_,       "Z" },
            { L_,       "a" },
            { L_,       "b" },
            { L_,       "c" },
            { L_,       "d" },
            { L_,       "e" },
            { L_,       "f" },

            // Testing various interesting combinations
            { L_,       "CD" },
            { L_,       "FG" },
            { L_,       "GG" },
            { L_,       "GH" },
            { L_,       "MN" },
            { L_,       "PQ" },
            { L_,       "KQ" },
            { L_,       "UV" },
            { L_,       "ef" },

            { L_,       "YZa" },
            { L_,       "BLRW" },
            { L_,       "JHKHSK" },
            { L_,       "RISXLSW" },
            { L_,       "MXKZOLPR" },
            { L_,       "GGGGGGGGG" },
            { L_,       "QQQQQQQQQQ" },
            { L_,       "abcdABCDEFG" },
            { L_,       "FDLALAabADSF" },
            { L_,       "ABCDEFGHIJKLMNOPQRSTWXYZabcd" },
        };
        static const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

        const Obj EMPTY;

        for (int i1 = 0; i1 < NUM_DATA; ++i1) {
          const int   LINE1  = DATA[i1].d_line;
          const char *SPEC1  = DATA[i1].d_catalogSpec;
          const int   LEN1   = bsl::strlen(SPEC1);

          if (veryVerbose) { P(SPEC1); }

          for (int j1 = 0; j1 < LEN1; ++j1) {
            const char   S1  = SPEC1[j1];
            const CERef C1_A = getConstERefA(S1);
            const TypesCatalog CATALOG1 = ggCatalog(SPEC1);

            bslma_TestAllocator alloc1(veryVeryVerbose);
            Obj mX(CATALOG1, &alloc1); const Obj& X = mX;
            Obj mY(CATALOG1, &alloc1); const Obj& Y = mY;
            Obj mZ(CATALOG1, &alloc1); const Obj& Z = mZ;

            mX.makeSelection(j1).replaceValue(C1_A);
            mY.makeSelection(j1).replaceValue(C1_A);
            mZ.makeSelection(j1).replaceValue(C1_A);

            mX.reset();
            LOOP_ASSERT(LINE1, 0  == X.numSelections());
            LOOP_ASSERT(LINE1, -1 == X.selector());
            LOOP_ASSERT(LINE1, EType::BDEM_VOID == X.selectionType());
            LOOP_ASSERT(LINE1, EType::BDEM_VOID == X.selectionType(-1));
            LOOP_ASSERT(LINE1, X == EMPTY);

            for (int i2 = 0; i2 < NUM_DATA; ++i2) {
              const int   LINE2  = DATA[i2].d_line;
              const char *SPEC2  = DATA[i2].d_catalogSpec;

              if (veryVerbose) { P(SPEC2); }

              const TypesCatalog CATALOG2 = ggCatalog(SPEC2);
              const Obj CONTROL(CATALOG2);

              const EType::Type *BEGIN = 0 == CATALOG2.size()
                                       ? 0
                                       : &CATALOG2[0];
              mY.reset(BEGIN, CATALOG2.size());
              mZ.reset(CATALOG2);

              LOOP2_ASSERT(LINE1, LINE2,
                           CATALOG2.size() == (unsigned) Y.numSelections());
              LOOP2_ASSERT(LINE1, LINE2, -1 == Y.selector());
              LOOP2_ASSERT(LINE1, LINE2,
                           EType::BDEM_VOID == Y.selectionType());
              LOOP2_ASSERT(LINE1, LINE2,
                           EType::BDEM_VOID == Y.selectionType(-1));
              LOOP2_ASSERT(LINE1, LINE2, Y == CONTROL);

              LOOP2_ASSERT(LINE1, LINE2,
                           CATALOG2.size() == (unsigned) Z.numSelections());
              LOOP2_ASSERT(LINE1, LINE2, -1 == Z.selector());
              LOOP2_ASSERT(LINE1, LINE2,
                           EType::BDEM_VOID == Z.selectionType());
              LOOP2_ASSERT(LINE1, LINE2,
                           EType::BDEM_VOID == Z.selectionType(-1));
              LOOP2_ASSERT(LINE1, LINE2, Y == CONTROL);
            }
          }
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING CREATORS
        //
        // Concerns:
        //   The constructor can be used to create an object containing any
        //   unset value.
        //
        // Plan:
        //   Specify a set S whose elements have substantial and varied
        //   differences in value.  For each element in S, construct and
        //   initialize identically valued objects mX and mY, where mX is used
        //   for comparison and mY serves as the control object used to
        //   verify equality with original value.  Then construct object
        //   similar to mX using the various constructors and the addSelection
        //   function.  Use different allocators and allocation strategies.
        //   Use the equality operator to confirm that all objects have the
        //   same value.  Modify mX and confirm that none of the other objects
        //   are modified and are equal to mY.  Modify each object and
        //   confirm that mX remains the same and equal to mY.  Finally, reset
        //   mX and confirm that none of the other objs are changed.  Using the
        //   allocators specified to construct the objects ensure that each
        //   object uses the correct allocator for memory allocations.
        //
        // Testing:
        //   bdem_Choice(bslma_Allocator               *alloc = 0);
        //   bdem_Choice(AggOption::AllocationStrategy  allocMode,
        //               bslma_Allocator               *alloc = 0);
        //   bdem_Choice(const bdem_ElemType::Type      typesCatalog[],
        //               int                            typesCatalogLen,
        //               bslma_Allocator               *alloc = 0);
        //   bdem_Choice(const bdem_ElemType::Type      typesCatalog[],
        //               int                            typesCatalogLen,
        //               AggOption::AllocationStrategy  allocMode,
        //               bslma_Allocator               *alloc = 0);
        //   bdem_Choice(const TypesCatalog&            typesCatalog,
        //               bslma_Allocator               *alloc = 0);
        //   bdem_Choice(const TypesCatalog&            typesCatalog,
        //               AggOption::AllocationStrategy  allocMode,
        //               bslma_Allocator               *alloc = 0);
        //   bdem_Choice(const bdem_ChoiceArrayItem&    original,
        //               bslma_Allocator               *alloc = 0);
        //   bdem_Choice(const bdem_ChoiceArrayItem&    original,
        //               AggOption::AllocationStrategy  allocMode,
        //               bslma_Allocator               *alloc = 0);
        //   ~bdem_Choice();
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Creators"
                               << "\n================" << bsl::endl;

        const struct {
            int         d_line;
            const char *d_catalogSpec;  // Specification to create the catalog
        } DATA[] = {
            // Line     DescriptorSpec
            // ====     ==============
            { L_,       "" },
            { L_,       "A" },
            { L_,       "B" },
            { L_,       "C" },
            { L_,       "D" },
            { L_,       "E" },
            { L_,       "F" },
            { L_,       "G" },
            { L_,       "H" },
            { L_,       "I" },
            { L_,       "J" },
            { L_,       "K" },
            { L_,       "L" },
            { L_,       "M" },
            { L_,       "N" },
            { L_,       "O" },
            { L_,       "P" },
            { L_,       "Q" },
            { L_,       "R" },
            { L_,       "S" },
            { L_,       "T" },
            { L_,       "U" },
            { L_,       "V" },
            { L_,       "W" },
            { L_,       "X" },
            { L_,       "Y" },
            { L_,       "Z" },
            { L_,       "a" },
            { L_,       "b" },
            { L_,       "c" },
            { L_,       "d" },
            { L_,       "e" },
            { L_,       "f" },

            // Testing various interesting combinations
            { L_,       "AA" },
            { L_,       "AC" },
            { L_,       "CD" },
            { L_,       "CE" },
            { L_,       "FG" },
            { L_,       "GG" },
            { L_,       "GH" },
            { L_,       "MN" },
            { L_,       "OP" },
            { L_,       "PQ" },
            { L_,       "KQ" },
            { L_,       "ST" },
            { L_,       "WX" },
            { L_,       "YZ" },
            { L_,       "ab" },
            { L_,       "cd" },
            { L_,       "ef" },

            { L_,       "HIJ" },
            { L_,       "KLM" },
            { L_,       "RST" },
            { L_,       "YZa" },

            { L_,       "BLRW" },
            { L_,       "DGNQ" },
            { L_,       "QRST" },

            { L_,       "JHKHSK" },
            { L_,       "RISXLSW" },
            { L_,       "MXKZOLPR" },
            { L_,       "GGGGGGGGG" },
            { L_,       "QQQQQQQQQQ" },
            { L_,       "abcdABCDEFG" },
            { L_,       "FDLALAabADSF" },

            { L_,       "GSRWSASDEFDSA" },
            { L_,       "PKJHALKGabASDA" },
            { L_,       "XZSAEWRPOIJLKaAS" },
            { L_,       "GALKacKASJDKSWEIO" },

            { L_,       "ABCDEFGHIJKLMNOPQRSTWXYZabcd" },
        };
        static const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

        if (veryVerbose) { bsl::cout << "Construct objects with default "
                                     << "allocation strategy" << bsl::endl; }
        {
          for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE  = DATA[i].d_line;
            const char *SPEC  = DATA[i].d_catalogSpec;
            const int   LEN   = bsl::strlen(SPEC);

            const TypesCatalog CATALOG = ggCatalog(SPEC);
            bslma_TestAllocator testAllocator(veryVeryVerbose);
            bslma_DefaultAllocatorGuard guard(&testAllocator);

            if (veryVerbose) { P(SPEC); }

          BEGIN_BSLMA_EXCEPTION_TEST {
            Obj mA; const Obj& A = mA;
            LOOP_ASSERT(LINE, 0  == A.numSelections());
            LOOP_ASSERT(LINE, -1 == A.selector());
            LOOP_ASSERT(LINE, EType::BDEM_VOID == A.selectionType());
            LOOP_ASSERT(LINE, EType::BDEM_VOID == A.selectionType(-1));

            Obj mB(&testAllocator); const Obj& B = mB;
            LOOP_ASSERT(LINE, 0  == B.numSelections());
            LOOP_ASSERT(LINE, -1 == B.selector());
            LOOP_ASSERT(LINE, EType::BDEM_VOID == B.selectionType());
            LOOP_ASSERT(LINE, EType::BDEM_VOID == B.selectionType(-1));

            for (int j = 0; j < LEN; ++j) {
              mA.addSelection(CATALOG[j]);
              mB.addSelection(CATALOG[j]);
            }

            LOOP_ASSERT(LINE, CATALOG.size() == (unsigned) A.numSelections());
            LOOP_ASSERT(LINE, -1 == A.selector());
            LOOP_ASSERT(LINE, EType::BDEM_VOID == A.selectionType());
            LOOP_ASSERT(LINE, EType::BDEM_VOID == A.selectionType(-1));

            LOOP_ASSERT(LINE, CATALOG.size() == (unsigned) B.numSelections());
            LOOP_ASSERT(LINE, -1 == B.selector());
            LOOP_ASSERT(LINE, EType::BDEM_VOID == B.selectionType());
            LOOP_ASSERT(LINE, EType::BDEM_VOID == B.selectionType(-1));

            const EType::Type *BEGIN = 0 == CATALOG.size()
                                       ? 0
                                       : &CATALOG[0];

            Obj mC(BEGIN, CATALOG.size()); const Obj& C = mC;
            ASSERT(CATALOG.size() == (unsigned) C.numSelections());
            ASSERT(-1 == C.selector());
            ASSERT(EType::BDEM_VOID == C.selectionType());
            ASSERT(EType::BDEM_VOID == C.selectionType(-1));

            Obj mD(BEGIN, CATALOG.size(), &testAllocator);
            const Obj& D = mD;
            ASSERT(CATALOG.size() == (unsigned) D.numSelections());
            ASSERT(-1 == D.selector());
            ASSERT(EType::BDEM_VOID == D.selectionType());
            ASSERT(EType::BDEM_VOID == D.selectionType(-1));

            Obj mE(CATALOG);
            const Obj& E = mE;
            ASSERT(CATALOG.size() == (unsigned) E.numSelections());
            ASSERT(-1 == E.selector());
            ASSERT(EType::BDEM_VOID == E.selectionType());
            ASSERT(EType::BDEM_VOID == E.selectionType(-1));

            Obj mF(CATALOG, &testAllocator);
            const Obj& F = mF;
            ASSERT(CATALOG.size() == (unsigned) F.numSelections());
            ASSERT(-1 == F.selector());
            ASSERT(EType::BDEM_VOID == F.selectionType());
            ASSERT(EType::BDEM_VOID == F.selectionType(-1));

            LOOP_ASSERT(LINE, A == B);
            LOOP_ASSERT(LINE, B == C);
            LOOP_ASSERT(LINE, C == D);
            LOOP_ASSERT(LINE, D == E);
            LOOP_ASSERT(LINE, E == F);

            testAllocator.setAllocationLimit(-1);
            // Perturbation
            for (int j = 0; j < LEN; ++j) {
              const char  S     = SPEC[j];
              EType::Type TYPE  = getElemType(S);
              CERef       VAL_A = getConstERefA(S);
              CERef       VAL_B = getConstERefB(S);
              CERef       VAL_VOID = CERef(&Prop::d_voidAttr, 0);

              mA.makeSelection(j).replaceValue(VAL_A);
              mB.makeSelection(j).replaceValue(VAL_A);
              mC.makeSelection(j).replaceValue(VAL_A);
              mD.makeSelection(j).replaceValue(VAL_A);
              mE.makeSelection(j).replaceValue(VAL_A);
              mF.makeSelection(j).replaceValue(VAL_A);

              LOOP_ASSERT(LINE, j      == A.selector());
              LOOP_ASSERT(LINE, TYPE   == A.selectionType());
              LOOP_ASSERT(LINE, VAL_A  == A.selection());

              LOOP_ASSERT(LINE, j      == B.selector());
              LOOP_ASSERT(LINE, TYPE   == B.selectionType());
              LOOP_ASSERT(LINE, VAL_A  == B.selection());

              LOOP_ASSERT(LINE, j      == C.selector());
              LOOP_ASSERT(LINE, TYPE   == C.selectionType());
              LOOP_ASSERT(LINE, VAL_A  == C.selection());

              LOOP_ASSERT(LINE, j      == D.selector());
              LOOP_ASSERT(LINE, TYPE   == D.selectionType());
              LOOP_ASSERT(LINE, VAL_A  == D.selection());

              LOOP_ASSERT(LINE, j      == E.selector());
              LOOP_ASSERT(LINE, TYPE   == E.selectionType());
              LOOP_ASSERT(LINE, VAL_A  == E.selection());

              LOOP_ASSERT(LINE, j      == F.selector());
              LOOP_ASSERT(LINE, TYPE   == F.selectionType());
              LOOP_ASSERT(LINE, VAL_A  == F.selection());

              LOOP_ASSERT(LINE, A == B);
              LOOP_ASSERT(LINE, B == C);
              LOOP_ASSERT(LINE, C == D);
              LOOP_ASSERT(LINE, D == E);
              LOOP_ASSERT(LINE, E == F);

              mA.makeSelection(j).replaceValue(VAL_B);
              mB.makeSelection(j).replaceValue(VAL_B);
              mC.makeSelection(j).replaceValue(VAL_B);
              mD.makeSelection(j).replaceValue(VAL_B);
              mE.makeSelection(j).replaceValue(VAL_B);
              mF.makeSelection(j).replaceValue(VAL_B);

              LOOP_ASSERT(LINE, j      == A.selector());
              LOOP_ASSERT(LINE, TYPE   == A.selectionType());
              LOOP_ASSERT(LINE, VAL_B  == A.selection());

              LOOP_ASSERT(LINE, j      == B.selector());
              LOOP_ASSERT(LINE, TYPE   == B.selectionType());
              LOOP_ASSERT(LINE, VAL_B  == B.selection());

              LOOP_ASSERT(LINE, j      == C.selector());
              LOOP_ASSERT(LINE, TYPE   == C.selectionType());
              LOOP_ASSERT(LINE, VAL_B  == C.selection());

              LOOP_ASSERT(LINE, j      == D.selector());
              LOOP_ASSERT(LINE, TYPE   == D.selectionType());
              LOOP_ASSERT(LINE, VAL_B  == D.selection());

              LOOP_ASSERT(LINE, j      == E.selector());
              LOOP_ASSERT(LINE, TYPE   == E.selectionType());
              LOOP_ASSERT(LINE, VAL_B  == E.selection());

              LOOP_ASSERT(LINE, j      == F.selector());
              LOOP_ASSERT(LINE, TYPE   == F.selectionType());
              LOOP_ASSERT(LINE, VAL_B  == F.selection());

              LOOP_ASSERT(LINE, A == B);
              LOOP_ASSERT(LINE, B == C);
              LOOP_ASSERT(LINE, C == D);
              LOOP_ASSERT(LINE, D == E);
              LOOP_ASSERT(LINE, E == F);

              mA.makeSelection(-1);
              mB.makeSelection(-1);
              mC.makeSelection(-1);
              mD.makeSelection(-1);
              mE.makeSelection(-1);
              mF.makeSelection(-1);

              LOOP_ASSERT(LINE, -1          == A.selector());
              LOOP_ASSERT(LINE, EType::BDEM_VOID == A.selectionType());
              LOOP_ASSERT(LINE, EType::BDEM_VOID == A.selection().type());

              LOOP_ASSERT(LINE, -1          == B.selector());
              LOOP_ASSERT(LINE, EType::BDEM_VOID == B.selectionType());
              LOOP_ASSERT(LINE, EType::BDEM_VOID == B.selection().type());

              LOOP_ASSERT(LINE, -1          == C.selector());
              LOOP_ASSERT(LINE, EType::BDEM_VOID == C.selectionType());
              LOOP_ASSERT(LINE, EType::BDEM_VOID == C.selection().type());

              LOOP_ASSERT(LINE, -1          == D.selector());
              LOOP_ASSERT(LINE, EType::BDEM_VOID == D.selectionType());
              LOOP_ASSERT(LINE, EType::BDEM_VOID == D.selection().type());

              LOOP_ASSERT(LINE, -1          == E.selector());
              LOOP_ASSERT(LINE, EType::BDEM_VOID == E.selectionType());
              LOOP_ASSERT(LINE, EType::BDEM_VOID == E.selection().type());

              LOOP_ASSERT(LINE, -1          == F.selector());
              LOOP_ASSERT(LINE, EType::BDEM_VOID == F.selectionType());
              LOOP_ASSERT(LINE, EType::BDEM_VOID == F.selection().type());

              LOOP_ASSERT(LINE, A == B);
              LOOP_ASSERT(LINE, B == C);
              LOOP_ASSERT(LINE, C == D);
              LOOP_ASSERT(LINE, D == E);
              LOOP_ASSERT(LINE, E == F);
            }
          } END_BSLMA_EXCEPTION_TEST
          }
        }

        if (veryVerbose) { bsl::cout << "Construct objects with custom "
                                     << "allocation strategy" << bsl::endl; }
        {
          for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE  = DATA[i].d_line;
            const char *SPEC  = DATA[i].d_catalogSpec;
            const int   LEN   = bsl::strlen(SPEC);

            const TypesCatalog CATALOG = ggCatalog(SPEC);

            if (veryVerbose) { P(SPEC); }

            for (int i = 0; i < 4; ++i) {
              AggOption::AllocationStrategy mode =
                  (AggOption::AllocationStrategy) i;

              bslma_TestAllocator testAllocator(veryVeryVerbose);

            BEGIN_BSLMA_EXCEPTION_TEST {

              bdema_SequentialAllocator  seqAlloc(&testAllocator);
              bslma_Allocator           *alloc = &testAllocator;

              if (veryVerbose) { P(mode); }

              if (AggOption::BDEM_SUBORDINATE == mode) {
                  alloc = &seqAlloc;
              }

              bslma_DefaultAllocatorGuard guard(alloc);

              Obj mA(mode); const Obj& A = mA;
              LOOP_ASSERT(LINE, 0  == A.numSelections());
              LOOP_ASSERT(LINE, -1 == A.selector());
              LOOP_ASSERT(LINE, EType::BDEM_VOID == A.selectionType());
              LOOP_ASSERT(LINE, EType::BDEM_VOID == A.selectionType(-1));

              Obj mB(mode, alloc); const Obj& B = mB;
              LOOP_ASSERT(LINE, 0  == B.numSelections());
              LOOP_ASSERT(LINE, -1 == B.selector());
              LOOP_ASSERT(LINE, EType::BDEM_VOID == B.selectionType());
              LOOP_ASSERT(LINE, EType::BDEM_VOID == B.selectionType(-1));

              for (int j = 0; j < LEN; ++j) {
                mA.addSelection(CATALOG[j]);
                mB.addSelection(CATALOG[j]);
              }

              LOOP_ASSERT(LINE, CATALOG.size() == (unsigned)A.numSelections());
              LOOP_ASSERT(LINE, -1 == A.selector());
              LOOP_ASSERT(LINE, EType::BDEM_VOID == A.selectionType());
              LOOP_ASSERT(LINE, EType::BDEM_VOID == A.selectionType(-1));

              LOOP_ASSERT(LINE, CATALOG.size() == (unsigned)B.numSelections());
              LOOP_ASSERT(LINE, -1 == B.selector());
              LOOP_ASSERT(LINE, EType::BDEM_VOID == B.selectionType());
              LOOP_ASSERT(LINE, EType::BDEM_VOID == B.selectionType(-1));

              const EType::Type *BEGIN = 0 == CATALOG.size()
                  ? 0
                  : &CATALOG[0];

              Obj mC(BEGIN, CATALOG.size(), mode); const Obj& C = mC;
              ASSERT(CATALOG.size() == (unsigned) C.numSelections());
              ASSERT(-1 == C.selector());
              ASSERT(EType::BDEM_VOID == C.selectionType());
              ASSERT(EType::BDEM_VOID == C.selectionType(-1));

              Obj mD(BEGIN, CATALOG.size(), mode, alloc);
              const Obj& D = mD;
              ASSERT(CATALOG.size() == (unsigned)D.numSelections());
              ASSERT(-1 == D.selector());
              ASSERT(EType::BDEM_VOID == D.selectionType());
              ASSERT(EType::BDEM_VOID == D.selectionType(-1));

              Obj mE(CATALOG, mode);
              const Obj& E = mE;
              ASSERT(CATALOG.size() == (unsigned) E.numSelections());
              ASSERT(-1 == E.selector());
              ASSERT(EType::BDEM_VOID == E.selectionType());
              ASSERT(EType::BDEM_VOID == E.selectionType(-1));

              Obj mF(CATALOG, mode, alloc);
              const Obj& F = mF;
              ASSERT(CATALOG.size() == (unsigned) F.numSelections());
              ASSERT(-1 == F.selector());
              ASSERT(EType::BDEM_VOID == F.selectionType());
              ASSERT(EType::BDEM_VOID == F.selectionType(-1));

              LOOP_ASSERT(LINE, A == B);
              LOOP_ASSERT(LINE, B == C);
              LOOP_ASSERT(LINE, C == D);
              LOOP_ASSERT(LINE, D == E);
              LOOP_ASSERT(LINE, E == F);

              testAllocator.setAllocationLimit(-1);
              // Perturbation
              for (int j = 0; j < LEN; ++j) {
                const char  S     = SPEC[j];
                EType::Type TYPE  = getElemType(S);
                CERef       VAL_A = getConstERefA(S);
                CERef       VAL_B = getConstERefB(S);
                CERef       VAL_VOID = CERef(&Prop::d_voidAttr, 0);

                mA.makeSelection(j).replaceValue(VAL_A);
                mB.makeSelection(j).replaceValue(VAL_A);
                mC.makeSelection(j).replaceValue(VAL_A);
                mD.makeSelection(j).replaceValue(VAL_A);
                mE.makeSelection(j).replaceValue(VAL_A);
                mF.makeSelection(j).replaceValue(VAL_A);

                LOOP_ASSERT(LINE, j      == A.selector());
                LOOP_ASSERT(LINE, TYPE   == A.selectionType());
                LOOP_ASSERT(LINE, VAL_A  == A.selection());

                LOOP_ASSERT(LINE, j      == B.selector());
                LOOP_ASSERT(LINE, TYPE   == B.selectionType());
                LOOP_ASSERT(LINE, VAL_A  == B.selection());

                LOOP_ASSERT(LINE, j      == C.selector());
                LOOP_ASSERT(LINE, TYPE   == C.selectionType());
                LOOP_ASSERT(LINE, VAL_A  == C.selection());

                LOOP_ASSERT(LINE, j      == D.selector());
                LOOP_ASSERT(LINE, TYPE   == D.selectionType());
                LOOP_ASSERT(LINE, VAL_A  == D.selection());

                LOOP_ASSERT(LINE, j      == E.selector());
                LOOP_ASSERT(LINE, TYPE   == E.selectionType());
                LOOP_ASSERT(LINE, VAL_A  == E.selection());

                LOOP_ASSERT(LINE, j      == F.selector());
                LOOP_ASSERT(LINE, TYPE   == F.selectionType());
                LOOP_ASSERT(LINE, VAL_A  == F.selection());

                LOOP_ASSERT(LINE, A == B);
                LOOP_ASSERT(LINE, B == C);
                LOOP_ASSERT(LINE, C == D);
                LOOP_ASSERT(LINE, D == E);
                LOOP_ASSERT(LINE, E == F);

                mA.makeSelection(j).replaceValue(VAL_B);
                mB.makeSelection(j).replaceValue(VAL_B);
                mC.makeSelection(j).replaceValue(VAL_B);
                mD.makeSelection(j).replaceValue(VAL_B);
                mE.makeSelection(j).replaceValue(VAL_B);
                mF.makeSelection(j).replaceValue(VAL_B);

                LOOP_ASSERT(LINE, j      == A.selector());
                LOOP_ASSERT(LINE, TYPE   == A.selectionType());
                LOOP_ASSERT(LINE, VAL_B  == A.selection());

                LOOP_ASSERT(LINE, j      == B.selector());
                LOOP_ASSERT(LINE, TYPE   == B.selectionType());
                LOOP_ASSERT(LINE, VAL_B  == B.selection());

                LOOP_ASSERT(LINE, j      == C.selector());
                LOOP_ASSERT(LINE, TYPE   == C.selectionType());
                LOOP_ASSERT(LINE, VAL_B  == C.selection());

                LOOP_ASSERT(LINE, j      == D.selector());
                LOOP_ASSERT(LINE, TYPE   == D.selectionType());
                LOOP_ASSERT(LINE, VAL_B  == D.selection());

                LOOP_ASSERT(LINE, j      == E.selector());
                LOOP_ASSERT(LINE, TYPE   == E.selectionType());
                LOOP_ASSERT(LINE, VAL_B  == E.selection());

                LOOP_ASSERT(LINE, j      == F.selector());
                LOOP_ASSERT(LINE, TYPE   == F.selectionType());
                LOOP_ASSERT(LINE, VAL_B  == F.selection());

                LOOP_ASSERT(LINE, A == B);
                LOOP_ASSERT(LINE, B == C);
                LOOP_ASSERT(LINE, C == D);
                LOOP_ASSERT(LINE, D == E);
                LOOP_ASSERT(LINE, E == F);

                mA.makeSelection(-1);
                mB.makeSelection(-1);
                mC.makeSelection(-1);
                mD.makeSelection(-1);
                mE.makeSelection(-1);
                mF.makeSelection(-1);

                LOOP_ASSERT(LINE, -1          == A.selector());
                LOOP_ASSERT(LINE, EType::BDEM_VOID == A.selectionType());
                LOOP_ASSERT(LINE, EType::BDEM_VOID == A.selection().type());

                LOOP_ASSERT(LINE, -1          == B.selector());
                LOOP_ASSERT(LINE, EType::BDEM_VOID == B.selectionType());
                LOOP_ASSERT(LINE, EType::BDEM_VOID == B.selection().type());

                LOOP_ASSERT(LINE, -1          == C.selector());
                LOOP_ASSERT(LINE, EType::BDEM_VOID == C.selectionType());
                LOOP_ASSERT(LINE, EType::BDEM_VOID == C.selection().type());

                LOOP_ASSERT(LINE, -1          == D.selector());
                LOOP_ASSERT(LINE, EType::BDEM_VOID == D.selectionType());
                LOOP_ASSERT(LINE, EType::BDEM_VOID == D.selection().type());

                LOOP_ASSERT(LINE, -1          == E.selector());
                LOOP_ASSERT(LINE, EType::BDEM_VOID == E.selectionType());
                LOOP_ASSERT(LINE, EType::BDEM_VOID == E.selection().type());

                LOOP_ASSERT(LINE, -1          == F.selector());
                LOOP_ASSERT(LINE, EType::BDEM_VOID == F.selectionType());
                LOOP_ASSERT(LINE, EType::BDEM_VOID == F.selection().type());

                LOOP_ASSERT(LINE, A == B);
                LOOP_ASSERT(LINE, B == C);
                LOOP_ASSERT(LINE, C == D);
                LOOP_ASSERT(LINE, D == E);
                LOOP_ASSERT(LINE, E == F);
              }
            } END_BSLMA_EXCEPTION_TEST
            }
          }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'addSelection' MANIPULATOR
        //
        // Concerns:
        //   Using 'addSelection' adds a selection to the catalog
        //
        // Plan:
        //   Construct several objects with different values.  Confirm that
        //   calling the addSelection adds a new selection to the object and
        //   does not modify the current selector value.
        //
        // Testing:
        //   void addSelection(EType::Type type);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting addSelection Function"
                               << "\n============================="
                               << bsl::endl;

        static const struct TestRow {
            int         d_line;
            const char *d_catalogSpec;  // Specification to create the catalog
        } DATA[] = {
            // Line     DescriptorSpec
            // ====     ==============
            { L_,       "A" },
            { L_,       "B" },
            { L_,       "C" },
            { L_,       "D" },
            { L_,       "E" },
            { L_,       "F" },
            { L_,       "G" },
            { L_,       "H" },
            { L_,       "I" },
            { L_,       "J" },
            { L_,       "K" },
            { L_,       "L" },
            { L_,       "M" },
            { L_,       "N" },
            { L_,       "O" },
            { L_,       "P" },
            { L_,       "Q" },
            { L_,       "R" },
            { L_,       "S" },
            { L_,       "T" },
            { L_,       "U" },
            { L_,       "V" },
            { L_,       "W" },
            { L_,       "X" },
            { L_,       "Y" },
            { L_,       "Z" },
            { L_,       "a" },
            { L_,       "b" },
            { L_,       "c" },
            { L_,       "d" },
            { L_,       "e" },
            { L_,       "f" },

            // Testing various interesting combinations
            { L_,       "AA" },
            { L_,       "AC" },
            { L_,       "CD" },
            { L_,       "CE" },
            { L_,       "FG" },
            { L_,       "GG" },
            { L_,       "GH" },
            { L_,       "MN" },
            { L_,       "OP" },
            { L_,       "PQ" },
            { L_,       "KQ" },
            { L_,       "ST" },
            { L_,       "WX" },
            { L_,       "UV" },
            { L_,       "YZ" },
            { L_,       "ab" },
            { L_,       "cd" },
            { L_,       "ef" },
            { L_,       "Ue" },
            { L_,       "Vf" },

            { L_,       "HIJ" },
            { L_,       "KLM" },
            { L_,       "RST" },
            { L_,       "YZa" },

            { L_,       "BLRW" },
            { L_,       "DGNQ" },
            { L_,       "QRST" },

            { L_,       "JHKHSK" },
            { L_,       "RISXLSW" },
            { L_,       "MXKZOLPR" },
            { L_,       "GGGGGGGGG" },
            { L_,       "QQQQQQQQQQ" },
            { L_,       "abcdABCDEFG" },
            { L_,       "FDLALAabADSF" },

            { L_,       "GSRWSASDEFDSA" },
            { L_,       "PKJHALKGabASDA" },
            { L_,       "XZSAEWRPOIJLKaAS" },
            { L_,       "GALKacKASJDKSWEIO" },

            { L_,       "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef" },
        };
        static const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

        if (veryVerbose) { bsl::cout << "Adding selections to a empty obj"
                                     << bsl::endl; }
        {
            Obj mX; const Obj& X = mX;
            ASSERT(0  == X.numSelections());
            ASSERT(-1 == X.selector());

            for (int i = 0; i < EType::BDEM_NUM_TYPES; ++i) {
                EType::Type TYPE = (EType::Type) i;
                mX.addSelection(TYPE);
                ASSERT(i + 1 == X.numSelections());
                ASSERT(TYPE  == X.selectionType(i));
                ASSERT(-1 == X.selector());
            }
        }

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE  = DATA[i].d_line;
            const char *SPEC  = DATA[i].d_catalogSpec;
            const int   LEN   = bsl::strlen(SPEC);

            const TypesCatalog CATALOG = ggCatalog(SPEC);

            const int         SEL   = i % LEN;
            const EType::Type TYPE  = getElemType(SPEC[SEL]);
            const CERef       VALUE = getConstERefA(SPEC[SEL]);
            EType::Type randomType =
                (EType::Type ) ((i + 10) % EType::BDEM_NUM_TYPES);

            if (veryVerbose) { P(SEL) P(randomType) }

            bslma_TestAllocator alloc(veryVeryVerbose);
            Obj mX(CATALOG, &alloc); const Obj& X = mX;
            mX.makeSelection(SEL).replaceValue(VALUE);

            LOOP_ASSERT(LINE, LEN   == X.numSelections());
            LOOP_ASSERT(LINE, SEL   == X.selector());
            LOOP_ASSERT(LINE, VALUE == X.selection());
            LOOP_ASSERT(LINE, TYPE  == X.selectionType());

            mX.addSelection(randomType);
            LOOP_ASSERT(LINE, LEN + 1 == X.numSelections());
            LOOP_ASSERT(LINE, randomType == X.selectionType(LEN));
            LOOP_ASSERT(LINE, SEL == X.selector());
            LOOP_ASSERT(LINE, VALUE == X.selection());
            LOOP_ASSERT(LINE, TYPE  == X.selectionType());
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'bdex' STREAMING FUNCTIONALITY:
        //
        // Concerns:
        //  -The 'bdex' streaming concerns for this component are standard.  We
        //   first test the class method 'maxSupportedBdexVersion' and then
        //   use that method to probe the member functions 'outStream' and
        //   'inStream' in the manner of a "breathing test" to verify basic
        //   functionality.  We then thoroughly test streaming functionality
        //   relying on the available bdex 'streamOut' and 'streamIn' functions
        //   which forward appropriate calls to the member functions of this
        //   component.  We next step through the sequence of possible stream
        //   states (valid, empty, invalid, incomplete, and corrupted),
        //   appropriately selecting data sets as described below.  In all
        //   cases, exception neutrality is confirmed using the specially
        //   instrumented 'bdex_TestInStream' and a pair of standard macros,
        //   'BEGIN_BDEX_EXCEPTION_TEST' and 'END_BDEX_EXCEPTION_TEST', which
        //   configure the 'bdex_TestInStream' object appropriately in a
        //   loop.
        //  -Nullness is correctly encoded/decoded.
        //
        // Plan:
        //   PRELIMINARY MEMBER FUNCTION TEST
        //     First test 'maxSupportedBdexVersion' explicitly, and then
        //     perform a trivial direct test of the 'outStream' and 'inStream'
        //     methods (the rest of the testing will use the stream operators).
        //
        //   VALID STREAMS
        //     For the set S of globally-defined test values, use all
        //     combinations (mX, mY) in the cross product S X S, stream the
        //     value of mY into (a temporary copy of) mX and assert mX == mY.
        //
        //   EMPTY AND INVALID STREAMS
        //     For each mX in S, create a copy and attempt to stream into it
        //     from an empty and then invalid stream.  Verify after each try
        //     that the object is unchanged and that the stream is invalid.
        //
        //   INCOMPLETE (BUT OTHERWISE VALID) DATA
        //     Write 3 distinct objects to an output stream buffer of total
        //     length N.  For each partial stream length from 0 to N - 1,
        //     construct an input stream and attempt to read into objects
        //     initialized with distinct values.  Verify values of objects
        //     that are either successfully modified or left entirely
        //     unmodified, and that the stream became invalid immediately after
        //     the first incomplete read.  Finally ensure that each object
        //     streamed into is in some valid state by assigning it a distinct
        //     new value and testing for equality.
        //
        //   CORRUPTED DATA
        //     Use the underlying stream package to simulate an instance of a
        //     typical valid (control) stream and verify that it can be
        //     streamed in successfully.  Then for each data field in the
        //     stream (beginning with the version number), provide one or more
        //     similar tests with that data field corrupted.  After each test,
        //     verify that the object is in some valid state after streaming,
        //     and that the input stream has gone invalid.
        //
        // Testing:
        //   int maxSupportedBdexVersion() const;
        //   STREAM& bdexStreamOut(STREAM& stream, int version);
        //   STREAM& bdexStreamIn(STREAM& stream, int version);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Streaming Functionality"
                               << "\n==============================="
                               << bsl::endl;

        if (veryVerbose) {
            bsl::cout << "\tTesting maxSupportedBdexVersion" << bsl::endl; }
        const int MAX_VERSION = 3;
        {
            ASSERT(MAX_VERSION == Obj::maxSupportedBdexVersion());
        }

        static const struct TestRow {
            int         d_line;
            const char *d_catalogSpec;  // Specification to create the catalog
        } DATA[] = {
            // Line     DescriptorSpec
            // ====     ==============
            { L_,       "C" },
            { L_,       "D" },
            { L_,       "F" },
            { L_,       "G" },
            { L_,       "H" },
            { L_,       "M" },
            { L_,       "P" },
            { L_,       "Q" },
            { L_,       "R" },
            { L_,       "U" },
            { L_,       "V" },
            { L_,       "W" },
            { L_,       "X" },
            { L_,       "b" },
            { L_,       "e" },
            { L_,       "f" },

            // Testing various interesting combinations
            { L_,       "GQ"                             },
            { L_,       "UV"                             },
            { L_,       "ef"                             },
            { L_,       "YZa"                            },
            { L_,       "BLRW"                           },
            { L_,       "JHKHSK"                         },
            { L_,       "efQDEFG"                        },
            { L_,       "ABCDEFGHIJKLMNOPQRSTWXYZabcdef" },
        };
        static const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

        for (int VERSION = 1; VERSION <= MAX_VERSION; ++VERSION) {
            for (int i1 = 0; i1 < NUM_DATA; ++i1) {
                const int   LINE1  = DATA[i1].d_line;
                const char *SPEC1  = DATA[i1].d_catalogSpec;
                const int   LEN1   = bsl::strlen(SPEC1);

                if (veryVerbose) { P(SPEC1); }

                for (int j1 = 0; j1 < LEN1; ++j1) {
                    const char  S1   = SPEC1[j1];
                    const CERef C1_A = getConstERefA(S1);
                    const TypesCatalog CATALOG1 = ggCatalog(SPEC1);

                    bslma_TestAllocator alloc1(veryVeryVerbose);
                    Obj mX(CATALOG1, &alloc1);  const Obj& X = mX;
                    Obj mY(CATALOG1, &alloc1);  const Obj& Y = mY;

                    mX.makeSelection(j1).replaceValue(C1_A);
                    LOOP_ASSERT(LINE1, !X.selection().isNull());

                    mY.makeSelection(j1);
                    LOOP_ASSERT(LINE1, Y.selection().isNull());

                    bdex_TestOutStream os1;
                    bdex_TestOutStream os2;
                    bdex_TestOutStream os3;
                    bdex_TestOutStream os4;

                    X.bdexStreamOut(os1, VERSION);
                    bdex_OutStreamFunctions::streamOut(os2, X, VERSION);
                    Y.bdexStreamOut(os3, VERSION);
                    bdex_OutStreamFunctions::streamOut(os4, Y, VERSION);

                    // Stream a constructed obj to an empty obj.
                    {
                        bslma_TestAllocator tmpAlloc(veryVeryVerbose);
                        Obj mA(&tmpAlloc);  const Obj& A = mA;
                        Obj mB(&tmpAlloc);  const Obj& B = mB;
                        Obj mC(&tmpAlloc);  const Obj& C = mC;
                        Obj mD(&tmpAlloc);  const Obj& D = mD;

                        LOOP_ASSERT(LINE1, A.selection().isNull());
                        LOOP_ASSERT(LINE1, B.selection().isNull());
                        LOOP_ASSERT(LINE1, C.selection().isNull());
                        LOOP_ASSERT(LINE1, D.selection().isNull());

                        LOOP_ASSERT(LINE1, X != A);
                        LOOP_ASSERT(LINE1, X != B);
                        LOOP_ASSERT(LINE1, X != C);
                        LOOP_ASSERT(LINE1, X != D);

                        LOOP_ASSERT(LINE1, Y != A);
                        LOOP_ASSERT(LINE1, Y != B);
                        LOOP_ASSERT(LINE1, Y != C);
                        LOOP_ASSERT(LINE1, Y != D);

                        bdex_TestInStream is1(os1.data(), os1.length());
                        bdex_TestInStream is2(os2.data(), os2.length());
                        bdex_TestInStream is3(os3.data(), os3.length());
                        bdex_TestInStream is4(os4.data(), os4.length());

                        is1.setSuppressVersionCheck(1);
                        is2.setSuppressVersionCheck(1);
                        is3.setSuppressVersionCheck(1);
                        is4.setSuppressVersionCheck(1);
                        mA.bdexStreamIn(is1, VERSION);
                        bdex_InStreamFunctions::streamIn(is2, mB, VERSION);
                        mC.bdexStreamIn(is3, VERSION);
                        bdex_InStreamFunctions::streamIn(is4, mD, VERSION);

                        LOOP_ASSERT(LINE1, !A.selection().isNull());
                        LOOP_ASSERT(LINE1, !B.selection().isNull());
                        LOOP3_ASSERT(LINE1, X, A, X == A);
                        LOOP3_ASSERT(LINE1, X, B, X == B);
                        if (VERSION < 3) {
                            LOOP_ASSERT(LINE1, !C.selection().isNull());
                            LOOP3_ASSERT(LINE1, Y, C,
                                         compare(Y.selection(),
                                                 C.selection(), S1));
                        }
                        else {
                            LOOP_ASSERT(LINE1, C.selection().isNull());
                            LOOP3_ASSERT(LINE1, Y, C, Y == C);
                        }
                    }

                    for (int i2 = 0; i2 < NUM_DATA; ++i2) {
                        const int   LINE2  = DATA[i2].d_line;
                        const char *SPEC2  = DATA[i2].d_catalogSpec;
                        const int   LEN2   = bsl::strlen(SPEC2);

                        if (veryVerbose) { P(SPEC2); }
                        for (int j2 = 0; j2 < LEN2; ++j2) {
                            const char  S2   = SPEC2[j2];
                            const CERef C2_B = getConstERefB(S2);

                            const TypesCatalog CATALOG2 = ggCatalog(SPEC2);

                            bslma_TestAllocator testAllocator(veryVeryVerbose);

                            BEGIN_BSLMA_EXCEPTION_TEST {
                                Obj mA(CATALOG2, &testAllocator);
                                const Obj& A = mA;
                                Obj mB(CATALOG2, &testAllocator);
                                const Obj& B = mB;
                                Obj mC(CATALOG2, &testAllocator);
                                const Obj& C = mC;
                                Obj mD(CATALOG2, &testAllocator);
                                const Obj& D = mD;
                                mA.makeSelection(j2).replaceValue(C2_B);
                                mB.makeSelection(j2).replaceValue(C2_B);
                                mC.makeSelection(j2).replaceValue(C2_B);
                                mD.makeSelection(j2).replaceValue(C2_B);

                                mA.selection().makeNull();
                                mC.selection().makeNull();

                                bdex_TestInStream is1(os1.data(),
                                                      os1.length());
                                bdex_TestInStream is2(os2.data(),
                                                      os2.length());
                                bdex_TestInStream is3(os3.data(),
                                                      os3.length());
                                bdex_TestInStream is4(os4.data(),
                                                      os4.length());

                                is1.setSuppressVersionCheck(1);
                                is2.setSuppressVersionCheck(1);
                                is3.setSuppressVersionCheck(1);
                                is4.setSuppressVersionCheck(1);
                                mA.bdexStreamIn(is1, VERSION);
                                bdex_InStreamFunctions::streamIn(is2,
                                                                 mB,
                                                                 VERSION);
                                mC.bdexStreamIn(is3, VERSION);
                                bdex_InStreamFunctions::streamIn(is4,
                                                                 mD,
                                                                 VERSION);

                                LOOP4_ASSERT(LINE1, LINE2, X, A, X == A);
                                LOOP4_ASSERT(LINE1, LINE2, X, B, X == B);
                                LOOP2_ASSERT(LINE1, LINE2,
                                             !A.selection().isNull());
                                LOOP2_ASSERT(LINE1, LINE2,
                                             !B.selection().isNull());
                                if (VERSION == MAX_VERSION) {
                                    LOOP_ASSERT(LINE1, D.selection().isNull());
                                }
                                else {
                                    LOOP_ASSERT(LINE1,
                                                !D.selection().isNull());
                                }

                                LOOP3_ASSERT(LINE1, Y, D,
                                             compare(Y.selection(),
                                                     D.selection(), S1));
                                if (VERSION < 3) {
                                    LOOP2_ASSERT(LINE1, LINE2,
                                                !C.selection().isNull());
                                    LOOP3_ASSERT(LINE1, Y, C,
                                                 compare(Y.selection(),
                                                         C.selection(),
                                                         S1));
                                }
                                else {
                                    LOOP2_ASSERT(LINE1, LINE2,
                                                 C.selection().isNull());
                                    LOOP3_ASSERT(LINE1, Y, C, Y == C);
                                }
                            } END_BSLMA_EXCEPTION_TEST
                        }
                    }

                    // Stream from an empty and invalid stream.
                    {
                        bslma_TestAllocator tmpAlloc(veryVeryVerbose);
                        Obj mA(X, &tmpAlloc);  const Obj& A = mA;
                        Obj mB(X, &tmpAlloc);  const Obj& B = mB;
                        Obj mC(Y, &tmpAlloc);  const Obj& C = mC;
                        Obj mD(Y, &tmpAlloc);  const Obj& D = mD;

                        LOOP_ASSERT(LINE1, !A.selection().isNull());
                        LOOP_ASSERT(LINE1, !B.selection().isNull());
                        LOOP_ASSERT(LINE1,  C.selection().isNull());
                        LOOP_ASSERT(LINE1,  D.selection().isNull());

                        LOOP_ASSERT(LINE1, A == X);
                        LOOP_ASSERT(LINE1, B == X);
                        LOOP_ASSERT(LINE1, C == Y);
                        LOOP_ASSERT(LINE1, D == Y);
                        bdex_TestInStream is1;  // empty stream
                        bdex_TestInStream is2;  // empty stream
                        bdex_TestInStream is3;  // empty stream
                        bdex_TestInStream is4;  // empty stream

                        is1.setSuppressVersionCheck(1);
                        is2.setSuppressVersionCheck(1);
                        is3.setSuppressVersionCheck(1);
                        is4.setSuppressVersionCheck(1);
                        mA.bdexStreamIn(is1, VERSION);
                        bdex_InStreamFunctions::streamIn(is2, mB, VERSION);
                        mC.bdexStreamIn(is3, VERSION);
                        bdex_InStreamFunctions::streamIn(is4, mD, VERSION);

                        LOOP_ASSERT(LINE1, A == X);
                        LOOP_ASSERT(LINE1, B == X);
                        LOOP_ASSERT(LINE1, C == Y);
                        LOOP_ASSERT(LINE1, D == Y);
                        ASSERT(A == B);
                        ASSERT(C == D);
                        ASSERT(!is1);
                        ASSERT(!is2);
                        ASSERT(!is3);
                        ASSERT(!is4);

                        bdex_TestInStream is5(os2.data(), os2.length());
                        is5.invalidate();  // invalid stream
                        bdex_TestInStream is6(os2.data(), os2.length());
                        is6.invalidate();  // invalid stream
                        bdex_TestInStream is7(os2.data(), os2.length());
                        is7.invalidate();  // invalid stream
                        bdex_TestInStream is8(os2.data(), os2.length());
                        is8.invalidate();  // invalid stream

                        is5.setSuppressVersionCheck(1);
                        is6.setSuppressVersionCheck(1);
                        is7.setSuppressVersionCheck(1);
                        is8.setSuppressVersionCheck(1);
                        mA.bdexStreamIn(is5, VERSION);
                        bdex_InStreamFunctions::streamIn(is6, mB, VERSION);
                        mC.bdexStreamIn(is7, VERSION);
                        bdex_InStreamFunctions::streamIn(is8, mD, VERSION);

                        LOOP_ASSERT(LINE1, A == X);
                        LOOP_ASSERT(LINE1, B == X);
                        LOOP_ASSERT(LINE1, C == Y);
                        LOOP_ASSERT(LINE1, D == Y);
                        ASSERT(A == B);
                        ASSERT(C == D);
                        ASSERT(!is5);
                        ASSERT(!is6);
                        ASSERT(!is7);
                        ASSERT(!is8);
                    }

                    // Stream an empty obj to a constructed obj.
                    {
                        bslma_TestAllocator tmpAlloc(veryVeryVerbose);
                        Obj mA(X, &tmpAlloc);  const Obj& A = mA;
                        Obj mB(X, &tmpAlloc);  const Obj& B = mB;
                        Obj mC(&tmpAlloc);     const Obj& C = mC;  // empty obj
                        mA.selection().makeNull();

                        if (veryVerbose) { P(X) P(A) P(B) }
                        LOOP_ASSERT(LINE1, A.selection().isNull());
                        LOOP_ASSERT(LINE1, !B.selection().isNull());

                        LOOP_ASSERT(LINE1, A == Y);
                        LOOP_ASSERT(LINE1, B == X);

                        LOOP_ASSERT(LINE1, C != A);
                        LOOP_ASSERT(LINE1, C != B);

                        bdex_TestOutStream os1;
                        bdex_TestOutStream os2;

                        C.bdexStreamOut(os1, VERSION);
                        bdex_OutStreamFunctions::streamOut(os2, C, VERSION);

                        bdex_TestInStream is1(os1.data(), os1.length());
                        bdex_TestInStream is2(os2.data(), os2.length());

                        is1.setSuppressVersionCheck(1);
                        is2.setSuppressVersionCheck(1);
                        mA.bdexStreamIn(is1, VERSION);
                        bdex_InStreamFunctions::streamIn(is2, mB, VERSION);

                        if (veryVerbose) { P(A) P(B) P(C) }
                        LOOP_ASSERT(LINE1, A == C);
                        LOOP_ASSERT(LINE1, B == C);
                    }
                }
            }
        }
      } break;
      case 7: {
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
        //   bdem_Choice& operator=(const bdem_Choice& rhs);
        //   bdem_Choice& operator=(const bdem_ChoiceArrayItem& rhs);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Assignment Operator"
                               << "\n===========================" << bsl::endl;
        const struct {
            int         d_line;
            const char *d_catalogSpec;  // Specification to create the catalog
        } DATA[] = {
            // Line     DescriptorSpec
            // ====     ==============
            { L_,       "C" },
            { L_,       "D" },
            { L_,       "F" },
            { L_,       "G" },
            { L_,       "H" },
            { L_,       "M" },
            { L_,       "P" },
            { L_,       "Q" },
            { L_,       "R" },
            { L_,       "U" },
            { L_,       "V" },
            { L_,       "W" },
            { L_,       "X" },
            { L_,       "b" },
            { L_,       "e" },
            { L_,       "f" },

            // Testing various interesting combinations
            { L_,       "GQ" },
            { L_,       "UV" },
            { L_,       "ef" },

            { L_,       "YZa" },
            { L_,       "BLRW" },
            { L_,       "JHKHSK" },
            { L_,       "efQDEFG" },
            { L_,       "ABCDEFGHIJKLMNOPQRSTWXYZabcdef" },
        };
        static const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

        for (int i1 = 0; i1 < NUM_DATA; ++i1) {
            const int   LINE1  = DATA[i1].d_line;
            const char *SPEC1  = DATA[i1].d_catalogSpec;
            const int   LEN1   = bsl::strlen(SPEC1);

            if (veryVerbose) { P(SPEC1); }

            for (int j1 = 0; j1 < LEN1; ++j1) {
                const char   S1  = SPEC1[j1];
                const CERef C1_A = getConstERefA(S1);
                const CERef C1_B = getConstERefB(S1);

                const TypesCatalog CATALOG1 = ggCatalog(SPEC1);

                bslma_TestAllocator alloc1(veryVeryVerbose);
                Obj mX(CATALOG1, &alloc1); const Obj& X = mX;
                Obj mY(CATALOG1, &alloc1); const Obj& Y = mY;
                Obj mN(CATALOG1, &alloc1); const Obj& N = mN;

                mX.makeSelection(j1).replaceValue(C1_A);
                mY.makeSelection(j1).replaceValue(C1_A);
                mN.selection().makeNull();

                const ChoiceArrayItem& I  = X.item();
                const ChoiceArrayItem& IN = N.item();

                ASSERT(N.selection().isNull());
                ASSERT(IN.selection().isNull());

                // Assign a constructed obj to an empty obj
                {
                    bslma_TestAllocator tmpAlloc(veryVeryVerbose);
                    Obj mA(&tmpAlloc); const Obj& A = mA;
                    Obj mB(&tmpAlloc); const Obj& B = mB;

                    LOOP_ASSERT(LINE1, X != A);
                    LOOP_ASSERT(LINE1, X != B);
                    LOOP_ASSERT(LINE1, X == Y);

                    const int BYTES = alloc1.numBytesInUse();
                    mA = X;
                    mB = I;

                    LOOP_ASSERT(LINE1, X == A);
                    LOOP_ASSERT(LINE1, X == B);
                    LOOP_ASSERT(LINE1, X == Y);
                    LOOP_ASSERT(LINE1, BYTES == alloc1.numBytesInUse());
                }

                // Assign a constructed (null) obj to an empty obj
                {
                    bslma_TestAllocator tmpAlloc(veryVeryVerbose);
                    Obj mA(&tmpAlloc); const Obj& A = mA;
                    Obj mB(&tmpAlloc); const Obj& B = mB;

                    LOOP_ASSERT(LINE1, N != A);
                    LOOP_ASSERT(LINE1, N != B);
                    LOOP_ASSERT(LINE1, N != X);

                    const int BYTES = alloc1.numBytesInUse();
                    mA = N;
                    mB = IN;

                    LOOP_ASSERT(LINE1, N == A);
                    LOOP_ASSERT(LINE1, N == B);
                    LOOP_ASSERT(LINE1, N != X);
                    LOOP_ASSERT(LINE1, BYTES == alloc1.numBytesInUse());
                }

                for (int i2 = 0; i2 < NUM_DATA; ++i2) {
                    const int   LINE2  = DATA[i2].d_line;
                    const char *SPEC2  = DATA[i2].d_catalogSpec;
                    const int   LEN2   = bsl::strlen(SPEC2);

                    if (veryVerbose) { P(SPEC2); }

                    for (int j2 = 0; j2 < LEN2; ++j2) {
                        const char  S2   = SPEC2[j2];
                        const CERef C2_B = getConstERefB(S2);

                        const TypesCatalog CATALOG2 = ggCatalog(SPEC2);

                        for (int k = 0; k < 4; ++k) {
                            AggOption::AllocationStrategy mode =
                                            (AggOption::AllocationStrategy) k;

                            bslma_TestAllocator testAllocator(veryVeryVerbose);

                          BEGIN_BSLMA_EXCEPTION_TEST {

                            bdema_SequentialAllocator seqAlloc(&testAllocator);
                            bslma_Allocator          *alloc = &testAllocator;

                            if (AggOption::BDEM_SUBORDINATE == mode) {
                                alloc = &seqAlloc;
                            }

                            Obj mA(CATALOG2, mode, alloc);
                            Obj mB(CATALOG2, mode, alloc);
                            const Obj& A = mA;
                            const Obj& B = mB;

                            mA.makeSelection(j2).replaceValue(C2_B);
                            mB.makeSelection(j2).replaceValue(C2_B);
                            LOOP2_ASSERT(LINE1, LINE2, X != A);
                            LOOP2_ASSERT(LINE1, LINE2, X != B);
                            LOOP2_ASSERT(LINE1, LINE2, X == Y);

                            const int BYTES = alloc1.numBytesInUse();
                            mA = X;
                            mB = I;

                            LOOP2_ASSERT(LINE1, LINE2, X == A);
                            LOOP2_ASSERT(LINE1, LINE2, X == B);
                            LOOP2_ASSERT(LINE1, LINE2, X == Y);
                            LOOP2_ASSERT(LINE1,
                                         LINE2,
                                         BYTES == alloc1.numBytesInUse());

                            LOOP2_ASSERT(LINE1, LINE2, N != A);
                            LOOP2_ASSERT(LINE1, LINE2, N != B);
                            LOOP2_ASSERT(LINE1, LINE2, X != N);

                            const int BYTESN = alloc1.numBytesInUse();
                            mA = N;
                            mB = IN;

                            LOOP2_ASSERT(LINE1, LINE2, N == A);
                            LOOP2_ASSERT(LINE1, LINE2, N == B);
                            LOOP2_ASSERT(LINE1, LINE2, X != N);
                            LOOP2_ASSERT(LINE1,
                                         LINE2,
                                         BYTESN == alloc1.numBytesInUse());

                            mA = X;
                            mB = I;

                            // We don't need to check exception neutrality
                            // anymore
                            testAllocator.setAllocationLimit(-1);
                            mX.makeSelection(j1).replaceValue(C1_B);
                            LOOP2_ASSERT(LINE1, LINE2, X != A);
                            LOOP2_ASSERT(LINE1, LINE2, X != B);
                            LOOP2_ASSERT(LINE1, LINE2, Y == A);
                            LOOP2_ASSERT(LINE1, LINE2, Y == B);

                            mX.makeSelection(j1).replaceValue(C1_A);
                            LOOP2_ASSERT(LINE1, LINE2, X == A);
                            LOOP2_ASSERT(LINE1, LINE2, X == B);
                            LOOP2_ASSERT(LINE1, LINE2, X == Y);

                            mA.makeSelection(j1).replaceValue(C1_B);
                            LOOP2_ASSERT(LINE1, LINE2, X != A);
                            LOOP2_ASSERT(LINE1, LINE2, X == Y);

                            mA.makeSelection(j1).replaceValue(C1_A);
                            LOOP2_ASSERT(LINE1, LINE2, X == A);
                            LOOP2_ASSERT(LINE1, LINE2, Y == A);

                            mB.makeSelection(j1).replaceValue(C1_B);
                            LOOP2_ASSERT(LINE1, LINE2, X != B);
                            LOOP2_ASSERT(LINE1, LINE2, X == Y);

                            mB.makeSelection(j1).replaceValue(C1_A);
                            LOOP2_ASSERT(LINE1, LINE2, X == B);
                            LOOP2_ASSERT(LINE1, LINE2, Y == A);

                            Obj mC; const Obj& C = mC;
                            LOOP2_ASSERT(LINE1, LINE2, A != C);
                            LOOP2_ASSERT(LINE1, LINE2, B != C);
                            LOOP2_ASSERT(LINE1, LINE2, X == A);
                            LOOP2_ASSERT(LINE1, LINE2, X == B);
                            LOOP2_ASSERT(LINE1, LINE2, X == Y);

                            // Assign an empty obj to a constructed obj
                            mA = C;
                            mB = C;
                            LOOP2_ASSERT(LINE1, LINE2, A == C);
                            LOOP2_ASSERT(LINE1, LINE2, B == C);
                            LOOP2_ASSERT(LINE1, LINE2, X != A);
                            LOOP2_ASSERT(LINE1, LINE2, X != B);
                            LOOP2_ASSERT(LINE1, LINE2, X == Y);

                            // Assign an empty obj to a constructed obj (null)
                            mA = N;
                            mB = IN;

                            LOOP2_ASSERT(LINE1, LINE2, N == A);
                            LOOP2_ASSERT(LINE1, LINE2, N == B);
                            LOOP2_ASSERT(LINE1, LINE2, X != N);

                            mA = C;
                            mB = C;
                            LOOP2_ASSERT(LINE1, LINE2, A == C);
                            LOOP2_ASSERT(LINE1, LINE2, B == C);
                            LOOP2_ASSERT(LINE1, LINE2, N != A);
                            LOOP2_ASSERT(LINE1, LINE2, N != B);
                            LOOP2_ASSERT(LINE1, LINE2, N != Y);
                          } END_BSLMA_EXCEPTION_TEST
                        }
                    }
                }
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR:
        //
        // Concerns:
        //   Any value must be able to be copy constructed without affecting
        //   its argument.  For values that require dynamic allocation, each
        //   copy must allocate storage independently from its own allocator.
        //
        // Plan:
        //   Specify a set S whose elements have substantial and varied
        //   differences in value.  For each element in S, construct and
        //   initialize identically valued objects mX and mY, where mX is used
        //   for copy construction and mY serves as the control object used to
        //   verify equality with original value.  Then copy construct two
        //   objects mA and mB from mX.  mA is created by specifying not
        //   allocation strategy and the test allocator and mB using multiple
        //   allocation strategies and an appropriate allocator.  Use the
        //   equality operator to confirm that all mX, mY, mA and mB have the
        //   same value.  Modify mX and confirm that mA and mB are not
        //   modified and are equal to mY.  Modify each of mA and mB and
        //   confirm that mX remains the same and equal to mY.  Finally, reset
        //   mX and confirm that none of mA or mB are changed.  Using the
        //   allocators specified to construct the objects ensure that each
        //   object uses the correct allocator for memory allocations.
        //
        // Testing:
        //   bdem_Choice(const bdem_Choice& original, alloc = 0);
        //   bdem_Choice(const bdem_Choice& original, allocMode, alloc = 0);
        //   bdem_Choice(const Item& item, alloc = 0);
        //   bdem_Choice(const Item& item, allocMode, alloc = 0);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Copy Constructor"
                               << "\n========================" << bsl::endl;

        const struct {
            int         d_line;
            const char *d_catalogSpec;  // Specification to create the catalog
        } DATA[] = {
            // Line     DescriptorSpec
            // ====     ==============
            { L_,       "A" },
            { L_,       "B" },
            { L_,       "C" },
            { L_,       "D" },
            { L_,       "E" },
            { L_,       "F" },
            { L_,       "G" },
            { L_,       "H" },
            { L_,       "I" },
            { L_,       "J" },
            { L_,       "K" },
            { L_,       "L" },
            { L_,       "M" },
            { L_,       "N" },
            { L_,       "O" },
            { L_,       "P" },
            { L_,       "Q" },
            { L_,       "R" },
            { L_,       "S" },
            { L_,       "T" },
            { L_,       "U" },
            { L_,       "V" },
            { L_,       "W" },
            { L_,       "X" },
            { L_,       "Y" },
            { L_,       "Z" },
            { L_,       "a" },
            { L_,       "b" },
            { L_,       "c" },
            { L_,       "d" },
            { L_,       "e" },
            { L_,       "f" },

            // Testing various interesting combinations
            { L_,       "AA" },
            { L_,       "AC" },
            { L_,       "CD" },
            { L_,       "CE" },
            { L_,       "FG" },
            { L_,       "GG" },
            { L_,       "GH" },
            { L_,       "MN" },
            { L_,       "OP" },
            { L_,       "PQ" },
            { L_,       "KQ" },
            { L_,       "ST" },
            { L_,       "WX" },
            { L_,       "YZ" },
            { L_,       "ab" },
            { L_,       "cd" },
            { L_,       "ef" },

            { L_,       "HIJ" },
            { L_,       "KLM" },
            { L_,       "RST" },
            { L_,       "YZa" },

            { L_,       "BLRW" },
            { L_,       "DGNQ" },
            { L_,       "QRST" },

            { L_,       "JHKHSK" },
            { L_,       "RISXLSW" },
            { L_,       "MXKZOLPR" },
            { L_,       "GGGGGGGGG" },
            { L_,       "QQQQQQQQQQ" },
            { L_,       "abcdABCDEFG" },
            { L_,       "FDLALAabADSF" },

            { L_,       "GSRWSASDEFDSA" },
            { L_,       "PKJHALKGabASDA" },
            { L_,       "XZSAEWRPOIJLKaAS" },
            { L_,       "GALKacKASJDKSWEIO" },

            { L_,       "ABCDEFGHIJKLMNOPQRSTWXYZabcd" },
        };
        static const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

        if (veryVerbose) { bsl::cout << "Copy construct empty objects"
                                     << bsl::endl; }
        {
            bslma_TestAllocator alloc1(veryVeryVerbose);

            Obj mX(&alloc1); const Obj& X = mX;

            {
                bslma_TestAllocator alloc(veryVeryVerbose);
                bslma_DefaultAllocatorGuard guard(&alloc);
                const int BYTES = alloc1.numBytesInUse();
                Obj mA(mX); const Obj& A = mA;
                Obj mB(mX, &alloc); const Obj& B = mB;
                Obj mC(X.item()); const Obj& C = mC;
                Obj mD(X.item(), &alloc); const Obj& D = mD;

                ASSERT(X == A);
                ASSERT(X == B);
                ASSERT(X == C);
                ASSERT(X == D);
                ASSERT(BYTES == alloc1.numBytesInUse());
            }

            for (int i = 0; i < 4; ++i) {

                AggOption::AllocationStrategy mode =
                                              (AggOption::AllocationStrategy)i;

                bslma_TestAllocator testAllocator(veryVeryVerbose);

                bdema_SequentialAllocator seqAlloc(
                                       &bslma_NewDeleteAllocator::singleton());

                bslma_Allocator *alloc;
                if (AggOption::BDEM_SUBORDINATE == mode) {
                    alloc = &seqAlloc;
                }
                else {
                    alloc = &testAllocator;
                }

                bslma_DefaultAllocatorGuard guard(alloc);

                const int BYTES = alloc1.numBytesInUse();
                Obj mA(mX, mode);               const Obj& A = mA;
                Obj mB(mX, mode, alloc);        const Obj& B = mB;
                Obj mC(X.item(), mode);         const Obj& C = mC;
                Obj mD(X.item(), mode, alloc);  const Obj& D = mD;

                ASSERT(X == A);
                ASSERT(X == B);
                ASSERT(X == C);
                ASSERT(X == D);
                ASSERT(BYTES == alloc1.numBytesInUse());
            }
        }

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE  = DATA[i].d_line;
            const char *SPEC  = DATA[i].d_catalogSpec;
            const int   LEN   = bsl::strlen(SPEC);

            const TypesCatalog CATALOG = ggCatalog(SPEC);

            if (veryVerbose) { P(SPEC); }

            for (int j = 0; j < LEN; ++j) {
                const char S = SPEC[j];
                CERef VAL_A = getConstERefA(S);
                CERef VAL_B = getConstERefB(S);

                bslma_TestAllocator origAlloc(veryVeryVerbose);
                Obj mX(CATALOG, &origAlloc); const Obj& X = mX;
                Obj mY(CATALOG, &origAlloc); const Obj& Y = mY;

                mX.makeSelection(j).replaceValue(VAL_A);
                mY.makeSelection(j).replaceValue(VAL_A);
                LOOP_ASSERT(LINE, X == Y);

                // Test copy construction using default allocation strategy.
                {
                    bslma_TestAllocator testAllocator(veryVeryVerbose);
                    bslma_DefaultAllocatorGuard guard(&testAllocator);

                  BEGIN_BSLMA_EXCEPTION_TEST {
                    const int BYTES = origAlloc.numBytesInUse();
                    Obj mA(X); const Obj& A = mA;
                    Obj mB(X, &testAllocator); const Obj& B = mB;
                    Obj mC(X.item()); const Obj& C = mC;
                    Obj mD(X.item(), &testAllocator); const Obj& D = mD;
                    LOOP_ASSERT(LINE, X == A);
                    LOOP_ASSERT(LINE, X == B);
                    LOOP_ASSERT(LINE, X == C);
                    LOOP_ASSERT(LINE, X == D);
                    LOOP3_ASSERT(LINE, X, Y, X == Y);
                    LOOP_ASSERT(LINE, BYTES == origAlloc.numBytesInUse());

                    testAllocator.setAllocationLimit(-1);
                    mX.makeSelection(j).replaceValue(VAL_B);
                    LOOP_ASSERT(LINE, X != A);
                    LOOP_ASSERT(LINE, X != B);
                    LOOP_ASSERT(LINE, X != C);
                    LOOP_ASSERT(LINE, X != D);
                    LOOP_ASSERT(LINE, Y == A);
                    LOOP_ASSERT(LINE, Y == B);
                    LOOP_ASSERT(LINE, Y == C);
                    LOOP_ASSERT(LINE, Y == D);

                    mX.makeSelection(j).replaceValue(VAL_A);
                    LOOP_ASSERT(LINE, X == A);
                    LOOP_ASSERT(LINE, X == B);
                    LOOP_ASSERT(LINE, X == C);
                    LOOP_ASSERT(LINE, X == D);
                    LOOP_ASSERT(LINE, X == Y);

                    mA.makeSelection(j).replaceValue(VAL_B);
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, X != A);

                    mB.makeSelection(j).replaceValue(VAL_B);
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, X != B);

                    mC.makeSelection(j).replaceValue(VAL_B);
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, X != C);

                    mD.makeSelection(j).replaceValue(VAL_B);
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, X != D);

                    mA.makeSelection(j).replaceValue(VAL_A);
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, X == A);

                    mB.makeSelection(j).replaceValue(VAL_A);
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, X == B);

                    mC.makeSelection(j).replaceValue(VAL_A);
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, X == C);

                    mD.makeSelection(j).replaceValue(VAL_A);
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, X == D);

                    mA.selection().makeNull();
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, X != A);

                    mB.selection().makeNull();
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, X != B);

                    mC.selection().makeNull();
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, X != C);

                    mD.selection().makeNull();
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, X != D);

                    mA.makeSelection(j).replaceValue(VAL_A);
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, X == A);

                    mB.makeSelection(j).replaceValue(VAL_A);
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, X == B);

                    mC.makeSelection(j).replaceValue(VAL_A);
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, X == C);

                    mD.makeSelection(j).replaceValue(VAL_A);
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, X == D);

                    mA.makeSelection(-1);
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, X != A);

                    mB.makeSelection(-1);
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, X != B);

                    mC.makeSelection(-1);
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, X != C);

                    mD.makeSelection(-1);
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, X != D);
                  } END_BSLMA_EXCEPTION_TEST
                }

                // Test copy construction using custom allocation strategies.

                for (int k = 0; k < 4; ++k) {

                    AggOption::AllocationStrategy mode =
                                              (AggOption::AllocationStrategy)k;

                    bslma_TestAllocator testAllocator(veryVeryVerbose);

                  BEGIN_BSLMA_EXCEPTION_TEST {

                    bdema_SequentialAllocator seqAlloc(
                                       &bslma_NewDeleteAllocator::singleton());

                    bslma_Allocator *alloc;
                    if (AggOption::BDEM_SUBORDINATE == mode) {
                        alloc = &seqAlloc;
                    }
                    else {
                        alloc = &testAllocator;
                    }

                    bslma_DefaultAllocatorGuard guard(alloc);

                    const int BYTES = origAlloc.numBytesInUse();
                    Obj mA(mX, mode);              const Obj& A = mA;
                    Obj mB(mX, mode, alloc);       const Obj& B = mB;
                    Obj mC(X.item(), mode);        const Obj& C = mC;
                    Obj mD(X.item(), mode, alloc); const Obj& D = mD;
                    LOOP_ASSERT(LINE, X == A);
                    LOOP_ASSERT(LINE, X == B);
                    LOOP_ASSERT(LINE, X == C);
                    LOOP_ASSERT(LINE, X == D);
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, BYTES == origAlloc.numBytesInUse());

                    testAllocator.setAllocationLimit(-1);
                    mX.makeSelection(j).replaceValue(VAL_B);
                    LOOP_ASSERT(LINE, X != A);
                    LOOP_ASSERT(LINE, X != B);
                    LOOP_ASSERT(LINE, X != C);
                    LOOP_ASSERT(LINE, X != D);
                    LOOP_ASSERT(LINE, Y == A);
                    LOOP_ASSERT(LINE, Y == B);
                    LOOP_ASSERT(LINE, Y == C);
                    LOOP_ASSERT(LINE, Y == D);

                    mX.makeSelection(j).replaceValue(VAL_A);
                    LOOP_ASSERT(LINE, X == A);
                    LOOP_ASSERT(LINE, X == B);
                    LOOP_ASSERT(LINE, X == C);
                    LOOP_ASSERT(LINE, X == D);
                    LOOP_ASSERT(LINE, X == Y);

                    mA.makeSelection(j).replaceValue(VAL_B);
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, X != A);

                    mB.makeSelection(j).replaceValue(VAL_B);
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, X != B);

                    mC.makeSelection(j).replaceValue(VAL_B);
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, X != C);

                    mD.makeSelection(j).replaceValue(VAL_B);
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, X != D);

                    mA.makeSelection(j).replaceValue(VAL_A);
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, X == A);

                    mB.makeSelection(j).replaceValue(VAL_A);
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, X == B);

                    mC.makeSelection(j).replaceValue(VAL_A);
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, X == C);

                    mD.makeSelection(j).replaceValue(VAL_A);
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, X == D);

                    mA.selection().makeNull();
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, X != A);

                    mB.selection().makeNull();
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, X != B);

                    mC.selection().makeNull();
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, X != C);

                    mD.selection().makeNull();
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, X != D);

                    mA.makeSelection(j).replaceValue(VAL_A);
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, X == A);

                    mB.makeSelection(j).replaceValue(VAL_A);
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, X == B);

                    mC.makeSelection(j).replaceValue(VAL_A);
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, X == C);

                    mD.makeSelection(j).replaceValue(VAL_A);
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, X == D);

                    mA.makeSelection(-1);
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, X != A);

                    mB.makeSelection(-1);
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, X != B);

                    mC.makeSelection(-1);
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, X != C);

                    mD.makeSelection(-1);
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, X != D);

                 } END_BSLMA_EXCEPTION_TEST
               }
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS:
        //
        // Concerns:
        //   Any subtle variation in value must be detected by the equality
        //   operators.  The allocator used to construct the object is not
        //   part of the value and does not affect the results of the equality
        //   tests.
        //
        // Plan:
        //   Specify a set S of unique object values having various minor or
        //   subtle differences.  Verify the correctness of 'operator==' and
        //   'operator!=' using all elements (u, v) of the cross product
        //    S X S.
        //
        // Testing:
        //   operator==(const bdem_Choice&, const bdem_Choice&);
        //   operator!=(const bdem_Choice&, const bdem_Choice&);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Equality Operators"
                               << "\n==========================" << bsl::endl;

        const struct {
            int         d_line;
            const char *d_catalogSpec;  // Specification to create the catalog
        } DATA[] = {
            // Line     DescriptorSpec
            // ====     ==============
            { L_,       "" },
            { L_,       "A" },
            { L_,       "B" },
            { L_,       "C" },
            { L_,       "D" },
            { L_,       "E" },
            { L_,       "F" },
            { L_,       "G" },
            { L_,       "H" },
            { L_,       "I" },
            { L_,       "J" },
            { L_,       "K" },
            { L_,       "L" },
            { L_,       "M" },
            { L_,       "N" },
            { L_,       "O" },
            { L_,       "P" },
            { L_,       "Q" },
            { L_,       "R" },
            { L_,       "S" },
            { L_,       "T" },
            { L_,       "U" },
            { L_,       "V" },
            { L_,       "W" },
            { L_,       "X" },
            { L_,       "Y" },
            { L_,       "Z" },
            { L_,       "a" },
            { L_,       "b" },
            { L_,       "c" },
            { L_,       "d" },
            { L_,       "e" },
            { L_,       "f" },

            // Testing various interesting combinations
            { L_,       "AA" },
            { L_,       "AC" },
            { L_,       "CD" },
            { L_,       "CE" },
            { L_,       "FG" },
            { L_,       "GG" },
            { L_,       "GH" },
            { L_,       "MN" },
            { L_,       "OP" },
            { L_,       "PQ" },
            { L_,       "KQ" },
            { L_,       "ST" },
            { L_,       "WX" },
            { L_,       "YZ" },
            { L_,       "ab" },
            { L_,       "cd" },
            { L_,       "ef" },

            { L_,       "HIJ" },
            { L_,       "KLM" },
            { L_,       "RST" },
            { L_,       "YZa" },

            { L_,       "BLRW" },
            { L_,       "DGNQ" },
            { L_,       "QRST" },

            { L_,       "JHKHSK" },
            { L_,       "RISXLSW" },
            { L_,       "MXKZOLPR" },
            { L_,       "GGGGGGGGG" },
            { L_,       "QQQQQQQQQQ" },
            { L_,       "abcdABCDEFG" },
            { L_,       "FDLALAabADSF" },

            { L_,       "GSRWSASDEFDSA" },
            { L_,       "PKJHALKGabASDA" },
            { L_,       "XZSAEWRPOIJLKaAS" },
            { L_,       "GALKacKASJDKSWEIO" },

            { L_,       "ABCDEFGHIJKLMNOPQRSTWXYZabcd" },
        };
        static const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

        for (int i1 = 0; i1 < NUM_DATA; ++i1) {
            const int   LINE1  = DATA[i1].d_line;
            const char *SPEC1  = DATA[i1].d_catalogSpec;
            const int   LEN1   = bsl::strlen(SPEC1);

            for (int j1 = 0; j1 < LEN1; ++j1) {

                for (int i2 = 0; i2 < NUM_DATA; ++i2) {
                    const int   LINE2 = DATA[i2].d_line;
                    const char *SPEC2 = DATA[i2].d_catalogSpec;
                    const int   LEN2  = bsl::strlen(SPEC2);

                    for (int j2 = 0; j2 < LEN2; ++j2) {

                        const char S1 = SPEC1[j1];
                        const char S2 = SPEC2[j2];

                        const TypesCatalog CATALOG1 = ggCatalog(SPEC1);
                        const TypesCatalog CATALOG2 = ggCatalog(SPEC2);

                        CERef C1_A = getConstERefA(S1);
                        CERef C1_B = getConstERefB(S1);
                        CERef C2_A = getConstERefA(S2);
                        CERef C2_B = getConstERefB(S2);

                        if (veryVerbose) { P(SPEC1); P(SPEC2); }

                        bslma_TestAllocator alloc1(veryVeryVerbose);
                        bslma_TestAllocator alloc2(veryVeryVerbose);

                        Obj mX(CATALOG1, &alloc1); const Obj& X = mX;
                        mX.makeSelection(j1).replaceValue(C1_A);

                        Obj mY(CATALOG2, &alloc2); const Obj& Y = mY;
                        mY.makeSelection(j2).replaceValue(C2_A);

                        if (i1 == i2 && j1 == j2) {
                            // This assumes that each spec is unique

                            LOOP2_ASSERT(LINE1, LINE2,    X == Y);
                            LOOP2_ASSERT(LINE1, LINE2,  !(X != Y));

                            // Some perturbation
                            mY.makeSelection(j2).replaceValue(C2_B);

                            LOOP2_ASSERT(LINE1, LINE2,    X != Y);
                            LOOP2_ASSERT(LINE1, LINE2,  !(X == Y));

                            mY.makeSelection(j2).replaceValue(C2_A);

                            LOOP2_ASSERT(LINE1, LINE2,    X == Y);
                            LOOP2_ASSERT(LINE1, LINE2,  !(X != Y));

                            mX.makeSelection(j1).replaceValue(C1_B);

                            LOOP2_ASSERT(LINE1, LINE2,    X != Y);
                            LOOP2_ASSERT(LINE1, LINE2,  !(X == Y));

                            mX.makeSelection(j1).replaceValue(C1_A);

                            LOOP2_ASSERT(LINE1, LINE2,    X == Y);
                            LOOP2_ASSERT(LINE1, LINE2,  !(X != Y));

                            mY.selection().makeNull();

                            LOOP2_ASSERT(LINE1, LINE2,   X != Y);
                            LOOP2_ASSERT(LINE1, LINE2, !(X == Y));

                            mX.selection().makeNull();

                            LOOP2_ASSERT(LINE1, LINE2,   X == Y);
                            LOOP2_ASSERT(LINE1, LINE2, !(X != Y));

                            mY.makeSelection(-1);

                            LOOP2_ASSERT(LINE1, LINE2,   X != Y);
                            LOOP2_ASSERT(LINE1, LINE2, !(X == Y));

                            mX.makeSelection(-1);

                            LOOP2_ASSERT(LINE1, LINE2,   X == Y);
                            LOOP2_ASSERT(LINE1, LINE2, !(X != Y));
                        }
                        else {
                            LOOP2_ASSERT(LINE1, LINE2,    X != Y);
                            LOOP2_ASSERT(LINE1, LINE2,  !(X == Y));
                        }
                    }
                }
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING print() FUNCTION AND OUTPUT (<<) OPERATOR:
        //
        // Concerns:
        //   The print() function is correctly forwarded to the appropriate
        //   print function for the type stored in the choice object.  The
        //   '<<' operator prints the output on one line.
        //
        // Plan:
        //   For each of a small representative set of object values, use
        //   'ostrstream' to write that object's value to a character buffer
        //   and then compare the contents of that buffer with the expected
        //   output format.
        //
        // Testing:
        //   bsl::ostream& print(bsl::ostream& stream,
        //                       int           level = 0,
        //                       int           spacesPerLevel = 4) const;
        //   operator<<(ostream&, const bdem_Choice&);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl
                          << "Testing Output Functions" << bsl::endl
                          << "========================" << bsl::endl;
        if (veryVerbose) {
                      bsl::cout << "\tTesting empty choice" << bsl::endl; }
        {
            const char *EX_P1 = "    {\n"
                                 "        Selection Types: [\n"
                                 "        ]\n"
                                 "        {\n"
                                 "            VOID NULL\n"
                                 "        }\n"
                                 "    }\n";
            const char *EX_P2 = "{\n"
                                 "        Selection Types: [\n"
                                 "        ]\n"
                                 "        {\n"
                                 "            VOID NULL\n"
                                 "        }\n"
                                 "    }\n";
            const char *EX_P3 = "    { Selection Types: [ ] { VOID NULL } }";
            const char *EX_P4 = "{ Selection Types: [ ] { VOID NULL } }";
            const char *EX_OP = "{ Selection Types: [ ] { VOID NULL } }";

            Obj mX; const Obj& X = mX;

            bsl::ostringstream os1, os2, os3, os4, os5;
            X.print(os1, 1, 4);
            X.print(os2, -1, 4);
            X.print(os3, 1, -4);
            X.print(os4, -1, -4);
            os5 << X;

            LOOP2_ASSERT(EX_P1,
                         os1.str(),
                         0 == bsl::strcmp(EX_P1, os1.str().c_str()));
            LOOP2_ASSERT(EX_P2,
                         os2.str(),
                         0 == bsl::strcmp(EX_P2, os2.str().c_str()));
            LOOP2_ASSERT(EX_P3,
                         os3.str(),
                         0 == bsl::strcmp(EX_P3, os3.str().c_str()));
            LOOP2_ASSERT(EX_P4,
                         os4.str(),
                         0 == bsl::strcmp(EX_P4, os4.str().c_str()));
            LOOP2_ASSERT(EX_OP,
                         os5.str(),
                         0 == bsl::strcmp(EX_OP, os5.str().c_str()));
        }

        if (veryVerbose) {
            bsl::cout << "\tTesting empty (NULL) choice" << bsl::endl; }
        {
            const char *EX_P1 = "    {\n"
                                 "        Selection Types: [\n"
                                 "        ]\n"
                                 "        {\n"
                                 "            VOID NULL\n"
                                 "        }\n"
                                 "    }\n";
            const char *EX_P2 = "{\n"
                                 "        Selection Types: [\n"
                                 "        ]\n"
                                 "        {\n"
                                 "            VOID NULL\n"
                                 "        }\n"
                                 "    }\n";
            const char *EX_P3 =
                "    { Selection Types: [ ] { VOID NULL } }";
            const char *EX_P4 =
                "{ Selection Types: [ ] { VOID NULL } }";
            const char *EX_OP =
                "{ Selection Types: [ ] { VOID NULL } }";

            Obj mX; const Obj& X = mX;
            mX.selection().makeNull();

            bsl::ostringstream os1, os2, os3, os4, os5;
            X.print(os1, 1, 4);
            X.print(os2, -1, 4);
            X.print(os3, 1, -4);
            X.print(os4, -1, -4);
            os5 << X;

            LOOP2_ASSERT(EX_P1,
                         os1.str(),
                         0 == bsl::strcmp(EX_P1, os1.str().c_str()));
            LOOP2_ASSERT(EX_P2,
                         os2.str(),
                         0 == bsl::strcmp(EX_P2, os2.str().c_str()));
            LOOP2_ASSERT(EX_P3,
                         os3.str(),
                         0 == bsl::strcmp(EX_P3, os3.str().c_str()));
            LOOP2_ASSERT(EX_P4,
                         os4.str(),
                         0 == bsl::strcmp(EX_P4, os4.str().c_str()));
            LOOP2_ASSERT(EX_OP,
                         os5.str(),
                         0 == bsl::strcmp(EX_OP, os5.str().c_str()));
        }

        const int MAX_PRINT_TESTS = 4;
        const struct {
            int         d_line;       // Line number
            const char  d_spec;       // Type Specification string
            char        d_valueSpec;  // Spec specifying the value for that
                                      // type.  Can be one of 'A', 'B' or 'N'
                                      // corresponding to the A, B or N value
                                      // for that type.
            struct {
                int         d_level;      // Level to print at
                int         d_spacesPerLevel;  // spaces per level to print at
                const char *d_printExpOutput;  // Expected o/p from print
                const char *d_printExpOutputN; // Expected o/p from print
                                               // (null)
            } PRINT_TESTS[MAX_PRINT_TESTS];
            const char *d_outputOpExpOutput;  // Expected o/p from operator<<
            const char *d_outputOpExpOutputN; // Expected o/p from operator<<
                                              // (null)
        } DATA[] = {
            {
                L_,
                'A',
                'A',
                {
                    {
                        1,
                        4,
                        "    {\n"
                        "        Selection Types: [\n"
                        "            CHAR\n"
                        "        ]\n"
                        "        {\n"
                        "            CHAR A\n"
                        "        }\n"
                        "    }\n",
                        "    {\n"
                        "        Selection Types: [\n"
                        "            CHAR\n"
                        "        ]\n"
                        "        {\n"
                        "            CHAR NULL\n"
                        "        }\n"
                        "    }\n",
                    },
                    {
                       -1,
                        4,
                        "{\n"
                        "        Selection Types: [\n"
                        "            CHAR\n"
                        "        ]\n"
                        "        {\n"
                        "            CHAR A\n"
                        "        }\n"
                        "    }\n",
                        "{\n"
                        "        Selection Types: [\n"
                        "            CHAR\n"
                        "        ]\n"
                        "        {\n"
                        "            CHAR NULL\n"
                        "        }\n"
                        "    }\n",
                    },
                    {
                        1,
                        -4,
                        "    { Selection Types: [ CHAR ] { CHAR A } }",
                        "    { Selection Types: [ CHAR ] { CHAR NULL } }",
                    },
                    {
                        -1,
                        -4,
                        "{ Selection Types: [ CHAR ] { CHAR A } }",
                        "{ Selection Types: [ CHAR ] { CHAR NULL } }",
                    },
                },
                "{ Selection Types: [ CHAR ] { CHAR A } }",
                "{ Selection Types: [ CHAR ] { CHAR NULL } }",
            },

            {
                L_,
                'C',
                'A',
                {
                    {
                        1,
                        4,
                        "    {\n"
                        "        Selection Types: [\n"
                        "            INT\n"
                        "        ]\n"
                        "        {\n"
                        "            INT 10\n"
                        "        }\n"
                        "    }\n",
                        "    {\n"
                        "        Selection Types: [\n"
                        "            INT\n"
                        "        ]\n"
                        "        {\n"
                        "            INT NULL\n"
                        "        }\n"
                        "    }\n",
                    },
                    {
                       -1,
                        4,
                        "{\n"
                        "        Selection Types: [\n"
                        "            INT\n"
                        "        ]\n"
                        "        {\n"
                        "            INT 10\n"
                        "        }\n"
                        "    }\n",
                        "{\n"
                        "        Selection Types: [\n"
                        "            INT\n"
                        "        ]\n"
                        "        {\n"
                        "            INT NULL\n"
                        "        }\n"
                        "    }\n",
                    },
                    {
                        1,
                        -4,
                        "    { Selection Types: [ INT ] { INT 10 } }",
                        "    { Selection Types: [ INT ] { INT NULL } }",
                    },
                    {
                        -1,
                        -4,
                        "{ Selection Types: [ INT ] { INT 10 } }",
                        "{ Selection Types: [ INT ] { INT NULL } }",
                    },
                },
                "{ Selection Types: [ INT ] { INT 10 } }",
                "{ Selection Types: [ INT ] { INT NULL } }",
            },

            {
                L_,
                'F',
                'A',
                {
                    {
                        1,
                        4,
                        "    {\n"
                        "        Selection Types: [\n"
                        "            DOUBLE\n"
                        "        ]\n"
                        "        {\n"
                        "            DOUBLE 10.5\n"
                        "        }\n"
                        "    }\n",
                        "    {\n"
                        "        Selection Types: [\n"
                        "            DOUBLE\n"
                        "        ]\n"
                        "        {\n"
                        "            DOUBLE NULL\n"
                        "        }\n"
                        "    }\n",
                    },
                    {
                       -1,
                        4,
                        "{\n"
                        "        Selection Types: [\n"
                        "            DOUBLE\n"
                        "        ]\n"
                        "        {\n"
                        "            DOUBLE 10.5\n"
                        "        }\n"
                        "    }\n",
                        "{\n"
                        "        Selection Types: [\n"
                        "            DOUBLE\n"
                        "        ]\n"
                        "        {\n"
                        "            DOUBLE NULL\n"
                        "        }\n"
                        "    }\n",
                    },
                    {
                        1,
                        -4,
                        "    { Selection Types: [ DOUBLE ] { DOUBLE 10.5 } }",
                        "    { Selection Types: [ DOUBLE ] { DOUBLE NULL } }",
                    },
                    {
                        -1,
                        -4,
                        "{ Selection Types: [ DOUBLE ] { DOUBLE 10.5 } }",
                        "{ Selection Types: [ DOUBLE ] { DOUBLE NULL } }",
                    },
                },
                "{ Selection Types: [ DOUBLE ] { DOUBLE 10.5 } }",
                "{ Selection Types: [ DOUBLE ] { DOUBLE NULL } }",
            },

            {
                L_,
                'G',
                'A',
                {
                    {
                        1,
                        4,
                        "    {\n"
                        "        Selection Types: [\n"
                        "            STRING\n"
                        "        ]\n"
                        "        {\n"
                        "            STRING one\n"
                        "        }\n"
                        "    }\n",
                        "    {\n"
                        "        Selection Types: [\n"
                        "            STRING\n"
                        "        ]\n"
                        "        {\n"
                        "            STRING NULL\n"
                        "        }\n"
                        "    }\n",
                    },
                    {
                       -1,
                        4,
                        "{\n"
                        "        Selection Types: [\n"
                        "            STRING\n"
                        "        ]\n"
                        "        {\n"
                        "            STRING one\n"
                        "        }\n"
                        "    }\n",
                        "{\n"
                        "        Selection Types: [\n"
                        "            STRING\n"
                        "        ]\n"
                        "        {\n"
                        "            STRING NULL\n"
                        "        }\n"
                        "    }\n",
                    },
                    {
                        1,
                        -4,
                        "    { Selection Types: [ STRING ] { STRING one } }",
                        "    { Selection Types: [ STRING ] { STRING NULL } }",
                    },
                    {
                        -1,
                        -4,
                        "{ Selection Types: [ STRING ] { STRING one } }",
                        "{ Selection Types: [ STRING ] { STRING NULL } }",
                    },
                },
                "{ Selection Types: [ STRING ] { STRING one } }",
                "{ Selection Types: [ STRING ] { STRING NULL } }",
            },

            {
                L_,
                'H',
                'A',
                {
                    {
                        1,
                        4,
                        "    {\n"
                        "        Selection Types: [\n"
                        "            DATETIME\n"
                        "        ]\n"
                        "        {\n"
                        "            DATETIME 01JAN2000_00:01:02.003\n"
                        "        }\n"
                        "    }\n",
                        "    {\n"
                        "        Selection Types: [\n"
                        "            DATETIME\n"
                        "        ]\n"
                        "        {\n"
                        "            DATETIME NULL\n"
                        "        }\n"
                        "    }\n",
                    },
                    {
                       -1,
                        4,
                        "{\n"
                        "        Selection Types: [\n"
                        "            DATETIME\n"
                        "        ]\n"
                        "        {\n"
                        "            DATETIME 01JAN2000_00:01:02.003\n"
                        "        }\n"
                        "    }\n",
                        "{\n"
                        "        Selection Types: [\n"
                        "            DATETIME\n"
                        "        ]\n"
                        "        {\n"
                        "            DATETIME NULL\n"
                        "        }\n"
                        "    }\n",
                    },
                    {
                        1,
                        -4,
                        "    { Selection Types: [ DATETIME ] "
                        "{ DATETIME 01JAN2000_00:01:02.003 } }",
                        "    { Selection Types: [ DATETIME ] "
                        "{ DATETIME NULL } }",
                    },
                    {
                        -1,
                        -4,
                        "{ Selection Types: [ DATETIME ] "
                        "{ DATETIME 01JAN2000_00:01:02.003 } }",
                        "{ Selection Types: [ DATETIME ] "
                        "{ DATETIME NULL } }",
                    },
                },
                "{ Selection Types: [ DATETIME ] "
                "{ DATETIME 01JAN2000_00:01:02.003 } }",
                "{ Selection Types: [ DATETIME ] "
                "{ DATETIME NULL } }",
            },

            {
                L_,
                'M',
                'A',
                {
                    {
                        1,
                        4,
                        "    {\n"
                        "        Selection Types: [\n"
                        "            INT_ARRAY\n"
                        "        ]\n"
                        "        {\n"
                        "            INT_ARRAY [\n"
                        "                10\n"
                        "            ]\n"
                        "        }\n"
                        "    }\n",
                        "    {\n"
                        "        Selection Types: [\n"
                        "            INT_ARRAY\n"
                        "        ]\n"
                        "        {\n"
                        "            INT_ARRAY NULL\n"
                        "        }\n"
                        "    }\n",
                    },
                    {
                       -1,
                        4,
                        "{\n"
                        "        Selection Types: [\n"
                        "            INT_ARRAY\n"
                        "        ]\n"
                        "        {\n"
                        "            INT_ARRAY [\n"
                        "                10\n"
                        "            ]\n"
                        "        }\n"
                        "    }\n",
                        "{\n"
                        "        Selection Types: [\n"
                        "            INT_ARRAY\n"
                        "        ]\n"
                        "        {\n"
                        "            INT_ARRAY NULL\n"
                        "        }\n"
                        "    }\n",
                    },
                    {
                        1,
                        -4,
                        "    { Selection Types: [ INT_ARRAY ] "
                        "{ INT_ARRAY [ 10 ] } }",
                        "    { Selection Types: [ INT_ARRAY ] "
                        "{ INT_ARRAY NULL } }",
                    },
                    {
                        -1,
                        -4,
                        "{ Selection Types: [ INT_ARRAY ] "
                        "{ INT_ARRAY [ 10 ] } }",
                        "{ Selection Types: [ INT_ARRAY ] "
                        "{ INT_ARRAY NULL } }",
                    },
                },
                "{ Selection Types: [ INT_ARRAY ] { INT_ARRAY [ 10 ] } }",
                "{ Selection Types: [ INT_ARRAY ] { INT_ARRAY NULL } }",
            },

            {
                L_,
                'Q',
                'A',
                {
                    {
                        1,
                        4,
                        "    {\n"
                        "        Selection Types: [\n"
                        "            STRING_ARRAY\n"
                        "        ]\n"
                        "        {\n"
                        "            STRING_ARRAY [\n"
                        "                one\n"
                        "            ]\n"
                        "        }\n"
                        "    }\n",
                        "    {\n"
                        "        Selection Types: [\n"
                        "            STRING_ARRAY\n"
                        "        ]\n"
                        "        {\n"
                        "            STRING_ARRAY NULL\n"
                        "        }\n"
                        "    }\n",
                    },
                    {
                       -1,
                        4,
                        "{\n"
                        "        Selection Types: [\n"
                        "            STRING_ARRAY\n"
                        "        ]\n"
                        "        {\n"
                        "            STRING_ARRAY [\n"
                        "                one\n"
                        "            ]\n"
                        "        }\n"
                        "    }\n",
                        "{\n"
                        "        Selection Types: [\n"
                        "            STRING_ARRAY\n"
                        "        ]\n"
                        "        {\n"
                        "            STRING_ARRAY NULL\n"
                        "        }\n"
                        "    }\n",
                    },
                    {
                        1,
                        -4,
                        "    { Selection Types: [ STRING_ARRAY ] "
                        "{ STRING_ARRAY [ one ] } }",
                        "    { Selection Types: [ STRING_ARRAY ] "
                        "{ STRING_ARRAY NULL } }",
                    },
                    {
                        -1,
                        -4,
                        "{ Selection Types: [ STRING_ARRAY ] "
                        "{ STRING_ARRAY [ one ] } }",
                        "{ Selection Types: [ STRING_ARRAY ] "
                        "{ STRING_ARRAY NULL } }",
                    },
                },
                "{ Selection Types: [ STRING_ARRAY ] "
                "{ STRING_ARRAY [ one ] } }",
                "{ Selection Types: [ STRING_ARRAY ] "
                "{ STRING_ARRAY NULL } }",
            },

            {
                L_,
                'U',
                'A',
                {
                    {
                        1,
                        4,
                        "    {\n"
                        "        Selection Types: [\n"
                        "            LIST\n"
                        "        ]\n"
                        "        {\n"
                        "            LIST {\n"
                        "                INT 10\n"
                        "                DOUBLE 10.5\n"
                        "                STRING one\n"
                        "                STRING_ARRAY [\n"
                        "                    one\n"
                        "                ]\n"
                        "            }\n"
                        "        }\n"
                        "    }\n",
                        "    {\n"
                        "        Selection Types: [\n"
                        "            LIST\n"
                        "        ]\n"
                        "        {\n"
                        "            LIST NULL\n"
                        "        }\n"
                        "    }\n",
                    },
                    {
                       -1,
                        4,
                        "{\n"
                        "        Selection Types: [\n"
                        "            LIST\n"
                        "        ]\n"
                        "        {\n"
                        "            LIST {\n"
                        "                INT 10\n"
                        "                DOUBLE 10.5\n"
                        "                STRING one\n"
                        "                STRING_ARRAY [\n"
                        "                    one\n"
                        "                ]\n"
                        "            }\n"
                        "        }\n"
                        "    }\n",
                        "{\n"
                        "        Selection Types: [\n"
                        "            LIST\n"
                        "        ]\n"
                        "        {\n"
                        "            LIST NULL\n"
                        "        }\n"
                        "    }\n",
                    },
                    {
                        1,
                        -4,
                        "    { Selection Types: [ LIST ] "
                        "{ LIST { INT 10 DOUBLE 10.5 STRING one"
                        " STRING_ARRAY [ one ] } } }",
                        "    { Selection Types: [ LIST ] "
                        "{ LIST NULL } }",
                    },
                    {
                        -1,
                        -4,
                        "{ Selection Types: [ LIST ] "
                        "{ LIST { INT 10 DOUBLE 10.5 STRING one"
                        " STRING_ARRAY [ one ] } } }",
                        "{ Selection Types: [ LIST ] "
                        "{ LIST NULL } }",
                    },
                },
                "{ Selection Types: [ LIST ] "
                "{ LIST { INT 10 DOUBLE 10.5 STRING one"
                " STRING_ARRAY [ one ] } } }",
                "{ Selection Types: [ LIST ] "
                "{ LIST NULL } }",
            },

            {
                L_,
                'V',
                'A',
                {
                    {
                        1,
                        4,
                        "    {\n"
                        "        Selection Types: [\n"
                        "            TABLE\n"
                        "        ]\n"
                        "        {\n"
                        "            TABLE {\n"
                        "                Column Types: [\n"
                        "                    INT\n"
                        "                    DOUBLE\n"
                        "                    STRING\n"
                        "                    STRING_ARRAY\n"
                        "                ]\n"
                        "                Row 0: {\n"
                        "                    10\n"
                        "                    10.5\n"
                        "                    one\n"
                        "                    [\n"
                        "                        one\n"
                        "                    ]\n"
                        "                }\n"
                        "            }\n"
                        "        }\n"
                        "    }\n",
                        "    {\n"
                        "        Selection Types: [\n"
                        "            TABLE\n"
                        "        ]\n"
                        "        {\n"
                        "            TABLE NULL\n"
                        "        }\n"
                        "    }\n",
                    },
                    {
                       -1,
                        4,
                        "{\n"
                        "        Selection Types: [\n"
                        "            TABLE\n"
                        "        ]\n"
                        "        {\n"
                        "            TABLE {\n"
                        "                Column Types: [\n"
                        "                    INT\n"
                        "                    DOUBLE\n"
                        "                    STRING\n"
                        "                    STRING_ARRAY\n"
                        "                ]\n"
                        "                Row 0: {\n"
                        "                    10\n"
                        "                    10.5\n"
                        "                    one\n"
                        "                    [\n"
                        "                        one\n"
                        "                    ]\n"
                        "                }\n"
                        "            }\n"
                        "        }\n"
                        "    }\n",
                        "{\n"
                        "        Selection Types: [\n"
                        "            TABLE\n"
                        "        ]\n"
                        "        {\n"
                        "            TABLE NULL\n"
                        "        }\n"
                        "    }\n",
                    },
                    {
                        1,
                        -4,
                        "    { Selection Types: [ TABLE ] { TABLE {"
                        " Column Types: [ INT DOUBLE STRING STRING_ARRAY ]"
                        " Row 0: { 10 10.5 one [ one ] } } } }",
                        "    { Selection Types: [ TABLE ] { TABLE NULL } }",
                    },
                    {
                        -1,
                        -4,
                        "{ Selection Types: [ TABLE ] { TABLE {"
                        " Column Types: [ INT DOUBLE STRING STRING_ARRAY ]"
                        " Row 0: { 10 10.5 one [ one ] } } } }",
                        "{ Selection Types: [ TABLE ] { TABLE NULL } }",
                    },
                },
                "{ Selection Types: [ TABLE ] { TABLE {"
                " Column Types: [ INT DOUBLE STRING STRING_ARRAY ]"
                " Row 0: { 10 10.5 one [ one ] } } } }",
                "{ Selection Types: [ TABLE ] { TABLE NULL } }",
            },

            {
                L_,
                'e',
                'A',
                {
                    {
                        1,
                        4,
                        "    {\n"
                        "        Selection Types: [\n"
                        "            CHOICE\n"
                        "        ]\n"
                        "        {\n"
                        "            CHOICE {\n"
                        "                Selection Types: [\n"
                        "                    INT\n"
                        "                    DOUBLE\n"
                        "                    STRING\n"
                        "                    STRING_ARRAY\n"
                        "                ]\n"
                        "                {\n"
                        "                    STRING one\n"
                        "                }\n"
                        "            }\n"
                        "        }\n"
                        "    }\n",
                        "    {\n"
                        "        Selection Types: [\n"
                        "            CHOICE\n"
                        "        ]\n"
                        "        {\n"
                        "            CHOICE NULL\n"
                        "        }\n"
                        "    }\n",
                    },
                    {
                       -1,
                        4,
                        "{\n"
                        "        Selection Types: [\n"
                        "            CHOICE\n"
                        "        ]\n"
                        "        {\n"
                        "            CHOICE {\n"
                        "                Selection Types: [\n"
                        "                    INT\n"
                        "                    DOUBLE\n"
                        "                    STRING\n"
                        "                    STRING_ARRAY\n"
                        "                ]\n"
                        "                {\n"
                        "                    STRING one\n"
                        "                }\n"
                        "            }\n"
                        "        }\n"
                        "    }\n",
                        "{\n"
                        "        Selection Types: [\n"
                        "            CHOICE\n"
                        "        ]\n"
                        "        {\n"
                        "            CHOICE NULL\n"
                        "        }\n"
                        "    }\n",
                    },
                    {
                        1,
                        -4,
                        "    { Selection Types: [ CHOICE ] { CHOICE {"
                        " Selection Types: [ INT DOUBLE STRING STRING_ARRAY ]"
                        " { STRING one } } } }",
                        "    { Selection Types: [ CHOICE ] { CHOICE NULL } }",
                    },
                    {
                        -1,
                        -4,
                        "{ Selection Types: [ CHOICE ] { CHOICE {"
                        " Selection Types: [ INT DOUBLE STRING STRING_ARRAY ]"
                        " { STRING one } } } }",
                        "{ Selection Types: [ CHOICE ] { CHOICE NULL } }",
                    },
                },
                "{ Selection Types: [ CHOICE ] { CHOICE {"
                " Selection Types: [ INT DOUBLE STRING STRING_ARRAY ]"
                " { STRING one } } } }",
                "{ Selection Types: [ CHOICE ] { CHOICE NULL } }",
            },

            {
                L_,
                'f',
                'A',
                {
                    {
                        1,
                        4,
                        "    {\n"
                        "        Selection Types: [\n"
                        "            CHOICE_ARRAY\n"
                        "        ]\n"
                        "        {\n"
                        "            CHOICE_ARRAY {\n"
                        "                Selection Types: [\n"
                        "                    INT\n"
                        "                    DOUBLE\n"
                        "                    STRING\n"
                        "                    STRING_ARRAY\n"
                        "                ]\n"
                        "                Item 0: {\n"
                        "                    INT 10\n"
                        "                }\n"
                        "                Item 1: {\n"
                        "                    DOUBLE 10.5\n"
                        "                }\n"
                        "                Item 2: {\n"
                        "                    STRING one\n"
                        "                }\n"
                        "                Item 3: {\n"
                        "                    STRING_ARRAY [\n"
                        "                        one\n"
                        "                    ]\n"
                        "                }\n"
                        "            }\n"
                        "        }\n"
                        "    }\n",
                        "    {\n"
                        "        Selection Types: [\n"
                        "            CHOICE_ARRAY\n"
                        "        ]\n"
                        "        {\n"
                        "            CHOICE_ARRAY NULL\n"
                        "        }\n"
                        "    }\n",
                    },
                    {
                       -1,
                        4,
                        "{\n"
                        "        Selection Types: [\n"
                        "            CHOICE_ARRAY\n"
                        "        ]\n"
                        "        {\n"
                        "            CHOICE_ARRAY {\n"
                        "                Selection Types: [\n"
                        "                    INT\n"
                        "                    DOUBLE\n"
                        "                    STRING\n"
                        "                    STRING_ARRAY\n"
                        "                ]\n"
                        "                Item 0: {\n"
                        "                    INT 10\n"
                        "                }\n"
                        "                Item 1: {\n"
                        "                    DOUBLE 10.5\n"
                        "                }\n"
                        "                Item 2: {\n"
                        "                    STRING one\n"
                        "                }\n"
                        "                Item 3: {\n"
                        "                    STRING_ARRAY [\n"
                        "                        one\n"
                        "                    ]\n"
                        "                }\n"
                        "            }\n"
                        "        }\n"
                        "    }\n",
                        "{\n"
                        "        Selection Types: [\n"
                        "            CHOICE_ARRAY\n"
                        "        ]\n"
                        "        {\n"
                        "            CHOICE_ARRAY NULL\n"
                        "        }\n"
                        "    }\n",
                    },
                    {
                        1,
                        -4,
                        "    { Selection Types: [ CHOICE_ARRAY ]"
                        " { CHOICE_ARRAY { Selection Types: "
                        "[ INT DOUBLE STRING STRING_ARRAY ]"
                        " Item 0: { INT 10 } Item 1: { DOUBLE 10.5 }"
                        " Item 2: { STRING one } Item 3: { STRING_ARRAY "
                        "[ one ] } } } }",
                        "    { Selection Types: [ CHOICE_ARRAY ]"
                        " { CHOICE_ARRAY NULL } }",
                    },
                    {
                        -1,
                        -4,
                        "{ Selection Types: [ CHOICE_ARRAY ] { CHOICE_ARRAY {"
                        " Selection Types: [ INT DOUBLE STRING STRING_ARRAY ]"
                        " Item 0: { INT 10 } Item 1: { DOUBLE 10.5 }"
                        " Item 2: { STRING one } Item 3: { STRING_ARRAY "
                        "[ one ] } } } }",
                        "{ Selection Types: [ CHOICE_ARRAY ] { "
                        "CHOICE_ARRAY NULL } }",
                    },
                },
                "{ Selection Types: [ CHOICE_ARRAY ] { CHOICE_ARRAY {"
                " Selection Types: [ INT DOUBLE STRING STRING_ARRAY ]"
                " Item 0: { INT 10 } Item 1: { DOUBLE 10.5 }"
                " Item 2: { STRING one } Item 3: { STRING_ARRAY "
                "[ one ] } } } }",
                "{ Selection Types: [ CHOICE_ARRAY ] { "
                "CHOICE_ARRAY NULL } }",
            },
        };
        const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE      = DATA[i].d_line;
            const char  SPEC      = DATA[i].d_spec;
            const char  VAL_SPEC  = DATA[i].d_valueSpec;
            const char *OP_EXP    = DATA[i].d_outputOpExpOutput;
            const char *OP_EXPN   = DATA[i].d_outputOpExpOutputN;
            const Desc *DESC      = getDescriptor(SPEC);
            EType::Type TYPE      = getElemType(SPEC);

            const void *VALUE;
            if ('A' == VAL_SPEC) {
                VALUE = getValueA(SPEC);
            }
            else if ('B' == VAL_SPEC) {
                VALUE = getValueB(SPEC);
            }
            else if ('N' == VAL_SPEC) {
                VALUE = getValueN(SPEC);
            }
            else {
                ASSERT(0);
            }
            const CERef VALUE_REF(VALUE, DESC);

            bslma_TestAllocator alloc1(veryVeryVerbose);

            Obj mX(&TYPE, 1, &alloc1); const Obj& X = mX;
            mX.makeSelection(0).replaceValue(VALUE_REF);

            bsl::ostringstream os;
            os << X;
            LOOP2_ASSERT(OP_EXP,
                         os.str(),
                         0 == bsl::strcmp(OP_EXP, os.str().c_str()));

            for (int j = 0; j < MAX_PRINT_TESTS; ++j) {
                const int   LEVEL     = DATA[i].PRINT_TESTS[j].d_level;
                const int   SPL       =
                                      DATA[i].PRINT_TESTS[j].d_spacesPerLevel;
                const char *PRINT_EXP =
                                      DATA[i].PRINT_TESTS[j].d_printExpOutput;

                bsl::ostringstream os1;
                X.print(os1, LEVEL, SPL);
                LOOP2_ASSERT(PRINT_EXP,
                             os1.str(),
                             0 == bsl::strcmp(PRINT_EXP, os1.str().c_str()));
            }

            mX.selection().makeNull();

            bsl::ostringstream osn;
            osn << X;
            LOOP3_ASSERT(LINE, OP_EXPN,
                         osn.str(),
                         0 == bsl::strcmp(OP_EXPN, osn.str().c_str()));

            for (int j = 0; j < MAX_PRINT_TESTS; ++j) {
                const int   LEVEL     = DATA[i].PRINT_TESTS[j].d_level;
                const int   SPL       =
                                      DATA[i].PRINT_TESTS[j].d_spacesPerLevel;
                const char *PRINT_EXP =
                                      DATA[i].PRINT_TESTS[j].d_printExpOutputN;

                bsl::ostringstream os1;
                X.print(os1, LEVEL, SPL);

                if (veryVerbose) {
                    T_; T_; P(L_);
                    T_; T_; P(PRINT_EXP);
                    T_; T_; P(os1.str());
                }

                LOOP3_ASSERT(LINE,
                             PRINT_EXP,
                             os1.str(),
                             0 == bsl::strcmp(PRINT_EXP, os1.str().c_str()));
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING BASIC MANIPULATORS (BOOTSTRAP) AND PRIMARY ACCESSORS:
        //
        // Concerns:
        //   We are able to create a 'bdem_Choice' object and put it into
        //   any legal state.  We are not concerned in this test about trying
        //   to move the object from one state to another.
        //
        // Plan:
        //   A 'bdem_Choice' object can be put into any state using the
        //   two-argument constructor and the 'makeSelection' manipulator.
        //   The state of the object can be determined using the
        //   primary accessors.
        //
        //   Build objects containing 3 different values (including the
        //   "unset" value) of each of data types by first
        //   constructing each object, then using 'makeSelection' to
        //   set its selection type and value.  Using the primary accessors,
        //   verify the descriptor, value, and selector ID of each
        //   constructed object.
        //
        // Testing:
        //   bdem_Choice(const vector<EType::Type>&  typesCatalog,
        //               bslma_Allocator            *basicAlloc = 0);
        //   ~bdem_Choice();
        //   bdem_ElemRef  makeSelection(int index);
        //   bdem_ChoiceArrayItem& item();
        //   bdem_ElemRef& selection();
        //   int numSelections() const;
        //   int selector() const;
        //   const bdem_ChoiceArrayItem& item() const;
        //   bdem_ElemType::Type selectionType() const;
        //   bdem_ElemType::Type selectionType(int index) const;
        //   void selectionTypes(bsl::vector<EType::Type> *types) const;
        //   bdem_ConstElemRef& selection() const;
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING BASIC MANIPULATORS"
                               << "\n==========================" << bsl::endl;

        static const struct TestRow {
            int         d_line;
            const char *d_catalogSpec;  // Specification to create the catalog
        } DATA[] = {
            // Line     DescriptorSpec
            // ====     ==============
            { L_,       "" },
            { L_,       "A" },
            { L_,       "B" },
            { L_,       "C" },
            { L_,       "D" },
            { L_,       "E" },
            { L_,       "F" },
            { L_,       "G" },
            { L_,       "H" },
            { L_,       "I" },
            { L_,       "J" },
            { L_,       "K" },
            { L_,       "L" },
            { L_,       "M" },
            { L_,       "N" },
            { L_,       "O" },
            { L_,       "P" },
            { L_,       "Q" },
            { L_,       "R" },
            { L_,       "S" },
            { L_,       "T" },
            { L_,       "U" },
            { L_,       "V" },
            { L_,       "W" },
            { L_,       "X" },
            { L_,       "Y" },
            { L_,       "Z" },
            { L_,       "a" },
            { L_,       "b" },
            { L_,       "c" },
            { L_,       "d" },
            { L_,       "e" },
            { L_,       "f" },

            // Testing various interesting combinations
            { L_,       "AA" },
            { L_,       "AC" },
            { L_,       "CD" },
            { L_,       "CE" },
            { L_,       "FG" },
            { L_,       "GG" },
            { L_,       "GH" },
            { L_,       "MN" },
            { L_,       "OP" },
            { L_,       "PQ" },
            { L_,       "KQ" },
            { L_,       "ST" },
            { L_,       "WX" },
            { L_,       "UV" },
            { L_,       "YZ" },
            { L_,       "ab" },
            { L_,       "cd" },
            { L_,       "ef" },
            { L_,       "Ue" },
            { L_,       "Vf" },

            { L_,       "HIJ" },
            { L_,       "KLM" },
            { L_,       "RST" },
            { L_,       "YZa" },

            { L_,       "BLRW" },
            { L_,       "DGNQ" },
            { L_,       "QRST" },

            { L_,       "JHKHSK" },
            { L_,       "RISXLSW" },
            { L_,       "MXKZOLPR" },
            { L_,       "GGGGGGGGG" },
            { L_,       "QQQQQQQQQQ" },
            { L_,       "abcdABCDEFG" },
            { L_,       "FDLALAabADSF" },

            { L_,       "GSRWSASDEFDSA" },
            { L_,       "PKJHALKGabASDA" },
            { L_,       "XZSAEWRPOIJLKaAS" },
            { L_,       "GALKacKASJDKSWEIO" },

            { L_,       "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef" },
        };
        static const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

        {
          for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE  = DATA[i].d_line;
            const char *SPEC  = DATA[i].d_catalogSpec;
            const int   LEN   = bsl::strlen(SPEC);

            const TypesCatalog CATALOG = ggCatalog(SPEC);

            for (int k = 0; k < 4; ++k) {
              AggOption::AllocationStrategy mode =
                  (AggOption::AllocationStrategy) k;

              bslma_TestAllocator testAllocator(veryVeryVerbose);

            BEGIN_BSLMA_EXCEPTION_TEST {
              bdema_SequentialAllocator  seqAlloc(&testAllocator);
              bslma_Allocator           *alloc = &testAllocator;

              if (AggOption::BDEM_SUBORDINATE == mode) {
                  alloc = &seqAlloc;
              }

              Obj mX(CATALOG, mode, alloc); const Obj& X = mX;
              LOOP_ASSERT(LINE, CATALOG.size() == (unsigned)X.numSelections());
              LOOP_ASSERT(LINE, -1             == X.selector());
              LOOP_ASSERT(LINE, EType::BDEM_VOID    == X.selectionType());
              LOOP_ASSERT(LINE, EType::BDEM_VOID    == X.selectionType(-1));
              TypesCatalog tmpCatalog;
              X.selectionTypes(&tmpCatalog);
              LOOP_ASSERT(LINE, CATALOG        == tmpCatalog);

              for (int j = 0; j < LEN; ++j) {
                const char   S    = SPEC[j];
                EType::Type  TYPE = getElemType(S);
                const CERef VAL_A = getConstERefA(S);
                const CERef VAL_B = getConstERefB(S);
                const CERef VAL_N = getConstERefNull(S);

                if (veryVerbose) { P_(i) P_(j) P_(SPEC) P(S) }

                mX.makeSelection(j);
                LOOP_ASSERT(S, CATALOG.size() == (unsigned) X.numSelections());
                LOOP_ASSERT(S, j     == X.selector());
                LOOP_ASSERT(S, TYPE  == X.selectionType());
                LOOP_ASSERT(S, TYPE  == X.selectionType(j));
                LOOP_ASSERT(S, VAL_N == mX.selection());
                LOOP_ASSERT(S, VAL_N == X.selection());
                LOOP_ASSERT(S, compare(mX.item(), VAL_N, S));
                LOOP_ASSERT(S, compare(X.item(), VAL_N, S));
                tmpCatalog.clear();
                X.selectionTypes(&tmpCatalog);
                LOOP_ASSERT(LINE, CATALOG == tmpCatalog);

                mX.makeSelection(j).replaceValue(VAL_A);
                LOOP_ASSERT(S, CATALOG.size() == (unsigned) X.numSelections());
                LOOP_ASSERT(S, j     == X.selector());
                LOOP_ASSERT(S, TYPE  == X.selectionType());
                LOOP_ASSERT(S, TYPE  == X.selectionType(j));
                LOOP_ASSERT(S, VAL_A == mX.selection());
                LOOP_ASSERT(S, VAL_A == X.selection());
                LOOP_ASSERT(S, compare(mX.item(), VAL_A, S));
                LOOP_ASSERT(S, compare(X.item(), VAL_A, S));
                tmpCatalog.clear();
                X.selectionTypes(&tmpCatalog);
                LOOP_ASSERT(LINE, CATALOG == tmpCatalog);

                bdem_ElemRef elem = mX.makeSelection(j);
                LOOP_ASSERT(S, CATALOG.size() == (unsigned) X.numSelections());
                LOOP_ASSERT(S, j     == X.selector());
                LOOP_ASSERT(S, TYPE  == X.selectionType());
                LOOP_ASSERT(S, TYPE  == X.selectionType(j));
                LOOP_ASSERT(S, VAL_N == mX.selection());
                LOOP_ASSERT(S, VAL_N == X.selection());
                LOOP_ASSERT(S, VAL_N == elem);
                LOOP_ASSERT(S, compare(mX.item(), VAL_N, S));
                LOOP_ASSERT(S, compare(X.item(), VAL_N, S));
                tmpCatalog.clear();
                X.selectionTypes(&tmpCatalog);
                LOOP_ASSERT(LINE, CATALOG == tmpCatalog);

                mX.makeSelection(j).replaceValue(VAL_B);
                LOOP_ASSERT(S, CATALOG.size() == (unsigned) X.numSelections());
                LOOP_ASSERT(S, j     == X.selector());
                LOOP_ASSERT(S, TYPE  == X.selectionType());
                LOOP_ASSERT(S, TYPE  == X.selectionType(j));
                LOOP_ASSERT(S, VAL_B == mX.selection());
                LOOP_ASSERT(S, VAL_B == X.selection());
                LOOP_ASSERT(S, compare(mX.item(), VAL_B, S));
                LOOP_ASSERT(S, compare(X.item(), VAL_B, S));
                tmpCatalog.clear();
                X.selectionTypes(&tmpCatalog);
                LOOP_ASSERT(LINE, CATALOG == tmpCatalog);

                mX.makeSelection(j).replaceValue(VAL_N);
                LOOP_ASSERT(S, CATALOG.size() == (unsigned) X.numSelections());
                LOOP_ASSERT(S, j     == X.selector());
                LOOP_ASSERT(S, TYPE  == X.selectionType());
                LOOP_ASSERT(S, TYPE  == X.selectionType(j));
                LOOP_ASSERT(S, VAL_N == mX.selection());
                LOOP_ASSERT(S, VAL_N == X.selection());
                LOOP_ASSERT(S, compare(mX.item(), VAL_N, S));
                LOOP_ASSERT(S, compare(X.item(), VAL_N, S));
                tmpCatalog.clear();
                X.selectionTypes(&tmpCatalog);
                LOOP_ASSERT(LINE, CATALOG == tmpCatalog);

                int  newIdx  = (j + 5) % LEN;
                char newSpec = SPEC[newIdx];

                if (veryVerbose) { P(newIdx) P(newSpec) }

                EType::Type NEW_TYPE  = getElemType(newSpec);
                CERef       NEW_VAL_A = getConstERefA(newSpec);
                CERef       NEW_VAL_N = getConstERefNull(newSpec);

                mX.makeSelection(newIdx);
                LOOP_ASSERT(S, CATALOG.size() == (unsigned) X.numSelections());
                LOOP_ASSERT(S, newIdx         == X.selector());
                LOOP_ASSERT(S, NEW_TYPE       == X.selectionType());
                LOOP_ASSERT(S, NEW_TYPE  == X.selectionType(newIdx));
                LOOP_ASSERT(S, NEW_VAL_N == mX.selection());
                LOOP_ASSERT(S, NEW_VAL_N == X.selection());
                LOOP_ASSERT(S, compare(mX.item(), NEW_VAL_N, newSpec));
                LOOP_ASSERT(S, compare(X.item(), NEW_VAL_N, newSpec));
                tmpCatalog.clear();
                X.selectionTypes(&tmpCatalog);
                LOOP_ASSERT(LINE, CATALOG == tmpCatalog);

                mX.makeSelection(newIdx).replaceValue(NEW_VAL_A);
                LOOP_ASSERT(S, CATALOG.size() == (unsigned) X.numSelections());
                LOOP_ASSERT(S, newIdx    == X.selector());
                LOOP_ASSERT(S, NEW_TYPE  == X.selectionType());
                LOOP_ASSERT(S, NEW_TYPE  == X.selectionType(newIdx));
                LOOP_ASSERT(S, NEW_VAL_A == mX.selection());
                LOOP_ASSERT(S, NEW_VAL_A == X.selection());
                LOOP_ASSERT(S, compare(mX.item(), NEW_VAL_A, newSpec));
                LOOP_ASSERT(S, compare(X.item(), NEW_VAL_A, newSpec));
                tmpCatalog.clear();
                X.selectionTypes(&tmpCatalog);
                LOOP_ASSERT(LINE, CATALOG == tmpCatalog);

                mX.makeSelection(-1);
                LOOP_ASSERT(S, CATALOG.size() == (unsigned) X.numSelections());
                LOOP_ASSERT(S, -1             == X.selector());
                LOOP_ASSERT(S, EType::BDEM_VOID    == X.selectionType());
                LOOP_ASSERT(S, EType::BDEM_VOID    == X.selectionType(-1));
                LOOP_ASSERT(S, EType::BDEM_VOID  == mX.selection().type());
                LOOP_ASSERT(S, EType::BDEM_VOID  == X.selection().type());
                tmpCatalog.clear();
                X.selectionTypes(&tmpCatalog);
                LOOP_ASSERT(LINE, CATALOG == tmpCatalog);
              }
            } END_BSLMA_EXCEPTION_TEST
            }
          }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING helper functions
        //
        // Concerns:
        //  1. Confirm that 'ggCatalog' function works as expected
        //  2. Confirm that 'getValueA', 'getValueB', 'getValueN' work as
        //     expected.
        //
        // Plan:
        //  1. Test generation of the types catalog with various spec
        //     strings.
        //  2. Test that the getValue functions return the expected value for
        //     each value.  (Brute force testing).
        //
        // Testing:
        //   EType::Type getElemType(char spec);
        //   const Descriptor* getDescriptor(char spec);
        //   TypesCatalog ggCatalog(const char *spec);
        //   const void *getValueA(char spec);
        //   const void *getValueB(char spec);
        //   const void *getValueN(char spec);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting 'ggCatalog'"
                               << "\n===================" << bsl::endl;

        const char SPEC[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef";
              int  LEN    = sizeof(SPEC) - 1;

        bslma_TestAllocator testAllocator;

        if (verbose)  bsl::cout << "\nTesting 'getElemType'" << bsl::endl;
        {
            for (int i = 0; i < LEN; ++i) {
                ASSERT((EType::Type) i == getElemType(SPEC[i]));
            }
        }

        if (verbose)  bsl::cout << "\nTesting 'getDescriptor'" << bsl::endl;
        {
            for (int i = 0; i < LEN; ++i) {
                ASSERT(DESCRIPTORS[i] == getDescriptor(SPEC[i]));
            }
        }

        if (verbose)  bsl::cout << "\nTesting 'ggCatalog' with empty "
                                << "string." << bsl::endl;
        {
            TypesCatalog x1 = ggCatalog("");
            ASSERT(0 == x1.size());
        }

        if (verbose)
            bsl::cout << "\nTesting 'ggCatalog' for a 1 length catalog"
                      << " with all types." << bsl::endl;
        {

            ASSERT(LEN == SPEC_LEN);

            for (int i = 0; i < LEN; ++i) {
                char tempSpec[] = { SPEC[i], 0 };
                if (veryVerbose)
                    bsl::cout << "testing 'ggCatalog' using " << tempSpec
                              << bsl::endl;

                TypesCatalog x1 = ggCatalog(tempSpec);
                LOOP_ASSERT(i, 1 == x1.size());
                LOOP_ASSERT(i, (EType::Type) i == x1[0]);
            }
        }

        if (verbose)
            bsl::cout << "\nTesting 'ggCatalog' for a multi length catalog"
                      << bsl::endl;
        {
            const struct {
                int         d_line;
                const char *d_spec;
            } SPECS[] =
                {
                    { L_,       "AA" },
                    { L_,       "AC" },
                    { L_,       "CD" },
                    { L_,       "CE" },
                    { L_,       "FG" },
                    { L_,       "GG" },
                    { L_,       "GH" },
                    { L_,       "MN" },
                    { L_,       "OP" },
                    { L_,       "PQ" },
                    { L_,       "KQ" },
                    { L_,       "ST" },
                    { L_,       "WX" },
                    { L_,       "UV" },
                    { L_,       "YZ" },
                    { L_,       "ab" },
                    { L_,       "cd" },
                    { L_,       "ef" },
                    { L_,       "Ue" },
                    { L_,       "Vf" },

                    { L_,       "HIJ" },
                    { L_,       "KLM" },
                    { L_,       "RST" },
                    { L_,       "YZa" },

                    { L_,       "BLRW" },
                    { L_,       "DGNQ" },
                    { L_,       "QRST" },

                    { L_,       "JHKHSK" },
                    { L_,       "RISXLSW" },
                    { L_,       "MXKZOLPR" },
                    { L_,       "GGGGGGGGG" },
                    { L_,       "QQQQQQQQQQ" },
                    { L_,       "abcdABCDEFG" },
                    { L_,       "FDLALAabADSF" },

                    { L_,       "GSRWSASDEFDSA" },
                    { L_,       "PKJHALKGabASDA" },
                    { L_,       "XZSAEWRPOIJLKaAS" },
                    { L_,       "GALKacKASJDKSWEIO" },

                    { L_,       "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef" },
                };
            const int NUM_SPECS = sizeof (SPECS) / sizeof (*SPECS);

            for (int i = 0; i < NUM_SPECS; ++i) {
              const int   LINE = SPECS[i].d_line;
              const char *SPEC = SPECS[i].d_spec;
              const int   LEN  = bsl::strlen(SPEC);

              if (veryVerbose) { P(SPEC) }

              TypesCatalog x1 = ggCatalog(SPEC);
              LOOP3_ASSERT(LINE, LEN, x1.size(), (unsigned) LEN == x1.size());

              for (int j = 0; j < LEN; ++j) {
                const char S = SPEC[j];
                LOOP_ASSERT(LINE, getElemType(S) == x1[j]);
              }
            }
        }

        if (verbose)
            bsl::cout << "\nTesting 'getValueA'." << bsl::endl;
        {
            ASSERT(A00   == *(char *) getValueA(SPEC[0]));
            ASSERT(A01   == *(short *) getValueA(SPEC[1]));
            ASSERT(A02   == *(int *) getValueA(SPEC[2]));
            ASSERT(A03   == *(Int64 *) getValueA(SPEC[3]));
            ASSERT(A04   == *(float *) getValueA(SPEC[4]));
            ASSERT(A05   == *(double *) getValueA(SPEC[5]));
            ASSERT(A06   == *(bsl::string *) getValueA(SPEC[6]));
            ASSERT(A07   == *(Datetime *) getValueA(SPEC[7]));
            ASSERT(A08   == *(Date *) getValueA(SPEC[8]));
            ASSERT(A09   == *(Time *) getValueA(SPEC[9]));
            ASSERT(A10   == *(bsl::vector<char> *) getValueA(SPEC[10]));
            ASSERT(A11   == *(bsl::vector<short> *) getValueA(SPEC[11]));
            ASSERT(A12   == *(bsl::vector<int> *) getValueA(SPEC[12]));
            ASSERT(A13   == *(bsl::vector<Int64> *) getValueA(SPEC[13]));
            ASSERT(A14   == *(bsl::vector<float> *) getValueA(SPEC[14]));
            ASSERT(A15   == *(bsl::vector<double> *) getValueA(SPEC[15]));
            ASSERT(A16   == *(bsl::vector<bsl::string> *) getValueA(SPEC[16]));
            ASSERT(A17   == *(bsl::vector<Datetime> *) getValueA(SPEC[17]));
            ASSERT(A18   == *(bsl::vector<Date> *) getValueA(SPEC[18]));
            ASSERT(A19   == *(bsl::vector<Time> *) getValueA(SPEC[19]));
            ASSERT(A20   == *(bdem_List *) getValueA(SPEC[20]));
            ASSERT(A21   == *(bdem_Table *) getValueA(SPEC[21]));
            ASSERT(A22   == *(bool *) getValueA(SPEC[22]));
            ASSERT(A23   == *(DatetimeTz *) getValueA(SPEC[23]));
            ASSERT(A24   == *(DateTz *) getValueA(SPEC[24]));
            ASSERT(A25   == *(TimeTz *) getValueA(SPEC[25]));
            ASSERT(A26   == *(bsl::vector<bool> *) getValueA(SPEC[26]));
            ASSERT(A27   == *(bsl::vector<DatetimeTz> *) getValueA(SPEC[27]));
            ASSERT(A28   == *(bsl::vector<DateTz> *) getValueA(SPEC[28]));
            ASSERT(A29   == *(bsl::vector<TimeTz> *) getValueA(SPEC[29]));
            ASSERT(A30   == *(bdem_Choice *) getValueA(SPEC[30]));
            ASSERT(A31   == *(bdem_ChoiceArray *) getValueA(SPEC[31]));
        }

        if (verbose)
            bsl::cout << "\nTesting 'getValueB'." << bsl::endl;
        {
            ASSERT(B00   == *(char *) getValueB(SPEC[0]));
            ASSERT(B01   == *(short *) getValueB(SPEC[1]));
            ASSERT(B02   == *(int *) getValueB(SPEC[2]));
            ASSERT(B03   == *(Int64 *) getValueB(SPEC[3]));
            ASSERT(B04   == *(float *) getValueB(SPEC[4]));
            ASSERT(B05   == *(double *) getValueB(SPEC[5]));
            ASSERT(B06   == *(bsl::string *) getValueB(SPEC[6]));
            ASSERT(B07   == *(Datetime *) getValueB(SPEC[7]));
            ASSERT(B08   == *(Date *) getValueB(SPEC[8]));
            ASSERT(B09   == *(Time *) getValueB(SPEC[9]));
            ASSERT(B10   == *(bsl::vector<char> *) getValueB(SPEC[10]));
            ASSERT(B11   == *(bsl::vector<short> *) getValueB(SPEC[11]));
            ASSERT(B12   == *(bsl::vector<int> *) getValueB(SPEC[12]));
            ASSERT(B13   == *(bsl::vector<Int64> *) getValueB(SPEC[13]));
            ASSERT(B14   == *(bsl::vector<float> *) getValueB(SPEC[14]));
            ASSERT(B15   == *(bsl::vector<double> *) getValueB(SPEC[15]));
            ASSERT(B16   == *(bsl::vector<bsl::string> *) getValueB(SPEC[16]));
            ASSERT(B17   == *(bsl::vector<Datetime> *) getValueB(SPEC[17]));
            ASSERT(B18   == *(bsl::vector<Date> *) getValueB(SPEC[18]));
            ASSERT(B19   == *(bsl::vector<Time> *) getValueB(SPEC[19]));
            ASSERT(B20   == *(bdem_List *) getValueB(SPEC[20]));
            ASSERT(B21   == *(bdem_Table *) getValueB(SPEC[21]));
            ASSERT(B22   == *(bool *) getValueB(SPEC[22]));
            ASSERT(B23   == *(DatetimeTz *) getValueB(SPEC[23]));
            ASSERT(B24   == *(DateTz *) getValueB(SPEC[24]));
            ASSERT(B25   == *(TimeTz *) getValueB(SPEC[25]));
            ASSERT(B26   == *(bsl::vector<bool> *) getValueB(SPEC[26]));
            ASSERT(B27   == *(bsl::vector<DatetimeTz> *) getValueB(SPEC[27]));
            ASSERT(B28   == *(bsl::vector<DateTz> *) getValueB(SPEC[28]));
            ASSERT(B29   == *(bsl::vector<TimeTz> *) getValueB(SPEC[29]));
            ASSERT(B30   == *(bdem_Choice *) getValueB(SPEC[30]));
            ASSERT(B31   == *(bdem_ChoiceArray *) getValueB(SPEC[31]));
        }

        if (verbose)
            bsl::cout << "\nTesting 'getValueN'." << bsl::endl;
        {
            ASSERT(N00       == *(char *) getValueN(SPEC[0]));
            ASSERT(N01       == *(short *) getValueN(SPEC[1]));
            ASSERT(N02       == *(int *) getValueN(SPEC[2]));
            ASSERT(N03       == *(Int64 *) getValueN(SPEC[3]));
            ASSERT(N04       == *(float *) getValueN(SPEC[4]));
            ASSERT(N05       == *(double *) getValueN(SPEC[5]));
            ASSERT(N06       == *(bsl::string *) getValueN(SPEC[6]));
            ASSERT(N07       == *(Datetime *) getValueN(SPEC[7]));
            ASSERT(N08       == *(Date *) getValueN(SPEC[8]));
            ASSERT(N09       == *(Time *) getValueN(SPEC[9]));
            ASSERT(N10       == *(bsl::vector<char> *) getValueN(SPEC[10]));
            ASSERT(N11       == *(bsl::vector<short> *) getValueN(SPEC[11]));
            ASSERT(N12       == *(bsl::vector<int> *) getValueN(SPEC[12]));
            ASSERT(N13       == *(bsl::vector<Int64> *) getValueN(SPEC[13]));
            ASSERT(N14       == *(bsl::vector<float> *) getValueN(SPEC[14]));
            ASSERT(N15       == *(bsl::vector<double> *) getValueN(SPEC[15]));
            ASSERT(N16       == *(bsl::vector<bsl::string> *)
                                                         getValueN(SPEC[16]));
            ASSERT(N17       == *(bsl::vector<Datetime> *)
                                                         getValueN(SPEC[17]));
            ASSERT(N18       == *(bsl::vector<Date> *) getValueN(SPEC[18]));
            ASSERT(N19       == *(bsl::vector<Time> *) getValueN(SPEC[19]));
            ASSERT(N20       == *(bdem_List *) getValueN(SPEC[20]));
            ASSERT(N21       == *(bdem_Table *) getValueN(SPEC[21]));
            ASSERT(N22       == *(bool *) getValueN(SPEC[22]));
            ASSERT(N23       == *(DatetimeTz *) getValueN(SPEC[23]));
            ASSERT(N24       == *(DateTz *) getValueN(SPEC[24]));
            ASSERT(N25       == *(TimeTz *) getValueN(SPEC[25]));
            ASSERT(N26       == *(bsl::vector<bool> *) getValueN(SPEC[26]));
            ASSERT(N27       == *(bsl::vector<DatetimeTz> *)
                                                         getValueN(SPEC[27]));
            ASSERT(N28       == *(bsl::vector<DateTz> *) getValueN(SPEC[28]));
            ASSERT(N29       == *(bsl::vector<TimeTz> *) getValueN(SPEC[29]));
            ASSERT(N30       == *(bdem_Choice *) getValueN(SPEC[30]));
            ASSERT(N31       == *(bdem_ChoiceArray *) getValueN(SPEC[31]));
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
        //   After each of these steps, check that all attributes of the
        //   bdem_Choice object are as expected, check that memory is
        //   being used as expected, and (if in verbose mode), print the value
        //   of the created or modified object.
        //   1. Create an object, mX, using the default constructor.
        //   2. Modify mX so that it holds an int.
        //   3. Create an object, mY, that holds an unset string.
        //   3. Modify the value in mY so that it holds "hello
        //      world".
        //   4. Copy-construct mZ from mY.
        //   5. Assign mY = mX.
        //   6. Construct mA holding a double.
        //   7. Stream each choice into a test stream.
        //   8. Read all choices from the test stream into a vector of
        //      choices.
        //
        // Testing:
        //   This Test Case exercises basic value-semantic functionality.
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nBREATHING TEST"
                               << "\n==============" << bsl::endl;

        if (verbose) bsl::cout << "\tSizeof bdem_Choice :"
                               << sizeof(bdem_Choice) << bsl::endl;

        {
            if (veryVerbose) { bsl::cout << "\tDefault construct Obj mX"
                                         << bsl::endl; }
            bslma_TestAllocator alloc(veryVeryVerbose);

            Obj mX(&alloc); const Obj& X = mX;
            ASSERT(0  == X.numSelections());
            ASSERT(-1 == X.selector());
            ASSERT(EType::BDEM_VOID == X.selectionType());
            if (veryVerbose) { bsl::cout << "\tX:" << bsl::endl;
                               X.print(bsl::cout, 1, 4); }

            const int INT_VAL = 100;
            if (veryVerbose) {
                            bsl::cout << "\tModify mX to hold int with "
                                      <<  "Value: " << INT_VAL << bsl::endl; }
            mX.addSelection(EType::BDEM_INT);
            ASSERT(1  == X.numSelections());
            ASSERT(-1 == X.selector());
            ASSERT(EType::BDEM_VOID == X.selectionType());
            ASSERT(EType::BDEM_INT  == X.selectionType(0));

            mX.makeSelection(0).theModifiableInt() = INT_VAL;
            ASSERT(1 == X.numSelections());
            ASSERT(0 == X.selector());
            ASSERT(EType::BDEM_INT == X.selectionType());
            ASSERT(EType::BDEM_INT == X.selectionType(0));
            ASSERT(INT_VAL    == X.theInt());
            if (veryVerbose) { bsl::cout << "\tX:" << bsl::endl;
                               X.print(bsl::cout, 1, 4); }

            if (veryVerbose) {
                           bsl::cout << "\tConstruct Obj mY holding a string"
                                     << bsl::endl; }
            EType::Type catalog1[] = { EType::BDEM_STRING };
            Obj mY(catalog1, 1, &alloc); const Obj& Y = mY;
            ASSERT(1  == Y.numSelections());
            ASSERT(-1 == Y.selector());
            ASSERT(EType::BDEM_VOID   == Y.selectionType());
            ASSERT(EType::BDEM_STRING == Y.selectionType(0));
            ASSERT(X != Y);
            ASSERT(!(X == Y));
            if (veryVerbose) { bsl::cout << "\tY:" << bsl::endl;
                               Y.print(bsl::cout, 1, 4); }

            const bsl::string STR_VAL = "Hello World";
            if (veryVerbose) {
                           bsl::cout << "\tModify mY to have value "
                                     << STR_VAL << bsl::endl; }
            mY.makeSelection(0).theModifiableString() = STR_VAL;
            ASSERT(1 == Y.numSelections());
            ASSERT(0 == Y.selector());
            ASSERT(EType::BDEM_STRING == Y.selectionType());
            ASSERT(EType::BDEM_STRING == Y.selectionType(0));
            ASSERT(STR_VAL       == Y.theString());
            if (veryVerbose) { bsl::cout << "\tY:" << bsl::endl;
                               Y.print(bsl::cout, 1, 4); }

            if (veryVerbose) {
                    bsl::cout << "\tCopy Construct mZ from mY" << bsl::endl; }
            Obj mZ(mY, &alloc); const Obj& Z = mZ;
            ASSERT(!Y.selection().isNull());
            ASSERT(1 == Y.numSelections());
            ASSERT(0 == Y.selector());
            ASSERT(EType::BDEM_STRING == Y.selectionType());
            ASSERT(EType::BDEM_STRING == Y.selectionType(0));
            ASSERT(STR_VAL       == Y.theString());
            ASSERT(!Z.selection().isNull());
            ASSERT(1 == Z.numSelections());
            ASSERT(0 == Z.selector());
            ASSERT(EType::BDEM_STRING == Z.selectionType());
            ASSERT(EType::BDEM_STRING == Z.selectionType(0));
            ASSERT(STR_VAL       == Z.theString());
            ASSERT(X != Y);
            ASSERT(!(X == Y));
            ASSERT(Z == Y);
            ASSERT(!(Z != Y));
            if (veryVerbose) { bsl::cout << "\tX:" << bsl::endl;
                               X.print(bsl::cout, 1, 4); }
            if (veryVerbose) { bsl::cout << "\tY:" << bsl::endl;
                               Y.print(bsl::cout, 1, 4); }
            if (veryVerbose) { bsl::cout << "\tZ:" << bsl::endl;
                               Z.print(bsl::cout, 1, 4); }

            if (veryVerbose) {
                    bsl::cout << "\tMake mY null" << bsl::endl; }

            mY.selection().makeNull();
            ASSERT(1 == Y.selection().isNull());
            ASSERT(1 == isUnset(Y.selection()));
            ASSERT(1 == Y.numSelections());
            ASSERT(0 == Y.selector());
            ASSERT(EType::BDEM_STRING == Y.selectionType());
            ASSERT(EType::BDEM_STRING == Y.selectionType(0));
            ASSERT(STR_VAL       != Y.theString());
            ASSERT(1 == Z.selection().isNonNull());
            ASSERT(1 == Z.numSelections());
            ASSERT(0 == Z.selector());
            ASSERT(EType::BDEM_STRING == Z.selectionType());
            ASSERT(EType::BDEM_STRING == Z.selectionType(0));
            ASSERT(STR_VAL       == Z.theString());
            ASSERT(X != Y);
            ASSERT(!(X == Y));
            ASSERT(Z != Y);
            ASSERT(!(Z == Y));

            if (veryVerbose) { bsl::cout << "\tX:" << bsl::endl;
                               X.print(bsl::cout, 1, 4); }
            if (veryVerbose) { bsl::cout << "\tY:" << bsl::endl;
                               Y.print(bsl::cout, 1, 4); }
            if (veryVerbose) { bsl::cout << "\tZ:" << bsl::endl;
                               Z.print(bsl::cout, 1, 4); }

            if (veryVerbose) {
                    bsl::cout << "\tMake mZ null" << bsl::endl; }

            mZ.selection().makeNull();
            ASSERT(1 == Z.selection().isNull());
            ASSERT(1 == isUnset(Z.selection()));
            ASSERT(1 == Z.numSelections());
            ASSERT(0 == Z.selector());
            ASSERT(EType::BDEM_STRING == Z.selectionType());
            ASSERT(EType::BDEM_STRING == Z.selectionType(0));
            ASSERT(STR_VAL       != Z.theString());
            ASSERT(X != Y);
            ASSERT(!(X == Y));
            ASSERT(Z == Y);
            ASSERT(!(Z != Y));

            if (veryVerbose) { bsl::cout << "\tX:" << bsl::endl;
                               X.print(bsl::cout, 1, 4); }
            if (veryVerbose) { bsl::cout << "\tY:" << bsl::endl;
                               Y.print(bsl::cout, 1, 4); }
            if (veryVerbose) { bsl::cout << "\tZ:" << bsl::endl;
                               Z.print(bsl::cout, 1, 4); }

            if (veryVerbose) {
                    bsl::cout << "\tAssign mX to mY" << bsl::endl; }
            mY = mX;
            ASSERT(1 == X.numSelections());
            ASSERT(0 == X.selector());
            ASSERT(EType::BDEM_INT  == X.selectionType());
            ASSERT(EType::BDEM_INT  == X.selectionType(0));
            ASSERT(INT_VAL     == X.theInt());
            ASSERT(1 == Y.numSelections());
            ASSERT(0 == Y.selector());
            ASSERT(EType::BDEM_INT  == Y.selectionType());
            ASSERT(EType::BDEM_INT  == Y.selectionType(0));
            ASSERT(INT_VAL     == Y.theInt());
            ASSERT(X == Y);
            ASSERT(!(X != Y));
            ASSERT(Z != Y);
            ASSERT(!(Z == Y));
            if (veryVerbose) { bsl::cout << "\tX:" << bsl::endl;
                               X.print(bsl::cout, 1, 4); }
            if (veryVerbose) { bsl::cout << "\tY:" << bsl::endl;
                               Y.print(bsl::cout, 1, 4); }
            if (veryVerbose) { bsl::cout << "\tZ:" << bsl::endl;
                               Z.print(bsl::cout, 1, 4); }

            const double DBL_VAL = 123.5;
            if (veryVerbose) {
                           bsl::cout << "\tConstruct Obj mA having double"
                                     << " Value: " << DBL_VAL << bsl::endl; }
            TypesCatalog catalog2;
            catalog2.push_back(EType::BDEM_DOUBLE);
            Obj mA(catalog2, &alloc); const Obj& A = mA;
            mA.makeSelection(0).theModifiableDouble() = DBL_VAL;
            ASSERT(1 == A.numSelections());
            ASSERT(0 == A.selector());
            ASSERT(EType::BDEM_DOUBLE == A.selectionType());
            ASSERT(EType::BDEM_DOUBLE == A.selectionType(0));
            ASSERT(DBL_VAL       == A.theDouble());
            if (veryVerbose) { bsl::cout << "\tA:" << bsl::endl;
                               A.print(bsl::cout, 1, 4); }

            if (veryVerbose) {
                           bsl::cout << "\tStream out each choice"
                                     << bsl::endl; }
            const int MAX_VERSION = Obj::maxSupportedBdexVersion();

            bdex_TestOutStream os(&alloc);
            X.bdexStreamOut(os, MAX_VERSION);
            Y.bdexStreamOut(os, MAX_VERSION);
            Z.bdexStreamOut(os, MAX_VERSION);
            A.bdexStreamOut(os, MAX_VERSION);
            ASSERT(os);

            bdex_TestInStream is(os.data(), os.length());
            bsl::vector<Obj> objVec(4);
            objVec[0].bdexStreamIn(is, MAX_VERSION);
            objVec[1].bdexStreamIn(is, MAX_VERSION);
            objVec[2].bdexStreamIn(is, MAX_VERSION);
            objVec[3].bdexStreamIn(is, MAX_VERSION);

            ASSERT(X == objVec[0]);
            ASSERT(!(X != objVec[0]));
            ASSERT(Y == objVec[1]);
            ASSERT(!(Y != objVec[1]));
            // TBD: Uncomment when we bump MAX_VERSION.
            // Currently Z is null but bdex version doesn't know how to
            // transmit null information.
//             ASSERT(Z == objVec[2]);
//             ASSERT(!(Z != objVec[2]));
            ASSERT(A == objVec[3]);
            ASSERT(!(A != objVec[3]));
            if (veryVerbose) { bsl::cout << "\tX:" << bsl::endl;
                               X.print(bsl::cout, 1, 4); }
            if (veryVerbose) { bsl::cout << "\tY:" << bsl::endl;
                               Y.print(bsl::cout, 1, 4); }
            if (veryVerbose) { bsl::cout << "\tZ:" << bsl::endl;
                               Z.print(bsl::cout, 1, 4); }
            if (veryVerbose) { bsl::cout << "\tA:" << bsl::endl;
                               A.print(bsl::cout, 1, 4); }
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
