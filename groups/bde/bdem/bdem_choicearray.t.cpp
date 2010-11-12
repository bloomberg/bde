// bdem_choicearray.t.cpp                  -*-C++-*-
#include <bdem_choicearray.h>

#include <bdem_choice.h>            // testing only
#include <bdem_properties.h>
#include <bdem_elemattrlookup.h>
#include <bdem_functiontemplates.h>

#include <bdem_listimp.h>           // testing only
#include <bdem_tableimp.h>          // testing only
#include <bdem_choiceimp.h>         // testing only
#include <bdem_rowdata.h>

#include <bdema_sequentialallocator.h>
#include <bdesb_memoutstreambuf.h>
#include <bdesb_fixedmeminstreambuf.h>
#include <bdetu_unset.h>
#include <bdeu_printmethods.h>

#include <bdex_byteoutstream.h>
#include <bdex_byteinstream.h>
#include <bdex_testoutstream.h>
#include <bdex_testinstream.h>
#include <bdex_testinstreamexception.h>

#include <bslalg_typetraits.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_newdeleteallocator.h>
#include <bslmf_isconvertible.h>
#include <bsls_platform.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// Although 'bdem_ChoiceArray' is a fully value-semantic class, all of the
// substantive implementation resides in 'bdem_ChoiceArrayImp'.
//
// Our goal here is primarily to ensure correct function signatures and that
// function arguments and return values are being propagated properly.
// Important issues related to the various allocation strategies, aliasing,
// thread safety, and exception neutrality are also tested via this interface.
//
// The test plan for this class follows the general outline for
// tests of a value semantic component.
//
//  o [ 1] Breathing test
//  o [ 2] Verify basic helper functions (ggCatalog, getValue, etc)
//  o [ 3] Basic constructors- Tests the functionality of the basic
//                    constructors.  It also tests the catalog accessors
//                    of the constructed choice array, e.g. numSelections() and
//                    selectionType().
//  o [ 4] Basic modifiers & accessors - Tests the basic modifier and accessor
//                    methods.  appendItem and operator[] are the key
//                    methods.
//  o [ 5] Verify secondary helper functions (populateData) - these functions
//                    require accessors and modifiers tested in test case 4.
//  o [ 6] Output operations - operator<<, print()
//  o [ 7] Equality operators - operator==, operator!=
//  o [ 8] Other Constructors - constructors, except for copy-constructors,
//                              that have not been tested
//  o [ 9] Copy constructors
//  o [10] Bdex streaming - input and output objects to streams
//  o [11] Assignment - operator=
//  o [12-16] Other manipulators - Any manipulators not tested already
//                   (operator[], reset, append, insert, remove)
//  o [17] Usage example
//
//-----------------------------------------------------------------------------
//
//                         ==========================
//                         class bdem_ChoiceArray
//                         ==========================
// CREATORS
// [ 3]  bdem_ChoiceArray(const vector<EType::Type>&  typesCatalog,
//                        AggOption::AllocationStrategy allocMode,
//                        bslma_Allocator            *basicAlloc = 0);
// [ 8]  bdem_ChoiceArray(bslma_Allocator *basicAllocator = 0);
// [ 8]  bdem_ChoiceArray(
//                  bdem_AggregateOption::AllocationStrategy  allocMode,
//                  bslma_Allocator  *basicAllocator = 0);
// [ 8]  bdem_ChoiceArray(
//                        const bdem_ElemType::Type       typesCatalog[],
//                        int                             typesCatalogLen,
//                        bslma_Allocator                 *basicAllocator = 0);
// [ 8]  bdem_ChoiceArray(
//                        const bdem_ElemType::Type        typesCatalog[],
//                        int                              typesCatalogLen,
//                        bdem_AggregateOption::AllocationStrategy  allocMode,
//                        bslma_Allocator                 *basicAllocator = 0);
// [ 8] bdem_ChoiceArray(
//                       const bsl::vector<bdem_ElemType::Type>&  typesCatalog,
//                       bdem_AggregateOption::AllocationStrategy  allocMode,
//                       bslma_Allocator                  *basicAllocator =
//                       0);
// [ 9]  bdem_ChoiceArray(const bdem_ChoiceArrayImp& original,
//                        bslma_Allocator *);
// [ 9]  bdem_ChoiceArray(const bdem_ChoiceArrayImp& original,
//                        bdem_AllocationStrategy,
//                        bslma_Allocator *);
// [ 3]  ~bdem_ChoiceArray();
// MANIPULATORS
// [ 4]  void appendItem(const bdem_Choice& src);
// [10]  bdex_InStream& streamIn(bdex_InStream& stream);
// [10]  bdex_InStream& streamIn(bdex_InStream& stream, int version);
// [11]  bdem_ChoiceArray& operator=(const bdem_ChoiceArray& rhs);
// [12]  bdem_ChoiceArrayItem& theModifiableItem(int index);
// [13]  void reset(const bdem_ElemType::Type [], int);
// [13]  void reset(const bsl::vector<bdem_ElemType::Type>& );
// [14]  void appendNullItems(int elemCount);
// [14]  void appendItem(int, const bdem_ChoiceArryItem&);
// [15]  void insertItem(int, const bdem_Choice&);
// [15]  void insertItem(int, const bdem_ChoiceArrayItem&);
// [15]  void insertNullItems(int,int);
// [16]  void removeItem(int ,int );
// [16]  void removeItem(int );
// [16]  void removeAllItems();
// [16]  void removeAll();
// ACCESSORS
// [ 3]  int numSelections() const;
// [ 3]  bdem_ElemType::Type selectionType(int selectionIndex) const;
// [ 3]  void selectionTypes(bsl::vector<EType::Type> *result) const;
// [ 4]  void int length() const;
// [ 4]  const bdem_ChoiceArrayItem& operator[](int index) const;
// [ 4]  bool isEmpty() const;
// [ 6]  bsl::ostream& print(bsl::ostream& stream,
//                           int           level = 0,
//                           int           spacesPerLevel = 4) const;
// [ 6]  operator<<(ostream&, const bdem_Choice&);
// [ 7]  operator==(const bdem_ChoiceArray&, const bdem_ChoiceArray&);
// [ 7]  operator!=(const bdem_ChoiceArray&, const bdem_ChoiceArray&);
// [10]  int maxSupportedBdexVersion() const;
// [10]  bdex_OutStream& streamOut(bdex_OutStream& stream) const;
// [10]  bdex_OutStream& streamOut(bdex_OutStream& stream, int) const;
// [ 4]  const bdem_ChoiceArrayItem& theItem(int index) const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] populateCatalog
// [ 5] populateData
// [17] USAGE
//-----------------------------------------------------------------------------

#define READ(OBJ, STREAM) {\
OBJ.bdexStreamIn(STREAM, VERSION);\
}

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
#define P_(X) bsl::cout<< #X " = " << (X) << ", " <<bsl::flush;// P(X) w/o '\n'
#define L_ __LINE__                                // current Line number
#define T_ bsl::cout << "\t" << bsl::flush;        // Print a tab (w/o newline)
#define N_  cout << "\n" << flush;          // Print a newline
#define PL(X) bsl::cout << "Line " << (X) << ": ";

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdem_ChoiceArray Obj;

typedef bdem_Properties                      Prop;
typedef bdem_Descriptor                      Desc;
typedef bdem_ElemType                        EType;
typedef bdem_ElemRef                         ERef;
typedef bdem_ConstElemRef                    CERef;
typedef bdem_AggregateOption                 AggOption;
typedef bdem_Choice                          Choice;
typedef bdem_ChoiceArrayItem                 Item;

typedef bsl::vector<EType::Type>             Catalog;

typedef bsls_PlatformUtil::Int64             Int64;

typedef bdet_Datetime                        Datetime;
typedef bdet_Date                            Date;
typedef bdet_Time                            Time;
typedef bdet_DatetimeTz                      DatetimeTz;
typedef bdet_DateTz                          DateTz;
typedef bdet_TimeTz                          TimeTz;

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;

static const AggOption::AllocationStrategy PASSTH =
                                                  AggOption::BDEM_PASS_THROUGH;

typedef bdex_TestInStream                In;
typedef bdex_TestOutStream               Out;

//=============================================================================
//                  GLOBAL STUB CLASSES FOR TESTING
//-----------------------------------------------------------------------------

namespace BloombergLP {

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
    bdem_RowRef theModifiableRow(int index);
};

bool operator==(const bdem_Table& lhs, const bdem_Table& rhs);
bool operator!=(const bdem_Table& lhs, const bdem_Table& rhs);

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

bdem_RowRef bdem_Table::theModifiableRow(int index)
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
       const int NUM_TYPES = sizeof(types) / sizeof(*types);
       bdem_Table t(types, NUM_TYPES); t.appendNullRow();
       t.theModifiableRow(0)[0].theModifiableInt() = A02;
       t.theModifiableRow(0)[1].theModifiableDouble() = A05;
       t.theModifiableRow(0)[2].theModifiableString() = A06;
       t.theModifiableRow(0)[3].theModifiableStringArray() = A16;
       return t;
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
       t.theModifiableRow(0)[0].theModifiableInt() = B02;
       t.theModifiableRow(0)[1].theModifiableDouble() = B05;
       t.theModifiableRow(0)[2].theModifiableString() = B06;
       t.theModifiableRow(0)[3].theModifiableStringArray() = B16;
       return t;
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
       t.theModifiableItem(0).makeSelection(0).theModifiableInt() = A02;
       t.theModifiableItem(1).makeSelection(1).theModifiableDouble() = A05;
       t.theModifiableItem(2).makeSelection(2).theModifiableString() = A06;
       t.theModifiableItem(3).makeSelection(3).theModifiableStringArray()= A16;
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
       t.theModifiableItem(0).makeSelection(0).theModifiableInt() = B02;
       t.theModifiableItem(1).makeSelection(1).theModifiableDouble() = B05;
       t.theModifiableItem(2).makeSelection(2).theModifiableString() = B06;
       t.theModifiableItem(3).makeSelection(3).theModifiableStringArray()= B16;
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
    // a Catalog.  Each character specifies an element type and the order
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
int compare(const TYPE& lhs, const Choice& rhs, char spec)
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
      case 'R': return lhs.theDatetimeArray() ==
                                                        rhs.theDatetimeArray();
      case 'S': return lhs.theDateArray() == rhs.theDateArray();
      case 'T': return lhs.theTimeArray() == rhs.theTimeArray();
      case 'U': return lhs.theList() == rhs.theList();
      case 'V': return lhs.theTable() == rhs.theTable();
      case 'W': return lhs.theBool() == rhs.theBool();
      case 'X': return lhs.theDatetimeTz() == rhs.theDatetimeTz();
      case 'Y': return lhs.theDateTz() == rhs.theDateTz();
      case 'Z': return lhs.theTimeTz() == rhs.theTimeTz();
      case 'a': return lhs.theBoolArray() == rhs.theBoolArray();
      case 'b': return lhs.theDatetimeTzArray() ==
                                                      rhs.theDatetimeTzArray();
      case 'c': return lhs.theDateTzArray() == rhs.theDateTzArray();
      case 'd': return lhs.theTimeTzArray() == rhs.theTimeTzArray();
      case 'e': return lhs.theChoice() == rhs.theChoice();
      case 'f': return lhs.theChoiceArray() == rhs.theChoiceArray();
      default: return 0;
    }
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

static EType::Type getElemType(char spec)
    // Return the element type corresponding to the specified
    // 'spec' value.  Valid input consists of uppercase letters where the
    // index of each letter is in "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef".
{
    const char *p = bsl::strchr(SPECIFICATIONS, spec);
    LOOP_ASSERT(spec, p);
    int index = p - SPECIFICATIONS;
    LOOP3_ASSERT(*p, index, SPEC_LEN, index < SPEC_LEN);
    return (EType::Type) index;
}

static const Desc *getDescriptor(EType::Type type)
{
    const int index = type;
    LOOP2_ASSERT(index, SPEC_LEN, index < SPEC_LEN);
    return DESCRIPTORS[index];
}

static const Desc *getDescriptor(char spec)
    // Return the non-modifiable descriptor corresponding to the specified
    // 'spec' value.  Valid input consists of uppercase letters where the
    // index of each letter is in "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef".
{
    const char *p = bsl::strchr(SPECIFICATIONS, spec);
    LOOP_ASSERT(spec, p);
    int index = p - SPECIFICATIONS;
    return getDescriptor((EType::Type)index);
}

static Catalog ggCatalog(const char *s)
    // Return a types catalog containing the types corresponding to the
    // specified 's' specification string.
{
    Catalog catalog;
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
    const char *p = bsl::strchr(SPECIFICATIONS, spec);
    LOOP_ASSERT(spec, p);
    int index = p - SPECIFICATIONS;
    LOOP3_ASSERT(*p, index, SPEC_LEN, index < SPEC_LEN);
    return VALUES_A[index];
}

static const void *getValueB(char spec)
    // Return the 'B' value corresponding to the specified 'spec'.
{
    const char *p = bsl::strchr(SPECIFICATIONS, spec);
    LOOP_ASSERT(spec, p);
    int index = p - SPECIFICATIONS;
    LOOP3_ASSERT(*p, index, SPEC_LEN, index < SPEC_LEN);
    return VALUES_B[index];
}

