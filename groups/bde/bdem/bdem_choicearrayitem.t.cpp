// bdem_choicearrayitem.t.cpp                  -*-C++-*-

#include <bdem_choicearrayitem.h>

#include <bdem_choicearrayimp.h>
#include <bdem_choiceimp.h>
#include <bdem_descriptor.h>
#include <bdem_elemattrlookup.h>
#include <bdem_listimp.h>
#include <bdem_properties.h>
#include <bdem_rowdata.h>
#include <bdem_tableimp.h>

#include <bdema_sequentialallocator.h>
#include <bdex_testoutstream.h>
#include <bdex_testinstream.h>

#include <bdetu_unset.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>

#include <bsls_platform.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test is container class, capable of holding at most one
// object of any 'bdem' type.  A 'bdem_ChoiceArrayItem' contains a pointer to
// an externally-managed catalog of type descriptors, along with an index into
// that catalog (the "selector") that specifies which type the 'bdem_Choice'
// object is currently holding.  The interface to 'bdem_ChoiceArrayItem' is
// essentially a subset of the interface to 'bdem_Choice'.  Because the
// descriptor catalog is externally managed, there are no manipulators within
// 'bdem_ChoiceArrayItem' to change the descriptor catalog (i.e. the
// "structure" of the choice).  The selector and selection value of the
// choice *may* be changed, however.  This is what will be tested within.  We
// will also test the private manipulator 'setSelectionValue' indirectly.
//-----------------------------------------------------------------------------
// CLASS METHODS
// [ 6] static int maxSupportedBdexVersion();
//
// MANIPULATORS
// [ 7] bdem_ElemRef makeSelection(int index)
// [ 3] bdem_ElemRef selection()
// [ 8] bool& theModifiableBool();
// [ 8] char& theModifiableChar();
// [ 8] short& theModifiableShort();
// [ 8] int& theModifiableInt();
// [ 8] bsls_PlatformUtil::Int64& theModifiableInt64();
// [ 8] float& theModifiableFloat();
// [ 8] double& theModifiableDouble();
// [ 8] bsl::string& theModifiableString();
// [ 8] bdet_Datetime& theModifiableDatetime();
// [ 8] bdet_DatetimeTz& theModifiableDatetimeTz();
// [ 8] bdet_Date& theModifiableDate();
// [ 8] bdet_DateTz& theModifiableDateTz();
// [ 8] bdet_Time& theModifiableTime();
// [ 8] bdet_TimeTz& theModifiableTimeTz();
// [ 8] bsl::vector<bool>& theModifiableBoolArray();
// [ 8] bsl::vector<char>& theModifiableCharArray();
// [ 8] bsl::vector<short>& theModifiableShortArray();
// [ 8] bsl::vector<int>& theModifiableIntArray();
// [ 8] bsl::vector<Int64>& theModifiableInt64Array();
// [ 8] bsl::vector<float>& theModifiableFloatArray();
// [ 8] bsl::vector<double>& theModifiableDoubleArray();
// [ 8] bsl::vector<bsl::string>& theModifiableStringArray();
// [ 8] bsl::vector<bdet_Datetime>& theModifiableDatetimeArray();
// [ 8] bsl::vector<bdet_DatetimeTz>& theModifiableDatetimeTzArray();
// [ 8] bsl::vector<bdet_Date>& theModifiableDateArray();
// [ 8] bsl::vector<bdet_DateTz>& theModifiableDateTzArray();
// [ 8] bsl::vector<bdet_Time>& theModifiableTimeArray();
// [ 8] bsl::vector<bdet_TimeTz>& theModifiableTimeTzArray();
// [ 8] bdem_List& theModifiableList();
// [ 8] bdem_Table& theModifiableTable();
// [ 8] bdem_Choice& theModifiableChoice();
// [ 8] bdem_ChoiceArray& theModifiableChoiceArray();
// [11] void reset();
// [ 6] STREAM& bdexStreamIn(STREAM& stream, int version)
//
// ACCESSORS
// [ 3] int numSelections() const
// [ 3] int selector() const
// [ 3] bdem_ElemType::Type selectionType() const
// [ 3] bdem_ElemType::Type selectionType(int index) const
// [ 3] void selectionTypes(bsl::vector<bdem_ElemType::Type> *result) const
// [ 3] bdem_ConstElemRef selection() const
// [ 8] const bool& theBool() const;
// [ 8] const char& theChar() const;
// [ 8] const short& theShort() const;
// [ 8] const int& theInt() const;
// [ 8] const bsls_PlatformUtil::Int64& theInt64() const;
// [ 8] const float& theFloat() const;
// [ 8] const double& theDouble() const;
// [ 8] const bsl::string& theString() const;
// [ 8] const bdet_Datetime& theDatetime() const;
// [ 8] const bdet_DatetimeTz& theDatetimeTz() const;
// [ 8] const bdet_Date& theDate() const;
// [ 8] const bdet_DateTz& theDateTz() const;
// [ 8] const bdet_Time& theTime() const;
// [ 8] const bdet_TimeTz& theTimeTz() const;
// [ 8] const bsl::vector<bool>& theBoolArray() const;
// [ 8] const bsl::vector<char>& theCharArray() const;
// [ 8] const bsl::vector<short>& theShortArray() const;
// [ 8] const bsl::vector<int>& theIntArray() const;
// [ 8] const bsl::vector<bsls_PlatformUtil::Int64>& theInt64Array() const;
// [ 8] const bsl::vector<float>& theFloatArray() const;
// [ 8] const bsl::vector<double>& theDoubleArray() const;
// [ 8] const bsl::vector<bsl::string>& theStringArray() const;
// [ 8] const bsl::vector<bdet_Datetime>& theDatetimeArray() const;
// [ 8] const bsl::vector<bdet_DatetimeTz>& theDatetimeTzArray() const;
// [ 8] const bsl::vector<bdet_Date>& theDateArray() const;
// [ 8] const bsl::vector<bdet_DateTz>& theDateTzArray() const;
// [ 8] const bsl::vector<bdet_Time>& theTimeArray() const;
// [ 8] const bsl::vector<bdet_TimeTz>& theTimeTzArray() const;
// [ 8] const bdem_List& theList() const;
// [ 8] const bdem_Table& theTable() const;
// [ 8] const bdem_Choice& theChoice() const;
// [ 8] const bdem_ChoiceArray& theChoiceArray() const;

// [ 6] STREAM& bdexStreamOut(STREAM& stream, int version) const
// [ 4] bsl::ostream& print(bsl::ostream& stream,
//                          int           level = 0,
//                          int           spacesPerLevel = 4) const;
// USAGE EXAMPLE
//
// 'bdem_ChoiceArrayItem' free operators
// [ 5] bool operator==(const bdem_ChoiceArrayItem& lhs,
//                      const bdem_ChoiceArrayItem& rhs);
// [ 5] bool operator!=(const bdem_ChoiceArrayItem& lhs,
//                      const bdem_ChoiceArrayItem& rhs);
// [ 4] bsl::ostream& operator<<(bsl::ostream&               stream,
//                               const bdem_ChoiceArrayItem& rhs);

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
#define L_ __LINE__                                // current Line number
#define T_ bsl::cout << "\t" << bsl::flush;        // Print a tab (w/o newline)
#define N_ bsl::cout << bsl::endl;                 // Print a newline

#define A_(X) \
if (veryVeryVerbose) { \
  bsl::cout << "\nSPECIFIED ALLOCATOR:" << bsl::endl; \
  X.print(); \
  bsl::cout << "\nDEFAULT ALLOCATOR:" << bsl::endl; \
  defaultAllocator.print(); \
}

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;

typedef bdem_ChoiceArrayItem                 Obj;

typedef bdem_ChoiceHeader                    CHead;
typedef bdem_ChoiceHeader::DescriptorCatalog DescCatalog;
typedef bdem_Properties                      Prop;
typedef bdem_Descriptor                      Desc;
typedef bdem_ElemType                        EType;
typedef bdem_ElemRef                         ERef;
typedef bdem_ConstElemRef                    CERef;
typedef bdem_ElemType                        EType;
typedef bdem_AggregateOption                 AggOption;

typedef bsls_PlatformUtil::Int64             Int64;

typedef bdem_Choice                          Choice;
typedef bdem_ChoiceArray                     ChoiceArray;
typedef bdem_List                            List;
typedef bdem_Table                           Table;
typedef bdet_Datetime                        Datetime;
typedef bdet_Date                            Date;
typedef bdet_Time                            Time;
typedef bdet_DatetimeTz                      DatetimeTz;
typedef bdet_DateTz                          DateTz;
typedef bdet_TimeTz                          TimeTz;

typedef bsl::vector<EType::Type>             TypesCatalog;

//=============================================================================
//                  GLOBAL STUB CLASSES FOR TESTING
//-----------------------------------------------------------------------------

namespace BloombergLP {

                        // =======================
                        // DUMMY class bdem_Choice
                        // =======================

class bdem_Choice {
    // This class is a dummy bdem_Choice class to facilitate testing.
    // Note that it contains only the functions called in this test driver
    // and not the entire interface of bdem_ChoiceArray.

    bdem_ChoiceImp     d_choiceImp;

    friend bool operator==(const bdem_Choice& lhs,
                           const bdem_Choice& rhs);
  public:
    // CREATORS
    bdem_Choice();
    bdem_Choice(const EType::Type types[], int numTypes);

    // MANIPULATORS
    bdem_ElemRef makeSelection(int index);
};

bool operator==(const bdem_Choice& lhs, const bdem_Choice& rhs);
bool operator!=(const bdem_Choice& lhs, const bdem_Choice& rhs);

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
    // This class is a dummy bdem_List class to facilitate testing.
    // Note that it contains only the functions called in this test driver
    // and not the entire interface of bdem_List.

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

                        // ======================
                        // DUMMY class bdem_RowRef
                        // ======================

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
    // Note that it contains only the functions called in this test driver
    // and not the entire interface of bdem_Table.

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

