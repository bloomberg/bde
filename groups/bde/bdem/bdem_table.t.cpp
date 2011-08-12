// bdem_table.t.cpp                                                   -*-C++-*-

#include <bdem_table.h>

#include <bdem_choice.h>                   // for testing only
#include <bdem_choicearray.h>              // for testing only
#include <bdem_list.h>                     // for testing only
#include <bdem_row.h>                      // for testing only

#include <bdema_multipoolallocator.h>      // for testing only
#include <bdet_datetime.h>
#include <bdet_datetimetz.h>
#include <bdet_date.h>
#include <bdet_datetz.h>
#include <bdet_time.h>
#include <bdet_timetz.h>
#include <bdetu_unset.h>                   // for testing only
#include <bdeu_printmethods.h>             // for testing only
#include <bdex_byteinstream.h>             // for testing only
#include <bdex_byteoutstream.h>            // for testing only
#include <bdex_testinstream.h>             // for testing only
#include <bdex_testoutstream.h>            // for testing only

#include <bslma_default.h>                 // for testing only
#include <bslma_defaultallocatorguard.h>   // for testing only
#include <bslma_testallocator.h>           // for testing only
#include <bslma_testallocatorexception.h>  // for testing only
#include <bslmf_isconvertible.h>           // for testing only
#include <bsls_platform.h>                 // for testing only
#include <bsls_types.h>                    // for testing only

#include <bsl_algorithm.h>
#include <bsl_new.h>                       // placement syntax
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_strstream.h>

#include <bsl_cstdlib.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// With just over 75 methods, the 'bdem_Table' class serves as the
// presentation view for 'bdem' table functionality, and "knows about" all 32
// 'bdem' element types.  As part of its contract, 'bdem_Table' contains
// exactly one data member of type 'bdem_TableImp' -- hence it is possible to
// cast safely from one table type (address) to the other.  Although
// 'bdem_Table' is a fully value-semantic class, all of the substantive
// implementation resides in 'bdem_TableImp', which, in turn, resides in a
// separate (lower-level) component.
//
// Our goal here is primarily to ensure correct function signatures and that
// function arguments and return values are being propagated properly.
// Important issues related to the various allocation strategies, aliasing,
// thread safety, and exception neutrality are presumed to have been thoroughly
// tested via the lower-level interface.
//
// Finally note that both 'bdem_List' and 'bdem_Table' are implemented as
// peers.  For the purposes of testing, however, we will require
// that 'bdem_list' has already been tested and is available for use in testing
// (and therefore building) 'bdem_table'.  That is, 'bdem_list.cpp' (for the
// purposes of testing only) will -- by fiat -- be forced to "gratuitously"
// include 'bdem_list.h' just as 'bdem_list.t.cpp' (and therefore -- by fiat
// -- also 'bdem_list.cpp') currently depends on 'bdem_Row'.
//
//       Primary Constructors, Primary Manipulators, and Basic Accessors
//       ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Primary Constructors:
//   A 'bdem_Table' is created with an allocation mode and an allocator,
//   neither of which can be changed throughout the lifetime of an instance.
//   The number and types of columns can be specified at construction, but
//   can also be modified vi the 'reset' method, (taking an array of element
//   types).  We will (somewhat arbitrarily) choose to include 'reset' as
//   a primary manipulator; hence the constructor taking both an allocation
//   mode and an allocator is therefore sufficient for creating instances that
//   can attain any achievable black-box (add probably also white-box) state.
//   Note, however, that the internal workings of allocation are presumed to
//   have already been tested in the underlying 'bdem_tableimp' component.  The
//   default constructor, therefore will be sufficient for our purposes here.
//
//    o bdem_Table(bslma_Allocator *basicAllocator = 0);
//
// Primary Manipulators:
//   We would normally need to bring this object to any achievable white-box
//   state, but since the details of aliasing, exception neutrality, and
//   such have already been tested thoroughly (in 'bdem_tableimp'), it will
//   be sufficient (but not necessary) to achieve all black-box states.
//   Hence, the ability to configure column types, add unset rows, and
//   assign arbitrary values to row elements is sufficient:
//
//    o void reset(const bdem_ElemType::Type elementTypes[], int ne);
//    o void appendNullRow();
//    o bdem_Row& operator[](int rowIndex);
//
// Basic Accessors:
//   We would like to find the largest set of *direct* accessors that can be
//   used generally to report back on the state of the object.  Methods
//   returning the number of rows ('numRows') and columns ('numColumns') and
//   a reference to the indicated non-modifiable row ('operator[]') are obvious
//   first choices.  In addition, the method that returns the type of the
//   indicated column ('elemType') is also useful.
//
//    o const bdem_Row& operator[](int rowIndex) const;
//    o bdem_ElemType::Type columnType(int index) const;
//    o void columnTypes(bsl::vector<bdem_ElemType::Type> *result) const;
//    o int numColumns() const;
//    o int numRows() const;
//
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// STATIC METHODS
//*[10] static int maxSupportedBdexVersion();
//
// CREATORS
// [ 3] bdem_Table(bslma_Allocator *basicAllocator = 0);
// [11] bdem_Table(bdem_AggregateOption::AllocationStrategy am, ba = 0);
// [11] bdem_Table(const ET:T et[], int ne, *ba = 0);
// [11] bdem_Table(const ET:T et[], int ne, am, *ba = 0);
// [11] bdem_Table(const bsl::vector<ET:T>& et, *ba = 0);
// [11] bdem_Table(const bsl::vector<ET:T>& et, am, *ba = 0);
// [ 7] bdem_Table(const bdem_Table& original, ba = 0);
// [11] bdem_Table(const bdem_Table& original, am, ba = 0);
// [11] ~bdem_Table();
//
// MANIPULATORS
// [ 9] bdem_Table& operator=(const bdem_Table& rhs);
// [ 4] void appendNullRow();
// [16] void appendNullRows(int rowCount);
// [12] void appendRow(const bdem_List& srcList);
// [12] void appendRow(const bdem_Table& srcTable, int srcIndex);
// [12] void appendRows(const bdem_Table& srcTable);
// [12] void appendRows(const bdem_Table& st, int srcIndex, int rowCount);
// [16] void insertNullRow(int dstIndex);
// [16] void insertNullRows(int dstIndex, int rowCount);
// [12] void insertRow(int dstIndex, const bdem_List& srcList);
// [12] void insertRow(int dstIndex, const bdem_Table& st, int srcIndex);
// [12] void insertRows(int dstIndex, const bdem_Table& srcTable);
// [12] void insertRows(int di, const bdem_Table& st, int si, int rc);
// [15] void makeColumNull(int index);
// [15] void makeRowsNull(int startIndex, int rowCount);
// [15] void makeAllNull();
// [ 4] void reset(const bdem_ElemType::Type elementTypes[], int ne);
// [ 4] void reset(const vector<bdem_ElemType::Type>& elementTypes);
// [12] void removeRow(int index);
// [12] void removeRows(int startIndex, int rowCount);
// [17] void removeAllRows();
// [17] void removeAll();
// [14] void setColumnValue(int colIdx, const bdem_ConstElemRef& value);
// [14] void setColumnBool(int colIdx, bool value);
// [14] void setColumnChar(int colIdx, char value);
// [14] void setColumnShort(int colIdx, short value);
// [14] void setColumnInt(int colIdx, int value);
// [14] void setColumnInt64(int colIdx, bsls_Types::Int64 value);
// [14] void setColumnFloat(int colIdx, float value);
// [14] void setColumnDouble(int colIdx, double value);
// [14] void setColumnString(int colIdx, const char *value);
// [14] void setColumnString(int colIdx, const bsl::string& value);
// [14] void setColumnDatetime(int colIdx, const bdet_Datetime& value);
// [14] void setColumnDatetimeTz(int colIdx, const bdet_DatetimeTz& value);
// [14] void setColumnDate(int colIdx, const bdet_Date& value);
// [14] void setColumnDateTz(int colIdx, const bdet_DateTz& value);
// [14] void setColumnTime(int colIdx, const bdet_Time& value);
// [14] void setColumnTimeTz(int colIdx, const bdet_TimeTz& value);
// [14] void setColumnBoolArray(int colIdx, const bsl::vector<bool>& val);
// [14] void setColumnCharArray(int colIdx, const bsl::vector<char>& val);
// [14] void setColumnShortArray(int colIdx, const bsl::vector<short>& v);
// [14] void setColumnIntArray(int colIdx, const bsl::vector<int>& value);
// [14] void setColumnInt64Array(int ci, const bsl::vector<Int64>& value);
// [14] void setColumnFloatArray(int ci, const bsl::vector<float>& v);
// [14] void setColumnDoubleArray(int ci, const bsl::vector<double>& val);
// [14] void setColumnStringArray(int, const bsl::vector<bsl::string>& v);
// [14] void setColumnDatetimeArray(int ci, const vector<bdet_Datetime>&);
// [14] void setColumnDatetimeTzArray(int ci, const vector<bdet_DatetimeTz>&);
// [14] void setColumnDateArray(int ci, const bsl::vector<bdet_Date>& v);
// [14] void setColumnDateTzArray(int ci, const bsl::vector<bdet_DateTz>& v);
// [14] void setColumnTimeArray(int ci, const bsl::vector<bdet_Time>& v);
// [14] void setColumnTimeTzArray(int ci, const bsl::vector<bdet_TimeTz>& v);
// [14] void setColumnChoice(int colIdx, const bdem_Choice& value);
// [14] void setColumnList(int colIdx, const bdem_List& value);
// [14] void setColumnList(int colIdx, const bdem_Row& value);
// [14] void setColumnTable(int colIdx, const bdem_Table& value);
// [18] void swapRows(int rowIndex1, int rowIndex2);
// [ 4] bdem_Row& theModifiableRow(int rowIndex);
//*[10] STREAM& bdexStreamIn(STREAM& stream, int version);
//
// ACCESSORS
// [ 4] const bdem_Row& operator[](int rowIndex) const;
// [ 4] bdem_ElemType::Type columnType(int index) const;
// [ 4] void columnTypes(bsl::vector<bdem_ElemType::Type> *result) const;
// [15] bool isAnyInColumnNonNull(int columnIndex) const;
// [15] bool isAnyInColumnNull(int columnIndex) const;
// [15] bool isAnyNonNull() const;
// [15] bool isAnyNull() const;
// [ 4] int numColumns() const;
// [ 4] int numRows() const;
// [ 4] const bdem_Row& theRow(int rowIndex) const;
//*[10] STREAM& bdexStreamOut(STREAM& stream, int version) const;
// [ 5] bsl::ostream& print(bsl::ostream& s, int l, int spl) const;
//
// FREE OPERATORS
// [ 6] operator==(const bdem_Table& lhs, const bdem_Table& rhs);
// [ 6] operator!=(const bdem_Table& lhs, const bdem_Table& rhs);
// [ 5] operator<<(bsl::ostream& stream, bdem_Table rhs);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] TEST APPARATUS
// [ 2] CONCERN: That operator== and operator!= work properly on tables.
// [ 5] operator<<(ostream&, const bsl::vector<T>&);
// [ 4] void loadReferenceA(bdem_List *result);
// [ 4] void loadReferenceB(bdem_List *result);
// [ 4] void loadReferenceC(bdem_List *result);
// [ 4] void loadReferenceD(bdem_List *result);
// [ 4] void loadReferenceE(bdem_List *result);
// [ 4] void loadReferenceU(bdem_List *result);
// [ 4] void loadReferenceN(bdem_List *result);
// [ 2] bdem_List gList(const char *spec, const bdem_Row& referenceRow) //!used
// [ 8] void gg(bdem_Table *t, const char *s, const bdem_Row& r)
// [ 8] bdem_Table g(const char *spec);
// [ 8] bdem_Table g(const char *spec, row1);
// [ 8] bdem_Table g(const char *spec, row1, row2);
// [ 8] bdem_Table g(const char *spec, row1, row2, row3);               //!used
// [14] bool isSameColumnValue(table, columnIndex, value)
// [14] bool isSameExceptColumn(table, columnIndex, values)
// [19] USAGE EXAMPLE
//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//--------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
        << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) { bsl::cout << #X " = "; \
               bdeu_PrintMethods::print(bsl::cout, X, -1, -1); \
               bsl::cout << bsl::endl; }
#define P_(X) { bsl::cout << #X " = "; \
                bdeu_PrintMethods::print(bsl::cout, X, -1, -1); \
                bsl::cout << ", " << bsl::flush; }

#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define L_ __LINE__                           // current Line number
#define T_ cout << "\t" << flush;             // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdem_Table    Obj;
typedef bdem_TableImp ObjImp;
typedef bdem_ElemType ET;
typedef bdem_AggregateOption::AllocationStrategy Strategy;

static const bdem_AggregateOption::AllocationStrategy BDEM_PASS_THROUGH =
             bdem_AggregateOption::BDEM_PASS_THROUGH;

static const bdem_AggregateOption::AllocationStrategy BDEM_WRITE_MANY =
             bdem_AggregateOption::BDEM_WRITE_MANY;

static const bdem_AggregateOption::AllocationStrategy BDEM_WRITE_ONCE =
             bdem_AggregateOption::BDEM_WRITE_ONCE;

        // Create Three Distinct Exemplars For Each Element Type
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const char             A00 = 'A';
const char             B00 = 'B';
const char             C00 = 'C';
const char             D00 = 'D';
const char             E00 = 'E';
const char             U00 = bdetu_Unset<char>::unsetValue();

const short            A01 = -1;
const short            B01 = -2;
const short            C01 = -3;
const short            D01 = -4;
const short            E01 = -5;
const short            U01 = bdetu_Unset<short>::unsetValue();

const int              A02 = 10;
const int              B02 = 20;
const int              C02 = 30;
const int              D02 = 40;
const int              E02 = 50;
const int              U02 = bdetu_Unset<int>::unsetValue();

const bsls_Types::Int64
                       A03 = -100;
const bsls_Types::Int64
                       B03 = -200;
const bsls_Types::Int64
                       C03 = -300;
const bsls_Types::Int64
                       D03 = -400;
const bsls_Types::Int64
                       E03 = -200;
const bsls_Types::Int64
                       U03 = bdetu_Unset<bsls_Types::Int64>::unsetValue();

const float            A04 = -1.5;
const float            B04 = -2.5;
const float            C04 = -3.5;
const float            D04 = -4.5;
const float            E04 = -5.5;
const float            U04 = bdetu_Unset<float>::unsetValue();

const double           A05 = 10.5;
const double           B05 = 20.5;
const double           C05 = 30.5;
const double           D05 = 40.5;
const double           E05 = 50.5;
const double           U05 = bdetu_Unset<double>::unsetValue();

const bsl::string      A06 = "one";
const bsl::string      B06 = "two";
const bsl::string      C06 = "three";
const bsl::string      D06 = "four";
const bsl::string      E06 = "five";
const bsl::string      U06 = bdetu_Unset<bsl::string>::unsetValue();

// Note: bdet_Datetime X07 implemented in terms of X08 and X09.

const bdet_Date        A08(2000,  1,  1);
const bdet_Date        B08(9999, 12, 31);
const bdet_Date        C08(1961,  7,  9);
const bdet_Date        D08(1959,  3,  8);
const bdet_Date        E08(1993,  8, 14);
const bdet_Date        U08 = bdetu_Unset<bdet_Date>::unsetValue();

const bdet_Time        A09( 0,  1,  2,   3);
const bdet_Time        B09( 4,  5,  6, 789);
const bdet_Time        C09(12,  3,  4, 567);
const bdet_Time        D09(13, 24, 57, 986);
const bdet_Time        E09(23, 59, 59, 999);
const bdet_Time        U09 = bdetu_Unset<bdet_Time>::unsetValue();

const bdet_Datetime    A07(A08, A09);
const bdet_Datetime    B07(B08, B09);
const bdet_Datetime    C07(C08, C09);
const bdet_Datetime    D07(D08, D09);
const bdet_Datetime    E07(E08, E09);
const bdet_Datetime    U07 = bdetu_Unset<bdet_Datetime>::unsetValue();

static bsl::vector<char>                        fA10() {
       bsl::vector<char> t;          t.push_back(A00); return t; }
static bsl::vector<char>                        fB10() {
       bsl::vector<char> t;          t.push_back(B00); return t; }
static bsl::vector<char>                        fC10() {
       bsl::vector<char> t;          t.push_back(C00); return t; }
static bsl::vector<char>                        fD10() {
       bsl::vector<char> t;          t.push_back(D00); return t; }
static bsl::vector<char>                        fE10() {
       bsl::vector<char> t;          t.push_back(E00); return t; }
const  bsl::vector<char>                         A10 = fA10();
const  bsl::vector<char>                         B10 = fB10();
const  bsl::vector<char>                         C10 = fC10();
const  bsl::vector<char>                         D10 = fD10();
const  bsl::vector<char>                         E10 = fE10();
const  bsl::vector<char>                         U10;

static bsl::vector<short>                       fA11() {
       bsl::vector<short> t;         t.push_back(A01); return t; }
static bsl::vector<short>                       fB11() {
       bsl::vector<short> t;         t.push_back(B01); return t; }
static bsl::vector<short>                       fC11() {
       bsl::vector<short> t;         t.push_back(C01); return t; }
static bsl::vector<short>                       fD11() {
       bsl::vector<short> t;         t.push_back(D01); return t; }
static bsl::vector<short>                       fE11() {
       bsl::vector<short> t;         t.push_back(E01); return t; }
const  bsl::vector<short>                        A11 = fA11();
const  bsl::vector<short>                        B11 = fB11();
const  bsl::vector<short>                        C11 = fC11();
const  bsl::vector<short>                        D11 = fD11();
const  bsl::vector<short>                        E11 = fE11();
const  bsl::vector<short>                        U11;

static bsl::vector<int>                         fA12() {
       bsl::vector<int> t;           t.push_back(A02); return t; }
static bsl::vector<int>                         fB12() {
       bsl::vector<int> t;           t.push_back(B02); return t; }
static bsl::vector<int>                         fC12() {
       bsl::vector<int> t;           t.push_back(C02); return t; }
static bsl::vector<int>                         fD12() {
       bsl::vector<int> t;           t.push_back(D02); return t; }
static bsl::vector<int>                         fE12() {
       bsl::vector<int> t;           t.push_back(E02); return t; }
const  bsl::vector<int>                          A12 = fA12();
const  bsl::vector<int>                          B12 = fB12();
const  bsl::vector<int>                          C12 = fC12();
const  bsl::vector<int>                          D12 = fD12();
const  bsl::vector<int>                          E12 = fE12();
const  bsl::vector<int>                          U12;

static bsl::vector<bsls_Types::Int64>    fA13() {
       bsl::vector<bsls_Types::Int64> t;
                                     t.push_back(A03); return t; }
static bsl::vector<bsls_Types::Int64>    fB13() {
       bsl::vector<bsls_Types::Int64> t;
                                     t.push_back(B03); return t; }
static bsl::vector<bsls_Types::Int64>    fC13() {
       bsl::vector<bsls_Types::Int64> t;
                                     t.push_back(C03); return t; }
static bsl::vector<bsls_Types::Int64>    fD13() {
       bsl::vector<bsls_Types::Int64> t;
                                     t.push_back(D03); return t; }
static bsl::vector<bsls_Types::Int64>    fE13() {
       bsl::vector<bsls_Types::Int64> t;
                                     t.push_back(E03); return t; }
const  bsl::vector<bsls_Types::Int64>     A13 = fA13();
const  bsl::vector<bsls_Types::Int64>     B13 = fB13();
const  bsl::vector<bsls_Types::Int64>     C13 = fC13();
const  bsl::vector<bsls_Types::Int64>     D13 = fD13();
const  bsl::vector<bsls_Types::Int64>     E13 = fE13();
const  bsl::vector<bsls_Types::Int64>     U13;

static bsl::vector<float>                       fA14() {
       bsl::vector<float> t;         t.push_back(A04); return t; }
static bsl::vector<float>                       fB14() {
       bsl::vector<float> t;         t.push_back(B04); return t; }
static bsl::vector<float>                       fC14() {
       bsl::vector<float> t;         t.push_back(C04); return t; }
static bsl::vector<float>                       fD14() {
       bsl::vector<float> t;         t.push_back(D04); return t; }
static bsl::vector<float>                       fE14() {
       bsl::vector<float> t;         t.push_back(E04); return t; }
const  bsl::vector<float>                        A14 = fA14();
const  bsl::vector<float>                        B14 = fB14();
const  bsl::vector<float>                        C14 = fC14();
const  bsl::vector<float>                        D14 = fD14();
const  bsl::vector<float>                        E14 = fE14();
const  bsl::vector<float>                        U14;

static bsl::vector<double>                      fA15() {
       bsl::vector<double> t;        t.push_back(A05); return t; }
static bsl::vector<double>                      fB15() {
       bsl::vector<double> t;        t.push_back(B05); return t; }
static bsl::vector<double>                      fC15() {
       bsl::vector<double> t;        t.push_back(C05); return t; }
static bsl::vector<double>                      fD15() {
       bsl::vector<double> t;        t.push_back(D05); return t; }
static bsl::vector<double>                      fE15() {
       bsl::vector<double> t;        t.push_back(E05); return t; }
const  bsl::vector<double>                       A15 = fA15();
const  bsl::vector<double>                       B15 = fB15();
const  bsl::vector<double>                       C15 = fC15();
const  bsl::vector<double>                       D15 = fD15();
const  bsl::vector<double>                       E15 = fE15();
const  bsl::vector<double>                       U15;

static bsl::vector<bsl::string>                 fA16() {
       bsl::vector<bsl::string> t;   t.push_back(A06); return t; }
static bsl::vector<bsl::string>                 fB16() {
       bsl::vector<bsl::string> t;   t.push_back(B06); return t; }
static bsl::vector<bsl::string>                 fC16() {
       bsl::vector<bsl::string> t;   t.push_back(C06); return t; }
static bsl::vector<bsl::string>                 fD16() {
       bsl::vector<bsl::string> t;   t.push_back(D06); return t; }
static bsl::vector<bsl::string>                 fE16() {
       bsl::vector<bsl::string> t;   t.push_back(E06); return t; }
const  bsl::vector<bsl::string>                  A16 = fA16();
const  bsl::vector<bsl::string>                  B16 = fB16();
const  bsl::vector<bsl::string>                  C16 = fC16();
const  bsl::vector<bsl::string>                  D16 = fD16();
const  bsl::vector<bsl::string>                  E16 = fE16();
const  bsl::vector<bsl::string>                  U16;

static bsl::vector<bdet_Datetime>               fA17() {
       bsl::vector<bdet_Datetime> t; t.push_back(A07); return t; }
static bsl::vector<bdet_Datetime>               fB17() {
       bsl::vector<bdet_Datetime> t; t.push_back(B07); return t; }
static bsl::vector<bdet_Datetime>               fC17() {
       bsl::vector<bdet_Datetime> t; t.push_back(C07); return t; }
static bsl::vector<bdet_Datetime>               fD17() {
       bsl::vector<bdet_Datetime> t; t.push_back(D07); return t; }
static bsl::vector<bdet_Datetime>               fE17() {
       bsl::vector<bdet_Datetime> t; t.push_back(E07); return t; }
const  bsl::vector<bdet_Datetime>                A17 = fA17();
const  bsl::vector<bdet_Datetime>                B17 = fB17();
const  bsl::vector<bdet_Datetime>                C17 = fC17();
const  bsl::vector<bdet_Datetime>                D17 = fD17();
const  bsl::vector<bdet_Datetime>                E17 = fE17();
const  bsl::vector<bdet_Datetime>                U17;

static bsl::vector<bdet_Date>                   fA18() {
       bsl::vector<bdet_Date> t;     t.push_back(A08); return t; }
static bsl::vector<bdet_Date>                   fB18() {
       bsl::vector<bdet_Date> t;     t.push_back(B08); return t; }
static bsl::vector<bdet_Date>                   fC18() {
       bsl::vector<bdet_Date> t;     t.push_back(C08); return t; }
static bsl::vector<bdet_Date>                   fD18() {
       bsl::vector<bdet_Date> t;     t.push_back(D08); return t; }
static bsl::vector<bdet_Date>                   fE18() {
       bsl::vector<bdet_Date> t;     t.push_back(E08); return t; }
const  bsl::vector<bdet_Date>                    A18 = fA18();
const  bsl::vector<bdet_Date>                    B18 = fB18();
const  bsl::vector<bdet_Date>                    C18 = fC18();
const  bsl::vector<bdet_Date>                    D18 = fD18();
const  bsl::vector<bdet_Date>                    E18 = fE18();
const  bsl::vector<bdet_Date>                    U18;

static bsl::vector<bdet_Time>                   fA19() {
       bsl::vector<bdet_Time> t;     t.push_back(A09); return t; }
static bsl::vector<bdet_Time>                   fB19() {
       bsl::vector<bdet_Time> t;     t.push_back(B09); return t; }
static bsl::vector<bdet_Time>                   fC19() {
       bsl::vector<bdet_Time> t;     t.push_back(B09); return t; }
static bsl::vector<bdet_Time>                   fD19() {
       bsl::vector<bdet_Time> t;     t.push_back(B09); return t; }
static bsl::vector<bdet_Time>                   fE19() {
       bsl::vector<bdet_Time> t;     t.push_back(B09); return t; }
const  bsl::vector<bdet_Time>                    A19 = fA19();
const  bsl::vector<bdet_Time>                    B19 = fB19();
const  bsl::vector<bdet_Time>                    C19 = fC19();
const  bsl::vector<bdet_Time>                    D19 = fD19();
const  bsl::vector<bdet_Time>                    E19 = fE19();
const  bsl::vector<bdet_Time>                    U19;

static bdem_List                            fA20() {
       bdem_List t;              t.appendInt(A02); return t; }
static bdem_List                            fB20() {
       bdem_List t;              t.appendInt(B02); return t; }
static bdem_List                            fC20() {
       bdem_List t;              t.appendInt(C02); return t; }
static bdem_List                            fD20() {
       bdem_List t;              t.appendInt(D02); return t; }
static bdem_List                            fE20() {
       bdem_List t;              t.appendInt(E02); return t; }
// Note: on Windows, the function pointers in bdem_Properties::d_intAttr
// are 0x00000000 before entering main().  Consequently, the calls in
// fA20() and fB20() to t.appendInt() cause a runtime exception.
#ifndef BSLS_PLATFORM__CMP_MSVC
const  bdem_List                      A20 = fA20();
const  bdem_List                      B20 = fB20();
const  bdem_List                      C20 = fC20();
const  bdem_List                      D20 = fD20();
const  bdem_List                      E20 = fE20();
#endif
const  bdem_List                             U20;

const  bdem_ElemType::Type ET21A = bdem_ElemType::BDEM_CHAR;
const  bdem_ElemType::Type ET21B = bdem_ElemType::BDEM_INT;
const  bdem_ElemType::Type ET21C = bdem_ElemType::BDEM_STRING;
const  bdem_ElemType::Type ET21D = bdem_ElemType::BDEM_INT_ARRAY;
const  bdem_ElemType::Type ET21E = bdem_ElemType::BDEM_STRING_ARRAY;
const  bdem_Table                            A21(&ET21A, 1);
const  bdem_Table                            B21(&ET21B, 1);
const  bdem_Table                            C21(&ET21C, 1);
const  bdem_Table                            D21(&ET21D, 1);
const  bdem_Table                            E21(&ET21E, 1);
const  bdem_Table                            U21;

const bool             A22 = true;
const bool             B22 = false;
const bool             C22 = true;
const bool             D22 = true;
const bool             E22 = true;
const bool             U22 = bdetu_Unset<bool>::unsetValue();

const bdet_DatetimeTz  A23(A07, -5);
const bdet_DatetimeTz  B23(B07, -3);
const bdet_DatetimeTz  C23(C07,  0);
const bdet_DatetimeTz  D23(D07, +3);
const bdet_DatetimeTz  E23(E07, +5);
const bdet_DatetimeTz  U23 = bdetu_Unset<bdet_DatetimeTz>::unsetValue();

const bdet_DateTz      A24(A08, -5);
const bdet_DateTz      B24(B08, -3);
const bdet_DateTz      C24(C08,  0);
const bdet_DateTz      D24(D08, +3);
const bdet_DateTz      E24(E08, +5);
const bdet_DateTz      U24 = bdetu_Unset<bdet_DateTz>::unsetValue();

const bdet_TimeTz      A25(A09, -5);
const bdet_TimeTz      B25(B09, -3);
const bdet_TimeTz      C25(C09,  0);
const bdet_TimeTz      D25(D09, +3);
const bdet_TimeTz      E25(E09, +5);
const bdet_TimeTz      U25 = bdetu_Unset<bdet_TimeTz>::unsetValue();

static bsl::vector<bool>                        fA26() {
       bsl::vector<bool> t;          t.push_back(A22); return t; }
static bsl::vector<bool>                        fB26() {
       bsl::vector<bool> t;          t.push_back(B22); return t; }
static bsl::vector<bool>                        fC26() {
       bsl::vector<bool> t;          t.push_back(C22); return t; }
static bsl::vector<bool>                        fD26() {
       bsl::vector<bool> t;          t.push_back(D22); return t; }
static bsl::vector<bool>                        fE26() {
       bsl::vector<bool> t;          t.push_back(E22); return t; }
const  bsl::vector<bool>                         A26 = fA26();
const  bsl::vector<bool>                         B26 = fB26();
const  bsl::vector<bool>                         C26 = fC26();
const  bsl::vector<bool>                         D26 = fD26();
const  bsl::vector<bool>                         E26 = fE26();
const  bsl::vector<bool>                         U26;

static bsl::vector<bdet_DatetimeTz>               fA27() {
       bsl::vector<bdet_DatetimeTz> t; t.push_back(A23); return t; }
static bsl::vector<bdet_DatetimeTz>               fB27() {
       bsl::vector<bdet_DatetimeTz> t; t.push_back(B23); return t; }
static bsl::vector<bdet_DatetimeTz>               fC27() {
       bsl::vector<bdet_DatetimeTz> t; t.push_back(C23); return t; }
static bsl::vector<bdet_DatetimeTz>               fD27() {
       bsl::vector<bdet_DatetimeTz> t; t.push_back(D23); return t; }
static bsl::vector<bdet_DatetimeTz>               fE27() {
       bsl::vector<bdet_DatetimeTz> t; t.push_back(E23); return t; }
const  bsl::vector<bdet_DatetimeTz>                A27 = fA27();
const  bsl::vector<bdet_DatetimeTz>                B27 = fB27();
const  bsl::vector<bdet_DatetimeTz>                C27 = fC27();
const  bsl::vector<bdet_DatetimeTz>                D27 = fD27();
const  bsl::vector<bdet_DatetimeTz>                E27 = fE27();
const  bsl::vector<bdet_DatetimeTz>                U27;

static bsl::vector<bdet_DateTz>                   fA28() {
       bsl::vector<bdet_DateTz> t;     t.push_back(A24); return t; }
static bsl::vector<bdet_DateTz>                   fB28() {
       bsl::vector<bdet_DateTz> t;     t.push_back(B24); return t; }
static bsl::vector<bdet_DateTz>                   fC28() {
       bsl::vector<bdet_DateTz> t;     t.push_back(C24); return t; }
static bsl::vector<bdet_DateTz>                   fD28() {
       bsl::vector<bdet_DateTz> t;     t.push_back(D24); return t; }
static bsl::vector<bdet_DateTz>                   fE28() {
       bsl::vector<bdet_DateTz> t;     t.push_back(E24); return t; }
const  bsl::vector<bdet_DateTz>                    A28 = fA28();
const  bsl::vector<bdet_DateTz>                    B28 = fB28();
const  bsl::vector<bdet_DateTz>                    C28 = fC28();
const  bsl::vector<bdet_DateTz>                    D28 = fD28();
const  bsl::vector<bdet_DateTz>                    E28 = fE28();
const  bsl::vector<bdet_DateTz>                    U28;

static bsl::vector<bdet_TimeTz>                   fA29() {
       bsl::vector<bdet_TimeTz> t;     t.push_back(A25); return t; }
static bsl::vector<bdet_TimeTz>                   fB29() {
       bsl::vector<bdet_TimeTz> t;     t.push_back(B25); return t; }
static bsl::vector<bdet_TimeTz>                   fC29() {
       bsl::vector<bdet_TimeTz> t;     t.push_back(B25); return t; }
static bsl::vector<bdet_TimeTz>                   fD29() {
       bsl::vector<bdet_TimeTz> t;     t.push_back(B25); return t; }
static bsl::vector<bdet_TimeTz>                   fE29() {
       bsl::vector<bdet_TimeTz> t;     t.push_back(B25); return t; }
const  bsl::vector<bdet_TimeTz>                    A29 = fA29();
const  bsl::vector<bdet_TimeTz>                    B29 = fB29();
const  bsl::vector<bdet_TimeTz>                    C29 = fC29();
const  bsl::vector<bdet_TimeTz>                    D29 = fD29();
const  bsl::vector<bdet_TimeTz>                    E29 = fE29();
const  bsl::vector<bdet_TimeTz>                    U29;

static bdem_Choice                                 fA30() {
       const bdem_ElemType::Type types[] =
       {
           bdem_ElemType::BDEM_INT,
           bdem_ElemType::BDEM_DOUBLE,
           bdem_ElemType::BDEM_STRING,
           bdem_ElemType::BDEM_STRING_ARRAY
       };
       const int NUM_TYPES = sizeof(types) / sizeof(*types);
       bdem_Choice t(types, NUM_TYPES);
                                  t.makeSelection(0).theModifiableInt() = A02;
                                  return t;
}

static bdem_Choice                              fB30() {
       const bdem_ElemType::Type types[] =
       {
           bdem_ElemType::BDEM_INT,
           bdem_ElemType::BDEM_DOUBLE,
           bdem_ElemType::BDEM_STRING,
           bdem_ElemType::BDEM_STRING_ARRAY
       };
       const int NUM_TYPES = sizeof(types) / sizeof(*types);
       bdem_Choice t(types, NUM_TYPES);
                              t.makeSelection(1).theModifiableDouble() = A05;
                              return t;
}

static bdem_Choice                              fC30() {
       const bdem_ElemType::Type types[] =
       {
           bdem_ElemType::BDEM_INT,
           bdem_ElemType::BDEM_DOUBLE,
           bdem_ElemType::BDEM_STRING,
           bdem_ElemType::BDEM_STRING_ARRAY
       };
       const int NUM_TYPES = sizeof(types) / sizeof(*types);
       bdem_Choice t(types, NUM_TYPES);
                              t.makeSelection(2).theModifiableString() = A06;
                              return t;
}

static bdem_Choice                              fD30() {
       const bdem_ElemType::Type types[] =
       {
           bdem_ElemType::BDEM_INT,
           bdem_ElemType::BDEM_DOUBLE,
           bdem_ElemType::BDEM_STRING,
           bdem_ElemType::BDEM_STRING_ARRAY
       };
       const int NUM_TYPES = sizeof(types) / sizeof(*types);
       bdem_Choice t(types, NUM_TYPES);
                          t.makeSelection(3).theModifiableStringArray() = A16;
                          return t;
}

static bdem_Choice                              fE30() {
       const bdem_ElemType::Type types[] =
       {
           bdem_ElemType::BDEM_INT,
           bdem_ElemType::BDEM_DOUBLE,
           bdem_ElemType::BDEM_STRING,
           bdem_ElemType::BDEM_STRING_ARRAY
       };
       const int NUM_TYPES = sizeof(types) / sizeof(*types);
       bdem_Choice t(types, NUM_TYPES);
       return t;
}

// Note: on Windows, the function pointers in bdem_Properties::d_intAttr
// are 0x00000000 before entering main().  Consequently, the calls in
// fAxx() and fBxx() cause a runtime exception.  Our workaround is to
// initialize these constants at the start of main.
#ifndef BSLS_PLATFORM__CMP_MSVC
const  bdem_Choice                               A30 = fA30();
const  bdem_Choice                               B30 = fB30();
const  bdem_Choice                               C30 = fC30();
const  bdem_Choice                               D30 = fD30();
const  bdem_Choice                               E30 = fE30();
#endif
const  bdem_Choice                               U30;

static bdem_ChoiceArray                         fA31() {
       const bdem_ElemType::Type types[] =
       {
           bdem_ElemType::BDEM_INT,
           bdem_ElemType::BDEM_DOUBLE,
           bdem_ElemType::BDEM_STRING,
           bdem_ElemType::BDEM_STRING_ARRAY
       };
       const int NUM_TYPES = sizeof(types) / sizeof(*types);
       bdem_ChoiceArray t(types, NUM_TYPES); t.appendNullItems(4);
      t.theModifiableItem(0).makeSelection(0).theModifiableInt() = A02;
      t.theModifiableItem(1).makeSelection(1).theModifiableDouble() = A05;
      t.theModifiableItem(2).makeSelection(2).theModifiableString() = A06;
      t.theModifiableItem(3).makeSelection(3).theModifiableStringArray() = A16;
                      return t;
}

static bdem_ChoiceArray                         fB31() {
       const bdem_ElemType::Type types[] =
       {
           bdem_ElemType::BDEM_INT,
           bdem_ElemType::BDEM_DOUBLE,
           bdem_ElemType::BDEM_STRING,
           bdem_ElemType::BDEM_STRING_ARRAY
       };
       const int NUM_TYPES = sizeof(types) / sizeof(*types);
       bdem_ChoiceArray t(types, NUM_TYPES); t.appendNullItems(4);
      t.theModifiableItem(0).makeSelection(0).theModifiableInt() = B02;
      t.theModifiableItem(1).makeSelection(1).theModifiableDouble() = B05;
      t.theModifiableItem(2).makeSelection(2).theModifiableString() = B06;
      t.theModifiableItem(3).makeSelection(3).theModifiableStringArray() = B16;
                      return t;
}

static bdem_ChoiceArray                         fC31() {
       const bdem_ElemType::Type types[] =
       {
           bdem_ElemType::BDEM_INT,
           bdem_ElemType::BDEM_DOUBLE,
           bdem_ElemType::BDEM_STRING,
           bdem_ElemType::BDEM_STRING_ARRAY
       };
       const int NUM_TYPES = sizeof(types) / sizeof(*types);
       bdem_ChoiceArray t(types, NUM_TYPES); t.appendNullItems(4);
      t.theModifiableItem(0).makeSelection(0).theModifiableInt() = C02;
      t.theModifiableItem(1).makeSelection(1).theModifiableDouble() = C05;
      t.theModifiableItem(2).makeSelection(2).theModifiableString() = C06;
      t.theModifiableItem(3).makeSelection(3).theModifiableStringArray() = C16;
                      return t;
}

static bdem_ChoiceArray                         fD31() {
       const bdem_ElemType::Type types[] =
       {
           bdem_ElemType::BDEM_INT,
           bdem_ElemType::BDEM_DOUBLE,
           bdem_ElemType::BDEM_STRING,
           bdem_ElemType::BDEM_STRING_ARRAY
       };
       const int NUM_TYPES = sizeof(types) / sizeof(*types);
       bdem_ChoiceArray t(types, NUM_TYPES); t.appendNullItems(4);
      t.theModifiableItem(0).makeSelection(0).theModifiableInt() = D02;
      t.theModifiableItem(1).makeSelection(1).theModifiableDouble() = D05;
      t.theModifiableItem(2).makeSelection(2).theModifiableString() = D06;
      t.theModifiableItem(3).makeSelection(3).theModifiableStringArray() = D16;
                      return t;
}

static bdem_ChoiceArray                         fE31() {
       const bdem_ElemType::Type types[] =
       {
           bdem_ElemType::BDEM_INT,
           bdem_ElemType::BDEM_DOUBLE,
           bdem_ElemType::BDEM_STRING,
           bdem_ElemType::BDEM_STRING_ARRAY
       };
       const int NUM_TYPES = sizeof(types) / sizeof(*types);
       bdem_ChoiceArray t(types, NUM_TYPES); t.appendNullItems(4);
       return t;
}

// Note: on Windows, the function pointers in bdem_Properties::d_intAttr
// are 0x00000000 before entering main().  Consequently, the calls in
// fAxx() and fBxx() cause a runtime exception.  Our workaround is to
// initialize these constants at the start of main.
#ifndef BSLS_PLATFORM__CMP_MSVC
const  bdem_ChoiceArray                          A31 = fA31();
const  bdem_ChoiceArray                          B31 = fB31();
const  bdem_ChoiceArray                          C31 = fC31();
const  bdem_ChoiceArray                          D31 = fD31();
const  bdem_ChoiceArray                          E31 = fE31();
#endif
const  bdem_ChoiceArray                          U31;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

        // =======================================================
        // operator<< for bdem_AggregateOption::AllocationStrategy
        // =======================================================

namespace BloombergLP {

inline
bsl::ostream& operator<<(bsl::ostream& stream,
                         const bdem_AggregateOption::AllocationStrategy mode)
{
    switch(mode) {
      case bdem_AggregateOption::BDEM_PASS_THROUGH: {
                      stream << "BDEM_PASS_THROUGH" << bsl::flush;
      } break;
      case bdem_AggregateOption::BDEM_WRITE_ONCE: {
                      stream << "BDEM_WRITE_ONCE" << bsl::flush;
      } break;
      case bdem_AggregateOption::BDEM_WRITE_MANY: {
                      stream << "BDEM_WRITE_MANY" << bsl::flush;
      } break;
      case bdem_AggregateOption::BDEM_SUBORDINATE: {
                      stream << "BDEM_SUBORDINATE" << bsl::flush;
      } break;
      default: {
        P(mode);
        ASSERT("Unknown bdem_AggregateOption" && 0);
      }
    };
    return stream;
}
} // End namespace BloombergLP

                        // ==============
                        // loadReferenceA
                        // ==============

void loadReferenceA(bdem_List *result)
    // Append to the specified 'result' a sequence of 32 element types
    // corresponding to the 'A' category of element values defined above.
{
    ASSERT(result);
    bdem_List& mL = *result; const bdem_List& L = mL; const int LEN=L.length();

    mL.appendChar(A00);                         ASSERT(LEN +  1 == L.length());
    mL.appendShort(A01);                        ASSERT(LEN +  2 == L.length());
    mL.appendInt(A02);                          ASSERT(LEN +  3 == L.length());
    mL.appendInt64(A03);                        ASSERT(LEN +  4 == L.length());
    mL.appendFloat(A04);                        ASSERT(LEN +  5 == L.length());
    mL.appendDouble(A05);                       ASSERT(LEN +  6 == L.length());
    mL.appendString(A06);                       ASSERT(LEN +  7 == L.length());
    mL.appendDatetime(A07);                     ASSERT(LEN +  8 == L.length());
    mL.appendDate(A08);                         ASSERT(LEN +  9 == L.length());
    mL.appendTime(A09);                         ASSERT(LEN + 10 == L.length());

    mL.appendCharArray(A10);                    ASSERT(LEN + 11 == L.length());
    mL.appendShortArray(A11);                   ASSERT(LEN + 12 == L.length());
    mL.appendIntArray(A12);                     ASSERT(LEN + 13 == L.length());
    mL.appendInt64Array(A13);                   ASSERT(LEN + 14 == L.length());
    mL.appendFloatArray(A14);                   ASSERT(LEN + 15 == L.length());
    mL.appendDoubleArray(A15);                  ASSERT(LEN + 16 == L.length());
    mL.appendStringArray(A16);                  ASSERT(LEN + 17 == L.length());
    mL.appendDatetimeArray(A17);                ASSERT(LEN + 18 == L.length());
    mL.appendDateArray(A18);                    ASSERT(LEN + 19 == L.length());
    mL.appendTimeArray(A19);                    ASSERT(LEN + 20 == L.length());

    #ifndef BSLS_PLATFORM__CMP_MSVC
    mL.appendList(A20);                         ASSERT(LEN + 21 == L.length());
    #else
    mL.appendList(fA20());                      ASSERT(LEN + 21 == L.length());
    #endif
    mL.appendTable(A21);                        ASSERT(LEN + 22 == L.length());

    mL.appendBool(A22);                         ASSERT(LEN + 23 == L.length());
    mL.appendDatetimeTz(A23);                   ASSERT(LEN + 24 == L.length());
    mL.appendDateTz(A24);                       ASSERT(LEN + 25 == L.length());
    mL.appendTimeTz(A25);                       ASSERT(LEN + 26 == L.length());
    mL.appendBoolArray(A26);                    ASSERT(LEN + 27 == L.length());
    mL.appendDatetimeTzArray(A27);              ASSERT(LEN + 28 == L.length());
    mL.appendDateTzArray(A28);                  ASSERT(LEN + 29 == L.length());
    mL.appendTimeTzArray(A29);                  ASSERT(LEN + 30 == L.length());
    #ifndef BSLS_PLATFORM__CMP_MSVC
    mL.appendChoice(A30);                       ASSERT(LEN + 31 == L.length());
    mL.appendChoiceArray(A31);                  ASSERT(LEN + 32 == L.length());
    #else
    mL.appendChoice(fA30());                    ASSERT(LEN + 31 == L.length());
    mL.appendChoiceArray(fA31());               ASSERT(LEN + 32 == L.length());
    #endif
}

                        // ==============
                        // loadReferenceB
                        // ==============

void loadReferenceB(bdem_List *result)
    // Append to the specified 'result' a sequence of 32 element types
    // corresponding to the 'B' category of element values defined above.
{
    ASSERT(result);
    bdem_List& mL = *result; const bdem_List& L = mL; const int LEN=L.length();

    mL.appendChar(B00);                         ASSERT(LEN +  1 == L.length());
    mL.appendShort(B01);                        ASSERT(LEN +  2 == L.length());
    mL.appendInt(B02);                          ASSERT(LEN +  3 == L.length());
    mL.appendInt64(B03);                        ASSERT(LEN +  4 == L.length());
    mL.appendFloat(B04);                        ASSERT(LEN +  5 == L.length());
    mL.appendDouble(B05);                       ASSERT(LEN +  6 == L.length());
    mL.appendString(B06);                       ASSERT(LEN +  7 == L.length());
    mL.appendDatetime(B07);                     ASSERT(LEN +  8 == L.length());
    mL.appendDate(B08);                         ASSERT(LEN +  9 == L.length());
    mL.appendTime(B09);                         ASSERT(LEN + 10 == L.length());

    mL.appendCharArray(B10);                    ASSERT(LEN + 11 == L.length());
    mL.appendShortArray(B11);                   ASSERT(LEN + 12 == L.length());
    mL.appendIntArray(B12);                     ASSERT(LEN + 13 == L.length());
    mL.appendInt64Array(B13);                   ASSERT(LEN + 14 == L.length());
    mL.appendFloatArray(B14);                   ASSERT(LEN + 15 == L.length());
    mL.appendDoubleArray(B15);                  ASSERT(LEN + 16 == L.length());
    mL.appendStringArray(B16);                  ASSERT(LEN + 17 == L.length());
    mL.appendDatetimeArray(B17);                ASSERT(LEN + 18 == L.length());
    mL.appendDateArray(B18);                    ASSERT(LEN + 19 == L.length());
    mL.appendTimeArray(B19);                    ASSERT(LEN + 20 == L.length());

    #ifndef BSLS_PLATFORM__CMP_MSVC
    mL.appendList(B20);                         ASSERT(LEN + 21 == L.length());
    #else
    mL.appendList(fB20());                      ASSERT(LEN + 21 == L.length());
    #endif
    mL.appendTable(B21);                        ASSERT(LEN + 22 == L.length());

    mL.appendBool(B22);                         ASSERT(LEN + 23 == L.length());
    mL.appendDatetimeTz(B23);                   ASSERT(LEN + 24 == L.length());
    mL.appendDateTz(B24);                       ASSERT(LEN + 25 == L.length());
    mL.appendTimeTz(B25);                       ASSERT(LEN + 26 == L.length());
    mL.appendBoolArray(B26);                    ASSERT(LEN + 27 == L.length());
    mL.appendDatetimeTzArray(B27);              ASSERT(LEN + 28 == L.length());
    mL.appendDateTzArray(B28);                  ASSERT(LEN + 29 == L.length());
    mL.appendTimeTzArray(B29);                  ASSERT(LEN + 30 == L.length());
    #ifndef BSLS_PLATFORM__CMP_MSVC
    mL.appendChoice(B30);                       ASSERT(LEN + 31 == L.length());
    mL.appendChoiceArray(B31);                  ASSERT(LEN + 32 == L.length());
    #else
    mL.appendChoice(fB30());                    ASSERT(LEN + 31 == L.length());
    mL.appendChoiceArray(fB31());               ASSERT(LEN + 32 == L.length());
    #endif
}

                        // ==============
                        // loadReferenceC
                        // ==============

void loadReferenceC(bdem_List *result)
    // Append to the specified 'result' a sequence of 32 element types
    // corresponding to the 'C' category of element values defined above.
{
    ASSERT(result);
    bdem_List& mL = *result; const bdem_List& L = mL; const int LEN=L.length();

    mL.appendChar(C00);                         ASSERT(LEN +  1 == L.length());
    mL.appendShort(C01);                        ASSERT(LEN +  2 == L.length());
    mL.appendInt(C02);                          ASSERT(LEN +  3 == L.length());
    mL.appendInt64(C03);                        ASSERT(LEN +  4 == L.length());
    mL.appendFloat(C04);                        ASSERT(LEN +  5 == L.length());
    mL.appendDouble(C05);                       ASSERT(LEN +  6 == L.length());
    mL.appendString(C06);                       ASSERT(LEN +  7 == L.length());
    mL.appendDatetime(C07);                     ASSERT(LEN +  8 == L.length());
    mL.appendDate(C08);                         ASSERT(LEN +  9 == L.length());
    mL.appendTime(C09);                         ASSERT(LEN + 10 == L.length());

    mL.appendCharArray(C10);                    ASSERT(LEN + 11 == L.length());
    mL.appendShortArray(C11);                   ASSERT(LEN + 12 == L.length());
    mL.appendIntArray(C12);                     ASSERT(LEN + 13 == L.length());
    mL.appendInt64Array(C13);                   ASSERT(LEN + 14 == L.length());
    mL.appendFloatArray(C14);                   ASSERT(LEN + 15 == L.length());
    mL.appendDoubleArray(C15);                  ASSERT(LEN + 16 == L.length());
    mL.appendStringArray(C16);                  ASSERT(LEN + 17 == L.length());
    mL.appendDatetimeArray(C17);                ASSERT(LEN + 18 == L.length());
    mL.appendDateArray(C18);                    ASSERT(LEN + 19 == L.length());
    mL.appendTimeArray(C19);                    ASSERT(LEN + 20 == L.length());

    #ifndef BSLS_PLATFORM__CMP_MSVC
    mL.appendList(C20);                         ASSERT(LEN + 21 == L.length());
    #else
    mL.appendList(fC20());                      ASSERT(LEN + 21 == L.length());
    #endif
    mL.appendTable(C21);                        ASSERT(LEN + 22 == L.length());

    mL.appendBool(C22);                         ASSERT(LEN + 23 == L.length());
    mL.appendDatetimeTz(C23);                   ASSERT(LEN + 24 == L.length());
    mL.appendDateTz(C24);                       ASSERT(LEN + 25 == L.length());
    mL.appendTimeTz(C25);                       ASSERT(LEN + 26 == L.length());
    mL.appendBoolArray(C26);                    ASSERT(LEN + 27 == L.length());
    mL.appendDatetimeTzArray(C27);              ASSERT(LEN + 28 == L.length());
    mL.appendDateTzArray(C28);                  ASSERT(LEN + 29 == L.length());
    mL.appendTimeTzArray(C29);                  ASSERT(LEN + 30 == L.length());
    #ifndef BSLS_PLATFORM__CMP_MSVC
    mL.appendChoice(C30);                       ASSERT(LEN + 31 == L.length());
    mL.appendChoiceArray(C31);                  ASSERT(LEN + 32 == L.length());
    #else
    mL.appendChoice(fC30());                    ASSERT(LEN + 31 == L.length());
    mL.appendChoiceArray(fC31());               ASSERT(LEN + 32 == L.length());
    #endif
}

                        // ==============
                        // loadReferenceD
                        // ==============

void loadReferenceD(bdem_List *result)
    // Append to the specified 'result' a sequence of 32 element types
    // corresponding to the 'D' category of element values defined above.
{
    ASSERT(result);
    bdem_List& mL = *result; const bdem_List& L = mL; const int LEN=L.length();

    mL.appendChar(D00);                         ASSERT(LEN +  1 == L.length());
    mL.appendShort(D01);                        ASSERT(LEN +  2 == L.length());
    mL.appendInt(D02);                          ASSERT(LEN +  3 == L.length());
    mL.appendInt64(D03);                        ASSERT(LEN +  4 == L.length());
    mL.appendFloat(D04);                        ASSERT(LEN +  5 == L.length());
    mL.appendDouble(D05);                       ASSERT(LEN +  6 == L.length());
    mL.appendString(D06);                       ASSERT(LEN +  7 == L.length());
    mL.appendDatetime(D07);                     ASSERT(LEN +  8 == L.length());
    mL.appendDate(D08);                         ASSERT(LEN +  9 == L.length());
    mL.appendTime(D09);                         ASSERT(LEN + 10 == L.length());

    mL.appendCharArray(D10);                    ASSERT(LEN + 11 == L.length());
    mL.appendShortArray(D11);                   ASSERT(LEN + 12 == L.length());
    mL.appendIntArray(D12);                     ASSERT(LEN + 13 == L.length());
    mL.appendInt64Array(D13);                   ASSERT(LEN + 14 == L.length());
    mL.appendFloatArray(D14);                   ASSERT(LEN + 15 == L.length());
    mL.appendDoubleArray(D15);                  ASSERT(LEN + 16 == L.length());
    mL.appendStringArray(D16);                  ASSERT(LEN + 17 == L.length());
    mL.appendDatetimeArray(D17);                ASSERT(LEN + 18 == L.length());
    mL.appendDateArray(D18);                    ASSERT(LEN + 19 == L.length());
    mL.appendTimeArray(D19);                    ASSERT(LEN + 20 == L.length());

    #ifndef BSLS_PLATFORM__CMP_MSVC
    mL.appendList(D20);                         ASSERT(LEN + 21 == L.length());
    #else
    mL.appendList(fD20());                      ASSERT(LEN + 21 == L.length());
    #endif
    mL.appendTable(D21);                        ASSERT(LEN + 22 == L.length());

    mL.appendBool(D22);                         ASSERT(LEN + 23 == L.length());
    mL.appendDatetimeTz(D23);                   ASSERT(LEN + 24 == L.length());
    mL.appendDateTz(D24);                       ASSERT(LEN + 25 == L.length());
    mL.appendTimeTz(D25);                       ASSERT(LEN + 26 == L.length());
    mL.appendBoolArray(D26);                    ASSERT(LEN + 27 == L.length());
    mL.appendDatetimeTzArray(D27);              ASSERT(LEN + 28 == L.length());
    mL.appendDateTzArray(D28);                  ASSERT(LEN + 29 == L.length());
    mL.appendTimeTzArray(D29);                  ASSERT(LEN + 30 == L.length());
    #ifndef BSLS_PLATFORM__CMP_MSVC
    mL.appendChoice(D30);                       ASSERT(LEN + 31 == L.length());
    mL.appendChoiceArray(D31);                  ASSERT(LEN + 32 == L.length());
    #else
    mL.appendChoice(fD30());                    ASSERT(LEN + 31 == L.length());
    mL.appendChoiceArray(fD31());               ASSERT(LEN + 32 == L.length());
    #endif
}

                        // ==============
                        // loadReferenceE
                        // ==============

void loadReferenceE(bdem_List *result)
    // Append to the specified 'result' a sequence of 32 element types
    // corresponding to the 'E' category of element values defined above.
{
    ASSERT(result);
    bdem_List& mL = *result; const bdem_List& L = mL; const int LEN=L.length();

    mL.appendChar(E00);                         ASSERT(LEN +  1 == L.length());
    mL.appendShort(E01);                        ASSERT(LEN +  2 == L.length());
    mL.appendInt(E02);                          ASSERT(LEN +  3 == L.length());
    mL.appendInt64(E03);                        ASSERT(LEN +  4 == L.length());
    mL.appendFloat(E04);                        ASSERT(LEN +  5 == L.length());
    mL.appendDouble(E05);                       ASSERT(LEN +  6 == L.length());
    mL.appendString(E06);                       ASSERT(LEN +  7 == L.length());
    mL.appendDatetime(E07);                     ASSERT(LEN +  8 == L.length());
    mL.appendDate(E08);                         ASSERT(LEN +  9 == L.length());
    mL.appendTime(E09);                         ASSERT(LEN + 10 == L.length());

    mL.appendCharArray(E10);                    ASSERT(LEN + 11 == L.length());
    mL.appendShortArray(E11);                   ASSERT(LEN + 12 == L.length());
    mL.appendIntArray(E12);                     ASSERT(LEN + 13 == L.length());
    mL.appendInt64Array(E13);                   ASSERT(LEN + 14 == L.length());
    mL.appendFloatArray(E14);                   ASSERT(LEN + 15 == L.length());
    mL.appendDoubleArray(E15);                  ASSERT(LEN + 16 == L.length());
    mL.appendStringArray(E16);                  ASSERT(LEN + 17 == L.length());
    mL.appendDatetimeArray(E17);                ASSERT(LEN + 18 == L.length());
    mL.appendDateArray(E18);                    ASSERT(LEN + 19 == L.length());
    mL.appendTimeArray(E19);                    ASSERT(LEN + 20 == L.length());

    #ifndef BSLS_PLATFORM__CMP_MSVC
    mL.appendList(E20);                         ASSERT(LEN + 21 == L.length());
    #else
    mL.appendList(fE20());                      ASSERT(LEN + 21 == L.length());
    #endif
    mL.appendTable(E21);                        ASSERT(LEN + 22 == L.length());

    mL.appendBool(E22);                         ASSERT(LEN + 23 == L.length());
    mL.appendDatetimeTz(E23);                   ASSERT(LEN + 24 == L.length());
    mL.appendDateTz(E24);                       ASSERT(LEN + 25 == L.length());
    mL.appendTimeTz(E25);                       ASSERT(LEN + 26 == L.length());
    mL.appendBoolArray(E26);                    ASSERT(LEN + 27 == L.length());
    mL.appendDatetimeTzArray(E27);              ASSERT(LEN + 28 == L.length());
    mL.appendDateTzArray(E28);                  ASSERT(LEN + 29 == L.length());
    mL.appendTimeTzArray(E29);                  ASSERT(LEN + 30 == L.length());
    #ifndef BSLS_PLATFORM__CMP_MSVC
    mL.appendChoice(E30);                       ASSERT(LEN + 31 == L.length());
    mL.appendChoiceArray(E31);                  ASSERT(LEN + 32 == L.length());
    #else
    mL.appendChoice(fE30());                    ASSERT(LEN + 31 == L.length());
    mL.appendChoiceArray(fE31());               ASSERT(LEN + 32 == L.length());
    #endif
}

                        // ==============
                        // loadReferenceU
                        // ==============

void loadReferenceU(bdem_List *result)
    // Append to the specified 'result' a sequence of 32 element types
    // corresponding to the 'N' category Element values defined above.
{
    ASSERT(result);
    bdem_List& mL = *result; const bdem_List& L = mL; const int LEN=L.length();

    mL.appendChar(U00);                         ASSERT(LEN +  1 == L.length());
    mL.appendShort(U01);                        ASSERT(LEN +  2 == L.length());
    mL.appendInt(U02);                          ASSERT(LEN +  3 == L.length());
    mL.appendInt64(U03);                        ASSERT(LEN +  4 == L.length());
    mL.appendFloat(U04);                        ASSERT(LEN +  5 == L.length());
    mL.appendDouble(U05);                       ASSERT(LEN +  6 == L.length());
    mL.appendString(U06);                       ASSERT(LEN +  7 == L.length());
    mL.appendDatetime(U07);                     ASSERT(LEN +  8 == L.length());
    mL.appendDate(U08);                         ASSERT(LEN +  9 == L.length());
    mL.appendTime(U09);                         ASSERT(LEN + 10 == L.length());

    mL.appendCharArray(U10);                    ASSERT(LEN + 11 == L.length());
    mL.appendShortArray(U11);                   ASSERT(LEN + 12 == L.length());
    mL.appendIntArray(U12);                     ASSERT(LEN + 13 == L.length());
    mL.appendInt64Array(U13);                   ASSERT(LEN + 14 == L.length());
    mL.appendFloatArray(U14);                   ASSERT(LEN + 15 == L.length());
    mL.appendDoubleArray(U15);                  ASSERT(LEN + 16 == L.length());
    mL.appendStringArray(U16);                  ASSERT(LEN + 17 == L.length());
    mL.appendDatetimeArray(U17);                ASSERT(LEN + 18 == L.length());
    mL.appendDateArray(U18);                    ASSERT(LEN + 19 == L.length());
    mL.appendTimeArray(U19);                    ASSERT(LEN + 20 == L.length());

    mL.appendList(U20);                         ASSERT(LEN + 21 == L.length());
    mL.appendTable(U21);                        ASSERT(LEN + 22 == L.length());

    mL.appendBool(U22);                         ASSERT(LEN + 23 == L.length());
    mL.appendDatetimeTz(U23);                   ASSERT(LEN + 24 == L.length());
    mL.appendDateTz(U24);                       ASSERT(LEN + 25 == L.length());
    mL.appendTimeTz(U25);                       ASSERT(LEN + 26 == L.length());
    mL.appendBoolArray(U26);                    ASSERT(LEN + 27 == L.length());
    mL.appendDatetimeTzArray(U27);              ASSERT(LEN + 28 == L.length());
    mL.appendDateTzArray(U28);                  ASSERT(LEN + 29 == L.length());
    mL.appendTimeTzArray(U29);                  ASSERT(LEN + 30 == L.length());
    mL.appendChoice(U30);                       ASSERT(LEN + 31 == L.length());
    mL.appendChoiceArray(U31);                  ASSERT(LEN + 32 == L.length());
}

const int NUM_ELEMS = 32;

                        // ==============
                        // loadReferenceN
                        // ==============

void loadReferenceN(bdem_List *result)
    // Append to the specified 'result' a sequence of 32 element types
    // corresponding to the 'N' category Element values defined above.
{
    ASSERT(result);
    bdem_List& mL = *result; const bdem_List& L = mL; const int LEN=L.length();

    loadReferenceU(result);
    mL.makeAllNull();
}

                        // ====================
                        // generator function g
                        // ====================

static char SPECIFICATIONS[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef";// guaranteed
                                                                  // contiguous
static const int SPEC_LEN    = strlen(SPECIFICATIONS);

ET::Type getElemType(char spec)
    // Return the element type corresponding to the specified
    // 'spec' value.  Valid input consists of uppercase letters where the
    // index of each letter is in "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef".
{
    char *p = bsl::strchr(SPECIFICATIONS, spec);
    LOOP_ASSERT(spec, p);
    int index = p - SPECIFICATIONS;
    LOOP3_ASSERT(*p, index, SPEC_LEN, index < SPEC_LEN);
    return (ET::Type) index;
}

void makeElemRefUnset(bdem_ElemRef elemRef)
{
    const bdem_Descriptor *desc = elemRef.descriptor();
    desc->makeUnset(elemRef.data());
}

void gg(bdem_Table *target, const char *spec, const bdem_Row& reference)
    // Append to the specified 'target', a row consisting of elements
    // extracted from the specified 'reference' row according to the
    // specified 'spec' string.  Valid input consists of sequences of
    // CAPITAL letters from 'A' to 'Z' and from 'a' to 'e'.  Additionally,
    // each element type char can optionally be followed by '@' and '*'.  The
    // former specifies that that particular element should be made unset and
    // the latter specifies that it should be made null.  The position of
    // each non-blank letter in 'spec' identifies the the column index in
    // 'target'; the lexical difference between each letter and 'A' determines
    // the position of the source element in 'reference'.  The behavior is
    // undefined unless the number of active (non-whitespace) element type
    // characters N in 'spec' is less than or equal to 'target->numColumns()',
    // the indices calculated from the N active letters are each less than
    // 'reference->length()', and the types of the N row elements match
    // exactly.  Note that this function assumes that the 'bdem_Table'
    // column-types constructor, and 'appendNullRow' method have been
    // demonstrated to work properly.  Note also that this function can be used
    // in conjunction with the g function to create tables of up to 26 columns
    // of *any* value.
{
    ASSERT(target);
    ASSERT(spec);

    const int LENGTH      = reference.length();
    const int NUM_COLUMNS = target->numColumns();
    const int ROW_INDEX   = target->numRows();
    const char UNSET_CHAR = '@';
    const char NULL_CHAR  = '*';

    target->appendNullRow();
    bdem_Row& row = target->theModifiableRow(ROW_INDEX);
    int elemIdx = 0; // incremented with each value assigned

    for (const char *s = spec; *s; ++s) {
        if (' ' == *s || '\t' == *s || '\n' == *s) {
            continue; // ignore whitespace
        }
        char *p = strchr(SPECIFICATIONS, *s);
        LOOP_ASSERT(*s, p);

        int index = p - SPECIFICATIONS;
        LOOP3_ASSERT(*spec, LENGTH, index, index < LENGTH);
        LOOP3_ASSERT(*spec, NUM_COLUMNS, elemIdx, elemIdx < NUM_COLUMNS);

        const bdem_ElemType::Type LIST_TYPE = reference[index].type();
        const bdem_ElemType::Type TABLE_TYPE = target->columnType(elemIdx);
//------^
LOOP4_ASSERT(index, elemIdx, LIST_TYPE, TABLE_TYPE, LIST_TYPE == TABLE_TYPE);
//------v

        if (UNSET_CHAR == *(s + 1)) {
            makeElemRefUnset(row[elemIdx]);
            ++s;
        }
        else if (NULL_CHAR == *(s + 1)) {
            row[elemIdx].makeNull();
            ++s;
        }
        else {
            row[elemIdx].replaceValue(reference[index]);
        }
        ++elemIdx;
    }
}

                        // ====================
                        // generator function g
                        // ====================

bdem_Table g(const char *spec);
bdem_Table g(const char *spec,
             const bdem_Row& row1);
bdem_Table g(const char *spec,
             const bdem_Row& row1,
             const bdem_Row& row2);
bdem_Table g(const char *spec,
             const bdem_Row& row1,
             const bdem_Row& row2,
             const bdem_Row& row3);
    // Return a table with column types indicated by the specified 'spec'
    // string.  Valid input consists of sequences of CAPITAL letters from
    // 'A' to 'V' where the column's 'bdem_ElemType' is the difference of
    // the letter from 'A'.  The table will have as many rows as specified
    // 32-element references rows.  Note that this function assumes that the
    // 'bdem_Table' type constructor, copy constructor, and 'appendNullRow'
    // methods have been demonstrated to work properly.

bdem_Table g(const char *spec)
{
    vector<bdem_ElemType::Type> types;
    for (const char *s = spec; *s; ++s) {
        if (' ' == *s|| '\t' == *s|| '\n' == *s) {
            continue; // ignore whitespace
        }
        types.push_back(getElemType(*s));
        if ('@' == *(s + 1) || '*' == *(s + 1)) {
            ++s;
        }
    }
    if (types.size()) {
        return bdem_Table(&types.front(), types.size());
    }
    else {
        return bdem_Table();
    }
}

bdem_Table g(const char *spec,
             const bdem_Row& row1)
{
    bdem_Table result(g(spec));
    gg(&result, spec, row1);
    return result;
}

bdem_Table g(const char *spec,
             const bdem_Row& row1,
             const bdem_Row& row2)
{
    bdem_Table result(g(spec));
    gg(&result, spec, row1);
    gg(&result, spec, row2);
    return result;
}

bdem_Table g(const char *spec,
             const bdem_Row& row1,
             const bdem_Row& row2,
             const bdem_Row& row3)
{
    bdem_Table result(g(spec));
    gg(&result, spec, row1);
    gg(&result, spec, row2);
    gg(&result, spec, row3);
    return result;
}

bdem_List gList(const char *spec, const bdem_List& referenceList)
    // Return a list composed of the specified 'referenceList' elements
    // identified by the characters in the specified 'spec' string.  Valid
    // input consists of uppercase letters where the index of each letter in
    // "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef" is in the range [0
    // .. srcList.length()-1].  Note that this function assumes that the
    // 'bdem_List' copy constructor and its 'appendElement' method have been
    // demonstrate to work properly.
{
    // guaranteed contiguous
    static char A[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef";

    ASSERT(spec);
    const int LEN = referenceList.length();

    bdem_List result;

    for (const char *s = spec; *s; ++s) {
        if (' ' == *s || '\t' == *s || '\n' == *s) continue; // ignore WS
        char *p = strchr(A, *s);
        LOOP_ASSERT(*s, p);
        int index = p - A;
        LOOP3_ASSERT(*p, index, LEN, index < LEN);
        result.appendElement(referenceList[index]);
    }

    return result;
}

                        // =================
                        // isSameColumnValue
                        // =================

bool isSameColumnValue(const bdem_Table& table,
                       int columnIndex, const bdem_ConstElemRef& value)
// Return true if none of the element values in the column of the specified
// 'table' indicated by the specified 'columnIndex' differs from that of
// of the specified 'value', and false otherwise.  The behavior is undefined
// unless the type of 'value' and that of the indicated column are the same.
{
    LOOP_ASSERT(columnIndex, table.columnType(columnIndex) == value.type());
    int numRows = table.numRows();
    for (int i = 0; i < numRows; ++i) {
        if (value != table[i][columnIndex]) {
            return false;
        }
    }
    return true;
}
                        // ==================
                        // isSameExceptColumn
                        // ==================

bool isSameExceptColumn(const bdem_Table& table,
                        int               columnIndex,
                        const bdem_Row&   values)
// Return true if none of the element values in the specified table except
// those in the column indicated by the specified 'columnIndex' differs from
// that of the corresponding element in the specified 'values', and false
// otherwise.  The behavior is undefined unless the respective types of
// 'values' and those of 'table' match exactly.
{
    ASSERT(table.numColumns() == values.length());
    for (int c = 0; c < table.numColumns(); ++c) {
        LOOP_ASSERT(c, table.columnType(c) == values.elemType(c));
        if (columnIndex == c) {
            continue;
        }
        for (int r = 0; r < table.numRows(); ++r) {
            if (values[c] != table[r][c]) {
                return false;
            }
        }
    }
    return true;
}

                        // ================
                        // class BdexHelper
                        // ================

template <class CHAR_TYPE>
class BdexHelper {
    // This 'class' is used in testing of 'bdex' streaming whereby version 1
    // maps null values to "unset", and version 2 maps "unset" values to null.

    // DATA
    int d_version;

  public:
    BdexHelper(int version)
    : d_version(version)
    {
    }

    bool operator()(CHAR_TYPE& value) const
    {
       if (1 == d_version) {
           if ('*' == value) {
               value = '@';
           }
       }
       else if (2 == d_version) {
           if ('@' == value) {
               value = '*';
           }
       }
       return true;
    }
};

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

//=============================================================================
//                              TEST CASES
//-----------------------------------------------------------------------------

#ifdef BSLS_PLATFORM__CMP_MSVC
#define DECLARE_MAIN_VARIABLES                                                \
    const bdem_List        A20 = fA20();                                      \
    const bdem_List        B20 = fB20();                                      \
    const bdem_List        C20 = fC20();                                      \
    const bdem_List        D20 = fD20();                                      \
    const bdem_List        E20 = fE20();                                      \
    const bdem_Choice      A30 = fA30();                                      \
    const bdem_Choice      B30 = fB30();                                      \
    const bdem_Choice      C30 = fC30();                                      \
    const bdem_Choice      D30 = fD30();                                      \
    const bdem_Choice      E30 = fE30();                                      \
    const bdem_ChoiceArray A31 = fA31();                                      \
    const bdem_ChoiceArray B31 = fB31();                                      \
    const bdem_ChoiceArray C31 = fC31();                                      \
    const bdem_ChoiceArray D31 = fD31();                                      \
    const bdem_ChoiceArray E31 = fE31();
#else
#define DECLARE_MAIN_VARIABLES
#endif

#define DEFINE_TEST_CASE(NUMBER)                                              \
  void testCase##NUMBER(bool verbose,\
                        bool veryVerbose,\
                        bool veryVeryVerbose,\
                        bool veryVeryVeryVerbose)

DEFINE_TEST_CASE(23) {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Simple example illustrating how one might use a table.
        //
        // Concerns:
        //
        // Plan:
        //
        // Tactics:
        //   - Add-Hoc Data Selection Method
        //   - Broot-Force implementation technique
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        bslma_TestAllocator defaultAllocator;
        const bslma_DefaultAllocatorGuard dag(&defaultAllocator);

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

///Usage
///-----
// A 'bdem_Table', like a 'bdem_List', is designed to represent and transmit
// arbitrarily-complex self-describing data in and out of a process.  Both
// objects are containers for sub-objects of type 'bdem_Row'.  A 'bdem_Table'
// owns and manages a sequence of 0 or more 'bdem_Row' objects, whereas each
// 'bdem_List' always manages exactly one 'bdem_Row'.  Hence, 'bdem_Row' is the
// common "vocabulary" type that makes the data records contained in list and
// table objects interchangeable.
//
// We use a 'bdem_List' when we want to represent a single record (or C-style
// 'struct').  For example, we might choose to represent a single "Leg" of a
// securities-trading transaction as a 'bdem_List' having the following three
// fields:
//..
//  LEG: { STRING /* secId */, INT /* quantity */, TIME /* time */ }
//..
// i.e., a "security ID" of type 'BDEM_STRING', a "quantity" of type
// 'BDEM_INT', and a "time" of type 'BDEM_TIME'.  The 'bdem_List' would, in
// turn, contain a single 'bdem_Row' with the same sequence of field types.
// Note that, for simplicity, we have deliberately side-stepped the need for
// defining a "price" field.  Suppose we want to represent a sequence of two
// legs as in the following:
//..
//  ===========================================
//  *                 twoLegs                 *
//  -------------------------------------------
//  Field Mnemonic:  secId    quantity    time
//                   ~~~~~    ~~~~~~~~    ~~~~
//  Field Type:      STRING     INT       TIME
//                   ~~~~~~     ~~~       ~~~~
//  Leg 0:            IBM       1000      12:03
//
//  Leg 1:            TYC       -500      12:05
//  ===========================================
//..
// We could represent legs as a list of lists:
//..
    {
        bdem_List twoLegs;

        twoLegs.appendNullList();
        twoLegs[0].theModifiableList().appendString("IBM");
        twoLegs[0].theModifiableList().appendInt(1000);
        twoLegs[0].theModifiableList().appendTime(bdet_Time(12, 3));

        twoLegs.appendNullList();
        twoLegs[1].theModifiableList().appendString("TYC");
        twoLegs[1].theModifiableList().appendInt(-500);
        twoLegs[1].theModifiableList().appendTime(bdet_Time(12, 5));

//        twoLegs.print(cout);
    }
//..
// The above code snippet creates a 'bdem_List' with two similar sublists and
// produces the following list-based textual representation:
//..
//  {
//      LIST {
//          STRING "IBM"
//          INT 1000
//          TIME 12:03:00.000
//      }
//      LIST {
//          STRING "TYC"
//          INT -500
//          TIME 12:05:00.000
//      }
//  }
//..
// But given that the lengths and respective field types in each sublist are
// necessarily the same, this representation is sub-optimal.  We can instead
// represent a sequence of transactions more simply *and* *efficiently* in a
// 'bdem_Table':
//..
    {
        const bdem_ElemType::Type LEG[] = {
            bdem_ElemType::BDEM_STRING,      // column 0: secId
            bdem_ElemType::BDEM_INT,         // column 1: quantity
            bdem_ElemType::BDEM_TIME         // column 2: time
        };
        const int NUM_LEG_FIELDS = sizeof LEG / sizeof *LEG;

        bdem_Table twoLegs(LEG, NUM_LEG_FIELDS);
        twoLegs.appendNullRows(2);

        twoLegs.theModifiableRow(0)[0].theModifiableString() = "IBM";
        twoLegs.theModifiableRow(0)[1].theModifiableInt()    = 1000;
        twoLegs.theModifiableRow(0)[2].theModifiableTime()   = bdet_Time(12,3);

        twoLegs.theModifiableRow(1)[0].theModifiableString() = "TYC";
        twoLegs.theModifiableRow(1)[1].theModifiableInt()    = -500;
        twoLegs.theModifiableRow(1)[2].theModifiableTime()   = bdet_Time(12,5);

//      twoLegs.print(cout);
    }
//..
// This code creates a 'bdem_Table' object with three columns and two rows that
// represents the same information as the list of lists above.  The more
// compact representation is reflected in the text output below:
//..
//  {
//      [
//          STRING
//          INT
//          TIME
//      ]
//      {
//          "IBM"
//          1000
//          12:03:00.000
//      }
//      {
//          "TYC"
//          -500
//          12:05:00.000
//      }
//  }
//..
// Note that a much richer output format is achieved in conjunction with a
// corresponding 'bdem_Schema' when using generator methods that can be found
// in 'bdem_schemaaggregateutil'.
//
// Together, lists and tables support efficient representation of arbitrarily
// complex hierarchical data.  As a more comprehensive example, consider the
// following "MY SCHEMA" informally describing a set of hierarchical business
// types:
//..
//  ======================================================================
//  *                             MY SCHEMA                              *
//  ----------------------------------------------------------------------
//
//   bdem_List     bdem_Table     (Common) Row Fields for Lists and Tables
//   ---------     ------------   ----------------------------------------
//   LEG           LEGS           STRING secId, INT quantity, TIME time
//
//   TRANSACTION   TRANSACTIONS   LEGS legs, DATE date
//
//   POSITION      POSITIONS      STRING secId, INT64 quantity
//
//   ACCOUNT       ACCOUNTS       CHAR type, POSITIONS portfolio
//
//   CUSTOMER      CUSTOMERS      STRING name, STRING address,
//                                ACCOUNTS accounts, TRANSACTIONS history
//
//  ======================================================================
//..
// The first step is to convert this informal schema to code.  For the purpose
// of illustration, we will do so here by hand; However, use of 'bdem_schema'
// will greatly facilitate this task, and is strongly recommended:
//..
                // *** "LEG" RECORD DEFINITION ***

    const bdem_ElemType::Type LEG[] = {
        bdem_ElemType::BDEM_STRING,          // 0: STRING secId;
        bdem_ElemType::BDEM_INT,             // 1: INT    quantity;
        bdem_ElemType::BDEM_TIME             // 2: TIME   time;
    };
    const int LEG_NUM_FIELDS = sizeof LEG / sizeof *LEG;
    enum {
        LEG_SECID_INDEX,                     // 0: STRING secId;
        LEG_QUANTITY_INDEX,                  // 1: INT    quantity;
        LEG_TIME_INDEX,                      // 2: TIME   time;
        LEG_NUM_INDICES
    };
    ASSERT(LEG_NUM_INDICES == LEG_NUM_FIELDS);

                // *** "TRANSACTION" RECORD DEFINITION ***

    const bdem_ElemType::Type TRANSACTION[] = {
        bdem_ElemType::BDEM_TABLE,           // 0: TABLE<LEG> legs;
        bdem_ElemType::BDEM_DATE,            // 1: DATE       date;
    };
    const int TRANSACTION_NUM_FIELDS = sizeof TRANSACTION / sizeof*TRANSACTION;
    enum {
        TRANSACTION_LEGS_INDEX,              // 0: TABLE<LEG> legs;
        TRANSACTION_DATE_INDEX,              // 1: DATE       date;
        TRANSACTION_NUM_INDICES
    };
    ASSERT(TRANSACTION_NUM_INDICES == TRANSACTION_NUM_FIELDS);

                // *** "POSITION" RECORD DEFINITION ***

    const bdem_ElemType::Type POSITION[] = {
        bdem_ElemType::BDEM_STRING,          // 0: STRING secId;
        bdem_ElemType::BDEM_INT64            // 1: INT64  quantity;
    };
    const int POSITION_NUM_FIELDS = sizeof POSITION / sizeof *POSITION;
    enum {
        POSITION_SECID_INDEX,                // 0: STRING secId;
        POSITION_QUANTITY_INDEX,             // 1: INT64  quantity;
        POSITION_NUM_INDICES
    };
    ASSERT(POSITION_NUM_INDICES == POSITION_NUM_FIELDS);

                // *** "ACCOUNT" RECORD DEFINITION ***

    const bdem_ElemType::Type ACCOUNT[] = {
        bdem_ElemType::BDEM_CHAR,            // 0: CHAR            type;
        bdem_ElemType::BDEM_TABLE            // 1: TABLE<POSITION> portfolio;
    };
    const int ACCOUNT_NUM_FIELDS = sizeof ACCOUNT / sizeof *ACCOUNT;
    enum {
        ACCOUNT_TYPE_INDEX,                  // 0: CHAR            type;
        ACCOUNT_PORTFOLIO_INDEX,             // 1: TABLE<POSITION> portfolio;
        ACCOUNT_NUM_INDICES
    };
    ASSERT(ACCOUNT_NUM_INDICES == ACCOUNT_NUM_FIELDS);

                // *** "CUSTOMER" RECORD DEFINITION ***

    const bdem_ElemType::Type CUSTOMER[] = {
        bdem_ElemType::BDEM_STRING,          // 0: STRING             name;
        bdem_ElemType::BDEM_STRING,          // 1: STRING             address;
        bdem_ElemType::BDEM_TABLE,           // 2: TABLE<ACCOUNT>     accounts;
        bdem_ElemType::BDEM_TABLE            // 3: TABLE<TRANSACTION> history;
    };
    const int CUSTOMER_NUM_FIELDS = sizeof CUSTOMER / sizeof *CUSTOMER;
    enum {
        CUSTOMER_NAME_INDEX,                 // 0: STRING             name;
        CUSTOMER_ADDRESS_INDEX,              // 1: STRING             address;
        CUSTOMER_ACCOUNTS_INDEX,             // 2: TABLE<ACCOUNT>     accounts;
        CUSTOMER_HISTORY_INDEX,              // 3: TABLE<TRANSACTION> history;
        CUSTOMER_NUM_INDICES
    };
    ASSERT(CUSTOMER_NUM_INDICES == CUSTOMER_NUM_FIELDS);
//..
// Now suppose we want to build up an in-core representation of a single
// customer record for "John Smith" on "Easy Street" having two accounts: 'B'
// for "Brokerage" and 'R' for "Retirement".  Furthermore, we want to populate
// each account with a number of positions (3 and 2, respectively) and simulate
// a plausible recent trading history that has lead to the current positions:
//..
//  {
//    STRING "John Smith"                           // customer.name
//    STRING "Easy Street"                          // customer.address
//    TABLE { [ CHAR TABLE                    ]     // customer.accounts
//            { 'B'  { [ STRING   INT64   ]         //   'B' for "Brokerage"
//                     { "USD"    4522500 }         //      Position 0
//                     { "IBM"    1000    }         //      Position 1
//                     { "TYC"    -500    } } }     //      Position 2
//            { 'R'  { [ STRING   INT64   ]         //   'R' for "Retirement"
//                     { "USD"    1275000 }         //      Position 0
//                     { "NT"     800     } } }     //      Position 1
//    }                                             // customer.history
//    TABLE { [ TABLE                                   DATE      ]  // History
//            { { [ STRING  INT        TIME         ]                //
//                { "USD"   100000000  10:52:00.000 } } 02JAN2000 }  // H0.Leg0
//            { { [ STRING  INT        TIME         ]
//                { "USD"   -6350000   09:13:00.000 }                // H1.Leg0
//                { "IBM"   1000       09:13:00.000 } } 04JAN2000 }  // H1.Leg1
//            { { [ STRING  INT        TIME         ]
//                { "USD"   -250000    14:10:00.000 }                // H2.Leg0
//                { "NT"    1000       14:10:00.000 } } 28FEB2000 }  // H2.Leg1
//            { { [ STRING  INT        TIME         ]
//                { "USD"   1750000    09:23:00.000 }                // H3.Leg0
//                { "TYC"   500        09:23:00.000 } } 15MAR2000 }  // H3.Leg1
//    }
//  }
//..
// The following block-structured code produces the output shown above (albeit,
// less compactly formatted and, of course, without '//' annotations):
//..
    bdem_List aCustomer(CUSTOMER, CUSTOMER_NUM_FIELDS);
    {
      ASSERT(4 == aCustomer.length());
      aCustomer[CUSTOMER_NAME_INDEX].theModifiableString()    = "John Smith";
      aCustomer[CUSTOMER_ADDRESS_INDEX].theModifiableString() = "Easy Street";

      bdem_Table& accounts =
                       aCustomer[CUSTOMER_ACCOUNTS_INDEX].theModifiableTable();
      {
        accounts.reset(ACCOUNT, ACCOUNT_NUM_FIELDS);
        accounts.appendNullRows(2);
        {
          bdem_Row& row = accounts.theModifiableRow(0);
          row[ACCOUNT_TYPE_INDEX].theModifiableChar() = 'B';
          bdem_Table& portfolio =
                             row[ACCOUNT_PORTFOLIO_INDEX].theModifiableTable();
          {
            portfolio.reset(POSITION, POSITION_NUM_FIELDS);
            portfolio.appendNullRows(3);
            {
              bdem_Row& row = portfolio.theModifiableRow(0);
              row[POSITION_SECID_INDEX].theModifiableString()   = "USD";
              row[POSITION_QUANTITY_INDEX].theModifiableInt64() = 4522500;
            }
            {
              bdem_Row& row = portfolio.theModifiableRow(1);
              row[POSITION_SECID_INDEX].theModifiableString()   = "IBM";
              row[POSITION_QUANTITY_INDEX].theModifiableInt64() = 1000;
            }
            {
              bdem_Row& row = portfolio.theModifiableRow(2);
              row[POSITION_SECID_INDEX].theModifiableString()   = "TYC";
              row[POSITION_QUANTITY_INDEX].theModifiableInt64() = -500;
            }
          }
        }
        {
          bdem_Row& row = accounts.theModifiableRow(1);
          row[ACCOUNT_TYPE_INDEX].theModifiableChar() = 'R';
          bdem_Table& portfolio =
                             row[ACCOUNT_PORTFOLIO_INDEX].theModifiableTable();
          {
            portfolio.reset(POSITION, POSITION_NUM_FIELDS);
            portfolio.appendNullRows(2);
            {
              bdem_Row& row = portfolio.theModifiableRow(0);
              row[POSITION_SECID_INDEX].theModifiableString()   = "USD";
              row[POSITION_QUANTITY_INDEX].theModifiableInt64() = 1275000;
            }
            {
              bdem_Row& row = portfolio.theModifiableRow(1);
              row[POSITION_SECID_INDEX].theModifiableString()   = "NT";
              row[POSITION_QUANTITY_INDEX].theModifiableInt64() = 800;
            }
          }
        }
      }
      bdem_Table& history =
                        aCustomer[CUSTOMER_HISTORY_INDEX].theModifiableTable();
      {
        history.reset(TRANSACTION, TRANSACTION_NUM_FIELDS);
        history.appendNullRows(4);
        {
          bdem_Row& row = history.theModifiableRow(0);
          bdem_Table& legs = row[TRANSACTION_LEGS_INDEX].theModifiableTable();
          {
            legs.reset(LEG, LEG_NUM_FIELDS);
            legs.appendNullRows(1);
            {
              bdem_Row& row = legs.theModifiableRow(0);
              row[LEG_SECID_INDEX].theModifiableString() = "USD";
              row[LEG_QUANTITY_INDEX].theModifiableInt() = 100000000;
              row[LEG_TIME_INDEX].theModifiableTime()    = bdet_Time(10, 52);
            }
          }
          row[TRANSACTION_DATE_INDEX].theModifiableDate() =
                                                         bdet_Date(2000, 1, 2);
        }
        {
          bdem_Row& row = history.theModifiableRow(1);
          bdem_Table& legs = row[TRANSACTION_LEGS_INDEX].theModifiableTable();
          {
            legs.reset(LEG, LEG_NUM_FIELDS);
            legs.appendNullRows(2);
            {
              bdem_Row& row = legs.theModifiableRow(0);
              row[LEG_SECID_INDEX].theModifiableString() = "USD";
              row[LEG_QUANTITY_INDEX].theModifiableInt() = -6350000;
              row[LEG_TIME_INDEX].theModifiableTime()    = bdet_Time(9, 13);
            }
            {
              bdem_Row& row = legs.theModifiableRow(1);
              row[LEG_SECID_INDEX].theModifiableString() = "IBM";
              row[LEG_QUANTITY_INDEX].theModifiableInt() = 1000;
              row[LEG_TIME_INDEX].theModifiableTime()    = bdet_Time(9, 13);
            }
          }
          row[TRANSACTION_DATE_INDEX].theModifiableDate() =
                                                         bdet_Date(2000, 1, 4);
        }
        {
          bdem_Row& row = history.theModifiableRow(2);
          bdem_Table& legs = row[TRANSACTION_LEGS_INDEX].theModifiableTable();
          {
            legs.reset(LEG, LEG_NUM_FIELDS);
            legs.appendNullRows(2);
            {
              bdem_Row& row = legs.theModifiableRow(0);
              row[LEG_SECID_INDEX].theModifiableString() = "USD";
              row[LEG_QUANTITY_INDEX].theModifiableInt() = -250000;
              row[LEG_TIME_INDEX].theModifiableTime()    = bdet_Time(14, 10);
            }
            {
              bdem_Row& row = legs.theModifiableRow(1);
              row[LEG_SECID_INDEX].theModifiableString() = "NT";
              row[LEG_QUANTITY_INDEX].theModifiableInt() = 1000;
              row[LEG_TIME_INDEX].theModifiableTime()    = bdet_Time(14, 10);
            }
          }
          row[TRANSACTION_DATE_INDEX].theModifiableDate() =
                                                        bdet_Date(2000, 2, 28);
        }
        {
          bdem_Row& row = history.theModifiableRow(3);
          bdem_Table& legs = row[TRANSACTION_LEGS_INDEX].theModifiableTable();
          {
            legs.reset(LEG, LEG_NUM_FIELDS);
            legs.appendNullRows(2);
            {
              bdem_Row& row = legs.theModifiableRow(0);
              row[LEG_SECID_INDEX].theModifiableString() = "USD";
              row[LEG_QUANTITY_INDEX].theModifiableInt() = 1750000;
              row[LEG_TIME_INDEX].theModifiableTime()    = bdet_Time(9, 23);
            }
            {
              bdem_Row& row = legs.theModifiableRow(1);
              row[LEG_SECID_INDEX].theModifiableString() = "TYC";
              row[LEG_QUANTITY_INDEX].theModifiableInt() = 500;
              row[LEG_TIME_INDEX].theModifiableTime()    = bdet_Time(9, 23);
            }
          }
          row[TRANSACTION_DATE_INDEX].theModifiableDate() =
                                                        bdet_Date(2000, 3, 15);
        }
      }
    }

//  aCustomer.print(cout);
//..
// The order in which the data structure is created can significantly affect
// runtime efficiency.  Notice in the above code that we started with the
// customer list and worked our way down.  By starting at the top we minimize
// the amount of copying that was needed to create the data structure.   If
// instead we create the lower level constructs (e.g., "POSITION") first, we
// would then have to copy those values into the higher-level constructs
// (e.g., "ACCOUNT"), which, in turn, would then have to be copied into the
// "CUSTOMER" list.
//
// Furthermore, if we know that we are going to build up the list once and not
// subsequently modify it (much), significant additional runtime and spatial
// performance gains can be achieved throughout the instance hierarchy, simply
// by supplying the 'BDEM_WRITE_ONCE' hint to the constructor of the top-level
// list (or table):
//..
    bdem_List fastCustomer(CUSTOMER, CUSTOMER_NUM_FIELDS,
                           bdem_AggregateOption::BDEM_WRITE_ONCE);
//..
// Robustness, as well as efficiency, is achieved through the use of 'bdem'
// lists and tables.  As our final illustration, consider that we wish to
// augment MY SCHEMA above to include two new record types:
//..
//  =================================================================
//                       NEW RECORD DEFINITIONS
//
//  REPORT   REPORTS   CUSTOMER subject, DATETIME asOf
//
//  UPDATE   UPDATES   INT custId, INT accountId, TRANSACTIONS events
//
//  =================================================================
//..
// Rendering these new types in code is straightforward:
//..
                // *** "REPORT" RECORD DEFINITION ***

    const bdem_ElemType::Type REPORT[] = {
        bdem_ElemType::BDEM_LIST,            // 0: LIST<CUSTOMER> subject;
        bdem_ElemType::BDEM_DATETIME,        // 1: DATETIME       asOf;
    };
    const int REPORT_NUM_FIELDS = sizeof REPORT / sizeof *REPORT;
    enum {
        REPORT_SUBJECT_INDEX,                // 0: LIST<CUSTOMER> subject;
        REPORT_ASOF_INDEX,                   // 1: DATETIME       asOf;
        REPORT_NUM_INDICES
    };
    ASSERT(REPORT_NUM_INDICES == REPORT_NUM_FIELDS);

                // *** "UPDATE" RECORD DEFINITION ***

    const bdem_ElemType::Type UPDATE[] = {
        bdem_ElemType::BDEM_INT,             // 0: INT custId;
        bdem_ElemType::BDEM_INT,             // 1: INT accountId;
        bdem_ElemType::BDEM_TABLE            // 2: TABLE<TRANSACTION> events;
    };
    const int UPDATE_NUM_FIELDS = sizeof UPDATE / sizeof *UPDATE;
    enum {
        UPDATE_CUSTID_INDEX,                 // 0: INT custId;
        UPDATE_ACCOUNTID_INDEX,              // 1: INT accountId;
        UPDATE_EVENT_INDEX,                  // 2: TABLE<TRANSACTION> events;
        UPDATE_NUM_INDICES
    };
    ASSERT(UPDATE_NUM_INDICES == UPDATE_NUM_FIELDS);
//..
// Notice, however, that the latter of these new types makes reference to a
// customer id and an account id -- fields that now need to be added to the
// respective, previously-defined records.  Our concern here is that we do not
// want to break existing code that currently has no need for these new fields,
// yet we want to make them available where they are needed.  The goal is
// achieved by supplying new definitions for the affected records, with the
// missing fields appended to the ends, respectively:
//..
//  ========================================================================
//                       AUGMENTED RECORD DEFINITIONS
//
//  ACCOUNT    ACCOUNTS      CHAR type, POSITIONS portfolio, INT accountId
//
//  CUSTOMER   CUSTOMERS     STRING name, STRING address, ACCOUNTS accounts,
//                           TRANSACTIONS history, INT custId
//  ========================================================================
//..
// For new applications, we can now use the extended definitions to create,
// read, and write ACCOUNT and CUSTOMER structures; existing code will be
// able to read the relevant (leading) fields without having to recompile:
//..
    {
                // *** AUGMENTED "ACCOUNT" RECORD DEFINITION ***

        const bdem_ElemType::Type ACCOUNT[] = {
            bdem_ElemType::BDEM_CHAR,          // 0: CHAR            type;
            bdem_ElemType::BDEM_TABLE,         // 1: TABLE<POSITION> portfolio;
            bdem_ElemType::BDEM_INT            // 1: INT             accountId;
        };
        const int ACCOUNT_NUM_FIELDS = sizeof ACCOUNT / sizeof *ACCOUNT;
        enum {
            ACCOUNT_TYPE_INDEX,                // 0: CHAR            type;
            ACCOUNT_PORTFOLIO_INDEX,           // 1: TABLE<POSITION> portfolio;
            ACCOUNT_ACCOUNTID_INDEX,           // 2: INT             accountId;
            ACCOUNT_NUM_INDICES
        };
        ASSERT(ACCOUNT_NUM_INDICES == ACCOUNT_NUM_FIELDS);

                    // *** AUGMENTED "CUSTOMER" RECORD DEFINITION ***

        const bdem_ElemType::Type CUSTOMER[] = {
            bdem_ElemType::BDEM_STRING,      // 0: STRING             name;
            bdem_ElemType::BDEM_STRING,      // 1: STRING             address;
            bdem_ElemType::BDEM_TABLE,       // 2: TABLE<ACCOUNT>     accounts;
            bdem_ElemType::BDEM_TABLE,       // 3: TABLE<TRANSACTION> history;
            bdem_ElemType::BDEM_INT          // 4: INT                custId;
        };
        const int CUSTOMER_NUM_FIELDS = sizeof CUSTOMER / sizeof *CUSTOMER;
        enum {
            CUSTOMER_NAME_INDEX,             // 0: STRING             name;
            CUSTOMER_ADDRESS_INDEX,          // 1: STRING             address;
            CUSTOMER_ACCOUNTS_INDEX,         // 2: TABLE<ACCOUNT>     accounts;
            CUSTOMER_HISTORY_INDEX,          // 3: TABLE<TRANSACTION> history;
            CUSTOMER_CUSTID_INDEX,           // 4: INT                custId;
            CUSTOMER_NUM_INDICES
        };
        ASSERT(CUSTOMER_NUM_INDICES == CUSTOMER_NUM_FIELDS);
    }
//..
// In the examples above, we have hard-coded all of the indices at
// compile-time.  Using 'bdem_Schema', however, it is possible to look up and
// install the needed indices at runtime, thereby relaxing the requirement that
// augmented record fields must necessarily be appended to the ends of existing
// records.  For more information, see 'bdem_schema'.

        // eliminate compiler warnings: "set but never used"
        {
            ASSERT(REPORT[0] == bdem_ElemType::BDEM_LIST);
            ASSERT(UPDATE[0] == bdem_ElemType::BDEM_INT);
        }

      }

DEFINE_TEST_CASE(22) {
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

        bslma_TestAllocator defaultAllocator;
        const bslma_DefaultAllocatorGuard dag(&defaultAllocator);

        if (verbose) cout << "\nTesting allocator traits"
                          << "\n========================" << endl;

        typedef bdem_Table Obj;

        ASSERT((0 == bslmf_IsConvertible<bslma_Allocator*, Obj>::VALUE));
        ASSERT((1 ==
             bslalg_HasTrait<Obj, bslalg_TypeTraitUsesBslmaAllocator>::VALUE));
      }

DEFINE_TEST_CASE(21) {
    // --------------------------------------------------------------------
    // TESTING 'reserveRaw' and 'getCapacityRaw'
    //
    // Concerns:
    // 1 'reserveRaw' correctly forwards to the method 
    //   'bdem_TableImp::reserveRaw'.
    //
    // Plan:
    //
    // Testing:
    //   void reserveRaw(int numRows);
    // --------------------------------------------------------------------
 
    if (verbose) cout << "\nTesting 'reserveRaw' and 'getCapacityRaw'"
                      << "\n=================================================" 
                      << endl;
    
    static const Strategy STRATEGY_DATA[] = {
            BDEM_PASS_THROUGH,
            BDEM_WRITE_ONCE,
            BDEM_WRITE_MANY
    };
    enum { STRATEGY_LEN = sizeof(STRATEGY_DATA) / sizeof(*STRATEGY_DATA) };
    
    for (int i = 0; i < STRATEGY_LEN; i++) {
 
        const Strategy STRATEGY = STRATEGY_DATA[i];
        
        bslma_TestAllocator ta1("TestAllocator 1", veryVeryVeryVerbose);
        bslma_TestAllocator ta2("TestAllocator 2", veryVeryVeryVerbose);
    
        Obj    mX(STRATEGY, &ta1); const Obj&    X = mX;
        ObjImp mY(STRATEGY, &ta2); const ObjImp& Y = mY;
        
        for (int j = 1; j <= 1024; j <<= 1) {
            mX.reserveRaw(j);
            mY.reserveRaw(j);
 
            LOOP4_ASSERT(i, 
                         j,
                         X.getCapacityRaw(),
                         Y.getCapacityRaw(),
                         X.getCapacityRaw() == Y.getCapacityRaw());
 
            LOOP4_ASSERT(i,
                         j,
                         ta1.numBytesInUse(),
                         ta2.numBytesInUse(),
                         ta1.numBytesInUse() == ta2.numBytesInUse());
            
            LOOP4_ASSERT(i,
                         j,
                         ta1.numBytesTotal(),
                         ta2.numBytesTotal(),
                         ta1.numBytesTotal() == ta2.numBytesTotal());
        }
    }
}

DEFINE_TEST_CASE(20) {
        // --------------------------------------------------------------------
        // TESTING 'reserveMemory'
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   void reserveMemory(int numBytes);
        // --------------------------------------------------------------------

        bslma_TestAllocator defaultAllocator;
        const bslma_DefaultAllocatorGuard dag(&defaultAllocator);

        if (verbose) cout << "\nTesting 'reserveMemory'"
                          << "\n=======================" << endl;

        const int SIZE = 4000;

        if (verbose) cout << "\nUsing 'BDEM_WRITE_ONCE'" << endl;
        {
            bslma_TestAllocator allocator;
            bdem_Table mX(bdem_AggregateOption::BDEM_WRITE_ONCE, &allocator);

            const int beforeSize = allocator.numBytesInUse();

            mX.reserveMemory(SIZE);

            LOOP2_ASSERT(allocator.numBytesInUse(), beforeSize,
                         SIZE <= allocator.numBytesInUse() - beforeSize);
        }

        if (verbose) cout << "\nUsing 'BDEM_WRITE_MANY'" << endl;
        {
            bslma_TestAllocator allocator;
            bdem_Table mX(bdem_AggregateOption::BDEM_WRITE_MANY, &allocator);

            const int beforeSize = allocator.numBytesInUse();

            mX.reserveMemory(SIZE);

            LOOP2_ASSERT(allocator.numBytesInUse(), beforeSize,
                         SIZE <= allocator.numBytesInUse() - beforeSize);
        }

        if (verbose) cout << "\nEnd of 'reserveMemory' Test." << endl;
      }

DEFINE_TEST_CASE(19) {
        // --------------------------------------------------------------------
        // TESTING CTORS WITH INITIAL MEMORY
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   InitialMemory
        // --------------------------------------------------------------------

        bslma_TestAllocator defaultAllocator;
        const bslma_DefaultAllocatorGuard dag(&defaultAllocator);

        if (verbose) cout << "\nTESTING CTORS WITH INITIAL MEMORY"
                          << "\n=================================" << endl;

        const int SIZE = 4000;  // initial memory

        if (verbose) cout << "\nWith allocMode only." << endl;
        {
            if (veryVerbose) cout << "\tUsing 'BDEM_WRITE_ONCE'." << endl;
            {
                bslma_TestAllocator allocator;

                bdem_Table mX(bdem_AggregateOption::BDEM_WRITE_ONCE,
                              bdem_Table::InitialMemory(SIZE),
                              &allocator);

                LOOP_ASSERT(allocator.numBytesInUse(),
                            SIZE <= allocator.numBytesInUse());
            }
            if (veryVerbose) cout << "\tUsing 'BDEM_WRITE_MANY'." << endl;
            {
                bslma_TestAllocator allocator;

                bdem_Table mX(bdem_AggregateOption::BDEM_WRITE_MANY,
                              bdem_Table::InitialMemory(SIZE),
                              &allocator);

                LOOP_ASSERT(allocator.numBytesInUse(),
                            SIZE <= allocator.numBytesInUse());
            }
        }

        if (verbose) cout << "\nWith element types." << endl;
        {
            if (veryVerbose) cout << "\tUsing 'BDEM_WRITE_ONCE'." << endl;
            {
                const bdem_ElemType::Type CHAR = bdem_ElemType::BDEM_CHAR;

                bslma_TestAllocator allocator;

                bdem_Table mX(&CHAR,
                              1,
                              bdem_AggregateOption::BDEM_WRITE_ONCE,
                              bdem_Table::InitialMemory(SIZE),
                              &allocator);

                LOOP_ASSERT(allocator.numBytesInUse(),
                            SIZE <= allocator.numBytesInUse());
            }
            if (veryVerbose) cout << "\tUsing 'BDEM_WRITE_MANY'." << endl;
            {
                const bdem_ElemType::Type CHAR = bdem_ElemType::BDEM_CHAR;

                bslma_TestAllocator allocator;

                bdem_Table mX(&CHAR,
                              1,
                              bdem_AggregateOption::BDEM_WRITE_MANY,
                              bdem_Table::InitialMemory(SIZE),
                              &allocator);

                LOOP_ASSERT(allocator.numBytesInUse(),
                            SIZE <= allocator.numBytesInUse());
            }
        }

        if (verbose) cout << "\nWith element types vector." << endl;
        {
            if (veryVerbose) cout << "\tUsing 'BDEM_WRITE_ONCE'." << endl;
            {
                const bsl::vector<bdem_ElemType::Type> CHAR_VECTOR(
                                                     1,
                                                     bdem_ElemType::BDEM_CHAR);

                bslma_TestAllocator allocator;

                bdem_Table mX(CHAR_VECTOR,
                              bdem_AggregateOption::BDEM_WRITE_ONCE,
                              bdem_Table::InitialMemory(SIZE),
                              &allocator);

                LOOP_ASSERT(allocator.numBytesInUse(),
                            SIZE <= allocator.numBytesInUse());
            }
            if (veryVerbose) cout << "\tUsing 'BDEM_WRITE_MANY'." << endl;
            {
                const bsl::vector<bdem_ElemType::Type> CHAR_VECTOR(
                                                     1,
                                                     bdem_ElemType::BDEM_CHAR);

                bslma_TestAllocator allocator;

                bdem_Table mX(CHAR_VECTOR,
                              bdem_AggregateOption::BDEM_WRITE_MANY,
                              bdem_Table::InitialMemory(SIZE),
                              &allocator);

                LOOP_ASSERT(allocator.numBytesInUse(),
                            SIZE <= allocator.numBytesInUse());
            }
        }

        if (verbose) cout << "\nWith copy ctor." << endl;
        {
            if (veryVerbose) cout << "\tUsing 'BDEM_WRITE_ONCE'." << endl;
            {
                bslma_TestAllocator allocator;
                bdem_Table original;

                bdem_Table mX(original,
                              bdem_AggregateOption::BDEM_WRITE_ONCE,
                              bdem_Table::InitialMemory(SIZE),
                              &allocator);

                LOOP_ASSERT(allocator.numBytesInUse(),
                            SIZE <= allocator.numBytesInUse());
            }
            if (veryVerbose) cout << "\tUsing 'BDEM_WRITE_MANY'." << endl;
            {
                bslma_TestAllocator allocator;
                bdem_Table original;

                bdem_Table mX(original,
                              bdem_AggregateOption::BDEM_WRITE_MANY,
                              bdem_Table::InitialMemory(SIZE),
                              &allocator);

                LOOP_ASSERT(allocator.numBytesInUse(),
                            SIZE <= allocator.numBytesInUse());
            }
        }

        if (verbose) cout << "\nEnd of 'InitialMemory' Test." << endl;
      }

DEFINE_TEST_CASE(18) {
        // --------------------------------------------------------------------
        // SWAP ROWS
        //   Ability to swap two rows efficiently.
        //
        // Concerns:
        //   - That rows with any sequence of types are handled properly.
        //   - That rows at all positions are handled properly.
        //   - That indices need not be ordered (e.g., i < j).
        //   - That no allocation occurs as a result of a swap.
        //   - That swapping a row with itself has no effect.
        //   - That row values, but *NOT* the rows themselves are swapped,
        //      just as would be the case with an "in-place" array (i.e.,
        //      the address vale &T[i] is not affected by a swap).
        //   - That the address value &T[i][j] is not affected by a swap.
        //
        // Plan:
        //   - Create a 32-element reference list with A-type values.
        //   - Create an empty reference table with 32 unique columns.
        //   - for each of an increasingly large number of rows,
        //      for each allocation strategy,
        //       Use swapping adjacent elements to reverse the rows in the
        //       tables.  Do it again to resore the list and compare with
        //       the original table.
        //   - Provide an explicit allocator; ensure that no additional
        //      allocation occur after construction and that the
        //      default allocator is not used at all.
        //   - Separately create a 32-element list with with a few rows;
        //      Swap each row with itself and verify that there is no effect.
        //   - Finally create a zero-column table with two rows; swap these
        //      rows and verify that the addresses returned by indexing are
        //      are the same as before.
        //   - Repeat with a one-column table; observe that although the values
        //      are swapped, each individual element address also behaves as if
        //      it is "in place".
        //
        // Tactics:
        //   - Add-Hoc Data Selection Method
        //   - Brute-Force and Loop-Based implementation techniques
        //
        // Testing:
        //   void swapRows(int rowIndex1, int rowIndex2);
        // --------------------------------------------------------------------

        bslma_TestAllocator defaultAllocator;
        const bslma_DefaultAllocatorGuard dag(&defaultAllocator);

        if (verbose) cout << endl
                  << "SWAP ELEMENTS" << endl
                  << "=============" << endl;

        if (verbose) cout << "\nSet up reference lists." << endl;

        bdem_List mA; const bdem_List& A = mA; ASSERT(0         == A.length());
        loadReferenceA(&mA);                   ASSERT(NUM_ELEMS == A.length());
        bdem_List mB; const bdem_List& B = mB; ASSERT(0         == B.length());
        loadReferenceB(&mB);                   ASSERT(NUM_ELEMS == B.length());
        bdem_List mC; const bdem_List& C = mC; ASSERT(0         == C.length());
        loadReferenceC(&mC);                   ASSERT(NUM_ELEMS == C.length());
        bdem_List mD; const bdem_List& D = mD; ASSERT(0         == D.length());
        loadReferenceD(&mD);                   ASSERT(NUM_ELEMS == D.length());
        bdem_List mE; const bdem_List& E = mE; ASSERT(0         == E.length());
        loadReferenceE(&mE);                   ASSERT(NUM_ELEMS == E.length());
        if (veryVeryVerbose) {
            cout << "\tlist A: "; A.print(cout, -1, 8);
            cout << "\tlist B: "; B.print(cout, -1, 8);
            cout << "\tlist C: "; C.print(cout, -1, 8);
            cout << "\tlist D: "; D.print(cout, -1, 8);
            cout << "\tlist E: "; E.print(cout, -1, 8);
        }

        if (verbose) cout << "\nSet up reference tables." << endl;

        const char COLUMN_SPEC_32[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef";

        bdem_Table mTT = g(COLUMN_SPEC_32); const bdem_Table& TT = mTT;
        gg(&mTT, COLUMN_SPEC_32, A.row());
        gg(&mTT, COLUMN_SPEC_32, B.row());

        bdem_Table mTTT = TT; const bdem_Table& TTT = mTTT;
        gg(&mTTT, COLUMN_SPEC_32, C.row());
        gg(&mTTT, COLUMN_SPEC_32, D.row());
        gg(&mTTT, COLUMN_SPEC_32, E.row());
        mTTT.appendNullRow(); // Last row is appended, but not used in test.

        if (veryVeryVerbose) {
            cout << "\tTable TT: "; TT.print(cout, -1, 8);
            cout << "\tTable TTT: "; TTT.print(cout, -1, 8);
        }
        ASSERT(2 ==  TT.numRows()); ASSERT(NUM_ELEMS ==  TT.numColumns());
        ASSERT(6 == TTT.numRows()); ASSERT(NUM_ELEMS == TTT.numColumns());

        const int MIN_NUM_ROWS = TT.numRows();
        const int MAX_NUM_ROWS = TTT.numRows() - 1;

        if (verbose) cout << "\nSet up strategies array." << endl;

        const bdem_AggregateOption::AllocationStrategy STRATEGIES[] = {
            bdem_AggregateOption::BDEM_PASS_THROUGH,
            bdem_AggregateOption::BDEM_WRITE_ONCE,
            bdem_AggregateOption::BDEM_WRITE_MANY
        };
        const int NUM_STRATEGIES = sizeof STRATEGIES / sizeof *STRATEGIES;

        if (veryVerbose) {
            for (int i = 0; i < NUM_STRATEGIES; ++i) {
                T_ P_(i) P(STRATEGIES[i])
            }
        }

        if (verbose) cout << "\nTest swap on 32-column tables." << endl;

        for (int numRows = MIN_NUM_ROWS; numRows <= MAX_NUM_ROWS; ++numRows) {
            if (verbose) { T_ P(numRows) }
            LOOP_ASSERT(numRows, numRows == TT.numRows());

            for (int si = 0; si < NUM_STRATEGIES; ++si) {
                const bdem_AggregateOption::AllocationStrategy STRATEGY =
                                                               STRATEGIES[si];
                if (veryVerbose) { T_ T_ P_(si) P(STRATEGY) }
 // v--------------^
    bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);

    bslma_TestAllocator a;  // explicit allocation

    bdem_Table mT(TT, STRATEGY, &a); const bdem_Table& T = mT;
    const int USAGE = a.numBlocksTotal();
    LOOP3_ASSERT(numRows, si, USAGE, USAGE > 0);

    LOOP3_ASSERT(numRows, si, a.numBlocksTotal(), USAGE == a.numBlocksTotal());
    LOOP3_ASSERT(numRows, si, da.numBlocksTotal(), 0 == da.numBlocksTotal());

    if (veryVeryVerbose) { cout << "\tBefore T = "; T.print(cout, -1, 8); }
    ASSERT(TT == T);

    if (veryVeryVerbose) {
        cout << endl <<
        "*************************** BEFORE **************************"
        << endl;
        cout << "SPECIFIED ALLOCATOR:" << endl;
        a.print();
        cout << "\nDEFAULT ALLOCATOR:" << endl;
        da.print();
    }
    LOOP3_ASSERT(numRows, si, da.numBlocksTotal(), 0 == da.numBlocksTotal());
    LOOP3_ASSERT(numRows, si, a.numBlocksTotal(), USAGE == a.numBlocksTotal());

    for (int i = 1; i < numRows; ++i) {
        if (veryVeryVerbose) { T_ T_ T_ P(i) }
        for (int j = 0; j < i; ++j) {
            if (veryVeryVerbose) { T_ T_ T_ T_ P(j) }

            if (veryVeryVerbose) { T_ T_ T_ T_ P(T) }
            mT.swapRows(i, j);
            if (veryVeryVerbose) { T_ T_ T_ T_ P(T) }
        }
    }

    if (veryVeryVerbose) { cout << "\tDuring T = "; T.print(cout, -1, 8); }
    LOOP2_ASSERT(numRows, si, TT != T);

    if (veryVeryVerbose) {
        cout << endl <<
        "*************************** DURING **************************"
        << endl;
        cout << "SPECIFIED ALLOCATOR:" << endl;
        a.print();
        cout << "\nDEFAULT ALLOCATOR:" << endl;
        da.print();
    }
    LOOP3_ASSERT(numRows, si, da.numBlocksTotal(), 0 == da.numBlocksTotal());
    LOOP3_ASSERT(numRows, si, a.numBlocksTotal(), USAGE == a.numBlocksTotal());

    for (int i = 0; i < numRows - 1; ++i) {
        if (veryVeryVerbose) { T_ T_ P(i) }
        for (int j = i + 1; j < numRows; ++j) {
            if (veryVeryVerbose) { T_ T_ T_ P(j) }

            if (veryVeryVerbose) { T_ T_ T_ P(T) }
            mT.swapRows(i, j);
            if (veryVeryVerbose) { T_ T_ T_ P(T) }
        }
    }

    if (veryVeryVerbose) { cout << "\tAfter T = "; T.print(cout, -1, 8); }
    LOOP2_ASSERT(numRows, si, TT == T);

    if (veryVerbose) {
        cout << endl <<
        "*************************** AFTER ***************************"
        << endl;
        cout << "SPECIFIED ALLOCATOR:" << endl;
        a.print();
        cout << "\nDEFAULT ALLOCATOR:" << endl;
        da.print();
    }

    LOOP3_ASSERT(numRows, si, da.numBlocksTotal(), 0 == da.numBlocksTotal());
    LOOP3_ASSERT(numRows, si, a.numBlocksTotal(), USAGE == a.numBlocksTotal());
 // ^----------v
            } // for each strategy, 'si'

            // Extend from TTT number of unique rows for reference table TT.
            gg(&mTT, COLUMN_SPEC_32, TTT[numRows]);

        } // for each 'numRows'

        if (verbose) cout << "\nAlias test." << endl;
        {
            bdem_Table mT(TTT); const bdem_Table& T = mT;
            for (int i = 0; i < TTT.numRows(); ++i) {
                if (veryVerbose) { T_ P(i) }
                LOOP3_ASSERT(i, T, TTT, TTT == T);
            }
        }

        if (verbose) cout << "\nTest on zero-column table." << endl;
        {
            bdem_Table mT; const bdem_Table& T = mT;
            mT.appendNullRow();
            mT.appendNullRow();
            ASSERT(2 == T.numRows()); ASSERT(0 == T.numColumns());

            bdem_Table mX(T); const bdem_Table& X = mX;
            ASSERT(2 == X.numRows()); ASSERT(0 == X.numColumns());

            const bdem_Row *const P0 = &X[0];

            if (veryVerbose) cout << "\tBefore." << endl;
            ASSERT(T == X);
            ASSERT(P0 == &X[0]);
            ASSERT(P0 != &X[1]);
            ASSERT(P0 == &X[0]);
            ASSERT(P0 != &X[1]);

            mT.swapRows(1, 0);

            if (veryVerbose) cout << "\tDuring." << endl;
            ASSERT(T == X);
            ASSERT(P0 == &X[0]);
            ASSERT(P0 != &X[1]);
            ASSERT(P0 == &X[0]);
            ASSERT(P0 != &X[1]);

            mT.swapRows(0, 1);

            if (veryVerbose) cout << "\tAfter." << endl;
            ASSERT(T == X);
            ASSERT(P0 == &X[0]);
            ASSERT(P0 != &X[1]);
            ASSERT(P0 == &X[0]);
            ASSERT(P0 != &X[1]);
        }

        if (verbose) cout << "\nTest on one-column table." << endl;
        {
            bdem_ElemType::Type t = bdem_ElemType::BDEM_INT;

            bdem_Table mT(&t, 1); const bdem_Table& T = mT;
            mT.appendNullRow();
            mT.appendNullRow();
            mT.theModifiableRow(0)[0].theModifiableInt() = A02;
            mT.theModifiableRow(1)[0].theModifiableInt() = B02;
            ASSERT(2 == T.numRows()); ASSERT(1 == T.numColumns());

            bdem_Table mX(T); const bdem_Table& X = mX;
            ASSERT(2 == X.numRows()); ASSERT(1 == X.numColumns());

            const bdem_Row *const P0 = &X[0];
            const bdem_Row *const P1 = &X[1];

            const int *const PE0 = &X[0][0].theInt();
            const int *const PE1 = &X[1][0].theInt();

            if (veryVerbose) cout << "\tBefore." << endl;
            ASSERT(T == X);
            ASSERT(P0 == &X[0]);
            ASSERT(P0 != &X[1]);
            ASSERT(P1 != &X[0]);
            ASSERT(P1 == &X[1]);
            ASSERT(PE0 == &X[0][0].theInt());
            ASSERT(PE0 != &X[1][0].theInt());
            ASSERT(PE1 != &X[0][0].theInt());
            ASSERT(PE1 == &X[1][0].theInt());

            mT.swapRows(0, 1);

            if (veryVerbose) cout << "\tDuring." << endl;
            ASSERT(T != X);
            ASSERT(P0 == &X[0]);
            ASSERT(P0 != &X[1]);
            ASSERT(P1 != &X[0]);
            ASSERT(P1 == &X[1]);
            ASSERT(PE0 == &X[0][0].theInt());
            ASSERT(PE0 != &X[1][0].theInt());
            ASSERT(PE1 != &X[0][0].theInt());
            ASSERT(PE1 == &X[1][0].theInt());

            mT.swapRows(0, 1);

            if (veryVerbose) cout << "\tAfter." << endl;
            ASSERT(T == X);
            ASSERT(P0 == &X[0]);
            ASSERT(P0 != &X[1]);
            ASSERT(P1 != &X[0]);
            ASSERT(P1 == &X[1]);
            ASSERT(PE0 == &X[0][0].theInt());
            ASSERT(PE0 != &X[1][0].theInt());
            ASSERT(PE1 != &X[0][0].theInt());
            ASSERT(PE1 == &X[1][0].theInt());
        }

        if (verbose) cout << "\nTest on zero-column table (null)." << endl;
        {
            bdem_Table mT; const bdem_Table& T = mT;
            mT.appendNullRow();
            mT.appendNullRow();
            ASSERT(2 == T.numRows()); ASSERT(0 == T.numColumns());

            bdem_Table mX(T); const bdem_Table& X = mX;
            ASSERT(2 == X.numRows()); ASSERT(0 == X.numColumns());

            const bdem_Row *const P0 = &X[0];

            if (veryVerbose) cout << "\tBefore." << endl;
            ASSERT(T == X);
            ASSERT(P0 == &X[0]);
            ASSERT(P0 != &X[1]);
            ASSERT(P0 == &X[0]);
            ASSERT(P0 != &X[1]);

            mT.swapRows(1, 0);

            if (veryVerbose) cout << "\tDuring." << endl;
            ASSERT(T  == X);
            ASSERT(P0 == &X[0]);
            ASSERT(P0 != &X[1]);
            ASSERT(P0 == &X[0]);
            ASSERT(P0 != &X[1]);

            mT.swapRows(0, 1);

            if (veryVerbose) cout << "\tAfter." << endl;
            ASSERT(T == X);
            ASSERT(P0 == &X[0]);
            ASSERT(P0 != &X[1]);
            ASSERT(P0 == &X[0]);
            ASSERT(P0 != &X[1]);
        }

        if (verbose) cout << "\nTest on one-column table." << endl;
        {
            bdem_ElemType::Type t = bdem_ElemType::BDEM_INT;

            bdem_Table mT(&t, 1); const bdem_Table& T = mT;
            mT.appendNullRow();
            mT.appendNullRow();
            mT.theModifiableRow(0)[0].theModifiableInt() = A02;
            ASSERT(2 == T.numRows()); ASSERT(1 == T.numColumns());

            bdem_Table mX(T); const bdem_Table& X = mX;
            ASSERT(2 == X.numRows()); ASSERT(1 == X.numColumns());

            const bdem_Row *const P0 = &X[0];
            const bdem_Row *const P1 = &X[1];

            const int *const PE0 = &X[0][0].theInt();
            const int *const PE1 = &X[1][0].theInt();

            if (veryVerbose) cout << "\tBefore." << endl;
            ASSERT(T   ==  X);
            ASSERT(0   ==  T.theRow(0)[0].isNull());
            ASSERT(1   ==  T.theRow(1)[0].isNull());
            ASSERT(P0  == &X[0]);
            ASSERT(P0  != &X[1]);
            ASSERT(P1  != &X[0]);
            ASSERT(P1  == &X[1]);
            ASSERT(PE0 == &X[0][0].theInt());
            ASSERT(PE0 != &X[1][0].theInt());
            ASSERT(PE1 != &X[0][0].theInt());
            ASSERT(PE1 == &X[1][0].theInt());

            mT.swapRows(0, 1);

            if (veryVerbose) cout << "\tDuring." << endl;
            ASSERT(T   !=  X);
            ASSERT(1   ==  T.theRow(0)[0].isNull());
            ASSERT(0   ==  T.theRow(1)[0].isNull());
            ASSERT(P0  == &X[0]);
            ASSERT(P0  != &X[1]);
            ASSERT(P1  != &X[0]);
            ASSERT(P1  == &X[1]);
            ASSERT(PE0 == &X[0][0].theInt());
            ASSERT(PE0 != &X[1][0].theInt());
            ASSERT(PE1 != &X[0][0].theInt());
            ASSERT(PE1 == &X[1][0].theInt());

            mT.swapRows(0, 1);

            if (veryVerbose) cout << "\tAfter." << endl;
            ASSERT(T   ==  X);
            ASSERT(0   ==  T.theRow(0)[0].isNull());
            ASSERT(1   ==  T.theRow(1)[0].isNull());
            ASSERT(P0  == &X[0]);
            ASSERT(P0  != &X[1]);
            ASSERT(P1  != &X[0]);
            ASSERT(P1  == &X[1]);
            ASSERT(PE0 == &X[0][0].theInt());
            ASSERT(PE0 != &X[1][0].theInt());
            ASSERT(PE1 != &X[0][0].theInt());
            ASSERT(PE1 == &X[1][0].theInt());
        }

        if (verbose) cout << "\nTest on one-column table." << endl;
        {
            bdem_ElemType::Type t = bdem_ElemType::BDEM_INT;

            bdem_Table mT(&t, 1); const bdem_Table& T = mT;
            mT.appendNullRow();
            mT.appendNullRow();
            mT.theModifiableRow(0)[0].theModifiableInt() = A02;
            ASSERT(2 == T.numRows()); ASSERT(1 == T.numColumns());

            bdem_Table mX(T); const bdem_Table& X = mX;
            ASSERT(2 == X.numRows()); ASSERT(1 == X.numColumns());

            const bdem_Row *const P0 = &X[0];
            const bdem_Row *const P1 = &X[1];

            const int *const PE0 = &X[0][0].theInt();
            const int *const PE1 = &X[1][0].theInt();

            if (veryVerbose) cout << "\tBefore." << endl;
            ASSERT(T   ==  X);
            ASSERT(0   ==  T.theRow(0)[0].isNull());
            ASSERT(1   ==  T.theRow(1)[0].isNull());
            ASSERT(P0  == &X[0]);
            ASSERT(P0  != &X[1]);
            ASSERT(P1  != &X[0]);
            ASSERT(P1  == &X[1]);
            ASSERT(PE0 == &X[0][0].theInt());
            ASSERT(PE0 != &X[1][0].theInt());
            ASSERT(PE1 != &X[0][0].theInt());
            ASSERT(PE1 == &X[1][0].theInt());

            mT.swapRows(0, 1);

            if (veryVerbose) cout << "\tDuring." << endl;
            ASSERT(T   !=  X);
            ASSERT(1   ==  T.theRow(0)[0].isNull());
            ASSERT(0   ==  T.theRow(1)[0].isNull());
            ASSERT(P0  == &X[0]);
            ASSERT(P0  != &X[1]);
            ASSERT(P1  != &X[0]);
            ASSERT(P1  == &X[1]);
            ASSERT(PE0 == &X[0][0].theInt());
            ASSERT(PE0 != &X[1][0].theInt());
            ASSERT(PE1 != &X[0][0].theInt());
            ASSERT(PE1 == &X[1][0].theInt());

            mT.swapRows(0, 1);

            if (veryVerbose) cout << "\tAfter." << endl;
            ASSERT(T   ==  X);
            ASSERT(0   ==  T.theRow(0)[0].isNull());
            ASSERT(1   ==  T.theRow(1)[0].isNull());
            ASSERT(P0  == &X[0]);
            ASSERT(P0  != &X[1]);
            ASSERT(P1  != &X[0]);
            ASSERT(P1  == &X[1]);
            ASSERT(PE0 == &X[0][0].theInt());
            ASSERT(PE0 != &X[1][0].theInt());
            ASSERT(PE1 != &X[0][0].theInt());
            ASSERT(PE1 == &X[1][0].theInt());
        }

        if (verbose) cout << "\nTest on 60-column table." << endl;
        {
            bdem_List mU;  const bdem_List& U = mU;
                                           ASSERT(0             == U.length());
            loadReferenceU(&mU);           ASSERT(NUM_ELEMS     == U.length());
            loadReferenceU(&mU);           ASSERT(NUM_ELEMS * 2 == U.length());
            bdem_List mN;  const bdem_List& N = mN;
                                           ASSERT(0             == N.length());
            loadReferenceN(&mN);           ASSERT(NUM_ELEMS     == N.length());
            loadReferenceN(&mN);           ASSERT(NUM_ELEMS * 2 == N.length());

            const char SPEC[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefABCDEFGHIJKLMNOPQRSTUVWXYZabcdef";
            bdem_Table mT = g(SPEC); const bdem_Table& T = mT;
            mT.appendNullRow();
            mT.appendNullRow();
            ASSERT(N.row() == T.theRow(0));
            ASSERT(N.row() == T.theRow(1));
            ASSERT(2 == T.numRows()); ASSERT(NUM_ELEMS * 2 == T.numColumns());

            mT.swapRows(0, 1);

            ASSERT(N.row() == T.theRow(0));
            ASSERT(N.row() == T.theRow(1));
            ASSERT(2 == T.numRows()); ASSERT(NUM_ELEMS * 2 == T.numColumns());

            mT.swapRows(0, 1);

            ASSERT(N.row() == T.theRow(0));
            ASSERT(N.row() == T.theRow(1));
            ASSERT(2 == T.numRows()); ASSERT(NUM_ELEMS * 2 == T.numColumns());
        }

        if (verbose) cout << "\nTest swapping null with set row." << endl;
        {
            bdem_List mN;  const bdem_List& N = mN;
                                           ASSERT(0             == N.length());
            loadReferenceN(&mN);           ASSERT(NUM_ELEMS     == N.length());
            loadReferenceN(&mN);           ASSERT(NUM_ELEMS * 2 == N.length());
            bdem_List mA;  const bdem_List& A = mA;
                                           ASSERT(0             == A.length());
            loadReferenceA(&mA);           ASSERT(NUM_ELEMS     == A.length());
            loadReferenceA(&mA);           ASSERT(NUM_ELEMS * 2 == A.length());

            const char SPEC[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefABCDEFGHIJKLMNOPQRSTUVWXYZabcdef";
            bdem_Table mT = g(SPEC); const bdem_Table& T = mT;
            mT.appendNullRow();
            mT.appendRow(A);
            ASSERT(N.row() == T.theRow(0));
            ASSERT(A.row() == T.theRow(1));
            ASSERT(2 == T.numRows()); ASSERT(NUM_ELEMS * 2 == T.numColumns());

            mT.swapRows(0, 1);

            ASSERT(A.row() == T.theRow(0));
            ASSERT(N.row() == T.theRow(1));
            ASSERT(2 == T.numRows()); ASSERT(NUM_ELEMS * 2 == T.numColumns());

            mT.swapRows(0, 1);

            ASSERT(N.row() == T.theRow(0));
            ASSERT(A.row() == T.theRow(1));
            ASSERT(2 == T.numRows()); ASSERT(NUM_ELEMS * 2 == T.numColumns());
        }
      }

DEFINE_TEST_CASE(17) {
        // --------------------------------------------------------------------
        // REMOVING All ROWS/REMOVING ALL
        //   Ability to revert back to an empty/unset state.
        //
        // Concerns:
        //   - That the appropriate rows are removed.
        //   - That the methods work properly with zero-column tables.
        //   - That removeAllRows leaves columns unchanged.
        //   - That removeAll leaves the table empty.
        //   - That each works with the various allocation Strategies.
        //
        // Plan:
        //   - Create a 32-element reference list with A-type values.
        //   - Create two empty reference tables:
        //     + EMPTY_0   with 0 columns
        //     + EMPTY_32  with 32 unique columns
        //   - for each of an increasingly large number of rows,
        //      for each allocation strategy,
        //       Copy construct both tables; remove all rows, verify columns.
        //       Copy construct both tables; remove all, verify numColumns = 0.
        //
        // Tactics:
        //   - Add-Hoc and Area Data Selection Methods.
        //   - Brute Force and Loop-Based Implementation Techniques.
        //
        // Testing:
        //   void removeAllRows();
        //   void removeAll();
        // --------------------------------------------------------------------

        bslma_TestAllocator defaultAllocator;
        const bslma_DefaultAllocatorGuard dag(&defaultAllocator);

        if (verbose) cout << endl
                          << "REMOVE ALL ROWS/REMOVE ALL" << endl
                          << "==========================" << endl;

        if (verbose) cout << "\nSet up reference list." << endl;

        bdem_List mA; const bdem_List& A = mA; ASSERT(0         == A.length());
        loadReferenceA(&mA);                   ASSERT(NUM_ELEMS == A.length());
        if (veryVeryVerbose) {
            cout << "\tlist A: "; A.print(cout, -1, 8);
        }

        if (verbose) cout << "\nSet up strategies array." << endl;

        const bdem_AggregateOption::AllocationStrategy STRATEGIES[] = {
            bdem_AggregateOption::BDEM_PASS_THROUGH,
            bdem_AggregateOption::BDEM_WRITE_ONCE,
            bdem_AggregateOption::BDEM_WRITE_MANY
        };
        const int NUM_STRATEGIES = sizeof STRATEGIES / sizeof *STRATEGIES;

        if (veryVerbose) {
            for (int i = 0; i < NUM_STRATEGIES; ++i) {
                T_ P_(i) P(STRATEGIES[i])
            }
        }

        if (verbose) cout << "\nSet up reference tables." << endl;

        const char COLUMN_SPEC_32[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef";

        bdem_Table mTT0; const bdem_Table& TT0 = mTT0;
        bdem_Table mTT32 = g(COLUMN_SPEC_32); const bdem_Table& TT32 = mTT32;

        const bdem_Table ET_0  = TT0;  // empty table with 0 columns
        const bdem_Table ET_32 = TT32; // empty table with 32 columns

        if (verbose) cout << "\nInstall test allocator as the default." <<endl;
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag2(&da);

        if (verbose) cout << "\nTest both manipulators." << endl;

        const int NUM_ROW_TESTS = 10;

        for (int numRows = 0; numRows < NUM_ROW_TESTS; ++numRows) {
            if (verbose) { T_ P(numRows) }
            LOOP_ASSERT(numRows, numRows == TT0.numRows());
            LOOP_ASSERT(numRows, numRows == TT32.numRows());

            for (int si = 0; si < NUM_STRATEGIES; ++si) {
                const bdem_AggregateOption::AllocationStrategy STRATEGY =
                                                               STRATEGIES[si];

                if (veryVerbose) { T_ T_ P_(si) P(STRATEGY) }

                if (veryVerbose) cout << "\t\t\tvoid removeAllRows();" << endl;
                {
                    bdem_Table mT0 (TT0, STRATEGY); const bdem_Table& T0 = mT0;
                    if (veryVeryVerbose) { T_ T_ T_ T_ P(T0) }
                    mT0.removeAllRows();
                    if (veryVeryVerbose) { T_ T_ T_ T_ P(T0) }
                    LOOP4_ASSERT(numRows, STRATEGY, ET_0, T0, ET_0 == T0)

                    bdem_Table mT32(TT32, STRATEGY);const bdem_Table& T32=mT32;
                    if (veryVeryVerbose) { T_ T_ T_ T_ P(T32) }
                    mT32.removeAllRows();
                    if (veryVeryVerbose) { T_ T_ T_ T_ P(T32) }
                    LOOP4_ASSERT(numRows, STRATEGY, ET_32, T32, ET_32 == T32)
                }

                if (veryVerbose) cout << "\t\t\tvoid removeAll();" << endl;
                {
                    bdem_Table mT0 (TT0, STRATEGY); const bdem_Table& T0 = mT0;
                    if (veryVeryVerbose) { T_ T_ T_ T_ P(T0) }
                    mT0.removeAll();
                    if (veryVeryVerbose) { T_ T_ T_ T_ P(T0) }
                    LOOP4_ASSERT(numRows, STRATEGY, ET_0, T0, ET_0 == T0)

                    bdem_Table mT32(TT32, STRATEGY);const bdem_Table& T32=mT32;
                    if (veryVeryVerbose) { T_ T_ T_ T_ P(T32) }
                    mT32.removeAll();
                    if (veryVeryVerbose) { T_ T_ T_ T_ P(T32) }
                    LOOP4_ASSERT(numRows, STRATEGY, ET_0, T32, ET_0 == T32)
                }

            } // for each strategy, 'si'

            // Extend number of rows for both reference tables.
            mTT0.appendNullRow();
            gg(&mTT32, COLUMN_SPEC_32, A.row());

        } // for each 'numRows'

      }

DEFINE_TEST_CASE(16) {
        // --------------------------------------------------------------------
        // APPEND/INSERT NULL ELEMENTS
        //   Ability to append/insert rows with null element values.
        //
        // Concerns:
        //   - That elements of the correct type are appended.
        //   - That the correct number of unset rows are appended.
        //   - That the methods work properly with zero-column tables.
        //
        // Plan:
        //   - Create two 32-element reference lists with A and N values.
        //   - Create a short set of representative columns sequence specs.
        //   - For each column sequence,
        //     + Create a "control" table with corresponding columns (using g).
        //     + For each of a sequence initial number of rows containing, A's.
        //     +  For each of a sequence of contiguous unset rows to be added.
        //        =Copy construct the control table, and also an additional
        //         table (in a separate block) for each method that applies.
        //        =Compare results in each block with reference table
        //         constructed using only appendNullRow (and replaceValues).
        //
        // Tactics:
        //   - Add-Hoc and Area Data Selection Methods.
        //   - Loop-Based Implementation Techniques
        //
        // Testing:
        //   void appendNullRows(int rowCount);
        //   void insertNullRow(int dstIndex);
        //   void insertNullRows(int dstIndex, int rowCount);
        // --------------------------------------------------------------------

        bslma_TestAllocator defaultAllocator;
        const bslma_DefaultAllocatorGuard dag(&defaultAllocator);

        if (verbose) cout << endl
                          << "APPEND/INSERT UNSET/NULL ELEMENTS" << endl
                          << "=================================" << endl;

        if (verbose) cout << "\nSet up reference lists." << endl;

        bdem_List mA; const bdem_List& A = mA; ASSERT(0         == A.length());
        loadReferenceA(&mA);                   ASSERT(NUM_ELEMS == A.length());

        bdem_List mU; const bdem_List& U = mU; ASSERT(0         == U.length());
        loadReferenceU(&mU);                   ASSERT(NUM_ELEMS == U.length());

        bdem_List mN; const bdem_List& N = mN; ASSERT(0         == N.length());
        loadReferenceN(&mN);                   ASSERT(NUM_ELEMS == N.length());

        if (veryVeryVerbose) {
            cout << "\tlist A: "; A.print(cout, -1, 8);
            cout << "\tlist U: "; U.print(cout, -1, 8);
            cout << "\tlist N: "; N.print(cout, -1, 8);
        }

        if (verbose) cout << "\nCreate column specifications." << endl;
        const char *COLUMN_SPECS[] = {
            "A", "", "BC", "DEF", "GHIJ", "KLMNO", "PQRSTUV"
        };
        const int NUM_COLUMN_SPECS = sizeof COLUMN_SPECS/sizeof *COLUMN_SPECS;

        if (veryVerbose) {
            for (int i = 0; i < NUM_COLUMN_SPECS; ++i) {
                T_ P_(i) P(COLUMN_SPECS[i]);
            }
        }

        const int NUM_INITIAL_ROW_TESTS = 6;    // { 0 .. 5 }
        const int NUM_INSERTED_ROW_TESTS = 6;   // { 0 .. 5 }

        if (verbose) cout << "\nTest append/insert unset manipulators." <<endl;
        for (int ci = 0; ci < NUM_COLUMN_SPECS; ++ci) {
            const char *const COLUMN_SPEC = COLUMN_SPECS[ci];
            if (verbose) { T_ P_(ci) P(COLUMN_SPEC) }

            const bool PRINT = veryVerbose && ci == 0 || veryVeryVerbose;

            bdem_Table mTTT = g(COLUMN_SPEC); const bdem_Table& TTT = mTTT;
            const bdem_Table EMPTY_CONTROL = TTT; // Used to build exptd value.
            if (veryVeryVerbose) { T_ T_ P(TTT) }
            LOOP_ASSERT(ci, (int)strlen(COLUMN_SPECS[ci]) == TTT.numColumns());

            for (int numRows = 0; numRows < NUM_INITIAL_ROW_TESTS; ++numRows) {
                if (PRINT) { T_ T_ P(numRows) }
                LOOP2_ASSERT(ci, numRows, numRows == TTT.numRows());

                for (int ri = 0; ri <= numRows; ++ri) { // insert position
                    if (PRINT) { T_ T_ T_ P(ri) }

                    for (int ni = 0; ni < NUM_INSERTED_ROW_TESTS; ++ni) {
                        if (PRINT) { T_ T_ T_ T_ P(ni) }

                        bdem_Table mTT=EMPTY_CONTROL; const bdem_Table& TT=mTT;

                        if (veryVeryVerbose) { T_ T_ T_ T_ T_ Q(start) }
                        for (int si = 0; si < ri; ++si) {
                            if (veryVeryVerbose) { T_ T_ T_ T_ T_ P(si) }
                            gg(&mTT, COLUMN_SPEC, A.row());
                        }

                        if (veryVeryVerbose) { T_ T_ T_ T_ T_ Q(middle) }
                        for (int mj = 0; mj < ni; ++mj) {
                            if (veryVeryVerbose) { T_ T_ T_ T_ T_ P(mj) }
                            mTT.appendNullRow();
                        }

                        if (veryVeryVerbose) { T_ T_ T_ T_ T_ Q(end) }
                        for (int ei = ri; ei < numRows; ++ei) {
                            if (veryVeryVerbose) { T_ T_ T_ T_ T_ P(ei) }
                            gg(&mTT, COLUMN_SPEC, A.row());
                        }
                        if (veryVeryVerbose) { T_ T_ T_ T_ T_ P(TT) }
 // v-------------------^

    if (numRows == ri) {
        if (PRINT) cout << "\t\t\t\t\tappendNullRows(int idx)" << endl;
        bdem_Table mT = mTTT; const bdem_Table& T = mT;
        mT.appendNullRows(ni);
        if (veryVeryVerbose) { T_ T_ T_ T_ T_ P(T) }
        LOOP6_ASSERT(ci, numRows, ri, ni, TT, T, TT == T);
    }

    if (1 == ni) {
        if (PRINT) cout << "\t\t\t\t\tInsertNullRow(int idx)" << endl;
        bdem_Table mT = mTTT; const bdem_Table& T = mT;
        mT.insertNullRow(ri);
        if (veryVeryVerbose) { T_ T_ T_ T_ T_ P(T) }
        LOOP6_ASSERT(ci, numRows, ri, ni, TT, T, TT == T);
    }

    if (1) {
        if (PRINT) cout <<
                    "\t\t\t\t\tInsertUnsetRows(int idx, int rowCount)" << endl;
        bdem_Table mT = mTTT; const bdem_Table& T = mT;
        mT.insertNullRows(ri, ni);
        if (veryVeryVerbose) { T_ T_ T_ T_ T_ P(T) }
        LOOP6_ASSERT(ci, numRows, ri, ni, TT, T, TT == T);
    }

 // ^-------------------v
                    } // for each 'ni'
                } // for each 'ri'

                gg(&mTTT, COLUMN_SPEC, A.row()); // Extend reference table.

            } // for each 'numRows'
        } // for each 'ci'

        if (verbose) cout << "\nTest append/insert null manipulators." <<endl;
        for (int ci = 0; ci < NUM_COLUMN_SPECS; ++ci) {
            const char *const COLUMN_SPEC = COLUMN_SPECS[ci];
            if (verbose) { T_ P_(ci) P(COLUMN_SPEC) }

            const bool PRINT = veryVerbose && ci == 0 || veryVeryVerbose;

            bdem_Table mTTT = g(COLUMN_SPEC); const bdem_Table& TTT = mTTT;
            const bdem_Table EMPTY_CONTROL = TTT; // Used to build exptd value.
            if (veryVeryVerbose) { T_ T_ P(TTT) }
            LOOP_ASSERT(ci, (int)strlen(COLUMN_SPECS[ci]) == TTT.numColumns());

            for (int numRows = 0; numRows < NUM_INITIAL_ROW_TESTS; ++numRows) {
                if (PRINT) { T_ T_ P(numRows) }
                LOOP2_ASSERT(ci, numRows, numRows == TTT.numRows());

                for (int ri = 0; ri <= numRows; ++ri) { // insert position
                    if (PRINT) { T_ T_ T_ P(ri) }

                    for (int ni = 0; ni < NUM_INSERTED_ROW_TESTS; ++ni) {
                        if (PRINT) { T_ T_ T_ T_ P(ni) }

                        bdem_Table mTT=EMPTY_CONTROL; const bdem_Table& TT=mTT;

                        if (veryVeryVerbose) { T_ T_ T_ T_ T_ Q(start) }
                        for (int si = 0; si < ri; ++si) {
                            if (veryVeryVerbose) { T_ T_ T_ T_ T_ P(si) }
                            gg(&mTT, COLUMN_SPEC, A.row());
                        }

                        if (veryVeryVerbose) { T_ T_ T_ T_ T_ Q(middle) }
                        for (int mj = 0; mj < ni; ++mj) {
                            if (veryVeryVerbose) { T_ T_ T_ T_ T_ P(mj) }
                            gg(&mTT, COLUMN_SPEC, N.row());
                        }

                        if (veryVeryVerbose) { T_ T_ T_ T_ T_ Q(end) }
                        for (int ei = ri; ei < numRows; ++ei) {
                            if (veryVeryVerbose) { T_ T_ T_ T_ T_ P(ei) }
                            gg(&mTT, COLUMN_SPEC, A.row());
                        }
                        if (veryVeryVerbose) { T_ T_ T_ T_ T_ P(TT) }
 // v-------------------^

    if (numRows == ri) {
        if (PRINT) cout << "\t\t\t\t\tappendNullRows(int idx)" << endl;
        bdem_Table mT = mTTT; const bdem_Table& T = mT;
        bdem_Table mN(TT); const bdem_Table& N = mN;
        mT.appendNullRows(ni);
        mN.makeRowsNull(ri, ni);
        if (veryVeryVerbose) { T_ T_ T_ T_ T_ P(T) }
        LOOP6_ASSERT(ci, numRows, ri, ni, N, T, N == T);
    }

    if (1 == ni) {
        if (PRINT) cout << "\t\t\t\t\tInsertNullRow(int idx)" << endl;
        bdem_Table mT = mTTT; const bdem_Table& T = mT;
        bdem_Table mN(TT); const bdem_Table& N = mN;
        mT.insertNullRow(ri);
        mN.makeRowsNull(ri, 1);
        if (veryVeryVerbose) { T_ T_ T_ T_ T_ P(T) }
        LOOP6_ASSERT(ci, numRows, ri, ni, N, T, N == T);
    }

    if (1) {
        if (PRINT) cout <<
                    "\t\t\t\t\tInsertNullRows(int idx, int rowCount)" << endl;
        bdem_Table mT = mTTT; const bdem_Table& T = mT;
        bdem_Table mN(TT); const bdem_Table& N = mN;
        mT.insertNullRows(ri, ni);
        mN.makeRowsNull(ri, ni);
        if (veryVeryVerbose) { T_ T_ T_ T_ T_ P(T) }
        LOOP6_ASSERT(ci, numRows, ri, ni, N, T, N == T);
    }

 // ^-------------------v
                    } // for each 'ni'
                } // for each 'ri'

                gg(&mTTT, COLUMN_SPEC, A.row()); // Extend reference table.

            } // for each 'numRows'
        } // for each 'ci'

      }

DEFINE_TEST_CASE(15) {
        // --------------------------------------------------------------------
        // IS UNSET/MAKE NULL/IS NULL
        //   Ability to make and verify that elements are null.
        //
        // Concerns:
        //   - That all element types are considered.
        //   - That elements at all relevant positions are considered.
        //   - That accessors never have an affect on subject tables.
        //   - That zero row/column tables are handled properly.
        //   - That manipulators have no effect on tables with no elements.
        //
        // Plan:
        //   - Create reference lists, A, N each containing all 32 elem types
        //   - Create controls for subject tables:
        //      + empty
        //      + multiple row, zero columns
        //      + zero rows, 32 rows
        //      + multiple rows, 32 columns (All null)
        //      + multiple rows, 32 columns (All set)
        //   - Try each of the accessor functions on these types.
        //   - For each element in the last two respective tables, swap the
        //      "setness" and verify the appropriate accessors detect the
        //      change.
        //   - Apply the manipulators to each of the subject tables.
        //   - Use controls to verify that
        //       + accessors never affect the subject.
        //       + manipulators have no effect for tables with no elements.
        //   - Use the *now* *proven* accessors to verify the results of
        //      manipulators on tables that do have elements.
        //
        // Tactics:
        //   - Add-Hoc Data Selection Method
        //   - Brute-Force and Loop-Based implementation techniques
        //
        // Testing:
        //   bool isAnyInColumnNonNull(int columnIndex) const;
        //   bool isAnyInColumnNull(int columnIndex) const;
        //   bool isAnyNonNull() const;
        //   bool isAnyNull() const;
        //
        //   void makeColumnNull(int index);
        //   void makeRowsNull(int startIndex, int rowCount);
        //   void makeAllNull();
        // --------------------------------------------------------------------

        bslma_TestAllocator defaultAllocator;
        const bslma_DefaultAllocatorGuard dag(&defaultAllocator);

        if (verbose) cout << endl << "IS UNSET/MAKE NULL/IS NULL" << endl
                                  << "==========================" << endl;

        if (verbose) cout << "\nSet up reference lists." << endl;

        bdem_List mA; const bdem_List& A = mA; ASSERT(0         == A.length());
        loadReferenceA(&mA);                   ASSERT(NUM_ELEMS == A.length());

        bdem_List mU; const bdem_List& U = mU; ASSERT(0         == U.length());
        loadReferenceU(&mU);                   ASSERT(NUM_ELEMS == U.length());

        bdem_List mN; const bdem_List& N = mN; ASSERT(0         == N.length());
        loadReferenceN(&mN);                   ASSERT(NUM_ELEMS == N.length());

        if (veryVeryVerbose) {
            cout << "\tlist A: "; A.print(cout, -1, 8);
            cout << "\tlist U: "; U.print(cout, -1, 8);
            cout << "\tlist N: "; N.print(cout, -1, 8);
        }

        if (verbose) cout << "\nCreate controls for subject tables." << endl;

        if (veryVerbose) cout <<
                        "\tTable with 0 Rows and  0 Columns (Empty)." << endl;
        const bdem_Table mTT_R0_C0; const bdem_Table& TT_R0_C0 = mTT_R0_C0;
        if (veryVeryVerbose) { T_ T_ P(TT_R0_C0) }
        ASSERT(0 == TT_R0_C0.numRows());
        ASSERT(0 == TT_R0_C0.numColumns());

        if (veryVerbose) cout << "\tTable with 5 Rows and  0 Columns." << endl;
        bdem_Table mTT_R5_C0; const bdem_Table& TT_R5_C0 = mTT_R5_C0;
        bdem_Table mTT_R5_CN; const bdem_Table& TT_R5_CN = mTT_R5_CN;
        mTT_R5_C0.appendNullRow();
        mTT_R5_C0.appendNullRow();
        mTT_R5_C0.appendNullRow();
        mTT_R5_C0.appendNullRow();
        mTT_R5_C0.appendNullRow();

        mTT_R5_CN.appendNullRow();
        mTT_R5_CN.appendNullRow();
        mTT_R5_CN.appendNullRow();
        mTT_R5_CN.appendNullRow();
        mTT_R5_CN.appendNullRow();
        if (veryVeryVerbose) { T_ T_ P(TT_R5_C0) }
        ASSERT(5 == TT_R5_C0.numRows());
        ASSERT(0 == TT_R5_C0.numColumns());

        if (veryVerbose) cout << "\tTable with 0 Rows and 32 Columns." << endl;
        bdem_Table mTT_R0_C32 = g("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef");
        const bdem_Table& TT_R0_C32 = mTT_R0_C32;
        if (veryVeryVerbose) { T_ T_ P(TT_R0_C32) }
        ASSERT( 0        == TT_R0_C32.numRows());
        ASSERT(NUM_ELEMS == TT_R0_C32.numColumns());

        if (veryVerbose) cout <<
                "\tTable with 5 Rows and 32 Columns (NULL = N)." << endl;
        bdem_Table mTT_R5_C32_N = g("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef");
        const bdem_Table& TT_R5_C32_N = mTT_R5_C32_N;
        mTT_R5_C32_N.appendNullRow();
        mTT_R5_C32_N.appendNullRow();
        mTT_R5_C32_N.appendNullRow();
        mTT_R5_C32_N.appendNullRow();
        mTT_R5_C32_N.appendNullRow();
        if (veryVeryVerbose) { T_ T_ P(TT_R5_C32_N) }
        ASSERT( 5 == TT_R5_C32_N.numRows());
        ASSERT(NUM_ELEMS == TT_R5_C32_N.numColumns());

        if (veryVerbose) cout <<
                "\tTable with 5 Rows and 32 Columns   (SET = A)." << endl;
        bdem_Table mTT_R5_C32_A = mTT_R5_C32_N;
        const bdem_Table& TT_R5_C32_A = mTT_R5_C32_A;
        mTT_R5_C32_A.theModifiableRow(0).replaceValues(A.row());
        mTT_R5_C32_A.theModifiableRow(1).replaceValues(A.row());
        mTT_R5_C32_A.theModifiableRow(2).replaceValues(A.row());
        mTT_R5_C32_A.theModifiableRow(3).replaceValues(A.row());
        mTT_R5_C32_A.theModifiableRow(4).replaceValues(A.row());
        if (veryVeryVerbose) { T_ T_ P(TT_R5_C32_A) }
        ASSERT( 5 == TT_R5_C32_A.numRows());
        ASSERT(NUM_ELEMS == TT_R5_C32_A.numColumns());

        if (verbose) cout <<
                        "\nTable with 0 Rows and  0 Columns (Empty)." << endl;
        {
             bdem_Table TT = TT_R0_C0;
             bdem_Table mT = TT; const bdem_Table& T = mT;
             if (veryVeryVerbose) { T_ P(T) }
             ASSERT(0 == T.numRows());
             ASSERT(0 == T.numColumns());

             if (veryVerbose) cout << "\tisAnyNonNull" << endl;
             ASSERT(0 == T.isAnyNonNull());

             if (veryVerbose) cout << "\tisAnyNull" << endl;
             ASSERT(0 == T.isAnyNull());

             if (veryVerbose) cout << "\tisAnyInColumnNonNull" << endl;
             {
                 for (int c = 0; c < TT.numColumns(); ++c) {
                     if (veryVeryVerbose) { T_ T_ P(c) }
                     LOOP_ASSERT(c, 0 == T.isAnyInColumnNonNull(c));
                 }
             }

             if (veryVerbose) cout << "\tisAnyInColumnNull" << endl;
             {
                 for (int c = 0; c < TT.numColumns(); ++c) {
                     if (veryVeryVerbose) { T_ T_ P(c) }
                     LOOP_ASSERT(c, 0 == T.isAnyInColumnNull(c));
                 }
             }

             if (veryVerbose) cout << "\tmakeAllNull" << endl;
             mT.makeAllNull();

             if (veryVerbose) cout << "\tmakeColumnNull" << endl;
             {
                 for (int c = 0; c < TT.numColumns(); ++c) {
                     if (veryVeryVerbose) { T_ T_ P(c) }
                     mT.makeColumnNull(c);
                 }
             }

             if (veryVerbose) cout << "\tmakeRowsNull" << endl;
             {
                 for (int r = 0; r <= TT.numRows(); ++r) {
                     if (veryVeryVerbose) { T_ T_ P(r) }
                     for (int n = 0; n + r <= TT.numRows(); ++n) {
                         if (veryVeryVerbose) { T_ T_ T_ P(n) }
                         mT.makeRowsNull(r, n);
                     }
                 }
             }

             ASSERT(TT_R0_C0 == T); // Ensure no change to subject table.

             if (veryVerbose) cout << "\tisAnyNull" << endl;
             ASSERT(0 == T.isAnyNull());

             if (veryVerbose) cout << "\tisAnyNonNull" << endl;
             ASSERT(0 == T.isAnyNonNull());

             if (veryVerbose) cout << "\tisAnyInColumnNull" << endl;
             {
                 for (int c = 0; c < TT.numColumns(); ++c) {
                     if (veryVeryVerbose) { T_ T_ P(c) }
                     LOOP_ASSERT(c, 0 == T.isAnyInColumnNull(c));
                 }
             }

             if (veryVerbose) cout << "\tisAnyInColumnNonNull" << endl;
             {
                 for (int c = 0; c < TT.numColumns(); ++c) {
                     if (veryVeryVerbose) { T_ T_ P(c) }
                     LOOP_ASSERT(c, 0 == T.isAnyInColumnNonNull(c));
                 }
             }

             if (veryVerbose) cout << "\tmakeAllNull" << endl;
             mT.makeAllNull();

             if (veryVerbose) cout << "\tmakeColumnNull" << endl;
             {
                 for (int c = 0; c < TT.numColumns(); ++c) {
                     if (veryVeryVerbose) { T_ T_ P(c) }
                     mT.makeColumnNull(c);
                 }
             }

             if (veryVerbose) cout << "\tmakeRowsNull" << endl;
             {
                 for (int r = 0; r <= TT.numRows(); ++r) {
                     if (veryVeryVerbose) { T_ T_ P(r) }
                     for (int n = 0; n + r <= TT.numRows(); ++n) {
                         if (veryVeryVerbose) { T_ T_ T_ P(n) }
                         mT.makeRowsNull(r, n);
                     }
                 }
             }

             ASSERT(TT_R0_C0 == T); // Ensure no change to subject table.
        }

        if (verbose) cout << "\nTable with 5 Rows and  0 Columns." << endl;
        {
             bdem_Table TT = TT_R5_C0;
             bdem_Table mT = TT; const bdem_Table& T = mT;
             if (veryVeryVerbose) { T_ P(T) }
             ASSERT(5 == T.numRows());
             ASSERT(0 == T.numColumns());

             if (veryVerbose) cout << "\tisAnyNonNull" << endl;
             ASSERT(0 == T.isAnyNonNull());

             if (veryVerbose) cout << "\tisAnyNull" << endl;
             ASSERT(0 == T.isAnyNull());

             if (veryVerbose) cout << "\tisAnyInColumnNonNull" << endl;
             {
                 for (int c = 0; c < TT.numColumns(); ++c) {
                     if (veryVeryVerbose) { T_ T_ P(c) }
                     LOOP_ASSERT(c, 0 == T.isAnyInColumnNonNull(c));
                 }
             }

             if (veryVerbose) cout << "\tisAnyInColumnNull" << endl;
             {
                 for (int c = 0; c < TT.numColumns(); ++c) {
                     if (veryVeryVerbose) { T_ T_ P(c) }
                     LOOP_ASSERT(c, 0 == T.isAnyInColumnNull(c));
                 }
             }

             if (veryVerbose) cout << "\tmakeAllNull" << endl;
             mT.makeAllNull();

             if (veryVerbose) cout << "\tmakeColumnNull" << endl;
             {
                 for (int c = 0; c < TT.numColumns(); ++c) {
                     if (veryVeryVerbose) { T_ T_ P(c) }
                     mT.makeColumnNull(c);
                 }
             }

             if (veryVerbose) cout << "\tmakeRowsNull" << endl;
             {
                 for (int r = 0; r <= TT.numRows(); ++r) {
                     if (veryVeryVerbose) { T_ T_ P(r) }
                     for (int n = 0; n + r <= TT.numRows(); ++n) {
                         if (veryVeryVerbose) { T_ T_ T_ P(n) }
                         mT.makeRowsNull(r, n);
                     }
                 }
             }

             ASSERT(TT_R5_C0 == T); // Ensure no change to subject table.

             if (veryVerbose) cout << "\tisAnyNonNull" << endl;
             ASSERT(0 == T.isAnyNonNull());

             if (veryVerbose) cout << "\tisAnyNull" << endl;
             ASSERT(0 == T.isAnyNull());

             if (veryVerbose) cout << "\tisAnyInColumnNonNull" << endl;
             {
                 for (int c = 0; c < TT.numColumns(); ++c) {
                     if (veryVeryVerbose) { T_ T_ P(c) }
                     LOOP_ASSERT(c, 0 == T.isAnyInColumnNonNull(c));
                 }
             }

             if (veryVerbose) cout << "\tisAnyInColumnNull" << endl;
             {
                 for (int c = 0; c < TT.numColumns(); ++c) {
                     if (veryVeryVerbose) { T_ T_ P(c) }
                     LOOP_ASSERT(c, 0 == T.isAnyInColumnNull(c));
                 }
             }

             if (veryVerbose) cout << "\tmakeAllNull" << endl;
             mT.makeAllNull();

             if (veryVerbose) cout << "\tmakeColumnNull" << endl;
             {
                 for (int c = 0; c < TT.numColumns(); ++c) {
                     if (veryVeryVerbose) { T_ T_ P(c) }
                     mT.makeColumnNull(c);
                 }
             }

             if (veryVerbose) cout << "\tmakeRowsNull" << endl;
             {
                 for (int r = 0; r <= TT.numRows(); ++r) {
                     if (veryVeryVerbose) { T_ T_ P(r) }
                     for (int n = 0; n + r <= TT.numRows(); ++n) {
                         if (veryVeryVerbose) { T_ T_ T_ P(n) }
                         mT.makeRowsNull(r, n);
                     }
                 }
             }

             ASSERT(TT_R5_CN == T); // Ensure no change to subject table.
        }

        if (verbose) cout << "\nTable with 0 Rows and 32 Columns." << endl;
        {
             bdem_Table TT = TT_R0_C32;
             bdem_Table mT = TT; const bdem_Table& T = mT;
             if (veryVeryVerbose) { T_ P(T) }
             ASSERT( 0 == T.numRows());
             ASSERT(NUM_ELEMS == T.numColumns());

             if (veryVerbose) cout << "\tisAnyNonNull" << endl;
             ASSERT(0 == T.isAnyNonNull());

             if (veryVerbose) cout << "\tisAnyNull" << endl;
             ASSERT(0 == T.isAnyNull());

             if (veryVerbose) cout << "\tisAnyInColumnNonNull" << endl;
             {
                 for (int c = 0; c < TT.numColumns(); ++c) {
                     if (veryVeryVerbose) { T_ T_ P(c) }
                     LOOP_ASSERT(c, 0 == T.isAnyInColumnNonNull(c));
                 }
             }

             if (veryVerbose) cout << "\tisAnyInColumnNull" << endl;
             {
                 for (int c = 0; c < TT.numColumns(); ++c) {
                     if (veryVeryVerbose) { T_ T_ P(c) }
                     LOOP_ASSERT(c, 0 == T.isAnyInColumnNull(c));
                 }
             }

             if (veryVerbose) cout << "\tmakeAllNull" << endl;
             mT.makeAllNull();

             if (veryVerbose) cout << "\tmakeColumnNull" << endl;
             {
                 for (int c = 0; c < TT.numColumns(); ++c) {
                     if (veryVeryVerbose) { T_ T_ P(c) }
                     mT.makeColumnNull(c);
                 }
             }

             if (veryVerbose) cout << "\tmakeRowsNull" << endl;
             {
                 for (int r = 0; r <= TT.numRows(); ++r) {
                     if (veryVeryVerbose) { T_ T_ P(r) }
                     for (int n = 0; n + r <= TT.numRows(); ++n) {
                         if (veryVeryVerbose) { T_ T_ T_ P(n) }
                         mT.makeRowsNull(r, n);
                     }
                 }
             }

             ASSERT(TT_R0_C32 == T); // Ensure no change to subject table.

             if (veryVerbose) cout << "\tisAnyNonNull" << endl;
             ASSERT(0 == T.isAnyNonNull());

             if (veryVerbose) cout << "\tisAnyNull" << endl;
             ASSERT(0 == T.isAnyNull());

             if (veryVerbose) cout << "\tisAnyInColumnNonNull" << endl;
             {
                 for (int c = 0; c < TT.numColumns(); ++c) {
                     if (veryVeryVerbose) { T_ T_ P(c) }
                     LOOP_ASSERT(c, 0 == T.isAnyInColumnNonNull(c));
                 }
             }

             if (veryVerbose) cout << "\tisAnyInColumnNull" << endl;
             {
                 for (int c = 0; c < TT.numColumns(); ++c) {
                     if (veryVeryVerbose) { T_ T_ P(c) }
                     LOOP_ASSERT(c, 0 == T.isAnyInColumnNull(c));
                 }
             }

             if (veryVerbose) cout << "\tmakeAllNull" << endl;
             mT.makeAllNull();

             if (veryVerbose) cout << "\tmakeColumnNull" << endl;
             {
                 for (int c = 0; c < TT.numColumns(); ++c) {
                     if (veryVeryVerbose) { T_ T_ P(c) }
                     mT.makeColumnNull(c);
                 }
             }

             if (veryVerbose) cout << "\tmakeRowsNull" << endl;
             {
                 for (int r = 0; r <= TT.numRows(); ++r) {
                     if (veryVeryVerbose) { T_ T_ P(r) }
                     for (int n = 0; n + r <= TT.numRows(); ++n) {
                         if (veryVeryVerbose) { T_ T_ T_ P(n) }
                         mT.makeRowsNull(r, n);
                     }
                 }
             }

             ASSERT(TT_R0_C32 == T); // Ensure no change to subject table.
        }

        if (verbose) cout <<
                     "\nTable with 5 Rows and 32 Columns (UNSET = U)." << endl;
        {
             bdem_Table TT = TT_R5_C32_N;
             bdem_Table mT = TT; const bdem_Table& T = mT;
             if (veryVeryVerbose) { T_ P(T) }
             ASSERT( 5 == T.numRows());
             ASSERT(NUM_ELEMS == T.numColumns());

             if (veryVerbose) cout << "\tisAnyNonNull" << endl;
             ASSERT(0 == T.isAnyNonNull());

             if (veryVerbose) cout << "\tisAnyNull" << endl;
             ASSERT(1 == T.isAnyNull());

             if (veryVerbose) cout << "\tisAnyInColumnNonNull" << endl;
             {
                 for (int c = 0; c < TT.numColumns(); ++c) {
                     if (veryVeryVerbose) { T_ T_ P(c) }
                     LOOP_ASSERT(c, 0 == T.isAnyInColumnNonNull(c));
                 }
             }

             if (veryVerbose) cout << "\tisAnyInColumnNull" << endl;
             {
                 for (int c = 0; c < TT.numColumns(); ++c) {
                     if (veryVeryVerbose) { T_ T_ P(c) }
                     LOOP_ASSERT(c, 1 == T.isAnyInColumnNull(c));
                 }
             }

             if (veryVerbose) cout << "\tmakeAllNull" << endl;
             mT.makeAllNull();

             if (veryVerbose) cout << "\tmakeColumnNull" << endl;
             {
                 for (int c = 0; c < TT.numColumns(); ++c) {
                     if (veryVeryVerbose) { T_ T_ P(c) }
                     mT.makeColumnNull(c);
                 }
             }

             if (veryVerbose) cout << "\tmakeRowsNull" << endl;
             {
                 for (int r = 0; r <= TT.numRows(); ++r) {
                     if (veryVeryVerbose) { T_ T_ P(r) }
                     for (int n = 0; n + r <= TT.numRows(); ++n) {
                         if (veryVeryVerbose) { T_ T_ T_ P(n) }
                         mT.makeRowsNull(r, n);
                     }
                 }
             }

             ASSERT(TT_R5_C32_N == T); // Ensure no change to subject table.

             if (veryVerbose) cout <<
                                    "\tSet each element individually." << endl;
             for (int r = 0; r < TT.numRows(); ++r) {
                 if (veryVeryVerbose) { T_ T_ P(r) }
                 for (int c = 0; c < TT.numColumns(); ++c) {
                     if (veryVeryVerbose) { T_ T_ T_ P(c) }

                     LOOP2_ASSERT(r, c, 0 == T.isAnyNonNull());
                     LOOP2_ASSERT(r, c, 1 == T.isAnyNull());
                     LOOP2_ASSERT(r, c, 0 == T.isAnyInColumnNonNull(c));
                     LOOP2_ASSERT(r, c, 1 == T.isAnyInColumnNull(c));

                     mT.theModifiableRow(r)[c].replaceValue(A[c]);

                     LOOP2_ASSERT(r, c, 1 == T.isAnyNonNull());
                     LOOP2_ASSERT(r, c, 1 == T.isAnyNull());
                     for (int i = 0; i < TT.numColumns(); ++i) {
                         const bool Z = c == i;
                         if (veryVeryVerbose) { T_ T_ T_ T_ P_(i) P(Z) }
                         LOOP3_ASSERT(r, c, i, Z == T.isAnyInColumnNonNull(i));
                     }
                     LOOP2_ASSERT(r, c, 1 == T.isAnyInColumnNull(c));

                     mT.theModifiableRow(r)[c].replaceValue(N[c]);
                }
            }
        }

        if (verbose) cout <<
                     "\nTable with 5 Rows and 32 Columns (NULL = N)." << endl;
        {
             bdem_Table TT = TT_R5_C32_N;
             bdem_Table mT = TT; const bdem_Table& T = mT;
             if (veryVeryVerbose) { T_ P(T) }
             ASSERT( 5 == T.numRows());
             ASSERT(NUM_ELEMS == T.numColumns());

             if (veryVerbose) cout << "\tisAnyNonNull" << endl;
             ASSERT(0 == T.isAnyNonNull());

             if (veryVerbose) cout << "\tisAnyNull" << endl;
             ASSERT(1 == T.isAnyNull());

             if (veryVerbose) cout << "\tisAnyInColumnNonNull" << endl;
             {
                 for (int c = 0; c < TT.numColumns(); ++c) {
                     if (veryVeryVerbose) { T_ T_ P(c) }
                     LOOP_ASSERT(c, 0 == T.isAnyInColumnNonNull(c));
                 }
             }

             if (veryVerbose) cout << "\tisAnyInColumnNull" << endl;
             {
                 for (int c = 0; c < TT.numColumns(); ++c) {
                     if (veryVeryVerbose) { T_ T_ P(c) }
                     LOOP_ASSERT(c, 1 == T.isAnyInColumnNull(c));
                 }
             }

             if (veryVerbose) cout << "\tmakeAllNull" << endl;
             mT.makeAllNull();

             if (veryVerbose) cout << "\tmakeColumnNull" << endl;
             {
                 for (int c = 0; c < TT.numColumns(); ++c) {
                     if (veryVeryVerbose) { T_ T_ P(c) }
                     mT.makeColumnNull(c);
                 }
             }

             if (veryVerbose) cout << "\tmakeRowsNull" << endl;
             {
                 for (int r = 0; r <= TT.numRows(); ++r) {
                     if (veryVeryVerbose) { T_ T_ P(r) }
                     for (int n = 0; n + r <= TT.numRows(); ++n) {
                         if (veryVeryVerbose) { T_ T_ T_ P(n) }
                         mT.makeRowsNull(r, n);
                     }
                 }
             }

             ASSERT(TT_R5_C32_N == T); // Ensure no change to subject table.

             if (veryVerbose)
                 cout << "\tNonNull each element individually." << endl;
             for (int r = 0; r < TT.numRows(); ++r) {
                 if (veryVeryVerbose) { T_ T_ P(r) }
                 for (int c = 0; c < TT.numColumns(); ++c) {
                     if (veryVeryVerbose) { T_ T_ T_ P(c) }

                     LOOP2_ASSERT(r, c, 0 == T.isAnyNonNull());
                     LOOP2_ASSERT(r, c, 1 == T.isAnyNull());
                     LOOP2_ASSERT(r, c, 0 == T.isAnyInColumnNonNull(c));
                     LOOP2_ASSERT(r, c, 1 == T.isAnyInColumnNull(c));

                     mT.theModifiableRow(r)[c].replaceValue(A[c]);

                     LOOP2_ASSERT(r, c, 1 == T.isAnyNonNull());
                     LOOP2_ASSERT(r, c, 1 == T.isAnyNull());
                     for (int i = 0; i < TT.numColumns(); ++i) {
                         const bool Z = c == i;
                         if (veryVeryVerbose) { T_ T_ T_ T_ P_(i) P(Z) }
                         LOOP3_ASSERT(r, c, i, Z == T.isAnyInColumnNonNull(i));
                     }
                     LOOP2_ASSERT(r, c, 1 == T.isAnyInColumnNull(c));

                     mT.theModifiableRow(r)[c].replaceValue(N[c]);

                     ASSERT(TT_R5_C32_N[r] == T[r]);
                     mT.makeRowsNull(r, 1);
                }
            }
        }

        if (verbose) cout <<
                     "\nTable with 5 Rows and 32 Columns   (SET = A)." << endl;
        {
             bdem_Table TT = TT_R5_C32_A;
             bdem_Table mTX = TT; const bdem_Table& TX = mTX;
             if (veryVeryVerbose) { T_ P(TX) }
             ASSERT( 5 == TX.numRows());
             ASSERT(NUM_ELEMS == TX.numColumns());

             if (veryVerbose) cout << "\tisAnyNonNull" << endl;
             ASSERT(1 == TX.isAnyNonNull());

             if (veryVerbose) cout << "\tisAnyNull" << endl;
             ASSERT(0 == TX.isAnyNull());

             if (veryVerbose) cout << "\tisAnyInColumnNonNull" << endl;
             {
                 for (int c = 0; c < TT.numColumns(); ++c) {
                     if (veryVeryVerbose) { T_ T_ P(c) }
                     LOOP_ASSERT(c, 1 == TX.isAnyInColumnNonNull(c));
                 }
             }

             if (veryVerbose) cout << "\tisAnyInColumnNull" << endl;
             {
                 for (int c = 0; c < TT.numColumns(); ++c) {
                     if (veryVeryVerbose) { T_ T_ P(c) }
                     LOOP_ASSERT(c, 0 == TX.isAnyInColumnNull(c));
                 }
             }

             if (veryVerbose) cout << "\tmakeAllNull" << endl;
             {
                 bdem_Table mT = TT; const bdem_Table& T = mT;
                 mT.makeAllNull();
                 ASSERT(TT_R5_C32_N == T);
             }

             if (veryVerbose) cout << "\tmakeColumnNull" << endl;
             {
                 bdem_Table mT = TT; const bdem_Table& T = mT;
                 for (int c = 0; c < TT.numColumns(); ++c) {
                     if (veryVeryVerbose) { T_ T_ P(c) }
                     mT.makeColumnNull(c);
                     LOOP_ASSERT(c, T.isAnyInColumnNull(c));
                     LOOP_ASSERT(c, !T.isAnyInColumnNonNull(c));
                 }
             }

             if (veryVerbose) cout << "\tmakeRowsNull" << endl;
             {
                 for (int r = 0; r <= TT.numRows(); ++r) {
                     bdem_Table mT = TT; const bdem_Table& T = mT;
                     if (veryVeryVerbose) { T_ T_ P(r) }
                     for (int n = 0; n + r <= TT.numRows(); ++n) {
                         if (veryVeryVerbose) { T_ T_ T_ P(n) }
                         mT.makeRowsNull(r, n);
                     }
                     LOOP_ASSERT(r, !r == (TT_R5_C32_N == T));
                 }
             }

             ASSERT(TT_R5_C32_A == TX); // Ensure no change to subject table.

             if (veryVerbose) cout <<
                                  "\tNull each element individually." << endl;
             for (int r = 0; r < TT.numRows(); ++r) {
                 if (veryVeryVerbose) { T_ T_ P(r) }
                 for (int c = 0; c < TT.numColumns(); ++c) {
                     if (veryVeryVerbose) { T_ T_ T_ P(c) }

                     LOOP2_ASSERT(r, c, 1 == TX.isAnyNonNull());
                     LOOP2_ASSERT(r, c, 0 == TX.isAnyNull());
                     LOOP2_ASSERT(r, c, 1 == TX.isAnyInColumnNonNull(c));
                     LOOP2_ASSERT(r, c, 0 == TX.isAnyInColumnNull(c));

                     mTX.theModifiableRow(r)[c].replaceValue(N[c]);

                     LOOP2_ASSERT(r, c, 1 == TX.isAnyNonNull());
                     LOOP2_ASSERT(r, c, 1 == TX.isAnyNull());
                     LOOP2_ASSERT(r, c, 1 == TX.isAnyInColumnNonNull(c));
                     for (int i = 0; i < TT.numColumns(); ++i) {
                         const bool Z = c == i;
                         if (veryVeryVerbose) { T_ T_ T_ T_ P_(i) P(Z) }
                         LOOP3_ASSERT(r, c, i, Z == TX.isAnyInColumnNull(i));
                     }

                     mTX.theModifiableRow(r)[c].replaceValue(A[c]);

                     LOOP2_ASSERT(r, c, TT_R5_C32_A == TX);
                 }
             }

             if (veryVerbose) cout << "\tNull each column." << endl;
             {
                 for (int c = 0; c < TT.numColumns(); ++c) {
                     if (veryVeryVerbose) { T_ T_ P(c) }
                     bdem_Table mT = TT; const bdem_Table& T = mT;
                     mT.makeColumnNull(c);
                     LOOP_ASSERT(c, isSameColumnValue(T, c, N[c]));
                     LOOP_ASSERT(c, isSameExceptColumn(T, c, A.row()));
                 }
             }

             if (veryVerbose) cout << "\tNull each subrange of rows." << endl;
             {
                 for (int r = 0; r <= TT.numRows(); ++r) {
                     if (veryVeryVerbose) { T_ T_ P(r) }
                     for (int n = 0; n + r <= TT.numRows(); ++n) {
                         if (veryVeryVerbose) { T_ T_ T_ P(n) }
                         bdem_Table mT = TT; const bdem_Table& T = mT;
                         mT.makeRowsNull(r, n);
                         for (int i = 0; i < TT.numRows(); ++i) {
                             const bool Z = i >= r && i < r + n;
                             if (veryVeryVerbose) { T_ T_ T_ T_ P_(i) P(Z) }
                             LOOP3_ASSERT(r, n, i,
                                          Z == T.theRow(i).isAnyNull());
                         } // each inspected row, i
                     } // each number of elements, n
                 } // each row, r
             } // Block scope: checking each unset subrange of rows.
        } // 5 rows, 32 columns, values of type A.

      }

DEFINE_TEST_CASE(14) {
        // --------------------------------------------------------------------
        // SET COLUMN VALUES
        //   Ability to set entire column to a given value efficiently.
        //
        // Concerns:
        //   - That when applied to a table with no rows it has no effect.
        //   - That all elements in the correct are set to the correct value.
        //   - That no other elements are affected.
        //   - That all 3 user allocation modes work the same.
        //   - That the default allocator is not used.
        //
        // Plan:
        //   - Create 32-element reference lists.
        //   - Test isSameColumnValue helper function.
        //   - Test isSameExceptColumn helper function.
        //   - Create a reference table with 32 elements.
        //   - Install a local default test allocator.
        //   - Copy construct this table 3 times with separate test allocators
        //     and different allocation modes: 'BDEM_PASS_THROUGH',
        //     'BDEM_WRITE_ONCE', and 'BDEM_WRITE_MANY'.
        //   - Create a C array of table pointers to each of these 3 tables.
        //   - for each table
        //      + Set each column's values and verify that the tables still
        //         have the same value as the reference table.
        //      + Append an unset rows; set each column in turn; verify
        //         that all other columns are unchanged; reset the column
        //         and compare with the control.
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //   - Brute-Force and Loop-Based Implementation Techniques
        //
        // Testing:
        //   bool isSameColumnValue(table, columnIndex, value)
        //   bool isSameExceptColumn(table, columnIndex, values)
        //
        //   void setColumnBool(int colIdx, bool value);
        //   void setColumnChar(int colIdx, char value);
        //   void setColumnShort(int colIdx, short value);
        //   void setColumnInt(int colIdx, int value);
        //   void setColumnInt64(int colIdx, bsls_Types::Int64 value);
        //   void setColumnFloat(int colIdx, float value);
        //   void setColumnDouble(int colIdx, double value);
        //   void setColumnString(int colIdx, const char *value);
        //   void setColumnString(int colIdx, const bsl::string& value);
        //   void setColumnDatetime(int colIdx, const bdet_Datetime& value);
        //   void setColumnDatetimeTz(int   colIdx,
        //                            const bdet_DatetimeTz& value);
        //   void setColumnDate(int colIdx, const bdet_Date& value);
        //   void setColumnDateTz(int colIdx, const bdet_DateTz& value);
        //   void setColumnTime(int colIdx, const bdet_Time& value);
        //   void setColumnTimeTz(int colIdx, const bdet_TimeTz& value);
        //   void setColumnBoolArray(int colIdx, const bsl::vector<bool>& val);
        //   void setColumnCharArray(int colIdx, const bsl::vector<char>& val);
        //   void setColumnShortArray(int colIdx, const bsl::vector<short>& v);
        //   void setColumnIntArray(int colIdx, const bsl::vector<int>& value);
        //   void setColumnInt64Array(int ci, const bsl::vector<Int64>& value);
        //   void setColumnFloatArray(int ci, const bsl::vector<float>& v);
        //   void setColumnDoubleArray(int ci, const bsl::vector<double>& val);
        //   void setColumnStringArray(int, const bsl::vector<bsl::string>& v);
        //   void setColumnDatetimeArray(int ci, const vector<bdet_Datetime>&);
        //   void setColumnDatetimeTzArray(int   ci,
        //                                 const vector<bdet_DatetimeTz>&);
        //   void setColumnDateArray(int ci, const bsl::vector<bdet_Date>& v);
        //   void setColumnDateTzArray(int   ci,
        //                             const bsl::vector<bdet_DateTz>& v);
        //   void setColumnTimeArray(int ci, const bsl::vector<bdet_Time>& v);
        //   void setColumnTimeTzArray(int   ci,
        //                             const bsl::vector<bdet_TimeTz>& v);
        //   void setColumnChoice(int colIdx, const bdem_Choice& value);
        //   void setColumnList(int colIdx, const bdem_List& value);
        //   void setColumnList(int colIdx, const bdem_Row& value);
        //   void setColumnTable(int colIdx, const bdem_Table& value);
        //
        //   void setColumnValue(int colIdx, const bdem_ConstElemRef& value);
        // --------------------------------------------------------------------

        DECLARE_MAIN_VARIABLES

        bslma_TestAllocator defaultAllocator;
        const bslma_DefaultAllocatorGuard dag(&defaultAllocator);

        if (verbose) cout << endl
                          << "SET COLUMN VALUES" << endl
                          << "=================" << endl;

        if (verbose) cout << "\nCreate general reference lists." << endl;

        bdem_List mA; const bdem_List& A = mA; ASSERT(0         == A.length());
        loadReferenceA(&mA);                   ASSERT(NUM_ELEMS == A.length());
        bdem_List mB; const bdem_List& B = mB; ASSERT(0         == B.length());
        loadReferenceB(&mB);                   ASSERT(NUM_ELEMS == B.length());
        bdem_List mC; const bdem_List& C = mC; ASSERT(0         == C.length());
        loadReferenceC(&mC);                   ASSERT(NUM_ELEMS == C.length());
        bdem_List mD; const bdem_List& D = mD; ASSERT(0         == D.length());
        loadReferenceD(&mD);                   ASSERT(NUM_ELEMS == D.length());
        bdem_List mE; const bdem_List& E = mE; ASSERT(0         == E.length());
        loadReferenceE(&mE);                   ASSERT(NUM_ELEMS == E.length());
        bdem_List mN; const bdem_List& N = mN; ASSERT(0         == N.length());
        loadReferenceU(&mN);                   ASSERT(NUM_ELEMS == N.length());
        bdem_List mNN; const bdem_List& NN = mNN;
                                              ASSERT(0         == NN.length());
        loadReferenceN(&mNN);                 ASSERT(NUM_ELEMS == NN.length());

        if (verbose) cout << "\nTest is isSameColumnValue helper." << endl;
        {
            if (veryVerbose) cout << "\tCreate table." << endl;
            const int M = NUM_ELEMS; // length of reference lists and columns
                                     // of table
                  bdem_Table  mT = g("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef");
            const bdem_Table&  T = mT;
            int i;
            for (i = 0; i < M; ++i) {       if (veryVeryVerbose) { T_ T_ P(i) }
                    LOOP_ASSERT(i, 1 == isSameColumnValue(T, i, A[i]));
                    LOOP_ASSERT(i, 1 == isSameColumnValue(T, i, N[i]));
            }

            if (veryVerbose) cout << "\tAppend a null row." << endl;
            mT.appendNullRow();
            for (i = 0; i < M; ++i) {       if (veryVeryVerbose) { T_ T_ P(i) }
                LOOP_ASSERT(i, 0 == isSameColumnValue(T, i, A[i]));
                LOOP_ASSERT(i, 1 == isSameColumnValue(T, i, NN[i]));
            }

            if (veryVerbose) cout << "\tAppend another null row." << endl;
            mT.appendNullRow();
            for (i = 0; i < M; ++i) {       if (veryVeryVerbose) { T_ T_ P(i) }
                LOOP_ASSERT(i, 0 == isSameColumnValue(T, i, A[i]));
                LOOP_ASSERT(i, 1 == isSameColumnValue(T, i, NN[i]));
            }

            if (veryVerbose) cout <<
                                "\tMake this new row have 'A' values." << endl;
            mT.theModifiableRow(1).replaceValues(A.row());
            for (i = 0; i < M; ++i) {       if (veryVeryVerbose) { T_ T_ P(i) }
                LOOP_ASSERT(i, 0 == isSameColumnValue(T, i, A[i]));
                LOOP_ASSERT(i, 0 == isSameColumnValue(T, i, N[i]));
            }

            if (veryVerbose) cout << "\tAppend yet another null row." << endl;
            mT.appendNullRow();
            for (i = 0; i < M; ++i) {       if (veryVeryVerbose) { T_ T_ P(i) }
                LOOP_ASSERT(i, 0 == isSameColumnValue(T, i, A[i]));
                LOOP_ASSERT(i, 0 == isSameColumnValue(T, i, NN[i]));
            }

            if (veryVerbose) cout <<
                                "\tMake this new row have 'A' values." << endl;
            mT.theModifiableRow(2).replaceValues(A.row());
            for (i = 0; i < M; ++i) {       if (veryVeryVerbose) { T_ T_ P(i) }
                LOOP_ASSERT(i, 0 == isSameColumnValue(T, i, A[i]));
                LOOP_ASSERT(i, 0 == isSameColumnValue(T, i, N[i]));
            }

            if (veryVerbose) cout << "\tMake row 0 have 'A' values." << endl;
            mT.theModifiableRow(0).replaceValues(A.row());
            for (i = 0; i < M; ++i) {       if (veryVeryVerbose) { T_ T_ P(i) }
                LOOP_ASSERT(i, 1 == isSameColumnValue(T, i, A[i]));
                LOOP_ASSERT(i, 0 == isSameColumnValue(T, i, N[i]));
            }

            if (veryVerbose) cout << "\tAppend yet another null row." << endl;
            mT.appendNullRow();
            for (i = 0; i < M; ++i) {       if (veryVeryVerbose) { T_ T_ P(i) }
                LOOP_ASSERT(i, 0 == isSameColumnValue(T, i, A[i]));
                LOOP_ASSERT(i, 0 == isSameColumnValue(T, i, NN[i]));
            }
        }

        if (verbose) cout << "\nTest is isSameExceptColumn helper."<<endl;
        {
            if (veryVerbose) cout << "\tCreate table." << endl;
            const int          M = 32; // length of reference lists and test
            bdem_Table        mT = g("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef");
            const bdem_Table&  T = mT;
            int i;
            for (i = 0; i < M; ++i) {       if (veryVeryVerbose) { T_ T_ P(i) }
                LOOP_ASSERT(i, 1 == isSameExceptColumn(T, i, A.row()));
                LOOP_ASSERT(i, 1 == isSameExceptColumn(T, i, N.row()));
            }

            if (veryVerbose) cout << "\tAppend a null row." << endl;
            mT.appendNullRow();
            for (i = 0; i < M; ++i) {       if (veryVeryVerbose) { T_ T_ P(i) }
                LOOP_ASSERT(i, 0 == isSameExceptColumn(T, i, A.row()));
                LOOP_ASSERT(i, 1 == isSameExceptColumn(T, i, NN.row()));
            }

            if (veryVerbose) cout <<
               "\tMake T.theModifiableRow(0)[0] of have an 'A' value." << endl;
            mT.theModifiableRow(0)[0].theModifiableChar() = A00;
            for (i = 1; i < M; ++i) {       if (veryVeryVerbose) { T_ T_ P(i) }
                LOOP_ASSERT(i, 0 == isSameExceptColumn(T, i, A.row()));
                LOOP_ASSERT(i, 0 == isSameExceptColumn(T, i, NN.row()));
            }
            ASSERT(0 == isSameExceptColumn(T, 0, A.row()));
            ASSERT(1 == isSameExceptColumn(T, 0, NN.row()));

            if (veryVerbose) cout << "\tMake T.theModifiableRow(0)[0] of "
                                  << "again have an 'N' value." << endl;
            mT.theModifiableRow(0)[0].theModifiableChar() = U00;

            if (veryVerbose) cout << "\tAppend another null row." << endl;

            mT.appendNullRow();
            for (i = 1; i < M; ++i) {       if (veryVeryVerbose) { T_ T_ P(i) }
                LOOP_ASSERT(i, 0 == isSameExceptColumn(T, i, A.row()));
                LOOP_ASSERT(i, 0 == isSameExceptColumn(T, i, NN.row()));
            }
            ASSERT(0 == isSameExceptColumn(T, 0, A.row()));
            ASSERT(1 == isSameExceptColumn(T, 0, NN.row()));

            if (veryVerbose) cout << "\tMake T.theModifiableRow(1)[21] "
                                  << "have an 'A' value." << endl;
            mT.theModifiableRow(1)[21].theModifiableTable() = A21;
            for (i = 0; i < M - 1; ++i) {      if (veryVeryVerbose) { T_ P(i) }
                LOOP_ASSERT(i, 0 == isSameExceptColumn(T, i, A.row()));
                if (i != 21) {
                    LOOP_ASSERT(i, 0 == isSameExceptColumn(T, i, N.row()));
                }
            }
            ASSERT(0 == isSameExceptColumn(T, 21, A.row()));
            ASSERT(0 == isSameExceptColumn(T, 21, NN.row()));
        }

        if (verbose) cout << "\nCreate Setup for setColumn testing." << endl;

        if (verbose) cout << "\tCreate elem-type array for reference table."
                                                                       << endl;
        vector<bdem_ElemType::Type> t32;
        {
            for (int i = 0; i < NUM_ELEMS; ++i) {
                t32.push_back(bdem_ElemType::Type(i));
            }
        }

        if (verbose) cout << "\tCreate reference table." << endl;

        bdem_Table mTT(&t32.front(), t32.size()); const bdem_Table& TT = mTT;
        ASSERT(0 == TT.numRows()); ASSERT(NUM_ELEMS == TT.numColumns());

        if (verbose) cout << "\tInstall test allocator as default." << endl;

        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag2(&da);

        if (verbose) cout <<
          "\tCreate 3 subject tables with different allocation modes." << endl;

        bslma_TestAllocator a[3];
        const bdem_AggregateOption::AllocationStrategy S[] = {
            bdem_AggregateOption::BDEM_PASS_THROUGH,
            bdem_AggregateOption::BDEM_WRITE_ONCE,
            bdem_AggregateOption::BDEM_WRITE_MANY
        };

        bdem_Table mT0(TT, S[0], &a[0]);
        bdem_Table mT1(TT, S[1], &a[1]);
        bdem_Table mT2(TT, S[2], &a[2]);

        bdem_Table *const TP[] = { &mT0, &mT1, &mT2 };
        const int NUM_TABLES = sizeof TP / sizeof *TP;

        if (verbose) cout << "\nTest all ``set column'' functions." << endl;

        const int NUM_ROW_TESTS = 5;

        for (int ri = 0; ri < NUM_ROW_TESTS; ++ri, mTT.appendNullRow()) {

            const bool Z = !ri; // case where setColumnValue has no effect

            if (veryVerbose) { T_ P_(ri) P(Z) }

            for (int ti = 0; ti < NUM_TABLES; TP[ti++]->appendNullRow()) {
                if (veryVerbose) { T_ T_ P_(ti) P(S[ti]) }
 // v-----------^

    if (veryVeryVerbose) { cout << "\t\t\tCHAR" << endl; }
    TP[ti]->setColumnChar(          0, A00);
    LOOP2_ASSERT(ri, ti, Z == (TT == *TP[ti]));
    LOOP2_ASSERT(ri, ti, 1 == isSameExceptColumn(*TP[ti],  0, NN.row()));
    LOOP4_ASSERT(ri, ti, *TP[ti], A[0],
                 1 == isSameColumnValue( *TP[ti],  0, A[ 0]));
    TP[ti]->setColumnChar(          0, U00);
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti],  0, N[ 0]));
    TP[ti]->makeColumnNull(0);

    if (veryVeryVerbose) { cout << "\t\t\tSHORT" << endl; }
    TP[ti]->setColumnShort(         1, A01);
    LOOP2_ASSERT(ri, ti, Z == (TT == *TP[ti]));
    LOOP2_ASSERT(ri, ti, 1 == isSameExceptColumn(*TP[ti],  1, NN.row()));
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti],  1, A[ 1]));
    TP[ti]->setColumnShort(         1, U01);
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti],  1, N[ 1]));
    TP[ti]->makeColumnNull(1);

    if (veryVeryVerbose) { cout << "\t\t\tINT" << endl; }
    TP[ti]->setColumnInt(           2, A02);
    LOOP2_ASSERT(ri, ti, Z == (TT == *TP[ti]));
    LOOP2_ASSERT(ri, ti, 1 == isSameExceptColumn(*TP[ti],  2, NN.row()));
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti],  2, A[ 2]));
    TP[ti]->setColumnInt(           2, U02);
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti],  2, N[ 2]));
    TP[ti]->makeColumnNull(2);

    if (veryVeryVerbose) { cout << "\t\t\tINT64" << endl; }
    TP[ti]->setColumnInt64(         3, A03);
    LOOP2_ASSERT(ri, ti, Z == (TT == *TP[ti]));
    LOOP2_ASSERT(ri, ti, 1 == isSameExceptColumn(*TP[ti],  3, NN.row()));
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti],  3, A[ 3]));
    TP[ti]->setColumnInt64(         3, U03);
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti],  3, N[ 3]));
    TP[ti]->makeColumnNull(3);

    if (veryVeryVerbose) { cout << "\t\t\tFLOAT" << endl; }
    TP[ti]->setColumnFloat(         4, A04);
    LOOP2_ASSERT(ri, ti, Z == (TT == *TP[ti]));
    LOOP2_ASSERT(ri, ti, 1 == isSameExceptColumn(*TP[ti],  4, NN.row()));
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti],  4, A[ 4]));
    TP[ti]->setColumnFloat(         4, U04);
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti],  4, N[ 4]));
    TP[ti]->makeColumnNull(4);

    if (veryVeryVerbose) { cout << "\t\t\tDOUBLE" << endl; }
    TP[ti]->setColumnDouble(        5, A05);
    LOOP2_ASSERT(ri, ti, Z == (TT == *TP[ti]));
    LOOP2_ASSERT(ri, ti, 1 == isSameExceptColumn(*TP[ti],  5, NN.row()));
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti],  5, A[ 5]));
    TP[ti]->setColumnDouble(        5, U05);
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti],  5, N[ 5]));
    TP[ti]->makeColumnNull(5);

    if (veryVeryVerbose) { cout << "\t\t\tSTRING" << endl; }
    TP[ti]->setColumnString(        6, A06);
    LOOP2_ASSERT(ri, ti, Z == (TT == *TP[ti]));
    LOOP2_ASSERT(ri, ti, 1 == isSameExceptColumn(*TP[ti],  6, NN.row()));
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti],  6, A[ 6]));
    TP[ti]->setColumnString(        6, U06);
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti],  6, N[ 6]));
    TP[ti]->makeColumnNull(6);

    if (veryVeryVerbose) { cout << "\t\t\t(const char *)" << endl; }
    TP[ti]->setColumnString(        6, A06.c_str());
    LOOP2_ASSERT(ri, ti, Z == (TT == *TP[ti]));
    LOOP2_ASSERT(ri, ti, 1 == isSameExceptColumn(*TP[ti],  6, NN.row()));
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti],  6, A[ 6]));
    TP[ti]->setColumnString(        6, U06.c_str());
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti],  6, N[ 6]));
    TP[ti]->makeColumnNull(6);

    if (veryVeryVerbose) { cout << "\t\t\tDATETIME" << endl; }
    TP[ti]->setColumnDatetime(      7, A07);
    LOOP2_ASSERT(ri, ti, Z == (TT == *TP[ti]));
    LOOP2_ASSERT(ri, ti, 1 == isSameExceptColumn(*TP[ti],  7, NN.row()));
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti],  7, A[ 7]));
    TP[ti]->setColumnDatetime(      7, U07);
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti],  7, N[ 7]));
    TP[ti]->makeColumnNull(7);

    if (veryVeryVerbose) { cout << "\t\t\tDATE" << endl; }
    TP[ti]->setColumnDate(          8, A08);
    LOOP2_ASSERT(ri, ti, Z == (TT == *TP[ti]));
    LOOP2_ASSERT(ri, ti, 1 == isSameExceptColumn(*TP[ti],  8, NN.row()));
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti],  8, A[ 8]));
    TP[ti]->setColumnDate(          8, U08);
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti],  8, N[ 8]));
    TP[ti]->makeColumnNull(8);

    if (veryVeryVerbose) { cout << "\t\t\tTIME" << endl; }
    TP[ti]->setColumnTime(          9, A09);
    LOOP2_ASSERT(ri, ti, Z == (TT == *TP[ti]));
    LOOP2_ASSERT(ri, ti, 1 == isSameExceptColumn(*TP[ti],  9, NN.row()));
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti],  9, A[ 9]));
    TP[ti]->setColumnTime(          9, U09);
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti],  9, N[ 9]));
    TP[ti]->makeColumnNull(9);

    if (veryVeryVerbose) { cout << "\t\t\tCHAR_ARRAY" << endl; }
    TP[ti]->setColumnCharArray(    10, A10);
    LOOP2_ASSERT(ri, ti, Z == (TT == *TP[ti]));
    LOOP2_ASSERT(ri, ti, 1 == isSameExceptColumn(*TP[ti], 10, NN.row()));
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 10, A[10]));
    TP[ti]->setColumnCharArray(    10, U10);
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 10, N[10]));
    TP[ti]->makeColumnNull(10);

    if (veryVeryVerbose) { cout << "\t\t\tSHORT_ARRAY" << endl; }
    TP[ti]->setColumnShortArray(   11, A11);
    LOOP2_ASSERT(ri, ti, Z == (TT == *TP[ti]));
    LOOP2_ASSERT(ri, ti, 1 == isSameExceptColumn(*TP[ti], 11, NN.row()));
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 11, A[11]));
    TP[ti]->setColumnShortArray(   11, U11);
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 11, N[11]));
    TP[ti]->makeColumnNull(11);

    if (veryVeryVerbose) { cout << "\t\t\tINT_ARRAY" << endl; }
    TP[ti]->setColumnIntArray(     12, A12);
    LOOP2_ASSERT(ri, ti, Z == (TT == *TP[ti]));
    LOOP2_ASSERT(ri, ti, 1 == isSameExceptColumn(*TP[ti], 12, NN.row()));
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 12, A[12]));
    TP[ti]->setColumnIntArray(     12, U12);
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 12, N[12]));
    TP[ti]->makeColumnNull(12);

    if (veryVeryVerbose) { cout << "\t\t\tINT64_ARRRAY" << endl; }
    TP[ti]->setColumnInt64Array(   13, A13);
    LOOP2_ASSERT(ri, ti, Z == (TT == *TP[ti]));
    LOOP2_ASSERT(ri, ti, 1 == isSameExceptColumn(*TP[ti], 13, NN.row()));
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 13, A[13]));
    TP[ti]->setColumnInt64Array(   13, U13);
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 13, N[13]));
    TP[ti]->makeColumnNull(13);

    if (veryVeryVerbose) { cout << "\t\t\tFLOAT_ARRAY" << endl; }
    TP[ti]->setColumnFloatArray(   14, A14);
    LOOP2_ASSERT(ri, ti, Z == (TT == *TP[ti]));
    LOOP2_ASSERT(ri, ti, 1 == isSameExceptColumn(*TP[ti], 14, NN.row()));
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 14, A[14]));
    TP[ti]->setColumnFloatArray(   14, U14);
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 14, N[14]));
    TP[ti]->makeColumnNull(14);

    if (veryVeryVerbose) { cout << "\t\t\tDOUBLE_ARRAY" << endl; }
    TP[ti]->setColumnDoubleArray(  15, A15);
    LOOP2_ASSERT(ri, ti, Z == (TT == *TP[ti]));
    LOOP2_ASSERT(ri, ti, 1 == isSameExceptColumn(*TP[ti], 15, NN.row()));
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 15, A[15]));
    TP[ti]->setColumnDoubleArray(  15, U15);
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 15, N[15]));
    TP[ti]->makeColumnNull(15);

    if (veryVeryVerbose) { cout << "\t\t\tSTRING_ARRAY" << endl; }
    TP[ti]->setColumnStringArray(  16, A16);
    LOOP2_ASSERT(ri, ti, Z == (TT == *TP[ti]));
    LOOP2_ASSERT(ri, ti, 1 == isSameExceptColumn(*TP[ti], 16, NN.row()));
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 16, A[16]));
    TP[ti]->setColumnStringArray(  16, U16);
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 16, N[16]));
    TP[ti]->makeColumnNull(16);

    if (veryVeryVerbose) { cout << "\t\t\tDATETIME_ARRAY" << endl; }
    TP[ti]->setColumnDatetimeArray(17, A17);
    LOOP2_ASSERT(ri, ti, Z == (TT == *TP[ti]));
    LOOP2_ASSERT(ri, ti, 1 == isSameExceptColumn(*TP[ti], 17, NN.row()));
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 17, A[17]));
    TP[ti]->setColumnDatetimeArray(17, U17);
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 17, N[17]));
    TP[ti]->makeColumnNull(17);

    if (veryVeryVerbose) { cout << "\t\t\tDATE_ARRAY" << endl; }
    TP[ti]->setColumnDateArray(    18, A18);
    LOOP2_ASSERT(ri, ti, Z == (TT == *TP[ti]));
    LOOP2_ASSERT(ri, ti, 1 == isSameExceptColumn(*TP[ti], 18, NN.row()));
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 18, A[18]));
    TP[ti]->setColumnDateArray(    18, U18);
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 18, N[18]));
    TP[ti]->makeColumnNull(18);

    if (veryVeryVerbose) { cout << "\t\t\tTIME_ARRAY" << endl; }
    TP[ti]->setColumnTimeArray(    19, A19);
    LOOP2_ASSERT(ri, ti, Z == (TT == *TP[ti]));
    LOOP2_ASSERT(ri, ti, 1 == isSameExceptColumn(*TP[ti], 19, NN.row()));
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 19, A[19]));
    TP[ti]->setColumnTimeArray(    19, U19);
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 19, N[19]));
    TP[ti]->makeColumnNull(19);

    if (veryVeryVerbose) { cout << "\t\t\tLIST" << endl; }
    TP[ti]->setColumnList(         20, A20);
    LOOP2_ASSERT(ri, ti, Z == (TT == *TP[ti]));
    LOOP2_ASSERT(ri, ti, 1 == isSameExceptColumn(*TP[ti], 20, NN.row()));
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 20, A[20]));
    TP[ti]->setColumnList(         20, U20);
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 20, N[20]));
    TP[ti]->makeColumnNull(20);

    if (veryVeryVerbose) { cout << "\t\t\t(const bdem_Row&)" << endl; }
    TP[ti]->setColumnList(         20, A20.row());
    LOOP2_ASSERT(ri, ti, Z == (TT == *TP[ti]));
    LOOP2_ASSERT(ri, ti, 1 == isSameExceptColumn(*TP[ti], 20, NN.row()));
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 20, A[20]));
    TP[ti]->setColumnList(         20, U20.row());
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 20, N[20]));
    TP[ti]->makeColumnNull(20);

    if (veryVeryVerbose) { cout << "\t\t\tTABLE" << endl; }
    TP[ti]->setColumnTable(        21, A21);
    LOOP2_ASSERT(ri, ti, Z == (TT == *TP[ti]));
    LOOP2_ASSERT(ri, ti, 1 == isSameExceptColumn(*TP[ti], 21, NN.row()));
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 21, A[21]));
    TP[ti]->setColumnTable(        21, U21);
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 21, N[21]));
    TP[ti]->makeColumnNull(21);

    if (veryVeryVerbose) { cout << "\t\t\tBOOL" << endl; }
    TP[ti]->setColumnBool(         22, A22);
    LOOP2_ASSERT(ri, ti, Z == (TT == *TP[ti]));
    LOOP2_ASSERT(ri, ti, 1 == isSameExceptColumn(*TP[ti], 22, NN.row()));
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 22, A[22]));
    TP[ti]->setColumnBool(         22, U22);
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 22, N[22]));
    TP[ti]->makeColumnNull(22);

    if (veryVeryVerbose) { cout << "\t\t\tDATETIMETZ" << endl; }
    TP[ti]->setColumnDatetimeTz(     23, A23);
    LOOP2_ASSERT(ri, ti, Z == (TT == *TP[ti]));
    LOOP2_ASSERT(ri, ti, 1 == isSameExceptColumn(*TP[ti], 23, NN.row()));
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 23, A[23]));
    TP[ti]->setColumnDatetimeTz(     23, U23);
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 23, N[23]));
    TP[ti]->makeColumnNull(23);

    if (veryVeryVerbose) { cout << "\t\t\tDATETZ" << endl; }
    TP[ti]->setColumnDateTz(         24, A24);
    LOOP2_ASSERT(ri, ti, Z == (TT == *TP[ti]));
    LOOP2_ASSERT(ri, ti, 1 == isSameExceptColumn(*TP[ti], 24, NN.row()));
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 24, A[24]));
    TP[ti]->setColumnDateTz(         24, U24);
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 24, N[24]));
    TP[ti]->makeColumnNull(24);

    if (veryVeryVerbose) { cout << "\t\t\tTIMETZ" << endl; }
    TP[ti]->setColumnTimeTz(         25, A25);
    LOOP2_ASSERT(ri, ti, Z == (TT == *TP[ti]));
    LOOP2_ASSERT(ri, ti, 1 == isSameExceptColumn(*TP[ti], 25, NN.row()));
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 25, A[25]));
    TP[ti]->setColumnTimeTz(         25, U25);
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 25, N[25]));
    TP[ti]->makeColumnNull(25);

    if (veryVeryVerbose) { cout << "\t\t\tBOOL_ARRAY" << endl; }
    TP[ti]->setColumnBoolArray(    26, A26);
    LOOP2_ASSERT(ri, ti, Z == (TT == *TP[ti]));
    LOOP2_ASSERT(ri, ti, 1 == isSameExceptColumn(*TP[ti], 26, NN.row()));
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 26, A[26]));
    TP[ti]->setColumnBoolArray(    26, U26);
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 26, N[26]));
    TP[ti]->makeColumnNull(26);

    if (veryVeryVerbose) { cout << "\t\t\tDATETIMETZ_ARRAY" << endl; }
    TP[ti]->setColumnDatetimeTzArray(27, A27);
    LOOP2_ASSERT(ri, ti, Z == (TT == *TP[ti]));
    LOOP2_ASSERT(ri, ti, 1 == isSameExceptColumn(*TP[ti], 27, NN.row()));
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 27, A[27]));
    TP[ti]->setColumnDatetimeTzArray(27, U27);
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 27, N[27]));
    TP[ti]->makeColumnNull(27);

    if (veryVeryVerbose) { cout << "\t\t\tDATETZ_ARRAY" << endl; }
    TP[ti]->setColumnDateTzArray(    28, A28);
    LOOP2_ASSERT(ri, ti, Z == (TT == *TP[ti]));
    LOOP2_ASSERT(ri, ti, 1 == isSameExceptColumn(*TP[ti], 28, NN.row()));
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 28, A[28]));
    TP[ti]->setColumnDateTzArray(    28, U28);
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 28, N[28]));
    TP[ti]->makeColumnNull(28);

    if (veryVeryVerbose) { cout << "\t\t\tTIMETZ_ARRAY" << endl; }
    TP[ti]->setColumnTimeTzArray(    29, A29);
    LOOP2_ASSERT(ri, ti, Z == (TT == *TP[ti]));
    LOOP2_ASSERT(ri, ti, 1 == isSameExceptColumn(*TP[ti], 29, NN.row()));
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 29, A[29]));
    TP[ti]->setColumnTimeTzArray(    29, U29);
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 29, N[29]));
    TP[ti]->makeColumnNull(29);

    if (veryVeryVerbose) { cout << "\t\t\tCHOICE" << endl; }
    TP[ti]->setColumnChoice(    30, A30);
    LOOP2_ASSERT(ri, ti, Z == (TT == *TP[ti]));
    LOOP2_ASSERT(ri, ti, 1 == isSameExceptColumn(*TP[ti], 30, NN.row()));
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 30, A[30]));
    TP[ti]->setColumnChoice(    30, U30);
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 30, N[30]));
    TP[ti]->makeColumnNull(30);

    if (veryVeryVerbose) { cout << "\t\t\tCHOICE_ARRAY" << endl; }
    TP[ti]->setColumnChoiceArray(    31, A31);
    LOOP2_ASSERT(ri, ti, Z == (TT == *TP[ti]));
    LOOP2_ASSERT(ri, ti, 1 == isSameExceptColumn(*TP[ti], 31, NN.row()));
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 31, A[31]));
    TP[ti]->setColumnChoiceArray(    31, U31);
    LOOP2_ASSERT(ri, ti, 1 == isSameColumnValue( *TP[ti], 31, N[31]));
    TP[ti]->makeColumnNull(31);

    for (int ci = 0; ci < NUM_ELEMS; ++ci) {
        if (veryVeryVerbose) { T_ T_ T_ P(ci); }

        LOOP3_ASSERT(ri, ti, ci,
                     1 == isSameExceptColumn(*TP[ti],ci, NN.row()));
        LOOP3_ASSERT(ri, ti, ci, 1 == isSameColumnValue(*TP[ti], ci, NN[ci]));
        LOOP3_ASSERT(ri, ti, ci, Z == isSameColumnValue(*TP[ti], ci, A[ci]));

        TP[ti]->setColumnValue(ci, A[ci]);
        LOOP3_ASSERT(ri, ti, ci, Z == (TT == *TP[ti]));
        LOOP3_ASSERT(ri, ti, ci,
                     1 == isSameExceptColumn(*TP[ti],ci, NN.row()));
        LOOP3_ASSERT(ri, ti, ci, Z == isSameColumnValue(*TP[ti], ci, N[ci]));
        LOOP3_ASSERT(ri, ti, ci, 1 == isSameColumnValue(*TP[ti], ci, A[ci]));

        TP[ti]->setColumnValue(ci, NN[ci]);
        LOOP3_ASSERT(ri, ti, ci, !Z == TP[ti]->isAnyInColumnNull(ci));
        LOOP3_ASSERT(ri, ti, ci, 0 == TP[ti]->isAnyInColumnNonNull(ci));
        LOOP3_ASSERT(ri, ti, ci,
                     1 == isSameExceptColumn(*TP[ti],ci, NN.row()));
        LOOP3_ASSERT(ri, ti, ci, 1 == isSameColumnValue(*TP[ti], ci, NN[ci]));
        LOOP3_ASSERT(ri, ti, ci, Z == isSameColumnValue(*TP[ti], ci, A[ci]));

        TP[ti]->setColumnValue(ci, NN[ci]);
        LOOP3_ASSERT(ri, ti, ci,
                     1 == isSameExceptColumn(*TP[ti],ci, NN.row()));
        LOOP3_ASSERT(ri, ti, ci, 1 == isSameColumnValue(*TP[ti], ci, NN[ci]));
        LOOP3_ASSERT(ri, ti, ci, Z == isSameColumnValue(*TP[ti], ci, A[ci]));
    }
 // ^-----------v
            } // for each able with unique allocation strategy, ti
        } // for each total number of rows, ri

        if (veryVeryVerbose) {
            cout <<
                "\nRelative Memory Usage for the three allocation strategies:"
                                                               << endl << endl;
            P_(S[0]); a[0].print(); cout << endl;
            P_(S[1]); a[1].print(); cout << endl;
            P_(S[2]); a[2].print(); cout << endl;
        }

        // ==================================================
        // ORIG. VALUES FOR VARIOUS ALLOCATION MODES (CYGWIN)
        //
        // S[0] = BDEM_PASS_THROUGH,
        // ==================================================
        //                 TEST ALLOCATOR STATE
        // --------------------------------------------------
        //         Category     Blocks  Bytes
        //         --------     ------  -----
        //           IN USE     83      2968
        //              MAX     83      2968
        //            TOTAL     406     8060
        //   NUM MISMATCHES     0
        // --------------------------------------------------
        //
        // S[1] = BDEM_WRITE_ONCE,
        // ==================================================
        //                 TEST ALLOCATOR STATE
        // --------------------------------------------------
        //         Category     Blocks  Bytes
        //         --------     ------  -----
        //           IN USE     14      10336
        //              MAX     14      10336
        //            TOTAL     14      10336
        //   NUM MISMATCHES     0
        // --------------------------------------------------
        //
        // S[2] = BDEM_WRITE_MANY,
        // ==================================================
        //                 TEST ALLOCATOR STATE
        // --------------------------------------------------
        //         Category     Blocks  Bytes
        //         --------     ------  -----
        //           IN USE     24      4956
        //              MAX     24      4956
        //            TOTAL     24      4956
        //   NUM MISMATCHES     0
        // --------------------------------------------------
      }

DEFINE_TEST_CASE(13) {
        // --------------------------------------------------------------------
        // EXCEPTION SAFETY OF APPENDING/INSERTING ROWS
        //
        // Concerns:
        //   That all the functions tested in case 12 that allocate memory
        //   are exception neutral.
        //
        // Plan:
        //   Use the BSLMA_EXCEPTION_TEST macros to provoke and catch
        //   exceptions when strings are allocated.
        //   Note it is not possible to test the remove rows functions since
        //   that would involve somehow making destructors throw exceptions.
        // --------------------------------------------------------------------

        bslma_TestAllocator defaultAllocator;
        const bslma_DefaultAllocatorGuard dag(&defaultAllocator);

        {
            bslma_TestAllocator testAllocator, *Z = &testAllocator;
            int passCount = 0;

            // verify copying and assigning strings allocates memory
            string s1(Z), s2(Z);
            s1 = "woof                                          woof";
            BEGIN_BSLMA_EXCEPTION_TEST
                ++passCount;

                s2 = s1;

                string s3(s1, &testAllocator);
            END_BSLMA_EXCEPTION_TEST

            LOOP_ASSERT(passCount, -1 == passCount || 3 == passCount);

            if (verbose) {
                P(passCount);
            }
        }

        {
            bslma_TestAllocator testAllocator, *Z = &testAllocator;
            int passCount = 0, firstAllocCount;

            bdem_List mL; const bdem_List& L = mL; ASSERT(0 == L.length());
            mL.appendString("woof                                    woof");
            ASSERT(1 == L.length());

            vector<bdem_ElemType::Type> typeVec;
            typeVec.push_back(bdem_ElemType::BDEM_STRING);
            bdem_Table mTOrig(typeVec); const bdem_Table& TOrig = mTOrig;
            mTOrig.appendRow(L);
            mTOrig.appendRow(L);
            mTOrig.appendRow(L);
            mTOrig.theModifiableRow(0)[0].theModifiableString()
                             = "meow                                     meow";
            mTOrig.theModifiableRow(1)[0].theModifiableString()
                             = "arf                                       arf";
            mTOrig.theModifiableRow(2)[0].theModifiableString()
                             = "gruff                                   gruff";

            bdem_Table mT(typeVec, Z); const bdem_Table& T = mT;

            BEGIN_BSLMA_EXCEPTION_TEST
                ++passCount;

                mT.appendRow(L);
                // just out of curiosity, find out how many allocations it
                // took to add one list with one string in it.
                {
                    static bool firstTime = true;
                    if (firstTime) {
                        firstTime = false;
                        firstAllocCount = passCount - 1;
                    }
                }

                mT.appendRow(L);
                mT.appendRow(TOrig, 0);
                mT.appendRows(TOrig);
                mT.appendRows(TOrig, 1, 2);

                mT.insertRow(2, L);
                mT.insertRow(2, L);
                mT.insertRow(2, TOrig, 0);
                mT.insertRows(2, TOrig);
                mT.insertRows(2, TOrig, 1, 2);
            END_BSLMA_EXCEPTION_TEST

            if (verbose) {
                P_(passCount); P_(firstAllocCount); P(T.numRows());
            }
        }
      }

DEFINE_TEST_CASE(12) {
        // --------------------------------------------------------------------
        // APPENDING/INSERTING/REMOVING LISTS/ROWS
        //   Ability to append/insert/remove 0 or more elements from list/row.
        //
        // Concerns:
        //   - That the indices are implemented correctly.
        //   - That the correct row values are inserted properly.
        //   - That the source objects are not changed (except with aliasing).
        //   - That (shallow) aliasing works properly.
        //   - That if an allocator is specified, the default is NOT used.
        //
        // Plan:
        //  Create two separate table-based tests - the second for aliasing.
        //   - Create vectors designed for the most general case.
        //   - For each special case, apply that vector only if applicable.
        //   - Implement the tables such that each table is created with
        //      an explicit allocator and verify that the default is NOT used.
        //   - Use orthogonal perturbation to exercise:
        //      - different combinations of columns.
        //      - different allocation strategies.
        //   - Explicitly test the locally installed default allocator for zero
        //      total usage prior to leaving scope in each test block.
        //
        // Tactics:
        //   - Depth-Ordered-Enumeration Data Selection Method
        //   - Table-Based Implementation Technique
        //
        // Testing:
        //   void appendRow(const bdem_List& srcList);
        //   void appendRow(const bdem_Table& srcTable, int srcIndex);
        //   void appendRows(const bdem_Table& srcTable);
        //   void appendRows(const bdem_Table& st, int srcIndex, int rowCount);
        //
        //   void insertRow(int dstIndex, const bdem_List& srcList);
        //   void insertRow(int dstIndex, const bdem_Table& st, int srcIndex);
        //   void insertRows(int dstIndex, const bdem_Table& srcTable);
        //   void insertRows(int di, const bdem_Table& st, int si, int rc);
        //
        //   void removeRow(int index);
        //   void removeRows(int startIndex, int rowCount);
        // --------------------------------------------------------------------

        bslma_TestAllocator defaultAllocator;
        const bslma_DefaultAllocatorGuard dag(&defaultAllocator);

        if (verbose) cout << endl
                  << "APPENDING/INSERTING/REMOVING LIST/ROW ELEMENTS" << endl
                  << "==============================================" << endl;

        bslma_TestAllocator testAllocator;

        if (verbose) cout << "\nCreate general reference lists." << endl;

        bdem_List mA; const bdem_List& A = mA; ASSERT(0         == A.length());
        loadReferenceA(&mA);                   ASSERT(NUM_ELEMS == A.length());
        bdem_List mB; const bdem_List& B = mB; ASSERT(0         == B.length());
        loadReferenceB(&mB);                   ASSERT(NUM_ELEMS == B.length());
        bdem_List mC; const bdem_List& C = mC; ASSERT(0         == C.length());
        loadReferenceC(&mC);                   ASSERT(NUM_ELEMS == C.length());
        bdem_List mD; const bdem_List& D = mD; ASSERT(0         == D.length());
        loadReferenceD(&mD);                   ASSERT(NUM_ELEMS == D.length());
        bdem_List mE; const bdem_List& E = mE; ASSERT(0         == E.length());
        loadReferenceE(&mE);                   ASSERT(NUM_ELEMS == E.length());
        bdem_List mU; const bdem_List& U = mU; ASSERT(0         == U.length());
        loadReferenceU(&mU);                   ASSERT(NUM_ELEMS == U.length());

        if (verbose) cout << "\tCreate elem-type array for reference row."
                                                                       << endl;
        vector<bdem_ElemType::Type> t32;
        {
            for (int i = 0; i < NUM_ELEMS; ++i) {
                t32.push_back(bdem_ElemType::Type(i));
            }
        }

        bdem_Table mT(&t32.front(), t32.size()); const bdem_Table& T = mT;
        ASSERT(0 == T.numRows()); ASSERT(NUM_ELEMS == T.numColumns());

        if (verbose) cout <<
            "\tEstablish sequence of column-config specs." << endl;

        // The following array of strings describe the various column-type
        // configuration specifications to be tested for both alias and
        // non-alias tables.

        const char *const CONFIGS[] = {
            "C",                        // an INT
            "",                         // no columns
            "EF",                       // a FLOAT and a DOUBLE
            "P",                        // a single STRING_ARRAY column
            "VWUTSXRQYPfOZNeMaLKbJIdHGcFEDCBA", // all column types in reverse
                                                // order
        };
        const int NUM_CONFIGS = sizeof CONFIGS / sizeof *CONFIGS;

        if (verbose) { T_ T_ P(NUM_CONFIGS) }
        ASSERT(NUM_ELEMS == strlen(CONFIGS[NUM_CONFIGS - 1])); // double check!

        mT.appendRow(A);
        mT.appendRow(B);
        mT.appendRow(C);
        mT.appendRow(D);
        mT.appendRow(E);
        ASSERT(5 == T.numRows()); ASSERT(NUM_ELEMS == T.numColumns());

        if (verbose) cout << "\nTesting NON-ALIAS case." << endl;
        {
            static const struct {
                int         d_lineNum; // source line number
                const char *d_dSpec;   // initial (destination) table value, dt
                int         d_di;      // index at which to insert into dt
                const char *d_sSpec;   // source table value, st
                int         d_si;      // index at which to insert from st
                int         d_ne;      // number of rows to insert
                const char *d_eSpec;   // expected final value of dt
            } DATA[] = {
                //line  d-object di  s-object si  ne  expected
                //----  -------- --  -------- --  --  --------   Depth = 0
                { L_,   "",      0,  "",      0,  0,  ""      },

                //line  d-object di  s-object si  ne  expected
                //----  -------- --  -------- --  --  --------   Depth = 1
                { L_,   "A",     0,  "",      0,  0,  "A"     },
                { L_,   "A",     1,  "",      0,  0,  "A"     },

                { L_,   "",      0,  "A",     0,  0,  ""      },
                { L_,   "",      0,  "A",     0,  1,  "A"     },
                { L_,   "",      0,  "A",     1,  0,  ""      },

                //line  d-object di  s-object si  ne  expected
                //----  -------- --  -------- --  --  --------   Depth = 2
                { L_,   "AB",    0,  "",      0,  0,  "AB"    },
                { L_,   "AB",    1,  "",      0,  0,  "AB"    },
                { L_,   "AB",    2,  "",      0,  0,  "AB"    },

                { L_,   "A",     0,  "B",     0,  0,  "A"     },
                { L_,   "A",     0,  "B",     0,  1,  "BA"    },
                { L_,   "A",     0,  "B",     1,  0,  "A"     },
                { L_,   "A",     1,  "B",     0,  0,  "A"     },
                { L_,   "A",     1,  "B",     0,  1,  "AB"    },
                { L_,   "A",     1,  "B",     1,  0,  "A"     },

                { L_,   "",      0,  "AB",    0,  0,  ""      },
                { L_,   "",      0,  "AB",    0,  1,  "A"     },
                { L_,   "",      0,  "AB",    0,  2,  "AB"    },
                { L_,   "",      0,  "AB",    1,  0,  ""      },
                { L_,   "",      0,  "AB",    1,  1,  "B"     },
                { L_,   "",      0,  "AB",    2,  0,  ""      },

                //line  d-object di  s-object si  ne  expected
                //----  -------- --  -------- --  --  --------   Depth = 3
                { L_,   "ABC",   0,  "",      0,  0,  "ABC"   },
                { L_,   "ABC",   1,  "",      0,  0,  "ABC"   },
                { L_,   "ABC",   2,  "",      0,  0,  "ABC"   },
                { L_,   "ABC",   3,  "",      0,  0,  "ABC"   },

                { L_,   "AB",    0,  "C",     0,  0,  "AB"    },
                { L_,   "AB",    0,  "C",     0,  1,  "CAB"   },
                { L_,   "AB",    0,  "C",     1,  0,  "AB"    },
                { L_,   "AB",    1,  "C",     0,  0,  "AB"    },
                { L_,   "AB",    1,  "C",     0,  1,  "ACB"   },
                { L_,   "AB",    1,  "C",     1,  0,  "AB"    },
                { L_,   "AB",    2,  "C",     0,  0,  "AB"    },
                { L_,   "AB",    2,  "C",     0,  1,  "ABC"   },
                { L_,   "AB",    2,  "C",     1,  0,  "AB"    },

                { L_,   "A",     0,  "BC",    0,  0,  "A"     },
                { L_,   "A",     0,  "BC",    0,  1,  "BA"    },
                { L_,   "A",     0,  "BC",    0,  2,  "BCA"   },
                { L_,   "A",     0,  "BC",    1,  0,  "A"     },
                { L_,   "A",     0,  "BC",    1,  1,  "CA"    },
                { L_,   "A",     0,  "BC",    2,  0,  "A"     },
                { L_,   "A",     1,  "BC",    0,  0,  "A"     },
                { L_,   "A",     1,  "BC",    0,  1,  "AB"    },
                { L_,   "A",     1,  "BC",    0,  2,  "ABC"   },
                { L_,   "A",     1,  "BC",    1,  0,  "A"     },
                { L_,   "A",     1,  "BC",    1,  1,  "AC"    },
                { L_,   "A",     1,  "BC",    2,  0,  "A"     },

                { L_,   "",      0,  "ABC",   0,  0,  ""      },
                { L_,   "",      0,  "ABC",   0,  1,  "A"     },
                { L_,   "",      0,  "ABC",   0,  2,  "AB"    },
                { L_,   "",      0,  "ABC",   0,  3,  "ABC"   },
                { L_,   "",      0,  "ABC",   1,  0,  ""      },
                { L_,   "",      0,  "ABC",   1,  1,  "B"     },
                { L_,   "",      0,  "ABC",   1,  2,  "BC"    },
                { L_,   "",      0,  "ABC",   2,  0,  ""      },
                { L_,   "",      0,  "ABC",   2,  1,  "C"     },
                { L_,   "",      0,  "ABC",   3,  0,  ""      },

                //line  d-object di  s-object si  ne  expected
                //----  -------- --  -------- --  --  --------   Depth = 4
                { L_,   "ABCD",  0,  "",      0,  0,  "ABCD"  },
                { L_,   "ABCD",  1,  "",      0,  0,  "ABCD"  },
                { L_,   "ABCD",  2,  "",      0,  0,  "ABCD"  },
                { L_,   "ABCD",  3,  "",      0,  0,  "ABCD"  },
                { L_,   "ABCD",  4,  "",      0,  0,  "ABCD"  },

                { L_,   "ABC",   0,  "D",     0,  0,  "ABC"   },
                { L_,   "ABC",   0,  "D",     0,  1,  "DABC"  },
                { L_,   "ABC",   0,  "D",     1,  0,  "ABC"   },
                { L_,   "ABC",   1,  "D",     0,  0,  "ABC"   },
                { L_,   "ABC",   1,  "D",     0,  1,  "ADBC"  },
                { L_,   "ABC",   1,  "D",     1,  0,  "ABC"   },
                { L_,   "ABC",   2,  "D",     0,  0,  "ABC"   },
                { L_,   "ABC",   2,  "D",     0,  1,  "ABDC"  },
                { L_,   "ABC",   2,  "D",     1,  0,  "ABC"   },
                { L_,   "ABC",   3,  "D",     0,  0,  "ABC"   },
                { L_,   "ABC",   3,  "D",     0,  1,  "ABCD"  },
                { L_,   "ABC",   3,  "D",     1,  0,  "ABC"   },

                { L_,   "AB",    0,  "CD",    0,  0,  "AB"    },
                { L_,   "AB",    0,  "CD",    0,  1,  "CAB"   },
                { L_,   "AB",    0,  "CD",    0,  2,  "CDAB"  },
                { L_,   "AB",    0,  "CD",    1,  0,  "AB"    },
                { L_,   "AB",    0,  "CD",    1,  1,  "DAB"   },
                { L_,   "AB",    0,  "CD",    2,  0,  "AB"    },
                { L_,   "AB",    1,  "CD",    0,  0,  "AB"    },
                { L_,   "AB",    1,  "CD",    0,  1,  "ACB"   },
                { L_,   "AB",    1,  "CD",    0,  2,  "ACDB"  },
                { L_,   "AB",    1,  "CD",    1,  0,  "AB"    },
                { L_,   "AB",    1,  "CD",    1,  1,  "ADB"   },
                { L_,   "AB",    1,  "CD",    2,  0,  "AB"    },
                { L_,   "AB",    2,  "CD",    0,  0,  "AB"    },
                { L_,   "AB",    2,  "CD",    0,  1,  "ABC"   },
                { L_,   "AB",    2,  "CD",    0,  2,  "ABCD"  },
                { L_,   "AB",    2,  "CD",    1,  0,  "AB"    },
                { L_,   "AB",    2,  "CD",    1,  1,  "ABD"   },
                { L_,   "AB",    2,  "CD",    2,  0,  "AB"    },

                { L_,   "A",     0,  "BCD",   0,  0,  "A"     },
                { L_,   "A",     0,  "BCD",   0,  1,  "BA"    },
                { L_,   "A",     0,  "BCD",   0,  2,  "BCA"   },
                { L_,   "A",     0,  "BCD",   0,  3,  "BCDA"  },
                { L_,   "A",     0,  "BCD",   1,  0,  "A"     },
                { L_,   "A",     0,  "BCD",   1,  1,  "CA"    },
                { L_,   "A",     0,  "BCD",   1,  2,  "CDA"   },
                { L_,   "A",     0,  "BCD",   2,  0,  "A"     },
                { L_,   "A",     0,  "BCD",   2,  1,  "DA"    },
                { L_,   "A",     0,  "BCD",   3,  0,  "A"     },
                { L_,   "A",     1,  "BCD",   0,  0,  "A"     },
                { L_,   "A",     1,  "BCD",   0,  1,  "AB"    },
                { L_,   "A",     1,  "BCD",   0,  2,  "ABC"   },
                { L_,   "A",     1,  "BCD",   0,  3,  "ABCD"  },
                { L_,   "A",     1,  "BCD",   1,  0,  "A"     },
                { L_,   "A",     1,  "BCD",   1,  1,  "AC"    },
                { L_,   "A",     1,  "BCD",   1,  2,  "ACD"   },
                { L_,   "A",     1,  "BCD",   2,  0,  "A"     },
                { L_,   "A",     1,  "BCD",   2,  1,  "AD"    },
                { L_,   "A",     1,  "BCD",   3,  0,  "A"     },

                { L_,   "",      0,  "ABCD",  0,  0,  ""      },
                { L_,   "",      0,  "ABCD",  0,  1,  "A"     },
                { L_,   "",      0,  "ABCD",  0,  2,  "AB"    },
                { L_,   "",      0,  "ABCD",  0,  3,  "ABC"   },
                { L_,   "",      0,  "ABCD",  0,  4,  "ABCD"  },
                { L_,   "",      0,  "ABCD",  1,  0,  ""      },
                { L_,   "",      0,  "ABCD",  1,  1,  "B"     },
                { L_,   "",      0,  "ABCD",  1,  2,  "BC"    },
                { L_,   "",      0,  "ABCD",  1,  3,  "BCD"   },
                { L_,   "",      0,  "ABCD",  2,  0,  ""      },
                { L_,   "",      0,  "ABCD",  2,  1,  "C"     },
                { L_,   "",      0,  "ABCD",  2,  2,  "CD"    },
                { L_,   "",      0,  "ABCD",  3,  0,  ""      },
                { L_,   "",      0,  "ABCD",  3,  1,  "D"     },
                { L_,   "",      0,  "ABCD",  4,  0,  ""      },

                //line  d-object di  s-object si  ne  expected
                //----  -------- --  -------- --  --  --------   Depth = 5
                { L_,   "ABCDE", 0,  "",      0,  0,  "ABCDE" },
                { L_,   "ABCDE", 1,  "",      0,  0,  "ABCDE" },
                { L_,   "ABCDE", 2,  "",      0,  0,  "ABCDE" },
                { L_,   "ABCDE", 3,  "",      0,  0,  "ABCDE" },
                { L_,   "ABCDE", 4,  "",      0,  0,  "ABCDE" },
                { L_,   "ABCDE", 5,  "",      0,  0,  "ABCDE" },

                { L_,   "ABCD",  0,  "E",     0,  0,  "ABCD"  },
                { L_,   "ABCD",  0,  "E",     0,  1,  "EABCD" },
                { L_,   "ABCD",  0,  "E",     1,  0,  "ABCD"  },
                { L_,   "ABCD",  1,  "E",     0,  0,  "ABCD"  },
                { L_,   "ABCD",  1,  "E",     0,  1,  "AEBCD" },
                { L_,   "ABCD",  1,  "E",     1,  0,  "ABCD"  },
                { L_,   "ABCD",  2,  "E",     0,  0,  "ABCD"  },
                { L_,   "ABCD",  2,  "E",     0,  1,  "ABECD" },
                { L_,   "ABCD",  2,  "E",     1,  0,  "ABCD"  },
                { L_,   "ABCD",  3,  "E",     0,  0,  "ABCD"  },
                { L_,   "ABCD",  3,  "E",     0,  1,  "ABCED" },
                { L_,   "ABCD",  3,  "E",     1,  0,  "ABCD"  },
                { L_,   "ABCD",  4,  "E",     0,  0,  "ABCD"  },
                { L_,   "ABCD",  4,  "E",     0,  1,  "ABCDE" },
                { L_,   "ABCD",  4,  "E",     1,  0,  "ABCD"  },

                { L_,   "ABC",   0,  "DE",    0,  0,  "ABC"   },
                { L_,   "ABC",   0,  "DE",    0,  1,  "DABC"  },
                { L_,   "ABC",   0,  "DE",    0,  2,  "DEABC" },
                { L_,   "ABC",   0,  "DE",    1,  0,  "ABC"   },
                { L_,   "ABC",   0,  "DE",    1,  1,  "EABC"  },
                { L_,   "ABC",   0,  "DE",    2,  0,  "ABC"   },
                { L_,   "ABC",   1,  "DE",    0,  0,  "ABC"   },
                { L_,   "ABC",   1,  "DE",    0,  1,  "ADBC"  },
                { L_,   "ABC",   1,  "DE",    0,  2,  "ADEBC" },
                { L_,   "ABC",   1,  "DE",    1,  0,  "ABC"   },
                { L_,   "ABC",   1,  "DE",    1,  1,  "AEBC"  },
                { L_,   "ABC",   1,  "DE",    2,  0,  "ABC"   },
                { L_,   "ABC",   2,  "DE",    0,  0,  "ABC"   },
                { L_,   "ABC",   2,  "DE",    0,  1,  "ABDC"  },
                { L_,   "ABC",   2,  "DE",    0,  2,  "ABDEC" },
                { L_,   "ABC",   2,  "DE",    1,  0,  "ABC"   },
                { L_,   "ABC",   2,  "DE",    1,  1,  "ABEC"  },
                { L_,   "ABC",   2,  "DE",    2,  0,  "ABC"   },
                { L_,   "ABC",   3,  "DE",    0,  0,  "ABC"   },
                { L_,   "ABC",   3,  "DE",    0,  1,  "ABCD"  },
                { L_,   "ABC",   3,  "DE",    0,  2,  "ABCDE" },
                { L_,   "ABC",   3,  "DE",    1,  0,  "ABC"   },
                { L_,   "ABC",   3,  "DE",    1,  1,  "ABCE"  },
                { L_,   "ABC",   3,  "DE",    2,  0,  "ABC"   },

                { L_,   "AB",    0,  "CDE",   0,  0,  "AB"    },
                { L_,   "AB",    0,  "CDE",   0,  1,  "CAB"   },
                { L_,   "AB",    0,  "CDE",   0,  2,  "CDAB"  },
                { L_,   "AB",    0,  "CDE",   0,  3,  "CDEAB" },
                { L_,   "AB",    0,  "CDE",   1,  0,  "AB"    },
                { L_,   "AB",    0,  "CDE",   1,  1,  "DAB"   },
                { L_,   "AB",    0,  "CDE",   1,  2,  "DEAB"  },
                { L_,   "AB",    0,  "CDE",   2,  0,  "AB"    },
                { L_,   "AB",    0,  "CDE",   2,  1,  "EAB"   },
                { L_,   "AB",    0,  "CDE",   3,  0,  "AB"    },
                { L_,   "AB",    1,  "CDE",   0,  0,  "AB"    },
                { L_,   "AB",    1,  "CDE",   0,  1,  "ACB"   },
                { L_,   "AB",    1,  "CDE",   0,  2,  "ACDB"  },
                { L_,   "AB",    1,  "CDE",   0,  3,  "ACDEB" },
                { L_,   "AB",    1,  "CDE",   1,  0,  "AB"    },
                { L_,   "AB",    1,  "CDE",   1,  1,  "ADB"   },
                { L_,   "AB",    1,  "CDE",   1,  2,  "ADEB"  },
                { L_,   "AB",    1,  "CDE",   2,  0,  "AB"    },
                { L_,   "AB",    1,  "CDE",   2,  1,  "AEB"   },
                { L_,   "AB",    1,  "CDE",   3,  0,  "AB"    },
                { L_,   "AB",    2,  "CDE",   0,  0,  "AB"    },
                { L_,   "AB",    2,  "CDE",   0,  1,  "ABC"   },
                { L_,   "AB",    2,  "CDE",   0,  2,  "ABCD"  },
                { L_,   "AB",    2,  "CDE",   0,  3,  "ABCDE" },
                { L_,   "AB",    2,  "CDE",   1,  0,  "AB"    },
                { L_,   "AB",    2,  "CDE",   1,  1,  "ABD"   },
                { L_,   "AB",    2,  "CDE",   1,  2,  "ABDE"  },
                { L_,   "AB",    2,  "CDE",   2,  0,  "AB"    },
                { L_,   "AB",    2,  "CDE",   2,  1,  "ABE"   },
                { L_,   "AB",    2,  "CDE",   3,  0,  "AB"    },

                { L_,   "A",     0,  "BCDE",  0,  0,  "A"     },
                { L_,   "A",     0,  "BCDE",  0,  1,  "BA"    },
                { L_,   "A",     0,  "BCDE",  0,  2,  "BCA"   },
                { L_,   "A",     0,  "BCDE",  0,  3,  "BCDA"  },
                { L_,   "A",     0,  "BCDE",  1,  0,  "A"     },
                { L_,   "A",     0,  "BCDE",  1,  1,  "CA"    },
                { L_,   "A",     0,  "BCDE",  1,  2,  "CDA"   },
                { L_,   "A",     0,  "BCDE",  2,  0,  "A"     },
                { L_,   "A",     0,  "BCDE",  2,  1,  "DA"    },
                { L_,   "A",     0,  "BCDE",  3,  0,  "A"     },
                { L_,   "A",     1,  "BCDE",  0,  0,  "A"     },
                { L_,   "A",     1,  "BCDE",  0,  1,  "AB"    },
                { L_,   "A",     1,  "BCDE",  0,  2,  "ABC"   },
                { L_,   "A",     1,  "BCDE",  0,  3,  "ABCD"  },
                { L_,   "A",     1,  "BCDE",  1,  0,  "A"     },
                { L_,   "A",     1,  "BCDE",  1,  1,  "AC"    },
                { L_,   "A",     1,  "BCDE",  1,  2,  "ACD"   },
                { L_,   "A",     1,  "BCDE",  2,  0,  "A"     },
                { L_,   "A",     1,  "BCDE",  2,  1,  "AD"    },
                { L_,   "A",     1,  "BCDE",  3,  0,  "A"     },

                { L_,   "",      0,  "ABCDE", 0,  0,  ""      },
                { L_,   "",      0,  "ABCDE", 0,  1,  "A"     },
                { L_,   "",      0,  "ABCDE", 0,  2,  "AB"    },
                { L_,   "",      0,  "ABCDE", 0,  3,  "ABC"   },
                { L_,   "",      0,  "ABCDE", 0,  4,  "ABCD"  },
                { L_,   "",      0,  "ABCDE", 0,  5,  "ABCDE" },
                { L_,   "",      0,  "ABCDE", 1,  0,  ""      },
                { L_,   "",      0,  "ABCDE", 1,  1,  "B"     },
                { L_,   "",      0,  "ABCDE", 1,  2,  "BC"    },
                { L_,   "",      0,  "ABCDE", 1,  3,  "BCD"   },
                { L_,   "",      0,  "ABCDE", 1,  4,  "BCDE"  },
                { L_,   "",      0,  "ABCDE", 2,  0,  ""      },
                { L_,   "",      0,  "ABCDE", 2,  1,  "C"     },
                { L_,   "",      0,  "ABCDE", 2,  2,  "CD"    },
                { L_,   "",      0,  "ABCDE", 2,  3,  "CDE"   },
                { L_,   "",      0,  "ABCDE", 3,  0,  ""      },
                { L_,   "",      0,  "ABCDE", 3,  1,  "D"     },
                { L_,   "",      0,  "ABCDE", 3,  2,  "DE"    },
                { L_,   "",      0,  "ABCDE", 4,  0,  ""      },
                { L_,   "",      0,  "ABCDE", 4,  1,  "E"     },
                { L_,   "",      0,  "ABCDE", 5,  0,  ""      },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const bdem_AggregateOption::AllocationStrategy MODES[] = {
                bdem_AggregateOption::BDEM_PASS_THROUGH,
                bdem_AggregateOption::BDEM_WRITE_ONCE,
                bdem_AggregateOption::BDEM_WRITE_MANY,
            };
            const int NUM_MODES = sizeof MODES / sizeof *MODES;

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *D_SPEC = DATA[ti].d_dSpec;
                const int   DI     = DATA[ti].d_di;
                const char *S_SPEC = DATA[ti].d_sSpec;
                const int   SI     = DATA[ti].d_si;
                const int   NE     = DATA[ti].d_ne;
                const char *E_SPEC = DATA[ti].d_eSpec;

                const int   DEPTH  = strlen(D_SPEC) + strlen(S_SPEC);
                if (DEPTH != oldDepth) {
                    oldDepth = DEPTH;
                    if (verbose) { cout << '\t';  P(DEPTH); }
                }

                for (int ci = 0; ci < NUM_CONFIGS; ++ci) { // CONFIG INDEX

                    // Parse col config to create the sequence of column types.

                    const char *const COLUMN_CONFIG_SPEC = CONFIGS[ci];

                    vector<bdem_ElemType::Type> columnTypes;
                    {
                        for (const char *s = COLUMN_CONFIG_SPEC; *s; ++s) {
                            columnTypes.push_back(getElemType(*s));
                        }
                    }

                    Obj mDD(columnTypes); const Obj& DD = mDD;
                    Obj mSS(columnTypes); const Obj& SS = mSS;
                    Obj mEE(columnTypes); const Obj& EE = mEE;

                    {
                        for (const char *s = D_SPEC; *s; ++s) {
                            gg(&mDD, COLUMN_CONFIG_SPEC, T[*s - 'A']);
                        }
                        LOOP2_ASSERT(ti, ci,
                                     (int)strlen(D_SPEC) == DD.numRows());
                    }
                    {
                        for (const char *s = S_SPEC; *s; ++s) {
                            gg(&mSS, COLUMN_CONFIG_SPEC, T[*s - 'A']);
                        }
                        LOOP2_ASSERT(ti, ci,
                                     (int)strlen(S_SPEC) == SS.numRows());
                    }
                    {
                        for (const char *s = E_SPEC; *s; ++s) {
                            gg(&mEE, COLUMN_CONFIG_SPEC, T[*s - 'A']);
                        }
                        LOOP2_ASSERT(ti, ci,
                                     (int)strlen(E_SPEC) == EE.numRows());
                    }

                    if (veryVerbose && !ci || veryVeryVerbose) {
                        cout << "\t  =================================="
                                    "=================================="
                        << endl;

                        cout << "\t  "; P_(D_SPEC); P_(DI); P_(S_SPEC);
                                                    P_(SI); P_(NE); P(E_SPEC);
                        cout << "\t  "; P_(ci); P(COLUMN_CONFIG_SPEC);
                        cout << "\t\t"; P(DD);
                        cout << "\t\t"; P(SS);
                        cout << "\t\t"; P(EE);
                    }

                    for (int ai = 0; ai < NUM_MODES; ++ai) { // ALLOCATION MODE

                        const bool PRINT = veryVerbose && !ci && !ai
                                        || veryVeryVerbose;

                        const bdem_AggregateOption::AllocationStrategy
                                                              MODE = MODES[ai];
                        if (PRINT) {
                            cout << "\t  ----------------------------------"
                                        "----------------------------------"
                            << endl;
                            cout << "\t  "; P(MODE);
                        }
 // v-------------------^
    if (DI == DD.numRows() && 1 == NE)
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);

        if (PRINT) cout << "\t\tappendRow(const bdem_List& srcList);" << endl;

        Obj mX(DD, MODE, &testAllocator);  const Obj &X = mX;
        {
            Obj mS(SS, &testAllocator);  const Obj &S = mS;
            if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
            const bdem_List L(S[SI], &testAllocator);
            mX.appendRow(L);
            if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP3_ASSERT(LINE, ci, ai, EE == X);
            LOOP3_ASSERT(LINE, ci, ai, SS == S);
        }
        LOOP3_ASSERT(LINE, ci, ai, EE == X);  // source is out of scope

        LOOP4_ASSERT(LINE, ci, ai, da.numBlocksTotal(), !da.numBlocksTotal());
    }

    if (DI == DD.numRows() && 1 == NE)
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);

        if (PRINT) cout <<
            "\t\tappendRow(const bdem_Table& srcTable, int srcIndex);" << endl;

        Obj mX(DD, MODE, &testAllocator);  const Obj &X = mX;
        {
            Obj mS(SS, &testAllocator);  const Obj &S = mS;
            if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
            mX.appendRow(S, SI);
            if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP3_ASSERT(LINE, ci, ai, EE == X);
            LOOP3_ASSERT(LINE, ci, ai, SS == S);
        }
        LOOP3_ASSERT(LINE, ci, ai, EE == X);  // source is out of scope

//      LOOP4_ASSERT(LINE, ci, ai, da.numBlocksTotal(), !da.numBlocksTotal());
    }

    if (DI == DD.numRows() && SS.numRows() == NE)
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);

        if (PRINT) cout <<
            "\t\tappendRows(const bdem_Table& srcTable);" << endl;

        Obj mX(DD, MODE, &testAllocator);  const Obj &X = mX;
        {
            Obj mS(SS, &testAllocator);  const Obj &S = mS;
            if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
            mX.appendRows(S);
            if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP3_ASSERT(LINE, ci, ai, EE == X);
            LOOP3_ASSERT(LINE, ci, ai, SS == S);
        }
        LOOP3_ASSERT(LINE, ci, ai, EE == X);  // source is out of scope

        LOOP4_ASSERT(LINE, ci, ai, da.numBlocksInUse(), !da.numBlocksInUse());
    }

    if (DI == DD.numRows())
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);

        if (PRINT) cout <<
            "\t\tappendRows(const bdem_Table& st, int srcIndex, int rowCount);"
                                                                       << endl;

        Obj mX(DD, MODE, &testAllocator);  const Obj &X = mX;
        {
            Obj mS(SS, &testAllocator);  const Obj &S = mS;
            if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
            mX.appendRows(S, SI, NE);
            if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP3_ASSERT(LINE, ci, ai, EE == X);
            LOOP3_ASSERT(LINE, ci, ai, SS == S);
        }
        LOOP3_ASSERT(LINE, ci, ai, EE == X);  // source is out of scope

        LOOP4_ASSERT(LINE, ci, ai, da.numBlocksInUse(), !da.numBlocksInUse());
    }

    if (1 == NE)
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);

        if (PRINT) cout <<
            "\t\tinsertRow(int dstIndex, const bdem_List& srcList);" << endl;

        Obj mX(DD, MODE, &testAllocator);  const Obj &X = mX;
        {
            Obj mS(SS, &testAllocator);  const Obj &S = mS;
            if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
            const bdem_List L(S[SI], &testAllocator);
            mX.insertRow(DI, L);
            if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP3_ASSERT(LINE, ci, ai, EE == X);
            LOOP3_ASSERT(LINE, ci, ai, SS == S);
        }
        LOOP3_ASSERT(LINE, ci, ai, EE == X);  // source is out of scope

        LOOP4_ASSERT(LINE, ci, ai, da.numBlocksTotal(), !da.numBlocksTotal());
    }

    if (1 == NE)
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);

        if (PRINT) cout <<
            "\t\tinsertRow(int dstIndex, const bdem_Table& st, int srcIndex);"
                                                                       << endl;
        Obj mX(DD, MODE, &testAllocator);  const Obj &X = mX;
        {
            Obj mS(SS, &testAllocator);  const Obj &S = mS;
            if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
            mX.insertRow(DI, S, SI);
            if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP3_ASSERT(LINE, ci, ai, EE == X);
            LOOP3_ASSERT(LINE, ci, ai, SS == S);
        }
        LOOP3_ASSERT(LINE, ci, ai, EE == X);  // source is out of scope

//      LOOP4_ASSERT(LINE, ci, ai, da.numBlocksTotal(), !da.numBlocksTotal());
    }

    if (SS.numRows() == NE)
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);

        if (PRINT) cout <<
            "\t\tinsertRows(int dstIndex, const bdem_Table& srcTable);"<< endl;

        Obj mX(DD, MODE, &testAllocator);  const Obj &X = mX;
        {
            Obj mS(SS, &testAllocator);  const Obj &S = mS;
            if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
            mX.insertRows(DI, S);
            if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP3_ASSERT(LINE, ci, ai, EE == X);
            LOOP3_ASSERT(LINE, ci, ai, SS == S);
        }
        LOOP3_ASSERT(LINE, ci, ai, EE == X);  // source is out of scope

        LOOP4_ASSERT(LINE, ci, ai, da.numBlocksInUse(), !da.numBlocksInUse());
    }

    if (1)
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);

        if (PRINT) cout <<
            "\t\tinsertRows(int di, const bdem_Table& st, int si, int rc);"
                                                                       << endl;
        Obj mX(DD, MODE, &testAllocator);  const Obj &X = mX;
        {
            Obj mS(SS, &testAllocator);  const Obj &S = mS;
            if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
            mX.insertRows(DI, S, SI, NE);
            if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP3_ASSERT(LINE, ci, ai, EE == X);
            LOOP3_ASSERT(LINE, ci, ai, SS == S);
        }
        LOOP3_ASSERT(LINE, ci, ai, EE == X);  // source is out of scope

        LOOP4_ASSERT(LINE, ci, ai, da.numBlocksInUse(), !da.numBlocksInUse());
    }

    if (1 == NE)
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);

        if (PRINT) cout << "\t\tremoveRow(int index);" << endl;

        Obj mX(EE, MODE, &testAllocator);  const Obj &X = mX;

        if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
        mX.removeRow(DI);
        if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
        LOOP3_ASSERT(LINE, ci, ai, DD == X);

        LOOP4_ASSERT(LINE, ci, ai, da.numBlocksTotal(), !da.numBlocksTotal());
    }

    if (1)
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);

        if (PRINT) cout <<
            "\t\tremoveRows(int startIndex, int rowCount);" << endl;

        Obj mX(EE, MODE, &testAllocator);  const Obj &X = mX;

        if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
        mX.removeRows(DI, NE);
        if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
        LOOP3_ASSERT(LINE, ci, ai, DD == X);

        LOOP4_ASSERT(LINE, ci, ai, da.numBlocksTotal(), !da.numBlocksTotal());
    }
 // ^-------------------v
                    } // end for each allocation mode
                } // end for each column-config spec
            } // end for each test vector
        } // end block scope

        //---------------------------------------------------------------------

        if (verbose) cout << "\nTesting ALIAS case." << endl;
        {
            static const struct {
                int         d_lineNum; // source line number
                const char *d_xSpec;   // initial table value, xt
                int         d_di;      // index at which to insert into xl
                int         d_si;      // index at which to insert from xl
                int         d_ne;      // number of elements to insert
                const char *d_eSpec;   // expected final value of dl

            } DATA[] = {
                //line  x-object  di  si  ne  expected
                //----  --------  --  --  --  --------        Depth = 0
                { L_,   "",       0,  0,  0,  ""           },

                //line  x-object  di  si  ne  expected
                //----  --------  --  --  --  --------        Depth = 1
                { L_,   "A",      0,  0,  0,  "A"          },
                { L_,   "A",      0,  0,  1,  "AA"         },
                { L_,   "A",      0,  1,  0,  "A"          },

                { L_,   "A",      1,  0,  0,  "A"          },
                { L_,   "A",      1,  0,  1,  "AA"         },
                { L_,   "A",      1,  1,  0,  "A"          },

                //line  x-object  di  si  ne  expected
                //----  --------  --  --  --  --------        Depth = 2
                { L_,   "AB",     0,  0,  0,  "AB"         },
                { L_,   "AB",     0,  0,  1,  "AAB"        },
                { L_,   "AB",     0,  0,  2,  "ABAB"       },
                { L_,   "AB",     0,  1,  0,  "AB"         },
                { L_,   "AB",     0,  1,  1,  "BAB"        },
                { L_,   "AB",     0,  2,  0,  "AB"         },

                { L_,   "AB",     1,  0,  0,  "AB"         },
                { L_,   "AB",     1,  0,  1,  "AAB"        },
                { L_,   "AB",     1,  0,  2,  "AABB"       },
                { L_,   "AB",     1,  1,  0,  "AB"         },
                { L_,   "AB",     1,  1,  1,  "ABB"        },
                { L_,   "AB",     1,  2,  0,  "AB"         },

                { L_,   "AB",     2,  0,  0,  "AB"         },
                { L_,   "AB",     2,  0,  1,  "ABA"        },
                { L_,   "AB",     2,  0,  2,  "ABAB"       },
                { L_,   "AB",     2,  1,  0,  "AB"         },
                { L_,   "AB",     2,  1,  1,  "ABB"        },
                { L_,   "AB",     2,  2,  0,  "AB"         },

                //line  x-object  di  si  ne  expected
                //----  --------  --  --  --  --------        Depth = 3
                { L_,   "ABC",    0,  0,  0,  "ABC"        },
                { L_,   "ABC",    0,  0,  1,  "AABC"       },
                { L_,   "ABC",    0,  0,  2,  "ABABC"      },
                { L_,   "ABC",    0,  0,  3,  "ABCABC"     },
                { L_,   "ABC",    0,  1,  0,  "ABC"        },
                { L_,   "ABC",    0,  1,  1,  "BABC"       },
                { L_,   "ABC",    0,  1,  2,  "BCABC"      },
                { L_,   "ABC",    0,  2,  0,  "ABC"        },
                { L_,   "ABC",    0,  2,  1,  "CABC"       },
                { L_,   "ABC",    0,  3,  0,  "ABC"        },

                { L_,   "ABC",    1,  0,  0,  "ABC"        },
                { L_,   "ABC",    1,  0,  1,  "AABC"       },
                { L_,   "ABC",    1,  0,  2,  "AABBC"      },
                { L_,   "ABC",    1,  0,  3,  "AABCBC"     },
                { L_,   "ABC",    1,  1,  0,  "ABC"        },
                { L_,   "ABC",    1,  1,  1,  "ABBC"       },
                { L_,   "ABC",    1,  1,  2,  "ABCBC"      },
                { L_,   "ABC",    1,  2,  0,  "ABC"        },
                { L_,   "ABC",    1,  2,  1,  "ACBC"       },
                { L_,   "ABC",    1,  3,  0,  "ABC"        },

                { L_,   "ABC",    2,  0,  0,  "ABC"        },
                { L_,   "ABC",    2,  0,  1,  "ABAC"       },
                { L_,   "ABC",    2,  0,  2,  "ABABC"      },
                { L_,   "ABC",    2,  0,  3,  "ABABCC"     },
                { L_,   "ABC",    2,  1,  0,  "ABC"        },
                { L_,   "ABC",    2,  1,  1,  "ABBC"       },
                { L_,   "ABC",    2,  1,  2,  "ABBCC"      },
                { L_,   "ABC",    2,  2,  0,  "ABC"        },
                { L_,   "ABC",    2,  2,  1,  "ABCC"       },
                { L_,   "ABC",    2,  3,  0,  "ABC"        },

                { L_,   "ABC",    3,  0,  0,  "ABC"        },
                { L_,   "ABC",    3,  0,  1,  "ABCA"       },
                { L_,   "ABC",    3,  0,  2,  "ABCAB"      },
                { L_,   "ABC",    3,  0,  3,  "ABCABC"     },
                { L_,   "ABC",    3,  1,  0,  "ABC"        },
                { L_,   "ABC",    3,  1,  1,  "ABCB"       },
                { L_,   "ABC",    3,  1,  2,  "ABCBC"      },
                { L_,   "ABC",    3,  2,  0,  "ABC"        },
                { L_,   "ABC",    3,  2,  1,  "ABCC"       },
                { L_,   "ABC",    3,  3,  0,  "ABC"        },

                //line  x-object  di  si  ne  expected
                //----  --------  --  --  --  --------        Depth = 4
                { L_,   "ABCD",   0,  0,  0,  "ABCD"       },
                { L_,   "ABCD",   0,  0,  1,  "AABCD"      },
                { L_,   "ABCD",   0,  0,  2,  "ABABCD"     },
                { L_,   "ABCD",   0,  0,  3,  "ABCABCD"    },
                { L_,   "ABCD",   0,  0,  4,  "ABCDABCD"   },
                { L_,   "ABCD",   0,  1,  0,  "ABCD"       },
                { L_,   "ABCD",   0,  1,  1,  "BABCD"      },
                { L_,   "ABCD",   0,  1,  2,  "BCABCD"     },
                { L_,   "ABCD",   0,  1,  3,  "BCDABCD"    },
                { L_,   "ABCD",   0,  2,  0,  "ABCD"       },
                { L_,   "ABCD",   0,  2,  1,  "CABCD"      },
                { L_,   "ABCD",   0,  2,  2,  "CDABCD"     },
                { L_,   "ABCD",   0,  3,  0,  "ABCD"       },
                { L_,   "ABCD",   0,  3,  1,  "DABCD"      },
                { L_,   "ABCD",   0,  4,  0,  "ABCD"       },

                { L_,   "ABCD",   1,  0,  0,  "ABCD"       },
                { L_,   "ABCD",   1,  0,  1,  "AABCD"      },
                { L_,   "ABCD",   1,  0,  2,  "AABBCD"     },
                { L_,   "ABCD",   1,  0,  3,  "AABCBCD"    },
                { L_,   "ABCD",   1,  0,  4,  "AABCDBCD"   },
                { L_,   "ABCD",   1,  1,  0,  "ABCD"       },
                { L_,   "ABCD",   1,  1,  1,  "ABBCD"      },
                { L_,   "ABCD",   1,  1,  2,  "ABCBCD"     },
                { L_,   "ABCD",   1,  1,  3,  "ABCDBCD"    },
                { L_,   "ABCD",   1,  2,  0,  "ABCD"       },
                { L_,   "ABCD",   1,  2,  1,  "ACBCD"      },
                { L_,   "ABCD",   1,  2,  2,  "ACDBCD"     },
                { L_,   "ABCD",   1,  3,  0,  "ABCD"       },
                { L_,   "ABCD",   1,  3,  1,  "ADBCD"      },
                { L_,   "ABCD",   1,  4,  0,  "ABCD"       },

                { L_,   "ABCD",   2,  0,  0,  "ABCD"       },
                { L_,   "ABCD",   2,  0,  1,  "ABACD"      },
                { L_,   "ABCD",   2,  0,  2,  "ABABCD"     },
                { L_,   "ABCD",   2,  0,  3,  "ABABCCD"    },
                { L_,   "ABCD",   2,  0,  4,  "ABABCDCD"   },
                { L_,   "ABCD",   2,  1,  0,  "ABCD"       },
                { L_,   "ABCD",   2,  1,  1,  "ABBCD"      },
                { L_,   "ABCD",   2,  1,  2,  "ABBCCD"     },
                { L_,   "ABCD",   2,  1,  3,  "ABBCDCD"    },
                { L_,   "ABCD",   2,  2,  0,  "ABCD"       },
                { L_,   "ABCD",   2,  2,  1,  "ABCCD"      },
                { L_,   "ABCD",   2,  2,  2,  "ABCDCD"     },
                { L_,   "ABCD",   2,  3,  0,  "ABCD"       },
                { L_,   "ABCD",   2,  3,  1,  "ABDCD"      },
                { L_,   "ABCD",   2,  4,  0,  "ABCD"       },

                { L_,   "ABCD",   3,  0,  0,  "ABCD"       },
                { L_,   "ABCD",   3,  0,  1,  "ABCAD"      },
                { L_,   "ABCD",   3,  0,  2,  "ABCABD"     },
                { L_,   "ABCD",   3,  0,  3,  "ABCABCD"    },
                { L_,   "ABCD",   3,  0,  4,  "ABCABCDD"   },
                { L_,   "ABCD",   3,  1,  0,  "ABCD"       },
                { L_,   "ABCD",   3,  1,  1,  "ABCBD"      },
                { L_,   "ABCD",   3,  1,  2,  "ABCBCD"     },
                { L_,   "ABCD",   3,  1,  3,  "ABCBCDD"    },
                { L_,   "ABCD",   3,  2,  0,  "ABCD"       },
                { L_,   "ABCD",   3,  2,  1,  "ABCCD"      },
                { L_,   "ABCD",   3,  2,  2,  "ABCCDD"     },
                { L_,   "ABCD",   3,  3,  0,  "ABCD"       },
                { L_,   "ABCD",   3,  3,  1,  "ABCDD"      },
                { L_,   "ABCD",   3,  4,  0,  "ABCD"       },

                { L_,   "ABCD",   4,  0,  0,  "ABCD"       },
                { L_,   "ABCD",   4,  0,  1,  "ABCDA"      },
                { L_,   "ABCD",   4,  0,  2,  "ABCDAB"     },
                { L_,   "ABCD",   4,  0,  3,  "ABCDABC"    },
                { L_,   "ABCD",   4,  0,  4,  "ABCDABCD"   },
                { L_,   "ABCD",   4,  1,  0,  "ABCD"       },
                { L_,   "ABCD",   4,  1,  1,  "ABCDB"      },
                { L_,   "ABCD",   4,  1,  2,  "ABCDBC"     },
                { L_,   "ABCD",   4,  1,  3,  "ABCDBCD"    },
                { L_,   "ABCD",   4,  2,  0,  "ABCD"       },
                { L_,   "ABCD",   4,  2,  1,  "ABCDC"      },
                { L_,   "ABCD",   4,  2,  2,  "ABCDCD"     },
                { L_,   "ABCD",   4,  3,  0,  "ABCD"       },
                { L_,   "ABCD",   4,  3,  1,  "ABCDD"      },
                { L_,   "ABCD",   4,  4,  0,  "ABCD"       },

                //line  x-object  di  si  ne  expected
                //----  --------  --  --  --  --------        Depth = 5
                { L_,   "ABCDE",  0,  0,  0,  "ABCDE"      },
                { L_,   "ABCDE",  0,  0,  1,  "AABCDE"     },
                { L_,   "ABCDE",  0,  0,  2,  "ABABCDE"    },
                { L_,   "ABCDE",  0,  0,  3,  "ABCABCDE"   },
                { L_,   "ABCDE",  0,  0,  4,  "ABCDABCDE"  },
                { L_,   "ABCDE",  0,  0,  5,  "ABCDEABCDE" },
                { L_,   "ABCDE",  0,  1,  0,  "ABCDE"      },
                { L_,   "ABCDE",  0,  1,  1,  "BABCDE"     },
                { L_,   "ABCDE",  0,  1,  2,  "BCABCDE"    },
                { L_,   "ABCDE",  0,  1,  3,  "BCDABCDE"   },
                { L_,   "ABCDE",  0,  1,  4,  "BCDEABCDE"  },
                { L_,   "ABCDE",  0,  2,  0,  "ABCDE"      },
                { L_,   "ABCDE",  0,  2,  1,  "CABCDE"     },
                { L_,   "ABCDE",  0,  2,  2,  "CDABCDE"    },
                { L_,   "ABCDE",  0,  2,  3,  "CDEABCDE"   },
                { L_,   "ABCDE",  0,  3,  0,  "ABCDE"      },
                { L_,   "ABCDE",  0,  3,  1,  "DABCDE"     },
                { L_,   "ABCDE",  0,  3,  2,  "DEABCDE"    },
                { L_,   "ABCDE",  0,  4,  0,  "ABCDE"      },
                { L_,   "ABCDE",  0,  4,  1,  "EABCDE"     },
                { L_,   "ABCDE",  0,  5,  0,  "ABCDE"      },

                { L_,   "ABCDE",  1,  0,  0,  "ABCDE"      },
                { L_,   "ABCDE",  1,  0,  1,  "AABCDE"     },
                { L_,   "ABCDE",  1,  0,  2,  "AABBCDE"    },
                { L_,   "ABCDE",  1,  0,  3,  "AABCBCDE"   },
                { L_,   "ABCDE",  1,  0,  4,  "AABCDBCDE"  },
                { L_,   "ABCDE",  1,  0,  5,  "AABCDEBCDE" },
                { L_,   "ABCDE",  1,  1,  0,  "ABCDE"      },
                { L_,   "ABCDE",  1,  1,  1,  "ABBCDE"     },
                { L_,   "ABCDE",  1,  1,  2,  "ABCBCDE"    },
                { L_,   "ABCDE",  1,  1,  3,  "ABCDBCDE"   },
                { L_,   "ABCDE",  1,  1,  4,  "ABCDEBCDE"  },
                { L_,   "ABCDE",  1,  2,  0,  "ABCDE"      },
                { L_,   "ABCDE",  1,  2,  1,  "ACBCDE"     },
                { L_,   "ABCDE",  1,  2,  2,  "ACDBCDE"    },
                { L_,   "ABCDE",  1,  2,  3,  "ACDEBCDE"   },
                { L_,   "ABCDE",  1,  3,  0,  "ABCDE"      },
                { L_,   "ABCDE",  1,  3,  1,  "ADBCDE"     },
                { L_,   "ABCDE",  1,  3,  2,  "ADEBCDE"    },
                { L_,   "ABCDE",  1,  4,  0,  "ABCDE"      },
                { L_,   "ABCDE",  1,  4,  1,  "AEBCDE"     },
                { L_,   "ABCDE",  1,  5,  0,  "ABCDE"      },

                { L_,   "ABCDE",  2,  0,  0,  "ABCDE"      },
                { L_,   "ABCDE",  2,  0,  1,  "ABACDE"     },
                { L_,   "ABCDE",  2,  0,  2,  "ABABCDE"    },
                { L_,   "ABCDE",  2,  0,  3,  "ABABCCDE"   },
                { L_,   "ABCDE",  2,  0,  4,  "ABABCDCDE"  },
                { L_,   "ABCDE",  2,  0,  5,  "ABABCDECDE" },
                { L_,   "ABCDE",  2,  1,  0,  "ABCDE"      },
                { L_,   "ABCDE",  2,  1,  1,  "ABBCDE"     },
                { L_,   "ABCDE",  2,  1,  2,  "ABBCCDE"    },
                { L_,   "ABCDE",  2,  1,  3,  "ABBCDCDE"   },
                { L_,   "ABCDE",  2,  1,  4,  "ABBCDECDE"  },
                { L_,   "ABCDE",  2,  2,  0,  "ABCDE"      },
                { L_,   "ABCDE",  2,  2,  1,  "ABCCDE"     },
                { L_,   "ABCDE",  2,  2,  2,  "ABCDCDE"    },
                { L_,   "ABCDE",  2,  2,  3,  "ABCDECDE"   },
                { L_,   "ABCDE",  2,  3,  0,  "ABCDE"      },
                { L_,   "ABCDE",  2,  3,  1,  "ABDCDE"     },
                { L_,   "ABCDE",  2,  3,  2,  "ABDECDE"    },
                { L_,   "ABCDE",  2,  4,  0,  "ABCDE"      },
                { L_,   "ABCDE",  2,  4,  1,  "ABECDE"     },
                { L_,   "ABCDE",  2,  5,  0,  "ABCDE"      },

                { L_,   "ABCDE",  3,  0,  0,  "ABCDE"      },
                { L_,   "ABCDE",  3,  0,  1,  "ABCADE"     },
                { L_,   "ABCDE",  3,  0,  2,  "ABCABDE"    },
                { L_,   "ABCDE",  3,  0,  3,  "ABCABCDE"   },
                { L_,   "ABCDE",  3,  0,  4,  "ABCABCDDE"  },
                { L_,   "ABCDE",  3,  0,  5,  "ABCABCDEDE" },
                { L_,   "ABCDE",  3,  1,  0,  "ABCDE"      },
                { L_,   "ABCDE",  3,  1,  1,  "ABCBDE"     },
                { L_,   "ABCDE",  3,  1,  2,  "ABCBCDE"    },
                { L_,   "ABCDE",  3,  1,  3,  "ABCBCDDE"   },
                { L_,   "ABCDE",  3,  1,  4,  "ABCBCDEDE"  },
                { L_,   "ABCDE",  3,  2,  0,  "ABCDE"      },
                { L_,   "ABCDE",  3,  2,  1,  "ABCCDE"     },
                { L_,   "ABCDE",  3,  2,  2,  "ABCCDDE"    },
                { L_,   "ABCDE",  3,  2,  3,  "ABCCDEDE"   },
                { L_,   "ABCDE",  3,  3,  0,  "ABCDE"      },
                { L_,   "ABCDE",  3,  3,  1,  "ABCDDE"     },
                { L_,   "ABCDE",  3,  3,  2,  "ABCDEDE"    },
                { L_,   "ABCDE",  3,  4,  0,  "ABCDE"      },
                { L_,   "ABCDE",  3,  4,  1,  "ABCEDE"     },
                { L_,   "ABCDE",  3,  5,  0,  "ABCDE"      },

                { L_,   "ABCDE",  4,  0,  0,  "ABCDE"      },
                { L_,   "ABCDE",  4,  0,  1,  "ABCDAE"     },
                { L_,   "ABCDE",  4,  0,  2,  "ABCDABE"    },
                { L_,   "ABCDE",  4,  0,  3,  "ABCDABCE"   },
                { L_,   "ABCDE",  4,  0,  4,  "ABCDABCDE"  },
                { L_,   "ABCDE",  4,  0,  5,  "ABCDABCDEE" },
                { L_,   "ABCDE",  4,  1,  0,  "ABCDE"      },
                { L_,   "ABCDE",  4,  1,  1,  "ABCDBE"     },
                { L_,   "ABCDE",  4,  1,  2,  "ABCDBCE"    },
                { L_,   "ABCDE",  4,  1,  3,  "ABCDBCDE"   },
                { L_,   "ABCDE",  4,  1,  4,  "ABCDBCDEE"  },
                { L_,   "ABCDE",  4,  2,  0,  "ABCDE"      },
                { L_,   "ABCDE",  4,  2,  1,  "ABCDCE"     },
                { L_,   "ABCDE",  4,  2,  2,  "ABCDCDE"    },
                { L_,   "ABCDE",  4,  2,  3,  "ABCDCDEE"   },
                { L_,   "ABCDE",  4,  3,  0,  "ABCDE"      },
                { L_,   "ABCDE",  4,  3,  1,  "ABCDDE"     },
                { L_,   "ABCDE",  4,  3,  2,  "ABCDDEE"    },
                { L_,   "ABCDE",  4,  4,  0,  "ABCDE"      },
                { L_,   "ABCDE",  4,  4,  1,  "ABCDEE"     },
                { L_,   "ABCDE",  4,  5,  0,  "ABCDE"      },

                { L_,   "ABCDE",  5,  0,  0,  "ABCDE"      },
                { L_,   "ABCDE",  5,  0,  1,  "ABCDEA"     },
                { L_,   "ABCDE",  5,  0,  2,  "ABCDEAB"    },
                { L_,   "ABCDE",  5,  0,  3,  "ABCDEABC"   },
                { L_,   "ABCDE",  5,  0,  4,  "ABCDEABCD"  },
                { L_,   "ABCDE",  5,  0,  5,  "ABCDEABCDE" },
                { L_,   "ABCDE",  5,  1,  0,  "ABCDE"      },
                { L_,   "ABCDE",  5,  1,  1,  "ABCDEB"     },
                { L_,   "ABCDE",  5,  1,  2,  "ABCDEBC"    },
                { L_,   "ABCDE",  5,  1,  3,  "ABCDEBCD"   },
                { L_,   "ABCDE",  5,  1,  4,  "ABCDEBCDE"  },
                { L_,   "ABCDE",  5,  2,  0,  "ABCDE"      },
                { L_,   "ABCDE",  5,  2,  1,  "ABCDEC"     },
                { L_,   "ABCDE",  5,  2,  2,  "ABCDECD"    },
                { L_,   "ABCDE",  5,  2,  3,  "ABCDECDE"   },
                { L_,   "ABCDE",  5,  3,  0,  "ABCDE"      },
                { L_,   "ABCDE",  5,  3,  1,  "ABCDED"     },
                { L_,   "ABCDE",  5,  3,  2,  "ABCDEDE"    },
                { L_,   "ABCDE",  5,  4,  0,  "ABCDE"      },
                { L_,   "ABCDE",  5,  4,  1,  "ABCDEE"     },
                { L_,   "ABCDE",  5,  5,  0,  "ABCDE"      },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const bdem_AggregateOption::AllocationStrategy MODES[] = {
                bdem_AggregateOption::BDEM_PASS_THROUGH,
                bdem_AggregateOption::BDEM_WRITE_ONCE,
                bdem_AggregateOption::BDEM_WRITE_MANY,
            };
            const int NUM_MODES = sizeof MODES / sizeof *MODES;

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *X_SPEC = DATA[ti].d_xSpec;
                const int   DI     = DATA[ti].d_di;
                const int   SI     = DATA[ti].d_si;
                const int   NE     = DATA[ti].d_ne;
                const char *E_SPEC = DATA[ti].d_eSpec;

                const int   DEPTH  = (int)strlen(X_SPEC);
                if (DEPTH != oldDepth) {
                    oldDepth = DEPTH;
                    if (verbose) { cout << '\t';  P(DEPTH); }
                }

                for (int ci = 0; ci < NUM_CONFIGS; ++ci) { // CONFIG INDEX

                    // Parse col config to create the sequence of column types.

                    const char *const COLUMN_CONFIG_SPEC = CONFIGS[ci];

                    vector<bdem_ElemType::Type> columnTypes;
                    {
                        for (const char *s = COLUMN_CONFIG_SPEC; *s; ++s) {
                            columnTypes.push_back(getElemType(*s));
                        }
                    }

                    Obj mXX(columnTypes); const Obj& XX = mXX;
                    Obj mEE(columnTypes); const Obj& EE = mEE;
                    {
                        for (const char *s = X_SPEC; *s; ++s) {
                            gg(&mXX, COLUMN_CONFIG_SPEC, T[*s - 'A']);
                        }
                        LOOP2_ASSERT(ti, ci,
                                     (int)strlen(X_SPEC) == XX.numRows());
                    }
                    {
                        for (const char *s = E_SPEC; *s; ++s) {
                            gg(&mEE, COLUMN_CONFIG_SPEC, T[*s - 'A']);
                        }
                        LOOP2_ASSERT(ti, ci,
                                     (int)strlen(E_SPEC) == EE.numRows());
                    }

                    if (veryVerbose && !ci || veryVeryVerbose) {
                        cout << "\t  =================================="
                                    "=================================="
                        << endl;

                        cout << "\t  "; P_(X_SPEC); P_(DI);
                                                    P_(SI); P_(NE); P(E_SPEC);
                        cout << "\t  "; P_(ci); P(COLUMN_CONFIG_SPEC);
                        cout << "\t\t"; P(XX);
                        cout << "\t\t"; P(EE);
                    }

                    for (int ai = 0; ai < NUM_MODES; ++ai) { // ALLOCATION MODE

                        const bool PRINT = veryVerbose && !ci && !ai
                                        || veryVeryVerbose;

                        const bdem_AggregateOption::AllocationStrategy
                                                              MODE = MODES[ai];
                        if (PRINT) {
                            cout << "\t  ----------------------------------"
                                        "----------------------------------"
                            << endl;
                            cout << "\t  "; P(MODE);
                        }
 // v-------------------^
    if (DI == XX.numRows() && 1 == NE)
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);

        if (PRINT) cout <<
            "\t\tappendRow(const bdem_Table& srcTable, int srcIndex);" << endl;

        Obj mX(XX, MODE, &testAllocator);  const Obj &X = mX;
        if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
        mX.appendRow(X, SI);
        if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
        LOOP3_ASSERT(LINE, ci, ai, EE == X);

//      LOOP4_ASSERT(LINE, ci, ai, da.numBlocksTotal(), !da.numBlocksTotal());
    }

    if (DI == XX.numRows() && XX.numRows() == NE)
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);

        if (PRINT) cout <<
            "\t\tappendRows(const bdem_Table& srcTable);" << endl;

        Obj mX(XX, MODE, &testAllocator);  const Obj &X = mX;
        if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
        mX.appendRows(X);
        if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
        LOOP3_ASSERT(LINE, ci, ai, EE == X);

        LOOP4_ASSERT(LINE, ci, ai, da.numBlocksInUse(), !da.numBlocksInUse());
    }

    if (DI == XX.numRows())
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);

        if (PRINT) cout <<
            "\t\tappendRows(const bdem_Table& st, int srcIndex, int rowCount);"
                                                                       << endl;
        Obj mX(XX, MODE, &testAllocator);  const Obj &X = mX;
        if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
        mX.appendRows(X, SI, NE);
        if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
        LOOP3_ASSERT(LINE, ci, ai, EE == X);

        LOOP4_ASSERT(LINE, ci, ai, da.numBlocksInUse(), !da.numBlocksInUse());
    }

    if (1 == NE)
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);

        if (PRINT) cout <<
            "\t\tinsertRow(int dstIndex, const bdem_Table& st, int srcIndex);"
                                                                       << endl;
        Obj mX(XX, MODE, &testAllocator);  const Obj &X = mX;
        if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
        mX.insertRow(DI, X, SI);
        if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
        LOOP3_ASSERT(LINE, ci, ai, EE == X);

//      LOOP4_ASSERT(LINE, ci, ai, da.numBlocksTotal(), !da.numBlocksTotal());
    }

    if (XX.numRows() == NE)
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);

        if (PRINT) cout <<
            "\t\tinsertRows(int dstIndex, const bdem_Table& srcTable);"<< endl;

        Obj mX(XX, MODE, &testAllocator);  const Obj &X = mX;
        if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
        mX.insertRows(DI, X);
        if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
        LOOP3_ASSERT(LINE, ci, ai, EE == X);

        LOOP4_ASSERT(LINE, ci, ai, da.numBlocksInUse(), !da.numBlocksInUse());
    }

    if (1)
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);

        if (PRINT) cout <<
            "\t\tinsertRows(int di, const bdem_Table& st, int si, int rc);"
                                                                       << endl;
        Obj mX(XX, MODE, &testAllocator);  const Obj &X = mX;
        if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
        mX.insertRows(DI, X, SI, NE);
        if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
        LOOP3_ASSERT(LINE, ci, ai, EE == X);

        LOOP4_ASSERT(LINE, ci, ai, da.numBlocksInUse(), !da.numBlocksInUse());
    }
 // ^-------------------v
                    } // end for each allocation mode
                } // end for each column-config spec
            } // end for each test vector
        } // end block scope
      }

DEFINE_TEST_CASE(11) {
        // --------------------------------------------------------------------
        // NON-PRIMARY CONSTRUCTORS AND DESTRUCTOR.
        //
        // Concerns:
        //   - That the specified or bslma_Default::defaultAllocator is used.
        //   - That the specified or default ('BDEM_PASS_THROUGH') strategy is
        //     used.
        //   - That the value is initialized properly.
        //   - That the destructor cleans up properly.
        //
        // Plan:
        //   - Use 'bslma_TestAllocator' to verify that specified allocator
        //      is used.
        //   - Install a 'bslma_TestAllocator' in 'bdema_DefaultAllocator' to
        //      verify that 'bslma_Default::defaultAllocator()' is used by
        //      default.
        //   - For each constructor, create four instances: the three
        //      strategies other than 'BDEM_SUBORDINATE' and one with the
        //      strategy omitted.  Verify that 'BDEM_PASS_THROUGH' and
        //      "not specified" have the same allocation pattern.
        //   - Throughout this test case, confirm that each constructors
        //      produces the correct value.
        //   - Allow the destructor to execute in essentially all relevant
        //      white-box states (and asserts all appropriate invariants).
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //   - Brute-Force and Loop-Based Implementation Techniques
        //
        // Testing:
        //   bdem_Table(bdem_AggregateOption::AllocationStrategy am, ba = 0);
        //
        //   bdem_Table(const ET:T et[], int ne, *ba = 0);
        //   bdem_Table(const ET:T et[], int ne, am, *ba = 0);
        //
        //   bdem_Table(const bsl::vector<ET:T>& et, *ba = 0);
        //   bdem_Table(const bsl::vector<ET:T>& et, am, *ba = 0);
        //
        //   bdem_Table(const bdem_Table& original, am, ba = 0);
        //
        //   ~bdem_Table();
        // --------------------------------------------------------------------

        bslma_TestAllocator defaultAllocator;
        const bslma_DefaultAllocatorGuard dag(&defaultAllocator);

        if (verbose) cout << endl
                          << "NON-PRIMARY CONSTRUCTORS AND DESTRUCTOR" << endl
                          << "=======================================" << endl;

        // ---------------- VERIFY SPECIFIED ALLOCATOR IS USED ----------------

        if (verbose) cout << "\nVerify that the specified allocator is used."
                                                                       << endl;

        // The following variables are used to compare usage between the
        // explicit allocator via 'BDEM_PASS_THROUGH' and the one supplied by
        // default.

        int usage1_one,   usage2_one;    // bdem_Table(am, ba);
        int usage1_two,   usage2_two;    // bdem_Table(et, ne, ba);
        int usage1_three, usage2_three;  // bdem_Table(et, ne, am, ba);
        int usage1_four,  usage2_four;   // bdem_Table(original, am, ba);
        int usage1_five,  usage2_five;   // bdem_Table(et, ba);
        int usage1_six,   usage2_six;    // bdem_Table(et, am, ba);

        // - - - - - - - - - - - - - - - ONE - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tONE: bdem_Table(am, ba);" << endl;
        {
            bslma_TestAllocator a;
            int usage0 = a.numBlocksTotal();
            if (veryVerbose) { T_ T_ P(usage0) }
            ASSERT(0 == usage0);

            bdem_Table table(bdem_AggregateOption::BDEM_PASS_THROUGH, &a);
            int usage1 = a.numBlocksTotal();
            if (veryVerbose) { T_ T_ P(usage1) }
            ASSERT(usage1 > usage0);

            bdem_ElemType::Type t = bdem_ElemType::BDEM_STRING;
            table.reset(&t, 1);
            table.appendNullRow();
            table.theModifiableRow(0)[0].theModifiableString() = A06;

            int usage2 = a.numBlocksTotal();
            if (veryVerbose) { T_ T_ P(usage2) }
            ASSERT(usage2 > usage1);

            ASSERT(1 == table.numRows());         // Confirm value.
            ASSERT(1 == table.numColumns());
            ASSERT(A06 == table.theModifiableRow(0)[0].theString());

            usage1_one = usage1; // Record usage for comparison with
            usage2_one = usage2; // allocations from the default allocator.
        }

        // - - - - - - - - - - - - - - - TWO - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tTWO: bdem_Table(et, ne, ba);" << endl;
        {
            const bdem_ElemType::Type TYPES[] = {
                bdem_ElemType::BDEM_SHORT,
                bdem_ElemType::BDEM_INT,
                bdem_ElemType::BDEM_STRING
            };
            const int N = sizeof TYPES / sizeof *TYPES;

            bslma_TestAllocator a;
            int usage0 = a.numBlocksTotal();
            if (veryVerbose) { T_ T_ P(usage0) }
            ASSERT(0 == usage0);

            bdem_Table table(TYPES, N, &a);
            int usage1 = a.numBlocksTotal();
            if (veryVerbose) { T_ T_ P(usage1) }
            ASSERT(usage1 > usage0);

            table.appendNullRow();
            int usage2 = a.numBlocksTotal();
            if (veryVerbose) { T_ T_ P(usage2) }
            ASSERT(usage2 > usage1);

            ASSERT(1 == table.numRows());         // Confirm value.
            ASSERT(3 == table.numColumns());
            ASSERT(U01 == table.theModifiableRow(0)[0].theShort());
            ASSERT(U02 == table.theModifiableRow(0)[1].theInt());
            ASSERT(U06 == table.theModifiableRow(0)[2].theString());

            usage1_two = usage1; // Record usage for comparison with
            usage2_two = usage2; // allocations from the default allocator.
        }

        // - - - - - - - - - - - - - - - THREE - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tTHREE: bdem_Table(et, ne, am, ba);" << endl;
        {
            const bdem_ElemType::Type TYPES[] = {
                bdem_ElemType::BDEM_SHORT,
                bdem_ElemType::BDEM_INT,
                bdem_ElemType::BDEM_STRING
            };
            const int N = sizeof TYPES / sizeof *TYPES;

            bslma_TestAllocator a;
            int usage0 = a.numBlocksTotal();
            if (veryVerbose) { T_ T_ P(usage0) }
            ASSERT(0 == usage0);

            bdem_Table table(TYPES,
                             N,
                             bdem_AggregateOption::BDEM_PASS_THROUGH,
                             &a);

            int usage1 = a.numBlocksTotal();
            if (veryVerbose) { T_ T_ P(usage1) }
            ASSERT(usage1 > usage0);

            table.appendNullRow();
            int usage2 = a.numBlocksTotal();
            if (veryVerbose) { T_ T_ P(usage2) }
            ASSERT(usage2 > usage1);

            ASSERT(1 == table.numRows());         // Confirm value.
            ASSERT(3 == table.numColumns());
            ASSERT(U01 == table.theModifiableRow(0)[0].theShort());
            ASSERT(U02 == table.theModifiableRow(0)[1].theInt());
            ASSERT(U06 == table.theModifiableRow(0)[2].theString());

            usage1_three = usage1; // Record usage for comparison with
            usage2_three = usage2; // allocations from the default allocator.

            // The specified allocator should have the same usage whether the
            // allocation strategy is allowed to default to 'BDEM_PASS_THROUGH'
            // (two) or 'BDEM_PASS_THROUGH' is specified explicitly (three).

            LOOP2_ASSERT(usage1_two, usage1_three, usage1_two == usage1_three);
            LOOP2_ASSERT(usage2_two, usage2_three, usage2_two == usage2_three);
        }

        // - - - - - - - - - - - - - - - FOUR - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tFOUR: bdem_Table(original, am, ba);"
                          << endl;
        {
            bslma_TestAllocator aA;                // A: original table
            bslma_TestAllocator aB;                // B: copy constructed table

            int usageA0 = aA.numBlocksTotal();
            int usageB0 = aB.numBlocksTotal();

            if (veryVerbose) { T_ T_ P_(usageA0) P(usageB0) }
            ASSERT(0 == usageA0); ASSERT(0 == usageB0);

            bdem_Table tableA(bdem_AggregateOption::BDEM_PASS_THROUGH, &aA);
            bdem_Table tableB(tableA,
                              bdem_AggregateOption::BDEM_PASS_THROUGH,
                              &aB);

            int usageA1 = aA.numBlocksTotal();
            int usageB1 = aB.numBlocksTotal();

            if (veryVerbose) { T_ T_ P_(usageA1) P(usageB1) }

            ASSERT(usageA1 > 0); ASSERT(usageB1 > 0);

            bdem_ElemType::Type t = bdem_ElemType::BDEM_STRING;
            tableB.reset(&t, 1);
            tableB.appendNullRow();
            tableB.theModifiableRow(0)[0].theModifiableString() = A06;

            int usageA2 = aA.numBlocksTotal();
            int usageB2 = aB.numBlocksTotal();

            if (veryVerbose) { T_ T_ P_(usageA2) P(usageB2) }

            ASSERT(usageA1 == usageA2);
            ASSERT(usageB1  < usageB2);

            ASSERT(1 == tableB.numRows());         // Confirm value.
            ASSERT(1 == tableB.numColumns());
            ASSERT(A06 == tableB.theModifiableRow(0)[0].theString());

            usage1_four = usageB1; // Record usage for comparison with
            usage2_four = usageB2; // allocations from the default allocator.
        }

        // - - - - - - - - - - - - - - - FIVE - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tFIVE: bdem_Table(et, ba);" << endl;
        {
            bsl::vector<bdem_ElemType::Type> TYPES;
            TYPES.push_back(bdem_ElemType::BDEM_SHORT);
            TYPES.push_back(bdem_ElemType::BDEM_INT);
            TYPES.push_back(bdem_ElemType::BDEM_STRING);

            bslma_TestAllocator a;
            int usage0 = a.numBlocksTotal();
            if (veryVerbose) { T_ T_ P(usage0) }
            ASSERT(0 == usage0);

            bdem_Table table(TYPES, &a);
            int usage1 = a.numBlocksTotal();
            if (veryVerbose) { T_ T_ P(usage1) }
            ASSERT(usage1 > usage0);

            table.appendNullRow();
            int usage2 = a.numBlocksTotal();
            if (veryVerbose) { T_ T_ P(usage2) }
            ASSERT(usage2 > usage1);

            ASSERT(1 == table.numRows());         // Confirm value.
            ASSERT(3 == table.numColumns());
            ASSERT(U01 == table.theModifiableRow(0)[0].theShort());
            ASSERT(U02 == table.theModifiableRow(0)[1].theInt());
            ASSERT(U06 == table.theModifiableRow(0)[2].theString());

            usage1_five = usage1; // Record usage for comparison with
            usage2_five = usage2; // allocations from the default allocator.
        }

        // - - - - - - - - - - - - - - - SIX - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tSIX: bdem_Table(et, am, ba);" << endl;
        {
            bsl::vector<bdem_ElemType::Type> TYPES;
            TYPES.push_back(bdem_ElemType::BDEM_SHORT);
            TYPES.push_back(bdem_ElemType::BDEM_INT);
            TYPES.push_back(bdem_ElemType::BDEM_STRING);

            bslma_TestAllocator a;
            int usage0 = a.numBlocksTotal();
            if (veryVerbose) { T_ T_ P(usage0) }
            ASSERT(0 == usage0);

            bdem_Table table(TYPES,
                             bdem_AggregateOption::BDEM_PASS_THROUGH,
                             &a);

            int usage1 = a.numBlocksTotal();
            if (veryVerbose) { T_ T_ P(usage1) }
            ASSERT(usage1 > usage0);

            table.appendNullRow();
            int usage2 = a.numBlocksTotal();
            if (veryVerbose) { T_ T_ P(usage2) }
            ASSERT(usage2 > usage1);

            ASSERT(1 == table.numRows());         // Confirm value.
            ASSERT(3 == table.numColumns());
            ASSERT(U01 == table.theModifiableRow(0)[0].theShort());
            ASSERT(U02 == table.theModifiableRow(0)[1].theInt());
            ASSERT(U06 == table.theModifiableRow(0)[2].theString());

            usage1_six = usage1; // Record usage for comparison with
            usage2_six = usage2; // allocations from the default allocator.

            // The specified allocator should have the same usage whether the
            // allocation strategy is allowed to default to 'BDEM_PASS_THROUGH'
            // (five) or 'BDEM_PASS_THROUGH' is specified explicitly (six).

            LOOP2_ASSERT(usage1_five, usage1_six, usage1_five == usage1_six);
            LOOP2_ASSERT(usage2_five, usage2_six, usage2_five == usage2_six);
        }

        // ---------------- VERIFY DEFAULT ALLOCATOR IS USED ------------------

        if (verbose) cout <<
             "\nVerify that the 'bslma_Default::defaultAllocator' is used."
                                                                       << endl;

        // - - - - - - - - - - - - - - - ONE - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tONE: bdem_Table(am);" << endl;
        {
            bslma_TestAllocator a;
            const bslma_DefaultAllocatorGuard dag(&a);
            {
                int usage0 = a.numBlocksTotal();
                if (veryVerbose) { T_ T_ P(usage0) }
                ASSERT(0 == usage0);

                bdem_Table table(bdem_AggregateOption::BDEM_PASS_THROUGH);
                int usage1 = a.numBlocksTotal();
                if (veryVerbose) { T_ T_ P(usage1) }
                ASSERT(usage1 > usage0);

                bdem_ElemType::Type t = bdem_ElemType::BDEM_STRING;
                table.reset(&t, 1);
                table.appendNullRow();
                table.theModifiableRow(0)[0].theModifiableString() = A06;

                int usage2 = a.numBlocksTotal();
                if (veryVerbose) { T_ T_ P(usage2) }
                ASSERT(usage2 > usage1);

                ASSERT(1 == table.numRows());         // Confirm value.
                ASSERT(1 == table.numColumns());
                ASSERT(A06 == table.theModifiableRow(0)[0].theString());

                // Usage for the default allocator (usageN) should be the
                // same as when it is specified explicitly (usageN_one):

                LOOP2_ASSERT(usage1_one, usage1, usage1_one == usage1);
                LOOP2_ASSERT(usage2_one, usage2, usage2_one == usage2);
            }
        }

        // - - - - - - - - - - - - - - - TWO - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tTWO: bdem_Table(et, ne);" << endl;
        {
            bslma_TestAllocator a;
            const bslma_DefaultAllocatorGuard dag(&a);
            {
                const bdem_ElemType::Type TYPES[] = {
                    bdem_ElemType::BDEM_SHORT,
                    bdem_ElemType::BDEM_INT,
                    bdem_ElemType::BDEM_STRING
                };
                const int N = sizeof TYPES / sizeof *TYPES;

                int usage0 = a.numBlocksTotal();
                if (veryVerbose) { T_ T_ P(usage0) }
                ASSERT(0 == usage0);

                bdem_Table table(TYPES, N);
                int usage1 = a.numBlocksTotal();
                if (veryVerbose) { T_ T_ P(usage1) }
                ASSERT(usage1 > usage0);

                table.appendNullRow();
                int usage2 = a.numBlocksTotal();
                if (veryVerbose) { T_ T_ P(usage2) }
                ASSERT(usage2 > usage1);

                ASSERT(1 == table.numRows());         // Confirm value.
                ASSERT(3 == table.numColumns());
                ASSERT(U01 == table.theModifiableRow(0)[0].theShort());
                ASSERT(U02 == table.theModifiableRow(0)[1].theInt());
                ASSERT(U06 == table.theModifiableRow(0)[2].theString());

                // Usage for the default allocator (usageN) should be the
                // same as when it is specified explicitly (usageN_two):

                LOOP2_ASSERT(usage1_two, usage1, usage1_two == usage1);
                LOOP2_ASSERT(usage2_two, usage2, usage2_two == usage2);
            }
        }

        // - - - - - - - - - - - - - - - THREE - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tTHREE: bdem_Table(et, ne, am);" << endl;
        {
            bslma_TestAllocator a;
            const bslma_DefaultAllocatorGuard dag(&a);
            {
                const bdem_ElemType::Type TYPES[] = {
                    bdem_ElemType::BDEM_SHORT,
                    bdem_ElemType::BDEM_INT,
                    bdem_ElemType::BDEM_STRING
                };
                const int N = sizeof TYPES / sizeof *TYPES;

                int usage0 = a.numBlocksTotal();
                if (veryVerbose) { T_ T_ P(usage0) }
                ASSERT(0 == usage0);

                bdem_Table table(TYPES,
                                 N,
                                 bdem_AggregateOption::BDEM_PASS_THROUGH);

                int usage1 = a.numBlocksTotal();
                if (veryVerbose) { T_ T_ P(usage1) }
                ASSERT(usage1 > usage0);

                table.appendNullRow();
                int usage2 = a.numBlocksTotal();
                if (veryVerbose) { T_ T_ P(usage2) }
                ASSERT(usage2 > usage1);

                ASSERT(1 == table.numRows());         // Confirm value.
                ASSERT(3 == table.numColumns());
                ASSERT(U01 == table.theModifiableRow(0)[0].theShort());
                ASSERT(U02 == table.theModifiableRow(0)[1].theInt());
                ASSERT(U06 == table.theModifiableRow(0)[2].theString());

                // Usage for the default allocator (usageN) should be the
                // same as when it is specified explicitly (usageN_three):

                LOOP2_ASSERT(usage1_three, usage1, usage1_three == usage1);
                LOOP2_ASSERT(usage2_three, usage2, usage2_three == usage2);
            }
        }

        // - - - - - - - - - - - - - - - FOUR - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tFOUR: bdem_Table(originalList, am);" << endl;
        {
            bslma_TestAllocator aB;                // B: copy constructed list
            const bslma_DefaultAllocatorGuard dag(&aB);
            {
                bslma_TestAllocator aA;                // A: original list

                int usageA0 = aA.numBlocksTotal();
                int usageB0 = aB.numBlocksTotal();

                if (veryVerbose) { T_ T_ P_(usageA0) P(usageB0) }
                ASSERT(0 == usageA0); ASSERT(0 == usageB0);

                bdem_Table tableA(bdem_AggregateOption::BDEM_PASS_THROUGH,
                                  &aA);
                bdem_Table tableB(tableA,
                                  bdem_AggregateOption::BDEM_PASS_THROUGH);

                int usageA1 = aA.numBlocksTotal();
                int usageB1 = aB.numBlocksTotal();

                if (veryVerbose) { T_ T_ P_(usageA1) P(usageB1) }

                ASSERT(usageA1 > 0); ASSERT(usageB1 > 0);

                bdem_ElemType::Type t = bdem_ElemType::BDEM_STRING;
                tableB.reset(&t, 1);
                tableB.appendNullRow();
                tableB.theModifiableRow(0)[0].theModifiableString() = A06;

                int usageA2 = aA.numBlocksTotal();
                int usageB2 = aB.numBlocksTotal();

                if (veryVerbose) { T_ T_ P_(usageA2) P(usageB2) }

                ASSERT(usageA1 == usageA2);
                ASSERT(usageB1  < usageB2);

                ASSERT(1 == tableB.numRows());         // Confirm value.
                ASSERT(1 == tableB.numColumns());
                ASSERT(A06 == tableB.theModifiableRow(0)[0].theString());

                // Usage for the default allocator (usageN) should be the
                // same as when it is specified explicitly (usageN_four):

                LOOP2_ASSERT(usage1_four, usageB1, usage1_four == usageB1);
                LOOP2_ASSERT(usage2_four, usageB2, usage2_four == usageB2);
            }
        }

        // - - - - - - - - - - - - - - - FIVE - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tFIVE: bdem_Table(et);" << endl;
        {
            bslma_TestAllocator a;
            bsl::vector<bdem_ElemType::Type> TYPES;
            TYPES.push_back(bdem_ElemType::BDEM_SHORT);
            TYPES.push_back(bdem_ElemType::BDEM_INT);
            TYPES.push_back(bdem_ElemType::BDEM_STRING);

            const bslma_DefaultAllocatorGuard dag(&a);
            {
                int usage0 = a.numBlocksTotal();
                if (veryVerbose) { T_ T_ P(usage0) }
                ASSERT(0 == usage0);

                bdem_Table table(TYPES);
                int usage1 = a.numBlocksTotal();
                if (veryVerbose) { T_ T_ P(usage1) }
                ASSERT(usage1 > usage0);

                table.appendNullRow();
                int usage2 = a.numBlocksTotal();
                if (veryVerbose) { T_ T_ P(usage2) }
                ASSERT(usage2 > usage1);

                ASSERT(1 == table.numRows());         // Confirm value.
                ASSERT(3 == table.numColumns());
                ASSERT(U01 == table.theModifiableRow(0)[0].theShort());
                ASSERT(U02 == table.theModifiableRow(0)[1].theInt());
                ASSERT(U06 == table.theModifiableRow(0)[2].theString());

                // Usage for the default allocator (usageN) should be the
                // same as when it is specified explicitly (usageN_five):

                LOOP2_ASSERT(usage1_five, usage1, usage1_five == usage1);
                LOOP2_ASSERT(usage2_five, usage2, usage2_five == usage2);
            }
        }

        // - - - - - - - - - - - - - - - SIX - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tSIX: bdem_Table(et, am);" << endl;
        {
            bslma_TestAllocator a;

            bsl::vector<bdem_ElemType::Type> TYPES;
            TYPES.push_back(bdem_ElemType::BDEM_SHORT);
            TYPES.push_back(bdem_ElemType::BDEM_INT);
            TYPES.push_back(bdem_ElemType::BDEM_STRING);

            const bslma_DefaultAllocatorGuard dag(&a);
            {
                int usage0 = a.numBlocksTotal();
                if (veryVerbose) { T_ T_ P(usage0) }
                ASSERT(0 == usage0);

                bdem_Table table(TYPES,
                                 bdem_AggregateOption::BDEM_PASS_THROUGH);
                int usage1 = a.numBlocksTotal();
                if (veryVerbose) { T_ T_ P(usage1) }
                ASSERT(usage1 > usage0);

                table.appendNullRow();
                int usage2 = a.numBlocksTotal();
                if (veryVerbose) { T_ T_ P(usage2) }
                ASSERT(usage2 > usage1);

                ASSERT(1 == table.numRows());         // Confirm value.
                ASSERT(3 == table.numColumns());
                ASSERT(U01 == table.theModifiableRow(0)[0].theShort());
                ASSERT(U02 == table.theModifiableRow(0)[1].theInt());
                ASSERT(U06 == table.theModifiableRow(0)[2].theString());

                // Usage for the default allocator (usageN) should be the
                // same as when it is specified explicitly (usageN_six):

                LOOP2_ASSERT(usage1_six, usage1, usage1_six == usage1);
                LOOP2_ASSERT(usage2_six, usage2, usage2_six == usage2);
            }
        }

        // ------- VERIFY *SPECIFIED* AND *DEFAULT* ALLOCATION POLICY ------

        if (verbose) cout <<
             "\nVerify *specified* and *default* allocation mode." << endl;

        const bdem_AggregateOption::AllocationStrategy S1 =
                                       bdem_AggregateOption::BDEM_PASS_THROUGH;
        const bdem_AggregateOption::AllocationStrategy S2 =
                                       bdem_AggregateOption::BDEM_WRITE_MANY;
        const bdem_AggregateOption::AllocationStrategy S3 =
                                       bdem_AggregateOption::BDEM_WRITE_ONCE;
        const bdem_AggregateOption::AllocationStrategy S4 =
                                       bdem_AggregateOption::BDEM_SUBORDINATE;

        // - - - - - - - - - - - - - - - ONE - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tONE: bdem_Table(am, ba);" << endl;
        {
            bslma_TestAllocator a;
            bslma_TestAllocator a1;
            bslma_TestAllocator a2;
            bslma_TestAllocator a3;
            bslma_TestAllocator a4;
            bdema_MultipoolAllocator ma4(20, &a4); // handle 'BDEM_SUBORDINATE'

            bdem_Table mT(     &a);  const bdem_Table& T  = mT;
            bdem_Table mT1(S1, &a1); const bdem_Table& T1 = mT1;
            bdem_Table mT2(S2, &a2); const bdem_Table& T2 = mT2;
            bdem_Table mT3(S3, &a3); const bdem_Table& T3 = mT3;
            bdem_Table *p = new(ma4) bdem_Table(S4, &ma4);
            bdem_Table& mT4 = *p;    const bdem_Table& T4 = mT4;
            // Note that if 'BDEM_SUBORDINATE' were the default, it would leek
            // memory, which would be reported by the 'bslma_TestAllocator'
            // 'a4'.

            ASSERT(T == T1); ASSERT(T == T2); ASSERT(T == T3); ASSERT(T == T4);

            int use, use1, use2, use3, use4;

            use  =  a.numBytesInUse();
            use1 = a1.numBytesInUse();
            use2 = a2.numBytesInUse();
            use3 = a3.numBytesInUse();
            use4 = a4.numBytesInUse();

            if (veryVerbose) {T_ T_ P_(use) P_(use1) P_(use2) P_(use3) P(use4)}

            ASSERT(0 == T.numRows());
            ASSERT(0 == T.numColumns());

            bdem_ElemType::Type t = bdem_ElemType::BDEM_STRING_ARRAY;

             mT.reset(&t, 1);
            mT1.reset(&t, 1);
            mT2.reset(&t, 1);
            mT3.reset(&t, 1);
            mT4.reset(&t, 1);

            ASSERT(T == T1); ASSERT(T == T2); ASSERT(T == T3); ASSERT(T == T4);

            use  =  a.numBytesInUse();
            use1 = a1.numBytesInUse();
            use2 = a2.numBytesInUse();
            use3 = a3.numBytesInUse();
            use4 = a4.numBytesInUse();

            if (veryVerbose) {T_ T_ P_(use) P_(use1) P_(use2) P_(use3) P(use4)}

            ASSERT(0 == T.numRows());
            ASSERT(1 == T.numColumns());

            mT.appendNullRow();
            mT1.appendNullRow();
            mT2.appendNullRow();
            mT3.appendNullRow();
            mT4.appendNullRow();

            ASSERT(T == T1); ASSERT(T == T2); ASSERT(T == T3); ASSERT(T == T4);

            use  =  a.numBytesInUse();
            use1 = a1.numBytesInUse();
            use2 = a2.numBytesInUse();
            use3 = a3.numBytesInUse();
            use4 = a4.numBytesInUse();

            if (veryVerbose) {T_ T_ P_(use) P_(use1) P_(use2) P_(use3) P(use4)}

            ASSERT(use1 == use);
            ASSERT(use2 != use); ASSERT(use2 != use3);  ASSERT(use2 != use4)
            ASSERT(use3 != use); ASSERT(use3 != use4);
            ASSERT(use4 != use);

            ASSERT(1 == T.numRows()); // Confirm value.
            ASSERT(1 == T.numColumns());
            ASSERT(U16 == T.theRow(0)[0].theStringArray());

        } // Note: 'bdema_MultipoolAllocator' releases memory on destruction.

        // - - - - - - - - - - - - - - - TWO/THREE - - - - - - - - - - - - - -

        if (verbose) cout << "\tTWO/THREE: bdem_Table(et, ne, am, ba);"<<endl;
        {
            const bdem_ElemType::Type TYPES[] = {
                bdem_ElemType::BDEM_INT_ARRAY
            };
            const int N = sizeof TYPES / sizeof *TYPES;

            bslma_TestAllocator a;
            bslma_TestAllocator a1;
            bslma_TestAllocator a2;
            bslma_TestAllocator a3;
            bslma_TestAllocator a4;
            bdema_MultipoolAllocator ma4(20, &a4); // handle 'BDEM_SUBORDINATE'

            bdem_Table  mT(TYPES, N,     &a);  const bdem_Table& T  = mT;
            bdem_Table mT1(TYPES, N, S1, &a1); const bdem_Table& T1 = mT1;
            bdem_Table mT2(TYPES, N, S2, &a2); const bdem_Table& T2 = mT2;
            bdem_Table mT3(TYPES, N, S3, &a3); const bdem_Table& T3 = mT3;
            bdem_Table *p = new(ma4) bdem_Table(TYPES, N, S4, &ma4);
            bdem_Table& mT4 = *p;    const bdem_Table& T4 = mT4;
            // Note that if 'BDEM_SUBORDINATE' were the default, it would leek
            // memory, which would be reported by the 'bslma_TestAllocator'
            // 'a4'.

            ASSERT(T == T1); ASSERT(T == T2); ASSERT(T == T3); ASSERT(T == T4);

            int use, use1, use2, use3, use4;

            use  =  a.numBytesInUse();
            use1 = a1.numBytesInUse();
            use2 = a2.numBytesInUse();
            use3 = a3.numBytesInUse();
            use4 = a4.numBytesInUse();

            if (veryVerbose) {T_ T_ P_(use) P_(use1) P_(use2) P_(use3) P(use4)}

            mT.appendNullRow();
            mT1.appendNullRow();
            mT2.appendNullRow();
            mT3.appendNullRow();
            mT4.appendNullRow();

            ASSERT(T == T1); ASSERT(T == T2); ASSERT(T == T3); ASSERT(T == T4);

            use  =  a.numBytesInUse();
            use1 = a1.numBytesInUse();
            use2 = a2.numBytesInUse();
            use3 = a3.numBytesInUse();
            use4 = a4.numBytesInUse();

            if (veryVerbose) {T_ T_ P_(use) P_(use1) P_(use2) P_(use3) P(use4)}

            ASSERT(use1 == use);
            ASSERT(use2 != use); ASSERT(use2 != use3);  ASSERT(use2 != use4)
            ASSERT(use3 != use); ASSERT(use3 != use4);
            ASSERT(use4 != use);

            ASSERT(1 == T.numRows()); // Confirm value.
            ASSERT(1 == T.numColumns());
            ASSERT(U12 == T.theRow(0)[0].theIntArray());
        }

        // - - - - - - - - - - - - - - - FOUR - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tFOUR: bdem_Table(originalList, am, ba);"<<endl;
        {
            bdem_Table mZ(S2); const bdem_Table& Z = mZ;  // non-default mode

            bdem_ElemType::Type t = bdem_ElemType::BDEM_STRING_ARRAY;

            mZ.reset(&t, 1);
            mZ.appendNullRow();
            mZ.theModifiableRow(0)[0].theModifiableStringArray() = A16;

            ASSERT(1 == Z.numRows());
            ASSERT(1 == Z.numColumns());
            ASSERT(A16 == Z[0][0].theStringArray());

            bslma_TestAllocator a;
            bslma_TestAllocator a1;
            bslma_TestAllocator a2;
            bslma_TestAllocator a3;
            bslma_TestAllocator a4;
            bdema_MultipoolAllocator ma4(20, &a4); // handle 'BDEM_SUBORDINATE'

            bdem_Table  mT(Z,     &a);  const bdem_Table& T  = mT;
            bdem_Table mT1(Z, S1, &a1); const bdem_Table& T1 = mT1;
            bdem_Table mT2(Z, S2, &a2); const bdem_Table& T2 = mT2;
            bdem_Table mT3(Z, S3, &a3); const bdem_Table& T3 = mT3;
            bdem_Table *p = new(ma4) bdem_Table(Z, S4, &ma4);
            bdem_Table& mT4 = *p;    const bdem_Table& T4 = mT4;
            // Note that if BDEM_SUBORDINATE were the default, it would leek
            // memory, which would be reported by the 'bslma_TestAllocator'
            // 'a4'.

            ASSERT(T == T1); ASSERT(T == T2); ASSERT(T == T3); ASSERT(T == T4);

            int use, use1, use2, use3, use4;

            use  =  a.numBytesInUse();
            use1 = a1.numBytesInUse();
            use2 = a2.numBytesInUse();
            use3 = a3.numBytesInUse();
            use4 = a4.numBytesInUse();

            if (veryVerbose) {T_ T_ P_(use) P_(use1) P_(use2) P_(use3) P(use4)}

            mT.appendNullRow();
            mT1.appendNullRow();
            mT2.appendNullRow();
            mT3.appendNullRow();
            mT4.appendNullRow();

            ASSERT(T == T1); ASSERT(T == T2); ASSERT(T == T3); ASSERT(T == T4);

            use  =  a.numBytesInUse();
            use1 = a1.numBytesInUse();
            use2 = a2.numBytesInUse();
            use3 = a3.numBytesInUse();
            use4 = a4.numBytesInUse();

            if (veryVerbose) {T_ T_ P_(use) P_(use1) P_(use2) P_(use3) P(use4)}

            ASSERT(use1 == use);
            ASSERT(use2 != use); ASSERT(use2 != use3);  ASSERT(use2 != use4)
            ASSERT(use3 != use); ASSERT(use3 != use4);
            ASSERT(use4 != use);

            ASSERT(2 == T.numRows());
            ASSERT(1 == T.numColumns());
            ASSERT(A16 == T.theRow(0)[0].theStringArray());
            ASSERT(U16 == T.theRow(1)[0].theStringArray());
        }

        // - - - - - - - - - - - - - - - FIVE/SIX - - - - - - - - - - - - - -

        if (verbose) cout << "\tFIVE/SIX: bdem_Table(et, am, ba);"<<endl;
        {
            bsl::vector<bdem_ElemType::Type> TYPES;
            TYPES.push_back(bdem_ElemType::BDEM_INT_ARRAY);

            bslma_TestAllocator a;
            bslma_TestAllocator a1;
            bslma_TestAllocator a2;
            bslma_TestAllocator a3;
            bslma_TestAllocator a4;
            bdema_MultipoolAllocator ma4(20, &a4); // handle 'BDEM_SUBORDINATE'

            bdem_Table  mT(TYPES,     &a);  const bdem_Table& T  = mT;
            bdem_Table mT1(TYPES, S1, &a1); const bdem_Table& T1 = mT1;
            bdem_Table mT2(TYPES, S2, &a2); const bdem_Table& T2 = mT2;
            bdem_Table mT3(TYPES, S3, &a3); const bdem_Table& T3 = mT3;
            bdem_Table *p = new(ma4) bdem_Table(TYPES, S4, &ma4);
            bdem_Table& mT4 = *p;    const bdem_Table& T4 = mT4;
            // Note that if BDEM_SUBORDINATE were the default, it would leek
            // memory, which would be reported by the 'bslma_TestAllocator'
            // 'a4'.

            ASSERT(T == T1); ASSERT(T == T2); ASSERT(T == T3); ASSERT(T == T4);

            int use, use1, use2, use3, use4;

            use  =  a.numBytesInUse();
            use1 = a1.numBytesInUse();
            use2 = a2.numBytesInUse();
            use3 = a3.numBytesInUse();
            use4 = a4.numBytesInUse();

            if (veryVerbose) {T_ T_ P_(use) P_(use1) P_(use2) P_(use3) P(use4)}

             mT.appendNullRow();
            mT1.appendNullRow();
            mT2.appendNullRow();
            mT3.appendNullRow();
            mT4.appendNullRow();

            ASSERT(T == T1); ASSERT(T == T2); ASSERT(T == T3); ASSERT(T == T4);

            use  =  a.numBytesInUse();
            use1 = a1.numBytesInUse();
            use2 = a2.numBytesInUse();
            use3 = a3.numBytesInUse();
            use4 = a4.numBytesInUse();

            if (veryVerbose) {T_ T_ P_(use) P_(use1) P_(use2) P_(use3) P(use4)}

            ASSERT(use1 == use);
            ASSERT(use2 != use); ASSERT(use2 != use3);  ASSERT(use2 != use4)
            ASSERT(use3 != use); ASSERT(use3 != use4);
            ASSERT(use4 != use);

            ASSERT(1 == T.numRows()); // Confirm value.
            ASSERT(1 == T.numColumns());
            ASSERT(U12 == T.theRow(0)[0].theIntArray());
        }

      }

DEFINE_TEST_CASE(10) {
        // --------------------------------------------------------------------
        // BDEX STREAMING
        //   Ensure that a 'bdem_Table' can convert itself to/from byte stream.
        //
        // Concerns:
        //
        // Plan:
        //
        // Tactics:
        //
        // Testing:
        //   static int maxSupportedBdexVersion();
        //
        //   STREAM& bdexStreamOut(STREAM& stream, int version) const;
        //   STREAM& bdexStreamIn(STREAM& stream, int version)
        // --------------------------------------------------------------------

        bslma_TestAllocator defaultAllocator;
        const bslma_DefaultAllocatorGuard dag(&defaultAllocator);

        if (verbose) cout << endl << "BDEX STREAMING" << endl
                                  << "==============" << endl;

        const int MAX_VERSION = 3;
        {
            ASSERT(MAX_VERSION == Obj::maxSupportedBdexVersion());
        }

        const struct {
            const int   d_line;    // line
            const char *d_spec_p;  // list spec
        } DATA[] = {
               // Line                Spec
               // ----                ----
            {      L_,                ""             },

            // One element
            {      L_,                "A"            },
            {      L_,                "C"            },
            {      L_,                "F"            },
            {      L_,                "G"            },
            {      L_,                "O"            },
            {      L_,                "Q"            },
            {      L_,                "R"            },
            {      L_,                "U"            },
            {      L_,                "V"            },
            {      L_,                "W"            },
            {      L_,                "b"            },
            {      L_,                "e"            },
            {      L_,                "f"            },
            {      L_,                "B@"           },
            {      L_,                "B*"           },
            {      L_,                "D*"           },
            {      L_,                "U*"           },
            {      L_,                "V@"           },
            {      L_,                "e@"           },
            {      L_,                "V*"           },
            {      L_,                "e*"           },
            {      L_,                "f*"           },

            // Two elements
            {      L_,                "AB"           },
            {      L_,                "CD"           },
            {      L_,                "EF"           },
            {      L_,                "GH"           },
            {      L_,                "OM"           },
            {      L_,                "QP"           },
            {      L_,                "RS"           },
            {      L_,                "UT"           },
            {      L_,                "VZ"           },
            {      L_,                "Wc"           },
            {      L_,                "ab"           },
            {      L_,                "ef"           },
            {      L_,                "A*B"          },
            {      L_,                "CD@"          },
            {      L_,                "U@T"          },
            {      L_,                "CD*"          },
            {      L_,                "U*T"          },
            {      L_,                "V*Z"          },
            {      L_,                "U*T*"         },
            {      L_,                "V*Z@"         },
            {      L_,                "a@b@"         },
            {      L_,                "V*Z*"         },
            {      L_,                "a*b*"         },
            {      L_,                "e*f*"         },

            // Three elements
            {      L_,                "ABa"           },
            {      L_,                "CDd"           },
            {      L_,                "EFA"           },
            {      L_,                "GHF"           },
            {      L_,                "OMW"           },
            {      L_,                "QPQ"           },
            {      L_,                "RSJ"           },
            {      L_,                "UTK"           },
            {      L_,                "VZb"           },
            {      L_,                "WcA"           },
            {      L_,                "abD"           },
            {      L_,                "efE"           },
            {      L_,                "a@bD"          },
            {      L_,                "e*f@E"         },
            {      L_,                "U*T*K@"        },
            {      L_,                "V@Z@b*"        },
            {      L_,                "a*bD"          },
            {      L_,                "e*f*E"         },
            {      L_,                "U*T*K*"        },
            {      L_,                "V*Z*b*"        },

            {      L_,                "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef"   },
            {      L_,          "ABCD@EFG*HIJK*LMNOPQRSTUVWXYZa*bcde@f*"   },
            {      L_,          "ABCD*EFG*HIJK*LMNOPQRSTUVWXYZa*bcde*f*"   },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bdem_List mA;  const bdem_List& A = mA;
        loadReferenceA(&mA);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE = DATA[i].d_line;
            const char *SPEC = DATA[i].d_spec_p;

            for (int version = 1; version < MAX_VERSION; ++version) {

                Obj mX = g(SPEC);  const Obj& X = mX;
                gg(&mX, SPEC, A.row());
                bdex_TestOutStream out;
                X.bdexStreamOut(out, version);
                LOOP_ASSERT(LINE, out);

                bdex_TestInStream in(out.data(), out.length());
                LOOP_ASSERT(LINE, in);
                LOOP_ASSERT(LINE, !in.isEmpty());
                in.setSuppressVersionCheck(1);

                Obj mY; const Obj& Y = mY;
                mY.bdexStreamIn(in, version);
                LOOP_ASSERT(LINE, in);

                bsl::string altSpec(SPEC);
                bsl::for_each(altSpec.begin(),
                              altSpec.end(),
                              BdexHelper<bsl::string::value_type>(version));

                Obj mE = g(altSpec.c_str());  const Obj& E = mE;
                gg(&mE, altSpec.c_str(), A.row());
                LOOP5_ASSERT(LINE, version, X, Y, E, E == Y);
            }
        }
      }

DEFINE_TEST_CASE(9) {
        // --------------------------------------------------------------------
        // ASSIGNMENT OPERATOR (op=)
        //   Ensure that assignment is "wired-up" properly.
        //
        // Concerns:
        //   - That the assigned object has the same value.
        //     + fewer rows can be assigned to more (and vice versa).
        //     + fewer columns can be assigned to more (and vice versa).
        //   - That neither the allocator nor allocation strategy is copied.
        //   - That assignment works with a 'const' 'bdem_Table'.
        //   - That there are no memory leaks.
        //   - That aliasing is not a problem.
        //
        // Plan:
        //  For each of a small set of tables constructed with a test allocator
        //  and a specific strategy, ensure that the assigned instance has the
        //  same value as the source, and that neither its allocator nor its
        //  strategy is modified.  The general approach is to use the
        //  'bslma_TestAllocator' to verify that subsequent allocations from
        //  assigned object are (1) from its original allocator and (2) that
        //  the allocation pattern exactly matches a similar table explicitly
        //  constructed with that strategy.  Verify the value aspects on
        //  various initial values including the following:
        //   - The empty table.
        //   - Each of the 32 one-element tables.
        //   - A table consisting of all 32 element types.
        //  Specifically make sure that we can assign a table with 32 columns
        //  to each one-column table and vice versa; ensure that similar tables
        //  with few rows can be assigned to ones with many and vice versa.
        //  Also try a couple of examples involving aliasing.
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //   - Brute-Force and Array-Based Implementation Techniques
        //
        // Testing:
        //   bdem_Table& operator=(const bdem_Table& rhs);
        // --------------------------------------------------------------------

        bslma_TestAllocator defaultAllocator;
        const bslma_DefaultAllocatorGuard dag(&defaultAllocator);

        if (verbose) cout << endl
                          << "ASSIGNMENT OPERATOR" << endl
                          << "===================" << endl;

        if (verbose) cout << "\nVerify Allocators aren't modified." << endl;
        {
            bslma_TestAllocator a1;
            bslma_TestAllocator a2;
            bdem_Table table1(&a1);
            bdem_Table table2(&a2);

            const int USAGE_1 = a1.numBlocksTotal();
            const int USAGE_2 = a2.numBlocksTotal();
            ASSERT(USAGE_1 == USAGE_2);

            table2 = table1;
            for (int i = 0; i < 5; ++i) {
                table2.appendNullRow();
            }
            for (int i = 0; i < 5; ++i) {
                table2.appendNullRow();
            }

            if (veryVerbose) { P(table1); P(table2); }

            ASSERT(USAGE_1 == a1.numBlocksTotal());
            ASSERT(USAGE_2  < a2.numBlocksTotal());
        }

        if (verbose) cout << "\nVerify Strategies aren't modified." << endl;
        {
            const bdem_AggregateOption::AllocationStrategy S1 =
                                       bdem_AggregateOption::BDEM_PASS_THROUGH;
            const bdem_AggregateOption::AllocationStrategy S2 =
                                       bdem_AggregateOption::BDEM_WRITE_ONCE;

            if (verbose) cout << "\tCreate 4 test allocators." << endl;

            bslma_TestAllocator a1;
            bslma_TestAllocator a2;
            bslma_TestAllocator aX;
            bslma_TestAllocator aY;

            int usage1 = a1.numBytesTotal();
            int usage2 = a2.numBytesTotal();
            int usageX = aX.numBytesTotal();
            int usageY = aY.numBytesTotal();

            if (veryVerbose) {T_ T_ P_(usage1) P_(usage2) P_(usageX) P(usageY)}

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

            if (verbose) cout <<
                     "\tCreate 4 tables where 1 is BDEM_PASS_THROUGH." << endl;
            bdem_Table table1(S1, &a1);   // PASS THROUGH
            bdem_Table table2(S2, &a2);
            bdem_Table tableX(S2, &aX);
            bdem_Table tableY(S2, &aY);

            usage1 = a1.numBytesTotal();
            usage2 = a2.numBytesTotal();
            usageX = aX.numBytesTotal();
            usageY = aY.numBytesTotal();

            if (veryVerbose) {T_ T_ P_(usage1) P_(usage2) P_(usageX) P(usageY)}

            ASSERT(tableX == table1); ASSERT(tableY == tableX);
            ASSERT(tableX == table2); ASSERT(usageY == usageX);

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

            if (verbose) cout << "\tAssign from different and same strategies."
                                                                       << endl;
            tableX = table1;      // Assign X from strategy 1 (different).
            tableY = table2;      // Assign Y from strategy 2 (same).

            usage1 = a1.numBytesTotal();
            usage2 = a2.numBytesTotal();
            usageX = aX.numBytesTotal();
            usageY = aY.numBytesTotal();

            if (veryVerbose) {T_ T_ P_(usage1) P_(usage2) P_(usageX) P(usageY)}

            ASSERT(tableX == table1); ASSERT(tableY == tableX);
            ASSERT(tableX == table2); ASSERT(usageY == usageX);

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

            if (verbose) cout << "\tAppend some rows to each table." << endl;

            for (int i = 0; i < 5; ++i) {
                table1.appendNullRow();
                table2.appendNullRow();
                tableX.appendNullRow();
                tableY.appendNullRow();
            }

            usage1 = a1.numBytesTotal();
            usage2 = a2.numBytesTotal();
            usageX = aX.numBytesTotal();
            usageY = aY.numBytesTotal();

            if (veryVerbose) {T_ T_ P_(usage1) P_(usage2) P_(usageX) P(usageY)}

            ASSERT(tableX == table1); ASSERT(tableY == tableX);
            ASSERT(tableX == table2); ASSERT(usageY == usageX);

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

            if (verbose) cout << "\tAppend some null rows to each table."
                              << endl;

            for (int i = 0; i < 5; ++i) {
                table1.appendNullRow();
                table2.appendNullRow();
                tableX.appendNullRow();
                tableY.appendNullRow();
            }

            usage1 = a1.numBytesTotal();
            usage2 = a2.numBytesTotal();
            usageX = aX.numBytesTotal();
            usageY = aY.numBytesTotal();

            if (veryVerbose) {T_ T_ P_(usage1) P_(usage2) P_(usageX) P(usageY)}

            ASSERT(tableX == table1); ASSERT(tableY == tableX);
            ASSERT(tableX == table2); ASSERT(usageY == usageX);

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

            if (verbose) cout << "\tAssign from different and same strategies."
                                                                       << endl;
            tableX = table1;      // Assign X from strategy 1 (different).
            tableY = table2;      // Assign Y from strategy 2 (same).

            usage1 = a1.numBytesTotal();
            usage2 = a2.numBytesTotal();
            usageX = aX.numBytesTotal();
            usageY = aY.numBytesTotal();

            if (veryVerbose) {T_ T_ P_(usage1) P_(usage2) P_(usageX) P(usageY)}

            ASSERT(tableX == table1); ASSERT(tableY == tableX);
            ASSERT(tableX == table2); ASSERT(usageY == usageX);

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

            if (verbose) cout
                << "\tAppend some more rows to each table." << endl;

            for (int i = 0; i < 10; ++i) {
                table1.appendNullRow();
                table2.appendNullRow();
                tableX.appendNullRow();
                tableY.appendNullRow();
            }

            usage1 = a1.numBytesTotal();
            usage2 = a2.numBytesTotal();
            usageX = aX.numBytesTotal();
            usageY = aY.numBytesTotal();

            if (veryVerbose) {T_ T_ P_(usage1) P_(usage2) P_(usageX) P(usageY)}

            ASSERT(tableX == table1); ASSERT(tableY == tableX);
            ASSERT(tableX == table2); ASSERT(usageY == usageX);

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

            if (verbose) cout
                << "\tAppend some more null rows to each table." << endl;

            for (int i = 0; i < 10; ++i) {
                table1.appendNullRow();
                table2.appendNullRow();
                tableX.appendNullRow();
                tableY.appendNullRow();
            }

            usage1 = a1.numBytesTotal();
            usage2 = a2.numBytesTotal();
            usageX = aX.numBytesTotal();
            usageY = aY.numBytesTotal();

            if (veryVerbose) {T_ T_ P_(usage1) P_(usage2) P_(usageX) P(usageY)}

            ASSERT(tableX == table1); ASSERT(tableY == tableX);
            ASSERT(tableX == table2); ASSERT(usageY == usageX);

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

            if (verbose) cout << "\tAssign from different and same strategies."
                                                                       << endl;
            tableX = table1;      // Assign X from strategy 1 (different).
            tableY = table2;      // Assign Y from strategy 2 (same).

            usage1 = a1.numBytesTotal();
            usage2 = a2.numBytesTotal();
            usageX = aX.numBytesTotal();
            usageY = aY.numBytesTotal();

            if (veryVerbose) {T_ T_ P_(usage1) P_(usage2) P_(usageX) P(usageY)}

            ASSERT(tableX == table1); ASSERT(tableY == tableX);
            ASSERT(tableX == table2); ASSERT(usageY == usageX);
        }

        if (verbose) cout <<
             "\nVerify that various tables are assigned correctly." << endl;

        const char *SPECS[] = {
                "",
                "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K",
                "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V",
                "ABCDEFGHIJKLMNOPQRSTUV"
        };
        const int NUM_SPECS = sizeof SPECS / sizeof *SPECS;
        LOOP_ASSERT(NUM_SPECS, 24 == NUM_SPECS);

        bdem_List mA; const bdem_List& A = mA; ASSERT(0         == A.length());
        loadReferenceA(&mA);                   ASSERT(NUM_ELEMS == A.length());
        bdem_List mB; const bdem_List& B = mB; ASSERT(0         == B.length());
        loadReferenceB(&mB);                   ASSERT(NUM_ELEMS == B.length());
        bdem_List mN; const bdem_List& N = mN; ASSERT(0         == N.length());
        loadReferenceU(&mN);                   ASSERT(NUM_ELEMS == N.length());

        for (int i = 0; i < NUM_SPECS; ++i) {
            if (veryVerbose) { T_ P(i) };

            const bdem_Table SOURCE0 = g(SPECS[i]);
            const bdem_Table SOURCE1 = g(SPECS[i], N.row());
            const bdem_Table SOURCE2 = g(SPECS[i], A.row(), B.row());

            if (veryVerbose) {
                T_ P(SOURCE0)
                T_ P(SOURCE1)
                T_ P(SOURCE2)
            }

            for (int j = 0; j < NUM_SPECS; ++j) {
                bool PRINT = veryVerbose && (i <= 1 || i >= NUM_SPECS - 2)
                          || veryVeryVerbose;

                if (PRINT) { T_ P(j) };

                bdem_Table d00(g(SPECS[j]));            // make 0 from 0 rows
                bdem_Table d01(g(SPECS[j], A.row()));   // make 0 from 1 row
                bdem_Table d02(g(SPECS[j], N.row(),
                                           B.row()));   // make 0 from 2 rows

                bdem_Table d10(d00);                    // make 1 from 0 rows
                bdem_Table d11(d01);                    // make 1 from 1 row
                bdem_Table d12(d02);                    // make 1 from 2 rows

                bdem_Table d20(d10);                    // make 2 from 0 rows
                bdem_Table d21(d11);                    // make 2 from 1 row
                bdem_Table d30(d12);                    // make 2 from 2 rows

                if (PRINT) {
                    T_ P(d20)
                    T_ P(d21)
                    T_ P(d30)
                }

                const bool U = i != j; // Assumes column types are unique.
                const bool C = U || SOURCE0.numColumns() > 0;

                LOOP4_ASSERT(i, j, U, C, U == (d00 != SOURCE0));
                LOOP4_ASSERT(i, j, U, C, 1 == (d10 != SOURCE1));
                LOOP4_ASSERT(i, j, U, C, 1 == (d20 != SOURCE2));

                LOOP4_ASSERT(i, j, U, C, 1 == (d01 != SOURCE0));
                LOOP4_ASSERT(i, j, U, C, C == (d11 != SOURCE1));
                LOOP4_ASSERT(i, j, U, C, 1 == (d21 != SOURCE2));

                LOOP4_ASSERT(i, j, U, C, 1 == (d02 != SOURCE0));
                LOOP4_ASSERT(i, j, U, C, 1 == (d12 != SOURCE1));
                LOOP4_ASSERT(i, j, U, C, C == (d30 != SOURCE2));

                d00 = SOURCE0;
                d10 = SOURCE1;
                d20 = SOURCE2;

                d01 = SOURCE0;
                d11 = SOURCE1;
                d21 = SOURCE2;

                d02 = SOURCE0;
                d12 = SOURCE1;
                d30 = SOURCE2;

                LOOP2_ASSERT(i, j, d00 == SOURCE0);
                LOOP2_ASSERT(i, j, d10 == SOURCE1);
                LOOP2_ASSERT(i, j, d20 == SOURCE2);

                LOOP2_ASSERT(i, j, d01 == SOURCE0);
                LOOP2_ASSERT(i, j, d11 == SOURCE1);
                LOOP2_ASSERT(i, j, d21 == SOURCE2);

                LOOP2_ASSERT(i, j, d02 == SOURCE0);
                LOOP2_ASSERT(i, j, d12 == SOURCE1);
                LOOP2_ASSERT(i, j, d30 == SOURCE2);

            }
        }

        if (verbose) cout << "\nTry some examples involving aliasing." << endl;
        {
            bdem_Table x;
            x = x;
            ASSERT(bdem_Table() == x);

            bdem_Table Y = g("ECA", A.row());
            bdem_Table y(Y);
            y = y;
            ASSERT(Y == y);

            bdem_Table Z = g("VUTSRQPONMLKJIHGFEDCBA", A.row(),
                                                       B.row(),
                                                       N.row());
            bdem_Table z(Z);
            z = z;
            ASSERT(Z == z);
        }

      }

DEFINE_TEST_CASE(8) {
        // --------------------------------------------------------------------
        // TEST GENERATOR FUNCTIONS
        //   Verify the gg function and various g functions for table.
        //
        // Concerns:
        //   g:
        //     - That all 32 values select the correct type.
        //     - That common whitespace is ignored { space, tab, newline }.
        //   gg:
        //     - That all 32 values select the correct list item.
        //     - That common whitespace is ignored { space, tab, newline }.
        //   g(list1), g(list1, list2), g(list1, list2, list3).
        //     - That all 32 values select the correct type and list item.
        //     - That common whitespace is ignored { space, tab, newline }.
        //
        // Plan:
        //   These tests can be light; if there is a bug, it is unlikely to
        //   match the failure mode of the test in which the test helper
        //   function is used:
        //   g:
        //     - Create various tables and verify that the column types are
        //        correct.
        //   gg:
        //     - Append to these tables values take from specific reference
        //        lists A, B, and N.
        //   g(list1), g(list1, list2), g(list1, list2, list3).
        //     - Use g and gg to validate these combined operations.
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //   - Brute-Force Implementation Technique
        //
        // Testing:
        //   bdem_Table g(const char *spec);
        //   void gg(bdem_Table *t, const char *s, const bdem_List& r)
        //   bdem_Table g(const char *spec, list1);
        //   bdem_Table g(const char *spec, list1, list2);
        //   bdem_Table g(const char *spec, list1, list2, list3);
        //
        // --------------------------------------------------------------------

        bslma_TestAllocator defaultAllocator;
        const bslma_DefaultAllocatorGuard dag(&defaultAllocator);

        if (verbose) cout << endl
                          << "TEST GENERATOR FUNCTIONS" << endl
                          << "========================" << endl;
        bdem_List mA;  const bdem_List& A = mA;
                                            ASSERT(0         == A.length());
        loadReferenceA(&mA);                ASSERT(NUM_ELEMS == A.length());
        bdem_List mB;  const bdem_List& B = mB;
                                            ASSERT(0         == B.length());
        loadReferenceB(&mB);                ASSERT(NUM_ELEMS == B.length());
        bdem_List mN;  const bdem_List& N = mN;
                                            ASSERT(0         == N.length());
        loadReferenceU(&mN);                ASSERT(NUM_ELEMS == N.length());

        if (verbose) cout << "\nCreate a 32-column table." << endl;
        {
            const char S[] = " A B C D E F G H I J\nK L M N O P Q R S T\tU V "
                             " W X Y Z a b c d e f\n";

            if (verbose) cout << "\tg." << endl;
            bdem_Table mT = g(S); const bdem_Table& T = mT;
            ASSERT(0 == T.numRows()); ASSERT(NUM_ELEMS == T.numColumns());

            for (int i = 0; i < NUM_ELEMS; ++i) {
                LOOP_ASSERT(i, A[i].type() == T.columnType(i));
            }

            if (verbose) cout << "\tgg." << endl;
            gg(&mT, S, A.row());
            ASSERT(1 == T.numRows()); ASSERT(NUM_ELEMS == T.numColumns());
            ASSERT(A.row() == T.theRow(0));

            {
                const bdem_Table X = g(S, A.row()); ASSERT(T == X);
            }

            if (verbose) cout << "\tgg." << endl;
            gg(&mT, S, B.row());
            ASSERT(2 == T.numRows()); ASSERT(NUM_ELEMS == T.numColumns());
            ASSERT(A.row() == T.theRow(0));
            ASSERT(B.row() == T.theRow(1));

            {
                const bdem_Table X = g(S, A.row(), B.row()); ASSERT(T == X);
            }

            if (verbose) cout << "\tgg." << endl;
            gg(&mT, S, N.row());
            ASSERT(3 == T.numRows()); ASSERT(NUM_ELEMS == T.numColumns());
            ASSERT(A.row() == T.theRow(0));
            ASSERT(B.row() == T.theRow(1));
            ASSERT(N.row() == T.theRow(2));

            {
                const bdem_Table X = g(S, A.row(), B.row(), N.row());
                ASSERT(T == X);
            }

            if (veryVerbose) { P(T) }
        }

        if (verbose) cout << "\nCreate a 36-column table." << endl;
        {
            bdem_List mL; const bdem_List& L = mL;
            mL = A;
            mL.appendElements(A, 0, 4);
            ASSERT(NUM_ELEMS + 4 == L.length());
            if (veryVerbose) { T_ P(L) }

            const char S[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef"  "ABCD";

            if (verbose) cout << "\tg." << endl;
            bdem_Table mT = g(S); const bdem_Table& T = mT;
            ASSERT(0 == T.numRows()); ASSERT(36 == T.numColumns());

            for (int i = 0; i < NUM_ELEMS + 4; ++i) {
                LOOP_ASSERT(i, A[i % NUM_ELEMS].type() == T.columnType(i));
            }

            const char U[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef"  "ABCD";

            if (verbose) cout << "\tgg." << endl;
            gg(&mT, U, L.row());
            ASSERT(1 == T.numRows()); ASSERT(NUM_ELEMS + 4 == T.numColumns());
            ASSERT(L.row() == T.theRow(0));

            if (veryVerbose) { P(T) }
        }

        if (verbose) cout << "\nCreate a 4-column table." << endl;
        {
            const char S[] = "CGMQ"; // INT STRING INT_ARRAY STRING_ARRAY

            if (verbose) cout << "\tg." << endl;
            bdem_Table mT = g(S); const bdem_Table& T = mT;
            ASSERT(0 == T.numRows()); ASSERT(4 == T.numColumns());

            if (verbose) cout << "\tgg." << endl;
            gg(&mT, S, A.row());
            ASSERT(1 == T.numRows()); ASSERT(4 == T.numColumns());
            ASSERT(A02 == T.theRow(0)[0].theInt());
            ASSERT(A06 == T.theRow(0)[1].theString());
            ASSERT(A12 == T.theRow(0)[2].theIntArray());
            ASSERT(A16 == T.theRow(0)[3].theStringArray());

            {
                const bdem_Table X = g(S, A.row()); ASSERT(T == X);
            }

            if (verbose) cout << "\tgg." << endl;
            gg(&mT, S, B.row());
            ASSERT(2 == T.numRows()); ASSERT(4 == T.numColumns());
            ASSERT(B02 == T.theRow(1)[0].theInt());
            ASSERT(B06 == T.theRow(1)[1].theString());
            ASSERT(B12 == T.theRow(1)[2].theIntArray());
            ASSERT(B16 == T.theRow(1)[3].theStringArray());

            {
                const bdem_Table X = g(S, A.row(), B.row()); ASSERT(T == X);
            }

            if (verbose) cout << "\tgg." << endl;
            gg(&mT, S, N.row());
            ASSERT(3 == T.numRows()); ASSERT(4 == T.numColumns());
            ASSERT(U02 == T.theRow(2)[0].theInt());
            ASSERT(U06 == T.theRow(2)[1].theString());
            ASSERT(U12 == T.theRow(2)[2].theIntArray());
            ASSERT(U16 == T.theRow(2)[3].theStringArray());

            {
                const bdem_Table X = g(S, A.row(), B.row(), N.row());
                ASSERT(T == X);
            }

            if (veryVerbose) { P(T) }
        }

        if (verbose) cout << "\tCreate a table with unset and nulls." << endl;
        {
            const char      *SPEC       = "A@B@C*D*c@d@e*f*";
            const int        LEN        = strlen(SPEC);
            bdem_Table       mT         = g(SPEC); const bdem_Table& T = mT;
            ASSERT(0 == T.numRows()); ASSERT(LEN / 2 == T.numColumns());
            const char       UNSET_CHAR = '@';
            const char       NULL_CHAR  = '*';

            if (veryVerbose) {
                cout << "\t\tT: "; T.print(cout, -3, 4);
            }

            for (int i = 0, j = 0; i < LEN; i += 2, ++j) {
                ET::Type TYPE = getElemType(SPEC[i]);
                LOOP3_ASSERT(j, TYPE, T.columnType(j),
                             TYPE == T.columnType(j));
            }

            gg(&mT, SPEC, A.row());
            ASSERT(1 == T.numRows()); ASSERT(LEN / 2 == T.numColumns());

            for (int i = 0, j = 0; i < LEN; i += 2, ++j) {
                if (UNSET_CHAR == SPEC[i + 1]) {
                    LOOP_ASSERT(i, T.theRow(0)[j].isNonNull());
                }
                else if (NULL_CHAR == SPEC[i + 1]) {
                    LOOP_ASSERT(i, T.theRow(0)[j].isNull());
                }
            }
        }

      }

DEFINE_TEST_CASE(7) {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTION
        //   Ensure that this constructor is "wired-up" and defaults properly.
        //
        // Concerns:
        //   - That neither the allocator nor allocation strategy is copied.
        //   - That the current default allocator is uses by default.
        //   - That when an allocator is specified, net default allocator
        //      usage between operations is zero.
        //   - That the copy-constructed object has the same value.
        //   - That it works with a 'const' 'bdem_Table' argument.
        //
        // Plan:
        //   - To ensure that the allocator isn't copied and that the current
        //      default allocator is used, start by installing a test allocator
        //      as the default.  Create tables from both non-modifiable and
        //      modifiable tables installed explicitly with a separate test
        //      allocator; append rows to the copied tables and observer that
        //      default and not that explicitly specified allocator was used.
        //   - To ensure that the default strategy is used by creating 3
        //      separate tables, each created with an explicit strategy.
        //      Create four additional tables, each with its own test
        //      allocator.  Three of these tables are copy constructed from a
        //      different original table with the corresponding strategy
        //      specified explicitly; the fourth is copied from an original
        //      with a non-default strategy, but its own is left unspecified.
        //      After appending the same value to each of the four tables,
        //      respective usages are compared to ensure that the forth
        //      conforms to the default and not the other two.  Periodically
        //      confirm that the default allocator net usage is zero.
        //   - Ensure that the copy constructed value is correct using:
        //      + The empty Table.
        //      + Each of the 32 one-column tables.
        //      + A table consisting of all 32 unique column types.
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //   - Brute-Force and Loop-Based Implementation Techniques
        //
        // Testing:
        //   bdem_List(const bdem_List& original, ba);
        //
        // --------------------------------------------------------------------

        bslma_TestAllocator defaultAllocator;
        const bslma_DefaultAllocatorGuard dag(&defaultAllocator);

        if (verbose) cout << endl
                          << "Copy Construction" << endl
                          << "=================" << endl;

        if (verbose) cout << "\nVerify Allocators aren't copied." << endl;
        {
            if (verbose) cout << "\tFirst create initial objects." << endl;
            bslma_TestAllocator da;  // Must be created first!

            bslma_TestAllocator ta0;  // used with initial table T
            bslma_TestAllocator ta1;  // used to copy construct table1a
            bslma_TestAllocator ta2;  // used to copy construct table2a

            if (verbose) cout <<
                "\tCreate/install a test allocator as the default." << endl;

            const bslma_DefaultAllocatorGuard dag(&da);

            if (verbose) cout <<
                "\tConstruct an initial table T." << endl;

            ASSERT(0 == ta0.numBlocksTotal());

            if (verbose) cout <<
                "\tCopy construct from the empty initial table." << endl;

            bdem_Table T(&ta0);

            const int TA0_USAGE_FINAL = ta0.numBlocksTotal();

            if (verbose) cout <<
              "\tCopy empty tables without/with supplying an allocator."<<endl;

            ASSERT(0 == da.numBlocksTotal());
            bdem_Table table1(T);
            const int DA_USAGE_1 = da.numBlocksTotal();
            ASSERT(0 < DA_USAGE_1);
            ASSERT(TA0_USAGE_FINAL == ta0.numBlocksTotal());

            ASSERT(T == table1);

            ASSERT(0 == ta1.numBlocksTotal());
            bdem_Table table1a(T, &ta1);
            const int TA1_USAGE_1 = ta1.numBlocksTotal();
            ASSERT(0 < TA1_USAGE_1);
            ASSERT(DA_USAGE_1 == da.numBlocksTotal());
            ASSERT(TA0_USAGE_FINAL == ta0.numBlocksTotal());

            ASSERT(T == table1a);

            if (verbose) cout <<
               "\t\tVerify gross/net allocations with/without are same."
                                                                       << endl;
            {
                // Gross and net allocations from 'ta1' and 'da' should be eq.
                const int daByInUse = da.numBytesInUse();
                const int daBkInUse = da.numBlocksInUse();
                const int daByTotal = da.numBytesTotal();
                const int daBkTotal = da.numBlocksTotal();

                const int ta1ByInUse = ta1.numBytesInUse();
                const int ta1BkInUse = ta1.numBlocksInUse();
                const int ta1ByTotal = ta1.numBytesTotal();
                const int ta1BkTotal = ta1.numBlocksTotal();

                if (veryVerbose) { P_(daByInUse) P_(daBkInUse)
                                   P_(daByTotal)  P(daBkTotal) }

                LOOP2_ASSERT(ta1ByInUse, daByInUse, ta1ByInUse == daByInUse);
                LOOP2_ASSERT(ta1BkInUse, daBkInUse, ta1BkInUse == daBkInUse);
                LOOP2_ASSERT(ta1ByTotal, daByTotal, ta1ByTotal == daByTotal);
                LOOP2_ASSERT(ta1BkTotal, daBkTotal, ta1BkTotal == daBkTotal);
            }

            if (verbose) cout << "\tAppend several rows." << endl;
            const int SEVERAL = 10;

            ASSERT(DA_USAGE_1 == da.numBlocksTotal());
            {
                for (int i = 0; i < SEVERAL; ++i) {
                    table1.appendNullRow();
                }
            }
            const int DA_USAGE_2 = da.numBlocksTotal();
            ASSERT(DA_USAGE_1 < DA_USAGE_2);
            ASSERT(TA1_USAGE_1 == ta1.numBlocksTotal());
            ASSERT(TA0_USAGE_FINAL == ta0.numBlocksTotal());

            ASSERT(table1a != table1);

            ASSERT(TA1_USAGE_1 == ta1.numBlocksTotal());
            {
                for (int i = 0; i < SEVERAL; ++i) {
                    table1a.appendNullRow();
                }
            }
            const int TA1_USAGE_2 = ta1.numBlocksTotal();
            ASSERT(TA1_USAGE_1 < TA1_USAGE_2);
            ASSERT(DA_USAGE_2 == da.numBlocksTotal());
            ASSERT(TA0_USAGE_FINAL == ta0.numBlocksTotal());

            ASSERT(table1a == table1);

            if (verbose) cout <<
                "\t\tVerify gross/net allocations with/without are still same."
                                                                       << endl;
            {
                // Gross and net allocations from 'ta1' and 'da' should be eq.
                const int daByInUse = da.numBytesInUse();
                const int daBkInUse = da.numBlocksInUse();
                const int daByTotal = da.numBytesTotal();
                const int daBkTotal = da.numBlocksTotal();

                const int ta1ByInUse = ta1.numBytesInUse();
                const int ta1BkInUse = ta1.numBlocksInUse();
                const int ta1ByTotal = ta1.numBytesTotal();
                const int ta1BkTotal = ta1.numBlocksTotal();

                if (veryVerbose) { P_(daByInUse) P_(daBkInUse)
                                   P_(daByTotal)  P(daBkTotal) }

                LOOP2_ASSERT(ta1ByInUse, daByInUse, ta1ByInUse == daByInUse);
                LOOP2_ASSERT(ta1BkInUse, daBkInUse, ta1BkInUse == daBkInUse);
                LOOP2_ASSERT(ta1ByTotal, daByTotal, ta1ByTotal == daByTotal);
                LOOP2_ASSERT(ta1BkTotal, daBkTotal, ta1BkTotal == daBkTotal);
            }

            if (verbose) cout <<
                "\tCopy non-empty tables without/with supplying an allocator."
                                                                       << endl;

            ASSERT(0 == ta2.numBlocksTotal());
            bdem_Table table2a(table1a, &ta2);
            const int TA2_USAGE_1 = ta2.numBlocksTotal();
            ASSERT(0 < TA2_USAGE_1);
            ASSERT(DA_USAGE_2 == da.numBlocksTotal());
            ASSERT(TA1_USAGE_2 == ta1.numBlocksTotal());
            ASSERT(TA0_USAGE_FINAL == ta0.numBlocksTotal());

            ASSERT(table2a == table1a);

            ASSERT(DA_USAGE_2 == da.numBlocksTotal());
            bdem_Table table2(table1);
            const int DA_USAGE_3 = da.numBlocksTotal();
            ASSERT(DA_USAGE_2 < DA_USAGE_3);
            ASSERT(TA2_USAGE_1 == ta2.numBlocksTotal());
            ASSERT(TA1_USAGE_2 == ta1.numBlocksTotal());
            ASSERT(TA0_USAGE_FINAL == ta0.numBlocksTotal());

            ASSERT(table2a == table2);

            if (verbose) cout <<
                "\tAppend several more rows each copied table." << endl;

            ASSERT(TA2_USAGE_1 == ta2.numBlocksTotal());
            {
                for (int i = 0; i < SEVERAL; ++i) {
                    table2a.appendNullRow();
                }
            }
            const int TA2_USAGE_2 = ta2.numBlocksTotal();
            ASSERT(TA2_USAGE_1 < TA2_USAGE_2);
            ASSERT(DA_USAGE_3 == da.numBlocksTotal());
            ASSERT(TA1_USAGE_2 == ta1.numBlocksTotal());
            ASSERT(TA0_USAGE_FINAL == ta0.numBlocksTotal());

            ASSERT(table2a != table2);

            ASSERT(DA_USAGE_3 == da.numBlocksTotal());
            {
                for (int i = 0; i < SEVERAL; ++i) {
                    table2.appendNullRow();
                }
            }
            const int DA_USAGE_4 = da.numBlocksTotal();
            ASSERT(DA_USAGE_3 < DA_USAGE_4);
            ASSERT(TA2_USAGE_2 == ta2.numBlocksTotal());
            ASSERT(TA1_USAGE_2 == ta1.numBlocksTotal());
            ASSERT(TA0_USAGE_FINAL == ta0.numBlocksTotal());

            ASSERT(table2a == table2);
        }

        if (verbose) cout << "\nVerify Strategies aren't copied." << endl;
        {
            if (verbose) cout << "\tFirst create allocators." << endl;
            bslma_TestAllocator da;  // will be default - must be created first

            bslma_TestAllocator taA;  // A: BDEM_PASS_THROUGH
            bslma_TestAllocator taB;  // B: BDEM_WRITE_ONCE
            bslma_TestAllocator taC;  // C: BDEM_WRITE_MANY
            bslma_TestAllocator taX;  // D: UNSPECIFIED

            bslma_TestAllocator ta;   // Used to create original tables.

            if (verbose) cout << "\tSecond create reference objects." << endl;

            bdem_List mA;  const bdem_List& A = mA;
            ASSERT(0 == A.length());

            loadReferenceA(&mA);
            ASSERT(NUM_ELEMS == A.length());

            vector<bdem_ElemType::Type> t32; // Used to create 32-col tables.
            {
                for (int i = 0; i < NUM_ELEMS; ++i) {
                    t32.push_back(bdem_ElemType::Type(i));
                }
                ASSERT(NUM_ELEMS == t32.size());
            }

            if (verbose) cout <<
             "\tThird create/install a test allocator as the default." << endl;

            const bslma_DefaultAllocatorGuard dag(&da);

            if (verbose) cout <<
                "\tNext initialize tables with explicit strategies." << endl;

            const bdem_AggregateOption::AllocationStrategy SA =
                                       bdem_AggregateOption::BDEM_PASS_THROUGH;
            const bdem_AggregateOption::AllocationStrategy SB =
                                       bdem_AggregateOption::BDEM_WRITE_ONCE;
            const bdem_AggregateOption::AllocationStrategy SC =
                                       bdem_AggregateOption::BDEM_WRITE_MANY;

            ASSERT(0 == da.numBytesInUse());
            ASSERT(0 == ta.numBytesTotal());

            bdem_Table tableAA(SA, &ta);  ASSERT(0 == da.numBytesInUse());
            bdem_Table tableBB(SB, &ta);  ASSERT(0 == da.numBytesInUse());
            bdem_Table tableCC(SC, &ta);  ASSERT(0 == da.numBytesInUse());

                                               ASSERT(0 == da.numBytesInUse());
            tableAA.reset(&t32.front(), t32.size());
                                               ASSERT(0 == da.numBytesInUse());
            tableBB.reset(&t32.front(), t32.size());
                                               ASSERT(0 == da.numBytesInUse());
            tableCC.reset(&t32.front(), t32.size());
                                               ASSERT(0 == da.numBytesInUse());

            const int INITIAL_USAGE = ta.numBytesTotal();
                                               ASSERT(0 <  INITIAL_USAGE);

            LOOP_ASSERT(da.numBytesInUse(), 0 == da.numBytesInUse());

            if (verbose) cout << "\tNow create the four copied tables." <<endl;

            bdem_Table tableA(tableAA, SA, &taA);
            bdem_Table tableB(tableBB, SB, &taB);
            bdem_Table tableC(tableCC, SC, &taC);
            bdem_Table tableX(tableBB,     &taX); // should act like tableA

                    LOOP_ASSERT(da.numBytesInUse(), 0 == da.numBytesInUse());

            ASSERT(0 == da.numBytesTotal());
            ASSERT(INITIAL_USAGE == ta.numBytesTotal());

            int usageA, usageB, usageC, usageX;

            usageA = taA.numBytesTotal();
            usageB = taB.numBytesTotal();
            usageC = taC.numBytesTotal();
            usageX = taX.numBytesTotal();          // Should be same as usageA.

            if (veryVerbose) { P_(usageA) P_(usageB) P_(usageC) P(usageX) }

            if (verbose) cout <<
                "\t\tAppend an unset row to each table." << endl;

                    LOOP_ASSERT(da.numBytesInUse(), 0 == da.numBytesInUse());
            tableA.appendNullRow();
                    LOOP_ASSERT(da.numBytesInUse(), 0 == da.numBytesInUse());
            tableB.appendNullRow();
                    LOOP_ASSERT(da.numBytesInUse(), 0 == da.numBytesInUse());
            tableC.appendNullRow();
                    LOOP_ASSERT(da.numBytesInUse(), 0 == da.numBytesInUse());
            tableX.appendNullRow();
                    LOOP_ASSERT(da.numBytesInUse(), 0 == da.numBytesInUse());

            usageA = taA.numBytesTotal();
            usageB = taB.numBytesTotal();
            usageC = taC.numBytesTotal();
            usageX = taX.numBytesTotal();          // Should be same as usageA.

            if (veryVerbose) { P_(usageA) P_(usageB) P_(usageC) P(usageX) }

            if (verbose) cout <<
                "\t\tAssign each element its 'A' value." << endl;

            int i; // reused loop index

                    LOOP_ASSERT(da.numBytesInUse(), 0 == da.numBytesInUse());
            for (i = 0; i < NUM_ELEMS; ++i) {
                tableA.theModifiableRow(0)[i].replaceValue(A[i]);
            }
                    LOOP_ASSERT(da.numBytesInUse(), 0 == da.numBytesInUse());
            for (i = 0; i < NUM_ELEMS; ++i) {
                tableB.theModifiableRow(0)[i].replaceValue(A[i]);
            }
                    LOOP_ASSERT(da.numBytesInUse(), 0 == da.numBytesInUse());
            for (i = 0; i < NUM_ELEMS; ++i) {
                tableC.theModifiableRow(0)[i].replaceValue(A[i]);
            }
                    LOOP_ASSERT(da.numBytesInUse(), 0 == da.numBytesInUse());
            for (i = 0; i < NUM_ELEMS; ++i) {
                tableX.theModifiableRow(0)[i].replaceValue(A[i]);
            }
                    LOOP_ASSERT(da.numBytesInUse(), 0 == da.numBytesInUse());

            usageA = taA.numBytesTotal();
            usageB = taB.numBytesTotal();
            usageC = taC.numBytesTotal();
            usageX = taX.numBytesTotal();          // Should be same as usageA.

            if (veryVerbose) { P_(usageA) P_(usageB) P_(usageC) P(usageX)
                                   P(tableA) P(tableB) P(tableC) }

            if (verbose) cout <<
                "\t\tAppend another unset row to each table." << endl;

                    LOOP_ASSERT(da.numBytesInUse(), 0 == da.numBytesInUse());
            tableA.appendNullRow();
                    LOOP_ASSERT(da.numBytesInUse(), 0 == da.numBytesInUse());
            tableB.appendNullRow();
                    LOOP_ASSERT(da.numBytesInUse(), 0 == da.numBytesInUse());
            tableC.appendNullRow();
                    LOOP_ASSERT(da.numBytesInUse(), 0 == da.numBytesInUse());
            tableX.appendNullRow();
                    LOOP_ASSERT(da.numBytesInUse(), 0 == da.numBytesInUse());

            usageA = taA.numBytesTotal();
            usageB = taB.numBytesTotal();
            usageC = taC.numBytesTotal();
            usageX = taX.numBytesTotal();          // Should be same as usageA.

            if (veryVerbose) { P_(usageA) P_(usageB) P_(usageC) P(usageX)
                                   P(tableA) P(tableB) P(tableC) }

            ASSERT(tableA == tableX);
            ASSERT(tableB == tableX);
            ASSERT(tableC == tableX);

            ASSERT(usageA == usageX);  // if fail: either bad copy or default
            ASSERT(usageB != usageX);  // if fail: either bad copy or default
            ASSERT(usageC != usageX);  // if fail: bad default was
                                       // BDEM_WRITE_MANY!

            ASSERT(usageC != usageX); // if fail: bad default was
                                      // 'BDEM_WRITE_MANY'!

            ASSERT(0 == da.numBytesInUse());
            ASSERT(INITIAL_USAGE == ta.numBytesTotal());
                      LOOP_ASSERT(da.numBytesInUse(), 0 == da.numBytesInUse());

            if (veryVeryVerbose) P(tableX)

        } //  da is un-installed as default allocator and then leaves scope.
      }

DEFINE_TEST_CASE(6) {
        // --------------------------------------------------------------------
        // EQUALITY OPERATORS: '==' and '!='
        //   Ensure that these operators correctly interpret value.
        //
        // Concerns:
        //   - That equality is observed.
        //   - That subtle inequality is observed.
        //   - That the allocator is not part of the value.
        //   - That the allocation strategy is not part of the value.
        //
        // Plan:
        //   - Verify that we can append an arbitrary element via the
        //      'appendElement' method taking a const 'bdem_ConstElemRef' arg.
        //   - Ensure that the empty lists compare ==.
        //   - Create 2 multi-dimensional arrays of homogeneous tables
        //      of varying numbers of rows and columns (from reference list A).
        //       + Verify alias case is always ==.
        //       + Verify that equal type/values are equal.
        //       + Verify that changing any value makes them not equal.
        //       + Verify each table is distinct from every other table.
        //   - Create similar 1-row lists each containing all 32 types as
        //      columns, but with varying allocators and allocation strategies;
        //      verify that all lists compare equal.
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //   - Loop-Based Implementation Techniques
        //
        // Testing:
        //   operator==(const bdem_Table& lhs, const bdem_Table& rhs);
        //   operator!=(const bdem_Table& lhs, const bdem_Table& rhs);
        // --------------------------------------------------------------------

        bslma_TestAllocator defaultAllocator;
        const bslma_DefaultAllocatorGuard dag(&defaultAllocator);

        if (verbose) cout << endl
                          << "EQUALITY OPERATORS '==' and '!='" << endl
                          << "================================" << endl;

        if (verbose) cout <<
            "\nCreate reference lists with each element type." << endl;

        bdem_List mA; const bdem_List& A = mA; ASSERT(0         == A.length());
        loadReferenceA(&mA);                   ASSERT(NUM_ELEMS == A.length());
        bdem_List mB; const bdem_List& B = mB; ASSERT(0         == B.length());
        loadReferenceB(&mB);                   ASSERT(NUM_ELEMS == B.length());
        if (veryVeryVerbose) { T_ P(A) T_ P(B) }

        if (verbose) cout <<
                 "\nCreate 2 similar multi-dimensional arrays of small tables."
                                                                       << endl;

        //      ,Populated from reference list A (e.g., ``[ "one" ]'' )  .
        //     / ,Type index (i.e., ``STRING_ARRAY'')
        //    / /
        //   TA[16][3][2]
        //          \  `
        //           \  `Number of columns (i.e., 2)
        //            `Number of rows (i.e., 3)
        //

        const int NT = 32; // number of types
        const int NR = 4;  // number of unique row tests (max num rows is NR-1)
        const int NC = 3;  // number of unique col tests (max num cols is NC-1)

        bdem_Table mTA[NT][NR][NC];
        bdem_Table mTX[NT][NR][NC];
        {
            for (int index = 0; index < NUM_ELEMS; ++index) {
                bdem_ElemType::Type type = A.elemType(index);
                vector<bdem_ElemType::Type> types;
                if (veryVerbose) { T_ P(type) }

                bool PRINT = veryVerbose && (index == 0 || index == 21)
                          || veryVeryVerbose;

                for (int columns = 0; columns < NC; ++columns) {
                    if (PRINT) { T_ T_ P_(columns) P(types) }

                    for (int rows = 0; rows < NR; ++rows) {
                        if (PRINT) { T_ T_ T_ P(rows) }

                        // This block is for each element in TA and TB.

                        bdem_Table& ta = mTA[index][rows][columns];
                        bdem_Table& tx = mTX[index][rows][columns];

                        if (types.size()) {
                            ta.reset(&types.front(), types.size());
                            tx.reset(&types.front(), types.size());
                        }
                        else {
                            ta.removeAll();
                            tx.removeAll();
                        }

                        for (int r = 0; r < rows; ++r) {
                            if (veryVeryVerbose) { T_ T_ T_ T_ P(r) }
                            ta.appendNullRow();
                            tx.appendNullRow();

                            for (int c = 0; c < columns; ++c) {
                                if (veryVeryVerbose) { T_ T_ T_ T_ T_ P(c) }
                                ta.theModifiableRow(r)[c].replaceValue(
                                                                     A[index]);
                                tx.theModifiableRow(r)[c].replaceValue(
                                                                     A[index]);
                            }
                        }
                        if (PRINT) { P(ta) P(tx) }

                    } // for each number of 'rows'

                    types.push_back(type); // increase the number of columns

                } // for each number of 'columns'

            } // for each type 'index'

        } // end of initialization block

        if (verbose) cout <<
            "\nVerify logical correctness of op== and op!==." << endl;
        {
            for (int index = 0; index < NUM_ELEMS; ++index) {
                if (veryVerbose) { T_ P(index) }
                for (int rows = 0; rows < NR; ++rows) {
                    if (veryVerbose) { T_ T_ P(rows) }

                    for (int columns = 0; columns < NC; ++columns) {
                        if (veryVerbose) { T_ T_ T_ P(columns) }

                        // This block is for each element in TA and TX.
                        bdem_Table& ta = mTA[index][rows][columns];
                        bdem_Table& tx = mTX[index][rows][columns];

                        // Identical tables are always equal.
                        LOOP3_ASSERT(index, columns, rows, 1 == (ta == ta));
                        LOOP3_ASSERT(index, columns, rows, 1 == (tx == tx));
                        LOOP3_ASSERT(index, columns, rows, 0 == (ta != ta));
                        LOOP3_ASSERT(index, columns, rows, 0 == (tx != tx));

                        // Equivalent tables are equal.
                        LOOP3_ASSERT(index, columns, rows, 1 == (ta == tx));
                        LOOP3_ASSERT(index, columns, rows, 0 == (ta != tx));

                        // Now perturb the value of each element in turn and
                        // verify that equivalent tables are not equal iff
                        // table contains at least one element.

                        for (int r = 0; r < rows; ++r) {
                            if (veryVeryVerbose) { T_ T_ T_ T_ P(r) }

                            for (int c = 0; c < columns; ++c) {
                                if (veryVeryVerbose) { T_ T_ T_ T_ T_ P(c) }
        //----------------------^
        // Almost equal tables are NOT equal.
        LOOP5_ASSERT(index, rows, columns, r, c, 1 == (ta == tx));
        LOOP5_ASSERT(index, rows, columns, r, c, 0 == (ta != tx));

        tx.theModifiableRow(r)[c].replaceValue(B[index]);
        LOOP6_ASSERT(index, rows, columns, r, tx.theModifiableRow(r)[c],
                     ta.theModifiableRow(r)[c], 0 == (ta == tx));
        LOOP6_ASSERT(index, rows, columns, r, c, tx.theModifiableRow(r)[c],
                     1 == (ta != tx));

        tx.theModifiableRow(r)[c].replaceValue(A[index]);
        LOOP5_ASSERT(index, rows, columns, r, c, 1 == (ta == tx));
        LOOP5_ASSERT(index, rows, columns, r, c, 0 == (ta != tx));
        //----------------------v
                            }
                        }

                        // Now compare with every other table to make sure
                        // that any difference in number of rows/columns
                        // imply inequality.

                        for (int i = 0; i < NT; ++i) {
                            if (veryVeryVerbose) { T_ T_ T_ T_ P(i) }

                            for (int r = 0; r < NR; ++r) {
                                if (veryVeryVerbose) { T_ T_ T_ T_ T_ P(r) }
        //----------------------^
        for (int c = 0; c < NC; ++c) {
            bdem_Table& ty = mTX[i][r][c];

            // The other table is the same only if it has the same dimensions;
            // note that the element type matters only if the table contains at
            // least one column.

            const bool isSame = rows == r
                             && columns == c
                             && (index == i || 0 == c);

            if (veryVeryVerbose) { T_ T_ T_ T_ T_ T_ P_(c) P(isSame) }

            LOOP6_ASSERT(index, rows, columns, i, r, c,  isSame == (ta == ty));
            LOOP6_ASSERT(index, rows, columns, i, r, c, !isSame == (ta != ty));

        } // for each other column 'c'
        //----------------------v
                            } // for each other row 'r'

                        } // for each other index 'i'

                    } // for each number of 'columns'

                } // for each number of 'rows'

            } // for each type 'index'

        } // end of block

        if (verbose) cout <<
            "\nVerify that allocator and allocation mode to not affect value."
                                                                       << endl;
        {
            vector<bdem_ElemType::Type> t32; // Used to create 32-col tables.
            {
                for (int i = 0; i < NUM_ELEMS; ++i) {
                    t32.push_back(bdem_ElemType::Type(i));
                }
                ASSERT(NUM_ELEMS == t32.size());
            }

            bslma_TestAllocator a1;
            bslma_TestAllocator a2;
            bslma_TestAllocator a3;
            bslma_TestAllocator a4;
            bdema_MultipoolAllocator ma4(20, &a4); // handle 'BDEM_SUBORDINATE'

            const bdem_AggregateOption::AllocationStrategy S1 =
                                       bdem_AggregateOption::BDEM_PASS_THROUGH;
            const bdem_AggregateOption::AllocationStrategy S2 =
                                       bdem_AggregateOption::BDEM_WRITE_MANY;
            const bdem_AggregateOption::AllocationStrategy S3 =
                                       bdem_AggregateOption::BDEM_WRITE_ONCE;
            const bdem_AggregateOption::AllocationStrategy S4 =
                                       bdem_AggregateOption::BDEM_SUBORDINATE;

            if (verbose) cout <<
                "\tCreate tables with different allocators and modes." << endl;
            bdem_Table mT0;           const bdem_Table& T0 = mT0;
            bdem_Table mT1(S1, &a1);  const bdem_Table& T1 = mT1;
            bdem_Table mT2(S2, &a2);  const bdem_Table& T2 = mT2;
            bdem_Table mT3(S3, &a3);  const bdem_Table& T3 = mT3;
            bdem_Table mT4(S4, &ma4); const bdem_Table& T4 = mT4;
            if (veryVerbose) { T_ T_ P(T0) }

            ASSERT(1 == (T0 == T1)); ASSERT(0 == (T0 != T1));
            ASSERT(1 == (T0 == T2)); ASSERT(0 == (T0 != T2));
            ASSERT(1 == (T0 == T3)); ASSERT(0 == (T0 != T3));
            ASSERT(1 == (T0 == T4)); ASSERT(0 == (T0 != T4));

            if (verbose) cout <<
                "\tReset each table to have 32 unique column type." << endl;

            mT0.reset(&t32.front(), t32.size());
            mT1.reset(&t32.front(), t32.size());
            mT2.reset(&t32.front(), t32.size());
            mT3.reset(&t32.front(), t32.size());
            mT4.reset(&t32.front(), t32.size());
            if (veryVerbose) { T_ T_ P(T0) }

            ASSERT(1 == (T0 == T1)); ASSERT(0 == (T0 != T1));
            ASSERT(1 == (T0 == T2)); ASSERT(0 == (T0 != T2));
            ASSERT(1 == (T0 == T3)); ASSERT(0 == (T0 != T3));
            ASSERT(1 == (T0 == T4)); ASSERT(0 == (T0 != T4));

            if (verbose) cout << "\tAppend null rows to each table." << endl;
            mT0.appendNullRow();
            mT1.appendNullRow();
            mT2.appendNullRow();
            mT3.appendNullRow();
            mT4.appendNullRow();
            if (veryVerbose) { T_ T_ P(T0) }

            ASSERT(1 == (T0 == T1)); ASSERT(0 == (T0 != T1));
            ASSERT(1 == (T0 == T2)); ASSERT(0 == (T0 != T2));
            ASSERT(1 == (T0 == T3)); ASSERT(0 == (T0 != T3));
            ASSERT(1 == (T0 == T4)); ASSERT(0 == (T0 != T4));

            if (verbose) cout << "\tAssign 'A' values to each column." << endl;
            for (int i = 0; i < NUM_ELEMS; ++i) {
                mT0.theModifiableRow(0)[i].replaceValue(A[i]);
                mT1.theModifiableRow(0)[i].replaceValue(A[i]);
                mT2.theModifiableRow(0)[i].replaceValue(A[i]);
                mT3.theModifiableRow(0)[i].replaceValue(A[i]);
                mT4.theModifiableRow(0)[i].replaceValue(A[i]);
            }
            if (veryVerbose) { T_ T_ P(T0) }

            ASSERT(1 == (T0 == T1)); ASSERT(0 == (T0 != T1));
            ASSERT(1 == (T0 == T2)); ASSERT(0 == (T0 != T2));
            ASSERT(1 == (T0 == T3)); ASSERT(0 == (T0 != T3));
            ASSERT(1 == (T0 == T4)); ASSERT(0 == (T0 != T4));

        } // Note: 'bdema_MultipoolAllocator' releases memory on destruction.

      }

DEFINE_TEST_CASE(5) {
        // --------------------------------------------------------------------
        // OUTPUT: bsl::ostream& 'operator<<' and 'print'
        //
        // Concerns:
        //   - that value is formatted correctly (e.g., '{}' vs '[]' for list).
        //   - that non-equal empty tables print differently: { [ INT ] { 5 } }
        //   - that character and string data is quoted (') & (") respectively.
        //   - that no additional characters are written after terminating.
        //   - that these functions work on references to 'const' instances.
        //   - that each return a reference to the modifiable stream argument.
        //   - that print properly forwards its arguments.
        //   - that negative 'level' suppresses indentation on first line.
        //   - that negative 'spacesPerLevel' forces single-line output.
        //   - that negative 'spacesPerLevel' does NOT suppress indentation.
        //   - that if an allocator is supplied, it is not used for temporary
        //      allocations (e.g., to format float or double values).
        //   - that if an allocator is supplied, the default allocator is not
        //      used AT ALL!
        //
        //  Note that formatting FLOAT, DOUBLE, FLOAT_ARRAY, AND DOUBLE_ARRAY
        //  appear to be problematic with respect to use of the default
        //  allocator.
        //
        // Plan:
        //  For each of a small representative set of object values use
        //  'ostrstream' to write that object's value to two separate
        //  character buffers each with different initial values.  Compare
        //  the contents of these buffers with the literal expected output
        //  format and verify that the characters beyond the null characters
        //  are unaffected in both buffers.
        //   - Test operator<< on the empty Table
        //   - Test operator<< on all 32 1-col tables, with 0, 1, and 2 rows.
        //   - Test print on each of the following with various arguments:
        //      + the empty table: { [ ] }
        //      + a one-element table (containing a scalar):
        //          { [ CHAR ] { 'A' } ]
        //      + a one-element table (containing an array)
        //          { [ INT_ARRAY ] { [ 10 ] } }
        //      + a two-column table with two rows:
        //          { [ INT STRING_ARRAY] { 10 "one" } { 20 "two" } }
        //  To insure that no memory is allocated from the default allocator
        //  during printing, install a test allocator as the default and also
        //  supply a separate test allocator explicitly.  Measure the total
        //  usage of both allocators before and after printing:
        //      + Create an empty ostrstream.
        //      + Print an empty table.
        //      + Print a table with all 32 kinds of elements with/without row.
        //      + Print all of the 32 kinds of 1-element tables.
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //   - Table-Driven Implementation Technique
        //
        // Testing:
        //   operator<<(bsl::ostream& stream, bdem_Table rhs);
        //   bsl::ostream& print(bsl::ostream& s, int l, int spl) const;
        //   operator<<(ostream&, const bsl::vector<T>&);
        // --------------------------------------------------------------------

        DECLARE_MAIN_VARIABLES

        bslma_TestAllocator defaultAllocator;
        const bslma_DefaultAllocatorGuard dag(&defaultAllocator);

        if (verbose) cout << endl
                  << "OUTPUT bsl::ostream& 'operator<<' and 'print'" << endl
                  << "=============================================" << endl;

        if (verbose) cout << "\nCreate tables to be printed." << endl;

        if (veryVerbose) cout << "\tT" << endl;
        const bdem_Table T;
        if (veryVeryVerbose) { T_ T_ P(T) }

        bdem_ElemType::Type t;  // Used to create single-element tables.

        if (veryVerbose) cout <<                   "\tT_A00" << endl;
        bdem_Table mT_A00; const bdem_Table& T_A00 = mT_A00;
                                 t = bdem_ElemType::Type( 0);
                   mT_A00.reset(&t, 1);
                   mT_A00.appendNullRow();
                   mT_A00.theModifiableRow(0)[0].theModifiableChar()     = A00;
        if (veryVeryVerbose) { T_ T_                          P(T_A00) }

        if (veryVerbose) cout <<                   "\tT_A01" << endl;
        bdem_Table mT_A01; const bdem_Table& T_A01 = mT_A01;
                                 t = bdem_ElemType::Type( 1);
                   mT_A01.reset(&t, 1);
                   mT_A01.appendNullRow();
                   mT_A01.theModifiableRow(0)[0].theModifiableShort()    = A01;
        if (veryVeryVerbose) { T_ T_                          P(T_A01) }

        if (veryVerbose) cout <<                   "\tT_A02" << endl;
        bdem_Table mT_A02; const bdem_Table& T_A02 = mT_A02;
                                 t = bdem_ElemType::Type( 2);
                   mT_A02.reset(&t, 1);
                   mT_A02.appendNullRow();
                   mT_A02.theModifiableRow(0)[0].theModifiableInt()      = A02;
        if (veryVeryVerbose) { T_ T_                          P(T_A02) }

        if (veryVerbose) cout <<                   "\tT_A03" << endl;
        bdem_Table mT_A03; const bdem_Table& T_A03 = mT_A03;
                                 t = bdem_ElemType::Type( 3);
                   mT_A03.reset(&t, 1);
                   mT_A03.appendNullRow();
                   mT_A03.theModifiableRow(0)[0].theModifiableInt64()    = A03;
        if (veryVeryVerbose) { T_ T_                          P(T_A03) }

        if (veryVerbose) cout <<                   "\tT_A04" << endl;
        bdem_Table mT_A04; const bdem_Table& T_A04 = mT_A04;
                                 t = bdem_ElemType::Type( 4);
                   mT_A04.reset(&t, 1);
                   mT_A04.appendNullRow();
                   mT_A04.theModifiableRow(0)[0].theModifiableFloat()    = A04;
        if (veryVeryVerbose) { T_ T_                          P(T_A04) }

        if (veryVerbose) cout <<                   "\tT_A05" << endl;
        bdem_Table mT_A05; const bdem_Table& T_A05 = mT_A05;
                                 t = bdem_ElemType::Type( 5);
                   mT_A05.reset(&t, 1);
                   mT_A05.appendNullRow();
                   mT_A05.theModifiableRow(0)[0].theModifiableDouble()   = A05;
        if (veryVeryVerbose) { T_ T_                          P(T_A05) }

        if (veryVerbose) cout <<                   "\tT_A06" << endl;
        bdem_Table mT_A06; const bdem_Table& T_A06 = mT_A06;
                                 t = bdem_ElemType::Type( 6);
                   mT_A06.reset(&t, 1);
                   mT_A06.appendNullRow();
                   mT_A06.theModifiableRow(0)[0].theModifiableString()   = A06;
        if (veryVeryVerbose) { T_ T_                          P(T_A06) }

        if (veryVerbose) cout <<                   "\tT_A07" << endl;
        bdem_Table mT_A07; const bdem_Table& T_A07 = mT_A07;
                                 t = bdem_ElemType::Type( 7);
                   mT_A07.reset(&t, 1);
                   mT_A07.appendNullRow();
                   mT_A07.theModifiableRow(0)[0].theModifiableDatetime() = A07;
        if (veryVeryVerbose) { T_ T_                          P(T_A07) }

        if (veryVerbose) cout <<                   "\tT_A08" << endl;
        bdem_Table mT_A08; const bdem_Table& T_A08 = mT_A08;
                                 t = bdem_ElemType::Type( 8);
                   mT_A08.reset(&t, 1);
                   mT_A08.appendNullRow();
                   mT_A08.theModifiableRow(0)[0].theModifiableDate()     = A08;
        if (veryVeryVerbose) { T_ T_                          P(T_A08) }

        if (veryVerbose) cout <<                   "\tT_A09" << endl;
        bdem_Table mT_A09; const bdem_Table& T_A09 = mT_A09;
                                 t = bdem_ElemType::Type( 9);
                   mT_A09.reset(&t, 1);
                   mT_A09.appendNullRow();
                   mT_A09.theModifiableRow(0)[0].theModifiableTime()     = A09;
        if (veryVeryVerbose) { T_ T_                          P(T_A09) }

        if (veryVerbose) cout <<                   "\tT_A10" << endl;
        bdem_Table mT_A10; const bdem_Table& T_A10 = mT_A10;
                                 t = bdem_ElemType::Type(10);
                   mT_A10.reset(&t, 1);
                   mT_A10.appendNullRow();
                   mT_A10.theModifiableRow(0)[0].theModifiableCharArray()= A10;
        if (veryVeryVerbose) { T_ T_                          P(T_A10) }

        if (veryVerbose) cout <<                   "\tT_A10B" << endl;
        bdem_Table mT_A10B; const bdem_Table& T_A10B = mT_A10B;
                                 t = bdem_ElemType::Type(10);
        mT_A10B.reset(&t, 1);
        mT_A10B.appendNullRow();
        mT_A10B.theModifiableRow(0)[0].theModifiableCharArray() = A10;
       mT_A10B.theModifiableRow(0)[0].theModifiableCharArray().push_back(0x81);
        if (veryVeryVerbose) { T_ T_                          P(T_A10B) }

        if (veryVerbose) cout <<                   "\tT_A10C" << endl;
        bdem_Table mT_A10C; const bdem_Table& T_A10C = mT_A10C;
                                 t = bdem_ElemType::Type(10);
        mT_A10C.reset(&t, 1);
        mT_A10C.appendNullRow();
        mT_A10C.theModifiableRow(0)[0].theModifiableCharArray()= A10;
       mT_A10C.theModifiableRow(0)[0].theModifiableCharArray().push_back(0xC8);
        if (veryVeryVerbose) { T_ T_                          P(T_A10C) }

        if (veryVerbose) cout <<                   "\tT_A11" << endl;
        bdem_Table mT_A11; const bdem_Table& T_A11 = mT_A11;
                                 t = bdem_ElemType::Type(11);
        mT_A11.reset(&t, 1);
        mT_A11.appendNullRow();
        mT_A11.theModifiableRow(0)[0].theModifiableShortArray() = A11;
        if (veryVeryVerbose) { T_ T_                          P(T_A11) }

        if (veryVerbose) cout <<                   "\tT_A12" << endl;
        bdem_Table mT_A12; const bdem_Table& T_A12 = mT_A12;
                                 t = bdem_ElemType::Type(12);
                   mT_A12.reset(&t, 1);
                   mT_A12.appendNullRow();
                   mT_A12.theModifiableRow(0)[0].theModifiableIntArray() = A12;
        if (veryVeryVerbose) { T_ T_                          P(T_A12) }

        if (veryVerbose) cout <<                   "\tT_A13" << endl;
        bdem_Table mT_A13; const bdem_Table& T_A13 = mT_A13;
                                 t = bdem_ElemType::Type(13);
        mT_A13.reset(&t, 1);
        mT_A13.appendNullRow();
        mT_A13.theModifiableRow(0)[0].theModifiableInt64Array() = A13;
        if (veryVeryVerbose) { T_ T_                          P(T_A13) }

        if (veryVerbose) cout <<                   "\tT_A14" << endl;
        bdem_Table mT_A14; const bdem_Table& T_A14 = mT_A14;
                                 t = bdem_ElemType::Type(14);
        mT_A14.reset(&t, 1);
        mT_A14.appendNullRow();
        mT_A14.theModifiableRow(0)[0].theModifiableFloatArray() = A14;
        if (veryVeryVerbose) { T_ T_                          P(T_A14) }

        if (veryVerbose) cout <<                   "\tT_A15" << endl;
        bdem_Table mT_A15; const bdem_Table& T_A15 = mT_A15;
                                 t = bdem_ElemType::Type(15);
                   mT_A15.reset(&t, 1);
                   mT_A15.appendNullRow();
                   mT_A15.theModifiableRow(0)[0].theModifiableDoubleArray() =
                                                                           A15;
        if (veryVeryVerbose) { T_ T_                          P(T_A15) }

        if (veryVerbose) cout <<                   "\tT_A16" << endl;
        bdem_Table mT_A16; const bdem_Table& T_A16 = mT_A16;
                                 t = bdem_ElemType::Type(16);
                   mT_A16.reset(&t, 1);
                   mT_A16.appendNullRow();
                   mT_A16.theModifiableRow(0)[0].theModifiableStringArray() =
                                                                           A16;
        if (veryVeryVerbose) { T_ T_                          P(T_A16) }

        if (veryVerbose) cout <<                   "\tT_A17" << endl;
        bdem_Table mT_A17; const bdem_Table& T_A17 = mT_A17;
                                 t = bdem_ElemType::Type(17);
                   mT_A17.reset(&t, 1);
                   mT_A17.appendNullRow();
                   mT_A17.theModifiableRow(0)[0].theModifiableDatetimeArray() =
                                                                           A17;
        if (veryVeryVerbose) { T_ T_                          P(T_A17) }

        if (veryVerbose) cout <<                   "\tT_A18" << endl;
        bdem_Table mT_A18; const bdem_Table& T_A18 = mT_A18;
                                 t = bdem_ElemType::Type(18);
                   mT_A18.reset(&t, 1);
                   mT_A18.appendNullRow();
                   mT_A18.theModifiableRow(0)[0].theModifiableDateArray()= A18;
        if (veryVeryVerbose) { T_ T_                          P(T_A18) }

        if (veryVerbose) cout <<                   "\tT_A19" << endl;
        bdem_Table mT_A19; const bdem_Table& T_A19 = mT_A19;
                                 t = bdem_ElemType::Type(19);
                   mT_A19.reset(&t, 1);
                   mT_A19.appendNullRow();
                   mT_A19.theModifiableRow(0)[0].theModifiableTimeArray()= A19;
        if (veryVeryVerbose) { T_ T_                          P(T_A19) }

        if (veryVerbose) cout <<                   "\tT_A20" << endl;
        bdem_Table mT_A20; const bdem_Table& T_A20 = mT_A20;
                                 t = bdem_ElemType::Type(20);
                   mT_A20.reset(&t, 1);
                   mT_A20.appendNullRow();
                   mT_A20.theModifiableRow(0)[0].theModifiableList()     = A20;
        if (veryVeryVerbose) { T_ T_                          P(T_A20) }

        if (veryVerbose) cout <<                   "\tT_A21" << endl;
        bdem_Table mT_A21; const bdem_Table& T_A21 = mT_A21;
                                 t = bdem_ElemType::Type(21);
                   mT_A21.reset(&t, 1);
                   mT_A21.appendNullRow();
                   mT_A21.theModifiableRow(0)[0].theModifiableTable()    = A21;
        if (veryVeryVerbose) { T_ T_                          P(T_A21) }

        vector<bdem_ElemType::Type> t32; // Used to create 32-column tables.
        {
            for (int i = 0; i < NUM_ELEMS; ++i) {
                t32.push_back(bdem_ElemType::Type(i));
            }
            ASSERT(NUM_ELEMS == t32.size());
        }

        if (veryVerbose) cout << "\tT32" << endl;
        bdem_Table mT32; const bdem_Table& T32 = mT32;
                   mT32.reset(&t32.front(), t32.size());
        if (veryVeryVerbose) { T_ T_ P(T32) }

        bdem_List mA; const bdem_List& A = mA; ASSERT(0         == A.length());
        loadReferenceA(&mA);                   ASSERT(NUM_ELEMS == A.length());

        if (veryVerbose) cout << "\tT32A" << endl;
        bdem_Table mT32A; const bdem_Table& T32A = mT32A;
                   mT32A.reset(&t32.front(), t32.size());
                   mT32A.appendNullRow();
                   mT32A.theModifiableRow(0).replaceValues(A.row());
        if (veryVeryVerbose) { T_ T_ P(T32A) }

        vector<bdem_ElemType::Type> t2; // used to create 2-column table.
        t2.push_back(bdem_ElemType::BDEM_INT);
        t2.push_back(bdem_ElemType::BDEM_STRING_ARRAY);

        if (veryVerbose) cout << "\tT2AB" << endl;
        bdem_Table mT2AB; const bdem_Table& T2AB = mT2AB;
                   mT2AB.reset(t2);
                   mT2AB.appendNullRow();
                   mT2AB.appendNullRow();
                   mT2AB.theModifiableRow(0)[0].theModifiableInt() = A02;
                   mT2AB.theModifiableRow(0)[1].theModifiableStringArray() =
                                                                           A16;
                   mT2AB.theModifiableRow(1)[0].theModifiableInt() = B02;
                   mT2AB.theModifiableRow(1)[1].theModifiableStringArray() =
                                                                           B16;

        if (veryVeryVerbose) { T_ T_ P(T2AB) }

// TBD
//Q(FIX ME - CHAR data should be enclosed in single-quotes when printed)
//Q(FIX ME - STRING data should be enclosed in double-quotes when printed)
//
//Note: doing these fixes would involve changing bdem_properties.cpp, and
//affect the print() behavior of many components, so many .t.cpp's would have
//to be updated.

        if (verbose) cout << "\nTesting 'operator<<' (ostream)." << endl;
        {
            static const struct {
                int               d_lineNum; // source line number
                const bdem_Table *d_obj_p;   // address of object to be printed
                const char       *d_fmt_p;   // expected output format
            } DATA[] = {
                //L# Object  Format
                //-- ------  --------------------------------------------------
                {L_, &T,     "{ Column Types: [ ]}"                          },
                {L_, &T_A00, "{ Column Types: [ CHAR ] Row 0: { A }}"        },
                {L_, &T_A01, "{ Column Types: [ SHORT ] Row 0: { -1 }}"      },
                {L_, &T_A02, "{ Column Types: [ INT ] Row 0: { 10 }}"        },
                {L_, &T_A03, "{ Column Types: [ INT64 ] Row 0: { -100 }}"    },
                {L_, &T_A04, "{ Column Types: [ FLOAT ] Row 0: { -1.5 }}"    },
                {L_, &T_A05, "{ Column Types: [ DOUBLE ] Row 0: { 10.5 }}"   },
                {L_, &T_A06, "{ Column Types: [ STRING ] Row 0: { one }}"    },
                {L_, &T_A07,
           "{ Column Types: [ DATETIME ] Row 0: { 01JAN2000_00:01:02.003 }}" },
                {L_, &T_A08,
                            "{ Column Types: [ DATE ] Row 0: { 01JAN2000 }}" },
                {L_, &T_A09,
                         "{ Column Types: [ TIME ] Row 0: { 00:01:02.003 }}" },
                {L_, &T_A10,
                          "{ Column Types: [ CHAR_ARRAY ] Row 0: { \"A\" }}" },
                {L_, &T_A10B,
                     "{ Column Types: [ CHAR_ARRAY ] Row 0: { \"A\\x81\" }}" },
                {L_, &T_A10C,
                     "{ Column Types: [ CHAR_ARRAY ] Row 0: { \"A\\xC8\" }}" },
                {L_, &T_A11,
                        "{ Column Types: [ SHORT_ARRAY ] Row 0: { [ -1 ] }}" },
                {L_, &T_A12,
                          "{ Column Types: [ INT_ARRAY ] Row 0: { [ 10 ] }}" },
                {L_, &T_A13,
                      "{ Column Types: [ INT64_ARRAY ] Row 0: { [ -100 ] }}" },
                {L_, &T_A14,
                      "{ Column Types: [ FLOAT_ARRAY ] Row 0: { [ -1.5 ] }}" },
                {L_, &T_A15,
                     "{ Column Types: [ DOUBLE_ARRAY ] Row 0: { [ 10.5 ] }}" },
                {L_, &T_A16,
                      "{ Column Types: [ STRING_ARRAY ] Row 0: { [ one ] }}" },
                {L_, &T_A17,
 "{ Column Types: [ DATETIME_ARRAY ] Row 0: { [ 01JAN2000_00:01:02.003 ] }}" },
                {L_, &T_A18,
                  "{ Column Types: [ DATE_ARRAY ] Row 0: { [ 01JAN2000 ] }}" },
                {L_, &T_A19,
               "{ Column Types: [ TIME_ARRAY ] Row 0: { [ 00:01:02.003 ] }}" },
                {L_, &T_A20,
                           "{ Column Types: [ LIST ] Row 0: { { INT 10 } }}" },
                {L_, &T_A21,
          "{ Column Types: [ TABLE ] Row 0: { { Column Types: [ CHAR ] } }}" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int SIZE = 1000; // Must be big enough to hold output string.
            const char Z1 = (char) 0xff;  // Used to represent an unset char.
            const char Z2 = 0x00;  // Value 2 used to represent an unset char.

            char mCtrlBuf1[SIZE];  memset(mCtrlBuf1, Z1, SIZE);
            char mCtrlBuf2[SIZE];  memset(mCtrlBuf2, Z2, SIZE);
            const char *CTRL_BUF1 = mCtrlBuf1;
            const char *CTRL_BUF2 = mCtrlBuf2;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE        =  DATA[ti].d_lineNum;
                const bdem_Table& OBJ = *DATA[ti].d_obj_p;
                const char *const FMT =  DATA[ti].d_fmt_p;

                char buf1[SIZE], buf2[SIZE];
                memcpy(buf1, CTRL_BUF1, SIZE); // Preset buf1 to Z1 values.
                memcpy(buf2, CTRL_BUF2, SIZE); // Preset buf2 to Z2 values.

                if (veryVerbose) cout   // print format BEFORE output operation
                    << "EXPECTED FORMAT: ``" << FMT << "''" << endl;

                ostrstream out1(buf1, SIZE), out2(buf2, SIZE);
                out1 << OBJ << ends;  // Ensure modifiable
                out2 << OBJ << ends;  // stream is returned.

                const int SZ = strlen(FMT) + 1;
                const int REST = SIZE - SZ;
                const bool failure = 0 != memcmp(buf1, FMT, SZ);

                if (failure) {
                    ++testStatus;
                    cout << "Error, LINE = " << LINE << endl;
                    cout << "EXPECTED FORMAT:(" << strlen(FMT)  <<
                                                ") ``" << FMT  << "''" << endl;
                    cout << "  ACTUAL FORMAT:(" << strlen(buf1) <<
                                                ") ``" << buf1 << "''" << endl;
                }

                LOOP_ASSERT(LINE, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(LINE, Z1 == buf1[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE, Z2 == buf2[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE, 0 == memcmp(buf1, FMT, SZ));
                LOOP_ASSERT(LINE, 0 == memcmp(buf2, FMT, SZ));
                LOOP_ASSERT(LINE, 0 == memcmp(buf1 + SZ, CTRL_BUF1 + SZ,REST));
                LOOP_ASSERT(LINE, 0 == memcmp(buf2 + SZ, CTRL_BUF2 + SZ,REST));
            }
        }

        if (verbose) cout << "\nTesting 'print'." << endl;
        {
            static const struct {
                int               d_lineNum; // source line number
                const bdem_Table *d_obj_p;   // address of object to be printed
                int               d_level;
                int               d_spacesPerLevel;
                const char       *d_fmt_p;   // expected output format
            } DATA[] = {
                //L# Object      Lev SPL Format
                //-- ----------  --- --- ------------------------------------
                {L_, &T,         0,  0,  "{\n"
                                         "Column Types: [\n"
                                         "]\n"
                                         "}\n"                               },
                {L_, &T,         2,  1,  "  {\n"
                                         "   Column Types: [\n"
                                         "   ]\n"
                                         "  }\n"                             },
                {L_, &T,         -2, 1,  "{\n"
                                         "   Column Types: [\n"
                                         "   ]\n"
                                         "  }\n"                             },
                {L_, &T,         2,  -1, "  { Column Types: [ ]  }"          },
                //-- ----------  --- --- ------------------------------------
                {L_, &T_A00,     1,  0,  "{\n"
                                         "Column Types: [\n"
                                         "CHAR\n"
                                         "]\n"
                                         "Row 0: {\n"
                                         "A\n"
                                         "}\n"
                                         "}\n"                               },
                {L_, &T_A00,     0,  1,  "{\n"
                                         " Column Types: [\n"
                                         "  CHAR\n"
                                         " ]\n"
                                         " Row 0: {\n"
                                         "  A\n"
                                         " }\n"
                                         "}\n"                               },
                {L_, &T_A00,     1,  3,  "   {\n"
                                         "      Column Types: [\n"
                                         "         CHAR\n"
                                         "      ]\n"
                                         "      Row 0: {\n"
                                         "         A\n"
                                         "      }\n"
                                         "   }\n"                            },
                {L_, &T_A00,     3,  1,  "   {\n"
                                         "    Column Types: [\n"
                                         "     CHAR\n"
                                         "    ]\n"
                                         "    Row 0: {\n"
                                         "     A\n"
                                         "    }\n"
                                         "   }\n"                            },
                //-- ----------  --- --- ------------------------------------
                {L_, &T_A12,     0,  0,  "{\n"
                                         "Column Types: [\n"
                                         "INT_ARRAY\n"
                                         "]\n"
                                         "Row 0: {\n"
                                         "[\n"
                                         "10\n"
                                         "]\n"
                                         "}\n"
                                         "}\n"                               },
                {L_, &T_A12,     3,  2,  "      {\n"
                                         "        Column Types: [\n"
                                         "          INT_ARRAY\n"
                                         "        ]\n"
                                         "        Row 0: {\n"
                                         "          [\n"
                                         "            10\n"
                                         "          ]\n"
                                         "        }\n"
                                         "      }\n"                         },
                {L_, &T_A12,     2,  3,  "      {\n"
                                         "         Column Types: [\n"
                                         "            INT_ARRAY\n"
                                         "         ]\n"
                                         "         Row 0: {\n"
                                         "            [\n"
                                         "               10\n"
                                         "            ]\n"
                                         "         }\n"
                                         "      }\n"                         },
                {L_, &T_A12,     -3, 2,  "{\n"
                                         "        Column Types: [\n"
                                         "          INT_ARRAY\n"
                                         "        ]\n"
                                         "        Row 0: {\n"
                                         "          [\n"
                                         "            10\n"
                                         "          ]\n"
                                         "        }\n"
                                         "      }\n"                         },
                {L_, &T_A12,     3,  -2,
              "      { Column Types: [ INT_ARRAY ] Row 0: { [ 10 ] }      }" },
                {L_, &T_A12,     -3, -2,
                    "{ Column Types: [ INT_ARRAY ] Row 0: { [ 10 ] }      }" },
                //-- ----------  --- --- ------------------------------------
                {L_, &T2AB,      -2,  5, "{\n"
                                         "               Column Types: [\n"
                                         "                    INT\n"
                                         "                    STRING_ARRAY\n"
                                         "               ]\n"
                                         "               Row 0: {\n"
                                         "                    10\n"
                                         "                    [\n"
                                         "                         one\n"
                                         "                    ]\n"
                                         "               }\n"
                                         "               Row 1: {\n"
                                         "                    20\n"
                                         "                    [\n"
                                         "                         two\n"
                                         "                    ]\n"
                                         "               }\n"
                                         "          }\n"                     },
                //-- ----------  --- --- ------------------------------------
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int SIZE = 1000; // Must be big enough to hold output string.
            const char Z1 = (char) 0xff;  // Used to represent an unset char.
            const char Z2 = 0x00;  // Value 2 used to represent an unset char.

            char mCtrlBuf1[SIZE];  memset(mCtrlBuf1, Z1, SIZE);
            char mCtrlBuf2[SIZE];  memset(mCtrlBuf2, Z2, SIZE);
            const char *CTRL_BUF1 = mCtrlBuf1;
            const char *CTRL_BUF2 = mCtrlBuf2;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE        =  DATA[ti].d_lineNum;
                const int LEVEL       =  DATA[ti].d_level;
                const int SPL         =  DATA[ti].d_spacesPerLevel;
                const bdem_Table& OBJ = *DATA[ti].d_obj_p;
                const char *const FMT =  DATA[ti].d_fmt_p;

                char buf1[SIZE], buf2[SIZE];
                memcpy(buf1, CTRL_BUF1, SIZE); // Preset buf1 to Z1 values.
                memcpy(buf2, CTRL_BUF2, SIZE); // Preset buf2 to Z2 values.

                if (veryVerbose) cout   // print format BEFORE output operation
                    << "EXPECTED FORMAT: ``" << FMT << "''" << endl;

                ostrstream out1(buf1, SIZE), out2(buf2, SIZE);
                OBJ.print(out1, LEVEL, SPL) << ends;  // Ensure modifiable
                OBJ.print(out2, LEVEL, SPL) << ends;  // stream is returned.

                const int SZ = strlen(FMT) + 1;
                const int REST = SIZE - SZ;
                const bool failure = 0 != memcmp(buf1, FMT, SZ);

                if (failure) {
                    ++testStatus;
                    cout << "Error: Line = " << LINE << endl;
                    cout << "EXPECTED FORMAT: ``" << FMT  << "''" << endl;
                    cout << "  ACTUAL FORMAT: ``" << buf1 << "''" << endl;
                }

                LOOP_ASSERT(LINE, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(LINE, Z1 == buf1[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE, Z2 == buf2[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE, 0 == memcmp(buf1, FMT, SZ));
                LOOP_ASSERT(LINE, 0 == memcmp(buf2, FMT, SZ));
                LOOP_ASSERT(LINE, 0 == memcmp(buf1 + SZ, CTRL_BUF1 + SZ,REST));
                LOOP_ASSERT(LINE, 0 == memcmp(buf2 + SZ, CTRL_BUF2 + SZ,REST));
            }
        }

        if (verbose) cout <<
           "\nEnsure default allocator not used inappropriately." << endl;
        {
            const int SIZE = 10000; // arbitrarily large size

            bslma_TestAllocator sa;
            stringstream ss(&sa);

            if (verbose) cout << "\tCreate an empty ostrstream." << endl;
            {
                bslma_TestAllocator da; // default allocator
                const bslma_DefaultAllocatorGuard DAG(&da);

                // Verify that creation of, and random output to, stringstream
                // with its own allocator doesn't affect default allocator.

                LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
                bslma_TestAllocator sa;
                stringstream ss(&sa);
                ss << "woof" << 'm' << 17 << 17.4 << (short) 7 << endl << ends;
                LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
            }

            if (verbose) cout << "\tPrint an empty table." << endl;
            {
                bslma_TestAllocator sa;
                stringstream ss(&sa);

                // Note that in this test block, we create the ostream before
                // swapping in our own test allocator.

                bslma_TestAllocator da; // default allocator
                const bslma_DefaultAllocatorGuard DAG(&da);
                bslma_TestAllocator a;  // specified allocator

                LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
                LOOP_ASSERT(a.numBlocksTotal(), 0 == a.numBlocksTotal());

                bdem_Table mX(&a); const bdem_Table& X = mX;
                LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());

                const int USAGE = a.numBlocksTotal();
                if (veryVerbose) { T_ T_ P(USAGE) }
                ASSERT(USAGE > 0);

                if (veryVerbose) {
                    cout << endl <<
                    "************************* BEFORE ************************"
                    << endl;
                    cout << "SPECIFIED ALLOCATOR:" << endl;
                    a.print();
                    cout << "\nDEFAULT ALLOCATOR:" << endl;
                    da.print();
                }
                LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
                LOOP_ASSERT(a.numBlocksTotal(), USAGE == a.numBlocksTotal());

                ss << X << ends;
                if (veryVerbose) P(ss);

                if (veryVerbose) {
                    cout << endl <<
                    "************************* AFTER *************************"
                    << endl;
                    cout << "SPECIFIED ALLOCATOR:" << endl;
                    a.print();
                    cout << "\nDEFAULT ALLOCATOR:" << endl;
                    da.print();
                }
                LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
                LOOP_ASSERT(a.numBlocksTotal(), USAGE == a.numBlocksTotal());
            }

            if (verbose) cout << "\tPrint the table T32." << endl;
            {
                bslma_TestAllocator sa;
                stringstream ss(&sa);

                // Note that in this test block, we again create the ostream
                // before swapping in our own test allocator.

                bslma_TestAllocator da; // default allocator
                const bslma_DefaultAllocatorGuard DAG(&da);
                bslma_TestAllocator a;  // specified allocator

                LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
                LOOP_ASSERT(a.numBlocksTotal(), 0 == a.numBlocksTotal());

                bdem_Table mX(T32, &a); const bdem_Table& X = mX;
                LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());

                const int USAGE = a.numBlocksTotal();
                if (veryVerbose) { T_ T_ P(USAGE) }
                ASSERT(USAGE > 0);

                if (veryVerbose) {
                    cout << endl <<
                    "************************* BEFORE ************************"
                    << endl;
                    cout << "SPECIFIED ALLOCATOR:" << endl;
                    a.print();
                    cout << "\nDEFAULT ALLOCATOR:" << endl;
                    da.print();
                }
                LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
                LOOP_ASSERT(a.numBlocksTotal(), USAGE == a.numBlocksTotal());

                ss << X << ends;
                if (veryVerbose) P(ss);

                if (veryVerbose) {
                    cout << endl <<
                    "************************* AFTER *************************"
                    << endl;
                    cout << "SPECIFIED ALLOCATOR:" << endl;
                    a.print();
                    cout << "\nDEFAULT ALLOCATOR:" << endl;
                    da.print();
                }
                LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
                LOOP_ASSERT(a.numBlocksTotal(), USAGE == a.numBlocksTotal());
            }

            if (verbose) cout << "\tPrint the table T32A." << endl;
            {
                bslma_TestAllocator sa;
                stringstream ss(&sa);

                // Note that in this test block, we yet again create the
                // ostream before swapping in our own test allocator.

                bslma_TestAllocator da; // default allocator
                const bslma_DefaultAllocatorGuard DAG(&da);
                bslma_TestAllocator a;  // specified allocator

                LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
                LOOP_ASSERT(a.numBlocksTotal(), 0 == a.numBlocksTotal());

                bdem_Table mX(T32A, &a); const bdem_Table& X = mX;
                LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());

                const int USAGE = a.numBlocksTotal();
                if (veryVerbose) { T_ T_ P(USAGE) }
                ASSERT(USAGE > 0);

                if (veryVerbose) {
                    cout << endl <<
                    "************************* BEFORE ************************"
                    << endl;
                    cout << "SPECIFIED ALLOCATOR:" << endl;
                    a.print();
                    cout << "\nDEFAULT ALLOCATOR:" << endl;
                    da.print();
                }
                LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
                LOOP_ASSERT(a.numBlocksTotal(), USAGE == a.numBlocksTotal());

                ss << X << ends;
                if (veryVerbose) P(ss);

                if (veryVerbose) {
                    cout << endl <<
                    "************************* AFTER *************************"
                    << endl;
                    cout << "SPECIFIED ALLOCATOR:" << endl;
                    a.print();
                    cout << "\nDEFAULT ALLOCATOR:" << endl;
                    da.print();
                }
//                 LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
                LOOP_ASSERT(a.numBlocksTotal(), USAGE == a.numBlocksTotal());
            }

            if (verbose) cout << "\tPrint each kind of 1-element Table."<<endl;
            for (int i = 0; i < NUM_ELEMS; ++i) {
                if (veryVerbose) { T_ T_
                    Q(########################################################)
                    T_ T_ cout << "BEGIN "; P_(i) P(bdem_ElemType::Type(i))
                }

                bslma_TestAllocator sa;
                stringstream ss(&sa);

                // Note that in this test block, we even yet again create the
                // ostream before swapping in our own test allocator.

                bslma_TestAllocator da; // default allocator
                const bslma_DefaultAllocatorGuard DAG(&da);
                bslma_TestAllocator a;  // specified allocator

                LOOP2_ASSERT(i, da.numBlocksTotal(), 0 == da.numBlocksTotal());
                LOOP2_ASSERT(i, a.numBlocksTotal(), 0 == a.numBlocksTotal());

                bdem_ElemType::Type t = bdem_ElemType::Type(i);
                bdem_Table mX(&a); const bdem_Table& X = mX;
                mX.reset(&t, 1);
                mX.appendNullRow();
                mX.theModifiableRow(0)[0].replaceValue(A.row()[i]);
                LOOP2_ASSERT(i, da.numBlocksTotal(), 0 == da.numBlocksTotal());

                const int USAGE = a.numBlocksTotal();
                if (veryVerbose) { T_ T_ P(USAGE) }
                LOOP_ASSERT(i, USAGE > 0);

                if (veryVerbose) {
                    cout << endl <<
                    "************************* BEFORE ************************"
                    << endl;
                    cout << "SPECIFIED ALLOCATOR:" << endl;
                    a.print();
                    cout << "\nDEFAULT ALLOCATOR:" << endl;
                    da.print();
                }
                LOOP2_ASSERT(i, da.numBlocksTotal(), 0 == da.numBlocksTotal());
                LOOP2_ASSERT(i, a.numBlocksTotal(), USAGE==a.numBlocksTotal());

                ss << X << ends;
                if (veryVerbose) P(ss);

                if (veryVerbose) {
                    cout << endl <<
                    "************************* AFTER *************************"
                    << endl;
                    cout << "SPECIFIED ALLOCATOR:" << endl;
                    a.print();
                    cout << "\nDEFAULT ALLOCATOR:" << endl;
                    da.print();
                }
// TBD
#if 0
                LOOP2_ASSERT(i, da.numBlocksTotal(), 0 == da.numBlocksTotal());
#endif
                LOOP2_ASSERT(i, a.numBlocksTotal(), USAGE==a.numBlocksTotal());

                if (veryVerbose) {
                    T_ T_ cout << "END "; P_(i) P(bdem_ElemType::Type(i))
                }
            }
        }

      }

DEFINE_TEST_CASE(4) {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS/BASIC ACCESSORS
        //   Verify the most basic functionality of a 'bdem_Table'
        //
        // Concerns:
        //  Basically to make sure that functions are forwarding properly:
        //   - that each manipulators delegate to the underlying bdem_tableimp.
        //   - that basic (direct) accessors also delegate correctly.
        //   - that accessors work off of references to 'const' instances.
        //
        // Plan:
        //   - Verify 'loadReferenceA', 'loadReferenceB', 'loadReferenceC',
        //      'loadReferenceD', 'loadReferenceE', 'loadReferenceU' and
        //      'loadReferenceU'.
        //   - Create an empty table and verify its numRows/numColumns;
        //      append (empty) rows and verify numRows/numColumns.
        //   - Create and verify all 32 types of one-column tables; verify
        //      the contents with 0, 1, 2, and 3 rows.
        //   - Create and verify a table consisting of all 32 unique column
        //      types in forward order; verify it with 0, 1, 2, and 3 rows.
        //   - Create a 32-column table with types in reverse order; verify
        //      the contents with 0, 1, 2 rows.
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //   - Brute-Force and Loop-Based Implementation Techniques
        //
        // Testing:
        //   void loadReferenceA(bdem_List *result);
        //   void loadReferenceB(bdem_List *result);
        //   void loadReferenceC(bdem_List *result);
        //   void loadReferenceD(bdem_List *result);
        //   void loadReferenceE(bdem_List *result);
        //   void loadReferenceU(bdem_List *result);
        //   void loadReferenceN(bdem_List *result);
        //
        //   void reset(const bdem_ElemType::Type elementTypes[], int ne);
        //   void reset(const vector<bdem_ElemType::Type>& elementTypes);
        //
        //   void appendNullRow();
        //   void appendNullRow();
        //   bdem_Row& theModifiableRow(int rowIndex);
        //
        //   int numColumns() const;
        //   int numRows() const;
        //   bdem_ElemType::Type columnType(int index) const;
        //   void columnTypes(bsl::vector<bdem_ElemType::Type> *result) const;
        //   const bdem_Row& operator[](int rowIndex) const;
        //   const bdem_Row& theRow(int rowIndex) const;
        // --------------------------------------------------------------------

        DECLARE_MAIN_VARIABLES

        bslma_TestAllocator defaultAllocator;
        const bslma_DefaultAllocatorGuard dag(&defaultAllocator);

        if (verbose) cout << endl
                          << "PRIMARY MANIPULATORS/BASIC ACCESSORS" << endl
                          << "====================================" << endl;

        if (verbose) cout <<
             "\nVerify creators of lists with all 32 element types." << endl;

        if (verbose) cout << "\tloadReferenceA" << endl;
        {
            bdem_List mL;  const bdem_List& L = mL;
                                               ASSERT(0         == L.length());
            loadReferenceA(&mL);               ASSERT(NUM_ELEMS == L.length());

            ASSERT(bdem_ElemType::BDEM_CHAR             == L.elemType( 0));
            ASSERT(bdem_ElemType::BDEM_SHORT            == L.elemType( 1));
            ASSERT(bdem_ElemType::BDEM_INT              == L.elemType( 2));
            ASSERT(bdem_ElemType::BDEM_INT64            == L.elemType( 3));
            ASSERT(bdem_ElemType::BDEM_FLOAT            == L.elemType( 4));
            ASSERT(bdem_ElemType::BDEM_DOUBLE           == L.elemType( 5));
            ASSERT(bdem_ElemType::BDEM_STRING           == L.elemType( 6));
            ASSERT(bdem_ElemType::BDEM_DATETIME         == L.elemType( 7));
            ASSERT(bdem_ElemType::BDEM_DATE             == L.elemType( 8));
            ASSERT(bdem_ElemType::BDEM_TIME             == L.elemType( 9));
            ASSERT(bdem_ElemType::BDEM_CHAR_ARRAY       == L.elemType(10));
            ASSERT(bdem_ElemType::BDEM_SHORT_ARRAY      == L.elemType(11));
            ASSERT(bdem_ElemType::BDEM_INT_ARRAY        == L.elemType(12));
            ASSERT(bdem_ElemType::BDEM_INT64_ARRAY      == L.elemType(13));
            ASSERT(bdem_ElemType::BDEM_FLOAT_ARRAY      == L.elemType(14));
            ASSERT(bdem_ElemType::BDEM_DOUBLE_ARRAY     == L.elemType(15));
            ASSERT(bdem_ElemType::BDEM_STRING_ARRAY     == L.elemType(16));
            ASSERT(bdem_ElemType::BDEM_DATETIME_ARRAY   == L.elemType(17));
            ASSERT(bdem_ElemType::BDEM_DATE_ARRAY       == L.elemType(18));
            ASSERT(bdem_ElemType::BDEM_TIME_ARRAY       == L.elemType(19));
            ASSERT(bdem_ElemType::BDEM_LIST             == L.elemType(20));
            ASSERT(bdem_ElemType::BDEM_TABLE            == L.elemType(21));
            ASSERT(bdem_ElemType::BDEM_BOOL             == L.elemType(22));
            ASSERT(bdem_ElemType::BDEM_DATETIMETZ       == L.elemType(23));
            ASSERT(bdem_ElemType::BDEM_DATETZ           == L.elemType(24));
            ASSERT(bdem_ElemType::BDEM_TIMETZ           == L.elemType(25));
            ASSERT(bdem_ElemType::BDEM_BOOL_ARRAY       == L.elemType(26));
            ASSERT(bdem_ElemType::BDEM_DATETIMETZ_ARRAY == L.elemType(27));
            ASSERT(bdem_ElemType::BDEM_DATETZ_ARRAY     == L.elemType(28));
            ASSERT(bdem_ElemType::BDEM_TIMETZ_ARRAY     == L.elemType(29));

            ASSERT(A00 == L[ 0].theChar());
            ASSERT(A01 == L[ 1].theShort());
            ASSERT(A02 == L[ 2].theInt());
            ASSERT(A03 == L[ 3].theInt64());
            ASSERT(A04 == L[ 4].theFloat());
            ASSERT(A05 == L[ 5].theDouble());
            ASSERT(A06 == L[ 6].theString());
            ASSERT(A07 == L[ 7].theDatetime());
            ASSERT(A08 == L[ 8].theDate());
            ASSERT(A09 == L[ 9].theTime());
            ASSERT(A10 == L[10].theCharArray());
            ASSERT(A11 == L[11].theShortArray());
            ASSERT(A12 == L[12].theIntArray());
            ASSERT(A13 == L[13].theInt64Array());
            ASSERT(A14 == L[14].theFloatArray());
            ASSERT(A15 == L[15].theDoubleArray());
            ASSERT(A16 == L[16].theStringArray());
            ASSERT(A17 == L[17].theDatetimeArray());
            ASSERT(A18 == L[18].theDateArray());
            ASSERT(A19 == L[19].theTimeArray());
            ASSERT(A20 == L[20].theList());
            ASSERT(A21 == L[21].theTable());
            ASSERT(A22 == L[22].theBool());
            ASSERT(A23 == L[23].theDatetimeTz());
            ASSERT(A24 == L[24].theDateTz());
            ASSERT(A25 == L[25].theTimeTz());
            ASSERT(A26 == L[26].theBoolArray());
            ASSERT(A27 == L[27].theDatetimeTzArray());
            ASSERT(A28 == L[28].theDateTzArray());
            ASSERT(A29 == L[29].theTimeTzArray());

            ASSERT(0   == L.isAnyNull());
        }

        if (verbose) cout << "\tloadReferenceB" << endl;
        {
            bdem_List mL;  const bdem_List& L = mL;
                                               ASSERT(0         == L.length());
            loadReferenceB(&mL);               ASSERT(NUM_ELEMS == L.length());

            ASSERT(bdem_ElemType::BDEM_CHAR             == L.elemType( 0));
            ASSERT(bdem_ElemType::BDEM_SHORT            == L.elemType( 1));
            ASSERT(bdem_ElemType::BDEM_INT              == L.elemType( 2));
            ASSERT(bdem_ElemType::BDEM_INT64            == L.elemType( 3));
            ASSERT(bdem_ElemType::BDEM_FLOAT            == L.elemType( 4));
            ASSERT(bdem_ElemType::BDEM_DOUBLE           == L.elemType( 5));
            ASSERT(bdem_ElemType::BDEM_STRING           == L.elemType( 6));
            ASSERT(bdem_ElemType::BDEM_DATETIME         == L.elemType( 7));
            ASSERT(bdem_ElemType::BDEM_DATE             == L.elemType( 8));
            ASSERT(bdem_ElemType::BDEM_TIME             == L.elemType( 9));
            ASSERT(bdem_ElemType::BDEM_CHAR_ARRAY       == L.elemType(10));
            ASSERT(bdem_ElemType::BDEM_SHORT_ARRAY      == L.elemType(11));
            ASSERT(bdem_ElemType::BDEM_INT_ARRAY        == L.elemType(12));
            ASSERT(bdem_ElemType::BDEM_INT64_ARRAY      == L.elemType(13));
            ASSERT(bdem_ElemType::BDEM_FLOAT_ARRAY      == L.elemType(14));
            ASSERT(bdem_ElemType::BDEM_DOUBLE_ARRAY     == L.elemType(15));
            ASSERT(bdem_ElemType::BDEM_STRING_ARRAY     == L.elemType(16));
            ASSERT(bdem_ElemType::BDEM_DATETIME_ARRAY   == L.elemType(17));
            ASSERT(bdem_ElemType::BDEM_DATE_ARRAY       == L.elemType(18));
            ASSERT(bdem_ElemType::BDEM_TIME_ARRAY       == L.elemType(19));
            ASSERT(bdem_ElemType::BDEM_LIST             == L.elemType(20));
            ASSERT(bdem_ElemType::BDEM_TABLE            == L.elemType(21));
            ASSERT(bdem_ElemType::BDEM_BOOL             == L.elemType(22));
            ASSERT(bdem_ElemType::BDEM_DATETIMETZ       == L.elemType(23));
            ASSERT(bdem_ElemType::BDEM_DATETZ           == L.elemType(24));
            ASSERT(bdem_ElemType::BDEM_TIMETZ           == L.elemType(25));
            ASSERT(bdem_ElemType::BDEM_BOOL_ARRAY       == L.elemType(26));
            ASSERT(bdem_ElemType::BDEM_DATETIMETZ_ARRAY == L.elemType(27));
            ASSERT(bdem_ElemType::BDEM_DATETZ_ARRAY     == L.elemType(28));
            ASSERT(bdem_ElemType::BDEM_TIMETZ_ARRAY     == L.elemType(29));

            ASSERT(B00 == L[ 0].theChar());
            ASSERT(B01 == L[ 1].theShort());
            ASSERT(B02 == L[ 2].theInt());
            ASSERT(B03 == L[ 3].theInt64());
            ASSERT(B04 == L[ 4].theFloat());
            ASSERT(B05 == L[ 5].theDouble());
            ASSERT(B06 == L[ 6].theString());
            ASSERT(B07 == L[ 7].theDatetime());
            ASSERT(B08 == L[ 8].theDate());
            ASSERT(B09 == L[ 9].theTime());
            ASSERT(B10 == L[10].theCharArray());
            ASSERT(B11 == L[11].theShortArray());
            ASSERT(B12 == L[12].theIntArray());
            ASSERT(B13 == L[13].theInt64Array());
            ASSERT(B14 == L[14].theFloatArray());
            ASSERT(B15 == L[15].theDoubleArray());
            ASSERT(B16 == L[16].theStringArray());
            ASSERT(B17 == L[17].theDatetimeArray());
            ASSERT(B18 == L[18].theDateArray());
            ASSERT(B19 == L[19].theTimeArray());
            ASSERT(B20 == L[20].theList());
            ASSERT(B21 == L[21].theTable());
            ASSERT(B22 == L[22].theBool());
            ASSERT(B23 == L[23].theDatetimeTz());
            ASSERT(B24 == L[24].theDateTz());
            ASSERT(B25 == L[25].theTimeTz());
            ASSERT(B26 == L[26].theBoolArray());
            ASSERT(B27 == L[27].theDatetimeTzArray());
            ASSERT(B28 == L[28].theDateTzArray());
            ASSERT(B29 == L[29].theTimeTzArray());

            ASSERT(0   == L.isAnyNull());
        }

        if (verbose) cout << "\tloadReferenceC" << endl;
        {
            bdem_List mL;  const bdem_List& L = mL;
                                               ASSERT(0         == L.length());
            loadReferenceC(&mL);               ASSERT(NUM_ELEMS == L.length());

            ASSERT(bdem_ElemType::BDEM_CHAR             == L.elemType( 0));
            ASSERT(bdem_ElemType::BDEM_SHORT            == L.elemType( 1));
            ASSERT(bdem_ElemType::BDEM_INT              == L.elemType( 2));
            ASSERT(bdem_ElemType::BDEM_INT64            == L.elemType( 3));
            ASSERT(bdem_ElemType::BDEM_FLOAT            == L.elemType( 4));
            ASSERT(bdem_ElemType::BDEM_DOUBLE           == L.elemType( 5));
            ASSERT(bdem_ElemType::BDEM_STRING           == L.elemType( 6));
            ASSERT(bdem_ElemType::BDEM_DATETIME         == L.elemType( 7));
            ASSERT(bdem_ElemType::BDEM_DATE             == L.elemType( 8));
            ASSERT(bdem_ElemType::BDEM_TIME             == L.elemType( 9));
            ASSERT(bdem_ElemType::BDEM_CHAR_ARRAY       == L.elemType(10));
            ASSERT(bdem_ElemType::BDEM_SHORT_ARRAY      == L.elemType(11));
            ASSERT(bdem_ElemType::BDEM_INT_ARRAY        == L.elemType(12));
            ASSERT(bdem_ElemType::BDEM_INT64_ARRAY      == L.elemType(13));
            ASSERT(bdem_ElemType::BDEM_FLOAT_ARRAY      == L.elemType(14));
            ASSERT(bdem_ElemType::BDEM_DOUBLE_ARRAY     == L.elemType(15));
            ASSERT(bdem_ElemType::BDEM_STRING_ARRAY     == L.elemType(16));
            ASSERT(bdem_ElemType::BDEM_DATETIME_ARRAY   == L.elemType(17));
            ASSERT(bdem_ElemType::BDEM_DATE_ARRAY       == L.elemType(18));
            ASSERT(bdem_ElemType::BDEM_TIME_ARRAY       == L.elemType(19));
            ASSERT(bdem_ElemType::BDEM_LIST             == L.elemType(20));
            ASSERT(bdem_ElemType::BDEM_TABLE            == L.elemType(21));
            ASSERT(bdem_ElemType::BDEM_BOOL             == L.elemType(22));
            ASSERT(bdem_ElemType::BDEM_DATETIMETZ       == L.elemType(23));
            ASSERT(bdem_ElemType::BDEM_DATETZ           == L.elemType(24));
            ASSERT(bdem_ElemType::BDEM_TIMETZ           == L.elemType(25));
            ASSERT(bdem_ElemType::BDEM_BOOL_ARRAY       == L.elemType(26));
            ASSERT(bdem_ElemType::BDEM_DATETIMETZ_ARRAY == L.elemType(27));
            ASSERT(bdem_ElemType::BDEM_DATETZ_ARRAY     == L.elemType(28));
            ASSERT(bdem_ElemType::BDEM_TIMETZ_ARRAY     == L.elemType(29));

            ASSERT(C00 == L[ 0].theChar());
            ASSERT(C01 == L[ 1].theShort());
            ASSERT(C02 == L[ 2].theInt());
            ASSERT(C03 == L[ 3].theInt64());
            ASSERT(C04 == L[ 4].theFloat());
            ASSERT(C05 == L[ 5].theDouble());
            ASSERT(C06 == L[ 6].theString());
            ASSERT(C07 == L[ 7].theDatetime());
            ASSERT(C08 == L[ 8].theDate());
            ASSERT(C09 == L[ 9].theTime());
            ASSERT(C10 == L[10].theCharArray());
            ASSERT(C11 == L[11].theShortArray());
            ASSERT(C12 == L[12].theIntArray());
            ASSERT(C13 == L[13].theInt64Array());
            ASSERT(C14 == L[14].theFloatArray());
            ASSERT(C15 == L[15].theDoubleArray());
            ASSERT(C16 == L[16].theStringArray());
            ASSERT(C17 == L[17].theDatetimeArray());
            ASSERT(C18 == L[18].theDateArray());
            ASSERT(C19 == L[19].theTimeArray());
            ASSERT(C20 == L[20].theList());
            ASSERT(C21 == L[21].theTable());
            ASSERT(C22 == L[22].theBool());
            ASSERT(C23 == L[23].theDatetimeTz());
            ASSERT(C24 == L[24].theDateTz());
            ASSERT(C25 == L[25].theTimeTz());
            ASSERT(C26 == L[26].theBoolArray());
            ASSERT(C27 == L[27].theDatetimeTzArray());
            ASSERT(C28 == L[28].theDateTzArray());
            ASSERT(C29 == L[29].theTimeTzArray());

            ASSERT(0   == L.isAnyNull());
        }

        if (verbose) cout << "\tloadReferenceD" << endl;
        {
            bdem_List mL;  const bdem_List& L = mL;
                                               ASSERT(0         == L.length());
            loadReferenceD(&mL);               ASSERT(NUM_ELEMS == L.length());

            ASSERT(bdem_ElemType::BDEM_CHAR             == L.elemType( 0));
            ASSERT(bdem_ElemType::BDEM_SHORT            == L.elemType( 1));
            ASSERT(bdem_ElemType::BDEM_INT              == L.elemType( 2));
            ASSERT(bdem_ElemType::BDEM_INT64            == L.elemType( 3));
            ASSERT(bdem_ElemType::BDEM_FLOAT            == L.elemType( 4));
            ASSERT(bdem_ElemType::BDEM_DOUBLE           == L.elemType( 5));
            ASSERT(bdem_ElemType::BDEM_STRING           == L.elemType( 6));
            ASSERT(bdem_ElemType::BDEM_DATETIME         == L.elemType( 7));
            ASSERT(bdem_ElemType::BDEM_DATE             == L.elemType( 8));
            ASSERT(bdem_ElemType::BDEM_TIME             == L.elemType( 9));
            ASSERT(bdem_ElemType::BDEM_CHAR_ARRAY       == L.elemType(10));
            ASSERT(bdem_ElemType::BDEM_SHORT_ARRAY      == L.elemType(11));
            ASSERT(bdem_ElemType::BDEM_INT_ARRAY        == L.elemType(12));
            ASSERT(bdem_ElemType::BDEM_INT64_ARRAY      == L.elemType(13));
            ASSERT(bdem_ElemType::BDEM_FLOAT_ARRAY      == L.elemType(14));
            ASSERT(bdem_ElemType::BDEM_DOUBLE_ARRAY     == L.elemType(15));
            ASSERT(bdem_ElemType::BDEM_STRING_ARRAY     == L.elemType(16));
            ASSERT(bdem_ElemType::BDEM_DATETIME_ARRAY   == L.elemType(17));
            ASSERT(bdem_ElemType::BDEM_DATE_ARRAY       == L.elemType(18));
            ASSERT(bdem_ElemType::BDEM_TIME_ARRAY       == L.elemType(19));
            ASSERT(bdem_ElemType::BDEM_LIST             == L.elemType(20));
            ASSERT(bdem_ElemType::BDEM_TABLE            == L.elemType(21));
            ASSERT(bdem_ElemType::BDEM_BOOL             == L.elemType(22));
            ASSERT(bdem_ElemType::BDEM_DATETIMETZ       == L.elemType(23));
            ASSERT(bdem_ElemType::BDEM_DATETZ           == L.elemType(24));
            ASSERT(bdem_ElemType::BDEM_TIMETZ           == L.elemType(25));
            ASSERT(bdem_ElemType::BDEM_BOOL_ARRAY       == L.elemType(26));
            ASSERT(bdem_ElemType::BDEM_DATETIMETZ_ARRAY == L.elemType(27));
            ASSERT(bdem_ElemType::BDEM_DATETZ_ARRAY     == L.elemType(28));
            ASSERT(bdem_ElemType::BDEM_TIMETZ_ARRAY     == L.elemType(29));

            ASSERT(D00 == L[ 0].theChar());
            ASSERT(D01 == L[ 1].theShort());
            ASSERT(D02 == L[ 2].theInt());
            ASSERT(D03 == L[ 3].theInt64());
            ASSERT(D04 == L[ 4].theFloat());
            ASSERT(D05 == L[ 5].theDouble());
            ASSERT(D06 == L[ 6].theString());
            ASSERT(D07 == L[ 7].theDatetime());
            ASSERT(D08 == L[ 8].theDate());
            ASSERT(D09 == L[ 9].theTime());
            ASSERT(D10 == L[10].theCharArray());
            ASSERT(D11 == L[11].theShortArray());
            ASSERT(D12 == L[12].theIntArray());
            ASSERT(D13 == L[13].theInt64Array());
            ASSERT(D14 == L[14].theFloatArray());
            ASSERT(D15 == L[15].theDoubleArray());
            ASSERT(D16 == L[16].theStringArray());
            ASSERT(D17 == L[17].theDatetimeArray());
            ASSERT(D18 == L[18].theDateArray());
            ASSERT(D19 == L[19].theTimeArray());
            ASSERT(D20 == L[20].theList());
            ASSERT(D21 == L[21].theTable());
            ASSERT(D22 == L[22].theBool());
            ASSERT(D23 == L[23].theDatetimeTz());
            ASSERT(D24 == L[24].theDateTz());
            ASSERT(D25 == L[25].theTimeTz());
            ASSERT(D26 == L[26].theBoolArray());
            ASSERT(D27 == L[27].theDatetimeTzArray());
            ASSERT(D28 == L[28].theDateTzArray());
            ASSERT(D29 == L[29].theTimeTzArray());

            ASSERT(0   == L.isAnyNull());
        }

        if (verbose) cout << "\tloadReferenceE" << endl;
        {
            bdem_List mL;  const bdem_List& L = mL;
                                               ASSERT(0         == L.length());
            loadReferenceE(&mL);               ASSERT(NUM_ELEMS == L.length());

            ASSERT(bdem_ElemType::BDEM_CHAR             == L.elemType( 0));
            ASSERT(bdem_ElemType::BDEM_SHORT            == L.elemType( 1));
            ASSERT(bdem_ElemType::BDEM_INT              == L.elemType( 2));
            ASSERT(bdem_ElemType::BDEM_INT64            == L.elemType( 3));
            ASSERT(bdem_ElemType::BDEM_FLOAT            == L.elemType( 4));
            ASSERT(bdem_ElemType::BDEM_DOUBLE           == L.elemType( 5));
            ASSERT(bdem_ElemType::BDEM_STRING           == L.elemType( 6));
            ASSERT(bdem_ElemType::BDEM_DATETIME         == L.elemType( 7));
            ASSERT(bdem_ElemType::BDEM_DATE             == L.elemType( 8));
            ASSERT(bdem_ElemType::BDEM_TIME             == L.elemType( 9));
            ASSERT(bdem_ElemType::BDEM_CHAR_ARRAY       == L.elemType(10));
            ASSERT(bdem_ElemType::BDEM_SHORT_ARRAY      == L.elemType(11));
            ASSERT(bdem_ElemType::BDEM_INT_ARRAY        == L.elemType(12));
            ASSERT(bdem_ElemType::BDEM_INT64_ARRAY      == L.elemType(13));
            ASSERT(bdem_ElemType::BDEM_FLOAT_ARRAY      == L.elemType(14));
            ASSERT(bdem_ElemType::BDEM_DOUBLE_ARRAY     == L.elemType(15));
            ASSERT(bdem_ElemType::BDEM_STRING_ARRAY     == L.elemType(16));
            ASSERT(bdem_ElemType::BDEM_DATETIME_ARRAY   == L.elemType(17));
            ASSERT(bdem_ElemType::BDEM_DATE_ARRAY       == L.elemType(18));
            ASSERT(bdem_ElemType::BDEM_TIME_ARRAY       == L.elemType(19));
            ASSERT(bdem_ElemType::BDEM_LIST             == L.elemType(20));
            ASSERT(bdem_ElemType::BDEM_TABLE            == L.elemType(21));
            ASSERT(bdem_ElemType::BDEM_BOOL             == L.elemType(22));
            ASSERT(bdem_ElemType::BDEM_DATETIMETZ       == L.elemType(23));
            ASSERT(bdem_ElemType::BDEM_DATETZ           == L.elemType(24));
            ASSERT(bdem_ElemType::BDEM_TIMETZ           == L.elemType(25));
            ASSERT(bdem_ElemType::BDEM_BOOL_ARRAY       == L.elemType(26));
            ASSERT(bdem_ElemType::BDEM_DATETIMETZ_ARRAY == L.elemType(27));
            ASSERT(bdem_ElemType::BDEM_DATETZ_ARRAY     == L.elemType(28));
            ASSERT(bdem_ElemType::BDEM_TIMETZ_ARRAY     == L.elemType(29));

            ASSERT(E00 == L[ 0].theChar());
            ASSERT(E01 == L[ 1].theShort());
            ASSERT(E02 == L[ 2].theInt());
            ASSERT(E03 == L[ 3].theInt64());
            ASSERT(E04 == L[ 4].theFloat());
            ASSERT(E05 == L[ 5].theDouble());
            ASSERT(E06 == L[ 6].theString());
            ASSERT(E07 == L[ 7].theDatetime());
            ASSERT(E08 == L[ 8].theDate());
            ASSERT(E09 == L[ 9].theTime());
            ASSERT(E10 == L[10].theCharArray());
            ASSERT(E11 == L[11].theShortArray());
            ASSERT(E12 == L[12].theIntArray());
            ASSERT(E13 == L[13].theInt64Array());
            ASSERT(E14 == L[14].theFloatArray());
            ASSERT(E15 == L[15].theDoubleArray());
            ASSERT(E16 == L[16].theStringArray());
            ASSERT(E17 == L[17].theDatetimeArray());
            ASSERT(E18 == L[18].theDateArray());
            ASSERT(E19 == L[19].theTimeArray());
            ASSERT(E20 == L[20].theList());
            ASSERT(E21 == L[21].theTable());
            ASSERT(E22 == L[22].theBool());
            ASSERT(E23 == L[23].theDatetimeTz());
            ASSERT(E24 == L[24].theDateTz());
            ASSERT(E25 == L[25].theTimeTz());
            ASSERT(E26 == L[26].theBoolArray());
            ASSERT(E27 == L[27].theDatetimeTzArray());
            ASSERT(E28 == L[28].theDateTzArray());
            ASSERT(E29 == L[29].theTimeTzArray());

            ASSERT(0   == L.isAnyNull());
        }

        if (verbose) cout << "\tloadReferenceU" << endl;
        {
            bdem_List mL;  const bdem_List& L = mL;
                                               ASSERT(0         == L.length());
            loadReferenceU(&mL);               ASSERT(NUM_ELEMS == L.length());

            ASSERT(bdem_ElemType::BDEM_CHAR             == L.elemType( 0));
            ASSERT(bdem_ElemType::BDEM_SHORT            == L.elemType( 1));
            ASSERT(bdem_ElemType::BDEM_INT              == L.elemType( 2));
            ASSERT(bdem_ElemType::BDEM_INT64            == L.elemType( 3));
            ASSERT(bdem_ElemType::BDEM_FLOAT            == L.elemType( 4));
            ASSERT(bdem_ElemType::BDEM_DOUBLE           == L.elemType( 5));
            ASSERT(bdem_ElemType::BDEM_STRING           == L.elemType( 6));
            ASSERT(bdem_ElemType::BDEM_DATETIME         == L.elemType( 7));
            ASSERT(bdem_ElemType::BDEM_DATE             == L.elemType( 8));
            ASSERT(bdem_ElemType::BDEM_TIME             == L.elemType( 9));
            ASSERT(bdem_ElemType::BDEM_CHAR_ARRAY       == L.elemType(10));
            ASSERT(bdem_ElemType::BDEM_SHORT_ARRAY      == L.elemType(11));
            ASSERT(bdem_ElemType::BDEM_INT_ARRAY        == L.elemType(12));
            ASSERT(bdem_ElemType::BDEM_INT64_ARRAY      == L.elemType(13));
            ASSERT(bdem_ElemType::BDEM_FLOAT_ARRAY      == L.elemType(14));
            ASSERT(bdem_ElemType::BDEM_DOUBLE_ARRAY     == L.elemType(15));
            ASSERT(bdem_ElemType::BDEM_STRING_ARRAY     == L.elemType(16));
            ASSERT(bdem_ElemType::BDEM_DATETIME_ARRAY   == L.elemType(17));
            ASSERT(bdem_ElemType::BDEM_DATE_ARRAY       == L.elemType(18));
            ASSERT(bdem_ElemType::BDEM_TIME_ARRAY       == L.elemType(19));
            ASSERT(bdem_ElemType::BDEM_LIST             == L.elemType(20));
            ASSERT(bdem_ElemType::BDEM_TABLE            == L.elemType(21));
            ASSERT(bdem_ElemType::BDEM_BOOL             == L.elemType(22));
            ASSERT(bdem_ElemType::BDEM_DATETIMETZ       == L.elemType(23));
            ASSERT(bdem_ElemType::BDEM_DATETZ           == L.elemType(24));
            ASSERT(bdem_ElemType::BDEM_TIMETZ           == L.elemType(25));
            ASSERT(bdem_ElemType::BDEM_BOOL_ARRAY       == L.elemType(26));
            ASSERT(bdem_ElemType::BDEM_DATETIMETZ_ARRAY == L.elemType(27));
            ASSERT(bdem_ElemType::BDEM_DATETZ_ARRAY     == L.elemType(28));
            ASSERT(bdem_ElemType::BDEM_TIMETZ_ARRAY     == L.elemType(29));

            ASSERT(U00 == L[ 0].theChar());
            ASSERT(U01 == L[ 1].theShort());
            ASSERT(U02 == L[ 2].theInt());
            ASSERT(U03 == L[ 3].theInt64());
            ASSERT(U04 == L[ 4].theFloat());
            ASSERT(U05 == L[ 5].theDouble());
            ASSERT(U06 == L[ 6].theString());
            ASSERT(U07 == L[ 7].theDatetime());
            ASSERT(U08 == L[ 8].theDate());
            ASSERT(U09 == L[ 9].theTime());
            ASSERT(U10 == L[10].theCharArray());
            ASSERT(U11 == L[11].theShortArray());
            ASSERT(U12 == L[12].theIntArray());
            ASSERT(U13 == L[13].theInt64Array());
            ASSERT(U14 == L[14].theFloatArray());
            ASSERT(U15 == L[15].theDoubleArray());
            ASSERT(U16 == L[16].theStringArray());
            ASSERT(U17 == L[17].theDatetimeArray());
            ASSERT(U18 == L[18].theDateArray());
            ASSERT(U19 == L[19].theTimeArray());
            ASSERT(U20 == L[20].theList());
            ASSERT(U21 == L[21].theTable());
            ASSERT(U22 == L[22].theBool());
            ASSERT(U23 == L[23].theDatetimeTz());
            ASSERT(U24 == L[24].theDateTz());
            ASSERT(U25 == L[25].theTimeTz());
            ASSERT(U26 == L[26].theBoolArray());
            ASSERT(U27 == L[27].theDatetimeTzArray());
            ASSERT(U28 == L[28].theDateTzArray());
            ASSERT(U29 == L[29].theTimeTzArray());

            ASSERT(0   == L.isAnyNull());
        }

        if (verbose) cout << "\tloadReferenceN" << endl;
        {
            bdem_List mL;  const bdem_List& L = mL;
                                               ASSERT(0         == L.length());
            loadReferenceN(&mL);               ASSERT(NUM_ELEMS == L.length());

            ASSERT(bdem_ElemType::BDEM_CHAR             == L.elemType( 0));
            ASSERT(bdem_ElemType::BDEM_SHORT            == L.elemType( 1));
            ASSERT(bdem_ElemType::BDEM_INT              == L.elemType( 2));
            ASSERT(bdem_ElemType::BDEM_INT64            == L.elemType( 3));
            ASSERT(bdem_ElemType::BDEM_FLOAT            == L.elemType( 4));
            ASSERT(bdem_ElemType::BDEM_DOUBLE           == L.elemType( 5));
            ASSERT(bdem_ElemType::BDEM_STRING           == L.elemType( 6));
            ASSERT(bdem_ElemType::BDEM_DATETIME         == L.elemType( 7));
            ASSERT(bdem_ElemType::BDEM_DATE             == L.elemType( 8));
            ASSERT(bdem_ElemType::BDEM_TIME             == L.elemType( 9));
            ASSERT(bdem_ElemType::BDEM_CHAR_ARRAY       == L.elemType(10));
            ASSERT(bdem_ElemType::BDEM_SHORT_ARRAY      == L.elemType(11));
            ASSERT(bdem_ElemType::BDEM_INT_ARRAY        == L.elemType(12));
            ASSERT(bdem_ElemType::BDEM_INT64_ARRAY      == L.elemType(13));
            ASSERT(bdem_ElemType::BDEM_FLOAT_ARRAY      == L.elemType(14));
            ASSERT(bdem_ElemType::BDEM_DOUBLE_ARRAY     == L.elemType(15));
            ASSERT(bdem_ElemType::BDEM_STRING_ARRAY     == L.elemType(16));
            ASSERT(bdem_ElemType::BDEM_DATETIME_ARRAY   == L.elemType(17));
            ASSERT(bdem_ElemType::BDEM_DATE_ARRAY       == L.elemType(18));
            ASSERT(bdem_ElemType::BDEM_TIME_ARRAY       == L.elemType(19));
            ASSERT(bdem_ElemType::BDEM_LIST             == L.elemType(20));
            ASSERT(bdem_ElemType::BDEM_TABLE            == L.elemType(21));
            ASSERT(bdem_ElemType::BDEM_BOOL             == L.elemType(22));
            ASSERT(bdem_ElemType::BDEM_DATETIMETZ       == L.elemType(23));
            ASSERT(bdem_ElemType::BDEM_DATETZ           == L.elemType(24));
            ASSERT(bdem_ElemType::BDEM_TIMETZ           == L.elemType(25));
            ASSERT(bdem_ElemType::BDEM_BOOL_ARRAY       == L.elemType(26));
            ASSERT(bdem_ElemType::BDEM_DATETIMETZ_ARRAY == L.elemType(27));
            ASSERT(bdem_ElemType::BDEM_DATETZ_ARRAY     == L.elemType(28));
            ASSERT(bdem_ElemType::BDEM_TIMETZ_ARRAY     == L.elemType(29));

            ASSERT(U00 == L[ 0].theChar());
            ASSERT(U01 == L[ 1].theShort());
            ASSERT(U02 == L[ 2].theInt());
            ASSERT(U03 == L[ 3].theInt64());
            ASSERT(U04 == L[ 4].theFloat());
            ASSERT(U05 == L[ 5].theDouble());
            ASSERT(U06 == L[ 6].theString());
            ASSERT(U07 == L[ 7].theDatetime());
            ASSERT(U08 == L[ 8].theDate());
            ASSERT(U09 == L[ 9].theTime());
            ASSERT(U10 == L[10].theCharArray());
            ASSERT(U11 == L[11].theShortArray());
            ASSERT(U12 == L[12].theIntArray());
            ASSERT(U13 == L[13].theInt64Array());
            ASSERT(U14 == L[14].theFloatArray());
            ASSERT(U15 == L[15].theDoubleArray());
            ASSERT(U16 == L[16].theStringArray());
            ASSERT(U17 == L[17].theDatetimeArray());
            ASSERT(U18 == L[18].theDateArray());
            ASSERT(U19 == L[19].theTimeArray());
            ASSERT(U20 == L[20].theList());
            ASSERT(U21 == L[21].theTable());
            ASSERT(U22 == L[22].theBool());
            ASSERT(U23 == L[23].theDatetimeTz());
            ASSERT(U24 == L[24].theDateTz());
            ASSERT(U25 == L[25].theTimeTz());
            ASSERT(U26 == L[26].theBoolArray());
            ASSERT(U27 == L[27].theDatetimeTzArray());
            ASSERT(U28 == L[28].theDateTzArray());
            ASSERT(U29 == L[29].theTimeTzArray());

            ASSERT(0   == L.isAnyNonNull());
        }

        if (verbose) cout << "\nEmpty table." << endl;
        {
            bdem_Table mT; const bdem_Table& T = mT;
            ASSERT(0 == T.numRows()); ASSERT(0 == T.numColumns());
            mT.appendNullRow();
            ASSERT(1 == T.numRows()); ASSERT(0 == T.numColumns());
            mT.appendNullRow();
            ASSERT(2 == T.numRows()); ASSERT(0 == T.numColumns());
            mT.appendNullRow();
            ASSERT(3 == T.numRows()); ASSERT(0 == T.numColumns());
        }

        if (verbose) cout << "\nNull table." << endl;
        {
            bdem_Table mN; const bdem_Table& N = mN;
            ASSERT(0 == N.numRows()); ASSERT(0 == N.numColumns());
            mN.appendNullRow();
            ASSERT(1 == N.numRows()); ASSERT(0 == N.numColumns());
            mN.appendNullRow();
            ASSERT(2 == N.numRows()); ASSERT(0 == N.numColumns());
            mN.appendNullRow();
            ASSERT(3 == N.numRows()); ASSERT(0 == N.numColumns());
        }

        // The following lists are used throughout the rest of this test case.

        bdem_List mA; const bdem_List& A = mA; ASSERT(0         == A.length());
        loadReferenceA(&mA);                   ASSERT(NUM_ELEMS == A.length());
        bdem_List mB; const bdem_List& B = mB; ASSERT(0         == B.length());
        loadReferenceB(&mB);                   ASSERT(NUM_ELEMS == B.length());
        bdem_List mU; const bdem_List& U = mU; ASSERT(0         == U.length());
        loadReferenceU(&mU);                   ASSERT(NUM_ELEMS == U.length());
        bdem_List mN; const bdem_List& N = mN; ASSERT(0         == N.length());
        loadReferenceN(&mN);                   ASSERT(NUM_ELEMS == N.length());
// TBD
#if 0
        bdem_List mNN; const bdem_List& NN = mNN;
                                              ASSERT(0         == NN.length());
        loadReferenceNull(&mNN);              ASSERT(NUM_ELEMS == NN.length());
#endif

        bsl::vector<bdem_ElemType::Type> elemTypesVec1, elemTypesVec2;
        if (verbose) cout << "\nAll 32 kinds of one-column table." << endl;
        {
            {
                bdem_Table mT; const bdem_Table& T  = mT;
                T.columnTypes(&elemTypesVec1);
                ASSERT(0 == elemTypesVec1.size());
            }

            for (int i = 0; i < NUM_ELEMS; ++i) {
                const bdem_ElemType::Type COLUMN_TYPE = bdem_ElemType::Type(i);
                if (veryVerbose) { T_ P_(i) P(COLUMN_TYPE) }

                bdem_Table mT;  const bdem_Table& T  = mT;
                bdem_Table mT2; const bdem_Table& T2 = mT2;

                LOOP_ASSERT(i, 0 == T.numColumns());
                LOOP_ASSERT(i, 0 == T2.numColumns());

                mT.reset(&COLUMN_TYPE, 1);
                T.columnTypes(&elemTypesVec1);

                bsl::vector<bdem_ElemType::Type> ONE_TYPE_VEC(1, COLUMN_TYPE);
                mT2.reset(ONE_TYPE_VEC);
                T2.columnTypes(&elemTypesVec2);

                LOOP_ASSERT(i, 1 == T.numColumns());
                LOOP_ASSERT(i, 0 == T.numRows());
                LOOP_ASSERT(i, COLUMN_TYPE  == T.columnType(0))
                LOOP_ASSERT(i, COLUMN_TYPE  == elemTypesVec1[0]);

                LOOP_ASSERT(i, 1 == T2.numColumns());
                LOOP_ASSERT(i, 0 == T2.numRows());
                LOOP_ASSERT(i, COLUMN_TYPE  == T2.columnType(0))
                LOOP_ASSERT(i, COLUMN_TYPE  == elemTypesVec2[0]);

                mT.appendNullRow();
                mT2.appendNullRow();

                LOOP_ASSERT(i, 1 == T.numColumns());
                LOOP_ASSERT(i, 1 == T.numRows());
                LOOP3_ASSERT(i, N[i], T.theRow(0)[0], N[i] == T.theRow(0)[0]);
                LOOP3_ASSERT(i, N[i], T[0][0], N[i] == T[0][0]);

                LOOP_ASSERT(i, 1 == T2.numColumns());
                LOOP_ASSERT(i, 1 == T2.numRows());
                LOOP_ASSERT(i, N[i] == T2.theRow(0)[0]);
                LOOP_ASSERT(i, N[i] == T2[0][0]);
//.............................................................................
                if (0 == i) {
                    if (verbose) Q(tbd-Make_Overloaded_ElemRef::op=_Private)
                    // Make sure all versions of operator= in bdem_ElemRef and
                    // bdem_ConstElemRef are private so that an explicit
                    // operator= IS PRIVATE message is printed by the compiler:
#if 0
                    mT.theModifiableRow(0)[0] = A[i];  // should fail to
                                                       // compile with clear
                                                       // msg why
#endif
                }
//.............................................................................

                mT.theModifiableRow(0)[0].replaceValue(A[i]);
                LOOP_ASSERT(i, 1 == T.numColumns());
                LOOP_ASSERT(i, 1 == T.numRows());
                LOOP_ASSERT(i, A[i] == T.theRow(0)[0]);
                LOOP_ASSERT(i, A[i] == T[0][0]);

                mT.appendNullRow();
                LOOP_ASSERT(i, 1 == T.numColumns());
                LOOP_ASSERT(i, 2 == T.numRows());
                LOOP_ASSERT(i, A[i] == T.theRow(0)[0]);
                LOOP_ASSERT(i, N[i] == T.theRow(1)[0]);
                LOOP_ASSERT(i, A[i] == T[0][0]);
                LOOP_ASSERT(i, N[i] == T[1][0]);

                mT.theModifiableRow(1)[0].replaceValue(B[i]);
                LOOP_ASSERT(i, 1 == T.numColumns());
                LOOP_ASSERT(i, 2 == T.numRows());
                LOOP_ASSERT(i, A[i] == T.theRow(0)[0]);
                LOOP_ASSERT(i, B[i] == T.theRow(1)[0]);
                LOOP_ASSERT(i, A[i] == T[0][0]);
                LOOP_ASSERT(i, B[i] == T[1][0]);

                mT.appendNullRow();
                LOOP_ASSERT(i, 1 == T.numColumns());
                LOOP_ASSERT(i, 3 == T.numRows());
                LOOP_ASSERT(i, A[i] == T.theRow(0)[0]);
                LOOP_ASSERT(i, B[i] == T.theRow(1)[0]);
                LOOP_ASSERT(i, N[i] == T.theRow(2)[0]);
                LOOP_ASSERT(i, A[i] == T[0][0]);
                LOOP_ASSERT(i, B[i] == T[1][0]);
                LOOP_ASSERT(i, N[i] == T[2][0]);

                if (veryVeryVerbose) { T_ T_ P(T) }
            }
        }

        if (verbose) cout << "\nAll 32 kinds of one-column table (null)."
                          << endl;
        {
            for (int i = 0; i < NUM_ELEMS; ++i) {
                const bdem_ElemType::Type COLUMN_TYPE = bdem_ElemType::Type(i);
                if (veryVerbose) { T_ P_(i) P(COLUMN_TYPE) }

                bdem_Table mT;  const bdem_Table& T  = mT;
                bdem_Table mT2; const bdem_Table& T2 = mT2;

                LOOP_ASSERT(i, 0 == T.numColumns());
                LOOP_ASSERT(i, 0 == T2.numColumns());

                mT.reset(&COLUMN_TYPE, 1);
                T.columnTypes(&elemTypesVec1);

                bsl::vector<bdem_ElemType::Type> ONE_TYPE_VEC(1, COLUMN_TYPE);
                mT2.reset(ONE_TYPE_VEC);
                T2.columnTypes(&elemTypesVec2);

                LOOP_ASSERT(i, 1 == T.numColumns());
                LOOP_ASSERT(i, 0 == T.numRows());
                LOOP_ASSERT(i, COLUMN_TYPE  == T.columnType(0))
                LOOP_ASSERT(i, COLUMN_TYPE  == elemTypesVec1[0]);

                LOOP_ASSERT(i, 1 == T2.numColumns());
                LOOP_ASSERT(i, 0 == T2.numRows());
                LOOP_ASSERT(i, COLUMN_TYPE  == T2.columnType(0))
                LOOP_ASSERT(i, COLUMN_TYPE  == elemTypesVec2[0]);

                mT.appendNullRow();
                mT2.appendNullRow();

                LOOP_ASSERT(i, 1 == T.numColumns());
                LOOP_ASSERT(i, 1 == T.numRows());
                LOOP_ASSERT(i, N[i] == T.theRow(0)[0]);
                LOOP_ASSERT(i, N[i] == T[0][0]);

                LOOP_ASSERT(i, 1 == T2.numColumns());
                LOOP_ASSERT(i, 1 == T2.numRows());
                LOOP_ASSERT(i, N[i] == T2.theRow(0)[0]);
                LOOP_ASSERT(i, N[i] == T2[0][0]);

                mT.theModifiableRow(0)[0].replaceValue(A[i]);
                LOOP_ASSERT(i, 1 == T.numColumns());
                LOOP_ASSERT(i, 1 == T.numRows());
                LOOP_ASSERT(i, A[i] == T.theRow(0)[0]);
                LOOP_ASSERT(i, A[i] == T[0][0]);

                mT.appendNullRow();
                LOOP_ASSERT(i, 1 == T.numColumns());
                LOOP_ASSERT(i, 2 == T.numRows());
                LOOP_ASSERT(i, A[i] == T.theRow(0)[0]);
                LOOP_ASSERT(i, N[i] == T.theRow(1)[0]);
                LOOP_ASSERT(i, A[i] == T[0][0]);
                LOOP_ASSERT(i, N[i] == T[1][0]);

                mT.theModifiableRow(1)[0].replaceValue(B[i]);
                LOOP_ASSERT(i, 1 == T.numColumns());
                LOOP_ASSERT(i, 2 == T.numRows());
                LOOP_ASSERT(i, A[i] == T.theRow(0)[0]);
                LOOP_ASSERT(i, B[i] == T.theRow(1)[0]);
                LOOP_ASSERT(i, A[i] == T[0][0]);
                LOOP_ASSERT(i, B[i] == T[1][0]);

                mT.appendNullRow();
                LOOP_ASSERT(i, 1 == T.numColumns());
                LOOP_ASSERT(i, 3 == T.numRows());
                LOOP_ASSERT(i, A[i] == T.theRow(0)[0]);
                LOOP_ASSERT(i, B[i] == T.theRow(1)[0]);
                LOOP_ASSERT(i, N[i] == T.theRow(2)[0]);
                LOOP_ASSERT(i, A[i] == T[0][0]);
                LOOP_ASSERT(i, B[i] == T[1][0]);
                LOOP_ASSERT(i, N[i] == T[2][0]);

                if (veryVeryVerbose) { T_ T_ P(T) }
            }
        }

        if (verbose) cout <<
            "\nVerify forward table with all 32 element types." << endl;
        {
            int i;
            vector<bdem_ElemType::Type> columnTypes;
            for (i = 0; i < NUM_ELEMS; ++i) {
                columnTypes.push_back(bdem_ElemType::Type(i));
            }

            if (verbose) cout << "\tCreate table." << endl;
            bdem_Table mT; const bdem_Table& T = mT;
            bdem_Table mT2; const bdem_Table& T2 = mT2;

            ASSERT(0 == T.numRows()); ASSERT(0 == T.numColumns());
            ASSERT(0 == T2.numRows()); ASSERT(0 == T2.numColumns());
            if (veryVeryVerbose) { T_ T_ P(T) P(T2)}

            if (verbose) cout << "\tReset column types." << endl;
            mT.reset(&columnTypes.front(), NUM_ELEMS);
            mT2.reset(columnTypes);

            T.columnTypes(&elemTypesVec1);
            T2.columnTypes(&elemTypesVec2);

            ASSERT(0 == T.numRows());
            ASSERT(NUM_ELEMS == T.numColumns());

            ASSERT(0 == T2.numRows());
            ASSERT(NUM_ELEMS == T2.numColumns());

            for (int i = 0; i < NUM_ELEMS; ++i) {
                LOOP_ASSERT(i, columnTypes[i] == elemTypesVec1[i]);
                LOOP_ASSERT(i, columnTypes[i] == elemTypesVec2[i]);
            }

            if (veryVeryVerbose) { T_ T_ P(T) P(T2)}

            if (verbose) cout << "\tAppend unset row 0." << endl;
            mT.appendNullRow();
            mT2.appendNullRow();
            ASSERT(1 == T.numRows());  ASSERT(NUM_ELEMS == T.numColumns());
            ASSERT(1 == T2.numRows()); ASSERT(NUM_ELEMS == T2.numColumns());
            ASSERT(N.row() == T.theRow(0));
            ASSERT(N.row() == T2.theRow(0));
            ASSERT(N.row() == T[0]);
            ASSERT(N.row() == T2[0]);
            if (veryVeryVerbose) { T_ T_ P(T) P(T2) }

            if (verbose) cout <<
                           "\tAssign each row-0 element an 'A' value." << endl;
            for (i = 0; i < NUM_ELEMS; ++i) {
                mT.theModifiableRow(0)[i].replaceValue(A[i]);
            }
            ASSERT(1 == T.numRows()); ASSERT(NUM_ELEMS == T.numColumns());
            ASSERT(A.row() == T.theRow(0));
            ASSERT(A.row() == T[0]);
            if (veryVeryVerbose) { T_ T_ P(T) }

            if (verbose) cout << "\tAppend unset row 1." << endl;
            mT.appendNullRow();
            ASSERT(2 == T.numRows()); ASSERT(NUM_ELEMS == T.numColumns());
            ASSERT(A.row() == T.theRow(0));
            ASSERT(N.row() == T.theRow(1));
            ASSERT(A.row() == T[0]);
            ASSERT(N.row() == T[1]);
            if (veryVeryVerbose) { T_ T_ P(T) }

            if (verbose) cout <<
                            "\tAssign each row-1 element a 'B' value." << endl;
            for (i = 0; i < NUM_ELEMS; ++i) {
                mT.theModifiableRow(1)[i].replaceValue(B[i]);
            }
            ASSERT(2 == T.numRows()); ASSERT(NUM_ELEMS == T.numColumns());
            ASSERT(A.row() == T.theRow(0));
            ASSERT(B.row() == T.theRow(1));
            ASSERT(A.row() == T[0]);
            ASSERT(B.row() == T[1]);
            if (veryVeryVerbose) { T_ T_ P(T) }

            if (verbose) cout << "\tAppend unset row 2." << endl;
            mT.appendNullRow();
            ASSERT(3 == T.numRows()); ASSERT(NUM_ELEMS == T.numColumns());
            ASSERT(A.row() == T.theRow(0));
            ASSERT(B.row() == T.theRow(1));
            ASSERT(N.row() == T.theRow(2));
            ASSERT(A.row() == T[0]);
            ASSERT(B.row() == T[1]);
            ASSERT(N.row() == T[2]);
            if (veryVeryVerbose) { T_ T_ P(T) }
        }

        if (verbose)
             cout << "\nVerify forward table with all 32 element types (null)."
                  << endl;
        {
            int i;
            vector<bdem_ElemType::Type> columnTypes;
            for (i = 0; i < NUM_ELEMS; ++i) {
                columnTypes.push_back(bdem_ElemType::Type(i));
            }

            if (verbose) cout << "\tCreate table." << endl;
            bdem_Table mT; const bdem_Table& T = mT;
            bdem_Table mT2; const bdem_Table& T2 = mT2;

            ASSERT(0 == T.numRows()); ASSERT(0 == T.numColumns());
            ASSERT(0 == T2.numRows()); ASSERT(0 == T2.numColumns());
            if (veryVeryVerbose) { T_ T_ P(T) P(T2)}

            if (verbose) cout << "\tReset column types." << endl;
            mT.reset(&columnTypes.front(), NUM_ELEMS);
            mT2.reset(columnTypes);

            T.columnTypes(&elemTypesVec1);
            T2.columnTypes(&elemTypesVec2);

            ASSERT(0 == T.numRows());
            ASSERT(NUM_ELEMS == T.numColumns());

            ASSERT(0 == T2.numRows());
            ASSERT(NUM_ELEMS == T2.numColumns());

            for (int i = 0; i < NUM_ELEMS; ++i) {
                LOOP_ASSERT(i, columnTypes[i] == elemTypesVec1[i]);
                LOOP_ASSERT(i, columnTypes[i] == elemTypesVec2[i]);
            }

            if (veryVeryVerbose) { T_ T_ P(T) P(T2)}

            if (verbose) cout << "\tAppend null row 0." << endl;
            mT.appendNullRow();
            mT2.appendNullRow();
            ASSERT(1 == T.numRows());  ASSERT(NUM_ELEMS == T.numColumns());
            ASSERT(1 == T2.numRows()); ASSERT(NUM_ELEMS == T2.numColumns());
            ASSERT(N.row() == T.theRow(0));
            ASSERT(N.row() == T2.theRow(0));
            ASSERT(N.row() == T[0]);
            ASSERT(N.row() == T2[0]);
            if (veryVeryVerbose) { T_ T_ P(T) P(T2) }

            if (verbose) cout <<
                           "\tAssign each row-0 element an 'A' value." << endl;
            for (i = 0; i < NUM_ELEMS; ++i) {
                mT.theModifiableRow(0)[i].replaceValue(A[i]);
            }
            ASSERT(1 == T.numRows()); ASSERT(NUM_ELEMS == T.numColumns());
            ASSERT(A.row() == T.theRow(0));
            ASSERT(A.row() == T[0]);
            if (veryVeryVerbose) { T_ T_ P(T) }

            if (verbose) cout << "\tAppend null row 1." << endl;
            mT.appendNullRow();
            ASSERT(2 == T.numRows()); ASSERT(NUM_ELEMS == T.numColumns());
            ASSERT(A.row() == T.theRow(0));
            ASSERT(N.row() == T.theRow(1));
            ASSERT(A.row() == T[0]);
            ASSERT(N.row() == T[1]);
            if (veryVeryVerbose) { T_ T_ P(T) }

            if (verbose) cout <<
                            "\tAssign each row-1 element a 'B' value." << endl;
            for (i = 0; i < NUM_ELEMS; ++i) {
                mT.theModifiableRow(1)[i].replaceValue(B[i]);
            }
            ASSERT(2 == T.numRows()); ASSERT(NUM_ELEMS == T.numColumns());
            ASSERT(A.row() == T.theRow(0));
            ASSERT(B.row() == T.theRow(1));
            ASSERT(A.row() == T[0]);
            ASSERT(B.row() == T[1]);
            if (veryVeryVerbose) { T_ T_ P(T) }

            if (verbose) cout << "\tAppend null row 2." << endl;
            mT.appendNullRow();
            ASSERT(3 == T.numRows()); ASSERT(NUM_ELEMS == T.numColumns());
            ASSERT(A.row() == T.theRow(0));
            ASSERT(B.row() == T.theRow(1));
            ASSERT(N.row() == T.theRow(2));
            ASSERT(A.row() == T[0]);
            ASSERT(B.row() == T[1]);
            ASSERT(N.row() == T[2]);
            if (veryVeryVerbose) { T_ T_ P(T) }
        }

        if (verbose) cout <<
            "\nVerify reverse table with all 32 element types." << endl;
        {
            if (verbose) cout << "\tSet up reverse reference lists." << endl;

            // List L will be the reverse of A, and list M the reverse of U.

            bdem_List mL; const bdem_List& L = mL;  ASSERT(0 == L.length());
            bdem_List mM; const bdem_List& M = mM;  ASSERT(0 == M.length());
            vector<bdem_ElemType::Type> columnTypes;

            int i;
            for (i = NUM_ELEMS - 1; i >= 0; --i) {
                columnTypes.push_back(bdem_ElemType::Type(i));
                mL.appendElement(A[i]);
                mM.appendElement(N[i]);
            }
            if (veryVeryVerbose) {
                T_ T_ P(L);
                T_ T_ P(M);
            }

            for (i = 0; i < NUM_ELEMS; ++i) {
                LOOP_ASSERT(i, A[NUM_ELEMS - 1 - i] == L[i]);
            }

            if (verbose) cout << "\tCreate table." << endl;
            bdem_Table mT; const bdem_Table& T = mT;
            bdem_Table mT2; const bdem_Table& T2 = mT2;

            ASSERT(0 == T.numRows());
            ASSERT(0 == T.numColumns());

            ASSERT(0 == T2.numRows()); ASSERT(0 == T2.numColumns());
            if (veryVeryVerbose) { T_ T_ P(T) P(T2) }

            if (verbose) cout << "\tReset column types." << endl;
            mT.reset(&columnTypes.front(), NUM_ELEMS);
            mT2.reset(columnTypes);

            T.columnTypes(&elemTypesVec1);
            T2.columnTypes(&elemTypesVec2);

            ASSERT(0 == T.numRows());
            ASSERT(NUM_ELEMS == T.numColumns());

            ASSERT(0 == T2.numRows());
            ASSERT(NUM_ELEMS == T2.numColumns());

            for (int i = 0; i < NUM_ELEMS; ++i) {
                LOOP_ASSERT(i, columnTypes[i] == elemTypesVec1[i]);
                LOOP_ASSERT(i, columnTypes[i] == elemTypesVec2[i]);
            }

            if (veryVeryVerbose) { T_ T_ P(T) P(T2) }

            if (verbose) cout << "\tAppend unset row 0." << endl;
            mT.appendNullRow();
            mT2.appendNullRow();
            ASSERT(1 == T.numRows());  ASSERT(NUM_ELEMS == T.numColumns());
            ASSERT(1 == T2.numRows()); ASSERT(NUM_ELEMS == T2.numColumns());
            ASSERT(M.row() == T.theRow(0));
            ASSERT(M.row() == T2.theRow(0));
            ASSERT(M.row() == T[0]);
            ASSERT(M.row() == T2[0]);
            if (veryVeryVerbose) { T_ T_ P(T) P(T2) }

            if (verbose) cout <<
                           "\tAssign each row-0 element an 'A' value." << endl;
            for (i = 0; i < NUM_ELEMS; ++i) {
                mT.theModifiableRow(0)[i].replaceValue(L[i]);
            }
            ASSERT(1 == T.numRows()); ASSERT(NUM_ELEMS == T.numColumns());
            ASSERT(L.row() == T.theRow(0));
            ASSERT(L.row() == T[0]);
            if (veryVeryVerbose) { T_ T_ P(T) }

            if (verbose) cout << "\tAppend unset row 1." << endl;
            mT.appendNullRow();
            ASSERT(2 == T.numRows()); ASSERT(NUM_ELEMS == T.numColumns());
            ASSERT(L.row() == T.theRow(0));
            ASSERT(M.row() == T.theRow(1));
            ASSERT(L.row() == T[0]);
            ASSERT(M.row() == T[1]);
            if (veryVeryVerbose) { T_ T_ P(T) }

        }

        if (verbose) cout <<
            "\nVerify reverse table with all 32 element types." << endl;
        {
            if (verbose) cout << "\tSet up reverse reference lists (null)."
                              << endl;

            // List L will be the reverse of A, and list M the reverse of N.

            bdem_List mL; const bdem_List& L = mL;  ASSERT(0 == L.length());
            bdem_List mM; const bdem_List& M = mM;  ASSERT(0 == M.length());
            vector<bdem_ElemType::Type> columnTypes;

            int i;
            for (i = NUM_ELEMS - 1; i >= 0; --i) {
                columnTypes.push_back(bdem_ElemType::Type(i));
                mL.appendElement(A[i]);
                mM.appendNullElement(bdem_ElemType::Type(i));
            }
            if (veryVeryVerbose) {
                T_ T_ P(L);
                T_ T_ P(M);
            }

            for (i = 0; i < NUM_ELEMS; ++i) {
                LOOP_ASSERT(i, A[NUM_ELEMS - 1 - i] == L[i]);
            }

            if (verbose) cout << "\tCreate table." << endl;
            bdem_Table mT; const bdem_Table& T = mT;
            bdem_Table mT2; const bdem_Table& T2 = mT2;

            ASSERT(0 == T.numRows());
            ASSERT(0 == T.numColumns());

            ASSERT(0 == T2.numRows()); ASSERT(0 == T2.numColumns());
            if (veryVeryVerbose) { T_ T_ P(T) P(T2) }

            if (verbose) cout << "\tReset column types." << endl;
            mT.reset(&columnTypes.front(), NUM_ELEMS);
            mT2.reset(columnTypes);

            T.columnTypes(&elemTypesVec1);
            T2.columnTypes(&elemTypesVec2);

            ASSERT(0 == T.numRows());
            ASSERT(NUM_ELEMS == T.numColumns());

            ASSERT(0 == T2.numRows());
            ASSERT(NUM_ELEMS == T2.numColumns());

            for (int i = 0; i < NUM_ELEMS; ++i) {
                LOOP_ASSERT(i, columnTypes[i] == elemTypesVec1[i]);
                LOOP_ASSERT(i, columnTypes[i] == elemTypesVec2[i]);
            }

            if (veryVeryVerbose) { T_ T_ P(T) P(T2) }

            if (verbose) cout << "\tAppend Null row 0." << endl;
            mT.appendNullRow();
            mT2.appendNullRow();
            ASSERT(1 == T.numRows());  ASSERT(NUM_ELEMS == T.numColumns());
            ASSERT(1 == T2.numRows()); ASSERT(NUM_ELEMS == T2.numColumns());
            LOOP2_ASSERT(M.row(), T.theRow(0), M.row() == T.theRow(0));
            ASSERT(M.row() == T2.theRow(0));
            ASSERT(M.row() == T2[0]);
            if (veryVeryVerbose) { T_ T_ P(T) P(T2) }

            if (verbose) cout <<
                           "\tAssign each row-0 element an 'A' value." << endl;
            for (i = 0; i < NUM_ELEMS; ++i) {
                mT.theModifiableRow(0)[i].replaceValue(L[i]);
            }
            ASSERT(1 == T.numRows()); ASSERT(NUM_ELEMS == T.numColumns());
            ASSERT(L.row() == T.theRow(0));
            ASSERT(L.row() == T[0]);
            if (veryVeryVerbose) { T_ T_ P(T) }

            if (verbose) cout << "\tAppend null row 1." << endl;
            mT.appendNullRow();
            ASSERT(2 == T.numRows()); ASSERT(NUM_ELEMS == T.numColumns());
            ASSERT(L.row() == T.theRow(0));
            ASSERT(M.row() == T.theRow(1));
            ASSERT(L.row() == T[0]);
            ASSERT(M.row() == T[1]);
            if (veryVeryVerbose) { T_ T_ P(T) }

        }
      }

DEFINE_TEST_CASE(3) {
        // --------------------------------------------------------------------
        // PRIMARY CONSTRUCTOR
        //   Ensure that this constructor is "wired-up" and defaults properly.
        //
        // Concerns:
        //   - That the default allocator comes from 'bslma_Default::allocator'
        //   - That changing the default allocator after construction has
        //      no effect on an existing 'bdem_Table' object.
        //   - That the primary constructor defaults to BDEM_PASS_THROUGH mode.
        //
        // Plan:
        //   - To ensure that the basic constructor takes its allocator by
        //      default from 'bslma_Default::allocator', install a separate
        //      instance of 'bdem_TestAllocator' as the default allocator and
        //      use its instance-specific statistics to verify that it is in
        //      fact the source of default allocations and deallocations.
        //   - To ensure that changing the default allocator has no effect
        //      on existing table objects, swap it out and then continue to
        //      allocate additional objects.
        //   - To ensure that the default mode is 'BDEM_PASS_THROUGH', install
        //      and remove an instance of a long string two times and use the
        //      test allocator's statistics to verify that memory is being
        //      allocated exactly as if 'BDEM_PASS_THROUGH' were supplied.
        //      Also perform the same test on the other three modes to show
        //      that in each of those cases the statistics are different.
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //   - Brute-Force Implementation Technique
        //
        // Testing:
        //   bdem_Table(bslma_Allocator *basicAllocator = 0);
        //
        // --------------------------------------------------------------------

        bslma_TestAllocator defaultAllocator;
        const bslma_DefaultAllocatorGuard dag(&defaultAllocator);

        if (verbose) cout << endl
                          << "PRIMARY CONSTRUCTOR" << endl
                          << "===================" << endl;

        if (verbose) cout <<
            "\nEnsure bdem_Default::allocator() is used by default." << endl;
        {

            bslma_TestAllocator da; // This allocator must be installed as the
                                    // default in order to pass this test.

            if (verbose) cout <<
                       "\tInstall test allocator 'da' as the default." << endl;

            {
                const bslma_DefaultAllocatorGuard dag(&da);

                ASSERT(0 == da.numBlocksTotal());
                bdem_Table x;
                const int NBT = da.numBlocksTotal();
                ASSERT(NBT > 0);

                bslma_TestAllocator ta;
                ASSERT(0 == ta.numBlocksTotal());

                bdem_Table y(&ta);
                ASSERT(NBT == da.numBlocksTotal());
                ASSERT(NBT == ta.numBlocksTotal());

                bslma_TestAllocator oa; // Installing this other allocator
                                        // should have no effect on subsequent
                                        // use of pre-existing list objects.
                if (verbose) cout <<
                       "\tInstall test allocator 'oa' as the default." << endl;
                {
                    const bslma_DefaultAllocatorGuard oag(&oa);

                    ASSERT(0 == oa.numBlocksTotal());
                    bdem_Table z;
                    ASSERT(NBT == oa.numBlocksTotal());

                    ASSERT(NBT == da.numBlocksTotal());
                    ASSERT(NBT == ta.numBlocksTotal());
                    ASSERT(NBT == oa.numBlocksTotal());

                    x.appendNullRow();    // Note: method not yet tested.

                    ASSERT(NBT <  da.numBlocksTotal());
                    ASSERT(NBT == ta.numBlocksTotal());
                    ASSERT(NBT == oa.numBlocksTotal());

                    y.appendNullRow();    // Note: method not yet tested.

                    ASSERT(NBT <  da.numBlocksTotal());
                    ASSERT(NBT <  ta.numBlocksTotal());
                    ASSERT(NBT == oa.numBlocksTotal());

                    z.appendNullRow();    // Note: method not yet tested.

                    ASSERT(NBT <  da.numBlocksTotal());
                    ASSERT(NBT <  ta.numBlocksTotal());
                    ASSERT(NBT <  oa.numBlocksTotal());

                    ASSERT(ta.numBlocksTotal() == da.numBlocksTotal());
                    ASSERT(ta.numBlocksTotal() == oa.numBlocksTotal());
                }

                if (verbose) cout <<
                    "\tUn-install test allocator 'oa' as the default." << endl;

            }
            if (verbose) cout <<
                    "\tUn-install test allocator 'da' as the default." << endl;
        }

        if (verbose) cout << "\nEnsure default mode is BDEM_PASS_THROUGH."
                          << endl;

        bslma_TestAllocator aX, aA, aB, aC, aD;
        {
            const int N0X = aX.numBlocksInUse(), M0X = aX.numBytesInUse();
            const int N0A = aA.numBlocksInUse(), M0A = aA.numBytesInUse();
            const int N0B = aB.numBlocksInUse(), M0B = aB.numBytesInUse();
            const int N0C = aC.numBlocksInUse(), M0C = aC.numBytesInUse();
            const int N0D = aD.numBlocksInUse(), M0D = aD.numBytesInUse();

            if (veryVerbose) {
                T_; T_; P_(N0X); P_(N0A); P_(N0B); P_(N0C); P(N0D);
                T_; T_; P_(M0X); P_(M0A); P_(M0B); P_(M0C); P(M0D);
            }

            if (verbose) cout << "\tCreate five tables." << endl;

            bdem_Table tableX(                                    &aX);
            bdem_Table tableA(bdem_AggregateOption::BDEM_PASS_THROUGH, &aA);
            bdem_Table tableB(bdem_AggregateOption::BDEM_WRITE_ONCE,   &aB);
            bdem_Table tableC(bdem_AggregateOption::BDEM_WRITE_MANY,   &aC);
            //bdem_Table tableD(bdem_AggregateOption::BDEM_SUBORDINATE,&aD);
            bdem_Table tableD(bdem_AggregateOption::BDEM_WRITE_MANY,   &aD);
                                                                       // dummy

            const int N1X = aX.numBlocksInUse(), M1X = aX.numBytesInUse();
            const int N1A = aA.numBlocksInUse(), M1A = aA.numBytesInUse();
            const int N1B = aB.numBlocksInUse(), M1B = aB.numBytesInUse();
            const int N1C = aC.numBlocksInUse(), M1C = aC.numBytesInUse();
            const int N1D = aD.numBlocksInUse(), M1D = aD.numBytesInUse();

            if (veryVerbose) {
                T_; T_; P_(N1X); P_(N1A); P_(N1B); P_(N1C); P(N1D);
                T_; T_; P_(M1X); P_(M1A); P_(M1B); P_(M1C); P(M1D);
                T_; T_; P(tableX); P(tableA); P(tableB); P(tableC); P(tableD);
            }

            if (verbose) cout << "\tReset column types to [ STRING ]." << endl;

            const bdem_ElemType::Type COLUMN_TYPE = bdem_ElemType::BDEM_STRING;

            // Note that we need to use the 'reset' method here,
            // before it has been tested.

            tableX.reset(&COLUMN_TYPE, 1);
            tableA.reset(&COLUMN_TYPE, 1);
            tableB.reset(&COLUMN_TYPE, 1);
            tableC.reset(&COLUMN_TYPE, 1);
            tableD.reset(&COLUMN_TYPE, 1);

            const int N2X = aX.numBlocksInUse(), M2X = aX.numBytesInUse();
            const int N2A = aA.numBlocksInUse(), M2A = aA.numBytesInUse();
            const int N2B = aB.numBlocksInUse(), M2B = aB.numBytesInUse();
            const int N2C = aC.numBlocksInUse(), M2C = aC.numBytesInUse();
            const int N2D = aD.numBlocksInUse(), M2D = aD.numBytesInUse();

            if (veryVerbose) {
                T_; T_; P_(N2X); P_(N2A); P_(N2B); P_(N2C); P(N2D);
                T_; T_; P_(M2X); P_(M2A); P_(M2B); P_(M2C); P(M2D);
                T_; T_; P(tableX); P(tableA); P(tableB); P(tableC); P(tableD);
            }

            const char *const S = "These are the times that try men's soles.";

            if (verbose) cout << "\tAppend a null row." << endl;

            // Note that we need to use the 'appendNullRow' method here,
            // before it has been tested.

            tableX.appendNullRow();
            tableA.appendNullRow();
            tableB.appendNullRow();
            tableC.appendNullRow();
            tableD.appendNullRow();

            const int N3X = aX.numBlocksInUse(), M3X = aX.numBytesInUse();
            const int N3A = aA.numBlocksInUse(), M3A = aA.numBytesInUse();
            const int N3B = aB.numBlocksInUse(), M3B = aB.numBytesInUse();
            const int N3C = aC.numBlocksInUse(), M3C = aC.numBytesInUse();
            const int N3D = aD.numBlocksInUse(), M3D = aD.numBytesInUse();

            if (veryVerbose) {
                T_; T_; P_(N3X); P_(N3A); P_(N3B); P_(N3C); P(N3D);
                T_; T_; P_(M3X); P_(M3A); P_(M3B); P_(M3C); P(M3D);
                T_; T_; P(tableX); P(tableA); P(tableB); P(tableC); P(tableD);
            }

            if (verbose) cout << "\tAssign long STRING element." << endl;

            // Note that we need to use the 'operator[]' method here,
            // before it has been tested.

            tableX.theModifiableRow(0)[0].theModifiableString() = S;
            tableA.theModifiableRow(0)[0].theModifiableString() = S;
            tableB.theModifiableRow(0)[0].theModifiableString() = S;
            tableC.theModifiableRow(0)[0].theModifiableString() = S;
            tableD.theModifiableRow(0)[0].theModifiableString() = S;

            const int N4X = aX.numBlocksInUse(), M4X = aX.numBytesInUse();
            const int N4A = aA.numBlocksInUse(), M4A = aA.numBytesInUse();
            const int N4B = aB.numBlocksInUse(), M4B = aB.numBytesInUse();
            const int N4C = aC.numBlocksInUse(), M4C = aC.numBytesInUse();
            const int N4D = aD.numBlocksInUse(), M4D = aD.numBytesInUse();

            if (veryVerbose) {
                T_; T_; P_(N4X); P_(N4A); P_(N4B); P_(N4C); P(N4D);
                T_; T_; P_(M4X); P_(M4A); P_(M4B); P_(M4C); P(M4D);
                T_; T_; P(tableX); P(tableA); P(tableB); P(tableC); P(tableD);
            }

            if (verbose) cout << "\tRemove that STRING element." << endl;

            // Note that we need to use the 'removeRow' method here,
            // before it has been tested.

            tableX.removeRow(0);
            tableA.removeRow(0);
            tableB.removeRow(0);
            tableC.removeRow(0);
            tableD.removeRow(0);

            const int N5X = aX.numBlocksInUse(), M5X = aX.numBytesInUse();
            const int N5A = aA.numBlocksInUse(), M5A = aA.numBytesInUse();
            const int N5B = aB.numBlocksInUse(), M5B = aB.numBytesInUse();
            const int N5C = aC.numBlocksInUse(), M5C = aC.numBytesInUse();
            const int N5D = aD.numBlocksInUse(), M5D = aD.numBytesInUse();

            if (veryVerbose) {
                T_; T_; P_(N5X); P_(N5A); P_(N5B); P_(N5C); P(N5D);
                T_; T_; P_(M5X); P_(M5A); P_(M5B); P_(M5C); P(M5D);
                T_; T_; P(tableX); P(tableA); P(tableB); P(tableC); P(tableD);
            }

            if (verbose) cout << "\tAppend an unset row again." << endl;

            tableX.appendNullRow();
            tableA.appendNullRow();
            tableB.appendNullRow();
            tableC.appendNullRow();
            tableD.appendNullRow();

            const int N6X = aX.numBlocksInUse(), M6X = aX.numBytesInUse();
            const int N6A = aA.numBlocksInUse(), M6A = aA.numBytesInUse();
            const int N6B = aB.numBlocksInUse(), M6B = aB.numBytesInUse();
            const int N6C = aC.numBlocksInUse(), M6C = aC.numBytesInUse();
            const int N6D = aD.numBlocksInUse(), M6D = aD.numBytesInUse();

            if (veryVerbose) {
                T_; T_; P_(N6X); P_(N6A); P_(N6B); P_(N6C); P(N6D);
                T_; T_; P_(M6X); P_(M6A); P_(M6B); P_(M6C); P(M6D);
                T_; T_; P(tableX); P(tableA); P(tableB); P(tableC); P(tableD);
            }

            if (verbose) cout << "\tAssign long STRING element again." << endl;

            tableX.theModifiableRow(0)[0].theModifiableString() = S;
            tableA.theModifiableRow(0)[0].theModifiableString() = S;
            tableB.theModifiableRow(0)[0].theModifiableString() = S;
            tableC.theModifiableRow(0)[0].theModifiableString() = S;
            tableD.theModifiableRow(0)[0].theModifiableString() = S;

            const int N7X = aX.numBlocksInUse(), M7X = aX.numBytesInUse();
            const int N7A = aA.numBlocksInUse(), M7A = aA.numBytesInUse();
            const int N7B = aB.numBlocksInUse(), M7B = aB.numBytesInUse();
            const int N7C = aC.numBlocksInUse(), M7C = aC.numBytesInUse();
            const int N7D = aD.numBlocksInUse(), M7D = aD.numBytesInUse();

            if (veryVerbose) {
                T_; T_; P_(N7X); P_(N7A); P_(N7B); P_(N7C); P(N7D);
                T_; T_; P_(M7X); P_(M7A); P_(M7B); P_(M7C); P(M7D);
                T_; T_; P(tableX); P(tableA); P(tableB); P(tableC); P(tableD);
            }

            if (verbose) cout << "\tRemove that STRING element again." << endl;

            tableX.removeRow(0);
            tableA.removeRow(0);
            tableB.removeRow(0);
            tableC.removeRow(0);
            tableD.removeRow(0);

            const int N8X = aX.numBlocksInUse(), M8X = aX.numBytesInUse();
            const int N8A = aA.numBlocksInUse(), M8A = aA.numBytesInUse();
            const int N8B = aB.numBlocksInUse(), M8B = aB.numBytesInUse();
            const int N8C = aC.numBlocksInUse(), M8C = aC.numBytesInUse();
            const int N8D = aD.numBlocksInUse(), M8D = aD.numBytesInUse();

            if (veryVerbose) {
                T_; T_; P_(N8X); P_(N8A); P_(N8B); P_(N8C); P(N8D);
                T_; T_; P_(M8X); P_(M8A); P_(M8B); P_(M8C); P(M8D);
                T_; T_; P(tableX); P(tableA); P(tableB); P(tableC); P(tableD);
            }

            if (verbose) cout << "\tEnsure BDEM_PASS_THROUGH's the default."
                              << endl;

            ASSERT(N8A == N8X);         ASSERT(M8A == M8X);
            ASSERT(N8B != N8X);         ASSERT(M8B != M8X);
            ASSERT(N8C != N8X);         ASSERT(M8C != M8X);
            ASSERT(N8D != N8X);         ASSERT(M8D != M8X);
        }

      }

DEFINE_TEST_CASE(2) {
        // --------------------------------------------------------------------
        // TEST APPARATUS
        //   Verify the equipment we have set up to test itself works properly.
        //
        // Concerns:
        //   - That all three distinct instances of each of the 32 element
        //      types was instantiated correctly.
        //   - That all 32 of the Nxx values are respectively "NULL".
        //   - That '==' and !=' for 'bdem_Table' work as expected (and
        //      specifically with respect to the exemplar table elements 'A21',
        //      'B21', and 'U21').  Note that the general functionality has not
        //      yet been tested.
        //   - That gList properly generates correct list objects.
        //      + That all 26 values select the correct list item.
        //      + That common whitespace is ignored { space, tab, newline }.
        //
        // Plan:
        //   - Create all 90 instances at file scope to facilitate reuse.
        //   - Display these values only in 'veryVerbose' mode.
        //   - Use the native operator == to verify that each instance of
        //      a given element type differs from the other two.
        //   - Verify that each N0x scalar holds its respective "null" value.
        //   - Verify that each N1x array has length a length of zero;
        //   - Take extra measures with list and table, as their operator
        //      equals have not yet been verified.
        //   - To ensure gList is working properly,
        //      + Create a list of 26 items and select each unique item out
        //         of it and verify that the lists are the same.
        //      + Create a similar list with WS between each active char.
        //      + Create an empty list (empty spec string); verify length.
        //      + Create another couple of lists and verify them.
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //   - Brute-Force Implementation Technique
        //
        // Testing:
        //   TEST APPARATUS
        //   CONCERN: That operator== and operator!= work properly on tables.
        //   bdem_List gList(const char *spec, const bdem_List& referenceList)
        // --------------------------------------------------------------------

        DECLARE_MAIN_VARIABLES

        bslma_TestAllocator defaultAllocator;
        const bslma_DefaultAllocatorGuard dag(&defaultAllocator);

        if (verbose) cout << endl
                          << "TEST APPARATUS" << endl
                          << "==============" << endl;

        if (verbose) cout <<
                "\nVerify all 90 static element instances." << endl;

        if (veryVerbose) {
            P_(A00); P_(B00); P(U00);   // Scalar values
            P_(A01); P_(B01); P(U01);
            P_(A02); P_(B02); P(U02);
            P_(A03); P_(B03); P(U03);
            P_(A04); P_(B04); P(U04);
            P_(A05); P_(B05); P(U05);
            P_(A06); P_(B06); P(U06);
            P_(A07); P_(B07); P(U07);
            P_(A08); P_(B08); P(U08);
            P_(A09); P_(B09); P(U09);

            P_(A10); P_(B10); P(U10);   // Array values
            P_(A11); P_(B11); P(U11);
            P_(A12); P_(B12); P(U12);
            P_(A13); P_(B13); P(U13);
            P_(A14); P_(B14); P(U14);
            P_(A15); P_(B15); P(U15);
            P_(A16); P_(B16); P(U16);
            P_(A17); P_(B17); P(U17);
            P_(A18); P_(B18); P(U18);
            P_(A19); P_(B19); P(U19);

            P_(A20); P_(B20); P(U20);   // Aggregate values
            P_(A21); P_(B21); P(U21);

            P_(A22); P_(B22); P(U22);
            P_(A23); P_(B23); P(U23);
            P_(A24); P_(B24); P(U24);
            P_(A25); P_(B25); P(U25);
            P_(A26); P_(B26); P(U26);
            P_(A27); P_(B27); P(U27);
            P_(A28); P_(B28); P(U28);
            P_(A29); P_(B29); P(U29);
            P_(A30); P_(B30); P(U30);
            P_(A31); P_(B31); P(U31);
        }

        if (verbose) cout <<
                "\tThat each set of 3 instances has distinct values." << endl;
        ASSERT(A00 != B00); ASSERT(B00 != U00); ASSERT(U00 != A00);
        ASSERT(A01 != B01); ASSERT(B01 != U01); ASSERT(U01 != A01);
        ASSERT(A02 != B02); ASSERT(B02 != U02); ASSERT(U02 != A02);
        ASSERT(A03 != B03); ASSERT(B03 != U03); ASSERT(U03 != A03);
        ASSERT(A04 != B04); ASSERT(B04 != U04); ASSERT(U04 != A04);
        ASSERT(A05 != B05); ASSERT(B05 != U05); ASSERT(U05 != A05);
        ASSERT(A06 != B06); ASSERT(B06 != U06); ASSERT(U06 != A06);
        ASSERT(A07 != B07); ASSERT(B07 != U07); ASSERT(U07 != A07);
        ASSERT(A08 != B08); ASSERT(B08 != U08); ASSERT(U08 != A08);
        ASSERT(A09 != B09); ASSERT(B09 != U09); ASSERT(U09 != A09);

        ASSERT(A10 != B10); ASSERT(B10 != U10); ASSERT(U10 != A10);
        ASSERT(A11 != B11); ASSERT(B11 != U11); ASSERT(U11 != A11);
        ASSERT(A12 != B12); ASSERT(B12 != U12); ASSERT(U12 != A12);
        ASSERT(A13 != B13); ASSERT(B13 != U13); ASSERT(U13 != A13);
        ASSERT(A14 != B14); ASSERT(B14 != U14); ASSERT(U14 != A14);
        ASSERT(A15 != B15); ASSERT(B15 != U15); ASSERT(U15 != A15);
        ASSERT(A16 != B16); ASSERT(B16 != U16); ASSERT(U16 != A16);
        ASSERT(A17 != B17); ASSERT(B17 != U17); ASSERT(U17 != A17);
        ASSERT(A18 != B18); ASSERT(B18 != U18); ASSERT(U18 != A18);
        ASSERT(A19 != B19); ASSERT(B19 != U19); ASSERT(U19 != A19);

        ASSERT(A20 != B20); ASSERT(B20 != U20); ASSERT(U20 != A20);
        ASSERT(A21 != B21); ASSERT(B21 != U21); ASSERT(U21 != A21);

        ASSERT(A22 != B22); ASSERT(B22 == U22); ASSERT(U22 != A22);
        ASSERT(A23 != B23); ASSERT(B23 != U23); ASSERT(U23 != A23);
        ASSERT(A24 != B24); ASSERT(B24 != U24); ASSERT(U24 != A24);
        ASSERT(A25 != B25); ASSERT(B25 != U25); ASSERT(U25 != A25);
        ASSERT(A26 != B26); ASSERT(B26 != U26); ASSERT(U26 != A26);
        ASSERT(A27 != B27); ASSERT(B27 != U27); ASSERT(U27 != A27);
        ASSERT(A28 != B28); ASSERT(B28 != U28); ASSERT(U28 != A28);
        ASSERT(A29 != B29); ASSERT(B29 != U29); ASSERT(U29 != A29);
        ASSERT(A30 != B30); ASSERT(B30 != U30); ASSERT(U30 != A30);
        ASSERT(A31 != B31); ASSERT(B31 != U31); ASSERT(U31 != A31);

        if (verbose) cout <<
                "\tVerify U00 - U09 have unset values." << endl;

        ASSERT(1 == bdetu_Unset<char>::isUnset(U00));
        ASSERT(1 == bdetu_Unset<short>::isUnset(U01));
        ASSERT(1 == bdetu_Unset<int>::isUnset(U02));
        ASSERT(1 == bdetu_Unset<bsls_Types::Int64>::isUnset(U03));
        ASSERT(1 == bdetu_Unset<float>::isUnset(U04));
        ASSERT(1 == bdetu_Unset<double>::isUnset(U05));
        ASSERT(1 == bdetu_Unset<bsl::string>::isUnset(U06));
        ASSERT(1 == bdetu_Unset<bdet_Datetime>::isUnset(U07));
        ASSERT(1 == bdetu_Unset<bdet_Date>::isUnset(U08));
        ASSERT(1 == bdetu_Unset<bdet_Time>::isUnset(U09));

        ASSERT(1 == bdetu_Unset<bool>::isUnset(U22));
        ASSERT(1 == bdetu_Unset<bdet_DatetimeTz>::isUnset(U23));
        ASSERT(1 == bdetu_Unset<bdet_DateTz>::isUnset(U24));
        ASSERT(1 == bdetu_Unset<bdet_TimeTz>::isUnset(U25));

        if (verbose) cout <<
                "\tVerify U10 - U19 have length zero." << endl;

        ASSERT(0 == U10.size());
        ASSERT(0 == U11.size());
        ASSERT(0 == U12.size());
        ASSERT(0 == U13.size());
        ASSERT(0 == U14.size());
        ASSERT(0 == U15.size());
        ASSERT(0 == U16.size());
        ASSERT(0 == U17.size());
        ASSERT(0 == U18.size());
        ASSERT(0 == U19.size());

        ASSERT(0 == U26.size());
        ASSERT(0 == U27.size());
        ASSERT(0 == U28.size());
        ASSERT(0 == U29.size());

        if (verbose) cout <<
                "\tThat table values compare == with self." << endl;

        ASSERT(A21 == A21); ASSERT(B21 == B21); ASSERT(U21 == U21);

        ASSERT(!(A21 == B21)); ASSERT(!(B21 == U21)); ASSERT(!(U21 == A21));

        if (verbose) cout <<
                "\tThat table values are as they should be." << endl;

        if (verbose) cout << "\t\tTable - numRows/numColumns" << endl;
        ASSERT(0 == A21.numRows());     ASSERT(1 == A21.numColumns());
        ASSERT(0 == B21.numRows());     ASSERT(1 == B21.numColumns());
        ASSERT(0 == U21.numRows());     ASSERT(0 == U21.numColumns());

        if (verbose) cout << "\t\tTable - columnType"<<endl;
        ASSERT(bdem_ElemType::BDEM_CHAR == A21.columnType(0));
        ASSERT(bdem_ElemType::BDEM_INT == B21.columnType(0));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout <<
            "\nTest the list generator function ``gList''." << endl;

        if (verbose) cout << "\tCreate a list of 26 unique elements." << endl;

        bdem_List mL;  const bdem_List& L = mL;
                                               ASSERT(0         == L.length());
        loadReferenceA(&mL);                   ASSERT(NUM_ELEMS == L.length());

        if (verbose) cout << "\tCreate an equivalent list using spec." << endl;
        {
            const bdem_List RESULT = gList("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef",
                                           L);
            if (veryVerbose) {
                cout << "\t\tL: ";           L.print(cout, -3, 4);
                cout << "\t\tRESULT: "; RESULT.print(cout, -3, 4);
            }
            ASSERT(L == RESULT);
        }

        if (verbose) cout << "\tRepeat with whitespace." << endl;
        {
            const bdem_List RESULT = gList(
                " A B \t\t C \t\t D E \t\n"
                "  F  G  H  I  J   K   \tL"
                "\t\t\tM\n\n\n\tN\tO  P\tQ"
                " R S T U V W X Y \tZ \t\n"
                " a  \n\t  b c \t\t    d  ef\n", L);
            if (veryVerbose) {
                cout << "\t\tL: ";           L.print(cout, -3, 4);
                cout << "\t\tRESULT: "; RESULT.print(cout, -3, 4);
            }
            ASSERT(L == RESULT);
        }

        if (verbose) cout << "\tCreate an empty list using spec." << endl;
        {
            const bdem_List RESULT = gList("", L);
            if (veryVerbose) {
                cout << "\t\tRESULT: "; RESULT.print(cout, -3, 4);
            }
            ASSERT(0 == RESULT.length())
        }

        if (verbose) cout << "\tCreate a short list using spec." << endl;
        {
            const bdem_List RESULT = gList("dAdA", L);
            if (veryVerbose) {
                cout << "\t\tRESULT: "; RESULT.print(cout, -3, 4);
            }
            ASSERT(4 == RESULT.length())
            ASSERT(A29 == RESULT[0].theTimeTzArray())
            ASSERT(A00 == RESULT[1].theChar())
            ASSERT(A29 == RESULT[0].theTimeTzArray())
            ASSERT(A00 == RESULT[1].theChar())
        }

        if (verbose) cout << "\tCreate another short list using spec." << endl;
        {
            const bdem_List RESULT = gList("JIHBCE", L);
            if (veryVerbose) {
                cout << "\t\tRESULT: "; RESULT.print(cout, -3, 4);
            }
            ASSERT(6 == RESULT.length())
            ASSERT(A09 == RESULT[0].theTime())
            ASSERT(A08 == RESULT[1].theDate())
            ASSERT(A07 == RESULT[2].theDatetime())

            ASSERT(A01 == RESULT[3].theShort())
            ASSERT(A02 == RESULT[4].theInt())
            ASSERT(A04 == RESULT[5].theFloat())
        }

      }

DEFINE_TEST_CASE(1) {
        // --------------------------------------------------------------------
        // BREATHING/USAGE TEST
        //   Note: This test exercises basic functionality, but tests nothing!
        //
        // Concerns:
        //   - That basic essential functionality is superficially operational.
        //   - Provide "Developers' Sandbox".
        //
        // Plan:
        //  Do whatever is needed:
        //   - Try basic append, accessor, and print functionality on tables.
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //   - Brute-Force Implementation Technique
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        bslma_TestAllocator defaultAllocator;
        const bslma_DefaultAllocatorGuard dag(&defaultAllocator);

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        if (verbose) cout << "\tSizeof bdem_Table:"
                          << sizeof(bdem_Table) << bsl::endl;

        if (verbose) cout <<
            "\nMake sure we can create and use a 'bdem_Table'." << endl;
        {
            bslma_TestAllocator da;
            const bslma_DefaultAllocatorGuard dag(&da);

            const bdem_ElemType::Type COLUMN_TYPES[] = {
                bdem_ElemType::BDEM_SHORT,
                bdem_ElemType::BDEM_INT,
            };
            const int NUM_COLUMNS = sizeof COLUMN_TYPES / sizeof *COLUMN_TYPES;

            bslma_TestAllocator a;

            bdem_Table mT(COLUMN_TYPES, NUM_COLUMNS, &a);
            const bdem_Table& T = mT;

            if (verbose) { cout << "\tCreate a table[SHORT INT]: "; P(T) }
            ASSERT(0 == T.numRows());
            ASSERT(2 == T.numColumns());

            mT.appendNullRow();
            if (verbose) { cout << "\tAppend an unset row: "; P(T) }
            ASSERT(1 == T.numRows());
            ASSERT(2 == T.numColumns());
            ASSERT(short(~0x7fff) == T[0][0].theShort());
            ASSERT(int(~0x7fffFFFF) == T[0].theInt(1));

            mT.theModifiableRow(0)[0].theModifiableShort() = -25;
            mT.theModifiableRow(0).theModifiableInt(1) = 123;
            if (verbose) { cout << "\tAssign to row[0] -25 and 123: "; P(T) }
            ASSERT(1 == T.numRows());
            ASSERT(2 == T.numColumns());
            ASSERT(-25 == T[0][0].theShort());
            ASSERT(123 == T[0].theInt(1));

            const bdem_Table T2(T);
            if (verbose) {
                cout << "\tCopy construct T2 from T:\n";
                T_ T_ P(T);
                T_ T_ P(T2);
            }
            ASSERT(T == T2);
            ASSERT(1 == T.numRows());
            ASSERT(2 == T.numColumns());
            ASSERT(-25 == T[0][0].theShort());
            ASSERT(-25 == T[0].theShort(0));
            ASSERT(123 == T[0][1].theInt());
            ASSERT(123 == T[0].theInt(1));

            ASSERT(1 == T2.numRows());
            ASSERT(2 == T2.numColumns());
            ASSERT(-25 == T[0][0].theShort());
            ASSERT(-25 == T[0].theShort(0));
            ASSERT(123 == T[0][1].theInt());
            ASSERT(123 == T[0].theInt(1));

            ASSERT(T == T2);

            if (verbose) cout << "\tCreate an empty list" << endl;

            bdem_List mL; const bdem_List& L = mL;

            ASSERT(T == T2);

            ASSERT(0 == L.length());

            mL.appendTable(T);
            if (verbose) { cout << "\tAppend the above table: "; P(L) }
            ASSERT(1 == L.length());
            ASSERT(bdem_ElemType::BDEM_TABLE == L.elemType(0));
            ASSERT(T == T2);
            ASSERT(T == L[0].theTable());
            ASSERT(T == L.theTable(0));
            ASSERT(T2 == L[0].theTable());
            ASSERT(T2 == L.theTable(0));

            mT.theModifiableRow(0)[0].theModifiableShort() = -35;
            mT.theModifiableRow(0).theModifiableInt(1) = 456;
            if (verbose) {
                cout << "\tAssign to row[0] of original table -35 and 456:\n";
                T_ T_ P(T)
                T_ T_ P(L)
            }
            ASSERT(1 == T.numRows());
            ASSERT(2 == T.numColumns());
            ASSERT(-35 == T[0][0].theShort());
            ASSERT(456 == T[0].theInt(1));

            mL.appendTable(T);
            if (verbose) { cout << "\tAppend the modified table: "; P(L) }
            ASSERT(2 == L.length());
            ASSERT(bdem_ElemType::BDEM_TABLE == L.elemType(0));
            ASSERT(bdem_ElemType::BDEM_TABLE == L.elemType(1));
            ASSERT(T2 == L[0].theTable());
            ASSERT(T2 == L.theTable(0));
            ASSERT(T == L[1].theTable());
            ASSERT(T == L.theTable(1));
        } // default allocator da is un-installed and then destroyed.

      }

#undef DEFINE_TEST_CASE
//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose = argc > 2;
    bool veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

        // **************************************************************
        // INSTALLING A TEST ALLOCATOR AS THE DEFAULT FOR ALL TEST CASES.
        // **************************************************************

    switch (test) { case 0:  // Zero is always the leading case.
    // This macro would be conventional if it were not for the Windows platform
#define CASE(NUMBER)                                                     \
  case NUMBER: testCase##NUMBER(verbose,\
                                veryVerbose,\
                                veryVeryVerbose,\
                                veryVeryVeryVerbose ); break
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