static const void *getValueN(char spec)
    // Return the 'N' value corresponding to the specified 'spec'.
{
    const char *p = bsl::strchr(SPECIFICATIONS, spec);
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

static CERef getConstERef(EType::Type type, const void **VALUES)
{
    return CERef(VALUES[(int)type], getDescriptor(type));
}

static Choice getChoiceA(const char *SPEC, char selection)
{
    const Catalog CATALOG = ggCatalog(SPEC);
    const char *p = bsl::strchr(SPEC, selection);
    LOOP_ASSERT(selection, p);
    int index = p - SPEC;
    Choice c(CATALOG);
    CERef VAL = getConstERefA(selection);
    c.makeSelection(index).replaceValue(VAL);
    return c;
}

static Choice getChoiceB(const char *SPEC, char selection)
{
    const Catalog CATALOG = ggCatalog(SPEC);
    const char *p = bsl::strchr(SPEC, selection);
    LOOP_ASSERT(selection, p);
    int index = p - SPEC;
    Choice c(CATALOG);
    CERef VAL = getConstERefB(selection);
    c.makeSelection(index).replaceValue(VAL);
    return c;
}

static Choice getChoiceN(const char *SPEC, char selection)
{
    const Catalog CATALOG = ggCatalog(SPEC);
    const char *p = bsl::strchr(SPEC, selection);
    LOOP_ASSERT(selection, p);
    int index = p - SPEC;
    Choice c(CATALOG);
    CERef VAL = getConstERefN(selection);
    c.makeSelection(index).replaceValue(VAL);
    return c;
}

static void populateData(Obj *X, const void **VALUES)
// populates array X with dummy data.  Uses the size of the catalog
// to determine the size of the array to populate.  For each element i
// it selects the type at index i from the catalog and looks up a value for
// that type from the VALUES array.
// The VALUES array must be an array of values indexed by the types
// (e.g. VALUES_A)
// Uses operations: appendItem, numSelections, length, selectionTypes
// (they must be tested prior to use)
{
    const int size = X->numSelections();
    Catalog CAT;
    X->selectionTypes(&CAT);
    X->removeAllItems();
    for (int i=0; i < size; ++i) {
        EType::Type type = CAT[i];
        const void *VALUE = VALUES[(int)type];
        Choice c(CAT);
        CERef VAL = getConstERef(type, VALUES);
        c.makeSelection(i).replaceValue(VAL);
        X->appendItem(c);
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

    // define a set of test specs we will use in subsequent tests
    const struct TestRow {
            int         d_line;
            const char *d_catalogSpec;  // Specification to create the catalog
    } DATA[] = {
        // Line     DescriptorSpec
        // ====     ==============
//         { L_,       "" },
//         { L_,       "A" },
//         { L_,       "B" },
//         { L_,       "C" },
//         { L_,       "D" },
//         { L_,       "E" },
//         { L_,       "F" },
//         { L_,       "G" },
//         { L_,       "H" },
//         { L_,       "I" },
//         { L_,       "J" },
//         { L_,       "K" },
//         { L_,       "L" },
//         { L_,       "M" },
//         { L_,       "N" },
//         { L_,       "O" },
//         { L_,       "P" },
//         { L_,       "Q" },
//         { L_,       "R" },
//         { L_,       "S" },
//         { L_,       "T" },
//         { L_,       "U" },
//         { L_,       "V" },
//         { L_,       "W" },
//         { L_,       "X" },
//         { L_,       "Y" },
//         { L_,       "Z" },
//         { L_,       "a" },
//         { L_,       "b" },
//         { L_,       "c" },
//         { L_,       "d" },
//         { L_,       "e" },
//         { L_,       "f" },

//         // Testing various interesting combinations
//         { L_,       "AA" },
//         { L_,       "AC" },
//         { L_,       "CD" },
//         { L_,       "CE" },
//         { L_,       "FG" },
//         { L_,       "GG" },
//         { L_,       "GH" },
//         { L_,       "MN" },
//         { L_,       "OP" },
//         { L_,       "PQ" },
//         { L_,       "KQ" },
//         { L_,       "ST" },
//         { L_,       "WX" },
//         { L_,       "UV" },
//         { L_,       "YZ" },
//         { L_,       "ab" },
//         { L_,       "cd" },
//         { L_,       "ef" },
//         { L_,       "Ue" },
//         { L_,       "Vf" },

//         { L_,       "HIJ" },
//         { L_,       "KLM" },
//         { L_,       "RST" },
//         { L_,       "YZa" },

//         { L_,       "BLRW" },
//         { L_,       "DGNQ" },
//         { L_,       "QRST" },

//         { L_,       "JHKHSK" },
//         { L_,       "RISXLSW" },
//         { L_,       "MXKZOLPR" },
//         { L_,       "GGGGGGGGG" },
//         { L_,       "QQQQQQQQQQ" },
//         { L_,       "abcdABCDEFG" },
//         { L_,       "FDLALAabADSF" },

//         { L_,       "GSRWSASDEFDSA" },
//         { L_,       "PKJHALKGabASDA" },
//         { L_,       "XZSAEWRPOIJLKaAS" },
//         { L_,       "GALKacKASJDKSWEIO" },

        { L_,       "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef" },
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    switch (test) { case 0:  // Zero is always the leading case.
      case 19: {
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

        typedef bdem_ChoiceArray Obj;

        ASSERT((0 == bslmf_IsConvertible<bslma_Allocator*, Obj>::VALUE));
        ASSERT((1 ==
             bslalg_HasTrait<Obj, bslalg_TypeTraitUsesBslmaAllocator>::VALUE));

      } break;
      case 18: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 2
        //
        // Concerns:
        //   That the usage example compiles and runs as expected.
        //
        // Testing:
        //   Usage example 2
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << "\nUSAGE EXAMPLE 2"
                               << "\n===============" << bsl::endl;

        ///Example 2
        ///- - - - -
        // The class bdem_ChoiceArray class can be used to store arrays
        // of choice data, where the type of the value in each element can
        // be selected from a catalog of types held by the object.
        //
        // Each element in a ChoiceArray uses the same catalog as the
        // ChoiceArray itself.  The catalog is an array of bdem_ElemTypes
        // specifying the what are the valid types for the selections held
        // in the ChoiceArray.  Each element in the ChoiceArray can specify
        // it's chosen selection type and selection independently.
        //
        // In the following example we use an array of data representing the
        // results of some computation we wish to display.
        //
        //   [ 12  "No Data"  23  52  "Dived by zero"]
        //
        // Here each element is either an integer or a string describing
        // why data is not available for that element.
        //
        // The example will show how to populate the ChoiceArray with
        // this data, output the array to a data stream, read the value
        // from the stream into a new object, and access the values
        // individually.
        //----------------------------------------------------------------

        //..
        // First create the catalog of types we will use.
        //..
        bsl::string noData   = "No data.";
        bsl::string divError = "Divided by zero.";

        vector<bdem_ElemType::Type> CHOICE_TYPES;
        CHOICE_TYPES.push_back(bdem_ElemType::BDEM_INT);     // result value
        CHOICE_TYPES.push_back(bdem_ElemType::BDEM_STRING);  // error value
        const int NUM_CHOICES = CHOICE_TYPES.size();
        enum { RESULT_VAL, ERROR };

        //..
        // We are now ready to construct our ChoiceArray.
        //..
        bdem_ChoiceArray choiceArray(CHOICE_TYPES);

        //..
        // Now insert our sample data into the array:
        // [ 12  "No Data"  23  52  "Dived by zero"]
        //..
        // choiceArray.insertNullItems(0, 5);
        const int VAL0 = 12;
        const int VAL1 = 23;
        const int VAL2 = 52;

        choiceArray.appendNullItems(5);
        choiceArray.theModifiableItem(0).makeSelection(RESULT_VAL).
                                                     theModifiableInt() = VAL0;
        choiceArray.theModifiableItem(1).makeSelection(ERROR).
                                              theModifiableString()   = noData;
        choiceArray.theModifiableItem(2).makeSelection(RESULT_VAL).
                                                     theModifiableInt() = VAL1;
        choiceArray.theModifiableItem(3).makeSelection(RESULT_VAL).
                                                     theModifiableInt() = VAL2;
        choiceArray.theModifiableItem(4).makeSelection(ERROR).
                                            theModifiableString()   = divError;

        //..
        // We can now output our data onto a bdex stream.
        //..
        bdex_ByteOutStream output;
        choiceArray.bdexStreamOut(output,1);

        //..
        // Now read the  stream into a new choice array.
        //..
        bdem_ChoiceArray choiceArray2;
        bdex_ByteInStream inStream(output.data(),output.length());
        choiceArray2.bdexStreamIn(inStream, 1);

        //..
        // Verify nothing has changed.
        //..
        LOOP2_ASSERT(choiceArray, choiceArray2, choiceArray == choiceArray2);

        //..
        // We are now finished with our original choice array.  We can reset it
        // to clear it's values.
        //..

        choiceArray.removeAll();

        ASSERT(0 == choiceArray.numSelections());
        ASSERT(0 == choiceArray.length());
        ASSERT(bdem_ChoiceArray() == choiceArray);

      } break;
      case 17: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 1
        //
        // Concerns:
        //   That the usage example compiles and runs as expected.
        //
        // Testing:
        //   Usage example 1
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << "\nUSAGE EXAMPLE 1"
                               << "\n===============" << bsl::endl;

        ///Example 1
        ///---------
        // In the following example, we create a couple of 'bdem_ChoiceArray'
        // objects, add items in two different ways, and print the results.
        //
        // The choices within the arrays have selection ID 0 if holding a
        // 'double' result and ID 1 if holding a 'bsl::string' error string.
        // See 'bdem_Choice' for a more complete description of how choices
        // work.
        //..
        static const bdem_ElemType::Type CHOICE_TYPES[] = {
            bdem_ElemType::BDEM_DOUBLE,
            bdem_ElemType::BDEM_STRING,
        };
        static const int NUM_CHOICES =
            sizeof(CHOICE_TYPES) / sizeof(CHOICE_TYPES[0]);
        enum ChoiceIndexes { RESULT_VAL, ERROR_STRING };
        //..
        // Create an empty choice array, 'a', with the structure described in
        // 'CHOICE_TYPES':
        //..
        bdem_ChoiceArray a(CHOICE_TYPES, NUM_CHOICES);
        ASSERT(0 == a.length());
        ASSERT(NUM_CHOICES == a.numSelections());
        //..
        // Append one null item to 'a', then set it to hold a 'double':
        //..
        a.appendNullItems(1);
        a.theModifiableItem(0).makeSelection(RESULT_VAL).
                                                   theModifiableDouble() = 1.2;
        ASSERT(1 == a.length());
        ASSERT(RESULT_VAL == a.theModifiableItem(0).selector());
        ASSERT(1.2 == a.theModifiableItem(0).theDouble());
        //..
        // Create a second choice array, 'b', with the same structure as 'a':
        //..
        bdem_ChoiceArray b(CHOICE_TYPES, NUM_CHOICES);

        //..
        // Append a copy of 'a.theModifiableItem(0)' to 'b':
        //..
        b.appendItem(a, 0);
        ASSERT(1 == b.length());
        // assert(RESULT_VAL = b.theModifiableItem(0).selector());
        // assert(1.2 == b.theModifiableItem(0).theDouble());
        //..
        // Append a null item to 'b', then set it to hold a 'bsl::string':
        //..
        b.appendNullItems(1);
        b.theModifiableItem(1).makeSelection(ERROR_STRING).
                                       theModifiableString() = "Error Message";
        ASSERT(2 == b.length());
        ASSERT(RESULT_VAL == b.theModifiableItem(0).selector());
        ASSERT(1.2 == b.theModifiableItem(0).theDouble());
        ASSERT(ERROR_STRING == b.theModifiableItem(1).selector());
        ASSERT("Error Message" == b.theModifiableItem(1).theString());
        //..
        // Now print the resulting array:
        //..
        if (verbose) b.print(bsl::cout);

      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING 'removeItem' METHODS:
        //
        // Concerns:
        //      That removeItem methods remove items at the correct index.
        //      That they remove the correct number of items.  That there are
        //      no other side effects to the operation
        // Plan:
        //   1.Specify a set S of (unique) objects with substantial and varied
        //     differences in value.  For each element in the set create
        //     a test array A and a control X.  Iterate over the indices of the
        //     array A and and iterate over the number of possible elements
        //     to remove at that index.  Call
        //     removeItems(index,numItemsToRemove) on A and then validate the
        //     resulting array A against the control X.
        //   2.Specify a set S of (unique) objects with substantial and varied
        //     differences in value.  For each element in the set create an
        //     array A and a control X.  Iterate over the indices of the array
        //     A.  Call removeItem(index) on A and validate the resulting array
        //     against the control X.
        //   3.Specify a set S of (unique) objects with substantial and varied
        //     differences in value.  For each element in the set create an
        //     array A and a array B.  Call removeAllItems() on A and call
        //     removeAll() on B and verify the data items for A & B and the
        //     catalog for B the have been removed.  Verify they are
        //
        // Testing:
        //   void removeItem(int ,int );
        //   void removeItem(int );
        //   void removeAllItems();
        //   void removeAll();
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << "\nTesting removeItem METHODS"
                               << "\n======================" << bsl::endl;
        {
            if (verbose) {
                bsl::cout << "\tTest removeItem(int,int) for "
                          << " ChoiceArrays based on data from a table of "
                          << "test specs." << bsl::endl;
            }

            bslma_TestAllocator testAllocator(veryVeryVerbose);

            // iterate over the sample test data
            for (int i = 0; i < NUM_DATA; ++i) {
                const char *SPEC  = DATA[i].d_catalogSpec;
                const int   LEN   = bsl::strlen(SPEC);

                if (veryVerbose) {
                    bsl::cout << "Testing removeItems(int, int) for "
                              << "array based on spec '" << SPEC
                              << "'" << bsl::endl;
                }
                Catalog cat = ggCatalog(SPEC);

                // iterate over the index for removal
                for (int index=0; index < LEN; ++index) {

                    // iterate over how many elements to remove
                    for (int num=0;num<LEN-index;++num) {

                        Obj mA(cat, &testAllocator);   const Obj& A = mA;
                        populateData(&mA,VALUES_A);

                        // create a control copy X
                        Obj mX(mA,&testAllocator);     const Obj &X = mX;

                        // remove the elements
                        mA.removeItems(index,num);

                        // validate the elements have been removed
                        LOOP2_ASSERT(i,index,A.length() == LEN-num);

                        // verify we haven't modified any of the previous
                        // elements
                        for (int j=0;j<index-1;++j) {
                            LOOP3_ASSERT(i,index,j,A.theItem(j) ==
                                                                 X.theItem(j));
                        }
                        // verify the subsequent elements
                        for (int j=index;j<LEN-num;++j) {
                            LOOP3_ASSERT(i,index,j,A.theItem(j) ==
                                                             X.theItem(j+num));
                        }
                    }
                }
            }
        }

        {
            if (verbose) {
                bsl::cout << "\tTest removeItem(int) for "
                          << " ChoiceArrays based on data from a table of "
                          << "test specs" << bsl::endl;
            }
            bslma_TestAllocator testAllocator(veryVeryVerbose);

            // iterate over the sample test data
            for (int i = 0; i < NUM_DATA; ++i) {
                const char *SPEC  = DATA[i].d_catalogSpec;
                const int   LEN   = bsl::strlen(SPEC);

                Catalog cat = ggCatalog(SPEC);

                if (veryVerbose) {
                    bsl::cout << "Testing removeItems(int) for "
                              << "array based on spec '" << SPEC << "'"
                              << bsl::endl;
                }

                // iterate over the index for removal
                for (int index=0; index < LEN; ++index) {
                    if (LEN==0) {
                        continue;
                    }

                    Obj mA(cat, &testAllocator);     const Obj& A = mA;
                    populateData(&mA,VALUES_A);

                    // create a control copy X
                    Obj mX(mA,PASSTH,&testAllocator);
                    const Obj &X = mX;

                    // remove the element
                    mA.removeItem(index);

                    // validate an element has been removed
                    LOOP2_ASSERT(i,index,A.length() == LEN-1);

                    // verify we haven't modified any of the previous
                    // elements
                    for (int j=0;j<index-1;++j) {
                        LOOP3_ASSERT(i,index,j,A.theItem(j) == X.theItem(j));
                    }
                    // verify the subsequent elements
                    for (int j=index;j<LEN-1;++j) {
                        LOOP3_ASSERT(i,index,j,A.theItem(j) == X.theItem(j+1));
                    }

                }
            }
        }
        if (verbose) {
            bsl::cout << "\tTest removeAllItems() and removeAll() for "
                      <<" ChoiceArrays.  Based on data from a table test specs"
                      << bsl::endl;
        }
        {
            bslma_TestAllocator testAllocator(veryVeryVerbose);

            // iterate over the sample test data
            for (int i = 0; i < NUM_DATA; ++i) {
                const char *SPEC  = DATA[i].d_catalogSpec;
                const int   LEN   = bsl::strlen(SPEC);

                Catalog cat = ggCatalog(SPEC);

                if (veryVerbose) {
                    bsl::cout << "Testing removeAllItems() and removeAll() "
                              << "for array based on spec '" << SPEC << "'"
                              << bsl::endl;
                }

                Obj mA(cat, &testAllocator);       const Obj& A = mA;
                populateData(&mA,VALUES_A);
                Obj mB(mA,  &testAllocator);       const Obj& B = mB;

                // sanity check
                ASSERT(cat.size() == A.numSelections());
                ASSERT(cat.size() == B.numSelections());

                mA.removeAllItems();
                mB.removeAll();

                // verify the properties we expect
                ASSERT(0 == A.length());
                ASSERT(0 == B.length());
                ASSERT(cat.size() == A.numSelections());
                ASSERT(0 == B.numSelections());

                Catalog testCat;     A.selectionTypes(&testCat);
                ASSERT(cat == testCat);

                // verify that the arrays are equivalent to newly constructed
                // arrays
                Obj testA(cat,&testAllocator);     const Obj &TESTA = testA;
                Obj testB(&testAllocator);         const Obj &TESTB = testB;
                ASSERT(TESTA == A);
                ASSERT(TESTB == B);
            }
        }

      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING 'insertItem' METHODS:
        //
        // Concerns:
        //   - That insertItem properly inserts a new element in the array
        //     at the correct index.  That it sets the new elements value
        //     correctly.That there are no other side effects to the operation.
        //   - That insertNullItems properly inserts new unset elements into
        //     the array at the proper index and that there are no unexpected
        //     side effects.
        // Plan:
        //   1) Specify a set S of (unique) objects with substantial and varied
        //      differences in value.  For each element in the set iterate
        //      through all the indices.  For each index: insert a value,
        //      verify the correct value was inserted at the correct index.
        //   2) Create a choice array with data values set to an arbitrary
        //      value.  Attempt inserting unset values to each of the points
        //      in the array.  Test with varying numbers of inserted items.
        //      Ensure that the correct number of unset choice header elements
        //      are added to the correct position in the choice array imp
        //   3.Specify a set S of (unique) objects with substantial and varied
        //      differences in value.  For each element in the set iterate
        //      through all the indices.  For each index test for aliasing:
        //      insert a value with a reference to the beginning of the array,
        //      insert a value with a reference to the end of the array,
        //      insert a value with a reference to the middle of the array.
        //      in each case verify the correct value was inserted at the
        //      correct index.

        //
        // Testing:
        //   void insertItem(int, const bdem_Choice&);
        //   void insertItem(int, const bdem_ChoiceArrayItem&);
        //   void insertNullItems(int,int);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting insertItem METHOD"
                               << "\n======================" << bsl::endl;

        static const struct TestRow {
                int         d_line;
                const char *d_catalogSpec;  // Specification to create
                // the catalog
        } DATA[] = {
            // Line     DescriptorSpec
            // ====     ==============
            { L_,       "" },
            { L_,       "A" },
            { L_,       "D" },
            { L_,       "AA" },
            { L_,       "GALKacK" },
        };
        static const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

        {
            if (verbose) {
                bsl::cout << "\tTest insert(i,bdem_ChoiceHeader) for choice"
                          << "arrays based on data from a table test specs"
                          << bsl::endl;
            }
            bslma_TestAllocator testAllocator(veryVeryVerbose);

            // iterate over element over our selection of test data
            for (int i = 0; i < NUM_DATA; ++i) {
                const char *SPEC  = DATA[i].d_catalogSpec;
                const int   LEN   = bsl::strlen(SPEC);

                Catalog cat = ggCatalog(SPEC);
                if (veryVerbose) {
                   bsl::cout << "Testing insertItem(i,Obj) for "
                             << "array based on spec '" << SPEC << "'"
                             << bsl::endl;
                }
                // iterate over the index for insertion
                // notice index=LEN+1 appends to the array
                for (int index=0; index < LEN+1; ++index) {
                    BEGIN_BSLMA_EXCEPTION_TEST {
                        Obj mX(cat, &testAllocator);    const Obj& X = mX;
                        populateData(&mX, VALUES_A);

                        Obj mY(mX,&testAllocator);      const Obj &Y = mY;
                        // create a control copy
                        Obj mZ(mX,&testAllocator);      const Obj &Z = mZ;

                        // insert an element at the current index, use a
                        // different type
                        const int selector = (LEN>0) ? (index+1) % LEN : -1;

                        // if we have an empty catalog, insert an unset choice
                        if (selector == -1)  {
                            Choice c; const Choice &C = c;
                            mX.insertItem(index,C);
                            mY.insertItem(index,C);
                        }
                        else {
                            mX.insertItem(index,
                                          getChoiceB(SPEC,SPEC[selector]));
                            mY.insertItem(index,
                                          getChoiceB(SPEC,SPEC[selector]));
                        }

                        // VERIFY THE INSERTED ITEM

                        // validate the index has been added
                        LOOP2_ASSERT(i,index,X.length() == LEN+1);
                        LOOP2_ASSERT(i,index,Y.length() == LEN+1);
                        LOOP2_ASSERT(i,index,X != Z);
                        LOOP2_ASSERT(i,index,Y != Z);

                        // verify we haven't modified any of the
                        // previous elements
                        for (int j=0;j<index;++j) {
                            LOOP3_ASSERT(i,index,j,
                                         X.theItem(j) == Z.theItem(j));
                            LOOP3_ASSERT(i,index,j,
                                         Y.theItem(j) == Z.theItem(j));
                        }
                        // verify our new element
                        if (selector==-1) {
                            LOOP2_ASSERT(i,index,
                                         X.theItem(index).selector() == -1);
                            LOOP2_ASSERT(i,index,
                                         Y.theItem(index).selector() == -1);
                        }
                        else {
                            LOOP2_ASSERT(i,index,
                                         X.theItem(index).selection() ==
                                         getChoiceB(SPEC,
                                                  SPEC[selector]).selection());
                            LOOP2_ASSERT(i,index,
                                         Y.theItem(index).selection() ==
                                         getChoiceB(SPEC,
                                                  SPEC[selector]).selection());
                        }
                        // verify the subsequent elements
                        for (int j=index+1;j<LEN+1;++j) {
                            LOOP3_ASSERT(i,index,j,
                                         X.theItem(j) == Z.theItem(j-1));
                            LOOP3_ASSERT(i,index,j,
                                         Y.theItem(j) == Z.theItem(j-1));
                        }
                    }END_BSLMA_EXCEPTION_TEST
                }
            }
        }
        // Test insertNullItem in various points with various lengths
        // in the array
        {
            if (verbose) {
                bsl::cout << "\tTest insertNullItems at all positions with "
                     << "various lengths in a populated array"
                     << bsl::endl;
            }

            bslma_TestAllocator testAllocator(veryVeryVerbose);
            const char *testSpec = "ABCDEFG";
            const int specLength = strlen(testSpec);

            Catalog cat = ggCatalog(testSpec);
            const int MAX_SPACES = 4;
            // iterate over where to add the spaces
            for (int indexToAdd=0; indexToAdd <= specLength; ++indexToAdd) {
                // iterate over the number of spaces to add
                for (int numSpaces = 0; numSpaces < MAX_SPACES; ++numSpaces) {

                    if (veryVerbose) {
                        bsl::cout << "\tInserting " << numSpaces
                                  << " spaces at position " << indexToAdd
                                  << " in the array"
                                  << bsl::endl;
                    }
                    BEGIN_BSLMA_EXCEPTION_TEST {
                        Obj mX(cat, &testAllocator);    const Obj& X = mX;

                        // We're not concerned with the actual dummy data
                        // Just create an array of a single arbitrary value
                        Choice value(getChoiceA(testSpec,
                                                testSpec[0]));
                        const Choice &VALUE = value;
                        for (int i = 0; i < specLength; ++i) {
                            mX.appendItem(VALUE);
                        }

                        ASSERT(specLength == X.length());

                        mX.insertNullItems(indexToAdd,numSpaces);
                        ASSERT(X.length()==numSpaces + specLength);
                        // verify that previous elements haven't been affected
                        for (int i=0;i<indexToAdd;++i) {
                            LOOP2_ASSERT(indexToAdd,i,
                                         X.theItem(i).selection() ==
                                                            VALUE.selection());
                        }

                        // verify we've added unset items
                        for (int i=indexToAdd;i<indexToAdd+numSpaces;++i) {
                            LOOP2_ASSERT(indexToAdd,i,-1 ==
                                         X.theItem(i).selector());
                        }

                        // verify that subsequent elements haven't been
                        // affected
                        for (int i=indexToAdd+numSpaces;
                             i<specLength+numSpaces; ++i){
                            LOOP2_ASSERT(indexToAdd,i,
                                         X.theItem(i).selection() ==
                                                            VALUE.selection());
                        }
                    }END_BSLMA_EXCEPTION_TEST

                }

            }
        }

        {
            if (verbose) {
                bsl::cout << "\tTest insertItem(i,ChoiceArrayItem) for "
                          << "aliasing issues."  << bsl::endl;
            }
            bslma_TestAllocator testAllocator(veryVeryVerbose);

            // iterate over element over our selection of test data
            for (int i = 0; i < NUM_DATA; ++i) {
                const char *SPEC  = DATA[i].d_catalogSpec;
                const int   LEN   = bsl::strlen(SPEC);
                Catalog cat = ggCatalog(SPEC);

                if (veryVerbose) {
                    bsl::cout << "Testing insertItem(i,Obj) for array "
                              << "based on spec '" << SPEC << "'" << endl;
                }
                BEGIN_BSLMA_EXCEPTION_TEST {
                    // iterate over the index for insertion
                    // notice index=LEN+1 appends to the array
                    for (int index=0; index <= LEN; ++index) {

                        Obj mA1(cat , &testAllocator);   const Obj& A1 = mA1;
                        populateData(&mA1,VALUES_A);
                        // add a empty choice, to ensure even the empty spec
                        // has data
                        mA1.appendItem(Choice());

                        Obj mA2(mA1,&testAllocator); const Obj &A2 = mA2;
                        Obj mB1(mA1,&testAllocator); const Obj &B1 = mB1;
                        Obj mB2(mA1,&testAllocator); const Obj &B2 = mB2;
                        Obj mC1(mA1,&testAllocator); const Obj &C1 = mC1;
                        Obj mC2(mA1,&testAllocator); const Obj &C2 = mC2;

                        // create a control copy
                        Obj mX(mA1,&testAllocator); const Obj &X = mX;

                        mA1.insertItem(index, A1, 0);
                        mB1.insertItem(index, B1, B1.length()-1);
                        mC1.insertItem(index, C1, (C1.length()-1)/2);

                        mA2.insertItem(index, Choice(A2.theItem(0)));
                        mB2.insertItem(index,
                                       Choice(B2.theItem(B2.length()-1)));
                        mC2.insertItem(index,
                                       Choice(C2.theItem((C2.length()-1)/2)));

                        // VERIFY THE INSERTED ITEM

                        // validate the index has been added
                        LOOP2_ASSERT(i,index,X.length()+1 == A1.length());
                        LOOP2_ASSERT(i,index,X.length()+1 == A2.length());
                        LOOP2_ASSERT(i,index,X.length()+1 == B1.length());
                        LOOP2_ASSERT(i,index,X.length()+1 == B2.length());
                        LOOP2_ASSERT(i,index,X.length()+1 == C1.length());
                        LOOP2_ASSERT(i,index,X.length()+1 == C2.length());
                        LOOP2_ASSERT(i,index,X != A1);
                        LOOP2_ASSERT(i,index,X != A2);
                        LOOP2_ASSERT(i,index,X != B1);
                        LOOP2_ASSERT(i,index,X != B2);
                        LOOP2_ASSERT(i,index,X != C1);
                        LOOP2_ASSERT(i,index,X != C2);

                        // verify we haven't modified any of the
                        // previous elements
                        for (int j=0;j<index;++j) {
                            LOOP3_ASSERT(i,index,j,
                                         X.theItem(j) == A1.theItem(j));
                            LOOP3_ASSERT(i,index,j,
                                         X.theItem(j) == B1.theItem(j));
                            LOOP3_ASSERT(i,index,j,
                                         X.theItem(j) == C1.theItem(j));

                            LOOP3_ASSERT(i,index,j,
                                         X.theItem(j) == A2.theItem(j));
                            LOOP3_ASSERT(i,index,j,
                                         X.theItem(j) == B2.theItem(j));
                            LOOP3_ASSERT(i,index,j,
                                         X.theItem(j) == C2.theItem(j));

                        }

                        // verify our new element
                        LOOP2_ASSERT(i,index,
                                     X.theItem(0) == A1.theItem(index));
                        LOOP2_ASSERT(i,index,
                                 X.theItem(X.length()-1) == B1.theItem(index));
                        LOOP2_ASSERT(i,index,
                             X.theItem((X.length()-1)/2) == C1.theItem(index));

                        LOOP2_ASSERT(i,index,
                                     X.theItem(0) == A2.theItem(index));
                        LOOP2_ASSERT(i,index,
                                 X.theItem(X.length()-1) == B2.theItem(index));
                        LOOP2_ASSERT(i,index,
                             X.theItem((X.length()-1)/2) == C2.theItem(index));

                        // verify the subsequent elements
                        for (int j=index+1;j<LEN+1;++j) {
                            LOOP3_ASSERT(i,index,j,
                                         X.theItem(j-1) == A1.theItem(j));
                            LOOP3_ASSERT(i,index,j,
                                         X.theItem(j-1) == B1.theItem(j));
                            LOOP3_ASSERT(i,index,j,
                                         X.theItem(j-1) == C1.theItem(j));

                            LOOP3_ASSERT(i,index,j,
                                         X.theItem(j-1) == A2.theItem(j));
                            LOOP3_ASSERT(i,index,j,
                                         X.theItem(j-1) == B2.theItem(j));
                            LOOP3_ASSERT(i,index,j,
                                         X.theItem(j-1) == C2.theItem(j));

                        }
                    }
                } END_BSLMA_EXCEPTION_TEST
            }
        }

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING MUTATOR - 'append' METHODS:
        //   Tests the remaining append methods that haven't already been
        //   tested
        // Concerns:
        //   - That appendNullItems appends the correct number of elements
        //     and that those elements are have no selection
        //   - That appendItem(choiceArrayItem) appends only one element to the
        //     array and that its selection is the same as the ChoiceArrayItem
        //   - That appendItem(ChoiceArrayItem) behaves correctly with an
        //     aliased reference to an array item (e.g. an appending an item
        //     from an arrayitem within the array itself)
        // Plan:
        //   1.Specify a set S of (unique) objects with substantial and varied
        //     differences in value.  For each element in the set append from
        //     0 to K unset items.  Verify the K unset values have been
        //     appended and that the rest of the array was not affected.
        //   2.Specify a set S of (unique) objects with substantial and varied
        //     differences in value.  For each element, iterate through the
        //     types in its catalog calling appendItem(ChoiceArrayItem &) on
        //     a value of that type.  Verify that the correct value has been
        //     appended and that the rest of the array was not affected.
        //   3.Specify a set S of (unique) objects with substantial and varied
        //     differences in value.  Test for aliasing:
        //     append a value with a reference to the beginning of the array,
        //     append a value with a reference to the end of the array,
        //     append a value with a reference to the middle of the array.
        //     in each case verify the correct value was inserted at the
        //     correct index.
        //
        // Testing:
        //   void appendNullItems(int elemCount);
        //   void appendItem(int, const bdem_ChoiceArryItem&);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting appendItem METHODS"
                               << "\n======================" << bsl::endl;

        // Create a simplified set of test cases to append to
        static const struct TestRow {
                int         d_line;
                const char *d_catalogSpec;  // Specification to create
                                            // the catalog
        } DATA[] = {
            // Line     DescriptorSpec
            // ====     ==============
            { L_,       "" },
            { L_,       "A" },
            { L_,       "D" },
            { L_,       "AA" },
            { L_,       "GALKacK" },
        };
        static const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

        {
            {
                if (verbose) {
                    bsl::cout << "\tTest appendNullItems(i) for choice arrays"
                              << " based on data from a table of test specs"
                              << bsl::endl;
                }

                const int K = 4; // maximum # of elements to append
                bslma_TestAllocator testAllocator(veryVeryVerbose);
                bslma_TestAllocator &tAlloc = testAllocator;
                for (int i = 0; i < NUM_DATA; ++i) {
                    const char *SPEC  = DATA[i].d_catalogSpec;
                    const int   LEN   = bsl::strlen(SPEC);
                    BEGIN_BSLMA_EXCEPTION_TEST {
                        for (int numToAdd = 0; numToAdd < K; ++numToAdd) {
                            Catalog cat = ggCatalog(SPEC);
                            Obj mX(cat,&tAlloc); const Obj& X = mX;

                            // populate data
                            for (int k = 0; k < LEN; ++k) {
                                mX.appendItem(getChoiceA(SPEC,SPEC[k]));
                            }
                            Obj mY(mX,&tAlloc);  const Obj& Y = mY;

                            // test append
                            mX.appendNullItems(numToAdd);

                            LOOP_ASSERT(SPEC, Y.length()+numToAdd ==
                                              X.length());
                            // validate no previous values were changed
                            for (int k = 0; k < LEN; ++k) {
                                LOOP3_ASSERT(SPEC,numToAdd,k,
                                             X.theItem(k).selection() ==
                                             Y.theItem(k).selection());
                            }
                            for (int k = LEN; k<LEN+numToAdd;++k)
                            {
                                LOOP3_ASSERT(SPEC,numToAdd,k,
                                             X.theItem(k).selector() == -1);
                            }
                        }
                    } END_BSLMA_EXCEPTION_TEST
                }
            }
        }
        {
            if (verbose) {
                bsl::cout << "\tTest appendItems(ChoiceArrayItem &) for choice"
                          << " arrays based on data from a table of test specs"
                          << bsl::endl;
            }
            bslma_TestAllocator testAllocator(veryVeryVerbose);
            bslma_TestAllocator &tAlloc = testAllocator;

            for (int i = 0; i < NUM_DATA; ++i) {
                const char *SPEC  = DATA[i].d_catalogSpec;
                const int   LEN   = bsl::strlen(SPEC);

                // iterate over each element type (held in SPEC)
                for (int j = 0; j < LEN; ++j) {
                    BEGIN_BSLMA_EXCEPTION_TEST {
                        Catalog cat = ggCatalog(SPEC);
                        Obj mX(cat,&tAlloc); const Obj& X = mX;

                        // populate data
                        for (int k = 0; k < LEN; ++k) {
                            mX.appendItem(getChoiceA(SPEC,SPEC[k]));
                        }
                        Obj mY(mX,&tAlloc);  const Obj& Y = mY;

                        bdem_Choice tmpC(getChoiceA(SPEC,SPEC[j]).item(),
                                         &tAlloc);
                        mX.appendItem(tmpC);
                        LOOP_ASSERT(SPEC, Y.length()+1 == X.length());

                        // validate no previous values were changed
                        for (int k = 0; k < LEN; ++k) {
                            LOOP3_ASSERT(SPEC,SPEC[j], k,
                                         X.theItem(k).selection() ==
                                                     Y.theItem(k).selection());
                        }
                        LOOP2_ASSERT(SPEC,SPEC[j],
                                     X.theItem(LEN).selection() ==
                                     getChoiceA(SPEC,SPEC[j]).selection());
                    } END_BSLMA_EXCEPTION_TEST

                }
            }
        }
        {
            if (verbose) {
                bsl::cout << "\tTest appendItem(bdem_Choice) for aliasing "
                          << "issues"
                          << endl;
            }
            bslma_TestAllocator testAllocator(veryVeryVerbose);

            // iterate over element over our selection of test data
            for (int i = 0; i < NUM_DATA; ++i) {
                const char *SPEC  = DATA[i].d_catalogSpec;
                const int   LEN   = bsl::strlen(SPEC);

                Catalog cat =ggCatalog(SPEC);

                if (veryVerbose) {
                    bsl::cout << "Testing appendItem(Obj) for array "
                              << "based on spec '" << SPEC << "'" << endl;
                }
                BEGIN_BSLMA_EXCEPTION_TEST {
                    Obj mA(cat,&testAllocator); const Obj &A = mA;
                    for (int j=0; j<LEN;++j)
                    {
                        mA.appendItem(getChoiceA(SPEC, SPEC[j]));
                    }
                    mA.appendItem(Choice());

                    Obj mB(mA,&testAllocator); const Obj &B = mB;
                    Obj mC(mA,&testAllocator); const Obj &C = mC;

                    // create a control copy Y
                    Obj mX(mA,&testAllocator); const Obj &X = mX;

                    bdem_Choice tmpC(A.theItem(0), &testAllocator);
                    mA.appendItem(tmpC);
                    tmpC = B.theItem(B.length()-1);
                    mB.appendItem(tmpC);
                    tmpC = C.theItem((C.length()-1)/2);
                    mC.appendItem(tmpC);

                    // VERIFY THE APPENDED ITEM
                    // validate the index has been added
                    LOOP_ASSERT(i,X.length()+1 == A.length());
                    LOOP_ASSERT(i,X.length()+1 == B.length());
                    LOOP_ASSERT(i,X.length()+1 == C.length());
                    LOOP_ASSERT(i,X != A);
                    LOOP_ASSERT(i,X != B);
                    LOOP_ASSERT(i,X != C);

                    // verify we haven't modified any of the
                    // previous elements
                    for (int j=0;j<X.length();++j) {
                        LOOP2_ASSERT(i,j,X.theItem(j) == A.theItem(j));
                        LOOP2_ASSERT(i,j,X.theItem(j) == B.theItem(j));
                        LOOP2_ASSERT(i,j,X.theItem(j) == C.theItem(j));
                    }

                    // verify our new element
                    LOOP_ASSERT(i,X.theItem(0) == A.theItem(A.length()-1));
                    LOOP_ASSERT(i,
                           X.theItem(X.length()-1) == B.theItem(B.length()-1));
                    LOOP_ASSERT(i,X.theItem((X.length()-1)/2) ==
                                                      C.theItem(C.length()-1));

                } END_BSLMA_EXCEPTION_TEST
            }
        }

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING MUTATOR 'reset' FUNCTIONS:
        //
        // Concerns:
        //     Verify that when reset, the object has a state identical
        //     to its state after construction
        //
        // Plan:
        //   Specify a set S of (unique) objects with substantial and varied
        //   differences in value.  Construct and initialize objects mX1 & mX2
        //   using the set S.  Then construct an object mY with the same
        //   catalog taken from set S.  Use the two flavors of reset on mX1 and
        //   mX2 and verify that they are equal to mY.
        // Testing:
        //   void reset(const bdem_ElemType::Type [], int);
        //   void reset(const bsl::vector<bdem_ElemType::Type>& );
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "\nTesting reset Functions"
                      << "\n======================" << bsl::endl;
        }
        {

            bslma_TestAllocator testAllocator(veryVeryVerbose);
            bslma_TestAllocator &tAlloc = testAllocator;
            // iterate U over catalogs in S
            for (int i = 0; i < NUM_DATA; ++i) {
                const char *SPEC  = DATA[i].d_catalogSpec;
                const int   LEN   = bsl::strlen(SPEC);
                BEGIN_BSLMA_EXCEPTION_TEST {
                    if (veryVerbose) { P(SPEC); }

                    Catalog cat = ggCatalog(SPEC);

                    // create and populate mX1 and mX2
                    Obj mX1(cat,&tAlloc); const Obj &X1 = mX1;
                    populateData(&mX1,VALUES_A);
                    Obj mX2(mX1,&tAlloc); const Obj &X2 = mX2;

                    // create the control arrays
                    const int newIndex = (i+1) % NUM_DATA;
                    const char *NEWSPEC  = DATA[i].d_catalogSpec;
                    Catalog newCat = ggCatalog(NEWSPEC);
                    EType::Type *newCatP = (cat.size()>0) ? &cat.front() :
                                                            NULL;

                    Obj mY(newCat,&tAlloc); const Obj &Y=mY;

                    // reset
                    mX1.reset(newCat);
                    mX2.reset(newCatP, newCat.size());

                    // compare
                    LOOP3_ASSERT(i, X1, Y, X1==Y);
                    LOOP3_ASSERT(i, X2, Y, X2==Y);
                }END_BSLMA_EXCEPTION_TEST
            }
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING MANIPULATORS - 'theModifiableItem':
        //
        // Concerns:
        //      That theModifiableItem method returns a modifiable
        //      ChoiceArrayItem referring to the correct index in the choice
        //      array.  That it's value is the same as is returned by the const
        //      array.  That it's value is the same as is returned by the const
        //      operator[] and when modified the value is identical.
        //
        // Plan:
        //   Specify a set S of (unique) objects with substantial and varied
        //   differences in value.  For each element in the set iterate
        //   through all the indices.  For each index: Make a selection,
        //   compare the values returned by the two operator[] methods,
        //   assign a value to the ChoiceArrayItem returned by the
        //   theModifiableItem method, then perform a second comparison to
        //   verify the assignment.  Test for aliasing problems by assigning
        //   using another element from the beginning, middle, and end of the
        // Testing:
        //     bdem_ChoiceArrayItem& theModifiableItem(int );
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING non-const operator[] METHOD"
                               << "\n======================" << bsl::endl;

        {

            bslma_TestAllocator testAllocator(veryVeryVerbose);
            Catalog cat = ggCatalog(SPECIFICATIONS);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE  = DATA[i].d_line;
                const char *SPEC  = DATA[i].d_catalogSpec;
                const int   LEN   = bsl::strlen(SPEC);

                Obj mX(cat,  &testAllocator);
                const Obj& X = mX;

                if (veryVeryVerbose) {
                    bsl::cout << "\tTesting for '" << SPEC << "'" << bsl::endl;
                }

                LOOP_ASSERT(i,0 == X.length());
                for (int j=0; j<LEN;++j) {
                    // set the new element to a value and verify that value
                    // is set.
                    char type      = SPEC[j];
                    int selection = getElemType(type);

                    const Choice CA = getChoiceA(SPECIFICATIONS,SPEC[j]);
                    const Choice CB = getChoiceB(SPECIFICATIONS,SPEC[j]);

                    mX.appendItem(CA);
                    ASSERT (j == X.length()-1);
                    LOOP2_ASSERT(i,j,selection == X.theItem(j).selector());

                    // test the two elements initially return the same value
                    LOOP2_ASSERT(i,j, X.theItem(j) == mX.theModifiableItem(j));

                    // assign a value the selection held at that element
                    mX.theModifiableItem(j).selection().replaceValue(
                                                               CB.selection());

                    // verify the 2 methods return equal and correct results
                    LOOP2_ASSERT(i,j, X.theItem(j) == mX.theModifiableItem(j));
                    LOOP2_ASSERT(i,j,
                                 X.theItem(j).selection() == CB.selection());
                    LOOP2_ASSERT(i,j, mX.theModifiableItem(j).selection() ==
                                                               CB.selection());
                    LOOP2_ASSERT(i,j, X.theItem(j).selection() !=
                                                               CA.selection());
                    LOOP2_ASSERT(i,j, mX.theModifiableItem(j).selection() !=
                                                               CA.selection());
                }
                for (int j = 0; j < LEN; ++j) {
                    Obj tmp(mX);         const Obj& TMP = tmp;
                    // assign a value from the Beginning of the array
                    //(aliasing)
                    const int BS  = TMP.theItem(0).selector();
                    tmp.theModifiableItem(j).makeSelection(BS).replaceValue(
                                         tmp.theModifiableItem(0).selection());
                    // verify the 2 methods return equal and correct results
                    LOOP2_ASSERT(i,j, TMP.theItem(j) ==
                                                     tmp.theModifiableItem(j));
                    LOOP2_ASSERT(i,j, TMP.theItem(0) == TMP.theItem(j));

                    // assign a value from the End of the array (aliasing)
                    const int E  = TMP.length() - 1;
                    const int ES = TMP.theItem(E).selector();
                    tmp.theModifiableItem(j).makeSelection(ES).replaceValue(
                                         tmp.theModifiableItem(E).selection());
                    // verify the 2 methods return equal and correct results
                    LOOP2_ASSERT(i,j, TMP.theItem(j) ==
                                                     tmp.theModifiableItem(j));
                    LOOP2_ASSERT(i,j, TMP.theItem(E) == TMP.theItem(j));

                    // assign a value from the Middle of the array (aliasing)
                    const int M  = (tmp.length() - 1)/2;
                    const int MS =  TMP.theItem(M).selector();
                    tmp.theModifiableItem(j).makeSelection(MS).replaceValue(
                                         tmp.theModifiableItem(M).selection());
                    // verify the 2 methods return equal and correct results
                    LOOP2_ASSERT(i,j,
                                 TMP.theItem(j) == tmp.theModifiableItem(j));
                    LOOP2_ASSERT(i,j,  TMP.theItem(M) == TMP.theItem(j));
                }
            }
        }

      } break;
      case 11: {
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
        //   (u, v) in the cross product S x S, copy construct a control w from
        //   v, assign v to u, and assert that w == u and w == v.  Then test
        //   aliasing by copy constructing a control w from each u in S,
        //   assigning u to itself, and verifying that w == u.
        //
        // Testing:
        //   bdem_ChoiceArray& operator=(const bdem_ChoiceArray& rhs);
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << "\nTesting Assignment Operator"
                               << "\n===========================" << bsl::endl;

        {
            if (verbose) {
                bsl::cout << "\tTesting basic assignment" << bsl::endl;
            }

            for (int uI = 0; uI < NUM_DATA; ++uI) {
                // for each test in the DATA table
                const char *SPECU      = DATA[uI].d_catalogSpec;
                const int  LENU      = bsl::strlen(SPECU);

                bslma_TestAllocator tAlloc(veryVeryVerbose);
                const bslma_DefaultAllocatorGuard dag1(&tAlloc);
                for (int vI = 0; vI < NUM_DATA; ++vI) {
                    const char *SPECV      = DATA[vI].d_catalogSpec;
                    const int  LENV        = bsl::strlen(SPECV);

                    if (veryVerbose) {
                        bsl::cout << "\tTesting table spec '" << SPECU << "'"
                                  << " and '" << SPECV << "'" << bsl::endl;
                    }

                    Catalog catU = ggCatalog(SPECU);
                    Catalog catV = ggCatalog(SPECV);

                    Obj mV(catV,&tAlloc);             const Obj &V = mV;
                    Obj mU(catU,&tAlloc);             const Obj &U = mU;

                    // populate data
                    populateData(&mU, VALUES_A);
                    populateData(&mV, VALUES_A);

                    Obj mW(mV);     const Obj &W = mW;

                    // sanity check
                    LOOP2_ASSERT(uI,vI,(vI==uI) == (U==W));
                    LOOP2_ASSERT(uI,vI,(vI==uI) == (U==V));

                    // PERFORM ASSIGNMENT
                    mU = V;

                    LOOP2_ASSERT(uI,vI, (W==V) && (W==U));

                    // modify V, and ensure no change to U
                    if (catV.size() > 0) {
                        const void *VALUE = VALUES_B[catV[0]];
                        mV.theModifiableItem(0).selection().replaceValue(
                            getChoiceB(SPECV,SPECV[0]).selection());

                        LOOP2_ASSERT(uI,vI, U==W);
                        LOOP2_ASSERT(uI,vI, U!=V);
                    }
                }
            }

        }
        {
            if (verbose) {
                bsl::cout << "\tTesting aliasing" << bsl::endl;
            }

            for (int uI = 0; uI < NUM_DATA; ++uI) {
                // for each test in the DATA table

                const char *SPEC      = DATA[uI].d_catalogSpec;
                const int   LEN       = bsl::strlen(SPEC);

                bslma_TestAllocator tAlloc(veryVeryVerbose);

                if (veryVerbose) {
                    bsl::cout << "\tTesting table spec '" << SPEC << "'"
                              << bsl::endl;
                }

                Catalog cat = ggCatalog(SPEC);

                Obj mU(cat,&tAlloc);    const Obj &U = mU;
                // populate data
                for (int i =0; i < LEN; ++i) {
                    mU.appendItem(getChoiceA(SPEC, SPEC[i]));
                }

                Obj mW(U);     const Obj &W = mW;

                // PERFORM ASSIGNMENT
                mU = U;

                LOOP_ASSERT(uI,W==U);
            }

        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'bdex' STREAMING FUNCTIONALITY:
        //
        // Concerns:
        //   The 'bdex' streaming concerns for this component are standard.  We
        //   first probe the member functions 'outStream' and
        //   'inStream' in the manner of a "breathing test" to verify basic
        //   functionality.  We then thoroughly test streaming functionality
        //   relying on the bdex functions which forward appropriate calls
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
        //   1.PRELIMINARY MEMBER FUNCTION TEST
        //     First test 'maxSupportedBdexVersion' explicitly, and then
        //     perform a trivial direct test of the 'outStream' and 'inStream'
        //     methods (the rest of the testing will use the stream operators).
        //
        //   2.VALID STREAMS
        //     For the set S of globally-defined test values, use all
        //     combinations (u, v) in the cross product S X S, stream the
        //     value of v into (a temporary copy of) u and assert u == v.
        //
        //   3.EMPTY AND INVALID STREAMS
        //     For each u in S, create a copy and attempt to stream into it
        //     from an empty and then invalid stream.  Verify after each try
        //     that the object is unchanged and that the stream is invalid.
        //
        //   4.INCOMPLETE DATA
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
        //
        // Testing:
        //   int maxSupportedBdexVersion() const;
        //   bdex_InStream& streamIn(bdex_InStream& stream);
        //   bdex_InStream& streamIn(bdex_InStream& stream, int version);
        //   bdex_OutStream& streamOut(bdex_OutStream& stream) const;
        //   bdex_OutStream& streamOut(bdex_OutStream& stream, int) const;
        // --------------------------------------------------------------------

          if (verbose) bsl::cout << "\nTesting Streaming Functionality"
                                 << "\n==============================="
                                 << bsl::endl;

          const int MAX_VERSION = 3;
          {
              ASSERT(MAX_VERSION ==
                                  bdem_ChoiceArray::maxSupportedBdexVersion());
          }

          {

              bslma_TestAllocator tAlloc(veryVeryVerbose);
              const bslma_DefaultAllocatorGuard dag1(&tAlloc);

              for (int VERSION = 1; VERSION <= MAX_VERSION; ++VERSION) {
                  // iterate U over catalogs in S
                  for (int uI = 0; uI < NUM_DATA; ++uI) {
                      const char *SPECU  = DATA[uI].d_catalogSpec;

                      if (veryVerbose) { P(SPECU); }

                      Catalog catU = ggCatalog(SPECU);
                      Obj mU(catU, PASSTH, &tAlloc);
                      const Obj &U = mU;
                      populateData(&mU, VALUES_A);

                      Out os;
                      U.bdexStreamOut(os, VERSION);

                      // TEST 2.VALID STREAMS (case 1)
                      // Stream a constructed obj to an empty obj
                      {
                          Obj mA(&tAlloc);    const Obj& A = mA;

                          if (catU.size() > 0) {
                              LOOP_ASSERT(uI, U != A);
                          }

                          ASSERT(os);

                          In testInStream(os.data(), os.length());
                          BEGIN_BDEX_EXCEPTION_TEST {
                              testInStream.reload(os.data(),os.length());
                              testInStream.setSuppressVersionCheck(1);
                              ASSERT(testInStream);
                              mA.bdexStreamIn(testInStream, VERSION);
                          } END_BDEX_EXCEPTION_TEST
                          LOOP_ASSERT(uI, U == A);
                      }

                      // TEST 3. EMPTY AND INVALID STREAMS
                      // Stream from an empty and invalid stream
                      {
                          Obj mA(U, &tAlloc); const Obj& A = mA;
                          {
                              In testInStream;  // Empty stream

                              testInStream.setSuppressVersionCheck(1);

                              BEGIN_BDEX_EXCEPTION_TEST {
                                  testInStream.reload(NULL,0);
                                  mA.bdexStreamIn(testInStream, VERSION);
                                  ASSERT(!testInStream);
                              } END_BDEX_EXCEPTION_TEST
                              ASSERT(A == U);

                          }
                          {
                              In testInStream(os.data(), os.length());

                              BEGIN_BDEX_EXCEPTION_TEST {
                                  testInStream.setSuppressVersionCheck(1);
                                  testInStream.reload(os.data(),os.length());
                                  testInStream.invalidate();  // Invalid stream
                                  mA.bdexStreamIn(testInStream, VERSION);
                              } END_BDEX_EXCEPTION_TEST
                              ASSERT(A == U);
                              ASSERT(!testInStream);
                          }
                      }

                      for (int vI = 0; vI < NUM_DATA; ++vI) {
                          const char *SPECV  = DATA[vI].d_catalogSpec;

                          Catalog catV = ggCatalog(SPECV);
                          Obj mV(catV, &tAlloc); const Obj &V = mV;

                          for (int i = 0; i < catV.size(); ++i)
                          {
                              mV.appendItem(getChoiceA(SPECV, SPECV[i]));
                          }

                          // TEST 2.VALID STREAMS (case 2)
                          // Stream a constructed U into a temp copy of V
                          {
                              Obj tmpV(&tAlloc); const Obj& TMPV = tmpV;

                              In testInStream(os.data(), os.length());

                              testInStream.setSuppressVersionCheck(1);
                              BEGIN_BDEX_EXCEPTION_TEST {
                                  testInStream.reload(os.data(),os.length());
                                  tmpV.bdexStreamIn(testInStream, VERSION);
                                  LOOP2_ASSERT(uI,vI, U == TMPV);
                              } END_BDEX_EXCEPTION_TEST
                          }
                      }
                  }

                  // TEST 4. INCOMPLETE DATA
                  // Streaming in from an incomplete stream
                  {
                      bslma_TestAllocator tAlloc(veryVeryVerbose);
                      const bslma_DefaultAllocatorGuard dag1(&tAlloc);

                      Catalog cat = ggCatalog("A");
                      Obj mA(cat,&tAlloc);     const Obj& A = mA;
                      Obj mB(cat,&tAlloc);     const Obj& B = mB;
                      Obj mC(cat,&tAlloc);     const Obj& C = mC;

                      mA.appendItem(getChoiceA("A",'A'));
                      mB.appendItem(getChoiceB("A",'A'));
                      mC.appendItem(getChoiceN("A",'A'));

                      ASSERT(A != B);
                      ASSERT(A != C);
                      ASSERT(B != C);

                      Out os;
                      A.bdexStreamOut(os, VERSION);

                      const int LOD1 = os.length();
                      B.bdexStreamOut(os, VERSION);
                      const int LOD2 = os.length();
                      C.bdexStreamOut(os, VERSION);

                      const int LOD = os.length();
                      const char* const OD = os.data();

                      for (int bytes = 0; bytes < LOD; ++bytes) {
                          // create an in stream of the first i bytes
                          // of the output
                          In testInStream(OD,bytes);
                          In &in = testInStream;

                          Obj mX;    const Obj& X = mX;
                          Obj mY(A); const Obj& Y = mY;
                          mY.removeItems(0,A.length());

                          BEGIN_BDEX_EXCEPTION_TEST {
                              testInStream.reload(OD,bytes);

                              LOOP_ASSERT(bytes,testInStream);
                              LOOP_ASSERT(bytes,
                                          !bytes == testInStream.isEmpty());

                              Obj t1(X), t2(X), t3(X);

                              // The number of bytes on the stream determines
                              // whether there is enough data to deserialize
                              // into the test objects.
                              if (bytes < LOD1) {
                                  t1.bdexStreamIn(in,VERSION);
                                  LOOP_ASSERT(bytes, !in);
                                  if (0 == bytes)
                                      LOOP_ASSERT(bytes, X == t1);

                                  t2.bdexStreamIn(in,VERSION);
                                  LOOP_ASSERT(bytes, !in);
                                  LOOP_ASSERT(bytes, X == t2);

                                  t3.bdexStreamIn(in,VERSION);
                                  LOOP_ASSERT(bytes, !in);
                                  LOOP_ASSERT(bytes, X == t3);
                              }
                              else if (bytes < LOD2) {
                                  t1.bdexStreamIn(in,VERSION);
                                  LOOP_ASSERT(bytes,  in);
                                  LOOP_ASSERT(bytes, A == t1);

                                  t2.bdexStreamIn(in,VERSION);
                                  LOOP_ASSERT(bytes, !in);
                                  if (LOD1 == bytes)
                                      LOOP_ASSERT(bytes, X == t2);

                                  t3.bdexStreamIn(in,VERSION);
                                  LOOP_ASSERT(bytes, !in);
                                  LOOP_ASSERT(bytes, X == t3);
                              }
                              else {
                                  t1.bdexStreamIn(in,VERSION);
                                  LOOP_ASSERT(bytes,  in);
                                  LOOP_ASSERT(bytes, A == t1);

                                  t2.bdexStreamIn(in,VERSION);
                                  LOOP_ASSERT(bytes,  in);
                                  LOOP_ASSERT(bytes, B == t2);

                                  t3.bdexStreamIn(in,VERSION);
                                  LOOP_ASSERT(bytes, !in);
                                  if (LOD2 == bytes)
                                      LOOP_ASSERT(bytes, X == t3);
                              }

                              Obj blank(A); const Obj &BLANK = blank;
                              // not tested, but for our purposes its fine
                              blank.removeAll();

                              LOOP_ASSERT(bytes, C != BLANK);
                              t1 = BLANK;
                              LOOP_ASSERT(bytes, BLANK == t1);

                              LOOP_ASSERT(bytes, C != BLANK);
                              t2 = BLANK;
                              LOOP_ASSERT(bytes, BLANK == t2);

                              LOOP_ASSERT(bytes, C != BLANK);
                              t3 = BLANK;
                              LOOP_ASSERT(bytes, BLANK == t3);

                          } END_BDEX_EXCEPTION_TEST
                      }
                  }
              }
          }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR:
        //
        // Concerns:
        //   - That the specified or bslma_Default::defaultAllocator is used.
        //   - That the specified or default (BDEM_PASS_THROUGH) strategy is
        //     used.
        //   - Any value must be able to be copy constructed without affecting
        //     its argument.  For values that require dynamic allocation, each
        //     copy must allocate storage independently from its own allocator.
        //
        // Plan:
        //  1. Create a default allocator guard.  Create a test array using
        //     our default guard, create a test array using a allocator we
        //     explicitly specify.  Inspect the allocators to make sure when
        //     that when the objects are created memory is allocated from
        //     the correct allocator.
        //  2. Create arrays A1 & A2,using the 2 constructors under test
        //     and the BDEM_PASS_THROUGH allocation strategy,create a second
        //     set of arrays, B, using the BDEM_WRITE_ONCE alloc strategy.
        //     Add and remove elements from the arrays.  Ensure arrays using
        //     the same catalog allocated with BDEM_PASS_THROUGH have
        //     allocated an equal amount of memory.  Ensure that the memory
        //     allocated by BDEM_PASS_THROUGH arrays is less than the memory
        //     allocated by arrays using BDEM_WRITE_ONCE.
        //  3. Specify a set S whose elements have substantial and varied
        //     differences in value.  For each element in S, construct and
        //     initialize identically valued objects A and B, with different
        //     allocators, using tested methods.  Then copy construct objects
        //     X, Y, and Z using different constructors from A using a
        //     third allocator, and use the equality operator to
        //     assert that A,B,X,Y and Z have the same value.  Modify X,Y, and
        //     Z by changing one attribute and verify that A and B still have
        //     the same value but that X,Y, and Z are different from both.
        //     Repeat the entire process,but change different attributes of
        //     X,Y and Z  in the last step.
        //
        // Testing:
        //   bdem_ChoiceArray(const bdem_ChoiceArrayImp& original,
        //                  bslma_Allocator *);
        //   bdem_ChoiceArray(const bdem_ChoiceArrayImp& original,
        //                   bdem_AllocationStrategy,
        //                   bslma_Allocator *);
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "\nTesting Copy Constructor"
                      << "\n========================" << bsl::endl;
        }
        {
            if (veryVerbose) bsl::cout << "Verify the allocator is passed "
                                       << "correctly" << bsl::endl;

            // a simple catalog to work with
            const Catalog cat = ggCatalog(SPECIFICATIONS);
            bslma_TestAllocator badAlloc,alloc1,alloc2;

            // Set a default alloc guard
            Obj orig(cat,&badAlloc);  const Obj& ORIG = orig;
            const int OB = badAlloc.numBytesInUse();

            const bslma_DefaultAllocatorGuard dag1(&alloc1);

            ASSERT(0 == alloc1.numBytesInUse());
            ASSERT(0 == alloc2.numBytesInUse());

            // create an array using the default allocator
            int N=0;
            Obj a1(ORIG);
            ASSERT(N <  alloc1.numBytesInUse()); N = alloc1.numBytesInUse();
            ASSERT(0 == alloc2.numBytesInUse());
            ASSERT(OB == badAlloc.numBytesInUse());

            Obj a2(ORIG,PASSTH);
            ASSERT(N <  alloc1.numBytesInUse()); N = alloc1.numBytesInUse();
            ASSERT(0 == alloc2.numBytesInUse());
            ASSERT(OB  == badAlloc.numBytesInUse());

            int M=0;
            // create an array using alloc 2
            Obj b1(ORIG,&alloc2);
            ASSERT(N == alloc1.numBytesInUse());
            ASSERT(M < alloc2.numBytesInUse());  M = alloc2.numBytesInUse();
            ASSERT(OB  == badAlloc.numBytesInUse());

            Obj b2(ORIG,PASSTH,&alloc2);
            ASSERT(N == alloc1.numBytesInUse());
            ASSERT(M <  alloc2.numBytesInUse());
            ASSERT(OB  == badAlloc.numBytesInUse());

        }
        {
            if (veryVerbose) bsl::cout << "Verify the allocation mode is "
                                       << "passed correctly" << bsl::endl;

            // This test checks if a the allocation mode parameters is
            // passed to the delegating class.  It does not attempt to verify
            // that all the allocation modes behave correctly.

            // a simple catalog to work with
            const Catalog cat = ggCatalog(SPECIFICATIONS);

            bslma_TestAllocator alloc1,alloc2;
            bslma_TestAllocator alloc2B, badAlloc1, badAlloc2;

            Obj orig(cat,&badAlloc1); const Obj& ORIG = orig;
            const int badAllocBytes = badAlloc1.numBytesInUse();

            // Set a default alloc guard
            const bslma_DefaultAllocatorGuard dag1(&badAlloc2);

            // create an array using the BDEM_PASS_THROUGH mode
            Obj a1(ORIG,&alloc1);
            Obj a2(ORIG,PASSTH,&alloc2);

            // create an array using BDEM_WRITE_ONCE mode
            Obj b2(ORIG,AggOption::BDEM_WRITE_ONCE,&alloc2B);

            ASSERT(0 < alloc1.numBytesInUse());
            ASSERT(0 < alloc2.numBytesInUse());
            ASSERT(0 < alloc2B.numBytesInUse());
            ASSERT(badAllocBytes == badAlloc1.numBytesInUse());
            ASSERT(0 == badAlloc2.numBytesInUse());

            a1.insertNullItems(0,100); // NOTE: not tested yet
            a2.insertNullItems(0,100);
            b2.insertNullItems(0,100);

            // All the BDEM_PASS_THROUGH allocators should have used the
            // same amount of memory
            ASSERT(alloc1.numBytesInUse() == alloc2.numBytesInUse());

            // the array with a BDEM_PASS_THROUGH allocator should have
            // used less memory
            ASSERT(alloc2.numBytesInUse() < alloc2B.numBytesInUse());
            ASSERT(badAllocBytes == badAlloc1.numBytesInUse());
            ASSERT(0 == badAlloc2.numBytesInUse());

        }
        {
            if (veryVerbose) {
                bsl::cout << "\tTesting Copy Construction constructs arrays"
                          << " identical to the original" << bsl::endl;
            }
            for (int testI = 0; testI < NUM_DATA; ++testI) {
                // for each test in the DATA table

                const int   LINE      = DATA[testI].d_line;
                const char *SPEC  = DATA[testI].d_catalogSpec;
                const int   LEN       = bsl::strlen(SPEC);

                if (veryVerbose) {
                    bsl::cout << "\tTesting table spec '" << SPEC << "'"
                              << bsl::endl;
                }

                bslma_TestAllocator testAllocator(veryVeryVerbose);
                // Set a default alloc guard
                const bslma_DefaultAllocatorGuard dag1(&testAllocator);
#if !defined(BSLS_PLATFORM__CMP_MSVC)
                BEGIN_BSLMA_EXCEPTION_TEST {
#endif
                    Catalog cat = ggCatalog(SPEC);
                    Obj orig(cat,&testAllocator); const Obj& ORIG = orig;
                    populateData(&orig, VALUES_A);
                    {
                        // validate the copy constructed versions are identical
                        Obj a1(ORIG);                       const Obj& A1 = a1;
                        Obj a2(ORIG,&testAllocator);        const Obj& A2 = a2;
                        Obj b1(ORIG,PASSTH);                const Obj& B1 = a1;
                        Obj b2(ORIG,PASSTH,&testAllocator); const Obj& B2 = a2;

                        ASSERT(ORIG == A1);
                        ASSERT(ORIG == A2);
                        ASSERT(ORIG == B1);
                        ASSERT(ORIG == B2);
                    }

                    // perturb by modifying a value
                    if (LEN>0) {
                        Obj a1(ORIG);                       const Obj& A1 = a1;
                        Obj a2(ORIG,&testAllocator);        const Obj& A2 = a2;
                        Obj b1(ORIG,PASSTH);                const Obj& B1 = a1;
                        Obj b2(ORIG,PASSTH,&testAllocator); const Obj& B2 = a2;

                        ASSERT(ORIG == A1);
                        ASSERT(ORIG == A2);
                        ASSERT(ORIG == B1);
                        ASSERT(ORIG == B2);
                        Choice CB = getChoiceB(SPEC,SPEC[0]);

                        a1.theModifiableItem(0).selection().replaceValue(
                                                               CB.selection());
                        a2.theModifiableItem(0).selection().replaceValue(
                                                               CB.selection());
                        b1.theModifiableItem(0).selection().replaceValue(
                                                               CB.selection());
                        b2.theModifiableItem(0).selection().replaceValue(
                                                               CB.selection());

                        ASSERT((A1==A2) && (A2==B1) && (B1 == B2));
                        ASSERT(ORIG != A1);
                        ASSERT(ORIG != A2);
                        ASSERT(ORIG != B1);
                        ASSERT(ORIG != B2);
                    }
                    // perturb by adding an element
                    {
                        Obj a1(ORIG);                       const Obj& A1 = a1;
                        Obj a2(ORIG,&testAllocator);        const Obj& A2 = a2;
                        Obj b1(ORIG,PASSTH);                const Obj& B1 = a1;
                        Obj b2(ORIG,PASSTH,&testAllocator); const Obj& B2 = a2;

                        ASSERT(ORIG == A1);
                        ASSERT(ORIG == A2);
                        ASSERT(ORIG == B1);
                        ASSERT(ORIG == B2);

                        a1.appendItem(Choice());
                        a2.appendItem(Choice());
                        b1.appendItem(Choice());
                        b2.appendItem(Choice());

                        ASSERT((A1==A2) && (A2==B1) && (B1 == B2));
                        ASSERT(ORIG != A1);
                        ASSERT(ORIG != A2);
                        ASSERT(ORIG != B1);
                        ASSERT(ORIG != B2);
                    }
#if !defined(BSLS_PLATFORM__CMP_MSVC)
                } END_BSLMA_EXCEPTION_TEST
#endif
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTOR VARIANTS:
        //   Verify the behavior of ChoiceArray constructor variants that
        //   haven't already been tested.
        //
        // Concerns:
        //   - That the specified or bslma_Default::defaultAllocator is used.
        //   - That the specified or default (BDEM_PASS_THROUGH) strategy is
        //     used.
        //   - That the value is initialized properly.
        //   - That the destructor cleans up properly.
        // Plan:
        //  1. Create a default allocator guard.  Create a test array using
        //     our default guard, create a test array using a allocator we
        //     explicitly specify.  Inspect the allocators to make sure when
        //     that when the objects are created memory is allocated from
        //     the correct allocator.
        //  2. Create a set of array,A1-5, using the 5 constructors under test
        //     and the BDEM_PASS_THROUGH allocation strategy,create
        //     a second set of arrays, B, using the BDEM_WRITE_ONCE alloc
        //     strategy.  Add and remove elements from the arrays.  Ensure
        //     arrays using the same catalog allocated with BDEM_PASS_THROUGH
        //     have allocated an equal amount of memory.  Ensure that the
        //     memory allocated by BDEM_PASS_THROUGH arrays is less than the
        //     memory allocated by arrays using BDEM_WRITE_ONCE.
        //  3. For a varied series of test catalog selections S, create a
        //     ChoiceArray.  Ensure that selection type catalog  inspection
        //     methods return the expected values.
        //
        // Testing:
        //
        //    bdem_ChoiceArray(bslma_Allocator *basicAllocator = 0);
        //    bdem_ChoiceArray(
        //          bdem_AggregateOption::AllocationStrategy  allocMode,
        //          bslma_Allocator  *basicAllocator = 0);
        //    bdem_ChoiceArray(
        //          const bdem_ElemType::Type            typesCatalog[],
        //          int                                  typesCatalogLen,
        //          bslma_Allocator                      *basicAllocator = 0);
        //    bdem_ChoiceArray(
        //          const bdem_ElemType::Type            typesCatalog[],
        //          int                                  typesCatalogLen,
        //          bdem_AggregateOption::AllocationStrategy  allocMode,
        //          bslma_Allocator                     *basicAllocator = 0);
        //    bdem_ChoiceArray(
        //          const bsl::vector<bdem_ElemType::Type>&   typesCatalog,
        //          bdem_AggregateOption::AllocationStrategy  allocMode,
        //          bslma_Allocator                     *basicAllocator = 0);
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING OTHER CONSTRUCTORS"
                               << "\n==========================" << bsl::endl;
        {
            if (veryVerbose) bsl::cout << "Verify the allocator is passed "
                                       << "correctly" << bsl::endl;

            // a simple catalog to work with
            const Catalog cat = ggCatalog(SPECIFICATIONS);
            bslma_TestAllocator alloc1,alloc2;

            // Set a default alloc guard

            const bslma_DefaultAllocatorGuard dag1(&alloc1);

            ASSERT(0 == alloc1.numBytesInUse());
            ASSERT(0 == alloc2.numBytesInUse());

            // create an array using the default allocator
            int N=0;
            Obj a1;
            ASSERT(N <  alloc1.numBytesInUse()); N = alloc1.numBytesInUse();
            ASSERT(0 == alloc2.numBytesInUse());
            Obj a2(PASSTH);
            ASSERT(N < alloc1.numBytesInUse());  N = alloc1.numBytesInUse();
            ASSERT(0 == alloc2.numBytesInUse());

            Obj a3(&cat.front(),cat.size(), PASSTH);
            ASSERT(N < alloc1.numBytesInUse());  N = alloc1.numBytesInUse();
            ASSERT(0 == alloc2.numBytesInUse());

            Obj a4(&cat.front(),cat.size());
            ASSERT(N < alloc1.numBytesInUse());  N = alloc1.numBytesInUse();
            ASSERT(0 == alloc2.numBytesInUse());

            Obj a5(cat);
            ASSERT(N < alloc1.numBytesInUse());  N = alloc1.numBytesInUse();
            ASSERT(0 == alloc2.numBytesInUse());

            // create an array using alloc 2
            int M=0;
            Obj b1(&alloc2);
            ASSERT(N == alloc1.numBytesInUse());
            ASSERT(M < alloc2.numBytesInUse());  M = alloc2.numBytesInUse();

            Obj b2(PASSTH,&alloc2);
            ASSERT(N == alloc1.numBytesInUse());
            ASSERT(M <  alloc2.numBytesInUse()); M = alloc2.numBytesInUse();

            Obj b3(&cat.front(),cat.size(),PASSTH,&alloc2);
            ASSERT(N == alloc1.numBytesInUse());
            ASSERT(M < alloc2.numBytesInUse());  M = alloc2.numBytesInUse();

            Obj b4(&cat.front(),cat.size(),&alloc2);
            ASSERT(N == alloc1.numBytesInUse());
            ASSERT(M < alloc2.numBytesInUse());  M = alloc2.numBytesInUse();

            Obj b5(cat,&alloc2);
            ASSERT(N == alloc1.numBytesInUse());
            ASSERT(M < alloc2.numBytesInUse());
        }
        {
            if (veryVerbose) bsl::cout << "Verify the allocation mode is "
                                       << "passed correctly" << bsl::endl;

            // This test checks if a the allocation mode parameter is
            // passed to the delegating class.  It does not attempt to verify
            // that all the allocation modes behave correctly.

            // a simple catalog to work with
            const Catalog cat = ggCatalog(SPECIFICATIONS);

            bslma_TestAllocator alloc1,alloc2,alloc3,alloc4,alloc5;
            bslma_TestAllocator alloc2B,alloc3B, badAlloc;

            // Set a default alloc guard
            const bslma_DefaultAllocatorGuard dag1(&badAlloc);

            // create an array using the BDEM_PASS_THROUGH mode
            Obj a1(&alloc1);
            Obj a2(PASSTH,&alloc2);
            Obj a3(&cat.front(),cat.size(),PASSTH,&alloc3);
            Obj a4(&cat.front(),cat.size(),&alloc4);
            Obj a5(cat,&alloc5);
            // create an array using BDEM_WRITE_ONCE mode
            Obj b2(AggOption::BDEM_WRITE_ONCE,&alloc2B);
            Obj b3(&cat.front(),cat.size(),
                   AggOption::BDEM_WRITE_ONCE,&alloc3B);

            ASSERT(0 < alloc1.numBytesInUse());
            ASSERT(0 < alloc2.numBytesInUse());
            ASSERT(0 < alloc3.numBytesInUse());
            ASSERT(0 < alloc4.numBytesInUse());
            ASSERT(0 < alloc5.numBytesInUse());
            ASSERT(0 < alloc2B.numBytesInUse());
            ASSERT(0 < alloc3B.numBytesInUse());
            ASSERT(0 == badAlloc.numBytesInUse());

            a1.insertNullItems(0,100); // NOTE: not tested yet
            a2.insertNullItems(0,100);
            a3.insertNullItems(0,100);
            a4.insertNullItems(0,100);
            a5.insertNullItems(0,100);
            b2.insertNullItems(0,100);
            b3.insertNullItems(0,100);

            // All the BDEM_PASS_THROUGH allocators should have used the
            // same amount of memory
            ASSERT(alloc1.numBytesInUse() == alloc2.numBytesInUse());
            ASSERT(alloc3.numBytesInUse() == alloc4.numBytesInUse());
            ASSERT(alloc3.numBytesInUse() == alloc5.numBytesInUse());
            // the array with a BDEM_PASS_THROUGH allocator should have
            // used less memory
            ASSERT(alloc2.numBytesInUse() < alloc2B.numBytesInUse());
            ASSERT(alloc3.numBytesInUse() < alloc3B.numBytesInUse());

        }
        {
            if (veryVerbose) bsl::cout << "Verify the catalog is passed "
                                       << "correctly and the new array is "
                                       << "constructed using it." << bsl::endl;

            bslma_TestAllocator testAllocator;
            bslma_TestAllocator &ta = testAllocator;
            {
                BEGIN_BSLMA_EXCEPTION_TEST {
                    // test variants with no catalog
                    Obj a1(&ta);                          const Obj &A1 = a1;
                    Obj a2(PASSTH,&ta);                   const Obj &A2 = a2;

                    ASSERT(0 == A1.numSelections());
                    ASSERT(0 == A2.numSelections());
                } END_BSLMA_EXCEPTION_TEST

            }
            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE  = DATA[i].d_line;
                const char *SPEC  = DATA[i].d_catalogSpec;
                const int   LEN   = bsl::strlen(SPEC);
                BEGIN_BSLMA_EXCEPTION_TEST {

                    const Catalog CATALOG = ggCatalog(SPEC);
                    const EType::Type *cPtr = (CATALOG.size()>0) ?
                                                   &CATALOG.front() : NULL;

                    Obj a3(cPtr,CATALOG.size(),PASSTH,&ta); const Obj &A3 = a3;
                    Obj a4(cPtr,CATALOG.size(),&ta);        const Obj &A4 = a4;
                    Obj a5(CATALOG,&ta);                    const Obj &A5 = a5;

                    LOOP_ASSERT(SPEC, LEN == A3.numSelections());
                    LOOP_ASSERT(SPEC, LEN == A4.numSelections());
                    LOOP_ASSERT(SPEC, LEN == A5.numSelections());
                    for (int j = 0; j < LEN; ++j) {
                        LOOP2_ASSERT(SPEC, j,
                                   getElemType(SPEC[j])==A3.selectionType(j));
                        LOOP2_ASSERT(SPEC, j,
                                   getElemType(SPEC[j])==A4.selectionType(j));
                        LOOP2_ASSERT(SPEC, j,
                                   getElemType(SPEC[j])==A5.selectionType(j));

                    }
                    Catalog cat3,cat4,cat5;
                    A3.selectionTypes(&cat3);
                    A4.selectionTypes(&cat4);
                    A5.selectionTypes(&cat5);
                    LOOP_ASSERT(SPEC, CATALOG == cat3);
                    LOOP_ASSERT(SPEC, CATALOG == cat4);
                    LOOP_ASSERT(SPEC, CATALOG == cat5);
                } END_BSLMA_EXCEPTION_TEST
            }

        }

      } break;
      case 7: {
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
        //  1 Specify a set S of unique object values.  For each object value
        //    iterate over the catalog of types, the array of elements, and
        //    the allocator.  On each iteration perturbate the values (or not),
        //    perturbate the catalog (or not).  Verify the correctness of
        //    'operator==' and 'operator!='
        //
        // Testing:
        //   operator==(const bdem_ChoiceArray&, const bdem_ChoiceArray&);
        //   operator!=(const bdem_ChoiceArray&, const bdem_ChoiceArray&);
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "\nTesting Equality Operators"
                      << "\n==========================" << bsl::endl;
        }

        {
            if (veryVerbose) {
                bsl::cout << "\tTesting equality choice array"
                          << bsl::endl;
            }

            bslma_TestAllocator testAllocator1(veryVeryVerbose);
            bslma_TestAllocator tempAlloc(veryVeryVerbose);
            bdema_SequentialAllocator  testAllocator2(&tempAlloc);

            // create a structure holding our variations of allocators
            // and allocation strategy (to lower the number of loop
            // nestings used)
            const struct {
                    const AggOption::AllocationStrategy  strat1;
                    const AggOption::AllocationStrategy  strat2;
                    bslma_Allocator *alloc1;
                    bslma_Allocator *alloc2;
            } ALLOC[] = {
                {
                    PASSTH,
                    PASSTH,
                    &testAllocator1,
                    &testAllocator1
                },
                {
                    PASSTH,
                    PASSTH,
                    &testAllocator1,
                    &testAllocator2
                },
                {
                    AggOption::BDEM_WRITE_ONCE,
                    AggOption::BDEM_PASS_THROUGH,
                    &testAllocator1,
                    &testAllocator1
                },
                {
                    AggOption::BDEM_WRITE_ONCE,
                    AggOption::BDEM_PASS_THROUGH,
                    &testAllocator1,
                    &testAllocator2,
                },
            };

            const int NUM_ALLOC = sizeof(ALLOC)/sizeof(*ALLOC);

            // iterate over allocation strategy, test specification,
            // catalog perturbation, and index perturbation.

            for (int allocI = 0;allocI < NUM_ALLOC; ++ allocI) {
                // for each combination of allocators

                const AggOption::AllocationStrategy strat1 =
                    ALLOC[allocI].strat1;
                const AggOption::AllocationStrategy strat2 =
                    ALLOC[allocI].strat2;
                bslma_Allocator *alloc1 = ALLOC[allocI].alloc1;
                bslma_Allocator *alloc2 = ALLOC[allocI].alloc2;

                for (int testI = 0; testI < NUM_DATA; ++testI) {
                    // for each test in the DATA table

                    const int   LINE      = DATA[testI].d_line;
                    const char *SPEC      = DATA[testI].d_catalogSpec;
                    const int   LEN       = bsl::strlen(SPEC);

                    for (int i=0;i<LEN;++i) {
                        // Attempt to perturbate the catalog
                        char SPECB[100];
                        bsl::strcpy(SPECB,SPEC);

                        if (LEN==0) {
                            bsl::strcpy(SPECB,"A");
                        } else {
                            const EType::Type type = getElemType(SPEC[i]);
                            const EType::Type newType = (EType::Type)
                                                        ((type+1) % SPEC_LEN);

                            SPECB[i] = SPECIFICATIONS[newType];
                        }

                        Catalog catA = ggCatalog(SPEC);
                        Catalog catB = ggCatalog(SPECB);

                        Obj caA1(catA,strat1,alloc1); const Obj &CAA1 = caA1;
                        Obj caA2(catA,strat2,alloc2); const Obj &CAA2 = caA2;
                        Obj caB1(catB,strat1,alloc1); const Obj &CAB1 = caB1;
                        Obj caB2(catB,strat2,alloc2); const Obj &CAB2 = caB2;

                        ASSERT(CAA1 == CAA2);
                        ASSERT(CAB1 == CAB2);
                        ASSERT(!(CAA1 != CAA2));
                        ASSERT(!(CAB1 != CAB2));

                        ASSERT(!(CAA1 == CAB1));
                        ASSERT(!(CAA2 == CAB2));
                        ASSERT(CAA1 != CAB1);
                        ASSERT(CAA2 != CAB2);
                    }

                    for (int pertIndex=0 ;pertIndex<LEN;++pertIndex) {
                        // perturbate the data

                        Catalog cat = ggCatalog(SPEC);

                        Obj caA1(cat,strat1,alloc1); const Obj &CAA1 = caA1;
                        Obj caA2(cat,strat2,alloc2); const Obj &CAA2 = caA2;
                        Obj caB1(cat,strat1,alloc1); const Obj &CAB1 = caB1;
                        Obj caB2(cat,strat2,alloc2); const Obj &CAB2 = caB2;

                        // populate the data in our arrays
                        for (int j = 0; j<LEN; ++j) {
                            Choice cA = getChoiceA(SPEC,SPEC[j]);
                            const Choice &CA = cA;
                            // for the b arrays, perturbate the value
                            // if we are at the specified index
                            Choice cB = (pertIndex!=j) ? CA :
                                                      getChoiceB(SPEC,SPEC[j]);
                            const Choice &CB = cB;

                            caA1.appendItem(CA);
                            caA2.appendItem(CA);

                            caB1.appendItem(CB);
                            caB2.appendItem(CB);
                        }

                        ASSERT(CAA1 == CAA2);
                        ASSERT(CAB1 == CAB2);
                        ASSERT(!(CAA1 != CAA2));
                        ASSERT(!(CAB1 != CAB2));

                        ASSERT(!(CAA1 == CAB1));
                        ASSERT(!(CAA2 == CAB2));
                        ASSERT(CAA1 != CAB1);
                        ASSERT(CAA2 != CAB2);
                    }
                }
            }
        }

      } break;
      case 6: {
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
                      bsl::cout << "\tTesting empty choice array"
                                << bsl::endl; }
        {
            const char *EXP_P1 = "    {\n"
                                 "        Selection Types: [\n"
                                 "        ]\n"
                                 "    }\n";
            const char *EXP_P2 = "{\n"
                                 "        Selection Types: [\n"
                                 "        ]\n"
                                 "    }\n";
            const char *EXP_P3 = "    { Selection Types: [ ] }";
            const char *EXP_P4 = "{ Selection Types: [ ] }";
            const char *EXP_OP = "{ Selection Types: [ ] }";

            Obj mX; const Obj& X = mX;

            bsl::ostringstream os1, os2, os3, os4, os5;
            X.print(os1, 1, 4);
            X.print(os2, -1, 4);
            X.print(os3, 1, -4);
            X.print(os4, -1, -4);
            os5 << X;

            LOOP2_ASSERT(EXP_P1,
                         os1.str(),
                         0 == bsl::strcmp(EXP_P1, os1.str().c_str()));
            LOOP2_ASSERT(EXP_P2,
                         os2.str(),
                         0 == bsl::strcmp(EXP_P2, os2.str().c_str()));
            LOOP2_ASSERT(EXP_P3,
                         os3.str(),
                         0 == bsl::strcmp(EXP_P3, os3.str().c_str()));
            LOOP2_ASSERT(EXP_P4,
                         os4.str(),
                         0 == bsl::strcmp(EXP_P4, os4.str().c_str()));
            LOOP2_ASSERT(EXP_OP,
                         os5.str(),
                         0 == bsl::strcmp(EXP_OP, os5.str().c_str()));
        }

            const int MAX_PRINT_TESTS = 4;
            const struct {
                    int         d_line;       // Line number
                    const char  *d_spec;       // Type Specification string
                    char        d_valueSpec;  // Spec specifying the value
                                              // for that
                    // type.  Can be one of 'A', 'B' or 'N'
                    // corresponding to the A, B or N value
                    // for that type.
                    struct {
                            int         d_level;      // Level to print at
                            int         d_spacesPerLevel; // spaces per level
                                                          // to print at
                            const char *d_printExpOutput; // Expected o/p from
                                                          // print
                    } PRINT_TESTS[MAX_PRINT_TESTS];
                    const char *d_outputOpExpOutput; // Expected o/p from
                                                     // operator<<
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
                            "        Selection Types: [\n"
                            "            CHAR\n"
                            "        ]\n"
                            "        Item 0: {\n"
                            "            CHAR A\n"
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
                            "        Item 0: {\n"
                            "            CHAR A\n"
                            "        }\n"
                            "    }\n",
                        },
                        {
                            1,
                            -4,
                            "    { Selection Types: [ CHAR ] "
                            "Item 0: { CHAR A } }",
                        },
                        {
                            -1,
                            -4,
                            "{ Selection Types: [ CHAR ] Item 0: { CHAR A } }",
                        },
                    },
                    "{ Selection Types: [ CHAR ] Item 0: { CHAR A } }"
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
                            "        Selection Types: [\n"
                            "            INT\n"
                            "        ]\n"
                            "        Item 0: {\n"
                            "            INT 10\n"
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
                            "        Item 0: {\n"
                            "            INT 10\n"
                            "        }\n"
                            "    }\n",
                        },
                        {
                            1,
                            -4,
                            "    { Selection Types: [ INT ] Item 0: { INT 10 }"
                            " }",
                        },
                        {
                            -1,
                            -4,
                            "{ Selection Types: [ INT ] Item 0: { INT 10 } }",
                        },
                    },
                    "{ Selection Types: [ INT ] Item 0: { INT 10 } }"
                },
                {
                    L_,
                    "AC",
                    'A',
                    {
                        {
                            1,
                            4,
                            "    {\n"
                            "        Selection Types: [\n"
                            "            CHAR\n"
                            "            INT\n"
                            "        ]\n"
                            "        Item 0: {\n"
                            "            CHAR A\n"
                            "        }\n"
                            "        Item 1: {\n"
                            "            INT 10\n"
                            "        }\n"
                            "    }\n",
                        },
                        {
                            -1,
                            4,
                            "{\n"
                            "        Selection Types: [\n"
                            "            CHAR\n"
                            "            INT\n"
                            "        ]\n"
                            "        Item 0: {\n"
                            "            CHAR A\n"
                            "        }\n"
                            "        Item 1: {\n"
                            "            INT 10\n"
                            "        }\n"
                            "    }\n",
                        },
                        {
                            1,
                            -4,
                            "    { Selection Types: [ CHAR INT ] "
                            "Item 0: { CHAR A } Item 1: { INT 10 } }",
                        },
                        {
                            -1,
                            -4,
                            "{ Selection Types: [ CHAR INT ] "
                            "Item 0: { CHAR A } Item 1: { INT 10 } }",
                        },
                    },
                    "{ Selection Types: [ CHAR INT ] "
                    "Item 0: { CHAR A } Item 1: { INT 10 } }"
                },
                {
                    L_,
                    "AMG",
                    'A',
                    {
                        {
                            1,
                            4,
                            "    {\n"
                            "        Selection Types: [\n"
                            "            CHAR\n"
                            "            INT_ARRAY\n"
                            "            STRING\n"
                            "        ]\n"
                            "        Item 0: {\n"
                            "            CHAR A\n"
                            "        }\n"
                            "        Item 1: {\n"
                            "            INT_ARRAY [\n"
                            "                10\n"
                            "            ]\n"
                            "        }\n"
                            "        Item 2: {\n"
                            "            STRING one\n"
                            "        }\n"
                            "    }\n",
                        },
                        {
                            -1,
                            4,
                            "{\n"
                            "        Selection Types: [\n"
                            "            CHAR\n"
                            "            INT_ARRAY\n"
                            "            STRING\n"
                            "        ]\n"
                            "        Item 0: {\n"
                            "            CHAR A\n"
                            "        }\n"
                            "        Item 1: {\n"
                            "            INT_ARRAY [\n"
                            "                10\n"
                            "            ]\n"
                            "        }\n"
                            "        Item 2: {\n"
                            "            STRING one\n"
                            "        }\n"
                            "    }\n",
                        },
                        {
                            1,
                            -4,
                            "    { Selection Types: [ CHAR INT_ARRAY STRING ] "
                            "Item 0: { CHAR A } "
                            "Item 1: { INT_ARRAY [ 10 ] } "
                            "Item 2: { STRING one } }",
                        },
                        {
                            -1,
                            -4,
                            "{ Selection Types: [ CHAR INT_ARRAY STRING ] "
                            "Item 0: { CHAR A } "
                            "Item 1: { INT_ARRAY [ 10 ] } "
                            "Item 2: { STRING one } }",
                        },
                    },
                    "{ Selection Types: [ CHAR INT_ARRAY STRING ] "
                    "Item 0: { CHAR A } "
                    "Item 1: { INT_ARRAY [ 10 ] } "
                    "Item 2: { STRING one } }"
                },
        };
        const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE     = DATA[i].d_line;
            const char *SPEC     = DATA[i].d_spec;
            const char  VAL_SPEC = DATA[i].d_valueSpec;
            const char *OP_EXP   = DATA[i].d_outputOpExpOutput;

            bslma_TestAllocator alloc1(veryVeryVerbose);
            Catalog cat = ggCatalog(SPEC);
            Obj mX(cat, PASSTH,&alloc1);    const Obj& X = mX;

            // Set Element j, to a catalog value of j
            for (int j = 0; j < cat.size(); ++j)
            {
                const char SPEC_V  = SPEC[j];
                const Desc *DESC   = getDescriptor(SPEC_V);
                EType::Type TYPE   = getElemType(SPEC_V);
                const Choice VAL_A = getChoiceA(SPEC, SPEC_V);
                Choice value; const Choice &VALUE = value;
                if ('A' == VAL_SPEC) {
                    value = getChoiceA(SPEC,SPEC_V);
                }
                else if ('B' == VAL_SPEC) {
                    value = getChoiceB(SPEC,SPEC_V);
                }
                else if ('N' == VAL_SPEC) {
                    value = getChoiceN(SPEC,SPEC_V);
                }
                else {
                    ASSERT(0);
                }
                mX.appendItem(VALUE);
            }

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
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING HELPER FUNCTIONS populateData
        //   Ensure that the helper functions to create a
        //   an array of data from a specification string works correctly.
        //
        // Concerns:
        //   - Given valid input the generator function 'populateData'
        //     creates the correct array definition.
        // Plan:
        //   1. Test an empty spec string
        //   2. Test all single element spec strings
        //   3. Test a long spec string containing at least 1
        //      of each element.
        //
        // Tactics:
        //   - Brute force implementation
        //
        // Testing:
        //   void populateData(bdem_ChoiceArrayImp *, const char *);
        //
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << bsl::endl
                      <<"TESTING HELPER FUNCTION populateData" << bsl::endl
                      <<"=======================================" << bsl::endl;
        }
        {
            bslma_TestAllocator tAlloc;
            if (verbose) {
                bsl::cout << "\tTESTING 'populateData'" << bsl::endl;
            }
            {
                if (veryVerbose) {
                    bsl::cout << "\tTesting with empty string" << bsl::endl;
                }
                // create and populate mX
                Obj mX(NULL,0,PASSTH,&tAlloc);       const Obj &X = mX;
                populateData(&mX,VALUES_A);

                ASSERT(0 == X.length());
                ASSERT(0 == X.numSelections());
            }
            {
                if (veryVerbose) {
                    bsl::cout << "Testing all types" << bsl::endl;
                }
                char spec [] = "%";
                for (int i = 0; i < SPEC_LEN; ++i)  {
                    spec[0] = SPECIFICATIONS[i];
                    Catalog c = ggCatalog(spec);

                    // create and populate mX
                    Obj mX(c,&tAlloc);    const Obj &X = mX;
                    populateData(&mX,VALUES_A);

                    ASSERT(1 == X.length());
                    ASSERT(0 == X.theItem(0).selector());
                    EType::Type type = (EType::Type)i;
                    CERef value(VALUES_A[i], getDescriptor(type));
                    ASSERT(value == X.theItem(0).selection());
                }
            }
            {
                if (veryVerbose) {
                    bsl::cout << "\tTesting a large invocation touching"
                              << "all tokens multiple times" << bsl::endl;
                }
                // create a specification string
                // and a matching vector of ETypes
                bsl::string spec;
                Catalog indexVec;
                // cover all the types, add each a few times
                for (int i = 0; i < (4*SPEC_LEN); ++i)    {
                    int r = i % SPEC_LEN;
                    spec += SPECIFICATIONS[r];
                    indexVec.push_back(getElemType(SPECIFICATIONS[r]));
                }

                Catalog c = ggCatalog(spec.data());
                ASSERT (c.size() == spec.length());
                ASSERT (c.size() == indexVec.size());

                // create and populate mX
                Obj mX(c,&tAlloc);          const Obj &X = mX;

                // add some empty items to double check that we remove
                // existing dat when populating an array
                mX.insertNullItems(0,5);

                populateData(&mX,VALUES_B);
                ASSERT(c.size()==mX.length());
                for (int i = 0; i < mX.length() ; ++i) {
                    LOOP_ASSERT(c[i],i==X.theItem(i).selector());
                    EType::Type type = indexVec[i];
                    CERef value(VALUES_B[(int)type], getDescriptor(type));
                    LOOP_ASSERT(c[i],value == X.theItem(i).selection());
                }
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC MANIPULATORS (BOOTSTRAP) AND PRIMARY ACCESSORS:
        //
        // Concerns:
        //   We are able to create a 'bdem_ChoiceArray' object and put it into
        //   any legal state.  We are not concerned in this test about trying
        //   to move the object from one state to another.
        //
        // Plan:
        //   A 'bdem_ChoiceArray' object can be put into any state using the
        //   two-argument constructor and the 'appendItem' manipulator.
        //   The state of the object can be determined using the
        //   primary accessors.
        //
        //   1. Build objects containing 3 different values (including the
        //     "unset" value) of each of data types by first constructing each
        //      object, then using 'appendItem' to add a choice to it.  Using
        //      the primary accessors, verify the descriptor, value, and
        //      selector, catalog of each constructed object.
        //   2.Specify a set S of (unique) objects with substantial and varied
        //     differences in value.  For each array test for aliasing issues
        //     with the appendItem method:
        //     append a value with a reference to the beginning of the array,
        //     append a value with a reference to the end of the array,
        //     append a value with a reference to the middle of the array.
        //     in each case verify the correct value was inserted at the
        //     correct index.
        //
        // Testing:
        //   void appendItem(const bdem_Choice& src);
        //   int length() const;
        //   const bdem_ChoiceArrayItem& operator[](int index) const;
        //   const bdem_ChoiceArrayItem& theItem(int index) const;
        //   bool isEmpty() const;
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING BASIC MANIPULATORS"
                               << "\n==========================" << bsl::endl;

        {
            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE  = DATA[i].d_line;
                const char *SPEC  = DATA[i].d_catalogSpec;
                const int   LEN   = bsl::strlen(SPEC);

                const Catalog CATALOG = ggCatalog(SPEC);

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

                      // verify we start with an empty array
                      Obj mX(CATALOG, mode, alloc); const Obj& X = mX;
                      LOOP_ASSERT(LINE, LEN        == X.numSelections());
                      LOOP_ASSERT(LINE, 0          == X.length());
                      LOOP_ASSERT(LINE,
                                  EType::BDEM_VOID == X.selectionType(-1));
                      Catalog tmpCatalog;
                      X.selectionTypes(&tmpCatalog);
                      LOOP_ASSERT(LINE, CATALOG    == tmpCatalog);
                      LOOP_ASSERT(LINE, X.isEmpty());

                      for (int j = 0; j < LEN; ++j) {
                          // append a value to each of our test arrays
                          const char   S     = SPEC[j];
                          EType::Type  TYPE  = getElemType(S);
                          const Choice VAL_A = getChoiceA(SPEC, S);
                          const Choice VAL_B = getChoiceB(SPEC, S);
                          const Choice VAL_N = getChoiceN(SPEC, S);
                          int          arrayLen = 3 * j;

                          if (veryVerbose) { P(i) P(j) P(VAL_A) P(VAL_B)
                                             P(VAL_N) P(X) }

                          mX.appendItem(VAL_A);
                          LOOP_ASSERT(S, LEN          == X.numSelections());
                          LOOP_ASSERT(S, arrayLen + 1 == X.length());
                          LOOP_ASSERT(S,
                                     EType::BDEM_VOID == X.selectionType(-1));
                          LOOP_ASSERT(S, TYPE         == X.selectionType(j));
                          LOOP_ASSERT(S,
                                      compare(mX.theModifiableItem(arrayLen),
                                              VAL_A, S));
                          LOOP_ASSERT(S, compare(X.theItem(arrayLen),
                                                 VAL_A, S));
                          LOOP_ASSERT(S, compare(X[arrayLen], VAL_A, S));

                          tmpCatalog.clear();
                          X.selectionTypes(&tmpCatalog);
                          LOOP_ASSERT(LINE, CATALOG == tmpCatalog);

                          mX.appendItem(VAL_B);
                          ++arrayLen;
                          LOOP_ASSERT(S, LEN          == X.numSelections());
                          LOOP_ASSERT(S, arrayLen + 1 == X.length());
                          LOOP_ASSERT(S,
                                     EType::BDEM_VOID == X.selectionType(-1));
                          LOOP_ASSERT(S, TYPE         == X.selectionType(j));
                          LOOP_ASSERT(S,
                                      compare(mX.theModifiableItem(arrayLen),
                                              VAL_B, S));
                          LOOP_ASSERT(S, compare(X.theItem(arrayLen),
                                                 VAL_B, S));
                          LOOP_ASSERT(S, compare(X[arrayLen], VAL_B, S));

                          tmpCatalog.clear();
                          X.selectionTypes(&tmpCatalog);
                          LOOP_ASSERT(LINE, CATALOG == tmpCatalog);

                          mX.appendItem(VAL_N);
                          ++arrayLen;
                          LOOP_ASSERT(S, LEN          == X.numSelections());
                          LOOP_ASSERT(S, arrayLen + 1 == X.length());
                          LOOP_ASSERT(S,
                                     EType::BDEM_VOID == X.selectionType(-1));
                          LOOP_ASSERT(S, TYPE         == X.selectionType(j));
                          LOOP_ASSERT(S,
                                      compare(mX.theModifiableItem(arrayLen),
                                              VAL_N, S));
                          LOOP_ASSERT(S, compare(X.theItem(arrayLen),
                                                 VAL_N, S));
                          LOOP_ASSERT(S, compare(X[arrayLen], VAL_N, S));

                          tmpCatalog.clear();
                          X.selectionTypes(&tmpCatalog);
                          LOOP_ASSERT(LINE, CATALOG == tmpCatalog);

                          if (veryVerbose) { P(X) }
                      }
                   } END_BSLMA_EXCEPTION_TEST
                }
            }
        }
        {
            // Use a small set of test specs for alignment test
            static const struct TestRow {
                    int         d_line;
                    const char *d_catalogSpec;  // Specification to create
                    // the catalog
            } DATA[] = {
                // Line     DescriptorSpec
                // ====     ==============
                { L_,       "" },
                { L_,       "A" },
                { L_,       "D" },
                { L_,       "AA" },
                { L_,       "GALKacK" },
            };
            static const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

            if (verbose) {
                cout << "\tTest appendItem(bdem_Choice) for aliasing "
                     << "issues"
                     << endl;
            }
            // don't use aggregate types in the test spec
            bslma_TestAllocator testAllocator(veryVeryVerbose);

            // iterate over element over our selection of test data
            for (int i = 0; i < NUM_DATA; ++i) {
                const char *SPEC  = DATA[i].d_catalogSpec;
                const int   LEN   = bsl::strlen(SPEC);

                Catalog cat = ggCatalog(SPEC);

                if (veryVerbose) {
                    cout << "Testing appendItem(Obj) for "
                         << "array based on spec '" << SPEC << "'" << endl;
                }
                BEGIN_BSLMA_EXCEPTION_TEST {
                    Obj mA(cat,&testAllocator); const Obj &A = mA;
                    for (int j=0; j<LEN;++j)
                    {
                        mA.appendItem(getChoiceA(SPEC, SPEC[j]));
                    }
                    mA.appendItem(Choice());

                    Obj mB(mA,&testAllocator); const Obj &B = mB;
                    Obj mC(mA,&testAllocator); const Obj &C = mC;

                    // create a control copy
                    Obj mX(mA,&testAllocator); const Obj &X = mX;

                    mA.appendItem(Choice(A[0]));
                    mB.appendItem(Choice(B[B.length()-1]));
                    mC.appendItem(Choice(C[(C.length()-1)/2]));

                    // VERIFY THE APPENDED ITEM
                    // validate the index has been added
                    LOOP_ASSERT(i,X.length()+1 == A.length());
                    LOOP_ASSERT(i,X.length()+1 == B.length());
                    LOOP_ASSERT(i,X.length()+1 == C.length());
                    LOOP_ASSERT(i,X != A);
                    LOOP_ASSERT(i,X != B);
                    LOOP_ASSERT(i,X != C);

                    // verify we haven't modified any of the
                    // previous elements
                    for (int j=0;j<X.length();++j) {
                        LOOP2_ASSERT(i,j,X.theItem(j) == A.theItem(j));
                        LOOP2_ASSERT(i,j,X.theItem(j) == B.theItem(j));
                        LOOP2_ASSERT(i,j,X.theItem(j) == C.theItem(j));
                        LOOP2_ASSERT(i,j,X[j] == A[j]);
                        LOOP2_ASSERT(i,j,X[j] == B[j]);
                        LOOP2_ASSERT(i,j,X[j] == C[j]);
                    }

                    // verify our new element
                    LOOP_ASSERT(i,X.theItem(0) == A.theItem(A.length()-1));
                    LOOP_ASSERT(i,X.theItem(X.length()-1) ==
                                                      B.theItem(B.length()-1));
                    LOOP_ASSERT(i,X.theItem((X.length()-1)/2) ==
                                                      C.theItem(C.length()-1));
                    LOOP_ASSERT(i,X[0] == A[A.length()-1]);
                    LOOP_ASSERT(i,X[X.length()-1] == B[B.length()-1]);
                    LOOP_ASSERT(i,X[(X.length()-1)/2] == C[C.length()-1]);
                } END_BSLMA_EXCEPTION_TEST
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY CONSTRUCTOR AND ACCESSORS (BOOTSTRAP)
        //
        // Concerns:
        //   - We are able to create a 'bdem_ChoiceArray' with a catalog.
        //   - That the specified default allocator or the specified allocator
        //           is used.
        //   - That the agg option behaves correctly
        //
        //   Since the ChoiceArray delegates its behavior to implementation we
        //   only need to ensure that the appropriate values are passed
        //   down correctly.
        //
        // Plan:
        //  1. Create a default allocator guard.  Create a test array using
        //     our default guard, create a test array using a allocator we
        //     explicitly specify.  Inspect the allocators to make sure when
        //     that when the objects are created memory is allocated from
        //     the correct allocator.
        //  2. Create an array, A, using the constructor under test
        //     and the BDEM_PASS_THROUGH allocation strategy,create
        //     a second array, B, using the BDEM_WRITE_ONCE alloc strategy.
        //     Add and remove elements from the  arrays.  Ensure that the
        //     memory allocated by the PASS_THROUGH array is less than the
        //     memory allocated by the array using WRITE_ONCE.
        //  3. For a varied series of test catalog selections S, create a
        //     ChoiceArray.  Ensure that selection type catalog  inspection
        //     methods return the expected values.
        //
        // Testing:
        //   bdem_ChoiceArray(const vector<EType::Type>&  typesCatalog,
        //                    AggOption::AllocationStrategy allocMode,
        //                    bslma_Allocator            *basicAlloc = 0);
        //   ~bdem_ChoiceArray();
        //   int numSelections() const;
        //   bdem_ElemType::Type selectionType(int selectionIndex) const;
        //   void selectionTypes(bsl::vector<EType::Type> *result) const;
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING PRIMARY CONSTRUCTOR AND ACCESSORS"
                               << "\n========================================="
                               << bsl::endl;

        {
            if (veryVerbose) bsl::cout << "Verify the allocator is passed "
                                       << "correctly" << bsl::endl;

            // a simple catalog to work with
            const Catalog cat = ggCatalog(SPECIFICATIONS);
            bslma_TestAllocator alloc1,alloc2;

            // Set a default alloc guard
            const bslma_DefaultAllocatorGuard dag1(&alloc1);

            ASSERT(0 == alloc1.numBlocksTotal());
            ASSERT(0 == alloc2.numBlocksTotal());

            // create an array using the default allocator
            Obj a(cat,PASSTH);
            ASSERT(0 < alloc1.numBytesTotal());
            ASSERT(0 == alloc2.numBytesTotal());
            const int N = alloc1.numBytesTotal();

            // create an array using alloc 2
            Obj b(cat,PASSTH, &alloc2);
            ASSERT(N == alloc1.numBytesTotal());
            ASSERT(0  < alloc2.numBytesTotal());
        }
        {
            if (veryVerbose) bsl::cout << "Verify the allocation mode is "
                                       << "passed correctly" << bsl::endl;

            // This test checks if a the allocation mode parameters is
            // passed to the delegating class.  It does not attempt to verify
            // that all the allocation modes behave correctly.

            // a simple catalog to work with
            const Catalog cat = ggCatalog(SPECIFICATIONS);

            bslma_TestAllocator alloc1,alloc2, alloc3;

            // Set a default alloc guard
            const bslma_DefaultAllocatorGuard dag1(&alloc3);
            int NA1,NA2,NB1,NB2;

            {
                // create an array using the BDEM_PASS_THROUGH mode
                Obj a(cat,AggOption::BDEM_PASS_THROUGH, &alloc1);
                Obj b(cat,AggOption::BDEM_WRITE_ONCE,   &alloc2);

                ASSERT(0 < alloc1.numBytesInUse());
                ASSERT(0 < alloc2.numBytesInUse());
                ASSERT(0 == alloc3.numBytesInUse());
                NA1 = alloc1.numBytesInUse();
                NB1 = alloc1.numBytesInUse();
                ASSERT(0 == alloc3.numBytesInUse());
                const char *S = SPECIFICATIONS;
                a.insertNullItems(0,100);
                b.insertNullItems(0,100);
                ASSERT(NA1 < alloc1.numBytesInUse());
                ASSERT(NB1 < alloc2.numBytesInUse());

                NA2 = alloc1.numBytesInUse();
                NB2 = alloc2.numBytesInUse();

                a.removeAllItems(); // NOTE: not tested yet
                b.removeAllItems(); // NOTE: not tested yet

                // the array with a BDEM_PASS_THROUGH allocator should have
                // used less memory
                ASSERT(alloc1.numBytesInUse() < alloc2.numBytesInUse());

            }

        }
        {
            if (veryVerbose) bsl::cout << "Verify the catalog is passed "
                                       << "correctly and the new array is "
                                       << "constructed using it." << bsl::endl;

            bslma_TestAllocator testAllocator;
            bslma_TestAllocator &ta = testAllocator;
            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE  = DATA[i].d_line;
                const char *SPEC  = DATA[i].d_catalogSpec;
                const int   LEN   = bsl::strlen(SPEC);
                BEGIN_BSLMA_EXCEPTION_TEST {
                    const Catalog CATALOG = ggCatalog(SPEC);
                    Obj x(CATALOG,AggOption::BDEM_PASS_THROUGH,&ta);
                    const Obj &X = x;

                    LOOP_ASSERT(SPEC, LEN == X.numSelections());
                    for (int j = 0; j < LEN; ++j) {
                        LOOP2_ASSERT(SPEC, j,
                                     getElemType(SPEC[j])==X.selectionType(j));
                    }
                    Catalog cat;
                    X.selectionTypes(&cat);
                    LOOP_ASSERT(SPEC, CATALOG == cat);
                } END_BSLMA_EXCEPTION_TEST
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
        //   Catalog ggCatalog(const char *spec);
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
            Catalog x1 = ggCatalog("");
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

                Catalog x1 = ggCatalog(tempSpec);
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

                Catalog x1 = ggCatalog(SPEC);
                LOOP3_ASSERT(LINE, LEN, x1.size(), LEN == x1.size());

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
        //   We want to exercise basic value-semantic functionality.  In
        //   particular we want to demonstrate a base-line level of correct
        //   operation of the following methods and operators:
        //      - default and copy constructors (and also the destructor)
        //      - the assignment operator (including aliasing)
        //      - equality operators: 'operator==' and 'operator!='
        //      - the output operator: 'operator<<'
        //      - primary manipulators: 'append' and 'removeAll' methods
        //      - basic accessors: 'length' and 'operator[]'
        //   In addition we would like to exercise objects with potentially
        //   different internal organizations representing the same value.
        //
        // Plan:
        //   After each of these steps, check that all attributes of the
        //   bdem_ChoiceArray object are as expected, check that memory is
        //   being used as expected, and (if in verbose mode), print the value
        //   of the created or modified object.
        //   1. Create an object, X1, using the default constructor.
        //   2. Copy construct a new object, X2, from X1
        //   3. Reset X1's catalog and append an element A  to X1
        //   3. Reset X2's catalog and append A to X2, verify X1==X2
        //   4. Append an element B to X2, verify X1!=X2.
        //   5. Remove all elements from X1
        //   6. For various test specs perform the following
        //      a. Append unset and set values to the array
        //      b. Remove an Item
        //      c. bdex Stream the array into a new array
        //      d. Assign one array to another
        //      e. Copy Construct
        //      f. Reset
        // Testing:
        //   This Test Case exercises basic value-semantic functionality.
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nBREATHING TEST"
                               << "\n==============" << bsl::endl;

        if (verbose) bsl::cout << "\tSizeof bdem_ChoiceArray :"
                               << sizeof(bdem_ChoiceArray) << bsl::endl;

        bslma_TestAllocator defaultAllocator;  // default allocation
        const bslma_DefaultAllocatorGuard DAG(&defaultAllocator);

        bslma_TestAllocator testAllocator(veryVeryVerbose);
        bslma_TestAllocator &alloc = testAllocator;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose)
        {
          bsl::cout << "\n 1. Create an object x1 (default ctor)."
                       "\t\t\t{ x1: }" << bsl::endl;
        }

        Obj mX1(&testAllocator);  const Obj& X1 = mX1;

        if (veryVerbose) {
          T_; P(X1);
        }
        if (veryVerbose) {
          bsl::cout << "\ta. Check initial state of x1." << bsl::endl;
        }
        ASSERT(0 == X1.length());

        if (veryVerbose) {
          bsl::cout << "\tb. Try equality operators: x1 <op> x1."
                    << bsl::endl;
        }
        ASSERT(X1 == X1 == 1);
        ASSERT(X1 != X1 == 0);
        LOOP_ASSERT(defaultAllocator.numBlocksTotal(),
                                     0 == defaultAllocator.numBlocksTotal());

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) {
          bsl::cout << "\n 2. Create a second object x2 (copy from x1)."
                       "\t\t{ x1: x2: }" << bsl::endl;
        }
        Obj mX2(X1, &testAllocator);  const Obj& X2 = mX2;

        if (veryVerbose) {
          T_;  P(X2);
        }

        if (veryVerbose) {
          bsl::cout << "\ta. Check the initial state of x2." << bsl::endl;
        }
        ASSERT(0 == X2.length());

        if (veryVerbose) {
          bsl::cout << "\tb. Try equality operators: x2 <op> x1, x2."
                    << bsl::endl;
        }
        ASSERT(X2 == X1 == 1);          ASSERT(X2 != X1 == 0);
        ASSERT(X2 == X2 == 1);          ASSERT(X2 != X2 == 0);
        LOOP_ASSERT(defaultAllocator.numBlocksTotal(),
                                     0 == defaultAllocator.numBlocksTotal());

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) {
          bsl::cout << "\n 3. Append an element value A to x1)."
                       "\t\t\t{ x1:A x2: }" << bsl::endl;
        }
        EType::Type catalog1[] = { EType::BDEM_INT, EType::BDEM_STRING };
        mX1.reset(catalog1, 2);
        Choice choice(catalog1, 2, &testAllocator);
        choice.makeSelection(0).theModifiableInt() = A02;
        mX1.appendItem(choice);

        if (veryVerbose) {
          T_;  P(X1);
        }

        if (veryVerbose) {
          bsl::cout << "\ta. Check new state of x1." << bsl::endl;
        }
        ASSERT(2 == X1.numSelections());
        ASSERT(EType::BDEM_INT == X1.selectionType(0));
        ASSERT(1 == X1.length());
        ASSERT(A02 == X1.theItem(0).theInt());

        if (veryVerbose) {
          bsl::cout << "\tb. Try equality operators: x1 <op> x1, x2."
                    << bsl::endl;
        }
        ASSERT(X1 == X1 == 1);          ASSERT(X1 != X1 == 0);
        ASSERT(X1 == X2 == 0);          ASSERT(X1 != X2 == 1);
        LOOP_ASSERT(defaultAllocator.numBlocksTotal(),
                                     0 == defaultAllocator.numBlocksTotal());

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) {
          bsl::cout << "\n 4. Append the same element value A to x2)."
                       "\t\t{ x1:A x2:A }" << bsl::endl;
        }
        mX2.reset(catalog1, 2);
        mX2.appendItem(choice);

        if (veryVerbose) {
          T_;  P(X2);
        }

        if (veryVerbose) {
          bsl::cout << "\ta. Check new state of x2." << bsl::endl;
        }
        ASSERT(2 == X2.numSelections());
        ASSERT(EType::BDEM_INT == X2.selectionType(0));
        ASSERT(1 == X2.length());
        ASSERT(A02 == X2.theItem(0).theInt());

        if (veryVerbose) {
          bsl::cout << "\tb. Try equality operators: x2 <op> x1, x2."
                    << bsl::endl;
        }
        ASSERT(X2 == X1 == 1);          ASSERT(X2 != X1 == 0);
        ASSERT(X2 == X2 == 1);          ASSERT(X2 != X2 == 0);
        LOOP_ASSERT(defaultAllocator.numBlocksTotal(),
                                     0 == defaultAllocator.numBlocksTotal());

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) {
          bsl::cout << "\n 5. Append another element value B to x2)."
                       "\t\t{ x1:A x2:AB }" << bsl::endl;
        }
        Choice choice2(catalog1, 2, &testAllocator);
        choice2.makeSelection(1).theModifiableString() = A06;

        mX2.appendItem(choice2);
        if (veryVerbose) {
          T_;  P(X2);
        }

        if (veryVerbose) {
          bsl::cout << "\ta. Check new state of x2." << bsl::endl;
        }
        ASSERT(2 == X2.numSelections());
        ASSERT(EType::BDEM_STRING == X2.selectionType(1));
        ASSERT(2 == X2.length());
        ASSERT(A02 == X2.theItem(0).theInt());
        ASSERT(A06 == X2.theItem(1).theString());
        ASSERT(2 == X2.length());

        if (veryVerbose) {
          bsl::cout << "\tb. Try equality operators: x2 <op> x1, x2."
                    << bsl::endl;
        }
        ASSERT(X2 == X1 == 0);          ASSERT(X2 != X1 == 1);
        ASSERT(X2 == X2 == 1);          ASSERT(X2 != X2 == 0);
        LOOP_ASSERT(defaultAllocator.numBlocksTotal(),
                                     0 == defaultAllocator.numBlocksTotal());

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) {
          bsl::cout << "\n 6. Remove all elements from x1."
                       "\t\t\t{ x1: x2:AB }" << bsl::endl;
        }
        mX1.removeAll();
        if (veryVerbose) {
          T_;  P(X1);
        }

        if (veryVerbose) {
          bsl::cout << "\ta. Check new state of x1." << bsl::endl;
        }
        ASSERT(0 == X1.length());

        if (veryVerbose) {
          bsl::cout << "\tb. Try equality operators: x1 <op> x1, x2."
                    << bsl::endl;
        }
        ASSERT(X1 == X1 == 1);          ASSERT(X1 != X1 == 0);
        ASSERT(X1 == X2 == 0);          ASSERT(X1 != X2 == 1);

        if (veryVerbose) bsl::cout << "\n\tTesting different specs"
                                   << "\n\t======================="
                                   << bsl::endl;

        {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const int   LINE  = DATA[i].d_line;
                    const char *SPEC  = DATA[i].d_catalogSpec;
                    const int   LEN   = bsl::strlen(SPEC);

                    bslma_TestAllocator testAllocator(veryVeryVerbose);
                    bslma_TestAllocator &alloc = testAllocator;
                    Catalog catalog = ggCatalog(SPEC);

//                 BEGIN_BSLMA_EXCEPTION_TEST { // removed for performance

                     Obj mX(catalog, &alloc);      const Obj& X = mX;

                     LOOP_ASSERT(i, catalog.size() == X.numSelections());
                     LOOP_ASSERT(i, 0              == X.length());

                     for (int k = 0; k < LEN; ++k) {
                        const char   S    = SPEC[k];
                        const Desc  *DESC = getDescriptor(S);
                        EType::Type  TYPE = getElemType(S);

                        const void *VAL_A  = getValueA(S);
                        const void *VAL_B  = getValueB(S);
                        const void *VAL_N  = getValueN(S);

                        const bdem_ElemRef    EREF_A(const_cast<void *>(VAL_A),
                                                        DESC);
                        const bdem_ConstElemRef CEREF_A(VAL_A, DESC);

                        const bdem_ElemRef    EREF_B(const_cast<void *>(VAL_B),
                                                        DESC);
                        const bdem_ConstElemRef CEREF_B(VAL_B, DESC);

                        const bdem_ElemRef    EREF_N(const_cast<void *>(VAL_N),
                                                        DESC);
                        const bdem_ConstElemRef CEREF_N(VAL_N, DESC);

                        if (veryVerbose){P_(LINE) P_(i) P_(k) P_(SPEC) P(S) N_}

                        mX.appendNullItems(1);
                        LOOP_ASSERT(S, catalog.size() == X.numSelections());
                        LOOP_ASSERT(S, k + 1          == X.length());
                        LOOP_ASSERT(S, TYPE           == X.selectionType(k));

                        if (veryVeryVerbose) { PL(LINE) P_(i) P_(k) P_(X) N_}

                        bdem_ChoiceArrayItem& mI = mX.theModifiableItem(k);
                        mI.makeSelection(k);
                        assign(&mI, CEREF_A, S);
                        if (veryVeryVerbose) { PL(LINE) P_(i) P_(k) P_(X) N_}
                        LOOP_ASSERT(S, catalog.size() == X.numSelections());
                        LOOP_ASSERT(S, k + 1          == X.length());
                        LOOP_ASSERT(S, TYPE           == X.selectionType(k));
                        LOOP3_ASSERT(S,  EREF_A, X.theItem(k).selection(),
                                EREF_A == mX.theModifiableItem(k).selection());
                        LOOP_ASSERT(S, CEREF_A == X.theItem(k).selection());
                        LOOP_ASSERT(S,
                                EREF_B != mX.theModifiableItem(k).selection());
                        LOOP_ASSERT(S, CEREF_B != X.theItem(k).selection());

                        bdem_Choice tmpC(mI, &alloc);
                        mX.appendItem(tmpC);
                        if (veryVeryVerbose) { PL(LINE) P_(i) P_(k) P_(X) N_}
                        LOOP_ASSERT(S, catalog.size() == X.numSelections());
                        LOOP_ASSERT(S, k + 2          == X.length());
                        LOOP_ASSERT(S, TYPE           == X.selectionType(k));
                        LOOP_ASSERT(S,
                                EREF_A == mX.theModifiableItem(k).selection());
                        LOOP_ASSERT(S,
                                    CEREF_A == X.theItem(k).selection());
                        LOOP_ASSERT(S,
                            EREF_A == mX.theModifiableItem(k + 1).selection());
                        LOOP_ASSERT(S,
                                    CEREF_A == X.theItem(k + 1).selection());

                        bdem_ChoiceArrayItem& mJ = mX.theModifiableItem(k + 1);
                        assign(&mJ, CEREF_B, S);
                        if (veryVeryVerbose) { PL(LINE) P_(i) P_(k) P_(X) N_}
                        LOOP_ASSERT(S, catalog.size() == X.numSelections());
                        LOOP_ASSERT(S, k + 2          == X.length());
                        LOOP_ASSERT(S, TYPE           == X.selectionType(k));
                        LOOP_ASSERT(S,
                                EREF_A == mX.theModifiableItem(k).selection());
                        LOOP_ASSERT(S,
                                    CEREF_A == X.theItem(k).selection());
                        LOOP_ASSERT(S,  EREF_B        ==
                                    mX.theModifiableItem(k + 1).selection());
                        LOOP_ASSERT(S, CEREF_B        ==
                                    X.theItem(k + 1).selection());

                        {
                            const int NUM_ITEMS = 2;
                            mX.insertNullItems(0, NUM_ITEMS);
                            if (veryVeryVerbose) {
                                P_(LINE) P_(i) P_(k) P_(X) N_
                            }
                            LOOP_ASSERT(S, catalog.size()==X.numSelections());
                            LOOP_ASSERT(S, k + 4         ==X.length());
                            LOOP_ASSERT(S, TYPE        == X.selectionType(k));
                            LOOP_ASSERT(S, EType::BDEM_VOID ==
                                   mX.theModifiableItem(0).selection().type());
                            LOOP_ASSERT(S, EType::BDEM_VOID ==
                                        X.theItem(0).selection().type());
                            LOOP_ASSERT(S, EType::BDEM_VOID ==
                                   mX.theModifiableItem(1).selection().type());
                            LOOP_ASSERT(S, EType::BDEM_VOID ==
                                        X.theItem(1).selection().type());
                            LOOP_ASSERT(S, EREF_A  ==
                                      mX.theModifiableItem(k + 2).selection());
                            LOOP_ASSERT(S, CEREF_A ==
                                        X.theItem(k + 2).selection());
                            LOOP_ASSERT(S,  EREF_B  ==
                                      mX.theModifiableItem(k + 3).selection());
                            LOOP_ASSERT(S, CEREF_B  ==
                                        X.theItem(k + 3).selection());

                            mX.removeItems(0, NUM_ITEMS);
                            if (veryVeryVerbose) {
                                P_(LINE) P_(i) P_(k) P_(X) N_
                            }
                            LOOP_ASSERT(S, catalog.size() ==
                                        X.numSelections());
                            LOOP_ASSERT(S, k + 2   == X.length());
                            LOOP_ASSERT(S, TYPE    == X.selectionType(k));
                            LOOP_ASSERT(S,
                                EREF_A == mX.theModifiableItem(k).selection());
                            LOOP_ASSERT(S,
                                        CEREF_A == X.theItem(k).selection());
                            LOOP_ASSERT(S,
                            EREF_B == mX.theModifiableItem(k + 1).selection());
                            LOOP_ASSERT(S,
                                      CEREF_B == X.theItem(k + 1).selection());
                        }

                        {
                            const int NUM_ITEMS = 2;
                            mX.insertNullItems(X.length(), NUM_ITEMS);
                            if (veryVeryVerbose) {
                                PL(LINE) P_(i) P_(k) P_(X) N_}
                            LOOP_ASSERT(S, catalog.size()==X.numSelections());
                            LOOP_ASSERT(S, k + 4 == X.length());
                            LOOP_ASSERT(S, TYPE  == X.selectionType(k));
                            LOOP_ASSERT(S,
                                EREF_A == mX.theModifiableItem(k).selection());
                            LOOP_ASSERT(S,
                                        CEREF_A == X.theItem(k).selection());
                            LOOP_ASSERT(S,
                            EREF_B == mX.theModifiableItem(k + 1).selection());
                            LOOP_ASSERT(S,
                                      CEREF_B == X.theItem(k + 1).selection());
                            LOOP_ASSERT(S, EType::BDEM_VOID    ==
                               mX.theModifiableItem(k + 2).selection().type());
                            LOOP_ASSERT(S, EType::BDEM_VOID    ==
                                        X.theItem(k + 2).selection().type());
                            LOOP_ASSERT(S, EType::BDEM_VOID    ==
                               mX.theModifiableItem(k + 3).selection().type());
                            LOOP_ASSERT(S, EType::BDEM_VOID    ==
                                        X.theItem(k + 3).selection().type());

                            mX.removeItems(X.length() - 2, NUM_ITEMS);
                            if (veryVeryVerbose) {
                                PL(LINE) P_(i) P_(k) P_(X) N_}
                            LOOP_ASSERT(S, catalog.size() ==
                                        X.numSelections());
                            LOOP_ASSERT(S, k + 2          ==
                                        X.length());
                            LOOP_ASSERT(S, TYPE           ==
                                        X.selectionType(k));
                            LOOP_ASSERT(S,  EREF_A        ==
                                        mX.theModifiableItem(k).selection());
                            LOOP_ASSERT(S, CEREF_A        ==
                                        X.theItem(k).selection());
                            LOOP_ASSERT(S,  EREF_B        ==
                                      mX.theModifiableItem(k + 1).selection());
                            LOOP_ASSERT(S, CEREF_B        ==
                                        X.theItem(k + 1).selection());
                        }

                        mX.removeItem(k + 1);
                        if (veryVeryVerbose) { PL(LINE) P_(i) P_(k) P_(X) N_}
                        LOOP_ASSERT(S, catalog.size() == X.numSelections());
                        LOOP_ASSERT(S, k + 1          == X.length());
                        LOOP_ASSERT(S, TYPE           == X.selectionType(k));
                        LOOP_ASSERT(S,
                                EREF_A == mX.theModifiableItem(k).selection());
                        LOOP_ASSERT(S,
                                    CEREF_A == X.theItem(k).selection());

                        if (veryVerbose) { bsl::cout << "bdex Streaming"
                                                     << bsl::endl; }
                        {
                            if (veryVerbose) {
                                P(X);
                            }
                            bdex_TestOutStream os;
                            X.bdexStreamOut(os, 3);

                            bdex_TestInStream is(os.data(), os.length());
                            Obj mY(&alloc);              const Obj& Y = mY;

                            LOOP_ASSERT(S,   X != Y);
                            LOOP_ASSERT(S, !(X == Y));

                            mY.bdexStreamIn(is, 3);

                            LOOP3_ASSERT(S, X, Y, X == Y);
                            LOOP_ASSERT(S, !(X != Y));
                            if (veryVeryVerbose) {
                                PL(LINE) P_(i) P_(k) P_(Y) N_}
                        }

                        if (veryVerbose) { bsl::cout << "Assignment operator"
                                                     << bsl::endl; }
                        {
                            Obj mY(&alloc); const Obj& Y = mY;

                            LOOP_ASSERT(S,   X != Y);
                            LOOP_ASSERT(S, !(X == Y));

                            mY = X;

                            LOOP_ASSERT(S,   X == Y);
                            LOOP_ASSERT(S, !(X != Y));
                            if (veryVeryVerbose) {
                                PL(LINE) P_(i) P_(k) P_(Y) N_}
                        }

                        if (veryVerbose) { bsl::cout << "Copy constructor"
                                                     << bsl::endl; }
                        {
                            Obj mY(X, &alloc); const Obj& Y = mY;

                            LOOP_ASSERT(S,   X == Y);
                            LOOP_ASSERT(S, !(X != Y));
                            if (veryVeryVerbose) {
                                PL(LINE) P_(i) P_(k) P_(Y) N_}
                        }
                    }

                    mX.removeAllItems();
                    LOOP_ASSERT(i, catalog.size() == X.numSelections());
                    LOOP_ASSERT(i, 0 == X.length());
                    if (veryVeryVerbose) { PL(LINE) P_(i) P_(X) N_}

                    mX.reset(0, 0);
                    LOOP_ASSERT(i, 0 == X.numSelections());
                    LOOP_ASSERT(i, 0 == X.length());
                    if (veryVeryVerbose) { PL(LINE) P_(i) P_(X) N_}
//                  } END_BSLMA_EXCEPTION_TEST
                }
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