                        // =======================
                        // DUMMY class bdem_Choice
                        // =======================

bdem_Choice::bdem_Choice()
{
}

bdem_Choice::bdem_Choice(const EType::Type types[], int numTypes)
: d_choiceImp(types,
              numTypes,
              bdem_ElemAttrLookup::lookupTable(),
              bdem_AggregateOption::BDEM_PASS_THROUGH)
{
}

bdem_ElemRef bdem_Choice::makeSelection(int index)
{
    return d_choiceImp.makeSelection(index);
}

bool operator==(const bdem_Choice& lhs, const bdem_Choice& rhs)
{
    return lhs.d_choiceImp == rhs.d_choiceImp;
}

bool operator!=(const bdem_Choice& lhs, const bdem_Choice& rhs)
{
    return !(lhs == rhs);
}

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
    return (bdem_ChoiceArrayItem&)d_choiceArrayImp.theModifiableItem(index);
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

const bsls_PlatformUtil::Int64
                       A03 = -100;
const bsls_PlatformUtil::Int64
                       B03 = -200;
const bsls_PlatformUtil::Int64
                       N03 =
                           bdetu_Unset<bsls_PlatformUtil::Int64>::unsetValue();

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

static bsl::vector<bsls_PlatformUtil::Int64>    fA13() {
       bsl::vector<bsls_PlatformUtil::Int64> t;
                                     t.push_back(A03); return t; }
static bsl::vector<bsls_PlatformUtil::Int64>    fB13() {
       bsl::vector<bsls_PlatformUtil::Int64> t;
                                     t.push_back(B03); return t; }
const  bsl::vector<bsls_PlatformUtil::Int64>     A13 = fA13();
const  bsl::vector<bsls_PlatformUtil::Int64>     B13 = fB13();
const  bsl::vector<bsls_PlatformUtil::Int64>     N13;

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
       const int NUM_TYPES = sizeof types / sizeof *types;
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
       const int NUM_TYPES = sizeof types / sizeof *types;
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
       const int NUM_TYPES = sizeof types / sizeof *types;
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
       const int NUM_TYPES = sizeof types / sizeof *types;
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
       const int NUM_TYPES = sizeof types / sizeof *types;
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
       const int NUM_TYPES = sizeof types / sizeof *types;
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
const int SPEC_LEN = sizeof SPECIFICATIONS - 1;

static const Desc *DESCRIPTORS_RAW[] =
    // This array stores the descriptors corresponding to each element
    // type specified in the spec string.
{
    &Prop::d_voidAttr,
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

static const Desc **const DESCRIPTORS = &DESCRIPTORS_RAW[1];
const int NUM_DESCS =
                        sizeof DESCRIPTORS_RAW / sizeof *DESCRIPTORS_RAW - 1;

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
const int NUM_VALUESA = sizeof VALUES_A / sizeof *VALUES_A;

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
const int NUM_VALUESB = sizeof VALUES_B / sizeof *VALUES_B;

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
const int NUM_VALUESN = sizeof VALUES_N / sizeof *VALUES_N;

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
void set(TYPE *lhs, const CERef &rhs, char spec)
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
      case 'b': lhs->theModifiableDatetimeTzArray() =
                                              rhs.theDatetimeTzArray(); break;
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
    if (!spec) {
      return EType::BDEM_VOID;
    }

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
    if (!spec) {
        return DESCRIPTORS[-1];
    }

    char *p = bsl::strchr(SPECIFICATIONS, spec);
    LOOP_ASSERT(spec, p);
    int index = p - SPECIFICATIONS;
    LOOP3_ASSERT(*p, index, SPEC_LEN, index < SPEC_LEN);
    return DESCRIPTORS[index];
}

static void ggCatalog(const char *s,
                      DescCatalog &catalog)
    // Return a types catalog containing the types corresponding to the
    // specified 's' specification string.
{
    const char *q = s + bsl::strlen(s);
    for (const char *p = s; p != q; ++p) {
        catalog.push_back(getDescriptor(*p));
    }
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

static bool isUnset(const CERef& ref)
    // Return 'true' if the specified element 'ref' references an element that
    // has the "unset" value for its type, and 'false' otherwise.
{
    return ref.descriptor()->isUnset(ref.data());
}

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------
// A simple choice array class, 'my_ChoiceArray' capable of holding any number
// homogeneous choice objects.  By "homogeneous," we mean that all of the
// choices have identical types catalogs.  The descriptor catalog for
// 'my_ChoiceArray' must be supplied at construction in the form of an array
// of type descriptors.  The implementation of 'my_ChoiceArray' actually holds
// 'bdem_ChoiceHeader' objects, which are cast to 'bdem_ChoiceArrayItem'
// on access.
class my_ChoiceArray {
        bdem_ChoiceHeader::DescriptorCatalog d_catalog;
        bsl::vector<bdem_ChoiceHeader>       d_array;
    public:
        // CREATORS
        my_ChoiceArray(const bdem_Descriptor *descriptors[],
                       int                    numDescriptors);
        my_ChoiceArray(const my_ChoiceArray& other);
        ~my_ChoiceArray();

        // MANIPULATORS
        my_ChoiceArray& operator=(const my_ChoiceArray& rhs);
            // Assign this object the value of rhs.

        void appendNullItem();
            // Append a null choice item to this array.

        bdem_ChoiceArrayItem& operator[](int index);
            // Return the item at the specified 'index'.

        // ACCESSORS
        int length() const;
            // Return the number of items in the array.

        const bdem_ChoiceArrayItem& operator[](int index) const;
            // Return the item at the specified 'index'.
};

my_ChoiceArray::my_ChoiceArray(const bdem_Descriptor *descriptors[],
                               int                    numDescriptors)
    : d_catalog(descriptors, descriptors + numDescriptors)
{
}

my_ChoiceArray::~my_ChoiceArray()
{
}

void my_ChoiceArray::appendNullItem() {
    bdem_ChoiceHeader newItem(&d_catalog);
    d_array.push_back(newItem);
}

bdem_ChoiceArrayItem& my_ChoiceArray::operator[](int index) {
    return reinterpret_cast<bdem_ChoiceArrayItem&>(d_array[index]);
}

int my_ChoiceArray::length() const {
    return d_array.size();
}

const bdem_ChoiceArrayItem& my_ChoiceArray::operator[](int index) const {
    return reinterpret_cast<const bdem_ChoiceArrayItem&>(d_array[index]);
}

my_ChoiceArray& my_ChoiceArray::operator=(const my_ChoiceArray& rhs) {
    if (this != &rhs) {
        d_array.clear();
        d_catalog = rhs.d_catalog;
        bdem_ChoiceHeader newItem(&d_catalog);
        d_array.resize(rhs.length(), newItem);
        for (int i = 0; i < d_array.size(); ++i) {
            d_array[i].makeSelection(rhs.d_array[i].selector(),
                                     rhs.d_array[i].selectionPointer());
        }
    }

    return *this;
}

my_ChoiceArray::my_ChoiceArray(const my_ChoiceArray& other) {
    // Delegate to the assignment operator
    this->operator=(other);
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

    bslma_TestAllocator defaultAllocator;  // default allocation
    const bslma_DefaultAllocatorGuard DAG(&defaultAllocator);

    switch (test) { case 0:  // Zero is always the leading case.
      case 11: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl << "USAGE EXAMPLE" << bsl::endl
                                            << "=============" << bsl::endl;
// Usage
// -----
// In this example, we will implement a simple choice array class,
// 'my_ChoiceArray' capable of holding any number homogeneous choice objects.
// By "homogeneous," we mean that all of the choices have identical types
// catalogs.  The descriptor catalog for 'my_ChoiceArray' must be supplied at
// construction in the form of an array of type descriptors.  The
// implementation of 'my_ChoiceArray' actually holds 'bdem_ChoiceHeader'
// objects, which are cast to 'bdem_ChoiceArrayItem' on access:
//..
//  class my_ChoiceArray {
//          bdem_ChoiceHeader::DescriptorCatalog d_catalog;
//          bsl::vector<bdem_ChoiceHeader>       d_array;
//      public:
//          // CREATORS
//          my_ChoiceArray(const bdem_Descriptor *descriptors[],
//                         int                    numDescriptors);
//          my_ChoiceArray(const my_ChoiceArray& other);
//          ~my_ChoiceArray();
//
//          // MANIPULATORS
//          void appendNullItem();
//              // Append a null choice item to this array.
//
//          bdem_ChoiceArrayItem& operator[](int index);
//              // Return the item at the specified 'index'.
//
//          my_ChoiceArray& operator=(const my_ChoiceArray& rhs);
//
//          // ACCESSORS
//          int length() const;
//              // Return the number of items in the array.
//
//          const bdem_ChoiceArrayItem& operator[](int index) const;
//              // Return the item at the specified 'index'.
//  };
//..
// The main constructor must create the types catalog, which will be common to
// all items in the array.  The array remains empty:
//..
//  my_ChoiceArray::my_ChoiceArray(const bdem_Descriptor *descriptors[],
//                                 int                    numDescriptors)
//      : d_catalog(descriptors, descriptors + numDescriptors)
//  {
//  }
//
//  my_ChoiceArray::~my_ChoiceArray()
//  {
//  }
//..
// Now, when we insert a new item, we must ensure that it uses the common
// catalog.  We are still treating the items as 'bdem_ChoiceHeader' objects,
// so we can have access to its constructors:
//..
//  void my_ChoiceArray::appendNullItem() {
//      bdem_ChoiceHeader newItem(&d_catalog);
//      d_array.push_back(newItem);
//  }
//..
// To access an item in the array, we cast the 'bdem_ChoiceHeader' to
// a 'bdem_ChoiceArrayImp' reference:
//..
//  bdem_ChoiceArrayItem& my_ChoiceArray::operator[](int index) {
//      return reinterpret_cast<bdem_ChoiceArrayItem&>(d_array[index]);
//  }
//..
// The two accessors are straight-forward:
//..
//  int my_ChoiceArray::length() const {
//      return d_array.size();
//  }
//
//  const bdem_ChoiceArrayItem& my_ChoiceArray::operator[](int index) const {
//      return reinterpret_cast<const bdem_ChoiceArrayItem&>(d_array[index]);
//  }
//..
// Finally, we implement the copy constructor and assignment for
// 'my_ChoiceArray'.  This requires some care because if we do s
// straight-forward copy of the 'd_array' vector, the items in the copy will
// end up pointing to the descriptor catalog in the original.  Instead, we
// implement the copy by emptying the destination array, copying the catalog,
// then looping through the choice items and using 'makeSelection' to copy the
// data (but not the catalog pointer) for each item.
//..
//  my_ChoiceArray& my_ChoiceArray::operator=(const my_ChoiceArray& rhs) {
//      if (this != &rhs) {
//          d_array.clear();
//          d_catalog = rhs.d_catalog;
//          bdem_ChoiceHeader newItem(&d_catalog);
//          d_array.resize(rhs.length(), newItem);
//          for (int i = 0; i < d_array.size(); ++i) {
//              d_array[i].makeSelection(rhs.d_array[i].selector(),
//                                       rhs.d_array[i].selectionPointer());
//          }
//      }
//
//      return *this;
//  }
//
//  my_ChoiceArray::my_ChoiceArray(const my_ChoiceArray& other) {
//      // Delegate to the assignment operator
//      this->operator=(other);
//  }
//..
// Our main program exercises the facilities of 'my_ChoiceArray'.  We start by
// creating an array of choices, each of which may contain a double or a
// string.  We then create three array items: two strings and a double.  For
// brevity, the definitions of 'my_StringDescriptor' and 'my_DoubleDescriptor'
// are omitted.  Implementations can be found in 'bdem_attributes.cpp'.
//..
//  int main() {

        static const bdem_Descriptor *DESCRIPTORS[] = {
            &Prop::d_stringAttr,
            &Prop::d_doubleAttr
        };

        my_ChoiceArray theArray(DESCRIPTORS, 2);
        ASSERT(0 == theArray.length());

        theArray.appendNullItem();
        theArray.appendNullItem();
        theArray.appendNullItem();
        ASSERT(3 == theArray.length());
        ASSERT(-1 == theArray[0].selector());
        ASSERT(-1 == theArray[1].selector());
        ASSERT(-1 == theArray[2].selector());

        theArray[0].makeSelection(0).theModifiableString() = "Hello";
        theArray[1].makeSelection(0).theModifiableString() = "World";
        theArray[2].makeSelection(1).theModifiableDouble() = 3.2;

        ASSERT(0       == theArray[0].selector());
        ASSERT("Hello" == theArray[0].theString());
        ASSERT(0       == theArray[1].selector());
        ASSERT("World" == theArray[1].theString());
        ASSERT(1       == theArray[2].selector());
        ASSERT(3.2     == theArray[2].theDouble());

        // Make a copy
        my_ChoiceArray theCopy(theArray);
        ASSERT(3       == theCopy.length());
        ASSERT(0       == theCopy[0].selector());
        ASSERT("Hello" == theCopy[0].theString());
        ASSERT(0       == theCopy[1].selector());
        ASSERT("World" == theCopy[1].theString());
        ASSERT(1       == theCopy[2].selector());
        ASSERT(3.2     == theCopy[2].theDouble());

//      return 0;
//  }
// ..
      } break;
      case 10: {
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
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bslma_TestAllocator alloc(veryVeryVerbose);

        for (int i1 = 0; i1 < NUM_DATA; ++i1) {
            const int   LINE1  = DATA[i1].d_line;
            const char *SPEC1  = DATA[i1].d_catalogSpec;
            const int   LEN1   = bsl::strlen(SPEC1);

            if (veryVerbose) { P(SPEC1); }

            for (int j1 = 0; j1 < LEN1; ++j1) {
                const char   S1  = SPEC1[j1];
                const CERef C1_A = CERef(getValueA(S1), getDescriptor(S1));

                bslma_TestAllocator alloc(veryVeryVerbose);
                DescCatalog catalog(&alloc);  ggCatalog(SPEC1, catalog);

                CHead mXCH(&catalog);
                Obj &mX = reinterpret_cast<Obj&>(mXCH);
                const Obj& X = mX;
                Obj &mY = reinterpret_cast<Obj&>(mXCH);
                const Obj& Y = mY;

                mX.makeSelection(j1).replaceValue(C1_A);
                mY.makeSelection(j1).replaceValue(C1_A);
                LOOP_ASSERT(LINE1, !X.selection().isNull());

                mX.reset();
                LOOP_ASSERT(LINE1, catalog.size()  == X.numSelections());
                LOOP_ASSERT(LINE1, -1 == X.selector());
                LOOP_ASSERT(LINE1, EType::BDEM_VOID == X.selectionType());
                LOOP_ASSERT(LINE1, EType::BDEM_VOID == X.selectionType(-1));
                LOOP_ASSERT(LINE1, X.selection().isNull());
                LOOP_ASSERT(LINE1, X == Y);
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'theModifiableXXX' MANIPULATORS/ACCESSORS
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
        //   theModifiableBool(value);
        //   theModifiableChar(value);
        //   theModifiableShort(value);
        //   theModifiableInt(value);
        //   theModifiableInt64(value);
        //   theModifiableFloat(value);
        //   theModifiableDouble(value);
        //   theModifiableString(value);
        //   theModifiableDatetime(value);
        //   theModifiableDate(value);
        //   theModifiableTime(value);
        //   theModifiableCharArray(value);
        //   theModifiableShortArray(value);
        //   theModifiableIntArray(value);
        //   theModifiableInt64Array(value);
        //   theModifiableFloatArray(value);
        //   theModifiableDoubleArray(value);
        //   theModifiableDatetimeArray(value);
        //   theModifiableDateArray(value);
        //   theModifiableTimeArray(value);
        //   theModifiableList(value);
        //   theModifiableTable(value);
        //   theModifiableChoice(value);
        //   theModifiableChoiceArray(value);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting theModifiableXXX functions"
                               << "\n=================================="
                               << bsl::endl;

        if (verbose) bsl::cout << "\nAll kinds of one-selection choice"
                               << bsl::endl;

#       define TEST_THEMODIFIABLE(TYPE, LTYPE, NR)                      \
        if (verbose) {                                                  \
            bsl::cout << "\t" << #TYPE << bsl::endl;                    \
        }                                                               \
        {                                                               \
            bslma_TestAllocator alloc(veryVeryVerbose);                 \
                                                                        \
            DescCatalog catalog(&alloc);                                \
            catalog.push_back(&Prop::d_ ## LTYPE ## Attr);              \
            CHead mXCH(&catalog);                                       \
            Obj &mX = reinterpret_cast<Obj&>(mXCH);                     \
            const Obj& X = mX;                                          \
            mX.makeSelection(0);                                        \
                                                                        \
            mX.theModifiable ## TYPE() = A ## NR;                       \
            ASSERT(A ## NR == X.the ## TYPE());                         \
            ASSERT(!X.selection().isNull());                            \
                                                                        \
            mX.selection().makeNull();                                  \
            ASSERT(X.selection().isNull());                             \
                                                                        \
            mX.theModifiable ## TYPE() = B ## NR;                       \
            ASSERT(!X.selection().isNull());                            \
            ASSERT(B ## NR == X.the ## TYPE());                         \
                                                                        \
            mX.theModifiable ## TYPE() = N ## NR;                       \
            ASSERT(N ## NR == X.the ## TYPE());                         \
            ASSERT(!X.selection().isNull());                            \
        }

        TEST_THEMODIFIABLE(Bool, bool, 22);
        TEST_THEMODIFIABLE(Char, char, 00);
        TEST_THEMODIFIABLE(Short, short, 01);
        TEST_THEMODIFIABLE(Int, int, 02);
        TEST_THEMODIFIABLE(Int64, int64, 03);
        TEST_THEMODIFIABLE(Float, float, 04);
        TEST_THEMODIFIABLE(Double, double, 05);
        TEST_THEMODIFIABLE(String, string, 06);
        TEST_THEMODIFIABLE(Datetime, datetime, 07);
        TEST_THEMODIFIABLE(DatetimeTz, datetimeTz, 23);
        TEST_THEMODIFIABLE(Date, date, 08);
        TEST_THEMODIFIABLE(DateTz, dateTz, 24);
        TEST_THEMODIFIABLE(Time, time, 09);
        TEST_THEMODIFIABLE(TimeTz, timeTz, 25);
        TEST_THEMODIFIABLE(BoolArray, boolArray, 26);
        TEST_THEMODIFIABLE(CharArray, charArray, 10);
        TEST_THEMODIFIABLE(ShortArray, shortArray, 11);
        TEST_THEMODIFIABLE(IntArray, intArray, 12);
        TEST_THEMODIFIABLE(Int64Array, int64Array, 13);
        TEST_THEMODIFIABLE(FloatArray, floatArray, 14);
        TEST_THEMODIFIABLE(DoubleArray, doubleArray, 15);
        TEST_THEMODIFIABLE(StringArray, stringArray, 16);
        TEST_THEMODIFIABLE(DatetimeArray, datetimeArray, 17);
        TEST_THEMODIFIABLE(DatetimeTzArray, datetimeTzArray, 27);
        TEST_THEMODIFIABLE(DateArray, dateArray, 18);
        TEST_THEMODIFIABLE(DateTzArray, dateTzArray, 28);
        TEST_THEMODIFIABLE(TimeArray, timeArray, 19);
        TEST_THEMODIFIABLE(TimeTzArray, timeTzArray, 29);

#       undef TEST_THEMODIFIABLE

#       define TEST_THEMODIFIABLE2(TYPE, DESC, NR)                      \
        if (verbose) {                                                  \
            bsl::cout << "\t" << #TYPE << bsl::endl;                    \
        }                                                               \
        {                                                               \
            bslma_TestAllocator alloc(veryVeryVerbose);                 \
                                                                        \
            DescCatalog catalog(&alloc);                                \
            catalog.push_back(DESC);                                    \
            CHead mXCH(&catalog);                                       \
            Obj &mX = reinterpret_cast<Obj&>(mXCH);                     \
            const Obj& X = mX;                                          \
            mX.makeSelection(0);                                        \
                                                                        \
            mX.theModifiable ## TYPE() = A ## NR;                       \
            ASSERT(A ## NR == X.the ## TYPE());                         \
            ASSERT(!X.selection().isNull());                            \
                                                                        \
            mX.selection().makeNull();                                  \
            ASSERT(X.selection().isNull());                             \
                                                                        \
            mX.theModifiable ## TYPE() = B ## NR;                       \
            ASSERT(!X.selection().isNull());                            \
            ASSERT(B ## NR == X.the ## TYPE());                         \
                                                                        \
            mX.theModifiable ## TYPE() = N ## NR;                       \
            ASSERT(N ## NR == X.the ## TYPE());                         \
            ASSERT(!X.selection().isNull());                            \
        }

        TEST_THEMODIFIABLE2(List, &bdem_ListImp::d_listAttr, 20);
        TEST_THEMODIFIABLE2(Table, &bdem_TableImp::d_tableAttr, 21);
        TEST_THEMODIFIABLE2(Choice, &bdem_ChoiceImp::d_choiceAttr, 30);
        TEST_THEMODIFIABLE2(ChoiceArray,
                                  &bdem_ChoiceArrayImp::d_choiceArrayAttr, 31);

#       undef TEST_THEMODIFIABLE2

        if (verbose) bsl::cout << "\nMulti-selection choice" << bsl::endl;
        {
          const struct {
              int         d_line;
              const char *d_catalogSpec;  // Specification to create the
                                          // catalog
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
          const int NUM_DATA = sizeof DATA / sizeof *DATA;

          for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE  = DATA[i].d_line;
            const char *SPEC  = DATA[i].d_catalogSpec;
            const int   LEN   = bsl::strlen(SPEC);

            bslma_TestAllocator alloc(veryVeryVerbose);
            DescCatalog catalog(&alloc);  ggCatalog(SPEC, catalog);

            CHead mXCH(&catalog);
            Obj &mX = reinterpret_cast<Obj&>(mXCH);
            const Obj& X = mX;

            for (int j = 0; j < LEN; ++j) {
                const char  S     = SPEC[j];
                const CERef VAL_A = CERef(getValueA(S), getDescriptor(S));
                const CERef VAL_B = CERef(getValueB(S), getDescriptor(S));
                const CERef VAL_N = CERef(getValueN(S), getDescriptor(S));

                mX.makeSelection(j);
                set(&mX, VAL_A, S);
                LOOP_ASSERT(S, compare(X, VAL_A, S));
                LOOP_ASSERT(LINE, !X.selection().isNull());

                set(&mX, VAL_B, S);
                LOOP_ASSERT(S, compare(X, VAL_B, S));
                LOOP_ASSERT(LINE, !X.selection().isNull());

                set(&mX, VAL_N, S);
                LOOP_ASSERT(S, compare(X, VAL_N, S));
                LOOP_ASSERT(LINE, !X.selection().isNull());
            }
          }
        }
      } break;
      case 8: {
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
        //   bool& theModifiableBool();
        //   char& theModifiableChar();
        //   short& theModifiableShort();
        //   int& theModifiableInt();
        //   bsls_PlatformUtil::Int64& theModifiableInt64();
        //   float& theModifiableFloat();
        //   double& theModifiableDouble();
        //   bsl::string& theModifiableString();
        //   bdet_Datetime& theModifiableDatetime();
        //   bdet_DatetimeTz& theModifiableDatetimeTz();
        //   bdet_Date& theModifiableDate();
        //   bdet_DateTz& theModifiableDateTz();
        //   bdet_Time& theModifiableTime();
        //   bdet_TimeTz& theModifiableTimeTz();
        //   bsl::vector<bool>& theModifiableBoolArray();
        //   bsl::vector<char>& theModifiableCharArray();
        //   bsl::vector<short>& theModifiableShortArray();
        //   bsl::vector<int>& theModifiableIntArray();
        //   bsl::vector<Int64>& theModifiableInt64Array();
        //   bsl::vector<float>& theModifiableFloatArray();
        //   bsl::vector<double>& theModifiableDoubleArray();
        //   bsl::vector<bsl::string>& theModifiableStringArray();
        //   bsl::vector<bdet_Datetime>& theModifiableDatetimeArray();
        //   bsl::vector<bdet_DatetimeTz>& theModifiableDatetimeTzArray();
        //   bsl::vector<bdet_Date>& theModifiableDateArray();
        //   bsl::vector<bdet_DateTz>& theModifiableDateTzArray();
        //   bsl::vector<bdet_Time>& theModifiableTimeArray();
        //   bsl::vector<bdet_TimeTz>& theModifiableTimeTzArray();
        //   bdem_List& theModifiableList();
        //   bdem_Table& theModifiableTable();
        //   bdem_Choice& theModifiableChoice();
        //   bdem_ChoiceArray& theModifiableChoiceArray();
        //   const bool& theBool() const;
        //   const char& theChar() const;
        //   const short& theShort() const;
        //   const int& theInt() const;
        //   const bsls_PlatformUtil::Int64& theInt64() const;
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
        //   const vector<bsls_PlatformUtil::Int64>& theInt64Array() const;
        //   const vector<float>& theFloatArray() const;
        //   const vector<double>& theDoubleArray() const;
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
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting theXXX functions"
                               << "\n========================"
                               << bsl::endl;

        if (verbose) bsl::cout << "\nAll kinds of one-selection choice"
                               << bsl::endl;

#       define TEST_THEMODIFIABLE3(TYPE, LTYPE, NR)                     \
        if (verbose) {                                                  \
            bsl::cout << "\t" #TYPE << bsl::endl;                       \
        }                                                               \
        {                                                               \
            bslma_TestAllocator alloc(veryVeryVerbose);                 \
                                                                        \
            DescCatalog catalog(&alloc);                                \
            catalog.push_back(&Prop::d_ ## LTYPE ## Attr);              \
            CHead mXCH(&catalog);                                       \
            Obj &mX = reinterpret_cast<Obj&>(mXCH);                     \
            const Obj& X = mX;                                          \
            mX.makeSelection(0);                                        \
                                                                        \
            mX.theModifiable ## TYPE() = A ## NR;                       \
            ASSERT(A ## NR == X.the ## TYPE());                         \
            mX.theModifiable ## TYPE() = B ## NR;                       \
            ASSERT(B ## NR == X.the ## TYPE());                         \
            mX.theModifiable ## TYPE() = N ## NR;                       \
            ASSERT(N ## NR == X.the ## TYPE());                         \
        }

        TEST_THEMODIFIABLE3(Bool, bool, 22);
        TEST_THEMODIFIABLE3(Char, char, 00);
        TEST_THEMODIFIABLE3(Short, short, 01);
        TEST_THEMODIFIABLE3(Int, int, 02);
        TEST_THEMODIFIABLE3(Int64, int64, 03);
        TEST_THEMODIFIABLE3(Float, float, 04);
        TEST_THEMODIFIABLE3(Double, double, 05);
        TEST_THEMODIFIABLE3(String, string, 06);
        TEST_THEMODIFIABLE3(Datetime, datetime, 07);
        TEST_THEMODIFIABLE3(DatetimeTz, datetimeTz, 23);
        TEST_THEMODIFIABLE3(Date, date, 08);
        TEST_THEMODIFIABLE3(DateTz, dateTz, 24);
        TEST_THEMODIFIABLE3(Time, time, 09);
        TEST_THEMODIFIABLE3(TimeTz, timeTz, 25);
        TEST_THEMODIFIABLE3(BoolArray, boolArray, 26);
        TEST_THEMODIFIABLE3(CharArray, charArray, 10);
        TEST_THEMODIFIABLE3(ShortArray, shortArray, 11);
        TEST_THEMODIFIABLE3(IntArray, intArray, 12);
        TEST_THEMODIFIABLE3(Int64Array, int64Array, 13);
        TEST_THEMODIFIABLE3(FloatArray, floatArray, 14);
        TEST_THEMODIFIABLE3(DoubleArray, doubleArray, 15);
        TEST_THEMODIFIABLE3(StringArray, stringArray, 16);
        TEST_THEMODIFIABLE3(DatetimeArray, datetimeArray, 17);
        TEST_THEMODIFIABLE3(DatetimeTzArray, datetimeTzArray, 27);
        TEST_THEMODIFIABLE3(DateArray, dateArray, 18);
        TEST_THEMODIFIABLE3(DateTzArray, dateTzArray, 28);
        TEST_THEMODIFIABLE3(TimeArray, timeArray, 19);
        TEST_THEMODIFIABLE3(TimeTzArray, timeTzArray, 29);

#       undef TEST_THEMODIFIABLE3

#       define TEST_THEMODIFIABLE4(TYPE, DESC, NR)                      \
        if (verbose) {                                                  \
            bsl::cout << "\t" #TYPE << bsl::endl;                       \
        }                                                               \
        {                                                               \
            bslma_TestAllocator alloc(veryVeryVerbose);                 \
                                                                        \
            DescCatalog catalog(&alloc);                                \
            catalog.push_back(DESC);                                    \
            CHead mXCH(&catalog);                                       \
            Obj &mX = reinterpret_cast<Obj&>(mXCH);                     \
            const Obj& X = mX;                                          \
            mX.makeSelection(0);                                        \
                                                                        \
            mX.theModifiable ## TYPE() = A ## NR;                       \
            ASSERT(A ## NR == X.the ## TYPE());                         \
            mX.theModifiable ## TYPE() = B ## NR;                       \
            ASSERT(B ## NR == X.the ## TYPE());                         \
            mX.theModifiable ## TYPE() = N ## NR;                       \
            ASSERT(N ## NR == X.the ## TYPE());                         \
        }

        TEST_THEMODIFIABLE4(List, &bdem_ListImp::d_listAttr, 20);
        TEST_THEMODIFIABLE4(Table, &bdem_TableImp::d_tableAttr, 21);
        TEST_THEMODIFIABLE4(Choice, &bdem_ChoiceImp::d_choiceAttr, 30);
        TEST_THEMODIFIABLE4(ChoiceArray,
                                  &bdem_ChoiceArrayImp::d_choiceArrayAttr, 31);

#       undef TEST_THEMODIFIABLE4

        if (verbose) bsl::cout << "\nMulti-selection choice" << bsl::endl;
        {
          const struct {
              int         d_line;
              const char *d_catalogSpec;  // spec to create the catalog
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
          const int NUM_DATA = sizeof DATA / sizeof *DATA;

          for (int i = 0; i < NUM_DATA; ++i) {
              const int   LINE  = DATA[i].d_line;
              const char *SPEC  = DATA[i].d_catalogSpec;
              const int   LEN   = bsl::strlen(SPEC);

              bslma_TestAllocator alloc(veryVeryVerbose);
              DescCatalog catalog(&alloc);  ggCatalog(SPEC, catalog);

              CHead mXCH(&catalog);
              Obj &mX = reinterpret_cast<Obj&>(mXCH);
              const Obj& X = mX;

              for (int j = 0; j < LEN; ++j) {
                  const char  S     = SPEC[j];
                  const CERef VAL_A = CERef(getValueA(S), getDescriptor(S));
                  const CERef VAL_B = CERef(getValueB(S), getDescriptor(S));
                  const CERef VAL_N = CERef(getValueN(S), getDescriptor(S));

                  mX.makeSelection(j);
                  assign(&mX, VAL_A, S);
                  LOOP_ASSERT(S, compare(X, VAL_A, S));

                  assign(&mX, VAL_B, S);
                  LOOP_ASSERT(S, compare(X, VAL_B, S));

                  assign(&mX, VAL_N, S);
                  LOOP_ASSERT(S, compare(X, VAL_N, S));
                }
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'makeSelection' FUNCTIONS:
        //
        // Concerns:
        //   It should be possible to switch from a selection to an any other
        //   selection.  After a switch to a new selection the value contained
        //   with in the ChoiceArrayItem should equal the corresponding
        //   null value of the newly selected type.  Switching a selection
        //   to the same selection should also result in a null value.
        //
        // Plan:
        //   Specify a set S of (unique) objects.  For each j in S, construct a
        //   control x from spec j, Assert that x is 'unSet' and initialized to
        //   the correct NULL value of the corresponding selection type.  For
        //   each k in S, change the selection of x to the selection
        //   described by spec k.  Assert that the selection changed to the
        //   correct type and x remained 'unSet'.  Set x to a value and assert
        //   that the selection type did not change and the x 'isNonNull'.
        //   Replace x with a new value and assert that it is still set.
        //   Finally, call makeSelect for the same k spec and assert that x is
        //   now 'isNull'.  For each transformation, verify that no memory is
        //   leaked and that the correct number constructors and destructors
        //   are called.
        //
        // Testing:
        //   void *makeSelection(int index);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting makeSelection Functions"
                               << "\n==============================="
                               << bsl::endl;
        static const struct TestRow {
          int         d_line;
          const char *d_catalogSpec;  // Specification to create the catalog
        } DATA[] = {
          // Line       DescriptorSpec
          // ====       ==============
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
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < 4; ++i) {
            AggOption::AllocationStrategy mode =
                                             (AggOption::AllocationStrategy) i;

            if (veryVerbose) {
                bsl::cout << "AllocationStrategy "; P(mode);
            }

            for (int j = 0; j < NUM_DATA; ++j) {
                const int    JLINE = DATA[j].d_line;
                const char  *JSPEC = DATA[j].d_catalogSpec;
                const Desc  *JDESC = getDescriptor(*JSPEC);
                EType::Type  JTYPE = getElemType(*JSPEC);

                // explicit allocation
                bslma_TestAllocator testAllocator(veryVeryVerbose);

                BEGIN_BSLMA_EXCEPTION_TEST {
                  bdema_SequentialAllocator  seqAllocator(&testAllocator);
                  bslma_Allocator           *alloc = 0;

                  if (i < 2) {
                      alloc = &testAllocator;
                  }
                  else {
                      alloc = &seqAllocator;
                  }

                  DescCatalog catalog(alloc);
                  ggCatalog(SPECIFICATIONS, catalog);

                  for (int k = 0; k < NUM_DATA; ++k) {
                      const int    KLINE = DATA[j].d_line;
                      const char  *KSPEC = DATA[k].d_catalogSpec;;
                      const Desc  *KDESC = getDescriptor(*KSPEC);
                      EType::Type  KTYPE = getElemType(*KSPEC);

                      CHead mXCH(&catalog, mode);
                      Obj &X = reinterpret_cast<Obj&>(mXCH);

                      if (veryVerbose) {
                          N_; P_(j); P_(JSPEC); P(X);
                      }
                      if (veryVeryVerbose) {
                          bsl::cout << "\nSPECIFIED ALLOCATOR:" << bsl::endl;
                          testAllocator.print();
                          bsl::cout << "\nDEFAULT ALLOCATOR:" << bsl::endl;
                          defaultAllocator.print();
                      }

                      LOOP_ASSERT(defaultAllocator.numBlocksTotal(),
                                  0 == defaultAllocator.numBlocksTotal());

                      LOOP2_ASSERT(JLINE, j,
                                   catalog.size() == X.numSelections());
                      LOOP2_ASSERT(JLINE, j,
                                   EType::BDEM_VOID == X.selectionType());

                      bdem_ElemRef XELEM = X.makeSelection(j - 1);
                      LOOP2_ASSERT(JLINE, JSPEC, isUnset(XELEM));
                      LOOP2_ASSERT(JLINE, JSPEC, XELEM.isNull());
                      LOOP2_ASSERT(JLINE, JSPEC, JTYPE == XELEM.type());
                      LOOP2_ASSERT(JLINE, JSPEC, JDESC == XELEM.descriptor());
                      if (XELEM.type() != EType::BDEM_VOID) {
                          const void *VAL_XN  = getValueN(*JSPEC);

                          const int NULLBIT = 1;
                          int nullBit = 1;
                          const ERef EREF_XN(const_cast<void *>(VAL_XN),
                                             JDESC, &nullBit, 0);
                          const CERef CEREF_XN(VAL_XN, JDESC, &NULLBIT, 0);

                          LOOP2_ASSERT(JLINE, JSPEC, EREF_XN == XELEM);
                      }

                      if (EType::BDEM_VOID == KTYPE) {
                          bdem_ElemRef AELEM = X.makeSelection(k - 1);
                          LOOP2_ASSERT(KLINE, KSPEC, isUnset(AELEM));
                          LOOP2_ASSERT(KLINE, KSPEC, XELEM.isNull());
                          LOOP2_ASSERT(KLINE, KSPEC, KTYPE == AELEM.type());
                          LOOP2_ASSERT(KLINE, KSPEC,
                                       KDESC == AELEM.descriptor());
                          continue;                                  //CONTINUE
                      }

                      const void *VAL_A  = getValueA(*KSPEC);
                      const void *VAL_B  = getValueB(*KSPEC);
                      const void *VAL_N  = getValueN(*KSPEC);

                      const bdem_ElemRef      EREF_A(const_cast<void *>(VAL_A),
                                                     KDESC);
                      const bdem_ConstElemRef CEREF_A(VAL_A, KDESC);

                      const bdem_ElemRef      EREF_B(const_cast<void *>(VAL_B),
                                                     KDESC);
                      const bdem_ConstElemRef CEREF_B(VAL_B, KDESC);

                      const int NULLBIT = 1;
                      int       nullBit = 1;
                      const bdem_ElemRef      EREF_N(const_cast<void *>(VAL_N),
                                                     KDESC,
                                                     &nullBit, 0);
                      const bdem_ConstElemRef CEREF_N(VAL_N, KDESC,
                                                      &NULLBIT, 0);

                      bdem_ElemRef AELEM = X.makeSelection(k - 1);

                      if (veryVerbose) {
                          N_; T_; P_(j); P_(k); P_(JSPEC); P_(KSPEC); P(X);
                      }
                      if (veryVeryVerbose) {
                          bsl::cout << "\nSPECIFIED ALLOCATOR:" << bsl::endl;
                          testAllocator.print();
                          bsl::cout << "\nDEFAULT ALLOCATOR:" << bsl::endl;
                          defaultAllocator.print();
                      }
                      LOOP_ASSERT(defaultAllocator.numBlocksTotal(),
                                  0 == defaultAllocator.numBlocksTotal());

                      LOOP2_ASSERT(KLINE, KSPEC, AELEM.isNull());
                      LOOP2_ASSERT(KLINE, KSPEC, isUnset(AELEM));
                      LOOP2_ASSERT(KLINE, KSPEC, KTYPE == AELEM.type());
                      LOOP2_ASSERT(KLINE, KSPEC, KDESC == AELEM.descriptor());
                      LOOP2_ASSERT(KLINE, KSPEC,
                                   catalog.size() == X.numSelections());

                      AELEM.replaceValue(CEREF_A);

                      if (veryVerbose) {
                          N_; T_; T_; P_(EREF_A); P(X);
                      }
                      if (veryVeryVerbose) {
                          bsl::cout << "\nSPECIFIED ALLOCATOR:" << bsl::endl;
                          testAllocator.print();
                          bsl::cout << "\nDEFAULT ALLOCATOR:" << bsl::endl;
                          defaultAllocator.print();
                      }
                      LOOP_ASSERT(defaultAllocator.numBlocksTotal(),
                                  0 == defaultAllocator.numBlocksTotal());

                      LOOP2_ASSERT(KLINE, KSPEC, !AELEM.isNull());
                      LOOP2_ASSERT(KLINE, KSPEC, KTYPE == AELEM.type());
                      LOOP2_ASSERT(KLINE, KSPEC, KDESC == AELEM.descriptor());
                      LOOP2_ASSERT(KLINE, KSPEC, k - 1    == X.selector());
                      LOOP2_ASSERT(KLINE, KSPEC,
                                   KTYPE == X.selectionType());
                      LOOP2_ASSERT(KLINE, KSPEC, EREF_A   == X.selection());
                      LOOP2_ASSERT(KLINE, KSPEC, CEREF_A  == X.selection());
                      LOOP2_ASSERT(KLINE, KSPEC,
                                   KTYPE == X.selectionType(k - 1));
                      LOOP2_ASSERT(KLINE, KSPEC, catalog.size()
                                                         == X.numSelections());

                      AELEM.replaceValue(CEREF_B);

                      if (veryVerbose) {
                          N_; T_; T_; P_(EREF_B); P(X);
                      }
                      if (veryVeryVerbose) {
                          bsl::cout << "\nSPECIFIED ALLOCATOR:" << bsl::endl;
                          testAllocator.print();
                          bsl::cout << "\nDEFAULT ALLOCATOR:" << bsl::endl;
                          defaultAllocator.print();
                      }
                      LOOP_ASSERT(defaultAllocator.numBlocksTotal(),
                                  0 == defaultAllocator.numBlocksTotal());

                      // special check for bool EREF_B == CEREF_N; unset
                      if (EType::BDEM_BOOL != KTYPE
                       && EType::BDEM_BOOL_ARRAY != KTYPE) {
                          LOOP2_ASSERT(KLINE, KSPEC, AELEM.isNonNull());
                      }
                      LOOP2_ASSERT(KLINE, KSPEC, !AELEM.isNull());
                      LOOP2_ASSERT(KLINE, KSPEC, KTYPE == AELEM.type());
                      LOOP2_ASSERT(KLINE, KSPEC, KDESC == AELEM.descriptor());
                      LOOP2_ASSERT(KLINE, KSPEC, k - 1    == X.selector());
                      LOOP2_ASSERT(KLINE, KSPEC,
                                   KTYPE == X.selectionType());
                      LOOP2_ASSERT(KLINE, KSPEC, EREF_B   == X.selection());
                      LOOP2_ASSERT(KLINE, KSPEC, CEREF_B  == X.selection());
                      LOOP2_ASSERT(KLINE, KSPEC, KTYPE
                                   == X.selectionType(k - 1));
                      LOOP2_ASSERT(KLINE, KSPEC, catalog.size()
                                   == X.numSelections());

                      bdem_ElemRef BELEM = X.makeSelection(k - 1);

                      if (veryVerbose) {
                          N_; T_; T_; P_(EREF_N); P(X);
                      }
                      if (veryVeryVerbose) {
                          bsl::cout << "\nSPECIFIED ALLOCATOR:" << bsl::endl;
                          testAllocator.print();
                          bsl::cout << "\nDEFAULT ALLOCATOR:" << bsl::endl;
                          defaultAllocator.print();
                      }
                      LOOP_ASSERT(defaultAllocator.numBlocksTotal(),
                                  0 == defaultAllocator.numBlocksTotal());

                      LOOP2_ASSERT(KLINE, KSPEC, BELEM.isNull());
                      LOOP2_ASSERT(KLINE, KSPEC, isUnset(BELEM));
                      LOOP2_ASSERT(KLINE, KSPEC, EREF_N == BELEM);
                      LOOP2_ASSERT(KLINE, KSPEC, KTYPE  == BELEM.type());
                      LOOP2_ASSERT(KLINE, KSPEC, KDESC  == BELEM.descriptor());
                      LOOP2_ASSERT(KLINE, KSPEC, k - 1    == X.selector());
                      LOOP2_ASSERT(KLINE, KSPEC,
                                   KTYPE == X.selectionType());
                      LOOP2_ASSERT(KLINE, KSPEC, EREF_N   == X.selection());
                      LOOP2_ASSERT(KLINE, KSPEC, CEREF_N  == X.selection());
                      LOOP2_ASSERT(KLINE, KSPEC,
                                   KTYPE == X.selectionType(k - 1));
                      LOOP2_ASSERT(KLINE, KSPEC, catalog.size()
                                   == X.numSelections());
                  }
                } END_BSLMA_EXCEPTION_TEST
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'bdex' STREAMING FUNCTIONALITY:
        //
        // Concerns:
        //   The 'bdex' streaming concerns for this component are standard.  We
        //   first test the class method 'maxSupportedBdexVersion' and then
        //   use that method to probe the member functions 'outStream' and
        //   'inStream' in the manner of a "breathing test" to verify basic
        //   functionality.  We then thoroughly test streaming functionality
        //   relying on the overloaded template free operators '<<' and '>>'
        //   provided by the concrete streams, which forward appropriate calls
        //   to the member functions of this component.  We next step through
        //   the sequence of possible stream states (valid, empty, invalid,
        //   incomplete, and corrupted), appropriately selecting data sets as
        //   described below.  In all cases, exception neutrality is confirmed
        //   using the specially instrumented 'bdex_TestInStream' and a pair of
        //   standard macros, 'BEGIN_BDEX_EXCEPTION_TEST' and
        //   'END_BDEX_EXCEPTION_TEST', which configure the
        //   'bdex_TestInStream' object appropriately in a loop.
        //
        // Plan:
        //   PRELIMINARY MEMBER FUNCTION TEST
        //     First test 'maxSupportedBdexVersion' explicitly, and then
        //     perform a trivial direct test of the 'outStream' and 'inStream'
        //     methods (the rest of the testing will use the stream operators).
        //
        //   VALID STREAMS
        //     For the set S of globally-defined test values, use all
        //     combinations (u, v) in the cross product S X S, stream the
        //     value of v into (a temporary copy of) u and assert u == v.
        //
        //   EMPTY AND INVALID STREAMS
        //     For each u in S, create a copy and attempt to stream into it
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
        //   Finally, tests of the explicit wire format will be performed.
        //
        // Testing:
        //   int maxSupportedBdexVersion() const;
        //   bdex_InStream& streamIn(bdex_InStream& stream);
        //   bdex_InStream& streamIn(bdex_InStream& stream, int version);
        //   bdex_OutStream& streamOut(bdex_OutStream& stream) const;
        //   bdex_OutStream& streamOut(bdex_OutStream& stream, int) const;
        //   operator>>(bdex_InStream&, bdem_Choice&);
        //   operator<<(bdex_OutStream&, const bdem_Choice&);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Streaming Functionality"
                               << "\n==============================="
                               << bsl::endl;

        if (veryVerbose) {
            bsl::cout << "\tTesting maxSupportedBdexVersion" << bsl::endl;
        }

        {
            ASSERT(3 == Obj::maxSupportedBdexVersion());
        }

        static const struct TestRow {
          int         d_line;
          const char *d_catalogSpec;  // Specification to create the catalog
          bool        d_originalIsNull;
          bool        d_streamedIsNull;
        } DATA[] = {
          // Line     DescriptorSpec
          // ====     ==============
          { L_,       ""                },
          { L_,       "A", false, false },
          { L_,       "A", true , false },
          { L_,       "A", false, true  },
          { L_,       "A", true , true  },

          { L_,       "B", false, false },
          { L_,       "B", true , false },
          { L_,       "B", false, true  },
          { L_,       "B", true , true  },

          { L_,       "C", false, false },
          { L_,       "C", true , false },
          { L_,       "C", false, true  },
          { L_,       "C", true , true  },

          { L_,       "D", false, false },
          { L_,       "D", true , false },
          { L_,       "D", false, true  },
          { L_,       "D", true , true  },

          { L_,       "E", false, false },
          { L_,       "E", true , false },
          { L_,       "E", false, true  },
          { L_,       "E", true , true  },

          { L_,       "F", false, false },
          { L_,       "F", true , false },
          { L_,       "F", false, true  },
          { L_,       "F", true , true  },

          { L_,       "G", false, false },
          { L_,       "G", true , false },
          { L_,       "G", false, true  },
          { L_,       "G", true , true  },

          { L_,       "H", false, false },
          { L_,       "H", true , false },
          { L_,       "H", false, true  },
          { L_,       "H", true , true  },

          { L_,       "I", false, false },
          { L_,       "I", true , false },
          { L_,       "I", false, true  },
          { L_,       "I", true , true  },

          { L_,       "J", false, false },
          { L_,       "J", true , false },
          { L_,       "J", false, true  },
          { L_,       "J", true , true  },

          { L_,       "K", false, false },
          { L_,       "K", true , false },
          { L_,       "K", false, true  },
          { L_,       "K", true , true  },

          { L_,       "L", false, false },
          { L_,       "L", true , false },
          { L_,       "L", false, true  },
          { L_,       "L", true , true  },

          { L_,       "M", false, false },
          { L_,       "M", true , false },
          { L_,       "M", false, true  },
          { L_,       "M", true , true  },

          { L_,       "N", false, false },
          { L_,       "N", true , false },
          { L_,       "N", false, true  },
          { L_,       "N", true , true  },

          { L_,       "O", false, false },
          { L_,       "O", true , false },
          { L_,       "O", false, true  },
          { L_,       "O", true , true  },

          { L_,       "P", false, false },
          { L_,       "P", true , false },
          { L_,       "P", false, true  },
          { L_,       "P", true , true  },

          { L_,       "Q", false, false },
          { L_,       "Q", true , false },
          { L_,       "Q", false, true  },
          { L_,       "Q", true , true  },

          { L_,       "R", false, false },
          { L_,       "R", true , false },
          { L_,       "R", false, true  },
          { L_,       "R", true , true  },

          { L_,       "S", false, false },
          { L_,       "S", true , false },
          { L_,       "S", false, true  },
          { L_,       "S", true , true  },

          { L_,       "T", false, false },
          { L_,       "T", true , false },
          { L_,       "T", false, true  },
          { L_,       "T", true , true  },

          { L_,       "U", false, false },
          { L_,       "U", true , false },
          { L_,       "U", false, true  },
          { L_,       "U", true , true  },

          { L_,       "V", false, false },
          { L_,       "V", true , false },
          { L_,       "V", false, true  },
          { L_,       "V", true , true  },

          { L_,       "W", false, false },
          { L_,       "W", true , false },
          { L_,       "W", false, true  },
          { L_,       "W", true , true  },

          { L_,       "X", false, false },
          { L_,       "X", true , false },
          { L_,       "X", false, true  },
          { L_,       "X", true , true  },

          { L_,       "Y", false, false },
          { L_,       "Y", true , false },
          { L_,       "Y", false, true  },
          { L_,       "Y", true , true  },

          { L_,       "Z", false, false },
          { L_,       "Z", true , false },
          { L_,       "Z", false, true  },
          { L_,       "Z", true , true  },

          { L_,       "a", false, false },
          { L_,       "a", true , false },
          { L_,       "a", false, true  },
          { L_,       "a", true , true  },

          { L_,       "b", false, false },
          { L_,       "b", true , false },
          { L_,       "b", false, true  },
          { L_,       "b", true , true  },

          { L_,       "c", false, false },
          { L_,       "c", true , false },
          { L_,       "c", false, true  },
          { L_,       "c", true , true  },

          { L_,       "d", false, false },
          { L_,       "d", true , false },
          { L_,       "d", false, true  },
          { L_,       "d", true , true  },

          { L_,       "e", false, false },
          { L_,       "e", true , false },
          { L_,       "e", false, true  },
          { L_,       "e", true , true  },

          { L_,       "f", false, false },
          { L_,       "f", true , false },
          { L_,       "f", false, true  },
          { L_,       "f", true , true  },
          // Testing various interesting combinations
          { L_,       "AA", false, false },
          { L_,       "AA", true , false },
          { L_,       "AA", false, true  },
          { L_,       "AA", true , true  },

          { L_,       "AC", false, false },
          { L_,       "AC", true , false },
          { L_,       "AC", false, true  },
          { L_,       "AC", true , true  },

          { L_,       "CD", false, false },
          { L_,       "CD", true , false },
          { L_,       "CD", false, true  },
          { L_,       "CD", true , true  },

          { L_,       "CE", false, false },
          { L_,       "CE", true , false },
          { L_,       "CE", false, true  },
          { L_,       "CE", true , true  },

          { L_,       "FG", false, false },
          { L_,       "FG", true , false },
          { L_,       "FG", false, true  },
          { L_,       "FG", true , true  },

          { L_,       "GG", false, false },
          { L_,       "GG", true , false },
          { L_,       "GG", false, true  },
          { L_,       "GG", true , true  },

          { L_,       "GH", false, false },
          { L_,       "GH", true , false },
          { L_,       "GH", false, true  },
          { L_,       "GH", true , true  },

          { L_,       "MN", false, false },
          { L_,       "MN", true , false },
          { L_,       "MN", false, true  },
          { L_,       "MN", true , true  },

          { L_,       "OP", false, false },
          { L_,       "OP", true , false },
          { L_,       "OP", false, true  },
          { L_,       "OP", true , true  },

          { L_,       "PQ", false, false },
          { L_,       "PQ", true , false },
          { L_,       "PQ", false, true  },
          { L_,       "PQ", true , true  },

          { L_,       "KQ", false, false },
          { L_,       "KQ", true , false },
          { L_,       "KQ", false, true  },
          { L_,       "KQ", true , true  },

          { L_,       "ST", false, false },
          { L_,       "ST", true , false },
          { L_,       "ST", false, true  },
          { L_,       "ST", true , true  },

          { L_,       "WX", false, false },
          { L_,       "WX", true , false },
          { L_,       "WX", false, true  },
          { L_,       "WX", true , true  },

          { L_,       "UV", false, false },
          { L_,       "UV", true , false },
          { L_,       "UV", false, true  },
          { L_,       "UV", true , true  },

          { L_,       "YZ", false, false },
          { L_,       "YZ", true , false },
          { L_,       "YZ", false, true  },
          { L_,       "YZ", true , true  },

          { L_,       "ab", false, false },
          { L_,       "ab", true , false },
          { L_,       "ab", false, true  },
          { L_,       "ab", true , true  },

          { L_,       "cd", false, false },
          { L_,       "cd", true , false },
          { L_,       "cd", false, true  },
          { L_,       "cd", true , true  },

          { L_,       "ef", false, false },
          { L_,       "ef", true , false },
          { L_,       "ef", false, true  },
          { L_,       "ef", true , true  },

          { L_,       "Ue", false, false },
          { L_,       "Ue", true , false },
          { L_,       "Ue", false, true  },
          { L_,       "Ue", true , true  },

          { L_,       "Vf", false, false },
          { L_,       "Vf", true , false },
          { L_,       "Vf", false, true  },
          { L_,       "Vf", true , true  },

          { L_,       "HIJ", false, false },
          { L_,       "HIJ", true , false },
          { L_,       "HIJ", false, true  },
          { L_,       "HIJ", true , true  },

          { L_,       "KLM", false, false },
          { L_,       "KLM", true , false },
          { L_,       "KLM", false, true  },
          { L_,       "KLM", true , true  },

          { L_,       "RST", false, false },
          { L_,       "RST", true , false },
          { L_,       "RST", false, true  },
          { L_,       "RST", true , true  },

          { L_,       "YZa", false, false },
          { L_,       "YZa", true , false },
          { L_,       "YZa", false, true  },
          { L_,       "YZa", true , true  },

          { L_,       "BLRW", false, false },
          { L_,       "BLRW", true , false },
          { L_,       "BLRW", false, true  },
          { L_,       "BLRW", true , true  },

          { L_,       "DGNQ", false, false },
          { L_,       "DGNQ", true , false },
          { L_,       "DGNQ", false, true  },
          { L_,       "DGNQ", true , true  },

          { L_,       "QRST", false, false },
          { L_,       "QRST", true , false },
          { L_,       "QRST", false, true  },
          { L_,       "QRST", true , true  },

          { L_,       "JHKHSK", false, false },
          { L_,       "JHKHSK", true , false },
          { L_,       "JHKHSK", false, true  },
          { L_,       "JHKHSK", true , true  },

          { L_,       "RISXLSW", false, false },
          { L_,       "RISXLSW", true , false },
          { L_,       "RISXLSW", false, true  },
          { L_,       "RISXLSW", true , true  },

          { L_,       "MXKZOLPR", false, false },
          { L_,       "MXKZOLPR", true , false },
          { L_,       "MXKZOLPR", false, true  },
          { L_,       "MXKZOLPR", true , true  },

          { L_,       "GGGGGGGGG", false, false },
          { L_,       "GGGGGGGGG", true , false },
          { L_,       "GGGGGGGGG", false, true  },
          { L_,       "GGGGGGGGG", true , true  },

          { L_,       "QQQQQQQQQQ", false, false },
          { L_,       "QQQQQQQQQQ", true , false },
          { L_,       "QQQQQQQQQQ", false, true  },
          { L_,       "QQQQQQQQQQ", true , true  },

          { L_,       "abcdABCDEFG", false, false },
          { L_,       "abcdABCDEFG", true , false },
          { L_,       "abcdABCDEFG", false, true  },
          { L_,       "abcdABCDEFG", true , true  },

          { L_,       "FDLALAabADSF", false, false },
          { L_,       "FDLALAabADSF", true , false },
          { L_,       "FDLALAabADSF", false, true  },
          { L_,       "FDLALAabADSF", true , true  },

          { L_,       "GSRWSASDEFDSA", false, false },
          { L_,       "GSRWSASDEFDSA", true , false },
          { L_,       "GSRWSASDEFDSA", false, true  },
          { L_,       "GSRWSASDEFDSA", true , true  },

          { L_,       "PKJHALKGabASDA", false, false },
          { L_,       "PKJHALKGabASDA", true , false },
          { L_,       "PKJHALKGabASDA", false, true  },
          { L_,       "PKJHALKGabASDA", true , true  },

          { L_,       "XZSAEWRPOIJLKaAS", false, false },
          { L_,       "XZSAEWRPOIJLKaAS", true , false },
          { L_,       "XZSAEWRPOIJLKaAS", false, true  },
          { L_,       "XZSAEWRPOIJLKaAS", true , true  },

          { L_,       "GALKacKASJDKSWEIO", false, false },
          { L_,       "GALKacKASJDKSWEIO", true , false },
          { L_,       "GALKacKASJDKSWEIO", false, true  },
          { L_,       "GALKacKASJDKSWEIO", true , true  },

          { L_,       "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef", false, false },
          { L_,       "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef", true , false },
          { L_,       "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef", false, true  },
          { L_,       "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef", true , true  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;
        const int MAX_VERSION = Obj::maxSupportedBdexVersion();

        ASSERT(3 == MAX_VERSION);

        for (int VERSION = 1; VERSION < MAX_VERSION; ++VERSION) {
            for (int i1 = 0; i1 < NUM_DATA; ++i1) {
                const int   LINE1  = DATA[i1].d_line;
                const char *SPEC1  = DATA[i1].d_catalogSpec;
                const int   LEN1   = bsl::strlen(SPEC1);
                const bool  XISNULL = DATA[i1].d_originalIsNull;
                const bool  AISNULL = DATA[i1].d_streamedIsNull;

                if (veryVerbose) {
                    P(SPEC1);
                }

                for (int j1 = 0; j1 < LEN1; ++j1) {
                    const char  S1   = SPEC1[j1];
                    const CERef C1_A = CERef(getValueA(S1), getDescriptor(S1));
                    const CERef C1_B = CERef(getValueB(S1), getDescriptor(S1));

                    bslma_TestAllocator alloc1(veryVeryVerbose);
                    DescCatalog catalog1(&alloc1);  ggCatalog(SPEC1, catalog1);
                    CHead mX(&catalog1);
                    Obj &X = reinterpret_cast<Obj&>(mX);
                    X.makeSelection(j1).replaceValue(C1_A);
                    if (XISNULL) {
                        X.selection().makeNull();
                        LOOP_ASSERT(LINE1, X.selection().isNull() == XISNULL);
                    }

                    A_(alloc1);
                    LOOP_ASSERT(defaultAllocator.numBlocksInUse(),
                                0 == defaultAllocator.numBlocksInUse());

                    bdex_TestOutStream os1(&alloc1);
                    bdex_TestOutStream os2(&alloc1);

                    X.bdexStreamOut(os1, VERSION);
                    bdex_OutStreamFunctions::streamOut(
                                                  os2,
                                                  X,
                                                  X.maxSupportedBdexVersion());

                    A_(alloc1);
                    LOOP_ASSERT(defaultAllocator.numBlocksInUse(),
                                0 == defaultAllocator.numBlocksInUse());

                    // Stream a constructed obj to an empty obj
                    {
                        bslma_TestAllocator tmpAlloc(veryVeryVerbose);

                        DescCatalog catalogA(&tmpAlloc);
                        catalogA = catalog1;
                        CHead mA(&catalogA);
                        Obj &A = reinterpret_cast<Obj&>(mA);

                        DescCatalog catalogB(&tmpAlloc);
                        catalogB = catalog1;
                        CHead mB(&catalogB);
                        Obj &B = reinterpret_cast<Obj&>(mB);

                        if (veryVerbose) {
                            P(VERSION); P(X); P(A); P(B);
                        }

                        LOOP_ASSERT(LINE1, X != A);
                        LOOP_ASSERT(LINE1, X != B);

                        A_(tmpAlloc);
                        LOOP_ASSERT(defaultAllocator.numBlocksInUse(),
                                    0 == defaultAllocator.numBlocksInUse());

                        bdex_TestInStream is1(os1.data(),
                                              os1.length(),
                                              &tmpAlloc);
                        bdex_TestInStream is2(os2.data(),
                                              os2.length(),
                                              &tmpAlloc);

                        is1.setSuppressVersionCheck(1);
                        is2.setSuppressVersionCheck(1);
                        A.bdexStreamIn(is1, VERSION);
                        bdex_InStreamFunctions::streamIn(
                                                  is2,
                                                  B,
                                                  B.maxSupportedBdexVersion());

                        if (veryVerbose) {
                            P(X); P(A); P(B);
                        }

                        if (XISNULL) {
                            LOOP_ASSERT(LINE1, B.selection().isNull());
                        }
                        else {
                            LOOP_ASSERT(LINE1, !B.selection().isNull());
                        }

                        LOOP_ASSERT(LINE1, compare(X.selection(),
                                                   B.selection(),
                                                   S1));
                        if (VERSION < 3) {
                            LOOP_ASSERT(LINE1, !A.selection().isNull());
                            LOOP_ASSERT(LINE1, compare(X.selection(),
                                                       A.selection(),
                                                       S1));
                        }
                        else {
                            LOOP_ASSERT(LINE1,
                                        A.selection().isNull() == XISNULL);
                            LOOP_ASSERT(LINE1, X == A);
                        }

                        A_(tmpAlloc);
                        LOOP_ASSERT(defaultAllocator.numBlocksInUse(),
                                    0 == defaultAllocator.numBlocksInUse());
                    }

                    // Stream a constructed obj to a non-empty obj
                    {
                        bslma_TestAllocator tmpAlloc(veryVeryVerbose);

                        DescCatalog catalogA(&tmpAlloc);
                        catalogA = catalog1;
                        CHead mA(&catalogA);
                        Obj &A = reinterpret_cast<Obj&>(mA);
                        A.makeSelection(j1).replaceValue(C1_B);

                        DescCatalog catalogB(&tmpAlloc);
                        catalogB= catalog1;
                        CHead mB(&catalogB);
                        Obj &B = reinterpret_cast<Obj&>(mB);
                        B.makeSelection(j1).replaceValue(C1_B);

                        if (AISNULL) { A.selection().makeNull(); }
                        LOOP_ASSERT(LINE1, A.selection().isNull() == AISNULL);
                        if (AISNULL) { B.selection().makeNull(); }
                        LOOP_ASSERT(LINE1, B.selection().isNull() == AISNULL);

                        if (veryVerbose) {
                            P(X); P(A); P(B);
                        }

                        if (true != XISNULL && true != AISNULL) {
                            LOOP_ASSERT(LINE1, X != A);
                            LOOP_ASSERT(LINE1, X != B);
                        }

                        A_(tmpAlloc);
                        LOOP_ASSERT(defaultAllocator.numBlocksInUse(),
                                    0 == defaultAllocator.numBlocksInUse());

                        bdex_TestInStream is1(os1.data(),
                                              os1.length(),
                                              &tmpAlloc);
                        bdex_TestInStream is2(os2.data(),
                                              os2.length(),
                                              &tmpAlloc);

                        is1.setSuppressVersionCheck(1);
                        is2.setSuppressVersionCheck(1);
                        A.bdexStreamIn(is1, VERSION);
                        bdex_InStreamFunctions::streamIn(
                                                  is2,
                                                  B,
                                                  B.maxSupportedBdexVersion());

                        if (veryVerbose) {
                            P(X); P(A); P(B);
                        }

                        if (XISNULL) {
                            LOOP_ASSERT(LINE1, B.selection().isNull());
                        }
                        else {
                            LOOP_ASSERT(LINE1, !B.selection().isNull());
                        }
                        LOOP_ASSERT(LINE1, compare(X.selection(),
                                                   B.selection(),
                                                   S1));
                        if (VERSION < 3) {
                            LOOP_ASSERT(LINE1, !A.selection().isNull());
                            LOOP_ASSERT(LINE1, compare(X.selection(),
                                                       A.selection(),
                                                       S1));
                        }
                        else {
                            LOOP_ASSERT(LINE1,
                                        A.selection().isNull() == XISNULL);
                            LOOP_ASSERT(LINE1, X == A);
                        }

                        A_(tmpAlloc);
                        LOOP_ASSERT(defaultAllocator.numBlocksInUse(),
                                    0 == defaultAllocator.numBlocksInUse());
                    }

                    // Stream from an empty and invalid stream
                    {
                        bslma_TestAllocator tmpAlloc(veryVeryVerbose);

                        DescCatalog catalogA(&tmpAlloc);
                        ggCatalog(SPEC1, catalogA);
                        CHead mA(&catalogA);
                        Obj &A = reinterpret_cast<Obj&>(mA);
                        A.makeSelection(j1).replaceValue(C1_A);

                        DescCatalog catalogB(&tmpAlloc);
                        catalogB = catalogA;
                        CHead mB(&catalogB);
                        Obj &B = reinterpret_cast<Obj&>(mB);
                        B.makeSelection(j1).replaceValue(C1_A);

                        if (AISNULL) { A.selection().makeNull(); }
                        LOOP_ASSERT(LINE1, A.selection().isNull() == AISNULL);
                        if (AISNULL) { B.selection().makeNull(); }
                        LOOP_ASSERT(LINE1, B.selection().isNull() == AISNULL);

                        if (veryVerbose) {
                            P(X); P(A); P(B);
                        }

                        if (XISNULL == AISNULL) {
                            LOOP_ASSERT(LINE1, A == X);
                            LOOP_ASSERT(LINE1, B == X);
                        }
                        else {
                            LOOP_ASSERT(LINE1, A != X);
                            LOOP_ASSERT(LINE1, B != X);
                        }

                        A_(tmpAlloc);
                        LOOP_ASSERT(defaultAllocator.numBlocksInUse(),
                                    0 == defaultAllocator.numBlocksInUse());

                        bdex_TestInStream is1(&tmpAlloc);  // Empty stream
                        bdex_TestInStream is2(&tmpAlloc);  // Empty stream

                        is1.setSuppressVersionCheck(1);
                        is2.setSuppressVersionCheck(1);
                        A.bdexStreamIn(is1, 1);
                        bdex_InStreamFunctions::streamIn(
                                                  is2,
                                                  B,
                                                  B.maxSupportedBdexVersion());

                        if (XISNULL == AISNULL) {
                            LOOP_ASSERT(LINE1, A == X);
                            LOOP_ASSERT(LINE1, B == X);
                        }
                        else {
                            LOOP_ASSERT(LINE1, A != X);
                            LOOP_ASSERT(LINE1, B != X);
                        }
                        if (veryVerbose) {
                            P(X); P(A); P(B);
                        }

                        ASSERT(!is1);
                        ASSERT(!is2);

                        A_(tmpAlloc);
                        LOOP_ASSERT(defaultAllocator.numBlocksInUse(),
                                    0 == defaultAllocator.numBlocksInUse());

                        bdex_TestInStream is3(os2.data(),
                                              os2.length(),
                                              &tmpAlloc);
                        is3.invalidate();  // Invalid stream
                        bdex_TestInStream is4(os2.data(),
                                              os2.length(),
                                              &tmpAlloc);
                        is4.invalidate();  // Invalid stream

                        is3.setSuppressVersionCheck(1);
                        is4.setSuppressVersionCheck(1);
                        A.bdexStreamIn(is3, 1);
                        bdex_InStreamFunctions::streamIn(
                                                  is4,
                                                  B,
                                                  B.maxSupportedBdexVersion());

                        if (veryVerbose) {
                            P(X); P(A); P(B);
                        }

                        if (XISNULL == AISNULL) {
                            LOOP_ASSERT(LINE1, A == X);
                            LOOP_ASSERT(LINE1, B == X);
                        }
                        else {
                            LOOP_ASSERT(LINE1, A != X);
                            LOOP_ASSERT(LINE1, B != X);
                        }

                        ASSERT(!is3);
                        ASSERT(!is4);

                        A_(tmpAlloc);
                        LOOP_ASSERT(defaultAllocator.numBlocksInUse(),
                                    0 == defaultAllocator.numBlocksInUse());
                    }

                    // Stream an empty obj to a constructed obj
                    {
                        bslma_TestAllocator tmpAlloc(veryVeryVerbose);
                        DescCatalog catalogY(&tmpAlloc);
                        ggCatalog(SPEC1, catalogY);
                        CHead mY(&catalogY);
                        Obj &Y = reinterpret_cast<Obj&>(mY);
                        Y.makeSelection(j1).replaceValue(C1_A);

                        DescCatalog catalogA(&tmpAlloc);
                        catalogA = catalogY;
                        CHead mA(&catalogA);
                        Obj &A = reinterpret_cast<Obj&>(mA);

                        if (AISNULL && EType::BDEM_VOID != A.selectionType()) {
                            A.selection().makeNull();
                            LOOP_ASSERT(LINE1,
                                        A.selection().isNull() == AISNULL);
                        }
                        if (AISNULL && EType::BDEM_VOID != Y.selectionType()) {
                            Y.selection().makeNull();
                            LOOP_ASSERT(LINE1,
                                        Y.selection().isNull() == AISNULL);
                        }

                        if (veryVerbose) {
                            P(X); P(Y); P(A);
                        }

                        if (XISNULL == AISNULL) {
                            LOOP_ASSERT(LINE1, X == Y);
                        }
                        else {
                            LOOP_ASSERT(LINE1, X != Y);
                        }
                        LOOP_ASSERT(LINE1, X != A);

                        A_(tmpAlloc);
                        LOOP_ASSERT(defaultAllocator.numBlocksInUse(),
                                    0 == defaultAllocator.numBlocksInUse());

                        bdex_TestOutStream os1(&tmpAlloc);
                        bdex_TestOutStream os2(&tmpAlloc);

                        A.bdexStreamOut(os1, VERSION);
                        bdex_OutStreamFunctions::streamOut(
                                                  os2,
                                                  A,
                                                  A.maxSupportedBdexVersion());

                        bdex_TestInStream is1(os1.data(),
                                              os1.length(),
                                              &tmpAlloc);
                        bdex_TestInStream is2(os2.data(),
                                              os2.length(),
                                              &tmpAlloc);

                        is1.setSuppressVersionCheck(1);
                        is2.setSuppressVersionCheck(1);
                        X.bdexStreamIn(is1, VERSION);
                        bdex_InStreamFunctions::streamIn(
                                                  is2,
                                                  Y,
                                                  Y.maxSupportedBdexVersion());

                        if (veryVerbose) {
                            P(X); P(Y); P(A);
                        }

                        if (EType::BDEM_VOID != A.selectionType()) {
                            LOOP2_ASSERT(LINE1, XISNULL,
                                         A.selection().isNull() == AISNULL);
                        }

                        if (EType::BDEM_VOID != Y.selectionType()) {
                            LOOP4_ASSERT(LINE1, XISNULL, VERSION,
                                         Y.selection(),
                                         Y.selection().isNull() == AISNULL);
                        }

                        if (EType::BDEM_VOID != X.selectionType()) {
                            LOOP4_ASSERT(LINE1, XISNULL, VERSION,
                                         X.selection(),
                                         X.selection().isNull() == AISNULL);
                        }

                        LOOP_ASSERT(LINE1, X == A);
                        LOOP_ASSERT(LINE1, Y == A);

                        A_(tmpAlloc);
                        LOOP_ASSERT(defaultAllocator.numBlocksInUse(),
                                    0 == defaultAllocator.numBlocksInUse());
                    }
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
        //   operator==(const bdem_ChoiceArrayItem&,
        //              const bdem_ChoiceArrayItem&);
        //   operator!=(const bdem_ChoiceArrayItem&,
        //              const bdem_ChoiceArrayItem&);
        // --------------------------------------------------------------------

        if (verbose) {
          bsl::cout << "\nTesting Equality Operators"
                    << "\n=========================="
                    << bsl::endl;
        }

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
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

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

                bslma_TestAllocator alloc1(veryVeryVerbose);
                DescCatalog catalog1(&alloc1);  ggCatalog(SPEC1, catalog1);

                bslma_TestAllocator alloc2(veryVeryVerbose);
                DescCatalog catalog2(&alloc2);  ggCatalog(SPEC2, catalog2);

                CERef C1_A = CERef(getValueA(S1), getDescriptor(S1));
                CERef C1_B = CERef(getValueB(S1), getDescriptor(S1));
                CERef C2_A = CERef(getValueA(S2), getDescriptor(S2));
                CERef C2_B = CERef(getValueB(S2), getDescriptor(S2));

                if (veryVerbose) {
                  P(SPEC1); P(SPEC2);
                }

                CHead mX(&catalog1);
                Obj &X = reinterpret_cast<Obj&>(mX);
                X.makeSelection(j1).replaceValue(C1_A);

                CHead mY(&catalog2);
                Obj &Y = reinterpret_cast<Obj&>(mY);
                Y.makeSelection(j2).replaceValue(C2_A);

                if (i1 == i2 && j1 == j2) {
                  // This assumes that each spec is unique

                  LOOP2_ASSERT(LINE1, LINE2,    X == Y);
                  LOOP2_ASSERT(LINE1, LINE2,  !(X != Y));

                  // Some perturbation
                  Y.makeSelection(j2).replaceValue(C2_B);

                  LOOP2_ASSERT(LINE1, LINE2,    X != Y);
                  LOOP2_ASSERT(LINE1, LINE2,  !(X == Y));

                  Y.makeSelection(j2).replaceValue(C2_A);

                  LOOP2_ASSERT(LINE1, LINE2,    X == Y);
                  LOOP2_ASSERT(LINE1, LINE2,  !(X != Y));

                  X.makeSelection(j1).replaceValue(C1_B);

                  LOOP2_ASSERT(LINE1, LINE2,    X != Y);
                  LOOP2_ASSERT(LINE1, LINE2,  !(X == Y));

                  X.makeSelection(j1).replaceValue(C1_A);

                  LOOP2_ASSERT(LINE1, LINE2,    X == Y);
                  LOOP2_ASSERT(LINE1, LINE2,  !(X != Y));

                  Y.selection().makeNull();

                  LOOP2_ASSERT(LINE1, LINE2,   X != Y);
                  LOOP2_ASSERT(LINE1, LINE2, !(X == Y));

                  X.selection().makeNull();

                  LOOP2_ASSERT(LINE1, LINE2,   X == Y);
                  LOOP2_ASSERT(LINE1, LINE2, !(X != Y));

                  Y.makeSelection(-1);

                  LOOP2_ASSERT(LINE1, LINE2,   X != Y);
                  LOOP2_ASSERT(LINE1, LINE2, !(X == Y));

                  X.makeSelection(-1);

                  LOOP2_ASSERT(LINE1, LINE2,   X == Y);
                  LOOP2_ASSERT(LINE1, LINE2, !(X != Y));
                }
                else {
                  LOOP2_ASSERT(LINE1, LINE2,    X != Y);
                  LOOP2_ASSERT(LINE1, LINE2,  !(X == Y));

                  // Some perturbation
                  Y.makeSelection(j2).replaceValue(C2_B);

                  LOOP2_ASSERT(LINE1, LINE2,    X != Y);
                  LOOP2_ASSERT(LINE1, LINE2,  !(X == Y));

                  Y.makeSelection(j2).replaceValue(C2_A);

                  LOOP2_ASSERT(LINE1, LINE2,    X != Y);
                  LOOP2_ASSERT(LINE1, LINE2,  !(X == Y));

                  X.makeSelection(j1).replaceValue(C1_B);

                  LOOP2_ASSERT(LINE1, LINE2,    X != Y);
                  LOOP2_ASSERT(LINE1, LINE2,  !(X == Y));

                  X.makeSelection(j1).replaceValue(C1_A);

                  LOOP2_ASSERT(LINE1, LINE2,    X != Y);
                  LOOP2_ASSERT(LINE1, LINE2,  !(X == Y));

                  Y.selection().makeNull();

                  LOOP2_ASSERT(LINE1, LINE2,   X != Y);
                  LOOP2_ASSERT(LINE1, LINE2, !(X == Y));

                  X.selection().makeNull();

                  LOOP2_ASSERT(LINE1, LINE2,   X != Y);
                  LOOP2_ASSERT(LINE1, LINE2, !(X == Y));

                  Y.makeSelection(-1);

                  LOOP2_ASSERT(LINE1, LINE2,   X != Y);
                  LOOP2_ASSERT(LINE1, LINE2, !(X == Y));

                  X.makeSelection(-1);

                  if (i1 == i2) {
                      LOOP2_ASSERT(LINE1, LINE2,   X == Y);
                      LOOP2_ASSERT(LINE1, LINE2, !(X != Y));
                  }
                  else {
                      LOOP2_ASSERT(LINE1, LINE2,   X != Y);
                      LOOP2_ASSERT(LINE1, LINE2, !(X == Y));
                  }
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

        if (verbose) {
          bsl::cout << "\nTesting Output Functions"
                    << "\n========================"
                    << bsl::endl;
        }

        if (veryVerbose) {
          bsl::cout << "\n\nTesting empty choice\n" << bsl::endl;
        }

        {
          const char *EXP_P1 = "    {\n"
                               "        VOID NULL\n"
                               "    }\n";
          const char *EXP_P2 = "{\n"
                               "        VOID NULL\n"
                               "    }\n";
          const char *EXP_P3 = "    { VOID NULL }";
          const char *EXP_P4 = "{ VOID NULL }";
          const char *EXP_OP = "{ VOID NULL }";

          bslma_TestAllocator t1(veryVeryVerbose);
          DescCatalog catalog(&t1);
          CHead mXCH(&catalog);
          Obj &X = reinterpret_cast<Obj&>(mXCH);

          bsl::ostringstream os1, os2, os3, os4, os5;
          X.print(os1, 1, 4);
          X.print(os2, -1, 4);
          X.print(os3, 1, -4);
          X.print(os4, -1, -4);
          os5 << X;

          if (veryVerbose) {
            P(EXP_P1); P(os1.str());
          }
          LOOP2_ASSERT(EXP_P1,
                       os1.str(),
                       0 == bsl::strcmp(EXP_P1, os1.str().c_str()));

          if (veryVerbose) {
            P(EXP_P2); P(os2.str());
          }
          LOOP2_ASSERT(EXP_P2,
                       os2.str(),
                       0 == bsl::strcmp(EXP_P2, os2.str().c_str()));

          if (veryVerbose) {
            P(EXP_P3); P(os3.str());
          }
          LOOP2_ASSERT(EXP_P3,
                       os3.str(),
                       0 == bsl::strcmp(EXP_P3, os3.str().c_str()));

          if (veryVerbose) {
            P(EXP_P4); P(os4.str());
          }
          LOOP2_ASSERT(EXP_P4,
                       os4.str(),
                       0 == bsl::strcmp(EXP_P4, os4.str().c_str()));

          if (veryVerbose) {
            P(EXP_OP); P(os5.str());
          }
          LOOP2_ASSERT(EXP_OP,
                       os5.str(),
                       0 == bsl::strcmp(EXP_OP, os5.str().c_str()));
        }

        {
          const char *EXP_P1 = "    {\n"
                               "        VOID NULL\n"
                               "    }\n";
          const char *EXP_P2 = "{\n"
                               "        VOID NULL\n"
                               "    }\n";
          const char *EXP_P3 = "    { VOID NULL }";
          const char *EXP_P4 = "{ VOID NULL }";
          const char *EXP_OP = "{ VOID NULL }";

          bslma_TestAllocator t1(veryVeryVerbose);
          DescCatalog catalog(&t1);
          CHead mXCH(&catalog);
          Obj &X = reinterpret_cast<Obj&>(mXCH);
          X.selection().makeNull();

          bsl::ostringstream os1, os2, os3, os4, os5;
          X.print(os1, 1, 4);
          X.print(os2, -1, 4);
          X.print(os3, 1, -4);
          X.print(os4, -1, -4);
          os5 << X;

          if (veryVerbose) {
            P(EXP_P1); P(os1.str());
          }
          LOOP2_ASSERT(EXP_P1,
                       os1.str(),
                       0 == bsl::strcmp(EXP_P1, os1.str().c_str()));

          if (veryVerbose) {
            P(EXP_P2); P(os2.str());
          }
          LOOP2_ASSERT(EXP_P2,
                       os2.str(),
                       0 == bsl::strcmp(EXP_P2, os2.str().c_str()));

          if (veryVerbose) {
            P(EXP_P3); P(os3.str());
          }
          LOOP2_ASSERT(EXP_P3,
                       os3.str(),
                       0 == bsl::strcmp(EXP_P3, os3.str().c_str()));

          if (veryVerbose) {
            P(EXP_P4); P(os4.str());
          }
          LOOP2_ASSERT(EXP_P4,
                       os4.str(),
                       0 == bsl::strcmp(EXP_P4, os4.str().c_str()));

          if (veryVerbose) {
            P(EXP_OP); P(os5.str());
          }
          LOOP2_ASSERT(EXP_OP,
                       os5.str(),
                       0 == bsl::strcmp(EXP_OP, os5.str().c_str()));
        }

        const int MAX_PRINT_TESTS = 4;
        const struct {
            int         d_line;       // Line number
            const char *d_spec;       // Type Specification string
            char        d_valueSpec;  // Spec specifying the value for that
                                      // type.  Can be one of 'A', 'B' or 'N'
                                      // corresponding to the A, B or N value
                                      // for that type.
            struct {
                int         d_level;      // Level to print at
                int         d_spacesPerLevel; // spaces per level to print at
                const char *d_printExpOutput;  // Expected o/p from print
                const char *d_printExpOutputN; // Expected o/p from print(null)
            } PRINT_TESTS[MAX_PRINT_TESTS];
            const char *d_outputOpExpOutput;  // Expected o/p from operator<<
            const char *d_outputOpExpOutputN; // Expected o/p from operator<<
                                              // (null)
        } DATA[] = {
            {
                L_,
                "A",
                'A',
                {
                    {
                        1,
                        4,
                        "    {\n"
                        "        CHAR A\n"
                        "    }\n",
                        "    {\n"
                        "        CHAR NULL\n"
                        "    }\n",
                    },
                    {
                       -1,
                        4,
                        "{\n"
                        "        CHAR A\n"
                        "    }\n",
                        "{\n"
                        "        CHAR NULL\n"
                        "    }\n",
                    },
                    {
                        1,
                        -4,
                        "    { CHAR A }",
                        "    { CHAR NULL }"
                    },
                    {
                        -1,
                        -4,
                        "{ CHAR A }",
                        "{ CHAR NULL }"
                    },
                },
                "{ CHAR A }",
                "{ CHAR NULL }"
            },

            {
                L_,
                "C",
                'A',
                {
                    {
                        1,
                        4,
                        "    {\n"
                        "        INT 10\n"
                        "    }\n",
                        "    {\n"
                        "        INT NULL\n"
                        "    }\n",
                    },
                    {
                       -1,
                        4,
                        "{\n"
                        "        INT 10\n"
                        "    }\n",
                        "{\n"
                        "        INT NULL\n"
                        "    }\n",
                    },
                    {
                        1,
                        -4,
                        "    { INT 10 }",
                        "    { INT NULL }"
                    },
                    {
                        -1,
                        -4,
                        "{ INT 10 }",
                        "{ INT NULL }"
                    },
                },
                "{ INT 10 }",
                "{ INT NULL }"
            },

            {
                L_,
                "F",
                'A',
                {
                    {
                        1,
                        4,
                        "    {\n"
                        "        DOUBLE 10.5\n"
                        "    }\n",
                        "    {\n"
                        "        DOUBLE NULL\n"
                        "    }\n",
                    },
                    {
                       -1,
                        4,
                        "{\n"
                        "        DOUBLE 10.5\n"
                        "    }\n",
                        "{\n"
                        "        DOUBLE NULL\n"
                        "    }\n",
                    },
                    {
                        1,
                        -4,
                        "    { DOUBLE 10.5 }",
                        "    { DOUBLE NULL }"
                    },
                    {
                        -1,
                        -4,
                        "{ DOUBLE 10.5 }",
                        "{ DOUBLE NULL }"
                    },
                },
                "{ DOUBLE 10.5 }",
                "{ DOUBLE NULL }"
            },

            {
                L_,
                "G",
                'A',
                {
                    {
                        1,
                        4,
                        "    {\n"
                        "        STRING one\n"
                        "    }\n",
                        "    {\n"
                        "        STRING NULL\n"
                        "    }\n",
                    },
                    {
                       -1,
                        4,
                        "{\n"
                        "        STRING one\n"
                        "    }\n",
                        "{\n"
                        "        STRING NULL\n"
                        "    }\n",
                    },
                    {
                        1,
                        -4,
                        "    { STRING one }",
                        "    { STRING NULL }"
                    },
                    {
                        -1,
                        -4,
                        "{ STRING one }",
                        "{ STRING NULL }"
                    },
                },
                "{ STRING one }",
                "{ STRING NULL }"
            },

            {
                L_,
                "H",
                'A',
                {
                    {
                        1,
                        4,
                        "    {\n"
                        "        DATETIME 01JAN2000_00:01:02.003\n"
                        "    }\n",
                        "    {\n"
                        "        DATETIME NULL\n"
                        "    }\n",
                    },
                    {
                       -1,
                        4,
                        "{\n"
                        "        DATETIME 01JAN2000_00:01:02.003\n"
                        "    }\n",
                        "{\n"
                        "        DATETIME NULL\n"
                        "    }\n",
                    },
                    {
                        1,
                        -4,
                        "    { DATETIME 01JAN2000_00:01:02.003 }",
                        "    { DATETIME NULL }"
                    },
                    {
                        -1,
                        -4,
                        "{ DATETIME 01JAN2000_00:01:02.003 }",
                        "{ DATETIME NULL }"
                    },
                },
                "{ DATETIME 01JAN2000_00:01:02.003 }",
                "{ DATETIME NULL }"
            },

            {
                L_,
                "M",
                'A',
                {
                    {
                        1,
                        4,
                        "    {\n"
                        "        INT_ARRAY [\n"
                        "            10\n"
                        "        ]\n"
                        "    }\n",
                        "    {\n"
                        "        INT_ARRAY NULL\n"
                        "    }\n",
                    },
                    {
                       -1,
                        4,
                        "{\n"
                        "        INT_ARRAY [\n"
                        "            10\n"
                        "        ]\n"
                        "    }\n",
                        "{\n"
                        "        INT_ARRAY NULL\n"
                        "    }\n",
                    },
                    {
                        1,
                        -4,
                        "    { INT_ARRAY [ 10 ] }",
                        "    { INT_ARRAY NULL }"
                    },
                    {
                        -1,
                        -4,
                        "{ INT_ARRAY [ 10 ] }",
                        "{ INT_ARRAY NULL }"
                    },
                },
                "{ INT_ARRAY [ 10 ] }",
                "{ INT_ARRAY NULL }"
            },

            {
                L_,
                "Q",
                'A',
                {
                    {
                        1,
                        4,
                        "    {\n"
                        "        STRING_ARRAY [\n"
                        "            one\n"
                        "        ]\n"
                        "    }\n",
                        "    {\n"
                        "        STRING_ARRAY NULL\n"
                        "    }\n",
                    },
                    {
                       -1,
                        4,
                        "{\n"
                        "        STRING_ARRAY [\n"
                        "            one\n"
                        "        ]\n"
                        "    }\n",
                        "{\n"
                        "        STRING_ARRAY NULL\n"
                        "    }\n",
                    },
                    {
                        1,
                        -4,
                        "    { STRING_ARRAY [ one ] }",
                        "    { STRING_ARRAY NULL }"
                    },
                    {
                        -1,
                        -4,
                        "{ STRING_ARRAY [ one ] }",
                        "{ STRING_ARRAY NULL }"
                    },
                },
                "{ STRING_ARRAY [ one ] }",
                "{ STRING_ARRAY NULL }"
            },

            {
                L_,
                "U",
                'A',
                {
                    {
                        1,
                        4,
                        "    {\n"
                        "        LIST {\n"
                        "            INT 10\n"
                        "            DOUBLE 10.5\n"
                        "            STRING one\n"
                        "            STRING_ARRAY [\n"
                        "                one\n"
                        "            ]\n"
                        "        }\n"
                        "    }\n",
                        "    {\n"
                        "        LIST NULL\n"
                        "    }\n",
                    },
                    {
                       -1,
                        4,
                        "{\n"
                        "        LIST {\n"
                        "            INT 10\n"
                        "            DOUBLE 10.5\n"
                        "            STRING one\n"
                        "            STRING_ARRAY [\n"
                        "                one\n"
                        "            ]\n"
                        "        }\n"
                        "    }\n",
                        "{\n"
                        "        LIST NULL\n"
                        "    }\n",
                    },
                    {
                        1,
                        -4,
                        "    { LIST { INT 10 DOUBLE 10.5 STRING one"
                        " STRING_ARRAY [ one ] } }",
                        "    { LIST NULL }"
                    },
                    {
                        -1,
                        -4,
                        "{ LIST { INT 10 DOUBLE 10.5 STRING one"
                        " STRING_ARRAY [ one ] } }",
                        "{ LIST NULL }"
                    },
                },
                "{ LIST { INT 10 DOUBLE 10.5 STRING one"
                " STRING_ARRAY [ one ] } }",
                "{ LIST NULL }"
            },

            {
                L_,
                "V",
                'A',
                {
                    {
                        1,
                        4,
                        "    {\n"
                        "        TABLE {\n"
                        "            Column Types: [\n"
                        "                INT\n"
                        "                DOUBLE\n"
                        "                STRING\n"
                        "                STRING_ARRAY\n"
                        "            ]\n"
                        "            Row 0: {\n"
                        "                10\n"
                        "                10.5\n"
                        "                one\n"
                        "                [\n"
                        "                    one\n"
                        "                ]\n"
                        "            }\n"
                        "        }\n"
                        "    }\n",
                        "    {\n"
                        "        TABLE NULL\n"
                        "    }\n",
                    },
                    {
                       -1,
                        4,
                        "{\n"
                        "        TABLE {\n"
                        "            Column Types: [\n"
                        "                INT\n"
                        "                DOUBLE\n"
                        "                STRING\n"
                        "                STRING_ARRAY\n"
                        "            ]\n"
                        "            Row 0: {\n"
                        "                10\n"
                        "                10.5\n"
                        "                one\n"
                        "                [\n"
                        "                    one\n"
                        "                ]\n"
                        "            }\n"
                        "        }\n"
                        "    }\n",
                        "{\n"
                        "        TABLE NULL\n"
                        "    }\n",
                    },
                    {
                        1,
                        -4,
                        "    { TABLE {"
                        " Column Types: [ INT DOUBLE STRING STRING_ARRAY ]"
                        " Row 0: { 10 10.5 one [ one ] } } }",
                        "    { TABLE NULL }"
                    },
                    {
                        -1,
                        -4,
                        "{ TABLE {"
                        " Column Types: [ INT DOUBLE STRING STRING_ARRAY ]"
                        " Row 0: { 10 10.5 one [ one ] } } }",
                        "{ TABLE NULL }"
                    },
                },
                "{ TABLE {"
                " Column Types: [ INT DOUBLE STRING STRING_ARRAY ]"
                " Row 0: { 10 10.5 one [ one ] } } }",
                "{ TABLE NULL }"
            },

            {
                L_,
                "e",
                'A',
                {
                    {
                        1,
                        4,
                        "    {\n"
                        "        CHOICE {\n"
                        "            Selection Types: [\n"
                        "                INT\n"
                        "                DOUBLE\n"
                        "                STRING\n"
                        "                STRING_ARRAY\n"
                        "            ]\n"
                        "            {\n"
                        "                STRING one\n"
                        "            }\n"
                        "        }\n"
                        "    }\n",
                        "    {\n"
                        "        CHOICE NULL\n"
                        "    }\n",
                    },
                    {
                       -1,
                        4,
                        "{\n"
                        "        CHOICE {\n"
                        "            Selection Types: [\n"
                        "                INT\n"
                        "                DOUBLE\n"
                        "                STRING\n"
                        "                STRING_ARRAY\n"
                        "            ]\n"
                        "            {\n"
                        "                STRING one\n"
                        "            }\n"
                        "        }\n"
                        "    }\n",
                        "{\n"
                        "        CHOICE NULL\n"
                        "    }\n",
                    },
                    {
                        1,
                        -4,
                        "    { CHOICE {"
                        " Selection Types: [ INT DOUBLE STRING STRING_ARRAY ]"
                        " { STRING one } } }",
                        "    { CHOICE NULL }"
                    },
                    {
                        -1,
                        -4,
                        "{ CHOICE {"
                        " Selection Types: [ INT DOUBLE STRING STRING_ARRAY ]"
                        " { STRING one } } }",
                        "{ CHOICE NULL }"
                    },
                },
                "{ CHOICE {"
                " Selection Types: [ INT DOUBLE STRING STRING_ARRAY ]"
                " { STRING one } } }",
                "{ CHOICE NULL }"
            },

            {
                L_,
                "f",
                'A',
                {
                    {
                        1,
                        4,
                        "    {\n"
                        "        CHOICE_ARRAY {\n"
                        "            Selection Types: [\n"
                        "                INT\n"
                        "                DOUBLE\n"
                        "                STRING\n"
                        "                STRING_ARRAY\n"
                        "            ]\n"
                        "            Item 0: {\n"
                        "                INT 10\n"
                        "            }\n"
                        "            Item 1: {\n"
                        "                DOUBLE 10.5\n"
                        "            }\n"
                        "            Item 2: {\n"
                        "                STRING one\n"
                        "            }\n"
                        "            Item 3: {\n"
                        "                STRING_ARRAY [\n"
                        "                    one\n"
                        "                ]\n"
                        "            }\n"
                        "        }\n"
                        "    }\n",
                        "    {\n"
                        "        CHOICE_ARRAY NULL\n"
                        "    }\n",
                    },
                    {
                       -1,
                        4,
                        "{\n"
                        "        CHOICE_ARRAY {\n"
                        "            Selection Types: [\n"
                        "                INT\n"
                        "                DOUBLE\n"
                        "                STRING\n"
                        "                STRING_ARRAY\n"
                        "            ]\n"
                        "            Item 0: {\n"
                        "                INT 10\n"
                        "            }\n"
                        "            Item 1: {\n"
                        "                DOUBLE 10.5\n"
                        "            }\n"
                        "            Item 2: {\n"
                        "                STRING one\n"
                        "            }\n"
                        "            Item 3: {\n"
                        "                STRING_ARRAY [\n"
                        "                    one\n"
                        "                ]\n"
                        "            }\n"
                        "        }\n"
                        "    }\n",
                        "{\n"
                        "        CHOICE_ARRAY NULL\n"
                        "    }\n",
                    },
                    {
                        1,
                        -4,
                        "    { CHOICE_ARRAY { Selection Types: "
                        "[ INT DOUBLE STRING STRING_ARRAY ]"
                        " Item 0: { INT 10 } Item 1: { DOUBLE 10.5 }"
                        " Item 2: { STRING one } Item 3: { STRING_ARRAY "
                        "[ one ] } } }",
                        "    { CHOICE_ARRAY NULL }"
                    },
                    {
                        -1,
                        -4,
                        "{ CHOICE_ARRAY {"
                        " Selection Types: [ INT DOUBLE STRING STRING_ARRAY ]"
                        " Item 0: { INT 10 } Item 1: { DOUBLE 10.5 }"
                        " Item 2: { STRING one } Item 3: { STRING_ARRAY "
                        "[ one ] } } }",
                        "{ CHOICE_ARRAY NULL }"
                    },
                },
                "{ CHOICE_ARRAY {"
                " Selection Types: [ INT DOUBLE STRING STRING_ARRAY ]"
                " Item 0: { INT 10 } Item 1: { DOUBLE 10.5 }"
                " Item 2: { STRING one } Item 3: { STRING_ARRAY "
                "[ one ] } } }",
                "{ CHOICE_ARRAY NULL }"
            }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (veryVerbose) {
          bsl::cout << "\n\nTesting non-empty choice\n" << bsl::endl;
        }

        for (int i = 0; i < NUM_DATA; ++i) {
          const int   LINE      = DATA[i].d_line;
          const char *SPEC_S    = DATA[i].d_spec;
          const char  SPEC      = SPEC_S[0];
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

          bslma_TestAllocator alloc(veryVeryVerbose);
          DescCatalog catalog(&alloc);  ggCatalog(SPEC_S, catalog);
          CHead mXCH(&catalog);
          Obj &X = reinterpret_cast<Obj&>(mXCH);

          X.makeSelection(0).replaceValue(VALUE_REF);

          bsl::ostringstream os;
          os << X;

          if (veryVerbose) {
             bsl::cout << "\n"; P_(i); P(X); P(OP_EXP); P(os.str());
          }
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
            if (veryVerbose) {
              bsl::cout << "\n"; P_(i); P(j); P(PRINT_EXP); P(os1.str());
            }
            LOOP2_ASSERT(PRINT_EXP,
                         os1.str(),
                         0 == bsl::strcmp(PRINT_EXP, os1.str().c_str()));
          }

          X.selection().makeNull();

          bsl::ostringstream osn;
          osn << X;

          if (veryVerbose) {
             bsl::cout << "\n"; P_(i); P(X); P(OP_EXPN); P(osn.str());
          }
          LOOP2_ASSERT(OP_EXPN,
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
              bsl::cout << "\n"; P_(i); P(j); P(PRINT_EXP); P(os1.str());
            }

            LOOP2_ASSERT(PRINT_EXP,
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
        //   We are able to cast to a 'bdem_ChoiceArrayItem' from a
        //   'bdem_ChoiceHeader' object and put it into any legal state.
        //   Need to cast because the (de)constructor(s) for
        //   'bdem_ChoiceArrayItem' are private and not implemented.
        //   We are not concerned in this test about trying to move the object
        //   from one state to another.
        //
        // Plan:
        //   A 'bdem_ChoiceArrayItem' object can be put into any state using
        //   the two-argument constructor and the 'makeSelection' manipulator.
        //   The state of the object can be determined using the
        //   primary accessors.
        //
        //   Build objects containing 3 different values (including the
        //   "unset" value) of each of the types, 'void', 'int', 'double', and
        //   'bsl::string' by first default-constructing each object, then
        //   using 'makeSelection' to set its selection type and value.  As
        //   orthogonal perturbations, use different client selection IDs.
        //   Using the primary accessors, verify the number of selections,
        //   the selector, the selection type, and the [const] selection.
        //   Also check the validity of the catalog after each selection
        //   change.
        //
        // Testing:
        //   bdem_ElemRef        makeSelection(int index);
        //   int                 numSelections() const;
        //   int                 selector() const;
        //   bdem_ElemRef        selection();
        //   bdem_ConstElemRef   selection() const;
        //   bdem_ElemType::Type selectionType() const;
        //   bdem_ElemType::Type selectionType(int index) const;
        // --------------------------------------------------------------------

        if (verbose) {
          bsl::cout << "\nTESTING BASIC MANIPULATORS"
                    << "\n=========================="
                    << bsl::endl;
        }

        static const struct TestRow {
          int         d_line;
          const char *d_catalogSpec;  // Specification to create the catalog
        } DATA[] = {
          // Line       DescriptorSpec
          // ====       ==============
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
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        {
          for (int j = 0; j < NUM_DATA; ++j) {
            const int   LINE  = DATA[j].d_line;
            const char *SPEC  = DATA[j].d_catalogSpec;
            const int   LEN   = bsl::strlen(SPEC);

            bslma_TestAllocator alloc(veryVeryVerbose);
            DescCatalog catalog(&alloc);  ggCatalog(SPEC, catalog);

            CHead mXCH(&catalog);
            Obj &X = reinterpret_cast<Obj&>(mXCH);

            if (veryVerbose) {
              P(X);
            }

            LOOP_ASSERT(j, catalog.size() == X.numSelections());
            LOOP_ASSERT(j, -1             == X.selector());
            LOOP_ASSERT(j, EType::BDEM_VOID    == X.selectionType());
            LOOP_ASSERT(j, EType::BDEM_VOID    == X.selectionType(-1));
            LOOP_ASSERT(j, true                == X.selection().isNull());
            LOOP_ASSERT(j, false               == X.selection().isNonNull());
            for (int i = 0; i < X.numSelections(); ++i) {
              ASSERT(catalog[i]->d_elemEnum == X.selectionType(i));
            }

            X.selection().makeNull();

            LOOP_ASSERT(j, catalog.size() == X.numSelections());
            LOOP_ASSERT(j, -1             == X.selector());
            LOOP_ASSERT(j, EType::BDEM_VOID    == X.selectionType());
            LOOP_ASSERT(j, EType::BDEM_VOID    == X.selectionType(-1));
            LOOP_ASSERT(j, true           == X.selection().isNull());
            LOOP_ASSERT(j, false          == X.selection().isNonNull());
            for (int i = 0; i < X.numSelections(); ++i) {
              ASSERT(catalog[i]->d_elemEnum == X.selectionType(i));
            }

            for (int k = 0; k < LEN; ++k) {
              const char   S    = SPEC[k];
              const Desc  *DESC = getDescriptor(S);
              EType::Type  TYPE = getElemType(S);

              const void *VAL_A  = getValueA(S);
              const void *VAL_B  = getValueB(S);
              const void *VAL_N  = getValueN(S);

              const bdem_ElemRef      EREF_A(const_cast<void *>(VAL_A),
                                             DESC);
              const bdem_ConstElemRef CEREF_A(VAL_A, DESC);

              const bdem_ElemRef      EREF_B(const_cast<void *>(VAL_B),
                                             DESC);
              const bdem_ConstElemRef CEREF_B(VAL_B, DESC);

              const bdem_ElemRef      EREF_N(const_cast<void *>(VAL_N),
                                             DESC);
              const bdem_ConstElemRef CEREF_N(VAL_N, DESC);

              int nullBits = 1;
              const bdem_ElemRef      EREF_NN(const_cast<void *>(VAL_N),
                                              DESC,
                                              &nullBits, 0);
              const bdem_ConstElemRef CEREF_NN(VAL_N, DESC, &nullBits, 0);

              if (veryVerbose) {
                P_(j); P_(k); P_(SPEC); P(S);
              }

              X.makeSelection(k).replaceValue(CEREF_A);
              LOOP_ASSERT(S, catalog.size() == X.numSelections());
              LOOP_ASSERT(S, k        == X.selector());
              LOOP_ASSERT(S, TYPE     == X.selectionType());
              LOOP_ASSERT(S, TYPE     == X.selectionType(k));
              LOOP_ASSERT(S, EREF_A   == X.selection());
              LOOP_ASSERT(S, CEREF_A  == X.selection());
              LOOP_ASSERT(S, EREF_NN  != X.selection());
              LOOP_ASSERT(S, CEREF_NN != X.selection());
              LOOP_ASSERT(S, false    == X.selection().isNull());
              LOOP_ASSERT(S, true     == X.selection().isNonNull());
              for (int i = 0; i < X.numSelections(); ++i) {
                ASSERT(catalog[i]->d_elemEnum == X.selectionType(i));
              }

              bsl::vector<EType::Type> vA;
              X.selectionTypes(&vA);
              int vASize = vA.size();
              LOOP_ASSERT(S, catalog.size() == vASize);
              for(int i = 0; i < vASize; ++i) {
                LOOP_ASSERT(S, vA[i] == catalog[i]->d_elemEnum);
              }

              X.selection().makeNull();
              LOOP_ASSERT(S, catalog.size() == X.numSelections());
              LOOP_ASSERT(S, k        == X.selector());
              LOOP_ASSERT(S, TYPE     == X.selectionType());
              LOOP_ASSERT(S, TYPE     == X.selectionType(k));
              LOOP_ASSERT(S, EREF_A   != X.selection());
              LOOP_ASSERT(S, CEREF_A  != X.selection());
              LOOP_ASSERT(S, EREF_NN  == X.selection());
              LOOP_ASSERT(S, CEREF_NN == X.selection());
              LOOP_ASSERT(S, true     == X.selection().isNull());
              LOOP_ASSERT(S, false    == X.selection().isNonNull());
              for (int i = 0; i < X.numSelections(); ++i) {
                ASSERT(catalog[i]->d_elemEnum == X.selectionType(i));
              }

              vA.clear();
              X.selectionTypes(&vA);
              vASize = vA.size();
              LOOP_ASSERT(S, catalog.size() == vASize);
              for(int i = 0; i < vASize; ++i) {
                LOOP_ASSERT(S, vA[i] == catalog[i]->d_elemEnum);
              }

              X.makeSelection(k).replaceValue(CEREF_A);
              LOOP_ASSERT(S, catalog.size() == X.numSelections());
              LOOP_ASSERT(S, k        == X.selector());
              LOOP_ASSERT(S, TYPE     == X.selectionType());
              LOOP_ASSERT(S, TYPE     == X.selectionType(k));
              LOOP_ASSERT(S, EREF_A   == X.selection());
              LOOP_ASSERT(S, CEREF_A  == X.selection());
              LOOP_ASSERT(S, EREF_NN  != X.selection());
              LOOP_ASSERT(S, CEREF_NN != X.selection());
              LOOP_ASSERT(S, false    == X.selection().isNull());
              LOOP_ASSERT(S, true     == X.selection().isNonNull());
              for (int i = 0; i < X.numSelections(); ++i) {
                ASSERT(catalog[i]->d_elemEnum == X.selectionType(i));
              }

              vA.clear();
              X.selectionTypes(&vA);
              vASize = vA.size();
              LOOP_ASSERT(S, catalog.size() == vASize);
              for(int i = 0; i < vASize; ++i) {
                LOOP_ASSERT(S, vA[i] == catalog[i]->d_elemEnum);
              }

              X.makeSelection(k).replaceValue(CEREF_B);
              LOOP_ASSERT(S, catalog.size() == X.numSelections());
              LOOP_ASSERT(S, k        == X.selector());
              LOOP_ASSERT(S, TYPE     == X.selectionType());
              LOOP_ASSERT(S, TYPE     == X.selectionType(k));
              LOOP_ASSERT(S, EREF_B   == X.selection());
              LOOP_ASSERT(S, CEREF_B  == X.selection());
              LOOP_ASSERT(S, EREF_NN  != X.selection());
              LOOP_ASSERT(S, CEREF_NN != X.selection());
              LOOP_ASSERT(S, false    == X.selection().isNull());
              LOOP_ASSERT(S, true     == X.selection().isNonNull());
              for (int i = 0; i < X.numSelections(); ++i) {
                ASSERT(catalog[i]->d_elemEnum == X.selectionType(i));
              }

              bsl::vector<EType::Type> vB;
              X.selectionTypes(&vB);
              int vBSize = vB.size();
              LOOP_ASSERT(S, catalog.size() == vBSize);
              for(int i = 0; i < vBSize; ++i) {
                LOOP_ASSERT(S, vB[i] == catalog[i]->d_elemEnum);
              }

              X.selection().makeNull();
              LOOP_ASSERT(S, catalog.size() == X.numSelections());
              LOOP_ASSERT(S, k        == X.selector());
              LOOP_ASSERT(S, TYPE     == X.selectionType());
              LOOP_ASSERT(S, TYPE     == X.selectionType(k));
              LOOP_ASSERT(S, EREF_B   != X.selection());
              LOOP_ASSERT(S, CEREF_B  != X.selection());
              LOOP_ASSERT(S, EREF_NN  == X.selection());
              LOOP_ASSERT(S, CEREF_NN == X.selection());
              LOOP_ASSERT(S, true     == X.selection().isNull());
              LOOP_ASSERT(S, false    == X.selection().isNonNull());
              for (int i = 0; i < X.numSelections(); ++i) {
                ASSERT(catalog[i]->d_elemEnum == X.selectionType(i));
              }

              vB.clear();
              X.selectionTypes(&vB);
              vBSize = vB.size();
              LOOP_ASSERT(S, catalog.size() == vBSize);
              for(int i = 0; i < vBSize; ++i) {
                LOOP_ASSERT(S, vB[i] == catalog[i]->d_elemEnum);
              }

              X.makeSelection(k).replaceValue(CEREF_B);
              LOOP_ASSERT(S, catalog.size() == X.numSelections());
              LOOP_ASSERT(S, k        == X.selector());
              LOOP_ASSERT(S, TYPE     == X.selectionType());
              LOOP_ASSERT(S, TYPE     == X.selectionType(k));
              LOOP_ASSERT(S, EREF_B   == X.selection());
              LOOP_ASSERT(S, CEREF_B  == X.selection());
              LOOP_ASSERT(S, EREF_NN  != X.selection());
              LOOP_ASSERT(S, CEREF_NN != X.selection());
              LOOP_ASSERT(S, false    == X.selection().isNull());
              LOOP_ASSERT(S, true     == X.selection().isNonNull());
              for (int i = 0; i < X.numSelections(); ++i) {
                ASSERT(catalog[i]->d_elemEnum == X.selectionType(i));
              }

              vB.clear();
              X.selectionTypes(&vB);
              vBSize = vB.size();
              LOOP_ASSERT(S, catalog.size() == vBSize);
              for(int i = 0; i < vBSize; ++i) {
                LOOP_ASSERT(S, vB[i] == catalog[i]->d_elemEnum);
              }

              X.makeSelection(k).replaceValue(CEREF_N);
              LOOP_ASSERT(S, catalog.size() == X.numSelections());
              LOOP_ASSERT(S, k        == X.selector());
              LOOP_ASSERT(S, TYPE     == X.selectionType());
              LOOP_ASSERT(S, TYPE     == X.selectionType(k));
              LOOP_ASSERT(S, EREF_N   == X.selection());
              LOOP_ASSERT(S, CEREF_N  == X.selection());
              LOOP_ASSERT(S, EREF_NN  != X.selection());
              LOOP_ASSERT(S, CEREF_NN != X.selection());
              LOOP_ASSERT(S, false    == X.selection().isNull());
              LOOP_ASSERT(S, true     == X.selection().isNonNull());
              for (int i = 0; i < X.numSelections(); ++i) {
                ASSERT(catalog[i]->d_elemEnum == X.selectionType(i));
              }

              bsl::vector<EType::Type> vN;
              X.selectionTypes(&vN);
              int vNSize = vN.size();
              LOOP_ASSERT(S, catalog.size() == vNSize);
              for(int i = 0; i < vNSize; ++i) {
                LOOP_ASSERT(S, vN[i] == catalog[i]->d_elemEnum);
              }

              X.selection().makeNull();
              LOOP_ASSERT(S, catalog.size() == X.numSelections());
              LOOP_ASSERT(S, k        == X.selector());
              LOOP_ASSERT(S, TYPE     == X.selectionType());
              LOOP_ASSERT(S, TYPE     == X.selectionType(k));
              LOOP_ASSERT(S, EREF_N   != X.selection());
              LOOP_ASSERT(S, CEREF_N  != X.selection());
              LOOP_ASSERT(S, EREF_NN  == X.selection());
              LOOP_ASSERT(S, CEREF_NN == X.selection());
              LOOP_ASSERT(S, true     == X.selection().isNull());
              LOOP_ASSERT(S, false    == X.selection().isNonNull());
              for (int i = 0; i < X.numSelections(); ++i) {
                ASSERT(catalog[i]->d_elemEnum == X.selectionType(i));
              }

              vN.clear();
              X.selectionTypes(&vN);
              vNSize = vN.size();
              LOOP_ASSERT(S, catalog.size() == vNSize);
              for(int i = 0; i < vNSize; ++i) {
                LOOP_ASSERT(S, vN[i] == catalog[i]->d_elemEnum);
              }

              X.makeSelection(k).replaceValue(CEREF_N);
              LOOP_ASSERT(S, catalog.size() == X.numSelections());
              LOOP_ASSERT(S, k        == X.selector());
              LOOP_ASSERT(S, TYPE     == X.selectionType());
              LOOP_ASSERT(S, TYPE     == X.selectionType(k));
              LOOP_ASSERT(S, EREF_N   == X.selection());
              LOOP_ASSERT(S, CEREF_N  == X.selection());
              LOOP_ASSERT(S, EREF_NN  != X.selection());
              LOOP_ASSERT(S, CEREF_NN != X.selection());
              LOOP_ASSERT(S, false    == X.selection().isNull());
              LOOP_ASSERT(S, true     == X.selection().isNonNull());
              for (int i = 0; i < X.numSelections(); ++i) {
                ASSERT(catalog[i]->d_elemEnum == X.selectionType(i));
              }

              vN.clear();
              X.selectionTypes(&vN);
              vNSize = vN.size();
              LOOP_ASSERT(S, catalog.size() == vNSize);
              for(int i = 0; i < vNSize; ++i) {
                LOOP_ASSERT(S, vN[i] == catalog[i]->d_elemEnum);
              }
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
              int  LEN    = sizeof SPEC - 1;

        bslma_TestAllocator  testAllocator;
        bslma_Allocator     *Z = &testAllocator;

        if (verbose) {
          bsl::cout << "\nTesting 'getElemType'" << bsl::endl;
        }

        {
          for (int i = 0; i < LEN; ++i) {
              ASSERT((EType::Type) i == getElemType(SPEC[i]));
          }
        }

        if (verbose) {
          bsl::cout << "\nTesting 'getDescriptor'" << bsl::endl;
        }

        {
          for (int i = 0; i < LEN; ++i) {
              ASSERT(DESCRIPTORS[i] == getDescriptor(SPEC[i]));
          }
        }

        if (verbose) {
          bsl::cout << "\nTesting 'ggCatalog' with empty "
                                << "string." << bsl::endl;
        }

        {
          DescCatalog x1;  ggCatalog("", x1);
          ASSERT(0 == x1.size());
        }

        if (verbose) {
          bsl::cout << "\nTesting 'ggCatalog' for a 1 length catalog"
                    << " with all types." << bsl::endl;
        }

        {
          ASSERT(LEN == SPEC_LEN);

          for (int i = 0; i < LEN; ++i) {
            char tempSpec[] = { SPEC[i], 0 };
            if (veryVerbose) {
              bsl::cout << "testing 'ggCatalog' using " << tempSpec
                        << bsl::endl;
            }

            DescCatalog x1;  ggCatalog(tempSpec, x1);
            LOOP_ASSERT(i, 1 == x1.size());
            LOOP_ASSERT(i, (EType::Type) i == x1[0]->d_elemEnum);
          }
        }

        if (verbose) {
            bsl::cout << "\nTesting 'ggCatalog' for a multi length catalog"
                      << bsl::endl;
        }

        {
          const struct {
              int         d_line;
              const char *d_spec;
          } SPECS[] = {
            // Line       DescriptorSpec
            // ====       ==============
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

              if (veryVerbose) {
                P(SPEC);
              }

              DescCatalog x1;  ggCatalog(SPEC, x1);
              LOOP3_ASSERT(LINE, LEN, x1.size(), LEN == x1.size());

              for (int j = 0; j < LEN; ++j) {
                  const char S = SPEC[j];
                  LOOP_ASSERT(LINE, getElemType(S) == x1[j]->d_elemEnum);
              }
          }
        }

        if (verbose) {
          bsl::cout << "\nTesting 'getValueA'." << bsl::endl;
        }

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

        if (verbose) {
          bsl::cout << "\nTesting 'getValueB'." << bsl::endl;
        }

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

        if (verbose) {
          bsl::cout << "\nTesting 'getValueN'." << bsl::endl;
        }

        {
          ASSERT(N00   == *(char *) getValueN(SPEC[0]));
          ASSERT(N01   == *(short *) getValueN(SPEC[1]));
          ASSERT(N02   == *(int *) getValueN(SPEC[2]));
          ASSERT(N03   == *(Int64 *) getValueN(SPEC[3]));
          ASSERT(N04   == *(float *) getValueN(SPEC[4]));
          ASSERT(N05   == *(double *) getValueN(SPEC[5]));
          ASSERT(N06   == *(bsl::string *) getValueN(SPEC[6]));
          ASSERT(N07   == *(Datetime *) getValueN(SPEC[7]));
          ASSERT(N08   == *(Date *) getValueN(SPEC[8]));
          ASSERT(N09   == *(Time *) getValueN(SPEC[9]));
          ASSERT(N10   == *(bsl::vector<char> *) getValueN(SPEC[10]));
          ASSERT(N11   == *(bsl::vector<short> *) getValueN(SPEC[11]));
          ASSERT(N12   == *(bsl::vector<int> *) getValueN(SPEC[12]));
          ASSERT(N13   == *(bsl::vector<Int64> *) getValueN(SPEC[13]));
          ASSERT(N14   == *(bsl::vector<float> *) getValueN(SPEC[14]));
          ASSERT(N15   == *(bsl::vector<double> *) getValueN(SPEC[15]));
          ASSERT(N16   == *(bsl::vector<bsl::string> *) getValueN(SPEC[16]));
          ASSERT(N17   == *(bsl::vector<Datetime> *)getValueN(SPEC[17]));
          ASSERT(N18   == *(bsl::vector<Date> *) getValueN(SPEC[18]));
          ASSERT(N19   == *(bsl::vector<Time> *) getValueN(SPEC[19]));
          ASSERT(N20   == *(bdem_List *) getValueN(SPEC[20]));
          ASSERT(N21   == *(bdem_Table *) getValueN(SPEC[21]));
          ASSERT(N22   == *(bool *) getValueN(SPEC[22]));
          ASSERT(N23   == *(DatetimeTz *) getValueN(SPEC[23]));
          ASSERT(N24   == *(DateTz *) getValueN(SPEC[24]));
          ASSERT(N25   == *(TimeTz *) getValueN(SPEC[25]));
          ASSERT(N26   == *(bsl::vector<bool> *) getValueN(SPEC[26]));
          ASSERT(N27   == *(bsl::vector<DatetimeTz> *)getValueN(SPEC[27]));
          ASSERT(N28   == *(bsl::vector<DateTz> *) getValueN(SPEC[28]));
          ASSERT(N29   == *(bsl::vector<TimeTz> *) getValueN(SPEC[29]));
          ASSERT(N30   == *(bdem_Choice *) getValueN(SPEC[30]));
          ASSERT(N31   == *(bdem_ChoiceArray *) getValueN(SPEC[31]));
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
        //   bdem_ChoiceArrayItem object are as expected, check that memory is
        //   being used as expected, and (if in verbose mode), print the value
        //   of the created or modified object.
        //
        //   0. Construct a types catalog (vector<bdem_ElemType::Type>)
        //      containing the types, (INT, STRING, DOUBLE).
        //   1. Create an object, X, using the types catalog.
        //   2. Modify X so that it holds an int, 35.
        //   3. Create an object, X, that holds an unset string.
        //   4. Modify the value in X so that it holds "hello world".
        //   5. Change X to hold a double.
        //
        // Testing:
        //   This test case exercises basic value-semantic functionality.
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nBREATHING TEST"
                               << "\n==============" << bsl::endl;

        bslma_TestAllocator t1;

        DescCatalog catalog(&t1);
        const int INT_IDX = catalog.size();
        catalog.push_back(&Prop::d_intAttr);
        const int STR_IDX = catalog.size();
        catalog.push_back(&Prop::d_stringAttr);
        const int DBL_IDX = catalog.size();
        catalog.push_back(&Prop::d_doubleAttr);

        const int baseBlocks = t1.numBlocksInUse();

        if (verbose) {
          bsl::cout << "\n\nConstruct header and cast to item" << bsl::endl;
        }

        CHead mXCH(&catalog);
        Obj &X = reinterpret_cast<Obj&>(mXCH);

        if (veryVerbose) {
          P(X);
        }

        ASSERT(catalog.size() == X.numSelections());
        ASSERT(-1 == X.selector());
        ASSERT(EType::BDEM_VOID == X.selectionType());
        ASSERT(EType::BDEM_VOID == X.selectionType(-1));
        ASSERT(EType::BDEM_VOID == X.selection().type());
        ASSERT(&Prop::d_voidAttr == X.selection().descriptor());
        for (int i = 0; i < X.numSelections(); ++i) {
          ASSERT(catalog[i]->d_elemEnum == X.selectionType(i));
        }

        if (verbose) {
          bsl::cout << "\n\nInteger value" << bsl::endl;
        }

        const int iv = 35;
        X.makeSelection(INT_IDX).theModifiableInt() = iv;

        if (veryVerbose) {
          P(X);
        }

        ASSERT(INT_IDX == X.selector());
        ASSERT(EType::BDEM_INT == X.selectionType());
        ASSERT(EType::BDEM_INT == X.selectionType(INT_IDX));
        ASSERT(EType::BDEM_INT == X.selection().type());
        ASSERT(false      == X.selection().isNull());
        ASSERT(iv == X.theInt());
        ASSERT(&Prop::d_intAttr == X.selection().descriptor());
        ASSERT(baseBlocks == t1.numBlocksInUse());
        for (int i = 0; i < X.numSelections(); ++i) {
          ASSERT(catalog[i]->d_elemEnum == X.selectionType(i));
        }

        if (verbose) {
          bsl::cout << "\n\nConstruct unset string" << bsl::endl;
        }

        X.makeSelection(STR_IDX);
        ASSERT(STR_IDX == X.selector());
        ASSERT(EType::BDEM_STRING == X.selectionType());
        ASSERT(EType::BDEM_STRING == X.selectionType(STR_IDX));
        ASSERT(EType::BDEM_STRING == X.selection().type());
        ASSERT(true               == X.selection().isNull());
        ASSERT(&Prop::d_stringAttr == X.selection().descriptor());
        for (int i = 0; i < X.numSelections(); ++i) {
          ASSERT(catalog[i]->d_elemEnum == X.selectionType(i));
        }

        if (veryVerbose) {
          bsl::cout << "unset string " << P(X);
        }

        bsl::string &sp = X.theModifiableString();
        ASSERT("" == sp);
        ASSERT("" == X.theString());
        ASSERT(t1.numBlocksInUse() <= baseBlocks + 1);
        ASSERT(STR_IDX == X.selector());
        ASSERT(EType::BDEM_STRING == X.selectionType());
        ASSERT(EType::BDEM_STRING == X.selectionType(STR_IDX));
        ASSERT(EType::BDEM_STRING == X.selection().type());
        ASSERT(false         == X.selection().isNull());
        ASSERT(&Prop::d_stringAttr == X.selection().descriptor());
        for (int i = 0; i < X.numSelections(); ++i) {
          ASSERT(catalog[i]->d_elemEnum == X.selectionType(i));
        }

        sp = "Hello world";

        if (veryVerbose) {
          bsl::cout << "set string " << P(X);
        }

        ASSERT("Hello world" == sp);
        ASSERT("Hello world" == X.theString());
        ASSERT(baseBlocks + 1 == t1.numBlocksInUse());
        ASSERT(STR_IDX == X.selector());
        ASSERT(EType::BDEM_STRING == X.selectionType());
        ASSERT(EType::BDEM_STRING == X.selectionType(STR_IDX));
        ASSERT(EType::BDEM_STRING == X.selection().type());
        ASSERT(false         == X.selection().isNull());
        ASSERT(&Prop::d_stringAttr == X.selection().descriptor());
        for (int i = 0; i < X.numSelections(); ++i) {
          ASSERT(catalog[i]->d_elemEnum == X.selectionType(i));
        }

        if (verbose) {
          bsl::cout << "\n\nChange int choice to double" << bsl::endl;
        }

        double dv = 1.23;

        X.makeSelection(DBL_IDX).theModifiableDouble() = dv;

        if (veryVerbose) {
          P(X);
        }

        ASSERT(DBL_IDX == X.selector());
        ASSERT(EType::BDEM_DOUBLE == X.selectionType());
        ASSERT(EType::BDEM_DOUBLE == X.selectionType(DBL_IDX));
        ASSERT(EType::BDEM_DOUBLE == X.selection().type());
        ASSERT(false         == X.selection().isNull());
        ASSERT(&Prop::d_doubleAttr == X.selection().descriptor());
        ASSERT(dv == X.theDouble());
        ASSERT(baseBlocks == t1.numBlocksInUse());
        for (int i = 0; i < X.numSelections(); ++i) {
          ASSERT(catalog[i]->d_elemEnum == X.selectionType(i));
        }

        X.selection().makeNull();
        ASSERT(DBL_IDX == X.selector());
        ASSERT(EType::BDEM_DOUBLE == X.selectionType());
        ASSERT(EType::BDEM_DOUBLE == X.selectionType(DBL_IDX));
        ASSERT(EType::BDEM_DOUBLE == X.selection().type());
        ASSERT(true          == X.selection().isNull());
        ASSERT(&Prop::d_doubleAttr == X.selection().descriptor());
        ASSERT(dv != X.theDouble());
        ASSERT(baseBlocks == t1.numBlocksInUse());
        for (int i = 0; i < X.numSelections(); ++i) {
          ASSERT(catalog[i]->d_elemEnum == X.selectionType(i));
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
